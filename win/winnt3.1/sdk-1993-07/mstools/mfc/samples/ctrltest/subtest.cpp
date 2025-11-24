// subedit.cpp : SubClassed Edit control example
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

#include "ctrltest.h"
#include "paredit.h"

/////////////////////////////////////////////////////////////////////////////
// Dialog class

class CSubEditDlg : public CModalDialog
{
protected:
	CParsedEdit edit1, edit2, edit3, edit4;
public:
	CSubEditDlg()
		: CModalDialog(IDD_SUB_EDIT)
			{ }

	BOOL OnInitDialog();
	void OnOK();
};

BOOL CSubEditDlg::OnInitDialog()
{
	edit1.SubclassEdit(IDC_EDIT1, this, PES_LETTERS);
	edit2.SubclassEdit(IDC_EDIT2, this, PES_NUMBERS);
	edit3.SubclassEdit(IDC_EDIT3, this, PES_NUMBERS | PES_LETTERS);
	edit4.SubclassEdit(IDC_EDIT4, this, PES_ALL);
	return TRUE;
}

void CSubEditDlg::OnOK()
{
#ifdef _DEBUG
	// dump results, normally you would do something with these
	CString s;
	edit1.GetWindowText(s);
	TRACE("edit1 = '%s'\n", (const char*) s);
	edit2.GetWindowText(s);
	TRACE("edit2 = '%s'\n", (const char*) s);
	edit3.GetWindowText(s);
	TRACE("edit3 = '%s'\n", (const char*) s);
	edit4.GetWindowText(s);
	TRACE("edit4 = '%s'\n", (const char*) s);
#endif

	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// Run the test

void CTestWindow::OnTestSubclassedEdit()
{
	TRACE("running dialog containing edit items aliased to ParsedEdits\n");
	CSubEditDlg dlg;
	dlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
