/****************************************************************************

    PROGRAM: EditCntl.c

    PURPOSE: Creates an edit window

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	About() - processes messages for "About" dialog box

    COMMENTS:

	After setting up the application's window, the size of the client
	area is determined and a child window is created to use for editing.

****************************************************************************/

#include "windows.h"
#include "editcntl.h"

HANDLE hInst;

HANDLE hAccTable;                                /* handle to accelerator table */
HWND hEditWnd;					    /* handle to edit window */
HWND hwnd;                    /* handle to main windows  */

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

    /* Only translate message if it is not an accelerator message */

        if (!TranslateAccelerator(hwnd, hAccTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
        }
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

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
    wc.lpszMenuName =  "EditCntlMenu";
    wc.lpszClassName = "EditCntlWClass";

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
    RECT            Rect;

    hInst = hInstance;

    hAccTable = LoadAccelerators(hInst, "EditCntlAcc");

    hwnd = CreateWindow(
        "EditCntlWClass",
        "EditCntl Sample Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
        return (FALSE);

    GetClientRect(hwnd, (LPRECT) &Rect);

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
	hwnd,
	IDC_EDIT,                          /* Child control i.d. */
	hInst,
	NULL);

    if (!hEditWnd) {
	DestroyWindow(hwnd);
	return (NULL);
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND    - application menu (About dialog box)
	WM_DESTROY    - destroy window

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
UINT message;
WPARAM wParam;
LPARAM lParam;
{
    FARPROC lpProcAbout;

    switch (message) {
	case WM_COMMAND:
            switch (wParam) {
                case IDM_ABOUT:
                    lpProcAbout = MakeProcInstance(About, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;

                /* file menu commands */

                case IDM_NEW:
                case IDM_OPEN:
                case IDM_SAVE:
                case IDM_SAVEAS:
                case IDM_PRINT:
                    MessageBox (
                          GetFocus(),
                          "Command not implemented",
                          "EditCntl Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  

                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
    
                /* edit menu commands */

                case IDM_UNDO:
                case IDM_CUT:
                case IDM_COPY:
                case IDM_PASTE:
                case IDM_CLEAR:
                    MessageBox (
                          GetFocus(),
                          "Command not implemented",
                          "EditCntl Sample Application",
                          MB_ICONASTERISK | MB_OK);
                    break;  

                case IDC_EDIT:
                    if (HIWORD (lParam) == EN_ERRSPACE) {
                        MessageBox (
                              GetFocus ()
                            , "Out of memory."
                            , "EditCntl Sample Application"
                            , MB_ICONHAND | MB_OK
                        );
                    }
                    break;

            } 
            break;

        case WM_SETFOCUS:
            SetFocus (hEditWnd);
            break;

        case WM_SIZE:
            MoveWindow(hEditWnd, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
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
	    if (wParam == IDOK
                || wParam == IDCANCEL) {
		EndDialog(hDlg, TRUE);
		return (TRUE);
	    }
	    break;
    }
    return (FALSE);
}

