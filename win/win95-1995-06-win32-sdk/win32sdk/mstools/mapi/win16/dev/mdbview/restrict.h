/*********************************************************************/
/*
 -  restrict.h
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for restrictations class.
 *      This class is used to bring up a dialog from which the 
 *      user can select data to be used as parameters to restrictations.
 *      Not all the dialog controls are used in every case.
 *
 */
/*********************************************************************/

#ifndef __restrict_h_       // test defined
#define __restrict_h_


/*
 -  CResDlg
 -
 *  Purpose:
 *
 *
 */

class CResDlg : public CModalDialog
{
private:
    LPSPropTagArray m_lpCurColumns;
    LPSRestriction  m_lpRes;
    LPSRestriction  m_lpSubRes;
    int             m_fComb;
    int             m_nResType1;
    int             m_nResType2;
    int             m_nResType3;

public:
    CResDlg(LPSPropTagArray lpspta, LPSRestriction lpr, CWnd* lpParent = NULL);

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    
    afx_msg void OnAnd();
    afx_msg void OnOr();
    afx_msg void OnResType1();
    afx_msg void OnResType2();
    afx_msg void OnResType3();
    afx_msg void OnSubRes1();
    afx_msg void OnSubRes2();
    afx_msg void OnSubRes3();

    DECLARE_MESSAGE_MAP();
};

/*
 -  CRestrictionDlg
 -
 *  Purpose:
 *
 *
 */

class CRestrictionDlg : public CModalDialog
{
public:
    ULONG           m_ulSearchState;
    LPSRestriction  m_prest;

    CRestrictionDlg( CWnd * pParentWnd)
        : CDialog(RESTRICTION, pParentWnd)
        {
            m_ulSearchState = 0;
            m_prest         = NULL;
        }
    
    ~CRestrictionDlg();

    BOOL OnInitDialog();
  
    void DisplayRestriction(LPSRestriction lpRes);

};


/*
 -  CAcceptRestrictionDlg
 -
 *  Purpose:
 *
 *
 */

class CAcceptRestrictionDlg : public CModalDialog
{
public:
    LPSRestriction  m_prest;

    CAcceptRestrictionDlg( CWnd * pParentWnd)
        : CDialog(ACCEPTRES, pParentWnd)
        {
            m_prest         = NULL;
        }
    
    ~CAcceptRestrictionDlg();

    BOOL OnInitDialog();
    afx_msg void OnModify();

    DECLARE_MESSAGE_MAP();
  
    void DisplayRestriction(LPSRestriction lpRes);

};
      

#endif



