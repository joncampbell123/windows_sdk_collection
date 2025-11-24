/*******************************************************************/
/*
 -  prop.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CPropDlg.  This modal dialog
 *      allows selective setting of properties on new, existing, and
 *      current properties.
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
#include <mapi.h>       
#include <strtbl.h>     
#include <misctool.h>
#include "resource.h"   
#include "mdbview.h" 
#include "oper.h"

/*************************** PropDlg Functions *********************/

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
    CheckDlgButton(IDC_STRING,  1);
    CheckDlgButton(IDC_HEX,     0);
    CheckDlgButton(IDC_DECIMAL, 0);
    CheckDlgButton(IDC_TYPES,   1);
    CheckDlgButton(IDC_DATA,    1);
    CheckDlgButton(IDC_VALUES,  1);
}   
   
/*******************************************************************/
/*
 -  CPropDlg::
 -
 -  OnType
 -  OnData
 -  OnValues
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
    CheckDlgButton(IDC_TYPES, !IsDlgButtonChecked(IDC_TYPES) );
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnData()
{   
    CheckDlgButton(IDC_DATA, !IsDlgButtonChecked(IDC_DATA) );
    RedrawPropTable();
}      

/*******************************************************************/

void CPropDlg::OnValues()
{   
    CheckDlgButton(IDC_VALUES, !IsDlgButtonChecked(IDC_VALUES) );
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnString()
{   
    CheckRadioButton(IDC_STRING,IDC_DECIMAL,IDC_STRING);
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnHex()
{   

    CheckRadioButton(IDC_STRING,IDC_DECIMAL,IDC_HEX);
    RedrawPropTable();
}

/*******************************************************************/

void CPropDlg::OnDecimal()
{   
    CheckRadioButton(IDC_STRING,IDC_DECIMAL,IDC_DECIMAL);
    RedrawPropTable();
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
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;
    LPSPropValue    lpPropValue     = NULL;
    ULONG           ulValues        = 0;
    
    // multi value props stuff    
    char            szMV[1024];    
    char            szMVSeps[]      = ";:.";
    char            *lpszToken      = NULL;
    char            szTemp[30];
    ULONG           ulMVRow         = 0;
    ULONG           ulMVcValues     = 0;
    char            *szEnd          = NULL;

                            // assuming size here
    char szID[50];
    char szData[512];
    char szType[32];      // Assumes no PropType string longer than 31 chars

    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_PROPS,LB_RESETCONTENT,0,0);
    
    hResult = m_lpEntry->GetProps(NULL,0, &ulValues, &lpPropValue);

    wsprintf(m_szLogBuf,"m_lpEntry->GetProps(NULL,0,&ulValues,&lpPropValue)\t SC: %s",
                        GetString( "MAPIErrors", GetScode(hResult), m_szError ) );
    SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,(LPARAM) m_szLogBuf );

    if( HR_FAILED(hResult))    
    {
        MessageBox( m_E.SzError("m_lpEntry->GetProps()", hResult), 
                            "Client", MBS_ERROR );
        return;
    }

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_PROPS,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_PROPS,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );
    
    // determine if string, hex, or decimal and build row of listbox
    for(idx = 0; idx < ulValues; idx++)
    {   
        szID[0]     = '\0' ;
        szData[0]   = '\0' ;
        szType[0]   = '\0' ;
        szBuffer[0] = '\0' ;
            
        if( IsDlgButtonChecked(IDC_VALUES) )
        {
            if( IsDlgButtonChecked(IDC_STRING) )
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
            else if(  IsDlgButtonChecked(IDC_DECIMAL) )
            {
                wsprintf(szBuffer,"%08d\t", PROP_ID(lpPropValue[idx].ulPropTag), szID );
            }
            else if(  IsDlgButtonChecked(IDC_HEX) )
            {
                wsprintf(szBuffer,"%#04X\t", PROP_ID(lpPropValue[idx].ulPropTag) );     
            }               
        }

        if( IsDlgButtonChecked(IDC_TYPES) )  
        {
            if( IsDlgButtonChecked(IDC_STRING) )
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
            else if(  IsDlgButtonChecked(IDC_DECIMAL) )
            {
                wsprintf(szType,"%08d\t", PROP_TYPE(lpPropValue[idx].ulPropTag) );
                lstrcat(szBuffer,szType);
            }
            else if(  IsDlgButtonChecked(IDC_HEX) )
            {
                wsprintf(szType,"%#04X\t", PROP_TYPE(lpPropValue[idx].ulPropTag) );
                lstrcat(szBuffer,szType);
            }
        }           

                    
        if( IsDlgButtonChecked(IDC_DATA) )  
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
                
                    dwReturn = SendDlgItemMessage(IDC_PROPS,LB_ADDSTRING,0,
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
                
                    dwReturn = SendDlgItemMessage(IDC_PROPS,LB_ADDSTRING,0,
                            (LPARAM)szMV);
                    ulMVRow++;                            
                }
            }
            else            
            {
                lstrcat(szBuffer,szData);
                dwReturn = SendDlgItemMessage(IDC_PROPS,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
            }
        }
        else
            dwReturn = SendDlgItemMessage(IDC_PROPS,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    
    }  // end of for loop of ulValues in lpPropValue

    dwReturn = SendDlgItemMessage(IDC_PROPS,LB_SETCURSEL,(WPARAM) -1 ,0 );

    
    if(lpPropValue)
    {
        MAPIFreeBuffer(lpPropValue);
        wsprintf(m_szLogBuf,"MAPIFreeBuffer(%s)", "lpPropValue from GetProps" );
        SendDlgItemMessage(IDC_MAPILOG,LB_ADDSTRING,0,(LPARAM) m_szLogBuf );
    }
}

