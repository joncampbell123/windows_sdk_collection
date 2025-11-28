//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       WideCtl.cpp
//
//  Contents:   ANSI Wrappers for OLE Control Interfaces and APIs.
//
//  History:    01-Jun-94   johnels     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertyNotifySinkW class

STDMETHODIMP CPropertyNotifySinkW::OnChanged(DISPID dispid)
{
	TraceMethodEnter("CPropertyNotifySinkW::OnChanged", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyNotifySink, OnChanged));
	return GetANSI()->OnChanged(dispid);
}

STDMETHODIMP CPropertyNotifySinkW::OnRequestEdit(DISPID dispid)
{
	TraceMethodEnter("CPropertyNotifySinkW::OnRequestEdit", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyNotifySink, OnRequestEdit));
	return GetANSI()->OnRequestEdit(dispid);
}


/////////////////////////////////////////////////////////////////////////////
// CProvideClassInfoW class

STDMETHODIMP CProvideClassInfoW::GetClassInfo(LPTYPEINFO FAR* ppTI)
{
	TraceMethodEnter("CProvideClassInfoW::GetClassInfo", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IProvideClassInfo, GetClassInfo));

	LPTYPEINFOA pTypeInfo;
	HRESULT hResult;

	hResult = GetANSI()->GetClassInfo(&pTypeInfo);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapITypeInfoWFromA(pTypeInfo, ppTI);
	pTypeInfo->Release();
	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectionPointContainerW class

STDMETHODIMP CConnectionPointContainerW::EnumConnectionPoints(
			LPENUMCONNECTIONPOINTS FAR* ppEnum)
{
	TraceMethodEnter("CConnectionPointContainerW::EnumConnectionPoints", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IConnectionPointContainer, EnumConnectionPoints));

	LPENUMCONNECTIONPOINTSA pEnumConnPts;
	HRESULT hResult;

	hResult = GetANSI()->EnumConnectionPoints(&pEnumConnPts);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionPointsWFromA(pEnumConnPts, ppEnum);
	pEnumConnPts->Release();
	return hResult;
}

STDMETHODIMP CConnectionPointContainerW::FindConnectionPoint(
			REFIID iid, LPCONNECTIONPOINT FAR* ppCP)
{
	TraceMethodEnter("CConnectionPointContainerW::FindConnectionPoint", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IConnectionPointContainer, FindConnectionPoint));

	LPCONNECTIONPOINTA pConnPt;
	HRESULT hResult;

	hResult = GetANSI()->FindConnectionPoint(iid, &pConnPt);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIConnectionPointWFromA(pConnPt, ppCP);
	pConnPt->Release();
	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionPointsW class

STDMETHODIMP CEnumConnectionPointsW::Next(
			ULONG cConnections,
			LPCONNECTIONPOINT FAR* rgpcn, ULONG FAR* lpcFetched)
{
	TraceMethodEnter("CEnumConnectionPointsW::Next", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnectionPoints, Next));

	LPCONNECTIONPOINTA FAR* rgpConnPt = new LPCONNECTIONPOINTA[cConnections];
	HRESULT hResult;

	hResult = GetANSI()->Next(cConnections, rgpConnPt, lpcFetched);
	if (FAILED(hResult))
	{
		delete [] rgpConnPt;
		return hResult;
	}

	ULONG i;
	for (i = 0; i < *lpcFetched; i++)   // Wrap each pointer in array
	{
		hResult = WrapIConnectionPointWFromA(rgpConnPt[i], &rgpcn[i]);
		rgpConnPt[i]->Release();
	}

	delete [] rgpConnPt;
	return hResult;
}

STDMETHODIMP CEnumConnectionPointsW::Skip(ULONG cConnections)
{
	TraceMethodEnter("CEnumConnectionPointsW::Skip", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnectionPoints, Skip));
	return GetANSI()->Skip(cConnections);
}

STDMETHODIMP CEnumConnectionPointsW::Reset()
{
	TraceMethodEnter("CEnumConnectionPointsW::Reset", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnectionPoints, Reset));
	return GetANSI()->Reset();
}

STDMETHODIMP CEnumConnectionPointsW::Clone(LPENUMCONNECTIONPOINTS FAR* ppEnum)
{
	TraceMethodEnter("CEnumConnectionPointsW::Clone", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnectionPoints, Clone));

	LPENUMCONNECTIONPOINTSA pEnumConnPts;
	HRESULT hResult;

	hResult = GetANSI()->Clone(&pEnumConnPts);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionPointsWFromA(pEnumConnPts, ppEnum);
	pEnumConnPts->Release();
	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// CConnectionPointW class

STDMETHODIMP CConnectionPointW::GetConnectionInterface(IID FAR* pIID)
{
	TraceMethodEnter("CConnectionPointW::GetConnectionInterface", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IConnectionPoint, GetConnectionInterface));
	return GetANSI()->GetConnectionInterface(pIID);
}

STDMETHODIMP CConnectionPointW::GetConnectionPointContainer(
			LPCONNECTIONPOINTCONTAINER FAR* ppCPC)
{
	TraceMethodEnter("CConnectionPointW::GetConnectionPointContainer", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IConnectionPoint, GetConnectionPointContainer));

	LPCONNECTIONPOINTCONTAINERA pConnPtCont;
	HRESULT hResult;

	hResult = GetANSI()->GetConnectionPointContainer(&pConnPtCont);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIConnectionPointContainerWFromA(pConnPtCont, ppCPC);
	pConnPtCont->Release();
	return hResult;
}

STDMETHODIMP CConnectionPointW::Advise(LPUNKNOWN pUnkSink, DWORD FAR* pdwCookie)
{
	TraceMethodEnter("CConnectionPointW::Advise", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IConnectionPoint, Advise));

	LPUNKNOWN pUnkSinkA;
	HRESULT hResult;

	hResult = WrapIUnknownAFromW(pUnkSink, &pUnkSinkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Advise(pUnkSinkA, pdwCookie);
	pUnkSinkA->Release();
	return hResult;
}

STDMETHODIMP CConnectionPointW::Unadvise(DWORD dwCookie)
{
	TraceMethodEnter("CConnectionPointW::Unadvise", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IConnectionPoint, Unadvise));
	return GetANSI()->Unadvise(dwCookie);
}

STDMETHODIMP CConnectionPointW::EnumConnections(LPENUMCONNECTIONS FAR* ppEnum)
{
	TraceMethodEnter("CConnectionPointW::EnumConnections", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IConnectionPoint, EnumConnections));

	LPENUMCONNECTIONSA pEnumConns;
	HRESULT hResult;

	hResult = GetANSI()->EnumConnections(&pEnumConns);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionsWFromA(pEnumConns, ppEnum);
	pEnumConns->Release();
	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionsW class

STDMETHODIMP CEnumConnectionsW::Next(
			ULONG cConnections,
			LPCONNECTDATA rgcd, ULONG FAR* lpcFetched)
{
	TraceMethodEnter("CEnumConnectionsW::Next", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnections, Next));

	LPCONNECTDATAA rgConnData = new CONNECTDATAA[cConnections];
	HRESULT hResult;

	hResult = GetANSI()->Next(cConnections, rgConnData, lpcFetched);
	if (FAILED(hResult))
	{
		delete [] rgConnData;
		return hResult;
	}

	ULONG i;
	for (i = 0; i < *lpcFetched; i++)   // Wrap each pointer in array
	{
		rgcd[i].dwCookie = rgConnData[i].dwCookie;
		hResult = WrapIUnknownWFromA(rgConnData[i].pUnk, &rgcd[i].pUnk);
		rgConnData[i].pUnk->Release();
	}

	delete [] rgConnData;
	return hResult;
}

STDMETHODIMP CEnumConnectionsW::Skip(ULONG cConnections)
{
	TraceMethodEnter("CEnumConnectionPointsW::Skip", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnectionPoints, Skip));
	return GetANSI()->Skip(cConnections);
}

STDMETHODIMP CEnumConnectionsW::Reset()
{
	TraceMethodEnter("CEnumConnectionPointsW::Reset", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnectionPoints, Reset));
	return GetANSI()->Reset();
}

STDMETHODIMP CEnumConnectionsW::Clone(LPENUMCONNECTIONS FAR* ppEnum)
{
	TraceMethodEnter("CEnumConnectionsW::Clone", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumConnections, Clone));

	LPENUMCONNECTIONSA pEnumConns;
	HRESULT hResult;

	hResult = GetANSI()->Clone(&pEnumConns);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionsWFromA(pEnumConns, ppEnum);
	pEnumConns->Release();
	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// COleControlW class

STDMETHODIMP COleControlW::GetControlInfo(LPCONTROLINFO pCI)
{
	TraceMethodEnter("COleControlW::GetControlInfo", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControl, GetControlInfo));
	return GetANSI()->GetControlInfo(pCI);
}

STDMETHODIMP COleControlW::OnMnemonic(LPMSG pMsg)
{
	TraceMethodEnter("COleControlW::OnMnemonic", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControl, OnMnemonic));
	return GetANSI()->OnMnemonic(pMsg);
}

STDMETHODIMP COleControlW::OnAmbientPropertyChange(DISPID dispid)
{
	TraceMethodEnter("COleControlW::OnAmbientPropertyChange", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControl, OnAmbientPropertyChange));
	return GetANSI()->OnAmbientPropertyChange(dispid);
}

STDMETHODIMP COleControlW::FreezeEvents(BOOL bFreeze)
{
	TraceMethodEnter("COleControlW::FreezeEvents", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControl, FreezeEvents));
	return GetANSI()->FreezeEvents(bFreeze);
}

/////////////////////////////////////////////////////////////////////////////
// COleControlSiteW class

STDMETHODIMP COleControlSiteW::OnControlInfoChanged()
{
	TraceMethodEnter("COleControlSiteW::OnControlInfoChanged", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControlSite, OnControlInfoChanged));
	return GetANSI()->OnControlInfoChanged();
}

STDMETHODIMP COleControlSiteW::LockInPlaceActive(BOOL fLock)
{
	TraceMethodEnter("COleControlSiteW::LockInPlaceActive", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControlSite, LockInPlaceActive));
	return GetANSI()->LockInPlaceActive(fLock);
}

STDMETHODIMP COleControlSiteW::GetExtendedControl(LPDISPATCH FAR* ppDisp)
{
	TraceMethodEnter("COleControlSiteW::GetExtendedControl", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControlSite, GetExtendedControl));

	LPDISPATCHA pDisp;
	HRESULT hResult;

	hResult = GetANSI()->GetExtendedControl(&pDisp);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIDispatchWFromA(pDisp, ppDisp);
	pDisp->Release();
	return hResult;
}

STDMETHODIMP COleControlSiteW::TransformCoords(POINTL FAR* lpptlHimetric,
	POINTF FAR* lpptfContainer, DWORD flags)
{
	TraceMethodEnter("COleControlSiteW::TransformCoords", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControlSite, TransformCoords));
	return GetANSI()->TransformCoords(lpptlHimetric, lpptfContainer, flags);
}

STDMETHODIMP COleControlSiteW::TranslateAccelerator(LPMSG lpMsg,
	DWORD grfModifiers)
{
	TraceMethodEnter("COleControlSiteW::TranslateAccelerator", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControlSite, TranslateAccelerator));
	return GetANSI()->TranslateAccelerator(lpMsg, grfModifiers);
}

STDMETHODIMP COleControlSiteW::OnFocus(BOOL fGotFocus)
{
	TraceMethodEnter("COleControlSiteW::OnFocus", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControlSite, OnFocus));
	return GetANSI()->OnFocus(fGotFocus);
}

STDMETHODIMP COleControlSiteW::ShowPropertyFrame()
{
	TraceMethodEnter("COleControlSiteW::ShowPropertyFrame", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IOleControlSite, ShowPropertyFrame));
	return GetANSI()->ShowPropertyFrame();
}


/////////////////////////////////////////////////////////////////////////////
// CSimpleFrameSiteW class

STDMETHODIMP CSimpleFrameSiteW::PreMessageFilter(HWND hwnd, UINT msg,
	WPARAM wp, LPARAM lp, LRESULT FAR* lplResult, DWORD FAR * lpdwCookie)
{
	TraceMethodEnter("CSimpleFrameSiteW::PreMessageFilter", this);
	_DebugHook(GetANSI(), MEMBER_PTR(ISimpleFrameSite, PreMessageFilter));
	return GetANSI()->PreMessageFilter(hwnd, msg, wp, lp, lplResult, lpdwCookie);
}

STDMETHODIMP CSimpleFrameSiteW::PostMessageFilter(HWND hwnd, UINT msg,
	WPARAM wp, LPARAM lp, LRESULT FAR* lplResult, DWORD dwCookie)
{
	TraceMethodEnter("CSimpleFrameSiteW::PostMessageFilter", this);
	_DebugHook(GetANSI(), MEMBER_PTR(ISimpleFrameSite, PostMessageFilter));
	return GetANSI()->PostMessageFilter(hwnd, msg, wp, lp, lplResult, dwCookie);
}


/////////////////////////////////////////////////////////////////////////////
// CPersistStreamInitW class

STDMETHODIMP CPersistStreamInitW::GetClassID(LPCLSID lpClassID)
{
	TraceMethodEnter("CPersistStreamInitW::GetClassID", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStreamInit, GetClassID));
	return GetANSI()->GetClassID(lpClassID);
}

STDMETHODIMP CPersistStreamInitW::IsDirty()
{
	TraceMethodEnter("CPersistStreamInitW::IsDirty", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStreamInit, IsDirty));
	return GetANSI()->IsDirty();
}

STDMETHODIMP CPersistStreamInitW::Load(LPSTREAM pStm)
{
	TraceMethodEnter("CPersistStreamInitW::Load", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStreamInit, Load));

	LPSTREAMA pStmA;
	HRESULT hResult;

	hResult = WrapIStreamAFromW(pStm, &pStmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Load(pStmA);
	pStmA->Release();
	return hResult;
}

STDMETHODIMP CPersistStreamInitW::Save(LPSTREAM pStm, BOOL fClearDirty)
{
	TraceMethodEnter("CPersistStreamInitW::Save", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStreamInit, Save));

	LPSTREAMA pStmA;
	HRESULT hResult;

	hResult = WrapIStreamAFromW(pStm, &pStmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Save(pStmA, fClearDirty);
	pStmA->Release();
	return hResult;
}

STDMETHODIMP CPersistStreamInitW::GetSizeMax(ULARGE_INTEGER FAR* pcbSize)
{
	TraceMethodEnter("CPersistStreamInitW::GetSizeMax", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStreamInit, GetSizeMax));
	return GetANSI()->GetSizeMax(pcbSize);
}

STDMETHODIMP CPersistStreamInitW::InitNew()
{
	TraceMethodEnter("CPersistStreamInitW::InitNew", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPersistStreamInit, InitNew));
	return GetANSI()->InitNew();
}

/////////////////////////////////////////////////////////////////////////////
// CClassFactory2W class

STDMETHODIMP CClassFactory2W::CreateInstance(LPUNKNOWN pUnkOuter,
		  REFIID riid, LPVOID FAR* ppvObject)
{
	TraceMethodEnter("CClassFactory2W::CreateInstance", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IClassFactory2, CreateInstance));

	LPUNKNOWN pUnkOuterA;
	LPUNKNOWN pUnkA;
	IDINTERFACE idRef;
	HRESULT hResult;

	hResult = WrapIUnknownAFromW(pUnkOuter, &pUnkOuterA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->CreateInstance(pUnkOuterA, riid, (LPVOID *)&pUnkA);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);
	hResult = WrapInterfaceWFromA(idRef, pUnkA, (LPUNKNOWN *)ppvObject);
	pUnkA->Release();

Error:
	if (pUnkOuterA)
		pUnkOuterA->Release();

	return hResult;
}


STDMETHODIMP CClassFactory2W::LockServer(BOOL fLock)
{
	TraceNotify("CClassFactory2W::LockServer");
	_DebugHook(GetANSI(), MEMBER_PTR(IClassFactory2, LockServer));
	return GetANSI()->LockServer(fLock);
}

STDMETHODIMP CClassFactory2W::GetLicInfo(LPLICINFO pLicInfo)
{
	TraceMethodEnter("COleClassFactory2W::GetLicInfo", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IClassFactory2, GetLicInfo));
	return GetANSI()->GetLicInfo(pLicInfo);
}

STDMETHODIMP CClassFactory2W::RequestLicKey(DWORD dwReserved, BSTR FAR* pbstrKey)
{
	TraceMethodEnter("COleClassFactory2W::RequestLicKey", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IClassFactory2, RequestLicKey));

	HRESULT hResult;
	BSTRA bstrKeyA, *pbstrKeyA;
	pbstrKeyA = (pbstrKey ? &bstrKeyA : NULL);

	hResult = GetANSI()->RequestLicKey(dwReserved, pbstrKeyA);
	if (FAILED(hResult))
		return hResult;

	if (pbstrKeyA)
	{
		hResult = ConvertDispStringToW(bstrKeyA, pbstrKey);
		SysFreeStringA(bstrKeyA);
	}

	return hResult;
}

STDMETHODIMP CClassFactory2W::CreateInstanceLic(LPUNKNOWN pUnkOuter,
		LPUNKNOWN pUnkReserved, REFIID riid, BSTR bstrKey,
		LPVOID FAR* ppvObject)
{
	TraceMethodEnter("COleClassFactory2W::CreateInstanceLic", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IClassFactory2, CreateInstanceLic));

	LPUNKNOWN pUnkOuterA;
	LPUNKNOWN pUnkReservedA;
	LPUNKNOWN pUnkA;
	IDINTERFACE idRef;
	BSTRA bstrKeyA;
	HRESULT hResult;

	hResult = WrapIUnknownAFromW(pUnkOuter, &pUnkOuterA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIUnknownAFromW(pUnkReserved, &pUnkReservedA);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToA(bstrKey, &bstrKeyA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->CreateInstanceLic(pUnkOuterA, pUnkReservedA, riid,
			bstrKeyA, (LPVOID *)&pUnkA);

	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);
	hResult = WrapInterfaceWFromA(idRef, pUnkA, (LPUNKNOWN *)ppvObject);
	pUnkA->Release();

Error:
	if (bstrKeyA)
		ConvertDispStringFreeA(bstrKeyA);

	if (pUnkReservedA)
		pUnkReservedA->Release();

	if (pUnkOuterA)
		pUnkOuterA->Release();

	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSpecifyPropertyPagesW class

STDMETHODIMP CSpecifyPropertyPagesW::GetPages(CAUUID FAR* pPages)
{
	TraceMethodEnter("CSpecifyPropertyPagesW::GetPages", this);
	_DebugHook(GetANSI(), MEMBER_PTR(ISpecifyPropertyPages, GetPages));
	return GetANSI()->GetPages(pPages);
}

/////////////////////////////////////////////////////////////////////////////
// CPerPropertyBrowsingW class

STDMETHODIMP CPerPropertyBrowsingW::GetDisplayString(DISPID dispid,
	BSTR FAR* pbstr)
{
	TraceMethodEnter("CPerPropertyBrowsingW::GetDisplayString", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPerPropertyBrowsing, GetDisplayString));

	HRESULT hResult;
	BSTRA bstrA, *pbstrA;
	pbstrA = (pbstr ? &bstrA : NULL);

	hResult = GetANSI()->GetDisplayString(dispid, pbstrA);
	if (hResult != NOERROR)
	{
		if (pbstr)
			*pbstr = NULL;
		return hResult;
	}

	HRESULT hResultConvert = NOERROR;

	if (pbstrA)
	{
		hResultConvert = ConvertDispStringToW(bstrA, pbstr);
		SysFreeStringA(bstrA);
	}

	return SUCCEEDED(hResultConvert) ? hResult : hResultConvert;
}

STDMETHODIMP CPerPropertyBrowsingW::MapPropertyToPage(DISPID dispid, LPCLSID lpclsid)
{
	TraceMethodEnter("CPerPropertyBrowsingW::MapPropertyToPage", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPerPropertyBrowsing, MapPropertyToPage));
	return GetANSI()->MapPropertyToPage(dispid, lpclsid);
}

STDMETHODIMP CPerPropertyBrowsingW::GetPredefinedStrings(DISPID dispid,
	CALPOLESTR FAR* lpcaStringsOut, CADWORD FAR* lpcaCookiesOut)
{
	TraceMethodEnter("CPerPropertyBrowsingW::GetPredefinedStrings", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPerPropertyBrowsing, GetPredefinedStrings));

	HRESULT hResult;
	CALPSTR caStringsOut;

	hResult = GetANSI()->GetPredefinedStrings(dispid, &caStringsOut, lpcaCookiesOut);
	if (hResult != NOERROR)
		return hResult;

	// Convert string array
	lpcaStringsOut->cElems = caStringsOut.cElems;
	hResult = ConvertStringArrayToW(caStringsOut.pElems, &lpcaStringsOut->pElems,
		caStringsOut.cElems);

	if (FAILED(hResult))
		delete lpcaCookiesOut->pElems;

	ConvertStringArrayFree(caStringsOut.pElems, caStringsOut.cElems);

	return hResult;
}

STDMETHODIMP CPerPropertyBrowsingW::GetPredefinedValue(DISPID dispid,
	DWORD dwCookie, VARIANT FAR* lpvarOut)
{
	TraceMethodEnter("CPerPropertyBrowsingW::GetPredefinedValue", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPerPropertyBrowsing, GetPredefinedValue));

	HRESULT hResult;

	hResult = GetANSI()->GetPredefinedValue(dispid, dwCookie, (LPVARIANTA)lpvarOut);
	if (FAILED(hResult))
		return hResult;

	return ConvertVariantToW(lpvarOut);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageSiteW class

STDMETHODIMP CPropertyPageSiteW::OnStatusChange(DWORD flags)
{
	TraceMethodEnter("CPropertyPageSiteW::OnStatusChange", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPageSite, OnStatusChange));
	return GetANSI()->OnStatusChange(flags);
}

STDMETHODIMP CPropertyPageSiteW::GetLocaleID(LCID FAR* pLocaleID)
{
	TraceMethodEnter("CPropertyPageSiteW::GetLocaleID", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPageSite, GetLocaleID));
	return GetANSI()->GetLocaleID(pLocaleID);
}

STDMETHODIMP CPropertyPageSiteW::GetPageContainer(LPUNKNOWN FAR* ppUnk)
{
	TraceMethodEnter("CPropertyPageSiteW::GetPageContainer", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPageSite, GetPageContainer));

	LPUNKNOWN pUnk;
	HRESULT hResult;

	hResult = GetANSI()->GetPageContainer(&pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIUnknownWFromA(pUnk, ppUnk);
	pUnk->Release();
	return hResult;
}

STDMETHODIMP CPropertyPageSiteW::TranslateAccelerator(LPMSG lpMsg)
{
	TraceMethodEnter("CPropertyPageSiteW::TranslateAccelerator", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPageSite, TranslateAccelerator));
	return GetANSI()->TranslateAccelerator(lpMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyPage2W class

STDMETHODIMP CPropertyPage2W::SetPageSite(LPPROPERTYPAGESITE pPageSite)
{
	TraceMethodEnter("CPropertyPage2W::SetPageSite", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, SetPageSite));

	LPPROPERTYPAGESITEA pPageSiteA;
	HRESULT hResult;

	WrapIPropertyPageSiteAFromW(pPageSite, &pPageSiteA);
	hResult = GetANSI()->SetPageSite(pPageSiteA);

	pPageSiteA->Release();
	return hResult;
}

STDMETHODIMP CPropertyPage2W::Activate(HWND hwndParent, LPCRECT rect,
			BOOL bModal)
{
	TraceMethodEnter("CPropertyPage2W::Activate", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, Activate));
	return GetANSI()->Activate(hwndParent, rect, bModal);
}

STDMETHODIMP CPropertyPage2W::Deactivate()
{
	TraceMethodEnter("CPropertyPage2W::Deactivate", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, Deactivate));
	return GetANSI()->Deactivate();
}

STDMETHODIMP CPropertyPage2W::GetPageInfo(LPPROPPAGEINFO pPageInfo)
{
	TraceMethodEnter("CPropertyPage2W::GetPageInfo", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, GetPageInfo));

	HRESULT hResult;
	PROPPAGEINFOA pageInfoA;

	hResult = GetANSI()->GetPageInfo(&pageInfoA);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertPROPPAGEINFOToW(&pageInfoA, pPageInfo);
	ConvertPROPPAGEINFOFree(&pageInfoA);
	return hResult;
}

STDMETHODIMP CPropertyPage2W::SetObjects(ULONG cObjects, LPUNKNOWN FAR* ppUnk)
{
	TraceMethodEnter("CPropertyPage2W::SetObjects", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, SetObjects));

	HRESULT hResult;
	LPUNKNOWN FAR* ppUnkA = new LPUNKNOWN[cObjects];

	ULONG i;
	for (i = 0; i < cObjects; i++)
		ppUnkA[i] = NULL;

	for (i = 0; i < cObjects; i++)
	{
		hResult = WrapIUnknownAFromW(ppUnk[i], &ppUnkA[i]);
		if (FAILED(hResult))
			goto Error;
	}

	hResult = GetANSI()->SetObjects(cObjects, ppUnkA);

Error:
	for (i = 0; i < cObjects; i++)
		if (ppUnkA[i] != NULL)
			ppUnkA[i]->Release();
	delete [] ppUnkA;

	return hResult;
}

STDMETHODIMP CPropertyPage2W::Show(UINT nCmdShow)
{
	TraceMethodEnter("CPropertyPage2W::Show", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, Show));
	return GetANSI()->Show(nCmdShow);
}

STDMETHODIMP CPropertyPage2W::Move(LPCRECT prect)
{
	TraceMethodEnter("CPropertyPage2W::Move", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, Move));
	return GetANSI()->Move(prect);
}

STDMETHODIMP CPropertyPage2W::IsPageDirty()
{
	TraceMethodEnter("CPropertyPage2W::IsPageDirty", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, IsPageDirty));
	return GetANSI()->IsPageDirty();
}

STDMETHODIMP CPropertyPage2W::Apply()
{
	TraceMethodEnter("CPropertyPage2W::Apply", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, Apply));
	return GetANSI()->Apply();
}

STDMETHODIMP CPropertyPage2W::Help(LPCOLESTR lpszHelpDir)
{
	TraceMethodEnter("CPropertyPage2W::Help", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, Help));

	char szHelpDirA[MAX_STRING];
	if (lpszHelpDir != NULL)
		ConvertStringToA(lpszHelpDir, szHelpDirA);
	else
		szHelpDirA[0] = '\0';

	return GetANSI()->Help(szHelpDirA);
}

STDMETHODIMP CPropertyPage2W::TranslateAccelerator(LPMSG lpMsg)
{
	TraceMethodEnter("CPropertyPage2W::TranslateAccelerator", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, TranslateAccelerator));
	return GetANSI()->TranslateAccelerator(lpMsg);
}

STDMETHODIMP CPropertyPage2W::EditProperty(DISPID dispid)
{
	TraceMethodEnter("CPropertyPage2W::EditProperty", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPropertyPage2, EditProperty));
	return GetANSI()->EditProperty(dispid);
}


/////////////////////////////////////////////////////////////////////////////
// CFontW class

STDMETHODIMP CFontW::get_Name(BSTR FAR* pname)
{
	TraceMethodEnter("CFontW::get_Name", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Name));

	HRESULT hResult;
	BSTRA bstrA, *pbstrA;
	pbstrA = (pname ? &bstrA : NULL);

	hResult = GetANSI()->get_Name(pbstrA);
	if (FAILED(hResult))
		return hResult;

	if (pbstrA)
	{
		hResult = ConvertDispStringToW(bstrA, pname);
		SysFreeStringA(bstrA);
	}

	return hResult;
}

STDMETHODIMP CFontW::put_Name(BSTR name)
{
	TraceMethodEnter("CFontW::put_Name", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Name));

	HRESULT hResult;
	BSTRA bstrA;

	hResult = ConvertDispStringToA(name, &bstrA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->put_Name(bstrA);
	ConvertDispStringFreeA(bstrA);
	return hResult;
}

STDMETHODIMP CFontW::get_Size(CY FAR* psize)
{
	TraceMethodEnter("CFontW::get_Size", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Size));
	return GetANSI()->get_Size(psize);
}

STDMETHODIMP CFontW::put_Size(CY size)
{
	TraceMethodEnter("CFontW::put_Size", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Size));
	return GetANSI()->put_Size(size);
}

STDMETHODIMP CFontW::get_Bold(BOOL FAR* pbold)
{
	TraceMethodEnter("CFontW::get_Bold", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Bold));
	return GetANSI()->get_Bold(pbold);
}

STDMETHODIMP CFontW::put_Bold(BOOL bold)
{
	TraceMethodEnter("CFontW::put_Bold", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Bold));
	return GetANSI()->put_Bold(bold);
}

STDMETHODIMP CFontW::get_Italic(BOOL FAR* pitalic)
{
	TraceMethodEnter("CFontW::get_Italic", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Italic));
	return GetANSI()->get_Italic(pitalic);
}

STDMETHODIMP CFontW::put_Italic(BOOL italic)
{
	TraceMethodEnter("CFontW::put_Italic", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Italic));
	return GetANSI()->put_Italic(italic);
}

STDMETHODIMP CFontW::get_Underline(BOOL FAR* punderline)
{
	TraceMethodEnter("CFontW::get_Underline", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Underline));
	return GetANSI()->get_Underline(punderline);
}

STDMETHODIMP CFontW::put_Underline(BOOL underline)
{
	TraceMethodEnter("CFontW::put_Underline", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Underline));
	return GetANSI()->put_Underline(underline);
}

STDMETHODIMP CFontW::get_Strikethrough(BOOL FAR* pstrikethrough)
{
	TraceMethodEnter("CFontW::get_Strikethrough", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Strikethrough));
	return GetANSI()->get_Strikethrough(pstrikethrough);
}

STDMETHODIMP CFontW::put_Strikethrough(BOOL strikethrough)
{
	TraceMethodEnter("CFontW::put_Strikethrough", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Strikethrough));
	return GetANSI()->put_Strikethrough(strikethrough);
}

STDMETHODIMP CFontW::get_Weight(short FAR* pweight)
{
	TraceMethodEnter("CFontW::get_Weight", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Weight));
	return GetANSI()->get_Weight(pweight);
}

STDMETHODIMP CFontW::put_Weight(short weight)
{
	TraceMethodEnter("CFontW::put_Weight", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Weight));
	return GetANSI()->put_Weight(weight);
}

STDMETHODIMP CFontW::get_Charset(short FAR* pcharset)
{
	TraceMethodEnter("CFontW::get_Charset", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_Charset));
	return GetANSI()->get_Charset(pcharset);
}

STDMETHODIMP CFontW::put_Charset(short charset)
{
	TraceMethodEnter("CFontW::put_Charset", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, put_Charset));
	return GetANSI()->put_Charset(charset);
}

STDMETHODIMP CFontW::get_hFont(HFONT FAR* phfont)
{
	TraceMethodEnter("CFontW::get_hFont", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, get_hFont));
	return GetANSI()->get_hFont(phfont);
}

STDMETHODIMP CFontW::Clone(IFont FAR* FAR* lplpFont)
{
	TraceMethodEnter("CFontW::Clone", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, Clone));

	LPFONTA lpFont;
	HRESULT hResult;

	hResult = GetANSI()->Clone(&lpFont);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIFontWFromA(lpFont, lplpFont);
	lpFont->Release();
	return hResult;
}

STDMETHODIMP CFontW::IsEqual(IFont FAR* lpFontOther)
{
	TraceMethodEnter("CFontW::IsEqual", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, IsEqual));

	HRESULT hResult;
	LPFONTA lpFontOtherA;
	hResult = WrapIFontAFromW(lpFontOther, &lpFontOtherA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->IsEqual(lpFontOtherA);
	lpFontOtherA->Release();

	return hResult;
}

STDMETHODIMP CFontW::SetRatio(long cyLogical, long cyHimetric)
{
	TraceMethodEnter("CFontW::SetRatio", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, SetRatio));
	return GetANSI()->SetRatio(cyLogical, cyHimetric);
}

STDMETHODIMP CFontW::QueryTextMetrics(LPTEXTMETRICW lptmW)
{
	TraceMethodEnter("CFontW::QueryTextMetrics", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, QueryTextMetrics));

	HRESULT hResult;
	TEXTMETRICA tmA;

	hResult = GetANSI()->QueryTextMetrics(&tmA);
	if (FAILED(hResult))
	{
		memset(lptmW, 0, sizeof(LPTEXTMETRICW));
		return hResult;
	}

	// Convert from TEXTMETRICA to TEXTMETRICW
	memcpy(lptmW, &tmA, sizeof(LONG) * 11);
	memcpy(&lptmW->tmItalic, &tmA.tmItalic, sizeof(BYTE) * 5);
	MultiByteToWideChar(CP_ACP, 0, (LPSTR)&tmA.tmFirstChar, 1, &lptmW->tmFirstChar, 1);
	MultiByteToWideChar(CP_ACP, 0, (LPSTR)&tmA.tmLastChar, 1, &lptmW->tmLastChar, 1);
	MultiByteToWideChar(CP_ACP, 0, (LPSTR)&tmA.tmDefaultChar, 1, &lptmW->tmDefaultChar, 1);
	MultiByteToWideChar(CP_ACP, 0, (LPSTR)&tmA.tmBreakChar, 1, &lptmW->tmBreakChar, 1);

	return hResult;
}

STDMETHODIMP CFontW::AddRefHfont(HFONT hfont)
{
	TraceMethodEnter("CFontW::AddRefHfont", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, AddRefHfont));
	return GetANSI()->AddRefHfont(hfont);
}

STDMETHODIMP CFontW::ReleaseHfont(HFONT hfont)
{
	TraceMethodEnter("CFontW::ReleaseHfont", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IFont, ReleaseHfont));
	return GetANSI()->ReleaseHfont(hfont);
}


/////////////////////////////////////////////////////////////////////////////
// CPictureW class

STDMETHODIMP CPictureW::get_Handle(OLE_HANDLE FAR* phandle)
{
	TraceMethodEnter("CPictureW::get_Handle", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_Handle));
	return GetANSI()->get_Handle(phandle);
}

STDMETHODIMP CPictureW::get_hPal(OLE_HANDLE FAR* phpal)
{
	TraceMethodEnter("CPictureW::get_hPal", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_hPal));
	return GetANSI()->get_hPal(phpal);
}

STDMETHODIMP CPictureW::get_Type(short FAR* ptype)
{
	TraceMethodEnter("CPictureW::get_Type", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_Type));
	return GetANSI()->get_Type(ptype);
}

STDMETHODIMP CPictureW::get_Width(OLE_XSIZE_HIMETRIC FAR* pwidth)
{
	TraceMethodEnter("CPictureW::get_Width", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_Width));
	return GetANSI()->get_Width(pwidth);
}

STDMETHODIMP CPictureW::get_Height(OLE_YSIZE_HIMETRIC FAR* pheight)
{
	TraceMethodEnter("CPictureW::get_Height", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_Height));
	return GetANSI()->get_Height(pheight);
}

STDMETHODIMP CPictureW::Render(HDC hdc, long x, long y, long cx, long cy,
	OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
	OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
	LPRECT lprcWBounds)
{
	TraceMethodEnter("CPictureW::Render", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, Render));
	return GetANSI()->Render(hdc, x, y, cx, cy, xSrc, ySrc, cxSrc, cySrc,
		lprcWBounds);
}

STDMETHODIMP CPictureW::set_hPal(OLE_HANDLE hpal)
{
	TraceMethodEnter("CPictureW::set_hPal", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, set_hPal));
	return GetANSI()->set_hPal(hpal);
}

STDMETHODIMP CPictureW::get_CurDC(HDC FAR * phdcOut)
{
	TraceMethodEnter("CPictureW::get_CurDC", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_CurDC));
	return GetANSI()->get_CurDC(phdcOut);
}

STDMETHODIMP CPictureW::SelectPicture(
	HDC hdcIn, HDC FAR * phdcOut, OLE_HANDLE FAR * phbmpOut)
{
	TraceMethodEnter("CPictureW::SelectPicture", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, SelectPicture));
	return GetANSI()->SelectPicture(hdcIn, phdcOut, phbmpOut);
}

STDMETHODIMP CPictureW::get_KeepOriginalFormat(BOOL * pfkeep)
{
	TraceMethodEnter("CPictureW::get_KeepOriginalFormat", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_KeepOriginalFormat));
	return GetANSI()->get_KeepOriginalFormat(pfkeep);
}

STDMETHODIMP CPictureW::put_KeepOriginalFormat(BOOL fkeep)
{
	TraceMethodEnter("CPictureW::put_KeepOriginalFormat", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, put_KeepOriginalFormat));
	return GetANSI()->put_KeepOriginalFormat(fkeep);
}

STDMETHODIMP CPictureW::PictureChanged()
{
	TraceMethodEnter("CPictureW::PictureChanged", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, PictureChanged));
	return GetANSI()->PictureChanged();
}

STDMETHODIMP CPictureW::SaveAsFile(LPSTREAM pStm, BOOL fSaveMemCopy,
	LONG FAR * lpcbSize)
{
	TraceMethodEnter("CPictureW::SaveAsFile", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, SaveAsFile));

	LPSTREAMA pStmA;
	HRESULT hResult;

	hResult = WrapIStreamAFromW(pStm, &pStmA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SaveAsFile(pStmA, fSaveMemCopy, lpcbSize);
	pStmA->Release();
	return hResult;
}

STDMETHODIMP CPictureW::get_Attributes(DWORD FAR * lpdwAttr)
{
	TraceMethodEnter("CPictureW::get_Attributes", this);
	_DebugHook(GetANSI(), MEMBER_PTR(IPicture, get_Attributes));
	return GetANSI()->get_Attributes(lpdwAttr);
}
