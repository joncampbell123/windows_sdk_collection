//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansiole1.cpp
//
//  Contents:   ANSI Wrappers for Unicode Ole2 Interfaces and APIs.
//
//  Classes:    CDropTargetA
//              CPersistStorageA
//              CPersistFileA
//              CEnumOLEVERBA
//              COleObjectA
//              CRunnableObjectA
//
//  Functions:  IDropTargetAFromW
//              IPersistStorageAFromW
//              IPersistFileAFromW
//              IEnumOLEVERBAFromW
//              IOleObjectAFromW
//              IRunnableObjectAFromW
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IDropTargetA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetA::DragEnter, public
//
//  Synopsis:   Thunks DragEnter to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetA::DragEnter(LPDATAOBJECTA pDataObjA,
		DWORD grfKeyState, POINTL pt, LPDWORD pdwEffect)
{
	TraceMethodEnter("CDropTargetA::DragEnter", this);

	LPDATAOBJECT pDataObj;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDropTarget, DragEnter));

	hResult = WrapIDataObjectWFromA(pDataObjA, &pDataObj);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->DragEnter(pDataObj, grfKeyState, pt, pdwEffect);

	if (pDataObj)
		pDataObj->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetA::DragOver, public
//
//  Synopsis:   Thunks DragOver to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetA::DragOver(DWORD grfKeyState, POINTL pt,
		LPDWORD pdwEffect)
{
	TraceMethodEnter("CDropTargetA::DragOver", this);

	_DebugHook(GetWide(), MEMBER_PTR(IDropTarget, DragOver));

	return GetWide()->DragOver(grfKeyState, pt, pdwEffect);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetA::DragLeave, public
//
//  Synopsis:   Thunks DragLeave to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetA::DragLeave(VOID)
{
	TraceMethodEnter("CDropTargetA::DragLeave", this);

	_DebugHook(GetWide(), MEMBER_PTR(IDropTarget, DragLeave));

	return GetWide()->DragLeave();
}


//+--------------------------------------------------------------------------
//
//  Member:     CDropTargetA::Drop, public
//
//  Synopsis:   Thunks Drop to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDropTargetA::Drop(LPDATAOBJECTA pDataObjA, DWORD grfKeyState,
		POINTL pt, LPDWORD pdwEffect)
{
	TraceMethodEnter("CDropTargetA::Drop", this);

	LPDATAOBJECT pDataObj;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDropTarget, Drop));

	hResult = WrapIDataObjectWFromA(pDataObjA, &pDataObj);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Drop(pDataObj, grfKeyState, pt, pdwEffect);

	if (pDataObj)
		pDataObj->Release();

	return hResult;
}



//***************************************************************************
//
//                   IPersistStorageA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageA::GetClassID, public
//
//  Synopsis:   Thunks GetClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageA::GetClassID(LPCLSID lpClassID)
{
	TraceMethodEnter("CPersistStorageA::GetClassID", this);

	_DebugHook(GetWide(), MEMBER_PTR(IPersist, GetClassID));

	return GetWide()->GetClassID(lpClassID);
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageA::IsDirty, public
//
//  Synopsis:   Thunks IsDirty to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageA::IsDirty(VOID)
{
	TraceMethodEnter("CPersistStorageA::IsDirty", this);

	_DebugHook(GetWide(), MEMBER_PTR(IPersistStorage, IsDirty));

	return GetWide()->IsDirty();
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageA::InitNew, public
//
//  Synopsis:   Thunks InitNew to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageA::InitNew(LPSTORAGEA pStgA)
{
	TraceMethodEnter("CPersistStorageA::InitNew", this);

	LPSTORAGE pStg;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistStorage, InitNew));

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->InitNew(pStg);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStorageA::Load, public
//
//  Synopsis:   Thunks Load to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageA::Load(LPSTORAGEA pStgA)
{
	TraceMethodEnter("CPersistStorageA::Load", this);

	LPSTORAGE pStg;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistStorage, Load));

	hResult = WrapIStorageWFromA(pStgA, &pStg);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Load(pStg);

	if (pStg)
		pStg->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStreamA::Save, public
//
//  Synopsis:   Thunks Save to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageA::Save(LPSTORAGEA pStgSaveA, BOOL fSameAsLoad)
{
	TraceMethodEnter("CPersistStorageA::Save", this);

	LPSTORAGE pStgSave;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistStorage, Save));

	hResult = WrapIStorageWFromA(pStgSaveA, &pStgSave);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Save(pStgSave, fSameAsLoad);

	if (pStgSave)
		pStgSave->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStreamA::SaveCompleted, public
//
//  Synopsis:   Thunks SaveCompleted to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageA::SaveCompleted(LPSTORAGEA pStgNewA)
{
	TraceMethodEnter("CPersistStorageA::SaveCompleted", this);

	LPSTORAGE pStgNew;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistStorage, SaveCompleted));

	hResult = WrapIStorageWFromA(pStgNewA, &pStgNew);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SaveCompleted(pStgNew);

	if (pStgNew)
		pStgNew->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistStreamA::HandsOffStorage, public
//
//  Synopsis:   Thunks HandsOffStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistStorageA::HandsOffStorage(VOID)
{
	TraceMethodEnter("CPersistStorageA::HandsOffStorage", this);

	_DebugHook(GetWide(), MEMBER_PTR(IPersistStorage, HandsOffStorage));

	return GetWide()->HandsOffStorage();
}



//***************************************************************************
//
//                   IPersistFileA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileA::GetClassID, public
//
//  Synopsis:   Thunks GetClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileA::GetClassID(LPCLSID lpClassID)
{
	TraceMethodEnter("CPersistFileA::GetClassID", this);

	_DebugHook(GetWide(), MEMBER_PTR(IPersist, GetClassID));

	return GetWide()->GetClassID(lpClassID);
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileA::IsDirty, public
//
//  Synopsis:   Thunks IsDirty to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileA::IsDirty(VOID)
{
	TraceMethodEnter("CPersistFileA::IsDirty", this);

	_DebugHook(GetWide(), MEMBER_PTR(IPersistFile, IsDirty));

	return GetWide()->IsDirty();
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileA::Load, public
//
//  Synopsis:   Thunks Load to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileA::Load(LPCSTR lpszFileNameA, DWORD grfMode)
{
	TraceMethodEnter("CPersistFileA::Load", this);

	LPOLESTR lpszFileName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistFile, Load));

	hResult = ConvertStringToW(lpszFileNameA, &lpszFileName);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->Load(lpszFileName, grfMode);

	ConvertStringFree(lpszFileName);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileA::Save, public
//
//  Synopsis:   Thunks Save to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileA::Save(LPCSTR lpszFileNameA, BOOL fRemember)
{
	TraceMethodEnter("CPersistFileA::Save", this);

	LPOLESTR lpszFileName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistFile, Save));

	hResult = ConvertStringToW(lpszFileNameA, &lpszFileName);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->Save(lpszFileName, fRemember);

	ConvertStringFree(lpszFileName);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileA::SaveCompleted, public
//
//  Synopsis:   Thunks SaveCompleted to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileA::SaveCompleted(LPCSTR lpszFileNameA)
{
	TraceMethodEnter("CPersistFileA::SaveCompleted", this);

	LPOLESTR lpszFileName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistFile, SaveCompleted));

	hResult = ConvertStringToW(lpszFileNameA, &lpszFileName);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SaveCompleted(lpszFileName);

	ConvertStringFree(lpszFileName);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CPersistFileA::GetCurFile, public
//
//  Synopsis:   Thunks GetCurFile to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CPersistFileA::GetCurFile(LPSTR FAR* lplpszFileNameA)
{
	TraceMethodEnter("CPersistFileA::GetCurFile", this);

	LPOLESTR lpszFileName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IPersistFile, GetCurFile));

	hResult = GetWide()->GetCurFile(&lpszFileName);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToA(lpszFileName, lplpszFileNameA);

	ConvertStringFree(lpszFileName);

	return hResult;
}



//***************************************************************************
//
//                   IEnumOLEVERBA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBA::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBA::Next(
		ULONG celt,
		OLEVERBA * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumOLEVERBA::Next", this);
	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumOLEVERB, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetWide()->Next(celt, (OLEVERB *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertOLEVERBArrayToA(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBA::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBA::Skip(ULONG celt)
{
	TraceMethodEnter("CEnumOLEVERBA::Skip", this);

	_DebugHook(GetWide(), MEMBER_PTR(IEnumOLEVERB, Skip));

	return GetWide()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBA::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBA::Reset(VOID)
{
	TraceMethodEnter("CEnumOLEVERBA::Reset", this);

	_DebugHook(GetWide(), MEMBER_PTR(IEnumOLEVERB, Reset));

	return GetWide()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumOLEVERBA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumOLEVERBA::Clone(IEnumOLEVERBA * * ppobjA)
{
	TraceMethodEnter("CEnumOLEVERBA::Clone", this);

	IEnumOLEVERB * pobj;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumOLEVERB, Clone));

	*ppobjA = NULL;

	HRESULT hResult = GetWide()->Clone(&pobj);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumOLEVERBAFromW(pobj, ppobjA);

	if (pobj)
		pobj->Release();

	return hResult;
}



//***************************************************************************
//
//                   IOleObjectA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::SetClientSite, public
//
//  Synopsis:   Thunks SetClientSite to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::SetClientSite(LPOLECLIENTSITEA pClientSiteA)
{
	TraceMethodEnter("COleObjectA::SetClientSite", this);

	LPOLECLIENTSITE pClientSite;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, SetClientSite));

	hResult = WrapIOleClientSiteWFromA(pClientSiteA, &pClientSite);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetClientSite(pClientSite);

	if (pClientSite)
	   pClientSite->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::GetClientSite, public
//
//  Synopsis:   Thunks GetClientSite to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::GetClientSite(LPOLECLIENTSITEA * ppClientSiteA)
{
	TraceMethodEnter("COleObjectA::GetClientSite", this);

	LPOLECLIENTSITE pClientSite;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, GetClientSite));

	*ppClientSiteA = NULL;

	hResult = GetWide()->GetClientSite(&pClientSite);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIOleClientSiteAFromW(pClientSite, ppClientSiteA);

	if (pClientSite)
		pClientSite->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::SetHostNames, public
//
//  Synopsis:   Thunks SetHostNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::SetHostNames(LPCSTR szContainerAppA,
		LPCSTR szContainerObjA)
{
	TraceMethodEnter("COleObjectA::SetHostNames", this);

	LPOLESTR szContainerApp;
	LPOLESTR szContainerObj = 0;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, SetHostNames));

	hResult = ConvertStringToW(szContainerAppA, &szContainerApp);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToW(szContainerObjA, &szContainerObj);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->SetHostNames(szContainerApp, szContainerObj);

Error:
	ConvertStringFree(szContainerApp);
	ConvertStringFree(szContainerObj);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::Close, public
//
//  Synopsis:   Thunks Close to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::Close(DWORD dwSaveOption)
{
	TraceMethodEnter("COleObjectA::Close", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, Close));

	return GetWide()->Close(dwSaveOption);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::SetMoniker, public
//
//  Synopsis:   Thunks SetMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::SetMoniker(DWORD dwWhichMoniker, LPMONIKERA pmkA)
{
	TraceMethodEnter("COleObjectA::SetMoniker", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, SetMoniker));

	hResult = WrapIMonikerWFromA(pmkA, &pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SetMoniker(dwWhichMoniker, pmk);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::GetMoniker, public
//
//  Synopsis:   Thunks GetMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,
		LPMONIKERA * ppmkA)
{
	TraceMethodEnter("COleObjectA::GetMoniker", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, GetMoniker));

	*ppmkA = NULL;

	hResult = GetWide()->GetMoniker(dwAssign, dwWhichMoniker, &pmk);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::InitFromData, public
//
//  Synopsis:   Thunks InitFromData to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::InitFromData(LPDATAOBJECTA pDataObjectA,
		BOOL fCreation, DWORD dwReserved)
{
	TraceMethodEnter("COleObjectA::InitFromData", this);

	LPDATAOBJECT pDataObject;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, InitFromData));

	hResult = WrapIDataObjectWFromA(pDataObjectA, &pDataObject);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->InitFromData(pDataObject, fCreation, dwReserved);

	if (pDataObject)
		pDataObject->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::GetClipboardData, public
//
//  Synopsis:   Thunks GetClipboardData to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::GetClipboardData(DWORD dwReserved,
		LPDATAOBJECTA * ppDataObjectA)
{
	TraceMethodEnter("COleObjectA::GetClipboardData", this);

	LPDATAOBJECT pDataObject;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, GetClipboardData));

	*ppDataObjectA = NULL;

	hResult = GetWide()->GetClipboardData(dwReserved, &pDataObject);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIDataObjectAFromW(pDataObject, ppDataObjectA);

	if (pDataObject)
		pDataObject->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::DoVerb, public
//
//  Synopsis:   Thunks DoVerb to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::DoVerb(LONG iVerb, LPMSG lpmsg,
		LPOLECLIENTSITEA pActiveSiteA, LONG lindex, HWND hwndParent,
		LPCRECT lprcPosRect)
{
	TraceMethodEnter("COleObjectA::DoVerb", this);

	LPOLECLIENTSITE pActiveSite;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, DoVerb));

	hResult = WrapIOleClientSiteWFromA(pActiveSiteA, &pActiveSite);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->DoVerb(iVerb, lpmsg, pActiveSite, lindex, hwndParent,
			lprcPosRect);

	if (pActiveSite)
		pActiveSite->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::EnumVerbs, public
//
//  Synopsis:   Thunks EnumVerbs to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::EnumVerbs(LPENUMOLEVERBA * ppenumOleVerbA)
{
	TraceMethodEnter("COleObjectA::EnumVerbs", this);

	LPENUMOLEVERB penumOleVerb = NULL;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, EnumVerbs));

	hResult = GetWide()->EnumVerbs(&penumOleVerb);
	if (FAILED(hResult) || hResult == OLE_S_USEREG)
		return (hResult);

	if (penumOleVerb)
	{
		hResult = WrapIEnumOLEVERBAFromW(penumOleVerb, ppenumOleVerbA);

		penumOleVerb->Release();
	}
	else
		*ppenumOleVerbA = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::Update, public
//
//  Synopsis:   Thunks Update to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::Update(VOID)
{
	TraceMethodEnter("COleObjectA::Update", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, Update));

	return GetWide()->Update();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::IsUpToDate, public
//
//  Synopsis:   Thunks IsUpToDate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::IsUpToDate(VOID)
{
	TraceMethodEnter("COleObjectA::IsUpToDate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, IsUpToDate));

	return GetWide()->IsUpToDate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::GetUserClassID, public
//
//  Synopsis:   Thunks GetUserClassID to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::GetUserClassID(CLSID * pClsid)
{
	TraceMethodEnter("COleObjectA::GetUserClassID", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, GetUserClassID));

	return GetWide()->GetUserClassID(pClsid);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::GetUserType, public
//
//  Synopsis:   Thunks GetUserType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::GetUserType(DWORD dwFormOfType,
		LPSTR * pszUserTypeA)
{
	TraceMethodEnter("COleObjectA::GetUserType", this);

	LPOLESTR lpszUserType;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, GetUserType));

	hResult = GetWide()->GetUserType(dwFormOfType, &lpszUserType);
	if (FAILED(hResult) || hResult == OLE_S_USEREG)
		return (hResult);

	hResult = ConvertStringToA(lpszUserType, pszUserTypeA);

	ConvertStringFree(lpszUserType);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::SetExtent, public
//
//  Synopsis:   Thunks SetExtent to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::SetExtent(DWORD dwDrawAspect, LPSIZEL lpsizel)
{
	TraceMethodEnter("COleObjectA::SetExtent", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, SetExtent));

	return GetWide()->SetExtent(dwDrawAspect, lpsizel);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::GetExtent, public
//
//  Synopsis:   Thunks GetExtent to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::GetExtent(DWORD dwDrawAspect, LPSIZEL lpsizel)
{
	TraceMethodEnter("COleObjectA::GetExtent", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, GetExtent));

	return GetWide()->GetExtent(dwDrawAspect, lpsizel);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::Advise, public
//
//  Synopsis:   Thunks Advise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::Advise(LPADVISESINKA pAdvSinkA,
		DWORD * pdwConnection)
{
	TraceMethodEnter("COleObjectA::Advise", this);

	LPADVISESINK pAdvSink;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, Advise));

	hResult = WrapIAdviseSinkWFromA(pAdvSinkA, &pAdvSink);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->Advise(pAdvSink, pdwConnection);

	if (pAdvSink)
		pAdvSink->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::Unadvise, public
//
//  Synopsis:   Thunks Unadvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::Unadvise(DWORD dwConnection)
{
	TraceMethodEnter("COleObjectA::Unadvise", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, Unadvise));

	return GetWide()->Unadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::EnumAdvise, public
//
//  Synopsis:   Thunks EnumAdvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::EnumAdvise(LPENUMSTATDATAA * ppenumAdviseA)
{
	TraceMethodEnter("COleObjectA::EnumAdvise", this);

	LPENUMSTATDATA penumAdvise = NULL;
	HRESULT hResult;



	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, EnumAdvise));

	hResult = GetWide()->EnumAdvise(&penumAdvise);
	if (FAILED(hResult))
		return (hResult);

	if (penumAdvise)
	{
		hResult = WrapIEnumSTATDATAAFromW(penumAdvise, ppenumAdviseA);

		penumAdvise->Release();
	}
	else
		*ppenumAdviseA = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::GetMiscStatus, public
//
//  Synopsis:   Thunks GetMiscStatus to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::GetMiscStatus(DWORD dwAspect, DWORD * pdwStatus)
{
	TraceMethodEnter("COleObjectA::GetMiscStatus", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, GetMiscStatus));

	return GetWide()->GetMiscStatus(dwAspect, pdwStatus);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleObjectA::SetColorScheme, public
//
//  Synopsis:   Thunks SetColorScheme to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleObjectA::SetColorScheme(LPLOGPALETTE lpLogpal)
{
	TraceMethodEnter("COleObjectA::SetColorScheme", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleObject, SetColorScheme));

	return GetWide()->SetColorScheme(lpLogpal);
}



//***************************************************************************
//
//                   IOleClientSiteA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteA::SaveObject, public
//
//  Synopsis:   Thunks SaveObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteA::SaveObject(VOID)
{
	TraceMethodEnter("COleClientSiteA::SaveObject", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleClientSite, SaveObject));

	return GetWide()->SaveObject();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteA::GetMoniker, public
//
//  Synopsis:   Thunks GetMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteA::GetMoniker(DWORD dwAssign,
		DWORD dwWhichMoniker, LPMONIKERA * ppmkA)
{
	TraceMethodEnter("COleClientSiteA::GetMoniker", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleClientSite, GetMoniker));

	*ppmkA = NULL;

	hResult = GetWide()->GetMoniker(dwAssign, dwWhichMoniker, &pmk);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteA::GetContainer, public
//
//  Synopsis:   Thunks GetContainer to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteA::GetContainer(LPOLECONTAINERA * ppContainerA)
{
	TraceMethodEnter("COleClientSiteA::GetContainer", this);

	LPOLECONTAINER pContainer;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleClientSite, GetContainer));

	*ppContainerA = NULL;

	hResult = GetWide()->GetContainer(&pContainer);
	if (FAILED(hResult))
	{
		*ppContainerA = NULL;
		return (hResult);
	}

	hResult = WrapIOleContainerAFromW(pContainer, ppContainerA);

	if (pContainer)
		pContainer->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteA::ShowObject, public
//
//  Synopsis:   Thunks ShowObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteA::ShowObject(VOID)
{
	TraceMethodEnter("COleClientSiteA::ShowObject", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleClientSite, ShowObject));

	return GetWide()->ShowObject();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteA::OnShowWindow, public
//
//  Synopsis:   Thunks OnShowWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteA::OnShowWindow(BOOL fShow)
{
	TraceMethodEnter("COleClientSiteA::OnShowWindow", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleClientSite, OnShowWindow));

	return GetWide()->OnShowWindow(fShow);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleClientSiteA::RequestNewObjectLayout, public
//
//  Synopsis:   Thunks RequestNewObjectLayout to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleClientSiteA::RequestNewObjectLayout(VOID)
{
	TraceMethodEnter("COleClientSiteA::RequestNewObjectLayout", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleClientSite, RequestNewObjectLayout));

	return GetWide()->RequestNewObjectLayout();
}



//***************************************************************************
//
//                   IRunnableObjectA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectA::GetRunningClass, public
//
//  Synopsis:   Thunks GetRunningClass to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectA::GetRunningClass(LPCLSID lpClsid)
{
	_DebugHook(GetWide(), MEMBER_PTR(IRunnableObject, GetRunningClass));

	return GetWide()->GetRunningClass(lpClsid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectA::Run, public
//
//  Synopsis:   Thunks Run to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectA::Run(LPBINDCTXA pbcA)
{
	TraceMethodEnter("CRunnableObjectA::Run", this);

	LPBINDCTX pbc;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IRunnableObject, Run));

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->Run(pbc);

	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectA::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(BOOL) CRunnableObjectA::IsRunning(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IRunnableObject, IsRunning));

	return GetWide()->IsRunning();
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectA::LockRunning, public
//
//  Synopsis:   Thunks LockRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectA::LockRunning(BOOL fLock,
		BOOL fLastUnlockCloses)
{
	_DebugHook(GetWide(), MEMBER_PTR(IRunnableObject, LockRunning));

	return GetWide()->LockRunning(fLock, fLastUnlockCloses);
}


//+--------------------------------------------------------------------------
//
//  Member:     CRunnableObjectA::SetContainedObject, public
//
//  Synopsis:   Thunks SetContainedObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CRunnableObjectA::SetContainedObject(BOOL fContained)
{
	_DebugHook(GetWide(), MEMBER_PTR(IRunnableObject, SetContainedObject));

	return GetWide()->SetContainedObject(fContained);
}
