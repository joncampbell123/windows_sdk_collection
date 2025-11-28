//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansimoni.h
//
//  Contents:   ANSI Wrappers for Unicode Moniker Interfaces and APIs.
//
//  Classes:    CBindCtxA
//              CMonikerA
//              CRunningObjectTableA
//              CEnumMonikerA
//
//  Functions:  BindMonikerA
//              MkParseDisplayNameA
//              MonikerRelativePathToA
//              MonikerCommonPrefixWithA
//              CreateBindCtxA
//              CreateGenericCompositeA
//              GetClassFileA
//              CreateFileMonikerA
//              CreateItemMonikerA
//              CreateAntiMonikerA
//              CreatePointerMonikerA
//              GetRunningObjectTableA
//              IBindCtxAFromW
//              IMonikerAFromW
//              IRunningObjectTableAFromW
//              IEnumMonikerAFromW
//
//  History:    01-Nov-93   v-kentc     Created.
//                      28-Mar-94   v-kentc     Wrap all IUnknowns.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IBindCtxA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::RegisterObjectBound, public
//
//  Synopsis:   Thunks RegisterObjectBound to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::RegisterObjectBound(LPUNKNOWN punkA)
{
	TraceMethodEnter("CBindCtxA::RegisterObjectBound", this);

	LPUNKNOWN punk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, RegisterObjectBound));

	hResult = WrapIUnknownWFromA(punkA, &punk);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->RegisterObjectBound(punk);

	punk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::RevokeObjectBound, public
//
//  Synopsis:   Thunks RevokeObjectBound to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::RevokeObjectBound(LPUNKNOWN punkA)
{
	TraceMethodEnter("CBindCtxA::RevokeObjectBound", this);

	LPUNKNOWN punk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, RevokeObjectBound));

	hResult = WrapIUnknownWFromA(punkA, &punk);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->RevokeObjectBound(punk);

	punk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::ReleaseBoundObjects, public
//
//  Synopsis:   Thunks ReleaseBoundObjects to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::ReleaseBoundObjects()
{
	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, ReleaseBoundObjects));

	return GetWide()->ReleaseBoundObjects();
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::SetBindOptions, public
//
//  Synopsis:   Thunks SetBindOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::SetBindOptions(LPBIND_OPTS pbindopts)
{
	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, SetBindOptions));

	return GetWide()->SetBindOptions(pbindopts);
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::GetBindOptions, public
//
//  Synopsis:   Thunks GetBindOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::GetBindOptions(LPBIND_OPTS pbindopts)
{
	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, GetBindOptions));

	return GetWide()->GetBindOptions(pbindopts);
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::GetRunningObjectTable, public
//
//  Synopsis:   Thunks GetRunningObjectTable to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::GetRunningObjectTable(LPRUNNINGOBJECTTABLEA * pprotA)
{
	TraceMethodEnter("CBindCtxA::GetRunningObjectTable", this);

	LPRUNNINGOBJECTTABLE prot;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, GetRunningObjectTable));

	*pprotA = NULL;

	hResult = GetWide()->GetRunningObjectTable(&prot);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIRunningObjectTableAFromW(prot, pprotA);

	if (prot)
		prot->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::RegisterObjectParam, public
//
//  Synopsis:   Thunks RegisterObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::RegisterObjectParam(LPSTR lpszKeyA, LPUNKNOWN punkA)
{
	TraceMethodEnter("CBindCtxA::RegisterObjectParam", this);

	LPOLESTR lpszKey;
	LPUNKNOWN punk;
	HRESULT  hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, RegisterObjectParam));

	hResult = ConvertStringToW(lpszKeyA, &lpszKey);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIUnknownWFromA(punkA, &punk);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->RegisterObjectParam(lpszKey, punk);

	punk->Release();

Error:
	ConvertStringFree(lpszKey);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::GetObjectParam, public
//
//  Synopsis:   Thunks GetObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::GetObjectParam(LPSTR lpszKeyA, LPUNKNOWN * ppunkA)
{
	TraceMethodEnter("CBindCtxA::GetObjectParam", this);

	LPOLESTR  lpszKey;
	LPUNKNOWN punk;
	HRESULT   hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, GetObjectParam));

	*ppunkA = NULL;

	hResult = ConvertStringToW(lpszKeyA, &lpszKey);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->GetObjectParam(lpszKey, &punk);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIUnknownAFromW(punk, ppunkA);
	punk->Release();

Error:
	ConvertStringFree(lpszKey);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::EnumObjectParam, public
//
//  Synopsis:   Thunks EnumObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::EnumObjectParam(LPENUMSTRINGA * ppenumA)
{
	TraceMethodEnter("CBindCtxA::EnumObjectParam", this);

	LPENUMSTRING penum;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, EnumObjectParam));

	*ppenumA = NULL;

	hResult = GetWide()->EnumObjectParam(&penum);
	if (FAILED(hResult))
		return hResult;

	if (penum)
	{
		hResult = WrapIEnumStringAFromW(penum, ppenumA);
		penum->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CBindCtxA::RevokeObjectParam, public
//
//  Synopsis:   Thunks RevokeObjectParam to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CBindCtxA::RevokeObjectParam(LPSTR lpszKeyA)
{
	TraceMethodEnter("CBindCtxA::RevokeObjectParam", this);

	LPOLESTR lpszKey;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IBindCtx, RevokeObjectParam));

	hResult = ConvertStringToW(lpszKeyA, &lpszKey);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->RevokeObjectParam(lpszKey);

	ConvertStringFree(lpszKey);

	return hResult;
}



//***************************************************************************
//
//                   IMonikerA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::GetClassID, public
//
//  Synopsis:   Thunks GetClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::GetClassID(LPCLSID lpClassID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IPersist, GetClassID));

	return GetWide()->GetClassID(lpClassID);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::IsDirty, public
//
//  Synopsis:   Thunks IsDirty to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::IsDirty(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStream, IsDirty));

	return GetWide()->IsDirty();
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::Load, public
//
//  Synopsis:   Thunks Load to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::Load(LPSTREAMA pStrmA)
{
	TraceMethodEnter("CMonikerA::Load", this);

	LPSTREAM pStrm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistStream, Load));

	hResult = WrapIStreamWFromA(pStrmA, &pStrm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Load(pStrm);

	if (pStrm)
		pStrm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::Save, public
//
//  Synopsis:   Thunks Save to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::Save(LPSTREAMA pStrmA, BOOL fClearDirty)
{
	TraceMethodEnter("CMonikerA::Save", this);

	LPSTREAM pStrm;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistStream, Save));

	hResult = WrapIStreamWFromA(pStrmA, &pStrm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Save(pStrm, fClearDirty);

	if (pStrm)
		pStrm->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::GetSizeMax, public
//
//  Synopsis:   Thunks GetSizeMax to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::GetSizeMax(ULARGE_INTEGER * pcbSize)
{
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStream, GetSizeMax));

	return GetWide()->GetSizeMax(pcbSize);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::BindToObject, public
//
//  Synopsis:   Thunks BindToObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::BindToObject(LPBCA pbcA, LPMONIKERA pmkToLeftA,
		REFIID riidResult, LPVOID * ppvResult)
{
	TraceMethodEnter("CMonikerA::BindToObject", this);

	LPBC        pbc;
	LPMONIKER   pmk;
	LPUNKNOWN   punk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, BindToObject));

	*ppvResult = NULL;

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkToLeftA, &pmk);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->BindToObject(pbc, pmk, riidResult, (LPVOID *)&punk);
	if (FAILED(hResult))
		goto Error1;

	if (punk)
	{
		IDINTERFACE idRef = WrapTranslateIID(riidResult);
		hResult = WrapInterfaceAFromW(idRef, punk, (LPUNKNOWN *)ppvResult);
		punk->Release();
	}

Error1:
	if (pmk)
		pmk->Release();

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::BindToStorage, public
//
//  Synopsis:   Thunks BindToStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::BindToStorage(LPBCA pbcA, LPMONIKERA pmkToLeftA,
		REFIID riid, LPVOID * ppvObj)
{
	TraceMethodEnter("CMonikerA::BindToStorage", this);

	LPBC pbc;
	LPMONIKER pmk;
	LPUNKNOWN punk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, BindToStorage));

	*ppvObj = NULL;

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkToLeftA, &pmk);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->BindToStorage(pbc, pmk, riid, (LPVOID *)&punk);
	if (FAILED(hResult))
		goto Error1;

	if (punk)
	{
		IDINTERFACE idRef = WrapTranslateIID(riid);
		hResult = WrapInterfaceAFromW(idRef, punk, (LPUNKNOWN *)ppvObj);
		punk->Release();
	}

Error1:
	if (pmk)
		pmk->Release();

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::Reduce, public
//
//  Synopsis:   Thunks Reduce to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::Reduce(LPBCA pbcA, DWORD dwReduceHowFar,
		LPMONIKERA * ppmkToLeftA, LPMONIKERA * ppmkReducedA)
{
	TraceMethodEnter("CMonikerA::Reduce", this);

	LPBC pbc;
	LPMONIKER pmkToLeft, * ppmkToLeft;
	LPMONIKER pmkReduced, * ppmkReduced;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, Reduce));

	if (ppmkToLeftA)
		*ppmkToLeftA = NULL;

	if (ppmkReducedA)
		*ppmkReducedA = NULL;

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	if (ppmkToLeftA)
		ppmkToLeft = &pmkToLeft;
	else
		ppmkToLeft = NULL;

	if (ppmkReducedA)
		ppmkReduced = &pmkReduced;
	else
		ppmkReduced = NULL;

	hReturn = GetWide()->Reduce(pbc, dwReduceHowFar, ppmkToLeft,
			ppmkReduced);
	if (FAILED(hReturn))
		goto Error;

	if (ppmkToLeft)
	{
		hResult = WrapIMonikerAFromW(pmkToLeft, ppmkToLeftA);
		if (FAILED(hResult))
			goto Error1;
	}

	if (ppmkReduced)
	{
		hResult = WrapIMonikerAFromW(pmkReduced, ppmkReducedA);
		if (FAILED(hResult))
			goto Error1;
	}

Error1:
	if (ppmkToLeft && *ppmkToLeft)
		pmkToLeft->Release();

	if (ppmkReduced && *ppmkReduced)
		pmkReduced->Release();

Error:
	if (pbc)
		pbc->Release();

	if (FAILED(hResult))
		hReturn = hResult;

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::ComposeWith, public
//
//  Synopsis:   Thunks ComposeWith to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::ComposeWith(LPMONIKERA pmkRightA,
		BOOL fOnlyIfNotGeneric, LPMONIKERA * ppmkCompositeA)
{
	TraceMethodEnter("CMonikerA::ComposeWith", this);

	LPMONIKER pmk;
	LPMONIKER pmkComposite;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, ComposeWith));

	*ppmkCompositeA = NULL;

	hResult = WrapIMonikerWFromA(pmkRightA, &pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->ComposeWith(pmk, fOnlyIfNotGeneric, &pmkComposite);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerAFromW(pmkComposite, ppmkCompositeA);

	if (pmkComposite)
		pmkComposite->Release();

Error:
	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::Enum, public
//
//  Synopsis:   Thunks Enum to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::Enum(BOOL fForward, LPENUMMONIKERA * ppenumMonikerA)
{
	TraceMethodEnter("CMonikerA::Enum", this);

	LPENUMMONIKER penum;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, Enum));

	*ppenumMonikerA = NULL;

	hResult = GetWide()->Enum(fForward, &penum);
	if (FAILED(hResult))
		return hResult;

	if (penum)
	{
		hResult = WrapIEnumMonikerAFromW(penum, ppenumMonikerA);
		penum->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::IsEqual, public
//
//  Synopsis:   Thunks IsEqual to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::IsEqual(LPMONIKERA pmkOtherMonikerA)
{
	TraceMethodEnter("CMonikerA::IsEqual", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, IsEqual));

	hResult = WrapIMonikerWFromA(pmkOtherMonikerA, &pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->IsEqual(pmk);

	pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::Hash, public
///
//  Synopsis:   Thunks Hash to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::Hash(LPDWORD pdwHash)
{
	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, Hash));

	return GetWide()->Hash(pdwHash);
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::IsRunning(LPBCA pbcA, LPMONIKERA pmkToLeftA,
		LPMONIKERA pmkNewlyRunningA)
{
	TraceMethodEnter("CMonikerA::IsRunning", this);

	LPBC pbc;
	LPMONIKER pmk;
	LPMONIKER pmkRunning;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, IsRunning));

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkToLeftA, &pmk);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerWFromA(pmkNewlyRunningA, &pmkRunning);
	if (FAILED(hResult))
		goto Error1;

	hResult = GetWide()->IsRunning(pbc, pmk, pmkRunning);

	if (pmkRunning)
		pmkRunning->Release();

Error1:
	if (pmk)
		pmk->Release();

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::GetTimeOfLastChange, public
//
//  Synopsis:   Thunks GetTimeOfLastChange to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::GetTimeOfLastChange(LPBCA pbcA,
		LPMONIKERA pmkToLeftA, FILETIME * pfiletime)
{
	TraceMethodEnter("CMonikerA::GetTimeOfLastChange", this);

	LPBC pbc;
	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, GetTimeOfLastChange));

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkToLeftA, &pmk);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->GetTimeOfLastChange(pbc, pmk, pfiletime);

	if (pmk)
		pmk->Release();

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::Inverse, public
//
//  Synopsis:   Thunks Inverse to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::Inverse(LPMONIKERA * ppmkA)
{
	TraceMethodEnter("CMonikerA::Inverse", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, Inverse));

	*ppmkA = NULL;

	hResult = GetWide()->Inverse(&pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::CommonPrefixWith, public
//
//  Synopsis:   Thunks CommonPrefixWith to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::CommonPrefixWith(LPMONIKERA pmkOtherA,
		LPMONIKERA * ppmkPrefixA)
{
	TraceMethodEnter("CMonikerA::CommonPrefixWith", this);

	LPMONIKER pmkOther;
	LPMONIKER pmkPrefix;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, CommonPrefixWith));

	*ppmkPrefixA = NULL;

	hResult = WrapIMonikerWFromA(pmkOtherA, &pmkOther);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->CommonPrefixWith(pmkOther, &pmkPrefix);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerAFromW(pmkPrefix, ppmkPrefixA);

	if (pmkPrefix)
		pmkPrefix->Release();

Error:
	if (pmkOther)
		pmkOther->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::RelativePathTo, public
//
//  Synopsis:   Thunks RelativePathTo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::RelativePathTo(LPMONIKERA pmkOtherA,
		LPMONIKERA * ppmkRelPathA)
{
	TraceMethodEnter("CMonikerA::RelativePathTo", this);

	LPMONIKER pmkOther;
	LPMONIKER pmkRelPath;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, RelativePathTo));

	*ppmkRelPathA = NULL;

	hResult = WrapIMonikerWFromA(pmkOtherA, &pmkOther);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->RelativePathTo(pmkOther, &pmkRelPath);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerAFromW(pmkRelPath, ppmkRelPathA);

	if (pmkRelPath)
		pmkRelPath->Release();

Error:
	pmkOther->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::GetDisplayName, public
//
//  Synopsis:   Thunks GetDisplayName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::GetDisplayName(LPBCA pbcA, LPMONIKERA pmkToLeftA,
		LPSTR * lplpszDisplayNameA)
{
	TraceMethodEnter("CMonikerA::GetDisplayName", this);

	LPBC pbc;
	LPMONIKER pmkToLeft;
	LPOLESTR lpszDisplayName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, GetDisplayName));

	*lplpszDisplayNameA = NULL;

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkToLeftA, &pmkToLeft);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->GetDisplayName(pbc, pmkToLeft, &lpszDisplayName);
	if (FAILED(hResult))
		goto Error1;

	hResult = ConvertStringToA(lpszDisplayName, lplpszDisplayNameA);

	ConvertStringFree(lpszDisplayName);

Error1:
	if (pmkToLeft)
		pmkToLeft->Release();

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::ParseDisplayName, public
//
//  Synopsis:   Thunks ParseDisplayName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::ParseDisplayName(LPBCA pbcA, LPMONIKERA pmkToLeftA,
		LPSTR lpszDisplayNameA, ULONG * pchEaten, LPMONIKERA * ppmkOutA)
{
	TraceMethodEnter("CMonikerA::ParseDisplayName", this);

	LPBC pbc;
	LPMONIKER pmkToLeft;
	LPOLESTR lpszDisplayName;
	LPMONIKER pmkOut;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, ParseDisplayName));

	*ppmkOutA = NULL;

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkToLeftA, &pmkToLeft);
	if (FAILED(hResult))
		goto Error;

	hResult = ConvertStringToW(lpszDisplayNameA, &lpszDisplayName);
	if (FAILED(hResult))
		goto Error1;

	hResult = GetWide()->ParseDisplayName(pbc, pmkToLeft, lpszDisplayName,
			pchEaten, &pmkOut);
	if (FAILED(hResult))
		goto Error2;

	hResult = WrapIMonikerAFromW(pmkOut, ppmkOutA);

	if (pmkOut)
		pmkOut->Release();

Error2:
	ConvertStringFree(lpszDisplayName);

Error1:
	if (pmkToLeft)
		pmkToLeft->Release();

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CMonikerA::IsSystemMoniker, public
//
//  Synopsis:   Thunks IsSystemMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CMonikerA::IsSystemMoniker(LPDWORD pdwMksys)
{
	_DebugHook(GetWide(), MEMBER_PTR(IMoniker, IsSystemMoniker));

	return GetWide()->IsSystemMoniker(pdwMksys);
}



//***************************************************************************
//
//                   IRunningObjectTableA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableA::Register, public
//
//  Synopsis:   Thunks Register to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableA::Register(DWORD grfFlags,
		LPUNKNOWN punkObjectA, LPMONIKERA pmkObjectNameA, DWORD * pdwRegister)
{
	TraceMethodEnter("CRunningObjectTableA::Register", this);

	LPMONIKER pmkObjectName;
	LPUNKNOWN punk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IRunningObjectTable, Register));

	hResult = WrapIMonikerWFromA(pmkObjectNameA, &pmkObjectName);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIUnknownWFromA(punkObjectA, &punk);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->Register(grfFlags, punk, pmkObjectName,
			pdwRegister);

	punk->Release();

Error:
	if (pmkObjectName)
		pmkObjectName->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableA::Revoke, public
//
//  Synopsis:   Thunks Revoke to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableA::Revoke(DWORD dwRegister)
{
	TraceMethodEnter("CRunningObjectTableA::Revoke", this);

	_DebugHook(GetWide(), MEMBER_PTR(IRunningObjectTable, Revoke));

	return GetWide()->Revoke(dwRegister);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableA::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableA::IsRunning(LPMONIKERA pmkObjectNameA)
{
	TraceMethodEnter("CRunningObjectTableA::IsRunning", this);

	LPMONIKER pmkObjectName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IRunningObjectTable, IsRunning));

	hResult = WrapIMonikerWFromA(pmkObjectNameA, &pmkObjectName);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->IsRunning(pmkObjectName);

	if (pmkObjectName)
		pmkObjectName->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableA::GetObject, public
//
//  Synopsis:   Thunks GetObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableA::GetObject(LPMONIKERA pmkObjectNameA,
		LPUNKNOWN * ppunkObjectA)
{
	TraceMethodEnter("CRunningObjectTableA::GetObject", this);

	LPMONIKER pmkObjectName;
	LPUNKNOWN punk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IRunningObjectTable, GetObject));

	*ppunkObjectA = NULL;

	hResult = WrapIMonikerWFromA(pmkObjectNameA, &pmkObjectName);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->GetObject(pmkObjectName, &punk);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIUnknownAFromW(punk, ppunkObjectA);
	punk->Release();

Error:
	if (pmkObjectName)
		pmkObjectName->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableA::NoteChangeTime, public
//
//  Synopsis:   Thunks NoteChangeTime to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableA::NoteChangeTime(DWORD dwRegister,
		FILETIME * pfiletime)
{
	_DebugHook(GetWide(), MEMBER_PTR(IRunningObjectTable, NoteChangeTime));

	return GetWide()->NoteChangeTime(dwRegister, pfiletime);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableA::GetTimeOfLastChange, public
//
//  Synopsis:   Thunks GetTimeOfLastChange to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableA::GetTimeOfLastChange(
		LPMONIKERA pmkObjectNameA, FILETIME * pfiletime)
{
	TraceMethodEnter("CRunningObjectTableA::GetTimeOfLastChange", this);

	LPMONIKER pmkObjectName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IRunningObjectTable, GetTimeOfLastChange));

	hResult = WrapIMonikerWFromA(pmkObjectNameA, &pmkObjectName);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->GetTimeOfLastChange(pmkObjectName, pfiletime);

	if (pmkObjectName)
		pmkObjectName->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunningObjectTableA::EnumRunning, public
//
//  Synopsis:   Thunks EnumRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunningObjectTableA::EnumRunning(
		LPENUMMONIKERA * ppenumMonikerA)
{
	TraceMethodEnter("CRunningObjectTableA::EnumRunning", this);

	LPENUMMONIKER penumMoniker;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IRunningObjectTable, EnumRunning));

	*ppenumMonikerA = NULL;

	hResult = GetWide()->EnumRunning(&penumMoniker);
	if (FAILED(hResult))
		return hResult;

	if (penumMoniker)
	{
		hResult = WrapIEnumMonikerAFromW(penumMoniker, ppenumMonikerA);
		penumMoniker->Release();
	}

	return hResult;
}



//***************************************************************************
//
//                   IEnumMonikerA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerA::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerA::Next(
		ULONG celt,
		LPMONIKERA * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumMonikerA::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumMoniker, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetWide()->Next(celt, (LPMONIKER *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return hReturn;

	hResult = ConvertMonikerArrayToA(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return hResult;

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerA::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerA::Skip(ULONG celt)
{
	_DebugHook(GetWide(), MEMBER_PTR(IEnumMoniker, Skip));

	return GetWide()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerA::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerA::Reset(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IEnumMoniker, Reset));

	return GetWide()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumMonikerA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumMonikerA::Clone(IEnumMonikerA * * ppenmA)
{
	TraceMethodEnter("CEnumMonikerA::Clone", this);

	IEnumMoniker * penm;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumMoniker, Clone));

	*ppenmA = NULL;

	HRESULT hResult = GetWide()->Clone(&penm);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumMonikerAFromW(penm, ppenmA);

	if (penm)
		penm->Release();

	return hResult;
}



//***************************************************************************
//
//                          Moniker API Thunks.
//
//***************************************************************************


//+--------------------------------------------------------------------------
//
//  Routine:    BindMonikerA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI BindMonikerA(LPMONIKERA pmkA, DWORD grfOpt, REFIID iidResult,
		LPVOID * ppvResult)
{
	TraceSTDAPIEnter("BindMonikerA");
	LPMONIKER pmk;
	LPUNKNOWN punk;
	HRESULT hResult;


	*ppvResult = NULL;

	hResult = WrapIMonikerWFromA(pmkA, &pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = BindMoniker(pmk, grfOpt, iidResult, (LPVOID *)&punk);
	if (FAILED(hResult))
		goto Error;

	if (punk)
	{
		IDINTERFACE idRef = WrapTranslateIID(iidResult);

		hResult = WrapInterfaceAFromW(idRef, punk, (LPUNKNOWN *)ppvResult);

		punk->Release();
	}

Error:
	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    MkParseDisplayNameA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI MkParseDisplayNameA(LPBCA pbcA, LPSTR szUserNameA, ULONG * pchEaten,
		LPMONIKERA * ppmkA)
{
	TraceSTDAPIEnter("MkParseDisplayNameA");
	LPBC  pbc;
	LPOLESTR lpszUserName;
	LPMONIKER pmk;
	HRESULT hResult;


	*ppmkA = NULL;

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertStringToW(szUserNameA, &lpszUserName);
	if (FAILED(hResult))
		goto Error;

	hResult = MkParseDisplayName(pbc, lpszUserName, pchEaten, &pmk);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

Error1:
	ConvertStringFree(lpszUserName);

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    MonikerRelativePathToA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI MonikerRelativePathToA(LPMONIKERA pmkSrcA, LPMONIKERA pmkDestA,
		LPMONIKERA * ppmkRelPathA, BOOL fCalledFromMethod)
{
	TraceSTDAPIEnter("MonikerRelativePathToA");
	LPMONIKER pmkSrc;
	LPMONIKER pmkDest;
	LPMONIKER pmkRelPath;
	HRESULT hReturn;
	HRESULT hResult;


	*ppmkRelPathA = NULL;

	hResult = WrapIMonikerWFromA(pmkSrcA, &pmkSrc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkDestA, &pmkDest);
	if (FAILED(hResult))
		goto Error;

	hReturn = MonikerRelativePathTo(pmkSrc, pmkDest, &pmkRelPath, fCalledFromMethod);
	if (FAILED(hReturn))
		goto Error1;

	hResult = WrapIMonikerAFromW(pmkRelPath, ppmkRelPathA);

	if (pmkRelPath)
		pmkRelPath->Release();

Error1:
	if (pmkDest)
		pmkDest->Release();

Error:
	if (pmkSrc)
		pmkSrc->Release();

	if (FAILED(hResult))
		hReturn = hResult;

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Routine:    MonikerCommonPrefixWithA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI MonikerCommonPrefixWithA(LPMONIKERA pmkThisA, LPMONIKERA pmkOtherA,
		LPMONIKERA * ppmkCommonA)
{
	TraceSTDAPIEnter("MonikerCommonPrefixWithA");
	LPMONIKER pmkThis;
	LPMONIKER pmkOther;
	LPMONIKER pmkCommon;
	HRESULT hReturn;
	HRESULT hResult;


	*ppmkCommonA = NULL;

	hResult = WrapIMonikerWFromA(pmkThisA, &pmkThis);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkOtherA, &pmkOther);
	if (FAILED(hResult))
		goto Error;

	hReturn = MonikerCommonPrefixWith(pmkThis, pmkOther, &pmkCommon);
	if (FAILED(hReturn))
		goto Error1;

	hResult = WrapIMonikerAFromW(pmkCommon, ppmkCommonA);

	if (pmkCommon)
		pmkCommon->Release();

Error1:
	if (pmkOther)
		pmkOther->Release();

Error:
	if (pmkThis)
		pmkThis->Release();

	if (FAILED(hResult))
		hReturn = hResult;

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateBindCtxA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateBindCtxA(DWORD reserved, LPBCA * ppbcA)
{
	TraceSTDAPIEnter("CreateBindCtxA");
	LPBC pbc;
	HRESULT hResult;


	*ppbcA = NULL;

	hResult = CreateBindCtx(reserved, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIBindCtxAFromW(pbc, ppbcA);

	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateGenericCompositeA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateGenericCompositeA(LPMONIKERA pmkFirstA, LPMONIKERA pmkRestA,
		LPMONIKERA * ppmkCompositeA)
{
	TraceSTDAPIEnter("CreateGenericCompositeA");
	LPMONIKER pmkFirst;
	LPMONIKER pmkRest;
	LPMONIKER pmkComposite;
	HRESULT hResult;


	*ppmkCompositeA = NULL;

	hResult = WrapIMonikerWFromA(pmkFirstA, &pmkFirst);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerWFromA(pmkRestA, &pmkRest);
	if (FAILED(hResult))
		goto Error;

	hResult = CreateGenericComposite(pmkFirst, pmkRest, &pmkComposite);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIMonikerAFromW(pmkComposite, ppmkCompositeA);

	if (pmkComposite)
		pmkComposite->Release();

Error1:
	if (pmkRest)
		pmkRest->Release();

Error:
	if (pmkFirst)
		pmkFirst->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    GetClassFileA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI GetClassFileA(LPCSTR szFilenameA, CLSID * pclsid)
{
	TraceSTDAPIEnter("GetClassFileA");
	LPOLESTR lpszFilename;
	HRESULT hResult;


	hResult = ConvertStringToW(szFilenameA, &lpszFilename);
	if (FAILED(hResult))
		return hResult;

	hResult = GetClassFile(lpszFilename, pclsid);

	ConvertStringFree(lpszFilename);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateFileMonikerA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateFileMonikerA(LPSTR lpszPathNameA, LPMONIKERA * ppmkA)
{
	TraceSTDAPIEnter("CreateFileMonikerA");
	LPOLESTR lpszPathName;
	LPMONIKER pmk;
	HRESULT hReturn;
	HRESULT hResult;


	*ppmkA = NULL;

	hResult = ConvertStringToW(lpszPathNameA, &lpszPathName);
	if (FAILED(hResult))
		return hResult;

	hReturn = CreateFileMoniker(lpszPathName, &pmk);
	if (FAILED(hReturn))
		goto Error;

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

Error:
	ConvertStringFree(lpszPathName);

	if (FAILED(hResult))
		hReturn = hResult;

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateItemMonikerA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateItemMonikerA(LPSTR lpszDelimA, LPSTR lpszItemA,
		LPMONIKERA * ppmkA)
{
	TraceSTDAPIEnter("CreateItemMonikerA");
	LPOLESTR lpszDelim;
	LPOLESTR lpszItem;
	LPMONIKER pmk;
	HRESULT hResult;


	*ppmkA = NULL;

	hResult = ConvertStringToW(lpszDelimA, &lpszDelim);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertStringToW(lpszItemA, &lpszItem);
	if (FAILED(hResult))
		goto Error;

	hResult = CreateItemMoniker(lpszDelim, lpszItem, &pmk);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

Error1:
	ConvertStringFree(lpszItem);

Error:
	ConvertStringFree(lpszDelim);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreateAntiMonikerA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreateAntiMonikerA(LPMONIKERA * ppmkA)
{
	TraceSTDAPIEnter("CreateAntiMonikerA");
	LPMONIKER pmk;
	HRESULT hResult;


	*ppmkA = NULL;

	hResult = CreateAntiMoniker(&pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    CreatePointerMonikerA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI CreatePointerMonikerA(LPUNKNOWN punkA, LPMONIKERA * ppmkA)
{
	TraceSTDAPIEnter("CreatePointerMonikerA");


	LPUNKNOWN punk;
	LPMONIKER pmk;
	HRESULT hResult;


	*ppmkA = NULL;

	hResult = WrapIUnknownWFromA(punkA, &punk);
	if (FAILED(hResult))
		return hResult;

	hResult = CreatePointerMoniker(punk, &pmk);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

Error:
	punk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    GetRunningObjectTableA
//
//  Synopsis:   Creates an ANSI wrapper of an Unicode OLE2 routine.
//
//  Returns:    See OLE2 docs for details on this API.
//
//---------------------------------------------------------------------------
STDAPI GetRunningObjectTableA(DWORD reserved, LPRUNNINGOBJECTTABLEA * pprotA)
{
	TraceSTDAPIEnter("GetRunningObjectTableA");
	LPRUNNINGOBJECTTABLE prot;
	HRESULT hResult;


	*pprotA = NULL;

	hResult = GetRunningObjectTable(reserved, &prot);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIRunningObjectTableAFromW(prot, pprotA);

	if (prot)
		prot->Release();

	return hResult;
}
