/*********************************************************************/
/*
 -  msg.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for message class
 */
/*********************************************************************/

#ifndef __msg_h_        // test defined
#define __msg_h_

#include "mdbview.h"



/*********************************************************************/
/*
 -  CMessageDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Message viewer Dialog
 *
 */
/*********************************************************************/

class CMessageDlg : public CPropDlg
{
protected:
    BOOL    InitMsgDialog();

public:
    CString         m_CurrentMessage;
    LPMESSAGE       m_lpMessage;
    LPMAPITABLE     m_lpAttachTable;
    LPMAPITABLE     m_lpRecipTable;

//#ifdef XVPORT
    LPNOTIFS        m_lpNotifAttTbl;
    LPNOTIFS        m_lpNotifRecipTbl;
    LPNOTIFS        m_lpNotifMsg;
//#endif

    char            m_szAttContext[COMMENT_SIZE];
    char            m_szRecipContext[COMMENT_SIZE];
    char            m_szMsgContextCreated[COMMENT_SIZE];
    char            m_szMsgContextDeleted[COMMENT_SIZE];
    char            m_szMsgContextCopied[COMMENT_SIZE];
    char            m_szMsgContextMoved[COMMENT_SIZE];
    char            m_szMsgContextModified[COMMENT_SIZE];
    char            m_szMsgContextCritical[COMMENT_SIZE];


    CMessageDlg(CString MsgName,LPMESSAGE lpMsg,CWnd * pParentWnd)
        : CPropDlg((LPMAPIPROP) lpMsg,Msg, NULL)
    {
        m_lpRecipTable      = NULL; 
        m_lpMessage         = lpMsg;        
        m_lpAttachTable     = NULL;
        m_CurrentMessage    = MsgName;
//#ifdef XVPORT
        m_lpNotifAttTbl     = NULL;
        m_lpNotifRecipTbl   = NULL;
        m_lpNotifMsg        = NULL;
//#endif        
        if( !InitMsgDialog() )
            Cleanup();
    }

    ~CMessageDlg()   { Cleanup(); }
    void OnCancel();
    void Cleanup();         
    
    void OnAttachLB();
    void OnOpenAttachLB();

    void OnRecipLB();   
    
    // display    
    void OnProperty();
    void OnRecipTable();
    void OnAttachTable();
    void OnCurrentMsg();
    void OnMsgFlags();

    void    DisplayMsgFlags();
    void    DisplayRecips();
    void    DisplayAttachments();
    
    void    RedrawPriority();

    // operations 
    void OnModifyRecipients();
    void OnSubmitMessage();
    void OnSetReadFlag();
    void OnDeleteAttach();
    void OnCreateAttach();

    void OnRecipientOptions();
    void OnMsgProp();
    void OnMDBProp();
    void OnMessageOptions();
    
    void OnShowForm();

    void OnSetCopyToDest();
    
    void OnNotifAttTbl();
    void OnNotifRecipTbl();
    void OnNotifMsg();
    
    DECLARE_MESSAGE_MAP();
};

#endif


