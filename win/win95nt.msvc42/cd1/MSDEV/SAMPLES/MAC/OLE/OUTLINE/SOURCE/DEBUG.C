/*****************************************************************************\
*                                                                             *
*    Debug.c                                                                  *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "Util.h"
#if qDebugWindow
	#include "DebugWindow.h"
#endif
#include "stdio.h"

#if qAssert

Boolean gEnableAsserts = false;

void __ASSERTCOND(char *expr, char *file, int line)
{
	char	s[255];

	if (!gEnableAsserts)
		return;

	sprintf(s, "%s(%ld): %s", file, line, expr);

#if qDebugWindow
	{
		ProcessSerialNumber		psn;
		ProcessInfoRec			info;
		Str32					str;
		
		info.processName = str;
		info.processAppSpec = nil;
		
		GetCurrentProcess(&psn);
		GetProcessInformation(&psn, &info);
		p2cstr(info.processName);

		DebugWindow("%s assert: %s(%ld): %s\n", info.processName, file, line, expr);
	}
#endif

	DebugStr((StringPtr)c2pstr(s));
}

void __ASSERTNOERROR(HRESULT hrErr, char *file, int line)
{
	SCODE	sc;
	char	s[255];

	if (!gEnableAsserts)
		return;

	sc = GetScode(hrErr);
	sprintf(s, "%s(%ld): %s (%lx)", file, line, szFromScode(sc), sc);

#if qDebugWindow
	{
		ProcessSerialNumber		psn;
		ProcessInfoRec			info;
		Str32					str;
		
		info.processName = str;
		info.processAppSpec = nil;
		
		GetCurrentProcess(&psn);
		GetProcessInformation(&psn, &info);
		p2cstr(info.processName);

		DebugWindow("%s assert: %s(%ld): %s (%lx)\n", info.processName, file, line,szFromScode(sc), sc);
	}
#endif

	DebugStr((StringPtr)c2pstr(s));
}

#endif // qAssert

#if qMemCheck
static void cdecl erf_DebuggerMessage(char *mc_msgtext);
#endif

#if qDebug
void InitDebugging()
{
#if qMemCheck
		mc_startcheck(erf_DebuggerMessage);
#endif
}

void FinishDebugging()
{
#if qMemCheck
		mc_endcheck();
#endif
}
#endif // qDebug

#if qMemCheck

static Boolean gInitedMemCheck = false;

static void cdecl erf_DebuggerMessage(char *mc_msgtext)
{
	if (!gInitedMemCheck)
	{
		gInitedMemCheck = true;
		return;
	}
	
#if qDebugWindow
	{
		ProcessSerialNumber		psn;
		ProcessInfoRec			info;
		Str32					str;
		
		info.processName = str;
		info.processAppSpec = nil;
		
		GetCurrentProcess(&psn);
		GetProcessInformation(&psn, &info);
		p2cstr(info.processName);

		DebugWindow("%s MemCheck: %s\n", info.processName, mc_msgtext);
	}
#else
	if (mc_msgtext == nil)
		Debugger();
	else
	{
		DebugStr(c2pstr(mc_msgtext));
		p2cstr((StringPtr)mc_msgtext);
	}
#endif
}

#endif

