/*--------------------------------------------------------------------------
  DIALOGS.CPP - dialog functions for the C++ demonstration of ODBC classes

  This code is furnished on an as-is basis as part of the ODBC SDK and is
  intended for example purposes only.

--------------------------------------------------------------------------*/
      
//////////////////////////////////////////////////////////////////////////////
//
//	Includes
//
#include	"headers.h"

#include	"resource.h"
#include	"cppdemo.h"


//////////////////////////////////////////////////////////////////////////////
//
//	Defines
//


//////////////////////////////////////////////////////////////////////////////
//
//	Constants
//


//////////////////////////////////////////////////////////////////////////////
//
//	Types
//
        
        
//////////////////////////////////////////////////////////////////////////////
//
//	Globals
//


//////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
void		INTFUNC	CenterWindows(HWND);
BOOL		INTFUNC	DefaultDlgProc(HWND, UINT, WPARAM, LPARAM);


//////////////////////////////////////////////////////////////////////////////
//
//  CenterWindow
//
//      Center a window inside its parent.
//
//  Params:
//      hwnd        -- handle to the window to center
//
//  Returns:
//      none
//
void INTFUNC CenterWindow(HWND hwnd)
    {
    HWND    hwndParent;             // parent window handle
    RECT    rcWnd,                  // window rectangle
            rcScr,                  // screen rectangle
            rcParent;               // parent rectangle
    int     cx, cy;                 // window dimensions

        // Get the window's parent

    hwndParent = GetParent(hwnd);
    if( !hwndParent )
        hwndParent = GetDesktopWindow();
	
    	// Get the dimensions of the window we're centering
    	
	GetWindowRect(hwnd, &rcWnd);
	cx = rcWnd.right  - rcWnd.left;
	cy = rcWnd.bottom - rcWnd.top;

		// Center it inside its parent
		
    GetWindowRect(hwndParent, &rcParent);
	rcWnd.top    = rcParent.top +
					(((rcParent.bottom - rcParent.top) - cy) >> 1);
	rcWnd.left   = rcParent.left + 
					(((rcParent.right - rcParent.left) - cx) >> 1);
	rcWnd.bottom = rcWnd.top  + cy;
	rcWnd.right  = rcWnd.left + cx;
                                      
		// Make sure that it is completely on the screen,
		// centered or not, it's more important to see it!
		
	GetWindowRect(GetDesktopWindow(), &rcScr);
	if( rcWnd.bottom > rcScr.bottom )
		{
		rcWnd.bottom = rcScr.bottom;
		rcWnd.top    = rcWnd.bottom - cy;
		}
	if( rcWnd.right  > rcScr.right )
		{
		rcWnd.right = rcScr.right;
		rcWnd.left  = rcWnd.right - cx;
		}

	if( rcWnd.left < 0 )
		rcWnd.left = 0;
	if( rcWnd.top  < 0 )
		rcWnd.top  = 0;

		// Move the window to its new position
		
	MoveWindow(hwnd, rcWnd.left, rcWnd.top, cx, cy, TRUE);
	return;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	DefaultDlgProc
//
//      Handle processing common to all dialog boxes.
//
//  Params:
//      hdlg        -- handle to the dialog box
//      wmsg        -- message ID
//      wParam      -- word parameter
//      lParam      -- long parameter
//
//  Returns:
//      BOOL        -- FALSE if DefDlgProc should handle message
//
BOOL INTFUNC DefaultDlgProc(HWND hdlg,
							UINT wmsg,
							WPARAM wParam,
							LPARAM lParam )
	{
    LPAPPINST   lpInst;             // instance information
	
		// Get the instance-specific information
	
	lpInst = (LPAPPINST)GetWindowLong(hdlg, DWL_USER);
	
	switch( wmsg )
		{
		case WM_INITDIALOG:
			CenterWindow(hdlg);
#ifndef _MAC
			if( lpInst->fCtl3d && !lpInst->fAutoCtl3d )
				Ctl3dSubclassDlg(hdlg, CTL3D_ALL);
#endif
			
				// Let DefDlgProc pick the default control
			 
			return TRUE;

		case WM_CTLCOLORBTN:
		case WM_CTLCOLORDLG:
		case WM_CTLCOLOREDIT:
		case WM_CTLCOLORLISTBOX:
		case WM_CTLCOLORMSGBOX:
		case WM_CTLCOLORSCROLLBAR:
		case WM_CTLCOLORSTATIC:
#ifndef _MAC
			if( lpInst->fCtl3d && !lpInst->fAutoCtl3d ) 
				return (BOOL)Ctl3dCtlColorEx(wmsg, wParam, lParam);
#endif
			break;

		case WM_SYSCOLORCHANGE:
#ifndef _MAC
			if( lpInst->fCtl3d && !lpInst->fAutoCtl3d ) 
				return Ctl3dColorChange();
#endif
			break;

		case WM_NCACTIVATE:
		case WM_NCPAINT:
		case WM_SETTEXT:
#ifndef _MAC
			if( lpInst->fCtl3d && !lpInst->fAutoCtl3d )
				{
				SetWindowLong(hdlg, DWL_MSGRESULT,
							  Ctl3dDlgFramePaint(hdlg, wmsg, wParam, lParam));
				return TRUE;
				}
#endif
			break;

		case WM_COMMAND:
			if( wParam == IDOK || wParam == IDCANCEL )
				{
				EndDialog(hdlg, wParam);
				return TRUE;
				}
			break;	
   		}
   		
	return FALSE;
	}
	
	
	
//////////////////////////////////////////////////////////////////////////////
//
//  AboutDlgProc
//
//      Processing for the about dialog box.
//
//  Params:
//      hdlg        -- handle to the dialog box
//      wmsg        -- message ID
//      wParam      -- word parameter
//      lParam      -- long parameter
//
//  Returns:
//      BOOL        -- FALSE if DefDlgProc should handle message
//
BOOL EXPFUNC AboutDlgProc(	HWND hdlg,
							UINT wmsg,
							WPARAM wParam,
							LPARAM lParam )
	{
    char    szSpace[15];            // buffer for free memory display
	
	switch( wmsg )
		{
		case WM_INITDIALOG:
			{
            DWORD           dwFree;
            MEMORYSTATUS    mst;
			
				// Display the current free memory in KB

            mst.dwLength = sizeof(mst);
            GlobalMemoryStatus(&mst);
            dwFree = mst.dwAvailVirtual / 1024;
			if( dwFree < 1000000L )
				wsprintf(szSpace, "%ld KB", dwFree);
			else
				wsprintf(szSpace, "%ld MB", dwFree / 1024); 
			SetDlgItemText(hdlg, IDC_STATUS1, szSpace);
			
				// Store the LPAPPINST pointer
			
			SetWindowLong(hdlg, DWL_USER, lParam);
			}
			break;
   		}
   		
	return DefaultDlgProc(hdlg, wmsg, wParam, lParam);
	}
	
	
//////////////////////////////////////////////////////////////////////////////
//
//  ConfirmDlgProc
//
//      Dialog procedure for the confirmation box; this will appear whenever
//      the user selects an action that will close the current connection.
//
//  Params:
//      hdlg        -- handle to the dialog box
//      wmsg        -- message ID
//      wParam      -- word parameter
//      lParam      -- long parameter
//
//  Returns:
//      BOOL        -- FALSE if DefDlgProc should handle message
//
BOOL EXPFUNC ConfirmDlgProc(HWND hdlg,
							UINT wmsg,
							WPARAM wParam,
							LPARAM lParam )
	{
    LPAPPINST   lpInst;             // instance information
    char        szConnect[SQL_MAX_DSN_LENGTH+3];    // DSN string
    LPSTR       lpsz;               // pointer for loop
    RECT        rc;                 // status box rectangle
    WORD        wWidth,             // DSN string width in pixels
                wHalf,              // index of DSN string middle
                i;                  // loop counter
    HWND        hwnd;               // handle to status box
    HDC         hdc;                // HDC for determining DSN width
    SIZE        size;               // GetTextExtentPoint return structure
	
	switch( wmsg )
		{
		case WM_INITDIALOG:
			{
				// Store the LPAPPINST pointer
				
			SetWindowLong(hdlg, DWL_USER, lParam);
			lpInst = (LPAPPINST)lParam;
			
				// Split the data source name in half if it's too long
				// for the given status box..
			
			hdc = GetDC(NULL);
			GetTextExtentPoint(hdc, lpInst->szDSN,
										  lstrlen(lpInst->szDSN), &size);
			wWidth = (WORD)size.cx;
			ReleaseDC(NULL, hdc);
				
			hwnd = GetDlgItem(hdlg, IDC_STATUS1);
			GetWindowRect(hwnd, &rc);
			
			if( wWidth > (WORD)(rc.right - rc.left) )
				{
				wHalf = lstrlen(lpInst->szDSN) >> 1;
				lpsz = lpInst->szDSN;
				for( i = 0; i < wHalf; i++ )
					szConnect[i] = *lpsz++;
				szConnect[i++] = '\n';
				while( *lpsz )
					szConnect[i++] = *lpsz++;
				szConnect[i] = '\0';
				}
			else
				lstrcpy(szConnect, lpInst->szDSN);
				
				// Place the name in the status field of the dialog
				
			SetDlgItemText(hdlg, IDC_STATUS1, szConnect);
			}
			break;
   		}
   		
	return DefaultDlgProc(hdlg, wmsg, wParam, lParam);
	}
	
	
//////////////////////////////////////////////////////////////////////////////
//
//  OkDlgProc
//
//      Dialog procedure for the informational 'ok' dialog box; this dialog
//      is displayed after successful execution of an SQL statement that
//      does not return a result set, by design.
//
//  Params:
//      hdlg        -- handle to the dialog box
//      wmsg        -- message ID
//      wParam      -- word parameter
//      lParam      -- long parameter
//
//  Returns:
//      BOOL        -- FALSE if DefDlgProc should handle message
//
BOOL EXPFUNC OkDlgProc(	HWND hdlg,
						UINT wmsg,
						WPARAM wParam,
						LPARAM lParam )
	{
    LPAPPINST       lpInst;         // instance information
	
	switch( wmsg )
		{
		case WM_INITDIALOG:
		
				// Store the LPAPPINST pointer
				
			SetWindowLong(hdlg, DWL_USER, lParam);
			lpInst = (LPAPPINST)lParam;
			
				// Set the status text to show the executed SQL statement
			
			SetDlgItemText(hdlg, IDC_STATUS1, lpInst->szSQL);
			break;
   		}
   		
	return DefaultDlgProc(hdlg, wmsg, wParam, lParam);
	}
	
	
//////////////////////////////////////////////////////////////////////////////
//
//  SQLDlgProc
//
//      Dialog procedure for SQL statement dialog box; this dialog box allows
//      the user to enter and edit a SQL statement.
//
//  Params:
//      hdlg        -- handle to the dialog box
//      wmsg        -- message ID
//      wParam      -- word parameter
//      lParam      -- long parameter
//
//  Returns:
//      BOOL        -- FALSE if DefDlgProc should handle message
//
BOOL EXPFUNC SQLDlgProc(HWND hdlg,
						UINT wmsg,
						WPARAM wParam,
						LPARAM lParam )
	{
    LPAPPINST       lpInst;         // instance information
	
	if( wmsg != WM_INITDIALOG )
		lpInst = (LPAPPINST)GetWindowLong(hdlg, DWL_USER);
	
	switch( wmsg )
		{
		case WM_INITDIALOG:

				// Store the LPAPPINST pointer
				
			SetWindowLong(hdlg, DWL_USER, lParam);
			lpInst = (LPAPPINST)lParam;
			
			SetDlgItemText(hdlg, IDC_EDIT1, lpInst->szSQL);
			SendDlgItemMessage(hdlg, IDC_EDIT1, EM_LIMITTEXT,
							   cbMAXSQL - 1, 0L);
							   
			break;
			
		case WM_COMMAND:
			switch( wParam )
				{
				case IDOK:
					GetDlgItemText(hdlg, IDC_EDIT1, lpInst->szSQL, cbMAXSQL);
					// fall through
					
				case IDCANCEL:
					EndDialog(hdlg, wParam);
					return TRUE;
				}
			break;
		}
		
	return DefaultDlgProc(hdlg, wmsg, wParam, lParam);
	}
