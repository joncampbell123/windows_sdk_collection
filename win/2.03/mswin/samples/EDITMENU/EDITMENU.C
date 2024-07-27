/****************************************************************************

    PROGRAM: Editmenu.c

    PURPOSE: Loads, saves, and edits text files

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	EditmenuInit() - initializes window data and registers window
	EditmenuWndProc() - processes messages
	About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "editmenu.h"

HANDLE hInst;

HANDLE hAccTable;			      /* handle to accelerator table */

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

	Sets DefSpec to "*.*" for use in the list box.	This will be changed
	whenever the user specifies a wild card in the OpenDlg Edit box.  One
	could also get the default extension from the WIN.INI file.

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int nCmdShow;
{

    HWND hWnd;
    MSG msg;
    HMENU hMenu;

    if (!hPrevInstance)
	if (!EditMenuInit(hInstance))
	    return (NULL);

    hInst = hInstance;

    hWnd = CreateWindow("EditMenu",
	"EditMenu Sample Application",
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

    /* Only translate message if it is not an accelerator message */

	if (!TranslateAccelerator(hWnd, hAccTable, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg); 
	}
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: EditmenuInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL EditMenuInit(hInstance)
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
    pWndClass->lpszClassName = (LPSTR) "EditMenu";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = EditMenuWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: EditmenuWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND    - message from application's menu
	WM_DESTROY    - destroy window

****************************************************************************/

long FAR PASCAL EditMenuWndProc(hWnd, message, wParam, lParam)
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
