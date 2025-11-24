/*********************************************************************/
/*
 -  NamesIDs.h
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for Names to IDs mapping Dialog class
 *
 */
/*********************************************************************/

#ifndef __namesids_h_
#define __namesids_h_


#include "propvu.h"

/*********************************************************************/
/*
 -  CNamesIDsDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Names To IDs/IDs to Names mapping
 */
/*********************************************************************/

class CNamesIDsDlg : public CModalDialog
{
public:

    LPSPropTagArray     m_lpPTA;
    LPGUID              m_lpPropSetGuid;
    
    LPMAPINAMEID   FAR *m_lppMAPINameID;
    ULONG               m_ulPropNames;

    CNamesIDsDlg(CWnd  *pParentWnd)
        : CModalDialog(NAMESIDS, pParentWnd)
    {
        m_lpPTA             = NULL;
        m_lppMAPINameID     = NULL;    
        m_ulPropNames       = 0;
    }

    ~CNamesIDsDlg();    

    BOOL OnInitDialog();

    void DisplayNamesIDs();
};

#endif




