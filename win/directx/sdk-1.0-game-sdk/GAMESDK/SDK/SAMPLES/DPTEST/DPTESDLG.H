// dptesdlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDptestDlg dialog

class CDptestDlg : public CDialog
{
// Construction
public:
    CDptestDlg(CWnd* pParent = NULL);    // standard constructor
    void ListenProc(DWORD);

// Dialog Data
    //{{AFX_DATA(CDptestDlg)
    enum { IDD = IDD_DPTEST_DIALOG };
    CListBox    m_cLBoxResults;
    CString    m_csMsg;
    CString    m_csFrom;
    CString    m_csTo;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDptestDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CDptestDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnConnect();
    afx_msg void OnCreate();
    afx_msg void OnCreateplayer();
    afx_msg void OnGetmessage();
    afx_msg void OnSendmessage();
    virtual void OnCancel();
    afx_msg void OnSavesession();
    afx_msg void OnOpensession();
    afx_msg void OnBandwidth();
    afx_msg void OnClose();
    afx_msg void OnDestroyplayer();
    afx_msg void OnEnablenewplayers();
    afx_msg void OnEnumplayers();
    afx_msg void OnGetcaps();
    afx_msg void OnGetplayercaps();
    afx_msg void OnGetplayername();
    afx_msg void OnSetplayername();
    afx_msg void OnAddplayertogrp();
    afx_msg void OnCreategroup();
    afx_msg void OnDelplayerfromgrp();
    afx_msg void OnDestroygroup();
    afx_msg void OnEnumgrp();
    afx_msg void OnClear();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()


};
