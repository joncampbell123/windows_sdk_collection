/****************************************************************************

    PROGRAM: Input.c

    PURPOSE: Demonstrates various input capabilities of Windows

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        InputInit() - initializes window data and registers window
        InputWndProc() - processes messages
        About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "input.h"

HANDLE hInst;

char MouseText[40];                                   /* mouse state         */
char ButtonText[40];                                  /* mouse-button state  */
char KeyboardText[40];                                /* keyboard state      */
char CharacterText[40];                               /* latest character    */
char ScrollText[40];                                  /* scroll status       */
char TimerText[40];                                   /* timer state         */
int idTimer;                                          /* timer ID            */
int nTimerCount = 0;                                  /* current timer count */

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{

    HWND hWnd;
    MSG msg;

    if (!hPrevInstance)
        if (!InputInit(hInstance))
            return (FALSE);

    hInst = hInstance;

    hWnd = CreateWindow("Input",
        "Input Sample Window",
        WS_OVERLAPPEDWINDOW |
        WS_HSCROLL | WS_VSCROLL,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hWnd)
        return (FALSE);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: InputInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL InputInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) NULL;
    pWndClass->lpszClassName = (LPSTR) "Input";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = CS_DBLCLKS;                  /* double-click messages */
    pWndClass->lpfnWndProc = InputWndProc;

    bSuccess = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: InputWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

        WM_SYSCOMMAND - system menu (About dialog box)
        WM_CREATE     - create window and objects
        WM_MOUSEMOVE  - mouse movement
        WM_LBUTTONDOWN - left mouse button pressed
        WM_LBUTTONUP  - left mouse button released
        WM_LBUTTONDBLCLK - left mouse button double clicked
        WM_KEYDOWN    - key pressed
        WM_KEYUP      - key released
        WM_CHAR       - ASCII character received
        WM_TIMER      - timer has elapsed
        WM_HSCROLL    - mouse click in horizontal scroll bar
        WM_VSCROLL    - mouse click in vertical scroll bar
        WM_PAINT      - update window, draw objects
        WM_DESTROY    - destroy window

    COMMENTS:

        This demonstrates how input messages are received, and what the
        additional information is that comes with the message.

****************************************************************************/

long FAR PASCAL InputWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;
    HMENU hMenu;

    HDC hDC;                                     /* display-context variable */
    PAINTSTRUCT ps;                              /* paint structure          */

    switch (message) {
        case WM_SYSCOMMAND:
            if (wParam == ID_ABOUT) {
                lpProcAbout = MakeProcInstance(About, hInst);
                DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                FreeProcInstance(lpProcAbout);
                break;
            }
            else
                return (DefWindowProc(hWnd, message, wParam, lParam));

        case WM_CREATE:

            /* Set the timer for five-second intervals */

            idTimer =  SetTimer(hWnd, NULL, 5000, (FARPROC) NULL);

            hMenu = GetSystemMenu(hWnd, FALSE);
            ChangeMenu(hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
            ChangeMenu(hMenu, NULL, (LPSTR) "A&bout Input...", ID_ABOUT,
                MF_APPEND | MF_STRING);
            break;

        case WM_MOUSEMOVE:
            sprintf(MouseText, "WM_MOUSEMOVE: %x, %d, %d     ",
                wParam, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_LBUTTONDOWN:
            sprintf(ButtonText, "WM_LBUTTONDOWN: %x, %d, %d     ",
                wParam, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_LBUTTONUP:
            sprintf(ButtonText, "WM_LBUTTONUP: %x, %d, %d     ",
                wParam, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_LBUTTONDBLCLK:
            sprintf(ButtonText, "WM_LBUTTONDBLCLK: %x, %d, %d     ",
                wParam, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_KEYDOWN:
            sprintf(KeyboardText, "WM_KEYDOWN: %x, %x, %x     ",
                wParam, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_KEYUP:
            sprintf(KeyboardText, "WM_KEYUP: %x, %x, %x     ",
                wParam, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_CHAR:
            sprintf(CharacterText, "WM_CHAR: %c, %x, %x     ",
                wParam, LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_TIMER:
            sprintf(TimerText, "WM_TIMER: %d seconds     ",
                nTimerCount += 5);
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_HSCROLL:
        case WM_VSCROLL:
            sprintf(ScrollText, "%s: %s, %x, %x     ",
                (message == WM_HSCROLL) ? "WM_HSCROLL" : "WM_VSCROLL",
                (wParam == SB_LINEUP) ? "SB_LINEUP" :
                (wParam == SB_LINEDOWN) ? "SB_LINEDOWN" :
                (wParam == SB_PAGEUP) ? "SB_PAGEUP" :
                (wParam == SB_PAGEDOWN) ? "SB_PAGEDOWN" :
                (wParam == SB_THUMBPOSITION) ? "SB_THUMBPOSITION" :
                (wParam == SB_THUMBTRACK) ? "SB_THUMBTRACK" :
                (wParam == SB_ENDSCROLL) ? "SB_ENDSCROLL" : "unknown",
                LOWORD(lParam),
                HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case WM_PAINT:
            hDC = BeginPaint (hWnd, &ps);

            TextOut(hDC, 20, 20, MouseText, strlen(MouseText));
            TextOut(hDC, 20, 40, ButtonText, strlen(ButtonText));
            TextOut(hDC, 20, 60, KeyboardText, strlen(KeyboardText));
            TextOut(hDC, 20, 80, CharacterText, strlen(CharacterText));
            TextOut(hDC, 20, 100, TimerText, strlen(TimerText));
            TextOut(hDC, 20, 120, ScrollText, strlen(ScrollText));

            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            KillTimer(hWnd, idTimer);                     /* Stops the timer */
            PostQuitMessage(0);
            break;

        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}
