// minmdi.h : Declares the class interfaces for MinMDI.
//            This application is the simplest Multiple Document Interface
//            (MDI) program.  It demonstrates how the CMDIFrameWnd and
//            CMDIChildWnd classes can be used together to make an MDI
//            application.
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

#ifndef __MINMDI_H__
#define __MINMDI_H__

/////////////////////////////////////////////////////////////////////////////

class CMainWindow : public CMDIFrameWnd
{
public:
	// Constructor.
	CMainWindow();

	// Message handlers.
	afx_msg void OnAbout();
	afx_msg void OnNewWindow();

	// Message map in the minmdi.cpp file.
	DECLARE_MESSAGE_MAP();
};

/////////////////////////////////////////////////////////////////////////////

class CTheApp : public CWinApp
{
public:
	BOOL InitInstance();
};

/////////////////////////////////////////////////////////////////////////////

#endif // __MINMDI_H__

