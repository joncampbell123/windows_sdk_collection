// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#ifdef _WINDOWS
#include "afxwin.h"
#else
#include "afx.h"
#endif
#pragma hdrstop

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG

// If you want to route afxDump output to a different location than
// the default, just copy this file to your application build directory,
// modify the afxDumpFile and link the new object module into your program.

// You must have AFX.INI (from \C700\MFC\SRC) in your Windows 
// directory if you desire diagnostic output.

// See TN007.TXT for a description of afxTraceFlags and afxTraceEnabled.

#ifndef _WINDOWS
static CStdioFile _afxDumpFile(stderr);
CDumpContext NEAR _afxDump(&_afxDumpFile);
#else
CDumpContext NEAR _afxDump(NULL);
#endif //!_WINDOWS
CDumpContext& afxDump = _afxDump;

static char BASED_CODE szIniFile[] = "AFX.INI";
static char BASED_CODE szDiagSection[] = "Diagnostics";

extern "C" BOOL 
AfxDiagnosticInit(void)
{
#ifdef _WINDOWS
	afxTraceEnabled = ::GetPrivateProfileInt(szDiagSection, "TraceEnabled", 
		FALSE, szIniFile);
	afxTraceFlags = ::GetPrivateProfileInt(szDiagSection, "TraceFlags", 
		0, szIniFile);
#else
	afxTraceEnabled = 1; // dump to stderr by default
#endif
	return TRUE;
}

#endif
