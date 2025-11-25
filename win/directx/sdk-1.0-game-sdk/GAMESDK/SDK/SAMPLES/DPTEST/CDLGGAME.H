// cdlggame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// cdlggame dialog

class cdlggame : public CDialog
{
// Construction
public:
    cdlggame(CWnd* pParent = NULL);   // standard constructor
        void SetOpenFlags(DWORD dwFlags) {m_dwFlags = dwFlags;}

// Dialog Data
    //{{AFX_DATA(cdlggame)
    enum { IDD = IDD_SELSESSION };
    CListBox    m_lbox;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(cdlggame)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
        DWORD m_dwFlags;


    // Generated message map functions
    //{{AFX_MSG(cdlggame)
    afx_msg void OnDblclkLbSession();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnGetplayers();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};
