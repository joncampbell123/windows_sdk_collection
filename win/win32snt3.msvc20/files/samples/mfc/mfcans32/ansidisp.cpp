//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ansidisp.cpp
//
//  Contents:   ANSI Wrappers for Dispatch Interfaces and APIs.
//
//  Classes:    CTypeLibA        - ANSI wrapper object for ITypeLib.
//              CTypeInfoA       - ANSI wrapper object for ITypeInfo.
//              CTypeCompA       - ANSI wrapper object for ITypeComp.
//              CCreateTypeLibA  - ANSI wrapper object for ICreateTypeLib.
//              CCreateTypeInfoA - ANSI wrapper object for ICreateTypeInfo.
//              CEnumVARIANTA    - ANSI wrapper object for IEnumVARIANT.
//              CDispatchA       - ANSI wrapper object for IDispatch.
//
//  History:    01-Nov-93   v-kentc     Created.
//              31-Mar-94   v-kentc     Result return fix ::AddRefTypeInfo.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"



//***************************************************************************
//
//                   ITypeLibA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::GetTypeInfoCount, public
//
//  Synopsis:   Thunks GetTypeInfoCount to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(unsigned int) CTypeLibA::GetTypeInfoCount(VOID)
{
	TraceMethodEnter("CTypeLibA::GetTypeInfoCount", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, GetTypeInfoCount));

	return GetWide()->GetTypeInfoCount();
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::GetTypeInfo, public
//
//  Synopsis:   Thunks GetTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::GetTypeInfo(unsigned int index,
		ITypeInfoA * * pptinfo)
{
	TraceMethodEnter("CTypeLibA::GetTypeInfo", this);

	LPTYPEINFO pTypeInfo;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, GetTypeInfo));

	hResult = GetWide()->GetTypeInfo(index, &pTypeInfo);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeInfoAFromW(pTypeInfo, pptinfo);

	pTypeInfo->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::GetTypeInfoType, public
//
//  Synopsis:   Thunks GetTypeInfoType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::GetTypeInfoType(unsigned int index,
		TYPEKIND * ptypekind)
{
	TraceMethodEnter("CTypeLibA::GetTypeInfoType", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, GetTypeInfoType));

	return GetWide()->GetTypeInfoType(index, ptypekind);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::GetTypeInfoOfGuid, public
//
//  Synopsis:   Thunks GetTypeInfoOfGuid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::GetTypeInfoOfGuid(
	  REFGUID guid, ITypeInfoA * * pptinfo)
{
	TraceMethodEnter("CTypeLibA::GetTypeInfoOfGuid", this);

	LPTYPEINFO pTypeInfo;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, GetTypeInfoOfGuid));

	hResult = GetWide()->GetTypeInfoOfGuid(guid, &pTypeInfo);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeInfoAFromW(pTypeInfo, pptinfo);

	pTypeInfo->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::GetLibAttr, public
//
//  Synopsis:   Thunks GetLibAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::GetLibAttr(TLIBATTR * * pptlibattr)
{
	TraceMethodEnter("CTypeLibA::GetLibAttr", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, GetLibAttr));

	return GetWide()->GetLibAttr(pptlibattr);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::GetTypeComp, public
//
//  Synopsis:   Thunks GetTypeComp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::GetTypeComp(ITypeCompA * * pptcomp)
{
	TraceMethodEnter("CTypeLibA::GetTypeComp", this);

	LPTYPECOMP pTypeComp;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, GetTypeComp));

	hResult = GetWide()->GetTypeComp(&pTypeComp);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeCompAFromW(pTypeComp, pptcomp);

	pTypeComp->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::GetDocumentation, public
//
//  Synopsis:   Thunks GetDocumentation to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::GetDocumentation(int index, BSTRA * pbstrNameA,
		BSTRA * pbstrDocStringA, unsigned long * pdwHelpContext,
		BSTRA * pbstrHelpFileA)
{
	TraceMethodEnter("CTypeLibA::GetDocumentation", this);

	BSTR pstrName, *ppstrName;
	BSTR pstrDocString, *ppstrDocString;
	BSTR pstrHelpFile, *ppstrHelpFile;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, GetDocumentation));

	if (pbstrNameA)
		ppstrName = &pstrName;
	else
		ppstrName = NULL;

	if (pbstrDocStringA)
		ppstrDocString = &pstrDocString;
	else
		ppstrDocString = NULL;

	if (pbstrHelpFileA)
		ppstrHelpFile = &pstrHelpFile;
	else
		ppstrHelpFile = NULL;

	hResult = GetWide()->GetDocumentation(index, ppstrName, ppstrDocString,
			pdwHelpContext, ppstrHelpFile);
	if (FAILED(hResult))
		return (hResult);

	if (ppstrName)
	{
		hResult = ConvertDispStringToA(pstrName, pbstrNameA);
		if (FAILED(hResult))
			goto Error;

		SysFreeString(pstrName);
	}

	if (ppstrDocString)
	{
		hResult = ConvertDispStringToA(pstrDocString, pbstrDocStringA);
		if (FAILED(hResult))
			goto Error1;

		SysFreeString(pstrDocString);
	}

	if (ppstrHelpFile)
	{
		hResult = ConvertDispStringToA(pstrHelpFile, pbstrHelpFileA);
		if (FAILED(hResult))
			goto Error2;

		SysFreeString(pstrHelpFile);
	}

	return NOERROR;

Error2:
	if (pbstrDocStringA)
		ConvertDispStringFreeA(*pbstrDocStringA);

Error1:
	if (pbstrNameA)
		ConvertDispStringFreeA(*pbstrNameA);

Error:
	if (ppstrName)
		SysFreeString(pstrName);
	if (ppstrDocString)
		SysFreeString(pstrDocString);
	if (ppstrHelpFile)
		SysFreeString(pstrHelpFile);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::IsName, public
//
//  Synopsis:   Thunks IsName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::IsName(CHAR * szNameBufA, unsigned long lHashVal,
		int * lpfName)
{
	TraceMethodEnter("CTypeLibA::IsName", this);

	OLECHAR szNameBuf[MAX_STRING];
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, IsName));

	ConvertStringToW(szNameBufA, szNameBuf);

	hResult = GetWide()->IsName(szNameBuf, lHashVal, lpfName);

	// copy back result into original string (guaranteed to be same length)
	if (hResult == NOERROR && *lpfName != 0)
	{
		int Count = lstrlen(szNameBufA);

		WideCharToMultiByte(CP_ACP, 0, szNameBuf, Count, szNameBufA, Count, NULL, NULL);
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::FindName, public
//
//  Synopsis:   Thunks FindName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeLibA::FindName(CHAR * szNameBufA, unsigned long lHashVal,
		ITypeInfoA * * rgptinfo, MEMBERID * rgmemid, unsigned short * pcFound)
{
	TraceMethodEnter("CTypeLibA::FindName", this);

	OLECHAR       szNameBuf[MAX_STRING];
	LPTYPEINFO    pTypeInfo;
	LPTYPEINFOA   pTypeInfoA;
	HRESULT       hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, FindName));

	ConvertStringToW(szNameBufA, szNameBuf);

	hResult = GetWide()->FindName(szNameBuf, lHashVal,
				(LPTYPEINFO *)rgptinfo,
				rgmemid,
					pcFound);

	if (SUCCEEDED(hResult))
	{
		// now convert the array of ITypeInfo's to ITypeInfoA's in-place

		for (USHORT i = 0; i < *pcFound; i++)
		{
			pTypeInfo = (LPTYPEINFO)rgptinfo[i];
			if (SUCCEEDED(hResult))   // if we haven't failed already
			{
				hResult = WrapITypeInfoAFromW((LPTYPEINFO)rgptinfo[i], &pTypeInfoA);
				if (FAILED(hResult))  // release any Ansi wrappers we've made so far
				{
					for (USHORT j = 0; j < i; j++)
					{
						rgptinfo[j]->Release();
					}
				}
				else
				{
					rgptinfo[i] = pTypeInfoA;
				}
			}
			pTypeInfo->Release();
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeLibA::ReleaseTLibAttr, public
//
//  Synopsis:   Thunks ReleaseTLibAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeLibA::ReleaseTLibAttr(TLIBATTR * ptlibattr)
{
	TraceMethodEnter("CTypeLibA::ReleaseTLibAttr", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeLib, ReleaseTLibAttr));

	GetWide()->ReleaseTLibAttr(ptlibattr);
}



//***************************************************************************
//
//                   ITypeInfoA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetTypeAttr, public
//
//  Synopsis:   Thunks GetTypeAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetTypeAttr(TYPEATTR * * pptypeattr)
{
	TraceMethodEnter("CTypeInfoA::GetTypeAttr", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetTypeAttr));

	return GetWide()->GetTypeAttr(pptypeattr);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetTypeComp, public
//
//  Synopsis:   Thunks GetTypeComp to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetTypeComp(ITypeCompA * * pptcomp)
{
	TraceMethodEnter("CTypeInfoA::GetTypeComp", this);

	LPTYPECOMP pTypeComp;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetTypeComp));

	hResult = GetWide()->GetTypeComp(&pTypeComp);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeCompAFromW(pTypeComp, pptcomp);

	pTypeComp->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetFuncDesc, public
//
//  Synopsis:   Thunks GetFuncDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetFuncDesc(unsigned int index,
		FUNCDESC * * ppfuncdesc)
{
	TraceMethodEnter("CTypeInfoA::GetFuncDesc", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetFuncDesc));

	return GetWide()->GetFuncDesc(index, ppfuncdesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetVarDesc, public
//
//  Synopsis:   Thunks GetVarDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetVarDesc(unsigned int index,
		VARDESCA * * ppvardescA)
{
	TraceMethodEnter("CTypeInfoA::GetVarDesc", this);

	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetVarDesc));

	hResult = GetWide()->GetVarDesc(index, (VARDESC * *)ppvardescA);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertVARDESCToA(*ppvardescA);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetNames, public
//
//  Synopsis:   Thunks GetNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetNames(MEMBERID memid, BSTRA * rgbstrNames,
		unsigned int cMaxNames, unsigned int * pcNames)
{
	TraceMethodEnter("CTypeInfoA::GetNames", this);

	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetNames));

	hResult = GetWide()->GetNames(memid, (BSTR *)rgbstrNames, cMaxNames,
			pcNames);
	if (FAILED(hResult))
		return (hResult);

	return ConvertDispStringArrayToA(rgbstrNames, *pcNames);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetRefTypeOfImplType, public
//
//  Synopsis:   Thunks GetRefTypeOfImplType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetRefTypeOfImplType(unsigned int index,
		HREFTYPE * phreftype)
{
	TraceMethodEnter("CTypeInfoA::GetRefTypeOfImplType", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetRefTypeOfImplType));

	return GetWide()->GetRefTypeOfImplType(index, phreftype);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetImplTypeFlags, public
//
//  Synopsis:   Thunks GetImplTypeFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetImplTypeFlags(unsigned int index,
		int * pimpltypeflags)
{
	TraceMethodEnter("CTypeInfoA::GetImplTypeFlags", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetImplTypeFlags));

	return GetWide()->GetImplTypeFlags(index, pimpltypeflags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetIDsOfNames, public
//
//  Synopsis:   Thunks GetIDsOfNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetIDsOfNames(CHAR * * rgszNamesA,
		unsigned int cNames, MEMBERID * rgmemid)
{
	TraceMethodEnter("CTypeInfoA::GetIDsOfNames", this);

	LPWSTR * rgszNames;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetIDsOfNames));

	hResult = ConvertStringArrayToW(rgszNamesA, &rgszNames, cNames);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->GetIDsOfNames(rgszNames, cNames, rgmemid);

	ConvertStringArrayFree(rgszNames, cNames);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::Invoke, public
//
//  Synopsis:   Thunks Invoke to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::Invoke(void * pvInstance, MEMBERID memid,
		unsigned short wFlags, DISPPARAMSA *pdispparamsA,
		VARIANTA *pvarResultA, EXCEPINFOA *pexcepinfoA,
		unsigned int *puArgErr)
{
	TraceMethodEnter("CTypeInfoA::Invoke", this);

	LPDISPPARAMS pDispParams;
	LPVARIANT pByrefTemp;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, Invoke));

	hResult = ConvertDispParamsToW(pdispparamsA, &pDispParams, &pByrefTemp);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Invoke(pvInstance, memid, wFlags, pDispParams,
			(LPVARIANT)pvarResultA, (EXCEPINFO *)pexcepinfoA, puArgErr);

	if (hResult == DISP_E_EXCEPTION)
		ConvertExcepInfoToA(pexcepinfoA);

	if (FAILED(hResult))
		goto Error;

	if (pByrefTemp)
	{
		hResult = ConvertDispParamsCopyBack(pdispparamsA, pByrefTemp);
		if (FAILED(hResult))
			goto Error;
	}

	hResult = ConvertVariantToA(pvarResultA);
	if (FAILED(hResult))
		goto Error;

Error:
	ConvertDispParamsFree(pDispParams, pByrefTemp);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetDocumentation, public
//
//  Synopsis:   Thunks GetDocumentation to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetDocumentation(MEMBERID memid, BSTRA * pbstrNameA,
		BSTRA * pbstrDocStringA, unsigned long * pdwHelpContext,
		BSTRA * pbstrHelpFileA)
{
	TraceMethodEnter("CTypeInfoA::GetDocumentation", this);

	BSTR pstrName, *ppstrName;
	BSTR pstrDocString, *ppstrDocString;
	BSTR pstrHelpFile, *ppstrHelpFile;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetDocumentation));

	if (pbstrNameA)
		ppstrName = &pstrName;
	else
		ppstrName = NULL;

	if (pbstrDocStringA)
		ppstrDocString = &pstrDocString;
	else
		ppstrDocString = NULL;

	if (pbstrHelpFileA)
		ppstrHelpFile = &pstrHelpFile;
	else
		ppstrHelpFile = NULL;

	hResult = GetWide()->GetDocumentation(memid, ppstrName, ppstrDocString,
			pdwHelpContext, ppstrHelpFile);
	if (FAILED(hResult))
		return (hResult);

	if (ppstrName)
	{
		hResult = ConvertDispStringToA(pstrName, pbstrNameA);
		if (FAILED(hResult))
			goto Error;

		SysFreeString(pstrName);
	}

	if (ppstrDocString)
	{
		hResult = ConvertDispStringToA(pstrDocString, pbstrDocStringA);
		if (FAILED(hResult))
			goto Error1;

		SysFreeString(pstrDocString);
	}

	if (ppstrHelpFile)
	{
		hResult = ConvertDispStringToA(pstrHelpFile, pbstrHelpFileA);
		if (FAILED(hResult))
			goto Error2;

		SysFreeString(pstrHelpFile);
	}

	return NOERROR;

Error2:
	if (pbstrDocStringA)
		ConvertDispStringFreeA(*pbstrDocStringA);

Error1:
	if (pbstrNameA)
		ConvertDispStringFreeA(*pbstrNameA);

Error:
	if (ppstrName)
		SysFreeString(pstrName);
	if (ppstrDocString)
		SysFreeString(pstrDocString);
	if (ppstrHelpFile)
		SysFreeString(pstrHelpFile);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetDllEntry, public
//
//  Synopsis:   Thunks GetDllEntry to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetDllEntry(MEMBERID memid, INVOKEKIND invkind,
		BSTRA * pbstrDllNameA, BSTRA * pbstrNameA,
		unsigned short * pwOrdinal)
{
	TraceMethodEnter("CTypeInfoA::GetDllEntry", this);

	BSTR pstrDllName, *ppstrDllName;
	BSTR pstrName, *ppstrName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetDllEntry));

	if (pbstrDllNameA)
		ppstrDllName = &pstrDllName;
	else
		ppstrDllName = NULL;

	if (pbstrNameA)
		ppstrName = &pstrName;
	else
		ppstrName = NULL;

	hResult = GetWide()->GetDllEntry(memid, invkind, ppstrDllName, ppstrName,
			pwOrdinal);
	if (FAILED(hResult))
		return (hResult);

	if (pbstrDllNameA)
	{
		hResult = ConvertDispStringToA(pstrDllName, pbstrDllNameA);
		if (FAILED(hResult))
			goto Error;

		SysFreeString(pstrDllName);
	}

	if (pbstrNameA)
	{
		hResult = ConvertDispStringToA(pstrName, pbstrNameA);
		if (FAILED(hResult))
			goto Error1;

		SysFreeString(pstrName);
	}

	return NOERROR;

Error1:
	if (pbstrNameA)
		ConvertDispStringFreeA(*pbstrNameA);

Error:
	SysFreeString(pstrDllName);
	SysFreeString(pstrName);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetRefTypeInfo, public
//
//  Synopsis:   Thunks GetRefTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetRefTypeInfo(HREFTYPE hreftype,
		ITypeInfoA * * pptinfo)
{
	TraceMethodEnter("CTypeInfoA::GetRefTypeInfo", this);

	LPTYPEINFO pTypeInfo;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetRefTypeInfo));

	hResult = GetWide()->GetRefTypeInfo(hreftype, &pTypeInfo);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeInfoAFromW(pTypeInfo, pptinfo);

	pTypeInfo->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::AddressOfMember, public
//
//  Synopsis:   Thunks AddressOfMember to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::AddressOfMember(MEMBERID memid, INVOKEKIND invkind,
		void * * ppv)
{
	TraceMethodEnter("CTypeInfoA::AddressOfMember", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, AddressOfMember));

	return GetWide()->AddressOfMember(memid, invkind, ppv);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::CreateInstance, public
//
//  Synopsis:   Thunks CreateInstance to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::CreateInstance(IUnknown * punkOuter, REFIID riid,
		void * * ppvObj)
{
	HRESULT hResult;
	IUnknown * pvObjW;

	TraceMethodEnter("CTypeInfoA::CreateInstance", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, CreateInstance));

	hResult = GetWide()->CreateInstance(punkOuter, riid, (void **)&pvObjW);
	if (SUCCEEDED(hResult))
	{
		hResult = WrapInterfaceAFromW(WrapTranslateIID(riid), pvObjW, (IUnknown**)ppvObj);
		pvObjW->Release();
	}
	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetMops, public
//
//  Synopsis:   Thunks GetMops to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetMops(MEMBERID memid, BSTRA * pbstrMopsA)
{
	TraceMethodEnter("CTypeInfoA::GetMops", this);

	BSTR bstrMops;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetMops));

	hResult = GetWide()->GetMops(memid, &bstrMops);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToA(bstrMops, pbstrMopsA);

	ConvertDispStringFreeW(bstrMops);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::GetContainingTypeLib, public
//
//  Synopsis:   Thunks GetContainingTypeLib to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeInfoA::GetContainingTypeLib(ITypeLibA * * pptlib,
		unsigned int * pindex)
{
	TraceMethodEnter("CTypeInfoA::GetContainingTypeLib", this);

	LPTYPELIB pTypeLib;
	LPTYPELIB *ppTypeLib;
	HRESULT hResult;

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, GetContainingTypeLib));

	if (pptlib)
		ppTypeLib = &pTypeLib;
	else
		ppTypeLib = NULL;

	hResult = GetWide()->GetContainingTypeLib(ppTypeLib, pindex);
	if (FAILED(hResult))
		return (hResult);

	if (pptlib)
	{
		hResult = WrapITypeLibAFromW(pTypeLib, pptlib);
		pTypeLib->Release();
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::ReleaseTypeAttr, public
//
//  Synopsis:   Thunks ReleaseTypeAttr to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeInfoA::ReleaseTypeAttr(TYPEATTR * ptypeattr)
{
	TraceMethodEnter("CTypeInfoA::ReleaseTypeAttr", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, ReleaseTypeAttr));

	GetWide()->ReleaseTypeAttr(ptypeattr);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::ReleaseFuncDesc, public
//
//  Synopsis:   Thunks ReleaseFuncDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeInfoA::ReleaseFuncDesc(FUNCDESC * pfuncdesc)
{
	TraceMethodEnter("CTypeInfoA::ReleaseFuncDesc", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, ReleaseFuncDesc));

	GetWide()->ReleaseFuncDesc(pfuncdesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeInfoA::ReleaseVarDesc, public
//
//  Synopsis:   Thunks ReleaseVarDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP_(void) CTypeInfoA::ReleaseVarDesc(VARDESCA * pvardesc)
{
	TraceMethodEnter("CTypeInfoA::ReleaseVarDesc", this);

	_DebugHook(GetWide(), MEMBER_PTR(ITypeInfo, ReleaseVarDesc));

	GetWide()->ReleaseVarDesc((VARDESC *)pvardesc);
}



//***************************************************************************
//
//                   ITypeCompA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CTypeCompA::Bind, public
//
//  Synopsis:   Thunks Bind to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeCompA::Bind(CHAR * szNameA, unsigned long lHashVal,
		unsigned short wflags, ITypeInfoA * * pptinfo, DESCKIND * pdesckind,
		BINDPTRA * pbindptr)
{
	TraceMethodEnter("CTypeCompA::Bind", this);

	OLECHAR    szName[MAX_STRING];
	LPTYPEINFO pTypeInfo;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeComp, Bind));

	ConvertStringToW(szNameA, szName);

	hResult = GetWide()->Bind(szName, lHashVal, wflags, &pTypeInfo,
			pdesckind, (BINDPTR *)pbindptr);

	// NOTE: if *pdesckind == DESKIND_NONE, then *pTypeInfo and *pbindptr
	// are indeterminate.
	if (FAILED(hResult) || *pdesckind == DESCKIND_NONE)
		return hResult;

	if (pTypeInfo == NULL)
	{
		*pptinfo = NULL;
	}
	else
	{
		hResult = WrapITypeInfoAFromW(pTypeInfo, pptinfo);
		if (FAILED(hResult))
			goto Error;
	}

	// convert *pbindptr from BINDPTR to BINDPTRA
	switch (*pdesckind)
	{
	case DESCKIND_VARDESC:
	case DESCKIND_IMPLICITAPPOBJ:
		hResult = ConvertVARDESCToA(pbindptr->lpvardesc);
		break;

	case DESCKIND_TYPECOMP:
		{
		LPTYPECOMP pTypeComp;
		pTypeComp = (LPTYPECOMP)pbindptr->lptcomp;
		hResult = WrapITypeCompAFromW(pTypeComp, &pbindptr->lptcomp);
		pTypeComp->Release();
		break;
		}

	//case DESCKIND_NONE:
	//case DESCKIND_FUNCDESC:
	// nothing to do
	}

Error:
	if (pTypeInfo)
		pTypeInfo->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CTypeCompA::BindType, public
//
//  Synopsis:   Thunks BindType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CTypeCompA::BindType(CHAR * szNameA, unsigned long lHashVal,
		ITypeInfoA * * pptinfoA, ITypeCompA * * pptcompA)
{
	TraceMethodEnter("CTypeCompA::BindType", this);

	OLECHAR    szName[MAX_STRING];
	LPTYPEINFO pTypeInfo;
	LPTYPECOMP pTypeComp;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ITypeComp, BindType));

	ConvertStringToW(szNameA, szName);

	hResult = GetWide()->BindType(szName, lHashVal,  &pTypeInfo, &pTypeComp);
	if (FAILED(hResult))
		return hResult;

	if (pTypeInfo == NULL)
	{
		*pptinfoA = NULL;
	}
	else
	{
		hResult = WrapITypeInfoAFromW(pTypeInfo, pptinfoA);
		if (FAILED(hResult))
			goto Error;
	}

	if (pTypeComp == NULL)
	{
		*pptcompA = NULL;
	}
	else
	{
		hResult = WrapITypeCompAFromW(pTypeComp, pptcompA);
		pTypeComp->Release();
	}

Error:
	if (pTypeInfo)
		pTypeInfo->Release();

	return hResult;
}



//***************************************************************************
//
//                   ICreateTypeLibA Implementation
//
//***************************************************************************
//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::CreateTypeInfo, public
//
//  Synopsis:   Thunks CreateTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::CreateTypeInfo(CHAR * szNameA, TYPEKIND tkind,
		ICreateTypeInfoA * * lplpictinfo)
{
	TraceMethodEnter("CCreateTypeLibA::CreateTypeInfo", this);

	OLECHAR    szName[MAX_STRING];
	LPCREATETYPEINFO pTypeInfo;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, CreateTypeInfo));

	ConvertStringToW(szNameA, szName);

	hResult = GetWide()->CreateTypeInfo(szName, tkind,  &pTypeInfo);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapICreateTypeInfoAFromW(pTypeInfo, lplpictinfo);

	if (pTypeInfo)
		pTypeInfo->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetName, public
//
//  Synopsis:   Thunks SetName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetName(CHAR * szNameA)
{
	TraceMethodEnter("CCreateTypeLibA::SetName", this);

	OLECHAR szName[MAX_STRING];


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetName));

	ConvertStringToW(szNameA, szName);

	return GetWide()->SetName(szName);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetVersion, public
//
//  Synopsis:   Thunks SetVersion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetVersion(unsigned short wMajorVerNum,
		unsigned short wMinorVerNum)
{
	TraceMethodEnter("CCreateTypeLibA::SetVersion", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetVersion));

	return GetWide()->SetVersion(wMajorVerNum, wMinorVerNum);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetGuid, public
//
//  Synopsis:   Thunks SetGuid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetGuid(REFGUID guid)
{
	TraceMethodEnter("CCreateTypeLibA::SetGuid", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetGuid));

	return GetWide()->SetGuid(guid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetDocString, public
//
//  Synopsis:   Thunks SetDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetDocString(CHAR * szDocA)
{
	TraceMethodEnter("CCreateTypeLibA::SetDocString", this);

	LPWSTR     lpszDoc;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetDocString));

	hResult = ConvertStringToW(szDocA, &lpszDoc);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetDocString(lpszDoc);

	ConvertStringFree(lpszDoc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetHelpFileName, public
//
//  Synopsis:   Thunks SetHelpFileName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetHelpFileName(CHAR * szHelpFileNameA)
{
	TraceMethodEnter("CCreateTypeLibA::SetHelpFileName", this);

	OLECHAR szHelpFileName[MAX_STRING];


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetHelpFileName));

	ConvertStringToW(szHelpFileNameA, szHelpFileName);

	return GetWide()->SetHelpFileName(szHelpFileName);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetHelpContext, public
//
//  Synopsis:   Thunks SetHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetHelpContext(unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeLibA::SetHelpContext", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetHelpContext));

	return GetWide()->SetHelpContext(dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetLcid, public
//
//  Synopsis:   Thunks SetLcid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetLcid(LCID lcid)
{
	TraceMethodEnter("CCreateTypeLibA::SetLcid", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetLcid));

	return GetWide()->SetLcid(lcid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SetLibFlags, public
//
//  Synopsis:   Thunks SetLibFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SetLibFlags(unsigned int uLibFlags)
{
	TraceMethodEnter("CCreateTypeLibA::SetLibFlags", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SetLibFlags));

	return GetWide()->SetLibFlags(uLibFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeLibA::SaveAllChanges, public
//
//  Synopsis:   Thunks SaveAllChanges to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeLibA::SaveAllChanges(VOID)
{
	TraceMethodEnter("CCreateTypeLibA::SaveAllChanges", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeLib, SaveAllChanges));

	return GetWide()->SaveAllChanges();
}



//***************************************************************************
//
//                   ICreateTypeInfoA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetGuid, public
//
//  Synopsis:   Thunks SetGuid to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetGuid(REFGUID guid)
{
	TraceMethodEnter("CCreateTypeInfoA::SetGuid", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetGuid));

	return GetWide()->SetGuid(guid);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetTypeFlags, public
//
//  Synopsis:   Thunks SetTypeFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetTypeFlags(unsigned int uTypeFlags)
{
	TraceMethodEnter("CCreateTypeInfoA::SetTypeFlags", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetTypeFlags));

	return GetWide()->SetTypeFlags(uTypeFlags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetDocString, public
//
//  Synopsis:   Thunks SetDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetDocString(CHAR * pstrDocA)
{
	TraceMethodEnter("CCreateTypeInfoA::SetDocString", this);

	LPWSTR   lpszDoc;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetDocString));

	hResult = ConvertStringToW(pstrDocA, &lpszDoc);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetDocString(lpszDoc);

	ConvertStringFree(lpszDoc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetHelpContext, public
//
//  Synopsis:   Thunks SetHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetHelpContext(unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeInfoA::SetHelpContext", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetHelpContext));

	return GetWide()->SetHelpContext(dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetVersion, public
//
//  Synopsis:   Thunks SetVersion to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetVersion(unsigned short wMajorVerNum,
		unsigned short wMinorVerNum)
{
	TraceMethodEnter("CCreateTypeInfoA::SetVersion", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetVersion));

	return GetWide()->SetVersion(wMajorVerNum, wMinorVerNum);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::AddRefTypeInfo, public
//
//  Synopsis:   Thunks AddRefTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::AddRefTypeInfo(ITypeInfoA * ptinfo,
		HREFTYPE * phreftype)
{
	TraceMethodEnter("CCreateTypeInfoA::AddRefTypeInfo", this);

	ITypeInfo * pTypeInfo;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, AddRefTypeInfo));

	// ptinfo->QueryInterface(IID_ITypeInfo, (PVOID *)&pTypeInfo);
	if( FAILED( hResult = WrapITypeInfoWFromA( ptinfo, &pTypeInfo )))
	  return hResult;

	hResult = GetWide()->AddRefTypeInfo(pTypeInfo, phreftype);

	pTypeInfo->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::AddFuncDesc, public
//
//  Synopsis:   Thunks AddFuncDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::AddFuncDesc(unsigned int index,
		FUNCDESC * pfuncdesc)
{
	TraceMethodEnter("CCreateTypeInfoA::AddFuncDesc", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, AddFuncDesc));

	return GetWide()->AddFuncDesc(index, pfuncdesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::AddImplType, public
//
//  Synopsis:   Thunks AddImplType to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::AddImplType(unsigned int index,
		HREFTYPE hreftype)
{
	TraceMethodEnter("CCreateTypeInfoA::AddImplType", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, AddImplType));

	return GetWide()->AddImplType(index, hreftype);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetImplTypeFlags, public
//
//  Synopsis:   Thunks SetImplTypeFlags to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetImplTypeFlags(unsigned int index,
		int impltypeflags)
{
	TraceMethodEnter("CCreateTypeInfoA::SetImplTypeFlags", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetImplTypeFlags));

	return GetWide()->SetImplTypeFlags(index, impltypeflags);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetAlignment, public
//
//  Synopsis:   Thunks SetAlignment to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetAlignment(unsigned short cbAlignment)
{
	TraceMethodEnter("CCreateTypeInfoA::SetAlignment", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetAlignment));

	return GetWide()->SetAlignment(cbAlignment);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetSchema, public
//
//  Synopsis:   Thunks SetSchema to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetSchema(CHAR * lpstrSchemaA)
{
	TraceMethodEnter("CCreateTypeInfoA::SetSchema", this);

	LPWSTR   lpszSchema;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetSchema));

	hResult = ConvertStringToW(lpstrSchemaA, &lpszSchema);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetSchema(lpszSchema);

	ConvertStringFree(lpszSchema);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::AddVarDesc, public
//
//  Synopsis:   Thunks AddVarDesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::AddVarDesc(unsigned int index,
		VARDESCA * pvardescA)
{
	TraceMethodEnter("CCreateTypeInfoA::AddVarDesc", this);

	LPVARDESC pVarDesc;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, AddVarDesc));

	hResult = ConvertVARDESCToW(pvardescA, &pVarDesc);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->AddVarDesc(index, pVarDesc);

	ConvertVARDESCFree(pVarDesc);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetFuncAndParamNames, public
//
//  Synopsis:   Thunks SetFuncAndParamNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetFuncAndParamNames(unsigned int index,
		CHAR * * rgszNamesA, unsigned int cNames)
{
	TraceMethodEnter("CCreateTypeInfoA::SetFuncAndParamNames", this);

	LPWSTR * rgszNames;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetFuncAndParamNames));

	hResult = ConvertStringArrayToW(rgszNamesA, &rgszNames, cNames);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetFuncAndParamNames(index, rgszNames, cNames);

	ConvertStringArrayFree(rgszNames, cNames);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetVarName, public
//
//  Synopsis:   Thunks SetVarName to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetVarName(unsigned int index, CHAR * szNameA)
{
	TraceMethodEnter("CCreateTypeInfoA::SetVarName", this);

	OLECHAR szName[MAX_STRING];


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetVarName));

	ConvertStringToW(szNameA, szName);

	return GetWide()->SetVarName(index, szName);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetTypeDescAlias, public
//
//  Synopsis:   Thunks SetTypeDescAlias to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetTypeDescAlias(TYPEDESC * ptdescAlias)
{
	TraceMethodEnter("CCreateTypeInfoA::SetTypeDescAlias", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetTypeDescAlias));

	return GetWide()->SetTypeDescAlias(ptdescAlias);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::DefineFuncAsDllEntry, public
//
//  Synopsis:   Thunks DefineFuncAsDllEntry to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::DefineFuncAsDllEntry(unsigned int index,
		CHAR * szDllNameA, CHAR * szProcNameA)
{
	TraceMethodEnter("CCreateTypeInfoA::DefineFuncAsDllEntry", this);

	OLECHAR szDllName[MAX_STRING];
	LPWSTR  lpszProcName;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, DefineFuncAsDllEntry));

	ConvertStringToW(szDllNameA, szDllName);

	if (HIWORD(szProcNameA))
	{
		// not an ordinal
		hResult = ConvertStringToW(szProcNameA, &lpszProcName);
		if (FAILED(hResult))
			goto Error;
	}
	else
	{
		// really an ordinal
		lpszProcName = (LPWSTR)szProcNameA;
	}

	hResult = GetWide()->DefineFuncAsDllEntry(index, szDllName, lpszProcName);

	if (HIWORD(szProcNameA))
	{
		// not an ordinal
		ConvertStringFree(lpszProcName);
	}

Error:
	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetFuncDocString, public
//
//  Synopsis:   Thunks SetFuncDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetFuncDocString(unsigned int index,
		CHAR * szDocStringA)
{
	TraceMethodEnter("CCreateTypeInfoA::SetFuncDocString", this);

	LPWSTR   lpszDocString;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetFuncDocString));

	hResult = ConvertStringToW(szDocStringA, &lpszDocString);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetFuncDocString(index, lpszDocString);

	ConvertStringFree(lpszDocString);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetVarDocString, public
//
//  Synopsis:   Thunks SetVarDocString to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetVarDocString(unsigned int index,
		CHAR * szDocStringA)
{
	TraceMethodEnter("CCreateTypeInfoA::SetVarDocString", this);

	LPWSTR   lpszDocString;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetVarDocString));

	hResult = ConvertStringToW(szDocStringA, &lpszDocString);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->SetVarDocString(index, lpszDocString);

	ConvertStringFree(lpszDocString);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetFuncHelpContext, public
//
//  Synopsis:   Thunks SetFuncHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetFuncHelpContext(unsigned int index,
		unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeInfoA::SetFuncHelpContext", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetFuncHelpContext));

	return GetWide()->SetFuncHelpContext(index, dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetVarHelpContext, public
//
//  Synopsis:   Thunks SetVarHelpContext to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetVarHelpContext(unsigned int index,
		unsigned long dwHelpContext)
{
	TraceMethodEnter("CCreateTypeInfoA::SetVarHelpContext", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetVarHelpContext));

	return GetWide()->SetVarHelpContext(index, dwHelpContext);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetMops, public
//
//  Synopsis:   Thunks SetMops to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetMops(unsigned int index, BSTRA bstrMopsA)
{
	TraceMethodEnter("CCreateTypeInfoA::SetMops", this);

	BSTR bstrMops;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetMops));

	hResult = ConvertStringToW(bstrMopsA, &bstrMops);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->SetMops(index, bstrMops);

	ConvertStringFree(bstrMops);

	return hResult;

}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::SetTypeIdldesc, public
//
//  Synopsis:   Thunks SetTypeIdldesc to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::SetTypeIdldesc(IDLDESC * pidldesc)
{
	TraceMethodEnter("CCreateTypeInfoA::SetTypeIdldesc", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, SetTypeIdldesc));

	return GetWide()->SetTypeIdldesc(pidldesc);
}


//+--------------------------------------------------------------------------
//
//  Member:     CCreateTypeInfoA::LayOut, public
//
//  Synopsis:   Thunks LayOut to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CCreateTypeInfoA::LayOut(VOID)
{
	TraceMethodEnter("CCreateTypeInfoA::LayOut", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateTypeInfo, LayOut));

	return GetWide()->LayOut();
}



//***************************************************************************
//
//                   IEnumVARIANTA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTA::Next, public
//
//  Synopsis:   Thunks Next to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTA::Next(unsigned long celt, VARIANTA * rgvar,
		unsigned long * pceltFetched)
{
	TraceMethodEnter("CEnumVARIANTA::Next", this);

	ULONG   celtFetched;
	HRESULT hReturn;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumVARIANT, Next));

	if (pceltFetched == NULL)
		pceltFetched = &celtFetched;

	hReturn = GetWide()->Next(celt, (VARIANT *)rgvar, pceltFetched);
	if (FAILED(hReturn))
		return (hReturn);

	hResult = ConvertVariantArrayToA(rgvar, *pceltFetched);
	if (FAILED(hResult))
		return (hResult);

	return hReturn;
}



//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTA::Skip, public
//
//  Synopsis:   Thunks Skip to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTA::Skip(unsigned long celt)
{
	TraceMethodEnter("CEnumVARIANTA::Skip", this);

	_DebugHook(GetWide(), MEMBER_PTR(IEnumVARIANT, Skip));

	return GetWide()->Skip(celt);
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTA::Reset, public
//
//  Synopsis:   Thunks Reset to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTA::Reset(VOID)
{
	TraceMethodEnter("CEnumVARIANTA::Reset", this);

	_DebugHook(GetWide(), MEMBER_PTR(IEnumVARIANT, Reset));

	return GetWide()->Reset();
}


//+--------------------------------------------------------------------------
//
//  Member:     CEnumVARIANTA::Clone, public
//
//  Synopsis:   Thunks Clone to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CEnumVARIANTA::Clone(IEnumVARIANTA * * ppenmA)
{
	TraceMethodEnter("CEnumVARIANTA::Clone", this);

	IEnumVARIANT * penm;


	_DebugHook(GetWide(), MEMBER_PTR(IEnumVARIANT, Clone));

	HRESULT hResult = GetWide()->Clone(&penm);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapIEnumVARIANTAFromW(penm, ppenmA);

	penm->Release();

	return hResult;
}



//***************************************************************************
//
//                   IDispatchA Implementation
//
//***************************************************************************

//+--------------------------------------------------------------------------
//
//  Member:     CDispatchA::GetTypeInfoCount, public
//
//  Synopsis:   Thunks GetTypeInfoCount to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchA::GetTypeInfoCount(unsigned int * pctinfo)
{
	TraceMethodEnter("CDispatchA::GetTypeInfoCount", this);

	_DebugHook(GetWide(), MEMBER_PTR(IDispatch, GetTypeInfoCount));

	return GetWide()->GetTypeInfoCount(pctinfo);
}


//+--------------------------------------------------------------------------
//
//  Member:     CDispatchA::GetTypeInfo, public
//
//  Synopsis:   Thunks GetTypeInfo to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchA::GetTypeInfo(unsigned int itinfo, LCID lcid,
		ITypeInfoA * * pptinfo)
{
	TraceMethodEnter("CDispatchA::GetTypeInfo", this);

	LPTYPEINFO pTypeInfo;
	HRESULT    hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDispatch, GetTypeInfo));

	hResult = GetWide()->GetTypeInfo(itinfo, lcid, &pTypeInfo);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapITypeInfoAFromW(pTypeInfo, pptinfo);

	pTypeInfo->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDispatchA::GetIDsOfNames, public
//
//  Synopsis:   Thunks GetIDsOfNames to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchA::GetIDsOfNames(REFIID riid, char * * rgszNamesA,
		unsigned int cNames, LCID lcid, DISPID * rgdispid)
{
	TraceMethodEnter("CDispatchA::GetIDsOfNames", this);

	LPWSTR * rgszNames;
	HRESULT hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDispatch, GetIDsOfNames));

	hResult = ConvertStringArrayToW(rgszNamesA, lcid, &rgszNames, cNames);
	if (FAILED(hResult))
		return (hResult);

	hResult = GetWide()->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);

	ConvertStringArrayFree(rgszNames, cNames);

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Member:     CDispatchA::Invoke, public
//
//  Synopsis:   Thunks Invoke to Unicode method.
//
//  Returns:    OLE2 Result Code.
//
//  History:    24-Jun-94   patl    Fixed case where GetWide->Invoke
//                  returns an error, and an exception.
//                  Previously, the exception would be
//                  left wide, not converted to ANSI.
//
//---------------------------------------------------------------------------
STDMETHODIMP CDispatchA::Invoke(DISPID dispidMember, REFIID riid, LCID lcid,
		unsigned short wFlags, DISPPARAMSA * pdispparamsA,
		VARIANTA * pvarResultA, EXCEPINFOA * pexcepinfoA,
		unsigned int * puArgErr)
{
	TraceMethodEnter("CDispatchA::Invoke", this);

	LPDISPPARAMS pDispParams;
	LPVARIANT    pByrefTemp;
	HRESULT      hResult;


	_DebugHook(GetWide(), MEMBER_PTR(IDispatch, Invoke));

	hResult = ConvertDispParamsToW(pdispparamsA, &pDispParams, &pByrefTemp);
	if (FAILED(hResult))
		return hResult;

	hResult = GetWide()->Invoke(dispidMember, riid, lcid, wFlags, pDispParams,
			(LPVARIANT)pvarResultA, (EXCEPINFO *)pexcepinfoA, puArgErr);

	if (hResult == DISP_E_EXCEPTION)
		ConvertExcepInfoToA(pexcepinfoA);

	if (FAILED(hResult))
		goto Error;

	if (pByrefTemp)
	{
		hResult = ConvertDispParamsCopyBack(pdispparamsA, pByrefTemp);
		if (FAILED(hResult))
			goto Error;
	}

	hResult = ConvertVariantToA(pvarResultA);
	if (FAILED(hResult))
		goto Error;

Error:

	ConvertDispParamsFree(pDispParams, pByrefTemp);

	return hResult;
}


#ifndef NOERRORINFO
//***************************************************************************
//
//                       IErrorInfoA Implementation
//
//***************************************************************************

STDMETHODIMP
CErrorInfoA::GetGUID(GUID *pguid)
{
	TraceMethodEnter("CErrorInfoA::GetGUID", this);

	_DebugHook(GetWide(), MEMBER_PTR(IErrorInfo, GetGUID));

	return GetWide()->GetGUID(pguid);
}

STDMETHODIMP
CErrorInfoA::GetSource(BSTRA *pbstraSource)
{
	TraceMethodEnter("CErrorInfoA::GetSource", this);

	HRESULT hresult;
	BSTR bstrSource;

	bstrSource = NULL;

	_DebugHook(GetWide(), MEMBER_PTR(IErrorInfo, GetSource));

	hresult = GetWide()->GetSource(&bstrSource);
	if(FAILED(hresult))
	return hresult;

	hresult = ConvertDispStringToA(bstrSource, pbstraSource);

	SysFreeString(bstrSource);

	return hresult;
}

STDMETHODIMP
CErrorInfoA::GetDescription(BSTRA *pbstraDescription)
{
	TraceMethodEnter("CErrorInfoA::GetDescription", this);

	HRESULT hresult;
	BSTR bstrDescription;

	bstrDescription = NULL;

	_DebugHook(GetWide(), MEMBER_PTR(IErrorInfo, GetDescription));

	hresult = GetWide()->GetDescription(&bstrDescription);
	if(FAILED(hresult))
	return hresult;

	hresult = ConvertDispStringToA(bstrDescription, pbstraDescription);

	SysFreeString(bstrDescription);

	return hresult;
}

STDMETHODIMP
CErrorInfoA::GetHelpFile(BSTRA *pbstraHelpFile)
{
	TraceMethodEnter("CErrorInfoA::GetHelpFile", this);

	HRESULT hresult;
	BSTR bstrHelpFile;

	bstrHelpFile = NULL;

	_DebugHook(GetWide(), MEMBER_PTR(IErrorInfo, GetHelpFile));

	hresult = GetWide()->GetHelpFile(&bstrHelpFile);
	if(FAILED(hresult))
	return hresult;

	hresult = ConvertDispStringToA(bstrHelpFile, pbstraHelpFile);

	SysFreeString(bstrHelpFile);

	return hresult;
}

STDMETHODIMP
CErrorInfoA::GetHelpContext(DWORD *pdwHelpContext)
{
	TraceMethodEnter("CErrorInfoA::GetHelpContext", this);

	_DebugHook(GetWide(), MEMBER_PTR(IErrorInfo, GetHelpContext));

	return GetWide()->GetHelpContext(pdwHelpContext);
}


//***************************************************************************
//
//                      ICreateErrorInfo Implementation
//
//***************************************************************************

STDMETHODIMP
CCreateErrorInfoA::SetGUID(REFGUID rguid)
{
	TraceMethodEnter("CCreateErrorInfoA::SetGUID", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateErrorInfo, SetGUID));

	return GetWide()->SetGUID(rguid);
}

STDMETHODIMP
CCreateErrorInfoA::SetSource(LPSTR szSource)
{
	TraceMethodEnter("CCreateErrorInfoA::SetSource", this);

	HRESULT hresult;
	LPWSTR szwSource;

	szwSource = NULL;

	_DebugHook(GetWide(), MEMBER_PTR(ICreateErrorInfo, SetSource));

	if(szSource != NULL){
	hresult = ConvertStringToW(szSource, &szwSource);
	if(FAILED(hresult))
		return hresult;
	}

	hresult = GetWide()->SetSource(szwSource);

	if(szwSource != NULL)
	  ConvertStringFree(szwSource);

	return hresult;
}

STDMETHODIMP
CCreateErrorInfoA::SetDescription(LPSTR szDescription)
{
	TraceMethodEnter("CCreateErrorInfoA::SetDescription", this);

	HRESULT hresult;
	LPWSTR szwDescription;

	szwDescription = NULL;

	_DebugHook(GetWide(), MEMBER_PTR(ICreateErrorInfo, SetDescription));

	if(szDescription != NULL){
	hresult = ConvertStringToW(szDescription, &szwDescription);
	if(FAILED(hresult))
		return hresult;
	}

	hresult = GetWide()->SetDescription(szwDescription);

	if(szwDescription != NULL)
	ConvertStringFree(szwDescription);

	return hresult;
}

STDMETHODIMP
CCreateErrorInfoA::SetHelpFile(LPSTR szHelpFile)
{
	TraceMethodEnter("CCreateErrorInfoA::SetHelpFile", this);

	HRESULT hresult;
	LPWSTR szwHelpFile;

	szwHelpFile = NULL;

	_DebugHook(GetWide(), MEMBER_PTR(ICreateErrorInfo, SetHelpFile));

	if(szHelpFile != NULL){
	hresult = ConvertStringToW(szHelpFile, &szwHelpFile);
	if(FAILED(hresult))
		return hresult;
	}

	hresult = GetWide()->SetHelpFile(szwHelpFile);

	if(szwHelpFile != NULL)
	ConvertStringFree(szwHelpFile);

	return hresult;
}

STDMETHODIMP
CCreateErrorInfoA::SetHelpContext(DWORD dwHelpContext)
{
	TraceMethodEnter("CCreateErrorInfoA::SetHelpContext", this);

	_DebugHook(GetWide(), MEMBER_PTR(ICreateErrorInfo, SetHelpContext));

	return GetWide()->SetHelpContext(dwHelpContext);
}



STDAPI
SetErrorInfoA(DWORD dwReserved, IErrorInfoA *perrinfoA)
{
	TraceSTDAPIEnter("SetErrorInfo");

	HRESULT hresult;
	IErrorInfo *perrinfoW;

	hresult = WrapIErrorInfoWFromA(perrinfoA, &perrinfoW);
	if(FAILED(hresult))
	return hresult;
	hresult = SetErrorInfo(dwReserved, perrinfoW);
	perrinfoW->Release(); // ownership has transferred to the system.
	return hresult;
}

STDAPI
GetErrorInfoA(DWORD dwReserved, IErrorInfoA **pperrinfoA)
{
	TraceSTDAPIEnter("GetErrorInfo");

	HRESULT hresult;
	IErrorInfo *perrinfoW;

	hresult = GetErrorInfo(dwReserved, &perrinfoW);
	if(hresult != NOERROR) // S_FALSE means no Error Info obj available
	return hresult;

	hresult = WrapIErrorInfoAFromW(perrinfoW, pperrinfoA);
	if(FAILED(hresult))
	return hresult;

	perrinfoW->Release(); // now held by perrinfoA
	return NOERROR;
}

STDAPI
CreateErrorInfoA(ICreateErrorInfoA **pperrinfoA)
{
	TraceSTDAPIEnter("CreateErrorInfo");

	HRESULT hresult;
	ICreateErrorInfo *perrinfoW;

	hresult = CreateErrorInfo(&perrinfoW);
	if(FAILED(hresult))
	return hresult;

	hresult = WrapICreateErrorInfoAFromW(perrinfoW, pperrinfoA);
	if(FAILED(hresult))
	return hresult;

	perrinfoW->Release(); // now held by pperrinfoA
	return NOERROR;
}
#endif  //!NOERRORINFO
