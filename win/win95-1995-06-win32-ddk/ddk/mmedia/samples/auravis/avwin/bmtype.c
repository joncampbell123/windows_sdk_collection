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
#include <windows.h>
#include <windowsx.h>
#include "bmtype.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Surface Types
//
///////////////////////////////////////////////////////////////////////////////

#ifdef WIN32
#define BCODE 
#else
#define BCODE _based(_segname("_CODE"))
#endif

static BYTE  BCODE bits8[]   = {0x00,0xF9,0xFA,0xFC,0xFF};
static WORD  BCODE bits555[] = {0x0000,0x7C00,0x03E0,0x001F,0x7FFF};
static WORD  BCODE bits5551[]= {0x8000,0xFC00,0x83E0,0x801F,0xFFFF};
static WORD  BCODE bits5552[]= {0x0000,0x7C00,0x03E0,0x001F,0xFFFF};
static WORD  BCODE bits565[] = {0x0000,0xF800,0x07E0,0x001F,0xFFFF};
static BYTE  BCODE bitsBGR[] = {0x00,0x00,0x00, 0x00,0x00,0xFF, 0x00,0xFF,0x00, 0xFF,0x00,0x00, 0xFF,0xFF,0xFF};
static BYTE  BCODE bitsRGB[] = {0x00,0x00,0x00, 0xFF,0x00,0x00, 0x00,0xFF,0x00, 0x00,0x00,0xFF, 0xFF,0xFF,0xFF};
static DWORD BCODE bitsRGBX[]= {0x000000, 0x0000FF, 0x00FF00, 0xFF0000, 0xFFFFFF};
static DWORD BCODE bitsBGRX[]= {0x000000, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFFFF};

///////////////////////////////////////////////////////////////////////////////
//
//  GetBitmapType
//
//  return the bitmap type that the display driver uses
//
///////////////////////////////////////////////////////////////////////////////

UINT GetBitmapType(HDC hdc)
{
    BITMAP bm;
    HBITMAP hbm;
    HBITMAP hbmT;
    BYTE bits[20*4];    // at most 32bpp

    hbm = CreateCompatibleBitmap(hdc,20,1);

    hdc = CreateCompatibleDC(hdc);
    hbmT = SelectObject(hdc, hbm);

    GetObject(hbm, sizeof(bm), &bm);
    PatBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, BLACKNESS);

    SetPixel(hdc, 0, 0, RGB(000,000,000));
    SetPixel(hdc, 1, 0, RGB(255,000,000));
    SetPixel(hdc, 2, 0, RGB(000,255,000));
    SetPixel(hdc, 3, 0, RGB(000,000,255));
    SetPixel(hdc, 4, 0, RGB(255,255,255));

    GetBitmapBits(hbm, sizeof(bits), bits);

    SelectObject(hdc, hbmT);
    DeleteObject(hbm);
    DeleteDC(hdc);

    #define TESTFMT(a,n) \
        if (_fmemcmp(bits, (LPVOID)a, sizeof(a)) == 0) return n;

    TESTFMT(bits8,    BM_8BIT);
    TESTFMT(bits555,  BM_16555);
    TESTFMT(bits5551, BM_16555);
    TESTFMT(bits5552, BM_16555);
    TESTFMT(bits565,  BM_16565);
    TESTFMT(bitsRGB,  BM_24RGB);
    TESTFMT(bitsBGR,  BM_24BGR);
    TESTFMT(bitsRGBX, BM_32RGB);
    TESTFMT(bitsBGRX, BM_32BGR);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
//  GetPhysColor
//
//  returns the phys color for a device.
//
///////////////////////////////////////////////////////////////////////////////

DWORD GetPhysColor(HDC hdc, COLORREF rgb)
{
    HBITMAP hbm;
    HBITMAP hbmT;
    DWORD color = 0;

    hbm = CreateCompatibleBitmap(hdc,1,1);
    hdc = CreateCompatibleDC(hdc);
    hbmT = SelectObject(hdc, hbm);

    PatBlt(hdc, 0, 0, 1, 1, BLACKNESS);
    SetPixel(hdc, 0, 0, rgb);
    GetBitmapBits(hbm, sizeof(color), &color);

    SelectObject(hdc, hbmT);
    DeleteObject(hbm);
    DeleteDC(hdc);

    return color;
}
