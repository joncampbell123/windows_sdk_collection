// Microsoft Foundation Classes C++ library.
// Copyright (C) 1992,93 Microsoft Corporation,
// All rights reserved.

// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// Target version/configuration control for supported platforms
    

/////////////////////////////////////////////////////////////////////////////
//	Intel x86

	// None needed

	
/////////////////////////////////////////////////////////////////////////////
//	MIPS R4000

#ifdef _M_MRX000

#ifndef ALL_WARNINGS
#pragma warning(disable: 4616)  // disabled warning number out of range
#endif //ALL_WARNINGS

#define CALL_DLL_CRT_INIT   	// DllMain requires explicit call to _CRT_INIT

#endif


/////////////////////////////////////////////////////////////////////////////
//	DEC Alpha AXP

#ifdef _ALPHA_

#define CALL_DLL_CRT_INIT   	// DllMain requires explicit call to _CRT_INIT

#endif
