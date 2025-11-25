/*--------------------------------------------------------------------------
  CPPDEMO.CPP - C++ demonstration of the ODBC SDK low level C++ classes

  This code is furnished on an as-is basis as part of the ODBC SDK and is
  intended for example purposes only.

--------------------------------------------------------------------------*/
      
//////////////////////////////////////////////////////////////////////////////
//
//	Includes
//
#include	"headers.h"
                    
	// Attach globals to this file
	              
#define		INCL_GLOBAL
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
typedef struct tagErrorText
    {
    char        szText[1024];
    char        szFmt[cbSTRLEN];
    char        szSQLState[6];
    char        szError[SQL_MAX_MESSAGE_LENGTH];
    }
    ERRORTEXT, FAR *LPERRORTEXT;
        
        
//////////////////////////////////////////////////////////////////////////////
//
//	Globals
//


//////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
BOOL	INTFUNC	fConnect(HWND);
BOOL	INTFUNC	fDisconnect(HWND);
void	INTFUNC	vDoSQL(HWND);
void	INTFUNC vEnableMenus(HWND);
void	INTFUNC	vFetch(HWND);
void	INTFUNC	vFreeStmt(HWND, UWORD);
void	INTFUNC vPaintWindow(HWND, PAINTSTRUCT);
void	INTFUNC	vSetScroll(HWND);
void	INTFUNC	vSizeScroll(HWND);
int		PASCAL	WinMain(HINSTANCE, HINSTANCE, LPSTR, int);


//////////////////////////////////////////////////////////////////////////////
//
//	fConnect
//
//      Open a connection to a data source.  Assumes that there is currently
//      no connection.
//
//  Params:
//      hwnd        -- window handle
//
//  Returns:
//      BOOL        -- TRUE if connection succeeded, FALSE otherwise
//
BOOL INTFUNC fConnect(HWND hwnd)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    RETCODE     rc;                 // SQL return code
    char        sz[cbMAXSQL];       // DSN string
    HCURSOR     hCurs;              // cursor handle
    SWORD       swInfoValue;        // total # of bytes available to return
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
		// If we're already connected, that's bad
		
	DASSERT(!lpInst->fConnected);
	
		// Allocate a new connection

	HOURGLASS(hCurs);								
	lpInst->cDbc = lpInst->cEnv->AllocConnect();
	ARROW(hCurs);
	
		// If that's not successful, return failure
		
	if( !lpInst->cDbc )
		{
		vDoMessage(hwnd, IDS_NOMEMORY);
		return FALSE;
		}
	else if( fODBCError(hwnd, lpInst->cDbc->m_rc) )
		{
		delete lpInst->cDbc;
		lpInst->cDbc = NULL;
		return FALSE;	
		}
	
		// Don't use the cursor library for this connection, since there's no
		// need for it; make the connection to a user-chosen data source
		
	HOURGLASS(hCurs);
	fODBCError(hwnd, lpInst->cDbc->SetConnectOption(SQL_ODBC_CURSORS,
                                                    SQL_CUR_USE_DRIVER));
	rc = lpInst->cDbc->DriverConnect(hwnd, NULL, sz, sizeof(sz),
                                     SQL_DRIVER_COMPLETE);
	ARROW(hCurs);
		
		// Check for errors in making the connection
					 
	if( rc == SQL_NO_DATA_FOUND || fODBCError(hwnd, rc) )
		{
		delete lpInst->cDbc;
		lpInst->cDbc = NULL;
		return FALSE;
		}

		// Get the data source name

    *lpInst->szDSN = '\0';
    fODBCError(hwnd, lpInst->cDbc->GetInfo(SQL_DATA_SOURCE_NAME,
                                           lpInst->szDSN,
                                           SQL_MAX_DSN_LENGTH + 1,
                                           &swInfoValue));

    if( !(*lpInst->szDSN) )
        lstrcpy(lpInst->szDSN, szNODSN);
		
		// Allocate a statement
		
	HOURGLASS(hCurs);
	lpInst->cStmt = lpInst->cDbc->AllocStmt();
	ARROW(hCurs);
	
		// See if there was a problem with the statement allocation..
		
	if( !lpInst->cStmt )
		{
		delete lpInst->cDbc;
		lpInst->cDbc = NULL;
		vDoMessage(hwnd, IDS_NOMEMORY);
		return FALSE;
		}
	else if( fODBCError(hwnd, lpInst->cStmt->m_rc) )
		{
		delete lpInst->cDbc;
		lpInst->cDbc = NULL;
		delete lpInst->cStmt;
		lpInst->cStmt = NULL;
		return FALSE;
		}
		
		// Finally, the connection is good at this point
		
	lpInst->fConnected = TRUE;
	
		// Add the DSN to the window title
		
	lstrcpy(lpInst->szTitle, lpGlob->szTitle);
	lstrcat(lpInst->szTitle, szDASH);
	lstrcat(lpInst->szTitle, lpInst->szDSN);
	SetWindowText(hwnd, lpInst->szTitle);
	
	return TRUE;
	}	


//////////////////////////////////////////////////////////////////////////////
//
//	fDisconnect
//
//      Close a connection to a data source.
//
//  Params:
//      hwnd        -- window handle
//
//  Returns:
//      BOOL        -- TRUE if the disconnect succeeded, FALSE otherwise
//
BOOL INTFUNC fDisconnect(HWND hwnd)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    HCURSOR     hCurs;              // cursor handle
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
		// If we're not connected, then we're now disconnected :)
		
	if( !lpInst->fConnected )
		return TRUE;
		
        // Release the associated statement resources if we've
		// not already done so..
		
	if( lpInst->cStmt )
		{
		vFreeStmt(hwnd, SQL_DROP);
		delete lpInst->cStmt;
		lpInst->cStmt = NULL;
		}
		
		// Disconnect..

	HOURGLASS(hCurs);
	if( !fODBCError(hwnd, lpInst->cDbc->Disconnect()) )
		{
		delete lpInst->cDbc;
		lpInst->cDbc = NULL;
		}
	else
		{
		ARROW(hCurs);
		vEnableMenus(hwnd);
		return FALSE;
		}
	ARROW(hCurs);
	
		// Flag the connection closed and remove the DSN from the
		// Window title bar display
		
	lpInst->fConnected = FALSE;
	lpInst->szDSN[0] = '\0';
	
	lstrcpy(lpInst->szTitle, lpGlob->szTitle);
	SetWindowText(hwnd, lpInst->szTitle);
	
	return TRUE;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vDoMessage
//
//      Put up an informational message box.
//
//  Params:
//      hwnd        -- window handle
//      id          -- string resource ID
//
//  Returns:
//      none
//
void INTFUNC vDoMessage(HWND hwnd, UINT id)
	{
	LPAPPGLOB	lpGlob;
	LPAPPINST	lpInst;
	char		sz[cbSTRLEN];
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
	LoadString(lpInst->hinst, id, sz, sizeof(sz));
	MessageBox(hwnd, sz, lpGlob->szTitle, MB_ICONINFORMATION | MB_OK);
	
	return;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vDoSQL
//
//      Execute a SQL statement and, if there is a result set from this
//      execution, make bindings, allocate storage, and do the first data
//      fetch.
//
//  Params:
//      hwnd        -- window handle
//
//  Returns:
//      none
//
void INTFUNC vDoSQL(HWND hwnd)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    HCURSOR     hCurs;              // cursor handle
    SWORD       cCol;               // columns in result set
    UWORD       i;                  // loop counter
    LPCOL       lpcol;              // column information
	
	HOURGLASS(hCurs);
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
		// If there's already an outstanding result set, unbind it
		
	if( lpInst->fResultSet )
		vFreeStmt(hwnd, SQL_UNBIND);
	
		// Execute the SQL statement with SQLExecDirect
	
	if( fODBCError(hwnd, lpInst->cStmt->ExecDirect(lpInst->szSQL,
												   lstrlen(lpInst->szSQL))) )
		{
		ARROW(hCurs);
		return;
		}
		
		// Determine the number of columns in the result set

	cCol = 0;		
	if( fODBCError(hwnd, lpInst->cStmt->NumResultCols(&cCol)) )
		{
		ARROW(hCurs);
		return;
		}
		
		// If there's a row set, we've got data. Otherwise the operation was
		// successful without returning any data (probably not a SELECT,
		// therefore!); display a dialog to indicate this success
		
	lpInst->cCol = cCol;
	if( lpInst->cCol )
		lpInst->fResultSet = TRUE;
	else
		{
        DialogBoxParam(lpInst->hinst, MAKEINTRESOURCE(OKBOX),
                       hwnd, DLGPROC(OkDlgProc),
                       (LONG)lpInst);
		ARROW(hCurs);
		return;
		}
		
		// Allocate column- and row-related storage
		
	lpcol = lpInst->lpcol	= (LPCOL  )AllocPtr(sizeof(COL  ) *
												lpInst->cCol);
	if( !lpcol )
		{
		vDoMessage(hwnd, IDS_NOMEMORY);
		vFreeStmt(hwnd, SQL_DROP);
		ARROW(hCurs);
		return;
		}
	
		// For each bound column, get the column attributes
		
	for( i = 1; i <= lpInst->cCol; i++ )
		{
			// Get column name
			
		if( fODBCError(hwnd,
					   lpInst->cStmt->ColAttributes(i, SQL_COLUMN_NAME,
													lpcol->szName,
													sizeof(lpcol->szName),
											  		NULL)) )
			{
			vFreeStmt(hwnd, SQL_DROP);
			ARROW(hCurs);
			return;
			}
			
			// Get actual column length
			
		if( fODBCError(hwnd, lpInst->cStmt->ColAttributes(i,
														  SQL_COLUMN_LENGTH,
														  &lpcol->cb)) )
			{
			vFreeStmt(hwnd, SQL_DROP);
			ARROW(hCurs);
			return;
			}
			
			// Get display width
			
		if( fODBCError(hwnd,
					   lpInst->cStmt->ColAttributes(i,
					   								SQL_COLUMN_DISPLAY_SIZE,
													&lpcol->cbc)) )
			{
			vFreeStmt(hwnd, SQL_DROP);
			ARROW(hCurs);
			return;
			}
			
			// All columns need to be at least long enough for the column name
			// and long enough to hold the <NULL> string, just in case
			
		if( lstrlen(lpcol->szName) > lpcol->cbc )
			lpcol->cbc = lstrlen(lpcol->szName);
		if( lstrlen(lpGlob->szNull) > lpcol->cbc )
			lpcol->cbc = lstrlen(lpGlob->szNull);
		
			// Get column SQL data type
			
		if( fODBCError(hwnd, lpInst->cStmt->ColAttributes(i, SQL_COLUMN_TYPE,
														  &lpcol->fSqlType)) )
			{
			vFreeStmt(hwnd, SQL_DROP);
			ARROW(hCurs);
			return;
			}
			
			// If the SQL data type is a CHAR form, make sure that the
			// display width at least as large as the transfer width
			
		if( (lpcol->fSqlType == SQL_CHAR ||
			 lpcol->fSqlType == SQL_VARCHAR ||
			 lpcol->fSqlType == SQL_LONGVARCHAR) &&
				lpcol->cbc < lpcol->cb )
			lpcol->cbc = lpcol->cb;
		
			// Since we just want to see the data, let the driver
			// convert it to SQL_C_CHAR format for us
			
		lpcol->fCType	= SQL_C_CHAR;
		
			// Change the transfer length to be the same length as the display
			// width and increase the storage size by one to hold the final \0
			// if this can be done without truncation
			
			// Otherwise, adjust the size of the transfers so that memory
			// allocations can be done in cbCOLMEMMAX blocks
		
		lpcol->cb = lpcol->cbc;
		if( lpcol->cb > cbCOLMEMMAX
                || lpcol->cb * lpInst->cMaxRow > cbCOLMEMMAX )
			{
			lpcol->cb	= (cbCOLMEMMAX / lpInst->cMaxRow) - 1;
			lpcol->cbc	= lpcol->cb;
			}
		else
			lpcol->cb++;
			
			// Round transfer width to even byte boundaries
			
		lpcol->cb = (lpcol->cb + 3) & 0xFFFFFFFC;
		
			// Increment the storage and display width of the row
			
		lpInst->cbrow += (UWORD)lpcol->cb;
		lpInst->ccols += (UWORD)lpcol->cbc;
		
			// Go to the next element in the lpcol array
			
		lpcol++;
		}
		
		// Bind the data value for each column

	lpcol = lpInst->lpcol;
	for( i = 1; i <= lpInst->cCol; i++ )
		{
			// Allocate the memory for the column bindings
		
		lpcol->lpb    = (LPBYTE)AllocPtr(lpcol->cb);
		lpcol->lpcb	  = (LPSDWORD)AllocPtr(sizeof(SDWORD));
		lpcol->lpbuf  = (LPBYTE)AllocPtr(lpcol->cb * lpInst->cMaxRow);
		lpcol->lpcbuf = (LPSDWORD)AllocPtr(sizeof(SDWORD) * lpInst->cMaxRow);
		if( lpcol->lpb		== NULL || lpcol->lpcb		== NULL ||
			lpcol->lpbuf	== NULL || lpcol->lpcbuf	== NULL )
			{
			vDoMessage(hwnd, IDS_NOMEMORY);
			vFreeStmt(hwnd, SQL_DROP);
			ARROW(hCurs);
			return;
			}
		
			// Bind the column
		
		if( fODBCError(hwnd, lpInst->cStmt->BindCol(i, (SWORD)lpcol->fCType,
													(PTR)lpcol->lpb,
													lpcol->cb,
													lpcol->lpcb)) )
			{
			vFreeStmt(hwnd, SQL_UNBIND);
			ARROW(hCurs);
			return;
			}

			// Advance to next column entry in array
			
		lpcol++;
		}
		
		// Do the first fetch
	
	vFetch(hwnd);

	ARROW(hCurs);
	return;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vEnableMenus
//
//      Enable all connection-related menu items.
//
//  Params:
//      hwnd        -- window handle
//
//  Returns:
//      none
//
void INTFUNC vEnableMenus(HWND hwnd)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    HMENU       hmenu;              // menu handle
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
	hmenu = GetMenu(hwnd);
	
	if( lpInst && lpInst->fConnected )
		{
	    EnableMenuItem(hmenu, ID_ENV_CLOSE, 	MF_ENABLE);
	    EnableMenuItem(hmenu, ID_SQL_EXEC,		MF_ENABLE);
	    if( lpInst->fResultSet && lpInst->cRow == lpInst->cMaxRow )
	    	EnableMenuItem(hmenu, ID_SQL_FETCH,	MF_ENABLE);
	    else
	    	EnableMenuItem(hmenu, ID_SQL_FETCH,	MF_DISABLE);
		}
	else
		{
		EnableMenuItem(hmenu, ID_ENV_CLOSE, 	MF_DISABLE);
		EnableMenuItem(hmenu, ID_SQL_EXEC,		MF_DISABLE);
		EnableMenuItem(hmenu, ID_SQL_FETCH,		MF_DISABLE);
		}
		
	if( lpInst && lpInst->fConnected && !lpInst->cStmt )
		{
		EnableMenuItem(hmenu, ID_SQL_EXEC,		MF_DISABLE);
		EnableMenuItem(hmenu, ID_SQL_FETCH,		MF_DISABLE);
		}
		
	return;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vFetch
//
//      Retrieve rows into the space allocated for this result set.
//
//  Params:
//      hwnd        -- window handle
//
//  Returns:
//      none
//
void INTFUNC vFetch(HWND hwnd)
	{                              
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    RETCODE     rc;                 // SQL return code
    HCURSOR     hCurs;              // cursor handle
    WORD        nRow, nCol;         // loop counters
    LPCOL       lpcol;              // column information
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
		// Fetch the data
		
	HOURGLASS(hCurs);
	for( nRow = 0; nRow < lpInst->cMaxRow; nRow++ )
		{
			// Get a row of data
			
		rc = lpInst->cStmt->Fetch();
		if( rc == SQL_NO_DATA_FOUND || fODBCError(hwnd, rc) )
			break;
			
			// Copy the row from the transfer buffer to
			// the aggregate data storage buffer
			
		lpcol = lpInst->lpcol;
		for( nCol = 0; nCol < lpInst->cCol; nCol++ )
			{
			lstrcpy((LPSTR)(lpcol->lpbuf + nRow * lpcol->cb),
					(LPSTR)lpcol->lpb);
			*(LPSDWORD)(lpcol->lpcbuf + nRow) = *(LPSDWORD)lpcol->lpcb;
					
			lpcol++;
			}
		}
	ARROW(hCurs);

		// Did we get any data?
	
	if( !nRow )
		lpInst->fData = FALSE;
	else
		lpInst->fData = TRUE;	
		
		// Save the number of rows fetched
	
	lpInst->cRow = nRow;
		
	return;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vFreeStmt
//
//		Release memory and resources at the same time that we issue
//      SQLFreeStmt calls.
//
//  Params:
//      hwnd        -- window handle
//      fOption     -- option flag, either SQL_DROP or SQL_UNBIND
//
//  Returns:
//      none
//
void INTFUNC vFreeStmt(HWND hwnd, UWORD fOption)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    UWORD       i;                  // loop counter
    LPCOL       lpcol;              // column information
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
	if( !lpInst->cStmt )                
		return;
		
		// Issue the SQLFreeStmt call, unless fOption == SQL_DROP, since
		// SQL_DROP will be handled by the destructor for the CSTMT class;
		// in addition, we'll always close the current cursor so that we
		// can be assured of having a clean operation next time
		
	if( fODBCError(hwnd, lpInst->cStmt->Close()) )
		return;
	if( fOption != SQL_DROP )
		if( fODBCError(hwnd, lpInst->cStmt->FreeStmt(fOption)) )
			return;
			
		// Release memory
		
	lpcol = lpInst->lpcol;
	for( i = 0; i < lpInst->cCol; i++ )
		{
		FreePtr(lpcol->lpb);
		lpcol->lpb = NULL;
		FreePtr(lpcol->lpcb);
		lpcol->lpcb = NULL;
		FreePtr(lpcol->lpbuf);
		lpcol->lpbuf = NULL;
		FreePtr(lpcol->lpcbuf);
		lpcol->lpcbuf = NULL;
		
		lpcol++;
		}
		
	FreePtr(lpInst->lpcol);
	lpInst->lpcol = NULL;
	
		// Set default values
	
	lpInst->fData		= FALSE;
	lpInst->fResultSet	= FALSE;
	lpInst->cCol		= 0;
	lpInst->cRow		= 0;
	lpInst->ccols		= 0;
	lpInst->cbrow		= 0;
	lpInst->ccolwin		= 0;
	lpInst->crowwin		= 0;
	
        // Reset the scroll bars if necessary

	if( lpInst->fVScroll )
		{
		SetScrollRange(lpInst->hwndVScroll, SB_CTL, 0, 0, FALSE);
		SetScrollPos(lpInst->hwndVScroll, SB_CTL, 0, FALSE);
		ShowWindow(lpInst->hwndVScroll, SW_HIDE);
		lpInst->fVScroll = FALSE;
		}
		
	if( lpInst->fHScroll )
		{
		SetScrollRange(lpInst->hwndHScroll, SB_CTL, 0, 0, FALSE);
		SetScrollPos(lpInst->hwndHScroll, SB_CTL, 0, FALSE);
		ShowWindow(lpInst->hwndHScroll, SW_HIDE);
		lpInst->fHScroll = FALSE;
		}		

		// Invalidate the window so that it'll all get redrawn
		
	InvalidateRect(hwnd, NULL, FALSE);

		// Update the window now
		
	UpdateWindow(hwnd);
	
	return;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	fODBCError
//
//		Check for an ODBC error; SQL_NO_DATA_FOUND and SQL_STILL_EXECUTING
//		are not considered errors.  If an error is found, bring up a message
//      box detailing the error.  In the case of SQL_SUCCESS_WITH_INFO,
//      a message box with the additional information is shown, but this is
//      also not considered an error.
//
//  Params:
//      hwnd        -- window handle
//      rc          -- SQL return code to check
//
//  Returns:
//      BOOL        -- TRUE if there was an ODBC error, FALSE otherwise
//
BOOL INTFUNC fODBCError(HWND hwnd, RETCODE rc)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    SDWORD      fNative;            // native error code
    SWORD       cbError;            // length of returned error string
    LPERRORTEXT lpErr;              // error text structure
    HENV        henv;               // environment handle
    HDBC        hdbc;               // connection handle
    HSTMT       hstmt;              // statement handle
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
    lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));

        // Check for non-errors first
	
	if( rc == SQL_SUCCESS )
		return FALSE;
		
	if( rc == SQL_NO_DATA_FOUND )
		{
		vDoMessage(hwnd, IDS_NODATAFOUND);
		return FALSE;
		}
		
	if( rc == SQL_STILL_EXECUTING )
		{
		vDoMessage(hwnd, IDS_STILLEXECUTING);
		return FALSE;
		}            
		
		// Allocate storage
		
	lpErr = (LPERRORTEXT)AllocPtr(sizeof(ERRORTEXT));
	if( lpErr == NULL )
		{
		vDoMessage(hwnd, IDS_NOMEMORY);
		return TRUE;
		}
	
	LoadString(lpInst->hinst, IDS_MSGFMT, lpErr->szFmt, cbSTRLEN);
	
        // Retrieve and display messages until they're all gone
		       
	henv	= lpInst->cEnv	? HENV(*(lpInst->cEnv))		: SQL_NULL_HENV;
	hdbc	= lpInst->cDbc	? HDBC(*(lpInst->cDbc))		: SQL_NULL_HDBC;
	hstmt	= lpInst->cStmt	? HSTMT(*(lpInst->cStmt))	: SQL_NULL_HSTMT;
	while( SQLError(henv, hdbc, hstmt,
					(UCHAR FAR*)lpErr->szSQLState,
					&fNative,
					(UCHAR FAR*)lpErr->szError,
					SQL_MAX_MESSAGE_LENGTH - 1,
					&cbError) != SQL_NO_DATA_FOUND )
		if( lstrcmpi(lpErr->szSQLState, szDATATRUNC) )
			{
			wsprintf(lpErr->szText, lpErr->szFmt, lpErr->szSQLState,
                     fNative, lpErr->szError);
            MessageBox(hwnd, lpErr->szText, lpGlob->szTitle,
                        rc == SQL_SUCCESS_WITH_INFO ?
                        MB_ICONINFORMATION | MB_OK :
                        MB_ICONEXCLAMATION | MB_OK);
			}
	
		// Release storage
		
	FreePtr(lpErr);
	
    return (rc != SQL_SUCCESS_WITH_INFO);
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vPaintBox
//
//      Paint the box between scroll bars if required.
//
//  Params:
//      hdc     -- device context handle
//      hwnd    -- window handle
//
//  Returns:
//      none
//
void INTFUNC vPaintBox(HDC hdc, HWND hwnd)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    HRGN        hrgn;               // clipping region handle
    HBRUSH      hbrushOld;          // old brush handle
    RECT        rc;                 // client rectangle

		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
		// Get the client rectangle

	GetClientRect(hwnd, &rc);

		// Include the entire box in the clipping region

	hrgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	SelectClipRgn(hdc, hrgn);

		// Select the scrollbar brush and draw the box

	hbrushOld = (HBRUSH)SelectObject(hdc, lpGlob->hbrScroll);
	PatBlt(hdc,
		   rc.right  - (lpGlob->cxVScroll - 1),
		   rc.bottom - (lpGlob->cyHScroll - 1),
		   lpGlob->cxVScroll, lpGlob->cyHScroll,
		   PATCOPY);

		// Restore the device context
				   
	SelectObject(hdc, hbrushOld);
	DeleteObject(hrgn);
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vPaintWindow
//
//      Paint the main window.
//
//  Params:
//      hwnd        -- window handle
//      ps          -- paint structure
//
//  Returns:
//      none
//
void INTFUNC vPaintWindow(HWND hwnd, PAINTSTRUCT ps)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    RECT        rc;                 // general-purpose rectangle
    HDC         hdc;                // device context handle
    HFONT       hfontOld;           // old font handle
    HBRUSH      hbrushOld;          // old brush handle
    LPCOL       lpcol;              // column information
    UINT        cSkip,              // columns skipped width
                uFirstCol,          // first column to paint
                uLastCol,           // last column to paint
                uFirstRow,          // first row to paint
                uLastRow,           // last row to paint
                uCol, uRow;         // loop counters
    int         left, right,        // edges of current column
                row, col,           // characters shifted by scroll position
                cx, cy;             // pixels shifted by scroll position
    BOOL        fPaintBox;          // box between scrollbars needs painting
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
		// Extract the hdc from the paintstruct
	
    hdc = ps.hdc;

	if( !lpInst->fResultSet )
		FillRect(hdc, &ps.rcPaint, lpGlob->hbrWin);
	else
		{
			// Get the client rectangle
			
		GetClientRect(hwnd, &rc);
		
			// If the paint region includes the corner box between
			// the scroll bars, remember to paint it last
			
		if( lpInst->fVScroll && lpInst->fHScroll &&
				ps.rcPaint.right  > rc.right  - (lpGlob->cxVScroll - 1) &&
				ps.rcPaint.bottom > rc.bottom - (lpGlob->cyHScroll - 1) )
			fPaintBox = TRUE;
		else
			fPaintBox = FALSE;
			
			// Adjust the paint and client rectangles to take the
			// presence of scroll bars into account
			
		ps.rcPaint.right	= min(rc.right -
							   (lpInst->fVScroll ? lpGlob->cxVScroll - 1 : 0),
							   ps.rcPaint.right);
		ps.rcPaint.bottom	= min(rc.bottom -
							   (lpInst->fHScroll ? lpGlob->cyHScroll - 1 : 0),
							   ps.rcPaint.bottom);
		rc.right			= min(rc.right,		ps.rcPaint.right);
		rc.bottom			= min(rc.bottom, 	ps.rcPaint.bottom);
		
			// Determine how many characters we're shifted over due to
			// scrolling and convert that to pixels
			
		row = GetScrollPos(lpInst->hwndVScroll, SB_CTL);
		col = GetScrollPos(lpInst->hwndHScroll, SB_CTL);
		
		cx  = col * lpGlob->cx;
		cy  = row * lpGlob->cy;
		
			// Locate the first and last columns that need painting and
			// save the left and right edges of the bounding rectangle
			
		cSkip = 0;
		lpcol = lpInst->lpcol;
		for( uCol = 0; uCol < lpInst->cCol; uCol++ )
			{
			if( (int)cSkip + (int)lpcol->cbc * (int)lpGlob->cx + 2 * cxBORDER
					<= ps.rcPaint.left + cx )
				cSkip += (int)lpcol->cbc * (int)lpGlob->cx + 2 * cxBORDER;
			else
				break;
			lpcol++;
			}
			
		left		= (int)cSkip - cx;
		uFirstCol	= uCol;
		
			// Are there no columns in the invalid region?
		
		if( uFirstCol == lpInst->cCol )
			{
			if( fPaintBox )
				vPaintBox(hdc, hwnd);
			return;
			}
		
		for( uCol = uFirstCol; uCol < lpInst->cCol; uCol++ )
			{
			if( (int)cSkip + (int)lpcol->cbc * (int)lpGlob->cx + 2 * cxBORDER
					<= ps.rcPaint.right + cx )
				cSkip += (int)lpcol->cbc * (int)lpGlob->cx + 2 * cxBORDER;
			else
				break;
			lpcol++;
			}
			
			// Are all the columns in the invalid region?
			
		if( uCol == lpInst->cCol )
			{
			uCol--;
			lpcol--;
            cSkip -= (int)lpcol->cbc * (int)lpGlob->cx + 2 * cxBORDER;
			}
			
		right		= (int)cSkip + (int)lpcol->cbc * (int)lpGlob->cx
					  + 2 * cxBORDER - 1 - cx;
		uLastCol	= uCol;
		
			// Determine if we need to paint the titles; if so, do it
			
		if( ps.rcPaint.top < rc.top + lpGlob->cy )
			{
				// Select color and font for the column names
		
			SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
			hfontOld = (HFONT)SelectObject(hdc, lpGlob->hfontName);
		
				// Determine the bounding rectangle for the names
			
			rc.bottom	= rc.top + lpGlob->cy - 1;
			rc.left		= left;
			rc.right	= right;
		
				// Fill rectangle
			
			FillRect(hdc, &rc, lpGlob->hbrBtn);
		
				// Paint white bar across top of all the name boxes
			
			PatBlt(hdc, left, rc.top, right - left + 1, 1, WHITENESS);

				// Paint column names
			
			lpcol = lpInst->lpcol;
			for( uCol = 0; uCol < uFirstCol; uCol++ )
				lpcol++;
			rc.right = rc.left;
			for( uCol = uFirstCol; uCol <= uLastCol; uCol++ )
				{
				RECT	rcTemp;
				WORD	cTextLen, cHalfWidth;
				SIZE	size;
				
					// Add a white line on the left of the name box
					
				PatBlt(hdc, rc.left, rc.top, 1, lpGlob->cy, WHITENESS);
				
					// Locate the new right edge
			
				rc.right += (int)lpcol->cbc * (int)lpGlob->cx
							+ 2 * cxBORDER - 1;
				
					// Draw the name text string

                GetTextExtentPoint(hdc, lpcol->szName,
                                   lstrlen(lpcol->szName), &size);
				cTextLen = (WORD)size.cx;
												
				cHalfWidth		= (rc.right - rc.left + 1 - cTextLen) / 2;
				rcTemp.top		= rc.top	+ 1;
				rcTemp.bottom	= rc.bottom;
				rcTemp.left		= rc.left	+ cHalfWidth;
				rcTemp.right	= rc.right	- cHalfWidth;
											  
				DrawText(hdc, lpcol->szName, lstrlen(lpcol->szName),
						 &rcTemp, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
						 
					// Put a black line on the right edge of the name box
			
				PatBlt(hdc, rc.right, rc.top, 1, lpGlob->cy, BLACKNESS);
			
					// Move the left edge over for the next column
					// and advance the lpcol array pointer
					 
				rc.left = ++rc.right;
				lpcol++;
				}
			
				// Paint black line across bottom of all the name boxes
			
			PatBlt(hdc, left, rc.bottom, right - left + 1, 1, BLACKNESS);
		
				// Reset device context
			
			SelectObject(hdc, hfontOld);
			
				// Fill anything outside the rightmost column
				
			rc.left		= rc.right;
			rc.right	= ps.rcPaint.right;
			if( rc.left <= rc.right )
				FillRect(hdc, &rc, lpGlob->hbrWin);
			}
			
			// Determine if we need to paint any rows
			
		if( ps.rcPaint.bottom > rc.top + lpGlob->cy - 1 &&
            lpInst->fData )
			{
				// Locate the first and last rows that need to be painted
			
			uFirstRow = (ps.rcPaint.top <= rc.top + lpGlob->cy - 1)
						? cy / lpGlob->cy
						: (ps.rcPaint.top + cy - (rc.top + lpGlob->cy))
							/ lpGlob->cy;
						
			uLastRow  = (ps.rcPaint.bottom + cy - (rc.top + lpGlob->cy))
							/ lpGlob->cy;
			
				// Are there really rows there?
				
            if( uFirstRow > (UINT)(lpInst->cRow - 1) )
				{
				if( fPaintBox )
					vPaintBox(hdc, hwnd);
				return;
				}
				
            if( uLastRow > (UINT)(lpInst->cRow - 1) )
				uLastRow = lpInst->cRow - 1;
			
				// Determine the bounding rectangle for the SQL data
			
			rc.top		= rc.top + (uFirstRow + 1) * lpGlob->cy - cy;
			rc.bottom	= rc.top + lpGlob->cy - 1;
			rc.left		= left;
			rc.right	= right;
			
				// Select the font and brush for the data text and borders;
				// set the text background color to window background color
				
			SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
			hfontOld	= (HFONT )SelectObject(hdc, lpGlob->hfontData);
			hbrushOld	= (HBRUSH)SelectObject(hdc, lpGlob->hbrBtn);
			
				// Draw each row for which there is data

			for( uRow = uFirstRow; uRow <= uLastRow; uRow++ )
				{
					// Start the left and right edges at the far left
				
				rc.right = rc.left = left;
				
					// And each column for that row
					
				lpcol = lpInst->lpcol;
				for( uCol = 0; uCol < uFirstCol; uCol++ )
					lpcol++;
				for( uCol = uFirstCol; uCol <= uLastCol; uCol++ )
					{
					LPBYTE		lpb;
					LPSDWORD	lpcb;
					
						// Determine the new right edge
						
					rc.right += (int)lpcol->cbc * (int)lpGlob->cx
                                + 2 * cxBORDER - 1;

                        // Fill the bounding rectangle

                    FillRect(hdc, &rc, lpGlob->hbrWin);
					
						// Draw the data text
						
					lpcb = lpcol->lpcbuf + uRow;
					lpb	 = lpcol->lpbuf  + uRow * lpcol->cb;
					
					if( *lpcb == SQL_NULL_DATA )
						TextOut(hdc, rc.left + cxBORDER, rc.top,
								lpGlob->szNull, lstrlen(lpGlob->szNull));
					else
						TextOut(hdc, rc.left + cxBORDER, rc.top,
								(LPSTR)lpb, lstrlen((LPSTR)lpb));
								 
						// Add a line to the right of the box
						
					PatBlt(hdc, rc.right, rc.top, 1, lpGlob->cy, PATCOPY);
					
						// Move the left edge over for the next column
						// and advance the lpcol array pointer
						
					rc.left = ++rc.right;
					lpcol++;
					}
					
					// Paint a line across the bottom of the row
					
				PatBlt(hdc, left, rc.bottom, rc.right - left, 1, PATCOPY);
					   
					// Update the values of the bounding rectangle
					
				rc.top		+= lpGlob->cy;
				rc.bottom	+= lpGlob->cy;
				}
				
				// Restore the device context
				
			SelectObject(hdc, hbrushOld);
			SelectObject(hdc, hfontOld);
			
				// Save the current left edge
				
			left 		= rc.left;
			
				// Fill anything outside the bottom row
				
			rc.bottom	= ps.rcPaint.bottom;
			rc.left		= ps.rcPaint.left;
			rc.right	= ps.rcPaint.right;
			if( rc.top <= rc.bottom )
				FillRect(hdc, &rc, lpGlob->hbrWin);
			
				// Fill anything outside the rightmost column
				
			rc.top 		= ps.rcPaint.top;
			rc.bottom	= ps.rcPaint.bottom;
			rc.left		= left;
			rc.right	= ps.rcPaint.right;
			if( rc.left <= rc.right )
				FillRect(hdc, &rc, lpGlob->hbrWin);
			}
			
			// Paint the box between scroll bars if necessary
			
		if( fPaintBox )
			vPaintBox(hdc, hwnd);
		}
		
	return;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	vSetScroll
//
//		Determine if scroll bars are required, and if so, set their ranges
//      and show them.
//
//  Params:
//      hwnd        -- window handle
//
//  Returns:
//      none
//
void INTFUNC vSetScroll(HWND hwnd)
	{
static  BOOL        fInSetScroll;   // inside this procedure flag
        LPAPPGLOB   lpGlob;         // global information
        LPAPPINST   lpInst;         // instance information
        RECT        rc;             // window rectangle
        int         cx, cy,         // window dimensions
                    row, col;       // scroll positions
				
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
		// Use the fInSetScroll flag to prevent recursion from WM_SIZE msgs
		
	if( fInSetScroll )
		return;
		
	fInSetScroll = TRUE;
	
		// If there's no result set, there's no need for scrollbars
		
	if( !lpInst->fResultSet )
		{
		lpInst->fVScroll =
		lpInst->fHScroll = FALSE;
		
		ShowWindow(lpInst->hwndVScroll, SW_HIDE);
		ShowWindow(lpInst->hwndHScroll, SW_HIDE);
		
		fInSetScroll = FALSE;
		return;
		}
	
		// Save the current scroll positions
		
	row = GetScrollPos(lpInst->hwndVScroll, SB_CTL);
	col = GetScrollPos(lpInst->hwndHScroll, SB_CTL);
	
		// Get window dimensions
		
	GetClientRect(hwnd, &rc);
	cx = rc.right	- rc.left	+ 1;
	cy = rc.bottom	- rc.top	+ 1	- lpGlob->cy;
	
		// Assume no scroll bars to begin with
		
	lpInst->fHScroll =
	lpInst->fVScroll = FALSE;
	
		// Determine if a vertical scroll bar is needed
		
	if( cy / lpGlob->cy < (int)lpInst->cRow )
		{
		lpInst->fVScroll = TRUE;
		cx -= lpGlob->cxVScroll;
		}
		
		// Determine if a horizontal scroll bar is needed
		
	if( cx / lpGlob->cx < (int)lpInst->ccols )
		{
		lpInst->fHScroll = TRUE;
		cy -= lpGlob->cyHScroll;
		
			// If there's not already a vertical scroll bar,
			// perhaps we now need one
			
		if( !lpInst->fVScroll && cy / lpGlob->cy < (int)lpInst->cRow )
			{
			lpInst->fVScroll = TRUE;
			cx -= lpGlob->cxVScroll;
			}
		}
		
	lpInst->ccolwin = cx / lpGlob->cx;
 	lpInst->crowwin = cy / lpGlob->cy;
		
		// If no scrolling is necessary in either the vertical or horizontal
		// direction (or both, perhaps), reset the scroll positions
		
	if( !lpInst->fVScroll )
		row = 0;
	if( !lpInst->fHScroll )
		col = 0;
		
		// Set scroll ranges, positions, and scroll bar visibility
		
	SetScrollRange(lpInst->hwndVScroll, SB_CTL, 0,
				   lpInst->cRow - lpInst->crowwin, TRUE);
	SetScrollRange(lpInst->hwndHScroll, SB_CTL, 0,
				   lpInst->ccols + (2 * cxBORDER * lpInst->cCol / lpGlob->cx)
				   	- lpInst->ccolwin + 1,
				   TRUE);
	
	SetScrollPos  (lpInst->hwndVScroll, SB_CTL, row, TRUE);
	SetScrollPos  (lpInst->hwndHScroll, SB_CTL, col, TRUE);
	
	ShowWindow    (lpInst->hwndVScroll, lpInst->fVScroll ? SW_SHOW : SW_HIDE);
	ShowWindow    (lpInst->hwndHScroll, lpInst->fHScroll ? SW_SHOW : SW_HIDE);
	
		// If the window isn't completely filled vertically,
		// add one extra to the number of display rows so that
		// there is no white space between the last row and
		// the horizontal scroll bar or the window edge
		
	if( cy % lpGlob->cy )
		lpInst->crowwin++;
		
		// Size and position the scroll bars
		
	vSizeScroll(hwnd);
	
		// Allow entrace into this function
		
	fInSetScroll = FALSE;
	
	return;
	}
	

//////////////////////////////////////////////////////////////////////////////
//
//	vSizeScroll
//
//      Size and position scroll bars.
//
//  Params:
//      hwnd        -- window handle
//
//  Returns:
//      none
//
void INTFUNC vSizeScroll(HWND hwnd)
	{
    LPAPPGLOB   lpGlob;             // global information
    LPAPPINST   lpInst;             // instance information
    RECT        rc;                 // window rectangle

        // Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
    lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));

        // Get the window rectangle
	
    GetClientRect(hwnd, &rc);
	
        // Place the vertical scroll bar
		
	MoveWindow(lpInst->hwndVScroll,
				rc.right - lpGlob->cxVScroll + 1,
				rc.top,
				lpGlob->cxVScroll,
				rc.bottom - rc.top + 1 -
					(lpInst->fHScroll ? lpGlob->cyHScroll : 0),
				TRUE);
				
		// Place the horizontal scroll bar
		
	MoveWindow(lpInst->hwndHScroll,
				rc.left,
				rc.bottom - lpGlob->cyHScroll + 1,
				rc.right - rc.left + 1 -
					(lpInst->fVScroll ? lpGlob->cxVScroll : 0),
				lpGlob->cyHScroll,
				TRUE);
				
	return;
	}
	

//////////////////////////////////////////////////////////////////////////////
//
//	WndProc
//
//      Main window procedure.
//
//  Params:
//      hwnd        -- window handle
//      wmsg        -- message ID
//      wParam      -- word parameter
//      lParam      -- long parameter
//
//  Returns:
//      LRESULT     -- depends on wmsg
//
LRESULT EXPFUNC WndProc(HWND	hwnd,
						UINT	wmsg,
						WPARAM	wParam,
						LPARAM	lParam )
	{
    LPAPPGLOB       lpGlob;         // global information
    LPAPPINST       lpInst;         // instance information
	
		// Get the global and instance-specific structures
		
	lpGlob = (LPAPPGLOB)GetWindowLong(hwnd, 0);
	lpInst = (LPAPPINST)GetWindowLong(hwnd, sizeof(LONG));
	
	switch( wmsg )
		{
			// Initially, disable unnecessary menu items
		
		case WM_CREATE:
			vEnableMenus(hwnd);
			break;
			
		case WM_CLOSE:
			if( lpInst->fConnected )
                {
                int         iRes;

                    // Bring up a confirmation dialog box

                iRes = DialogBoxParam(lpInst->hinst,
                                      MAKEINTRESOURCE(CONFIRMBOX),
                                      hwnd, DLGPROC(ConfirmDlgProc),
                                      (LONG)lpInst);
				UpdateWindow(hwnd);
				
				if( iRes == IDOK && fDisconnect(hwnd) )
					DestroyWindow(hwnd);
				break;
				}
			else
				return DefWindowProc(hwnd, wmsg, wParam, lParam);
			
#ifdef _MAC
        case WM_SYSCOMMAND:
            if (lParam ==0 && LOWORD(wParam) == ID_HELP_ABOUT)
                {
                    SendMessage(hwnd,WM_COMMAND,ID_HELP_ABOUT,0);
                    return 0;
                }
            else
				return DefWindowProc(hwnd, wmsg, wParam, lParam);
#endif
		
	  	case WM_COMMAND:
	  		switch( GET_WM_COMMAND_ID(wParam, lParam) )
				{
                case ID_FILE_EXIT:
					PostMessage(hwnd, WM_CLOSE, 0, 0L);
					break;
					
				case ID_ENV_OPEN:
				
						// Disconnect in order to reconnect..
						
					if( lpInst->fConnected )
                        {
                        int         iRes;
					
							// Bring up a confirmation dialog box

                        iRes = DialogBoxParam(lpInst->hinst,
                                              MAKEINTRESOURCE(CONFIRMBOX),
                                              hwnd,
                                              DLGPROC(ConfirmDlgProc),
                                              (LONG)lpInst);
                        UpdateWindow(hwnd);
						
						if( iRes != IDOK || !fDisconnect(hwnd) )
							break;
						
							// Disable appropriate menu items, in
							// case the new connection fails
						                
						vEnableMenus(hwnd);
						}
									
						// Make the connection and enable the appropriate menu
						// items if it is successfully made
									
					if( fConnect(hwnd) )
						vEnableMenus(hwnd);
					break;
					
				case ID_ENV_CLOSE:
                    {
                    int         iRes;
					
						// If we're not connected, there's something wrong
					
					DASSERT(lpInst->fConnected);

						// Bring up a confirmation dialog box

                    iRes = DialogBoxParam(lpInst->hinst,
                                          MAKEINTRESOURCE(CONFIRMBOX),
                                          hwnd, DLGPROC(ConfirmDlgProc),
                                          (LONG)lpInst);
                    UpdateWindow(hwnd);
				
						// Disconnect and disable appropriate menu
						// items if the disconnection was successful
						
					if( iRes == IDOK && fDisconnect(hwnd) )
						vEnableMenus(hwnd);
					}
					break;
					
				case ID_ENV_ADD:
					
						// Bring up the create data source dialog box
					#ifndef _MAC
					SQLCreateDataSource(hwnd, NULL);
					#else
					SQLCreateDataSource(GetWrapperWindow(hwnd), NULL);
					#endif
					// We don't care whether a data source was created or
					// not, so we'll ignore the return code from this call
					break;
					
				case ID_SQL_EXEC:
                    {
                    int         iRes;
					
					DASSERT(lpInst->fConnected && lpInst->cStmt);
					
						// Bring up the SQL statement dialog box and execute
						// a SQL statement if the user gives us one to use

                    iRes = DialogBoxParam(lpInst->hinst,
                                          MAKEINTRESOURCE(SQLBOX), hwnd,
                                          DLGPROC(SQLDlgProc), (LONG)lpInst);

                    if( iRes == IDOK )
                        {
                        UpdateWindow(hwnd);
                        vDoSQL(hwnd);
                        }
                    else
                        break;
					
						// Draw the data
						
					vSetScroll(hwnd);
					InvalidateRect(hwnd, NULL, FALSE);
					
						// Enable menu items
						
					vEnableMenus(hwnd);
					}
					break;
					
				case ID_SQL_FETCH:
					{
					RECT	rc;

                        // Exclude column titles
					
					GetClientRect(hwnd, &rc);
					rc.top += lpGlob->cy + 1;
						
						// Get the data
						
					vFetch(hwnd);
					
						// Adjust the scrollbars if necessary
						
					if( lpInst->cRow < lpInst->cMaxRow )
						vSetScroll(hwnd);
					
						// Paint it
					
					InvalidateRect(hwnd, &rc, FALSE);
					
						// Enable (or disable) menu items
						
					vEnableMenus(hwnd);
					}
                    break;

                case ID_HELP_HELP:
                    WinHelp(hwnd, szHELPFILE, HELP_CONTEXT, HLP_CPPDEMO);
                    break;

                case ID_HELP_ABOUT:

						// Bring up the About.. dialog box

                    DialogBoxParam(lpInst->hinst, MAKEINTRESOURCE(ABOUTBOX),
                                   hwnd, DLGPROC(AboutDlgProc), (LONG)lpInst);
					break;
                }
			break;
			
		case WM_PAINT:
			{
			PAINTSTRUCT	ps;
			
				// Paint the window
			
			if( BeginPaint(hwnd, &ps) )
				{
				vPaintWindow(hwnd, ps);
				EndPaint(hwnd, &ps);
				}
			}
			break;
			
		case WM_ERASEBKGND:
			{
			RECT	rc;
			
			GetClientRect(hwnd, &rc);
			FillRect((HDC)wParam, &rc, lpGlob->hbrWin);
			}
			return TRUE;
			
		case WM_SYSCOLORCHANGE:
	#ifndef _MAC
			if( lpInst->fCtl3d )
				Ctl3dColorChange();
	#endif
			
				// Recreate brushes when system colors change
				
			if( lpGlob->hbrWin )
				DeleteObject(lpGlob->hbrWin);
			if( lpGlob->hbrBtn )
				DeleteObject(lpGlob->hbrBtn);
			if( lpGlob->hbrScroll )
				DeleteObject(lpGlob->hbrScroll);
				
			lpGlob->hbrWin		=
				CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			lpGlob->hbrBtn		=
				CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
			lpGlob->hbrScroll	=
				CreateSolidBrush(GetSysColor(COLOR_SCROLLBAR));
			break;
			
		case WM_KEYDOWN:
		
				// Convert keyboard commands into scroll equivalents
		
			if( wParam == VK_DOWN || wParam == VK_UP )
				{
				if( !lpInst->fVScroll )
					break;
					
                wmsg = WM_VSCROLL;
                wParam = (wParam == VK_DOWN ? SB_LINEDOWN : SB_LINEUP);
				}
			else if( wParam == VK_LEFT || wParam == VK_RIGHT )
				{
				if( !lpInst->fHScroll )
					break;
					
                wmsg = WM_HSCROLL;
                wParam = (wParam == VK_RIGHT ? SB_LINEDOWN : SB_LINEUP);
				}
			else if( wParam == VK_HOME || wParam == VK_END )
				{
				if( lpInst->fVScroll )
					{
                    wmsg = WM_VSCROLL;
                    wParam = (wParam == VK_HOME ? SB_TOP : SB_BOTTOM);
					}
				else if( lpInst->fHScroll )
					{
                    wmsg = WM_HSCROLL;
                    wParam = (wParam == VK_HOME ? SB_TOP : SB_BOTTOM);
					}
				else
					break;
				}
			else if( wParam == VK_PRIOR || wParam == VK_NEXT )
				{
				if( lpInst->fVScroll )
					{
                    wmsg = WM_VSCROLL;
                    wParam = (wParam == VK_PRIOR ? SB_PAGEUP : SB_PAGEDOWN);
					}
				else if( lpInst->fHScroll )
					{
                    wmsg = WM_HSCROLL;
                    wParam = (wParam == VK_PRIOR ? SB_PAGEUP : SB_PAGEDOWN);
					}
				else
					break;
				}
			else
				break;
				
				// If we adjusted the message, fall through to the scroll code
													                          
		case WM_VSCROLL:
		case WM_HSCROLL:
		    {
				// Scroll the results window
			
			HWND	hwndCtl;
			int		cPage,
					cLine,	
					nPos, iPos,
					iOrig,
					nMin, nMax;
			
				// If there's no result set, don't scroll
			
			if( !lpInst->fResultSet )
				break;
			
				// Determine scroll direction and distance
				
			hwndCtl	= (wmsg == WM_VSCROLL
							? lpInst->hwndVScroll
							: lpInst->hwndHScroll);
			cPage	= (wmsg == WM_VSCROLL
							? lpInst->crowwin - 1
							: lpInst->ccolwin);
			cLine	= 1;
			nPos	= GET_WM_HSCROLL_POS(wParam, lParam);
			iPos	=
			iOrig	= GetScrollPos(hwndCtl, SB_CTL);
			
			GetScrollRange(hwndCtl, SB_CTL, &nMin, &nMax);
			switch( GET_WM_VSCROLL_CODE(wParam, lParam) )
				{
				case SB_BOTTOM:
					iPos  = nMax;
					break;
					
				case SB_LINEDOWN:
					iPos += cLine;
					break;
					
				case SB_LINEUP:
					iPos -= cLine;
					break;
					
				case SB_PAGEDOWN:
					iPos += cPage;
					break;
					
				case SB_PAGEUP:
					iPos -= cPage;
					break;
					
				case SB_TOP:
					iPos  = nMin;
					break;
					
				case SB_THUMBPOSITION:
					iPos = nPos;
					break;
				}
				
				// Don't let scroll requests leave the boundaries
				
			if( iPos < nMin )
				iPos = nMin;
			else if( iPos > nMax )
				iPos = nMax;
				
				// If movement has occurred, scroll the window
				
			if( iPos != iOrig )
				{
				RECT	rc;
				
				GetClientRect(hwnd, &rc);
				rc.top		+= (wmsg == WM_VSCROLL ? lpGlob->cy : 0);
				rc.bottom	-= (lpInst->fHScroll ? lpGlob->cyHScroll - 1 : 0);
				rc.right	-= (lpInst->fVScroll ? lpGlob->cxVScroll - 1 : 0);
				
				SetScrollPos(hwndCtl, SB_CTL, iPos, TRUE);
				ScrollWindow(hwnd,
					(wmsg == WM_HSCROLL ? lpGlob->cx * (iOrig - iPos) : 0),
					(wmsg == WM_VSCROLL ? lpGlob->cy * (iOrig - iPos) : 0),
					&rc, &rc);
				UpdateWindow(hwnd);
				}
			}
			break;
			
		case WM_SIZE:
			if( wParam == SIZE_MINIMIZED )
				lpInst->fIsMinimized = TRUE;
			else
				{
				if( lpInst->fIsMinimized )
					lpInst->fIsMinimized = FALSE;
				vSizeScroll(hwnd);
				if( lpInst->fResultSet )
					vSetScroll(hwnd);
				}
			break;
			
		case WM_DESTROY:
			WinHelp(hwnd, szHELPFILE, HELP_QUIT, NULL);
			PostQuitMessage(0);
			break;
		                                            
		default:
			return DefWindowProc(hwnd, wmsg, wParam, lParam);
		}
		
	return 0;
	}


//////////////////////////////////////////////////////////////////////////////
//
//	WinMain
//
//      Windows entry point.
//
//  Params:
//      hInstance       -- application instance handle
//      hPrevInstance   -- previous instance of application handle
//      lpszCmdLine     -- command line string
//      int             -- desired initial window appearance
//
//  Returns:
//      int             -- application return value; 0 if no errors
//
int PASCAL WinMain(	HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					LPSTR		lpszCmdLine,
					int			nCmdShow )
	{
    LPAPPINST       lpInst;         // instance information
    LPAPPGLOB       lpGlob;         // global information
    HGLOBAL         hGlob;          // global information handle
    HWND            hwnd;           // window handle
    MSG             msg;            // message structure
    WNDCLASS        wc;             // window class
    HDC             hdc;            // device context handle
    HFONT           hfontOld;       // old font handle
    TEXTMETRIC      tm;             // text metrics structure
	HACCEL			hAccel;			// accelerators.
#ifdef _MAC
	CHAR			szAbout[80];	// About string.
#endif
	
	#ifdef _68K_
	if (IsLibraryManagerLoaded())
		UnloadLibraryManager();
	if (InitLibraryManager(0, kCurrentZone, kNormalMemory) != 0)
	{
		char szAppName[80];
		char szBuffer[80];
		LoadString(hInstance, IDS_MAIN, szAppName, sizeof(szAppName));
		LoadString(hInstance, IDS_ASLMERROR, szBuffer, sizeof(szBuffer));
		MessageBox(NULL, szBuffer, szAppName, MB_OK|MB_ICONHAND);
	 	return 0;
	}
	#endif

// Allocate a new APPINST structure
		
	lpInst = new APPINST;
	if( !lpInst )
		return FALSE;
	
		// If this is the first instance of the app, register the class
		
	if( !hPrevInstance )
		{
			// Allocate a new APPGLOB structure
			              
		hGlob = GlobalAlloc(GHND | GMEM_SHARE, sizeof(APPGLOB));
		lpGlob = (LPAPPGLOB)GlobalLock(hGlob);	         
		if( !lpGlob )
			{
			delete lpInst;
			if( hGlob )
				GlobalFree(hGlob);
			return FALSE;
			}
			
			// Fill out the class structure
		
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = WndProc;
		wc.cbClsExtra    = sizeof(LONG);
		wc.cbWndExtra    = sizeof(LONG) * 2;
		wc.hInstance     = hInstance;
		wc.hIcon         =
		lpGlob->hicon	 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1);
		wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU);
		wc.lpszClassName = szCLASSNAME;
		
		if( !RegisterClass(&wc) )
			{
			delete lpInst;
			GlobalUnlock(hGlob);
			GlobalFree(hGlob);
			return FALSE;
			}
			
			// Fill in the APPGLOB structure
			
		LoadString(hInstance, IDS_MAIN,
				   lpGlob->szTitle, sizeof(lpGlob->szTitle));
		LoadString(hInstance, IDS_SZNULL,
				   lpGlob->szNull,  sizeof(lpGlob->szNull));
		lpGlob->hbrWin		= CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		lpGlob->hbrBtn		= CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		lpGlob->hbrScroll	= CreateSolidBrush(GetSysColor(COLOR_SCROLLBAR));
		lpGlob->cxVScroll	= GetSystemMetrics(SM_CXVSCROLL);
		lpGlob->cyHScroll	= GetSystemMetrics(SM_CYHSCROLL);
		lpGlob->cInstance	= 0;
		
			// Create fonts
		
		hdc = GetDC(NULL);
		if( !hdc )
			{
			delete lpInst;
			GlobalUnlock(hGlob);
			GlobalFree(hGlob);
			return FALSE;
			}
	
			// Column name font

		lpGlob->hfontName	= CreateFont((GetDeviceCaps(hdc, LOGPIXELSY)
									* cPOINTS) / 72,
									0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0,
									szFONT);
	
		hfontOld			= (HFONT)SelectObject(hdc, lpGlob->hfontName);
	
		GetTextMetrics(hdc, &tm);
		lpGlob->cx			= min(tm.tmMaxCharWidth,
								  tm.tmAveCharWidth + tm.tmAveCharWidth / 8);
		lpGlob->cy			= tm.tmHeight + tm.tmInternalLeading;
	
			// Data font
	
		lpGlob->hfontData = CreateFont((GetDeviceCaps(hdc, LOGPIXELSY)
									* cPOINTS) / 72,
									0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0,
									szFONT);
							
		SelectObject(hdc, lpGlob->hfontData);
	
		GetTextMetrics(hdc, &tm);
		lpGlob->cx			= max(lpGlob->cx, min(tm.tmMaxCharWidth,
								  tm.tmAveCharWidth + tm.tmAveCharWidth / 8));
		lpGlob->cy			= max(lpGlob->cy,
								  tm.tmHeight + tm.tmInternalLeading);
	
			// Restore original font and release DC
		
		SelectObject(hdc, hfontOld);
		ReleaseDC(NULL, hdc);
		}
	else
		lpGlob = NULL;

		// Create cEnv, the environment object for this instance --
		// if this fails, we can't really do anything else
		
	lpInst->cEnv = new CENV;
	if( !lpInst->cEnv || !lpInst->cEnv->Success(lpInst->cEnv->m_rc) )
		{
		delete lpInst;
		if( lpGlob )
			{
			GlobalUnlock(hGlob);
			GlobalFree(hGlob);
			}
		return FALSE;
		}
	
		// Initialize this instance of the app
		
	lpInst->hinst			= hInstance;
	lpInst->fVScroll		= FALSE;
	lpInst->fHScroll		= FALSE;
	lpInst->fIsMinimized	= (nCmdShow == SW_MINIMIZE ||
					   		   nCmdShow == SW_SHOWMINIMIZED ||
					   		   nCmdShow == SW_SHOWMINNOACTIVE);
	lpInst->cDbc			= NULL;
	lpInst->cStmt			= NULL;
	lpInst->fConnected		= FALSE;
	lpInst->fResultSet		= FALSE;
	lpInst->fData			= FALSE;
	lpInst->cCol			= 0;
	lpInst->cRow			= 0;
	lpInst->cMaxRow			= 100;
	lpInst->crowwin			= 0;
	lpInst->ccolwin			= 0;
	lpInst->ccols			= 0;
	lpInst->cbrow			= 0;
	lpInst->lpcol			= NULL;
	
	LoadString(hInstance, IDS_SQLDEF,
			   lpInst->szSQL, sizeof(lpInst->szSQL));

		// Create the window..
		
	if( !(hwnd = CreateWindow(szCLASSNAME, NULL,
			  					WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
								CW_USEDEFAULT, CW_USEDEFAULT,
								CW_USEDEFAULT, CW_USEDEFAULT,
			  					HWND_DESKTOP,
			  					NULL,
			  					hInstance,
			  					NULL)) )
		{
		delete lpInst;
		if( lpGlob )
			{
			GlobalUnlock(hGlob);
			GlobalFree(hGlob);
			}		  					
		return FALSE;
		}

#ifdef _MAC
     // Change About to About Multipad on Apple Menu
		{
        HMENU hmenuSys;

        hmenuSys = GetSystemMenu(hwnd, FALSE);
		LoadString(hInstance, IDS_ABOUT, szAbout, sizeof(szAbout));
        ModifyMenu(hmenuSys, 0, MF_BYPOSITION, ID_HELP_ABOUT, szAbout);
		}
#endif
		
		// Create scroll bars
		
	lpInst->hwndVScroll	= CreateWindow(szSCROLLCLASS, NULL,
								   WS_CHILD | WS_CLIPSIBLINGS | SBS_VERT,
								   0, 0, 0, 0,
								   hwnd, (HMENU)1, hInstance, NULL);
	ShowWindow(lpInst->hwndVScroll, SW_HIDE);
								   
	lpInst->hwndHScroll	= CreateWindow(szSCROLLCLASS, NULL,
								   WS_CHILD | WS_CLIPSIBLINGS | SBS_HORZ,
								   0, 0, 0, 0,
								   hwnd, (HMENU)2, hInstance, NULL);
	ShowWindow(lpInst->hwndHScroll, SW_HIDE);
	
		// If this is the first instance, lpGlob will be non-NULL
		// and, in that case, we need to add it to the class;
		// otherwise, retrieve the value of lpGlob through hGlob
		
	if( lpGlob )
		SetClassLong(hwnd, 0, (LONG)(LPVOID)hGlob);
	else
		{
		hGlob = (HGLOBAL)GetClassLong(hwnd, 0);
		lpGlob = (LPAPPGLOB)GlobalLock(hGlob);
		}
		
		// Store the lpGlob pointer in this window's data space
		
	SetWindowLong(hwnd, 0, (LONG)lpGlob);
		
		// Increment the instance counter
		
	lpGlob->cInstance++;
	
		// Set the window title
		
	lstrcpy(lpInst->szTitle, lpGlob->szTitle);
	SetWindowText(hwnd, lpInst->szTitle);
		
		// Initialize the CTL3D effects
#ifndef _MAC		
	lpInst->fCtl3d = Ctl3dRegister(hInstance);
	if( lpInst->fCtl3d )
		lpInst->fAutoCtl3d = Ctl3dAutoSubclass(hInstance);
	else
		lpInst->fAutoCtl3d = FALSE;
#endif

		// Store the lpInst value with the window
		
	SetWindowLong(hwnd, sizeof(LONG), (LONG)lpInst);
		
		// Show the window
		
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

    /* Load main menu accelerators */
    if (!(hAccel = LoadAccelerators (hInstance, MAKEINTRESOURCE(IDR_MENU))))
        return FALSE;

		// Get and dispatch messages

    while (GetMessage (&msg, NULL, NULL, NULL))
		{
        /* If a keyboard message is for the MDI , let the MDI client
         * take care of it.  Otherwise, check to see if it's a normal
         * accelerator key (like F3 = find next).  Otherwise, just handle
         * the message as usual.
         */
        if (!TranslateAccelerator (hwnd, hAccel, &msg))
			{
            TranslateMessage (&msg);
            DispatchMessage (&msg);
			}
		}

		// Clean up this instance of the app
		
	if( lpInst->cStmt )
		delete lpInst->cStmt;
	if( lpInst->cDbc )
		delete lpInst->cDbc;
	if( lpInst->cEnv )
		delete lpInst->cEnv;
		
	if( lpInst->lpcol )
		{
		LPCOL	lpcol;
		WORD	n;
		
		lpcol = lpInst->lpcol;
		for( n = 0; n < lpInst->cCol; n++ )
			{
			FreePtr(lpcol->lpb);
			FreePtr(lpcol->lpcb);
			FreePtr(lpcol->lpbuf);
			FreePtr(lpcol->lpcbuf);
		
			lpcol++;
			}
		FreePtr(lpInst->lpcol);
		}
		
#ifndef _MAC
	if( lpInst->fCtl3d )
		Ctl3dUnregister(hInstance);
#endif
	
	delete lpInst;
	lpGlob->cInstance--;
	
		// If this was the last instance, clean up the global structs
		
	if( !lpGlob->cInstance )
		{
			// Delete objects
		
		if( lpGlob->hbrWin )
			DeleteObject(lpGlob->hbrWin);
		if( lpGlob->hbrBtn )
			DeleteObject(lpGlob->hbrBtn);
		if( lpGlob->hbrScroll )
			DeleteObject(lpGlob->hbrScroll);
		
		if( lpGlob->hfontName )
			DeleteObject(lpGlob->hfontName);
		if( lpGlob->hfontData )
			DeleteObject(lpGlob->hfontData);
		
		DestroyIcon(lpGlob->hicon);
		
			// Free up the global structure
			
		GlobalUnlock(hGlob);
		GlobalFree(hGlob);
		}
	else
		{
			// Otherwise, just decrement the lock count on the global struct
			
		GlobalUnlock(hGlob);
		}

	#ifdef _68K_
	CleanupLibraryManager();
	#endif

	return TRUE;
	}
