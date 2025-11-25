/*****************************************************************************\
*                                                                             *
*    Debug.h                                                                  *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <ole2.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef ASSERTCOND

#if qAssert
	extern Boolean gEnableAsserts;
	extern void __ASSERTCOND(char *, char *, int);
	extern void __ASSERTNOERROR(HRESULT, char *, int);
	#define ASSERTCOND(x)			( (x) ? (void) 0 : (__ASSERTCOND(#x, __FILE__, __LINE__)))
	#define ASSERTCONDSZ(x, y)		( (x) ? (void) 0 : (__ASSERTCOND(#y, __FILE__, __LINE__)))
	#define ASSERTNOERROR(hr)		( (hr == 0) ? (void) 0 : (__ASSERTNOERROR(hr, __FILE__, __LINE__)))
#else
	#define ASSERTCOND(x)			((void)0)
	#define ASSERTCONDSZ(x, y)		((void)0)
	#define ASSERTNOERROR(hr)		((void)0)
#endif

#ifdef __cplusplus
}
#endif

#if qDebug
	void InitDebugging(void);
	void FinishDebugging(void);

#if qMemCheck
		#include <MemCheck.h>
#endif
#else
	#define	InitDebugging()		((void)0)
	#define FinishDebugging()	((void)0)
#endif

#endif
