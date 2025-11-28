//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//  Information Contained Herein Is Proprietary and Confidential.
//
//  File:       ansioa.cpp
//
//  Contents:   ANSI version of OLE Automation APIs.
//              See OLE2 docs for details of Unicode equalivent APIs.
//
//  Functions:  SysAllocStringA
//              SysAllocStringLenA
//              SysStringLenA
//              SysReAllocStringA
//              SysReAllocStringLenA
//              SysFreeStringA
//
//              SafeArrayAllocDescriptorA
//              SafeArrayAllocDataA
//              SafeArrayCreateA
//              SafeArrayDestroyDescriptorA
//              SafeArrayDestroyDataA
//              SafeArrayDestroyA
//              SafeArrayRedimA
//              SafeArrayGetDimA
//              SafeArrayGetElemsizeA
//              SafeArrayGetUBoundA
//              SafeArrayGetLBoundA
//              SafeArrayLockA
//              SafeArrayUnlockA
//              SafeArrayAccessDataA
//              SafeArrayUnaccessDataA
//              SafeArrayGetElement
//              SafeArrayPutElementA
//              SafeArrayCopyA
//
//              VariantInitA
//              VariantClearA
//              VariantCopyA
//              VariantCopyIndA
//              VariantChangeTypeA
//              VariantChangeTypeExA
//
//              VarUI1FromStrA
//              VarUI1FromDispA
//              VarI2FromStrA
//              VarI2FromDispA
//              VarI4FromStrA
//              VarI4FromDispA
//              VarR4FromStrA
//              VarR4FromDispA
//              VarR8FromStrA
//              VarR8FromDispA
//              VarDateFromStrA
//              VarDateFromDispA
//              VarCyFromStrA
//              VarCyFromDispA
//              VarBstrFromUI1A
//              VarBstrFromI2A
//              VarBstrFromI4A
//              VarBstrFromR4A
//              VarBstrFromR8A
//              VarBstrFromCyA
//              VarBstrFromDateA
//              VarBstrFromDispA
//              VarBstrFromBoolA
//              VarBoolFromStrA
//              VarBoolFromDispA
//
//              LHashValOfNameSysA
//              LoadTypeLibA
//              LoadRegTypeLibA
//              QueryPathOfRegTypeLibA
//              RegisterTypeLibA
//              CreateTypeLibA
//
//              DispGetParamA
//              DispGetIDsOfNamesA
//              DispInvokeA
//              CreateDispTypeInfoA
//              CreateStdDispatchA
//
//  History:    12-Jan-94   tomteng     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"

extern "C" int __cdecl _fltused = 1; // avoid dragging in C-runtime

/*---------------------------------------------------------------------*/
/*                         ANSI BSTR API                               */
/*---------------------------------------------------------------------*/


STDAPI_(BSTRA) SysAllocStringA(const char * psz)
{
	TraceSTDAPIEnter("SysAllocStringA");

	if (psz == NULL)
		return NULL;

	return (BSTRA)SysAllocStringByteLen(psz, lstrlen(psz));
}


STDAPI_(BSTRA) SysAllocStringLenA(const char * psz, unsigned int len)
{
	TraceSTDAPIEnter("SysAllocStringLenA");

	BSTRA bstr = (BSTRA)SysAllocStringByteLen(psz, len);
	return bstr;
}


STDAPI_(int) SysReAllocStringA(BSTRA * pbstr, const char * psz)
{
	TraceSTDAPIEnter("SysReAllocStringA");

	return SysReAllocStringLenA(pbstr, psz, (psz ? lstrlen(psz) : 0));
}


STDAPI_(int) SysReAllocStringLenA(BSTRA * pbstr, const char * psz, unsigned int len)
{
	TraceSTDAPIEnter("SysReAllocStringLenA");

	BSTRA bstrNew = (BSTRA) SysAllocStringLenA(psz, len);
	if (bstrNew == NULL)
	return FALSE;

	SysFreeString((BSTR) *pbstr);
	*pbstr = bstrNew;

	return TRUE;
}


STDAPI_(unsigned int) SysStringLenA(BSTRA bstr)
{
	TraceSTDAPIEnter("int");

	return SysStringByteLen((BSTR)bstr);
}


STDAPI_(void) SysFreeStringA(BSTRA bstr)
{
	TraceSTDAPIEnter("SysFreeStringA");

	SysFreeString((BSTR)bstr);
}


/*---------------------------------------------------------------------*/
/*                         ANSI SafeArray API                          */
/*---------------------------------------------------------------------*/


STDAPI SafeArrayAllocDescriptorA(unsigned int cDims, SAFEARRAYA * * ppsaOut)
{
	TraceSTDAPIEnter("SafeArrayAllocDescriptorA");

	return (SafeArrayAllocDescriptor(cDims, (SAFEARRAY * *) ppsaOut));
}


STDAPI SafeArrayAllocDataA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayAllocDataA");

	return (SafeArrayAllocData((SAFEARRAY *) psa));
}


STDAPI_(SAFEARRAYA *) SafeArrayCreateA(
	VARTYPE vt,
	unsigned int cDims,
	SAFEARRAYBOUND * rgsabound)
{
	TraceSTDAPIEnter("SafeArrayCreateA");

	return ((SAFEARRAYA *) SafeArrayCreate(vt, cDims, rgsabound));
}


STDAPI SafeArrayDestroyDescriptorA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayDestroyDescriptorA");

	return (SafeArrayDestroyDescriptor((SAFEARRAY *) psa));
}


STDAPI SafeArrayDestroyDataA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayDestroyDataA");

	return (SafeArrayDestroyData((SAFEARRAY *) psa));
}


STDAPI SafeArrayDestroyA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayDestroyA");

  return (SafeArrayDestroy((SAFEARRAY *) psa));
}


STDAPI SafeArrayRedimA(SAFEARRAYA * psa, SAFEARRAYBOUND * psaboundNew)
{
	TraceSTDAPIEnter("SafeArrayRedimA");

	return (SafeArrayRedim((SAFEARRAY *) psa, psaboundNew));
}


STDAPI_(UINT) SafeArrayGetDimA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayGetDimA");

	return (SafeArrayGetDim((SAFEARRAY *) psa));
}


STDAPI_(UINT) SafeArrayGetElemsizeA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayGetElemsizeA");

	return (SafeArrayGetElemsize((SAFEARRAY *) psa));
}


STDAPI SafeArrayGetUBoundA(
	SAFEARRAYA * psa,
	UINT nDim,
	long * plUbound)
{
	TraceSTDAPIEnter("SafeArrayGetUBoundA");

	return (SafeArrayGetUBound((SAFEARRAY *) psa, nDim, plUbound));
}


STDAPI SafeArrayGetLBoundA(
	SAFEARRAYA * psa,
	UINT nDim,
	long * plLbound)
{
	TraceSTDAPIEnter("SafeArrayGetLBoundA");

	return (SafeArrayGetLBound((SAFEARRAY *) psa, nDim, plLbound));
}


STDAPI SafeArrayLockA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayLockA");

	return (SafeArrayLock((SAFEARRAY *) psa));
}


STDAPI SafeArrayUnlockA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayUnlockA");

	return (SafeArrayUnlock((SAFEARRAY *) psa));
}


STDAPI SafeArrayAccessDataA(SAFEARRAYA * psa, void HUGEP* * ppvData)
{
	TraceSTDAPIEnter("SafeArrayAccessDataA");

	return (SafeArrayAccessData((SAFEARRAY *) psa, ppvData));
}


STDAPI SafeArrayUnaccessDataA(SAFEARRAYA * psa)
{
	TraceSTDAPIEnter("SafeArrayUnaccessDataA");

	return (SafeArrayUnaccessData((SAFEARRAY *) psa));
}


STDAPI SafeArrayGetElementA(
	SAFEARRAYA * psa,
	long * rgIndices,
	void * pv)
{
	TraceSTDAPIEnter("SafeArrayGetElementA");

	return (SafeArrayGetElement((SAFEARRAY *) psa, rgIndices, pv));
}


STDAPI SafeArrayPutElementA(
	SAFEARRAYA * psa,
	long * rgIndices,
	void * pv)
{
	TraceSTDAPIEnter("SafeArrayPutElementA");

	return (SafeArrayPutElement((SAFEARRAY *) psa, rgIndices, pv));
}


STDAPI SafeArrayCopyA(
	SAFEARRAYA * psa,
	SAFEARRAYA * * ppsaOut)
{
	TraceSTDAPIEnter("SafeArrayCopyA");

	return (SafeArrayCopy((SAFEARRAY *) psa, (SAFEARRAY * *) ppsaOut));
}



/*---------------------------------------------------------------------*/
/*                         ANSI VARIANT API                            */
/*---------------------------------------------------------------------*/


STDAPI_(void) VariantInitA(VARIANTARGA * pvarg)
{
	TraceSTDAPIEnter("VariantInitA");

	VariantInit((VARIANTARG *)  pvarg);
}


STDAPI VariantClearA(VARIANTARGA * pvarg)
{
	TraceSTDAPIEnter("VariantClearA");

	return VariantClear((VARIANTARG *) pvarg);
}


STDAPI VariantCopyA(
	VARIANTARGA * pvargDest,
	VARIANTARGA * pvargSrc)
{
	TraceSTDAPIEnter("VariantCopyA");

	return VariantCopy((VARIANTARG *) pvargDest, (VARIANTARG *) pvargSrc);
}


STDAPI VariantCopyIndA(
	VARIANTA * pvargDest,
	VARIANTARGA * pvargSrc)

{
	TraceSTDAPIEnter("VariantCopyIndA");

	return VariantCopyInd((VARIANT *) pvargDest, (VARIANTARG *) pvargSrc);
}

STDAPI VariantChangeTypeA(
	VARIANTARGA * pvargDestA,
	VARIANTARGA * pvargSrcA,
	unsigned short wFlags,
	VARTYPE vt)
{
	TraceSTDAPIEnter("VariantChangeTypeA");

	HRESULT hResult;
	VARIANT varTempW;

	// CONSIDER: optimize this for the case of simple Variants (src, dest,
	// CONSIDER: and vt all non-string, non-array, non-object variants)
	VariantInit(&varTempW);
	hResult = VariantCopy((VARIANT *) &varTempW, (VARIANTARG *) pvargSrcA);
	if (hResult == NOERROR)
	{
		hResult = ConvertVariantToW(&varTempW);
		if (hResult == NOERROR)
		{
		  hResult = VariantChangeType(&varTempW, &varTempW, wFlags, vt);
			if (hResult == NOERROR)
			{
				hResult = ConvertVariantToA((VARIANTA *)&varTempW);
				if (hResult == NOERROR)
					hResult = VariantCopy((VARIANT *)pvargDestA, &varTempW);
			}
		}
	}

	VariantClear(&varTempW);
	return hResult;
}


STDAPI VariantChangeTypeExA(
	VARIANTARGA * pvargDestA,
	VARIANTARGA * pvargSrcA,
	LCID lcid,
	unsigned short wFlags,
	VARTYPE vt)
{
	TraceSTDAPIEnter("VariantChangeTypeExA");

	HRESULT hResult;
	VARIANT varTempW;

	// CONSIDER: optimize this for the case of simple Variants (src, dest,
	// CONSIDER: and vt all non-string, non-array, non-object variants)
	VariantInit(&varTempW);
	hResult = VariantCopy((VARIANT *) &varTempW, (VARIANTARG *) pvargSrcA);
	if (hResult == NOERROR)
	{
		hResult = ConvertVariantToW(&varTempW);
		if (hResult == NOERROR)
		{
			hResult = VariantChangeTypeEx(&varTempW, &varTempW, lcid, wFlags, vt);
			if (hResult == NOERROR)
			{
				hResult = ConvertVariantToA((VARIANTA *)&varTempW);
				if (hResult == NOERROR)
					hResult = VariantCopy((VARIANT *)pvargDestA, &varTempW);
			}
		}
	}

	VariantClear(&varTempW);
	return hResult;
}



/*---------------------------------------------------------------------*/
/*                   ANSI VARIANT Coercion API                         */
/*---------------------------------------------------------------------*/

#ifndef NOI1
STDAPI VarUI1FromStrA(char * strIn, LCID lcid,
			  unsigned long dwFlags, unsigned char * pbOut)
{
	TraceSTDAPIEnter("VarUI1FromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarUI1FromStr(lpwStrIn, lcid, dwFlags, pbOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}

STDAPI VarUI1FromDispA(IDispatchA * pdispIn, LCID lcid, unsigned char * pbOut)
{
	TraceSTDAPIEnter("VarUI1FromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;

	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarUI1FromDisp(pDisp, lcid, pbOut);
	pDisp->Release();

	return hResult;
}
#endif //!NOI1

STDAPI VarI2FromStrA(char * strIn, LCID lcid,
			  unsigned long dwFlags, short * psOut)
{
	TraceSTDAPIEnter("VarI2FromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarI2FromStr(lpwStrIn, lcid, dwFlags, psOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}


STDAPI VarI2FromDispA(IDispatchA * pdispIn, LCID lcid, short * psOut)
{
	TraceSTDAPIEnter("VarI2FromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;

	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarI2FromDisp(pDisp, lcid, psOut);
	pDisp->Release();

	return hResult;
}


STDAPI VarI4FromStrA(char * strIn, LCID lcid,
			  unsigned long dwFlags, long * plOut)
{
	TraceSTDAPIEnter("VarI4FromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarI4FromStr(lpwStrIn, lcid, dwFlags, plOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}


STDAPI VarI4FromDispA(IDispatchA * pdispIn, LCID lcid, long * plOut)
{
	TraceSTDAPIEnter("VarI4FromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;

	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarI4FromDisp(pDisp, lcid, plOut);
	pDisp->Release();

	return hResult;
}


STDAPI VarR4FromStrA(char * strIn, LCID lcid,
					 unsigned long dwFlags, float * pfltOut)
{
	TraceSTDAPIEnter("VarR4FromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarR4FromStr(lpwStrIn, lcid, dwFlags, pfltOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}


STDAPI VarR4FromDispA(IDispatchA * pdispIn, LCID lcid, float * pfltOut)
{
	TraceSTDAPIEnter("VarR4FromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;

	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarR4FromDisp(pDisp, lcid, pfltOut);
	pDisp->Release();

	return hResult;
}


STDAPI VarR8FromStrA(char * strIn, LCID lcid,
			  unsigned long dwFlags, double * pdblOut)
{
	TraceSTDAPIEnter("VarR8FromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarR8FromStr(lpwStrIn, lcid, dwFlags, pdblOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}


STDAPI VarR8FromDispA(IDispatchA * pdispIn, LCID lcid, double * pdblOut)
{
	TraceSTDAPIEnter("VarR8FromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;

	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarR8FromDisp(pDisp, lcid, pdblOut);
	pDisp->Release();

	return hResult;
}


STDAPI VarDateFromStrA(char * strIn, LCID lcid,
				unsigned long dwFlags, DATE * pdateOut)
{
	TraceSTDAPIEnter("VarDateFromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarDateFromStr(lpwStrIn, lcid, dwFlags, pdateOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}


STDAPI VarDateFromDispA(IDispatchA * pdispIn, LCID lcid, DATE * pdateOut)
{
	TraceSTDAPIEnter("VarDateFromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;


	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarDateFromDisp(pDisp, lcid, pdateOut);
	pDisp->Release();

	return hResult;
}


STDAPI VarCyFromStrA(char * strIn, LCID lcid,
			  unsigned long dwFlags, CY * pcyOut)
{
	TraceSTDAPIEnter("VarCyFromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarCyFromStr(lpwStrIn, lcid, dwFlags, pcyOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}


STDAPI VarCyFromDispA(IDispatchA * pdispIn, LCID lcid, CY * pcyOut)
{
	TraceSTDAPIEnter("VarCyFromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;


	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarCyFromDisp(pDisp, lcid, pcyOut);
	pDisp->Release();

	return hResult;

}


#ifndef NOI1
STDAPI VarBstrFromUI1A(unsigned char bVal, LCID lcid,
			   unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromUI1A");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromUI1(bVal, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}
#endif //!NOI1


STDAPI VarBstrFromI2A(short iVal, LCID lcid,
			   unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromI2A");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromI2(iVal, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBstrFromI4A(long lIn, LCID lcid,
			   unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromI4A");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromI4(lIn, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBstrFromR4A(float fltIn, LCID lcid,
			   unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromR4A");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromR4(fltIn, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBstrFromR8A(double dblIn, LCID lcid,
			   unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromR8A");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromR8(dblIn, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBstrFromCyA(CY cyIn, LCID lcid,
					  unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromCyA");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromCy(cyIn, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBstrFromDateA(DATE dateIn, LCID lcid,
				 unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromDateA");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromDate(dateIn, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBstrFromDispA(IDispatchA * pdispIn, LCID lcid,
				 unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromDispA");

	HRESULT    hResult;
	BSTR       bstr;
	IDispatch* pDisp;


	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarBstrFromDisp(pDisp, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	pDisp->Release();

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBstrFromBoolA(VARIANT_BOOL boolIn, LCID lcid,
				 unsigned long dwFlags, BSTRA * pbstrOut)
{
	TraceSTDAPIEnter("VarBstrFromBoolA");

	HRESULT    hResult;
	BSTR       bstr;

	hResult = VarBstrFromBool(boolIn, lcid, dwFlags, &bstr);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstr, pbstrOut);
	SysFreeString(bstr);

	return hResult;
}


STDAPI VarBoolFromStrA(char * strIn, LCID lcid,
				unsigned long dwFlags, VARIANT_BOOL * pboolOut)
{
	TraceSTDAPIEnter("VarBoolFromStrA");

	LPOLESTR   lpwStrIn;
	HRESULT    hResult;


	hResult = ConvertStringToW(strIn, &lpwStrIn);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarBoolFromStr(lpwStrIn, lcid, dwFlags, pboolOut);

	ConvertStringFree(lpwStrIn);

	return hResult;

}


STDAPI VarBoolFromDispA(IDispatchA * pdispIn, LCID lcid,
				 VARIANT_BOOL * pboolOut)
{
	TraceSTDAPIEnter("VarBoolFromDispA");

	HRESULT    hResult;
	IDispatch* pDisp;

	hResult = WrapIDispatchWFromA(pdispIn, &pDisp);
	if (FAILED(hResult))
		return (hResult);

	hResult = VarBoolFromDisp(pDisp, lcid, pboolOut);
	pDisp->Release();

	return hResult;
}


/*---------------------------------------------------------------------*/
/*                         ANSI Typelib API                            */
/*---------------------------------------------------------------------*/



STDAPI LoadTypeLibA(const char * szFileA, ITypeLibA * * pptlib)
{
	TraceSTDAPIEnter("LoadTypeLibA");

	LPOLESTR   lpszFile;
	LPTYPELIB  pTypeLib;
	HRESULT    hResult;


	hResult = ConvertStringToW(szFileA, &lpszFile);
	if (FAILED(hResult))
		return (hResult);

	hResult = LoadTypeLib(lpszFile, &pTypeLib);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapITypeLibAFromW(pTypeLib, pptlib);

	pTypeLib->Release();

Error:
	ConvertStringFree(lpszFile);

	return hResult;
}


STDAPI LoadRegTypeLibA(REFGUID rguid, unsigned short wVerMajor,
		unsigned short wVerMinor, LCID lcid, ITypeLibA * * pptlibA)
{
	TraceSTDAPIEnter("LoadRegTypeLibA");

	LPTYPELIB  pTypeLib;
	HRESULT    hResult;


	hResult = LoadRegTypeLib(rguid, wVerMajor, wVerMinor, lcid, &pTypeLib);
	if (FAILED(hResult))
		return (hResult);

	hResult = WrapITypeLibAFromW(pTypeLib, pptlibA);

	pTypeLib->Release();

	return hResult;
}


STDAPI QueryPathOfRegTypeLibA(REFGUID guid, unsigned short wMaj,
		unsigned short wMin, LCID lcid, LPBSTRA lpbstrPathNameA)
{
	TraceSTDAPIEnter("QueryPathOfRegTypeLibA");

	BSTR       bstrPathName;
	HRESULT    hResult;


	hResult = QueryPathOfRegTypeLib(guid, wMaj, wMin, lcid,
			&bstrPathName);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispStringToA(bstrPathName, lpbstrPathNameA);

	SysFreeString(bstrPathName);

	return hResult;
}


STDAPI RegisterTypeLibA(ITypeLibA * ptlibA, char * szFullPathA,
		char * szHelpDirA)
{
	TraceSTDAPIEnter("RegisterTypeLibA");

	LPTYPELIB pTypeLib;
	LPOLESTR  lpszFullPath;
	LPOLESTR  lpszHelpDir;
	HRESULT   hResult;


	hResult = WrapITypeLibWFromA(ptlibA, &pTypeLib);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringToW(szFullPathA, &lpszFullPath);
	if (FAILED(hResult))
		goto Error;

	hResult = ConvertStringToW(szHelpDirA, &lpszHelpDir);
	if (FAILED(hResult))
		goto Error1;

	hResult = RegisterTypeLib(pTypeLib, lpszFullPath, lpszHelpDir);

	ConvertStringFree(lpszHelpDir);

Error1:
	ConvertStringFree(lpszFullPath);

Error:
	pTypeLib->Release();

	return hResult;
}


STDAPI CreateTypeLibA(SYSKIND syskind, const char * szFileA,
		ICreateTypeLibA * * ppctlibA)
{
	TraceSTDAPIEnter("CreateTypeLibA");

	LPOLESTR         lpszFile;
	LPCREATETYPELIB  pCreateTypeLib;
	HRESULT          hResult;


	hResult = ConvertStringToW(szFileA, &lpszFile);
	if (FAILED(hResult))
		return (hResult);

	hResult = CreateTypeLib(syskind, lpszFile, &pCreateTypeLib);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapICreateTypeLibAFromW(pCreateTypeLib, ppctlibA);

	pCreateTypeLib->Release();

Error:
	ConvertStringFree(lpszFile);

	return hResult;
}


STDAPI_(unsigned long) LHashValOfNameSysHACK(SYSKIND syskind, LCID lcid, const char FAR* szName)
{
	TraceSTDAPIEnter("LHashValOfNameSysHACK");

	return LHashValOfNameSysA(syskind, lcid, szName);
}


/*---------------------------------------------------------------------*/
/*                         ANSI Dispatch API                           */
/*---------------------------------------------------------------------*/

STDAPI DispGetParamA(DISPPARAMSA * pdispparamsA, UINT position,
		VARTYPE vtTarg, VARIANTA * pvarResult, UINT * puArgErr)
{
	TraceSTDAPIEnter("DispGetParamA");

	LPDISPPARAMS pDispParams;
	LPVARIANT    pByrefTemp;
	HRESULT hResult;


	hResult = ConvertDispParamsToW(pdispparamsA, &pDispParams, &pByrefTemp);
	if (FAILED(hResult))
		return hResult;

	hResult = DispGetParam(pDispParams, position, vtTarg,
						  (LPVARIANT)pvarResult, puArgErr);
	if (FAILED(hResult))
		goto Error;

	hResult = ConvertVariantToA(pvarResult);

Error:
	ConvertDispParamsFree(pDispParams, pByrefTemp);

	return hResult;
}


STDAPI DispGetIDsOfNamesA(ITypeInfoA * ptinfoA, char * * rgszNamesA,
		UINT cNames, DISPID * rgdispid)
{
	TraceSTDAPIEnter("DispGetIDsOfNamesA");

	LPTYPEINFO pTypeInfo;
	LPOLESTR * rgszNames;
	HRESULT    hResult;


	hResult = WrapITypeInfoWFromA(ptinfoA, &pTypeInfo);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertStringArrayToW(rgszNamesA, &rgszNames, cNames);
	if (FAILED(hResult))
		goto Error;

	hResult = DispGetIDsOfNames(pTypeInfo, rgszNames, cNames, rgdispid);

	ConvertStringArrayFree(rgszNames, cNames);

Error:
	pTypeInfo->Release();

	return hResult;
}


STDAPI DispInvokeA(void * _this, ITypeInfoA * ptinfoA, DISPID dispidMember,
		unsigned short wFlags, DISPPARAMSA * pparamsA, VARIANTA * pvarResultA,
		EXCEPINFOA * pexcepinfoA, UINT * puArgErr)
{
	TraceSTDAPIEnter("DispInvokeA");

	LPTYPEINFO   pTypeInfo;
	LPDISPPARAMS pDispParams;
	LPVARIANT    pByrefTemp;
	HRESULT      hResult;


	hResult = WrapITypeInfoWFromA(ptinfoA, &pTypeInfo);
	if (FAILED(hResult))
		return (hResult);

	hResult = ConvertDispParamsToW(pparamsA, &pDispParams, &pByrefTemp);
	if (FAILED(hResult))
		goto Error;

	hResult = DispInvoke(_this, pTypeInfo, dispidMember, wFlags, pDispParams,
				 (LPVARIANT)pvarResultA, (EXCEPINFO *)pexcepinfoA, puArgErr);

		if (hResult == DISP_E_EXCEPTION)
			ConvertExcepInfoToA(pexcepinfoA);

	if (FAILED(hResult))
		goto Error1;

	if (pByrefTemp)
	{
		hResult = ConvertDispParamsCopyBack(pparamsA, pByrefTemp);
		if (FAILED(hResult))
			goto Error1;
	}

	hResult = ConvertVariantToA(pvarResultA);

Error1:
	ConvertDispParamsFree(pDispParams, pByrefTemp);

Error:
	pTypeInfo->Release();

	return hResult;
}


STDAPI CreateDispTypeInfoA(INTERFACEDATAA * pidataA, LCID lcid,
		ITypeInfoA * * pptinfoA)
{
	TraceSTDAPIEnter("CreateDispTypeInfoA");

	INTERFACEDATA * pData;
	LPTYPEINFO  pTypeInfo;
	HRESULT    hResult;


	hResult = ConvertInterfaceDataToW(pidataA, lcid, &pData);
	if (FAILED(hResult))
		return (hResult);

	hResult = CreateDispTypeInfo(pData, lcid, &pTypeInfo);
	if (FAILED(hResult))
		goto Error;

	hResult = WrapITypeInfoAFromW(pTypeInfo, pptinfoA);

	pTypeInfo->Release();

	if (FAILED(hResult))
		goto Error;

	//
	//  Save the Unicode InterfaceData structure in the TypeInfo wrapper,
	//  to be delete on pTypeInfo is released.
	//
	((CInterface *)*pptinfoA)->SetInterfaceData(pData);

	return hResult;

Error:
	ConvertInterfaceDataFree(pData);

	return hResult;
}


STDAPI CreateStdDispatchA(IUnknown * punkOuterA, void * pvThis,
		ITypeInfoA * ptinfoA, IUnknown * * ppunkStdDisp)
{
	TraceSTDAPIEnter("CreateStdDispatchA");

	LPTYPEINFO pTypeInfo;
	LPUNKNOWN  punkDisp;
	IDispatch* pDispatch;
	LPINTERFACEDATA pidata;
	HRESULT    hResult;


	hResult = WrapITypeInfoWFromA(ptinfoA, &pTypeInfo);
	if (FAILED(hResult))
		return hResult;

	hResult = CreateStdDispatch(punkOuterA, pvThis, pTypeInfo, &punkDisp);
	if (FAILED(hResult))
		goto Error;

	if (punkOuterA)
	{
		hResult = WrapIUnknownAFromW(punkDisp, ppunkStdDisp);
		punkDisp->Release();
	}
	else
	{
		hResult = punkDisp->QueryInterface( IID_IDispatch, (LPVOID *)&pDispatch );
		punkDisp->Release();
		if (FAILED(hResult))
			goto Error;

		hResult = WrapIDispatchAFromW(pDispatch, (IDispatchA **)ppunkStdDisp);
		pDispatch->Release();

		if (FAILED(hResult))
			goto Error;

	}

	//
	//  Move any interface data from the TypeInto to the Dispatch wrapper.
	//  This solves the problem of the TypeInfo wrapper being deleted
	//  before the Dispatch wrapper.
	//
	pidata = ((CInterface *)ptinfoA)->GetInterfaceData();
	((CInterface *)ptinfoA)->SetInterfaceData(0);
	((CInterface *)*ppunkStdDisp)->SetInterfaceData(pidata);

Error:
	pTypeInfo->Release();

	return hResult;
}

/*---------------------------------------------------------------------*/
/*                    Active Object Registration API                   */
/*---------------------------------------------------------------------*/

STDAPI RegisterActiveObjectA(IUnknown * punkA, REFCLSID rclsid,
		unsigned long dwFlags, unsigned long * pdwRegister)
{
	TraceSTDAPIEnter("RegisterActiveObjectA");

	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = WrapIUnknownWFromA(punkA, &pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = RegisterActiveObject(pUnk, rclsid, dwFlags, pdwRegister);

	pUnk->Release();

	return hResult;
}


STDAPI GetActiveObjectA(REFCLSID rclsid, void * pvReserved,
		IUnknown * FAR* ppunkA)
{
	TraceSTDAPIEnter("GetActiveObjectA");

	LPUNKNOWN pUnk;
	HRESULT hResult;


	hResult = GetActiveObject(rclsid, pvReserved, &pUnk);
	if (FAILED(hResult))
		return hResult;

	hResult = WrapIUnknownAFromW(pUnk, ppunkA);

	pUnk->Release();

	return hResult;
}

// only required for Chicago support, since Chicago doesn't support forwarders
// but available on all platforms

STDAPI DispGetIDsOfNamesX(ITypeInfo * ptinfo, OLECHAR * * rgszNames,
		UINT cNames, DISPID * rgdispid)
{
	return DispGetIDsOfNames(ptinfo, rgszNames, cNames, rgdispid);
}

STDAPI LoadTypeLibX(OLECHAR * szFile, ITypeLib * * pptlib)
{
	return LoadTypeLib(szFile, pptlib);
}

STDAPI RegisterTypeLibX(ITypeLib * ptlib, OLECHAR * szFullPath,
		OLECHAR * szHelpDir)
{
	return RegisterTypeLib(ptlib, szFullPath, szHelpDir);
}

STDAPI VariantChangeTypeX(
	VARIANTARG * pvargDest,
	VARIANTARG * pvargSrc,
	unsigned short wFlags,
	VARTYPE vt)
{
	return VariantChangeType(pvargDest, pvargSrc, wFlags, vt);
}

STDAPI_(unsigned long) LHashValOfNameSysX(SYSKIND syskind, LCID lcid, const OLECHAR * szName)
{
	return LHashValOfNameSys(syskind, lcid, szName);
}

STDAPI DispInvokeX(void * _this, ITypeInfo * ptinfo, DISPID dispidMember,
		unsigned short wFlags, DISPPARAMS * pparams, VARIANT * pvarResult,
		EXCEPINFO * pexcepinfo, UINT * puArgErr)
{
	return DispInvoke(_this, ptinfo, dispidMember, wFlags, pparams, pvarResult, pexcepinfo, puArgErr);
}

STDAPI_(BSTR) SysAllocStringLenW(const OLECHAR FAR* sz, unsigned int len)
{
	return SysAllocStringLen(sz, len);
}

STDAPI_(BSTR) SysAllocStringW(const OLECHAR FAR* sz)
{
	return SysAllocString(sz);
}
