// catalog2View.h : interface of the CCatalog2View class
//
/////////////////////////////////////////////////////////////////////////////

class CCatalog2View : public CListView
{
protected: // create from serialization only
	CCatalog2View();
	DECLARE_DYNCREATE(CCatalog2View)

// Attributes
public:
	CString		m_strTableName;
	CCatalog2Doc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCatalog2View)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
//	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCatalog2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCatalog2View)
	afx_msg void OnViewTablelevel();
	afx_msg void OnViewColumnlevel();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateViewColumnlevel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTablelevel(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in catalog2View.cpp
inline CCatalog2Doc* CCatalog2View::GetDocument()
   { return (CCatalog2Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
