/**[f******************************************************************
 * realize.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

#include "pscript.h"
#include "driver.h"
#include "utils.h"
#include "debug.h"
#include "enum.h"
#include "gdidm.h"
#include "realize.h"
#include "etm.h"
#include "truetype.h"

// #define COLOR_BRUSH	/* color patterned brushes */

#ifdef COLOR_BRUSH
void	PASCAL GrabColor(LPSTR lpb, DWORD FAR *bg, DWORD FAR *fg);
#endif


/********************************************************
* Name: RealizePen()
*
* Action: This routine is called from RealizeObject() to
*	  create a new physical pen.
*
* Returns: The amount of memory to allocate for the pen descriptor.
*
*********************************************************/

int FAR PASCAL RealizePen(lpdv, lpLogPen, lppen)
LPDV		lpdv;
LPLOGPEN	lpLogPen;	/* logical pen description */
LPPEN		lppen;		/* physical pen being realized */
{
	ASSERT(lpdv);

	if (lppen) {
		ASSERT(lpLogPen);

		lppen->lopn = *lpLogPen;
	}
	return sizeof(PEN);
}


#ifdef COLOR_BRUSH
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

void PASCAL GrabColor(LPSTR lpb, DWORD FAR *bg, DWORD FAR *fg)
{
	if ((lpb[13] == 'C') && (lpb[15] == 'G')) {	/* our magic # */
		DBMSG(("Grabbing color from paterned brush!\n"));
		*bg = RGB(lpb[1], lpb[3], lpb[5]);
		*fg = RGB(lpb[7], lpb[9], lpb[11]);
	} else {
		// note, this is backwards from the default, but apps
		// like excel and PM set text and bk color to these
		// values at brush creation time, assuming they will
		// get these in their output.

		*bg = 0x00FFFFFF;		/* use black and white */
		*fg = 0L;
	}
}
#endif



/********************************************************
* Name: RealizeBrush()
*
* Action: This routine is called from RealizeObject() to
*	  create a new physical brush.
*
* Returns: The amount of memory to allocate for the brush descriptor.
*
* Note: we convert HATCHED brushes to PATTERNED here.
*
**********************************************************/

int FAR PASCAL RealizeBrush(lpdv, lplb, lpbr)
LPDV		lpdv;
LPLOGBRUSH	lplb;	/* logical brush input */
LPBR		lpbr;	/* physical brush to realize */
{

	int	i;
	LPSTR	lpb;
	LPBITMAP lpbm;	/* ptr to the pattern bitmap */


	ASSERT(lpdv);

	if (lpbr) {

		ASSERT(lplb);

		lpbr->lb = *lplb;		/* copy the structure */

		if (lplb->lbStyle == BS_PATTERN) {

			/* Get the 8 byte brush pattern from the bitmap */
			/* this guy better by a mono bitmap */
			lpbm = (LPBITMAP)lplb->lbColor;

			ASSERT(lpbm);

			lpb = lpbm->bmBits;
			for (i = 0; i < 8; ++i) {
				lpbr->rgbPat[i] = *lpb;
				lpb += lpbm->bmWidthBytes;
			}

#ifdef COLOR_BRUSH

			/* bitblt has stored the fg color here */
 			GrabColor(lpbm->bmBits, &(lpbr->lb.lbBkColor), &(lpbr->lb.lbColor));

#ifdef DEBUG_ON
			lpb = lpbm->bmBits;

			DBMSG(("BS_PATTERN bits:"));
			for (i = 0; i < 16; i++) {
				DBMSG(("%02x", *lpb++));
			}
			DBMSG(("\n"));
#endif

			DBMSG(("RealizeBrush(): fg %lx bk %lx\n",
	 			lpbr->lb.lbColor,
				lpbr->lb.lbBkColor));
#else

			// these aren't really used.  we use the TextColor and bkColor 
			// from the drawmode at draw time

			// lpbr->lb.lbColor = 0L;
			// lpbr->lb.lbBkColor = 0x00FFFFFF;
#endif


		}
	}

	return sizeof(BR);	/* physical brush size */
}



/**********************************************************
* Name: RealizeObject()
*
* Action: This routine creates a data strucuture containing
*	  required to realize a specific instance of an object.
*	  The object may be a pen, brush, or font.
*
************************************************************/

int FAR PASCAL RealizeObject(lpdv, iStyle, lpbIn, lpbOut, lptf)
LPDV	lpdv;		/* ptr to the device descriptor */
int	iStyle;		/* object type (pen, brush, font) */
LPSTR	lpbIn;		/* ptr to structure containing object attributes */
LPSTR	lpbOut;		/* ptr to structure for storing realized object */
LPTEXTXFORM lptf;	/* ptr Contains additional object attributes */
{
	if (!lpdv->iType) {			/* memory device */
		if (iStyle == OBJ_FONT)
			return FALSE;
		else {

			DBMSG(("RealizeObject for memory bitmap\n"));

			return dmRealizeObject(lpdv, iStyle, lpbIn, lpbOut, lptf);
		}
	}

	if (lpdv->iType != 0x5750)
		return FALSE;		/* fail: this is not our PDEVICE */

	ASSERT(lpdv);

	switch (iStyle) {
	case OBJ_PEN:
		return RealizePen(lpdv, (LPLOGPEN)lpbIn, (LPPEN)lpbOut);
		break;

	case OBJ_BRUSH:
		return RealizeBrush(lpdv, (LPLOGBRUSH)lpbIn, (LPBR)lpbOut);
		break;

	case OBJ_FONT:
		return RealizeFont(lpdv, (LPLOGFONT)lpbIn, (LPFONTINFO)lpbOut, lptf);
		break;

        case -OBJ_FONT:
                if (((LPFONTINFO) lpbIn)->dfType & TYPE_TRUETYPE)
                    EngineDeleteFont((LPFONTINFO) lpbIn);
                break;
	}
}


