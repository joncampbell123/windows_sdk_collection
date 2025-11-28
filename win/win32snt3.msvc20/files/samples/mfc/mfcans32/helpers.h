//+--------------------------------------------------------------------------
//
//  Copyright (C) 1994, Microsoft Corporation.  All Rights Reserved.
//
//  File:       helpers.h
//
//  Contents:   Helper functions for the OLE2 ANSI/UNICODE Layer.
//
//  History:    01-Nov-93   v-kentc     Created.
//
//---------------------------------------------------------------------------

extern HRESULT QueryInterfaceANSI(LPUNKNOWN punk, REFIID, LPUNKNOWN * ppunk);
extern HRESULT QueryInterfaceWide(LPUNKNOWN punk, REFIID, LPUNKNOWN * ppunk);

extern BOOL TranslateInterfaceAToW(REFIID riidA, IID * riidW);
extern BOOL TranslateInterfaceWToA(REFIID riidW, IID * riidA);
