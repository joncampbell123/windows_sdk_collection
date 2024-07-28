/****************************************************************************

    PROGRAM: wmfdcode.c

    PURPOSE: view and decode Windows Metafiles

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	WaitCursor() - loads hourglass cursor/restores original cursor

    HISTORY: 1/16/91 - wrote it - drc

****************************************************************************/

#define MAIN

#include "windows.h"                /* required for all Windows applications */
#include "wmfdcode.h"		    /* specific to this program 	     */


/***********************************************************************

  FUNCTION   : WinMain

  PARAMETERS : HANDLE
	       HANDLE
	       LPSTR
	       int

  PURPOSE    : calls initialization function, processes message loop

  CALLS      : WINDOWS
		 GetMessage
		 TranslateMessage
		 DispatchMessage

	       APP
		 InitApplication

  RETURNS    : int

  COMMENTS   : Windows recognizes this function by name as the initial entry
	       point for the program.  This function calls the application
	       initialization routine, if no other instance of the program is
	       running, and always calls the instance initialization routine.
	       It then executes a message retrieval and dispatch loop that is
	       the top-level control structure for the remainder of execution.
	       The loop is terminated when a WM_QUIT message is received, at
	       which time this function exits the application instance by
	       returning the value passed by PostQuitMessage().

	       If this function must abort before entering the message loop,
	       it returns the conventional value NULL.

  HISTORY    : 1/16/91 - created - modified from SDK sample GENERIC

************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;                            /* current instance             */
HANDLE hPrevInstance;                        /* previous instance            */
LPSTR lpCmdLine;                             /* command line                 */
int nCmdShow;                                /* show-window type (open/icon) */
{
    MSG msg;                                 /* message                      */

    if (!hPrevInstance)                  /* Other instances of app running? */
	if (!InitApplication(hInstance)) /* Initialize shared things */
	    return (FALSE);              /* Exits if unable to initialize     */

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
	return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    while (GetMessage(&msg,        /* message structure                      */
	    NULL,                  /* handle of window receiving the message */
	    NULL,                  /* lowest message to examine              */
	    NULL))                 /* highest message to examine             */
	{
	TranslateMessage(&msg);    /* Translates virtual key codes           */
	DispatchMessage(&msg);     /* Dispatches message to window           */
    }
    return (msg.wParam);           /* Returns the value from PostQuitMessage */
}


/***********************************************************************

  FUNCTION   : InitApplication

  PARAMETERS : HANDLE hInstance

  PURPOSE    : Initializes window data and registers window class

  CALLS      : WINDOWS
		 RegisterClass

  MESSAGES   : none

  RETURNS    : BOOL

  COMMENTS   : This function is called at initialization time only if no
	       other instances of the application are running.  This function
	       performs initialization tasks that can be done once for any
	       number of running instances.

	       In this case, we initialize a window class by filling out a
	       data structure of type WNDCLASS and calling the Windows
	       RegisterClass() function.  Since all instances of this
	       application use the same window class, we only need to do this
	       when the first instance is initialized.

  HISTORY    : 1/16/91 - created - modified from SDK sample app GENERIC

************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;                       /* current instance */
{
    WNDCLASS  wc;

    bInPaint = FALSE;

    /* Fill in window class structure with parameters that describe the
       main window. */

    wc.style = NULL;                    /* Class style(s) */
    wc.lpfnWndProc = MainWndProc;       /* Function to retrieve messages for */
					/* windows of this class */
    wc.cbClsExtra = 0;                  /* No per-class extra data */
    wc.cbWndExtra = 0;                  /* No per-window extra data */
    wc.hInstance = hInstance;           /* Application that owns the class */
    wc.hIcon = LoadIcon(hInstance, "WMFICON");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = COLOR_WINDOW + 1;
    wc.lpszMenuName =  "MetaMenu";      /* Name of menu resource in .RC file */
    wc.lpszClassName = "MetaWndClass";  /* Name used in call to CreateWindow */

    /* Register the window class and return success/failure code */

    return (RegisterClass(&wc));

}

/***********************************************************************

  FUNCTION   : InitInstance

  PARAMETERS : HANDLE  hInstance - Current instance identifier
	       int     nCmdShow  - Param for first ShowWindow() call

  PURPOSE    : Saves instance handle and creates main window

  CALLS      : WINDOWS
		 CreateWindow
		 ShowWindow
		 UpdateWindow

  MESSAGES   : none

  RETURNS    : BOOL

  COMMENTS   : This function is called at initialization time for every
	       instance of this application.  This function performs
	       initialization tasks that cannot be shared by multiple
	       instances.

	       In this case, we save the instance handle in a static variable
	       and create and display the main program window.

  HISTORY    :

************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
HANDLE     hInstance;          /* Current instance identifier.       */
int        nCmdShow;           /* Param for first ShowWindow() call. */
{
    HWND   hWnd;               /* Main window handle.                */

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */

    hInst = hInstance;

    /* Create a main window for this application instance.  */

    hWnd = CreateWindow(
	"MetaWndClass",                 /* See RegisterClass() call.          */
	APPNAME,                        /* Text for window title bar.         */
	WS_OVERLAPPEDWINDOW,            /* Window style.                      */
	CW_USEDEFAULT,                  /* Default horizontal position.       */
	CW_USEDEFAULT,                  /* Default vertical position.         */
	CW_USEDEFAULT,                  /* Default width.                     */
	CW_USEDEFAULT,                  /* Default height.                    */
	NULL,                           /* Overlapped windows have no parent. */
	NULL,                           /* Use the window class menu.         */
	hInstance,                      /* This instance owns this window.    */
	NULL                            /* Pointer not needed.                */
    );

    /* If window could not be created, return "failure" */

    if (!hWnd)
	return (FALSE);

    hWndMain = hWnd;

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/***********************************************************************

  FUNCTION   : MainWndProc

  PARAMETERS : HWND hWnd        -  window handle
	       unsigned message -  type of message
	       WORD wParam      -  additional information
	       LONG lParam      -  additional information

  PURPOSE    : Processes messages

  CALLS      :

  MESSAGES   : WM_CREATE

	       WM_COMMAND

		 wParams
		 - IDM_EXIT
		 - IDM_ABOUT
		 - IDM_OPEN
		 - IDM_PRINT
		 - IDM_LIST
		 - IDM_CLEAR
		 - IDM_ENUM
		 - IDM_ENUMRANGE
		 - IDM_ALLREC
		 - IDM_DESTDISPLAY
		 - IDM_DESTMETA
		 - IDM_HEADER
		 - IDM_CLIPHDR
		 - IDM_ALDUSHDR

	       WM_DESTROY

  RETURNS    : long

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;                                /* window handle                   */
UINT message;			      /* type of message		 */
WPARAM wParam;				    /* additional information	       */
LPARAM lParam;				    /* additional information	       */
{
    RECT        rect;
    int         iFOpenRet;
    char        TempOpenName[128];
    int         iDlgRet;

    switch (message) {
	case WM_CREATE:

	    /* init the state of the menu items */
	    EnableMenuItem(GetMenu(hWnd), IDM_VIEW, MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
	    EnableMenuItem(GetMenu(hWnd), IDM_PLAY, MF_DISABLED|MF_GRAYED|MF_BYPOSITION);
	    EnableMenuItem(GetMenu(hWnd), IDM_PRINT, MF_DISABLED|MF_GRAYED);
	    CheckMenuItem(GetMenu(hWnd), IDM_DESTDISPLAY, MF_CHECKED);
	    break;

	case WM_COMMAND:
	    /* message: command from application menu */
	    switch (wParam) {

		case IDM_EXIT: /* file exit menu option */

		   PostQuitMessage(0);
		   break;

		case IDM_ABOUT: /* about box */

		   lpProcAbout = MakeProcInstance(About, hInst);
		   DialogBox(hInst,                      /* current instance         */
			    "AboutBox",                  /* resource to use          */
			     hWnd,                       /* parent handle            */
			     lpProcAbout);               /* About() instance address */
		   FreeProcInstance(lpProcAbout);
		   break;

		case IDM_OPEN: /* select a metafile to open */

		    /* save the name of previously opened file */
		    if (lstrlen((LPSTR)OpenName) != 0)
			lstrcpy((LPSTR)TempOpenName, (LPSTR)OpenName);

		    /* initialize file info flags */
		    if (!bMetaFileOpen) {
		      bBadFile = FALSE;
		      bValidFile = FALSE;
		    }

		    /* clear the client area */
		    GetClientRect(hWnd, (LPRECT)&rect);
		    InvalidateRect(hWnd, (LPRECT)&rect, TRUE);

		    /* call file open dlg */
		      iFOpenRet = OpenFileDialog((LPSTR)OpenName);

		    /* if a file was selected */
		    if (iFOpenRet)  {

		      /* if file contains a valid metafile and it was rendered */
		      if (!ProcessFile(hWnd, (LPSTR)OpenName))
			lstrcpy((LPSTR)OpenName, (LPSTR)TempOpenName);
		    }
		    else
		      lstrcpy((LPSTR)OpenName, (LPSTR)TempOpenName);

		    break;

		case IDM_PRINT: /* play the metafile to a printer DC */

		    /* if the metafile hasn't already been rendered as a placeable
		       or clipboard metafile */
		    if (!bMetaInRam)
		      hMF = GetMetaFile( (LPSTR)OpenName);

		    /* print it */
		    PrintWMF();
		    break;

		case IDM_LIST: /* list box containing all records of metafile */

		    WaitCursor(TRUE);
		    lpListDlgProc = MakeProcInstance(ListDlgProc, hInst);
		    DialogBox(hInst,             /* current instance         */
			     "LISTRECS",                         /* resource to use          */
			      hWnd,                      /* parent handle            */
			      lpListDlgProc);            /* About() instance address */
		    FreeProcInstance(lpListDlgProc);
		    WaitCursor(FALSE);
		    break;

		case IDM_CLEAR: /* clear the client area */

		    GetClientRect(hWnd, (LPRECT)&rect);
		    InvalidateRect(hWnd, (LPRECT)&rect, TRUE);
		    break;

		case IDM_ENUM: /* play - step - all menu option */

		    /* set flags appropriately before playing to destination */
		    bEnumRange = FALSE;
		    bPlayItAll = FALSE;
		    PlayMetaFileToDest(hWnd, iDestDC);
		    break;

		case IDM_ENUMRANGE: /* play - step - range menu option */

		    /* odd logic here...this just forces evaluation of the
		       enumeration range in MetaEnumProc. We are not "playing
		       it all" */

		    bPlayItAll = TRUE;

		    lpEnumRangeDlg = MakeProcInstance(EnumRangeDlgProc, hInst);
		    iDlgRet = DialogBox(hInst,
				       "ENUMRANGE",
				       hWnd,
				       lpEnumRangeDlg);
		    FreeProcInstance(lpEnumRangeDlg);

		    /* if cancel button not pressed, play to destination */
		    if (iDlgRet != IDCANCEL)
		      PlayMetaFileToDest(hWnd, iDestDC);
		    break;


		case IDM_ALLREC: /* play - all menu option */

		    /* set flag appropriately and play to destination */
		    bEnumRange = FALSE;
		    bPlayItAll = TRUE;
		    bPlayRec = TRUE;
		    PlayMetaFileToDest(hWnd, iDestDC);
		    break;

		case IDM_DESTDISPLAY: /* play - destination - display menu option */

		    CheckMenuItem(GetMenu(hWnd), IDM_DESTDISPLAY, MF_CHECKED);
		    CheckMenuItem(GetMenu(hWnd), IDM_DESTMETA, MF_UNCHECKED);

		    /* set destination flag to display */
		    iDestDC = DESTDISPLAY;
		    break;

		case IDM_DESTMETA: /* play - destination - metafile menu option */

		    CheckMenuItem(GetMenu(hWnd), IDM_DESTDISPLAY, MF_UNCHECKED);
		    CheckMenuItem(GetMenu(hWnd), IDM_DESTMETA, MF_CHECKED);

		    /* set destination flag to metafile */
		    iDestDC = DESTMETA;
		    break;


		case IDM_HEADER: /* display the common metafile header */

		   if (bValidFile) {
		     lpHeaderDlg = MakeProcInstance(HeaderDlgProc, hInst);
		     DialogBox(hInst,
			      "HEADER",
			       hWnd,
			       lpHeaderDlg);
		     FreeProcInstance(lpHeaderDlg);
		   }
		   break;

		case IDM_CLIPHDR: /* display the metafilepict of a clipboard file */

		   if (bValidFile) {
		     lpClpHeaderDlg = MakeProcInstance(ClpHeaderDlgProc, hInst);
		     DialogBox(hInst,            /* current instance         */
			      "CLIPHDR",                         /* resource to use          */
			       hWnd,                     /* parent handle            */
			       lpClpHeaderDlg);          /* About() instance address */
		     FreeProcInstance(lpClpHeaderDlg);
		   }
		   break;

		case IDM_ALDUSHDR: /* display the placeable metafile header */

		   if (bValidFile) {
		     lpAldusHeaderDlg = MakeProcInstance(AldusHeaderDlgProc, hInst);
		     DialogBox(hInst,            /* current instance         */
			      "ALDUSHDR",                        /* resource to use          */
			       hWnd,                     /* parent handle            */
			       lpAldusHeaderDlg);                /* About() instance address */
		     FreeProcInstance(lpAldusHeaderDlg);
		   }
		   break;

		default:  /* let Windows process it */
		    return (DefWindowProc(hWnd, message, wParam, lParam));
	    }
	    break;

	case WM_DESTROY: /* message: window being destroyed */
	    PostQuitMessage(0);
	    break;


	default:  /* passes it on if unproccessed */
	    return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}

/***********************************************************************

  FUNCTION   : WaitCursor

  PARAMETERS : BOOL bWait - TRUE for the hour glass cursor
			    FALSE to return to the previous cursor

  PURPOSE    : toggle the mouse cursor to the hourglass and back

  CALLS      : WINDOWS
		 LoadCursor
		 SetCursor

  MESSAGES   : none

  RETURNS    : void

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc

************************************************************************/

void WaitCursor(bWait)
BOOL bWait;
{
  HCURSOR hCursor;
  static HCURSOR hOldCursor;

  /* if hourglass cursor is to be used */
  if (bWait) {

    hCursor = LoadCursor(NULL, IDC_WAIT);
    hOldCursor = SetCursor(hCursor);
  }
  else {
    SetCursor(hOldCursor);
  }
}
