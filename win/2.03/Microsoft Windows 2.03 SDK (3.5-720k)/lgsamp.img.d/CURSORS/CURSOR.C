/****************************************************************************

    PROGRAM: Cursor.c

    PURPOSE: Demonstrates how to manipulate a cursor and select a region

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	CursorInit() - initializes window data and registers window
	CursorWndProc() - processes messages
	About() - processes messages for "About" dialog box
	sieve() - time consuming function, generates primes

****************************************************************************/

#include "windows.h"
#include "cursor.h"

HANDLE hInst;

char str[255];				    /* general-purpose string buffer */

HCURSOR hSaveCursor;			    /* handle to current cursor	     */
HCURSOR hHourGlass;			    /* handle to hourglass cursor    */

BOOL bTrack = FALSE;			    /* TRUE if left button clicked   */
int OrgX = 0, OrgY = 0;			    /* original cursor position	     */
int PrevX = 0, PrevY = 0;		    /* current cursor position	     */
int X = 0, Y = 0;			    /* last cursor position	     */
RECT Rect;				    /* selection rectangle	     */

POINT ptCursor;				    /* x and y coordinates of cursor */
int repeat = 1;				    /* repeat count of keystroke     */

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
	if (!CursorInit(hInstance))
	    return (NULL);

    hInst = hInstance;

    hWnd = CreateWindow("Cursor",
	"Cursor Sample Application",
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

    FUNCTION: CursorInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL CursorInit(hInstance)
HANDLE hInstance;
{
    HANDLE hMemory;
    PWNDCLASS pWndClass;
    BOOL bSuccess;

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);
    pWndClass->hCursor = LoadCursor(hInstance, "bullseye");
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->lpszMenuName = (LPSTR) NULL;
    pWndClass->lpszClassName = (LPSTR) "Cursor";
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->hInstance = hInstance;
    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = CursorWndProc;

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);
    LocalFree(hMemory);
    return (bSuccess);
}

/****************************************************************************

    FUNCTION: CursorWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window
	WM_DESTROY    - destroy window
	WM_CHAR	      - ASCII key value received
	WM_LBUTTONDOWN - left mouse button
	WM_MOUSEMOVE   - mouse movement
	WM_LBUTTONUP   - left button released
	WM_KEYDOWN     - key pressed
	WM_KEYUPS      - key released

    COMMENTS:

	When the left mouse button is pressed, btrack is set to TRUE so that
	the code for WM_MOUSEMOVE will keep track of the mouse and update
	the box accordingly.  Once the button is released, btrack is set to
	FALSE, and the current position is saved.  Holding the SHIFT key
	while pressing the left button will extend the current box rather
	then erasing it and starting a new one.

	When an arrow key is pressed, the cursor is repositioned in the
	direction of the arrow key.  A repeat count is kept so that the
	longer the user holds down the arrow key, the faster it will move.
	As soon as the key is released, the repeat count is set to 1 for
	normal cursor movement.

****************************************************************************/

long FAR PASCAL CursorWndProc(hWnd, message, wParam, lParam)
HWND hWnd;
unsigned message;
WORD wParam;
LONG lParam;
{
    FARPROC lpProcAbout;
    HMENU hMenu;

    HDC hDC;					/* handle to display context */

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

	    /* Get handle to the hourglass cursor */

	    hHourGlass = LoadCursor(hInst, IDC_WAIT);

	    hMenu = GetSystemMenu(hWnd, FALSE);
	    ChangeMenu(hMenu, NULL, NULL, NULL, MF_APPEND | MF_SEPARATOR);
	    ChangeMenu(hMenu, NULL, "A&bout Cursor...", ID_ABOUT,
		MF_APPEND | MF_STRING);
	    break;

	case WM_CHAR:
	    if (wParam == '\r') {
		SetCapture(hWnd);

		/* Set the cursor to an hourglass */

		hSaveCursor = SetCursor(hHourGlass);

		hDC = GetDC(hWnd);
		TextOut(hDC, 1, 1, "Calculating prime numbers...", 28);
		sprintf(str, "Calculated %d primes.	  ", sieve());
		TextOut(hDC, 1, 1, str, strlen(str));
		ReleaseDC(hWnd, hDC);

		SetCursor(hSaveCursor);		 /* Restores previous cursor */
		ReleaseCapture();
	    }
	    break;

	case WM_LBUTTONDOWN:
	    bTrack = TRUE;

	    if (OrgX != X || OrgY != Y) {	 /* Clears previous box	     */
		hDC = GetDC(hWnd);
		SetROP2(hDC, R2_NOT);
		MoveTo(hDC, OrgX, OrgY);
		LineTo(hDC, OrgX, Y);
		LineTo(hDC, X, Y);
		LineTo(hDC, X, OrgY);
		LineTo(hDC, OrgX, OrgY);
		ReleaseDC(hWnd, hDC);
	    }
	    PrevX = LOWORD(lParam);
	    PrevY = HIWORD(lParam);
	    if (!(wParam & MK_SHIFT)) {	      /* If shift key is not pressed */
		OrgX = LOWORD(lParam);
		OrgY = HIWORD(lParam);
	    }
	    else {	     /* shift key is pressed, update the current box */
		hDC = GetDC(hWnd);
		SetROP2(hDC, R2_NOT);
		MoveTo(hDC, OrgX, OrgY);
		LineTo(hDC, OrgX, PrevY);
		LineTo(hDC, PrevX, PrevY);
		LineTo(hDC, PrevX, OrgY);
		LineTo(hDC, OrgX, OrgY);
		ReleaseDC(hWnd, hDC);
	    }

	    /* Capture all input even if the mouse goes outside of window */

	    SetCapture(hWnd);
	    break;

	case WM_MOUSEMOVE:
	    if (bTrack) {
		hDC = GetDC(hWnd);
		SetROP2(hDC, R2_NOT);		  /* Erases the previous box */
		MoveTo(hDC, OrgX, OrgY);
		LineTo(hDC, OrgX, PrevY);
		LineTo(hDC, PrevX, PrevY);
		LineTo(hDC, PrevX, OrgY);
		LineTo(hDC, OrgX, OrgY);

		/* Get the current mouse position */

		PrevX = LOWORD(lParam);
		PrevY = HIWORD(lParam);
		MoveTo(hDC, OrgX, OrgY);		/* Draws the new box */
		LineTo(hDC, OrgX, PrevY);
		LineTo(hDC, PrevX, PrevY);
		LineTo(hDC, PrevX, OrgY);
		LineTo(hDC, OrgX, OrgY);
		ReleaseDC(hWnd, hDC);
	    }
	    break;

	case WM_LBUTTONUP:
	    bTrack = FALSE;		     /* Ignores mouse input	     */
	    ReleaseCapture();		     /* Releases hold on mouse input */

	    X = LOWORD(lParam);		     /* Saves the current value	     */
	    Y = HIWORD(lParam);
	    break;

	case WM_KEYDOWN:
	    if (wParam != VK_LEFT && wParam != VK_RIGHT
		    && wParam != VK_UP && wParam != VK_DOWN)
		break;

	    GetCursorPos(&ptCursor);

	    /* Convert screen coordinates to client coordinates */

	    ScreenToClient(hWnd, &ptCursor);
	    repeat++;				/* Increases the repeat rate */

	    switch (wParam) {

	    /* Adjust cursor position according to which key was pressed. */

		case VK_LEFT:
		    ptCursor.x -= repeat;
		    break;

		case VK_RIGHT:
		    ptCursor.x += repeat;
		    break;

		case VK_UP:
		    ptCursor.y -= repeat;
		    break;

		case VK_DOWN:
		    ptCursor.y += repeat;
		    break;

	    }

	    /* Get the client boundaries */

	    GetClientRect(hWnd, &Rect);

	    /* Adjust cursor to keep it within the boundaries */

	    if (ptCursor.x >= Rect.right)
		ptCursor.x = Rect.right - 1;
	    else if (ptCursor.x < Rect.left)
		ptCursor.x = Rect.left;
	    if (ptCursor.y >= Rect.bottom)
		ptCursor.y = Rect.bottom - 1;
	    else if (ptCursor.y < Rect.top)
		ptCursor.y = Rect.top;

	    /* Convert the coordinates to screen coordinates */

	    ClientToScreen(hWnd, &ptCursor);
	    SetCursorPos(ptCursor.x, ptCursor.y);
	    break;

	case WM_KEYUP:
	    repeat = 1;				 /* Clears the repeat count. */
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

/****************************************************************************

    FUNCTION: Sieve()

    PURPOSE:  Example of time consuming process

    COMMENTS:

	Sieve of Eratosthenes, BYTE, Volume 8, Number 1, by Jim Gilbreath
	and Gary Gilbreath.  Code changed to give correct results.

	One could return the count, and after restoring the cursor, use
	sprintf() to copy the information to a string which could then be
	put up in a MessageBox().

****************************************************************************/

#define NITER	20				   /* number of iterations */
#define SIZE	8190

char flags[SIZE+1]={ 0};

sieve() {
    int i,k;
    int iter, count;

    for (iter = 1; iter <= NITER; iter++) {	 /* Does sieve NITER times */
	count = 0;
	for (i = 0; i <= SIZE; i++)		 /* Sets all flags TRUE	   */
	    flags[i] = TRUE;

	for (i = 2; i <= SIZE; i++) {
	    if (flags[i] ) {			    /* Found a prime?	    */
		for (k = i + i; k <= SIZE; k += i)
		    flags[k] = FALSE;		   /* Cancelsits multiples */
		count++;
	    }
	}
    }
    return (count);
}
