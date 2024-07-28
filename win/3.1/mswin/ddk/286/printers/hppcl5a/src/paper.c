/**[f******************************************************************
* paper.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
/***************************************************************************/
/*******************************   paper.c   *******************************/
/*
*  Service routines for reading paper formats from resources.
*
*  27 jan 92  SD       Moved PaperBit2Str here from devmode.c so it can
*                      be used in devcap.c for DC_PAPERNAMES.
*  09 jul 90  SD       Added envelope support.
*
*  09 may 90  SD       Changed order of operations in _llseek parameter
*                      list in GetPaperFormat() so as not to use unary -.
*                      MS requested and it was put in 3.42.
*
*  15 sep 89   peterbe Simplified and corrected ComputeLineBufSize().
*
*  05 sep 89   peterbe Added ComputeLineBufSize() with 5/4 worst-case
*          expansion.
*          Speed up NumBytes().
*   2-07-89    jimmat  Driver initialization changes.
*/
//#define DEBUG
#include "generic.h"
#include "resource.h"
#include "paper.h"
#include "strings.h"
  
#define LOCAL static
  
extern WORD __AHINCR;
#define ahincr  ((WORD)(&__AHINCR))
  
LOCAL WORD NumBytes (short);
LOCAL void SetBandCount(LPDEVICE, short);
long FAR PASCAL lmul(long, long);
long FAR PASCAL ldiv(long, long);
  
/**************************************************************************/
/****************************   Global Procs   ****************************/
  
/*  GetPaperFormat
*
*  Read the paper format from the resources.
*
*  This function called by Enable() to set up the page and image area
*  dimensions.
*/
  
BOOL FAR PASCAL
GetPaperFormat(PAPERFORMAT FAR *lpPF, HANDLE hModule, short paperInd,
short paperSize, short Orientation) {
  
    PAPERHEAD paperHead;
    PAPERLIST paperList;
    PAPERLISTITEM FAR *p;
    HANDLE hResInfo;
    LONG startpos;
    WORD paperid;
    BOOL success = FALSE;
    int hFile;
    int i;
  
#ifdef DEBUG_FUNCT
    DB(("Entering GetPaperFormat\n"));
#endif
    if ((hResInfo=FindResource(hModule,(LPSTR)PAPER1,(LPSTR)PAPERFMT)) &&
    (hFile=AccessResource(hModule,hResInfo)) >= 0) {
  
        if ((_lread(hFile,(LPSTR)&paperHead,sizeof(PAPERHEAD)) ==
            sizeof(PAPERHEAD)) &&
            (_llseek(hFile, paperHead.offsLists - sizeof(PAPERHEAD) +
            (paperInd*sizeof(PAPERLIST)),1) > 0L) &&
            (_lread(hFile,(LPSTR)&paperList,sizeof(PAPERLIST)) ==
        sizeof(PAPERLIST))) {
  
            startpos = -paperHead.offsLists-sizeof(PAPERLIST)-
            (paperInd*sizeof(PAPERLIST));
  
            switch (paperSize) {
  
                case DMPAPER_LETTER:    paperid = PAPERID_LETTER;   break;
                case DMPAPER_LEGAL: paperid = PAPERID_LEGAL;    break;
                case DMPAPER_EXECUTIVE: paperid = PAPERID_EXEC;     break;
                case DMPAPER_A4:    paperid = PAPERID_A4;       break;
                case DMPAPER_MONARCH:  paperid = PAPERID_MONARCH;    break;
                case DMPAPER_COM10:    paperid = PAPERID_COM10;      break;
                case DMPAPER_DL:       paperid = PAPERID_DL;         break;
                case DMPAPER_C5:       paperid = PAPERID_C5;         break;
                default:        paperid = 0;            break;
            }
  
            for (i=0, p=paperList.p; i < paperList.len &&
                !(p->id & paperid); ++i, ++p)
                ;
  
            if (i < paperList.len) {
  
                if (Orientation == DMORIENT_LANDSCAPE)
                    i = p->indLandPaperFormat;
                else
                    i = p->indPortPaperFormat;
  
                if ((_llseek(hFile,startpos+paperHead.offsFormats+
                    (i*sizeof(PAPERFORMAT)),1) > 0L) &&
                    (_lread(hFile,(LPSTR)lpPF,sizeof(PAPERFORMAT)) ==
                sizeof(PAPERFORMAT))) {
  
                    success = TRUE;
                }
            }
        }
  
        _lclose(hFile);
    }
  
#ifdef DEBUG
    if (success)
    {
        //  DBMSG(("GetPaperFormat(): paper %d, paperInd %d, ind %d\n",
        //       paperSize, paperInd, i));
        //  DBMSG(("   xPhys=%d\n", lpPF->xPhys));
        //  DBMSG(("   yPhys=%d\n", lpPF->yPhys));
        //  DBMSG(("   xImage=%d\n", lpPF->xImage));
        //  DBMSG(("   yImage=%d\n", lpPF->yImage));
        //  DBMSG(("   xPrintingOffset=%d\n", lpPF->xPrintingOffset));
        //  DBMSG(("   yPrintingOffset=%d\n", lpPF->yPrintingOffset));
        //  DBMSG(("   select=%ls\n", (LPSTR)lpPF->select));
    }
    else
    {
        //  DBMSG(("GetPaperFormat(): paper %d, paperInd %d, FAILED\n",
        //       paperSize, paperInd));
    }
#endif
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetPaperFormat\n"));
#endif
    return (success);
}
  
/*  GetPaperBits
*
*  Get the paper lists from the resources and merge the supported
*  papers for each list into one WORD per list.
*
*  This function called by DeviceMode() to get the list of possible
*  paper combinations (each printer has an index into this list).
*/
BOOL FAR PASCAL GetPaperBits (hModule, lpPaperBits)
HANDLE hModule;
WORD FAR *lpPaperBits;
{
    PAPERHEAD paperHead;
    PAPERLIST paperList;
    PAPERLISTITEM FAR *p;
    HANDLE hResInfo;
    short i, j;
    int hFile;
#ifdef DEBUG_FUNCT
    DB(("Entering GetPaperBits\n"));
#endif
  
    if ((hResInfo=FindResource(hModule,(LPSTR)PAPER1,(LPSTR)PAPERFMT)) &&
        (hFile=AccessResource(hModule,hResInfo)) >= 0)
    {
        if ((_lread(hFile,(LPSTR)&paperHead,sizeof(PAPERHEAD)) ==
            sizeof(PAPERHEAD)) &&
            (paperHead.offsLists == (DWORD)sizeof(PAPERHEAD)))
        {
            if (paperHead.numLists > MAX_PAPERLIST)
                paperHead.numLists = MAX_PAPERLIST;
  
            for (i=0; i < paperHead.numLists; ++i, ++lpPaperBits)
            {
                *lpPaperBits = 0;
  
                if (_lread(hFile,(LPSTR)&paperList,sizeof(PAPERLIST)) ==
                    sizeof(PAPERLIST))
                {
                    for (j=0, p=paperList.p; j < paperList.len; ++j, ++p)
                        *lpPaperBits |= p->id;
                }
  
                /*               #ifdef DEBUG
                {
                WORD bits = *lpPaperBits;
                DBMSG(("Paper bits at ind %d\n", i));
                if (bits & PAPERID_LETTER)
                { DBMSG(("    PAPERID_LETTER\n"));  }
                if (bits & PAPERID_LEGAL)
                { DBMSG(("    PAPERID_LEGAL\n"));   }
                if (bits & PAPERID_EXEC)
                { DBMSG(("    PAPERID_EXEC\n"));        }
                if (bits & PAPERID_A4)
                { DBMSG(("    PAPERID_A4\n"));      }
                }
                #endif
  
            */                }
        }
  
        _lclose(hFile);
    }
    else
        return FALSE;
  
#ifdef DEBUG_FUNCT
    DB(("Exiting GetPaperBits\n"));
#endif
    return TRUE;
}
  
/*  Paper2Bit
*/
WORD FAR PASCAL
Paper2Bit(short paper) {
#ifdef DEBUG_FUNCT
    DB(("Entering Paper2Bit\n"));
#endif
  
    switch(paper) {
  
        case DMPAPER_LETTER:    return PAPERID_LETTER;
        case DMPAPER_LEGAL: return PAPERID_LEGAL;
        case DMPAPER_EXECUTIVE: return PAPERID_EXEC;
        case DMPAPER_A4:    return PAPERID_A4;
        case DMPAPER_MONARCH:  return PAPERID_MONARCH;
        case DMPAPER_COM10:    return PAPERID_COM10;
        case DMPAPER_DL:       return PAPERID_DL;
        case DMPAPER_C5:       return PAPERID_C5;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting Paper2Bit\n"));
#endif
    return 0;
}

/***********************************************************************
P A P E R  B I T  2  S T R
***********************************************************************/
  
/*  Map paper bits to name string. */
  
short FAR PASCAL
PaperBit2Str(short paper) {
#ifdef DEBUG_FUNCT
    DB(("Entering PaperBit2Str \n"));
#endif
    switch(paper) {
  
        case PAPERID_LETTER:        return IDS_LETTER;
        case PAPERID_LEGAL:     return IDS_LEGAL;
        case PAPERID_EXEC:      return IDS_EXEC;
        case PAPERID_A4:        return IDS_A4;
        case PAPERID_MONARCH:    return IDS_MONARCH;
        case PAPERID_COM10:      return IDS_COM10;
        case PAPERID_DL:         return IDS_DL;
        case PAPERID_C5:         return IDS_C5;
    }
  
#ifdef DEBUG_FUNCT
    DB(("Exiting PaperBit2Str \n"));
#endif
    return IDS_LETTER;
}
 
  
/*  ComputeBandBitmapSize
*
*  Compute the smallest size possible for the bitmap for banding.
*
*  This function called by Enable() to determine the size of the
*  device header (which includes the banding bitmap).
*/
DWORD FAR PASCAL
ComputeBandBitmapSize (lpDevice, lpPF, prtResFac, orient, iBandDepth)
LPDEVICE lpDevice;
PAPERFORMAT FAR *lpPF;
short prtResFac;
short orient;
short iBandDepth;
{
    DWORD size;
    short nfullsegs;
    BITMAP lpbm;
  
#ifdef DEBUG_FUNCT
    DB(("Entering ComputeBandBitmapSize\n"));
#endif
    if (ProtectMode)
    {
        lpbm = lpDevice->epBmpHdr;
        if (!lpbm.bmScanSegment)
            goto SmallBitmap;
        nfullsegs = ((lpbm.bmHeight+7)&0xFFF8) / lpbm.bmScanSegment;
        size = lmul(0x10000L, (long)nfullsegs) +
        (long)(lpbm.bmWidthBytes*(((lpbm.bmHeight+7)&0xFFF8) %
        lpbm.bmScanSegment));
    }
    else
    {
        SmallBitmap:
        if (orient == DMORIENT_LANDSCAPE)
            size = NumBytes(iBandDepth) * (lpPF->yImage >> prtResFac);
        else
            size = NumBytes(lpPF->xImage >> prtResFac) * iBandDepth;
    }
  
    // DBMSG(("ComputeBandBitmapSize(%lp): size=%ld\n", lpPF, size));
#ifdef DEBUG_FUNCT
    DB(("Exiting ComputeBandBitmapSize\n"));
#endif
    return (size);
}   // ComputeBandBitmapSize()
  
  
// ComputeLineBufSize()
// Compute size of special scanline buffer (at offset lpDevice->epLineBuf)
// used for some printers.  We assume this printer needs such a buffer.
  
WORD FAR PASCAL
ComputeLineBufSize (PAPERFORMAT FAR *lpPF, LPPCLDEVMODE lpDevmode)
{
    WORD size;
#ifdef DEBUG_FUNCT
    DB(("Entering ComputeLineBufSize\n"));
#endif
  
    // figure basic size of scanline: scale and convert to bytes.
    size = NumBytes ( ((lpDevmode->dm.dmOrientation == DMORIENT_LANDSCAPE) ?
    (lpPF->yImage) : (lpPF->xImage)) >> (lpDevmode->prtResFac) );
  
    // correct for worst-case randomness (combo of small repeats and
    // short random patterns), round to even bytes.
    // always round UP!
  
    size = (5 * size + 3) / 4;      // times 5/4
    size = 2 * ((size + 1) / 2);    // round up to even value.
  
#ifdef DEBUG_FUNCT
    DB(("Exiting ComputeLineBufSize\n"));
#endif
    return size;
  
}   // ComputeLineBufSize()
  
/*  ComputeBandingParameters
*
*  Set up the bitmap sizes and other banding information used by control
*  to print bands.
*
*  This function is called by Enable() in reset.c to fill in the
*  BITMAP structure.
*/
void FAR PASCAL ComputeBandingParameters (lpDevice, prtResFac)
LPDEVICE lpDevice;
short prtResFac;
{
    PAPERFORMAT FAR *lpPF = &lpDevice->epPF;
    BITMAP FAR *lpBitmap = &lpDevice->epBmpHdr;
  
#ifdef DEBUG_FUNCT
    DB(("Entering ComputeBandingParameters\n"));
#endif
    if (lpDevice->epType == (short)DEV_LAND)
    {
        lpBitmap->bmWidth = lpDevice->epBandDepth;
        lpBitmap->bmHeight = lpPF->yImage >> prtResFac;
        SetBandCount(lpDevice, lpPF->xImage >> prtResFac);
    }
    else
    {
        lpBitmap->bmWidth = lpPF->xImage >> prtResFac;
        lpBitmap->bmHeight = lpDevice->epBandDepth;
        SetBandCount(lpDevice, lpPF->yImage >> prtResFac);
    }
  
    lpBitmap->bmWidthBytes = NumBytes(lpBitmap->bmWidth);
    lpBitmap->bmWidthPlanes = lmul((long)lpBitmap->bmWidthBytes,
    (long)lpBitmap->bmHeight);
    lpBitmap->bmPlanes = 1;
    lpBitmap->bmBitsPixel = 1;
  
    if (lmul((long)lpBitmap->bmWidthBytes,(long)lpBitmap->bmHeight) > 0x10000)
    {
        lpBitmap->bmSegmentIndex = ahincr;
        lpBitmap->bmScanSegment = (short)ldiv( 0x10000L,
        (long)lpBitmap->bmWidthBytes );
  
        lpBitmap->bmFillBytes = (short)( 0x10000L -
        lmul( (long)lpBitmap->bmScanSegment,
        (long)lpBitmap->bmWidthBytes ) );
    }
  
    /*    DBMSG(("ComputeBandingParameters(%lp,%d):\n", lpDevice, prtResFac));
    DBMSG(("    lpBitmap->bmWidth = %d\n", lpBitmap->bmWidth));
    DBMSG(("    lpBitmap->bmHeight = %d\n", lpBitmap->bmHeight));
    DBMSG(("    lpBitmap->bmWidthBytes = %d\n",
    lpBitmap->bmWidthBytes));
    DBMSG(("    lpBitmap->bmWidthPlanes = %ld\n",
    lpBitmap->bmWidthPlanes));
    DBMSG(("    lpBitmap->bmPlanes = %d\n", (WORD)lpBitmap->bmPlanes));
    DBMSG(("    lpBitmap->bmBitsPixel = %d\n",
    (WORD)lpBitmap->bmBitsPixel));
    DBMSG(("    lpDevice->epNumBands = %d\n", lpDevice->epNumBands));
    DBMSG(("    lpDevice->epLastBandSz = %d\n",
    lpDevice->epLastBandSz));
    DBMSG(("    lpBitmap->bmSegmentIndex = %d\n",
    (WORD)lpBitmap->bmSegmentIndex));
    DBMSG(("    lpBitmap->bmScanSegment = %d\n",
    (WORD)lpBitmap->bmScanSegment));
    DBMSG(("    lpBitmap->bmFillBytes = %d\n",
    (WORD)lpBitmap->bmFillBytes));
  
    */
#ifdef DEBUG_FUNCT
    DB(("Exiting ComputeBandingParameters\n"));
#endif
}
  
/*  ComputeBandStartPosition
*
*  Compute cursor positions to the band.  Normally, it is not necessary
*  to do an explicit move to the start of a band because the current
*  cursor position is already there (output of the previous band put it
*  there).  This function is called when the previous graphics bands has
*  been skipped.
*
*  In portrait, the starting cursor position is the top left corner of
*  the band rectangle.  In landscape, it is the top right corner.
*
*  This function is called by the banding code for the NEXTBAND escape
*  inside of control.c.
*/
void FAR PASCAL ComputeBandStartPosition(lpPos, lpDevice, bandnum)
LPPOINT lpPos;
LPDEVICE lpDevice;
short bandnum;
{
#ifdef DEBUG_FUNCT
    DB(("Entering ComputeBandStartPosition\n"));
#endif
    lpPos->xcoord = 0;
    lpPos->ycoord = 0;
  
    /*  Text band: punt.
    */
    if (bandnum < 1)
        return;
  
    /*  Off of page, return coord of last band.
    */
    if (bandnum > lpDevice->epNumBands)
        bandnum = lpDevice->epNumBands;
  
    if (lpDevice->epType == (short)DEV_LAND)
    {
        lpPos->xcoord = (lpDevice->epPF.xImage >> lpDevice->epScaleFac) -
        (lpDevice->epBandDepth * (bandnum - 1));
    }
    else
        lpPos->ycoord = (bandnum - 1) * lpDevice->epBandDepth;
  
    /*  Shift out because we always report these numbers at
    *  300dpi, GDI scales them down if the resolution is
    *  less than 300dpi.
    */
    lpPos->xcoord <<= lpDevice->epScaleFac;
    lpPos->ycoord <<= lpDevice->epScaleFac;
  
    //    DBMSG(("ComputeBandStartPosition(%lp,%lp,%d): x=%d, y=%d\n",
    //        lpPos, lpDevice, bandnum, (WORD)lpPos->xcoord,
    //         (WORD)lpPos->ycoord));
#ifdef DEBUG_FUNCT
    DB(("Exiting ComputeBandStartPosition\n"));
#endif
}
  
/*  ComputeNextBandRect
*
*  Compute the offsets and dimensions of the next banding rectangle.
*  If we've banded the whole page, return FALSE.
*
*  We always report these numbers at 300dpi, Windows GDI scales them
*  down if we're printing at 75 and 150dpi.  This accounts for the
*  occasional loss of hairlines (1/300 inch lines) when printing at
*  75dpi (the app prints them one pixel wide and GDI clips them out
*  because four 300dpi scanlines are merged into one 75dpi scanline).
*
*  This function is called by the banding code for the NEXTBAND escape
*  inside of control.c.
*/
BOOL FAR PASCAL ComputeNextBandRect(lpDevice, currentband, lpBandRect)
LPDEVICE lpDevice;
short currentband;
LPRECT lpBandRect;
{
    short bandsize = lpDevice->epBandDepth;
#ifdef DEBUG_FUNCT
    DB(("Entering ComputeNextBandRect\n"));
#endif
    //DBMSG(("Entering ComputeNextBandRect\n"));
    //DBMSG(("epNumBands=%d, TEXTBAND=%d\n",lpDevice->epNumBands,TEXTBAND));
    //DBMSG(("currentband=%d, Nband=%d\n",currentband,lpDevice->epNband));
  
    if(currentband==1) {  // first band - do init
        lpDevice->epXOffset = 0;
        lpDevice->epYOffset = 0;
        // Shift rectangle coordiates by scaling factor.
        // Fixes bug #124
        SetRect(lpBandRect, 0, 0, lpDevice->epPF.xImage<<lpDevice->epScaleFac,
        lpDevice->epPF.yImage<<lpDevice->epScaleFac);
        return TRUE;
    }
    // Last band, set banding rectangle to 0
    if (currentband > lpDevice->epNumBands+2){
        //DBMSG(("currentband > lpDevice->epNumBands+2, setting rect //empty.\n"));
        SetRectEmpty(lpBandRect);
        return FALSE;
    }
  
    // Text band, set banding rectangle to whole page
    if ((currentband) == lpDevice->epNumBands+2) {
        //DBMSG(("currentband == lpDevice->epNumBands+2\n"));
        TEXTBAND=TRUE;
        // reset epXOffset and epYOffset for use in ExtTextOut() - SJC
        lpDevice->epXOffset = 0;
        lpDevice->epYOffset = 0;
        // Shift rectangle coordiates by scaling factor.
        // Fixes bug #124
        SetRect(lpBandRect, 0, 0, lpDevice->epPF.xImage<<
        lpDevice->epScaleFac,
        lpDevice->epPF.yImage<<lpDevice->epScaleFac);
        return TRUE;
    }
  
    if (lpDevice->epType == (short)DEV_LAND) {
        //DBMSG(("epType==DEV_LAND\n"));
        /*     ________________
        *    |    :  :        |   Landscape page:
        *    |    :  :        |
        *  X |    :  :        |   Band across  (right to left)
        *    |____:__:________|
        *
        * (0,0)       Y
        */
        if (currentband == lpDevice->epNumBands+1) { // last graphics band
            BITMAP FAR *lpBitmap = &lpDevice->epBmpHdr;
            DBMSG(("==> last graphics band <==\n"));
  
            bandsize = lpDevice->epLastBandSz;
            lpBitmap->bmWidth=bandsize;
            lpBitmap->bmWidthBytes=NumBytes(lpBitmap->bmWidth);
            lpBitmap->bmWidthPlanes=lpBitmap->bmWidthBytes * lpBitmap->bmHeight;
            if (lmul((long)lpBitmap->bmWidthBytes,(long)lpBitmap->bmHeight) >
            0x10000) {
                lpBitmap->bmSegmentIndex = ahincr;
                lpBitmap->bmScanSegment = (short)ldiv( 0x10000L,
                (long)lpBitmap->bmWidthBytes );
  
                lpBitmap->bmFillBytes = (short)( 0x10000L -
                lmul( (long)lpBitmap->bmScanSegment,
                (long)lpBitmap->bmWidthBytes ) );
            }
        } // if(currentband==epNumBands+1)
  
        /* The following line was modified (+1 added) to fix the duplicate
        * image problem in the shipped 3.70 driver.      Terry
        */
        lpDevice->epXOffset = (currentband == lpDevice->epNumBands+1) ? 0 :
        (lpDevice->epPF.xImage >> lpDevice->epScaleFac) -
        (lpDevice->epBandDepth * (currentband-1));
        lpDevice->epXOffset <<= lpDevice->epScaleFac;
  
        DBMSG(("epBandDepth=%d, epLastBandSz=%d\n",
        lpDevice->epBandDepth,lpDevice->epLastBandSz));
        DBMSG(("epXOffset=%d, bandsize=%d, epScaleFac=%d\n",
        lpDevice->epXOffset,bandsize,lpDevice->epScaleFac));
        DBMSG(("xImage=%d, yImage=%d\n",lpDevice->epPF.xImage,
        lpDevice->epPF.yImage));
  
        SetRect(lpBandRect, lpDevice->epXOffset, 0,
        lpDevice->epXOffset + (bandsize << lpDevice->epScaleFac),
        lpDevice->epPF.yImage);
  
        if (bandsize < lpDevice->epBandDepth) {
            /*  If the last band's width does not equal a WORD boundary,
            *  modify epXOffset so the routines in stub.c will draw the
            *  image flush right within the band bitmap.
            */
  
            lpDevice->epXOffset -=
            ((bandsize + 15) / 16 * 16 - bandsize)
            << lpDevice->epScaleFac;
  
  
            DBMSG(("ComputeNextBandRect(): epXOffset modified to %d\n",
            lpDevice->epXOffset));
        }
    }  // if (lpDevice->epType == (short)DEV_LAND)
    else { // PORTRAIT mode
        //DBMSG(("epType==Portrait\n"));
        /*     _________
        *    |         |
        *    |         |   Portrait page:
        *    |- - - - -|
        *  X |- - - - -|   Band down
        *    |         |
        *    |         |
        *    |_________|
        *
        * (0,0)   Y
        */
        if (currentband == 1 || currentband == lpDevice->epNumBands+1) {
            BITMAP FAR *lpBitmap = &lpDevice->epBmpHdr;
  
            //            if (currentband == 1)
            if (currentband == lpDevice->epNumBands+1) { // last graphics band
                //DBMSG(("==> last graphics band <==\n"));
                bandsize = lpDevice->epLastBandSz;
            }
            lpBitmap->bmHeight = bandsize;
            lpBitmap->bmWidthPlanes=
            lpBitmap->bmWidthBytes*lpBitmap->bmHeight;
        }
  
        lpDevice->epYOffset = (currentband - 2) * lpDevice->epBandDepth;
        lpDevice->epYOffset <<= lpDevice->epScaleFac;
  
        //DBMSG(("epBandDepth=%d, epLastBandSz=%d\n",
        //        lpDevice->epBandDepth,lpDevice->epLastBandSz));
        //DBMSG(("epYOffset=%d, bandsize=%d, epScaleFac=%d\n",
        //        lpDevice->epYOffset,bandsize,lpDevice->epScaleFac));
        SetRect(lpBandRect, 0, lpDevice->epYOffset,
        lpDevice->epPF.xImage,
        lpDevice->epYOffset + (bandsize << lpDevice->epScaleFac));
    } // PORTRAIT mode
#ifdef DEBUG_FUNCT
    DB(("Exiting ComputeNextBandRect\n"));
#endif
    return TRUE;
}
  
/**************************************************************************/
/*****************************   Local Procs   ****************************/
  
  
/*  NumBytes
*
*  Compute the number of bytes, but align it on word boundaries.
*  (convert pixel count to even byte count)
*/
LOCAL WORD NumBytes (val)
short val;
{
    WORD num;
#ifdef DEBUG_FUNCT
    DB(("Entering NumBytes\n"));
#endif
  
    num = (val + 15) / 16;
  
    //if ((num * 16) < val)
    //    ++num;
  
#ifdef DEBUG_FUNCT
    DB(("Exiting NumBytes\n"));
#endif
    return (num * 2);
}
  
  
/*  SetBandCount
*
*  Set up the number of print bands on the page and compute the size
*  of the last band (it does not necessarily have to be 64 scan lines).
*/
LOCAL void SetBandCount(lpDevice, depth)
LPDEVICE lpDevice;
short depth;
{
    short tmp;
  
#ifdef DEBUG_FUNCT
    DB(("Entering SetBandCount\n"));
#endif
    lpDevice->epNumBands = depth / lpDevice->epBandDepth;
  
    if ((tmp=(lpDevice->epNumBands * lpDevice->epBandDepth)) < depth)
    {
        ++lpDevice->epNumBands;
        lpDevice->epLastBandSz = depth - tmp;
    }
    else
        lpDevice->epLastBandSz = lpDevice->epBandDepth;
  
    //  DBMSG(("SetBandCount(%lp,%d): numBands=%d, lastBandSz=%d\n",
    //      lpDevice, depth, lpDevice->epNumBands,
    //        lpDevice->epLastBandSz));
#ifdef DEBUG_FUNCT
    DB(("Exiting SetBandCount\n"));
#endif
}
