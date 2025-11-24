// chartdlg.h : Declares the interfaces for the Entry and print abort
//              dialogs.  The Entry dialog layout is defined in
//              entry.dlg; print abort is defined in chart.rc
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.
//

#ifndef __CHARTDLG_H__
#define __CHARTDLG_H__

/////////////////////////////////////////////////////////////////////////////
// class CEntryDialog

class CEntryDialog : public CModalDialog
{
public:
	// Chart data
	CChartObject* m_pData;

	// Current listbox selection index
	int m_nIndex;

	CEntryDialog(CWnd* pWndParent = NULL)
		: CModalDialog("EntryDlg", pWndParent)
		{ 
			m_pData = NULL;
			m_nIndex = -1;
		}

	void DoModal(CChartObject*);
	BOOL SetupArrayStructure();
	void ClearEditBoxes();

	BOOL OnInitDialog ();
	void OnOK();

	// Message handlers
	//
	afx_msg void OnBtnAdd();
	afx_msg void OnBtnDel();
	afx_msg void OnListSelChange();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPrintDlgBox

class CPrintDlgBox : public CDialog
{
public:
	CPrintDlgBox(); 

	BOOL OnInitDialog();
	void OnCancel();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __CHARTDLG_H__

