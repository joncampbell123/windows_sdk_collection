// mainwnd.cpp : Defines the behavior of the main window for ShowFont.
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

#include "showfont.h"

/////////////////////////////////////////////////////////////////////////////

static UINT idPaint = 0;
static CFont oemFont;
static CFont ansiFixedFont;
static CFont ansiVarFont;
static CFont deviceFont;

// used for mutual exclusion of menus
static UINT idPrevVAlign = IDM_ALIGNBASE;
static UINT idPrevHAlign = IDM_ALIGNLEFT;
static UINT idPrevFont = IDM_SYSTEM;

/////////////////////////////////////////////////////////////////////////////
// Initialization and cleanup

CMainWindow::CMainWindow(const char* szTitle)
{
	VERIFY(Create(NULL, szTitle,
		WS_OVERLAPPEDWINDOW, rectDefault, NULL, "ShowFont"));
}

int CMainWindow::OnCreate(LPCREATESTRUCT)
{
	OnFontChange();
	myFont = new CFont;
	myFont->CreateFont(
		10,                                      /* height           */
		10,                                      /* width            */
		0,                                       /* escapement       */
		0,                                       /* orientation      */
		FW_NORMAL,                               /* weight           */
		FALSE,                                   /* italic           */
		FALSE,                                   /* underline        */
		FALSE,                                   /* strikeout        */
		OEM_CHARSET,                             /* charset          */
		OUT_DEFAULT_PRECIS,                      /* out precision    */
		CLIP_DEFAULT_PRECIS,                     /* clip precision   */
		DEFAULT_QUALITY,                         /* quality          */
		FIXED_PITCH | FF_MODERN,                 /* pitch and family */
		"Courier");                              /* typeface         */

	VERIFY(oemFont.CreateStockObject(OEM_FIXED_FONT));
	VERIFY(ansiFixedFont.CreateStockObject(ANSI_FIXED_FONT));
	VERIFY(ansiVarFont.CreateStockObject(ANSI_VAR_FONT));
	VERIFY(systemFont.CreateStockObject(SYSTEM_FONT));
	VERIFY(deviceFont.CreateStockObject(DEVICE_DEFAULT_FONT));

	pTheFont = &systemFont;
	LOGFONT logFont;
	pTheFont->GetObject(sizeof(LOGFONT), &logFont);
	strcpy(szWindowTitle, szAppName);
	strcat(szWindowTitle, "SYSTEM");
	SetWindowText(szWindowTitle);

	for (int i=0; i<64; i++)
	{
		outputText[0][i] = i;
		outputText[1][i] = i+64;
		outputText[2][i] = i+128;
		outputText[3][i] = i+192;
	}
	return 0;
}

void CMainWindow::SetFaceName()
{
	char buf[80];

	CClientDC dc(this);
	dc.SelectObject(pTheFont);
	strcpy(szWindowTitle, szAppName);
	dc.GetTextFace(80, buf);
	strcat(szWindowTitle, buf);
	SetWindowText(szWindowTitle);
}

void CMainWindow::OnExit()
{
	DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// Main message handling

void CMainWindow::OnPaint()
{
	CPaintDC dc(this);
	InitDC(dc); // Prepare the DC in a common way.

	// Character set is drawn as a background.
	//
	if (idPaint == IDM_SHOWCHARSET)
		ShowCharacterSet(dc, pTheFont);
}

void CMainWindow::OnLButtonUp(UINT, CPoint pt)
{
	// Draw the string where the mouse was clicked.
	//
	ptCurrent = pt;
	ShowString();
}

void CMainWindow::OnShowCharSet()
{
	Invalidate(TRUE);
	idPaint = GetCurrentMessage()->wParam;
}

void CMainWindow::OnClear()
{
	// Clear the window.
	//
	Invalidate(TRUE);
	idPaint = 0;
}

/////////////////////////////////////////////////////////////////////////////
// Menu commands for selecting from standard fonts.

void CMainWindow::SetFontFromMenu(CFont& newFont)
{
	// call from inside message callback
	UINT id = GetCurrentMessage()->wParam;

	pTheFont = &newFont;
	SetFaceName();                  /* Sets window title for us. */
	GetMenu()->CheckMenuItem(idPrevFont, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(id, MF_CHECKED);
	idPrevFont = id;
}

void CMainWindow::OnOemFont()
{
	SetFontFromMenu(oemFont);
}

void CMainWindow::OnAnsiFixedFont()
{
	SetFontFromMenu(ansiFixedFont);
}

void CMainWindow::OnAnsiVarFont()
{
	SetFontFromMenu(ansiVarFont);
}

void CMainWindow::OnSystemFont()
{
	SetFontFromMenu(systemFont);
}

void CMainWindow::OnDeviceFont()
{
	SetFontFromMenu(deviceFont);
}

/////////////////////////////////////////////////////////////////////////////

// This class handles the menu commands for selecting drawing colors.
//
// Note we completely define this modal dialog here, then use it in the
// CMainWindow member functions, OnSetTextColor and OnSetBackgroundColor.

class CColorDlg : public CModalDialog
{
public:
	CColorDlg() : CModalDialog("Colors")
		{ }

	// Attributes
	DWORD m_rgbColor; // set this before invoking, and retrieve it afterwards

	UINT GetRed()
		{ return GetDlgItemInt(ID_RED, NULL, FALSE); }
	void SetRed(UINT nColor)
		{ SetDlgItemInt(ID_RED, nColor, FALSE); }

	UINT GetGreen()
		{ return GetDlgItemInt(ID_GREEN, NULL, FALSE); }
	void SetGreen(UINT nColor)
		{ SetDlgItemInt(ID_GREEN, nColor, FALSE); }

	UINT GetBlue()
		{ return GetDlgItemInt(ID_BLUE, NULL, FALSE); }
	void SetBlue(UINT nColor)
		{ SetDlgItemInt(ID_BLUE, nColor, FALSE); }

	BOOL OnInitDialog()
	{
		SetRed(GetRValue(m_rgbColor));
		SetGreen(GetGValue(m_rgbColor));
		SetBlue(GetBValue(m_rgbColor));
		return TRUE;
	}

	void OnOK()
	{
		m_rgbColor = RGB(GetRed(), GetGreen(), GetBlue());
		EndDialog(IDOK);
	}

	// Note: OnCancel predefined to end dialog and return IDCANCEL.
};

void CMainWindow::OnSetTextColor()
{
	CColorDlg   dlg;
	dlg.m_rgbColor = rgbTextColor;
	if (dlg.DoModal() == IDOK)
		rgbTextColor = dlg.m_rgbColor;
}

void CMainWindow::OnSetBackgroundColor()
{
	CColorDlg   dlg;
	dlg.m_rgbColor = rgbBkColor;
	if (dlg.DoModal() == IDOK)
		rgbBkColor = dlg.m_rgbColor;
}

/////////////////////////////////////////////////////////////////////////////

// These handle the menu commands for selecting drawing modes.

void CMainWindow::OnSetOpaque()
{
	nBkMode = OPAQUE;
	GetMenu()->CheckMenuItem(IDM_TRANSPARENT, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(IDM_OPAQUE, MF_CHECKED);
}

void CMainWindow::OnSetTransparent()
{
	nBkMode = TRANSPARENT;
	GetMenu()->CheckMenuItem(IDM_OPAQUE,  MF_UNCHECKED);
	GetMenu()->CheckMenuItem(IDM_TRANSPARENT,  MF_CHECKED);
}

/////////////////////////////////////////////////////////////////////////////

// These routines handle the menu commands for selecting drawing alignment.

void CMainWindow::SetAlignFromMenu(short newAlign)
{
	// This routine assumes that a function listed in a message map is
	// calling us, so that "GetCurrentMessage" works.
	//
	UINT id = GetCurrentMessage()->wParam;

	nAlignLCR = newAlign;
	GetMenu()->CheckMenuItem(idPrevHAlign, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(id, MF_CHECKED);
	idPrevHAlign = id;
}

void CMainWindow::OnSetAlignLeft()
{
	SetAlignFromMenu(TA_LEFT);
}

void CMainWindow::OnSetAlignCenter()
{
	SetAlignFromMenu(TA_CENTER);
}

void CMainWindow::OnSetAlignRight()
{
	SetAlignFromMenu(TA_RIGHT);
}

void CMainWindow::SetVAlignFromMenu(short newAlign)
{
	// This routine assumes that a function listed in a message map is
	// calling us, so that "GetCurrentMessage" works.
	//
	UINT id = GetCurrentMessage()->wParam;

	nAlignTBB = newAlign;
	GetMenu()->CheckMenuItem(idPrevVAlign, MF_UNCHECKED);
	GetMenu()->CheckMenuItem(id, MF_CHECKED);
	idPrevVAlign = id;
}

void CMainWindow::OnSetAlignTop()
{
	SetVAlignFromMenu(TA_TOP);
}

void CMainWindow::OnSetAlignBase()
{
	SetVAlignFromMenu(TA_BASELINE);
}

void CMainWindow::OnSetAlignBottom()
{
	SetVAlignFromMenu(TA_BOTTOM);
}

/////////////////////////////////////////////////////////////////////////////

// OnCreateFont:
// Handles the menu command for creating a new font.
//
void CMainWindow::OnCreateFont()
{
	LOGFONT logFont;

	myFont->GetObject(sizeof(LOGFONT), &logFont);
	if (DoCreateFontDlg(this, logFont))
	{
		delete myFont;
		pTheFont = myFont = new CFont;
		pTheFont->CreateFontIndirect(&logFont);
		SetFaceName();
		InvalidateRect(NULL, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////

// CMainWindow message map:
//
BEGIN_MESSAGE_MAP(CMainWindow, CFrameWnd)

	// Window messages
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_FONTCHANGE()

	// File menu
	ON_COMMAND(IDM_ADDFONT, OnAddFont)
	ON_COMMAND(IDM_DELFONT, OnDeleteFont)
	ON_COMMAND(IDM_EXIT, OnExit)
	ON_COMMAND(IDM_ABOUT, OnAbout)

	// Show menu
	ON_COMMAND(IDM_SHOWSTRING, ShowString)
	ON_COMMAND(IDM_SHOWCHARSET, OnShowCharSet)
	ON_COMMAND(IDM_SHOWLOGFONT, OnShowLogFont)
	ON_COMMAND(IDM_SHOWTEXTMETRICS, OnShowTextMetric)
	ON_COMMAND(IDM_CLEAR, OnClear)

	// Font menu
	ON_COMMAND(IDM_SYSTEM, OnSystemFont)
	ON_COMMAND(IDM_ANSIFIXED, OnAnsiFixedFont)
	ON_COMMAND(IDM_ANSIVAR, OnAnsiVarFont)
	ON_COMMAND(IDM_OEM, OnOemFont)
	ON_COMMAND(IDM_DEVICEDEF, OnDeviceFont)
	ON_COMMAND(IDM_SELECTFONT, OnSelectFont)
	ON_COMMAND(IDM_CFONT, OnCreateFont)

	// Options menu
	ON_COMMAND(IDM_TEXTCOLOR, OnSetTextColor)
	ON_COMMAND(IDM_BACKGROUNDCOLOR, OnSetBackgroundColor)
	ON_COMMAND(IDM_OPAQUE, OnSetOpaque)
	ON_COMMAND(IDM_TRANSPARENT, OnSetTransparent)
	ON_COMMAND(IDM_ALIGNLEFT, OnSetAlignLeft)
	ON_COMMAND(IDM_ALIGNCENTER, OnSetAlignCenter)
	ON_COMMAND(IDM_ALIGNRIGHT, OnSetAlignRight)
	ON_COMMAND(IDM_ALIGNTOP, OnSetAlignTop)
	ON_COMMAND(IDM_ALIGNBASE, OnSetAlignBase)
	ON_COMMAND(IDM_ALIGNBOTTOM, OnSetAlignBottom)
END_MESSAGE_MAP()
