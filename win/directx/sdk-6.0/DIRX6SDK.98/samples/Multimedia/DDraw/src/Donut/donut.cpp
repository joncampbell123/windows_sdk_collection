//-----------------------------------------------------------------------------
// File: Donut.CPP
//
// Desc: This program is useful for testing multiple exclusive mode apps
//       interacting with multiple non-exclusive mode apps.  The program
//       displays a rotating donut in the upper left corner of the screen.  It
//       may be terminated by pressing Esc or F12.
//
//       Various command line switches can be specified to modify the
//       characteristics of this program.  Each switch consists of one
//       character and need not be preceeded with a dash or slash.  The
//       switches are as follows:
//
//          0   -   Display in far left position
//          1   -   Display in middle position
//          2   -   Display in right position
//          X   -   Use exclusive mode
//          A   -   Switch mode to 640x480x8 and use exclusive mode
//          B   -   Switch mode to 800x600x8 and use exclusive mode
//          C   -   Switch mode to 1024x768x8 and use exclusive mode
//          D   -   Switch mode to 1280x1024x8 and use exclusive mode
//
//       The switches may be combined.  If two switches are used which
//       contradict, the last switch specified will be in effect.
//
//       If this program is running in non-exclusive mode, it attempts to
//       continue to run even when it loses focus.  If it is running in
//       exclusive mode, the program does not attempt to modify the screen when
//       it doesn't have focus.
//
// Copyright (c) 1995-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include <windows.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdarg.h>
#include "resource.h"
#include "ddutil.h"

//-----------------------------------------------------------------------------
// Local definitions
//-----------------------------------------------------------------------------
#define NAME                "Donut"
#define TITLE               "Direct Draw Donut Example"

//-----------------------------------------------------------------------------
// Global data
//-----------------------------------------------------------------------------
LPDIRECTDRAW4               g_pDD = NULL;        // DirectDraw object
LPDIRECTDRAWSURFACE4        g_pDDSPrimary = NULL;// DirectDraw primary surface
LPDIRECTDRAWSURFACE4        g_pDDSOne = NULL;    // Offscreen surface 1
LPDIRECTDRAWSURFACE4        g_pDDSTwo = NULL;    // Offscreen surface 2
LPDIRECTDRAWPALETTE         g_pDDPal = NULL;     // The primary surface palette
BOOL                        g_bActive = FALSE;   // Is application active?
int                         g_ArgPos = 0;
BOOL                        g_ArgExclusive = FALSE;
int                         g_ArgMode = 0;

//-----------------------------------------------------------------------------
// Local data
//-----------------------------------------------------------------------------
// Name of our bitmap resource.
static char                 szBitmap[] = "DONUT";




//-----------------------------------------------------------------------------
// Name: ReleaseAllObjects()
// Desc: Finished with all objects we use; release them
//-----------------------------------------------------------------------------
static void
ReleaseAllObjects(void)
{
    if (g_pDD != NULL)
    {
        if (g_pDDSPrimary != NULL)
        {
            g_pDDSPrimary->Release();
            g_pDDSPrimary = NULL;
        }
        if (g_pDDSOne != NULL)
        {
            g_pDDSOne->Release();
            g_pDDSOne = NULL;
        }
        if (g_pDDSTwo != NULL)
        {
            g_pDDSTwo->Release();
            g_pDDSTwo = NULL;
        }
        if (g_pDDPal != NULL)
        {
            g_pDDPal->Release();
            g_pDDPal = NULL;
        }
        g_pDD->Release();
        g_pDD = NULL;
    }
    // Clean up the screen on exit
    RedrawWindow(NULL,NULL,NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
}




//-----------------------------------------------------------------------------
// Name: InitFail()
// Desc: This function is called if an initialization function fails
//-----------------------------------------------------------------------------
HRESULT
InitFail(HWND hWnd, HRESULT hRet, LPCTSTR szError,...)
{
    char                        szBuff[128];
    va_list                     vl;

    va_start(vl, szError);
    vsprintf(szBuff, szError, vl);
    ReleaseAllObjects();
    MessageBox(hWnd, szBuff, TITLE, MB_OK);
    DestroyWindow(hWnd);
    va_end(vl);
    return hRet;
}




//-----------------------------------------------------------------------------
// Name: RestoreAll()
// Desc: Restore all lost objects
//-----------------------------------------------------------------------------
HRESULT 
RestoreAll(void)
{
    HRESULT                     hRet;

    hRet = g_pDDSPrimary->Restore();
    if (hRet == DD_OK)
    {
        hRet = g_pDDSOne->Restore();
        if (hRet == DD_OK)
        {
            hRet = g_pDDSTwo->Restore();
            if (hRet == DD_OK)
            {
                DDReLoadBitmap(g_pDDSOne, szBitmap);
            }
        }
    }
    return hRet;
}




//-----------------------------------------------------------------------------
// Name: UpdateFrame()
// Desc: Decide what needs to be blitted next, wait for flip to complete,
//       then flip the buffers.
//-----------------------------------------------------------------------------
void 
UpdateFrame(void)
{
    static DWORD                lastTickCount = 0;
    static int                  currentFrame = 0;
    static BOOL                 haveBackground = FALSE;
    DWORD                       thisTickCount;
    RECT                        rcRect;
    DWORD                       delay = 17;
    HRESULT                     hRet;
    int                         pos;

    thisTickCount = GetTickCount();
    if ((thisTickCount - lastTickCount) <= delay)
        return;

    switch (g_ArgPos)
    {
        case 0:
            pos = 0;
            break;
        case 1:
            pos = 64;
            break;
        case 2:
            pos = 128;
            break;
    }
    rcRect.left = 0;
    rcRect.top = 0;
    rcRect.right = 64;
    rcRect.bottom = 64;

    // Restore a previously saved patch
    while (haveBackground)
    {
        hRet = g_pDDSPrimary->BltFast(pos, 0, g_pDDSTwo, &rcRect, FALSE);
        if (hRet == DD_OK)
        {
            haveBackground = TRUE;
            break;
        }
        if (hRet == DDERR_SURFACELOST)
        {
            hRet = RestoreAll();
            if (hRet != DD_OK)
                return;
        }
        if (hRet != DDERR_WASSTILLDRAWING)
            return;
    }

    rcRect.left = pos;
    rcRect.right = pos + 64;
    // Save the current primary surface that we are about to overwrite
    while (TRUE)
    {
        haveBackground = FALSE;
        hRet = g_pDDSTwo->BltFast(0, 0, g_pDDSPrimary,
                                  &rcRect, DDBLTFAST_NOCOLORKEY);
        if (hRet == DD_OK)
        {
            haveBackground = TRUE;
            break;
        }
        if (hRet == DDERR_SURFACELOST)
        {
            hRet = RestoreAll();
            if (hRet != DD_OK)
                return;
        }
        if (hRet != DDERR_WASSTILLDRAWING)
            return;
    }

    thisTickCount = GetTickCount();
    if ((thisTickCount - lastTickCount) > delay)
    {
        // Move to next frame;
        lastTickCount = thisTickCount;
        currentFrame++;
        if (currentFrame > 59)
            currentFrame = 0;
    }

    // Blit the stuff for the next frame
    rcRect.left = currentFrame % 10 * 64;
    rcRect.top = currentFrame / 10 * 64;
    rcRect.right = currentFrame % 10 * 64 + 64;
    rcRect.bottom = currentFrame / 10 * 64 + 64;

    while (TRUE)
    {
        hRet = g_pDDSPrimary->BltFast(pos, 0, g_pDDSOne,
                                      &rcRect, DDBLTFAST_SRCCOLORKEY);
        if (hRet == DD_OK)
        {
            break;
        }
        if (hRet == DDERR_SURFACELOST)
        {
            hRet = RestoreAll();
            if (hRet != DD_OK)
                return;
        }
        if (hRet != DDERR_WASSTILLDRAWING)
            return;
    }
    if (hRet != DD_OK)
        return;
}




//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The Main Window Procedure
//-----------------------------------------------------------------------------
long FAR PASCAL 
WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_ACTIVATEAPP:
            // Pause if minimized or not the top window
            g_bActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
            return 0L;

        case WM_DESTROY:
            // Clean up and close the app
            ReleaseAllObjects();
            PostQuitMessage(0);
            return 0L;

        case WM_KEYDOWN:
            // Handle any non-accelerated key commands
            switch (wParam)
            {
                case VK_ESCAPE:
                case VK_F12:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0L;
            }
            break;

        case WM_PALETTECHANGED:
            if ((HWND) wParam == hWnd)
                break;
            // Fall through to WM_QUERYNEWPALETTE

        case WM_QUERYNEWPALETTE:
            // Install our palette here
            if (g_pDDPal)
                g_pDDSPrimary->SetPalette(g_pDDPal);
            DDReLoadBitmap(g_pDDSOne, szBitmap);
            break;

        case WM_SETCURSOR:
            // Turn off the cursor since this is a full-screen app
            SetCursor(NULL);
            return TRUE;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}




//-----------------------------------------------------------------------------
// Name: InitApp()
// Desc: Do work required for every instance of the application:
//          Create the window, initialize data
//-----------------------------------------------------------------------------
static HRESULT
InitApp(HINSTANCE hInstance, int nCmdShow)
{
    HWND                        hWnd;
    WNDCLASS                    wc;
    DDSURFACEDESC2              ddsd;
    HRESULT                     hRet;
    LPDIRECTDRAW                pDD;

    // Set up and register window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAIN_ICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass(&wc);

    // Create a window
    hWnd = CreateWindowEx(0,
                          NAME,
                          TITLE,
                          WS_POPUP,
                          0,
                          0,
                          1,
                          1,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetFocus(hWnd);

    ///////////////////////////////////////////////////////////////////////////
    // Create the main DirectDraw object
    ///////////////////////////////////////////////////////////////////////////
    hRet = DirectDrawCreate(NULL, &pDD, NULL);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "DirectDrawCreate FAILED");

    // Fetch DirectDraw4 interface
    hRet = pDD->QueryInterface(IID_IDirectDraw4, (LPVOID *) & g_pDD);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "QueryInterface FAILED");

    // Get exclusive mode if requested
    if (g_ArgExclusive)
        hRet = g_pDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
    else
        hRet = g_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "SetCooperativeLevel FAILED");

    // Set the video mode to 640x480x8
    switch (g_ArgMode)
    {
        case 1:
            hRet = g_pDD->SetDisplayMode(640, 480, 8, 0, 0);
            break;
        case 2:
            hRet = g_pDD->SetDisplayMode(800, 600, 8, 0, 0);
            break;
        case 3:
            hRet = g_pDD->SetDisplayMode(1024, 768, 8, 0, 0);
            break;
        case 4:
            hRet = g_pDD->SetDisplayMode(1280, 1024, 8, 0, 0);
            break;
    }
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "SetDisplayMode FAILED");

    // Create the primary surface
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "CreateSurface FAILED");

    // Create and set the palette
    g_pDDPal = DDLoadPalette(g_pDD, szBitmap);
    if (g_pDDPal)
        g_pDDSPrimary->SetPalette(g_pDDPal);

    // Create the offscreen surface, by loading our bitmap.
    g_pDDSOne = DDLoadBitmap(g_pDD, szBitmap, 0, 0);
    if (g_pDDSOne == NULL)
        return InitFail(hWnd, hRet, "DDLoadBitmap FAILED");

    // set color key to black
    DDSetColorKey(g_pDDSOne, RGB(0, 0, 0));

    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwHeight = 64;
    ddsd.dwWidth = 64;
    hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSTwo, NULL);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "CreateSurface FAILED");

    return DD_OK;
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Initialization, message loop
//-----------------------------------------------------------------------------
int PASCAL
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    MSG                         msg;
    LPSTR                       c;

    for (c = lpCmdLine; *c != '\0'; c++)
    {
        switch (*c)
        {
            case '0':
                g_ArgPos = 0;
                break;
            case '1':
                g_ArgPos = 1;
                break;
            case '2':
                g_ArgPos = 2;
                break;
            case 'X':
                g_ArgExclusive = TRUE;
                break;
            case 'A':
                g_ArgExclusive = TRUE;
                g_ArgMode = 1;
                break;
            case 'B':
                g_ArgExclusive = TRUE;
                g_ArgMode = 2;
                break;
            case 'C':
                g_ArgExclusive = TRUE;
                g_ArgMode = 3;
                break;
            case 'D':
                g_ArgExclusive = TRUE;
                g_ArgMode = 4;
                break;
        }
    }

    if (InitApp(hInstance, nCmdShow) != DD_OK)
        return FALSE;

    while (TRUE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0))
                return msg.wParam;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else if (!g_ArgExclusive || g_bActive)
        {
            UpdateFrame();
        }
        else
        {
            // Make sure we go to sleep if we have nothing else to do
            WaitMessage();
        }
    }
}


