/*******************************************************************/
/*
 -  Results.cpp
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains member functions for CResultsDlg
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
#include "results.h"

/*******************************************************************/
/**************************** GETPROPS *****************************/

/******************* Set Property Message Map *********************/

BEGIN_MESSAGE_MAP(CResultsDlg, CModalDialog)

    ON_COMMAND(         IDC_RES_DUMP_BEFORE,     OnDumpPropValsBeforeToFile)
    ON_COMMAND(         IDC_RES_DUMP_AFTER,      OnDumpPropValsAfterToFile)
    ON_COMMAND(         IDC_RES_DUMP_PROBLEMS,   OnDumpProblemsToFile)
    ON_COMMAND(         IDC_RES_DUMP_MODIFY,     OnDumpPropsModifyToFile)
    ON_LBN_DBLCLK(      IDC_RES_MOD,             OnSelectMod )
    ON_LBN_DBLCLK(      IDC_RES_BEFORE,          OnSelectBefore )
    ON_LBN_DBLCLK(      IDC_RES_AFTER,           OnSelectAfter )
    ON_LBN_DBLCLK(      IDC_RES_PROBLEMS,        OnSelectProblem )
                 
END_MESSAGE_MAP()

/********************************************************************/
/*
 -  CResultsDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CResultsDlg::OnInitDialog()
{       
    int             rgTabStops[4];
    DWORD           dwReturn        = 0;      

    SetWindowText( m_Operation.GetBuffer(40) );

    SendDlgItemMessage(IDC_RES_BEFORE,      LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_RES_AFTER,       LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_RES_MOD,      LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_RES_PROBLEMS,    LB_RESETCONTENT,0,0);


    if( (!m_lpspvaBefore) || (!m_lpspvaAfter) || (!m_cValuesBefore) || (!m_cValuesAfter))
    {
        MessageBox( "CResultsDlg::OnInitDialog  (!m_lpspvaBefore) || (!m_lpspvaAfter) || (!m_cValuesBefore) || (!m_cValuesAfter) ",
                    "Client", MBS_ERROR );
        goto error;
    }           
    
    // set tab stops for all listboxes
    rgTabStops[0] = PROBLEM_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROBLEM_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS, LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    rgTabStops[0] = PROP_LISTBOX_TAB1_SIZE ;
    rgTabStops[1] = PROP_LISTBOX_TAB2_SIZE ;

    dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    dwReturn = SendDlgItemMessage(IDC_RES_AFTER,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    dwReturn = SendDlgItemMessage(IDC_RES_AFTER,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );

    dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETHORIZONTALEXTENT,
                    (WPARAM) PROP_LISTBOX_HORIZONTAL_SIZE ,0 );

    dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETTABSTOPS,
                    (WPARAM) 2,(LPARAM)rgTabStops );
        
    DisplayAll();

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


/********************************************************************/
/*
 -  CResultsDlg::
 -  OnSelectMod
 -
 *  Purpose:
 *
 */
/********************************************************************/

void CResultsDlg::OnSelectMod()
{       
    LONG                lSelection      = -1;
    ULONG               ulSelectedTag   = 0;
    ULONG               idx;
    DWORD               dwReturn        = 0;
    

    // get selected item from modify listbox
    lSelection = SendDlgItemMessage(IDC_RES_MOD,LB_GETCURSEL,0,0 );
    if(lSelection == LB_ERR)
        return;

    if(m_fIsPropValueArray) 
    {           
        ulSelectedTag = m_lpspvaModify[lSelection].ulPropTag;
    }
    else
    {
        ulSelectedTag = m_lpsptaModify->aulPropTag[lSelection];            
    }        
        
    // find the same proptag in other listboxes(After,Before,Problem)
    //   and set cursor selection on other corresponding items or set 
    //   cursor selection on nothing if it is not in other listboxes.

    // BEFORE
    for(idx = 0 ; idx < m_cValuesBefore; idx++)
    {
        if( m_lpspvaBefore[idx].ulPropTag == ulSelectedTag )            
        {
            dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,LB_SETCURSEL,(WPARAM) idx ,0 );
            break;
        }
    }
    // if couldn't find the proptag, set cursor selection to nothing
    if(idx == m_cValuesBefore)
        dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,    LB_SETCURSEL,(WPARAM) -1 , 0 );

    // AFTER
    for(idx = 0 ; idx < m_cValuesAfter; idx++)
    {
        if( m_lpspvaAfter[idx].ulPropTag == ulSelectedTag )            
        {
            dwReturn = SendDlgItemMessage(IDC_RES_AFTER, LB_SETCURSEL,(WPARAM) idx, 0 );
            break;
        }
    }                                                                                
    // if couldn't find the proptag, set cursor selection to nothing
    if(idx == m_cValuesAfter)
        dwReturn = SendDlgItemMessage(IDC_RES_AFTER,     LB_SETCURSEL,(WPARAM) -1,  0 );

    // PROBLEMS
    if(m_lpProblems)
    {
        for(idx = 0 ; idx < m_lpProblems->cProblem; idx++)
        {
            if( m_lpProblems->aProblem[idx].ulPropTag == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,    LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_lpProblems->cProblem)
            dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,        LB_SETCURSEL,(WPARAM) -1 ,0 );
    }
}
/********************************************************************/
/*
 -  CResultsDlg::
 -  OnSelectBefore
 -
 *  Purpose:
 *
 */
/********************************************************************/

void CResultsDlg::OnSelectBefore()
{       
    LONG                lSelection      = -1;
    ULONG               ulSelectedTag   = 0;
    ULONG               idx;
    DWORD               dwReturn        = 0;
    
    
    // get selected item from modify listbox
    lSelection = SendDlgItemMessage(IDC_RES_BEFORE,LB_GETCURSEL,0,0 );
    if(lSelection == LB_ERR)
        return;
            
    ulSelectedTag = m_lpspvaBefore[lSelection].ulPropTag;
        
        
    // find the same proptag in other listboxes(After,Before,Problem)
    //   and set cursor selection on other corresponding items or set 
    //   cursor selection on nothing if it is not in other listboxes.

    // MODIFY
    if(m_fIsPropValueArray) 
    {           

        for(idx = 0 ; idx < m_cValuesModify; idx++)
        {
            if( m_lpspvaModify[idx].ulPropTag == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_cValuesModify)
            dwReturn = SendDlgItemMessage(IDC_RES_MOD,    LB_SETCURSEL,(WPARAM) -1 , 0 );
    }
    else
    {
        for(idx = 0 ; idx < m_lpsptaModify->cValues; idx++)
        {
            if( m_lpsptaModify->aulPropTag[idx] == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_lpsptaModify->cValues)
            dwReturn = SendDlgItemMessage(IDC_RES_MOD,    LB_SETCURSEL,(WPARAM) -1 , 0 );        
    }

    // AFTER
    for(idx = 0 ; idx < m_cValuesAfter; idx++)
    {
        if( m_lpspvaAfter[idx].ulPropTag == ulSelectedTag )            
        {
            dwReturn = SendDlgItemMessage(IDC_RES_AFTER, LB_SETCURSEL,(WPARAM) idx, 0 );
            break;
        }
    }                                                                                
    // if couldn't find the proptag, set cursor selection to nothing
    if(idx == m_cValuesAfter)
        dwReturn = SendDlgItemMessage(IDC_RES_AFTER,     LB_SETCURSEL,(WPARAM) -1,  0 );

    // PROBLEMS
    if(m_lpProblems)
    {
        for(idx = 0 ; idx < m_lpProblems->cProblem; idx++)
        {
            if( m_lpProblems->aProblem[idx].ulPropTag == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,    LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_lpProblems->cProblem)
            dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,        LB_SETCURSEL,(WPARAM) -1 ,0 );
    }
}
/********************************************************************/
/*
 -  CResultsDlg::
 -  OnSelectAfter
 -
 *  Purpose:
 *
 */
/********************************************************************/

void CResultsDlg::OnSelectAfter()
{       
    LONG                lSelection      = -1;
    ULONG               ulSelectedTag   = 0;
    ULONG               idx;
    DWORD               dwReturn        = 0;
    
    
    // get selected item from modify listbox
    lSelection = SendDlgItemMessage(IDC_RES_AFTER,LB_GETCURSEL,0,0 );
    if(lSelection == LB_ERR)
        return;
            
    ulSelectedTag = m_lpspvaAfter[lSelection].ulPropTag;
        
        
    // find the same proptag in other listboxes(After,Before,Problem)
    //   and set cursor selection on other corresponding items or set 
    //   cursor selection on nothing if it is not in other listboxes.

    // MODIFY
    if(m_fIsPropValueArray) 
    {           

        for(idx = 0 ; idx < m_cValuesModify; idx++)
        {
            if( m_lpspvaModify[idx].ulPropTag == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_cValuesModify)
            dwReturn = SendDlgItemMessage(IDC_RES_MOD,    LB_SETCURSEL,(WPARAM) -1 , 0 );
    }
    else
    {
        for(idx = 0 ; idx < m_lpsptaModify->cValues; idx++)
        {
            if( m_lpsptaModify->aulPropTag[idx] == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_lpsptaModify->cValues)
            dwReturn = SendDlgItemMessage(IDC_RES_MOD,    LB_SETCURSEL,(WPARAM) -1 , 0 );        
    }

    // BEFORE
    for(idx = 0 ; idx < m_cValuesBefore; idx++)
    {
        if( m_lpspvaBefore[idx].ulPropTag == ulSelectedTag )            
        {
            dwReturn = SendDlgItemMessage(IDC_RES_BEFORE, LB_SETCURSEL,(WPARAM) idx, 0 );
            break;
        }
    }                                                                                
    // if couldn't find the proptag, set cursor selection to nothing
    if(idx == m_cValuesBefore)
        dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,     LB_SETCURSEL,(WPARAM) -1,  0 );

    // PROBLEMS
    if(m_lpProblems)
    {
        for(idx = 0 ; idx < m_lpProblems->cProblem; idx++)
        {
            if( m_lpProblems->aProblem[idx].ulPropTag == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,    LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_lpProblems->cProblem)
            dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,        LB_SETCURSEL,(WPARAM) -1 ,0 );
    }
}

/********************************************************************/
/*
 -  CResultsDlg::
 -  OnSelectBefore
 -
 *  Purpose:
 *
 */
/********************************************************************/

void CResultsDlg::OnSelectProblem()
{       
    LONG                lSelection      = -1;
    ULONG               ulSelectedTag   = 0;
    ULONG               idx;
    DWORD               dwReturn        = 0;
    
    
    // get selected item from modify listbox
    lSelection = SendDlgItemMessage(IDC_RES_PROBLEMS,LB_GETCURSEL,0,0 );
    if(lSelection == LB_ERR)
        return;
            
    if(!m_lpProblems)
        return;
                    
    ulSelectedTag = m_lpProblems->aProblem[lSelection].ulPropTag;
        
        
    // find the same proptag in other listboxes(After,Before,Problem)
    //   and set cursor selection on other corresponding items or set 
    //   cursor selection on nothing if it is not in other listboxes.

    // MODIFY
    if(m_fIsPropValueArray) 
    {           

        for(idx = 0 ; idx < m_cValuesModify; idx++)
        {
            if( m_lpspvaModify[idx].ulPropTag == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_cValuesModify)
            dwReturn = SendDlgItemMessage(IDC_RES_MOD,    LB_SETCURSEL,(WPARAM) -1 , 0 );
    }
    else
    {
        for(idx = 0 ; idx < m_lpsptaModify->cValues; idx++)
        {
            if( m_lpsptaModify->aulPropTag[idx] == ulSelectedTag )            
            {
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_SETCURSEL,(WPARAM) idx ,0 );
                break;
            }
        }
        // if couldn't find the proptag, set cursor selection to nothing
        if(idx == m_lpsptaModify->cValues)
            dwReturn = SendDlgItemMessage(IDC_RES_MOD,    LB_SETCURSEL,(WPARAM) -1 , 0 );        
    }
    // BEFORE
    for(idx = 0 ; idx < m_cValuesBefore; idx++)
    {
        if( m_lpspvaBefore[idx].ulPropTag == ulSelectedTag )            
        {
            dwReturn = SendDlgItemMessage(IDC_RES_BEFORE, LB_SETCURSEL,(WPARAM) idx, 0 );
            break;
        }
    }                                                                                
    // if couldn't find the proptag, set cursor selection to nothing
    if(idx == m_cValuesBefore)
        dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,     LB_SETCURSEL,(WPARAM) -1,  0 );

    // AFTER
    for(idx = 0 ; idx < m_cValuesAfter; idx++)
    {
        if( m_lpspvaAfter[idx].ulPropTag == ulSelectedTag )            
        {
            dwReturn = SendDlgItemMessage(IDC_RES_AFTER, LB_SETCURSEL,(WPARAM) idx, 0 );
            break;
        }
    }                                                                                
    // if couldn't find the proptag, set cursor selection to nothing
    if(idx == m_cValuesAfter)
        dwReturn = SendDlgItemMessage(IDC_RES_AFTER,     LB_SETCURSEL,(WPARAM) -1,  0 );

}

/*******************************************************************/
/*
 -  CBldPropDlg::
 -  DisplayAll
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CResultsDlg::DisplayAll()
{
    char            szBuffer[1024];
    int             idx             = 0;
    DWORD           dwReturn        = 0;      
    char            szCValues[30];
    char            szID[50];
    char            szData[512];
    char            szType[32];      // Assumes no PropType string longer than 31 chars
    char            szIndex[10];
    char            szPropTag[50];
    char            szScError[50];      

    // multi value props stuff    
    char            szMV[1024];    
    char            szMVSeps[]      = ";:.";
    char            *lpszToken      = NULL;
    char            szTemp[30];
    ULONG           ulMVRow         = 0;
    ULONG           ulMVcValues     = 0;
    char            *szEnd          = NULL;


    SendDlgItemMessage(IDC_RES_PROBLEMS,LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_RES_BEFORE,  LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_RES_AFTER,   LB_RESETCONTENT,0,0);
    SendDlgItemMessage(IDC_RES_MOD,     LB_RESETCONTENT,0,0);


    // DISPLAY MODIFY PTA OR PVA
    if(m_fIsPropValueArray) 
    {           
            
        for(idx = 0; idx < m_cValuesModify; idx++)
        {   
            szID[0]     = '\0' ;
            szData[0]   = '\0' ;
            szType[0]   = '\0' ;
            szBuffer[0] = '\0' ;
                
            if(GetString("PropIDs", PROP_ID( m_lpspvaModify[idx].ulPropTag), szID ) )
            {
                lstrcat(szBuffer, szID );               
                lstrcat(szBuffer, "\t");
            }
            else
            {
                wsprintf(szBuffer,"%#04X\t", PROP_ID(  m_lpspvaModify[idx].ulPropTag) );        
            }

            if( GetString("PropType", PROP_TYPE( m_lpspvaModify[idx].ulPropTag), szType) )
            {
                lstrcat(szBuffer, szType);
                lstrcat(szBuffer,"\t");
            }       
            else
            {
                wsprintf(szType,"%#04X\t", PROP_TYPE( m_lpspvaModify[idx].ulPropTag) );
                lstrcat(szBuffer,szType);
            }

            SzGetPropValue(szData,(LPSPropValue) &m_lpspvaModify[idx]);

            // if it is a MultiValueProperty, parse the output, and add
            //   more than one row for this property
            if( (PROP_TYPE(m_lpspvaModify[idx].ulPropTag)) & MV_FLAG )
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
                
                    dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_ADDSTRING,0,
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
                
                    dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_ADDSTRING,0,
                            (LPARAM)szMV);
                    ulMVRow++;                            
                }
            }
            else            
            {
                lstrcat(szBuffer,szData);
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_ADDSTRING,0,
                            (LPARAM)szBuffer);
            }

        
        }  // end of for loop of m_cValuesModify in lpNewPropValue

        wsprintf(szCValues,"cValues:  %lu",m_cValuesModify);
        SetDlgItemText(IDT_RES_MOD_CVALS,szCValues);       

    }
    else  // it is a proptag array
    {
        if(m_lpsptaModify)      // for CopyTo(), this may be null
        {            
            for(idx = 0; idx < m_lpsptaModify->cValues; idx++)
            {   
                szID[0]     = '\0' ;
                szType[0]   = '\0' ;
                szBuffer[0] = '\0' ;
                    
                if(GetString("PropIDs", PROP_ID(m_lpsptaModify->aulPropTag[idx]), szID ) )
                {
                    lstrcat(szBuffer, szID );               
                    lstrcat(szBuffer, "\t");
                }
                else
                {
                    wsprintf(szBuffer,"%#04X\t", PROP_ID(m_lpsptaModify->aulPropTag[idx]) );        
                }

                if( GetString("PropType", PROP_TYPE(m_lpsptaModify->aulPropTag[idx]), szType) )
                {
                    lstrcat(szBuffer, szType);
                    lstrcat(szBuffer,"\t");
                }       
                else
                {
                    wsprintf(szType,"%#04X\t", PROP_TYPE(m_lpsptaModify->aulPropTag[idx]) );
                    lstrcat(szBuffer,szType);
                }
                dwReturn = SendDlgItemMessage(IDC_RES_MOD,LB_ADDSTRING,0,
                                    (LPARAM)szBuffer);
            
 
            }  
            wsprintf(szCValues,"lpspta->cValues:  %lu",m_lpsptaModify->cValues);
            SetDlgItemText(IDT_RES_MOD_CVALS,szCValues);       
        }
    }             
    
    // BEFORE DISPLAY       
    wsprintf(szCValues,"cValues:  %lu",m_cValuesBefore);
    SetDlgItemText(IDT_RES_BEFORE_CVALS,szCValues);
    szBuffer[0] = '\0' ;
        
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

        // if it is a MultiValueProperty, parse the output, and add
        //   more than one row for this property
        if( (PROP_TYPE(m_lpspvaBefore[idx].ulPropTag)) & MV_FLAG )
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
            
                dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,LB_ADDSTRING,0,
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
            
                dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,LB_ADDSTRING,0,
                        (LPARAM)szMV);
                ulMVRow++;                            
            }
        }
        else            
        {
            lstrcat(szBuffer,szData);
            dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
        }
    
    } 

    // AFTER DISPLAY
    wsprintf(szCValues,"cValues:  %lu",m_cValuesAfter);
    SetDlgItemText(IDT_RES_AFTER_CVALS,szCValues);
    szBuffer[0] = '\0' ;

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

        // if it is a MultiValueProperty, parse the output, and add
        //   more than one row for this property
        if( (PROP_TYPE(m_lpspvaAfter[idx].ulPropTag)) & MV_FLAG )
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
            
                dwReturn = SendDlgItemMessage(IDC_RES_AFTER,LB_ADDSTRING,0,
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
            
                dwReturn = SendDlgItemMessage(IDC_RES_AFTER,LB_ADDSTRING,0,
                        (LPARAM)szMV);
                ulMVRow++;                            
            }
        }
        else            
        {
            lstrcat(szBuffer,szData);
            dwReturn = SendDlgItemMessage(IDC_RES_AFTER,LB_ADDSTRING,0,
                        (LPARAM)szBuffer);
        }
 
    } 

    // DISPLAY PROBLEMS(if any)
    if(m_lpProblems)
    {
        wsprintf(szCValues,"cProblems:  %lu",m_lpProblems->cProblem);
        SetDlgItemText(IDT_RES_PROBLEMS_CVALS,szCValues);

        szBuffer[0] = '\0' ;


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
            dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS,LB_ADDSTRING,0,
                                (LPARAM)szBuffer);      
        }  
    }

    // set focus/cursor selection off all listboxes
    dwReturn = SendDlgItemMessage(IDC_RES_AFTER,    LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwReturn = SendDlgItemMessage(IDC_RES_BEFORE,   LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwReturn = SendDlgItemMessage(IDC_RES_PROBLEMS, LB_SETCURSEL,(WPARAM) -1 ,0 );
    dwReturn = SendDlgItemMessage(IDC_RES_MOD,   LB_SETCURSEL,(WPARAM) -1 ,0 );
}


/*******************************************************************/
/*
 -  CResultsDlg::
 -  ~CResultsDlg
 -
 *  Purpose:
 *      Destructor for class CResultsDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CResultsDlg::~CResultsDlg()
{

} 



/*******************************************************************/
/*
 -  CResultsDlg::
 -  OnDumpPropValsBeforeToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CResultsDlg::OnDumpPropValsBeforeToFile()
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
            WritePropValArray(  StoreProp.m_szFileName,
                                StoreProp.m_szTagID,
                                m_cValuesBefore,
                                m_lpspvaBefore,
                                StoreProp.m_ulFlags);
        }                        
    }
}

/*******************************************************************/
/*
 -  CResultsDlg::
 -  OnDumpPropValsAfterToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CResultsDlg::OnDumpPropValsAfterToFile()
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
/*******************************************************************/
/*
 -  CResultsDlg::
 -  OnDumpProblemsToFile
 *
 *  Purpose:
 *      Store PropValues in object to file
 */
/*******************************************************************/

void CResultsDlg::OnDumpProblemsToFile()
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
    strcpy(szTag,"[PROBLEM 0001]");       

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
        
    if( StoreProp.DoModal() == IDOK )
    {    
        WriteProblemArray( StoreProp.m_szFileName,
                      StoreProp.m_szTagID,
                      m_lpProblems);
    }
}


/*******************************************************************/
/*
 -  CBldPropDlg::
 -  OnDumpPropsModifyToFile
 *
 *  Purpose:
 */
/*******************************************************************/

void CResultsDlg::OnDumpPropsModifyToFile()
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
    strcpy(szTag,"[PROPS MODIFY 0001]");

    StoreProp.m_TagID       = szTag;
    StoreProp.m_FileName    = szFileName;
    
    if( StoreProp.DoModal() == IDOK )
    {    
        // if it is a prop tag array 
        if(m_lpsptaModify)
        {   
            WritePropTagArray(  StoreProp.m_szFileName,
                            StoreProp.m_szTagID,
                            m_lpsptaModify);
        }
        else        
        {                    
            WritePropValArray( StoreProp.m_szFileName,
                            StoreProp.m_szTagID,
                            m_cValuesModify,
                            m_lpspvaModify,
                            StoreProp.m_ulFlags);
        }                            
    }    
}



