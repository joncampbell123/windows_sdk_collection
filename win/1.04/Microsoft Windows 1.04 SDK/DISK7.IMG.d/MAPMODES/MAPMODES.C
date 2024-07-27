/*  Mapmodes.c
    Mapmodes Application
    Microsoft Windows Version 1.03
    Copyright (c) Microsoft 1985	*/

/*
 * MAPMODES is a simple program to let the user experiment with different
 * mapping modes and coordinate systems used by GDI.   It lets the user
 * set the window origin and extents, the viewport origin and extents, the
 * mapping mode (8 choices), and the logical coordinates of a displayed
 * rectangle.
 *
 * The user should refer to the Microsoft Windows Programming guide, Appendix
 * C, GDI Definitions, for a full discussion on coordinate systems and
 * mapping modes.   But here is a very brief summary anyways:
 *
 * GDI uses a logical coordinate system to draw all images before mapping
 * them to the actual physical device coordinate system.  To draw images
 * on the display device, the user uses GDI functions to draw in the logical
 * coordinate system.  GDI then maps the image to the device.  There are
 * two "rectangles" which define what area of each coordinate system to
 * display.  In the logical coordinate system the "window" defines the region
 * which is to be drawn to the display device.	In the physical device
 * coordinate system, the "viewport" defines what region of the device to
 * map the window onto.   Thus all window functions use logical coordinates,
 * while all viewport functions use device units (pixels).
 *
 * Mapping modes set up the window and viewport extents into common
 * configurations.  There are three classes of mapping modes:
 *  1)	Completely Constrained -
 *	all changes to the window or viewport extents are ignored.
 *	used to map logical units to actual physical sizes (millimeters or inches)
 *	includes: MM_TEXT, MM_LO/HIMETRIC, MM_LO/HIENGLISH, MM_TWIPS
 *
 *  2)	Partially Constrained -
 *	user can change window and viewport extents, but GDI makes adjustments
 *	to ensure that logical units are mapped to equally scaled axes.
 *	includes: MM_ISOTROPIC
 *
 *  3)	Unconstrained -
 *	user must specify the desired units, orientation, and scaling.
 *	includes: MM_ANISOTROPIC
 *
 *
 * MAPMODES lets the user experiment with all these different modes, origins,
 * and extents and it shows the logical coordinates and equivalent device
 * coordinates of the cursor, when the user presses the mouse left button.
 *
 *============================================================================*/





#include "windows.h"
#include "mapmodes.h"	    /* definitions of dialog box control IDs	*/

LONG FAR PASCAL MapModesWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL SetupDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL RectDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL AboutDialog(HWND, unsigned, WORD, LONG);

typedef BOOL FAR * LPBOOL;

LPSTR FAR PASCAL lstrcpy(LPSTR, LPSTR);
LPSTR FAR PASCAL lstrcat(LPSTR, LPSTR);
int   FAR PASCAL lstrlen(LPSTR);

/*
** The following variables are initialized once when the application
** is first loaded and then copied into each succeeding instance, using
** GetInstanceData procedure
*/
HBRUSH hbrWhite;
HBRUSH hbrBlack;
HANDLE hAccelTable;  /* handle to accelerator table */

HPEN	hpenBlack;
HWND	hWndMapMode;
FARPROC lpprocSetupDlg;
FARPROC lpprocRectDlg;
FARPROC lpprocAbout;
HANDLE	hInst;

/* global values holding current DC modes */
int wMapMode  = IDMTEXT;		  /* def = text mode */
int wWinOrgX  = 0;
int wWinOrgY  = 0;
int wWinExtX  = 640;
int wWinExtY  = 200;
int wViewOrgX = 0;
int wViewOrgY = 0;
int wViewExtX = 640;
int wViewExtY = 200;

int wRectLeft = 50;
int wRectTop  = 50;
int wRectRight = 150;
int wRectBottom = 150;

char rgchPosX[ 60 ];
char rgchPosY[ 60 ];
char rgchBuffer[ 150 ];
char szAppName[ 10 ];
char szAbout[ 10 ];
char szWindowTitle[ 25 ];

/*=============================================================================
 MAPMODEPAINT is the function which specifies to GDI the current mapping mode,
 the window origin and extents, and the viewport origin and extents.  It also
 draws a polygon, using the coordinates of the rectangle to base its location.
=============================================================================*/
MapModePaint(hWnd, hDC, fErase)
HWND hWnd;		/* a handle to a window data structure	*/
HDC  hDC;		/* a handle to a GDI display context	*/
BOOL fErase;
{
    RECT    r;
    LPRECT  lpRectangle = (LPRECT)&r;

    /* inform GDI of the mapping mode */
    SetMapMode(hDC, wMapMode - 0x100);

    /* these calls are ignored if the mapping mode is
       not equal to mm_isotropic or mm_anisotropic  */
    SetWindowOrg(hDC, wWinOrgX, wWinOrgY);
    SetWindowExt(hDC, wWinExtX, wWinExtY);
    SetViewportOrg(hDC, wViewOrgX, wViewOrgY);
    SetViewportExt(hDC, wViewExtX, wViewExtY);

    /* White-out screen and draw polygon */
    GetClientRect(hWnd, lpRectangle);
    FillRect(hDC, lpRectangle, hbrWhite);
    MoveTo(hDC, wRectLeft, wRectTop);
    LineTo(hDC, wRectLeft, wRectBottom);
    LineTo(hDC, (wRectLeft + wRectRight)/2, wRectBottom);
    LineTo(hDC, (wRectLeft + wRectRight)/2, (wRectTop + wRectBottom)/2);
    LineTo(hDC, wRectRight,  (wRectTop + wRectBottom)/2);
    LineTo(hDC, wRectLeft, wRectTop);
}


int MapModeCommand(hWnd, wParam)
HWND hWnd;		/* A handle to a window data structure */
WORD wParam;
{
    HMENU   hMenu;
    int     i;

    switch (wParam)
    {
    case IDMTEXT:	    /* MAPMODE menu item */
    case IDMLOMETRIC:	    /* MAPMODE menu item */
    case IDMHIMETRIC:	    /* MAPMODE menu item */
    case IDMLOENGLISH:	    /* MAPMODE menu item */
    case IDMHIENGLISH:	    /* MAPMODE menu item */
    case IDMTWIPS:	    /* MAPMODE menu item */
    case IDMISOTROPIC:	    /* MAPMODE menu item */
    case IDMANISOTROPIC:    /* MAPMODE menu item */

	hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu, wMapMode, MF_UNCHECKED); /* uncheck old map mode */
	wMapMode = wParam;			      /* set up new map mode */
	CheckMenuItem(hMenu, wMapMode, MF_CHECKED);   /* check new map mode */

	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
	break;

    case IDMDRAW:  /* DRAW menu item */
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
	break;

    case IDMSETUP:  /* SETUP menu item */
	DialogBox(hInst, MAKEINTRESOURCE(SETUPBOX), hWnd, lpprocSetupDlg);
	break;

    case IDMRECTANGLE: /* RECTANGLE menu item */
	DialogBox(hInst, MAKEINTRESOURCE(RECTBOX), hWnd, lpprocRectDlg);
	break;

    default:
	break;
    }
}


/* when the user clicks the mousebutton, put out both the
 * logical & device pt coordinates of where the cursor is.
 */
MapModeMouse(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
POINT lParam;
{
    POINT point;	 /* Mouse click point */
    HDC   hDC;		 /* DC for writing mouse click coordinates */
    LPRECT lpCoordRect;  /* Area to be erased before displaying coords */
    RECT  WndRect;	 /* Dimensions of Mapmode Window needed for */
			 /* displaying the mouse coordinates */
    int top, left, bottom, right;
    int mTop, mLeft, mBottom, mRight;

    point.x = lParam.x;
    point.y = lParam.y;
    hDC = GetDC(hWnd);

    SetMapMode(hDC, wMapMode - 0x100);	    /* set to user specified mode */
    SetWindowOrg(hDC, wWinOrgX, wWinOrgY);
    SetWindowExt(hDC, wWinExtX, wWinExtY);
    SetViewportOrg(hDC, wViewOrgX, wViewOrgY);
    SetViewportExt(hDC, wViewExtX, wViewExtY);

    DPtoLP(hDC, (LPPOINT) &point, 1); /* device point to logical point */
    itoa(point.x, rgchPosX, 10);
    itoa(point.y, rgchPosY, 10);

    SetMapMode(hDC, MM_TEXT);  /* put back in text mode so we know where to output text */
    SetWindowOrg(hDC, 0, 0);
    SetViewportOrg(hDC, 0, 0);
    GetClientRect(hWnd, (LPRECT)&WndRect);

    /* White out mouse-click coordinates display area */
    if ((WndRect.right - WndRect.left) > 450)
	mLeft = ((WndRect.right - WndRect.left)/4 * 3);
    else if
	((WndRect.right - WndRect.left) > 280)
	mLeft = ((WndRect.right - WndRect.left)/2);
    else
	mLeft = (WndRect.left + 15);
    mTop = (WndRect.top + 10);
    mRight = (WndRect.right - 5);
    mBottom = (WndRect.top + 50);

    PatBlt(hDC, mLeft, mTop, mRight,
	       mBottom, WHITENESS);

    /* Display Logical coordinates */
    lstrcpy((LPSTR)rgchBuffer, (LPSTR)"LC: ");
    lstrcat((LPSTR)rgchBuffer, (LPSTR)"(");
    lstrcat((LPSTR)rgchBuffer, (LPSTR)rgchPosX);
    lstrcat((LPSTR)rgchBuffer, (LPSTR)",");
    lstrcat((LPSTR)rgchBuffer, (LPSTR)rgchPosY);
    lstrcat((LPSTR)rgchBuffer, (LPSTR)")");
    TextOut(hDC, mLeft, mTop, (LPSTR)rgchBuffer,
		lstrlen((LPSTR)rgchBuffer));

    itoa(lParam.x, rgchPosX, 10);
    itoa(lParam.y, rgchPosY, 10);

    /* Display mouse-click coordinates */
    lstrcpy((LPSTR)rgchBuffer, (LPSTR)"DC: ");
    lstrcat((LPSTR)rgchBuffer, (LPSTR)"(");
    lstrcat((LPSTR)rgchBuffer, (LPSTR)rgchPosX);
    lstrcat((LPSTR)rgchBuffer, (LPSTR)",");
    lstrcat((LPSTR)rgchBuffer, (LPSTR)rgchPosY);
    lstrcat((LPSTR)rgchBuffer, (LPSTR)")");
    TextOut(hDC, mLeft, mTop+20, (LPSTR)rgchBuffer,
		lstrlen((LPSTR)rgchBuffer));
    ReleaseDC(hWnd, hDC);
}


/*===========================================================================
 The SetUp Dialog Box lets the user specify the window origin, window extents,
 viewport origin, and viewport extents.  These are irrelevant though, if the
 mapmode is not MM_ISOTROPIC or MM_ANISOTROPIC.
=============================================================================*/

BOOL FAR PASCAL SetupDialog (hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message)
    {
    case WM_INITDIALOG:
	SetDlgItemInt(hDlg, IDWINORGX, wWinOrgX, TRUE);
	SetDlgItemInt(hDlg, IDWINORGY, wWinOrgY, TRUE);
	SetDlgItemInt(hDlg, IDWINEXTX, wWinExtX, TRUE);
	SetDlgItemInt(hDlg, IDWINEXTY, wWinExtY, TRUE);
	SetDlgItemInt(hDlg, IDVIEWORGX, wViewOrgX, TRUE);
	SetDlgItemInt(hDlg, IDVIEWORGY, wViewOrgY, TRUE);
	SetDlgItemInt(hDlg, IDVIEWEXTX, wViewExtX, TRUE);
	SetDlgItemInt(hDlg, IDVIEWEXTY, wViewExtY, TRUE);
	break;

    case WM_COMMAND:
	SetDlgCommand(hDlg, wParam, lParam);
	break;

    default:
	return(FALSE);
	break;
    }
    return(TRUE);
}


SetDlgCommand(hDlg, wParam, lParam)
HWND hDlg;
WORD wParam;
LONG lParam;
{
    BOOL fValOK;

    switch (wParam)
    {
    case IDOK:
	wWinOrgX  = GetDlgItemInt(hDlg, IDWINORGX, (LPBOOL)&fValOK, TRUE);
	wWinOrgY  = GetDlgItemInt(hDlg, IDWINORGY, (LPBOOL)&fValOK, TRUE);
	wWinExtX  = GetDlgItemInt(hDlg, IDWINEXTX, (LPBOOL)&fValOK, TRUE);
	wWinExtY  = GetDlgItemInt(hDlg, IDWINEXTY, (LPBOOL)&fValOK, TRUE);
	wViewOrgX = GetDlgItemInt(hDlg, IDVIEWORGX, (LPBOOL)&fValOK, TRUE);
	wViewOrgY = GetDlgItemInt(hDlg, IDVIEWORGY, (LPBOOL)&fValOK, TRUE);
	wViewExtX = GetDlgItemInt(hDlg, IDVIEWEXTX, (LPBOOL)&fValOK, TRUE);
	wViewExtY = GetDlgItemInt(hDlg, IDVIEWEXTY, (LPBOOL)&fValOK, TRUE);

	EndDialog(hDlg, TRUE);
	InvalidateRect(hWndMapMode, NULL, TRUE);
	UpdateWindow(hWndMapMode);
	break;

    case IDCANCEL:
	EndDialog(hDlg, TRUE);
	InvalidateRect(hWndMapMode, NULL, TRUE);
	UpdateWindow(hWndMapMode);
	break;

    default:
	break;
    }
}


#define xrat 20
SpewLines(hDlg)   /* draws neat looking lines over the about dialog box! */
HWND hDlg;
{
    HDC hdc;
    RECT r;
    unsigned xdiv, count, x;

    hdc = GetDC(hDlg);
    SetROP2(hdc, R2_NOTXORPEN);
    GetClientRect(hDlg, (LPRECT)&r);
    xdiv = (r.right - r.left)/xrat;

    for (x=0; x < r.right; x+=xdiv) {
	MoveTo(hdc, r.left, r.bottom);
	LineTo(hdc, r.left + x, 0);
	LineTo(hdc, r.right, r.bottom);
	MoveTo(hdc, r.right, r.top);
	LineTo(hdc, r.right - x, r.bottom);
	LineTo(hdc, r.left, r.top);
	}
    ReleaseDC(hDlg, hdc);
}


BOOL FAR PASCAL AboutDialog(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message)
    {
    case WM_INITDIALOG:
	break;

    case WM_COMMAND:
	SpewLines(hDlg);	/* cover box with neat display */
	EndDialog(hDlg, TRUE);
	break;

    default:
	return FALSE;
	break;
    }
    return(TRUE);
}

/*=============================================================================
 The Rectangle Dialog Box lets the user specify the upper left corner and lower
 right corner coordinates for a rectangle.  The rectangle coordinates are used
 to draw the polygon on the display.
=============================================================================*/

BOOL FAR PASCAL RectDialog (hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message)
    {
    case WM_INITDIALOG:
	SetDlgItemInt(hDlg, IDRECTLEFT, wRectLeft, TRUE);
	SetDlgItemInt(hDlg, IDRECTTOP, wRectTop, TRUE);
	SetDlgItemInt(hDlg, IDRECTRIGHT, wRectRight, TRUE);
	SetDlgItemInt(hDlg, IDRECTBOTTOM, wRectBottom, TRUE);
	break;

    case WM_COMMAND:
	RectDlgCommand(hDlg, wParam, lParam);
	break;

    default: return (FALSE);
    }
    return TRUE;
}


RectDlgCommand(hDlg, wParam, lParam)
HWND hDlg;
WORD wParam;
LONG lParam;
{
    BOOL fValOK;

    switch (wParam)
    {
    case IDOK:
	wRectLeft = GetDlgItemInt(hDlg, IDRECTLEFT, (LPBOOL)&fValOK, TRUE);
	wRectTop  = GetDlgItemInt(hDlg, IDRECTTOP, (LPBOOL)&fValOK, TRUE);
	wRectRight = GetDlgItemInt(hDlg, IDRECTRIGHT, (LPBOOL)&fValOK, TRUE);
	wRectBottom = GetDlgItemInt(hDlg, IDRECTBOTTOM, (LPBOOL)&fValOK, TRUE);
	/* fall through */

    case IDCANCEL:
	EndDialog(hDlg, TRUE);
	InvalidateRect(hWndMapMode, NULL, TRUE);
	UpdateWindow(hWndMapMode);
	break;

    default:
	break;
    }
}


/****************************************************************************/
/* No more Windows entry points 					    */
/****************************************************************************/

MapModeInit(hInstance)
HANDLE	  hInstance;
{
    HMENU	hmenu;
    PWNDCLASS	pMapModeClass;

    /* Set up some default brushes */
    hbrWhite  = GetStockObject(WHITE_BRUSH);
    hbrBlack  = GetStockObject(BLACK_BRUSH);
    hpenBlack = GetStockObject(BLACK_PEN);

    /* Loading from string table */
    LoadString( hInstance, IDSNAME, (LPSTR)szAppName, 10 );
    LoadString( hInstance, IDSABOUT, (LPSTR)szAbout, 10 );
    LoadString( hInstance, IDSTITLE, (LPSTR)szWindowTitle, 25 );

    /* Allocate class structure in local heap */
    pMapModeClass = (PWNDCLASS)LocalAlloc(LPTR, sizeof(WNDCLASS));

    pMapModeClass->hCursor  = LoadCursor(NULL, IDC_ARROW);
    pMapModeClass->hIcon    = LoadIcon(hInstance, (LPSTR)szAppName);
    pMapModeClass->lpszMenuName  = (LPSTR)szAppName;

    pMapModeClass->lpszClassName  = (LPSTR)szAppName;
    pMapModeClass->hbrBackground  = (HBRUSH) hbrWhite;
    pMapModeClass->hInstance	  = hInstance;

    /* The class chooses to always redraw its contents whenever the window  */
    /* is sized 							    */
    pMapModeClass->style  = CS_VREDRAW | CS_HREDRAW;

    pMapModeClass->lpfnWndProc	  = MapModesWndProc;

    /* register this new class with WINDOWS */
    if (!RegisterClass((LPWNDCLASS)pMapModeClass))
	return FALSE;	/* Initialization failed */

    LocalFree((HANDLE)pMapModeClass);

    hAccelTable = LoadAccelerators(hInstance, (LPSTR)szAppName);

    return TRUE;    /* Initialization succeeded */
}


int PASCAL WinMain(hInstance, hPrev, lpszCommand, cmdShow)
HANDLE hInstance, hPrev;
LPSTR  lpszCommand;
int    cmdShow;
{
    HWND  hWnd;
    MSG   msg;
    HMENU hMenu;

    if (!hPrev)
    {
	if (!MapModeInit(hInstance))
	    return FALSE;
    }
    else
    {
	/* Copy global instance variables from previous instance */
	GetInstanceData(hPrev, (PSTR)&hbrWhite,    sizeof(hbrWhite));
	GetInstanceData(hPrev, (PSTR)&hbrBlack,    sizeof(hbrBlack));
	GetInstanceData(hPrev, (PSTR)&hInst,	   sizeof(hInst));
	GetInstanceData(hPrev, (PSTR)&hAccelTable, sizeof(hAccelTable));
	GetInstanceData(hPrev, (PSTR)szAppName,    10 );
	GetInstanceData(hPrev, (PSTR)szAbout,	   10 );
	GetInstanceData(hPrev, (PSTR)szWindowTitle,25 );
	}

    /* Create a window instance of class "MapMode" */
    hWnd = (HWND)CreateWindow(
	(LPSTR)szAppName,	/* The class name */
	(LPSTR)szWindowTitle,	/* The window instance name */
	WS_TILEDWINDOW,
	0,
	0,
	0,
	0,
	(HMENU)NULL,	   /* no parent window */
	(HANDLE)NULL,	   /* use class default menu */
	(HANDLE)hInstance, /* handle to instance's window */
	(LPSTR)NULL	   /* no params to pass on */
       );

    /* initialize global */
    hWndMapMode = hWnd;
    hInst = hInstance;

    /* set up for handling dialog boxes */
    lpprocSetupDlg = MakeProcInstance((FARPROC)SetupDialog, hInst);
    lpprocRectDlg = MakeProcInstance((FARPROC)RectDialog, hInst);
    lpprocAbout = MakeProcInstance((FARPROC)AboutDialog, hInst);
    /* Add 'About' to system menu */
    hMenu = GetSystemMenu(hWnd, FALSE);
    ChangeMenu(hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR);
    ChangeMenu(hMenu, 0, (LPSTR)szAbout, IDSABOUT, MF_APPEND | MF_STRING);

	/* Make window visible */
    ShowWindow(hWnd, cmdShow);
    UpdateWindow(hWnd);

    /* WM_QUIT message return FALSE and terminate loop */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0)) {
	if (TranslateAccelerator(hWnd, hAccelTable, (LPMSG)&msg) == 0) {
	    TranslateMessage((LPMSG)&msg);
	    DispatchMessage((LPMSG)&msg);
	    }
	}
    exit(msg.wParam);
}

LONG FAR PASCAL MapModesWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;

    switch (message) {

    case WM_SYSCOMMAND:
	switch (wParam) {
	    case IDSABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(ABOUTBOX), hWnd, lpprocAbout);
		break;
	    default:
		return(DefWindowProc(hWnd, message, wParam, lParam));
		break;
	    }
	break;

    case WM_DESTROY:
	PostQuitMessage(0);
	break;

    case WM_PAINT:
	BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
	MapModePaint(hWnd, ps.hdc, TRUE);
	EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
	break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
	MapModeMouse(hWnd, message, wParam, MAKEPOINT(lParam));
	break;

    case WM_COMMAND:
	MapModeCommand(hWnd, wParam);
	break;

    default:
	return(DefWindowProc(hWnd, message, wParam, lParam));
	break;
    }
    return(0L);
}
