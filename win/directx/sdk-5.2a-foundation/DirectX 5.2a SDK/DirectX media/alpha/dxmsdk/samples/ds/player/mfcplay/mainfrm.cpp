//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
// mainfrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "mfcplay.h"

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
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars
	
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_MEDIA_PLAY,
	ID_MEDIA_PAUSE,
		ID_SEPARATOR,
	ID_MEDIA_STOP,
};

const int nButtons = sizeof(buttons)/sizeof(UINT);
const int nButtonImageWidth = 32;
const int nButtonImageHeight = 32;
const int nButtonBorder = 8;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
    sizeImage = CSize( nButtonImageWidth, nButtonImageHeight );
    sizeButton = sizeImage + CSize( nButtonBorder, nButtonBorder );
	
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
		!m_wndToolBar.SetButtons(buttons, nButtons))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}


    m_wndToolBar.SetSizes( sizeButton, sizeImage );

    SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOMOVE );

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX;

	return CFrameWnd::PreCreateWindow(cs);
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

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
    // Find out the default sizes
	CFrameWnd::OnGetMinMaxInfo(lpMMI);

    // Our client area is going to be the toolbar
    CRect rectClient =
        CRect( CPoint(0, 0), CSize(sizeButton.cx * nButtons, sizeButton.cy) );

    // Adjust for a border around the buttons
    rectClient.BottomRight().y += nButtonBorder;

    // The windows API AdjustWindowRect (and thus Cwnd::CalcWindowRect) gets its
    // sums wrong when you have a WS_OVERLAPPED style. The following line gets
    // around this.
    AdjustWindowRect( &rectClient, WS_CAPTION|WS_THICKFRAME, TRUE );

    CSize sizeRequired = rectClient.BottomRight() - rectClient.TopLeft();

    lpMMI->ptMinTrackSize = CPoint( sizeRequired );
    lpMMI->ptMaxTrackSize.y = sizeRequired.cy;
}
