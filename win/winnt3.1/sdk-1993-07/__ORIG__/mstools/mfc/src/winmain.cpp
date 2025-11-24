// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "afxwin.h"
#pragma hdrstop

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

/////////////////////////////////////////////////////////////////////////////
// Standard WinMain implementation
//  Can be replaced as long as 'AfxWinInit' is called first

#ifndef _WINDLL
extern "C"
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	int nReturnCode = -1;

	// AFX internal initialization
	if (!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
		goto InitFailure;

	// App global initializations (rare)
	if (hPrevInstance == NULL && !AfxGetApp()->InitApplication())
		goto InitFailure;

	// Perform specific initializations
	if (!AfxGetApp()->InitInstance())
		goto InitFailure;

	nReturnCode = AfxGetApp()->Run();

InitFailure:
	AfxWinTerm();
	return nReturnCode;
}
#else
// _WINDLL initialization

#ifndef _NTWIN
extern "C"
int FAR PASCAL LibMain(HINSTANCE hInstance,
	WORD wDataSegment, WORD wHeapSize, LPSTR lpszCmdLine)
{
	// Initialize DLL's instance(/module) not the app's
	if (!AfxWinInit(hInstance, NULL, lpszCmdLine, 0))
	{
		AfxWinTerm();
		return 0;       // Init Failed
	}

	// initialize the single instance DLL
	if (!AfxGetApp()->InitInstance())
	{
		AfxWinTerm();
		return 0;
	}

	// nothing to run
	return 1;   // ok
}
#else //_NTWIN
#include <process.h>	// for _cexit()

// NOTE: To provide your own DLL entry point use a different name than
//	AfxLibMain, but make sure to call AfxLibMain much like _CRT_INIT
//	is called from AfxLibMain itself.

extern "C" BOOL WINAPI
_CRT_INIT(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved);

extern "C" int APIENTRY
AfxLibMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH || dwReason == DLL_THREAD_ATTACH)
	{
		// Initialize the C/C++ runtime first
		if (!_CRT_INIT(hInstance, dwReason, lpReserved))
			return 0;		// CRT init Failed
	}

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		// Initialize DLL's instance(/module) not the app's
		if (!AfxWinInit(hInstance, NULL, "", 0))
		{
			AfxWinTerm();
			return 0;		// Init Failed
		}

		// initialize the single instance DLL
		if (!AfxGetApp()->InitInstance())
		{
			AfxWinTerm();
			return 0;
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		// Terminate the library before destructors are called
		AfxWinTerm();

		// This should call C++ static destructors
		_cexit();
	}

	if (dwReason == DLL_PROCESS_DETACH || dwReason == DLL_THREAD_DETACH)
	{
		// Terminate the C/C++ runtime last
		if (!_CRT_INIT(hInstance, dwReason, lpReserved))
			return 0;		// CRT term	Failed
	}

	return 1;   // ok
}
#endif //_NTWIN
#endif //_WINDLL

/////////////////////////////////////////////////////////////////////////////
