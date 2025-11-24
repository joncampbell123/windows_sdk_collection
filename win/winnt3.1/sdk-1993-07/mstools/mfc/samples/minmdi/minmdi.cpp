// minmdi.cpp : Defines the class behaviors for MinMDI.
//              This application is the simplest Multiple Document Interface
//              (MDI) program.  It demonstrates how the CMDIFrameWnd and
//              CMDIChildWnd classes can be used together to make an MDI
//              application.
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

#include "minmdi.h"

/////////////////////////////////////////////////////////////////////////////

// Create one global CTheApp object.  Once created, it takes care of itself.
//
CTheApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainWindow

// CMainWindow constructor:
// Create the window, with appropriate style, default size, and the
// "MainMenu" menu and "MainAccelTable" resources (see minmdi.rc).
//
CMainWindow::CMainWindow()
{
	LoadAccelTable("MainAccelTable");
	Create(NULL, "Minimal MDI Sample Application", WS_OVERLAPPEDWINDOW, 
		rectDefault, NULL, "MainMenu");
}

// OnAbout:
// Invoke the "AboutBox" modal dialog (see minmdi.rc and about.dlg).
//
void CMainWindow::OnAbout()
{
	CModalDialog about("AboutBox", this);
	about.DoModal();
}

// OnNewWindow:
// Create and show a new CMDIChildWnd object and window.
//
void CMainWindow::OnNewWindow()
{
	CMDIChildWnd* pWnd = new CMDIChildWnd;

	if (pWnd->Create(NULL, "Child Window", 0, rectDefault, this))
		pWnd->ShowWindow(SW_SHOW);
	else
		MessageBeep(0);
}

// CMainWindow message map:
// Associate two WM_COMMAND menu choices to member functions.
//
BEGIN_MESSAGE_MAP(CMainWindow, CMDIFrameWnd)
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_COMMAND(IDM_WINDOWNEW, OnNewWindow)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTheApp

// InitInstance:
// Create and show the main window.
//
BOOL CTheApp::InitInstance()
{
	m_pMainWnd = new CMainWindow();
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

