//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       System.cpp
//
//  Contents:   System Interfaces.
//
//  Classes:    CWrapperAlloc
//
//  History:    19-Jan-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   CInterface Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CInterface::CInterface, public
//
//  Synopsis:   Constructor.
//
//  Arguments:  [idInterface] --
//              [pUnk]
//              [pObj]
//
//  Returns:    OLE2 result code.
//
//---------------------------------------------------------------------------
CInterface::CInterface(IDINTERFACE idInterface, LPUNKNOWN pUnk, LPUNKNOWN pObj)
{
	m_idInterface = idInterface;
	m_refs        = 1;
	m_pUnk        = pUnk;
	m_pObj        = pObj;
	m_pInterfaceData = NULL;
}


//+--------------------------------------------------------------------------
//
//  Member:     CInterface::QueryInterface, public
//
//  Synopsis:   IUnknown::QueryInterface method implementation.
//
//  Arguments:  [riid] -- Interface ID.
//              [ppv]  -- Pointer to query results.
//
//  Returns:    OLE2 result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CInterface::QueryInterface(REFIID riid, void * * ppv)
{
	TraceNotify("CInterface::QueryInterface");

	Assert(this   != NULL);
	Assert(m_pObj != NULL);
	Assert(m_pUnk != NULL);

	return m_pUnk->QueryInterface(riid, ppv);
}


//+--------------------------------------------------------------------------
//
//  Member:     CInterface::AddRef, public
//
//  Synopsis:   IUnknown::AddRef method implementation.
//
//  Returns:    Current reference count.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned long) CInterface::AddRef()
{
	TraceNotify("CInterface::AddRef");

	Assert(this   != NULL);
	Assert(m_pObj != NULL);
	Assert(m_pUnk != NULL);

	m_pUnk->AddRef();

	return InterlockedIncrement(&m_refs);
}


//+--------------------------------------------------------------------------
//
//  Member:     CInterface::Release, public
//
//  Synopsis:   IUnknown::Release method implementation.
//
//  Returns:    Current reference count.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned long) CInterface::Release()
{
	TraceMethodEnter("CInterface::Release", this);

	LONG lRet;


	Assert(this   != NULL);
	Assert(m_pObj != NULL);
	Assert(m_pUnk != NULL);

	lRet = InterlockedDecrement(&m_refs);

	if (lRet == 0)
	{
		m_pObj->Release();
		((CUnknownA *)m_pUnk)->m_pInterfaces[GetInterfaceId()] = NULL;
		m_pUnk->Release();

		//
		//  The thunked routine CreateDispTypeInfoA creates an UNICODE image
		//  of the InterfaceData structure which must be kept around until
		//  the TypeInfo is released.   So if we got it, time to free it.
		//
		if (m_pInterfaceData)
			ConvertInterfaceDataFree(m_pInterfaceData);

		WrapDeleteWrapper(this);
		TraceNotify("CInterface::Release - Release Interface");
		return 0;
	}

	m_pUnk->Release();

	return lRet;
}


//***************************************************************************
//
//                   CWrapperAlloc Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Method:     CWrapperAlloc::new
//
//  Synopsis:   Memory Allocator.
//
//  Returns:    Pointer to newly allocated memory.
//
//---------------------------------------------------------------------------
void * CWrapperAlloc::operator new (unsigned int size)
{
	return ::operator new(size);
}


//+--------------------------------------------------------------------------
//
//  Method:     CWrapperAlloc::delete
//
//  Synopsis:
//
//  Returns:
//
//---------------------------------------------------------------------------
void CWrapperAlloc::operator delete(void * ptr)
{
	::operator delete(ptr);
}
