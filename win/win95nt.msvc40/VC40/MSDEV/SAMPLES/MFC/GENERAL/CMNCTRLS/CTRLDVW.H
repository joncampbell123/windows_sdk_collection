// ctrldVw.h : interface of the CCtrldemoView class
//
/////////////////////////////////////////////////////////////////////////////

class CCtrldemoView : public CView
{
protected: // create from serialization only
	CCtrldemoView();
	DECLARE_DYNCREATE(CCtrldemoView)

// Attributes
public:
	CCtrldemoDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCtrldemoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCtrldemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CCtrldemoView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ctrldVw.cpp
inline CCtrldemoDoc* CCtrldemoView::GetDocument()
   { return (CCtrldemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
