/****************************************************************************
 *
 *  PUZZPROC.C
 *
 *  Modification of standard AVI drawing handler.
 *
 ***************************************************************************/
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
#include <windowsx.h>
#include <vfw.h>
#include "puzzle.h"

#define SZCODE char _based(_segname("_CODE"))
static SZCODE szDescription[] = "Microsoft Puzzle Draw handler";
static SZCODE szName[]        = "MS Puzzle";

#define FOURCC_AVIDraw      mmioFOURCC('P','U','Z','Z')
#define VERSION_AVIDraw     0x00010000      // 1.00

#ifndef HUGE
    #define HUGE _huge
#endif

extern PUZZLE	gPuzzle;

/***************************************************************************
 ***************************************************************************/

typedef struct {
    HDRAWDIB		hdd;

    HDC                 hdc;            // HDC to draw to
			     
    int                 xDst;           // destination rectangle
    int                 yDst;
    int                 dxDst;
    int                 dyDst;
    int                 xSrc;           // source rectangle
    int                 ySrc;
    int                 dxSrc;
    int                 dySrc;
    LPBYTE		lpBuffer;
} INSTINFO, FAR * PINSTINFO;

// static stuff in this file.
LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);
static LONG NEAR PASCAL AVIDrawOpen(ICOPEN FAR * icopen);
static LONG NEAR PASCAL AVIDrawClose(PINSTINFO pi);
static LONG NEAR PASCAL AVIDrawGetInfo(ICINFO FAR *icinfo, LONG lSize);
static LONG NEAR PASCAL AVIDrawQuery(PINSTINFO pi, LPBITMAPINFOHEADER lpbiIn);
static LONG NEAR PASCAL AVIDrawSuggestFormat(PINSTINFO pi, ICDRAWSUGGEST FAR *lpicd, LONG cbicd);
static LONG NEAR PASCAL AVIDrawBegin(PINSTINFO pi, ICDRAWBEGIN FAR *lpicd, LONG cbicd);
static LONG NEAR PASCAL AVIDraw(PINSTINFO pi, ICDRAW FAR *lpicd, LONG cbicd);
static LONG NEAR PASCAL AVIDrawEnd(PINSTINFO pi);
static LONG NEAR PASCAL AVIDrawChangePalette(PINSTINFO pi, LPBITMAPINFOHEADER lpbi);

/***************************************************************************
 ***************************************************************************/

LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2)
{
    PINSTINFO pi = (PINSTINFO)id;

    switch (uiMessage)
    {
        case DRV_LOAD:
        case DRV_FREE:
            return 1;

        /*********************************************************************
            open
        *********************************************************************/

        case DRV_OPEN:
            if (lParam2 == 0L)
                return 1;

            return AVIDrawOpen((ICOPEN FAR *)lParam2);

	case DRV_CLOSE:
            return AVIDrawClose(pi);

        /*********************************************************************
            Configure/Info messages
        *********************************************************************/

        case DRV_QUERYCONFIGURE:    // configuration from drivers applet
            return 0;

        case DRV_CONFIGURE:
            return 1;

        case ICM_CONFIGURE:
        case ICM_ABOUT:
            return ICERR_UNSUPPORTED;

        /*********************************************************************
            state messages
        *********************************************************************/

        case ICM_GETSTATE:
        case ICM_SETSTATE:
            return 0L;

        case ICM_GETINFO:
            return AVIDrawGetInfo((ICINFO FAR *)lParam1, lParam2);

        /*********************************************************************
            draw messages
        *********************************************************************/

        case ICM_DRAW_QUERY:
            return AVIDrawQuery(pi, (LPBITMAPINFOHEADER)lParam1);

	case ICM_DRAW_SUGGESTFORMAT:
	    return AVIDrawSuggestFormat(pi, (ICDRAWSUGGEST FAR *) lParam1, lParam2);

        case ICM_DRAW_BEGIN:
	    return AVIDrawBegin(pi, (ICDRAWBEGIN FAR *) lParam1, lParam2);

	case ICM_DRAW_REALIZE:
	    pi->hdc = (HDC) lParam1;
	    
	    if (!pi->hdc || !pi->hdd)
		break;

	    return DrawDibRealize(pi->hdd, pi->hdc, (BOOL) lParam2);

	case ICM_DRAW_GET_PALETTE:
	    if (!pi->hdd)
		break;

	    return (LONG) (UINT) DrawDibGetPalette(pi->hdd);
	    
        case ICM_DRAW:
            return AVIDraw(pi, (ICDRAW FAR *)lParam1, lParam2);

	case ICM_DRAW_CHANGEPALETTE:
	    return AVIDrawChangePalette(pi, (LPBITMAPINFOHEADER) lParam1);
	    	    
        case ICM_DRAW_END:
            return AVIDrawEnd(pi);

        /*********************************************************************
            standard driver messages
        *********************************************************************/

        case DRV_DISABLE:
        case DRV_ENABLE:
            return 1;

        case DRV_INSTALL:
        case DRV_REMOVE:
            return 1;
    }

    if (uiMessage < DRV_USER)
        return DefDriverProc(id,hDriver,uiMessage,lParam1,lParam2);
    else
        return ICERR_UNSUPPORTED;
}

/*****************************************************************************
 *
 * AVIDrawOpen() is called from the DRV_OPEN message
 *
 ****************************************************************************/

static LONG NEAR PASCAL AVIDrawOpen(ICOPEN FAR * icopen)
{
    PINSTINFO pinst;

    //
    // refuse to open if we are not being opened as a video draw device
    //
    if (icopen->fccType != streamtypeVIDEO)
        return 0;
    
    if (icopen->dwFlags == ICMODE_COMPRESS)
        return 0;

    if (icopen->dwFlags == ICMODE_DECOMPRESS)
        return 0;

    pinst = (PINSTINFO)GlobalAllocPtr(GHND, sizeof(INSTINFO));

    if (!pinst)
    {
        icopen->dwError = ICERR_MEMORY;
        return NULL;
    }

    //
    // init structure
    //
    pinst->hdd = DrawDibOpen();

    //
    // return success.
    //
    icopen->dwError = ICERR_OK;

    return (LONG) pinst;
}

/*****************************************************************************
 *
 * Close() is called on the DRV_CLOSE message.
 *
 ****************************************************************************/
static LONG NEAR PASCAL AVIDrawClose(PINSTINFO pi)
{
    if (pi->hdd) {
	DrawDibClose(pi->hdd);
    }
    
    if (pi->lpBuffer) {
	GlobalFreePtr(pi->lpBuffer);
    }
    
    GlobalFreePtr(pi);
    
    return 1;
}

/*****************************************************************************
 *
 * AVIDrawGetInfo() implements the ICM_GETINFO message
 *
 ****************************************************************************/
static LONG NEAR PASCAL AVIDrawGetInfo(ICINFO FAR *icinfo, LONG lSize)
{
    if (icinfo == NULL)
        return sizeof(ICINFO);

    if (lSize < sizeof(ICINFO))
        return 0;

    icinfo->dwSize	    = sizeof(ICINFO);
    icinfo->fccType	    = ICTYPE_VIDEO;
    icinfo->fccHandler      = FOURCC_AVIDraw;
    icinfo->dwFlags	    = VIDCF_DRAW;
    icinfo->dwVersion       = VERSION_AVIDraw;
    icinfo->dwVersionICM    = ICVERSION;
    lstrcpy(icinfo->szDescription, szDescription);
    lstrcpy(icinfo->szName, szName);

    return sizeof(ICINFO);
}

/*****************************************************************************
 *
 * AVIDrawQuery() implements ICM_DRAW_QUERY
 *
 ****************************************************************************/
static LONG NEAR PASCAL AVIDrawQuery(PINSTINFO pi,
			 LPBITMAPINFOHEADER lpbiIn)
{
    //
    // determine if the input DIB data is in a format we like.
    //
    if (lpbiIn == NULL)
        return ICERR_BADFORMAT;

    //
    // determine if the input DIB data is in a format we like.
    //
    if (lpbiIn->biCompression != BI_RGB)
        return ICERR_BADFORMAT;

    return ICERR_OK;
}

/*****************************************************************************
 *
 * AVIDrawSuggestFormat() implements ICM_DRAW_SUGGESTFORMAT
 *
 ****************************************************************************/

static LONG NEAR PASCAL AVIDrawSuggestFormat(PINSTINFO pi, ICDRAWSUGGEST FAR *lpicd, LONG cbicd)
{
    HIC hic;
    
    if (lpicd->lpbiSuggest == NULL)
	return sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

    //
    // Call COMPMAN to get a good format to display data in....
    //
    hic = ICGetDisplayFormat(NULL, lpicd->lpbiIn, lpicd->lpbiSuggest,
			     0, lpicd->dxDst, lpicd->dyDst);

    if (hic)
	ICClose(hic);

    if (lpicd->lpbiSuggest) {
	if (lpicd->lpbiSuggest->biCompression == BI_RLE8)
	    lpicd->lpbiSuggest->biCompression = BI_RGB;
    }
	
    // !!! Should check this format here to make sure it's RGB...

    // !!! If not, we could force it to 8-bit....

    return sizeof(BITMAPINFOHEADER) + lpicd->lpbiSuggest->biClrUsed * sizeof(RGBQUAD);
}

/*****************************************************************************
 *
 * AVIDrawBegin() implements ICM_DRAW_BEGIN
 *
 ****************************************************************************/

static LONG NEAR PASCAL AVIDrawBegin(PINSTINFO pi, ICDRAWBEGIN FAR *lpicd, LONG cbicd)
{
    LONG    l;

    l = AVIDrawQuery(pi, lpicd->lpbi);

    if ((l != 0) || (lpicd->dwFlags & ICDRAW_QUERY))
	return l;

    // Copy over whatever we want to remember
    pi->hdc = lpicd->hdc;
    pi->xDst = lpicd->xDst;
    pi->yDst = lpicd->yDst;
    pi->dxDst = lpicd->dxDst;
    pi->dyDst = lpicd->dyDst;
    pi->xSrc = lpicd->xSrc;
    pi->ySrc = lpicd->ySrc;
    pi->dxSrc = lpicd->dxSrc;
    pi->dySrc = lpicd->dySrc;

    SetStretchBltMode(pi->hdc, COLORONCOLOR);

    if (!DrawDibBegin(pi->hdd, pi->hdc,
		 pi->dxDst, pi->dyDst,
		 lpicd->lpbi,
		 pi->dxSrc, pi->dySrc,
		 0)) {  // !!! Flags?
	return ICERR_UNSUPPORTED;
    }

    // !!! error check


    //
    // Allocate a buffer for the scrambled picture
    //
    if (pi->lpBuffer)
	GlobalFreePtr(pi->lpBuffer);

    pi->lpBuffer = GlobalAllocPtr(GMEM_MOVEABLE, lpicd->lpbi->biSizeImage);

    if (!pi->lpBuffer)
	return ICERR_MEMORY;
	    
    return ICERR_OK;
}


/*****************************************************************************
 *
 * AVIDraw() implements ICM_DRAW
 *
 ****************************************************************************/

static LONG NEAR PASCAL AVIDraw(PINSTINFO pi, ICDRAW FAR *lpicd, LONG cbicd)
{
    UINT  wFlags;

    wFlags = DDF_SAME_HDC;

    if ((lpicd->dwFlags & ICDRAW_NULLFRAME) || lpicd->lpData == NULL) {
	if (lpicd->dwFlags & ICDRAW_UPDATE)
	    wFlags |= DDF_UPDATE;
	else
	    return ICERR_OK;	    // nothing to draw
    }

    if (lpicd->dwFlags & ICDRAW_PREROLL)
	wFlags |= DDF_DONTDRAW;

    if (lpicd->dwFlags & ICDRAW_HURRYUP)
	wFlags |= DDF_HURRYUP;

    //
    // This is the only part that actually has to do with the puzzle:
    // Mix up the picture into our extra buffer.
    //
    if (lpicd->lpData)
	MixPicture(&gPuzzle, lpicd->lpFormat, lpicd->lpData, pi->lpBuffer);
    
    if (!DrawDibDraw(pi->hdd, pi->hdc,
		pi->xDst, pi->yDst,
		pi->dxDst, pi->dyDst,
		lpicd->lpFormat,
		pi->lpBuffer,
		pi->xSrc, pi->ySrc,
		pi->dxSrc, pi->dySrc,
		wFlags)) {
	if (wFlags & DDF_UPDATE)
	    return ICERR_CANTUPDATE;
	else
	    return ICERR_UNSUPPORTED;
    }
    
    return ICERR_OK;
}


/*****************************************************************************
 *
 * AVIDrawChangePalette() implements ICM_DRAW_CHANGE_PALETTE
 *
 ****************************************************************************/

static LONG NEAR PASCAL AVIDrawChangePalette(PINSTINFO pi, LPBITMAPINFOHEADER lpbi)
{
    PALETTEENTRY    ape[256];
    LPRGBQUAD	    lprgb;
    int i;

    lprgb = (LPRGBQUAD) ((LPBYTE) lpbi + lpbi->biSize);
    
    for (i = 0; i < (int) lpbi->biClrUsed; i++) {
	ape[i].peRed = lprgb[i].rgbRed;
	ape[i].peGreen = lprgb[i].rgbGreen;
	ape[i].peBlue = lprgb[i].rgbBlue;
	ape[i].peFlags = 0;
    }
	
    DrawDibChangePalette(pi->hdd, 0, (int) lpbi->biClrUsed,
				 (LPPALETTEENTRY)ape);

    return ICERR_OK;
}


/*****************************************************************************
 *
 * AVIDrawEnd() implements ICM_DRAW_END
 *
 ****************************************************************************/

static LONG NEAR PASCAL AVIDrawEnd(PINSTINFO pi)
{
    // Note: do not call DrawDibEnd here, as we still may be asked to
    // update our current display, and calling DrawDibEnd would wipe
    // that out.
    
    return ICERR_OK;
}
