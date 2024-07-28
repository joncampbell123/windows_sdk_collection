/*----------------------------------------------------------------------------*\
 *
 *  MCIPUZZLE:
 *
 *    Sample app showing the use of MCIWnd, installable draw procedure
 *
 *    MCIPUZZLE is a MDI aplication that demonstates the following:
 *
 *      - Using the MCIWnd window class to play MCI files.
 *
 *	- Using an installable draw procedure with MCIAVI
 *
 *	- Implementing a version of the classic "15" puzzle
 *
 *----------------------------------------------------------------------------*/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/



#include <windows.h>
#include <commdlg.h>

#include "mmsystem.h"
#include "digitalv.h"
#include "mciavi.h"

#include <vfw.h>

#include "mciwnd.h"

#include "mcipuzzl.h"

#include "puzzle.h"

typedef LONG (FAR PASCAL *LPWNDPROC)(); // pointer to a window procedure

/*-------------------------------------------------------------------------*\
|                                                                          |
|   g l o b a l   v a r i a b l e s                                        |
|                                                                          |
\*------------------------------------------------------------------------*/
char    szAppName[]  = "MCIPuzzle";   /* change this to your app's name */
char    szDocClass[] = MCIWND_WINDOW_CLASS;

char    szOpenFilter[] = "Video Files\0*.avi\0";

HANDLE  hInstApp;                   /* Instance handle */
HACCEL  hAccelApp;
HWND    hwndApp;                    /* Handle to parent window */
HWND    hwndMCI;                    /* Handle to MCI client window */
PUZZLE	gPuzzle;

OFSTRUCT     of;
OPENFILENAME ofn;
char         achFileName[128];

static FARPROC lpfnHook = NULL;

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

long FAR PASCAL _export AppWndProc(HWND, UINT, WPARAM, LPARAM);
long FAR PASCAL _export mdiDocWndProc(HWND, unsigned, WORD, LONG);
int ErrMsg (LPSTR sz,...);

BOOL FAR RemoveAVIDrawHandler(void);
BOOL FAR InstallAVIDrawHandler(void);
#include "mciavi.h"
LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

/*----------------------------------------------------------------------------*\
|   AppAbout( hDlg, msg, wParam, lParam )                                      |
|                                                                              |
|   Description:                                                               |
|       This function handles messages belonging to the "About" dialog box.    |
|       The only message that it looks for is WM_COMMAND, indicating the use   |
|       has pressed the "OK" button.  When this happens, it takes down         |
|       the dialog box.                                                        |
|                                                                              |
|   Arguments:                                                                 |
|       hDlg            window handle of about dialog window                   |
|       msg             message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if message has been processed, else FALSE                         |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL FAR PASCAL _export AppAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_COMMAND:
            EndDialog(hwnd,TRUE);
            return TRUE;

        case WM_INITDIALOG:
	    return TRUE;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*\
|   AppInit ( hInstance, hPrevInstance )				       |
|                                                                              |
|   Description:                                                               |
|       This is called when the application is first loaded into               |
|       memory.  It performs all initialization that doesn't need to be done   |
|       once per instance.                                                     |
|                                                                              |
|   Arguments:                                                                 |
|	hPrevInstance	instance handle of previous instance		       |
|       hInstance       instance handle of current instance                    |
|                                                                              |
|   Returns:                                                                   |
|       TRUE if successful, FALSE if not                                       |
|                                                                              |
\*----------------------------------------------------------------------------*/
BOOL AppInit(HANDLE hInst, HANDLE hPrev, LPSTR szCmd, int sw)
{
    WNDCLASS    cls;
    WORD	wVer;

    /* first let's make sure we are running on 1.1 */
    wVer = HIWORD(VideoForWindowsVersion());
    if (wVer < 0x010a){
	    /* oops, we are too old, blow out of here */
	    MessageBeep(MB_ICONHAND);
	    MessageBox(NULL, "Video for Windows version is too old",
		       "MCI Puzzle Error", MB_OK|MB_ICONSTOP);
	    return FALSE;
    }
    
    
    /* Save instance handle for DialogBox */
    hInstApp = hInst;

    hAccelApp = LoadAccelerators(hInstApp, "AppAccel");

    if (!hPrev) {
        cls.hCursor        = LoadCursor(NULL,IDC_ARROW);
        cls.hIcon          = LoadIcon(hInst,"AppIcon");
        cls.lpszMenuName   = "AppMenu";
        cls.lpszClassName  = szAppName;
        cls.hbrBackground  = (HBRUSH)COLOR_APPWORKSPACE+1;
        cls.hInstance      = hInst;
        cls.style          = 0;
        cls.lpfnWndProc    = (WNDPROC)AppWndProc;
        cls.cbClsExtra     = 0;
	cls.cbWndExtra	   = 0;

        if (!RegisterClass(&cls))
            return FALSE;
    }

    hwndApp = CreateWindow(szAppName,szAppName,
	       WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
	       CW_USEDEFAULT, 0,
	       160, 120,
	       (HWND)NULL,	  /* no parent */
	       (HMENU)NULL,	  /* use class menu */
               (HANDLE)hInst,     /* handle to window instance */
	       (LPSTR)NULL	  /* no params to pass on */
	     );

    InitPuzzle(&gPuzzle, TRUE /* FALSE to not scramble */);

    if (szCmd && szCmd[0]) {
	MCIWndOpen(hwndMCI, szCmd, 0);
    }

    /* Make window visible according to the way the app is activated */
    ShowWindow(hwndApp,sw);

    return TRUE;
}

/*----------------------------------------------------------------------------*\
|   WinMain( hInstance, hPrevInstance, lpszCmdLine, cmdShow )                  |
|                                                                              |
|   Description:                                                               |
|       The main procedure for the App.  After initializing, it just goes      |
|       into a message-processing loop until it gets a WM_QUIT message         |
|       (meaning the app was closed).                                          |
|                                                                              |
|   Arguments:                                                                 |
|       hInstance       instance handle of this instance of the app            |
|       hPrevInstance   instance handle of previous instance, NULL if first    |
|       lpszCmdLine     ->null-terminated command line                         |
|       cmdShow         specifies how the window is initially displayed        |
|                                                                              |
|   Returns:                                                                   |
|       The exit code as specified in the WM_QUIT message.                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int sw)
{
    MSG     msg;

    if (!AppInit(hInstance,hPrevInstance,szCmdLine,sw))
       return FALSE;

    /*
     * Polling messages from event queue
     */
    for (;;)
    {
        if (PeekMessage(&msg, NULL, 0, 0,PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;

            if (hAccelApp && TranslateAccelerator(hwndApp, hAccelApp, &msg))
                continue;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // idle time here, DONT BE A PIG!
            WaitMessage();
        }
    }

    return msg.wParam;
}

/*----------------------------------------------------------------------------*\
\*----------------------------------------------------------------------------*/

BOOL fDialog(HWND hwnd,int id,FARPROC fpfn)
{
    BOOL	f;
    HANDLE	hInst;

    hInst = (HINSTANCE)GetWindowWord(hwnd,GWW_HINSTANCE);
    fpfn  = MakeProcInstance(fpfn,hInst);
    f = DialogBox(hInst,MAKEINTRESOURCE(id),hwnd,(DLGPROC)fpfn);
    FreeProcInstance (fpfn);
    return f;
}

/*----------------------------------------------------------------------------*\
|   AppWndProc( hwnd, msg, wParam, lParam )                                    |
|                                                                              |
|   Description:                                                               |
|       The window proc for the app's main (tiled) window.  This processes all |
|       of the parent window's messages.                                       |
|									       |
|   Arguments:                                                                 |
|       hwnd            window handle for the parent window                    |
|       msg             message number                                         |
|       wParam          message-dependent                                      |
|       lParam          message-dependent                                      |
|                                                                              |
|   Returns:                                                                   |
|       0 if processed, nonzero if ignored                                     |
|                                                                              |
\*----------------------------------------------------------------------------*/
long FAR PASCAL _export AppWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    UINT            f;
    PAINTSTRUCT     ps;
    HDC             hdc;
    MCI_DGV_SETVIDEO_PARMS  dgv;
    DWORD	    dw;
    RECT	    rc;
    UINT	    uDevice;

    switch (msg) {
        case WM_COMMAND:

	    switch(wParam) {
		case MENU_ABOUT:
                    fDialog(hwnd,ABOUTBOX,(FARPROC)AppAbout);
		    break;

		case MENU_EXIT:
                    PostMessage(hwnd,WM_CLOSE,0,0L);
                    break;

                case MENU_CLOSE:
                    PostMessage(hwndMCI, WM_CLOSE, 0, 0L);
                    break;

                case MENU_OPEN:
                    /* prompt user for file to open */
                    ofn.lStructSize = sizeof(OPENFILENAME);
                    ofn.hwndOwner = hwnd;
                    ofn.hInstance = NULL;
                    ofn.lpstrFilter = szOpenFilter;
                    ofn.lpstrCustomFilter = NULL;
                    ofn.nMaxCustFilter = 0;
                    ofn.nFilterIndex = 0;
                    ofn.lpstrFile = achFileName;
                    ofn.nMaxFile = sizeof(achFileName);
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.lpstrTitle = "Open";
                    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.nFileOffset = 0;
                    ofn.nFileExtension = 0;
                    ofn.lpstrDefExt = NULL;
                    ofn.lCustData = 0;
                    ofn.lpfnHook = NULL;
                    ofn.lpTemplateName = NULL;

                    if (GetOpenFileName(&ofn))
                    {
			MCIWndOpen(hwndMCI, (LPARAM)(LPSTR)achFileName, 0);
			InitPuzzle(&gPuzzle, TRUE /* FALSE to not scramble */);
                    }
                    break;

                /* Movie Menu */
                case IDM_PLAY:
                    MCIWndPlay(hwndMCI);
                    break;
                case IDM_RPLAY:
                    MCIWndPlayReverse(hwndMCI);
                    break;
                case IDM_STOP:
                    MCIWndStop(hwndMCI);
                    break;
                case IDM_HOME:
                    MCIWndHome(hwndMCI);
                    break;
                case IDM_END:
                    MCIWndEnd(hwndMCI);
                    break;
                case IDM_STEP:
                    MCIWndStep(hwndMCI, 1);
                    break;
                case IDM_RSTEP:
                    MCIWndStep(hwndMCI, -1);
                    break;

//
//	Start of interesting code:
//
//
//
//
//
		    
		// Solve or scramble the puzzle, as appropriate
		case IDM_SOLVE:
		case IDM_SCRAMBLE:
		    InitPuzzle(&gPuzzle, wParam == IDM_SCRAMBLE);
		    InvalidateRect(hwndMCI, NULL, FALSE);
		    break;
//
//	End of interesting code
//

	    }
            break;

        case WM_PALETTECHANGED:
        case WM_QUERYNEWPALETTE:
	case WM_ACTIVATE:
	    //
	    // Forward palette-related messages through to the MCIWnd,
	    // so it can do the right thing.
	    //
	    if (hwndMCI)
		return SendMessage(hwndMCI, msg, wParam, lParam);
	    break;

        case WM_INITMENUPOPUP:
	    //
	    // Check the styles properly when styles is chosen
	    // !!! Make sure position constants don't change!
	    //

	    //
	    // Enable/Disable the stuff under the MOVIE popup
	    // !!! Make sure position constants don't change!
	    //
	    if ((HMENU)wParam == GetSubMenu(GetMenu(hwnd), 1)) {

                EnableMenuItem((HMENU)wParam, 10,
		    MF_BYPOSITION | (hwndMCI ? MF_ENABLED : MF_GRAYED));
		
                if (!hwndMCI || MCIWndGetMode(hwndMCI, NULL, 0) ==
		    	    MCI_MODE_NOT_READY) {
		    f = hwndMCI ? MF_ENABLED : MF_GRAYED;
                    EnableMenuItem((HMENU)wParam, IDM_STOP, MF_GRAYED);
                    EnableMenuItem((HMENU)wParam, IDM_PLAY, MF_GRAYED);
                    EnableMenuItem((HMENU)wParam, IDM_RPLAY, MF_GRAYED);
                    EnableMenuItem((HMENU)wParam, IDM_HOME, MF_GRAYED);
                    EnableMenuItem((HMENU)wParam, IDM_END, MF_GRAYED);
                    EnableMenuItem((HMENU)wParam, IDM_STEP, MF_GRAYED);
                    EnableMenuItem((HMENU)wParam, IDM_RSTEP, MF_GRAYED);
                } else {
                   f = MCIWndGetMode(hwndMCI, NULL, 0) != MCI_MODE_STOP;
                   EnableMenuItem((HMENU)wParam, IDM_PLAY,
				!f ? MF_ENABLED : MF_GRAYED);
                   EnableMenuItem((HMENU)wParam, IDM_RPLAY,
				!f ? MF_ENABLED : MF_GRAYED);
                   EnableMenuItem((HMENU)wParam, IDM_STOP,
				 f ? MF_ENABLED : MF_GRAYED);
                   EnableMenuItem((HMENU)wParam, IDM_HOME, MF_ENABLED);
                   EnableMenuItem((HMENU)wParam, IDM_END,  MF_ENABLED);
                   EnableMenuItem((HMENU)wParam, IDM_STEP, MF_ENABLED);
                   EnableMenuItem((HMENU)wParam, IDM_RSTEP,MF_ENABLED);
               }
	    }

            break;

       case WM_CREATE:
            hwndMCI = MCIWndCreate(hwnd, hInstApp,
				   WS_CHILD | WS_VISIBLE |
				   MCIWNDF_NOTIFYMEDIA | MCIWNDF_NOTIFYSIZE |
				   MCIWNDF_NOMENU,
				   0);
            break;

       case WM_SIZE:
	    if (hwndMCI)
		MoveWindow(hwndMCI,0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
            break;

       case WM_DESTROY:
	    PostQuitMessage(0);
	    break;

       case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;

       case WM_PARENTNOTIFY:
	   if (wParam == WM_LBUTTONDOWN && hwndMCI) {
	       RECT	rc;
	       POINT	pt;
	       
	       pt.x = LOWORD(lParam);
	       pt.y = HIWORD(lParam);

	       ClientToScreen(hwnd, &pt);

	       MCIWndGetDest(hwndMCI, &rc);

	       if (rc.bottom != rc.top) {
		   ScreenToClient(hwndMCI, &pt);

		   ClickPuzzle(&gPuzzle,
			       (pt.x - rc.left) * PSIZE / (rc.right - rc.left),
			       (pt.y - rc.top) * PSIZE / (rc.bottom - rc.top));

		   InvalidateRect(hwndMCI, &rc, FALSE);
	       }
	   }
	   break;

       case MCIWNDM_NOTIFYMEDIA:
	   if (hwndMCI == 0)
	       hwndMCI = (HWND) wParam;

	   //
	   // Important note: the callback will be called using
	   // a different stack segment, so we have to call
	   // MakeProcInstance, and can't rely on "smart callbacks"!
	   //
	   if (!lpfnHook) {
	       // !!! we don't free this ever!
	       lpfnHook = MakeProcInstance((FARPROC) ICAVIDrawProc,
 							hInstApp);
	   }
	   dgv.dwValue = (DWORD) lpfnHook;
	       
	   dgv.dwItem = MCI_AVI_SETVIDEO_DRAW_PROCEDURE;

	   uDevice = MCIWndGetDeviceID(hwndMCI);
	   if (uDevice) {
	       dw = mciSendCommand(uDevice,
			       MCI_SETVIDEO,
			       MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
			       (DWORD) (MCI_DGV_SETVIDEO_PARMS FAR *)&dgv);

	       if (dw != 0) {
		   MessageBox(hwnd, 
			      "The currently installed MCIAVI does not "
			      "support the MCI_AVI_SETVIDEO_DRAW_PROCEDURE "
			      "command during play.","MCI Problem",
			      MB_OK | MB_ICONHAND);
	       }
	   }

	   break;
	   
       case MCIWNDM_NOTIFYSIZE:
	   if (hwndMCI == 0)
	       hwndMCI = (HWND) wParam;
	   
	   GetWindowRect(hwndMCI, &rc);
	   AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), TRUE);
	   rc.bottom++;	// AdjustWindowRect is broken.
	   SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left,
                    rc.bottom - rc.top,
                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
	   break;
    }
    return DefWindowProc(hwnd,msg,wParam,lParam);
}

/*----------------------------------------------------------------------------*\
|   ErrMsg - Opens a Message box with a error message in it.  The user can     |
|	     select the OK button to continue or the CANCEL button to kill     |
|	     the parent application.					       |
\*----------------------------------------------------------------------------*/
int ErrMsg (LPSTR sz,...)
{
    char ach[128];
    wvsprintf(ach,sz,(LPSTR)(&sz+1));   /* Format the string */
    MessageBox (NULL,ach,NULL,MB_OK|MB_ICONEXCLAMATION);
    return FALSE;
}
