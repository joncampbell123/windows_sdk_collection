// mdi.h : Declares the class interfaces for the MDI sample application.
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

#ifndef __AFXMDI_H__
#define __AFXMDI_H__

#include <afxwin.h>
#include <afxdlgs.h>

#include "hello.h"
#include "bounce.h"
#include "resource.h"
#include "common.h"

/////////////////////////////////////////////////////////////////////////////

class CMainWindow : public CMDIFrameWnd
{
public:
	CMenu*    m_pMenuInit;

	CMainWindow();
	virtual ~CMainWindow();

	// message handlers

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAbout();
	afx_msg void OnNewHello();
	afx_msg void OnNewBounce();
	afx_msg void OnExit();       

	// custom message and handler used by child windows to notify
	// parent of death
	afx_msg LONG OnChildDestroy(UINT wParam, LONG lParam);
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CTheApp : public CWinApp
{
public:
	virtual BOOL InitInstance();
};

/////////////////////////////////////////////////////////////////////////////

#endif // __AFXMDI_H__

