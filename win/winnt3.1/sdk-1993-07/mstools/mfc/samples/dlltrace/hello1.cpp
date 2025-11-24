// hello.cpp : Defines the class behaviors for the application.
//           Hello is a simple program which consists of a main window
//           and an "About" dialog which can be invoked by a menu choice.
//           It is intended to serve as a starting-point for new
//           applications.
//
//           This version of hello also demonstrates communication with
//           a dynamic link library (DLL) implemented with the
//           Microsoft Foundation Classes.
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

#include "traceapi.h"

#ifndef _DEBUG
#error This source file must be compiled with _DEBUG defined
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainWindow

class CMainWindow : public CFrameWnd
{
public:
	CMainWindow();

	afx_msg void OnPaint();
	afx_msg void OnAbout();
	afx_msg void OnTraceFlags();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CMainWindow, CFrameWnd)
	ON_WM_PAINT()
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_COMMAND(IDM_TRACE_FLAGS, OnTraceFlags)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

CMainWindow::CMainWindow()
{
	VERIFY(Create(NULL, "Hello Foundation Application",
		WS_OVERLAPPEDWINDOW, rectDefault, NULL, "MainMenu"));
}

void CMainWindow::OnPaint()
{
	CString s = "Hello, Windows! (with DLL support)";
	CPaintDC dc(this);
	CRect rect;

	GetClientRect(rect);
	dc.SetTextAlign(TA_BASELINE | TA_CENTER);
	dc.SetBkMode(TRANSPARENT);
	dc.TextOut(rect.right / 2, rect.bottom / 2, s);
}

void CMainWindow::OnAbout()
{
	CModalDialog about("AboutBox", this);
	about.DoModal();
}

void CMainWindow::OnTraceFlags()
{
	struct TracerData data;
	data.bEnabled = afxTraceEnabled;
	data.flags = afxTraceFlags;

	TRACE("Calling Tracer DLL\n");
	if (PromptTraceFlags(&data))
	{
		TRACE("Changing trace flags\n");
		afxTraceEnabled = data.bEnabled;
		afxTraceFlags = data.flags;
		TRACE("Changed trace flags\n");
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTheApp

class CTheApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

BOOL CTheApp::InitInstance()
{
	// standard initialization of a main window

	m_pMainWnd = new CMainWindow;
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

BOOL CTheApp::PreTranslateMessage(MSG* pMsg)
{
	// special filter for DLL
	return (CWinApp::PreTranslateMessage(pMsg) ||
			FilterDllMsg(pMsg));
}

CTheApp theApp; // application object

/////////////////////////////////////////////////////////////////////////////
