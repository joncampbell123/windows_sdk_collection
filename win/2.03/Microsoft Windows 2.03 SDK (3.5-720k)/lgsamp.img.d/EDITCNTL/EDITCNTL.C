/****************************************************************************

    PROGRAM: Editcntl.c

    PURPOSE: Creates an edit window

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	EditcntlInit() - initializes window data and registers window
	EditcntlWndProc() - processes messages
	About() - processes messages for "About" dialog box

    COMMENTS:

	After setting up the application's window, the size of the client
	area is determined and a child window is created to use for editing.

****************************************************************************/

#include "windows.h"
#include "editcntl.h"

HANDLE hInst;
HANDLE hAccTable;

HWND hEditWnd;					    /* handle to edit window */

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
    RECT Rect;

    if (!hPrevInstance)
	if (!EditCntlInit(hInstance))
	    return (NULL);

    hInst = hInstance;

    hWnd = CreateWindow("EditCntl",
	"EditCntl Sample Application",
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

    GetClientRect(hWnd, (LPRECT) &Rect);

    /* Create a child window */

    hEditWnd = CreateWindow("Edit",
	NULL,
	WS_CHILD | WS_VISIBLE |
	ES_MULTILINE |
	WS_VSCROLL | WS_HSCROLL |
	ES_AUTOHSCROLL | ES_AUTOVSCROLL,
	0,
	0,
	(Rect.right-Rect.left),
	(Rect.bottom-Rect.top),
	hWnd,
	ID_EDIT,
	hInst,
	NULL);

    if (!hEditWnd) {
	DestroyWindow(hWnd);
	return (NULL);
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, NULL, NULL)) {
	if (!TranslateAccelerator(hWnd, hAccTable, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg); 
	}
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: EditcntlInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL EditCntlInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "EditMenu";
    pWndClass->lpszClassName = (LPSTR) "EditCntl";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = EditCntlWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: EditcntlWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window and objects
	WM_DESTROY    - destroy window
	WM_SIZE	      - window size has changed

    COMMENTS:

	When a WM_SIZE message is received, the edit window must be resized
	to match the parent's client area.

****************************************************************************/

long FAR PASCAL EditCntlWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;

    switch (message) {
	case WM_CREATE:
	    hAccTable = LoadAccelerators(hInst, "EditMenu");
	    break;

	case WM_COMMAND:
	    switch (wParam) {

		/* file menu commands */

		case IDM_NEW:
		case IDM_OPEN:
		case IDM_SAVE:
		case IDM_SAVEAS:
		case IDM_PRINT:
		    MessageBox(hWnd, "Command not implemented",
			(LPSTR) NULL, MB_OK);
		    break;  

		case IDM_EXIT:
		    DestroyWindow(hWnd);
		    break;
    
		case IDM_ABOUT:
		    lpProcAbout = MakeProcInstance(About, hInst);
		    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
		    FreeProcInstance(lpProcAbout);
		    break;

		/* edit menu commands */

		case IDM_UNDO:
		case IDM_CUT:
		case IDM_COPY:
		case IDM_PASTE:
		case IDM_CLEAR:
		    MessageBox(hWnd, "Command not implemented",
			(LPSTR) NULL, MB_OK);
		    break;  
	    } 
	    break;

	case WM_SIZE:
	    MoveWindow(hEditWnd, 0, 0, LOWORD(lParam), HIWORD(lParam), FALSE);
	    break;

	case WM_DESTROY:
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
