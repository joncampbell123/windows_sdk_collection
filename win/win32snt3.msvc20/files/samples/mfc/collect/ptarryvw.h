// ptarryvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPointArrayView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CPointArrayView : public CFormView
{
public:
	CPointArrayView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPointArrayView)

// Attributes
public:
	CCollectDoc* GetDocument();

// Overrides
public:
	void OnInitialUpdate();

// Form Data
public:
	//{{AFX_DATA(CPointArrayView)
	enum { IDD = IDD_POINT_ARRAY };
	CListBox    m_ctlList;
	int     m_x;
	int     m_y;
	//}}AFX_DATA

// Implementation
protected:
	void AddPointToListBox(CPoint pt, int nSel = -1);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPointArrayView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPointArrayView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPointArrayView)
	afx_msg void OnAdd();
	afx_msg void OnUpdate();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	afx_msg void OnSelChangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CCollectDoc* CPointArrayView::GetDocument()
   { return (CCollectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
