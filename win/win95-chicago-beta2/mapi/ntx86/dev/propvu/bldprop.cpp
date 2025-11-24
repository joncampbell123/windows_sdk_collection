/*******************************************************************/
/*
 -  BldProp.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CBldPropDlg
 *      This MFC Dialog is used to build up a lpsPropTagArray
 *      for the LPMAPIPROP functions lpObj->DeleteProps()
 *      lpObj->GetProps(), or a lpsPropValueArray for
 *      lpObj->SetProps().  The property viewer DLL uses
 *      this function to build up these property arrays
 *      by selecting Property IDs, Property Types, and
 *      Property Data to build in the PropValue Arrays
 *      and/or PropTag Arrays.  You are able to view the
 *      currently selected arrays, and add/delete from the 
 *      selected arrays.
 */
/*******************************************************************/

#undef  CINTERFACE      // C++ calling convention for mapi calls

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
#include <strtbl.h>
#include <misctool.h>
#include <pvalloc.h>
#include "resource.h"
#include "propvu.h"  
#include "bldprop.h"

/******************* Set Property Message Map *********************/

BEGIN_MESSAGE_MAP(CBldPropDlg, CModalDialog)

    ON_LBN_SELCHANGE(   IDC_SP_PROPTAGS,                OnSelectPropTagLB)
    ON_EN_CHANGE(       IDC_SP_PROPID,                  OnChangePropIDHexLB)
    ON_COMMAND(         IDC_SP_ADD,                     OnAdd)
    ON_COMMAND(         IDC_SP_DELETE,                  OnDelete)
    ON_COMMAND(         IDC_SP_CALL,                    OnCall)
    ON_COMMAND(         IDC_SP_REMOVEALL,               OnRemoveAll)
    ON_COMMAND(         IDC_SP_ADD_ALL_MAPITAGS,        OnAddAllMapiTags)
    ON_COMMAND(         IDC_SP_ADD_ALL_CURRENT,         OnAddAllCurrent)
    ON_COMMAND(         IDC_STOREPROPS,                 OnDumpPropValsToFile)
    ON_CBN_SELCHANGE(   IDC_SP_PROPTYPE,                OnSelectPropTypeCB)
    

END_MESSAGE_MAP()

/*******************************************************************/
/**************************** SETPROPS *****************************/



void CBldPropDlg::OnAddAllMapiTags()
{
    ULONG               ulMax           = 0;
    ULONG               idx             = 0;
    LPSPropValue        lpspva          = NULL;
    ULONG               ulPropTag       = 0;

    ulMax =   GetRowCount("PropTags");

    
    if(m_fIsPropValueArray)
    {
        if( (ulMax + m_ulNewValues) >= MAX_SET_PROPS  )
            return;

        // add one of each mapi tag type
        for(idx = 0; idx < ulMax ; idx++)
        {

            lpspva = (LPSPropValue) PvAlloc(sizeof(SPropValue));
            memset(lpspva,0,(size_t)sizeof(SPropValue));

            lpspva->ulPropTag = GetRowID("PropTags",idx);

            switch(PROP_TYPE(lpspva->ulPropTag))
            {


                //$  LOOK RETHINK THIS NOW !!!!




                // DONT ADD PROPTAGS OF THESE THESE TYPES TO LIST
                case PT_UNSPECIFIED:
                case PT_CLSID:
                case PT_OBJECT:
                case PT_UNICODE:

                case PT_MV_I2:
                case PT_MV_LONG:
                case PT_MV_R4:
                case PT_MV_DOUBLE:
                case PT_MV_CURRENCY:
                case PT_MV_APPTIME:
                case PT_MV_SYSTIME:
                case PT_MV_STRING8:
                case PT_MV_BINARY:
                case PT_MV_UNICODE:
                case PT_MV_CLSID:
                case PT_MV_I8:

                    // dont add to list 
                    PvFree(lpspva);
                    break;



                // ADD PROPTAGS OF THESE TYPES TO LIST
                case PT_STRING8:
                    lpspva->Value.lpszA = (LPSTR) PvAllocMore(10,lpspva);
                    lstrcpy(lpspva->Value.lpszA,"JoeSmi");
                    m_ulNewValues++;
                    m_lppNewPropValue[m_ulNewValues -1 ] = lpspva;
                    break;
                case PT_BINARY:
                    lpspva->Value.bin.cb        = 3;                
                    lpspva->Value.bin.lpb       = (LPBYTE) PvAllocMore(10,lpspva);
                    lpspva->Value.bin.lpb[0]    = 0;
                    lpspva->Value.bin.lpb[1]    = 1;
                    lpspva->Value.bin.lpb[2]    = 0;
                    m_ulNewValues++;                            
                    m_lppNewPropValue[m_ulNewValues -1 ] = lpspva;
                    break;
                default:        
                    m_ulNewValues++;
                    m_lppNewPropValue[m_ulNewValues -1 ] = lpspva;
                    break;
            }            
        }            
    }
    else        // its a PropTagArray
    {
        if( (ulMax + m_lpNewPTA->cValues) >= MAX_SET_PROPS  )
             return;

        for(idx = 0; idx < ulMax ; idx++)
        {
            ulPropTag = GetRowID("PropTags",idx);
            
            switch(PROP_TYPE(ulPropTag) )
            {

                //$  LOOK RETHINK THIS NOW !!!!




                // DONT ADD PROPTAGS OF THESE THESE TYPES TO LIST
                case PT_UNSPECIFIED:
                case PT_CLSID:
                case PT_OBJECT:
                case PT_UNICODE:

                case PT_MV_I2:
                case PT_MV_LONG:
                case PT_MV_R4:
                case PT_MV_DOUBLE:
                case PT_MV_CURRENCY:
                case PT_MV_APPTIME:
                case PT_MV_SYSTIME:
                case PT_MV_STRING8:
                case PT_MV_BINARY:
                case PT_MV_UNICODE:
                case PT_MV_CLSID:
                case PT_MV_I8:
                       break;

                default:
                    m_lpNewPTA->cValues++;
                    m_lpNewPTA->aulPropTag[m_lpNewPTA->cValues - 1] = ulPropTag;
                    break;
            }
        }   
    }     

      
    RedrawBuildProps();     


}

/********************************************************************/
/********************************************************************/

void CBldPropDlg::OnAddAllCurrent()
{
    LPSPropValue        lpPVAGetProps   = NULL;
    ULONG               ulValues        = 0;
    LPSPropTagArray     lpPTA           = NULL;
    ULONG               idx             = 0;
    LPSPropValue        lpspva          = NULL;
    HRESULT             hResult         = hrSuccess;
    CGetError           E;
    
    
    if(m_fIsPropValueArray)
    {

        if( HR_FAILED(hResult = m_lpSetEntry->GetProps(NULL, 0, &ulValues, &lpPVAGetProps) ) )
        {
            MessageBox( E.SzError("CBldPropDlg::OnAddAllCurrent   GetProps()", hResult), 
                                "Client", MBS_ERROR );
            return;
        }
    
        if( (ulValues + m_ulNewValues) >= MAX_SET_PROPS  )
            return;
            
        // add one of each mapi tag type
        for(idx = 0; idx < ulValues ; idx++)
        {        
            lpspva = (LPSPropValue) PvAlloc(sizeof(SPropValue));
            memset(lpspva,0,(size_t)sizeof(SPropValue));

            CopyPropValue(lpspva,&(lpPVAGetProps[idx]),lpspva);

            m_ulNewValues++;                            
            m_lppNewPropValue[m_ulNewValues -1 ] = lpspva;
        }            
    }
    else        // its a PropTagArray
    {

        if( HR_FAILED(hResult = m_lpSetEntry->GetPropList(0, &lpPTA) ) )
        {
            MessageBox( E.SzError("CBldDlg::OnAddAllCurrent()  lpEntry->GetPropList()",
                hResult), "Client", MBS_ERROR );
            return;
        }
        if( (lpPTA->cValues + m_lpNewPTA->cValues) >= MAX_SET_PROPS  )
             return;

        for(idx = 0; idx < lpPTA->cValues ; idx++)
        {           
            m_lpNewPTA->cValues++;
            m_lpNewPTA->aulPropTag[m_lpNewPTA->cValues - 1] = lpPTA->aulPropTag[idx];
        }   
    }     
    RedrawBuildProps();     

    if(lpPTA)
        MAPIFreeBuffer(lpPTA);    

    if(lpPVAGetProps)
        MAPIFreeBuffer(lpPVAGetProps);
}


/********************************************************************/
/********************************************************************/

void CBldPropDlg::OnRemoveAll()
{
    ULONG               idx             = 0;
    
    if(m_fIsPropValueArray)
    {
        if(m_lppNewPropValue)
        {
            for(idx = 0 ; idx < m_ulNewValues; idx++)
            {
                PvFree(m_lppNewPropValue[idx]);
                m_lppNewPropValue[idx] = NULL;
            }
        }
        m_ulNewValues = 0;       
    }
    else        // its a PropTagArray
    {
        m_lpNewPTA->cValues = 0;
    }
    
    RedrawBuildProps();     
}


/********************************************************************/
/*
 -  CBldPropDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      First routine called when an instance of a setprops dialog
 *      is created.  This routine initalizes the listboxs and edit
 *      controls to select the property to set chosen from the 
 *      property listbox of the calling dialog(folder/attachment/message)
 *
 */
/********************************************************************/

BOOL CBldPropDlg::OnInitDialog()
{
    HRESULT         hResult         = hrSuccess;
    SCODE           scResult        = SUCCESS_SUCCESS;
    CGetError       E;
    ULONG           cColumn         = 0;
    LPSPropValue    lpPropValue     = NULL;
    int             iRow            = 0;
    char            szBuffer[4096];
    int             idx             = 0;
    LONG            lSelection      = -1;
    int             len             = 0;
    char            szPropID[30];
    ULONG           ulPropID        = 0;
    DWORD           dwIndex         = 0;
    ULONG           ulRowCount      = 0;
    
    szBuffer[0] = '\0' ; 
    SetWindowText( m_Operation.GetBuffer(30) );

    SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_SP_PROPTAGS,LB_RESETCONTENT,0,0);
    dwIndex = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_SETCURSEL,(WPARAM)-1,0);
    dwIndex = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_SETCURSEL,(WPARAM)-1,0);
    
    if( !m_lpSetEntry )
    {
        MessageBox( E.SzError("CBldPropDlg::OnInitDialog   lpSetEntry == NULL", hResult),
                         "Client", MBS_ERROR );
        goto error;
    }           

    // m_lpOldPropValue holds onto state of property when enter this setprops dialog
    // m_lpNewPropValue holds new propval array to set existing props with

    if(m_lpOldPropValue)
    {
        MAPIFreeBuffer(m_lpOldPropValue);
        m_lpOldPropValue = NULL;
    }

    if( HR_FAILED(hResult = m_lpSetEntry->GetProps(NULL, 0, &m_ulOldValues, &m_lpOldPropValue) ) )
    {
        MessageBox( E.SzError("CBldPropDlg::OnInitDialog   GetProps(Old)", hResult), 
                            "Client", MBS_ERROR );
        goto error;
    }

    // ADD DATA TO LISTBOX OF PROP IDS
    szBuffer[0] = '\0' ;
    // this (UNKNOWN PROPID) is always in the 0th place in listbox    
    dwIndex = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_ADDSTRING,0,(LPARAM)(void *)"AN UNKNOWN PROPID");

    ulRowCount = GetRowCount("PropIDs");
    for( idx = 0; idx < ulRowCount ; idx++ )
    {
        if( !GetRowString("PropIDs",idx,szBuffer) )
             goto error;

        dwIndex = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_ADDSTRING,0,(LPARAM)szBuffer);
    }          

    // select string in listbox

    m_SelectedTag = "PR_SUBJECT";
    
    lSelection = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_SELECTSTRING,(WPARAM) -1,
                        (LPARAM) m_SelectedTag.GetBuffer(10) );

    szBuffer[0] = '\0' ;
    
    // ADD DATA TO LISTBOX OF PROP TYPES, ORDER IS IMPORTANT, alphabetical !!!!
    SendDlgItemMessage(IDC_SP_PROPTYPE,CB_RESETCONTENT,0,0);

    ulRowCount = GetRowCount("PropType");
    for( idx = 0; idx < ulRowCount ; idx++ )
    {
        if( !GetRowString("PropType",idx,szBuffer) )
             goto error;

        dwIndex = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_ADDSTRING,0,(LPARAM)szBuffer);

    }          

    
    // now select the property type associated with the tag selected

    // set PropID edit control
    // THIS MUST COME BEFORE  PROP TYPE IS SELECTED or else
    // the event of change to the PropIDHex edit control will overwrite the
    // selected prop type.

    m_lSelectedTag = PR_SUBJECT;      
  
    SendDlgItemMessage(IDC_SP_PROPID,WM_CLEAR,0,0);
    wsprintf (szPropID, "%04X", PROP_ID(m_lSelectedTag));               
    GetDlgItem(IDC_SP_PROPID    )->SetWindowText(szPropID);

    // this must come after the Setting of the PropID Hex field !!!!!!

    // explain why it must

    
    GetString("PropType",PROP_TYPE(m_lSelectedTag),szBuffer);
    lSelection = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_SELECTSTRING,(WPARAM) -1,
                        (LPARAM) szBuffer );


    SelectNewPropType( PROP_TYPE(m_lSelectedTag) );

    szBuffer[0] = '\0';

    // Set PropData edit control
    SendDlgItemMessage(IDC_SP_PROPDATA,WM_CLEAR,0,0);
        
    for(idx = 0; idx < m_ulOldValues ; idx++)
    {
        if( ((ULONG) m_lSelectedTag) == ((ULONG)m_lpOldPropValue[idx].ulPropTag ) )
        {
            
            if( PROP_TYPE(m_lSelectedTag) == PT_BINARY )
            {
                char    lpszHex[9];
                ULONG   cb = 0;
                ULONG   cChars = 0;
                LPBYTE  lpb = m_lpOldPropValue[idx].Value.bin.lpb;

                while((cb < m_lpOldPropValue[idx].Value.bin.cb) && (cChars < MAX_LOG_BUFF-16 ) )
                {
                    wsprintf(lpszHex, "%02X ", *lpb);
                    lstrcat(szBuffer, lpszHex);
                    cChars += 3;
                    lpb++;
                    cb++;
                }
            }
            else
            {
                SzGetPropValue(szBuffer,(LPSPropValue) &m_lpOldPropValue[idx]);
            }
            break;
        }
    }
    GetDlgItem(IDC_SP_PROPDATA  )->SetWindowText(szBuffer);

    // if there is not already a value for selected PVA in listbox    
    if(!m_lppNewPropValue)  
        m_lppNewPropValue = (LPSPropValue *) PvAlloc(MAX_SET_PROPS * sizeof(LPSPropValue));
    
    // if it is a Prop tag array, disable the Data.
    if( !m_fIsPropValueArray)
    {
        // is not already allocated an initialized
        if(!m_lpNewPTA)
        {
            m_lpNewPTA = (LPSPropTagArray) PvAlloc( (MAX_SET_PROPS * sizeof(ULONG)) + sizeof(SPropTagArray));
            m_lpNewPTA->cValues = 0;
        }
        GetDlgItem(IDC_SP_PROPDATA  )->EnableWindow(FALSE);

        SetDlgItemText(IDT_SP_PROPDISPLAY,"Selected lpsPropTagArray:");       
    }       

    GetDlgItem(IDC_SP_PROPTYPEHEX)->EnableWindow(FALSE);

    // for some cases, we may already have a value inside the selected PTA,PVA
    RedrawBuildProps();

    return TRUE;    

error:

    if(m_lpOldPropValue)
    {
        MAPIFreeBuffer(m_lpOldPropValue);
        m_lpOldPropValue    = NULL;
        m_ulOldValues       = 0;
    }
    
    return FALSE;
    
}



/*******************************************************************/
/*
 -  CBldPropDlg::
 -  RedrawBuildProps
 *
 *  Purpose:
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CBldPropDlg::RedrawBuildProps()
{
    char            szBuffer[1024];
    int             idx             = 0;
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;
    char            szCValues[80];

    // multi value props stuff    
    char            szMV[1024];    

    char            szMVSeps[]      = ";}";


    char            *lpszToken      = NULL;
    char            szTemp[30];
    ULONG           ulMVRow         = 0;
    ULONG           ulMVcValues     = 0;
    char            *szEnd          = NULL;

    char szID[50];
    char szData[512];
    char szType[32];      // Assumes no PropType string longer than 31 chars

    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_RESETCONTENT,0,0);

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );
    

    // either a PTA or PVA
    
    // if it is a PropValueArray
    if(m_fIsPropValueArray) 
    {           
            
        for(idx = 0; idx < m_ulNewValues; idx++)
        {   
            szID[0]     = '\0' ;
            szData[0]   = '\0' ;
            szType[0]   = '\0' ;
            szBuffer[0] = '\0' ;
                
            if(GetString("PropIDs", PROP_ID( (*(m_lppNewPropValue[idx])).ulPropTag), szID ) )
            {
                lstrcat(szBuffer, szID );               
                lstrcat(szBuffer, "\t");
            }
            else
            {
                wsprintf(szBuffer,"%#04X\t", PROP_ID(  (*(m_lppNewPropValue[idx])).ulPropTag) );        
            }

            if( GetString("PropType", PROP_TYPE( (*(m_lppNewPropValue[idx])).ulPropTag), szType) )
            {
                lstrcat(szBuffer, szType);
                lstrcat(szBuffer,"\t");
            }       
            else
            {
                wsprintf(szType,"%#04X\t", PROP_TYPE( (*(m_lppNewPropValue[idx])).ulPropTag) );
                lstrcat(szBuffer,szType);
            }


            SzGetPropValue(szData,(LPSPropValue) m_lppNewPropValue[idx]);

            // if it is a MultiValueProperty, parse the output, and add
            //   more than one row for this property
            if( (PROP_TYPE( (*(m_lppNewPropValue[idx])).ulPropTag)) & MV_FLAG )
            {
                // it is multi value, so strtok out the data 
                // and add a row for each data value                                                        
                ulMVRow = 0;

                // determine number of cValues
                lpszToken   = strtok(szData,":");

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
                
                    dwReturn = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_ADDSTRING,0,
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
                
                    dwReturn = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_ADDSTRING,0,
                            (LPARAM)szMV);
                    ulMVRow++;                            
                }
            }
            else            
            {
                lstrcat(szBuffer,szData);
                dwReturn = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
            }


        }  // end of for loop of m_ulNewValues in lpNewPropValue

        wsprintf(szCValues,"Selected cValues:  %lu",m_ulNewValues);
        SetDlgItemText(IDT_SP_PROPCVALUES,szCValues);       

    }
    else  // it is a proptag array
    {
            
        for(idx = 0; idx < m_lpNewPTA->cValues; idx++)
        {   
            szID[0]     = '\0' ;
            szType[0]   = '\0' ;
            szBuffer[0] = '\0' ;
                
            if(GetString("PropIDs", PROP_ID(m_lpNewPTA->aulPropTag[idx]), szID ) )
            {
                lstrcat(szBuffer, szID );               
                lstrcat(szBuffer, "\t");
            }
            else
            {
                wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpNewPTA->aulPropTag[idx]) );        
            }

            if( GetString("PropType", PROP_TYPE(m_lpNewPTA->aulPropTag[idx]), szType) )
            {
                lstrcat(szBuffer, szType);
                lstrcat(szBuffer,"\t");
            }       
            else
            {
                wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpNewPTA->aulPropTag[idx]) );
                lstrcat(szBuffer,szType);
            }
            dwReturn = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_ADDSTRING,0,
                                (LPARAM)szBuffer);
        
        }  
        wsprintf(szCValues,"Selected lpsPTA->cValues:  %lu",m_lpNewPTA->cValues);
        SetDlgItemText(IDT_SP_PROPCVALUES,szCValues);       
        
    }             


    
    dwReturn = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_SETCURSEL,(WPARAM) -1 ,0 );
}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  OnSelectPropTagLB
 *
 *  Purpose:
 *      Upon selection of a property tag id in the listbox of available
 *      proptag ids, the combo box of the proptype will correspond to
 *      the default of this id and the data and ID hex value will adjust 
 *      to currently selected item as well.
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CBldPropDlg::OnSelectPropTagLB()
{
    char    szBuffer[512];
    char    szPropID[30];
    int     idx         = 0;
    LONG    lCurSelect  = 0;
    DWORD   dwIndex     = 0;

    szBuffer[0] = '\0';

    // determine which entry in listbox is selected
    lCurSelect = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_GETCURSEL,0,0);

    // if select an unknown ID
    if(lCurSelect == 0 )
    {
        // select unknown type for Type combo box
        dwIndex = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_SELECTSTRING,
                (WPARAM) -1, (LPARAM)(void *) "PT_UNSPECIFIED");

        szBuffer[0] = '\0';
        // Set PropData edit control
        SendDlgItemMessage(IDC_SP_PROPDATA,WM_CLEAR,0,0);
        GetDlgItem( IDC_SP_PROPDATA )->SetWindowText(  szBuffer );
        return;   
    }       

    // set proptag listbox selection
    SendDlgItemMessage(IDC_SP_PROPTAGS,LB_GETTEXT,(WPARAM)lCurSelect,(LPARAM)szBuffer);
    m_SelectedTag = szBuffer;

    GetID("PropTags", m_SelectedTag.GetBuffer(10),&m_lSelectedTag );
        

    // set PropID edit control
    SendDlgItemMessage(IDC_SP_PROPID,WM_CLEAR,0,0);
    wsprintf (szPropID, "%04X", PROP_ID(m_lSelectedTag));               
    GetDlgItem(IDC_SP_PROPID    )->SetWindowText(szPropID);

}


/*******************************************************************/
/*
 -  CBldPropDlg::
 -  SelectNewPropType
 *
 *  Purpose:
 *
 *  Parameters:
 *      LONG lSelectedType
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CBldPropDlg::SelectNewPropType(ULONG ulSelectedType)
{
    char    szPropType[128];
    char    szDataItemDisplay[128];

    SendDlgItemMessage(IDC_SP_PROPTYPEHEX,WM_CLEAR,0,0);
    wsprintf (szPropType, "%04X", ulSelectedType );             
    GetDlgItem(IDC_SP_PROPTYPEHEX   )->SetWindowText(szPropType);


    if(!m_fIsPropValueArray)
    {
        wsprintf(szDataItemDisplay,"Building PropTagArray, Data Not Applicable");                                
        SetDlgItemText(IDT_SP_PROPDATA,szDataItemDisplay);
        return;
    }            
    
    // now set data field description of how to enter data
    // depending on the type. 
    
    switch(ulSelectedType)
    {
        case PT_UNSPECIFIED:
            wsprintf(szDataItemDisplay,"Prop Data: PT_UNSPECIFIED, Data Unspecified");
            break;

        case PT_NULL:
            wsprintf(szDataItemDisplay,"Prop Data: PT_NULL, No Data");        
            break;

        case PT_I2:
            wsprintf(szDataItemDisplay,"Prop Data: PT_I2, enter a short int ex. 43");                
            break;

        case PT_LONG:
            wsprintf(szDataItemDisplay,"Prop Data: PT_LONG, enter a long ex. 43");                        
            break;

        case PT_R4:
            wsprintf(szDataItemDisplay,"Prop Data: PT_R4, enter a float ex. 43.0007");                                
            break;

        case PT_DOUBLE:
            wsprintf(szDataItemDisplay,"Prop Data: PT_DOUBLE, enter a double ex. 435677");                                        
            break;

        case PT_CURRENCY:
            wsprintf(szDataItemDisplay,"Prop Data: PT_CURRENCY, enter two longs(low then high) ex. 5 78");                                                
            break;

        case PT_APPTIME:
            wsprintf(szDataItemDisplay,"Prop Data: PT_APPTIME, enter a double ex. 435677");                                                
            break;

        case PT_ERROR:
            wsprintf(szDataItemDisplay,"Prop Data: PT_ERROR, enter a textized Error code ex. MAPI_E_NO_ACCESS");                                                        
            break;

        case PT_BOOLEAN:
            wsprintf(szDataItemDisplay,"Prop Data: PT_BOOLEAN, enter a boolean 1 or 0");                                                                            
            break;

        case PT_OBJECT:
            wsprintf(szDataItemDisplay,"Prop Data: PT_OBJECT, data doesn't make sense");                                                                                    
            break;

        case PT_I8:
            wsprintf(szDataItemDisplay,"Prop Data: PT_I8, enter 2 DWORDs LowPart HighPart ex. 34 67");                                                                                            
            break;

        case PT_SYSTIME:
            wsprintf(szDataItemDisplay,"Prop Data: PT_SYSTEMTIME, Enter Y/M/D Hr:Min:Sec ex. 1994/02/08 17:55:27");                                                                                                    
            break;

        case PT_STRING8:
            wsprintf(szDataItemDisplay,"Prop Data: PT_STRING8, Enter string ex. Hello World");                                                                                                    
            break;

        case PT_UNICODE:
            wsprintf(szDataItemDisplay,"Prop Data: PT_UNICODE, Enter string ex. Hello World");                                                                                                            
            break;

        case PT_CLSID:
            wsprintf(szDataItemDisplay,"Prop Data: PT_CLSID, Enter Guid, currently NOT SUPPORTED");                                                                                                                    
            break;

        case PT_BINARY:
            wsprintf(szDataItemDisplay,"Prop Data: PT_BINARY, Enter 2 bytes,space,2 bytes... Ex. 00 01 10 11");
            break;

        case PT_MV_I2:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_I2, Enter cVals: Val1; Val2;... Ex. 3: 43; 33; 12;");
            break;

        case PT_MV_LONG:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_LONG, Enter cVals: Val1; Val2;... Ex. 3: 43; 33; 12;");
            break;

        case PT_MV_R4:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_LONG, Enter cVals: Val1; Val2;... Ex. 3: 43.00; 33.78; 12.9001;");
            break;

        case PT_MV_DOUBLE:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_DOUBLE, Enter cVals: Val1; Val2;... Ex. 3: 43; 33; 12;");
            break;

        case PT_MV_CURRENCY:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_CURRENCY, Enter cVals: lo1 hi1 ; lo2 hi2;... Ex. 2: 4 3; 3 53");
            break;

        case PT_MV_APPTIME:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_APPTIME, Enter cVals: Val1; Val2;... Ex. 3: 43; 33; 12;");
            break;

        case PT_MV_SYSTIME:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_SYSTIME, Enter cVals: Val1; Val2; Val3 Ex. 1: 1994/2/2 1:12:30 ;");
            break;

        case PT_MV_BINARY:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_BINARY, Enter cVals:Val1;Val2;Val3 Ex. 2:11 00 10;11 01;");
            break;

        case PT_MV_STRING8:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_STRING8, Enter cVals: Val1; Val2;... Ex. 3: joe; smith; test;");
            break;

        case PT_MV_UNICODE:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_UNICODE, Enter cVals: Val1; Val2;... Ex. 3: joe; smith; test;");
            break;

        case PT_MV_CLSID:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_CLSID, Currently NOT SUPPORTED");
            break;

        case PT_MV_I8:
            wsprintf(szDataItemDisplay,"Prop Data: PT_MV_I8, Enter cVals: Low1 High1; Low2 High2;... Ex. 2: 4 43; 67 33; ");
            break;
            
        default:
            wsprintf(szDataItemDisplay,"Prop Data: Unknown Prop Type ");
            break;
    }            

    SetDlgItemText(IDT_SP_PROPDATA,szDataItemDisplay);
}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  OnSelectPropTypeCB
 *
 *  Purpose:
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CBldPropDlg::OnSelectPropTypeCB()
{
    char    szBuffer[512];
    LONG    lCurSelect      = 0;
    LONG    lSelectedType   = 0;
    
    szBuffer[0] = '\0';
    // determine which entry in listbox is selected
    lCurSelect = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_GETCURSEL,0,0);

    // set proptag listbox selection
    SendDlgItemMessage(IDC_SP_PROPTYPE,CB_GETLBTEXT,(WPARAM)lCurSelect,(LPARAM)szBuffer);
        
    GetID("PropType",szBuffer,&lSelectedType);

    SelectNewPropType((ULONG)lSelectedType);
}


/*******************************************************************/
/*
 -  CBldPropDlg::
 -  OnChangePropHexLB
 *
 *  Purpose:
 *      Select a hex value for the PROP ID and adjust the 
 *      proptag listbox string values, proptype string values.
 *      and the prop data accordingly
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
/*******************************************************************/

void CBldPropDlg::OnChangePropIDHexLB()
{
    char    szBuffer[4096];
    int     idx         = 0;
    LONG    lCurSelect  = 0;
    DWORD   dwIndex     = 0;
    ULONG   ulIDVal     = 0;
    LONG    len         = -1;
    int     i           = 0;
    LONG    lTempTag    = 0;

    // get value out of the edit control
    *(WORD *)szBuffer = sizeof(szBuffer) -1;    // first char has buffer length

    len  = SendDlgItemMessage(IDC_SP_PROPID,EM_GETLINE,0,(LPARAM)(void *)szBuffer);
            
    szBuffer[len] = '\0';        // null terminate our buffer

    if( ! AsciiToHex(len,szBuffer, &ulIDVal ) )

    {
        MessageBox("CDlg::OnSet AsciiToHex Failed", "Client", MBS_ERROR );
        return;
    }       

    // find out if the hex value is in PropTag list
    if( !GetString("PropIDs",ulIDVal,szBuffer) )
    {
        // set the current selection as UNKNOWN PROPID and PROPTYPE
        dwIndex = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_SETCURSEL, 0,0);
        
        dwIndex = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_SELECTSTRING,
                (WPARAM) -1, (LPARAM)(void *) "PT_UNSPECIFIED");

        szBuffer[0] = '\0';
        // Set PropData edit control
        SendDlgItemMessage(IDC_SP_PROPDATA,WM_CLEAR,0,0);

        SelectNewPropType( PT_UNSPECIFIED );

        GetDlgItem( IDC_SP_PROPDATA )->SetWindowText(  szBuffer );

        return;           
    }

                    
    // if it is NOT an UNKNOWN PROPID, adjust PropTags, PropType, and DATA
    dwIndex = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_SELECTSTRING,
                (WPARAM) -1, (LPARAM) szBuffer);
        
    // determine which entry in listbox is selected
    lCurSelect = SendDlgItemMessage(IDC_SP_PROPTAGS,LB_GETCURSEL,0,0);
    SendDlgItemMessage(IDC_SP_PROPTAGS,LB_GETTEXT,(WPARAM)lCurSelect,
                        (LPARAM)szBuffer);
    m_SelectedTag = szBuffer;
    
    // if property exists, don't change the m_lSelectedTag   
    // only reset the m_lSelectedTag if the property does not already exist !!!!!!!!!    

    GetID("PropTags", m_SelectedTag.GetBuffer(5),&lTempTag );

    // loop through the list of existing old properties and see if it is there

    for( i = 0; i < m_ulOldValues ; i++)
    {
        if( (PROP_ID(m_lpOldPropValue[i].ulPropTag)) == (PROP_ID(lTempTag)) )
            break;                 
    }    

    // if not, set m_lSelectedTag to new computed value(default)
    if( i == m_ulOldValues )  // if it reached the end of the list without match
        m_lSelectedTag = lTempTag;    

    // set PropType Control Box
    GetString("PropType",PROP_TYPE(m_lSelectedTag),szBuffer);
    dwIndex = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_SELECTSTRING,
                (WPARAM) -1, (LPARAM) szBuffer);


    // new
    SelectNewPropType( PROP_TYPE(m_lSelectedTag) );

    // Set PropData edit control
    SendDlgItemMessage(IDC_SP_PROPDATA,WM_CLEAR,0,0);
    szBuffer[0] = '\0';

    for(idx = 0; idx < m_ulOldValues ; idx++)
    {
        if( ((ULONG) m_lSelectedTag) == ((ULONG)m_lpOldPropValue[idx].ulPropTag ) )
        {
            if( PROP_TYPE(m_lSelectedTag) == PT_BINARY )
            {
                char    lpszHex[9];
                ULONG   cb          = 0;
                ULONG   cChars      = 0;
                LPBYTE  lpb         = m_lpOldPropValue[idx].Value.bin.lpb;

                while((cb < m_lpOldPropValue[idx].Value.bin.cb) && (cChars < MAX_LOG_BUFF-16 ) )
                {
                    wsprintf(lpszHex, "%02X ", *lpb);
                    lstrcat(szBuffer, lpszHex);
                    cChars += 3;
                    lpb++;
                    cb++;
                }
            }
            else
            {
                SzGetPropValue(szBuffer,(LPSPropValue) &m_lpOldPropValue[idx]);
            }
            break;
        }
        
    }
    GetDlgItem( IDC_SP_PROPDATA )->SetWindowText(  szBuffer );
}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  OnAdd
 -
 *  Purpose:
 *      Commit the SetProps on newly constructed Property
 */
/*******************************************************************/

#ifdef WIN16
#pragma optimize("",off)
#endif

void CBldPropDlg::OnAdd()
{
    char    szBuffer[512];
    LONG    lCurSelect      = 0;
    LONG    lSelectedType   = 0;
    int     idx             = 0;
    HRESULT hResult         = hrSuccess;
    SCODE   scResult        = SUCCESS_SUCCESS;
    LPSPropProblemArray     lpProblems = NULL;
    CGetError E;
    ULONG   ulPropID        = 0;
    LONG    len             = 0;
    LONG    lenBin          = 0;
    ULONG   ulVal           = 0;
    ULONG   ulIDVal         = 0;
    int     i               = 0;
    LONG    lVal            = 0;
    double  dVal            = 0;
    int     iVal            = 0;
    float   fVal            = (float) 0.0;
    int     dBytes          = 0;
    int     dCopiedBytes    = 0;
    DWORD   dwRet           = 0;
    int     dBufferLen      = 0;
    LPSPropValue    lpNewPVA = NULL;
    ULONG           ulMax           = 0;
    SYSTEMTIME FAR  *lpSysTime          = NULL;

    // multi value props stuff    
    char            szMVSeps[]      = ";:";

    LPSTR           *lppszToken     = NULL;
    char            *lpszTemp       = NULL;
    char            *lpszToken      = NULL;
    ULONG           ulMVRow         = 0;
    char            *szEnd          = NULL;



    //PROPTYPE
    szBuffer[0] = '\0' ;
    // get property type combo box entry
    lCurSelect = SendDlgItemMessage(IDC_SP_PROPTYPE,CB_GETCURSEL,0,0);
    SendDlgItemMessage(IDC_SP_PROPTYPE,CB_GETLBTEXT,(WPARAM)lCurSelect,
                        (LPARAM)szBuffer);
    GetID("PropType", szBuffer,&lSelectedType );    
    
    //PROPID HEX VALUE read out
    szBuffer[0] = '\0' ;
    // get value out of the edit control
    *(WORD *)szBuffer = sizeof(szBuffer) -1;    // first char has buffer length
    len  = SendDlgItemMessage(IDC_SP_PROPID,EM_GETLINE,0,(LPARAM)szBuffer);
    szBuffer[len] = '\0';        // null terminate our buffer
    // get a line of data from ID edit control
    if( ! AsciiToHex(len,szBuffer, &ulIDVal ) )
    {
        MessageBox("CDlg::OnSet AsciiToHex Failed", "Client", MBS_ERROR );
        goto Error;
    }       

    // if it is a PropValueArray
    if(m_fIsPropValueArray) 
    {           
        // allocate a new SPropValue and add to list(use PvAlloc)
        lpNewPVA = (LPSPropValue) PvAlloc( sizeof(SPropValue) );
        lpNewPVA[0].ulPropTag = PROP_TAG((ULONG)lSelectedType, (ULONG)ulIDVal ); 
        GetString("PropTags",lpNewPVA[0].ulPropTag,szBuffer);

        //PROPDATA 
        // get value out of the edit control
        *(WORD *)szBuffer = sizeof(szBuffer) -1;    // first char has buffer length
        // get a line of data from data edit control
        len = SendDlgItemMessage(IDC_SP_PROPDATA,EM_GETLINE,0,(LPARAM)szBuffer);
        szBuffer[len] = '\0' ;  // ensure end of string is null terminated from getline

        // determine which type of data it is and load  
        switch(lSelectedType)
        {
            // all elements of size ULONG
            //$ FUTURE this should not convert all of these to hex, it should
            //    convert to the appropriate type Decimal, SYSTEMTIME, etc.

            case PT_UNSPECIFIED:                       
                break;
                
            case PT_I2:         // short int
                iVal = atoi(szBuffer);
                lpNewPVA[0].Value.i = iVal;                 
                break;        

            case PT_LONG:       // long
                lVal = strtol(szBuffer,&szEnd,10);
                lpNewPVA[0].Value.l = lVal;                 
                break;        

            case PT_R4:         // float
                fVal = (float) atof(szBuffer);
                lpNewPVA[0].Value.flt = fVal;                   
                break;        

            case PT_DOUBLE:     // double
                dVal = strtod(szBuffer,&szEnd);
                lpNewPVA[0].Value.dbl = dVal;                   
                break;        

            case PT_CURRENCY:   // Special Currency two longs such as 3 45
                lpszToken   = strtok(szBuffer," ");
                lpNewPVA[0].Value.cur.Lo =  strtol(lpszToken,&szEnd,10);        

                lpszToken   = strtok(NULL," ");
                lpNewPVA[0].Value.cur.Hi =  strtol(lpszToken,&szEnd,10);        
                break;

            case PT_APPTIME:    // double
                dVal = strtod(szBuffer,&szEnd);
                lpNewPVA[0].Value.at = dVal;                    
                break;        

            case PT_BOOLEAN:    // unsigned short int
                iVal = atoi(szBuffer);
                lpNewPVA[0].Value.b = iVal;         
                break;

            case PT_SYSTIME:    // Special FILETIME
                
                lpSysTime = (LPSYSTEMTIME) PvAlloc(sizeof(SYSTEMTIME) );
                memset(lpSysTime,0,(size_t) sizeof(SYSTEMTIME) );
                
                lpszToken   = strtok(szBuffer,"/");
                lpSysTime->wYear   = atoi(lpszToken);
                lpszToken   = strtok(NULL,"/");
                lpSysTime->wMonth  = atoi(lpszToken);
                lpszToken   = strtok(NULL," ");
                lpSysTime->wDay    = atoi(lpszToken);
                lpszToken   = strtok(NULL,":");
                lpSysTime->wHour   = atoi(lpszToken);
                lpszToken   = strtok(NULL,":");
                lpSysTime->wMinute = atoi(lpszToken);
                lpszToken   = strtok(NULL," ");
                lpSysTime->wSecond = atoi(lpszToken);

                if( ! SystemTimeToFileTime(lpSysTime, &(lpNewPVA[0].Value.ft) ) )
                {
                    MessageBox("Could not convert SystemTimeToFileTime", "Client", MBS_INFO );
                    PvFree(lpSysTime);
                    goto Error;
                }    

                PvFree(lpSysTime);
                break;                


            case PT_STRING8:    // LPSTR
                lpNewPVA[0].Value.lpszA = (LPSTR) PvAllocMore( len+1 ,lpNewPVA);
                memset(lpNewPVA[0].Value.lpszA,0,(size_t)len+1);
                lstrcpy(lpNewPVA[0].Value.lpszA, szBuffer);
                break;                                

            case PT_BINARY:     // binary 00 11 00 10 01 
                // make sure it is in the proper format for putting back in Hex
                if (((len % 3) != 0) && (((len + 1) % 3) != 0))
                {
                    MessageBox("Enter Binary data as follows:  00 00 10 01 11 ",
                                     "Client", MBS_ERROR );
                    goto Error;
                }       
                lpNewPVA[0].Value.bin.lpb = (LPBYTE) PvAllocMore( len/3 ,lpNewPVA);
                len = (len + 1) / 3;
                memset (lpNewPVA[0].Value.bin.lpb, 0,(size_t) len);

                for (i = 0; i < len; i++)
                {
                    if (!AsciiToHex (2, &szBuffer[i * 3],
                                (ULONG *) & lpNewPVA[0].Value.bin.lpb[i]))
                    {
                        MessageBox("CBldPropDlg::OnSet AsciiToHex Failed", "Client", MBS_ERROR );
                        goto Error;
                    }       
                }
                lpNewPVA[0].Value.bin.cb = len;
                break;


            case PT_UNICODE:
    #ifdef WIN32
                if(szBuffer)                        
                    dBufferLen = lstrlen(szBuffer) + 1;     // the number of bytes plus the null char

                // determines the number of Wide char bytes requried
                dBytes = MultiByteToWideChar(   (UINT)      CP_ACP,            
                                                (DWORD)     0,                 
                                                (LPCTSTR)   szBuffer,          
                                                (int)       dBufferLen,                                              
                                                (LPWSTR)    NULL,
                                                (int)       0);
                   
                // allocate memory for the UNICODE string in the lpsPropValue               
                // a wide character is two bytes so the amount of memory required is 2 * number of wide char bytes     
 
                lpNewPVA[0].Value.lpszW = (LPWSTR) PvAllocMore( (dBytes * sizeof(WCHAR)),lpNewPVA ) ;
                
                 // now actually convert the szBuffer edit control STRING8 into a UNICODE                       
                dCopiedBytes = MultiByteToWideChar((UINT)   CP_ACP,            
                                                (DWORD)     0,                 
                                                (LPCTSTR)   szBuffer,          
                                                (int)       dBufferLen,                                              
                                                (LPWSTR)    lpNewPVA[0].Value.lpszW,    
                                                (int)       dBytes );
                          
                dwRet = GetLastError();
                break;
    #endif      
                // else it will fall down here in 16 bit
                MessageBox("Viewer doesn't support setting PT_UNICODE on 16 bit", "Client", MBS_INFO );
                goto Error;

            // SUPPORT IN  FUTURE 
            
            case PT_CLSID:      // Special LPUID
                MessageBox("Viewer doesn't support setting PT_CLSID", "Client", MBS_INFO );
                goto Error;

            case PT_I8:         // Special LARGE_INTEGER
                lpszToken   = strtok(szBuffer," ");
                lpNewPVA[0].Value.li.LowPart =  strtol(lpszToken,&szEnd,10);        

                lpszToken   = strtok(NULL," ");
                lpNewPVA[0].Value.li.HighPart =  strtol(lpszToken,&szEnd,10);        
                break;
                
            case PT_NULL:       // set not data for PT_NULL
                break;

            case PT_OBJECT:     // not supported yet
                break;

            case PT_ERROR:      // Special Scode (ULONG)
                // read data, find out if recognized SCODE, if not, goto error
                
                if(GetID("MAPIErrors",szBuffer,&lVal) )
                    lpNewPVA[0].Value.err = lVal;
                else
                {                                        
                    MessageBox("Could not interpret this String as a MAPI Return code", "Client", MBS_INFO );
                    goto Error;
                }                    
                break;
            
            // ********** MULTI VALUE PROPERTIES
            // Generally read in as follows:  cVals: val1; val2; val3; ...                
                
            case PT_MV_I2:      // array of short ints
    
                ulMVRow = 0;
                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVi.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVi.lpi = (short int *) PvAllocMore(
                            (sizeof(short int) * lpNewPVA[0].Value.MVi.cValues), lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    lpNewPVA[0].Value.MVi.lpi[ulMVRow] = atoi(lpszToken);
                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVi.cValues )
                {
                    lpNewPVA[0].Value.MVi.lpi[ulMVRow] = 0;
                    ulMVRow++;                            
                }
                break;
                
            case PT_MV_LONG:    // array of longs

                ulMVRow = 0;
                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVl.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVl.lpl = (LONG *) PvAllocMore(
                            (sizeof(LONG) * lpNewPVA[0].Value.MVl.cValues),lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    lpNewPVA[0].Value.MVl.lpl[ulMVRow] = strtol(lpszToken,&szEnd,10);
                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVl.cValues )
                {
                    lpNewPVA[0].Value.MVl.lpl[ulMVRow] = 0;
                    ulMVRow++;                            
                }
                break;
                
            case PT_MV_R4:
                ulMVRow = 0;

                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVflt.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVflt.lpflt = (float *) PvAllocMore(
                            (sizeof(float) * lpNewPVA[0].Value.MVflt.cValues),lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    lpNewPVA[0].Value.MVflt.lpflt[ulMVRow] = (float) atof(lpszToken);
                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVflt.cValues )
                {
                    lpNewPVA[0].Value.MVflt.lpflt[ulMVRow] = (float) 0.0;
                    ulMVRow++;                            
                }
            
                break;
                
            case PT_MV_DOUBLE:
                ulMVRow = 0;

                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVdbl.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVdbl.lpdbl = (double *) PvAllocMore(
                            (sizeof(double) * lpNewPVA[0].Value.MVdbl.cValues),lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    lpNewPVA[0].Value.MVdbl.lpdbl[ulMVRow] = (double) strtod(lpszToken,&szEnd);
                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVdbl.cValues )
                {
                    lpNewPVA[0].Value.MVdbl.lpdbl[ulMVRow] = 0;
                    ulMVRow++;                            
                }
            
                break;
                

            case PT_MV_CURRENCY:
                ulMVRow = 0;
                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVcur.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVcur.lpcur = (CURRENCY *) PvAllocMore(
                            (sizeof(CURRENCY) * lpNewPVA[0].Value.MVcur.cValues),lpNewPVA );

                lppszToken = (LPSTR *) PvAlloc(
                            (30 * sizeof(char) * lpNewPVA[0].Value.MVcur.cValues) );

                memset(lpNewPVA[0].Value.MVcur.lpcur,0,
                        (size_t) (sizeof(CURRENCY) * lpNewPVA[0].Value.MVcur.cValues) );

                // rip out remaining rows                
                while( (lppszToken[ulMVRow] = strtok(NULL,";")) != NULL )
                    ulMVRow++;

                for(idx = 0 ; idx < ulMVRow ; idx++)
                {
                    lpszToken   = strtok(lppszToken[idx]," ");
                    lpNewPVA[0].Value.MVcur.lpcur[idx].Lo =  strtol(lpszToken,&szEnd,10);        

                    lpszToken   = strtok(NULL," ");
                    lpNewPVA[0].Value.MVcur.lpcur[idx].Hi =  strtol(lpszToken,&szEnd,10);        
                }

                break;
                
            case PT_MV_APPTIME:
                ulMVRow = 0;
                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVat.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVat.lpat = (double *) PvAllocMore(
                            (sizeof(double) * lpNewPVA[0].Value.MVat.cValues),lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    lpNewPVA[0].Value.MVat.lpat[ulMVRow] = (double) strtod(lpszToken,&szEnd);
                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVat.cValues )
                {
                    lpNewPVA[0].Value.MVat.lpat[ulMVRow] = 0;
                    ulMVRow++;                            
                }
                break;

            case PT_MV_SYSTIME:
                ulMVRow = 0;
                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVft.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVft.lpft = (FILETIME *) PvAllocMore(
                            (sizeof(FILETIME) * lpNewPVA[0].Value.MVft.cValues),lpNewPVA );

                lppszToken = (LPSTR *) PvAlloc(
                            (30 * sizeof(char) * lpNewPVA[0].Value.MVft.cValues) );

                memset(lpNewPVA[0].Value.MVft.lpft,0,
                        (size_t) (sizeof(FILETIME) * lpNewPVA[0].Value.MVft.cValues ));

                // rip out remaining rows                
                while( (lppszToken[ulMVRow] = strtok(NULL,";")) != NULL )
                    ulMVRow++;

                for(idx = 0 ; idx < ulMVRow ; idx++)
                {
                    lpSysTime = (LPSYSTEMTIME) PvAlloc(sizeof(SYSTEMTIME) );
                    memset(lpSysTime,0,(size_t) sizeof(SYSTEMTIME) );
                                       
                    lpszTemp   = strtok(lppszToken[idx],"/");
                    lpSysTime->wYear   = atoi(lpszTemp);
                    lpszTemp   = strtok(NULL,"/");
                    lpSysTime->wMonth  = atoi(lpszTemp);
                    lpszTemp   = strtok(NULL," ");
                    lpSysTime->wDay    = atoi(lpszTemp);
                    lpszTemp   = strtok(NULL,":");
                    lpSysTime->wHour   = atoi(lpszTemp);
                    lpszTemp   = strtok(NULL,":");
                    lpSysTime->wMinute = atoi(lpszTemp);
                    lpszTemp   = strtok(NULL," ");
                    lpSysTime->wSecond = atoi(lpszTemp);

                    if( ! SystemTimeToFileTime(lpSysTime, &(lpNewPVA[0].Value.MVft.lpft[idx]) ) )
                    {
                        MessageBox("Could not convert SystemTimeToFileTime", "Client", MBS_INFO );
                        PvFree(lpSysTime);
                        goto Error;
                    }    

                    PvFree(lpSysTime);
                }
            
                break;
                
            case PT_MV_STRING8:

                ulMVRow = 0;
                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVszA.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVszA.lppszA = (LPSTR *) PvAllocMore(
                            (sizeof(LPSTR) * lpNewPVA[0].Value.MVszA.cValues),lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    
                    lpNewPVA[0].Value.MVszA.lppszA[ulMVRow] = (LPSTR) PvAllocMore(
                                lstrlen(lpszToken) + 1,lpNewPVA);
                    lstrcpy(lpNewPVA[0].Value.MVszA.lppszA[ulMVRow],lpszToken);

                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVszA.cValues )
                {
                    lpNewPVA[0].Value.MVszA.lppszA[ulMVRow] = (LPSTR) PvAllocMore(
                                30 * sizeof(char),lpNewPVA);


                    lstrcpy(lpNewPVA[0].Value.MVszA.lppszA[ulMVRow], "Unknown");

                    ulMVRow++;                            
                }
                break;

               
            case PT_MV_BINARY:

                ulMVRow = 0;
                 // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVbin.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVbin.lpbin = (SBinary *) PvAllocMore(
                            (sizeof(SBinary) * lpNewPVA[0].Value.MVbin.cValues),lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {
                    lenBin = strlen(lpszToken);

                    // make sure it is in the proper format for putting back in Hex
                    if (((lenBin % 3) != 0) && (((lenBin + 1) % 3) != 0))
                    {
                        MessageBox("Enter Binary data as follows:  00 00 10 01 11 ",
                                         "Client", MBS_ERROR );
                        goto Error;
                    }       
                    lpNewPVA[0].Value.MVbin.lpbin[ulMVRow].lpb = (LPBYTE) PvAllocMore( lenBin/3 ,lpNewPVA);
                    lenBin = (lenBin + 1) / 3;
                    memset (lpNewPVA[0].Value.MVbin.lpbin[ulMVRow].lpb, 0,(size_t) lenBin);

                    for (i = 0; i < lenBin; i++)
                    {
                        if (!AsciiToHex (2, &lpszToken[i * 3],
                                    (ULONG *) & lpNewPVA[0].Value.MVbin.lpbin[ulMVRow].lpb[i]))
                        {
                            MessageBox("CBldPropDlg::OnSet AsciiToHex Failed", "Client", MBS_ERROR );
                            goto Error;
                        }       
                    }
                    lpNewPVA[0].Value.MVbin.lpbin[ulMVRow].cb = lenBin;
                    
                    lpszToken = strtok(NULL,szMVSeps);
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVbin.cValues )
                {
                    lpNewPVA[0].Value.MVbin.lpbin[ulMVRow].lpb = (BYTE *) PvAllocMore(
                                30 * sizeof(BYTE),lpNewPVA);

                    memset(lpNewPVA[0].Value.MVbin.lpbin[ulMVRow].lpb,0,(size_t)10*sizeof(BYTE));
                    lpNewPVA[0].Value.MVbin.lpbin[ulMVRow].cb = 10;

                    ulMVRow++;                            
                }
                break;
            
            case PT_MV_UNICODE:

    #ifdef WIN32
                 ulMVRow = 0;
                 // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVszW.cValues =  strtoul(lpszToken,&szEnd,10);        


                lpNewPVA[0].Value.MVszW.lppszW = (LPWSTR *) PvAllocMore(
                            (sizeof(LPWSTR) * lpNewPVA[0].Value.MVszW.cValues),lpNewPVA );

                // rip out first row of multi value prop
                lpszToken = strtok(NULL,szMVSeps);
                            
                // rip out remaining rows                
                while( lpszToken != NULL )
                {

                    if(lpszToken)                        
                        dBufferLen = lstrlen(lpszToken) + 1;     // the number of bytes plus the null char

                    // determines the number of Wide char bytes requried
                    dBytes = MultiByteToWideChar(   (UINT)      CP_ACP,            
                                                    (DWORD)     0,                 
                                                    (LPCTSTR)   lpszToken,          
                                                    (int)       dBufferLen,                                              
                                                    (LPWSTR)    NULL,
                                                    (int)       0);
                       
                    // allocate memory for the UNICODE string in the lpsPropValue               
                    // a wide character is two bytes so the amount of memory required is 2 * number of wide char bytes     
     
                    lpNewPVA[0].Value.MVszW.lppszW[ulMVRow] = 
                            (LPWSTR) PvAllocMore( (dBytes * sizeof(WCHAR)),lpNewPVA ) ;
                    
                     // now actually convert the lpszToken edit control STRING8 into a UNICODE                      
                    dCopiedBytes = MultiByteToWideChar((UINT)   CP_ACP,            
                                                    (DWORD)     0,                 
                                                    (LPCTSTR)   lpszToken,          
                                                    (int)       dBufferLen,                                              
                                                    (LPWSTR)    lpNewPVA[0].Value.MVszW.lppszW[ulMVRow],    
                                                    (int)       dBytes );
                              
                    dwRet = GetLastError();

                    lpszToken = strtok(NULL,szMVSeps);
 
                    ulMVRow++;
                }                
            
                // if all the data was not there, fill in the remainder with bogus stub
                while( ulMVRow < lpNewPVA[0].Value.MVszW.cValues )
                {
                    lpNewPVA[0].Value.MVszW.lppszW[ulMVRow] = (LPWSTR) PvAllocMore(
                                30 * sizeof(WCHAR),lpNewPVA);

                     // now actually convert the lpszToken edit control STRING8 into a UNICODE                      
                    dCopiedBytes = MultiByteToWideChar((UINT)   CP_ACP,            
                                                    (DWORD)     0,                 
                                                    (LPCTSTR)   "UnKnown",          
                                                    (int)       20,                                              
                                                    (LPWSTR)    lpNewPVA[0].Value.MVszW.lppszW[ulMVRow],    
                                                    (int)       dBytes );
                              
                    dwRet = GetLastError();

                    ulMVRow++;                            
                }
                break;
    #endif      
                // else it will fall down here in 16 bit
                MessageBox("Viewer doesn't support setting PT_MV_UNICODE on 16 bit", "Client", MBS_INFO );
                goto Error;



                // SUPPORT IN  FUTURE 
                         
            case PT_MV_CLSID:
                MessageBox("Viewer doesn't support setting PT_MV_CLSID", "Client", MBS_INFO );            
                goto Error;
                
            case PT_MV_I8:
                ulMVRow = 0;
                // determine count of multi values
                lpszToken   = strtok(szBuffer,szMVSeps);
                lpNewPVA[0].Value.MVli.cValues =  strtoul(lpszToken,&szEnd,10);        

                lpNewPVA[0].Value.MVli.lpli = (LARGE_INTEGER *) PvAllocMore(
                            (sizeof(LARGE_INTEGER) * lpNewPVA[0].Value.MVli.cValues),lpNewPVA );

                lppszToken = (LPSTR *) PvAlloc(
                            (30 * sizeof(char) * lpNewPVA[0].Value.MVli.cValues) );

                memset(lpNewPVA[0].Value.MVli.lpli,0,
                        (size_t) (sizeof(LARGE_INTEGER) * lpNewPVA[0].Value.MVli.cValues ));

                // rip out remaining rows                
                while( (lppszToken[ulMVRow] = strtok(NULL,";")) != NULL )
                    ulMVRow++;

                for(idx = 0 ; idx < ulMVRow ; idx++)
                {
                    lpszToken   = strtok(lppszToken[idx]," ");
                    lpNewPVA[0].Value.MVli.lpli[idx].LowPart =  strtol(lpszToken,&szEnd,10);        

                    lpszToken   = strtok(NULL," ");
                    lpNewPVA[0].Value.MVli.lpli[idx].HighPart =  strtol(lpszToken,&szEnd,10);        
                }

                break;
                
            default:
                MessageBox("[Error] UNKNOWN PROPERTY TYPE", "Client", MBS_INFO );                        
                goto Error;
        }

        m_ulNewValues++;
        m_lppNewPropValue[m_ulNewValues - 1] = lpNewPVA;
        
    }
    else  // its a Prop Tag Array
    {
        //Move NEW TAG from ID and TYPE

        m_lpNewPTA->cValues++;
        m_lpNewPTA->aulPropTag[m_lpNewPTA->cValues - 1] = 
                        PROP_TAG((ULONG)lSelectedType, (ULONG)ulIDVal ); 
    
    }     
      
    RedrawBuildProps();     
    
    return;
    
Error: 

    if(lpNewPVA)
    {
        PvFree(lpNewPVA);
        lpNewPVA = NULL;
    }
}

#ifdef WIN16
#pragma optimize("",on)
#endif


/*******************************************************************/
/*                        
 -  CBldPropDlg::
 -  OnCall
 -
 *  Purpose:
 *      Calls SetProps on lpsPropValue array built up in m_lpNewPropValue
 *      and closes dialog
 */
/*******************************************************************/

void CBldPropDlg::OnCall()
{
    ULONG       idx;
    
    // build the lpSelected PropValue

    // ASSERT m_lppNewPropValue != NULL and m_ulNewValue !=0
    if(((!m_lppNewPropValue) || (m_ulNewValues==0)) && (!m_lpNewPTA))
        goto End;        

    m_lpspvaSelected = (LPSPropValue) PvAlloc(m_ulNewValues * sizeof(SPropValue) );
    
    for(idx = 0 ; idx < m_ulNewValues; idx++)
    {
        CopyPropValue(  &(m_lpspvaSelected[idx]),
                        (LPSPropValue) m_lppNewPropValue[idx],
                        m_lpspvaSelected);
        
            
        PvFree(m_lppNewPropValue[idx]);
        m_lppNewPropValue[idx] = NULL;
    }
    PvFree(m_lppNewPropValue);
    m_lppNewPropValue = NULL;
    
    if(m_lpNewPTA) 
    {
        if(m_lpNewPTA->cValues == 0)
        {
            PvFree(m_lpNewPTA);
            m_lpNewPTA = NULL;
        }
    }
    
    m_fCall = TRUE;

End:

    EndDialog(IDCANCEL);
}


/*******************************************************************/
/*
 -  CBldPropDlg::
 -  ~CBldPropDlg
 -
 *  Purpose:
 *      Destructor for class CBldPropDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CBldPropDlg::~CBldPropDlg()
{
    ULONG   idx;

    if(m_lpOldPropValue)
    {
        MAPIFreeBuffer(m_lpOldPropValue);
        m_lpOldPropValue = NULL;
    }

    if(m_lppNewPropValue)
    {
        for(idx = 0 ; idx < m_ulNewValues; idx++)
        {
            PvFree(m_lppNewPropValue[idx]);
            m_lppNewPropValue[idx] = NULL;
        }
        PvFree(m_lppNewPropValue);
        m_lppNewPropValue = NULL;
    }
    
    // lpsPropValue to pass back to user
    PvFree(m_lpspvaSelected);
    m_lpspvaSelected = NULL;
    
    if(m_lpNewPTA)
    {
        PvFree(m_lpNewPTA);
        m_lpNewPTA = NULL;
    }
} 
     
/*******************************************************************/
/*
 -  CBldPropDlg::
 -  OnDelete
 *
 *  Purpose:
 *      Allow user to Help a message with a Help note
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 *  Note: listbox's are zero based, meaning the 1st element is 
 *          in 0th index
 *
 */ 
/*******************************************************************/

void CBldPropDlg::OnDelete()
{
    LONG            lCurSelect      = 0;
    LPSPropValue    lppvaDelete     = NULL;
    ULONG           ulMax           = 0;

    // get property type combo box entry
    lCurSelect = SendDlgItemMessage(IDC_SP_PROPDISPLAY,LB_GETCURSEL,0,0);
    if(lCurSelect == LB_ERR)
    {
        MessageBox("Please Select a Property from Listbox of Selected List to delete", 
                    "OnDelete", MBS_ERROR );
        return;
    }    

    // if it is a PropValueArray
    if(m_fIsPropValueArray) 
    {           
        if(m_ulNewValues == 0)
            return;
            
        lppvaDelete                   = m_lppNewPropValue[lCurSelect];    
        m_lppNewPropValue[lCurSelect] = m_lppNewPropValue[m_ulNewValues - 1];

        if(lppvaDelete)
        {
            PvFree(lppvaDelete);
            lppvaDelete = NULL;
        }        
        m_ulNewValues--;
    }
    else  // its a Prop Tag Array
    {                
        if(m_lpNewPTA->cValues == 0)
            return;

        // zero based index, get numvalues - 1 for max
        ulMax = m_lpNewPTA->cValues - 1;

        m_lpNewPTA->aulPropTag[lCurSelect] = m_lpNewPTA->aulPropTag[ulMax];      
        m_lpNewPTA->cValues--;    
    }     
  
    RedrawBuildProps();     
    
    return;
   
    
}


/*******************************************************************/
/*
 -  CBldPropDlg::
 -  OnDumpPropValsToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CBldPropDlg::OnDumpPropValsToFile()
{
    char                szTag[80];
    char                szFileName[80];
    CStorePropValDlg    StoreProp(this);
    LPTSTR              lpszTemp;
    LPSPropValue        lpspvaTemp = NULL;
    ULONG               idx;        
        
    // find file to open
    lpszTemp = getenv("MAPITEST");

    if(lpszTemp)
        strcpy(szFileName, lpszTemp);
    else
        strcpy(szFileName, "c:\\mapitest");

    strcat(szFileName, "\\data\\propvu.txt");

    // create the tag with braces around it
    strcpy(szTag,"[BLDPROP00001]");

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
    
    if( StoreProp.DoModal() == IDOK )
    {    
        // if it is a prop tag array 
        if(m_lpNewPTA)
        {   
            WritePropTagArray(  StoreProp.m_szFileName,
                            StoreProp.m_szTagID,
                            m_lpNewPTA);
        }
        else        
        {      
            // build selectd list, and dump to file
            lpspvaTemp = (LPSPropValue) PvAlloc(m_ulNewValues * sizeof(SPropValue) );
    
            for(idx = 0 ; idx < m_ulNewValues; idx++)
            {
                CopyPropValue(  &(lpspvaTemp[idx]),
                                (LPSPropValue) m_lppNewPropValue[idx],
                                lpspvaTemp);
            }
            
              
            WritePropValArray( StoreProp.m_szFileName,
                            StoreProp.m_szTagID,
                            m_ulNewValues,
                            lpspvaTemp,
                            StoreProp.m_ulFlags);
            if(lpspvaTemp)
                PvFree(lpspvaTemp);                            
        }                            
    }
    
}

