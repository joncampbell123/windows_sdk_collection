/*****************************************************************************\
*                                                                             *
*    OleExceptions.c                                                          *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __OLEEXCEPTIONS_H__
#define __OLEEXCEPTIONS_H__

#include "except.h"
#include "ole2.h"
#include "types.h"

#define kOleErrorMessage		1024
#define kOleRegMessage			1025

Boolean IsOleFailure(void);
HRESULT GetOleFailure(void);

void FailOleErr(HRESULT error);
void FaileOleRegErr(OLEREGSTATUS error);

void _FailMemError();

#if qOle
#ifdef UNNECESSARY

#ifndef __powerc
#define FailNIL			_FailNIL
#define FailOSErr		_FailOSErr
#define FailMemError	_FailMemError
#define FailResError	_FailResError
#define Failure			_Failure

void _FailNIL(void *);
void _FailOSErr(short error);
void _FailMemError(void);
void _FailResError(void);
void _Failure(short error, long message);
#endif
#endif
#endif

#endif
