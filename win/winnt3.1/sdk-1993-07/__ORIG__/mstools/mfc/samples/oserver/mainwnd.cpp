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
#include "bibdoc.h"

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_WM_INITMENUPOPUP()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDM_ABOUT, OnAbout)
	ON_COMMAND(IDM_EXIT, OnClose)       // Exit just closes main window

	// only make sense if run non-embedded
	ON_COMMAND(IDM_ITEM_ADD, OnItemAdd)
	ON_COMMAND(IDM_ITEM_DELETE, OnItemDelete)
	ON_COMMAND(IDM_ITEM_MODIFY, OnItemModify)

	// for embedded case
	ON_COMMAND(IDM_UPDATE, OnUpdateClient)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Basic creation etc

CMainWnd::CMainWnd(BOOL bEmbedded)
{
	CRect rect;
	char szRect[32];

	if (GetPrivateProfileString("Settings", "Last Position", "",
		szRect, sizeof(szRect), ::app.strIniFile) == 0 ||
		sscanf(szRect, "%d %d %d %d", &rect.left, &rect.top,
			&rect.right, &rect.bottom) != 4)
	{
		// use initial value
		rect = rectDefault;
	}

	Create(NULL, "Bibliographic References",
		WS_OVERLAPPEDWINDOW, rect, NULL,
		bEmbedded ? "EmbeddedMenu" : "MainMenu");
}

void CMainWnd::OnDestroy()
{
	CRect rect;
	GetWindowRect(&rect);

	char szRect[32];
	sprintf(szRect, "%d %d %d %d", 
		rect.left, rect.top, rect.right, rect.bottom);
	WritePrivateProfileString("Settings", "Last Position",
		szRect, ::app.strIniFile);

	CFrameWnd::OnDestroy();
}


int CMainWnd::OnCreate(LPCREATESTRUCT)
{
	// Create the listbox child window
	CRect       rect;
	GetClientRect(&rect);

	if (!m_listbox.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_VSCROLL |
	  LBS_SORT | LBS_USETABSTOPS | LBS_NOINTEGRALHEIGHT,
	  rect, this, -1))
	{
		TRACE("Error creating listbox\n");
		return -1;
	}

	m_listbox.SetTabStops(40);      // 10 characters wide

	m_pDoc = new CBibDoc(&m_listbox, ::app.strIniFile, "Items");

	if (!m_pDoc->Load())
	{
		MessageBox("Load failed");
		delete m_pDoc;
		return -1;
	}

	return 0;   // ok
}

/////////////////////////////////////////////////////////////////////////////
// Closing the window/server

void CMainWnd::OnClose()
{
	// prompt if open and something selected (assume dirty)
	if (m_pDoc->IsOpen() && m_listbox.GetCurSel() != -1)
	{
		// optionally update the client on exit
		char szPrompt[256];
		wsprintf(szPrompt, "Update %s %s?", (LPCSTR)GetDocument()->m_strHost,
			(LPCSTR)GetDocument()->m_strHostObj);

		// Note: should only prompt if document is dirty, instead of all
		//  the time as here.
		if (MessageBox(szPrompt, AfxGetAppName(), MB_YESNO) == IDYES)
			OnUpdateClient();
	}

	app.ShutDown();
}

/////////////////////////////////////////////////////////////////////////////

void CMainWnd::OnSize(UINT, int, int)
{
	// Resize child
	CRect       rect;
	GetClientRect(&rect);
	m_listbox.MoveWindow(rect);
}

void CMainWnd::OnSetFocus(CWnd*)
{
	// forward the focus to our one and only child
	m_listbox.SetFocus();
}

void CMainWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT, BOOL bSysMenu)
{
	if (bSysMenu)
		return;

	// if no item selected - disable delete and modify
	UINT    mf = MF_GRAYED|MF_DISABLED;

	if (m_listbox.GetCurSel() != -1)
		mf = MF_ENABLED;
	pPopupMenu->EnableMenuItem(IDM_ITEM_DELETE, mf);
	pPopupMenu->EnableMenuItem(IDM_ITEM_MODIFY, mf);
	pPopupMenu->EnableMenuItem(IDM_UPDATE, mf);
}

/////////////////////////////////////////////////////////////////////////////
// File menu commands

void CMainWnd::OnAbout()
{
	CModalDialog about(IDM_ABOUT, this);
	about.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// Item menu commands

class CPromptDlg : public CModalDialog
{
	CString&    m_rKey;
	CString&    m_rValue;
public:
	CPromptDlg(CString& key, CString& value)
		: CModalDialog(IDM_ITEM_ADD), m_rKey(key), m_rValue(value)
		{ }

protected:
	CEdit&      Edit1()
					{ return *((CEdit*) GetDlgItem(IDC_EDIT1)); } 
	CEdit&      Edit2()
					{ return *((CEdit*) GetDlgItem(IDC_EDIT2)); } 

	BOOL    OnInitDialog()
	{
		Edit1().SetWindowText(m_rKey);
		Edit2().SetWindowText(m_rValue);
		return TRUE;
	}

	void OnOK()
	{
		Edit1().GetWindowText(m_rKey);
		Edit2().GetWindowText(m_rValue);

		if (m_rKey == "" || m_rValue == "")
		{
			MessageBox("Both key and value must be specified");
			return;
		}
		EndDialog(IDOK);
	}
};

void CMainWnd::OnItemAdd()
{
	CString key, value;
	CPromptDlg  dlg(key, value);

	if (dlg.DoModal() != IDOK)
		return;

	CString oldValue;
	int nOldIndex = m_pDoc->GetItemValue(key, oldValue);
	if (nOldIndex != -1)
		m_pDoc->DeleteItem(key, nOldIndex);

	// add the item
	m_pDoc->AddItem(key, value);
}

void CMainWnd::OnItemDelete()
{
	int nIndex = m_listbox.GetCurSel();
	CString key, value;
	m_pDoc->GetItemKeyValue(nIndex, key, value);

	// delete the key (in listbox and in ini file)
	m_pDoc->DeleteItem(key, nIndex);
}

void CMainWnd::OnItemModify()
{
	int nIndex = m_listbox.GetCurSel();
	CString key, value;
	m_pDoc->GetItemKeyValue(nIndex, key, value);

	CString originalKey = key;
	CPromptDlg  dlg(key, value);

	if (dlg.DoModal() != IDOK)
		return;

	m_pDoc->DeleteItem(originalKey, nIndex);
	m_pDoc->AddItem(key, value);
}

/////////////////////////////////////////////////////////////////////////////
// Update Client

void CMainWnd::OnUpdateClient()
{
	int nIndex = m_listbox.GetCurSel();
	if (nIndex == -1)
		return;

	CString key, value;
	m_pDoc->GetItemKeyValue(nIndex, key, value);
	if (!m_pDoc->UpdateClient(key))
		MessageBox("Couldn't update client");
}

/////////////////////////////////////////////////////////////////////////////
