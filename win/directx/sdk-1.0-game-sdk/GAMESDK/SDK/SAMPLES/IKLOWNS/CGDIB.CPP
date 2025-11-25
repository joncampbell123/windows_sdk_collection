/*===========================================================================*\
|
|  File:        cgdib.cpp
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/


#ifdef __WATCOMC__
#include <windows.h>
#endif
#include <windowsx.h>
#include "cgglobl.h"
#include "cgrsrce.h"
#include "cgscreen.h"
#include "cgexcpt.h"
#include "cgdebug.h"
#include "cgdib.h"

/* Macro to determine to round off the given value to the closest byte */
#define WIDTHBYTES(i)   ((i+31)/32*4)

#define BFT_BITMAP 0x4d42           /* 'BM' */

/* macro to determine if resource is a DIB */
#define ISDIB(bft) ((bft) == BFT_BITMAP)

/* Macros to display/remove hourglass cursor for lengthy operations */
#define StartWait() hcurSave = SetCursor(hCursor=LoadCursor(NULL,IDC_WAIT));
#define EndWait()   SetCursor(hCursor=hcurSave)

#define PALVERSION                0x300
#define MAXPALETTE      256      /* max. # supported palette entries */

WORD                  PaletteSize (VOID FAR * pv);
WORD                  DibNumColors (VOID FAR * pv);
HPALETTE         CreateDibPalette (HANDLE hdib);
HPALETTE         CreateBIPalette (LPBITMAPINFOHEADER lpbi);
HANDLE            DibFromBitmap (HBITMAP hbm, DWORD biStyle, WORD biBits, HPALETTE hpal);
HBITMAP           BitmapFromDib (HANDLE hdib, HPALETTE hpal);
BOOL                  DibInfo (HANDLE hdib,LPBITMAPINFOHEADER lpbi);
HANDLE            ReadDibBitmapInfo (int fh);
VOID    ReadBitMapFileHeaderandConvertToDwordAlign(HFILE fh, LPBITMAPFILEHEADER pbf, LPDWORD lpdwoff);

static HCURSOR hCursor;
static HCURSOR hcurSave;

/*---------------------------------------------------------------------------*\
|
|       Class CGameDIB
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameDIB::CGameDIB(
    ) : mImageValid( FALSE ),
        mpBitmapInfo( NULL ),
        mpBits( NULL ),
        mpFileName( NULL ),
        mhBitmap( NULL )
{
}

CGameDIB::CGameDIB(
    LPSTR pFileName
    ) : mImageValid( FALSE ),
        mpBitmapInfo( NULL ),
        mpBits( NULL ),
        mpFileName( NULL ),
        mhBitmap( NULL )
{
    if (pFileName)
    {
        mpFileName = new char[lstrlen(pFileName)+1];
        lstrcpy( mpFileName, pFileName );
    
        // Load the DIB into memory.
        OpenDIB(pFileName);

        // Set the flag to indicate that we now have a valid image in memory.
        // NOTE: OpenDIB will throw exception if there's a problem, so we can
        // assume here that image is valid.
        mImageValid = TRUE;
    }
}

// construct a new DIB section in memory of given size, palette
CGameDIB::CGameDIB(
    int width,
    int height,
    HPALETTE hPal
    ) : mImageValid( FALSE ),
        mpBitmapInfo( NULL ),
        mpBits( NULL ),
        mpFileName( NULL ),
        mhBitmap( NULL )
{
    PALETTEENTRY entries[256];
    LPBITMAPINFO pBI = (LPBITMAPINFO) GlobalAllocPtr( GHND, sizeof( BITMAPINFO ) );

    if (!pBI)
        throw CGameResourceError();

    // !!!NOTE: we're assuming 8 BPP
    pBI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pBI->bmiHeader.biWidth = width;
    pBI->bmiHeader.biHeight = height;   // make it top-down
    pBI->bmiHeader.biPlanes = 1;
    pBI->bmiHeader.biBitCount = 8;

    pBI->bmiHeader.biCompression = BI_RGB;
    pBI->bmiHeader.biXPelsPerMeter = 0;
    pBI->bmiHeader.biYPelsPerMeter = 0;
    pBI->bmiHeader.biClrUsed = 256;
    pBI->bmiHeader.biClrImportant = 256;

    pBI->bmiHeader.biSizeImage = WIDTHBYTES(width*8) * height;

    /* Calculate the memory needed to hold the DIB */
    DWORD dwBits = pBI->bmiHeader.biSizeImage;
    DWORD dwLen  = pBI->bmiHeader.biSize + (DWORD)PaletteSize (pBI);

    /* Try to increase the size of the bitmap info. buffer to hold the DIB */
    pBI = (LPBITMAPINFO) GlobalReAllocPtr(pBI, dwLen, GHND);
    if (!pBI)
    {
        throw CGameResourceError();
    }

    mpBitmapInfo = pBI;

    HDC hdc = GetDC( HWND_DESKTOP );

    if (hdc)
    {
        // now create the color table from given palette
        HPALETTE hOldPal = NULL;

        if (hPal == NULL)
            hPal = GetCurrentObject( hdc, OBJ_PAL );
        else
        {
            hOldPal = SelectPalette( hdc, hPal, FALSE );
            RealizePalette( hdc );
        }

        UINT numColors = GetSystemPaletteEntries(
                        hdc,
                        0,  // start index
                        MAXPALETTE, // num to get
                        entries
                        );



        for (UINT i=0; i<numColors; i++)
        {
            pBI->bmiColors[i].rgbRed = entries[i].peRed;
            pBI->bmiColors[i].rgbGreen = entries[i].peGreen;
            pBI->bmiColors[i].rgbBlue = entries[i].peBlue;
        }

        mhBitmap = CreateDIBSection(
            hdc,
            mpBitmapInfo,
            DIB_RGB_COLORS,
            (void**) &mpBits,
            NULL,
            0
            );

        mImageValid = (mhBitmap != NULL);

        if (hOldPal)
        {
            SelectPalette( hdc, hOldPal, FALSE );
            RealizePalette( hdc );
        }

        ReleaseDC( HWND_DESKTOP, hdc );
    }
}

/*---------------------------------------------------------------------------*\
|   ~CGameDIB
\*---------------------------------------------------------------------------*/
CGameDIB::~CGameDIB()
{
    delete[] mpFileName;

    if (mhBitmap)
    {
        DeleteObject( mhBitmap );
    }

    if (mpBitmapInfo)
    {
        GlobalFreePtr( mpBitmapInfo );
    }
}   

/*---------------------------------------------------------------------------*\
|   GetWidth()
\*---------------------------------------------------------------------------*/
int
CGameDIB::GetWidth()
{
    if (mpBitmapInfo)
        return mpBitmapInfo->bmiHeader.biWidth;

    return 0;
}

/*---------------------------------------------------------------------------*\
|   GetHeight()
\*---------------------------------------------------------------------------*/
int
CGameDIB::GetHeight()
{
    if (mpBitmapInfo)
        return mpBitmapInfo->bmiHeader.biHeight;

    return 0;
}

/*---------------------------------------------------------------------------*\
|   CreatePalette
\*---------------------------------------------------------------------------*/
HPALETTE
CGameDIB::CreatePalette()
{
    return CreateBIPalette( (PBITMAPINFOHEADER) mpBitmapInfo );
}

/*---------------------------------------------------------------------------*\
|   OpenDIB
\*---------------------------------------------------------------------------*/
void
CGameDIB::OpenDIB (char* pFileName)
{
    HFILE       fh;
    BITMAPINFOHEADER    bi;
    LPBITMAPINFOHEADER  lpbi;
    DWORD       dwLen = 0;
    DWORD       dwBits;
    HANDLE      hdib;
    HANDLE              h;
    OFSTRUCT        of;

    if (!pFileName)
        return;

    /* Open the file and read the DIB information */
    fh = OpenFile(pFileName, &of, (UINT)OF_READ);
    if (fh == -1)
    {
        DB_LOG(DB_PROBLEM, "DEBUG: OpenFile failed in OpenDIB");
        throw CGameException(
                    IDS_INVALID_FILE
                   );
    }

    hdib = ReadDibBitmapInfo(fh);

    DWORD offset = SetFilePointer( (HANDLE) fh, 0, NULL, FILE_CURRENT );

    if (!hdib)
    {
        DB_LOG(DB_PROBLEM, "DEBUG: Invalid DIB file in OpenDIB");
        throw CGameException(
                    IDS_INVALID_FILE
                   );
    }

    DibInfo(hdib,&bi);

#ifdef ConvertToPaletteRelative
     /*  Convert the DIB color table to palette relative indexes, so
      *  SetDIBits() and SetDIBitsToDevice() can avoid color matching.
      *  We can do this because the palette we realize is identical
      *  to the color table of the bitmap, ie the indexes match 1 to 1
      *
      *  Now that the DIB color table is palette indexes not RGB values
      *  we must use DIB_PAL_COLORS as the wUsage parameter to SetDIBits()
      */
    lpbi = (VOID FAR *) GlobalLock(hbiCurrent);
    if (lpbi->biBitCount != 24) {
        fPalColors = TRUE;

        pw = (WORD FAR *) ((LPSTR) lpbi + lpbi->biSize);

        for (i = 0;  i < (int) lpbi->biClrUsed;  i++)
            *pw++ = (WORD) i;
        }
    GlobalUnlock(hbiCurrent);
#endif

    /* Calculate the memory needed to hold the DIB */
    dwBits = bi.biSizeImage;
    dwLen  = bi.biSize + (DWORD)PaletteSize (&bi);

    /* Try to increase the size of the bitmap info. buffer to hold the DIB */
    h = GlobalReAlloc(hdib, dwLen, GHND);
    if (!h)
    {
        GlobalFree(hdib);
        hdib = NULL;
        throw CGameResourceError();
    }
    else
        hdib = h;

    /* Read in the bits */
    if (hdib)
    {
        lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);
        mpBitmapInfo = (LPBITMAPINFO) lpbi;

        HDC hdc = GetDC( HWND_DESKTOP );

        if (hdc)
        {
            mhBitmap = CreateDIBSection(
                hdc,
                mpBitmapInfo,
                DIB_RGB_COLORS,
                (void**) &mpBits,
                NULL,
                0
                );

            ReadFile(
                    (HANDLE)fh,
                    mpBits,
                    dwBits,
                    &dwLen,
                    NULL
                    );

            ReleaseDC( HWND_DESKTOP, hdc );
        }
    }
    
    CloseHandle((HANDLE)fh);
}

/*---------------------------------------------------------------------------*\
|   GetPixelAddress()
\*---------------------------------------------------------------------------*/
LPBYTE  // return address of the pixel -- NULL if out of range
CGameDIB::GetPixelAddress(
    int x,
    int y
    )
{
    long width;

    // check params
    if (( x < 0 ) || ( y < 0 ) || ( x >= GetWidth() ) || ( y >= GetHeight() ))
    {
        return NULL;
    }

    // round up to DWORD boundary
    width = (GetWidth() + 3) & ~3;
    return mpBits + (long)(GetHeight()-y-1) * width + (long)x;
}

/*---------------------------------------------------------------------------*\
|   GetPixelAddress()
\*---------------------------------------------------------------------------*/
COLORREF
CGameDIB::GetPixelColor(
    int x,
    int y
    )
{
    LPBYTE pPixel = GetPixelAddress( x, y );

    if (pPixel)
    {
        RGBQUAD quad = mpBitmapInfo->bmiColors[*pPixel];

        return PALETTERGB( quad.rgbRed, quad.rgbGreen, quad.rgbBlue );
    }

    // else
    return 0;   // invalid params
}

/*---------------------------------------------------------------------------*\
|   GetColorTable()
\*---------------------------------------------------------------------------*/
UINT    // return the number of entries copied
CGameDIB::GetColorTable(UINT start, UINT count, RGBQUAD* pArray)
{
    UINT result = 0;

    if (mpBitmapInfo)
    {
        for (int i=start; ( i < DibNumColors(mpBitmapInfo)) && (result < count); i++)
        {
            pArray[result] = mpBitmapInfo->bmiColors[i];
            ++result;
        }
    }

    return result;
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : DibInfo(HANDLE hbi,LPBITMAPINFOHEADER lpbi)            *
 *                                      *
 *  PURPOSE    : Retrieves the DIB info associated with a CF_DIB        *
 *       format memory block.                       *
 *                                      *
 *  RETURNS    : TRUE  - if successful.                     *
 *       FALSE - otherwise                      *
 *                                      *
 ****************************************************************************/
BOOL DibInfo (
     HANDLE hbi,
     LPBITMAPINFOHEADER lpbi)
{
    if (hbi)
    {
        *lpbi = *(LPBITMAPINFOHEADER)GlobalLock (hbi);

        /* fill in the default fields */
        if (lpbi->biSize != sizeof (BITMAPCOREHEADER))
        {
            if (lpbi->biSizeImage == 0L)
                lpbi->biSizeImage = WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) * lpbi->biHeight;

            if (lpbi->biClrUsed == 0L)
                lpbi->biClrUsed = DibNumColors (lpbi);
        }
        GlobalUnlock (hbi);
        return TRUE;
    }
    return FALSE;
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : CreateBIPalette(LPBITMAPINFOHEADER lpbi)           *
 *                                      *
 *  PURPOSE    : Given a Pointer to a BITMAPINFO struct will create a       *
 *       a GDI palette object from the color table.         *
 *                                      *
 *  RETURNS    : A handle to the palette.                   *
 *                                      *
 ****************************************************************************/
HPALETTE CreateBIPalette (LPBITMAPINFOHEADER lpbi)
{
     LOGPALETTE          *pPal;
     HPALETTE            hpal = NULL;
     WORD                nNumColors;
     BYTE                red;
     BYTE                green;
     BYTE                blue;
     WORD                i;
     RGBQUAD        FAR *pRgb;

    if (!lpbi)
        return NULL;

    if (lpbi->biSize != sizeof(BITMAPINFOHEADER))
        return NULL;

    /* Get a pointer to the color table and the number of colors in it */
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
    nNumColors = DibNumColors(lpbi);

    if (nNumColors)
    {
        /* Allocate for the logical palette structure */
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        /* Fill in the palette entries from the DIB color table and
        * create a logical color palette.
        */

        // keep index 0 black, index 255 white to keep Windows happy
        pPal->palPalEntry[0].peRed   = 0;
        pPal->palPalEntry[0].peGreen = 0;
        pPal->palPalEntry[0].peBlue  = 0;
        pPal->palPalEntry[0].peFlags = PC_EXPLICIT;

        for (i = 1; i < (nNumColors-1); i++)
        {
            pPal->palPalEntry[i].peRed   = pRgb[i].rgbRed;
            pPal->palPalEntry[i].peGreen = pRgb[i].rgbGreen;
            pPal->palPalEntry[i].peBlue  = pRgb[i].rgbBlue;
            pPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
        }

        if (nNumColors == 256)
        {
            pPal->palPalEntry[255].peRed   = 255;   // PC_EXPLICIT treats number as a long
            pPal->palPalEntry[255].peGreen = 0;
            pPal->palPalEntry[255].peBlue  = 0;
            pPal->palPalEntry[255].peFlags = PC_EXPLICIT;
        }

        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }
    else if (lpbi->biBitCount == 24)
    {
        /* A 24 bitcount DIB has no color table entries so, set the number of
        * to the maximum value (256).
        */
        nNumColors = MAXPALETTE;
        pPal = (LOGPALETTE*)LocalAlloc(LPTR,sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
        if (!pPal)
            return NULL;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        red = green = blue = 0;

        /* Generate 256 (= 8*8*4) RGB combinations to fill the palette
        * entries.
        */
        for (i = 0; i < pPal->palNumEntries; i++)
        {
            pPal->palPalEntry[i].peRed   = red;
            pPal->palPalEntry[i].peGreen = green;
            pPal->palPalEntry[i].peBlue  = blue;
            pPal->palPalEntry[i].peFlags = (BYTE)0;

            if (!(red += 32))
                if (!(green += 32))
                    blue += 64;
        }
        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
    }
    return hpal;
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : CreateDibPalette(HANDLE hbi)                   *
 *                                      *
 *  PURPOSE    : Given a Global HANDLE to a BITMAPINFO Struct           *
 *       will create a GDI palette object from the color table.     *
 *       (BITMAPINFOHEADER format DIBs only)                     *
 *                                      *
 *  RETURNS    : A handle to the palette.                   *
 *                                      *
 ****************************************************************************/
HPALETTE CreateDibPalette (HANDLE hbi)
{
    HPALETTE hpal;

    if (!hbi)
        return NULL;
    hpal = CreateBIPalette((LPBITMAPINFOHEADER)GlobalLock(hbi));
    GlobalUnlock(hbi);
    return hpal;
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : ReadDibBitmapInfo(int fh)                  *
 *                                      *
 *  PURPOSE    : Will read a file in DIB format and return a global HANDLE  *
 *       to it's BITMAPINFO.  This function will work with both     *
 *       "old" (BITMAPCOREHEADER) and "new" (BITMAPINFOHEADER)      *
 *       bitmap formats, but will always return a "new" BITMAPINFO  *
 *                                      *
 *  RETURNS    : A handle to the BITMAPINFO of the DIB in the file.     *
 *                                      *
 ****************************************************************************/
HANDLE ReadDibBitmapInfo (INT fh)
{
    DWORD     off;
    HANDLE    hbi = NULL;
    INT       size;
    INT       i;
    WORD      nNumColors;

    RGBQUAD FAR       *pRgb;
    BITMAPINFOHEADER   bi;
    BITMAPCOREHEADER   bc;
    LPBITMAPINFOHEADER lpbi;
    BITMAPFILEHEADER   bf;
    DWORD          dwWidth = 0;
    DWORD          dwHeight = 0;
    WORD           wPlanes, wBitCount;

    if (fh == -1)
        return NULL;
#ifdef FIXDWORDALIGNMENT
    /* Reset file pointer and read file header */
    off = _llseek(fh, 0L, (UINT)SEEK_CUR);
    if ((SIZEOF_BITMAPFILEHEADER_PACKED)  != _lread(fh, (LPSTR)&bf, (UINT)sizeof (SIZEOF_BITMAPFILEHEADER_PACKED)))
        return FALSE;
#else
    ReadBitMapFileHeaderandConvertToDwordAlign(fh, &bf, &off);
    /* at this point we have read the file into bf*/
#endif

    /* Do we have a RC HEADER? */
    if (!ISDIB (bf.bfType))
    {
        bf.bfOffBits = 0L;
        SetFilePointer((HANDLE)fh, off, NULL, FILE_BEGIN); /*seek back to beginning of file*/
    }
    if (!ReadFile((HANDLE)fh, (LPSTR)&bi, (UINT)sizeof(bi), &dwWidth, NULL) ||
        sizeof (bi) != dwWidth)
        return FALSE;

    nNumColors = DibNumColors (&bi);

    /* Check the nature (BITMAPINFO or BITMAPCORE) of the info. block
     * and extract the field information accordingly. If a BITMAPCOREHEADER,
     * transfer it's field information to a BITMAPINFOHEADER-style block
     */
    switch (size = (INT)bi.biSize)
    {
        case sizeof (BITMAPINFOHEADER):
            break;

        case sizeof (BITMAPCOREHEADER):
            bc = *(BITMAPCOREHEADER*)&bi;

            dwWidth   = (DWORD)bc.bcWidth;
            dwHeight  = (DWORD)bc.bcHeight;
            wPlanes   = bc.bcPlanes;
            wBitCount = bc.bcBitCount;

            bi.biSize           = sizeof(BITMAPINFOHEADER);
            bi.biWidth          = dwWidth;
            bi.biHeight         = dwHeight;
            bi.biPlanes         = wPlanes;
            bi.biBitCount       = wBitCount;

            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = nNumColors;
            bi.biClrImportant       = nNumColors;

            _llseek(fh, (LONG)sizeof (BITMAPCOREHEADER) - sizeof (BITMAPINFOHEADER), (UINT)SEEK_CUR);
            break;

        default:
            /* Not a DIB! */
            return NULL;
    }

    /* Fill in some default values if they are zero */
    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = WIDTHBYTES ((DWORD)bi.biWidth * bi.biBitCount)
               * bi.biHeight;
    }
    if (bi.biClrUsed == 0)
        bi.biClrUsed = DibNumColors(&bi);

    /* Allocate for the BITMAPINFO structure and the color table. */
    hbi = GlobalAlloc (GHND, (LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));
    if (!hbi)
        return NULL;
    lpbi = (LPBITMAPINFOHEADER)GlobalLock (hbi);
    *lpbi = bi;

    /* Get a pointer to the color table */
    pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + bi.biSize);
    if (nNumColors)
    {
        DWORD bytesRead;

        if (size == sizeof(BITMAPCOREHEADER))
        {
            /* Convert a old color table (3 byte RGBTRIPLEs) to a new
             * color table (4 byte RGBQUADs)
             */
            ReadFile((HANDLE)fh, (LPSTR)pRgb, (UINT)nNumColors * sizeof(RGBTRIPLE), &bytesRead, NULL);

            for (i = nNumColors - 1; i >= 0; i--)
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
            ReadFile((HANDLE)fh, (LPSTR)pRgb, (UINT)nNumColors * sizeof(RGBQUAD), &bytesRead, NULL);
    }

    if (bf.bfOffBits != 0L)
    {
        SetFilePointer((HANDLE)fh, off + bf.bfOffBits, NULL, FILE_BEGIN);
    }
    GlobalUnlock(hbi);
    return hbi;
}

/****************************************************************************
 *                                      *
 *  FUNCTION   :  PaletteSize(VOID FAR * pv)                    *
 *                                      *
 *  PURPOSE    :  Calculates the palette size in bytes. If the info. block  *
 *        is of the BITMAPCOREHEADER type, the number of colors is  *
 *        multiplied by 3 to give the palette size, otherwise the   *
 *        number of colors is multiplied by 4.                              *
 *                                      *
 *  RETURNS    :  Palette size in number of bytes.              *
 *                                      *
 ****************************************************************************/
WORD PaletteSize (VOID FAR * pv)
{
    LPBITMAPINFOHEADER lpbi;
    WORD           NumColors;

    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = DibNumColors(lpbi);

    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (WORD)(NumColors * sizeof(RGBTRIPLE));
    else
        return (WORD)(NumColors * sizeof(RGBQUAD));
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : DibNumColors(VOID FAR * pv)                    *
 *                                      *
 *  PURPOSE    : Determines the number of colors in the DIB by looking at   *
 *       the BitCount filed in the info block.              *
 *                                      *
 *  RETURNS    : The number of colors in the DIB.               *
 *                                      *
 ****************************************************************************/
WORD DibNumColors (VOID FAR * pv)
{
    INT         bits;
    LPBITMAPINFOHEADER  lpbi;
    LPBITMAPCOREHEADER  lpbc;

    lpbi = ((LPBITMAPINFOHEADER)pv);
    lpbc = ((LPBITMAPCOREHEADER)pv);

    /*  With the BITMAPINFO format headers, the size of the palette
     *  is in biClrUsed, whereas in the BITMAPCORE - style headers, it
     *  is dependent on the bits per pixel ( = 2 raised to the power of
     *  bits/pixel).
     */
    if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
    {
        if (lpbi->biClrUsed != 0)
            return (WORD)lpbi->biClrUsed;
        bits = lpbi->biBitCount;
    }
    else
        bits = lpbc->bcBitCount;

    switch (bits)
    {
        case 1:
            return 2;
        case 4:
            return 16;
        case 8:
            return 256;
        default:
            /* A 24 bitcount DIB has no color table */
            return 0;
    }
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : BitmapFromDib(HANDLE hdib, HPALETTE hpal)          *
 *                                      *
 *  PURPOSE    : Will create a DDB (Device Dependent Bitmap) given a global *
 *       handle to a memory block in CF_DIB format          *
 *                                      *
 *  RETURNS    : A handle to the DDB.                       *
 *                                      *
 ****************************************************************************/

HBITMAP BitmapFromDib (
     HANDLE    hdib,
     HPALETTE   hpal)
{
    LPBITMAPINFOHEADER  lpbi;
    HPALETTE        hpalT;
    HDC         hdc;
    HBITMAP     hbm;

    StartWait();

    if (!hdib)
        return NULL;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib);

    if (!lpbi)
        return NULL;

    hdc = GetDC(NULL);

    if (hpal)
    {
        hpalT = SelectPalette(hdc,hpal,FALSE);
        RealizePalette(hdc);     // GDI Bug...????
    }

    hbm = CreateDIBitmap(hdc,
             (LPBITMAPINFOHEADER)lpbi,
             (LONG)CBM_INIT,
             (LPBYTE)lpbi + lpbi->biSize + PaletteSize(lpbi), //cast changed for C++
             (LPBITMAPINFO)lpbi,
             DIB_RGB_COLORS );

    if (hpal)
        SelectPalette(hdc,hpalT,FALSE);

    ReleaseDC(NULL,hdc);
    GlobalUnlock(hdib);

    EndWait();

    return hbm;
}

/****************************************************************************
 *                                      *
 *  FUNCTION   : ReadBitMapFileHeaderandConvertToDwordAlign(HFILE fh, LPBITMAPFILEHEADER pbf)
 *                                      *
 *  PURPOSE    : read file header (which is packed) and convert into unpacked BITMAPFILEHEADER strucutre
 *                                      *
 *  RETURNS    : VOID
 *                                      *
 ****************************************************************************/

VOID ReadBitMapFileHeaderandConvertToDwordAlign(HFILE fh, LPBITMAPFILEHEADER pbf, LPDWORD lpdwoff)
{
    DWORD off;

    off = SetFilePointer((HANDLE)fh, 0L, NULL, FILE_CURRENT);
    *lpdwoff = off;

/*      BITMAPFILEHEADER STRUCUTURE is as follows
 *      BITMAPFILEHEADER
 *          WORD    bfType
 >          ....          < add WORD if packed here!
 *          DWORD   bfSize
 *          WORD    bfReserved1
 *          WORD    bfReserved2
 *          DWORD   bfOffBits
 *          This is the packed format, unpacked adds a WORD after bfType
 */

    /* read in bfType*/
    ReadFile((HANDLE)fh, (LPSTR) &pbf->bfType, sizeof(WORD), &off, NULL);
    /* read in last 3 dwords*/
    ReadFile((HANDLE)fh, (LPSTR) &pbf->bfSize, sizeof(DWORD) * 3, &off, NULL);
}
