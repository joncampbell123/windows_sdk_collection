// hello.cpp : Defines the class behaviors for the Hello child window.
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
								   RGB (255, 255, 255) } ;

/////////////////////////////////////////////////////////////////////////////
// CHelloWnd Member Functions


BEGIN_MESSAGE_MAP(CHelloWnd, CMDIChildWnd)
	ON_WM_CREATE()
	ON_COMMAND(IDM_BLACK, OnColor)
	ON_COMMAND(IDM_RED, OnColor)
	ON_COMMAND(IDM_GREEN, OnColor)
	ON_COMMAND(IDM_BLUE, OnColor)
	ON_COMMAND(IDM_WHITE, OnColor)
	ON_COMMAND(IDM_CUSTOM, OnColor)
	ON_WM_PAINT()
	ON_WM_MDIACTIVATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// Create:
// Register a custom WndClass and create a window.
// This must be done because CHelloWnd has a custom icon.
// 
BOOL CHelloWnd::Create(LPCSTR szTitle, LONG style /* = 0 */,
	const RECT& rect /* = rectDefault */,
	CMDIFrameWnd* parent /* = NULL */)
{
	const char* pszHelloClass = 
		  AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW,
			LoadCursor(NULL, IDC_ARROW), 
			(HBRUSH) (COLOR_WINDOW+1),
			LoadIcon(AfxGetInstanceHandle(), "hello"));

	return CMDIChildWnd::Create(pszHelloClass, szTitle, style, rect, parent);
}


// Constructor
// Do minimum initialization
//
CHelloWnd::CHelloWnd()
{
	m_nColor = 0;
	m_clrText = 0;
	m_pMenuCurrent = NULL;
	m_bWindowActive = FALSE;
}

// Destructor:
// Clean up menu iff Windows won't
//
CHelloWnd::~CHelloWnd()
{ 
	if (m_bWindowActive)
	{
		// Suppress Foundation DestroyMenu done in CMenu destructor 
		// (Windows takes care of menu cleanup for the active window)
		//
		m_pMenuCurrent->Detach();
	}
}

// OnCreate:
// Set up colors -- this also could have been done in constructor
//
int CHelloWnd::OnCreate(LPCREATESTRUCT /* p */)
{
	m_nColor = IDM_BLACK;
	m_clrText = RGB (0, 0, 0);

	return 0;
}

// OnDestroy:
// Notify app main MDI frame window of destruction so it may
// do some cleanup.  Note: this uses a custom message -- see
// mdi.h and mdi.cpp for the custom message handler
//
void CHelloWnd::OnDestroy()
{
	m_pMDIFrameWnd->SendMessage(WM_CHILDDESTROY, (UINT)m_hWnd, 0);
}

// OnColor:
// Change menu checkmarks to indicate the newly selected color.
//
void CHelloWnd::OnColor()
{
	CMenu* pMenu = m_pMDIFrameWnd->GetMenu();
	pMenu->CheckMenuItem(m_nColor, MF_UNCHECKED);

	m_nColor = GetCurrentMessage()->wParam;
	pMenu->CheckMenuItem(m_nColor, MF_CHECKED);

	if (m_nColor != IDM_CUSTOM)
		m_clrText = clrTextArray[m_nColor - IDM_BLACK];
	else
	{
		CColorDialog dlgColor(m_clrText);
		if (dlgColor.DoModal() == IDOK)
			m_clrText = dlgColor.GetColor();
	}

	// Force the client area text to be repainted in the new color

	Invalidate();
}

// OnPaint:
// Draw a string in the center of the client area.
//
void CHelloWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;

	dc.SetTextColor(m_clrText);
	dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
	GetClientRect(rect);
	dc.DrawText("Hello, World!", -1, rect, 
		DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}


// OnMDIActivate:
// This window is being activated or deactivated.  
// Switch the menus appropriately.
//
void CHelloWnd::OnMDIActivate(BOOL bActivate, CWnd* /*pActive*/, 
							  CWnd* /*pDeActive*/)
{
	CMDIFrameWnd* pFrame = m_pMDIFrameWnd;
	CMenu* pWinPopupMenu = NULL;
	CMenu* pMenu = NULL;

	m_bWindowActive = bActivate;

	if (bActivate)
	{
		pMenu = new CMenu;
		pMenu->LoadMenu("MdiMenuHello");
		pWinPopupMenu = pMenu->GetSubMenu(HELLO_MENU_POS);

		CMenu* pLastMenu = pFrame->MDISetMenu(pMenu, pWinPopupMenu);
		pLastMenu->DestroyMenu();

		pMenu->CheckMenuItem(m_nColor, MF_CHECKED);

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
