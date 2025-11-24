/*******************************************************************/
/*
 -  Attach.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CAttachDlg.
 *      CAttachDlg is a class dialog that allows manipulation and
 *      display of Attachment properties and operations.
 */
/*******************************************************************/
                                                   
#undef  CINTERFACE      // use C++ calling convention for mapi calls


#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>     
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

#include <mapix.h>
#include <strtbl.h>     
#include "resource.h"   
#include "mdbview.h" 
#include "attach.h"
#include "oper.h"
#include "msg.h"

/* PROPVU functions */

extern HINSTANCE    hlibPROPVU;

typedef BOOL (*LPFNVIEWMAPIPROP)(
    LPSTR           lpszName,
    LPMAPIPROP FAR *lppMAPIProp,
    LPVOID          lpvDest,
    HWND            hWnd);    

extern LPFNVIEWMAPIPROP lpfnViewMapiProp;

#define PROPVUViewMapiProp (*lpfnViewMapiProp)



extern LPMDB                lpMDB;
extern LPVOID               lpvCopyToDest;
//extern CString              CopyToDest;

/******************* Attachment Message Map ****************************/

BEGIN_MESSAGE_MAP(CAttachDlg, CDialog)

    ON_LBN_DBLCLK(  IDC_PROPS,          OnAttachProp)        
    ON_BN_CLICKED(  IDC_TYPES,          OnType)
    ON_BN_CLICKED(  IDC_VALUES,         OnValues)
    ON_BN_CLICKED(  IDC_DATA,           OnData)
    ON_COMMAND(     IDC_STRING,         OnString)
    ON_COMMAND(     IDC_HEX,            OnHex)
    ON_COMMAND(     IDC_DECIMAL,        OnDecimal)
    ON_LBN_SETFOCUS(IDC_PROPS,          OnProperty)
    ON_COMMAND(     IDC_ATTPROP,        OnAttachProp)
    ON_COMMAND(     IDC_MDBPROP,        OnMDBProp)
    ON_COMMAND(     IDC_MAPILOGCLEAR,   OnClearMapiLog)
    ON_COMMAND(     IDC_COPYTODEST,     OnSetCopyToDest)
    ON_COMMAND(     IDC_ATTOPENASMSG,   OnOpenAttachAsMsg)
    
         
END_MESSAGE_MAP()

/*******************************************************************/
/**************************** Attach ******************************/

/*******************************************************************/
/*
 -  CAttachDlg::
 -  OnSetCopyToDest
 *
 *  Purpose:
 *      Set Destination object of CopyTo operation or CopyMessage or CopyFolder
 *
 *  lpvCopyToDest and CopyToDest are global
 */
/*******************************************************************/

void CAttachDlg::OnSetCopyToDest()
{                
    char    szBuffer[256];
    
    lpvCopyToDest   = (LPVOID) m_lpAttach;       

    wsprintf(szBuffer,"%s is next Destination for CopyTo(), CopyMessage(), or CopyFolder()",
                m_CurrentAttach.GetBuffer(40) );

    MessageBox( szBuffer,"Client", MBS_INFO );

}    

/********************************************************************/
/*
 -  CAttachDlg::
 -  InitAttachDialog
 -
 *  Purpose:
 *      Initialize the dialog after it was created in the PropDlg class
 *      This routine is called in the constructor in instead of OnInitDialog()
 *      so that it can fill in all of the appropriate listboxes, etc in
 *      the dialog.  We can't use the OnInitDialog because the OnInitDialog
 *      in the PropDialog Class and MFC doesn't explicitly call it for
 *      this inherited class as well.
 */
/********************************************************************/

BOOL CAttachDlg::InitAttachDialog()
{
    DWORD           dwReturn    = 0;
    int             rgTabStops[2];
    LPSPropValue    lpspva      = NULL;
    ULONG           ulValues    = 0;

    
    SizedSPropTagArray(1,sptaMethod) = 
    {
        1, 
        { 
            PR_ATTACH_METHOD,
        }
    };


    // set title of dialog to attachment name/number
    SetWindowText( m_CurrentAttach.GetBuffer(15) );

    dwReturn = SendDlgItemMessage(IDC_ATTCURRENT,LB_ADDSTRING,0,
                        (LPARAM) m_CurrentAttach.GetBuffer(20));


    m_hResult = m_lpAttach->GetProps( (LPSPropTagArray) &sptaMethod,0, &ulValues, &lpspva);
    wsprintf(m_szLogBuf,"lpAttach->GetProps( &sptaMethod, &ulValues, &lpspva)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
    SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,(LPARAM) m_szLogBuf );

    if( HR_FAILED(m_hResult))        
    {
        MessageBox( m_E.SzError("lpAttach->GetProps(&spta)",m_hResult),
                     "Client", MBS_ERROR );
    }

    dwReturn = SendDlgItemMessage(IDC_ATTMETHOD,LB_ADDSTRING,0,
                        (LPARAM) GetString("AttachMethod",lpspva[0].Value.l,NULL) );

    if(lpspva)
        MAPIFreeBuffer(lpspva);

    // disable the system menu close item
    //  this is done because there is a MFC 2.0 bug that
    //  makes you capture several PostNcDestroy messages etc.
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED);

    // inherited property class operations for initialization
    ResetPropButtons();
    RedrawPropTable();

    rgTabStops[0] = MAPILOG_LISTBOX_TAB1_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_MAPILOG,LB_SETTABSTOPS,
                    (WPARAM) 1,(LPARAM)rgTabStops );

    dwReturn = SendDlgItemMessage(IDC_MAPILOG, LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    //Disable table Viewer button if DLL is not present.
    if(!hlibPROPVU)    
    {
        GetDlgItem(IDC_ATTPROP          )->EnableWindow(0);
        GetDlgItem(IDC_MDBPROP          )->EnableWindow(0);
    }
    else
    {
        GetDlgItem(IDC_ATTPROP          )->EnableWindow(1);
        GetDlgItem(IDC_MDBPROP          )->EnableWindow(1);
    }
  
    return TRUE;
}   



/*******************************************************************/
/*
 -  CAttachDlg::
 -  OnOpenAttachAsMsg
 *
 *  Purpose:
 *     Open An Attachment as a message in message
 */
/*******************************************************************/

void CAttachDlg::OnOpenAttachAsMsg()
{            
    LPIID           lpInterface     = 0;
    LPUNKNOWN       lpUnk           = NULL;
    ULONG           ulInterfaceOptions = 0;    
    CMessageDlg     *lpMessageDlg   = NULL;

    // if we have a data object, open property on this and open the message
    lpInterface = (LPIID)(&IID_IMessage);

    m_hResult = m_lpAttach->OpenProperty(
                            PR_ATTACH_DATA_OBJ,
                            lpInterface,
                            ulInterfaceOptions,
                            MAPI_MODIFY,
                            &lpUnk  );
    wsprintf(m_szLogBuf,"lpAttach->OpenProperty(PR_ATTACH_DATA_OBJ,0x%X,%lu,MAPI_MODIFY,&lpUnk)\t SC: %s",
                        lpInterface,ulInterfaceOptions, GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );
    SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM) m_szLogBuf );

    if(HR_FAILED(m_hResult) )
    {
        MessageBox( m_E.SzError("lpAttach->OpenProperty(PR_ATTACH_DATA_OBJ)", m_hResult), 
                            "Client", MBS_ERROR );
        return;
    }

    lpMessageDlg   =  new CMessageDlg("MESSAGE IN MESSAGE",(LPMESSAGE)lpUnk,this);
}    



/*******************************************************************/
/*
 -  CAttachDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Closes the Attach viewer dialog.
 *
 */
/*******************************************************************/

void CAttachDlg::OnCancel()
{
    delete this;
}

/*******************************************************************/
/*
 -  CAttachDlg::
 -  Cleanup
 -
 *  Purpose:
 *      Cleanup called by destructor for class CAttachDlg.  
 *      Releases and Frees memory allocated in class
 */
/*******************************************************************/

void CAttachDlg::Cleanup()
{   
    if(m_lpAttach)
    {
        m_lpAttach->Release();
        m_lpAttach  = NULL;
        m_lpEntry   = NULL;
    }
}    



/********************************************************************/
/*
 -  CAttachDlg::
 -  OnFldProp
 -
 *  Purpose:
 *  
 /********************************************************************/

void CAttachDlg::OnAttachProp()
{       
    char    szBuffer[80];
    
    
    lstrcpy(szBuffer,m_CurrentAttach.GetBuffer(50));
    lstrcat(szBuffer," Properties");

    PROPVUViewMapiProp(szBuffer, 
                (LPMAPIPROP FAR *)&m_lpEntry,lpvCopyToDest, (HWND)this->m_hWnd );
}


/********************************************************************/
/*
 -  CAttachDlg::
 -  OnMDBProp
 -
 *  Purpose:
 *  
 /********************************************************************/

void CAttachDlg::OnMDBProp()
{   
    PROPVUViewMapiProp("MDB Properties", 
            (LPMAPIPROP FAR *)&lpMDB, lpvCopyToDest, (HWND)this->m_hWnd );
}
  
                 