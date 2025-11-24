/*
 -  S T A T V U . H
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Header for STATVU.CPP
 *
 */

#ifndef __STATVU_H__
#define __STATVU_H__

// Message Box Styles 

#define MBS_ERROR                       (MB_ICONSTOP | MB_OK )
#define MBS_INFO                        (MB_ICONINFORMATION | MB_OK)
#define MBS_OOPS                        (MB_ICONEXCLAMATION | MB_OK)


#define PROP_LISTBOX_HORIZONTAL_SIZE    2000


/*********************************************************************/
/*
 -  COpenStatusDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Open Status Dialog
 *
 */
/*********************************************************************/

class COpenStatusDlg : public CModalDialog
{
public:
    LPMAPITABLE     m_lptblStatus;
    LPSRowSet       m_lpRows;
    LPMAPISESSION   m_lpSession;
    ULONG           m_cbEntryIDTarget;
    LPENTRYID       m_lpEntryIDTarget;
    CString         m_szTarget;

    COpenStatusDlg(LPMAPISESSION lpSession, HWND hWnd)
        : CModalDialog(STATUS_CHOOSE, NULL)
    {
        m_lpSession         = lpSession;
        m_lptblStatus       = NULL;
        m_lpRows            = NULL;
        m_cbEntryIDTarget   = 0;
        m_lpEntryIDTarget   = NULL;
    }

    ~COpenStatusDlg();  

    BOOL OnInitDialog();

    void OnOpen();
    void OnReadOnly();  
    void OnTargetXport();
    void DisplayStatus();   
    void OnCancel();
    
    DECLARE_MESSAGE_MAP();
};

                     

/*********************************************************************/
/*
 -  CStatusDlg Class
 -
 *  Purpose:
 *      Defines the controls for the Open Status Dialog
 *
 */
/*********************************************************************/

class CStatusDlg : public CModalDialog
{
public:
    LPMAPISTATUS        m_lpStatus;
    LONG                m_lResourceType;
    LONG                m_lResourceFlags;
    CString             m_szDLLName;
    ULONG               m_cbEntryIDTargetXport;
    LPENTRYID           m_lpEntryIDTargetXport;
    CString             m_szTargetXport;

    CStatusDlg(CWnd * pParentWnd)
        : CModalDialog(STATUS, pParentWnd)
    {
        m_lpStatus              = NULL;
        m_lResourceType         = 0;
        m_lResourceFlags        = 0;        
        m_cbEntryIDTargetXport  = 0;
        m_lpEntryIDTargetXport  = NULL;
    }

    ~CStatusDlg();  

    BOOL OnInitDialog();

    void OnCancel();
    
    void DisplayProps();

    void OnStatusProps();
    void OnValidateState();
    void OnSettingsDialog();
    void OnFlushQueues();
    void OnChangePassword();
    void OnRegisterNotification();
        
    DECLARE_MESSAGE_MAP();
};

/*********************************************************************/
/*
 -  CGetError
 -
 *  Purpose:
 *      Defines the methods to Get Error Strings.
 *
 */
/*********************************************************************/

class CGetError
{
    private:
        char    m_szMessage[80];
        char    m_szResult[80];
        char    m_szBuffer[80];

    public:
        CGetError()
        {
            m_szMessage[0]  = '\0';
            m_szResult[0]   = '\0';
        };

        ~CGetError() { };           

#ifdef WIN16
        LPSTR SzError( LPSTR, HRESULT );
#endif        
        LPSTR SzError( LPSTR, SCODE );
};


#endif  /* __STATVU_H__ */
