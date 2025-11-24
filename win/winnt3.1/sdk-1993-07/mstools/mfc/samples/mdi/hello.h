// hello.h : Declares the class interfaces for the Hello child window.
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

#ifndef __HELLO_H__
#define __HELLO_H__

/////////////////////////////////////////////////////////////////////////////

// class CHelloWnd

class CHelloWnd : public CMDIChildWnd
{
private:
	short m_nColor;
	COLORREF m_clrText;
	CMenu* m_pMenuCurrent;
	BOOL m_bWindowActive;

public:
	BOOL Create(LPCSTR szTitle, LONG style = 0,
				const RECT& rect = rectDefault,
				CMDIFrameWnd* pParent = NULL);

	CHelloWnd();
	virtual ~CHelloWnd();

	// message handlers

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnMDIActivate(BOOL bActivate,
					CWnd* pActivateWnd, CWnd* pDeactivateWnd);

	afx_msg void OnColor();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __HELLO_H__

