/*******************************************************************/
/*
 -  Attach.cpp
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CAttachDlg.
 *      CAttachDlg is a class dialog that allows manipulation and
 *      display of Attachment properties and operations.
 */
/*******************************************************************/
                                                   
#undef  CINTERFACE      // use C++ calling convention for mapi calls


#ifdef WIN32
#ifdef _WIN95
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
#ifdef _WIN95
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

/******************* Attachment Message Map ****************************/

BEGIN_MESSAGE_MAP(CAttachDlg, CDialog)

    ON_LBN_DBLCLK(  IDC_PROPS,          OnPropInterface)
    ON_COMMAND(     IDC_CALLFUNC,       OnCallFunction)
    ON_COMMAND(     IDC_PROPINTERFACE,  OnPropInterface)
    ON_COMMAND(     IDC_SPECIALPROPS,   OnSpecialProps)
                                        
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
    LPSPropValue    lpspva      = NULL;
    ULONG           ulValues    = 0;
    DWORD           dwIndex     = 0;
    
    SizedSPropTagArray(1,sptaMethod) = 
    {
        1, 
        { 
            PR_ATTACH_METHOD,
        }
    };


    // set title of dialog to attachment name/number
    SetWindowText( m_CurrentAttach.GetBuffer(15) );

    m_hResult = m_lpAttach->GetProps( (LPSPropTagArray) &sptaMethod,0, &ulValues, &lpspva);
    wsprintf(m_szLogBuf,"lpAttach->GetProps( &sptaMethod, &ulValues, &lpspva)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(m_hResult), m_szError ) );

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

    //***** Add Strings to call function combo box *****
    //***** Add Strings to call function combo box *****

    dwIndex = SendDlgItemMessage(IDC_FUNCTIONS, CB_ADDSTRING,
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "Attachment Properties" );

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
                                (WPARAM) 0 , (LPARAM) (LPCTSTR) "Open Attachment as Message" );

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
 -  CAttachDlg::
 -  OnCallFunction
 -
 *  Purpose:
 *
 /********************************************************************/

void CAttachDlg::OnCallFunction()
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
        // Attach FUNCTIONS
        case 0:        
            OnPropInterface();       
            break;
        case 1:
            OnMDBProp();             
            break;
        case 2:
            OnOpenAttachAsMsg();
            break;
        case 3:
            OnSetCopyToDest();      
            break;

        default:
            MessageBox( "CAttachDlg::OnCallFunction() default ",
                "Client", MBS_OOPS );
            break;       
    }    

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
 -  OnPropInterface
 -
 *  Purpose:
 *  
 /********************************************************************/

void CAttachDlg::OnPropInterface()
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
  
/*******************************************************************/
/*
 -  CAttachDlg::
 -  OnSpecialProps
 -
 *  Purpose:
 *      Closes the Attach dialog.
 *
 */
/*******************************************************************/

void CAttachDlg::OnSpecialProps()
{
    CAttachSpecialDlg    AttachSpecial((LPATTACH)m_lpAttach,this);
    AttachSpecial.DoModal();
}


/*******************************************************************/
/*********************** AttachSpecial **************************/

/********************************************************************/
/*
 -  CAttachSpecialDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CAttachSpecialDlg::OnInitDialog()
{
    CGetError       E;
    DWORD           dwReturn            = 0;
    HRESULT         hResult             = hrSuccess;
    char            szBuffer[300];
    ULONG           cVals               = 0;
    LPSPropValue    lpspva              = NULL;
    ULONG           i;

  
    SizedSPropTagArray(4,sptaAttachSpecial) =
    {
        4,
        {
            PR_ATTACH_METHOD,
            PR_OBJECT_TYPE,
            PR_ACCESS,
            PR_ACCESS_LEVEL,
        }
    };



    hResult = m_lpAttach->GetProps((LPSPropTagArray)&sptaAttachSpecial,0,&cVals,&lpspva);
    if( HR_FAILED(hResult) )
    {
        MessageBox( E.SzError("lpMDB->GetProps(sptaAttachSpecial)", hResult),
                 "Client", MBS_ERROR );
        return FALSE;
    }

    i = 0;


    // PR_ATTACH_METHOD
    if(lpspva[i].ulPropTag != PR_ATTACH_METHOD )
    {
        if( PROP_TYPE(lpspva[i].ulPropTag) == PT_ERROR)
            GetString( "MAPIErrors", lpspva[i].Value.err, szBuffer );
        else
            wsprintf(szBuffer,"PR_ATTACH_METHOD not available");
    }
    else
        GetString("AttachMethod",lpspva[0].Value.l,szBuffer);
    SetDlgItemText(IDT_ATTACH_METHOD1,szBuffer);
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


    MAPIFreeBuffer(lpspva);

    return TRUE;
}


/*******************************************************************/
/*
 -  CAttachSpecialDlg::
 -  ~CAttachSpecialDlg
 -
 *  Purpose:
 *      Destructor for class CAttachSpecialDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CAttachSpecialDlg::~CAttachSpecialDlg()
{

}


/*******************************************************************/
/*
 -  CAttachSpecialDlg::
 -  OnCancel
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CAttachSpecialDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}

          
                 