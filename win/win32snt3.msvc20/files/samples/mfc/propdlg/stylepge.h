// stylepge.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStylePage dialog

class CStylePage : public CPropertyPage
{
// Construction
public:
	CStylePage();

// Dialog Data
	//{{AFX_DATA(CStylePage)
	enum { IDD = IDD_STYLE };
	int     m_nShapeStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CStylePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void OnStyleClicked(UINT nCmdID);
	// Generated message map functions
	//{{AFX_MSG(CStylePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
