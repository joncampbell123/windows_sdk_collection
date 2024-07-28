/**[f******************************************************************
 * bitblt.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 * mod history:
 *
 *	StretchBlt() moved to strchblt.c
 *
 **f]*****************************************************************/



#include "pscript.h"
#include "atstuff.h"
#include "debug.h"
#include "utils.h"
#include "channel.h"
#include "driver.h"
#include "gdidm.h"
#include "graph.h"

#ifdef COLOR_BRUSH

void PASCAL StuffColor(LPDV lpdv, LPSTR lpb, DWORD bg, DWORD fg);


/* pattern brush fix
 *
 * hack-o-death
 * 
 * since bitmaps are word alligned and in our 8x8 pattern bitmap we have
 * 8 (count em) bytes to use as we please.  we will use this to store
 * foreground and background colors so when we go to create a pattern
 * brush we know what colors to use.
 *
 */

void PASCAL StuffColor(LPDV lpdv, LPSTR lpb, DWORD bg, DWORD fg)
{
	lpb[1] = GetRComponent(lpdv, bg);
	lpb[3] = GetGComponent(lpdv, bg);
	lpb[5] = GetBComponent(lpdv, bg);
	lpb[7] = GetRComponent(lpdv, fg);
	lpb[9] = GetGComponent(lpdv, fg);
	lpb[11] = GetBComponent(lpdv, fg);
	lpb[13] = 'C';			/* magic # 1 */
	lpb[15] = 'G';			/* magic # 2 */
}

#endif


/*****************************************************************************
 * Name: BitBlt()
 *
 * Action: This routine transfers a rectangular region of a
 *	  source bitmap to the display surface.  The bits
 *	  in the source, destination, and brush pattern are
 *	  combines as specified by the raster operation code
 *	  in the drawing mode structure.
 *
 * Returns:
 *	TRUE  = Success
 *	FALSE = Failure
 *
 * note:
 *    postscript printers do not support copying from the device
 *    to memory bitmaps. they also do not support copying device
 *    to device bitmaps. this driver lies and says it supports this
 *    with the understanding that apps will not ask us to do these
 *    things.
 *
 * the cases that real bitblt must support are these:
 *
 *	operation:		supported:
 *
 *	memory -> memory	yes, mono bitmaps (uses brute routines)
 *	brush  -> memory	yes, mono bitmaps (use brute routines)
 *	brush  -> device	yes, also does BLACKNESS and WHITENESS
 *	device -> memory	no
 *	device -> device	no
 *	memory -> device	yes, but only SRCCOPY and a few others
 *
 ****************************************************************************/

BOOL FAR PASCAL BitBlt(lpdv, xDst, yDst, lpbmSrc, xSrc, ySrc, cx, cy, rop, lpbr, lpdm)
LPDV	lpdv;		/* ptr to the destination device descriptor */
int	xDst;		/* The destination's horizontal origin */
int	yDst;		/* The destination's vertical origin */
LPBITMAP lpbmSrc;	/* ptr to the source device descriptor */
int	xSrc;		/* The source's horizontal origin */
int	ySrc;		/* The source's vertical origin */
int	cx;		/* The horizontal extent (count of x units) */
int	cy;		/* The vertical extent (count of y units) */
long	rop;		/* The raster operation code */
LPBR	lpbr;		/* ptr to the brush (pattern) */
LPDRAWMODE	lpdm;	/* ptr to the drawing mode */
{
	BOOL	result;
	RECT	rect;

	ASSERT(lpdv);

	DBMSG(("BitBlt(): src planes:%d bits:%d dx:%d dy:%d  dst %d %d\n",
		lpbmSrc->bmPlanes,
		lpbmSrc->bmBitCount,
		lpbmSrc->bmWidth,
		lpbmSrc->bmHeight,
		((LPBITMAP)lpdv)->bmWidth,
		((LPBITMAP)lpdv)->bmHeight));

	/* sequentially check for cases that this driver supports */

	// is DEST memory?

	if (!lpdv->iType) {			/* dest == memory */
	
		// DEST == memory case

		if (!lpbmSrc || !lpbmSrc->bmType) {	/* brush || src == memory */

			result = dmBitblt(lpdv, xDst, yDst, lpbmSrc, xSrc, ySrc, cx, cy,
				rop, (long)lpbr, lpdm);
#ifdef COLOR_BRUSH
			/* hack to store color brush fogeground and
			 * background colors */

			lpbm = (LPBITMAP)lpdv;
			/* store the FG color with this bitmap */
			if (lpbm->bmWidth == 8 && lpbm->bmHeight == 8) {
				StuffColor(lpdv, lpbm->bmBits, lpdm->bkColor,	lpdm->TextColor);

				DBMSG(("BitBlt(): fg %lx bk %lx\n",
					lpdm->TextColor, lpdm->bkColor));
			}
#endif

			return result;

		} else {
			return FALSE;	/* src == device, not supported */
		}

	} else {

#ifdef PS_IGNORE
		if (lpdv->fSupressOutput)
			return 1;
#endif
		// DEST is device

		// use our friend, mister StretchBlt().  he can do all
		// the neat stuff that we need

		rect.top  = yDst;
		rect.left = xDst;
		rect.bottom = yDst + cy;
		rect.right  = xDst + cx;

		return StretchBlt(lpdv, xDst, yDst, cx, cy,
			lpbmSrc, xSrc, ySrc, cx, cy, rop, lpbr, lpdm, &rect);
	}
}

