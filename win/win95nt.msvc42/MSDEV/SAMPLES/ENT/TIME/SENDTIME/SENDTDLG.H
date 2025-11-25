// SendTimeDlg.h : header file
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

#include "cl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendTimeDlg dialog

class CSendTimeDlg : public CDialog
{
// Construction
public:
        CSendTimeDlg(CWnd* pParent = NULL);        // standard constructor

// Dialog Data
        //{{AFX_DATA(CSendTimeDlg)
        enum { IDD = IDD_SENDTIME_DIALOG };
        CStatic        m_CurrentMachine;
        CProgressCtrl        m_SettimeProgress;
                // NOTE: the ClassWizard will add data members here
        //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CSendTimeDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
        //}}AFX_VIRTUAL

//Implementation
public:
        ComLoc myComLoc;        //make the class object available to the dialog class

protected:
        HICON m_hIcon;
        void CallMachine(LPCTSTR lpszMachine);
        char activeButtonInd;        //'I' = insert, 'U' = update, 'D' = delete, 'F' = find, ' ' = not active

        // Generated message map functions
        //{{AFX_MSG(CSendTimeDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnButtonFirst();
        afx_msg void OnButtonPrev();
        afx_msg void OnButtonNext();
        afx_msg void OnButtonLast();
        afx_msg void OnButtonFind();
        afx_msg void OnButtonInsert();
        afx_msg void OnButtonUpdate();
        afx_msg void OnButtonDelete();
        afx_msg void OnButtonApply();
        afx_msg void OnButtonCancel();
        afx_msg void OnButton1();
        afx_msg void OnButtonSettime();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        void EnableEditBoxes();
        void DisableEditBoxes();
        void EnableButtons();
        void DisableButtons();
        void GetText();
        void SetText();
        void ClearText();
        void HideBoxes();
        void ShowBoxes();
};
