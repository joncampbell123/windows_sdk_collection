// $$dlg_hfile$$.h : header file
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/////////////////////////////////////////////////////////////////////////////
// $$DLG_CLASS$$ dialog

class $$DLG_CLASS$$ : public $$DLG_BASE_CLASS$$
{
// Construction
public:
	$$DLG_CLASS$$(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA($$DLG_CLASS$$)
	enum { IDD = IDD_$$SAFE_ROOT$$_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL($$DLG_CLASS$$)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG($$DLG_CLASS$$)
	virtual BOOL OnInitDialog();
$$IF(ABOUT)	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
$$ENDIF
$$IF(HELP)
	afx_msg void OnDestroy();
$$ENDIF
$$IF(PRESS_ME_DIALOG)
	afx_msg void OnPressMe();
$$ENDIF
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
