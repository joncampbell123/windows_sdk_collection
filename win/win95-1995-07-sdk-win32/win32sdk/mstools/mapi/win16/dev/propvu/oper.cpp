/*******************************************************************/
/*
 -  oper.cpp
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for COperation.
 *      This class is used to bring up a dialog from which the 
 *      user can select data to be used as parameters to operations.
 *      Not all the dialog controls are used in every case.
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
#include "oper.h"

/********************************************************************/

BEGIN_MESSAGE_MAP(COperation, CModalDialog)

    ON_BN_CLICKED(  IDC_OPSFLAG1,       OnFlag1)
    ON_BN_CLICKED(  IDC_OPSFLAG2,       OnFlag2)
    ON_BN_CLICKED(  IDC_OPSFLAG3,       OnFlag3)
    ON_BN_CLICKED(  IDC_OPSFLAG4,       OnFlag4)
    ON_BN_CLICKED(  IDC_OPSFLAG5,       OnFlag5)
    ON_BN_CLICKED(  IDC_OPSFLAG6,       OnFlag6)
         
END_MESSAGE_MAP()

/********************************************************************/
/*
 -  COperation::
 -  OnInitDialog
 -
 *  Purpose:
 *      Initialize the dialog text and data in controls
 *      Data members are set prior to calling this function
 *      so that this function merely brings up UI with appropriate
 *      controls reflecting the initialized data members.
 */
 /********************************************************************/

BOOL COperation::OnInitDialog()
{
    HRESULT         hResult         = hrSuccess;
    char            szBuff[80];
    DWORD           dwRet           = 0;
    int             dCount          = 0;
    int             idx             = 0;
    CString         szCBElement;
    
    SetWindowText( m_CurrentOperation.GetBuffer(25) );

    // describe controls
    SetDlgItemText(IDT_OPSCB1,m_CBText1);
    SetDlgItemText(IDT_OPSCB2,m_CBText2);
    SetDlgItemText(IDT_OPSCB3,m_CBText3);

    SetDlgItemText(IDT_OPSEDIT1,m_EditText1);
    SetDlgItemText(IDT_OPSEDIT2,m_EditText2);
    SetDlgItemText(IDT_OPSEDIT3,m_EditText3);

    SetDlgItemText(IDC_OPSFLAG1,m_FlagText1);
    SetDlgItemText(IDC_OPSFLAG2,m_FlagText2);
    SetDlgItemText(IDC_OPSFLAG3,m_FlagText3);
    SetDlgItemText(IDC_OPSFLAG4,m_FlagText4);
    SetDlgItemText(IDC_OPSFLAG5,m_FlagText5);
    SetDlgItemText(IDC_OPSFLAG6,m_FlagText6);

    // determine which fields are enabled/disabled
    if( !GetDlgItemText(IDT_OPSCB1,szBuff,80) )
    {        
        GetDlgItem(IDC_OPSCB1)->EnableWindow(FALSE);
        GetDlgItem(IDT_OPSCB1)->EnableWindow(FALSE);
    }    
    if( !GetDlgItemText(IDT_OPSCB2,szBuff,80) )
    {
        GetDlgItem(IDC_OPSCB2)->EnableWindow(FALSE);
        GetDlgItem(IDT_OPSCB2)->EnableWindow(FALSE);
    }
    if( !GetDlgItemText(IDT_OPSCB3,szBuff,80) )
    {
        GetDlgItem(IDC_OPSCB3)->EnableWindow(FALSE);
        GetDlgItem(IDT_OPSCB3)->EnableWindow(FALSE);
    }
    if( !GetDlgItemText(IDT_OPSEDIT1,szBuff,80) )
    {
        GetDlgItem(IDC_OPSEDIT1)->EnableWindow(FALSE);
        GetDlgItem(IDT_OPSEDIT1)->EnableWindow(FALSE);
    }
    if( !GetDlgItemText(IDT_OPSEDIT2,szBuff,80) )
    {
        GetDlgItem(IDC_OPSEDIT2)->EnableWindow(FALSE);
        GetDlgItem(IDT_OPSEDIT2)->EnableWindow(FALSE);
    }
    if( !GetDlgItemText(IDT_OPSEDIT3,szBuff,80) )
    {
        GetDlgItem(IDC_OPSEDIT3)->EnableWindow(FALSE);
        GetDlgItem(IDT_OPSEDIT3)->EnableWindow(FALSE);
    }
    
    if( !GetDlgItemText(IDC_OPSFLAG1,szBuff,80) )
        GetDlgItem(IDC_OPSFLAG1)->EnableWindow(FALSE);
    else
        CheckDlgButton(IDC_OPSFLAG1,    0);
        
    if( !GetDlgItemText(IDC_OPSFLAG2,szBuff,80) )
        GetDlgItem(IDC_OPSFLAG2)->EnableWindow(FALSE);
    else
        CheckDlgButton(IDC_OPSFLAG2,    0);

    if( !GetDlgItemText(IDC_OPSFLAG3,szBuff,80) )
        GetDlgItem(IDC_OPSFLAG3)->EnableWindow(FALSE);
    else
        CheckDlgButton(IDC_OPSFLAG3,    0);

    if( !GetDlgItemText(IDC_OPSFLAG4,szBuff,80) )
        GetDlgItem(IDC_OPSFLAG4)->EnableWindow(FALSE);    
    else
        CheckDlgButton(IDC_OPSFLAG4,    0);

    if( !GetDlgItemText(IDC_OPSFLAG5,szBuff,80) )
        GetDlgItem(IDC_OPSFLAG5)->EnableWindow(FALSE);    
    else
        CheckDlgButton(IDC_OPSFLAG5,    0);

    if( !GetDlgItemText(IDC_OPSFLAG6,szBuff,80) )
        GetDlgItem(IDC_OPSFLAG6)->EnableWindow(FALSE);    
    else
        CheckDlgButton(IDC_OPSFLAG6,    0);
    

    // initialize data inside Combo boxes  
    if(dCount = m_CBContents1.GetSize() )
    {
        for( idx = 0 ; idx < dCount ; idx++)
        {
            szCBElement = m_CBContents1.GetAt(idx);
            dwRet = SendDlgItemMessage(IDC_OPSCB1, CB_ADDSTRING,0 ,(LPARAM)szCBElement.GetBuffer(10) );
        }        
        dwRet = SendDlgItemMessage(IDC_OPSCB1, CB_SETCURSEL,0 ,0 );       
    }

    if( dCount = m_CBContents2.GetSize() )
    {
        for( idx = 0 ; idx < dCount ; idx++)
        {
            szCBElement = m_CBContents2.GetAt(idx);
            dwRet = SendDlgItemMessage(IDC_OPSCB2, CB_ADDSTRING,0 ,(LPARAM)szCBElement.GetBuffer(10) );
        }        
        dwRet = SendDlgItemMessage(IDC_OPSCB2, CB_SETCURSEL,0 ,0 );       
    }

    if( dCount = m_CBContents3.GetSize() )
    {
        for( idx = 0 ; idx < dCount ; idx++)
        {
            szCBElement = m_CBContents3.GetAt(idx);
            dwRet = SendDlgItemMessage(IDC_OPSCB3, CB_ADDSTRING,0 ,(LPARAM)szCBElement.GetBuffer(10) );
        }        
        dwRet = SendDlgItemMessage(IDC_OPSCB3, CB_SETCURSEL,0 ,0 );       
    }

    // initialize data in edit controls 
    SetDlgItemText(IDC_OPSEDIT1,m_EditDefault1);
    SetDlgItemText(IDC_OPSEDIT2,m_EditDefault2);
    SetDlgItemText(IDC_OPSEDIT3,m_EditDefault3);

    return TRUE;
}

/*******************************************************************/
/*
 -  COperation::
 -  OnOK
 -
 *  Purpose:
 *      Take data in edit controls, comboboxes, and flags and
 *      put in format to be passed back to the calling function
 *      i.e. put in strings and boolean flags
 *  
 */
/*******************************************************************/

void COperation::OnOK()
{
    LONG    lCurSelect      = 0;
    LONG    dRet            = 0;

    // get combo box selection and put in data member strings
    lCurSelect = SendDlgItemMessage(IDC_OPSCB1,CB_GETCURSEL,0,0);
    dRet = SendDlgItemMessage(IDC_OPSCB1,CB_GETLBTEXT,(WPARAM)lCurSelect,(LPARAM)m_szCB1);
    m_szCB1[dRet] = '\0';

    lCurSelect = SendDlgItemMessage(IDC_OPSCB2,CB_GETCURSEL,0,0);
    dRet = SendDlgItemMessage(IDC_OPSCB2,CB_GETLBTEXT,(WPARAM)lCurSelect,(LPARAM)m_szCB2);
    m_szCB2[dRet] = '\0';

    lCurSelect = SendDlgItemMessage(IDC_OPSCB3,CB_GETCURSEL,0,0);
    dRet = SendDlgItemMessage(IDC_OPSCB3,CB_GETLBTEXT,(WPARAM)lCurSelect,(LPARAM)m_szCB3);
    m_szCB3[dRet] = '\0';

    // get edit control selection and put in data member strings
    *(WORD *)m_szEdit1 = sizeof(m_szEdit1) -1;    // first char has buffer length   
    dRet = SendDlgItemMessage(IDC_OPSEDIT1,EM_GETLINE,0,(LPARAM)m_szEdit1);
    m_szEdit1[dRet] = '\0';

    *(WORD *)m_szEdit2 = sizeof(m_szEdit2) -1;    // first char has buffer length   
    dRet = SendDlgItemMessage(IDC_OPSEDIT2,EM_GETLINE,0,(LPARAM)m_szEdit2);
    m_szEdit2[dRet] = '\0';

    *(WORD *)m_szEdit3 = sizeof(m_szEdit3) -1;    // first char has buffer length   
    dRet = SendDlgItemMessage(IDC_OPSEDIT3,EM_GETLINE,0,(LPARAM)m_szEdit3);
    m_szEdit3[dRet] = '\0';
    
    // get checkbox state and put in boolean data members
    if( IsDlgButtonChecked(IDC_OPSFLAG1) )                
        m_bFlag1 = TRUE;

    if( IsDlgButtonChecked(IDC_OPSFLAG2) )                
        m_bFlag2 = TRUE;

    if( IsDlgButtonChecked(IDC_OPSFLAG3) )                
        m_bFlag3 = TRUE;

    if( IsDlgButtonChecked(IDC_OPSFLAG4) )                
        m_bFlag4 = TRUE;

    if( IsDlgButtonChecked(IDC_OPSFLAG5) )                
        m_bFlag5 = TRUE;

    if( IsDlgButtonChecked(IDC_OPSFLAG6) )                
        m_bFlag6 = TRUE;

    EndDialog(IDOK);
}

/*******************************************************************/
/*
 -  COperation::
 -  OnCancel
 -
 *  Purpose:
 *      Closes the Table viewer dialog.
 *
 */
/*******************************************************************/

void COperation::OnCancel()
{
    EndDialog(IDCANCEL);
}

/*******************************************************************/
/*
 -  COperation::
 -  OnFlag*
 -
 *  Purpose:
 *      Toggles the state of the flag check
 *
 */
/*******************************************************************/

void COperation::OnFlag1()
{   
    CheckDlgButton(IDC_OPSFLAG1, !IsDlgButtonChecked(IDC_OPSFLAG1) );
}

void COperation::OnFlag2()
{   
    CheckDlgButton(IDC_OPSFLAG2, !IsDlgButtonChecked(IDC_OPSFLAG2) );
}

void COperation::OnFlag3()
{   
    CheckDlgButton(IDC_OPSFLAG3, !IsDlgButtonChecked(IDC_OPSFLAG3) );
}

void COperation::OnFlag4()
{
    CheckDlgButton(IDC_OPSFLAG4, !IsDlgButtonChecked(IDC_OPSFLAG4) );
}
void COperation::OnFlag5()
{
    CheckDlgButton(IDC_OPSFLAG5, !IsDlgButtonChecked(IDC_OPSFLAG5) );
}
void COperation::OnFlag6()
{
    CheckDlgButton(IDC_OPSFLAG6, !IsDlgButtonChecked(IDC_OPSFLAG6) );
}


