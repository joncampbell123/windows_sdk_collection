// ctrltest.h : main window class interface
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


#ifndef RC_INVOKED
#include <afxwin.h>
#else
#include <windows.h>
#include <afxres.h>
#endif

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////
// Menu command IDs

#define IDM_EXIT                    100
#define IDM_ABOUT                   101

// Simple tests
#define IDM_TEST_DERIVED_EDIT       200
#define IDM_TEST_WNDCLASS_EDIT      201
#define IDM_TEST_SUB_EDIT           202

// Pen edit tests
#define IDM_TEST_PENEDIT_CODE       300
#define IDM_TEST_PENEDIT_TEMPLATE   301
#define IDM_TEST_PENEDIT_FEATURES   302

// custom control tests
#define IDM_TEST_BITMAP_BUTTON1     400
#define IDM_TEST_BITMAP_BUTTON2     401
#define IDM_TEST_CUSTOM_MENU        402
#define IDM_TEST_CUSTOM_LIST        403
#define IDM_TEST_SPIN_EDIT          404

// custom menu test - menu ids: BASE + RGB bits : 8 colors max

#define IDM_COLOR_FIRST             500
#define IDM_COLOR_BLACK             (IDM_COLOR_FIRST + 0)
#define IDM_COLOR_BLUE              (IDM_COLOR_FIRST + 1)
#define IDM_COLOR_GREEN             (IDM_COLOR_FIRST + 2)
#define IDM_COLOR_CYAN              (IDM_COLOR_FIRST + 3)
#define IDM_COLOR_RED               (IDM_COLOR_FIRST + 4)
#define IDM_COLOR_MAGENTA           (IDM_COLOR_FIRST + 5)
#define IDM_COLOR_YELLOW            (IDM_COLOR_FIRST + 6)
#define IDM_COLOR_WHITE             (IDM_COLOR_FIRST + 7)

#define IDM_COLOR_LAST              (IDM_COLOR_FIRST + 7)

/////////////////////////////////
// Dialog IDs are same as related menu commands

#define IDD_ABOUT                   IDM_ABOUT
#define IDD_DERIVED_EDIT            IDM_TEST_DERIVED_EDIT
#define IDD_WNDCLASS_EDIT           IDM_TEST_WNDCLASS_EDIT
#define IDD_SUB_EDIT                IDM_TEST_SUB_EDIT
#define IDD_SUB_PENEDIT             IDM_TEST_PENEDIT_TEMPLATE
#define IDD_PENEDIT_FEATURES        IDM_TEST_PENEDIT_FEATURES
#define IDD_SPIN_EDIT               IDM_TEST_SPIN_EDIT
#define IDD_CUSTOM_LIST             IDM_TEST_CUSTOM_LIST

/////////////////////////////////
// Specific control commands and dialogs
// other special IDs for controls and dialogs

// configure local pen edit
#define IDC_CONFIGURE               1000
#define IDD_PENEDIT_CONFIGURE       IDC_CONFIGURE
#define IDC_LEFTHANDED              1001

// ALC configuration
#define IDC_ALC_FIRST               1100
#define IDC_ALC_LAST                (IDC_ALC_MIN + 31) // at most 32 bits

/////////////////////////////////
// other general control IDs

#define IDC_EDIT1       2101
#define IDC_EDIT2       2102
#define IDC_EDIT3       2103
#define IDC_EDIT4       2104

#define IDC_BUTTON1     2201
#define IDC_BUTTON2     2202
#define IDC_BUTTON3     2203
#define IDC_BUTTON4     2204

#define IDC_LISTBOX1    2301

/////////////////////////////////////////////////////////////////////////////
// ColorMenu - used for custom menu test
//   included here to show how it should be embedded as a member of the
//    main frame window that uses it

class CColorMenu : public CMenu
{
public:
// Operations
	void AppendColorMenuItem(UINT nID, COLORREF color);

// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	CColorMenu();
	virtual ~CColorMenu();
};

/////////////////////////////////////////////////////////////////////////////
// Main Window
//   used as the context for running all the tests

class CTestWindow : public CFrameWnd
{ 
public:
	// construction helpers
	void SetupMenus();

protected:
	// command handlers
	afx_msg void OnExit();
	afx_msg void OnAbout();

	// simple tests
	afx_msg void OnTestDerivedEdit();
	afx_msg void OnTestWndClassEdit();
	afx_msg void OnTestSubclassedEdit();

#ifndef _NTWIN
	// pen tests
	afx_msg void OnTestPenEditFromCode();
	afx_msg void OnTestPenEditFromTemplate();
	afx_msg void OnTestPenEditFeatures();
#endif

	// custom control tests
	afx_msg void OnTestBitmapButton1();
	afx_msg void OnTestBitmapButton2();
	afx_msg void OnTestCustomList();
	afx_msg void OnTestSpinEdit();

	// custom menu tests
	void AttachCustomMenu();
	CColorMenu  m_colorMenu;
	virtual BOOL OnCommand(UINT wParam, LONG lParam);

// Implementation
	virtual void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

