/*********************************************************************/
/*
 -  attach.h
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for attach class
 */
/*********************************************************************/

#ifndef __attach_h_     
#define __attach_h_

#include "mdbview.h"

/*********************************************************************/
/*
 -  CAttachDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Attach viewer Dialog
 *
 */
/*********************************************************************/

class CAttachDlg : public CPropDlg
{
protected:
    BOOL InitAttachDialog();

public:

    LPMAPIPROP      m_lpAttach;
    CString         m_CurrentAttach;

    CAttachDlg( CString     AttachName,
                LPATTACH    lpAttach,
                CWnd *      pParentWnd)
        : CPropDlg((LPMAPIPROP) lpAttach,ATTACH,NULL)
    {
        m_lpAttach      = lpAttach;
        m_CurrentAttach = AttachName;
        
        if( !InitAttachDialog() )
            OnCancel();
    }

    ~CAttachDlg()   { Cleanup(); }
    void OnCancel();
    void Cleanup();

    void OnMDBProp();
    void OnOpenAttachAsMsg();                   
    void OnProperty() { }
    void OnSetCopyToDest();
    void OnCallFunction(); 
    void OnSpecialProps(); 
    void OnPropInterface();

    DECLARE_MESSAGE_MAP();
};

/*********************************************************************/
/*
 -  CAttachSpecialDlg
 -
 *  Purpose:
 *      Defines the controls for the GetPropList Property Dialog
 *
 */
/*********************************************************************/

class CAttachSpecialDlg : public CModalDialog
{
public:

    LPATTACH        m_lpAttach;

    CAttachSpecialDlg(  LPATTACH       lpAttach,
                        CWnd           *pParentWnd)
        : CModalDialog(ATTACH_SPECIAL, pParentWnd)
    {
       m_lpAttach = lpAttach;
    }

    ~CAttachSpecialDlg();   

    BOOL OnInitDialog();
    void OnCancel();    

};




#endif




