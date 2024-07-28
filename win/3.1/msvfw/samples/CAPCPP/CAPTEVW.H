// captevw.h : interface of the CCaptestView class
//
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
// 
/////////////////////////////////////////////////////////////////////////////

#include "avicap.h"

class CCaptestView : public CView
{
protected: // create from serialization only
         CCaptestView();
         DECLARE_DYNCREATE(CCaptestView)
         virtual BOOL PreCreateWindow (CREATESTRUCT &cs);

// Attributes
public:
         CCaptestDoc* GetDocument();
         HWND hwndC;
         HWND hwndPlayback;
         CAPDRIVERCAPS CapDriverCaps;
         CAPSTATUS CapStatus;
         CAPTUREPARMS CaptureParms;
         int nDriverIndex;
         BOOL fDialogIsUp;
         
// Operations
public:  
         void CCaptestView::GetDriverList(void);
         void CCaptestView::ConnectToDriver(int nIndex);
         void CCaptestView::ResizeNow (void);
         void CCaptestView::Layout(void);
// Implementation
public:
         virtual ~CCaptestView();
         virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
         virtual void AssertValid() const;
         virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
         //{{AFX_MSG(CCaptestView)
         afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
         afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCapOverlay();
    afx_msg void OnCapPreview();
    afx_msg void OnCapDlgFormat();
    afx_msg void OnCapDlgDisplay();
    afx_msg void OnCapDlgSource();
    afx_msg void OnCapAutopal5();
    afx_msg void OnCapSequence();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditPaste();
    afx_msg void OnEditCopy();
    afx_msg void OnUpdateCapPreview(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCapOverlay(CCmdUI* pCmdUI);
    afx_msg void OnCapScale();
    afx_msg void OnUpdateCapScale(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCapDlgDisplay(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCapDlgFormat(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCapDlgSource(CCmdUI* pCmdUI);
    afx_msg void OnCapSetup();
    afx_msg void OnCapPlayback();
    afx_msg void OnUpdateCapPlayback(CCmdUI* pCmdUI);
    afx_msg void OnCapDrv0();
    afx_msg void OnUpdateCapDrv(CCmdUI* pCmdUI);
    afx_msg void OnCapDrv1();
    afx_msg void OnCapDrv2();
    afx_msg void OnCapDrv3();
    afx_msg void OnCapDrv4();
    afx_msg void OnCapDrv5();
    afx_msg void OnCapDrv6();
    afx_msg void OnCapDrv7();
    afx_msg void OnCapDrv8();
    afx_msg void OnCapDrv9();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnUpdateCapAutopal5(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCapSequence(CCmdUI* pCmdUI);
    afx_msg void OnUpdateCapSetup(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg BOOL OnQueryEndSession();
    //}}AFX_MSG
         DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG    // debug version in captevw.cpp
inline CCaptestDoc* CCaptestView::GetDocument()
   { return (CCaptestDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
