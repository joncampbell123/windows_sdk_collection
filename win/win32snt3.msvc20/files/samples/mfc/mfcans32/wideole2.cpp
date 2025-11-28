//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       wideole1.cpp
//
//  Contents:   Unicode Wrappers for ANSI Ole2 Interfaces and APIs.
//
//  Classes:    COleItemContainerW
//              COleAdviseHolderW
//              COleLinkW
//              COleInPlaceObjectW
//              COleInPlaceActiveObjectW
//              COleInPlaceFrameW
//              COleInPlaceSiteW
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IOleItemContainerW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerW::ParseDisplayNameA, public
//
//  Synopsis:   Thunks ParseDisplayNameA to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerW::ParseDisplayName(LPBC pbc,
		LPOLESTR lpszDisplayName, ULONG * pchEaten, LPMONIKER * ppmkOut)
{
	TraceMethodEnter("COleItemContainerW::ParseDisplayName", this);

	LPBCA pbcA;
	LPSTR lpszDisplayNameA;
	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleItemContainerA, ParseDisplayName));

	*ppmkOut = NULL;

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToA(lpszDisplayName, &lpszDisplayNameA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->ParseDisplayName(pbcA, lpszDisplayNameA, pchEaten, &pmkA);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIMonikerWFromA(pmkA, ppmkOut);

	if (pmkA)
		pmkA->Release();

Error1:
	ConvertStringFree(lpszDisplayNameA);

Error:
	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerW::EnumObjects, public
//
//  Synopsis:   Thunks EnumObjects to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerW::EnumObjects(DWORD grfFlags,
		LPENUMUNKNOWN * ppenumUnknown)
{
	TraceMethodEnter("COleItemContainerW::EnumObjects", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleItemContainerA, EnumObjects));

	return GetANSI()->EnumObjects(grfFlags, ppenumUnknown);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerW::LockContainer, public
//
//  Synopsis:   Thunks LockContainer to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerW::LockContainer(BOOL fLock)
{
	TraceMethodEnter("COleItemContainerW::LockContainer", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleItemContainerA, LockContainer));

	return GetANSI()->LockContainer(fLock);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerW::GetObject, public
//
//  Synopsis:   Thunks GetObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerW::GetObject(LPOLESTR lpszItem,
		DWORD dwSpeedNeeded, LPBINDCTX pbc, REFIID riid,
		LPVOID * ppvObject)
{
	TraceMethodEnter("COleItemContainerW::GetObject", this);

	LPSTR lpszItemA;
	LPBINDCTXA pbcA;
	LPUNKNOWN  punkA;
	IDINTERFACE idRef;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleItemContainerA, GetObject));

	hResult = ConvertStringToA(lpszItem, &lpszItemA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->GetObject(lpszItemA, dwSpeedNeeded, pbcA, riid,
			(LPVOID *)&punkA);
	if (FAILED(hResult))
		goto Error1;

	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceWFromA(idRef, punkA, (LPUNKNOWN *)ppvObject);

	if (punkA)
		punkA->Release();

Error1:
	if (pbcA)
		pbcA->Release();

Error:
	ConvertStringFree(lpszItemA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerW::GetObjectStorage, public
//
//  Synopsis:   Thunks GetObjectStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerW::GetObjectStorage(LPOLESTR lpszItem,
		LPBINDCTX pbc, REFIID riid, LPVOID * ppvStorage)
{
	TraceMethodEnter("COleItemContainerW::GetObjectStorage", this);

	LPSTR lpszItemA;
	LPBINDCTXA  pbcA;
	LPUNKNOWN   punkA;
	IDINTERFACE idRef;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleItemContainerA, GetObjectStorage));

	hResult = ConvertStringToA(lpszItem, &lpszItemA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->GetObjectStorage(lpszItemA, pbcA, riid, (LPVOID *)&punkA);
	if (FAILED(hResult))
		goto Error1;

	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceWFromA(idRef, punkA, (LPUNKNOWN *)ppvStorage);

	if (punkA)
		punkA->Release();

Error1:
	if (pbcA)
		pbcA->Release();

Error:
	ConvertStringFree(lpszItemA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerW::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerW::IsRunning(LPOLESTR lpszItem)
{
	TraceMethodEnter("COleItemContainerW::IsRunning", this);

	LPSTR lpszItemA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleItemContainerA, IsRunning));

	hResult = ConvertStringToA(lpszItem, &lpszItemA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->IsRunning(lpszItemA);

	ConvertStringFree(lpszItemA);

	return hResult;
}



//***************************************************************************
//
//                   IOleAdviseHolderW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderW::Advise, public
//
//  Synopsis:   Thunks Advise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderW::Advise(LPADVISESINK pAdvise,
		DWORD * pdwConnection)
{
	TraceMethodEnter("COleAdviseHolderW::Advise", this);

	LPADVISESINKA pAdviseA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleAdviseHolderA, Advise));

	hResult = WrapIAdviseSinkAFromW(pAdvise, &pAdviseA);
	if (FAILED(hResult))
		return hResult;

	hResult =  GetANSI()->Advise(pAdviseA, pdwConnection);

	if (pAdviseA)
		pAdviseA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderW::Unadvise, public
//
//  Synopsis:   Thunks Unadvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderW::Unadvise(DWORD dwConnection)
{
	TraceMethodEnter("COleAdviseHolderW::Unadvise", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleAdviseHolderA, Unadvise));

	return GetANSI()->Unadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderW::EnumAdvise, public
//
//  Synopsis:   Thunks EnumAdvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderW::EnumAdvise(LPENUMSTATDATA * ppenumAdvise)
{
	TraceMethodEnter("COleAdviseHolderW::EnumAdvise", this);

	LPENUMSTATDATAA penumAdviseA = NULL;
	HRESULT hResult;



	_DebugHook(GetANSI(), MEMBER_PTR(IOleAdviseHolderA, EnumAdvise));

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
//  Member:     COleAdviseHolderW::SendOnReNameA, public
//
//  Synopsis:   Thunks SendOnReNameA to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderW::SendOnRename(LPMONIKER pmk)
{
	TraceMethodEnter("COleAdviseHolderW::SendOnRename", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleAdviseHolderA, SendOnRename));

	hResult = WrapIMonikerAFromW(pmk, &pmkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SendOnRename(pmkA);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderW::SendOnSave, public
//
//  Synopsis:   Thunks SendOnSave to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderW::SendOnSave(VOID)
{
	TraceMethodEnter("COleAdviseHolderW::SendOnSave", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleAdviseHolderA, SendOnSave));

	return GetANSI()->SendOnSave();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderW::SendOnClose, public
//
//  Synopsis:   Thunks SendOnClose to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderW::SendOnClose(VOID)
{
	TraceMethodEnter("COleAdviseHolderW::SendOnClose", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleAdviseHolderA, SendOnClose));

	return GetANSI()->SendOnClose();
}



//***************************************************************************
//
//                   IOleLinkW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::SetUpdateOptions, public
//
//  Synopsis:   Thunks SetUpdateOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::SetUpdateOptions(DWORD dwUpdateOpt)
{
	TraceMethodEnter("COleLinkW::SetUpdateOptions", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, SetUpdateOptions));

	return GetANSI()->SetUpdateOptions(dwUpdateOpt);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::GetUpdateOptions, public
//
//  Synopsis:   Thunks GetUpdateOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::GetUpdateOptions(LPDWORD pdwUpdateOpt)
{
	TraceMethodEnter("COleLinkW::GetUpdateOptions", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, GetUpdateOptions));

	return GetANSI()->GetUpdateOptions(pdwUpdateOpt);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::SetSourceMoniker, public
//
//  Synopsis:   Thunks SetSourceMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::SetSourceMoniker(LPMONIKER pmk, REFCLSID rclsid)
{
	TraceMethodEnter("COleLinkW::SetSourceMoniker", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, SetSourceMoniker));

	hResult = WrapIMonikerAFromW(pmk, &pmkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SetSourceMoniker(pmkA, rclsid);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::GetSourceMoniker, public
//
//  Synopsis:   Thunks GetSourceMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::GetSourceMoniker(LPMONIKER * ppmk)
{
	TraceMethodEnter("COleLinkW::GetSourceMoniker", this);

	LPMONIKERA pmkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, GetSourceMoniker));

	*ppmk = NULL;

	hResult = GetANSI()->GetSourceMoniker(&pmkA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIMonikerWFromA(pmkA, ppmk);

	if (pmkA)
		pmkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::SetSourceDisplayNameA, public
//
//  Synopsis:   Thunks SetSourceDisplayNameA to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::SetSourceDisplayName(LPCOLESTR lpszDisplayName)
{
	TraceMethodEnter("COleLinkW::SetSourceDisplayName", this);

	LPSTR lpszDisplayNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, SetSourceDisplayName));

	hResult = ConvertStringToA(lpszDisplayName, &lpszDisplayNameA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetSourceDisplayName(lpszDisplayNameA);

	ConvertStringFree(lpszDisplayNameA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::GetSourceDisplayNameA, public
//
//  Synopsis:   Thunks GetSourceDisplayNameA to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::GetSourceDisplayName(LPOLESTR * lplpszDisplayName)
{
	TraceMethodEnter("COleLinkW::GetSourceDisplayName", this);

	LPSTR lpszDisplayNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, GetSourceDisplayName));

	hResult = GetANSI()->GetSourceDisplayName(&lpszDisplayNameA);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToW(lpszDisplayNameA, lplpszDisplayName);

	ConvertStringFree(lpszDisplayNameA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::BindToSource, public
//
//  Synopsis:   Thunks BindToSource to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::BindToSource(DWORD bindflags, LPBINDCTX pbc)
{
	TraceMethodEnter("COleLinkW::BindToSource", this);

	LPBINDCTXA pbcA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, BindToSource));

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->BindToSource(bindflags, pbcA);

	if (pbcA)
		pbcA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::BindIfRunning, public
//
//  Synopsis:   Thunks BindIfRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::BindIfRunning(VOID)
{
	TraceMethodEnter("COleLinkW::BindIfRunning", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, BindIfRunning));

	return GetANSI()->BindIfRunning();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::GetBoundSource, public
//
//  Synopsis:   Thunks GetBoundSource to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::GetBoundSource(LPUNKNOWN * ppUnk)
{
	TraceMethodEnter("COleLinkW::GetBoundSource", this);

	LPUNKNOWN punkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, GetBoundSource));

	hResult = GetANSI()->GetBoundSource(&punkA);
	if (FAILED(hResult))
			return hResult;

	hResult = WrapIUnknownWFromA(punkA, ppUnk);
	punkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::UnbindSource, public
//
//  Synopsis:   Thunks UnbindSource to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::UnbindSource(VOID)
{
	TraceMethodEnter("COleLinkW::UnbindSource", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, UnbindSource));

	return GetANSI()->UnbindSource();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkW::Update, public
//
//  Synopsis:   Thunks Update to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkW::Update(LPBINDCTX pbc)
{
	TraceMethodEnter("COleLinkW::Update", this);

	LPBINDCTXA pbcA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleLinkA, Update));

	hResult = WrapIBindCtxAFromW(pbc, &pbcA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Update(pbcA);

	if (pbcA)
		pbcA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IOleInPlaceObjectW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectW::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectW::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceObjectW::GetWindow", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceObjectA, GetWindow));

	return GetANSI()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectW::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectW::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceObjectW::ContextSensitiveHelp", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceObjectA, ContextSensitiveHelp));

	return GetANSI()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectW::InPlaceDeactivate, public
//
//  Synopsis:   Thunks InPlaceDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectW::InPlaceDeactivate(VOID)
{
	TraceMethodEnter("COleInPlaceObjectW::InPlaceDeactivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceObjectA, InPlaceDeactivate));

	return GetANSI()->InPlaceDeactivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectW::UIDeactivate, public
//
//  Synopsis:   Thunks UIDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectW::UIDeactivate(VOID)
{
	TraceMethodEnter("COleInPlaceObjectW::UIDeactivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceObjectA, UIDeactivate));

	return GetANSI()->UIDeactivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectW::SetObjectRects, public
//
//  Synopsis:   Thunks SetObjectRects to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectW::SetObjectRects(LPCRECT lprcPosRect,
		LPCRECT lprcClipRect)
{
	TraceMethodEnter("COleInPlaceObjectW::SetObjectRects", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceObjectA, SetObjectRects));

	return GetANSI()->SetObjectRects(lprcPosRect, lprcClipRect);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectW::ReactivateAndUndo, public
//
//  Synopsis:   Thunks ReactivateAndUndo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectW::ReactivateAndUndo(VOID)
{
	TraceMethodEnter("COleInPlaceObjectW::ReactivateAndUndo", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceObjectA, ReactivateAndUndo));

	return GetANSI()->ReactivateAndUndo();
}



//***************************************************************************
//
//                   IOleInPlaceActiveObjectW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectW::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectW::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceActiveObjectW::GetWindow", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceActiveObjectA, GetWindow));

	return GetANSI()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectW::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectW::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceActiveObjectW::ContextSensitiveHelp", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceActiveObjectA, ContextSensitiveHelp));

	return GetANSI()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectW::TranslateAccelerator, public
//
//  Synopsis:   Thunks TranslateAccelerator to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectW::TranslateAccelerator(LPMSG lpmsg)
{
	TraceMethodEnter("COleInPlaceActiveObjectW::TranslateAccelerator", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceActiveObjectA, TranslateAccelerator));

	return GetANSI()->TranslateAccelerator(lpmsg);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectW::OnFrameWindowActivate, public
//
//  Synopsis:   Thunks OnFrameWindowActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectW::OnFrameWindowActivate(BOOL fActivate)
{
	TraceMethodEnter("COleInPlaceActiveObjectW::OnFrameWindowActivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceActiveObjectA, OnFrameWindowActivate));

	return GetANSI()->OnFrameWindowActivate(fActivate);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectW::OnDocWindowActivate, public
//
//  Synopsis:   Thunks OnDocWindowActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectW::OnDocWindowActivate(BOOL fActivate)
{
	TraceMethodEnter("COleInPlaceActiveObjectW::OnDocWindowActivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceActiveObjectA, OnDocWindowActivate));

	return GetANSI()->OnDocWindowActivate(fActivate);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectW::ResizeBorder, public
//
//  Synopsis:   Thunks ResizeBorder to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectW::ResizeBorder(LPCRECT lprectBorder,
		LPOLEINPLACEUIWINDOW lpUIWindow, BOOL fFrameWindow)
{
	TraceMethodEnter("COleInPlaceActiveObjectW::ResizeBorder", this);

	LPOLEINPLACEUIWINDOWA lpUIWindowA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceActiveObjectA, ResizeBorder));

	hResult = WrapIOleInPlaceUIWindowAFromW(lpUIWindow, &lpUIWindowA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->ResizeBorder(lprectBorder, lpUIWindowA, fFrameWindow);

	if (lpUIWindowA)
		lpUIWindowA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectW::EnableModeless, public
//
//  Synopsis:   Thunks EnableModeless to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectW::EnableModeless(BOOL fEnable)
{
	TraceMethodEnter("COleInPlaceActiveObjectW::EnableModeless", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceActiveObjectA, EnableModeless));

	return GetANSI()->EnableModeless(fEnable);
}



//***************************************************************************
//
//                   IOleInPlaceFrameW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceFrameW::GetWindow", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, GetWindow));

	return GetANSI()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceFrameW::ContextSensitiveHelp", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, ContextSensitiveHelp));

	return GetANSI()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::GetBorder, public
//
//  Synopsis:   Thunks GetBorder to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::GetBorder(LPRECT lprectBorder)
{
	TraceMethodEnter("COleInPlaceFrameW::GetBorder", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, GetBorder));

	return GetANSI()->GetBorder(lprectBorder);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::RequestBorderSpace, public
//
//  Synopsis:   Thunks RequestBorderSpace to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::RequestBorderSpace(
		LPCBORDERWIDTHS lpborderwidths)
{
	TraceMethodEnter("COleInPlaceFrameW::RequestBorderSpace", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, RequestBorderSpace));

	return GetANSI()->RequestBorderSpace(lpborderwidths);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::SetBorderSpace, public
//
//  Synopsis:   Thunks SetBorderSpace to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::SetBorderSpace(
		LPCBORDERWIDTHS lpborderwidths)
{
	TraceMethodEnter("COleInPlaceFrameW::SetBorderSpace", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, SetBorderSpace));

	return GetANSI()->SetBorderSpace(lpborderwidths);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::SetActiveObject, public
//
//  Synopsis:   Thunks SetActiveObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::SetActiveObject(
		LPOLEINPLACEACTIVEOBJECT lpActiveObject, LPCOLESTR lpszObjName)
{
	TraceMethodEnter("COleInPlaceFrameW::SetActiveObject", this);


	LPOLEINPLACEACTIVEOBJECTA lpActiveObjectA;
	LPSTR lpszObjNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, SetActiveObject));

	hResult = WrapIOleInPlaceActiveObjectAFromW(lpActiveObject, &lpActiveObjectA);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertStringToA(lpszObjName, &lpszObjNameA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->SetActiveObject(lpActiveObjectA, lpszObjNameA);

	ConvertStringFree(lpszObjNameA);

Error:
	if (lpActiveObjectA)
		lpActiveObjectA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::InsertMenus, public
//
//  Synopsis:   Thunks InsertMenus to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::InsertMenus(HMENU hmenuShared,
		LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	TraceMethodEnter("COleInPlaceFrameW::InsertMenus", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, InsertMenus));

	return GetANSI()->InsertMenus(hmenuShared, lpMenuWidths);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::SetMenu, public
//
//  Synopsis:   Thunks SetMenu to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::SetMenu(HMENU hmenuShared, HOLEMENU holemenu,
		HWND hwndActiveObject)
{
	TraceMethodEnter("COleInPlaceFrameW::SetMenu", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, SetMenu));

	return GetANSI()->SetMenu(hmenuShared, holemenu, hwndActiveObject);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::RemoveMenus, public
//
//  Synopsis:   Thunks RemoveMenus to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::RemoveMenus(HMENU hmenuShared)
{
	TraceMethodEnter("COleInPlaceFrameW::RemoveMenus", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, RemoveMenus));

	return GetANSI()->RemoveMenus(hmenuShared);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::SetStatusText, public
//
//  Synopsis:   Thunks SetStatusText to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::SetStatusText(LPCOLESTR lpszStatusText)
{
	TraceMethodEnter("COleInPlaceFrameW::SetStatusText", this);

	LPSTR lpszStatusTextA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, SetStatusText));

	hResult = ConvertStringToA(lpszStatusText, &lpszStatusTextA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetStatusText(lpszStatusTextA);

	ConvertStringFree(lpszStatusTextA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::EnableModeless, public
//
//  Synopsis:   Thunks EnableModeless to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::EnableModeless(BOOL fEnable)
{
	TraceMethodEnter("COleInPlaceFrameW::EnableModeless", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, EnableModeless));

	return GetANSI()->EnableModeless(fEnable);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameW::TranslateAccelerator, public
//
//  Synopsis:   Thunks TranslateAccelerator to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameW::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	TraceMethodEnter("COleInPlaceFrameW::TranslateAccelerator", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceFrameA, TranslateAccelerator));

	return GetANSI()->TranslateAccelerator(lpmsg, wID);
}



//***************************************************************************
//
//                   IOleInPlaceSiteW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceSiteW::GetWindow", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, GetWindow));

	return GetANSI()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceSiteW::ContextSensitiveHelp", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, ContextSensitiveHelp));

	return GetANSI()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::CanInPlaceActivate, public
//
//  Synopsis:   Thunks CanInPlaceActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::CanInPlaceActivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteW::CanInPlaceActivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, CanInPlaceActivate));

	return GetANSI()->CanInPlaceActivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::OnInPlaceActivate, public
//
//  Synopsis:   Thunks OnInPlaceActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::OnInPlaceActivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteW::OnInPlaceActivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, OnInPlaceActivate));

	return GetANSI()->OnInPlaceActivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::OnUIActivate, public
//
//  Synopsis:   Thunks OnUIActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::OnUIActivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteW::OnUIActivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, OnUIActivate));

	return GetANSI()->OnUIActivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::GetWindowContext, public
//
//  Synopsis:   Thunks GetWindowContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::GetWindowContext(
		LPOLEINPLACEFRAME * lplpFrame, LPOLEINPLACEUIWINDOW * lplpDoc,
		LPRECT lprcPosRect, LPRECT lprcClipRect,
		LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	TraceMethodEnter("COleInPlaceSiteW::GetWindowContext", this);

	LPOLEINPLACEFRAMEA lpFrameA;
	LPOLEINPLACEUIWINDOWA lpDocA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, GetWindowContext));

	*lplpFrame = NULL;
	*lplpDoc = NULL;

	hResult = GetANSI()->GetWindowContext(&lpFrameA, &lpDocA, lprcPosRect,
			lprcClipRect, lpFrameInfo);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIOleInPlaceFrameWFromA(lpFrameA, lplpFrame);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIOleInPlaceUIWindowWFromA(lpDocA, lplpDoc);

Error:
	if (lpFrameA)
		lpFrameA->Release();
Error1:
	if (lpDocA)
		lpDocA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::Scroll, public
//
//  Synopsis:   Thunks Scroll to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::Scroll(SIZE scrollExtent)
{
	TraceMethodEnter("COleInPlaceSiteW::Scroll", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, Scroll));

	return GetANSI()->Scroll(scrollExtent);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::OnUIDeactivate, public
//
//  Synopsis:   Thunks OnUIDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::OnUIDeactivate(BOOL fUndoable)
{
	TraceMethodEnter("COleInPlaceSiteW::OnUIDeactivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, OnUIDeactivate));

	return GetANSI()->OnUIDeactivate(fUndoable);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::OnInPlaceDeactivate, public
//
//  Synopsis:   Thunks OnInPlaceDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::OnInPlaceDeactivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteW::OnInPlaceDeactivate", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, OnInPlaceDeactivate));

	return GetANSI()->OnInPlaceDeactivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::DiscardUndoState, public
//
//  Synopsis:   Thunks DiscardUndoState to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::DiscardUndoState(VOID)
{
	TraceMethodEnter("COleInPlaceSiteW::DiscardUndoState", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, DiscardUndoState));

	return GetANSI()->DiscardUndoState();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::DeactivateAndUndo, public
//
//  Synopsis:   Thunks DeactivateAndUndo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::DeactivateAndUndo(VOID)
{
	TraceMethodEnter("COleInPlaceSiteW::DeactivateAndUndo", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, DeactivateAndUndo));

	return GetANSI()->DeactivateAndUndo();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteW::OnPosRectChange, public
//
//  Synopsis:   Thunks OnPosRectChange to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteW::OnPosRectChange(LPCRECT lprcPosRect)
{
	TraceMethodEnter("COleInPlaceSiteW::OnPosRectChange", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IOleInPlaceSiteA, OnPosRectChange));

	return GetANSI()->OnPosRectChange(lprcPosRect);
}
