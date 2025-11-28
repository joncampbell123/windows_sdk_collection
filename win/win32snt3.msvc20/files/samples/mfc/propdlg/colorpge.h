// colorpge.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorPage dialog

class CColorPage : public CPropertyPage
{
// Construction
public:
	CColorPage();

// Dialog Data
	//{{AFX_DATA(CColorPage)
	enum { IDD = IDD_COLOR };
	int     m_nColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnColorClicked(UINT nCmdID);
	// Generated message map functions
	//{{AFX_MSG(CColorPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
