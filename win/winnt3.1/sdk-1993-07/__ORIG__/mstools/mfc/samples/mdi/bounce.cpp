// bounce.cpp : Defines the class behaviors for the Bounce child window.
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
//

#include "mdi.h"

static COLORREF clrTextArray[] = { RGB (0,   0, 0), RGB (255, 0,   0),
								   RGB (0, 255, 0), RGB (  0, 0, 255),
								   RGB (255, 255, 255) };

#define ABS(x) ((x) < 0? -(x) : (x) > 0? (x) : 0)


/////////////////////////////////////////////////////////////////////////////
// CBounceWnd

BEGIN_MESSAGE_MAP(CBounceWnd, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MDIACTIVATE()
	ON_COMMAND(IDM_BLACK, OnColor)
	ON_COMMAND(IDM_RED,   OnColor)
	ON_COMMAND(IDM_GREEN, OnColor)
	ON_COMMAND(IDM_BLUE,  OnColor)
	ON_COMMAND(IDM_WHITE, OnColor)
	ON_COMMAND(IDM_CUSTOM, OnColor)
	ON_COMMAND(IDM_SLOW,  OnSpeed)
	ON_COMMAND(IDM_FAST,  OnSpeed)
END_MESSAGE_MAP()

// Create:
// Register a custom WndClass and create a window.
// This must be done because CBounceWnd has a custom cursor.
// 
BOOL CBounceWnd::Create(LPCSTR szTitle, LONG style /* = 0 */,
	const RECT& rect /* = rectDefault */,
	CMDIFrameWnd* parent /* = NULL */)
{
	const char* pszBounceClass = 
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
			LoadCursor(NULL, IDC_UPARROW), 
			(HBRUSH)(COLOR_WINDOW+1),
			NULL);

	return CMDIChildWnd::Create(pszBounceClass, szTitle, style, rect, parent);
}

// Constructor:
// Minimum initialization
//
CBounceWnd::CBounceWnd()
{
	m_pBitmap = NULL;
	m_pMenuCurrent = NULL;
	m_bWindowActive = FALSE;
}

// Destructor:
// Release Windows resources
//
CBounceWnd::~CBounceWnd()
{
	if (m_bWindowActive)
	{
		// Suppress Foundation DestroyMenu done in CMenu destructor 
		// (Windows takes care of menu cleanup for the active window)
		//
		m_pMenuCurrent->Detach();
	}

	delete m_pBitmap;

	if (m_nSpeed != 0)
		KillTimer(1);
}


// OnCreate:
// Set up the ball parameters and start a timer.
//
int CBounceWnd::OnCreate(LPCREATESTRUCT /* p */)
{
	m_nSpeed = IDM_SLOW;
	if (!SetTimer(1, (m_nSpeed==IDM_SLOW? 100 : 0), NULL))
	{
		MessageBox("Not enough timers available for this window.",
				"MDI:Bounce", MB_ICONEXCLAMATION | MB_OK);

		m_nSpeed = 0;

		// signal creation failure...
		return -1;
	}
	else
	{
		m_nColor = IDM_BLACK;
		m_clrBall = clrTextArray[m_nColor - IDM_BLACK];

		CDC* pDC = GetDC();
		m_xPixel = pDC->GetDeviceCaps(ASPECTX);
		m_yPixel = pDC->GetDeviceCaps(ASPECTY);
		ReleaseDC(pDC);

	// Note that we could call MakeNewBall here (which should be called
	// whenever the ball's speed, color or size has been changed), but the
	// OnSize member function is always called after the OnCreate. Since
	// the OnSize member has to call MakeNewBall anyway, we don't here.
	//

	}

	return 0;
}

// OnDestroy:
// Notify app main MDI frame window of destruction so it may
// do some cleanup.  Note: this uses a custom message -- see
// mdi.h and mdi.cpp for the custom message handler
//
void CBounceWnd::OnDestroy()
{
	m_pMDIFrameWnd->SendMessage(WM_CHILDDESTROY, (UINT)m_hWnd, 0);
}

// MakeNewBall:
// Whenever a parameter changes which would affect the speed or appearance
// of the ball, call this to regenerate the ball bitmap.
//
void CBounceWnd::MakeNewBall()
{
	if (m_pBitmap != NULL)
	{
		// Reclaim Windows resources associated with the ball bitmap
		// before redrawing

		m_pBitmap->DeleteObject();
	}
	else
	{
		m_pBitmap = new CBitmap;
	}

	m_cxTotal = (m_cxRadius + ABS(m_cxMove)) << 1;
	m_cyTotal = (m_cyRadius + ABS(m_cyMove)) << 1;

	CDC dcMem;
	CDC* pDC = GetDC();
	
	dcMem.CreateCompatibleDC(pDC);

	m_pBitmap->CreateCompatibleBitmap(pDC, m_cxTotal, m_cyTotal);

	ASSERT(m_pBitmap->m_hObject != NULL);

	ReleaseDC(pDC);


	CBitmap* pOldBitmap = dcMem.SelectObject(m_pBitmap);

	// draw a rectangle in the background (window) color
	CRect rect(0, 0, m_cxTotal, m_cyTotal);
	CBrush brBackground(::GetSysColor(COLOR_WINDOW));
	dcMem.FillRect(rect, &brBackground);

	CBrush brCross(HS_DIAGCROSS, 0L);
	CBrush* pOldBrush = dcMem.SelectObject(&brCross);

	dcMem.SetBkColor(m_clrBall);
	dcMem.Ellipse(ABS(m_cxMove), ABS(m_cyMove),
					m_cxTotal - ABS(m_cxMove), m_cyTotal - ABS(m_cyMove));

	dcMem.SelectObject(pOldBrush);
	dcMem.SelectObject(pOldBitmap);
	dcMem.DeleteDC();
}

// OnSize:
// The ball's size and displacement change according to the window size.
//
void CBounceWnd::OnSize(UINT nType, int cx, int cy)
{
	LONG lScale;

	m_xCenter = (m_cxClient = cx) >> 1;
	m_yCenter = (m_cyClient = cy) >> 1;
	m_xCenter += m_cxClient >> 3; // make the ball a little off-center

	lScale = min((LONG)m_cxClient * m_xPixel,
		(LONG)m_cyClient * m_yPixel) >> 4;
	m_cxRadius = (short)(lScale / m_xPixel);
	m_cyRadius = (short)(lScale / m_yPixel);
	m_cxMove = max(1, m_cyRadius >> 2);
	m_cyMove = max(1, m_cyRadius >> 2);

	MakeNewBall();

	CMDIChildWnd::OnSize(nType, cx, cy);
}

// OnColor:
// The ball's color needs to be changed.  Checkmark the right color on the
// menu.
//
void CBounceWnd::OnColor()
{
	CMenu* pMenu = m_pMDIFrameWnd->GetMenu();
	pMenu->CheckMenuItem(m_nColor, MF_UNCHECKED);

	m_nColor = GetCurrentMessage()->wParam;
	pMenu->CheckMenuItem(m_nColor, MF_CHECKED);

	if (m_nColor != IDM_CUSTOM)
		m_clrBall = clrTextArray[m_nColor - IDM_BLACK];
	else
	{
		CColorDialog dlgColor(m_clrBall);
		if (dlgColor.DoModal() == IDOK)
			m_clrBall = dlgColor.GetColor();
	}

	MakeNewBall();

	Invalidate();
}

// OnSpeed:
// The ball's speed needs to be changed.  Checkmark the menus, kill the
// current timer and start a new one at the new speed.
//
void CBounceWnd::OnSpeed()
{
	CMenu* pMenu = m_pMDIFrameWnd->GetMenu();
	pMenu->CheckMenuItem(m_nSpeed, MF_UNCHECKED);

	m_nSpeed = GetCurrentMessage()->wParam;
	pMenu->CheckMenuItem(m_nSpeed, MF_CHECKED);

	KillTimer(1);
	if (!SetTimer(1, (m_nSpeed==IDM_SLOW? 100 : 0), NULL))
	{
		MessageBox("Not enough timers available for this window.",
				"MDI:Bounce", MB_ICONEXCLAMATION | MB_OK);

		m_nSpeed = 0;

		DestroyWindow();
	}
}

// OnTimer:
// Animate the ball.
//
void CBounceWnd::OnTimer(UINT /* wParam */)
{
	if (m_pBitmap != NULL)
	{
		CDC dcMem;
		CDC* pdcScreen = NULL;
		CBitmap* pOldMap = NULL;

		pdcScreen = GetDC();    

		dcMem.CreateCompatibleDC(pdcScreen);

		ASSERT(m_pBitmap->m_hObject != NULL);

		pOldMap = dcMem.SelectObject(m_pBitmap);
	
		pdcScreen->BitBlt(m_xCenter - m_cxTotal / 2,
					m_yCenter - m_cyTotal / 2, m_cxTotal, m_cyTotal,
					&dcMem, 0, 0, SRCCOPY);
	
		ReleaseDC(pdcScreen);
	
		m_xCenter += m_cxMove;
		m_yCenter += m_cyMove;
	
		if ((m_xCenter + m_cxRadius >= m_cxClient) || 
			(m_xCenter - m_cxRadius <= 0))
		{
			m_cxMove = -m_cxMove;
		}
	
		if ((m_yCenter + m_cyRadius >= m_cyClient) || 
			(m_yCenter - m_cyRadius <= 0))
		{
			m_cyMove = -m_cyMove;
		}
	
		dcMem.SelectObject(pOldMap);
		dcMem.DeleteDC();
	}
}

// OnMDIActivate:
// The current window is being activated or deactivated.  
// Change MDI frame window menu as appropriate.
//

void CBounceWnd::OnMDIActivate(BOOL bActivate, CWnd* /*pActive*/, 
							   CWnd* /*pDeActive*/)
{

	CMDIFrameWnd* pFrame = m_pMDIFrameWnd;
	CMenu* pWinPopupMenu = NULL;
	CMenu* pMenu = NULL;

	m_bWindowActive = bActivate;

	if (bActivate)
	{
		pMenu = new CMenu;
		pMenu->LoadMenu("MdiMenuBounce");
		pWinPopupMenu = pMenu->GetSubMenu(BOUNCE_MENU_POS);

		CMenu* pLastMenu = pFrame->MDISetMenu(pMenu, pWinPopupMenu);
		pLastMenu->DestroyMenu();

		pMenu->CheckMenuItem(m_nColor, bActivate ? MF_CHECKED : MF_UNCHECKED);
		pMenu->CheckMenuItem(m_nSpeed, bActivate ? MF_CHECKED : MF_UNCHECKED);

		delete m_pMenuCurrent;
		m_pMenuCurrent = pMenu;

	}
	else
	{
		pMenu = new CMenu;
		pMenu->LoadMenu("MdiMenuInit");
		pWinPopupMenu = pMenu->GetSubMenu(INIT_MENU_POS);

		CMenu* pLastMenu = pFrame->MDISetMenu(pMenu, pWinPopupMenu);
		pLastMenu->DestroyMenu();

		delete m_pMenuCurrent;
		m_pMenuCurrent = pMenu;
	}

	pFrame->DrawMenuBar();
}
