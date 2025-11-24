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

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	// windows messages
	ON_WM_CLOSE()
	// menu commands
	ON_COMMAND(IDM_UPDATE, OnUpdateClient)
	ON_COMMAND(IDM_EXIT, OnClose)       // exit calls close
	ON_COMMAND(IDM_CHANGESTRING, OnChangeString)
	ON_COMMAND(IDM_ABOUT, OnAbout)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Basic creation etc

#pragma warning(disable:4355)   // this used in constructor

CMainWnd::CMainWnd() : m_server(this)
{
	CRect rect(0, 200, 200, 400);

	Create(NULL, SERVER_LOCAL_NAME,
		WS_OVERLAPPEDWINDOW, rect, NULL, "MainMenu");
}

/////////////////////////////////////////////////////////////////////////////

void CMainWnd::OnClose()
{
	// to shut-down, just revoke the server, OLE will terminate the app
	m_server.BeginRevoke();
}

/////////////////////////////////////////////////////////////////////////////
// Edit menu commands

void CMainWnd::OnChangeString()
{
	if (m_server.m_doc.m_item.PromptChangeString())
	{
		// example of immediately updating client doc if open
		// for more complicated data you shouldn't update until
		//  the user selects the update menu
		if (m_server.m_doc.IsOpen())
			OnUpdateClient();
	}
}

// Help menu commands
void CMainWnd::OnAbout()
{
	CModalDialog dlg("AboutBox");
	dlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// Update Client

void CMainWnd::OnUpdateClient()
{
	TRY
	{
		m_server.m_doc.NotifySaved();
	}
	CATCH (CException, e)
	{
		MessageBox("Could not update client");
	}
	END_CATCH
}

/////////////////////////////////////////////////////////////////////////////
