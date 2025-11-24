/*******************************************************************/
/*
 -  fld.cpp
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CFolderDlg
 */
/*******************************************************************/

#undef  CINTERFACE      // C++ calling convention for mapi calls


#define XVPORT

#ifdef WIN32
#ifdef _WIN95
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
#ifdef _WIN95
#include <ole2.h>
#endif
#endif

#ifdef WIN16
#include <mapiwin.h>
#endif
#include <mapix.h>
#include <mapiguid.h>
#include <strtbl.h>
#include <pvalloc.h>
#include <misctool.h>
#include "resource.h"
#include "mdbview.h"
#include "fld.h"
#include "msg.h"
#include "oper.h"
#include "restrict.h"



/*******************************************************************/
/*********************** Function Prototypes for Notif Callbacks ***/

#ifdef XVPORT
extern "C"
{
extern NOTIFCALLBACK LogNotifToXVPLog;
}
#endif

extern "C"
{
extern NOTIFCALLBACK RedrawHeirarchyTable;
}


extern "C"
{
extern NOTIFCALLBACK RedrawContentsTable;
}


extern "C"
{
extern NOTIFCALLBACK RedrawAssMsgTable;
}


/* TBLVU functions */

extern  HINSTANCE    hlibTBLVU;

typedef BOOL (*LPFNVIEWMAPITABLE)(
    LPMAPITABLE FAR *lppMAPITable,
    HWND hWnd);

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

/********************** Global Variables external declaration *******/

extern LPMAPISESSION        lpSession;
extern LPMDB                lpMDB;
extern LPADRBOOK            lpAdrBook;
extern CTheApp              theApp;
extern HINSTANCE            hInst;
extern HRESULT              hResultSession;
extern LPVOID               lpvCopyToDest;
extern ULONG                cFolders;
extern LPENTRYLIST          lpEntryListSearchFolder;
extern ULONG                ulAccess;

// PropTag Array used by GetProps for getting subject and entryid

SizedSPropTagArray(2,sptaSubject) =
{
    2,
    {
        PR_SUBJECT,
        PR_ENTRYID,
    }
};



/******************* Fld Message Map ****************************/

BEGIN_MESSAGE_MAP(CFolderDlg, CDialog)

    ON_LBN_DBLCLK(  IDC_PROPS,          OnPropInterface)
    ON_LBN_DBLCLK(  IDC_FLDCHILD,       OnOpenChildFolder)
    ON_LBN_DBLCLK(  IDC_FLDMESSAGES,    OnOpenMessage)
    ON_LBN_DBLCLK(  IDC_FLD_ASS,        OnOpenAssociated)
    ON_COMMAND(     IDC_CALLFUNC,       OnCallFunction)
    ON_COMMAND(     IDC_PROPINTERFACE,  OnPropInterface)
    ON_COMMAND(     IDC_SPECIALPROPS,   OnSpecialProps)
                                        
END_MESSAGE_MAP() 


/********************************************************************/
/********************** CFolderDlg Member Functions ****************/

/********************************************************************/
/********************* Callbacks ************************************/

SCODE STDAPICALLTYPE RedrawHeirarchyTable(  LPVOID          lpvContext,
                                            ULONG           cNotif,
                                            LPNOTIFICATION  lpNotif)
{
    ((CFolderDlg *)(lpvContext))->RedrawFolderTable();
    return(S_OK);
}

/********************************************************************/

SCODE STDAPICALLTYPE RedrawContentsTable(  LPVOID          lpvContext,
                                            ULONG           cNotif,
                                            LPNOTIFICATION  lpNotif)
{
    ((CFolderDlg *)(lpvContext))->RedrawMessageTable();
    return(S_OK);
}

/********************************************************************/


SCODE STDAPICALLTYPE RedrawAssMsgTable(  LPVOID          lpvContext,
                                            ULONG           cNotif,
                                            LPNOTIFICATION  lpNotif)
{
    ((CFolderDlg *)(lpvContext))->RedrawAssociatedTable();
    return(S_OK);
}

/********************************************************************/
/*
 -  CFolderDlg::
 -  OnPropInterface
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnPropInterface()
{
    char    szBuffer[80];

    lstrcpy(szBuffer,m_CurrentFolder.GetBuffer(50));
    lstrcat(szBuffer," Properties");

    PROPVUViewMapiProp(szBuffer,
                (LPMAPIPROP FAR *)&m_lpEntry,lpvCopyToDest, (HWND)this->m_hWnd );
}


/********************************************************************/
/*
 -  CFolderDlg::
 -  OnCallFunction
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnCallFunction()
{
    LONG    lCurSelect = CB_ERR;
    
    // get the selection from the Drop Down Listbox combo box
    // and switch to appropriate function selected based upon 
    // position in the combo box. (NOTE position is determined by
    // initialization from OnInitFld  order of adding to combobox
    
    // GetCurrentSelection Position
    lCurSelect = SendDlgItemMessage(IDC_FUNCTIONS,CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0 );
    if(lCurSelect == CB_ERR)
    {
        // no item was selected, bring up dialog to tell them to select something
        MessageBox( "Please Select a function in the adjacent drop down listbox to call",
                "Client", MBS_OOPS );
        return;
   
    }
    // else it is a valid index in combo box

    // Switch to appropriate function
    switch(lCurSelect)
    {
        // FOLDER FUNCTIONS
        case 0:        
            OnPropInterface();            // lpFld->PROPERTY (OF CURRENT FLD)
            break;

        // Tables
        case 1:
            OnContTable();          // lpFld->GetContentsTable() (OF CURRENT FLD)
            break;
        case 2:
            OnHeirTable();          // lpFld->GetHeirarchyTable() (OF CURRENT FLD)
            break;

        // opening objects
        case 3:
            OnOpenChildFolder();    // lpFld->OpenEntry() -- Child Folder (ON SELECTED FLD)
            break;
        case 4:
            OnOpenMessage();        // lpFld->OpenEntry() -- Message (ON SELECTED MSG)
            break;
        case 5:
            OnOpenAssociated();     // lpFld->OpenEntry() -- Associated Msg (ON SELECTED MSG)
            break;
        
        // creating objects
        case 6:
            OnMsgCreate();          // lpFld->CreateMessage() (UNDER CURRENT FLD)
            break;
        case 7:
            OnFldCreate();          // lpFld->CreateFolder() (UNDER CURRENT FLD)
            break;
        case 8:
            OnCopyFolder();         // lpFld->CopyFolder() (CURRENT FLD to DEST)
            break;
        case 9:
            OnCopyMessages();       // lpFld->CopyMessages() (ON SELECTED MSGS)
            break;
        
        // Deleting objects
        case 10:
            OnFldDelete();          // lpFld->DeleteFolder() (ON SELECTED FLD)
            break;
        case 11:    
            OnMsgDelete();          // lpFld->DeleteMessages() (ON SELECTED MSGS)
            break;
        case 12:
            OnEmptyFolder();        // lpFld->EmptyFolder()  (ON CURRENT FLD)
            break;  

        // misc 
        case 13:
            OnGetSearchCriteria();  // lpFld->GetSearchCriteria() (ON CURRENT FLD)
            break;
        case 14:
            OnSetSearchCriteria();  // lpFld->SetSearchCriteria() (ON CURRENT FLD)
            break;
        case 15:
            OnSetMsgStatus();       // lpFld->SetMsgStatus() (ON SELECTED MSG)
            break;
        case 16:
            OnGetMsgStatus();       // lpFld->GetMsgStatus() (ON SELECTED MSG)
            break;
        case 17:
            OnAbortSubmit();        // lpMDB->AbortSubmit() (ON SELECTED MSG)
            break;

        // MDB FUNCTIONS
        case 18:
            OnMDBProp();            // lpMDB->PROPERTY (OF CURRENT MDB)
            break;  
        case 19:
            OnSetReceiveFolder();   // lpMDB->SetReceiveFolder() (ON CURRENT FLD)
            break;

        // NOTIFICATIONS
        case 20:
            OnNotifFld();           // lpFld->Adivse() / UnAdvise (NOTIFICATIONS)
            break;
        case 21:
            OnNotifHeir();          // lpHeirTbl->Adivse() / UnAdvise (NOTIFICATIONS)
            break;
        case 22:
            OnNotifAss();           // lpAssContTbl->Adivse() / UnAdvise (NOTIFICATIONS)
            break;
        case 23:
            OnNotifCont();          // lpContTbl->Adivse() / UnAdvise (NOTIFICATIONS)
            break;
        
        // INTERNAL POINTERS
        case 24:
            OnSetCopyToDest();      // SET THIS FOLDER AS COPY DESTINATION
            break;
        case 25:
            OnSetSearchDestFlds();  // SET THIS FOLDER TO BE IN LIST OF SEARCH FOLDERS
            break;

        // DEFAULT
        default:
            MessageBox( "CFolderDlg::OnCallFunction() default ",
                "Client", MBS_OOPS );
            break;       
    }    

}


/********************************************************************/
/*
 -  CFolderDlg::
 -  OnNotifFld
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnNotifFld()
{
    CNotifDlg   Notif(this);

    Notif.m_Description = m_CurrentFolder;
    Notif.m_lpUnk       = lpMDB;
    Notif.m_ulObjType   = MAPI_STORE;
    Notif.m_lppNotifs   = &m_lpNotifFld;
    Notif.DoModal();

}
/********************************************************************/
/*
 -  CFolderDlg::
 -  OnNotifHeir
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnNotifHeir()
{
    CNotifDlg   Notif(this);

    Notif.m_Description = "HeirarchyTable Notification";
    Notif.m_lpUnk       = m_lpChildTable;
    Notif.m_ulObjType   = MAPI_TABLE;
    Notif.m_lppNotifs   = &m_lpNotifHeirTbl;
    Notif.DoModal();

}
/********************************************************************/
/*
 -  CFolderDlg::
 -  OnNotifAss
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnNotifAss()
{
    CNotifDlg   Notif(this);

    Notif.m_Description = "Associated ContentsTable Notification";
    Notif.m_lpUnk       = m_lpAssociatedTable;
    Notif.m_ulObjType   = MAPI_TABLE;
    Notif.m_lppNotifs   = &m_lpNotifAssTbl;
    Notif.DoModal();

}
/********************************************************************/
/*
 -  CFolderDlg::
 -  OnNotifCont
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnNotifCont()
{
    CNotifDlg   Notif(this);

    Notif.m_Description = "ContentsTable Notification";
    Notif.m_lpUnk       = m_lpMessageTable;
    Notif.m_ulObjType   = MAPI_TABLE;
    Notif.m_lppNotifs   = &m_lpNotifContTbl;
    Notif.DoModal();
}

/********************************************************************/
/*
 -  CFolderDlg::
 -  OnSetMsgStatus
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnSetMsgStatus()
{
    COperation      SetMessageStatusDlg(this);
    CString         OperName;
    char            szBuff[512];
    ULONG           ulFlags     = 0;
    LPENTRYID       lpEntryID   = NULL;
    ULONG           ulEntryID   = 0;
    LONG            lCurSelect  = -1;
    LPSRowSet       lpRows      = NULL;
    LONG            lRowsSeeked = 0;
    int max=500;
    int rgItems[500];
    DWORD           dRet        = 0;
    char            *szEnd      = NULL;
    ULONG           ulTemp      = 0;
    ULONG           ulNewStatus = 0;
    ULONG           ulNewStatusMask = 0;
    ULONG           ulOldStatus = 0;


    // initalize data for dialog box
    OperName  = m_CurrentFolder;
    OperName += "->SetMessageStatus()";

    SetMessageStatusDlg.m_CurrentOperation= OperName;
    SetMessageStatusDlg.m_CBText1         = "lpEntryID/cbEntryID";
    SetMessageStatusDlg.m_EditText1       = "Message Subject";
    SetMessageStatusDlg.m_EditText2       = "ulNewStatus(Hex)";
    SetMessageStatusDlg.m_EditText3       = "ulNewStatusMask(Hex)";
    SetMessageStatusDlg.m_FlagText1       = "MSGSTATUS_HIGHLIGHTED";
    SetMessageStatusDlg.m_FlagText2       = "MSGSTATUS_TAGGED";
    SetMessageStatusDlg.m_FlagText3       = "MSGSTATUS_HIDDEN";
    SetMessageStatusDlg.m_FlagText4       = "MSGSTATUS_DELMARKED";
    SetMessageStatusDlg.m_FlagText5       = "MSGSTATUS_REMOTE_DOWNLOAD";
    SetMessageStatusDlg.m_FlagText6       = "MSGSTATUS_REMOTE_DELETE";

    SetMessageStatusDlg.m_EditDefault2 = "0x00000000";
    SetMessageStatusDlg.m_EditDefault3 = "0xFFFF001F";

    // initalize data for dialog box
    if( (lCurSelect = SendDlgItemMessage(IDC_FLDMESSAGES,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
            goto Associated;

    m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);
    wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpMessageTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpMessageTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    // rip out info
    if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
        SetMessageStatusDlg.m_EditDefault1 = lpRows->aRow[0].lpProps[0].Value.lpszA;

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;

        SzGetPropValue(szBuff, &(lpRows->aRow[0].lpProps[1]));
        dRet = SetMessageStatusDlg.m_CBContents1.Add(szBuff);

    }

    // bring up modal dialog box, and if user hits OK, process operation
    if( SetMessageStatusDlg.DoModal() == IDOK )
    {

        if( SetMessageStatusDlg.m_bFlag1 )
            ulNewStatus |= MSGSTATUS_HIGHLIGHTED;

        if( SetMessageStatusDlg.m_bFlag2 )
            ulNewStatus |= MSGSTATUS_TAGGED;

        if( SetMessageStatusDlg.m_bFlag3 )
            ulNewStatus |= MSGSTATUS_HIDDEN;

        if( SetMessageStatusDlg.m_bFlag4 )
            ulNewStatus |= MSGSTATUS_DELMARKED;

        if( SetMessageStatusDlg.m_bFlag5 )
            ulNewStatus |= MSGSTATUS_REMOTE_DOWNLOAD;

        if( SetMessageStatusDlg.m_bFlag6 )
            ulNewStatus |= MSGSTATUS_REMOTE_DELETE;

        // now logical or this with the ulNewStatus from the Edit Control
        ulTemp = strtoul( SetMessageStatusDlg.m_szEdit2 , &szEnd,16);
        ulNewStatus |= ulTemp;

        ulNewStatusMask = strtoul( SetMessageStatusDlg.m_szEdit3 , &szEnd,16);

        // m_lpFolder is global variable
        m_hResult = m_lpFolder->SetMessageStatus(
                        (ULONG)     ulEntryID,
                        (LPENTRYID) lpEntryID,
                        ulNewStatus,
                        ulNewStatusMask,
                        &ulOldStatus  );
        wsprintf(m_szLogBuf,"m_lpFolder->SetMessageStatus(%lu,0x%X,%lu%lu,&ulOldStatus)\t SC: %s",
                        ulEntryID,lpEntryID,ulNewStatus,ulNewStatusMask,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->SetMessageStatus()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
        wsprintf(szBuff,"ulOldStatus == %#04X, %s%s%s%s",ulOldStatus,
          ((ulOldStatus & MSGSTATUS_HIGHLIGHTED)    ? "MSGSTATUS_HIGHLIGHTED | ": ""),
          ((ulOldStatus & MSGSTATUS_TAGGED)         ? "MSGSTATUS_TAGGED | "     : ""),
          ((ulOldStatus & MSGSTATUS_HIDDEN)         ? "MSGSTATUS_HIDDEN | "     : ""),
          ((ulOldStatus & MSGSTATUS_DELMARKED)      ? "MSGSTATUS_DELMARKED | "  : ""));

        MessageBox( szBuff,"Client", MBS_INFO );
    }

    goto Cleanup;

Associated:


    lCurSelect  = -1;

    // initalize data for dialog box
    if( (lCurSelect = SendDlgItemMessage(IDC_FLD_ASS,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
    {
            MessageBox("Select Only One Message in Listbox to Set Status", "OnSetMessageStatus", MBS_ERROR );
            goto Cleanup;
    }

    // ASSOCIATED MESSAGES
    m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpAssociatedTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpAssociatedTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    // rip out info
    if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
        SetMessageStatusDlg.m_EditDefault1 = lpRows->aRow[0].lpProps[0].Value.lpszA;

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;

        SzGetPropValue(szBuff, &(lpRows->aRow[0].lpProps[1]));
        dRet = SetMessageStatusDlg.m_CBContents1.Add(szBuff);
    }

    // bring up modal dialog box, and if user hits OK, process operation
    if( SetMessageStatusDlg.DoModal() == IDOK )
    {
        if( SetMessageStatusDlg.m_bFlag1 )
            ulNewStatus |= MSGSTATUS_HIGHLIGHTED;

        if( SetMessageStatusDlg.m_bFlag2 )
            ulNewStatus |= MSGSTATUS_TAGGED;

        if( SetMessageStatusDlg.m_bFlag3 )
            ulNewStatus |= MSGSTATUS_HIDDEN;

        if( SetMessageStatusDlg.m_bFlag4 )
            ulNewStatus |= MSGSTATUS_DELMARKED;

        // now logical or this with the ulNewStatus from the Edit Control
        ulTemp = strtoul( SetMessageStatusDlg.m_szEdit2 , &szEnd,16);
        ulNewStatus |= ulTemp;

        ulNewStatusMask = strtoul( SetMessageStatusDlg.m_szEdit3 , &szEnd,16);

        // m_lpFolder is global variable
        m_hResult = m_lpFolder->SetMessageStatus(
                        (ULONG)     ulEntryID,
                        (LPENTRYID) lpEntryID,
                        ulNewStatus,
                        ulNewStatusMask,
                        &ulOldStatus  );
        wsprintf(m_szLogBuf,"m_lpFolder->SetMessageStatus(%lu,0x%X,%lu%lu,&ulOldStatus)\t SC: %s",
                        ulEntryID,lpEntryID,ulNewStatus,ulNewStatusMask,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->SetMessageStatus()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }

        wsprintf(szBuff,"ulOldStatus == %#04X, %s%s%s%s",ulOldStatus,
          ((ulOldStatus & MSGSTATUS_HIGHLIGHTED)    ? "MSGSTATUS_HIGHLIGHTED | ": ""),
          ((ulOldStatus & MSGSTATUS_TAGGED)         ? "MSGSTATUS_TAGGED | "     : ""),
          ((ulOldStatus & MSGSTATUS_HIDDEN)         ? "MSGSTATUS_HIDDEN | "     : ""),
          ((ulOldStatus & MSGSTATUS_DELMARKED)      ? "MSGSTATUS_DELMARKED | "  : ""));

        MessageBox( szBuff,"Client", MBS_INFO );


    }


Cleanup:

    FreeRowSet(lpRows);
}


/********************************************************************/
/*
 -  CFolderDlg::
 -  OnGetMsgStatus
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnGetMsgStatus()
{
    char            szBuff[512];
    ULONG           ulFlags     = 0;
    LPENTRYID       lpEntryID   = NULL;
    ULONG           ulEntryID   = 0;
    LONG            lCurSelect  = -1;
    LPSRowSet       lpRows      = NULL;
    LONG            lRowsSeeked = 0;
    int max=500;
    int rgItems[500];
    ULONG           ulMessageStatus = 0;



    if( (lCurSelect = SendDlgItemMessage(IDC_FLDMESSAGES,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
            goto Associated;

    m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpMessageTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpMessageTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;
    }


    m_hResult = m_lpFolder->GetMessageStatus(
                        (ULONG)     ulEntryID,
                        (LPENTRYID) lpEntryID,
                        ulFlags,
                        &ulMessageStatus  );
    wsprintf(m_szLogBuf,"lpFolder->GetMessageStatus(%lu,0x%X,%lu,&ulMsgStatus)\t SC: %s",
                        ulEntryID,lpEntryID,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("lpFolder->GetMessageStatus()", m_hResult),
                "Client", MBS_ERROR );
        goto Cleanup;
    }

    wsprintf(szBuff,"ulMsgStatus == %#04X, %s%s%s%s",ulMessageStatus,
      ((ulMessageStatus & MSGSTATUS_HIGHLIGHTED)    ? "MSGSTATUS_HIGHLIGHTED | ": ""),
      ((ulMessageStatus & MSGSTATUS_TAGGED)         ? "MSGSTATUS_TAGGED | "     : ""),
      ((ulMessageStatus & MSGSTATUS_HIDDEN)         ? "MSGSTATUS_HIDDEN | "     : ""),
      ((ulMessageStatus & MSGSTATUS_DELMARKED)      ? "MSGSTATUS_DELMARKED | "  : ""));

    MessageBox( szBuff,"Client", MBS_INFO );

    goto Cleanup;

Associated:


    lCurSelect  = -1;

    // initalize data for dialog box
    if( (lCurSelect = SendDlgItemMessage(IDC_FLD_ASS,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
    {
            MessageBox("Select Only One Message in Listbox to GetMessageStatus", "OnGetMessageStatus", MBS_ERROR );
            goto Cleanup;
    }

    // ASSOCIATED MESSAGES

    m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpAssociatedTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpAssociatedTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;
    }

    m_hResult = m_lpFolder->GetMessageStatus(
                        (ULONG)     ulEntryID,
                        (LPENTRYID) lpEntryID,
                        ulFlags,
                        &ulMessageStatus  );
    wsprintf(m_szLogBuf,"lpFolder->GetMessageStatus(%lu,0x%X,%lu,&ulMsgStatus)\t SC: %s",
                        ulEntryID,lpEntryID,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("lpFolder->GetMessageStatus()", m_hResult),
                "Client", MBS_ERROR );
        goto Cleanup;
    }

    wsprintf(szBuff,"ulMsgStatus == %#04X, %s%s%s%s",ulMessageStatus,
      ((ulMessageStatus & MSGSTATUS_HIGHLIGHTED)    ? "MSGSTATUS_HIGHLIGHTED | ": ""),
      ((ulMessageStatus & MSGSTATUS_TAGGED)         ? "MSGSTATUS_TAGGED | "     : ""),
      ((ulMessageStatus & MSGSTATUS_HIDDEN)         ? "MSGSTATUS_HIDDEN | "     : ""),
      ((ulMessageStatus & MSGSTATUS_DELMARKED)      ? "MSGSTATUS_DELMARKED | "  : ""));

    MessageBox( szBuff,"Client", MBS_INFO );

Cleanup:

    FreeRowSet(lpRows);

}


/********************************************************************/
/*
 -  CFolderDlg::
 -  OnMDBProp
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnMDBProp()
{
    PROPVUViewMapiProp("MDB Properties",
            (LPMAPIPROP FAR *)&lpMDB, lpvCopyToDest, (HWND)this->m_hWnd );
}

/********************************************************************/
/*
 -  CFolderDlg::
 -  OnContTable
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnContTable()
{
    COperation      GetContentsDlg(this);
    ULONG           ulFlags         = 0;
    LPSPropTagArray lpsptaAll       = NULL;
    LONG            lRowsSeeked     = 0;

    // determine the most common properties in messages for
    // contents table.  

    SizedSPropTagArray(21,sptaMessage) =
    {
        21,
        {
            PR_SUBJECT,
            PR_NORMALIZED_SUBJECT,
            PR_ENTRYID,
            PR_OBJECT_TYPE,
            PR_MESSAGE_CLASS,
            PR_MESSAGE_FLAGS,
            PR_MESSAGE_SIZE,            
            PR_CLIENT_SUBMIT_TIME,
            PR_MESSAGE_DELIVERY_TIME,
            PR_SENDER_NAME,
            PR_SENDER_ENTRYID,
            PR_PRIORITY,
            PR_IMPORTANCE,
            PR_DISPLAY_TO,
            PR_DISPLAY_CC,
            PR_BODY,
            PR_HASATTACH,
            PR_RECORD_KEY,
            PR_SEARCH_KEY,
            PR_STORE_ENTRYID,
            PR_STORE_RECORD_KEY,
        }
    };

    GetContentsDlg.m_CurrentOperation= "lpFld->GetContentsTable()";
    GetContentsDlg.m_FlagText1       = "MAPI_ASSOCIATED";
    GetContentsDlg.m_FlagText2       = "Invalid Flag";

    if( GetContentsDlg.DoModal() == IDOK )
    {
        if( GetContentsDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        if( GetContentsDlg.m_bFlag1 )
        {
            // assocated messages in folder
            ulFlags |= MAPI_ASSOCIATED;

            m_hResult = m_lpAssociatedTable->SetColumns( (LPSPropTagArray) &sptaMessage, 0);
            wsprintf(m_szLogBuf,"lpAssociatedTable->SetColumns( &sptaMessage, 0)\t SC: %s",
                                  GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpAssociatedTable->SetColumns", m_hResult),
                                    "Client", MBS_ERROR );
                return;
            }

            m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

            wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                                GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                                    "Client", MBS_ERROR );
                return;
            }

            TBLVUViewMapiTable( (LPMAPITABLE FAR *)&m_lpAssociatedTable, (HWND)this->m_hWnd );

            // set which columns are important to see in table
            m_hResult = m_lpAssociatedTable->SetColumns( (LPSPropTagArray) &sptaSubject, 0);
            wsprintf(m_szLogBuf,"lpAssociatedTable->SetColumns( &sptaSubject, 0)\t SC: %s",
                                GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpAssociatedTable->SetColumns", m_hResult),
                            "Client", MBS_ERROR );
                return;
            }


        }
        else
        {

            // set columns all columns
            m_hResult = m_lpMessageTable->SetColumns( (LPSPropTagArray) &sptaMessage, 0);
            wsprintf(m_szLogBuf,"lpMessageTable->SetColumns( &sptaMessage, 0)\t SC: %s",
                                GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpMessageTable->SetColumns", m_hResult),
                                    "Client", MBS_ERROR );
                return;
            }

            m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

            wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                                GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                                    "Client", MBS_ERROR );
                return;
            }

            TBLVUViewMapiTable( (LPMAPITABLE FAR *)&m_lpMessageTable, (HWND)this->m_hWnd );


            // set which columns are important to see in table
            m_hResult = m_lpMessageTable->SetColumns( (LPSPropTagArray) &sptaSubject, 0);
            wsprintf(m_szLogBuf,"lpMessageTable->SetColumns( &sptaSubject, 0)\t SC: %s",
                                GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpMessageTable->SetColumns", m_hResult),
                            "Client", MBS_ERROR );
                return;
            }

        }
    }
}

/********************************************************************/
/*
 -  CFolderDlg::
 -  OnHeirTable
 -
 *  Purpose:
 *
 /********************************************************************/

void CFolderDlg::OnHeirTable()
{
    LPSPropTagArray lpsptaAll       = NULL;
    LONG            lRowsSeeked     = 0;

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };


    // query all columns
    m_hResult = m_lpChildTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll);
    wsprintf(m_szLogBuf,"lpChildTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->QueryColumns", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    // set columns all columns
    m_hResult = m_lpChildTable->SetColumns( (LPSPropTagArray) lpsptaAll, 0);
    wsprintf(m_szLogBuf,"lpChildTable->SetColumns( &sptaAll, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SetColumns", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpChildTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpChildTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    TBLVUViewMapiTable( (LPMAPITABLE FAR *)&m_lpChildTable, (HWND)this->m_hWnd );

    // set which columns are important to see in table
    m_hResult = m_lpChildTable->SetColumns( (LPSPropTagArray) &sptaDisplayName, 0);
    wsprintf(m_szLogBuf,"lpChildTable->SetColumns( &sptaDisplayName, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SetColumns", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

Error:

    // free Memory allocated
    if(lpsptaAll)
        MAPIFreeBuffer(lpsptaAll);
}

/********************************************************************/
/*
 -  CFolderDlg::
 -  OnInitFldDialog
 -
 *  Purpose:
 *
 /********************************************************************/

BOOL CFolderDlg::InitFldDialog()
{
    DWORD           dwIndex         = 0;
    DWORD           dwReturn        = 0;
    LPSPropValue    lpspvaEID       = NULL;
    ULONG           ulValuesEID     = 0;
    LPSPropValue    lpspvaFldType   = NULL;
    ULONG           ulValuesFldType = 0;

    SizedSPropTagArray(1,sptaEID) =
    {
        1,
        {
            PR_ENTRYID,
        }
    };


    SizedSPropTagArray(1,sptaFldType) =
    {
        1,
        {
            PR_FOLDER_TYPE,
        }
    };

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };



    SetWindowText( m_CurrentFolder.GetBuffer(15) );

    // load current folder string into Current Folder Listbox
//    dwIndex = SendDlgItemMessage(IDC_FLDCURRENT,LB_RESETCONTENT,0,0);
//    dwIndex = SendDlgItemMessage(IDC_FLDCURRENT,LB_ADDSTRING,0,
//                        (LPARAM)m_CurrentFolder.GetBuffer(20) );

    // disable the system menu close item
    //  this is done because there is a MFC 2.0 bug that
    //  makes you capture several PostNcDestroy messages etc.
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED);


    // FLD OBJECT NOTIFICATION
    m_hResult = m_lpFolder->GetProps( (LPSPropTagArray) &sptaEID,0, &ulValuesEID, &lpspvaEID);
    wsprintf(m_szLogBuf,"lpFolder->GetProps( &sptaEID,0, &ulValuesEID, &lpspvaEID)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("lpFolder->GetProps(&sptaEID)",m_hResult),
                     "Client", MBS_ERROR );
        return FALSE;
    }

    // notif window
    wsprintf(m_szFldContextCritical,"fnevCriticalError, lpFolder %s, lpMDB",
                    m_CurrentFolder.GetBuffer(15));

#ifdef XVPORT
    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevCriticalError,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szFldContextCritical,
                NULL,
                &m_lpNotifFld);

    wsprintf(m_szFldContextCreated,"fnevObjectCreated, lpFolder %s, lpMDB",
                    m_CurrentFolder.GetBuffer(15));

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectCreated,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szFldContextCreated,
                NULL,
                &m_lpNotifFld);

    wsprintf(m_szFldContextDeleted,"fnevObjectDeleted, lpFolder %s, lpMDB",
                    m_CurrentFolder.GetBuffer(15));

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectDeleted,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szFldContextDeleted,
                NULL,
                &m_lpNotifFld);

    wsprintf(m_szFldContextModified,"fnevObjectModified, lpFolder %s, lpMDB",
                    m_CurrentFolder.GetBuffer(15));

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectModified,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szFldContextModified,
                NULL,
                &m_lpNotifFld);

    wsprintf(m_szFldContextMoved,"fnevObjectMoved, lpFolder %s, lpMDB",
                    m_CurrentFolder.GetBuffer(15));

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectMoved,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szFldContextMoved,
                NULL,
                &m_lpNotifFld);

    wsprintf(m_szFldContextCopied,"fnevObjectCopied, lpFolder %s, lpMDB",
                    m_CurrentFolder.GetBuffer(15));

    AdviseObj(  lpMDB,
                MAPI_STORE,
                LogNotifToXVPLog,
                fnevObjectCopied,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szFldContextCopied,
                NULL,
                &m_lpNotifFld);

#endif

    // SEARCH FOLDER NOTIFICATION
    m_hResult = m_lpFolder->GetProps( (LPSPropTagArray) &sptaFldType,0, &ulValuesFldType, &lpspvaFldType);
    wsprintf(m_szLogBuf,"lpFolder->GetProps( &sptaFldType,0, &ulValuesFldType, &lpspvaFldType)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("lpFolder->GetProps(&sptaFldType)",m_hResult),
                     "Client", MBS_ERROR );
    }

    if(lpspvaFldType[0].Value.l == FOLDER_SEARCH )
    {
        wsprintf(m_szSearchFldContext,"fnevSearchComplete, lpFolder %s,lpMDB",
                            m_CurrentFolder.GetBuffer(15));
#ifdef XVPORT
        AdviseObj(  lpMDB,
                    MAPI_STORE,
                    LogNotifToXVPLog,
                    fnevSearchComplete,
                    (ULONG)     lpspvaEID[0].Value.bin.cb,
                    (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                    m_szSearchFldContext,
                    NULL,
                    &m_lpNotifFld);

#endif
    }

    if(lpspvaFldType)
        MAPIFreeBuffer(lpspvaFldType);

    if(lpspvaEID)
        MAPIFreeBuffer(lpspvaEID);

    // **** CHILD FOLDERS

    m_hResult = m_lpFolder->GetHierarchyTable( 0, &m_lpChildTable );
    wsprintf(m_szLogBuf,"lpFolder->GetHierarchyTable( 0, &m_lpChildTable )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpFolder->GetHierarchyTable", m_hResult), "Client", MBS_ERROR );
        goto Contents;

    }

    // redraw routine
    AdviseObj(  m_lpChildTable,
                MAPI_TABLE,
                RedrawHeirarchyTable,
                fnevTableModified,
                0,
                NULL,
                "Folder Heirarchy Redraw",
                this,
                &m_lpNotifHeirTbl);

    // notif window
    wsprintf(m_szHeirContext,"fnevTableModified, HeirarchyTable of Folder %s, lpTable",
                            m_CurrentFolder.GetBuffer(15));

#ifdef XVPORT

    AdviseObj(  m_lpChildTable,
                MAPI_TABLE,
                LogNotifToXVPLog,
                fnevTableModified,
                0,
                NULL,
                m_szHeirContext,
                NULL,
                &m_lpNotifHeirTbl);

#endif
    // set which columns are important to see in table
    m_hResult = m_lpChildTable->SetColumns( (LPSPropTagArray) &sptaDisplayName, 0);
    wsprintf(m_szLogBuf,"lpChildTable->SetColumns( &sptaDisplayName, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SetColumns", m_hResult),
                            "Client", MBS_ERROR );
    }

    RedrawFolderTable();

    // **** MESSAGES

Contents:

    // find all Messages of this folder
    m_hResult = m_lpFolder->GetContentsTable( 0, &m_lpMessageTable );
    wsprintf(m_szLogBuf,"lpFolder->GetContentsTable( 0, &m_lpMessageTable )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpFolder->GetContentsTable", m_hResult), "Client", MBS_ERROR );
        goto Associated;
    }

    // redraw routine
    AdviseObj(  m_lpMessageTable,
                MAPI_TABLE,
                RedrawContentsTable,
                fnevTableModified,
                0,
                NULL,
                "Folder Contents Redraw",
                this,
                &m_lpNotifContTbl);

    wsprintf(m_szContContext,"fnevTableModified, ContentsTable of Folder %s, lpTable",
                            m_CurrentFolder.GetBuffer(15));

#ifdef XVPORT

    AdviseObj(  m_lpMessageTable,
                MAPI_TABLE,
                LogNotifToXVPLog,
                fnevTableModified,
                0,
                NULL,
                m_szContContext,
                NULL,
                &m_lpNotifContTbl);

#endif
    // set which columns are important to see in table
    m_hResult = m_lpMessageTable->SetColumns( (LPSPropTagArray) &sptaSubject, 0);
    wsprintf(m_szLogBuf,"lpMessageTable->SetColumns( &sptaSubject, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->SetColumns", m_hResult),
                    "Client", MBS_ERROR );
    }

    RedrawMessageTable();


Associated:

    // **** ASSOCIATED MESSAGES

    // find all Messages of this folder
    m_hResult = m_lpFolder->GetContentsTable( MAPI_ASSOCIATED , &m_lpAssociatedTable );
    wsprintf(m_szLogBuf,"lpFolder->GetContentsTable( 0, &m_lpAssociatedTable )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpFolder->GetContentsTable", m_hResult), "Client", MBS_ERROR );
        goto Setup;
    }

    // redraw routine
    AdviseObj(  m_lpAssociatedTable,
                MAPI_TABLE,
                RedrawAssMsgTable,
                fnevTableModified,
                0,
                NULL,
                "Folder Associated Contents Redraw",
                this,
                &m_lpNotifAssTbl);

    // notif window
    wsprintf(m_szAssContext,"fnevTableModified, Associated ContentsTable of Folder %s, lpTable",
                            m_CurrentFolder.GetBuffer(15));

#ifdef XVPORT

    AdviseObj(  m_lpAssociatedTable,
                MAPI_TABLE,
                LogNotifToXVPLog,
                fnevTableModified,
                0,
                NULL,
                m_szAssContext,
                NULL,
                &m_lpNotifAssTbl);
#endif

    // set which columns are important to see in table
    m_hResult = m_lpAssociatedTable->SetColumns( (LPSPropTagArray) &sptaSubject, 0);
    wsprintf(m_szLogBuf,"lpAssociatedTable->SetColumns( &sptaSubject, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->SetColumns", m_hResult),
                    "Client", MBS_ERROR );
    }

    RedrawAssociatedTable();


Setup:


    ResetPropButtons();

    RedrawPropTable();


    // set focus of the listboxs
    dwIndex = SendDlgItemMessage(IDC_FLDCHILD,      LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_PROPS,         LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,   LB_SETSEL,   (WPARAM)FALSE ,(LPARAM) -1);
    dwIndex = SendDlgItemMessage(IDC_FLD_ASS,       LB_SETSEL,   (WPARAM)FALSE ,(LPARAM) -1);

    dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,   LB_SETHORIZONTALEXTENT,
                                (WPARAM) MSG_LISTBOX_HORIZONTAL_SIZE ,0 );

    dwIndex = SendDlgItemMessage(IDC_FLD_ASS,   LB_SETHORIZONTALEXTENT,
                                (WPARAM) MSG_LISTBOX_HORIZONTAL_SIZE ,0 );



//NOTICE: THIS PRESENTS A PROBLEM !!!!!


    //***** Add Strings to call function combo box *****
    //***** Add Strings to call function combo box *****

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "Folder Properties" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->GetContentsTable()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->GetHeirarchyTable()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->OpenEntry() -- Child Folder        (ON SELECTED FLD)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
                     
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->OpenEntry() -- Message             (ON SELECTED MSG)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
    
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->OpenEntry() -- Associated Msg      (ON SELECTED MSG)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
    
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->CreateMessage()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->CreateFolder()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->CopyFolder()                       (ON SELECTED FLD)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->CopyMessages()                     (ON SELECTED MSGS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
       
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->DeleteFolder()                     (ON SELECTED FLD)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
       
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->DeleteMessages()                   (ON SELECTED MSGS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->EmptyFolder()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->GetSearchCriteria()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->SetSearchCriteria()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->SetMsgStatus()                     (ON SELECTED MSG)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->GetMsgStatus()                     (ON SELECTED MSG)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMDB->AbortSubmit()                      (ON SELECTED MSG)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "Message Store Properties" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMDB->SetReceiveFolder()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpFld->Adivse() / UnAdvise                (NOTIFICATIONS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpHeirTbl->Adivse() / UnAdvise            (NOTIFICATIONS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpAssContTbl->Adivse() / UnAdvise         (NOTIFICATIONS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpContTbl->Adivse() / UnAdvise            (NOTIFICATIONS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "INTERNAL -- SET THIS FOLDER AS COPY DESTINATION" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "INTERNAL -- SET THIS FOLDER TO BE IN LIST OF SEARCH FOLDERS" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            



    // SET CURRENT SELECTION IN COMBO BOX TO 1ST ITEM
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_SETCURSEL,
                                (WPARAM) 0 , (LPARAM) 0 );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_SETCURSEL functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            




    return TRUE;
}

/********************************************************************/
/*
 -  CFolderDlg::
 -  RedrawFolderTable
 -
 *  Purpose:
 *      Refresh view of Hierarchytable in this folder
 *
 /********************************************************************/

void CFolderDlg::RedrawFolderTable()
{
    ULONG           cColumn         = 0;
    int             iRow            = 0;
    DWORD           dwIndex         = 0;
    LPSRowSet       lpRows          = NULL;
    LONG            lRowsSeeked     = 0;
    ULONG           ulRowCount      = 0;

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };


    dwIndex = SendDlgItemMessage(IDC_FLDCHILD,LB_RESETCONTENT,0,0);

    m_hResult = m_lpChildTable->GetRowCount(0,&ulRowCount);
    wsprintf(m_szLogBuf,"lpChildTable->GetRowCount(0,&ulRowCount)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->GetRowCount()", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }

    if( !ulRowCount)
        return;

    m_hResult = m_lpChildTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpChildTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpChildTable->QueryRows( ulRowCount, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpChildTable->QueryRows( %lu, 0, &lpRows )\t SC: %s",
                        ulRowCount,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( !HR_FAILED(m_hResult) )
    {
        for(iRow = 0; iRow < lpRows->cRows; iRow++)
        {
            if(lpRows->aRow[iRow].lpProps[0].ulPropTag == PR_DISPLAY_NAME )
            {
                dwIndex = SendDlgItemMessage(IDC_FLDCHILD,LB_ADDSTRING,0,
                            (LPARAM)lpRows->aRow[iRow].lpProps[0].Value.lpszA);
            }
        }
    }

    FreeRowSet(lpRows);

    return;

Error:
    FreeRowSet(lpRows);

    MessageBox( "RedrawFolderTable Failed","Client", MBS_ERROR );

}





/********************************************************************/
/*
 -  CFolderDlg::
 -  RedrawMessageTable
 -
 *  Purpose:
 *      Refresh view of ContentsTable in this folder
 *
 /********************************************************************/

void CFolderDlg::RedrawMessageTable()
{
    ULONG           cColumn         = 0;
    int             iRow            = 0;
    DWORD           dwIndex         = 0;
    LPSRowSet       lpRows          = NULL;
    ULONG           ulRowCount      = 0;
    char            szBuffer[256];
    LONG            lRowsSeeked     = 0;

    // NORMAL MESSAGES
    dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,LB_RESETCONTENT,0,0);

    m_hResult = m_lpMessageTable->GetRowCount(0,&ulRowCount);
    wsprintf(m_szLogBuf,"lpMessageTable->GetRowCount(0,&ulRowCount)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->GetRowCount()", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }

    if( !ulRowCount)
        return;
    m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpMessageTable->QueryRows( ulRowCount, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpMessageTable->QueryRows( %lu, 0, &lpRows )\t SC: %s",
                        ulRowCount,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( GetScode(m_hResult) == S_OK )
    {
        for(iRow = 0; iRow < lpRows->cRows; iRow++)
        {
            // if there is a subject
            if( (PROP_TYPE(lpRows->aRow[iRow].lpProps[0].ulPropTag) != PT_ERROR) &&
                (PROP_TYPE(lpRows->aRow[iRow].lpProps[0].ulPropTag) != PT_NULL ) )
            {
                dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,LB_ADDSTRING,0,
                        (LPARAM)lpRows->aRow[iRow].lpProps[0].Value.lpszA);
            }
            else
            {
                // no PR_SUBJECT, use EntryID
                SzGetPropValue(szBuffer, &(lpRows->aRow[iRow].lpProps[1]) );

                dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
            }
        }
    }

    FreeRowSet(lpRows);

    return;

Error:

    FreeRowSet(lpRows);

    MessageBox( "RedrawMessageTable Failed","Client", MBS_ERROR );

}



/********************************************************************/
/*
 -  CFolderDlg::
 -  RedrawAssociatedTable
 -
 *  Purpose:
 *      Refresh view of ContentsTable in this folder
 *
/********************************************************************/

void CFolderDlg::RedrawAssociatedTable()
{
    ULONG           cColumn         = 0;
    int             iRow            = 0;
    DWORD           dwIndex         = 0;
    LPSRowSet       lpRows          = NULL;
    ULONG           ulRowCount      = 0;
    char            szBuffer[256];
    LONG            lRowsSeeked     = 0;

    // NORMAL MESSAGES
    dwIndex = SendDlgItemMessage(IDC_FLD_ASS,LB_RESETCONTENT,0,0);

    m_hResult = m_lpAssociatedTable->GetRowCount(0,&ulRowCount);
    wsprintf(m_szLogBuf,"lpAssociatedTable->GetRowCount(0,&ulRowCount)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->GetRowCount()", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }

    if( !ulRowCount)
        return;

    m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpAssociatedTable->QueryRows( ulRowCount, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpAssociatedTable->QueryRows( %lu, 0, &lpRows )\t SC: %s",
                        ulRowCount,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( GetScode(m_hResult) == S_OK )
    {
        for(iRow = 0; iRow < lpRows->cRows; iRow++)
        {
            // if there is a subject
            if( (PROP_TYPE(lpRows->aRow[iRow].lpProps[0].ulPropTag) != PT_ERROR) &&
                (PROP_TYPE(lpRows->aRow[iRow].lpProps[0].ulPropTag) != PT_NULL ) )
            {
                dwIndex = SendDlgItemMessage(IDC_FLD_ASS,LB_ADDSTRING,0,
                        (LPARAM)lpRows->aRow[iRow].lpProps[0].Value.lpszA);
            }
            else
            {
                // no PR_SUBJECT, use EntryID
                SzGetPropValue(szBuffer, &(lpRows->aRow[iRow].lpProps[1]) );

                dwIndex = SendDlgItemMessage(IDC_FLD_ASS,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
            }
        }
    }

    FreeRowSet(lpRows);

    return;

Error:

    FreeRowSet(lpRows);

    MessageBox( "RedrawAssociatedTable Failed","Client", MBS_ERROR );
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnSetSearchDestFlds
 *
 *  Purpose:
 *      SetSearchEntryList folder list
 */
/*******************************************************************/

void CFolderDlg::OnSetSearchDestFlds()
{
    ULONG           ulValues        = 0;
    LPSPropValue    lpPropValue    = NULL;
    CEntryListDlg   SearchList(this);

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };

    // for SetsearchCriteria Folder Entrylist

    m_hResult = m_lpFolder->GetProps( (LPSPropTagArray) &sptaDisplayName,0,&ulValues,&lpPropValue);
    wsprintf(m_szLogBuf,"lpFolder->GetProps( &sptaDisplayName,0,&ulValues,&lpPropValue)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpFolder->GetProps()", m_hResult),
                            "Client", MBS_ERROR );
        return;
    }

    if(cFolders == 0)
    {
        lpEntryListSearchFolder = (LPENTRYLIST) PvAlloc(sizeof(ENTRYLIST) );
        lpEntryListSearchFolder->lpbin      = (SBinary *) PvAllocMore(
                       ( 10 * sizeof(SBinary)) , lpEntryListSearchFolder);
        lpEntryListSearchFolder->cValues    = 1;
    }
    else
        lpEntryListSearchFolder->cValues++;

    cFolders++;

    lpEntryListSearchFolder->lpbin[cFolders - 1].cb  = lpPropValue[1].Value.bin.cb;
    lpEntryListSearchFolder->lpbin[cFolders - 1].lpb = (BYTE *)PvAllocMore(
                                    lpPropValue[1].Value.bin.cb,lpEntryListSearchFolder);

    memcpy(lpEntryListSearchFolder->lpbin[cFolders - 1].lpb,
               lpPropValue[1].Value.bin.lpb,
               (size_t) lpPropValue[1].Value.bin.cb);

    SearchList.m_List = "SearchFolder EntryList";
    SearchList.m_lppEntryList = &lpEntryListSearchFolder;

    SearchList.DoModal();

    // if cleared list, reset count
    if( !lpEntryListSearchFolder)
         cFolders = 0;

    if(lpPropValue)
    {
        MAPIFreeBuffer(lpPropValue);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpPropValue from GetProps" );
    }
}


/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnSetCopyToDest
 *
 *  Purpose:
 *      Set Destination object of CopyTo operation or CopyMessage
 *
 *  lpvCopyToDest and CopyToDest are global
 */
/*******************************************************************/

void CFolderDlg::OnSetCopyToDest()
{
    char    szBuffer[256];

    lpvCopyToDest   = (LPVOID) m_lpFolder;

    wsprintf(szBuffer,"%s is next Destination for CopyTo() or CopyMessage()",
                m_CurrentFolder.GetBuffer(40) );

    MessageBox( szBuffer,"Client", MBS_INFO );

}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnFldCreate
 *
 *  Purpose:
 *      Operation lpFolder->CreateFolder()
 *
 */
/*******************************************************************/

void CFolderDlg::OnFldCreate()
{
    COperation      FldCreateDlg(this);
    CFolderDlg      *lpDlgChildFld  = NULL;
    char            szBuff[80];
    CString         OperName;
    int             dRet            = 0;
    LONG            lCurSelect      = 0;
    LPIID           lpInterface     = NULL;
    LONG            lFolderType     = 0;
    ULONG           ulFlags         = 0;
    LPMAPIFOLDER        lpFolder        = NULL;
    ULONG           idx             = 0;
    LPWSTR          lpNewBuffer1    = NULL;
    LPWSTR          lpNewBuffer2    = NULL;
    SCODE           scResult        = S_OK;
    ULONG           ulRowCount      = 0;
    char            szBuffer[80];

    // initalize data for dialog box

    OperName  = m_CurrentFolder;
    OperName += "->CreateFolder()";

    FldCreateDlg.m_CurrentOperation= OperName;
    FldCreateDlg.m_CBText1         = "lpInterface";
    FldCreateDlg.m_CBText2         = "ulFolderType";
    FldCreateDlg.m_EditText1       = "lpszFolderName";
    FldCreateDlg.m_EditText2       = "lpszFolderComment";
    FldCreateDlg.m_FlagText1       = "MAPI_UNICODE";
    FldCreateDlg.m_FlagText2       = "MAPI_DEFERRED_ERRORS";
    FldCreateDlg.m_FlagText3       = "OPEN_IF_EXISTS";
    FldCreateDlg.m_FlagText4       = "Invalid Flag";
    FldCreateDlg.m_EditDefault1    = "New Folder Name";
    FldCreateDlg.m_EditDefault2    = "New Folder Comment";

    dRet = FldCreateDlg.m_CBContents1.Add("NULL");
    dRet = FldCreateDlg.m_CBContents1.Add("BadParam");

    ulRowCount = GetRowCount("FolderType");
    for( idx = 0; idx < ulRowCount ; idx++ )
    {
        if( !GetRowString("FolderType",idx,szBuff) )
             return;

        FldCreateDlg.m_CBContents2.Add(szBuff);
    }

    // bring up modal dialog box, and if user hits OK, process operation
    if( FldCreateDlg.DoModal() == IDOK )
    {

        // determine state/settings of data in dialog upon closing
        if( !lstrcmp(FldCreateDlg.m_szCB1,"NULL") )
            lpInterface = NULL;
        else
            lpInterface = (LPIID) &ulFlags;         // invalid

        GetID("FolderType",FldCreateDlg.m_szCB2,&lFolderType);

        if( FldCreateDlg.m_bFlag2 )
            ulFlags |= MAPI_DEFERRED_ERRORS;

        if( FldCreateDlg.m_bFlag3 )
            ulFlags |= OPEN_IF_EXISTS;

        if( FldCreateDlg.m_bFlag4)
            ulFlags |= TEST_INVALID_FLAG;

        if( FldCreateDlg.m_bFlag1 )
        {
            ulFlags |= MAPI_UNICODE;

            String8ToUnicode(FldCreateDlg.m_szEdit1, &lpNewBuffer1, NULL);
            String8ToUnicode(FldCreateDlg.m_szEdit2, &lpNewBuffer2, NULL);


            m_hResult = m_lpFolder->CreateFolder(
                        (ULONG)  lFolderType,
                        (LPTSTR) lpNewBuffer1,
                        (LPTSTR) lpNewBuffer2,
                        lpInterface,
                        ulFlags,
                        &lpFolder);

            wsprintf(m_szLogBuf,"lpFolder->CreateFolder(%lu,%s,%s,0x%X,%lu,&lpFolder)\t SC: %s",
                        lFolderType,FldCreateDlg.m_szEdit1,FldCreateDlg.m_szEdit2,
                        lpInterface,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpFolder->CreateFolder()", m_hResult),
                            "Client", MBS_ERROR );
                PvFree(lpNewBuffer1);
                PvFree(lpNewBuffer2);
                return;
            }

            PvFree(lpNewBuffer1);
            PvFree(lpNewBuffer2);
        }
        else
        {

            m_hResult = m_lpFolder->CreateFolder(
                        (ULONG)  lFolderType,
                        (LPTSTR) FldCreateDlg.m_szEdit1,
                        (LPTSTR) FldCreateDlg.m_szEdit2,
                        lpInterface,
                        ulFlags,
                        &lpFolder);


            wsprintf(m_szLogBuf,"lpFolder->CreateFolder(%lu,%s,%s,0x%X,%lu,&lpFolder)\t SC: %s",
                        lFolderType,FldCreateDlg.m_szEdit1,FldCreateDlg.m_szEdit2,
                        lpInterface,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("m_lpFolder->CreateFolder()", m_hResult),
                                "Client", MBS_ERROR );
                return;
            }

        }


        wsprintf(szBuffer,"MAPI_FOLDER - %s",FldCreateDlg.m_szEdit1);

        lpDlgChildFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpFolder,this);

    }
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnGetSearchCriteria
 *
 *  Purpose:
 *      Operation lpFolder->GetSearchCriteria()
 *
 */
/*******************************************************************/

void CFolderDlg::OnGetSearchCriteria()
{
    CRestrictionDlg GetSearchRestrictionDlg(this);
    LPSRestriction  lpsRestriction      = NULL;
    LPENTRYLIST     lpEntryList         = NULL;
    ULONG           ulSearchState       = 0;
    CEntryListDlg   SearchList(this);
    char            szBuffer[128];

    m_hResult = m_lpFolder->GetSearchCriteria(
                        0,
                        &lpsRestriction,
                        &lpEntryList,
                        &ulSearchState);

    wsprintf(m_szLogBuf,"lpFolder->GetSearchCriteria(0,&lpsRes,&lpEntryList,&ulSearchState)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpFolder->GetSearchCriteria()", m_hResult),
                    "Client", MBS_ERROR );
        goto End;
    }

    GetSearchRestrictionDlg.m_ulSearchState = ulSearchState;
    GetSearchRestrictionDlg.m_prest         = lpsRestriction;
    GetSearchRestrictionDlg.DoModal();

    SearchList.m_List = "GetSearchCriteria EntryList";
    SearchList.m_lppEntryList = &lpEntryList;

    SearchList.DoModal();

    wsprintf(szBuffer,"ulSearchState == %lu, %s%s%s%s",ulSearchState,
            ((ulSearchState & SEARCH_RUNNING)     ? "SEARCH_RUNNING     | "   : ""),
            ((ulSearchState & SEARCH_REBUILD)     ? "SEARCH_REBUILD     | "   : ""),
            ((ulSearchState & SEARCH_RECURSIVE)   ? "SEARCH_RECURSIVE   | "   : ""),
            ((ulSearchState & SEARCH_FOREGROUND)  ? "SEARCH_FOREGROUND  | "   : ""));

    MessageBox( szBuffer,"Client", MBS_INFO );


End:

    if(lpsRestriction)
    {
        MAPIFreeBuffer(lpsRestriction);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpsRestriction from GetSearchCriteria" );
    }

    if(lpEntryList)
    {
        MAPIFreeBuffer(lpEntryList);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpEntryList from GetSearchCriteria" );
    }
}



/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnSetSearchCriteria
 *
 *  Purpose:
 *      Operation lpFolder->SetSearchCriteria()
 *
 */
/*******************************************************************/

void CFolderDlg::OnSetSearchCriteria()
{
    COperation      SetSearchDlg(this);
    char            szBuffer[512];
    char            szBuff[80];
    CString         OperName;
    ULONG           ulFlags         = 0;
    SCODE           sc              = S_OK;
    LPSRestriction  lpRes           = NULL;
    LPSPropTagArray lpsptaAll       = NULL;
    DWORD           dRet            = 0;
    ULONG           idx             = 0;
    ULONG           ulMax           = 0;
    ULONG           ulPropTag       = 0;
    CEntryListDlg   SearchList(this);
    ULONG           cb              = 0;

    ulMax =   GetRowCount("PropTags");


    cb = CbNewSPropTagArray(ulMax);
    lpsptaAll = (LPSPropTagArray) PvAlloc(cb);
    lpsptaAll->cValues = 0;

    for(idx = 0; idx < ulMax ; idx++)
    {
        ulPropTag = GetRowID("PropTags",idx);

        lpsptaAll->cValues++;
        lpsptaAll->aulPropTag[lpsptaAll->cValues - 1] = ulPropTag;
    }

    lpRes = (LPSRestriction)PvAlloc(sizeof(SRestriction));

    CResDlg         dlgBuildRes(lpsptaAll, lpRes);
    dlgBuildRes.DoModal();

    PvFree(lpsptaAll);

    // initalize data for dialog box

    OperName  = m_CurrentFolder;
    OperName += "->SetSearchCriteria()";

    SetSearchDlg.m_CurrentOperation= OperName;
    SetSearchDlg.m_CBText1         = "lpEntryList:";
    SetSearchDlg.m_CBText2         = "lpRestriction:";
    SetSearchDlg.m_FlagText1       = "STOP_SEARCH";
    SetSearchDlg.m_FlagText2       = "RESTART_SEARCH";
    SetSearchDlg.m_FlagText3       = "SHALLOW_SEARCH";
    SetSearchDlg.m_FlagText4       = "RECURSIVE_SEARCH";
    SetSearchDlg.m_FlagText5       = "FOREGROUND_SEARCH";
    SetSearchDlg.m_FlagText6       = "BACKGROUND_SEARCH";

    if(lpEntryListSearchFolder)
    {
        for( idx = 0 ; idx < lpEntryListSearchFolder->cValues ; idx++)
        {
            SzGetEntryID(   szBuffer,
                            (LPENTRYID) lpEntryListSearchFolder->lpbin[idx].lpb,
                            (ULONG)     lpEntryListSearchFolder->lpbin[idx].cb);

            dRet = SetSearchDlg.m_CBContents1.Add(szBuffer);
        }
    }

    wsprintf(szBuff,"Built Restriction == 0x%X",lpRes);
    dRet = SetSearchDlg.m_CBContents2.Add(szBuff);
    dRet = SetSearchDlg.m_CBContents2.Add("NULL");

     // bring up modal dialog box, and if user hits OK, process operation
    if( SetSearchDlg.DoModal() == IDOK )
    {
        if( SetSearchDlg.m_bFlag1 )
            ulFlags |= STOP_SEARCH;

        if( SetSearchDlg.m_bFlag2 )
            ulFlags |= RESTART_SEARCH;

        if( SetSearchDlg.m_bFlag3)
            ulFlags |= SHALLOW_SEARCH;

        if( SetSearchDlg.m_bFlag4)
            ulFlags |= RECURSIVE_SEARCH;

        if( SetSearchDlg.m_bFlag5)
            ulFlags |= FOREGROUND_SEARCH;

        if( SetSearchDlg.m_bFlag6 )
            ulFlags |= BACKGROUND_SEARCH;


        if( !lstrcmp(SetSearchDlg.m_szCB2,"NULL") )
        {
            PvFree(lpRes);
            lpRes = NULL;
        }

        SearchList.m_List = "SearchFolder EntryList";
        SearchList.m_lppEntryList = &lpEntryListSearchFolder;

        SearchList.DoModal();

        // if cleared list, reset count
        if( !lpEntryListSearchFolder)
             cFolders = 0;


        // else keep built restriction
        m_hResult = m_lpFolder->SetSearchCriteria(lpRes,
                        lpEntryListSearchFolder,
                        ulFlags);
        wsprintf(m_szLogBuf,"lpFolder->SetSearchCriteria(0x%X,0x%X,%lu)\t SC: %s",
                        lpRes,lpEntryListSearchFolder,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->SetSearchCriteria()", m_hResult),
                            "Client", MBS_ERROR );
            return;
        }
        PvFree(lpRes);
    }
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnSetReceiveFolder
 *
 *  Purpose:
 *
 *  Comments:
 *      lpMDB is global pointer to open message store
 */
/*******************************************************************/

void CFolderDlg::OnSetReceiveFolder()
{
    COperation      SetReceiveFldDlg(this);
    char            szBuff[256];
    CString         OperName;
    LONG            lCurSelect      = 0;
    ULONG           ulFlags         = 0;
    ULONG           ulValues        = 0;
    LPSPropValue    lpPropValue     = NULL;
    LPWSTR          lpNewBuffer1    = NULL;

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };

    // initalize data for dialog box

    OperName  = "lpMDB->SetReceiveFolder()";

    SetReceiveFldDlg.m_CurrentOperation= OperName;
    SetReceiveFldDlg.m_EditText1       = "lpszMessageClass";
    SetReceiveFldDlg.m_EditText2       = "Folder Name:";
    SetReceiveFldDlg.m_EditText3       = "lpEntryID/cbEntryID:";
    SetReceiveFldDlg.m_FlagText1       = "MAPI_UNICODE";
    SetReceiveFldDlg.m_FlagText2       = "Invalid Flag";
    SetReceiveFldDlg.m_EditDefault1    = "IPM";

    m_hResult = m_lpFolder->GetProps( (LPSPropTagArray) &sptaDisplayName,0,&ulValues,&lpPropValue);
    wsprintf(m_szLogBuf,"lpFolder->GetProps( &sptaDisplayName,0,&ulValues,&lpPropValue)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpFolder->GetProps()", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    SzGetPropValue(szBuff, &lpPropValue[1]);

    SetReceiveFldDlg.m_EditDefault2    = lpPropValue[0].Value.lpszA;
    SetReceiveFldDlg.m_EditDefault3    = szBuff;


    // bring up modal dialog box, and if user hits OK, process operation
    if( SetReceiveFldDlg.DoModal() == IDOK )
    {

        if( SetReceiveFldDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        if( SetReceiveFldDlg.m_bFlag1 )
        {
            ulFlags |= MAPI_UNICODE;

            String8ToUnicode(SetReceiveFldDlg.m_szEdit1,&lpNewBuffer1, NULL);

            m_hResult = lpMDB->SetReceiveFolder(
                        (LPTSTR) lpNewBuffer1,
                        ulFlags,
                        lpPropValue[1].Value.bin.cb,
                        (LPENTRYID) lpPropValue[1].Value.bin.lpb );
            wsprintf(m_szLogBuf,"lpMDB->SetReceiveFolder(%s,%lu,%lu,0x%X)\t SC: %s",
                        SetReceiveFldDlg.m_szEdit1,ulFlags,
                        lpPropValue[1].Value.bin.cb,
                        lpPropValue[1].Value.bin.lpb,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("lpMDB->SetReceiveFolder", m_hResult),
                                "Client", MBS_ERROR );
                PvFree(lpNewBuffer1);
                goto Cleanup;
            }

            PvFree(lpNewBuffer1);

            wsprintf(szBuff,"%s set to receive %s Message Class",lpPropValue[0].Value.lpszA,
                                  (LPTSTR)SetReceiveFldDlg.m_szEdit1);

            MessageBox( szBuff,"Client", MBS_INFO );
        }
        else
        {

            m_hResult = lpMDB->SetReceiveFolder(
                        (LPTSTR)SetReceiveFldDlg.m_szEdit1,
                        ulFlags,
                        lpPropValue[1].Value.bin.cb,
                        (LPENTRYID) lpPropValue[1].Value.bin.lpb );
            wsprintf(m_szLogBuf,"lpMDB->SetReceiveFolder(%s,%lu,%lu,0x%X)\t SC: %s",
                        SetReceiveFldDlg.m_szEdit1,ulFlags,
                        lpPropValue[1].Value.bin.cb,
                        lpPropValue[1].Value.bin.lpb,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

            if( HR_FAILED(m_hResult))
            {
                MessageBox( m_E.SzError("lpMDB->SetReceiveFolder", m_hResult),
                            "Client", MBS_ERROR );
                goto Cleanup;
            }
            wsprintf(szBuff,"%s set to receive %s Message Class",lpPropValue[0].Value.lpszA,
                                  (LPTSTR)SetReceiveFldDlg.m_szEdit1);

            MessageBox( szBuff,"Client", MBS_INFO );
        }
    }

Cleanup:

    // free memory from GetProps
    if(lpPropValue)
    {
        MAPIFreeBuffer(lpPropValue);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpPropValue from GetProps" );
    }
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnCopyMessages
 *
 *  Purpose:
 *      Operation lpFolder->CopyMessages
 *
 */
/*******************************************************************/

void CFolderDlg::OnCopyMessages()
{
    COperation      CopyMessagesDlg(this);
    char            szBuff[80];
    char            szBuffer[80];
    CString         OperName;
    int             dRet            = 0;
    LPIID           lpInterface     = NULL;
    ULONG           ulFlags         = 0;
    LPENTRYLIST     lpEntryList     = NULL;
    ULONG           ulUIParam       = 0;
    LPSRowSet       lpRows          = NULL;
    LPENTRYID       lpEntryID       = NULL;
    ULONG           ulEntryID       = 0;
    ULONG           ulErrorIndex    = 0;
    LONG            lRowsSeeked     = 0;
    int max=500;
    int rgItems[500];
    int j;
    LONG            lNumSelect      = -1;

    //$ NYI, FIX UP LPMAPIPROGRESS
    LPMAPIPROGRESS  lpProgress       = NULL;


    // initalize data for dialog box

    OperName  = m_CurrentFolder;
    OperName += "->CopyMessages()";

    CopyMessagesDlg.m_CurrentOperation= OperName;
    CopyMessagesDlg.m_CBText1         = "lpInterface:";
    CopyMessagesDlg.m_CBText2         = "ulUIParam:";
    CopyMessagesDlg.m_CBText3         = "lpMsgList";
    CopyMessagesDlg.m_EditText2       = "lpDestFolder:";
    CopyMessagesDlg.m_EditText3       = "lpMsgList";
    CopyMessagesDlg.m_FlagText1       = "MESSAGE_MOVE";
    CopyMessagesDlg.m_FlagText2       = "MESSAGE_DIALOG";
    CopyMessagesDlg.m_FlagText3       = "MAPI_DECLINE_OK";
    CopyMessagesDlg.m_FlagText4       = "Invalid Flag";

    wsprintf(szBuffer,"0x%X",lpvCopyToDest);
    CopyMessagesDlg.m_EditDefault2    = szBuffer;
    CopyMessagesDlg.m_EditDefault3    = "";

    dRet = CopyMessagesDlg.m_CBContents1.Add("NULL");
    dRet = CopyMessagesDlg.m_CBContents1.Add("Bad Interface Param");

    dRet = CopyMessagesDlg.m_CBContents2.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = CopyMessagesDlg.m_CBContents2.Add(szBuff);


    // initalize data for dialog box
    if( (lNumSelect = SendDlgItemMessage(IDC_FLDMESSAGES,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) < 1 )
    {
        goto Associated;
    }

    lpEntryList = (LPENTRYLIST) PvAlloc(sizeof(ENTRYLIST));
    lpEntryList->cValues    = lNumSelect;
    lpEntryList->lpbin      = (LPSBinary)PvAllocMore(lNumSelect * sizeof(SBinary), lpEntryList);

    for(j=0; j < lNumSelect; j++)
    {
        // set which columns are important to see in table
        m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,rgItems[j],&lRowsSeeked);

        wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[j],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        m_hResult = m_lpMessageTable->QueryRows( 1, 0, &lpRows );
        wsprintf(m_szLogBuf,"lpMessageTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpMessageTable->QueryRows", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        // rip out info
        if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
            dRet = CopyMessagesDlg.m_CBContents3.Add( lpRows->aRow[0].lpProps[0].Value.lpszA );

        if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
        {
            lpEntryList->lpbin[j].cb  = (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb;
            lpEntryList->lpbin[j].lpb = (BYTE *)PvAllocMore(
                                        lpRows->aRow[0].lpProps[1].Value.bin.cb,
                                        lpEntryList);

            memcpy(lpEntryList->lpbin[j].lpb,
              (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb,
              (size_t) (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb);
            // _fmemcpy for 16 bit ??????
        }

        if(lpRows)
        {
             FreeRowSet(lpRows);
             lpRows=NULL;
        }
    }


    // bring up modal dialog box, and if user hits OK, process operation
    if( CopyMessagesDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( !lstrcmp(CopyMessagesDlg.m_szCB1,"NULL") )
            lpInterface = (LPIID) NULL;
        else if(!lstrcmp(CopyMessagesDlg.m_szCB1,"Bad Interface Param") )
            lpInterface = (LPIID) &ulFlags;         // invalid

        if( !lstrcmp(CopyMessagesDlg.m_szCB2,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( CopyMessagesDlg.m_bFlag1 )
            ulFlags |= MESSAGE_MOVE;

        if( CopyMessagesDlg.m_bFlag2 )
            ulFlags |= MESSAGE_DIALOG;

        if( CopyMessagesDlg.m_bFlag3)
            ulFlags |= MAPI_DECLINE_OK;

        if( CopyMessagesDlg.m_bFlag4)
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->CopyMessages(lpEntryList,
                        lpInterface,
                        (LPVOID) lpvCopyToDest,
                        ulUIParam,
                        lpProgress,
                        ulFlags);

        wsprintf(m_szLogBuf,"lpFolder->CopyMessages(0x%X,0x%X,0x%X,%lu,0x%X,%lu,&ulErrorIndex)\t SC: %s",
                        lpEntryList,lpInterface,lpvCopyToDest,ulUIParam,lpProgress,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->CopyMessages()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }

    goto Cleanup;

Associated:


    // ASSOCIATED MESSAGES

    lNumSelect  = -1;

    // initalize data for dialog box
    if( (lNumSelect = SendDlgItemMessage(IDC_FLD_ASS,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) < 1 )
    {
        MessageBox("Select one or several Messages in (Msg or Associated Msg)Listbox to Copy", "OnCopyMessages", MBS_ERROR );
        goto Cleanup;
    }


    lpEntryList = (LPENTRYLIST) PvAlloc(sizeof(ENTRYLIST));
    lpEntryList->cValues    = lNumSelect;
    lpEntryList->lpbin      = (LPSBinary)PvAllocMore(lNumSelect * sizeof(SBinary), lpEntryList);

    for(j=0; j < lNumSelect; j++)
    {
        // set which columns are important to see in table
        m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,rgItems[j],&lRowsSeeked);

        wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[j],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        m_hResult = m_lpAssociatedTable->QueryRows( 1, 0, &lpRows );
        wsprintf(m_szLogBuf,"lpAssociatedTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpAssociatedTable->QueryRows", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        // rip out info
        if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
            dRet = CopyMessagesDlg.m_CBContents3.Add( lpRows->aRow[0].lpProps[0].Value.lpszA );

        if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
        {
            lpEntryList->lpbin[j].cb  = (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb;
            lpEntryList->lpbin[j].lpb = (BYTE *)PvAllocMore(
                                        lpRows->aRow[0].lpProps[1].Value.bin.cb,
                                        lpEntryList);

            memcpy(lpEntryList->lpbin[j].lpb,
              (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb,
              (size_t) (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb);
            // _fmemcpy for 16 bit ??????
        }

        if(lpRows)
        {
             FreeRowSet(lpRows);
             lpRows=NULL;
        }
    }


    // bring up modal dialog box, and if user hits OK, process operation
    if( CopyMessagesDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( !lstrcmp(CopyMessagesDlg.m_szCB1,"NULL") )
            lpInterface = (LPIID) NULL;
        else if(!lstrcmp(CopyMessagesDlg.m_szCB1,"Bad Interface Param") )
            lpInterface = (LPIID) &ulFlags;         // invalid

        if( !lstrcmp(CopyMessagesDlg.m_szCB2,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( CopyMessagesDlg.m_bFlag1 )
            ulFlags |= MESSAGE_MOVE;

        if( CopyMessagesDlg.m_bFlag2 )
            ulFlags |= MESSAGE_DIALOG;

        if( CopyMessagesDlg.m_bFlag3)
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->CopyMessages(lpEntryList,
                        lpInterface,
                        (LPVOID) lpvCopyToDest,
                        ulUIParam,
                        lpProgress,
                        ulFlags);

        wsprintf(m_szLogBuf,"lpFolder->CopyMessages(0x%X,0x%X,%lu,0x%X,%lu,&ulErrorIndex)\t SC: %s",
                        lpEntryList,lpInterface,lpvCopyToDest,ulUIParam,lpProgress,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->CopyMessages()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }


Cleanup:

    // free memory allocated in this routine

    PvFree(lpEntryList);

    FreeRowSet(lpRows);
}


/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnAbortSubmit
 *
 *  Purpose:
 *      Operation lpMDB->AbortSubmit,  lpMDB is global Message store object
 *
 */
/*******************************************************************/

void CFolderDlg::OnAbortSubmit()
{
    COperation      AbortSubmitDlg(this);
    CString         OperName;
    char            szBuff[512];
    ULONG           ulFlags     = 0;
    LPENTRYID       lpEntryID   = NULL;
    ULONG           ulEntryID   = 0;
    LONG            lCurSelect  = -1;
    LPSRowSet       lpRows      = NULL;
    LONG            lRowsSeeked = 0;
    int max=500;
    int rgItems[500];

    // initalize data for dialog box

    OperName = "lpMDB->AbortSubmit()";

    AbortSubmitDlg.m_CurrentOperation= OperName;
    AbortSubmitDlg.m_CBText1         = "";
    AbortSubmitDlg.m_CBText2         = "";
    AbortSubmitDlg.m_CBText3         = "";
    AbortSubmitDlg.m_EditText1       = "Message Subject";
    AbortSubmitDlg.m_EditText2       = "lpEntryID/cbEntryID";
    AbortSubmitDlg.m_EditText3       = "";
    AbortSubmitDlg.m_FlagText1       = "Invalid Flag";
    AbortSubmitDlg.m_FlagText2       = "";
    AbortSubmitDlg.m_FlagText3       = "";
    AbortSubmitDlg.m_FlagText4       = "";
    AbortSubmitDlg.m_EditDefault3    = "";


    // initalize data for dialog box
    if( (lCurSelect = SendDlgItemMessage(IDC_FLDMESSAGES,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
            goto Associated;

    m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpMessageTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpMessageTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    // rip out info
    if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
        AbortSubmitDlg.m_EditDefault1 = lpRows->aRow[0].lpProps[0].Value.lpszA;

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;

        SzGetPropValue(szBuff, &(lpRows->aRow[0].lpProps[1]));
        AbortSubmitDlg.m_EditDefault2    = szBuff;
    }

    // bring up modal dialog box, and if user hits OK, process operation
    if( AbortSubmitDlg.DoModal() == IDOK )
    {

        //$ NYI, should split up cbEntryID and lpEntryid and allow mofidication and then
        //     read out data to be passed into function

        if( AbortSubmitDlg.m_bFlag1)
            ulFlags |= TEST_INVALID_FLAG;

        // lpMDB is global variable
        m_hResult = lpMDB->AbortSubmit(
                        (ULONG)     ulEntryID,
                        (LPENTRYID) lpEntryID,
                        ulFlags  );
        wsprintf(m_szLogBuf,"lpMDB->AbortSubmit(%lu,0x%X,%lu)\t SC: %s",
                        ulEntryID,lpEntryID,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("lpMDB->AbortSubmit()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }

    goto Cleanup;

Associated:


    lCurSelect  = -1;

    // initalize data for dialog box
    if( (lCurSelect = SendDlgItemMessage(IDC_FLD_ASS,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
    {
            MessageBox("Select Only One Message in Listbox to Abort", "OnAbortSubmit", MBS_ERROR );
            goto Cleanup;
    }

    // ASSOCIATED MESSAGES

    m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpAssociatedTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpAssociatedTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    // rip out info
    if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
        AbortSubmitDlg.m_EditDefault1 = lpRows->aRow[0].lpProps[0].Value.lpszA;

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;

        SzGetPropValue(szBuff, &(lpRows->aRow[0].lpProps[1]));
        AbortSubmitDlg.m_EditDefault2    = szBuff;
    }

    // bring up modal dialog box, and if user hits OK, process operation
    if( AbortSubmitDlg.DoModal() == IDOK )
    {

        //$ NYI, should split up cbEntryID and lpEntryid and allow mofidication and then
        //     read out data to be passed into function

        if( AbortSubmitDlg.m_bFlag1)
            ulFlags |= TEST_INVALID_FLAG;

        // lpMDB is global variable
        m_hResult = lpMDB->AbortSubmit(
                        (ULONG)     ulEntryID,
                        (LPENTRYID) lpEntryID,
                        ulFlags  );
        wsprintf(m_szLogBuf,"lpMDB->AbortSubmit(%lu,0x%X,%lu)\t SC: %s",
                        ulEntryID,lpEntryID,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("lpMDB->AbortSubmit()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }


Cleanup:

    FreeRowSet(lpRows);
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnFldDelete
 *
 *  Purpose:
 *      Operation lpFolder->DeleteFolder
 *
 */
/*******************************************************************/

void CFolderDlg::OnFldDelete()
{
    COperation      FldDeleteDlg(this);
    char            szBuff[512];
    CString         OperName;
    int             dRet            = 0;
    LONG            lCurSelect      = -1;
    LPIID           lpInterface     = NULL;
    LONG            lFolderType     = 0;
    ULONG           ulFlags         = 0;
    LPMAPIFOLDER        lpFolder        = NULL;
    ULONG           idx             = 0;
    LPENTRYID       lpChildEntryID  = NULL;
    ULONG           cbChildEntryID  = 0;
    LPSRowSet       lpRows          = NULL;
    ULONG           ulUIParam       = 0;
    LONG            lRowsSeeked     = 0;

    //$ NYI, FIX UP LPMAPIPROGRESS
    LPMAPIPROGRESS  lpProgress       = NULL;

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };


    // get selected item from child folder listbox
    if( (lCurSelect = SendDlgItemMessage(IDC_FLDCHILD,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Folder in Listbox to Delete", "OnFldDelete", MBS_ERROR );
        goto Cleanup;
    }

    // initalize data for dialog box

    OperName  = m_CurrentFolder;
    OperName += "->DeleteFolder()";

    FldDeleteDlg.m_CurrentOperation= OperName;
    FldDeleteDlg.m_CBText2         = "ulUIParam";
    FldDeleteDlg.m_EditText1       = "Delete Folder";
    FldDeleteDlg.m_EditText2       = "lpEntryID/cbEntryID";
    FldDeleteDlg.m_FlagText1       = "DEL_MESSAGES";
    FldDeleteDlg.m_FlagText2       = "DEL_FOLDERS";
    FldDeleteDlg.m_FlagText3       = "FOLDER_DIALOG";
    FldDeleteDlg.m_FlagText4       = "Invalid Flag";

    m_hResult = m_lpChildTable->SeekRow( BOOKMARK_BEGINNING,lCurSelect,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpChildTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        lCurSelect,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpChildTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpChildTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    // rip out info
    if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_DISPLAY_NAME )
        FldDeleteDlg.m_EditDefault1 = lpRows->aRow[0].lpProps[0].Value.lpszA;

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {

        lpChildEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        cbChildEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;

        SzGetPropValue(szBuff, &(lpRows->aRow[0].lpProps[1]));
        FldDeleteDlg.m_EditDefault2    = szBuff;
    }

    FldDeleteDlg.m_EditDefault3    = "";


    dRet = FldDeleteDlg.m_CBContents2.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = FldDeleteDlg.m_CBContents2.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( FldDeleteDlg.DoModal() == IDOK )
    {

        if( !lstrcmp(FldDeleteDlg.m_szCB2,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( FldDeleteDlg.m_bFlag1 )
            ulFlags |= DEL_MESSAGES;

        if( FldDeleteDlg.m_bFlag2 )
            ulFlags |= DEL_FOLDERS;

        if( FldDeleteDlg.m_bFlag3)
            ulFlags |= FOLDER_DIALOG;

        if( FldDeleteDlg.m_bFlag4)
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->DeleteFolder(
                        cbChildEntryID,
                        lpChildEntryID,
                        ulUIParam,
                        lpProgress,
                        ulFlags);
        wsprintf(m_szLogBuf,"lpFolder->DeleteFolder(%lu,0x%X,%lu,0x%X,%lu)\t SC: %s",
                        cbChildEntryID,
                        lpChildEntryID,
                        ulUIParam,
                        lpProgress,
                        ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->DeleteFolder()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }

Cleanup:

    FreeRowSet(lpRows);
}



/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnCopyFolder
 *
 *  Purpose:
 *      Operation lpFolder->DeleteFolder
 *
 */
/*******************************************************************/

void CFolderDlg::OnCopyFolder()
{
    COperation      CopyFolderDlg(this);
    char            szBuff[512];
    CString         OperName;
    int             dRet            = 0;
    LONG            lCurSelect      = -1;
    LPIID           lpInterface     = NULL;
    LONG            lFolderType     = 0;
    ULONG           ulFlags         = 0;
    LPMAPIFOLDER    lpFolder        = NULL;
    ULONG           idx             = 0;
    LPENTRYID       lpChildEntryID  = NULL;
    ULONG           cbChildEntryID  = 0;
    LPSRowSet       lpRows          = NULL;
    ULONG           ulUIParam       = 0;
    LONG            lRowsSeeked     = 0;
    LPMAPIPROGRESS  lpProgress      = NULL;

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };


    // get selected item from child folder listbox
    if( (lCurSelect = SendDlgItemMessage(IDC_FLDCHILD,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Source Folder in Listbox to Copy", "OnCopyFolder", MBS_ERROR );
        goto Cleanup;
    }

    // initalize data for dialog box

    OperName  = m_CurrentFolder;
    OperName += "->CopyFolder()";

    CopyFolderDlg.m_CurrentOperation= OperName;
    CopyFolderDlg.m_CBText1         = "lpvDestFld";
    CopyFolderDlg.m_CBText2         = "ulUIParam";
    CopyFolderDlg.m_EditText1       = "Copy Folder";
    CopyFolderDlg.m_EditText2       = "lpEntryID/cbEntryID";
    CopyFolderDlg.m_EditText3       = "lpszNewFolderName";
    CopyFolderDlg.m_FlagText1       = "FOLDER_MOVE";
    CopyFolderDlg.m_FlagText2       = "FOLDER_DIALOG";
    CopyFolderDlg.m_FlagText3       = "MAPI_DECLINE_OK";
    CopyFolderDlg.m_FlagText4       = "COPY_SUBFOLDERS";
    CopyFolderDlg.m_FlagText5       = "MAPI_UNICODE";
    CopyFolderDlg.m_FlagText6       = "Invalid Flag";

    wsprintf(szBuff,"0x%08X",lpvCopyToDest);
    dRet = CopyFolderDlg.m_CBContents1.Add(szBuff);

    CopyFolderDlg.m_EditDefault3 = "New Dest Folder Name";

    m_hResult = m_lpChildTable->SeekRow( BOOKMARK_BEGINNING,lCurSelect,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpChildTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        lCurSelect,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }


    m_hResult = m_lpChildTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpChildTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Cleanup;
    }

    // rip out info
    if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_DISPLAY_NAME )
        CopyFolderDlg.m_EditDefault1 = lpRows->aRow[0].lpProps[0].Value.lpszA;

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {

        lpChildEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        cbChildEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;

        SzGetPropValue(szBuff, &(lpRows->aRow[0].lpProps[1]));
        CopyFolderDlg.m_EditDefault2    = szBuff;
    }

    dRet = CopyFolderDlg.m_CBContents2.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = CopyFolderDlg.m_CBContents2.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( CopyFolderDlg.DoModal() == IDOK )
    {

        if( !lstrcmp(CopyFolderDlg.m_szCB2,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( CopyFolderDlg.m_bFlag1 )
            ulFlags |= FOLDER_MOVE;

        if( CopyFolderDlg.m_bFlag2 )
            ulFlags |= FOLDER_DIALOG;

        if( CopyFolderDlg.m_bFlag3)
            ulFlags |= MAPI_DECLINE_OK;

        if( CopyFolderDlg.m_bFlag4)
            ulFlags |= COPY_SUBFOLDERS;

        if( CopyFolderDlg.m_bFlag5)
            ulFlags |= MAPI_UNICODE;

        if( CopyFolderDlg.m_bFlag6)
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->CopyFolder(
                        cbChildEntryID,
                        lpChildEntryID,
                        lpInterface,
                        lpvCopyToDest,
                        CopyFolderDlg.m_szEdit3,
                        ulUIParam,
                        lpProgress,
                        ulFlags);
        wsprintf(m_szLogBuf,"lpFolder->CopyFolder()\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->CopyFolder()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }

Cleanup:

    FreeRowSet(lpRows);
}



/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnEmptyFolder
 *
 *  Purpose:
 *      Operation lpFolder->EmptyFolder
 *
 *
 */
/*******************************************************************/

void CFolderDlg::OnEmptyFolder()
{
    COperation      EmptyFolderDlg(this);
    char            szBuff[512];
    CString         OperName;
    int             dRet            = 0;
    ULONG           ulFlags         = 0;
    ULONG           ulUIParam       = 0;
    LONG            lRowsSeeked     = 0;


    //$ FUTURE, FIX UP LPMAPIPROGRESS
    LPMAPIPROGRESS  lpProgress       = NULL;


    // initalize data for dialog box
    OperName  = m_CurrentFolder;
    OperName += "->EmptyFolder()";

    EmptyFolderDlg.m_CurrentOperation= OperName;
    EmptyFolderDlg.m_FlagText1       = "FOLDER_DIALOG";
    EmptyFolderDlg.m_FlagText2       = "DEL_ASSOCIATED";
    EmptyFolderDlg.m_FlagText5       = "Invalid Flag";


    dRet = EmptyFolderDlg.m_CBContents2.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = EmptyFolderDlg.m_CBContents2.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( EmptyFolderDlg.DoModal() == IDOK )
    {

        // determine state/settings of data in dialog upon closing

        if( !lstrcmp(EmptyFolderDlg.m_szCB2,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( EmptyFolderDlg.m_bFlag1)
            ulFlags |= FOLDER_DIALOG;

        if( EmptyFolderDlg.m_bFlag2)
             ulFlags |= DEL_ASSOCIATED;

        if( EmptyFolderDlg.m_bFlag5)
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->EmptyFolder(
                        ulUIParam,
                        lpProgress,
                        ulFlags);
        wsprintf(m_szLogBuf,"lpFolder->EmptyFolder(%lu,0x%X,%lu)\t SC: %s",
                        ulUIParam,
                        lpProgress,
                        ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->EmptyFolder()", m_hResult),
                            "Client", MBS_ERROR );
        }
    }
}



/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnMsgCreate
 *
 *  Purpose:
 *      Operation lpFolder->CreateMessage
 */
/*******************************************************************/

void CFolderDlg::OnMsgCreate()
{
    COperation      MsgCreateDlg(this);
    CString         OperName;
    int             dRet            = 0;
    LONG            lCurSelect      = 0;
    LPIID           lpInterface     = NULL;
    ULONG           ulFlags         = 0;
    LPMESSAGE       lpMessage       = NULL;
    ULONG           idx             = 0;
    CMessageDlg     *lpDlgChildMsg  = NULL;

    // initalize data for dialog box

    OperName  = m_CurrentFolder;
    OperName += "->CreateMessage()";

    MsgCreateDlg.m_CurrentOperation= OperName;

    MsgCreateDlg.m_CBText1         = "lpInterface";

    MsgCreateDlg.m_FlagText1       = "MAPI_ASSOCIATED";
    MsgCreateDlg.m_FlagText2       = "MAPI_DEFERRED_ERRORS";
    MsgCreateDlg.m_FlagText3       = "Invalid Flag";

    dRet = MsgCreateDlg.m_CBContents1.Add("NULL");
    dRet = MsgCreateDlg.m_CBContents1.Add("Bad Interface");

    // bring up modal dialog box, and if user hits OK, process operation
    if( MsgCreateDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( !lstrcmp(MsgCreateDlg.m_szCB1,"NULL") )
            lpInterface = NULL;
        else
            lpInterface = (LPIID) &ulFlags;         // invalid

        if( MsgCreateDlg.m_bFlag1 )
            ulFlags |= MAPI_ASSOCIATED;

        if( MsgCreateDlg.m_bFlag2 )
            ulFlags |= MAPI_DEFERRED_ERRORS;

        if( MsgCreateDlg.m_bFlag3)
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->CreateMessage(
                        lpInterface,
                        ulFlags,
                        &lpMessage);
        wsprintf(m_szLogBuf,"lpFolder->CreateMessage(0x%X,%lu,&lpMessage)\t SC: %s",
                        lpInterface,
                        ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->CreateMessage()", m_hResult),
                            "Client", MBS_ERROR );
            return;
        }

        lpDlgChildMsg   =  new CMessageDlg("MAPI_MESSAGE",(LPMESSAGE)lpMessage,this);
    }
}


/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnMsgDelete
 *
 *  Purpose:
 *      Operation lpFolder->CreateMessage
 */
/*******************************************************************/

void CFolderDlg::OnMsgDelete()
{
    COperation      MsgDeleteDlg(this);
    char            szBuff[512];
    CString         OperName;
    int             dRet            = 0;
    ULONG           ulFlags         = 0;
    LPSRowSet       lpRows          = NULL;
    ULONG           ulUIParam       = 0;
    LPENTRYLIST     lpEntryList     = NULL;
    ULONG           ulErrorIndex    = 0;
    LONG            lRowsSeeked     = 0;
    int max=500;
    int rgItems[500];
    int j;
    LONG            lNumSelect      = -1;


    //$ FUTURE, FIX UP LPMAPIPROGRESS
    LPMAPIPROGRESS  lpProgress       = NULL;

    // initalize data for dialog box

    OperName  = m_CurrentFolder;
    OperName += "->DeleteMessages()";

    MsgDeleteDlg.m_CurrentOperation= OperName;
    MsgDeleteDlg.m_CBText2         = "ulUIParam";
    MsgDeleteDlg.m_CBText3         = "Delete Messages";
    MsgDeleteDlg.m_EditText1       = "";
    MsgDeleteDlg.m_FlagText1       = "MESSAGE_DIALOG";
    MsgDeleteDlg.m_FlagText2       = "Invalid Flag";

    // initalize data for dialog box
    if( (lNumSelect = SendDlgItemMessage(IDC_FLDMESSAGES,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) < 1 )
        goto Associated;

    // NORMAL MESSAGES

    lpEntryList = (LPENTRYLIST) PvAlloc(sizeof(ENTRYLIST));
    lpEntryList->cValues    = lNumSelect;
    lpEntryList->lpbin      = (LPSBinary)PvAllocMore(lNumSelect * sizeof(SBinary), lpEntryList);

    for(j=0; j < lNumSelect; j++)
    {
        // set which columns are important to see in table
        m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,rgItems[j],&lRowsSeeked);

        wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[j],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        m_hResult = m_lpMessageTable->QueryRows( 1, 0, &lpRows );
        wsprintf(m_szLogBuf,"lpMessageTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpMessageTable->QueryRows", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        // rip out info
        if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
            dRet = MsgDeleteDlg.m_CBContents3.Add( lpRows->aRow[0].lpProps[0].Value.lpszA );

        if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
        {
            lpEntryList->lpbin[j].cb  = (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb;
            lpEntryList->lpbin[j].lpb = (BYTE *)PvAllocMore(
                                        lpRows->aRow[0].lpProps[1].Value.bin.cb,
                                        lpEntryList);

            memcpy(lpEntryList->lpbin[j].lpb,
              (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb,
              (size_t) (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb);
            // _fmemcpy for 16 bit ??????
        }

        if(lpRows)
        {
             FreeRowSet(lpRows);
             lpRows=NULL;
        }
    }


    dRet = MsgDeleteDlg.m_CBContents2.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = MsgDeleteDlg.m_CBContents2.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( MsgDeleteDlg.DoModal() == IDOK )
    {

        // determine state/settings of data in dialog upon closing

        if( !lstrcmp(MsgDeleteDlg.m_szCB2,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( MsgDeleteDlg.m_bFlag1 )
            ulFlags |= MESSAGE_DIALOG;

        if( MsgDeleteDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->DeleteMessages(
                        lpEntryList,
                        ulUIParam,
                        lpProgress,
                        ulFlags);

        wsprintf(m_szLogBuf,"lpFolder->DeleteMessages(0x%X,%lu,0x%X,%lu,&ulErrorIndex)\t SC: %s",
                        lpEntryList,ulUIParam,lpProgress,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->DeleteMessages()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }

    goto Cleanup;

Associated:


    // ASSOCIATED MESSAGES

    lNumSelect  = -1;

    // initalize data for dialog box
    if( (lNumSelect = SendDlgItemMessage(IDC_FLD_ASS,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) < 1 )
    {
        MessageBox("Select one or several Messages in (Msg or Associated Msg)Listbox to Delete", "OnMsgDelete", MBS_ERROR );
        goto Cleanup;
    }

    lpEntryList = (LPENTRYLIST) PvAlloc(sizeof(ENTRYLIST));
    lpEntryList->cValues    = lNumSelect;
    lpEntryList->lpbin      = (LPSBinary)PvAllocMore(lNumSelect * sizeof(SBinary), lpEntryList);

    for(j=0; j < lNumSelect; j++)
    {
         // set which columns are important to see in table
        m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,rgItems[j],&lRowsSeeked);

        wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[j],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        m_hResult = m_lpAssociatedTable->QueryRows( 1, 0, &lpRows );
        wsprintf(m_szLogBuf,"lpAssociatedTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpAssociatedTable->QueryRows", m_hResult),
                                "Client", MBS_ERROR );
            goto Cleanup;
        }

        // rip out info
        if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
            dRet = MsgDeleteDlg.m_CBContents3.Add( lpRows->aRow[0].lpProps[0].Value.lpszA );

        if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
        {
            lpEntryList->lpbin[j].cb  = (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb;
            lpEntryList->lpbin[j].lpb = (BYTE *)PvAllocMore(
                                        lpRows->aRow[0].lpProps[1].Value.bin.cb,
                                        lpEntryList);

            memcpy(lpEntryList->lpbin[j].lpb,
              (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb,
              (size_t) (ULONG) lpRows->aRow[0].lpProps[1].Value.bin.cb);
            // _fmemcpy for 16 bit ??????
        }

        if(lpRows)
        {
             FreeRowSet(lpRows);
             lpRows=NULL;
        }
    }

    dRet = MsgDeleteDlg.m_CBContents2.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = MsgDeleteDlg.m_CBContents2.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( MsgDeleteDlg.DoModal() == IDOK )
    {

        // determine state/settings of data in dialog upon closing

        if( !lstrcmp(MsgDeleteDlg.m_szCB2,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;

        if( MsgDeleteDlg.m_bFlag1 )
            ulFlags |= MESSAGE_DIALOG;

        if( MsgDeleteDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpFolder->DeleteMessages(
                        lpEntryList,
                        ulUIParam,
                        lpProgress,
                        ulFlags);

        wsprintf(m_szLogBuf,"lpFolder->DeleteMessages(0x%X,%lu,0x%X,%lu,&ulErrorIndex)\t SC: %s",
                        lpEntryList,ulUIParam,lpProgress,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))
        {
            MessageBox( m_E.SzError("m_lpFolder->DeleteMessages()", m_hResult),
                            "Client", MBS_ERROR );
            goto Cleanup;
        }
    }


Cleanup:

    PvFree(lpEntryList);

    if(lpRows)
        FreeRowSet(lpRows);
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnOpenChildFolder
 *
 *  Purpose:
 *      Allow user to Open a Child Folder with a double click or button
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CFolderDlg::OnOpenChildFolder()
{
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpChildFolder   = NULL;
    LPENTRYID       lpChildEntryID  = NULL;
    ULONG           ulChildEntryID  = 0;
    LONG            lSelection      = -1;
    LPSRowSet       lpRows          = NULL;
    CFolderDlg      *lpDlgChildFld  = NULL;
    char            szBuffer[80];

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(2,sptaDisplayName) =
    {
        2,
        {
            PR_DISPLAY_NAME,
            PR_ENTRYID
        }
    };


    // get selected item from child folder listbox
    if( (lSelection = SendDlgItemMessage(IDC_FLDCHILD,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Folder in Listbox to Open", "OnOpenChildFolder", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpChildTable->SeekRow( BOOKMARK_BEGINNING,lSelection,NULL);

    wsprintf(m_szLogBuf,"lpChildTable->SeekRow( BOOKMARK_BEGINNING,%ld,NULL)\t SC: %s",
                        lSelection,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }


    m_hResult = m_lpChildTable->QueryRows( 1, 0, &lpRows );
    wsprintf(m_szLogBuf,"lpChildTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpChildTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    // rip out info
    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpChildEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulChildEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;
    }

    // openentry on the entryid
    m_hResult = m_lpFolder->OpenEntry( ulChildEntryID , lpChildEntryID, NULL,
                         ulAccess, &ulObjType, (IUnknown**)&lpChildFolder);
    wsprintf(m_szLogBuf,"lpFolder->OpenEntry(%lu,0x%X,NULL,%lu,&ulObjType,&lpChildFolder)\t SC: %s",
                        ulChildEntryID,lpChildEntryID,ulAccess,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("OpenEntry on Folder", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }

    // if it is a folder, find it's child folders
    if( ulObjType == MAPI_FOLDER )
    {
        if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_DISPLAY_NAME )
        {
            wsprintf(szBuffer,"%s - %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),
                    lpRows->aRow[0].lpProps[0].Value.lpszA);
        }
        else
        {
            wsprintf(szBuffer,"%s - %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),
                    "BUG BUG !Unknown Folder Name");
        }
        lpDlgChildFld   =  new CFolderDlg(szBuffer,(LPMAPIFOLDER)lpChildFolder,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpFolder != MAPI_FOLDER","Client",MBS_ERROR);
        goto Error;
    }

    FreeRowSet(lpRows);

    return;

Error:

    FreeRowSet(lpRows);
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnOpenMessage
 *
 *  Purpose:
 *      Allow user to Open a Message with a DoubleClick or button
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CFolderDlg::OnOpenMessage()
{
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpMessage       = NULL;
    LPENTRYID       lpChildEntryID  = NULL;
    ULONG           ulChildEntryID  = 0;
    int             iRow            = 0;
    LONG            lSelection      = -1;
    LPSRowSet       lpRows          = NULL;
    CMessageDlg     *lpMsgDlg       = NULL;
    char            szBuffer[256];
    LONG            lRowsSeeked     = 0;
    int max=500;
    int rgItems[500];

    // initalize data for dialog box
    if( (lSelection = SendDlgItemMessage(IDC_FLDMESSAGES,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
    {
        MessageBox("Select ONLY one Message in Listbox to Open", "OnOpenMessage", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpMessageTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpMessageTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpMessageTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }


    m_hResult = m_lpMessageTable->QueryRows( 1, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpMessageTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult) )
    {
        MessageBox( m_E.SzError("m_lpMessageTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }

    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpChildEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulChildEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;
    }

    // openentry on the entryid
    m_hResult = m_lpFolder->OpenEntry( ulChildEntryID , lpChildEntryID, NULL,
                         ulAccess, &ulObjType, (IUnknown**)&lpMessage);
    wsprintf(m_szLogBuf,"lpFolder->OpenEntry(%lu,0x%X,NULL,%lu,&ulObjType,&lpMessage)\t SC: %s",
                        ulChildEntryID,lpChildEntryID,ulAccess,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("OpenEntry on Folder", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }

    // if it's a message display a dialog
    if( ulObjType == MAPI_MESSAGE )
    {
        if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
        {
            wsprintf(szBuffer,"%s - %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),
                    lpRows->aRow[0].lpProps[0].Value.lpszA);
        }
        else
        {

            wsprintf(szBuffer,"%s - %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),
                    &(lpRows->aRow[0].lpProps[1]) );
        }
        lpMsgDlg   =  new CMessageDlg(szBuffer,(LPMESSAGE)lpMessage,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpMessage != MAPI_MESSAGE","Client",MBS_ERROR);
        goto Error;
    }

    FreeRowSet(lpRows);

    return;

Error:

    FreeRowSet(lpRows);

    if(lpMessage)
    {
        lpMessage->Release();
        lpMessage = NULL;
    }
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnOpenAssociated
 *
 *  Purpose:
 *      Allow user to Open a Message with a DoubleClick or button
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CFolderDlg::OnOpenAssociated()
{
    ULONG           ulObjType       = 0;
    LPMAPIPROP      lpMessage       = NULL;
    LPENTRYID       lpChildEntryID  = NULL;
    ULONG           ulChildEntryID  = 0;
    int             iRow            = 0;
    LONG            lSelection      = -1;
    LPSRowSet       lpRows          = NULL;
    CMessageDlg     *lpMsgDlg       = NULL;
    char            szBuffer[256];
    LONG            lRowsSeeked     = 0;
    int max=500;
    int rgItems[500];

    // initalize data for dialog box
    if( (lSelection = SendDlgItemMessage(IDC_FLD_ASS,LB_GETSELITEMS, (WPARAM)max, (LPARAM)(LPINT)rgItems )) != 1 )
    {
        MessageBox("Select ONLY one Message in Listbox to Open", "OnOpenMessage", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,rgItems[0],&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpAssociatedTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        rgItems[0],
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->SeekRow", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }


    m_hResult = m_lpAssociatedTable->QueryRows( 1, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpAssociatedTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult) )
    {
        MessageBox( m_E.SzError("m_lpAssociatedTable->QueryRows", m_hResult),
                            "Client", MBS_ERROR );
        goto Error;
    }


    if( lpRows->aRow[0].lpProps[1].ulPropTag == PR_ENTRYID )
    {
        lpChildEntryID = (LPENTRYID) lpRows->aRow[0].lpProps[1].Value.bin.lpb ;
        ulChildEntryID = (ULONG)     lpRows->aRow[0].lpProps[1].Value.bin.cb  ;
    }

    // openentry on the entryid
    m_hResult = m_lpFolder->OpenEntry( ulChildEntryID , lpChildEntryID, NULL,
                         ulAccess, &ulObjType, (IUnknown**)&lpMessage);
    wsprintf(m_szLogBuf,"lpFolder->OpenEntry(%lu,0x%X,NULL,%lu,&ulObjType,&lpMessage)\t SC: %s",
                        ulChildEntryID,lpChildEntryID,ulAccess,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("OpenEntry on Folder", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }

    // if it's a message display a dialog
    if( ulObjType == MAPI_MESSAGE )
    {
        if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
        {
            wsprintf(szBuffer,"%s - %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),
                    lpRows->aRow[0].lpProps[0].Value.lpszA);
        }
        else
        {

            wsprintf(szBuffer,"%s - %s",
                    GetString("MAPIObjTypes",ulObjType,NULL),
                    &(lpRows->aRow[0].lpProps[1]) );
        }
        lpMsgDlg   =  new CMessageDlg(szBuffer,(LPMESSAGE)lpMessage,this);
    }
    else
    {
        MessageBox("ulObjType of OpenEntry on lpMessage != MAPI_MESSAGE","Client",MBS_ERROR);
        goto Error;
    }

    FreeRowSet(lpRows);

    return;

Error:

    FreeRowSet(lpRows);

    if(lpMessage)
    {
        lpMessage->Release();
        lpMessage = NULL;
    }
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -
 -  OnProperty
 -  OnMessage
 -  OnChildFolder
 -  OnCurrentFolder
 -  SetEnableState
 *
 *  Purpose:
 *      Enable/disable appropriate buttons, set focus of appropiate listboxs
 *
 */
/*******************************************************************/

 
void CFolderDlg::OnProperty()
{
 
    DWORD   dwIndex;

    // disable other listboxes
    dwIndex = SendDlgItemMessage(IDC_FLDCURRENT, LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLDCHILD,   LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,LB_SETSEL,(WPARAM)FALSE ,(LPARAM) -1);
    dwIndex = SendDlgItemMessage(IDC_FLD_ASS,    LB_SETSEL,(WPARAM)FALSE ,(LPARAM) -1);
    
}


/*******************************************************************/

  
void CFolderDlg::OnMessage()
{
    DWORD   dwIndex;


    // disable other listboxes
    dwIndex = SendDlgItemMessage(IDC_FLDCURRENT, LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLDCHILD,   LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_PROPS,      LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLD_ASS,    LB_SETSEL,(WPARAM)FALSE ,(LPARAM) -1);

}


/*******************************************************************/
 
void CFolderDlg::OnAssociated()
{
    DWORD   dwIndex;


    // disable other listboxes
    dwIndex = SendDlgItemMessage(IDC_FLDCURRENT, LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLDCHILD,   LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_PROPS,      LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,LB_SETSEL,(WPARAM)FALSE ,(LPARAM) -1);

}

/*******************************************************************/
 
void CFolderDlg::OnChildFolder()
{
    DWORD   dwIndex;

    // disable other listboxes
    dwIndex = SendDlgItemMessage(IDC_FLDCURRENT, LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_PROPS,      LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwIndex = SendDlgItemMessage(IDC_FLDMESSAGES,LB_SETSEL,(WPARAM)FALSE ,(LPARAM) -1);
    dwIndex = SendDlgItemMessage(IDC_FLD_ASS,    LB_SETSEL,(WPARAM)FALSE ,(LPARAM) -1);
}


/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Closes the Folder dialog.
 *
 */
/*******************************************************************/

void CFolderDlg::OnCancel()
{
    delete this;
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  Cleanup()
 -
 *  Purpose:
 *      Destructor for class CFolderDlg and CPropDlg call this
 *      function that releases and Frees memory
 *      allocated in class
 *
 */
/*******************************************************************/

void CFolderDlg::Cleanup()
{
#ifdef XVPORT
    UnadviseAll(m_lpChildTable,     MAPI_TABLE,"HeirarchyTable lpTable",    &m_lpNotifHeirTbl);
    UnadviseAll(m_lpMessageTable,   MAPI_TABLE,"ContentsTable lpTable",     &m_lpNotifContTbl);
    UnadviseAll(m_lpAssociatedTable,MAPI_TABLE,"AssociatedTable lpTable",   &m_lpNotifAssTbl);
    UnadviseAll(lpMDB,              MAPI_STORE,"Folder Objs lpMDB",         &m_lpNotifFld);

#endif

    // table obtained from GetHeirarchyTable
    if(m_lpChildTable)
    {
        m_lpChildTable->Release();
        m_lpChildTable = NULL;
    }

    // table obtained from GetContentsTable
    if(m_lpMessageTable)
    {
        m_lpMessageTable->Release();
        m_lpMessageTable = NULL;
    }

    // table obtained from GetContentsTable Associated
    if(m_lpAssociatedTable)
    {
        m_lpAssociatedTable->Release();
        m_lpAssociatedTable = NULL;
    }

    // folder object
    if(m_lpFolder)
    {
        m_lpFolder->Release();
        m_lpFolder = NULL;
    }
}

/*******************************************************************/
/*
 -  CFolderDlg::
 -  OnSpecialProps
 -
 *  Purpose:
 *      Closes the Folder dialog.
 *
 */
/*******************************************************************/

void CFolderDlg::OnSpecialProps()
{
    CFldSpecialDlg    FldSpecial(m_lpFolder,this);
    FldSpecial.DoModal();
}

/*******************************************************************/
/*********************** FldSpecial **************************/

/********************************************************************/
/*
 -  CFldSpecialDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CFldSpecialDlg::OnInitDialog()
{
    CGetError       E;
    DWORD           dwReturn            = 0;
    HRESULT         hResult             = hrSuccess;
    char            szBuffer[300];
    ULONG           cVals               = 0;
    LPSPropValue    lpspva              = NULL;
    ULONG           i;

    
    SizedSPropTagArray(7,sptaFldSpecial) =
    {
        7,
        {
            PR_DISPLAY_NAME,
            PR_FOLDER_TYPE,
            PR_OBJECT_TYPE,
            PR_ACCESS,
            PR_ACCESS_LEVEL,
            PR_SUBFOLDERS,
            PR_STATUS,
        }
    };



    hResult = m_lpFolder->GetProps((LPSPropTagArray)&sptaFldSpecial,0,&cVals,&lpspva);
    if( HR_FAILED(hResult) )
    {
        MessageBox( E.SzError("lpMDB->GetProps(sptaFldSpecial)", hResult),
                 "Client", MBS_ERROR );
        return FALSE;
    }

    i = 0;

    // PR_DISPLAY_NAME
    if(lpspva[i].ulPropTag != PR_DISPLAY_NAME )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }            
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_DISPLAY_NAME1,szBuffer);
    i++;

    // PR_FOLDER_TYPE
    if(lpspva[i].ulPropTag != PR_FOLDER_TYPE )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }            
    else
        GetString( "FolderType", lpspva[i].Value.ul, szBuffer );
    SetDlgItemText(IDT_FOLDER_TYPE1,szBuffer);
    i++;

    // PR_OBJECT_TYPE
    if(lpspva[i].ulPropTag != PR_OBJECT_TYPE )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }            
    else
        GetString( "MAPIObjTypes", lpspva[i].Value.ul, szBuffer );
    SetDlgItemText(IDT_OBJECT_TYPE1,szBuffer);
    i++;


    // PR_ACCESS
    if(lpspva[i].ulPropTag != PR_ACCESS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
        SetDlgItemText(IDT_ACCESS1,szBuffer);
    }
    else
    {
        if( (lpspva[i].Value.ul & MAPI_ACCESS_MODIFY) )
            SetDlgItemText(IDT_ACCESS1,"MAPI_ACCESS_MODIFY");
            
        if( (lpspva[i].Value.ul & MAPI_ACCESS_READ) )
            SetDlgItemText(IDT_ACCESS2,"MAPI_ACCESS_READ");
            
        if( (lpspva[i].Value.ul & MAPI_ACCESS_DELETE) )
            SetDlgItemText(IDT_ACCESS3,"MAPI_ACCESS_DELETE");
            
        if( (lpspva[i].Value.ul & MAPI_ACCESS_CREATE_HIERARCHY) )        
            SetDlgItemText(IDT_ACCESS4,"MAPI_ACCESS_CREATE_HIERARCHY");
            
        if( (lpspva[i].Value.ul & MAPI_ACCESS_CREATE_CONTENTS) )
            SetDlgItemText(IDT_ACCESS5,"MAPI_ACCESS_CREATE_CONTENTS");
            
        if( (lpspva[i].Value.ul & MAPI_ACCESS_CREATE_ASSOCIATED) )
            SetDlgItemText(IDT_ACCESS6,"MAPI_ACCESS_CREATE_ASSOCIATED");
    }
    i++;

    // PR_ACCESS_LEVEL
    if(lpspva[i].ulPropTag != PR_ACCESS_LEVEL )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }            
    else
    {
        if(lpspva[i].Value.ul == MAPI_MODIFY )
            wsprintf(szBuffer,"MAPI_MODIFY");
        else
            wsprintf(szBuffer,"Read Only");
    }            
    SetDlgItemText(IDT_ACCESS_LEVEL1,szBuffer);
    i++;

    // PR_SUBFOLDERS
    if(lpspva[i].ulPropTag != PR_SUBFOLDERS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }            
    else
    {
        if(lpspva[i].Value.b == TRUE )
            wsprintf(szBuffer,"YES");
        else
            wsprintf(szBuffer,"NO");
    }            
    SetDlgItemText(IDT_SUBFOLDERS1,szBuffer);
    i++;


    // PR_STATUS (NOTE, should I call some other function to get this ?)
    if(lpspva[i].ulPropTag != PR_STATUS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
        SetDlgItemText(IDT_STATUS1,szBuffer);
    }
    else
    {
        if( (lpspva[i].Value.ul & FLDSTATUS_HIGHLIGHTED) )
            SetDlgItemText(IDT_STATUS1,"FLDSTATUS_HIGHLIGHTED");

        if( (lpspva[i].Value.ul & FLDSTATUS_TAGGED) )
            SetDlgItemText(IDT_STATUS2,"FLDSTATUS_TAGGED");

        if( (lpspva[i].Value.ul & FLDSTATUS_HIDDEN) )
            SetDlgItemText(IDT_STATUS3,"FLDSTATUS_HIDDEN");

        if( (lpspva[i].Value.ul & FLDSTATUS_DELMARKED) )
            SetDlgItemText(IDT_STATUS4,"FLDSTATUS_DELMARKED");
    }
    i++;

    MAPIFreeBuffer(lpspva);

    return TRUE;
}


/*******************************************************************/
/*
 -  CFldSpecialDlg::
 -  ~CFldSpecialDlg
 -
 *  Purpose:
 *      Destructor for class CFldSpecialDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CFldSpecialDlg::~CFldSpecialDlg()
{

}


/*******************************************************************/
/*
 -  CFldSpecialDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CFldSpecialDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}

