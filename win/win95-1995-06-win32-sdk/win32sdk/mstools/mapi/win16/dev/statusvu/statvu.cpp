/*
 -  S T A T V U . C P P
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *
 */

#ifdef WIN32
#ifdef _WIN95
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>     
#include <windowsx.h>
#include <stdio.h>
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


#include <mapiwin.h>
#include <mapix.h>
#include <strtbl.h>
#include <misctool.h>
#include <pvalloc.h>
#include <stentry.h>
#include <limits.h>
#include "resource.h"
#include "statvu.h"
#include "oper.h"

/* Global Objects */
CWnd *pStatusWnd;
CWnd *pLogWnd;


SizedSPropTagArray(5,sptaStatus) = 
{
    5, 
    { 
        PR_DISPLAY_NAME,
        PR_ENTRYID,
        PR_RESOURCE_TYPE,
        PR_PROVIDER_DLL_NAME,
        PR_RESOURCE_FLAGS 
    }
};

SizedSPropTagArray(3,sptaStatDisplay) = 
{
    3, 
    { 
        PR_DISPLAY_NAME,
        PR_RESOURCE_METHODS,
        PR_STATUS_STRING
    }
};


/* PROPVU functions */


extern HINSTANCE    hlibPROPVU;

typedef BOOL (*LPFNVIEWMAPIPROP)(
    LPSTR           lpszName,
    LPMAPIPROP FAR *lppMAPIProp,
    LPVOID          lpvDest,
    HWND            hWnd);    

extern LPFNVIEWMAPIPROP lpfnViewMapiProp;

#define PROPVUViewMapiProp (*lpfnViewMapiProp)


HINSTANCE           hlibPROPVU          = (HINSTANCE)NULL;
LPFNVIEWMAPIPROP    lpfnViewMapiProp    = NULL;




/*
 -  ViewStatusObj
 -
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *      void.
 *
 */

extern "C"
BOOL ViewStatusObj(LPMAPISESSION FAR *lppMAPISession, HWND hWnd)
{
    COpenStatusDlg     ChooseDlg(*lppMAPISession, hWnd);
    return (ChooseDlg.DoModal() == IDOK);
}


/*******************************************************************/

/* COpenStatusDlg implementation */

BEGIN_MESSAGE_MAP(COpenStatusDlg, CModalDialog)

    ON_COMMAND(     IDC_CHOOSE_OPEN,            OnOpen)
    ON_BN_CLICKED(  IDC_CHOOSE_MODIFY,          OnReadOnly)
    ON_COMMAND(     IDC_CHOOSE_XPORT,           OnTargetXport)

END_MESSAGE_MAP()


/*******************************************************************/
/*
 -  COpenStatus::
 -  OnInitDialog
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

BOOL COpenStatusDlg::OnInitDialog()
{
    int             rgTabStops[5];
    DWORD           dwReturn    = 0;

    SetWindowText( "Select Status Object To Open" );

    CheckDlgButton(IDC_CHOOSE_MODIFY,0);

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_CHOOSE_STATUS,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = 120 ;
    rgTabStops[1] = 200 ;

    dwReturn = SendDlgItemMessage(IDC_CHOOSE_STATUS,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    DisplayStatus();
    
    return TRUE;
}

/*******************************************************************/

void COpenStatusDlg::OnReadOnly()
{   
    CheckDlgButton(IDC_CHOOSE_MODIFY, !IsDlgButtonChecked(IDC_CHOOSE_MODIFY) );
}
   

/*******************************************************************/

void COpenStatusDlg::OnTargetXport()
{   
    LONG            lSelection  = 0;

    if( (lSelection = SendDlgItemMessage(IDC_CHOOSE_STATUS,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Status Object to Set as Target Xport for FlushQueues", 
                "OnOpen", MBS_ERROR );
        return;
    }

    m_cbEntryIDTarget = m_lpRows->aRow[lSelection].lpProps[1].Value.bin.cb;
    m_lpEntryIDTarget = (LPENTRYID)m_lpRows->aRow[lSelection].lpProps[1].Value.bin.lpb;
    m_szTarget = m_lpRows->aRow[lSelection].lpProps[0].Value.lpszA;
}
   
   
/*******************************************************************/
/*
 -  COpenStatus::
 -  OnOpen
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

void COpenStatusDlg::OnOpen()
{
    LONG            lSelection  = 0;
    ULONG           ulFlags     = 0;
    HRESULT         hResult     = hrSuccess;
    CGetError       E;
    LPMAPIPROP      lpObj       = NULL;
    ULONG           ulObjType   = 0;
    CStatusDlg      StatusDlg(this);
                     
    
    if( (lSelection = SendDlgItemMessage(IDC_CHOOSE_STATUS,LB_GETCURSEL,0,0 )) < 0 )
    {
        MessageBox("Select a Status Object to open", "OnOpen", MBS_ERROR );
        return;
    }
    
    // determine access privileges
    if( IsDlgButtonChecked(IDC_CHOOSE_MODIFY) )
        ulFlags = MAPI_MODIFY;
    else
        ulFlags = 0;

    if( HR_FAILED( hResult = m_lpSession->OpenEntry( 
                    (ULONG)     m_lpRows->aRow[lSelection].lpProps[1].Value.bin.cb,
                    (LPENTRYID) m_lpRows->aRow[lSelection].lpProps[1].Value.bin.lpb,
                    NULL,
                    ulFlags,
                    &ulObjType,
                    (LPUNKNOWN*) &lpObj ) ) )
    {
        MessageBox( E.SzError("lpSession->OpenEntry()", hResult),
                 "Client", MBS_ERROR );
        return;
    }               
 
    if(ulObjType == MAPI_STATUS)
    {   


        if(m_cbEntryIDTarget == 0)
        { 
            m_szTarget = "No Target Xport Specified";
        }
        
        StatusDlg.m_lResourceType   = m_lpRows->aRow[lSelection].lpProps[2].Value.l;
        StatusDlg.m_szDLLName       = m_lpRows->aRow[lSelection].lpProps[3].Value.lpszA;
        StatusDlg.m_lResourceFlags  = m_lpRows->aRow[lSelection].lpProps[4].Value.l;
        StatusDlg.m_lpStatus        = (LPMAPISTATUS)lpObj;
        StatusDlg.m_cbEntryIDTargetXport = m_cbEntryIDTarget; 
        StatusDlg.m_lpEntryIDTargetXport = m_lpEntryIDTarget; 
        StatusDlg.m_szTargetXport   = m_szTarget;
        StatusDlg.DoModal();
    }        
    else
    {
        MessageBox( "ulObjType != MAPI_STATUS, exiting ",
                 "Client", MBS_ERROR );
        return;
    }               

    if(lpObj)
        lpObj->Release();
               
    EndDialog(IDOK);
}

   
/*******************************************************************/
/*
 -  COpenStatus::
 -  ~COpenStatusDlg
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

COpenStatusDlg::~COpenStatusDlg()
{
    // FREE MEMORY AND OBJECTS, and exit dialog

    FreeRowSet(m_lpRows);

    if(m_lptblStatus)
    {
        m_lptblStatus->Release();
        m_lptblStatus = NULL;
    }
}
   
/*******************************************************************/
/*
 -  COpenStatus::
 -  OnDisplayStatus
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

void COpenStatusDlg::DisplayStatus()
{
    CGetError       E;
    LPSRowSet       lpRows                  = NULL;
    ULONG           iRow                    = 0;
    HRESULT         hResult                 = hrSuccess;
    ULONG           ulRows                  = 0;
    char            szBuffer[512];
    DWORD           dwReturn        = 0;
    
    SendDlgItemMessage(IDC_CHOOSE_STATUS,LB_RESETCONTENT,0,0);

    if(m_lptblStatus)
    {
        FreeRowSet(m_lpRows);
        m_lptblStatus->Release();
        m_lptblStatus = NULL;
    }
 
    if( HR_FAILED(hResult = m_lpSession->GetStatusTable(0,&m_lptblStatus) ) )
    {
        MessageBox( E.SzError("m_lpSession->GetStatusTable", hResult), 
                            "Client", MBS_ERROR );
        return;
    }
 
    if( HR_FAILED(hResult = m_lptblStatus->SetColumns( (LPSPropTagArray) &sptaStatus, 0) ) )
    {
        MessageBox( E.SzError("m_lptblStatus->SetColumns", hResult), 
                            "Client", MBS_ERROR );
        return;
    }

    if( HR_FAILED(hResult = m_lptblStatus->GetRowCount(0,&ulRows) ) )
    {
        MessageBox( E.SzError("m_lptblStatus->GetRowCount", hResult), 
                            "Client", MBS_ERROR );
        return;
    }
            
    if( HR_FAILED(hResult = m_lptblStatus->SeekRow( BOOKMARK_BEGINNING,0 , NULL ) ) )
    {
        MessageBox( E.SzError("m_lptblStatus->SeekRow", hResult), 
                            "Client", MBS_ERROR );
        return;
    }
    
    if( !HR_FAILED(hResult = m_lptblStatus->QueryRows( ulRows, 0, &m_lpRows ) ) )
    {    
        // if there is at least one row, find column of EntryID and Default Status
        if(m_lpRows->cRows)
        {
            // find out which row has the Default Status and open it
            for(iRow = 0; iRow < m_lpRows->cRows; iRow++)
            {            
                // display PR_DISPLAY_NAME
                strcpy(szBuffer,m_lpRows->aRow[iRow].lpProps[0].Value.lpszA);       
                strcat(szBuffer,"\t");

                // display PR_ENTRYID
                char    lpszHex[9];
                ULONG   cb      = 0;
                ULONG   cChars  = 0;
                LPBYTE  lpb     = m_lpRows->aRow[iRow].lpProps[1].Value.bin.lpb;

                while((cb < m_lpRows->aRow[iRow].lpProps[1].Value.bin.cb) && (cChars < 255 -16 ) )
                {
                    wsprintf(lpszHex, "%02X ", *lpb);
                    lstrcat(szBuffer, lpszHex);
                    cChars += 3;
                    lpb++;
                    cb++;
                }
                
                dwReturn = SendDlgItemMessage(IDC_CHOOSE_STATUS,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
            }
        }
    }
}    

/*******************************************************************/
/*
 -  COpenStatus::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void COpenStatusDlg::OnCancel()
{

    EndDialog(IDCANCEL);
}

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/* COpenStatusDlg implementation */

BEGIN_MESSAGE_MAP(CStatusDlg, CModalDialog)

    ON_COMMAND(     IDC_STATUS_PROPS,           OnStatusProps)
    ON_COMMAND(     IDC_STATUS_VALIDATE,        OnValidateState)
    ON_COMMAND(     IDC_STATUS_SETTINGS,        OnSettingsDialog)
    ON_COMMAND(     IDC_STATUS_FLUSH,           OnFlushQueues)
    ON_COMMAND(     IDC_STATUS_CHANGEPASS,      OnChangePassword)
    ON_COMMAND(     IDC_STATUS_REGNOTIF,        OnRegisterNotification)

END_MESSAGE_MAP()



   
/*******************************************************************/
/*
 -  CStatus::
 -  ~CStatusDlg
 *
 *  Purpose:
 *  Parameters
 *  Returns:
 *
 */
/*******************************************************************/

CStatusDlg::~CStatusDlg()
{
    // FREE MEMORY AND OBJECTS, and exit dialog

}


/********************************************************************/
/*
 -  CStatusDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */
/********************************************************************/

BOOL CStatusDlg::OnInitDialog()
{
    char   *szPropLibName;
    char    szPropMsgBuf[256];

    // ASSERTS
    if(!m_lpStatus)
        return FALSE;

        
    // LOAD UP PROPERTY DLL


#ifdef WIN16
    szPropLibName = "propvu.dll";
    if ((UINT)(hlibPROPVU = LoadLibrary (szPropLibName)) < 32)
#else
    szPropLibName = "propvu32.dll";
    if (!(hlibPROPVU = LoadLibrary (szPropLibName)))
#endif
    {
        wsprintf(szPropMsgBuf,"Cannot Load %s.  Make sure this DLL is available and in your path.",szPropLibName);
        MessageBox(szPropMsgBuf,"Error", MB_ICONSTOP | MB_OK);
        return FALSE;
    }
    
    if (!(lpfnViewMapiProp  =  (LPFNVIEWMAPIPROP)GetProcAddress (hlibPROPVU, "ViewMapiProp") ))
    {
        MessageBox("Cannot Load ViewMapiProp process address.","Error", MB_ICONSTOP | MB_OK);
        return FALSE;
    }


    DisplayProps();

    return TRUE;
}



/********************************************************************/


void CStatusDlg::DisplayProps()
{
    DWORD           dwIndex     = 0;    
    ULONG           cValues     = 0;
    LPSPropValue    lpspva      = NULL;
    HRESULT         hResult     = hrSuccess;
    char            szBuffer[80];
    CGetError       E;
       
    if( HR_FAILED( hResult = m_lpStatus->GetProps( (LPSPropTagArray) &sptaStatDisplay,0, &cValues, &lpspva) ) )
    {
        MessageBox( E.SzError("CPropDlg::OnBldProps()  lpStatus->GetProps()",
             hResult), "Client", MBS_ERROR );
        return;
    }

    
    // PR_DISPLAY_NAME
    if( PROP_TYPE(lpspva[0].ulPropTag) == PT_ERROR)
    {
        
        if(GetString( "MAPIErrors", lpspva[0].Value.err, szBuffer ))
            wsprintf( szBuffer, "PT_ERROR: %04X", lpspva[0].Value.err );

        dwIndex = SendDlgItemMessage(IDC_STATUS_DISPLAY_NAME,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }
    else
    {
        dwIndex = SendDlgItemMessage(IDC_STATUS_DISPLAY_NAME,LB_ADDSTRING,0,
                        (LPARAM) lpspva[0].Value.lpszA );
    }
    
    // REQUIRED
    // PR_RESOURCE_TYPE
    GetString("ResourceType",m_lResourceType,szBuffer);
    dwIndex = SendDlgItemMessage(IDC_STATUS_TYPE,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    
    // REQUIRED
    // PR_PROVIDER_DLL_NAME
    dwIndex = SendDlgItemMessage(IDC_STATUS_DLL_NAME,LB_ADDSTRING,0,
                            (LPARAM) m_szDLLName.GetBuffer(30) );

    // PR_RESOURCE_FLAGS    
    if( (m_lResourceFlags &  STATUS_DEFAULT_OUTBOUND) )
    {
         strcpy(szBuffer,"STATUS_DEFAULT_OUTBOUND");
         dwIndex = SendDlgItemMessage(IDC_STATUS_FLAGS,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }
    if( (m_lResourceFlags &  STATUS_DEFAULT_STORE) )
    {
         strcpy(szBuffer,"STATUS_DEFAULT_STORE");
         dwIndex = SendDlgItemMessage(IDC_STATUS_FLAGS,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }
    if( (m_lResourceFlags &  STATUS_PRIMARY_IDENTITY) )
    {
         strcpy(szBuffer,"STATUS_PRIMARY_IDENTITY");
         dwIndex = SendDlgItemMessage(IDC_STATUS_FLAGS,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }
    if( (m_lResourceFlags &  STATUS_SIMPLE_STORE) )
    {
         strcpy(szBuffer,"STATUS_SIMPLE_STORE");
         dwIndex = SendDlgItemMessage(IDC_STATUS_FLAGS,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }
    if( (m_lResourceFlags &  STATUS_XP_PREFER_LAST) )
    {
         strcpy(szBuffer,"STATUS_XP_PREFER_LAST");
         dwIndex = SendDlgItemMessage(IDC_STATUS_FLAGS,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }



    // PR_RESOURCE_METHODS
    if( PROP_TYPE(lpspva[1].ulPropTag) == PT_ERROR)
    {
        
        if(GetString( "MAPIErrors", lpspva[1].Value.err, szBuffer ))
            wsprintf( szBuffer, "PT_ERROR: %04X", lpspva[1].Value.err );

        dwIndex = SendDlgItemMessage(IDC_STATUS_METHODS,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }
    else
    {
        if( (lpspva[1].Value.l &  STATUS_VALIDATE_STATE) )
        {
             strcpy(szBuffer,"STATUS_VALIDATE_STATE");
             dwIndex = SendDlgItemMessage(IDC_STATUS_METHODS,LB_ADDSTRING,0,
                            (LPARAM) szBuffer );
        }
        if( (lpspva[1].Value.l &  STATUS_SETTINGS_DIALOG) )
        {
             strcpy(szBuffer,"STATUS_SETTINGS_DIALOG");
             dwIndex = SendDlgItemMessage(IDC_STATUS_METHODS,LB_ADDSTRING,0,
                            (LPARAM) szBuffer );
        }
        if( (lpspva[1].Value.l &  STATUS_CHANGE_PASSWORD) )
        {
             strcpy(szBuffer,"STATUS_CHANGE_PASSWORD");
             dwIndex = SendDlgItemMessage(IDC_STATUS_METHODS,LB_ADDSTRING,0,
                            (LPARAM) szBuffer );
        }
        if( (lpspva[1].Value.l &  STATUS_FLUSH_QUEUES) )
        {
             strcpy(szBuffer,"STATUS_FLUSH_QUEUES");
             dwIndex = SendDlgItemMessage(IDC_STATUS_METHODS,LB_ADDSTRING,0,
                            (LPARAM) szBuffer );
        }
        if( (lpspva[1].Value.l &  STATUS_OWN_STORE) )
        {
             strcpy(szBuffer,"STATUS_OWN_STORE");
             dwIndex = SendDlgItemMessage(IDC_STATUS_METHODS,LB_ADDSTRING,0,
                            (LPARAM) szBuffer );
        }
    }
    
    
    // PR_STATUS_STRING
    if( PROP_TYPE(lpspva[2].ulPropTag) == PT_ERROR)
    {
        
        if(GetString( "MAPIErrors", lpspva[2].Value.err, szBuffer ))
            wsprintf( szBuffer, "PT_ERROR: %04X", lpspva[2].Value.err );

        dwIndex = SendDlgItemMessage(IDC_STATUS_CODE,LB_ADDSTRING,0,
                        (LPARAM) szBuffer );
    }
    else
    {
        dwIndex = SendDlgItemMessage(IDC_STATUS_CODE,LB_ADDSTRING,0,
                                (LPARAM) lpspva[2].Value.lpszA );
    }    

    // FREE MEMORY    
    if(lpspva)
        MAPIFreeBuffer(lpspva);

}

/********************************************************************/

void CStatusDlg::OnStatusProps()
{
    PROPVUViewMapiProp("Status Props", 
                (LPMAPIPROP FAR *)&m_lpStatus,NULL, (HWND)this->m_hWnd );
}

/********************************************************************/

void CStatusDlg::OnValidateState()
{
    HRESULT         hResult         = hrSuccess;
    COperation      ValidateStateDlg(this);
    ULONG           ulFlags         = 0;
    CGetError       E;
    ULONG           ulUIParam       = 0;
    int             dRet            = 0;
    char            szBuff[80];        

    ValidateStateDlg.m_CurrentOperation= "lpStatus->ValidateState()";
    ValidateStateDlg.m_CBText1         = "ulUIParam:";
    ValidateStateDlg.m_FlagText1       = "SUPPRESS_UI";
    ValidateStateDlg.m_FlagText2       = "REFRESH_XP_HEADER_CACHE";
    ValidateStateDlg.m_FlagText3       = "PROCESS_XP_HEADER_CACHE";
    ValidateStateDlg.m_FlagText4       = "FORCE_XP_CONNECT";
    ValidateStateDlg.m_FlagText5       = "FORCE_XP_DISCONNECT";
    ValidateStateDlg.m_FlagText6       = "CONFIG_CHANGED";

    dRet = ValidateStateDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = ValidateStateDlg.m_CBContents1.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( ValidateStateDlg.DoModal() == IDOK )   
    {       
        if( !lstrcmp(ValidateStateDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;                    

        if( ValidateStateDlg.m_bFlag1 )                
            ulFlags |= SUPPRESS_UI;

        if( ValidateStateDlg.m_bFlag2 )                
            ulFlags |= REFRESH_XP_HEADER_CACHE;

        if( ValidateStateDlg.m_bFlag3 )                
            ulFlags |= PROCESS_XP_HEADER_CACHE;

        if( ValidateStateDlg.m_bFlag4 )                
            ulFlags |= FORCE_XP_CONNECT;

        if( ValidateStateDlg.m_bFlag5 )                
            ulFlags |= FORCE_XP_DISCONNECT;

        if( ValidateStateDlg.m_bFlag6 )                
            ulFlags |= CONFIG_CHANGED;
                
        if( HR_FAILED(hResult = m_lpStatus->ValidateState(
                        ulUIParam,
                        ulFlags ) ) ) 
        {
            MessageBox( E.SzError("lpStatus->ValidateState()",
                     hResult),"Client", MBS_ERROR );
            return;
        }        
    }


}

/********************************************************************/

void CStatusDlg::OnSettingsDialog()
{
    HRESULT         hResult         = hrSuccess;
    COperation      SettingsDlg(this);
    ULONG           ulFlags             = 0;
    CGetError       E;
    ULONG           ulUIParam       = 0;
    int             dRet            = 0;
    char            szBuff[80];        

    SettingsDlg.m_CurrentOperation= "lpStatus->SettingsDialog()";
    SettingsDlg.m_CBText1         = "ulUIParam:";
    SettingsDlg.m_FlagText1       = "UI_READONLY";
    SettingsDlg.m_FlagText2       = "Invalid Flag";

    dRet = SettingsDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = SettingsDlg.m_CBContents1.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( SettingsDlg.DoModal() == IDOK )   
    {       
        if( !lstrcmp(SettingsDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;                    

        if( SettingsDlg.m_bFlag1 )                
            ulFlags |= UI_READONLY;

        if( SettingsDlg.m_bFlag2 )                
            ulFlags |= TEST_INVALID_FLAG;
                
        if( HR_FAILED(hResult = m_lpStatus->SettingsDialog(
                        ulUIParam,
                        ulFlags ) ) ) 
        {
            MessageBox( E.SzError("lpStatus->SettingsDialog()",
                     hResult),"Client", MBS_ERROR );
            return;
        }        
    }
}

/********************************************************************/

void CStatusDlg::OnFlushQueues()
{
    HRESULT         hResult         = hrSuccess;
    COperation      FlushQueuesDlg(this);
    ULONG           ulFlags         = 0;
    CGetError       E;
    ULONG           ulUIParam       = 0;
    int             dRet            = 0;
    char            szBuff[80];        

    FlushQueuesDlg.m_CurrentOperation= "lpStatus->FlushQueues()";
    FlushQueuesDlg.m_CBText1         = "ulUIParam:";
    FlushQueuesDlg.m_FlagText1       = "FLUSH_NO_UI";
    FlushQueuesDlg.m_FlagText2       = "FLUSH_UPLOAD";
    FlushQueuesDlg.m_FlagText3       = "FLUSH_DOWNLOAD";
    FlushQueuesDlg.m_FlagText4       = "FLUSH_FORCE";
    FlushQueuesDlg.m_FlagText5       = "Invalid Flag";

    FlushQueuesDlg.m_EditText1       = "cbTargetXport:";
    FlushQueuesDlg.m_EditText2       = "lpTargetXport:";
    FlushQueuesDlg.m_EditText3       = "Target DisplayName:";
    
    wsprintf(szBuff,"%lu",m_cbEntryIDTargetXport);    
    FlushQueuesDlg.m_EditDefault1    = szBuff;

    wsprintf(szBuff,"0x%X",m_lpEntryIDTargetXport);    
    FlushQueuesDlg.m_EditDefault2    = szBuff;

    FlushQueuesDlg.m_EditDefault3    = m_szTargetXport;

    dRet = FlushQueuesDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = FlushQueuesDlg.m_CBContents1.Add(szBuff);

    // bring up modal dialog box, and if user hits OK, process operation
    if( FlushQueuesDlg.DoModal() == IDOK )   
    {       
        if( !lstrcmp(FlushQueuesDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;                    

        if( FlushQueuesDlg.m_bFlag1 )                
            ulFlags |= FLUSH_NO_UI;

        if( FlushQueuesDlg.m_bFlag2 )                
            ulFlags |= FLUSH_UPLOAD;

        if( FlushQueuesDlg.m_bFlag3 )                
            ulFlags |= FLUSH_DOWNLOAD;

        if( FlushQueuesDlg.m_bFlag4 )                
            ulFlags |= FLUSH_FORCE;

        if( FlushQueuesDlg.m_bFlag5 )                
            ulFlags |= TEST_INVALID_FLAG;
                
        if( HR_FAILED(hResult = m_lpStatus->FlushQueues(
                        ulUIParam,
                        m_cbEntryIDTargetXport,
                        m_lpEntryIDTargetXport,
                        ulFlags ) ) ) 
        {
            MessageBox( E.SzError("lpStatus->FlushQueues()",
                     hResult),"Client", MBS_ERROR );
            return;
        }        
    }
}

/********************************************************************/

void CStatusDlg::OnChangePassword()
{
    HRESULT         hResult         = hrSuccess;
    COperation      ChangePasswordDlg(this);
    ULONG           ulFlags             = 0;
    CGetError       E;
    LPWSTR          lpNewBuffer1        = NULL;
    LPWSTR          lpNewBuffer2        = NULL;

    // initalize data for dialog box
    ChangePasswordDlg.m_CurrentOperation= "lpStatus->ChangePassword()";
    ChangePasswordDlg.m_EditText1       = "lpOldPass:";
    ChangePasswordDlg.m_EditText2       = "lpNewPass:";
    ChangePasswordDlg.m_FlagText1       = "MAPI_UNICODE";
    ChangePasswordDlg.m_FlagText2       = "Invalid Flag";
    ChangePasswordDlg.m_EditDefault1    = "Enter your old password here";
    ChangePasswordDlg.m_EditDefault2    = "Enter your new password here";

    // bring up modal dialog box, and if user hits OK, process operation
    if( ChangePasswordDlg.DoModal() == IDOK )   
    {       
        // determine state/settings of data in dialog upon closing
        if( ChangePasswordDlg.m_bFlag2 )                
            ulFlags |= TEST_INVALID_FLAG;
                
        if( ChangePasswordDlg.m_bFlag1 )                
        {
            ulFlags |= MAPI_UNICODE;

            String8ToUnicode(ChangePasswordDlg.m_szEdit1, &lpNewBuffer1, NULL);
            String8ToUnicode(ChangePasswordDlg.m_szEdit2, &lpNewBuffer2, NULL);

            if( HR_FAILED(hResult = m_lpStatus->ChangePassword(
                            (LPTSTR) lpNewBuffer1,
                            (LPTSTR) lpNewBuffer2,
                            ulFlags ) ) ) 
            {
                MessageBox( E.SzError("lpStatus->ChangePassword()",
                         hResult),"Client", MBS_ERROR );
                return;
            }        
        
            PvFree(lpNewBuffer1);
            PvFree(lpNewBuffer2);
        }
        else
        {

            if( HR_FAILED(hResult = m_lpStatus->ChangePassword(
                            (LPTSTR) ChangePasswordDlg.m_szEdit1,
                            (LPTSTR) ChangePasswordDlg.m_szEdit2,
                            ulFlags ) ) ) 
            {
                MessageBox( E.SzError("lpStatus->ChangePassword()",
                         hResult),"Client", MBS_ERROR );
                return;
            }        
        }
    }
}

/********************************************************************/

void CStatusDlg::OnRegisterNotification()
{
    MessageBox("TEST Not Yet Implemented", "INFO", MBS_INFO );

}

/*******************************************************************/
/*
 -  CStatusDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CStatusDlg::OnCancel()
{
    if (hlibPROPVU)
    {
        FreeLibrary (hlibPROPVU);
        hlibPROPVU = (HINSTANCE) NULL;
    }

    EndDialog(IDCANCEL);
}


/********************************************************************/
/********************** CGetError Member Functions ******************/
/*******************************************************************/

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


/*--------------------*/
/* Library Init stuff */
/*--------------------*/

class CStatViewDLL : public CWinApp
{
public:
    virtual BOOL InitInstance();
    virtual BOOL ExitInstance();

    CStatViewDLL(const char *pszAppName)
            : CWinApp(pszAppName)
        {
        }
};

BOOL CStatViewDLL::InitInstance()
{
    SetDialogBkColor();
    return TRUE;
}


BOOL CStatViewDLL::ExitInstance()
{
    return TRUE;
}

CStatViewDLL  vtDLL("statvu32.dll");
