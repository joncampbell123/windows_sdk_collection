//===========================================================================
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//===========================================================================
//
// filename: exdevuid.h
//

// The CLSID's used by the VCR Control filter

#ifndef __EXDEVUID__
#define __EXDEVUID__

//
// VCR Control Filter Object
// {c8a3b330-0f2a-11cf-8c09-204c4f4f5020}
DEFINE_GUID(CLSID_VCRControlFilter,
0xc8a3b330, 0x0f2a, 0x11cf, 0x8c, 0x09, 0x20, 0x4c, 0x4f, 0x4f, 0x50, 0x20);

// VCR Property Page - General Props
// {c8a3b331-0f2a-11cf-8c09-204c4f4f5020}
DEFINE_GUID(CLSID_VCRControlPropertyPage,
0xc8a3b331, 0x0f2a, 0x11cf, 0x8c, 0x09, 0x20, 0x4c, 0x4f, 0x4f, 0x50, 0x20);

// VCR Property Page - Transport Props
// {c8a3b332-0f2a-11cf-8c09-204c4f4f5020}
DEFINE_GUID(CLSID_VCRTransPropertyPage,
0xc8a3b332, 0x0f2a, 0x11cf, 0x8c, 0x09, 0x20, 0x4c, 0x4f, 0x4f, 0x50, 0x20);


#endif __EXDEVUID__	// #ifndef __EXDEVUID__

// eof exdevuid.h
