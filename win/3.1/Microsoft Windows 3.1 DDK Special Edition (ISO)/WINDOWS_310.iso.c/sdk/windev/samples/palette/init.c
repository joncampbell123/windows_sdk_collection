/*
	init.c

	initialisation and error handling code

*/

#include <windows.h>
#include "palette.h"


/************* first instance initialisation ***************************/

BOOL InitFirstInstance(hInstance)
HANDLE hInstance;
{
	WNDCLASS wc;
	
	/* define the class of window we want to register */

	wc.lpszClassName	= szAppName;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
//  wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hCursor			= LoadCursor(hInstance, "Cursor");
	wc.hIcon			= NULL; // LoadIcon(hInstance,"Icon");
	wc.lpszMenuName		= "Menu";
	wc.hbrBackground	= COLOR_WINDOW+1;
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= MainWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	
	if (! RegisterClass(&wc)) {
		return FALSE; /* Initialisation failed */
	}
	
	return TRUE;
}

int Error(msg)
LPSTR msg;
{
	MessageBeep(0);
	return MessageBox(hMainWnd, msg, szAppName, MB_OK);
}

BOOL bHasPalette(HWND hWnd)
{
    /* returns TRUE if display supports palette elese
    returns FALSE
    */

    HDC hDC;
    BOOL bPal;
    WORD wPalSize;

    bPal = TRUE; /* ever the optimist */
    hDC = GetDC(hWnd);
    if (!GetDeviceCaps(hDC,RASTERCAPS) & RC_PALETTE) {
        bPal = FALSE;
    } else {
        wPalSize = GetDeviceCaps(hDC,SIZEPALETTE);
        if (!wPalSize) {
            bPal = FALSE;
        }
    }
    ReleaseDC(hWnd, hDC);

    if (bPal) return TRUE;

    Error("Display device does not\nsupport a palette");
    return FALSE;
}
