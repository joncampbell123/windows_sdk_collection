//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansidvob.h
//
//  Contents:   ANSI Wrappers for Unicode DvObj Interfaces and APIs.
//
//  Classes:    CEnumSTATDATAA
//              CDataObjectA
//              CViewObject2A
//              CAdviseSink2A
//              CDataAdviseHolder
//              COleCache2
//              COleCacheControl
//
//  Functions:  CreateDataAdviseHolderA
//              CreateDataCacheA
//              IEnumSTATDATAAFromW
//              IDataObjectAFromW
//              IViewObjectAFromW
//              IViewObject2AFromW
//              IAdviseSinkAFromW
//              IAdviseSink2AFromW
//              IDataAdviseHolderAFromW
//              IOleCacheAFromW
//              IOleCache2AFromW
//              IOleCacheControlAFromW
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   IEnumSTATDATAA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAA::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAA::Next(
		ULONG celt,
		STATDATAA * rgelt,
		ULONG * pceltFetched)
{
	TraceMethodEnter("CEnumSTATDATAA::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATDATA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetWide()->Next(celt, (STATDATA *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertSTATDATAArrayToA(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAA::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAA::Skip(ULONG celt)
{
	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATDATA, Skip));

	return (GetWide()->Skip(celt));
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAA::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAA::Reset(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATDATA, Reset));

	return (GetWide()->Reset());
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumSTATDATAA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumSTATDATAA::Clone(IEnumSTATDATAA * * ppstatA)
{
	TraceMethodEnter("CEnumSTATDATAA::Clone", this);

	IEnumSTATDATA * pwide;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumSTATDATA, Clone));

	*ppstatA = NULL;

	HRESULT hResult = GetWide()->Clone(&pwide);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumSTATDATAAFromW(pwide, ppstatA);

	if (pwide)
		pwide->Release();

	return hResult;
}



//***************************************************************************
//
//                   IEnumFORMATETCA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCA::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCA::Next(
		ULONG celt,
		FORMATETCA * rgelt,
		ULONG * pceltFetched)
{
	TraceNotify("CEnumFORMATETCA::Next");

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumFORMATETC, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetWide()->Next(celt, (FORMATETC *)rgelt, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertFORMATETCArrayToA(rgelt, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCA::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCA::Skip(ULONG celt)
{
	TraceNotify("CEnumFORMATETCA::Skip");

	_DebugHook(GetWide(), MEMBER_PTR(IEnumFORMATETC, Skip));

	return GetWide()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCA::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCA::Reset(VOID)
{
	TraceNotify("CEnumFORMATETCA::Reset");

	_DebugHook(GetWide(), MEMBER_PTR(IEnumFORMATETC, Reset));

	return GetWide()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumFORMATETCA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumFORMATETCA::Clone(IEnumFORMATETCA * * ppenmA)
{
	TraceNotify("CEnumFORMATETCA::Clone");

	IEnumFORMATETC * penm;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumFORMATETC, Clone));

	*ppenmA = NULL;

	HRESULT hResult = GetWide()->Clone(&penm);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumFORMATETCAFromW(penm, ppenmA);

	if (penm)
		penm->Release();

	return hResult;
}


//***************************************************************************
//
//                   IDataObjectA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::GetData, public
//
//  Synopsis:   Thunks GetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::GetData(LPFORMATETCA pformatetcIn,
		LPSTGMEDIUMA pmediumA)
{
	TraceMethodEnter("CDataObjectA::GetData", this);

	FORMATETC FormatEtc;
	HRESULT   hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, GetData));

	hResult = ConvertFORMATETCToW(pformatetcIn, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->GetData(&FormatEtc, (LPSTGMEDIUM)pmediumA);

	ConvertFORMATETCFree(&FormatEtc);

	if (FAILED(hResult))
		return hResult;

	return ConvertSTGMEDIUMToA(pformatetcIn->cfFormat, pmediumA);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::GetDataHere, public
//
//  Synopsis:   Thunks GetDataHere to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::GetDataHere(LPFORMATETCA pformatetc,
		LPSTGMEDIUMA pmediumA)
{
	TraceMethodEnter("CDataObjectA::GetDataHere", this);

	FORMATETC FormatEtc;
	STGMEDIUM Medium;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, GetDataHere));

	hResult = ConvertFORMATETCToW(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertSTGMEDIUMToW(pformatetc->cfFormat, pmediumA, &Medium);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->GetDataHere(&FormatEtc, &Medium);

	ConvertSTGMEDIUMFree(pformatetc->cfFormat, &Medium);

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::QueryGetData, public
//
//  Synopsis:   Thunks QueryGetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::QueryGetData(LPFORMATETCA pformatetc)
{
	TraceMethodEnter("CDataObjectA::QueryGetData", this);

	FORMATETC FormatEtc;
	HRESULT   hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, QueryGetData));

	hResult = ConvertFORMATETCToW(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->QueryGetData(&FormatEtc);

	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::GetCanonicalFormatEtc, public
//
//  Synopsis:   Thunks GetCanonicalFormatEtc to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::GetCanonicalFormatEtc(LPFORMATETCA pformatetc,
		LPFORMATETCA pformatetcOut)
{
	TraceMethodEnter("CDataObjectA::GetCanonicalFormatEtc", this);

	FORMATETC FormatEtc;
	FORMATETC FormatEtcOut;
	HRESULT   hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, GetCanonicalFormatEtc));

	hResult = ConvertFORMATETCToW(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->GetCanonicalFormatEtc(&FormatEtc, &FormatEtcOut);

	ConvertFORMATETCFree(&FormatEtc);

	if (hResult != NOERROR)
		return hResult;

	hResult = ConvertFORMATETCToA(&FormatEtcOut, pformatetcOut);

	ConvertFORMATETCFree(&FormatEtcOut);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::SetData, public
//
//  Synopsis:   Thunks SetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::SetData(LPFORMATETCA pformatetc,
		STGMEDIUMA * pmediumA, BOOL fRelease)
{
	TraceMethodEnter("CDataObjectA::SetData", this);

	FORMATETC FormatEtc;
	STGMEDIUM Medium;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, SetData));

	hResult = ConvertFORMATETCToW(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertSTGMEDIUMToW(pformatetc->cfFormat, pmediumA, &Medium);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->SetData(&FormatEtc, &Medium, fRelease);

	ConvertSTGMEDIUMFree(pformatetc->cfFormat, &Medium);

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::EnumFormatEtc, public
//
//  Synopsis:   Thunks EnumFormatEtc to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::EnumFormatEtc(DWORD dwDirection,
		LPENUMFORMATETCA * ppenumFormatEtcA)
{
	TraceMethodEnter("CDataObjectA::EnumFormatEtc", this);

	LPENUMFORMATETC pEnumFormatEtc;
	HRESULT hResult;

	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, EnumFormatEtc));

	hResult = GetWide()->EnumFormatEtc(dwDirection, &pEnumFormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIEnumFORMATETCAFromW(pEnumFormatEtc, ppenumFormatEtcA);

	if (pEnumFormatEtc)
		pEnumFormatEtc->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::DAdvise, public
//
//  Synopsis:   Thunks DAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::DAdvise(FORMATETCA * pFormatetc, DWORD advf,
		LPADVISESINKA pAdvSinkA, DWORD * pdwConnection)
{
	TraceMethodEnter("CDataObjectA::DAdvise", this);

	FORMATETC FormatEtc;
	LPADVISESINK pAdvSink;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, DAdvise));

	hResult = ConvertFORMATETCToW(pFormatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

		hResult = WrapIAdviseSinkWFromA(pAdvSinkA, &pAdvSink);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->DAdvise(&FormatEtc, advf, pAdvSink, pdwConnection);

	if (pAdvSink)
		pAdvSink->Release();

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::DUnadvise, public
//
//  Synopsis:   Thunks DUnadvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::DUnadvise(DWORD dwConnection)
{
	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, DUnadvise));

	return GetWide()->DUnadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataObjectA::EnumDAdvise, public
//
//  Synopsis:   Thunks EnumDAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataObjectA::EnumDAdvise(LPENUMSTATDATAA * ppenumAdviseA)
{
	TraceMethodEnter("CDataObjectA::EnumDAdvise", this);

	LPENUMSTATDATA penumAdvise = NULL;
	HRESULT hResult;



	_DebugHook(GetWide(), MEMBER_PTR(IDataObject, EnumDAdvise));

	hResult = GetWide()->EnumDAdvise(&penumAdvise);
	if (FAILED(hResult))
		return hResult;

	if (penumAdvise)
	{
		hResult = WrapIEnumSTATDATAAFromW(penumAdvise, ppenumAdviseA);
		penumAdvise->Release();
	}
	else
		*ppenumAdviseA = NULL;

	return hResult;
}


//***************************************************************************
//
//                   IViewObject2A Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2A::Draw, public
//
//  Synopsis:   Thunks Draw to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2A::Draw(DWORD dwDrawAspect, LONG lindex,
		void * pvAspect, DVTARGETDEVICEA * ptd, HDC hicTargetDev, HDC hdcDraw,
		LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
		BOOL (CALLBACK * pfnContinue)(DWORD), DWORD dwContinue)
{
	TraceMethodEnter("CViewObject2A::Draw", this);

	LPDVTARGETDEVICE pDevice;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IViewObject, Draw));

	if (ptd)
	{
		hResult = ConvertDVTARGETDEVICEToW(ptd, &pDevice);
		if (FAILED(hResult))
			return hResult;
	}
	else
		pDevice = NULL;

	hResult = GetWide()->Draw(dwDrawAspect, lindex, pvAspect, pDevice,
			hicTargetDev, hdcDraw, lprcBounds, lprcWBounds, pfnContinue,
			dwContinue);

	ConvertDVTARGETDEVICEFree(pDevice);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2A::GetColorSet, public
//
//  Synopsis:   Thunks GetColorSet to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2A::GetColorSet(DWORD dwDrawAspect, LONG lindex,
		void * pvAspect, DVTARGETDEVICEA * ptd, HDC hicTargetDev,
		LPLOGPALETTE * ppColorSet)
{
	TraceMethodEnter("CViewObject2A::GetColorSet", this);

	LPDVTARGETDEVICE pDevice;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IViewObject, GetColorSet));

	if (ptd)
	{
		hResult = ConvertDVTARGETDEVICEToW(ptd, &pDevice);
		if (FAILED(hResult))
			return hResult;
	}
	else
		pDevice = NULL;

	hResult = GetWide()->GetColorSet(dwDrawAspect, lindex, pvAspect, pDevice,
			hicTargetDev, ppColorSet);

	ConvertDVTARGETDEVICEFree(pDevice);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2A::Freeze, public
//
//  Synopsis:   Thunks Freeze to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2A::Freeze(DWORD dwDrawAspect, LONG lindex,
		void * pvAspect, DWORD * pdwFreeze)
{
	_DebugHook(GetWide(), MEMBER_PTR(IViewObject, Freeze));

	return GetWide()->Freeze(dwDrawAspect, lindex, pvAspect, pdwFreeze);
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2A::Unfreeze, public
//
//  Synopsis:   Thunks Unfreeze to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2A::Unfreeze(DWORD dwFreeze)
{
	_DebugHook(GetWide(), MEMBER_PTR(IViewObject, Unfreeze));

	return GetWide()->Unfreeze(dwFreeze);
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2A::SetAdvise, public
//
//  Synopsis:   Thunks SetAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2A::SetAdvise(DWORD aspects, DWORD advf,
		LPADVISESINKA pAdvSinkA)
{
	TraceMethodEnter("CViewObject2A::SetAdvise", this);

	LPADVISESINK pAdvSink;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IViewObject, SetAdvise));

		hResult = WrapIAdviseSinkWFromA(pAdvSinkA, &pAdvSink);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SetAdvise(aspects, advf, pAdvSink);

	if (pAdvSink)
		pAdvSink->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2A::GetAdvise, public
//
//  Synopsis:   Thunks GetAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2A::GetAdvise(DWORD * pAspects, DWORD * pAdvf,
		LPADVISESINKA * ppAdvSinkA)
{
	TraceMethodEnter("CViewObject2A::GetAdvise", this);

	LPADVISESINK pAdvSink, * ppAdvSink;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IViewObject, GetAdvise));

	if (ppAdvSinkA)
	{
		ppAdvSink = &pAdvSink;
		pAdvSink = NULL;
	}
	else
		ppAdvSink = NULL;

	hResult = GetWide()->GetAdvise(pAspects, pAdvf, ppAdvSink);
	if (FAILED(hResult))
		return hResult;

	if (ppAdvSinkA)
	{
		hResult = WrapIAdviseSinkAFromW(pAdvSink, ppAdvSinkA);

		if (pAdvSink)
			pAdvSink->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CViewObject2A::GetExtent, public
//
//  Synopsis:   Thunks GetExtent to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CViewObject2A::GetExtent(DWORD dwDrawAspect, LONG lindex,
		DVTARGETDEVICEA * ptd, LPSIZEL lpsizel)
{
	TraceMethodEnter("CViewObject2A::GetExtent", this);

	LPDVTARGETDEVICE pDevice;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IViewObject2, GetExtent));

	if (ptd)
	{
		hResult = ConvertDVTARGETDEVICEToW(ptd, &pDevice);
		if (FAILED(hResult))
			return hResult;
	}
	else
		pDevice = NULL;

	hResult = GetWide()->GetExtent(dwDrawAspect, lindex, pDevice, lpsizel);

	ConvertDVTARGETDEVICEFree(pDevice);

	return hResult;
}



//***************************************************************************
//
//                   IAdviseSink2A Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2A::OnDataChange, public
//
//  Synopsis:   Thunks OnDataChange to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2A::OnDataChange(FORMATETCA * pFormatetc,
		STGMEDIUMA * pStgmedA)
{
	TraceMethodEnter("CAdviseSink2A::OnDataChange", this);

	FORMATETC FormatEtc;
	STGMEDIUM Medium;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IAdviseSink, OnDataChange));

	hResult = ConvertFORMATETCToW(pFormatetc, &FormatEtc);
	if (FAILED(hResult))
		return;

	hResult = ConvertSTGMEDIUMToW(pFormatetc->cfFormat, pStgmedA, &Medium);
	if (FAILED(hResult))
		goto Error;

	GetWide()->OnDataChange(&FormatEtc, &Medium);

	ConvertSTGMEDIUMFree(pFormatetc->cfFormat, &Medium);

Error:
	ConvertFORMATETCFree(&FormatEtc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2A::OnViewChange, public
//
//  Synopsis:   Thunks OnViewChange to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2A::OnViewChange(DWORD dwAspect, LONG lindex)
{
	_DebugHook(GetWide(), MEMBER_PTR(IAdviseSink, OnViewChange));

	GetWide()->OnViewChange(dwAspect, lindex);
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2A::OnRename, public
//
//  Synopsis:   Thunks OnRename to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2A::OnRename(LPMONIKERA pmkA)
{
	TraceMethodEnter("CAdviseSink2A::OnRename", this);

	LPMONIKER pmk;


	_DebugHook(GetWide(), MEMBER_PTR(IAdviseSink, OnRename));

	WrapIMonikerWFromA(pmkA, &pmk);

	GetWide()->OnRename(pmk);

	if (pmk)
		pmk->Release();
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2A::OnSave, public
//
//  Synopsis:   Thunks OnSave to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2A::OnSave(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IAdviseSink2, OnSave));

	GetWide()->OnSave();
}


//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2A::OnClose, public
//
//  Synopsis:   Thunks OnClose to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2A::OnClose(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IAdviseSink2, OnClose));

	GetWide()->OnClose();
}



//+--------------------------------------------------------------------------
//
//  Member:     CAdviseSink2A::OnLinkSrcChange, public
//
//  Synopsis:   Thunks OnLinkSrcChange to Unicode method.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CAdviseSink2A::OnLinkSrcChange(LPMONIKERA pmkA)
{
	TraceMethodEnter("CAdviseSink2A::OnLinkSrcChange", this);

	LPMONIKER pmk;


	_DebugHook(GetWide(), MEMBER_PTR(IAdviseSink2, OnLinkSrcChange));

	WrapIMonikerWFromA(pmkA, &pmk);

	GetWide()->OnLinkSrcChange(pmk);

	if (pmk)
		pmk->Release();
}



//***************************************************************************
//
//                   IDataAdviseHolderA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDataAdviseHolderA::Advise, public
//
//  Synopsis:   Thunks Advise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderA::Advise(LPDATAOBJECTA pDataObjectA,
		FORMATETCA * pFetc, DWORD advf, LPADVISESINKA pAdviseA,
		DWORD * pdwConnection)
{
	TraceMethodEnter("CDataAdviseHolderA::Advise", this);

	LPDATAOBJECT pDataObject;
	FORMATETC    FormatEtc;
	LPADVISESINK pAdvise;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataAdviseHolder, Advise));

	hResult = WrapIDataObjectWFromA(pDataObjectA, &pDataObject);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertFORMATETCToW(pFetc, &FormatEtc);
	if (FAILED(hResult))
		goto Error;

		hResult = WrapIAdviseSinkWFromA(pAdviseA, &pAdvise);
	if (FAILED(hResult))
		goto Error1;

	hResult = GetWide()->Advise(pDataObject, &FormatEtc, advf, pAdvise, pdwConnection);

	if (pAdvise)
		pAdvise->Release();

Error1:
	if (pDataObject)
		pDataObject->Release();

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataAdviseHolderA::Unadvise, public
//
//  Synopsis:   Thunks Unadvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderA::Unadvise(DWORD dwConnection)
{
	_DebugHook(GetWide(), MEMBER_PTR(IDataAdviseHolder, Unadvise));

	return GetWide()->Unadvise(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDataAdviseHolderA::EnumAdvise, public
//
//  Synopsis:   Thunks EnumAdvise to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderA::EnumAdvise(
		LPENUMSTATDATAA * ppenumAdviseA)
{
	TraceMethodEnter("CDataAdviseHolderA::EnumAdvise", this);

	LPENUMSTATDATA penumAdvise = NULL;
	HRESULT hResult;



	_DebugHook(GetWide(), MEMBER_PTR(IDataAdviseHolder, EnumAdvise));

	hResult = GetWide()->EnumAdvise(&penumAdvise);
	if (FAILED(hResult))
		return hResult;

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
//  Member:     CDataAdviseHolderA::SendOnDataChange, public
//
//  Synopsis:   Thunks SendOnDataChange to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDataAdviseHolderA::SendOnDataChange(LPDATAOBJECTA pDataObjectA,
		DWORD dwReserved, DWORD advf)
{
	TraceMethodEnter("CDataAdviseHolderA::SendOnDataChange", this);

	LPDATAOBJECT pDataObject;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDataAdviseHolder, SendOnDataChange));

	hResult = WrapIDataObjectWFromA(pDataObjectA, &pDataObject);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SendOnDataChange(pDataObject, dwReserved, advf);

	if (pDataObject)
		pDataObject->Release();

	return hResult;
}



//***************************************************************************
//
//                   IOleCache2A Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleCache2A::Cache, public
//
//  Synopsis:   Thunks Cache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2A::Cache(LPFORMATETCA lpFormatetc, DWORD advf,
		LPDWORD lpdwConnection)
{
	TraceMethodEnter("COleCache2A::Cache", this);

	FORMATETC FormatEtc;
	HRESULT   hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleCache, Cache));

	hResult = ConvertFORMATETCToW(lpFormatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Cache(&FormatEtc, advf, lpdwConnection);

	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2A::Uncache, public
//
//  Synopsis:   Thunks Uncache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2A::Uncache(DWORD dwConnection)
{
	_DebugHook(GetWide(), MEMBER_PTR(IOleCache, Uncache));

	return GetWide()->Uncache(dwConnection);
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2A::EnumCache, public
//
//  Synopsis:   Thunks EnumCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2A::EnumCache(LPENUMSTATDATAA * ppenumStatDataA)
{
	TraceMethodEnter("COleCache2A::EnumCache", this);

	LPENUMSTATDATA penumStatData = NULL;
	HRESULT hResult;



	_DebugHook(GetWide(), MEMBER_PTR(IOleCache, EnumCache));

	hResult = GetWide()->EnumCache(&penumStatData);
	if (FAILED(hResult))
		return hResult;

	if (penumStatData)
	{
		hResult = WrapIEnumSTATDATAAFromW(penumStatData, ppenumStatDataA);
		penumStatData->Release();
	}
	else
		*ppenumStatDataA = NULL;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2A::InitCache, public
//
//  Synopsis:   Thunks InitCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2A::InitCache(LPDATAOBJECTA pDataObjectA)
{
	TraceMethodEnter("COleCache2A::InitCache", this);

	LPDATAOBJECT pDataObject;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleCache, InitCache));

	hResult = WrapIDataObjectWFromA(pDataObjectA, &pDataObject);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->InitCache(pDataObject);

	if (pDataObject)
		pDataObject->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2A::SetData, public
//
//  Synopsis:   Thunks SetData to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2A::SetData(LPFORMATETCA pformatetc,
		STGMEDIUMA * pmediumA, BOOL fRelease)
{
	TraceMethodEnter("COleCache2A::SetData", this);

	FORMATETC FormatEtc;
	STGMEDIUM Medium;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleCache, SetData));

	hResult = ConvertFORMATETCToW(pformatetc, &FormatEtc);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertSTGMEDIUMToW(pformatetc->cfFormat, pmediumA, &Medium);
	if (FAILED(hResult))
		goto Error;

	hResult = GetWide()->SetData(&FormatEtc, &Medium, fRelease);

	ConvertSTGMEDIUMFree(pformatetc->cfFormat, &Medium);

Error:
	ConvertFORMATETCFree(&FormatEtc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2A::UpdateCache, public
//
//  Synopsis:   Thunks UpdateCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2A::UpdateCache(LPDATAOBJECTA pDataObjectA,
		DWORD grfUpdf, LPVOID pReserved)
{
	TraceMethodEnter("COleCache2A::UpdateCache", this);

	LPDATAOBJECT pDataObject;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleCache2, UpdateCache));

	hResult = WrapIDataObjectWFromA(pDataObjectA, &pDataObject);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->UpdateCache(pDataObject, grfUpdf, pReserved);

	if (pDataObject)
		pDataObject->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCache2A::DiscardCache, public
//
//  Synopsis:   Thunks DiscardCache to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCache2A::DiscardCache(DWORD dwDiscardOptions)
{
	_DebugHook(GetWide(), MEMBER_PTR(IOleCache2, DiscardCache));

	return GetWide()->DiscardCache(dwDiscardOptions);
}



//***************************************************************************
//
//                   IOleCacheControlA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     COleCacheControlA::OnRun, public
//
//  Synopsis:   Thunks OnRun to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCacheControlA::OnRun(LPDATAOBJECTA pDataObjectA)
{
	TraceMethodEnter("COleCacheControlA::OnRun", this);

	LPDATAOBJECT pDataObject;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IOleCacheControl, OnRun));

	hResult = WrapIDataObjectWFromA(pDataObjectA, &pDataObject);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->OnRun(pDataObject);

	if (pDataObject)
		pDataObject->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     COleCacheControlA::OnStop, public
//
//  Synopsis:   Thunks OnStop to Unicode method.
//
//  Returns:    OLE result code.
//
//---------------------------------------------------------------------------
STDMETHODIMP COleCacheControlA::OnStop(VOID)
{
	_DebugHook(GetWide(), MEMBER_PTR(IOleCacheControl, OnStop));

	return GetWide()->OnStop();
}



//***************************************************************************
//
//                          DvObj API Thunks.
//
//***************************************************************************
STDAPI CreateDataAdviseHolderA(LPDATAADVISEHOLDERA * ppDAHolder)
{
	TraceSTDAPIEnter("CreateDataAdviseHolderA");
	LPDATAADVISEHOLDER pDAHolder;
	HRESULT hResult;


	*ppDAHolder = NULL;

	hResult = CreateDataAdviseHolder(&pDAHolder);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIDataAdviseHolderAFromW(pDAHolder, ppDAHolder);

	if (pDAHolder)
		pDAHolder->Release();

	return hResult;
}
