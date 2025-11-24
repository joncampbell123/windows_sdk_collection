/*********************************************************************/
/*
 -  bldprop.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for SetProps class
 *
 */
/*********************************************************************/

#ifndef __bldprop_h_        // test defined
#define __bldprop_h_


#include "propvu.h"

/*********************************************************************/
/*
 -  CBldPropDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Set Property Dialog
 *
 */
/*********************************************************************/

class CBldPropDlg : public CModalDialog
{
public:
    CString         m_SelectedTag;      
    LPMAPIPROP      m_lpSetEntry;       // pointer to LPMAPIPROP object
    LONG            m_lSelectedTag;     // currently selected proptag

    BOOL            m_fCall;            // set to true if call, else don't call, false

    BOOL            m_fIsPropValueArray;    // if PropValueArray then ==1
    CString         m_Operation;

    LPSPropValue    m_lpOldPropValue;   // holds state when come into setprops
    ULONG           m_ulOldValues;

    LPSPropValue   *m_lppNewPropValue;  // used to build new lpsPropValue to set with
    ULONG           m_ulNewValues;
    
    LPSPropValue    m_lpspvaSelected;

    LPSPropTagArray m_lpNewPTA;         // used to build new lpsPropTagArray get with

    CBldPropDlg(CWnd * pParentWnd)
        : CModalDialog(PropBuilder, pParentWnd)
    {
        m_Operation         = "PropBuilder";
        m_lpOldPropValue    = NULL;
        m_ulOldValues       = 0;
        m_lppNewPropValue   = NULL;
        m_lpNewPTA          = NULL;
        m_ulNewValues       = 0;
        m_lSelectedTag      = 0;
        m_lpSetEntry        = NULL;
        m_lpspvaSelected    = NULL;
        m_fCall             = FALSE;
    }

    ~CBldPropDlg(); 

    BOOL OnInitDialog();

    void OnCall();      
    void OnAdd();
    void OnDelete();        
    void OnSelectPropTagLB();
    void OnSelectPropTypeCB();
    void OnChangePropIDHexLB();
    void OnRemoveAll();
    void OnAddAllMapiTags();
    void OnAddAllCurrent();
    void RedrawBuildProps();

    void SelectNewPropType(ULONG ulSelectedType);

    void OnDumpPropValsToFile();
    
    DECLARE_MESSAGE_MAP();
};
                         

#endif




























