// mdi.cpp : Defines the class behaviors for the MDI sample application.
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

/////////////////////////////////////////////////////////////////////////////

#include "mdi.h"

// Create one global CTheApp object.  Once created, it takes care of itself.
//
CTheApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainWindow

// CMainWindow message map:
// Note: certain built-in MDI window behavior (tiling and cascading child
// windows, arranging icons, switching to the next child window) must be
// linked to user menu items explicitly by the developer (since MFC cannot
// know an application's menu structure ahead of time).  MDICascade, 
// MDITile, MDINext, MDIIconArrange are all CMDIFrameWnd member functions.
//
BEGIN_MESSAGE_MAP(CMainWindow, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_COMMAND(IDM_HELLO, OnNewHello)
	ON_COMMAND(IDM_BOUNCE, OnNewBounce)
	ON_COMMAND(IDM_CASCADE, MDICascade)
	ON_COMMAND(IDM_TILE, MDITile)
	ON_COMMAND(IDM_NEXT, MDINext)
	ON_COMMAND(IDM_ARRANGE, MDIIconArrange)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_MESSAGE(WM_CHILDDESTROY, OnChildDestroy)
END_MESSAGE_MAP()


// CMainWindow constructor:
//
CMainWindow::CMainWindow()
{
	VERIFY(LoadAccelTable("MdiAccel"));
	Create(NULL, "MDI Sample Application", WS_OVERLAPPEDWINDOW, rectDefault, 
		   NULL, "MdiMenuInit");
}

// OnCreate:
// Load application's initial MDI frame menu and create an MDI client
//
int CMainWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{ 
	m_pMenuInit = new CMenu();
	m_pMenuInit->LoadMenu("MdiMenuInit");
	CreateClient(lpCreateStruct, m_pMenuInit->GetSubMenu(0));

	return 0;
}

// OnAbout:
// Display this application's about box (defined in about.dlg).
//
void CMainWindow::OnAbout()
{
	CModalDialog about("AboutBox", this);
	about.DoModal();
}

// OnNewHello:
// Create a new Hello child window.
//
void CMainWindow::OnNewHello()
{
	CHelloWnd *pHelloWnd = new CHelloWnd;
	if (!pHelloWnd->Create("Hello", 0, rectDefault, this))
	{
		delete pHelloWnd;       // HWND not created
		return;
	}
	pHelloWnd->ShowWindow(SW_SHOW);
	// the default PostNcDestroy handler will delete this object when destroyed
}

// OnNewBounce:
// Create a new Bounce child window.
//
void CMainWindow::OnNewBounce()
{
	CBounceWnd *pBounceWnd = new CBounceWnd;
	if (!pBounceWnd->Create("Bounce", 0, rectDefault, this))
	{
		delete pBounceWnd;      // HWND not created
		return;
	}
	pBounceWnd->ShowWindow(SW_SHOW);
	// the default PostNcDestroy handler will delete this object when destroyed
}

// OnChildDestroy:
// Example of a custom message handler (for the custom WM_CHILDDESTROY message)
// This handler is triggered when a CBounceWnd or CHelloWnd destroys
// itself -- the default implementation here does nothing but could
// be customized to do additional work.
//
LONG CMainWindow::OnChildDestroy(UINT /*hWnd*/, LONG /* lParam */)
{
	return 0;
}

// Destructor:
// Destroy all existing child windows.
//
CMainWindow::~CMainWindow()
{
	delete m_pMenuInit;
}

// OnExit:
// Exit the application.
//
void CMainWindow::OnExit()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CTheApp

// InitInstance:
// Create and display the application main frame window
//
BOOL CTheApp::InitInstance()
{
	m_pMainWnd = new CMainWindow();
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}
