/*----------------------------------------------------------------------------*\
|   PROGRESS.C -							       |
\*----------------------------------------------------------------------------*/

/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
#include <windows.h>
#include "pro.h"

static HWND	ghWnd = NULL;
static int	iCnt;
static FARPROC  fpxProDlg;
static DWORD    rgbFG;
static DWORD    rgbBG;

#define BAR_RANGE 0
#define BAR_POS   2

#define BAR_SETRANGE  WM_USER+BAR_RANGE
#define BAR_SETPOS    WM_USER+BAR_POS
#define BAR_DELTAPOS  WM_USER+4

#ifndef COLOR_HIGHLIGHT
    #define COLOR_HIGHLIGHT	  (COLOR_APPWORKSPACE + 1)
    #define COLOR_HIGHLIGHTTEXT   (COLOR_APPWORKSPACE + 2)
#endif

#define COLORBG  rgbBG
#define COLORFG  rgbFG

typedef LONG (FAR PASCAL *LPWNDPROC)(); // pointer to a window procedure

BOOL FAR PASCAL _export ProDlgProc(HWND, unsigned, WORD, LONG);
LONG FAR PASCAL _export ProBarProc(HWND, unsigned, WORD, LONG);

/*----------------------------------------------------------------------------*\
|   ProInit( hPrev,hInst )						       |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|	memory.  It performs all initialization.			       |
|                                                                              |
|   Arguments:                                                                 |
|	hPrev	   instance handle of previous instance 		       |
|	hInst	   instance handle of current instance			       |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL ProInit (hPrev,hInst)
    HANDLE hPrev;
    HANDLE hInst;
{
    WNDCLASS   rClass;

    if (!hPrev)
    {
       rClass.hCursor	     = LoadCursor(NULL,IDC_ARROW);
       rClass.hIcon	     = NULL;
       rClass.lpszMenuName   = NULL;
       rClass.lpszClassName  = PRO_CLASS;
       rClass.hbrBackground  = (HBRUSH)COLOR_WINDOW+1;
       rClass.hInstance      = hInst;
       rClass.style	     = CS_HREDRAW | CS_VREDRAW;
       rClass.lpfnWndProc    = (WNDPROC)ProBarProc;
       rClass.cbClsExtra     = 0;
       rClass.cbWndExtra     = 2*sizeof(WORD);

       if (!RegisterClass(&rClass))
          return FALSE;
    }

#if 0
    rgbFG = GetSysColor(COLOR_HIGHLIGHTTEXT);
    rgbBG = GetSysColor(COLOR_HIGHLIGHT);
#endif

    if (FALSE /* fMono */)  // is the device monochrome?
    {
        rgbBG = RGB(0,0,0);
        rgbFG = RGB(255,255,255);
    }
    else
    {
        rgbBG = RGB(0,0,255);
        rgbFG = RGB(255,255,255);
    }
    return TRUE;
}

void FAR PASCAL ProClear(HWND hDlg)
{
    if (!hDlg)
	    hDlg = ghWnd;

    SetDlgItemText (hDlg, ID_STATUS1, "");
    SetDlgItemText (hDlg, ID_STATUS2, "");
    SetDlgItemText (hDlg, ID_STATUS3, "");
    SetDlgItemText (hDlg, ID_STATUS4, "");
}

/*----------------------------------------------------------------------------*\
|   ProDlgProc( hWnd, uiMessage, wParam, lParam )			       |
|                                                                              |
|   Description:                                                               |
|	The window proc for the Progress dialog box			       |
|                                                                              |
|   Arguments:                                                                 |
|	hWnd		window handle for the dialog			       |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

BOOL FAR PASCAL _export ProDlgProc( hDlg, uiMessage, wParam, lParam )
    HWND     hDlg;
    unsigned uiMessage;
    WORD     wParam;
    long     lParam;
{
    switch (uiMessage) {

    case WM_INITDIALOG:
        ProClear(hDlg);
        return TRUE;

    }
    return FALSE;
}

/*----------------------------------------------------------------------------*\
|   ProBarProc( hWnd, uiMessage, wParam, lParam )			       |
|                                                                              |
|   Description:                                                               |
|	The window proc for the Progress Bar chart			       |
|                                                                              |
|   Arguments:                                                                 |
|	hWnd		window handle for the dialog			       |
|       uiMessage       message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/

LONG FAR PASCAL _export ProBarProc( hWnd, uiMessage, wParam, lParam )
    HWND     hWnd;
    unsigned uiMessage;
    WORD     wParam;
    long     lParam;
{
    PAINTSTRUCT rPS;
    RECT	rc1,rc2;
    WORD	dx,dy,x;
    WORD	iRange,iPos;
    char	ach[30];
    DWORD	dwExtent;

    switch (uiMessage) {
        case WM_CREATE:
	    SetWindowWord (hWnd,BAR_RANGE,100);
	    SetWindowWord (hWnd,BAR_POS,0);
            return 0L;

	case BAR_SETRANGE:
	case BAR_SETPOS:
	    SetWindowWord (hWnd,uiMessage-WM_USER,wParam);
	    InvalidateRect (hWnd,NULL,FALSE);
	    UpdateWindow(hWnd);
            return 0L;

	case BAR_DELTAPOS:
	    iPos = GetWindowWord (hWnd,BAR_POS);
	    SetWindowWord (hWnd,BAR_POS,iPos+wParam);
	    InvalidateRect (hWnd,NULL,FALSE);
	    UpdateWindow(hWnd);
            return 0L;

        case WM_PAINT:
	    BeginPaint(hWnd,&rPS);
	    GetClientRect (hWnd,&rc1);
	    FrameRect(rPS.hdc,&rc1,GetStockObject(BLACK_BRUSH));
	    InflateRect(&rc1,-1,-1);
	    rc2 = rc1;
	    iRange = GetWindowWord (hWnd,BAR_RANGE);
            iPos   = GetWindowWord (hWnd,BAR_POS);

            if (iRange <= 0)
                iRange = 1;

	    if (iPos > iRange)	// make sure we don't go past 100%
	    	iPos = iRange;

	    dx = rc1.right;
	    dy = rc1.bottom;
	    x  = (WORD)((DWORD)iPos * dx / iRange) + 1;

	    wsprintf (ach,"%3d%%",(WORD)((DWORD)iPos * 100 / iRange));
	    dwExtent = GetTextExtent (rPS.hdc,ach,4);

	    rc1.right = x;
	    rc2.left  = x;

	    SetBkColor(rPS.hdc,COLORBG);
	    SetTextColor(rPS.hdc,COLORFG);
	    ExtTextOut (rPS.hdc,
		(dx-LOWORD(dwExtent))/2,(dy-HIWORD(dwExtent))/2,
		ETO_OPAQUE | ETO_CLIPPED,
		&rc1,
		ach,4,NULL);

            SetBkColor(rPS.hdc,COLORFG);
            SetTextColor(rPS.hdc,COLORBG);
	    ExtTextOut (rPS.hdc,
		(dx-LOWORD(dwExtent))/2,(dy-HIWORD(dwExtent))/2,
		ETO_OPAQUE | ETO_CLIPPED,
		&rc2,
		ach,4,NULL);

            EndPaint(hWnd,(LPPAINTSTRUCT)&rPS);
	    return 0L;
    }
    return DefWindowProc(hWnd,uiMessage,wParam,lParam);
}

/*----------------------------------------------------------------------------*\
|   ProOpen ()								       |
|                                                                              |
|   Description:                                                               |
|                                                                              |
|   Arguments:                                                                 |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
HWND FAR PASCAL ProOpen(HWND hWnd, int id)
{
#if 1
    HANDLE hInst;

    hInst = GetWindowWord(hWnd,GWW_HINSTANCE);
#else
    #define hInst hInstWS
#endif

    if (id == NULL)
       id = DLG_PROGRESS;

    iCnt++;
    if (!ghWnd) {
       fpxProDlg  = MakeProcInstance ((FARPROC)ProDlgProc,hInst);
       ghWnd = CreateDialog(hInst,MAKEINTRESOURCE(id),hWnd,fpxProDlg);
       if (ghWnd) {
	   ShowWindow (ghWnd,SHOW_OPENWINDOW);
	   UpdateWindow(ghWnd);
	   EnableWindow(hWnd, FALSE);
       }
    }
    ProSetBarRange(100);
    ProSetBarPos(0);
    return ghWnd;
}   

/*----------------------------------------------------------------------------*\
|   ProClose () 							       |
|                                                                              |
|   Description:                                                               |
|                                                                              |
|   Arguments:                                                                 |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL FAR PASCAL ProClose()
{
    iCnt--;
    if (ghWnd && iCnt == 0) {
	EnableWindow(GetParent(ghWnd), TRUE);
	DestroyWindow (ghWnd);
	FreeProcInstance (fpxProDlg);
	ghWnd = NULL;
    }
    return TRUE;
}

BOOL FAR PASCAL ProSetText (int i,LPSTR lpch)
{
    if (ghWnd) {
	SetDlgItemText (ghWnd,i,lpch);
	return TRUE;
    }
    return FALSE;
}

BOOL FAR cdecl ProPrintf (int i, LPSTR lpch, ...)
{
    char ach[200];
    if (ghWnd) {
        wvsprintf(ach, lpch, (LPSTR)(&lpch+1));
	SetDlgItemText(ghWnd, i, ach);
	return TRUE;
    }
    return FALSE;
}

BOOL FAR PASCAL ProSetBarRange (int i)
{
    if (ghWnd) {
        SendDlgItemMessage(ghWnd,ID_BAR,BAR_SETRANGE,i,0L);
	return TRUE;
    }
    return FALSE;
}

BOOL FAR PASCAL ProSetBarPos (int i)
{
    if (ghWnd) {
        SendDlgItemMessage(ghWnd,ID_BAR,BAR_SETPOS,i,0L);
	return TRUE;
    }
    return FALSE;
}

BOOL FAR PASCAL ProDeltaPos (int i)
{
    if (ghWnd) {
        SendDlgItemMessage(ghWnd,ID_BAR,BAR_DELTAPOS,i,0L);
	return TRUE;
    }
    return FALSE;
}
