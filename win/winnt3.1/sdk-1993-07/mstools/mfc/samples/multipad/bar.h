// bar.h : Defines the interfaces to the status-bar window classes.
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

#ifndef __BAR_H__
#define __BAR_H__

/////////////////////////////////////////////////////////////////////////////

// CBarWnd
// A CBarWnd is a window which has the typical Windows three-dimensional
// "chiseled" look to it, and can be used (by deriving new classes from it)
// to create status bars, toolbars, etc.

class CBarWnd : public CWnd
{
private:
	static HCURSOR m_hCursor;
	static CString m_szRegistration;

public:
	BOOL Create(CWnd* pParentWnd, CRect rect);
	
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(NCCALCSIZE_PARAMS FAR* lpcsps);
	
	DECLARE_MESSAGE_MAP()
};

// CStatBar
// A CStatBar is a CBarWnd which acts as a status bar.  It is very similar
// in operation to a plain static text label in dialogs, but it has the
// chiseled look of a CBarWnd.  Typically, it is put along the bottom of a
// frame window and the text set according to the status of the application.
//
// To set the text, use the SetText member function.  New text is drawn 
// when the window is next painted.

class CStatBar : public CBarWnd
{
private:
	CString m_string;
	CFont m_font;
	
public:
	BOOL Create(CWnd* pParentWnd, CRect rect);
	
	afx_msg void OnPaint();

	void SetText(char* sz)
		{ m_string = sz; Invalidate(FALSE); }
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __BAR_H__

