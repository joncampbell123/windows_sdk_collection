/************************************************************************
 * strblt.c	STRETCHBLT escape support
 *
 * mod history:
 *
 * 	88Jan02 chrisg	removed from bitblt.c
 * 			this code  resembles that in bitblt.
 *			mods made there should be reflected here.
 *
 *	88Jan10 chrisg	added non byte alligned source pixels
 *
 *	90Feb15	chrisg	> 64k bitmap support
 *
 ***********************************************************************/

#include "pscript.h"
#include "atstuff.h"
#include "debug.h"
#include "utils.h"
#include "channel.h"
#include "printers.h"
#include "getdata.h"
#include "resource.h"
#include "graph.h"
#include "psdata.h"


int FAR PASCAL Output(LPPDEVICE, int, int, LPPOINT, LPPEN, LPBR, LPDRAWMODE, LPRECT);


#define SHAPE_SOURCE	/* enable source shapping */

#define DOIT_GDI	-1	/* return code, GDI should do it */

extern char image_proc[];
extern char restore[];
extern char str_def[];

typedef BYTE *PBYTE;
typedef  BYTE  FAR *  LPBYTE;

void FAR PASCAL RunEncode(LPDV lpdv, PBYTE lpbytes, int len);

void FAR PASCAL OpaqueBox(LPDV, BOOL, int, int, int, int, DWORD);	/* text.c */


#ifdef SHAPE_SOURCE

#define WHITE	0x0ff			/* white byte in a bitmap */

/*
 * BOOL PASCAL ShapeSource()
 *
 * note the standard convention is
 * a rectangle {1,1,1,1} specifies a null rectangle
 * XE = right - left.  This is adhered to except for boundingRect.
 *
 * Clip requested srcRect with bitmap.
 * If rop is SRCAND, then
 * adjust a rectangle in a bitmap so that it bounds all non white
 * data. this is to reduce output size for bitblt and strechblt.
 * this is for 1 bit per pixel bitmaps. small or huge
 *
 * in:
 *	bitmap and the dimentions of what is going to be blted
 *  and rop
 *
 * out:
 *	adjusted values reduced to minimal bounding box that
 *	encloses the bitmap (to byte accuracy on left and right)
 *
 * returns:
 *	TRUE	there is some something in this bitmap (do output)
 *	FALSE	this is a blank bitmap (don't bother doing output)
 *
 * restrictions:  B & W bitmaps only, 1 bit per pixel, 1 plane.
 *
 * PeterWo: totally rewritten to properly handle
 *  partial left and right scanline bytes
 *  and multisegment bitmaps.
 *  Clipping and etc.
 */




void  NEAR  PASCAL  updateBounds(lpBRect, x,y)
LPRECT  lpBRect;
int   x, y;
{
    if(lpBRect->left)
    {
        if(x < lpBRect->left)
            lpBRect->left = x;
        else if(x > lpBRect->right)
            lpBRect->right = x;

        if(y > lpBRect->bottom)
            lpBRect->bottom = y;
        // note .top is set by the first blot and
        // never needs to be updated.
    }
    else  // first blot found on the bitmap
    {
        lpBRect->right = lpBRect->left = x;
        lpBRect->top = lpBRect->bottom = y;
    }
}


void  NEAR  PASCAL  updateBounds(
    LPRECT  lpBRect,
    int   x, 
    int   y
    );


BOOL PASCAL ShapeSource(
	LPBITMAP lpbm, 
	int FAR *lpSrcX, 
	int FAR *lpSrcY, 
	int FAR *lpSrcXE, 
	int FAR *lpSrcYE,
    DWORD  dwRop)
{
    BYTE  FAR * lpBitMap;  //  this points to current byte
                          // that we are examining in the source
                         // bitmap.
    RECT  boundingRect,  //  this holds coordinates of minimal
                         //  bounding rectangle containing all ink.  
                         //  The X coordinate is in units of bitmap
                         //  bytes.  The Y coordinate is scanlines.
                         //  Note x and y are indexed from 1.
                         //  left = right  indicates a width of 1
                         //  The upper left corner of the Clipped
                         //  SrcRect is (1, 1).  Later converted to
                         //  bitmap coordinates.
          bitmapRect,    //  this bounds the entire source bitmap
          srcRect;       //  this bounds the src region for copy
                         //  is subsequently clipped to bitmapRect.

    WORD  clippedXE,     //  width of the clipped srcRect.
        emptyBits,      // number of unused bits in the leftmost
                        // byte of the clipped src scanline.
        rightBits,      // number of bits contained in the
                        // rightmost byte of the clipped src scanline
        coreLen,        // number of full bytes in the src Scanline.
        srcScansRem,    // number of scanlines remaining to be examined
        YStart,         // used to calculate y position
                        // y = YStart - srcScansRem;   y is one based.
        byteOffset,     // ignore the first few bytes of each scanline.
        scanOffset,     // ignore the first few scanlines of the
                        // first segment we read from.
        segScansRem;    // how many scanlines remaining in the
                        // current segment.

    BYTE  leftMask, rightMask;   // mask bytes to be OR'ed with
                        // partial bytes 

    boundingRect.left = boundingRect.top =
            boundingRect.right = boundingRect.bottom = 0;

    bitmapRect.left = bitmapRect.top = 0;
    bitmapRect.right = lpbm->bmWidth;
    bitmapRect.bottom = lpbm->bmHeight;

    srcRect.left = *lpSrcX;
    srcRect.top = *lpSrcY;
    srcRect.right = *lpSrcX + *lpSrcXE;
    srcRect.bottom = *lpSrcY + *lpSrcYE;

    if(!IntersectRect(&srcRect, &srcRect, &bitmapRect))
        return(FALSE);

    if(dwRop != SRCAND)
    {
        //  must return clipped src rectangle

        *lpSrcX = srcRect.left;
        *lpSrcY = srcRect.top;
        *lpSrcXE = srcRect.right - srcRect.left;
        *lpSrcYE = srcRect.bottom - srcRect.top;
        return(TRUE);     //  don't strip white space for other rops
    }

    clippedXE = srcRect.right - srcRect.left;

    // count number of full bytes in clipped Src scanline
    // we will now refer only to the clipped src so 
    // drop the qualifier 'clipped'

    emptyBits = srcRect.left % 8 ;

    rightBits = srcRect.right % 8 ;

    coreLen = (srcRect.right / 8) - (srcRect.left / 8);
    if(coreLen)
        coreLen -= (emptyBits) ? (1) : (0);
    
    //  shift a word to avoid inadvertant sign extension.

    leftMask = (BYTE)~(0x00ff >> emptyBits);
    rightMask = (BYTE)(0x00ff >> rightBits);


    //  at this point lpBitMap should point to 
    //  first byte in the bitmap.
    lpBitMap = lpbm->bmBits;

    srcScansRem = srcRect.bottom - srcRect.top;
    YStart = srcScansRem + 1;  //  x, y cooridnates are 1 based

    byteOffset = srcRect.left / 8;

    //  Lord, my kingdom for a large linear address space!

    if(lpbm->bmSegmentIndex)  // multi-segment bitmap
    {
        WORD  segOffset;  //  how many segments do we skip?

        segOffset = srcRect.top / lpbm->bmScanSegment;
        scanOffset = srcRect.top % lpbm->bmScanSegment;
        segScansRem = lpbm->bmScanSegment - scanOffset;
                    
        //  move to appropriate segment
        lpBitMap = (LPBYTE)((DWORD) lpBitMap + 
            ((DWORD)(lpbm->bmSegmentIndex * segOffset) << 16)) ;
    }
    else
    {
        scanOffset = srcRect.top;
        segScansRem = srcScansRem;
    }
    lpBitMap += scanOffset * lpbm->bmWidthBytes;



    for( ; srcScansRem > 0 ; lpBitMap = (LPBYTE)((DWORD) lpBitMap + 
            ((DWORD)lpbm->bmSegmentIndex << 16))   )
    {
        lpBitMap += byteOffset;

        for( ; srcScansRem > 0  &&  segScansRem > 0 ;
                segScansRem--, srcScansRem--)
        {
            BYTE  FAR *lpByte = lpBitMap;
            WORD  x = 1, y = YStart - srcScansRem, i;
            //  x and y define a coordinate system for
            // the bytes of the BitMap, where the
            // upper left hand corner of the clipped
            // SrcRect is (1, 1)

            if(emptyBits)  //  we have a partial leftByte
            {
                if((leftMask | *lpByte) != WHITE)
                    updateBounds(&boundingRect, x, y);
                lpByte++;
                x++;
            }
            for(i = 0 ; i < coreLen ; i++)
            {
                if(*lpByte != WHITE)
                    updateBounds(&boundingRect, x, y);
                lpByte++;
                x++;
            }
            if(rightBits)  //  we have a partial rightByte
            {
                if((rightMask | *lpByte) != WHITE)
                    updateBounds(&boundingRect, x, y);
            }

            lpBitMap += lpbm->bmWidthBytes;  // next scan line
        }
        lpBitMap = (LPBYTE)((DWORD)lpBitMap & 0xFFFF0000);
        //  reset offset.
        segScansRem = lpbm->bmScanSegment;
    }

    //  lpBitMap and segScansRem are meaningless now

    // convert byte cooridinates to src bitmap cooridinates
    
    if(boundingRect.left == 0)        //  no ink found
        return (FALSE);
    else if(boundingRect.left == 1)
        boundingRect.left = 0;
    else 
    {
        boundingRect.left--;     //  return to zero based
        boundingRect.left *= 8;  // convert bytes to bits
        boundingRect.left -= emptyBits;
    }

    boundingRect.right *= 8;
    boundingRect.right -= emptyBits;
    if((int)clippedXE < boundingRect.right)
        boundingRect.right = clippedXE;

    // left and right are now zero based pixel units
    *lpSrcX = srcRect.left + boundingRect.left;
    *lpSrcXE = boundingRect.right - boundingRect.left;

    boundingRect.top--;  // back to zero based

    *lpSrcY = srcRect.top + boundingRect.top;
    *lpSrcYE = boundingRect.bottom - boundingRect.top;

    return(TRUE);
}



#endif


/****************************************************************************
 * BOOL FAR PASCAL StrechBlt()
 *
 * This routine handles the the GDI StretchBlt routine. 
 * uses "image" operator to stretch a bitmap.
 *
 * if we return DOIT_GDI GDI will do the stretching for us.  this is
 * probally not a good thing.  so, we should probally tell GDI that
 * we do everything (except memory to memory stretching) and convert
 * to one of the rops we can do or faily quietly. if GDI has to do this
 * stuff we generate tons of data and everyone will be unhappy.
 *
 * in:
 *	lpdv	points to the device to recieve output
 *
 * restrictions:
 *	bmBitsPer pixel must be 1.  (use DIBs to get more)
 *
 *	dwRop supported:
 *		SRCCOPY		src -> dst
 *		NOTSRCCOPY	(NOT src) -> dst
 *		SRCAND	src AND dst -> dst
 *		NOTSRCAND	(NOT src) AND dst -> dst
 *
 * returns:
 *	TRUE		we did it
 *	FALSE		we couldn't do it
 *
 *	DOIT_GDI(-1)	if GDI should do this for us
 *
 ***************************************************************************/

#define  NOTSRCAND  0x00220326


int FAR PASCAL StretchBlt(
	LPPDEVICE lpdv,
	int DstX,
	int DstY,
	int DstXE,
	int DstYE,
	LPBITMAP lpbm,		/* source bitmap */
	int SrcX,
	int SrcY,
	int SrcXE,
	int SrcYE,
	DWORD rop,
	LPBR lpbr,
	LPDRAWMODE lpdm,	/* what do i do with this? */
	LPRECT lpClip)
{
	LPSTR   lpBits;
	int	byte_width;	/* bitmap width in bytes */
	int	pix_extra;	/* used for non byte alligned source */
	RECT	rect;		/* used for clipping */
	int	dx, dy;		/* pre Shapped values */
	int	x, y;
	HANDLE	hBuf;
	BYTE	*pBuf;
	unsigned scans_this_seg, scans;
        BOOL    bEncode;
        int     bmExtra;



	ASSERT(lpdv);

	// is DEST memory?

	if (!lpdv->iType)
		return DOIT_GDI;	/* let GDI do it */


	DBMSG(("StretchBlt() DstX:%d DstY:%d DstXE:%d DstYE:%d\n", 
		DstX, DstY, DstXE, DstYE));

#ifdef PS_IGNORE
       	if (lpdv->fSupressOutput)
       		return 1;
#endif

	// is SRC device?

	if (lpbm && lpbm->bmType) {
		DBMSG(("StretchBlt() source is device\n"));
		return FALSE;		// not even GDI can help us here
	}

	// from here down SRC must be memory or brush

	// special case these easy brushes and ROPs

	if (rop == BLACKNESS || rop == WHITENESS) {
		OpaqueBox(lpdv, FALSE, DstX, DstY, SrcXE, SrcYE,
			(rop == WHITENESS) ? 0x00FFFFFF : 0L);
		return TRUE;
	}

	if (rop == PATCOPY) {

		rect.left = DstX;
		rect.top  = DstY;
		rect.right  = DstX + SrcXE;
		rect.bottom = DstY + SrcYE;

		Output(lpdv, OS_RECTANGLE, 2, (LPPOINT)&rect, NULL, lpbr, lpdm, NULL);

		return TRUE;
	}

	// from here down only accept memory sources

	if (!lpbm)
		return FALSE;	// no funny PAT stuff

	DBMSG(("StretchBlt(): src planes:%d bits:%d dx:%d dy:%d\n",
		lpbm->bmPlanes,
		lpbm->bmBitsPixel,
		lpbm->bmWidth,
		lpbm->bmHeight));

	// and no bogus memory stuff

	if (lpbm->bmBitsPixel != 1 || lpbm->bmPlanes != 1) {
		DBMSG(("More than one bit image!\n"));
		return FALSE;
	}

	// and hey! no silly stuff that I just can't do!

	if ((rop != SRCCOPY) && (rop != NOTSRCAND) &&
	    (rop != NOTSRCCOPY) && (rop != SRCAND)) {
	    	DBMSG(("warning, rop converted\n"));
	    	// rop = SRCCOPY;		/* convert to something we can handle */
	    	rop = SRCAND;		/* convert to something we can handle */
	}

	/* rect in defines destination */

	rect.left   = DstX;
	rect.top    = DstY;
	rect.right  = DstX + DstXE;
	rect.bottom = DstY + DstYE;

	/* merge destination and clip rect */

	if (IntersectRect(&rect, &rect, lpClip) == 0) {
		return TRUE;	/* empty clip rect */
	}

	ClipBox(lpdv, &rect);

	// wierd hack: the oqaque rect does not seem to get clipped the
	// same as the image operator.  it seems to draw slightly outside
	// the clip rect (define above).  to account for this bump it down
	// and shrink it by a pixel.

	if (rop == SRCCOPY || rop == NOTSRCCOPY)
		OpaqueBox(lpdv, FALSE, rect.left + 1, rect.top + 1,
			rect.right - rect.left - 1, rect.bottom - rect.top - 1,
			lpdm->bkColor);

#ifdef SHAPE_SOURCE

	x = SrcX;
	y = SrcY;
	dx = SrcXE;
	dy = SrcYE;

	DBMSG(("Before Shape %d %d %d %d\n", SrcX, SrcY, SrcXE, SrcYE));

	if (!ShapeSource(lpbm, &SrcX, &SrcY, &SrcXE, &SrcYE, rop)) {
		DBMSG(("empty bitmap\n"));
		goto EXIT;
	}

	DBMSG(("After  Shape %d %d %d %d\n", SrcX, SrcY, SrcXE, SrcYE));

#endif

        /* Determine if bitmap should be encoded */
        bEncode = lpdv->bCompress;

	// calc the byte width accounting for non byte aligned start of
	// the scan and rounding up for the padding in the last byte
	// of the scan

	// round the last point up to byte boundry, the last down

	byte_width = ((((SrcX + SrcXE - 1) | 0x0007) - (SrcX & 0xFFF8)) + 7) / 8;

	DBMSG(("byte_width = %d\n", byte_width));

	/* compute # of non byte alligned pixels */

	pix_extra = SrcX % 8;

	DBMSG(("pix_extra = %d\n", pix_extra));

	/* this byte_width is how big the string (and encode buffer) are */

	PrintChannel(lpdv, str_def, byte_width);

	if (!(hBuf = LocalAlloc(LPTR, byte_width)))
		goto ERR_EXIT;

	if (!(pBuf = LocalLock(hBuf))) {
		LocalFree(hBuf);
		goto ERR_EXIT;
	}

        if (bEncode)
	    DumpResourceString(lpdv, PS_DATA, PS_UNPACK);

	PrintChannel(lpdv, "%d %d %d sc\n",
		GetRComponent(lpdv, lpdm->TextColor),
		GetGComponent(lpdv, lpdm->TextColor),
		GetBComponent(lpdv, lpdm->TextColor));

	PrintChannel(lpdv, "save %d %d translate %d %d scale\n", DstX, DstY, DstXE, DstYE);

        /* Bracket image with %%BeginData / %%EndData pair */
        PrintChannel(lpdv, "%%%%BeginData: %d ASCII Lines\n", 1 + SrcYE);

	// PrintChannel(lpdv, "%d %d ", byte_width * 8 + pix_extra, SrcYE);
	PrintChannel(lpdv, "%d %d ", SrcXE + pix_extra, SrcYE);

	PrintChannel(lpdv, rop == NOTSRCAND || rop == NOTSRCCOPY ?
		(LPSTR) "true" : (LPSTR) "false");


    /*  true  means paint 1 bits with black, 
     *  ignore 0 bits in the mask,
     *  false  means  paint 0 bits in the mask with black,
     *  and ignore 1 bits.
     */

	/* this matrix maps the destination space into the coord system
	 * of the image.  this space is reduced by the scaling factor
	 * (DstXE, DstYE) and then transformed by the image matrix.
	 * we add an extra translation to the image matrix (pix_extra)
	 * to correct for non byte alligned source pixels (in X). */

#ifdef SHAPE_SOURCE

	/* since we reshaped the source we have to change the mapping
	 * so that it accounts for the offset of the smaller source
	 * (we translate by the difference between the old SrcX, SrcY
	 * and the new one (adjusted by ShapeSource)) */

	PrintChannel(lpdv, " [%d 0 0 %d %d %d] ", dx, dy,
		pix_extra + x - SrcX, y - SrcY);
#else
	PrintChannel(lpdv, " [%d 0 0 %d %d 0] ", SrcXE, SrcYE, pix_extra);
#endif

        if (bEncode) {
	    PrintChannel(lpdv, "{unpack} bind imagemask\n");
        } else {
	    PrintChannel(lpdv, image_proc);
	    PrintChannel(lpdv, "imagemask\n");
        }

	/* calc offset into source bitmap */

	scans = SrcY;
	lpBits = lpbm->bmBits;

	// > 64k?

	if (lpbm->bmSegmentIndex) {

		// yes, offset the bits pointer to the proper segment

		while (scans >= lpbm->bmScanSegment) {
			scans -= lpbm->bmScanSegment;
			lpBits = (LPSTR)MAKELONG(0, HIWORD(lpBits) + lpbm->bmSegmentIndex);
		}

	}

	// use segment adjusted values to calc the starting point

	lpBits = lpBits + SrcX / 8 + scans * lpbm->bmWidthBytes;

	// now supports > 64k bitmaps

	scans = SrcYE;	// total number of scan lines to do

        bmExtra = SrcY;

	while (scans) {

		if (lpbm->bmSegmentIndex)
			scans_this_seg = min(scans, lpbm->bmScanSegment - bmExtra);
		else
			scans_this_seg = scans;
                bmExtra = 0;

		DBMSG(("scans_this_seg = %d\n", scans_this_seg));

                ASSERT((int)scans_this_seg == scans_this_seg);

		for (y = 0; y < (int)scans_this_seg; y++) {

                        if (bEncode) {
			    lmemcpy(pBuf, lpBits, byte_width);
			    RunEncode(lpdv, pBuf, byte_width);
			    PrintChannel(lpdv, newline);
			    lpBits += lpbm->bmWidthBytes;
                        } else {
	                    /* don't do binary stuff, unpack only accepts ascii */
			    if (lpdv->fBinary) {
				    WriteChannel(lpdv, lpBits, byte_width);
				    lpBits += byte_width;

			    } else {
				    for (x = 0; x < byte_width; x++) {
					    PrintChannel(lpdv, hex, *lpBits++);
				    }
				    PrintChannel(lpdv, newline);
			    }

			    lpBits += lpbm->bmWidthBytes - byte_width;
                        }

			if (lpdv->fh < 0)
				goto FREE_EXIT;
		}

		if (lpbm->bmSegmentIndex) {
			// or bump HIWORD(lpBits) += lpbm->bmSegmentIndex ?

			DBMSG(("lpBits before add = %lx\n", lpBits));

			// adding in bmSegIndex hits the first scan of the next
			// seg.  use the x position offset to get the start point right

			lpBits = (LPSTR)MAKELONG(SrcX / 8, HIWORD(lpBits) + lpbm->bmSegmentIndex);

			DBMSG(("lpBits after add = %lx\n", lpBits));
		}

		scans -= scans_this_seg;
	}

        PrintChannel(lpdv, "%%%%EndData\n");

	PrintChannel(lpdv, restore);	/* match "save" in trans_scale */


FREE_EXIT:
	LocalUnlock(hBuf);
	LocalFree(hBuf);
EXIT:
	ClipBox(lpdv, NULL);


	return TRUE;

ERR_EXIT:
	return 0;
}
