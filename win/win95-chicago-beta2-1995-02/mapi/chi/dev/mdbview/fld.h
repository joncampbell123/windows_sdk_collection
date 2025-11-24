/*********************************************************************/
/*
 -  fld.h
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Defines id's, function prototypes, classes for folder class
 */
/*********************************************************************/

#ifndef __folder_h_    
#define __folder_h_

#include "mdbview.h"

/*********************************************************************/
/*
 -  CFolderDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Folder viewer Dialog
 *
 */
/*********************************************************************/

class CFolderDlg : public CPropDlg
{
protected:

    BOOL InitFldDialog();               

public:

    // data members
    LPMAPITABLE     m_lpAssociatedTable;    
    LPMAPITABLE     m_lpMessageTable;   
    LPMAPITABLE     m_lpChildTable;
    CString         m_CurrentFolder;
    LPMAPIFOLDER        m_lpFolder;

//#ifdef XVPORT
    // used for keeping track of current advises on objects
    LPNOTIFS        m_lpNotifAssTbl;
    LPNOTIFS        m_lpNotifContTbl;
    LPNOTIFS        m_lpNotifHeirTbl;
    LPNOTIFS        m_lpNotifFld;
//#endif

    char            m_szHeirContext[COMMENT_SIZE];
    char            m_szContContext[COMMENT_SIZE];
    char            m_szAssContext[COMMENT_SIZE];
    char            m_szFldContextCreated[COMMENT_SIZE];
    char            m_szFldContextDeleted[COMMENT_SIZE];
    char            m_szFldContextCopied[COMMENT_SIZE];
    char            m_szFldContextMoved[COMMENT_SIZE];
    char            m_szFldContextModified[COMMENT_SIZE];
    char            m_szFldContextCritical[COMMENT_SIZE];

    char            m_szSearchFldContext[COMMENT_SIZE];

    // constructor
    CFolderDlg( CString     FolderName,
                LPMAPIFOLDER    lpFld,
                CWnd        *pParentWnd)
        : CPropDlg( (LPMAPIPROP)lpFld,Fld, pParentWnd)  
    {             
        m_CurrentFolder     = FolderName;
        m_lpFolder          = lpFld;
        m_lpChildTable      = NULL;
        m_lpMessageTable    = NULL;       
        m_lpAssociatedTable = NULL;       

//#ifdef XVPORT
        m_lpNotifAssTbl     = NULL;
        m_lpNotifContTbl    = NULL;
        m_lpNotifHeirTbl    = NULL;
        m_lpNotifFld        = NULL;
//#endif
            
        if( !InitFldDialog())       // after creation of dialog, to folder
           OnCancel();              //  specific initialization to fill in
                                    //  listbox's etc.
    }

    ~CFolderDlg()       { Cleanup(); }
    void OnCancel();
    void Cleanup();
         
    void OnChildFolder();
    void OnOpenChildFolder();

    void OnMessage();
    void OnOpenMessage();

    void OnAssociated();
    void OnOpenAssociated();
    
    // setting state, redrawing
    void RedrawFolderTable();
    void RedrawMessageTable();
    void RedrawAssociatedTable();

    void OnProperty();
    void OnCurrentFolder();
    void OnMapiLog();

    // operations   
    void OnFldCreate();
    void OnFldDelete();
    void OnMsgCreate();
    void OnMsgDelete();
    void OnSetSearchDestFlds();
    void OnCopyFolder();
    void OnCopyMessages();
    void OnAbortSubmit();
    void OnSetReceiveFolder();
    void OnContTable();
    void OnHeirTable();
    void OnEmptyFolder();
    void OnFldProp();
    void OnMDBProp();
    void OnGetSearchCriteria();        
    void OnSetSearchCriteria();        
    void OnSetCopyToDest();
    void OnGetMsgStatus();                
    void OnSetMsgStatus();                

    void OnNotifHeir();
    void OnNotifAss();
    void OnNotifFld();
    void OnNotifCont();
                
    DECLARE_MESSAGE_MAP();
};

#endif



