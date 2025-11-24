// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (C) 1992 Microsoft Corporation 
// All rights reserved. 
//  
// This source code is only intended as a supplement to the 
// Microsoft Foundation Classes Reference and Microsoft 
// QuickHelp documentation provided with the library. 
// See these sources for detailed information regarding the 
// Microsoft Foundation Classes product. 

#ifdef _WINDOWS
#ifndef _WINDLL
#include "afxole.h"
#else
#include "afxwin.h"
#endif //_WINDLL
#else
#include "afx.h"
#endif

#include "afxcoll.h"

#pragma hdrstop

#include <new.h>        // for set_new_handler

#ifdef AFX_CORE_SEG
#pragma code_seg(AFX_CORE_SEG)
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Runtime Typing

// special runtime-class structure for CObject (no base class)
struct CRuntimeClass NEAR CObject::classCObject =
	{ "CObject", sizeof(CObject), 0xffff, NULL, NULL };
static CClassInit _init_CObject(&CObject::classCObject);

CRuntimeClass* CObject::GetRuntimeClass() const
{
	return &CObject::classCObject;
}

BOOL CObject::IsKindOf(const CRuntimeClass* pClass) const
{
	ASSERT(this != NULL);
	// it better be in valid memory, at least for CObject size
	ASSERT(AfxIsValidAddress(this, sizeof(CObject)));

	// simple SI case
	register CRuntimeClass* pClassThis = GetRuntimeClass();
	ASSERT(pClass != NULL);
	ASSERT(pClassThis != NULL);
	while (pClassThis != NULL)
	{
		if (pClassThis == pClass)
			return TRUE;
		pClassThis = pClassThis->m_pBaseClass;
	}
	return FALSE;       // walked to the top, no match
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostic Support

#ifdef _DEBUG
extern "C" void PASCAL AfxAssertValidObject(const CObject* pOb)
{
	if (pOb == NULL)
	{
		TRACE("ASSERT_VALID fails with NULL pointer\n");
		ASSERT(FALSE);
		return;     // quick escape
	}
	ASSERT(::AfxIsValidAddress(pOb, sizeof(CObject)));
	pOb->AssertValid();
	ASSERT(::AfxIsValidAddress(pOb, pOb->GetRuntimeClass()->m_nObjectSize));
}
#endif


void CObject::AssertValid() const
{
	ASSERT(this != NULL);
}

void
CObject::Dump(CDumpContext& dc) const
{
#ifdef _DEBUG
	dc << "a " << GetRuntimeClass()->m_pszClassName << " at " 
		<< (void*) this << " ";
#else
	dc;
#endif //_DEBUG
}

////////////////////////////////////////////////////////////////////////////
// Allocation/Creation

CObject* CRuntimeClass::CreateObject()
{
	void* p = CObject::operator new(m_nObjectSize);
	if (!ConstructObject(p))
	{
		CObject::operator delete(p);
		p = NULL;
	}
	return (CObject*) p;
}

BOOL CRuntimeClass::ConstructObject(void* pThis)
/*
  -- dynamically construct an instance of this class in the memory
		pointed to by 'pThis'
  -- return FALSE if can't construct (only possible cause is an abstract class)
*/
{
	ASSERT(AfxIsValidAddress(pThis, m_nObjectSize));

	if (m_pfnConstruct != NULL)
	{
		(*m_pfnConstruct)(pThis);
		return TRUE;
	}
	else
	{
		TRACE("Error: Trying to construct an instance of an abstract class.\n");
		return FALSE;
	}
}


////////////////////////////////////////////////////////////////////////////
// Class loader & class serialization

BOOL 
CObject::IsSerializable() const
{ 
	return (GetRuntimeClass()->m_wSchema != 0xffff);
}

CRuntimeClass* CRuntimeClass::pFirstClass = NULL;

CClassInit::CClassInit(register CRuntimeClass* pNewClass)
{
	ASSERT(pNewClass->m_pNextClass == NULL);
	pNewClass->m_pNextClass = CRuntimeClass::pFirstClass;
	CRuntimeClass::pFirstClass = pNewClass;
}

#ifdef _DEBUG
void PASCAL 
AfxDoForAllClasses(void (*pfn)(const CRuntimeClass*, void*),
	void* pContext)
{
	CRuntimeClass* pClass;

	for (pClass = CRuntimeClass::pFirstClass; pClass != NULL;
		pClass = pClass->m_pNextClass)
	{
		(*pfn)(pClass, pContext);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Non-diagnostic memory routines
//

#ifndef _DEBUG
	// Debugging version replaces global ::new so this is not needed

int cdecl AfxNewHandler(size_t /* nSize */)
{
	//  AFX memory allocation will never return "NULL" it will always throw
	//      a memory exception instead
	AfxThrowMemoryException();
	return 0;
}
#endif //!_DEBUG


// hook in our own new_handler
static BOOL AfxInitialize()
{
	(void)_afx_version();
#ifdef _DEBUG

	// Force reference of the following symbols for CodeView
#ifdef _WINDOWS
	(void)afxTraceFlags;
#endif

	(void)afxTraceEnabled;
	(void)afxMemDF;

	return AfxDiagnosticInit();
#else
	_set_new_handler(AfxNewHandler);
	return TRUE;
#endif // _DEBUG
}

static BOOL bInitialized = AfxInitialize();
		// a way to force initialization

/////////////////////////////////////////////////////////////////////////////
