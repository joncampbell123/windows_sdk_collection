// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// afxv_cpu.h - target version/configuration control for non-Intel CPUs

#if !defined(_M_ALPHA) && !defined(_M_IA64)
	#error afxv_cpu.h is only for DEC AXP and Merced builds
#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef _ALPHA_
// specific overrides for ALPHA...
#define _AFX_PACKING    8       // default AXP alignment (required)
#ifdef _AFX_NO_DEBUG_CRT
extern "C" void _BPT();
#pragma intrinsic(_BPT)
#define AfxDebugBreak() _BPT()
#else
#define AfxDebugBreak() _CrtDbgBreak()
#endif
#endif  //_ALPHA_

/////////////////////////////////////////////////////////////////////////////

#ifdef _IA64_
// specific overrides for IA64...
#define _AFX_PACKING    8
#define _SHADOW_DOUBLES
#endif //_IA64_
