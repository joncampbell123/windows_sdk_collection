/*
 * Clock.c -  toolkit version
 *
 * this version is the same as the retail clock program except for the
 * addition of two different methods of drawing the iconic clock.  This
 * version has a menu which lets the user choose either 1) direct to screen,
 * which is what the retail version uses, or 2) screen via bitmap, which
 * is included to give an example of how to use bitmaps and compatible dc's.
 * Screen via bitmap works by creating a compatible dc, creating a compatible
 * bitmap, assigning the bitmap to the new dc, having all following gdi
 * operations write to the new dc, and finally using bitblt to paint the
 * bitmap of the new dc onto the screen.  All of this example code can be
 * found in the routine ClockPaint().
 *
 *============================================================================*/

#include "windows.h"
#include "clock.h"

#define HOURSCALE	65
#define MINUTESCALE	80
#define RHOURSCALE	15
#define RMINUTESCALE	20
#define WHOURSCALE	7
#define WMINUTESCALE	5
#define HHAND		TRUE
#define MHAND		FALSE
#define SECONDSCALE	80
#define MAXBLOBWIDTH	25
#define REPAINT 	0
#define HANDPAINT	1

#define BUFLEN		15

long FAR PASCAL ClockWndProc( HWND, unsigned, WORD, LONG );
BOOL FAR PASCAL About( HWND, unsigned, WORD, LONG );

FARPROC lpprocAbout;
HANDLE hInst;

typedef struct {			/*structure for holding time*/
    int hour;
    int minute;
    int second;
} TIME;

extern void GetTime( TIME * );

HBRUSH hbrBackground;
HBRUSH hbrForeground;
HPEN   hpenForeground;
HPEN   hpenBackground;
BOOL   bFirst = TRUE;
BOOL   bIconic = FALSE;

int TimerID = 1;	/* number used for timer-id */
char szBuffer[BUFLEN];	/* buffer for stringtable stuff */
POINT FAR *lpcirTab;
HANDLE hcirTab;
TIME oTime;
RECT clockRect;
int clockRadius;
POINT clockCenter;
long aspectD;
long aspectN;
int VertRes, HorzRes;
int IconDrawMode = IDM_SCREEN;	    /* drawing clock icon directly to screen, */
				    /* or to bitmap first, then screen? */


BOOL FAR PASCAL About( hDlg, message, wParam, lParam )
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    if (message == WM_COMMAND) {
	EndDialog( hDlg, TRUE );
	return TRUE;
	}
    else if (message == WM_INITDIALOG)
	return TRUE;
    else return FALSE;
}


long FAR PASCAL ClockWndProc( hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    PAINTSTRUCT ps;
    HDC hDC;
    HMENU hMenu;

    switch (message)
    {
    case WM_SYSCOMMAND:
	switch (wParam)
	{
	case IDM_ABOUT:
	    DialogBox( hInst, MAKEINTRESOURCE(1), hWnd, lpprocAbout );
	    break;
	default:
	    goto defproc;
	}
	break;

    case WM_COMMAND:
	if (wParam == IDM_SCREEN || wParam == IDM_BITMAP)
	    {
	    hMenu = GetMenu(hWnd);
	    CheckMenuItem(hMenu, IconDrawMode, MF_UNCHECKED);
	    CheckMenuItem(hMenu, IconDrawMode = wParam, MF_CHECKED);
	    }
	break;

    case WM_SIZE:
	ClockSize( hWnd, LOWORD(lParam), HIWORD(lParam), wParam );
	UpdateWindow( hWnd );
	break;

    case WM_CREATE:
	ClockCreate( hWnd );
	break;

    case WM_DESTROY:
	KillTimer( hWnd, TimerID );
	DeleteTools();
	PostQuitMessage( 0 );
	break;

    case WM_PAINT:
	BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
	ClockPaint( hWnd, ps.hdc, REPAINT );
	EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
	break;

    case WM_TIMECHANGE:
    case WM_TIMER:
	ClockTimer( hWnd, wParam );
	break;

    case WM_SYSCOLORCHANGE:
	DeleteTools();
	CreateTools();
	break;

    case WM_ERASEBKGND:
	{
	RECT rect;
	GetClientRect( hWnd, (LPRECT)&rect );
	SelectObject( (HDC)wParam, hbrBackground );
	FillRect( (HDC)wParam, (LPRECT)&rect, hbrBackground );
	}
	break;

    default:
    defproc:
	return( DefWindowProc(hWnd, message, wParam, lParam) );
    }
    return( 0L );
}


CreateTools()
{
    hbrForeground  = CreateSolidBrush( GetSysColor(COLOR_WINDOWTEXT) );
    hbrBackground  = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
    hpenForeground = CreatePen( 0, 1, GetSysColor(COLOR_WINDOWTEXT) );
    hpenBackground = CreatePen( 0, 1, GetSysColor(COLOR_WINDOW) );
}


DeleteTools()
{
    DeleteObject( hbrForeground );
    DeleteObject( hbrBackground );
    DeleteObject( hpenForeground );
    DeleteObject( hpenBackground );
}


int ClockCreate( hWnd )
HWND hWnd;		  /* Handle to a window data structure */
{
    HDC     hDC;
    int     i;
    int     VertSize, HorzSize;
    POINT   FAR *pt;

    hDC = GetDC( hWnd );
    VertRes = GetDeviceCaps( hDC, VERTRES );
    HorzRes = GetDeviceCaps( hDC, HORZRES );
    VertSize= GetDeviceCaps( hDC, VERTSIZE );
    HorzSize= GetDeviceCaps( hDC, HORZSIZE );
    ReleaseDC( hWnd, hDC );

    aspectN = ((long)VertRes*100) / (long)VertSize;
    aspectD = ((long)HorzRes*100) / (long)HorzSize;

    CreateTools();

    /* Scale cosines for aspect ratio if this is the first instance */

    if (bFirst) {
	lpcirTab = (POINT far *)GlobalLock( hcirTab );
	for (i=0; i<60; i++) {
	    pt = lpcirTab+i;
	    pt->y = ((pt->y) * aspectN) / aspectD;
	    }
	GlobalUnlock( hcirTab );
	}

    GetTime( (TIME *) &oTime );
}


int ClockSize( hWnd, newWidth, newHeight, SizeWord )
HWND hWnd;
int newWidth, newHeight;
WORD  SizeWord;
{
    SetRect( (LPRECT)&(clockRect), 0, 0, newWidth, newHeight );
    CompClockDim();

    if(SizeWord == SIZEICONIC) {

	/* Update once every minute in the iconic state */

	KillTimer( hWnd, TimerID );
	SetTimer( hWnd, TimerID, (unsigned)59000, (long)0 );
	bIconic = TRUE;
	}
    else if (bIconic) {

	/* Update every second in the opened state.  Ignore tiling */

	KillTimer( hWnd, TimerID );
	SetTimer( hWnd, TimerID, 1000, (long)0 );
	bIconic = FALSE;
	}
}


int ClockTimer( hWnd, msg )
HWND hWnd;		  /* A handle to a window data structure  */
WORD msg;		  /* timer ID */
{
    TIME nTime;
    HDC hDC;

    GetTime( (TIME *) &nTime );

    /* It's possible to change any part of the system at any time
     * through the Control Panel.  So we check everything.
     */

    if (nTime.second == oTime.second &&
	nTime.minute == oTime.minute &&
	nTime.hour   == oTime.hour)
	return;

    hDC = GetDC( hWnd );
    ClockPaint( hWnd, hDC, HANDPAINT );
    ReleaseDC( hWnd, hDC );
}


int ClockPaint( hWnd, hDC, hint )
HWND	hWnd;
HDC	hDC;
int	hint;
{
    RECT    tRect;
    TIME    nTime;

    HDC     hTempDC;
    HDC     hMemDC;
    HBITMAP hBitmap;
    HANDLE  hOldObject;
    RECT    tempRect;

    if (bIconic & IconDrawMode==IDM_BITMAP)  /* example code follows...*/
	{
	/* since clockrect sometimes is modified, inflate temprect by
	 * some constant to make sure all gdi operations stay within
	 * the bitmap of the memory display context.
	 */
	CopyRect((LPRECT)&tempRect, (LPRECT)&clockRect);
	InflateRect((LPRECT)&tempRect, 15, 15);

	hMemDC = CreateCompatibleDC(hDC);
	hBitmap = CreateCompatibleBitmap(hDC, tempRect.right, tempRect.bottom);
	hOldObject = SelectObject(hMemDC, hBitmap);
	hTempDC = hDC;
	hDC = hMemDC;
	if (hint == REPAINT)  /* fill the whole temprect with white */
	    FillRect( hDC, (LPRECT)&tempRect, hbrBackground);
	}

    lpcirTab = (POINT far *)GlobalLock( hcirTab );

    SetBkMode( hDC, TRANSPARENT );
    if (hint == REPAINT){
	FillRect( hDC, (LPRECT)&clockRect, hbrBackground );
	DrawFace( hDC );
	DrawFatHand( hDC, oTime.hour*5+(oTime.minute/12),
		     hpenForeground, HHAND );
	DrawFatHand( hDC, oTime.minute,
		     hpenForeground, MHAND );

	if (!bIconic)
	    /* Erase old second hand */
	    DrawHand( hDC, oTime.second,
		     hpenBackground, SECONDSCALE, R2_NOT );
	}
    else
    if (hint == HANDPAINT){
	GetTime( (TIME *) &nTime );
	if ( (!bIconic) && nTime.second != oTime.second)
	    /* Erase old second hand */
	    DrawHand( hDC, oTime.second,
		      hpenBackground, SECONDSCALE, R2_NOT );

	if (nTime.minute != oTime.minute || nTime.hour != oTime.hour) {
	    if (bIconic) {
		DrawHand( hDC, oTime.minute,
			  hpenBackground, MINUTESCALE, R2_COPYPEN );
		DrawHand( hDC, oTime.hour*5+(oTime.minute/12),
			  hpenBackground, HOURSCALE, R2_COPYPEN );
		DrawHand( hDC, oTime.minute=nTime.minute,
			  hpenForeground, MINUTESCALE, R2_COPYPEN );
		DrawHand( hDC, (oTime.hour=nTime.hour)*5+(oTime.minute/12),
			  hpenForeground, HOURSCALE, R2_COPYPEN );
		}
	    else {
		DrawFatHand( hDC, oTime.minute,
			     hpenBackground, MHAND );
		DrawFatHand( hDC, oTime.hour*5+(oTime.minute/12),
			     hpenBackground, HHAND );
		DrawFatHand( hDC, oTime.minute=nTime.minute,
			     hpenForeground, MHAND );
		DrawFatHand( hDC, (oTime.hour=nTime.hour)*5+(oTime.minute/12),
			     hpenForeground, HHAND );
		}
	    }

	if (!bIconic && nTime.second != oTime.second)
	    /* Draw new second hand */
	    DrawHand( hDC, oTime.second=nTime.second,
		      hpenBackground, SECONDSCALE, R2_NOT );
	}

    if (bIconic & IconDrawMode==IDM_BITMAP) /* example code follows...*/
	{
	BitBlt(hTempDC, 0, 0, tempRect.right, tempRect.bottom, hMemDC, 0, 0, SRCCOPY);
	SelectObject(hMemDC, hOldObject);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	}

    GlobalUnlock( hcirTab );

}


DrawFace( hDC )
HDC hDC;
{
    int  i;
    RECT tRect;
    POINT FAR *pt;
    int  blobHeight, blobWidth;
    int  clockHeight, clockWidth;

    blobWidth = (MAXBLOBWIDTH * (long)(clockRect.right - clockRect.left)) / HorzRes;
    blobHeight = (blobWidth * aspectN) / aspectD;
    if (blobHeight < 2)
	blobHeight = 1;
    if (blobWidth < 2)
	blobWidth = 2;
    InflateRect( (LPRECT)&clockRect, -(blobHeight >> 1), -(blobWidth >> 1) );

    clockRadius = (clockRect.right - clockRect.left) >> 1;
    clockCenter.y = clockRect.top +
		       ((clockRect.bottom - clockRect.top) >> 1);
    clockCenter.x = clockRect.left + clockRadius;
    for (i=0; i<60; i++) {
	pt = lpcirTab + i;
	tRect.top = ((long)(pt->y) * clockRadius) / 8000 +
		    clockCenter.y;
	tRect.left = ((long)(pt->x) * clockRadius) / 8000 +
		     clockCenter.x;
	if (i % 5) {
			/*Draw a dot		 */
	    if (blobWidth > 2 && blobHeight >= 2) {
		tRect.right = tRect.left + 1;
		tRect.bottom = tRect.top + 1;
		FillRect( hDC, (LPRECT)&tRect, hbrForeground );
		}
	    }
	else {
	    tRect.right = tRect.left + blobWidth;
	    tRect.bottom = tRect.top + blobHeight;
	    OffsetRect( (LPRECT)&tRect, -(blobWidth >> 1) , -(blobHeight >> 1) );
	    FillRect( hDC, (LPRECT)&tRect, hbrForeground );
	    }
	}
	InflateRect( (LPRECT)&clockRect, (blobHeight >> 1), (blobWidth >> 1) );
}


DrawHand( hDC, pos, hPen, scale, patMode )
HDC	hDC;
int	pos;
HPEN	hPen;
int	scale;
int	patMode;
{
    POINT   FAR *pt;
    int radius;

    MoveTo( hDC, clockCenter.x, clockCenter.y );
    radius = ((long) clockRadius * scale) / 100;
    pt = lpcirTab+pos;
    SetROP2( hDC, patMode );
    SelectObject( hDC, hPen );
    LineTo( hDC, clockCenter.x+(int)(((long)(pt->x)*(radius))/8000),
	       clockCenter.y+(int)(((long)(pt->y)*(radius))/8000) );
}


DrawFatHand( hDC, pos, hPen, hHand )
HDC	hDC;
int	pos;
HPEN	hPen;
BOOL	hHand;
{
    POINT   *pt, tip, stip;
    int radius, scale;
    int n, m;

    SetROP2( hDC, 13 );
    SelectObject( hDC, hPen );
    scale = hHand ? 7 : 5;
    n = (pos+15)%60;
    m = (((long)clockRadius*scale) / 100);
    stip.y = (long)(lpcirTab[n].y) * m / 8000;

    stip.x = (long)(lpcirTab[n].x) * m / 8000;

    scale = hHand ? 65 : 80;
    tip.y = (long)(lpcirTab[pos].y)*(((long)clockRadius*scale)/100)/8000;
    tip.x = (long)(lpcirTab[pos].x)*(((long)clockRadius*scale)/100)/8000;

    MoveTo( hDC, clockCenter.x+stip.x, clockCenter.y+stip.y );
    LineTo( hDC, clockCenter.x+tip.x,  clockCenter.y+tip.y );
    MoveTo( hDC, clockCenter.x-stip.x, clockCenter.y-stip.y );
    LineTo( hDC, clockCenter.x+tip.x,  clockCenter.y+tip.y );

    scale = hHand ? 15 : 20;

    n = (pos+30)%60;
    m = (((long)clockRadius*scale) / 100);
    tip.y = (long)(lpcirTab[n].y) * m/ 8000;
    tip.x = (long)(lpcirTab[n].x) * m/ 8000;
    MoveTo( hDC, clockCenter.x+stip.x, clockCenter.y+stip.y );
    LineTo( hDC, clockCenter.x+tip.x,  clockCenter.y+tip.y );
    MoveTo( hDC, clockCenter.x-stip.x, clockCenter.y-stip.y );
    LineTo( hDC, clockCenter.x+tip.x,  clockCenter.y+tip.y );
}


CompClockDim()
{
    int clockHeight, clockWidth;
    int tWidth, tHeight;

    tWidth = clockRect.right-clockRect.left;
    tHeight = clockRect.bottom - clockRect.top;
    if (tWidth > (tHeight * aspectD) / aspectN) {
	clockWidth = (tHeight * aspectD) / aspectN;
	clockRect.left += (tWidth-clockWidth) >> 1;
	clockRect.right = clockRect.left + clockWidth;
	}
    else {
	clockHeight = (tWidth * aspectN) / aspectD;
	clockRect.top += (tHeight-clockHeight) >> 1;
	clockRect.bottom = clockRect.top + clockHeight;
	}
}


int PASCAL WinMain (hInstance, hPrev, lpszCmdLine, cmdShow)
HANDLE hInstance, hPrev;
LPSTR lpszCmdLine;
int cmdShow;
{
    HWND    hWnd;
    MSG     msg;
    HMENU   hMenu;
    char*   szTooMany;

    LoadString( hInstance, IDS_APPNAME, (LPSTR)szBuffer, BUFLEN );

    if (!hPrev) {
	if (!ClockInit( hInstance ))
	    return FALSE;
	}
    else {
	GetInstanceData( hPrev, (PSTR)&hcirTab, sizeof(HANDLE) );
	bFirst = FALSE;
	}

    hWnd = CreateWindow(
	(LPSTR)szBuffer,   /* The class name */
	(LPSTR)szBuffer,   /* The window instance name */
	(long)WS_TILEDWINDOW,
	0,		    /* The window wants to be opened in column zero */
	NULL,		    /*y not used*/
	NULL,		    /*cx not used*/
	100,		    /* A desired height of one hundred screen pixels */
	NULL,		    /* The window instance will be created with the class default menu */
	NULL,		    /*null parent window parm*/
	hInstance,
	(LPSTR)NULL );

    if ( !SetTimer(hWnd, TimerID, 1000, (long)0) ) {

	/* Windows only supports 16 public timers */

	szTooMany = (char *)LocalAlloc( LPTR, 40 );
	LoadString( hInstance, IDS_TOOMANY, (LPSTR)szTooMany, 40 );
	MessageBox( (HWND)NULL, (LPSTR)szTooMany, (LPSTR)szBuffer,
		     MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
	DeleteTools();
	return FALSE;
	}

    hInst = hInstance;
    lpprocAbout = MakeProcInstance( (FARPROC)About, hInstance );
    LoadString( hInstance, IDS_ABOUTMENU, (LPSTR)szBuffer, BUFLEN );
    hMenu = GetSystemMenu( hWnd, FALSE );
    ChangeMenu( hMenu, 0, NULL, 999, MF_APPEND | MF_SEPARATOR );
    ChangeMenu( hMenu, 0, (LPSTR)szBuffer, IDM_ABOUT, MF_APPEND | MF_STRING );

    ShowWindow( hWnd, cmdShow );

    while (GetMessage( (LPMSG)&msg, NULL, 0, 0)){
	TranslateMessage( (LPMSG)&msg );
	DispatchMessage( (LPMSG)&msg );
	}
    return(msg.wParam);
}


ClockInit( hInstance)
HANDLE hInstance;
{
    PWNDCLASS pClockClass;
    HANDLE hRes;

    pClockClass = (PWNDCLASS)LocalAlloc( LPTR, sizeof(WNDCLASS) );

    pClockClass->lpszClassName = (LPSTR)szBuffer;
    pClockClass->hbrBackground = (HBRUSH)NULL;
    pClockClass->style	       = CS_VREDRAW | CS_HREDRAW;
    pClockClass->hInstance     = hInstance;
    pClockClass->lpfnWndProc   = ClockWndProc;
    pClockClass->hCursor       = LoadCursor( NULL, IDC_ARROW );
    pClockClass->hIcon	       = NULL;
    pClockClass->lpszMenuName  = (LPSTR)"clock";

    if (!RegisterClass( (LPWNDCLASS)pClockClass ) )
	return FALSE;

    LocalFree( (HANDLE)pClockClass );

    /* Load in pre-computed cosine values from resource file */

    hRes = FindResource( hInstance, (LPSTR)MAKEINTRESOURCE(1), (LPSTR)"DATA");
    if (hRes)
	hcirTab = LoadResource( hInstance, hRes );
    else return FALSE;

    return TRUE;
}

