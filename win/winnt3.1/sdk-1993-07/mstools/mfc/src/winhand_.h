// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

/////////////////////////////////////////////////////////////////////////////
// INTERNAL INTERNAL INTERNAL 
// must be included after afxwin.h
// DOS I/O functions

#ifndef __HANDLE_H__
#define __HANDLE_H__

#ifndef __AFXCOLL_H__
#include "afxcoll.h"
#endif


#undef THIS_FILE
#define THIS_FILE "winhand_.h"

/* Included in this file:
 *  Most Windows objects are represented with a HANDLE, including
 *      the most important ones, HWND, HDC, HPEN, HFONT etc.
 *  We want C++ objects to wrap these handle based objects whenever we can.
 *  Since Windows objects can be created outside of C++ (eg: calling
 *      ::CreateWindow will return an HWND with no C++ wrapper) we must
 *      support a reasonably uniform mapping from permanent handles
 *      (i.e. the ones allocated in C++) and temporary handles (i.e.
 *      the ones allocated in C, but passed through a C++ interface.
 *  We keep two dictionaries for this purpose.  The permanent dictionary
 *      stores those C++ objects that have been explicitly created by
 *      the developer.  The C++ constructor for the wrapper class will
 *      insert the mapping into the permanent dictionary and the C++
 *      destructor will remove it and possibly free up the associated
 *      Windows object (true for everything except CDevice's).
 *  When a handle passes through a C++ interface that doesn't exist in
 *      the permanent dictionary, we allocate a temporary wrapping object
 *      and store that mapping into the temporary dictionary.
 *  At idle time the temporary wrapping objects are flushed (since you better
 *      not be holding onto something you didn't create).
 */

// forward class declarations
class NEAR CHandleMap
{
public:
#ifndef _NTWIN
	CMapWordToPtr permanentMap;
	CMapWordToPtr temporaryMap;
#else
	CMapPtrToPtr permanentMap;
	CMapPtrToPtr temporaryMap;
#endif

// Callbacks
	virtual CObject* NewTempObject(HANDLE h) = 0;
	virtual void DeleteTempObject(CObject *) = 0;

// Operations
	CObject* FromHandle(HANDLE h);
	void     DeleteTemp();

	BOOL    LookupPermanent(HANDLE h, void*& pv)
#ifdef _NTWIN
				{ return permanentMap.Lookup((LPVOID)h, pv); }
#else
				{ return permanentMap.Lookup((WORD)h, pv); }
#endif
	BOOL    LookupTemporary(HANDLE h, void*& pv)
#ifdef _NTWIN
				{ return temporaryMap.Lookup((LPVOID)h, pv); }
#else
				{ return temporaryMap.Lookup((WORD)h, pv); }
#endif

	CObject* GetPermanent(HANDLE h)
				{
					void* ob;
					VERIFY(LookupPermanent(h, ob));
					return (CObject*) ob;
				}

	void    SetPermanent(HANDLE h, CObject* permOb)
				{
#ifdef _DEBUG
					void* pv;
					ASSERT(!LookupPermanent(h, pv)); // must not be in there
					BOOL bEnable = AfxEnableMemoryTracking(FALSE);
#endif
#ifdef _NTWIN
					permanentMap[(LPVOID)h] = permOb;
#else
					permanentMap[(WORD)h] = permOb;
#endif
#ifdef _DEBUG
					AfxEnableMemoryTracking(bEnable);
#endif
				}

	void    RemovePermanent(HANDLE h)       // remove, don't assert if not there
#ifdef _NTWIN
				{ permanentMap.RemoveKey((LPVOID)h); }
#else
				{ permanentMap.RemoveKey((WORD)h); }
#endif
	void    RemoveTemporary(HANDLE h)       // remove, don't assert if not there
#ifdef _NTWIN
				{ temporaryMap.RemoveKey((LPVOID)h); }
#else
				{ temporaryMap.RemoveKey((WORD)h); }
#endif
};

#undef THIS_FILE
#define THIS_FILE __FILE__

#endif //__HANDLE_H__
