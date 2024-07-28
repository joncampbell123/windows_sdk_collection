/*
	palette.c

	This is a shell for a Windows 3.0 app
	
    11/14/89    box sizes conform to window extent
	3/29/89		created
	
*/

#include <windows.h>
#include "palette.h"

#define PALSIZE 256

/* usefull global things */

HANDLE hInst;					/* global instance handle */
char szAppName[10];				/* app name */
HWND hMainWnd;					/* handle of main window */
int iXcells, iYcells;           /* number of cells on each axis */
int iWidth, iHeight;
NPLOGPALETTE pLogPal;
HPALETTE hPal;
BOOL bCaptured = FALSE;         /* mouse capture flag */

/***************** local function predecs ***************************/

int get_xcells(int xsize, int ysize);
void show_pixel(LONG lParam);

/***************** Main entry point routine *************************/

int PASCAL WinMain(hInstance,hPrevInstance,lpszCmdLine,cmdShow)
HANDLE hInstance,hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
	MSG msg;

	hInst = hInstance;		   	/* save our instance handle */

	LoadString(hInstance, IDS_APPNAME, szAppName, sizeof(szAppName)); 
	if (!hPrevInstance) {
		if (! InitFirstInstance(hInstance)) {
			return 1;
		}
	}

	hMainWnd = CreateWindow(szAppName, 	  	 	/* class name */
						szAppName,				/* caption text */
						WS_OVERLAPPEDWINDOW,	/* window style */
                        CW_USEDEFAULT,
                        0,
						GetSystemMetrics(SM_CXSCREEN) / 2,
						GetSystemMetrics(SM_CYSCREEN) / 4,
						(HWND)NULL,				/* handle of parent window */
						(HMENU)NULL,			/* menu handle (default class) */
						hInstance,		 		/* handle to window instance */
						(LPSTR)NULL				/* no params to pass on */
						);
	
	if (!hMainWnd) {
		return 1;
	}

	ShowWindow(hMainWnd, cmdShow); /* display window as open or icon */
	UpdateWindow(hMainWnd);		/* paint it */
	
	/* check for messages from Windows and process them */
	/* if no messages, perform some idle function */

	do {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			/* got a message to process */
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			/* perform some idle routine */
		}
	} while (1);

#ifdef PROFILE
    ProfFinish();
#endif
	return (msg.wParam);
}
	
/************* main window message handler ******************************/

long EXPORT MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
{
    HANDLE hPalMem;
    HDC hDC;
	PAINTSTRUCT ps;				/* paint structure */
    int i;
	
	/* process any messages we want */

	switch(message) {
    case WM_CREATE:
        /*
            We don't bother to check for palette support as
            we want the app to run even if the display doesn't
            have a palette
        */

        /* create a logical palette to play with */
        hPalMem = LocalAlloc(LMEM_FIXED,
                          sizeof(LOGPALETTE) 
                          + PALSIZE * sizeof(PALETTEENTRY));
        if (!hPalMem) {
            Error("No memory for palette");
            return -1;
        }

        pLogPal = (NPLOGPALETTE) LocalLock(hPalMem);
        pLogPal->palVersion = 0x300; /* bloody mysterious */
        pLogPal->palNumEntries = PALSIZE;
        for (i=0; i<PALSIZE; i++) {
            pLogPal->palPalEntry[i].peRed = (BYTE)i;
            pLogPal->palPalEntry[i].peGreen = 0;
            pLogPal->palPalEntry[i].peBlue = 0;
            pLogPal->palPalEntry[i].peFlags = PC_EXPLICIT;
        }
        hPal = CreatePalette((LPLOGPALETTE)pLogPal);
        if (!hPal) {
            Error("CreatePalette() failed");
            return -1;
        }

        break;

    case WM_SIZE:
    
        /* remember the window size for repaint later */
        iWidth = LOWORD(lParam);
        iHeight = HIWORD(lParam);

        /* compute the number of cells on each axis to keep aspect good */
        iXcells = get_xcells(iWidth, iHeight);
        iYcells = PALSIZE / iXcells;

        break;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);
        Paint(hWnd, ps.hdc);
		EndPaint(hWnd, &ps);
 		break;

    case WM_PALETTECHANGED:
        if (wParam != hWnd) {
            hDC = GetDC(hWnd);
            SelectPalette(hDC, hPal, 0);
            if (RealizePalette(hDC)) {
                /* some colors changed */
                InvalidateRect(hWnd, NULL, TRUE);
                MessageBeep(0);
            }
            ReleaseDC(hWnd, hDC);
        }
        break;

    case WM_LBUTTONDOWN:
        SetCapture(hWnd);       /* grab the mouse input */
        bCaptured = TRUE;
        show_pixel(lParam);     /* show pixel value here */
        break;

    case WM_MOUSEMOVE:
        if (bCaptured) {
            show_pixel(lParam); /* show pixel value here */
        }
        break;

    case WM_LBUTTONUP:
        if (bCaptured) {
            ReleaseCapture();    
            bCaptured = FALSE;
            SetWindowText(hWnd, szAppName);
        }
        break;

	case WM_DESTROY:
        if (hPal) {
            DeleteObject(hPal);
        }

		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return NULL;
}

void Paint(HWND hWnd, HDC hDC)
{
    HBRUSH hBrush, hOldBrush;
    HPEN hPen, hOldPen;

    int i, j, top, left, bottom, right;

#ifdef PROFILE
    ProfStart();
#endif

    SelectPalette(hDC, hPal, 0);
    RealizePalette(hDC);
    hPen = GetStockObject(NULL_PEN);
    hOldPen = SelectObject(hDC, hPen);

    /*
        note: the use of (long) casts here is to avoid overrange
        values for large windows
    */

    for (j=0, top=0; j<iYcells; j++, top=bottom) {
        bottom = (int)((long)(j+1)*(long)iHeight/(long)iYcells) + 1;
        for (i=0, left=0; i<iXcells; i++, left=right) {
            right = (int)((long)(i+1)*(long)iWidth/(long)iXcells) + 1;
            hBrush = CreateSolidBrush(PALETTEINDEX(j * iXcells + i));
            hOldBrush = SelectObject(hDC, hBrush);
            Rectangle(hDC, left, top, right, bottom);
            SelectObject(hDC, hOldBrush);
            DeleteObject(hBrush);
        }
    }

    SelectObject(hDC, hOldPen);

#ifdef PROFILE
    ProfStop();
#endif
}

/********************************************************************** 

    Get the number of cells on the X axis which gives the closes
    aspect ratio to 1

**********************************************************************/

int get_xcells(int xsize, int ysize)
{
    int i, j;

    for (i=1; i<PALSIZE; i*=2) {
        j = PALSIZE / i;
        if (i * ysize / j >= xsize) break;
    }
    return i;
}


/**********************************************************************

    Show the rgb value of a given pixel at mouse position

**********************************************************************/

void show_pixel(LONG lParam)
{
    HDC hDC;
    char buf[80];
    DWORD rgb;
    POINT pt;

    /* get a hdc to the whole screen */

    hDC = GetDC(NULL);

    /* convert mouse coords to screen coords */

    pt = MAKEPOINT(lParam);
    ClientToScreen(hMainWnd, &pt);

    /* get rgb at mouse position */

    rgb = GetPixel(hDC, pt.x, pt.y);

    ReleaseDC(NULL, hDC);

    wsprintf(buf,
             "%s - RGB(%d,%d,%d)",
             (LPSTR)szAppName,
             GetRValue(rgb),
             GetGValue(rgb),
             GetBValue(rgb));
    SetWindowText(hMainWnd, buf);
}
