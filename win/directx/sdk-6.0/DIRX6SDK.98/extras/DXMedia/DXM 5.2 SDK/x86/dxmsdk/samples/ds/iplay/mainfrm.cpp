// This code and information is provided "as is" without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.

// Copyright (C) 1996 - 1997 Intel corporation.  All rights reserved.

// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "IPlay.h"
#include "IPlayDoc.h"

#include "mainfrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_MEDIA_INDEO, OnMediaIndeo)
	ON_UPDATE_COMMAND_UI(ID_MEDIA_INDEO, OnUpdateMediaIndeo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars
	
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_OPEN,
		ID_SEPARATOR,
	ID_MEDIA_PLAY,
	ID_MEDIA_PAUSE,
	ID_MEDIA_STOP,
		ID_SEPARATOR,
	ID_MEDIA_LOOP,
	ID_MEDIA_ZOOMX2,
		ID_SEPARATOR,
		ID_SEPARATOR,
		ID_SEPARATOR,
	ID_MEDIA_INDEO
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    EnableDocking(CBRS_FLOAT_MULTI);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

    m_wndToolBar.SetSizes( CSize( 40, 40 ), CSize( 32, 32 ) );
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);

	// Enable tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	// Position window in upper, left corner
    SetWindowPos( NULL, 0, 0, 424, 96, SWP_NOZORDER );
	m_bIndeo = FALSE;
	return 0;
}



/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers



BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// Set optimum size for view
	cs.cx = 422;
	cs.cy = 524;

	cs.style = WS_OVERLAPPED | WS_CAPTION |
	           WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX;

	return CFrameWnd::PreCreateWindow(cs);
}


void CMainFrame::OnMediaIndeo() 
{
	m_bIndeo = !m_bIndeo;
	if (m_bIndeo) 
		SetWindowPos(NULL, 0, 0, 422, 524, SWP_NOMOVE | SWP_NOZORDER );
	else
		SetWindowPos(NULL, 0, 0, 422, 96, SWP_NOMOVE | SWP_NOZORDER );
	
}

void CMainFrame::OnUpdateMediaIndeo(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_bIndeo );
	
}
