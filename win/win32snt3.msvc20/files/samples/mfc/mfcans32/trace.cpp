//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansistor.h
//
//  Contents:   ANSI Wrappers for Unicode Storage Interfaces and APIs.
//
//  Classes:    None.
//
//  Functions:  TraceInit
//              TraceSTDAPIEntry
//              TraceSTDAPIExit
//              TraceMethodEntry
//              TraceMethodExit
//
//  History:    01-Mar-94   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



#ifdef _DEBUG

//
//  Define trace options.
//
#define TRACE_STDAPI    0x0001
#define TRACE_METHOD    0x0002
#define TRACE_REFERENCE 0x0004
#define TRACE_WRAPPER   0x0008
#define TRACE_NOTIFY    0x0010


//
//  Contains trace bits retrieve from the registry.
//
DWORD g_Trace = 0x0000;


//+--------------------------------------------------------------------------
//
//  Routine:    TraceInit
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
void TraceInit(void)
{
	HKEY  hKey;
	DWORD Type;
	ULONG Length;


	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Ole2Ansi", 0,
			KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		Length = sizeof(g_Trace);
		RegQueryValueEx(hKey, "Trace", NULL, &Type, (PBYTE)&g_Trace, &Length);

		RegCloseKey(hKey);
	}
}


void TraceSTDAPIEnter(PSTR pText)
{
	if (g_Trace & TRACE_STDAPI)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "STDAPI In  %s", pText);
		TraceOutput(buf);
	}
}


void TraceSTDAPIExit(PSTR pText, HRESULT hResult)
{
	if (g_Trace & TRACE_STDAPI)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "STDAPI Out %s, result %x", pText, hResult);
		TraceOutput(buf);
	}
}


void TraceMethodEnter(PSTR pText, PVOID pThis)
{
	if (g_Trace & TRACE_METHOD)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "Method In  %s, this %x", pText, pThis);
		TraceOutput(buf);
	}
}


void TraceMethodExit(PSTR pText, PVOID pThis, HRESULT hResult)
{
	if (g_Trace & TRACE_METHOD)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "Method Out %s, this %x, result %x", pText, pThis, hResult);
		TraceOutput(buf);
	}
}


void TraceAddRef(PSTR pText, PVOID pThis, LONG lRef)
{
	if (g_Trace & TRACE_REFERENCE)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "AddRef  %s, this %x, ref %d", pText, pThis, lRef);
		TraceOutput(buf);
	}
}


void TraceRelease(PSTR pText, PVOID pThis, LONG lRef)
{
	if (g_Trace & TRACE_REFERENCE)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "Release %s, this %x, ref %d", pText, pThis, lRef);
		TraceOutput(buf);
	}
}


void TraceNotify(PSTR pText)
{
	if (g_Trace & TRACE_NOTIFY)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "%s", pText);
		TraceOutput(buf);
	}
}


void TraceWrapper(PSTR pText)
{
	if (g_Trace & TRACE_WRAPPER)
	{
		char buf[MAX_STRING];

		wsprintf(buf, "%s", pText);
		TraceOutput(buf);
	}
}


void TraceWarning(PSTR pText)
{
	char buf[MAX_STRING];

	wsprintf(buf, "*** WARNING ***  %s", pText);
	TraceOutput(buf);
}


void TraceFatalError(PSTR pText)
{
	char buf[MAX_STRING];

	wsprintf(buf, "*** FATAL ERROR ***  %s", pText);
	TraceOutput(buf);
	DebugBreak();
}


void TraceOutput(PSTR pText)
{
	char buf[MAX_STRING];

	wsprintf(buf, "Ole2Ansi: [%d] %s\r\n", GetCurrentThreadId(), pText);
	OutputDebugString(buf);
}


#endif
