// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "minsvrMI.h"

// Main application parts and OLE server parts
//  see 'mainwnd.cpp' for main window parts and item UI

/////////////////////////////////////////////////////////////////////////////
// single object for the app
CMiApp app;

CMiApp::CMiApp()
	: CWinApp(SERVER_NAME), COleServer(TRUE)
{
	m_data = "sample data (MI version)";
}

/////////////////////////////////////////////////////////////////////////////
// Main app processing

BOOL CMiApp::InitInstance()
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
			MessageBox("Server Registered,"
				" please run from within host application.",
				AfxGetAppName());
		}
		else
		{
			MessageBox("Server failed to register, try using REGEDIT.",
				AfxGetAppName());
		}
		return FALSE;
	}

	// Server Init
	if (!COleServer::Register(SERVER_NAME, TRUE))   // multi-instance server
	{
		MessageBox("Could not register server", AfxGetAppName(), MB_OK);
		return FALSE;
	}


	// MiApp ignores rest of command line - normally use this as a
	//   document file name

	// Window init
	CRect rect(0, 200, 200, 400);
	Create(NULL, SERVER_LOCAL_NAME, WS_OVERLAPPEDWINDOW,
			rect, NULL, "MainMenu");
	ShowWindow(m_nCmdShow);
	UpdateWindow();
	m_pMainWnd = this;  // we are a main window !

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server functionality

COleServerDoc*
CMiApp::OnOpenDoc(LPCSTR)
{
	return this;
}

COleServerDoc*
CMiApp::OnCreateDoc(LPCSTR, LPCSTR)
{
	return this;
}

COleServerDoc*
CMiApp::OnEditDoc(LPCSTR, LPCSTR)
{
	return this;
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server Doc functionality

COleServerItem* CMiApp::OnGetDocument()
{
	return this;
}

COleServerItem* CMiApp::OnGetItem(LPCSTR)
{
	return this;
}

COleServerItem* CMiApp::GetNextItem(POSITION& rPosition)
{
	if (rPosition != NULL)
		return NULL;
	rPosition = (POSITION) 1;
	return this;
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server Item functionality

void
CMiApp::Serialize(CArchive& ar)
{
	// Customize this to store real data
	if (ar.IsStoring())
	{
		ar << m_data;
	}
	else
	{
		ar >> m_data;
	}
}


BOOL
CMiApp::OnGetTextData(CString& rStringReturn)
{
	rStringReturn = m_data;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
