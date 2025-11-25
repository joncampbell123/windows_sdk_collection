// This code and information is provided "as is" without warranty of
// any kind, either expressed or implied, including but not limited to
// the implied warranties of merchantability and/or fitness for a
// particular purpose.

// Copyright (C) 1996 - 1997 Intel corporation.  All rights reserved.

// indeo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIndeo form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <afxcmn.h>  // for CSliderCtrl

class CIndeo : public CFormView
{
protected:
	CIndeo();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CIndeo)
	
	DWORD	m_dwInitTransRGBVal;
	COLORREF m_clrefUserCustClrs[16];
	
// Form Data
public:
	//{{AFX_DATA(CIndeo)
	enum { IDD = IDD_INDEO };
	CButton	m_btnCustomColors;
	CButton	m_checkDontDropQuality;
	CButton	m_checkDontDropFrames;
	CEdit	m_editSaturation;
	CEdit	m_editContrast;
	CEdit	m_editBrightness;
	CEdit	m_editTransFillGreen;
	CEdit	m_editTransFillBlue;
	CEdit	m_editTransFillRed;
	CButton	m_checkAltLine;
	CEdit	m_editDecodeTime;
	CEdit	m_editViewWidth;
	CEdit	m_editViewY;
	CEdit	m_editViewX;
	CEdit	m_editViewHeight;
	CEdit	m_editKeyValue;
	CEdit	m_editDecodeY;
	CEdit	m_editDecodeX;
	CEdit	m_editDecodeWidth;
	CButton	m_checkAccessKey;
	CButton	m_btnDefaults;
	CButton	m_btnApply;
	CEdit	m_editDecodeHeight;
	CSliderCtrl	m_tbSaturation;
	CSliderCtrl	m_tbContrast;
	CSliderCtrl	m_tbBrightness;
	BOOL	m_bAccessKey;
	DWORD	m_dwDecodeHeight;
	DWORD	m_dwDecodeWidth;
	DWORD	m_dwDecodeX;
	DWORD	m_dwDecodeY;
	DWORD	m_dwKeyValue;
	DWORD	m_dwViewHeight;
	DWORD	m_dwViewWidth;
	DWORD	m_dwViewX;
	DWORD	m_dwViewY;
	int		m_intBrightness;
	int		m_intContrast;
	int		m_intSaturation;
	DWORD	m_dwDecodeTime;
	CButton m_radTransFill;
	CButton m_radTransNoFill;
	BOOL	m_bAltLine;
	int		m_intGreen;
	int		m_intRed;
	int		m_intBlue;
	BOOL	m_bDontDropFrames;
	BOOL	m_bDontDropQuality;
	CButton m_radFrameOrigin;
	CButton m_radViewOrigin;
	int		m_bViewOrigin;
	int		m_bTransFill;
	//}}AFX_DATA
	
// Attributes
public:

// Operations
public:
   	void EnableSeqOptions();
	void DisableSeqOptions();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIndeo)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CIndeo();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Helper functions
	void EnableAllOptions();
	void DisableAllOptions();

	// Generated message map functions
	//{{AFX_MSG(CIndeo)
	afx_msg void OnIndeoApply();
	afx_msg void OnIndeoDefaults();
	afx_msg void OnChangeDecodeHeight();
	afx_msg void OnChangeDecodeWidth();
	afx_msg void OnChangeDecodeX();
	afx_msg void OnChangeDecodeY();
	afx_msg void OnChangeViewHeight();
	afx_msg void OnChangeViewWidth();
	afx_msg void OnChangeViewX();
	afx_msg void OnChangeViewY();
	afx_msg void OnAccesskey();
	afx_msg void OnChangeDecodeTime();
	afx_msg void OnChangeKeyValue();
	afx_msg void OnTransFill();
	afx_msg void OnTransNofill();
	afx_msg void OnAltline();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeGreenEdit();
	afx_msg void OnChangeRedEdit();
	afx_msg void OnChangeBlueEdit();
	afx_msg void OnDontDropFrames();
	afx_msg void OnDontDropQuality();
	afx_msg void OnViewOrigin();
	afx_msg void OnFrameOrigin();
	afx_msg void OnCustomColors();
	afx_msg void OnUpdateBSCText();
	afx_msg void OnClickBSCColorcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
