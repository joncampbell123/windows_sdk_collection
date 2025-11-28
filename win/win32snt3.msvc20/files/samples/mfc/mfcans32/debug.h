//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Debug.h
//
//  Contents:
//
//  Classes:    None.
//
//  Functions:  DebugAssert
//
//  Comment:    It is assumed that the compiler's string pooling option is
//              turned on.
//
//  History:    05-Apr-94   v-kentc     Created.
//
//---------------------------------------------------------------------------

#ifdef _DEBUG
extern void DebugAssert(PCSTR, PCSTR, UINT);

#define Assert(a) { if (!(a)) DebugAssert(NULL, __FILE__, __LINE__); }
#define AssertSz(a,t) { if (!(a)) DebugAssert(t, __FILE__, __LINE__); }
#else
#define Assert(a)
#define AssertSz(a,t)
#endif

//
// Special hooks for VC++ 2.0 LRPC debugging.  Defined even in
//      release version of the DLL usually.
//

#define MEMBER_PTR(class, member) \
		((void (STDMETHODCALLTYPE IUnknown::*)()) \
		 (void (STDMETHODCALLTYPE class::*)())(&class::member)) \

#ifdef _DEBUG_HOOKS
extern void __cdecl _DebugHook(
		LPUNKNOWN lpUnk, void (STDMETHODCALLTYPE IUnknown::*pfnMember)());
#else
#define _DebugHook(lpUnk, pfnMember)
#endif
