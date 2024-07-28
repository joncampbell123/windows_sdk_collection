/**[f******************************************************************
 * charwdth.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 *
 * getCHARWiDTH.C
 *
 * Created 15Dec87 to make application requests for charwidths as
 * efficient as possible.  The contents of this file should be
 * put in _TEXT to make it as fast as possible. (wrong!)
 *
 * this segment might want to be NON discardable to make calls here
 * fast. (probally not)
 *
 **********************************************************************/

#include "pscript.h"
#include "etm.h"
#include "fonts.h"
#include "utils.h"
#include "driver.h"
#include "debug.h"
#include "truetype.h"

/*******************************************************************
* Name: LockFont()
*
* Action: This routine takes a pointer to a device font and
*	  returns a pointer to the extended font info structure.
*	  It also converts several offsets in the device font
*	  to pointers.  Previously in FONTDIR.C.
*
*********************************************************************/

LPFX FAR PASCAL LockFont(lpdf)
LPDF lpdf;
{
	LPFX lpfx;

        if (lpdf->dfType & TYPE_TRUETYPE)
            lpfx = NULL;
	else if (lpdf->dfDriverInfo) {
		lpfx = (LPFX) (((LPSTR) lpdf) + lpdf->dfDriverInfo);
		lpfx->lpdf = lpdf;
		lpfx->lszFont = ((LPSTR)lpdf) + lpfx->dfFont;
		lpfx->lszDevType = ((LPSTR)lpdf) + lpdf->dfDevice;
		lpfx->rgWidths = (LPSHORT) (((LPSTR)lpdf) + lpdf->dfExtentTable);
		if (lpdf->dfTrackKernTable)
			lpfx->lpkt = (LPKT)(((LPSTR)lpdf) + lpdf->dfTrackKernTable);
		else
			lpfx->lpkt = (LPKT)(long)NULL;
		if (lpdf->dfPairKernTable)
			lpfx->lpkp = (LPKP)(((LPSTR)lpdf) + lpdf->dfPairKernTable);
		else
			lpfx->lpkp = NULL;
	} else
		lpfx = NULL;

	return lpfx;
}


/*********************************************************************
* Name: GetCharWidth()
*
* Action: Get the widths of a range of characters.  This is an OEM
* 	  layer call.  Previously in TEXT.C.
*
* Returns: The widths are returned in an array of shorts.
*
***********************************************************************/

short FAR PASCAL GetCharWidth(lpdv, lpBuf, firstChar, lastChar, lpdf, lpdm, lpft)
LPDV		lpdv;		/* Far ptr the device descriptor */
LPSHORT		lpBuf;		/* Far ptr to the output width buffer */
BYTE		firstChar;
BYTE		lastChar;
LPDF		lpdf;		/* Far ptr to the font structure */
LPDRAWMODE	lpdm;		/* Far ptr to the justification info, etc. */
LPTEXTXFORM	lpft;		/* Font transformation structure */
{
	LPSHORT lpCharWidths;
	short	defCharWidth;
	short	lowerDefaultRange, upperDefaultRange;
	short	i;
	LPFX	lpfx;
	short	rc = TRUE;
        LPTTFONTINFO lpttfi;

	/* if the range is invalid fail */
	if (lastChar < firstChar)
	   return FALSE;

    /* if this is a TrueType font get widths from TT font info */
    if (lpdf->dfType & TYPE_TRUETYPE) 
    {
        lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);

        if (!(lpdf->dfType & TYPE_HAVEWIDTHS)) {
            EngineGetCharWidth(lpdf, 0, 255, lpttfi->rgwWidths);
            lpdf->dfType |= TYPE_HAVEWIDTHS;    
        }

        lmemcpy(lpBuf, (LPSTR) (lpttfi->rgwWidths + firstChar),
                (lastChar - firstChar + 1) * sizeof(WORD));

        return TRUE;
    }

	if (!(lpdf->dfPitchAndFamily & 1))
    {   //  This is a fixed pitch font
        for( i = firstChar ; i <= lastChar ; i++)
            lpBuf[i - firstChar] = lpdf->dfAvgWidth;
        return(TRUE);
    }
    
	/* get the extended font structure */
	lpfx = LockFont(lpdf);

	lpCharWidths = (LPSHORT) lpfx->rgWidths;

	/* get a scaled copy of the default character width */
	defCharWidth = Scale(
	    *(lpCharWidths + lpdf->dfDefaultChar),
	    lpfx->sx, EM);
	DBMSG3(((LPSTR)">GetCharWidth(): first=%d,last=%d,sx=%d,EM=%d\r",
	    firstChar, lastChar, lpfx->sx, EM));
	DBMSG3(((LPSTR)" GetCharWidth(): dffirst=%d,dflast=%d,dfavg=%d,def=%d\r",
	    lpdf->dfFirstChar, lpdf->dfLastChar, lpdf->dfAvgWidth, defCharWidth));

	/* check to see if the last character is before the first...
	 * i.e. a parameter error.
	 */
	if (lastChar < firstChar) {
		DBMSG3(((LPSTR)" GetCharWidth(): LAST BEFORE FIRST\r"));
		rc = FALSE;

		/* check to see if the entire range of desired characters is outside
	 * the font's specified range
	 */
	} else if ((short)lpdf->dfFirstChar > (short)lastChar || 
	    (short)lpdf->dfLastChar < (short)firstChar) {

		/* all the requested characters are outside the font's
		 * specified range...return all default char widths
		 */
		DBMSG3(((LPSTR)" GetCharWidth(): ALL OUTSIDE\r"));
		for (i = (short)firstChar; i <= (short)lastChar; i++) {
			*lpBuf = defCharWidth;
			lpBuf++;
		}

	} else {
		DBMSG3(((LPSTR)" GetCharWidth(): NORMAL\r"));

		/* check to see if any of the characters are below the
		 * font's specified first character.
		 * 	lowerDefaultRange > 0...if this is true
		 * 	lowerDefaultRange = 0...if this is false
		 */
		if ((lowerDefaultRange = (short)lpdf->dfFirstChar - (short)firstChar) <= 0) {
			lowerDefaultRange = 0;
		}

		/* check to see if any of the characters are above the
		 * font's specified last character.
		 * 	upperDefaultRange > 0...if this is true
		 * 	upperDefaultRange = 0...if this is false
		 */
		if ((upperDefaultRange = (short)lastChar - (short)lpdf->dfLastChar) <= 0) {
			upperDefaultRange = 0;
		}
		DBMSG3(((LPSTR)" GetCharWidth(): lower=%d,upper=%d\r",
		    lowerDefaultRange, upperDefaultRange));

		/* set the widths of characters below the font's first char
		 * to defaults...if any
		 */
		for (i = firstChar; i < firstChar + lowerDefaultRange; i++) {
			DBMSG3(((LPSTR)" GetCharWidth(): BELOW %d\r", i));
			*lpBuf = defCharWidth;
			lpBuf++;
		}

		/* set the widths of characters in the valid range
		 * ...if any
		 */
		for (i = firstChar + lowerDefaultRange; i < lastChar + 1 - upperDefaultRange; i++) {
			if (lpdf->dfPitchAndFamily & 1) {
				/* proportional font */
				*lpBuf = *(lpCharWidths + (i - lpdf->dfFirstChar));

				/* now scale it... */
				DBMSG3(((LPSTR)" GetCharWidth(): PROPORTIONAL..i=%d,unsc=%d,",
				    i, *lpBuf));
				*lpBuf = Scale(*lpBuf, lpfx->sx, EM);
				DBMSG3(((LPSTR)"sc=%d\r", *lpBuf));
			} else {
				/* fixed pitch font...no need to scale this because
				 * it is already scaled
				 */
				*lpBuf = lpdf->dfAvgWidth;
				DBMSG3(((LPSTR)" GetCharWidth(): FIXED PITCH...i=%d,avg=%d\r",
				    i, *lpBuf));
			}
			/* get ready for the next one */
			*lpBuf++;
		}

		/* set the widths of characters above the font's last char
		 * to defaults...if any
		 */
		for (i = lastChar + 1 - upperDefaultRange; i < lastChar + 1; i++) {
			DBMSG3(((LPSTR)" GetCharWidth(): ABOVE %d\r", i));
			*lpBuf = defCharWidth;
			*lpBuf++;
		}
	}
	DBMSG3(("<GetCharWidth()\r"));
	return rc;
}


