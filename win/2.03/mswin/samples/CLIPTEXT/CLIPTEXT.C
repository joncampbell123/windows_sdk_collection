/****************************************************************************

    PROGRAM: Cliptext.c

    PURPOSE: Demonstrates copying text to and from the clipboard

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	CliptextInit() - initializes window data and registers window
	CliptextWndProc() - processes messages
	About() - processes messages for "About" dialog box
	_lstrlen(LPSTR) - string length using far pointers
	_lstrcpy(LPSTR, LPSTR) - string copy using far pointers

****************************************************************************/

#include "windows.h"
#include "cliptext.h"

HANDLE hInst;
HANDLE hAccTable;

char a_string[] = "Hello Windows!";		    /* string to copy	     */
HANDLE hData, hClipData;			    /* handles to clip data  */
LPSTR lpData, lpClipData;			    /* pointers to clip data */

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
	if (!CliptextInit(hInstance))
	    return (FALSE);

    hInst = hInstance;

    hWnd = CreateWindow("Cliptext",
	"Cliptext Sample Application",
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
	if (!TranslateAccelerator(hWnd, hAccTable, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg); 
	}
    }
    return (msg.wParam);
}

/****************************************************************************

    FUNCTION: CliptextInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL CliptextInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bRegisterError;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "ClipTextMenu";
    pWndClass->lpszClassName = (LPSTR) "Cliptext";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = CS_HREDRAW | CS_VREDRAW;
    pWndClass->lpfnWndProc = CliptextWndProc;

    bRegisterError = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bRegisterError);
}

/****************************************************************************

    FUNCTION: CliptextWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window
	WM_DESTROY    - destroy window
	WM_INITMENU   - initialize menu
	WM_COMMAND    - message from menu

    COMMENTS:

	WM_INITMENU - when this message is received, the application checks
	to see if there is any text data in the clipboard, and enables or
	disables the Paste menu item accordingly.

	Seclecting the Copy menu item will send the text "Hello Windows" to
	the clipboard.

	Seclecting the Paste menu item will copy whatever text is in the
	clipboard to the application window.

****************************************************************************/

long FAR PASCAL CliptextWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
DWORD lParam;
{
    FARPROC lpProcAbout;
    WORD wFormat;
    HDC hDC;
    HMENU hMenu;

    switch (message) {
	case WM_CREATE:
	    hAccTable = LoadAccelerators(hInst, "EditMenu");
	    break;

	case WM_INITMENU:
	    if (wParam == GetMenu(hWnd)) {
		wFormat = 0;
		if (OpenClipboard(hWnd)) {
		    while ((wFormat = EnumClipboardFormats(wFormat))
			!= 0 && wFormat != CF_TEXT);
		    CloseClipboard();
		    if (wFormat == CF_TEXT)
			EnableMenuItem(wParam, IDM_PASTE, MF_ENABLED);
		    else
			EnableMenuItem(wParam, IDM_PASTE, MF_GRAYED);
		    return (TRUE);
		}
		else			       /* Clipboard is not available */
		    return (FALSE);

	    }
	    return (TRUE);

	case WM_COMMAND:
	    switch(wParam) {

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
		case IDM_CLEAR:
		    MessageBox(hWnd, "Command not implemented",
			(LPSTR) NULL, MB_OK);
		    break;  

		case IDM_COPY:

		    /* Allocate memory and copy the string to it */

		    hData = GlobalAlloc(GMEM_MOVEABLE,
			(DWORD) _lstrlen(a_string));
		    if (!hData)
			break;
		    lpData = GlobalLock(hData);
		    _lstrcpy(lpData, (LPSTR) a_string);
		    GlobalUnlock(hData);

		    /* Clear the current contents of the clipboard, and set
		     * the data handle to the new string.
		     */

		    if (OpenClipboard(hWnd)) {
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hData);
			CloseClipboard();
		    }
		    hData = 0;
		    return (TRUE);

		case IDM_PASTE:
		    if (OpenClipboard(hWnd)) {

			/* get text from the clipboard */

			hClipData = GetClipboardData(CF_TEXT);
			CloseClipboard();
			if (!hClipData)
			    break;
			lpClipData = GlobalLock(hClipData);

			/* copy text to the application window */

			hDC = GetDC(hWnd);
			TextOut(hDC, 10, 10, lpClipData, _lstrlen(lpClipData));
			GlobalUnlock(hClipData);
			ReleaseDC(hWnd, hDC);
			return (TRUE);
		    }
		    else
			return (FALSE);
	    }
	    break;

	case WM_DESTROY:
	    PostQuitMessage(0);
	    return (NULL);

	default:
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
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
	    return (TRUE);
    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: _lstrlen(LPSTR)

    PURPOSE:  uses a long far pointer to the string, returns the length

****************************************************************************/

int _lstrlen(lpString)
LPSTR lpString;
{
    int i;
    for (i=0; *lpString++; ++i);
    return (i);
}

/****************************************************************************

    FUNCTION: _lstrcpy(LPSTR, LPSTR)

    PURPOSE:  FAR version of strcpy()

****************************************************************************/

void _lstrcpy(lpDestination, lpSource)
LPSTR lpDestination, lpSource;
{
    while (*lpDestination++ = *lpSource++);
}
