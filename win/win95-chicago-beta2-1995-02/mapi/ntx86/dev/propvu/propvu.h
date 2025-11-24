/*
 -  P R O P V U . H
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Header for PROPVU.CPP property viewer dll
 *
 */

#ifndef __PROPVU_H__
#define __PROPVU_H__

/******************** Utility/Tool Function Prototypes ***************/


/******************** Application Defines ****************************/

// listbox sizes

#define PROP_LISTBOX_HORIZONTAL_SIZE    2000
#define PROP_LISTBOX_TAB1_SIZE          120
#define PROP_LISTBOX_TAB2_SIZE          185
#define MAPILOG_LISTBOX_TAB1_SIZE       90

// Message Box Styles 

#define MBS_ERROR                       (MB_ICONSTOP | MB_OK )
#define MBS_INFO                        (MB_ICONINFORMATION | MB_OK)
#define MBS_OOPS                        (MB_ICONEXCLAMATION | MB_OK)

#define MAX_LOG_BUFF                    4096 // for displaying PT_BINARY
#define MAX_SET_PROPS                   500 // maximum number of propvals in setprops
        
/******************** Class Definitions ******************************/

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


/*********************************************************************/
/*
 -  CPropDlg Class
 -
 *  Purpose:
 */
/*********************************************************************/

class CPropDlg : public CDialog
{
protected:

    void OnType();  
    void OnID();
    void OnData();  
    void OnString();   
    void OnHex();
    void OnDecimal();    
    void RedrawPropTable();             
    void ResetPropButtons();          

public:
    LPMAPIPROP  m_lpEntry;   
    CString     m_CurrentProp;
    HRESULT     m_hResult;
    ULONG       m_ulRefCount;

    LPVOID      m_lpvDestObj;

    CPropDlg(   CString     PropName,
                LPMAPIPROP  *lppEntry,
                LPVOID      lpvDest,
                HWND        hWnd)
            : CDialog(PROPVIEW,  NULL) 
        {
            m_lpvDestObj    = lpvDest;
            m_lpEntry       = *lppEntry;
                    
            m_CurrentProp   = PropName;            
            // prevent user from Releasing on first instance 
            //   AddRef will affect this, as will subsequent Releases
            m_ulRefCount    = 1;

            if( ! Create(PROPVIEW,NULL) )         // create dialog here
            {
                MessageBox("CreateDialog PropVu Failed","Client",MBS_ERROR);           
                delete this;
            }
        }
    
    ~CPropDlg() { Cleanup();  }

    BOOL OnInitDialog();
    void Stub();                        // generic NYI message
    void OnClearMapiLog();
    void OnStorePropValsToFile();
    void OnCancel();
    void Cleanup();
    
    // PROPERTY OPERATIONS    
    void OnAddRef();
    void OnCopyTo();
    void OnDeleteProps();
    void OnGetIDsFromNames();
    void OnGetLastError();
    void OnGetNamesFromIDs();
    void OnGetProps();
    void OnGetPropList();
    void OnOpenProperty();
    void OnQueryInterface();
    void OnRelease();    
    void OnSaveChanges();                   
    void OnSetProps();
    void OnCopyProps();
    
    DECLARE_MESSAGE_MAP();
        
};


/*********************************************************************/
/*
 -  CStorePropValDlg Class
 -
 *  Purpose:
 *      Defines the controls for the GetPropList Property Dialog
 *
 */
/*********************************************************************/

class CStorePropValDlg : public CModalDialog
{
public:
    CString m_TagID;
    CString m_FileName;

    char    m_szTagID[80];
    char    m_szFileName[80];
    ULONG   m_ulFlags;
    
    CStorePropValDlg(CWnd  *pParentWnd)
        : CModalDialog(STOREPROP, pParentWnd)
    {
        m_ulFlags = 0;
    }

    ~CStorePropValDlg();    

    BOOL OnInitDialog();

    void OnOK();
    void OnDumpBinary();

    DECLARE_MESSAGE_MAP();

};

#endif  /* __PROPVU_H__ */
