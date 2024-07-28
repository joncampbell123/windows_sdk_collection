/****************************************************************************

    PROGRAM: Input.c

    PURPOSE: Input template for Windows applications

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "input.h"

HANDLE hInst;

char MouseText[48];                /* mouse state         */
char ButtonText[48];               /* mouse-button state  */
char KeyboardText[48];             /* keyboard state      */
char CharacterText[48];            /* latest character    */
char ScrollText[48];               /* scroll status       */
char TimerText[48];                /* timer state         */
RECT rectMouse;
RECT rectButton;
RECT rectKeyboard;
RECT rectCharacter;
RECT rectScroll;
RECT rectTimer;
int  idTimer;                      /* timer ID            */
int  nTimerCount = 0;              /* current timer count */

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
    MSG msg;

    if (!hPrevInstance)
	   if (!InitApplication(hInstance))
	      return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
	   TranslateMessage(&msg);
	   DispatchMessage(&msg);
    }
    return (msg.wParam);
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    wc.style = CS_DBLCLKS;          /* double-click messages */
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = COLOR_WINDOW+1;
    wc.lpszMenuName =  "InputMenu";
    wc.lpszClassName = "InputWClass";

    return (RegisterClass(&wc));
}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
HANDLE          hInstance;
int             nCmdShow;
{
    HWND            hWnd;
    HDC             hDC;
    TEXTMETRIC      textmetric;
    RECT            rect;
    int             nLineHeight;
 

    hInst = hInstance;

    hWnd = CreateWindow(
        "InputWClass",
        "Input Sample Application",
        WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,  /* horz & vert scroll bars */
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
        return (FALSE);


    hDC = GetDC(hWnd);
    if (!hDC)
        return FALSE;
    GetTextMetrics(hDC, &textmetric);
    nLineHeight = textmetric.tmExternalLeading + textmetric.tmHeight;

    rect.left   = GetDeviceCaps(hDC, LOGPIXELSX) / 4;   /* 1/4 inch */
    rect.right  = GetDeviceCaps(hDC, HORZRES);
    rect.top    = GetDeviceCaps(hDC, LOGPIXELSY) / 4;   /* 1/4 inch */
    ReleaseDC(hWnd, hDC);
    rect.bottom = rect.top + nLineHeight;
    rectMouse   = rect;

    rect.top += nLineHeight;
    rect.bottom += nLineHeight;
    rectButton = rect;

    rect.top += nLineHeight;
    rect.bottom += nLineHeight;
    rectKeyboard = rect;

    rect.top += nLineHeight;
    rect.bottom += nLineHeight;
    rectCharacter = rect;

    rect.top += nLineHeight;
    rect.bottom += nLineHeight;
    rectScroll = rect;

    rect.top += nLineHeight;
    rect.bottom += nLineHeight;
    rectTimer = rect;
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND    - application menu (About dialog box)
	WM_CREATE     - create window
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

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
{
    FARPROC lpProcAbout;

    HDC hDC;                         /* display-context variable     */
    PAINTSTRUCT ps;                  /* paint structure              */
    char HorzOrVertText[12];
    char ScrollTypeText[20];
    RECT rect;

    switch (message) {
	   case WM_COMMAND:
	      if (wParam == IDM_ABOUT) {
		      lpProcAbout = MakeProcInstance(About, hInst);

		      DialogBox(hInst,
		         "AboutBox",
		         hWnd,
		         lpProcAbout);

		      FreeProcInstance(lpProcAbout);
		      break;
	      }
	      else
		      return (DefWindowProc(hWnd, message, wParam, lParam));

      case WM_CREATE:

         /* Set the timer for five-second intervals */

         idTimer =  SetTimer(hWnd, NULL, 5000, (FARPROC) NULL);
         break;

      case WM_MOUSEMOVE:
         wsprintf(MouseText, "WM_MOUSEMOVE: %x, %d, %d",
             wParam, LOWORD(lParam), HIWORD(lParam));
         InvalidateRect(hWnd, &rectMouse, TRUE);
         break;

      case WM_LBUTTONDOWN:
         wsprintf(ButtonText, "WM_LBUTTONDOWN: %x, %d, %d",
                wParam, LOWORD(lParam), HIWORD(lParam));
         InvalidateRect(hWnd, &rectButton, TRUE);
         break;

      case WM_LBUTTONUP:
         wsprintf(ButtonText, "WM_LBUTTONUP: %x, %d, %d",
                wParam, LOWORD(lParam), HIWORD(lParam));
         InvalidateRect(hWnd, &rectButton, TRUE);
         break;

      case WM_LBUTTONDBLCLK:
         wsprintf(ButtonText, "WM_LBUTTONDBLCLK: %x, %d, %d",
                wParam, LOWORD(lParam), HIWORD(lParam));
         InvalidateRect(hWnd, &rectButton, TRUE);
         break;

      case WM_KEYDOWN:
          wsprintf(KeyboardText, "WM_KEYDOWN: %x, %x, %x",
              wParam, LOWORD(lParam), HIWORD(lParam));
          InvalidateRect(hWnd, &rectKeyboard, TRUE);
          break;

      case WM_KEYUP:
          wsprintf(KeyboardText, "WM_KEYUP: %x, %x, %x",
              wParam, LOWORD(lParam), HIWORD(lParam));
          InvalidateRect(hWnd, &rectKeyboard, TRUE);
          break;

      case WM_CHAR:
          wsprintf(CharacterText, "WM_CHAR: %c, %x, %x",
              wParam, LOWORD(lParam), HIWORD(lParam));
          InvalidateRect(hWnd, &rectCharacter, TRUE);
          break;

      case WM_TIMER:
          wsprintf(TimerText, "WM_TIMER: %d seconds",
              nTimerCount += 5);
          InvalidateRect(hWnd, &rectTimer, TRUE);
          break;

      case WM_HSCROLL:
      case WM_VSCROLL:
          lstrcpy(HorzOrVertText,
              (message == WM_HSCROLL) ? "WM_HSCROLL" : "WM_VSCROLL");
          lstrcpy(ScrollTypeText,
              (wParam == SB_LINEUP) ? "SB_LINEUP" :
              (wParam == SB_LINEDOWN) ? "SB_LINEDOWN" :
              (wParam == SB_PAGEUP) ? "SB_PAGEUP" :
              (wParam == SB_PAGEDOWN) ? "SB_PAGEDOWN" :
              (wParam == SB_THUMBPOSITION) ? "SB_THUMBPOSITION" :
              (wParam == SB_THUMBTRACK) ? "SB_THUMBTRACK" :
              (wParam == SB_ENDSCROLL) ? "SB_ENDSCROLL" : "unknown");
          wsprintf(ScrollText, "%s: %s, %x, %x",
              (LPSTR)HorzOrVertText,
              (LPSTR)ScrollTypeText,
              LOWORD(lParam),
              HIWORD(lParam));
          InvalidateRect(hWnd, &rectScroll, TRUE);
          break;

      case WM_PAINT:
          hDC = BeginPaint (hWnd, &ps);

          if (IntersectRect(&rect, &rectMouse, &ps.rcPaint))
              TextOut(hDC, rectMouse.left, rectMouse.top, 
                      MouseText, lstrlen(MouseText));
          if (IntersectRect(&rect, &rectButton, &ps.rcPaint))
              TextOut(hDC, rectButton.left, rectButton.top, 
                      ButtonText, lstrlen(ButtonText));
          if (IntersectRect(&rect, &rectKeyboard, &ps.rcPaint))
              TextOut(hDC, rectKeyboard.left, rectKeyboard.top, 
                      KeyboardText, lstrlen(KeyboardText));
          if (IntersectRect(&rect, &rectCharacter, &ps.rcPaint))
              TextOut(hDC, rectCharacter.left, rectCharacter.top, 
                      CharacterText, lstrlen(CharacterText));
          if (IntersectRect(&rect, &rectTimer, &ps.rcPaint))
              TextOut(hDC, rectTimer.left, rectTimer.top, 
                      TimerText, lstrlen(TimerText));
          if (IntersectRect(&rect, &rectScroll, &ps.rcPaint))
              TextOut(hDC, rectScroll.left, rectScroll.top, 
                      ScrollText, lstrlen(ScrollText));

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

