/*********************************************************************/
/*
 -  mdbview.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for mdbview
 *      Classes for Setting Property dialog, Main Window, Application,
 *      and property display dialog are all defined in this header.
 */
/*********************************************************************/

#ifndef __mdbview_h_        // test defined
#define __mdbview_h_

// curently xvport has problems on CHICAGO platform
#ifdef CHICAGO
#undef XVPORT
#else
#define XVPORT
#endif

/******************** Utility/Tool Function Prototypes ***************/

void FreeAdrList( LPADRLIST lpadrlist );

/******************** Application Defines ****************************/

// listbox sizes

#define MSG_LISTBOX_HORIZONTAL_SIZE     2000
#define PROP_LISTBOX_HORIZONTAL_SIZE    2000
#define PROP_LISTBOX_TAB1_SIZE          120
#define PROP_LISTBOX_TAB2_SIZE          185
#define MAPILOG_LISTBOX_TAB1_SIZE       250


// Message Box Styles 

#define MBS_ERROR                       (MB_ICONSTOP | MB_OK )
#define MBS_INFO                        (MB_ICONINFORMATION | MB_OK)
#define MBS_OOPS                        (MB_ICONEXCLAMATION | MB_OK)

// application window size 
   
#define MW_LEFT                         5
#define MW_TOP                          5
#define MW_RIGHT                        500
#define MW_BOTTOM                       200


#define MAX_LOG_BUFF                    255 // for displaying PT_BINARY

#define COMMENT_SIZE                    130


// focus of button enables/disables

#define FOCUS_PROPERTY                  6
#define FOCUS_MESSAGE                   7
#define FOCUS_ATTACH                    8
#define FOCUS_FOLDER                    9
#define FOCUS_NONE                      10

/*********************************************************************/
//#ifdef XVPORT

// used to keep track of one Advise on object
typedef struct _Connect
{
    ULONG       ulConnection;           // connection number returned from Advise
    char        szContext[256];          // description of connection lpvContext
} CONNECT, *LPCONNECT;

// used to keep track of all the Advises an object Registers
typedef struct _Notifs
{
    ULONG       cConnect;               // number of current Advises Registerd on object
    CONNECT     Connect[25];            // pointer to an array of     
} NOTIFS, *LPNOTIFS;

void    AdviseObj(  LPUNKNOWN       lpUnk,
                    ULONG           ulObjType, 
                    LPNOTIFCALLBACK lpfnNotifCallback,
                    ULONG           ulEventType, 
                    ULONG           cbEntryID,
                    LPENTRYID       lpEntryID,
                    LPSTR           lpszComment,
                    LPVOID          lpvExplicitContext,
                    LPNOTIFS FAR    *lppNotifs);

#ifdef XVPORT
void AdviseAllStatusObjects(void);
#endif

void UnadviseAll(   LPUNKNOWN       lpUnk,
                    ULONG           ulObjType, 
                    LPSTR           lpszDescription,
                    LPNOTIFS FAR    *lppNotifs);

//#endif
/******************** Class Definitions ******************************/

/*********************************************************************/
/*
 -  CGetError
 -
 *  Purpose:
 *      Defines the methods to Get Error Strings.
 *
 */
/*********************************************************************/

class CGetError
{
    private:
        char    m_szMessage[COMMENT_SIZE];      // this is so notification will work/display correctly
        char    m_szResult[80];
        char    m_szBuffer[80];

    public:
        CGetError()
        {
            m_szMessage[0]  = '\0';
            m_szResult[0]   = '\0';
        };

        ~CGetError() { };           

#ifdef WIN16
        LPSTR SzError( LPSTR, HRESULT );
#endif
        LPSTR SzError( LPSTR, SCODE );
};

/*********************************************************************/
/*
 -  CMainWindow
 -
 *  Purpose:
 *      Defines an Object that holds all the info needed for the CLNT's
 *      main window.  
 *
 */
/*********************************************************************/

class CMainWindow : public CFrameWnd
{
protected:
    void ToggleMenu(BOOL bState);
    
public:
    afx_msg void OnAbout(); 
    afx_msg void OnExit();
    afx_msg void OnClose(); 
    afx_msg void OnLogon(); 
    afx_msg void OnLogoff();
    afx_msg void OnOpenMDB(); 
    afx_msg void OnMDBProps(); 
    afx_msg void OnABProps(); 
    afx_msg void OnMDBSupportMask(); 


    afx_msg void OnIPMSubtree(); 
    afx_msg void OnIPMOutbox(); 
    afx_msg void OnIPMWasteBasket(); 
    afx_msg void OnIPMSentMail(); 


    afx_msg void OnGetReceiveFolder();
    afx_msg void OnGetReceiveFolderTable();
    afx_msg void OnGetStatusTable();
    afx_msg void OnGetStoresTable();    
    afx_msg void OnEnumAdrTypes();    
    afx_msg void OnQueryIdentity();    
    afx_msg void OnQueryDefaultMsgOptions();    
    afx_msg void OnOpenRootFolder(); 
    afx_msg void OnStoreLogoff(); 
    afx_msg void OnOpenStatusObj(); 

    afx_msg void OnGetLastError(); 
    afx_msg void OnAddress(); 
    afx_msg void OnCreateOneOff(); 
    afx_msg void OnQueryDefRecipOpts(); 
    afx_msg void OnResolveName(); 
    afx_msg void OnSessionRegNotif(); 
    afx_msg void OnStatusRegNotif(); 
    afx_msg void OnMDBRegNotif(); 
    afx_msg void OnTableRegNotif(); 
    afx_msg void OnABRegNotif(); 
    afx_msg void OnNotifState();
    
    afx_msg void PostNcDestroy();

    DECLARE_MESSAGE_MAP()
};
    
/*********************************************************************/
/*
 -  CTheApp
 -
 *  Purpose:
 *      Defines an Object that holds all the info needed for the MDBViewer
 *      Application.  
 *
 */
/*********************************************************************/

class CTheApp : public CWinApp
{
protected:               
    CMainWindow m_MainWnd;

public:
    BOOL    InitInstance();
    int     ExitInstance();
    
};

/*********************************************************************/
/*
 -  CPropDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Dialogs with Properties
 *      This class is the base class for all LPMAPIPROP dialogs
 *      it defines common preperty display and property setting functions
 *      for all LPMAPIPROP objects in the message store.  The objects
 *      That inherit from this class are Folder, Message, and Attachment.
 *
 */
/*********************************************************************/

class CPropDlg : public CDialog
{
public:
    LPMAPIPROP  m_lpEntry;          
    char        m_szLogBuf[512];    // used for MAPI log window output
    char        m_szError[80];      // used for passing into GetString for MAPIErrors
    HRESULT     m_hResult;
    CGetError   m_E;

    CPropDlg(   LPMAPIPROP  lpEntry,
                UINT        Prop, 
                CWnd        *pParentWnd)
            : CDialog(Prop,  pParentWnd) 
        {
            m_lpEntry   = lpEntry;
            m_hResult   = hrSuccess;

            if( ! Create(Prop,NULL) )         // create dialog here
            {
                MessageBox("CreateDialog Failed","Client",MBS_ERROR);           
                delete this;
            }
        }
    
    ~CPropDlg() {  }

    // displaying properties
    virtual void OnProperty() = 0;      
    void OnType();  
    void OnValues();
    void OnData();  
    void OnString();   
    void OnHex();
    void OnDecimal();    
    void RedrawPropTable();             
    void ResetPropButtons();          

    void OnClearMapiLog();                

    // property operations
    void Stub();                        // generic NYI message
    void OnSave();                  
    void OnSetProps();
    void OnDeleteProp();
};


/*********************************************************************/
/*
 -  COpenStoreDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Open Store Dialog
 *
 */
/*********************************************************************/

class COpenStoreDlg : public CModalDialog
{
public:
    LPMAPITABLE     m_lptblStores;
    LPSRowSet       m_lpRows;

    COpenStoreDlg(CWnd * pParentWnd)
        : CModalDialog(STORELOGON, pParentWnd)
    {
        m_lptblStores   = NULL;
        m_lpRows        = NULL;
    }

    ~COpenStoreDlg();   

    BOOL OnInitDialog();

    void OnOpen();
    void OnReadOnly();  
    void DisplayStores();   
    void OnSetDefaultStore();
    void OnCancel();
    
    DECLARE_MESSAGE_MAP();
};


/*********************************************************************/
/*
 -  CStoreSupportMaskDlg
 -
 *  Purpose:
 *      Defines the controls for the GetPropList Property Dialog
 *
 */
/*********************************************************************/

class CStoreSupportMaskDlg : public CModalDialog
{
public:

    ULONG           m_cVals;
    LPSPropValue    m_lpspva;

    CStoreSupportMaskDlg(CWnd  *pParentWnd)
        : CModalDialog(SUPPORTMASK, pParentWnd)
    {
        m_cVals     = 0;
        m_lpspva    = NULL;
    }

    ~CStoreSupportMaskDlg();    

    BOOL OnInitDialog();
    void OnCancel();    

};


/*********************************************************************/
/*
 -  CEnumAdrTypesDlg
 -
 *  Purpose:
 *      Defines the controls for the GetPropList Property Dialog
 *
 */
/*********************************************************************/

class CEnumAdrTypesDlg : public CModalDialog
{
public:

    LPTSTR  *m_lppszAdrTypes;

    CEnumAdrTypesDlg(CWnd  *pParentWnd)
        : CModalDialog(IDC_ENUMADRTYPES, pParentWnd)
    {
        m_lppszAdrTypes       = NULL;
    }

    ~CEnumAdrTypesDlg();    

    BOOL OnInitDialog();
    
    void OnCancel();    
//    void DisplayAdrTypes();

};


/*********************************************************************/
/*
 -  CQueryDefaultMsgOptsDlg
 -
 *  Purpose:
 *      Defines the controls for the GetPropList Property Dialog
 *
 */
/*********************************************************************/

class CQueryDefaultMsgOptsDlg : public CModalDialog
{
public:

    LPSPropValue    m_lpspva;
    ULONG           m_cValues;    

    CQueryDefaultMsgOptsDlg(CWnd  *pParentWnd)
        : CModalDialog(QUERYDEFAULTMSGOPTS, pParentWnd)
    {
        m_lpspva  = NULL;
        m_cValues = 0;
    }

    ~CQueryDefaultMsgOptsDlg(); 

    BOOL OnInitDialog();
    
    void OnCancel();    
};
      


/*********************************************************************/
/*
 -  CEntryListDlg
 -
 *  Purpose:
 *      Defines the controls for the Entryist dlg
 *
 */
/*********************************************************************/

class CEntryListDlg : public CModalDialog
{
public:
    
    LPENTRYLIST     *m_lppEntryList;
    CString         m_List;
        
    CEntryListDlg(CWnd  *pParentWnd)
        : CModalDialog(ENTRYLIST, pParentWnd)
    {
        m_lppEntryList       = NULL;
    }

    ~CEntryListDlg();   

    BOOL OnInitDialog();
    
    void OnCancel();    
    void OnClearList();
    
    void DisplayList();

    DECLARE_MESSAGE_MAP();

};



/*********************************************************************/
/*
 -  CAdrListDlg
 -
 *  Purpose:
 *      Defines the controls for the Entryist dlg
 *
 */
/*********************************************************************/

class CAdrListDlg : public CModalDialog
{
public:
    
    LPADRLIST       m_lpAdrList;
    CString         m_List;
        
    CAdrListDlg(CWnd  *pParentWnd)
        : CModalDialog(ADRLIST, pParentWnd)
    {
        m_lpAdrList       = NULL;
    }

    ~CAdrListDlg(); 

    BOOL OnInitDialog();
    
    void OnCancel();    
    
    void DisplayAdrList();
};

      
      


/*********************************************************************/
/*
 -  CPropDisplayDlg
 -
 *  Purpose:
 *      Defines the controls for the Entryist dlg
 *
 */
/*********************************************************************/

class CPropDisplayDlg : public CModalDialog
{
public:
    LPSPropValue    m_lpspva;
    LPENTRYID       m_lpEntryID;
    ULONG           m_ulValues;
        
    CString         m_List;
        
    CPropDisplayDlg(CWnd  *pParentWnd)
        : CModalDialog(PROP_DISPLAY, pParentWnd)
    {
        m_lpspva        = NULL;
        m_ulValues      = 0;
        m_lpEntryID     = NULL;
    }

    ~CPropDisplayDlg(); 

    BOOL OnInitDialog();
    
    void OnCancel();    
    
    void DisplayProps();
};


/*********************************************************************/
/*
 -  CNotifDlg
 -
 *  Purpose:
 */
/*********************************************************************/

// test defined
#define MAPI_TABLE  ((ULONG) 0x0000000C)


//#ifdef XVPORT
class CNotifDlg : public CModalDialog
{
public:   
    LPNOTIFS        *m_lppNotifs;
    CString         m_Description;
    LPUNKNOWN       m_lpUnk;
    ULONG           m_ulObjType;
    LPENTRYID       m_lpEntryID;
    ULONG           m_cbEntryID;
            
    CNotifDlg(CWnd  *pParentWnd)
        : CModalDialog(NOTIF, pParentWnd)
    {
        m_lppNotifs      = NULL;
        m_lpUnk          = NULL;
        m_ulObjType      = 0;
        m_lpEntryID      = NULL;
        m_cbEntryID      = 0;
    }

    ~CNotifDlg();   

    BOOL OnInitDialog();
    void OnCancel();    
    void Display();

    void OnNewAdvise();
    void OnUnadvise();

    DECLARE_MESSAGE_MAP();
   
};

//#endif


/*********************************************************************/
/*
 -  CAdviseNotifDlg
 -
 *  Purpose:
 */
/*********************************************************************/
#ifdef XVPORT

class CAdviseNotifDlg : public CModalDialog
{
public:   
    LPNOTIFS        *m_lppNotifs;
    CString         m_Description;
    LPUNKNOWN       m_lpUnk;
    ULONG           m_ulObjType;
    LPENTRYID       m_lpEntryID;
    ULONG           m_cbEntryID;
        
    CAdviseNotifDlg(CWnd  *pParentWnd)
        : CModalDialog(ADVISENOTIF, pParentWnd)
    {
        m_lppNotifs      = NULL;
        m_lpUnk          = NULL;
        m_ulObjType      = 0;
        m_lpEntryID      = NULL;
        m_cbEntryID      = 0;
    }

    ~CAdviseNotifDlg(); 

    BOOL OnInitDialog();
    void OnCancel();    
    void OnOK();
    
};

#endif



      
#endif  // mapitest defined __mdbview_h_

                        