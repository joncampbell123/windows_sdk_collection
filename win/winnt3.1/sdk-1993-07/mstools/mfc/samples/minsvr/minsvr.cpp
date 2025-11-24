// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "minsvr.h"

/////////////////////////////////////////////////////////////////////////////
// One instance per client

CMinApp app;

BOOL CMinApp::InitInstance()
{
	BOOL bEmbedded = FALSE;

	// check if run with /Embedding
	LPCSTR lpsz = m_lpCmdLine;

	while (*lpsz == ' ')
		lpsz++;
	if ((*lpsz == '-' || *lpsz == '/') &&
		_fstrncmp("Embedding", lpsz+1, 9) == 0)
	{
		lpsz += 10;
		bEmbedded = TRUE;
	}

	if (!bEmbedded)
	{
		// register as a server
		if (AfxOleRegisterServerName(SERVER_NAME, SERVER_LOCAL_NAME))
		{
			MessageBox(NULL,
				"Server Registered, please run from within host application.",
				AfxGetAppName(), MB_OK);
		}
		else
		{
			MessageBox(NULL,
				"Server failed to register, try using REGEDIT.",
				AfxGetAppName(), MB_OK);
		}

		return FALSE;
	}
	// MinApp ignores rest of command line - normally use this as a
	//   document file name

	CMainWnd* pWnd = new CMainWnd;
	m_pMainWnd = pWnd;
	pWnd->ShowWindow(m_nCmdShow);
	pWnd->UpdateWindow();

	if (!pWnd->m_server.Register(SERVER_NAME, TRUE))
		 // multi-instance server
	{
		MessageBox(NULL, "couldn't register server", AfxGetAppName(), MB_OK);
		return FALSE;
	}

	return TRUE;
}

int CMinApp::ExitInstance()
{
	// on exit, destroy the main window
	if (m_pMainWnd != NULL)
		VERIFY(m_pMainWnd->DestroyWindow());
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// Minimum server

CMinServer::CMinServer(CMainWnd* pMainWnd)
			: COleServer(TRUE), m_doc()
{
	m_pMainWnd = pMainWnd;
}

COleServerDoc* CMinServer::OnOpenDoc(LPCSTR)
{
	return &m_doc;
}

COleServerDoc* CMinServer::OnCreateDoc(LPCSTR, LPCSTR)
{
	return &m_doc;
}

COleServerDoc* CMinServer::OnEditDoc(LPCSTR, LPCSTR)
{
	return &m_doc;
}

/////////////////////////////////////////////////////////////////////////////
