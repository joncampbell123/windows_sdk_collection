// view.cpp : Defines the behaviors for the application and frame window.
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include <afxwin.h>

#include "resource.h"
#include "database.h"
#include "view.h"

#include <commdlg.h>

#define SIZESTRING 256
#define SIZENAME 30
#define SIZEPHONE 26
#define PAGESIZE 8

// a simple way to reduce size of C runtimes
// disables the use of getenv and argv/argc
extern "C" void _setargv() { }
extern "C" void _setenvp() { }

/////////////////////////////////////////////////////////////////////////////
// Create the single global instance of the database viewer app.

CTheApp PersonApp;

/////////////////////////////////////////////////////////////////////////////
// CTheApp

//////////////////////////////////////////////////
//  CtheApp::InitInstance
//  Override InitInstance function to create a CMainWindow object
//  and display it on the screen
//
BOOL CTheApp::InitInstance()
{
	m_pMainWnd = new CMainWindow();
	m_pMainWnd->ShowWindow( m_nCmdShow );
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFindDialog

//////////////////////////////////////////////////
//  CFindDialog::OnOK
//  When the user hits OK get the data entered and store it in this
//  object.  Then end the dialog.
//
void CFindDialog::OnOK()
{
	GetDlgItemText( IDC_DATA, m_szFindName.GetBuffer( SIZESTRING ), SIZESTRING );
	m_szFindName.ReleaseBuffer();
	EndDialog( IDOK );
}

/////////////////////////////////////////////////////////////////////////////
// CEditPerson

//////////////////////////////////////////////////
//  CEditDialog::OnInitDialog
//  Fill in the fields witht the data placed in this object
//  when it was created.
//
BOOL CEditDialog::OnInitDialog()
{
	SetDlgItemText( IDC_LASTNAME,  m_pData->GetLastName() );
	SetDlgItemText( IDC_FIRSTNAME, m_pData->GetFirstName() );
	SetDlgItemText( IDC_PHONE, m_pData->GetPhoneNumber() );
	SetDlgItemText( IDC_MOD, m_pData->GetModTime().Format("%m/%d/%y %H:%M" ) );
	SendDlgItemMessage( IDC_LASTNAME, EM_SETSEL );

	return TRUE;
}

//////////////////////////////////////////////////
//  CEditDialog::OnOK
//  When OK is pressed set the data to what the user has entered.
//
void CEditDialog::OnOK()
{
	char szTmp[SIZESTRING];
	
	GetDlgItemText( IDC_LASTNAME, szTmp, sizeof ( szTmp ) );
	m_pData->SetLastName( szTmp );
	
	GetDlgItemText( IDC_FIRSTNAME, szTmp, sizeof ( szTmp ) );
	m_pData->SetFirstName( szTmp );
	
	GetDlgItemText( IDC_PHONE, szTmp, sizeof ( szTmp ) );
	m_pData->SetPhoneNumber( szTmp );
	
	EndDialog( IDOK );
}

/////////////////////////////////////////////////////////////////////////////
// CMainWindow

BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )

	// File menu commands:
	ON_COMMAND( IDM_NEW, OnNew )
	ON_COMMAND( IDM_OPEN, OnOpen )
	ON_COMMAND( IDM_SAVE, OnSave )
	ON_COMMAND( IDM_SAVEAS, OnSaveAs )
	ON_COMMAND( IDM_CLOSE, OnDBClose )
	ON_COMMAND( IDM_PRINT, OnPrint )
	ON_COMMAND( IDM_EXIT, OnExit )

	// Person menu commands:
	ON_COMMAND( IDM_ADD, OnAdd )
	ON_COMMAND( IDM_DELETE, OnDelete )
	ON_COMMAND( IDM_EDIT, OnEdit )
	ON_COMMAND( IDM_FIND, OnFind )
	ON_COMMAND( IDM_FINDALL, OnFindAll )

	// Help menu commands:
	ON_COMMAND( IDM_HELP, OnHelp )
	ON_COMMAND( IDM_ABOUT, OnAbout )

	// Selection accelerators:
	ON_COMMAND( VK_UP, OnUp )
	ON_COMMAND( VK_DOWN, OnDown )

	// Other Windows messages:
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
END_MESSAGE_MAP()


//////////////////////////////////////////////////
//  CMainWindow::CMainWindow
//  Constructs and initializes a CMainWindow object
//
CMainWindow::CMainWindow()
{
	VERIFY( LoadAccelTable( "MainAccelTable" ) );
	VERIFY( Create( NULL, "Phone Book",
			WS_OVERLAPPEDWINDOW, rectDefault, NULL, "MainMenu" ) );
	m_nSelectLine = -1;
}

/////////////////////////////////////////////////////////////////////////////
//  The Following are CMainWindow Menu Items

//////////////////////////////////////////////////
//  CMainWindow::OnNew
//  After checking to see if current data needs to be stored, call
//  database New and resize/repaint the window.
//
void CMainWindow::OnNew()
{
	if ( !CheckForSave( "File New", "Save file before New?" ) )
		return;
	m_people.New();
	SetMenu();
	SetWindowText( m_people.GetTitle() );
	OnSize( 0, m_cxClient, m_cyClient );
}

//////////////////////////////////////////////////
//  CMainWindow::OnOpen
//
void CMainWindow::OnOpen()
{
	if ( !CheckForSave( "File Open", "Save file before Open?" ) )
		return;

	// Attempt to open a database file and read it.
	// If a file or archive exception occurs, catch it and
	// present an error message box.
	CString szFileName, szFileTitle;
	TRY
	{
		// Use CommDlg to get the file name and then call DoOpen.
		// Set the Window title and menus.  Resize/Repaint.
		if ( FileDlg( TRUE, SIZESTRING, szFileName.GetBuffer( SIZESTRING ),
				SIZESTRING, szFileTitle.GetBuffer( SIZESTRING ) ) )
		{
			szFileName.ReleaseBuffer();
			szFileTitle.ReleaseBuffer();
			m_people.DoOpen( szFileName );
			m_people.SetTitle( szFileTitle );
			SetWindowText( m_people.GetTitle() );
			SetMenu();
			OnSize( 0, m_cxClient, m_cyClient );
		}
	}
	CATCH( CFileException, e )
	{
		char ErrorMsg[SIZESTRING];
		sprintf( ErrorMsg,"Opening %s returned a 0x%lx.",
				(const char*)szFileTitle, e->m_lOsError );
		MessageBox( ErrorMsg, "File Open Error" );
	}
	AND_CATCH( CArchiveException, e )
	{
		char ErrorMsg[SIZESTRING];
		sprintf( ErrorMsg,"Reading the %s archive failed.",
				(const char*)szFileTitle );
		MessageBox( ErrorMsg, "File Open Error" );
	}
	END_CATCH
}

//////////////////////////////////////////////////
//  CMainWindow::OnSave
//
void CMainWindow::OnSave()
{
	Save( m_people.IsNamed() );
}

//////////////////////////////////////////////////
//  CMainWindow::OnSaveAs
//
void CMainWindow::OnSaveAs()
{
	Save();
}

//////////////////////////////////////////////////
//  CMainWindow::OnDBClose
//  Closes the current database, checking to see if it should be
//  saved first.  Reset the window title and the scroll regions.
//  Invalidating the entire screen causes OnPaint to repaint but
//  this time without any data.
//
void CMainWindow::OnDBClose()
{
	if ( !CheckForSave( "File Close", "Save file before closing?" ) )
		return;
	m_people.Terminate();
	SetWindowText( "Phone Book" );
	SetMenu();
	OnSize( 0, m_cxClient, m_cyClient );
}

//////////////////////////////////////////////////
//  CMainWindow::OnPrint
//  Uses the commdlg print dialog to create a printer dc
//  Then it uses code almost identical to the OnPaint code
//  to write the data to the printer.
//
void CMainWindow::OnPrint()
{

	PRINTDLG pd;
	
	pd.lStructSize = sizeof( PRINTDLG );
	pd.hwndOwner=m_hWnd;
	pd.hDevMode=(HANDLE)NULL;
	pd.hDevNames=(HANDLE)NULL;
	pd.Flags=PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS;
	pd.nFromPage=0;
	pd.nToPage=0;
	pd.nMinPage=0;
	pd.nMaxPage=0;
	pd.nCopies=1;
	pd.hInstance=(HINSTANCE)NULL;

	if ( PrintDlg( &pd ) != 0 )
	{
		// CommDlg returned a DC so create a CDC object from it.
		ASSERT( pd.hDC != 0 );
		CDC * dc;
		dc = CDC::FromHandle( pd.hDC );

		// Change to hour glass while printing
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_WAIT ) );

		// Begin printing the document.
		int rc;
		char szError[SIZESTRING];

		rc = dc->StartDoc( "Phone Book" );

		if ( rc < 0 )
		{
			sprintf( szError, "Unable to Begin printing - Error[%d]", rc );
			MessageBox( szError, NULL,MB_OK );
			return;
		}
		
		int x, y;
		CPerson* pCurrent;
		int nPerson=0;
		CString szDisplay;
		int nStart, nEnd;
		
		// Get Height and Width of large character
		CSize extentChar = dc->GetTextExtent( "M", 1 );
		int nCharHeight = extentChar.cy;
		int nCharWidth = extentChar.cx;
		
		// Get Page size in # of full lines
		int nExtPage = ( dc->GetDeviceCaps(VERTRES) - nCharHeight )
				/ nCharHeight;
		
		CString szTitle;
		szTitle = CString( "Phone Book - " ) + m_people.GetName();

		while ( nPerson != m_people.GetCount() )
		{
			// Print a Page Header
			dc->StartPage();
			dc->SetTextAlign ( TA_LEFT | TA_TOP );
			dc->TextOut( 0, 0, szTitle, szTitle.GetLength() );
			dc->MoveTo( 0, nCharHeight );
			dc->LineTo( 
				dc->GetTextExtent( szTitle, szTitle.GetLength() ).cx,
				nCharHeight );

			// Print People from start to last person or page size minus
			// 2 ( header size )
			nEnd = min( m_people.GetCount() - nPerson, nExtPage-2 );
			for ( nStart = 0; nStart < nEnd; nStart++, nPerson++ )
			{
				x = 0;
				y = nCharHeight * ( nStart+2 );

				pCurrent = m_people.GetPerson( nPerson );
				szDisplay = " " + pCurrent->GetLastName() + ", " +
						pCurrent->GetFirstName();
				dc->SetTextAlign( TA_LEFT | TA_TOP );
				dc->TextOut( x, y, szDisplay, szDisplay.GetLength() );

				szDisplay = pCurrent->GetPhoneNumber();
				dc->SetTextAlign( TA_RIGHT | TA_TOP );
				dc->TextOut( x + SIZENAME * nCharWidth, y, szDisplay,
						szDisplay.GetLength() );

				szDisplay = pCurrent->GetModTime().Format( "%m/%d/%y %H:%M" );
				dc->TextOut( x + ( SIZENAME + SIZEPHONE ) * nCharWidth, y,
					szDisplay, szDisplay.GetLength() );
			}
			dc->EndPage();
		}
		dc->EndDoc();
		dc->DeleteDC();
		SetCursor( AfxGetApp()->LoadStandardCursor( IDC_ARROW ) );
	}
}

//////////////////////////////////////////////////
//  CMainWindow::OnExit
//
void CMainWindow::OnExit()
{
	OnClose();
}

//////////////////////////////////////////////////
//  CMainWindow::OnAdd
//  Using the EditDialog fill in a new person object.  If the user
//  selects OK then add the person, call OnSize to resize the scroll
//  region, and invalidate the screen so it will be redrawn with the
//  new person in the correct order.
//
void CMainWindow::OnAdd()
{
	CPerson* person=new CPerson();
	
	CEditDialog dlgAdd( person, this );
	if ( dlgAdd.DoModal() == IDOK )
	{
		m_people.AddPerson( person );
		OnSize( 0, m_cxClient, m_cyClient );
	}
	else
		delete person;
}

//////////////////////////////////////////////////
//  CMainWindow::OnDelete
//  Deletes the current selection.  Check to see if the selection is
//  now past then end of the list.  Also call OnSize since the list
//  length has now changed.
//
void CMainWindow::OnDelete()
{
	if ( m_nSelectLine == -1 )
	{
		MessageBox( "Select a person to delete first" );
		return;
	}
	m_people.DeletePerson( m_nSelectLine );
	if ( m_nSelectLine >= (int)m_people.GetCount() )
		m_nSelectLine--;
	OnSize( 0, m_cxClient, m_cyClient );
}

////////////////////////////////////////////////////
//  CMainWindow::OnFind
//  Gets information from the CFindDialog modal dialog box, then searches for
//  matching people.  Note the Add and Delete menu items are disabled after
//  a find is made.  Find All is enabled.
//
void CMainWindow::OnFind()
{
	CFindDialog dlgFind( this );
	if ( dlgFind.DoModal() == IDOK &&
			dlgFind.GetFindString().GetLength() != 0 )
	{
		if ( m_people.DoFind( dlgFind.GetFindString() ) )
		{
			m_nSelectLine = -1;
			CString tmp;
			tmp = m_people.GetTitle() + " Found: "
					+ dlgFind.GetFindString();
			SetWindowText( tmp );
			CMenu* pMenu = GetMenu();
			pMenu->EnableMenuItem( IDM_FINDALL, MF_ENABLED );
			pMenu->EnableMenuItem( IDM_FIND, MF_GRAYED );
			pMenu->EnableMenuItem( IDM_DELETE, MF_GRAYED );
			pMenu->EnableMenuItem( IDM_ADD, MF_GRAYED );
			OnSize( 0, m_cxClient, m_cyClient );
		}
		else
			MessageBox( "No match found in list." );
	}
}

////////////////////////////////////////////////////
//  CMainWindow::OnFindAll
//  Returns to view the whole database.  Add, Delete are re-enabled, and
//  Find All is again disabled.  OnSize is called because the list
//  has changed length.
//
void CMainWindow::OnFindAll()
{
	m_people.DoFind();
	SetWindowText( m_people.GetTitle() );
	CMenu* pMenu = GetMenu();
	pMenu->EnableMenuItem( IDM_FINDALL, MF_GRAYED );
	pMenu->EnableMenuItem( IDM_FIND, MF_ENABLED );
	pMenu->EnableMenuItem( IDM_DELETE, MF_ENABLED );
	pMenu->EnableMenuItem( IDM_ADD, MF_ENABLED );
	OnSize( 0, m_cxClient, m_cyClient );
}

////////////////////////////////////////////////////
//  CMainWindow::OnEdit
//  Using the member variable m_nSelectLine a CEditDialog is created
//  and filled with the selected person.  If the dialog OK button is
//  used the dialog saves the changes into the object.
//  over any old information.
//
void CMainWindow::OnEdit()
{
	if ( m_nSelectLine == -1 )
	{
		MessageBox( "Select a person to edit first" );
		return;
	}

	// Get a pointer to the person in the list.
	CPerson* pPerson = m_people.GetPerson( m_nSelectLine );
	CPerson tmpPerson = *pPerson;
	
	//Edit the data.
	CEditDialog dlgEdit( &tmpPerson, this );
	
	//if the ok button is pressed redraw the screen
	if ( dlgEdit.DoModal() == IDOK )
	{
		m_people.ReplacePerson( pPerson, tmpPerson );
		InvalidateLine();
	}
}

//////////////////////////////////////////////////
//  CMainWindow::OnHelp
//
void CMainWindow::OnHelp()
{
	if ( !m_people.IsPresent() )
	{
		CModalDialog dlgHelp( "NoData", this );
		dlgHelp.DoModal();
		return;
	}
	
	if ( !m_people.IsNamed() )
	{
		CModalDialog dlgHelp( "NoName", this );
		if ( dlgHelp.DoModal() == IDCANCEL )
			return;
	}
	
	CModalDialog dlgHelp( "Enter", this );
	dlgHelp.DoModal();
}

//////////////////////////////////////////////////
//  CMainWindow::OnAbout
//
void CMainWindow::OnAbout()
{
	CModalDialog dlgAbout( "AboutBox", this );
	dlgAbout.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
//  The Following are WINDOW messages

//////////////////////////////////////////////////
//  CMainWindow::OnCreate
//  Queries the current text metrics to determine char size.
//
int CMainWindow::OnCreate( LPCREATESTRUCT )
{
	TEXTMETRIC tm;

	// Get the text metrics.
	CDC* dc = GetDC();
	dc->GetTextMetrics( &tm );
	ReleaseDC( dc );

	// Decide the statistics on how many rows, etc., we can display.
	m_cxChar = tm.tmAveCharWidth;
	m_cxCaps = ( (tm.tmPitchAndFamily & 1 )? 3 : 2 ) * m_cxChar / 2;
	m_cyChar = tm.tmHeight + tm.tmExternalLeading;
	m_nMaxWidth = ( SIZENAME + SIZEPHONE + 1 ) * m_cxCaps;
	m_nVscrollPos = m_nHscrollPos = 0;

	return 0;
}

//////////////////////////////////////////////////
//  CMainWindow::OnClose
//  Check to see if the current file needs to be saved.  Terminate
//  the database and destory the window.
//
void CMainWindow::OnClose()
{
	if ( !CheckForSave( "File Exit", "Save file before exit?" ) )
		return;
	m_people.Terminate();
	DestroyWindow();
}

//////////////////////////////////////////////////
//  CMainWindow::OnSize
//  When resized, we need to recalculate our scrollbar ranges based on what
//  part of the database is visible.
//
void CMainWindow::OnSize( UINT, int x, int y )
{
	m_cxClient = x;
	m_cyClient = y;

	m_nVscrollMax = max( 0,
			(int)( m_people.GetCount() ) - m_cyClient / m_cyChar );
	m_nVscrollPos = min( m_nVscrollPos, m_nVscrollMax );

	SetScrollRange( SB_VERT, 0, m_nVscrollMax, FALSE );
	SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );

	m_nHscrollMax = max( 0, ( m_nMaxWidth - m_cxClient ) / m_cxChar );
	m_nHscrollPos = min( m_nHscrollPos, m_nHscrollMax );

	SetScrollRange( SB_HORZ, 0, m_nHscrollMax, FALSE );
	SetScrollPos( SB_HORZ, m_nHscrollPos, TRUE );
	Invalidate( TRUE );
}

//////////////////////////////////////////////////
//  CMainWindow::OnVScroll
//  Translate scroll messages into Scroll increments and then
//  checks the current position to determine if scrolling is possible
//
void CMainWindow::OnVScroll( UINT wParam, UINT pos, CScrollBar* )
{
	short nScrollInc;
	
	switch ( wParam )
	{
		case SB_TOP:
			nScrollInc = -m_nVscrollPos;
			break;

		case SB_BOTTOM:
			nScrollInc = m_nVscrollMax - m_nVscrollPos;
			break;
			
		case SB_LINEUP:
			nScrollInc = -1;
			break;

		case SB_LINEDOWN:
			nScrollInc = 1;
			break;

		case SB_PAGEUP:
			nScrollInc = min( -1, -m_cyClient / m_cyChar );
			break;

		case SB_PAGEDOWN:
			nScrollInc = max( 1, m_cyClient / m_cyChar );
			break;

		case SB_THUMBTRACK:
			nScrollInc = pos - m_nVscrollPos;
			break;

		default:
			nScrollInc = 0;
	}

	if ( nScrollInc = max( -m_nVscrollPos,
			min( nScrollInc, m_nVscrollMax - m_nVscrollPos ) ) )
	{
		m_nVscrollPos += nScrollInc;
		ScrollWindow( 0, -m_cyChar * nScrollInc );
		SetScrollPos( SB_VERT, m_nVscrollPos );
		UpdateWindow();
	}
}

//////////////////////////////////////////////////
//  CMainWindow::OnHScroll
//  Translate scroll messages into Scroll increments and then
//  checks the current position to determine if scrolling is possible
//
void CMainWindow::OnHScroll( UINT wParam, UINT pos, CScrollBar* )
{
	int nScrollInc;
	switch ( wParam )
	{
		case SB_LINEUP:
			nScrollInc = -1;
			break;

		case SB_LINEDOWN:
			nScrollInc = 1;
			break;

		case SB_PAGEUP:
			nScrollInc = -PAGESIZE;
			break;

		case SB_PAGEDOWN:
			nScrollInc = PAGESIZE;
			break;

		case SB_THUMBPOSITION:
			nScrollInc = pos - m_nHscrollPos;
			break;

		default:
			nScrollInc = 0;
	}

	if ( nScrollInc = max( -m_nHscrollPos,
			 min( nScrollInc, m_nHscrollMax - m_nHscrollPos ) ) )
	{
		m_nHscrollPos += nScrollInc;
		ScrollWindow( -m_cxChar * nScrollInc, 0 );
		SetScrollPos( SB_HORZ, m_nHscrollPos );
		UpdateWindow();
	}
}

//////////////////////////////////////////////////
//  CMainWindow::OnUp
//  Uses Accelerator tables to link the up arrow key to this
//  routine.  Decrements the select line with checking for scrolling
//  and wrapping off the top of the list.
//
//
void CMainWindow::OnUp()
{
	InvalidateLine();
	
	if ( m_nSelectLine <= 0 )
	{
		m_nSelectLine = m_people.GetCount() - 1;
		m_nVscrollPos = max( 0, m_nSelectLine + 1 - ( m_cyClient / m_cyChar ) );
		Invalidate( TRUE );
	}
	else
	{
		m_nSelectLine--;
		if ( m_nSelectLine - m_nVscrollPos < 0 )
			OnVScroll( SB_LINEUP, 0, NULL );
		
		// Selection is off the screen
		if ( m_nSelectLine - m_nVscrollPos > ( m_cyClient / m_cyChar ) )
		{
			m_nVscrollPos = m_nSelectLine + 1 - ( m_cyClient / m_cyChar );
			SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
			Invalidate( TRUE );
		}
		if ( m_nSelectLine - m_nVscrollPos < 0 )
		{
			m_nVscrollPos = m_nSelectLine;
			SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
			Invalidate( TRUE );
		}
	}
	
	InvalidateLine();
}

//////////////////////////////////////////////////
//  CMainWindow::OnDown
//  Uses Accelerator tables to link the down arrow key to this
//  routine.  Inc the select line with checking for scrolling
//  and wrapping off the bottom of the list.
//
void CMainWindow::OnDown()
{
	InvalidateLine();
	
	if ( m_nSelectLine == (int)( m_people.GetCount() - 1 )
			|| m_nSelectLine == -1 )
	{
		m_nSelectLine = 0;
		m_nVscrollPos = 0;
		Invalidate( TRUE );
	}
	else
	{
		m_nSelectLine++;
		if ( ( m_nSelectLine - m_nVscrollPos + 1 ) > ( m_cyClient / m_cyChar ) )
			OnVScroll( SB_LINEDOWN, 0, NULL );
		
		// Selection is off the screen
		if ( ( m_nSelectLine - m_nVscrollPos ) > ( m_cyClient / m_cyChar ) )
		{
			m_nVscrollPos = m_nSelectLine + 1 - ( m_cyClient / m_cyChar );
			SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
			Invalidate( TRUE );
		}
		if ( ( m_nSelectLine - m_nVscrollPos ) < 0 )
		{
			m_nVscrollPos = m_nSelectLine;
			SetScrollPos( SB_VERT, m_nVscrollPos, TRUE );
			Invalidate( TRUE );
		}
	}

	InvalidateLine();
}

//////////////////////////////////////////////////
//  CMainWindow::OnLButtonDown
//  Turns the location of the mouse pointer into a line number
//  and stores that information in m_nSelectLine.  Uses
//  InvalidateLine to cause OnPaint to change the screen.
//
void CMainWindow::OnLButtonDown( UINT, CPoint location )
{
	InvalidateLine();
	
	int pos = m_nVscrollPos + location.y / m_cyChar;
	
	if ( ( m_nSelectLine != pos )  && ( pos < (int)m_people.GetCount() ) )
	{
		m_nSelectLine = pos;
		InvalidateLine();
	}
	else
		m_nSelectLine = -1;
}

//////////////////////////////////////////////////
//  CMainWindow::OnLButtonDblClk
//  Translates mouse left button double click into edit person.
//
void CMainWindow::OnLButtonDblClk( UINT wParam, CPoint location )
{
	if ( m_nSelectLine == -1 )
		OnLButtonDown( wParam, location );
	OnEdit();
}

//////////////////////////////////////////////////
//  CMainWindow::OnKeyDown
//  Translates keyboard input into scroll messages
//
void CMainWindow::OnKeyDown( UINT wParam, UINT, UINT )
{
	switch ( wParam )
	{
		case VK_HOME:
			OnVScroll( SB_TOP, 0, NULL );
			break;
		case VK_END:
			OnVScroll( SB_BOTTOM, 0, NULL );
			break;
		case VK_PRIOR:
			OnVScroll( SB_PAGEUP, 0, NULL );
			break;
		case VK_NEXT:
			OnVScroll( SB_PAGEDOWN, 0, NULL );
			break;
		case VK_LEFT:
			OnHScroll( SB_PAGEUP, 0, NULL );
			break;
		case VK_RIGHT:
			OnHScroll( SB_PAGEDOWN, 0, NULL );
			break;
	}
}

//////////////////////////////////////////////////
//  CMainWindow::OnPaint
//  This routine does all the painting for the screen.
//
void CMainWindow::OnPaint()
{
	
	CPaintDC dc( this );

	// Set the Text and background colors for the DC also create a Brush
	CBrush bBack;
	dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );
	dc.SetBkColor( GetSysColor( COLOR_WINDOW ) );
	bBack.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );

	// Compute the lines that need to be redrawn
	int nStart = max( 0, m_nVscrollPos + dc.m_ps.rcPaint.top / m_cyChar - 1 );
	int nEnd = min( (int)m_people.GetCount(),
				m_nVscrollPos + ( dc.m_ps.rcPaint.bottom / m_cyChar+1 ) );

	// Create a rect the width of the display.
	CRect area( 0, 0, m_cxClient, 0 );
	
	CString szDisplay;
	CPerson* pCurrent;
	int x,y;
	for ( ;nStart < nEnd; nStart++ )
	{
		// if the current line is the select line then change the
		// colors to the highlight text colors.
		if ( m_nSelectLine == nStart )
		{
			bBack.DeleteObject();
			bBack.CreateSolidBrush( GetSysColor( COLOR_HIGHLIGHT ) );
			dc.SetTextColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) );
			dc.SetBkColor( GetSysColor( COLOR_HIGHLIGHT ) );
		}

		// x is the number of characters horz scrolled * the width of
		// char.  y is the current line no. - number of lines scrolled
		// times the height of a line.
		x = m_cxChar * ( -m_nHscrollPos );
		y = m_cyChar * ( nStart - m_nVscrollPos );

		// Set the rect to y and y + the height of the line.  Fill the
		// rect with the background color.
		area.top = y;
		area.bottom = y+ m_cyChar;
		dc.FillRect( area, &bBack );

		// Get the person and build a string with his name.
		pCurrent = m_people.GetPerson( nStart );
		szDisplay = " " + pCurrent->GetLastName() + ", " +
				pCurrent->GetFirstName();

		// Set the dc to write using the point as the left top of the
		// character.  Write the name.
		dc.SetTextAlign( TA_LEFT | TA_TOP );
		dc.TextOut ( x, y,szDisplay, szDisplay.GetLength() );

		// Write the phone number right aligned.
		szDisplay = pCurrent->GetPhoneNumber();
		dc.SetTextAlign ( TA_RIGHT | TA_TOP );
		dc.TextOut ( x + SIZENAME * m_cxCaps, y, szDisplay,
			szDisplay.GetLength() );

		// Write the time.
		szDisplay = pCurrent->GetModTime().Format( "%m/%d/%y %H:%M" );
		dc.TextOut ( x + ( SIZENAME + SIZEPHONE ) * m_cxCaps, y,
			szDisplay, szDisplay.GetLength() );

		// If this is the select line then we need to reset the dc
		// colors back to the original colors.
		if ( m_nSelectLine == nStart )
		{
			bBack.DeleteObject();
			bBack.CreateSolidBrush( GetSysColor( COLOR_WINDOW ) );
			dc.SetTextColor( GetSysColor( COLOR_WINDOWTEXT ) );
			dc.SetBkColor( GetSysColor( COLOR_WINDOW ) );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//  The following are utility routines


//////////////////////////////////////////////////
//  CMainWindow::FileDlg
//  Call the commdlg routine to display File Open or File Save As
//  dialogs.  The setup is the same for either.  If bOpen is TRUE
//  then File Open is display otherwise File Save As is displayed.
//  The File Name and File Title are stored at the string pointer
//  passed in.
//
BOOL CMainWindow::FileDlg( BOOL bOpen, int nMaxFile, LPSTR szFile,
		int nMaxFileTitle, LPSTR szFileTitle )
{
	
	OPENFILENAME of;

	char szDirName[SIZESTRING];
	char szFilter[] = "Phone Book Files (*.pb)\0"
		"*.pb\0"
		"\0";

	szDirName[0] = '.';

	of.lStructSize = sizeof( OPENFILENAME );
	of.hwndOwner = m_hWnd;
	of.lpstrFilter = szFilter;
	of.lpstrCustomFilter = NULL;
	of.nMaxCustFilter = 0L;
	of.nFilterIndex = 1L;
	of.lpstrFile=szFile;
	of.nMaxFile=nMaxFile;
	of.lpstrFileTitle = szFileTitle;
	of.nMaxFileTitle = nMaxFileTitle;
	of.lpstrInitialDir = szDirName;
	of.lpstrTitle = NULL;
	of.nFileOffset = 0;
	of.nFileExtension = 0;
	of.lpstrDefExt = "pb";
	if ( bOpen )
	{
		of.Flags = OFN_HIDEREADONLY;
		return GetOpenFileName( &of );
	}
	else
	{
		of.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		return GetSaveFileName( &of );
	}
}

//////////////////////////////////////////////////
//  CMainWindow::Save
//  Handles any time a file needs to be saved to the disk.
//  Passing in FALSE for name brings up the file save as dialog
//  whether or not the database had a name before.
//
BOOL CMainWindow::Save( BOOL bIsNamed /* = FALSE */ )
{
	CString szFileName, szFileTitle;
	TRY
	{
		if ( bIsNamed )
			m_people.DoSave();
		else
		{
			szFileName = m_people.GetName();
			if ( FileDlg( FALSE, SIZESTRING,
					szFileName.GetBuffer( SIZESTRING ), SIZESTRING,
					szFileTitle.GetBuffer( SIZESTRING ) ) )
			{
				szFileName.ReleaseBuffer();
				m_people.DoSave( szFileName );
				m_people.SetTitle( szFileTitle );
				SetWindowText( m_people.GetTitle() );
			}
			else
				return FALSE;
		}
	}
	CATCH( CFileException, e )
	{
		char ErrorMsg[SIZESTRING];
		sprintf( ErrorMsg,"Saving %s returned a 0x%lx.",
				(const char*)szFileTitle, e->m_lOsError );
		MessageBox( ErrorMsg, "File Open Error" );
	}
	AND_CATCH( CArchiveException, e )
	{
		char ErrorMsg[SIZESTRING];
		sprintf( ErrorMsg,"Reading the %s archive failed.",
				(const char*)szFileTitle );
		MessageBox( ErrorMsg, "File Open Error" );
	}
	END_CATCH
	return TRUE;
}

//////////////////////////////////////////////////
//  CMainWindow::CheckForSave
//  Whenever a new file is opened this routine will determine if
//  there are unsaved changes in the current database.  If so it
//  will query the user and determine save or not as appropriate.
//
BOOL CMainWindow::CheckForSave( const char* pszTitle, const char* pszMessage )
{
	if ( m_people.IsDirty() )
	{
		UINT nButton = MessageBox( pszMessage, pszTitle, MB_YESNOCANCEL );
		if ( nButton == IDYES )
		{
			if ( !Save( m_people.IsNamed() ) )
				return FALSE;
		}
		else if ( nButton == IDCANCEL )
			return FALSE;
	}
	return TRUE;
}


//////////////////////////////////////////////////
//  CMainWindow::SetMenu
//  Whenever the existance of the DataBase is changed this
//  routine will reset the menus so only the possible commands
//  are accessible.
//
void CMainWindow::SetMenu()
{
	CMenu* pMenu = GetMenu();
	if ( m_people.IsPresent() )
	{
		if ( m_people.IsNamed() )
			pMenu->EnableMenuItem( IDM_SAVE, MF_ENABLED );
		else
			pMenu->EnableMenuItem( IDM_SAVE, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_SAVEAS, MF_ENABLED );
		pMenu->EnableMenuItem( IDM_CLOSE, MF_ENABLED );
		pMenu->EnableMenuItem( IDM_PRINT, MF_ENABLED );
		pMenu->EnableMenuItem( IDM_ADD, MF_ENABLED );
		pMenu->EnableMenuItem( IDM_DELETE, MF_ENABLED );
		pMenu->EnableMenuItem( IDM_FIND, MF_ENABLED );
		pMenu->EnableMenuItem( IDM_EDIT, MF_ENABLED );
	}
	else
	{
		pMenu->EnableMenuItem( IDM_SAVE, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_SAVEAS, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_CLOSE, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_PRINT, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_ADD, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_DELETE, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_FIND, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_FINDALL, MF_GRAYED );
		pMenu->EnableMenuItem( IDM_EDIT, MF_GRAYED );
	}
}

//////////////////////////////////////////////////
//  CMainWindow::InvalidateLine
//  Marks the screen area of the currently selected person as
//  invalid causing windows to call OnPaint to redraw the area.
//  This is normally used when the selected line is being changed.
//
void CMainWindow::InvalidateLine()
{
	CRect area( 0, ( m_nSelectLine - m_nVscrollPos ) * m_cyChar, m_cxClient,
				( m_nSelectLine + 1 - m_nVscrollPos ) * m_cyChar );
	InvalidateRect( area );
}
