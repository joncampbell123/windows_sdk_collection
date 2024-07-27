/****************************************************************************

    PROGRAM: Generic.c

    PURPOSE: Generic template for Windows applications

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	GenericInit() - initializes window data and registers window
	GenericWndProc() - processes messages
	About() - processes messages for "About" dialog box

    COMMENTS:

	Windows can have several copies of your application running at the
	same time.  The variable hInst keeps track of which instance this
	application is so that processing will be to the correct window.

	You only need to initialize the application once.  After it is
	initialized, all other copies of the application will use the same
	window class, and do not need to be separately initialized.

****************************************************************************/

#include "windows.h"		    /* required for all Windows applications */
#include "generic.h"		    /* specific to this program		     */

HANDLE hInst;			    /* current instance			     */

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

	This will initialize the window class if it is the first time this
	application is run.  It then creates the window, and processes the
	message loop until a PostQuitMessage is received.  It exits the
	application by returning the value passed by the PostQuitMessage.

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;			     /* current instance	     */
HANDLE hPrevInstance;			     /* previous instance	     */
LPSTR lpCmdLine;			     /* command line		     */
int nCmdShow;				     /* show-window type (open/icon) */
{
    HWND hWnd;				     /* window handle		     */
    MSG msg;				     /* message			     */


    if (!hPrevInstance)			/* Has application been initialized? */
	if (!GenericInit(hInstance))
	    return (NULL);		/* Exits if unable to initialize     */

    hInst = hInstance;			/* Saves the current instance	     */

    hWnd = CreateWindow("Generic",		  /* window class	     */
	"Generic Sample Application",		  /* window name	     */
	WS_OVERLAPPEDWINDOW,			  /* window style	     */
	CW_USEDEFAULT,				  /* x position		     */
	CW_USEDEFAULT,				  /* y position		     */
	CW_USEDEFAULT,				  /* width		     */
	CW_USEDEFAULT,				  /* height		     */
	NULL,					  /* parent handle	     */
	NULL,					  /* menu or child ID	     */
	hInstance,				  /* instance		     */
	NULL);					  /* additional info	     */

    if (!hWnd)					  /* Was the window created? */
	return (NULL);

    ShowWindow(hWnd, nCmdShow);			  /* Shows the window	     */
    UpdateWindow(hWnd);				  /* Sends WM_PAINT message  */

    while (GetMessage(&msg,	   /* message structure			     */
	    NULL,		   /* handle of window receiving the message */
	    NULL,		   /* lowest message to examine		     */
	    NULL))		   /* highest message to examine	     */
	{
	TranslateMessage(&msg);	   /* Translates virtual key codes	     */
	DispatchMessage(&msg);	   /* Dispatches message to window	     */
    }
    return (msg.wParam);	   /* Returns the value from PostQuitMessage */
}


/****************************************************************************

    FUNCTION: GenericInit(HANDLE)

    PURPOSE: Initializes window data and registers window class

    COMMENTS:

	Sets up a structure to register the window class.  Structure includes
	such information as what function will process messages, what cursor
	and icon to use, etc.

	This provides an example of how to allocate local memory using the
	LocalAlloc() call instead of malloc().	This provides a handle to
	memory.	 When you actually need the memory, LocalLock() is called
	which returns a pointer.  As soon as you are done processing the
	memory, call LocalUnlock so that Windows can move the memory as
	needed.	 Call LocalLock() to get a pointer again, or LocalFree() if
	you don't need the memory again.


****************************************************************************/

BOOL GenericInit(hInstance)
HANDLE hInstance;			       /* current instance	     */
{
    HANDLE hMemory;			       /* handle to allocated memory */
    PWNDCLASS pWndClass;		       /* structure pointer	     */
    BOOL bSuccess;			       /* RegisterClass() result     */

    hMemory = LocalAlloc(LPTR, sizeof(WNDCLASS));
    pWndClass = (PWNDCLASS) LocalLock(hMemory);

    pWndClass->style = NULL;
    pWndClass->lpfnWndProc = GenericWndProc;
    pWndClass->hInstance = hInstance;
    pWndClass->hIcon = LoadIcon(NULL, IDI_APPLICATION);
    pWndClass->hCursor = LoadCursor(NULL, IDC_ARROW);
    pWndClass->hbrBackground = GetStockObject(WHITE_BRUSH);
    pWndClass->lpszMenuName = (LPSTR) NULL;
    pWndClass->lpszClassName = (LPSTR) "Generic";

    bSuccess = RegisterClass(pWndClass);

    LocalUnlock(hMemory);			    /* Unlocks the memory    */
    LocalFree(hMemory);				    /* Returns it to Windows */

    return (bSuccess);		 /* Returns result of registering the window */
}

/****************************************************************************

    FUNCTION: GenericWndProc(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_SYSCOMMAND - system menu (About dialog box)
	WM_CREATE     - create window
	WM_DESTROY    - destroy window

    COMMENTS:

	To process the ID_ABOUT message, call MakeProcInstance() to get the
	current instance address of the About() function.  Then call Dialog
	box which will create the box according to the information in your
	generic.rc file and turn control over to the About() function.	When
	it returns, free the intance address.

	Since you don't have a menu, use the WM_CREATE to change the system
	menu to include a separator and the "About Generic..." menu item.

****************************************************************************/

long FAR PASCAL GenericWndProc(hWnd, message, wParam, lParam)
HWND hWnd;				  /* window handle		     */
unsigned message;			  /* type of message		     */
WORD wParam;				  /* additional information	     */
LONG lParam;				  /* additional information	     */
{
    FARPROC lpProcAbout;		  /* pointer to the "About" function */
    HMENU hMenu;			  /* handle to the System menu	     */

    switch (message) {
	case WM_SYSCOMMAND:		/* message: command from system menu */
	    if (wParam == ID_ABOUT) {
		lpProcAbout = MakeProcInstance(About, hInst);

		DialogBox(hInst,		 /* current instance	     */
		    "AboutBox",			 /* resource to use	     */
		    hWnd,			 /* parent handle	     */
		    lpProcAbout);		 /* About() instance address */

		FreeProcInstance(lpProcAbout);
		break;
	    }

	    else			    /* Lets Windows process it	     */
		return (DefWindowProc(hWnd, message, wParam, lParam));

	case WM_CREATE:			    /* message: window being created */

	    /* Get the handle of the System menu */

	    hMenu = GetSystemMenu(hWnd, FALSE);

	    /* Add a separator to the menu */

	    ChangeMenu(hMenu,			      /* menu handle	     */
		NULL,				      /* menu item to change */
		NULL,				      /* new menu item	     */
		NULL,				      /* menu identifier     */
		MF_APPEND | MF_SEPARATOR);	      /* type of change	     */

	    /* Add new menu item to the System menu */

	    ChangeMenu(hMenu,			      /* menu handle	     */
		NULL,				      /* menu item to change */
		"A&bout Generic...",		      /* new menu item	     */
		ID_ABOUT,			      /* menu identifier     */
		MF_APPEND | MF_STRING);		      /* type of change	     */
	    break;

	case WM_DESTROY:		  /* message: window being destroyed */
	    PostQuitMessage(0);
	    break;

	default:			  /* Passes it on if unproccessed    */
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

    COMMENTS:

	No initialization is needed for this particular dialog box, but TRUE
	must be returned to Windows.

	Wait for user to click on "Ok" button, then close the dialog box.

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned message;
WORD wParam;
LONG lParam;
{
    switch (message) {
	case WM_INITDIALOG:		   /* message: initialize dialog box */
	    return (TRUE);

	case WM_COMMAND:		      /* message: received a command */
	    if (wParam == IDOK) {	      /* "OK" box selected?	     */
		EndDialog(hDlg, NULL);	      /* Exits the dialog box	     */
		return (TRUE);
	    }
	    break;
    }
    return (FALSE);			      /* Didn't process a message    */
}
