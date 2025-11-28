//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       wideole1.cpp
//
//  Contents:   Unicode Wrappers for ANSI Ole2 Interfaces and APIs.
//
//  Classes:    CDropTargetW
//              CPersistStorageW
//              CPersistFileW
//              CEnumOLEVERBW
//              COleObjectW
//              CRunnableObjectW
//
//  Functions:  IDropTargetWFromA
//              IPersistStorageWFromA
//              IPersistFileWFromA
//              IEnumOLEVERBWFromA
//              IOleObjectWFromA
//              IRunnableObjectWFromA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IDropTargetW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetW::DragEnter, public
//
//  Synopsis:   Thunks DragEnter to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetW::DragEnter(LPDATAOBJECT pDataObj,
		DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
	TraceMethodEnter("CDropTargetW::DragEnter", this);

	LPDATAOBJECTA pDataObjA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDropTargetA, DragEnter));

	hResult = WrapIDataObjectAFromW(pDataObj, &pDataObjA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->DragEnter(pDataObjA, grfKeyState, pt, pdwEffect);

	if (pDataObjA)
		pDataObjA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetW::DragOver, public
//
//  Synopsis:   Thunks DragOver to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetW::DragOver(DWORD grfKeyState, POINTL pt,
		LPDWORD pdwEffect)
{
	TraceMethodEnter("CDropTargetW::DragOver", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IDropTargetA, DragOver));

	return GetANSI()->DragOver(grfKeyState, pt, pdwEffect);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetW::DragLeave, public
//
//  Synopsis:   Thunks DragLeave to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetW::DragLeave(VOID)
{
	TraceMethodEnter("CDropTargetW::DragLeave", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IDropTargetA, DragLeave));

	return GetANSI()->DragLeave();
}


//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetW::Drop, public
//
//  Synopsis:   Thunks Drop to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetW::Drop(LPDATAOBJECT pDataObj, DWORD grfKeyState,
		POINTL pt, LPDWORD pdwEffect)
{
	TraceMethodEnter("CDropTargetW::Drop", this);

	LPDATAOBJECTA pDataObjA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDropTargetA, Drop));

	hResult = WrapIDataObjectAFromW(pDataObj, &pDataObjA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Drop(pDataObjA, grfKeyState, pt, pdwEffect);

	if (pDataObjA)
		pDataObjA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IPersistStorageW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageW::GetClassID, public
//
//  Synopsis:   Thunks GetClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageW::GetClassID(LPCLSID lpClassID)
{
	TraceMethodEnter("CPersistStorageW::GetClassID", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStorageA, GetClassID));

	return GetANSI()->GetClassID(lpClassID);
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageW::IsDirty, public
//
//  Synopsis:   Thunks IsDirty to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageW::IsDirty(VOID)
{
	TraceMethodEnter("CPersistStorageW::IsDirty", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStorageA, IsDirty));

	return GetANSI()->IsDirty();
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageW::InitNew, public
//
//  Synopsis:   Thunks InitNew to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageW::InitNew(LPSTORAGE pStg)
{
	TraceMethodEnter("CPersistStorageW::InitNew", this);

	LPSTORAGEA pStgA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStorageA, InitNew));

	hResult = WrapIStorageAFromW(pStg, &pStgA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->InitNew(pStgA);

	if (pStgA)
		pStgA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageW::Load, public
//
//  Synopsis:   Thunks Load to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageW::Load(LPSTORAGE pStg)
{
	TraceMethodEnter("CPersistStorageW::Load", this);

	LPSTORAGEA pStgA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStorageA, Load));

	hResult = WrapIStorageAFromW(pStg, &pStgA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Load(pStgA);

	if (pStgA)
		pStgA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStreamW::Save, public
//
//  Synopsis:   Thunks Save to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageW::Save(LPSTORAGE pStgSave, BOOL fSameAsLoad)
{
	TraceMethodEnter("CPersistStorageW::Save", this);

	LPSTORAGEA pStgSaveA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStorageA, Save));

	hResult = WrapIStorageAFromW(pStgSave, &pStgSaveA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Save(pStgSaveA, fSameAsLoad);

	if (pStgSaveA)
		pStgSaveA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStreamW::SaveCompleted, public
//
//  Synopsis:   Thunks SaveCompleted to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageW::SaveCompleted(LPSTORAGE pStgNew)
{
	TraceMethodEnter("CPersistStorageW::SaveCompleted", this);

	LPSTORAGEA pStgNewA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStorageA, SaveCompleted));

	hResult = WrapIStorageAFromW(pStgNew, &pStgNewA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SaveCompleted(pStgNewA);

	if (pStgNewA)
		pStgNewA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStreamW::HandsOffStorage, public
//
//  Synopsis:   Thunks HandsOffStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageW::HandsOffStorage(VOID)
{
	TraceMethodEnter("CPersistStorageW::HandsOffStorage", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStorageA, HandsOffStorage));

	return GetANSI()->HandsOffStorage();
}



//***************************************************************************
//
//                   IPersistFileW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileW::GetClassID, public
//
//  Synopsis:   Thunks GetClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileW::GetClassID(LPCLSID lpClassID)
{
	TraceMethodEnter("CPersistFileW::GetClassID", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IPersistFileA, GetClassID));

	return GetANSI()->GetClassID(lpClassID);
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileW::IsDirty, public
//
//  Synopsis:   Thunks IsDirty to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileW::IsDirty(VOID)
{
	TraceMethodEnter("CPersistFileW::IsDirty", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IPersistFileA, IsDirty));

	return GetANSI()->IsDirty();
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileW::Load, public
//
//  Synopsis:   Thunks Load to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileW::Load(LPCOLESTR lpszFileName, DWORD grfMode)
{
	TraceMethodEnter("CPersistFileW::Load", this);

	LPSTR lpszFileNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistFileA, Load));

	hResult = ConvertStringToA(lpszFileName, &lpszFileNameA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->Load(lpszFileNameA, grfMode);

	ConvertStringFree(lpszFileNameA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileW::Save, public
//
//  Synopsis:   Thunks Save to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileW::Save(LPCOLESTR lpszFileName, BOOL fRemember)
{
	TraceMethodEnter("CPersistFileW::Save", this);

	LPSTR lpszFileNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistFileA, Save));

	hResult = ConvertStringToA(lpszFileName, &lpszFileNameA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->Save(lpszFileNameA, fRemember);

	ConvertStringFree(lpszFileNameA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileW::SaveCompleted, public
//
//  Synopsis:   Thunks SaveCompleted to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileW::SaveCompleted(LPCOLESTR lpszFileName)
{
	TraceMethodEnter("CPersistFileW::SaveCompleted", this);

	LPSTR lpszFileNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistFileA, SaveCompleted));

	hResult = ConvertStringToA(lpszFileName, &lpszFileNameA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SaveCompleted(lpszFileNameA);

	ConvertStringFree(lpszFileNameA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileW::GetCurFile, public
//
//  Synopsis:   Thunks GetCurFile to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileW::GetCurFile(LPOLESTR FAR* lplpszFileName)
{
	TraceMethodEnter("CPersistFileW::GetCurFile", this);

	LPSTR lpszFileNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IPersistFileA, GetCurFile));

	hResult = GetANSI()->GetCurFile(&lpszFileNameA);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToW(lpszFileNameA, lplpszFileName);

	ConvertStringFree(lpszFileNameA);

	return hResult;
}



//***************************************************************************
//
//                   IEnumOLEVERBW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBW::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBW::Next(
		ULONG celt,
		OLEVERB * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumOLEVERBW::Next", this);

	ULONG    celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumOLEVERBA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetANSI()->Next(celt, (OLEVERBA *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertOLEVERBArrayToW(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBW::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBW::Skip(ULONG celt)
{
	TraceMethodEnter("CEnumOLEVERBW::Skip", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumOLEVERBA, Skip));

	return GetANSI()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBW::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBW::Reset(VOID)
{
	TraceMethodEnter("CEnumOLEVERBW::Reset", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumOLEVERBA, Reset));

	return GetANSI()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBW::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBW::Clone(IEnumOLEVERB * * ppobj)
{
	TraceMethodEnter("CEnumOLEVERBW::Clone", this);

	IEnumOLEVERBA * pobjA;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumOLEVERBA, Clone));

	*ppobj = NULL;

	HRESULT hResult = GetANSI()->Clone(&pobjA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumOLEVERBWFromA(pobjA, ppobj);

	if (pobjA)
		pobjA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IOleObjectW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::SetClientSite, public
//
//  Synopsis:   Thunks SetClientSite to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::SetClientSite(LPOLECLIENTSITE pClientSite)
{
	TraceMethodEnter("COleObjectW::SetClientSite", this);

	LPOLECLIENTSITEA pClientSiteA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, SetClientSite));

	hResult = WrapIOleClientSiteAFromW(pClientSite, &pClientSiteA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetClientSite(pClientSiteA);

	if (pClientSiteA)
		pClientSiteA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::GetClientSite, public
//
//  Synopsis:   Thunks GetClientSite to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::GetClientSite(LPOLECLIENTSITE * ppClientSite)
{
	TraceMethodEnter("COleObjectW::GetClientSite", this);

	LPOLECLIENTSITEA pClientSiteA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, GetClientSite));

	*ppClientSite = NULL;

	hResult = GetANSI()->GetClientSite(&pClientSiteA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, ppClientSite);

	if (pClientSiteA)
		pClientSiteA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::SetHostNames, public
//
//  Synopsis:   Thunks SetHostNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::SetHostNames(LPCOLESTR szContainerApp,
		LPCOLESTR szContainerObj)
{
	TraceMethodEnter("COleObjectW::SetHostNames", this);

	LPSTR szContainerAppA;
	LPSTR szContainerObjA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, SetHostNames));

	hResult = ConvertStringToA(szContainerApp, &szContainerAppA);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToA(szContainerObj, &szContainerObjA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->SetHostNames(szContainerAppA, szContainerObjA);

	ConvertStringFree(szContainerObjA);
Error:
	ConvertStringFree(szContainerAppA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::Close, public
//
//  Synopsis:   Thunks Close to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::Close(DWORD dwSaveOption)
{
	TraceMethodEnter("COleObjectW::Close", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, Close));

	return GetANSI()->Close(dwSaveOption);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::SetMoniker, public
//
//  Synopsis:   Thunks SetMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::SetMoniker(DWORD dwWhichMoniker, LPMONIKER pmk)
{
	TraceMethodEnter("COleObjectW::SetMoniker", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, SetMoniker));

	hResult = WrapIMonikerAFromW(pmk, &pmkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SetMoniker(dwWhichMoniker, pmkA);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::GetMoniker, public
//
//  Synopsis:   Thunks GetMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
		LPMONIKER * ppmk)
{
	TraceMethodEnter("COleObjectW::GetMoniker", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, GetMoniker));

	*ppmk = NULL;

	hResult = GetANSI()->GetMoniker(dwAssign, dwWhichMoniker, &pmkA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIMonikerWFromA(pmkA, ppmk);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::InitFromData, public
//
//  Synopsis:   Thunks InitFromData to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::InitFromData(LPDATAOBJECT pDataObject,
		BOOL fCreation, DWORD dwReserved)
{
	TraceMethodEnter("COleObjectW::InitFromData", this);

	LPDATAOBJECTA pDataObjectA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, InitFromData));

	hResult = WrapIDataObjectAFromW(pDataObject, &pDataObjectA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->InitFromData(pDataObjectA, fCreation, dwReserved);

	if (pDataObjectA)
		pDataObjectA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::GetClipboardData, public
//
//  Synopsis:   Thunks GetClipboardData to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::GetClipboardData(DWORD dwReserved,
		LPDATAOBJECT * ppDataObject)
{
	TraceMethodEnter("COleObjectW::GetClipboardData", this);

	LPDATAOBJECTA pDataObjectA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, GetClipboardData));

	*ppDataObject = NULL;

	hResult = GetANSI()->GetClipboardData(dwReserved, &pDataObjectA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIDataObjectWFromA(pDataObjectA, ppDataObject);

	if (pDataObjectA)
		pDataObjectA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::DoVerb, public
//
//  Synopsis:   Thunks DoVerb to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::DoVerb(LONG iVerb, LPMSG lpmsg,
		LPOLECLIENTSITE pActiveSite, LONG lindex, HWND hwndParent,
		LPCRECT lprcPosRect)
{
	TraceMethodEnter("COleObjectW::DoVerb", this);

	LPOLECLIENTSITEA pActiveSiteA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, DoVerb));

	hResult = WrapIOleClientSiteAFromW(pActiveSite, &pActiveSiteA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->DoVerb(iVerb, lpmsg, pActiveSiteA, lindex, hwndParent,
			lprcPosRect);

	if (pActiveSiteA)
		pActiveSiteA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::EnumVerbs, public
//
//  Synopsis:   Thunks EnumVerbs to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::EnumVerbs(LPENUMOLEVERB * ppEnumOLEVERB)
{
	TraceMethodEnter("COleObjectW::EnumVerbs", this);

	LPENUMOLEVERBA penumOLEVERBA = NULL;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, EnumVerbs));

	hResult = GetANSI()->EnumVerbs(&penumOLEVERBA);
	if (FAILED(hResult) || hResult == OLE_S_USEREG)
		return (hResult);

	if (penumOLEVERBA)
	{
		hResult = WrapIEnumOLEVERBWFromA(penumOLEVERBA, ppEnumOLEVERB);
		penumOLEVERBA->Release();
	}
	else
		*ppEnumOLEVERB = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::Update, public
//
//  Synopsis:   Thunks Update to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::Update(VOID)
{
	TraceMethodEnter("COleObjectW::Update", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, Update));

	return GetANSI()->Update();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::IsUpToDate, public
//
//  Synopsis:   Thunks IsUpToDate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::IsUpToDate(VOID)
{
	TraceMethodEnter("COleObjectW::IsUpToDate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, IsUpToDate));

	return GetANSI()->IsUpToDate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::GetUserClassID, public
//
//  Synopsis:   Thunks GetUserClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::GetUserClassID(CLSID * pClsid)
{
	TraceMethodEnter("COleObjectW::GetUserClassID", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, GetUserClassID));

	return GetANSI()->GetUserClassID(pClsid);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::GetUserType, public
//
//  Synopsis:   Thunks GetUserType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::GetUserType(DWORD dwFormOfType,
		LPOLESTR * pszUserType)
{
	TraceMethodEnter("COleObjectW::GetUserType", this);

	LPSTR lpszUserTypeA = NULL;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, GetUserType));

	hResult = GetANSI()->GetUserType(dwFormOfType, &lpszUserTypeA);
	if (FAILED(hResult) || hResult == OLE_S_USEREG)
		return (hResult);

	if (lpszUserTypeA)
	{
		hResult = ConvertStringToW(lpszUserTypeA, pszUserType);

		ConvertStringFree(lpszUserTypeA);
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::SetExtent, public
//
//  Synopsis:   Thunks SetExtent to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::SetExtent(DWORD dwDrawAspect, LPSIZEL lpsizel)
{
	TraceMethodEnter("COleObjectW::SetExtent", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, SetExtent));

	return GetANSI()->SetExtent(dwDrawAspect, lpsizel);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::GetExtent, public
//
//  Synopsis:   Thunks GetExtent to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::GetExtent(DWORD dwDrawAspect, LPSIZEL lpsizel)
{
	TraceMethodEnter("COleObjectW::GetExtent", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, GetExtent));

	return GetANSI()->GetExtent(dwDrawAspect, lpsizel);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::Advise, public
//
//  Synopsis:   Thunks Advise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::Advise(LPADVISESINK pAdvSink,
		DWORD * pdwConnection)
{
	TraceMethodEnter("COleObjectW::Advise", this);

	LPADVISESINKA pAdvSinkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, Advise));

	hResult = WrapIAdviseSinkAFromW(pAdvSink, &pAdvSinkA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->Advise(pAdvSinkA, pdwConnection);

	if (pAdvSinkA)
		pAdvSinkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::Unadvise, public
//
//  Synopsis:   Thunks Unadvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::Unadvise(DWORD dwConnection)
{
	TraceMethodEnter("COleObjectW::Unadvise", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, Unadvise));

	return GetANSI()->Unadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::EnumAdvise, public
//
//  Synopsis:   Thunks EnumAdvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::EnumAdvise(LPENUMSTATDATA * ppenumAdvise)
{
	TraceMethodEnter("COleObjectW::EnumAdvise", this);

	LPENUMSTATDATAA penumAdviseA = NULL;
	HRESULT hResult;



	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, EnumAdvise));

	hResult = GetANSI()->EnumAdvise(&penumAdviseA);
	if (FAILED(hResult))
		return (hResult);

	if (penumAdviseA)
	{
		hResult = WrapIEnumSTATDATAWFromA(penumAdviseA, ppenumAdvise);
		penumAdviseA->Release();
	}
	else
		*ppenumAdvise = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::GetMiscStatus, public
//
//  Synopsis:   Thunks GetMiscStatus to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::GetMiscStatus(DWORD dwAspect, DWORD * pdwStatus)
{
	TraceMethodEnter("COleObjectW::GetMiscStatus", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, GetMiscStatus));

	return GetANSI()->GetMiscStatus(dwAspect, pdwStatus);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectW::SetColorScheme, public
//
//  Synopsis:   Thunks SetColorScheme to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectW::SetColorScheme(LPLOGPALETTE lpLogpal)
{
	TraceMethodEnter("COleObjectW::SetColorScheme", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleObjectA, SetColorScheme));

	return GetANSI()->SetColorScheme(lpLogpal);
}



//***************************************************************************
//
//                   IOleClientSiteW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteW::SaveObject, public
//
//  Synopsis:   Thunks SaveObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteW::SaveObject(VOID)
{
	TraceMethodEnter("COleClientSiteW::SaveObject", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleClientSiteA, SaveObject));

	return GetANSI()->SaveObject();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteW::GetMoniker, public
//
//  Synopsis:   Thunks GetMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteW::GetMoniker(DWORD dwAssign,
		DWORD dwWhichMoniker, LPMONIKER * ppmk)
{
	TraceMethodEnter("COleClientSiteW::GetMoniker", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleClientSiteA, GetMoniker));

	*ppmk = NULL;

	hResult = GetANSI()->GetMoniker(dwAssign, dwWhichMoniker, &pmkA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIMonikerWFromA(pmkA, ppmk);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteW::GetContainer, public
//
//  Synopsis:   Thunks GetContainer to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteW::GetContainer(LPOLECONTAINER * ppContainer)
{
	TraceMethodEnter("COleClientSiteW::GetContainer", this);

	LPOLECONTAINERA pContainerA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleClientSiteA, GetContainer));

	*ppContainer = NULL;

	hResult = GetANSI()->GetContainer(&pContainerA);
	if (FAILED(hResult))
	{
		*ppContainer = NULL;
		return (hResult);
	}

	hResult = WrapIOleContainerWFromA(pContainerA, ppContainer);

	if (pContainerA)
		pContainerA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteW::ShowObject, public
//
//  Synopsis:   Thunks ShowObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteW::ShowObject(VOID)
{
	TraceMethodEnter("COleClientSiteW::ShowObject", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleClientSiteA, ShowObject));

	return GetANSI()->ShowObject();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteW::OnShowWindow, public
//
//  Synopsis:   Thunks OnShowWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteW::OnShowWindow(BOOL fShow)
{
	TraceMethodEnter("COleClientSiteW::OnShowWindow", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleClientSiteA, OnShowWindow));

	return GetANSI()->OnShowWindow(fShow);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteW::RequestNewObjectLayout, public
//
//  Synopsis:   Thunks RequestNewObjectLayout to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteW::RequestNewObjectLayout(VOID)
{
	TraceMethodEnter("COleClientSiteW::RequestNewObjectLayout", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleClientSiteA, RequestNewObjectLayout));

	return GetANSI()->RequestNewObjectLayout();
}



//***************************************************************************
//
//                   IRunnableObjectW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectW::GetRunningClass, public
//
//  Synopsis:   Thunks GetRunningClass to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectW::GetRunningClass(LPCLSID lpClsid)
{
	TraceMethodEnter("CRunnableObjectW::GetRunningClass", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IRunnableObjectA, GetRunningClass));

	return GetANSI()->GetRunningClass(lpClsid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectW::Run, public
//
//  Synopsis:   Thunks Run to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectW::Run(LPBINDCTX pbc)
{
	TraceMethodEnter("CRunnableObjectW::Run", this);

	LPBINDCTXA pbcA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IRunnableObjectA, Run));

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->Run(pbcA);

	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectW::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(BOOL) CRunnableObjectW::IsRunning(VOID)
{
	TraceMethodEnter("CRunnableObjectW::IsRunning", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IRunnableObjectA, IsRunning));

	return GetANSI()->IsRunning();
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectW::LockRunning, public
//
//  Synopsis:   Thunks LockRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectW::LockRunning(BOOL fLock,
		BOOL fLastUnlockCloses)
{
	TraceMethodEnter("CRunnableObjectW::LockRunning", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IRunnableObjectA, LockRunning));

	return GetANSI()->LockRunning(fLock, fLastUnlockCloses);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectW::SetContainedObject, public
//
//  Synopsis:   Thunks SetContainedObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectW::SetContainedObject(BOOL fContained)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IRunnableObjectA, SetContainedObject));

	return GetANSI()->SetContainedObject(fContained);
}
