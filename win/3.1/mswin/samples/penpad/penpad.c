/***************************************************************************
 *									   *
 *  PROGRAM	: penpad.c						   *
 *									   *
 *  PURPOSE	: To give a demonstration of the easy modification of an   *
 *		  existing multi-pad application to be pen-aware.  The only*
 *		  difference between this code and the original            *
 *		  (PENPAD.OLD) is a subclassed window procedure called     *
 *		  FakeProc.						   *
 *                                                                *
 *        PenWin must be present to run this application            *
 *									   *
 *  FUNCTIONS	: WinMain()	      - Calls the initialization function  *
 *					and processes message loop	   *
 *									   *
 *		  PPFrameWndProc()    - Window function for the "frame"    *
 *					window, which controls the menu    *
 *					and contains the MDI document	   *
 *					windows as child windows.	   *
 *									   *
 *		  PPMDIChildWndProc() - Window function for the individual *
 *					document windows		   *
 *									   *
 *		  InitializeMenu()    - Handles enabling/greying of menu   *
 *					items according to the app's state.*
 *									   *
 *		  CloseAllChildren    - Destroys all MDI child windows.    *
 *									   *
 *		  CommandHandler()    - Processes the "frame" window's     *
 *					WM_COMMAND messages.		   *
 *									   *
 *		  AboutDlgProc()      - Dialog function for the ubiquitous *
 *					About.. dialog. 		   *
 *									   *
 *		  SetWrap()	      - Alters word wrapping in the edit   *
 *					control.			   *
 *									   *
 *		  PPError()	      - Flashes an error messagebox.	   *
 *									   *
 *		  QueryCloseChild     - Prompts for saving current MDI	   *
 *					child window.			   *
 *									   *
 *		  QueryCloseAllChildren() - Asks whether it is OK to close *
 *					    down app.			   *
 *									   *
 *		  FakeProc()	      - Subclassed window procedure to     *
 *					support pens.			   *
 *									   *
 ***************************************************************************/

#include "penpad.h"
#define NOPENINFO
#define NOVIRTEVENT
#define NORCDICTIONARY
#define NOCONFIGRECOG
#include <penwin.h>

/* global variables used in this module or among more than one module */
FARPROC lpfnFakeProc;		    /* Subclassed window procedure           */
FARPROC lpfnEditProc;		    /* Edit control procedure used during    */
				    /*  subclassing			     */
HANDLE hInst;			    /* Program instance handle		     */
HANDLE hAccel;			    /* Main accelerator resource	     */
HWND hwndFrame		 = NULL;    /* Handle to main window		     */
HWND hwndMDIClient	 = NULL;    /* Handle to MDI client		     */
HWND hwndActive 	 = NULL;    /* Handle to currently activated child   */
HWND hwndActiveEdit	 = NULL;    /* Handle to edit control		     */
LONG styleDefault    = WS_MAXIMIZE; /* Default style bits for child windows  */
				    /* The first window is created maximized */
				    /* to resemble Notepad.  Later children  */
				    /* are normal windows.		     */
LPCSTR lpMenu	     = IDPENPAD;  /* Contains the resource id of the	     */
				    /* current frame menu		     */
HCURSOR hCursor;


int (FAR PASCAL *lpfnProcessWriting)(HWND, LPRC);
int (FAR PASCAL *lpfnTPtoDP)(LPPOINT, int);

/* Forward declarations of helper functions in this module */
int       PASCAL WinMain( HANDLE, HANDLE, LPSTR, int);
VOID NEAR PASCAL InitializeMenu (HANDLE);
VOID NEAR PASCAL CommandHandler (HWND,WORD);
VOID NEAR PASCAL SetWrap (HWND,BOOL);
BOOL NEAR PASCAL QueryCloseAllChildren ( VOID );
int  NEAR PASCAL QueryCloseChild (HWND);
VOID NEAR PASCAL CloseAllChildren ( VOID );
BOOL NEAR PASCAL FCheckForPenWin(VOID);

/****************************************************************************
 *									    *
 *  FUNCTION   : WinMain(HANDLE, HANDLE, LPSTR, int)			    *
 *									    *
 *  PURPOSE    : Creates the "frame" window, does some initialization and   *
 *		 enters the message loop.				    *
 *									    *
 ****************************************************************************/
int PASCAL WinMain(hInstance, hPrevInstance, lpszCmdLine, nCmdShow)

HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR  lpszCmdLine;
int    nCmdShow;
{
    MSG msg;

    hInst = hInstance;

    /* If this is the first instance of the app. register window classes */
    if (!hPrevInstance){
	if (!InitializeApplication ())
	    return 0;
    }

	if (!FCheckForPenWin())
		return 0;

    /* Create the frame and do other initialization */
    if (!InitializeInstance (lpszCmdLine, nCmdShow))
		return 0;

	 /* Load appropriate cursor */ 
    hCursor = LoadCursor(NULL, IDC_PEN );	

    /* Enter main message loop */
    while (GetMessage (&msg, NULL, 0, 0)){
	/* If a keyboard message is for the MDI , let the MDI client
	 * take care of it.  Otherwise, check to see if it's a normal
	 * accelerator key (like F3 = find next).  Otherwise, just handle
	 * the message as usual.
	 */
	if ( !TranslateMDISysAccel (hwndMDIClient, &msg) &&
	     !TranslateAccelerator (hwndFrame, hAccel, &msg)){
	    TranslateMessage (&msg);
	    DispatchMessage (&msg);
	}
    }
    return 0;
}



/****************************************************************************
 *									    *
 *  FUNCTION   : FCheckForPenWin()     *
 *									    *
 *  PURPOSE    : Detect presences of Pen Win.  If not there, will just
 *             : behavior as a normal Win 3.0 applicaiton.
 *
 *  RETURN     : Return false on error. *
 ****************************************************************************/
BOOL NEAR PASCAL FCheckForPenWin()
	{
	BOOL fRet = FALSE;
	HANDLE hLib;

	if ( (hLib = GetSystemMetrics(SM_PENWINDOWS)) != NULL)
		{
		/* Load up pen win stubb; */ 
		if ( ((FARPROC)lpfnProcessWriting = GetProcAddress(hLib, "ProcessWriting")) != NULL)
			{
			if ( ((FARPROC)lpfnTPtoDP = GetProcAddress(hLib, "TPtoDP")) != NULL)
				fRet = TRUE;
			}


		}
	/* else running on a non pen aware system */

	if (!fRet)
		MessageBox(NULL, "This application requires Microsoft Windows for Pen Computing", "PenPad", MB_OK);

	return fRet;
	}




/****************************************************************************
 *									    *
 *  FUNCTION   : PPFrameWndProc (hwnd, msg, wParam, lParam )		    *
 *									    *
 *  PURPOSE    : The window function for the "frame" window, which controls *
 *		 the menu and encompasses all the MDI child windows. Does   *
 *		 the major part of the message processing. Specifically, in *
 *		 response to:						    *
 *									    *
 *		     WM_CREATE		: Creates and displays the "frame". *
 *									    *
 *		     WM_INITMENU	: Sets up the state of the menu.    *
 *									    *
 *		     WM_WININICHANGE &  : If default printer characteristics*
 *		     WM_DEVMODECHANGE	  have been changed, reinitialises  *
 *					  printer DC.			    *
 *									    *
 *		     WM_COMMAND 	: Passes control to a command-	    *
 *					  handling function.		    *
 *									    *
 *		     WM_CLOSE		: Quits the app. if all the child   *
 *					  windows agree.		    *
 *									    *
 *		     WM_QUERYENDSESSION : Checks that all child windows     *
 *					  agree to quit.		    *
 *									    *
 *		     WM_DESTROY 	: Destroys frame window and quits   *
 *					  app.				    *
 *									    *
 ****************************************************************************/
LONG FAR PASCAL PPFrameWndProc ( hwnd, msg, wParam, lParam )

register HWND	 hwnd;
UINT		 msg;
register WPARAM    wParam;
LPARAM		   lParam;

{
    switch (msg){
	case WM_CREATE:{

	    CLIENTCREATESTRUCT ccs;
	    HDC hdc;

	    /* Find window menu where children will be listed */
	    ccs.hWindowMenu = GetSubMenu (GetMenu(hwnd),WINDOWMENU);
	    ccs.idFirstChild = IDM_WINDOWCHILD;

	    /* Create the MDI client filling the client area */
	    hwndMDIClient = CreateWindow ("mdiclient",
					  NULL,
					  WS_CHILD | WS_CLIPCHILDREN |
					  WS_VSCROLL | WS_HSCROLL,
					  0,
					  0,
					  0,
					  0,
					  hwnd,
					  0xCAC,
					  hInst,
					  (LPSTR)&ccs);

	    ShowWindow (hwndMDIClient,SW_SHOW);

	    /* Check if printer can be initialized */
	    if (hdc = GetPrinterDC ()){
		DeleteDC (hdc);
	    }

	    break;
	}

	case WM_INITMENU:
	    /* Set up the menu state */
	    InitializeMenu ((HMENU)wParam);
	    break;

	case WM_WININICHANGE:
	case WM_DEVMODECHANGE:{

	    /*	If control panel changes default printer characteristics,
	     *	reinitialize our printer information...
	     */
	    HDC hdc;

	    if (hdc = GetPrinterDC ()){
		DeleteDC (hdc);
	    }
	    break;
	}


	case WM_COMMAND:
	    /* Direct all menu selection or accelerator commands to another
	     * function
	     */
	    CommandHandler (hwnd,wParam);
	    break;

	case WM_CLOSE:
	    /* don't close if any children cancel the operation */
	    if (!QueryCloseAllChildren ())
		break;
	    DestroyWindow (hwnd);
	    break;

	case WM_QUERYENDSESSION:
	    /*	Before session ends, check that all files are saved */
	    return QueryCloseAllChildren ();

	case WM_DESTROY:
	    PostQuitMessage (0);
	    break;

	default:
	    /*	use DefFrameProc() instead of DefWindowProc() since there
	     *	are things that have to be handled differently because of MDI
	     */
	    return DefFrameProc (hwnd,hwndMDIClient,msg,wParam,lParam);
    }
    return 0;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : PPMDIChildWndProc ( hwnd, msg, wParam, lParam )		    *
 *									    *
 *  PURPOSE    : The window function for the individual document windows,   *
 *		 each of which has a "note". Each of these windows contain  *
 *		 one multi-line edit control filling their client area.     *
 *		 In response to the following:				    *
 *									    *
 *		     WM_CREATE		: Creates & diplays an edit control *
 *					  and does some initialization.     *
 *									    *
 *		     WM_MDIACTIVATE	: Activates/deactivates the child.  *
 *									    *
 *		     WM_SETFOCUS	: Sets focus on the edit control.   *
 *									    *
 *		     WM_SIZE		: Resizes the edit control.	    *
 *									    *
 *		     WM_COMMAND 	: Processes some of the edit	    *
 *					  commands, saves files and alters  *
 *					  the edit wrap state.		    *
 *									    *
 *		     WM_CLOSE		: Closes child if it is ok to do so.*
 *									    *
 *		     WM_QUERYENDSESSION : Same as above.		    *
 *									    *
 ****************************************************************************/

LONG FAR PASCAL PPMDIChildWndProc ( hwnd, msg, wParam, lParam )

register HWND	hwnd;
UINT		msg;
register WPARAM   wParam;
LPARAM		  lParam;

{
    HWND hwndEdit;

    switch (msg){
	case WM_CREATE:
	    /* Create an edit control */
	    hwndEdit = CreateWindow ("edit",
				     NULL,
				     WS_CHILD|WS_HSCROLL|WS_MAXIMIZE|WS_VISIBLE|WS_VSCROLL|ES_AUTOHSCROLL|ES_AUTOVSCROLL|ES_MULTILINE,
				     0,
				     0,
				     0,
				     0,
				     hwnd,
				     ID_EDIT,
				     hInst,
				     NULL);

	    /* Subclass this window for handwriting support.  Easier to
			just use hedit's, but this allows ProcessWriting to
			be demostrating.
		*/
	    
	    lpfnEditProc = (FARPROC)GetWindowLong(hwndEdit, GWL_WNDPROC);
    	    lpfnFakeProc = MakeProcInstance((FARPROC)FakeProc, hInst);
    	    SetWindowLong(hwndEdit, GWL_WNDPROC, (LONG)lpfnFakeProc);

	    /* Remember the window handle and initialize some window attributes */
	    SetWindowWord (hwnd, GWW_HWNDEDIT, (WORD)hwndEdit);
	    SetWindowWord (hwnd, GWW_CHANGED, FALSE);
	    SetWindowWord (hwnd, GWW_WORDWRAP, FALSE);
	    SetWindowWord (hwnd, GWW_UNTITLED, TRUE);
	    SetFocus (hwndEdit);
	    break;

	case WM_MDIACTIVATE:
	    /* If we're activating this child, remember it */
	    if (wParam){
		hwndActive     = hwnd;
		hwndActiveEdit = (HWND)GetWindowWord (hwnd, GWW_HWNDEDIT);
	    }
	    else{
		hwndActive     = NULL;
		hwndActiveEdit = NULL;
	    }
	    break;

	case WM_QUERYENDSESSION:
	    /* Prompt to save the child */
	    return !QueryCloseChild (hwnd);

	case WM_CLOSE:
	    /* If its OK to close the child, do so, else ignore */
	    if (QueryCloseChild (hwnd))
		goto CallDCP;
	    else
		break;

	case WM_SIZE:{
	    RECT rc;

	    /* On creation or resize, size the edit control. */
	    hwndEdit = GetWindowWord (hwnd, GWW_HWNDEDIT);
	    GetClientRect (hwnd, &rc);
	    MoveWindow (hwndEdit,
			rc.left,
			rc.top,
			rc.right-rc.left,
			rc.bottom-rc.top,
			TRUE);
	    goto CallDCP;
	}

	case WM_SETFOCUS:
	    SetFocus (GetWindowWord (hwnd, GWW_HWNDEDIT));
	    break;

	case WM_COMMAND:
	    switch (wParam){
		case ID_EDIT:
		    switch (HIWORD(lParam)){
			case EN_CHANGE:

			    /* If the contents of the edit control have changed,
			       set the changed flag
			     */
			    SetWindowWord (hwnd, GWW_CHANGED, TRUE);
			    break;

			case EN_ERRSPACE:
			    /* If the control is out of space, honk */
			    MessageBeep (0);
			    break;

			default:
			    goto CallDCP;
		    }
		    break;

		case IDM_FILESAVE:
		    /* If empty file, ignore save */
		    if ((GetWindowWord(hwnd, GWW_UNTITLED)) && (!ChangeFile(hwnd)))
			break;

		    /* Save the contents of the edit control and reset the
		     * changed flag
		     */
		    SaveFile (hwnd);
		    SetWindowWord (hwnd, GWW_CHANGED, FALSE);
		    break;

		case IDM_EDITWRAP: {
		    int fWrap = GetWindowWord (hwnd, GWW_WORDWRAP);

		    /* Set the wrap state, or report it */
		    if (LOWORD (lParam)){
			fWrap = !fWrap;
			SetWrap (hwnd, fWrap);
		    }

		    /* return wrap state */
		    return fWrap;
		}

		default:
		    goto CallDCP;
	    }
	    break;

	default:
CallDCP:
	    /* Again, since the MDI default behaviour is a little different,
	     * call DefMDIChildProc instead of DefWindowProc()
	     */
	    return DefMDIChildProc (hwnd, msg, wParam, lParam);
    }
    return FALSE;
}


/****************************************************************************
 *									    *
 *  FUNCTION   : AboutDlgProc ( hwnd, msg, wParam, lParam )		    *
 *									    *
 *  PURPOSE    : Dialog function for the About PenPad... dialog.	    *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL AboutDlgProc ( hwnd, msg, wParam, lParam )
HWND	      hwnd;
register WORD msg;
register WORD wParam;
LONG	      lParam;
{
	lParam;					/* Needed to prevent compiler warning message */

    switch (msg){
	case WM_INITDIALOG:
	    /* nothing to initialize */
	    break;

	case WM_COMMAND:
	    switch (wParam){
		case IDOK:
		case IDCANCEL:
		    EndDialog(hwnd, 0);
		    break;

		default:
		    return FALSE;
	    }
	    break;

	default:
	    return FALSE;
    }

    return TRUE;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : Initializemenu ( hMenu )				    *
 *									    *
 *  PURPOSE    : Sets up greying, enabling and checking of main menu items  *
 *		 based on the app's state.                                  *
 *									    *
 ****************************************************************************/
VOID NEAR PASCAL InitializeMenu ( hmenu )
register HANDLE hmenu;
{
    register WORD status;
    int 	  i;
    long	  l;

    /* Is there any active child to talk to? */
    if (hwndActiveEdit){
	/* If edit control can respond to an undo request, enable the
	 * undo selection.
	 */
	if (SendMessage (hwndActiveEdit, EM_CANUNDO, 0, 0L))
	    status = MF_ENABLED;
	else
	    status = MF_GRAYED;
	EnableMenuItem (hmenu, IDM_EDITUNDO, status);

	/* If edit control is non-empty, allow cut/copy/clear */
	l      = SendMessage (hwndActiveEdit, EM_GETSEL, 0, 0L);
	status = (HIWORD(l) == LOWORD(l)) ? MF_GRAYED : MF_ENABLED;
	EnableMenuItem (hmenu, IDM_EDITCUT, status);
	EnableMenuItem (hmenu, IDM_EDITCOPY, status);
	EnableMenuItem (hmenu, IDM_EDITCLEAR, status);

	status=MF_GRAYED;
	/* If the clipboard contains some CF_TEXT data, allow paste */
	if (OpenClipboard (hwndFrame)){
	    int wFmt = 0;

	    while (wFmt = EnumClipboardFormats (wFmt))
		if (wFmt == CF_TEXT){
		    status = MF_ENABLED;
		    break;
		}

	    CloseClipboard ();
	}
	EnableMenuItem (hmenu, IDM_EDITPASTE, status);

	/* Set the word wrap state for the window */
	if ((WORD) SendMessage (hwndActive, WM_COMMAND, IDM_EDITWRAP, 0L))
	    status = MF_CHECKED;
	else
	    status = MF_UNCHECKED;
	CheckMenuItem (hmenu, IDM_EDITWRAP, status);

	/* Enable search menu items only if there is a search string */
	if (*szSearch)
	    status = MF_ENABLED;
	else
	    status = MF_GRAYED;
	EnableMenuItem (hmenu, IDM_SEARCHNEXT, status);
	EnableMenuItem (hmenu, IDM_SEARCHPREV, status);

	/* Enable File/Print only if a printer is available */
	status = iPrinter ? MF_ENABLED : MF_GRAYED;
	EnableMenuItem (hmenu, IDM_FILEPRINT, status);

	/* select all and wrap toggle always enabled */
	status = MF_ENABLED;
	EnableMenuItem(hmenu, IDM_EDITSELECT, status);
	EnableMenuItem(hmenu, IDM_EDITWRAP, status);
	EnableMenuItem(hmenu, IDM_SEARCHFIND, status);
    }
    else {
	/* There are no active child windows */
	status = MF_GRAYED;

	/* No active window, so disable everything */
	for (i = IDM_EDITFIRST; i <= IDM_EDITLAST; i++)
	    EnableMenuItem (hmenu, i, status);

	CheckMenuItem (hmenu, IDM_EDITWRAP, MF_UNCHECKED);

	for (i = IDM_SEARCHFIRST; i <= IDM_SEARCHLAST; i++)
	    EnableMenuItem (hmenu, i, status);

	EnableMenuItem (hmenu, IDM_FILEPRINT, status);

    }

    /* The following menu items are enabled if there is an active window */
    EnableMenuItem (hmenu, IDM_FILESAVE, status);
    EnableMenuItem (hmenu, IDM_FILESAVEAS, status);
    EnableMenuItem (hmenu, IDM_WINDOWTILE, status);
    EnableMenuItem (hmenu, IDM_WINDOWCASCADE, status);
    EnableMenuItem (hmenu, IDM_WINDOWICONS, status);
    EnableMenuItem (hmenu, IDM_WINDOWCLOSEALL, status);

    /* Allow printer setup only if printer driver supports device initialization */
    if (iPrinter < 2)
	status = MF_GRAYED;
    EnableMenuItem ( hmenu, IDM_FILESETUP, status);

}

/****************************************************************************
 *									    *
 *  FUNCTION   : CloseAllChildren ()					    *
 *									    *
 *  PURPOSE    : Destroys all MDI child windows.			    *
 *									    *
 ****************************************************************************/
VOID NEAR PASCAL CloseAllChildren ()
{
    register HWND hwndT;

	 /* kludge : if not minimized, closeall rips. multipad (win SDK sample)
		 also has this bug. Bug was reported, but not fixed yet. */
    ShowWindow(hwndMDIClient,SW_MINIMIZE);

    /* hide the MDI client window to avoid multiple repaints */
    ShowWindow(hwndMDIClient,SW_HIDE);

    /* As long as the MDI client has a child, destroy it */
    while ( hwndT = GetWindow (hwndMDIClient, GW_CHILD)){

	/* Skip the icon title windows */
	while (hwndT && GetWindow (hwndT, GW_OWNER))
	    hwndT = GetWindow (hwndT, GW_HWNDNEXT);

	if (!hwndT)
	    break;

	SendMessage (hwndMDIClient, WM_MDIDESTROY, (WORD)hwndT, 0L);
    }
}

/****************************************************************************
 *									    *
 *  FUNCTION   : CommandHandler ()					    *
 *									    *
 *  PURPOSE    : Processes all "frame" WM_COMMAND messages.		    *
 *									    *
 ****************************************************************************/
VOID NEAR PASCAL CommandHandler ( hwnd, wParam )
register HWND hwnd;
register WORD wParam;

{
    switch (wParam){
	case IDM_FILENEW:
	    /* Add a new, empty MDI child */
	    AddFile (NULL);
	    break;

	case IDM_FILEOPEN:
	    ReadFile (hwnd);
	    break;

	case IDM_FILESAVE:
	    /* Save the active child MDI */
	    SendMessage (hwndActive, WM_COMMAND, IDM_FILESAVE, 0L);
	    break;

	case IDM_FILESAVEAS:
	    /* Save active child MDI under another name */
	    if (ChangeFile (hwndActive))
		SendMessage (hwndActive, WM_COMMAND, IDM_FILESAVE, 0L);
	    break;

	case IDM_FILEPRINT:
	    /* Print the active child MDI */
	    PrintFile (hwndActive);
	    break;

	case IDM_FILESETUP:
	    /* Set up the printer environment for this app */
	    GetInitializationData (hwnd);
	    break;

	case IDM_FILEMENU:{

	      /* lengthen / shorten the size of the MDI menu */
	      HMENU hMenu;
	      HMENU hWindowMenu;
	      int i;

	      if (lpMenu == IDPENPAD){
		  lpMenu = IDPENPAD2;
		  i	 = SHORTMENU;
	      }
	      else{
		  lpMenu = IDPENPAD;
		  i	 = WINDOWMENU;
	      }

	      hMenu = LoadMenu (hInst, lpMenu);
	      hWindowMenu = GetSubMenu (hMenu, i);

	      /* Set the new menu */
	      hMenu = (HMENU)SendMessage (hwndMDIClient,
					  WM_MDISETMENU,
					  0,
					  MAKELONG(hMenu,hWindowMenu));

	      DestroyMenu (hMenu);
	      DrawMenuBar (hwndFrame);
	      break;
	}

	case IDM_FILEEXIT:
	    /* Close Penpad */
	    SendMessage (hwnd, WM_CLOSE, 0, 0L);
	    break;

	case IDM_HELPABOUT:{
	    /* Bring up the ubiquitous Ego box */
	    FARPROC lpfn;

	    lpfn = MakeProcInstance(AboutDlgProc, hInst);
	    DialogBox (hInst, IDD_ABOUT, hwnd, lpfn);
	    FreeProcInstance (lpfn);
	    break;
	}

	/* The following are edit commands. Pass these off to the active
	 * child's edit control window.
	 */
	case IDM_EDITCOPY:
	    SendMessage (hwndActiveEdit, WM_COPY, 0, 0L);
	    break;

	case IDM_EDITPASTE:
	    SendMessage (hwndActiveEdit, WM_PASTE, 0, 0L);
	    break;

	case IDM_EDITCUT:
	    SendMessage (hwndActiveEdit, WM_CUT, 0, 0L);
	    break;

	case IDM_EDITCLEAR:
	    SendMessage (hwndActiveEdit, EM_REPLACESEL, 0,( LONG)(LPSTR)"");
	    break;

	case IDM_EDITSELECT:
	    SendMessage (hwndActiveEdit, EM_SETSEL, 0, MAKELONG(0, 0xe000));
	    break;

	case IDM_EDITUNDO:
	    SendMessage (hwndActiveEdit, EM_UNDO, 0, 0L);
	    break;

	case IDM_EDITWRAP:
	    SendMessage (hwndActive, WM_COMMAND, IDM_EDITWRAP, 1L);
	    break;

	case IDM_SEARCHFIND:
	    /* Put up the find dialog box */
	    Find ();
	    break;

	case IDM_SEARCHNEXT:
	    /* Find next occurence */
	    FindNext ();
	    break;

	case IDM_SEARCHPREV:
	    /* Find previous occurence */
	    FindPrev ();
	    break;

	/* The following are window commands - these are handled by the
	 * MDI Client.
	 */
	case IDM_WINDOWTILE:
	    /* Tile MDI windows */
	    SendMessage (hwndMDIClient, WM_MDITILE, 0, 0L);
	    break;

	case IDM_WINDOWCASCADE:
	    /* Cascade MDI windows */
	    SendMessage (hwndMDIClient, WM_MDICASCADE, 0, 0L);
	    break;

	case IDM_WINDOWICONS:
	    /* Auto - arrange MDI icons */
	    SendMessage (hwndMDIClient, WM_MDIICONARRANGE, 0, 0L);
	    break;

	case IDM_WINDOWCLOSEALL:
	    /* Abort operation if something is not saved */
	    if (!QueryCloseAllChildren())
		break;

	    CloseAllChildren();

	    
		 /* kludge : since we minimized the window to prevent closeall tp rip.
			 we must now restore it */
		 ShowWindow( hwndMDIClient, SW_RESTORE);
		
		 /* Show the window since CloseAllChilren() hides the window
	     * for fewer repaints.
	     */
	    
		 ShowWindow( hwndMDIClient, SW_SHOW);

	    break;

	default:
	   /*
	    * This is essential, since there are frame WM_COMMANDS generated
	    * by the MDI system for activating child windows via the
	    * window menu.
	    */
	    DefFrameProc(hwnd, hwndMDIClient, WM_COMMAND, wParam, 0L);
    }
}
/****************************************************************************
 *									    *
 *  FUNCTION   : SetWrap ()						    *
 *									    *
 *  PURPOSE    : Changes the word wrapping in an edit control. Since this   *
 *		 cannot be done by direct means, the function creates a new *
 *		 edit control, moves data from the old control to the new   *
 *		 control and destroys the original control. Note that the   *
 *		 function assumes that the child being modified is currently*
 *		 active.						    *	    *
 *									    *
 ****************************************************************************/

VOID NEAR PASCAL SetWrap(hwnd, fWrap)
HWND hwnd;
BOOL fWrap;

{
    LONG    dws;
    HANDLE  hT;
    HANDLE  hTT;
    HWND    hwndOld;
    HWND    hwndNew;
    FARPROC lpfnT;

    /* Change word wrap mode */
    SetWindowWord (hwnd, GWW_WORDWRAP, fWrap);

    /* Create the appropriate window style, adding a horizontal scroll
     * facility if wrapping is not present.
     */
    dws = WS_CHILD | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE;
    if (!fWrap)
	dws |= WS_HSCROLL | ES_AUTOHSCROLL;

    /* Create a new child window */
    hwndNew = CreateWindow ( "edit",
			     NULL,
			     dws,
			     0,
			     SW_SHOW,
			     0,
			     0,
			     hwnd,
			     ID_EDIT,
			     hInst,
			     NULL);

    /* Get handle to current edit control */
    hwndOld = GetWindowWord (hwnd, GWW_HWNDEDIT);

    /* Get the data handle of the old control */
    hT = (HANDLE)SendMessage (hwndOld, EM_GETHANDLE, 0, 0L);

    /* Get the subclassed window proc */
    lpfnT = (FARPROC)GetWindowLong(hwndOld, GWL_WNDPROC);
    SetWindowLong(hwndNew, GWL_WNDPROC, (LONG)lpfnT);

    /* Create a dummy data handle and make it the handle to
     * the old edit control( hT still references the text of
     * old control).
     */
    hTT = LocalAlloc (LHND, 0);
    SendMessage (hwndOld, EM_SETHANDLE, hTT, 0L);

    /* Make the new window the window of interest and destroy the
     * old control.
     */
    SetWindowWord (hwnd, GWW_HWNDEDIT, hwndNew);
    hwndActiveEdit = hwndNew;
    DestroyWindow (hwndOld);

    /* Cause the window to be properly sized */
    SendMessage (hwnd, WM_SIZE, 0, 0L);

    /* Free the new window's old data handle and set it to
     * hT (text of old edit control)
     */
    LocalFree ((HANDLE)SendMessage (hwndNew, EM_GETHANDLE, 0, 0L));
    SendMessage (hwndNew, EM_SETHANDLE, hT, 0L);

    ShowWindow (hwndNew, SW_SHOW);

    /* Set focus to the new edit control */
    SetFocus (hwndNew);

}


/****************************************************************************
 *									    *
 *  FUNCTION   : PPError ( hwnd, flags, id, ...)			    *
 *									    *
 *  PURPOSE    : Flashes a Message Box to the user. The format string is    *
 *		 taken from the STRINGTABLE.				    *
 *									    *
 *  RETURNS    : Returns value returned by MessageBox() to the caller.	    *
 *									    *
 ****************************************************************************/
short FAR CDECL PPError(hwnd, bFlags, id, ...)
HWND hwnd;
WORD bFlags;
WORD id;
{
    char sz[160];
    char szFmt[128];

	 hwnd;					/* Needed to prevent compiler warning message */

    LoadString (hInst, id, szFmt, sizeof (szFmt));
    wvsprintf (sz, szFmt, (LPSTR)(&id + 1));
    LoadString (hInst, IDS_APPNAME, szFmt, sizeof (szFmt));
    return MessageBox (hwndFrame, sz, szFmt, bFlags);
}


/****************************************************************************
 *									    *
 *  FUNCTION   : QueryCloseAllChildren()				    *
 *									    *
 *  PURPOSE    : Asks the child windows if it is ok to close up app. Nothing*
 *		 is destroyed at this point. The z-order is not changed.    *
 *									    *
 *  RETURNS    : TRUE - If all children agree to the query.		    *
 *		 FALSE- If any one of them disagrees.			    *
 *									    *
 ****************************************************************************/

BOOL NEAR PASCAL QueryCloseAllChildren()
{
    register HWND hwndT;

    for ( hwndT = GetWindow (hwndMDIClient, GW_CHILD);
	  hwndT;
	  hwndT = GetWindow (hwndT, GW_HWNDNEXT)       ){

	/* Skip if an icon title window */
	if (GetWindow (hwndT, GW_OWNER))
	    continue;

	if (SendMessage (hwndT, WM_QUERYENDSESSION, 0, 0L))
	    return FALSE;
    }
    return TRUE;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : QueryCloseChild (hwnd) 				    *
 *									    *
 *  PURPOSE    : If the child MDI is unsaved, allow the user to save, not   *
 *               save, or cancel the close operation.                       *
 *									    *
 *  RETURNS    : TRUE  - if user chooses save or not save, or if the file   *
 *                       has not changed.                                   *
 *		 FALSE - otherwise.					    *
 *									    *
 ****************************************************************************/

BOOL NEAR PASCAL QueryCloseChild(hwnd)
register HWND hwnd;
{
    char	 sz [64];
    register int i;

    /* Return OK if edit control has not changed. */
    if (!GetWindowWord (hwnd, GWW_CHANGED))
	return TRUE;

    GetWindowText (hwnd, sz, sizeof(sz));

    /* Ask user whether to save / not save / cancel */
    i = PPError (hwnd,
		 MB_YESNOCANCEL|MB_ICONQUESTION,IDS_CLOSESAVE,
		 (LPSTR)sz);

    switch (i){
	case IDYES:
	    /* User wants file saved */
	    SaveFile(hwnd);
	    break;

	case IDNO:
	    /* User doesn't want file saved */
	    break;

	default:
	    /* We couldn't do the messagebox, or not ok to close */
	    return FALSE;
    }
    return TRUE;
}


/****************************************************************************
 *									    *
 *  FUNCTION   : FakeProc ( hwnd, msg, wParam, lParam )			    *
 *									    *
 *  PURPOSE    : The subclassed window function for the individual document *
 *		 windows.  It is this simple modification that allows the   *
 *		 support for pens.  Note the difference between this        *
 *		 file and the original non-pen-aware PENPAD.OLD.	    *
 *									    *
 *		 In response to the following:				    *
 *									    *
 *		     WM_SETCURSOR	: Sets the cursor to a pen while    *
 *					  cursor is in this window.         *
 *									    *
 *		     WM_RBUTTONDOWN and					    *
 *		     WM_RBUTTONUP	: Translates to WM_LBUTTON* message.*
 *									    *
 *		     WM_LBUTTONDOWN	: Invokes the recognizer via the    *
 *					  ProcessWriting API.		    *
 *									    *
 *		     WM_RCRESULT	: Returns FALSE.  This message is   *
 *					  a Pen Windows message sent by the *
 *					  recognizer when results are       *
 *					  available.			    *
 *									    *
 ****************************************************************************/

LONG FAR PASCAL FakeProc ( hwnd, msg, wParam, lParam )

HWND	hwnd;
WORD	msg;
WORD	wParam;
LONG	lParam;

{
    switch (msg){

	case WM_SETCURSOR:
	    /* Change cursor to a pen if in the client area of edit control */
	    if (LOWORD(lParam) == HTCLIENT)
	    	{
	    	SetCursor(hCursor);
	    	return TRUE;
	    	}
	    break;

	case WM_LBUTTONDOWN:
	    /* Check to see if button-down should invoke recognizer */
	    if ((*lpfnProcessWriting)(hwnd, NULL) >= 0)
	    	return TRUE;	/* Recognition has started, do not pass to default proc */
	    break;				/* Else, do default edit processing */


	case WM_RCRESULT:
	   /* Returning FALSE will cause default recognition handling
	      to occur.

			ProcessWriting will convert the result to messages that this
			WndProc already handles (such as WM_CHAR's, WM_LBUTTONDOWNS)
			We need only check for gestures that need to detect
			if there is a selection and if not, select word or character
			under gesture
	   */
		{
		LPRCRESULT lpr = (LPRCRESULT)lParam;

	   /*	Check if a gesture, but not a circle-letter gesture which has been
			mapped to one of the standard gestures. */ 

		if ((lpr->wResultsType&RCRT_GESTURE) && (lpr->wResultsType&RCRT_GESTURETRANSLATED)==0)
			{
			switch (*lpr->lpsyv)
				{
			case SYV_COPY:
			case SYV_CUT:
			case SYV_CLEAR:
				{
				LONG lSel = SendMessage(hwnd, EM_GETSEL, 0, 0L);
				POINT ptHotSpot;

				if (LOWORD(lSel) >= HIWORD(lSel))
				   /* There is no selection, so send in doubleclick to
						select word underselection (or for clear, just set
						insertion point since we want to delete a character. */ 
					{
					LONG lParamPt;
					ptHotSpot = lpr->syg.rgpntHotSpots[0];

					(*lpfnTPtoDP)(&ptHotSpot, 1);
					ScreenToClient(hwnd, &ptHotSpot);
					lParamPt = *(LONG FAR *)&ptHotSpot;
					SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, lParamPt);
					SendMessage(hwnd, WM_LBUTTONUP, 0, lParamPt);
					if (*lpr->lpsyv != SYV_CLEAR)
						{
						SendMessage(hwnd, WM_LBUTTONDBLCLK, MK_LBUTTON, lParamPt);
						SendMessage(hwnd, WM_LBUTTONUP, 0, lParamPt);
						}
					else
						{
					   /*	If we send this window a CLEAR and nothing is selected,
							the character to the right will not be deleted, so
							send a delete key. */ 
						SendMessage(hwnd, WM_KEYDOWN, VK_DELETE, 0L);
						SendMessage(hwnd, WM_KEYUP, VK_DELETE, 0L);
						return 0;
						}

				   /* Fall through and return false, will receive one of
						WM_COPY, WM_CUT */ 
					}

				}
				break;

			default:
				break;
				}
			}
	    return FALSE;
		}

	default:
	    /* Since this window procedure is a subclass, must call the
	       real window procedure now. */
	    break;
    }
    return CallWindowProc (lpfnEditProc, hwnd, msg, wParam, lParam);
}
