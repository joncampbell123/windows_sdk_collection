#ifndef ___RLDS3D_HEADER_FILE
#define ___RLDS3D_HEADER_FILE

#include "d3drmwin.h"

/*
** This is an interface between the viewer and the RL/DS3D APIs which provides the
**   functionality required by the viewer in simplified form.
**
** Note: This is not an object-oriented API since there should only be need for one
**   copy at a time and this way C++ to C conversions should be fairly easy
**
** DESCRIPTION: The user interfaces the 3D world and objects therein by selecting items
**   with the mouse (on the screen) and performing operations on that item.  The user will
**   also be able to perform operations on the camera and a few global operations applied
**   to all objects.
**
** USAGE: Create the interface using RLDS3D_Initialize() (returns FALSE if not created)
**   and remove it using RLDS3D_Deinitialize().  The RLDS3D_Render() functionality draws the world
**   on the screen.
*/

/*
************************  INITIALIZATION/DEINITIALIZATION ****************************
*/

/*
** Initialize will attach itself to the passed window.  Call initialize after
**  getting a handle for your window but before you display it.
**
** Initialize will initialize the RL API and return false if it fails.  It will
**  also attempt to initialize a DirectSound3D API, but failing this does not
**  justify a failed Initialize (since the 3D sound isn't a necessary part of
**  the viewer)
*/

BOOL RLDS3D_Initialize(HWND hwndPW, HANDLE this_inst);

/*
** Deinitializes where necessary (assumes program quits after calling this otherwise it'd free up memory, etc...)
*/

void RLDS3D_Deinitialize();

/*
***************************  ADDING/REMOVING/EDITING OBJECTS  *********************************
*/

/*
** Loads XOF file into the RL world (with textures)
*/

void RLDS3D_LoadXOF(char* file);

/*
** Sets/Gets whether or not boxes are shown around selected item
*/

BOOL RLDS3D_GetBoxes(void);
void RLDS3D_SetBoxes(BOOL new_val);

/*
** Updates the bounding box around the selected visual (this could be done using a render callback function to compare the
**   frame's scaling and transform functions instead)
*/

void RLDS3D_UpdateSelectionBox(void);

/*
** Deselects the currently selected 3D visual.
*/

void RLDS3D_DeselectVisual();

/*
** Given coordinates it selects the first visual under those coordinates in the window's viewport
*/

void RLDS3D_FindAndSelectVisual(int x, int y, LPBOOL changed = NULL);

/*
** Cuts the current selection to the clipboard
*/

void RLDS3D_CutVisual();

/*
** Copies the current selection to the clipboard
*/

void RLDS3D_CopyVisual();

/*
** Pastes the current selection to the window
*/

void RLDS3D_PasteVisual();

/*
** Deletes the current selection from the world without copying to the clipboard
*/

void RLDS3D_DeleteVisual();

/*
** Add a directional light
*/
void RLDS3D_AddDirectionalLight();

/*
** Add a parallel point light
*/
void RLDS3D_AddParallelPointLight();

/*
** Add Point Light
*/
void RLDS3D_AddPointLight();

/*
** Add a spotlight
*/
void RLDS3D_AddSpotlight();

/*
***********************************  OBJECT MOTION/SCALING/COLOURING  ****************************************
*/

/*
** Sets the selected object's colour
*/

void RLDS3D_SetSelColour();

/*
** Moves the camera relative to itself by providing scalars to multiply against the CAMERA-RELATIVE unit vectors
**   forwards/up/right.
*/

void RLDS3D_SetCamVelRelToCam(D3DVALUE forward, D3DVALUE up, D3DVALUE right);

/*
** Rotates the camera around its three axis
**
** forward_axis is roll, up_axis is yaw, right_axis is pitch
** (Only one affects at a time)
*/

void RLDS3D_SetCamRotForward(D3DVALUE forward_axis);
void RLDS3D_SetCamRotUp(D3DVALUE up_axis);
void RLDS3D_SetCamRotRight(D3DVALUE right_axis);

/*
** Scales the selected object in the x/y/z axis of it's orientation by the specified values
*/

void RLDS3D_ScaleSelected(D3DVALUE sx, D3DVALUE sy, D3DVALUE sz);

/*
** Moves the currently selected object in the 3D world relative to the camera
*/

void RLDS3D_SetSelectedVelRelToCam(D3DVALUE forward, D3DVALUE up, D3DVALUE right);

/*
** Rotates the currently selected object relative to the camera's frame when passed
**   a vector for the axis and an angle of rotation.
** This is useful because the AXIS which is specified is relative to what appears on the screen with it's origin at the camera
**   (ie: (0,0,1) will be an axis straight into the screen and thus things will spin around the centre of the screen)
*/

void RLDS3D_SetSelectedRotRelToCam(D3DVALUE AxisX, D3DVALUE AxisY, D3DVALUE AxisZ, D3DVALUE angle);

/*
** Moves the currently selected object by x/y pixels on the screen from it's relative position.  Allows a user
**   to drag objects around the screen using the mouse.  Assumes that the distance from the frame to the screen
**   will remain constant
*/

void RLDS3D_MoveSelectedPosByScreenCoords(double delta_x, double delta_y);

/*
** Orbits the selected object around the camera
*/

void RLDS3D_OrbitSelected(void);
void RLDS3D_StopOrbitSelected();

/*
** Bullets it towards the camera
*/

void RLDS3D_BulletSelected(void);

/*
**********************  DIRECTSOUND 3D INTERFACE  ******************************
*/

/*
** Stops all sounds from playing
*/

void RLDS3D_StopAllSounds();

/*
** Removes all of the sounds
*/

void RLDS3D_RemoveAllSounds();

/*
** Plays the sound associated with the currently selected object
*/

void RLDS3D_PlaySound(BOOL bIsLooping);

/*
** Stops the sound associated with the currently selected object
*/

void RLDS3D_StopSelectedSound();

/*
** Removes the sound from the currently selected object
*/

void RLDS3D_RemoveSound();
/*
** Attaches a sound (filename provided) to the selected frame
*/

void RLDS3D_AttachSound(char* filename);

/*
** Global parameter mods
*/
void RLDS3D_GetDistanceFactor(D3DVALUE *temp);
void RLDS3D_GetDopplerFactor(D3DVALUE *temp);
void RLDS3D_GetRolloffFactor(D3DVALUE *temp);
void RLDS3D_SetDistanceFactor(D3DVALUE temp);
void RLDS3D_SetDopplerFactor(D3DVALUE temp);
void RLDS3D_SetRolloffFactor(D3DVALUE temp);

void RLDS3D_CommitDeferredSettings(void);

/*
** Selected sound parameter modifications
*/

BOOL RLDS3D_SoundSelected(void);
void RLDS3D_GetSelConeAngles(LPDWORD inner, LPDWORD outer);
void RLDS3D_GetSelConeOutsideVolume(LPLONG temp);
void RLDS3D_GetSelMinimumDistance(D3DVALUE *temp);
void RLDS3D_GetSelMaximumDistance(D3DVALUE *temp);	
void RLDS3D_SetSelConeAngles(DWORD inner, DWORD outer);
void RLDS3D_SetSelConeOutsideVolume(LONG temp);
void RLDS3D_SetSelMinimumDistance(D3DVALUE temp);
void RLDS3D_SetSelMaximumDistance(D3DVALUE temp);

/*
*************************************  MISC. MAINTENANCE FUNCTIONS  **************************************
*/

/*
** Allows external users access to the RL Device to deal with Windows-related issues
**   (See case WM_ACTIVATE: and case WM_PAINT: in viewer source for examples of HandleActivate() and HandlePaint())
** Design note: This was done to save time from the conversion from the old version of the viewer rather than
**   having RLDS3D_HandleActivate(), etc.
*/

LPDIRECT3DRMDEVICE RLDS3D_WinDevice();

// Handles window activation (pass the wparam from the winproc)
void RLDS3D_HandleActivate(WPARAM wparam);

// Handles paint messages from the window.  Pass this one the paintstructure created using BeginPaint
void RLDS3D_HandlePaint(PAINTSTRUCT* ps);

// Tells whether or not something is currently selected

BOOL RLDS3D_FrameSelected();

/*
** Renders the scene's next frame into the viewport.
*/

void RLDS3D_Render(D3DVALUE time_delta);

/*
 * Resize the viewport and device when the window size changes.
 */

void RLDS3D_ResizeViewport(int width, int height);

/*
** Returns whether or not the 3D Sound API was actually initialized properly
*/

BOOL RLDS3D_SoundInitialized();

/*
** Sets/Gets the current polygon fill mode
*/

D3DRMFILLMODE RLDS3D_GetPolygonFillMode(void);
void RLDS3D_SetPolygonFillMode(D3DRMFILLMODE quality);

/*
** Sets/Gets the current polygon shade mode
*/

void RLDS3D_SetPolygonShadeMode(D3DRMSHADEMODE quality);
D3DRMSHADEMODE RLDS3D_GetPolygonShadeMode(void);

/*
** Sets/Gets the colour model for the viewport (RGB or ramp)
*/

D3DRMCOLORMODEL RLDS3D_GetColourModel(void);
void RLDS3D_SetColourModel(D3DRMCOLORMODEL model);

/*
** Sets/Gets whether or not the lights affect the visuals
*/

void RLDS3D_SetLighting(BOOL new_val);
BOOL RLDS3D_GetLighting(void);

/*
** Sets/Gets whether or not dithering is on
*/

BOOL RLDS3D_GetDither(void);
void RLDS3D_SetDither(BOOL dither);

/*
** Sets/Gets texture quality (only relevant for RGB modes)
*/

D3DRMTEXTUREQUALITY RLDS3D_GetTextureQuality(void);
void RLDS3D_SetTextureQuality(D3DRMTEXTUREQUALITY new_quality);

#endif


