// AdderDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAdderDialog dialog

class CAdderDialog : public CDialog
{
// Construction
public:
	CAdderDialog(CWnd* pParent);   // standard constructor
    BOOL Create();

// Dialog Data
	//{{AFX_DATA(CAdderDialog)
	enum { IDD = IDD_ADDME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdderDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	CWnd* m_pParent;
	int m_nID;

	// Generated message map functions
	//{{AFX_MSG(CAdderDialog)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
