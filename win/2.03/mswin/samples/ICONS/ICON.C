/****************************************************************************

    PROGRAM: Icon.c

    PURPOSE: Demonstrates using an icon for the About box and minimize icon

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	IconInit() - initializes window data and registers window
	IconWndProc() - processes messages
	About() - processes messages for "About" dialog box

****************************************************************************/

#include "windows.h"
#include "icon.h"

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
	if (!IconInit(hInstance))
	    return (NULL);

    hInst = hInstance;

    hWnd = CreateWindow("Icon",
	"Icon Sample Application",
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

    FUNCTION: IconInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL IconInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(hInstance, "Icon");	       /* loads icon */
    pWndClass->lpszMenuName = (LPSTR) NULL;
    pWndClass->lpszClassName = (LPSTR) "Icon";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = IconWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: IconWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window and objects
	WM_DESTROY    - destroy window

****************************************************************************/

long FAR PASCAL IconWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;
    HMENU hMenu;

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
	    hMenu = GetSystemMenu(hWnd, FALSE);
	    ChangeMenu(hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
	    ChangeMenu(hMenu, NULL, "A&bout Icon...", ID_ABOUT,
		MF_APPEND | MF_STRING);
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
	    if (wParam == IDOK) {
		EndDialog(hDlg, TRUE);
		return (TRUE);
	    }
	    break;
    }
    return (FALSE);
}
