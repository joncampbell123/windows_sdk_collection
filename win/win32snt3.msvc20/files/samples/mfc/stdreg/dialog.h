// dialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStdRegSetupDlg dialog

class CStdRegSetupDlg : public CDialog
{
// Construction
public:
	CStdRegSetupDlg(CWnd* pParent = NULL);  // standard constructor

// Dialog Data
	//{{AFX_DATA(CStdRegSetupDlg)
	enum { IDD = IDD_STDREG_DIALOG };
	CStatic m_ctlProgress;
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStdRegSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CStdRegSetupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnAddDataSource();
	afx_msg void OnInitializeData();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
