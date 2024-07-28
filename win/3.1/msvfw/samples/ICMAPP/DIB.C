/*----------------------------------------------------------------------------*\
|   Routines for dealing with Device independent bitmaps                       |
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

#include <windows.h>
#include "dib.h"

/* flags for _lseek */
#define  SEEK_CUR 1
#define  SEEK_END 2
#define  SEEK_SET 0

#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

#define ISDIB(bft) ((bft) == BFT_BITMAP)

#define PALVERSION      0x300
#define MAXPALETTE      256

/*
 *   Open a DIB file and return a MEMORY DIB, a memory handle containing..
 *
 *   BITMAP INFO    bi
 *   palette data
 *   bits....
 *
 */
HDIB OpenDIB(LPSTR szFile)
{
    int			fh;
    LPBITMAPINFOHEADER  lpbi;
    DWORD		dwLen;
    HDIB                hdib;
    HANDLE              h;
    OFSTRUCT            of;

    if (HIWORD((DWORD)szFile) == 0)
    {
        fh = LOWORD((DWORD)szFile);
    }
    else
    {
        fh = OpenFile(szFile, &of, OF_READ);
    }

    if (fh == -1)
	return NULL;

    hdib = ReadDibBitmapInfo(fh);

    if (!hdib) 
	goto error;

    lpbi = (LPVOID)GlobalLock(hdib);

    /* How much memory do we need to hold the DIB */

    dwLen  = DibSize(lpbi);

    /* Can we get more memory? */

    h = GlobalReAlloc(hdib,dwLen,0);

    if (!h)
    {
	GlobalFree(hdib);
	hdib = NULL;
    }
    else
    {
	hdib = h;
    }

    if (hdib)
    {
        lpbi = (LPVOID)GlobalLock(hdib);

	/* read in the bits */
        _hread(fh, DibPtr(lpbi), lpbi->biSizeImage);
    }

error:
    if (HIWORD((DWORD)szFile) != 0)
        _lclose(fh);

    return hdib;
}

/*
 *   Write a global handle in CF_DIB format to a file.
 *
 */
BOOL WriteDIB(LPSTR szFile,HDIB hdib)
{
    BITMAPFILEHEADER	hdr;
    LPBITMAPINFOHEADER  lpbi;
    int                 fh;
    OFSTRUCT            of;

    if (!hdib)
	return FALSE;

    if (HIWORD((DWORD)szFile) == 0)
    {
        fh = LOWORD((DWORD)szFile);
    }
    else
    {
        fh = OpenFile(szFile,&of,OF_CREATE|OF_READWRITE);
    }

    if (fh == -1)
        return FALSE;

    lpbi = (LPVOID)GlobalLock(hdib);

    hdr.bfType		= BFT_BITMAP;
    hdr.bfSize          = DibSize(lpbi) + sizeof(BITMAPFILEHEADER);
    hdr.bfReserved1     = 0;
    hdr.bfReserved2     = 0;
    hdr.bfOffBits       = (DWORD)sizeof(BITMAPFILEHEADER) +
                          lpbi->biSize + lpbi->biClrUsed * sizeof(RGBQUAD);

    _lwrite(fh,(LPVOID)&hdr,sizeof(BITMAPFILEHEADER));
    _hwrite(fh,(LPVOID)lpbi,DibSize(lpbi));

    GlobalUnlock(hdib);

    if (HIWORD((DWORD)szFile) != 0)
        _lclose(fh);

    return TRUE;
}

/*
 *  CreateBIPalette()
 *
 *  Given a Pointer to a BITMAPINFO struct will create a
 *  a GDI palette object from the color table.
 *
 */
HPALETTE CreateBIPalette(LPBITMAPINFOHEADER lpbi)
{
    LOGPALETTE          *pPal;
    HPALETTE            hpal = NULL;
    WORD                nNumColors;
    int                 i;
    RGBQUAD        FAR *pRgb;

    if (!lpbi)
	return NULL;

    nNumColors = DibNumColors(lpbi);

    if (nNumColors)
    {
        pRgb = DibColors(lpbi);
	pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));

        if (!pPal)
            goto exit;

        pPal->palNumEntries = nNumColors;
	pPal->palVersion    = PALVERSION;

        for (i = 0; i < (int)nNumColors; i++)
	{
	    pPal->palPalEntry[i].peRed	 = pRgb->rgbRed;
	    pPal->palPalEntry[i].peGreen = pRgb->rgbGreen;
	    pPal->palPalEntry[i].peBlue  = pRgb->rgbBlue;
	    pPal->palPalEntry[i].peFlags = (BYTE)0;
            pRgb++;
        }

        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }

exit:
    return hpal;
}

/*
 *  ReadDibBitmapInfo()
 *
 *  Will read a file in DIB format and return a global HANDLE to it's
 *  BITMAPINFO.  This function will work with both "old" and "new"
 *  bitmap formats, but will allways return a "new" BITMAPINFO
 *
 */
HDIB ReadDibBitmapInfo(int fh)
{
    DWORD     off;
    HDIB    hbi = NULL;
    int       size;
    int       i;
    WORD      nNumColors;

    RGBQUAD FAR       *pRgb;
    BITMAPINFOHEADER   bi;
    BITMAPCOREHEADER   bc;
    LPBITMAPINFOHEADER lpbi;
    BITMAPFILEHEADER   bf;

    if (fh == -1)
        return NULL;

    off = _llseek(fh,0L,SEEK_CUR);

    if (sizeof(bf) != _lread(fh,(LPVOID)&bf,sizeof(bf)))
        return NULL;

    /*
     *  do we have a RC HEADER?
     */
    if (!ISDIB(bf.bfType))
    {
        bf.bfOffBits = 0L;
        _llseek(fh,off,SEEK_SET);
    }

    if (sizeof(bi) != _lread(fh,(LPVOID)&bi,sizeof(bi)))
        return NULL;

    nNumColors = DibNumColors(&bi);

    /*
     *  what type of bitmap info is this?
     */
    switch (size = (int)bi.biSize)
    {
        default:
        case sizeof(BITMAPINFOHEADER):
            break;

        case sizeof(BITMAPCOREHEADER):
            bc = *(BITMAPCOREHEADER*)&bi;
            bi.biSize               = sizeof(BITMAPINFOHEADER);
            bi.biWidth              = (DWORD)bc.bcWidth;
            bi.biHeight             = (DWORD)bc.bcHeight;
            bi.biPlanes             =  (WORD)bc.bcPlanes;
            bi.biBitCount           =  (WORD)bc.bcBitCount;
            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = nNumColors;
            bi.biClrImportant       = nNumColors;

            _llseek(fh,(LONG)sizeof(BITMAPCOREHEADER)-sizeof(BITMAPINFOHEADER),SEEK_CUR);

            break;
    }

    /*
     *	fill in some default values!
     */
    if (bi.biSizeImage == 0)
        bi.biSizeImage = DibSizeImage(&bi);

    if (bi.biClrUsed == 0)
	bi.biClrUsed = DibNumColors(&bi);

    hbi = GlobalAlloc(GMEM_MOVEABLE,(LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));

    if (!hbi)
        return NULL;

    lpbi = (VOID FAR *)GlobalLock(hbi);
    *lpbi = bi;

    if (lpbi->biSize > sizeof(bi)) {
	if (_lread(fh,
		   (LPBYTE) lpbi + sizeof(bi),
		   (UINT)lpbi->biSize - sizeof(bi))
			 != (lpbi->biSize - sizeof(bi))) {
	    GlobalFree(hbi);
	    return NULL;
	}
    }
    
    pRgb = DibColors(lpbi);

    if (nNumColors)
    {
        if (size == sizeof(BITMAPCOREHEADER))
        {
            /*
             * convert a old color table (3 byte entries) to a new
             * color table (4 byte entries)
             */
            _lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBTRIPLE));

            for (i=nNumColors-1; i>=0; i--)
            {
                RGBQUAD rgb;

                rgb.rgbRed      = ((RGBTRIPLE FAR *)pRgb)[i].rgbtRed;
                rgb.rgbBlue     = ((RGBTRIPLE FAR *)pRgb)[i].rgbtBlue;
                rgb.rgbGreen    = ((RGBTRIPLE FAR *)pRgb)[i].rgbtGreen;
                rgb.rgbReserved = (BYTE)0;

                pRgb[i] = rgb;
            }
        }
        else
        {
            _lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBQUAD));
        }
    }

    if (bf.bfOffBits != 0L)
        _llseek(fh,off + bf.bfOffBits,SEEK_SET);

    return hbi;
}

/*
 *  DibFromBitmap()
 *
 *  Will create a global memory block in DIB format that represents the DDB
 *  passed in
 *
 */
HDIB DibFromBitmap(HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal, WORD wUsage)
{
    BITMAP               bm;
    BITMAPINFOHEADER     bi;
    BITMAPINFOHEADER FAR *lpbi;
    DWORD                dwLen;
    int                  nColors;
    HDIB                 hdib;
    HANDLE               h;
    HDC                  hdc;

    if (!hbm)
        return NULL;

    if (hpal == NULL)
        hpal = GetStockObject(DEFAULT_PALETTE);

    GetObject(hbm,sizeof(bm),(LPSTR)&bm);
    GetObject(hpal,sizeof(nColors),(LPSTR)&nColors);

    if (biBits == 0)
        biBits = bm.bmPlanes * bm.bmBitsPixel;

    bi.biSize               = sizeof(BITMAPINFOHEADER);
    bi.biWidth              = bm.bmWidth;
    bi.biHeight             = bm.bmHeight;
    bi.biPlanes             = 1;
    bi.biBitCount           = biBits;
    bi.biCompression        = biStyle;
    bi.biSizeImage          = 0;
    bi.biXPelsPerMeter      = 0;
    bi.biYPelsPerMeter      = 0;
    bi.biClrUsed            = 0;
    bi.biClrImportant       = 0;

    dwLen  = bi.biSize + DibNumColors(&bi) * sizeof(RGBQUAD);

    hdc = CreateCompatibleDC(NULL);
    hpal = SelectPalette(hdc,hpal,FALSE);
    RealizePalette(hdc);  // why is this needed on a MEMORY DC? GDI bug??

    hdib = GlobalAlloc(GMEM_MOVEABLE,dwLen);

    if (!hdib)
        goto exit;

    lpbi = (VOID FAR *)GlobalLock(hdib);

    *lpbi = bi;

    /*
     *  call GetDIBits with a NULL lpBits param, so it will calculate the
     *  biSizeImage field for us
     */
    GetDIBits(hdc, hbm, 0, (WORD)bi.biHeight,
        NULL, (LPBITMAPINFO)lpbi, wUsage);

    bi = *lpbi;

    /*
     * If the driver did not fill in the biSizeImage field, fill it in
     * based on the width, height and bit depth.
     */
    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = (DWORD)WIDTHBYTES(bm.bmWidth * biBits) * bm.bmHeight;

        if (biStyle != BI_RGB)
            bi.biSizeImage = (bi.biSizeImage * 3) / 2;
    }

    /*
     *  realloc the buffer big enough to hold all the bits
     */
    dwLen = bi.biSize + DibNumColors(&bi) * sizeof(RGBQUAD) + bi.biSizeImage;

    if (h = GlobalReAlloc(hdib,dwLen,0))
    {
        hdib = h;
    }
    else
    {
        GlobalFree(hdib);
        hdib = NULL;
        goto exit;
    }

    /*
     *  call GetDIBits with a NON-NULL lpBits param, and actualy get the
     *  bits this time
     */
    lpbi = (VOID FAR *)GlobalLock(hdib);

    GetDIBits(hdc, hbm, 0, (WORD)bi.biHeight,
        DibPtr(lpbi),(LPBITMAPINFO)lpbi, wUsage);

    bi = *lpbi;
    lpbi->biClrUsed = DibNumColors(lpbi);

exit:
    SelectPalette(hdc,hpal,FALSE);
    DeleteDC(hdc);
    return hdib;
}

/*
 *  BitmapFromDib()
 *
 *  Will create a DDB (Device Dependent Bitmap) given a global handle to
 *  a memory block in CF_DIB format
 *
 */
HBITMAP BitmapFromDib(HDIB hdib, HPALETTE hpal, WORD wUsage)
{
    LPBITMAPINFOHEADER lpbi;
    HPALETTE    hpalT;
    HDC         hdc;
    HBITMAP     hbm;

    if (!hdib)
        return NULL;

    lpbi = (LPVOID)GlobalLock(hdib);

    if (!lpbi)
	return NULL;

    hdc = GetDC(NULL);

    if (hpal)
    {
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);
    }

    hbm = CreateDIBitmap(hdc,(LPBITMAPINFOHEADER)lpbi,(LONG)CBM_INIT,
                DibPtr(lpbi),(LPBITMAPINFO)lpbi,wUsage);

    if (hpal && hpalT)
        SelectPalette(hdc,hpalT,FALSE);

    ReleaseDC(NULL,hdc);

    return hbm;
}

/*
 *  SetDibUsage(hdib,hpal,wUsage)
 *
 *  Modifies the color table of the passed DIB for use with the wUsage
 *  parameter specifed.
 *
 *  if wUsage is DIB_PAL_COLORS the DIB color table is set to 0-256
 *  if wUsage is DIB_RGB_COLORS the DIB color table is set to the RGB values
 *      in the passed palette
 *
 */
BOOL SetDibUsage(HDIB hdib, HPALETTE hpal,WORD wUsage)
{
    LPBITMAPINFOHEADER lpbi;
    PALETTEENTRY       ape[MAXPALETTE];
    RGBQUAD FAR *      pRgb;
    WORD FAR *         pw;
    int                nColors;
    int                n;

    if (hpal == NULL)
        hpal = GetStockObject(DEFAULT_PALETTE);

    if (!hdib)
        return FALSE;

    lpbi = (VOID FAR *)GlobalLock(hdib);

    if (!lpbi)
	return FALSE;

    nColors = DibNumColors(lpbi);

    if (nColors > 0)
    {
        pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);

        switch (wUsage)
        {
            //
            // Set the DIB color table to palette indexes
            //
            case DIB_PAL_COLORS:
                for (pw = (WORD FAR*)pRgb,n=0; n<nColors; n++,pw++)
                    *pw = n;
                break;

            //
            // Set the DIB color table to RGBQUADS
            //
            default:
            case DIB_RGB_COLORS:
                nColors = min(nColors,MAXPALETTE);

                GetPaletteEntries(hpal,0,nColors,ape);

                for (n=0; n<nColors; n++)
                {
                    pRgb[n].rgbRed      = ape[n].peRed;
                    pRgb[n].rgbGreen    = ape[n].peGreen;
                    pRgb[n].rgbBlue     = ape[n].peBlue;
                    pRgb[n].rgbReserved = 0;
                }
                break;
        }
    }
    return TRUE;
}

HDIB CreateDib(int bits, int dx, int dy)
{
    HDIB                hdib;
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  lpbi;
    DWORD FAR *         pRgb;
    int                 i;

    //
    // These are the standard VGA colors, we will be stuck with until the
    // end of time!
    //
    static DWORD CosmicColors[16] = {
         0x00000000        // 0000  black
        ,0x00800000        // 0001  dark red
        ,0x00008000        // 0010  dark green
        ,0x00808000        // 0011  mustard
        ,0x00000080        // 0100  dark blue
        ,0x00800080        // 0101  purple
        ,0x00008080        // 0110  dark turquoise
        ,0x00C0C0C0        // 1000  gray
        ,0x00808080        // 0111  dark gray
        ,0x00FF0000        // 1001  red
        ,0x0000FF00        // 1010  green
        ,0x00FFFF00        // 1011  yellow
        ,0x000000FF        // 1100  blue
        ,0x00FF00FF        // 1101  pink (magenta)
        ,0x0000FFFF        // 1110  cyan
        ,0x00FFFFFF        // 1111  white
        };

    bi.biSize           = sizeof(BITMAPINFOHEADER);
    bi.biPlanes         = 1;
    bi.biBitCount       = bits;
    bi.biWidth          = dx;
    bi.biHeight         = dy;
    bi.biCompression    = BI_RGB;
    bi.biSizeImage      = 0;
    bi.biXPelsPerMeter  = 0;
    bi.biYPelsPerMeter  = 0;
    bi.biClrUsed	= 0;
    bi.biClrImportant   = 0;
    bi.biClrUsed	= DibNumColors(&bi);

    hdib = GlobalAlloc(GMEM_MOVEABLE,sizeof(BITMAPINFOHEADER) +
                + (long)bi.biClrUsed * sizeof(RGBQUAD)
                + (long)DIBWIDTHBYTES(bi) * (long)dy);

    if (hdib)
    {
        lpbi  = (LPVOID)GlobalLock(hdib);
        *lpbi = bi;

        pRgb  = (LPVOID)DibColors(lpbi);

        //
        //  setup the color table
        //
        if (bits == 1)
        {
            pRgb[0] = CosmicColors[0];
            pRgb[1] = CosmicColors[15];
        }
        else
        {
            for (i=0; i<16; i++)
                pRgb[i] = CosmicColors[i%16];
        }
    }

    return hdib;
}

/*
 *  StretchDib()
 *
 *  draws a bitmap in CF_DIB format, using StretchDIBits()
 *
 *  takes the same parameters as StretchBlt()
 */
BOOL StretchDib(HDC hdc, int x, int y, int dx, int dy, HDIB hdib, HPALETTE hpal, int x0, int y0, int dx0, int dy0, LONG rop, WORD wUsage)
{
    LPBITMAPINFOHEADER  lpbi;
    HPALETTE            hpalT;
    BOOL                f;

    if (!hdib)
        return PatBlt(hdc,x,y,dx,dy,rop);

    if (hpal)
    {
        hpalT = SelectPalette(hdc, hpal, FALSE);
        RealizePalette(hdc);
    }

    lpbi = (LPVOID)GlobalLock(hdib);

    if (!lpbi)
        return FALSE;

    if (dx0 == -1 && dy0 == -1)
    {
        dx0 = (int)lpbi->biWidth;
        dy0 = (int)lpbi->biHeight;
    }

    if (dx < 0 && dy < 0)
    {
        dx = dx0 * (-dx);
        dy = dy0 * (-dy);
    }

    f = StretchDIBits(hdc,x,y,dx,dy,x0,y0,dx0,dy0,
        DibPtr(lpbi), (LPBITMAPINFO)lpbi,wUsage,rop) > 0;

    if (hpal && hpalT)
        SelectObject(hdc, hpalT);

    return f;
}
