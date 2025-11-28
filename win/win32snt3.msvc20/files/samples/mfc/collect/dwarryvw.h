// dwarryvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDWordArrayView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CDWordArrayView : public CFormView
{
public:
	CDWordArrayView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDWordArrayView)

// Attributes
public:
	CCollectDoc* GetDocument();

// Overrides
public:
	void OnInitialUpdate();

// Form Data
public:
	//{{AFX_DATA(CDWordArrayView)
	enum { IDD = IDD_DWORD_ARRAY };
	CListBox    m_ctlList;
	DWORD   m_dw;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDWordArrayView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDWordArrayView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDWordArrayView)
	afx_msg void OnAdd();
	afx_msg void OnUpdate();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	afx_msg void OnSelChangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CCollectDoc* CDWordArrayView::GetDocument()
   { return (CCollectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
