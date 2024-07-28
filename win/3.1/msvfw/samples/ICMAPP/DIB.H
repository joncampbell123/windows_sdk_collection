/*----------------------------------------------------------------------------*\
|
|   DIB.H
|
|   Routines for dealing with Device independent bitmaps                       
|
\*----------------------------------------------------------------------------*/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/


typedef HANDLE HDIB;

HDIB        ReadDibBitmapInfo   (int fh);
HDIB        OpenDIB             (LPSTR szFile);
BOOL        WriteDIB            (LPSTR szFile,HDIB hdib);
HPALETTE    CreateBIPalette     (LPBITMAPINFOHEADER lpbi);
HDIB        DibFromBitmap       (HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal, WORD wUsage);
HBITMAP     BitmapFromDib       (HDIB hdib, HPALETTE hpal, WORD wUsage);
BOOL        SetDibUsage         (HDIB hdib, HPALETTE hpal,WORD wUsage);
HDIB        CreateDib           (int bits, int dx, int dy);
BOOL        StretchDib          (HDC hdc, int x, int y, int dx, int dy, HDIB hdib, HPALETTE hpal, int x0, int y0, int dx0, int dy0, LONG rop, WORD wUsage);
HDIB        CopyDib             (HDIB hdib);
HDIB        CopyPalette         (HPALETTE hpal);

/*----------------------------------------------------------------------------*\
|   Macros                                                                     |
\*----------------------------------------------------------------------------*/

#define DrawDib(hdc,x,y,hdib,hpal,wUsage)  StretchDib(hdc,x,y,-1,-1,hdib,hpal,0,0,-1,-1,SRCCOPY,wUsage)
#define CreateDibPalette(hdib)  CreateBIPalette(GlobalLock(hdib))

#define ALIGNULONG(i)           ((i+3)/4*4)        /* ULONG aligned ! */
#define WIDTHBYTES(i)           ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */

#define DIBWIDTHBYTES(bi)       DibWidthBytes(&bi)

#define DibWidthBytesN(lpbi, n) (UINT)WIDTHBYTES((UINT)(lpbi)->biWidth * (UINT)(n))
#define DibWidthBytes(lpbi)     DibWidthBytesN(lpbi, (lpbi)->biBitCount)

#define DibSizeImage(lpbi)      ((lpbi)->biSizeImage == 0 \
                                    ? ((DWORD)(UINT)DibWidthBytes(lpbi) * (DWORD)(UINT)(lpbi)->biHeight) \
                                    : (lpbi)->biSizeImage)

#define DibSize(lpbi)           ((lpbi)->biSize + (lpbi)->biSizeImage + (int)(lpbi)->biClrUsed * sizeof(RGBQUAD))

#define DibPtr(lpbi)            (LPVOID)(DibColors(lpbi) + (UINT)(lpbi)->biClrUsed)
#define DibColors(lpbi)         ((LPRGBQUAD)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))

#define DibNumColors(lpbi)      ((lpbi)->biClrUsed == 0 && (lpbi)->biBitCount <= 8 \
                                    ? (int)(1 << (int)(lpbi)->biBitCount)          \
                                    : (int)(lpbi)->biClrUsed)

#define DibXYN(lpbi,pb,x,y,n)   (LPVOID)(                                     \
                                (PBYTE)(pb) +                                 \
                                (UINT)((UINT)(x) * (UINT)(n) / 8u) +          \
                                ((DWORD)DibWidthBytesN(lpbi,n) * (DWORD)(UINT)(y)))

#define DibXY(lpbi,x,y)         DibXYN(lpbi,DibPtr(lpbi),x,y,(lpbi)->biBitCount)


#define FixBitmapInfo(lpbi)     if ((lpbi)->biSizeImage == 0)                 \
                                    (lpbi)->biSizeImage = DibSizeImage(lpbi); \
                                if ((lpbi)->biClrUsed == 0)                   \
                                    (lpbi)->biClrUsed = DibNumColors(lpbi);
