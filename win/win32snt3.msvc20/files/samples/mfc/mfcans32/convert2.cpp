//+--------------------------------------------------------------------------
//
//  Copyright (C) 1993, Microsoft Corporation.  All Rights Reserved.
//
//  File:       convert.cpp
//
//  Contents:   ANSI/UNICODE Internal convertion routines.
//
//  Functions:  ConvertDispStringToW
//              ConvertDispStringToA
//              ConvertDispStringArrayToA
//              ConvertDispStringArrayToW
//              ConvertDispStringAlloc
//              ConvertDispStringFree
//              ConvertDispStringArrayFree
//              ConvertVARDESCToA
//              ConvertVARDESCToW
//              ConvertVARDESCFree
//              ConvertDispParamsToA
//              ConvertDispParamsToW
//              ConvertDispParamsFree
//              ConvertVariantToA
//              ConvertVariantToW
//              ConvertVariantArrayToA
//              ConvertVariantArrayToW
//              ConvertExcepInfoToA
//              ConvertExcepInfoToW
//
//  History:    01-Nov-93   v-kentc     Created.
//      05-Apr-94   v-kentc Use SysStringLen NOT strlen for BSTR.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"
#include <stdlib.h>



//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringToW(LPBSTR ppStrW)
{
	ULONG Count;
	HRESULT hResult;


	// If input is null then just return.
	if (*ppStrW == NULL)
		return ResultFromScode(S_OK);

	BSTRA pStrA = (BSTRA)*ppStrW;

	Count = SysStringLenA(pStrA);

	hResult = ConvertDispStringAlloc(Count * sizeof(WCHAR), ppStrW);
	if (FAILED(hResult))
		return (hResult);

	MultiByteToWideChar(CP_ACP, 0, pStrA, Count, *ppStrW, Count);

	SysFreeStringA(pStrA);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringToW(BSTRA bstrA, BSTR * ppStrW)
{
	ULONG Count;
	HRESULT hResult;


	// If input is null then just return the same.
	if (bstrA == NULL)
	{
		*ppStrW = NULL;
		return ResultFromScode(S_OK);
	}

	Count = SysStringByteLen((BSTR)bstrA);

	hResult = ConvertDispStringAlloc(Count * sizeof(WCHAR), ppStrW);
	if (FAILED(hResult))
		return (hResult);

	MultiByteToWideChar(CP_ACP, 0, bstrA, Count, *ppStrW, Count);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringToW(BSTRA bstrA, LCID lcid, LPBSTR ppStrW)
{
	static LCID lcidCache = (LCID)~0;
	static UINT cpCache;
	ULONG Count;
	char szCodePage[16];        // string of LCID code page number.
	UINT  cpLCID;               // Integer value of LCID code page.
	HRESULT hResult;


	// If input is null then just return the same.
	if (bstrA == NULL)
	{
		*ppStrW = NULL;
		return ResultFromScode(S_OK);
	}

	Count = SysStringByteLen((BSTR)bstrA);

	hResult = ConvertDispStringAlloc(Count * sizeof(WCHAR), ppStrW);
	if (FAILED(hResult))
		return (hResult);

	//
	//  Due to the slowness of GetLocaleInfoA(), cache the code page.
	//
	if (lcidCache != lcid)
	{
	GetLocaleInfoA(lcid, LOCALE_IDEFAULTCODEPAGE, szCodePage, sizeof(szCodePage));
	cpLCID = atoi(szCodePage);

		// Code pages are non-zero values.
		Assert(cpLCID);

		lcidCache = lcid;
		cpCache   = cpLCID;
	}
	else
		cpLCID = cpCache;

	MultiByteToWideChar(cpLCID, 0, bstrA, Count + 1, *ppStrW, Count + 1);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringToA(LPBSTRA ppStrA)
{
	// If input is null then just return.
	if (*ppStrA == NULL)
		return ResultFromScode(S_OK);

	BSTR pStrW = (BSTR)*ppStrA;

	HRESULT hResult = ConvertDispStringToA(pStrW, ppStrA);
	if (FAILED(hResult))
		return (hResult);

	SysFreeString(pStrW);

	return (NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringToA(BSTR pStrW, LPBSTRA ppStrA)
{
	ULONG Count;
	HRESULT hResult;


	// If input is null then just return the same.
	if (pStrW == NULL)
	{
		*ppStrA = NULL;
		return ResultFromScode(S_OK);
	}

	Count = SysStringLen(pStrW);

	hResult = ConvertDispStringAlloc(Count, (LPBSTR)ppStrA);
	if (FAILED(hResult))
		return (hResult);

	WideCharToMultiByte(CP_ACP, 0, pStrW, Count, *ppStrA, Count, NULL, NULL);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringArrayToA(LPBSTRA ppstr, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertDispStringToA(ppstr);
		if (FAILED(hResult))
			return (hResult);

		ppstr++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringArrayToW(LPBSTR ppstr, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertDispStringToW(ppstr);
		if (FAILED(hResult))
			return (hResult);

		ppstr++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringArrayToA(LPBSTR ppstr, LPBSTRA ppstrA, ULONG ulSize)
{
	ULONG i;
	HRESULT hResult;


	hResult = NOERROR;

	for (i = 0; i < ulSize; i++)
		ppstrA[i] = NULL;

	for (i = 0; i < ulSize; i++)
	{
		hResult = ConvertDispStringToA(ppstr[i], &ppstrA[i]);
		if (FAILED(hResult))
		{
			ConvertDispStringArrayFree(ppstrA, ulSize);
			break;
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringArrayToW(LPBSTRA ppstrA, LPBSTR ppstr, ULONG ulSize)
{
	ULONG i;
	HRESULT hResult;


	hResult = NOERROR;

	for (i = 0; i < ulSize; i++)
		ppstr[i] = NULL;

	for (i = 0; i < ulSize; i++)
	{
		hResult = ConvertDispStringToW(ppstrA[i], &ppstr[i]);
		if (FAILED(hResult))
		{
			ConvertDispStringArrayFree(ppstr, ulSize);
			break;
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringArrayToW(LPBSTRA ppstrA, LCID lcid, LPBSTR ppstr, ULONG ulSize)
{
	ULONG i;
	HRESULT hResult;


	hResult = NOERROR;

	for (i = 0; i < ulSize; i++)
		ppstr[i] = NULL;

	for (i = 0; i < ulSize; i++)
	{
		hResult = ConvertDispStringToW(ppstrA[i], lcid, &ppstr[i]);
		if (FAILED(hResult))
		{
			ConvertDispStringArrayFree(ppstr, ulSize);
			break;
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringAlloc
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispStringAlloc(ULONG ulSize, LPBSTR pptr)
{
	*pptr = SysAllocStringByteLen(NULL, ulSize);
	if (*pptr == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringArrayFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertDispStringArrayFree(BSTRA * ptr, ULONG ulSize)
{
	while (ulSize--)
		ConvertDispStringFreeA(*ptr++);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispStringArrayFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertDispStringArrayFree(BSTR * ptr, ULONG ulSize)
{
	while (ulSize--)
		ConvertDispStringFreeW(*ptr++);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVARDESCToA
//
//  Synopsis:   Perform in place conversion of a VARDESC structure to ANSI.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVARDESCToA(LPVARDESCA pVarDescA)
{
	if (pVarDescA->varkind == VAR_CONST)
		return ConvertVariantToA(pVarDescA->lpvarValue);

	return NOERROR;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVARDESCToW
//
//  Synopsis:   Perform inplace conversion of a VARDESC structure to ANSI.
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVARDESCToW(LPVARDESC pVarDesc)
{
	if (pVarDesc->varkind == VAR_CONST)
		return ConvertVariantToW(pVarDesc->lpvarValue);

	return NOERROR;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVARDESCToA
//
//  Synopsis:   Perform conversion of a VARDESC structure to ANSI.
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
HRESULT ConvertVARDESCToA(LPVARDESC pVarDesc, LPVARDESCA * ppVarDescA)
{
	LPVARDESCA pVarDescA;
	HRESULT hResult;


	pVarDescA = new VARDESCA;

	memcpy((LPVOID)pVarDescA, (LPVOID)pVarDesc, sizeof(VARDESCA));

	if (pVarDesc->varkind == VAR_CONST)
	{
		hResult = ConvertVariantToA(pVarDesc->lpvarValue,
				&pVarDescA->lpvarValue);
	}
	else
		hResult = NOERROR;

	*ppVarDescA = pVarDescA;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVARDESCToW
//
//  Synopsis:   Perform conversion of a VARDESC structure to ANSI.
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
HRESULT ConvertVARDESCToW(LPVARDESCA pVarDescA, LPVARDESC * ppVarDesc)
{
	LPVARDESC pVarDesc;
	HRESULT hResult;


	pVarDesc = new VARDESC;

	memcpy((LPVOID)pVarDesc, (LPVOID)pVarDescA, sizeof(VARDESC));

	if (pVarDescA->varkind == VAR_CONST)
	{
		hResult = ConvertVariantToW(pVarDescA->lpvarValue,
				&pVarDesc->lpvarValue);
	}
	else
		hResult = NOERROR;

	*ppVarDesc = pVarDesc;

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVARDESCFree
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
VOID ConvertVARDESCFree(LPVARDESC pVarDesc)
{
	if (pVarDesc->varkind == VAR_CONST)
		ConvertVariantFree(pVarDesc->lpvarValue);

	delete pVarDesc;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVARDESCFree
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
VOID ConvertVARDESCFree(LPVARDESCA pVarDescA)
{
	if (pVarDescA->varkind == VAR_CONST)
		ConvertVariantFree(pVarDescA->lpvarValue);

	delete pVarDescA;
}

// UNDONE: Must still analyze/fix handling of:
// UNDONE:  (VT_BYREF | VT_UNKNOWN)
// UNDONE:  (VT_ARRAY | VT_XXX)
// UNDONE:  (VT_BYREF | VT_ARRAY | VT_XXX)

//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispParamsToA
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispParamsToA(LPDISPPARAMS pDispParams,
		LPDISPPARAMSA * ppDispParamsA, LPVARIANTA * ppByrefTempA)
{
	LPDISPPARAMSA pDispParamsA;
	HRESULT hResult;
	UINT i;
	LPVARIANTA pByrefTempA = NULL;

	pDispParamsA = new DISPPARAMSA;

	if (pDispParams->cArgs)
	{
		LPVARIANTA pVariantA;
		VARTYPE vt;

		pVariantA = new VARIANTA [pDispParams->cArgs];
		pDispParamsA->rgvarg = pVariantA;

		for (i = 0; i < pDispParams->cArgs; i++)
		{
			vt = pDispParams->rgvarg[i].vt;

			VariantInit((VARIANT *)pVariantA);
			if (vt & VT_BYREF)
			{
				// alloc byref array now if not already alloc'ed
				if (pByrefTempA == NULL)
					pByrefTempA = new VARIANTA [pDispParams->cArgs];
				VariantInitA(&pByrefTempA[i]);

				// copy real data to temp variant, removing the VT_BYREF
				hResult = VariantCopyInd((VARIANT *)&pByrefTempA[i], &(pDispParams->rgvarg[i]));
				if (FAILED(hResult))
					goto Error;
				// convert it to Ansi
				hResult = ConvertVariantToA(&pByrefTempA[i]);
				if (FAILED(hResult))
					goto Error;

				// Create a new byref variant to point to the temp variant
				pVariantA->vt = vt;
				switch(vt)
				{
				case (VT_VARIANT | VT_BYREF):       // variant
					pVariantA->pvarVal = &pByrefTempA[i];
					break;
				default:                // normal
					pVariantA->byref = (void *)&(pByrefTempA[i].iVal);
				}
			}
			else
			{
				hResult = VariantCopy((VARIANT *)pVariantA, &(pDispParams->rgvarg[i]));
				if (hResult != NOERROR)
					goto Error;

				hResult = ConvertVariantToA(pVariantA);
				if (FAILED(hResult))
					goto Error;
			}
			pVariantA++;
		}
	}
	else
		pDispParamsA->rgvarg = NULL;

	pDispParamsA->cArgs = pDispParams->cArgs;

	if (pDispParams->cNamedArgs)
	{
		pDispParamsA->rgdispidNamedArgs = new DISPID [pDispParams->cNamedArgs];

		memcpy((LPVOID)pDispParamsA->rgdispidNamedArgs,
				pDispParams->rgdispidNamedArgs,
				pDispParams->cNamedArgs * sizeof(DISPID));
	}
	else
		pDispParamsA->rgdispidNamedArgs = NULL;

	pDispParamsA->cNamedArgs = pDispParams->cNamedArgs;

	*ppDispParamsA = pDispParamsA;
	*ppByrefTempA = pByrefTempA;    // return * to alloc'ed data

	return NOERROR;

Error:
	pDispParamsA->cArgs = i+1;  // set count to how many we've filled in so far,
				// so ConvertDispParamsFree will free correct
				// amount of memory.
	ConvertDispParamsFree((LPDISPPARAMS)pDispParamsA, (LPVARIANT)pByrefTempA);
	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispParamsToW
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
HRESULT ConvertDispParamsToW(LPDISPPARAMSA pDispParamsA,
		LPDISPPARAMS * ppDispParams, LPVARIANT * ppByrefTemp)
{
	LPDISPPARAMS pDispParams;
	HRESULT hResult;
	UINT i;
	LPVARIANT pByrefTemp = NULL;


	pDispParams = new DISPPARAMS;

	if (pDispParamsA->cArgs)
	{
		LPVARIANT pVariant;
		VARTYPE vt;

		pVariant = new VARIANT [pDispParamsA->cArgs];
		pDispParams->rgvarg = pVariant;

		for (i = 0; i < pDispParamsA->cArgs; i++)
		{
			vt = pDispParamsA->rgvarg[i].vt;

			VariantInit(pVariant);
			if (vt & VT_BYREF)
			{
				// alloc byref array now if not already alloc'ed
				if (pByrefTemp == NULL)
					pByrefTemp = new VARIANT [pDispParamsA->cArgs];
				VariantInit(&pByrefTemp[i]);

				// copy real data to temp variant, removing the VT_BYREF
				hResult = VariantCopyInd(&pByrefTemp[i], (VARIANT *)&(pDispParamsA->rgvarg[i]));
				if (FAILED(hResult))
					goto Error;
				// convert it to Unicode
				hResult = ConvertVariantToW(&pByrefTemp[i]);
				if (FAILED(hResult))
					goto Error;

				// Create a new byref variant to point to the temp variant
				pVariant->vt = vt;
				switch(vt)
				{
				case (VT_VARIANT | VT_BYREF):       // variant
					pVariant->pvarVal = &pByrefTemp[i];
					break;
				default:                // normal
					pVariant->byref = (void *)&(pByrefTemp[i].iVal);
				}
			}
			else
			{
				hResult = VariantCopy(pVariant, (VARIANT *)&(pDispParamsA->rgvarg[i]));
				if (hResult != NOERROR)
					goto Error;

				hResult = ConvertVariantToW(pVariant);
				if (FAILED(hResult))
					goto Error;
			}

			pVariant++;
		}
	}
	else
		pDispParams->rgvarg = NULL;

	pDispParams->cArgs = pDispParamsA->cArgs;

	if (pDispParamsA->cNamedArgs)
	{
		pDispParams->rgdispidNamedArgs = new DISPID [pDispParamsA->cNamedArgs];

		memcpy((LPVOID)pDispParams->rgdispidNamedArgs,
				pDispParamsA->rgdispidNamedArgs,
				pDispParamsA->cNamedArgs * sizeof(DISPID));
	}
	else
		pDispParams->rgdispidNamedArgs = NULL;

	pDispParams->cNamedArgs = pDispParamsA->cNamedArgs;

	*ppDispParams = pDispParams;
	*ppByrefTemp = pByrefTemp;      // return * to alloc'ed data

	return NOERROR;

Error:
	pDispParams->cArgs = i+1;   // set count to how many we've filled in so far,
				// so ConvertDispParamsFree will free correct
				// amount of memory.
	ConvertDispParamsFree(pDispParams, pByrefTemp);
	return hResult;
}


USHORT rgcbCopy[] = {
	0,      //VT_EMPTY
	4,      //VT_NULL
	2,      //VT_I2
	4,      //VT_I4
	4,      //VT_R4
	8,      //VT_R8
	8,      //VT_CY
	8,      //VT_DATE
	4,      //VT_BSTR
	4,      //VT_DISPATCH
	4,      //VT_ERROR
	2,      //VT_BOOL
	0,      //VT_VARIANT
	4,      //VT_UNKNOWN
	0,      //unused
	0,      //unused
	1,      //VT_I1
	1,      //VT_UI1
};

//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispParamsCopyBack
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:
//
//---------------------------------------------------------------------------
HRESULT ConvertDispParamsCopyBack(LPDISPPARAMSA pDispParamsOldA, LPVARIANT pByrefTemp)
{
	LPVARIANTA pVariantOld;
	LPVARIANT pVariantNew;
	HRESULT hResult = NOERROR;

	pVariantOld = pDispParamsOldA->rgvarg;
	pVariantNew = pByrefTemp;

	Assert(pByrefTemp);
	Assert(pDispParamsOldA->cArgs);
	for (UINT i = 0; i < pDispParamsOldA->cArgs; i++, pVariantOld++, pVariantNew++)
	{
		switch (pVariantOld->vt)
		{
		case (VT_BSTR | VT_BYREF):
			Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
			// free old byref Ansi string data, translate new Unicode
			// string, and copy it back to the old byref string
			SysFreeStringA(*pVariantOld->pbstrVal);
			*pVariantOld->pbstrVal = NULL;      // in case of failure
			hResult = ConvertDispStringToA(pVariantNew->bstrVal, pVariantOld->pbstrVal);
			break;

		case (VT_DISPATCH | VT_BYREF):
			Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
			// free old byref Ansi object, create a Unicode wrapper,
			// and copy it back to the old byref object
			if (*pVariantOld->ppdispVal)
					(*pVariantOld->ppdispVal)->Release();
			*pVariantOld->ppdispVal = NULL;     // in case of failure
			hResult = WrapIDispatchAFromW(pVariantNew->pdispVal,
						  pVariantOld->ppdispVal);
			break;

		case (VT_UNKNOWN | VT_BYREF):
			Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
			// free old byref Ansi object, create a Unicode wrapper,
			// and copy it back to the old byref object
			if (*pVariantOld->ppunkVal)
					(*pVariantOld->ppunkVal)->Release();
			*pVariantOld->ppunkVal = NULL;     // in case of failure
			hResult = WrapIUnknownAFromW(pVariantNew->punkVal,
						  pVariantOld->ppunkVal);
			break;

		case (VT_VARIANT | VT_BYREF):
			// free old byref Ansi Variant, translate new Unicode
			// variant, and copy it back to the old byref variant
			VariantClearA(pVariantOld->pvarVal);
			hResult = ConvertVariantToA((LPVARIANTA)pVariantNew);
			if (hResult == NOERROR)
			{
				memcpy(pVariantOld->pvarVal, pVariantNew, sizeof(VARIANT));
				VariantInit(pVariantNew);   // don't try to free this
			}
			break;

		default:
			if (pVariantOld->vt & VT_BYREF)
			{
				// non-owner byref data
				Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
				memcpy(pVariantOld->piVal, &pVariantNew->iVal, rgcbCopy[pVariantNew->vt]);
			}
			break;
		}
		if (hResult != NOERROR)
			break;
	}
	return hResult;
}

//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispParamsCopyBack
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:
//
//---------------------------------------------------------------------------
HRESULT ConvertDispParamsCopyBack(LPDISPPARAMS pDispParamsOld, LPVARIANTA pByrefTempA)
{
	LPVARIANT pVariantOld;
	LPVARIANTA pVariantNew;
	HRESULT hResult = NOERROR;

	pVariantOld = pDispParamsOld->rgvarg;
	pVariantNew = pByrefTempA;

	Assert(pByrefTempA);
	Assert(pDispParamsOld->cArgs);
	for (UINT i = 0; i < pDispParamsOld->cArgs; i++, pVariantOld++, pVariantNew++)
	{
		switch (pVariantOld->vt)
		{
		case (VT_BSTR | VT_BYREF):
			Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
			// free old byref Unicode string data, translate new Ansi
			// string, and copy it back to the old byref string
			SysFreeString(*pVariantOld->pbstrVal);
			*pVariantOld->pbstrVal = NULL;      // in case of failure
			hResult = ConvertDispStringToW(pVariantNew->bstrVal, pVariantOld->pbstrVal);
			break;

		case (VT_DISPATCH | VT_BYREF):
			Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
			// free old byref Unicode object, create a Ansi wrapper,
			// and copy it back to the old byref object
			if (*pVariantOld->ppdispVal)
				(*pVariantOld->ppdispVal)->Release();
			*pVariantOld->ppdispVal = NULL;     // in case of failure
			hResult = WrapIDispatchWFromA(pVariantNew->pdispVal,
						  pVariantOld->ppdispVal);
			break;

		case (VT_UNKNOWN | VT_BYREF):
			Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
			// free old byref Unicode object, create a Ansi wrapper,
			// and copy it back to the old byref object
			if (*pVariantOld->ppunkVal)
				(*pVariantOld->ppunkVal)->Release();
			*pVariantOld->ppunkVal = NULL;   // in case of failure
			hResult = WrapIUnknownWFromA(pVariantNew->punkVal,
						  pVariantOld->ppunkVal);
			break;

		case (VT_VARIANT | VT_BYREF):
			// free old byref Unicode Variant, translate new Ansi
			// variant, and copy it back to the old byref variant
			VariantClear(pVariantOld->pvarVal);
			hResult = ConvertVariantToW((LPVARIANT)pVariantNew);
			if (hResult == NOERROR)
			{
				memcpy(pVariantOld->pvarVal, pVariantNew, sizeof(VARIANT));
				VariantInitA(pVariantNew);  // don't try to free this
			}
			break;

		default:
			if (pVariantOld->vt & VT_BYREF)
			{
				// non-owner byref data
				Assert(pVariantNew->vt == (pVariantOld->vt & ~VT_BYREF));
				memcpy(pVariantOld->piVal, &pVariantNew->iVal, rgcbCopy[pVariantNew->vt]);
			}
			break;
		}
		if (hResult != NOERROR)
			break;
	}
	return hResult;
}

//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDispParamsFree
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//      It can take either LPDISPPARAMS or LPDISPPARAMSA.
//
//---------------------------------------------------------------------------
VOID ConvertDispParamsFree(LPDISPPARAMS pDispParams, LPVARIANT pByrefTemp)
{
	if (pDispParams->cArgs)
	{
		LPVARIANT pVariant;

		pVariant = pDispParams->rgvarg;
		for (UINT i = 0; i < pDispParams->cArgs; i++, pVariant++)
		{
			if (pVariant->vt & VT_BYREF)
			{
				Assert(pByrefTemp);
				VariantClear(&pByrefTemp[i]);
			}
			VariantClear(pVariant);
		}

		delete pDispParams->rgvarg;
	}

	if (pDispParams->cNamedArgs)
		delete pDispParams->rgdispidNamedArgs;

	delete pDispParams;

	if (pByrefTemp)
		delete pByrefTemp;
}


unsigned long SafeArrayElems(SAFEARRAY FAR* psa)
{
	unsigned long cElems;
	unsigned short us;
	SAFEARRAYBOUND FAR* psabound;

	cElems = 0L;
	if(psa->cDims)
	{
		psabound = &psa->rgsabound[psa->cDims - 1];
		cElems = 1L;
		for (us = 0; us < psa->cDims; ++us)
		{
			cElems *= psabound->cElements;
			--psabound;
		}
	}

	return cElems;
}

//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantToA
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVariantToA(LPVARIANTA pVariantA)
{
	LPDISPATCH pDispatch;
	LPDISPATCHA pDispatchA;
	LPUNKNOWN pUnknown;
	LPUNKNOWN pUnknownA;
	HRESULT hResult = NOERROR;
	VARTYPE vt;

	if (pVariantA == NULL)
		return NOERROR;

	vt = V_VT(pVariantA);

	if (vt & VT_ARRAY)
	{
		SAFEARRAYA * psa;

		if (vt & VT_BYREF)
			psa = *(pVariantA->pparray);
		else
			psa = pVariantA->parray;

		vt &= ~(VT_ARRAY|VT_BYREF);

		switch (vt)
		{
		case VT_BSTR:
		case VT_VARIANT:
		case VT_DISPATCH:
		case VT_UNKNOWN:
			// iterate over the elements, translating each one inplace
			SafeArrayLockA(psa);

			BYTE * pvData;
			ULONG cElems;

			// point to first element;
			SafeArrayAccessDataA(psa, (VOID **)&pvData);
			for (cElems = SafeArrayElems(psa); cElems > 0; cElems--)
			{
				switch (vt)
				{
				case VT_BSTR:
					hResult = ConvertDispStringToA((LPBSTRA)pvData);
					pvData += sizeof(BSTRA);
					break;
				case VT_VARIANT:
					hResult = ConvertVariantToA((LPVARIANTA)pvData);
					pvData += sizeof(VARIANTA);
					break;
				case VT_DISPATCH:
					pDispatch = *(LPDISPATCH *)pvData;
					hResult = WrapIDispatchAFromW(pDispatch, &pDispatchA);
					if (!FAILED(hResult))
					{
						if (pDispatch)
							pDispatch->Release();
						*(LPDISPATCHA *)pvData = pDispatchA;
					}
					pvData += sizeof(LPDISPATCHA);
					break;
				case VT_UNKNOWN:
					pUnknown = *(LPUNKNOWN *)pvData;
					hResult = WrapIUnknownAFromW(pUnknown, &pUnknownA);
					if (!FAILED(hResult))
					{
						if (pUnknown)
							pUnknown->Release();
						*(LPUNKNOWN *)pvData = pUnknownA;
					}
					pvData += sizeof(LPUNKNOWN);
					break;
				}
				if (hResult != NOERROR)
					break;
			}
			SafeArrayUnaccessDataA(psa);
			SafeArrayUnlockA(psa);
			break;

		default:
			break;  // nothing to do
		}
	}
	else
	{   // not an array
		switch (vt)
		{
		case VT_BSTR:
			hResult = ConvertDispStringToA(&pVariantA->bstrVal);
			break;

		case VT_BYREF|VT_BSTR:
			// can't convert to a byref BSTR, so must convert to a byval BSTR
			BSTRA bstrTmp;
			hResult = ConvertDispStringToA((BSTR)*pVariantA->bstrVal, &bstrTmp);
			if (hResult == NOERROR)
			{
				V_VT(pVariantA) = VT_BSTR;      // no longer byref
				pVariantA->bstrVal = bstrTmp;
			}
			break;

		case VT_BYREF|VT_VARIANT:
			// can't convert to a byref VARIANT, so must convert to a byval VARIANT
			VARIANTA varTmp;
			VariantInit((LPVARIANT)&varTmp);
			hResult = VariantCopy((LPVARIANT)&varTmp, (LPVARIANT)pVariantA->pvarVal);
			if (hResult == NOERROR)
			{
				hResult = ConvertVariantToA(&varTmp);
				if (hResult == NOERROR)
					*pVariantA = varTmp;
				else
					VariantClear((LPVARIANT)&varTmp);
			}
			break;

		case VT_DISPATCH:
			pDispatch = (LPDISPATCH)pVariantA->pdispVal;
			hResult = WrapIDispatchAFromW(pDispatch, &pDispatchA);
			if (!FAILED(hResult))
			{
				if (pDispatch)
					pDispatch->Release();
				pVariantA->pdispVal = pDispatchA;
			}
			break;

		case VT_BYREF|VT_DISPATCH:
			// can't convert to a byref VT_DISPATCH, so must convert to a byval VT_DISPATCH
			pDispatch = (LPDISPATCH)*pVariantA->ppdispVal;
			hResult = WrapIDispatchAFromW(pDispatch, &pDispatchA);
			if (!FAILED(hResult))
			{
				V_VT(pVariantA) = VT_DISPATCH;      // no longer byref
				pVariantA->pdispVal = pDispatchA;
			}
			break;

		case VT_UNKNOWN:
			pUnknown = (LPUNKNOWN)pVariantA->punkVal;
			hResult = WrapIUnknownAFromW(pUnknown, &pUnknownA);
			if (!FAILED(hResult))
			{
				if (pUnknown)
					pUnknown->Release();
				pVariantA->punkVal = pUnknownA;
			}
			break;

		case VT_BYREF|VT_UNKNOWN:
			// can't convert to a byref VT_UNKNOWN, so must convert to a byval VT_UNKNOWN
			pUnknown = (LPUNKNOWN)*pVariantA->ppunkVal;
			hResult = WrapIUnknownAFromW(pUnknown, &pUnknownA);
			if (!FAILED(hResult))
			{
				V_VT(pVariantA) = VT_UNKNOWN;      // no longer byref
				pVariantA->punkVal = pUnknownA;
			}
			break;

		default:
			break;
		}
	}
	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantToW
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVariantToW(LPVARIANT pVariant)
{
	LPDISPATCHA pDispatchA;
	LPDISPATCH pDispatch;
	LPUNKNOWN pUnknownA;
	LPUNKNOWN pUnknown;
	HRESULT hResult = NOERROR;
	VARTYPE vt;


	if (pVariant == NULL)
		return NOERROR;

	vt = V_VT(pVariant);

	if (vt & VT_ARRAY)
	{
		SAFEARRAY * psa;

		if (vt & VT_BYREF)
			psa = *(pVariant->pparray);
		else
			psa = pVariant->parray;

		vt &= ~(VT_ARRAY|VT_BYREF);

		switch (vt)
		{
		case VT_BSTR:
		case VT_VARIANT:
		case VT_DISPATCH:
		case VT_UNKNOWN:
			SafeArrayLock(psa);

			BYTE * pvData;
			ULONG cElems;

			// point to first element;
			SafeArrayAccessDataA(psa, (VOID **)&pvData);
			for (cElems = SafeArrayElems(psa); cElems > 0; cElems--)
			{
				switch (vt)
				{
				case VT_BSTR:
					hResult = ConvertDispStringToW((LPBSTR)pvData);
					pvData += sizeof(BSTR);
					break;
				case VT_VARIANT:
					hResult = ConvertVariantToW((LPVARIANT)pvData);
					pvData += sizeof(VARIANT);
					break;
				case VT_DISPATCH:
					pDispatchA = *(LPDISPATCHA *)pvData;
					hResult = WrapIDispatchWFromA(pDispatchA, &pDispatch);
					if (!FAILED(hResult))
					{
						if (pDispatchA)
							pDispatchA->Release();
						*(LPDISPATCH *)pvData = pDispatch;
					}
					pvData += sizeof(LPDISPATCH);
					break;
				case VT_UNKNOWN:
					pUnknownA = *(LPUNKNOWN *)pvData;
					hResult = WrapIUnknownWFromA(pUnknownA, &pUnknown);
					if (!FAILED(hResult))
					{
						if (pUnknownA)
							pUnknownA->Release();
						*(LPUNKNOWN *)pvData = pUnknown;
					}
					pvData += sizeof(LPUNKNOWN);
					break;
				}
				if (hResult != NOERROR)
					break;
			}
			SafeArrayUnaccessDataA(psa);
			SafeArrayUnlockA(psa);
			break;

		default:
			break;  // nothing to do
		}
	}
	else
	{   // not an array
		switch (vt)
		{
		case VT_BSTR:
			hResult = ConvertDispStringToW(&pVariant->bstrVal);
			break;

		case VT_BYREF|VT_BSTR:
			// can't convert to a byref BSTR, so must convert to a byval BSTR
			BSTR bstrTmp;
			hResult = ConvertDispStringToW((BSTRA)*pVariant->pbstrVal, &bstrTmp);
			if (hResult == NOERROR)
			{
				V_VT(pVariant) = VT_BSTR;       // no longer byref
				pVariant->bstrVal = bstrTmp;
			}
			break;

		case VT_BYREF|VT_VARIANT:
			// can't convert to a byref VARIANT, so must convert to a byval VARIANT
			VARIANT varTmp;
			VariantInit(&varTmp);
			hResult = VariantCopy(&varTmp, pVariant->pvarVal);
			if (hResult == NOERROR)
			{
				hResult = ConvertVariantToW(&varTmp);
				if (hResult == NOERROR)
					*pVariant = varTmp;
				else
					VariantClear(&varTmp);
			}
			break;

		case VT_DISPATCH:
			pDispatchA = (LPDISPATCHA)pVariant->pdispVal;
			hResult = WrapIDispatchWFromA(pDispatchA, &pDispatch);
			if (!FAILED(hResult))
			{
				if (pDispatchA)
					pDispatchA->Release();
				pVariant->pdispVal = pDispatch;
			}
			break;

		case VT_BYREF|VT_DISPATCH:
			// can't convert to a byref VT_DISPATCH, so must convert to a byval VT_DISPATCH
			pDispatchA = (LPDISPATCHA)*pVariant->ppdispVal;
			hResult = WrapIDispatchWFromA(pDispatchA, &pDispatch);
			if (!FAILED(hResult))
			{
				V_VT(pVariant) = VT_DISPATCH;       // no longer byref
				pVariant->pdispVal = pDispatch;
			}
			break;

		case VT_UNKNOWN:
			pUnknownA = (LPUNKNOWN)pVariant->punkVal;
			hResult = WrapIUnknownWFromA(pUnknownA, &pUnknown);
			if (!FAILED(hResult))
			{
				if (pUnknownA)
					pUnknownA->Release();
				pVariant->punkVal = pUnknown;
			}
			break;

		case VT_BYREF|VT_UNKNOWN:
			// can't convert to a byref VT_UNKNOWN, so must convert to a byval VT_UNKNOWN
			pUnknownA = (LPUNKNOWN)*pVariant->ppunkVal;
			hResult = WrapIUnknownWFromA(pUnknownA, &pUnknown);
			if (!FAILED(hResult))
			{
				V_VT(pVariant) = VT_UNKNOWN;       // no longer byref
				pVariant->punkVal = pUnknown;
			}
			break;

		default:
			break;
		}
	}
	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantToA
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVariantToA(LPVARIANT pVariant, LPVARIANTA * ppVariantA)
{
	HRESULT hResult;
	*ppVariantA = new VARIANTA;
	if (*ppVariantA == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	VariantInit((VARIANT *)*ppVariantA);
	hResult = VariantCopy((VARIANT *)*ppVariantA, pVariant);
	if (hResult != NOERROR)
	return hResult;

	return ConvertVariantToA(*ppVariantA);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantToW
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVariantToW(LPVARIANTA pVariantA, LPVARIANT * ppVariant)
{
	HRESULT hResult;
	*ppVariant = new VARIANT;
	if (*ppVariant == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	VariantInit(*ppVariant);
	hResult = VariantCopy(*ppVariant, (VARIANT *)pVariantA);
	if (hResult != NOERROR)
	return hResult;

	return ConvertVariantToW(*ppVariant);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantArrayToA
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVariantArrayToA(LPVARIANTA pVariantA, ULONG ulSize)
{
	HRESULT hResult;


	if (pVariantA == NULL)
		return NOERROR;

	while (ulSize--)
	{
		hResult = ConvertVariantToA(pVariantA);
		if (FAILED(hResult))
			return hResult;

		pVariantA++;
	}

	return NOERROR;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantArrayToW
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
HRESULT ConvertVariantArrayToW(LPVARIANT pVariant, ULONG ulSize)
{
	HRESULT hResult;


	if (pVariant == NULL)
		return NOERROR;

	while (ulSize--)
	{
		hResult = ConvertVariantToW(pVariant);
		if (FAILED(hResult))
			return hResult;

		pVariant++;
	}

	return NOERROR;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantFree
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
VOID ConvertVariantFree(LPVARIANT pVariant)
{
	VariantClear(pVariant);
	delete pVariant;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertVariantFree
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
VOID ConvertVariantFree(LPVARIANTA pVariantA)
{
	VariantClearA(pVariantA);
	delete pVariantA;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertExcepInfoToA
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDAPI ConvertExcepInfoToA(LPEXCEPINFOA pExcepInfoA)
{
	HRESULT hResult;


	if (pExcepInfoA == NULL)
		return NOERROR;

	hResult = ConvertDispStringToA(&pExcepInfoA->bstrSource);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToA(&pExcepInfoA->bstrDescription);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToA(&pExcepInfoA->bstrHelpFile);
	if (FAILED(hResult))
		return hResult;

	return NOERROR;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertExcepInfoToW
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
STDAPI ConvertExcepInfoToW(LPEXCEPINFO pExcepInfo)
{
	HRESULT hResult;


	if (pExcepInfo == NULL)
		return NOERROR;

	hResult = ConvertDispStringToW(&pExcepInfo->bstrSource);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToW(&pExcepInfo->bstrDescription);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertDispStringToW(&pExcepInfo->bstrHelpFile);
	if (FAILED(hResult))
		return hResult;

	return NOERROR;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertInterfaceDataToW
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
HRESULT ConvertInterfaceDataToW(LPINTERFACEDATAA pDataA, LCID lcid,
		LPINTERFACEDATA * ppData)
{
	LPINTERFACEDATA pData;
	HRESULT hResult;


	//
	//  Allocate the new Unicode INTERFACEDATA data structure.
	//
	pData = new INTERFACEDATA;
	if (pData == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	pData->cMembers = pDataA->cMembers;

	//
	//  Convert the METHODDATA data structures to Unicode.
	//
	if (pDataA->cMembers)
	{
		LPMETHODDATA pMethod;
		LPMETHODDATAA pMethodA;
		UINT i;

		pData->pmethdata = new METHODDATA [pData->cMembers];

		for (pMethod = pData->pmethdata, pMethodA = pDataA->pmethdata, i = 0;
				i < pData->cMembers;
				pMethod++, pMethodA++, i++)
		{
			hResult = ConvertStringToW(pMethodA->szName, lcid,
					&pMethod->szName);
			if (FAILED(hResult))
				return hResult;

			if (pMethodA->ppdata)
			{
				pMethod->ppdata = new PARAMDATA;

				hResult = ConvertStringToW(pMethodA->ppdata->szName, lcid,
						&pMethod->ppdata->szName);
				if (FAILED(hResult))
					return hResult;

				pMethod->ppdata->vt = pMethodA->ppdata->vt;
			}
			else
				pMethod->ppdata = NULL;

			pMethod->dispid   = pMethodA->dispid;
			pMethod->iMeth    = pMethodA->iMeth;
			pMethod->cc       = pMethodA->cc;
			pMethod->cArgs    = pMethodA->cArgs;
			pMethod->wFlags   = pMethodA->wFlags;
			pMethod->vtReturn = pMethodA->vtReturn;
		}
	}
	else
		pData->pmethdata = NULL;

	*ppData = pData;

	return NOERROR;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertExcepInfoToW
//
//  Synopsis:
//
//  Returns:    None.
//
//  Notes:      This structure is only used internally so the standard C++
//              memory routines are used to create and free it.
//
//---------------------------------------------------------------------------
HRESULT ConvertInterfaceDataFree(LPINTERFACEDATA pData)
{
	if (pData->cMembers)
	{
		LPMETHODDATA pMethod;
		UINT i;

		for (pMethod = pData->pmethdata, i = 0; i < pData->cMembers;
				pMethod++, i++)
		{
			ConvertStringFree(pMethod->szName);

			if (pMethod->ppdata)
			{
				ConvertStringFree(pMethod->ppdata->szName);
				delete pMethod->ppdata;
			}
		}

		delete pData->pmethdata;
	}

	delete pData;

	return NOERROR;
}


STDAPI Ole2AnsiVariantAFromW(LPVARIANT pVariant, LPVARIANTA pVariantA)
{
	LPDISPATCH pDispatch;
	LPDISPATCHA pDispatchA;
	LPUNKNOWN pUnknown;
	LPUNKNOWN pUnknownA;
	HRESULT hResult = NOERROR;
	VARTYPE vt;

	if (pVariant == NULL)
		return NOERROR;

	vt = V_VT(pVariant);

	if (vt & VT_ARRAY)
	{
		SAFEARRAYA * psa, * psaOut;

		if (vt & VT_BYREF)
			psa = *(pVariant->pparray);
		else
			psa = pVariant->parray;

		vt &= ~(VT_ARRAY|VT_BYREF);

		switch (vt)
		{
		case VT_BSTR:
		case VT_VARIANT:
		case VT_DISPATCH:
		case VT_UNKNOWN:
			BYTE * pvData, * pvDataOut;
			ULONG cElems, cDims;

			// iterate over the elements, translating and copying to new array.
			hResult = SafeArrayAllocDescriptorA( psa->cDims, &psaOut );
			if( FAILED(hResult) )
				return hResult;

			psaOut->cDims = psa->cDims;
			psaOut->fFeatures = psa->fFeatures;
			psaOut->cbElements = psa->cbElements;
			for (cDims=0; cDims < psa->cDims; cDims++)
				psaOut->rgsabound[ cDims ] = psa->rgsabound[ cDims ];
			hResult = SafeArrayAllocDataA( psaOut );
			if( FAILED( hResult ) )
			{
				SafeArrayDestroyDescriptorA( psaOut );
				return hResult;
			}

			SafeArrayLockA(psa);

			// point to first element;
			SafeArrayAccessData(psa, (VOID **)&pvData);
			SafeArrayAccessDataA(psaOut, (VOID **)&pvDataOut);

			for (cElems = SafeArrayElems(psa); cElems > 0; cElems--)
			{
				switch (vt)
				{
				case VT_BSTR:
					hResult = ConvertDispStringToA(*(LPBSTR)pvData,
							(LPBSTRA)pvDataOut);
					pvData += sizeof(BSTR);
					pvDataOut += sizeof(BSTRA);
					break;

				case VT_VARIANT:
					hResult = Ole2AnsiVariantAFromW((LPVARIANT)pvData,
							 (LPVARIANTA)pvDataOut);
					pvData += sizeof(VARIANT);
					pvDataOut += sizeof(VARIANTA);
					break;

				case VT_DISPATCH:
					pDispatch = *(LPDISPATCH *)pvData;
					hResult = WrapIDispatchAFromW(pDispatch, &pDispatchA);
					if (!FAILED(hResult))
						*(LPDISPATCHA *)pvDataOut = pDispatchA;
					pvData += sizeof(LPDISPATCH);
					pvDataOut += sizeof(LPDISPATCHA);
					break;

				case VT_UNKNOWN:
					pUnknown = *(LPUNKNOWN *)pvData;
					hResult = WrapIUnknownAFromW(pUnknown, &pUnknownA);
					if (!FAILED(hResult))
						*(LPUNKNOWN *)pvDataOut = pUnknownA;
					pvData += sizeof(LPUNKNOWN);
					pvDataOut += sizeof(LPUNKNOWN);
					break;
				}
				if (hResult != NOERROR)
					break;
			}

			SafeArrayUnaccessData(psa);
			SafeArrayUnaccessDataA(psaOut);
			pVariantA->parray = psaOut;
			SafeArrayUnlockA(psa);
			break;

		default:
			hResult = SafeArrayCopyA( (SAFEARRAYA *)psa, &pVariantA->parray );
			break;  // nothing to do
		}
		pVariantA->vt = VT_ARRAY & vt;

	}
	else
	{   // not an array
		switch (vt)
		{
		case VT_BSTR:
			hResult = ConvertDispStringToA(pVariant->bstrVal, &pVariantA->bstrVal);
			break;

		case VT_BYREF|VT_BSTR:
			hResult = ConvertDispStringToA(*pVariant->pbstrVal, &pVariantA->bstrVal);
			V_VT(pVariantA) = VT_BSTR;      // no longer byref
			break;

		case VT_BYREF|VT_VARIANT:
			hResult = Ole2AnsiVariantAFromW(pVariant->pvarVal, pVariantA->pvarVal);
			V_VT(pVariantA) = VT_VARIANT;       // no longer byref
			break;

		case VT_DISPATCH:
			pDispatch = pVariant->pdispVal;
			hResult = WrapIDispatchAFromW(pDispatch, &pDispatchA);
			if (!FAILED(hResult))
				pVariantA->pdispVal = pDispatchA;
			break;

		case VT_BYREF|VT_DISPATCH:
			pDispatch = *pVariant->ppdispVal;
			hResult = WrapIDispatchAFromW(pDispatch, &pDispatchA);
			if (!FAILED(hResult))
			{
				pVariantA->pdispVal = pDispatchA;
				V_VT(pVariant) = VT_DISPATCH;       // no longer byref
			}
			break;

		case VT_UNKNOWN:
			pUnknown = pVariant->punkVal;
			hResult = WrapIUnknownAFromW(pUnknown, &pUnknownA);
			if (!FAILED(hResult))
				pVariantA->punkVal = pUnknownA;
			break;

		case VT_BYREF|VT_UNKNOWN:
			pUnknown = *pVariant->ppunkVal;
			hResult = WrapIUnknownAFromW(pUnknown, &pUnknownA);
			if (!FAILED(hResult))
			{
				pVariantA->punkVal = pUnknownA;
				V_VT(pVariant) = VT_UNKNOWN;       // no longer byref
			}
			break;

		default:
			VariantCopyA( pVariantA, (LPVARIANTA)pVariant );
			break;
		}
		pVariantA->vt = vt;
	}

	return hResult;
}


STDAPI Ole2AnsiVariantWFromA(LPVARIANTA pVariantA, LPVARIANT pVariant)
{
	LPDISPATCH pDispatch;
	LPDISPATCHA pDispatchA;
	LPUNKNOWN pUnknown;
	LPUNKNOWN pUnknownA;
	HRESULT hResult = NOERROR;
	VARTYPE vt;

	if (pVariantA == NULL)
		return NOERROR;

	vt = V_VT(pVariantA);

	if (vt & VT_ARRAY)
	{
		SAFEARRAY * psa, * psaOut;

		if (vt & VT_BYREF)
			psa = *(pVariantA->pparray);
		else
			psa = pVariantA->parray;

		vt &= ~(VT_ARRAY|VT_BYREF);

		switch (vt)
		{
		case VT_BSTR:
		case VT_VARIANT:
		case VT_DISPATCH:
		case VT_UNKNOWN:
			BYTE * pvData, * pvDataOut;
			ULONG cElems, cDims;

			// iterate over the elements, translating and copying to new array.
			hResult = SafeArrayAllocDescriptor( psa->cDims, &psaOut );
			if( FAILED(hResult) )
				return hResult;

			psaOut->cDims = psa->cDims;
			psaOut->fFeatures = psa->fFeatures;
			psaOut->cbElements = psa->cbElements;
			for (cDims=0; cDims < psa->cDims; cDims++)
				psaOut->rgsabound[ cDims ] = psa->rgsabound[ cDims ];
			hResult = SafeArrayAllocData( psaOut );
			if( FAILED( hResult ) )
			{
				SafeArrayDestroyDescriptor( psaOut );
				return hResult;
			}

			SafeArrayLockA(psa);

			// point to first element;
			SafeArrayAccessDataA(psa, (VOID **)&pvData);
			SafeArrayAccessData(psaOut, (VOID **)&pvDataOut);

			for (cElems = SafeArrayElems(psa); cElems > 0; cElems--)
			{
				switch (vt)
				{
				case VT_BSTR:
					hResult = ConvertDispStringToW(*(LPBSTRA)pvData,
						(LPBSTR)pvDataOut);
					pvData += sizeof(BSTRA);
					pvDataOut += sizeof(BSTR);
					break;

				case VT_VARIANT:
					hResult = Ole2AnsiVariantWFromA((LPVARIANTA)pvData,
								 (LPVARIANT)pvDataOut);
					pvData += sizeof(VARIANTA);
					pvDataOut += sizeof(VARIANT);
					break;

				case VT_DISPATCH:
					pDispatchA = *(LPDISPATCHA *)pvData;
					hResult = WrapIDispatchWFromA(pDispatchA, &pDispatch);
					if (!FAILED(hResult))
						*(LPDISPATCH *)pvDataOut = pDispatch;
					pvData += sizeof(LPDISPATCHA);
					pvDataOut += sizeof(LPDISPATCH);
					break;

				case VT_UNKNOWN:
					pUnknownA = *(LPUNKNOWN *)pvData;
					hResult = WrapIUnknownWFromA(pUnknownA, &pUnknown);
					if (!FAILED(hResult))
						*(LPUNKNOWN *)pvDataOut = pUnknown;
					pvData += sizeof(LPUNKNOWN);
					pvDataOut += sizeof(LPUNKNOWN);
					break;
				}
				if (hResult != NOERROR)
					break;
			}

			SafeArrayUnaccessData(psa);
			SafeArrayUnaccessDataA(psaOut);
			pVariant->parray = psaOut;
			SafeArrayUnlockA(psa);
			break;

		default:
			hResult = SafeArrayCopy( (SAFEARRAY *)psa, &pVariant->parray );
			break;  // nothing to do
		}
		pVariant->vt = VT_ARRAY & vt;

	}
	else
	{   // not an array
		switch (vt)
		{
		case VT_BSTR:
			hResult = ConvertDispStringToW(pVariantA->bstrVal, &pVariant->bstrVal);
			break;

		case VT_BYREF|VT_BSTR:
			hResult = ConvertDispStringToW(*pVariantA->pbstrVal, &pVariant->bstrVal);
			V_VT(pVariant) = VT_BSTR;       // no longer byref
			break;

		case VT_BYREF|VT_VARIANT:
			hResult = Ole2AnsiVariantWFromA(pVariantA->pvarVal, pVariant->pvarVal);
			V_VT(pVariant) = VT_VARIANT;        // no longer byref
			break;

		case VT_DISPATCH:
			pDispatchA = pVariantA->pdispVal;
			hResult = WrapIDispatchWFromA(pDispatchA, &pDispatch);
			if (!FAILED(hResult))
				pVariant->pdispVal = pDispatch;
			break;

		case VT_BYREF|VT_DISPATCH:
			pDispatchA = *pVariantA->ppdispVal;
			hResult = WrapIDispatchWFromA(pDispatchA, &pDispatch);
			if (!FAILED(hResult))
			{
				V_VT(pVariant) = VT_DISPATCH;       // no longer byref
				pVariant->pdispVal = pDispatch;
			}
			break;

		case VT_UNKNOWN:
			pUnknownA = pVariantA->punkVal;
			hResult = WrapIUnknownWFromA(pUnknownA, &pUnknown);
			if (!FAILED(hResult))
				pVariant->punkVal = pUnknown;
			break;

		case VT_BYREF|VT_UNKNOWN:
			pUnknownA = *pVariantA->ppunkVal;
			hResult = WrapIUnknownWFromA(pUnknownA, &pUnknown);
			if (!FAILED(hResult))
			{
				V_VT(pVariant) = VT_UNKNOWN;       // no longer byref
				pVariant->punkVal = pUnknown;
			}
			break;

		default:
			VariantCopy( pVariant, (LPVARIANT)pVariantA );
			break;
		}
		pVariant->vt = vt;
	}
	return hResult;
}

STDAPI Ole2AnsiDispAFromW(LPDISPATCH pobj, LPDISPATCHA * ppobjA)
{
	return WrapIDispatchAFromW( pobj, ppobjA );
}


STDAPI Ole2AnsiDispWFromA(LPDISPATCHA pobjA, LPDISPATCH * ppobj)
{
	return WrapIDispatchWFromA( pobjA, ppobj );
}
