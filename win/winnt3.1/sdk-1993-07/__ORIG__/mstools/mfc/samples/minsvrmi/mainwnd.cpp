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

// Main window and user interface parts

/////////////////////////////////////////////////////////////////////////////

// message map since we are a window
BEGIN_MESSAGE_MAP(CMiApp, CFrameWnd)
	// windows messages
	ON_MESSAGE(WM_CLOSE, OnCloseWindow)
		// Both CWnd and COleServer have an 'OnClose' member function but
		//  they differ only by the return type.
		// Unfortunately C++ multiple inheritance rules prevent us from
		//  from overriding either of them.  We use the more general
		//  form of message map entries to replace CWnd::OnClose with
		//  CMiApp::OnCloseWindow to get around this restriction of C++.

	// menu commands
	ON_COMMAND(IDM_UPDATE, OnUpdateClient)
	ON_COMMAND(IDM_EXIT, OnFileExit)
	ON_COMMAND(IDM_CHANGESTRING, OnChangeString)
	ON_COMMAND(IDM_ABOUT, OnAbout)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// File menu commands

void CMiApp::OnFileExit()
{
	// to shut-down, just revoke the server, OLE will terminate the app
	COleServer::BeginRevoke();
}

LONG CMiApp::OnCloseWindow(UINT, LONG)
{
	OnFileExit();
	return 0;
}

void CMiApp::PostNcDestroy()
{
	// CFrameWnd::PostNcDestroy will 'delete this'.
	// since the CMiApp is-a CFrameWnd we don't want PostNcDestroy
	//  to do anything - so we override it here to remove the default
	//  behaviour.
}

/////////////////////////////////////////////////////////////////////////////

void CMiApp::OnUpdateClient()
{
	TRY
	{
		NotifySaved();
	}
	CATCH (CException, e)
	{
		MessageBox("Couldn't update client");
	}
	END_CATCH
}

// Help menu commands
void CMiApp::OnAbout()
{
	CModalDialog dlg("AboutBox");
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// OLE Item UI

OLESTATUS CMiApp::OnShow(BOOL /* bTakeFocus */)
{
	// make sure server is the topmost window
	BringWindowToTop();
	return OLE_OK;
}

///////////////////////////////////////////
// Simple dialog for changing the string

class CChangeDlg : public CModalDialog
{
protected:
	CString&    m_rString;

public:
	CChangeDlg(CString& rString)
		: CModalDialog("ChangeDlg"), m_rString(rString)
			{ }

	BOOL OnInitDialog()
	{
		GetDlgItem(IDC_EDIT1)->SetWindowText(m_rString);
		return TRUE;
	}
	void OnOK()
	{
		GetDlgItem(IDC_EDIT1)->GetWindowText(m_rString);
		EndDialog(IDOK);
	}
};


void CMiApp::OnChangeString()
{
	CChangeDlg dlg(m_data);
	if (dlg.DoModal() == IDOK && COleServerDoc::IsOpen())
		OnUpdateClient(); // example of immediately updating client
}

/////////////////////////////////////////////////////////////////////////////
// Drawing items into bitmap or metafile

BOOL CMiApp::OnDraw(CMetaFileDC* pDC)
{
	ASSERT_VALID(pDC);
	CSize textSize;

	// first calculate the text size in MM_TEXT units
	{
		CWindowDC screenDC(NULL);
		textSize = screenDC.GetTextExtent(m_data, m_data.GetLength());
	}

	// if you want the item to always be drawn in a specific mapping
	//  mode set it here.

	// Otherwise the OLE DLLs will scale the metafile to fit the
	//  client specified size.  Setting the viewport size/extent
	//  determines the relative scale of everything.

	int cx = textSize.cx + 100;
	int cy = (cx * 4) / 3;      // nice aspect ratio
	TRACE("Item drawing size is %d x %d\n", cx, cy);
	pDC->SetWindowExt(cx, cy);

	// Draw a shaded circle
	pDC->SelectStockObject(LTGRAY_BRUSH);
	pDC->Ellipse(0, 0, cx, cy);

	// draw the text in the middle (as best we can)
	pDC->SetBkMode(TRANSPARENT);
	pDC->TextOut((cx - textSize.cx) / 2, (cy - textSize.cy) / 2, m_data);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
