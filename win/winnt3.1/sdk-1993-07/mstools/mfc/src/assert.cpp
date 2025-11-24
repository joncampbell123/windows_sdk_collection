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


/*
 *  NOTE: in separate module so it can replaced if needed
 */

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
int afxIgnoreAssertCount = 0;               // for testing diagnostics
#endif

#pragma optimize("q", off)

extern "C"
void PASCAL AfxAssertFailedLine(const char FAR* lpszFileName, int nLine)
{
#ifdef _DEBUG
	if (afxIgnoreAssertCount > 0)
	{
		afxIgnoreAssertCount--;
		return;
	}

#ifdef _WINDOWS
	char sz[255];
	static char BASED_CODE szTitle[] = "Assertion Failed!";
	static char BASED_CODE szMessage[] = "%s: File %s, Line %d";
	static char BASED_CODE szUnknown[] = "<unknown application>";

	wsprintf(sz, (LPCSTR)szMessage, 
		AfxGetApp() == NULL ? (LPCSTR)szUnknown : (LPCSTR)AfxGetAppName(), 
		lpszFileName, 
		nLine);

	if (afxTraceEnabled)
	{
		// assume the debugger or auxiliary port
		::OutputDebugString(szTitle);
		::OutputDebugString("\n\r");
		::OutputDebugString(sz);
		::OutputDebugString("\n\r");
#ifdef _NTWIN
		DebugBreak();
#else
		_asm { int 3 };
#endif
	}

	::MessageBox(NULL, sz, szTitle, MB_SYSTEMMODAL | MB_ICONHAND | MB_OK);
#else
	static char szMessage[] = "Assertion Failed: file %Fs, line %d\r\n";
	fprintf(stderr, szMessage, lpszFileName, nLine);
#endif // _WINDOWS

#else
	// parameters not used if non-debug
	(void)lpszFileName;
	(void)nLine;
#endif // _DEBUG

	AfxAbort();
}

#pragma optimize("", on)
