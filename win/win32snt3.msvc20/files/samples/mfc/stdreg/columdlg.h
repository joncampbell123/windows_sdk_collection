// columdlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CColSyntaxDlg dialog

class CColSyntaxDlg : public CDialog
{
// Construction
public:
	CColSyntaxDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColSyntaxDlg)
	enum { IDD = IDD_COLUMN_SYNTAX_DLG };
	CButton m_ctlOK;
	CListBox    m_ctlTypeInfoList;
	CString m_strSyntax;
	CString m_strListBoxInstruction;
	CString m_strColTypeInstruction;
	//}}AFX_DATA

	CMapSQLTypeToSyntax* m_pMapSQLTypeToSyntax;
	POSITION m_posMapSQLTypeToSyntax;
	SWORD m_swCurrentSQLType;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColSyntaxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void GetNextSQLTypeAndUpdateControls();
	void FillTypeInfoListbox();

	// Generated message map functions
	//{{AFX_MSG(CColSyntaxDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
