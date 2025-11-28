// intlstvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIntListView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CIntListView : public CFormView
{
public:
	CIntListView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CIntListView)

// Attributes
public:
	CCollectDoc* GetDocument();

// Overrides
public:
	void OnInitialUpdate();

// Form Data
public:
	//{{AFX_DATA(CIntListView)
	enum { IDD = IDD_INT_LIST };
	CListBox    m_ctlList;
	int     m_int;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIntListView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	BOOL FindInt(int& nSel, POSITION& pos);
	void AddIntToListBox(int n, int nSel = -1);

	virtual ~CIntListView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CIntListView)
	afx_msg void OnAdd();
	afx_msg void OnUpdate();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	afx_msg void OnSelChangeList();
	afx_msg void OnInsertBefore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CCollectDoc* CIntListView::GetDocument()
   { return (CCollectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
