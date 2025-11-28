//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansiole1.cpp
//
//  Contents:   ANSI Wrappers for Unicode Ole2 Interfaces and APIs.
//
//  Classes:    COleItemContainerA
//              COleAdviseHolderA
//              COleLinkA
//              COleInPlaceObjectA
//              COleInPlaceActiveObjectA
//              COleInPlaceFrameA
//              COleInPlaceSiteA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IOleItemContainerA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerA::ParseDisplayName, public
//
//  Synopsis:   Thunks ParseDisplayName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerA::ParseDisplayName(LPBCA pbcA,
		LPSTR lpszDisplayNameA, ULONG * pchEaten, LPMONIKERA * ppmkOutA)
{
	TraceMethodEnter("COleItemContainerA::ParseDisplayName", this);

	LPBC pbc;
	LPOLESTR lpszDisplayName;
	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IParseDisplayName, ParseDisplayName));

	*ppmkOutA = NULL;

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToW(lpszDisplayNameA, &lpszDisplayName);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->ParseDisplayName(pbc, lpszDisplayName, pchEaten, &pmk);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIMonikerAFromW(pmk, ppmkOutA);

	if (pmk)
		pmk->Release();

Error1:
	ConvertStringFree(lpszDisplayName);

Error:
	if (pbc)
		pbc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerA::EnumObjects, public
//
//  Synopsis:   Thunks EnumObjects to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerA::EnumObjects(DWORD grfFlags,
		LPENUMUNKNOWN * ppenumUnknown)
{
	TraceMethodEnter("COleItemContainerA::EnumObjects", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleContainer, EnumObjects));

	return GetWide()->EnumObjects(grfFlags, ppenumUnknown);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerA::LockContainer, public
//
//  Synopsis:   Thunks LockContainer to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerA::LockContainer(BOOL fLock)
{
	TraceMethodEnter("COleItemContainerA::LockContainer", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleContainer, LockContainer));

	return GetWide()->LockContainer(fLock);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerA::GetObject, public
//
//  Synopsis:   Thunks GetObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerA::GetObject(LPSTR lpszItemA,
		DWORD dwSpeedNeeded, LPBINDCTXA pbcA, REFIID riid,
		LPVOID * ppvObject)
{
	TraceMethodEnter("COleItemContainerA::GetObject", this);

	LPOLESTR lpszItem;
	LPBINDCTX   pbc;
	LPUNKNOWN   punk;
	IDINTERFACE idRef;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleItemContainer, GetObject));

	hResult = ConvertStringToW(lpszItemA, &lpszItem);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->GetObject(lpszItem, dwSpeedNeeded, pbc, riid,
			(LPVOID *)&punk);
	if (FAILED(hResult))
		goto Error1;

	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, punk, (LPUNKNOWN *)ppvObject);

	if (punk)
		punk->Release();

Error1:
	if (pbc)
		pbc->Release();

Error:
	ConvertStringFree(lpszItem);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerA::GetObjectStorage, public
//
//  Synopsis:   Thunks GetObjectStorage to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerA::GetObjectStorage(LPSTR lpszItemA,
		LPBINDCTXA pbcA, REFIID riid, LPVOID * ppvStorage)
{
	TraceMethodEnter("COleItemContainerA::GetObjectStorage", this);

	LPOLESTR lpszItem;
	LPBINDCTX   pbc;
	LPUNKNOWN   punk;
	IDINTERFACE idRef;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleItemContainer, GetObjectStorage));

	hResult = ConvertStringToW(lpszItemA, &lpszItem);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->GetObjectStorage(lpszItem, pbc, riid, (LPVOID *)&punk);
	if (FAILED(hResult))
		goto Error1;

	idRef = WrapTranslateIID(riid);

	hResult = WrapInterfaceAFromW(idRef, punk, (LPUNKNOWN *)ppvStorage);

	if (punk)
		punk->Release();

Error1:
	if (pbc)
		pbc->Release();

Error:
	ConvertStringFree(lpszItem);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleItemContainerA::IsRunning, public
//
//  Synopsis:   Thunks IsRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleItemContainerA::IsRunning(LPSTR lpszItemA)
{
	TraceMethodEnter("COleItemContainerA::IsRunning", this);

	LPOLESTR lpszItem;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleItemContainer, IsRunning));

	hResult = ConvertStringToW(lpszItemA, &lpszItem);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->IsRunning(lpszItem);

	ConvertStringFree(lpszItem);

	return hResult;
}



//***************************************************************************
//
//                   IOleAdviseHolderA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderA::Advise, public
//
//  Synopsis:   Thunks Advise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderA::Advise(LPADVISESINKA pAdviseA,
		DWORD * pdwConnection)
{
	TraceMethodEnter("COleAdviseHolderA::Advise", this);

	LPADVISESINK pAdvise;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleAdviseHolder, Advise));

	hResult = WrapIAdviseSinkWFromA(pAdviseA, &pAdvise);
	if (FAILED(hResult))
		return hResult;

	hResult =  GetWide()->Advise(pAdvise, pdwConnection);

	if (pAdvise)
		pAdvise->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderA::Unadvise, public
//
//  Synopsis:   Thunks Unadvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderA::Unadvise(DWORD dwConnection)
{
	TraceMethodEnter("COleAdviseHolderA::Unadvise", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleAdviseHolder, Unadvise));

	return GetWide()->Unadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderA::EnumAdvise, public
//
//  Synopsis:   Thunks EnumAdvise to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderA::EnumAdvise(LPENUMSTATDATAA * ppenumAdviseA)
{
	TraceMethodEnter("COleAdviseHolderA::EnumAdvise", this);

	LPENUMSTATDATA penumAdvise = NULL;
	HRESULT hResult;



	_DebugHook(GetWide(), MEMBER_PTR(IOleAdviseHolder, EnumAdvise));

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
//  Member:     COleAdviseHolderA::SendOnRename, public
//
//  Synopsis:   Thunks SendOnRename to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderA::SendOnRename(LPMONIKERA pmkA)
{
	TraceMethodEnter("COleAdviseHolderA::SendOnRename", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleAdviseHolder, SendOnRename));

	hResult = WrapIMonikerWFromA(pmkA, &pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SendOnRename(pmk);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderA::SendOnSave, public
//
//  Synopsis:   Thunks SendOnSave to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderA::SendOnSave(VOID)
{
	TraceMethodEnter("COleAdviseHolderA::SendOnSave", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleAdviseHolder, SendOnSave));

	return GetWide()->SendOnSave();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleAdviseHolderA::SendOnClose, public
//
//  Synopsis:   Thunks SendOnClose to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleAdviseHolderA::SendOnClose(VOID)
{
	TraceMethodEnter("COleAdviseHolderA::SendOnClose", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleAdviseHolder, SendOnClose));

	return GetWide()->SendOnClose();
}



//***************************************************************************
//
//                   IOleLinkA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::SetUpdateOptions, public
//
//  Synopsis:   Thunks SetUpdateOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::SetUpdateOptions(DWORD dwUpdateOpt)
{
	TraceMethodEnter("COleLinkA::SetUpdateOptions", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, SetUpdateOptions));

	return GetWide()->SetUpdateOptions(dwUpdateOpt);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::GetUpdateOptions, public
//
//  Synopsis:   Thunks GetUpdateOptions to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::GetUpdateOptions(LPDWORD pdwUpdateOpt)
{
	TraceMethodEnter("COleLinkA::GetUpdateOptions", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, GetUpdateOptions));

	return GetWide()->GetUpdateOptions(pdwUpdateOpt);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::SetSourceMoniker, public
//
//  Synopsis:   Thunks SetSourceMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::SetSourceMoniker(LPMONIKERA pmkA, REFCLSID rclsid)
{
	TraceMethodEnter("COleLinkA::SetSourceMoniker", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, SetSourceMoniker));

	hResult = WrapIMonikerWFromA(pmkA, &pmk);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SetSourceMoniker(pmk, rclsid);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::GetSourceMoniker, public
//
//  Synopsis:   Thunks GetSourceMoniker to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::GetSourceMoniker(LPMONIKERA * ppmkA)
{
	TraceMethodEnter("COleLinkA::GetSourceMoniker", this);

	LPMONIKER pmk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, GetSourceMoniker));

	*ppmkA = NULL;

	hResult = GetWide()->GetSourceMoniker(&pmk);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIMonikerAFromW(pmk, ppmkA);

	if (pmk)
		pmk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::SetSourceDisplayName, public
//
//  Synopsis:   Thunks SetSourceDisplayName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::SetSourceDisplayName(LPCSTR lpszDisplayNameA)
{
	TraceMethodEnter("COleLinkA::SetSourceDisplayName", this);

	LPOLESTR lpszDisplayName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, SetSourceDisplayName));

	hResult = ConvertStringToW(lpszDisplayNameA, &lpszDisplayName);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetSourceDisplayName(lpszDisplayName);

	ConvertStringFree(lpszDisplayName);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::GetSourceDisplayName, public
//
//  Synopsis:   Thunks GetSourceDisplayName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::GetSourceDisplayName(LPSTR * lplpszDisplayNameA)
{
	TraceMethodEnter("COleLinkA::GetSourceDisplayName", this);

	LPOLESTR lpszDisplayName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, GetSourceDisplayName));

	hResult = GetWide()->GetSourceDisplayName(&lpszDisplayName);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToA(lpszDisplayName, lplpszDisplayNameA);

	ConvertStringFree(lpszDisplayName);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::BindToSource, public
//
//  Synopsis:   Thunks BindToSource to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::BindToSource(DWORD bindflags, LPBINDCTXA pbcA)
{
	TraceMethodEnter("COleLinkA::BindToSource", this);

	LPBINDCTX pbc;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, BindToSource));

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->BindToSource(bindflags, pbc);

	if (pbc)
		pbc->Release();

	TraceMethodExit("COleLinkA::BindToSource", this, hResult);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::BindIfRunning, public
//
//  Synopsis:   Thunks BindIfRunning to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::BindIfRunning(VOID)
{
	TraceMethodEnter("COleLinkA::BindIfRunning", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, BindIfRunning));

	return GetWide()->BindIfRunning();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::GetBoundSource, public
//
//  Synopsis:   Thunks GetBoundSource to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::GetBoundSource(LPUNKNOWN * ppUnk)
{
	TraceMethodEnter("COleLinkA::GetBoundSource", this);

	LPUNKNOWN punk;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, GetBoundSource));

	hResult = GetWide()->GetBoundSource(&punk);
	if (FAILED(hResult))
			return hResult;

	hResult = WrapIUnknownAFromW(punk, ppUnk);
	punk->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::UnbindSource, public
//
//  Synopsis:   Thunks UnbindSource to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::UnbindSource(VOID)
{
	TraceMethodEnter("COleLinkA::UnbindSource", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, UnbindSource));

	return GetWide()->UnbindSource();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleLinkA::Update, public
//
//  Synopsis:   Thunks Update to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleLinkA::Update(LPBINDCTXA pbcA)
{
	TraceMethodEnter("COleLinkA::Update", this);

	LPBINDCTX pbc;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleLink, Update));

	hResult = WrapIBindCtxWFromA(pbcA, &pbc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Update(pbc);

	if (pbc)
		pbc->Release();

	return hResult;
}



//***************************************************************************
//
//                   IOleInPlaceObjectA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectA::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectA::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceObjectA::GetWindow", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, GetWindow));

	return GetWide()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectA::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectA::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceObjectA::ContextSensitiveHelp", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, ContextSensitiveHelp));

	return GetWide()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectA::InPlaceDeactivate, public
//
//  Synopsis:   Thunks InPlaceDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectA::InPlaceDeactivate(VOID)
{
	TraceMethodEnter("COleInPlaceObjectA::InPlaceDeactivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceObject, InPlaceDeactivate));

	return GetWide()->InPlaceDeactivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectA::UIDeactivate, public
//
//  Synopsis:   Thunks UIDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectA::UIDeactivate(VOID)
{
	TraceMethodEnter("COleInPlaceObjectA::UIDeactivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceObject, UIDeactivate));

	return GetWide()->UIDeactivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectA::SetObjectRects, public
//
//  Synopsis:   Thunks SetObjectRects to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectA::SetObjectRects(LPCRECT lprcPosRect,
		LPCRECT lprcClipRect)
{
	TraceMethodEnter("COleInPlaceObjectA::SetObjectRects", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceObject, SetObjectRects));

	return GetWide()->SetObjectRects(lprcPosRect, lprcClipRect);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceObjectA::ReactivateAndUndo, public
//
//  Synopsis:   Thunks ReactivateAndUndo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceObjectA::ReactivateAndUndo(VOID)
{
	TraceMethodEnter("COleInPlaceObjectA::ReactivateAndUndo", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceObject, ReactivateAndUndo));

	return GetWide()->ReactivateAndUndo();
}



//***************************************************************************
//
//                   IOleInPlaceActiveObjectA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectA::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectA::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceActiveObjectA::GetWindow", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, GetWindow));

	return GetWide()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectA::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectA::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceActiveObjectA::ContextSensitiveHelp", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, ContextSensitiveHelp));

	return GetWide()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectA::TranslateAccelerator, public
//
//  Synopsis:   Thunks TranslateAccelerator to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectA::TranslateAccelerator(LPMSG lpmsg)
{
	TraceMethodEnter("COleInPlaceActiveObjectA::TranslateAccelerator", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceActiveObject, TranslateAccelerator));

	return GetWide()->TranslateAccelerator(lpmsg);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectA::OnFrameWindowActivate, public
//
//  Synopsis:   Thunks OnFrameWindowActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectA::OnFrameWindowActivate(BOOL fActivate)
{
	TraceMethodEnter("COleInPlaceActiveObjectA::OnFrameWindowActivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceActiveObject, OnFrameWindowActivate));

	return GetWide()->OnFrameWindowActivate(fActivate);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectA::OnDocWindowActivate, public
//
//  Synopsis:   Thunks OnDocWindowActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectA::OnDocWindowActivate(BOOL fActivate)
{
	TraceMethodEnter("COleInPlaceActiveObjectA::OnDocWindowActivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceActiveObject, OnDocWindowActivate));

	return GetWide()->OnDocWindowActivate(fActivate);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectA::ResizeBorder, public
//
//  Synopsis:   Thunks ResizeBorder to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectA::ResizeBorder(LPCRECT lprectBorder,
		LPOLEINPLACEUIWINDOWA lpUIWindowA, BOOL fFrameWindow)
{
	TraceMethodEnter("COleInPlaceActiveObjectA::ResizeBorder", this);

	LPOLEINPLACEUIWINDOW lpUIWindow;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceActiveObject, ResizeBorder));

	hResult = WrapIOleInPlaceUIWindowWFromA(lpUIWindowA, &lpUIWindow);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->ResizeBorder(lprectBorder, lpUIWindow, fFrameWindow);

	if (lpUIWindow)
		lpUIWindow->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceActiveObjectA::EnableModeless, public
//
//  Synopsis:   Thunks EnableModeless to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceActiveObjectA::EnableModeless(BOOL fEnable)
{
	TraceMethodEnter("COleInPlaceActiveObjectA::EnableModeless", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceActiveObject, EnableModeless));

	return GetWide()->EnableModeless(fEnable);
}



//***************************************************************************
//
//                   IOleInPlaceFrameA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceFrameA::GetWindow", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, GetWindow));

	return GetWide()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceFrameA::ContextSensitiveHelp", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, ContextSensitiveHelp));

	return GetWide()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::GetBorder, public
//
//  Synopsis:   Thunks GetBorder to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::GetBorder(LPRECT lprectBorder)
{
	TraceMethodEnter("COleInPlaceFrameA::GetBorder", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceUIWindow, GetBorder));

	return GetWide()->GetBorder(lprectBorder);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::RequestBorderSpace, public
//
//  Synopsis:   Thunks RequestBorderSpace to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::RequestBorderSpace(
		LPCBORDERWIDTHS lpborderwidths)
{
	TraceMethodEnter("COleInPlaceFrameA::RequestBorderSpace", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceUIWindow, RequestBorderSpace));

	return GetWide()->RequestBorderSpace(lpborderwidths);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::SetBorderSpace, public
//
//  Synopsis:   Thunks SetBorderSpace to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::SetBorderSpace(
		LPCBORDERWIDTHS lpborderwidths)
{
	TraceMethodEnter("COleInPlaceFrameA::SetBorderSpace", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceUIWindow, SetBorderSpace));

	return GetWide()->SetBorderSpace(lpborderwidths);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::SetActiveObject, public
//
//  Synopsis:   Thunks SetActiveObject to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::SetActiveObject(
		LPOLEINPLACEACTIVEOBJECTA lpActiveObjectA, LPCSTR lpszObjNameA)
{
	TraceMethodEnter("COleInPlaceFrameA::SetActiveObject", this);


	LPOLEINPLACEACTIVEOBJECT lpActiveObject;
	LPOLESTR lpszObjName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceUIWindow, SetActiveObject));

	hResult = WrapIOleInPlaceActiveObjectWFromA(lpActiveObjectA, &lpActiveObject);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertStringToW(lpszObjNameA, &lpszObjName);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->SetActiveObject(lpActiveObject, lpszObjName);

	ConvertStringFree(lpszObjName);

Error:
	if (lpActiveObject)
		lpActiveObject->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::InsertMenus, public
//
//  Synopsis:   Thunks InsertMenus to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::InsertMenus(HMENU hmenuShared,
		LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	TraceMethodEnter("COleInPlaceFrameA::InsertMenus", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceFrame, InsertMenus));

	return GetWide()->InsertMenus(hmenuShared, lpMenuWidths);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::SetMenu, public
//
//  Synopsis:   Thunks SetMenu to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::SetMenu(HMENU hmenuShared, HOLEMENU holemenu,
		HWND hwndActiveObject)
{
	TraceMethodEnter("COleInPlaceFrameA::SetMenu", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceFrame, SetMenu));

	return GetWide()->SetMenu(hmenuShared, holemenu, hwndActiveObject);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::RemoveMenus, public
//
//  Synopsis:   Thunks RemoveMenus to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::RemoveMenus(HMENU hmenuShared)
{
	TraceMethodEnter("COleInPlaceFrameA::RemoveMenus", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceFrame, RemoveMenus));

	return GetWide()->RemoveMenus(hmenuShared);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::SetStatusText, public
//
//  Synopsis:   Thunks SetStatusText to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::SetStatusText(LPCSTR lpszStatusTextA)
{
	TraceMethodEnter("COleInPlaceFrameA::SetStatusText", this);

	LPOLESTR lpszStatusText;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceFrame, SetStatusText));

	hResult = ConvertStringToW(lpszStatusTextA, &lpszStatusText);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetStatusText(lpszStatusText);

	ConvertStringFree(lpszStatusText);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::EnableModeless, public
//
//  Synopsis:   Thunks EnableModeless to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::EnableModeless(BOOL fEnable)
{
	TraceMethodEnter("COleInPlaceFrameA::EnableModeless", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceFrame, EnableModeless));

	return GetWide()->EnableModeless(fEnable);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceFrameA::TranslateAccelerator, public
//
//  Synopsis:   Thunks TranslateAccelerator to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceFrameA::TranslateAccelerator(LPMSG lpmsg, WORD wID)
{
	TraceMethodEnter("COleInPlaceFrameA::TranslateAccelerator", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceFrame, TranslateAccelerator));

	return GetWide()->TranslateAccelerator(lpmsg, wID);
}



//***************************************************************************
//
//                   IOleInPlaceSiteA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::GetWindow, public
//
//  Synopsis:   Thunks GetWindow to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::GetWindow(HWND * lphwnd)
{
	TraceMethodEnter("COleInPlaceSiteA::GetWindow", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, GetWindow));

	return GetWide()->GetWindow(lphwnd);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::ContextSensitiveHelp, public
//
//  Synopsis:   Thunks ContextSensitiveHelp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::ContextSensitiveHelp(BOOL fEnterMode)
{
	TraceMethodEnter("COleInPlaceSiteA::ContextSensitiveHelp", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleWindow, ContextSensitiveHelp));

	return GetWide()->ContextSensitiveHelp(fEnterMode);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::CanInPlaceActivate, public
//
//  Synopsis:   Thunks CanInPlaceActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::CanInPlaceActivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteA::CanInPlaceActivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, CanInPlaceActivate));

	return GetWide()->CanInPlaceActivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::OnInPlaceActivate, public
//
//  Synopsis:   Thunks OnInPlaceActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::OnInPlaceActivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteA::OnInPlaceActivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, OnInPlaceActivate));

	return GetWide()->OnInPlaceActivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::OnUIActivate, public
//
//  Synopsis:   Thunks OnUIActivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::OnUIActivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteA::OnUIActivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, OnUIActivate));

	return GetWide()->OnUIActivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::GetWindowContext, public
//
//  Synopsis:   Thunks GetWindowContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::GetWindowContext(
		LPOLEINPLACEFRAMEA * lplpFrameA, LPOLEINPLACEUIWINDOWA * lplpDocA,
		LPRECT lprcPosRect, LPRECT lprcClipRect,
		LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	TraceMethodEnter("COleInPlaceSiteA::GetWindowContext", this);

	LPOLEINPLACEFRAME lpFrame;
	LPOLEINPLACEUIWINDOW lpDoc;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, GetWindowContext));

	*lplpFrameA = NULL;
	*lplpDocA = NULL;

	hResult = GetWide()->GetWindowContext(&lpFrame, &lpDoc, lprcPosRect,
			lprcClipRect, lpFrameInfo);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIOleInPlaceFrameAFromW(lpFrame, lplpFrameA);
	if (FAILED(hResult))
		goto Error1;

	hResult = WrapIOleInPlaceUIWindowAFromW(lpDoc, lplpDocA);

Error:
	if (lpFrame)
		lpFrame->Release();
Error1:
	if (lpDoc)
		lpDoc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::Scroll, public
//
//  Synopsis:   Thunks Scroll to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::Scroll(SIZE scrollExtent)
{
	TraceMethodEnter("COleInPlaceSiteA::Scroll", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, Scroll));

	return GetWide()->Scroll(scrollExtent);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::OnUIDeactivate, public
//
//  Synopsis:   Thunks OnUIDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::OnUIDeactivate(BOOL fUndoable)
{
	TraceMethodEnter("COleInPlaceSiteA::OnUIDeactivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, OnUIDeactivate));

	return GetWide()->OnUIDeactivate(fUndoable);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::OnInPlaceDeactivate, public
//
//  Synopsis:   Thunks OnInPlaceDeactivate to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::OnInPlaceDeactivate(VOID)
{
	TraceMethodEnter("COleInPlaceSiteA::OnInPlaceDeactivate", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, OnInPlaceDeactivate));

	return GetWide()->OnInPlaceDeactivate();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::DiscardUndoState, public
//
//  Synopsis:   Thunks DiscardUndoState to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::DiscardUndoState(VOID)
{
	TraceMethodEnter("COleInPlaceSiteA::DiscardUndoState", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, DiscardUndoState));

	return GetWide()->DiscardUndoState();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::DeactivateAndUndo, public
//
//  Synopsis:   Thunks DeactivateAndUndo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::DeactivateAndUndo(VOID)
{
	TraceMethodEnter("COleInPlaceSiteA::DeactivateAndUndo", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, DeactivateAndUndo));

	return GetWide()->DeactivateAndUndo();
}


//+--------------------------------------------------------------------------
//
//  Member:     COleInPlaceSiteA::OnPosRectChange, public
//
//  Synopsis:   Thunks OnPosRectChange to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleInPlaceSiteA::OnPosRectChange(LPCRECT lprcPosRect)
{
	TraceMethodEnter("COleInPlaceSiteA::OnPosRectChange", this);

	_DebugHook(GetWide(), MEMBER_PTR(IOleInPlaceSite, OnPosRectChange));

	return GetWide()->OnPosRectChange(lprcPosRect);
}
