/*******************************************************************/
/*
 -  mdbview.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Main source module for MDB Viewer, a MFC windows application
 *      that views properties of folders, messages, attachments, etc,
 *      inside a MAPI 1.0 compliant message store.  It has the ability
 *      to do several MAPI 1.0 message store, property, and table,
 *      operations.
 *      Currently this application works on both 32 bit NT and 16 bit
 *      windows 3.1.
 *
 *      Contains member functions for CTheApp, CMainWindow, Utility
 *      functions, CGetError, and global variables.
 *
 */
/*******************************************************************/

//#ifndef CHICAGO
//#define CTL3D
//#endif

#undef CTL3D

//#undef XVPORT
//#define XVPORT

#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>
#include <windowsx.h>
#include <string.h>

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


#ifdef WIN16
#include <mapiwin.h>
#endif
#include <mapix.h>
#include <mapiutil.h>   // for HrAllocAdviseSink

#ifdef CTL3D
#include <ctl3d.h>
#endif
#include <strtbl.h>
#include <misctool.h>
#include <pvalloc.h>
#include "resource.h"
#include "mdbview.h"
#include "fld.h"
#include "oper.h"

#ifdef XVPORT

#include <xvport.h>     // for notification logging port

extern "C"
{
NOTIFCALLBACK LogNotifToXVPLog;
}

#endif

typedef struct _NotifContext
{
    ULONG               ulCount;
    char                szComment[128];
} NOTIFCONTEXT,FAR *LPNOTIFCONTEXT;



/********************** Function decls for Tablevu dll function *****/


/* TBLVU functions */

extern  HINSTANCE    hlibTBLVU;

typedef BOOL (*LPFNVIEWMAPITABLE)(LPMAPITABLE FAR *lppMAPITable,HWND hWnd);

extern  LPFNVIEWMAPITABLE lpfnViewMapiTable;

#define TBLVUViewMapiTable (*lpfnViewMapiTable)


/* PROPVU functions */

extern HINSTANCE    hlibPROPVU;

typedef BOOL (*LPFNVIEWMAPIPROP)(
    LPSTR           lpszName,
    LPMAPIPROP FAR *lppMAPIProp,
    LPVOID          lpvDest,
    HWND            hWnd);

extern LPFNVIEWMAPIPROP lpfnViewMapiProp;

#define PROPVUViewMapiProp (*lpfnViewMapiProp)


/* STATVU functions */

extern HINSTANCE    hlibSTATVU;

typedef BOOL (*LPFNVIEWMAPISTAT)(
        LPMAPISESSION FAR *lppMAPISession,
        HWND hWnd);

extern LPFNVIEWMAPISTAT lpfnViewMapiStat;

#define STATVUViewMapiStat (*lpfnViewMapiStat)


/********************** Global Variables ****************************/

// windows application globals
CTheApp             theApp;                         // application framework
HINSTANCE           hInst               = NULL;     // handle of instance for this application

// mapi session/mdb/adrbook globals
LPMAPISESSION       lpSession           = NULL;     // global MAPI session handle
LPMDB               lpMDB               = NULL;     // global pointer to open message store
LPADRBOOK           lpAdrBook           = NULL;


//#ifdef XVPORT
// Notification structures
LPNOTIFS            lpMDBNotifs         = NULL;
LPNOTIFS            lpSessionNotifs     = NULL;
LPNOTIFS            lpStatusTblNotifs      = NULL;
LPNOTIFS            lpStatusObjsNotifs  = NULL;
LPNOTIFS            lpNotifStoresTable  = NULL;
// XVPort id number
UINT                xvpNum              = 0;
//#endif

LPMAPIPROP          lpIdentityObj       = NULL;
LPMAPITABLE         lpStoresTable       = NULL;
LPMAPITABLE         lpStatusTable       = NULL;
LPMAPITABLE         lpReceiveFolderTable = NULL;


// Used for deterimining desination of CopyTo, CopyMessages,CopyFolder operations
//CString             CopyToDest;
LPVOID              lpvCopyToDest       = NULL;




// library/dll instances
HINSTANCE           hlibTBLVU           = (HINSTANCE)NULL; // handle to library tablevu.dll
LPFNVIEWMAPITABLE   lpfnViewMapiTable   = NULL;            // pointer to function in tablevu.dll

HINSTANCE           hlibPROPVU          = (HINSTANCE)NULL;
LPFNVIEWMAPIPROP    lpfnViewMapiProp    = NULL;

HINSTANCE           hlibSTATVU          = (HINSTANCE)NULL;
LPFNVIEWMAPISTAT    lpfnViewMapiStat    = NULL;

// session global hresult used for lpSession->GetLastError
HRESULT             hResultSession      = hrSuccess;


// search folder entrylist of what folders to search
ULONG               cFolders                = 0;
LPENTRYLIST         lpEntryListSearchFolder = NULL;

// access privileges on all OpenEntry calls, set on Opening of MDB
ULONG               ulAccess            = MAPI_MODIFY;

/******************* CMainWindow Message Map ****************************/

BEGIN_MESSAGE_MAP( CMainWindow, CFrameWnd )

    ON_COMMAND( IDM_EXIT,           OnExit )
    ON_COMMAND( IDM_ABOUT,          OnAbout )
    ON_COMMAND( IDM_LOGON,          OnLogon )
    ON_COMMAND( IDM_LOGOFF,         OnLogoff )
    ON_COMMAND( IDM_OPENMDB,        OnOpenMDB )
    ON_COMMAND( IDM_GETREC,         OnGetReceiveFolder )
    ON_COMMAND( IDM_GETSTATUSTBL,   OnGetStatusTable )
    ON_COMMAND( IDM_GETRECFLDTBL,   OnGetReceiveFolderTable )
    ON_COMMAND( IDM_GETSTORESTBL,   OnGetStoresTable )
    ON_COMMAND( IDM_OPENROOT,       OnOpenRootFolder )
    ON_COMMAND( IDM_LOGOFFMDB,      OnStoreLogoff )
    ON_COMMAND( IDM_QUERYIDENTITY,  OnQueryIdentity )
    ON_COMMAND( IDM_QUERYDEFMSGOPTS,OnQueryDefaultMsgOptions )
    ON_COMMAND( IDM_ENUMADRTYPES,   OnEnumAdrTypes )
    ON_COMMAND( IDM_OPENSTATUS,     OnOpenStatusObj )
    ON_COMMAND( IDM_MDBPROPS,       OnMDBProps )
    ON_COMMAND( IDM_SUPPORTMASK,    OnMDBSupportMask )
    ON_COMMAND( IDM_ABPROPS,        OnABProps )
    ON_COMMAND( IDM_SESGETLASTERROR,OnGetLastError )
    ON_COMMAND( IDM_ADDRESS,        OnAddress )
    ON_COMMAND( IDM_CREATEONEOFF,   OnCreateOneOff )
    ON_COMMAND( IDM_QUERYDEFRECIPOPT, OnQueryDefRecipOpts )
    ON_COMMAND( IDM_RESOLVENAME,    OnResolveName )
    ON_COMMAND( IDM_REGSTATUS,      OnStatusRegNotif )
    ON_COMMAND( IDM_REGSESSION,     OnSessionRegNotif )
    ON_COMMAND( IDM_REGMDB,         OnMDBRegNotif )
    ON_COMMAND( IDM_REGTABLE,       OnTableRegNotif )
    ON_COMMAND( IDM_REGAB,          OnABRegNotif )
    ON_COMMAND( IDM_REGON,          OnNotifState )
    ON_COMMAND( IDM_IPMSUBTREE,     OnIPMSubtree )
    ON_COMMAND( IDM_IPMOUTBOX,      OnIPMOutbox )
    ON_COMMAND( IDM_IPMWASTEBASKET, OnIPMWasteBasket )
    ON_COMMAND( IDM_IPMSENTMAIL,    OnIPMSentMail )

    ON_WM_CLOSE()

END_MESSAGE_MAP()


/********************************************************************/
/********************** TOOLS ***************************************/

//#ifdef XVPORT

/********************************************************************/
/*
 -  UnadviseAll
 -
 *  Purpose:
 *      Call Unadvise notification for all Advises in list'
 *
 *
 *  Parameters:
 *
 */
/********************************************************************/

void UnadviseAll(   LPUNKNOWN       lpUnk,
                    ULONG           ulObjType,
                    LPSTR           lpszDescription,
                    LPNOTIFS FAR    *lppNotifs)
{
    HRESULT             hResult1    = hrSuccess;
    ULONG               idx;
    char                szBuff1[300];
    char                szBuff2[300];
    CGetError           E;

    if( (!lppNotifs) || !(*lppNotifs) )
        return;

    if( !lpUnk)
        return;

    for(idx = 0 ; idx < (*lppNotifs)->cConnect ; idx++)
    {
        switch(ulObjType)
        {
            case MAPI_STORE:
                hResult1 = ((LPMDB)lpUnk)->Unadvise((*lppNotifs)->Connect[idx].ulConnection);
                break;
            case MAPI_TABLE:  // defined in mdbview.h
                hResult1 = ((LPMAPITABLE)lpUnk)->Unadvise((*lppNotifs)->Connect[idx].ulConnection);
                break;
            case MAPI_SESSION:
            case MAPI_STATUS:
                hResult1 = ((LPMAPISESSION)lpUnk)->Unadvise((*lppNotifs)->Connect[idx].ulConnection);
                break;
            case MAPI_ADDRBOOK:
                hResult1 = ((LPADRBOOK)lpUnk)->Unadvise((*lppNotifs)->Connect[idx].ulConnection);
                break;
            default:
                wsprintf(szBuff1,"[Error] Can't Advise on Object Type == %s, Description == %s",
                        GetString("MAPIObjTypes",(LONG)ulObjType,szBuff2),lpszDescription );
                MessageBox(NULL, szBuff1, "Client", MBS_ERROR );
                break;
        }

        if(HR_FAILED(hResult1) )
        {
            wsprintf(szBuff1,"%s->Unadvise()",lpszDescription);
            MessageBox(NULL, E.SzError(szBuff1, hResult1),
                  "Client", MBS_ERROR );
        }
    }
    PvFree( *lppNotifs );
    lppNotifs = NULL;
}

/********************************************************************/
/*
 -  AdviseObj
 -
 *  Purpose:
 *      Add an Advise Notification to List of Notifications registered
 *      for this object.
 *
 *  Parameters:
 *     [in]     lpUnk       - pointer to object(type determined by ulObjType)
 *     [in]     ulObjType   - Object Type of lpUnk pointer
 *     [in]     ulEventType - notification events to fire upon
 *     [in]     lpfnNotifCallback - address of callback routine
 *     [in]     cbEntryID   - Count Bytes in EntryID(not used for Table)
 *     [in]     lpEntryID   - ponter to EntryID to Advise on
 *     [in]     lpszComment - Comment that goes into lpvContext
 *     [in]     lpvExplicitContext  - used to explicitly pass a lpvContext instead of LPNOTIFCONTEXT)
 *                            if using LPNOTIFCONTEXT to log to window,
 *                            pass in NULL for this parameter.  If != NULL,
 *                            use this param instead of LPNOTIFCONTEXT
 *     [in/out] lppNotifs   - Pointer to list of Advises already notified on this obj
 *
 */
/********************************************************************/

void    AdviseObj(  LPUNKNOWN       lpUnk,
                    ULONG           ulObjType,
                    LPNOTIFCALLBACK lpfnNotifCallback,
                    ULONG           ulEventType,
                    ULONG           cbEntryID,
                    LPENTRYID       lpEntryID,
                    LPSTR           lpszComment,
                    LPVOID          lpvExplicitContext,
                    LPNOTIFS FAR    *lppNotifs)
{
    CGetError           E;
    HRESULT             hResult1        = hrSuccess;
    LPMAPIADVISESINK    lpAdviseSink    = NULL;
    LPNOTIFCONTEXT      lpContext;
    ULONG               ulConnection    = 0;
    char                szBuff1[300];
    char                szBuff2[300];
    LPNOTIFS            lpNotifsTemp    = NULL;

    // setup lpvContext
    lpContext = (LPNOTIFCONTEXT) PvAlloc( sizeof(NOTIFCONTEXT) );
    lpContext->ulCount = 0;
    if(lpszComment)
        lstrcpy(lpContext->szComment,lpszComment);
    else
        lstrcpy(lpContext->szComment,"NULL");


    // if pass in an explicit context pointer use it instead of my construted LPNOTIFCONTEXT
    //   commonly the lpvExplicitContext is used on Redraws of listboxes and the
    //   this pointer of the dialog is passed in.
    if( !lpvExplicitContext)
    {
        hResult1 = HrAllocAdviseSink(   (LPNOTIFCALLBACK)       lpfnNotifCallback,
                                        (LPVOID)                lpContext,
                                        (LPMAPIADVISESINK FAR *) &lpAdviseSink);
    }
    else
    {
        hResult1 = HrAllocAdviseSink(   (LPNOTIFCALLBACK)       lpfnNotifCallback,
                                        (LPVOID)                lpvExplicitContext,
                                        (LPMAPIADVISESINK FAR *) &lpAdviseSink);
    }

    if(HR_FAILED(hResult1) )
    {
#ifdef XVPORT
            XVPLog(xvpNum,0,E.SzError("HrAllocAdviseSink()", hResult1));
#else
        MessageBox( NULL, E.SzError("HrAllocAdviseSink()", hResult1),
                     "Client", MBS_ERROR );
#endif
        return;
    }


    switch(ulObjType)
    {
        case MAPI_STORE:
            hResult1 = ((LPMDB)lpUnk)->Advise(
                cbEntryID,
                lpEntryID,
                ulEventType,
                lpAdviseSink,
                &ulConnection);
            break;
        case MAPI_TABLE:  // defined in mdbview.h
            hResult1 = ((LPMAPITABLE)lpUnk)->Advise(
                ulEventType,
                lpAdviseSink,
                &ulConnection);
            break;
        case MAPI_SESSION:
        case MAPI_STATUS:
            hResult1 = ((LPMAPISESSION)lpUnk)->Advise(
                cbEntryID,
                lpEntryID,
                ulEventType,
                lpAdviseSink,
                &ulConnection);
            break;
        case MAPI_ADDRBOOK:
            hResult1 = ((LPADRBOOK)lpUnk)->Advise(
                cbEntryID,
                lpEntryID,
                ulEventType,
                lpAdviseSink,
                &ulConnection);
            break;
        default:
            wsprintf(szBuff1,"[Error] Can't Advise on Object Type == %s",
                    GetString("MAPIObjTypes",(LONG)ulObjType,szBuff2) );
#ifdef XVPORT
            XVPLog(xvpNum,0,szBuff1);
#else
            MessageBox( NULL, szBuff1, "Client", MBS_ERROR );
#endif
            break;
    }
    if( HR_FAILED(hResult1 ) )
    {
        if(lpszComment)
        {
            wsprintf(szBuff1,"%s->Advise()",lpszComment);

#ifdef XVPORT
            XVPLog(xvpNum,0,E.SzError(szBuff1, hResult1));
#else
            MessageBox(  NULL, E.SzError(szBuff1, hResult1), "Client", MBS_ERROR );
#endif
        }
        else
        {
#ifdef XVPORT
            XVPLog(xvpNum,0,E.SzError("lpObj->Advise()", hResult1));
#else
            MessageBox(  NULL, E.SzError("lpObj->Advise()", hResult1), "Client", MBS_ERROR );
#endif
        }        
        return;
    }



    // if no notif list is currently allocated, allocate it now
    if(! (*lppNotifs) )
    {
        lpNotifsTemp  = (LPNOTIFS) PvAlloc(sizeof(NOTIFS) );
        *lppNotifs    = lpNotifsTemp;
        (*lppNotifs)->cConnect = 0;
    }



    (*lppNotifs)->cConnect++;

    // now add to my list
    (*lppNotifs)->Connect[(*lppNotifs)->cConnect - 1].ulConnection = ulConnection;
    if(lpszComment)
        lstrcpy((*lppNotifs)->Connect[(*lppNotifs)->cConnect - 1].szContext,lpszComment);
    else
        lstrcpy((*lppNotifs)->Connect[(*lppNotifs)->cConnect - 1].szContext,"NULL");

    if(lpAdviseSink)
        lpAdviseSink->Release();


//$ FUTURE NEED TO FREE THIS MEMORY FOR LPCONTEXT !!!!

//    if(lpContext)
//        PvFree(lpContext);

}

#ifdef XVPORT

/********************************************************************/
/*
 -  AdviseAllStatusObjects
 -
 *  Purpose:
 *      Advise notification for status object modified on all
 *      current status objects
 *
 *
 *  Parameters:
 *
 */
/********************************************************************/

void AdviseAllStatusObjects(void)
{
    CGetError       E;
    HRESULT         hResult1    = hrSuccess;
    LPSRowSet       lpRows      = NULL;
    LONG            lRowsSeeked = 0;
    ULONG           ulRowCount  = 0;
    ULONG           iRow;
    LPSPropTagArray lpsptaAll   = NULL;
    char            szBuffer[COMMENT_SIZE];

    SizedSPropTagArray(2,sptaDisplay) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };

    // if anything is already advised in status table, unadvise them
    UnadviseAll( lpSession,MAPI_SESSION,"Status Objects lpSession",&lpStatusObjsNotifs);

    // then determine which objets have a status table row and advise them.

    hResult1 = lpStatusTable->GetRowCount(0,&ulRowCount);
    if( HR_FAILED(hResult1))
    {
        MessageBox(  NULL, E.SzError("lpStatusTable->GetRowCount()", hResult1), "Client", MBS_ERROR );
        goto Error;
    }

    if( !ulRowCount)
        return;

    // set which columns are important to see in table
    hResult1 = lpStatusTable->SetColumns( (LPSPropTagArray) &sptaDisplay, 0);
    if( HR_FAILED(hResult1))
    {
        MessageBox(  NULL, E.SzError("lpStatusTable->SetColumns", hResult1),
                            "Client", MBS_ERROR );
        goto Error;
    }

    hResult1 = lpStatusTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);
    if( HR_FAILED(hResult1))
    {
        MessageBox(  NULL, E.SzError("lpStatusTable->SeekRow", hResult1),
                            "Client", MBS_ERROR );
        goto Error;
    }

    hResult1 = lpStatusTable->QueryRows( ulRowCount, 0, &lpRows );

    if( GetScode(hResult1) == S_OK )
    {
        for(iRow = 0; iRow < lpRows->cRows; iRow++)
        {
            wsprintf(szBuffer,"fnevStatusObjectModified, Status Object %s, lpSession",
                            lpRows->aRow[iRow].lpProps[0].Value.lpszA);
            AdviseObj(  lpSession,
                        MAPI_SESSION,
                        LogNotifToXVPLog,
                        fnevStatusObjectModified,
                        (ULONG)     lpRows->aRow[iRow].lpProps[1].Value.bin.cb,
                        (LPENTRYID) lpRows->aRow[iRow].lpProps[1].Value.bin.lpb,
                        szBuffer,
                        NULL,
                        &lpStatusObjsNotifs);
        }
    }

Error:

    // query columns all columns
    hResult1 = lpStatusTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll);
    if( HR_FAILED(hResult1))
    {
        MessageBox(  NULL, E.SzError("lpStatusTable->QueryColumns(TBL_ALL_COLUMNS)", hResult1),
                            "Client", MBS_ERROR );
    }

    // set columns all columns to get back to original state
    hResult1 = lpStatusTable->SetColumns( (LPSPropTagArray) lpsptaAll, 0);
    if( HR_FAILED(hResult1))
    {
        MessageBox(  NULL, E.SzError("lpStatusTable->SetColumns", hResult1),
                            "Client", MBS_ERROR );
    }

    // free Memory allocated
    FreeRowSet(lpRows);

    if(lpsptaAll)
        MAPIFreeBuffer(lpsptaAll);

    // NOTE: it should fire two notifications on table modified for status table
}

#endif // XVPORT


/********************************************************************/
/*
 -  FreeAdrList
 -
 *  Purpose:
 *      Given an Address List, will free it.
 *
 *  Parameters:
 *      LPADRLIST - Pointer to Address List to free.
 *
 */
/********************************************************************/

void FreeAdrList( LPADRLIST lpadrlist )
{
    ULONG cEntries = 0;

    if (lpadrlist)
    {
        for (cEntries = 0; cEntries < lpadrlist->cEntries; cEntries++)
        {
            if(lpadrlist->aEntries[cEntries].rgPropVals)
                MAPIFreeBuffer(lpadrlist->aEntries[cEntries].rgPropVals);
        }

        MAPIFreeBuffer(lpadrlist);
    }
}


/********************************************************************/
/********************** CMainWindow Member Functions ****************/

/******************************************************************/
/*
 -  CMainWindow::
 -  OnAbout
 -
 *  Purpose:
 *      We create a CModalDialog object using the "AboutBox" resource
 *      (see mdbview.rc), and invoke it.  Describes what this application
 *      is, and what version it is.
 */
/*******************************************************************/

void CMainWindow::OnAbout()
{
    CModalDialog about( "AboutBox", this );
    about.DoModal();
}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnLogon
 *
 *  Purpose:
 *      Have user logon to MAPI 1.0 to obtain a session handle for MAPI
 *      user.  lpSession is a global variable used to reference the
 *      session this application has logged onto.
 */
/*******************************************************************/

void CMainWindow::OnLogon()
{
    CGetError   E;
    ULONG       ulResult    = S_OK;
    COperation  LogonDlg(this);
    char        szBuff[80];
    ULONG       ulUIParam   = 0;
    ULONG       ulFlags     = 0;
    DWORD       dRet        = 0;
    HRESULT             hResult1        = hrSuccess;
//    LPMAPIADVISESINK    lpAdviseSink    = NULL;
//    LPNOTIFCONTEXT      lpContext;
//    ULONG               ulConnection    = 0;

    CMenu           *menu = GetMenu();

    if(lpSession)
    {
        lpSession->Release();
        lpSession = NULL;
    }

    menu->EnableMenuItem( IDM_EXIT,             MF_GRAYED  );
    menu->EnableMenuItem( IDM_LOGON,            MF_GRAYED  );

    LogonDlg.m_CurrentOperation= "MAPILogon(MAPI_LOGON_UI | MAPI_EXTENDED)";
    LogonDlg.m_CBText1         = "ulUIParam:";
    LogonDlg.m_FlagText1       = "MAPI_FORCE_DOWNLOAD";
    LogonDlg.m_FlagText2       = "MAPI_NEW_SESSION";
    LogonDlg.m_FlagText3       = "MAPI_ALLOW_OTHERS";
    LogonDlg.m_FlagText4       = "MAPI_EXPLICIT_PROFILE";
    LogonDlg.m_FlagText5       = "MAPI_NO_MAIL";
    LogonDlg.m_FlagText6       = "Invalid Flag";

    dRet = LogonDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = LogonDlg.m_CBContents1.Add(szBuff);


    // bring up modal dialog box, and if user hits OK, process operation
    if( LogonDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing

        if( !lstrcmp(LogonDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        ulFlags = MAPI_EXTENDED | MAPI_LOGON_UI;

        if( LogonDlg.m_bFlag1 )
            ulFlags |= MAPI_FORCE_DOWNLOAD;

        if( LogonDlg.m_bFlag2 )
            ulFlags |= MAPI_NEW_SESSION;

        if( LogonDlg.m_bFlag3 )
            ulFlags |= MAPI_ALLOW_OTHERS;

        if( LogonDlg.m_bFlag4 )
            ulFlags |= MAPI_EXPLICIT_PROFILE;

        if( LogonDlg.m_bFlag5)
            ulFlags |= MAPI_NO_MAIL;

        if( LogonDlg.m_bFlag6)
            ulFlags |= TEST_INVALID_FLAG;


        ulResult = MAPILogon(   ulUIParam,
                                NULL,
                                NULL,
                                ulFlags,
                                0,
                                (LHANDLE *) &lpSession);
        if( ulResult != S_OK)
        {
            MessageBox( E.SzError("MAPILogon", ulResult), "Client", MBS_ERROR );
//          menu->EnableMenuItem( IDM_LOGON,            MF_ENABLED   );
            menu->EnableMenuItem( IDM_EXIT,             MF_ENABLED  );
            goto Error;
        }

//      menu->EnableMenuItem( IDM_LOGON,            MF_GRAYED   );
        menu->EnableMenuItem( IDM_EXIT,             MF_ENABLED  );

        // open address book to lpSession->QueryIdentity() entryid
        //   can be resolved as a oneoff, and for lpMessage->ModifyRecipients();


        hResultSession = lpSession->OpenAddressBook( NULL,
                                NULL,
                                0,
                                &lpAdrBook);
        if( HR_FAILED(hResultSession ) )
        {
            MessageBox( E.SzError("lpSession->OpenAddressBook()", hResultSession), "Client", MBS_ERROR );
            goto Error;
        }



        hResultSession = lpSession->GetStatusTable(0, &lpStatusTable);
        if(HR_FAILED(hResultSession) )
        {
            MessageBox( E.SzError("lpSession->GetStatusTable()", hResultSession),
                     "Client", MBS_ERROR );
            goto Error;
        }

        // ADVISE STATUS TABLE NOTIFICATON
/*
        AdviseObj(  lpStatusTable,
                    MAPI_TABLE,
                    LogNotifToXVPLog,
                    fnevTableModified,
                    0,
                    NULL,
                    "fnevTableModified, lpStatusTable, lpTable",
                    NULL,
                    &lpStatusTblNotifs);

*/


        hResultSession = lpSession->GetMsgStoresTable(0, &lpStoresTable);

        if(HR_FAILED(hResultSession) )
        {
            MessageBox( E.SzError("lpSession->GetMsgStoresTable()", hResultSession),
                     "Client", MBS_ERROR );
            return;
        }


#ifdef XVPORT

        AdviseObj(  lpStoresTable,
                    MAPI_TABLE,
                    LogNotifToXVPLog,
                    fnevTableModified,
                    0,
                    NULL,
                    "fnevTableModified, lpSession->GetMsgStoresTable, lpSession",
                    NULL,
                    &lpNotifStoresTable);

        // if logon successfully, allow opening of store, etc.

        XVPShow(xvpNum, XVP_SHOW);
        XVPShow(xvpNum, XVP_MIN);
        XVPReset(xvpNum);
#endif

        ToggleMenu( TRUE );

        return;
    }

Error:

#ifdef XVPORT
    UnadviseAll(lpStatusTable,  MAPI_TABLE,     "Status Table lpSession",    &lpStatusTblNotifs);
    UnadviseAll(lpSession,      MAPI_SESSION,   "Session Objs lpSession",    &lpSessionNotifs);
    UnadviseAll(lpStoresTable,  MAPI_TABLE,     "GetMsgStoresTable, lpTable",&lpNotifStoresTable);
#endif

    // call OnLogoff

    if(lpSession)
    {
        lpSession->Release();
        lpSession = NULL;
    }

    if(lpAdrBook)
    {
        lpAdrBook->Release();
        lpAdrBook = NULL;
    }

    if(lpStatusTable)
    {
        lpStatusTable->Release();
        lpStatusTable = NULL;
    }

    if(lpEntryListSearchFolder)
    {
        PvFree(lpEntryListSearchFolder);
        lpEntryListSearchFolder = NULL;
    }

    if(lpStoresTable)
    {
        lpStoresTable->Release();
        lpStoresTable = NULL;
    }

    ToggleMenu( FALSE );

#ifdef XVPORT
    XVPShow(xvpNum, XVP_HIDE);
    XVPReset(xvpNum);
#endif

}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnLogoff
 *
 *  Purpose:
 *      Logoff From MAPI session.   lpSession is a global variable
 *      used to reference the session this application has logged onto.
 *
 */
/*******************************************************************/

void CMainWindow::OnLogoff()
{
    CGetError   E;

    if(lpSession)
    {

#ifdef XVPORT
        UnadviseAll(lpStatusTable,  MAPI_TABLE,     "Status Table lpSession",   &lpStatusTblNotifs);
        UnadviseAll(lpSession,      MAPI_SESSION,   "Session Objs lpSession",   &lpSessionNotifs);
        UnadviseAll(lpMDB,          MAPI_STORE,     "MDB Objs lpMDB",           &lpMDBNotifs);
        UnadviseAll(lpStoresTable,  MAPI_TABLE,     "GetMsgStoresTable, lpTable",&lpNotifStoresTable);
#endif

        if(lpIdentityObj)
        {
            lpIdentityObj->Release();
            lpIdentityObj = NULL;
        }

        if(lpStoresTable)
        {
            lpStoresTable->Release();
            lpStoresTable = NULL;
        }

        if(lpReceiveFolderTable)
        {
            lpReceiveFolderTable->Release();
            lpReceiveFolderTable = NULL;
        }

        if(lpStatusTable)
        {
            lpStatusTable->Release();
            lpStatusTable = NULL;
        }

        if(lpAdrBook)
        {
            lpAdrBook->Release();
            lpAdrBook = NULL;
        }

        if(lpMDB)
            OnStoreLogoff();

        hResultSession = lpSession->Logoff(
                    (ULONG)(void *)m_hWnd,
                    MAPI_LOGOFF_UI,
                    0 );
        if( HR_FAILED(hResultSession) )
        {
            MessageBox( E.SzError("Logoff", hResultSession), "Client", MBS_ERROR );
            return;
        }

        if(lpSession)
        {
            lpSession->Release();
            lpSession = NULL;
        }

        ToggleMenu( FALSE );
    }

    if(lpEntryListSearchFolder)
    {
        PvFree(lpEntryListSearchFolder);
        lpEntryListSearchFolder = NULL;
    }

#ifdef XVPORT
    XVPShow(xvpNum,XVP_HIDE);
    XVPReset(xvpNum);
#endif
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnMDBSupportMask
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnMDBSupportMask()
{
    CStoreSupportMaskDlg    Support(this);
    Support.DoModal();
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnIPMSubtree
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnIPMSubtree()
{
    CFolderDlg      *lpIPMSubtreeFld        = NULL;
    HRESULT         hResult1                = hrSuccess;
    CGetError       E;
    LPMAPIFOLDER        lpFolder                = NULL;
    ULONG           ulObjType               = 0;
    LPSPropValue    lpspva                  = NULL;
    ULONG           cValues                 = 0;
    char            szBuffer[80];

    // proptag array used by GetProps for getting entryid of IPM subtree
    SPropTagArray   sptaIPMSubtree =
    {
        1,
        {
            PR_IPM_SUBTREE_ENTRYID
        }
    };


    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->GetProps( &sptaIPMSubtree,0,&cValues,&lpspva );
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->GetProps() PR_IPM_SUBTREE_ENTRYID",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }

    if( ! (lpspva[0].ulPropTag == PR_IPM_SUBTREE_ENTRYID) )
    {
        MessageBox( "Message Store has no valid PR_IPM_SUBTREE_ENTRYID" ,"Client", MBS_ERROR );
        goto Error;
    }

    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->OpenEntry(    (ULONG)     lpspva[0].Value.bin.cb,
                                    (LPENTRYID) lpspva[0].Value.bin.lpb,
                                    NULL,
                                    ulAccess,
                                    &ulObjType,
                                    (LPUNKNOWN*)(LPMAPIPROP *) &lpFolder);
    if( HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->OpenEntry() IPM SUBTREE",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }
    if( ulObjType == MAPI_FOLDER )
    {
        wsprintf(szBuffer,"%s - %s", GetString("MAPIObjTypes",ulObjType,NULL),"IPM_SUBTREE");
        lpIPMSubtreeFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpFolder,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpFolder != MAPI_FOLDER","Client",MBS_ERROR);
        goto Error;
    }

Error:

    if(lpspva)
        MAPIFreeBuffer(lpspva);

}


/********************************************************************/
/*
 -  CMainWindow::
 -  OnIPMOutbox
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnIPMOutbox()
{
    CFolderDlg      *lpIPMOutboxFld        = NULL;
    HRESULT         hResult1                = hrSuccess;
    CGetError       E;
    LPMAPIFOLDER        lpFolder                = NULL;
    ULONG           ulObjType               = 0;
    LPSPropValue    lpspva                  = NULL;
    ULONG           cValues                 = 0;
    char            szBuffer[80];


    // proptag array used by GetProps for getting entryid of IPM subtree
    SPropTagArray   sptaIPMOutbox =
    {
        1,
        {
            PR_IPM_OUTBOX_ENTRYID
        }
    };

    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->GetProps( &sptaIPMOutbox,0,&cValues,&lpspva );
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->GetProps() PR_IPM_OUTBOX_ENTRYID",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }

    if( ! (lpspva[0].ulPropTag == PR_IPM_OUTBOX_ENTRYID) )
    {
        MessageBox( "Message Store has no valid PR_IPM_OUTBOX_ENTRYID" ,"Client", MBS_ERROR );
        goto Error;
    }

    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->OpenEntry(    (ULONG)     lpspva[0].Value.bin.cb,
                                        (LPENTRYID) lpspva[0].Value.bin.lpb,
                                        NULL,
                                        ulAccess,
                                        &ulObjType,
                                        (LPUNKNOWN*)(LPMAPIPROP *) &lpFolder);
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->OpenEntry() IPM OUTBOX",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }
    if( ulObjType == MAPI_FOLDER )
    {
        wsprintf(szBuffer,"%s - %s", GetString("MAPIObjTypes",ulObjType,NULL),"IPM_OUTBOX");
        lpIPMOutboxFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpFolder,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpFolder != MAPI_FOLDER","Client",MBS_ERROR);
        goto Error;
    }

Error:

    if(lpspva)
        MAPIFreeBuffer(lpspva);

}


/********************************************************************/
/*
 -  CMainWindow::
 -  OnIPMWasteBasket
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnIPMWasteBasket()
{
    CFolderDlg      *lpIPMWasteBasketFld        = NULL;
    HRESULT         hResult1                = hrSuccess;
    CGetError       E;
    ULONG           ulEntryID               = 0;
    LPENTRYID       lpEntryID               = NULL;
    LPMAPIFOLDER        lpFolder                = NULL;
    ULONG           ulObjType               = 0;
    LPSPropValue    lpspva                  = NULL;
    ULONG           cValues                 = 0;
    char            szBuffer[80];

    // proptag array used by GetProps for getting entryid of IPM subtree
    SPropTagArray   sptaIPMWasteBasket =
    {
        1,
        {
            PR_IPM_WASTEBASKET_ENTRYID
        }
    };

    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->GetProps( &sptaIPMWasteBasket,0,&cValues,&lpspva );
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->GetProps() PR_IPM_WASTEBASKET_ENTRYID",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }

    if( ! (lpspva[0].ulPropTag == PR_IPM_WASTEBASKET_ENTRYID) )
    {
        MessageBox( "Message Store has no valid PR_IPM_WASTEBASKET_ENTRYID" ,"Client", MBS_ERROR );
        goto Error;
    }

    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->OpenEntry(    (ULONG)     lpspva[0].Value.bin.cb,
                                        (LPENTRYID) lpspva[0].Value.bin.lpb,
                                        NULL,
                                        ulAccess,
                                        &ulObjType,
                                        (LPUNKNOWN*)(LPMAPIPROP *) &lpFolder);
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->OpenEntry() IPM WASTEBASKET",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }
    if( ulObjType == MAPI_FOLDER )
    {
        wsprintf(szBuffer,"%s - %s", GetString("MAPIObjTypes",ulObjType,NULL),"IPM_WASTEBASKET");
        lpIPMWasteBasketFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpFolder,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpFolder != MAPI_FOLDER","Client",MBS_ERROR);
        goto Error;
    }

Error:

    if(lpspva)
        MAPIFreeBuffer(lpspva);

}


/********************************************************************/
/*
 -  CMainWindow::
 -  OnIPMSentMail
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnIPMSentMail()
{
    CFolderDlg      *lpIPMSentMailFld       = NULL;
    HRESULT         hResult1                = hrSuccess;
    CGetError       E;
    ULONG           ulEntryID               = 0;
    LPENTRYID       lpEntryID               = NULL;
    LPMAPIFOLDER        lpFolder                = NULL;
    ULONG           ulObjType               = 0;
    LPSPropValue    lpspva                  = NULL;
    ULONG           cValues                 = 0;
    char            szBuffer[80];

    // proptag array used by GetProps for getting entryid of IPM subtree
    SPropTagArray   sptaIPMSentMail =
    {
        1,
        {
            PR_IPM_SENTMAIL_ENTRYID
        }
    };


    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->GetProps( &sptaIPMSentMail,0,&cValues,&lpspva );
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->GetProps() PR_IPM_SENTMAIL_ENTRYID",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }

    if( ! (lpspva[0].ulPropTag == PR_IPM_SENTMAIL_ENTRYID) )
    {
        MessageBox( "Message Store has no valid PR_IPM_SENTMAIL_ENTRYID" ,"Client", MBS_ERROR );
        goto Error;
    }

    // open up the object described, determine if folder, and open folder dlg
    hResult1 = lpMDB->OpenEntry(    (ULONG)     lpspva[0].Value.bin.cb,
                                        (LPENTRYID) lpspva[0].Value.bin.lpb,
                                        NULL,
                                        ulAccess,
                                        &ulObjType,
                                        (LPUNKNOWN*)(LPMAPIPROP *) &lpFolder);
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->OpenEntry() IPM SENTMAIL",hResult1),
                        "Client", MBS_ERROR );
        goto Error;
    }
    if( ulObjType == MAPI_FOLDER )
    {
        wsprintf(szBuffer,"%s - %s", GetString("MAPIObjTypes",ulObjType,NULL),"IPM_SENTMAIL");
        lpIPMSentMailFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpFolder,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpFolder != MAPI_FOLDER","Client",MBS_ERROR);
        goto Error;
    }

Error:

    if(lpspva)
        MAPIFreeBuffer(lpspva);

}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnOpenStatusObj
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnOpenStatusObj()
{
    // ADVISE ALL STATUS OBJECTS
#ifdef XVPORT
    AdviseAllStatusObjects();

#endif
    STATVUViewMapiStat(
                (LPMAPISESSION FAR *)&lpSession, (HWND)this->m_hWnd );
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnGetLastError
 -
    hResultSession is Global(Session Objects Only)

 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnGetLastError()
{
    COperation      GetLastErrorDlg(this);
    ULONG           ulFlags         = 0;
    CGetError       E;
    HRESULT         hResult1        = hrSuccess;
    char            szResult[80];
    char            szBuffer[128];
    LPMAPIERROR     lpMAPIError     = NULL;

    GetLastErrorDlg.m_CurrentOperation= "lpSession->GetLastError()";
    GetLastErrorDlg.m_EditText1       = "hResultSession(HEX)";
    GetLastErrorDlg.m_EditText2       = "hResultSession(STRING):";
    GetLastErrorDlg.m_FlagText1       = "MAPI_UNICODE";
    GetLastErrorDlg.m_FlagText2       = "Invalid Flag";

    wsprintf( szResult, "0x%08X", hResultSession );
    GetLastErrorDlg.m_EditDefault1    = szResult;

    if(!GetString( "MAPIErrors", GetScode(hResultSession), szResult ))
        wsprintf( szResult, "0x%08X", hResultSession );
    GetLastErrorDlg.m_EditDefault2    = szResult;

    if( GetLastErrorDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( GetLastErrorDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        if( GetLastErrorDlg.m_bFlag1 )
        {
            ulFlags |= MAPI_UNICODE;

            hResult1 = lpSession->GetLastError(
                            hResultSession,
                            ulFlags,
                            &lpMAPIError );

            if(HR_FAILED(hResult1) )
            {
                MessageBox( E.SzError("lpSession->GetLastError()",
                             hResultSession),"Client", MBS_ERROR );
                return;
            }

            if(lpMAPIError)
            {
                // CONVERT TO STRING 8 then display !!!
                wsprintf(szBuffer,"lpObj->GetLastError()  ulLowLevelError == %lu,ulVersion == %lu lpszMessage == %s, lpszComponent == %s, ulContext == %lu",
                    lpMAPIError->ulLowLevelError,
                    lpMAPIError->ulVersion,
                    ((lpMAPIError->lpszError == NULL)     ? "NULL" : lpMAPIError->lpszError),
                    ((lpMAPIError->lpszComponent == NULL) ? "NULL" : lpMAPIError->lpszComponent),
                    lpMAPIError->ulContext);

                MessageBox( szBuffer, "Client", MBS_INFO );
  
            }
        }          
        else
        {

            hResult1 = lpSession->GetLastError(
                            hResultSession,
                            ulFlags,
                            &lpMAPIError );

            if(HR_FAILED(hResult1) )
            {
                MessageBox( E.SzError("lpSession->GetLastError()",
                             hResultSession),"Client", MBS_ERROR );
                return;
            }

            if(lpMAPIError)
            {
                // CONVERT TO STRING 8 then display !!!
                wsprintf(szBuffer,"lpObj->GetLastError()  ulLowLevelError == %lu,ulVersion == %lu lpszMessage == %s, lpszComponent == %s, ulContext == %lu",
                    lpMAPIError->ulLowLevelError,
                    lpMAPIError->ulVersion,
                    ((lpMAPIError->lpszError == NULL)     ? "NULL" : lpMAPIError->lpszError),
                    ((lpMAPIError->lpszComponent == NULL) ? "NULL" : lpMAPIError->lpszComponent),
                    lpMAPIError->ulContext);

                MessageBox( szBuffer, "Client", MBS_INFO );
            }
        }

        if(lpMAPIError)
            MAPIFreeBuffer(lpMAPIError);

        // reset global hResultSession
        hResultSession = hrSuccess;
    }
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnAddress
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnAddress()
{
    HRESULT         hResult1         = hrSuccess;
    LPSTR           rglpszDestTitles[3];
    ULONG           rgulDestComps[3];
    ADRPARM         adrparm         = { 0 };
    LPADRLIST       lpAdrList       = NULL;
    ULONG           ulUIParam       = (ULONG)(void *)m_hWnd;
    CGetError       E;
    CAdrListDlg     AdrListDlg(this);

    // bring up UI for determining what the ADDRESS LIST will look like

    rglpszDestTitles[0] = "To:";
    rglpszDestTitles[1] = "Cc:";
    rglpszDestTitles[2] = "Bcc:";

    rgulDestComps[0]    = MAPI_TO;
    rgulDestComps[1]    = MAPI_CC;
    rgulDestComps[2]    = MAPI_BCC;

    adrparm.cbABContEntryID     = 0;
    adrparm.lpABContEntryID     = NULL;
    adrparm.ulFlags             = DIALOG_MODAL;
    adrparm.lpReserved          = NULL;
    adrparm.ulHelpContext       = 0;
    adrparm.lpszHelpFileName    = NULL;
    adrparm.lpfnABSDI           = NULL;
    adrparm.lpfnDismiss         = NULL;
    adrparm.lpvDismissContext   = NULL;
    adrparm.lpszCaption         = "AddressBook";
    adrparm.lpszNewEntryTitle   = "MDB Viewer New Entry";
    adrparm.lpszDestWellsTitle  = "Destinations";
    adrparm.cDestFields         = 3;
    adrparm.nDestFieldFocus     = 0;
    adrparm.lppszDestTitles     = rglpszDestTitles;
    adrparm.lpulDestComps       = rgulDestComps;
    adrparm.lpContRestriction   = NULL;
    adrparm.lpHierRestriction   = NULL;
    adrparm.cDestFields         = (ULONG) 3;

    hResult1 = lpAdrBook->Address(
                    &ulUIParam,
                    &adrparm,
                    &lpAdrList );

    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpAdrBook->Address()", hResult1), "Client", MB_OK );
    }



    AdrListDlg.m_List       = "Address List Built with Address()";
    AdrListDlg.m_lpAdrList  = lpAdrList;

    AdrListDlg.DoModal();

    if(lpAdrList)
        MAPIFreeBuffer(lpAdrList);
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnCreateOneOff
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnCreateOneOff()
{
    CPropDisplayDlg DisplayEID(this);
    COperation      CreateOneOffDlg(this);
    LPWSTR          lpNewBuffer1    = NULL;
    LPWSTR          lpNewBuffer2    = NULL;
    LPWSTR          lpNewBuffer3    = NULL;
    ULONG           ulFlags         = 0;
    ULONG           ulEntryID       = 0;
    LPENTRYID       lpEntryID       = 0;
    CGetError       E;
    HRESULT         hResult1        = hrSuccess;

    CreateOneOffDlg.m_CurrentOperation= "lpAdrBook->CreateOneOff()";
    CreateOneOffDlg.m_EditText1       = "lpszName:";
    CreateOneOffDlg.m_EditText2       = "lpszAdrType:";
    CreateOneOffDlg.m_EditText3       = "lpszAddress:";
    CreateOneOffDlg.m_FlagText1       = "MAPI_UNICODE";
    CreateOneOffDlg.m_FlagText2       = "Invalid Flag";
    CreateOneOffDlg.m_EditDefault1    = "Your Name";
    CreateOneOffDlg.m_EditDefault2    = "MSPEER";
    CreateOneOffDlg.m_EditDefault3    = "Your Address";

    if( CreateOneOffDlg.DoModal() == IDOK )
    {
        if( CreateOneOffDlg.m_bFlag2)
            ulFlags |= TEST_INVALID_FLAG;

        if( CreateOneOffDlg.m_bFlag1 )
        {
            ulFlags |= MAPI_UNICODE;

            String8ToUnicode(CreateOneOffDlg.m_szEdit1, &lpNewBuffer1, NULL);
            String8ToUnicode(CreateOneOffDlg.m_szEdit2, &lpNewBuffer2, NULL);
            String8ToUnicode(CreateOneOffDlg.m_szEdit3, &lpNewBuffer3, NULL);

            hResult1 = lpAdrBook->CreateOneOff(
                        (LPTSTR) lpNewBuffer1,
                        (LPTSTR) lpNewBuffer2,
                        (LPTSTR) lpNewBuffer3,
                        ulFlags,
                        &ulEntryID,
                        &lpEntryID);
            if( HR_FAILED(hResult1))
            {
                MessageBox( E.SzError("lpAdrBook->CreateOneOff()", hResult1),
                            "Client", MBS_ERROR );
                PvFree(lpNewBuffer1);
                PvFree(lpNewBuffer2);
                PvFree(lpNewBuffer3);
                return;
            }

            PvFree(lpNewBuffer1);
            PvFree(lpNewBuffer2);
            PvFree(lpNewBuffer3);
        }
        else
        {

            hResult1 = lpAdrBook->CreateOneOff(
                        (LPTSTR) CreateOneOffDlg.m_szEdit1,
                        (LPTSTR) CreateOneOffDlg.m_szEdit2,
                        (LPTSTR) CreateOneOffDlg.m_szEdit3,
                        ulFlags,
                        &ulEntryID,
                        &lpEntryID);
            if( HR_FAILED(hResult1))
            {
                MessageBox( E.SzError("lpAdrBook->CreateOneOff()", hResult1),
                                "Client", MBS_ERROR );
                return;
            }
        }

        if(lpEntryID)
        {
            DisplayEID.m_ulValues   = ulEntryID;
            DisplayEID.m_lpEntryID  = lpEntryID;
            DisplayEID.DoModal();

            MAPIFreeBuffer(lpEntryID);
        }
    }
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnQueryDefRecipOpts
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnQueryDefRecipOpts()
{
    CGetError       E;
    CPropDisplayDlg DisplayProps(this);
    COperation      QueryDefaultRecipOptDlg(this);
    LPWSTR          lpNewBuffer1    = NULL;
    ULONG           ulFlags         = 0;
    ULONG           ulValues        = 0;
    LPSPropValue    lpspva          = 0;
    HRESULT         hResult1        = hrSuccess;

    QueryDefaultRecipOptDlg.m_CurrentOperation= "lpAdrBook->QueryDefaultRecipOpt()";
    QueryDefaultRecipOptDlg.m_EditText1       = "lpszAdrType:";
    QueryDefaultRecipOptDlg.m_FlagText1       = "MAPI_UNICODE";
    QueryDefaultRecipOptDlg.m_FlagText2       = "Invalid Flag";
    QueryDefaultRecipOptDlg.m_EditDefault1    = "MSPEER";

    if( QueryDefaultRecipOptDlg.DoModal() == IDOK )
    {
        if( QueryDefaultRecipOptDlg.m_bFlag2)
            ulFlags |= TEST_INVALID_FLAG;

        if( QueryDefaultRecipOptDlg.m_bFlag1 )
        {
            ulFlags |= MAPI_UNICODE;

            String8ToUnicode(QueryDefaultRecipOptDlg.m_szEdit1, &lpNewBuffer1, NULL);

            hResult1 = lpAdrBook->QueryDefaultRecipOpt(
                        (LPTSTR) lpNewBuffer1,
                        ulFlags,
                        &ulValues,
                        &lpspva);

            if( HR_FAILED(hResult1))
            {
                MessageBox( E.SzError("lpAdrBook->QueryDefaultRecipOpt()", hResult1),
                            "Client", MBS_ERROR );
                PvFree(lpNewBuffer1);
                return;
            }

            PvFree(lpNewBuffer1);
        }
        else
        {

            hResult1 = lpAdrBook->QueryDefaultRecipOpt(
                        (LPTSTR) QueryDefaultRecipOptDlg.m_szEdit1,
                        ulFlags,
                        &ulValues,
                        &lpspva);

            if( HR_FAILED(hResult1))
            {
                MessageBox( E.SzError("lpAdrBook->QueryDefaultRecipOpt()", hResult1),
                                "Client", MBS_ERROR );
                return;
            }
        }

        if(lpspva)
        {
            DisplayProps.m_ulValues   = ulValues;
            DisplayProps.m_lpspva     = lpspva;
            DisplayProps.DoModal();

            MAPIFreeBuffer(lpspva);
        }
    }
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnResolveName
 -
 *  Purpose:


 NOTE, later this should support UNICODE

 *
/********************************************************************/

void CMainWindow::OnResolveName()
{
    CAdrListDlg     AdrListDlg(this);
    COperation      ResolveNameDlg(this);
    ULONG           ulFlags         = 0;
    ULONG           ulUIParam       = 0;
    CGetError       E;
    HRESULT         hResult1        = hrSuccess;
    LPADRLIST       lpAdrList       = NULL;
    LONG            len1            = 0;
    LONG            len2            = 0;
    LONG            len3            = 0;
    ULONG           ulNumStrings    = 0;
    SCODE           sc              = S_OK;
    DWORD           dRet            = 0;
    char            szBuff[80];
    LPTSTR          lpszNewEntryTitle   = NULL;

    ResolveNameDlg.m_CurrentOperation= "lpAdrBook->ResolveName()";
    ResolveNameDlg.m_CBText1         = "ulUIParam:";
    ResolveNameDlg.m_CBText2         = "lpszNewEntryTitle:";
    ResolveNameDlg.m_EditText1       = "Name1 to Resolve:";
    ResolveNameDlg.m_EditText2       = "Name2 to Resolve:";
    ResolveNameDlg.m_EditText3       = "Name3 to Resolve:";
    ResolveNameDlg.m_FlagText1       = "MAPI_DIALOG";
    ResolveNameDlg.m_FlagText2       = "Invalid Flag";
    ResolveNameDlg.m_EditDefault1    = "Joe Smith";
    ResolveNameDlg.m_EditDefault2    = "Sue Jones";
    ResolveNameDlg.m_EditDefault3    = "Elvis";

    dRet = ResolveNameDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = ResolveNameDlg.m_CBContents1.Add(szBuff);

    dRet = ResolveNameDlg.m_CBContents2.Add("NULL");
    dRet = ResolveNameDlg.m_CBContents2.Add("Joe Smith");

    if( ResolveNameDlg.DoModal() == IDOK )
    {
        if( !lstrcmp(ResolveNameDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( !lstrcmp(ResolveNameDlg.m_szCB2,"NULL") )
            lpszNewEntryTitle = NULL;
        else
        {
            lpszNewEntryTitle = (LPTSTR) PvAlloc(30 * sizeof(char) );
            lstrcpy(lpszNewEntryTitle,"Joe Smith");
        }
        if( ResolveNameDlg.m_bFlag1 )
            ulFlags |= MAPI_DIALOG;

        if( ResolveNameDlg.m_bFlag2)
            ulFlags |= TEST_INVALID_FLAG;



        sc = MAPIAllocateBuffer( sizeof(ADRLIST) + 3*sizeof(ADRENTRY), (LPVOID *) &lpAdrList );

        ulNumStrings = 0;
        if( len1 = strlen(ResolveNameDlg.m_szEdit1) )
        {
            sc = MAPIAllocateBuffer( sizeof(SPropValue) ,
                        (LPVOID *) &(lpAdrList->aEntries[ulNumStrings].rgPropVals));
            lpAdrList->aEntries[ulNumStrings].rgPropVals->ulPropTag = PR_DISPLAY_NAME;
            lpAdrList->aEntries[ulNumStrings].rgPropVals->Value.lpszA = ResolveNameDlg.m_szEdit1;
            lpAdrList->aEntries[ulNumStrings].cValues = 1;
            ulNumStrings++;
        }
        if( len2 = strlen(ResolveNameDlg.m_szEdit2) )
        {
            sc = MAPIAllocateBuffer( sizeof(SPropValue) ,
                        (LPVOID *) &(lpAdrList->aEntries[ulNumStrings].rgPropVals));
            lpAdrList->aEntries[ulNumStrings].rgPropVals->ulPropTag = PR_DISPLAY_NAME;
            lpAdrList->aEntries[ulNumStrings].rgPropVals->Value.lpszA = ResolveNameDlg.m_szEdit2;
            lpAdrList->aEntries[ulNumStrings].cValues = 1;
            ulNumStrings++;
        }
        if( len3 = strlen(ResolveNameDlg.m_szEdit3) )
        {
            sc = MAPIAllocateBuffer( sizeof(SPropValue) ,
                        (LPVOID *) &(lpAdrList->aEntries[ulNumStrings].rgPropVals));
            lpAdrList->aEntries[ulNumStrings].rgPropVals[0].ulPropTag = PR_DISPLAY_NAME;
            lpAdrList->aEntries[ulNumStrings].rgPropVals->Value.lpszA = ResolveNameDlg.m_szEdit3;
            lpAdrList->aEntries[ulNumStrings].cValues = 1;
            ulNumStrings++;
        }

        lpAdrList->cEntries = ulNumStrings;

        // display entrylist before
        AdrListDlg.m_List       = "Address List Before ResolveName()";
        AdrListDlg.m_lpAdrList  = lpAdrList;
        AdrListDlg.DoModal();


        hResult1 = lpAdrBook->ResolveName(
                        ulUIParam,
                        ulFlags,
                        lpszNewEntryTitle,
                        lpAdrList);

        if( HR_FAILED(hResult1))
        {
            MessageBox( E.SzError("lpAdrBook->ResolveName()", hResult1),
                    "Client", MBS_ERROR );
        }

        // display entrylist after
        AdrListDlg.m_List       = "Address List After ResolveName()";
        AdrListDlg.m_lpAdrList  = lpAdrList;
        AdrListDlg.DoModal();

        if(lpszNewEntryTitle)
            PvFree(lpszNewEntryTitle);

        if(lpAdrList)
            FreeAdrList(lpAdrList);
    }
}
/********************************************************************/
/*
 -  CMainWindow::
 -  OnSessionRegNotif
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnSessionRegNotif()
{



    MessageBox( "TEST, Not Yet Implemented", "Client", MBS_INFO );
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnStatusRegNotif
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnStatusRegNotif()
{
    MessageBox( "TEST, Not Yet Implemented", "Client", MBS_INFO );
}


/***********************************************************************/
/********************** Callback Functions  ****************************/


/********************************************************************/
/*
 -  CMainWindow::
 -  OnMDBRegNotif
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnMDBRegNotif()
{
//#ifdef XVPORT
    CNotifDlg   Notif(this);

    if(!lpMDB)
    {
        MessageBox( "Need an Open MDB to Advise Notification on Message Store", "Client", MBS_ERROR );
        return;
    }
    Notif.m_Description = "MessageStore Object Notification(via lpSession->Advise)";
    Notif.m_lpUnk       = lpSession;
    Notif.m_ulObjType   = MAPI_SESSION;
    Notif.m_lppNotifs   = &lpMDBNotifs;
    Notif.DoModal();
//#endif
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnTableRegNotif
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnTableRegNotif()
{
    MessageBox( "TEST, Not Yet Implemented", "Client", MBS_INFO );
}

/********************************************************************/
/*
 -  CMainWindow::
 -  OnABRegNotif
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnABRegNotif()
{
    MessageBox( "TEST, Not Yet Implemented", "Client", MBS_INFO );
}


/********************************************************************/
/*
 -  CMainWindow::
 -  OnNotifState
 -
 *  Purpose:
 *
 /********************************************************************/

void CMainWindow::OnNotifState()
{
    CMenu           *menu   = GetMenu();
    UINT            uRet    = 0;



    uRet = menu->GetMenuState( IDM_REGON, MF_BYCOMMAND   );

    // if it is already checked, uncheck it
    if(uRet & MF_CHECKED )
    {
        menu->CheckMenuItem(   IDM_REGON, MF_UNCHECKED );
#ifdef XVPORT
        XVPShow(xvpNum, XVP_HIDE);
#endif
    }
    else
    {
        menu->CheckMenuItem(   IDM_REGON, MF_CHECKED );
#ifdef XVPORT
        XVPShow(xvpNum, XVP_SHOW);
#endif
    }
}



/*******************************************************************/
/*
 -  CMainWindow::
 -  OnGetStoresTable
 *
 *  Purpose:
 *      Bring up Table viewer interface used to view and perform
 *      operations on a table. In this case the table is the
 *      LPMAPITABLE obtained from lpSession->GetMsgStoresTable()
 *
 *  Comments:
 *      lpSession is global pointer to current session
 */
/*******************************************************************/

void CMainWindow::OnGetStoresTable()
{
    CGetError       E;

    hResultSession = lpStoresTable->SeekRow( BOOKMARK_BEGINNING, 0 , NULL );
    if( HR_FAILED(hResultSession) )
    {
        MessageBox( E.SzError("lpStoresTable->SeekRow", hResultSession),
                            "Client", MBS_ERROR );
        return;
    }

    TBLVUViewMapiTable( (LPMAPITABLE FAR *)&lpStoresTable, (HWND)this->m_hWnd );
}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnGetReceiveFolderTable
 *
 *  Purpose:
 *      Bring up Table viewer interface used to view and perform
 *      operations on a table. In this case the table is the
 *      LPMAPITABLE obtained from lpSession->GetReceiveFolderTable()
 *
 *  Comments:
 *      lpSession is global pointer to current session
 */
/*******************************************************************/

void CMainWindow::OnGetReceiveFolderTable()
{
    CGetError       E;
    HRESULT         hResult1        = hrSuccess;

    if(lpReceiveFolderTable)
    {
        lpReceiveFolderTable->Release();
        lpReceiveFolderTable = NULL;
    }

    hResult1 = lpMDB->GetReceiveFolderTable(0, &lpReceiveFolderTable);

    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpSession->GetReceiveFolderTable()", hResult1),
                 "Client", MBS_ERROR );
        return;
    }

    hResult1 = lpReceiveFolderTable->SeekRow( BOOKMARK_BEGINNING, 0 , NULL );
    if( HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpReceiveFolderTable->SeekRow", hResult1),
                            "Client", MBS_ERROR );
        return;
    }

    TBLVUViewMapiTable( (LPMAPITABLE FAR *)&lpReceiveFolderTable, (HWND)this->m_hWnd );
}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnGetStatusTable
 *
 *  Purpose:
 *      Bring up Table viewer interface used to view and perform
 *      operations on a table. In this case the table is the
 *      LPMAPITABLE obtained from lpSession->GetStatusTable()
 *
 *  Comments:
 *      lpSession is global pointer to current session
 */
/*******************************************************************/

void CMainWindow::OnGetStatusTable()
{
    TBLVUViewMapiTable( (LPMAPITABLE FAR *)&lpStatusTable, (HWND)this->m_hWnd );
}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnEnumAdrTypes
 *
 *  Purpose:
 *
 *  Comments:
 *      lpSession is global pointer to current session
 */
/*******************************************************************/

void CMainWindow::OnEnumAdrTypes()
{
    CEnumAdrTypesDlg    Enum(this);
    Enum.DoModal();
}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnGetReceiveFolder
 *
 *  Purpose:
 *      Get the Receive folder for a specific message class.  This
 *      will bring up an operations dialog to determine what parameters
 *      the caller wishes to use for the call to lpMDB->GetReceiveFolder()
 *      This call will bring up a MessageBox and fail if the application
 *      does not have a vaild Message Store pointer
 *
 *  Comments:
 *      lpMDB is global pointer to open message store
 */
/*******************************************************************/

void CMainWindow::OnGetReceiveFolder()
{
    COperation      GetReceiveFldDlg(this);
    CGetError       E;
    char            szBuff[256];
    CString         OperName;
    DWORD           dwRet               = 0;
    ULONG           ulFlags             = 0;
    HRESULT         hResult1            = hrSuccess;
    LPSPropTagArray lpPTA               = NULL;
    CFolderDlg      *lpRecFld           = NULL;
    LPMAPIFOLDER        lpFolder            = NULL;
    LPSTR           lpszExplicitClass   = NULL;
    LPENTRYID       lpEntryID           = NULL;
    ULONG           ulEntryID           = 0;
    ULONG           ulObjType           = 0;
    char            szBuffer[128];

    if(!lpMDB)
    {
        MessageBox( "No Open Message Store","Client", MBS_ERROR );
        return;
    }

    // setup data members of COperation dialog to bring up
    OperName  = "lpMDB->GetReceiveFolder()";

    GetReceiveFldDlg.m_CurrentOperation= OperName;
    GetReceiveFldDlg.m_EditText1       = "lpszMessageClass";
    GetReceiveFldDlg.m_FlagText1       = "MAPI_UNICODE";
    GetReceiveFldDlg.m_FlagText2       = "Invalid Flag";
    GetReceiveFldDlg.m_EditDefault1    = "IPM";

    // bring up modal dialog box, and if user hits OK, process operation
    if( GetReceiveFldDlg.DoModal() == IDOK )
    {
        if( GetReceiveFldDlg.m_bFlag1 )
            ulFlags |= MAPI_UNICODE;

        if( GetReceiveFldDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        hResult1 = lpMDB->GetReceiveFolder(
                        (LPSTR)GetReceiveFldDlg.m_szEdit1,
                        ulFlags,
                        &ulEntryID,
                        &lpEntryID,
                        &lpszExplicitClass );
        if(HR_FAILED(hResult1) )
        {
            MessageBox( E.SzError("lpMDB->GetReceiveFolder()", hResult1),
                            "Client", MBS_ERROR );
            return;
        }

        if(GetReceiveFldDlg.m_szEdit1)
           wsprintf(szBuff,"Receive Folder for Message class %s",
                    (LPSTR)GetReceiveFldDlg.m_szEdit1);
        else
           wsprintf(szBuff,"Receive Folder for Message class %s",
                    "Default Receive Folder");

        // open up the object described, determine if folder, and open folder dlg
        hResult1 = lpMDB->OpenEntry(
                            ulEntryID,
                            lpEntryID,
                            NULL,
                            ulAccess,
                            &ulObjType,
                            (LPUNKNOWN*)(LPMAPIPROP *) &lpFolder);

        if(HR_FAILED(hResult1) )
        {
            MessageBox( E.SzError("lpMDB->OpenEntry() GetReceiveFolder",hResult1),
                            "Client", MBS_ERROR );
            if(lpEntryID)
                MAPIFreeBuffer(lpEntryID);

            if(lpszExplicitClass)
                MAPIFreeBuffer(lpszExplicitClass);

            return;
        }
        if( ulObjType == MAPI_FOLDER )
        {
            if(lpszExplicitClass)
            {
                wsprintf(szBuffer,"%s - Explicit Class == %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),lpszExplicitClass);
            }
            else
            {
                wsprintf(szBuffer,"%s - Explicit Class == %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),"Default Receive Folder");
            }
            lpRecFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpFolder,this);
        }
        else
            MessageBox("ulObjType of OpenEntry on lpFolder != MAPI_FOLDER","Client",MBS_ERROR);

        if(lpEntryID)
            MAPIFreeBuffer(lpEntryID);

        if(lpszExplicitClass)
            MAPIFreeBuffer(lpszExplicitClass);
    }

}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnExit
 -
 *  Purpose:
 *      Asks the application to Close itself.
 */
/*******************************************************************/

void CMainWindow::OnExit()
{
    SendMessage( WM_CLOSE );
}

/*******************************************************************/
/*
 -  CMainWindow::
 -  OnClose
 *
 *  Purpose:
 *      Closes the application's main window from the system control
 *      or from the menu item selection of Exit.  It performs a logoff
 *      if user is still logged on, free's all dll's loaded during this
 *      instance, and Destroy's main window.
 */
/*******************************************************************/

void CMainWindow::OnClose()
{
    OnLogoff();

    if(hlibTBLVU)
    {
       FreeLibrary(hlibTBLVU);
       hlibTBLVU = (HINSTANCE) NULL;
    }

    if (hlibPROPVU)
    {
        FreeLibrary (hlibPROPVU);
        hlibPROPVU = (HINSTANCE) NULL;
    }

    if (hlibSTATVU)
    {
        FreeLibrary (hlibSTATVU);
        hlibSTATVU = (HINSTANCE) NULL;
    }

    MAPIUninitialize();

    DestroyWindow();

}

/*******************************************************************/
/*
 -  CMainWindow::
 -  PostNcDestroy
 *
 *  Purpose:
 *      In order to avoid an access violation when ending this application
 *      it it necessary to catch the WM_DESTROY window message.
 *
 *      Cleanup of the main window will occur in the OnClose function, with
 *      the DestroyWindow() call.
 *
 *      ExitInstance() will perform the remaining Cleanup.
 *
 */
/*******************************************************************/

void CMainWindow::PostNcDestroy()
{
    // don't delete
    // we need this stub so the MFC library doesn't perform Cleanup
}


/*******************************************************************/
/*
 -  CMainWindow::
 -  OnOpenMDB
 *
 *  Purpose:
 *
 */
/*******************************************************************/

void CMainWindow::OnOpenMDB()
{
    COpenStoreDlg   DlgStoreOpen(this);
    CMenu           *menu = GetMenu();

    if(lpMDB)
    {
        MessageBox("FOpenStore lpMDB != NULL, store already open","Client",MBS_ERROR);
        return;
    }

    if( DlgStoreOpen.DoModal() == IDOK)
    {
        menu->EnableMenuItem( IDM_OPENMDB,          MF_GRAYED   );
        menu->EnableMenuItem( IDM_LOGOFFMDB,        MF_ENABLED  );
        menu->EnableMenuItem( IDM_OPENROOT,         MF_ENABLED  );
        menu->EnableMenuItem( IDM_GETREC,           MF_ENABLED  );
        menu->EnableMenuItem( IDM_MDBPROPS,         MF_ENABLED  );
        menu->EnableMenuItem( IDM_SUPPORTMASK,      MF_ENABLED  );


        //$ FUTURE
        // need to lpMDB->GetProps( with PR_VALID_FOLDER_MASK ) to
        // determine if these values are valid or not.
        // ALSO, add PR_VIEWS_ENTRYID, PR_COMMON_VIEWS_ENTRYID, and PR_FINDER_ENTRYID
        // menu items to directly jump to those values.

        menu->EnableMenuItem( IDM_IPMSUBTREE,       MF_ENABLED  );
        menu->EnableMenuItem( IDM_IPMOUTBOX,        MF_ENABLED  );
        menu->EnableMenuItem( IDM_IPMWASTEBASKET,   MF_ENABLED  );
        menu->EnableMenuItem( IDM_IPMSENTMAIL,      MF_ENABLED  );

        if(!hlibTBLVU)
        {
            menu->EnableMenuItem( IDM_GETRECFLDTBL, MF_GRAYED );
        }
        else
        {
            menu->EnableMenuItem( IDM_GETRECFLDTBL, MF_ENABLED );
        }
    }
}


/*******************************************************************/
/*
 -  CMainWindow::
 -  OnMDBProps
 *
 *  Purpose:
 */
/*******************************************************************/

void CMainWindow::OnMDBProps()
{
    PROPVUViewMapiProp("MDB Properties",
                (LPMAPIPROP FAR *)&lpMDB,lpvCopyToDest, (HWND)this->m_hWnd );
}


/*******************************************************************/
/*
 -  CMainWindow::
 -  OnABProps
 *
 *  Purpose:
 */
/*******************************************************************/

void CMainWindow::OnABProps()
{
    PROPVUViewMapiProp("AB Properties",
                (LPMAPIPROP FAR *)&lpAdrBook,lpvCopyToDest, (HWND)this->m_hWnd );
}


/*******************************************************************/
/*
 -  CMainWindow::
 -  OnOpenRootFolder
 *
 *  Purpose:
 */
/*******************************************************************/

void CMainWindow::OnOpenRootFolder()
{
    CGetError       E;
    HRESULT         hResult1        = hrSuccess;
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpRootFolder    = NULL;
    CFolderDlg      *lpDlgChildFld  = NULL;
    CMenu           *menu = GetMenu();
    char            szBuffer[80];

    if(!lpMDB)
    {
        MessageBox("lpMDB == NULL, Need To Open Store before Opening Root Folder","Client",MBS_ERROR);
        goto Cleanup;
    }

    hResult1 = lpMDB->OpenEntry( 0, NULL, NULL, ulAccess, &ulObjType, (LPUNKNOWN*)&lpRootFolder);
    if(HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("lpMDB->OpenEntry(Root Fld)", hResult1), "Client", MBS_ERROR );
        goto Cleanup;
    }

    if( ulObjType == MAPI_FOLDER )
    {
        wsprintf(szBuffer,"%s - %s", GetString("MAPIObjTypes",ulObjType,NULL),"Root");
        lpDlgChildFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpRootFolder,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpMDB != MAPI_FOLDER","Client",MBS_ERROR);
        goto Cleanup;
    }

    return;

Cleanup:

    if(lpRootFolder)
    {
        lpRootFolder->Release();
        lpRootFolder = NULL;
    }
}


/*******************************************************************/
/*
 -  CMainWindow::
 -  OnQueryIdentity
 *
 *  Purpose:
 */
/*******************************************************************/

void CMainWindow::OnQueryIdentity()
{
    CGetError       E;
    ULONG           ulObjType       = 0;
    ULONG           cbEntryID       = 0;
    LPENTRYID       lpEntryID       = NULL;


    if(lpIdentityObj)
    {
        lpIdentityObj->Release();
        lpIdentityObj = NULL;
    }

    hResultSession = lpSession->QueryIdentity(&cbEntryID,&lpEntryID);
    if(HR_FAILED(hResultSession) )
    {
        MessageBox( E.SzError("lpSession->QueryIdentity()",
                 hResultSession), "Client", MBS_ERROR );
        return;
    }

    hResultSession = lpSession->OpenEntry(
                                    cbEntryID,
                                    lpEntryID,
                                    NULL,
                                    MAPI_BEST_ACCESS,
                                    &ulObjType,
                                    (LPUNKNOWN*)&lpIdentityObj);
    if(HR_FAILED(hResultSession) )
    {
        MessageBox( E.SzError("lpSession->OpenEntry(QueryIdentity)",
                 hResultSession), "Client", MBS_ERROR );
    }

    PROPVUViewMapiProp("Props From lpSess->QueryIdentity()",
                (LPMAPIPROP FAR *)&lpIdentityObj,lpvCopyToDest, (HWND)this->m_hWnd );

    if(lpEntryID)
        MAPIFreeBuffer(lpEntryID);

}


/*******************************************************************/
/*
 -  CMainWindow::
 -  OnQueryDefaultMsgOptions
 *
 *  Purpose:
 */
/*******************************************************************/

void CMainWindow::OnQueryDefaultMsgOptions()
{
    CQueryDefaultMsgOptsDlg Default(this);
    COperation      QueryDefMsgOptsDlg(this);
    ULONG           ulFlags         = 0;
    ULONG           cValues         = 0;
    LPSPropValue    lpspvaDefault   = NULL;
    CGetError       E;
    LPWSTR          lpNewBuffer1    = NULL;

    QueryDefMsgOptsDlg.m_CurrentOperation= "lpSession->QueryDefaultMsgOptions()";
    QueryDefMsgOptsDlg.m_EditText1       = "lpszAdrType:";
    QueryDefMsgOptsDlg.m_FlagText1       = "MAPI_UNICODE";
    QueryDefMsgOptsDlg.m_FlagText2       = "Invalid Flag";

    QueryDefMsgOptsDlg.m_EditDefault1    = "MSPEER";

    if( QueryDefMsgOptsDlg.DoModal() == IDOK )
    {

        // determine state/settings of data in dialog upon closing
        if( QueryDefMsgOptsDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        if( QueryDefMsgOptsDlg.m_bFlag1 )
        {
            ulFlags |= MAPI_UNICODE;

            String8ToUnicode(QueryDefMsgOptsDlg.m_szEdit1, &lpNewBuffer1, NULL);

            hResultSession = lpSession->QueryDefaultMessageOpt(
                            (LPTSTR) lpNewBuffer1,
                            ulFlags,
                            &cValues,
                            &lpspvaDefault );
            if(HR_FAILED(hResultSession) )
            {
                MessageBox( E.SzError("m_lpMessage->QueryDefaultMessageOpt()",
                             hResultSession),"Client", MBS_ERROR );
                PvFree(lpNewBuffer1);
                goto Error;
            }

            PvFree(lpNewBuffer1);
        }
        else
        {

            hResultSession = lpSession->QueryDefaultMessageOpt(
                            (LPTSTR) QueryDefMsgOptsDlg.m_szEdit1,
                            ulFlags,
                            &cValues,
                            &lpspvaDefault );
            if(HR_FAILED(hResultSession) )
            {
                MessageBox( E.SzError("m_lpMessage->QueryDefaultMessageOpt()",
                         hResultSession),"Client", MBS_ERROR );
                goto Error;
            }
        }

        Default.m_cValues = cValues;
        Default.m_lpspva  = lpspvaDefault;
        Default.DoModal();
    }
Error:

    if(lpspvaDefault)
        MAPIFreeBuffer(lpspvaDefault);
}



/*******************************************************************/
/*
 -  CMainWindow::
 -  OnStoreLogoff
 *
 *  Purpose:
 */
/*******************************************************************/

void CMainWindow::OnStoreLogoff()
{
    CGetError       E;
    ULONG           ulObjType       = 0;
    CMenu           *menu = GetMenu();
    COperation      StoreLogoffDlg(this);
    ULONG           ulFlags         = 0;
    char            szBuffer[128];

    if(!lpMDB)
    {
        MessageBox("lpMDB == NULL, No Open Store to Logoff","Client",MBS_ERROR);
        return;
    }

    StoreLogoffDlg.m_CurrentOperation= "lpMDB->StoreLogoff()";
    StoreLogoffDlg.m_FlagText1       = "LOGOFF_NO_WAIT";
    StoreLogoffDlg.m_FlagText2       = "LOGOFF_ORDERLY";
    StoreLogoffDlg.m_FlagText3       = "LOGOFF_PURGE";
    StoreLogoffDlg.m_FlagText4       = "LOGOFF_ABORT";
    StoreLogoffDlg.m_FlagText5       = "LOGOFF_QUIET";
    StoreLogoffDlg.m_FlagText6       = "INVALID FLAG";

    if( StoreLogoffDlg.DoModal() == IDOK )
    {

        // determine state/settings of data in dialog upon closing

        if( StoreLogoffDlg.m_bFlag1 )
            ulFlags |= LOGOFF_NO_WAIT;

        if( StoreLogoffDlg.m_bFlag2 )
            ulFlags |= LOGOFF_ORDERLY;

        if( StoreLogoffDlg.m_bFlag3 )
            ulFlags |= LOGOFF_PURGE;

        if( StoreLogoffDlg.m_bFlag4 )
            ulFlags |= LOGOFF_ABORT;

        if( StoreLogoffDlg.m_bFlag5 )
            ulFlags |= LOGOFF_QUIET;

        if( StoreLogoffDlg.m_bFlag6 )
            ulFlags |= TEST_INVALID_FLAG;

        hResultSession = lpMDB->StoreLogoff( &ulFlags );
        if(HR_FAILED(hResultSession) )
        {
            MessageBox( E.SzError("lpMDB->StoreLogoff()", hResultSession),
                            "Client", MBS_ERROR );
            return;
        }

        wsprintf(szBuffer,"StoreLogoff() [out] ulFlags == %#08X, %s%s%s%s%s%s%s%s%s",ulFlags,
            ((ulFlags & LOGOFF_NO_WAIT)         ? "LOGOFF_NO_WAIT | "       : ""),
            ((ulFlags & LOGOFF_ORDERLY)         ? "LOGOFF_ORDERLY | "       : ""),
            ((ulFlags & LOGOFF_PURGE)           ? "LOGOFF_PURGE | "         : ""),
            ((ulFlags & LOGOFF_ABORT)           ? "LOGOFF_ABORT | "         : ""),
            ((ulFlags & LOGOFF_QUIET)           ? "LOGOFF_QUIET | "         : ""),
            ((ulFlags & LOGOFF_COMPLETE)        ? "LOGOFF_COMPLETE | "      : ""),
            ((ulFlags & LOGOFF_INBOUND)         ? "LOGOFF_INBOUND | "       : ""),
            ((ulFlags & LOGOFF_OUTBOUND)        ? "LOGOFF_OUTBOUND | "      : ""),
            ((ulFlags & LOGOFF_OUTBOUND_QUEUE)  ? "LOGOFF_OUTBOUND_QUEUE | ": ""));

        MessageBox(szBuffer,"Client",MBS_INFO);


        if(lpMDB)
        {
            lpMDB->Release();
            lpMDB = NULL;
        }

        menu->EnableMenuItem( IDM_OPENMDB,      MF_ENABLED );
        menu->EnableMenuItem( IDM_LOGOFFMDB,    MF_GRAYED  );
        menu->EnableMenuItem( IDM_OPENROOT,     MF_GRAYED  );
        menu->EnableMenuItem( IDM_GETREC,       MF_GRAYED  );
        menu->EnableMenuItem( IDM_GETRECFLDTBL, MF_GRAYED  );
        menu->EnableMenuItem( IDM_MDBPROPS,     MF_GRAYED  );
        menu->EnableMenuItem( IDM_SUPPORTMASK,  MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMSUBTREE,   MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMOUTBOX,    MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMWASTEBASKET,MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMSENTMAIL,  MF_GRAYED  );
    }
}





/*******************************************************************/
/*
 -  CMainWindow
 -  ToggleMenu
 *
 *  Purpose:
 *      Toggles enable/disable of menu items for logon state
 *      TRUE means logon successful, FALSE means not logged on to MAPI
 *
 *  Parameters
 *      BOOL    TRUE == LOGGED ON  FALSE == NOT LOGGED ON
 *
 *  returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CMainWindow::ToggleMenu(BOOL bState)
{
    CMenu   *menu = GetMenu();

    if(bState)
    {
        //Disable Status Viewer button if DLL is not present.
        if(!hlibSTATVU)
            menu->EnableMenuItem( IDM_OPENSTATUS,   MF_GRAYED );
        else
            menu->EnableMenuItem( IDM_OPENSTATUS,   MF_ENABLED );

        //Disable Status Viewer button if DLL is not present.
        if(!hlibTBLVU)
        {
            menu->EnableMenuItem( IDM_GETSTORESTBL, MF_GRAYED );
            menu->EnableMenuItem( IDM_GETSTATUSTBL, MF_GRAYED );
        }
        else
        {
            menu->EnableMenuItem( IDM_GETSTORESTBL, MF_ENABLED );
            menu->EnableMenuItem( IDM_GETSTATUSTBL, MF_ENABLED );
        }

        //Disable Property Viewer button if DLL is not present.
        if(!hlibPROPVU)
        {
            menu->EnableMenuItem( IDM_MDBPROPS,     MF_GRAYED );
            menu->EnableMenuItem( IDM_ABPROPS,      MF_GRAYED );
            menu->EnableMenuItem( IDM_QUERYIDENTITY,MF_GRAYED );
        }
        else
        {
            menu->EnableMenuItem( IDM_MDBPROPS,     MF_GRAYED );
            menu->EnableMenuItem( IDM_ABPROPS,      MF_ENABLED );
            menu->EnableMenuItem( IDM_QUERYIDENTITY,MF_ENABLED );
        }

        menu->EnableMenuItem( IDM_LOGON,            MF_GRAYED  );
        menu->EnableMenuItem( IDM_LOGOFF,           MF_ENABLED );
        menu->EnableMenuItem( IDM_OPENMDB,          MF_ENABLED );
        menu->EnableMenuItem( IDM_GETREC,           MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMSUBTREE,       MF_GRAYED  );
        menu->EnableMenuItem( IDM_SUPPORTMASK,      MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMOUTBOX,        MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMWASTEBASKET,   MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMSENTMAIL,      MF_GRAYED  );
        menu->EnableMenuItem( IDM_OPENROOT,         MF_GRAYED  );
        menu->EnableMenuItem( IDM_LOGOFFMDB,        MF_GRAYED  );
        menu->EnableMenuItem( IDM_GETRECFLDTBL,     MF_GRAYED  );
        menu->EnableMenuItem( IDM_ENUMADRTYPES,     MF_ENABLED );
        menu->EnableMenuItem( IDM_QUERYDEFMSGOPTS,  MF_ENABLED );
        menu->EnableMenuItem( IDM_SESGETLASTERROR,  MF_ENABLED );
        menu->EnableMenuItem( IDM_ADDRESS,          MF_ENABLED );
        menu->EnableMenuItem( IDM_CREATEONEOFF,     MF_ENABLED );
        menu->EnableMenuItem( IDM_QUERYDEFRECIPOPT, MF_ENABLED );
        menu->EnableMenuItem( IDM_RESOLVENAME,      MF_ENABLED );
        menu->EnableMenuItem( IDM_REGSTATUS,        MF_ENABLED );
        menu->EnableMenuItem( IDM_REGSESSION,       MF_ENABLED );
        menu->EnableMenuItem( IDM_REGMDB,           MF_ENABLED );
        menu->EnableMenuItem( IDM_REGTABLE,         MF_ENABLED );
        menu->EnableMenuItem( IDM_REGAB,            MF_ENABLED );
        menu->EnableMenuItem( IDM_REGON,            MF_ENABLED );

    }
    else
    {
        menu->EnableMenuItem( IDM_LOGON,            MF_ENABLED );
        menu->EnableMenuItem( IDM_LOGOFF,           MF_GRAYED  );
        menu->EnableMenuItem( IDM_OPENMDB,          MF_GRAYED  );
        menu->EnableMenuItem( IDM_MDBPROPS,         MF_GRAYED  );
        menu->EnableMenuItem( IDM_ABPROPS,          MF_GRAYED  );
        menu->EnableMenuItem( IDM_GETREC,           MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMSUBTREE,       MF_GRAYED  );
        menu->EnableMenuItem( IDM_SUPPORTMASK,      MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMOUTBOX,        MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMWASTEBASKET,   MF_GRAYED  );
        menu->EnableMenuItem( IDM_IPMSENTMAIL,      MF_GRAYED  );
        menu->EnableMenuItem( IDM_GETSTORESTBL,     MF_GRAYED  );
        menu->EnableMenuItem( IDM_GETSTATUSTBL,     MF_GRAYED  );
        menu->EnableMenuItem( IDM_OPENROOT,         MF_GRAYED  );
        menu->EnableMenuItem( IDM_LOGOFFMDB,        MF_GRAYED  );
        menu->EnableMenuItem( IDM_ENUMADRTYPES,     MF_GRAYED  );
        menu->EnableMenuItem( IDM_QUERYDEFMSGOPTS,  MF_GRAYED  );
        menu->EnableMenuItem( IDM_OPENSTATUS,       MF_GRAYED  );
        menu->EnableMenuItem( IDM_SESGETLASTERROR,  MF_GRAYED  );
        menu->EnableMenuItem( IDM_ADDRESS,          MF_GRAYED  );
        menu->EnableMenuItem( IDM_CREATEONEOFF,     MF_GRAYED  );
        menu->EnableMenuItem( IDM_QUERYDEFRECIPOPT, MF_GRAYED  );
        menu->EnableMenuItem( IDM_RESOLVENAME,      MF_GRAYED  );
        menu->EnableMenuItem( IDM_REGSTATUS,        MF_GRAYED  );
        menu->EnableMenuItem( IDM_REGSESSION,       MF_GRAYED  );
        menu->EnableMenuItem( IDM_REGMDB,           MF_GRAYED  );
        menu->EnableMenuItem( IDM_REGTABLE,         MF_GRAYED  );
        menu->EnableMenuItem( IDM_REGAB,            MF_GRAYED  );
        menu->EnableMenuItem( IDM_REGON,            MF_GRAYED  );
        menu->EnableMenuItem( IDM_QUERYIDENTITY,    MF_GRAYED  );
        menu->EnableMenuItem( IDM_GETRECFLDTBL,     MF_GRAYED  );

    }
}

/********************************************************************/
/********************** CTheApp Member Functions ****************/

/********************************************************************/
/*
 -  CTheApp::
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
/********************************************************************/

BOOL CTheApp::InitInstance()
{
    CRect   rect;
    char   *szTableLibName;
    char    szMsgBuf[256];

    char   *szPropLibName;
    char    szPropMsgBuf[256];

    char   *szStatLibName;
    char    szStatMsgBuf[256];

    CString str = "MDB Viewer";


    hInst = m_hInstance;

    m_MainWnd.LoadAccelTable( "MainAccelTable" );

    rect.SetRect(MW_LEFT, MW_TOP, MW_RIGHT, MW_BOTTOM);

#ifdef CTL3D
    Ctl3dRegister(      AfxGetInstanceHandle() );   // Register Ctl3D controls
    Ctl3dAutoSubclass(  AfxGetInstanceHandle() );
#endif

    SetDialogBkColor();     // makes dialog background GREY

    if(!m_MainWnd.Create(   NULL,
                            "MDB Viewer Test Application",
                            WS_OVERLAPPEDWINDOW,
                            rect,
                            NULL,
                            MAKEINTRESOURCE(AFX_IDI_STD_FRAME) ) )

        return FALSE;

    m_pMainWnd = &m_MainWnd;

    m_MainWnd.m_bAutoMenuEnable = FALSE;        // MFC 2.0 specific data member
    m_MainWnd.ShowWindow( m_nCmdShow );
    m_MainWnd.UpdateWindow();



#ifdef XVPORT
    // NOTIFICATION VIEWPORT INITIALIZE, MINIMIZE ON DESKTOP
    if(!XVPInit(m_MainWnd.m_hWnd))
        return FALSE;

    if( !(xvpNum = XVPCreateViewport(str.GetBuffer(10)) ) )
        return FALSE;

    XVPReset(xvpNum);
    XVPShow(xvpNum, XVP_MIN);

    XVPSetLogLevel(xvpNum, 0);

#endif




    // LOAD LIBRARY TABLE OBJECT DLL

#ifdef WIN16
    szTableLibName = "tblvu.dll";
    if ((UINT)(hlibTBLVU = LoadLibrary (szTableLibName)) < 32)
#else
    szTableLibName = "tblvu32.dll";
    if ((hlibTBLVU = LoadLibrary (szTableLibName)) < (void *) 32)
#endif
    {
        wsprintf(szMsgBuf,"Cannot Load %s.  Make sure this DLL is available and in your path.",szTableLibName);
        MessageBox(m_MainWnd.m_hWnd,szMsgBuf,"Error", MBS_ERROR);
        goto out;
    }

    // get entrypoint for table viewer dll, and set it to global variable.
    if (!(lpfnViewMapiTable  =  (LPFNVIEWMAPITABLE)GetProcAddress (hlibTBLVU, "ViewMapiTable") ))
    {
        MessageBox(m_MainWnd.m_hWnd,"Cannot Load TBLVUViewMapiTable process address.","Error", MBS_ERROR);
        FreeLibrary(hlibTBLVU);
        hlibTBLVU = (HINSTANCE) NULL;
        goto out;
    }


out:
    // LOAD LIBRARY PROPERTY OBJECT DLL

#ifdef WIN16
    szPropLibName = "propvu.dll";
    if ((UINT)(hlibPROPVU = LoadLibrary (szPropLibName)) < 32)
#else
    szPropLibName = "propvu32.dll";
    if (!(hlibPROPVU = LoadLibrary (szPropLibName)))
#endif
    {
        wsprintf(szPropMsgBuf,"Cannot Load %s.  Make sure this DLL is available and in your path.",szPropLibName);
        MessageBox(m_MainWnd.m_hWnd,szPropMsgBuf,"Error", MB_ICONSTOP | MB_OK);
        goto out2;
    }

    if (!(lpfnViewMapiProp  =  (LPFNVIEWMAPIPROP)GetProcAddress (hlibPROPVU, "ViewMapiProp") ))
    {
        MessageBox(m_MainWnd.m_hWnd,"Cannot Load ViewMapiProp process address.","Error", MB_ICONSTOP | MB_OK);
        FreeLibrary(hlibPROPVU);
        hlibPROPVU = (HINSTANCE) NULL;
        goto out2;
    }



out2:

    // LOAD LIBRARY STATUS OBJECT DLL

#ifdef WIN16
    szStatLibName = "statvu.dll";
    if ((UINT)(hlibSTATVU = LoadLibrary (szStatLibName)) < 32)
#else
    szStatLibName = "statvu32.dll";
    if (!(hlibSTATVU = LoadLibrary (szStatLibName)))
#endif
    {
        wsprintf(szStatMsgBuf,"Cannot Load %s.  Make sure this DLL is available and in your path.",szStatLibName);
        MessageBox(m_MainWnd.m_hWnd,szStatMsgBuf,"Error", MB_ICONSTOP | MB_OK);
        goto logon;
    }

    if (!(lpfnViewMapiStat  =  (LPFNVIEWMAPISTAT)GetProcAddress (hlibSTATVU, "ViewStatusObj") ))
    {
        MessageBox(m_MainWnd.m_hWnd,"Cannot Load ViewStatusObj process address.","Error", MB_ICONSTOP | MB_OK);
        FreeLibrary(hlibSTATVU);
        hlibSTATVU = (HINSTANCE) NULL;
        goto logon;
    }

logon:



    MAPIInitialize(NULL);

    // Logon to Extended MAPI
    m_MainWnd.OnLogon();

    return TRUE;
}


/********************************************************************/
/*
 -  CTheApp::
 -  ExitInstance
 -
 *  Purpose:
 *     Unregister Control 3D and exit this instance of the application
 */
/********************************************************************/

int CTheApp::ExitInstance()
{

#ifdef XVPORT
    XVPReset(xvpNum);

    XVPDestroyViewport(xvpNum);

    XVPDeInit();
#endif

#ifdef CTL3D
    Ctl3dUnregister(AfxGetInstanceHandle());
#endif
    return CWinApp::ExitInstance();
}


/********************************************************************/
/********************** CGetError Member Functions ******************/

/********************************************************************/
/*
 -  CGetError::
 -  SzError       SCODE and ULONG
 -
 *  Purpose:
 *      For printable Error String.  Uses the stringtable
 *      technology from mapitest.
 *
 *  Parameters:
 *      szMsg       - Message Text
 *      scResult    - Error Code
 *
 *  Returns:
 *      szError     - Textized information
 *
 */
/********************************************************************/

LPSTR CGetError::SzError( LPSTR szMsg, SCODE scResult )
{
    wsprintf( m_szMessage, "%s: ", szMsg );

    if(!GetString( "MAPIErrors", scResult, m_szResult ))
    {
        lstrcpy(  m_szResult, "??" );
        wsprintf( m_szBuffer, " %04X", scResult );
        lstrcat(  m_szResult, m_szBuffer );
    }

    lstrcat( m_szMessage, m_szResult );

    return m_szMessage;
}

/********************************************************************/

#ifdef WIN16
LPSTR CGetError::SzError( LPSTR szMsg, HRESULT hResult )
{
    return( SzError(szMsg, GetScode(hResult) ) );
}
#endif


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


/******************* CMainWindow Message Map ****************************/

BEGIN_MESSAGE_MAP( COpenStoreDlg, CDialog )

    ON_COMMAND(     IDC_OPENSTORE,          OnOpen)
    ON_BN_CLICKED(  IDC_READONLY,           OnReadOnly)
    ON_COMMAND(     IDC_SETDEFAULTSTORE,    OnSetDefaultStore)

END_MESSAGE_MAP()


/*******************************************************************/
/*
 -  COpenStore::
 -  OnInitDialog
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

BOOL COpenStoreDlg::OnInitDialog()
{
    int             rgTabStops[5];
    DWORD           dwReturn        = 0;
    HRESULT         hResult1        = hrSuccess;
    CGetError       E;

    SizedSPropTagArray(4,sptaStores) =
    {
        4,
        {
            PR_DEFAULT_STORE,
            PR_DISPLAY_NAME,
            PR_PROVIDER_DISPLAY,
            PR_ENTRYID
        }
    };


    SetWindowText( "Select Message Store To Open" );

    CheckDlgButton(IDC_READONLY,    0);

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_ALL_STORES,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = 15  ;
    rgTabStops[1] = 200 ;
    rgTabStops[2] = 200 ;

    dwReturn = SendDlgItemMessage(IDC_ALL_STORES,LB_SETTABSTOPS,
                    (WPARAM) 3,(LPARAM)rgTabStops );

    // find out what message stores are out there
    hResultSession = lpSession->GetMsgStoresTable(0,&m_lptblStores);
    if( HR_FAILED(hResultSession) )
    {
        MessageBox( E.SzError("lpSession->GetMsgStoresTable", hResultSession),
                            "Client", MBS_ERROR );
        return FALSE;
    }

    hResult1 = m_lptblStores->SetColumns( (LPSPropTagArray) &sptaStores, 0);
    if( HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("m_lptblStores->SetColumns", hResult1),
                            "Client", MBS_ERROR );
        return FALSE;
    }

    DisplayStores();

    return TRUE;
}

/*******************************************************************/

void COpenStoreDlg::OnReadOnly()
{
    CheckDlgButton(IDC_READONLY, !IsDlgButtonChecked(IDC_READONLY) );
}


/*******************************************************************/
/*
 -  COpenStore::
 -  OnOpen
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

void COpenStoreDlg::OnOpen()
{
    LONG            lSelection  = 0;
    ULONG           ulFlags     = 0;
    CGetError       E;

    if( (lSelection = SendDlgItemMessage(IDC_ALL_STORES,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Store to open", "OnOpen", MBS_ERROR );
        return;
    }

    // determine access privileges
    if( IsDlgButtonChecked(IDC_READONLY) )
        ulAccess = 0;
    else
    {
        ulAccess = MAPI_MODIFY;
        ulFlags  = MDB_WRITE;
    }

    hResultSession = lpSession->OpenMsgStore( (ULONG)(void *)m_hWnd,
                    (ULONG)     m_lpRows->aRow[lSelection].lpProps[3].Value.bin.cb,
                    (LPENTRYID) m_lpRows->aRow[lSelection].lpProps[3].Value.bin.lpb,
                    NULL,
                    ulFlags,
                    &lpMDB );
    if( HR_FAILED( hResultSession ) )
    {
        MessageBox( E.SzError("lpSession->OpenMsgStore()", hResultSession),
                 "Client", MBS_ERROR );
        return;
    }

#ifdef XVPORT
    // advise for events on mdb object
    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevNewMail,
                0,
                NULL,
                "fnevNewMail, lpMDB , lpMDB",
                NULL,
                &lpMDBNotifs);

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectCreated,
                0,
                NULL,
                "fnevObjectCreated, lpMDB , lpMDB",
                NULL,
                &lpMDBNotifs);

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectDeleted,
                0,
                NULL,
                "fnevObjectDeleted, lpMDB , lpMDB",
                NULL,
                &lpMDBNotifs);

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectModified,
                0,
                NULL,
                "fnevObjectModified, lpMDB , lpMDB",
                NULL,
                &lpMDBNotifs);

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectMoved,
                0,
                NULL,
                "fnevObjectMoved, lpMDB , lpMDB",
                NULL,
                &lpMDBNotifs);

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectCopied,
                0,
                NULL,
                "fnevObjectCopied, lpMDB , lpMDB",
                NULL,
                &lpMDBNotifs);

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevCriticalError,
                0,
                NULL,
                "fnevCriticalError, lpMDB , lpMDB",
                NULL,
                &lpMDBNotifs);

#endif

    // start default dest to be Message Store object for next copy to
    lpvCopyToDest = lpMDB;

    EndDialog(IDOK);
}


/*******************************************************************/
/*
 -  COpenStore::
 -  ~COpenStoreDlg
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

COpenStoreDlg::~COpenStoreDlg()
{
    // FREE MEMORY AND OBJECTS, and exit dialog

    FreeRowSet(m_lpRows);

    // release table
    if(m_lptblStores)
    {
        m_lptblStores->Release();
        m_lptblStores = NULL;
    }
}


/*******************************************************************/
/*
 -  COpenStore::
 -  OnDisplayStores
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

void COpenStoreDlg::DisplayStores()
{
    CGetError       E;
    LPSRowSet       lpRows          = NULL;
    ULONG           iRow            = 0;
    HRESULT         hResult1        = hrSuccess;
    ULONG           ulRows          = 0;
    char            szBuffer[512];
    DWORD           dwReturn        = 0;
    ULONG           ulDefault       = 0;

    if(m_lpRows)
    {
        FreeRowSet(m_lpRows);
        m_lpRows = NULL;
    }

    SendDlgItemMessage(IDC_ALL_STORES,LB_RESETCONTENT,0,0);

    hResult1 = m_lptblStores->GetRowCount(0,&ulRows);
    if( HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("m_lptblStores->GetRowCount", hResult1),
                            "Client", MBS_ERROR );
        return;
    }

    hResult1 = m_lptblStores->SeekRow( BOOKMARK_BEGINNING,0 , NULL );
    if( HR_FAILED(hResult1) )
    {
        MessageBox( E.SzError("m_lptblStores->SeekRow", hResult1),
                            "Client", MBS_ERROR );
        return;
    }

    if( !HR_FAILED(hResult1 = m_lptblStores->QueryRows( ulRows, TBL_NOADVANCE, &m_lpRows ) ) )
    {
        // if there is at least one row, find column of EntryID and Default Store
        if(m_lpRows->cRows)
        {
            // find out which row has the Default Store and open it
            for(iRow = 0; iRow < m_lpRows->cRows; iRow++)
            {
                // if row is default store, display "D" in 1st column
                if(m_lpRows->aRow[iRow].lpProps[0].Value.b == 1)
                {
                    strcpy(szBuffer,"D");
                    strcat(szBuffer,"\t");
                    ulDefault  = iRow;
                }
                else
                {
                    strcpy(szBuffer," ");
                    strcat(szBuffer,"\t");
                }

                strcat(szBuffer,m_lpRows->aRow[iRow].lpProps[1].Value.lpszA);
                strcat(szBuffer,"\t");

                strcat(szBuffer,m_lpRows->aRow[iRow].lpProps[2].Value.lpszA);

                dwReturn = SendDlgItemMessage(IDC_ALL_STORES,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
            }
        }
    }

    dwReturn = SendDlgItemMessage(IDC_ALL_STORES,LB_SETCURSEL, (WPARAM) ulDefault, 0 );
}

/*******************************************************************/
/*
 -  COpenStore::
 -  OnSetDefaultStore
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

void COpenStoreDlg::OnSetDefaultStore()
{
    LONG    lSelection  = 0;
    CGetError       E;

    if( (lSelection = SendDlgItemMessage(IDC_ALL_STORES,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a store to set as default", "OnOpen", MBS_ERROR );
        return;
    }

    hResultSession = lpSession->SetDefaultStore(
                    MAPI_DEFAULT_STORE,
                    (ULONG)     m_lpRows->aRow[lSelection].lpProps[3].Value.bin.cb,
                    (LPENTRYID) m_lpRows->aRow[lSelection].lpProps[3].Value.bin.lpb );

    if( HR_FAILED(hResultSession) )
    {
        MessageBox( E.SzError("lpSession->SetDefaultStore()", hResultSession),
                 "Client", MBS_ERROR );
    }
    DisplayStores();
}

/*******************************************************************/
/*
 -  COpenStore::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void COpenStoreDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}



/*******************************************************************/
/**************************** ENUMADRTYPES **************************/

/********************************************************************/
/*
 -  CEnumAdrTypesDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CEnumAdrTypesDlg::OnInitDialog()
{
    CGetError       E;
    DWORD           dwReturn        = 0;
    ULONG           cAdrTypes       = 0;
    char            szCTypes[80];
    ULONG           idx;

    SendDlgItemMessage(IDC_ENUM,LB_RESETCONTENT,0,0);


    //$ FUTURE OPerations dialog with flags UNICODE
    hResultSession = lpSession->EnumAdrTypes(
                    0,
                    &cAdrTypes,
                    &m_lppszAdrTypes);
    if( HR_FAILED(hResultSession) )
    {
        MessageBox( E.SzError("lpSession->EnumAdrTypes()", hResultSession),
                 "Client", MBS_ERROR );
        goto error;
    }

    wsprintf(szCTypes,"cAdrTypes:  %lu",cAdrTypes);
    SetDlgItemText(IDT_ENUM_CTYPES,szCTypes);

    for( idx = 0 ; idx < cAdrTypes; idx++)
        dwReturn = SendDlgItemMessage(IDC_ENUM,LB_ADDSTRING,0,
                            (LPARAM) m_lppszAdrTypes[idx] );

    return TRUE;

error:

    return FALSE;
}


/*******************************************************************/
/*
 -  CEnumAdrTypesDlg::
 -  ~CEnumAdrTypesDlg
 -
 *  Purpose:
 *      Destructor for class CEnumAdrTypesDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CEnumAdrTypesDlg::~CEnumAdrTypesDlg()
{
    if(m_lppszAdrTypes)
        MAPIFreeBuffer(m_lppszAdrTypes);
}


/*******************************************************************/
/*
 -  CEnumAdrTypesDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CEnumAdrTypesDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*******************************************************************/
/*********************** StoreSupportMask **************************/

/********************************************************************/
/*
 -  CStoreSupportMaskDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CStoreSupportMaskDlg::OnInitDialog()
{
    CGetError       E;
    DWORD           dwReturn        = 0;
    HRESULT         hResult         = hrSuccess;
    char            szBuff[80];
    int             rgTabStops[4];
    char            szBuffer1[200];
    
    SPropTagArray   sptaSupport =
    {
        1,
        {
            PR_STORE_SUPPORT_MASK
        }
    };



    SendDlgItemMessage(IDC_SUPPORT_MASK,LB_RESETCONTENT,0,0);

    rgTabStops[0] = 150 ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );


    hResult = lpMDB->GetProps((LPSPropTagArray)&sptaSupport,0,&m_cVals,&m_lpspva);
    if( HR_FAILED(hResult) )
    {
        MessageBox( E.SzError("lpMDB->GetProps(PR_STORE_SUPPORT_MASK)", hResult),
                 "Client", MBS_ERROR );
        goto error;
    }

    if(m_lpspva[0].ulPropTag != PR_STORE_SUPPORT_MASK )
        goto error;
        
    wsprintf(szBuff,"PR_STORE_SUPPORT_MASK == %lu",m_lpspva[0].Value.l);
    SetDlgItemText(IDT_SUPPORT_MASK,szBuff);

    // now logical or with all valid types and addstring each value YES or NO

    wsprintf(szBuffer1,"STORE_ENTRYID_UNIQUE\t%s", 
            ((m_lpspva[0].Value.l & STORE_ENTRYID_UNIQUE) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_READONLY\t%s", 
            ((m_lpspva[0].Value.l & STORE_READONLY) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_SEARCH_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_SEARCH_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_MODIFY_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_MODIFY_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_CREATE_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_CREATE_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_ATTACH_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_ATTACH_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_OLE_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_OLE_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_NOTIFY_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_NOTIFY_OK) ? "YES" : "NO" ));
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_MV_PROPS_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_MV_PROPS_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_CATEGORIZE_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_CATEGORIZE_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_RTF_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_RTF_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_RESTRICTION_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_RESTRICTION_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );

    wsprintf(szBuffer1,"STORE_SORT_OK\t%s", 
            ((m_lpspva[0].Value.l & STORE_SORT_OK) ? "YES" : "NO") );
    dwReturn = SendDlgItemMessage(IDC_SUPPORT_MASK,LB_ADDSTRING,0,(LPARAM) szBuffer1 );
    
    return TRUE;

error:

    return FALSE;
}


/*******************************************************************/
/*
 -  CStoreSupportMaskDlg::
 -  ~CStoreSupportMaskDlg
 -
 *  Purpose:
 *      Destructor for class CStoreSupportMaskDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CStoreSupportMaskDlg::~CStoreSupportMaskDlg()
{
    if(m_lpspva)
        MAPIFreeBuffer(m_lpspva);
}


/*******************************************************************/
/*
 -  CStoreSupportMaskDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CStoreSupportMaskDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}




/*******************************************************************/
/**************************** QUERYDEFAULTMESSAGEOPT****************/

/********************************************************************/
/*
 -  CQueryDefaultMsgOptssDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CQueryDefaultMsgOptsDlg::OnInitDialog()
{
    DWORD           dwReturn        = 0;
    ULONG           cAdrTypes       = 0;
    char            szCValues[80];
    ULONG           idx;
    int             rgTabStops[4];
    char szID[50];
    char szData[512];
    char szType[32];      // Assumes no PropType string longer than 31 chars
    char            szBuffer[1024];


    SendDlgItemMessage(IDC_QD_PROPS,LB_RESETCONTENT,0,0);

    dwReturn = SendDlgItemMessage(IDC_QD_PROPS,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_QD_PROPS,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    wsprintf(szCValues,"cAdrTypes:  %lu",m_cValues);
    SetDlgItemText(IDT_QD_CVALS,szCValues);

    for( idx = 0 ; idx < m_cValues; idx++)
    {
        if(GetString("PropIDs", PROP_ID(m_lpspva[idx].ulPropTag), szID ) )
        {
            lstrcat(szBuffer, szID );
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpspva[idx].ulPropTag) );
        }

        if( GetString("PropType", PROP_TYPE(m_lpspva[idx].ulPropTag), szType) )
        {
            lstrcat(szBuffer, szType);
            lstrcat(szBuffer,"\t");
        }
        else
        {
            wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpspva[idx].ulPropTag) );
            lstrcat(szBuffer,szType);
        }

        SzGetPropValue(szData,(LPSPropValue) &m_lpspva[idx]);
        lstrcat(szBuffer,szData);

        dwReturn = SendDlgItemMessage(IDC_QD_PROPS,LB_ADDSTRING,0,
                            (LPARAM) szBuffer);
    }

    return TRUE;
}


/*******************************************************************/
/*
 -  CQueryDefaultMsgOptssDlg::
 -  ~CQueryDefaultMsgOptssDlg
 -
 *  Purpose:
 *      Destructor for class CQueryDefaultMsgOptssDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CQueryDefaultMsgOptsDlg::~CQueryDefaultMsgOptsDlg()
{

}


/*******************************************************************/
/*
 -  CQueryDefaultMsgOptssDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CQueryDefaultMsgOptsDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}



/*******************************************************************/
/**************************** ENTRYLIST **************************/


/******************* CMainWindow Message Map ****************************/

BEGIN_MESSAGE_MAP( CEntryListDlg, CModalDialog )

    ON_COMMAND( IDC_EL_CLEAR,        OnClearList )

END_MESSAGE_MAP()

/********************************************************************/
/*
 -  CEntryListDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CEntryListDlg::OnInitDialog()
{
    DWORD   dwReturn = 0;

    SetWindowText( m_List.GetBuffer(30) );

    dwReturn = SendDlgItemMessage(IDC_EL_LIST,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    DisplayList();

    return TRUE;
}


/*******************************************************************/
/*
 -  CEntryListDlg::
 -  ~CEntryListDlg
 -
 *  Purpose:
 *      Destructor for class CEntryListDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CEntryListDlg::~CEntryListDlg()
{

}


/*******************************************************************/
/*
 -  CEntryListDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CEntryListDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*******************************************************************/
/*
 -  CEntryListDlg::
 -  DisplayList
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CEntryListDlg::DisplayList()
{
    char    szCVals[30];
    ULONG   iRow        = 0;
    DWORD   dwReturn    = 0;
    char    szBuffer[256];

    SendDlgItemMessage(IDC_EL_LIST,LB_RESETCONTENT,0,0);

    if( !(*m_lppEntryList))
    {
        // DISPLAY Expected lpPTA->cValues
        wsprintf(szCVals,"cValues:  %lu",0);
        SetDlgItemText(IDT_EL_CVALS,szCVals);

        return;
    }
    if((*m_lppEntryList)->cValues)
    {
        // DISPLAY Expected lpPTA->cValues
        wsprintf(szCVals,"cValues:  %lu",(*m_lppEntryList)->cValues);
        SetDlgItemText(IDT_EL_CVALS,szCVals);

        // find out which row has the Default Status and open it
        for(iRow = 0; iRow < (*m_lppEntryList)->cValues; iRow++)
        {
            // display PR_ENTRYID
            char    lpszHex[9];
            ULONG   cb      = 0;
            ULONG   cChars  = 0;
            LPBYTE  lpb     = (*m_lppEntryList)->lpbin[iRow].lpb;

            szBuffer[0] = '\0';

            while((cb < (*m_lppEntryList)->lpbin[iRow].cb) && (cChars < 255 -16 ) )
            {
                wsprintf(lpszHex, "%02X ", *lpb);
                lstrcat(szBuffer, lpszHex);
                cChars += 3;
                lpb++;
                cb++;
            }

            dwReturn = SendDlgItemMessage(IDC_EL_LIST,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
        }
    }
}


/*******************************************************************/
/*
 -  CEntryListDlg::
 -  OnClearList
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CEntryListDlg::OnClearList()
{
    PvFree(*m_lppEntryList);
    *m_lppEntryList = NULL;

    DisplayList();
}



/***************************************************************/
/**************************** ADRLIST **************************/

/********************************************************************/
/*
 -  CAdrListDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CAdrListDlg::OnInitDialog()
{
    DWORD           dwReturn = 0;
    int             rgTabStops[5];

    SetWindowText( m_List.GetBuffer(30) );

    dwReturn = SendDlgItemMessage(IDC_ADRLIST,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = 20  ;   // first indent
    rgTabStops[1] = 150 ;   // for PropID space
    rgTabStops[2] = 210 ;   // for PropType space
    rgTabStops[3] = 270 ;   // for Data space

    dwReturn = SendDlgItemMessage(IDC_ADRLIST,LB_SETTABSTOPS,
                    (WPARAM) 4,(LPARAM)rgTabStops );

    DisplayAdrList();

    return TRUE;
}

/*******************************************************************/
/*
 -  CAdrListDlg::
 -  ~CAdrListDlg
 -
 *  Purpose:
 *      Destructor for class CAdrListDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CAdrListDlg::~CAdrListDlg()
{

}


/*******************************************************************/
/*
 -  CAdrListDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CAdrListDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*******************************************************************/
/*
 -  CAdrListDlg::
 -  DisplayList
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CAdrListDlg::DisplayAdrList()
{
    char    szCVals[30];
    ULONG   cEntries        = 0;
    DWORD   dwReturn        = 0;
    char    szBuffer[1024];
    ULONG   idx;

    SendDlgItemMessage(IDC_ADRLIST,LB_RESETCONTENT,0,0);

    if( !m_lpAdrList)
    {
        // DISPLAY Expected lpPTA->cValues
        wsprintf(szCVals,"cEntries:  %lu",0);
        SetDlgItemText(IDT_CENTRIES,szCVals);

        return;
    }
    if(m_lpAdrList->cEntries)
    {
        // DISPLAY Expected lpPTA->cValues
        wsprintf(szCVals,"cEntries:  %lu",m_lpAdrList->cEntries);
        SetDlgItemText(IDT_CENTRIES,szCVals);

        // find out which row has the Default Status and open it
        for(cEntries = 0; cEntries < m_lpAdrList->cEntries; cEntries++)
        {
            wsprintf(szBuffer,"lpAdrList->aEntries[%lu].ulReserved1 = %lu",
                    cEntries,m_lpAdrList->aEntries[cEntries].ulReserved1 );

            dwReturn = SendDlgItemMessage(IDC_ADRLIST,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);

            wsprintf(szBuffer,"lpAdrList->aEntries[%lu].cValues = %lu",
                    cEntries,m_lpAdrList->aEntries[cEntries].cValues );

            dwReturn = SendDlgItemMessage(IDC_ADRLIST,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);

            for( idx = 0 ; idx < m_lpAdrList->aEntries[cEntries].cValues ; idx++)
            {
                SzTextizeProp(szBuffer, &(m_lpAdrList->aEntries[cEntries].rgPropVals[idx]));

                dwReturn = SendDlgItemMessage(IDC_ADRLIST,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
            }
        }
    }
}


/*******************************************************************/
/**************************** PROP_DISPLAY **************************/



/********************************************************************/
/*
 -  CPropDisplayDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CPropDisplayDlg::OnInitDialog()
{
    DWORD           dwReturn = 0;
    int             rgTabStops[4];

    SetWindowText( m_List.GetBuffer(30) );

    // set size of horizontal scroll and tab stops in prop view listbox
    dwReturn = SendDlgItemMessage(IDC_PROP_DISPLAY,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_PROP_DISPLAY,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    DisplayProps();

    return TRUE;
}


/*******************************************************************/
/*
 -  CPropDisplayDlg::
 -  ~CPropDisplayDlg
 -
 *  Purpose:
 *      Destructor for class CPropDisplayDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CPropDisplayDlg::~CPropDisplayDlg()
{

}


/*******************************************************************/
/*
 -  CPropDisplayDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CPropDisplayDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*******************************************************************/
/*
 -  CPropDisplayDlg::
 -  DisplayProps
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CPropDisplayDlg::DisplayProps()
{
    ULONG   idx         = 0;
    DWORD   dwReturn    = 0;
    char    szBuffer[256];
    char    szID[50];
    char    szData[512];
    char    szType[32];      // Assumes no PropType string longer than 31 chars


    SendDlgItemMessage(IDC_PROP_DISPLAY,LB_RESETCONTENT,0,0);

    if( (!m_lpEntryID) && (!m_lpspva) )
        return;

    if(m_lpEntryID)
    {
        wsprintf(szBuffer,"cbEntryID: %lu",m_ulValues);
        SetDlgItemText(IDT_PROP_DISPLAY_CVALS,szBuffer);
        SetDlgItemText(IDT_PROP_DISPLAY,"LPENTRYID:");

        // print out entryid
        char    lpszHex[9];
        ULONG   cb      = 0;
        ULONG   cChars  = 0;
        LPBYTE  lpb     = (LPBYTE)m_lpEntryID;

        szBuffer[0] = '\0';

        while( (cb < m_ulValues) && (cChars < 255 -16 ) )
        {
            wsprintf(lpszHex, "%02X ", *lpb);
            lstrcat(szBuffer, lpszHex);
            cChars += 3;
            lpb++;
            cb++;
        }

        dwReturn = SendDlgItemMessage(IDC_PROP_DISPLAY,LB_ADDSTRING,0,
                    (LPARAM)szBuffer);
    }
    else    // else print out lpspva
    {

        wsprintf(szBuffer,"cValues:  %lu",m_ulValues);
        SetDlgItemText(IDT_PROP_DISPLAY_CVALS,szBuffer);

        // determine if string, hex, or decimal and build row of listbox
        for(idx = 0; idx < m_ulValues; idx++)
        {
            szID[0]     = '\0' ;
            szData[0]   = '\0' ;
            szType[0]   = '\0' ;
            szBuffer[0] = '\0' ;


            if(GetString("PropIDs", PROP_ID(m_lpspva[idx].ulPropTag), szID ) )
            {
                lstrcat(szBuffer, szID );
                lstrcat(szBuffer, "\t");
            }
            else
            {
                wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpspva[idx].ulPropTag) );
            }


            if( GetString("PropType", PROP_TYPE(m_lpspva[idx].ulPropTag), szType) )
            {
                lstrcat(szBuffer, szType);
                lstrcat(szBuffer,"\t");
            }
            else
            {
                wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpspva[idx].ulPropTag) );
                lstrcat(szBuffer,szType);

            }

            SzGetPropValue(szData,(LPSPropValue) &m_lpspva[idx]);
            lstrcat(szBuffer,szData);

            dwReturn = SendDlgItemMessage(IDC_PROP_DISPLAY,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
        }
    }
}


/*******************************************************************/
/*
 -  CNotificationDlg::
 -  DisplayNotif
 -
 *  Purpose:
 *
 */
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/*******************************************************************/
/*
 -  LogNotifToXVPLog CALLBACK
 -
 *  Purpose:
 *
 */
/*******************************************************************/

// OTHER WAY TO DECLARE FUNCTION
//STDAPI_(long) LogNotifToXVPLog(  LPVOID          lpvContext,
//                                 ULONG           cNotif,
//                                 LPNOTIFICATION  lpNotif)

#ifdef XVPORT

SCODE STDAPICALLTYPE LogNotifToXVPLog(  LPVOID          lpvContext,
                                        ULONG           cNotif,
                                        LPNOTIFICATION  lpNotif)
{


    ULONG   idx         = 0;
    char    szBuffer[1024];
    char    szBuff1[1024];
    LPBYTE  lpb;
    char    lpszHex[9];
    ULONG   cb          = 0;
    ULONG   cChars      = 0;
    UINT    bFlags      = 0;
    ULONG   i;

    wsprintf(szBuffer,"NOTIFICATION #%lu : %s",
                        (ULONG) ( (LPNOTIFCONTEXT)lpvContext)->ulCount,
                        (char *)( (LPNOTIFCONTEXT)lpvContext)->szComment);


    XVPLog(xvpNum,0,szBuffer);

    wsprintf(szBuffer,"cNotification:       == %lu",cNotif);
    XVPLog(xvpNum,1,szBuffer);

    if( !lpNotif )
        return 1;


    ((LPNOTIFCONTEXT)lpvContext)->ulCount++;
    wsprintf(szBuffer,"lpvContext:          == 0x%08X, Count == %lu Comment == %s", lpvContext,
                        (ULONG) ( (LPNOTIFCONTEXT)lpvContext)->ulCount,
                        (char *)( (LPNOTIFCONTEXT)lpvContext)->szComment);

    XVPLog(xvpNum,1,szBuffer);

    for( idx = 0 ; idx < cNotif ; idx++)
    {

        wsprintf(szBuffer,"lpNotif[%lu]         == 0x%08X",idx,lpNotif );
        XVPLog(xvpNum,1,szBuffer);

        GetString("NotifEvents",lpNotif[idx].ulEventType, szBuff1);
        wsprintf(szBuffer,"lpNotif[%lu].ulEventType                == %lu, %s",idx,
                lpNotif[idx].ulEventType,
                szBuff1);
        XVPLog(xvpNum,2,szBuffer);

        switch(lpNotif[idx].ulEventType)
        {
            case fnevTableModified:
                GetString("TableEvents",lpNotif[idx].info.tab.ulTableEvent, szBuff1);
                wsprintf(szBuffer,"lpNotif[%lu].info.tab.ulTableEvent    == %lu, %s",idx,
                            lpNotif[idx].info.tab.ulTableEvent,
                            szBuff1);
                XVPLog(xvpNum,2,szBuffer);

                GetString("MAPIErrors", GetScode(lpNotif[idx].info.tab.hResult), szBuff1 );
                wsprintf(szBuffer,"lpNotif[%lu].info.tab.hResult           == 0x%08X, %s",idx,
                            lpNotif[idx].info.tab.hResult,
                            szBuff1);
                XVPLog(xvpNum,2,szBuffer);

                SzTextizeProp((LPSTR)szBuff1, &(lpNotif[idx].info.tab.propIndex));
                wsprintf(szBuffer,"lpNotif[%lu].info.tab.propIndex        == %s", idx, szBuff1);
                XVPLog(xvpNum,2,szBuffer);


                SzTextizeProp((LPSTR)szBuff1, &(lpNotif[idx].info.tab.propPrior));
                wsprintf(szBuffer,"lpNotif[%lu].info.tab.propPrior         == %s", idx, szBuff1);
                XVPLog(xvpNum,2,szBuffer);


                wsprintf(szBuffer,"lpNotif[%lu].info.tab.row.ulAdrEntryPad == %lu",idx,
                                lpNotif[idx].info.tab.row.ulAdrEntryPad);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.tab.row.cValues     == %lu",idx,
                                lpNotif[idx].info.tab.row.cValues);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.tab.row.lpProps      == 0x%08X",idx,
                                lpNotif[idx].info.tab.row.lpProps);
                XVPLog(xvpNum,2,szBuffer);

                if(lpNotif[idx].info.tab.row.lpProps)
                {
                    for(i = 0; i < lpNotif[idx].info.tab.row.cValues ; i++)
                    {

                        SzTextizeProp(szBuff1, &lpNotif[idx].info.tab.row.lpProps[i] );
                        wsprintf(szBuffer,"lpNotif[%lu].info.tab.row.lpProps[%lu]  == %s",idx,i,szBuff1);
                        XVPLog(xvpNum,2,szBuffer);
                    }
                }

                break;

            case fnevCriticalError:
                wsprintf(szBuffer,"lpNotif[%lu].info.err.cbEntryID         == %lu",idx,
                                lpNotif[idx].info.err.cbEntryID);
                XVPLog(xvpNum,2,szBuffer);
                wsprintf(szBuffer,"lpNotif[%lu].info.err.lpEntryID         == 0x%08X",idx,
                                lpNotif[idx].info.err.lpEntryID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.err.lpEntryID)
                {
                    bFlags = (UINT)lpNotif[idx].info.err.lpEntryID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.err.lpEntryID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);

                    SzGetEntryID(szBuff1,lpNotif[idx].info.err.lpEntryID,lpNotif[idx].info.err.cbEntryID);
                    wsprintf(szBuffer,"lpNotif[%lu].info.err.lpEntryID         == %s",idx,szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }

                GetString("MAPIErrors",lpNotif[idx].info.err.scode , szBuff1 );
                wsprintf(szBuffer,"lpNotif[%lu].info.err,scode             == 0x%08X, %s",idx,
                            lpNotif[idx].info.err.scode,
                            szBuff1);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.err.ulFlags           == %lu",idx,
                                lpNotif[idx].info.err.ulFlags);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.err.lpMAPIError->lpszError         == %s",idx,
                                lpNotif[idx].info.err.lpMAPIError->lpszError);
                XVPLog(xvpNum,2,szBuffer);
                break;

            case fnevNewMail:
                wsprintf(szBuffer,"lpNotif[%lu].info.newmail.cbEntryID     == %lu",idx,
                                lpNotif[idx].info.newmail.cbEntryID);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.newmail.lpEntryID     == 0x%08X",idx,
                                lpNotif[idx].info.newmail.lpEntryID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.newmail.lpEntryID)
                {
                    bFlags = (UINT)lpNotif[idx].info.newmail.lpEntryID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.newmail.lpEntryID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);

                    SzGetEntryID(szBuff1,
                                    lpNotif[idx].info.newmail.lpEntryID,
                                    lpNotif[idx].info.newmail.cbEntryID);

                    wsprintf(szBuffer,"lpNotif[%lu].info.newmail.lpEntryID     == %s", idx, szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }

                wsprintf(szBuffer,"lpNotif[%lu].info.newmail.cbParentID    == %lu",idx,
                                lpNotif[idx].info.newmail.cbParentID);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.newmail.lpParentID    == 0x%08X",idx,
                                lpNotif[idx].info.newmail.lpParentID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.newmail.lpParentID)
                {
                    bFlags = (UINT)lpNotif[idx].info.newmail.lpParentID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.newmail.lpParentID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);



                    SzGetEntryID(szBuff1,lpNotif[idx].info.newmail.lpParentID,lpNotif[idx].info.newmail.cbParentID);
                    wsprintf(szBuffer,"lpNotif[%lu].info.newmail.lpParentID    == %s",idx,szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }

                wsprintf(szBuffer,"lpNotif[%lu].info.newmail.ulFlags       == %lu",idx,
                                lpNotif[idx].info.newmail.ulFlags);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.newmail.lpszMessageClass == %s",idx,
                                lpNotif[idx].info.newmail.lpszMessageClass);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.newmail.ulMessageFlags   == %lu",idx,
                                lpNotif[idx].info.newmail.ulMessageFlags);
                XVPLog(xvpNum,2,szBuffer);
                break;

            case fnevObjectCreated:
            case fnevObjectDeleted:
            case fnevObjectModified:
            case fnevObjectMoved:
            case fnevObjectCopied:
            case fnevSearchComplete:

                wsprintf(szBuffer,"lpNotif[%lu].info.obj.cbEntryID         == %lu",idx,
                                lpNotif[idx].info.obj.cbEntryID);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpEntryID         == 0x%08X",idx,
                                lpNotif[idx].info.obj.lpEntryID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.obj.lpEntryID)
                {
                    bFlags = (UINT)lpNotif[idx].info.obj.lpEntryID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpEntryID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);

                    SzGetEntryID(szBuff1,lpNotif[idx].info.obj.lpEntryID,lpNotif[idx].info.obj.cbEntryID);
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpEntryID         == %s",idx,szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }


                wsprintf(szBuffer,"lpNotif[%lu].info.obj.cbParentID        == %lu",idx,
                                lpNotif[idx].info.obj.cbParentID);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpParentID        == 0x%08X",idx,
                                lpNotif[idx].info.obj.lpParentID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.obj.lpParentID)
                {
                    bFlags = (UINT)lpNotif[idx].info.obj.lpParentID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpParentID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);



                    SzGetEntryID(szBuff1,lpNotif[idx].info.obj.lpParentID,lpNotif[idx].info.obj.cbParentID);
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpParentID        == %s",idx,szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }


                wsprintf(szBuffer,"lpNotif[%lu].info.obj.cbOldID           == %lu",idx,
                                lpNotif[idx].info.obj.cbOldID);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpOldID           == 0x%08X",idx,
                                lpNotif[idx].info.obj.lpOldID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.obj.lpOldID)
                {
                    bFlags = (UINT)lpNotif[idx].info.obj.lpOldID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpOldID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);

                    SzGetEntryID(szBuff1,lpNotif[idx].info.obj.lpOldID,lpNotif[idx].info.obj.cbOldID);
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpOldID           == %s",idx,szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }

                wsprintf(szBuffer,"lpNotif[%lu].info.obj.cbOldParentID     == %lu",idx,
                                lpNotif[idx].info.obj.cbOldParentID);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpOldParentID     == 0x%08X",idx,
                                lpNotif[idx].info.obj.lpOldParentID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.obj.lpOldParentID)
                {
                    bFlags = (UINT)lpNotif[idx].info.obj.lpOldParentID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpOldParentID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);

                    SzGetEntryID(szBuff1,lpNotif[idx].info.obj.lpOldParentID,lpNotif[idx].info.obj.cbOldParentID);
                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpOldParentID     == %s",idx,szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }


                wsprintf(szBuffer,"lpNotif[%lu].info.obj.ulObjType         == %lu",idx,
                                lpNotif[idx].info.obj.ulObjType);
                XVPLog(xvpNum,2,szBuffer);


                wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpPropTagArray    == 0x%08X",idx,
                            lpNotif[idx].info.obj.lpPropTagArray);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpPropTagArray    == 0x%08X",idx,
                        lpNotif[idx].info.obj.lpPropTagArray);
                XVPLog(xvpNum,2,szBuffer);

                if(lpNotif[idx].info.obj.lpPropTagArray)
                {

                    wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpPropTagArray->cValues == %lu",idx,
                            lpNotif[idx].info.obj.lpPropTagArray->cValues);
                    XVPLog(xvpNum,2,szBuffer);
                    for(i = 0 ; i < lpNotif[idx].info.obj.lpPropTagArray->cValues ; i++)
                    {
                        SzGetPropTag(szBuff1, lpNotif[idx].info.obj.lpPropTagArray->aulPropTag[i] );
                        wsprintf(szBuffer,"lpNotif[%lu].info.obj.lpPropTagArray->aulPropTag[%lu] == %s",
                                    idx,i,szBuff1);
                        XVPLog(xvpNum,2,szBuffer);
                    }
                }

                break;

            case fnevExtended:

                wsprintf(szBuffer,"lpNotif[%lu].info.ext.ulEvent           == %lu",idx,
                                lpNotif[idx].info.ext.ulEvent);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.ext.cb                == %lu",idx,
                                lpNotif[idx].info.ext.cb);
                XVPLog(xvpNum,2,szBuffer);

                lpb = lpNotif[idx].info.ext.pbEventParameters;
                wsprintf(szBuff1, "cb: %3lu   *lpNotif[%lu].info.ext.pbEventParameters: ",
                            lpNotif[idx].info.ext.cb, idx);
                cChars += strlen(szBuff1);

                while((cb < lpNotif[idx].info.ext.cb) && (cChars < (255 - 16) ) )
                {
                    wsprintf(lpszHex, "%02X ", *lpb);
                    lstrcat(szBuff1, lpszHex);
                    cChars += 3;
                    lpb++;
                    cb++;
                }

                if( ((lpNotif[idx].info.ext.cb)*3) >  (255 - 16) )
                {
                    lstrcat(szBuff1, " ...");
                }
                XVPLog(xvpNum,2,szBuffer);
                break;

            case fnevStatusObjectModified:

                wsprintf(szBuffer,"lpNotif[%lu].info.statobj.cbEntryID     == %lu",idx,
                                lpNotif[idx].info.statobj.cbEntryID);
                XVPLog(xvpNum,2,szBuffer);

                wsprintf(szBuffer,"lpNotif[%lu].info.statobj.lpEntryID     == 0x%08X",idx,
                                lpNotif[idx].info.statobj.lpEntryID);
                XVPLog(xvpNum,2,szBuffer);


                if(lpNotif[idx].info.statobj.lpEntryID)
                {
                    bFlags = (UINT)lpNotif[idx].info.statobj.lpEntryID->abFlags[0];         // might need to consider abFlags[1-3]
                    wsprintf(szBuffer,"lpNotif[%lu].info.statobj.lpEntryID->abFlags[0] == %d,  %s%s%s%s%s",idx,
                        bFlags,
                      ((bFlags & MAPI_SHORTTERM)        ? "MAPI_SHORTTERM | "   : ""),
                      ((bFlags & MAPI_NOTRECIP)         ? "MAPI_NOTRECIP | "    : ""),
                      ((bFlags & MAPI_THISSESSION)      ? "MAPI_THISSESSION | " : ""),
                      ((bFlags & MAPI_NOW)              ? "MAPI_NOW | "         : ""),
                      ((bFlags & MAPI_NOTRESERVED)      ? "MAPI_NOTRESERVED "   : "0"));
                    XVPLog(xvpNum,2,szBuffer);

                    SzGetEntryID(szBuff1,lpNotif[idx].info.statobj.lpEntryID,lpNotif[idx].info.statobj.cbEntryID);
                    wsprintf(szBuffer,"lpNotif[%lu].info.statobj.lpEntryID     == %s",idx,szBuff1);
                    XVPLog(xvpNum,2,szBuffer);
                }

                wsprintf(szBuffer,"lpNotif[%lu].info.statobj.cValues       == %lu",idx,
                                lpNotif[idx].info.statobj.cValues);
                XVPLog(xvpNum,2,szBuffer);

                if(lpNotif[idx].info.statobj.lpPropVals)
                {
                    for(i = 0; i < lpNotif[idx].info.statobj.cValues ; i++)
                    {

                        SzTextizeProp(szBuff1, &lpNotif[idx].info.statobj.lpPropVals[i] );
                        wsprintf(szBuffer,"lpNotif[%lu].info.statobj.lpPropVals[%lu] == %s",idx,i,szBuff1);
                        XVPLog(xvpNum,2,szBuffer);
                    }
                }

                break;

            case fnevReservedForMapi:

                wsprintf(szBuffer,"fnevReservedForMapi, You should not be here ");
                XVPLog(xvpNum,2,szBuffer);
                break;

            default:
                wsprintf(szBuffer,"lpNotif[%lu].ulEventType UNRECOGNIZED", idx);
                XVPLog(xvpNum,2,szBuffer);
                break;
        } // end switch

    }  // end of for loop

    return(0);
}

#endif





/********************************************************************/
/********************************************************************/

//#ifdef XVPORT
/******************* Notif Message Map ****************************/

BEGIN_MESSAGE_MAP(CNotifDlg, CDialog)

    ON_COMMAND(     IDC_N_NEWADVISE,    OnNewAdvise)
    ON_COMMAND(     IDC_N_UNADVISE,     OnUnadvise)

END_MESSAGE_MAP()


/********************************************************************/
/*
 -  CNotifDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CNotifDlg::OnInitDialog()
{
    DWORD           dwReturn = 0;
    int             rgTabStops[4];

    SetWindowText( m_Description.GetBuffer(40) );

    // set size of horizontal scroll and tab stops in prop view listbox
    dwReturn = SendDlgItemMessage(IDC_N_DISPLAY,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = 50  ;
    rgTabStops[1] = 100 ;

    dwReturn = SendDlgItemMessage(IDC_N_DISPLAY,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    Display();

    return TRUE;
}

/*******************************************************************/
/*
 -  CNotifDlg::
 -  ~CNotifDlg
 -
 *  Purpose:
 *      Destructor for class CNotifDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CNotifDlg::~CNotifDlg()
{

}

/*******************************************************************/
/*
 -  CNotifDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/


void CNotifDlg::OnUnadvise()
{
    HRESULT     hResult     = hrSuccess;
    LONG        lSelection  = -1;
    CGetError   E;


    if( (lSelection = SendDlgItemMessage(IDC_N_DISPLAY,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select Connection to Unadvise", "OnUnadvise", MBS_ERROR );
        return;
    }

    switch(m_ulObjType)
    {
        case MAPI_STORE:
            hResult = ((LPMDB)m_lpUnk)->Unadvise(
                (*m_lppNotifs)->Connect[lSelection].ulConnection);
            break;
        case MAPI_TABLE:  // defined in mdbview.h
            hResult = ((LPMAPITABLE)m_lpUnk)->Unadvise(
                (*m_lppNotifs)->Connect[lSelection].ulConnection);
            break;
        case MAPI_SESSION:
        case MAPI_STATUS:
            hResult = ((LPMAPISESSION)m_lpUnk)->Unadvise(
                (*m_lppNotifs)->Connect[lSelection].ulConnection);
            break;
        case MAPI_ADDRBOOK:
            hResult = ((LPADRBOOK)m_lpUnk)->Unadvise(
                (*m_lppNotifs)->Connect[lSelection].ulConnection);
            break;
        default:
            MessageBox("UNKNOWN Obj Type", "OnUnadvise", MBS_ERROR );
            break;
    }
    if( HR_FAILED(hResult ) )
        MessageBox( E.SzError("lpObj->Unadvise()", hResult), "Client", MBS_ERROR );

    // removed from mapi, now remove from my list LPNOTIFS
    (*m_lppNotifs)->Connect[lSelection].ulConnection =
              (*m_lppNotifs)->Connect[(*m_lppNotifs)->cConnect -1].ulConnection;

    lstrcpy((*m_lppNotifs)->Connect[lSelection].szContext,
             (*m_lppNotifs)->Connect[(*m_lppNotifs)->cConnect -1].szContext);

    (*m_lppNotifs)->cConnect--;

    Display();
}

/*******************************************************************/
/*
 -  CNotifDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/


void CNotifDlg::OnNewAdvise()
{
#ifdef XVPORT
    CAdviseNotifDlg     NewAdvise(this);

    NewAdvise.m_Description     = "New Advise";
    NewAdvise.m_lppNotifs       = m_lppNotifs;
    NewAdvise.m_lpUnk           = m_lpUnk;
    NewAdvise.m_ulObjType       = m_ulObjType;

    NewAdvise.DoModal();

    Display();
#endif
}

/*******************************************************************/
/*
 -  CNotifDlg::
 -  DisplayNotif
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CNotifDlg::Display()
{
    ULONG       idx;
    char        szBuffer[256];
    DWORD       dwReturn    = 0;

    if( (!m_lppNotifs) || (!(*m_lppNotifs)) )
        return;

    SendDlgItemMessage(IDC_N_DISPLAY,LB_RESETCONTENT,0,0);

    // list all the connections currently registered
    for(idx = 0 ; idx < (*m_lppNotifs)->cConnect ; idx++)
    {
        wsprintf(szBuffer,"%lu\t",(*m_lppNotifs)->Connect[idx].ulConnection);
        lstrcat(szBuffer, (*m_lppNotifs)->Connect[idx].szContext);

        dwReturn = SendDlgItemMessage(IDC_N_DISPLAY,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    }
}


/*******************************************************************/
/*
 -  CNotifDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CNotifDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}

//#endif





/********************************************************************/
/********************************************************************/
/**************** ADVISE NOTIF **************************************/

#ifdef XVPORT

/********************************************************************/
/*
 -  CAdviseNotifDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CAdviseNotifDlg::OnInitDialog()
{
    ULONG   ulMax   = 0;
    ULONG   idx;
    DWORD   dwRet   = 0;
    char    szBuffer[80];

    SetWindowText( m_Description.GetBuffer(40) );

    ulMax = GetRowCount("NotifEvents");

    for(idx = 0; idx < ulMax ; idx++)
    {
        GetRowString("NotifEvents",(LONG) idx, (LPSTR) szBuffer);
        dwRet = SendDlgItemMessage(IDC_ADVISE_EVENT1, CB_ADDSTRING,0 ,(LPARAM)szBuffer );
        dwRet = SendDlgItemMessage(IDC_ADVISE_EVENT2, CB_ADDSTRING,0 ,(LPARAM)szBuffer );
        dwRet = SendDlgItemMessage(IDC_ADVISE_EVENT3, CB_ADDSTRING,0 ,(LPARAM)szBuffer );
        dwRet = SendDlgItemMessage(IDC_ADVISE_EVENT4, CB_ADDSTRING,0 ,(LPARAM)szBuffer );
        dwRet = SendDlgItemMessage(IDC_ADVISE_EVENT5, CB_ADDSTRING,0 ,(LPARAM)szBuffer );
    }

    SetDlgItemText(IDC_ADVISE_CONTEXT,"Your Context string here");

    return TRUE;
}

/*******************************************************************/
/*
 -  CAdviseNotifDlg::
 -  ~CAdviseNotifDlg
 -
 *  Purpose:
 *      Destructor for class CAdviseNotifDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CAdviseNotifDlg::~CAdviseNotifDlg()
{

}
/*******************************************************************/
/*
 -  CAdviseNotifDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CAdviseNotifDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}

/*******************************************************************/
/*
 -  CAdviseNotifDlg::
 -  OnOK
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CAdviseNotifDlg::OnOK()
{
    char                szBuffer[512];
    DWORD               dRet            = 0;
    LONG                lCurSelect      = -1;
    ULONG               ulEvents        = 0;
    HRESULT             hResult         = hrSuccess;
    LPMAPIADVISESINK    lpAdviseSink    = NULL;
    LPNOTIFCONTEXT      lpContext       = NULL;
    ULONG               ulConnection    = 0;
    CGetError   E;
    LPNOTIFS            lpNotifs        = NULL;


    if( (lCurSelect = SendDlgItemMessage(IDC_ADVISE_EVENT1,CB_GETCURSEL,0,0)) >= 0 )
        ulEvents |= GetRowID("NotifEvents",lCurSelect);

    if( (lCurSelect = SendDlgItemMessage(IDC_ADVISE_EVENT2,CB_GETCURSEL,0,0)) >= 0 )
        ulEvents |= GetRowID("NotifEvents",lCurSelect);

    if( (lCurSelect = SendDlgItemMessage(IDC_ADVISE_EVENT3,CB_GETCURSEL,0,0)) >= 0 )
        ulEvents |= GetRowID("NotifEvents",lCurSelect);

    if( (lCurSelect = SendDlgItemMessage(IDC_ADVISE_EVENT4,CB_GETCURSEL,0,0)) >= 0 )
        ulEvents |= GetRowID("NotifEvents",lCurSelect);

    if( (lCurSelect = SendDlgItemMessage(IDC_ADVISE_EVENT5,CB_GETCURSEL,0,0)) >= 0 )
        ulEvents |= GetRowID("NotifEvents",lCurSelect);

    *(WORD *)szBuffer = sizeof(szBuffer) -1;    // first char has buffer length
    dRet = SendDlgItemMessage(IDC_ADVISE_CONTEXT,EM_GETLINE,0,(LPARAM)szBuffer);
    szBuffer[dRet] = '\0';

    lpContext = (LPNOTIFCONTEXT) PvAlloc(sizeof(NOTIFCONTEXT) );
    lpContext->ulCount = 0;
    lstrcpy(lpContext->szComment,szBuffer);

    hResult = HrAllocAdviseSink(    (LPNOTIFCALLBACK)       LogNotifToXVPLog,
                                    (LPVOID)                lpContext,
                                    (LPMAPIADVISESINK FAR *) &lpAdviseSink);
    if(HR_FAILED(hResult) )
    {
        MessageBox( E.SzError("HrAllocAdviseSink()", hResult),
                 "Client", MBS_ERROR );
        return;
    }

    switch(m_ulObjType)
    {
        case MAPI_STORE:
            hResult = ((LPMDB)m_lpUnk)->Advise(
                m_cbEntryID,
                m_lpEntryID,
                ulEvents,
                lpAdviseSink,
                &ulConnection);
            break;
        case MAPI_TABLE:  // defined in mdbview.h
            hResult = ((LPMAPITABLE)m_lpUnk)->Advise(
                ulEvents,
                lpAdviseSink,
                &ulConnection);
            break;
        case MAPI_SESSION:
        case MAPI_STATUS:
            hResult = ((LPMAPISESSION)m_lpUnk)->Advise(
                m_cbEntryID,
                m_lpEntryID,
                ulEvents,
                lpAdviseSink,
                &ulConnection);
            break;
        case MAPI_ADDRBOOK:
            hResult = ((LPADRBOOK)m_lpUnk)->Advise(
                m_cbEntryID,
                m_lpEntryID,
                ulEvents,
                lpAdviseSink,
                &ulConnection);
            break;
        default:
            MessageBox( "UNKNOWN object type", "Client", MBS_ERROR );
            break;
    }
    if( HR_FAILED(hResult ) )
    {
        MessageBox( E.SzError("lpObj->Advise()", hResult), "Client", MBS_ERROR );
        return;
    }

    // if no notif list is currently allocated, allocate it now
    if(!m_lppNotifs)
    {
        lpNotifs = (LPNOTIFS) PvAlloc(sizeof(NOTIFS) );
        m_lppNotifs = &lpNotifs;
        (*m_lppNotifs)->cConnect = 0;
    }

    (*m_lppNotifs)->cConnect++;

    // now add to my list
    (*m_lppNotifs)->Connect[(*m_lppNotifs)->cConnect - 1].ulConnection = ulConnection;
    lstrcpy((*m_lppNotifs)->Connect[(*m_lppNotifs)->cConnect - 1].szContext,szBuffer);

    if(lpAdviseSink)
        lpAdviseSink->Release();

//    if(lpContext)
//        PvFree(lpContext);

    EndDialog(IDOK);

}

#endif




