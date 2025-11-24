// mainwnd.h : Declares the main window class for ShowFont.
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

#ifndef __MAINWND_H__
#define __MAINWND_H__

/////////////////////////////////////////////////////////////////////////////

class CMainWindow : public CFrameWnd
{
public:
	CMainWindow(const char* title);

	// Operations
	void    InitDC(CDC& dc);
	void    SetFaceName();
	void    SetFontFromMenu(CFont& newFont);
	void    SetAlignFromMenu(short newAlign);
	void    SetVAlignFromMenu(short newAlign);

	// Window message callbacks
	afx_msg int     OnCreate(LPCREATESTRUCT cs);
	afx_msg void    OnPaint();
	afx_msg void    OnLButtonUp(UINT, CPoint pt);
	afx_msg void    OnFontChange();
	afx_msg void    OnDestroy();

	// File menu
	afx_msg void    OnAddFont();
	afx_msg void    OnDeleteFont();
	afx_msg void    OnExit();
	afx_msg void    OnAbout();

	// Show menu
	afx_msg void    ShowString();
	afx_msg void    OnShowCharSet();
	afx_msg void    OnShowLogFont();
	afx_msg void    OnShowTextMetric();
	afx_msg void    OnClear();

	// Font menu
	afx_msg void    OnSystemFont();
	afx_msg void    OnAnsiFixedFont();
	afx_msg void    OnAnsiVarFont();
	afx_msg void    OnOemFont();
	afx_msg void    OnDeviceFont();
	afx_msg void    OnSelectFont();
	afx_msg void    OnCreateFont();

	// Option menu
	afx_msg void    OnSetTextColor();
	afx_msg void    OnSetBackgroundColor();
	afx_msg void    OnSetOpaque();
	afx_msg void    OnSetTransparent();
	afx_msg void    OnSetAlignLeft();
	afx_msg void    OnSetAlignCenter();
	afx_msg void    OnSetAlignRight();
	afx_msg void    OnSetAlignTop();
	afx_msg void    OnSetAlignBase();
	afx_msg void    OnSetAlignBottom();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif __MAINWND_H__

