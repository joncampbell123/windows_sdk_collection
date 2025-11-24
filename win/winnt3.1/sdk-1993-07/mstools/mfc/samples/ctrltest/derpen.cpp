// derpen.cpp : C++ derived HEdit/BEdit control example
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

// we need the MFC extensions for PenWindows
#include <afxpen.h>

/////////////////////////////////////////////////////////////////////////////

class CDerPenEditDlg : public CModalDialog
{
protected:
	// 2 Handwriting edit items
	CHEdit edit1, edit2;
	// 2 Boxed Handwriting edit items
	CBEdit edit3, edit4;
	// static labels for them all
	CStatic static1, static2, static3, static4;

	// font for the dialog
	CFont*  m_pFont;
public:
	CDerPenEditDlg()
		: CModalDialog(IDD_DERIVED_EDIT)
		{ }

	BOOL OnInitDialog();
	void OnSetFont(CFont* pFont)
			{ m_pFont = pFont; }
	void OnOK();
};

/////////////////////////////////////////////////////////////////////////////
// pen helpers

void SetAlc(CHEdit& rHEdit, ALC alcNew)
{
	RC rc;      // recognition context
	VERIFY(rHEdit.GetRC(&rc));
	rc.alc = alcNew;
	VERIFY(rHEdit.SetRC(&rc));
}

/////////////////////////////////////////////////////////////////////////////

BOOL CDerPenEditDlg::OnInitDialog()
{
	// This is an example of the _incorrect_ way to create a dialog
	// see other comments in DERTEST.CPP
	const int yStart = 8;
	const int height = 36;

	CPoint whereLabel(10, yStart + 4);
	CSize sizeLabel(80, 24);

	CPoint whereEdit(90, yStart);
	CSize sizeEdit(140, 30);

	static1.Create("Letters:", WS_VISIBLE | WS_CHILD | SS_LEFT,
		CRect(whereLabel, sizeLabel), this, -1);
	static1.SetFont(m_pFont);
	whereLabel.y += height;
	edit1.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER,
		CRect(whereEdit, sizeEdit), this, IDC_EDIT1);
	edit1.SetFont(m_pFont);
	SetAlc(edit1, ALC_ALPHA);
	whereEdit.y += height;

	static2.Create("Numbers:", WS_VISIBLE | WS_CHILD | SS_LEFT,
		CRect(whereLabel, sizeLabel), this, -1);
	static2.SetFont(m_pFont);
	whereLabel.y += height;
	edit2.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER,
		CRect(whereEdit, sizeEdit), this, IDC_EDIT2);
	edit2.SetFont(m_pFont);
	SetAlc(edit2, ALC_NUMERIC);
	whereEdit.y += height;

	// followed by 2 boxed edit items
	static3.Create("Letters:", WS_VISIBLE | WS_CHILD | SS_LEFT,
		CRect(whereLabel, sizeLabel), this, -1);
	static3.SetFont(m_pFont);
	whereLabel.y += height;
	edit3.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER,
		CRect(whereEdit, sizeEdit), this, IDC_EDIT3);
	edit3.SetFont(m_pFont);
	SetAlc(edit3, ALC_ALPHA);
	whereEdit.y += height;

	static4.Create("Numbers:", WS_VISIBLE | WS_CHILD | SS_LEFT,
		CRect(whereLabel, sizeLabel), this, -1);
	whereLabel.y += height;
	static4.SetFont(m_pFont);
	edit4.Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER,
		CRect(whereEdit, sizeEdit), this, IDC_EDIT4);
	edit4.SetFont(m_pFont);
	SetAlc(edit4, ALC_NUMERIC);
	whereEdit.y += height;

	// change the dialog height so everything fits
	int yBottom = whereEdit.y + height * 2; // extra space
	CRect rect;
	GetWindowRect(rect);
	VERIFY(SetWindowPos(NULL, -1, -1, rect.Width(), yBottom,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW|SWP_NOACTIVATE));

	// set focus to first one
	edit1.SetFocus();
	return FALSE;   // focus set
}

void CDerPenEditDlg::OnOK()
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

void CTestWindow::OnTestPenEditFromCode()
{
	TRACE("running dialog containing CBEdit objects\n");
	CDerPenEditDlg dlg;
	dlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
