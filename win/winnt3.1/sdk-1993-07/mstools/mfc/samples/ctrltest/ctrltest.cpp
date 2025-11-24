// ctrltest.cpp : Dialogs and Controls test applet
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

#include "ctrltest.h"

/////////////////////////////////////////////////////////////////////////////
// Main Window

BEGIN_MESSAGE_MAP(CTestWindow, CFrameWnd)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_COMMAND(IDM_ABOUT, OnAbout)
	// simple
	ON_COMMAND(IDM_TEST_DERIVED_EDIT, OnTestDerivedEdit)
	ON_COMMAND(IDM_TEST_WNDCLASS_EDIT, OnTestWndClassEdit)
	ON_COMMAND(IDM_TEST_SUB_EDIT, OnTestSubclassedEdit)
#ifndef _NTWIN
	// pen
	ON_COMMAND(IDM_TEST_PENEDIT_CODE, OnTestPenEditFromCode)
	ON_COMMAND(IDM_TEST_PENEDIT_TEMPLATE, OnTestPenEditFromTemplate)
	ON_COMMAND(IDM_TEST_PENEDIT_FEATURES, OnTestPenEditFeatures)
#endif
	// custom
	ON_COMMAND(IDM_TEST_BITMAP_BUTTON1, OnTestBitmapButton1)
	ON_COMMAND(IDM_TEST_BITMAP_BUTTON2, OnTestBitmapButton2)
	ON_COMMAND(IDM_TEST_CUSTOM_LIST, OnTestCustomList)
	ON_COMMAND(IDM_TEST_SPIN_EDIT, OnTestSpinEdit)
END_MESSAGE_MAP()

void CTestWindow::SetupMenus()
{
	if ((GetSystemMetrics(SM_PENWINDOWS)) == NULL)
	{
		CMenu* pMenu = GetMenu();
		ASSERT(pMenu != NULL);
		pMenu->EnableMenuItem(IDM_TEST_PENEDIT_CODE, MF_DISABLED|MF_GRAYED);
		pMenu->EnableMenuItem(IDM_TEST_PENEDIT_TEMPLATE, MF_DISABLED|MF_GRAYED);
		pMenu->EnableMenuItem(IDM_TEST_PENEDIT_FEATURES, MF_DISABLED|MF_GRAYED);
	}
	// do not test for spin control until the user tries it
	// if the custom control DLL is not present, the test spin
	//  control menu item will be disabled in 'OnTestSpinEdit'.

	// custom menu tests
	AttachCustomMenu();
}

void CTestWindow::OnExit()
{
	SendMessage(WM_CLOSE);
}

void CTestWindow::OnAbout()
{
	CModalDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// Application class

class CTestApp : public CWinApp
{
protected:
	CTestWindow m_window;
		// instead of allocating a new main window, you can embed it
		//   if you like
public:
	virtual BOOL InitInstance();
};

CTestApp theTestApp;

BOOL CTestApp::InitInstance()
{
	if (!m_window.Create(NULL, "Control Test App",
	  WS_OVERLAPPEDWINDOW, CFrameWnd::rectDefault, NULL,
	  MAKEINTRESOURCE(AFX_IDI_STD_FRAME)/*menu*/))
		return FALSE;

	m_pMainWnd = &m_window;
	m_window.SetupMenus();
	m_window.ShowWindow(m_nCmdShow);
	return TRUE;
}

void CTestWindow::PostNcDestroy()
{
	// don't delete this
}

/////////////////////////////////////////////////////////////////////////////
