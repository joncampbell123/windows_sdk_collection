/*
 -  A B V I E W . C P P
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Main source module for ABVIEW.  This is an MFC application.
 *
 */

#ifndef CHICAGO
    #define CTL3D
#endif

#define CROWSREQUESTED 20

#include <memory.h>
#include <stdarg.h>

#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>

#ifdef WIN16
#include <compobj.h>
#endif

#ifdef WIN32
#include <objbase.h>
#include <objerror.h>
#ifdef CHICAGO
#include <ole2.h>
#endif
#endif

#ifdef CTL3D
    #include <ctl3d.h>
#endif

#include <mapiwin.h>
#include <mapix.h>
#include <strtbl.h>
#include <misctool.h>

#include "resource.h"
#include "tbllist.h"
#include "abview.h"

HINSTANCE hlibTBLVU = (HINSTANCE)NULL;
LPFNVIEWMAPITABLE lpfnViewMapiTable = NULL;

HINSTANCE hlibPROPVU = (HINSTANCE)NULL;
LPFNVIEWMAPIPROP lpfnViewMapiProp = NULL;

HINSTANCE hlibPERF = (HINSTANCE)NULL;
LPFNPERFORMANCE lpfnPerformance = NULL;

HINSTANCE hlibTEST = (HINSTANCE)NULL;
LPFNTEST lpfnTest = NULL;

LPMAPISESSION   lpSession       = NULL;
LPADRBOOK       lpAdrBook       = NULL;
ULONG           ulModlessABHwnd = 0;

/*
 -  Global Variables
 */

BOOL        fOpenEntry        = FALSE;
BOOL        fSetAsTarget      = FALSE;
BOOL        fUseUNICODE       = FALSE;
BOOL        fQueryForTable    = FALSE;
BOOL        fUNICODE          = FALSE;
BOOL        fCONVENIENT_DEPTH = FALSE;
BOOL        fObjectType       = FALSE;
CFont*      pBoldFont         = NULL;
CFont*      pNormFont         = NULL;
CFont*      pLightFont        = NULL;
ULONG       cbTargetEntryID   = 0;
LPENTRYID   lpTargetEntryID   = NULL;

LPSPropTagArray ABH_PropTagArray = NULL;

/*
 -  Support Function Prototypes
 */

void  Logprintf( LPSTR, ... );
void  _FreeRowSet( LPSRowSet );
void  DisplayError( LPUNKNOWN, HRESULT, LPSTR );
SCODE ScGetRow( LPMAPITABLE, int, LPSRowSet * );
SCODE ScOpenEntry( ULONG, LPENTRYID, ULONG, ULONG, BOOL );
void  CreateMAPIGuid( LPGUID );

CTheApp theApp;

/*
 *  CTheApp Methods.
 */

/*
 -  InitInstance
 -
 *  Purpose:
 *      When any CTheApp object is created, this member function is
 *      automatically  called.  The main window of the application
 *      is created and shown here.
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      TRUE if the initialization is successful.
 *
 */

BOOL CTheApp::InitInstance()
{
    CRect rect;
    m_MainWnd.LoadAccelTable( "MainAccelTable" );
    MAPIInitialize(NULL);

#ifdef CTL3D
    Ctl3dRegister( AfxGetInstanceHandle() );   // Register Ctl3D controls
    Ctl3dAutoSubclass( AfxGetInstanceHandle());
#endif

    rect.SetRect(MW_LEFT, MW_TOP, MW_RIGHT, MW_BOTTOM);

    if(!m_MainWnd.Create( NULL,
                            "Client Test Application",
                            WS_OVERLAPPEDWINDOW,
                            rect,
                            NULL,
                            MAKEINTRESOURCE(AFX_IDI_STD_FRAME)/*menu*/))
    return FALSE;

    TRACE( "ABClnt\n" );
    m_pMainWnd = &m_MainWnd;
    m_MainWnd.ShowWindow( m_nCmdShow );
    m_MainWnd.UpdateWindow();

    m_MainWnd.SetupOnStart();
    m_MainWnd.CreateDisplayFonts();

    return TRUE;
}

/*
 -  ExitInstance
 -
 *  Purpose:
 *      When a CWinApp application is exited, this function is
 *      automatically called.
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      TRUE if the Exit is successful.
 *
 */

int CTheApp::ExitInstance()
{

#ifdef CTL3D
    Ctl3dUnregister(AfxGetInstanceHandle());
#endif

    m_MainWnd.DeleteDisplayFonts();

    return CWinApp::ExitInstance();
}


/*
 -  PreTranslateMessage
 -
 *  Purpose:
 *      Allow this application to filter window messages.  This is needed
 *      for the 0 Well Address case.  The dialog must tie into the message
 *      loop.
 *
 *  Parameters:
 *      Pointer to a message structure containing the message to process
 *
 *  Returns:
 *      TRUE if the message was fully processed and should not be passed
 *      to TranslateMessage and PreTranslateMessage.  FALSE if the message
 *      should be processed in the normal way.
 *
 */

BOOL CTheApp::PreTranslateMessage(MSG* pMsg)
{
    if (!ulModlessABHwnd || !IsDialogMessage((HWND) ulModlessABHwnd, pMsg))
    {
        return CWinApp::PreTranslateMessage(pMsg);
    }
    
    return TRUE;
}

/*
 *  CMainWindow Methods.
 */


/*
 -  CMainWindow::
 -  OnAbout
 -
 *  Purpose:
 *      Create a CModalDialog object using the "AboutBox" resource.
 *
 */

void CMainWindow::OnAbout()
{
    CModalDialog about( "AboutBox", this );
    about.DoModal();
}


/*
 -  CMainWindow::
 -  SetupOnStart
 -
 *  Purpose:
 *      Called on Startup of Application.  Initializes the
 *      Address Book viewer application.
 *
 */

void CMainWindow::SetupOnStart()
{
    int     nOpenEntry;
    int     nSetAsTarget;
    int     nUseUNICODE;
    int     nUNICODE;
    int     nCONVENIENT_DEPTH;
    int     nQueryForTable;
    int     nObjectType;

    m_bAutoMenuEnable = FALSE; /*Menu to work like MFC1*/
    CMenu* menu = GetMenu();

    menu->EnableMenuItem( IDM_LOGON,            MF_GRAYED );
    menu->EnableMenuItem( IDM_LOGOFF,           MF_GRAYED );
    menu->EnableMenuItem( IDM_ROOTCONTAINER,    MF_GRAYED );
    menu->EnableMenuItem( IDM_ADDRESS0,         MF_GRAYED );
    menu->EnableMenuItem( IDM_ADDRESS1,         MF_GRAYED );
    menu->EnableMenuItem( IDM_ADDRESS2,         MF_GRAYED );
    menu->EnableMenuItem( IDM_ADDRESS3,         MF_GRAYED );
    menu->EnableMenuItem( IDM_RESOLVENAME,      MF_GRAYED );
    menu->EnableMenuItem( IDM_CREATEONEOFF,     MF_GRAYED );
    menu->EnableMenuItem( IDM_NEWENTRY,         MF_GRAYED );
    menu->EnableMenuItem( IDM_PREPARERECIPS,    MF_GRAYED );

    CMainWindow::LoadDlls();
    CMainWindow::OnLogon();

    if( (nOpenEntry = GetProfileInt( "ABView", "OPENENTRY", 0)) == 1)
        fOpenEntry = TRUE;

    if( (nSetAsTarget = GetProfileInt( "ABView", "SETASTARGET", 0)) == 1)
        fSetAsTarget = TRUE;

    if( (nUseUNICODE = GetProfileInt( "ABView", "UNICODE", 0)) == 1)
        fUseUNICODE = TRUE;
        
    if( (nUNICODE = GetProfileInt( "ABView", "UNICODEFLAG", 0)) == 1)
        fUNICODE = TRUE;

    if( (nCONVENIENT_DEPTH = GetProfileInt( "ABView", "DEPTHFLAG", 0)) == 1)
        fCONVENIENT_DEPTH = TRUE;
        
    if( (nQueryForTable = GetProfileInt( "ABView", "QUERYTABLE", 0)) == 1)
        fQueryForTable = TRUE;

    if( (nObjectType = GetProfileInt( "ABView", "OBJECTTYPE", 0)) == 1)
        fObjectType = TRUE;

    if(fUNICODE)
        ABH_PropTagArray = (LPSPropTagArray)&ABHW_PropTagArray;
    else
        ABH_PropTagArray = (LPSPropTagArray)&ABHN_PropTagArray;
}


/*
 -
 -  CMainWindow::
 -  LoadDlls
 -
 *  Purpose:
 *      Loads Extensibility DLLs into Memory.
 *
 */

void CMainWindow::LoadDlls()
{
    char *  szTablevu;
    char    szTblLibName[124];
    char    szPrpLibName[124];
    char    szPerLibName[124];
    char    szTstLibName[124];
    char    szMsgBuf[256];

//Check for an alternate Tablevu DLL from the win.ini.  If there is one, then
//attempt to load it.  If the DLL can't load, then deactivate access to
//Tablevu
#ifdef WIN16
    szTablevu = "tblvu.dll";
#else
    szTablevu = "tblvu32.dll";
#endif

    GetProfileString( "ABView", "TABLEVUDLLNAME", szTablevu, szTblLibName, sizeof(szTblLibName) );

#ifdef WIN16
    if ((UINT)(hlibTBLVU = LoadLibrary (szTblLibName)) < 32)
#else
    if (!(hlibTBLVU = LoadLibrary (szTblLibName)))
#endif
    {
        //Since this application can run without any Extensibility DLL
        //being loaded, the following warning box will not be displayed
        //for the Tablevu feature.  Abview defaults to load a DLL named
        //tblvu.dll, even when it has no entry in the Profile.  This
        //warning can become anoying.

        hlibTBLVU = (HINSTANCE) NULL;
        goto out;
    }

    if (!(lpfnViewMapiTable = (LPFNVIEWMAPITABLE)GetProcAddress (hlibTBLVU, "ViewMapiTable") ))
    {
        MessageBox("Cannot Load ViewMapiTable process address.","Error", MB_ICONSTOP | MB_OK);
        FreeLibrary (hlibTBLVU);
        hlibTBLVU = (HINSTANCE) NULL;
        goto out;
    }

out:


        //Attempt to load a Propvu DLL.  If the DLL can't load, or one is not
        //listed to load, then this function is deactivated.

    GetProfileString( "ABView", "PROPVUDLLNAME", "", szPrpLibName, sizeof(szPrpLibName) );

    if (*szPrpLibName)
    {
#ifdef WIN16
        if ((UINT)(hlibPROPVU = LoadLibrary (szPrpLibName)) < 32)
#else
        if (!(hlibPROPVU = LoadLibrary (szPrpLibName)))
#endif
        {
            wsprintf(szMsgBuf,"Cannot Load %s.  Propvu Feature will be turned off.", szPrpLibName);
            MessageBox(szMsgBuf,"Error", MB_ICONSTOP | MB_OK);
            hlibPROPVU = (HINSTANCE) NULL;
            goto out2;
        }

        if (!(lpfnViewMapiProp = (LPFNVIEWMAPIPROP)GetProcAddress (hlibPROPVU, "ViewMapiProp") ))
        {
            MessageBox("Cannot Load ViewMapiProp process address.","Error", MB_ICONSTOP | MB_OK);
            FreeLibrary (hlibPROPVU);
            hlibPROPVU = (HINSTANCE) NULL;
            goto out2;
        }
    }

out2:

//Attempt to load a performance DLL.  If the DLL can't load, or one is not
//listed to load, then performance is deactivated.

    GetProfileString( "ABView", "PERFDLLNAME", "", szPerLibName, sizeof(szPerLibName) );

    if (*szPerLibName)
    {
#ifdef WIN16
        if ((UINT)(hlibPERF = LoadLibrary (szPerLibName)) < 32)
#else
        if (!(hlibPERF = LoadLibrary (szPerLibName)))
#endif
        {
            wsprintf(szMsgBuf,"Cannot Load %s.  Performance Feature will be turned off.", szPerLibName);
            MessageBox(szMsgBuf,"Error", MB_ICONSTOP | MB_OK);
            hlibPERF = (HINSTANCE) NULL;
            goto out3;
        }

        if (!(lpfnPerformance  =  (LPFNPERFORMANCE)GetProcAddress (hlibPERF, "ABPerformance") ))
        {
            MessageBox("Cannot Load ABPerformance process address.","Error", MB_ICONSTOP | MB_OK);
            FreeLibrary (hlibPERF);
            hlibPERF = (HINSTANCE) NULL;
            goto out3;
        }
    }

out3:

//Attempt to load a test DLL.  If the DLL can't load, or one is not
//listed to load, then test is deactivated.

    GetProfileString( "ABView", "TESTDLLNAME", "", szTstLibName, sizeof(szTstLibName) );

    if (*szTstLibName)
    {
#ifdef WIN16
    if ((UINT)(hlibTEST = LoadLibrary (szTstLibName)) < 32)
#else
    if (!(hlibTEST = LoadLibrary (szTstLibName)))
#endif
    {
        wsprintf(szMsgBuf,"Cannot Load %s.  Test Feature will be turned off.", szTstLibName);
        MessageBox(szMsgBuf,"Error", MB_ICONSTOP | MB_OK);
        hlibTEST = (HINSTANCE) NULL;
        goto out4;
    }

    if (!(lpfnTest  =  (LPFNTEST)GetProcAddress (hlibTEST, "ABTest") ))
    {
        MessageBox("Cannot Load ABTest process address.","Error", MB_ICONSTOP | MB_OK);
        FreeLibrary (hlibTEST);
        hlibTEST = (HINSTANCE) NULL;
        goto out4;
    }
}

out4: {}

}


/*
 -
 -  CMainWindow::
 -  UnloadDlls
 -
 *  Purpose:
 *      Unloads Extensibility DLLs from Memory.
 *
 */

void CMainWindow::UnloadDlls()
{
    if (hlibTBLVU)
    {
        FreeLibrary (hlibTBLVU);
        hlibTBLVU = (HINSTANCE) NULL;
    }

    if (hlibPROPVU)
    {
        FreeLibrary (hlibPROPVU);
        hlibPROPVU = (HINSTANCE) NULL;
    }

    if (hlibPERF)
    {
        FreeLibrary (hlibPERF);
        hlibPERF = (HINSTANCE) NULL;
    }

    if (hlibTEST)
    {
        FreeLibrary (hlibTEST);
        hlibTEST = (HINSTANCE) NULL;
    }
}


/*
 -  CMainWindow::
 -  OnExit
 -
 *  Purpose:
 *      Asks the application to Close itself.
 *
 */

void CMainWindow::OnExit()
{
    SendMessage( WM_CLOSE );
}


/*
 -  CMainWindow::
 -  OnClose
 *
 *  Purpose:
 *      This displays a dialog querying the user if they would like to
 *      close this application.  If yes, then then a call is made to
 *      ensure that the user has logged off prior to ending the session.
 *
 */

void CMainWindow::OnClose()
{
    OnLogoff();
    MAPIUninitialize();
    CMainWindow::UnloadDlls();
    DestroyWindow();
}


/*
 -  CMainWindow::
 -  PostNcDestroy
 *
 *  Purpose:
 *      Catch the WM_DESTROY window message.
 *
 *      Cleanup of the main window will occur in the OnClose function, with
 *      the DestroyWindow() call.
 *
 *      ExitInstance() will perform the remaining cleanup.
 *
 */

void CMainWindow::PostNcDestroy()
{
    //Don't delete this.  This is required for 16Bit MFC to exit gracefully.
}


/*
 -  CMainWindow::
 -  OnSysColorChange
 *
 *  Purpose:
 *      Handles changing the applications colors when a system
 *      color change is requested.  This message overide is
 *      required when using Ctrl3D to display windows.
 *
 */

void CMainWindow::OnSysColorChange()
{
#ifdef CTL3D
    Ctl3dColorChange();
#else
    CMainWindow::OnSysColorChange();
#endif
}

/*
 -  CMainWindow::
 -  OnConfig
 *
 *  Purpose:
 *      Allows for easy modification of the abview section of the
 *      win.ini.  This section is used for configuring the test DLLs
 *      to be called when exercising objects and tables.
 *
 */

void CMainWindow::OnConfig()
{
    CConfigureDlg dlgConfigure;
}

/*
 -  CConfigureDlg::
 -  OnInitDialog
 *
 *  Purpose:
 *
 */

BOOL CConfigureDlg::OnInitDialog()
{
    char *  szTablevu;
    char    szTblLibName[124];
    char    szPrpLibName[124];
    char    szPerLibName[124];
    char    szTstLibName[124];
    int     nOpenEntry          = 0;
    int     nSetAsTarget        = 0;
    int     nUseUNICODE         = 0;
    int     nUNICODE            = 0;
    int     nCONVENIENT_DEPTH   = 0;
    int     nQueryForTable      = 0;
    int     nObjectType         = 0;

#ifdef WIN16
    szTablevu = "tblvu.dll";
#else
    szTablevu = "tblvu32.dll";
#endif

    GetProfileString( "ABView", "TABLEVUDLLNAME", szTablevu, szTblLibName, sizeof(szTblLibName) );
    GetProfileString( "ABView", "PROPVUDLLNAME", "", szPrpLibName, sizeof(szPrpLibName) );
    GetProfileString( "ABView", "PERFDLLNAME", "", szPerLibName, sizeof(szPerLibName) );
    GetProfileString( "ABView", "TESTDLLNAME", "", szTstLibName, sizeof(szTstLibName) );

    if( (nOpenEntry = GetProfileInt( "ABView", "OPENENTRY", 0)) == 1)
        fOpenEntry = TRUE;
    else
        fOpenEntry = FALSE;

    if( (nSetAsTarget = GetProfileInt( "ABView", "SETASTARGET", 0)) == 1)
        fSetAsTarget = TRUE;
    else
        fSetAsTarget = FALSE;

    if( (nUseUNICODE = GetProfileInt( "ABView", "UNICODE", 0)) == 1)
        fUseUNICODE = TRUE;
    else
        fUseUNICODE = FALSE;

    if( (nUNICODE = GetProfileInt( "ABView", "UNICODEFLAG", 0)) == 1)
        fUNICODE = TRUE;
    else
        fUNICODE = FALSE;
        
    if( (nCONVENIENT_DEPTH = GetProfileInt( "ABView", "DEPTHFLAG", 0)) == 1)
        fCONVENIENT_DEPTH = TRUE;
    else
        fCONVENIENT_DEPTH = FALSE;

    if( (nQueryForTable = GetProfileInt( "ABView", "QUERYTABLE", 0)) == 1)
        fQueryForTable = TRUE;
    else
        fQueryForTable = FALSE;
        
    if( (nObjectType = GetProfileInt( "ABView", "OBJECTTYPE", 0)) == 1)
        fObjectType = TRUE;
    else
        fObjectType = FALSE;

    SetDlgItemText(IDC_TABLEVUDLL,    szTblLibName );
    SetDlgItemText(IDC_PROPERTYVUDLL, szPrpLibName );
    SetDlgItemText(IDC_PERFDLL,       szPerLibName );
    SetDlgItemText(IDC_TESTDLL,       szTstLibName );

    if(fOpenEntry)
        CheckDlgButton(IDC_OPENENTRYF, 1);

    if(fSetAsTarget)
        CheckDlgButton(IDC_SETTARGETCONTF, 1);

    if(fUseUNICODE)
        CheckDlgButton(IDC_UNICODEF, 1);

    if(fUNICODE)
        CheckDlgButton(IDC_UNICODEFLAG, 1);
        
    if(fCONVENIENT_DEPTH)
        CheckDlgButton(IDC_CONVENIENTDEPTH, 1);

    if(fQueryForTable)
        CheckDlgButton(IDC_TABLEREQUEST, 1);

    if(fObjectType)
        CheckDlgButton(IDC_OBJECTTYPE, 1);

    return TRUE;
}

/*
 -  CConfigureDlg::
 -  OnOK
 *
 *  Purpose:
 *
 */

void CConfigureDlg::OnOK()
{
    char * szSection   = "ABView";
    char * szKeyTblLib = "TABLEVUDLLNAME";
    char * szKeyPrpLib = "PROPVUDLLNAME";
    char * szKeyPerLib = "PERFDLLNAME";
    char * szKeyTstLib = "TESTDLLNAME";
    char * szKeyOpenEntry = "OPENENTRY";
    char * szKeySetAsTarget = "SETASTARGET";
    char * szKeyUseUNICODE = "UNICODE";
    char * szKeyUNICODEFLAG = "UNICODEFLAG";
    char * szKeyCONVENIENTDEPTH = "DEPTHFLAG";
    char * szKeyQueryForTable = "QUERYTABLE";
    char * szKeyObjectType = "OBJECTTYPE";

    char szTblLibName[124];
    char szPrpLibName[124];
    char szPerLibName[124];
    char szTstLibName[124];

    GetDlgItemText(IDC_TABLEVUDLL,    szTblLibName, 124);
    GetDlgItemText(IDC_PROPERTYVUDLL, szPrpLibName, 124);
    GetDlgItemText(IDC_PERFDLL,       szPerLibName, 124);
    GetDlgItemText(IDC_TESTDLL,       szTstLibName, 124);
    fOpenEntry = IsDlgButtonChecked( IDC_OPENENTRYF );
    fSetAsTarget = IsDlgButtonChecked( IDC_SETTARGETCONTF );
    fUseUNICODE = IsDlgButtonChecked( IDC_UNICODEF );
    fUNICODE = IsDlgButtonChecked( IDC_UNICODEFLAG );
    fCONVENIENT_DEPTH = IsDlgButtonChecked( IDC_CONVENIENTDEPTH );
    fQueryForTable = IsDlgButtonChecked( IDC_TABLEREQUEST );
    fObjectType = IsDlgButtonChecked( IDC_OBJECTTYPE );

    if( *szTblLibName )
        WriteProfileString( szSection, szKeyTblLib, szTblLibName );
    else
        WriteProfileString( szSection, szKeyTblLib, NULL );

    if( *szPrpLibName )
        WriteProfileString( szSection, szKeyPrpLib, szPrpLibName );
    else
        WriteProfileString( szSection, szKeyPrpLib, NULL );

    if( *szPerLibName )
        WriteProfileString( szSection, szKeyPerLib, szPerLibName );
    else
        WriteProfileString( szSection, szKeyPerLib, NULL );

    if( *szTstLibName )
        WriteProfileString( szSection, szKeyTstLib, szTstLibName );
    else
        WriteProfileString( szSection, szKeyTstLib, NULL );

    if(fOpenEntry)
        WriteProfileString( szSection, szKeyOpenEntry, "1" );
    else
        WriteProfileString( szSection, szKeyOpenEntry, "0" );

    if(fSetAsTarget)
        WriteProfileString( szSection, szKeySetAsTarget, "1" );
    else
        WriteProfileString( szSection, szKeySetAsTarget, "0" );

    if(fUseUNICODE)
        WriteProfileString( szSection, szKeyUseUNICODE, "1" );
    else
        WriteProfileString( szSection, szKeyUseUNICODE, "0" );

    if(fUNICODE)
        WriteProfileString( szSection, szKeyUNICODEFLAG, "1" );
    else
        WriteProfileString( szSection, szKeyUNICODEFLAG, "0" );

    if(fCONVENIENT_DEPTH)
        WriteProfileString( szSection, szKeyCONVENIENTDEPTH, "1" );
    else
        WriteProfileString( szSection, szKeyCONVENIENTDEPTH, "0" );

    if(fQueryForTable)
        WriteProfileString( szSection, szKeyQueryForTable, "1" );
    else
        WriteProfileString( szSection, szKeyQueryForTable, "0" );

    if(fObjectType)
        WriteProfileString( szSection, szKeyObjectType, "1" );
    else
        WriteProfileString( szSection, szKeyObjectType, "0" );

    if(fUNICODE)
        ABH_PropTagArray = (LPSPropTagArray)&ABHW_PropTagArray;
    else
        ABH_PropTagArray = (LPSPropTagArray)&ABHN_PropTagArray;


    EndDialog(IDOK);
}

/*
 -  CConfigureDlg::
 -  OnCancel
 *
 *  Purpose:
 *
 */

void CConfigureDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*
 -  CMainWindow::
 -  OnAdminServices
 *
 *  Purpose:
 *      Allow a method to create a profile and configure it.  For more 
 *      complex configurations, the config app should be used.
 *
 */

void CMainWindow::OnAdminServices()
{
    CAdminDlg dlgAdminServices;
}


/*
 -  CAdminDlg::
 -  OnInitDialog
 *
 *  Purpose:
 *      Initialize the dialog with either default information, or if it
 *      exists, read it from an ini file.
 *
 */

BOOL CAdminDlg::OnInitDialog()
{
    SetDlgItemText(IDC_PROFILENAME, "foo" );
    SetDlgItemText(IDC_SERVERNAME,  "Server" );

    return TRUE;
}


/*
 -  CAdminDlg::
 -  OnOK
 *
 *  Purpose:
 *      Create the profile and add the properties requested.
 *
 */

void CAdminDlg::OnOK()
{
    CGetError       E;
    SCODE           sc              = S_OK;
    HRESULT         hResult         = NULL;
    LPPROFADMIN     lpProfAdmin     = NULL;
    LPSERVICEADMIN  lpServiceAdmin  = NULL;
    ULONG           ulUIParam       = (ULONG)(void *)m_hWnd;
    LPMAPISESSION   lpTempSession   = NULL;

    char szProfileName[256];
    char szServerName[256];
    char * szServiceName            = "ABVU_Admin";
    char * szServices               = "Services";

    char szSysPath[MAX_PATH];
    LPTSTR lpszSysPath = szSysPath;
    CString szMapiServicesIni; 

    GetSystemDirectory(lpszSysPath, MAX_PATH);
    szMapiServicesIni += lpszSysPath;
    szMapiServicesIni += "\\mapisvc.inf";

    GetDlgItemText(IDC_PROFILENAME,  szProfileName, 256);
    GetDlgItemText(IDC_SERVERNAME,   szServerName, 256);

    //Create the entries in the Services.ini for Admin connection
    HackServicesIni(szServerName, szServices, szServiceName, szMapiServicesIni);

    if( FAILED(hResult = MAPIAdminProfiles( 0, &lpProfAdmin )))
    {
        MessageBox( E.SzError("MAPIAdminProfiles", GetScode(hResult)), "Client", MB_OK );
        goto error;
    }

    if( FAILED(hResult = lpProfAdmin->CreateProfile(
                            szProfileName,
                            "",
                            ulUIParam,
                            0 )))
    {
        DisplayError( lpProfAdmin, hResult, "CreateProfile" );
        goto error;
    }

    if( FAILED(sc = MAPILogon( ulUIParam,
                            szProfileName,
                            NULL,
                            MAPI_EXTENDED | MAPI_NO_MAIL,
                            0,
                            (LPLHANDLE) &lpTempSession )))
    {
        MessageBox( E.SzError("MAPILogon", sc), "Client", MB_OK );
        goto error;
    }


    if( FAILED(hResult = lpTempSession->AdminServices(
                            0,
                            &lpServiceAdmin )))
    {
        DisplayError( lpSession, hResult, "AdminServices" );
        goto error;
    }

    if( FAILED(hResult = lpServiceAdmin->CreateMsgService(
                            szServiceName,
                            "",
                            ulUIParam,
                            0 )))
    {
        DisplayError( lpServiceAdmin, hResult, "CreateMsgService" );
        goto error;
    }


    if( FAILED(hResult = lpProfAdmin->SetDefaultProfile( szProfileName, 0 )))
    {
        DisplayError( lpProfAdmin, hResult, "SetDefaultProfile" );
        goto error;
    }

    error:

    if(lpTempSession)
    {
        if( FAILED(hResult = lpTempSession->Logoff( (ULONG)(void *)m_hWnd,
                            0,
                            0 )))
                            {
        DisplayError( lpTempSession, hResult, "Logoff" );
    }

    lpTempSession->Release();
    lpTempSession = NULL;
    }    

    if (lpProfAdmin)
    lpProfAdmin->Release;

    if (lpServiceAdmin)
    lpServiceAdmin->Release; 

    //Remove the Service from the INI file, so that Config does not show.
    WritePrivateProfileString( szServices, szServiceName, NULL, szMapiServicesIni );

    EndDialog(IDOK);
    }


/*
 -  CAdminDlg::
 -  OnCancel
 *
 *  Purpose:
 *      Cancel any changes made on this dialog.
 *
 */

void CAdminDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*
 -  CAdminDlg::
 -  HackServicesIni
 *
 *  Purpose:
 *      Modify the ServicesIni to create references for Admin Profile.
 *
 */

void CAdminDlg::HackServicesIni(char* szServerName,
                                char* szServices,
                                char* szServiceName,
                                CString szMapiServicesIni)
{
    int n = 0;
    char szKeyBuffer[256];
    char* szKey = NULL;

    struct ServicesIniRecord    // To create the profile.
    {
        char*   Section;
        char*   Key;
        DWORD   prop;
        char*   Value;
    };

    char* ServicesSectionServices           = szServices;
    char* ServicesSectionABView_Admin       = szServiceName;
    char* ServicesValueABView_Admin         = "MAPI ABview Admin Services";
    char* ServicesValueServiceDLL           = "emsui.dll";
    char* ServicesValueEntryName            = "EMSCfg";
    char* ServicesKeyProviders              = "Providers";
    char* ServicesSectionABView_DSA_Admin   = "ABVU_DSA_Admin";
    char* ServicesValueProviders            = ServicesSectionABView_DSA_Admin;
    char* ServicesValueDSAName              = ServicesSectionABView_DSA_Admin;
    char* ServicesValueDSADLL               = "emsabp.dll";
    char* ServicesValueDSAType              = "23000000";
    char* ServicesValueServerName           = szServerName;

    ServicesIniRecord   rgsir[] =
    {
        { ServicesSectionServices,              //General Services Section
            ServicesSectionABView_Admin,
                NULL,
                    ServicesValueABView_Admin },


        { ServicesSectionABView_Admin,          //ABview Profile Section
            NULL,
                PR_SERVICE_DLL_NAME,
                    ServicesValueServiceDLL },
        { ServicesSectionABView_Admin,
            NULL,
                PR_SERVICE_ENTRY_NAME,
                    ServicesValueEntryName },
        { ServicesSectionABView_Admin,
            ServicesKeyProviders,
                NULL,
                    ServicesValueProviders },


        { ServicesSectionABView_DSA_Admin,      //Directory Specific Properties
            NULL,
                PR_PROVIDER_DISPLAY,
                    ServicesValueDSAName },
        { ServicesSectionABView_DSA_Admin,
            NULL,
                PR_PROVIDER_DLL_NAME,
                    ServicesValueDSADLL },
        { ServicesSectionABView_DSA_Admin,
            NULL,
                PR_RESOURCE_TYPE,
                    ServicesValueDSAType },
        { ServicesSectionABView_DSA_Admin,
            NULL,
                PR_AB_SERVER,
                    ServicesValueServerName },
    };

    for (n = 0; n < sizeof(rgsir) / sizeof(ServicesIniRecord); n++)
    {
        if(rgsir[n].Key)
        {
            szKey = rgsir[n].Key;
        }
        else
        {
            wsprintf( szKeyBuffer, "%8lx", rgsir[n].prop ); 
            szKey = szKeyBuffer; 
    }

        WritePrivateProfileString(  rgsir[n].Section,
                                    szKey,
                                    rgsir[n].Value,
                                    szMapiServicesIni);

    }
}

/*
 -  CMainWindow::
 -  OnLogon
 *
 *  Purpose:
 *      Logs the user onto the Mail subsystem.  If the logon was
 *      successful, then the Address Book is opened as well.  Menu
 *      enabling is performed depending on the outcome of the calls.
 *
 */

void CMainWindow::OnLogon()
{
    CGetError E;
    CMenu* menu = GetMenu();
    SCODE sc = S_OK;
    HRESULT hResult = NULL;
    ULONG ulUIParam = (ULONG)(void *)m_hWnd;
    ULONG           ulFlags         = 0;

    ulFlags = ulFlags | MAPI_EXTENDED | MAPI_LOGON_UI | MAPI_NO_MAIL;

    if(fUNICODE)
    {
#ifdef NT    
        if( FAILED(sc = MAPILogonW( ulUIParam,
                                    NULL,
                                    NULL,
                                    ulFlags,
                                    0,
                                    (LPLHANDLE) &lpSession )))
        {
            MessageBox( E.SzError("MAPILogon", sc), "Client", MB_OK );
            goto error;
        }
#else
        AfxMessageBox( "UNICODE not supported on this OS.", MB_ICONEXCLAMATION | MB_OK, 0 );
#endif        
    }
    else
    {
        if( FAILED(sc = MAPILogon( ulUIParam,
                                    NULL,
                                    NULL,
                                    ulFlags,
                                    0,
                                    (LPLHANDLE) &lpSession )))
        {
            MessageBox( E.SzError("MAPILogon", sc), "Client", MB_OK );
            goto error;
        }
    }

    menu->EnableMenuItem( IDM_LOGON,    MF_GRAYED | MF_DISABLED );
    menu->EnableMenuItem( IDM_LOGOFF,   MF_ENABLED );

    if( hResult = lpSession->OpenAddressBook( (ULONG)(void *)m_hWnd,
                                NULL,
                                0,
                                &lpAdrBook ))
    {
        DisplayError( lpSession, hResult, "OpenAddressBook" );
        if( FAILED(hResult))
            goto error;
    }

    menu->EnableMenuItem( IDM_ROOTCONTAINER,    MF_ENABLED );
    menu->EnableMenuItem( IDM_ADDRESS0,         MF_ENABLED );
    menu->EnableMenuItem( IDM_ADDRESS1,         MF_ENABLED );
    menu->EnableMenuItem( IDM_ADDRESS2,         MF_ENABLED );
    menu->EnableMenuItem( IDM_ADDRESS3,         MF_ENABLED );
    menu->EnableMenuItem( IDM_RESOLVENAME,      MF_ENABLED );
    menu->EnableMenuItem( IDM_CREATEONEOFF,     MF_ENABLED );
    menu->EnableMenuItem( IDM_NEWENTRY,         MF_ENABLED );
    menu->EnableMenuItem( IDM_PREPARERECIPS,    MF_ENABLED );

    return;

error:
    OnLogoff();
    menu->EnableMenuItem( IDM_LOGON,            MF_ENABLED );
    menu->EnableMenuItem( IDM_LOGOFF,           MF_GRAYED );
}

/*
 -    CMainWindow::
 -    OnLogoff
 *
 *    Purpose:
 *      Logs off the Mail subsystem.  De-enables the menu if success.
 *
 */

void CMainWindow::OnLogoff()
{
    CMenu* menu = GetMenu();
    HRESULT hResult = NULL;

    if (ulModlessABHwnd)
        ::DestroyWindow((HWND)ulModlessABHwnd);

    if(lpAdrBook)
    {
        lpAdrBook->Release();
        lpAdrBook = NULL;

        menu->EnableMenuItem( IDM_ROOTCONTAINER,    MF_GRAYED );
        menu->EnableMenuItem( IDM_ADDRESS0,         MF_GRAYED );
        menu->EnableMenuItem( IDM_ADDRESS1,         MF_GRAYED );
        menu->EnableMenuItem( IDM_ADDRESS2,         MF_GRAYED );
        menu->EnableMenuItem( IDM_ADDRESS3,         MF_GRAYED );
        menu->EnableMenuItem( IDM_RESOLVENAME,      MF_GRAYED );
        menu->EnableMenuItem( IDM_CREATEONEOFF,     MF_GRAYED );
        menu->EnableMenuItem( IDM_NEWENTRY,         MF_GRAYED );
        menu->EnableMenuItem( IDM_PREPARERECIPS,    MF_GRAYED );
    }

    if( lpSession )
    {
        if( FAILED(hResult = lpSession->Logoff( (ULONG)(void *)m_hWnd,
                                MAPI_LOGOFF_UI,
                                0 )))
        {
            DisplayError( lpSession, hResult, "Logoff" );
            goto error;
        }

        lpSession->Release();
        lpSession = NULL;

        menu->EnableMenuItem( IDM_LOGON,            MF_ENABLED );
        menu->EnableMenuItem( IDM_LOGOFF,           MF_GRAYED );
    }

error:    {}

}


/*
 -  CMainWindow::
 -  OnRootContainer
 *
 *  Purpose:
 *      Opens up a dialog with a standard listbox displaying
 *      the hierarchy table of the Address Book.  Any AB providers
 *      that are registered and in the path should be loaded, and
 *      available for display.
 *
 */

void CMainWindow::OnRootContainer()
{
    SCODE sc = S_OK;

    sc = ScOpenEntry( 0, NULL, DT_NOT_SPECIFIC, fRoot, fCONVENIENT_DEPTH );
}


/*
 ** Dismissed
 *
 *  DESCRIPTION:
 *      Called when Browse mode of Address Book is dismissed.
 *      Resets the modless bit giving indication the dialog is gone.
 *
 */

void STDMETHODCALLTYPE
Dismissed (ULONG ulUIParam, LPVOID lpvContext)
{
    ulModlessABHwnd = 0;
}


/*
 -  CMainWindow::
 -  OnAddress0
 *
 *  Purpose:
 *      This opens the 0 well case for viewing the
 *      Address Book using the Mapi Dialogs supplied
 *      with the "Address" Call.
 *
 */

void CMainWindow::OnAddress0()
{
    ADRPARM     adrparm;
    ULONG       ulUIParam   = (ULONG)(void *)m_hWnd;
    HRESULT     hResult = NULL;
    ULONG       ulFlags = 0;

    if (ulModlessABHwnd)
        return;

    ulModlessABHwnd = ulUIParam;

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;
        
    ulFlags = ulFlags | DIALOG_SDI;     

    adrparm.cbABContEntryID     = 0;
    adrparm.lpABContEntryID     = NULL;
    adrparm.ulFlags             = ulFlags;
    adrparm.lpReserved          = NULL;
    adrparm.ulHelpContext       = 0;
    adrparm.lpszHelpFileName    = NULL;
    adrparm.lpfnABSDI           = NULL;
    adrparm.lpfnDismiss         = Dismissed;
    adrparm.lpvDismissContext   = NULL;
    adrparm.lpszCaption         = "MAPI ITP tests Address 0";
    adrparm.lpszNewEntryTitle   = "For this test dialog";
    adrparm.lpszDestWellsTitle  = "0 wells here";
    adrparm.cDestFields         = 0;
    adrparm.nDestFieldFocus     = 0;
    adrparm.lppszDestTitles     = NULL;
    adrparm.lpulDestComps       = NULL;
    adrparm.lpContRestriction   = NULL;
    adrparm.lpHierRestriction   = NULL;

    if( FAILED(hResult = lpAdrBook->Address(&ulModlessABHwnd,
                                    &adrparm,
                                    NULL )))
    {
        DisplayError( lpAdrBook, hResult, "Address" );
    }
}


/*
 -  CMainWindow::
 -  OnAddress1
 *
 *  Purpose:
 *      This calls "Address", requesting the single well
 *      case for display
 *
 */

void CMainWindow::OnAddress1()
{
    LPSTR    rglpszDestTitles[1];
    ULONG       rgulDestComps[1];
    ADRPARM     adrparm;
    LPADRLIST   lpadrlist   = NULL;
    ULONG       ulUIParam   = (ULONG)(void *)m_hWnd;
    HRESULT     hResult     = NULL;
    ULONG       ulFlags     = 0;

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;
        
    ulFlags = ulFlags | DIALOG_MODAL;     

    rglpszDestTitles[0]         = "To";
    rgulDestComps[0]            = MAPI_TO;

    adrparm.cbABContEntryID     = 0;
    adrparm.lpABContEntryID     = NULL;
    adrparm.ulFlags             = ulFlags;
    adrparm.lpReserved          = NULL;
    adrparm.ulHelpContext       = 0;
    adrparm.lpszHelpFileName    = NULL;
    adrparm.lpfnABSDI           = NULL;
    adrparm.lpfnDismiss         = NULL;
    adrparm.lpvDismissContext   = NULL;
    adrparm.lpszCaption         = "MAPI ITP tests Address 1";
    adrparm.lpszNewEntryTitle   = "For this test dialog";
    adrparm.lpszDestWellsTitle  = "1 well here";
    adrparm.cDestFields         = 1;
    adrparm.nDestFieldFocus     = 0;
    adrparm.lppszDestTitles     = rglpszDestTitles;
    adrparm.lpulDestComps       = rgulDestComps;
    adrparm.lpContRestriction   = NULL;
    adrparm.lpHierRestriction   = NULL;

    if( FAILED(hResult = lpAdrBook->Address( &ulUIParam,
                            &adrparm,
                            &lpadrlist )))
    {
        DisplayError( lpAdrBook, hResult, "Address" );
        _FreeRowSet((LPSRowSet) lpadrlist);
        return;
    }

    CAdrListDlg dlgAdrList(lpadrlist);
    _FreeRowSet((LPSRowSet)lpadrlist);
}


/*
 -  CMainWindow::
 -  OnAddress2
 *
 *  Purpose:
 *      This calls "Address", requesting the 2 well
 *      case for display
 *
 */

void CMainWindow::OnAddress2()
{
    LPSTR    rglpszDestTitles[2];
    ULONG       rgulDestComps[2];
    ADRPARM     adrparm;
    LPADRLIST   lpadrlist   = NULL;
    ULONG       ulUIParam   = (ULONG)(void *)m_hWnd;
    HRESULT     hResult     = NULL;
    ULONG       ulFlags     = 0;

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;
        
    ulFlags = ulFlags | DIALOG_MODAL;     

    rglpszDestTitles[0] = "To";
    rglpszDestTitles[1] = "Cc";

    rgulDestComps[0]    = MAPI_TO;
    rgulDestComps[1]    = MAPI_CC;

    adrparm.cbABContEntryID     = 0;
    adrparm.lpABContEntryID     = NULL;
    adrparm.ulFlags             = ulFlags;
    adrparm.lpReserved          = NULL;
    adrparm.ulHelpContext       = 0;
    adrparm.lpszHelpFileName    = NULL;
    adrparm.lpfnABSDI           = NULL;
    adrparm.lpfnDismiss         = NULL;
    adrparm.lpvDismissContext   = NULL;
    adrparm.lpszCaption         = "MAPI ITP tests Address 2";
    adrparm.lpszNewEntryTitle   = "For this test dialog";
    adrparm.lpszDestWellsTitle  = "2 wells here";
    adrparm.cDestFields         = 2;
    adrparm.nDestFieldFocus     = 0;
    adrparm.lppszDestTitles     = rglpszDestTitles;
    adrparm.lpulDestComps       = rgulDestComps;
    adrparm.lpContRestriction   = NULL;
    adrparm.lpHierRestriction   = NULL;

    if( FAILED(hResult = lpAdrBook->Address( &ulUIParam,
                                                &adrparm,
                                                &lpadrlist )))
    {
        DisplayError( lpAdrBook, hResult, "Address" );
        _FreeRowSet((LPSRowSet) lpadrlist);
        return;
    }

    CAdrListDlg dlgAdrList(lpadrlist);
    _FreeRowSet((LPSRowSet)lpadrlist);
}


/*
 -  CMainWindow::
 -  OnAddress3
 *
 *  Purpose:
 *      This calls "Address", requesting the 3 well
 *      case for display
 *
 */

void CMainWindow::OnAddress3()
{
    LPSTR    rglpszDestTitles[3];
    ULONG       rgulDestComps[3];
    ADRPARM     adrparm;
    LPADRLIST   lpadrlist   = NULL;
    ULONG       ulUIParam   = (ULONG)(void *)m_hWnd;
    HRESULT     hResult     = NULL;
    ULONG       ulFlags     = 0;

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;
        
    ulFlags = ulFlags | DIALOG_MODAL;     

    rglpszDestTitles[0] = "To";
    rglpszDestTitles[1] = "Cc";
    rglpszDestTitles[2] = "Bcc";

    rgulDestComps[0]    = MAPI_TO;
    rgulDestComps[1]    = MAPI_CC;
    rgulDestComps[2]    = MAPI_BCC;

    adrparm.cbABContEntryID     = 0;
    adrparm.lpABContEntryID     = NULL;
    adrparm.ulFlags             = ulFlags;
    adrparm.lpReserved          = NULL;
    adrparm.ulHelpContext       = 0;
    adrparm.lpszHelpFileName    = NULL;
    adrparm.lpfnABSDI           = NULL;
    adrparm.lpfnDismiss         = NULL;
    adrparm.lpvDismissContext   = NULL;
    adrparm.lpszCaption         = "MAPI ITP tests Address 3";
    adrparm.lpszNewEntryTitle   = "For this test dialog";
    adrparm.lpszDestWellsTitle  = "3 wells here";
    adrparm.cDestFields         = 3;
    adrparm.nDestFieldFocus     = 0;
    adrparm.lppszDestTitles     = rglpszDestTitles;
    adrparm.lpulDestComps       = rgulDestComps;
    adrparm.lpContRestriction   = NULL;
    adrparm.lpHierRestriction   = NULL;

    if( FAILED(hResult = lpAdrBook->Address( &ulUIParam,
                                                &adrparm,
                                                &lpadrlist )))
    {
        DisplayError( lpAdrBook, hResult, "Address" );
        _FreeRowSet((LPSRowSet) lpadrlist);
        return;
    }

    CAdrListDlg dlgAdrList(lpadrlist);
    _FreeRowSet((LPSRowSet)lpadrlist);
}



/*
 -  CMainWindow::
 -  OnResolveName
 *
 *  Purpose:
 *      This displays a dialog requesting a szPartialName that can be used
 *      to call "ResolveName".  The Address List returned will be displayed
 *      in a standard listbox format.
 *
 */

void CMainWindow::OnResolveName()
{
    CResolveNameDlg dlgResolveName;
}


/*
 -  CMainWindow::
 -  OnCreateOneOff
 *
 *  Purpose:
 *      This displays a dialog requesting a 3 strings that can be used
 *      to call "CreateOneOff".  The OneOff returned will be displayed
 *      as an entry, then freed.
 *
 */

void CMainWindow::OnCreateOneOff()
{
    COneOffDlg dlgOneOff;
}


/*
 -  CMainWindow::
 -  OnNewEntry
 *
 *  Purpose:
 *      Calls Session object's NewEntry method.
 *
 */

void CMainWindow::OnNewEntry()
{
    ULONG     ulUIParam = (ULONG)(void *)m_hWnd;
    ULONG     ulFlags = 0;
    ULONG     cbEIDContainer = 0;
    LPENTRYID lpEIDContainer = NULL;
    ULONG     cbEIDNewEntryTpl = 0;
    LPENTRYID lpEIDNewEntryTpl = NULL;
    ULONG     cbEIDNewEntry = 0;
    LPENTRYID EIDNewEntry = NULL;

    lpAdrBook->NewEntry( ulUIParam,
                         ulFlags,
                         cbEIDContainer,
                         lpEIDContainer,
                         cbEIDNewEntryTpl,
                         lpEIDNewEntryTpl,
                         &cbEIDNewEntry,
                         &EIDNewEntry );
    
    if(EIDNewEntry)
    {
        ScOpenEntry( cbEIDNewEntry, EIDNewEntry, DT_NOT_SPECIFIC, fContents, FALSE);
        MAPIFreeBuffer( EIDNewEntry );
    }
}


/*
 *  CMAPIPropDlg Methods.
 */

/*
 -  CMAPIPropDlg::
 -  OnGetPropsThis
 -
 *  Purpose:
 *      Will open a ListBox viewer with a List of Properties
 *      for "this" Container.  Properties are found by calling
 *      GetProps on the Container object.
 *
 */

void CMAPIPropDlg::OnGetPropsThis()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    LPSPropValue    lpsPropValue    = NULL;
    LPSPropTagArray lpPropTagArray  = NULL;
    ULONG           i;
    ULONG           ulFlags         = 0;
    
    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;

    if ( fUseUNICODE )
    {
        if( FAILED(hResult = m_lpEntry->GetPropList( ulFlags, &lpPropTagArray )))
        {
            DisplayError( m_lpEntry, hResult, "GetPropList" );
            return;
        }

            for ( i = 0; i < lpPropTagArray->cValues; i++ )
            {
                if ( PROP_TYPE( lpPropTagArray->aulPropTag[i] ) == PT_STRING8 )
                {
                    lpPropTagArray->aulPropTag[i] =
                    ( lpPropTagArray->aulPropTag[i] & 0xFFFF0000 ) |
                    PT_UNICODE;
                }
        }
    }

    if( FAILED(hResult = m_lpEntry->GetProps(lpPropTagArray, ulFlags,
                                                &cValues,
                                                &lpsPropValue )))
    {
        DisplayError( m_lpEntry, hResult, "GetProps" );
        MAPIFreeBuffer( lpPropTagArray );
        return;
    }

    if ( cValues == 0 || lpsPropValue == NULL )
    {
        MessageBox( "Couldn't get any properties on object.", "Error!", MB_OK );
        return;
    }

    CPropertiesDlg * pdlgProperties = new CPropertiesDlg( cValues,
                                                lpsPropValue,
                                                "GetProps",
                                                fmAProperty );

    MAPIFreeBuffer( lpsPropValue );
    MAPIFreeBuffer( lpPropTagArray );
    }


/*
 -  CMAPIPropDlg::
 -  OnGetPropListThis
 -
 *  Purpose:
 *      Will open a ListBox viewer with a List of Properties
 *      for "this" Container.  Properties are found by calling
 *      GetPropList on the Container object.
 *
 */

void CMAPIPropDlg::OnGetPropListThis()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    LPSPropTagArray lpPropTagArray  = NULL;
    LPSPropValue    lpProps         = NULL;
    ULONG           ulFlags         = 0;

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;

    if( FAILED(hResult = m_lpEntry->GetPropList( ulFlags, &lpPropTagArray )))
    {
        DisplayError( m_lpEntry, hResult, "GetPropList" );
        return;
    }

    if ( lpPropTagArray->cValues == 0 )
    {
        MessageBox( "Couldn't get a property list on object.", "Error!", MB_OK );
        return;
    }

    MAPIAllocateBuffer( sizeof(SPropValue) * lpPropTagArray->cValues, (LPVOID *) &lpProps );
    memset( (void *) lpProps, 0, (size_t)(sizeof(SPropValue) * lpPropTagArray->cValues) );

    for(cValues = 0; cValues < lpPropTagArray->cValues; cValues++)
    {
        lpProps[cValues].ulPropTag = lpPropTagArray->aulPropTag[cValues];
    }

    CPropertiesDlg * pdlgProperties = new CPropertiesDlg( lpPropTagArray->cValues,
                                                            lpProps,
                                                            "GetPropList",
                                                            fmProperty | fmPropType );

    MAPIFreeBuffer( lpProps );
    MAPIFreeBuffer( lpPropTagArray );
    }

/*
 -  CMAPIPropDlg::
 -  OnPropertyDLL
 -
 *  Purpose:
 *      Opens a dialog to manipulate the current object.
 *      The dialog is contained in a test DLL and can be
 *      called on any Table.
 *
 */

void CMAPIPropDlg::OnPropertyDLL()
{
    PROPVUViewMapiProp( "LPMAPIPROP",
                        (LPMAPIPROP FAR *)&m_lpEntry,
                        lpTargetEntryID ? lpTargetEntryID : (LPVOID) lpAdrBook,
                        (HWND)this->m_hWnd );
}

/*
 -  CMAPIPropDlg::
 -  OnAddToPAB
 -
 *  Purpose:
 *      Adds the Entry to the PAB using the CreateEntry method.
 *
 */

void CMAPIPropDlg::OnAddToPAB()
{
    HRESULT     hResult   = NULL;
    ULONG       cbEntryID = 0;
    LPENTRYID   lpEntryID = NULL;
    ULONG       ulObjType = 0;
    LPMAPIPROP  lpEntry   = NULL;
    LPMAPIPROP  lpMAPIPropEntry = NULL;
    ULONG       ulFlags   = 0;
    ULONG       ulTarget  = 0;
    ULONG           cValues         = 0;
    LPSPropValue    lpsPropValue    = NULL;
    ULONG           ulData[2];
    LPSPropTagArray lpPTArray   = (LPSPropTagArray) &ulData;

    //Make sure we have an entry we want to add
    if(!m_lpEntry)
        return;

    if ( lpTargetEntryID )
    {
        ulTarget = MessageBox(
                           "Add to the target instead of the PAB?",
                               "Question", MB_YESNO | MB_ICONQUESTION );
        switch ( ulTarget )
        {
            case IDYES: // A yes responce to question
                cbEntryID = cbTargetEntryID;
                lpEntryID = lpTargetEntryID;
                break;

            case IDNO: // A no responce to question
                //Find the PAB
                if( FAILED(hResult = lpAdrBook->GetPAB( &cbEntryID,
                                                        &lpEntryID )))
                {
                    DisplayError( lpAdrBook, hResult, "GetPAB" );
                    goto err;
                }
                break;

            default:
                return;
        }
    }
    else
    {
        //Find the PAB
        if( FAILED(hResult = lpAdrBook->GetPAB( &cbEntryID,
                                                &lpEntryID )))
        {
            DisplayError( lpAdrBook, hResult, "GetPAB" );
            goto err;
        }
    }

    if(fOpenEntry)
        ulFlags = MAPI_MODIFY;

    if( FAILED(hResult = lpAdrBook->OpenEntry( cbEntryID,
                                               lpEntryID,
                                               NULL,
                                               ulFlags,
                                               &ulObjType,
                                               (LPUNKNOWN*)&lpEntry )))

    {
        DisplayError( lpAdrBook, hResult, "OpenEntry" );
        goto err;
    }

    //Get the EntryID of the object we want to add
    //Create a PropTagArray with properties to request
    lpPTArray->cValues = 1;
    lpPTArray->aulPropTag[0] = PR_ENTRYID;

    if( FAILED(hResult = m_lpEntry->GetProps( lpPTArray, 0,
                                              &cValues,
                                              &lpsPropValue )))
    {
        DisplayError( m_lpEntry, hResult, "GetProps" );
        goto err;
    }

    if( FAILED( hResult =
           ((LPABCONT)lpEntry)->CreateEntry(
                                lpsPropValue[0].Value.bin.cb,
                                (LPENTRYID)lpsPropValue[0].Value.bin.lpb,
                                CREATE_CHECK_DUP_STRICT,
                                &lpMAPIPropEntry )))
                                {
                                    DisplayError( lpEntry, hResult, "CreateEntry" );
                                    goto err;
                                }

    if( FAILED( hResult = lpMAPIPropEntry->SaveChanges( 0 )))
    {
        DisplayError( lpEntry, hResult, "SaveChanges" );
        goto err;
    }

err:
    MAPIFreeBuffer( lpEntryID );

    if( lpEntry )
        lpEntry->Release();

    MAPIFreeBuffer( lpsPropValue );

    if( lpMAPIPropEntry )
        lpMAPIPropEntry->Release();
}

/*
 -  CMAPIPropDlg::
 -  OnDetails
 -
 *  Purpose:
 *      Calls the AB specific call "Details" on the current object.
 *      A details dialog box will be displayed.  If the entry is in
 *      a writable address-book container, the user may be allowed
 *      to modify the entry.  This is provider specific.
 *
 */

void CMAPIPropDlg::OnDetails()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    LPSPropValue    lpsPropValue    = NULL;

    ULONG           cProps          = 0;
    ULONG           ulUIParam       = (ULONG)(void *)m_hWnd;

    if(!m_lpEntry)
        return;

    if( FAILED(hResult = m_lpEntry->GetProps(NULL, 0,
                                    &cValues,
                                    &lpsPropValue )))
    {
        DisplayError( m_lpEntry, hResult, "GetProps" );
        return;
    }

    if ( cValues == 0 || lpsPropValue == NULL )
    {
        MessageBox( "Couldn't get any properties on object.", "Error!", MB_OK );
        return;
    }

    for( cProps=0; cProps < cValues; cProps++)
        if( lpsPropValue[cProps].ulPropTag == PR_ENTRYID )
        break;

    if( FAILED(hResult = lpAdrBook->Details(
                                    &ulUIParam,
                                    NULL,
                                    NULL,
                                    lpsPropValue[cProps].Value.bin.cb,
                                    (LPENTRYID)lpsPropValue[cProps].Value.bin.lpb,
                                    NULL,
                                    NULL,
                                    NULL,
                                    DIALOG_MODAL )))
    {
        DisplayError( lpAdrBook, hResult, "Details" );
    }

    MAPIFreeBuffer( lpsPropValue );
}



/*
 -  CMAPIPropDlg::
 -  OnGetIDsFromNames
 -
 *  Purpose:
 *      Calls GetIDsFromNames() requesting all IDs.
 *
 */

void CMAPIPropDlg::OnGetIDsFromNames()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    LPSPropTagArray lpPropTagArray  = NULL;
    LPSPropValue    lpPropValue     = NULL;
    
    if( FAILED(hResult = m_lpEntry->GetIDsFromNames( 0, NULL, 0, &lpPropTagArray )))
    {
        DisplayError( m_lpEntry, hResult, "GetIDsFromNames" );
        return;
    }

    if ( lpPropTagArray->cValues == 0 )
    {
       MessageBox( "lpPropTagArray->cValues == 0", "Error!", MB_OK );
       MAPIFreeBuffer( lpPropTagArray );
       return;
    }

    MAPIAllocateBuffer( sizeof(SPropValue) * lpPropTagArray->cValues, (LPVOID *) &lpPropValue );
    memset( (void *) lpPropValue, 0, (size_t)(sizeof(SPropValue) * lpPropTagArray->cValues) );

    for(cValues = 0; cValues < lpPropTagArray->cValues; cValues++)
    {
        lpPropValue[cValues].ulPropTag = lpPropTagArray->aulPropTag[cValues];
    }

    CPropertiesDlg * pdlgProperties = new CPropertiesDlg( lpPropTagArray->cValues,
                        lpPropValue,
                        "GetIDsFromNames",
                        fmProperty | fmPropType );

    MAPIFreeBuffer( lpPropValue );
    MAPIFreeBuffer( lpPropTagArray );
}



/*
 -  CMAPIPropDlg::
 -  OnGetNamesFromIDs
 -
 *  Purpose:
 *      Calls GetNamesFromIDs() requesting all Names.
 *
 */

void CMAPIPropDlg::OnGetNamesFromIDs()
{
    HRESULT             hResult         = NULL;
    ULONG               cPropNames      = 0;
    LPMAPINAMEID  FAR * lppPropNames    = NULL;
    LPGUID              lpPropSetGuid   = NULL;
    LPSPropTagArray     lpPropTags      = NULL;

    /* 
     * CreateMAPIGuid( lpPropSetGuid );
     -      Set the above to request PS_MAPI
     */

    if( FAILED(hResult = m_lpEntry->GetNamesFromIDs( &lpPropTags,
                                                     lpPropSetGuid,
                                                     0,
                                                     &cPropNames,
                                                     &lppPropNames )))
    {
        DisplayError( m_lpEntry, hResult, "GetNamesFromIDs" );
        return;
    }

    if ( cPropNames == 0 )
    {
       MessageBox( "cPropNames == 0", "Error!", MB_OK );
       MAPIFreeBuffer( lppPropNames );
       return;
    }

    CNamedIDsDlg * pdlgNamedIDs = new CNamedIDsDlg( cPropNames,
                                                    lppPropNames,
                                                    "GetNamesFromIDs" );
    MAPIFreeBuffer( lpPropTags );
    MAPIFreeBuffer( lppPropNames );

}



/*
 -  CMAPIPropDlg::
 -  OnDetailsTbl
 -
 *  Purpose:
 *      Request the PR_DETAILS_TABLE from the object being viewed.
 *      This will return a table with display type information.
 *
 */

void CMAPIPropDlg::OnDetailsTbl()
{
    HRESULT         hResult         = NULL;
    LPMAPITABLE     lpTable         = NULL;

    if(FAILED(hResult = m_lpEntry->OpenProperty( PR_DETAILS_TABLE,
                                                 (LPIID) &IID_IMAPITable,
                                                 0,
                                                 0,
                                                 (LPUNKNOWN *)&lpTable)))
    {
        DisplayError( m_lpEntry, hResult, "OpenProperty" );
        return;
    }
    
    if(hlibTBLVU)
        TBLVUViewMapiTable( (LPMAPITABLE FAR *)&lpTable, m_hWnd );
    else
        AfxMessageBox( "Table Viewer not available", MB_ICONEXCLAMATION | MB_OK, 0 );

    if(lpTable)
        lpTable->Release();
}


/*
 -  CMAPIPropDlg::
 -  ~CMAPIPropDlg
 -
 *  Purpose:
 *      Destructor for class CMAPIPropDlg.  Will release
 *      the object being viewed.
 *
 */

CMAPIPropDlg::~CMAPIPropDlg()
{
    if(m_lpEntry)
    {
        m_lpEntry->Release();
        m_lpEntry = NULL;
    }
}



/*
 *  CMAPIContainerDlg Methods.
 */

/*
 -  CMAPIContainerDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Loads the ListBox with rows from the MAPITable prior to
 *      displaying the dialog.
 *
 */

BOOL CMAPIContainerDlg::OnInitDialog()
{
    HRESULT         hResult           = NULL;
    CWnd *          pBTable           = NULL;
    CWnd *          pBDT              = NULL;
    CWnd *          pBOOT             = NULL;
    CWnd *          pBProp            = NULL;
    CWnd *          pBPerf            = NULL;
    CWnd *          pBTestThis        = NULL;
    CWnd *          pBTestSel         = NULL;
    CWnd *          pBSetAsTargetThis = NULL;
    CWnd *          pBSetAsTargetSel  = NULL;
    int             nQueryUser        = 0;
    CString         szBuffer;
    BOOL            fHierarchyTable   = FALSE;
    ULONG           ulFlags           = 0;

    if(!m_lpEntry)
        return FALSE;

    //Disable Tablevu button if DLL is not present.
    if(!hlibTBLVU)
    {
        pBTable = GetDlgItem(IDC_TABLEDLL);
        pBTable->EnableWindow( FALSE );
    }

    //Disable Display Table button if DLL is not present.
    if(!hlibTBLVU)
    {
        pBDT = GetDlgItem(IDC_PR_DETAILS_TABLE);
        pBDT->EnableWindow( FALSE );
    }

    //Disable One-Off Table button if DLL is not present.
    if(!hlibTBLVU)
    {
        pBOOT = GetDlgItem(IDC_PR_CREATE_TEMPLATES);
        pBOOT->EnableWindow( FALSE );
    }

    //Disable Propvu button if DLL is not present.
    if(!hlibPROPVU)
    {
        pBProp = GetDlgItem(IDC_PROPERTYDLL);
        pBProp->EnableWindow( FALSE );
    }

    //Disable Performance button if DLL is not present.
    if(!hlibPERF)
    {
        pBPerf = GetDlgItem(IDC_PERFORMANCE);
        pBPerf->EnableWindow( FALSE );
    }

    //Disable Test button if DLL is not present.
    if(!hlibTEST)
    {
        pBTestThis = GetDlgItem(IDC_TESTTHISDLL);
        pBTestThis->EnableWindow( FALSE );
        pBTestSel = GetDlgItem(IDC_TESTSELDLL);
        pBTestSel->EnableWindow( FALSE );
    }

//Disable SetAsTarget button if option is disabled.
    if(!fSetAsTarget)
    {
        pBSetAsTargetThis = GetDlgItem(IDC_SETASTARGETTHIS);
        pBSetAsTargetThis->EnableWindow( FALSE );
        pBSetAsTargetSel = GetDlgItem(IDC_SETASTARGETSEL);
        pBSetAsTargetSel->EnableWindow( FALSE );
    }

//Check to see if this is flaged as the RootContainer.  If it is then
//get a HierarchyTable, else get a ContentsTable.

    if(fQueryForTable)
    {
        szBuffer += "Do you want a Hierarchy Table?\r\n\r\n";
        szBuffer += "YES\tHierarchy Table is requested.\r\n";
        szBuffer += "NO\tContents Table is requested.\r\n\r\n";
        szBuffer += "Use Config to turn off/on this request dialog.";
        
        nQueryUser = AfxMessageBox( szBuffer, MB_ICONQUESTION | MB_YESNO, 0 );

        switch(nQueryUser)
        {
            case IDYES:
                fHierarchyTable = TRUE;
                break;
            case IDNO:
                fHierarchyTable = FALSE;
                break;
            default:
                break;
        }
    }
    else
    {
        if(m_ulTableTypeFrom == fRoot)
            fHierarchyTable = TRUE;    
    }

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;

    if(m_fCONVENIENT_DEPTH)
    {
        CheckDlgButton(IDC_CONVENIENTDEPTH, 1);
    }

    if(fHierarchyTable) 
    {
        if(m_fCONVENIENT_DEPTH)
        {
            ulFlags = ulFlags | CONVENIENT_DEPTH;
        }
    
        if( FAILED(hResult = ((LPABCONT)m_lpEntry)->GetHierarchyTable(
                                                    ulFlags,
                                                    &m_lpMAPITable )))
        {
            DisplayError( m_lpEntry, hResult, "GetHierarchyTable" );
        }
        else
        {
            m_ulCurrentTableType = fHierarchy;
        }
    }
    else
    {
        if( FAILED(hResult = ((LPABCONT)m_lpEntry)->GetContentsTable(
                                                    ulFlags,
                                                    &m_lpMAPITable )))
        {
            DisplayError( m_lpEntry, hResult, "GetContentsTable" );
        }
        else
        {
            m_ulCurrentTableType = fContents;
        }
    }

    m_lpTblList = new CTblList( IDC_CONTAINERLIST,
                                m_lpMAPITable,
                                m_ulCurrentTableType );

    VERIFY(m_lpTblList->SubclassDlgItem(IDC_CONTAINERLIST, this));

    m_lpTblList->InitListBox();


    //Change caption for appropriate object type
    switch ( m_ulObjType )
    {
        case MAPI_ABCONT:
            m_Caption += "Container - ";
            break;
        case MAPI_DISTLIST:
            m_Caption += "Distlist - ";
            break;
        default:
            m_Caption += "Unknown Obj Type - ";    
    }


    switch ( m_ulCurrentTableType )
    {
        case fRoot:
            m_Caption += "Root";
            break;
        case fHierarchy:
            m_Caption += "Hierarchy";
            break;
        case fContents:
            m_Caption += "Contents";
            break;
        default:
            m_Caption += "Unknown Table";    
    }

    SetWindowText(m_Caption);

    return TRUE;
}


/*
 -  CMAPIContainerDlg::
 -  OnOK
 -
 *  Purpose:
 *      Finds the Entry Selected and performs an OpenEntry on it.  If
 *      the object opened is a Container, then a Table viewer is
 *      created with the information.
 *
 */

void CMAPIContainerDlg::OnOK()
{
    SCODE        sc      = S_OK;
    LPSPropValue lpProps = NULL;
    BOOL         fDEPTH  = FALSE;

    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    fDEPTH = IsDlgButtonChecked( IDC_CONVENIENTDEPTH );
    
    lpProps = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    sc = ScOpenEntry( lpProps[1].Value.bin.cb,
    (LPENTRYID)lpProps[1].Value.bin.lpb,
    lpProps[3].Value.ul,
    m_ulCurrentTableType,
    fDEPTH );

}


/*
 -  CMAPIContainerDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the Table viewer dialog.
 *
 */

void CMAPIContainerDlg::OnCancel()
{
    while(m_lpTblList->DeleteString( 0 )) {}
    delete m_lpTblList;

    EndDialog(IDCANCEL);
}


/*
 -  CMAPIContainerDlg::
 -  OnProperties
 -
 *  Purpose:
 *      Will open a ListBox viewer with a List of Properties
 *      from the Table for the currently selected entry in
 *      the Table viewer.
 *
 */

void CMAPIContainerDlg::OnProperties()
{
    LPSPropValue    lpProps   = NULL;
    LPSPropTagArray lpPTArray = NULL;

    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    lpProps   = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    lpPTArray = (LPSPropTagArray)ABH_PropTagArray;

    CPropertiesDlg * pdlgProperties = new CPropertiesDlg(
                                        lpPTArray->cValues,
                                        lpProps,
                                        "Table Properties, Current Selection",
                                        fmAProperty );
}


/*
 -  CMAPIContainerDlg::
 -  OnGetProps
 -
 *  Purpose:
 *      Will open a ListBox viewer with a List of Properties
 *      for the currently selected entry in the Table viewer.
 *      This entry will be opened into an object.  GetProps
 *      is then performed on the object.
 *
 */

void CMAPIContainerDlg::OnGetProps()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    ULONG           ulFlags         = 0;
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpEntry         = NULL;
    LPSPropValue    lpsPropValue    = NULL;
    LPSPropValue    lpProps         = NULL;
    LPSPropTagArray lpPropTagArray  = NULL;
    ULONG           i;

    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    lpProps   = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    if(fOpenEntry)
        ulFlags = MAPI_MODIFY;

    if( FAILED(hResult = lpAdrBook->OpenEntry( lpProps[1].Value.bin.cb,
                                               (LPENTRYID)lpProps[1].Value.bin.lpb,
                                               NULL,
                                               ulFlags,
                                               &ulObjType,
                                               (LPUNKNOWN*)&lpEntry )))
    {
        DisplayError( lpAdrBook, hResult, "OpenEntry" );
        return;
    }

    if(fUNICODE)
        ulFlags = MAPI_UNICODE;
    else
        ulFlags = 0;

    if ( fUseUNICODE )
    {
        if( FAILED(hResult = lpEntry->GetPropList( ulFlags, &lpPropTagArray )))
        {
            DisplayError( lpEntry, hResult, "GetPropList" );
            return;
        }
        for ( i = 0; i < lpPropTagArray->cValues; i++ )
        {
            if ( PROP_TYPE( lpPropTagArray->aulPropTag[i] ) == PT_STRING8 )
            {
                lpPropTagArray->aulPropTag[i] =
                     ( lpPropTagArray->aulPropTag[i] & 0xFFFF0000 ) |
                     PT_UNICODE;
            }
        }
    }

    if( FAILED(hResult = lpEntry->GetProps( lpPropTagArray,
                                            ulFlags,
                                            &cValues,
                                            &lpsPropValue )))
    {
        DisplayError( lpEntry, hResult, "GetProps" );
        lpEntry->Release();
        return;
    }

    if ( cValues == 0 || lpsPropValue == NULL )
    {
       MessageBox( "Couldn't get any properties on object.", "Error!", MB_OK );
       return;
    }

    CPropertiesDlg * pdlgProperties = new CPropertiesDlg( cValues,
                lpsPropValue,
                "GetProps",
                fmAProperty );

    MAPIFreeBuffer( lpsPropValue );
    lpEntry->Release();
}


/*
 -  CMAPIContainerDlg::
 -  OnGetPropList
 -
 *  Purpose:
 *      Will open a ListBox viewer with a List of Properties
 *      for the currently selected entry in the Table viewer.
 *      This entry will be opened into an object.  GetPropList
 *      is then performed on the object.
 *
 */

void CMAPIContainerDlg::OnGetPropList()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    ULONG           ulFlags         = 0;
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpEntry         = NULL;
    LPSPropValue    lpsPropValue    = NULL;
    LPSPropValue    lpProps         = NULL;
    LPSPropTagArray lpPropTagArray  = NULL;

    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    lpProps   = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    if(fOpenEntry)
        ulFlags = MAPI_MODIFY;

    if( FAILED(hResult = lpAdrBook->OpenEntry( lpProps[1].Value.bin.cb,
                                               (LPENTRYID)lpProps[1].Value.bin.lpb,
                                               NULL,
                                               ulFlags,
                                               &ulObjType,
                                               (LPUNKNOWN*)&lpEntry )))
    {
        DisplayError( lpAdrBook, hResult, "OpenEntry" );
        return;
    }

    if(fUNICODE)
        ulFlags = MAPI_UNICODE;
    else
        ulFlags = 0;

    if( FAILED(hResult = lpEntry->GetPropList( ulFlags, &lpPropTagArray )))
    {
        DisplayError( lpEntry, hResult, "GetPropList" );
        lpEntry->Release();
        return;
    }

    if ( lpPropTagArray->cValues == 0 )
    {
       MessageBox( "Couldn't get a property list on object.", "Error!", MB_OK );
       MAPIFreeBuffer( lpPropTagArray );
       lpEntry->Release();
       return;
    }

    MAPIAllocateBuffer( sizeof(SPropValue) * lpPropTagArray->cValues, (LPVOID *) &lpsPropValue );
    memset( (void *) lpsPropValue, 0, (size_t)(sizeof(SPropValue) * lpPropTagArray->cValues) );

    for(cValues = 0; cValues < lpPropTagArray->cValues; cValues++)
    {
        lpsPropValue[cValues].ulPropTag = lpPropTagArray->aulPropTag[cValues];
    }

    CPropertiesDlg * pdlgProperties = new CPropertiesDlg( lpPropTagArray->cValues,
                        lpsPropValue,
                        "GetPropList",
                        fmProperty | fmPropType );

    MAPIFreeBuffer( lpsPropValue );
    MAPIFreeBuffer( lpPropTagArray );

    lpEntry->Release();
}


/*
 -  CMAPIContainerDlg::
 -  OnTableDLL
 -
 *  Purpose:
 *      Opens a dialog to manipulate the current Table.
 *      The dialog is contained in a test DLL and can be
 *      called on any Table.
 *
 */

void CMAPIContainerDlg::OnTableDLL()
{
    HRESULT         hResult         = NULL;
    LPMAPITABLE     lpMAPITable     = NULL;
    ULONG           ulFlags         = 0;

    if (!hlibTBLVU)
        return;

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;

//Check to see the type of the current table and open another that is the same type.

    if(m_ulCurrentTableType == fHierarchy)
    {
        if(fCONVENIENT_DEPTH)
            ulFlags = ulFlags | CONVENIENT_DEPTH;
    
        if( FAILED(hResult = ((LPABCONT)m_lpEntry)->GetHierarchyTable( ulFlags,
                                    &lpMAPITable )))
        {
            DisplayError( m_lpEntry, hResult, "GetHierarchyTable" );
            return;
        }
    }
    else
    {
    if( FAILED(hResult = ((LPABCONT)m_lpEntry)->GetContentsTable( ulFlags,
                                    &lpMAPITable )))
    {
        DisplayError( m_lpEntry, hResult, "GetContentsTable" );
        return;
    }
}

//Create Table Test dialog
TBLVUViewMapiTable( (LPMAPITABLE FAR *)&lpMAPITable, (HWND)this->m_hWnd );

lpMAPITable->Release();
}


/*
 -  CMAPIContainerDlg::
 -  OnPerformance
 -
 *  Purpose:
 *      Opens a dialog to perform performance testing of
 *      Address Book Service providers.
 *
 */

void CMAPIContainerDlg::OnPerformance()
{
    HRESULT         hResult         = NULL;
    LPMAPITABLE     lpMAPITable     = NULL;
    ULONG           ulFlags         = 0;

    if (!hlibPERF)
        return;

    if(fUNICODE)
        ulFlags = ulFlags | MAPI_UNICODE;

    if(m_ulCurrentTableType == fHierarchy)
    {
        if(fCONVENIENT_DEPTH)
            ulFlags = ulFlags | CONVENIENT_DEPTH;
    
        if( FAILED(hResult = ((LPABCONT)m_lpEntry)->GetHierarchyTable( ulFlags,
                                                &lpMAPITable )))
        {
            DisplayError( m_lpEntry, hResult, "GetHierarchyTable" );
            return;
        }
    }
    else
    {
        if( FAILED(hResult = ((LPABCONT)m_lpEntry)->GetContentsTable( ulFlags,
                                                    &lpMAPITable )))
        {
            DisplayError( m_lpEntry, hResult, "GetContentsTable" );
            return;
        }
    }

    ABPERFormance(  (LPADRBOOK FAR *)&lpAdrBook,
    (LPMAPITABLE FAR *)&lpMAPITable,
    (HWND)this->m_hWnd );

    lpMAPITable->Release();
}

/*
 -  CMAPIContainerDlg::
 -  OnDetailsSel
 -
 *  Purpose:
 *      Calls Details with the EntryID of the Selection.
 *
 */

void CMAPIContainerDlg::OnDetailsSel()
{
    HRESULT       hResult     = NULL;
    LPSPropValue  lpProps     = NULL;
    ULONG         ulUIParam   = (ULONG)(void *)m_hWnd;

    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    lpProps = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    if( FAILED(hResult = lpAdrBook->Details(
                                &ulUIParam,
                                NULL,
                                NULL,
                                lpProps[1].Value.bin.cb,
                                (LPENTRYID)lpProps[1].Value.bin.lpb,
                                NULL,
                                NULL,
                                NULL,
                                DIALOG_MODAL )))
    {
        DisplayError( lpAdrBook, hResult, "Details" );
    }

}


/*
 -  CMAPIContainerDlg::
 -  OnTestSel
 -
 *  Purpose:
 *      Calls Test DLL with the EntryID and Type of the selection.
 *
 */

void CMAPIContainerDlg::OnTestSel()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    ULONG           ulFlags         = 0;
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpEntry         = NULL;
    LPSPropValue    lpsPropValue    = NULL;
    LPSPropValue    lpProps         = NULL;
    LPSPropTagArray lpPropTagArray  = NULL;

    if (!hlibTEST)
        return;

    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    lpProps = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    if(fOpenEntry)
        ulFlags = MAPI_MODIFY;

    if( FAILED(hResult = lpAdrBook->OpenEntry( lpProps[1].Value.bin.cb,
                                               (LPENTRYID)lpProps[1].Value.bin.lpb,
                                               NULL,
                                               ulFlags,
                                               &ulObjType,
                                               (LPUNKNOWN*)&lpEntry )))
    {
        DisplayError( lpAdrBook, hResult, "OpenEntry" );
        return;
    }

    ABTest( (LPADRBOOK FAR *)&lpAdrBook,
            ulObjType,
            &lpEntry,
            (HWND)this->m_hWnd );
}


/*
 -  CMAPIContainerDlg::
 -  OnTestThis
 -
 *  Purpose:
 *      Calls Test DLL with the Type of the container.
 *
 */

void CMAPIContainerDlg::OnTestThis()
{
    HRESULT         hResult         = NULL;
    LPMAPITABLE     lpMAPITable     = NULL;

    if (!hlibTEST)
        return;

    ABTest( (LPADRBOOK FAR *)&lpAdrBook,
            m_ulObjType,
            &m_lpEntry,
            (HWND)this->m_hWnd );
}


/*
 -  CMAPIContainerDlg::
 -  OnAddToPABSel
 -
 *  Purpose:
 *      Adds selected EntryID to the PAB or target container.
 *
 */

void CMAPIContainerDlg::OnAddToPABSel()
{
    HRESULT         hResult         = NULL;
    ULONG           cbEntryID       = 0;
    LPENTRYID       lpEntryID       = NULL;
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpEntry         = NULL;
    LPMAPIPROP      lpMAPIPropEntry = NULL;
    ULONG           ulFlags         = 0;
    ULONG           ulTarget        = 0;
    ULONG           cValues         = 0;
    LPSPropValue    lpProps         = NULL;
    ULONG           ulData[2];
    LPSPropTagArray lpPTArray       = (LPSPropTagArray) &ulData;

    //Get the EntryID of the object we want to add
    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    lpProps = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    if ( lpTargetEntryID )
    {
        ulTarget = MessageBox(
                           "Add to the target instead of the PAB?",
                               "Question", MB_YESNO | MB_ICONQUESTION );
        switch ( ulTarget )
        {
            case IDYES:
                cbEntryID = cbTargetEntryID;
                lpEntryID = lpTargetEntryID;
                break;

            case IDNO:
                //Find the PAB
                if( FAILED(hResult = lpAdrBook->GetPAB( &cbEntryID,
                                                        &lpEntryID )))
                {
                    DisplayError( lpAdrBook, hResult, "GetPAB" );
                    goto err;
                }
                break;

            default:
                return;
        }
    }
    else
    {
        //Find the PAB
        if( FAILED(hResult = lpAdrBook->GetPAB( &cbEntryID,
                                                &lpEntryID )))
        {
            DisplayError( lpAdrBook, hResult, "GetPAB" );
            goto err;
        }
    }

    if(fOpenEntry)
        ulFlags = MAPI_MODIFY;

    if( FAILED(hResult = lpAdrBook->OpenEntry( cbEntryID,
                                               lpEntryID,
                                               NULL,
                                               ulFlags,
                                               &ulObjType,
                                               (LPUNKNOWN*)&lpEntry )))

    {
        DisplayError( lpAdrBook, hResult, "OpenEntry" );
        goto err;
    }

    if( FAILED( hResult =
           ((LPABCONT)lpEntry)->CreateEntry(
                                lpProps[1].Value.bin.cb,
                                (LPENTRYID)lpProps[1].Value.bin.lpb,
                                CREATE_CHECK_DUP_STRICT,
                                &lpMAPIPropEntry )))
                                {
                                    DisplayError( lpEntry, hResult, "CreateEntry" );
                                    goto err;
                                }

    if( FAILED( hResult = lpMAPIPropEntry->SaveChanges( 0 )))
    {
        DisplayError( lpEntry, hResult, "SaveChanges" );
        goto err;
    }

err:
    MAPIFreeBuffer( lpEntryID );

    if( lpEntry )
        lpEntry->Release();

    if( lpMAPIPropEntry )
        lpMAPIPropEntry->Release();
}


/*
 -  CMAPIContainerDlg::
 -  OnSetAsTargetSel
 -
 *  Purpose:
 *      Makes selected EntryID the target container.
 *
 */

void CMAPIContainerDlg::OnSetAsTargetSel()
{
    LPSPropValue  lpProps     = NULL;

    int nIndex = m_lpTblList->GetCurSel();
    if( nIndex == -1 )
    {
        MessageBox("Please Select an Entry");
        m_lpTblList->SetFocus();
        return;
    }

    lpProps = (LPSPropValue)m_lpTblList->GetItemData(nIndex);

    cbTargetEntryID = lpProps[1].Value.bin.cb;
    lpTargetEntryID = (LPENTRYID)lpProps[1].Value.bin.lpb;
}


/*
 -  CMAPIContainerDlg::
 -  OnSetAsTargetThis
 -
 *  Purpose:
 *      Makes EntryID of the 'this' container the target container.
 *
 */

void CMAPIContainerDlg::OnSetAsTargetThis()
{
    HRESULT         hResult   = NULL;
    ULONG           ulData[2];
    LPSPropTagArray lpPTArray   = (LPSPropTagArray) &ulData;
    ULONG           cValues         = 0;
    LPSPropValue    lpsPropValue    = NULL;

    //Get the EntryID of the object we want to add
    //Create a PropTagArray with properties to request
    lpPTArray->cValues = 1;
    lpPTArray->aulPropTag[0] = PR_ENTRYID;

    if( FAILED(hResult = m_lpEntry->GetProps( lpPTArray, 0,
                                              &cValues,
                                              &lpsPropValue )))
    {
        DisplayError( m_lpEntry, hResult, "GetProps" );
        cbTargetEntryID = 0;
        lpTargetEntryID = NULL;
        return;
    }

    cbTargetEntryID = lpsPropValue[0].Value.bin.cb;
    lpTargetEntryID = (LPENTRYID)lpsPropValue[0].Value.bin.lpb;
}

/*
 -  CMAPIContainerDlg::
 -  OnResolveNames
 -
 *  Purpose:
 *      Calls the ResolveNames() method of the Container.
 *
 */

void CMAPIContainerDlg::OnResolveNames()
{
    CResolveNameDlg dlgResolveName((LPABCONT)m_lpEntry);
}


/*
 -  CMAPIContainerDlg::
 -  OnOneOffTbl
 -
 *  Purpose:
 *      Calls the Container and request the One-Off Table.
 *
 */

void CMAPIContainerDlg::OnOneOffTbl()
{
    HRESULT         hResult         = NULL;
    LPMAPITABLE     lpTable         = NULL;

    if(FAILED(hResult = m_lpEntry->OpenProperty( PR_CREATE_TEMPLATES,
                                                 (LPIID) &IID_IMAPITable,
                                                 0,
                                                 0,
                                                 (LPUNKNOWN *)&lpTable)))
    {
        DisplayError( m_lpEntry, hResult, "OpenProperty" );
        return;
    }
    
    if(hlibTBLVU)
        TBLVUViewMapiTable( (LPMAPITABLE FAR *)&lpTable, m_hWnd );
    else
        AfxMessageBox( "Table Viewer not available", MB_ICONEXCLAMATION | MB_OK, 0 );

    if(lpTable)
        lpTable->Release();
}


/*
 -  CMAPIContainerDlg::
 -  ~CMAPIContainerDlg
 -
 *  Purpose:
 *      Destructor for class CMAPIContainerDlg.  Will release
 *      the Table being viewed.
 *
 */

CMAPIContainerDlg::~CMAPIContainerDlg()
{
    if(m_lpMAPITable)
    {
        m_lpMAPITable->Release();
        m_lpMAPITable = NULL;
    }
}



/*
 *  CABMailUserDlg Methods.
 */

/*
 -  CABMailUserDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Displays a MailUser.
 *
 */

BOOL CABMailUserDlg::OnInitDialog()
{
    HRESULT         hResult         = NULL;
    ULONG           cValues         = 0;
    LPSPropValue    lpsPropValue    = NULL;
    ULONG           cProps          = 0;
    CWnd *          pBProp          = NULL;
    CWnd *          pBTest          = NULL;
    CWnd *          pBSetAsTarget   = NULL;
    CWnd *          pBDT            = NULL;

    ULONG ulData[4];
    LPSPropTagArray lpPTArray = (LPSPropTagArray) &ulData;

    if(!m_lpEntry)
        return FALSE;

//Disable Propvu button if DLL is not present.
    if(!hlibPROPVU)
    {
        pBProp = GetDlgItem(IDC_PROPERTYDLL);
        pBProp->EnableWindow( FALSE );
    }

//Disable Test button if DLL is not present.
    if(!hlibTEST)
    {
        pBTest = GetDlgItem(IDC_TESTTHISDLL);
        pBTest->EnableWindow( FALSE );
    }

//Disable Display Table button if DLL is not present.
    if(!hlibTBLVU)
    {
        pBDT = GetDlgItem(IDC_PR_DETAILS_TABLE);
        pBDT->EnableWindow( FALSE );
    }

//Disable SetAsTarget button if option is disabled.
    if(!fSetAsTarget)
    {
        pBSetAsTarget = GetDlgItem(IDC_SETASTARGETTHIS);
        pBSetAsTarget->EnableWindow( FALSE );
    }

//Create a PropTagArray with properties to request
    lpPTArray->cValues = 3;
    lpPTArray->aulPropTag[0] = PR_DISPLAY_NAME;
    lpPTArray->aulPropTag[1] = PR_EMAIL_ADDRESS;
    lpPTArray->aulPropTag[2] = PR_ADDRTYPE;

    if( FAILED(hResult = m_lpEntry->GetProps( lpPTArray, 0,
                                                &cValues,
                                                &lpsPropValue )))
    {
        DisplayError( m_lpEntry, hResult, "GetProps" );
        return FALSE;
    }

    SetDlgItemText(IDC_DISPLAYNAME,  lpsPropValue[0].Value.lpszA );
    SetDlgItemText(IDC_EMAILADDRESS, lpsPropValue[1].Value.lpszA );
    SetDlgItemText(IDC_EMAILTYPE,    lpsPropValue[2].Value.lpszA );

    MAPIFreeBuffer( lpsPropValue );

    return TRUE;
}


/*
 -  CABMailUserDlg::
 -  OnOK
 -
 *  Purpose:
 *      Closes the dialog
 *
 */

void CABMailUserDlg::OnOK()
{
    EndDialog(IDOK);
}


/*
 -  CABMailUserDlg::
 -  OnTestThis
 -
 *  Purpose:
 *      Calls Test DLL with the Type of the container.
 *
 */

void CABMailUserDlg::OnTestThis()
{
    HRESULT         hResult         = NULL;
    LPMAPITABLE     lpMAPITable     = NULL;

    if (!hlibTEST)
        return;

    ABTest( (LPADRBOOK FAR *)&lpAdrBook,
            MAPI_MAILUSER,
            &m_lpEntry,
            (HWND)this->m_hWnd );
}


/*
 -  CABMailUserDlg::
 -  OnSetAsTargetThis
 -
 *  Purpose:
 *      Makes EntryID of the MU the target container.
 *
 */

void CABMailUserDlg::OnSetAsTargetThis()
{
    HRESULT         hResult   = NULL;
    ULONG           ulData[2];
    LPSPropTagArray lpPTArray   = (LPSPropTagArray) &ulData;
    ULONG           cValues         = 0;
    LPSPropValue    lpsPropValue    = NULL;

    //Get the EntryID of the object we want to add
    //Create a PropTagArray with properties to request
    lpPTArray->cValues = 1;
    lpPTArray->aulPropTag[0] = PR_ENTRYID;

    if( FAILED(hResult = m_lpEntry->GetProps( lpPTArray, 0,
                                              &cValues,
                                              &lpsPropValue )))
    {
        DisplayError( m_lpEntry, hResult, "GetProps" );
        cbTargetEntryID = 0;
        lpTargetEntryID = NULL;
        return;
    }

    cbTargetEntryID = lpsPropValue[0].Value.bin.cb;
    lpTargetEntryID = (LPENTRYID)lpsPropValue[0].Value.bin.lpb;
}


/*
 -  CABMailUserDlg::
 -  OnGetPropListThis
 -
 *  Purpose:
 *      Calls the base class method GetPropList.
 *
 */

void CABMailUserDlg::OnGetPropListThis()
{
    CMAPIPropDlg::OnGetPropListThis();
}


/*
 -  CABMailUserDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the Mail User viewer dialog.
 *
 */

void CABMailUserDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*
 *  COneOffDlg Methods.
 */

/*
 -  COneOffDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Displays Controls to Create a MailUser.
 *
 */

BOOL COneOffDlg::OnInitDialog()
{
    SetDlgItemText(IDC_DISPLAYNAME,  "foo" );
    SetDlgItemText(IDC_EMAILADDRESS, "\\\\foo\\inbound" );
    SetDlgItemText(IDC_EMAILTYPE,    "MSPEER" );

    return TRUE;
}


/*
 -  COneOffDlg::
 -  OnOK
 -
 *  Purpose:
 *      Closes the dialog
 *
 */

void COneOffDlg::OnOK()
{
    HRESULT     hResult = NULL;
    SCODE       sc  = S_OK;
    ULONG       cbEntryID = 0;
    LPENTRYID   lpEntryID = NULL;

    char szDisplayName[256];
    char szInboxPath[256];
    char szEmailType[15];

    GetDlgItemText(IDC_DISPLAYNAME,  szDisplayName, 256);
    GetDlgItemText(IDC_EMAILADDRESS, szInboxPath,   256);
    GetDlgItemText(IDC_EMAILTYPE,    szEmailType,    15);

    if( FAILED(hResult = lpAdrBook->CreateOneOff( szDisplayName,
                                        szEmailType,
                                        szInboxPath,
                                        0,
                                        &cbEntryID,
                                        &lpEntryID )))
    {
        DisplayError( lpAdrBook, hResult, "CreateOneOff" );
        return;
    }

    sc = ScOpenEntry( cbEntryID, lpEntryID, DT_MAILUSER, fContents, FALSE );
    MAPIFreeBuffer( lpEntryID );
}



/*
 -  COneOffDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the CreateOneOff dialog.
 *
 */

void COneOffDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}



/*
 *  CResolveNameDlg Methods.
 */

/*
 -  CResolveNameDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Initializes the ResolveName Dialog
 *
 */

BOOL CResolveNameDlg::OnInitDialog()
{
    wsprintf(m_szPartialName, "foo; bar" );

    SetDlgItemText(IDC_RESOLVENAME, m_szPartialName);
    SendDlgItemMessage(IDC_RESOLVENAME, EM_SETSEL);

    return TRUE;
}

/*
 -  CResolveNameDlg::
 -  OnOK
 -
 *  Purpose:
 *      Calls ResolveName with the string passed in.
 *
 */

void CResolveNameDlg::OnOK()
{
    HRESULT         hResult     = NULL;
    ULONG           ulUIParam   = (ULONG)(void *)m_hWnd;
    LPADRLIST       lpAdrList   = NULL;
    char            seps[]      = "\n;";
    LPSTR           lpsztoken   = NULL;
    ULONG           ulCount     = 0;
    ULONG           ulEntry     = 0;
    char            szBuffer[256];
    LPFlagList      lpFlagsResolveNames;
    
    GetDlgItemText( IDC_RESOLVENAME, m_szPartialName, 256 );
    GetDlgItemText( IDC_RESOLVENAME, szBuffer, 256 );

    lpsztoken = strtok( szBuffer, seps );

    while (lpsztoken != NULL)
    {
        ulCount++;
        lpsztoken = strtok( NULL, seps );
    }

    MAPIAllocateBuffer( (sizeof(ADRLIST)*ulCount) + (sizeof(ADRENTRY)*ulCount), (LPVOID *) &lpAdrList );

    if(m_fCallResolveNames)
    {
        MAPIAllocateBuffer( sizeof(FlagList) + (sizeof(ULONG)*ulCount), (LPVOID *) &lpFlagsResolveNames );
        lpFlagsResolveNames->cFlags = ulCount;
    }

    lpsztoken = strtok( m_szPartialName, seps );
    lpAdrList->cEntries = ulCount;

    while (lpsztoken != NULL)
    {
        MAPIAllocateBuffer( sizeof(SPropValue), (LPVOID *) &lpAdrList->aEntries[ulEntry].rgPropVals );
        lpAdrList->aEntries[ulEntry].cValues = 1;
        lpAdrList->aEntries[ulEntry].rgPropVals->ulPropTag = PR_DISPLAY_NAME;
        lpAdrList->aEntries[ulEntry].rgPropVals->Value.lpszA = lpsztoken;

        if(m_fCallResolveNames)
        {
            lpFlagsResolveNames->ulFlag[ulEntry] = MAPI_UNRESOLVED;
        }

        ulEntry++;
        lpsztoken = strtok( NULL, seps );
    }

    if(m_fCallResolveNames)
    {
        if( FAILED(hResult = m_lpABContainer->ResolveNames(
                                NULL,
                                0,
                                lpAdrList,
                                lpFlagsResolveNames )))
        {
            DisplayError( m_lpABContainer, hResult, "ResolveNames" );
            _FreeRowSet((LPSRowSet) lpAdrList);
            return;
        }

        MAPIFreeBuffer(lpFlagsResolveNames);
    }
    else
    {
        if( FAILED(hResult = lpAdrBook->ResolveName( ulUIParam,
                                MAPI_DIALOG,
                                "Add:",
                                lpAdrList )))
        {
            DisplayError( lpAdrBook, hResult, "ResolveName" );
            _FreeRowSet((LPSRowSet) lpAdrList);
            return;
        }
    }

    CAdrListDlg dlgAdrList(lpAdrList);
    _FreeRowSet((LPSRowSet)lpAdrList);
}

/*
 -  CResolveNameDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Closes the ResolveName dialog
 *
 */

void CResolveNameDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*
 -  CAdrListDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      InitDialog function for Adr Properties List.  Fills the
 *      ListBox with property information from the Address List.
 *
 */

BOOL CAdrListDlg::OnInitDialog()
{
    ULONG cEntries  = 0;
    ULONG cProp     = 0;

    VERIFY(m_AdrListBox.SubclassDlgItem(IDC_ADRLIST, this));

    if(!m_lpAdrList)
        return FALSE;

    for(cEntries=0; cEntries<m_lpAdrList->cEntries; cEntries++)
    {
        for(cProp=0; cProp<m_lpAdrList->aEntries[cEntries].cValues; cProp++)
        {
            if( m_lpAdrList->aEntries[cEntries].rgPropVals[cProp].ulPropTag == PR_DISPLAY_NAME )
            m_AdrListBox.AddString( m_lpAdrList->aEntries[cEntries].rgPropVals[cProp].Value.lpszA );
        }
    }

    SetWindowText(m_Caption);
    m_AdrListBox.SetCurSel( 0 );
    return TRUE;

}


/*
 -  CAdrListDlg::
 -  OnOK
 -
 *  Purpose:
 *      Will open a dialog with information about the Adr List
 *
 */

void CAdrListDlg::OnOK()
{
    int nIndex = m_AdrListBox.GetCurSel();
    if (nIndex == -1)
    {
        MessageBox("Please Select an Entry");
        m_AdrListBox.SetFocus();
        return;
    }

    CPropertiesDlg * pdlgProperties = new CPropertiesDlg(
    m_lpAdrList->aEntries[nIndex].cValues,
    m_lpAdrList->aEntries[nIndex].rgPropVals,
    "AdrEntry",
    fmAProperty );
}


/*
 -  CAdrListDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the Adr Property ListBox viewer
 *
 */

void CAdrListDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*
 -  CAdrListDlg::
 -  OnRecipOptions
 -
 *  Purpose:
 *      Displays a modal per-recipient options dialog box on
 *      a particular recipient entry.  The function returns
 *      a new ADRENTRY with the appropriate per-recipient options.
 *
 */

void CAdrListDlg::OnRecipOptions()
{
    HRESULT         hResult     = NULL;
    ULONG           ulUIParam   = (ULONG)(void *)m_hWnd;
    LPADRENTRY      lpRecip     = NULL;

    int nIndex = m_AdrListBox.GetCurSel();
    if (nIndex == -1)
    {
        MessageBox("Please Select an Entry");
        m_AdrListBox.SetFocus();
        return;
    }

    if(!m_lpAdrList)
        return;

    lpRecip = &m_lpAdrList->aEntries[nIndex];

    if( hResult = lpAdrBook->RecipOptions( ulUIParam,
                                           0,
                                           lpRecip ))
    {
        DisplayError( lpAdrBook, hResult, "RecipOptions" );
    }
}


/*
 -  CAdrListDlg::
 -  OnOpenEntry
 -
 *  Purpose:
 *      Finds the ENTRYID from the currently selected
 *      recipient, and open entries it.
 */

void CAdrListDlg::OnOpenEntry()
{
    SCODE   sc      = S_OK;
    ULONG   cProp   = 0;
    ULONG   ulDisplayType = 0;

    int nIndex = m_AdrListBox.GetCurSel();
    if (nIndex == -1)
    {
        MessageBox("Please Select an Entry");
        m_AdrListBox.SetFocus();
        return;
    }

    if(!m_lpAdrList)
        return;

    for(cProp=0; cProp<m_lpAdrList->aEntries[nIndex].cValues; cProp++)
    {
        if( m_lpAdrList->aEntries[nIndex].rgPropVals[cProp].ulPropTag == PR_DISPLAY_TYPE )
        {
            ulDisplayType = m_lpAdrList->aEntries[nIndex].rgPropVals[cProp].Value.ul;
        }
    }


    for(cProp=0; cProp<m_lpAdrList->aEntries[nIndex].cValues; cProp++)
    {
        if( m_lpAdrList->aEntries[nIndex].rgPropVals[cProp].ulPropTag == PR_ENTRYID )
        {
            sc = ScOpenEntry( m_lpAdrList->aEntries[nIndex].rgPropVals[cProp].Value.bin.cb,
            (LPENTRYID)m_lpAdrList->aEntries[nIndex].rgPropVals[cProp].Value.bin.lpb,
            ulDisplayType,
            fContents,
            FALSE );
            break;
        }
    }
}


/*
 -  CAdrListDlg::
 -  OnPrepareRecips
 -
 *  Purpose:
 *      Calls PrepareRecips against the current Address List.
 */

void CAdrListDlg::OnPrepareRecips()
{
    HRESULT         hResult   = NULL;
    ULONG           ulFlags   = 0;
    ULONG           ulData[6];
    LPSPropTagArray lpPTArray = (LPSPropTagArray) &ulData;
    CString         szBuffer;

    lpPTArray->cValues = 3;
    lpPTArray->aulPropTag[0] = PR_DISPLAY_NAME;
    lpPTArray->aulPropTag[1] = PR_SURNAME;
    lpPTArray->aulPropTag[2] = PR_COMMENT;

    if ( FAILED ( hResult = lpAdrBook->PrepareRecips( ulFlags,
                                                      lpPTArray,
                                                      m_lpAdrList )))
    {
        DisplayError( lpAdrBook, hResult, "PrepareRecips" );
        return;
    }

    szBuffer += "PrepareRecips() successfully called against Address List.";
    szBuffer += "\r\nThe following properties have been requested:\r\n";
    szBuffer += "\r\nPR_DISPLAY_NAME";
    szBuffer += "\r\nPR_SURNAME";
    szBuffer += "\r\nPR_COMMENT";
    szBuffer += "\r\n\r\nUse Recip Props to view internals of modified List.";
    AfxMessageBox( szBuffer, MB_OK, 0 );
}


/*
 -  CAdrListDlg::
 -  ~CAdrListDlg
 -
 *  Purpose:
 *      Destructor for CAdrListDlg Class
 *
 */

CAdrListDlg::~CAdrListDlg()
{
}


/*
 -  CPropertiesDlg::
 -  RenderProperties
 -
 *  Purpose:
 *      Fills the CListBox with Textized Property information.
 *
 */

void CPropertiesDlg::RenderProperties( ULONG cValues, LPSPropValue lpProps, ULONG fmDisplay )
{
    ULONG   idx;
    int     rgTabStops[2];
    char    szBuffer[1024];
    char    szID[50];
    char    szData[512];
    char    szType[32];

    /* Clear the ListBox, Set its size, and set its TabStops */

    SendDlgItemMessage(IDC_PROPLIST, LB_RESETCONTENT, 0, 0);

    SendDlgItemMessage(IDC_PROPLIST, LB_SETHORIZONTALEXTENT,
                (WPARAM)VALUES_LB_HOR_SIZE, 0);

    rgTabStops[0] = VALUES_LB_TAB1;
    rgTabStops[1] = VALUES_LB_TAB2;

    SendDlgItemMessage(IDC_PROPLIST, LB_SETTABSTOPS,
                (WPARAM)2, (LPARAM)rgTabStops);

    /* Populate the ListBox with the data for this row */

    for(idx = 0; idx < cValues; idx++)
    {
        szID[0]     = '\0';
        szData[0]   = '\0';
        szType[0]   = '\0';
        szBuffer[0] = '\0';

        if( fmProperty & fmDisplay )
        {
            if(GetString("PropIDs", PROP_ID(lpProps[idx].ulPropTag), szID))
            {
                lstrcat(szBuffer, szID );
                lstrcat(szBuffer, "\t");
            }
            else
            {
                wsprintf(szBuffer,"%#04X\t", PROP_ID(lpProps[idx].ulPropTag));
            }
        }


        if( fmPropType & fmDisplay )
        {
            if(GetString("PropType", PROP_TYPE(lpProps[idx].ulPropTag), szType))
            {
                lstrcat(szBuffer, szType);
                lstrcat(szBuffer, "\t");
            }
            else
            {
                wsprintf(szType, "%#04X\t", PROP_TYPE(lpProps[idx].ulPropTag));
                lstrcat(szBuffer, szType);
            }
        }


        if( fmPropValue & fmDisplay )
        {
            SzGetPropValue(szData, (LPSPropValue)&lpProps[idx]);
            lstrcat(szBuffer, szData);
        }

    SendDlgItemMessage(IDC_PROPLIST, LB_ADDSTRING, 0, (LPARAM)szBuffer);
    }
}



/*
 -  CPropertiesDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      InitDialog function for CPropertiesDlg.  Fills the CListBox
 *      with Textized Property information.
 *
 */

BOOL CPropertiesDlg::OnInitDialog()
{
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED);

    VERIFY(m_ListBox.SubclassDlgItem(IDC_PROPLIST, this));

    if(!m_lpProps)
        return FALSE;

    RenderProperties( m_cValues, m_lpProps, m_fmDisplay );

    SetWindowText(m_Caption);
    m_ListBox.SetCurSel( 0 );
    return TRUE;
}


/*
 -  CPropertiesDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the CPropertiesDlg dialog
 *
 */

void CPropertiesDlg::OnCancel()
{
    delete this;
}



/*
 -  CNamedIDsDlg::
 -  RenderNamedIDs
 -
 *  Purpose:
 *      Fills the CListBox with Named IDs Information.
 *
 */

void CNamedIDsDlg::RenderNamedIDs( ULONG cPropNames, LPMAPINAMEID FAR * lppPropNames )
{
    char szBuffer[256];
    int i = 0;
#ifdef WIN16
    WCHAR *pwsz;
    ULONG   cch;
#endif

    /* Clear the ListBox, Set its size, and set its TabStops */

    SendDlgItemMessage(IDC_PROPLIST, LB_RESETCONTENT, 0, 0);

    SendDlgItemMessage(IDC_PROPLIST, LB_SETHORIZONTALEXTENT,
                (WPARAM)VALUES_LB_HOR_SIZE, 0);

    if( !lppPropNames )
    {
        return;
    }
    
    while( i < cPropNames )
    {
        if( !lppPropNames[i] )
        {
            i++;
            continue;
        }
        
        switch( (*lppPropNames[i]).ulKind )
        {
            case MNID_ID:
            {
                 wsprintf( szBuffer, "(*lppPropNames[%d]).Kind.lID = %ld", i, (*lppPropNames[i]).Kind.lID );
            }
            break;
            
            case MNID_STRING:
            {
#ifdef WIN16
                pwsz = (*lppPropNames[i]).Kind.lpwstrName;
    
                cch = 0;
                while (*pwsz++ != 0)
                {
                    szBuffer[cch]=(char)*pwsz;
                    cch += 1;
                }
        
#else
                if( (*lppPropNames[i]).Kind.lpwstrName )
                {
                     wsprintf( szBuffer, "(*lppPropNames[%d]).Kind.lpwstrName = %ws", i, (*lppPropNames[i]).Kind.lpwstrName );
                }
                else
                {
                    wsprintf( szBuffer, "(*lppPropNames[%d]) = NULL", i );
                }
#endif
            }
            break;
            
            default:
            {
                wsprintf( szBuffer, "(*lppPropNames[i]).ulKind = %lu  Invalid ID type in MAPINAMEID", (*lppPropNames[i]).ulKind );  
                SendDlgItemMessage(IDC_PROPLIST, LB_ADDSTRING, 0, (LPARAM)szBuffer);
                return;
            }

        }//switch
        SendDlgItemMessage(IDC_PROPLIST, LB_ADDSTRING, 0, (LPARAM)szBuffer);
        i++;
    }

}

/*
 -  CNamedIDsDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      InitDialog function for CNamedIDsDlg.  Fills the CListBox
 *      with Named IDs.
 *
 */

BOOL CNamedIDsDlg::OnInitDialog()
{
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED);

    VERIFY(m_ListBox.SubclassDlgItem(IDC_PROPLIST, this));

    if(!m_lppPropNames)
        return FALSE;

    RenderNamedIDs( m_cPropNames, m_lppPropNames );

    SetWindowText(m_Caption);
    m_ListBox.SetCurSel( 0 );
    return TRUE;
}


/*
 -  CNamedIDsDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the CNamedIDsDlg dialog
 *
 */

void CNamedIDsDlg::OnCancel()
{
    delete this;
}


/*
 -  CGetError::
 -  SzError
 -
 *  Purpose:
 *      For printable Error String.  Uses the stringtable
 *      to textize the scode.
 *
 *  Parameters:
 *      szMsg       - Message Text
 *      sc          - SCODE
 *
 *  Returns:
 *      szError     - Textized information
 *
 */

LPSTR CGetError::SzError( LPSTR szMsg, SCODE sc )
{

    wsprintf( m_szMessage, "%s: ", szMsg );

    if(!GetString( "MAPIErrors", sc, m_szResult ))
    {
        lstrcpy( m_szResult, "??" );
        wsprintf( m_szBuffer, " 0x%04x", sc );
        strcat( m_szResult, m_szBuffer );
    }

    strcat( m_szMessage, m_szResult );

    return m_szMessage;
}

#ifdef WIN16
LPSTR CGetError::SzError( LPSTR szMsg, HRESULT hResult )
{
    SzError( szMsg, GetScode(hResult));
    return m_szMessage;
}
#endif


/*
 -  Logprintf
 -
 *  Purpose:
 *      Formats a string and prints it to OutputDebugString.
 *
 *  Parameters:
 *      LPSTR - Pointer to string to format
 *      ...   - variable argument list
 *
 */

void Logprintf( LPSTR lpszFormat, ... )
{
    va_list pArgs   = NULL;
    char    szLogStr[1024];
    int     i       = 0;

    va_start(pArgs, lpszFormat);
    vsprintf(szLogStr, lpszFormat, pArgs);
    va_end(pArgs);

    i = strlen( szLogStr);
    szLogStr[i] = '\n';
    szLogStr[i+1] = '\0';

    OutputDebugString( szLogStr );
}


/*
 -  _FreeRowSet
 -
 *  Purpose:
 *      Given an RowSet, will free it.  Note that AdrLists
 *      have the same structure and can be freed by this
 *      call as well.
 *
 *  Parameters:
 *      LPSRowSet - Pointer to RowSet to free.
 *
 */

void _FreeRowSet( LPSRowSet lpRows )
{
    ULONG cRows;

    if( !lpRows )
        return;

    for( cRows = 0; cRows < lpRows->cRows; ++cRows )
    {
        MAPIFreeBuffer( lpRows->aRow[cRows].lpProps );
    }

    MAPIFreeBuffer( lpRows );
}


/*
 -  SCDisplayError
 -
 *  Purpose:
 *      Displays a MessageBox with Scode Textized.
 *
 *  Parameters:
 *      LPUNKNOWN - Pointer to the Object
 *      HRESULT   - Error code returned on the last call on this object.
 *      LPSTR     - Pointer to string to for value added information.
 */

void DisplayError( LPUNKNOWN lpUnkObj, HRESULT hResult, LPSTR szFunction )
{
    CString     szBuffer;
    CGetError   E;
    char        szLowLevelError[20];
    char        szContext[20];
    ULONG       ulLowLevelError = 0;
    LPMAPIERROR lpMAPIError = NULL;

//Check to see if user pressed Cancel on a dialog.  Since
//this is an return value that will occur naturally, don't
//process further.

    if( GetScode( hResult ) == MAPI_E_USER_CANCEL )
        return;

//Cast the LPUNKNOWN object to one that contains GetLastError.
//Since this call is guaranteed to be in the same location on
//all objects, this is a safe assumption.

    ((LPMAPIPROP)lpUnkObj)->GetLastError( hResult, 0, &lpMAPIError );

//Always start with the textized error code

    szBuffer = E.SzError(szFunction, hResult);

//If GetLastError returns a Message then append it
    if ( lpMAPIError )
    {
        if( lpMAPIError->lpszError )
        {
            szBuffer += "\r\n\r\n";
            szBuffer += lpMAPIError->lpszError;
        }

//If GetLastError had a LowLevelError to report then append it

    if( lpMAPIError->ulLowLevelError )
    {
        szBuffer += "\r\nGetLastError( lpMAPIError->ulLowLevelError ): ";
        wsprintf( szLowLevelError, "0x%04x", lpMAPIError->ulLowLevelError );
        szBuffer += szLowLevelError;
    }

//If GetLastError returns a Component then append it

    if( lpMAPIError->lpszComponent )
    {
        szBuffer += "\r\nlpMAPIError->Component: ";
        szBuffer += lpMAPIError->lpszComponent;
    }

//If GetLastError had a Context to report then append it

    if( lpMAPIError->ulContext )
    {
        szBuffer += "\r\nGetLastError( lpMAPIError->ulContext ): ";
        wsprintf( szContext, "0x%04x", lpMAPIError->ulContext );
        szBuffer += szContext;
    }
}
//Display the Error

    AfxMessageBox( szBuffer, MB_ICONEXCLAMATION | MB_OK, 0 );
    MAPIFreeBuffer( lpMAPIError );
}


/*
 -  ScGetRow
 -
 *  Purpose:
 *      Given a MAPITable and an index, find the RowSet.
 *      Tables tend to be dynamic.  In a static table,
 *      this function works well.  In tables that update
 *      constantly or have difficulty maining a static
 *      RowSet, this function is less than perfect.
 *
 *  Parameters:
 *      LPMAPITABLE - Pointer of Table to traverse
 *      int         - Index of RowSet to find
 *      * LPSRowSet - Address of RowSet to return.
 *
 */

SCODE ScGetRow( LPMAPITABLE lpMAPITable, int nIndex, LPSRowSet *lppRows )
{
    HRESULT hResult = NULL;

    if( !lpMAPITable )
    {
        return E_INVALIDARG;
    }

    if( FAILED(hResult = lpMAPITable->SeekRow(BOOKMARK_BEGINNING,
                                            nIndex,
                                            NULL )))
    {
        DisplayError( lpMAPITable, hResult, "SeekRow" );
        goto error;
    }

    if( FAILED(hResult = lpMAPITable->QueryRows( 1,
                                            0,
                                            lppRows )))
    {
        DisplayError( lpMAPITable, hResult, "QueryRows" );
        goto error;
    }

    error:
    return GetScode(hResult);
}


/*
 *
 -  ScOpenEntry
 -
 *  Purpose:
 *      Performs OpenEntry on the EntryID, finds out the object type,
 *      then creates an appropriate dialog to display the object in.
 *
 *  Parameters:
 *      ULONG       - Count of bytes in lpEntryID.
 *      LPENTRYID   - Pointer to the EntryID.
 *
 */

SCODE ScOpenEntry( ULONG cbEntryID, LPENTRYID lpEntryID, ULONG ulDisplayType, ULONG ulTableTypeFrom, BOOL fCONVENIENT_DEPTH)
{
    HRESULT     hResult         = NULL;
    ULONG       ulObjType       = 0;
    LPMAPIPROP  lpEntry         = NULL;
    BOOL        fRootContainer  = FALSE;
    ULONG       ulFlags         = 0;
    ULONG       ulSwitchType    = 0;

    if(fOpenEntry)
        ulFlags = ulFlags | MAPI_MODIFY;
        
    if( FAILED(hResult = lpAdrBook->OpenEntry(cbEntryID,
                                        lpEntryID,
                                        NULL,
                                        ulFlags,
                                        &ulObjType,
                                        (LPUNKNOWN*)&lpEntry)))
    {
        DisplayError( lpAdrBook, hResult, "OpenEntry" );
        return GetScode( hResult );
    }

    if(!lpEntryID)
        fRootContainer = TRUE;

    //DT_MAILUSER and DT_NOT_SPECIFIC are both defined as ((ULONG) 0x00000000),
    // so we need a way to determine whether or not we are looking at a
    // contents table (PR_DISPLAY_TYPE) or a a hierarchy table (PR_OBJECT_TYPE)

    if((ulTableTypeFrom == fRoot) || (ulTableTypeFrom == fHierarchy))
    {
        switch(ulDisplayType)
        {
            case DT_NOT_SPECIFIC:
                ulSwitchType = ulObjType;
                break;

            case DT_MODIFIABLE:
            case DT_GLOBAL:
            case DT_LOCAL:
            case DT_WAN:
                ulSwitchType = MAPI_DISTLIST;
                break;
                
            default:
                ulSwitchType = ulObjType;
        }
    }
    else
    {
        switch(ulDisplayType)
        {
            case DT_MAILUSER:
            case DT_REMOTE_MAILUSER:
                ulSwitchType = MAPI_MAILUSER;
                break;

            case DT_DISTLIST:
            case DT_PRIVATE_DISTLIST:
                ulSwitchType = MAPI_DISTLIST;
                break;
                
            default:
                ulSwitchType = ulObjType;
        } 
    }

    switch(ulSwitchType)
    {
        case MAPI_MAILUSER:
             CABMailUserDlg( (LPMAILUSER)lpEntry );
             break;

        case MAPI_ABCONT:
        case MAPI_DISTLIST:
             CMAPIContainerDlg( (LPABCONT)lpEntry, ulObjType, ulTableTypeFrom, fCONVENIENT_DEPTH );
             break;

        default:
            //Unknown Type
            lpEntry->Release();
            break;
    }

    return GetScode( hResult );
}


/*
 -  CMainWindow::
 -  CreateDisplayFonts
 -
 *  Purpose:
 *      Create Fonts for the ownerdraw listbox
 *
 */

void CMainWindow::CreateDisplayFonts()
{
    if (pBoldFont)
        return;

    pBoldFont  = new CFont;
    pNormFont  = new CFont;
    pLightFont = new CFont;

    LOGFONT lf;
    CClientDC DC ( this );

    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
    char static BASED_CODE szFaceName[] = "MS Sans Serif";
    lstrcpy(lf.lfFaceName, szFaceName);
    lf.lfQuality = PROOF_QUALITY;
    lf.lfOutPrecision = OUT_CHARACTER_PRECIS;

    // Create Dark font
    // 10 point height Sans Serif font
    lf.lfHeight = -MulDiv(8,
                          DC.GetDeviceCaps( LOGPIXELSY),
                          72);
    lf.lfWeight = FW_BOLD;
    if ((pBoldFont->CreateFontIndirect(&lf)) == FALSE)
    {
        TRACE0("Warning: Using system font for List box font\n");
        pBoldFont->CreateStockObject(SYSTEM_FONT);

    }

    // Create Normal font
    // 9 point height Sans Serif font
    lf.lfHeight = -MulDiv(8,
                          DC.GetDeviceCaps( LOGPIXELSY),
                          72);
    lf.lfWeight = FW_SEMIBOLD;
    if ((pNormFont->CreateFontIndirect(&lf)) == FALSE)
    {
        TRACE0("Warning: Using system font for List box font\n");
        pNormFont->CreateStockObject(SYSTEM_FONT);

    }

    // Create light font
    // 8 point height Sans Serif font
    lf.lfHeight = -MulDiv(8,
                          DC.GetDeviceCaps( LOGPIXELSY),
                          72);

    lf.lfWeight = FW_MEDIUM;
    if ((pLightFont->CreateFontIndirect(&lf)) == FALSE)
    {
        TRACE0("Warning: Using system font for List box font\n");
        pLightFont->CreateStockObject(SYSTEM_FONT);

    }
}


/*
 -  CMainWindow::
 -  DeleteDisplayFonts
 -
 *  Purpose:
 *      Deletes the Fonts for the ownerdraw listbox
 *
 */

void CMainWindow::DeleteDisplayFonts()
{
    delete pBoldFont;
    delete pNormFont;
    delete pLightFont;

    pBoldFont  = NULL;
    pNormFont  = NULL;
    pLightFont = NULL;
}


/*
 -  CreateMAPIGuid
 -
 *  Purpose:
 *      Create a Guid.  Call MakeGuid helper function.
 *      In case the values changes, we only have to change it once.      
 *      The values are from PS_MAPI.
 */

void CreateMAPIGuid( LPGUID lpPropSetGuid )
{
    MakeGuid( lpPropSetGuid,
              0x3B7893D4,
              0x094C,
              0x101B,
              0x8D,
              0x98,
              0x00,
              0xAA,
              0x00,
              0x3C,
              0xD2,
              0x07 );
}
