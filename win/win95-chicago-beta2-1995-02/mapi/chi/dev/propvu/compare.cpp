/*******************************************************************/
/*
 -  compare.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CCompareDlg
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
#include "compare.h"

/*******************************************************************/
/**************************** GETPROPS *****************************/

/******************* Set Property Message Map *********************/

BEGIN_MESSAGE_MAP(CCompareDlg, CModalDialog)

    ON_COMMAND(         IDC_COMP_DUMP_BEFORE,     OnDumpPropValsBeforeToFile)
    ON_COMMAND(         IDC_COMP_DUMP_AFTER,      OnDumpPropValsAfterToFile)
             
END_MESSAGE_MAP()

/********************************************************************/
/*
 -  CCompareDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CCompareDlg::OnInitDialog()
{       
    SetWindowText( m_Operation.GetBuffer(40) );

    SendDlgItemMessage(IDC_COMP_BEFORE,LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_COMP_AFTER, LB_RESETCONTENT,0,0);

    if( (!m_lpspvaBefore) || (!m_lpspvaAfter) || (!m_cValuesBefore) || (!m_cValuesAfter))
    {
        MessageBox( "CCompareDlg::OnInitDialog  (!m_lpspvaBefore) || (!m_lpspvaAfter) || (!m_cValuesBefore) || (!m_cValuesAfter) ",
                    "Client", MBS_ERROR );
        goto error;
    }           

    DisplayProps();

    return TRUE;    

error:

    if(m_lpspvaBefore)
    {
        MAPIFreeBuffer(m_lpspvaBefore);
        m_lpspvaBefore          = NULL;
        m_cValuesBefore         = 0;
    }
    
    if(m_lpspvaAfter)
    {
        MAPIFreeBuffer(m_lpspvaAfter);
        m_lpspvaAfter           = NULL;
        m_cValuesAfter          = 0;
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

void CCompareDlg::DisplayProps()
{
    char            szBuffer[1024];
    int             idx             = 0;
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;      
    char            szExpectedCVals[30];
    char            szActualCVals[30];

    char szID[50];
    char szData[512];
    char szType[32];      // Assumes no PropType string longer than 31 chars

    
    // BEFORE DISPLAY
        
    // DISPLAY Actual m_cValues
    wsprintf(szActualCVals,"cValues:  %lu",m_cValuesBefore);
    SetDlgItemText(IDT_COMP_CVALBEFORE,szActualCVals);
    
    // DISPLAY PropValue Array
    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_COMP_BEFORE,LB_RESETCONTENT,0,0);

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_COMP_BEFORE,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_COMP_BEFORE,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );
    
    for(idx = 0; idx < m_cValuesBefore; idx++)
    {   
        szID[0]     = '\0' ;
        szData[0]   = '\0' ;
        szType[0]   = '\0' ;
        szBuffer[0] = '\0' ;
            
        if(GetString("PropIDs", PROP_ID(m_lpspvaBefore[idx].ulPropTag), szID ) )
        {
            lstrcat(szBuffer, szID );               
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpspvaBefore[idx].ulPropTag) );      
        }

        if( GetString("PropType", PROP_TYPE(m_lpspvaBefore[idx].ulPropTag), szType) )
        {
            lstrcat(szBuffer, szType);
            lstrcat(szBuffer,"\t");
        }       
        else
        {
            wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpspvaBefore[idx].ulPropTag) );
            lstrcat(szBuffer,szType);
        }
        
        SzGetPropValue(szData,(LPSPropValue) &m_lpspvaBefore[idx]);
        lstrcat(szBuffer,szData);

        dwReturn = SendDlgItemMessage(IDC_COMP_BEFORE,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    
    } 

    // AFTER DISPLAY
    
    
    // DISPLAY Actual m_cValues
    wsprintf(szActualCVals,"cValues:  %lu",m_cValuesAfter);
    SetDlgItemText(IDC_COMP_CVALAFTER,szActualCVals);
    
    // DISPLAY PropValue Array
    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_COMP_AFTER,LB_RESETCONTENT,0,0);

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_COMP_AFTER,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_COMP_AFTER,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );
    
    for(idx = 0; idx < m_cValuesAfter; idx++)
    {   
        szID[0]     = '\0' ;
        szData[0]   = '\0' ;
        szType[0]   = '\0' ;
        szBuffer[0] = '\0' ;
            
        if(GetString("PropIDs", PROP_ID(m_lpspvaAfter[idx].ulPropTag), szID ) )
        {
            lstrcat(szBuffer, szID );               
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpspvaAfter[idx].ulPropTag) );       
        }

        if( GetString("PropType", PROP_TYPE(m_lpspvaAfter[idx].ulPropTag), szType) )
        {
            lstrcat(szBuffer, szType);
            lstrcat(szBuffer,"\t");
        }       
        else
        {
            wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpspvaAfter[idx].ulPropTag) );
            lstrcat(szBuffer,szType);
        }
        
        SzGetPropValue(szData,(LPSPropValue) &m_lpspvaAfter[idx]);
        lstrcat(szBuffer,szData);

        dwReturn = SendDlgItemMessage(IDC_COMP_AFTER,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    
    } 

    dwReturn = SendDlgItemMessage(IDC_COMP_AFTER,LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwReturn = SendDlgItemMessage(IDC_COMP_BEFORE,LB_SETCURSEL,(WPARAM) -1 ,0 );
}


/*******************************************************************/
/*
 -  CCompareDlg::
 -  ~CCompareDlg
 -
 *  Purpose:
 *      Destructor for class CCompareDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CCompareDlg::~CCompareDlg()
{

} 



/*******************************************************************/
/*
 -  CCompareDlg::
 -  OnDumpPropValsBeforeToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CCompareDlg::OnDumpPropValsBeforeToFile()
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
    strcpy(szTag,"[PROPS BEFORE 0001]");

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
    
    if( StoreProp.DoModal() == IDOK )
    {    
        if((m_cValuesBefore != 0) && (m_lpspvaBefore != NULL) )
        {
            WritePropValArray( StoreProp.m_szFileName,
                        StoreProp.m_szTagID,
                        m_cValuesBefore,
                        m_lpspvaBefore,
                        StoreProp.m_ulFlags);
        }                        
    }
}

/*******************************************************************/
/*
 -  CCompareDlg::
 -  OnDumpPropValsAfterToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CCompareDlg::OnDumpPropValsAfterToFile()
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
    strcpy(szTag,"[PROPS AFTER 0001]");

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
    
    if( StoreProp.DoModal() == IDOK )
    {    
        if((m_cValuesAfter != 0) && (m_lpspvaAfter != NULL) )
        {
            WritePropValArray( StoreProp.m_szFileName,
                        StoreProp.m_szTagID,
                        m_cValuesAfter,
                        m_lpspvaAfter,
                        StoreProp.m_ulFlags);
        }                        
    }
}

