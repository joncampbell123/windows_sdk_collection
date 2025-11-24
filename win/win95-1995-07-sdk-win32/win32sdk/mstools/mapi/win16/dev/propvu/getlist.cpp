/*******************************************************************/
/*
 -  getlist.cpp
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CGetPropListDlg
 */
/*******************************************************************/
                                                   
#undef  CINTERFACE      // C++ calling convention for mapi calls

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
#include <strtbl.h>     
#include <misctool.h>
#include "resource.h"   
#include "propvu.h"  
#include "getlist.h"


/*******************************************************************/
/**************************** GETPROPLIST **************************/

/********************************************************************/
/*
 -  CGetPropListDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CGetPropListDlg::OnInitDialog()
{
    HRESULT         hResult         = hrSuccess;
    CGetError       E;
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;      
        
    SendDlgItemMessage(IDC_LIST_DISPLAY,LB_RESETCONTENT,0,0);


    if( !m_lpPropTagArray)
    {
        MessageBox( "CGetPropListDlg::OnInitDialog  lpPropTagArray == NULL",
                    "Client", MBS_ERROR );
        goto error;
    }           


    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE + 20 ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_LIST_DISPLAY,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    DisplayPropList();

    return TRUE;    

error:

    return FALSE;
}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  DisplayPropList
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CGetPropListDlg::DisplayPropList()
{
    char            szBuffer[1024];
    int             idx             = 0;
    DWORD           dwReturn        = 0;      
    char            szCValues[30];
    char szID[50];
    char szType[32];      // Assumes no PropType string longer than 31 chars

        
    // DISPLAY Expected lpPTA->cValues
    wsprintf(szCValues,"cValues:  %lu",m_lpPropTagArray->cValues);
    SetDlgItemText(IDT_LIST_CVALUES,szCValues);

    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_LIST_DISPLAY,LB_RESETCONTENT,0,0);
           
    for(idx = 0; idx < m_lpPropTagArray->cValues; idx++)
    {   
        szID[0]     = '\0' ;
        szType[0]   = '\0' ;
        szBuffer[0] = '\0' ;
            
        if(GetString("PropIDs", PROP_ID(m_lpPropTagArray->aulPropTag[idx]), szID ) )
        {
            lstrcat(szBuffer, szID );               
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpPropTagArray->aulPropTag[idx]) );      
        }

        if( GetString("PropType", PROP_TYPE(m_lpPropTagArray->aulPropTag[idx]), szType) )
        {
            lstrcat(szBuffer, szType);
            lstrcat(szBuffer,"\t");
        }       
        else
        {
            wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpPropTagArray->aulPropTag[idx]) );
            lstrcat(szBuffer,szType);
        }
        dwReturn = SendDlgItemMessage(IDC_LIST_DISPLAY,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    
    }  
    
    dwReturn = SendDlgItemMessage(IDC_LIST_DISPLAY,LB_SETCURSEL,(WPARAM) -1 ,0 );

}

/*******************************************************************/
/*
 -  CGetPropListDlg::
 -  ~CGetPropListDlg
 -
 *  Purpose:
 *      Destructor for class CGetPropListDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CGetPropListDlg::~CGetPropListDlg()
{

} 

