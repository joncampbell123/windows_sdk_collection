//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       convert.cpp
//
//  Contents:   ANSI/UNICODE Internal convertion routines.
//
//  Functions:  ConvertStringToW
//              ConvertStringToA
//              ConvertStringArrayToA
//              ConvertStringArrayToW
//              ConvertStringAlloc
//              ConvertStringFree
//              ConvertStringArrayFree
//              ConvertSTATSTGToA
//              ConvertSTATSTGToW
//              ConvertSTATSTGArrayToA
//              ConvertSTATSTGArrayToW
//              ConvertSTATDATAToA
//              ConvertSTATDATAToW
//              ConvertSTATDATAArrayToA
//              ConvertSTATDATAArrayToW
//              ConvertOLEVERBToA
//              ConvertOLEVERBToW
//              ConvertOLEVERBArrayToA
//              ConvertOLEVERBArrayToW
//              ConvertSTGMEDIUMToA
//              ConvertSTGMEDIUMToW
//              ConvertSTGMEDIUMFree
//              ConvertMonikerArrayToA
//              ConvertMonikerArrayToW
//              ConvertSNBToA
//              ConvertSNBToW
//              ConvertSNBFree
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#include "Ole2Ansi.h"

//
//  External Routines.
//
typedef struct
{
	UINT cbConvertSize;
	UINT cchDrvName;
	UINT cchDevName;
	UINT cchPortName;
} DVTDINFO, *PDVTDINFO;

extern "C" HRESULT UtGetDvtd16Info(DVTARGETDEVICEA const *pdvtd16,
								   PDVTDINFO pdvtdInfo);
extern "C" HRESULT UtConvertDvtd16toDvtd32(DVTARGETDEVICEA const *pdvtd16,
										   DVTDINFO const *pdvtdInfo,
										   DVTARGETDEVICE *pdvtd32);
extern "C" HRESULT UtGetDvtd32Info(DVTARGETDEVICE const *pdvtd32,
								   PDVTDINFO pdvtdInfo);
extern "C" HRESULT UtConvertDvtd32toDvtd16(DVTARGETDEVICE const *pdvtd32,
										   DVTDINFO const *pdvtdInfo,
										   DVTARGETDEVICEA *pdvtd16);

//
//  Define local SizeOf functions.
//
ULONG SizeOfObjectDescriptorAsANSI(HGLOBAL hGlobal);
ULONG SizeOfObjectDescriptorAsWide(HGLOBAL hGlobal);

// cheap atoi -- define here to avoid use of C-runtime version
int __cdecl atoi(const char * psz)
{
	register int i = 0;
	for (; *psz >= '0' && *psz <= '9'; ++psz)
		i = (i * 10) + (*psz - '0');
	return i;
}

//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringToW(LPWSTR * ppStrW)
{
	ULONG Count;
	HRESULT hResult;


	// If input is null then just return.
	if (*ppStrW == NULL)
		return ResultFromScode(S_OK);

	LPSTR pStrA = (LPSTR)*ppStrW;

	Count = lstrlen(pStrA) + 1;

	hResult = ConvertStringAlloc(Count * sizeof(WCHAR), (LPVOID *)ppStrW);
	if (FAILED(hResult))
		return (hResult);

	MultiByteToWideChar(CP_ACP, 0, pStrA, Count, *ppStrW, Count);

	ConvertStringFree(pStrA);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertStringToW(LPCSTR pStrA, LPWSTR pStrW)
{
	ULONG Count;


	Assert(pStrA != NULL);

	Count = lstrlen(pStrA) + 1;

	MultiByteToWideChar(CP_ACP, 0, pStrA, Count, pStrW, Count);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringToW(LPCSTR pStrA, LPWSTR * ppStrW)
{
	ULONG Count;
	HRESULT hResult;


	// If input is null then just return the same.
	if (pStrA == NULL)
	{
		*ppStrW = NULL;
		return ResultFromScode(S_OK);
	}

	Count = lstrlen(pStrA) + 1;

	hResult = ConvertStringAlloc(Count * sizeof(WCHAR), (LPVOID *)ppStrW);
	if (FAILED(hResult))
		return (hResult);

	MultiByteToWideChar(CP_ACP, 0, pStrA, Count, *ppStrW, Count);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringToW(LPCSTR pStrA, LCID lcid, LPWSTR * ppStrW)
{
	static LCID lcidCache = (LCID)~0;
	static UINT cpCache;
	ULONG Count;
		char  szCodePage[16];           // string of LCID code page number.
	UINT  cpLCID;               // Integer value of LCID code page.
	HRESULT hResult;


	// If input is null then just return the same.
	if (pStrA == NULL)
	{
		*ppStrW = NULL;
		return ResultFromScode(S_OK);
	}

	Count = lstrlen(pStrA) + 1;

	hResult = ConvertStringAlloc(Count * sizeof(WCHAR), (LPVOID *)ppStrW);
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

	MultiByteToWideChar(cpLCID, 0, pStrA, Count, *ppStrW, Count);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringToA(LPSTR * ppStrA)
{
	// If input is null then just return.
	if (*ppStrA == NULL)
		return ResultFromScode(S_OK);

	LPWSTR pStrW = (LPWSTR)*ppStrA;

	HRESULT hResult = ConvertStringToA(pStrW, ppStrA);
	if (FAILED(hResult))
		return (hResult);

	ConvertStringFree(pStrW);

	return (NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertStringToA(LPCWSTR pStrW, LPSTR pStrA)
{
	ULONG Count;


	Assert(pStrW != NULL);

	Count = wcslen(pStrW) + 1;

	WideCharToMultiByte(CP_ACP, 0, pStrW, Count, pStrA, Count, NULL, NULL);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringToA(LPCWSTR pStrW, LPSTR * ppStrA)
{
	ULONG Count;
	HRESULT hResult;


	// If input is null then just return the same.
	if (pStrW == NULL)
	{
		*ppStrA = NULL;
		return ResultFromScode(S_OK);
	}

	Count = wcslen(pStrW) + 1;

	hResult = ConvertStringAlloc(Count, (LPVOID *)ppStrA);
	if (FAILED(hResult))
		return (hResult);

	WideCharToMultiByte(CP_ACP, 0, pStrW, Count, *ppStrA, Count, NULL, NULL);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringArrayToA(LPSTR * ppstr, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertStringToA(ppstr);
		if (FAILED(hResult))
			return (hResult);

		ppstr++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringArrayToW(LPWSTR * ppstr, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertStringToW(ppstr);
		if (FAILED(hResult))
			return (hResult);

		ppstr++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringArrayToA(LPWSTR * ppstr, LPSTR * * pppstrA, ULONG ulSize)
{
	ULONG i;
	HRESULT hResult;


	hResult = NOERROR;

	*pppstrA = new LPSTR [ulSize];

	for (i = 0; i < ulSize; i++)
		(*pppstrA)[i] = NULL;

	for (i = 0; i < ulSize; i++)
	{
		hResult = ConvertStringToA(ppstr[i], &((*pppstrA)[i]));
		if (FAILED(hResult))
		{
			ConvertStringArrayFree(*pppstrA, ulSize);
			break;
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringArrayToW(LPSTR * ppstrA, LPWSTR * * pppstr, ULONG ulSize)
{
	ULONG i;
	HRESULT hResult;


	hResult = NOERROR;

	*pppstr = new LPWSTR [ulSize];

	for (i = 0; i < ulSize; i++)
		(*pppstr)[i] = NULL;

	for (i = 0; i < ulSize; i++)
	{
		hResult = ConvertStringToW(ppstrA[i], &((*pppstr)[i]));
		if (FAILED(hResult))
		{
			ConvertStringArrayFree(*pppstr, ulSize);
			break;
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringArrayToW(LPSTR * ppstrA, LCID lcid, LPWSTR * * pppstr, ULONG ulSize)
{
	ULONG i;
	HRESULT hResult;


	hResult = NOERROR;

	*pppstr = new LPWSTR [ulSize];

	for (i = 0; i < ulSize; i++)
		(*pppstr)[i] = NULL;

	for (i = 0; i < ulSize; i++)
	{
		hResult = ConvertStringToW(ppstrA[i], lcid, &((*pppstr)[i]));
		if (FAILED(hResult))
		{
			ConvertStringArrayFree(*pppstr, ulSize);
			break;
		}
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringAlloc
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertStringAlloc(ULONG ulSize, LPVOID * pptr)
{
	*pptr = new char [ulSize];

	if (*pptr == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertStringFree(LPVOID ptr)
{
	delete ptr;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertStringFree(LPSTR ptr)
{
	ConvertStringFree((LPVOID)ptr);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertStringFree(LPWSTR ptr)
{
	ConvertStringFree((LPVOID)ptr);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringArrayFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertStringArrayFree(LPSTR * ptr, ULONG ulSize)
{
	while (ulSize)
		ConvertStringFree(ptr[--ulSize]);

	delete [] ptr;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertStringArrayFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
VOID ConvertStringArrayFree(LPWSTR * ptr, ULONG ulSize)
{
	while (ulSize)
		ConvertStringFree(ptr[--ulSize]);

	delete [] ptr;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATSTGToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATSTGToA(STATSTGA * pstatstgA)
{
	return ConvertStringToA(&pstatstgA->pwcsName);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATSTGToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATSTGToW(STATSTG * pstatstg)
{
	return ConvertStringToW(&pstatstg->pwcsName);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATSTGArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATSTGArrayToA(STATSTGA * pstatstgA, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertSTATSTGToA(pstatstgA);
		if (FAILED(hResult))
			return (hResult);

		pstatstgA++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATSTGArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATSTGArrayToW(STATSTG * pstatstg, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertSTATSTGToW(pstatstg);
		if (FAILED(hResult))
			return (hResult);

		pstatstg++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATDATAToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATDATAToA(STATDATAA * pSTATDATAA)
{
	LPADVISESINK pAdvSink;
	HRESULT hResult;


	pAdvSink = ((STATDATA *)pSTATDATAA)->pAdvSink;

	hResult = WrapIAdviseSinkAFromW(pAdvSink, &pSTATDATAA->pAdvSink);

	if (pAdvSink)
		pAdvSink->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATDATAToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATDATAToW(STATDATA * pSTATDATA)
{
	LPADVISESINKA pAdvSinkA;
	HRESULT hResult;


	pAdvSinkA = ((STATDATAA *)pSTATDATA)->pAdvSink;

	hResult = WrapIAdviseSinkWFromA(pAdvSinkA, &pSTATDATA->pAdvSink);

	if (pAdvSinkA)
		pAdvSinkA->Release();

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATDATAArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATDATAArrayToA(STATDATAA * pSTATDATAA, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertSTATDATAToA(pSTATDATAA);
		if (FAILED(hResult))
			return (hResult);

		pSTATDATAA++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTATDATAArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTATDATAArrayToW(STATDATA * pSTATDATA, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertSTATDATAToW(pSTATDATA);
		if (FAILED(hResult))
			return (hResult);

		pSTATDATA++;
	}

	return ResultFromScode(NOERROR);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertFORMATETCToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertFORMATETCToA(FORMATETC * pFormatEtc, FORMATETCA * pFormatEtcA)
{
	memcpy(pFormatEtcA, pFormatEtc, sizeof(FORMATETC));
	return ConvertDVTARGETDEVICEToA(pFormatEtc->ptd, &pFormatEtcA->ptd);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertFORMATETCToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertFORMATETCToW(FORMATETCA * pFormatEtcA, FORMATETC * pFormatEtc)
{
	memcpy(pFormatEtc, pFormatEtcA, sizeof(FORMATETC));
	return ConvertDVTARGETDEVICEToW(pFormatEtcA->ptd, &pFormatEtc->ptd);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertFORMATETCArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertFORMATETCArrayToA(FORMATETCA * pFormatEtcA, ULONG ulLength)
{
	HRESULT hResult;

	while (ulLength--)
	{
		hResult = ConvertDVTARGETDEVICEToA(&pFormatEtcA->ptd);
		if (FAILED(hResult))
			return hResult;

		pFormatEtcA++;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertFORMATETCArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertFORMATETCArrayToW(FORMATETC * pFormatEtc, ULONG ulLength)
{
	HRESULT hResult;

	while (ulLength--)
	{
		hResult = ConvertDVTARGETDEVICEToW(&pFormatEtc->ptd);
		if (FAILED(hResult))
			return hResult;

		pFormatEtc++;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertFORMATETCFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
void ConvertFORMATETCFree(FORMATETC * pFormatEtc)
{
	ConvertDVTARGETDEVICEFree(pFormatEtc->ptd);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertFORMATETCFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
void ConvertFORMATETCFree(FORMATETCA * pFormatEtcA)
{
	ConvertDVTARGETDEVICEFree(pFormatEtcA->ptd);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDVTARGETDEVICEToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDVTARGETDEVICEToA(DVTARGETDEVICEA * * ppDeviceA)
{
	DVTARGETDEVICE * pDevice;

	if (*ppDeviceA == NULL)
		return ResultFromScode(S_OK);

	pDevice = (DVTARGETDEVICE *)*ppDeviceA;

	ConvertDVTARGETDEVICEToA(pDevice, ppDeviceA);

	ConvertDVTARGETDEVICEFree(pDevice);

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDVTARGETDEVICEToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDVTARGETDEVICEToA(DVTARGETDEVICE * pDevice,
		DVTARGETDEVICEA * * ppDeviceA)
{
	DVTDINFO Info;
	HRESULT hResult;


	if (pDevice == NULL)
	{
		*ppDeviceA = NULL;
		return ResultFromScode(S_OK);
	}

	hResult = UtGetDvtd32Info(pDevice, &Info);
	if (FAILED(hResult))
		return hResult;

	*ppDeviceA = (DVTARGETDEVICEA *) new char [Info.cbConvertSize];

	hResult = UtConvertDvtd32toDvtd16(pDevice, &Info, *ppDeviceA);
	if (FAILED(hResult))
	{
		delete *ppDeviceA;
		*ppDeviceA = NULL;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDVTARGETDEVICEToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDVTARGETDEVICEToW(DVTARGETDEVICE * * ppDevice)
{
	DVTARGETDEVICEA * pDeviceA;


	if (*ppDevice == NULL)
		return ResultFromScode(S_OK);

	pDeviceA = (DVTARGETDEVICEA *)*ppDevice;

	ConvertDVTARGETDEVICEToW(pDeviceA, ppDevice);

	ConvertDVTARGETDEVICEFree(pDeviceA);

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDVTARGETDEVICEToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertDVTARGETDEVICEToW(DVTARGETDEVICEA * pDeviceA,
		DVTARGETDEVICE * * ppDevice)
{
	DVTDINFO Info;
	HRESULT hResult;


	if (pDeviceA == NULL)
	{
		*ppDevice = NULL;
		return ResultFromScode(S_OK);
	}

	hResult = UtGetDvtd16Info(pDeviceA, &Info);
	if (FAILED(hResult))
		return hResult;

	*ppDevice = (DVTARGETDEVICE *) new char [Info.cbConvertSize];

	hResult = UtConvertDvtd16toDvtd32(pDeviceA, &Info, *ppDevice);
	if (FAILED(hResult))
	{
		delete *ppDevice;
		*ppDevice = NULL;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDVTARGETDEVICEFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
void ConvertDVTARGETDEVICEFree(LPDVTARGETDEVICE pDevice)
{
	delete [] pDevice;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertDVTARGETDEVICEFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
void ConvertDVTARGETDEVICEFree(LPDVTARGETDEVICEA pDevice)
{
	delete [] pDevice;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertOLEVERBToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertOLEVERBToA(OLEVERBA * pOLEVERBA)
{
	return ConvertStringToA(&pOLEVERBA->lpszVerbName);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertOLEVERBToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertOLEVERBToW(OLEVERB * pOLEVERB)
{
	return ConvertStringToW(&pOLEVERB->lpszVerbName);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertOLEVERBArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertOLEVERBArrayToA(OLEVERBA * pOLEVERBA, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertOLEVERBToA(pOLEVERBA);
		if (FAILED(hResult))
			return (hResult);

		pOLEVERBA++;
	}

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertOLEVERBArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertOLEVERBArrayToW(OLEVERB * pOLEVERB, ULONG ulSize)
{
	for (ULONG i = 0; i < ulSize; i++)
	{
		HRESULT hResult = ConvertOLEVERBToW(pOLEVERB);
		if (FAILED(hResult))
			return (hResult);

		pOLEVERB++;
	}

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTGMEDIUMToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTGMEDIUMToA(CLIPFORMAT cfFormat, LPSTGMEDIUMA pmediumA)
{
	HRESULT hResult;
	LPSTREAMA  pstmA;
	LPSTORAGEA pstgA;


	hResult = S_OK;

	switch (pmediumA->tymed)
	{
	case TYMED_HGLOBAL:
		if (OBJDESC_CF(cfFormat))
			hResult = ConvertObjectDescriptorToA(&pmediumA->hGlobal);
		break;

	case TYMED_FILE:
		hResult = ConvertStringToA(&pmediumA->lpszFileName);
		break;

	case TYMED_ISTREAM:
		hResult = WrapIStreamAFromW(((LPSTGMEDIUM)pmediumA)->pstm, &pstmA);
		if (FAILED(hResult))
			return hResult;

		pmediumA->pstm->Release();
		pmediumA->pstm = pstmA;
		break;

	case TYMED_ISTORAGE:
		hResult = WrapIStorageAFromW(((LPSTGMEDIUM)pmediumA)->pstg, &pstgA);
		if (FAILED(hResult))
			return hResult;

		pmediumA->pstg->Release();
		pmediumA->pstg = pstgA;
		break;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTGMEDIUMToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTGMEDIUMToA(CLIPFORMAT cfFormat, LPSTGMEDIUM pmedium,
		LPSTGMEDIUMA pmediumA)
{
	HRESULT hResult;


	hResult = S_OK;

	memcpy((LPVOID)pmediumA, (LPVOID)pmedium, sizeof(STGMEDIUM));

	switch (pmedium->tymed)
	{
	case TYMED_HGLOBAL:
		if (OBJDESC_CF(cfFormat))
			hResult = ConvertObjectDescriptorToA(pmedium->hGlobal, &pmediumA->hGlobal);
		break;

	case TYMED_FILE:
		hResult = ConvertStringToA(pmedium->lpszFileName, &pmediumA->lpszFileName);
		break;

	case TYMED_ISTREAM:
		hResult = WrapIStreamAFromW(pmedium->pstm, &pmediumA->pstm);
		if (FAILED(hResult))
			return hResult;
		break;

	case TYMED_ISTORAGE:
		hResult = WrapIStorageAFromW(pmedium->pstg, &pmediumA->pstg);
		if (FAILED(hResult))
			return hResult;
		break;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTGMEDIUMToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTGMEDIUMToW(CLIPFORMAT cfFormat, LPSTGMEDIUM pmedium)
{
	HRESULT hResult;
	LPSTREAM  pstm;
	LPSTORAGE pstg;


	hResult = S_OK;

	switch (pmedium->tymed)
	{
	case TYMED_HGLOBAL:
		if (OBJDESC_CF(cfFormat))
			hResult = ConvertObjectDescriptorToW(&pmedium->hGlobal);
		break;

	case TYMED_FILE:
		hResult = ConvertStringToW(&pmedium->lpszFileName);
		break;

	case TYMED_ISTREAM:
		hResult = WrapIStreamWFromA(((LPSTGMEDIUMA)pmedium)->pstm, &pstm);
		if (FAILED(hResult))
			return hResult;

		pmedium->pstm->Release();
		pmedium->pstm = pstm;
		break;

	case TYMED_ISTORAGE:
		hResult = WrapIStorageWFromA(((LPSTGMEDIUMA)pmedium)->pstg, &pstg);
		if (FAILED(hResult))
			return hResult;

		pmedium->pstg->Release();
		pmedium->pstg = pstg;
		break;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTGMEDIUMToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTGMEDIUMToW(CLIPFORMAT cfFormat, LPSTGMEDIUMA pmediumA,
		LPSTGMEDIUM pmedium)
{
	HRESULT hResult;


	hResult = S_OK;

	memcpy((LPVOID)pmedium, (LPVOID)pmediumA, sizeof(STGMEDIUM));

	switch (pmediumA->tymed)
	{
	case TYMED_HGLOBAL:
		if (OBJDESC_CF(cfFormat))
			hResult = ConvertObjectDescriptorToW(pmediumA->hGlobal, &pmedium->hGlobal);
		break;

	case TYMED_FILE:
		hResult = ConvertStringToW(pmediumA->lpszFileName, &pmedium->lpszFileName);
		break;

	case TYMED_ISTREAM:
		hResult = WrapIStreamWFromA(pmediumA->pstm, &pmedium->pstm);
		if (FAILED(hResult))
			return hResult;
		break;

	case TYMED_ISTORAGE:
		hResult = WrapIStorageWFromA(pmediumA->pstg, &pmedium->pstg);
		if (FAILED(hResult))
			return hResult;
		break;
	}

	return hResult;
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTGMEDIUMFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTGMEDIUMFree(CLIPFORMAT cfFormat, LPSTGMEDIUM pmedium)
{
	switch (pmedium->tymed)
	{
	case TYMED_HGLOBAL:
		if (OBJDESC_CF(cfFormat))
			GlobalFree(pmedium->hGlobal);
		break;

	case TYMED_FILE:
			ConvertStringFree(pmedium->lpszFileName);
			break;

	case TYMED_ISTREAM:
			pmedium->pstm->Release();
			break;

	case TYMED_ISTORAGE:
			pmedium->pstg->Release();
			break;
	}

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSTGMEDIUMFree
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSTGMEDIUMFree(CLIPFORMAT cfFormat, LPSTGMEDIUMA pmediumA)
{
	switch (pmediumA->tymed)
	{
	case TYMED_HGLOBAL:
		if (OBJDESC_CF(cfFormat))
			GlobalFree(pmediumA->hGlobal);
		break;

	case TYMED_FILE:
			ConvertStringFree(pmediumA->lpszFileName);
			break;

	case TYMED_ISTREAM:
			pmediumA->pstm->Release();
			break;

	case TYMED_ISTORAGE:
			pmediumA->pstg->Release();
			break;
	}

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertMonikerArrayToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertMonikerArrayToA(LPMONIKERA * ppmkA, ULONG ulSize)
{
	LPMONIKER pmk;
	HRESULT hResult;


	for (ULONG i = 0; i < ulSize; i++)
	{
		pmk = (LPMONIKER)(*ppmkA);

		hResult = WrapIMonikerAFromW(pmk, ppmkA);
		if (FAILED(hResult))
			return (hResult);

		pmk->Release();

		ppmkA++;
	}

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertMonikerArrayToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertMonikerArrayToW(LPMONIKER * ppmk, ULONG ulSize)
{
	LPMONIKERA pmkA;
	HRESULT hResult;


	for (ULONG i = 0; i < ulSize; i++)
	{
		pmkA = (LPMONIKERA)(*ppmk);

		hResult = WrapIMonikerWFromA(pmkA, ppmk);
		if (FAILED(hResult))
			return (hResult);

		pmkA->Release();

		ppmk++;
	}

	return ResultFromScode(S_OK);
}



//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSNBToA
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSNBToA(SNB snb, SNBA * psnba)
{
	SNB     pSNB;
	UINT    cSNB;
	UINT    iSNB;
	HRESULT hResult;


	if (snb == NULL)
	{
		*psnba = NULL;
		return ResultFromScode(S_OK);
	}

	//
	//  Count the number of elements in the String Name Block.
	//
	pSNB = snb;
	cSNB = 0;
	while (*pSNB++)
		cSNB++;

	//
	//
	//
	*psnba = new LPSTR [cSNB + 1];

	for (iSNB = 0; iSNB < cSNB; iSNB++, snb++);
	{
		hResult = ConvertStringToA(*snb, &(*psnba)[iSNB]);
	}
	(*psnba)[iSNB] = NULL;

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSNBToW
//
//  Synopsis:
//
//  Returns:    HRESULT Error code.
//
//---------------------------------------------------------------------------
HRESULT ConvertSNBToW(SNBA snba, SNB *psnb)
{
	SNBA    pSNB;
	UINT    cSNB;
	UINT    iSNB;
	HRESULT hResult;


	if (snba == NULL)
	{
		*psnb = NULL;
		return ResultFromScode(S_OK);
	}

	//
	//  Count the number of elements in the String Name Block.
	//
	pSNB = snba;
	cSNB = 0;
	while (*pSNB++)
		cSNB++;

	//
	//
	//
	*psnb = new LPWSTR [cSNB + 1];

	for (iSNB = 0; iSNB < cSNB; iSNB++, snba++);
	{
		hResult = ConvertStringToW(*snba, &(*psnb)[iSNB]);
	}
	(*psnb)[iSNB] = NULL;

	return ResultFromScode(S_OK);
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertSNBFree
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
VOID ConvertSNBFree(SNBA snba)
{
	UINT iSNB;


	if (snba != NULL)
	{
		for (iSNB = 0; snba[iSNB]; iSNB++)
			ConvertStringFree(snba[iSNB]);

		delete [] snba;
	}
}


//+--------------------------------------------------------------------------
//
//  Routine:    ConvertMonikerArrayToW
//
//  Synopsis:
//
//  Returns:    None.
//
//---------------------------------------------------------------------------
VOID ConvertSNBFree(SNB snb)
{
	UINT iSNB;

	if (snb != NULL)
	{
		for (iSNB = 0; snb[iSNB]; iSNB++)
			ConvertStringFree(snb[iSNB]);

		delete [] snb;
	}
}


//+-------------------------------------------------------------------------
//
//  Function:   UtGetDvtd16Info
//
//  Synopsis:   Fills in pdvdtInfo
//
//  Arguments:  [pdvtd16] -- pointer to ANSI DVTARGETDEVICE
//              [pdvtdInfo] -- pointer to DVDT_INFO block
//
//  Requires:
//
//  Returns:    HRESULT
//
//  Modifies:   pdvtdInfo
//
//  Algorithm:
//
//  History:    06-May-94 XXXXX     Created from XXXXXXX original functions
//
//  Notes:      Do we need to do any error checking on the strings?
//
//--------------------------------------------------------------------------

//  We can't use sizeof(DV_TARGETDEVICE) because MIDL keeps flipping back
//  and forth over whether to make the embedded array size 0 or size 1

#define UT_DVTARGETDEVICE_SIZE  (sizeof(DWORD) + sizeof(WORD) * 4)

extern "C" HRESULT UtGetDvtd16Info(DVTARGETDEVICEA const *pdvtd16,
								   PDVTDINFO pdvtdInfo)
{

	DEVMODEA *pdm16;

	//  We need at least a DVTARGETDEVICE
	pdvtdInfo->cbConvertSize = UT_DVTARGETDEVICE_SIZE;

	//  Compute required size for Drv, Device, Port names
	if (pdvtd16->tdDriverNameOffset != 0)
	{
		pdvtdInfo->cchDrvName = lstrlen((char *)pdvtd16 +
									   pdvtd16->tdDriverNameOffset) + 1;

		pdvtdInfo->cbConvertSize += pdvtdInfo->cchDrvName * sizeof(WCHAR);
	}
	else
	{
		pdvtdInfo->cchDrvName = 0;
	}

	if (pdvtd16->tdDeviceNameOffset != 0)
	{
		pdvtdInfo->cchDevName = lstrlen((char *)pdvtd16 +
									   pdvtd16->tdDeviceNameOffset) + 1;

		pdvtdInfo->cbConvertSize += pdvtdInfo->cchDevName * sizeof(WCHAR);
	}
	else
	{
		pdvtdInfo->cchDevName = 0;
	}

	if (pdvtd16->tdPortNameOffset != 0)
	{
		pdvtdInfo->cchPortName = lstrlen((char *)pdvtd16 +
										pdvtd16->tdPortNameOffset) + 1;

		pdvtdInfo->cbConvertSize += pdvtdInfo->cchPortName * sizeof(WCHAR);
	}
	else
	{
		pdvtdInfo->cchPortName = 0;
	}

	if (pdvtd16->tdExtDevmodeOffset != 0)
	{
		//  Now compute the space needed for the DEVMODE
		pdm16 = (DEVMODEA *)((BYTE *)pdvtd16 + pdvtd16->tdExtDevmodeOffset);

		//  We start with a basic DEVMODEW
		pdvtdInfo->cbConvertSize += sizeof(DEVMODEW);

		if (pdm16->dmSize > sizeof(DEVMODEA))
		{
			//  The input DEVMODEA is larger than a standard DEVMODEA, so
			//  add space for the extra amount
			pdvtdInfo->cbConvertSize += pdm16->dmSize - sizeof(DEVMODEA);
		}

		//  Finally we account for the extra driver data
		pdvtdInfo->cbConvertSize += pdm16->dmDriverExtra;
	}

	return(S_OK);
}

//+-------------------------------------------------------------------------
//
//  Function:   UtConvertDvtd16toDvtd32
//
//  Synopsis:   Fills in a 32-bit DVTARGETDEVICE based on a 16-bit
//              DVTARGETDEVICE
//
//  Arguments:  [pdvtd16] -- pointer to ANSI DVTARGETDEVICE
//              [pdvtdInfo] -- pointer to DVDT_INFO block
//              [pdvtd32] -- pointer to UNICODE DVTARGETDEVICE
//
//  Requires:   pdvtdInfo must have been filled in by a previous call to
//              UtGetDvtd16Info
//
//              pdvtd32 must be at least pdvtdInfo->cbConvertSize bytes long
//
//  Returns:    HRESULT
//
//  Modifies:   pdvtd32
//
//  Algorithm:
//
//  History:    06-May-94 XXXXX     Created from XXXXXXX original functions
//
//  Notes:      Do we need to do any error checking on the strings?
//
//--------------------------------------------------------------------------

extern "C" HRESULT UtConvertDvtd16toDvtd32(DVTARGETDEVICEA const *pdvtd16,
										   DVTDINFO const *pdvtdInfo,
										   DVTARGETDEVICE *pdvtd32)
{
	HRESULT hr = S_OK;
	USHORT cbOffset;
	int cchWritten;
	DEVMODEA *pdm16;
	DEVMODEW *pdm32;

	memset(pdvtd32, 0, pdvtdInfo->cbConvertSize);

	cbOffset = UT_DVTARGETDEVICE_SIZE;

	if (pdvtdInfo->cchDrvName != 0)
	{
		pdvtd32->tdDriverNameOffset = cbOffset;
		cchWritten = MultiByteToWideChar(
						CP_ACP, 0,
						(char *)pdvtd16+pdvtd16->tdDriverNameOffset,
						pdvtdInfo->cchDrvName,
						(LPOLESTR)((BYTE *)pdvtd32 +
							pdvtd32->tdDriverNameOffset),
						pdvtdInfo->cchDrvName);
		if (0 == cchWritten)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}
		cbOffset += cchWritten * sizeof(WCHAR);
	}

	if (pdvtdInfo->cchDevName != 0)
	{
		pdvtd32->tdDeviceNameOffset = cbOffset;
		cchWritten = MultiByteToWideChar(
						CP_ACP, 0,
						(char *)pdvtd16 + pdvtd16->tdDeviceNameOffset,
						pdvtdInfo->cchDevName,
						(LPOLESTR)((BYTE *)pdvtd32 +
							pdvtd32->tdDeviceNameOffset),
						pdvtdInfo->cchDevName);

		if (0 == cchWritten)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}
		cbOffset += cchWritten * sizeof(WCHAR);
	}

	if (pdvtdInfo->cchPortName != 0)
	{
		pdvtd32->tdPortNameOffset = cbOffset;
		cchWritten = MultiByteToWideChar(
						CP_ACP, 0,
						(char *)pdvtd16 + pdvtd16->tdPortNameOffset,
						pdvtdInfo->cchPortName,
						(LPOLESTR)((BYTE *)pdvtd32 +
							pdvtd32->tdPortNameOffset),
						pdvtdInfo->cchPortName);
		if (0 == cchWritten)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}

		cbOffset += cchWritten * sizeof(WCHAR);
	}

	if (pdvtd16->tdExtDevmodeOffset != 0)
	{
		pdvtd32->tdExtDevmodeOffset = cbOffset;
		pdm32 = (DEVMODEW *)((BYTE *)pdvtd32+pdvtd32->tdExtDevmodeOffset);

		pdm16 = (DEVMODEA *)((BYTE *)pdvtd16+pdvtd16->tdExtDevmodeOffset);

		//  The incoming DEVMODEA can have one of the following two forms:
		//
		//  1)  32 chars for dmDeviceName
		//      m bytes worth of fixed size data (where m <= 38)
		//      n bytes of dmDriverExtra data
		//
		//      and dmSize will be 32+m
		//
		//  2)  32 chars for dmDeviceName
		//      38 bytes worth of fixed size data
		//      32 chars for dmFormName
		//      m additional bytes of fixed size data
		//      n bytes of dmDriverExtra data
		//
		//      and dmSize will be 32 + 38 + 32 + m
		//
		//  We have to be careful to translate the dmFormName string, if it
		//  exists

		//  First, translate the dmDeviceName
		if (MultiByteToWideChar(CP_ACP, 0, (char *)pdm16->dmDeviceName,
								CCHDEVICENAME,
								pdm32->dmDeviceName, CCHDEVICENAME) == 0)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}

		//  Now check to see if we have a dmFormName to translate
		if (pdm16->dmSize <= FIELD_OFFSET(DEVMODEA, dmFormName))
		{
			//  No dmFormName, just copy the remaining m bytes
			memcpy(&pdm32->dmSpecVersion, &pdm16->dmSpecVersion,
				   pdm16->dmSize - CCHDEVICENAME);
		}
		else
		{
			//  There is a dmFormName;  copy the bytes between the names first
			memcpy(&pdm32->dmSpecVersion, &pdm16->dmSpecVersion,
				   FIELD_OFFSET(DEVMODEA, dmFormName) -
					FIELD_OFFSET(DEVMODEA, dmSpecVersion));

			//  Now translate the dmFormName
			if (MultiByteToWideChar(CP_ACP, 0, (char *)pdm16->dmFormName,
									CCHFORMNAME,
									pdm32->dmFormName, CCHFORMNAME) == 0)
			{
				hr = E_UNEXPECTED;
				goto ErrRtn;
			}

			//  Now copy the remaining m bytes

			if (pdm16->dmSize > FIELD_OFFSET(DEVMODEA, dmLogPixels))
			{
				memcpy(&pdm32->dmLogPixels, &pdm16->dmLogPixels,
					   pdm16->dmSize - FIELD_OFFSET(DEVMODEA, dmLogPixels));
			}
		}

		pdm32->dmSize = sizeof(DEVMODEW);
		if (pdm16->dmSize > sizeof(DEVMODEA))
		{
			pdm32->dmSize += pdm16->dmSize - sizeof(DEVMODEA);
		}

		//  Copy the extra driver bytes
		memcpy(((BYTE*)pdm32) + pdm32->dmSize, ((BYTE*)pdm16) + pdm16->dmSize,
			   pdm16->dmDriverExtra);

		cbOffset += pdm32->dmSize + pdm32->dmDriverExtra;
	}

	//  Finally, set pdvtd32's size
	pdvtd32->tdSize = cbOffset;

ErrRtn:

	return hr;
}

//+-------------------------------------------------------------------------
//
//  Function:   UtGetDvtd32Info
//
//  Synopsis:   Fills in pdvdtInfo
//
//  Arguments:  [pdvtd32] -- pointer to ANSI DVTARGETDEVICE
//              [pdvtdInfo] -- pointer to DVDT_INFO block
//
//  Requires:
//
//  Returns:    HRESULT
//
//  Modifies:   pdvtdInfo
//
//  Algorithm:
//
//  History:    06-May-94 XXXXX     Created from XXXXXXX original functions
//
//  Notes:      Do we need to do any error checking on the strings?
//
//--------------------------------------------------------------------------

extern "C" HRESULT UtGetDvtd32Info(DVTARGETDEVICE const *pdvtd32, PDVTDINFO pdvtdInfo)
{
	DEVMODEW *pdm32;

	pdvtdInfo->cbConvertSize = UT_DVTARGETDEVICE_SIZE;

	//  Compute required size for Drv, Device, Port names
	if (pdvtd32->tdDriverNameOffset != 0)
	{
		pdvtdInfo->cchDrvName = wcslen((WCHAR *)((BYTE *)pdvtd32 +
									   pdvtd32->tdDriverNameOffset)) + 1;

		pdvtdInfo->cbConvertSize += pdvtdInfo->cchDrvName * sizeof(WCHAR);
	}
	else
	{
		pdvtdInfo->cchDrvName = 0;
	}

	if (pdvtd32->tdDeviceNameOffset != 0)
	{
		pdvtdInfo->cchDevName = wcslen((WCHAR *)((BYTE *)pdvtd32 +
									   pdvtd32->tdDeviceNameOffset)) + 1;

		pdvtdInfo->cbConvertSize += pdvtdInfo->cchDevName * sizeof(WCHAR);
	}
	else
	{
		pdvtdInfo->cchDevName = 0;
	}

	if (pdvtd32->tdPortNameOffset != 0)
	{
		pdvtdInfo->cchPortName = wcslen((WCHAR *)((BYTE *)pdvtd32 +
										pdvtd32->tdPortNameOffset)) + 1;

		pdvtdInfo->cbConvertSize += pdvtdInfo->cchPortName * sizeof(WCHAR);
	}
	else
	{
		pdvtdInfo->cchPortName = 0;
	}

	//  Now compute the space needed for the DEVMODE
	if (pdvtd32->tdExtDevmodeOffset != 0)
	{
		pdm32 = (DEVMODEW *)((BYTE *)pdvtd32+pdvtd32->tdExtDevmodeOffset);

		//  We start with a basic DEVMODEA
		pdvtdInfo->cbConvertSize += sizeof(DEVMODEA);

		if (pdm32->dmSize > sizeof(DEVMODEW))
		{
			//  The input DEVMODEW is larger than a standard DEVMODEW, so
			//  add space for the extra amount
			pdvtdInfo->cbConvertSize += pdm32->dmSize - sizeof(DEVMODEW);
		}

		//  Finally we account for the extra driver data
		pdvtdInfo->cbConvertSize += pdm32->dmDriverExtra;
	}

	return(S_OK);
}

//+-------------------------------------------------------------------------
//
//  Function:   UtConvertDvtd32toDvtd16
//
//  Synopsis:   Fills in a 32-bit DVTARGETDEVICE based on a 16-bit
//              DVTARGETDEVICE
//
//  Arguments:  [pdvtd32] -- pointer to ANSI DVTARGETDEVICE
//              [pdvtdInfo] -- pointer to DVDT_INFO block
//              [pdvtd16] -- pointer to UNICODE DVTARGETDEVICE
//
//  Requires:   pdvtdInfo must have been filled in by a previous call to
//              UtGetDvtd32Info
//
//              pdvtd16 must be at least pdvtdInfo->cbSizeConvert bytes long
//
//  Returns:    HRESULT
//
//  Modifies:   pdvtd16
//
//  Algorithm:
//
//  History:    06-May-94 XXXXX     Created from XXXXXXX original functions
//
//  Notes:      Do we need to do any error checking on the strings?
//
//              On Chicago we'll have to provide helper code to do this
//              translation
//
//--------------------------------------------------------------------------

extern "C" HRESULT UtConvertDvtd32toDvtd16(DVTARGETDEVICE const *pdvtd32,
										   DVTDINFO const *pdvtdInfo,
										   DVTARGETDEVICEA *pdvtd16)
{
	HRESULT hr = S_OK;
	USHORT cbOffset;
	int cbWritten;
	DEVMODEA *pdm16;
	DEVMODEW *pdm32;

	memset(pdvtd16, 0, pdvtdInfo->cbConvertSize);

	cbOffset = UT_DVTARGETDEVICE_SIZE;

	if (pdvtdInfo->cchDrvName != 0)
	{
		pdvtd16->tdDriverNameOffset = cbOffset;
		cbWritten = WideCharToMultiByte(CP_ACP, 0,
								(WCHAR *)((BYTE *)pdvtd32 +
									pdvtd32->tdDriverNameOffset),
								pdvtdInfo->cchDrvName,
								(char *)pdvtd16 + pdvtd16->tdDriverNameOffset,
								pdvtdInfo->cchDrvName * sizeof(WCHAR),
								NULL, NULL);

		if (0 == cbWritten)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}
		cbOffset += cbWritten;
	}

	if (pdvtdInfo->cchDevName != 0)
	{
		pdvtd16->tdDeviceNameOffset = cbOffset;
		cbWritten = WideCharToMultiByte(
								CP_ACP, 0,
								(WCHAR *)((BYTE *)pdvtd32 +
									pdvtd32->tdDeviceNameOffset),
								pdvtdInfo->cchDevName,
								(char *)pdvtd16 + pdvtd16->tdDeviceNameOffset,
								pdvtdInfo->cchDevName * sizeof(WCHAR),
								NULL, NULL);

		if (0 == cbWritten)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}
		cbOffset += cbWritten;
	}

	if (pdvtdInfo->cchPortName != 0)
	{
		pdvtd16->tdPortNameOffset = cbOffset;
		cbWritten = WideCharToMultiByte(CP_ACP, 0,
								(WCHAR *)((BYTE *)pdvtd32 +
									pdvtd32->tdPortNameOffset),
								pdvtdInfo->cchPortName,
								(char *)pdvtd16 + pdvtd16->tdPortNameOffset,
								pdvtdInfo->cchPortName * sizeof(WCHAR),
								NULL, NULL);
		if (0 == cbWritten)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}
		cbOffset += cbWritten;
	}

	if (pdvtd32->tdExtDevmodeOffset != 0)
	{
		pdvtd16->tdExtDevmodeOffset = cbOffset;
		pdm16 = (DEVMODEA *)((BYTE *)pdvtd16+pdvtd16->tdExtDevmodeOffset);

		pdm32 = (DEVMODEW *)((BYTE *)pdvtd32+pdvtd32->tdExtDevmodeOffset);

		//  The incoming DEVMODEW can have one of the following two forms:
		//
		//  1)  32 WCHARs for dmDeviceName
		//      m bytes worth of fixed size data (where m <= 38)
		//      n bytes of dmDriverExtra data
		//
		//      and dmSize will be 64+m
		//
		//  2)  32 WCHARs for dmDeviceName
		//      38 bytes worth of fixed size data
		//      32 WCHARs for dmFormName
		//      m additional bytes of fixed size data
		//      n bytes of dmDriverExtra data
		//
		//      and dmSize will be 64 + 38 + 64 + m
		//
		//  We have to be careful to translate the dmFormName string, if it
		//  exists

		if (WideCharToMultiByte(CP_ACP, 0, pdm32->dmDeviceName, CCHDEVICENAME,
								(char *)pdm16->dmDeviceName, CCHDEVICENAME,
								NULL, NULL) == 0)
		{
			hr = E_UNEXPECTED;
			goto ErrRtn;
		}

		//  Now check to see if we have a dmFormName to translate
		if (pdm32->dmSize <= FIELD_OFFSET(DEVMODEW, dmFormName))
		{
			//  No dmFormName, just copy the remaining m bytes
			memcpy(&pdm16->dmSpecVersion, &pdm32->dmSpecVersion,
				   pdm32->dmSize - FIELD_OFFSET(DEVMODEW, dmSpecVersion));
		}
		else
		{
			//  There is a dmFormName;  copy the bytes between the names first
			memcpy(&pdm16->dmSpecVersion, &pdm32->dmSpecVersion,
				   FIELD_OFFSET(DEVMODEW, dmFormName) -
					 FIELD_OFFSET(DEVMODEW, dmSpecVersion));

			//  Now translate the dmFormName
			if (WideCharToMultiByte(CP_ACP, 0,
									pdm32->dmFormName, CCHDEVICENAME,
									(char *) pdm16->dmFormName, CCHDEVICENAME,
									NULL, NULL) == 0)
			{
				hr = E_UNEXPECTED;
				goto ErrRtn;
			}

			//  Now copy the remaining m bytes

			if (pdm32->dmSize > FIELD_OFFSET(DEVMODEW, dmLogPixels))
			{
				memcpy(&pdm16->dmLogPixels, &pdm32->dmLogPixels,
					   pdm32->dmSize - FIELD_OFFSET(DEVMODEW, dmLogPixels));
			}
		}

		pdm16->dmSize = sizeof(DEVMODEA);
		if (pdm32->dmSize > sizeof(DEVMODEW))
		{
			pdm16->dmSize += pdm32->dmSize - sizeof(DEVMODEW);
		}

		//  Copy the extra driver bytes
		memcpy(((BYTE*)pdm16) + pdm16->dmSize, ((BYTE*)pdm32) + pdm32->dmSize,
			   pdm32->dmDriverExtra);

		cbOffset += pdm16->dmSize + pdm16->dmDriverExtra;
	}

	//  Finally, set pdvtd16's size
	pdvtd16->tdSize = cbOffset;

ErrRtn:

	return hr;
}


//+-------------------------------------------------------------------------
//
//  Function:   ConvertObjectDescriptorToA
//
//  Synopsis:   Convert a Unicode ObjectDescriptor to ANSI.
//
//  Arguments:  [phGlobal] -- Pointer an Object Descriptor handle.
//
//  Returns:    HRESULT
//
//  Modifies:   phGlobal
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//--------------------------------------------------------------------------
HRESULT ConvertObjectDescriptorToA(HGLOBAL * phGlobal)
{
	HGLOBAL hGlobal;
	HRESULT hResult;


	hGlobal = * phGlobal;

	hResult = ConvertObjectDescriptorToA(hGlobal, phGlobal);
	if (FAILED(hResult))
		return hResult;

	GlobalFree(hGlobal);

	return ResultFromScode(S_OK);
}


//+-------------------------------------------------------------------------
//
//  Function:   ConvertObjectDescriptorToA
//
//  Synopsis:   Convert a Unicode ObjectDescriptor to ANSI.
//
//  Arguments:  [hGlobal]   -- Unicode Object Descriptor handle.
//              [phGlobalA] -- Pointer an ANSI Object Descriptor handle.
//
//  Returns:    HRESULT
//
//  Modifies:   phGlobalA
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//--------------------------------------------------------------------------
HRESULT ConvertObjectDescriptorToA(HGLOBAL hGlobal, HGLOBAL * phGlobalA)
{
	LPOBJECTDESCRIPTOR pObjDes;
	LPOBJECTDESCRIPTOR pObjDesA;
	ULONG   ulSize;
	PBYTE   ptr;


	ulSize = SizeOfObjectDescriptorAsANSI(hGlobal);

	*phGlobalA = GlobalAlloc(GMEM_MOVEABLE, ulSize);
	if (*phGlobalA == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	pObjDes  = (LPOBJECTDESCRIPTOR)GlobalLock(hGlobal);
	pObjDesA = (LPOBJECTDESCRIPTOR)GlobalLock(*phGlobalA);

	memcpy(pObjDesA, pObjDes, sizeof(OBJECTDESCRIPTOR));

	if (ulSize != pObjDes->cbSize)
	{
		pObjDesA->cbSize = ulSize;

		ptr = (PBYTE)pObjDesA + sizeof(OBJECTDESCRIPTOR);

		if (pObjDes->dwFullUserTypeName > 0)
		{
			pObjDesA->dwFullUserTypeName = sizeof(OBJECTDESCRIPTOR);

			ConvertStringToA(GetUnicodeString(pObjDes, dwFullUserTypeName),
					(LPSTR)ptr);
			ptr += lstrlen((LPSTR)ptr) + 1;
		}

		if (pObjDes->dwSrcOfCopy > 0)
		{
			pObjDesA->dwSrcOfCopy = ptr - (PBYTE)pObjDesA;

			ConvertStringToA(GetUnicodeString(pObjDes, dwSrcOfCopy),
					(LPSTR)ptr);
		}
	}

	GlobalUnlock(hGlobal);
	GlobalUnlock(*phGlobalA);

	return ResultFromScode(S_OK);
}


//+-------------------------------------------------------------------------
//
//  Function:   ConvertObjectDescriptorToW
//
//  Synopsis:   Convert an ANSI ObjectDescriptor to Unicode.
//
//  Arguments:  [phGlobal] -- Pointer an Object Descriptor handle.
//
//  Returns:    HRESULT
//
//  Modifies:   phGlobal
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//--------------------------------------------------------------------------
HRESULT ConvertObjectDescriptorToW(HGLOBAL * phGlobal)
{
	HGLOBAL hGlobal;
	HRESULT hResult;


	hGlobal = * phGlobal;

	hResult = ConvertObjectDescriptorToW(hGlobal, phGlobal);
	if (FAILED(hResult))
		return hResult;

	GlobalFree(hGlobal);

	return ResultFromScode(S_OK);
}


//+-------------------------------------------------------------------------
//
//  Function:   ConvertObjectDescriptorToW
//
//  Synopsis:   Convert an ANSI ObjectDescriptor to Unicode.
//
//  Arguments:  [hGlobalA] -- ANSI Object Descriptor handle.
//              [phGlobal] -- Pointer a Unicode Object Descriptor handle.
//
//  Returns:    HRESULT
//
//  Modifies:   phGlobal
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//--------------------------------------------------------------------------
HRESULT ConvertObjectDescriptorToW(HGLOBAL hGlobalA, HGLOBAL * phGlobal)
{
	LPOBJECTDESCRIPTOR pObjDesA;
	LPOBJECTDESCRIPTOR pObjDes;
	ULONG   ulSize;
	PBYTE   ptr;


	ulSize = SizeOfObjectDescriptorAsWide(hGlobalA);

	*phGlobal = GlobalAlloc(GMEM_MOVEABLE, ulSize);
	if (*phGlobal == NULL)
		return ResultFromScode(E_OUTOFMEMORY);

	pObjDesA = (LPOBJECTDESCRIPTOR)GlobalLock(hGlobalA);
	pObjDes  = (LPOBJECTDESCRIPTOR)GlobalLock(*phGlobal);

	memcpy(pObjDes, pObjDesA, sizeof(OBJECTDESCRIPTOR));

	if (ulSize != pObjDesA->cbSize)
	{
		pObjDes->cbSize = ulSize;

		ptr = (PBYTE)pObjDes + sizeof(OBJECTDESCRIPTOR);

		if (pObjDesA->dwFullUserTypeName > 0)
		{
			pObjDes->dwFullUserTypeName = sizeof(OBJECTDESCRIPTOR);

			ConvertStringToW(GetANSIString(pObjDesA, dwFullUserTypeName),
					(LPWSTR)ptr);
			ptr += (wcslen((LPWSTR)ptr) + 1) * 2;
		}

		if (pObjDesA->dwSrcOfCopy > 0)
		{
			pObjDes->dwSrcOfCopy = ptr - (PBYTE)pObjDes;

			ConvertStringToW(GetANSIString(pObjDesA, dwSrcOfCopy),
					(LPWSTR)ptr);
		}
	}

	GlobalUnlock(hGlobalA);
	GlobalUnlock(*phGlobal);

	return ResultFromScode(S_OK);
}


//+-------------------------------------------------------------------------
//
//  Function:   SizeOfObjectDescriptorAsANSI
//
//  Synopsis:   Compute the size of a Unicode structure in ANSI form.
//
//  Arguments:  [hGlobal] -- Handle to Object Descriptor.
//
//  Returns:    Byte count of Object descriptor as ANSI.
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//--------------------------------------------------------------------------
ULONG SizeOfObjectDescriptorAsANSI(HGLOBAL hGlobal)
{
	LPOBJECTDESCRIPTOR pObjDes;
	ULONG ulSize;


	pObjDes = (LPOBJECTDESCRIPTOR)GlobalLock(hGlobal);

	Assert(pObjDes != NULL);

	ulSize = pObjDes->cbSize;

	if (pObjDes->dwFullUserTypeName > 0)
		ulSize -= wcslen(GetUnicodeString(pObjDes, dwFullUserTypeName)) + 1;

	if (pObjDes->dwSrcOfCopy > 0)
		ulSize -= wcslen(GetUnicodeString(pObjDes, dwSrcOfCopy)) + 1;

	Assert(ulSize <= pObjDes->cbSize);
	Assert(ulSize >= sizeof(OBJECTDESCRIPTOR));

	GlobalUnlock(hGlobal);

	return ulSize;
}


//+-------------------------------------------------------------------------
//
//  Function:   SizeOfObjectDescriptorAsWide
//
//  Synopsis:   Compute the size of an ANSI structure in Unicode form.
//
//  Arguments:  [hGlobal] -- Handle to Object Descriptor.
//
//  Returns:    Byte count of Object descriptor as Unicode.
//
//  History:    15-Jun-94   'Hammer'    Created.
//
//--------------------------------------------------------------------------
ULONG SizeOfObjectDescriptorAsWide(HGLOBAL hGlobal)
{
	LPOBJECTDESCRIPTOR pObjDes;
	ULONG ulSize;


	pObjDes = (LPOBJECTDESCRIPTOR)GlobalLock(hGlobal);

	Assert(pObjDes != NULL);

	ulSize = pObjDes->cbSize;

	if (pObjDes->dwFullUserTypeName > 0)
		ulSize += lstrlen(GetANSIString(pObjDes, dwFullUserTypeName)) + 1;

	if (pObjDes->dwSrcOfCopy > 0)
		ulSize += lstrlen(GetANSIString(pObjDes, dwSrcOfCopy)) + 1;

	Assert(ulSize < pObjDes->cbSize + MAX_PATH * 2);
	Assert(ulSize >= sizeof(OBJECTDESCRIPTOR));

	GlobalUnlock(hGlobal);

	return ulSize;
}
