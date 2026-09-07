// DlgParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgParams dialog

class CDlgParams : public CDialog
{
// Construction
public:
	CDlgParams(CWnd* pParent = NULL);   // standard constructor

	void SetInfo(CDaoDatabase* pDB,LPCTSTR lpszQueryDef);
// Dialog Data
	//{{AFX_DATA(CDlgParams)
	enum { IDD = IDD_DIALOG2 };
	CListCtrlEx	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgParams)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDaoDatabase* m_pDB;
	CString m_strQueryDef;

	// Generated message map functions
	//{{AFX_MSG(CDlgParams)
	virtual BOOL OnInitDialog();
	afx_msg void OnEndlabeleditListview1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
