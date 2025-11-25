//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
// garguids.h
//

// The public CLSID's used by the gargle filter.
// These same numbers occur in gargle.reg which allows them to be added to the
// registry to give access to these objects by other software components
// such as the ActiveMovie Graph Builder, GraphEdt and so on.

#ifndef __GARGUIDS__
#define __GARGUIDS__

#ifdef __cplusplus
extern "C" {
#endif


//
// Gargle filter object
// {d616f350-d622-11ce-aac5-0020af0b99a3}
DEFINE_GUID(CLSID_Gargle,
0xd616f350, 0xd622, 0x11ce, 0xaa, 0xc5, 0x00, 0x20, 0xaf, 0x0b, 0x99, 0xa3);


//
// Gargle filter property page
// {d616f351-d622-11ce-aac5-0020af0b99a3}
DEFINE_GUID(CLSID_GargProp,
0xd616f351, 0xd622, 0x11ce, 0xaa, 0xc5, 0x00, 0x20, 0xaf, 0x0b, 0x99, 0xa3);

//
//  Note: IGargle's uuid is defined with the interface, see igargle.h
//  IGargle is a private interface created by the filter.
//  The filter object and the property page defined here are public interfaces
//  that can be called by an application or another filter.


#ifdef __cplusplus
}
#endif

#endif // __GARGUIDS__
