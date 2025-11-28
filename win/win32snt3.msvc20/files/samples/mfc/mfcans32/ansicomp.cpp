//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansicomp.cpp
//
//  Contents:   ANSI Wrappers for Unicode CompObj Interfaces and APIs.
//
//  Classes:    CUnknownA      - ANSI wrapper object for IUnknown.
//              CEnumStringA   - ANSI wrapper object for IEnumString.
//              CClassFactoryA - ANSI wrapper object for IClassFactory.
//
//  Functions:  CLSIDFromProgIDA
//              CLSIDFromProgIDA
//              CLSIDFromStringA
//              CoLoadLibraryA
//              CoCreateInstanceA
//              CoGetClassObjectA
//              IIDFromStringA
//              ProgIDFromCLSIDA
//              CoRegisterClassObjectA
//              StringFromCLSIDA
//              StringFromGUID2A
//              StringFromIIDA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IUnknownA Implementation
//
//***************************************************************************


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownA::CUnknownA, public
//
//  Synopsis:   Constructor.
//
//  Arguments:  [pestr] -- Unicode CUnknown object.
//
//  Returns:    OLE2 result code.
//
//---------------------------------------------------------------------------
CUnknownA::CUnknownA(IUnknown * pWide, IDINTERFACE idRef) :
		CInterface(ID_IUnknown | ID_ANSIINTERFACE, NULL, (LPUNKNOWN)pWide)
{
	m_idPrimary    = idRef;

	memset(m_pInterfaces, 0, sizeof(m_pInterfaces));
	TraceAddRef("CUnknownA::CUnknownA", pWide, pWide->AddRef());
}


void CUnknownA::AddInterface(IDINTERFACE idInterface, LPUNKNOWN pUnk)
{
	m_pInterfaces[idInterface] = pUnk;
}


long CUnknownA::FreeInterface(IDINTERFACE idInterface)
{
	WrapDeleteWrapper(m_pInterfaces[idInterface]);
	m_pInterfaces[idInterface] = NULL;
	TraceRelease("CUnknownA::FreeInterface", this, Release());
	return 0;
}


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownA::QueryInterface, public
//
//  Synopsis:   IUnknown::QueryInterface method implementation.
//
//  Arguments:  [riid] -- Interface ID.
//              [ppv]  -- Pointer to query results.
//
//  Returns:    OLE2 result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CUnknownA::QueryInterface(REFIID riid, void * * ppv)
{
	TraceMethodEnter("CUnknownA::QueryInterface", this);

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
	//  Query the Unicode interface and then wrap it.
	//
	hResult = m_pObj->QueryInterface(riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		return hResult;

	*ppv = m_pInterfaces[idRef] = WrapAnyAFromW(this, idRef, pUnk);
	if (m_pInterfaces[idRef] == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	TraceAddRef("CUnknownA::QueryInterface", this, AddRef());

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownA::AddRef, public
//
//  Synopsis:   IUnknown::AddRef method implementation.
//
//  Returns:    Current reference count.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned long) CUnknownA::AddRef()
{
	TraceMethodEnter("CUnknownA::AddRef", this);

	LONG lRet;


	lRet = InterlockedIncrement(&m_refs);

	TraceAddRef("CUnknownA::AddRef", this, m_refs);

	m_pObj->AddRef();

	return lRet;
}


//+--------------------------------------------------------------------------
//
//  Member:     CUnknownA::Release, public
//
//  Synopsis:   IUnknown::Release method implementation.
//
//  Returns:    Current reference count.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned long) CUnknownA::Release()
{
	TraceMethodEnter("CUnknownA::Release", this);

	LONG lRet;


	lRet = InterlockedDecrement(&m_refs);

	TraceRelease("CUnknownA::Release", this, m_refs);

	if (m_pInterfaces[m_idPrimary] != NULL)
	{
		if (m_refs < ((CUnknownA *)m_pInterfaces[m_idPrimary])->m_refs)
		{
			((CUnknownA *)m_pInterfaces[m_idPrimary])->m_refs--;
			if (((CUnknownA *)m_pInterfaces[m_idPrimary])->m_refs == 0)
			{
				((CUnknownA *)m_pInterfaces[m_idPrimary])->m_pObj->Release();
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
					((CUnknownA *)m_pInterfaces[i])->m_pObj->Release();
				WrapDeleteWrapper(m_pInterfaces[i]);
			}
		}

		++m_refs;
		TraceRelease("CUnknownA::Release", m_pObj, m_pObj->Release());
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
//                   IEnumStringA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringA::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringA::Next(
		ULONG celt,
		LPSTR * rgelt,
		ULONG * pceltFetched)
{
	TraceNotify("CEnumStringA::Next");

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumString, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetWide()->Next(celt, (OLECHAR * *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertStringArrayToA(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringA::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringA::Skip(ULONG celt)
{
	TraceNotify("CEnumStringA::Skip");

	_DebugHook(GetWide(), MEMBER_PTR(IEnumString, Skip));

	return GetWide()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringA::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringA::Reset(VOID)
{
	TraceNotify("CEnumStringA::Reset");

	_DebugHook(GetWide(), MEMBER_PTR(IEnumString, Reset));

	return GetWide()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumStringA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumStringA::Clone(IEnumStringA * * ppenmA)
{
	TraceNotify("CEnumStringA::Clone");

	IEnumString * penm;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumString, Clone));

	*ppenmA = NULL;

	HRESULT hResult = GetWide()->Clone(&penm);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumStringAFromW(penm, ppenmA);

	if (penm)
		penm->Release();

	return hResult;
}



//***************************************************************************
//
//                   IClassFactoryA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CClassFactoryA::CreateInstance, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CClassFactoryA::CreateInstance(LPUNKNOWN pUnkOuterA,
		  REFIID riid, LPVOID FAR* ppvObject)
{
	TraceNotify("CClassFactoryA::CreateInstance");

	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IClassFactory, CreateInstance));

	hResult = GetWide()->CreateInstance(pUnkOuterA, riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		return hResult;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObject);

	pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CClassFactoryA::LockServer, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CClassFactoryA::LockServer(BOOL fLock)
{
	TraceNotify("CClassFactoryA::LockServer");

	_DebugHook(GetWide(), MEMBER_PTR(IClassFactory, LockServer));

	return GetWide()->LockServer(fLock);
}



//***************************************************************************
//
//                   IMarshalA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CMarshalA::GetUnmarshalClass, public
//
//  Synopsis:   Thunks GetUnmarshalClass to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalA::GetUnmarshalClass(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPCLSID pCid)
{
	TraceNotify("CMarshalA::GetUnmarshalClass");

	_DebugHook(GetWide(), MEMBER_PTR(IMarshal, GetUnmarshalClass));

	return GetWide()->GetUnmarshalClass(riid, pv, dwDestContext, pvDestContext,
			mshlflags, pCid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalA::GetMarshalSizeMax, public
//
//  Synopsis:   Thunks GetMarshalSizeMax to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalA::GetMarshalSizeMax(REFIID riid, LPVOID pv,
			DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags, LPDWORD pSize)
{
	TraceNotify("CMarshalA::GetMarshalSizeMax");

	_DebugHook(GetWide(), MEMBER_PTR(IMarshal, GetMarshalSizeMax));

	return GetWide()->GetMarshalSizeMax(riid, pv, dwDestContext, pvDestContext,
			mshlflags, pSize);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalA::MarshalInterface, public
//
//  Synopsis:   Thunks MarshalInterface to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalA::MarshalInterface(LPSTREAMA pStmA, REFIID riid,
			LPVOID pv, DWORD dwDestContext, LPVOID pvDestContext,
			DWORD mshlflags)
{
	TraceNotify("CMarshalA::MarshalInterface");

	LPSTREAM pStm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMarshal, MarshalInterface));

	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->MarshalInterface(pStm, riid, pv, dwDestContext,
			pvDestContext, mshlflags);

	if (pStm)
		pStm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalA::UnmarshalInterface, public
//
//  Synopsis:   Thunks UnmarshalInterface to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalA::UnmarshalInterface(LPSTREAMA pStmA, REFIID riid,
		LPVOID FAR* ppv)
{
	TraceNotify("CMarshalA::UnmarshalInterface");

	LPSTREAM pStm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMarshal, UnmarshalInterface));

	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->UnmarshalInterface(pStm, riid, ppv);

	if (pStm)
		pStm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalA::ReleaseMarshalData, public
//
//  Synopsis:   Thunks ReleaseMarshalData to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalA::ReleaseMarshalData(LPSTREAMA pStmA)
{
	TraceNotify("CMarshalA::ReleaseMarshalData");

	LPSTREAM pStm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMarshal, ReleaseMarshalData));

	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->ReleaseMarshalData(pStm);

	if (pStm)
		pStm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMarshalA::DisconnectObject, public
//
//  Synopsis:   Thunks DisconnectObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMarshalA::DisconnectObject(DWORD dwReserved)
{
	TraceNotify("CMarshalA::DisconnectObject");

	_DebugHook(GetWide(), MEMBER_PTR(IMarshal, DisconnectObject));

	return GetWide()->DisconnectObject(dwReserved);
}



//***************************************************************************
//
//                          CompObj API Thunks.
//
//***************************************************************************


//+--------------------------------------------------------------------------
//
//  Routine:    CLSIDFromProgIDA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CLSIDFromProgIDA(LPCSTR lpszProgIDA, LPCLSID lpclsid)
{
	TraceSTDAPIEnter("CLSIDFromProgIDA");

	WCHAR  szProgID[MAX_STRING];


	ConvertStringToW(lpszProgIDA, szProgID);

	return CLSIDFromProgID(szProgID, lpclsid);
}


//+--------------------------------------------------------------------------
//
//  Routine:    CLSIDFromStringA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CLSIDFromStringA(LPSTR lpszA, LPCLSID pclsid)
{
	TraceSTDAPIEnter("CLSIDFromStringA");

	WCHAR sz[MAX_STRING];


	ConvertStringToW(lpszA, sz);

	return CLSIDFromString(sz, pclsid);
}


//+--------------------------------------------------------------------------
//
//  Routine:    CoCreateInstanceA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CoCreateInstanceA(REFCLSID rclsid, LPUNKNOWN pUnkOuterA,
					DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
	TraceSTDAPIEnter("CoCreateInstanceA");

	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;


	hResult = CoCreateInstance(rclsid, pUnkOuterA, dwClsContext, riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppv);

	if (pUnk)
		pUnk->Release();

Error:
	TraceSTDAPIExit("CoCreateInstanceA", hResult);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CoGetClassObjectA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CoGetClassObjectA(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved,
		REFIID riid, LPVOID FAR* ppv)
{
	TraceSTDAPIEnter("CoGetClassObjectA");

	LPUNKNOWN pUnk;
	IDINTERFACE      idRef;
	HRESULT hResult;

	hResult =  CoGetClassObject(rclsid, dwClsContext, pvReserved, riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		return hResult;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppv);

	if (pUnk)
		pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CoRegisterClassObjectA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CoRegisterClassObjectA(REFCLSID rclsid, LPUNKNOWN pUnk,
		DWORD dwClsContext, DWORD flags, LPDWORD lpdwRegister)
{
	TraceSTDAPIEnter("CoRegisterClassObjectA");

	LPUNKNOWN lpCF;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnk, &lpCF);
	if (FAILED(hResult))
		return hResult;

	hResult = CoRegisterClassObject(rclsid, lpCF, dwClsContext, flags,
			lpdwRegister);

	if (lpCF)
		lpCF->Release();

	TraceSTDAPIExit("CoRegisterClassObjectA", hResult);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CoRevokeClassObjectA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CoRevokeClassObjectA(DWORD dwRegister)
{
	TraceSTDAPIEnter("CoRevokeClassObjectA");

	HRESULT hResult;


	hResult = CoRevokeClassObject(dwRegister);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CoLoadLibraryA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI_(HINSTANCE) CoLoadLibraryA(LPSTR pszLibNameA, BOOL bAutoFree)
{
	TraceSTDAPIEnter("CoLoadLibraryA");

	WCHAR szLibName[MAX_STRING];


	ConvertStringToW(pszLibNameA, szLibName);

	return CoLoadLibrary(szLibName, bAutoFree);
}


//+--------------------------------------------------------------------------
//
//  Routine:    CoLockObjectExternalA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CoLockObjectExternalA(LPUNKNOWN pUnkA, BOOL fLock,
		BOOL fLastUnlockReleases)
{
	TraceSTDAPIEnter("CoLockObjectExternalA");

	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnkA, &pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = CoLockObjectExternal(pUnk, fLock, fLastUnlockReleases);

	if (pUnk)
		pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CoDisconnectObjectA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CoDisconnectObjectA(LPUNKNOWN pUnkA, DWORD dwReserved)
{
	TraceSTDAPIEnter("CoDisconnectObjectA");

	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(pUnkA, &pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = CoDisconnectObject(pUnk, dwReserved);

	if (pUnk)
		pUnk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    IIDFromStringA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI IIDFromStringA(LPSTR lpszA, LPIID lpiid)
{
	TraceSTDAPIEnter("IIDFromStringA");

	WCHAR sz[MAX_STRING];


	ConvertStringToW(lpszA, sz);

	return IIDFromString(sz, lpiid);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ProgIDFromCLSIDA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI ProgIDFromCLSIDA(REFCLSID clsid, LPSTR * lplpszProgIDA)
{
	TraceSTDAPIEnter("ProgIDFromCLSIDA");

	LPWSTR lpszProgID;
	HRESULT hResult;


	if (lplpszProgIDA == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = ProgIDFromCLSID(clsid, &lpszProgID);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToA(lpszProgID, lplpszProgIDA);

	ConvertStringFree(lpszProgID);

	return (hResult);
}


//+--------------------------------------------------------------------------
//
//  Routine:    StringFromCLSIDA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StringFromCLSIDA(REFCLSID rclsid, LPSTR * lplpszA)
{
	TraceSTDAPIEnter("StringFromCLSIDA");

	LPWSTR lpsz;
	HRESULT hResult;


	hResult = StringFromCLSID(rclsid, &lpsz);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToA(lpsz, lplpszA);

	ConvertStringFree(lpsz);

	return (hResult);
}


//+--------------------------------------------------------------------------
//
//  Routine:    StringFromGUID2A
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI_(int) StringFromGUID2A(REFGUID rguid, LPSTR szGuid, int cbMax)
{
	TraceSTDAPIEnter("StringFromGUID2A");

   int cchRet;

#ifndef CCH_SZGUID0
// char count of a guid in ansi/unicode form (including trailing null)
#define CCH_SZGUID0 39
#endif

   OLECHAR szGuidW[CCH_SZGUID0];

   cchRet = StringFromGUID2(rguid, szGuidW, CCH_SZGUID0);

   // convert szGuidW from unicode to Ansi.  Can't just convert in place.
   WideCharToMultiByte(CP_ACP,
			   0,
			   szGuidW,
			   CCH_SZGUID0,
			   szGuid,
			   cbMax,
			   NULL,
			   NULL);
   return cchRet;
}


//+--------------------------------------------------------------------------
//
//  Routine:    StringFromIIDA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI StringFromIIDA(REFIID rclsid, LPSTR * lplpszA)
{
	TraceSTDAPIEnter("StringFromIIDA");

	LPWSTR lpsz;
	HRESULT hResult;

	hResult = StringFromIID(rclsid, &lpsz);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToA(lpsz, lplpszA);

	ConvertStringFree(lpsz);

	return (hResult);
}


//***************************************************************************
//
//                   IStdMarshalInfoA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CStdMarshalInfoA::GetClassForHandler, public
//
//  Synopsis:   Thunks GetClassForHandler to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CStdMarshalInfoA::GetClassForHandler(DWORD dwMemctx,
			LPVOID pvMemctx, LPCLSID pclsid)
{
	TraceMethodEnter("CStdMarshalInfoA::GetClassForHandler", this);

	_DebugHook(GetWide(), MEMBER_PTR(IStdMarshalInfo, GetClassForHandler));

	return GetWide()->GetClassForHandler(dwMemctx, pvMemctx, pclsid);
}
