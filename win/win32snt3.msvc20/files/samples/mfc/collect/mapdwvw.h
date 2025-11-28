// mapdwvw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapDWordToMyStructView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CMapDWordToMyStructView : public CFormView
{
public:
	CMapDWordToMyStructView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMapDWordToMyStructView)

// Form Data
public:
	//{{AFX_DATA(CMapDWordToMyStructView)
	enum { IDD = IDD_MAP_DWORD_TO_MYSTRUCT };
	CListBox    m_ctlList;
	int     m_int;
	float   m_float;
	CString m_str;
	DWORD   m_dwKey;
	//}}AFX_DATA

// Attributes
public:
	CCollectDoc* GetDocument();

// Overrides
public:
	void OnInitialUpdate();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMapDWordToMyStructView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static CString FormatListBoxEntry(DWORD dwKey, CMyStruct* pMyStruct);
	int FindKeyInListBox(DWORD dwKey);
	void AddMapEntryToListBox(DWORD dwKey, CMyStruct* pMyStruct);
	CMyStruct* ConstructMyStructFromView();
	void UpdateViewFromMyStruct(CMyStruct* pMyStruct);

protected:
	virtual ~CMapDWordToMyStructView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CMapDWordToMyStructView)
	afx_msg void OnAddOrUpdate();
	afx_msg void OnFind();
	afx_msg void OnRemove();
	afx_msg void OnRemoveAll();
	afx_msg void OnSelChangeList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CCollectDoc* CMapDWordToMyStructView::GetDocument()
   { return (CCollectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
