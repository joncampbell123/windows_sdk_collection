// cdlgsp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cdlgsp dialog

class cdlgsp : public CDialog
{
// Construction
public:
    cdlgsp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(cdlgsp)
    enum { IDD = IDD_CHOOSEPROVIDER };
    CListBox    m_lboxconnect;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(cdlgsp)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(cdlgsp)
    afx_msg void OnDblclkList();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
