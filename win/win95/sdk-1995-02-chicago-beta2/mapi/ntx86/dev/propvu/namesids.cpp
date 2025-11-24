/*******************************************************************/
/*
 -  NamesIDs.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CNamesIDsDlg
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
#include "namesids.h"


/*******************************************************************/
/**************************** GETPROPID **************************/

/********************************************************************/
/*
 -  CNamesIDsDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CNamesIDsDlg::OnInitDialog()
{
    HRESULT         hResult         = hrSuccess;
    CGetError       E;
    int             rgTabStops[6];
    DWORD           dwReturn        = 0;      
    char            szBuffer[80];
    
        
    SendDlgItemMessage(IDC_NAMEID_DISPLAY,LB_RESETCONTENT,0,0);

    if( !m_lpPTA)
    {
        MessageBox( "CNamesIDsDlg::OnInitDialog  lpPTA == NULL",
                    "Client", MBS_ERROR );
        goto error;
    }           

    if(m_lpPTA->cValues != m_ulPropNames)
    {

        wsprintf(szBuffer,"m_lpPTA->cValues == %lu != m_ulPropNames == %lu",
                m_lpPTA->cValues,m_ulPropNames);
        MessageBox( "CNamesIDsDlg::OnInitDialog  m_lpPTA->cValues != m_ulPropNames",
              "Client", MBS_ERROR );
        MessageBox( szBuffer,"Client", MBS_ERROR );
    }

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_NAMEID_DISPLAY, LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = 32;
    rgTabStops[1] = 180;

    dwReturn = SendDlgItemMessage(IDC_NAMEID_DISPLAY,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    DisplayNamesIDs();

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

void CNamesIDsDlg::DisplayNamesIDs()
{
    char            szTemp[200];
    char            szTemp1[400];
    char            szBuffer[2048];
    int             idx             = 0;
    DWORD           dwReturn        = 0;      
    char            szID[50];
    DWORD           dwRet               = 0;
#ifdef WIN16
    WCHAR *pwsz;
    ULONG   cch;
#endif

        
    szBuffer[0] = '\0' ;
    SendDlgItemMessage( IDC_NAMEID_DISPLAY,LB_RESETCONTENT,0,0);    
           
    for(idx = 0; idx < m_lpPTA->cValues; idx++)
    {   
        szID[0]     = '\0' ;
        szBuffer[0] = '\0' ;
        szTemp[0]   = '\0' ;
        
        // PROPID
        wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpPTA->aulPropTag[idx]) );       

        // lpPropSetGuid
        // SzIIDToString takes care of case where lpPropSetGuid == NULL
//        SzIIDToString(szTemp, m_lpPropSetGuid);
        SzIIDToString(szTemp, (*m_lppMAPINameID[idx]).lpguid);

        lstrcat(szBuffer,szTemp);
        lstrcat(szBuffer,"\t");
        
        // Mapped Name
        if( !m_lppMAPINameID[idx] )
            continue;
        
        switch( (*m_lppMAPINameID[idx]).ulKind )
        {
            case MNID_ID:
            {
                 wsprintf( szTemp1, "(*lppPropNames[%d]).Kind.lID = %ld", idx, (*m_lppMAPINameID[idx]).Kind.lID );
                 lstrcat(szBuffer,szTemp1);
            }
            break;
            
            case MNID_STRING:
            {
#ifdef WIN16
                pwsz = (*m_lppMAPINameID[idx]).Kind.lpwstrName;
    
                cch = 0;
                while (*pwsz++ != 0)
                {
                    szTemp1[cch]=(char)*pwsz;
                    cch += 1;
                }
                lstrcat(szBuffer,szTemp1);
            
#else
                if( (*m_lppMAPINameID[idx]).Kind.lpwstrName )
                {
                    wsprintf( szTemp1, "(*lppPropNames[%d]).Kind.lpwstrName = %ws", idx, (*m_lppMAPINameID[idx]).Kind.lpwstrName );
                    lstrcat(szBuffer,szTemp1);
                }
                else
                {
                    wsprintf( szTemp1, "(*lppPropNames[%d]) = NULL", idx );
                    lstrcat(szBuffer,szTemp1);
                }
#endif
            }
            break;
            
            default:
            {
                wsprintf( szTemp1, "(*lppPropNames[idx]).ulKind = %lu  Invalid ID type in MAPINAMEID", (*m_lppMAPINameID[idx]).ulKind );  
                lstrcat(szBuffer,szTemp1);
            }
        }

        SendDlgItemMessage(IDC_NAMEID_DISPLAY, LB_ADDSTRING, 0, (LPARAM)szBuffer);
    }  
    
    dwReturn = SendDlgItemMessage(IDC_NAMEID_DISPLAY,LB_SETCURSEL,(WPARAM) -1 ,0 );

}

/*******************************************************************/
/*
 -  CNamesIDsDlg::
 -  ~CNamesIDsDlg
 -
 *  Purpose:
 *      Destructor for class CNamesIDsDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CNamesIDsDlg::~CNamesIDsDlg()
{

} 

