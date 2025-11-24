/*******************************************************************/
/*
 -  iid.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CIntefaceDlg
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
#include "iid.h"



/*******************************************************************/
/**************************** INTERFACE   **************************/


BEGIN_MESSAGE_MAP(CInterfaceDlg, CDialog)

    ON_COMMAND( IDC_INTERFACE_ADD,      OnAddInterface)
    ON_COMMAND( IDC_INTERFACE_REMOVE,   OnRemoveInterface)

END_MESSAGE_MAP()


/********************************************************************/
/*
 -  CIntefaceDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CInterfaceDlg::OnInitDialog()
{
    DWORD           dwReturn        = 0;            

    SendDlgItemMessage(IDC_INTERFACE_SELECTED,LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_INTERFACE_AVAILABLE,LB_RESETCONTENT,0,0);
    
    SetWindowText( "Build IID array to Exclude in lpObj->CopyTo() operation" );

    m_ciidSelect = 0;
    
    DisplayInterfaces();

    return TRUE;    

}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  DisplayInterfaces
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CInterfaceDlg::DisplayInterfaces()
{
    int             idx             = 0;
    DWORD           dwReturn        = 0;      
    
    SendDlgItemMessage(IDC_INTERFACE_SELECTED,LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_INTERFACE_AVAILABLE,LB_RESETCONTENT,0,0);

    // AVAILABLE LIST
    for(idx = 0; idx < NUM_INTERFACES; idx++)
    {           
        dwReturn = SendDlgItemMessage(IDC_INTERFACE_AVAILABLE,LB_ADDSTRING,0,
                                (LPARAM) AllIIDs[idx].lpsz);
    }


    // SELECTED LIST
    for(idx = 0; idx < m_ciidSelect ; idx++)
    {           
        // get string associated with interface
        dwReturn = SendDlgItemMessage(IDC_INTERFACE_SELECTED,LB_ADDSTRING,0,
                                (LPARAM) m_rgszSelected[idx]);
    }
    
    dwReturn = SendDlgItemMessage(IDC_INTERFACE_AVAILABLE,LB_SETCURSEL,(WPARAM) -1 ,0 );

}

/*******************************************************************/
/*
 -  CInterfaceDlg::
 -  ~CInterfaceDlg
 -
 *  Purpose:
 *      Destructor for class CInterfaceDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CInterfaceDlg::~CInterfaceDlg()
{

} 


/*******************************************************************/
/*
 -  CInterfaceDlg::
 -  OnAdd
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CInterfaceDlg::OnAddInterface()
{
    LONG    lSelection  = -1;
    
    lSelection = SendDlgItemMessage(IDC_INTERFACE_AVAILABLE,LB_GETCURSEL,0,0 );
    if(lSelection == LB_ERR)
    {
        MessageBox( "Please Select an Interface from the Available Interfaces to Add", 
                "Client", MBS_INFO );
        return;
    }
   
    m_ciidSelect++;
    
    CopyGuid( &(m_rgiidSelect[ m_ciidSelect -1]) , AllIIDs[lSelection].lpInterface);

    m_rgszSelected[m_ciidSelect -1] = (char *) PvAlloc(lstrlen(AllIIDs[lSelection].lpsz));        
    lstrcpy(m_rgszSelected[m_ciidSelect -1], AllIIDs[lSelection].lpsz);

    DisplayInterfaces();

} 



/*******************************************************************/
/*
 -  CInterfaceDlg::
 -  OnRemove
 -
 *  Purpose:
 *
 */
/*******************************************************************/

void CInterfaceDlg::OnRemoveInterface()
{
    LONG    lSelection  = -1;
    
    lSelection = SendDlgItemMessage(IDC_INTERFACE_SELECTED,LB_GETCURSEL,0,0 );
    if(lSelection == LB_ERR)
    {
        MessageBox( "Please Select an Interface from the Selected Interfaces to Remove",
                 "Client", MBS_INFO );
        return;
    }
  
    m_rgszSelected[lSelection] = m_rgszSelected[m_ciidSelect - 1];
    lstrcpy(m_rgszSelected[lSelection],m_rgszSelected[m_ciidSelect - 1]);
    m_ciidSelect--;

    DisplayInterfaces();

} 

