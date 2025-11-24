/********************** CTheApp *****************************/

class CTheApp : public CWinApp
{
public:
    virtual BOOL InitInstance();
};


/********************** CMainWindow *************************/

class CMainWindow : public CMDIFrameWnd
{
private:
    CMenu *	m_pMenuInit;

    BOOL CMainWindow::FileDlg( BOOL bOpen, int nMaxFile, LPSTR szFile,
		int nMaxFileTitle, LPSTR szFileTitle );
    CMapStringToPtr     mapCInstance;

public:
    CMainWindow();
    ~CMainWindow();

    void LoadFile(CString);
    void GetWindows(char * pchProgram, int iThread, CListWnd ** ppListWnd,
                    CTREEWND ** pTreeWnd);
    afx_msg int OnCreate(LPCREATESTRUCT);
    afx_msg void OnAbout();
    afx_msg void OnExit();
    afx_msg void OnOpen();
    afx_msg void OnHelp();

    DECLARE_MESSAGE_MAP()
};
