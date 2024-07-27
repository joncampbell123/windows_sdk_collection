/****************************************************************************

    PROGRAM: Output.c

    PURPOSE: Demonstrates various output capabilities of Windows

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	OutputInit() - initializes window data and registers window
	OutputWndProc() - processes messages
	About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "output.h"

HANDLE hInst;

HPEN hDashPen;					       /* "---" pen handle   */
HPEN hDotPen;					       /* "..." pen handle   */
HBRUSH hOldBrush;				       /* old brush handle   */
HBRUSH hRedBrush;				       /* red brush handle   */
HBRUSH hGreenBrush;				       /* green brush handle */
HBRUSH hBlueBrush;				       /* blue brush handle  */

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
	if (!OutputInit(hInstance))
	    return (FALSE);

    hInst = hInstance;

    hWnd = CreateWindow("Output",
	"Output Sample Window",
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

    FUNCTION: OutputInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL OutputInit(hInstance)
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
    pWndClass->lpszClassName = (LPSTR) "Output";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = OutputWndProc;

    bSuccess = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: OutputWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window and objects
	WM_PAINT      - update window, draw objects
	WM_DESTROY    - destroy window

    COMMENTS:

	Handles to the objects you will use are obtained when the WM_CREATE
	message is received, and deleted when the WM_DESTROY message is
	received.  The actual drawing is done whenever a WM_PAINT message is
	received.

****************************************************************************/

long FAR PASCAL OutputWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;
    HMENU hMenu;

    HDC hDC;					/* display-context variable  */
    PAINTSTRUCT ps;				/* paint structure	     */
    LPSTR lpText = "Hello Windows!  This is a very long line indeed.";
    RECT rcTextBox;				/* rectangle around the text */
    HPEN hOldPen;				/* old pen handle	     */

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

	    /* Create the brush objects */

	    hRedBrush =	  CreateSolidBrush(RGB(255,   0,   0));
	    hGreenBrush = CreateSolidBrush(RGB(	 0, 255,   0));
	    hBlueBrush =  CreateSolidBrush(RGB(	 0,   0, 255));

	    /* Create the "---" pen */

	    hDashPen = CreatePen(1,				    /* style */
		1,						    /* width */
		RGB(0, 0, 0));					    /* color */

	    /* Create the "..." pen */

	    hDotPen = CreatePen(2,				    /* style */
		1,						    /* width */
		RGB(0, 0, 0));					    /* color */

	    hMenu = GetSystemMenu(hWnd, FALSE);
	    ChangeMenu(hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
	    ChangeMenu(hMenu, NULL, "A&bout Output...", ID_ABOUT,
		MF_APPEND | MF_STRING);
	    break;

	case WM_PAINT:

	    /* Set up a display context to begin painting */

	    hDC = BeginPaint (hWnd, &ps);

	    /* Send characters to the screen */

	    TextOut(hDC, 1, 1, lpText, _lstrlen(lpText));

	    /* Put text in a rectangle and display it */

	    /* First define the size of the rectangle around the text */

	    SetRect(&rcTextBox, 1, 10, 161, 44);

	    /* Send the text, displayed within a box */

	    DrawText(hDC, lpText, _lstrlen(lpText),
		&rcTextBox, DT_LEFT | DT_WORDBREAK);

	    /* Draw a red rectangle */

	    hOldBrush = SelectObject(hDC, hRedBrush);
	    Rectangle (hDC, 10, 50, 60, 80);

	    /* Draw a green ellipse */

	    SelectObject(hDC, hGreenBrush);
	    Ellipse (hDC, 160, 50, 210, 80);

	    /* Draw a blue pie shape */

	    SelectObject(hDC, hBlueBrush);
	    Pie (hDC, 310, 50, 360, 100, 360, 50, 360, 100);

	    /* Restore the old brush */

	    SelectObject(hDC, hOldBrush);

	    /* Select a "---" pen, save the old value */

	    hOldPen = SelectObject(hDC, hDashPen);

	    /* Move to a specified point */

	    MoveTo(hDC, 10, 110);

	    /* Draw a line */

	    LineTo(hDC, 360, 110);

	    /* Select a "..." pen */

	    SelectObject(hDC, hDotPen);

	    /* Draw an arc connecting the line */

	    Arc(hDC, 10, 90, 360, 130, 10, 110, 360, 110);

	    /* Restore the old pen */

	    SelectObject(hDC, hOldPen);

	    /* Other graphics functions could go here */

	    /* Tell Windows you are done painting */

	    EndPaint (hWnd,  &ps);
	    break;

	case WM_DESTROY:
	    DeleteObject(hRedBrush);
	    DeleteObject(hGreenBrush);
	    DeleteObject(hBlueBrush);
	    DeleteObject(hDashPen);
	    DeleteObject(hDotPen);
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

/****************************************************************************

    FUNCTION: _lstrlen(LPSTR)

    PURPOSE:  uses a far pointer to the string, returns the length

    COMMENTS:

	Because you are compiling with the small model, the normal string
	handling functions will not handle far pointers.  This function would
	be unneccesary if compiled using a medium or larger model.

****************************************************************************/

int _lstrlen(lpString)
LPSTR lpString;
{
    int i;
    for (i=0; *lpString++; ++i);
    return (i);
}
