//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       ConvCtl.h
//
//  Contents:   ANSI Wrappers for OLE Control Interfaces and APIs.
//
//  History:    01-Jun-94   johnels     Created.
//
//---------------------------------------------------------------------------

HRESULT ConvertPROPPAGEINFOToA(LPPROPPAGEINFO, LPPROPPAGEINFOA);
HRESULT ConvertPROPPAGEINFOToW(LPPROPPAGEINFOA, LPPROPPAGEINFO);
void ConvertPROPPAGEINFOFree(LPPROPPAGEINFO);
void ConvertPROPPAGEINFOFree(LPPROPPAGEINFOA);
