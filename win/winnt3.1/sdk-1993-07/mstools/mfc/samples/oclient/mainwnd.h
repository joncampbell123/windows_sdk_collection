// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// class CMainWnd - the main frame window for this app

class CItemWnd;
class CMainWnd;

class CMainDocument : public COleClientDoc
{
	CMainWnd*   m_pView;
public:
	CMainDocument(CMainWnd* pView)
		{ m_pView = pView; }

	// iterator for contained items
	virtual COleClientItem* GetNextItem(POSITION& rPosition,
		BOOL* pIsSelected);
};


class CMainWnd : public CFrameWnd
{
public:
	CString         m_title;            // title of window
	CString         m_fileName;         // file name (may be empty)

public:
	CMainWnd();

// Attributes
	CItemWnd* GetSelection() const;
	void    SetSelection(CItemWnd* pNewSel);

	const CMainDocument* GetDocument() const;
	CMainDocument* GetDocument();

// Operations
	void    Hourglass(BOOL bOn);
	void    ErrorMessage(UINT id);

	void    Dirty();

	// File helpers
	BOOL    SaveAsNeeded();
	void    InitFile(BOOL fOpen);
	BOOL    DoSave(const char* szFileName);
	BOOL    DoFileDlg(CString& fileName, UINT nIDTitle, DWORD lFlags);

	// naming/registering
	void    SetTitle();
	void    DeregisterDoc();

	// reading-writing content
	virtual void Serialize(CArchive& ar);       // from CObject
#ifdef _DEBUG
	virtual void AssertValid() const;
#endif

// Callbacks
	// windows messages
	afx_msg void OnInitMenuPopup(CMenu*, UINT, BOOL);
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnQueryEndSession();

	// callback helpers
	BOOL    DoCopySelection();
	BOOL    DoPaste(BOOL fLink);
	void    ClearAll();

	// commands
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnExit();
	afx_msg void OnAbout();

	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnPasteLink();
	afx_msg void OnClear();
	afx_msg void OnClearAll();
	afx_msg void OnInsertObject();
	afx_msg void OnEditLinks();

// Implementation
protected:
	virtual BOOL OnCommand(UINT wParam, LONG lParam);
	CItemWnd* m_pSelection;
	BOOL    m_fDirty;

	CMainDocument   m_document;

	DECLARE_MESSAGE_MAP()
};

// inlines
inline void CMainWnd::Dirty()
	{ m_fDirty = TRUE; }
inline const CMainDocument* CMainWnd::GetDocument() const
	{ return &m_document; }
inline CMainDocument* CMainWnd::GetDocument()
	{ return &m_document; }
inline CItemWnd* CMainWnd::GetSelection() const
	{ return m_pSelection; } // Just the top-most window
