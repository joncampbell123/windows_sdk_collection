// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// OCLIENT.CXX : main client app

#include "oclient.h"
#include "mainwnd.h"

/////////////////////////////////////////////////////////////////////////////
// Main app class

class CMyApp : public CWinApp
{
public:
	CMyApp() : CWinApp("ole client") { }

// Overrides
	virtual BOOL InitInstance();

} myApp;

CString strUntitled;

BOOL CMyApp::InitInstance()
{
	CMainWnd * pWnd = new CMainWnd();

	VERIFY(strUntitled.LoadString(IDS_UNTITLED));

	m_pMainWnd = pWnd;
	pWnd->SetTitle();
	pWnd->ShowWindow(m_nCmdShow);
	pWnd->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
