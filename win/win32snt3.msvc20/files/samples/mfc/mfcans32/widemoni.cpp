//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       widemoni.h
//
//  Contents:   Unicode Wrappers for ANSI Moniker Interfaces.
//
//  Classes:    CBindCtxW
//              CMonikerW
//              CRunningObjectTableW
//              CEnumMonikerW
//
//  History:    01-Nov-93   v-kentc     Created.
//                      28-Mar-94   v-kentc         Wrap all IUnknowns.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IBindCtxW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::RegisterObjectBound, public
//
//  Synopsis:   Thunks RegisterObjectBound to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::RegisterObjectBound(LPUNKNOWN punk)
{
	LPUNKNOWN punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, RegisterObjectBound));

	hResult = WrapIUnknownAFromW(punk, &punkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->RegisterObjectBound(punkA);

	punkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::RevokeObjectBound, public
//
//  Synopsis:   Thunks RevokeObjectBound to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::RevokeObjectBound(LPUNKNOWN punk)
{
	LPUNKNOWN punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, RevokeObjectBound));

	hResult = WrapIUnknownAFromW(punk, &punkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->RevokeObjectBound(punkA);

	punkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::ReleaseBoundObjects, public
//
//  Synopsis:   Thunks ReleaseBoundObjects to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::ReleaseBoundObjects()
{
	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, ReleaseBoundObjects));

	return GetANSI()->ReleaseBoundObjects();
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::SetBindOptions, public
//
//  Synopsis:   Thunks SetBindOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::SetBindOptions(LPBIND_OPTS pbindopts)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, SetBindOptions));

	return GetANSI()->SetBindOptions(pbindopts);
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::GetBindOptions, public
//
//  Synopsis:   Thunks GetBindOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::GetBindOptions(LPBIND_OPTS pbindopts)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, GetBindOptions));

	return GetANSI()->GetBindOptions(pbindopts);
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::GetRunningObjectTable, public
//
//  Synopsis:   Thunks GetRunningObjectTable to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::GetRunningObjectTable(LPRUNNINGOBJECTTABLE * pprot)
{
	TraceMethodEnter("CBindCtxW::GetRunningObjectTable", this);

	LPRUNNINGOBJECTTABLEA protA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, GetRunningObjectTable));

	*pprot = NULL;

	hResult = GetANSI()->GetRunningObjectTable(&protA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIRunningObjectTableWFromA(protA, pprot);

	if (protA)
		protA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::RegisterObjectParam, public
//
//  Synopsis:   Thunks RegisterObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::RegisterObjectParam(LPOLESTR lpszKey, LPUNKNOWN punk)
{
	TraceMethodEnter("CBindCtxW::RegisterObjectParam", this);

	LPSTR lpszKeyA;
	LPUNKNOWN punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, RegisterObjectParam));

	hResult = ConvertStringToA(lpszKey, &lpszKeyA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIUnknownAFromW(punk, &punkA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->RegisterObjectParam(lpszKeyA, punkA);

	punkA->Release();

Error:
	ConvertStringFree(lpszKeyA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::GetObjectParam, public
//
//  Synopsis:   Thunks GetObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::GetObjectParam(LPOLESTR lpszKey, LPUNKNOWN * ppunk)
{
	TraceMethodEnter("CBindCtxW::GetObjectParam", this);

	LPSTR lpszKeyA;
	LPUNKNOWN punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, GetObjectParam));

	*ppunk = NULL;

	hResult = ConvertStringToA(lpszKey, &lpszKeyA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->GetObjectParam(lpszKeyA, &punkA);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIUnknownWFromA(punkA, ppunk);

	punkA->Release();

Error:
	ConvertStringFree(lpszKeyA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::EnumObjectParam, public
//
//  Synopsis:   Thunks EnumObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::EnumObjectParam(LPENUMSTRING * ppenum)
{
	TraceMethodEnter("CBindCtxW::EnumObjectParam", this);

	LPENUMSTRINGA penumA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, EnumObjectParam));

	*ppenum = NULL;

	hResult = GetANSI()->EnumObjectParam(&penumA);
	if (FAILED(hResult))
		return hResult;

	if (penumA)
	{
		hResult = WrapIEnumStringWFromA(penumA, ppenum);
		penumA->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxW::RevokeObjectParam, public
//
//  Synopsis:   Thunks RevokeObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxW::RevokeObjectParam(LPOLESTR lpszKey)
{
	TraceMethodEnter("CBindCtxW::RevokeObjectParam", this);

	LPSTR lpszKeyA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IBindCtxA, RevokeObjectParam));

	hResult = ConvertStringToA(lpszKey, &lpszKeyA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->RevokeObjectParam(lpszKeyA);

	ConvertStringFree(lpszKeyA);

	return hResult;
}



//***************************************************************************
//
//                   IMonikerW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::GetClassID, public
//
//  Synopsis:   Thunks GetClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::GetClassID(LPCLSID lpClassID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, GetClassID));

	return GetANSI()->GetClassID(lpClassID);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::IsDirty, public
//
//  Synopsis:   Thunks IsDirty to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::IsDirty(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, IsDirty));

	return GetANSI()->IsDirty();
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::Load, public
//
//  Synopsis:   Thunks Load to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::Load(LPSTREAM pStrm)
{
	TraceMethodEnter("CMonikerW::Load", this);

	LPSTREAMA pStrmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, Load));

	hResult = WrapIStreamAFromW(pStrm, &pStrmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Load(pStrmA);

	if (pStrmA)
		pStrmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::Save, public
//
//  Synopsis:   Thunks Save to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::Save(LPSTREAM pStrm, BOOL fClearDirty)
{
	TraceMethodEnter("CMonikerW::Save", this);

	LPSTREAMA pStrmA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, Save));

	hResult = WrapIStreamAFromW(pStrm, &pStrmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Save(pStrmA, fClearDirty);

	if (pStrmA)
		pStrmA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::GetSizeMax, public
//
//  Synopsis:   Thunks GetSizeMax to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::GetSizeMax(ULARGE_INTEGER * pcbSize)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, GetSizeMax));

	return GetANSI()->GetSizeMax(pcbSize);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::BindToObject, public
//
//  Synopsis:   Thunks BindToObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::BindToObject(LPBC pbc, LPMONIKER pmkToLeft,
		REFIID riidResult, LPVOID * ppvResult)
{
	TraceMethodEnter("CMonikerW::BindToObject", this);

	LPBCA pbcA;
	LPMONIKERA pmkA;
	LPUNKNOWN punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, BindToObject));

	*ppvResult = NULL;

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmkToLeft, &pmkA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->BindToObject(pbcA, pmkA, riidResult, (LPVOID *)&punkA);

	if (punkA)
	{
		IDINTERFACE idRef = WrapTranslateIID(riidResult);

		hResult = WrapInterfaceWFromA(idRef, punkA, (LPUNKNOWN *)ppvResult);

		punkA->Release();
	}

	if (pmkA)
		pmkA->Release();

Error:
	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::BindToStorage, public
//
//  Synopsis:   Thunks BindToStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::BindToStorage(LPBC pbc, LPMONIKER pmkToLeft,
		REFIID riid, LPVOID * ppvObj)
{
	TraceMethodEnter("CMonikerW::BindToStorage", this);

	LPBCA pbcA;
	LPMONIKERA pmkA;
	LPUNKNOWN punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, BindToStorage));

	*ppvObj = NULL;

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmkToLeft, &pmkA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->BindToStorage(pbcA, pmkA, riid, (LPVOID *)&punkA);

	if (punkA)
	{
		IDINTERFACE idRef = WrapTranslateIID(riid);

		hResult = WrapInterfaceWFromA(idRef, punkA, (LPUNKNOWN *)ppvObj);

		punkA->Release();
	}

	if (pmkA)
		pmkA->Release();

Error:
	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::Reduce, public
//
//  Synopsis:   Thunks Reduce to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::Reduce(LPBC pbc, DWORD dwReduceHowFar,
		LPMONIKER * ppmkToLeft, LPMONIKER * ppmkReduced)
{
	TraceMethodEnter("CMonikerW::Reduce", this);

	LPBCA pbcA;
	LPMONIKERA pmkToLeftA, * ppmkToLeftA;
	LPMONIKERA pmkReducedA, * ppmkReducedA;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, Reduce));

	if (ppmkToLeft)
		*ppmkToLeft = NULL;

	if (ppmkReduced)
		*ppmkReduced = NULL;

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	if (ppmkToLeft)
		ppmkToLeftA = &pmkToLeftA;
	else
		ppmkToLeftA = NULL;

	if (ppmkReduced)
		ppmkReducedA = &pmkReducedA;
	else
		ppmkReducedA = NULL;

	hReturn = GetANSI()->Reduce(pbcA, dwReduceHowFar, ppmkToLeftA, ppmkReducedA);
	if (FAILED(hReturn))
		goto Error;

	if (ppmkToLeftA)
	{
		hResult = WrapIMonikerWFromA(pmkToLeftA, ppmkToLeft);
		if (FAILED(hResult))
			goto Error1;
	}

	if (ppmkReducedA)
		hResult = WrapIMonikerWFromA(pmkReducedA, ppmkReduced);

Error1:
	if (ppmkToLeftA && *ppmkToLeftA)
		pmkToLeftA->Release();

	if (ppmkReducedA && *ppmkReducedA)
		pmkReducedA->Release();

Error:
	if (pbcA)
		pbcA->Release();

	if (FAILED(hResult))
		hReturn = hResult;

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::ComposeWith, public
//
//  Synopsis:   Thunks ComposeWith to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::ComposeWith(LPMONIKER pmkRight,
		BOOL fOnlyIfNotGeneric, LPMONIKER * ppmkComposite)
{
	TraceMethodEnter("CMonikerW::ComposeWith", this);

	LPMONIKERA pmkRightA;
	LPMONIKERA pmkCompositeA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, ComposeWith));

	*ppmkComposite = NULL;

	hResult = WrapIMonikerAFromW(pmkRight, &pmkRightA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->ComposeWith(pmkRightA, fOnlyIfNotGeneric, &pmkCompositeA);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerWFromA(pmkCompositeA, ppmkComposite);

	if (pmkCompositeA)
		pmkCompositeA->Release();

Error:
	if (pmkRightA)
		pmkRightA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::Enum, public
//
//  Synopsis:   Thunks Enum to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::Enum(BOOL fForward, LPENUMMONIKER * ppenumMoniker)
{
	TraceMethodEnter("CMonikerW::Enum", this);

	LPENUMMONIKERA penumA = NULL;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, Enum));

	*ppenumMoniker = NULL;

	hResult = GetANSI()->Enum(fForward, &penumA);
	if (FAILED(hResult))
		return hResult;

	if (penumA)
	{
		hResult = WrapIEnumMonikerWFromA(penumA, ppenumMoniker);
		penumA->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::IsEqual, public
//
//  Synopsis:   Thunks IsEqual to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::IsEqual(LPMONIKER pmkOtherMoniker)
{
	TraceMethodEnter("CMonikerW::IsEqual", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, IsEqual));

	hResult = WrapIMonikerAFromW(pmkOtherMoniker, &pmkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->IsEqual(pmkA);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::Hash, public
///
//  Synopsis:   Thunks Hash to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::Hash(LPDWORD pdwHash)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, Hash));

	return GetANSI()->Hash(pdwHash);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::IsRunning(LPBC pbc, LPMONIKER pmkToLeft,
		LPMONIKER pmkNewlyRunning)
{
	TraceMethodEnter("CMonikerW::IsRunning", this);

	LPBCA pbcA;
	LPMONIKERA pmkToLeftA;
	LPMONIKERA pmkRunningA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, IsRunning));

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmkToLeft, &pmkToLeftA);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerAFromW(pmkNewlyRunning, &pmkRunningA);
	if (FAILED(hResult))
		goto Error1;

	hResult = GetANSI()->IsRunning(pbcA, pmkToLeftA, pmkRunningA);

	if (pmkRunningA)
		pmkRunningA->Release();

Error1:
	if (pmkToLeftA)
		pmkToLeftA->Release();

Error:
	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::GetTimeOfLastChange, public
//
//  Synopsis:   Thunks GetTimeOfLastChange to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::GetTimeOfLastChange(LPBC pbc,
		LPMONIKER pmkToLeft, FILETIME * pfiletime)
{
	TraceMethodEnter("CMonikerW::GetTimeOfLastChange", this);

	LPBCA pbcA;
	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, GetTimeOfLastChange));

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmkToLeft, &pmkA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->GetTimeOfLastChange(pbcA, pmkA, pfiletime);

	if (pmkA)
		pmkA->Release();

Error:
	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::Inverse, public
//
//  Synopsis:   Thunks Inverse to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::Inverse(LPMONIKER * ppmk)
{
	TraceMethodEnter("CMonikerW::Inverse", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, Inverse));

	*ppmk = NULL;

	hResult = GetANSI()->Inverse(&pmkA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkA, ppmk);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::CommonPrefixWith, public
//
//  Synopsis:   Thunks CommonPrefixWith to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::CommonPrefixWith(LPMONIKER pmkOther,
		LPMONIKER * ppmkPrefix)
{
	TraceMethodEnter("CMonikerW::CommonPrefixWith", this);

	LPMONIKERA pmkOtherA;
	LPMONIKERA pmkPrefixA;
	HRESULT hResult;


	*ppmkPrefix = NULL;

	hResult = WrapIMonikerAFromW(pmkOther, &pmkOtherA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->CommonPrefixWith(pmkOtherA, &pmkPrefixA);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerWFromA(pmkPrefixA, ppmkPrefix);

	if (pmkPrefixA)
		pmkPrefixA->Release();

Error:
	if (pmkOtherA)
		pmkOtherA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::RelativePathTo, public
//
//  Synopsis:   Thunks RelativePathTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::RelativePathTo(LPMONIKER pmkOther,
		LPMONIKER * ppmkRelPath)
{
	TraceMethodEnter("CMonikerW::RelativePathTo", this);

	LPMONIKERA pmkOtherA;
	LPMONIKERA pmkRelPathA;
	HRESULT hResult;


	*ppmkRelPath = NULL;

	hResult = WrapIMonikerAFromW(pmkOther, &pmkOtherA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->RelativePathTo(pmkOtherA, &pmkRelPathA);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerWFromA(pmkRelPathA, ppmkRelPath);

	if (pmkRelPathA)
		pmkRelPathA->Release();

Error:
	if (pmkOtherA)
		pmkOtherA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::GetDisplayName, public
//
//  Synopsis:   Thunks GetDisplayName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::GetDisplayName(LPBC pbc, LPMONIKER pmkToLeft,
		LPOLESTR * lplpszDisplayName)
{
	TraceMethodEnter("CMonikerW::GetDisplayName", this);

	LPBCA pbcA;
	LPMONIKERA pmkToLeftA;
	LPSTR lpszDisplayNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, RelativePathTo));

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmkToLeft, &pmkToLeftA);
	if (FAILED(hResult))
		goto Error;

	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, GetDisplayName));

	hResult = GetANSI()->GetDisplayName(pbcA, pmkToLeftA, &lpszDisplayNameA);
	if (FAILED(hResult))
		goto Error1;

	hResult = ConvertStringToW(lpszDisplayNameA, lplpszDisplayName);

Error1:
	if (pmkToLeftA)
		pmkToLeftA->Release();

Error:
	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::ParseDisplayName, public
//
//  Synopsis:   Thunks ParseDisplayName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::ParseDisplayName(LPBC pbc, LPMONIKER pmkToLeft,
		LPOLESTR lpszDisplayName, ULONG * pchEaten, LPMONIKER * ppmkOut)
{
	TraceMethodEnter("CMonikerW::ParseDisplayName", this);

	LPBCA pbcA;
	LPMONIKERA pmkToLeftA;
	LPSTR lpszDisplayNameA;
	LPMONIKERA pmkOutA;
	HRESULT hResult;


	*ppmkOut = NULL;

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmkToLeft, &pmkToLeftA);
	if (FAILED(hResult))
		goto Error;

	hResult = ConvertStringToA(lpszDisplayName, &lpszDisplayNameA);
	if (FAILED(hResult))
		goto Error1;

	hResult = GetANSI()->ParseDisplayName(pbcA, pmkToLeftA, lpszDisplayNameA,
			pchEaten, &pmkOutA);
	if (FAILED(hResult))
		goto Error2;

	hResult = WrapIMonikerWFromA(pmkOutA, ppmkOut);

	if (pmkOutA)
		pmkOutA->Release();

Error2:
	ConvertStringFree(lpszDisplayNameA);

Error1:
	if (pmkToLeftA)
		pmkToLeftA->Release();

Error:
	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerW::IsSystemMoniker, public
//
//  Synopsis:   Thunks IsSystemMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerW::IsSystemMoniker(LPDWORD pdwMksys)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IMonikerA, IsSystemMoniker));

	return GetANSI()->IsSystemMoniker(pdwMksys);
}



//***************************************************************************
//
//                   IRunningObjectTableW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableW::Register, public
//
//  Synopsis:   Thunks Register to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableW::Register(DWORD grfFlags,
		LPUNKNOWN punkObject, LPMONIKER pmkObjectName, DWORD * pdwRegister)
{
	TraceMethodEnter("CRunningObjectTableW::Register", this);

	LPUNKNOWN  punkA;
	LPMONIKERA pmkObjectNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IRunningObjectTableA, Register));

	hResult = WrapIMonikerAFromW(pmkObjectName, &pmkObjectNameA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIUnknownAFromW(punkObject, &punkA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->Register(grfFlags, punkA, pmkObjectNameA,
			pdwRegister);

	punkA->Release();

Error:
	if (pmkObjectNameA)
		pmkObjectNameA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableW::Revoke, public
//
//  Synopsis:   Thunks Revoke to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableW::Revoke(DWORD dwRegister)
{
	TraceMethodEnter("CRunningObjectTableW::Revoke", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IRunningObjectTableA, Revoke));

	return GetANSI()->Revoke(dwRegister);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableW::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableW::IsRunning(LPMONIKER pmkObjectName)
{
	TraceMethodEnter("CRunningObjectTableW::IsRunning", this);

	LPMONIKERA pmkObjectNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IRunningObjectTableA, IsRunning));

	hResult = WrapIMonikerAFromW(pmkObjectName, &pmkObjectNameA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->IsRunning(pmkObjectNameA);

	if (pmkObjectNameA)
		pmkObjectNameA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableW::GetObject, public
//
//  Synopsis:   Thunks GetObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableW::GetObject(LPMONIKER pmkObjectName,
		LPUNKNOWN * ppunkObject)
{
	TraceMethodEnter("CRunningObjectTableW::GetObject", this);

	LPMONIKERA pmkObjectNameA;
	LPUNKNOWN  punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IRunningObjectTableA, GetObject));

	*ppunkObject = NULL;

	hResult = WrapIMonikerAFromW(pmkObjectName, &pmkObjectNameA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->GetObject(pmkObjectNameA, &punkA);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIUnknownWFromA(punkA, ppunkObject);

	punkA->Release();

Error:
	if (pmkObjectNameA)
		pmkObjectNameA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableW::NoteChangeTime, public
//
//  Synopsis:   Thunks NoteChangeTime to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableW::NoteChangeTime(DWORD dwRegister,
		FILETIME * pfiletime)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IRunningObjectTableA, NoteChangeTime));

	return GetANSI()->NoteChangeTime(dwRegister, pfiletime);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableW::GetTimeOfLastChange, public
//
//  Synopsis:   Thunks GetTimeOfLastChange to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableW::GetTimeOfLastChange(
		LPMONIKER pmkObjectName, FILETIME * pfiletime)
{
	TraceMethodEnter("CRunningObjectTableW::GetTimeOfLastChange", this);

	LPMONIKERA pmkObjectNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IRunningObjectTableA, GetTimeOfLastChange));

	hResult = WrapIMonikerAFromW(pmkObjectName, &pmkObjectNameA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->GetTimeOfLastChange(pmkObjectNameA, pfiletime);

	if (pmkObjectNameA)
		pmkObjectNameA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableW::EnumRunning, public
//
//  Synopsis:   Thunks EnumRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableW::EnumRunning(
		LPENUMMONIKER * ppenumMoniker)
{
	TraceMethodEnter("CRunningObjectTableW::EnumRunning", this);

	LPENUMMONIKERA penumMonikerA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IRunningObjectTableA, EnumRunning));

	*ppenumMoniker = NULL;

	hResult = GetANSI()->EnumRunning(&penumMonikerA);
	if (FAILED(hResult))
		return hResult;

	if (penumMonikerA)
	{
		hResult = WrapIEnumMonikerWFromA(penumMonikerA, ppenumMoniker);
		penumMonikerA->Release();
	}

	return hResult;
}



//***************************************************************************
//
//                   IEnumMonikerW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerW::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerW::Next(
		ULONG celt,
		LPMONIKER * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumMonikerW::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumMonikerA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetANSI()->Next(celt, (LPMONIKERA *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return hReturn;

	hResult = ConvertMonikerArrayToW(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return hResult;

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerW::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerW::Skip(ULONG celt)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumMonikerA, Skip));

	return GetANSI()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerW::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerW::Reset(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumMonikerA, Reset));

	return GetANSI()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerW::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerW::Clone(IEnumMoniker * * ppenm)
{
	TraceMethodEnter("CEnumMonikerW::Clone", this);

	IEnumMonikerA * penmA;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumMonikerA, Clone));

	*ppenm = NULL;

	HRESULT hResult = GetANSI()->Clone(&penmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumMonikerWFromA(penmA, ppenm);

	if (penmA)
		penmA->Release();

	return hResult;
}
