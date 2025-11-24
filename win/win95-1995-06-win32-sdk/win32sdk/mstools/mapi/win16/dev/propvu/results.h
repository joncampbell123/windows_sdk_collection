/*********************************************************************/
/*
 -  Results.h
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for Resultss Dialog class
 *
 */
/*********************************************************************/

#ifndef __Results_h_
#define __Results_h_


#include "propvu.h"


#define PROBLEM_LISTBOX_TAB1_SIZE    47
#define PROBLEM_LISTBOX_TAB2_SIZE    190

/*********************************************************************/
/*
 -  CResultsDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Get Property Dialog
 *
 */
/*********************************************************************/

class CResultsDlg : public CModalDialog
{
public:
    
    LPSPropValue            m_lpspvaBefore;
    ULONG                   m_cValuesBefore;
    LPSPropValue            m_lpspvaAfter;
    ULONG                   m_cValuesAfter;
    CString                 m_Operation;
    LPSPropProblemArray     m_lpProblems;
    BOOL                    m_fIsPropValueArray;
    LPSPropValue            m_lpspvaModify;
    ULONG                   m_cValuesModify;
    LPSPropTagArray         m_lpsptaModify;    

    CResultsDlg(CWnd  *pParentWnd)
        : CModalDialog(RESULTS, pParentWnd)
    {
        m_lpspvaBefore              = NULL;
        m_cValuesBefore             = 0;
        m_lpspvaAfter               = NULL;
        m_cValuesAfter              = 0;
        m_lpspvaModify              = NULL;
        m_cValuesModify             = 0;
        m_lpsptaModify              = NULL;
        m_lpProblems                = NULL;
        m_fIsPropValueArray         = FALSE;
    }

    ~CResultsDlg(); 

    BOOL OnInitDialog();

    void OnDumpPropValsBeforeToFile();
    void OnDumpPropValsAfterToFile();
    void OnDumpProblemsToFile();
    void OnDumpPropsModifyToFile();
        
    void DisplayAll();

    void OnSelectMod();
    void OnSelectBefore();
    void OnSelectAfter();
    void OnSelectProblem();
    
    DECLARE_MESSAGE_MAP();    

};
                         

#endif




























