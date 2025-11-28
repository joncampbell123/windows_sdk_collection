//+--------------------------------------------------------------------------
//
//  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
//
//  File:       convert.h
//
//  Contents:   ANSI Wrappers for Unicode CompObj Interfaces and APIs.
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
//              ConvertObjectDescriptorToA
//              ConvertObjectDescriptorToW
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

#define MAX_STRING  MAX_PATH


//
//  Define macros to aid retrieving info from data structures.
//
#define GetUnicodeString(ptr, offset) \
		((LPWSTR)((LPBYTE)(ptr) + (ptr->offset)))
#define GetANSIString(ptr, offset) \
		((LPSTR)((LPBYTE)(ptr) + (ptr->offset)))


HRESULT ConvertStringToW(LPWSTR *);
HRESULT ConvertStringToW(LPCSTR, LPWSTR *);
VOID    ConvertStringToW(LPCSTR, LPWSTR);
HRESULT ConvertStringToW(LPCSTR, LCID, LPWSTR *);
HRESULT ConvertStringToA(LPSTR *);
VOID    ConvertStringToA(LPCWSTR, LPSTR);
HRESULT ConvertStringToA(LPCWSTR, LPSTR *);
HRESULT ConvertStringArrayToA(LPSTR *, ULONG);
HRESULT ConvertStringArrayToW(LPWSTR *, ULONG);
HRESULT ConvertStringArrayToA(LPWSTR *, LPSTR * *, ULONG);
HRESULT ConvertStringArrayToW(LPSTR *, LPWSTR * *, ULONG);
HRESULT ConvertStringArrayToW(LPSTR *, LCID, LPWSTR * *, ULONG);

HRESULT ConvertStringAlloc(ULONG, LPVOID *);
VOID    ConvertStringFree(LPVOID);
VOID    ConvertStringFree(LPSTR);
VOID    ConvertStringFree(LPWSTR);
VOID    ConvertStringArrayFree(LPSTR *, ULONG);
VOID    ConvertStringArrayFree(LPWSTR *, ULONG);

HRESULT ConvertSTATSTGToA(STATSTGA *);
HRESULT ConvertSTATSTGToW(STATSTG *);
HRESULT ConvertSTATSTGArrayToA(STATSTGA *, ULONG);
HRESULT ConvertSTATSTGArrayToW(STATSTG *, ULONG);

HRESULT ConvertFORMATETCToA(FORMATETC *, FORMATETCA *);
HRESULT ConvertFORMATETCToW(FORMATETCA *, FORMATETC *);
HRESULT ConvertFORMATETCArrayToA(FORMATETCA *, ULONG);
HRESULT ConvertFORMATETCArrayToW(FORMATETC *, ULONG);
VOID    ConvertFORMATETCFree(FORMATETC *);
VOID    ConvertFORMATETCFree(FORMATETCA *);

HRESULT ConvertDVTARGETDEVICEToA(DVTARGETDEVICEA * *);
HRESULT ConvertDVTARGETDEVICEToA(DVTARGETDEVICE *, DVTARGETDEVICEA * *);
HRESULT ConvertDVTARGETDEVICEToW(DVTARGETDEVICE * *);
HRESULT ConvertDVTARGETDEVICEToW(DVTARGETDEVICEA *, DVTARGETDEVICE * *);
VOID    ConvertDVTARGETDEVICEFree(DVTARGETDEVICE *);
VOID    ConvertDVTARGETDEVICEFree(DVTARGETDEVICEA *);

HRESULT ConvertSNBToA(SNB, SNBA *);
HRESULT ConvertSNBToW(SNBA, SNB *);
VOID    ConvertSNBFree(SNBA);
VOID    ConvertSNBFree(SNB);

HRESULT ConvertSTATDATAToA(STATDATAA *);
HRESULT ConvertSTATDATAToW(STATDATA *);
HRESULT ConvertSTATDATAArrayToA(STATDATAA *, ULONG);
HRESULT ConvertSTATDATAArrayToW(STATDATA *, ULONG);

HRESULT ConvertOLEVERBToA(OLEVERBA *);
HRESULT ConvertOLEVERBToW(OLEVERB *);
HRESULT ConvertOLEVERBArrayToA(OLEVERBA *, ULONG);
HRESULT ConvertOLEVERBArrayToW(OLEVERB *, ULONG);

HRESULT ConvertSTGMEDIUMToA(CLIPFORMAT, LPSTGMEDIUMA);
HRESULT ConvertSTGMEDIUMToA(CLIPFORMAT, LPSTGMEDIUM, LPSTGMEDIUMA);
HRESULT ConvertSTGMEDIUMToW(CLIPFORMAT, LPSTGMEDIUM);
HRESULT ConvertSTGMEDIUMToW(CLIPFORMAT, LPSTGMEDIUMA, LPSTGMEDIUM);
HRESULT ConvertSTGMEDIUMFree(CLIPFORMAT, LPSTGMEDIUM);
HRESULT ConvertSTGMEDIUMFree(CLIPFORMAT, LPSTGMEDIUMA);

HRESULT ConvertMonikerArrayToA(LPMONIKERA *, ULONG);
HRESULT ConvertMonikerArrayToW(LPMONIKER *, ULONG);

HRESULT ConvertDispStringToW(LPBSTR);
HRESULT ConvertDispStringToW(BSTRA, LPBSTR);
HRESULT ConvertDispStringToW(BSTRA, LCID, LPBSTR);
HRESULT ConvertDispStringToA(LPBSTRA);
HRESULT ConvertDispStringToA(BSTR, LPBSTRA);
HRESULT ConvertDispStringArrayToA(LPBSTRA, ULONG);
HRESULT ConvertDispStringArrayToW(LPBSTR, ULONG);

HRESULT ConvertDispStringAlloc(ULONG, LPBSTR);
#define ConvertDispStringFreeA  SysFreeStringA
#define ConvertDispStringFreeW  SysFreeString
VOID ConvertDispStringArrayFree(BSTRA * ptr, ULONG ulSize);
VOID ConvertDispStringArrayFree(BSTR * ptr, ULONG ulSize);

HRESULT ConvertVARDESCToA(LPVARDESCA);
HRESULT ConvertVARDESCToW(LPVARDESC);
HRESULT ConvertVARDESCToA(LPVARDESC, LPVARDESCA *);
HRESULT ConvertVARDESCToW(LPVARDESCA, LPVARDESC *);
VOID    ConvertVARDESCFree(LPVARDESC);
VOID    ConvertVARDESCFree(LPVARDESCA);

HRESULT ConvertDispParamsToA(LPDISPPARAMS, LPDISPPARAMSA *, LPVARIANTA *);
HRESULT ConvertDispParamsToW(LPDISPPARAMSA, LPDISPPARAMS *, LPVARIANT *);
HRESULT ConvertDispParamsCopyBack(LPDISPPARAMSA, LPVARIANT);
HRESULT ConvertDispParamsCopyBack(LPDISPPARAMS, LPVARIANTA);
VOID    ConvertDispParamsFree(LPDISPPARAMS, LPVARIANT);

HRESULT ConvertVariantToA(LPVARIANTA);
HRESULT ConvertVariantToW(LPVARIANT);
HRESULT ConvertVariantToA(LPVARIANT, LPVARIANTA);
HRESULT ConvertVariantToA(LPVARIANT, LPVARIANTA *);
HRESULT ConvertVariantToW(LPVARIANTA, LPVARIANT);
HRESULT ConvertVariantToW(LPVARIANTA, LPVARIANT *);
HRESULT ConvertVariantArrayToA(LPVARIANTA, ULONG);
HRESULT ConvertVariantArrayToW(LPVARIANT, ULONG);
HRESULT ConvertVariantFix(LPVARIANT);
VOID    ConvertVariantFree(LPVARIANT);
VOID    ConvertVariantFree(LPVARIANTA);
VOID    ConvertVariantTempFree(LPVARIANT);

STDAPI  ConvertExcepInfoToA(LPEXCEPINFOA);
STDAPI  ConvertExcepInfoToW(LPEXCEPINFO);

HRESULT ConvertInterfaceDataToW(LPINTERFACEDATAA, LCID, LPINTERFACEDATA *);
HRESULT ConvertInterfaceDataFree(LPINTERFACEDATA);

//
//  Define Object Descriptor convert routines.
//
HRESULT ConvertObjectDescriptorToA(HGLOBAL *);
HRESULT ConvertObjectDescriptorToA(HGLOBAL, HGLOBAL *);
HRESULT ConvertObjectDescriptorToW(HGLOBAL *);
HRESULT ConvertObjectDescriptorToW(HGLOBAL, HGLOBAL *);
