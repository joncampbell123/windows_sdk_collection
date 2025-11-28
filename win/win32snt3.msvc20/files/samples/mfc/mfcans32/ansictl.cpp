//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       AnsiCtl.cpp
//
//  Contents:   ANSI Wrappers for OLE Control Interfaces and APIs.
//
//  History:    01-Jun-94   johnels     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"


/////////////////////////////////////////////////////////////////////////////
// Get an entry point for the OLE Controls DLL

HINSTANCE _hOleControlDll = (HINSTANCE)-1;

#ifdef _DEBUG
#define OC_DLL "oc30d.dll"
#else
#define OC_DLL "oc30.dll"
#endif

HRESULT GetEntryPoint(FARPROC* lplpfn, LPSTR pszFunction)
{
	if (*lplpfn == NULL)
	{
		if (_hOleControlDll == (HINSTANCE)-1)
			_hOleControlDll = LoadLibrary(OC_DLL);

		if (_hOleControlDll != NULL)
			*lplpfn = GetProcAddress(_hOleControlDll, pszFunction);

#ifdef _DEBUG
		if (*lplpfn == NULL)
		{
			char buf[MAX_STRING];
			wsprintf(buf, "%s: Entry point not found!", pszFunction);
		}
#endif
	}

	return (*lplpfn != NULL) ? NOERROR : ResultFromScode(E_UNEXPECTED);
}


/////////////////////////////////////////////////////////////////////////////
// Property sheet APIs

STDAPI OleCreatePropertyFrameA(HWND hwndOwner, UINT x, UINT y,
	LPCSTR lpszCaptionA, ULONG cObjects, LPUNKNOWN FAR* ppUnkA,
	ULONG cPages, LPCLSID pPageClsID, LCID lcid,
	DWORD dwReserved, LPVOID pvReserved)
{
	TraceSTDAPIEnter("OleCreatePropertyFrameA");

	OCPFIPARAMSA ocpfiA;
	ocpfiA.cbStructSize = sizeof(OCPFIPARAMSA);
	ocpfiA.hWndOwner = hwndOwner;
	ocpfiA.x = x;
	ocpfiA.y = y;
	ocpfiA.lpszCaption = (LPSTR)lpszCaptionA;
	ocpfiA.cObjects = cObjects;
	ocpfiA.lplpUnk = ppUnkA;
	ocpfiA.cPages = cPages;
	ocpfiA.lpPages = pPageClsID;
	ocpfiA.lcid = lcid;
	ocpfiA.dispidInitialProperty = DISPID_UNKNOWN;

	return OleCreatePropertyFrameIndirectA(&ocpfiA);
}

STDAPI OleCreatePropertyFrameIndirectA(LPOCPFIPARAMSA lpParams)
{
	static HRESULT (FAR STDAPICALLTYPE * _lpfnOleCreatePropertyFrameIndirect)(
		LPOCPFIPARAMS) = NULL;

	TraceSTDAPIEnter("OleCreatePropertyFrameIndirectA");

	HRESULT hResult;

	WCHAR szCaption[MAX_STRING];
	if (lpParams->lpszCaption != NULL)
		ConvertStringToW(lpParams->lpszCaption, szCaption);
	else
		szCaption[0] = L'\0';

	ULONG cObjects = lpParams->cObjects;
	LPUNKNOWN FAR* ppUnk = new LPUNKNOWN[cObjects];

	ULONG i;
	for (i = 0; i < cObjects; i++)
		ppUnk[i] = NULL;

	for (i = 0; i < cObjects; i++)
	{
		hResult = WrapIUnknownWFromA(lpParams->lplpUnk[i], &ppUnk[i]);
		if (FAILED(hResult))
			goto Error;
	}

	OCPFIPARAMS ocpfi;
	memcpy(&ocpfi, lpParams, lpParams->cbStructSize);
	ocpfi.lpszCaption = szCaption;
	ocpfi.lplpUnk = ppUnk;

	hResult = GetEntryPoint((FARPROC*)&_lpfnOleCreatePropertyFrameIndirect,
		"OleCreatePropertyFrameIndirect");

	if (FAILED(hResult))
		goto Error;

	hResult = (*_lpfnOleCreatePropertyFrameIndirect)(&ocpfi);

Error:
	for (i = 0; i < cObjects; i++)
		if (ppUnk[i] != NULL)
			ppUnk[i]->Release();
	delete [] ppUnk;

	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// Font APIs

STDAPI OleCreateFontIndirectA(LPFONTDESCA lpfd,
		REFIID riid, LPVOID FAR* lplpvObj)
{
	static HRESULT (FAR STDAPICALLTYPE * _lpfnOleCreateFontIndirect)(
			LPFONTDESC, REFIID, LPVOID FAR*) = NULL;

	TraceSTDAPIEnter("OleCreateFontIndirectA");

	HRESULT hResult;
	LPUNKNOWN pUnk;
	IDINTERFACE idRef;

	WCHAR szName[MAX_STRING];
	if (lpfd->lpstrName != NULL)
		ConvertStringToW(lpfd->lpstrName, szName);
	else
		szName[0] = L'\0';

	FONTDESC fontDesc;
	memcpy(&fontDesc, lpfd, lpfd->cbSizeofstruct);
	fontDesc.lpstrName = szName;

	hResult = GetEntryPoint((FARPROC*)&_lpfnOleCreateFontIndirect,
		"OleCreateFontIndirect");

	if (FAILED(hResult))
		return hResult;

	hResult = (*_lpfnOleCreateFontIndirect)(&fontDesc, riid,
		(LPVOID FAR*)&pUnk);

	if (FAILED(hResult))
		return hResult;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);
	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)lplpvObj);
	pUnk->Release();

	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyNotifySinkA class

STDMETHODIMP CPropertyNotifySinkA::OnChanged(DISPID dispid)
{
	TraceMethodEnter("CPropertyNotifySinkA::OnChanged", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyNotifySink, OnChanged));
	return GetWide()->OnChanged(dispid);
}

STDMETHODIMP CPropertyNotifySinkA::OnRequestEdit(DISPID dispid)
{
	TraceMethodEnter("CPropertyNotifySinkA::OnRequestEdit", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyNotifySink, OnRequestEdit));
	return GetWide()->OnRequestEdit(dispid);
}


/////////////////////////////////////////////////////////////////////////////
// CProvideClassInfoA class

STDMETHODIMP CProvideClassInfoA::GetClassInfo(LPTYPEINFOA FAR* ppTI)
{
	TraceMethodEnter("CProvideClassInfoA::GetClassInfo", this);
	_DebugHook(GetWide(), MEMBER_PTR(IProvideClassInfo, GetClassInfo));

	LPTYPEINFO pTypeInfo;
	HRESULT hResult;

	hResult = GetWide()->GetClassInfo(&pTypeInfo);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapITypeInfoAFromW(pTypeInfo, ppTI);
	pTypeInfo->Release();
	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// CConnectionPointContainerA class

STDMETHODIMP CConnectionPointContainerA::EnumConnectionPoints(
			LPENUMCONNECTIONPOINTSA FAR* ppEnum)
{
	TraceMethodEnter("CConnectionPointContainerA::EnumConnectionPoints", this);
	_DebugHook(GetWide(), MEMBER_PTR(IConnectionPointContainer, EnumConnectionPoints));

	LPENUMCONNECTIONPOINTS pEnumConnPts;
	HRESULT hResult;

	hResult = GetWide()->EnumConnectionPoints(&pEnumConnPts);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionPointsAFromW(pEnumConnPts, ppEnum);
	pEnumConnPts->Release();
	return hResult;
}

STDMETHODIMP CConnectionPointContainerA::FindConnectionPoint(
			REFIID iid, LPCONNECTIONPOINTA FAR* ppCP)
{
	TraceMethodEnter("CConnectionPointContainerA::FindConnectionPoint", this);
	_DebugHook(GetWide(), MEMBER_PTR(IConnectionPointContainer, FindConnectionPoint));

	LPCONNECTIONPOINT pConnPt;
	HRESULT hResult;

	hResult = GetWide()->FindConnectionPoint(iid, &pConnPt);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIConnectionPointAFromW(pConnPt, ppCP);
	pConnPt->Release();
	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionPointsA class

STDMETHODIMP CEnumConnectionPointsA::Next(
			ULONG cConnections,
			LPCONNECTIONPOINTA FAR* rgpcn, ULONG FAR* lpcFetched)
{
	TraceMethodEnter("CEnumConnectionPointsA::Next", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnectionPoints, Next));

	LPCONNECTIONPOINT FAR* rgpConnPt = new LPCONNECTIONPOINT[cConnections];
	HRESULT hResult;

	hResult = GetWide()->Next(cConnections, rgpConnPt, lpcFetched);
	if (FAILED(hResult))
	{
		delete [] rgpConnPt;
		return hResult;
	}

	ULONG i;
	for (i = 0; i < *lpcFetched; i++)   // Wrap each pointer in array
	{
		hResult = WrapIConnectionPointAFromW(rgpConnPt[i], &rgpcn[i]);
		rgpConnPt[i]->Release();
	}

	delete [] rgpConnPt;
	return hResult;
}

STDMETHODIMP CEnumConnectionPointsA::Skip(ULONG cConnections)
{
	TraceMethodEnter("CEnumConnectionPointsA::Skip", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnectionPoints, Skip));
	return GetWide()->Skip(cConnections);
}

STDMETHODIMP CEnumConnectionPointsA::Reset()
{
	TraceMethodEnter("CEnumConnectionPointsA::Reset", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnectionPoints, Reset));
	return GetWide()->Reset();
}

STDMETHODIMP CEnumConnectionPointsA::Clone(LPENUMCONNECTIONPOINTSA FAR* ppEnum)
{
	TraceMethodEnter("CEnumConnectionPointsA::Clone", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnectionPoints, Clone));

	LPENUMCONNECTIONPOINTS pEnumConnPts;
	HRESULT hResult;

	hResult = GetWide()->Clone(&pEnumConnPts);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionPointsAFromW(pEnumConnPts, ppEnum);
	pEnumConnPts->Release();
	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// CConnectionPointA class

STDMETHODIMP CConnectionPointA::GetConnectionInterface(IID FAR* pIID)
{
	TraceMethodEnter("CConnectionPointA::GetConnectionInterface", this);
	_DebugHook(GetWide(), MEMBER_PTR(IConnectionPoint, GetConnectionInterface));
	return GetWide()->GetConnectionInterface(pIID);
}

STDMETHODIMP CConnectionPointA::GetConnectionPointContainer(
			LPCONNECTIONPOINTCONTAINERA FAR* ppCPC)
{
	TraceMethodEnter("CConnectionPointA::GetConnectionPointContainer", this);
	_DebugHook(GetWide(), MEMBER_PTR(IConnectionPoint, GetConnectionPointContainer));

	LPCONNECTIONPOINTCONTAINER pConnPtCont;
	HRESULT hResult;

	hResult = GetWide()->GetConnectionPointContainer(&pConnPtCont);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIConnectionPointContainerAFromW(pConnPtCont, ppCPC);
	pConnPtCont->Release();
	return hResult;
}

STDMETHODIMP CConnectionPointA::Advise(LPUNKNOWN pUnkSinkA, DWORD FAR* pdwCookie)
{
	TraceMethodEnter("CConnectionPointA::Advise", this);
	_DebugHook(GetWide(), MEMBER_PTR(IConnectionPoint, Advise));

	LPUNKNOWN pUnkSink;
	HRESULT hResult;

	hResult = WrapIUnknownWFromA(pUnkSinkA, &pUnkSink);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Advise(pUnkSink, pdwCookie);
	pUnkSink->Release();
	return hResult;
}

STDMETHODIMP CConnectionPointA::Unadvise(DWORD dwCookie)
{
	TraceMethodEnter("CConnectionPointA::Unadvise", this);
	_DebugHook(GetWide(), MEMBER_PTR(IConnectionPoint, Unadvise));
	return GetWide()->Unadvise(dwCookie);
}

STDMETHODIMP CConnectionPointA::EnumConnections(LPENUMCONNECTIONSA FAR* ppEnum)
{
	TraceMethodEnter("CConnectionPointA::EnumConnections", this);
	_DebugHook(GetWide(), MEMBER_PTR(IConnectionPoint, EnumConnections));

	LPENUMCONNECTIONS pEnumConns;
	HRESULT hResult;

	hResult = GetWide()->EnumConnections(&pEnumConns);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionsAFromW(pEnumConns, ppEnum);
	pEnumConns->Release();
	return hResult;
}


/////////////////////////////////////////////////////////////////////////////
// CEnumConnectionsA class

STDMETHODIMP CEnumConnectionsA::Next(
			ULONG cConnections,
			LPCONNECTDATAA rgcd, ULONG FAR* lpcFetched)
{
	TraceMethodEnter("CEnumConnectionsA::Next", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnections, Next));

	LPCONNECTDATA rgConnData = new CONNECTDATA[cConnections];
	HRESULT hResult;

	hResult = GetWide()->Next(cConnections, rgConnData, lpcFetched);
	if (FAILED(hResult))
	{
		delete [] rgConnData;
		return hResult;
	}

	ULONG i;
	for (i = 0; i < *lpcFetched; i++)   // Wrap each pointer in array
	{
		rgcd[i].dwCookie = rgConnData[i].dwCookie;
		hResult = WrapIUnknownAFromW(rgConnData[i].pUnk, &rgcd[i].pUnk);
		rgConnData[i].pUnk->Release();
	}

	delete [] rgConnData;
	return hResult;
}

STDMETHODIMP CEnumConnectionsA::Skip(ULONG cConnections)
{
	TraceMethodEnter("CEnumConnectionPointsA::Skip", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnectionPoints, Skip));
	return GetWide()->Skip(cConnections);
}

STDMETHODIMP CEnumConnectionsA::Reset()
{
	TraceMethodEnter("CEnumConnectionPointsA::Reset", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnectionPoints, Reset));
	return GetWide()->Reset();
}

STDMETHODIMP CEnumConnectionsA::Clone(LPENUMCONNECTIONSA FAR* ppEnum)
{
	TraceMethodEnter("CEnumConnectionsA::Clone", this);
	_DebugHook(GetWide(), MEMBER_PTR(IEnumConnections, Clone));

	LPENUMCONNECTIONS pEnumConns;
	HRESULT hResult;

	hResult = GetWide()->Clone(&pEnumConns);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumConnectionsAFromW(pEnumConns, ppEnum);
	pEnumConns->Release();
	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// COleControlA class

STDMETHODIMP COleControlA::GetControlInfo(LPCONTROLINFO pCI)
{
	TraceMethodEnter("COleControlA::GetControlInfo", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControl, GetControlInfo));
	return GetWide()->GetControlInfo(pCI);
}

STDMETHODIMP COleControlA::OnMnemonic(LPMSG pMsg)
{
	TraceMethodEnter("COleControlA::OnMnemonic", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControl, OnMnemonic));
	return GetWide()->OnMnemonic(pMsg);
}

STDMETHODIMP COleControlA::OnAmbientPropertyChange(DISPID dispid)
{
	TraceMethodEnter("COleControlA::OnAmbientPropertyChange", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControl, OnAmbientPropertyChange));
	return GetWide()->OnAmbientPropertyChange(dispid);
}

STDMETHODIMP COleControlA::FreezeEvents(BOOL bFreeze)
{
	TraceMethodEnter("COleControlA::FreezeEvents", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControl, FreezeEvents));
	return GetWide()->FreezeEvents(bFreeze);
}

/////////////////////////////////////////////////////////////////////////////
// COleControlSiteA class

STDMETHODIMP COleControlSiteA::OnControlInfoChanged()
{
	TraceMethodEnter("COleControlSiteA::OnControlInfoChanged", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControlSite, OnControlInfoChanged));
	return GetWide()->OnControlInfoChanged();
}

STDMETHODIMP COleControlSiteA::LockInPlaceActive(BOOL fLock)
{
	TraceMethodEnter("COleControlSiteA::LockInPlaceActive", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControlSite, LockInPlaceActive));
	return GetWide()->LockInPlaceActive(fLock);
}

STDMETHODIMP COleControlSiteA::GetExtendedControl(LPDISPATCHA FAR* ppDisp)
{
	TraceMethodEnter("COleControlSiteA::GetExtendedControl", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControlSite, GetExtendedControl));

	LPDISPATCH pDisp;
	HRESULT hResult;

	hResult = GetWide()->GetExtendedControl(&pDisp);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIDispatchAFromW(pDisp, ppDisp);
	pDisp->Release();
	return hResult;
}

STDMETHODIMP COleControlSiteA::TransformCoords(POINTL FAR* lpptlHimetric,
	POINTF FAR* lpptfContainer, DWORD flags)
{
	TraceMethodEnter("COleControlSiteA::TransformCoords", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControlSite, TransformCoords));
	return GetWide()->TransformCoords(lpptlHimetric, lpptfContainer, flags);
}

STDMETHODIMP COleControlSiteA::TranslateAccelerator(LPMSG lpMsg,
	DWORD grfModifiers)
{
	TraceMethodEnter("COleControlSiteA::TranslateAccelerator", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControlSite, TranslateAccelerator));
	return GetWide()->TranslateAccelerator(lpMsg, grfModifiers);
}

STDMETHODIMP COleControlSiteA::OnFocus(BOOL fGotFocus)
{
	TraceMethodEnter("COleControlSiteA::OnFocus", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControlSite, OnFocus));
	return GetWide()->OnFocus(fGotFocus);
}

STDMETHODIMP COleControlSiteA::ShowPropertyFrame()
{
	TraceMethodEnter("COleControlSiteA::ShowPropertyFrame", this);
	_DebugHook(GetWide(), MEMBER_PTR(IOleControlSite, ShowPropertyFrame));
	return GetWide()->ShowPropertyFrame();
}


/////////////////////////////////////////////////////////////////////////////
// CSimpleFrameSiteA class

STDMETHODIMP CSimpleFrameSiteA::PreMessageFilter(HWND hwnd, UINT msg,
	WPARAM wp, LPARAM lp, LRESULT FAR* lplResult, DWORD FAR * lpdwCookie)
{
	TraceMethodEnter("CSimpleFrameSiteA::PreMessageFilter", this);
	_DebugHook(GetWide(), MEMBER_PTR(ISimpleFrameSite, PreMessageFilter));
	return GetWide()->PreMessageFilter(hwnd, msg, wp, lp, lplResult, lpdwCookie);
}

STDMETHODIMP CSimpleFrameSiteA::PostMessageFilter(HWND hwnd, UINT msg,
	WPARAM wp, LPARAM lp, LRESULT FAR* lplResult, DWORD dwCookie)
{
	TraceMethodEnter("CSimpleFrameSiteA::PostMessageFilter", this);
	_DebugHook(GetWide(), MEMBER_PTR(ISimpleFrameSite, PostMessageFilter));
	return GetWide()->PostMessageFilter(hwnd, msg, wp, lp, lplResult, dwCookie);
}


/////////////////////////////////////////////////////////////////////////////
// CPersistStreamInitA class

STDMETHODIMP CPersistStreamInitA::GetClassID(LPCLSID lpClassID)
{
	TraceMethodEnter("CPersistStreamInitA::GetClassID", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStreamInit, GetClassID));
	return GetWide()->GetClassID(lpClassID);
}

STDMETHODIMP CPersistStreamInitA::IsDirty()
{
	TraceMethodEnter("CPersistStreamInitA::IsDirty", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStreamInit, IsDirty));
	return GetWide()->IsDirty();
}

STDMETHODIMP CPersistStreamInitA::Load(LPSTREAMA pStmA)
{
	TraceMethodEnter("CPersistStreamInitA::Load", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStreamInit, Load));

	LPSTREAM pStm;
	HRESULT hResult;

	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Load(pStm);
	pStm->Release();
	return hResult;
}

STDMETHODIMP CPersistStreamInitA::Save(LPSTREAMA pStmA, BOOL fClearDirty)
{
	TraceMethodEnter("CPersistStreamInitA::Save", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStreamInit, Save));

	LPSTREAM pStm;
	HRESULT hResult;

	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Save(pStm, fClearDirty);
	pStm->Release();
	return hResult;
}

STDMETHODIMP CPersistStreamInitA::GetSizeMax(ULARGE_INTEGER FAR* pcbSize)
{
	TraceMethodEnter("CPersistStreamInitA::GetSizeMax", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStreamInit, GetSizeMax));
	return GetWide()->GetSizeMax(pcbSize);
}

STDMETHODIMP CPersistStreamInitA::InitNew()
{
	TraceMethodEnter("CPersistStreamInitA::InitNew", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPersistStreamInit, InitNew));
	return GetWide()->InitNew();
}

/////////////////////////////////////////////////////////////////////////////
// CClassFactory2A class

STDMETHODIMP CClassFactory2A::CreateInstance(LPUNKNOWN pUnkOuterA,
		  REFIID riid, LPVOID FAR* ppvObject)
{
	TraceMethodEnter("CClassFactory2A::CreateInstance", this);
	_DebugHook(GetWide(), MEMBER_PTR(IClassFactory2, CreateInstance));

	LPUNKNOWN pUnkOuter;
	LPUNKNOWN pUnk;
	IDINTERFACE idRef;
	HRESULT hResult;

	hResult = WrapIUnknownWFromA(pUnkOuterA, &pUnkOuter);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->CreateInstance(pUnkOuter, riid, (LPVOID *)&pUnk);
	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);
	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObject);
	pUnk->Release();

Error:
	if (pUnkOuter)
		pUnkOuter->Release();

	return hResult;
}


STDMETHODIMP CClassFactory2A::LockServer(BOOL fLock)
{
	TraceNotify("CClassFactory2A::LockServer");
	_DebugHook(GetWide(), MEMBER_PTR(IClassFactory2, LockServer));
	return GetWide()->LockServer(fLock);
}

STDMETHODIMP CClassFactory2A::GetLicInfo(LPLICINFO pLicInfo)
{
	TraceMethodEnter("COleClassFactory2A::GetLicInfo", this);
	_DebugHook(GetWide(), MEMBER_PTR(IClassFactory2, GetLicInfo));
	return GetWide()->GetLicInfo(pLicInfo);
}

STDMETHODIMP CClassFactory2A::RequestLicKey(DWORD dwReserved, BSTRA FAR* pbstrKeyA)
{
	TraceMethodEnter("COleClassFactory2A::RequestLicKey", this);
	_DebugHook(GetWide(), MEMBER_PTR(IClassFactory2, RequestLicKey));

	HRESULT hResult;
	BSTR bstrKey, *pbstrKey;
	pbstrKey = (pbstrKeyA ? &bstrKey : NULL);

	hResult = GetWide()->RequestLicKey(dwReserved, pbstrKey);
	if (FAILED(hResult))
		return hResult;

	if (pbstrKey)
	{
		hResult = ConvertDispStringToA(bstrKey, pbstrKeyA);
		SysFreeString(bstrKey);
	}

	return hResult;
}

STDMETHODIMP CClassFactory2A::CreateInstanceLic(LPUNKNOWN pUnkOuterA,
		LPUNKNOWN pUnkReservedA, REFIID riid, BSTRA bstrKeyA,
		LPVOID FAR* ppvObject)
{
	TraceMethodEnter("COleClassFactory2A::CreateInstanceLic", this);
	_DebugHook(GetWide(), MEMBER_PTR(IClassFactory2, CreateInstanceLic));

	LPUNKNOWN pUnkOuter;
	LPUNKNOWN pUnkReserved;
	LPUNKNOWN pUnk;
	IDINTERFACE idRef;
	BSTR bstrKey;
	HRESULT hResult;

	hResult = WrapIUnknownWFromA(pUnkOuterA, &pUnkOuter);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIUnknownWFromA(pUnkReservedA, &pUnkReserved);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToW(bstrKeyA, &bstrKey);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->CreateInstanceLic(pUnkOuter, pUnkReserved, riid,
			bstrKey, (LPVOID *)&pUnk);

	if (FAILED(hResult))
		goto Error;

	//
	//  Convert the 16 bytes GUID into an internal integer for speed.
	//
	idRef = WrapTranslateIID(riid);
	hResult = WrapInterfaceAFromW(idRef, pUnk, (LPUNKNOWN *)ppvObject);
	pUnk->Release();

Error:
	if (bstrKey)
		ConvertDispStringFreeW(bstrKey);

	if (pUnkReserved)
		pUnkReserved->Release();

	if (pUnkOuter)
		pUnkOuter->Release();

	return hResult;
}

/////////////////////////////////////////////////////////////////////////////
// CSpecifyPropertyPagesA class

STDMETHODIMP CSpecifyPropertyPagesA::GetPages(CAUUID FAR* pPages)
{
	TraceMethodEnter("CSpecifyPropertyPagesA::GetPages", this);
	_DebugHook(GetWide(), MEMBER_PTR(ISpecifyPropertyPages, GetPages));
	return GetWide()->GetPages(pPages);
}

/////////////////////////////////////////////////////////////////////////////
// CPerPropertyBrowsingA class

STDMETHODIMP CPerPropertyBrowsingA::GetDisplayString(DISPID dispid,
	BSTRA FAR* pbstrA)
{
	TraceMethodEnter("CPerPropertyBrowsingA::GetDisplayString", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPerPropertyBrowsing, GetDisplayString));

	HRESULT hResult;
	BSTR bstr, *pbstr;
	pbstr = (pbstrA ? &bstr : NULL);

	hResult = GetWide()->GetDisplayString(dispid, pbstr);
	if (hResult != NOERROR)
	{
		if (pbstrA)
			*pbstrA = NULL;
		return hResult;
	}

	HRESULT hResultConvert = NOERROR;

	if (pbstr)
	{
		hResultConvert = ConvertDispStringToA(bstr, pbstrA);
		SysFreeString(bstr);
	}

	return SUCCEEDED(hResultConvert) ? hResult : hResultConvert;
}

STDMETHODIMP CPerPropertyBrowsingA::MapPropertyToPage(DISPID dispid, LPCLSID lpclsid)
{
	TraceMethodEnter("CPerPropertyBrowsingA::MapPropertyToPage", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPerPropertyBrowsing, MapPropertyToPage));
	return GetWide()->MapPropertyToPage(dispid, lpclsid);
}

STDMETHODIMP CPerPropertyBrowsingA::GetPredefinedStrings(DISPID dispid,
	CALPSTR FAR* lpcaStringsOut, CADWORD FAR* lpcaCookiesOut)
{
	TraceMethodEnter("CPerPropertyBrowsingA::GetPredefinedStrings", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPerPropertyBrowsing, GetPredefinedStrings));

	HRESULT hResult;
	CALPOLESTR caStringsOut;

	hResult = GetWide()->GetPredefinedStrings(dispid, &caStringsOut, lpcaCookiesOut);
	if (hResult != NOERROR)
		return hResult;

	// Convert string array
	lpcaStringsOut->cElems = caStringsOut.cElems;
	hResult = ConvertStringArrayToA(caStringsOut.pElems, &lpcaStringsOut->pElems,
		caStringsOut.cElems);

	if (FAILED(hResult))
		delete lpcaCookiesOut->pElems;

	ConvertStringArrayFree(caStringsOut.pElems, caStringsOut.cElems);

	return hResult;
}

STDMETHODIMP CPerPropertyBrowsingA::GetPredefinedValue(DISPID dispid,
	DWORD dwCookie, VARIANTA FAR* lpvarOut)
{
	TraceMethodEnter("CPerPropertyBrowsingA::GetPredefinedValue", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPerPropertyBrowsing, GetPredefinedValue));

	HRESULT hResult;

	hResult = GetWide()->GetPredefinedValue(dispid, dwCookie, (LPVARIANT)lpvarOut);
	if (FAILED(hResult))
		return hResult;

	return ConvertVariantToA(lpvarOut);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageSiteA class

STDMETHODIMP CPropertyPageSiteA::OnStatusChange(DWORD flags)
{
	TraceMethodEnter("CPropertyPageSiteA::OnStatusChange", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPageSite, OnStatusChange));
	return GetWide()->OnStatusChange(flags);
}

STDMETHODIMP CPropertyPageSiteA::GetLocaleID(LCID FAR* pLocaleID)
{
	TraceMethodEnter("CPropertyPageSiteA::GetLocaleID", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPageSite, GetLocaleID));
	return GetWide()->GetLocaleID(pLocaleID);
}

STDMETHODIMP CPropertyPageSiteA::GetPageContainer(LPUNKNOWN FAR* ppUnk)
{
	TraceMethodEnter("CPropertyPageSiteA::GetPageContainer", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPageSite, GetPageContainer));

	LPUNKNOWN pUnk;
	HRESULT hResult;

	hResult = GetWide()->GetPageContainer(&pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIUnknownAFromW(pUnk, ppUnk);
	pUnk->Release();
	return hResult;
}

STDMETHODIMP CPropertyPageSiteA::TranslateAccelerator(LPMSG lpMsg)
{
	TraceMethodEnter("CPropertyPageSiteA::TranslateAccelerator", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPageSite, TranslateAccelerator));
	return GetWide()->TranslateAccelerator(lpMsg);
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyPage2A class

STDMETHODIMP CPropertyPage2A::SetPageSite(LPPROPERTYPAGESITEA pPageSiteA)
{
	TraceMethodEnter("CPropertyPage2A::SetPageSite", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, SetPageSite));

	LPPROPERTYPAGESITE pPageSite;
	HRESULT hResult;

	WrapIPropertyPageSiteWFromA(pPageSiteA, &pPageSite);
	hResult = GetWide()->SetPageSite(pPageSite);

	pPageSite->Release();
	return hResult;
}

STDMETHODIMP CPropertyPage2A::Activate(HWND hwndParent, LPCRECT rect,
			BOOL bModal)
{
	TraceMethodEnter("CPropertyPage2A::Activate", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, Activate));
	return GetWide()->Activate(hwndParent, rect, bModal);
}

STDMETHODIMP CPropertyPage2A::Deactivate()
{
	TraceMethodEnter("CPropertyPage2A::Deactivate", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, Deactivate));
	return GetWide()->Deactivate();
}

STDMETHODIMP CPropertyPage2A::GetPageInfo(LPPROPPAGEINFOA pPageInfoA)
{
	TraceMethodEnter("CPropertyPage2A::GetPageInfo", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, GetPageInfo));

	HRESULT hResult;
	PROPPAGEINFO pageInfo;

	hResult = GetWide()->GetPageInfo(&pageInfo);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertPROPPAGEINFOToA(&pageInfo, pPageInfoA);
	ConvertPROPPAGEINFOFree(&pageInfo);
	return hResult;
}

STDMETHODIMP CPropertyPage2A::SetObjects(ULONG cObjects, LPUNKNOWN FAR* ppUnkA)
{
	TraceMethodEnter("CPropertyPage2A::SetObjects", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, SetObjects));

	HRESULT hResult;
	LPUNKNOWN FAR* ppUnk = new LPUNKNOWN[cObjects];

	ULONG i;
	for (i = 0; i < cObjects; i++)
		ppUnk[i] = NULL;

	for (i = 0; i < cObjects; i++)
	{
		hResult = WrapIUnknownWFromA(ppUnkA[i], &ppUnk[i]);
		if (FAILED(hResult))
			goto Error;
	}

	hResult = GetWide()->SetObjects(cObjects, ppUnk);

Error:
	for (i = 0; i < cObjects; i++)
		if (ppUnk[i] != NULL)
			ppUnk[i]->Release();
	delete [] ppUnk;

	return hResult;
}

STDMETHODIMP CPropertyPage2A::Show(UINT nCmdShow)
{
	TraceMethodEnter("CPropertyPage2A::Show", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, Show));
	return GetWide()->Show(nCmdShow);
}

STDMETHODIMP CPropertyPage2A::Move(LPCRECT prect)
{
	TraceMethodEnter("CPropertyPage2A::Move", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, Move));
	return GetWide()->Move(prect);
}

STDMETHODIMP CPropertyPage2A::IsPageDirty()
{
	TraceMethodEnter("CPropertyPage2A::IsPageDirty", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, IsPageDirty));
	return GetWide()->IsPageDirty();
}

STDMETHODIMP CPropertyPage2A::Apply()
{
	TraceMethodEnter("CPropertyPage2A::Apply", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, Apply));
	return GetWide()->Apply();
}

STDMETHODIMP CPropertyPage2A::Help(LPCSTR lpszHelpDirA)
{
	TraceMethodEnter("CPropertyPage2A::Help", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, Help));

	WCHAR  szHelpDir[MAX_STRING];
	if (lpszHelpDirA != NULL)
		ConvertStringToW(lpszHelpDirA, szHelpDir);
	else
		szHelpDir[0] = L'\0';

	return GetWide()->Help(szHelpDir);
}

STDMETHODIMP CPropertyPage2A::TranslateAccelerator(LPMSG lpMsg)
{
	TraceMethodEnter("CPropertyPage2A::TranslateAccelerator", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, TranslateAccelerator));
	return GetWide()->TranslateAccelerator(lpMsg);
}

STDMETHODIMP CPropertyPage2A::EditProperty(DISPID dispid)
{
	TraceMethodEnter("CPropertyPage2A::EditProperty", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPropertyPage2, EditProperty));
	return GetWide()->EditProperty(dispid);
}


/////////////////////////////////////////////////////////////////////////////
// CFontA class

STDMETHODIMP CFontA::get_Name(BSTRA FAR* pname)
{
	TraceMethodEnter("CFontA::get_Name", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Name));

	HRESULT hResult;
	BSTR bstr, *pbstr;
	pbstr = (pname ? &bstr : NULL);

	hResult = GetWide()->get_Name(pbstr);
	if (FAILED(hResult))
		return hResult;

	if (pbstr)
	{
		hResult = ConvertDispStringToA(bstr, pname);
		SysFreeString(bstr);
	}

	return hResult;
}

STDMETHODIMP CFontA::put_Name(BSTRA name)
{
	TraceMethodEnter("CFontA::put_Name", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Name));

	HRESULT hResult;
	BSTR bstr;

	hResult = ConvertDispStringToW(name, &bstr);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->put_Name(bstr);
	ConvertDispStringFreeW(bstr);
	return hResult;
}

STDMETHODIMP CFontA::get_Size(CY FAR* psize)
{
	TraceMethodEnter("CFontA::get_Size", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Size));
	return GetWide()->get_Size(psize);
}

STDMETHODIMP CFontA::put_Size(CY size)
{
	TraceMethodEnter("CFontA::put_Size", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Size));
	return GetWide()->put_Size(size);
}

STDMETHODIMP CFontA::get_Bold(BOOL FAR* pbold)
{
	TraceMethodEnter("CFontA::get_Bold", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Bold));
	return GetWide()->get_Bold(pbold);
}

STDMETHODIMP CFontA::put_Bold(BOOL bold)
{
	TraceMethodEnter("CFontA::put_Bold", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Bold));
	return GetWide()->put_Bold(bold);
}

STDMETHODIMP CFontA::get_Italic(BOOL FAR* pitalic)
{
	TraceMethodEnter("CFontA::get_Italic", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Italic));
	return GetWide()->get_Italic(pitalic);
}

STDMETHODIMP CFontA::put_Italic(BOOL italic)
{
	TraceMethodEnter("CFontA::put_Italic", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Italic));
	return GetWide()->put_Italic(italic);
}

STDMETHODIMP CFontA::get_Underline(BOOL FAR* punderline)
{
	TraceMethodEnter("CFontA::get_Underline", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Underline));
	return GetWide()->get_Underline(punderline);
}

STDMETHODIMP CFontA::put_Underline(BOOL underline)
{
	TraceMethodEnter("CFontA::put_Underline", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Underline));
	return GetWide()->put_Underline(underline);
}

STDMETHODIMP CFontA::get_Strikethrough(BOOL FAR* pstrikethrough)
{
	TraceMethodEnter("CFontA::get_Strikethrough", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Strikethrough));
	return GetWide()->get_Strikethrough(pstrikethrough);
}

STDMETHODIMP CFontA::put_Strikethrough(BOOL strikethrough)
{
	TraceMethodEnter("CFontA::put_Strikethrough", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Strikethrough));
	return GetWide()->put_Strikethrough(strikethrough);
}

STDMETHODIMP CFontA::get_Weight(short FAR* pweight)
{
	TraceMethodEnter("CFontA::get_Weight", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Weight));
	return GetWide()->get_Weight(pweight);
}

STDMETHODIMP CFontA::put_Weight(short weight)
{
	TraceMethodEnter("CFontA::put_Weight", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Weight));
	return GetWide()->put_Weight(weight);
}

STDMETHODIMP CFontA::get_Charset(short FAR* pcharset)
{
	TraceMethodEnter("CFontA::get_Charset", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_Charset));
	return GetWide()->get_Charset(pcharset);
}

STDMETHODIMP CFontA::put_Charset(short charset)
{
	TraceMethodEnter("CFontA::put_Charset", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, put_Charset));
	return GetWide()->put_Charset(charset);
}

STDMETHODIMP CFontA::get_hFont(HFONT FAR* phfont)
{
	TraceMethodEnter("CFontA::get_hFont", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, get_hFont));
	return GetWide()->get_hFont(phfont);
}

STDMETHODIMP CFontA::Clone(IFontA FAR* FAR* lplpFont)
{
	TraceMethodEnter("CFontA::Clone", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, Clone));

	LPFONT lpFont;
	HRESULT hResult;

	hResult = GetWide()->Clone(&lpFont);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIFontAFromW(lpFont, lplpFont);
	lpFont->Release();
	return hResult;
}

STDMETHODIMP CFontA::IsEqual(IFontA FAR* lpFontOtherA)
{
	TraceMethodEnter("CFontA::IsEqual", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, IsEqual));

	HRESULT hResult;
	LPFONT lpFontOther;
	hResult = WrapIFontWFromA(lpFontOtherA, &lpFontOther);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->IsEqual(lpFontOther);
	lpFontOther->Release();

	return hResult;
}

STDMETHODIMP CFontA::SetRatio(long cyLogical, long cyHimetric)
{
	TraceMethodEnter("CFontA::SetRatio", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, SetRatio));
	return GetWide()->SetRatio(cyLogical, cyHimetric);
}

STDMETHODIMP CFontA::QueryTextMetrics(LPTEXTMETRICA lptmA)
{
	TraceMethodEnter("CFontA::QueryTextMetrics", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, QueryTextMetrics));

	HRESULT hResult;
	TEXTMETRICW tmW;

	hResult = GetWide()->QueryTextMetrics(&tmW);
	if (FAILED(hResult))
	{
		memset(lptmA, 0, sizeof(LPTEXTMETRICA));
		return hResult;
	}

	// Convert from TEXTMETRICW to TEXTMETRICA
	memcpy(lptmA, &tmW, sizeof(LONG) * 11);
	memcpy(&lptmA->tmItalic, &tmW.tmItalic, sizeof(BYTE) * 5);
	WideCharToMultiByte(CP_ACP, 0, &tmW.tmFirstChar, 1, (LPSTR)&lptmA->tmFirstChar, 1, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, &tmW.tmLastChar, 1, (LPSTR)&lptmA->tmLastChar, 1, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, &tmW.tmDefaultChar, 1, (LPSTR)&lptmA->tmDefaultChar, 1, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, &tmW.tmBreakChar, 1, (LPSTR)&lptmA->tmBreakChar, 1, NULL, NULL);

	return hResult;
}

STDMETHODIMP CFontA::AddRefHfont(HFONT hfont)
{
	TraceMethodEnter("CFontA::AddRefHfont", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, AddRefHfont));
	return GetWide()->AddRefHfont(hfont);
}

STDMETHODIMP CFontA::ReleaseHfont(HFONT hfont)
{
	TraceMethodEnter("CFontA::ReleaseHfont", this);
	_DebugHook(GetWide(), MEMBER_PTR(IFont, ReleaseHfont));
	return GetWide()->ReleaseHfont(hfont);
}


/////////////////////////////////////////////////////////////////////////////
// CPictureA class

STDMETHODIMP CPictureA::get_Handle(OLE_HANDLE FAR* phandle)
{
	TraceMethodEnter("CPictureA::get_Handle", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_Handle));
	return GetWide()->get_Handle(phandle);
}

STDMETHODIMP CPictureA::get_hPal(OLE_HANDLE FAR* phpal)
{
	TraceMethodEnter("CPictureA::get_hPal", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_hPal));
	return GetWide()->get_hPal(phpal);
}

STDMETHODIMP CPictureA::get_Type(short FAR* ptype)
{
	TraceMethodEnter("CPictureA::get_Type", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_Type));
	return GetWide()->get_Type(ptype);
}

STDMETHODIMP CPictureA::get_Width(OLE_XSIZE_HIMETRIC FAR* pwidth)
{
	TraceMethodEnter("CPictureA::get_Width", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_Width));
	return GetWide()->get_Width(pwidth);
}

STDMETHODIMP CPictureA::get_Height(OLE_YSIZE_HIMETRIC FAR* pheight)
{
	TraceMethodEnter("CPictureA::get_Height", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_Height));
	return GetWide()->get_Height(pheight);
}

STDMETHODIMP CPictureA::Render(HDC hdc, long x, long y, long cx, long cy,
	OLE_XPOS_HIMETRIC xSrc, OLE_YPOS_HIMETRIC ySrc,
	OLE_XSIZE_HIMETRIC cxSrc, OLE_YSIZE_HIMETRIC cySrc,
	LPRECT lprcWBounds)
{
	TraceMethodEnter("CPictureA::Render", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, Render));
	return GetWide()->Render(hdc, x, y, cx, cy, xSrc, ySrc, cxSrc, cySrc,
		lprcWBounds);
}

STDMETHODIMP CPictureA::set_hPal(OLE_HANDLE hpal)
{
	TraceMethodEnter("CPictureA::set_hPal", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, set_hPal));
	return GetWide()->set_hPal(hpal);
}

STDMETHODIMP CPictureA::get_CurDC(HDC FAR * phdcOut)
{
	TraceMethodEnter("CPictureA::get_CurDC", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_CurDC));
	return GetWide()->get_CurDC(phdcOut);
}

STDMETHODIMP CPictureA::SelectPicture(
	HDC hdcIn, HDC FAR * phdcOut, OLE_HANDLE FAR * phbmpOut)
{
	TraceMethodEnter("CPictureA::SelectPicture", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, SelectPicture));
	return GetWide()->SelectPicture(hdcIn, phdcOut, phbmpOut);
}

STDMETHODIMP CPictureA::get_KeepOriginalFormat(BOOL * pfkeep)
{
	TraceMethodEnter("CPictureA::get_KeepOriginalFormat", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_KeepOriginalFormat));
	return GetWide()->get_KeepOriginalFormat(pfkeep);
}

STDMETHODIMP CPictureA::put_KeepOriginalFormat(BOOL fkeep)
{
	TraceMethodEnter("CPictureA::put_KeepOriginalFormat", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, put_KeepOriginalFormat));
	return GetWide()->put_KeepOriginalFormat(fkeep);
}

STDMETHODIMP CPictureA::PictureChanged()
{
	TraceMethodEnter("CPictureA::PictureChanged", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, PictureChanged));
	return GetWide()->PictureChanged();
}

STDMETHODIMP CPictureA::SaveAsFile(LPSTREAMA pStmA, BOOL fSaveMemCopy,
	LONG FAR * lpcbSize)
{
	TraceMethodEnter("CPictureA::SaveAsFile", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, SaveAsFile));

	LPSTREAM pStm;
	HRESULT hResult;

	hResult = WrapIStreamWFromA(pStmA, &pStm);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SaveAsFile(pStm, fSaveMemCopy, lpcbSize);
	pStm->Release();
	return hResult;
}

STDMETHODIMP CPictureA::get_Attributes(DWORD FAR * lpdwAttr)
{
	TraceMethodEnter("CPictureA::get_Attributes", this);
	_DebugHook(GetWide(), MEMBER_PTR(IPicture, get_Attributes));
	return GetWide()->get_Attributes(lpdwAttr);
}
