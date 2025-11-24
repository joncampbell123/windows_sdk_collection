/*
 -  S E N D M A I L . H
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Header for SENDMAIL.CPP
 *
 */

// threaddata is used so that a user has the ability to stop the tests
// since multiple threads are implemented(WIN32) or simulated(WIN16).
typedef struct _threaddata
{
    DWORD   dwThreadId;
    HWND    hWnd;
    BOOL    fSendToAll;
    lpMapiMessage   lpmsg;
    lpMapiRecipDesc lpRecips;
    ULONG   cRecips;
    ULONG   cMessages;
    ULONG   nSeconds;
    BOOL    fSubjectAlt;
    BOOL    fMessageTypeAlt;
    BOOL    fNoteTextAlt;
} THREADDATA, *LPTHREADDATA;


class CMainWindow : public CFrameWnd
{
    protected:
        virtual void PostNcDestroy();

    public:
        CMainWindow();

    /* Menu command message map functions */

    afx_msg void OnExit();
    afx_msg void OnClose();
    afx_msg void OnSend();
    afx_msg void OnAbout();

    DECLARE_MESSAGE_MAP()
};

class CTheApp : public CWinApp
{
    public:
        BOOL InitInstance();
        int  ExitInstance();
};

class CSendMailDialog : public CModalDialog
{
    public:

        char            szDisplayName[128];
        ULONG           cRecips;
        ULONG           cNewRecips;
        ULONG           ulTestNumber;
        lpMapiRecipDesc lpRecips;
        lpMapiRecipDesc lpNewRecips;

    CSendMailDialog( CWnd* pWndParent = NULL )
        : CModalDialog( IDD_SENDMAIL, pWndParent ) { }

    BOOL OnInitDialog();

    /* Message Handlers */
    afx_msg void OnOK();
    afx_msg void OnAddress();
    afx_msg void OnCancel();
    afx_msg void OnStop();
    afx_msg void OnFile();

    DECLARE_MESSAGE_MAP()
};

#ifdef WIN16
class CHiddenDialog : public CDialog
{
    public:
        LPTHREADDATA    lptd2;
                                        
    CHiddenDialog( LPTHREADDATA lptd, CWnd * pWndParent = NULL )
        : CDialog( IDD_HIDDEN, pWndParent ) 
    {
        lptd2 = (LPTHREADDATA) lptd;
        Create(IDD_HIDDEN);
    }
 
    virtual BOOL    OnInitDialog();
    void OnCancel();
};
#endif

long DoSend( LPTHREADDATA );
void FillFile( ULONG ulsize, HFILE hFile );

