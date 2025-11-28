//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       widedisp.cpp
//
//  Contents:   ANSI Wrappers for Unicode Diaptch Interface.
//
//  Classes:    CTypeLibW        - Unicode wrapper object for ITypeLibA.
//              CTypeInfoW       - Unicode wrapper object for ITypeInfoA.
//              CTypeCompW       - Unicode wrapper object for ITypeCompA.
//              CCreateTypeLibW  - Unicode wrapper object for ICreateTypeLibA.
//              CCreateTypeInfoW - Unicode wrapper object for ICreateTypeInfoA.
//              CEnumVARIANTW    - Unicode wrapper object for IEnumVARIANTA.
//              CDispatchW       - Unicode wrapper object for IDispatchA.
//
//  History:    01-Nov-93   v-kentc     Created.
//              31-Mar-94   v-kentc     Result return fix ::AddRefTypeInfo.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   ITypeLibW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::GetTypeInfoCount, public
//
//  Synopsis:   Thunks GetTypeInfoCount to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned int) CTypeLibW::GetTypeInfoCount(VOID)
{
	TraceMethodEnter("CTypeLibW::GetTypeInfoCount", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, GetTypeInfoCount));

	return GetANSI()->GetTypeInfoCount();
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::GetTypeInfo, public
//
//  Synopsis:   Thunks GetTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::GetTypeInfo(unsigned int index,
		ITypeInfo * * pptinfo)
{
	TraceMethodEnter("CTypeLibW::GetTypeInfo", this);

	LPTYPEINFOA pTypeInfoA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, GetTypeInfo));

	if (pptinfo == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = GetANSI()->GetTypeInfo(index, &pTypeInfoA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeInfoWFromA(pTypeInfoA, pptinfo);

	if (pTypeInfoA)
		pTypeInfoA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::GetTypeInfoType, public
//
//  Synopsis:   Thunks GetTypeInfoType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::GetTypeInfoType(unsigned int index,
		TYPEKIND * ptypekind)
{
	TraceMethodEnter("CTypeLibW::GetTypeInfoType", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, GetTypeInfoType));

	return GetANSI()->GetTypeInfoType(index, ptypekind);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::GetTypeInfoOfGuid, public
//
//  Synopsis:   Thunks GetTypeInfoOfGuid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::GetTypeInfoOfGuid(REFGUID guid,
		ITypeInfo * * pptinfo)
{
	TraceMethodEnter("CTypeLibW::GetTypeInfoOfGuid", this);

	LPTYPEINFOA pTypeInfoA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, GetTypeInfoOfGuid));

	if (pptinfo == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = GetANSI()->GetTypeInfoOfGuid(guid, &pTypeInfoA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeInfoWFromA(pTypeInfoA, pptinfo);

	if (pTypeInfoA)
		pTypeInfoA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::GetLibAttr, public
//
//  Synopsis:   Thunks GetLibAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::GetLibAttr(TLIBATTR * * pptlibattr)
{
	TraceMethodEnter("CTypeLibW::GetLibAttr", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, GetLibAttr));

	return GetANSI()->GetLibAttr(pptlibattr);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::GetTypeComp, public
//
//  Synopsis:   Thunks GetTypeComp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::GetTypeComp(ITypeComp * * pptcomp)
{
	TraceMethodEnter("CTypeLibW::GetTypeComp", this);

	LPTYPECOMPA pTypeCompA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, GetTypeComp));

	if (pptcomp == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = GetANSI()->GetTypeComp(&pTypeCompA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeCompWFromA(pTypeCompA, pptcomp);

	if (pTypeCompA)
		pTypeCompA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::GetDocumentation, public
//
//  Synopsis:   Thunks GetDocumentation to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::GetDocumentation(int index, BSTR * pbstrName,
		BSTR * pbstrDocString, unsigned long * pdwHelpContext,
		BSTR * pbstrHelpFile)
{
	TraceMethodEnter("CTypeLibW::GetDocumentation", this);

	BSTRA pstrNameA, *ppstrNameA;
	BSTRA pstrDocStringA, *ppstrDocStringA;
	BSTRA pstrHelpFileA, *ppstrHelpFileA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, GetDocumentation));

	if (pbstrName)
		ppstrNameA = &pstrNameA;
	else
		ppstrNameA = NULL;

	if (pbstrDocString)
		ppstrDocStringA = &pstrDocStringA;
	else
		ppstrDocStringA = NULL;

	if (pbstrHelpFile)
		ppstrHelpFileA = &pstrHelpFileA;
	else
		ppstrHelpFileA = NULL;

	hResult = GetANSI()->GetDocumentation(index, ppstrNameA, ppstrDocStringA,
			pdwHelpContext, ppstrHelpFileA);
	if (FAILED(hResult))
		return (hResult);

	if (ppstrNameA)
	{
		hResult = ConvertDispStringToW(pstrNameA, pbstrName);
		if (FAILED(hResult))
			goto Error;

		ConvertDispStringFreeA(pstrNameA);
	}

	if (ppstrDocStringA)
	{
		hResult = ConvertDispStringToW(pstrDocStringA, pbstrDocString);
		if (FAILED(hResult))
			goto Error1;

		ConvertDispStringFreeA(pstrDocStringA);
	}

	if (ppstrHelpFileA)
	{
		hResult = ConvertDispStringToW(pstrHelpFileA, pbstrHelpFile);
		if (FAILED(hResult))
			goto Error2;

		ConvertDispStringFreeA(pstrHelpFileA);
	}

	return ResultFromScode(S_OK);

Error2:
	if (pbstrDocString)
		ConvertDispStringFreeW(*pbstrDocString);

Error1:
	if (pbstrName)
		ConvertDispStringFreeW(*pbstrName);

Error:
	if (ppstrNameA)
		ConvertDispStringFreeA(pstrNameA);
	if (ppstrDocStringA)
		ConvertDispStringFreeA(pstrDocStringA);
	if (ppstrHelpFileA)
		ConvertDispStringFreeA(pstrHelpFileA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::IsName, public
//
//  Synopsis:   Thunks IsName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::IsName(OLECHAR * szNameBuf, unsigned long lHashVal,
		int * lpfName)
{
	TraceMethodEnter("CTypeLibW::IsName", this);

	CHAR szNameBufA[MAX_PATH];
	HRESULT hResult;

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, IsName));

	if (szNameBuf == NULL)
		return ResultFromScode(E_INVALIDARG);

	ConvertStringToA(szNameBuf, szNameBufA);

	hResult = GetANSI()->IsName(szNameBufA, lHashVal, lpfName);

	// copy back result into original string (guaranteed to be same length)
	if (hResult == NOERROR && *lpfName != 0)
	{
		int Count = wcslen(szNameBuf);

		MultiByteToWideChar(CP_ACP, 0, szNameBufA, Count, szNameBuf, Count);
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::FindName, public
//
//  Synopsis:   Thunks FindName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibW::FindName(OLECHAR * szNameBuf, unsigned long lHashVal,
		ITypeInfo * * rgptinfo, MEMBERID * rgmemid, unsigned short * pcFound)
{
	TraceMethodEnter("CTypeLibW::FindName", this);

	CHAR          szNameBufA[MAX_STRING];
	LPTYPEINFOA   pTypeInfoA;
	LPTYPEINFO    pTypeInfo;
	HRESULT       hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, FindName));

	if (szNameBuf == NULL)
		return ResultFromScode(E_INVALIDARG);

	ConvertStringToA(szNameBuf, szNameBufA);

	hResult = GetANSI()->FindName(szNameBufA, lHashVal,
				(LPTYPEINFOA *)rgptinfo,
				rgmemid,
					pcFound);

	if (SUCCEEDED(hResult))
	{
		// now convert the array of ITypeInfoA's to ITypeInfo's in-place
		for (USHORT i = 0; i < *pcFound; i++)
		{
			pTypeInfoA = (LPTYPEINFOA)rgptinfo[i];
			if (SUCCEEDED(hResult))
			{   // if we haven't failed already
				hResult = WrapITypeInfoWFromA((LPTYPEINFOA)rgptinfo[i], &pTypeInfo);
				if (FAILED(hResult))
				{
					// release any wide wrappers we've made so far
					for (USHORT j = 0; j < i; j++)
						rgptinfo[j]->Release();
				}
				else
				{
					rgptinfo[i] = pTypeInfo;
				}
			}
			if (pTypeInfoA)
				pTypeInfoA->Release();
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibW::ReleaseTLibAttr, public
//
//  Synopsis:   Thunks ReleaseTLibAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeLibW::ReleaseTLibAttr(TLIBATTR * ptlibattr)
{
	TraceMethodEnter("CTypeLibW::ReleaseTLibAttr", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeLibA, ReleaseTLibAttr));

	GetANSI()->ReleaseTLibAttr(ptlibattr);
}



//***************************************************************************
//
//                   ITypeInfoW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetTypeAttr, public
//
//  Synopsis:   Thunks GetTypeAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetTypeAttr(TYPEATTR * * pptypeattr)
{
	TraceMethodEnter("CTypeInfoW::GetTypeAttr", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetTypeAttr));

	return GetANSI()->GetTypeAttr(pptypeattr);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetTypeComp, public
//
//  Synopsis:   Thunks GetTypeComp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetTypeComp(ITypeComp * * pptcomp)
{
	TraceMethodEnter("CTypeInfoW::GetTypeComp", this);

	LPTYPECOMPA pTypeCompA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetTypeComp));

	if (pptcomp == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = GetANSI()->GetTypeComp(&pTypeCompA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeCompWFromA(pTypeCompA, pptcomp);

	if (pTypeCompA)
		pTypeCompA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetFuncDesc, public
//
//  Synopsis:   Thunks GetFuncDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetFuncDesc(unsigned int index,
		FUNCDESC * * ppfuncdesc)
{
	TraceMethodEnter("CTypeInfoW::GetFuncDesc", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetFuncDesc));

	return GetANSI()->GetFuncDesc(index, ppfuncdesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetVarDesc, public
//
//  Synopsis:   Thunks GetVarDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetVarDesc(unsigned int index,
		VARDESC * * ppvardesc)
{
	TraceMethodEnter("CTypeInfoW::GetVarDesc", this);

	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetVarDesc));

	if (ppvardesc == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = GetANSI()->GetVarDesc(index, (VARDESCA * *)ppvardesc);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertVARDESCToW(*ppvardesc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetNames, public
//
//  Synopsis:   Thunks GetNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetNames(MEMBERID memid, BSTR * rgbstrNames,
		unsigned int cMaxNames, unsigned int * pcNames)
{
	TraceMethodEnter("CTypeInfoW::GetNames", this);

	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetNames));

	hResult = GetANSI()->GetNames(memid, (BSTRA *)rgbstrNames, cMaxNames,
			pcNames);
	if (FAILED(hResult))
		return (hResult);

	return ConvertDispStringArrayToW(rgbstrNames, *pcNames);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetRefTypeOfImplType, public
//
//  Synopsis:   Thunks GetRefTypeOfImplType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetRefTypeOfImplType(unsigned int index,
		HREFTYPE * phreftype)
{
	TraceMethodEnter("CTypeInfoW::GetRefTypeOfImplType", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetRefTypeOfImplType));

	return GetANSI()->GetRefTypeOfImplType(index, phreftype);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetImplTypeFlags, public
//
//  Synopsis:   Thunks GetImplTypeFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetImplTypeFlags(unsigned int index,
		int * pimpltypeflags)
{
	TraceMethodEnter("CTypeInfoW::GetImplTypeFlags", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetImplTypeFlags));

	return GetANSI()->GetImplTypeFlags(index, pimpltypeflags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetIDsOfNames, public
//
//  Synopsis:   Thunks GetIDsOfNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetIDsOfNames(OLECHAR * * rgszNames,
		unsigned int cNames, MEMBERID * rgmemid)
{
	TraceMethodEnter("CTypeInfoW::GetIDsOfNames", this);

	LPSTR * rgszNamesA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetIDsOfNames));

	if (rgszNames == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = ConvertStringArrayToA(rgszNames, &rgszNamesA, cNames);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->GetIDsOfNames(rgszNamesA, cNames, rgmemid);

	ConvertStringArrayFree(rgszNamesA, cNames);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::Invoke, public
//
//  Synopsis:   Thunks Invoke to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::Invoke(void * pvInstance, MEMBERID memid,
		unsigned short wFlags, DISPPARAMS *pdispparams,
		VARIANT *pvarResult, EXCEPINFO *pexcepinfo,
		unsigned int *puArgErr)
{
	TraceMethodEnter("CTypeInfoW::Invoke", this);

	LPDISPPARAMSA pDispParamsA;
	LPVARIANTA pByrefTempA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, Invoke));

	if (pdispparams == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = ConvertDispParamsToA(pdispparams, &pDispParamsA, &pByrefTempA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Invoke(pvInstance, memid, wFlags, pDispParamsA,
			(LPVARIANTA)pvarResult, (LPEXCEPINFOA)pexcepinfo, puArgErr);

	if (hResult == DISP_E_EXCEPTION)
		ConvertExcepInfoToW(pexcepinfo);

	if (FAILED(hResult))
		goto Error;

	if (pByrefTempA)
	{
		hResult = ConvertDispParamsCopyBack(pdispparams, pByrefTempA);
		if (FAILED(hResult))
			goto Error;
	}

	hResult = ConvertVariantToW(pvarResult);
	if (FAILED(hResult))
		goto Error;

Error:
	ConvertDispParamsFree((LPDISPPARAMS)pDispParamsA, (LPVARIANT)pByrefTempA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetDocumentation, public
//
//  Synopsis:   Thunks GetDocumentation to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetDocumentation(MEMBERID memid, BSTR * pbstrName,
		BSTR * pbstrDocString, unsigned long * pdwHelpContext,
		BSTR * pbstrHelpFile)
{
	TraceMethodEnter("CTypeInfoW::GetDocumentation", this);

	LPSTR pstrNameA, *ppstrNameA;
	LPSTR pstrDocStringA, *ppstrDocStringA;
	LPSTR pstrHelpFileA, *ppstrHelpFileA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetDocumentation));

	if (pbstrName)
		ppstrNameA = &pstrNameA;
	else
		ppstrNameA = NULL;

	if (pbstrDocString)
		ppstrDocStringA = &pstrDocStringA;
	else
		ppstrDocStringA = NULL;

	if (pbstrHelpFile)
		ppstrHelpFileA = &pstrHelpFileA;
	else
		ppstrHelpFileA = NULL;

	hResult = GetANSI()->GetDocumentation(memid, ppstrNameA, ppstrDocStringA,
			pdwHelpContext, ppstrHelpFileA);
	if (FAILED(hResult))
		return (hResult);

	if (ppstrNameA)
	{
		hResult = ConvertDispStringToW(pstrNameA, pbstrName);
		if (FAILED(hResult))
			goto Error;

		ConvertDispStringFreeA(pstrNameA);
	}

	if (ppstrDocStringA)
	{
		hResult = ConvertDispStringToW(pstrDocStringA, pbstrDocString);
		if (FAILED(hResult))
			goto Error1;

		ConvertDispStringFreeA(pstrDocStringA);
	}

	if (ppstrHelpFileA)
	{
		hResult = ConvertDispStringToW(pstrHelpFileA, pbstrHelpFile);
		if (FAILED(hResult))
			goto Error2;

		ConvertDispStringFreeA(pstrHelpFileA);
	}

	return ResultFromScode(S_OK);

Error2:
	if (pbstrDocString)
		ConvertDispStringFreeW(*pbstrDocString);

Error1:
	if (pbstrName)
		ConvertDispStringFreeW(*pbstrName);

Error:
	if (ppstrNameA)
		ConvertDispStringFreeA(pstrNameA);
	if (ppstrDocStringA)
		ConvertDispStringFreeA(pstrDocStringA);
	if (ppstrHelpFileA)
		ConvertDispStringFreeA(pstrHelpFileA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetDllEntry, public
//
//  Synopsis:   Thunks GetDllEntry to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetDllEntry(MEMBERID memid, INVOKEKIND invkind,
		BSTR * pbstrDllName, BSTR * pbstrName,
		unsigned short * pwOrdinal)
{
	TraceMethodEnter("CTypeInfoW::GetDllEntry", this);

	LPSTR pstrDllNameA, *ppstrDllNameA;
	LPSTR pstrNameA, *ppstrNameA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetDllEntry));

	if (pbstrDllName)
		ppstrDllNameA = &pstrDllNameA;
	else
		ppstrDllNameA = NULL;

	if (pbstrName)
		ppstrNameA = &pstrNameA;
	else
		ppstrNameA = NULL;

	hResult = GetANSI()->GetDllEntry(memid, invkind, ppstrDllNameA, ppstrNameA,
			pwOrdinal);
	if (FAILED(hResult))
		return (hResult);

	if (pbstrDllName)
	{
		hResult = ConvertDispStringToW(pstrDllNameA, pbstrDllName);
		if (FAILED(hResult))
			goto Error;

		ConvertDispStringFreeA(pstrDllNameA);
	}

	if (pbstrName)
	{
		hResult = ConvertDispStringToW(pstrNameA, pbstrName);
		if (FAILED(hResult))
			goto Error1;

		ConvertDispStringFreeA(pstrNameA);
	}

	return ResultFromScode(S_OK);

Error1:
	if (pbstrName)
		ConvertDispStringFreeW(*pbstrName);

Error:
	ConvertDispStringFreeA(pstrDllNameA);
	ConvertDispStringFreeA(pstrNameA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetRefTypeInfo, public
//
//  Synopsis:   Thunks GetRefTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetRefTypeInfo(HREFTYPE hreftype,
		ITypeInfo * * pptinfo)
{
	TraceMethodEnter("CTypeInfoW::GetRefTypeInfo", this);

	LPTYPEINFOA pTypeInfoA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetRefTypeInfo));

	if (pptinfo == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = GetANSI()->GetRefTypeInfo(hreftype, &pTypeInfoA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeInfoWFromA(pTypeInfoA, pptinfo);

	if (pTypeInfoA)
		pTypeInfoA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::AddressOfMember, public
//
//  Synopsis:   Thunks AddressOfMember to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::AddressOfMember(MEMBERID memid, INVOKEKIND invkind,
		void * * ppv)
{
	TraceMethodEnter("CTypeInfoW::AddressOfMember", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, AddressOfMember));

	return GetANSI()->AddressOfMember(memid, invkind, ppv);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::CreateInstance, public
//
//  Synopsis:   Thunks CreateInstance to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::CreateInstance(IUnknown * punkOuter, REFIID riid,
		void * * ppvObj)
{
	HRESULT hResult;
	IUnknown * pvObjA;

	TraceMethodEnter("CTypeInfoW::CreateInstance", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, CreateInstance));

	hResult = GetANSI()->CreateInstance(punkOuter, riid, (void **)&pvObjA);
	if (SUCCEEDED(hResult)) {
		hResult = WrapInterfaceWFromA(WrapTranslateIID(riid), pvObjA, (IUnknown**)ppvObj);
	pvObjA->Release();
	}
	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetMops, public
//
//  Synopsis:   Thunks GetMops to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetMops(MEMBERID memid, BSTR * pbstrMops)
{
	TraceMethodEnter("CTypeInfoW::GetMops", this);

	BSTRA bstrMopsA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetMops));

	if (pbstrMops == NULL)
		return ResultFromScode(E_INVALIDARG);

	hResult = GetANSI()->GetMops(memid, &bstrMopsA);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToW(bstrMopsA, pbstrMops);

	ConvertDispStringFreeA(bstrMopsA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::GetContainingTypeLib, public
//
//  Synopsis:   Thunks GetContainingTypeLib to Unicode method.
//              NULL is valid for either (or both) parameter
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoW::GetContainingTypeLib(ITypeLib * * pptlib,
		unsigned int * pindex)
{
	TraceMethodEnter("CTypeInfoW::GetContainingTypeLib", this);

	LPTYPELIBA pTypeLibA;
	LPTYPELIBA *ppTypeLibA;
	HRESULT hResult;

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, GetContainingTypeLib));

	if (pptlib)
		ppTypeLibA = &pTypeLibA;
	else
	ppTypeLibA = NULL;

	hResult = GetANSI()->GetContainingTypeLib(ppTypeLibA, pindex);
	if (FAILED(hResult))
		return (hResult);

	if (pptlib)
	{
		hResult = WrapITypeLibWFromA(pTypeLibA, pptlib);

		if (pTypeLibA)
			pTypeLibA->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::ReleaseTypeAttr, public
//
//  Synopsis:   Thunks ReleaseTypeAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeInfoW::ReleaseTypeAttr(TYPEATTR * ptypeattr)
{
	TraceMethodEnter("CTypeInfoW::ReleaseTypeAttr", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, ReleaseTypeAttr));

	GetANSI()->ReleaseTypeAttr(ptypeattr);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::ReleaseFuncDesc, public
//
//  Synopsis:   Thunks ReleaseFuncDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeInfoW::ReleaseFuncDesc(FUNCDESC * pfuncdesc)
{
	TraceMethodEnter("CTypeInfoW::ReleaseFuncDesc", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, ReleaseFuncDesc));

	GetANSI()->ReleaseFuncDesc(pfuncdesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoW::ReleaseVarDesc, public
//
//  Synopsis:   Thunks ReleaseVarDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeInfoW::ReleaseVarDesc(VARDESC * pvardesc)
{
	TraceMethodEnter("CTypeInfoW::ReleaseVarDesc", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ITypeInfoA, ReleaseVarDesc));

	GetANSI()->ReleaseVarDesc((LPVARDESCA)pvardesc);
}



//***************************************************************************
//
//                   ITypeCompW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CTypeCompW::Bind, public
//
//  Synopsis:   Thunks Bind to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeCompW::Bind(OLECHAR * szName, unsigned long lHashVal,
		unsigned short wflags, ITypeInfo * * pptinfo, DESCKIND * pdesckind,
		BINDPTR * pbindptr)
{
	TraceMethodEnter("CTypeCompW::Bind", this);

	CHAR        szNameA[MAX_STRING];
	LPTYPEINFOA pTypeInfoA;
	HRESULT     hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeCompA, Bind));

	if (szName == NULL)
		return ResultFromScode(E_INVALIDARG);

	ConvertStringToA(szName, szNameA);

	hResult = GetANSI()->Bind(szNameA, lHashVal, wflags, &pTypeInfoA,
			pdesckind, (BINDPTRA *)pbindptr);

	// NOTE: if *pdesckind == DESKIND_NONE, then *pTypeInfoA and *pbindptr
	// are indeterminate.
	if (FAILED(hResult) || *pdesckind == DESCKIND_NONE)
		return hResult;

	if (pTypeInfoA == NULL)
	{
		*pptinfo = NULL;
	}
	else
	{
		hResult = WrapITypeInfoWFromA(pTypeInfoA, pptinfo);
		if (FAILED(hResult))
			goto Error;
	}

	// convert *pbindptr from BINDPTRA to BINDPTR
	switch (*pdesckind)
	{
	case DESCKIND_VARDESC:
	case DESCKIND_IMPLICITAPPOBJ:
		hResult = ConvertVARDESCToW(pbindptr->lpvardesc);
		break;

	case DESCKIND_TYPECOMP:
		{
		LPTYPECOMPA pTypeCompA;
		pTypeCompA = (LPTYPECOMPA)pbindptr->lptcomp;
		hResult = WrapITypeCompWFromA(pTypeCompA, &pbindptr->lptcomp);
		if (pTypeCompA)
			pTypeCompA->Release();
		break;
		}

	//case DESCKIND_NONE:
	//case DESCKIND_FUNCDESC:
	// nothing to do
	}

Error:
	if (pTypeInfoA)
		pTypeInfoA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeCompW::BindType, public
//
//  Synopsis:   Thunks BindType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeCompW::BindType(OLECHAR * szName, unsigned long lHashVal,
		ITypeInfo * * pptinfo, ITypeComp * * pptcomp)
{
	TraceMethodEnter("CTypeCompW::BindType", this);

	CHAR        szNameA[MAX_STRING];
	LPTYPEINFOA pTypeInfoA;
	LPTYPECOMPA pTypeCompA;
	HRESULT     hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ITypeCompA, BindType));

	if (szName == NULL)
		return ResultFromScode(E_INVALIDARG);

	ConvertStringToA(szName, szNameA);

	hResult = GetANSI()->BindType(szNameA, lHashVal, &pTypeInfoA, &pTypeCompA);
	if (FAILED(hResult))
		return hResult;

	if (pTypeInfoA == NULL)
	{
		*pptinfo = NULL;
	}
	else
	{
		hResult = WrapITypeInfoWFromA(pTypeInfoA, pptinfo);
		if (FAILED(hResult))
			goto Error;
	}

	if (pTypeCompA == NULL)
	{
		*pptcomp = NULL;
	}
	else
	{
		hResult = WrapITypeCompWFromA(pTypeCompA, pptcomp);
		if (pTypeCompA)
			pTypeCompA->Release();
	}

Error:
	if (pTypeInfoA)
		pTypeInfoA->Release();

	return hResult;
}



//***************************************************************************
//
//                   ICreateTypeLibW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::CreateTypeInfo, public
//
//  Synopsis:   Thunks CreateTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::CreateTypeInfo(OLECHAR * szName, TYPEKIND tkind,
		ICreateTypeInfo * * lplpictinfo)
{
	TraceMethodEnter("CCreateTypeLibW::CreateTypeInfo", this);

	CHAR              szNameA[MAX_STRING];
	LPCREATETYPEINFOA pTypeInfoA;
	HRESULT           hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, CreateTypeInfo));

	ConvertStringToA(szName, szNameA);

	hResult = GetANSI()->CreateTypeInfo(szNameA, tkind, &pTypeInfoA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapICreateTypeInfoWFromA(pTypeInfoA, lplpictinfo);

	if (pTypeInfoA)
		pTypeInfoA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetName, public
//
//  Synopsis:   Thunks SetName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetName(OLECHAR * szName)
{
	TraceMethodEnter("CCreateTypeLibW::SetName", this);

	CHAR szNameA[MAX_STRING];


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetName));

	ConvertStringToA(szName, szNameA);

	return GetANSI()->SetName(szNameA);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetVersion, public
//
//  Synopsis:   Thunks SetVersion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetVersion(unsigned short wMajorVerNum,
		unsigned short wMinorVerNum)
{
	TraceMethodEnter("CCreateTypeLibW::SetVersion", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetVersion));

	return GetANSI()->SetVersion(wMajorVerNum, wMinorVerNum);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetGuid, public
//
//  Synopsis:   Thunks SetGuid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetGuid(REFGUID guid)
{
	TraceMethodEnter("CCreateTypeLibW::SetGuid", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetGuid));

	return GetANSI()->SetGuid(guid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetDocString, public
//
//  Synopsis:   Thunks SetDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetDocString(OLECHAR * szDoc)
{
	TraceMethodEnter("CCreateTypeLibW::SetDocString", this);

	LPSTR   lpszDocA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetDocString));

	hResult = ConvertStringToA(szDoc, &lpszDocA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetDocString(lpszDocA);

	ConvertStringFree(lpszDocA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetHelpFileName, public
//
//  Synopsis:   Thunks SetHelpFileName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetHelpFileName(OLECHAR * szHelpFileName)
{
	TraceMethodEnter("CCreateTypeLibW::SetHelpFileName", this);

	CHAR    szHelpFileNameA[MAX_STRING];


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetHelpFileName));

	ConvertStringToA(szHelpFileName, szHelpFileNameA);

	return GetANSI()->SetHelpFileName(szHelpFileNameA);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetHelpContext, public
//
//  Synopsis:   Thunks SetHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetHelpContext(unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeLibW::SetHelpContext", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetHelpContext));

	return GetANSI()->SetHelpContext(dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetLcid, public
//
//  Synopsis:   Thunks SetLcid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetLcid(LCID lcid)
{
	TraceMethodEnter("CCreateTypeLibW::SetLcid", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetLcid));

	return GetANSI()->SetLcid(lcid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SetLibFlags, public
//
//  Synopsis:   Thunks SetLibFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SetLibFlags(unsigned int uLibFlags)
{
	TraceMethodEnter("CCreateTypeLibW::SetLibFlags", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SetLibFlags));

	return GetANSI()->SetLibFlags(uLibFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibW::SaveAllChanges, public
//
//  Synopsis:   Thunks SaveAllChanges to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibW::SaveAllChanges(VOID)
{
	TraceMethodEnter("CCreateTypeLibW::SaveAllChanges", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeLibA, SaveAllChanges));

	return GetANSI()->SaveAllChanges();
}



//***************************************************************************
//
//                   ICreateTypeInfoW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetGuid, public
//
//  Synopsis:   Thunks SetGuid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetGuid(REFGUID guid)
{
	TraceMethodEnter("CCreateTypeInfoW::SetGuid", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetGuid));

	return GetANSI()->SetGuid(guid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetTypeFlags, public
//
//  Synopsis:   Thunks SetTypeFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetTypeFlags(unsigned int uTypeFlags)
{
	TraceMethodEnter("CCreateTypeInfoW::SetTypeFlags", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetTypeFlags));

	return GetANSI()->SetTypeFlags(uTypeFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetDocString, public
//
//  Synopsis:   Thunks SetDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetDocString(OLECHAR * pstrDoc)
{
	TraceMethodEnter("CCreateTypeInfoW::SetDocString", this);

	LPSTR   lpszDocA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetDocString));

	hResult = ConvertStringToA(pstrDoc, &lpszDocA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetDocString(lpszDocA);

	ConvertStringFree(lpszDocA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetHelpContext, public
//
//  Synopsis:   Thunks SetHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetHelpContext(unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeInfoW::SetHelpContext", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetHelpContext));

	return GetANSI()->SetHelpContext(dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetVersion, public
//
//  Synopsis:   Thunks SetVersion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetVersion(unsigned short wMajorVerNum,
		unsigned short wMinorVerNum)
{
	TraceMethodEnter("CCreateTypeInfoW::SetVersion", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetVersion));

	return GetANSI()->SetVersion(wMajorVerNum, wMinorVerNum);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::AddRefTypeInfo, public
//
//  Synopsis:   Thunks AddRefTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::AddRefTypeInfo(ITypeInfo * ptinfo,
		HREFTYPE * phreftype)
{
	TraceMethodEnter("CCreateTypeInfoW::AddRefTypeInfo", this);

	ITypeInfoA * pTypeInfoA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, AddRefTypeInfo));

	// ptinfo->QueryInterface(IID_ITypeInfo, (PVOID *)&pTypeInfoA);
	if( FAILED( hResult = WrapITypeInfoAFromW( ptinfo, &pTypeInfoA )))
	  return hResult;

	hResult = GetANSI()->AddRefTypeInfo(pTypeInfoA, phreftype);

	pTypeInfoA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::AddFuncDesc, public
//
//  Synopsis:   Thunks AddFuncDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::AddFuncDesc(unsigned int index,
		FUNCDESC * pfuncdesc)
{
	TraceMethodEnter("CCreateTypeInfoW::AddFuncDesc", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, AddFuncDesc));

	return GetANSI()->AddFuncDesc(index, pfuncdesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::AddImplType, public
//
//  Synopsis:   Thunks AddImplType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::AddImplType(unsigned int index,
		HREFTYPE hreftype)
{
	TraceMethodEnter("CCreateTypeInfoW::AddImplType", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, AddImplType));

	return GetANSI()->AddImplType(index, hreftype);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetImplTypeFlags, public
//
//  Synopsis:   Thunks SetImplTypeFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetImplTypeFlags(unsigned int index,
		int impltypeflags)
{
	TraceMethodEnter("CCreateTypeInfoW::SetImplTypeFlags", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetImplTypeFlags));

	return GetANSI()->SetImplTypeFlags(index, impltypeflags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetAlignment, public
//
//  Synopsis:   Thunks SetAlignment to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetAlignment(unsigned short cbAlignment)
{
	TraceMethodEnter("CCreateTypeInfoW::SetAlignment", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetAlignment));

	return GetANSI()->SetAlignment(cbAlignment);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetSchema, public
//
//  Synopsis:   Thunks SetSchema to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetSchema(OLECHAR * lpstrSchema)
{
	TraceMethodEnter("CCreateTypeInfoW::SetSchema", this);

	LPSTR   lpszSchemaA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetSchema));

	hResult = ConvertStringToA(lpstrSchema, &lpszSchemaA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetSchema(lpszSchemaA);

	ConvertStringFree(lpszSchemaA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::AddVarDesc, public
//
//  Synopsis:   Thunks AddVarDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::AddVarDesc(unsigned int index,
		VARDESC * pvardesc)
{
	TraceMethodEnter("CCreateTypeInfoW::AddVarDesc", this);

	LPVARDESCA pVarDescA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, AddVarDesc));

	hResult = ConvertVARDESCToA(pvardesc, &pVarDescA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->AddVarDesc(index, pVarDescA);

	ConvertVARDESCFree(pVarDescA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetFuncAndParamNames, public
//
//  Synopsis:   Thunks SetFuncAndParamNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetFuncAndParamNames(unsigned int index,
		OLECHAR * * rgszNames, unsigned int cNames)
{
	TraceMethodEnter("CCreateTypeInfoW::SetFuncAndParamNames", this);

	LPSTR * rgszNamesA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetFuncAndParamNames));

	hResult = ConvertStringArrayToA(rgszNames, &rgszNamesA, cNames);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetFuncAndParamNames(index, rgszNamesA, cNames);

	ConvertStringArrayFree(rgszNamesA, cNames);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetVarName, public
//
//  Synopsis:   Thunks SetVarName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetVarName(unsigned int index, OLECHAR * szName)
{
	TraceMethodEnter("CCreateTypeInfoW::SetVarName", this);

	CHAR    szNameA[MAX_PATH];


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetVarName));

	ConvertStringToA(szName, szNameA);

	return GetANSI()->SetVarName(index, szNameA);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetTypeDescAlias, public
//
//  Synopsis:   Thunks SetTypeDescAlias to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetTypeDescAlias(TYPEDESC * ptdescAlias)
{
	TraceMethodEnter("CCreateTypeInfoW::SetTypeDescAlias", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetTypeDescAlias));

	return GetANSI()->SetTypeDescAlias(ptdescAlias);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::DefineFuncAsDllEntry, public
//
//  Synopsis:   Thunks DefineFuncAsDllEntry to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::DefineFuncAsDllEntry(unsigned int index,
		OLECHAR * szDllName, OLECHAR * szProcName)
{
	TraceMethodEnter("CCreateTypeInfoW::DefineFuncAsDllEntry", this);

	LPSTR lpszDllNameA;
	LPSTR lpszProcNameA;
	HRESULT  hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, DefineFuncAsDllEntry));

	hResult = ConvertStringToA(szDllName, &lpszDllNameA);
	if (FAILED(hResult))
		return (hResult);

	if (HIWORD(szProcName))
	{
		// if not call by ordinal
		hResult = ConvertStringToA(szProcName, &lpszProcNameA);
		if (FAILED(hResult))
			goto Error;
	}
	else
	{
		// really an ordinal
		lpszProcNameA = (LPSTR)szProcName;
	}

	hResult = GetANSI()->DefineFuncAsDllEntry(index, lpszDllNameA, lpszProcNameA);

	if (HIWORD(szProcName))
	{
		// if not call by ordinal
		ConvertStringFree(lpszProcNameA);
	}

Error:
	ConvertStringFree(lpszDllNameA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetFuncDocString, public
//
//  Synopsis:   Thunks SetFuncDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetFuncDocString(unsigned int index,
		OLECHAR * szDocString)
{
	TraceMethodEnter("CCreateTypeInfoW::SetFuncDocString", this);

	LPSTR   lpszDocStringA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetFuncDocString));

	hResult = ConvertStringToA(szDocString, &lpszDocStringA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetFuncDocString(index, lpszDocStringA);

	ConvertStringFree(lpszDocStringA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetVarDocString, public
//
//  Synopsis:   Thunks SetVarDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetVarDocString(unsigned int index,
		OLECHAR * szDocString)
{
	TraceMethodEnter("CCreateTypeInfoW::SetVarDocString", this);

	LPSTR   lpszDocStringA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetVarDocString));

	hResult = ConvertStringToA(szDocString, &lpszDocStringA);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->SetVarDocString(index, lpszDocStringA);

	ConvertStringFree(lpszDocStringA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetFuncHelpContext, public
//
//  Synopsis:   Thunks SetFuncHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetFuncHelpContext(unsigned int index,
		unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeInfoW::SetFuncHelpContext", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetFuncHelpContext));

	return GetANSI()->SetFuncHelpContext(index, dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetVarHelpContext, public
//
//  Synopsis:   Thunks SetVarHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetVarHelpContext(unsigned int index,
		unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeInfoW::SetVarHelpContext", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetVarHelpContext));

	return GetANSI()->SetVarHelpContext(index, dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetMops, public
//
//  Synopsis:   Thunks SetMops to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetMops(unsigned int index, BSTR bstrMops)
{
	TraceMethodEnter("CCreateTypeInfoW::SetMops", this);

	BSTRA bstrMopsA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetMops));

	hResult = ConvertStringToA(bstrMops, &bstrMopsA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->SetMops(index, bstrMopsA);

	ConvertStringFree(bstrMopsA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::SetTypeIdldesc, public
//
//  Synopsis:   Thunks SetTypeIdldesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::SetTypeIdldesc(IDLDESC * pidldesc)
{
	TraceMethodEnter("CCreateTypeInfoW::SetTypeIdldesc", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, SetTypeIdldesc));

	return GetANSI()->SetTypeIdldesc(pidldesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoW::LayOut, public
//
//  Synopsis:   Thunks LayOut to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoW::LayOut(VOID)
{
	TraceMethodEnter("CCreateTypeInfoW::LayOut", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateTypeInfoA, LayOut));

	return GetANSI()->LayOut();
}



//***************************************************************************
//
//                   IEnumVARIANTW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTW::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTW::Next(unsigned long celt, VARIANT * rgvar,
		unsigned long * pceltFetched)
{
	TraceMethodEnter("CEnumVARIANTW::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumVARIANTA, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetANSI()->Next(celt, (VARIANTA *)rgvar, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertVariantArrayToW(rgvar, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}



//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTW::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTW::Skip(unsigned long celt)
{
	TraceMethodEnter("CEnumVARIANTW::Skip", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumVARIANTA, Skip));

	return GetANSI()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTW::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTW::Reset(VOID)
{
	TraceMethodEnter("CEnumVARIANTW::Reset", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IEnumVARIANTA, Reset));

	return GetANSI()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTW::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTW::Clone(IEnumVARIANT * * ppenm)
{
	TraceMethodEnter("CEnumVARIANTW::Clone", this);

	IEnumVARIANTA * penmA;


	_DebugHook(GetANSI(), MEMBER_PTR(IEnumVARIANTA, Clone));

	HRESULT hResult = GetANSI()->Clone(&penmA);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumVARIANTWFromA(penmA, ppenm);

	if (penmA)
		penmA->Release();

	return hResult;
}



//***************************************************************************
//
//                   IDispatchW Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDispatchW::GetTypeInfoCount, public
//
//  Synopsis:   Thunks GetTypeInfoCount to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchW::GetTypeInfoCount(unsigned int * pctinfo)
{
	TraceMethodEnter("CDispatchW::GetTypeInfoCount", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IDispatchA, GetTypeInfoCount));

	return GetANSI()->GetTypeInfoCount(pctinfo);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDispatchW::GetTypeInfo, public
//
//  Synopsis:   Thunks GetTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchW::GetTypeInfo(unsigned int itinfo, LCID lcid,
		ITypeInfo * * pptinfo)
{
	TraceMethodEnter("CDispatchW::GetTypeInfo", this);

	LPTYPEINFOA pTypeInfoA;
	HRESULT     hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDispatchA, GetTypeInfo));

	hResult = GetANSI()->GetTypeInfo(itinfo, lcid, &pTypeInfoA);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapITypeInfoWFromA(pTypeInfoA, pptinfo);

	if (pTypeInfoA)
		pTypeInfoA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDispatchW::GetIDsOfNames, public
//
//  Synopsis:   Thunks GetIDsOfNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchW::GetIDsOfNames(REFIID riid, OLECHAR * * rgszNames,
		unsigned int cNames, LCID lcid, DISPID * rgdispid)
{
	TraceMethodEnter("CDispatchW::GetIDsOfNames", this);

	LPSTR * rgszNamesA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDispatchA, GetIDsOfNames));

	hResult = ConvertStringArrayToA(rgszNames, &rgszNamesA, cNames);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetANSI()->GetIDsOfNames(riid, rgszNamesA, cNames, lcid, rgdispid);

	ConvertStringArrayFree(rgszNamesA, cNames);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDispatchW::Invoke, public
//
//  Synopsis:   Thunks Invoke to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//  History:    01-Jul-94  SatishC  Updated to have the same fix that
//                  PatL made for CDispatchA::Invoke
//                  for ExcepInfo conversion.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchW::Invoke(DISPID dispidMember, REFIID riid, LCID lcid,
		unsigned short wFlags, DISPPARAMS * pdispparams,
		VARIANT * pvarResult, EXCEPINFO * pexcepinfo,
		unsigned int * puArgErr)
{
	TraceMethodEnter("CDispatchW::Invoke", this);

	LPDISPPARAMSA pDispParamsA;
	LPVARIANTA pByrefTempA;
	HRESULT hResult;


	_DebugHook(GetANSI(), MEMBER_PTR(IDispatchA, Invoke));

	hResult = ConvertDispParamsToA(pdispparams, &pDispParamsA, &pByrefTempA);
	if (FAILED(hResult))
		return hResult;

	hResult = GetANSI()->Invoke(dispidMember, riid, lcid, wFlags, pDispParamsA,
			(LPVARIANTA)pvarResult, (LPEXCEPINFOA)pexcepinfo, puArgErr);

	if (hResult == DISP_E_EXCEPTION)
		ConvertExcepInfoToW(pexcepinfo);

	if (FAILED(hResult))
		goto Error;

	if (pByrefTempA)
	{
		hResult = ConvertDispParamsCopyBack(pdispparams, pByrefTempA);
		if (FAILED(hResult))
			goto Error;
	}

	hResult = ConvertVariantToW(pvarResult);
	if (FAILED(hResult))
		goto Error;

Error:
	ConvertDispParamsFree((LPDISPPARAMS)pDispParamsA, (LPVARIANT)pByrefTempA);

	return hResult;
}


#ifndef NOERRORINFO
//***************************************************************************
//
//                   IErrorInfoW Implementation
//
//***************************************************************************

STDMETHODIMP
CErrorInfoW::GetGUID(GUID *pguid)
{
	TraceMethodEnter("CErrorInfoW::GetGUID", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IErrorInfoA, GetGUID));

	return GetANSI()->GetGUID(pguid);
}

STDMETHODIMP
CErrorInfoW::GetSource(BSTR *pbstrSource)
{
	TraceMethodEnter("CErrorInfoW::GetSource", this);

	HRESULT hresult;
	BSTRA bstraSource;

	bstraSource = NULL;

	_DebugHook(GetANSI(), MEMBER_PTR(IErrorInfoA, GetSource));

	hresult = GetANSI()->GetSource(&bstraSource);
	if(FAILED(hresult))
	  return hresult;

	hresult = ConvertDispStringToW(bstraSource, pbstrSource);

	SysFreeStringA(bstraSource);

	return hresult;
}

STDMETHODIMP
CErrorInfoW::GetDescription(BSTR *pbstrDescription)
{
	TraceMethodEnter("CErrorInfoW::GetDescription", this);

	HRESULT hresult;
	BSTRA bstraDescription;

	bstraDescription = NULL;

	_DebugHook(GetANSI(), MEMBER_PTR(IErrorInfoA, GetDescription));

	hresult = GetANSI()->GetDescription(&bstraDescription);
	if(FAILED(hresult))
	  return hresult;

	hresult = ConvertDispStringToW(bstraDescription, pbstrDescription);

	SysFreeStringA(bstraDescription);

	return hresult;
}

STDMETHODIMP
CErrorInfoW::GetHelpFile(BSTR *pbstrHelpFile)
{
	TraceMethodEnter("CErrorInfoW::GetHelpFile", this);

	HRESULT hresult;
	BSTRA bstraHelpFile;

	bstraHelpFile = NULL;

	_DebugHook(GetANSI(), MEMBER_PTR(IErrorInfoA, GetHelpFile));

	hresult = GetANSI()->GetHelpFile(&bstraHelpFile);
	if(FAILED(hresult))
	  return hresult;

	hresult = ConvertDispStringToW(bstraHelpFile, pbstrHelpFile);

	SysFreeStringA(bstraHelpFile);

	return hresult;
}

STDMETHODIMP
CErrorInfoW::GetHelpContext(DWORD *pdwHelpContext)
{
	TraceMethodEnter("CErrorInfoW::GetHelpContext", this);

	_DebugHook(GetANSI(), MEMBER_PTR(IErrorInfoA, GetHelpContext));

	return GetANSI()->GetHelpContext(pdwHelpContext);
}


//***************************************************************************
//
//                   ICreateErrorInfoW Implementation
//
//***************************************************************************

STDMETHODIMP
CCreateErrorInfoW::SetGUID(REFGUID rguid)
{
	TraceMethodEnter("CCreateErrorInfoW::SetGUID", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateErrorInfoA, SetGUID));

	return GetANSI()->SetGUID(rguid);
}

STDMETHODIMP
CCreateErrorInfoW::SetSource(LPOLESTR szSource)
{
	TraceMethodEnter("CCreateErrorInfoW::SetSource", this);

	HRESULT hresult;
	LPSTR szaSource;

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateErrorInfoA, SetSource));

	hresult = ConvertStringToA(szSource, &szaSource);
	if(FAILED(hresult))
	  return hresult;

	hresult = GetANSI()->SetSource(szaSource);

	ConvertStringFree(szaSource);

	return hresult;
}

STDMETHODIMP
CCreateErrorInfoW::SetDescription(LPOLESTR szDescription)
{
	TraceMethodEnter("CCreateErrorInfoW::SetDescription", this);

	HRESULT hresult;
	LPSTR szaDescription;

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateErrorInfoA, SetDescription));

	hresult = ConvertStringToA(szDescription, &szaDescription);
	if(FAILED(hresult))
	  return hresult;

	hresult = GetANSI()->SetDescription(szaDescription);

	ConvertStringFree(szaDescription);

	return hresult;
}

STDMETHODIMP
CCreateErrorInfoW::SetHelpFile(LPOLESTR szHelpFile)
{
	TraceMethodEnter("CCreateErrorInfoW::SetHelpFile", this);

	HRESULT hresult;
	LPSTR szaHelpFile;

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateErrorInfoA, SetHelpFile));

	hresult = ConvertStringToA(szHelpFile, &szaHelpFile);
	if(FAILED(hresult))
	  return hresult;

	hresult = GetANSI()->SetHelpFile(szaHelpFile);

	ConvertStringFree(szaHelpFile);

	return hresult;
}

STDMETHODIMP
CCreateErrorInfoW::SetHelpContext(DWORD dwHelpContext)
{
	TraceMethodEnter("CCreateErrorInfoW::SetHelpContext", this);

	_DebugHook(GetANSI(), MEMBER_PTR(ICreateErrorInfoA, SetHelpContext));

	return GetANSI()->SetHelpContext(dwHelpContext);
}

#endif  //!NOERRORINFO
