//-----------------------------------------------------------------------------
// File: Font.CPP
//
// Desc: Draw a GDI font on a DirectDraw surface
//
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
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "resource.h"

//-----------------------------------------------------------------------------
// Local definitions
//-----------------------------------------------------------------------------
#define TIMER_ID            1
#define WIDTHBYTES(i)       ((i+31)/32*4)
#define PALETTE_SIZE        2
#define FULL_STRING         "Text (0000,0000)"

//-----------------------------------------------------------------------------
// Global data
//-----------------------------------------------------------------------------
LPDIRECTDRAW4               g_pDD;          // DirectDraw object
LPDIRECTDRAWSURFACE4        g_pDDSPrimary;  // DirectDraw primary surface
LPSTR                       g_pBmpBits;     // pointer to DIB bits
HDC                         g_hMemDC;       // memory DC for rendering text
HFONT                       g_hFont;        // font we render text in
HBITMAP                     g_hBitmap;      // bitmap for holding text
int                         g_iBmpHeight;   // height of DIB
int                         g_iBmpWidth;    // width of DIB
int                         g_iWinPosX;     // X pos of client area of window
int                         g_iWinPosY;     // Y pos of client area of window
int                         g_iWinWidth;    // width of client area of window
int                         g_iWinHeight;   // height of client area of window
int                         g_iScreenWidth; // width of display
int                         g_iScreenHeight;// height of display

//-----------------------------------------------------------------------------
// Local data
//-----------------------------------------------------------------------------
static char                 szClass[] = "DDFontClass";
static char                 szCaption[] = "DirectDraw Font Test";




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
        g_pDD->Release();
        g_pDD = NULL;
    }
    if (g_hMemDC != NULL)
    {
        DeleteDC(g_hMemDC);
        g_hMemDC = NULL;
    }
    if (g_hBitmap != NULL)
    {
        DeleteObject(g_hBitmap);
        g_hBitmap = NULL;
    }
    if (g_hFont != NULL)
    {
        DeleteObject(g_hFont);
        g_hFont = NULL;
    }
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
    MessageBox(hWnd, szBuff, szCaption, MB_OK);
    DestroyWindow(hWnd);
    va_end(vl);
    return hRet;
}




//-----------------------------------------------------------------------------
// Name: InitDC
// Desc: Creates a DC with an Arial font and a DIB big enough to hold
//       some text selected into it.
//-----------------------------------------------------------------------------
static BOOL 
InitDC(void)
{
    HDC                         hdc;
    LPBITMAPINFO                pbmi;
    SIZE                        size;

    g_hFont = CreateFont(24,
                         0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                         ANSI_CHARSET,
                         OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS,
                         DEFAULT_QUALITY,
                         VARIABLE_PITCH,
                         "Arial");
    if (g_hFont == NULL)
        return FALSE;

    // Create a memory DC for rendering our text into
    hdc = GetDC(HWND_DESKTOP);
    g_hMemDC = CreateCompatibleDC(hdc);
    ReleaseDC(NULL, hdc);
    if (g_hMemDC == NULL)
    {
        DeleteObject(g_hFont);
        return FALSE;
    }

    // Select font, and get text dimensions
    SelectObject(g_hMemDC, g_hFont);
    GetTextExtentPoint32(g_hMemDC, FULL_STRING, sizeof(FULL_STRING) - 1, &size);
    g_iBmpWidth = size.cx + 2;
    g_iBmpHeight = size.cy + 2;

    // Create a dib section for containing the bits
    pbmi = (LPBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFO) +
                                     PALETTE_SIZE * sizeof(RGBQUAD));
    if (pbmi == NULL)
    {
        DeleteObject(g_hFont);
        DeleteDC(g_hMemDC);
        return FALSE;
    }
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = g_iBmpWidth;
    pbmi->bmiHeader.biHeight = -1 * g_iBmpHeight;  // negative height = top-down
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biBitCount = 8;  // 8bpp makes it easy to get data

    pbmi->bmiHeader.biCompression = BI_RGB;
    pbmi->bmiHeader.biXPelsPerMeter = 0;
    pbmi->bmiHeader.biYPelsPerMeter = 0;
    pbmi->bmiHeader.biClrUsed = PALETTE_SIZE;
    pbmi->bmiHeader.biClrImportant = PALETTE_SIZE;

    pbmi->bmiHeader.biSizeImage = WIDTHBYTES(g_iBmpWidth * 8) * g_iBmpHeight;

    // Just a plain monochrome palette
    pbmi->bmiColors[0].rgbRed = 0;
    pbmi->bmiColors[0].rgbGreen = 0;
    pbmi->bmiColors[0].rgbBlue = 0;
    pbmi->bmiColors[1].rgbRed = 255;
    pbmi->bmiColors[1].rgbGreen = 255;
    pbmi->bmiColors[1].rgbBlue = 255;

    // Create a DIB section that we can use to read the font bits out of
    g_hBitmap = CreateDIBSection(hdc,
                                 pbmi,
                                 DIB_RGB_COLORS,
                                 (void **) &g_pBmpBits,
                                 NULL,
                                 0);
    LocalFree(pbmi);
    if (g_hBitmap == NULL)
    {
        DeleteObject(g_hFont);
        DeleteDC(g_hMemDC);
        return FALSE;
    }

    // Set up our memory DC with the font and bitmap
    SelectObject(g_hMemDC, g_hBitmap);
    SetBkColor(g_hMemDC, RGB(0, 0, 0));
    SetTextColor(g_hMemDC, RGB(255, 255, 255));
    return TRUE;
}




//-----------------------------------------------------------------------------
// Name: DisplayText()
// Desc: Displays a given string at a specified location on the primary surface.
//-----------------------------------------------------------------------------
static void 
DisplayText(int x, int y, LPSTR text,...)
{
    char                        buff[256];
    DDSURFACEDESC2              ddsd;
    HRESULT                     hRet;
    va_list                     vlist;
    int                         height;
    int                         width;

    if (g_pDD == NULL)
        return;

    // Get message to display
    va_start(vlist, text);
    vsprintf(buff, text, vlist);
    va_end(vlist);

    // Output text to our memory DC (the bits end up in our DIB section)
    PatBlt(g_hMemDC, 0, 0, g_iBmpWidth, g_iBmpHeight, BLACKNESS);
    TextOut(g_hMemDC, 1, 1, buff, lstrlen(buff));

    // Get access to the primary surface
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    hRet = g_pDDSPrimary->Lock(NULL, &ddsd, 0, NULL);
    if (hRet == DD_OK)
    {
        LPSTR                       lpbits;
        LPSTR                       lpsrc;
        int                         bytes_pixel;

        switch (ddsd.ddpfPixelFormat.dwRGBBitCount)
        {
            case 8:
                bytes_pixel = 1;
                break;
            case 16:
                bytes_pixel = 2;
                break;
            case 24:
                bytes_pixel = 3;
                break;
            case 32:
                bytes_pixel = 4;
                break;
        }

        // Correct width/height of new text
        height = g_iBmpHeight;
        if (height + y > g_iWinHeight)
            height = g_iWinHeight - y;
        if (height + (g_iWinPosY + y) > g_iScreenHeight)
            height = g_iScreenHeight - (g_iWinPosY + y);

        width = g_iBmpWidth;
        if (x + width > g_iWinWidth)
            width = g_iWinWidth - x;
        if (width + (g_iWinPosX + x) > g_iScreenWidth)
            width = g_iScreenWidth - (g_iWinPosX + x);

        // Get pointer to place on screen we want to copy the text to
        lpbits = &(((LPSTR) ddsd.lpSurface)[(g_iWinPosY + y) * ddsd.lPitch +
                                            (g_iWinPosX + x) * bytes_pixel]);

        //---------------------------------------------------------------------
        // Copy the bits.  Fastested implementation would be in assembly of
        // course, but for simplicity we show it in C.
        //
        // We always want red text.   The 8bpp works fastest - we generated
        // a monochrome DIB section, so the bits in the DIB section are either
        // 0 or 1.  0 is black, and 1 is red in the standard 8bpp palettized
        // mode, so we can just copy the values.   For larger bpp, we need
        // to copy the data pixel by pixel and do a conversion.
        //---------------------------------------------------------------------
        if (width > 0)
        {
            int                         i;
            int                         j;

            lpsrc = g_pBmpBits;
            switch (bytes_pixel)
            {
                    //---------------------------------------------------------
                    // 8bpp 
                    //---------------------------------------------------------
                case 1:
                    for (i = 0; i < height; i++)
                    {
                        memcpy(lpbits, lpsrc, width);
                        lpbits += ddsd.lPitch;
                        lpsrc += WIDTHBYTES(g_iBmpWidth * 8);
                    }
                    break;
                    //---------------------------------------------------------
                    // 16bpp
                    //---------------------------------------------------------
                case 2:
                    for (i = 0; i < height; i++)
                    {
                        for (j = 0; j < width; j++)
                        {
                            if (lpsrc[j])
                            {
                                ((WORD *) lpbits)[j] = 0x7c00;
                            }
                            else
                            {
                                ((WORD *) lpbits)[j] = 0x0000;
                            }
                        }
                        lpbits += ddsd.lPitch;
                        lpsrc += WIDTHBYTES(g_iBmpWidth * 8);
                    }
                    break;
                    //---------------------------------------------------------
                    // 24bpp
                    //---------------------------------------------------------
                case 3:
                    for (i = 0; i < height; i++)
                    {
                        for (j = 0; j < width; j++)
                        {
                            if (lpsrc[j])
                            {
                                lpbits[j * 3] = (char) 0x00;
                                lpbits[j * 3 + 1] = (char) 0x00;
                                lpbits[j * 3 + 2] = (char) 0xff;
                            }
                            else
                            {
                                lpbits[j * 3] = (char) 0x00;
                                lpbits[j * 3 + 1] = (char) 0x00;
                                lpbits[j * 3 + 2] = (char) 0x00;
                            }
                        }
                        lpbits += ddsd.lPitch;
                        lpsrc += WIDTHBYTES(g_iBmpWidth * 8);
                    }
                    break;
                    //---------------------------------------------------------
                    // 32bpp
                    //---------------------------------------------------------
                case 4:
                    for (i = 0; i < height; i++)
                    {
                        for (j = 0; j < width; j++)
                        {
                            if (lpsrc[j])
                            {
                                ((DWORD *) lpbits)[j] = 0x00ff0000l;
                            }
                            else
                            {
                                ((DWORD *) lpbits)[j] = 0x00000000l;
                            }
                        }
                        lpbits += ddsd.lPitch;
                        lpsrc += WIDTHBYTES(g_iBmpWidth * 8);
                    }
                    break;
            }
        }
        // Done with the primary surface
        g_pDDSPrimary->Unlock(NULL);
    }
}




//-----------------------------------------------------------------------------
// Name: UpdateDisplay
// Desc:
//-----------------------------------------------------------------------------
void 
UpdateDisplay(void)
{
    static BOOL                 bUpdating;
    static int                  iXPos = 100;
    static int                  iYPos = 100;

    if (bUpdating)
        return;
    bUpdating = TRUE;

    iXPos += (rand() % 3) - 1;
    if (iXPos < 0)
        iXPos = 0;
    else
        if (iXPos > g_iWinWidth)
            iXPos = g_iWinWidth;

    iYPos += (rand() % 3) - 1;
    if (iYPos < 0)
        iYPos = 0;
    else
        if (iYPos > g_iWinHeight)
            iYPos = g_iWinHeight;
    DisplayText(iXPos, iYPos, "Text (%ld,%ld)", iXPos, iYPos);

    bUpdating = FALSE;
}




//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The Main Window Procedure
//-----------------------------------------------------------------------------
LRESULT CALLBACK 
WindowProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
            }
            break;

        case WM_PAINT:
            {
                PAINTSTRUCT                 ps;

                BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
                break;
            }

        case WM_TIMER:
            if (TIMER_ID == wParam)
                UpdateDisplay();
            break;

        case WM_MOVE:
            g_iWinPosX = (int) (short) LOWORD(lParam);
            g_iWinPosY = (int) (short) HIWORD(lParam);
            return DefWindowProc(hWnd, uMsg, wParam, lParam);

        case WM_SIZE:
            g_iWinWidth = (int) (short) LOWORD(lParam);
            g_iWinHeight = (int) (short) HIWORD(lParam);
            return DefWindowProc(hWnd, uMsg, wParam, lParam);

        case WM_DESTROY:
            KillTimer(hWnd, TIMER_ID);
            ReleaseAllObjects();
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0L;
}




//-----------------------------------------------------------------------------
// Name: InitApp()
// Desc: Do work required for every instance of the application:
//          Create the window, initialize data
//-----------------------------------------------------------------------------
static BOOL 
InitApp(HANDLE hInstance, int nCmdShow)
{
    HWND                        hWnd;
    WNDCLASS                    wc;
    HRESULT                     hRet;
    DDSURFACEDESC2              ddsd;
    LPDIRECTDRAW                pDD;

    // Initialize our random number generator
    srand(GetTickCount());

    // Set up and register window class
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DWORD);
    wc.hInstance = (HINSTANCE) hInstance;
    wc.hIcon = LoadIcon((HINSTANCE) hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = szClass;
    if (!RegisterClass(&wc))
        return FALSE;

    // Create a window
    hWnd = CreateWindow(szClass,            // class
                        szCaption,          // caption
                        WS_OVERLAPPEDWINDOW,// style 
                        CW_USEDEFAULT,      // x pos
                        CW_USEDEFAULT,      // y pos
                        CW_USEDEFAULT,      // width
                        CW_USEDEFAULT,      // height
                        NULL,               // parent window
                        NULL,               // menu 
                        (HINSTANCE) hInstance,  // instance
                        NULL);              // parms
    if (!hWnd)
        return FALSE;
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Create a timer for moving text around
    if (!SetTimer(hWnd, TIMER_ID, 30, NULL))
    {
        DestroyWindow(hWnd);
        return FALSE;
    }

    // Create the main DirectDraw object
    hRet = DirectDrawCreate(NULL, &pDD, NULL);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "DirectDrawCreate FAILED");

    // Fetch DirectDraw4 interface
    hRet = pDD->QueryInterface(IID_IDirectDraw4, (LPVOID *) & g_pDD);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "QueryInterface FAILED");

    // Set our cooperative level
    hRet = g_pDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "SetCooperativeLevel FAILED");

    // Make DC, font & DIB section we need
    if (!InitDC())
    {
        g_pDD->Release();
        DestroyWindow(hWnd);
        return FALSE;
    }

    // Create the primary surface
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    hRet = g_pDD->CreateSurface(&ddsd, &g_pDDSPrimary, NULL);
    if (hRet != DD_OK)
        return InitFail(hWnd, hRet, "CreateSurface FAILED");

    // Get dimensions of display
    g_iScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    g_iScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    return TRUE;
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

    if (!InitApp(hInstance, nCmdShow))
        return FALSE;

    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}



