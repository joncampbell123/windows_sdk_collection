/****************************************************************************

    PROGRAM: Handtest.c

    PURPOSE: Demonstrates the use of the HANDLER.DLL

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	DemoInit() - initializes window data and registers window
	DemoWndProc() - processes messages
	About() - processes messages for "About" dialog box


****************************************************************************/
#include "windows.h"
#include "handtest.h"
#include "handler.h"

HANDLE hInst;


int PASCAL WinMain (hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{
     HWND hWnd;
     MSG msg;

     if (!hPrevInstance)
          if (!DemoInit (hInstance))
               return (NULL);
     hInst = hInstance;
     hWnd = CreateWindow ("HandlerDemo", "Handler Demonstration",
                          WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                          500, 200, NULL, NULL, hInstance, NULL);
     if (!hWnd)
          return (NULL);
     ShowWindow (hWnd, nCmdShow);
     UpdateWindow (hWnd);
     while (GetMessage (&msg, NULL, NULL, NULL)) {
          TranslateMessage (&msg);
          DispatchMessage (&msg);
     }
     return (msg.wParam);
}

/****************************************************************************

    FUNCTION: DemoInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/


BOOL DemoInit (hInstance)
HANDLE hInstance;
{
     HANDLE hMemory;
     PWNDCLASS pWndClass;
     BOOL bSuccess;

     hMemory = LocalAlloc (LPTR, sizeof (WNDCLASS));
     pWndClass = (PWNDCLASS) LocalLock (hMemory);
     pWndClass->hCursor = LoadCursor (NULL, IDC_ARROW);
     pWndClass->hIcon = LoadIcon (NULL, IDI_APPLICATION);
     pWndClass->lpszMenuName = (LPSTR) "Menu";
     pWndClass->lpszClassName = (LPSTR) "HandlerDemo";
     pWndClass->hbrBackground = COLOR_WINDOW+1;
     pWndClass->hInstance = hInstance;
     pWndClass->style = NULL;
     pWndClass->lpfnWndProc = DemoWndProc;
     bSuccess = RegisterClass (pWndClass);
     LocalUnlock (hMemory);
     LocalFree (hMemory);
     return (bSuccess);
}

/****************************************************************************

    FUNCTION: DemoWndProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages


****************************************************************************/


long FAR PASCAL DemoWndProc (hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
{
     FARPROC lpProcAbout;
     int nCount;
     HDC hDC;
     TEXTMETRIC tm;
     static short cxChar, cyChar;
     char szBuffer[80];

     switch (message) {
     case WM_CREATE:                     /* Register Window handle   */
          SetISRWindow (hWnd);             /* with handler DLL         */
          SetTimer (hWnd, 1, 2000, NULL);     /* Set a time-out for display */
          hDC = GetDC (hWnd);
          GetTextMetrics (hDC, &tm);
          cxChar = tm.tmAveCharWidth;
          cyChar = tm.tmHeight + tm.tmExternalLeading;
          ReleaseDC (hWnd, hDC);
          break;
     case ISRM_RUPT:                     /* Got a message from HANDLER */
          hDC = GetDC (hWnd);
          nCount = wParam;
	  TextOut (hDC, cxChar, cyChar * 2, szBuffer, wsprintf (szBuffer,
                   "Total interrupts = %d", nCount));
          ReleaseDC (hWnd, hDC);
          break;
     case WM_TIMER:                      /* Update display occasionally */
          hDC = GetDC (hWnd);
          nCount = GetISRCount ();
	  TextOut (hDC, cxChar, cyChar * 2, szBuffer, wsprintf (szBuffer,
                   "Total interrupts = %d", nCount));
          ReleaseDC (hWnd, hDC);
          break;
     case WM_COMMAND:
          switch (wParam) {
          case IDM_ABOUT:
               lpProcAbout = MakeProcInstance (About, hInst);
               DialogBox (hInst, "AboutBox", hWnd, lpProcAbout);
               FreeProcInstance (lpProcAbout);
               break;
          }
          break;
     case WM_DESTROY:
          KillTimer (hWnd, 1);
          PostQuitMessage (NULL);
          break;
     default:
          return (DefWindowProc (hWnd, message, wParam, lParam));
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


BOOL FAR PASCAL About (hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
     switch (message) {
     case WM_INITDIALOG:
          return (TRUE);
     case WM_COMMAND:
          if (wParam == IDOK || wParam == IDCANCEL) {
               EndDialog (hDlg, TRUE);
               return (TRUE);
          }
          return (TRUE);
     }
     return (FALSE);
}
