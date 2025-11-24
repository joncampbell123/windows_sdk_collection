// chart.cpp : Defines the class behaviors for the Chart application.
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

#include "chart.h"


// a simple way to reduce size of C runtimes
// disable the use of getenv and argv/argc
extern "C" void _setargv() { }
extern "C" void _setenvp() { }

/////////////////////////////////////////////////////////////////////////////
// Once created, the whole application object takes care of itself.
//
CTheApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTheApp

// InitInstance:
// Create and display the main frame window
//
BOOL CTheApp::InitInstance()
{
	m_pMainWnd = new CChartWnd();
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}



CTheApp::~CTheApp()
{
	delete m_pMainWnd;
}

