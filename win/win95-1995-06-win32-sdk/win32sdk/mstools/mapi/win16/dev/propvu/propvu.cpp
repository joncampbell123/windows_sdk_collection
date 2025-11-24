/*
 -  P R O P V U . C P P
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
#include <pventry.h>
#include "resource.h"
#include "propvu.h"
#include "bldprop.h"
#include "oper.h"
#include "getprop.h"
#include "getlist.h"
#include "namesids.h"
#include "iid.h"
#include "results.h"

/*
 -  ViewMapiProp
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
BOOL ViewMapiProp(LPSTR lpszName, LPMAPIPROP FAR *lppMAPIProp, LPVOID lpvDest,HWND hWnd)
{
    CString     CurrentProp(lpszName);
    CPropDlg    *lpDlgProp = NULL;
    
    
    lpDlgProp   =  new CPropDlg(CurrentProp, lppMAPIProp,lpvDest, hWnd);


    return TRUE;
}

/******************* Message Map ****************************/

BEGIN_MESSAGE_MAP(CPropDlg, CDialog)

    ON_BN_CLICKED(  IDC_PROPTYPE,       OnType)
    ON_BN_CLICKED(  IDC_PROPID,         OnID)
    ON_BN_CLICKED(  IDC_PROPDATA,       OnData)
    ON_COMMAND(     IDC_PROPSTRING,     OnString)
    ON_COMMAND(     IDC_PROPHEX,        OnHex)
    ON_COMMAND(     IDC_PROPDEC,        OnDecimal)
    ON_COMMAND(     IDC_ADDREF,         OnAddRef)
    ON_COMMAND(     IDC_COPYTO,         OnCopyTo)
    ON_COMMAND(     IDC_DELETEPROP,     OnDeleteProps)
    ON_COMMAND(     IDC_IDSNAMES,       OnGetIDsFromNames)
    ON_COMMAND(     IDC_GETLASTERR,     OnGetLastError)
    ON_COMMAND(     IDC_NAMESIDS,       OnGetNamesFromIDs)
    ON_COMMAND(     IDC_GETPROPS,       OnGetProps)
    ON_COMMAND(     IDC_GETPROPLIST,    OnGetPropList)
    ON_COMMAND(     IDC_OPENPROP,       Stub)
    ON_COMMAND(     IDC_QUERYINT,       OnQueryInterface)
    ON_COMMAND(     IDC_RELEASE,        OnRelease)
    ON_COMMAND(     IDC_SAVECHANGES,    OnSaveChanges)
    ON_COMMAND(     IDC_SETPROPS,       OnSetProps)
    ON_COMMAND(     IDC_COPYPROPS,      OnCopyProps)
    ON_COMMAND(     IDC_CLEARMAPILOG,   OnClearMapiLog)
    ON_COMMAND(     IDC_STOREPROPS,     OnStorePropValsToFile)
                
END_MESSAGE_MAP()


/*************************** StorePropValDlg Functions *********************/

/********************************************************************/

BEGIN_MESSAGE_MAP(CStorePropValDlg, CModalDialog)

    ON_BN_CLICKED(  IDC_STORE_BINARY,       OnDumpBinary)
         
END_MESSAGE_MAP()

/*******************************************************************/

void CStorePropValDlg::OnDumpBinary()
{
    CheckDlgButton(IDC_STORE_BINARY, !IsDlgButtonChecked(IDC_STORE_BINARY) );
}



/*******************************************************************/
/*
 -  CStorePropValDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */
/*******************************************************************/

BOOL CStorePropValDlg::OnInitDialog()
{
    DWORD           dwIndex         = 0;
    DWORD           dwReturn        = 0;

    SetDlgItemText(IDC_STORE_FILENAME,m_FileName);
    SetDlgItemText(IDC_STORE_TAGID,   m_TagID);

    return TRUE;
}

/*******************************************************************/
/*
 -  CStorePropValDlg::
 -  OnOK
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CStorePropValDlg::OnOK()
{
    LONG    dRet    = 0;
    
    // get edit control selection and put in data member strings
    *(WORD *)m_szFileName = sizeof(m_szFileName) -1;    // first char has buffer length   
    dRet = SendDlgItemMessage(IDC_STORE_FILENAME,EM_GETLINE,0,(LPARAM)m_szFileName);
    m_szFileName[dRet] = '\0';

    // get edit control selection and put in data member strings
    *(WORD *)m_szTagID = sizeof(m_szTagID) -1;    // first char has buffer length   
    dRet = SendDlgItemMessage(IDC_STORE_TAGID,EM_GETLINE,0,(LPARAM)m_szTagID);
    m_szTagID[dRet] = '\0';

    if( IsDlgButtonChecked(IDC_STORE_BINARY) )                
        m_ulFlags |= DUMP_BINARY_DATA;
    
    EndDialog(IDOK);
    
}


/*******************************************************************/
/*
 -  CStorePropValDlg::
 -  ~CStorePropValDlg
 -
 *  Purpose:
 *
 */
/*******************************************************************/

CStorePropValDlg::~CStorePropValDlg()
{

}


/*************************** PropDlg Functions *********************/

/*******************************************************************/
/*
 -  CPropDlg::
 -  OnStorePropValsToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CPropDlg::OnStorePropValsToFile()
{
    ULONG               ulValues        = 0;
    LPSPropValue        lpPropValue     = NULL;
    CGetError           E;
    char                szTag[80];
    char                szFileName[80];
    CStorePropValDlg    StoreProp(this);
    LPTSTR              lpszTemp;
    char                szBuff[512];
    CBldPropDlg         BldProp(this);
    LPSTR               lpszPropID      = NULL;
    ULONG               cbString        = 0;
    ULONG               idx             = 0;
    DWORD               dwIndex         = 0;
    LPSPropValue        lpspva          = NULL;       

    // need to set m_lSelectTag in case it is not one of our "MAPI" tags
    //  and it is stored as a hex value so that setprops can
    //  represent this value correctly
    if( HR_FAILED(m_hResult = m_lpEntry->GetProps(NULL, 0, &ulValues, &lpPropValue) ) )
    {
        MessageBox( E.SzError("CPropDlg::OnBldProps()  lpEntry->GetProps()", m_hResult), "Client", MBS_ERROR );
        return;
    }


    BldProp.m_fIsPropValueArray     = TRUE;
    BldProp.m_lpSetEntry            = m_lpEntry;
    BldProp.m_Operation             = "Dump Selected Properties to File";
    
    // initialize the selected ListBox
    BldProp.m_lppNewPropValue = (LPSPropValue *) PvAlloc(MAX_SET_PROPS * sizeof(LPSPropValue));
    for(idx = 0; idx < ulValues; idx++)
    {
        lpspva = (LPSPropValue) PvAlloc(sizeof(SPropValue));
        CopyPropValue(lpspva,&(lpPropValue[idx]),lpspva);
        BldProp.m_lppNewPropValue[idx] = lpspva;
    }
    BldProp.m_ulNewValues = ulValues;        
   
    BldProp.DoModal();

    if(BldProp.m_fCall)
    {
        // find file to open
        lpszTemp = getenv("MAPITEST");

        if(lpszTemp)
            strcpy(szFileName, lpszTemp);
        else
            strcpy(szFileName, "c:\\mapitest");

        strcat(szFileName, "\\data\\propvu.txt");

        // create the tag with braces around it
        strcpy(szTag,"[PVA]");

        StoreProp.m_TagID       = szTag;
        StoreProp.m_FileName    = szFileName;
        
        if( StoreProp.DoModal() == IDOK )
        {    
                              
            WritePropValArray(  StoreProp.m_szFileName,
                                StoreProp.m_szTagID,
                                BldProp.m_ulNewValues,
                                BldProp.m_lpspvaSelected,
                                StoreProp.m_ulFlags);

            wsprintf(szBuff,"Stored Props to File: %s with TagID: %s",
                        StoreProp.m_szFileName,StoreProp.m_szTagID);
            dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                            (LPARAM)szBuff );

        }

    }
    
    if(lpPropValue)
        MAPIFreeBuffer(lpPropValue);
}




/*******************************************************************/
/*
 -  CPropDlg::
 -  Stub
 *
 *  Purpose:
 *      Generic stub for functions not yet implemented
 */
/*******************************************************************/

void CPropDlg::Stub()
{
    MessageBox("Not Yet Implemented", "Client", MBS_INFO );
}

/********************************************************************/
/*
 -  CPropDlg::
 -  ResetPropButtons    (INTERNAL, protected member function)
 -
 *  Purpose:
 *      Reset Property Display configurable buttons to default(string/all)
 *
 *  Returns:
 *      Nothing
 */
/********************************************************************/

void CPropDlg::ResetPropButtons()
{
    // set check state of checkboxes (display all fields in string)
    CheckDlgButton(IDC_PROPSTRING,  1);
    CheckDlgButton(IDC_PROPHEX,     0);
    CheckDlgButton(IDC_PROPDEC,     0);
    CheckDlgButton(IDC_PROPTYPE,    1);
    CheckDlgButton(IDC_PROPDATA,    1);
    CheckDlgButton(IDC_PROPID,      1);
}


/********************************************************************/
/*
 -  CPropDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */
/********************************************************************/

BOOL CPropDlg::OnInitDialog()
{
    DWORD           dwIndex         = 0;
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;

    if( !m_lpEntry)
         return FALSE;

    SetWindowText( m_CurrentProp.GetBuffer(80) );

    // disable the system menu close item
    //  this is done because there is a MFC 2.0 bug that
    //  makes you capture several PostNcDestroy messages etc.
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_GRAYED);


    // set size of horizontal scroll and tab stops in prop view listbox
    dwReturn = SendDlgItemMessage(IDC_PROPVIEW,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_PROPVIEW,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );


    // set size of horizontal scroll and tab stops in MAPI log listbox
    dwReturn = SendDlgItemMessage(IDC_MAPILOG, LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

//  rgTabStops[0] = MAPILOG_LISTBOX_TAB1_SIZE ;
    rgTabStops[0] = 115 ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_MAPILOG,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );



    // display props
    ResetPropButtons();

    RedrawPropTable();

    return TRUE;
}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnQueryInterface
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnQueryInterface()
{
    COperation      QueryIntDlg(this);
    CPropDlg        *lpNewProp          = NULL;
    LPVOID          lpvNew              = NULL;
    LPIID           lpInterface         = NULL;
    CGetError       E;
    ULONG           idx;
    char            szBuff[80];
    int             dRet                = 0;
    DWORD           dwIndex             = 0;
    

    // initalize data for dialog box
    QueryIntDlg.m_CurrentOperation= "lpObj->QueryInterface()";
    QueryIntDlg.m_CBText1         = "lpInterface:";
    QueryIntDlg.m_CBText2         = "";
    QueryIntDlg.m_CBText3         = "";
    QueryIntDlg.m_EditText1       = "";
    QueryIntDlg.m_EditText2       = "";
    QueryIntDlg.m_EditText3       = "";
    QueryIntDlg.m_FlagText1       = "";
    QueryIntDlg.m_FlagText2       = "";
    QueryIntDlg.m_FlagText3       = "";
    QueryIntDlg.m_FlagText4       = "";
    QueryIntDlg.m_EditDefault1    = "";
    QueryIntDlg.m_EditDefault2    = "";
    QueryIntDlg.m_EditDefault3    = "";

    for(idx = 0; idx < NUM_INTERFACES; idx++)
    {           
        dRet = QueryIntDlg.m_CBContents1.Add( AllIIDs[idx].lpsz);
    }
    dRet = QueryIntDlg.m_CBContents1.Add("NULL");
    dRet = QueryIntDlg.m_CBContents1.Add("Bad Interface Param");

    // bring up modal dialog box, and if user hits OK, process operation
    if( QueryIntDlg.DoModal() == IDOK )
    {       
        lpInterface = NULL;
        for(idx = 0; idx < NUM_INTERFACES; idx++)
        {           
            if( !lstrcmp(QueryIntDlg.m_szCB1,AllIIDs[idx].lpsz) )
                lpInterface =  AllIIDs[idx].lpInterface;               
        }
        if(!lpInterface)
        {        
            if( !lstrcmp(QueryIntDlg.m_szCB1,"NULL") )
                lpInterface = (LPIID) NULL;
            else if(!lstrcmp(QueryIntDlg.m_szCB1,"Bad Interface Param") )
                lpInterface = (LPIID) &lpvNew;         // invalid
        }                     

        if( HR_FAILED( m_hResult = m_lpEntry->QueryInterface(*lpInterface, &lpvNew) ) )
        {
            MessageBox( E.SzError("m_lpEntry->QueryInterface()", m_hResult),
                            "Client", MBS_ERROR );
            return;
        }               
            
        wsprintf(szBuff,"%s",E.SzError("lpObj->QueryInterface() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );        

        lpNewProp   =  new CPropDlg("New Interface from QueryInterface",
                           (LPMAPIPROP *) &lpvNew,m_lpvDestObj,(HWND)this->m_hWnd );
    }

}


/*******************************************************************/
/*
 -  CPropDlg::
 -  CopyProps
 *
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnCopyProps()
{

    LPMAPIPROGRESS  lpProgress      = NULL;
    CResultsDlg     Results(this);
    COperation      CopyPropsDlg(this);
    CBldPropDlg     BldProp(this);
    LPSTR           lpszPropID      = NULL;
    char            szBuffer[512];
    CGetError       E;
    LPSPropProblemArray lpProblems  = NULL;
    LPSPropValue    lpPropValueBefore     = NULL;
    ULONG           ulValuesBefore        = 0;
    LPSPropValue    lpPropValueAfter      = NULL;
    ULONG           ulValuesAfter         = 0;
    char            szBuff[80];
    DWORD           dwIndex         = 0;
    HRESULT         hResult         = hrSuccess;
    int             dRet            = 0;
    ULONG           ulFlags         = 0;
    LPIID           lpInterface     = NULL;
    ULONG           ulUIParam       = 0;
    ULONG           idx;
    

    BldProp.m_fIsPropValueArray     = FALSE;
    BldProp.m_lpSetEntry    = m_lpEntry;
    BldProp.m_Operation     = "lpObj->CopyProps() Include PropTagArray";
    BldProp.DoModal();

    // initalize data for dialog box
    CopyPropsDlg.m_CurrentOperation= "lpObj->CopyProps()";
    CopyPropsDlg.m_CBText1         = "ulUIParam:";
    CopyPropsDlg.m_CBText3         = "lpInterface:";
    CopyPropsDlg.m_EditText1       = "lpvDestObj:";
    CopyPropsDlg.m_EditText2       = "";
    CopyPropsDlg.m_EditText3       = "";
    CopyPropsDlg.m_FlagText1       = "MAPI_MOVE";
    CopyPropsDlg.m_FlagText2       = "MAPI_NOREPLACE";
    CopyPropsDlg.m_FlagText3       = "MAPI_DIALOG";
    CopyPropsDlg.m_FlagText4       = "MAPI_DECLINE_OK";
    CopyPropsDlg.m_FlagText5       = "Invalid Flag";
    wsprintf(szBuffer,"0x%X",m_lpvDestObj);
    CopyPropsDlg.m_EditDefault1    = szBuffer;
    CopyPropsDlg.m_EditDefault2    = "";
    CopyPropsDlg.m_EditDefault3    = "";

    dRet = CopyPropsDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = CopyPropsDlg.m_CBContents1.Add(szBuff);

    dRet = CopyPropsDlg.m_CBContents3.Add("NULL");
    dRet = CopyPropsDlg.m_CBContents3.Add("Bad Interface Param");
    for(idx = 0; idx < NUM_INTERFACES; idx++)
    {           
        dRet = CopyPropsDlg.m_CBContents3.Add( AllIIDs[idx].lpsz);
    }


    // bring up modal dialog box, and if user hits OK, process operation
    if( CopyPropsDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( CopyPropsDlg.m_bFlag1 )
            ulFlags |= MAPI_MOVE;

        if( CopyPropsDlg.m_bFlag2)
            ulFlags |= MAPI_NOREPLACE;

        if( CopyPropsDlg.m_bFlag2)
            ulFlags |= MAPI_DIALOG;

        if( CopyPropsDlg.m_bFlag4)
            ulFlags |= MAPI_DECLINE_OK;

        if( CopyPropsDlg.m_bFlag5)
            ulFlags |= TEST_INVALID_FLAG;
                       
        if( !lstrcmp(CopyPropsDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;                    

        // determine interface of DestObj selected
        
        lpInterface = NULL;
        for(idx = 0; idx < NUM_INTERFACES; idx++)
        {           
            if( !lstrcmp(CopyPropsDlg.m_szCB3,AllIIDs[idx].lpsz) )
                lpInterface =  AllIIDs[idx].lpInterface;               
        }
        if(!lpInterface)
        {        
            if( !lstrcmp(CopyPropsDlg.m_szCB3,"NULL") )
                lpInterface = (LPIID) NULL;
            else if(!lstrcmp(CopyPropsDlg.m_szCB3,"Bad Interface Param") )
                lpInterface = (LPIID) &ulFlags;         // invalid
        }                     

        if( HR_FAILED( m_hResult = ((LPMAPIPROP)m_lpvDestObj)->GetProps(NULL, 0, &ulValuesBefore, &lpPropValueBefore) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnCopyProps()  lpEntry->GetProps() Before", m_hResult), "Client", MBS_ERROR );
            return;
        }

        if( m_hResult = m_lpEntry->CopyProps(BldProp.m_lpNewPTA,
                                            ulUIParam,
                                            lpProgress,
                                            lpInterface,
                                            m_lpvDestObj,
                                            ulFlags,
                                            &lpProblems) )
        {
            MessageBox( E.SzError("m_lpEntry->CopyProps()", m_hResult),
                            "Client", MBS_ERROR );
        }

    
        wsprintf(szBuff,"%s",E.SzError("lpObj->CopyProps() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

        if( HR_FAILED(m_hResult = ((LPMAPIPROP)m_lpvDestObj)->GetProps(NULL, 0, &ulValuesAfter, &lpPropValueAfter) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnCopyProps()  lpEntry->GetProps() After", m_hResult), "Client", MBS_ERROR );
            return;
        }

        Results.m_lpspvaBefore      = lpPropValueBefore;
        Results.m_cValuesBefore     = ulValuesBefore;    
        Results.m_lpspvaAfter       = lpPropValueAfter;
        Results.m_cValuesAfter      = ulValuesAfter;    
        Results.m_Operation         = "Properties Before/After lpvDestObj->CopyProps()";
        Results.m_lpProblems        = lpProblems;
        Results.m_fIsPropValueArray = FALSE;
        Results.m_lpsptaModify      = BldProp.m_lpNewPTA;

        Results.DoModal();
    }

    RedrawPropTable();
    
    if(BldProp.m_lpNewPTA)
    {
        PvFree(BldProp.m_lpNewPTA);
        BldProp.m_lpNewPTA = NULL;
    }
    
    if(lpPropValueBefore)
    {
        MAPIFreeBuffer(lpPropValueBefore);
        lpPropValueBefore = NULL;
    }

    if(lpPropValueAfter)
    {
        MAPIFreeBuffer(lpPropValueAfter);
        lpPropValueAfter = NULL;
    }
    if(lpProblems)
        MAPIFreeBuffer(lpProblems);
}



/*******************************************************************/
/*
 -  CPropDlg::
 -  OnCopyTo
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnCopyTo()
{

    LPMAPIPROGRESS  lpProgress      = NULL;
    CResultsDlg     Results(this);
    COperation      CopyToDlg(this);
    CBldPropDlg     BldProp(this);
    CInterfaceDlg   Interface(this);
    LPSTR           lpszPropID      = NULL;
    char            szBuffer[512];
    CGetError       E;
    LPSPropProblemArray lpProblems  = NULL;
    LPSPropValue    lpPropValueBefore     = NULL;
    ULONG           ulValuesBefore        = 0;
    LPSPropValue    lpPropValueAfter      = NULL;
    ULONG           ulValuesAfter         = 0;
    char            szBuff[80];
    DWORD           dwIndex         = 0;
    HRESULT         hResult         = hrSuccess;
    int             dRet            = 0;
    ULONG           ulFlags         = 0;
    LPIID           lpInterface     = NULL;
    ULONG           ulUIParam       = 0;
    ULONG           idx;
    

    BldProp.m_fIsPropValueArray     = FALSE;
    BldProp.m_lpSetEntry    = m_lpEntry;
    BldProp.m_Operation     = "lpObj->CopyTo() Exclude PropTagArray";
    BldProp.DoModal();

    Interface.DoModal();

    // initalize data for dialog box
    CopyToDlg.m_CurrentOperation= "lpObj->CopyTo()";
    CopyToDlg.m_CBText1         = "ulUIParam:";
    CopyToDlg.m_CBText3         = "lpInterface:";
    CopyToDlg.m_EditText1       = "lpvDestObj:";
    CopyToDlg.m_EditText2       = "";
    CopyToDlg.m_EditText3       = "";
    CopyToDlg.m_FlagText1       = "MAPI_MOVE";
    CopyToDlg.m_FlagText2       = "MAPI_NOREPLACE";
    CopyToDlg.m_FlagText3       = "MAPI_DIALOG";
    CopyToDlg.m_FlagText4       = "MAPI_DECLINE_OK";
    CopyToDlg.m_FlagText5       = "Invalid Flag";
    wsprintf(szBuffer,"0x%X",m_lpvDestObj);
    CopyToDlg.m_EditDefault1    = szBuffer;
    CopyToDlg.m_EditDefault2    = "";
    CopyToDlg.m_EditDefault3    = "";

    dRet = CopyToDlg.m_CBContents1.Add("NULL");
    wsprintf(szBuff,"Parent hWnd == %X",this->m_hWnd);
    dRet = CopyToDlg.m_CBContents1.Add(szBuff);

    for(idx = 0; idx < NUM_INTERFACES; idx++)
    {           
        dRet = CopyToDlg.m_CBContents3.Add( AllIIDs[idx].lpsz);
    }
    dRet = CopyToDlg.m_CBContents3.Add("NULL");
    dRet = CopyToDlg.m_CBContents3.Add("Bad Interface Param");


    // bring up modal dialog box, and if user hits OK, process operation
    if( CopyToDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( CopyToDlg.m_bFlag1 )
            ulFlags |= MAPI_MOVE;

        if( CopyToDlg.m_bFlag2)
            ulFlags |= MAPI_NOREPLACE;

        if( CopyToDlg.m_bFlag2)
            ulFlags |= MAPI_DIALOG;

        if( CopyToDlg.m_bFlag4)
            ulFlags |= MAPI_DECLINE_OK;

        if( CopyToDlg.m_bFlag5)
            ulFlags |= TEST_INVALID_FLAG;
                       
        if( !lstrcmp(CopyToDlg.m_szCB1,"NULL") )
            ulUIParam = (ULONG)NULL;
        else
            ulUIParam = (ULONG)(void *)this->m_hWnd;                    

        // determine interface of DestObj selected
        
        lpInterface = NULL;
        for(idx = 0; idx < NUM_INTERFACES; idx++)
        {           
            if( !lstrcmp(CopyToDlg.m_szCB3,AllIIDs[idx].lpsz) )
                lpInterface =  AllIIDs[idx].lpInterface;               
        }
        if(!lpInterface)
        {        
            if( !lstrcmp(CopyToDlg.m_szCB3,"NULL") )
                lpInterface = (LPIID) NULL;
            else if(!lstrcmp(CopyToDlg.m_szCB3,"Bad Interface Param") )
                lpInterface = (LPIID) &ulFlags;         // invalid
        }                     

        if( HR_FAILED( m_hResult = ((LPMAPIPROP)m_lpvDestObj)->GetProps(NULL, 0, &ulValuesBefore, &lpPropValueBefore) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnCopyTo()  lpEntry->GetProps() Before", m_hResult), "Client", MBS_ERROR );
            return;
        }

        if( m_hResult = m_lpEntry->CopyTo(  Interface.m_ciidSelect,
                                            (LPIID)Interface.m_rgiidSelect,
                                            BldProp.m_lpNewPTA,
                                            ulUIParam,
                                            lpProgress,
                                            lpInterface,
                                            m_lpvDestObj,
                                            ulFlags,
                                            &lpProblems) )
        {
            MessageBox( E.SzError("m_lpEntry->CopyTo()", m_hResult),
                            "Client", MBS_ERROR );
        }

    
        wsprintf(szBuff,"%s",E.SzError("lpObj->CopyTo() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

        if( HR_FAILED(m_hResult = ((LPMAPIPROP)m_lpvDestObj)->GetProps(NULL, 0, &ulValuesAfter, &lpPropValueAfter) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnCopyTo()  lpEntry->GetProps() After", m_hResult), "Client", MBS_ERROR );
            return;
        }

        Results.m_lpspvaBefore  = lpPropValueBefore;
        Results.m_cValuesBefore = ulValuesBefore;    
        Results.m_lpspvaAfter   = lpPropValueAfter;
        Results.m_cValuesAfter  = ulValuesAfter;    
        Results.m_Operation     = "Properties Before/After lpvDestObj->CopyTo()";
        Results.m_lpProblems    = lpProblems;
        Results.m_fIsPropValueArray = FALSE;
        Results.m_lpsptaModify  = BldProp.m_lpNewPTA;

        Results.DoModal();
    }

    RedrawPropTable();
    
    if(BldProp.m_lpNewPTA)
    {
        PvFree(BldProp.m_lpNewPTA);
        BldProp.m_lpNewPTA = NULL;
    }
    
    if(lpPropValueBefore)
    {
        MAPIFreeBuffer(lpPropValueBefore);
        lpPropValueBefore = NULL;
    }

    if(lpPropValueAfter)
    {
        MAPIFreeBuffer(lpPropValueAfter);
        lpPropValueAfter = NULL;
    }
    if(lpProblems)
        MAPIFreeBuffer(lpProblems);

}

    

/*******************************************************************/
/*
 -  CPropDlg::
 -  OnAddRef
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnAddRef()
{
    DWORD   dwIndex    = 0;
    ULONG   ulRefCount = 0;
    char    szBuff[80];

    ulRefCount = m_lpEntry->AddRef();

    // keep reference so user never releases to refcount == 0
    m_ulRefCount++;

    wsprintf(szBuff,"lpObj->AddRef() \tRC: %lu, Current RefCount >= %lu",ulRefCount,m_ulRefCount);
    dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );
}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnSetProps
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnSetProps()
{
    CResultsDlg     Results(this);
    CBldPropDlg     BldProp(this);
    LPSTR           lpszPropID      = NULL;
    LONG            lSelectedTag    = 0;
    CGetError       E;
    LPSPropProblemArray lpProblems  = NULL;
    LPSPropValue    lpPropValueBefore     = NULL;
    ULONG           ulValuesBefore        = 0;
    LPSPropValue    lpPropValueAfter      = NULL;
    ULONG           ulValuesAfter         = 0;
    char            szBuff[80];
    DWORD           dwIndex         = 0;

    // need to set m_lSelectTag in case it is not one of our "MAPI" tags
    //  and it is stored as a hex value so that setprops can
    //  represent this value correctly
    if( HR_FAILED( m_hResult = m_lpEntry->GetProps(NULL, 0, &ulValuesBefore, &lpPropValueBefore) ) )
    {
        MessageBox( E.SzError("CPropDlg::OnSetProps()  lpEntry->GetProps() Before", m_hResult), "Client", MBS_ERROR );
        return;
    }

    BldProp.m_fIsPropValueArray     = TRUE;
    BldProp.m_lpSetEntry    = m_lpEntry;
    BldProp.m_Operation     = "lpObj->SetProps()";
    BldProp.DoModal();

    if(BldProp.m_fCall)
    {
        if(BldProp.m_ulNewValues)
        {
            if( m_hResult = m_lpEntry->SetProps(BldProp.m_ulNewValues, BldProp.m_lpspvaSelected, &lpProblems) )
            {
                MessageBox( E.SzError("CPropDlg::OnSetProps()", m_hResult),
                                    "Client", MBS_ERROR );
            }

            wsprintf(szBuff,"%s",E.SzError("lpObj->SetProps() \tRC: ", m_hResult));
            dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );
        }

        if( HR_FAILED(m_hResult = m_lpEntry->GetProps(NULL, 0, &ulValuesAfter, &lpPropValueAfter) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnSetProps()  lpEntry->GetProps() After", m_hResult), "Client", MBS_ERROR );
            return;
        }


        Results.m_lpspvaBefore  = lpPropValueBefore;
        Results.m_cValuesBefore = ulValuesBefore;    
        Results.m_lpspvaAfter   = lpPropValueAfter;
        Results.m_cValuesAfter  = ulValuesAfter;    
        Results.m_Operation     = "Properties Before/After lpObj->SetProps()";
        Results.m_lpProblems    = lpProblems;
        Results.m_fIsPropValueArray = TRUE;
        Results.m_lpspvaModify  = BldProp.m_lpspvaSelected;
        Results.m_cValuesModify = BldProp.m_ulNewValues;

        Results.DoModal();
    }
    RedrawPropTable();
    
    if(lpPropValueBefore)
    {
        MAPIFreeBuffer(lpPropValueBefore);
        lpPropValueBefore = NULL;
    }

    if(lpPropValueAfter)
    {
        MAPIFreeBuffer(lpPropValueAfter);
        lpPropValueAfter = NULL;
    }
    
    if(lpProblems)
    {
        MAPIFreeBuffer(lpProblems);
        lpProblems = NULL;
    }

}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnDeleteProps
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnDeleteProps()
{
    CResultsDlg     Results(this);
    CBldPropDlg     BldProp(this);
    LPSTR           lpszPropID      = NULL;
    CGetError       E;
    LPSPropProblemArray lpProblems  = NULL;
    LPSPropValue    lpPropValueBefore     = NULL;
    ULONG           ulValuesBefore        = 0;
    LPSPropValue    lpPropValueAfter      = NULL;
    ULONG           ulValuesAfter         = 0;
    char            szBuff[80];
    DWORD           dwIndex         = 0;


    //$ ATTENTION this is dependent on order of properties in listbox
    //      DONT SORT PROPS IN LISTBOX

    // need to set m_lSelectTag in case it is not one of our "MAPI" tags
    //  and it is stored as a hex value so that setprops can
    //  represent this value correctly

    if( HR_FAILED( m_hResult = m_lpEntry->GetProps(NULL, 0, &ulValuesBefore, &lpPropValueBefore) ) )
    {
        MessageBox( E.SzError("CPropDlg::OnDeleteProps()  lpEntry->GetProps()", m_hResult), "Client", MBS_ERROR );
        return;
    }
    BldProp.m_fIsPropValueArray     = FALSE;
    BldProp.m_lpSetEntry    = m_lpEntry;
    BldProp.m_Operation     = "lpObj->DeleteProps()";
    BldProp.DoModal();

    if(BldProp.m_fCall)
    {
        if( HR_FAILED( m_hResult = m_lpEntry->DeleteProps(BldProp.m_lpNewPTA, &lpProblems) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnDeleteProps()", m_hResult),
                                "Client", MBS_ERROR );
        }
        wsprintf(szBuff,"%s",E.SzError("lpObj->DeleteProps() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );


        if( HR_FAILED( m_hResult = m_lpEntry->GetProps(NULL, 0, &ulValuesAfter, &lpPropValueAfter) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnDeleteProps()  lpEntry->GetProps() After", m_hResult), "Client", MBS_ERROR );
            return;
        }

        Results.m_lpspvaBefore  = lpPropValueBefore;
        Results.m_cValuesBefore = ulValuesBefore;    
        Results.m_lpspvaAfter   = lpPropValueAfter;
        Results.m_cValuesAfter  = ulValuesAfter;    
        Results.m_Operation     = "lpObj->DeleteProps()";
        Results.m_lpProblems    = lpProblems;
        Results.m_fIsPropValueArray = FALSE;
        Results.m_lpsptaModify  = BldProp.m_lpNewPTA;

        Results.DoModal();

    }

    RedrawPropTable();

    if(BldProp.m_lpNewPTA)
    {
        PvFree(BldProp.m_lpNewPTA);
        BldProp.m_lpNewPTA = NULL;
    }
    
    if(lpPropValueBefore)
    {
        MAPIFreeBuffer(lpPropValueBefore);
        lpPropValueBefore = NULL;
    }

    if(lpPropValueAfter)
    {
        MAPIFreeBuffer(lpPropValueAfter);
        lpPropValueAfter = NULL;
    }
    if(lpProblems)
        MAPIFreeBuffer(lpProblems);
        
}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnGetProps
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnGetProps()
{
    CBldPropDlg     BldProp(this);
    CGetPropDlg     GetProp(this);
    LPSTR           lpszPropID      = NULL;
    CGetError       E;
    LPSPropProblemArray lpProblems  = NULL;
    LPSPropValue    lpPropValue     = NULL;
    ULONG           ulValues        = 0;
    DWORD           dwIndex         = 0;
    char            szBuff[80];

    BldProp.m_fIsPropValueArray     = FALSE;
    BldProp.m_lpSetEntry    = m_lpEntry;
    BldProp.m_Operation     = "lpObj->GetProps()";
    BldProp.DoModal();

    if(BldProp.m_fCall)
    {
        if( HR_FAILED(m_hResult = m_lpEntry->GetProps(BldProp.m_lpNewPTA, 0, &ulValues, &lpPropValue) ) )
        {
            MessageBox( E.SzError("CPropDlg::OnGetProps()  lpEntry->GetProps()",
                    m_hResult), "Client", MBS_ERROR );
        }
        wsprintf(szBuff,"%s",E.SzError("lpObj->GetProps() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );


        GetProp.m_lpPTA     = BldProp.m_lpNewPTA;
        GetProp.m_lpPVA     = lpPropValue;
        GetProp.m_cValues   = ulValues;

        GetProp.DoModal();

        if(BldProp.m_lpNewPTA)
        {
            PvFree(BldProp.m_lpNewPTA);
            BldProp.m_lpNewPTA = NULL;
        }

        if(lpPropValue)
        {
            MAPIFreeBuffer(lpPropValue);
            lpPropValue = NULL;
            ulValues     = 0;
        }
    }
}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnGetPropList
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnGetPropList()
{
    CGetPropListDlg GetPropList(this);
    LPSPropTagArray lpPTA       = NULL;
    CGetError       E;
    char            szBuff[80];
    DWORD           dwIndex     = 0;

    if( HR_FAILED(m_hResult = m_lpEntry->GetPropList(0, &lpPTA) ) )
    {
        MessageBox( E.SzError("CPropDlg::OnBldProps()  lpEntry->GetPropList()",
            m_hResult), "Client", MBS_ERROR );
        return;
    }

    GetPropList.m_lpPropTagArray    = lpPTA;
    GetPropList.DoModal();

    wsprintf(szBuff,"%s",E.SzError("lpObj->GetPropList() \tRC: ", m_hResult));
    dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnRelease
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnRelease()
{
    DWORD   dwIndex    = 0;
    ULONG   ulRefCount = 0;
    char    szBuff[80];


    // cancel out of dialog if we no longer have a valid object
    if(m_ulRefCount == 1 )
    {
        MessageBox( "Current m_ulRefCount == 1, PropDlg will not allow you to Release",
                 "Warning",MB_OK | MB_ICONINFORMATION);
        wsprintf(szBuff,"m_ulRefCount == 1, Invalid UI option to Release Object Completely");

        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );
        return;
    }

    ulRefCount = m_lpEntry->Release();

    // keep reference so user never releases to refcount == 0
    m_ulRefCount--;

    wsprintf(szBuff,"lpObj->Release() \tRC: %lu, Current RefCount >= %lu",ulRefCount,m_ulRefCount);
    dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

}

/*******************************************************************/
/*
 -  CPropDlg::
 -  OnGetLastError
 -
 *  Purpose:
 */
/*******************************************************************/

void CPropDlg::OnGetLastError()
{

//$ FUTURE use Operation Dialog for this too
//$ FUTURE Support Unicode
//$ FUTURE, pass in hResult or allow it on all other dialogs as well

    CGetError       E;
    DWORD           dwIndex         = 0;
    LPMAPIERROR     lpMAPIError     = NULL;
    HRESULT         hResult         = hrSuccess;
    char            szBuff[120];


    if( hResult = m_lpEntry->GetLastError(
                    m_hResult,
                    0,
                    &lpMAPIError))
    {
        MessageBox( E.SzError("CPropDlg::OnGetLastError()  lpEntry->GetLastError()", hResult),
                 "Client", MBS_ERROR );
        return;

    }

    if(lpMAPIError)
    {
        wsprintf(szBuff,"lpObj->GetLastError()  \tulLowLevelError == %lu,ulVersion == %lu lpszMessage == %s, lpszComponent == %s, ulContext == %lu",
                lpMAPIError->ulLowLevelError,
                lpMAPIError->ulVersion,
                ((lpMAPIError->lpszError == NULL)     ? "NULL" : lpMAPIError->lpszError),
                ((lpMAPIError->lpszComponent == NULL) ? "NULL" : lpMAPIError->lpszComponent),
                lpMAPIError->ulContext);
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );
    }
    
    if(lpMAPIError)
        MAPIFreeBuffer(lpMAPIError);
}



/*******************************************************************/
/*
 -  CPropDlg::
 -  OnSaveChanges
 *
 *  Purpose:
 *      SaveChanges on Property object
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CPropDlg::OnSaveChanges()
{
    COperation      SaveChangesDlg(this);
    CGetError       E;
    CString         OperName;
    int             dRet            = 0;
    ULONG           ulFlags         = 0;
    char            szBuff[80];
    DWORD           dwIndex         = 0;

    // initalize data for dialog box

    OperName = "lpObj->SaveChanges()";

    SaveChangesDlg.m_CurrentOperation= OperName;

    SaveChangesDlg.m_FlagText1       = "KEEP_OPEN_READONLY";
    SaveChangesDlg.m_FlagText2       = "KEEP_OPEN_READWRITE";
    SaveChangesDlg.m_FlagText3       = "FORCE_SAVE";
    SaveChangesDlg.m_FlagText4       = "Invalid Flag";

    // bring up modal dialog box, and if user hits OK, process operation
    if( SaveChangesDlg.DoModal() == IDOK )
    {
        if( SaveChangesDlg.m_bFlag1 )
            ulFlags |= KEEP_OPEN_READONLY;

        if( SaveChangesDlg.m_bFlag2)
            ulFlags |= KEEP_OPEN_READWRITE;

        if( SaveChangesDlg.m_bFlag3)
            ulFlags |= FORCE_SAVE;

        if( SaveChangesDlg.m_bFlag4)
            ulFlags |= TEST_INVALID_FLAG;

        if( m_hResult = m_lpEntry->SaveChanges(ulFlags) )
        {
            MessageBox( E.SzError("m_lpEntry->SaveChanges()", m_hResult),
                            "Client", MBS_ERROR );
        }
        wsprintf(szBuff,"%s",E.SzError("lpObj->SaveChanges() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

        RedrawPropTable();
    }

}

/*******************************************************************/
/*
 -  CPropDlg::
 -  OnGetNamesFromIDs
 -
 *  Purpose:
 *
 *
 */
/*******************************************************************/

void  CPropDlg::OnGetNamesFromIDs()
{
    CNamesIDsDlg    DisplayNameIDs(this);
    COperation      NameIDsDlg(this);
    CBldPropDlg     BldProp(this);
    int             dRet            = 0;
    CGetError       E;
    LPIID           lpInterface     = NULL;
    ULONG           idx;
    DWORD           dwIndex         = 0;
    ULONG           ulFlags         = 0;
    HRESULT         hResult         = hrSuccess;
    char            szBuff[80];
    char            szTemp[30];
    ULONG           cPropNames      = 0;          
    LPMAPINAMEID FAR  *lppPropNames = NULL; 
    BOOL            fFreeMAPI       = FALSE;
    GUID            guid;
   
    // build up proptag array to select specific property tags in name/id mapping
    BldProp.m_fIsPropValueArray     = FALSE;
    BldProp.m_lpSetEntry            = m_lpEntry;
    BldProp.m_Operation             = "lpObj->GetNamesFromIDs() Select Tags(None == NULL) ";
    BldProp.DoModal();

    // initalize data for dialog box
    NameIDsDlg.m_CurrentOperation= "lpObj->GetNamesFromIDs()";
    NameIDsDlg.m_CBText1         = "lpInterface:";
    NameIDsDlg.m_EditText1       = "lpsPropTagArray:";
    NameIDsDlg.m_FlagText1       = "MAPI_NO_STRINGS";
    NameIDsDlg.m_FlagText2       = "MAPI_NO_IDS";
    NameIDsDlg.m_FlagText3       = "Invalid Flag";

    
    // if this is NULL, we will have to free with MAPIFreeBuffer and we should add
    // the null param to NameIDsDlg, else add m_lpNewPTA value to NameIDsDlg
    if(! BldProp.m_lpNewPTA)
    {
        NameIDsDlg.m_EditDefault1    = "NULL";
        fFreeMAPI = TRUE;                
    }
    else
    {
        wsprintf(szTemp,"0x%08X",BldProp.m_lpNewPTA);
        NameIDsDlg.m_EditDefault1    = szTemp;
        fFreeMAPI = FALSE;
    }

    dRet = NameIDsDlg.m_CBContents1.Add("NULL");
    dRet = NameIDsDlg.m_CBContents1.Add("TEST GUID");
    dRet = NameIDsDlg.m_CBContents1.Add("Bad Interface Param");

    for(idx = 0; idx < NUM_INTERFACES; idx++)
    {           
        dRet = NameIDsDlg.m_CBContents1.Add( AllIIDs[idx].lpsz);
    }

    // bring up modal dialog box, and if user hits OK, process operation
    if( NameIDsDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( NameIDsDlg.m_bFlag1 )
            ulFlags |= MAPI_NO_STRINGS;

        if( NameIDsDlg.m_bFlag2 )
            ulFlags |= MAPI_NO_IDS;

        if( NameIDsDlg.m_bFlag3 )
            ulFlags |= TEST_INVALID_FLAG;

        // determine interface of PropSet selected
        lpInterface = NULL;
        for(idx = 0; idx < NUM_INTERFACES; idx++)
        {           
            if( !lstrcmp(NameIDsDlg.m_szCB1,AllIIDs[idx].lpsz) )
                lpInterface =  AllIIDs[idx].lpInterface;               
        }
        if(!lpInterface)
        {        
            if( !lstrcmp(NameIDsDlg.m_szCB1,"NULL") )
                lpInterface = (LPIID) NULL;
            else if(!lstrcmp(NameIDsDlg.m_szCB1,"TEST GUID") )
            {
                MakeGuid(   &guid, 
                            (ULONG) 0x000000F0, 
                            (WORD)  0x0004,
                            (WORD)  0x0033,
                            (BYTE)  0x00,
                            (BYTE)  0x01,
                            (BYTE)  0x00,
                            (BYTE)  0x02,
                            (BYTE)  0x00,
                            (BYTE)  0x03,
                            (BYTE)  0x00,
                            (BYTE)  0x04);
                lpInterface = (LPIID) &guid;                            
            }
            else if(!lstrcmp(NameIDsDlg.m_szCB1,"Bad Interface Param") )
                lpInterface = (LPIID) &ulFlags;         // invalid
        }                     

        if( m_hResult = m_lpEntry->GetNamesFromIDs(                                           
                                            &BldProp.m_lpNewPTA,
                                            lpInterface,
                                            ulFlags,
                                            &cPropNames,
                                            &lppPropNames) )
        {
            MessageBox( E.SzError("m_lpEntry->GetNamesFromIDs()", m_hResult),
                            "Client", MBS_ERROR );
            goto Error;
        }
        wsprintf(szBuff,"%s",E.SzError("lpObj->GetNamesFromIDs() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

        DisplayNameIDs.m_lpPTA          = BldProp.m_lpNewPTA;
        DisplayNameIDs.m_lpPropSetGuid  = lpInterface;
        DisplayNameIDs.m_ulPropNames    = cPropNames;
        DisplayNameIDs.m_lppMAPINameID  = lppPropNames;

        DisplayNameIDs.DoModal();        
    }

    RedrawPropTable();

Error:

    if(fFreeMAPI)
    {
        if(BldProp.m_lpNewPTA)
        {
            MAPIFreeBuffer(BldProp.m_lpNewPTA);
            BldProp.m_lpNewPTA = NULL;
        }
    }    
    else
    {
        if(BldProp.m_lpNewPTA)
        {
            PvFree(BldProp.m_lpNewPTA);
            BldProp.m_lpNewPTA = NULL;
        }
    }

    if(lppPropNames)
        MAPIFreeBuffer(lppPropNames);
}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnGetIDsFromNames
 -
 *  Purpose:
 *
 *
 */
/*******************************************************************/

void  CPropDlg::OnGetIDsFromNames()
{
    CNamesIDsDlg    DisplayNameIDs(this);
    COperation      GetIDsFromNamesDlg(this);
    LPIID           lpInterface         = NULL;
    CGetError       E;
    ULONG           idx;
    char            szBuff[80];
    int             dRet                = 0;
    ULONG           ulFlags             = 0;
    DWORD           dwIndex             = 0;
    ULONG           cPropNames          = 0;          
    LPMAPINAMEID    *lppPropNames       = NULL; 
    ULONG           ulUnionType         = 0;
    ULONG           len1                = 0;
    ULONG           len2                = 0;
    ULONG           len3                = 0;
    ULONG           len4                = 0;
    ULONG           ulID                = 0;
    LPSPropTagArray lpPropTagArray      = NULL;
    GUID            guid;

    // initalize data for dialog box
    GetIDsFromNamesDlg.m_CurrentOperation= "lpObj->GetIDsFromNames()";
    GetIDsFromNamesDlg.m_CBText1         = "lpInterface:";
    GetIDsFromNamesDlg.m_CBText2         = "Union Type:";
    GetIDsFromNamesDlg.m_EditText1       = "Name/ID 1";
    GetIDsFromNamesDlg.m_EditText2       = "Name/ID 2";
    GetIDsFromNamesDlg.m_EditText3       = "Name/ID 3";
    GetIDsFromNamesDlg.m_FlagText1       = "MAPI_CREATE";
    GetIDsFromNamesDlg.m_FlagText2       = "Invalid Flag";
    GetIDsFromNamesDlg.m_EditDefault1    = "MAPI Name/ID 1";
    GetIDsFromNamesDlg.m_EditDefault2    = "MAPI Name/ID 2";
    GetIDsFromNamesDlg.m_EditDefault3    = "MAPI Name/ID 3";


    dRet = GetIDsFromNamesDlg.m_CBContents2.Add("MNID_ID");
    dRet = GetIDsFromNamesDlg.m_CBContents2.Add("MNID_STRING");

    dRet = GetIDsFromNamesDlg.m_CBContents1.Add("NULL");
    dRet = GetIDsFromNamesDlg.m_CBContents1.Add("TEST GUID");
    dRet = GetIDsFromNamesDlg.m_CBContents1.Add("Bad Interface Param");
    for(idx = 0; idx < NUM_INTERFACES; idx++)
    {           
        dRet = GetIDsFromNamesDlg.m_CBContents1.Add( AllIIDs[idx].lpsz);
    }

    // bring up modal dialog box, and if user hits OK, process operation
    if( GetIDsFromNamesDlg.DoModal() == IDOK )
    {
        // determine state/settings of data in dialog upon closing
        if( GetIDsFromNamesDlg.m_bFlag1 )
            ulFlags |= MAPI_CREATE;

        if( GetIDsFromNamesDlg.m_bFlag2 )
            ulFlags |= TEST_INVALID_FLAG;

        // determine interface of PropSet selected
        lpInterface = NULL;
        for(idx = 0; idx < NUM_INTERFACES; idx++)
        {           
            if( !lstrcmp(GetIDsFromNamesDlg.m_szCB1,AllIIDs[idx].lpsz) )
                lpInterface =  AllIIDs[idx].lpInterface;               
        }
        if(!lpInterface)
        {        
            if( !lstrcmp(GetIDsFromNamesDlg.m_szCB1,"NULL") )
                lpInterface = (LPIID) NULL;
            else if(!lstrcmp(GetIDsFromNamesDlg.m_szCB1,"TEST GUID") )
            {
                MakeGuid(   &guid, 
                            (ULONG) 0x000000F0, 
                            (WORD)  0x0004,
                            (WORD)  0x0033,
                            (BYTE)  0x00,
                            (BYTE)  0x01,
                            (BYTE)  0x00,
                            (BYTE)  0x02,
                            (BYTE)  0x00,
                            (BYTE)  0x03,
                            (BYTE)  0x00,
                            (BYTE)  0x04);
                lpInterface = (LPIID) &guid;                            
            }
            else if(!lstrcmp(GetIDsFromNamesDlg.m_szCB1,"Bad Interface Param") )
                lpInterface = (LPIID) &ulFlags;         // invalid
        }                     

        // determine UNION type from CB 1
        if( !lstrcmp(GetIDsFromNamesDlg.m_szCB2,"MNID_ID") )
            ulUnionType = MNID_ID;
        else if(!lstrcmp(GetIDsFromNamesDlg.m_szCB2,"MNID_STRING") )
            ulUnionType = MNID_STRING;
        else
            goto Error;

        lppPropNames = (LPMAPINAMEID *) PvAlloc(3 * sizeof(LPMAPINAMEID));
        lppPropNames[0] = (LPMAPINAMEID) PvAllocMore(sizeof(MAPINAMEID),lppPropNames);
        lppPropNames[1] = (LPMAPINAMEID) PvAllocMore(sizeof(MAPINAMEID),lppPropNames);
        lppPropNames[2] = (LPMAPINAMEID) PvAllocMore(sizeof(MAPINAMEID),lppPropNames);
        
        // read data from Edit control 1, 2, and 3  depending on union type
        // and build up a LPMAPINAMEID structure
        if(ulUnionType == MNID_ID )
        {
            // find out the number of IDs to map == 0 or 1 or 2 or 3
            // and initialized them with ulKind, lpInterface, and ID data

            cPropNames = 0;
            if( len1 = strlen(GetIDsFromNamesDlg.m_szEdit1) )
            {
                len4 = 0;
                (*lppPropNames[cPropNames]).lpguid = (LPIID) lpInterface;
                (*lppPropNames[cPropNames]).ulKind = ulUnionType;
                len4 = lstrlen(GetIDsFromNamesDlg.m_szEdit1);
                AsciiToHex(len4,GetIDsFromNamesDlg.m_szEdit1,&ulID);
                (*lppPropNames[cPropNames]).Kind.lID = ulID;
                cPropNames++;
            }
            if( len2 = strlen(GetIDsFromNamesDlg.m_szEdit2) )
            {
                len4 = 0;
                (*lppPropNames[cPropNames]).lpguid = (LPIID) lpInterface;
                (*lppPropNames[cPropNames]).ulKind = ulUnionType;
                len4 = lstrlen(GetIDsFromNamesDlg.m_szEdit2);
                AsciiToHex(len4,GetIDsFromNamesDlg.m_szEdit2,&ulID);
                (*lppPropNames[cPropNames]).Kind.lID = ulID;
                cPropNames++;
            }
            if( len3 = strlen(GetIDsFromNamesDlg.m_szEdit3) )
            {
                len4 = 0;
                (*lppPropNames[cPropNames]).lpguid = (LPIID) lpInterface;
                (*lppPropNames[cPropNames]).ulKind = ulUnionType;
                len4 = lstrlen(GetIDsFromNamesDlg.m_szEdit3);
                AsciiToHex(len4,GetIDsFromNamesDlg.m_szEdit3,&ulID);
                (*lppPropNames[cPropNames]).Kind.lID = ulID;
                cPropNames++;
            }
        }
        else // MNID_STRING
        {
            // find out the number of IDs to map == 0 or 1 or 2 or 3
            // and initialized them with ulKind, lpInterface, and String data

            cPropNames = 0;
            if( len1 = strlen(GetIDsFromNamesDlg.m_szEdit1) )
            {
                (*lppPropNames[cPropNames]).lpguid = (LPIID) lpInterface;
                (*lppPropNames[cPropNames]).ulKind = ulUnionType;
                String8ToUnicode(   GetIDsFromNamesDlg.m_szEdit1,
                                    &((*lppPropNames[cPropNames]).Kind.lpwstrName),
                                    lppPropNames);
                cPropNames++;
            }
            if( len2 = strlen(GetIDsFromNamesDlg.m_szEdit2) )
            {
                (*lppPropNames[cPropNames]).lpguid = (LPIID) lpInterface;
                (*lppPropNames[cPropNames]).ulKind = ulUnionType;
                String8ToUnicode(   GetIDsFromNamesDlg.m_szEdit2,
                                    &((*lppPropNames[cPropNames]).Kind.lpwstrName),
                                    lppPropNames);
                cPropNames++;
            }
            if( len3 = strlen(GetIDsFromNamesDlg.m_szEdit3) )
            {
                (*lppPropNames[cPropNames]).lpguid = (LPIID) lpInterface;
                (*lppPropNames[cPropNames]).ulKind = ulUnionType;
                String8ToUnicode(   GetIDsFromNamesDlg.m_szEdit3,
                                    &((*lppPropNames[cPropNames]).Kind.lpwstrName),
                                    lppPropNames);
                cPropNames++;
            }
        }

        if( m_hResult = m_lpEntry->GetIDsFromNames(                                           
                                            cPropNames,
                                            lppPropNames,
                                            ulFlags,
                                            &lpPropTagArray) )
        {
            MessageBox( E.SzError("m_lpEntry->GetIDsFromNames()", m_hResult),
                            "Client", MBS_ERROR );
            goto Error;
        }

        wsprintf(szBuff,"%s",E.SzError("lpObj->GetIDsFromNames() \tRC: ", m_hResult));
        dwIndex = SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,
                        (LPARAM)szBuff );

        DisplayNameIDs.m_lpPTA          = lpPropTagArray;
        DisplayNameIDs.m_lpPropSetGuid  = lpInterface;
        DisplayNameIDs.m_ulPropNames    = cPropNames;
        DisplayNameIDs.m_lppMAPINameID  = lppPropNames;

        DisplayNameIDs.DoModal();        

    }

    RedrawPropTable();

Error:

    if(lppPropNames)
        PvFree(lppPropNames);

    if(lpPropTagArray)
        MAPIFreeBuffer(lpPropTagArray);
}


/*******************************************************************/
/*
 -  CPropDlg::
 -  OnCancel
 -
 *  Purpose:
 *      Closes the Folder dialog.
 *
 */
/*******************************************************************/

void CPropDlg::OnCancel()
{
    delete this;
}

/*******************************************************************/
/*
 -  CPropDlg::
 -  Cleanup()
 -
 *  Purpose:
 *      Destructor for class CFolderDlg and CPropDlg call this
 *      function that releases and Frees memory
 *      allocated in class
 *
 */
/*******************************************************************/

void CPropDlg::Cleanup()
{

// do nothing now
}

/*
 -  CPropDlg
 -  OnClearMapiLog
 -
 *  Purpose:
 *      Clears the logging window.
 *
 */

void CPropDlg::OnClearMapiLog()
{
    DWORD   dwIndex = 0;

    dwIndex = SendDlgItemMessage( IDC_MAPILOG, LB_RESETCONTENT, 0, 0L);

}

/*******************************************************************/
/*
 -  CPropDlg::
 -
 -  OnType
 -  OnData
 -  OnIDs
 -  OnString
 -  OnHex
 -  OnDecimal
 -
 *  Purpose:
 *      To determine whether to display different qualities (types,values,data)
 *      and the method of displaying(HEX, Decimal, String) properties
 *      of the mapi object in the property listbox on the dialog when
 *      RedrawPropTable is called
 *
 */
/*******************************************************************/

void CPropDlg::OnType()
{
    CheckDlgButton(IDC_PROPTYPE, !IsDlgButtonChecked(IDC_PROPTYPE) );
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnData()
{
    CheckDlgButton(IDC_PROPDATA, !IsDlgButtonChecked(IDC_PROPDATA) );
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnID()
{
    CheckDlgButton(IDC_PROPID, !IsDlgButtonChecked(IDC_PROPID) );
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnString()
{
    CheckRadioButton(IDC_PROPSTRING,IDC_PROPDEC,IDC_PROPSTRING);
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnHex()
{

    CheckRadioButton(IDC_PROPSTRING,IDC_PROPDEC,IDC_PROPHEX);
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnDecimal()
{
    CheckRadioButton(IDC_PROPSTRING,IDC_PROPDEC,IDC_PROPDEC);
    RedrawPropTable();
}

/********************************************************************/
/*
 -  CPropDlg::
 -  RedrawPropTable
 -
 *  Purpose:
 *      To redraw the current property table of folder listbox.  This
 *      routine gets checkstate of property display checkboxs and
 *      radio buttons to determine how to display and what to display
 *
 *      Assuming the size(in bytes) of the data, types and IDs in
 *      array size.
 */
/********************************************************************/

void CPropDlg::RedrawPropTable()
{
    HRESULT         hResult         = SUCCESS_SUCCESS;
    CGetError       E;
    char            szBuffer[1024];
    int             idx             = 0;
    DWORD           dwReturn        = 0;
    LPSPropValue    lpPropValue     = NULL;
    ULONG           ulValues        = 0;
    char            szCVals[30];
                            // assuming size here
    char szID[50];
    char szData[512];
    char szType[32];      // Assumes no PropType string longer than 31 chars

    // multi value props stuff
    char            szMV[1024];    
    char            szMVSeps[]      = ";:.";
    char            *lpszToken      = NULL;
    char            szTemp[30];
    ULONG           ulMVRow         = 0;
    ULONG           ulMVcValues     = 0;
    char            *szEnd          = NULL;



    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_PROPVIEW,LB_RESETCONTENT,0,0);

    if( m_hResult = m_lpEntry->GetProps(NULL, 0, &ulValues, &lpPropValue) )
    {
        MessageBox( E.SzError("CPropDlg::RedrawPropTable() lpEntry->GetProps()", m_hResult),
                     "Client", MBS_ERROR );
        // allow warning messages, dont goto error here

        // ASSERT THAT there is at least one prop in listbox
        if(ulValues == 0)
        {
            MessageBox( "lpObj->GetProps(NULL) returned ulValues == 0, exiting",
                     "Client", MBS_ERROR );
            goto End;
        }
    }
    
    // DISPLAY Expected lpPTA->cValues
    wsprintf(szCVals,"cValues:  %lu",ulValues);
    SetDlgItemText(IDT_CVALUES,szCVals);
    
    // determine if string, hex, or decimal and build row of listbox
    for(idx = 0; idx < ulValues; idx++)
    {
        szID[0]     = '\0' ;
        szData[0]   = '\0' ;
        szType[0]   = '\0' ;
        szBuffer[0] = '\0' ;

        if( IsDlgButtonChecked(IDC_PROPID) )
        {
            if( IsDlgButtonChecked(IDC_PROPSTRING) )
            {
                if(GetString("PropIDs", PROP_ID(lpPropValue[idx].ulPropTag), szID ) )
                {
                    lstrcat(szBuffer, szID );
                    lstrcat(szBuffer, "\t");
                }
                else
                {
                    wsprintf(szBuffer,"%#04X\t", PROP_ID(lpPropValue[idx].ulPropTag) );
                }
            }
            else if(  IsDlgButtonChecked(IDC_PROPDEC) )
            {
                wsprintf(szBuffer,"%08d\t", PROP_ID(lpPropValue[idx].ulPropTag), szID );
            }
            else if(  IsDlgButtonChecked(IDC_PROPHEX) )
            {
                wsprintf(szBuffer,"%#04X\t", PROP_ID(lpPropValue[idx].ulPropTag) );
            }
        }

        if( IsDlgButtonChecked(IDC_PROPTYPE) )
        {
            if( IsDlgButtonChecked(IDC_PROPSTRING) )
            {
                if( GetString("PropType", PROP_TYPE(lpPropValue[idx].ulPropTag), szType) )
                {
                    lstrcat(szBuffer, szType);
                    lstrcat(szBuffer,"\t");
                }
                else
                {
                    wsprintf(szType,"%#04X\t", PROP_TYPE(lpPropValue[idx].ulPropTag) );
                    lstrcat(szBuffer,szType);

                }
            }
            else if(  IsDlgButtonChecked(IDC_PROPDEC) )
            {
                wsprintf(szType,"%08d\t", PROP_TYPE(lpPropValue[idx].ulPropTag) );
                lstrcat(szBuffer,szType);
            }
            else if(  IsDlgButtonChecked(IDC_PROPHEX) )
            {
                wsprintf(szType,"%#04X\t", PROP_TYPE(lpPropValue[idx].ulPropTag) );
                lstrcat(szBuffer,szType);
            }
        }

                    
        if( IsDlgButtonChecked(IDC_PROPDATA) )  
        {   
            SzGetPropValue(szData,(LPSPropValue) &lpPropValue[idx]);

            // if it is a MultiValueProperty, parse the output, and add
            //   more than one row for this property
            if( (PROP_TYPE(lpPropValue[idx].ulPropTag)) & MV_FLAG )
            {
                // it is multi value, so strtok out the data 
                // and add a row for each data value                                                        
                ulMVRow = 0;

                // determine number of cValues
                lpszToken   = strtok(szData,szMVSeps);
                ulMVcValues = strtoul(lpszToken,&szEnd,16);        

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                                
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    lstrcpy(szMV,szBuffer);
                    wsprintf(szTemp,"[%lu] ",ulMVRow);
                    lstrcat(szMV,szTemp);
                    lstrcat(szMV,lpszToken);
                
                    dwReturn = SendDlgItemMessage(IDC_PROPVIEW,LB_ADDSTRING,0,
                            (LPARAM)szMV);
                
                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
                
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < ulMVcValues )
                {
                    lstrcpy(szMV,szBuffer);
                    wsprintf(szTemp,"[%lu] ",ulMVRow);
                    lstrcat(szMV,szTemp);
                    lstrcat(szMV,"<No Data Available>");
                
                    dwReturn = SendDlgItemMessage(IDC_PROPVIEW,LB_ADDSTRING,0,
                            (LPARAM)szMV);
                    ulMVRow++;                            
                }
            }
            else            
            {
                lstrcat(szBuffer,szData);
                dwReturn = SendDlgItemMessage(IDC_PROPVIEW,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
            }
        }
        else
            dwReturn = SendDlgItemMessage(IDC_PROPVIEW,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    

    }  // end of for loop of ulValues in lpPropValue

    dwReturn = SendDlgItemMessage(IDC_PROPVIEW,LB_SETCURSEL,(WPARAM) -1 ,0 );

End:

    if(lpPropValue)
    {
        MAPIFreeBuffer(lpPropValue);
        lpPropValue = NULL;
    }
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




/*--------------------*/
/* Library Init stuff */
/*--------------------*/
/********************************************************************/

/********************************************************************/

/********************************************************************/

class CPropViewDLL : public CWinApp
{
public:
    virtual BOOL InitInstance();
    virtual BOOL ExitInstance();

    CPropViewDLL(const char *pszAppName)
            : CWinApp(pszAppName)
        {
        }
};

BOOL CPropViewDLL::InitInstance()
{
    SetDialogBkColor();
    return TRUE;
}


BOOL CPropViewDLL::ExitInstance()
{
    return TRUE;
}

CPropViewDLL  vtDLL("propvu32.dll");
