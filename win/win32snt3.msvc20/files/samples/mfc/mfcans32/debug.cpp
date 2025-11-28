//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Debug.cpp
//
//  Contents:   Debug code.
//
//  Classes:    None.
//
//  Functions:  DebugAssert
//
//  History:    05-Apr-94   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"


extern "C" int __cdecl _purecall()
{
	AssertSz(FALSE, "'__purecall called'");
	return 0;
}

#ifdef _DEBUG

void DebugAssert(PCSTR pText, PCSTR pFile, UINT uLineNo)
{
	char buf[256];


	if (pText)
		wsprintf(buf, "Assert %s in file %s - %ud", pText, pFile, uLineNo);
	else
		wsprintf(buf, "Assert in file %s - %u", pFile, uLineNo);

	MessageBox(NULL, buf, "Ole2Ansi", MB_SETFOREGROUND | MB_OK);

	DebugBreak();
}


#endif //!_DEBUG

#ifdef _DEBUG_HOOKS
#pragma code_seg(".olebrk")

void __cdecl _DebugHook(LPUNKNOWN, void (STDMETHODCALLTYPE IUnknown::*)())
{
}

#endif //!_DEBUG_HOOKS
