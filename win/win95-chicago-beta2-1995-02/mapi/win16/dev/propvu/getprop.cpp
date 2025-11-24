/*******************************************************************/
/*
 -  getprop.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CGetPropDlg
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
#include "resource.h"   
#include "propvu.h"  
#include "getprop.h"

/*******************************************************************/
/**************************** GETPROPS *****************************/

/******************* Set Property Message Map *********************/

BEGIN_MESSAGE_MAP(CGetPropDlg, CModalDialog)

    ON_COMMAND(         IDC_STOREPROPS,     OnDumpPropValsToFile)
    ON_COMMAND(         IDC_STORETAGS,      OnDumpPropTagsToFile)
             
END_MESSAGE_MAP()

/********************************************************************/
/*
 -  CGetPropDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CGetPropDlg::OnInitDialog()
{
    HRESULT         hResult         = hrSuccess;
    CGetError       E;
        
    SetWindowText( "lpObj->GetProps()" );

    SendDlgItemMessage(IDC_GP_EXPPROPTAGS,LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_RESETCONTENT,0,0);


    if( (!m_lpPTA)  || (!m_lpPVA) || (!m_cValues))
    {
        MessageBox( "CGetPropDlg::OnInitDialog   (lpsPTA == NULL) || (m_lpPVA == NULL) || (m_cValues == 0)",
                    "Client", MBS_ERROR );
        goto error;
    }           
    if((m_cValues) && (m_lpPVA != NULL))
        DisplayProps();
    else
        goto error;

    return TRUE;    

error:

    if(m_lpPVA)
    {
        MAPIFreeBuffer(m_lpPVA);
        m_lpPVA         = NULL;
        m_cValues       = 0;
    }
    
    return FALSE;
}



/*******************************************************************/
/*
 -  CBldPropDlg::
 -  DisplayProps
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CGetPropDlg::DisplayProps()
{
    char            szBuffer[1024];
    int             idx             = 0;
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;      
    char            szExpectedCVals[30];
    char            szActualCVals[30];


    // multi value props stuff    
    char            szMV[1024];    
    char            szMVSeps[]      = ";:.";
    char            *lpszToken      = NULL;
    char            szTemp[30];
    ULONG           ulMVRow         = 0;
    ULONG           ulMVcValues     = 0;
    char            *szEnd          = NULL;


    char szID[50];
    char szData[512];
    char szType[32];      // Assumes no PropType string longer than 31 chars

        
    // DISPLAY Expected lpPTA->cValues
    wsprintf(szExpectedCVals,"cValues:  %lu",m_lpPTA->cValues);
    SetDlgItemText(IDT_GP_EXPCVALUES,szExpectedCVals);

    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_GP_EXPPROPTAGS,LB_RESETCONTENT,0,0);

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_GP_EXPPROPTAGS,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_GP_EXPPROPTAGS,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );
    
    // DISPLAY Expected PropTagArray
    for(idx = 0; idx < m_lpPTA->cValues; idx++)
    {   
        szID[0]     = '\0' ;
        szType[0]   = '\0' ;
        szBuffer[0] = '\0' ;
            
        if(GetString("PropIDs", PROP_ID(m_lpPTA->aulPropTag[idx]), szID ) )
        {
            lstrcat(szBuffer, szID );               
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpPTA->aulPropTag[idx]) );       
        }

        if( GetString("PropType", PROP_TYPE(m_lpPTA->aulPropTag[idx]), szType) )
        {
            lstrcat(szBuffer, szType);
            lstrcat(szBuffer,"\t");
        }       
        else
        {
            wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpPTA->aulPropTag[idx]) );
            lstrcat(szBuffer,szType);
        }
        dwReturn = SendDlgItemMessage(IDC_GP_EXPPROPTAGS,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    
    }  
   
    // DISPLAY Actual m_cValues
    wsprintf(szActualCVals,"cValues:  %lu",m_cValues);
    SetDlgItemText(IDT_GP_ACTCVALUES,szActualCVals);
    
    // DISPLAY PropValue Array
    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_RESETCONTENT,0,0);

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );
    

    for(idx = 0; idx < m_cValues; idx++)
    {   
        szID[0]     = '\0' ;
        szData[0]   = '\0' ;
        szType[0]   = '\0' ;
        szBuffer[0] = '\0' ;
            
        if(GetString("PropIDs", PROP_ID(m_lpPVA[idx].ulPropTag), szID ) )
        {
            lstrcat(szBuffer, szID );               
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpPVA[idx].ulPropTag) );     
        }

        if( GetString("PropType", PROP_TYPE(m_lpPVA[idx].ulPropTag), szType) )
        {
            lstrcat(szBuffer, szType);
            lstrcat(szBuffer,"\t");
        }       
        else
        {
            wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpPVA[idx].ulPropTag) );
            lstrcat(szBuffer,szType);
        }

        SzGetPropValue(szData,(LPSPropValue) &m_lpPVA[idx]);

        // if it is a MultiValueProperty, parse the output, and add
        //   more than one row for this property
        if( (PROP_TYPE(m_lpPVA[idx].ulPropTag)) & MV_FLAG )
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
            
                dwReturn = SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_ADDSTRING,0,
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
            
                dwReturn = SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_ADDSTRING,0,
                        (LPARAM)szMV);
                ulMVRow++;                            
            }
        }
        else            
        {
            lstrcat(szBuffer,szData);
            dwReturn = SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
        }

    } 

    dwReturn = SendDlgItemMessage(IDC_GP_ACTPROPTAGS,LB_SETCURSEL,(WPARAM) -1 ,0 );
}


/*******************************************************************/
/*
 -  CGetPropDlg::
 -  ~CGetPropDlg
 -
 *  Purpose:
 *      Destructor for class CGetPropDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CGetPropDlg::~CGetPropDlg()
{

} 

/*******************************************************************/
/*
 -  CGetPropDlg::
 -  OnDumpPropValsToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CGetPropDlg::OnDumpPropValsToFile()
{
    char                szTag[80];
    char                szFileName[80];
    CStorePropValDlg    StoreProp(this);
    LPTSTR              lpszTemp;
        
    // find file to open
    lpszTemp = getenv("MAPITEST");

    if(lpszTemp)
        strcpy(szFileName, lpszTemp);
    else
        strcpy(szFileName, "c:\\mapitest");

    strcat(szFileName, "\\data\\propvu.txt");

    // create the tag with braces around it
    strcpy(szTag,"[PROPVAL00001]");

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
    
    if( StoreProp.DoModal() == IDOK )
    {    
        if((m_cValues != 0) && (m_lpPVA != NULL) )
        {
            WritePropValArray( StoreProp.m_szFileName,
                        StoreProp.m_szTagID,
                        m_cValues,
                        m_lpPVA,
                        StoreProp.m_ulFlags);
        }                        
    }
}



/*******************************************************************/
/*
 -  CGetPropDlg::
 -  OnDumpPropTagsToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CGetPropDlg::OnDumpPropTagsToFile()
{
    char                szTag[80];
    char                szFileName[80];
    CStorePropValDlg    StoreProp(this);
    LPTSTR              lpszTemp;
        
    // find file to open
    lpszTemp = getenv("MAPITEST");

    if(lpszTemp)
        strcpy(szFileName, lpszTemp);
    else
        strcpy(szFileName, "c:\\mapitest");

    strcat(szFileName, "\\data\\propvu.txt");

    // create the tag with braces around it
    strcpy(szTag,"[PROPTAG00001]");

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
    
    if( StoreProp.DoModal() == IDOK )
    {    
        // if it is a prop tag array 
        if(m_lpPTA)
        {   
            WritePropTagArray(  StoreProp.m_szFileName,
                            StoreProp.m_szTagID,
                            m_lpPTA);
        }
    }
}

