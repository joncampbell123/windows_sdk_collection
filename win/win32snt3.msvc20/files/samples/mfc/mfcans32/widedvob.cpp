//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       wideidvob.h
//
//  Contents:   ANSI Wrappers for Unicode DvObj Interfaces and APIs.
//
//  Classes:    CEnumSTATDATAW
//              CDataObjectW
//              CViewObjectW
//              CViewObject2W
//              CAdviseSinkW
//              CAdviseSink2W
//              CDataAdviseHolderW
//              COleCacheW
//              COleCache2W
//              COleCacheControlW
//
//  Functions:  IEnumSTATDATAWFromA
//              IDataObjectWFromA
//              IViewObjectWFromA
//              IViewObject2WFromA
//              IAdviseSinkWFromA
//              IAdviseSink2WFromA
//              IDataAdviseHolderWFromA
//              IOleCacheWFromA
//              IOleCache2WFromA
//              IOleCacheControlWFromA
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IEnumSTATDATAW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAW::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAW::Next(
		ULONG celt,
		STATDATA * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumSTATDATAW::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATDATAA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetANSI()->Next(celt, (STATDATAA *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertSTATDATAArrayToW(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAW::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAW::Skip(ULONG celt)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATDATAA, Skip));

	return GetANSI()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAW::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAW::Reset(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATDATAA, Reset));

	return GetANSI()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAW::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAW::Clone(IEnumSTATDATA * * ppstat)
{
	TraceMethodEnter("CEnumSTATDATAW::Clone", this);

	IEnumSTATDATAA * pstatA;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumSTATDATAA, Clone));

	*ppstat = NULL;

	HRESULT hResult = GetANSI()->Clone(&pstatA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumSTATDATAWFromA(pstatA, ppstat);

	if (pstatA)
		pstatA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IEnumFORMATETCW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCW::Next, public
//
//  Synopsis:   Thunks Next to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCW::Next(
		ULONG celt,
		FORMATETC * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumFORMATETCW::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumFORMATETCA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetANSI()->Next(celt, (FORMATETCA *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertFORMATETCArrayToW(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCW::Skip, public
//
//  Synopsis:   Thunks Skip to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCW::Skip(ULONG celt)
{
	TraceMethodEnter("CEnumFORMATETCW::Skip", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumFORMATETCA, Skip));

	return GetANSI()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCW::Reset, public
//
//  Synopsis:   Thunks Reset to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCW::Reset(VOID)
{
	TraceMethodEnter("CEnumFORMATETCW::Reset", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumFORMATETCA, Reset));

	return GetANSI()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCW::Clone, public
//
//  Synopsis:   Thunks Clone to ANSI Method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCW::Clone(IEnumFORMATETC * * ppenm)
{
	TraceMethodEnter("CEnumFORMATETCW::Clone", this);

	IEnumFORMATETCA * penmA;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumFORMATETCA, Clone));

	*ppenm = NULL;

	HRESULT hResult = GetANSI()->Clone(&penmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumFORMATETCWFromA(penmA, ppenm);

	if (penmA)
		penmA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IDataObjectW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::GetData, public
//
//  Synopsis:   Thunks GetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::GetData(LPFORMATETC pformatetcIn,
		LPSTGMEDIUM pmedium)
{
	TraceMethodEnter("CDataObjectW::GetData", this);

	FORMATETCA FormatEtc;
	HRESULT    hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, GetData));

	hResult = ConvertFORMATETCToA(pformatetcIn, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->GetData(&FormatEtc, (LPSTGMEDIUMA)pmedium);

	ConvertFORMATETCFree(&FormatEtc);

	if (FAILED(hResult))
		return hResult;

	return ConvertSTGMEDIUMToW(pformatetcIn->cfFormat, pmedium);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::GetDataHere, public
//
//  Synopsis:   Thunks GetDataHere to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::GetDataHere(LPFORMATETC pformatetc,
		LPSTGMEDIUM pmedium)
{
	TraceMethodEnter("CDataObjectW::GetDataHere", this);

	FORMATETCA FormatEtc;
	STGMEDIUMA Medium;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, GetDataHere));

	hResult = ConvertFORMATETCToA(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertSTGMEDIUMToA(pformatetc->cfFormat, pmedium, &Medium);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->GetDataHere(&FormatEtc, &Medium);

	ConvertSTGMEDIUMFree(pformatetc->cfFormat, &Medium);

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::QueryGetData, public
//
//  Synopsis:   Thunks QueryGetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::QueryGetData(LPFORMATETC pformatetc)
{
	TraceMethodEnter("CDataObjectW::QueryGetData", this);

	FORMATETCA FormatEtc;
	HRESULT   hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, QueryGetData));

	hResult = ConvertFORMATETCToA(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->QueryGetData(&FormatEtc);

	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::GetCanonicalFormatEtc, public
//
//  Synopsis:   Thunks GetCanonicalFormatEtc to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::GetCanonicalFormatEtc(LPFORMATETC pformatetc,
		LPFORMATETC pformatetcOut)
{
	TraceMethodEnter("CDataObjectW::GetCanonicalFormatEtc", this);

	FORMATETCA FormatEtc;
	FORMATETCA FormatEtcOut;
	HRESULT   hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, GetCanonicalFormatEtc));

	hResult = ConvertFORMATETCToA(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->GetCanonicalFormatEtc(&FormatEtc, &FormatEtcOut);

	ConvertFORMATETCFree(&FormatEtc);

	if (hResult != NOERROR)
		return hResult;

	hResult = ConvertFORMATETCToW(&FormatEtcOut, pformatetcOut);

	ConvertFORMATETCFree(&FormatEtcOut);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::SetData, public
//
//  Synopsis:   Thunks SetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::SetData(LPFORMATETC pformatetc,
		STGMEDIUM * pmediumA, BOOL fRelease)
{
	TraceMethodEnter("CDataObjectW::SetData", this);

	FORMATETCA FormatEtc;
	STGMEDIUMA Medium;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, SetData));

	hResult = ConvertFORMATETCToA(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertSTGMEDIUMToA(pformatetc->cfFormat, pmediumA, &Medium);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->SetData(&FormatEtc, &Medium, fRelease);

	ConvertSTGMEDIUMFree(pformatetc->cfFormat, &Medium);

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::EnumFormatEtc, public
//
//  Synopsis:   Thunks EnumFormatEtc to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::EnumFormatEtc(DWORD dwDirection,
		LPENUMFORMATETC * ppenumFormatEtc)
{
	TraceMethodEnter("CDataObjectW::EnumFormatEtc", this);

	LPENUMFORMATETCA pEnumFormatEtc;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, EnumFormatEtc));

	hResult = GetANSI()->EnumFormatEtc(dwDirection, &pEnumFormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumFORMATETCWFromA(pEnumFormatEtc, ppenumFormatEtc);

	if (pEnumFormatEtc)
		pEnumFormatEtc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::DAdvise, public
//
//  Synopsis:   Thunks DAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::DAdvise(FORMATETC * pFormatetc, DWORD advf,
		LPADVISESINK pAdvSink, DWORD * pdwConnection)
{
	TraceMethodEnter("CDataObjectW::DAdvise", this);

	FORMATETCA FormatEtcA;
	LPADVISESINKA pAdvSinkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, DAdvise));

	hResult = ConvertFORMATETCToA(pFormatetc, &FormatEtcA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIAdviseSinkAFromW(pAdvSink, &pAdvSinkA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->DAdvise(&FormatEtcA, advf, pAdvSinkA, pdwConnection);

	if (pAdvSinkA)
		pAdvSinkA->Release();

Error:
	ConvertFORMATETCFree(&FormatEtcA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::DUnadvise, public
//
//  Synopsis:   Thunks DUnadvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::DUnadvise(DWORD dwConnection)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, DUnadvise));

	return GetANSI()->DUnadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectW::EnumDAdvise, public
//
//  Synopsis:   Thunks EnumDAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectW::EnumDAdvise(LPENUMSTATDATA * ppenumAdvise)
{
	TraceMethodEnter("CDataObjectW::EnumDAdvise", this);

	LPENUMSTATDATAA penumAdviseA = NULL;
	HRESULT hResult;



	_DebugHook(GetANSI(), MEMBER_PTR(IDataObjectA, EnumDAdvise));

	hResult = GetANSI()->EnumDAdvise(&penumAdviseA);
	if (FAILED(hResult))
		return hResult;

	if (penumAdviseA)
	{
		hResult = WrapIEnumSTATDATAWFromA(penumAdviseA, ppenumAdvise);
		penumAdviseA->Release();
	}
	else
		*ppenumAdvise = NULL;

	return hResult;
}


//***************************************************************************
//
//                   IViewObject2W Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2W::Draw, public
//
//  Synopsis:   Thunks Draw to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2W::Draw(DWORD dwDrawAspect, LONG lindex,
		void * pvAspect, DVTARGETDEVICE * ptd, HDC hicTargetDev, HDC hdcDraw,
		LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
		BOOL (CALLBACK * pfnContinue)(DWORD), DWORD dwContinue)
{
	TraceMethodEnter("CViewObject2W::Draw", this);

	LPDVTARGETDEVICEA pDevice;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IViewObject2A, Draw));

	if (ptd)
	{
		hResult = ConvertDVTARGETDEVICEToA(ptd, &pDevice);
		if (FAILED(hResult))
			return hResult;
	}
	else
		pDevice = NULL;

	hResult = GetANSI()->Draw(dwDrawAspect, lindex, pvAspect, pDevice,
			hicTargetDev, hdcDraw, lprcBounds, lprcWBounds, pfnContinue,
			dwContinue);

	ConvertDVTARGETDEVICEFree(pDevice);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2W::GetColorSet, public
//
//  Synopsis:   Thunks GetColorSet to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2W::GetColorSet(DWORD dwDrawAspect, LONG lindex,
		void * pvAspect, DVTARGETDEVICE * ptd, HDC hicTargetDev,
		LPLOGPALETTE * ppColorSet)
{
	TraceMethodEnter("CViewObject2W::GetColorSet", this);

	LPDVTARGETDEVICEA pDevice;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IViewObject2A, GetColorSet));

	if (ptd)
	{
		hResult = ConvertDVTARGETDEVICEToA(ptd, &pDevice);
		if (FAILED(hResult))
			return hResult;
	}
	else
		pDevice = NULL;

	hResult = GetANSI()->GetColorSet(dwDrawAspect, lindex, pvAspect, pDevice,
			hicTargetDev, ppColorSet);

	ConvertDVTARGETDEVICEFree(pDevice);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2W::Freeze, public
//
//  Synopsis:   Thunks Freeze to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2W::Freeze(DWORD dwDrawAspect, LONG lindex,
		void * pvAspect, DWORD * pdwFreeze)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IViewObject2A, Freeze));

	return GetANSI()->Freeze(dwDrawAspect, lindex, pvAspect, pdwFreeze);
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2W::Unfreeze, public
//
//  Synopsis:   Thunks Unfreeze to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2W::Unfreeze(DWORD dwFreeze)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IViewObject2A, Unfreeze));

	return GetANSI()->Unfreeze(dwFreeze);
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2W::SetAdvise, public
//
//  Synopsis:   Thunks SetAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2W::SetAdvise(DWORD aspects, DWORD advf,
		LPADVISESINK pAdvSink)
{
	TraceMethodEnter("CViewObject2W::SetAdvise", this);

	LPADVISESINKA pAdvSinkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IViewObject2A, SetAdvise));

	hResult = WrapIAdviseSinkAFromW(pAdvSink, &pAdvSinkA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SetAdvise(aspects, advf, pAdvSinkA);

	if (pAdvSinkA)
		pAdvSinkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2W::GetAdvise, public
//
//  Synopsis:   Thunks GetAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2W::GetAdvise(DWORD * pAspects, DWORD * pAdvf,
		LPADVISESINK * ppAdvSink)
{
	TraceMethodEnter("CViewObject2W::GetAdvise", this);

	LPADVISESINKA pAdvSinkA, * ppAdvSinkA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IViewObject2A, GetAdvise));

	if (ppAdvSink)
	{
		ppAdvSinkA = &pAdvSinkA;
		pAdvSinkA = NULL;
	}
	else
		ppAdvSinkA = NULL;

	hResult = GetANSI()->GetAdvise(pAspects, pAdvf, ppAdvSinkA);
	if (FAILED(hResult))
		return hResult;

	if (ppAdvSink)
	{
		hResult = WrapIAdviseSinkWFromA(pAdvSinkA, ppAdvSink);

		if (pAdvSinkA)
			pAdvSinkA->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2W::GetExtent, public
//
//  Synopsis:   Thunks GetExtent to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2W::GetExtent(DWORD dwDrawAspect, LONG lindex,
		DVTARGETDEVICE * ptd, LPSIZEL lpsizel)
{
	LPDVTARGETDEVICEA pDevice;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IViewObject2A, GetExtent));

	if (ptd)
	{
		hResult = ConvertDVTARGETDEVICEToA(ptd, &pDevice);
		if (FAILED(hResult))
			return hResult;
	}
	else
		pDevice = NULL;

	hResult = GetANSI()->GetExtent(dwDrawAspect, lindex, pDevice, lpsizel);

	ConvertDVTARGETDEVICEFree(pDevice);

	return hResult;

}



//***************************************************************************
//
//                   IAdviseSink2W Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2W::OnDataChange, public
//
//  Synopsis:   Thunks OnDataChange to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2W::OnDataChange(FORMATETC * pFormatetc,
		STGMEDIUM * pStgmed)
{
	TraceMethodEnter("CAdviseSink2W::OnDataChange", this);

	FORMATETCA FormatEtc;
	STGMEDIUMA Medium;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IAdviseSink2A, OnDataChange));

	hResult = ConvertFORMATETCToA(pFormatetc, &FormatEtc);
	if (FAILED(hResult))
		return;

	hResult = ConvertSTGMEDIUMToA(pFormatetc->cfFormat, pStgmed, &Medium);
	if (FAILED(hResult))
		goto Error;

	GetANSI()->OnDataChange(&FormatEtc, &Medium);

	ConvertSTGMEDIUMFree(pFormatetc->cfFormat, &Medium);

Error:
	ConvertFORMATETCFree(&FormatEtc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2W::OnViewChange, public
//
//  Synopsis:   Thunks OnViewChange to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2W::OnViewChange(DWORD dwAspect, LONG lindex)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IAdviseSink2A, OnViewChange));

	GetANSI()->OnViewChange(dwAspect, lindex);
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2W::OnRename, public
//
//  Synopsis:   Thunks OnRename to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2W::OnRename(LPMONIKER pmk)
{
	TraceMethodEnter("CAdviseSink2W::OnRename", this);

	LPMONIKERA pmkA;


	_DebugHook(GetANSI(), MEMBER_PTR(IAdviseSink2A, OnRename));

	WrapIMonikerAFromW(pmk, &pmkA);

	GetANSI()->OnRename(pmkA);

	if (pmkA)
		pmkA->Release();
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2W::OnSave, public
//
//  Synopsis:   Thunks OnSave to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2W::OnSave(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IAdviseSink2A, OnSave));

	GetANSI()->OnSave();
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2W::OnClose, public
//
//  Synopsis:   Thunks OnClose to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2W::OnClose(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IAdviseSink2A, OnClose));

	GetANSI()->OnClose();
}



//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2W::OnLinkSrcChange, public
//
//  Synopsis:   Thunks OnLinkSrcChange to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2W::OnLinkSrcChange(LPMONIKER pmk)
{
	TraceMethodEnter("CAdviseSink2W::OnLinkSrcChange", this);

	LPMONIKERA pmkA;


	_DebugHook(GetANSI(), MEMBER_PTR(IAdviseSink2A, OnLinkSrcChange));

	WrapIMonikerAFromW(pmk, &pmkA);

	GetANSI()->OnLinkSrcChange(pmkA);

	if (pmkA)
		pmkA->Release();
}



//***************************************************************************
//
//                   IDataAdviseHolderW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDataAdviseHolderW::Advise, public
//
//  Synopsis:   Thunks Advise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderW::Advise(LPDATAOBJECT pDataObject,
		FORMATETC * pFetc, DWORD advf, LPADVISESINK pAdvise,
		DWORD * pdwConnection)
{
	TraceMethodEnter("CDataAdviseHolderW::Advise", this);

	LPDATAOBJECTA pDataObjectA;
	FORMATETCA    FormatEtc;
	LPADVISESINKA pAdviseA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataAdviseHolderA, Advise));

	hResult = WrapIDataObjectAFromW(pDataObject, &pDataObjectA);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertFORMATETCToA(pFetc, &FormatEtc);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapIAdviseSinkAFromW(pAdvise, &pAdviseA);
	if (FAILED(hResult))
		goto Error1;

	hResult = GetANSI()->Advise(pDataObjectA, &FormatEtc, advf, pAdviseA, pdwConnection);

	if (pAdviseA)
		pAdviseA->Release();

Error1:
	if (pDataObjectA)
		pDataObjectA->Release();

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataAdviseHolderW::Unadvise, public
//
//  Synopsis:   Thunks Unadvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderW::Unadvise(DWORD dwConnection)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IDataAdviseHolderA, Unadvise));

	return GetANSI()->Unadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataAdviseHolderW::EnumAdvise, public
//
//  Synopsis:   Thunks EnumAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderW::EnumAdvise(
		LPENUMSTATDATA * ppenumAdvise)
{
	TraceMethodEnter("CDataAdviseHolderW::EnumAdvise", this);

	LPENUMSTATDATAA penumAdviseA = NULL;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataAdviseHolderA, EnumAdvise));

	hResult = GetANSI()->EnumAdvise(&penumAdviseA);
	if (FAILED(hResult))
		return hResult;

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
//  Member:     CDataAdviseHolderW::SendOnDataChange, public
//
//  Synopsis:   Thunks SendOnDataChange to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderW::SendOnDataChange(LPDATAOBJECT pDataObject,
		DWORD dwReserved, DWORD advf)
{
	TraceMethodEnter("CDataAdviseHolderW::SendOnDataChange", this);

	LPDATAOBJECTA pDataObjectA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDataAdviseHolderA, SendOnDataChange));

	hResult = WrapIDataObjectAFromW(pDataObject, &pDataObjectA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SendOnDataChange(pDataObjectA, dwReserved, advf);

	if (pDataObjectA)
		pDataObjectA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IOleCache2W Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleCache2W::Cache, public
//
//  Synopsis:   Thunks Cache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2W::Cache(LPFORMATETC lpFormatetc, DWORD advf,
		LPDWORD lpdwConnection)
{
	TraceMethodEnter("COleCache2W::Cache", this);

	FORMATETCA FormatEtc;
	HRESULT    hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleCache2A, Cache));

	hResult = ConvertFORMATETCToA(lpFormatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Cache(&FormatEtc, advf, lpdwConnection);

	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2W::Uncache, public
//
//  Synopsis:   Thunks Uncache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2W::Uncache(DWORD dwConnection)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IOleCache2A, Uncache));

	return GetANSI()->Uncache(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2W::EnumCache, public
//
//  Synopsis:   Thunks EnumCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2W::EnumCache(LPENUMSTATDATA * ppenumStatData)
{
	TraceMethodEnter("COleCache2W::EnumCache", this);

	LPENUMSTATDATAA penumStatDataA = NULL;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleCache2A, EnumCache));

	hResult = GetANSI()->EnumCache(&penumStatDataA);
	if (FAILED(hResult))
		return hResult;

	if (penumStatDataA)
	{
		hResult = WrapIEnumSTATDATAWFromA(penumStatDataA, ppenumStatData);
		penumStatDataA->Release();
	}
	else
		*ppenumStatData = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2W::InitCache, public
//
//  Synopsis:   Thunks InitCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2W::InitCache(LPDATAOBJECT pDataObject)
{
	TraceMethodEnter("COleCache2W::InitCache", this);

	LPDATAOBJECTA pDataObjectA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleCache2A, InitCache));

	hResult = WrapIDataObjectAFromW(pDataObject, &pDataObjectA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->InitCache(pDataObjectA);

	if (pDataObjectA)
		pDataObjectA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2W::SetData, public
//
//  Synopsis:   Thunks SetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2W::SetData(LPFORMATETC pformatetc,
		STGMEDIUM * pmedium, BOOL fRelease)
{
	TraceMethodEnter("COleCache2W::SetData", this);

	FORMATETCA FormatEtc;
	STGMEDIUMA mediumA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleCache2A, SetData));

	hResult = ConvertFORMATETCToA(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertSTGMEDIUMToA(pformatetc->cfFormat, pmedium, &mediumA);
	if (FAILED(hResult))
		goto Error;

	hResult = GetANSI()->SetData(&FormatEtc, &mediumA, fRelease);

	ConvertSTGMEDIUMFree(pformatetc->cfFormat, &mediumA);

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2W::UpdateCache, public
//
//  Synopsis:   Thunks UpdateCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2W::UpdateCache(LPDATAOBJECT pDataObject,
		DWORD grfUpdf, LPVOID pReserved)
{
	TraceMethodEnter("COleCache2W::UpdateCache", this);

	LPDATAOBJECTA pDataObjectA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleCache2A, UpdateCache));

	hResult = WrapIDataObjectAFromW(pDataObject, &pDataObjectA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->UpdateCache(pDataObjectA, grfUpdf, pReserved);

	if (pDataObjectA)
		pDataObjectA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2W::DiscardCache, public
//
//  Synopsis:   Thunks DiscardCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2W::DiscardCache(DWORD dwDiscardOptions)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IOleCache2A, DiscardCache));

	return GetANSI()->DiscardCache(dwDiscardOptions);
}



//***************************************************************************
//
//                   IOleCacheControlW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleCacheControlW::OnRun, public
//
//  Synopsis:   Thunks OnRun to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCacheControlW::OnRun(LPDATAOBJECT pDataObject)
{
	TraceMethodEnter("COleCacheControlW::OnRun", this);

	LPDATAOBJECTA pDataObjectA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IOleCacheControlA, OnRun));

	hResult = WrapIDataObjectAFromW(pDataObject, &pDataObjectA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->OnRun(pDataObjectA);

	if (pDataObjectA)
		pDataObjectA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCacheControlW::OnStop, public
//
//  Synopsis:   Thunks OnStop to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCacheControlW::OnStop(VOID)
{
	_DebugHook(GetANSI(), MEMBER_PTR(IOleCacheControlA, OnStop));

	return GetANSI()->OnStop();
}
