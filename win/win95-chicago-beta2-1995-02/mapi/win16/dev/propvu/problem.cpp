/*******************************************************************/
/*
 -  problem.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CProblemDlg
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
#include "problem.h"


#define PROBLEM_LISTBOX_TAB1_SIZE    47
#define PROBLEM_LISTBOX_TAB2_SIZE    190


/*******************************************************************/
/**************************** GETPROPS *****************************/

/******************* Fld Message Map ****************************/

BEGIN_MESSAGE_MAP(CProblemDlg, CDialog)

    ON_COMMAND( IDC_PROB_DUMPPROB,  OnDumpProblemsToFile)

END_MESSAGE_MAP()


/********************************************************************/
/*
 -  CProblemDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CProblemDlg::OnInitDialog()
{
    HRESULT         hResult         = hrSuccess;
    CGetError       E;
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;      
        
    SetWindowText( m_Operation.GetBuffer(15) );

    SendDlgItemMessage(IDC_PROB_DISPLAY,LB_RESETCONTENT,0,0);

    if( !m_lpProblems)
    {
        MessageBox( "CProblemDlg::OnInitDialog  lpProblems == NULL",
                    "Client", MBS_ERROR );
        goto error;
    }           

    // load properties into listbox
    dwReturn = SendDlgItemMessage(IDC_PROB_DISPLAY, LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    rgTabStops[0] = PROBLEM_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROBLEM_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_PROB_DISPLAY,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    DisplayProblems();

    return TRUE;    

error:

    return FALSE;
}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  DisplayProblems
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CProblemDlg::DisplayProblems()
{
    char            szBuffer[1024];
    int             idx             = 0;
    DWORD           dwReturn        = 0;      
    char            szCProblems[30];
    char            szIndex[10];
    char            szPropTag[50];
    char            szScError[50];      

        
    // DISPLAY Expected lpPTA->cValues
    wsprintf(szCProblems,"cProblems:  %lu",m_lpProblems->cProblem);
    SetDlgItemText(IDT_PROB_CPROBLEMS,szCProblems);

    szBuffer[0] = '\0' ;

    SendDlgItemMessage(IDC_PROB_DISPLAY,LB_RESETCONTENT,0,0);


    // DISPLAY ALL PROBLEMS         
    for(idx = 0; idx < m_lpProblems->cProblem; idx++)
    {   
        szIndex[0]      = '\0' ;
        szPropTag[0]    = '\0' ;
        szScError[0]    = '\0' ;
            

        wsprintf(szBuffer,"%lu",m_lpProblems->aProblem[idx].ulIndex);
        lstrcat(szBuffer, "\t");


        if(GetString("PropTags", m_lpProblems->aProblem[idx].ulPropTag, szPropTag ) )
        {
            lstrcat(szBuffer, szPropTag );              
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szPropTag,"%#04X\t", m_lpProblems->aProblem[idx].ulPropTag );      
            lstrcat(szBuffer,szPropTag);
        }

        if( GetString("MAPIErrors", m_lpProblems->aProblem[idx].scode, szScError) )
        {
            lstrcat(szBuffer, szScError);
            lstrcat(szBuffer,"\t");
        }       
        else
        {
            wsprintf(szScError,"%#04X\t",  m_lpProblems->aProblem[idx].scode );
            lstrcat(szBuffer,szScError);
        }
        dwReturn = SendDlgItemMessage(IDC_PROB_DISPLAY,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
    
    }  
   
    dwReturn = SendDlgItemMessage(IDC_PROB_DISPLAY,LB_SETCURSEL,(WPARAM) -1 ,0 );
}


/*******************************************************************/
/*
 -  CProblemDlg::
 -  ~CProblemDlg
 -
 *  Purpose:
 *      Destructor for class CProblemDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CProblemDlg::~CProblemDlg()
{

} 

/*******************************************************************/
/*
 -  CPropDlg::
 -  OnStorePropValsToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CProblemDlg::OnDumpProblemsToFile()
{
    char                szTag[80];
    char                szFileName[80];
    LPTSTR              lpszTemp;
    CStorePropValDlg    StoreProp(this);
        
    // find file to open
    lpszTemp = getenv("MAPITEST");

    if(lpszTemp)
        strcpy(szFileName, lpszTemp);
    else
        strcpy(szFileName, "c:\\mapitest");

    strcat(szFileName, "\\data\\propvu.txt");

    // create the tag with braces around it
    strcpy(szTag,"[PROBLEM00001]");       

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
        
    if( StoreProp.DoModal() == IDOK )
    {    
        WriteProblemArray( StoreProp.m_szFileName,
                      StoreProp.m_szTagID,
                      m_lpProblems);
    }
}


