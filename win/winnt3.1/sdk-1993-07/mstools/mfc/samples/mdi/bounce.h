// bounce.h : Declares the class interfaces for the Bounce child window.
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

#ifndef __BOUNCE_H__
#define __BOUNCE_H__

/////////////////////////////////////////////////////////////////////////////

class CBounceWnd : public CMDIChildWnd
{
private:

	// bounce window client area and bouncing ball color/size parameters

	short m_nColor, m_xPixel, m_yPixel, m_cxRadius, m_cyRadius;
	short m_cxMove, m_cyMove, m_cxClient, m_cyClient, m_cxTotal, m_cyTotal;
	short m_xCenter, m_yCenter;

	short m_nSpeed;

	// for replicating bouncing ball

	CBitmap* m_pBitmap;
	COLORREF m_clrBall;
	
	CMenu* m_pMenuCurrent;
	BOOL m_bWindowActive;

public:

	CBounceWnd();

	BOOL Create(LPCSTR szTitle, LONG style = 0,
				const RECT& rect = rectDefault,
				CMDIFrameWnd* pParent = NULL);

	virtual ~CBounceWnd();

	void MakeNewBall();

	// message handlers

	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMDIActivate(BOOL bActivate,
					   CWnd* pActivatedWnd, CWnd* pDeactivatedWnd);
	afx_msg void OnColor();
	afx_msg void OnSpeed();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __BOUNCE_H__

