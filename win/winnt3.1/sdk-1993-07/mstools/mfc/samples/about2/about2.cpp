// about2.cpp : Defines the class behaviors for the About2 application.
//              This application demonstrates typical modal dialog use,
//              painting special graphics on a modal dialog, and when
//              compared to the original C code, shows equivalent
//              functionality between C code and C++ Foundation code.
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
// Based on the About2 application by Charles Petzold.
// The original application appeared in
// "Programming Windows", Second Edition (pp. 417-423),
// Copyright (C) 1990 Charles Petzold,
// published by Microsoft Press. Used with permission.

#include <afxwin.h>
#include "resource.h"

#include "about2.h"

/////////////////////////////////////////////////////////////////////////////

// An application can only contain one CWinApp-derived instance

CAbout2App theApp;

/////////////////////////////////////////////////////////////////////////////

// Paint:
// A helper function which paints a window with the given figure.  Note
// it can either be called with an existing CDC or NULL.  If NULL is sent,
// this routine creates its own display context and discards it when
// finished.
//
void Paint(CWnd* wnd, CDC* dc, short nColor, short nFigure)
{
	static DWORD dwColor[8] = { RGB (0,     0, 0), RGB (  0,   0, 255),
								RGB (0,   255, 0), RGB (  0, 255, 255),
								RGB (255,   0, 0), RGB (255,   0, 255),
								RGB (255, 255, 0), RGB (255, 255, 255) } ;

	CDC* odc = dc;

	if(!odc)
	{
		wnd->InvalidateRect(NULL, TRUE);
		wnd->UpdateWindow(); // paints the background
		dc = wnd->GetDC();
	}

	CRect rect;
	wnd->GetClientRect(rect);

	CBrush brush;
	CBrush* oldbrush;
	brush.CreateSolidBrush(dwColor[nColor-IDD_BLACK]);
	oldbrush = dc->SelectObject(&brush);

	if(nFigure == IDD_RECT)
		dc->Rectangle(rect);
	else
		dc->Ellipse(rect);

	dc->SelectObject(oldbrush);
	brush.DeleteObject();

	if(!odc)
		wnd->ReleaseDC(dc);
}

/////////////////////////////////////////////////////////////////////////////
// CAbout2Dlg

// OnInitDialog:
// Called once when the DoModal member function is called.
// Set up the checked/unchecked or focused states of the child controls.
//
// Note that this member function does not need an entry in the message map.
//
BOOL CAbout2Dlg::OnInitDialog()
{
	CheckRadioButton(IDD_BLACK, IDD_WHITE, m_nColor);
	CheckRadioButton(IDD_RECT, IDD_ELL, m_nFigure);

	m_ctrl = GetDlgItem(IDD_PAINT);
	GetDlgItem(m_nColor)->SetFocus();

	return FALSE;
}

// OnColor:
// This is a custom message handler, which we call anytime a radio button
// between IDD_BLACK and IDD_WHITE are pushed.  We just set all the color
// radio buttons, and repaint the inset graphic m_ctrl.
//
void CAbout2Dlg::OnColor()
{
	// Since we cannot pass any arguments with the ON_COMMAND message map,
	// we use the GetCurrentMessage call to see which control was pushed.
	//
	m_nColor = GetCurrentMessage()->wParam;
	CheckRadioButton(IDD_BLACK, IDD_WHITE, m_nColor);

	Paint(m_ctrl, NULL, m_nColor, m_nFigure);
}

// OnFigure:
// Like OnColor, except this handles the IDD_RECT and IDD_ELL radio buttons.
//
void CAbout2Dlg::OnFigure()
{
	// We use the GetCurrentMessage call to see which control was pushed.
	//
	m_nFigure = GetCurrentMessage()->wParam;
	CheckRadioButton(IDD_RECT, IDD_ELL, m_nFigure);

	Paint(m_ctrl, NULL, m_nColor, m_nFigure);
}

// OnPaint:
// The dialog is not clean.  Let the base class (CModalDialog::OnPaint)
// do the regular paint behavior, then redraw the special inset figure in
// the m_ctrl control.
//
void CAbout2Dlg::OnPaint()
{
	CModalDialog::OnPaint();

	Paint(m_ctrl, NULL, m_nColor, m_nFigure);
}

// CAbout2Dlg message map:
// Each Windows message we handle is tied to an CAbout2Dlg member function.
//
BEGIN_MESSAGE_MAP(CAbout2Dlg, CModalDialog)

	// These are the click command notifications from our radio buttons.
	// Associate them all to our OnColor member function for processing.
	// These assume the function is "void OnColor()".
	//
	ON_COMMAND(IDD_BLACK, OnColor)
	ON_COMMAND(IDD_RED, OnColor)
	ON_COMMAND(IDD_GREEN, OnColor)
	ON_COMMAND(IDD_YELLOW, OnColor)
	ON_COMMAND(IDD_BLUE, OnColor)
	ON_COMMAND(IDD_MAGENTA, OnColor)
	ON_COMMAND(IDD_CYAN, OnColor)
	ON_COMMAND(IDD_WHITE, OnColor)

	// Associate both of these to the OnFigure member function.
	// These assume the function is "void OnFigure()".
	//
	ON_COMMAND(IDD_RECT, OnFigure)
	ON_COMMAND(IDD_ELL, OnFigure)

	// Associate the WM_PAINT message to the OnPaint member function.
	// This assumes the function is "void OnPaint()".
	//
	ON_WM_PAINT()

	// OnInitDialog and OnOK are automatically tied to the messaging system.

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainWnd

// CMainWnd constructor:
// When the class object is created, the constructor should tell Windows
// to create a window of the appropriate style and size.  The menu resource
// "About2" in about2.rc is also loaded for the window.
//
CMainWnd::CMainWnd()
{
	if (!Create(NULL, "About Box Demo Program",
		WS_OVERLAPPEDWINDOW, rectDefault, NULL, "About2"))
		 AfxAbort();
	m_nCurrentColor = IDD_BLACK;
	m_nCurrentFigure = IDD_RECT;
}

// OnPaint:
// The image of our client area needs to be refreshed.
//
void CMainWnd::OnPaint()
{
	CPaintDC dc(this);
	Paint(this, &dc, m_nCurrentColor, m_nCurrentFigure);
}

// OnAbout:
// The "About About2..." item of the menu was selected.  All we need to
// do is create a dialog box object (CAbout2Dlg), and tell it to start up.
// Note that if it is successful (the user pressed OK), then the image we
// are currently displaying may be invalid.
//
void CMainWnd::OnAbout()
{
	CAbout2Dlg box(this, m_nCurrentColor, m_nCurrentFigure);

	if(box.DoModal() == IDOK)
	{
		// The user accepted the changes so get the information from the
		// dialog object and invalidate the client area.
		m_nCurrentColor = box.GetColor();
		m_nCurrentFigure = box.GetFigure();
		InvalidateRect(NULL, TRUE);
	}
}

BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
	ON_COMMAND(IDM_ABOUT, OnAbout)

	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAbout2App

// InitInstance:
// Called once when the application begins.  All we need to do is to create
// a new CMainWnd object which can handle itself once made visible.
//
BOOL CAbout2App::InitInstance()
{
	m_pMainWnd = new CMainWnd();
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

