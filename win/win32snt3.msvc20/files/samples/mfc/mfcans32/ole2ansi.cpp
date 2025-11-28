//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       Ole2Ansi.cpp
//
//  Contents:   ANSI Wrappers for Unicode 32bit OLE2.
//
//  Functions:  DllEntry
//              operator new
//              operator delete
//
//  History:    01-Jan-94   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"


#ifdef MAKEDLL

#pragma code_seg(".text$initseg")

//+--------------------------------------------------------------------------
//
//  Routine:    DllEntry
//
//  Synopsis:
//
//  Returns:    True if successful, else False.
//
//---------------------------------------------------------------------------
extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	unreference(hinstDLL);
	unreference(lpReserved);


	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstDLL);
		TraceInit();
		if (!WrapInit())
			return FALSE;
		break;

	case DLL_PROCESS_DETACH:
		WrapCleanup();
		break;
	}

	return TRUE;
}

#pragma code_seg(".orpc")


//+--------------------------------------------------------------------------
//
//  Routine:    operator new
//
//  Synopsis:
//
//  Returns:    Address of allocated memory block, else NULL on error.
//
//---------------------------------------------------------------------------
void * __cdecl operator new(unsigned int Size)
{
	IMalloc * pIMalloc;
	void * ptr;


	CoGetMalloc(MEMCTX_TASK, &pIMalloc);

	ptr = pIMalloc->Alloc(Size);

#ifdef _DEBUG
	//  Make finding problems easier.
	memset(ptr, 0xCE, Size);
#endif

	pIMalloc->Release();

	return ptr;
}


//+--------------------------------------------------------------------------
//
//  Routine:    operator delete
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
void __cdecl operator delete(void * ptr)
{
	if (ptr)
	{
		IMalloc * pIMalloc;

		CoGetMalloc(MEMCTX_TASK, &pIMalloc);

#ifdef _DEBUG
		//  Make finding problems easier.
		memset(ptr, 0xDE, pIMalloc->GetSize(ptr));
#endif

		pIMalloc->Free(ptr);

		pIMalloc->Release();
	}
}

#endif
