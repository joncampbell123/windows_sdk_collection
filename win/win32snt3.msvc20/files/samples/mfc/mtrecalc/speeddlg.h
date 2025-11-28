// speedlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpeedDlg dialog

class CSpeedDlg : public CDialog
{
// Construction
public:
	CSpeedDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpeedDlg)
	enum { IDD = IDD_SPEED };
	int     m_nRecalcSpeedSeconds;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSpeedDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpeedDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
