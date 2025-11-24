// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

// Pointer helper inlines (to avoid compiler warnings)

static inline void* GetPtrFromFarPtr(void FAR* lp, size_t offset)
{
#ifdef _NEARDATA
	// 16 bit data pointers
	ASSERT(_AFX_FP_SEG(lp) == _segname("_DATA"));
	return ((BYTE *)_AFX_FP_OFF(lp)) - offset;
#else
	// 32 bit data pointers
	return ((BYTE *)lp) - offset;
#endif
}

static inline void* GetPtrFromDWord(DWORD dw)
{
#ifdef _NEARDATA
	// 16 bit data pointers
	ASSERT(HIWORD(dw) == 0);
	return (void*)LOWORD(dw);
#else
	// 32 bit data pointers
	return (void*)dw;
#endif
}

static inline DWORD GetDWordFromPtr(void* p)
{
#ifdef _NEARDATA
	// 16 bit data pointers
	return MAKELONG((WORD)p, 0);
#else
	// 32 bit data pointers
	return (DWORD)p;
#endif
}

