/*
 -  A B V I E W . H
 -  Copyright (C) 1995 Microsoft Corporation
 *
 */

#ifndef __ABVIEW_H__
#define __ABVIEW_H__

#include "resource.h"

/* --------------------------------------------- */
/* Properties Dialog defines                     */
/* --------------------------------------------- */

#define fmProperty              ((ULONG) 0x00000001)
#define fmPropType              ((ULONG) 0x00000002)
#define fmPropValue             ((ULONG) 0x00000004)
#define fmAProperty             ((ULONG) 0xffffffff)

#define VALUES_LB_HOR_SIZE      2000
#define VALUES_LB_TAB1          140
#define VALUES_LB_TAB2          40
                                                                 
/*
 -  CMainWindow
 -
 *  Purpose:
 *      Defines an Object that holds all the info needed for the CLNT's
 *      main window.  
 *
 */

class CMainWindow : public CFrameWnd
{
protected:
    virtual void PostNcDestroy();
    afx_msg void OnSysColorChange();

public:
            void SetupOnStart();
            void CreateDisplayFonts();
            void DeleteDisplayFonts();
            void LoadDlls();
            void UnloadDlls();

    afx_msg void OnAbout();
    afx_msg void OnConfig();
    afx_msg void OnExit();
    afx_msg void OnClose();

    afx_msg void OnLogon();
    afx_msg void OnLogoff();
    afx_msg void OnRootContainer();
    afx_msg void OnAddress0();
    afx_msg void OnAddress1();
    afx_msg void OnAddress2();
    afx_msg void OnAddress3();
    afx_msg void OnResolveName();
    afx_msg void OnCreateOneOff();
    afx_msg void OnNewEntry();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )

    ON_COMMAND( IDM_ABOUT,          OnAbout )
    ON_COMMAND( IDM_CONFIG,         OnConfig )
    ON_COMMAND( IDM_EXIT,           OnExit )
    ON_WM_CLOSE()

    ON_COMMAND( IDM_LOGON,          OnLogon )
    ON_COMMAND( IDM_LOGOFF,         OnLogoff )
    ON_COMMAND( IDM_ROOTCONTAINER,  OnRootContainer )
    ON_COMMAND( IDM_ADDRESS0,       OnAddress0 )
    ON_COMMAND( IDM_ADDRESS1,       OnAddress1 )
    ON_COMMAND( IDM_ADDRESS2,       OnAddress2 )
    ON_COMMAND( IDM_ADDRESS3,       OnAddress3 )
    ON_COMMAND( IDM_RESOLVENAME,    OnResolveName )
    ON_COMMAND( IDM_CREATEONEOFF,   OnCreateOneOff )
    ON_COMMAND( IDM_NEWENTRY,       OnNewEntry )

END_MESSAGE_MAP()


/*
 -  CTheApp
 -
 *  Purpose:
 *      Defines an Object that holds all the info needed for the ABCLIENT
 *      Application.  See abclient.cpp for the code to the member functions
 *      and the message map.
 *
 */

class CTheApp : public CWinApp
{
protected:
    CMainWindow m_MainWnd;

public:
    BOOL InitInstance();
    int  ExitInstance();
    BOOL PreTranslateMessage( MSG*);
};


/*
 -  CConfigureDlg
 -
 *  Purpose:
 *      Defines the controls for the CConfigureDlg Dialog
 *
 *
 */

class CConfigureDlg : public CModalDialog
{
public:
     CConfigureDlg() 
        : CModalDialog( IDD_CONFIG )
    {
        DoModal();
    }

    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CConfigureDlg, CModalDialog)
END_MESSAGE_MAP()



/*
 -  CMAPIPropDlg
 -
 *  Purpose:
 *      Defines the controls for the CMAPIPropDlg Dialog
 *
 *
 */

class CMAPIPropDlg : public CModalDialog
{
protected:
    LPMAPIPROP      m_lpEntry;

public:
    CMAPIPropDlg() { };

    CMAPIPropDlg( LPMAILUSER lp )
        : CModalDialog( IDD_MAILUSER )
    {
        m_lpEntry = (LPMAPIPROP)lp;
    }

    CMAPIPropDlg( LPABCONT lp )
        : CModalDialog( IDD_CONTAINER )
    {
        m_lpEntry = (LPMAPIPROP)lp;
    }

    ~CMAPIPropDlg();

    void OnGetPropsThis();
    void OnGetPropListThis();
    void OnPropertyDLL();
    void OnDetails();
    void OnAddToPAB();
    void OnGetIDsFromNames();
    void OnGetNamesFromIDs();
    void OnDetailsTbl();
    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CMAPIPropDlg, CModalDialog)
    ON_COMMAND( IDC_GETPROPSTHIS,        OnGetPropsThis )
    ON_COMMAND( IDC_GETPROPLISTTHIS,     OnGetPropListThis )
    ON_COMMAND( IDC_PROPERTYDLL,         OnPropertyDLL )
    ON_COMMAND( IDC_DETAILSTHIS,         OnDetails )
    ON_COMMAND( IDC_ADDTOPAB,            OnAddToPAB )
    ON_COMMAND( IDC_IDSFROMNAMES,        OnGetIDsFromNames )
    ON_COMMAND( IDC_NAMESFROMIDS,        OnGetNamesFromIDs )
    ON_COMMAND( IDC_PR_DETAILS_TABLE,    OnDetailsTbl )
END_MESSAGE_MAP()



/*
 -  CMAPIContainerDlg
 -
 *  Purpose:
 *      Defines the controls for the CMAPIContainerDlg Dialog
 *
 *
 */

class CMAPIContainerDlg : public CMAPIPropDlg
{
protected:
    LPMAPITABLE     m_lpMAPITable;
    CString         m_Caption;
    CListBox        m_TableListBox;
    ULONG           m_ulObjType;
    ULONG           m_ulTableTypeFrom;
    ULONG           m_ulCurrentTableType;
    BOOL            m_fCONVENIENT_DEPTH;

    CTblList *      m_lpTblList;

public:
    CMAPIContainerDlg( LPABCONT lp, ULONG ulObjType, ULONG ulTableTypeFrom, BOOL fCONVENIENT_DEPTH )
        : CMAPIPropDlg( lp )
    {
        m_ulObjType          = ulObjType;
        m_ulTableTypeFrom    = ulTableTypeFrom;
        m_ulCurrentTableType = 0;
        m_lpMAPITable        = NULL;
        m_fCONVENIENT_DEPTH  = fCONVENIENT_DEPTH;
        DoModal();
    }

    ~CMAPIContainerDlg();

    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    
    void OnProperties();
    void OnGetProps();
    void OnGetPropList();
    
    void OnTableDLL();
    void OnPerformance();
    void OnDetailsSel();
    void OnTestThis();
    void OnTestSel();
    void OnAddToPABSel();
    void OnSetAsTargetThis();
    void OnSetAsTargetSel();
    void OnResolveNames();
    void OnOneOffTbl();

    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CMAPIContainerDlg, CMAPIPropDlg)
    ON_LBN_DBLCLK(IDC_CONTAINERLIST,     OnOK)
    ON_COMMAND( IDC_SHOWTABLEPROPS,      OnProperties )
    ON_COMMAND( IDC_GETPROPS,            OnGetProps )
    ON_COMMAND( IDC_GETPROPLIST,         OnGetPropList )
    ON_COMMAND( IDC_TABLEDLL,            OnTableDLL )
    ON_COMMAND( IDC_PERFORMANCE,         OnPerformance )
    ON_COMMAND( IDC_DETAILS,             OnDetailsSel )
    ON_COMMAND( IDC_ADDTOPABSEL,         OnAddToPABSel )
    ON_COMMAND( IDC_TESTSELDLL,          OnTestSel )
    ON_COMMAND( IDC_TESTTHISDLL,         OnTestThis )
    ON_COMMAND( IDC_SETASTARGETTHIS,     OnSetAsTargetThis )
    ON_COMMAND( IDC_SETASTARGETSEL,      OnSetAsTargetSel )
    ON_COMMAND( IDC_RESOLVENAMES,        OnResolveNames )
    ON_COMMAND( IDC_PR_CREATE_TEMPLATES, OnOneOffTbl )
END_MESSAGE_MAP()




/*
 -  CABMailUserDlg
 -
 *  Purpose:
 *      Defines the controls for the CABMailUserDlg Dialog
 *
 *
 */

class CABMailUserDlg : public CMAPIPropDlg
{
public:
     CABMailUserDlg( LPMAILUSER lp ) 
        : CMAPIPropDlg(lp)
    {
        DoModal();
    }

    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    void OnTestThis();
    void OnSetAsTargetThis();
    void OnGetPropListThis();
    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CABMailUserDlg, CMAPIPropDlg)
    ON_COMMAND( IDC_TESTTHISDLL,            OnTestThis )
    ON_COMMAND( IDC_SETASTARGETTHIS,        OnSetAsTargetThis )
    ON_COMMAND( IDC_GETPROPLIST,            OnGetPropListThis )
END_MESSAGE_MAP()


/*
 -  COneOffDlg
 -
 *  Purpose:
 *      Defines the controls for the COneOffDlg Dialog
 *
 *
 */

class COneOffDlg : public CModalDialog
{
public:
     COneOffDlg() 
        : CModalDialog( IDD_ONEOFF )
    {
        DoModal();
    }

    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(COneOffDlg, CModalDialog)
END_MESSAGE_MAP()


/*
 -  CAdrListDlg
 -
 *  Purpose:
 *      Defines the controls for displaying an Address List
 *
 */

class CAdrListDlg : public CModalDialog
{
protected:
    LPADRLIST   m_lpAdrList;
    CString     m_Caption;
    CListBox    m_AdrListBox;

public:
    CAdrListDlg( LPADRLIST lp )
        : CModalDialog(IDD_ADRLIST)
    {
        m_lpAdrList = lp;
        m_Caption = "Address List";
        DoModal();
    }

    ~CAdrListDlg();
    
    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    void OnRecipOptions();
    void OnOpenEntry();
    void OnPrepareRecips();
    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CAdrListDlg, CModalDialog)
    ON_LBN_DBLCLK( IDC_ADRLIST,     OnOpenEntry )      // double click for OK
    ON_COMMAND( IDC_RECIPOPTIONS,   OnRecipOptions )
    ON_COMMAND( IDC_OPENENTRY,      OnOpenEntry )
    ON_COMMAND( IDC_PREPARERECIPS,  OnPrepareRecips )
END_MESSAGE_MAP()


/*
 -  CPropertiesDlg
 -
 *  Purpose:
 *      Defines the controls for the CPropertiesDlg class.
 *      Displays properties given for a single row of an
 *      SRowSet structure.
 *
 */

class CPropertiesDlg : public CDialog
{
protected:
    ULONG           m_cValues;
    LPSPropValue    m_lpProps;
    CString         m_Caption;
    ULONG           m_fmDisplay;
    CListBox        m_ListBox;

    void RenderProperties( ULONG, LPSPropValue, ULONG );

public:
    CPropertiesDlg( ULONG cValues, LPSPropValue lpProps, LPSTR szCaption, ULONG fmDisplay )
    {
        m_cValues   = cValues;
        m_lpProps   = lpProps;
        m_Caption   = szCaption;
        m_fmDisplay = fmDisplay;
        Create(IDD_PROPERTIES);
    }
    
    BOOL OnInitDialog();
    void OnCancel();
    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CPropertiesDlg, CDialog)
    ON_LBN_DBLCLK(IDC_PROPLIST,     OnCancel)      // double click for Cancel
END_MESSAGE_MAP()


/*
 -  CNamedIDsDlg
 -
 *  Purpose:
 *      Defines the controls for the CNamedIDsDlg class.
 *      Displays an array of Named IDs.
 *
 */

class CNamedIDsDlg : public CDialog
{
protected:
    ULONG               m_cPropNames;
    LPMAPINAMEID  FAR * m_lppPropNames;
    CString             m_Caption;
    CListBox            m_ListBox;

    void RenderNamedIDs( ULONG, LPMAPINAMEID FAR * );

public:
    CNamedIDsDlg( ULONG cPropNames, LPMAPINAMEID FAR * lppPropNames, LPSTR szCaption )
    {
        m_cPropNames    = cPropNames;
        m_lppPropNames  = lppPropNames;
        m_Caption       = szCaption;
        Create(IDD_PROPERTIES);
    }
    
    BOOL OnInitDialog();
    void OnCancel();
    DECLARE_MESSAGE_MAP();
};

BEGIN_MESSAGE_MAP(CNamedIDsDlg, CDialog)
    ON_LBN_DBLCLK(IDC_PROPLIST,     OnCancel)      // double click for Cancel
END_MESSAGE_MAP()



/*
 -  CGetError
 -
 *  Purpose:
 *      Defines the methods to Get Error Strings.
 *
 *
 */

class CGetError
{
    private:
        char    m_szMessage[80];
        char    m_szResult[80];
        char    m_szBuffer[80];

    public:
        CGetError()
        {
            m_szMessage[0]  = '\0';
            m_szResult[0]   = '\0';
        };

        ~CGetError() { };

        LPSTR SzError( LPSTR, SCODE );
#ifdef WIN16
        LPSTR SzError( LPSTR, HRESULT );
#endif
};


/*
 -  CResolveNameDlg
 -
 *  Purpose:
 *      Prompt user for Name to call ResolveName with.
 *
 */

class CResolveNameDlg : public CModalDialog
{
private:
    char            m_szPartialName[256];
    BOOL            m_fCallResolveNames;
    LPABCONT        m_lpABContainer;
    
public:
    CResolveNameDlg()
        : CModalDialog( IDD_RESOLVENAME )
    {
        m_fCallResolveNames = FALSE;
        DoModal();
    }

    CResolveNameDlg( LPABCONT lp)
        : CModalDialog( IDD_RESOLVENAME )
    {
        m_fCallResolveNames = TRUE;
        m_lpABContainer     = lp;
        DoModal();
    }

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    DECLARE_MESSAGE_MAP();

    char* GetName()     { return m_szPartialName; }
};

BEGIN_MESSAGE_MAP(CResolveNameDlg, CModalDialog)
    ON_COMMAND( IDOK,             OnOK )
    ON_COMMAND( IDCANCEL,         OnCancel )
END_MESSAGE_MAP()


/*
 * TABLEVU functions
 */

extern HINSTANCE    hlibTBLVU;

typedef BOOL (*LPFNVIEWMAPITABLE)(
    LPMAPITABLE FAR *lppMAPITable,
    HWND hWnd
);    

extern LPFNVIEWMAPITABLE lpfnViewMapiTable;

#define TBLVUViewMapiTable (*lpfnViewMapiTable)


/*
 * PROPVU functions
 */

extern HINSTANCE    hlibPROPVU;

typedef BOOL (*LPFNVIEWMAPIPROP)(
    LPSTR       lpszName,
    LPMAPIPROP  FAR *lppMAPIProp,
    LPVOID      lpvDest,
    HWND hWnd
);    

extern LPFNVIEWMAPIPROP lpfnViewMapiProp;

#define PROPVUViewMapiProp (*lpfnViewMapiProp)


/*
 * ABPERF functions
 */

extern HINSTANCE    hlibPERF;

typedef BOOL (*LPFNPERFORMANCE)(
    LPADRBOOK   FAR *lppab,
    LPMAPITABLE FAR *lppMAPITable,
    HWND hWnd
);    

extern LPFNPERFORMANCE lpfnPerformance;

#define ABPERFormance (*lpfnPerformance)


/*
 * ABTEST functions
 */

extern HINSTANCE    hlibTEST;

typedef BOOL (*LPFNTEST)(
    LPADRBOOK     FAR *lppab,
    ULONG              ulObjType,
    LPMAPIPROP    FAR *lppMAPIProp,
    HWND hWnd
);    

extern LPFNTEST lpfnTest;

#define ABTest (*lpfnTest)

#endif  /* __ABVIEW_H__ */
