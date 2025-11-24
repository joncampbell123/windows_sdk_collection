/*
 -  T B L V I E W . H
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Header for TBLVIEW.CPP
 *
 */

#ifndef __TBLVIEW_H__
#define __TBLVIEW_H__


#define VALUES_LB_HOR_SIZE  2000
#define VALUES_LB_TAB1      140
#define VALUES_LB_TAB2      40

typedef struct _bk
{
    BOOKMARK        bk;
    LPSPropValue    lpProp;
}BK, FAR *LPBK;

#define BKLIST_MAX  100

typedef struct _bklist
{
    ULONG       cValues;
    BK          bkList[BKLIST_MAX];
}BKLIST, FAR *LPBKLIST;

        

/*
 -  CTblDlg
 -
 *  Purpose:
 *
 *
 */

class CTblDlg : public CModalDialog
{
private:
    LPMAPITABLE     m_lpTable;
    LPSRestriction  m_lpRes;
    LPSSortOrderSet m_lpSort;
    ULONG           m_ulptDisplay;
    ULONG           m_cRows;
    BKLIST          m_BookMarks;

public:
    CTblDlg(LPMAPITABLE lpTbl, HWND hWnd = NULL);

    virtual BOOL    OnInitDialog();

    afx_msg void    OnSelDisplayValue();
    afx_msg void    OnSetBookmark();
    afx_msg void    OnForceSort();
    afx_msg void    OnForceRestriction();
    afx_msg void    OnGetStatus();
    afx_msg void    OnQueryPosition();
    afx_msg void    OnAbort();
    afx_msg void    OnQuerySortOrder();
    afx_msg void    OnGetLastError();
    afx_msg void    OnGetRowCount();
    
    afx_msg void    OnFind();
    afx_msg void    OnFreeBookmark();
    afx_msg void    OnRestriction();
    afx_msg void    OnSeek();
    afx_msg void    OnSetColumns();
    afx_msg void    OnSortOrder();
    afx_msg void    OnClose();
    afx_msg void    OnClearStatus();
    afx_msg void    OnClearMapiLog();

private:
    void    RenderTable();
    void    RenderRow(ULONG cValues, LPSPropValue lpProps);
    void    UpdateRow(LPMAPITABLE lpTable);

    DECLARE_MESSAGE_MAP();
};



/*
 -  CSortDlg
 -
 *  Purpose:
 *
 *
 */

class CSortDlg : public CModalDialog
{
private:
    LPMAPITABLE     m_lpTable;
    LPSSortOrderSet *m_lppSort;
    BOOL            fDesc;

public:
    CSortDlg(LPMAPITABLE lpTbl, LPSSortOrderSet *lppsos, CWnd* lpParent = NULL)
        : CModalDialog(IDD_SORTDLG, lpParent)
    {
        m_lpTable = lpTbl;
        m_lppSort = lppsos;
    }

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    
    afx_msg void OnAdd();
    afx_msg void OnAddDesc();
    afx_msg void OnRemove();
    afx_msg void OnRemoveAll();
    afx_msg void OnAddHexValueA();
    afx_msg void OnAddHexValueD();
    afx_msg void OnAllMAPITags();

    DECLARE_MESSAGE_MAP();
};



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



/*
 -  CPickPropDlg
 -
 *  Purpose:
 *
 *
 */

class CPickPropDlg : public CModalDialog
{
private:
    LPSRow          m_lpRow;
    DWORD *         m_lpdwIndex;

public:
    CPickPropDlg(LPSRow lpsr, DWORD *lpdw, CWnd* lpParent = NULL)
        : CModalDialog(IDD_PICKPROP, lpParent)
    {
        m_lpRow     = lpsr;
        m_lpdwIndex = lpdw;
    }

    virtual BOOL OnInitDialog();
    virtual void OnOK();

    DECLARE_MESSAGE_MAP();
};



/*
 -  CSetColDlg
 -
 *  Purpose:
 *
 *
 */

class CSetColDlg : public CModalDialog
{
private:
    LPMAPITABLE m_lpTable;

public:
    CSetColDlg(LPMAPITABLE lpTbl, CWnd* lpParent = NULL)
        : CModalDialog(IDD_SETCOL, lpParent)
    {
        m_lpTable = lpTbl;
    }

    virtual BOOL OnInitDialog();

    afx_msg void OnAdd();
    afx_msg void OnRemove();
    afx_msg void OnRemoveAll();
    afx_msg void OnAllMAPITags();
    afx_msg void OnAddHexValue();
    afx_msg void OnSet();
    afx_msg void OnClose();
    afx_msg void OnCancel();

    DECLARE_MESSAGE_MAP();
};



/*
 -  CFreeBkDlg
 -
 *  Purpose:
 *
 *
 */

class CFreeBkDlg : public CModalDialog
{
private:
    LPMAPITABLE m_lpTable;
    LPBKLIST    m_lpBkList;

public:
    CFreeBkDlg(LPMAPITABLE lpTbl, LPBKLIST lpbkl, CWnd* lpParent = NULL)
        : CModalDialog(IDD_FREEBOOKMARK, lpParent)
    {
        m_lpTable  = lpTbl;
        m_lpBkList = lpbkl;
    }

    virtual BOOL OnInitDialog();

    afx_msg void OnFree();
    afx_msg void OnFreeAll();
    afx_msg void OnClose();

    DECLARE_MESSAGE_MAP();
};



/*
 -  CSeekDlg
 -
 *  Purpose:
 *
 *
 */

class CSeekDlg : public CModalDialog
{
private:
    LPMAPITABLE m_lpTable;
    LPBKLIST    m_lpBkList;
    BOOL        m_fSeek;
    int         m_nBk;

public:
    CSeekDlg(LPMAPITABLE lpTbl, LPBKLIST lpbkl, CWnd* lpParent = NULL)
        : CModalDialog(IDD_SEEK, lpParent)
    {
        m_lpTable = lpTbl;
        m_lpBkList = lpbkl;
    }

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();

    afx_msg void OnTypeSeekRow();
    afx_msg void OnTypeSeekRowApprox();
    afx_msg void OnBkBeginning();
    afx_msg void OnBkCurrent();
    afx_msg void OnBkEnd();
    afx_msg void OnBkUser();

    DECLARE_MESSAGE_MAP();
};



/*
 -  CFindDlg
 -
 *  Purpose:
 *
 *
 */

class CFindDlg : public CModalDialog
{
private:
    LPMAPITABLE     m_lpTable;
    LPBKLIST        m_lpBkList;
    LPSRestriction  m_lpFindRes;
    int             m_nBk;
    ULONG           m_ulFlags;

public:
    CFindDlg(LPMAPITABLE lpTbl, LPBKLIST lpbkl, CWnd* lpParent = NULL)
        : CModalDialog(IDD_FIND, lpParent)
    {
        m_lpTable   = lpTbl;
        m_lpBkList  = lpbkl;
        m_lpFindRes = NULL;
    }

    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    
    afx_msg void OnBuildRes();
    afx_msg void OnDirForward();
    afx_msg void OnDirBackward();
    afx_msg void OnDirInvalid();
    afx_msg void OnBkBeginning();
    afx_msg void OnBkCurrent();
    afx_msg void OnBkEnd();
    afx_msg void OnBkUser();

    DECLARE_MESSAGE_MAP();
};


/* Misc. Function Prototypes */
void AddBookmark(LPBKLIST lpBkList, BOOKMARK bk, LPSPropValue lpProp);
void RemoveBookmark(LPBKLIST lpBkList, DWORD idxBk);
void FreeRestriction(LPSRestriction lpRes);
#ifdef WIN16
void SetStatus(LPSTR szMsg, SCODE);
#endif
void SetStatus(LPSTR szMsg, HRESULT);

#endif  /* __TBLVIEW_H__ */
