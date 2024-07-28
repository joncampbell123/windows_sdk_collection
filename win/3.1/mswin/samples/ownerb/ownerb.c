/*
    Ownerb.c

    This is sample app to show how owner draw buttons work.
    
    12-Aug-91   Updated by NigelT to make it simpler.
    
*/

#include <windows.h>
#include "Ownerb.h"

#define IDC_BUTTON  1           // child id


void DrawControl(HWND hWnd, LPDRAWITEMSTRUCT lpInfo);


/* usefull global things */

HANDLE hInst;                   /* global instance handle */
char *szAppName = "Ownerb";
HWND hMainWnd;                  /* handle of main window */

HWND hwndButton;

// local functions

long FAR PASCAL MainWndProc(HWND, UINT , WPARAM, LPARAM);
BOOL InitFirstInstance(HANDLE);

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
                        200, 100,
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
    hwndButton = CreateWindow("button",
                              "Button",
                              WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                              40, 20,
                              40, 20,
                              hWnd,
                              IDC_BUTTON,
                              hInst,
                              (LPSTR)NULL);
        break;

    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;

    case WM_DRAWITEM:
        // owner draw control stuff
        DrawControl(hWnd, (LPDRAWITEMSTRUCT)lParam);
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

void DrawControl(HWND hWnd, LPDRAWITEMSTRUCT lpInfo)
{
    HBITMAP hbm, hOldbm;
    int ResourceID;
    HDC hMemDC;


    if (lpInfo->CtlType != ODT_BUTTON) return;

    // Load the bitmap for the image

    switch(lpInfo->CtlID) {
    case IDC_BUTTON:
        ResourceID = IDR_BUTTON;
        break;

    default:
        return;
    }

    if (lpInfo->itemState & ODS_SELECTED) {
        ResourceID += 1;
    }

    hbm = LoadBitmap(hInst, MAKEINTRESOURCE(ResourceID));
    if (!hbm) return;

    if ((lpInfo->itemAction & ODA_DRAWENTIRE)
    || (lpInfo->itemAction & ODA_SELECT)) {

        // draw the whole button
        hMemDC = CreateCompatibleDC(lpInfo->hDC);
        hOldbm = SelectObject(hMemDC, hbm);
        if (hOldbm) {
            BitBlt(lpInfo->hDC, 
                   (lpInfo->rcItem).left,
                   (lpInfo->rcItem).top,
                   (lpInfo->rcItem).right - (lpInfo->rcItem).left,
                   (lpInfo->rcItem).bottom - (lpInfo->rcItem).top,
                   hMemDC,
                   0, 0,
                   SRCCOPY);
            SelectObject(hMemDC, hOldbm);
        }
        DeleteDC(hMemDC);
    }
    DeleteObject(hbm); 
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
    wc.lpszMenuName     = "Menu";
    wc.hbrBackground	= COLOR_WINDOW+1;
    wc.hInstance        = hInstance;
    wc.lpfnWndProc      = MainWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    
    if (! RegisterClass(&wc)) {
        return FALSE; /* Initialisation failed */
    }
    
    return TRUE;
}
