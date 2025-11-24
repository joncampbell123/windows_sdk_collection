// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#include "bibref.h"
#include "mainwnd.h"
#include "bibsvr.h"

/////////////////////////////////////////////////////////////////////////////
// One instance of this server application per client

CBibApp app;
static CBibServer server;

BOOL CBibApp::InitInstance()
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
		server.SetLaunchEmbedded();
	}
	// BibRef ignores rest of command line - normally use this as a
	//   document file name

	// BibRef data file
	strIniFile = CString(m_pszAppName) + ".ini";
		// NOTE: ini file contains settings as well as data

	CMainWnd* pWnd = new CMainWnd(bEmbedded);
	m_pMainWnd = pWnd;
	pWnd->ShowWindow(m_nCmdShow);
	pWnd->UpdateWindow();

	if (!bEmbedded)
	{
		AfxOleRegisterServerName(SERVER_NAME, SERVER_LOCAL_NAME);
	}

	if (!server.Register(SERVER_NAME, TRUE))   // multi-instance server
	{
		MessageBox(NULL, "Could not register server - Exiting",
			SERVER_NAME, MB_OK);
		return FALSE;
	}

	return TRUE;
}

void CBibApp::ShutDown()
{
	if (server.IsOpen())
		server.BeginRevoke(); // revoke the server, OLE will terminate the app
	else
		::PostQuitMessage(0);
}

int CBibApp::ExitInstance()
{
	if (m_pMainWnd != NULL)
		m_pMainWnd->DestroyWindow();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
