// DlgSQL.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSQL dialog

class CDlgSQL : public CDialog
{
// Construction
public:
	CDlgSQL(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSQL)
	enum { IDD = IDD_DLGSQL };
	CString	m_strName;
	CString	m_strSQL;
	//}}AFX_DATA
	BOOL m_bEditMode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSQL)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSQL)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
