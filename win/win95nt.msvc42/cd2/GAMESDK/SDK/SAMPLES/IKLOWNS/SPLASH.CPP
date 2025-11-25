/*===========================================================================*\
|
|  File:        splash.cpp
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include "splashrc.h"

char szAppName[] = "ISplash";   // The name of this application
char acModuleName[256];

HINSTANCE       ghInst;
HWND            ghMainWnd;
LPBITMAPINFOHEADER  lpBitmap;
LPBYTE          mpBits;
HPALETTE        hPal;
HPALETTE        hPalOld;
LOGPALETTE      *pPal;
int         nPasses = 0;
int         fadeDirection = 1;
LPSTR           pSoundBuffer = NULL;

/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)

#define PALVERSION  0x300
#define MAXPALETTE  256      /* max. # supported palette entries */

#define NUMFADESTEPS    64
#define HOLD_TIME   500
#define INTERVAL_TIME   30

/****************************************************************************
 *                                      *
 *  FUNCTION   : DibNumColors(VOID FAR * pv)                    *
 *                                      *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *       the BitCount filed in the info block.              *
 *                                      *
 *  RETURNS    : The number of colors in the DIB.               *
 *                                      *
 ****************************************************************************/
WORD DibNumColors (VOID FAR * pv)
{
    INT             bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
    {
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits)
    {
        case 1:
            return 2;
        case 4:
            return 16;
        case 8:
            return 256;
        default:
            /* A 24 bitcount DIB has no color table */
            return 0;
    }
}

/****************************************************************************
 *                                      *
 *  FUNCTION   :  PaletteSize(VOID FAR * pv)                    *
 *                                      *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *        is of the BITMAPCOREHEADER type, the number of colors is  *
 *        multiplied by 3 to give the palette size, otherwise the   *
 *        number of colors is multiplied by 4.                              *
 *                                      *
 *  RETURNS    :  Palette size in number of bytes.              *
 *                                      *
 ****************************************************************************/
WORD PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER  lpbi;
    WORD            NumColors;

    lpbi = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

void FadePalette(
    LOGPALETTE      *pPal,
    LPBITMAPINFOHEADER  lpbi,
    int         fadestep
)
{
    RGBQUAD FAR *pRgb;

    if (lpbi == NULL)
        return;

    if (lpbi->biSize != sizeof(BITMAPINFOHEADER))
        return;

    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    for (int i = 1; i <= pPal->palNumEntries; i++)
    {
        pPal->palPalEntry[i-1].peRed   = (pRgb[i].rgbRed * fadestep) / NUMFADESTEPS;
        pPal->palPalEntry[i-1].peGreen = (pRgb[i].rgbGreen * fadestep) / NUMFADESTEPS;
        pPal->palPalEntry[i-1].peBlue  = (pRgb[i].rgbBlue * fadestep) / NUMFADESTEPS;
        pPal->palPalEntry[i-1].peFlags = PC_NOCOLLAPSE;
    }
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : CreateBIPalette(LPBITMAPINFOHEADER lpbi)           *
 *                                      *
 *  PURPOSE    : Given a Pointer to a BITMAPINFO struct will create a       *
 *       a GDI palette object from the color table.         *
 *                                      *
 *  RETURNS    :                                            *
 *                                      *
 ****************************************************************************/
LOGPALETTE  *CreateBIPalette (
     LPBITMAPINFOHEADER lpbi,
     int            fadestep
)
{

     LOGPALETTE *pPal;
     HPALETTE   hpal = NULL;
     WORD       nNumColors;
     BYTE       red;
     BYTE       green;
     BYTE       blue;
     WORD       i;
     RGBQUAD    FAR *pRgb;

    if (lpbi == NULL)
        return NULL;

    if (lpbi->biSize != sizeof(BITMAPINFOHEADER))
        return NULL;

    /* Get a pointer to the color table and the number of colors in it */
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    nNumColors = DibNumColors(lpbi);

    if (nNumColors)
    {
        /* Allocate for the logical palette structure */
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors - 2;
        pPal->palVersion    = PALVERSION;

        /* Fill in the palette entries from the DIB color table and
        * create a logical color palette.
        */

        for (i = 1; i < (nNumColors-1); i++)
        {
            pPal->palPalEntry[i-1].peRed   = (pRgb[i].rgbRed * fadestep) / NUMFADESTEPS;
            pPal->palPalEntry[i-1].peGreen = (pRgb[i].rgbGreen * fadestep) / NUMFADESTEPS;
            pPal->palPalEntry[i-1].peBlue  = (pRgb[i].rgbBlue * fadestep) / NUMFADESTEPS;
            pPal->palPalEntry[i-1].peFlags = PC_NOCOLLAPSE;
        }
    }
    else if (lpbi->biBitCount == 24)
    {
        /* A 24 bitcount DIB has no color table entries so, set the number of
        * to the maximum value (256).
        */
        nNumColors = MAXPALETTE;
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        red = green = blue = 0;

        /* Generate 256 (= 8*8*4) RGB combinations to fill the palette
        * entries.
        */
        for (i = 0; i < pPal->palNumEntries; i++)
        {
            pPal->palPalEntry[i].peRed   = red;
            pPal->palPalEntry[i].peGreen = green;
            pPal->palPalEntry[i].peBlue  = blue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;

            if (!(red += 32))
                if (!(green += 32))
                    blue += 64;
        }
    }
    return pPal;
}

/****************************************************************************

        FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)

        PURPOSE:  Processes messages

        MESSAGES:

        WM_COMMAND    - application menu (About dialog box)
        WM_DESTROY    - destroy window

        COMMENTS:

        To process the IDM_ABOUT message, call MakeProcInstance() to get the
        current instance address of the About() function.  Then call Dialog
        box which will create the box according to the information in your
        generic.rc file and turn control over to the About() function.  When
        it returns, free the intance address.

****************************************************************************/

LRESULT CALLBACK WndProc(
    HWND hWnd,         // window handle
    UINT message,      // type of message
    WPARAM uParam,     // additional information
    LPARAM lParam)     // additional information
{
        switch (message) {

    case WM_CREATE:
    {
        ShowCursor(FALSE);
        SetTimer(hWnd, 1, HOLD_TIME, NULL);
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT PS;
        HDC         hDC;
        HPALETTE    hPalTmp;

        hDC = BeginPaint( hWnd, &PS );

        hPalTmp = SelectPalette(hDC, hPal, FALSE);
        if (hPalOld == NULL)
            hPalOld = hPalTmp;
        RealizePalette(hDC);

        StretchDIBits(
                hDC,
                0,
                0,
#if 0
                lpBitmap->biWidth,      // stretch to fit
                lpBitmap->biHeight,     // stretch to fit
#endif
                GetSystemMetrics(SM_CXSCREEN),
                GetSystemMetrics(SM_CYSCREEN),
                0,
                0,
                lpBitmap->biWidth,      // stretch to fit
                lpBitmap->biHeight,     // stretch to fit
                mpBits,
                (CONST BITMAPINFO *)lpBitmap,
                DIB_RGB_COLORS,
                SRCCOPY
                );

        EndPaint( hWnd, &PS );
    }
    break;

    case WM_TIMER:
    {
        KillTimer(hWnd, 1);
        HDC hDC = GetDC(hWnd);

        DWORD lastTime = timeGetTime();

        if (pSoundBuffer != NULL)
            sndPlaySound(pSoundBuffer, SND_MEMORY | SND_ASYNC);

        do {
#if 1
            pPal = CreateBIPalette(lpBitmap, nPasses);
            hPal = CreatePalette(pPal);
#else
            FadePalette(pPal, lpBitmap, nPasses);
            AnimatePalette(hPal, 1, pPal->palNumEntries, pPal->palPalEntry);
#endif
            DeleteObject(SelectPalette(hDC, hPal, FALSE));
            RealizePalette(hDC);

            nPasses += fadeDirection;
            if (nPasses == NUMFADESTEPS)
            {
                fadeDirection = -1;
            } 

            while (timeGetTime() - lastTime < INTERVAL_TIME)
            {
                //Sleep(0);
            }
            lastTime = timeGetTime();

        } while(nPasses > 0);

        ReleaseDC(hWnd, hDC);
        WinExec(acModuleName, SW_SHOW);
    }

        case WM_DESTROY:  // message: window being destroyed
    {
#if 1
        HDC hDC = GetDC(hWnd);
        PatBlt(hDC, 0,0, 640, 480,BLACKNESS);
        SelectPalette(hDC, hPalOld, FALSE);
        RealizePalette(hDC);
        ReleaseDC(hWnd, hDC);
#endif

        sndPlaySound(NULL, SND_SYNC);
        ShowCursor(TRUE);
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    }

        default:          // Passes it on if unproccessed
        return (DefWindowProc(hWnd, message, uParam, lParam));
        }
        return (0);
}

#if 0
void dbgprintf(char *fmt,...)
{
    static HANDLE dbg = NULL;
    char    out [ 256 ];
    DWORD  len;
    va_list vlist;

    if (dbg == NULL)
    {
        AllocConsole();
        dbg =  GetStdHandle(STD_OUTPUT_HANDLE);
        if (dbg == NULL)
            return;         
    }
    
    va_start(vlist, fmt);
    wvsprintf(out, fmt, vlist);
    WriteConsole(dbg, out, strlen(out), &len, NULL);
}
#endif
/****************************************************************************

        FUNCTION:  InitInstance(HINSTANCE, int)

        PURPOSE:  Saves instance handle and creates main window

        COMMENTS:

                This function is called at initialization time for every instance of
                this application.  This function performs initialization tasks that
                cannot be shared by multiple instances.

                In this case, we save the instance handle in a static variable and
                create and display the main program window.

****************************************************************************/

BOOL InitInstance(
        HINSTANCE   hInstance,
        int     nCmdShow)
{
    WNDCLASS  wc;


    // Fill in window class structure with parameters that describe the
    // main window.

    wc.style         = CS_OWNDC;                        // Class style(s).
    wc.lpfnWndProc   = (WNDPROC)WndProc;       // Window Procedure
    wc.cbClsExtra    = 0;                      // No per-class extra data.
    wc.cbWndExtra    = 0;                      // No per-window extra data.
    wc.hInstance     = hInstance;              // Owner of this class
    wc.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APP)); // Icon name from .RC
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);// Cursor
    wc.hbrBackground = NULL;                    // Default color
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szAppName;              // Name to register as

    // Register the window class
    if (!RegisterClass(&wc))
        return(FALSE);

    // Create a main window for this application instance.

    ghMainWnd = CreateWindow(
            szAppName,           // See RegisterClass() call.
            NULL,               // Text for window title bar.
            WS_POPUP,                   // Window style.
            0,
            0,
            GetSystemMetrics(SM_CXSCREEN),
            GetSystemMetrics(SM_CYSCREEN),
            NULL,                // Overlapped windows have no parent.
            NULL,                // Use the window class menu.
            hInstance,           // This instance owns this window.
            NULL                 // We don't use any data in our WM_CREATE
            );

    // If window could not be created, return "failure"
    if (!ghMainWnd)
        return(FALSE);

    // Make the window visible; update its client area; and return "success"
    ShowWindow(ghMainWnd, nCmdShow); // Show the window
    UpdateWindow(ghMainWnd);         // Sends WM_PAINT message

    return (TRUE);              // We succeeded...
}

/****************************************************************************

        FUNCTION:  ShutDownApp()

        PURPOSE:  un-initialize the app as needed

        COMMENTS:

****************************************************************************/
void ShutDownApp()
{
}

/****************************************************************************

        FUNCTION: WinMain(HINSTANCE, HINSTANCE, LPSTR, int)

        PURPOSE: calls initialization function, processes message loop

        COMMENTS:

                Windows recognizes this function by name as the initial entry point
                for the program.  This function calls the application initialization
                routine, if no other instance of the program is running, and always
                calls the instance initialization routine.  It then executes a message
                retrieval and dispatch loop that is the top-level control structure
                for the remainder of execution.  The loop is terminated when a WM_QUIT
                message is received, at which time this function exits the application
                instance by returning the value passed by PostQuitMessage().

                If this function must abort before entering the message loop, it
                returns the conventional value NULL.

****************************************************************************/

int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow)
{
    MSG msg;
    int result = NULL;
    int nNameLength;

    // only allow 1 instance
    if (hPrevInstance)
      return NULL;

    ghInst = hInstance;

    nNameLength = GetModuleFileName(ghInst, acModuleName, sizeof(acModuleName));
    lstrcpy(&acModuleName[nNameLength-3], "OVL"); 


    HGLOBAL hgrBitmap = LoadResource( ghInst, FindResource(ghInst, MAKEINTRESOURCE(IDB_SPLASH), RT_BITMAP));

    lpBitmap = (LPBITMAPINFOHEADER)LockResource( hgrBitmap );

    mpBits = (LPBYTE)lpBitmap + (WORD)lpBitmap->biSize + PaletteSize(lpBitmap);
    pPal = CreateBIPalette(lpBitmap, NUMFADESTEPS);
    hPal = CreatePalette(pPal);
    fadeDirection = -1;
    nPasses = NUMFADESTEPS;


    HGLOBAL hgrSound = LoadResource( ghInst, FindResource(ghInst, MAKEINTRESOURCE(IDS_SPLASH), "WAVE"));
    pSoundBuffer = (LPSTR)LockResource( hgrSound );


    if (InitInstance(hInstance, nCmdShow))
    {
        /* Acquire and dispatch messages until a WM_QUIT message is
        received.
        */
        while (GetMessage(&msg, // message structure
          NULL,   // handle of window receiving the message
          0,      // lowest message to examine
          0))     // highest message to examine
        {
                TranslateMessage(&msg);// Translates virtual key codes
                DispatchMessage(&msg); // Dispatches message to window
        }

        ShutDownApp();  // call de-init code
        result = msg.wParam;
    }

    return (result);
}
