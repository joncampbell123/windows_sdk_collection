//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       widecomp.cpp
//
//  Contents:   Unicode Wrappers for ANSI CompObj Interfaces.
//
//  Classes:    CEnumStringW - Unicode wrapper object for IEnumStringA.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IUnknownW Implementation
//
//***************************************************************************


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownW::CUnknownW, public
//
//  Synopsis:   Constructor.
//
//  Arguments:  [pANSI] -- ANSI CUnknown object.
//
//  Returns:    OLE2 result code.
//
//---------------------------------------------------------------------------
CUnknownW::CUnknownW(IUnknown * pANSI, IDINTERFACE idRef) :
		CInterface(ID_IUnknown, NULL, (LPUNKNOWN)pANSI)
{
	m_idPrimary    = idRef;

	memset(m_pInterfaces, 0, sizeof(m_pInterfaces));
	TraceAddRef("CUnknownW::CUnknownW", pANSI, pANSI->AddRef());
}


void CUnknownW::AddInterface(IDINTERFACE idInterface, LPUNKNOWN pUnk)
{
	m_pInterfaces[idInterface] = pUnk;
}


long CUnknownW::FreeInterface(IDINTERFACE idInterface)
{
	WrapDeleteWrapper(m_pInterfaces[idInterface]);
	m_pInterfaces[idInterface] = NULL;
	TraceRelease("CUnknownW::FreeInterface", this, Release());
	return 0;
}


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownW::QueryInterface, public
//
//  Synopsis:   IUnknown::QueryInterface method implementation.
//
//  Arguments:  [riid] -- Interface ID.
//              [ppv]  -- Pointer to query results.
//
//  Returns:    OLE2 result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CUnknownW::QueryInterface(REFIID riid, void * * ppv)
{
	TraceMethodEnter("CUnknownW::QueryInterface", this);

	IDINTERFACE      idRef;
	LPUNKNOWN pUnk;
	HRESULT   hResult;


	*ppv = NULL;

	if (riid == IID_IUnknown)
	{
		*ppv = this;
		TraceAddRef("CUnknownA::QueryInterface", this, AddRef());
		return ResultFromScode(S_OK);
	}

	idRef = WrapTranslateIID(riid);

	//
	//  If the id is NULL then this is not a thunked interface.
	//
	if (idRef == ID_NULL)
	{
		hResult = m_pObj->QueryInterface(riid, ppv);

		if (SUCCEEDED(hResult))
			TraceWarning("Interface not handled");

		return hResult;
	}

	//
	//  If the requested interface has already been wrapped then use the
	//  original wrapped object.
	//
	if (m_pInterfaces[idRef])
	{
		*ppv = m_pInterfaces[idRef];
		TraceAddRef("CUnknownA::QueryInterface", m_pInterfaces[idRef], m_pInterfaces[idRef]->AddRef());
		return ResultFromScode(S_OK);
	}

	//
	//  Query the Ansi interface and then wrap it.
	//
	hResult = m_pObj->QueryInterface(riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		return hResult;

	*ppv = m_pInterfaces[idRef] = WrapAnyWFromA(this, idRef, pUnk);
	if (m_pInterfaces[idRef] == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	TraceAddRef("CUnknownA::QueryInterface", this, AddRef());

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownW::AddRef, public
//
//  Synopsis:   IUnknown::AddRef method implementation.
//
//  Returns:    Current reference count.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned long) CUnknownW::AddRef()
{
	TraceMethodEnter("CUnknownW::AddRef", this);

	LONG lRet;


	lRet = InterlockedIncrement(&m_refs);

	TraceAddRef("CUnknownW::AddRef", this, m_refs);

	m_pObj->AddRef();

	return lRet;
}


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownW::Release, public
//
//  Synopsis:   IUnknown::Release method implementation.
//
//  Returns:    Current reference count.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned long) CUnknownW::Release()
{
	TraceMethodEnter("CUnknownW::Release", this);

	LONG lRet;


	lRet = InterlockedDecrement(&m_refs);

	TraceRelease("CUnknownW::Release", this, m_refs);

	if (m_pInterfaces[m_idPrimary] != NULL)
	{
		if (m_refs < ((CUnknownW *)m_pInterfaces[m_idPrimary])->m_refs)
		{
			((CUnknownW *)m_pInterfaces[m_idPrimary])->m_refs--;
			if (((CUnknownW *)m_pInterfaces[m_idPrimary])->m_refs == 0)
			{
				((CUnknownW *)m_pInterfaces[m_idPrimary])->m_pObj->Release();
				WrapDeleteWrapper(m_pInterfaces[m_idPrimary]);
				m_pInterfaces[m_idPrimary] = NULL;
			}

			TraceWarning("Had to adjust primary interface.");
		}
	}

	if (lRet == 0)
	{
		for (UINT i = 0; i < ID_SIZE; i++)
		{
			if (m_pInterfaces[i])
			{
				TraceWarning("Interfaces not properly released.");
				if (i != m_idPrimary)
					((CUnknownW *)m_pInterfaces[i])->m_pObj->Release();
				WrapDeleteWrapper(m_pInterfaces[i]);
			}
		}

		++m_refs;
		TraceRelease("CUnknownW::Release", m_pObj, m_pObj->Release());
		--m_refs;

		//
		//  The thunked routine CreateDispTypeInfoA creates an UNICODE image
		//  of the InterfaceData structure which must be kept around until
		//  the TypeInfo is released.   So if we got it, time to free it.
		//
		if (m_pInterfaceData)
			ConvertInterfaceDataFree(m_pInterfaceData);

		WrapDeleteWrapper(this);

		return 0;
	}

	m_pObj->Release();

	return lRet;
}


//***************************************************************************
//
//                   IEnumStringW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringW::Next, public
//
//  Synopsis:   Thunks Next to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringW::Next(
		ULONG celt,
		LPOLESTR * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumStringW::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumStringA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetANSI()->Next(celt, (LPSTR *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertStringArrayToW(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringW::Skip, public
//
//  Synopsis:   Thunks Skip to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringW::Skip(ULONG celt)
{
	TraceMethodEnter("CEnumStringW::Skip", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumStringA, Skip));

	return GetANSI()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringW::Reset, public
//
//  Synopsis:   Thunks Reset to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringW::Reset(VOID)
{
	TraceMethodEnter("CEnumStringW::Reset", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumStringA, Reset));

	return GetANSI()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringW::Clone, public
//
//  Synopsis:   Thunks Clone to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringW::Clone(IEnumString * * ppenm)
{
	TraceMethodEnter("CEnumStringW::Clone", this);

	IEnumStringA * penmA;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumStringA, Clone));

	*ppenm = NULL;

	HRESULT hResult = GetANSI()->Clone(&penmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumStringWFromA(penmA, ppenm);

	if (penmA)
		penmA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IClassFactoryW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CClassFactoryW::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CClassFactoryW::CreateInstance(LPUNKNOWN pUnkOuter,
		  REFIID riid, LPVOID FAR* ppvObject)
{
	TraceMethodEnter("CClassFactoryW::CreateInstance", this);

	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IClassFactory, CreateInstance));

	hResult = GetANSI()->CreateInstance(pUnkOuter, riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		return hResult;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceWFromA(idRef, pUnk, (LPUNKNOWN *)ppvObject);

	pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CClassFactoryW::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CClassFactoryW::LockServer(BOOL fLock)
{
	TraceMethodEnter("CClassFactoryW::LockServer", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IClassFactory, LockServer));

	return GetANSI()->LockServer(fLock);
}



//***************************************************************************
//
//                   IMarshalW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CMarshalW::GetUnmarshalClass, public
//
//  Synopsis:   Thunks GetUnmarshalClass to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalW::GetUnmarshalClass(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPCLSID pCid)
{
	TraceMethodEnter("CMarshalW::GetUnmarshalClass", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IMarshalA, GetUnmarshalClass));

	return GetANSI()->GetUnmarshalClass(riid, pv, dwDestContext, pvDestContext,
			mshlflags, pCid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalW::GetMarshalSizeMax, public
//
//  Synopsis:   Thunks GetMarshalSizeMax to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalW::GetMarshalSizeMax(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPDWORD pSize)
{
	TraceMethodEnter("CMarshalW::GetMarshalSizeMax", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IMarshalA, GetMarshalSizeMax));

	return GetANSI()->GetMarshalSizeMax(riid, pv, dwDestContext, pvDestContext,
			mshlflags, pSize);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalW::MarshalInterface, public
//
//  Synopsis:   Thunks MarshalInterface to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalW::MarshalInterface(LPSTREAM pStm, REFIID riid,
			LPVOID pv, DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags)
{
	TraceMethodEnter("CMarshalW::MarshalInterface", this);

	LPSTREAMA pStmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMarshalA, MarshalInterface));

	hResult = WrapIStreamAFromW(pStm, &pStmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->MarshalInterface(pStmA, riid, pv, dwDestContext,
			pvDestContext, mshlflags);

	if (pStmA)
		pStmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalW::UnmarshalInterface, public
//
//  Synopsis:   Thunks UnmarshalInterface to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalW::UnmarshalInterface(LPSTREAM pStm, REFIID riid,
		LPVOID FAR* ppv)
{
	TraceMethodEnter("CMarshalW::UnmarshalInterface", this);

	LPSTREAMA pStmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMarshalA, UnmarshalInterface));

	hResult = WrapIStreamAFromW(pStm, &pStmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->UnmarshalInterface(pStmA, riid, ppv);

	if (pStmA)
		pStmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalW::ReleaseMarshalData, public
//
//  Synopsis:   Thunks ReleaseMarshalData to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalW::ReleaseMarshalData(LPSTREAM pStm)
{
	TraceMethodEnter("CMarshalW::ReleaseMarshalData", this);

	LPSTREAMA pStmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMarshalA, ReleaseMarshalData));

	hResult = WrapIStreamAFromW(pStm, &pStmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->ReleaseMarshalData(pStmA);

	if (pStmA)
		pStmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalW::DisconnectObject, public
//
//  Synopsis:   Thunks DisconnectObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalW::DisconnectObject(DWORD dwReserved)
{
	TraceMethodEnter("CMarshalW::DisconnectObject", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IMarshalA, DisconnectObject));

	return GetANSI()->DisconnectObject(dwReserved);
}


//***************************************************************************
//
//                   IStdMarshalInfoW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CStdMarshalInfoW::GetClassForHandler, public
//
//  Synopsis:   Thunks GetClassForHandler to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStdMarshalInfoW::GetClassForHandler(DWORD dwMemctx,
			LPVOID pvMemctx, LPCLSID pclsid)
{
	TraceMethodEnter("CStdMarshalInfoW::GetClassForHandler", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IStdMarshalInfoA, GetClassForHandler));

	return GetANSI()->GetClassForHandler(dwMemctx, pvMemctx, pclsid);
}
