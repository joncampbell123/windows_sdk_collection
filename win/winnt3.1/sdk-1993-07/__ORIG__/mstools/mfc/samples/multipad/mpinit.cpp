// mpinit.cpp : Defines the application initialization.
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
#pragma code_seg("_MPINIT")
#endif

// InitInstance:
// Does the typical printer and frame initialization, as well as loading a
// file if one is listed on the command line.  Note that only one filename
// on the command line is currently supported.
//
BOOL CMultiPad::InitInstance()
{
	extern void LoadMRU();
	extern CPrinter* thePrinter;
	char szCmdLine[128];
	char* pCmdLine;
	
	LoadMRU();
	
	// Create the frame.
	//
	m_pMainWnd = new CMPFrame(m_pszAppName);
	if (m_pMainWnd->m_hWnd == NULL)
		return FALSE;
	
	// Create the printer object
	thePrinter = new CPrinter;

	// Load main menu accelerators.
	//
	if (!CMPFrame::GetMDIFrameWnd()->LoadAccelTable(MAKEINTRESOURCE(IDMULTIPAD)))
		return FALSE;

	// Display the frame window.
	//
	m_pMainWnd->ShowWindow(m_nCmdShow);
	m_pMainWnd->UpdateWindow();
	
	// If the command line string is empty, nullify the pointer to it,
	// otherwise copy command line into our data segment.
	//
	if (m_lpCmdLine == NULL || m_lpCmdLine[0] == '\0')
	{
		pCmdLine = NULL;
	}
	else
	{
		pCmdLine = szCmdLine;
		lstrcpy(pCmdLine, m_lpCmdLine);
	}
	
	// Add the first MDI window.
	//
	new CMPChild(pCmdLine);

	return TRUE;
}
