/****************************************************************************

    PROGRAM: Filemenu.c

    PURPOSE: Displays a file menu

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	FilemenuInit() - initializes window data and registers window
	FilemenuWndProc() - processes messages
	About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "filemenu.h"

HANDLE hInst;

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
	if (!FileMenuInit(hInstance))
	    return (NULL);

    hInst = hInstance;

    hWnd = CreateWindow("FileMenu",
	"FileMenu Sample Application",
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

    FUNCTION: FilemenuInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL FileMenuInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "FileMenu";
    pWndClass->lpszClassName = (LPSTR) "FileMenu";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = FileMenuWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: FilemenuWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND - command
	WM_DESTROY - destroy window

****************************************************************************/

long FAR PASCAL FileMenuWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;

    switch (message) {

	case WM_COMMAND:
	    switch (wParam) {

		/* File menu commands */

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
