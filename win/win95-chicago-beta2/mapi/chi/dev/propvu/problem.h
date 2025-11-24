/*********************************************************************/
/*
 -  problem.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for Problems Dialog class
 *
 */
/*********************************************************************/

#ifndef __problem_h_
#define __problem_h_


#include "propvu.h"

/*********************************************************************/
/*
 -  CProblemDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Get Property Dialog
 *
 */
/*********************************************************************/

class CProblemDlg : public CModalDialog
{
public:

    LPSPropProblemArray     m_lpProblems;
    CString                 m_Operation;        

    CProblemDlg(CWnd  *pParentWnd)
        : CModalDialog(PROBLEMS, pParentWnd)
    {
        m_lpProblems       = NULL;
    }

    ~CProblemDlg(); 

    BOOL OnInitDialog();

    void DisplayProblems();

    void OnDumpProblemsToFile();
    
    DECLARE_MESSAGE_MAP();

};
                         

#endif




























