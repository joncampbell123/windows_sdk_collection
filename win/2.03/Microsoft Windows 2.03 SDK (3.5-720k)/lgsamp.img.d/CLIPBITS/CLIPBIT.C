/****************************************************************************

    PROGRAM: Clipbit.c

    PURPOSE: Demonstrates copying bitmaps to and from the clipboard

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	ClipbitInit() - initializes window data and registers window
	ClipbitWndProc() - processes messages
	About() - processes messages for "About" dialog box
	_lstrlen(LPSTR) - string length using far pointers
	_lstrcpy(LPSTR, LPSTR) - string copy using far pointers

****************************************************************************/

#include "windows.h"
#include "clipbit.h"
#include "select.h"			   /* library routines for selection */

HANDLE hInst;
HANDLE hAccTable;
char a_string[] = "Hello Windows!";
HANDLE hData, hClipData;
LPSTR lpData, lpClipData;

HDC hMemoryDC;			    /* handle to memory DC		     */
HBITMAP hBitmap, hOldBitmap;	    /* handles to bitmaps		     */

BOOL bTrack = FALSE;		    /* TRUE if the user is selecting an area */
RECT Rect;			    /* selection rectangle		     */

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
	if (!ClipbitInit(hInstance))
	    return (FALSE);

    hInst = hInstance;

    hWnd = CreateWindow("Clipbit",
	"Clipbit Sample Application",
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

    FUNCTION: ClipbitInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL ClipbitInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bRegisterError;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) "ClipBitMenu";
    pWndClass->lpszClassName = (LPSTR) "Clipbit";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = CS_HREDRAW | CS_VREDRAW;
    pWndClass->lpfnWndProc = ClipbitWndProc;

    bRegisterError = RegisterClass((LPWNDCLASS) pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bRegisterError);
}

/****************************************************************************

    FUNCTION: ClipbitWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window
	WM_DESTROY    - destroy window
	WM_INITMENU   - initialize menu
	WM_COMMAND    - application menu

    COMMENTS:

	The area to be copied to the clipboard is selected by using routines
	provided in the select.exe library.  Once an area has been selected,
	the Copy menu item will be enabled, and the user can click on it to
	send their selection to the clipboard.

****************************************************************************/

long FAR PASCAL ClipbitWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
DWORD lParam;
{
    FARPROC lpProcAbout;
    HMENU hMenu;

    WORD wFormat;
    HDC hDC;
    HBITMAP hOldBitmap;
    BITMAP PasteBitmap;

    switch (message) {
	case WM_CREATE:
	    hAccTable = LoadAccelerators(hInst, "EditMenu");
	    break;

	case WM_INITMENU:
	    if (wParam == GetMenu(hWnd)) {
		wFormat = 0;
		if (OpenClipboard(hWnd)) {
		    while ( (wFormat = EnumClipboardFormats(wFormat))
			!= 0 && wFormat != CF_BITMAP);
		    CloseClipboard();

		    /* Enable the PASTE menu if there is a bitmap in the
		     * clipboard.
		     */

		    if (wFormat == CF_BITMAP)
			EnableMenuItem(wParam, IDM_PASTE, MF_ENABLED);
		    else
			EnableMenuItem(wParam, IDM_PASTE, MF_GRAYED);
		    return (TRUE);
		}
		else
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
		    ClearSelection(hWnd, &Rect, SL_BOX);
		    hDC = GetDC(hWnd);
		    hMemoryDC = CreateCompatibleDC(hDC);
		    if (hMemoryDC) {
			hBitmap = CreateCompatibleBitmap (hMemoryDC,
			    Rect.right-Rect.left, Rect.bottom-Rect.top);
			if (hBitmap) {
			    hOldBitmap = SelectObject(hMemoryDC, hBitmap);
			    BitBlt(hMemoryDC, 0, 0,
				Rect.right-Rect.left, Rect.bottom-Rect.top,
				hDC, Rect.left, Rect.top, SRCCOPY);
			    if (OpenClipboard(hWnd)) {
				EmptyClipboard();
				SetClipboardData(CF_BITMAP, hBitmap);
				CloseClipboard();
			    }
			    SelectObject(hMemoryDC, hOldBitmap);
			}
			DeleteDC(hMemoryDC);
		    }
		    ReleaseDC(hWnd, hDC);
		    hMenu = GetMenu(hWnd);
		    EnableMenuItem(hMenu, IDM_COPY, MF_GRAYED);
		    SetRectEmpty(&Rect);	      /* clear the rectangle */
		    break;

		case IDM_PASTE:

		    /* Get a handle to the bitmap from the Clipboard, create
		     * a memory DC, copy the bitmap to PasteBitmap, select the
		     * memory DC, and call BitBlt() to copy the bitmap to the
		     * window.
		     */

		    if (OpenClipboard(hWnd)) {
			hClipData = GetClipboardData(CF_BITMAP);
			CloseClipboard();
			if (!hClipData)
			    break;
			hDC = GetDC(hWnd);
			hMemoryDC = CreateCompatibleDC(hDC);
			if (hMemoryDC) {
			    GetObject(hClipData, sizeof(BITMAP),
				(LPSTR) &PasteBitmap);
			    hOldBitmap = SelectObject(hMemoryDC, hClipData);
			    if (hOldBitmap) {
				BitBlt(hDC,
				    10,			/* x position	     */
				    10,			/* y position	     */
				    PasteBitmap.bmWidth,
				    PasteBitmap.bmHeight,
				    hMemoryDC,
				    0,			/* x source position */
				    0,			/* y source position */
				    SRCCOPY);
				SelectObject(hMemoryDC, hOldBitmap);
			    }
			    DeleteDC(hMemoryDC);
		       }
		       ReleaseDC(hWnd, hDC);
		       return (TRUE);
		    }
		    else
			return (FALSE);

	    }
	    break;

	case WM_LBUTTONDOWN:		  /* message: left mouse button down */
	    bTrack = TRUE;
	    StartSelection(hWnd, MAKEPOINT(lParam), &Rect,
		(wParam & MK_SHIFT) ? (SL_EXTEND | SL_BOX) : SL_BOX);
	    break;

	case WM_MOUSEMOVE:			  /* message: mouse movement */
	    if (bTrack)
		UpdateSelection(hWnd, MAKEPOINT(lParam), &Rect, SL_BOX);
	    break;

	case WM_LBUTTONUP:		    /* message: left mouse button up */
	    bTrack = FALSE;
	    EndSelection(MAKEPOINT(lParam), &Rect);

	    /* Enable the copy menu if user has selected a region. */

	    if (Rect.left != Rect.right || Rect.top != Rect.bottom) {
		hMenu = GetMenu(hWnd);
		EnableMenuItem(hMenu, IDM_COPY, MF_ENABLED);
	    }
	    break;

	case WM_DESTROY:
	    PostQuitMessage(0);
	    return (NULL);

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
	    return (TRUE);
    }
    return (FALSE);
}

/****************************************************************************

    FUNCTION: _lstrlen(LPSTR)

    PURPOSE:  FAR version of strlen()

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
