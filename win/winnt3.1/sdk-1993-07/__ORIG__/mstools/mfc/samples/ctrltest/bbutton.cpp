// bbutton.cpp : bitmap button test
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

/////////////////////////////////////////////////////////////////////////////
// BitmapButton Test dialog #1

// In this example we pass the bitmap resource names in the constructor
//  for the buttons.  OnInitDialog is used to Subclass the buttons
//  so the dialog controls get attached to the MFC WndProc for C++
//  message map dispatch.

class CBMTest1Dlg : public CModalDialog
{
protected:
	// construct
	CBitmapButton button1, button2;
public:
	CBMTest1Dlg()
		: CModalDialog(IDM_TEST_BITMAP_BUTTON1),
			button1("Image1Up", "Image1Down", "Image1Focus"),
			button2("Image2Up", "Image2Down", "Image2Focus")
		{ }

	BOOL OnInitDialog();
	void OnOK();
};

BOOL CBMTest1Dlg::OnInitDialog()
{
	// each dialog control has special bitmaps
	VERIFY(button1.SubclassDlgItem(IDOK, this));
	button1.SizeToContent();
	VERIFY(button2.SubclassDlgItem(IDCANCEL, this));
	button2.SizeToContent();

	return TRUE;
}

void CBMTest1Dlg::OnOK()
{
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// BitmapButton Test dialog #2

// In this example we use the CBitmapButton AutoLoad member function.
//  Autoload uses the text/title of the button as the base resource name.
//  For this trivial example the buttons are called "OK" and "CANCEL",
//  which use the bitmaps "OKU", "OKD", "OKF", "CANCELU", "CANCELD"
//  and "CANCELF" respectively for the up, down and focused images.

#define ID_BUTTON_MIN       IDOK
#define N_BUTTONS   (IDCANCEL - ID_BUTTON_MIN + 1)

class CBMTest2Dlg : public CModalDialog
{
protected:
	// construct
	CBitmapButton buttons[N_BUTTONS];
		// array of buttons constructed with no attached bitmap images
public:
	CBMTest2Dlg()
		: CModalDialog(IDM_TEST_BITMAP_BUTTON2)
		{ }

	BOOL OnInitDialog();
	void OnOK();
};

BOOL CBMTest2Dlg::OnInitDialog()
{
	// load bitmaps for all the bitmap buttons (does SubclassButton as well)
	for (int i = 0; i < N_BUTTONS; i++)
		VERIFY(buttons[i].AutoLoad(ID_BUTTON_MIN + i, this));
	return TRUE;
}

void CBMTest2Dlg::OnOK()
{
	EndDialog(IDOK);
}

/////////////////////////////////////////////////////////////////////////////
// Test driver routines

void CTestWindow::OnTestBitmapButton1()
{
	CBMTest1Dlg dlg;
	dlg.DoModal();
}

void CTestWindow::OnTestBitmapButton2()
{
	CBMTest2Dlg dlg;
	dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
