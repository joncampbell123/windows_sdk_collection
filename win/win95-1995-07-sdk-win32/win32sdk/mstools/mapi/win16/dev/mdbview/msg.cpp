/*******************************************************************/
/*
 -  msg.cpp
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CMessageDlg
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

#include <mapix.h>      
#ifdef WIN16
#include <mapiwin.h>    
#endif
#include <strtbl.h>     
#include <misctool.h>
#include <pvalloc.h>
#include "resource.h"   
#include "mdbview.h" 
#include "msg.h"
#include "attach.h"
#include "oper.h"

#include <string.h> // for _stricmp()

/***** Table viewer TBLVU functions ***********/

extern  HINSTANCE    hlibTBLVU;

typedef BOOL (*LPFNVIEWMAPITABLE)(
    LPMAPITABLE FAR     *lppMAPITable,
    HWND                hWnd);    

extern  LPFNVIEWMAPITABLE lpfnViewMapiTable;
#define TBLVUViewMapiTable (*lpfnViewMapiTable)


/***** Property viewer PROPVU functions *******/

extern HINSTANCE    hlibPROPVU;

typedef BOOL (*LPFNVIEWMAPIPROP)(
    LPSTR           lpszName,
    LPMAPIPROP FAR *lppMAPIProp,
    LPVOID          lpvDest,
    HWND            hWnd);    

extern LPFNVIEWMAPIPROP lpfnViewMapiProp;
#define PROPVUViewMapiProp (*lpfnViewMapiProp)

/******************* global variables ***************************/

extern LPMAPISESSION        lpSession;
extern LPMDB                lpMDB;
extern LPADRBOOK            lpAdrBook;
extern HRESULT              hResultSession;
extern LPVOID               lpvCopyToDest;
extern ULONG                ulAccess;



#ifdef XVPORT
extern "C"
{
extern NOTIFCALLBACK LogNotifToXVPLog;
}

#endif
extern "C"
{
extern NOTIFCALLBACK RedrawRecipientTable;
}

extern "C"
{
extern NOTIFCALLBACK RedrawAttachmentTable;
}

/******************* Msg Message Map ****************************/

BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)

    ON_LBN_DBLCLK(  IDC_PROPS,          OnPropInterface)
    ON_LBN_DBLCLK(  IDC_MSGATTACH,      OnOpenAttachLB)
    ON_LBN_DBLCLK(  IDC_MSGRECIPIENTS,  OnRecipTable)
    ON_COMMAND(     IDC_CALLFUNC,       OnCallFunction)
    ON_COMMAND(     IDC_PROPINTERFACE,  OnPropInterface)
    ON_COMMAND(     IDC_MSG_X400_PROPS, OnX400Props)
    ON_COMMAND(     IDC_SPECIALPROPS,   OnSpecialProps)
                                        
END_MESSAGE_MAP() 



/**************************** Message ******************************/
/********************************************************************/
/********************* Callbacks ************************************/

SCODE STDAPICALLTYPE RedrawRecipientTable(  LPVOID          lpvContext, 
                                            ULONG           cNotif, 
                                            LPNOTIFICATION  lpNotif)
{
    ((CMessageDlg *)(lpvContext))->DisplayRecips();
    return(S_OK);
}                             

/********************************************************************/

SCODE STDAPICALLTYPE RedrawAttachmentTable(  LPVOID          lpvContext, 
                                            ULONG           cNotif, 
                                            LPNOTIFICATION  lpNotif)
{
    ((CMessageDlg *)(lpvContext))->DisplayAttachments();
    return(S_OK);
}                             



/********************************************************************/
/*
 -  CMessageDlg::
 -  OnNotifMsg
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnNotifMsg()
{       
    CNotifDlg   Notif(this);
    
    Notif.m_Description = m_CurrentMessage;
    Notif.m_lpUnk       = lpMDB;
    Notif.m_ulObjType   = MAPI_STORE;    
    Notif.m_lppNotifs   = &m_lpNotifMsg;
    Notif.DoModal();
    
}
/********************************************************************/
/*
 -  CMessageDlg::
 -  OnNotifAttTbl
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnNotifAttTbl()
{       

    CNotifDlg   Notif(this);
    
    Notif.m_Description = "AttachmentTable Notification";
    Notif.m_lpUnk       = m_lpAttachTable;
    Notif.m_ulObjType   = MAPI_TABLE;
    Notif.m_lppNotifs   = &m_lpNotifAttTbl;
    Notif.DoModal();
}


/********************************************************************/
/*
 -  CMessageDlg::
 -  OnNotifRecipTbl
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnNotifRecipTbl()
{       
    CNotifDlg   Notif(this);
    
    Notif.m_Description = "RecipientTable Notification";
    Notif.m_lpUnk       = m_lpRecipTable;
    Notif.m_ulObjType   = MAPI_TABLE;
    Notif.m_lppNotifs   = &m_lpNotifRecipTbl;
    Notif.DoModal();
}


/*******************************************************************/
/*
 -  CMessageDlg::
 -  OnX400Props
 -
 *  Purpose:
 *      Closes the Message dialog.
 *
 */
/*******************************************************************/

void CMessageDlg::OnX400Props()
{
    LPSPropValue    lpspva          = NULL;
    ULONG           cValues         = 0;

    // switch based upon PR_MESSAGE_CLASS 
    //   IPM.Note               (IPM message)
    //   REPORT.IPM.Note.IPNRN  (IPN Receipt Notification)
    //   REPORT.IPM.Note.IPNNRN (IPN Non Receipt Notification)
    //   Other ???

    SizedSPropTagArray(1,sptaMsgClass) = 
    {
        1, 
        { 
            PR_MESSAGE_CLASS,
        }
    };
    
    m_hResult = m_lpMessage->GetProps( (LPSPropTagArray) &sptaMsgClass,0,&cValues, &lpspva);
    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("CMessageDlg::OnX400Props()  m_lpMessage->GetProps(&sptaMsgClass)", m_hResult),
                     "Client", MBS_ERROR );
        return;
    }
    
    if(lpspva[0].ulPropTag != PR_MESSAGE_CLASS )
    {
        MessageBox( "lpspva[0].ulPropTag != PR_MESSAGE_CLASS", "Client", MBS_ERROR );
        return;
    }
    
    if( !_stricmp(lpspva[0].Value.lpszA,"IPM.Note" ) )
    {
        CIPMSpecialDlg    MsgIPMX400(m_lpMessage,this);
        MsgIPMX400.DoModal();
    }        
    else if( !_stricmp(lpspva[0].Value.lpszA,"REPORT.IPM.Note.IPNRN" ) )
    {
        CIPNSpecialDlg    MsgIPNRNX400(m_lpMessage,this);
        MsgIPNRNX400.DoModal();
    }        
    else if( !_stricmp(lpspva[0].Value.lpszA,"REPORT.IPM.Note.IPNNRN" ) )
    {
        CIPNSpecialDlg    MsgIPNNRNX400(m_lpMessage,this);
        MsgIPNNRNX400.DoModal();
    }        
    else // this is .DR and .NDR, and others
    {
        CIPMSpecialDlg    MsgDefaultX400(m_lpMessage,this);
        MsgDefaultX400.DoModal();
    }        
}

/*******************************************************************/
/*
 -  CMessageDlg::
 -  OnSpecialProps
 -
 *  Purpose:
 *      Closes the Message dialog.
 *
 */
/*******************************************************************/

void CMessageDlg::OnSpecialProps()
{
    CMsgSpecialDlg    MsgSpecial(m_lpMessage,this);
    MsgSpecial.DoModal();
}

/********************************************************************/
/*
 -  CMessageDlg::
 -  OnPropInterface
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnPropInterface()
{       
    char    szBuffer[80];
    
    
    lstrcpy(szBuffer,m_CurrentMessage.GetBuffer(50));
    lstrcat(szBuffer," Properties");

    PROPVUViewMapiProp(szBuffer, 
                (LPMAPIPROP FAR *)&m_lpEntry,lpvCopyToDest, (HWND)this->m_hWnd );
                
}


/********************************************************************/
/*
 -  CMessageDlg::
 -  OnShowForm
 -
 *  Purpose:
 *  
                LPSERVICEADMIN FAR *        lppServiceAdmin) IPURE;     \
    MAPIMETHOD(ShowForm)                                                \
        (THIS_  ULONG                       ulUIParam,                  \
                LPMDB                       lpMsgStore,                 \
                LPMAPIFOLDER                lpParentFolder,             \
                LPCIID                      lpInterface,                \
                ULONG                       ulMessageToken,             \
                LPMESSAGE                   lpMessageSent,              \
                ULONG                       ulFlags,                    \
                ULONG                       ulMessageStatus,            \
                ULONG                       ulMessageFlags,             \
                ULONG                       ulAccess,                   \
                LPSTR                       lpszMessageClass) IPURE;    \
    MAPIMETHOD(PrepareForm)                                             \
        (THIS_  LPCIID                      lpInterface,                \
                LPMESSAGE                   lpMessage,                  \
                ULONG FAR *                 lpulMessageToken) IPURE;    \



 /********************************************************************/

void CMessageDlg::OnShowForm()
{   
    COperation      MsgShowFormDlg(this);
    CString         OperName;
    LPMAPIFOLDER    lpParentFld         = NULL;
    ULONG           ulFlags             = 0;
    ULONG           ulMessageStatus     = 0;
    LPSPropValue    lpspvaShow          = NULL;
    ULONG           cValuesShow         = 0;
    HRESULT         hResult1            = hrSuccess;
    ULONG           ulObjType           = 0;
    DWORD           dRet                = 0;
    ULONG           ulMessageToken      = 0;
    char            szAccess[80];
    char            szMsgFlags[80];
    char            szMsgStatus[512];
    char            szMsgClass[80];

    szAccess[0]     = '\0';
    szMsgFlags[0]   = '\0';
    szMsgStatus[0]  = '\0';
    szMsgClass[0]   = '\0';

    SizedSPropTagArray(5,sptaShowForm) = 
    {
        5, 
        { 
            PR_MESSAGE_FLAGS,
            PR_MESSAGE_CLASS,
            PR_ACCESS,
            PR_PARENT_ENTRYID,
            PR_ENTRYID,
        }
    };
    
    // GET REQUIRED PROPERTIES FROM MESSAGE    
    m_hResult = m_lpMessage->GetProps( (LPSPropTagArray) &sptaShowForm,0,&cValuesShow, &lpspvaShow);
    wsprintf(m_szLogBuf,"m_lpMessage->GetProps(&sptaShowForm,0, &cValuesShow, &lpspvaShow)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("CMessageDlg::OnShowForm()  m_lpMessage->GetProps(&sptaShowForm)", m_hResult),
                     "Client", MBS_ERROR );
        return;
    }

    // OPEN UP PARENT FOLDER
    if(lpspvaShow[3].ulPropTag != PR_PARENT_ENTRYID)
    {
        MessageBox("CMessageDlg::ShowForm  lpspvaShow[3].ulPropTag != PR_PARENT_ENTRYID",
                        "Client", MBS_ERROR );
        return;
    }
    hResult1 = lpMDB->OpenEntry(    (ULONG)     lpspvaShow[3].Value.bin.cb,
                                    (LPENTRYID) lpspvaShow[3].Value.bin.lpb,
                                    NULL,
                                    ulAccess,
                                    &ulObjType,
                                    (LPUNKNOWN*)(LPMAPIPROP *) &lpParentFld);
    if( HR_FAILED(hResult1) )
    {
        MessageBox( m_E.SzError("lpMDB->OpenEntry() Parent Folder",hResult1),
                        "Client", MBS_ERROR );
        return;
    }
    if( ulObjType != MAPI_FOLDER )
    {
        MessageBox("ulObjType of OpenEntry on lpFolder != MAPI_FOLDER","Client",MBS_ERROR);
        return;
    }
        
    // initalize data for dialog box
    OperName  = "lpSession->ShowForm()";

    MsgShowFormDlg.m_CurrentOperation= OperName;

    MsgShowFormDlg.m_CBText1         = "ulUIParam";
    MsgShowFormDlg.m_EditText1       = "lpszMessageClass:";
    MsgShowFormDlg.m_EditText2       = "ulMessageStatus:";
    MsgShowFormDlg.m_EditText3       = "ulMessageFlags:";
    MsgShowFormDlg.m_CBText1         = "ulAccess:";

    MsgShowFormDlg.m_FlagText1       = "MAPI_POST_MESSAGE";
    MsgShowFormDlg.m_FlagText2       = "MAPI_NEW_MESSAGE";
    MsgShowFormDlg.m_FlagText3       = "Invalid Flag";


    // PR_MESSAGE_FLAGS
    if(lpspvaShow[0].ulPropTag != PR_MESSAGE_FLAGS)
    {
        MessageBox("CMessageDlg::ShowForm  lpspvaShow[0].ulPropTag != PR_MESSAGE_FLAGS",
                        "Client", MBS_ERROR );
        return;
    }
    wsprintf(szMsgFlags,"%lu", lpspvaShow[0].Value.ul);
    MsgShowFormDlg.m_EditDefault3    = szMsgFlags;


    // PR_MESSAGE_CLASS
    if(lpspvaShow[1].ulPropTag != PR_MESSAGE_CLASS)
    {
        MessageBox("CMessageDlg::ShowForm  lpspvaShow[1].ulPropTag != PR_MESSAGE_CLASS",
                        "Client", MBS_ERROR );
        return;
    }
    wsprintf(szMsgClass,"%s",lpspvaShow[1].Value.lpszA);
    MsgShowFormDlg.m_EditDefault1    = szMsgClass;

    // PR_ACCESS
    if(lpspvaShow[2].ulPropTag != PR_ACCESS)
    {
        MessageBox("CMessageDlg::ShowForm  lpspvaShow[2].ulPropTag != PR_ACCESS",
                        "Client", MBS_ERROR );
        return;
    }
    wsprintf(szAccess,"%lu",lpspvaShow[2].Value.ul);
    dRet = MsgShowFormDlg.m_CBContents1.Add(szAccess);




    // PR_MSG_STATUS FROM lpFld->GetMsgStatus()
    if(lpspvaShow[4].ulPropTag != PR_ENTRYID)
    {
        MessageBox("CMessageDlg::ShowForm  lpspvaShow[4].ulPropTag != PR_ENTRYID",
                        "Client", MBS_ERROR );
        return;
    }

    m_hResult = lpParentFld->GetMessageStatus(
                        (ULONG)     lpspvaShow[4].Value.bin.cb,
                        (LPENTRYID) lpspvaShow[4].Value.bin.lpb,
                        0,
                        &ulMessageStatus  );
    wsprintf(m_szLogBuf,"lpFolder->GetMessageStatus()\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))
    {
        MessageBox( m_E.SzError("lpParentFld->GetMessageStatus()", m_hResult),
                "Client", MBS_ERROR );
        return;
    }

    wsprintf(szMsgStatus,"ulMsgStatus == %#04X, %s%s%s%s",ulMessageStatus,
      ((ulMessageStatus & MSGSTATUS_HIGHLIGHTED)    ? "MSGSTATUS_HIGHLIGHTED | ": ""),
      ((ulMessageStatus & MSGSTATUS_TAGGED)         ? "MSGSTATUS_TAGGED | "     : ""),
      ((ulMessageStatus & MSGSTATUS_HIDDEN)         ? "MSGSTATUS_HIDDEN | "     : ""),
      ((ulMessageStatus & MSGSTATUS_DELMARKED)      ? "MSGSTATUS_DELMARKED | "  : ""));

    MsgShowFormDlg.m_EditDefault2    = szMsgStatus;

    // first get props appropriate for this message off prop list

    // bring up modal dialog box, and if user hits OK, process operation
    if( MsgShowFormDlg.DoModal() == IDOK )      
    {       
        // determine state/settings of data in dialog upon closing
                
        if( MsgShowFormDlg.m_bFlag1 )                
            ulFlags |= MAPI_POST_MESSAGE;

        if( MsgShowFormDlg.m_bFlag2 )                
            ulFlags |= MAPI_NEW_MESSAGE;

        if( MsgShowFormDlg.m_bFlag3 )                
            ulFlags |= TEST_INVALID_FLAG;

        hResultSession = lpSession->PrepareForm(NULL,m_lpMessage,&ulMessageToken);
        wsprintf(m_szLogBuf,"lpSession->PrepareForm()\t SC: %s",
                            GetString( "MAPIErrors", GetScode(hResultSession), m_szError ) );

        if( HR_FAILED(hResultSession))        
        {
            MessageBox( m_E.SzError("CMessageDlg::ShowForm()  lpSession->PrepareForm()", hResultSession),
                         "Client", MBS_ERROR );

            m_lpMessage->Release();
            m_lpMessage = NULL;
            Cleanup();
            OnCancel();

            return;
        }

        m_lpMessage->Release();
        m_lpMessage = NULL;


        hResultSession = lpSession->ShowForm( 
                            (ULONG) (void *) this->m_hWnd,
                            (LPMDB)          lpMDB,
                            (LPMAPIFOLDER)   lpParentFld,
                            (LPIID)          NULL,
                            (ULONG)          ulMessageToken,
                            (LPMESSAGE)      NULL,
                            (ULONG)          ulFlags,
                            (ULONG)          ulMessageStatus,
                            (ULONG)          lpspvaShow[0].Value.ul,
                            (ULONG)          lpspvaShow[2].Value.ul,
                            (LPSTR)          lpspvaShow[1].Value.lpszA);
                            
        wsprintf(m_szLogBuf,"lpSession->ShowForm()\t SC: %s",
                            GetString( "MAPIErrors", GetScode(hResultSession), m_szError ) );

        if( HR_FAILED(hResultSession))        
        {
            MessageBox( m_E.SzError("CMessageDlg::ShowForm()  lpSession->ShowForm()", hResultSession),
                         "Client", MBS_ERROR );
        }
    }

    m_lpMessage = NULL;
    Cleanup();
    OnCancel();

}

/********************************************************************/
/*
 -  CMessageDlg::
 -  DisplayMsgFlags
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::DisplayMsgFlags()
{       
    DWORD           dwReturn    = 0;
    ULONG           ulValues    = 0;
    LPSPropValue    lpPropValue = NULL;
    ULONG           ulNum       = 0;
    ULONG           idx;
    ULONG           ulID        = 0;
    char            szBuffer[80];
    char            szFlags[80];


    SizedSPropTagArray(1,sptaMsgFlags) = 
    {
        1, 
        { 
            PR_MESSAGE_FLAGS,
        }
    };

    dwReturn = SendDlgItemMessage(IDC_MSGFLAGS,LB_RESETCONTENT,0,0);


    // get the msg flags from the message
    m_hResult = m_lpMessage->GetProps( (LPSPropTagArray) &sptaMsgFlags,0,&ulValues, &lpPropValue);
    wsprintf(m_szLogBuf,"m_lpMessage->GetProps(&sptaMsgFlags,0, &ulValues, &lpPropValue)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("CMessageDlg::DisplayMsgFlags()  m_lpMessage->GetProps(&sptaMsgFlags)", m_hResult),
                     "Client", MBS_ERROR );
        return;
    }

    wsprintf(szFlags,"PR_MESSAGE_FLAGS == %lu",lpPropValue[0].Value.l);
    SetDlgItemText(IDT_MSGFLAGS,szFlags);

    // for each flag in stringtable 
    ulNum = GetRowCount("MsgFlags");     
    for(idx = 0; idx < ulNum; idx++)
    {     
        ulID = GetRowID("MsgFlags",idx);
        if( ulID & lpPropValue[0].Value.l)
        {
            GetRowString("MsgFlags",idx,szBuffer);            
            SendDlgItemMessage(IDC_MSGFLAGS, LB_ADDSTRING, 0, (LPARAM)szBuffer);
        }
    }
    
    if(lpPropValue)
    {
        MAPIFreeBuffer(lpPropValue);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpPropValue from GetProps" );
    }
}


/********************************************************************/
/*
 -  CMessageDlg::
 -  OnRecipientOptions
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnRecipientOptions()
{       
    LONG        lCurSelect  = 0;
    LONG        lRowsSeeked = 0;
    LPSRowSet   lpRows      = NULL;
    HRESULT     hResult1    = hrSuccess;
    LPADRENTRY  lpAdrEntry  = NULL;

    LPADRLIST       lpAdrList       = NULL;
    CAdrListDlg     AdrList1(this);
    CAdrListDlg     AdrList2(this);

    
    // get selected item from child folder listbox
    if( (lCurSelect = SendDlgItemMessage(IDC_MSGRECIPIENTS,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Recipient to see Options", "OnRecipintOptions", MBS_ERROR );
        return;
    }
    
    m_hResult = m_lpRecipTable->SeekRow( BOOKMARK_BEGINNING,lCurSelect,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpRecipTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        lCurSelect,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpRecipTable->SeekRow", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpRecipTable->QueryRows( 1, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpRecipTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    

    {
        MessageBox( m_E.SzError("m_lpRecipTable->QueryRows", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    // we can caste a LPRowSet to a LPADRLIST
    lpAdrList = (LPADRLIST) lpRows;

    // display address list before
    AdrList1.m_List       = "Address List of Recipient BEFORE RecipOptions";
    AdrList1.m_lpAdrList  = lpAdrList;
    AdrList1.DoModal();

    // a row of the recipient table is the same structure as a LPADRENTRY
    
    lpAdrEntry = (LPADRENTRY) &(lpRows->aRow[0]);
                 
    // in order to get anythign back from MAPI my lpAdrEntry must have a 
    // PR_ENTRYID
    // PR_DISPLAY_NAME
    // PR_ADRTYPE                    
                 
    hResult1 = lpAdrBook->RecipOptions(NULL,0,(LPADRENTRY)  lpAdrEntry );
    
    wsprintf(m_szLogBuf,"lpAdrBook->RecipientOptions(NULL,0,(LPADRENTRY) lpAdrEntry)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(hResult1), m_szError ) );

    if( HR_FAILED(hResult1))    

    {
        MessageBox( m_E.SzError("lpAdrBook->RecipientOptions", hResult1), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    // display address list before
    AdrList1.m_List       = "Address List of Recipient AFTER RecipOptions";
    AdrList1.m_lpAdrList->aEntries[0].cValues       = lpAdrEntry[0].cValues;
    AdrList1.m_lpAdrList->aEntries[0].rgPropVals    = lpAdrEntry[0].rgPropVals;
    AdrList1.m_lpAdrList->aEntries[0].ulReserved1   = lpAdrEntry[0].ulReserved1;
    AdrList1.m_lpAdrList->cEntries                  = 1;
    AdrList1.DoModal();


Error:

    hResult1 = hrSuccess;

}



/*******************************************************************/
/*
 -  CMessageDlg::
 -  OnSetCopyToDest
 *
 *  Purpose:
 *      Set Destination object of CopyTo operation or CopyMessage or CopyFolder
 *
 *  lpvCopyToDest and CopyToDest are global
 */
/*******************************************************************/

void CMessageDlg::OnSetCopyToDest()
{                
    char       szBuffer[256];
    
    lpvCopyToDest   = (LPVOID) m_lpMessage;       
    
    wsprintf(szBuffer,"%s is next Destination for CopyTo(), CopyMessage(), or CopyFolder()",
                m_CurrentMessage.GetBuffer(40) );

    MessageBox( szBuffer,"Client", MBS_INFO );
}    

/********************************************************************/
/*
 -  CMessageDlg::
 -  OnMDBProp
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnMDBProp()
{   
    PROPVUViewMapiProp("MDB Properties", 
            (LPMAPIPROP FAR *)&lpMDB,lpvCopyToDest, (HWND)this->m_hWnd );
}

/********************************************************************/
/*
 -  CMessageDlg::
 -  InitMsgDialog
 -
 *  Purpose:  
 *      Initialize the dialog after it was created in the PropDlg class
 *      This routine is called in the constructor in lei of OnInitDialog()
 *      so that it can fill in all of the appropriate listboxes, etc in
 *      the dialog.  We can't use the OnInitDialog because the OnInitDialog
 *      in the PropDialog Class and MFC doesn't explicitly call it for
 *      this inherited class as well.
 */
/********************************************************************/

BOOL CMessageDlg::InitMsgDialog()
{
    DWORD           dwReturn        = 0;
    LPSPropValue    lpspvaEID       = NULL;
    ULONG           ulValuesEID     = 0;
    LPSPropTagArray lpsptaAll       = NULL;
    DWORD           dwIndex         = 0;

    // PropTag Array for getting only attachment number USED IN SETCOLUMNS
    SizedSPropTagArray(1,sptaAttachNum) = 
    {
        1, 
        { 
            PR_ATTACH_NUM,
        }
    };

    
    SizedSPropTagArray(1,sptaEID) = 
    {
        1, 
        { 
            PR_ENTRYID,
        }
    };

    SetWindowText( m_CurrentMessage.GetBuffer(15) );
        
    // disable the system menu close item
    //  this is done because there is a MFC 2.0 bug that
    //  makes you capture several PostNcDestroy messages etc.
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED);

     
    m_hResult = m_lpMessage->GetProps( (LPSPropTagArray) &sptaEID,0, &ulValuesEID, &lpspvaEID);
    wsprintf(m_szLogBuf,"lpMessage->GetProps( &sptaEID,0, &ulValues, &lpspva)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("lpMessage->GetProps(&sptaEID)",m_hResult),
                     "Client", MBS_ERROR );
        goto Setup;                             
    }

    // notif window 
    wsprintf(m_szMsgContextCritical,"fnevCriticalError, lpMessage %s, lpMDB",
                    m_CurrentMessage.GetBuffer(15));

#ifdef XVPORT
    AdviseObj(  lpMDB, 
                MAPI_STORE, 
                LogNotifToXVPLog,
                fnevCriticalError,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szMsgContextCritical,
                NULL,
                &m_lpNotifMsg);

    wsprintf(m_szMsgContextCreated,"fnevObjectCreated, lpMessage %s, lpMDB",
                    m_CurrentMessage.GetBuffer(15));

    AdviseObj(  lpMDB, 
                MAPI_STORE, 
                LogNotifToXVPLog,
                fnevObjectCreated,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szMsgContextCreated,
                NULL,
                &m_lpNotifMsg);

    wsprintf(m_szMsgContextDeleted,"fnevObjectDeleted, lpMessage %s, lpMDB",
                    m_CurrentMessage.GetBuffer(15));

    AdviseObj(  lpMDB, 
                MAPI_STORE, 
                LogNotifToXVPLog,
                fnevObjectDeleted,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szMsgContextDeleted,
                NULL,
                &m_lpNotifMsg);

    wsprintf(m_szMsgContextModified,"fnevObjectModified, lpMessage %s, lpMDB",
                    m_CurrentMessage.GetBuffer(15));

    AdviseObj(  lpMDB, 
                MAPI_STORE, 
                LogNotifToXVPLog,
                fnevObjectModified,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szMsgContextModified,
                NULL,
                &m_lpNotifMsg);

    wsprintf(m_szMsgContextMoved,"fnevObjectMoved, lpMessage %s, lpMDB",
                    m_CurrentMessage.GetBuffer(15));

    AdviseObj(  lpMDB, 
                MAPI_STORE, 
                LogNotifToXVPLog,
                fnevObjectMoved,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szMsgContextMoved,
                NULL,
                &m_lpNotifMsg);

    wsprintf(m_szMsgContextCopied,"fnevObjectCopied, lpMessage %s, lpMDB",
                    m_CurrentMessage.GetBuffer(15));

    AdviseObj(  lpMDB, 
                MAPI_STORE, 
                LogNotifToXVPLog,
                fnevObjectCopied,
                (ULONG)     lpspvaEID[0].Value.bin.cb,
                (LPENTRYID) lpspvaEID[0].Value.bin.lpb,
                m_szMsgContextCopied,
                NULL,
                &m_lpNotifMsg);

#endif

    if(lpspvaEID)
        MAPIFreeBuffer(lpspvaEID);                


    // ATTACHMENT TABLE
    // get list of attachments in table
    m_hResult = m_lpMessage->GetAttachmentTable( 0 , &m_lpAttachTable );

    wsprintf(m_szLogBuf,"lpMessage->GetAttachmentTable(0, &m_lpAttachTable)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpMessage->GetAttachmentTable", m_hResult), 
                            "Client", MBS_ERROR );
        goto Recipients;
    }
    
        
    // redraw routine
    AdviseObj(  m_lpAttachTable,
                MAPI_TABLE, 
                RedrawAttachmentTable,
                fnevTableModified, 
                0,
                NULL,
                "Msg AttachTable Redraw",
                this, 
                &m_lpNotifAttTbl);       

    // notif window 
    wsprintf(m_szAttContext,"fnevTableModified, AttachmentTable of Message %s, lpTable",
                            m_CurrentMessage.GetBuffer(15));


#ifdef XVPORT

    AdviseObj(  m_lpAttachTable,
                MAPI_TABLE, 
                LogNotifToXVPLog,
                fnevTableModified, 
                0,
                NULL,
                m_szAttContext, 
                NULL,
                &m_lpNotifAttTbl);       

#endif
    m_hResult = m_lpAttachTable->SetColumns( (LPSPropTagArray) &sptaAttachNum,0);

    wsprintf(m_szLogBuf,"lpAttachTable->SetColumns( &sptaAttachNum,0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->SetColumns", m_hResult), 
                            "Client", MBS_ERROR );
    }

    DisplayAttachments();


Recipients:

    // RECIPIENT TABLE

    // get all recipients of table and load reicips and recipient columns
    m_hResult = m_lpMessage->GetRecipientTable( 0, &m_lpRecipTable );
    wsprintf(m_szLogBuf,"m_lpMessage->GetRecipientTable( 0, &m_lpRecipTable )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpMessage->GetRecipientTable", m_hResult), 
                            "Client", MBS_ERROR );
        goto Setup;
    }


    // redraw routine
    AdviseObj(  m_lpRecipTable,
                MAPI_TABLE, 
                RedrawRecipientTable,
                fnevTableModified, 
                0,
                NULL,
                "Msg Recipient Table Redraw",
                this, 
                &m_lpNotifRecipTbl);       

    // notif window 
    wsprintf(m_szRecipContext,"fnevTableModified, RecipientTable of Message %s, lpTable",
                            m_CurrentMessage.GetBuffer(15));

#ifdef XVPORT

    AdviseObj(  m_lpRecipTable,
                MAPI_TABLE, 
                LogNotifToXVPLog,
                fnevTableModified, 
                0,
                NULL,
                m_szRecipContext, 
                NULL,
                &m_lpNotifRecipTbl);       

#endif

    m_hResult = m_lpRecipTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll);
    wsprintf(m_szLogBuf,"lpRecipTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpRecipTable->QueryColumns", m_hResult), 
                            "Client", MBS_ERROR );
        goto Setup;
    }
    
    // set columns all columns
    m_hResult = m_lpRecipTable->SetColumns( (LPSPropTagArray) lpsptaAll, 0);
    wsprintf(m_szLogBuf,"lpRecipTable->SetColumns( &sptaAll, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpRecipTable->SetColumns", m_hResult), 
                            "Client", MBS_ERROR );
        goto Setup;
    }
    
    if(lpsptaAll)
        MAPIFreeBuffer(lpsptaAll);

    DisplayRecips();

Setup:

    ResetPropButtons();
    RedrawPropTable();

    // no listboxs currently selected
    dwReturn = SendDlgItemMessage(IDC_MSGRECIPIENTS,LB_SETCURSEL,(WPARAM)-1,0);
    dwReturn = SendDlgItemMessage(IDC_PROPS,        LB_SETCURSEL,(WPARAM)-1,0);
    dwReturn = SendDlgItemMessage(IDC_MSGATTACH,    LB_SETCURSEL,(WPARAM)-1,0);
 
    //***** Add Strings to call function combo box *****
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "Message Properties" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->GetAttachmentTable()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->GetRecipientTable()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->OpenAttach()                       (ON SELECTED ATTACH)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
                     
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->ModifyRecipients()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
    
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->DeleteAttach()                     (ON SELECTED ATTACH)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
    
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->CreateAttach()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->SubmitMessage()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->SetReadFlag()" );

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
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpAdrBook->RecipientOptions               (ON SELECTED RECIPIENT)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            
       
    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpSession->MessageOptions()               (ON SELECTED MSG)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpSession->ShowForm()" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            



    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpMsg->Adivse() / UnAdvise                (NOTIFICATIONS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpAttachTbl->Adivse() / UnAdvise          (NOTIFICATIONS)" );

    if( (dwIndex == CB_ERR ) || (dwIndex == CB_ERRSPACE) )
    {
        MessageBox( "CB_ADDSTRING functions returned Error", "Client", MBS_ERROR );
        return FALSE;
    }            


    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "lpRecipTbl->Adivse() / UnAdvise           (NOTIFICATIONS)" );

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
 -  CMessageDlg::
 -  OnCallFunction
 -
 *  Purpose:
 *
 /********************************************************************/

void CMessageDlg::OnCallFunction()
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
        // MESSAGE FUNCTIONS
        case 0:        
            OnPropInterface();       
            break;
        case 1:
            OnAttachTable();         
            break;
        case 2:
            OnRecipTable();          
            break;
        case 3:
            OnOpenAttachLB();        
            break;
        case 4:
            OnModifyRecipients();    
            break;
        case 5:
            OnDeleteAttach();        
            break;
        case 6:
            OnCreateAttach();        
            break;
        case 7:
            OnSubmitMessage();       
            break;
        case 8:
            OnSetReadFlag();         
            break;
        case 9:
            OnMDBProp();             
            break;
        case 10:
            OnRecipientOptions();    
            break;
        case 11:    
            OnMessageOptions();      
            break;
        case 12:
            OnShowForm();
            break;  
        case 13:
            OnNotifMsg();
            break;
        case 14:
            OnNotifAttTbl();
            break;
        case 15:
            OnNotifRecipTbl();       
            break;
        case 16:
            OnSetCopyToDest();      
            break;

        default:
            MessageBox( "CMessageDlg::OnCallFunction() default ",
                "Client", MBS_OOPS );
            break;       
    }    

}

/*******************************************************************/
/*
 -  CMessageDlg::
 -  RedrawPriority
 *
 *  Purpose:
 */
/*******************************************************************/

void CMessageDlg::RedrawPriority()
{
    LPSPropValue    lpspvaPriority  = NULL;
    ULONG           ulValues        = 0;
    DWORD           dwReturn        = 0;

    // PropTag Array for getting displayname and entryid
    SizedSPropTagArray(1,sptaPriority) = 
    {
        1, 
        { 
            PR_PRIORITY,
        }
    };

    m_hResult = m_lpMessage->GetProps( (LPSPropTagArray) &sptaPriority,0, &ulValues, &lpspvaPriority);
    wsprintf(m_szLogBuf,"lpMessage->GetProps( &sptaPriority,0, &ulValues, &lpspvaPriority)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("lpMessage->GetProps(&sptaPriority)",m_hResult),
                     "Client", MBS_ERROR );
    }
    
    if(lpspvaPriority[0].ulPropTag == PR_PRIORITY)
    {
        dwReturn = SendDlgItemMessage(IDC_MSGPRIORITY,LB_ADDSTRING,0,
                    (LPARAM) GetString("Priority",lpspvaPriority[0].Value.l,NULL) );
    }
    else
        dwReturn = SendDlgItemMessage(IDC_MSGPRIORITY,LB_ADDSTRING,0,
                    (LPARAM) (char *) "No Priority Set" );

}
/*******************************************************************/
/*
 -  CMessageDlg::
 -  OnMessageOptions
 *
 *  Purpose:
 */
/*******************************************************************/

void CMessageDlg::OnMessageOptions()
{
    COperation      MsgOptsDlg(this);
    ULONG           ulFlags         = 0;
    ULONG           cValues         = 0;
    LPSPropValue    lpspvaDefault   = NULL;
    LPWSTR          lpNewBuffer1    = NULL;
    DWORD           dRet            = 0;
    char            szBuff[80];
    ULONG           ulUIParam       = 0;

    MsgOptsDlg.m_CurrentOperation= "lpSession->MessageOptions()";
    MsgOptsDlg.m_CBText1         = "ulUIParam";
    MsgOptsDlg.m_EditText1       = "lpszAdrType:";
    MsgOptsDlg.m_FlagText1       = "MAPI_UNICODE";
    MsgOptsDlg.m_FlagText2       = "Invalid Flag";

    MsgOptsDlg.m_EditDefault1    = "MSPEER";
      
    dRet = MsgOptsDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = MsgOptsDlg.m_CBContents1.Add(szBuff);

    if( MsgOptsDlg.DoModal() == IDOK )      
    {             
        if( !lstrcmp(MsgOptsDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;                    
      
        // determine state/settings of data in dialog upon closing
        if( MsgOptsDlg.m_bFlag2 )                
            ulFlags |= TEST_INVALID_FLAG;
                
        if( MsgOptsDlg.m_bFlag1 )                
        {
            ulFlags |= MAPI_UNICODE;

            String8ToUnicode(MsgOptsDlg.m_szEdit1, &lpNewBuffer1, NULL);

            hResultSession = lpSession->MessageOptions(
                                ulUIParam,
                                ulFlags,
                                (LPTSTR) lpNewBuffer1,
                                m_lpMessage);

            wsprintf(m_szLogBuf,"lpSession->MessageOptions(%lu,%lu,%s,&lpMsg)\t SC: %s",
                        ulUIParam,ulFlags,MsgOptsDlg.m_szEdit1,
                        GetString( "MAPIErrors", GetScode(hResultSession), m_szError ) );
                            
            if( HR_FAILED(hResultSession) )
            {
                MessageBox( m_E.SzError("lpSession->MessageOptions()",
                             hResultSession),"Client", MBS_ERROR );
                PvFree(lpNewBuffer1);
                goto Error;
            }        
        
            PvFree(lpNewBuffer1);
        }
        else
        {

            hResultSession = lpSession->MessageOptions(
                                ulUIParam,
                                ulFlags,
                                (LPTSTR) MsgOptsDlg.m_szEdit1,
                                m_lpMessage);

            wsprintf(m_szLogBuf,"lpSession->MessageOptions(%lu,%lu,%s,&lpMsg)\t SC: %s",
                        ulUIParam,ulFlags,MsgOptsDlg.m_szEdit1,
                        GetString( "MAPIErrors", GetScode(hResultSession), m_szError ) );

            if( HR_FAILED(hResultSession) )                            
            {
                MessageBox( m_E.SzError("lpSession->MessageOptions()",
                         hResultSession),"Client", MBS_ERROR );
                goto Error;
            }        
        }
    }
Error:

    return;
}



/*******************************************************************/
/*
 -  CMesageDlg::
 -  OnModifyRecipients
 *
 *  Purpose:
 *      Operation lpMessage->ModifyRecipients
 */
/*******************************************************************/

void CMessageDlg::OnModifyRecipients()
{   
    COperation      MsgModifyAttachDlg(this);
    CString         OperName;
    int             dRet            = 0;
    ULONG           ulFlags         = 0;
    LPSTR           rglpszDestTitles[3];
    ULONG           rgulDestComps[3];
    ADRPARM         adrparm         = { 0 };
    LPADRLIST       lpAdrList       = NULL;
    ULONG           ulUIParam       = (ULONG)(void *)m_hWnd;
    HRESULT         hResult1        = hrSuccess;
    ULONG           ulRowCount      = 0;
    LPSRowSet       lpRows          = NULL;
    LONG            lRowsSeeked     = 0;                                
    CAdrListDlg     AdrList1(this);
    CAdrListDlg     AdrList2(this);

    // get all rows/recipients of the recipient table
    m_hResult = m_lpRecipTable->GetRowCount(0,&ulRowCount);
    wsprintf(m_szLogBuf,"lpRecipTable->GetRowCount(0,&ulRowCount)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
    
    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpRecipTable->GetRowCount()", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }
    
    // if there are rows, set lpAdrList to be entire rowset
    if( ulRowCount)
    {   
        m_hResult = m_lpRecipTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

        wsprintf(m_szLogBuf,"lpRecipTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                            GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))    
        {
            MessageBox( m_E.SzError("m_lpRecipTable->SeekRow", m_hResult), 
                                "Client", MBS_ERROR );
            goto Error;
        }

        m_hResult = m_lpRecipTable->QueryRows( ulRowCount, 0, &lpRows );

        wsprintf(m_szLogBuf,"lpRecipTable->QueryRows( %lu, 0, &lpRows )\t SC: %s",
                            ulRowCount,
                            GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        // we can caste a LPRowSet to a LPADRLIST
        lpAdrList = (LPADRLIST) lpRows;
    }

    // initalize data for dialog box
    OperName  = m_CurrentMessage;
    OperName += "->ModifyRecipients()";

    MsgModifyAttachDlg.m_CurrentOperation= OperName;
    MsgModifyAttachDlg.m_FlagText1       = "MODRECIP_ADD";
    MsgModifyAttachDlg.m_FlagText2       = "MODRECIP_MODIFY";
    MsgModifyAttachDlg.m_FlagText3       = "MODRECIP_REMOVE";
    MsgModifyAttachDlg.m_FlagText4       = "Invalid Flag";

                                 
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

             
    // display address list before
    AdrList1.m_List       = "Address List of Recipients in Message BEFORE";
    AdrList1.m_lpAdrList  = lpAdrList;
    AdrList1.DoModal();
 
    hResult1 = lpAdrBook->Address( &ulUIParam, &adrparm,&lpAdrList );

    wsprintf(m_szLogBuf,"lpAdrBook->Address(%lu,0x%X,&lpAdrList)\t SC: %s",
               ulUIParam,&adrparm,
               GetString( "MAPIErrors", GetScode(hResult1), m_szError ) );
    
    if( HR_FAILED(hResult1) )
    {
        MessageBox( m_E.SzError("lpAdrBook->Address()", hResult1), "Client", MB_OK );
        goto Error;
    }


    // display address list after
    AdrList2.m_List       = "Address List of Recipients in Message AFTER Address()";
    AdrList2.m_lpAdrList  = lpAdrList;
    AdrList2.DoModal();

    // bring up modal dialog box, and if user hits OK, process operation
    if( MsgModifyAttachDlg.DoModal() == IDOK )      
    {       

        // determine state/settings of data in dialog upon closing
                
        if( MsgModifyAttachDlg.m_bFlag1 )                
            ulFlags |= MODRECIP_ADD;

        if( MsgModifyAttachDlg.m_bFlag2 )                
            ulFlags |= MODRECIP_MODIFY;

        if( MsgModifyAttachDlg.m_bFlag3 )                
            ulFlags |= MODRECIP_REMOVE;

        if( MsgModifyAttachDlg.m_bFlag4 )                
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpMessage->ModifyRecipients(
                        ulFlags,
                        lpAdrList );
 
         wsprintf(m_szLogBuf,"lpMessage->ModifyRecipients(%lu,0x%X)\t SC: %s",
               ulUIParam,lpAdrList,
               GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
        
        if( HR_FAILED(m_hResult) )
        {
            MessageBox( m_E.SzError("m_lpMessage->ModifyRecipients()", m_hResult), 
                            "Client", MBS_ERROR );
            goto Error;
        }        
    }

    DisplayMsgFlags();

Error:

    if(lpAdrList)
        FreeAdrList(lpAdrList);

}   
/********************************************************************/
/*
 -  CMessageDlg::
 -  OnRecipTable
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnRecipTable()
{
    LPSPropTagArray lpsptaAll       = NULL;
    LONG            lRowsSeeked     = 0;

    m_hResult = m_lpRecipTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpRecipTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpRecipTable->SeekRow", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    TBLVUViewMapiTable( (LPMAPITABLE FAR *)&m_lpRecipTable, (HWND)this->m_hWnd );

Error:

    // free Memory allocated
    if(lpsptaAll)
        MAPIFreeBuffer(lpsptaAll);    
}



/********************************************************************/
/*
 -  CMessageDlg::
 -  OnAttachTable
 -
 *  Purpose:
 *  
 /********************************************************************/

void CMessageDlg::OnAttachTable()
{

    LPSPropTagArray lpsptaAll       = NULL;
    LONG            lRowsSeeked     = 0;

    // PropTag Array for getting only attachment number USED IN SETCOLUMNS
    SizedSPropTagArray(1,sptaAttachNum) = 
    {
        1, 
        { 
            PR_ATTACH_NUM,
        }
    };
    
    // query all columns
    m_hResult = m_lpAttachTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll);
    wsprintf(m_szLogBuf,"lpAttachTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->QueryColumns", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }
    
    // set columns all columns
    m_hResult = m_lpAttachTable->SetColumns( (LPSPropTagArray) lpsptaAll, 0);
    wsprintf(m_szLogBuf,"lpAttachTable->SetColumns( &sptaAll, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->SetColumns", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpAttachTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpAttachTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->SeekRow", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    TBLVUViewMapiTable( (LPMAPITABLE FAR *)&m_lpAttachTable, (HWND)this->m_hWnd );

    // set which columns are important to see in table     
    m_hResult = m_lpAttachTable->SetColumns( (LPSPropTagArray) &sptaAttachNum, 0);
    wsprintf(m_szLogBuf,"lpAttachTable->SetColumns( &sptaAttachNum, 0)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->SetColumns", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

Error:

    // free Memory allocated
    if(lpsptaAll)
        MAPIFreeBuffer(lpsptaAll);    

}

/*******************************************************************/
/*
 -  CMesageDlg::
 -  OnSetReadFlag
 *
 *  Purpose:
 *
 */
/*******************************************************************/

void CMessageDlg::OnSetReadFlag()
{   
    COperation      SetReadFlagDlg(this);
    CString         OperName;
    ULONG           ulFlags         = 0;

    // initalize data for dialog box
    OperName  = m_CurrentMessage;
    OperName += "->SetReadFlag()";

    SetReadFlagDlg.m_CurrentOperation= OperName;
    SetReadFlagDlg.m_FlagText1       = "SUPPRESS_RECEIPT";
    SetReadFlagDlg.m_FlagText2       = "CLEAR_READ_FLAG";
    SetReadFlagDlg.m_FlagText3       = "MESSAGE_DIALOG";
    SetReadFlagDlg.m_FlagText4       = "MAPI_DEFERRED_ERRORS";
    SetReadFlagDlg.m_FlagText5       = "GENERATE_RECEIPT_ONLY";
    SetReadFlagDlg.m_FlagText6       = "Invalid Flag";
                                                 
    // bring up modal dialog box, and if user hits OK, process operation
    if( SetReadFlagDlg.DoModal() == IDOK )      
    {       
        // determine state/settings of data in dialog upon closing
                
        if( SetReadFlagDlg.m_bFlag1 )                
            ulFlags |= SUPPRESS_RECEIPT;

        if( SetReadFlagDlg.m_bFlag2 )                
            ulFlags |= CLEAR_READ_FLAG;

        if( SetReadFlagDlg.m_bFlag3 )                
            ulFlags |= MESSAGE_DIALOG;

        if( SetReadFlagDlg.m_bFlag4 )                
            ulFlags |= MAPI_DEFERRED_ERRORS;

        if( SetReadFlagDlg.m_bFlag5 )                
            ulFlags |= GENERATE_RECEIPT_ONLY;

        if( SetReadFlagDlg.m_bFlag6 )                
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpMessage->SetReadFlag(ulFlags);

        wsprintf(m_szLogBuf,"lpMessage->SetReadFlag(%lu)\t SC: %s",
                        ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))    
        {
            MessageBox( m_E.SzError("m_lpMessage->SetReadFlag()", m_hResult), 
                            "Client", MBS_ERROR );
            return;
        }        
        RedrawPropTable();
    }        
    DisplayMsgFlags();

}   

/*******************************************************************/
/*
 -  CMesageDlg::
 -  OnDeleteAttach
 *
 *  Purpose:
 *      Operation lpMessage->DeleteAttachment
 */
/*******************************************************************/

void CMessageDlg::OnDeleteAttach()
{   
    COperation      DeleteAttachDlg(this);
    CString         OperName;
    int             dRet            = 0;
    LONG            lCurSelect      = -1;
    ULONG           ulFlags         = 0;
    ULONG           ulUIParam       = 0;
    LPSRowSet       lpRows          = NULL;
    LONG            lRowsSeeked     = 0;
    LONG            lAttachNum      = -1;
    ULONG           iRow            = 0;
    char            szBuff[80];
    ULONG           ulRefCount      = 0;
    
  
    //$ NYI, FIX UP LPMAPIPROGRESS 
    LPMAPIPROGRESS  lpProgress       = NULL;
    
    // PropTag Array for getting only attachment number
    SizedSPropTagArray(1,sptaAttachNum) = 
    {
        1, 
        { 
            PR_ATTACH_NUM,
        }
    };
    
    // get selected item from child folder listbox
    if( (lCurSelect = SendDlgItemMessage(IDC_MSGATTACH,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Attachment in Listbox to DeleteAttach", "OnDeleteAttach", MBS_ERROR );
        return;
    }

    m_hResult = m_lpAttachTable->SeekRow( BOOKMARK_BEGINNING,lCurSelect,&lRowsSeeked);

    wsprintf(m_szLogBuf,"lpAttachTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        lCurSelect,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->SeekRow", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpAttachTable->QueryRows( 1, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpAttachTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    

    {
        MessageBox( m_E.SzError("m_lpAttachTable->QueryRows", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    // now open attachment based upon PR_ATTACHMENT_NUM from m_lpAttachmentTable
    if ( lpRows->aRow[iRow].lpProps[0].ulPropTag == PR_ATTACH_NUM )
        lAttachNum = lpRows->aRow[iRow].lpProps[0].Value.l ;

    if(lAttachNum == -1)
    {
        MessageBox( m_E.SzError("Could not find PR_ATTACH_NUM in m_lpAttachTable ",m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }   
                                   
    // initalize data for dialog box
    OperName  = m_CurrentMessage;
    OperName += "->DeleteAttach()";

    DeleteAttachDlg.m_CurrentOperation= OperName;
    DeleteAttachDlg.m_CBText1         = "ulUIParam";
    DeleteAttachDlg.m_EditText1       = "lAttachmentNum";
    DeleteAttachDlg.m_FlagText1       = "ATTACH_DIALOG";
    DeleteAttachDlg.m_FlagText2       = "Invalid Flag";
    
    wsprintf(szBuff,"%d",lAttachNum);
    DeleteAttachDlg.m_EditDefault1    = szBuff;
    DeleteAttachDlg.m_EditDefault2    = "";
    DeleteAttachDlg.m_EditDefault3    = "";
        
    dRet = DeleteAttachDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = DeleteAttachDlg.m_CBContents1.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( DeleteAttachDlg.DoModal() == IDOK )      
    {       
        // determine state/settings of data in dialog upon closing

        if( !lstrcmp(DeleteAttachDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;                    

        if( DeleteAttachDlg.m_bFlag1 )                
            ulFlags |= ATTACH_DIALOG;

        if( DeleteAttachDlg.m_bFlag2 )                
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpMessage->DeleteAttach(lAttachNum,ulUIParam,
                        lpProgress,ulFlags);
                        
        wsprintf(m_szLogBuf,"lpMessage->DeleteAttach(%lu,%lu,0x%X,%lu)\t SC: %s",
                        lAttachNum,ulUIParam,lpProgress,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))                            
        {
            MessageBox( m_E.SzError("m_lpMessage->DeleteAttach()", m_hResult), 
                            "Client", MBS_ERROR );
            return;
        }        
    }        

    DisplayMsgFlags();

Error:

    FreeRowSet(lpRows);
}   


/*******************************************************************/
/*
 -  CMessageDlg::
 -  OnCreateAttach
 *
 *  Purpose:
 *      Operation lpMessage->CreateAttachment
 */
/*******************************************************************/

void CMessageDlg::OnCreateAttach()
{   
    COperation      CreateAttDlg(this);
    CString         OperName;
    int             dRet            = 0;
    LONG            lCurSelect      = 0;
    LPIID           lpInterface     = NULL;
    ULONG           ulFlags         = 0;
    LPATTACH        lpAttach        = NULL;
    ULONG           ulAttachNum     = 0;
    ULONG           idx             = 0;
    CAttachDlg      *lpAttachDlg    = NULL;

    
    // initalize data for dialog box

    OperName  = m_CurrentMessage;
    OperName += "->CreateAttach()";

    CreateAttDlg.m_CurrentOperation= OperName;

    CreateAttDlg.m_CBText1         = "lpInterface";
    CreateAttDlg.m_FlagText1       = "MAPI_DEFERRED_ERRORS";
    CreateAttDlg.m_FlagText2       = "Invalid Flag";
    
    dRet = CreateAttDlg.m_CBContents1.Add("NULL");
    dRet = CreateAttDlg.m_CBContents1.Add("Bad Interface");
            
    // bring up modal dialog box, and if user hits OK, process operation
    if( CreateAttDlg.DoModal() == IDOK )      
    {       

        // determine state/settings of data in dialog upon closing
        if( !lstrcmp(CreateAttDlg.m_szCB1,"NULL") )
            lpInterface = NULL;
        else
            lpInterface = (LPIID) &ulFlags;         // invalid
               
        if( CreateAttDlg.m_bFlag1 )                
            ulFlags |= MAPI_DEFERRED_ERRORS;
            
        if( CreateAttDlg.m_bFlag2)
            ulFlags |= TEST_INVALID_FLAG;

        m_hResult = m_lpMessage->CreateAttach(
                        lpInterface,
                        ulFlags,
                        &ulAttachNum,
                        &lpAttach);

        wsprintf(m_szLogBuf,"lpMessage->CreateAttach(0x%X,%lu,&ulAttachNum,&lpAttach)\t SC: %s",
                        lpInterface,ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))                                                    
        {
            MessageBox( m_E.SzError("m_lpMessage->CreateAttach()", m_hResult), 
                            "Client", MBS_ERROR );
            return;
        }

        lpAttachDlg   =  new CAttachDlg("MAPI_ATTACH",(LPATTACH)lpAttach,this);
    }

    DisplayMsgFlags();

}   

/*******************************************************************/
/*
 -  CMesageDlg::
 -  OnSubmitMessage
 *
 *  Purpose:
 *      Operation lpMessage->SubmitMessage
 */
/*******************************************************************/

void CMessageDlg::OnSubmitMessage()
{   
    COperation      MsgSubmitMessageDlg(this);
    CString         OperName;
    int             dRet            = 0;
    LONG            lCurSelect      = -1;
    ULONG           ulFlags         = 0;

                               
    // initalize data for dialog box
    OperName  = m_CurrentMessage;
    OperName += "->SubmitMessage()";

    MsgSubmitMessageDlg.m_CurrentOperation= OperName;
    MsgSubmitMessageDlg.m_FlagText1       = "FORCE_SUBMIT";
    MsgSubmitMessageDlg.m_FlagText2       = "Invalid Flag";
                                                 
    // bring up modal dialog box, and if user hits OK, process operation
    if( MsgSubmitMessageDlg.DoModal() == IDOK )      
    {                                        
        if( MsgSubmitMessageDlg.m_bFlag1 )                
            ulFlags |= FORCE_SUBMIT;

        if( MsgSubmitMessageDlg.m_bFlag2 )                
            ulFlags |= TEST_INVALID_FLAG;

        if( m_hResult = m_lpMessage->SubmitMessage(ulFlags) )


        wsprintf(m_szLogBuf,"lpMessage->SubmitMessage(%lu)\t SC: %s",
                        ulFlags,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

        if( HR_FAILED(m_hResult))                                                            
        {
            MessageBox( m_E.SzError("m_lpMessage->SubmitMessage()", m_hResult), 
                            "Client", MBS_ERROR );
            return;
        }          
    }        

}   


/*******************************************************************/
/*
 -  CMessageDlg::
 -  OnOpenAttachLB
 *
 *  Purpose:
 *      Allow user to OpenAttachLB a message with a OpenAttachLB note
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CMessageDlg::OnOpenAttachLB()
{
    char            szBuff[30];
    LONG            lSelection      = -1;
    LPATTACH        lpAttach        = NULL;
    LPSRowSet       lpRows          = NULL;
    ULONG           ulValues        = 0;
    ULONG           ulRefCount      = 0;
    CAttachDlg      *lpAttachDlg    = NULL;
    LONG            lRowsSeeked     = 0;
    LONG            lAttachNum      = -1;
    ULONG           iRow            = 0;
    ULONG           cColumn         = 0;
    LPSPropValue    lpPropValue     = NULL;
    char            szBuffer[80];

    // PropTag Array for getting only Attach Pathname and Position
    SizedSPropTagArray(3,sptaAttachPathname) = 
    {
        3, 
        { 
            PR_ATTACH_PATHNAME,
            PR_RENDERING_POSITION,
            PR_OBJECT_TYPE,
        }
    };

    // PropTag Array for getting only attachment number
    SizedSPropTagArray(1,sptaAttachNum) = 
    {
        1, 
        { 
            PR_ATTACH_NUM,
        }
    };

    

    // get selected item
    if( (lSelection = SendDlgItemMessage(IDC_MSGATTACH,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Please Select a Attachment in Listbox to Open",
                         "OnOpenAttach", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpAttachTable->SeekRow( BOOKMARK_BEGINNING,lSelection,&lRowsSeeked);
                                                                    
    wsprintf(m_szLogBuf,"lpAttachTable->SeekRow( BOOKMARK_BEGINNING,%ld,&lRowsSeeked)\t SC: %s",
                        lSelection,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->SeekRow", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpAttachTable->QueryRows( 1, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpAttachTable->QueryRows( 1, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->QueryRows", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    // now open attachment based upon PR_ATTACHMENT_NUM from m_lpAttachmentTable
    if ( lpRows->aRow[iRow].lpProps[0].ulPropTag == PR_ATTACH_NUM )
        lAttachNum = lpRows->aRow[iRow].lpProps[0].Value.l ;

    if(lAttachNum == -1)
    {
        MessageBox( m_E.SzError("Could not find PR_ATTACH_NUM in m_lpAttachTable ",m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }   
        
    m_hResult = m_lpMessage->OpenAttach( lAttachNum , NULL, ulAccess, &lpAttach);

    wsprintf(m_szLogBuf,"lpMessage->OpenAttach( %ld , NULL, %lu, &lpAttach)\t SC: %s",
                        lAttachNum,ulAccess,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("OpenAttach()", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = lpAttach->GetProps((LPSPropTagArray) &sptaAttachPathname,0, &ulValues, &lpPropValue);
    wsprintf(m_szLogBuf,"lpAttach->GetProps(&sptaAttachPathname,0, &ulValues, &lpPropValue)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    // COMMENT OUT ???
    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("CMessageDlg::OnOpenAttachLB()  lpAttach->GetProps()", m_hResult),
                     "Client", MBS_ERROR );
        goto Error;
    }

    if (lpRows->aRow[0].lpProps[0].ulPropTag == PR_SUBJECT )
    {
        wsprintf(szBuffer,"%s - %s", 
                GetString("MAPIObjTypes",lpPropValue[2].Value.l,NULL),
                lpPropValue[0].Value.lpszA);
    }    
    else
    {
        wsprintf(szBuff,"AttachNum == %d",lAttachNum);
        wsprintf(szBuffer,"%s - %s", 
                GetString("MAPIObjTypes",lpPropValue[2].Value.l,NULL),
                szBuff);
    }            

    lpAttachDlg   =  new CAttachDlg(szBuffer,(LPATTACH)lpAttach,this);

    FreeRowSet( lpRows );

    if(lpPropValue)
        MAPIFreeBuffer(lpPropValue);

    return;

Error:

    if(lpPropValue)
    {
        MAPIFreeBuffer(lpPropValue);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpPropValue from GetProps" );
    }

    FreeRowSet( lpRows );

    if(lpAttach)
    {
        lpAttach->Release();
        lpAttach = NULL;
    }
}

/*******************************************************************/
/*
 -  CMessageDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Closes the message dialog.
 *
 */
/*******************************************************************/

void CMessageDlg::OnCancel()
{
    delete this;
}

/*******************************************************************/
/*
 -  CMessageDlg::
 -  Cleanup
 -
 *  Purpose:
 *      Cleanup called by destructor for class CMessageDlg.  
 *      Releases and Frees memory allocated in class
 *
 */
/*******************************************************************/

void CMessageDlg::Cleanup()
{     
#ifdef XVPORT

    UnadviseAll(m_lpRecipTable, MAPI_TABLE,"RecipientTable lpTable",    &m_lpNotifRecipTbl);
    UnadviseAll(m_lpAttachTable,MAPI_TABLE,"AttachmentTable lpTable",   &m_lpNotifAttTbl);
    
    if(m_lpMessage)    
        UnadviseAll(lpMDB,          MAPI_STORE,"Message Objs lpMDB",        &m_lpNotifMsg);

#endif
    // table obtained from GetAttachmentTable                               
    if(m_lpAttachTable)
    {    
        m_lpAttachTable->Release();
        m_lpAttachTable = NULL;
    }

    // table obtained from GetRecipientTable
    if(m_lpRecipTable)
    {
        m_lpRecipTable->Release();
        m_lpRecipTable = NULL;
    }
    
    // message object
    if(m_lpMessage)
    {
        m_lpMessage->Release();
        m_lpMessage     = NULL; 
    }
}

/********************************************************************/
/*
 -  CMessageDlg::
 -  DisplayAttachments  (INTERNAL, protected member function)
 -
 *  Purpose:
 *      Display Attachments contained in message
 *
 *  Returns:
 *      Nothing
 */     
/********************************************************************/

void CMessageDlg::DisplayAttachments()
{
    ULONG           cColumn         = 0;
    int             iRow            = 0;
    LPATTACH        lpAttach        = NULL;
    LPSRowSet       lpRows          = NULL;
    ULONG           ulValues        = 0;
    LPSPropValue    lpPropValue     = NULL;
    DWORD           dwReturn        = 0;
    LONG            lAttachNum      = -1;
    LPSPropValue    lpPV            = NULL;
    LPSPropTagArray lpPropTags      = NULL;
    char            szBuff[80];   
    LONG            lRowsSeeked     = 0;
    ULONG           ulRowCount      = 0;

    // PropTag Array for getting only Attach Pathname and Position USED IN GETPROPS
    SizedSPropTagArray(2,sptaAttachPathname) = 
    {
        2, 
        { 
            PR_ATTACH_PATHNAME,
            PR_RENDERING_POSITION,
        }
    };
    

    dwReturn = SendDlgItemMessage(IDC_MSGATTACH,LB_RESETCONTENT,0,0);

    m_hResult = m_lpAttachTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);
                                                                    
    wsprintf(m_szLogBuf,"lpAttachTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->SeekRow", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }



    m_hResult = m_lpAttachTable->GetRowCount(0,&ulRowCount);
    wsprintf(m_szLogBuf,"lpAttachTable->GetRowCount(0,&ulRowCount)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
    
    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpAttachTable->GetRowCount()", m_hResult), "Client", MBS_ERROR );
        goto Error;
    }
       
    m_hResult = m_lpAttachTable->QueryRows( ulRowCount, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpAttachTable->QueryRows( %lu, 0, &lpRows )\t SC: %s",ulRowCount,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
    
    if( !HR_FAILED(m_hResult) )
    {

        // for each attachment , open it, getprops and get PATHNAME and display in listbox
        for( iRow = 0 ; iRow < lpRows->cRows ; iRow++ )
        {
            lAttachNum = -1;
            // now open attachment based upon PR_ATTACHMENT_NUM from m_lpAttachmentTable
            
            lpPV = lpRows->aRow[iRow].lpProps;
        
            for(cColumn = 0; cColumn < lpRows->aRow[iRow].cValues; cColumn++, lpPV++)
            {
                if ( lpPV->ulPropTag == PR_ATTACH_NUM )
                {
                    lAttachNum = lpPV->Value.l ;
                    break;
                }
            }
            if(lAttachNum == -1)
            {
                MessageBox( m_E.SzError("Could not find PR_ATTACH_NUM in m_lpAttachTable ",m_hResult), 
                                    "Client", MBS_ERROR );
                goto Error;
            }   
            
            m_hResult = m_lpMessage->OpenAttach( lAttachNum , NULL, 0, &lpAttach);
            wsprintf(m_szLogBuf,"lpMessage->OpenAttach(%ld , NULL, 0, &lpAttach)\t SC: %s",
                        lAttachNum,
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
        
            if( HR_FAILED(m_hResult))    
            {
                MessageBox( m_E.SzError("OpenAttach()", m_hResult), "Client", MBS_ERROR );
                goto Error;
            }   

            m_hResult = lpAttach->GetProps( (LPSPropTagArray) &sptaAttachPathname,0, &ulValues, &lpPropValue);          

            wsprintf(m_szLogBuf,"lpAttach->GetProps(&sptaAttachPathname,0, &ulValues, &lpPropValue)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
            
            // COMMENT OUT ??
            if( HR_FAILED(m_hResult))    
            {
                MessageBox( m_E.SzError("CMessageDlg::OnOpenAttachLB()  lpAttach->GetProps()", m_hResult),
                     "Client", MBS_ERROR );
                goto Error;
            }

            if( (PROP_TYPE(lpPropValue[0].ulPropTag) != PT_ERROR) &&
                (PROP_TYPE(lpPropValue[0].ulPropTag) != PT_NULL) )
            {
                dwReturn = SendDlgItemMessage(IDC_MSGATTACH,LB_ADDSTRING,0,
                        (LPARAM)lpPropValue[0].Value.lpszA );
            }
            else
            {
                wsprintf(szBuff,"AttachNum == %d",lAttachNum);
                dwReturn = SendDlgItemMessage(IDC_MSGATTACH,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

            }
                       
            if(lpAttach)
            {
                lpAttach->Release();
                lpAttach = NULL;    
            }
        } // for loop on rows of attachment table
    }  // if queryrows successful
Error:

    if(lpPropValue)
    {
        MAPIFreeBuffer(lpPropValue);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpPropValue from GetProps" );
    }

    FreeRowSet( lpRows );

    if(lpAttach)
    {
        lpAttach->Release();
        lpAttach = NULL;    
    }
}




/********************************************************************/
/*
 -  CMessageDlg::
 -  DisplayRecips   (INTERNAL, protected member function)
 -
 *  Purpose:
 *      Display Recipient Columns and recipients
 *
 *  Returns:
 *      Nothing
 */
/********************************************************************/

void CMessageDlg::DisplayRecips()
{
    int             iRow            = 0;
    LPSRowSet       lpRows          = NULL;
    DWORD           dwReturn        = 0;
    LONG            lRowsSeeked     = 0;
    LPSPropValue    lpPropValue     = NULL;
    ULONG           ulColumnDisplayName = 0;
    ULONG           idx             = 0;
    LPSPropTagArray lpsptaAll       = NULL;

    dwReturn = SendDlgItemMessage(IDC_MSGRECIPIENTS,LB_RESETCONTENT,0,0);

    m_hResult = m_lpRecipTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked);
                                                                    
    wsprintf(m_szLogBuf,"lpRecipTable->SeekRow( BOOKMARK_BEGINNING,0,&lRowsSeeked)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

    if( HR_FAILED(m_hResult))    
    {
        MessageBox( m_E.SzError("m_lpRecipTable->SeekRow", m_hResult), 
                            "Client", MBS_ERROR );
        goto Error;
    }

    m_hResult = m_lpRecipTable->QueryRows( 100, 0, &lpRows );

    wsprintf(m_szLogBuf,"lpRecipTable->QueryRows( 100, 0, &lpRows )\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
    
    if( !HR_FAILED(m_hResult) )
    {           
        // make sure we have some rows before trying to reference columns of rows
        if(lpRows->cRows == 0)
            goto Error;

        // query columns to determine how many columns there are
        m_hResult = m_lpRecipTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll);
        wsprintf(m_szLogBuf,"lpRecipTable->QueryColumns(TBL_ALL_COLUMNS, &lpsptaAll)\t SC: %s",
                            GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
        
        if( HR_FAILED(m_hResult))    
        {
            MessageBox( m_E.SzError("m_lpRecipTable->QueryColumns", m_hResult), 
                                "Client", MBS_ERROR );
            goto Error;
        }
    
        // search all columns for PR_DISPLAY_NAME to determine name to display in listbox
        for(idx = 0; idx < lpsptaAll->cValues ; ++idx)
        {
            if(lpRows->aRow[0].lpProps[idx].ulPropTag == PR_DISPLAY_NAME)
                break;
        }            
        
        // if we could not find PR_DISPLAY_NAME, dump out
        if(idx == lpsptaAll->cValues )
            goto Error;
        
        for(iRow = 0; iRow < lpRows->cRows; ++iRow)
        {
            lpPropValue = lpRows->aRow[iRow].lpProps;
        
            if (lpPropValue[idx].ulPropTag == PR_DISPLAY_NAME )             
            {
                dwReturn = SendDlgItemMessage(IDC_MSGRECIPIENTS,LB_ADDSTRING,0,
                            (LPARAM)lpPropValue[idx].Value.lpszA );                 
            }
        
        }
    }        

Error:

    FreeRowSet(lpRows);

    if(lpsptaAll)
        MAPIFreeBuffer(lpsptaAll);
}     

                   
/*******************************************************************/
/*
 -  CMessageDlg::
 -
 -  OnAttachLB
 -  OnAttachLB
 -  OnProperty
 -  OnFocus
 *
 *  Purpose:
 *      Disables/enables appropriate buttons and sets current selected
 *      listbox states
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/
/*******************************************************************/

void CMessageDlg::OnAttachLB()
{
    DWORD   dwReturn    = 0;

    dwReturn = SendDlgItemMessage(IDC_MSGRECIPIENTS,LB_SETCURSEL,(WPARAM)-1,0);
    dwReturn = SendDlgItemMessage(IDC_PROPS,        LB_SETCURSEL,(WPARAM)-1,0);
    
}

/*******************************************************************/


void CMessageDlg::OnRecipLB()
{
    DWORD   dwReturn    = 0;

    dwReturn = SendDlgItemMessage(IDC_PROPS,        LB_SETCURSEL,(WPARAM)-1,0);
    dwReturn = SendDlgItemMessage(IDC_MSGATTACH,    LB_SETCURSEL,(WPARAM)-1,0);
}

/*******************************************************************/

void CMessageDlg::OnProperty()
{
    DWORD   dwReturn = 0;

    dwReturn = SendDlgItemMessage(IDC_MSGRECIPIENTS,LB_SETCURSEL,(WPARAM)-1,0);
    dwReturn = SendDlgItemMessage(IDC_MSGATTACH,    LB_SETCURSEL,(WPARAM)-1,0);
}


/*******************************************************************/
/*********************** MsgSpecial **************************/

/********************************************************************/
/*
 -  CMsgSpecialDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CMsgSpecialDlg::OnInitDialog()
{
    CGetError       E;
    DWORD           dwReturn            = 0;
    HRESULT         hResult             = hrSuccess;
    char            szBuffer[300];
    ULONG           cVals               = 0;
    LPSPropValue    lpspva              = NULL;
    ULONG           i;

  
    SizedSPropTagArray(7,sptaMsgSpecial) =
    {
        7,
        {
            PR_SUBJECT,
            PR_MESSAGE_CLASS,
            PR_OBJECT_TYPE,
            PR_ACCESS,
            PR_ACCESS_LEVEL,
            PR_HASATTACH,
            PR_MESSAGE_FLAGS,
        }
    };



    hResult = m_lpMessage->GetProps((LPSPropTagArray)&sptaMsgSpecial,0,&cVals,&lpspva);
    if( HR_FAILED(hResult) )
    {
        MessageBox( E.SzError("lpMDB->GetProps(sptaMsgSpecial)", hResult),
                 "Client", MBS_ERROR );
        return FALSE;
    }

    i = 0;

    // PR_SUBJECT
    if(lpspva[i].ulPropTag != PR_SUBJECT )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_SUBJECT1,szBuffer);
    i++;

    // PR_MESSAGE_CLASS
    if(lpspva[i].ulPropTag != PR_MESSAGE_CLASS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_MESSAGE_CLASS not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_MESSAGE_CLASS1,szBuffer);
    i++;

    // PR_OBJECT_TYPE
    if(lpspva[i].ulPropTag != PR_OBJECT_TYPE )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_OBJECT_TYPE not available");
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
            wsprintf(szBuffer,"PR_ACCESS not available");
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
            wsprintf(szBuffer,"PR_ACCESS_LEVEL not available");
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

    // PR_HASATTACH
    if(lpspva[i].ulPropTag != PR_HASATTACH )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_HASATTACH not available");
    }
    else
    {
        if(lpspva[i].Value.b == TRUE )
            wsprintf(szBuffer,"YES");
        else
            wsprintf(szBuffer,"NO");
    }            
    SetDlgItemText(IDT_HASATTACH1,szBuffer);
    i++;


    // PR_MESSAGE_FLAGS
    if(lpspva[i].ulPropTag != PR_MESSAGE_FLAGS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_MESSAGE_FLAGS not available");
        SetDlgItemText(IDT_MESSAGE_FLAGS1,szBuffer);
    }
    else
    {
        if( (lpspva[i].Value.ul & MSGFLAG_READ) )
            SetDlgItemText(IDT_MESSAGE_FLAGS1,"MSGFLAG_READ");

        if( (lpspva[i].Value.ul & MSGFLAG_UNMODIFIED) )
            SetDlgItemText(IDT_MESSAGE_FLAGS2,"MSGFLAG_UNMODIFIED");

        if( (lpspva[i].Value.ul & MSGFLAG_SUBMIT) )
            SetDlgItemText(IDT_MESSAGE_FLAGS3,"MSGFLAG_SUBMIT");

        if( (lpspva[i].Value.ul & MSGFLAG_UNSENT) )
            SetDlgItemText(IDT_MESSAGE_FLAGS4,"MSGFLAG_UNSENT");

        if( (lpspva[i].Value.ul & MSGFLAG_HASATTACH) )
            SetDlgItemText(IDT_MESSAGE_FLAGS5,"MSGFLAG_HASATTACH");

        if( (lpspva[i].Value.ul & MSGFLAG_FROMME) )
            SetDlgItemText(IDT_MESSAGE_FLAGS6,"MSGFLAG_FROMME");

        if( (lpspva[i].Value.ul & MSGFLAG_ASSOCIATED) )
            SetDlgItemText(IDT_MESSAGE_FLAGS7,"MSGFLAG_ASSOCIATED");

        if( (lpspva[i].Value.ul & MSGFLAG_RESEND) )
            SetDlgItemText(IDT_MESSAGE_FLAGS8,"MSGFLAG_RESEND");
    }
    i++;

    MAPIFreeBuffer(lpspva);

    return TRUE;
}


/*******************************************************************/
/*
 -  CMsgSpecialDlg::
 -  ~CMsgSpecialDlg
 -
 *  Purpose:
 *      Destructor for class CMsgSpecialDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CMsgSpecialDlg::~CMsgSpecialDlg()
{

}


/*******************************************************************/
/*
 -  CMsgSpecialDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CMsgSpecialDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}

          
        


/*******************************************************************/
/*********************** IPMSpecial **************************/

/********************************************************************/
/*
 -  CIPMSpecialDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CIPMSpecialDlg::OnInitDialog()
{
    CGetError       E;
    DWORD           dwReturn            = 0;
    HRESULT         hResult             = hrSuccess;
    char            szBuffer[300];
    ULONG           cVals               = 0;
    LPSPropValue    lpspva              = NULL;
    ULONG           i;

  
    SizedSPropTagArray(10,sptaIPMSpecial) =
    {
        10,
        {
            PR_SUBJECT,
            PR_MESSAGE_CLASS,
            PR_AUTO_FORWARDED,
            PR_IMPORTANCE,
            PR_INCOMPLETE_COPY,
            PR_SENSITIVITY,
            PR_SEARCH_KEY,
            PR_EXPIRY_TIME,
            PR_PRIORITY,
            PR_READ_RECEIPT_REQUESTED,
        }
    };



    hResult = m_lpMessage->GetProps((LPSPropTagArray)&sptaIPMSpecial,0,&cVals,&lpspva);
    if( HR_FAILED(hResult) )
    {
        MessageBox( E.SzError("lpMDB->GetProps(sptaIPMSpecial)", hResult),
                 "Client", MBS_ERROR );
        return FALSE;
    }

    i = 0;

    // PR_SUBJECT
    if(lpspva[i].ulPropTag != PR_SUBJECT )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_SUBJECT1,szBuffer);
    i++;

    // PR_MESSAGE_CLASS
    if(lpspva[i].ulPropTag != PR_MESSAGE_CLASS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_MESSAGE_CLASS not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_MESSAGE_CLASS1,szBuffer);
    i++;

    // PR_AUTO_FORWARDED
    if(lpspva[i].ulPropTag != PR_AUTO_FORWARDED )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_AUTO_FORWARDED not available");
    }
    else
    {
        if(lpspva[i].Value.b == TRUE )
            wsprintf(szBuffer,"YES");
        else
            wsprintf(szBuffer,"NO");
    }            
    SetDlgItemText(IDT_AUTO_FORWARDED1,szBuffer);
    i++;


    // PR_IMPORTANCE
    if(lpspva[i].ulPropTag != PR_IMPORTANCE )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_IMPORTANCE not available");
        SetDlgItemText(IDT_IMPORTANCE1,szBuffer);
    }
    else
    {
        if( (lpspva[i].Value.ul == IMPORTANCE_LOW) )
            SetDlgItemText(IDT_IMPORTANCE1,"IMPORTANCE_LOW");
        else if( (lpspva[i].Value.ul == IMPORTANCE_NORMAL) )
            SetDlgItemText(IDT_IMPORTANCE1,"IMPORTANCE_NORMAL");
        else if( (lpspva[i].Value.ul == IMPORTANCE_HIGH) )
            SetDlgItemText(IDT_IMPORTANCE1,"IMPORTANCE_HIGH");
        else
        {
            wsprintf(szBuffer,"PR_IMPORTANCE = %lu, Unknown !",lpspva[i].Value.ul);
            SetDlgItemText(IDT_IMPORTANCE1,szBuffer);
        }
    }
    i++;

    // PR_INCOMPLETE_COPY
    if(lpspva[i].ulPropTag != PR_INCOMPLETE_COPY )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_INCOMPLETE_COPY not available");
    }
    else
    {
        if(lpspva[i].Value.b == TRUE )
            wsprintf(szBuffer,"YES");
        else
            wsprintf(szBuffer,"NO");
    }            
    SetDlgItemText(IDT_INCOMPLETE_COPY1,szBuffer);
    i++;


    // PR_SENSITIVITY
    if(lpspva[i].ulPropTag != PR_SENSITIVITY )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SENSITIVITY not available");
        SetDlgItemText(IDT_SENSITIVITY1,szBuffer);
    }
    else
    {
        if( (lpspva[i].Value.ul == SENSITIVITY_NONE) )
            SetDlgItemText(IDT_SENSITIVITY1,"SENSITIVITY_NONE");
        else if( (lpspva[i].Value.ul == SENSITIVITY_PERSONAL) )
            SetDlgItemText(IDT_SENSITIVITY1,"SENSITIVITY_PERSONAL");
        else if( (lpspva[i].Value.ul == SENSITIVITY_PRIVATE) )
            SetDlgItemText(IDT_SENSITIVITY1,"SENSITIVITY_PRIVATE");
        else if( (lpspva[i].Value.ul == SENSITIVITY_COMPANY_CONFIDENTIAL) )
            SetDlgItemText(IDT_SENSITIVITY1,"SENSITIVITY_COMPANY_CONFIDENTIAL");
        else
        {
            wsprintf(szBuffer,"PR_SENSITIVITY = %lu, Unknown !",lpspva[i].Value.ul);
            SetDlgItemText(IDT_SENSITIVITY1,szBuffer);
        }
    }
    i++;

    // PR_SEARCH_KEY
    if(lpspva[i].ulPropTag != PR_SEARCH_KEY )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SEARCH_KEY not available");
    }
    else
    {
        szBuffer[0] = '\0';
        
        char    lpszHex[9];
        ULONG   cb = 0;
        LPBYTE  lpb = lpspva[i].Value.bin.lpb;
        ULONG   cChars = 0;

        while((cb < lpspva[i].Value.bin.cb) && (cChars < 40 ) )
        {
            wsprintf(lpszHex, "%02X ", *lpb);
            lstrcat(szBuffer, lpszHex);
            cChars += 3;
            lpb++;
            cb++;
        }

        if( ((lpspva[i].Value.bin.cb)*3) > 40 )
        {
            lstrcat(szBuffer, " }etc");
        }
    }
    SetDlgItemText(IDT_SEARCH_KEY1,szBuffer);
    i++;


    // PR_EXPIRY_TIME  
    if(lpspva[i].ulPropTag != PR_EXPIRY_TIME )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_EXPIRY_TIME not available");
        SetDlgItemText(IDT_EXPIRY_TIME1,szBuffer);
    }
    else
    {
        SzGetPropValue(szBuffer, (LPSPropValue) &(lpspva[i]) );
        SetDlgItemText(IDT_EXPIRY_TIME1,szBuffer);
    }
    i++;


    // PR_PRIORITY
    if(lpspva[i].ulPropTag != PR_PRIORITY )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_PRIORITY not available");
        SetDlgItemText(IDT_PRIORITY1,szBuffer);
    }
    else
    {
        if( (lpspva[i].Value.ul == PRIO_URGENT) )
            SetDlgItemText(IDT_PRIORITY1,"PRIO_URGENT");
        else if( (lpspva[i].Value.ul == PRIO_NORMAL) )
            SetDlgItemText(IDT_PRIORITY1,"PRIO_NORMAL");
        else if( (lpspva[i].Value.ul == PRIO_NONURGENT) )
            SetDlgItemText(IDT_PRIORITY1,"PRIO_NONURGENT");
        else
        {
            wsprintf(szBuffer,"PR_PRIORITY = %lu, Unknown !",lpspva[i].Value.ul);
            SetDlgItemText(IDT_PRIORITY1,szBuffer);
        }
    }
    i++;

    // PR_READ_RECEIPT_REQUESTED
    if(lpspva[i].ulPropTag != PR_READ_RECEIPT_REQUESTED )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_READ_RECEIPT_REQUESTED not available");
    }
    else
    {
        if(lpspva[i].Value.b == TRUE )
            wsprintf(szBuffer,"YES");
        else
            wsprintf(szBuffer,"NO");
    }            
    SetDlgItemText(IDT_READ_RECEIPT_REQUESTED1,szBuffer);
    i++;


    MAPIFreeBuffer(lpspva);

    return TRUE;
}


/*******************************************************************/
/*
 -  CIPMSpecialDlg::
 -  ~CIPMSpecialDlg
 -
 *  Purpose:
 *      Destructor for class CIPMSpecialDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CIPMSpecialDlg::~CIPMSpecialDlg()
{

}


/*******************************************************************/
/*
 -  CIPMSpecialDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CIPMSpecialDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}

/*******************************************************************/
/*********************** IPNSpecial **************************/

/********************************************************************/
/*
 -  CIPNSpecialDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CIPNSpecialDlg::OnInitDialog()
{
    CGetError       E;
    DWORD           dwReturn            = 0;
    HRESULT         hResult             = hrSuccess;
    char            szBuffer[300];
    ULONG           cVals               = 0;
    LPSPropValue    lpspva              = NULL;
    ULONG           i;

  
    SizedSPropTagArray(15,sptaIPNSpecial) =
    {
        15,
        {
            PR_SUBJECT,
            PR_MESSAGE_CLASS,
            PR_ORIGINAL_SEARCH_KEY,
            PR_CONVERSION_EITS,
            PR_ORIGINALLY_INTENDED_RECIPIENT_NAME,
            PR_SENDER_NAME,
            PR_SENDER_EMAIL_ADDRESS,
            PR_SENDER_ADDRTYPE,
            PR_REPORT_TEXT,
            PR_REPORT_TIME,
            PR_RECEIPT_TIME,
            PR_ACKNOWLEDGEMENT_MODE,
            PR_AUTO_FORWARD_COMMENT,
            PR_DISCARD_REASON,
            PR_NON_RECEIPT_REASON,
       }
    };

    hResult = m_lpMessage->GetProps((LPSPropTagArray)&sptaIPNSpecial,0,&cVals,&lpspva);
    if( HR_FAILED(hResult) )
    {
        MessageBox( E.SzError("lpMDB->GetProps(sptaIPNSpecial)", hResult),
                 "Client", MBS_ERROR );
        return FALSE;
    }

    i = 0;

    // PR_SUBJECT
    if(lpspva[i].ulPropTag != PR_SUBJECT )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SUBJECT not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_SUBJECT1,szBuffer);
    i++;

    // PR_MESSAGE_CLASS
    if(lpspva[i].ulPropTag != PR_MESSAGE_CLASS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_MESSAGE_CLASS not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_MESSAGE_CLASS1,szBuffer);
    i++;

    // PR_ORIGINAL_SEARCH_KEY
    if(lpspva[i].ulPropTag != PR_ORIGINAL_SEARCH_KEY )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_ORIGINAL_SEARCH_KEY not available");
    }
    else
    {
        szBuffer[0] = '\0';
        
        char    lpszHex[9];
        ULONG   cb = 0;
        LPBYTE  lpb = lpspva[i].Value.bin.lpb;
        ULONG   cChars = 0;

        while((cb < lpspva[i].Value.bin.cb) && (cChars < 40 ) )
        {
            wsprintf(lpszHex, "%02X ", *lpb);
            lstrcat(szBuffer, lpszHex);
            cChars += 3;
            lpb++;
            cb++;
        }

        if( ((lpspva[i].Value.bin.cb)*3) > 40 )
        {
            lstrcat(szBuffer, " }etc");
        }
    }
    SetDlgItemText(IDT_ORIGINAL_SEARCH_KEY1,szBuffer);
    i++;

    // PR_CONVERSION_EITS
    if(lpspva[i].ulPropTag != PR_CONVERSION_EITS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_CONVERSION_EITS not available");
    }
    else
    {
        szBuffer[0] = '\0';
        
        char    lpszHex[9];
        ULONG   cb = 0;
        LPBYTE  lpb = lpspva[i].Value.bin.lpb;
        ULONG   cChars = 0;

        while((cb < lpspva[i].Value.bin.cb) && (cChars < 40 ) )
        {
            wsprintf(lpszHex, "%02X ", *lpb);
            lstrcat(szBuffer, lpszHex);
            cChars += 3;
            lpb++;
            cb++;
        }

        if( ((lpspva[i].Value.bin.cb)*3) > 40 )
        {
            lstrcat(szBuffer, " }etc");
        }
    }
    SetDlgItemText(IDT_CONVERSION_EITS1,szBuffer);
    i++;

    // PR_ORIGINALLY_INTENDED_RECIPIENT_NAME
    if(lpspva[i].ulPropTag != PR_ORIGINALLY_INTENDED_RECIPIENT_NAME )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_ORIGINALLY_INTENDED_RECIPIENT_NAME not available");
    }
    else
    {
        szBuffer[0] = '\0';
        
        char    lpszHex[9];
        ULONG   cb = 0;
        LPBYTE  lpb = lpspva[i].Value.bin.lpb;
        ULONG   cChars = 0;

        while((cb < lpspva[i].Value.bin.cb) && (cChars < 40 ) )
        {
            wsprintf(lpszHex, "%02X ", *lpb);
            lstrcat(szBuffer, lpszHex);
            cChars += 3;
            lpb++;
            cb++;
        }

        if( ((lpspva[i].Value.bin.cb)*3) > 40 )
        {
            lstrcat(szBuffer, " }etc");
        }
    }
    SetDlgItemText(IDT_ORIGINALLY_INTENDED_RECIPIENT_NAME1,szBuffer);
    i++;

    // PR_SENDER_NAME
    if(lpspva[i].ulPropTag != PR_SENDER_NAME )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SENDER_NAME not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_SENDER_NAME1,szBuffer);
    i++;

    // PR_SENDER_EMAIL_ADDRESS
    if(lpspva[i].ulPropTag != PR_SENDER_EMAIL_ADDRESS )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SENDER_EMAIL_ADDRESS not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_SENDER_EMAIL_ADDRESS1,szBuffer);
    i++;

    // PR_SENDER_ADDRTYPE
    if(lpspva[i].ulPropTag != PR_SENDER_ADDRTYPE )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_SENDER_ADDRTYPE not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_SENDER_ADDRTYPE1,szBuffer);
    i++;

    // PR_REPORT_TEXT
    if(lpspva[i].ulPropTag != PR_REPORT_TEXT )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_REPORT_TEXT not available");
    }
    else
        wsprintf(szBuffer,lpspva[i].Value.lpszA);
    SetDlgItemText(IDT_REPORT_TEXT1,szBuffer);
    i++;

    // PR_REPORT_TIME  
    if(lpspva[i].ulPropTag != PR_REPORT_TIME )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_REPORT_TIME not available");
        SetDlgItemText(IDT_REPORT_TIME1,szBuffer);
    }
    else
    {
        SzGetPropValue(szBuffer, (LPSPropValue) &(lpspva[i]) );
        SetDlgItemText(IDT_REPORT_TIME1,szBuffer);
    }
    i++;

    // Depending on whether it is a NRN or an RN, we have different
    // required properties, and therefore display different props

    // NOTE NOTE NOTE, Order of the lpspva is important for these values !!!

    if( !_stricmp(lpspva[1].Value.lpszA,"REPORT.IPM.Note.IPNRN" ) )
    {
        // Receipt Notification Props
        //   PR_ACKNOWLEDGEMENT_MODE
        //   PR_RECEIPT_TIME

        // 1.  Fill in Name

        SetDlgItemText(IDT_RECEIPT_TIME,        "PR_RECEIPT_TIME");
        SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE, "PR_ACKNOWLEDGEMENT_MODE");
        SetDlgItemText(IDT_AUTO_FORWARD_COMMENT," ");

        // 2.  Fill in Data

        // PR_RECEIPT_TIME  
        if(lpspva[10].ulPropTag != PR_RECEIPT_TIME )
        {
            if( PROP_TYPE(lpspva[10].ulPropTag) == PT_ERROR)
                GetString( "MAPIErrors", lpspva[10].Value.err, szBuffer );
            else
                wsprintf(szBuffer,"PR_RECEIPT_TIME not available");
            SetDlgItemText(IDT_RECEIPT_TIME1,szBuffer);
        }
        else
        {
            SzGetPropValue(szBuffer, (LPSPropValue) &(lpspva[10]) );
            SetDlgItemText(IDT_RECEIPT_TIME1,szBuffer);
        }

        // PR_ACKNOWLEDGEMENT_MODE
        if(lpspva[11].ulPropTag != PR_ACKNOWLEDGEMENT_MODE )
        {
            if( PROP_TYPE(lpspva[11].ulPropTag) == PT_ERROR)
                GetString( "MAPIErrors", lpspva[11].Value.err, szBuffer );
            else
                wsprintf(szBuffer,"PR_ACKNOWLEDGEMENT_MODE not available");
            SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,szBuffer);
        }
        else
        {
            if( (lpspva[11].Value.ul == 0) )
                SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,"IM_MANUAL");
            else if( (lpspva[11].Value.ul == 1) )
                SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,"IM_AUTOMATIC");
            else
            {
                wsprintf(szBuffer,"PR_ACKNOWLEDGEMENT_MODE = %lu, Unknown !",lpspva[11].Value.ul);
                SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,szBuffer);
            }
        }
    }        
    else if( !_stricmp(lpspva[1].Value.lpszA,"REPORT.IPM.Note.IPNNRN" ) )
    {
        // Non Receipt Notification Props
        //   PR_DISCARD_REASON
        //   PR_NON_RECEIPT_REASON
        //   PR_AUTO_FORWARD_COMMENT

        // 1.  Fill in Name

        SetDlgItemText(IDT_RECEIPT_TIME,        "PR_DISCARD_REASON");
        SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE, "PR_NON_RECEIPT_REASON");
        SetDlgItemText(IDT_AUTO_FORWARD_COMMENT,"PR_AUTO_FORWARD_COMMENT");

        // 2.  Fill in Data
        
        // PR_DISCARD_REASON
        if(lpspva[13].ulPropTag != PR_DISCARD_REASON )
        {
            if( PROP_TYPE(lpspva[13].ulPropTag) == PT_ERROR)
                GetString( "MAPIErrors", lpspva[13].Value.err, szBuffer );
            else
                wsprintf(szBuffer,"PR_DISCARD_REASON not available");
            SetDlgItemText(IDT_RECEIPT_TIME1,szBuffer);
        }
        else
        {
            if( (lpspva[13].Value.ul == -1) )
                SetDlgItemText(IDT_RECEIPT_TIME1,"IM_NO_DISCARD");
            else if( (lpspva[13].Value.ul == 0) )
                SetDlgItemText(IDT_RECEIPT_TIME1,"IM_IPM_EXPIRED");
            else if( (lpspva[13].Value.ul == 1) )
                SetDlgItemText(IDT_RECEIPT_TIME1,"IM_IPM_OBSOLETED");
            else if( (lpspva[13].Value.ul == 2) )
                SetDlgItemText(IDT_RECEIPT_TIME1,"IM_IPM_TERMINATED");
            else
            {
                wsprintf(szBuffer,"PR_DISCARD_REASON = %lu, Unknown !",lpspva[13].Value.ul);
                SetDlgItemText(IDT_RECEIPT_TIME1,szBuffer);
            }
        }
                
        // PR_NON_RECEIPT_REASON
        if(lpspva[14].ulPropTag != PR_NON_RECEIPT_REASON )
        {
            if( PROP_TYPE(lpspva[14].ulPropTag) == PT_ERROR)
                GetString( "MAPIErrors", lpspva[14].Value.err, szBuffer );
            else
                wsprintf(szBuffer,"PR_NON_RECEIPT_REASON not available");
            SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,szBuffer);
        }
        else
        {
            if( (lpspva[14].Value.ul == 0) )
                SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,"IM_IPM_DISCARDED");
            else if( (lpspva[14].Value.ul == 1) )
                SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,"IM_IPM_AUTO_FORWARDED");
            else
            {
                wsprintf(szBuffer,"PR_NON_RECEIPT_REASON = %lu, Unknown !",lpspva[14].Value.ul);
                SetDlgItemText(IDT_ACKNOWLEGEMENT_MODE1,szBuffer);
            }
        }
        
        // PR_AUTO_FORWARD_COMMENT
        if(lpspva[12].ulPropTag != PR_AUTO_FORWARD_COMMENT )
        {
            if( PROP_TYPE(lpspva[12].ulPropTag) == PT_ERROR)
                GetString( "MAPIErrors", lpspva[12].Value.err, szBuffer );
            else
                wsprintf(szBuffer,"PR_AUTO_FORWARD_COMMENT not available");
        }
        else
            wsprintf(szBuffer,lpspva[12].Value.lpszA);
        SetDlgItemText(IDT_AUTO_FORWARD_COMMENT1,szBuffer);
        

    }        
    


    MAPIFreeBuffer(lpspva);

    return TRUE;
}


/*******************************************************************/
/*
 -  CIPNSpecialDlg::
 -  ~CIPNSpecialDlg
 -
 *  Purpose:
 *      Destructor for class CIPNSpecialDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CIPNSpecialDlg::~CIPNSpecialDlg()
{

}


/*******************************************************************/
/*
 -  CIPNSpecialDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CIPNSpecialDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}
