// zoomdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CZoomDlg dialog

class CZoomDlg : public CDialog
{
// Construction
public:
	CZoomDlg(CWnd* pParent = NULL); // standard constructor

// Dialog Data
	//{{AFX_DATA(CZoomDlg)
	enum { IDD = IDD_CUSTZOOM };
	int     m_zoomX;
	int     m_zoomY;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CZoomDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
