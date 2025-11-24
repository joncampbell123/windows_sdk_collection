// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// CMainWnd:

class CBibDoc;

class CMainWnd : public CFrameWnd
{
protected:
	CListBox    m_listbox;          // manipulated by document as well as window
	CBibDoc*    m_pDoc;

public:
	CMainWnd(BOOL bEmbedded);

// Attributes
	CBibDoc*    GetDocument()
					{ return m_pDoc; }

// Implementation
protected:
	// windows messages
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClose();
	afx_msg void OnDestroy();

	// standard menu commands
	afx_msg void OnAbout();

	// non-embedded options
	afx_msg void OnItemAdd();
	afx_msg void OnItemDelete();
	afx_msg void OnItemModify();

	// embedded options
	afx_msg void OnUpdateClient();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

