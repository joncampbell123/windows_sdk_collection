/****************************************************************************

    PROGRAM: Demo.c

    PURPOSE: Demonstrates how to manipulate a cursor and select a region

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	DemoInit() - initializes window data and registers window
	DemoWndProc() - processes messages
	About() - processes messages for "About" dialog box

    COMMENTS:
	This code is a modified version of the CURSOR.C program.  Instead of
	using inline code for drawing the shape, the routines from the Select
	library are called.

****************************************************************************/

#include "windows.h"
#include "demo.h"
#include "select.h"

HANDLE hInst;
BOOL bTrack = FALSE;
int OrgX = 0, OrgY = 0;
int PrevX = 0, PrevY = 0;
int X = 0, Y = 0;

RECT Rect;

int Shape = SL_BLOCK;			       /* Shape to use for rectangle */
BOOL RetainShape = FALSE;		       /* Retain or destroy shape    */

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{

    HWND hWnd;
    MSG msg;

    if (!hPrevInstance)
	if (!DemoInit(hInstance))
	    return (NULL);

    hInst = hInstance;

    hWnd = CreateWindow("Demo",
	"Demo Sample Application",
	WS_OVERLAPPEDWINDOW,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	CW_USEDEFAULT,
	NULL,
	NULL,
	hInstance,
	NULL);

    if (!hWnd)
	return (NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: DemoInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL DemoInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "Menu";
    pWndClass->lpszClassName = (LPSTR) "Demo";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = DemoWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: DemoWndProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window
	WM_DESTROY    - destroy window
	WM_LBUTTONDOWN - left mouse button
	WM_MOUSEMOVE   - mouse movement
	WM_LBUTTONUP   - left button released

	WM_COMMAND messages:
	    IDM_BOX    - use inverted box for selecting a region
	    IDM_BLOCK  - use empty box for selecting a region
	    IDM_RETAIN - retain/delete selection on button release

    COMMENTS:

	When the left mouse button is pressed, btrack is set to TRUE so that
	the code for WM_MOUSEMOVE will keep track of the mouse and update the
	box accordingly.  Once the button is released, btrack is set to
	FALSE, and the current position is saved.  Holding the SHIFT key
	while pressing the left button will extend the current box rather
	then erasing it and starting a new one.	 The exception is when the
	retain shape option is enabled.	 With this option, the rectangle is
	zeroed whenever the mouse is released so that it can not be erased or
	extended.

****************************************************************************/

long FAR PASCAL DemoWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
{
    FARPROC lpProcAbout;
    HMENU hMenu;

    switch (message) {

	case WM_COMMAND:
	    switch (wParam) {
		case IDM_BOX:
		    Shape = SL_BOX;
		    hMenu = GetMenu(hWnd);
		    CheckMenuItem(hMenu, IDM_BOX, MF_CHECKED);
		    CheckMenuItem(hMenu, IDM_BLOCK, MF_UNCHECKED);
		    break;

		case IDM_BLOCK:
		    Shape = SL_BLOCK;
		    hMenu = GetMenu(hWnd);
		    CheckMenuItem(hMenu, IDM_BOX, MF_UNCHECKED);
		    CheckMenuItem(hMenu, IDM_BLOCK, MF_CHECKED);
		    break;

		case IDM_RETAIN:
		    if (RetainShape) {
			hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, IDM_RETAIN, MF_UNCHECKED);
			RetainShape = FALSE;
		    }
		    else {
			hMenu = GetMenu(hWnd);
			CheckMenuItem(hMenu, IDM_RETAIN, MF_CHECKED);
			RetainShape = TRUE;
		    }
		    break;

		case IDM_ABOUT:
		    lpProcAbout = MakeProcInstance(About, hInst);
		    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
		    FreeProcInstance(lpProcAbout);
		    break;

	    }
	    break;

	case WM_LBUTTONDOWN:

	    bTrack = TRUE;		 /* user has pressed the left button */

	    /* If you don't want the shape cleared, you must clear the Rect
	     * coordinates before calling StartSelection
	     */

	    if (RetainShape)
		SetRectEmpty(&Rect);

	    StartSelection(hWnd, MAKEPOINT(lParam), &Rect,
		(wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
	    break;

	case WM_MOUSEMOVE:
	    if (bTrack)
		UpdateSelection(hWnd, MAKEPOINT(lParam), &Rect, Shape);
	    break;

	case WM_LBUTTONUP:
       if (bTrack) 
	       EndSelection(MAKEPOINT(lParam), &Rect);
   	 bTrack = FALSE;
	    break;

   case WM_SIZE:
      switch (wParam) {
         case SIZEICONIC:

            /* If we aren't in retain mode we want to clear the 
             * current rectangle now! 
             */
            if (!RetainShape)
               SetRectEmpty(&Rect);
      }
      break;

	case WM_DESTROY:
	    PostQuitMessage(NULL);
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
	    if (wParam == IDOK
                || wParam == IDCANCEL) {
		EndDialog(hDlg, TRUE);
		return (TRUE);
	    }
	    return (TRUE);
    }
    return (FALSE);
}

