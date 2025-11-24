// bar.cpp : Defines a standard "chiseled" window, and a status bar window.
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

#include "multipad.h"

#ifndef _NTWIN
#pragma code_seg("_MPTEXT")
#endif

/////////////////////////////////////////////////////////////////////////////
// CBarWnd
// See bar.h for details.

BEGIN_MESSAGE_MAP(CBarWnd, CWnd)
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

/*static*/ HCURSOR CBarWnd::m_hCursor;
/*static*/ CString CBarWnd::m_szRegistration;

// Create:
// Upon creation, we register our type with Windows as necessary.
//
BOOL CBarWnd::Create(CWnd* pParentWnd, CRect rect)
{
	if(m_szRegistration.IsEmpty())
	{
		m_hCursor = LoadCursor(NULL, IDC_ARROW);
		m_szRegistration = AfxRegisterWndClass(0, m_hCursor,
			(HBRUSH)(COLOR_BTNFACE+1), 0);
	}

	return CWnd::Create(m_szRegistration, NULL,
		WS_CHILD | WS_VISIBLE | WS_BORDER, rect, pParentWnd, 0);
}

// OnNcCalcSize:
// We're being asked how large we should be, for a given client rectangle.
// We assume we have a border (because our Create gives us one), so we add
// that to the rectangle given to us.
//
void CBarWnd::OnNcCalcSize(NCCALCSIZE_PARAMS FAR* lpcsps)
{
	int cxBorder = GetSystemMetrics(SM_CXBORDER);
	int cyBorder = GetSystemMetrics(SM_CYBORDER);

	InflateRect(lpcsps->rgrc, -cxBorder, -(cyBorder * 2));
}

// OnNCPaint:
// Paint the non-client area of a Bar window.  This includes the black
// border rectangle (WS_BORDER is assumed to be TRUE), a white "border"
// near the top and a grey "border" near the bottom, to give us that
// chiseled look.
//
void CBarWnd::OnNcPaint()
{
	CWindowDC dc(this);
	CRect rc;
	int cxBorder = GetSystemMetrics(SM_CXBORDER);
	int cyBorder = GetSystemMetrics(SM_CYBORDER);
	
	GetWindowRect(rc);
	int cxWidth = rc.Width();
	int cyHeight = rc.Height();

	CBrush borderBrush(GetSysColor(COLOR_WINDOWFRAME));
	CBrush* oldBrush = dc.SelectObject(&borderBrush);
	dc.PatBlt(0, 0, cxWidth, cyBorder, PATCOPY);
	dc.PatBlt(0, cyBorder, cxBorder, cyHeight - cyBorder * 2, PATCOPY);
	dc.PatBlt(0, cyHeight - cyBorder, cxWidth, cyBorder, PATCOPY);
	dc.PatBlt(cxWidth - cxBorder, cyBorder, 
		cxBorder, cyHeight - cyBorder * 2, PATCOPY);

	dc.PatBlt(cxBorder, cyBorder, 
		cxWidth - cxBorder * 2, cyBorder, WHITENESS);

	CBrush shadowBrush(GetSysColor(COLOR_BTNSHADOW));
	dc.SelectObject(&shadowBrush);
	dc.PatBlt(cxBorder, cyHeight - cyBorder * 2, 
		cxWidth - cxBorder * 2, cyBorder, PATCOPY);

	dc.SelectObject(oldBrush);
}

/////////////////////////////////////////////////////////////////////////////
// CStatBar
// See bar.h for details.

BEGIN_MESSAGE_MAP(CStatBar, CBarWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()


BOOL CStatBar::Create(CWnd* pParentWnd, CRect rect)
{
	int cyStatBar;
	int cyBorder = GetSystemMetrics(SM_CYBORDER);

	if (!CBarWnd::Create(pParentWnd, rect))
		return FALSE;

	m_font.CreateFont(13, 0, 0, 0, 0, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_SWISS, NULL);

	CClientDC dc(this);
	CFont* oldFont = dc.SelectObject(&m_font);
	cyStatBar = dc.GetTextExtent("M", 1).cy + 5 * cyBorder + 4;
	dc.SelectObject(oldFont);

	SetWindowPos(NULL, 0, 0, rect.right - rect.left, cyStatBar, 
		SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	return TRUE;
}


// OnPaint:
// A status bar, in addition to the chiseled look of all CBarWnd's, has a
// line of text to be displayed.
//
void CStatBar::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	int cyBorder = GetSystemMetrics(SM_CYBORDER);

	GetClientRect(rc);

	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
	CFont* oldFont = dc.SelectObject(&m_font);
	dc.ExtTextOut(8, 0, ETO_OPAQUE, rc, m_string, 
				  m_string.GetLength(), NULL);
	dc.SelectObject(oldFont);
}
