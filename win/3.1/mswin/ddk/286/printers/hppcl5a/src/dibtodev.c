/*********************************************************************
*
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
*
* DIBtoDev.C
*
* 17Feb89  chrisg  created
* 18May90  chrisg  addapting to PCL
* 11Jun90  chrisg  works reasonably well, doesn't scale down
*
* things to know:
*
*  dib scan lines are DWORD alligned
*
*  we don't support RLE or any biStyle != 0.
*
* things to add:
*  opaque the background (currently this is a transparent op)
*  some ROP support (related to above)
*  scale down support (is this needed?)
*
*********************************************************************/

//#define DEBUG
#include "generic.h"
#include "utils.h"

#define HIWORD(dw)      ((DWORD)(dw) >> 16)

int FAR PASCAL MulDiv(int, int, int);
int FAR PASCAL Control(LPDEVICE, short, LPSTR, LPPOINT);

typedef RGBQUAD FAR *LPRGBQUAD;
typedef RGBTRIPLE FAR *LPRGBTRIPLE;
typedef LPSTR LPBR;


#define RGB_BLACK 0L
#define RGB_WHITE 0x00FFFFFF

BOOL determine_bit(int, BYTE, int, int);
int Find_Lighter_Gray(BYTE);

int PASCAL HalfToneToDevice(
LPDEVICE    lpdv,           /* physical device */
WORD    DestX,  WORD DestY,
WORD    DestXE, WORD DestYE,
WORD    SrcX,   WORD SrcY,
WORD    SrcXE,  WORD SrcYE,
LPSTR   lpBits,         /* pointer to DIBitmap Bits */
LPBITMAPINFOHEADER lpBitmapInfoHeader,  /* DIBitmap info Block */
LPDRAWMODE lpdm,
LPRECT  lpClip
);


int FAR PASCAL StretchDIB(
LPDEVICE    lpdv,           /* physical device or mem bitmap */
WORD    wSetGetMode,
WORD    DestX,  WORD DestY,
WORD    DestXE, WORD DestYE,
WORD    SrcX,   WORD SrcY,
WORD    SrcXE,  WORD SrcYE,
LPSTR   lpBits,         /* pointer to DIBitmap Bits */
LPBITMAPINFOHEADER lpBitmapInfoHeader,  /* pointer to DIBitmap info Block */
LPSTR   lpConversionInfo,   /* not used */
DWORD   dwRop,
LPBR    lpbr,
LPDRAWMODE lpdm,
LPRECT  lpClip
);

int FAR PASCAL allocate_mem(LPDEVICE);


int FAR PASCAL StretchDIB(
LPDEVICE    lpdv,           /* physical device */
WORD    wSetGetMode,
WORD    DestX,  WORD DestY,
WORD    DestXE, WORD DestYE,
WORD    SrcX,   WORD SrcY,
WORD    SrcXE,  WORD SrcYE,
LPSTR   lpBits,         /* pointer to DIBitmap Bits */
LPBITMAPINFOHEADER lpBitmapInfoHeader,  /* pointer to DIBitmap info Block */
LPSTR   lpConversionInfo,   /* not used */
DWORD   dwRop,
LPBR    lpbr,
LPDRAWMODE lpdm,
LPRECT  lpClip
)
{
    int num;
#ifdef DEBUG_FUNCT
    DB(("Entering StretchDIB\n"));
#endif

    if (!lpdv->epType)   // dest memory, GDI simulate
        return -1;
    lpdv->epMode |= ANYGRX;  // remember that we have graphics
    if (lpdv->epMode & DRAFTFLAG)
        return TRUE;     // draft mode
    if (TEXTBAND) {          // text band
        DBMSG(("In StretchDIB, TEXTBAND=TRUE, exiting\n"));
        return TRUE;
    }
    if (lpdv->epNband<2){   // BAND not valid
        DBMSG(("in DibToDev, epNband<2, exiting\n"));
        return TRUE;
    }

    // test for all cases that we want GDI to simulate

    if (wSetGetMode != 0 ||                 // GET operation
        lpBitmapInfoHeader->biCompression != 0 ||   // RLE format
        //      lpBitmapInfoHeader->biBitCount == 1 ||  // 1 bit case
        (lpBitmapInfoHeader->biBitCount != 1 && lpdv->epScaleFac) ||
        dwRop != SRCCOPY ||
        (int)DestYE < 0 )  /*  If DestYE is neg then let GDI do it
                               Bug fix for MS bug # 8981  -  jcs    */
        return -1;
        
    // we probably want to text the destination rect with
    // the band rect to see if we really need to do this band

    lpdv->epMode |= GRXFLAG;

    num = HalfToneToDevice(lpdv,
    DestX, DestY,
    DestXE, DestYE,
    SrcX, SrcY,
    SrcXE, SrcYE,
    lpBits, lpBitmapInfoHeader,
    lpdm, lpClip);

#ifdef DEBUG_FUNCT
    DB(("Exiting StretchDIB\n"));
#endif
    return num;
}





/*
* do either Color or Gray DIB
*/

int FAR PASCAL DIBToDevice(
LPDEVICE    lpdv,       /* physical device */
WORD    DestX,          /* Destination X (on screen) */
WORD    DestY,          /* Destination Y (on screen) */
WORD    nStart,
WORD    nNumScans,      /* # of scan lines to do */
LPRECT  lpClip,
LPDRAWMODE lpdm,
LPSTR   lpBits,         /* pointer to DIBitmap Bits */
LPBITMAPINFOHEADER lpBitmapInfoHeader,  /* pointer to DIBitmap info Block */
LPSTR   lpConversionInfo    /* not used */
)
{
    DWORD realHeight;
    RECT rc;
    int res;
    WORD xScale, yScale;

#ifdef DEBUG_FUNCT
    DB(("Entering DIBToDevice\n"));
#endif
//    DBMSG(("Entering DIBToDevice()\n"));
//    DBMSG(("DestX = %d, DestY = %d, nStart = %d, nNumScans = %d\n",
//    DestX, DestY, nStart, nNumScans));

    xScale = (int)lpBitmapInfoHeader->biWidth;
    yScale = (int)lpBitmapInfoHeader->biHeight;

    realHeight = lpBitmapInfoHeader->biHeight;
    lpBitmapInfoHeader->biHeight = nNumScans;   // patch up the height

    rc.left = DestX;
    rc.top = DestY + MulDiv(((int)realHeight - nStart - nNumScans),
                                                yScale,
    (WORD) realHeight);
    rc.right = rc.left + xScale;
    rc.bottom = rc.top + MulDiv(nNumScans, yScale, (int)realHeight);
//    DBMSG(("xScale= %d, yScale= %d, rc.top= %d, rc.right= %d,
//            rc.bottom= %d\n",
//    xScale, yScale, rc.top, rc.right, rc.bottom));

    res = StretchDIB(lpdv, 0,
    rc.left, rc.top,
    xScale, MulDiv(nNumScans, yScale, (int)realHeight),
    0, 0, (WORD)lpBitmapInfoHeader->biWidth, nNumScans,
    lpBits, lpBitmapInfoHeader, lpConversionInfo, SRCCOPY, NULL, lpdm, 
lpClip);
//&rc); // This fixed the extra data on image when scaled down. Terry

    lpBitmapInfoHeader->biHeight = realHeight;//restore patched height


#ifdef DEBUG_FUNCT
    DB(("Exiting DIBToDevice\n"));
#endif
    return res;
}


BYTE dot_8x8_45[DX_CLUSTER][DY_CLUSTER] = {
    255, 249, 206,  31,   8,   1,  73, 164,
    231, 239, 198,  39,  14,  23,  64, 173,
    181, 189, 148,  98,  48,  56, 106, 139,
    94,  85, 119, 135, 227, 219, 160, 127,
    8,   1,  77, 169, 255, 249, 210,  35,
    19,  27,  69, 177, 235, 243, 202,  44,
    52,  60, 110, 144, 185, 194, 152, 102,
    223, 214, 156, 123,  89,  81, 114, 131,
};


#define LSCRN dot_8x8_lighten

BYTE dot_8x8_lighten[DX_CLUSTER][DY_CLUSTER] = {
    255, 221, 143,  17,   4,   1,  39,  96,
    179, 197, 133,  21,   7,  12,  34, 104,
    113, 122,  79,  50,  25,  30,  54,  73,
    47,  45,  59,  70, 171, 158,  91,  64,
    4,   1,  41, 100, 255, 237, 147,  19,
    9,  14,  36, 108, 187, 207, 137,  23,
    27,  32,  54,  76, 117, 127,  83,  52,
    165, 153,  87,  62,  47,  43,  57,  67
};


char bit_patterns_16 [128] =
{ 0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, //  black
    0x00,  0x02,  0x00,  0x00,  0x00,  0x20,  0x00,  0x00,
    0x00,  0x45,  0x00,  0xAA,  0x00,  0xA8,  0x00,  0x55,
    0x88,  0x44,  0x11,  0x22,  0x88,  0x44,  0x11,  0x22,
    0x00,  0x77,  0x00,  0xDD,  0x00,  0x77,  0x00,  0xDD,
    0x00,  0xFF,  0x00,  0xFF,  0x00,  0xFF,  0x00,  0xFF,
    0x88,  0xFF,  0x00,  0xFF,  0x22,  0xFF,  0x00,  0xFF,
    0x55,  0x77,  0x55,  0xDD,  0x55,  0x77,  0x55,  0xDD,
    0x2A,  0xFF,  0xA2,  0xFF,  0x8A,  0xFF,  0xA8,  0xFF,
    0xAA,  0xFF,  0xAA,  0xFF,  0xAA,  0xFF,  0xAA,  0xFF,
    0xF9,  0x3F,  0xE7,  0xFC,  0x9F,  0xF3,  0x7E,  0xCF,
    0xEF,  0xFD,  0xAF,  0x77,  0xFE,  0xDF,  0xFA,  0x77,
    0xEF,  0xFD,  0xBF,  0xF7,  0xFE,  0xDF,  0xFB,  0x7F,
    0xBF,  0xDF,  0xFB,  0xFD,  0xBF,  0xDF,  0xFB,  0xFD,
    0xBF,  0xFF,  0xFB,  0xFF,  0xBF,  0xFF,  0xFB,  0xFF,
0xFF,  0xFF,  0xDF,  0xFF,  0xFF,  0xFF,  0xFD,  0xFF }; // lt gray




char bit_patterns_32 [256] =
{ 0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00, //  black
    0x00,  0x02,  0x00,  0x00,  0x00,  0x20,  0x00,  0x00,
    0x00,  0x40,  0x00,  0x04,  0x00,  0x40,  0x00,  0x04,
    0x00,  0x8A,  0x00,  0x54,  0x00,  0x8A,  0x00,  0x54,
    0x00,  0x6D,  0x00,  0x55,  0x00,  0xD6,  0x00,  0xAA,
    0x00,  0x6D,  0x00,  0x77,  0x00,  0xD6,  0x00,  0xDD,
    0x00,  0x6D,  0x00,  0xDD,  0x00,  0x6B,  0x00,  0xDD,
    0x00,  0x77,  0x00,  0xDD,  0x00,  0x77,  0x00,  0xDD,
    0x33,  0x8C,  0x33,  0xC8,  0x33,  0x8C,  0x33,  0xC8,
    0x00,  0x6F,  0x00,  0xFF,  0x00,  0xF6,  0x00,  0xFF,
    0x00,  0xFF,  0x00,  0xFF,  0x00,  0xFF,  0x00,  0xFF,
    0x80,  0xFF,  0x00,  0xFF,  0x08,  0xFF,  0x00,  0xFF,
    0x88,  0xFF,  0x00,  0xFF,  0x22,  0xFF,  0x00,  0xFF,
    0x88,  0xFF,  0x10,  0xFF,  0x22,  0xFF,  0x01,  0xFF,
    0x55,  0x77,  0x55,  0xDD,  0x55,  0x77,  0x55,  0xDD,
    0x2A,  0xFF,  0xA2,  0xFF,  0x8A,  0xFF,  0xA8,  0xFF,
    0xCB,  0xF9,  0x2F,  0xE5,  0xBC,  0x9F,  0xF2,  0x5E,
    0x2A,  0xFF,  0xA2,  0xFF,  0x8A,  0xFF,  0xA8,  0xFF,
    0x2A,  0xFF,  0xAA,  0xFF,  0xA2,  0xFF,  0xAA,  0xFF,
    0xBB,  0x77,  0xDD,  0xEE,  0xBB,  0x77,  0xDD,  0xEE,
    0xAA,  0xFF,  0xAA,  0xFF,  0xAA,  0xFF,  0xAA,  0xFF,
    0xF3,  0x7E,  0xCB,  0xF9,  0x3F,  0xE7,  0x6C,  0x9F,
    0xF9,  0x3F,  0xE7,  0xFC,  0x9F,  0xF3,  0x7E,  0xCF,
    0xF9,  0x3F,  0xF7,  0xFC,  0x9F,  0xF3,  0x7F,  0xCF,
    0xEF,  0xFD,  0xAF,  0x77,  0xFE,  0xDF,  0xFA,  0x77,
    0xEF,  0xFD,  0xBF,  0xE7,  0xFE,  0xDF,  0xFB,  0x7E,
    0xEF,  0xFD,  0xBF,  0xF7,  0xFE,  0xDF,  0xFB,  0x7F,
    0xBF,  0xDF,  0xFB,  0xFD,  0xBF,  0xDF,  0xFB,  0xFD,
    0xF7,  0xFF,  0x7F,  0xEF,  0xFD,  0xFF,  0xDF,  0xFE,
    0xBF,  0xFF,  0xFB,  0xFF,  0xBF,  0xFF,  0xFB,  0xFF,
    0xFF,  0xEF,  0xDF,  0xFF,  0xFF,  0xFE,  0xFD,  0xFF,
0xFF,  0xFF,  0xDF,  0xFF,  0xFF,  0xFF,  0xFD,  0xFF};// light gray


void BuildGrayMap(LPBITMAPINFOHEADER lpBitmapInfoHeader, BYTE FAR *lpmap)
{
    int table_size;
    LPRGBQUAD lppal;

#ifdef DEBUG_FUNCT
    DB(("Entering BuildGrayMAp\n"));
#endif

    if (lpBitmapInfoHeader->biBitCount == 24)
        return;

    table_size = (1 << lpBitmapInfoHeader->biBitCount);
    lppal = (LPRGBQUAD)((LPSTR)lpBitmapInfoHeader +
            lpBitmapInfoHeader->biSize);

    while (table_size--) {
        *lpmap++ = INTENSITY(lppal->rgbRed,
        lppal->rgbGreen,
        lppal->rgbBlue);

        lppal++;
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting BuildGrayMAp\n"));
#endif
}

BYTE bit_index[] = {0x7F, 0xBF, 0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};


/*-------------------- HALFTONETODEVICE ----------------------*/

int PASCAL HalfToneToDevice (
LPDEVICE   lpdv  ,
WORD   DestX ,  WORD DestY ,
WORD   DestXE,  WORD DestYE,
WORD   SrcX  ,  WORD SrcY  ,
WORD   SrcXE ,  WORD SrcYE ,
LPSTR   lpBits,               // pointer to DIBitmap Bits
LPBITMAPINFOHEADER lpBitmapInfoHeader,   // pointer to DIBitmap info Block
LPDRAWMODE lpdm  ,
LPRECT    lpClip
)
{
BYTE huge     *lpbits;			/* let the compiler do seg stuff */
BYTE huge     *lplinestart;
int           bit_count;
int           scan_width;
RECT          dest_rect, clip_rect, white_rect, band_rect;
POINT         band_offset;
int           x_dst, y_dst;
BYTE FAR      *lp_dst_bits_start;
register BYTE FAR *lp_dst_bits;
unsigned int  scans, bit_offset;
BYTE          gray_map[256], byte; 	// NVF15NOV90
BYTE          use_low_nyble, start_low;
long          nextrow;				        // NVF15NOV90
int           xscaledown, yscaledown;
unsigned long cxscale, cxsample, cyscale, cysample,
              dxscale, dxsample, dyscale, dysample;
unsigned short IsLastRow,x_last,xoffset;
    static BOOL kludge_flag = TRUE;


//
//  Check for scaling down NVF 26DEC90
//  THE UNFORTUNATE DAY AFTER CHRISTMAS
//
    xscaledown = FALSE;
    yscaledown = FALSE;
    if (DestXE < SrcXE)
        xscaledown = TRUE;
    if (DestYE < SrcYE)
        yscaledown = TRUE;
//
// if we're doing a one bit DIB and scaling down our resolution,
// just Bitblt it
//
//    if ((lpdv->epScaleFac) && (lpBitmapInfoHeader->biBitCount))
//        return -1;


//
// Fill in the pointer to the destination bitmap
//
    if (RealMode)
        lpdv->epBmpHdr.bmBits=lpdv->epBmp;

//
// generate clippng rect based on band rect, dest rect and
// input clipping rect
//
    dest_rect.left = DestX;
    dest_rect.top  = DestY;
    dest_rect.right = DestX + DestXE;
    dest_rect.bottom = DestY + DestYE;

    clip_rect = *lpClip;
//
// make sure things are non empty, and bail if there is no work to be
// done
//
    if (!IntersectRect(&clip_rect, &clip_rect, &dest_rect))
        return 0;


    // More kludges for the 3.70 shipped driver.  Terry
    // This one adjusts the position of the last band for letter paper
    // This fix removes part of the white line between bands.

    if (((DestXE + DestX) == 1581) && (!RealMode) && (kludge_flag)
    && (lpdv->epType == (short)DEV_LAND)) {
        lpdv->epXOffset += 1;
        kludge_flag = FALSE;
        DBMSG(("First kludge fixup, xoffset incremented 1\n"));
    }


//
// offset these to account for band offset
//
    band_offset.xcoord = lpdv->epXOffset >> lpdv->epScaleFac;
    band_offset.ycoord  = lpdv->epYOffset >> lpdv->epScaleFac;

    OffsetRect(&clip_rect, -band_offset.xcoord, -band_offset.ycoord);
    OffsetRect(&dest_rect, -band_offset.xcoord, -band_offset.ycoord);

    // More kludges for the 3.70 shipped driver.  Terry
    if ((dest_rect.left < 0) && (!RealMode)
    && (lpdv->epType == (short)DEV_LAND))  {
        DBMSG(("DIBTODEV.C - 2nd Kludge clip moved %d, dest %d\n",
        -clip_rect.left, -dest_rect.left));
        OffsetRect (&clip_rect, -clip_rect.left, 0);
        OffsetRect (&dest_rect, -dest_rect.left, 0);
    }


    band_rect.left   = 0;
    band_rect.top    = 0;
    band_rect.right  = lpdv->epBmpHdr.bmWidth;
    band_rect.bottom = lpdv->epBmpHdr.bmHeight;

    IntersectRect(&white_rect, &band_rect, &dest_rect);
    IntersectRect(&white_rect, &clip_rect, &white_rect);

    /* Some non-well-behaved apps will try to do a dmBitblt, even though
    we told them not to yet.  Therefore, we must allocate some RAM
    now, before they kick us in the pants! */
    if(RealMode) lpdv->epBmpHdr.bmBits=lpdv->epBmp;
    allocate_mem(lpdv);

//
// opaque the background (make it white)
//
    dmBitblt((LPDEVICE)&lpdv->epBmpHdr, white_rect.left, white_rect.top,
             (BITMAP FAR *)NULL, 0, 0,  white_rect.right - white_rect.left,
                                        white_rect.bottom - white_rect.top,
             (LONG)WHITENESS, (void FAR *)NULL, (LPDRAWMODE)lpdm);





//------------------ deal with the clipping -----------------
    clip_rect = white_rect;
//
// now clip_rect holds actuall area where we are going to
// output to.  we need to translate this back into the
// DIB space, and setup initial error terms to account for
// scaling from the larger dest space to the smaller DIB
// space.
//
// transform the diff between the clip rect and the dest rect
// back into the DIB space to find the starting clipped pixel
// (this rounds down)
//
    SrcX += (WORD)ldiv(ldiv(lmul(lmul((long)(clip_rect.left - dest_rect.left),
                    (long)100), (long)SrcXE), (long)DestXE), (long)100);
//
// same thing in Y
//
   SrcY += (WORD)ldiv(ldiv(lmul(lmul((long)(dest_rect.bottom-clip_rect.bottom),
                           (long)100), (long)SrcYE), (long)DestYE), (long)100);
//
//---------------- get pointers to the DIB data --------------
// scan_width is the width of one scan line.
// DIB scanlines are on DWORD boundaries so this needs to be
// rounded up

    bit_count  = lpBitmapInfoHeader->biBitCount;
    scan_width = (((WORD)lpBitmapInfoHeader->biWidth * bit_count + 7)
                    / 8 + 3) & 0xFFFC;
//
// offset to start scan
//
    lplinestart = (BYTE huge *)lpBits;
    lplinestart += lmul((long)scan_width,(long)SrcY);
//
// and offset to starting pixel
// NOTE: this is upside down.  we need to run through the dib
// backwards to flip it right side up
//
    switch (bit_count) {
        case  1:
             lplinestart += SrcX / 8;
             start_low = (BYTE) SrcX & 7;
             break;
        case  4:
             lplinestart += SrcX / 2;
             start_low = (BYTE)SrcX & 1;
             break;
        case  8:
             lplinestart += SrcX;
             break;
        case 24:
             lplinestart += (WORD)MulDiv(SrcX, 3, 1);
             break;
        }

    BuildGrayMap(lpBitmapInfoHeader, gray_map);


/**************** BEGINNING OF THE SCALE CODE ****************/

// ----------------- deal with destination bitmap ----------------
// get pointer to destination huge bitmap, use bitmap segment info
// so we use far pointers (not huge)

    scans             = clip_rect.bottom;
    lp_dst_bits_start = lpdv->epBmpHdr.bmBits;
    nextrow           = lpdv->epBmpHdr.bmWidthBytes;   // NVF15NOV90

    if (lpdv->epBmpHdr.bmSegmentIndex) {
      while (scans > lpdv->epBmpHdr.bmScanSegment) {
            scans -= lpdv->epBmpHdr.bmScanSegment;
            lp_dst_bits_start=(LPSTR)MAKELONG(HIWORD(lp_dst_bits_start)
                             + lpdv->epBmpHdr.bmSegmentIndex, 0);
        }
    }
//
// Scans is really a displacenent into the destination bitmap. DIB
// processing is broken into managable segments, each segment causes
// a call to this program. The clipping rectangle is a window of what
// is to be processed in each call. The bottom of the clipping rect
// represents the deepest point of penetration into the dest bitmap
// for each call. The starting point of the dest bits is the beginning
// of the last line, so subtract 1 from scans before multiplying by
// the width.
//


    lp_dst_bits_start += ((scans - 1) * lpdv->epBmpHdr.bmWidthBytes);

    lp_dst_bits_start += clip_rect.left / 8;
    bit_offset         = clip_rect.left % 8;

       DBMSG(("Clip rect top= %d, bottom= %d, left= %d, right= %d\n",
        clip_rect.top, clip_rect.bottom, clip_rect.left,
        clip_rect.right));

            // allow the user to do something while we
            // rasterize.  this will slow things down
            // but it is necessary.

            if (lpdv->ephDC) {

                if (!QueryAbort(lpdv->ephDC, 0)) {
                    Control(lpdv, ABORTDOC, NULL, NULL);
                    goto QUIT;
                }
            }



    if (clip_rect.top < 0)
        clip_rect.top = 0;  // NVF 23OCT90 added check for clip_rect < 0

/*************** THE MEAT OF THE SCALE CODE *******************/
//
// start at the bottom, and go up, flipping the DIB
//
    dyscale  = DestYE;
    dysample = SrcYE ;
    cyscale  = dyscale ;
    cysample = dysample;

// This for loop is the one that prints each destination bitmap line.
// y_dst serves as both a line counter and the grayscale tileing
// reference point for the pattern orgin.

    for (y_dst  = clip_rect.bottom - 1;
         y_dst >= clip_rect.top;
         --y_dst  , cysample += dysample)
    {
        lp_dst_bits = lp_dst_bits_start;



/*************** GOTO THE NEXT LINE TO PRINT ******************/

        if (yscaledown) {
            while (cyscale < cysample) {
                lplinestart += scan_width;    // goto the next DIB line
                cyscale     += dyscale;
            }
        } else {
            if (cysample > cyscale) {
                lplinestart += scan_width;    // goto the next DIB line
                cyscale     += dyscale;
            }
        }

        lpbits = lplinestart;                 // the current DIB line

//
// HORIZONTAL DIRECTION STUFF
//
        x_dst    = clip_rect.left;

        dxscale  = DestXE  ;
        cxscale  = dxscale ;
        dxsample = SrcXE   ;
        cxsample = dxsample;

        switch (bit_count) {
            case 1 : use_low_nyble = (BYTE)7 - start_low; break;
            case 4 : use_low_nyble = start_low          ; break;
        }

/******************** PRINT A ROW OF PIXELS ***********************/

               if (y_dst < (clip_rect.top + ((SrcYE + (DestYE - 1))/DestYE)))
               {
                 IsLastRow = 1;

             xoffset = clip_rect.right;

             switch (bit_count)
             {
               case 1:
                  clip_rect.right -= ldiv(lmul(8L,cxscale) +
                              (cxsample - 1L),cxsample);

                  break;
               case 4:
                  clip_rect.right -= ldiv(lmul(2L,cxscale) +
                              (cxsample - 1L),cxsample);

                  break;
               case 8:
                  clip_rect.right -= ldiv(cxscale +
                              (cxsample - 1L),cxsample);
                  break;
               case 24:
                  clip_rect.right -= (WORD)MulDiv((ldiv(cxscale
                              + (cxsample - 1),cxsample) + 2),1,3);
                  break;
              }

            xoffset -= clip_rect.right;
           }
           else
                   IsLastRow = 0;


// START OF AN X ROW PROCESSING 

// The code from here to the end of the program is not structured code
// for performance reasons. This code is very heavily exercised - it 
// dithers and places every bit in a destination bitmap. 


// Process one bit per pixel bitmaps.
 if (bit_count == 1) { 

   if (xscaledown) {   // Scale Down case. 

        for (;
	     x_dst < clip_rect.right;
	     ++x_dst, cxsample += dxsample)
        {
           while (cxscale < cxsample) {
              if (!use_low_nyble) {
	           ++lpbits;
                 use_low_nyble = 7;
              } else
                 --use_low_nyble;

              cxscale += dxscale;
           }    // end of cxscale < cxsample while loop 
//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//

           
            if (!gray_map[(*lpbits >> use_low_nyble) & 1])
                *lp_dst_bits &= bit_index[bit_offset];



 /*---------- update bit offsets in dest. bitmap --------*/
           bit_offset++;
           if (bit_offset == 8) {
	        bit_offset = 0;
              lp_dst_bits++;
           }



        }  // end of x_dst for loop 

    } else {   // Scale Up case 

        for (;
	     x_dst < clip_rect.right;
	     ++x_dst, cxsample += dxsample)
        {



           if (cxsample > cxscale) {
              if (!use_low_nyble) {
                 ++lpbits;
                 use_low_nyble = 7;
              } else
                 --use_low_nyble;
              cxscale += dxscale;
           } // end of cxsample > cxscale if statement


//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//


           if (!gray_map[(*lpbits >> use_low_nyble) & 1])
               *lp_dst_bits &= bit_index[bit_offset];



 /*---------- update bit offsets in dest. bitmap --------*/
           bit_offset++;
           if (bit_offset == 8) {
               bit_offset = 0;
               lp_dst_bits++;
           }

      }  // end of x_dst for loop 
   }  // end of scaling if statement 

 } // end of single bit per pixel case. 
 





// Process four bit per pixel bitmaps.
  else if (bit_count == 4)  {

   if (xscaledown) {   // Scale Down case. 

        for (;
	     x_dst < clip_rect.right;
           ++x_dst, cxsample += dxsample)
        {

        while (cxscale < cxsample) {
           if (use_low_nyble) { //should be "used"_low_nyble
              use_low_nyble = FALSE;
              lpbits++;
           } else
              use_low_nyble = TRUE;

           cxscale += dxscale;
        }    // end of cxscale < cxsample while loop 
//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//

        if (use_low_nyble)
           byte = gray_map[*lpbits & 0x0F];
        else
           byte = gray_map[*lpbits >> 4];

/*------- dither and place pixel in destination bitmap ------*/
 if (byte == 0xFF); //  white  
 else if (byte == 0x00) //  black 
    *lp_dst_bits &= bit_index[bit_offset];
 else if (!(global_brighten || global_grayscale)) {//hand tuned bitmaps
         if (!((bit_patterns_16 [(int)((byte >> 4) << 3)
            + (y_dst % 8)]) & (0x80u >> (x_dst % 8))))
            *lp_dst_bits &= bit_index[bit_offset];
    } else {

       if (global_brighten) {  // GSDs lightened images for ScanJet 
          if (byte < LSCRN[SCREENX(x_dst)][SCREENY(y_dst)])
              *lp_dst_bits &= bit_index[bit_offset];
       } else {
          //Clustered dot dither (photographic images)
          if (byte < SCREEN[SCREENX(x_dst)][SCREENY(y_dst)])
             *lp_dst_bits &= bit_index[bit_offset];
       
       }
    }


 /*---------- update bit offsets in dest. bitmap --------*/
        bit_offset++;
        if (bit_offset == 8) {
           bit_offset = 0;
           lp_dst_bits++;
        }

      }  // end of x_dst for loop 

    } else {   // Scale Up case 

        for (;
	     x_dst < clip_rect.right;
	   ++x_dst, cxsample += dxsample)
        {



        if (cxsample > cxscale) {
           if (use_low_nyble) { // should be "used"_low_nyble
              use_low_nyble = FALSE;
              lpbits++;
           } else
              use_low_nyble = TRUE;

           cxscale += dxscale;
        } // end of cxsample > cxscale if statement


//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//
/*---------------------------- get pixel ----------------------------*/

        if (use_low_nyble)
            byte = gray_map[*lpbits & 0x0F];
        else
            byte = gray_map[*lpbits >> 4];


/*------- dither and place pixel in destination bitmap ------*/
 if (byte == 0xFF); //  white  
 else if (byte == 0x00) //  black 
    *lp_dst_bits &= bit_index[bit_offset];
 else if (!(global_brighten || global_grayscale)) {//hand tuned bitmaps
         if (!((bit_patterns_16 [(int)((byte >> 4) << 3)
            + (y_dst % 8)]) & (0x80u >> (x_dst % 8))))
            *lp_dst_bits &= bit_index[bit_offset];
    } else {

       if (global_brighten) {  // GSDs lightened images for ScanJet 
          if (byte < LSCRN[SCREENX(x_dst)][SCREENY(y_dst)])
              *lp_dst_bits &= bit_index[bit_offset];
       } else {
          //Clustered dot dither (photographic images)
          if (byte < SCREEN[SCREENX(x_dst)][SCREENY(y_dst)])
             *lp_dst_bits &= bit_index[bit_offset];
       
       }
    }


 /*---------- update bit offsets in dest. bitmap --------*/
        bit_offset++;
        if (bit_offset == 8) {
           bit_offset = 0;
           lp_dst_bits++;
        }

      }  // end of x_dst for loop 
    }  // end of scaling if statement 
  } // end of bit count if statement






// Process eight bit per pixel bitmaps.
  else if (bit_count == 8)     { // 8 bits per pixel 
   if (xscaledown) {   // Scale Down case. 

      for (;
          x_dst < clip_rect.right;
          ++x_dst, cxsample += dxsample)
      {

         while (cxscale < cxsample) {
            ++lpbits;
            cxscale += dxscale;
         }    // end of cxscale < cxsample while loop 
//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//

          byte = gray_map[*lpbits];


/*------- dither and place pixel in destination bitmap ------*/
 if (byte == 0xFF); //  white  
 else if (byte == 0x00) //  black 
    *lp_dst_bits &= bit_index[bit_offset];
 else if (!(global_brighten || global_grayscale)) {//hand tuned bitmaps
         if (!((bit_patterns_32 [(int)((byte >> 3) << 3)
            + (y_dst % 8)]) & (0x80u >> (x_dst % 8))))
            *lp_dst_bits &= bit_index[bit_offset];
    } else {

       if (global_brighten) {  // GSDs lightened images for ScanJet 
          if (byte < LSCRN[SCREENX(x_dst)][SCREENY(y_dst)])
              *lp_dst_bits &= bit_index[bit_offset];
       } else {
          //Clustered dot dither (photographic images)
          if (byte < SCREEN[SCREENX(x_dst)][SCREENY(y_dst)])
             *lp_dst_bits &= bit_index[bit_offset];
       
       }
    }



 /*---------- update bit offsets in dest. bitmap --------*/
          bit_offset++;
	    if (bit_offset == 8) {
	        bit_offset = 0;
              lp_dst_bits++;
 	    }


       }  // end of x_dst for loop 

    } else {   // Scale Up case 

        for (;
	     x_dst < clip_rect.right;
           ++x_dst, cxsample += dxsample)
        {

        if (cxsample > cxscale) {
           ++lpbits;
           cxscale += dxscale;
        } // end of cxsample > cxscale if statement


//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//
/*---------------------------- get pixel ----------------------------*/


         byte = gray_map[*lpbits];


/*------- dither and place pixel in destination bitmap ------*/
 if (byte == 0xFF); //  white  
 else if (byte == 0x00) //  black 
    *lp_dst_bits &= bit_index[bit_offset];
 else if (!(global_brighten || global_grayscale)) {//hand tuned bitmaps
         if (!((bit_patterns_32 [(int)((byte >> 3) << 3)
            + (y_dst % 8)]) & (0x80u >> (x_dst % 8))))
            *lp_dst_bits &= bit_index[bit_offset];
    } else {

       if (global_brighten) {  // GSDs lightened images for ScanJet 
          if (byte < LSCRN[SCREENX(x_dst)][SCREENY(y_dst)])
              *lp_dst_bits &= bit_index[bit_offset];
       } else {
          //Clustered dot dither (photographic images)
          if (byte < SCREEN[SCREENX(x_dst)][SCREENY(y_dst)])
             *lp_dst_bits &= bit_index[bit_offset];
       
       }
    }


 /*---------- update bit offsets in dest. bitmap --------*/
         bit_offset++;
         if (bit_offset == 8) {
	      bit_offset = 0;
	      lp_dst_bits++;
         }

       }  // end of x_dst for loop 
    }  // end of scaling if statement 
 } // end of 8 bit count if statement




// Process twenty four bit per pixel bitmaps.
  else if (bit_count == 24)   {  // 24 bits per pixel
   if (xscaledown) {   // Scale Down case. 

        for (;
	      x_dst < clip_rect.right;
            ++x_dst, cxsample += dxsample)
        {

	     while (cxscale < cxsample) {
              lpbits += sizeof(RGBTRIPLE);
              cxscale += dxscale;
           }    // end of cxscale < cxsample while loop 
//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//
/*---------------------------- get pixel ----------------------------*/


            byte = INTENSITY(((LPRGBTRIPLE)lpbits)->rgbtRed,
                   ((LPRGBTRIPLE)lpbits)->rgbtGreen,
                   ((LPRGBTRIPLE)lpbits)->rgbtBlue);

/*------- dither and place pixel in destination bitmap ------*/
 if (byte == 0xFF); //  white  
 else if (byte == 0x00) //  black 
    *lp_dst_bits &= bit_index[bit_offset];
 else if (!(global_brighten || global_grayscale)) {//hand tuned bitmaps
         if (!((bit_patterns_32 [(int)((byte >> 3) << 3)
            + (y_dst % 8)]) & (0x80u >> (x_dst % 8))))
            *lp_dst_bits &= bit_index[bit_offset];
    } else {

       if (global_brighten) {  // GSDs lightened images for ScanJet 
          if (byte < LSCRN[SCREENX(x_dst)][SCREENY(y_dst)])
              *lp_dst_bits &= bit_index[bit_offset];
       } else {
          //Clustered dot dither (photographic images)
          if (byte < SCREEN[SCREENX(x_dst)][SCREENY(y_dst)])
             *lp_dst_bits &= bit_index[bit_offset];
       
       }
    }


 /*---------- update bit offsets in dest. bitmap --------*/
            bit_offset++;
            if (bit_offset == 8) {
               bit_offset = 0;
               lp_dst_bits++;
            }


       }  // end of x_dst for loop 

    } else {   // Scale Up case 

        for (;
	     x_dst < clip_rect.right;
           ++x_dst, cxsample += dxsample)
        {

        if (cxsample > cxscale) {
           lpbits += sizeof(RGBTRIPLE);
	     cxscale += dxscale;
        } // end of cxsample > cxscale if statement


//
// At this point, we have the next pixel to print, so get it, dither it,
// and then print it.
//
/*---------------------------- get pixel ----------------------------*/

        byte = INTENSITY(((LPRGBTRIPLE)lpbits)->rgbtRed,
              ((LPRGBTRIPLE)lpbits)->rgbtGreen,
              ((LPRGBTRIPLE)lpbits)->rgbtBlue);


/*------- dither and place pixel in destination bitmap ------*/
 if (byte == 0xFF); //  white  
 else if (byte == 0x00) //  black 
    *lp_dst_bits &= bit_index[bit_offset];
 else if (!(global_brighten || global_grayscale)) {//hand tuned bitmaps
         if (!((bit_patterns_32 [(int)((byte >> 3) << 3)
            + (y_dst % 8)]) & (0x80u >> (x_dst % 8))))
            *lp_dst_bits &= bit_index[bit_offset];
    } else {

       if (global_brighten) {  // GSDs lightened images for ScanJet 
          if (byte < LSCRN[SCREENX(x_dst)][SCREENY(y_dst)])
              *lp_dst_bits &= bit_index[bit_offset];
       } else {
          //Clustered dot dither (photographic images)
          if (byte < SCREEN[SCREENX(x_dst)][SCREENY(y_dst)])
             *lp_dst_bits &= bit_index[bit_offset];
       
       }
    }


 /*---------- update bit offsets in dest. bitmap --------*/
        bit_offset++;
        if (bit_offset == 8) {
            bit_offset = 0;
            lp_dst_bits++;
        }

     }  // end of x_dst for loop 
   }  // end of scaling if statement 
 }  // end of 24 bits per pixel if statement


// END OF X ROW PROCESSING 

	if (IsLastRow)
	{


       for (x_last = 0; x_last < xoffset;x_last++)
         {
            if (determine_bit(bit_count, byte, x_dst, y_dst))
                *lp_dst_bits &= bit_index[bit_offset];

 /*---------- update bit offsets in dest. bitmap --------*/
            bit_offset++;
            if (bit_offset == 8) {
               bit_offset = 0;
		lp_dst_bits++;
	      }

         }
	}

//
// At this point we have finished printing a row of pixels, so update
// the destination pointer to the next row
//
	scans--;


	if (scans == 0) {
	    // step back to previous segment
	    scans = lpdv->epBmpHdr.bmScanSegment;

	    lp_dst_bits_start=(BYTE FAR *)MAKELONG(HIWORD(lp_dst_bits_start)
	                     - lpdv->epBmpHdr.bmSegmentIndex,
			     lpdv->epBmpHdr.bmWidthBytes * (scans - 1));
	    lp_dst_bits_start += clip_rect.left / 8;

	} else {
	    // within the same segment NVF15NOV90
          lp_dst_bits_start -= lpdv->epBmpHdr.bmWidthBytes;
        }
	bit_offset = clip_rect.left % 8;
    }

   QUIT:

#ifdef DEBUG_FUNCT
    DB(("Exiting HalfToneToDevicen"));
#endif
   return SrcYE;
 }


BOOL determine_bit(int bit_cnt, BYTE intensity_byte, int x_dst, int y_dst)
{
#ifdef DEBUG_FUNCT
    DB(("Entering determine_bit\n"));
#endif

    if (global_brighten)    // lighten the gray using GSD's big dot patterns.
        return (intensity_byte < LSCRN[SCREENX(x_dst)][SCREENY(y_dst)]);

    if (global_grayscale)   // use big dots for the grayscales.
        return (intensity_byte < SCREEN[SCREENX(x_dst)][SCREENY(y_dst)]);


    switch (bit_cnt)
    {

        case 1: 
            if (intensity_byte)
                return FALSE;       // white
            else
                return TRUE;        // black

            break;



        case 4:
            // We are limited to 16 grayscales, so bit_pattern_16 contains the
            // best looking 16 gray scales out of the available 32.


            if (intensity_byte == 0xFF)  // The only valid white value.
                return FALSE;
            else
                return !((bit_patterns_16 [(int)((intensity_byte / 16) * 8)
                + (y_dst % 8)]) & (0x80u >> (x_dst % 8)));

            break;



        case 8:

            if (intensity_byte == 0xFF)  // The only valid white value.
                return FALSE;
            else
                return !((bit_patterns_32 [(int)((intensity_byte / 8) * 8)
                + (y_dst % 8)]) & (0x80u >> (x_dst % 8)));

            break;


        case 24:

            if (intensity_byte == 0xFF)  // The only valid white value.
                return FALSE;
            else
                return !((bit_patterns_32 [(int)((intensity_byte / 8) * 8)
                + (y_dst % 8)]) & (0x80u >> (x_dst % 8)));


            break;


        default:

            return (FALSE);

            break;
    }
#ifdef DEBUG_FUNCT
    DB(("Exiting determine_bit\n"));
#endif

}


