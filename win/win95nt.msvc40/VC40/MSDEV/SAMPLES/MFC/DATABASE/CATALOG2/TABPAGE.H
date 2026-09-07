// TablePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTablePage dialog

class CTablePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTablePage)

// Construction
public:
	CTablePage();
	~CTablePage();

// Dialog Data
	//{{AFX_DATA(CTablePage)
	enum { IDD = IDD_TABLEPAGE };
	BOOL	m_bSynonyms;
	BOOL	m_bSystemTables;
	BOOL	m_bViews;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTablePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTablePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
