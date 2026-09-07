// OptionsD.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog

class COptionsDlg : public CPropertyPage
{
// Construction
public:
	COptionsDlg(UINT nIDCaption = 0);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_DLGOPTIONS };
	BOOL	m_bShowSystemObjects;
	BOOL	m_bShowWarnings;
	BOOL	m_bOpenODBC;
	UINT	m_nMaxRecords;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
