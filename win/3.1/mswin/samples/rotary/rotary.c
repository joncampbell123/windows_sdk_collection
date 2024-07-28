/*
    Rotary.c

    This is sample app to show how to implement
    non-rectangular controls.
    
    12-Aug-91   Created by NigelT
    
*/

#include <windows.h>
#include <math.h>
#include "Rotary.h"

#define IDC_CONTROL 1           // child id

// ROP codes

#define DSa     0x008800C6L
#define DSx     0x00660046L

// colors

#define rgbBlack RGB(0,0,0)
#define rgbWhite RGB(255,255,255)

/* usefull global things */

HANDLE hInst;                   /* global instance handle */
char *szAppName = "Rotary";
HWND hMainWnd;                  /* handle of main window */

HWND hwndControl;
BOOL bCaptured;
WORD wControl = (MIN_UNITS + MAX_UNITS) / 2;


// local functions

WORD CalcPos(WORD wMin, WORD wMax, int iXcenter, int iYcenter, int iXmouse, int iYmouse);
void CalcPoint(WORD wMin, WORD wMax, WORD wPos, int iXcenter, int iYcenter, int iRad, LPPOINT lpPt); 
void DrawStaticControl(HWND hWnd);
long FAR PASCAL RotaryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
long FAR PASCAL MainWndProc(HWND, UINT , WPARAM ,LPARAM);
BOOL InitFirstInstance(HANDLE);
HBITMAP GrabBackground(HWND hWnd);
BOOL TransBlt (HDC hdcD,int x,int y,int dx,int dy,HDC hdcS,int x0,int y0);


/***************** Main entry point routine *************************/

int PASCAL WinMain(hInstance,hPrevInstance,lpszCmdLine,cmdShow)
HANDLE hInstance,hPrevInstance;
LPSTR lpszCmdLine;
int cmdShow;
{
    MSG msg;

    hInst = hInstance;          /* save our instance handle */

    if (!hPrevInstance) {
        if (! InitFirstInstance(hInstance)) {
            return 1;
        }
    }

    /* create a window for the application */

    hMainWnd = CreateWindow(szAppName,          /* class name */
                        szAppName,              /* caption text */
                        WS_OVERLAPPEDWINDOW,    /* window style */
                        CW_USEDEFAULT, 0,
                        200, 250,
                        (HWND)NULL,             /* handle of parent window */
                        (HMENU)NULL,            /* menu handle (default class) */
                        hInstance,              /* handle to window instance */
                        (LPSTR)NULL             /* no params to pass on */
                        );
    
    if (!hMainWnd) {
        return 1;
    }

    ShowWindow(hMainWnd, cmdShow); /* display window as open or icon */
    UpdateWindow(hMainWnd);     /* paint it */
    
    // initialize the rotary control windows

    // show the window so it can grab the background

    ShowWindow(hwndControl, SW_NORMAL);

    // set the text, min and max values

    rcSetUnits(hwndControl, "Units");
    rcSetMin(hwndControl, MIN_UNITS);
    rcSetMax(hwndControl, MAX_UNITS);

    // set the current positions                      

    rcSetPos(hwndControl, wControl);

    /* Process messages for us */

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (msg.wParam);
}
    
/************* main window message handler ******************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
{
    PAINTSTRUCT ps;             /* paint structure */
    
    /* process any messages we want */

    switch(message) {
    case WM_CREATE:
    hwndControl = CreateWindow("rotary_control",
                              "Control",
                              WS_CHILD,
                              20, 20,
                              ROTARY_WIDTH, ROTARY_HEIGHT,
                              hWnd,
                              IDC_CONTROL,
                              hInst,
                              (LPSTR)NULL);
        break;

    case WM_SIZE:
        break;

    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return NULL;
}

BOOL InitFirstInstance(hInstance)
HANDLE hInstance;
{
    WNDCLASS wc;
    
    /* define the class of window we want to register */

    wc.lpszClassName    = szAppName;
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = LoadIcon(hInstance,"Icon");
    wc.lpszMenuName     = NULL;
    wc.hbrBackground    = (HBRUSH) (COLOR_APPWORKSPACE+1);
    wc.hInstance        = hInstance;
    wc.lpfnWndProc      = MainWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    
    if (! RegisterClass(&wc)) {
        return FALSE; /* Initialisation failed */
    }

    // register a class for the rotary controls

    wc.lpszClassName    = "rotary_control";
    wc.style            = 0;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = NULL;
    wc.lpszMenuName     = NULL;
    wc.hbrBackground    = NULL;
    wc.hInstance        = hInstance;
    wc.lpfnWndProc      = RotaryWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(NPRINFO);
    
    if (! RegisterClass(&wc)) {
        return FALSE; /* Initialisation failed */
    }
    
    return TRUE;
}

WORD CalcPos(WORD wMin, WORD wMax, int iXcenter, int iYcenter, int iXmouse, int iYmouse) 
{
    int dx, dy;
    WORD wPos;
    double a, min, max;

    // compute the x and y offsets
    dx = iXmouse - iXcenter;
    dy = iYmouse - iYcenter;

    // check for stupid case
    if ((dx == 0) && (dy == 0)) {
        dx = 1;
        dy = 1; // force a position that works
    }

    // get the angle in degrees
    a = (57.3 * atan2((double)dy, (double)dx));        
    if ((a > 45.0) && (a <= 90.)) {
        a = 45.0;
    } else if ((a > 90.0) && (a < 135.0)) {
        a = 135.0;
    }

    a -= 135.0;
    if (a < 0.0) a += 360.0;

    // compute the control value
    min = (double) wMin;
    max = (double) wMax;
    wPos = (WORD) (min + ((max - min) * a / 270.0));

    return wPos;

}

void CalcPoint(WORD wMin, WORD wMax, WORD wPos, int iXcenter, int iYcenter, int iRad, LPPOINT lpPt)  
{
    double a;  

    a = 2.36 + 4.71 * (double)(wPos - wMin) / (double)(wMax - wMin);

    lpPt->x = iXcenter + (int) ((double)iRad * cos(a));
    lpPt->y = iYcenter + (int) ((double)iRad * sin(a));

}

void DrawControl(HWND hWnd)
{
    HBITMAP hbmOld, hbmOff, hbmOldOff;
    HDC hDC, hdcMem, hdcOff;
    RECT rcClient;
    char text[40];
    HPEN hpenOld, hpenDot;
    HBRUSH hbrOld, hbrDot;
    POINT pt;
    NPRINFO npInfo;

    npInfo = (NPRINFO) GetWindowWord(hWnd, 0);
    if (!npInfo->hbmBkGnd) {
        return;
    }

    hDC = GetDC(hWnd);
    GetClientRect(hWnd, &rcClient);

    // create an off screen dc and a bitmap big enough

    hdcOff = CreateCompatibleDC(hDC);
    hbmOff = CreateCompatibleBitmap(hDC, 
                                    rcClient.right-rcClient.left,
                                    rcClient.bottom-rcClient.top);
    hbmOldOff = SelectObject(hdcOff, hbmOff);

    // blit the background image to the offscreen dc

    hdcMem = CreateCompatibleDC(hDC);
    hbmOld = SelectObject(hdcMem, npInfo->hbmBkGnd);

    BitBlt(hdcOff,
           rcClient.left,
           rcClient.top,
           rcClient.right-rcClient.left,
           rcClient.bottom-rcClient.top,
           hdcMem,
           0, 0, 
           SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);

    // Draw the control value at the bottom

    wsprintf(text, "%d %s", npInfo->wPos, (LPSTR)npInfo->szUnits);
    SetBkMode(hdcOff, TRANSPARENT);
    SetTextColor(hdcOff, BLUE);
    DrawText(hdcOff, text, -1, &rcClient, DT_SINGLELINE | DT_BOTTOM | DT_CENTER);

    // compute the angle of the dot

    CalcPoint(  npInfo->wMin, 
                npInfo->wMax, 
                npInfo->wPos,
                (rcClient.left + rcClient.right) / 2,
                (rcClient.top + rcClient.bottom) / 2,
                20, 
                &pt);

    hpenDot = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    hbrDot = CreateSolidBrush(RGB(255, 0, 0));
    hpenOld = SelectObject(hdcOff, hpenDot);
    hbrOld = SelectObject(hdcOff, hbrDot);
    Ellipse(hdcOff, pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
    SelectObject(hdcOff, hpenOld);
    SelectObject(hdcOff, hbrOld);
    DeleteObject(hpenDot);
    DeleteObject(hbrDot);

    // blit the offscreen dc stuff to the screen dc

    BitBlt(hDC,
           rcClient.left,
           rcClient.top,
           rcClient.right-rcClient.left,
           rcClient.bottom-rcClient.top,
           hdcOff,
           0, 0, 
           SRCCOPY);

    // tidy up the off screen stuff

    SelectObject(hdcOff, hbmOldOff);
    DeleteObject(hbmOff);
    DeleteDC(hdcOff);

    ReleaseDC(hWnd, hDC);
}


long FAR PASCAL RotaryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    RECT rcWnd;
    WORD w, xc, yc;
    NPRINFO npInfo;

    npInfo = (NPRINFO) GetWindowWord(hWnd, 0);

    switch(message) {
    case WM_CREATE:
        npInfo = (NPRINFO) LocalAlloc(LPTR, sizeof(RINFO));
        if (npInfo) {
            SetWindowWord(hWnd, 0, (WORD) npInfo);
            npInfo->wMin = 0;
            npInfo->wMax = 255;
            npInfo->wPos = 128;
            lstrcpy(npInfo->szUnits, "");
        }
        break;

    case WM_COMMAND:
        break;

    case WM_LBUTTONDOWN:
        if (!bCaptured) {
            SetCapture(hWnd);
            bCaptured = TRUE;
        }
        // fall through

    case WM_MOUSEMOVE:
        if (bCaptured) {
            // get the coords of the center of the control
            GetClientRect(hWnd, &rcWnd);
            xc = (rcWnd.left + rcWnd.right) / 2;
            yc = (rcWnd.top + rcWnd.bottom) / 2;
            w = CalcPos(npInfo->wMin,
                        npInfo->wMax,
                        xc,
                        yc,
                        LOWORD(lParam),
                        HIWORD(lParam));
            npInfo->wPos = w;
            DrawControl(hWnd);
            SendMessage(GetParent(hWnd),
                        RCN_DRAG,
                        GetWindowWord(hWnd, GWW_ID),
                        MAKELONG(hWnd, w));
        }
        break;

    case WM_LBUTTONUP:
        if (bCaptured) {
            ReleaseCapture();
            bCaptured = FALSE;
            SendMessage(GetParent(hWnd),
                        RCN_RELEASE,
                        GetWindowWord(hWnd, GWW_ID),
                        MAKELONG(hWnd, npInfo->wPos));
        }
        break;

    case RC_SETMIN:
        npInfo->wMin = wParam;
        w = npInfo->wPos;           
        if (w < wParam) {
            npInfo->wPos = wParam;
        }
//      DrawControl(hWnd);
        break;

    case RC_SETMAX:
        npInfo->wMax = wParam;
        w = npInfo->wPos;           
        if (w > wParam) {
            npInfo->wPos = wParam;
        }
//      DrawControl(hWnd);
        break;

    case RC_SETPOS:
        npInfo->wPos = wParam;
        DrawControl(hWnd);
        break;

    case RC_GETPOS:
        return MAKELONG(npInfo->wPos, 0);
        break;

    case RC_SETUNITS:
        lstrcpy(npInfo->szUnits, (LPSTR)lParam);
        break;

    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        DrawControl(hWnd);
        EndPaint(hWnd, &ps);
        break;

    case WM_SHOWWINDOW:
        // a good time to grab the background
        if (!npInfo->hbmBkGnd) {
            npInfo->hbmBkGnd = GrabBackground(hWnd);
            DrawStaticControl(hWnd);
        }
        break;

    case WM_ERASEBKGND:
        return TRUE; // say we erased it
        break;

    case WM_DESTROY:
        if (npInfo->hbmBkGnd) DeleteObject(npInfo->hbmBkGnd);
        LocalFree((HANDLE) npInfo);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    return NULL;
}


//
// Draw the bits of the control which don't vary on top of the
// background bitmap
//

void DrawStaticControl(HWND hWnd)
{
    NPRINFO npInfo;
    HDC hdcOffScr, hdcControl, hDC;
    HBITMAP hbmOldBkGnd, hbmOldControl, hbmControl;
    BITMAP bm;
    RECT rcClient;
    int x, y;
    char text[40];

    npInfo = (NPRINFO) GetWindowWord(hWnd, 0);
    hDC = GetDC(hWnd);
    hdcOffScr = CreateCompatibleDC(hDC);

    // select the raw background bmp into the dc
    hbmOldBkGnd = SelectObject(hdcOffScr, npInfo->hbmBkGnd);

    // draw the control and title on top of it

    // load the bitmap we need
    hbmControl = LoadBitmap(hInst, MAKEINTRESOURCE(IDR_CONTROL));
    if (!hbmControl) {
        return;
    }

    // get the size of the bitmap so we can center it
    GetObject(hbmControl, sizeof(bm), (LPSTR)&bm);

    // get the window size
    GetClientRect(hWnd, &rcClient);

    x = (rcClient.right - rcClient.left - bm.bmWidth) / 2 + rcClient.left;
    y = (rcClient.bottom - rcClient.top - bm.bmHeight) / 2 + rcClient.top;

    // draw the control
    hdcControl = CreateCompatibleDC(hdcOffScr);
    hbmOldControl = SelectObject(hdcControl, hbmControl);

    SetBkColor(hdcOffScr, RGB(0,0,255)); // transparency color is BLUE
    TransBlt(hdcOffScr, 
           x,
           y,
           bm.bmWidth,
           bm.bmHeight,
           hdcControl,
           0, 0);

    SelectObject(hdcControl, hbmOldControl);

    DeleteDC(hdcControl);
    DeleteObject(hbmControl);

    // Now put the caption at the top

    GetWindowText(hWnd, text, sizeof(text));
    SetBkMode(hdcOffScr, TRANSPARENT);
    SetTextColor(hdcOffScr, BLUE);
    DrawText(hdcOffScr, text, -1, &rcClient, DT_TOP | DT_CENTER);

    SelectObject(hdcOffScr, hbmOldBkGnd);
    DeleteDC(hdcOffScr);
    ReleaseDC(hWnd, hDC);
}

/**************************************************************************

    grab the background to a window

**************************************************************************/

HBITMAP GrabBackground(HWND hWnd)
{
    HDC hdcParent, hdcOffScr;
    HBITMAP hbmOld, hbm;
    RECT rc;
    POINT pt;

    GetClientRect(hWnd, &rc);
    hdcParent = GetDC(GetParent(hWnd));
    hdcOffScr = CreateCompatibleDC(hdcParent);
    hbm = CreateCompatibleBitmap(hdcParent,
                                 rc.right - rc.left,
                                 rc.bottom - rc.top);
    hbmOld = SelectObject(hdcOffScr, hbm);

    // grab a chunk of the main window dc

    pt.x = rc.left;
    pt.y = rc.top;
    ClientToScreen(hWnd, &pt);
    ScreenToClient(GetParent(hWnd), &pt);

    BitBlt(hdcOffScr,
           0, 0,
           rc.right-rc.left, rc.bottom-rc.top,
           hdcParent,
           pt.x, pt.y,
           SRCCOPY);

    SelectObject(hdcOffScr, hbmOld);
    DeleteDC(hdcOffScr);
    ReleaseDC(GetParent(hWnd), hdcParent);

    return hbm;
}

/*
 *
 * TransBlt() Transparent bitblt that uses the current
 *            background color of the DC as the transparent color.
 *
 */
BOOL TransBlt (HDC hdcD,int x,int y,int dx,int dy,HDC hdcS,int x0,int y0)
{
    DWORD       rgbBk,rgbFg;
    DWORD       rgbBkS;
    HBITMAP         hbmMask;
    HDC         hdcMask;
    HBITMAP         hbmT;
    BOOL        f = FALSE;

    //
    //  Get the current DC color's
    //
    rgbBk = GetBkColor(hdcD);
    rgbFg = GetTextColor(hdcD);
    rgbBkS= GetBkColor(hdcS);

    SetTextColor(hdcD,rgbBlack);

    //
    //  make a memory DC for use in color conversion
    //
    hdcMask = CreateCompatibleDC(hdcS);

    if (!hdcMask)
        return FALSE;

    //
    // create a mask bitmap and associated DC
    //
    hbmMask = CreateBitmap(dx, dy, 1, 1, NULL);

    if (!hbmMask)
        goto errorDC;

    // select the mask bitmap into the mono DC

    hbmT = SelectObject(hdcMask, hbmMask);

    // do a color to mono bitblt to build the mask
    // generate 1's where the source is equal to the background, else 0's

    SetBkColor(hdcS, rgbBk);
    BitBlt(hdcMask, 0, 0, dx, dy, hdcS, x0, y0, SRCCOPY);

    // do a MaskBlt to copy the bitmap to the dest

    SetBkColor(hdcD,rgbWhite);
    BitBlt(hdcD,x,y,dx,dy,hdcS,   x0,y0,DSx);
    BitBlt(hdcD,x,y,dx,dy,hdcMask,0 ,0 ,DSa);
    BitBlt(hdcD,x,y,dx,dy,hdcS,   x0,y0,DSx);

    f = TRUE;

    SelectObject(hdcMask, hbmT);
    DeleteObject(hbmMask);

    //
    // Restore the DC colors
    //
    SetBkColor(hdcS,rgbBkS);
    SetBkColor(hdcD,rgbBk);
    SetTextColor(hdcD,rgbFg);

errorDC:
    DeleteDC(hdcMask);

    return f;
}
