///////////////////////////////////////////////////////////////////////////////
//
//  GetBitmapType
//
//  return the bitmap type that the display driver uses
//
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------


#define BM_NULL         0           // I dont know the type
#define BM_8BIT         0x01        // 256 color
#define BM_16555        0x02        // 16bpp 555
#define BM_24BGR        0x03        // 24bpp BGR (just like a DIB)
#define BM_32BGR        0x04        // 32 bit BGR
#define BM_16565        0x06        // 16bpp 565
#define BM_24RGB        0x07        // 24 bit RGB
#define BM_32RGB        0x08        // 32 bit RGB

UINT GetBitmapType(HDC hdc);

///////////////////////////////////////////////////////////////////////////////
//
//  GetPhysColor
//
//  returns the phys color for a device.
//
///////////////////////////////////////////////////////////////////////////////

DWORD GetPhysColor(HDC hdc, COLORREF rgb);
