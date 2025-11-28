/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File: viewer.cpp
 *
 ***************************************************************************/



#define INITGUID
#include "d3drmwin.h"
#include "resource.h"

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <direct.h>

#include "rlds3d.h"
#include "file.h"


static char ViewerClass[32] = "ViewerClass";

static BOOL FirstInstance(HANDLE);
static BOOL AnyInstance(HANDLE, HWND*);
long FAR PASCAL WindowProc(HWND, UINT, WPARAM, LPARAM);
static void Idle();

BOOL CALLBACK WaitBoxDlgProc(HWND, UINT, WPARAM, LPARAM);

BOOL AboutBoxOn = FALSE;


/*
 * Initialization, message loop
 */

int PASCAL WinMain
    (HANDLE this_inst, HANDLE prev_inst, LPSTR cmdline, int cmdshow)
{
    MSG         msg;
    int         idle;
    int         done = FALSE;
    HACCEL      accel;


        DWORD           prev_tick;
        DWORD           this_tick;
        D3DVALUE        time_delta;

    prev_inst = prev_inst;
    cmdline = cmdline;

        // Register our class if necessary
    if (!prev_inst) {
                // Do the stuff for if this is the first instance of the program.
                if (!FirstInstance(this_inst)) return FALSE;
        }

        // Create our window and get it's handle (needs to be done whether we're first or not)
        HWND win;
    if (!AnyInstance(this_inst, &win)) return FALSE;

        // Initialize the RLDS3D interface
        if (!RLDS3D_Initialize(win, this_inst)) {
                // If it doesn't initialize then we don't have anything we need to clean up, so we just exit
                exit(-1);
        }

        // Show our window!
    ShowWindow(win, cmdshow);
    UpdateWindow(win);

        // Load accelerators
    accel = LoadAccelerators(this_inst, "ViewerAccel");

        prev_tick = timeGetTime();


    while (!done)
    {   idle = TRUE;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {   idle = FALSE;
            if (msg.message == WM_QUIT)
            {   done = TRUE;
                break;
            }

            if (!TranslateAccelerator(msg.hwnd, accel, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }

        }
                if (idle) Idle();
                this_tick = timeGetTime();
                time_delta = D3DVAL((float)((this_tick - prev_tick)/1000.0));
                prev_tick = this_tick;
                RLDS3D_Render(time_delta);
    }
        RLDS3D_Deinitialize();
        return msg.wParam;
}

/*
 * Register window class for the application, and do any other
 * application initialization
 */
static BOOL FirstInstance(HANDLE this_inst)
{
    WNDCLASS    wc;
    BOOL        rc;

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = this_inst;
    wc.hIcon = LoadIcon(this_inst, "ViewerIcon");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = "ViewerMenu";
    wc.lpszClassName = ViewerClass;
    rc = RegisterClass(&wc);

    return rc;
}

/*
 * Do work required for every instance of the application:
 * create the window, initialize data
 */
static BOOL AnyInstance(HANDLE this_inst, HWND* window_handle)
{
    HWND win;
    char szCaption[] = "DirectSound3D Demo";

    /*
     * create main window
     */
    win =
        CreateWindow
        (   ViewerClass,                /* class */
            szCaption,                  /* caption */
            WS_OVERLAPPEDWINDOW,        /* style */
            CW_USEDEFAULT,              /* init. x pos */
            CW_USEDEFAULT,              /* init. y pos */
            400,                        /* init. x size */
            400,                        /* init. y size */
            NULL,                       /* parent window */
            NULL,                       /* menu handle */
            this_inst,                  /* program handle */
            NULL                        /* create parms */
        );


    if (!win) return FALSE;
        *window_handle = win;
    return TRUE;
}

/*
 * Processes messages for the about dialog.
 */
BOOL FAR PASCAL AboutDlgProc
    (HWND win, unsigned msg, WORD wparam, LONG lparam)
{
    lparam = lparam;

    switch (msg)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (wparam == IDOK)
        {
            AboutBoxOn = FALSE;
            EndDialog(win, TRUE);
            return TRUE;
        }
        break;
    case WM_CLOSE:
        {
            AboutBoxOn = FALSE;
            EndDialog(win, TRUE);
            return FALSE;
        }
        break;
    }
    return FALSE;
}

static int
    left_drag = FALSE, right_drag = FALSE,
    last_x, last_y;

static void Idle()
{
        // Stop selected item being dragged from rotating/moving
    if (left_drag) {
                RLDS3D_SetSelectedRotRelToCam(D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0));
        }
    if (right_drag) {
                RLDS3D_SetSelectedVelRelToCam(D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0));
        }               
}

static int FillModeToMenuItem(D3DRMFILLMODE fm)
{
    switch (fm) {
    case D3DRMFILL_POINTS:
        return 2;

    case D3DRMFILL_WIREFRAME:
        return 3;

    case D3DRMFILL_SOLID:
        return 4;
    }
    return -1;
}

// Sets the polygon fill mode and checks/unchecks the appropriate menu items
static void SetFillMode(HWND win, D3DRMFILLMODE fm)
{
    HMENU menu;
    D3DRMFILLMODE oldfm = RLDS3D_GetPolygonFillMode();
    menu = GetMenu(win);
    menu = GetSubMenu(menu, 2);
    CheckMenuItem(menu, FillModeToMenuItem(oldfm),
                  MF_BYPOSITION|MF_UNCHECKED);
    CheckMenuItem(menu, FillModeToMenuItem(fm),
                  MF_BYPOSITION|MF_CHECKED);
        RLDS3D_SetPolygonFillMode(fm);
}

static int ShadeModeToMenuItem(D3DRMSHADEMODE sm)
{
    switch (sm) {
    case D3DRMSHADE_FLAT:
        return 6;

    case D3DRMSHADE_GOURAUD:
        return 7;

    case D3DRMSHADE_PHONG:
        return 8;
    }
    return -1;
}

// Sets the polygon shade mode and checks/unchecks the appropriate menu items
static void SetShadeMode(HWND win, D3DRMSHADEMODE sm)
{
    HMENU menu;
    D3DRMSHADEMODE oldsm = RLDS3D_GetPolygonShadeMode();

    menu = GetMenu(win);
    menu = GetSubMenu(menu, 2);
    CheckMenuItem(menu, ShadeModeToMenuItem(oldsm),
                  MF_BYPOSITION|MF_UNCHECKED);
    CheckMenuItem(menu, ShadeModeToMenuItem(sm),
                  MF_BYPOSITION|MF_CHECKED);
        RLDS3D_SetPolygonShadeMode(sm);
}

// Sets the rendering model and checks/unchecks the appropriate menu items
static void SetModel(HWND win, D3DRMCOLORMODEL model)
{
    HMENU menu;
    D3DRMCOLORMODEL oldModel = RLDS3D_GetColourModel();

    if (oldModel == model) return;

    RLDS3D_SetColourModel(model);
    
    menu = GetMenu(win);
    menu = GetSubMenu(menu, 2);
    CheckMenuItem(menu, 9 + (int) oldModel, MF_BYPOSITION|MF_UNCHECKED);
    CheckMenuItem(menu, 9 + (int) model, MF_BYPOSITION|MF_CHECKED);
}

// Toggles dithering and checks/unchecks menu item
static void ToggleDither(HWND win)
{
    HMENU menu;
    int dither = RLDS3D_GetDither();
    int checked;
    dither = !dither;
        RLDS3D_SetDither(dither);

    menu = GetMenu(win);
    menu = GetSubMenu(menu, 2);

    if (dither) checked = MF_CHECKED;
    else checked = MF_UNCHECKED;

    CheckMenuItem(menu, 13, MF_BYPOSITION|checked);
}

// Toggles texture filtering and checks/unchecks menu item
static void ToggleTextureFiltering(HWND win)
{
    HMENU menu;
    D3DRMTEXTUREQUALITY tq = RLDS3D_GetTextureQuality();
    int checked;
    if (tq == D3DRMTEXTURE_NEAREST)
        tq = D3DRMTEXTURE_LINEAR;
    else
        tq = D3DRMTEXTURE_NEAREST;

    RLDS3D_SetTextureQuality(tq);
    menu = GetMenu(win);
    menu = GetSubMenu(menu, 2);

    if (tq == D3DRMTEXTURE_LINEAR) checked = MF_CHECKED;
    else checked = MF_UNCHECKED;

    CheckMenuItem(menu, 14, MF_BYPOSITION|checked);
}

// Ditto for lighting
static void ToggleLighting(HWND win)
{
    HMENU menu;
    menu = GetMenu(win);
    menu = GetSubMenu(menu, 2);
    if (RLDS3D_GetLighting()) {
            CheckMenuItem(menu, 0, (MF_BYPOSITION | MF_UNCHECKED));
                RLDS3D_SetLighting(FALSE);
        }
        else {
            CheckMenuItem(menu, 0, (MF_BYPOSITION | MF_CHECKED));
                RLDS3D_SetLighting(TRUE);
        }
}

/*
** Windows procs for the sound param dlg boxes
*/

BOOL CALLBACK WaitBoxDlgProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam) {
        return FALSE;
};

BOOL CALLBACK GlobalParamDlgProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam)
{
    lparam = lparam;

    char lpszTS[100];

    switch (msg)
    {
                case WM_INITDIALOG:
                        {
                                // Set up our edit fields appropriately
                D3DVALUE temp;
                                RLDS3D_GetDistanceFactor(&temp);
                                sprintf(lpszTS, "%f",temp);
                                SendDlgItemMessage(win, IDC_DISTANCEFACTOR, WM_SETTEXT, 0, (LPARAM) lpszTS);
                                SendDlgItemMessage(win, IDC_DISTANCEFACTOR, EM_SETLIMITTEXT, 10, 0);
                                RLDS3D_GetRolloffFactor(&temp);
                                sprintf(lpszTS, "%f",temp);
                                SendDlgItemMessage(win, IDC_ROLLOFFFACTOR, WM_SETTEXT, 0, (LPARAM) lpszTS);
                                SendDlgItemMessage(win, IDC_ROLLOFFFACTOR, EM_SETLIMITTEXT, 10, 0);
                                RLDS3D_GetDopplerFactor(&temp);
                                sprintf(lpszTS, "%f",temp);
                                SendDlgItemMessage(win, IDC_DOPPLERFACTOR, WM_SETTEXT, 0, (LPARAM) lpszTS);
                                SendDlgItemMessage(win, IDC_DOPPLERFACTOR, EM_SETLIMITTEXT, 10, 0);
                                return TRUE;
                        }
                case WM_CLOSE:
                        EndDialog(win, TRUE);
                        return FALSE;

                case WM_COMMAND:
            {
                if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_DOPPLERFACTOR) {
                                        // Get value from edit box (get length, check lenth, set first char to length, request string, end it with 0)
                    int stringlength = SendDlgItemMessage(win, IDC_DOPPLERFACTOR, EM_LINELENGTH, 0, 0);
                    if (stringlength > 10) return TRUE;
                    lpszTS[0] = (char)stringlength;
                    SendDlgItemMessage(win, IDC_DOPPLERFACTOR, EM_GETLINE, 0, (LPARAM) lpszTS);
                    lpszTS[stringlength] = 0;
                                        // Set value in RLDS3D (convert string to long and go crazy with the bad boy)
                                        // No real need for error checking 'cause the edit field is numbers only
                                        RLDS3D_SetDopplerFactor(D3DVAL(atof(lpszTS)));
                    return TRUE;
                }
                else if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_ROLLOFFFACTOR) {
                                        // Get value from edit box (get length, check lenth, set first char to length, request string, end it with 0)
                    int stringlength = SendDlgItemMessage(win, IDC_ROLLOFFFACTOR, EM_LINELENGTH, 0, 0);
                    if (stringlength > 10) return TRUE;
                    lpszTS[0] = (char)stringlength;
                    SendDlgItemMessage(win, IDC_ROLLOFFFACTOR, EM_GETLINE, 0, (LPARAM) lpszTS);
                    lpszTS[stringlength] = 0;
                                        // Set value in RLDS3D (convert string to long and go crazy with the bad boy)
                                        RLDS3D_SetRolloffFactor(D3DVAL(atof(lpszTS)));
                    return TRUE;
                }
                else if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_DISTANCEFACTOR) {
                                        // Get value from edit box (get length, check lenth, set first char to length, request string, end it with 0)
                    int stringlength = SendDlgItemMessage(win, IDC_DISTANCEFACTOR, EM_LINELENGTH, 0, 0);
                    if (stringlength > 10) return TRUE;
                    lpszTS[0] = (char)stringlength;
                    SendDlgItemMessage(win, IDC_DISTANCEFACTOR, EM_GETLINE, 0, (LPARAM) lpszTS);
                    lpszTS[stringlength] = 0;
                                        // Set value in RLDS3D (convert string to long and go crazy with the bad boy)
                                        RLDS3D_SetDistanceFactor(D3DVAL(atof(lpszTS)));
                    return TRUE;
                }
                else return FALSE;
            }               
        break;
    }
    return FALSE;
}

HWND hSelCfgDlg = 0;

BOOL CALLBACK SelectedParamDlgProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam)
{
    lparam = lparam;

    char lpszTS[100];

    switch (msg)
    {
                case WM_INITDIALOG:
                        {
                                // Set up our edit fields appropriately
                D3DVALUE temp;
                                LONG temp2;
                                DWORD temp3, temp4;
                                // Ignores the out side cone angle for now, which is fine, because elsewhere we
                                // set it to equal the inside angle
                                RLDS3D_GetSelConeAngles(&temp3, &temp4);
                                sprintf(lpszTS, "%i",temp3);
                                SendDlgItemMessage(win, IDC_CONEANGLE, WM_SETTEXT, 0, (LPARAM) lpszTS);
                                SendDlgItemMessage(win, IDC_CONEANGLE, EM_SETLIMITTEXT, 10, 0);
                                RLDS3D_GetSelConeOutsideVolume(&temp2);
                                sprintf(lpszTS, "%i",temp2);
                                SendDlgItemMessage(win, IDC_CONEOUTSIDEVOLUME, WM_SETTEXT, 0, (LPARAM) lpszTS);
                                SendDlgItemMessage(win, IDC_CONEOUTSIDEVOLUME, EM_SETLIMITTEXT, 10, 0);
                                RLDS3D_GetSelMaximumDistance(&temp);
                                sprintf(lpszTS, "%f",temp);
                                SendDlgItemMessage(win, IDC_MAXIMUMDISTANCE, WM_SETTEXT, 0, (LPARAM) lpszTS);
                                SendDlgItemMessage(win, IDC_MAXIMUMDISTANCE, EM_SETLIMITTEXT, 10, 0);
                                RLDS3D_GetSelMinimumDistance(&temp);
                                sprintf(lpszTS, "%f",temp);
                                SendDlgItemMessage(win, IDC_MINIMUMDISTANCE, WM_SETTEXT, 0, (LPARAM) lpszTS);
                                SendDlgItemMessage(win, IDC_MINIMUMDISTANCE, EM_SETLIMITTEXT, 10, 0);
                                hSelCfgDlg = win;
                                return TRUE;
                        }

                case WM_CLOSE:
                        EndDialog(win, TRUE);
                        hSelCfgDlg = 0;
                        return FALSE;

                case WM_COMMAND:
            {
                if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_CONEANGLE) {
                                        // Get value from edit box (get length, check lenth, set first char to length, request string, end it with 0)
                    int stringlength = SendDlgItemMessage(win, IDC_CONEANGLE, EM_LINELENGTH, 0, 0);
                    if (stringlength > 10) return TRUE;
                    lpszTS[0] = (char)stringlength;
                    SendDlgItemMessage(win, IDC_CONEANGLE, EM_GETLINE, 0, (LPARAM) lpszTS);
                    lpszTS[stringlength] = 0;
                                        // Set value in RLDS3D (convert string to long and go crazy with the bad boy)
                                        // No real need for error checking 'cause the edit field is numbers only
                                        RLDS3D_SetSelConeAngles((DWORD)atol(lpszTS), (DWORD)atol(lpszTS));
                    return TRUE;
                }
                else if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_CONEOUTSIDEVOLUME) {
                                        // Get value from edit box (get length, check lenth, set first char to length, request string, end it with 0)
                    int stringlength = SendDlgItemMessage(win, IDC_CONEOUTSIDEVOLUME, EM_LINELENGTH, 0, 0);
                    if (stringlength > 10) return TRUE;
                    lpszTS[0] = (char)stringlength;
                    SendDlgItemMessage(win, IDC_CONEOUTSIDEVOLUME, EM_GETLINE, 0, (LPARAM) lpszTS);
                    lpszTS[stringlength] = 0;
                                        // Set value in RLDS3D (convert string to long and go crazy with the bad boy)
                                        RLDS3D_SetSelConeOutsideVolume(atol(lpszTS));
                    return TRUE;
                }
                else if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_MAXIMUMDISTANCE) {
                                        // Get value from edit box (get length, check lenth, set first char to length, request string, end it with 0)
                    int stringlength = SendDlgItemMessage(win, IDC_MAXIMUMDISTANCE, EM_LINELENGTH, 0, 0);
                    if (stringlength > 10) return TRUE;
                    lpszTS[0] = (char)stringlength;
                    SendDlgItemMessage(win, IDC_MAXIMUMDISTANCE, EM_GETLINE, 0, (LPARAM) lpszTS);
                    lpszTS[stringlength] = 0;
                                        // Set value in RLDS3D (convert string to long and go crazy with the bad boy)
                                        RLDS3D_SetSelMaximumDistance(D3DVAL(atof(lpszTS)));
                    return TRUE;
                }
                else if (HIWORD(wparam) == EN_UPDATE && LOWORD(wparam) == IDC_MINIMUMDISTANCE) {
                                        // Get value from edit box (get length, check lenth, set first char to length, request string, end it with 0)
                    int stringlength = SendDlgItemMessage(win, IDC_MINIMUMDISTANCE, EM_LINELENGTH, 0, 0);
                    if (stringlength > 10) return TRUE;
                    lpszTS[0] = (char)stringlength;
                    SendDlgItemMessage(win, IDC_MINIMUMDISTANCE, EM_GETLINE, 0, (LPARAM) lpszTS);
                    lpszTS[stringlength] = 0;
                                        // Set value in RLDS3D (convert string to long and go crazy with the bad boy)
                                        RLDS3D_SetSelMinimumDistance(D3DVAL(atof(lpszTS)));
                    return TRUE;
                }
                else return FALSE;
            }               
        break;
    }
    return FALSE;
}

#define SIGN_EXTEND(w)    ((((int)(w)) << 16) >> 16)

/*
 * Handle messages for the main application window
 */

LONG FAR PASCAL WindowProc(HWND win, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HANDLE inst_handle;
    static HCURSOR oldCursor = NULL;

    switch (msg)
    {
    case WM_KEYDOWN:
        {   
            switch (wparam)
            {
                        
            case 'A':
                                RLDS3D_SetCamVelRelToCam(D3DVAL(5.0), D3DVAL(0.0), D3DVAL(0.0));
                break;

            case 'Z':
                                RLDS3D_SetCamVelRelToCam(D3DVAL(-5.0), D3DVAL(0.0), D3DVAL(0.0));
                break;

            case VK_RIGHT:
                                RLDS3D_SetCamRotUp(D3DVAL(1.0));
                break;
            case VK_LEFT:
                                RLDS3D_SetCamRotUp(D3DVAL(-1.0));
                break;
                        case VK_UP:
                                RLDS3D_SetCamRotRight(D3DVAL(1.0));
                                break;
                        case VK_DOWN:
                                RLDS3D_SetCamRotRight(D3DVAL(-1.0));
                                break;
                        }
        }
        break;

    case WM_KEYUP:
        switch (wparam)
        {
                case VK_LEFT:
                case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
                        // Stop the camera from rotating completely
                        RLDS3D_SetCamRotForward(D3DVAL(0.0));
                        break;
            
                case 'A':
                case 'Z':
                        RLDS3D_SetCamVelRelToCam(D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0));
            break;
        }
        break;

    case WM_LBUTTONDOWN:
        {   HCURSOR hCur;
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            last_x = x;
            last_y = y;
                        BOOL bChanged = FALSE;
            RLDS3D_FindAndSelectVisual(x, y, &bChanged);
                        // Get rid of the Selected Object Cfg dialog if we've selected a new object
                        if (bChanged && hSelCfgDlg) EndDialog(hSelCfgDlg, TRUE);
            left_drag = TRUE;
            SetCapture(win);
            /* change to a groovy cursor */
            hCur = LoadCursor(NULL, IDC_ARROW);
            if (hCur) oldCursor = SetCursor(hCur);
            else oldCursor = NULL;
        }
        break;

    case WM_LBUTTONUP:
        ReleaseCapture();
        left_drag = FALSE;
        if (oldCursor) SetCursor(oldCursor);
        break;

    case WM_RBUTTONDOWN:
        {
            HCURSOR hCur;
            int x = LOWORD(lparam);
            int y = HIWORD(lparam);
            last_x = x;
            last_y = y;
                        BOOL bChanged = FALSE;
            RLDS3D_FindAndSelectVisual(x, y, &bChanged);
                        // Get rid of the Selected Object Cfg dialog if we've selected a new object
                        if (bChanged && hSelCfgDlg) EndDialog(hSelCfgDlg, TRUE);
            right_drag = TRUE;
            SetCapture(win);
            /* change to a groovy cursor */
            hCur = LoadCursor(NULL, IDC_ARROW);
            if (hCur) oldCursor = SetCursor(hCur);
            else oldCursor = NULL;
        }
        break;

    case WM_RBUTTONUP:
        right_drag = FALSE;
        ReleaseCapture();
        if (oldCursor) SetCursor(oldCursor);
        break;

    case WM_MOUSEMOVE:
        if ((wparam & MK_LBUTTON) && left_drag)
        {   double delta_x, delta_y;
            delta_x = SIGN_EXTEND(LOWORD(lparam)) - last_x;
            delta_y = -SIGN_EXTEND((HIWORD(lparam)) - last_y);
            last_x = SIGN_EXTEND(LOWORD(lparam));
            last_y = SIGN_EXTEND(HIWORD(lparam));
            {
                double delta_r = sqrt(delta_x * delta_x + delta_y * delta_y);
                double radius = 50;
                double denom;

                denom = 0.05 * sqrt(radius * radius + delta_r * delta_r);

                if (delta_r == 0 || denom == 0) break;
                                RLDS3D_SetSelectedRotRelToCam(D3DDivide(D3DVAL((float) delta_y), D3DVAL((float) delta_r)),
                                                                                          D3DDivide(D3DVAL((float) -delta_x), D3DVAL((float) delta_r)),
                                                                                          D3DVAL(0.0),
                                                                                          D3DDivide(D3DVAL((float) delta_r), D3DVAL((float) denom)));
            }
        }
        else if ((wparam & MK_RBUTTON) && right_drag)
        {   double delta_x, delta_y;

            delta_x = SIGN_EXTEND(LOWORD(lparam)) - last_x;
            delta_y = SIGN_EXTEND(HIWORD(lparam)) - last_y;
            last_x = SIGN_EXTEND(LOWORD(lparam));
            last_y = SIGN_EXTEND(HIWORD(lparam));
                        RLDS3D_MoveSelectedPosByScreenCoords(delta_x, delta_y);
        }
        break;
    case WM_COMMAND:
        switch( wparam & 0xffff )
        {
                case MENU_MOTION_ORBITSELECTEDOBJECT:
                        {
                                RLDS3D_OrbitSelected();
                        }
                        break;
                case MENU_MOTION_BULLETSELECTEDOBJECT:
                        {
                                RLDS3D_BulletSelected();
                        }
                        break;
                case MENU_SOUND_ATTACHSOUND:
                        {
                                if (RLDS3D_SoundInitialized())
                                {
                                        if (RLDS3D_FrameSelected()) {
                                                char* file = OpenNewSoundFile(win);
                                                if (file) RLDS3D_AttachSound(file);                                     
                                        }
                                        else MessageBox(win, "No object selected to attach a sound to!", "Doh!", MB_ICONEXCLAMATION);
                                }
                                else MessageBox(win, "Sound not initialized!", "Doh!", MB_ICONEXCLAMATION);
                        }
                        break;
                case MENU_SOUND_REMOVESOUND:
                        RLDS3D_RemoveSound();
                        break;
                case MENU_SOUND_REMOVEALLSOUNDS:
                        RLDS3D_RemoveAllSounds();
                        break;

                case MENU_SOUND_PLAYSOUND:
                        RLDS3D_PlaySound(FALSE);
                        break;
                case MENU_SOUND_PLAYSOUNDLOOPING:
                        RLDS3D_PlaySound(TRUE);
                        break;
                case MENU_SOUND_STOPSOUND:
                        RLDS3D_StopSelectedSound();
                        break;
                
                case MENU_SOUND_STOPALLSOUNDS:
                        RLDS3D_StopAllSounds();
                        break;

                case MENU_SOUND_SELECTEDSOUNDPROPERTIES:
                        {
                                if (RLDS3D_SoundInitialized()) {
                                        if (RLDS3D_SoundSelected()) {
                                                inst_handle = (HANDLE) GetWindowLong(win, GWL_HINSTANCE);
                                                HWND mydial = CreateDialog(inst_handle, "SelectedParamBox", win, (int(__stdcall*)(void))SelectedParamDlgProc);
                                                if (mydial) {
                                                        ShowWindow(mydial, SW_SHOW);
                                                        UpdateWindow(mydial);
                                                }
                                        }
                                        else MessageBox(win, "No sound currently selected", "Doh!", MB_ICONEXCLAMATION);
                                }
                                else MessageBox(win, "Sound not initialized!", "Doh!", MB_ICONEXCLAMATION);
                        }
                        break;
                case MENU_SOUND_GLOBALPROPERTIES:
                        {
                                if (RLDS3D_SoundInitialized()) {
                                        inst_handle = (HANDLE) GetWindowLong(win, GWL_HINSTANCE);
                                        HWND mydial = CreateDialog(inst_handle, "GlobalParamBox", win, (int(__stdcall*)(void))GlobalParamDlgProc);
                                        if (mydial) {
                                                ShowWindow(mydial, SW_SHOW);
                                                UpdateWindow(mydial);
                                        }
                                }
                                else MessageBox(win, "Sound not initialized!", "Doh!", MB_ICONEXCLAMATION);
                        }
                        break;

                case MENU_FILE_ABOUT:
                        {
                                inst_handle = (HANDLE) GetWindowLong(win, GWL_HINSTANCE);
                                DialogBox(inst_handle,"AboutBox", win, (int(__stdcall*)(void))AboutDlgProc);
                        }
            break;

        case MENU_FILE_OPEN:
            {   
                char *file = OpenNewFile(win);
                                if (file) {
                                        RLDS3D_LoadXOF(file);
                }
            }
            break;

        case MENU_FILE_EXIT:
            PostQuitMessage(0);
            break;

        case MENU_EDIT_CUT:
            RLDS3D_CutVisual();
            break;

        case MENU_EDIT_COPY:
            RLDS3D_CopyVisual();
            break;

        case MENU_EDIT_PASTE:
            RLDS3D_PasteVisual();
            break;

        case MENU_EDIT_DELETE:
            RLDS3D_DeleteVisual();
            break;

        case MENU_EDIT_COLOUR:
                        // Currently everything is handled there but the selection wheel maybe should be out here?
                        RLDS3D_SetSelColour();
            break;

        case MENU_EDIT_BOXES:
            {
                HMENU menu;
                                if (RLDS3D_GetBoxes() == TRUE) RLDS3D_SetBoxes(FALSE); else RLDS3D_SetBoxes(TRUE);
                int checked = (RLDS3D_GetBoxes()==TRUE) ? MF_CHECKED : MF_UNCHECKED;
                menu = GetMenu(win);
                menu = GetSubMenu(menu, 1);
                CheckMenuItem(menu, MENU_EDIT_BOXES, MF_BYCOMMAND|checked);
            }
            break;

                case MENU_QUALITY_LIGHTING:
                        ToggleLighting(win);
                        break;

                // Fill Modes
                case MENU_QUALITY_POINTS:
                        SetFillMode(win, D3DRMFILL_POINTS);
                        break;

        case MENU_QUALITY_WIREFRAME:
            SetFillMode(win, D3DRMFILL_WIREFRAME);
            break;

        case MENU_QUALITY_SOLID:
            SetFillMode(win, D3DRMFILL_SOLID);
            break;

                // Shading modes
        case MENU_QUALITY_FLAT:
            SetShadeMode(win, D3DRMSHADE_FLAT);
            break;

        case MENU_QUALITY_GOURAUD:
            SetShadeMode(win, D3DRMSHADE_GOURAUD);
            break;

        case MENU_QUALITY_PHONG:
            SetShadeMode(win, D3DRMSHADE_PHONG);
            break;

                // Color models
        case MENU_MODEL_MONO:
            SetModel(win, D3DCOLOR_MONO);
            break;

        case MENU_MODEL_RGB:
            SetModel(win, D3DCOLOR_RGB);
            break;

        case MENU_DITHER:
            ToggleDither(win);
            break;

        case MENU_TEXTURE_FILTERING:
            ToggleTextureFiltering(win);
            break;

        case MENU_LIGHT_DIRECTIONAL:
                        RLDS3D_AddDirectionalLight();
                        break;
        case MENU_LIGHT_PARALLEL_POINT:
                        RLDS3D_AddParallelPointLight();
                        break;
        case MENU_LIGHT_POINT:
                        RLDS3D_AddPointLight();
                        break;
        case MENU_LIGHT_SPOT:
                        RLDS3D_AddSpotlight();
                        break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;

    case WM_SIZE:
        {
            int width = LOWORD(lparam);
            int height = HIWORD(lparam);
            RLDS3D_ResizeViewport(width, height);
        }
        break;

    case WM_ACTIVATE:
                {
                        RLDS3D_HandleActivate(wparam);
                }
        break;

    case WM_PAINT:
                if (RLDS3D_WinDevice())
                {
                        RECT r;
            PAINTSTRUCT ps;

            if (GetUpdateRect(win, &r, FALSE))
            {   BeginPaint(win, &ps);
                                RLDS3D_HandlePaint(&ps);
                EndPaint(win, &ps);
            }
        }
        else return DefWindowProc(win, msg, wparam, lparam);

    default:
        return DefWindowProc(win, msg, wparam, lparam);
    }
    return 0L;
}

