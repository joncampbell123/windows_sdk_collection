/************************************************************************************************************

  Direct3DRM and DirectSound3D interface designed for the Viewer Sample Application

  (c) 1996 Microsoft Corporation

************************************************************************************************************/


#include "rlds3d.h"
#include "ds3dvi.h"     // "Internal" include stuff, like some structures

// For resource symbols
#include "resource.h"

#include "d3drmwin.h"

#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <direct.h>

#include <objbase.h>
#include <initguid.h>

#include "rodcone.h"
#include "file.h"

#include <mmsystem.h>
#include "dsound.h"
#include "dsutil3d.h"


/*
** This is an interface between the viewer and the RL/DS3D APIs which provides the
**   functionality required by the viewer in simplified form.
**
** Note: This is not an object-oriented API since there should only be need for one
**   copy at a time and this way C++ to C conversions should be fairly easy
**
** DESCRIPTION: The user interfaces the 3D world and objects therein by selecting an item
**   with the mouse (on the screen) and performing operations on that item.  The user will
**   also be able to perform operations on the camera and a few global operations applied
**   to all objects.
**
** USAGE: Create the interface using RLDS3D_Initialize() (returns FALSE if not created)
**   and remove it using RLDS3D_Deinitialize().  The Render() functionality draws the world
**   on the screen.  Other functionality as described.
*/

/*
***********************************  GLOBALS  *************************************
*/

/*
** VIEWER LPDIRECT3DRMFRAME APPDATA STRUCTURE
**
** Pointers to this class are placed (as necessary) in the appdata fields for LPDIRECT3DRMFRAMEs.  Any
**   necessary data to be attached to frames should be added to this class
*/

class FRAMEAPPDATA {
public:
        // Sound buffer interfaces (the standard one and the 3D interface)
        LPDIRECTSOUNDBUFFER Sound;
        LPDIRECTSOUND3DBUFFER i3DSound;

        // Whether or not this frame is currently in orbit (and thus contained in a temporary frame)
        BOOL bOrbiting;
        // Whether or not this frame is a bullet (and thus has a bullet callback and dialog)
        BOOL bBullet;
        // A window handle associated with the orbiting/bullet state which controls speed, etc.
        HWND hDlg;

        FRAMEAPPDATA() : Sound(NULL), i3DSound(NULL), bOrbiting(FALSE), bBullet(FALSE), hDlg(0) {};
        ~FRAMEAPPDATA() { 
                if (i3DSound) i3DSound->Release();
                if (Sound) Sound->Release(); 
        };
};
typedef FRAMEAPPDATA* LPFRAMEAPPDATA;

// Interface to the D3DRM functions
LPDIRECT3DRM lpD3DRM = NULL;

// Pointer to the Directsound API
LPDIRECTSOUND lpDS = NULL;

// Clipper for the DDraw surface
LPDIRECTDRAWCLIPPER lpDDClipper = NULL;

// Handle to the API's parent window (passed in Initialize)
HWND    hwndParent;

// Handle to our instance
HANDLE  hinst;

AppInfo* info;

/*
** Globals used for editing what is in the D3DRM world
*/

// Selected frame (contains visual currently selected by user)
LPDIRECT3DRMFRAME sFrame = NULL;

// Whether or not editing box should be shown around the selected object
BOOL showBoxes = FALSE;

// Currently selected visual (if one is selected)
static LPDIRECT3DRMMESHBUILDER sVisual = NULL;

// Currently selected light (if one is selected)
static LPDIRECT3DRMLIGHT sLight = NULL;

// Box/Speaker cone to go around the selected item (if boxes are on and if there IS a speaker)
static LPDIRECT3DRMMESH selectionBox = NULL;
static LPDIRECT3DRMMESH selectionSpeaker = NULL;

// Clipboard frame and visual allowing cutting and pasting
LPDIRECT3DRMFRAME clipboardFrame = NULL;
LPDIRECT3DRMVISUAL clipboardVisual = NULL;

/*
** Globals for DirectSound3D
*/

// Listener information (connected to the camera)
LPDIRECTSOUND3DLISTENER lp3DListenerInfo;

// Primary Buffer (we don't REALLY need a pointer to it but we keep it just in case)
LPDIRECTSOUNDBUFFER lpDSBuff;

/*
************************  Internal function declarations  ****************************
*/

// These are not part of the interface and are just used by the RLDS3D functions
static BOOL CreateDevice(HWND win, AppInfo* info);
HRESULT loadTextures(char *name, void *arg, LPDIRECT3DRMTEXTURE *tex);
char* LSTRRCHR( const char* lpString, int bChar );
static void PlaceMesh(LPDIRECT3DRMMESHBUILDER mesh, AppInfo *info);
static BOOL CreateScene(AppInfo* info);
static BOOL RebuildDevice(HWND win, AppInfo* info, int width, int height);
static LPDIRECT3DRMMESHBUILDER makeBox(D3DRMBOX*);
void SelectVisual(LPDIRECT3DRMMESHBUILDER visual, LPDIRECT3DRMFRAME frame);
int ChooseNewColor(HWND, D3DCOLOR*);
void RemoveSoundRecord(LPDIRECT3DRMFRAME owner);
void releaseSoundCallback(LPDIRECT3DRMFRAME frame);
void StopOrbiting(LPDIRECT3DRMFRAME stopme);
void UpdateConeVisual(void);

/*
***********************  Error checker functions  ************************************
*/

/*
** These are wrapped around DS or D3DRM calls to appropriately catch errors and handle them.
**
** The generalized cases are coded, callers can pass forced reactions to errors by using the force_critical variables
*/

/*
** In this code the general philosophy for when to use the viewer is when requesting anything from something outside the
**   program.  Thus, asking a frame which we know exists to accept an added visual doesn't get checked, but trying to create
**   a new frame (which tries to allocate memory) is checked in case we've run out of free memory.
*/

BOOL D3DRM_SUCCEED(HRESULT result, int force_critical, char* info) {
        if (result == D3DRM_OK) return TRUE;
        // Could be 0 for non-critical, 1 for non-critical but reports it, or 2 for critical (notify and quit)
        int priority = 0;
        char* error_string;
        switch (result) {
                case D3DRMERR_BADALLOC:                 error_string = "Out of memory";                                                         priority = 2; break;
                case D3DRMERR_BADDEVICE:                error_string = "Device is not compatible with renderer";        priority = 2; break;
                case D3DRMERR_BADFILE:                  error_string = "Data file is corrupt";                                          priority = 2; break;
                case D3DRMERR_BADMAJORVERSION:  error_string = "Bad DLL major version";                                         priority = 2; break;
                case D3DRMERR_BADMINORVERSION:  error_string = "Bad DLL minor version";                                         priority = 2; break;
                case D3DRMERR_BADOBJECT:                error_string = "Object expected in argument";                           priority = 2; break;
                case D3DRMERR_BADTYPE:                  error_string = "Bad argument type passed";                                      priority = 1; break;
                case D3DRMERR_BADVALUE:                 error_string = "Bad argument value passed";                                     priority = 1; break;
                case D3DRMERR_FACEUSED:                 error_string = "Face already used in a mesh";                           priority = 1; break;
                case D3DRMERR_FILENOTFOUND:             error_string = "File cannot be opened";                                         priority = 1; break;
                case D3DRMERR_NOTDONEYET:               error_string = "Unimplemented function called";                         priority = 1; break;
                case D3DRMERR_NOTFOUND:                 error_string = "Object not found in specified place";           priority = 1; break;
                case D3DRMERR_UNABLETOEXECUTE:  error_string = "Unable to carry out procedure";                         priority = 2; break;
                default:                                                error_string = "D3DRM Error: Unable to continue";                       priority = 2; break;
        }

        int ret;
        if (force_critical >= 0) priority = force_critical;
        if (priority == 1) {
            ret = MessageBox(hwndParent, error_string, "D3DRM Warning", MB_APPLMODAL|MB_ICONWARNING|MB_OK);
        }
        else if (priority == 2) {
            ret = MessageBox(hwndParent, error_string, "D3DRM Fatal Error", MB_APPLMODAL|MB_ICONSTOP|MB_OK);
                PostMessage(hwndParent, WM_CLOSE,0,0);
        }
        return FALSE;
}

/*
** DS isn't vital for the viewer to run so error messages don't force a quit
*/

BOOL DS3D_SUCCEED(HRESULT result, int force_critical, char* info) {
        if (result == DS_OK) return TRUE;
        // Could be 0 for no message, 1 for non-critical (simply needs to report it), or 2 for critical (notify and quit)
        int priority = 0;
        char* error_string = NULL;
        switch (result) {
                case DSERR_ALLOCATED:                   error_string = "Requested resources already in use";                    priority = 1; break;
                case DSERR_ALREADYINITIALIZED:  error_string = "Object already initialized";                                    priority = 0; break;
                case DSERR_BADFORMAT:                   error_string = "Wave format not supported";                                             priority = 0; break;
                case DSERR_BUFFERLOST:                  error_string = "Buffer lost and must be restored";                              priority = 0; break;
                case DSERR_CONTROLUNAVAIL:              error_string = "Control requested not available";                               priority = 0; break;
                case DSERR_GENERIC:                             error_string = "Undetermined error";                                                    priority = 1; break;
                case DSERR_INVALIDCALL:                 error_string = "Invalid call for object's current state";               priority = 0; break;
                case DSERR_INVALIDPARAM:                error_string = "Invalid parameters passed to object";                   priority = 0; break;
                case DSERR_NOAGGREGATION:               error_string = "Object does not support aggregation";                   priority = 1; break;
                case DSERR_NODRIVER:                    error_string = "No sound driver available";                                             priority = 1; break;
//              case DSERR_NOINTERFACE:                 error_string = "Requested COM interface not available";                 priority = 0; break;
                case DSERR_OUTOFMEMORY:                 error_string = "Out of memory";                                                                 priority = 1; break;
                case DSERR_PRIOLEVELNEEDED:             error_string = "Caller does not have required priority level";  priority = 0; break;
//              case DSERR_UNINITIALIZED:               error_string = "DirectSound not initialized";                                   priority = 1; break;
                case DSERR_UNSUPPORTED:                 error_string = "Unsupported function called";                                   priority = 0; break;
                default:                                                error_string = "Undetermined error";                                                    priority = 1; break;
        }

        int ret;
        if (force_critical >= 0) priority = force_critical;
        if (priority == 1) {
            ret = MessageBox(hwndParent, error_string, "DS3D Warning", MB_APPLMODAL|MB_ICONWARNING|MB_OK);
        }
        else if (priority == 2) {
            ret = MessageBox(hwndParent, error_string, "DS3D Fatal Error", MB_APPLMODAL|MB_ICONSTOP|MB_OK);
                PostMessage(hwndParent, WM_CLOSE,0,0);
        }
        return FALSE;
}




/*
************************  INITIALIZATION/DEINITIALIZATION ****************************
*/

/*
** Initialize will attach itself to the passed window.  Call initialize after
**  getting a handle for your window but before you display it.
**
** Initialize will initialize the D3DRM API and return false if it fails.  It will
**  also attempt to initialize a DirectSound3D API, but failing this does not
**  justify a failed Initialize (since the 3D sound isn't a necessary part of
**  the viewer)
*/

BOOL RLDS3D_Initialize(HWND hwndPW, HANDLE this_inst) {

        if (!hwndPW) return FALSE;

        hinst = this_inst;
        hwndParent = hwndPW;

        // D3DRM API INIT - we force non-critical warnings here because the initialization returns a boolean for success
        
        // Call Direct3DRMCreate to try to make a D3DRM object
        if (!D3DRM_SUCCEED(Direct3DRMCreate(&lpD3DRM), 1)) return FALSE;


                if (!D3DRM_SUCCEED(DirectDrawCreateClipper(0, &lpDDClipper, NULL),1)) {
                                        lpD3DRM->Release();
                        return FALSE;
                }
    
        if (!D3DRM_SUCCEED(lpDDClipper->SetHWnd(0, hwndParent),1)) {
                lpDDClipper->Release();
                lpD3DRM->Release();
        return FALSE;
    }


    // Set up D3DRM information structure
    info = (AppInfo*) malloc(sizeof(AppInfo));
        if (!info) {
                lpDDClipper->Release();
                lpD3DRM->Release();
                return FALSE;
        }

    info->model = D3DCOLOR_MONO;

    // Calls our internal function to create a viewport and device and attach it to the window (CreateDevice in misc. functionality)
        // (also fills in the global information structure "info" for what D3DRM can do and is currently doing)
        if (CreateDevice(hwndParent, info) == FALSE) {
                lpDDClipper->Release();
                lpD3DRM->Release();
                return FALSE;
        }
        
        // DIRECTSOUND 3D INIT - note, if this fails the entire initialization is still OK.

        // Description for our primary buffer creation
        DSBUFFERDESC dsbd;

    int ret = IDRETRY;

        // Try to create the Directsound objects until we either do it, are told to ignore it, or are told to abort
        while (ret == IDRETRY) {
                // Create the directsound object
                if (DS3D_SUCCEED(DirectSoundCreate(NULL, &lpDS, NULL))) {
                        // Set cooperative level
                        if (DS3D_SUCCEED(lpDS->SetCooperativeLevel(hwndParent, DSSCL_PRIORITY))) {
                                // Create a primary buffer so we can query for a 3D Listener interface
                                memset(&dsbd, 0, sizeof(DSBUFFERDESC));
                                dsbd.dwSize = sizeof(DSBUFFERDESC);
                                dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D;
                                if (DS3D_SUCCEED(lpDS->CreateSoundBuffer(&dsbd, &lpDSBuff, NULL))) {
                                        
                                        // Make the primary 44.1 KHz so that it sounds better
                                        WAVEFORMATEX wfx;
                                        wfx.wFormatTag = WAVE_FORMAT_PCM;
                                        wfx.nChannels = 2;
                                        wfx.nSamplesPerSec = 44100;
                                        wfx.nAvgBytesPerSec = 44100*2*2;
                                        wfx.nBlockAlign = 4;
                                        wfx.wBitsPerSample = 16;
                                        wfx.cbSize = 0;
                                        lpDSBuff->SetFormat(&wfx);

                                        // Get the 3D listener information (error currently ignored)
                                        if (DS3D_SUCCEED(lpDSBuff->QueryInterface(IID_IDirectSound3DListener, (void**) &lp3DListenerInfo))) {
                                                lp3DListenerInfo->SetDopplerFactor(D3DVAL(100.0), DS3D_IMMEDIATE);
                                        }
                                        else {
                                                // Failed to get listener info
                                                lpDSBuff->Release();
                                                lpDS->Release();
                                                lpDS = NULL;
                                        }
                                }
                                else {
                                        // Failed to create a primary buffer
                                        lpDS->Release();
                                        lpDS = NULL;
                                }
                        }
                        else {
                                // Failed to set cooperative level
                                lpDS->Release();
                                lpDS = NULL;
                        }
                }

                // Warn that we could create the DirectSound object
                if (!lpDS) {
                        ret = MessageBox(hwndParent, "DirectSound 3D could not initialize", "Warning", MB_APPLMODAL|MB_ICONWARNING|MB_ABORTRETRYIGNORE);
                        if (ret == IDABORT) {
                                lpDDClipper->Release();
                                lpD3DRM->Release();
                                return FALSE;
                            }
                }
                else ret = IDOK;
        }
        return TRUE;
}

/*
** Deinitializes
*/

void RLDS3D_Deinitialize() {
        // Releases the DSound interface... this is very important!
        if (lpDS != NULL) {
                lpDSBuff->Release();
                lpDS->Release();
        }
        info->dev->Release();
        lpDDClipper->Release();
        lpD3DRM->Release();
        free(info);
}

/*
********************************************  ADDING/REMOVING/EDITING OBJECTS  *********************************
*/

/*
** Loads XOF file into the RL world (with textures)
*/

void RLDS3D_LoadXOF(char* file) {
    if (!file) return;
        LPDIRECT3DRMMESHBUILDER builder = NULL;
        if (D3DRM_SUCCEED(lpD3DRM->CreateMeshBuilder(&builder))) {
                if (builder->Load(file, NULL, D3DRMLOAD_FROMFILE, loadTextures, NULL) != D3DRM_OK) {
            MessageBox(hwndParent, "Unable to load file", "D3DRM Fatal Error", MB_APPLMODAL|MB_ICONEXCLAMATION|MB_OK);
                        builder->Release();
                        return;
                }
                PlaceMesh(builder, info);
                builder->Release();
        }
}

/*
** Sets/Gets whether or not boxes are shown around selected item
*/

BOOL RLDS3D_GetBoxes(void) {
        return showBoxes;
}

void RLDS3D_SetBoxes(BOOL new_val) {
        showBoxes = new_val;
        // Re-selects the currently selected visual so that the box is generated/destroyed around it
        RLDS3D_UpdateSelectionBox();
}

/*
** Updates the bounding box around the selected visual (this could be done using a render callback function to compare the
**   frame's scaling and transform functions instead)
*/

void RLDS3D_UpdateSelectionBox(void) {
        // When we select this visual, we destroy any existing box and create a new one around it if showBoxes is true.
        SelectVisual(sVisual, sFrame);
}

/*
** Deselects the currently selected 3D visual.
*/

void RLDS3D_DeselectVisual()
{
        // Removes the bounding box from around it if it's there
    if (sFrame && selectionBox) {
        sFrame->DeleteVisual(selectionBox);
        sFrame->DeleteVisual(selectionSpeaker);
    }
                
        
    sFrame = NULL;
    sLight = NULL;
    sVisual = NULL;
    selectionBox = NULL;
    selectionSpeaker = NULL;
}

/*
** Given coordinates it selects the first visual under those coordinates in the window's viewport
*/

void RLDS3D_FindAndSelectVisual(int x, int y, LPBOOL changed) {
    LPDIRECT3DRMVISUAL visual;
    LPDIRECT3DRMFRAME frame;
    LPDIRECT3DRMPICKEDARRAY picked;
    LPDIRECT3DRMFRAMEARRAY frames;
    LPDIRECT3DRMMESHBUILDER mesh;
    LPDIRECT3DRMVIEWPORT view = info->view;

        LPDIRECT3DRMFRAME oldframe = sFrame;
        
    /*
     * Make sure we don't try to select the selection box of the current
     * selection.
     */
    RLDS3D_DeselectVisual();

    view->Pick(x, y, &picked);
    if (picked)
    {   if (picked->GetSize())
        {
                        // Get the top-level visual
            picked->GetPick(0, &visual, &frames, 0);
            // The frames that contain the visual are placed into a framearray in heiarchical order, take the
            //   last one (the one most closely associated with the visual)
            frames->GetElement(frames->GetSize() - 1, &frame);
            // We can only select meshes so we query the visual to make sure it is one
            if (D3DRM_SUCCEED(visual->QueryInterface(IID_IDirect3DRMMeshBuilder, (void **) &mesh), 0))
            {   
                // If we're clicking on an orbiting frame then we need to stop it.
                StopOrbiting(frame);
                SelectVisual(mesh, frame);
                mesh->Release();
            }
            frame->Release();
            frames->Release();
            visual->Release();
        }
        picked->Release();
    }
        if (changed) {
                if (sFrame == oldframe) *changed = FALSE; else *changed = TRUE;
        }
}

/*
** Cuts the current selection to the clipboard
*/

void RLDS3D_CutVisual()
{
    LPDIRECT3DRMFRAME frame;

    if (clipboardFrame)
    clipboardFrame->Release();

    if (sFrame)
    {   clipboardFrame = sFrame;
        clipboardVisual = sVisual;
                
        // If a 3D sound is attached, remove it (sounds not carried to clipboard)
                RemoveSoundRecord(clipboardFrame);
                RLDS3D_DeselectVisual();

        clipboardFrame->AddRef();
        clipboardFrame->GetParent(&frame);
        if (frame) {
            frame->DeleteChild(clipboardFrame);
            frame->Release();
        }
    }
}

/*
** Copies the current selection to the clipboard
*/

void RLDS3D_CopyVisual()
{
    LPDIRECT3DRMFRAME frame;

    if (sFrame)
    {
                // Things could really foul up if the clones aren't created so we make sure they are
        if (!D3DRM_SUCCEED(sFrame->Clone(0, IID_IDirect3DRMFrame, (void **) &clipboardFrame))) {
                        // If we've failed, we must make sure the clipboard is empty!
                        clipboardVisual->Release();
                        clipboardVisual = NULL;
                        return;
                }

        if (!D3DRM_SUCCEED(sVisual->Clone(0, IID_IDirect3DRMVisual, (void **) &clipboardVisual))) {
                        // If we've failed, we must make sure the clipboard is empty!
                        clipboardFrame->Release();
                        clipboardFrame = NULL;
                        return;
                }

                // If a 3D sound is attached, remove it (sounds not carried to clipboard)
                RemoveSoundRecord(clipboardFrame);

        clipboardFrame->AddVisual(clipboardVisual);
                clipboardVisual->Release();

        clipboardFrame->GetParent(&frame);
        if (frame) {
                        frame->DeleteChild(clipboardFrame);
                        frame->Release();
                }
    }
}

/*
** Pastes the current selection to the window
*/

void RLDS3D_PasteVisual()
{
    if (clipboardFrame)
    {
        LPDIRECT3DRMFRAME frame;
        LPDIRECT3DRMVISUAL visual;

        if (!D3DRM_SUCCEED(clipboardFrame->Clone(0, IID_IDirect3DRMFrame, (void **) &frame))) return;
        if (!D3DRM_SUCCEED(clipboardVisual->Clone(0, IID_IDirect3DRMVisual, (void **) &visual))) {
                        frame->Release();
                        return;
                }

        frame->AddVisual(visual);
        info->scene->AddChild(frame);
                visual->Release();
                frame->Release();
    }
}

/*
** Deletes the current selection from the world without copying to the clipboard
*/

void RLDS3D_DeleteVisual()
{
    if (sFrame) {
        LPDIRECT3DRMFRAME parent, frame;
        // Make a copy of the selected frame ('cause deselecting it will make it inaccessible)
        frame = sFrame;
        // If a 3D sound is attached, remove it
        RemoveSoundRecord(frame);

        // Deselect the frame (removes bounding box, etc, also sets sFrame to NULL)
        RLDS3D_DeselectVisual();        
        if (frame->GetAppData()) delete (FRAMEAPPDATA*)frame->GetAppData();
                frame->GetParent(&parent);
        if (parent) {
                parent->DeleteChild(frame);
                parent->Release();
        }
    }
}

/*
** Add a directional light
*/

void RLDS3D_AddDirectionalLight() {
    LPDIRECT3DRMMESHBUILDER builder;
    LPDIRECT3DRMLIGHT light;
    LPDIRECT3DRMFRAME frame;

    if (D3DRM_SUCCEED(lpD3DRM->CreateMeshBuilder(&builder))) {
                if (D3DRM_SUCCEED(builder->Load("camera.x", NULL, D3DRMLOAD_FROMFILE, NULL, NULL))) {
                        builder->SetQuality(D3DRMRENDER_UNLITFLAT);
                        if (D3DRM_SUCCEED(lpD3DRM->CreateLightRGB(D3DRMLIGHT_DIRECTIONAL, D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), &light))) {
                                if (D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &frame))) {
                                        frame->SetPosition(info->camera, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0));
                                        frame->AddVisual(builder);
                                        frame->AddLight(light);
                                        frame->Release();
                                }
                                light->Release();
                        }
                }
                builder->Release();
        }
}

/*
** Add a parallel point light
*/

void RLDS3D_AddParallelPointLight() {
    LPDIRECT3DRMMESHBUILDER builder;
    LPDIRECT3DRMLIGHT light;
    LPDIRECT3DRMFRAME frame;

    if (D3DRM_SUCCEED(lpD3DRM->CreateMeshBuilder(&builder))) {
            if (D3DRM_SUCCEED(builder->Load("sphere2.x", NULL, D3DRMLOAD_FROMFILE, NULL, NULL))) {
                        builder->SetQuality(D3DRMRENDER_UNLITFLAT);
                        builder->Scale(D3DVAL(0.2), D3DVAL(0.2), D3DVAL(0.2));
                        if (D3DRM_SUCCEED(lpD3DRM->CreateLightRGB(D3DRMLIGHT_PARALLELPOINT, D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), &light))) {
                                if (D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &frame))) {
                                        frame->SetPosition(info->camera, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0));
                                        frame->AddVisual(builder);
                                        frame->AddLight(light);
                                        frame->Release();
                                }
                                light->Release();
                        }
                }
                builder->Release();
        }
}

/*
** Add Point Light
*/

void RLDS3D_AddPointLight() {
    LPDIRECT3DRMMESHBUILDER builder;
    LPDIRECT3DRMLIGHT light;
    LPDIRECT3DRMFRAME frame;

    if (D3DRM_SUCCEED(lpD3DRM->CreateMeshBuilder(&builder))) {
                if (D3DRM_SUCCEED(builder->Load("sphere2.x", NULL, D3DRMLOAD_FROMFILE, NULL, NULL))) {
                        builder->SetQuality(D3DRMRENDER_UNLITFLAT);
                        builder->Scale(D3DVAL(0.2), D3DVAL(0.2), D3DVAL(0.2));
                        if (D3DRM_SUCCEED(lpD3DRM->CreateLightRGB(D3DRMLIGHT_POINT, D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), &light))) {
                                if (D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &frame))) {
                                        frame->SetPosition(info->camera, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0));
                                        frame->AddVisual(builder);
                                        frame->AddLight(light);
                                        frame->Release();
                                }
                                light->Release();
                        }
                }
                builder->Release();
        }
}

/*
** Add a spotlight
*/

void RLDS3D_AddSpotlight() {
    LPDIRECT3DRMMESHBUILDER builder;
    LPDIRECT3DRMLIGHT light;
    LPDIRECT3DRMFRAME frame;

        if (D3DRM_SUCCEED(lpD3DRM->CreateMeshBuilder(&builder))) {
            if (D3DRM_SUCCEED(builder->Load("camera.x", NULL, D3DRMLOAD_FROMFILE, NULL, NULL))) {
                        builder->SetQuality(D3DRMRENDER_UNLITFLAT);
                        if (D3DRM_SUCCEED(lpD3DRM->CreateLightRGB(D3DRMLIGHT_SPOT, D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), &light))) {
                                if (D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &frame))) {
                                        frame->SetPosition(info->camera, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(10.0));
                                        frame->AddVisual(builder);
                                        frame->AddLight(light);
                                        frame->Release();
                                }
                                light->Release();
                        }
                }
                builder->Release();
        }
}


/*
***********************************  OBJECT MOTION/SCALING/COLORING  ****************************************
*/

/*
** Chooses a new color and sets the selected object to it... handles the selection of the color, etc.
*/

void RLDS3D_SetSelColour(void) {
    if (!sFrame) return;
    LPDIRECT3DRMMESHBUILDER mesh;

    if (!D3DRM_SUCCEED(sVisual->QueryInterface(IID_IDirect3DRMMeshBuilder, (void**) &mesh),0)) return;

    if (sLight)
    {
        D3DCOLOR c = sLight->GetColor();

        if (ChooseNewColor(hwndParent, &c)) {
            mesh->SetColor(c);
            sLight->SetColor(c);
        }
    } else {
        D3DCOLOR c;

        if (mesh->GetFaceCount()) {
            LPDIRECT3DRMFACEARRAY faces;
            LPDIRECT3DRMFACE face;
            mesh->GetFaces(&faces);
            faces->GetElement(0, &face);
            c = face->GetColor();
            face->Release();
            faces->Release();
        } else
            c = D3DRMCreateColorRGB(D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0));

        if (ChooseNewColor(hwndParent, &c))
            mesh->SetColor(c);
    }

    mesh->Release();
}


/*
** Moves the camera relative to itself by providing scalars to multiply against the CAMERA-RELATIVE unit vectors
**   forwards/up/right.
*/

void RLDS3D_SetCamVelRelToCam(D3DVALUE forward, D3DVALUE up, D3DVALUE right) {
        D3DVECTOR vDir, vUp, vRight;
        info->camera->GetOrientation(info->scene, &vDir, &vUp);
        // Cross the UP vector and the RIGHT vector to get the FORWARDS vector
        D3DRMVectorCrossProduct(&vRight, &vUp, &vDir);
        info->camera->SetVelocity(info->scene, vDir.x*forward + vUp.x*up + vRight.x*right,
                                   vDir.y*forward + vUp.y*up + vRight.y*right,
                                   vDir.z*forward + vUp.z*up + vRight.z*right,
                                                                   TRUE);
}

/*
** Rotates the camera around its three axis
**
** forward_axis is roll, up_axis is yaw, right_axis is pitch
*/

void RLDS3D_SetCamRotForward(D3DVALUE forward_axis) {
        info->camera->SetRotation(info->camera, 0.0f, 0.0f, 1.0f, forward_axis);
}

void RLDS3D_SetCamRotUp(D3DVALUE up_axis) {
        info->camera->SetRotation(info->camera, 0.0f, 1.0f, 0.0f, up_axis);
}

void RLDS3D_SetCamRotRight(D3DVALUE right_axis) {
        info->camera->SetRotation(info->camera, 1.0f, 0.0f, 0.0f, right_axis);
}

/*
** Scales the currently selected object by the scale factors...
*/

void RLDS3D_ScaleSelected(D3DVALUE sx, D3DVALUE sy, D3DVALUE sz) {
        if (!sFrame) return;
        if (!sVisual) return;
        RLDS3D_StopOrbitSelected();
        sVisual->Scale(sx, sy, sz);
        RLDS3D_UpdateSelectionBox();
}

/*
** Moves the currently selected object in the 3D world relative to the camera
*/

void RLDS3D_SetSelectedVelRelToCam(D3DVALUE forward, D3DVALUE up, D3DVALUE right) {
        if (!sFrame) return;
        RLDS3D_StopOrbitSelected();
        D3DVECTOR vDir, vUp, vRight;
        info->camera->GetOrientation(info->scene, &vDir, &vUp);
        D3DRMVectorCrossProduct(&vRight, &vUp, &vDir);
        sFrame->SetVelocity(info->scene, vDir.x*forward + vUp.x*up + vRight.x*right,
                                                           vDir.y*forward + vUp.y*up + vRight.y*right,
                                                           vDir.z*forward + vUp.z*up + vRight.z*right,
                                                           TRUE);
}       

/*
** Rotates the currently selected object relative to the camera's frame when passed
**   a vector for the axis and an angle of rotation.
** This is useful because the AXIS which is specified is relative to what appears on the screen with it's origin at the camera
**   (ie: (0,0,1) will be an axis straight into the screen and thus things will spin around the centre of the screen)
*/

void RLDS3D_SetSelectedRotRelToCam(D3DVALUE AxisX, D3DVALUE AxisY, D3DVALUE AxisZ, D3DVALUE angle) {
        if (!sFrame) return;
        RLDS3D_StopOrbitSelected();
        sFrame->SetRotation(info->camera, AxisX, AxisY, AxisZ, angle);
}

/*
** Moves the currently selected object by x/y pixels on the screen from it's relative position.  Allows a user
**   to drag objects around the screen using the mouse.  Assumes that the distance from the frame to the screen
**   will remain constant
*/

void RLDS3D_MoveSelectedPosByScreenCoords(double delta_x, double delta_y) {
        if (!sFrame) return;
        RLDS3D_StopOrbitSelected();
        D3DVECTOR p1;
    D3DRMVECTOR4D p2;
    sFrame->GetPosition(info->scene, &p1);
        info->view->Transform(&p2, &p1);
        // returned value is in homogenous coords so we need to multiply by w to get vector coords
    p2.x += D3DMultiply(D3DVAL((D3DVALUE)delta_x), p2.w);
    p2.y += D3DMultiply(D3DVAL((D3DVALUE)delta_y), p2.w);
    info->view->InverseTransform(&p1, &p2);
    sFrame->SetPosition(info->scene, p1.x, p1.y, p1.z);
}

void RLDS3D_GetDistanceFactor(D3DVALUE *temp) {
        lp3DListenerInfo->GetDistanceFactor(temp);
}

void RLDS3D_GetDopplerFactor(D3DVALUE *temp) {
        lp3DListenerInfo->GetDopplerFactor(temp);
}

void RLDS3D_GetRolloffFactor(D3DVALUE *temp) {
        lp3DListenerInfo->GetRolloffFactor(temp);
}

void RLDS3D_SetDistanceFactor(D3DVALUE temp) {
        lp3DListenerInfo->SetDistanceFactor(temp, DS3D_IMMEDIATE);
}

void RLDS3D_SetDopplerFactor(D3DVALUE temp) {
        lp3DListenerInfo->SetDopplerFactor(temp, DS3D_IMMEDIATE);
}

void RLDS3D_SetRolloffFactor(D3DVALUE temp) {
        lp3DListenerInfo->SetRolloffFactor(temp, DS3D_IMMEDIATE);
}

void RLDS3D_CommitDeferredSettings(void) {
        lp3DListenerInfo->CommitDeferredSettings();
}

BOOL RLDS3D_SoundSelected(void) {
        if (!lpDS) return FALSE;
        if (!sFrame) return FALSE;
        if (!sFrame->GetAppData()) return FALSE;
        if (!((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound) return FALSE;
        return TRUE;
}

void RLDS3D_GetSelConeAngles(LPDWORD inner, LPDWORD outer) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->GetConeAngles(inner, outer);
}

void RLDS3D_GetSelConeOutsideVolume(LPLONG temp) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->GetConeOutsideVolume(temp);
}

void RLDS3D_GetSelMinimumDistance(D3DVALUE *temp) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->GetMinDistance(temp);
}

void RLDS3D_GetSelMaximumDistance(D3DVALUE *temp) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->GetMaxDistance(temp);
}
        
void RLDS3D_SetSelConeAngles(DWORD inner, DWORD outer) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->SetConeAngles(inner, outer, DS3D_IMMEDIATE);
        UpdateConeVisual();
}

void RLDS3D_SetSelConeOutsideVolume(LONG temp) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->SetConeOutsideVolume(temp, DS3D_IMMEDIATE);
}

void RLDS3D_SetSelMinimumDistance(D3DVALUE temp) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->SetMinDistance(temp, DS3D_IMMEDIATE);
}

void RLDS3D_SetSelMaximumDistance(D3DVALUE temp) {
        if (!RLDS3D_SoundSelected) return;
        ((LPFRAMEAPPDATA)(sFrame->GetAppData()))->i3DSound->SetMaxDistance(temp, DS3D_IMMEDIATE);
}





/*
** Orbit control windows have a pointer to the orbiting frame attached to them.  The frame, in return, has the handle of the window in it's
** frameappdata.  When the window is used (the value modified/Stop Orbit button hit) the window modifies the frame and itself appropriately.
** When the frame is modified (ie: Destroyed) it passes messages to the window appropriately.
*/

/*
** Structure to store info about an orbit
*/

struct ORBITDATA {
        LPDIRECT3DRMFRAME orbit_frame;
        LPDIRECT3DRMFRAME child_frame;
        D3DVECTOR axis; // Axis around which the orbit is going
        D3DVALUE speed; // Speed (angles per second)
};

typedef ORBITDATA* LPORBITDATA;

/*
** Structure to store info about a bullet, used by both the dialog box and the callback function
*/

struct BULLETDATA {
        LPDIRECT3DRMFRAME bullet_frame;
        D3DVECTOR vStartPosition;
        D3DVECTOR vDirection;
        D3DVALUE fSpeed;
        D3DVALUE fTime;
};

typedef BULLETDATA* LPBULLETDATA;

static void CDECL bulletCallback(LPDIRECT3DRMFRAME obj, void* arg, D3DVALUE delta) {
        LPBULLETDATA binfo = (LPBULLETDATA)arg;
        if (binfo->fTime <= D3DVAL(0.0)) {
                binfo->fTime = D3DVAL(10.0); // Ten seconds, 5 for before and 5 for after.
                D3DVALUE mult = D3DDivide(D3DMultiply(binfo->fTime, binfo->fSpeed), D3DVAL(-2.0)); // Calculate the displacement multiplier for calculating starting position
                obj->SetPosition(info->scene, binfo->vStartPosition.x + D3DMultiply(mult, binfo->vDirection.x),
                                                  binfo->vStartPosition.y + D3DMultiply(mult, binfo->vDirection.y),
                                                                          binfo->vStartPosition.z + D3DMultiply(mult, binfo->vDirection.z));
        }
        else binfo->fTime -= delta;
}


/*
** Stops the passed frame from orbiting or from bulleting
*/

void StopOrbiting(LPDIRECT3DRMFRAME stopme) {
        // If it isn't orbiting we don't need to stop it!
        if (!stopme) return;
        if (!stopme->GetAppData()) return;
        FRAMEAPPDATA* data = (FRAMEAPPDATA*)stopme->GetAppData();
        if (data->bOrbiting) {
                if (data->hDlg) {
                        // We explicitly call EndDialog on the orbit's control without sending it any message.  As a result we have
                        //   to grab it's associated orbitdata and delete it ourselves.
                        LPORBITDATA orbit_data = (LPORBITDATA)GetWindowLong(data->hDlg, DWL_USER);
                        if (orbit_data) delete orbit_data;
                        EndDialog(data->hDlg, TRUE);
                }

                // Get its parent
                LPDIRECT3DRMFRAME parent;
                stopme->GetParent(&parent);
                
                // Put this frame back into the scene frame (this assumes that's where it came from)
                info->scene->AddChild(stopme);
                
                // The parent frame of an orbiting visual is only around to make the object orbit, so we don't need it
                parent->Release();
                data->bOrbiting = FALSE;
                data->hDlg = 0;
        }
        if (data->bBullet) {
                if (data->hDlg) {
                        LPBULLETDATA bullet_data = (LPBULLETDATA)GetWindowLong(data->hDlg, DWL_USER);
                        stopme->SetPosition(info->scene, bullet_data->vStartPosition.x, bullet_data->vStartPosition.y, bullet_data->vStartPosition.z);
                        if (bullet_data) delete bullet_data;
                        EndDialog(data->hDlg, TRUE);
                        stopme->DeleteMoveCallback(bulletCallback, bullet_data);
                }
                stopme->SetVelocity(info->scene, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), TRUE);
                data->bBullet = FALSE;
                data->hDlg = 0;
        }
}

void RLDS3D_StopOrbitSelected() {
        StopOrbiting(sFrame);
}

/*
** Windows procedure for the orbit dialog box
*/  

BOOL CALLBACK OrbitDlgProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam)
{
    lparam = lparam;

        LPORBITDATA my_data = (ORBITDATA*)GetWindowLong(win, DWL_USER);
        char lpszTS[100];

    switch (msg)
    {
    case WM_INITDIALOG:
                // We've set it up to pass in the pointer to our case-specific data (through WM_INITDIALOG)
                // so we tuck it away for later use.  This points to the ORBITDATA structure associated with
                // this dialog box
                SetWindowLong(win, DWL_USER, lparam);
                sprintf(lpszTS, "%f", ((LPORBITDATA)lparam)->speed);
                SendDlgItemMessage(win, IDC_ORBIT, WM_SETTEXT, 0, (LPARAM) lpszTS);
                SendDlgItemMessage(win, IDC_ORBIT, EM_SETLIMITTEXT, 100, 0);
        return TRUE;

    case WM_COMMAND:
                {
                        if (wparam == IDOK) {
                                // Stopping the orbit will end the dialog for us.
                                StopOrbiting(my_data->child_frame);
                                return TRUE;
                        }
                        else if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_ORBIT) {
                                // Get string length
                                int stringlength = SendDlgItemMessage(win, IDC_ORBIT, EM_LINELENGTH, 0, 0);
                                // Check to make sure the string isn't too long - atof accepts up to 100 chars
                                if (stringlength > 99) return TRUE;
                                lpszTS[0] = (char)stringlength;
                                // Get string
                                SendDlgItemMessage(win, IDC_ORBIT, EM_GETLINE, 0, (LPARAM) lpszTS);
                                lpszTS[stringlength] = 0;
                                // Store speed and set new rotation
                                my_data->speed = D3DVAL(atof(lpszTS));
                                my_data->orbit_frame->SetRotation(info->scene, my_data->axis.x, my_data->axis.y, my_data->axis.z, my_data->speed);
                                return TRUE;
                        }
                        else return FALSE;
                }               
        break;
        }
        return FALSE;
}

/*
** Windows proc for the bullet dialog box
*/

BOOL CALLBACK BulletDlgProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam)
{
    lparam = lparam;

        LPBULLETDATA my_data = (BULLETDATA*)GetWindowLong(win, DWL_USER);
        char lpszTS[100];

    switch (msg)
    {
    case WM_INITDIALOG:
                // We've set it up to pass in the pointer to our case-specific data (through WM_INITDIALOG)
                // so we tuck it away for later use.  This points to the ORBITDATA structure associated with
                // this dialog box
                SetWindowLong(win, DWL_USER, lparam);
                sprintf(lpszTS, "%f", ((LPBULLETDATA)lparam)->fSpeed);
                SendDlgItemMessage(win, IDC_BULLET, WM_SETTEXT, 0, (LPARAM) lpszTS);
                SendDlgItemMessage(win, IDC_BULLET, EM_SETLIMITTEXT, 100, 0);
        return TRUE;

    case WM_COMMAND:
                {
                        if (wparam == IDOK) {
                                // Stopping the orbit will end the dialog for us.
                                StopOrbiting(my_data->bullet_frame);
                                return TRUE;
                        }
                        else if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_BULLET) {
                                // Get string length
                                int stringlength = SendDlgItemMessage(win, IDC_BULLET, EM_LINELENGTH, 0, 0);
                                // Check to make sure the string isn't too long - atof accepts up to 100 chars
                                if (stringlength > 99) return TRUE;
                                lpszTS[0] = (char)stringlength;
                                // Get string
                                SendDlgItemMessage(win, IDC_BULLET, EM_GETLINE, 0, (LPARAM) lpszTS);
                                lpszTS[stringlength] = 0;

                                // Store speed and set
                                my_data->fSpeed = D3DVAL(atof(lpszTS));
                                my_data->bullet_frame->SetVelocity(info->scene, D3DMultiply(my_data->vDirection.x, my_data->fSpeed),
                                                                    D3DMultiply(my_data->vDirection.y, my_data->fSpeed), 
                                                                    D3DMultiply(my_data->vDirection.z, my_data->fSpeed),
                                                                                                                                        TRUE);
                                return TRUE;
                        }
                        else return FALSE;
                }               
        break;
        }
        return FALSE;
}



/*
** Orbits the currently selected object around the camera... creates a frame which is centred at the camera and contains
**   the selected frame, and then rotates that frame around the camera
*/

ULONG orbit_num = 1;

void RLDS3D_OrbitSelected(void) {
        if (!sFrame) return;
        StopOrbiting(sFrame);
        FRAMEAPPDATA* tempdat;
        
        // Need to create orbit data, if not then can't orbit
        LPORBITDATA orbitdat = new ORBITDATA;
        if (!orbitdat) return;

        if (!sFrame->GetAppData()) {
                tempdat = new FRAMEAPPDATA();
                if (!tempdat) return;
                sFrame->SetAppData((ULONG)(tempdat));
        }
        else {
                tempdat = (LPFRAMEAPPDATA)sFrame->GetAppData();
        }

        // Create a frame.  Centre it around the camera.  Add the selected frame.  Rotate it!
        LPDIRECT3DRMFRAME orbiting_frame;

        if (!D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &orbiting_frame))) return;

        orbiting_frame->SetPosition(info->camera, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0));
        orbiting_frame->SetOrientation(info->camera, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0));
        orbiting_frame->AddChild(sFrame);

        D3DVECTOR vDir, vUp;
        info->camera->GetOrientation(info->scene, &vDir, &vUp);

        // Store orbit info
        orbitdat->orbit_frame = orbiting_frame;
        orbitdat->child_frame = sFrame;
        orbitdat->speed = D3DVAL(1.0);
        orbitdat->axis.x = vUp.x;
        orbitdat->axis.y = vUp.y;
        orbitdat->axis.z = vUp.z;

        orbiting_frame->SetRotation(info->scene, vUp.x, vUp.y, vUp.z, orbitdat->speed);

        // Set the boolean in the frame's appinfo to tell everyone it's orbiting
        tempdat->bOrbiting = TRUE;
        
        // Create a dialog box and orbitinfo so that this thing has something to control it
        HWND mydial = CreateDialogParam(hinst, "OrbitBox", hwndParent, (int(__stdcall*)(void))OrbitDlgProc, (LONG)orbitdat);
        if (mydial) {
                char name_string[50];
                sprintf(name_string, "Orbit #%i", orbit_num++);
                SetWindowText(mydial, name_string);
                ShowWindow(mydial, SW_SHOW);
                UpdateWindow(mydial);
                tempdat->hDlg = mydial;
        }
}

/*
** Bullet the currently selected object.  Bullets have two properties: Speed and bullet-time.  The bullet passes through it's original
**   position along the path defined by the viewer's original viewpoint (it heads towards the viewer).  It takes 5 seconds to get to the
**   viewer and five seconds once past the viewer, and continues doing thus until the bullet is stopped.  Speed is controlled by the user.
*/

ULONG bullet_num = 1;

void RLDS3D_BulletSelected(void) {
        if (!sFrame) return;
        StopOrbiting(sFrame);

        FRAMEAPPDATA* tempdat;
        if (!sFrame->GetAppData()) {
                tempdat = new FRAMEAPPDATA();
                if (!tempdat) return;
                sFrame->SetAppData((ULONG)(tempdat));
        }
        else {
                tempdat = (LPFRAMEAPPDATA)sFrame->GetAppData();
        }

        // Create a new bullet object and attach our callback to the frame (it will release itself and put itself back
        //   into position when it's done!)

        LPBULLETDATA bs = new BULLETDATA;
        if (!bs) return;

        sFrame->GetPosition(info->scene, &(bs->vStartPosition));

        D3DVECTOR vDir, vUp;
        info->camera->GetOrientation(info->scene, &vDir, &vUp);
        // velocity heading towards camera
        bs->vDirection.x = -vDir.x;
        bs->vDirection.y = -vDir.y;
        bs->vDirection.z = -vDir.z;

        bs->bullet_frame = sFrame;
        bs->fTime = D3DVAL(10.0); // Ten seconds, 5 for before and 5 for after.
        bs->fSpeed = D3DVAL(10.0);
        
        D3DVALUE mult = D3DDivide(D3DMultiply(bs->fTime, bs->fSpeed), D3DVAL(-2.0)); // Calculate the displacement multiplier for calculating starting position
        
        sFrame->SetPosition(info->scene, bs->vStartPosition.x + D3DMultiply(mult, bs->vDirection.x),
                                             bs->vStartPosition.y + D3DMultiply(mult, bs->vDirection.y),
                                                                     bs->vStartPosition.z + D3DMultiply(mult, bs->vDirection.z));

        sFrame->SetVelocity(info->scene, D3DMultiply(bs->vDirection.x, bs->fSpeed),
                             D3DMultiply(bs->vDirection.y, bs->fSpeed),
                             D3DMultiply(bs->vDirection.z, bs->fSpeed),
                                                         TRUE);

        sFrame->AddMoveCallback(bulletCallback, (void*)bs);
        
        // Set the boolean in the frame's appinfo to tell everyone it's orbiting
        tempdat->bBullet = TRUE;

        // Create a dialog box and orbitinfo so that this thing has something to control it
        HWND mydial = CreateDialogParam(hinst, "BulletBox", hwndParent, (int(__stdcall*)(void))BulletDlgProc, (LONG)bs);
        if (mydial) {
                char name_string[50];
                sprintf(name_string, "Bullet #%i", bullet_num++);
                SetWindowText(mydial, name_string);
                ShowWindow(mydial, SW_SHOW);
                UpdateWindow(mydial);
                tempdat->hDlg = mydial;
        }
}

/*
**********************************  DIRECTSOUND 3D FUNCTIONS  *****************************************
*/

/*
** The RLDS3D interface allows you to attach sound(s) to the selected object which can then be played and
**   whose 3D audio position will update according to the RL object's position
**
** Some functionality is applied to all sounds, and as a result, the RLDS3D interface keeps a record of
**   existing sounds and which frames they are attached to.  Removal of a frame and/or a sound updates these
**   records
*/

/*
** CALLBACKS - update position as 3D objects move around
*/


// The listener callback is attached to the camera.  This is done in CreateScene()
static void CDECL listenerCallback(LPDIRECT3DRMFRAME obj, void* arg, D3DVALUE delta)
{
        arg = arg;
        if (!lpDS) return;
        D3DVECTOR rlvCameraInfo, rlvCameraUp;

        info->camera->GetPosition(info->scene, &rlvCameraInfo);
        lp3DListenerInfo->SetPosition(rlvCameraInfo.x, rlvCameraInfo.y,
                                      rlvCameraInfo.z, DS3D_DEFERRED);

        info->camera->GetOrientation(info->scene, &rlvCameraInfo, &rlvCameraUp);
        lp3DListenerInfo->SetOrientation(rlvCameraInfo.x, rlvCameraInfo.y,
                                         rlvCameraInfo.z, rlvCameraUp.x,
                                         rlvCameraUp.y, rlvCameraUp.z, DS3D_DEFERRED);

        info->camera->GetVelocity(info->scene, &rlvCameraInfo, TRUE);
        lp3DListenerInfo->SetVelocity(rlvCameraInfo.x, rlvCameraInfo.y,
                                      rlvCameraInfo.z, DS3D_DEFERRED);

        lp3DListenerInfo->CommitDeferredSettings();
}


// Sounds are updated whenever their frame moves using Render Callbacks.  This is the callback function.
static void CDECL soundCallback(LPDIRECT3DRMFRAME obj, void* arg, D3DVALUE delay)
{
        arg = arg;
        LPDIRECT3DRMFRAME tFrame = (LPDIRECT3DRMFRAME)obj;
        if (!lpDS) return;
        // Get the sound from the frame's app data
        LPFRAMEAPPDATA lpAppDat = (LPFRAMEAPPDATA)tFrame->GetAppData();
        if (!lpAppDat) return;
        // Get the 3D sound buffer and remove the sound callback if it's NULL since it shouldn't exist
        if (!lpAppDat->Sound || !lpAppDat->i3DSound) {
                releaseSoundCallback(tFrame);
                return;
        }
        D3DVECTOR rlvVisualInfo, rlvVisualUp;

        tFrame->GetPosition(info->scene, &rlvVisualInfo);
        lpAppDat->i3DSound->SetPosition(rlvVisualInfo.x, rlvVisualInfo.y,
                                        rlvVisualInfo.z, DS3D_DEFERRED);

        tFrame->GetOrientation(info->scene, &rlvVisualInfo, &rlvVisualUp);
        lpAppDat->i3DSound->SetConeOrientation(rlvVisualInfo.x, rlvVisualInfo.y,
                                               rlvVisualInfo.z, DS3D_DEFERRED);
 
        tFrame->GetVelocity(info->scene, &rlvVisualInfo, TRUE);
        lpAppDat->i3DSound->SetVelocity(rlvVisualInfo.x, rlvVisualInfo.y,
                                        rlvVisualInfo.z, DS3D_DEFERRED);

        lp3DListenerInfo->CommitDeferredSettings();
}

// This adds a sound callback function to a frame
void setSoundCallback(LPDIRECT3DRMFRAME frame) {
        frame->AddMoveCallback(soundCallback, NULL);
}

// Removes a sound callback from a frame
void releaseSoundCallback(LPDIRECT3DRMFRAME frame) {
        frame->DeleteMoveCallback(soundCallback, NULL);
}

/*
** Sound records - keeps track of which frames have sounds associated with them and
**   adds/removes the sounds as required.
*/

struct SoundRecord {
        LPDIRECT3DRMFRAME  lpFrame;
        SoundRecord* next;
};

SoundRecord* top = NULL;

BOOL Recurse_Remove(LPDIRECT3DRMFRAME owner, SoundRecord* record) {
        if (!record) return FALSE;
        if (Recurse_Remove(owner, record->next)) {
                SoundRecord* temp = record->next;
                record->next = temp->next;
                delete temp;
        }
        if (owner == record->lpFrame) return TRUE;
        return FALSE;
}

// Removes a frame from the records
void RemoveSoundRecord(LPDIRECT3DRMFRAME owner) {
        if (!owner) return;

        // Don't bother deleting frames without appdata and a sound associated with them
        // because they SHOULDN'T be on the list.
        if (!owner->GetAppData()) return;
        LPFRAMEAPPDATA data = (LPFRAMEAPPDATA)(owner->GetAppData());
        if (!data->Sound) return;
        
        // Release the sound and remove the soundrecord from the list.
        data->i3DSound->Release();
        data->i3DSound = NULL;
        data->Sound->Release();
        data->Sound = NULL;
        releaseSoundCallback(owner);
        
        if (Recurse_Remove(owner, top)) {
                SoundRecord* temp = top;
                top = top->next;
                delete temp;
        }
}

// Adds a frame to the records
void AddSoundRecord(LPDIRECT3DRMFRAME owner, char* sound_filename) {
        if (!lpDS) return;
        if (!owner) return;
        if (!sound_filename) return;
        
        // Removes all previous sounds from this frame
        RemoveSoundRecord(owner);
        
        // Create frame data for this frame if it hasn't already been done
        LPFRAMEAPPDATA data = (LPFRAMEAPPDATA)(owner->GetAppData());
        if (!data) {
                data = new FRAMEAPPDATA();
                if (!data) return;
                owner->SetAppData((ULONG)data);
        }
        
        // Create the sound and attach it to the frame
        data->Sound = DSLoad3DSoundBuffer(lpDS, sound_filename);
        if (!data->Sound) return; 
        
        // Query to get the 3D interface, destroy the sound buffer if it's not available...
        data->Sound->QueryInterface(IID_IDirectSound3DBuffer, (void**)&data->i3DSound);
        if (!data->i3DSound) {
                data->Sound->Release();
                data->Sound = NULL;
                return;
        }

        // Set the minimum distance at which the sound's amplitude should decay.
        data->i3DSound->SetMinDistance((D3DVALUE)10.0, DS3D_IMMEDIATE);

        setSoundCallback(owner);

        SoundRecord* temp = new SoundRecord;
        temp->next = top;
        top = temp;
        top->lpFrame = owner;
}

/*
** Functionality for users
*/

// Stops all the sounds in the world.  (Actually, only in the local RL world.)
// Runs through the records of all the sounds and stops them.
void RLDS3D_StopAllSounds() {
        if (!lpDS) return;
        SoundRecord* temp = top;
        while (temp) {
                LPFRAMEAPPDATA data = (LPFRAMEAPPDATA)(temp->lpFrame->GetAppData());
                if (data) {
                        if (data->Sound) data->Sound->Stop();
                }
                temp = temp->next;
        }
}

// Removes all sounds in the world
void RLDS3D_RemoveAllSounds() {
        if (!lpDS) return;
        while (top) RemoveSoundRecord(top->lpFrame);
                UpdateConeVisual();
}

// Plays the sound associated with the currently selected object
void RLDS3D_PlaySound(BOOL bIsLooping) {
        if (!sFrame) return;
        if (!lpDS) return;
        LPFRAMEAPPDATA lpAppDat = (LPFRAMEAPPDATA)sFrame->GetAppData();
        if (lpAppDat) {
                if (lpAppDat->Sound) {
                        lpAppDat->Sound->Stop();
                        lpAppDat->Sound->SetCurrentPosition(0);
                        if (bIsLooping) {
                                lpAppDat->Sound->Play(0,0,DSBPLAY_LOOPING);
                        }
                        else {
                                lpAppDat->Sound->Play(0,0,0);
                        }
                }
        }
}

// Stops the sound associated with the currently selected object
void RLDS3D_StopSelectedSound() {
        if (!sFrame) return;
        if (!lpDS) return;
        LPFRAMEAPPDATA lpAppDat = (LPFRAMEAPPDATA)sFrame->GetAppData();
        if (lpAppDat) {
                if (lpAppDat->Sound) {
                        lpAppDat->Sound->Stop();
                }
        }
}

// Removes the sound from the currently selected object
void RLDS3D_RemoveSound() {
        RLDS3D_StopSelectedSound();
        if (!sFrame) return;
        if (!lpDS) return;
        RemoveSoundRecord(sFrame);
                UpdateConeVisual();
}

// Attaches a sound (filename provided) to the selected frame
void RLDS3D_AttachSound(char* filename) {
        // Removes the sound attached to the currently selected item (if it exists)
        RLDS3D_RemoveSound();
        if (!sFrame) return;
        if (!lpDS) return;
        AddSoundRecord(sFrame, filename);
                UpdateConeVisual();
}

/*
*************************************  MISC. MAINTENANCE FUNCTIONS  **************************************
*/

/*
** Allows external users access to the RL Device to deal with Windows-related issues
**   (See case WM_ACTIVATE: and case WM_PAINT: in viewer source for examples of HandleActivate() and HandlePaint())
** Design note: This was done to save time from the conversion from the old version of the viewer rather than
**   having RLDS3D_HandleActivate(), etc.
*/

LPDIRECT3DRMDEVICE RLDS3D_WinDevice() {
        if (!info) return NULL;
        return info->dev;
}

/*
** Handles activation messages from the window
*/

void RLDS3D_HandleActivate(WPARAM wparam) {
        if (!info || !info->dev) return;
        LPDIRECT3DRMWINDEVICE windev;
        if (D3DRM_SUCCEED(info->dev->QueryInterface(IID_IDirect3DRMWinDevice, (void **) &windev))) {
                windev->HandleActivate(wparam);
                windev->Release();
        }
}

/*
** Handles paint messages from the window - the paintstruct which BeginPaint has been called on is
**   passed to it
*/

void RLDS3D_HandlePaint(PAINTSTRUCT* ps) {
        if (!info) return;
        LPDIRECT3DRMWINDEVICE windev;
        if (D3DRM_SUCCEED(info->dev->QueryInterface(IID_IDirect3DRMWinDevice, (void **) &windev))) {
                windev->HandlePaint(ps->hdc);
                windev->Release();
        }
}

/*
** Tells whether or not sound is initialized
*/

BOOL RLDS3D_SoundInitialized() {
        if (lpDS) return TRUE;
        return FALSE;
}

/*
** Tells whether or not a frame is selected
*/

BOOL RLDS3D_FrameSelected() {
        if (sFrame) return TRUE;
        return FALSE;
}

/*
** Render the scene into the viewport.
*/

void RLDS3D_Render(D3DVALUE time_delta)
{
    // When the WM_SIZE message passes 0's as size to the ResizeViewport we know that it's minimized, in which case we don't render it.
    if (info->bMinimized == TRUE) return;
    D3DRM_SUCCEED(info->scene->Move(time_delta));
    D3DRM_SUCCEED(info->view->Clear());
    D3DRM_SUCCEED(info->view->Render(info->scene));
    D3DRM_SUCCEED(info->dev->Update());
}


/*
 * Resize the viewport and device when the window size changes.
 */
void RLDS3D_ResizeViewport(int width, int height)
{
    int view_width = info->view->GetWidth();
    int view_height = info->view->GetHeight();
    int dev_width = info->dev->GetWidth();
    int dev_height = info->dev->GetHeight();

    if (!(width && height)) {
            info->bMinimized = TRUE;
            return;

    }
    else info->bMinimized = FALSE;

    if (view_width == width && view_height == height)
        return;
        else info->bMinimized = FALSE;
    
    if (width <= dev_width && height <= dev_height) {
        info->view->Release();
        D3DRM_SUCCEED(lpD3DRM->CreateViewport(info->dev, info->camera, 0, 0, width, height, &info->view));
        info->view->SetBack(D3DVAL(400.0));
    }

    int ret;
    if (!RebuildDevice(hwndParent, info, width, height)) {
        ret = MessageBox(hwndParent, "Unable to create Direct3D device", "D3DRM Fatal Error", MB_APPLMODAL|MB_ICONSTOP|MB_OK);
        PostMessage(hwndParent, WM_CLOSE,0,0);
    };
}

/*
** Sets/Gets the polygon fill mode
*/

D3DRMFILLMODE RLDS3D_GetPolygonFillMode(void) {
    return (D3DRMFILLMODE)(info->dev->GetQuality() & D3DRMFILL_MASK);
}

void RLDS3D_SetPolygonFillMode(D3DRMFILLMODE quality) {
    D3DRMRENDERQUALITY oldq = info->dev->GetQuality();
    oldq = (oldq & ~D3DRMFILL_MASK) | quality;
    info->dev->SetQuality(oldq);
}

/*
** Sets/Gets the polygon shading mode
*/

D3DRMSHADEMODE RLDS3D_GetPolygonShadeMode(void) {
        return (D3DRMSHADEMODE)(info->dev->GetQuality() & D3DRMSHADE_MASK);
}

void RLDS3D_SetPolygonShadeMode(D3DRMSHADEMODE quality) {
        D3DRMRENDERQUALITY oldq = info->dev->GetQuality();
        oldq = (oldq & ~D3DRMSHADE_MASK) | quality;
        info->dev->SetQuality(oldq);
}


/*
** Sets/Gets the color model for the viewport (RGB or mono (256-color-based))
*/

D3DRMCOLORMODEL RLDS3D_GetColourModel(void) {
        return info->model;
}

void RLDS3D_SetColourModel(D3DRMCOLORMODEL model) {
    info->model = model;
    int ret;
    if (!RebuildDevice(hwndParent, info, info->dev->GetWidth(), info->dev->GetHeight())) {
        ret = MessageBox(hwndParent, "Unable to selected Direct3D device", "D3DRM Fatal Error", MB_APPLMODAL|MB_ICONSTOP|MB_OK);
        PostMessage(hwndParent, WM_CLOSE,0,0);
    }
}

/*
** Sets/Gets whether or not lighting is on
*/
 
BOOL RLDS3D_GetLighting(void) {
    D3DRMLIGHTMODE mode = (D3DRMLIGHTMODE)(info->dev->GetQuality() & D3DRMLIGHT_MASK);
    if (mode == D3DRMLIGHT_ON) return TRUE;
    return FALSE;
}

void RLDS3D_SetLighting(BOOL new_val) {
    D3DRMRENDERQUALITY qual = info->dev->GetQuality() & ~D3DRMLIGHT_MASK;
    if (new_val) qual |= D3DRMLIGHT_ON; else qual |= D3DRMLIGHT_OFF;
    info->dev->SetQuality(qual);
}

/*
** Sets/Gets whether or not dithering is on
*/

BOOL RLDS3D_GetDither(void) {
    return info->dev->GetDither();
}

void RLDS3D_SetDither(BOOL dither) {
    info->dev->SetDither(dither);
}

/*
** Sets/Gets texture quality (only relevant for RGB modes)
*/

D3DRMTEXTUREQUALITY RLDS3D_GetTextureQuality(void) {
    return info->dev->GetTextureQuality();
}

void RLDS3D_SetTextureQuality(D3DRMTEXTUREQUALITY new_quality) {
    info->dev->SetTextureQuality(new_quality);
}


/*
*************************************  INTERNAL FUNCTIONS (Not part of API)  ********************************
*/

/*
** Given a bounding box this generates a visual representation of it using rods and cones
*/

static LPDIRECT3DRMMESHBUILDER makeBox(D3DRMBOX* box)
{
    LPDIRECT3DRMMESHBUILDER mesh;
    static D3DVECTOR zero = {D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0)};
    static D3DVECTOR dir = {D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0)};
    D3DVECTOR a, b;

    if (!D3DRM_SUCCEED(lpD3DRM->CreateMeshBuilder(&mesh))) return NULL;

    dir.z = box->max.z + D3DVAL(1.0);
    AddRod(mesh, D3DVAL(0.05), zero, dir);
    a = dir;
    a.z += D3DVAL(0.6);
    AddCone(mesh, D3DVAL(0.2), dir, a);
    a = box->min;
    b = a;
    b.y = box->max.y;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.x = box->max.x;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.y = box->min.y;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.x = box->min.x;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.z = box->max.z;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.x = box->max.x;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.y = box->max.y;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.x = box->min.x;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b; b.y = box->min.y;
    AddRod(mesh, D3DVAL(0.05), a, b);
    b.y = box->max.y; a = b; b.z = box->min.z;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a = b = box->max; b.z = box->min.z;
    AddRod(mesh, D3DVAL(0.05), a, b);
    a.y = box->min.y; b = a; b.z = box->min.z;
    AddRod(mesh, D3DVAL(0.05), a, b);
                
    if (!D3DRM_SUCCEED(mesh->SetColor(D3DRMCreateColorRGB(D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0))))) {
        mesh->Release();
        return NULL;
    }
    return mesh;
}

/*
** Given a box, this creates a mesh in the shape of a 16-sided speaker cone aimed forward with requested angle
*/

#define CONE_POINTS 16
#define pi 3.14159

static LPDIRECT3DRMMESHBUILDER makeSpeaker(D3DRMBOX* box, D3DVALUE in_angle) {
    if (!box) return NULL;
    
    D3DVALUE angle = in_angle / D3DVAL(2.0); 
        DWORD* speaker_faces = new DWORD[CONE_POINTS*4+1];
    if (!speaker_faces) return NULL;
    memset(speaker_faces, 0, sizeof(DWORD[CONE_POINTS*4+1]));

    LPDIRECT3DRMMESHBUILDER mesh;
    if (!D3DRM_SUCCEED(lpD3DRM->CreateMeshBuilder(&mesh))) {
        delete speaker_faces;
        return NULL;
    }
    
    static D3DVECTOR zero = {D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0)};

    D3DVECTOR v[CONE_POINTS+1];

    // center of the cone
    v[CONE_POINTS] = zero;

    int looper;

    // Angle along XZ plane which is rotated CONE_POINT times to form cone
    D3DVECTOR base_angle;
    base_angle.z = (box->max.z + D3DVAL(2.0)) * (D3DVALUE)cos(angle * pi / 180.0);
    base_angle.x = (box->max.z + D3DVAL(2.0)) * (D3DVALUE)sin(angle * pi / 180.0);
    base_angle.y = D3DVAL(0.0);
    
    for (looper=0; looper<CONE_POINTS; looper++) {
        v[looper].z = base_angle.z;
        v[looper].x = base_angle.x * (D3DVALUE)cos((looper*2*pi)/CONE_POINTS);
        v[looper].y = base_angle.x * (D3DVALUE)sin((looper*2*pi)/CONE_POINTS);
        speaker_faces[looper*4] = 3;
        speaker_faces[looper*4+1] = looper % CONE_POINTS;
        speaker_faces[looper*4+2] = (looper + 1) % CONE_POINTS;
        speaker_faces[looper*4+3] = CONE_POINTS;
    }

    v[CONE_POINTS] = zero;

    if (!D3DRM_SUCCEED(mesh->AddFaces(CONE_POINTS+1, v, 0, NULL, speaker_faces, NULL))) {
        delete speaker_faces;
        mesh->Release();
        return NULL;
    }

    for (looper=0; looper<CONE_POINTS; looper++) {
        speaker_faces[looper*4+2] = looper % CONE_POINTS;
        speaker_faces[looper*4+1] = (looper + 1) % CONE_POINTS;
    }

    if (!D3DRM_SUCCEED(mesh->AddFaces(CONE_POINTS+1, v, 0, NULL, speaker_faces, NULL))) {
        delete speaker_faces;
        mesh->Release();
        return NULL;
    }
    
    delete speaker_faces;

    if (!D3DRM_SUCCEED(mesh->SetColor(D3DRMCreateColorRGB(D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0))))) {
        mesh->Release();
        return NULL;
    }

    if (!D3DRM_SUCCEED(mesh->SetQuality((mesh->GetQuality() & ~D3DRMSHADE_MASK) | D3DRMSHADE_FLAT))) {
        mesh->Release();
        return NULL;
    }

    return mesh;
}

/*
** Selects the given visual inside the given frame
*/

void UpdateConeVisual(void) {
        if (!sFrame) return;
        if (!sFrame->GetAppData()) return;
    LPFRAMEAPPDATA fd = (LPFRAMEAPPDATA)sFrame->GetAppData();
        if (!fd->i3DSound) return;
        if (showBoxes && sVisual)
        {   D3DRMBOX box;
            LPDIRECT3DRMMESHBUILDER builder;
                sFrame->DeleteVisual(selectionSpeaker);
            sVisual->GetBox(&box);
                        DWORD temp, outer;
                        fd->i3DSound->GetConeAngles(&temp, &outer);
            builder = makeSpeaker(&box, D3DVAL(temp));
            builder->CreateMesh(&selectionSpeaker);
            sFrame->AddVisual(selectionSpeaker);
            selectionSpeaker->Release();
                        builder->Release();
                }

}

void SelectVisual(LPDIRECT3DRMMESHBUILDER visual, LPDIRECT3DRMFRAME frame) {
    RLDS3D_DeselectVisual();
    sVisual = visual;
    sFrame = frame;

    if (sVisual)
    {   LPDIRECT3DRMLIGHTARRAY lights;

        sLight = 0;
        sFrame->GetLights(&lights);
        if (lights)
        {   if (lights->GetSize())
            {   lights->GetElement(0, &sLight);
                sLight->Release(); /* reinstate reference count */
            }
            lights->Release();
        }

        if (showBoxes && visual)
        {   D3DRMBOX box;
            LPDIRECT3DRMMESHBUILDER builder;

            sVisual->GetBox(&box);
            builder = makeBox(&box);
            builder->CreateMesh(&selectionBox);
            sFrame->AddVisual(selectionBox);
            selectionBox->Release();
                        builder->Release();
                        UpdateConeVisual();
        }
    }
}

LPGUID
FindDevice(D3DCOLORMODEL cm)
{
    LPDIRECTDRAW lpDD;
    LPDIRECT3D lpD3D;
    D3DFINDDEVICESEARCH search;
    static D3DFINDDEVICERESULT result;
    HRESULT error;

    if (DirectDrawCreate(NULL, &lpDD, NULL) != DD_OK)
        return NULL;

    if (lpDD->QueryInterface(IID_IDirect3D, (void**) &lpD3D) != DD_OK) {
        lpDD->Release();
        return NULL;
    }
    
    memset(&search, 0, sizeof search);
    search.dwSize = sizeof search;
    search.dwFlags = D3DFDS_COLORMODEL;
    search.dcmColorModel = (cm == D3DCOLOR_MONO) ? D3DCOLOR_MONO : D3DCOLOR_RGB;

    memset(&result, 0, sizeof result);
    result.dwSize = sizeof result;

    error = lpD3D->FindDevice(&search, &result);

    lpD3D->Release();
    lpDD->Release();

    if (error != D3D_OK)
        return NULL;
    else
        return &result.guid;
}

/*
 * Create the device and viewport.
 */

static BOOL CreateDevice(HWND win, AppInfo* info)
{
    RECT r;
    int bpp;
    HDC hdc;

    GetClientRect(win, &r);
    if (!D3DRM_SUCCEED(lpD3DRM->CreateDeviceFromClipper(lpDDClipper, NULL, r.right, r.bottom, &info->dev))) return FALSE;
        
        hdc = GetDC(win);
    bpp = GetDeviceCaps(hdc, BITSPIXEL);
    ReleaseDC(win, hdc);
    switch (bpp)
    {
    case 1:
        info->dev->SetShades(4);
        lpD3DRM->SetDefaultTextureShades(4);
        break;
    case 16:
        info->dev->SetShades(32);
        lpD3DRM->SetDefaultTextureColors(64);
        lpD3DRM->SetDefaultTextureShades(32);
        info->dev->SetDither(FALSE);
        break;
    case 24:
        info->dev->SetShades(256);
        lpD3DRM->SetDefaultTextureColors(64);
        lpD3DRM->SetDefaultTextureShades(256);
        info->dev->SetDither(FALSE);
        break;
    default:
        info->dev->SetDither(FALSE);
    }
    if (!CreateScene(info)) {
                info->dev->Release();
                return FALSE;
        }
    if (!D3DRM_SUCCEED(lpD3DRM->CreateViewport(info->dev, info->camera, 0, 0, info->dev->GetWidth(), info->dev->GetHeight(), &info->view))) {
                info->dev->Release();
                return FALSE;
        }
    info->view->SetBack(D3DVAL(5000.0));
        return TRUE;
}

/*
 * Creates a simple scene and adds it to the main scene
 */

static BOOL CreateScene(AppInfo* info)
{
    LPDIRECT3DRMFRAME light;
    LPDIRECT3DRMLIGHT light1, light2;

    // Note that if something fails, we don't bother freeing up everything we've created... the caller to CreateScene should destroy
    //   the lpD3DRM object and that should happily release everything created with it.
    // Also note that, since we're just the viewer, if there's a critical error we pass a quit message with our error message since
    //   we'd want to quit if the initialize failed anyways...

    if (!D3DRM_SUCCEED(lpD3DRM->CreateFrame(NULL, &info->scene))) return FALSE;
    if (!D3DRM_SUCCEED(lpD3DRM->CreateLightRGB(D3DRMLIGHT_DIRECTIONAL, D3DVAL(1.0), D3DVAL(1.0), D3DVAL(1.0), &light1))) return FALSE;
    if (!D3DRM_SUCCEED(lpD3DRM->CreateLightRGB(D3DRMLIGHT_AMBIENT, D3DVAL(0.1), D3DVAL(0.1), D3DVAL(0.1), &light2))) return FALSE;
    if (!D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &light))) return FALSE;

    light->SetPosition(info->scene, D3DVAL(2.0), D3DVAL(2.0), D3DVAL(5.0));
    light->SetOrientation(info->scene, D3DVAL(-1.0), D3DVAL(-1.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0));
    light->AddLight(light1);
    info->scene->AddLight(light2);

    if (!D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &info->camera))) return FALSE;
    info->camera->SetPosition(info->scene, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0));
    // Add a callback to the camera's frame so that the listener is updated with the camera
    info->camera->AddMoveCallback(listenerCallback, NULL);

    light->Release(), light1->Release(), light2->Release();
    return TRUE;
}

/*
 * Regenerate the device if the color model changes or the window size
 * changes.
 */
static BOOL RebuildDevice(HWND win, AppInfo* info, int width, int height)
{
    int old_dither = info->dev->GetDither();
    D3DRMRENDERQUALITY old_quality = info->dev->GetQuality();
    int old_shades = info->dev->GetShades();

    info->view->Release();
    info->dev->Release();
    
    LPGUID guid = FindDevice(info->model);

    if (!guid) return FALSE;

    if (!D3DRM_SUCCEED(lpD3DRM->CreateDeviceFromClipper(lpDDClipper, guid, width, height, &info->dev))) return FALSE;

    info->dev->SetDither(old_dither);
    info->dev->SetQuality(old_quality);
    info->dev->SetShades(old_shades);
    width = info->dev->GetWidth();
    height = info->dev->GetHeight();
    if (!D3DRM_SUCCEED(lpD3DRM->CreateViewport(info->dev, info->camera, 0, 0, width, height, &info->view))) return FALSE;
    info->view->SetBack(D3DVAL(400.0));
        return TRUE;
}

/*
 * Place an object in front of the camera.
 */
static void PlaceMesh(LPDIRECT3DRMMESHBUILDER mesh, AppInfo *info)
{
    LPDIRECT3DRMFRAME frame;

    if (!D3DRM_SUCCEED(lpD3DRM->CreateFrame(info->scene, &frame))) return;
    frame->AddVisual(mesh);
    frame->SetPosition(info->camera, D3DVAL(0.0), D3DVAL(0.0), D3DVAL(15.0));
    frame->Release();
}

HRESULT loadTextures(char *name, void *arg, LPDIRECT3DRMTEXTURE *tex)
{
    char* ext = LSTRRCHR(name, (int)'.');

    if (ext && !lstrcmpi(ext, ".ppm"))
        if (D3DRM_SUCCEED(lpD3DRM->LoadTexture(name, tex))) return 0;
    return -1;
}

/*
** Finds the last occurance of bChar in a null-terminated string, good for finding a pointer to the extension of a filename
*/
char* LSTRRCHR( const char* lpString, int bChar )
{
    if( lpString != NULL )
    {
        const char*     lpBegin;

        lpBegin = lpString;

        while( *lpString != 0 )
        {
            lpString++;
        }

        while( 1 )
        {
            if( *lpString == bChar )
            {
                return (char*)lpString;
            }
            
            if( lpString == lpBegin )
            {
                break;
            }

            lpString--;
        }
    }

    return NULL;
} /* LSTRRCHR */


/*
** Strange little function to pick a color from a table using standardized Windows stuff
*/

int ChooseNewColor(HWND win, D3DCOLOR* current)
{
    CHOOSECOLOR cc;
    COLORREF clr;
    COLORREF aclrCust[16];
    int i;

    for (i = 0; i < 16; i++)
        aclrCust[i] = RGB(255, 255, 255);

    clr =
        RGB
        (   (int) (255 * D3DRMColorGetRed(*current)),
            (int) (255 * D3DRMColorGetGreen(*current)),
            (int) (255 * D3DRMColorGetBlue(*current))
        );

    memset(&cc, 0, sizeof(CHOOSECOLOR));
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = win;
    cc.rgbResult = clr;
    cc.lpCustColors = aclrCust;
    cc.Flags = CC_RGBINIT|CC_FULLOPEN;

    if (ChooseColor(&cc))
    {   *current =
            D3DRMCreateColorRGB
            (   D3DVAL(GetRValue(cc.rgbResult) / D3DVAL(255.0)),
                D3DVAL(GetGValue(cc.rgbResult) / D3DVAL(255.0)),
                D3DVAL(GetBValue(cc.rgbResult) / D3DVAL(255.0))
            );
        return TRUE;
    }
    else return FALSE;
}
