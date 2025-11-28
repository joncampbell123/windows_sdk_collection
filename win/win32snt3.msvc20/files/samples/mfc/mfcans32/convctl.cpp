//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ConvCtl.cpp
//
//  Contents:   ANSI Wrappers for OLE Control Interfaces and APIs.
//
//  History:    01-Jun-94   johnels     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"


HRESULT ConvertPROPPAGEINFOToA(LPPROPPAGEINFO pPageInfo,
	LPPROPPAGEINFOA pPageInfoA)
{
	HRESULT hResult;

	memcpy((LPVOID)pPageInfoA, (LPVOID)pPageInfo, sizeof(PROPPAGEINFO));

	hResult = ConvertStringToA(pPageInfo->pszTitle, &pPageInfoA->pszTitle);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertStringToA(pPageInfo->pszDocString, &pPageInfoA->pszDocString);
	if (FAILED(hResult))
		goto Error1;

	hResult = ConvertStringToA(pPageInfo->pszHelpFile, &pPageInfoA->pszHelpFile);
	if (FAILED(hResult))
		goto Error2;

	return hResult;

Error2:
	ConvertStringFree(pPageInfoA->pszDocString);
	pPageInfoA->pszDocString = NULL;

Error1:
	ConvertStringFree(pPageInfoA->pszTitle);
	pPageInfoA->pszTitle = NULL;

	return hResult;
}

HRESULT ConvertPROPPAGEINFOToW(LPPROPPAGEINFOA pPageInfoA,
	LPPROPPAGEINFO pPageInfo)
{
	HRESULT hResult;

	memcpy((LPVOID)pPageInfo, (LPVOID)pPageInfoA, sizeof(PROPPAGEINFO));

	hResult = ConvertStringToW(pPageInfoA->pszTitle, &pPageInfo->pszTitle);
	if (FAILED(hResult))
		return hResult;

	hResult = ConvertStringToW(pPageInfoA->pszDocString, &pPageInfo->pszDocString);
	if (FAILED(hResult))
		goto Error1;

	hResult = ConvertStringToW(pPageInfoA->pszHelpFile, &pPageInfo->pszHelpFile);
	if (FAILED(hResult))
		goto Error2;

	return hResult;

Error2:
	ConvertStringFree(pPageInfo->pszDocString);
	pPageInfo->pszDocString = NULL;

Error1:
	ConvertStringFree(pPageInfo->pszTitle);
	pPageInfo->pszTitle = NULL;

	return hResult;
}

void ConvertPROPPAGEINFOFree(LPPROPPAGEINFO pPageInfo)
{
	ConvertStringFree(pPageInfo->pszTitle);
	ConvertStringFree(pPageInfo->pszDocString);
	ConvertStringFree(pPageInfo->pszHelpFile);
}

void ConvertPROPPAGEINFOFree(LPPROPPAGEINFOA pPageInfoA)
{
	ConvertStringFree(pPageInfoA->pszTitle);
	ConvertStringFree(pPageInfoA->pszDocString);
	ConvertStringFree(pPageInfoA->pszHelpFile);
}
