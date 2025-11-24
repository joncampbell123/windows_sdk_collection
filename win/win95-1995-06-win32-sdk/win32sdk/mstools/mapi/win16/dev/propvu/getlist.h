/*********************************************************************/
/*
 -  getlist.h
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for GetPropList Dialog class
 *
 */
/*********************************************************************/

#ifndef __getlist_h_
#define __getlist_h_


#include "propvu.h"

/*********************************************************************/
/*
 -  CGetPropListDlg Class
 -
 *  Purpose:
 *      Defines the controls for the GetPropList Property Dialog
 *
 */
/*********************************************************************/

class CGetPropListDlg : public CModalDialog
{
public:

    LPSPropTagArray     m_lpPropTagArray;

    CGetPropListDlg(CWnd  *pParentWnd)
        : CModalDialog(GETPROPLIST, pParentWnd)
    {
        m_lpPropTagArray       = NULL;
    }

    ~CGetPropListDlg(); 

    BOOL OnInitDialog();

    void DisplayPropList();

};
                         

#endif




























