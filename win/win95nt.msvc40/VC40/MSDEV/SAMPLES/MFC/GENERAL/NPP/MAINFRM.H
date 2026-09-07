// mainfrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CComboToolBar    m_wndToolBar;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	BOOL CreateComboBox();

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnViewStatusBar();
	afx_msg void OnEditFindCombo();
	//}}AFX_MSG
	afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLineNumber(CCmdUI* pCmdUI);
	afx_msg void OnUpdateClock(CCmdUI* pCmdUI);
	
	// timer call back function
	static void CALLBACK EXPORT UpdateTime(HWND hWnd,UINT nMsg, UINT nIDEvent, DWORD dwTime);
	DECLARE_MESSAGE_MAP()
};

inline CNotepadView* GetApplicationView()
{
	CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
	ASSERT(pFrame);
	return (CNotepadView*)(pFrame)->GetActiveView();
}

/////////////////////////////////////////////////////////////////////////////

