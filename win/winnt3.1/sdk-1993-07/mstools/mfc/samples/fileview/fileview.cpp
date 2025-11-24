// fileview.cpp : Defines the class behaviors for the application.
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

#include "fileview.h"

#include "afxdlgs.h"

#define SIZESTRING  256        // max characters in string

/////////////////////////////////////////////////////////////////////////////

// theApp:
// Just creating this application object runs the whole application.
//
CTheApp theApp;

/////////////////////////////////////////////////////////////////////////////

// CMainWindow constructor:
// Create the window with the appropriate style, size, menu, etc.
//
CMainWindow::CMainWindow()
{
	VERIFY(LoadAccelTable( "MainAccelTable" ));
	VERIFY(Create( NULL, "FileView Application",
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
		rectDefault, NULL, "MainMenu" ));
}

CMainWindow::~CMainWindow()
{
	if (m_pMyFile != NULL)
	{
		m_pMyFile->Close();
		delete m_pMyFile;
	}
}
// OnCreate
//

int CMainWindow::OnCreate( LPCREATESTRUCT /* lpcs */)
{
	TEXTMETRIC tm;
	CWindowDC dc( this );

	dc.SelectStockObject( SYSTEM_FIXED_FONT );
	dc.GetTextMetrics( &tm );

	m_nCxChar = tm.tmAveCharWidth;
	m_nCyChar = tm.tmHeight + tm.tmExternalLeading;

	m_nVScrollPos = 0;
	m_nHScrollPos = 0;
	m_lTopLine = 0L;
	m_pMyFile = NULL;

	SetScrollRange( SB_VERT, 0, SCROLLMAX, FALSE );
	SetScrollRange( SB_HORZ, 0, SIZESTRING, FALSE );

	return 0;
}


// OnPaint
//

void CMainWindow::OnPaint()
{
	char        acBuf[ SIZESTRING ];
	CPaintDC    dc( this );
	CRect       rect, rect2;
	int         y;
	char*       pc;

	GetClientRect( &rect );
	dc.SetTextAlign( TA_LEFT );
	dc.SelectStockObject( SYSTEM_FIXED_FONT );
	dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
	if ( m_pMyFile != NULL )
	{
		TRY
		{
			m_pMyFile->Seek( m_lTopLine, CFile::begin );
		}
		CATCH( CFileException, e )
		{
			TRACE( "Bad Seek in OnPaint %ld\n", m_lTopLine );
		}
		END_CATCH

		rect2.left  = rect.left;
		rect2.right = rect.right;

		m_nLinesPainted = 0;
		for ( y = m_nCxChar/2; y < rect.bottom; y += m_nCyChar )
		{
			m_pMyFile->NextLine( acBuf, SIZESTRING );

			pc = acBuf;
			if ( (int)strlen(acBuf) < m_nHScrollPos)
			{
				*acBuf = 0;
			}
			else
			{
				pc += m_nHScrollPos;
			}

			rect2.top = y;
			rect2.bottom = y + m_nCyChar;
			dc.ExtTextOut( m_nCyChar/2, rect2.top, ETO_OPAQUE, &rect2,
						   pc, strlen(pc), NULL );
			m_nLinesPainted++;
		}
	}
}

// OnAbout:
//

void CMainWindow::OnAbout()
{
	CModalDialog about( "AboutBox", this );
	about.DoModal();
}

// OnExit:
//

void CMainWindow::OnExit()
{
	DestroyWindow();
}

// OnOpen:
//

void CMainWindow::OnOpen()
{
	CString strFileName, strFileTitle;

	//
	// First close any open file
	//

	if ( m_pMyFile != NULL )
	{
		m_pMyFile->Close();
		delete m_pMyFile;
		m_pMyFile = NULL;
		SetWindowText( "No File" );
		Invalidate( TRUE );
		m_lTopLine = 0L;
		m_nVScrollPos = 0;
		SetScrollPos( SB_VERT, m_nVScrollPos, TRUE );
		m_nHScrollPos = 0;
		SetScrollPos( SB_HORZ, m_nHScrollPos, TRUE );
	}

	TRY
	{
		BOOL bValidFileName = FALSE;
		BOOL bStringsReleased = FALSE;

		if (FileDlg(TRUE, SIZESTRING, strFileName.GetBuffer(SIZESTRING),
				SIZESTRING, strFileTitle.GetBuffer(SIZESTRING)))
		{
			strFileName.ReleaseBuffer();
			strFileTitle.ReleaseBuffer();
			bStringsReleased = TRUE;
			bValidFileName = TRUE;
		}

		if (!bStringsReleased)
		{
			strFileName.ReleaseBuffer();
			strFileTitle.ReleaseBuffer();
		}

		if (bValidFileName)
		{
			//
			// try to open the file here
			//
			m_pMyFile = new CLineFile( strFileName,
										CFile::modeRead | CFile::typeBinary);
			m_lFileSize = m_pMyFile->GetLength();

			// check to make sure it is a text file
			BYTE byBuf[128];
			for (int iby = m_pMyFile->Read(&byBuf, 128); --iby >= 0;)
				if (byBuf[iby] > 128)
				{
					m_pMyFile->Close();
					delete m_pMyFile;
					m_pMyFile = NULL;
					m_lFileSize = 0L;
					MessageBox("File contains non-printable characters", 
						"Error", MB_OK);
					return;
				}
			m_pMyFile->SeekToBegin();

			SetWindowText( strFileTitle );
			Invalidate(TRUE);
		}
	}
	CATCH( CFileException, e )
	{
		char ErrorMsg[ 80 ];
		sprintf( ErrorMsg,"Opening %s returned a 0x%lx.",
				(const char*)strFileTitle, e->m_lOsError );
		MessageBox( ErrorMsg, "File Open Error" );
	}
	END_CATCH
}

//
// OnVScroll:

void CMainWindow::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* /* pScrollBar */)
{
	char        acBuf[SIZESTRING];
	LONG        oldLine = m_lTopLine;
	LONG        l;

	if ( m_pMyFile == NULL )
	{
		return;
	}

	TRY
	{
		switch ( nSBCode )
		{
		case SB_LINEUP:
			if ( m_lTopLine == 0L )
			{
				break;
			}
			m_pMyFile->SetBegin( m_lTopLine );
			m_lTopLine = m_pMyFile->BackLines( acBuf, SIZESTRING, 1 );
			break;

		case SB_LINEDOWN:
			m_pMyFile->Seek( m_lTopLine, CFile::begin);
			m_lTopLine = m_pMyFile->NextLine( acBuf, SIZESTRING );
			break;

		case SB_PAGEUP:
			if ( m_lTopLine == 0L )
			{
				break;
			}
			m_pMyFile->SetBegin( m_lTopLine );
			m_lTopLine = m_pMyFile->BackLines( acBuf, SIZESTRING,
											   m_nLinesPainted );
			break;

		case SB_PAGEDOWN:
			m_lTopLine = m_pMyFile->GetPosition();
			break;

		case SB_BOTTOM:
			nPos = 1000;
			goto ThumbGoTo;

		case SB_TOP:
			nPos = 0;
			// fall thru

		case SB_THUMBPOSITION:
	ThumbGoTo:

			m_nVScrollPos = nPos;
			if ( m_lFileSize > 40000L )
			{
				l = ( m_lFileSize / 1000L ) * nPos;
			}
			else
			{
				l = ( m_lFileSize * nPos ) / 1000L;
			}

			m_lTopLine = m_pMyFile->LineNear( acBuf, SIZESTRING, l );
			break;

		default:
			return;
		}
	}
	CATCH( CFileException, e )
	{
		TRACE( "Bad Seek in OnVScroll\n" );
		m_lTopLine = 0L;
	}
	END_CATCH

	if ( m_lTopLine < 0L )
	{
		m_lTopLine = 0L;
	}

	if ( m_lFileSize > 40000 )
	{
		m_nVScrollPos = (short)(m_lTopLine  / ( m_lFileSize / 1000L ));
	}
	else
	{
		m_nVScrollPos = (short)(m_lTopLine * 1000L / m_lFileSize);
	}

	if ( m_nVScrollPos < 0 )
	{
		m_nVScrollPos = 0;
	}
	if ( m_nVScrollPos > SCROLLMAX )
	{
		m_nVScrollPos = SCROLLMAX;
	}

	SetScrollPos( SB_VERT, m_nVScrollPos, TRUE );

	if ( m_lTopLine != oldLine )
	{
		Invalidate( FALSE );
	}
}

// OnHScroll:
//

void
CMainWindow::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* /* pScrollBar */)
{
	if ( m_pMyFile == NULL )
	{
		return;
	}

	switch ( nSBCode )
	{
	case SB_LINEUP:
		m_nHScrollPos -= 1;
		break;

	case SB_LINEDOWN:
		m_nHScrollPos += 1;
		break;

	case SB_PAGEUP:
		m_nHScrollPos -= 10;
		break;

	case SB_PAGEDOWN:
		m_nHScrollPos += 10;
		break;

	case SB_TOP:
		nPos = 0;
		// fall thru
	case SB_THUMBPOSITION:
		m_nHScrollPos = nPos;
		break;

	default:
		return;
	}

	if ( m_nHScrollPos < 0 )
	{
		m_nHScrollPos = 0;
	}

	if ( m_nHScrollPos > SIZESTRING )
	{
		m_nHScrollPos = SIZESTRING;
	}

	SetScrollPos( SB_HORZ, m_nHScrollPos, TRUE );
	Invalidate( FALSE );
}

//
// catch arrow keys and simulate touching scroll bars
//
void
CMainWindow::OnKeyDown( UINT wChar, UINT /* nRepCnt */, UINT /* wFlags */)
{
	switch( wChar )
	{
	case VK_HOME:
		SendMessage( WM_VSCROLL, SB_TOP, 0L);
		SendMessage( WM_HSCROLL, SB_TOP, 0L);
		break;

	case VK_END:
		SendMessage( WM_VSCROLL, SB_BOTTOM, 0L);
		break;

	case VK_PRIOR:
		SendMessage( WM_VSCROLL, SB_PAGEUP, 0L);
		break;

	case VK_NEXT:
		SendMessage( WM_VSCROLL, SB_PAGEDOWN, 0L);
		break;

	case VK_UP:
		SendMessage( WM_VSCROLL, SB_LINEUP, 0L);
		break;

	case VK_DOWN:
		SendMessage( WM_VSCROLL, SB_LINEDOWN, 0L);
		break;

	case VK_RIGHT:
		SendMessage( WM_HSCROLL, SB_LINEDOWN, 0L);
		break;

	case VK_LEFT:
		SendMessage( WM_HSCROLL, SB_LINEUP, 0L);
		break;
	}
}

BOOL
CMainWindow::FileDlg( BOOL bOpen, int nMaxFile, LPSTR szFile,
		int nMaxFileTitle, LPSTR szFileTitle )
{
	char szFilter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(bOpen, "txt", szFile, OFN_HIDEREADONLY, szFilter);

	// This example shows how to access the OPENFILENAME struct
	// directly.
	dlg.m_ofn.lpstrFile = szFile;
	dlg.m_ofn.lpstrFileTitle = szFileTitle;
	dlg.m_ofn.nMaxFileTitle = nMaxFileTitle;
	
	return dlg.DoModal() == IDOK ? TRUE : FALSE;
}

// CMainWindow message map:
// Associate messages with member functions.
//
// It is implied that the ON_WM_PAINT macro expects a member function
// "void OnPaint()".
//
// It is implied that members connected with the ON_COMMAND macro
// receive no arguments and are void of return type, e.g., "void OnAbout()".
//
BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )
	ON_WM_PAINT()
	ON_COMMAND( IDM_ABOUT, OnAbout )
	ON_COMMAND( IDM_OPEN,  OnOpen )
	ON_COMMAND( IDM_EXIT,  OnExit )
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTheApp

// InitInstance:
// When any CTheApp object is created, this member function is automatically
// called.  Any data may be set up at this point.
//
// Also, the main window of the application should be created and shown here.
// Return TRUE if the initialization is successful.
//
BOOL CTheApp::InitInstance()
{
	m_pMainWnd = new CMainWindow();
	m_pMainWnd->ShowWindow( m_nCmdShow );
	m_pMainWnd->UpdateWindow();

	return TRUE;
}
