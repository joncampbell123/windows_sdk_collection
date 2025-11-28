// pendlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPenWidthsDlg dialog

class CPenWidthsDlg : public CDialog
{
// Construction
public:
	CPenWidthsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPenWidthsDlg)
	enum { IDD = IDD_PEN_WIDTHS };
	int		m_nThinWidth;
	int		m_nThickWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPenWidthsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPenWidthsDlg)
	afx_msg void OnDefaultPenWidths();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
