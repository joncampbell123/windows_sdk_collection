/*********************************************************************/
/*
 -  getprop.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for GetProps Dialog class
 *
 */
/*********************************************************************/

#ifndef __getprop_h_
#define __getprop_h_


#include "propvu.h"

/*********************************************************************/
/*
 -  CGetPropDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Get Property Dialog
 *
 */
/*********************************************************************/

class CGetPropDlg : public CModalDialog
{
public:
    
    LPSPropTagArray m_lpPTA;
    LPSPropValue    m_lpPVA;
    ULONG           m_cValues;

    CGetPropDlg(CWnd  *pParentWnd)
        : CModalDialog(GETPROPS, pParentWnd)
    {
        m_lpPTA            = NULL;
        m_lpPVA            = NULL;
        m_cValues          = 0;
    }

    ~CGetPropDlg(); 
    BOOL OnInitDialog();

    void OnDumpPropValsToFile();
    void OnDumpPropTagsToFile();
    
    void DisplayProps();
    
    DECLARE_MESSAGE_MAP();    

};
                         

#endif




























