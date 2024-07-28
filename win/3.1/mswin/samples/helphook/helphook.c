/****************************************************************************

    PROGRAM: HelpHook.c

    PURPOSE: HelpHook template for Windows applications

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance handle and creates main window
	MainWndProc() - processes messages
	About() - processes messages for "About" dialog box

    COMMENTS:

        Windows can have several copies of your application running at the
        same time.  The variable hInst keeps track of which instance this
        application is so that processing will be to the correct window.

****************************************************************************/

#include "windows.h"		    /* required for all Windows applications */
#include "HelpHook.h"		    /* specific to this program		     */

HANDLE hInst;			    /* current instance			     */
WORD   PWM_PrivateHelpMessage;
FARPROC fnNextMessageFilterProc;
FARPROC fpMessageFilter;
WORD   wCurrentMenuCmd;
DWORD  dwCurrentMenuBits;
HANDLE hWndGlobal;


/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

    COMMENTS:

        Windows recognizes this function by name as the initial entry point 
        for the program.  This function calls the application initialization 
        routine, if no other instance of the program is running, and always 
        calls the instance initialization routine.  It then executes a message 
        retrieval and dispatch loop that is the top-level control structure 
        for the remainder of execution.  The loop is terminated when a WM_QUIT 
        message is received, at which time this function exits the application 
        instance by returning the value passed by PostQuitMessage(). 

        If this function must abort before entering the message loop, it 
        returns the conventional value NULL.  

****************************************************************************/

int PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;			     /* current instance	     */
HANDLE hPrevInstance;			     /* previous instance	     */
LPSTR lpCmdLine;			     /* command line		     */
int nCmdShow;				     /* show-window type (open/icon) */
{
    MSG msg;				     /* message			     */

    if (!hPrevInstance)			 /* Other instances of app running? */
	if (!InitApplication(hInstance)) /* Initialize shared things */
	    return (FALSE);		 /* Exits if unable to initialize     */

    /* Perform initializations that apply to a specific instance */

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    /* Acquire and dispatch messages until a WM_QUIT message is received. */

    PWM_PrivateHelpMessage=RegisterWindowMessage("HelpHookMessage");

    if (PWM_PrivateHelpMessage)
    {
        fpMessageFilter=MakeProcInstance((FARPROC)HelpMessageFilterHook, hInstance);
        fnNextMessageFilterProc=SetWindowsHook(WH_MSGFILTER, fpMessageFilter);
    }

    while (GetMessage(&msg,	   /* message structure			     */
	    NULL,		   /* handle of window receiving the message */
	    NULL,		   /* lowest message to examine		     */
	    NULL))		   /* highest message to examine	     */
	{
	TranslateMessage(&msg);	   /* Translates virtual key codes	     */
	DispatchMessage(&msg);	   /* Dispatches message to window	     */
    }

    if (PWM_PrivateHelpMessage)
    {
        UnhookWindowsHook(WH_MSGFILTER, fpMessageFilter);
        FreeProcInstance(fpMessageFilter);
    }

    return (msg.wParam);	   /* Returns the value from PostQuitMessage */
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

    COMMENTS:

        This function is called at initialization time only if no other 
        instances of the application are running.  This function performs 
        initialization tasks that can be done once for any number of running 
        instances.  

        In this case, we initialize a window class by filling out a data 
        structure of type WNDCLASS and calling the Windows RegisterClass() 
        function.  Since all instances of this application use the same window 
        class, we only need to do this when the first instance is initialized.  


****************************************************************************/

BOOL InitApplication(hInstance)
HANDLE hInstance;			       /* current instance	     */
{
    WNDCLASS  wc;

    /* Fill in window class structure with parameters that describe the       */
    /* main window.                                                           */

    wc.style = NULL;                    /* Class style(s).                    */
    wc.lpfnWndProc = MainWndProc;       /* Function to retrieve messages for  */
                                        /* windows of this class.             */
    wc.cbClsExtra = 0;                  /* No per-class extra data.           */
    wc.cbWndExtra = 0;                  /* No per-window extra data.          */
    wc.hInstance = hInstance;           /* Application that owns the class.   */
    wc.hIcon = LoadIcon(hInstance, MAKEINTATOM(helphook));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = COLOR_WINDOW+1;
    wc.lpszMenuName =  "HelpHookMenu";   /* Name of menu resource in .RC file. */
    wc.lpszClassName = "HelpHookWClass"; /* Name used in call to CreateWindow. */

    /* Register the window class and return success/failure code. */

    return (RegisterClass(&wc));

}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

    COMMENTS:

        This function is called at initialization time for every instance of 
        this application.  This function performs initialization tasks that 
        cannot be shared by multiple instances.  

        In this case, we save the instance handle in a static variable and 
        create and display the main program window.  
        
****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
    HANDLE          hInstance;          /* Current instance identifier.       */
    int             nCmdShow;           /* Param for first ShowWindow() call. */
{
    HWND            hWnd;               /* Main window handle.                */

    /* Save the instance handle in static variable, which will be used in  */
    /* many subsequence calls from this application to Windows.            */

    hInst = hInstance;

    /* Create a main window for this application instance.  */

    hWnd = CreateWindow(
        "HelpHookWClass",                /* See RegisterClass() call.          */
        "HelpHook Sample Application",   /* Text for window title bar.         */
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

    hWndGlobal=hWnd;
    /* If window could not be created, return "failure" */

    if (!hWnd)
        return (FALSE);

    /* Make the window visible; update its client area; and return "success" */

    ShowWindow(hWnd, nCmdShow);  /* Show the window                        */
    UpdateWindow(hWnd);          /* Sends WM_PAINT message                 */
    return (TRUE);               /* Returns the value from PostQuitMessage */

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages

    MESSAGES:

	WM_COMMAND    - application menu (About dialog box)
	WM_DESTROY    - destroy window

    COMMENTS:

	To process the IDM_ABOUT message, call MakeProcInstance() to get the
	current instance address of the About() function.  Then call Dialog
	box which will create the box according to the information in your
	HelpHook.rc file and turn control over to the About() function.	When
	it returns, free the intance address.

****************************************************************************/

long FAR PASCAL MainWndProc(hWnd, message, wParam, lParam)
HWND hWnd;				  /* window handle		     */
UINT message;			      /* type of message		 */
WPARAM wParam;				    /* additional information	       */
LPARAM lParam;				    /* additional information	       */
{
    FARPROC lpProcAbout;		  /* pointer to the "About" function */

    switch (message) 
    {
	case WM_COMMAND:	   /* message: command from application menu */
        {
            switch (wParam)
            {
                case IDM_ABOUT:
                    lpProcAbout=MakeProcInstance(About, hInst);
                    DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
                    FreeProcInstance(lpProcAbout);
                    break;

                case IDM_DLG1:
                case IDM_DLG2:
                case IDM_DLG3:
                case IDM_DLG4:
                case IDM_DLG5:
                case IDM_DLG6:
                case IDM_DLG7:
                case IDM_DLG8:
                case IDM_DLG9:
                {
                    FARPROC lpProc;
                    lpProc=MakeProcInstance(SampleDialog, hInst);
                    DialogBox(hInst, MAKEINTRESOURCE(wParam), hWnd, lpProc);
                    FreeProcInstance(lpProc);
                    break;
                }
                default:
                    return (DefWindowProc(hWnd, message, wParam, lParam));
            }
            break;
        }

	case WM_DESTROY:		  /* message: window being destroyed */
	    PostQuitMessage(0);
	    break;

        case WM_MENUSELECT:
            dwCurrentMenuBits=lParam;
            wCurrentMenuCmd=wParam;
            return (DefWindowProc(hWnd, message, wParam, lParam));

	default:			  /* Passes it on if unproccessed    */
            if (message==PWM_PrivateHelpMessage)
                if (wParam==MSGF_MENU)
                    if (!(LOWORD(dwCurrentMenuBits) & MF_POPUP))
                        if (!(LOWORD(dwCurrentMenuBits) & MF_SYSMENU))
                        {
                            char szContext[10];

                            wsprintf(szContext, "%i", wCurrentMenuCmd);

                            MessageBox(hWnd, szContext, "WinHelp:  Context:", MB_OK);
                            break;
                        }
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
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    extern WORD PWM_PrivateHelpMessage;

    switch (message) 
    {
    case WM_INITDIALOG:           /* message: initialize dialog box */
        return (TRUE);

    case WM_COMMAND:              /* message: received a command */
            switch (wParam)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, TRUE);
                    return TRUE;
                    break;
                case ID_HELP:
DoHelp:
                    {
                        char szContext[10];

                        GetDlgItemText(hDlg, ID_HELPCONTEXT, szContext, 10);
                        MessageBox(hDlg, szContext, "WinHelp:  Context:", MB_OK);
                        break;
                    }
            }
            break;

        default:
            if (message==PWM_PrivateHelpMessage)
                goto DoHelp;
    }
    return (FALSE);                  /* Didn't process a message    */
}
/****************************************************************************
        HelpMessageFilterHook():
****************************************************************************/

DWORD FAR PASCAL HelpMessageFilterHook(nCode, wParam, lpMsg)
int nCode;
WORD wParam;
LPMSG lpMsg;
{
    extern WORD PWM_PrivateHelpMessage;

    if (nCode < 0)
        goto DefHook;

    if (!lpMsg)
        goto DefHook;

    if (nCode==MSGF_DIALOGBOX)
        if (lpMsg->message==WM_KEYDOWN && lpMsg->wParam==VK_F1)
        {
            HWND hTemp=NULL;
            HWND hParent=lpMsg->hwnd;

            while (hParent != NULL)
            {
                hTemp=hParent;
                if (!(GetWindowLong(hTemp, GWL_STYLE) & WS_CHILD))
                    break;
                hParent=GetWindowWord(hParent, GWW_HWNDPARENT);
            }

            if (hTemp)
                PostMessage(hTemp, PWM_PrivateHelpMessage, nCode, 0L);
            return TRUE;
        }

    if (nCode==MSGF_MENU)
        if (lpMsg->message==WM_KEYDOWN && lpMsg->wParam==VK_F1)
        {
            PostMessage(hWndGlobal, PWM_PrivateHelpMessage, nCode, 
                        MAKELONG((WORD)lpMsg->hwnd,0));
        }

DefHook:
    return DefHookProc(nCode, wParam, (LONG)lpMsg, &fnNextMessageFilterProc);
}

/****************************************************************************
        SampleDialog();
****************************************************************************/

BOOL FAR PASCAL SampleDialog(hDlg, message, wParam, lParam)
HWND hDlg;                                /* window handle of the dialog box */
unsigned message;                         /* type of message                 */
WORD wParam;                              /* message-specific information    */
LONG lParam;
{
    extern WORD PWM_PrivateHelpMessage;

    switch (message) 
    {
    case WM_INITDIALOG:           /* message: initialize dialog box */
        return (TRUE);

    case WM_COMMAND:              /* message: received a command */
            switch (wParam)
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, TRUE);
                    return TRUE;
                    break;

                case IDM_DLG1:
                case IDM_DLG2:
                case IDM_DLG3:
                case IDM_DLG4:
                case IDM_DLG5:
                case IDM_DLG6:
                case IDM_DLG7:
                case IDM_DLG8:
                case IDM_DLG9:
                {
                    FARPROC lpProc;
                    lpProc=MakeProcInstance(SampleDialog, hInst);
                    DialogBox(hInst, MAKEINTRESOURCE(wParam), hDlg, lpProc);
                    FreeProcInstance(lpProc);
                    break;
                }

                case ID_HELP:
DoHelpSample:
                    {
                        char szContext[10];

                        GetDlgItemText(hDlg, ID_HELPCONTEXT, szContext, 10);
                        MessageBox(hDlg, szContext, "WinHelp:  Context:", MB_OK);
                        break;
                    }
            }
            break;

        default:
            if (message==PWM_PrivateHelpMessage)
                goto DoHelpSample;
    }
    return (FALSE);                  /* Didn't process a message    */
}
