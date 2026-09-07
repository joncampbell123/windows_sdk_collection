//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1995 - 1998  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//  File:       ddmm.h
//  Content:    Routines for using DirectDraw on a multimonitor system
//--------------------------------------------------------------------------;

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

// DDRAW.H might not include these
#ifndef DDENUM_ATTACHEDSECONDARYDEVICES
#define DDENUM_ATTACHEDSECONDARYDEVICES     0x00000001L
#endif

typedef HRESULT (*PDRAWCREATE)(IID *,LPDIRECTDRAW *,LPUNKNOWN);
typedef HRESULT (*PDRAWENUM)(LPDDENUMCALLBACKA, LPVOID);

IDirectDraw * DirectDrawCreateFromDevice(LPSTR, PDRAWCREATE, PDRAWENUM);
IDirectDraw * DirectDrawCreateFromDeviceEx(LPSTR, PDRAWCREATE, LPDIRECTDRAWENUMERATEEXA);

#ifdef __cplusplus
}
#endif	/* __cplusplus */
