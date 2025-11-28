// dockvw.h : interface of the CDockView class
//
/////////////////////////////////////////////////////////////////////////////

class CDockView : public CView
{
protected: // create from serialization only
	CDockView();
	DECLARE_DYNCREATE(CDockView)

// Attributes
public:
	CDockDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDockView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDockView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDockView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in dockvw.cpp
inline CDockDoc* CDockView::GetDocument()
   { return (CDockDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
