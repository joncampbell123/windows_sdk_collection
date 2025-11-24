/*********************************************************************/
/*
 -  Compare.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for Compares Dialog class
 *
 */
/*********************************************************************/

#ifndef __Compare_h_
#define __Compare_h_


#include "propvu.h"

/*********************************************************************/
/*
 -  CCompareDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Get Property Dialog
 *
 */
/*********************************************************************/

class CCompareDlg : public CModalDialog
{
public:
    
    LPSPropValue    m_lpspvaBefore;
    ULONG           m_cValuesBefore;
    LPSPropValue    m_lpspvaAfter;
    ULONG           m_cValuesAfter;
    CString         m_Operation;

    CCompareDlg(CWnd  *pParentWnd)
        : CModalDialog(COMPARE, pParentWnd)
    {
        m_lpspvaBefore            = NULL;
        m_cValuesBefore           = 0;
        m_lpspvaAfter             = NULL;
        m_cValuesAfter            = 0;
    }

    ~CCompareDlg(); 
    BOOL OnInitDialog();

    void OnDumpPropValsBeforeToFile();
    void OnDumpPropValsAfterToFile();
    
    void DisplayProps();

    DECLARE_MESSAGE_MAP();    

};
                         

#endif




























