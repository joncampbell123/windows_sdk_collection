// showfont.cpp : Defines the behavior of the major classes of ShowFont.
//                ShowFont is a simple Windows font parameter viewing and
//                modifying utility program.
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
// definition of common globals

CFont* pTheFont;    // current font
CFont* myFont;      // created font
CFont systemFont;

char outputText[4][64];
int nLineSpace;

CPoint ptCurrent(0, 0);
short nBkMode = OPAQUE;
short nAlignLCR = TA_LEFT;
short nAlignTBB = TA_TOP; 
char szAppName[] = "ShowFont Sample Application   Font: ";
char szWindowTitle[80];

// default color values
DWORD rgbTextColor = ::GetSysColor(COLOR_WINDOWTEXT);
DWORD rgbBkColor = ::GetSysColor(COLOR_WINDOW);

/////////////////////////////////////////////////////////////////////////////
// Main application initialization.

CShowFontApp theApp;

BOOL CShowFontApp::InitInstance()
{
	// Create the main window.  The caption includes the font name, SYSTEM.
	//
	strcpy(szWindowTitle, szAppName);
	strcat(szWindowTitle, "SYSTEM");
	m_pMainWnd = new CMainWindow(szWindowTitle);

	// Make the window visible; update its client area; and return "success".
	//
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

