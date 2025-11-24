// about2.h : Declares the class interfaces for the About2 application.
//            This application demonstrates typical modal dialog use,
//            painting special graphics on a modal dialog, and when
//            compared to the original C code, shows equivalent
//            functionality between C code and C++ Foundation code.
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

#ifndef __ABOUT2_H__
#define __ABOUT2_H__

/////////////////////////////////////////////////////////////////////////////

// CAbout2Dlg:
// This class is the "AboutBox" modal dialog.  The dialog's controls
// are defined in the dialog editor resource script, about2.dlg.
//
class CAbout2Dlg : public CModalDialog
{
private:
	// These are used to keep information for the duration of the dialog.
	short m_nColor;
	short m_nFigure;
	CWnd* m_ctrl;

public:
	// Constructor.  We have to pass a parent window to the
	// CModalDialog constructor.  We know that we correspond
	// to the "AboutBox" dialog template (see about2.rc).
	// When we construct the about box object we set the
	// internal state variables to the color and figure requested.
	//
	CAbout2Dlg(CWnd* pParentWnd, short nColor, short nFigure )
			: CModalDialog("AboutBox", pParentWnd)
	{   m_nColor = nColor;
		m_nFigure = nFigure; }
			
	// Messages we handle.
	//
	afx_msg BOOL OnInitDialog();
	afx_msg void OnColor();
	afx_msg void OnFigure();
	afx_msg void OnPaint();

	// Data Access Functions
	short GetColor()    { return m_nColor; }
	short GetFigure()   { return m_nFigure; }

	// Message map defined in the .cpp file.
	//
	DECLARE_MESSAGE_MAP()
private:
	short nCurrentColor;
	short nCurrentFigure;
};

/////////////////////////////////////////////////////////////////////////////

// CMainWnd:
// This is the main window of the application.  It has a small menu,
// with a "Help" popup and an "About About2..." item (see about2.rc).
// The main window draws the current figure in its client area.
//
class CMainWnd : public CFrameWnd
{
public:
	// Constructor.
	//
	CMainWnd();

	// Messages we handle.
	//
	afx_msg void OnAbout();
	afx_msg void OnPaint();

	// Message map defined in the .cpp file.
	//
	DECLARE_MESSAGE_MAP();
private:
	short m_nCurrentColor;
	short m_nCurrentFigure;
};

/////////////////////////////////////////////////////////////////////////////

// CAbout2App:
// This class represents the application as a whole.  The base class CWinApp
// does most of this for us, such as creating and running the message loop.
//
class CAbout2App : public CWinApp
{
public:
	// This member function is called automatically, so we can set up
	// our application-specific information.
	//
	BOOL InitInstance();
};

/////////////////////////////////////////////////////////////////////////////

#endif // __ABOUT2_H__

