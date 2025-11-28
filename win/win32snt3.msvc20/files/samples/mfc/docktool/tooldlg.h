// tooldlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolDlg dialog

class CToolDlg : public CDialog
{
// Construction
public:
	CToolDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToolDlg)
	enum { IDD = IDD_TOOLBAR };
	BOOL    m_bBrowse;
	BOOL    m_bDebug;
	BOOL    m_bEdit;
	BOOL    m_bMain;
	BOOL    m_bResource;
	int     m_nToolTips;
	BOOL    m_bPalette;
	int     m_nColumns;
	int     m_nColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToolDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
