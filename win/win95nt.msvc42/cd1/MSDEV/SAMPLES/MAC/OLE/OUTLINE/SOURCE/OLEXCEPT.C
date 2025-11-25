/*****************************************************************************\
*                                                                             *
*    OleExceptions.c                                                          *
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
#include "OleXcept.h"

static HRESULT			gLastHResult;
static OLEREGSTATUS		gLastRegStatus;

#ifdef _MSC_VER
static long				gLastMessage;
#endif

/******************************************************************************
 FailOleErr

 	Calls Failure if err != noErr. If gAskFailure is TRUE, breaks into debugger
 	before testing err. Failure may be simulated by setting err to the desired
 	error code in the debugger.
******************************************************************************/
void FailOleErr(HRESULT hrErr)
{
	SCODE	code;

#ifdef DEBUG
	if (gAskFailure) Debugger();
#endif

	if (hrErr != NOERROR)
	{
		code = GetScode(hrErr);
		gLastHResult = hrErr;
		
		if (FAILED(code))
			Failure((short)SCODE_CODE(code), kOleErrorMessage);
	}
}

void FaileOleRegErr(OLEREGSTATUS err)
{
#ifdef DEBUG
	if (gAskFailure) Debugger();
#endif

	if (err != OLEREG_OK)
	{
		gLastRegStatus = err;

		Failure((short)err, kOleRegMessage);
	}
}

Boolean IsOleFailure(void)
{
	return gLastMessage == kOleErrorMessage;
}

HRESULT GetOleFailure(void)
{
	ASSERTCOND(gLastMessage == kOleErrorMessage);
	
	return gLastHResult;
}

#ifdef UNNECESSARY

#ifndef _MSC_VER
#undef FailNIL
#undef FailOSErr
#undef FailMemError
#undef FailResError
#undef Failure
#endif

void _FailNIL(void *p)
{
	gLastMessage = 0;

	FailNIL(p);
}

void _FailOSErr(short error)
{
	gLastMessage = 0;

	FailOSErr(error);
}


void _FailMemError(void)
 {
 	gLastMessage = 0;

 	FailMemError();
 }

void _FailResError(void)
{
	gLastMessage = 0;

	FailResError();
}

void _Failure(short error, long message)
{
	gLastMessage = message;

	Failure(error, message);
}

#endif

