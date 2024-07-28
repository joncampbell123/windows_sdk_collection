/**[f******************************************************************
 * text.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*****************************************************************************
 *
 * TEXT.C
 *
 * 8Jan87...Added checks for OEM_CHARSET in MapFont(), and RealizeFont().
 *	Also fixed error return in EnumDFont(), and polished up error
 * 	handling in MapFont() and RealizeFont().  sjp
 * 9Jan87...Reformatted ShowStr() and StrBlt() and add debug messages to try
 * 	to find the baseline problem.  sjp
 * 12Jan87..Changed text metrics stuff so that internal leading=150 added to
 *	a cell of 1000.  Also involved changing PFM compiler.  Added
 *	DBMSG macro.
 * 14Jan87..Fixed pair kern stuff.
 * 16Jan87..Fixed text metrics stuff so that font size requests are correct,
 *	that is, correct based on the Adobe font descriptions.  Also,
 *	fixed problem with text widths being scaled too large.
 * 19Jan87..Added SETALLJUSTVALUES escape support stuff.  See ShowStr()
 *	and StrWidth().
 * 22Jan87..Enhanced SETALLJUSTVALUES escape stuff.  Same changes made
 *	in Postscript driver as made in PCL driver by MSD.
 * 29Jan87..Minor change in GetJustBreak() argument list to fix non
 *	justification in Windows Write.  Same as PCL change by MSD.
 * 8Jun87...Added OEM layer routine GetCharWidth().
 * 6/28/87 - R. Best - BitStream Inc.
 *	Modified LoadSoftFonts and friends to support Postscript
 *      temporary outline font download. Softfonts are specified by
 *      the user in WIN.INI in the standard fashion and loaded 
 *      on a per-page basis are necessary. Added routine
 *      DownLineLoadFont to perform primary duties.
 *
 * 7/20/87 - R. Best - BitStream Inc.
 *      Changed to support 2 charset layouts ANSI Windows and
 *      PostScript. Previous version assumed all fonts to be 
 *      PostScript encoded and simulated or retrieved from the
 *      Symbol fonts selected special characters like copyright
 *      and trademark.
 *
 *      New strategy: if dfCharSet = ANSI_CHARSET then bypass
 *      hacks to use PostScript Encoding else do hacks.
 *
 *
 * 10/26/88 - msd..Added code to DumpStr that zips through the string
 *      converting any characters out of range into dfDefaultChar.
 *
 * Jan89 chrisg lots of the BitStream code declared unncessary static
 *		local vars.  these have been changed to frame vars.
 *		note: if we start running into stack overflow problems
 *		these vars should be Alloced instead of declared on the
 *		stack.
 *
 * 04/14/89 chrisg	underlining bug with char width array fixed.
 *			add the runbreak extra space to cxChar instead
 *			of using it to advance to the next character.
 *
 **************************************************************************/


#include "pscript.h"
#include <winexp.h>
#include "etm.h"
#include "fonts.h"
#include "utils.h"
#include "debug.h"
#include "text.h"
#include "channel.h"
#include "kern.h"
#include "adobe.h"
#include "gdidm.h"
#include "driver.h"
#include "truetype.h"
#include "graph.h"

/* ExtTextOut() options switches */
#define EXTTEXT_D1 0x0002
#define EXTTEXT_D2 0x0004

#define IDF_SYMBOL   13 /* The font index for the Symbol font */

/*------------------------------- local functions ------------------------*/

BOOL	PASCAL SetJustification(LPDV, LPDRAWMODE, LPJUSTBREAKTYPE, LPJUSTBREAKREC, LPJUSTBREAKREC);
short	PASCAL GetJustBreak(LPJUSTBREAKREC, JUSTBREAKTYPE);
long	PASCAL DumpStr(LPDV, LPDF, LPDRAWMODE, int, int, BOOL, LPSTR, long, int, int, int, int);
long	PASCAL ProjectString(long, short);
DWORD   PASCAL TTShowStr(LPPDEVICE,int,int,LPSTR,int,LPDF,LPDRAWMODE,LPFT,short FAR *);
void	PASCAL TTSelectFont(LPPDEVICE, LPDF, LPSTR, int);


/*--------------------------- external functions ------------------------*/

extern short	FAR PASCAL RCos(short, short);	/* in trig.asm */
extern short	FAR PASCAL RSin(short, short);



/********************************************************************
* Name: ProjectString()
*
* Action: Compute the projection of a string on the
*	  vertical and horizontal axis.
*
***********************************************************************/

long PASCAL ProjectString(lsize, theta)
long	lsize;
short	theta;
{
#if 0
	short	cx;
	short	cy;
	short	cxPrime;
	short	cyPrime;
	short	iCos;
	short	iSin;


   return lsize;

	if (theta) {
		/* If the escapement angle is non-zero, do the trig calculations */
		cx = LOWORD(lsize);	 /* Get the string width along escapement */
		cy = HIWORD(lsize);	 /* Get the font height */

		if ((iCos = RCos(1000, theta)) < 0)
			iCos = -iCos;
		if ((iSin = RSin(1000, theta)) < 0)
			iSin = -iSin;

		cxPrime = Scale(cx, iCos, 1000) + Scale(cy, iSin, 1000);
		cyPrime = Scale(cx, iSin, 1000) + Scale(cy, iCos, 1000);
		lsize = MAKELONG(cxPrime, cyPrime);
	}
#endif
	return(lsize);
}


/***********************************************************************
 *SetJustification
 *
 *  Set up the justification records and return TRUE if justification
 *  will be required on the line.  The justification values can come
 *  from two places:
 *
 *	1. Windows GDI SetTextCharacterExtra() and
 *		SetTextJustification(), which handle only
 *		positive justification.
 *	2. The SETALLJUSTVALUES escape, which handles negative
 *		and positive justification.
 *
 *  Windows' justification parameters are stored in the DRAWMODE struct,
 *  while SETALLJUSTVALUES stuff comes from our DEVICE struct.
 *
 *  Aldus Corporation 19 January 1987--sjp
 ************************************************************************/

BOOL PASCAL SetJustification(lpdv, lpdm, lpJustType, lpJustifyWB, lpJustifyLTR)
LPDV lpdv;
LPDRAWMODE lpdm;
LPJUSTBREAKTYPE lpJustType;
LPJUSTBREAKREC lpJustifyWB;
LPJUSTBREAKREC lpJustifyLTR;
{
	DBMSG2(("***SetJustification(): epJust=%d,fdm=%d, tbe=%d\r",
	    lpdv->epJust, fromdrawmode, lpdm->TBreakExtra));
	if ((*lpJustType = lpdv->epJust) == fromdrawmode) {
		/*	Normal Windows justification.
		 */
		if (lpdm->TBreakExtra) {
			lpJustifyWB->extra = lpdm->BreakExtra;
			lpJustifyWB->rem = lpdm->BreakRem;
			lpJustifyWB->err = lpdm->BreakErr;
			lpJustifyWB->count = lpdm->BreakCount;
			lpJustifyWB->ccount = 0;
		} else {
			lpJustifyWB->extra = 0;
			lpJustifyWB->rem = 0;
			lpJustifyWB->err = 1;
			lpJustifyWB->count = 0;
			lpJustifyWB->ccount = 0;
		}

		lpJustifyLTR->extra = lpdm->CharExtra;
		lpJustifyLTR->rem = 0;
		lpJustifyLTR->err = 1;
		lpJustifyLTR->count = 0;
		lpJustifyLTR->ccount = 0;
	} else {
		/*	SETALLJUSTVALUES -- the records were filled when the
		 *	escape was called, now make local copies.
		 */
		lmemcpy((LPSTR)lpJustifyWB, (LPSTR) & lpdv->epJustWB,
		    sizeof(JUSTBREAKREC));
		lmemcpy((LPSTR)lpJustifyLTR, (LPSTR) & lpdv->epJustLTR,
		    sizeof(JUSTBREAKREC));
	}
	DBMSG2(("SetJust():x=%d,r=%d,e=%d,c=%d\r",
	    lpJustifyWB->extra, lpJustifyWB->rem, lpJustifyWB->err,
	    lpJustifyWB->count));
	DBMSG2(("SetJust()***:x=%d,r=%d,e=%d,c=%d\r",
	    lpJustifyLTR->extra, lpJustifyLTR->rem, lpJustifyLTR->err,
	    lpJustifyLTR->count));

	/*	Advise the caller as to whether or not justification
	 *	adjustments will be required.
	 */
	if (lpJustifyWB->extra || lpJustifyWB->rem || 
	    lpJustifyLTR->extra || lpJustifyLTR->rem) {
		return TRUE;
	} else 
		return FALSE;
}


/***********************************************************************
 *GetJustBreak
 *
 *  Calculate the additional pixels to add/subtract from the horizontal
 *  position.
 *Aldus Corporation 19 January 1987--sjp
 ************************************************************************/

short PASCAL GetJustBreak (lpJustBreak, justType)
LPJUSTBREAKREC lpJustBreak;
JUSTBREAKTYPE justType;
{
	short	adjust = lpJustBreak->extra;

	DBMSG2(("***GetJustBreak:bv=%d,e=%d,c=%d,cc=%d\r",
	    adjust, lpJustBreak->err, lpJustBreak->count, lpJustBreak->ccount));
	if ((lpJustBreak->err -= lpJustBreak->rem) <= 0) {
		++adjust;
		lpJustBreak->err += (short)lpJustBreak->count;
	}

	if ((justType != fromdrawmode) && lpJustBreak->count && 
	    (++lpJustBreak->ccount > lpJustBreak->count)
	    ) {
		/*	Not at a valid character position, return zero adjustment.
		 */
		adjust = 0;
	}
	DBMSG2(("GetJustBreak***:bv=%d,e=%d,c=%d,cc=%d\r",
	    adjust, lpJustBreak->err, lpJustBreak->count, lpJustBreak->ccount));

	return adjust;
}


/********************************************************
* Name: TrackKern()
*
* Action: Compute the amount of track kerning to add to
*	  the string length.
*
**********************************************************/

int FAR PASCAL TrackKern(lpdv, lpfx, cb)
LPDV	lpdv;	/* Far ptr to the device descriptor */
LPFX	lpfx;	/* Far ptr to the extended device font */
int	cb; 	/* The un-kerned string width */

{
	int	iTrack;
	int	iKernAmount;
	TRACK FAR * lpTrack;
	LPKT lpkt;

	/* Fail if this is not our font */
	if (!(lpfx->lpdf->dfType & 0x080))
		return(0);

	iKernAmount = 0;
	lpkt = lpfx->lpkt;
	iTrack = lpdv->iTrack;
	if (lpkt && iTrack > 0) {
		if (iTrack <= lpkt->cTracks) {
			lpTrack = &lpkt->rgTracks[lpdv->iTrack - 1];

			iKernAmount = KernAmount(
			    lpTrack->iKernMin,
			    lpTrack->iKernMax,
			    lpTrack->iPtMax,
			    lpTrack->iPtMin,
			    Scale(lpfx->sy, 72, lpdv->iRes),
			    cb
			    );

			iKernAmount = Scale(iKernAmount, lpdv->iRes, 7200);
		}
	}
	return(iKernAmount);
}


/****************************************************************
* Name: DumpStr()
*
* Action: Execute the Postscript calls necessary to output a
* string of text.
*
* Returns: A long value which is the length of the string.
*
*****************************************************************/

long PASCAL DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, count, cBrk, cxBrk,
    ULlength)

LPDV	lpdv;	/* Far ptr the device descriptor */
LPDF	lpdf;	/* Far ptr to the FONTINFO (font metrics) structure */
LPDRAWMODE	lpdm;	/* Far ptr to the justification info, etc. */
int	ix;	/* The horizontal origin */
int	iy;	/* The vertical origin */
BOOL	fPrint;	/* TRUE if the string should be output */
LPSTR	lpbStr;	/* Far ptr to the output string */
long	cxStr;	/* Desired length of string less word adjustment */
int	count;	/* Number of characters in the output string */
int	cBrk; 	/* The number of break characters */
int	cxBrk;	/* The pixel width added to break characters */
int ULlength;  /* total length to underline, this should be set = cxStr
    unless lpdx is present, in which case ULlength = cxStr + runbreak;  */
{
	LPFX lpfx = LockFont(lpdf);
	BYTE FAR * lsz;
	short	i;

	if ((lpdf->dfPitchAndFamily & 1) && !lpdv->fIntWidths) {
		cxStr = ldiv(lmul(cxStr, (long) lpfx->sx), (long)EM);
	}

        if (!(lpdf->dfType & TYPE_TRUETYPE))
	    cxStr += TrackKern(lpdv, lpfx, count);

	if (fPrint && cxStr) {

		if (cxBrk) {
			/* SJ == SetJustify */
			PrintChannel(lpdv, (LPSTR) "%d %d SJ\n", cxBrk, cBrk);
		}

		/* msd - 10/26/88:  Zip through the string converting any
		 * characters outside of the valid range into dfDefaultChar.
		 * We already computed widths assuming the default char would
		 * be used.  Note that this destroys the string, but that's
		 * okay because this is the last time we use it.
		 */
		for (lsz = lpbStr, i = count; i > 0; --i, ++lsz) {
			if (*lsz < (BYTE)lpdf->dfFirstChar || *lsz > (BYTE)lpdf->dfLastChar)
				*lsz = lpdf->dfFirstChar + lpdf->dfDefaultChar;
		}

		DBMSG(("DumpStr: %d %d %d\n", ix, iy, lpdf->dfAscent));

		/* note: the dfAscent offset has been moved to the font creation
		 * time.  this allows rotation about the upper left corner
		 * (windows char origin). */

		/* SB == StrBlt */
		PrintChannel(lpdv, "%d %d %d %q %d SB\n",
		    ix, iy, (int)cxStr, lpbStr, count, ULlength);

                /* add string length to VM */
                lpdv->dwCurVM += count;
	}

	return (cxStr + (long)cxBrk);
}



/****************************************************************
* Name: ShowStr()
*
* Action: This routine can either draw text on an output device
*	  or take the measurement of a string of text depending
*	  on the sign of the bytecount.  If the bytecount is
*	  positive, then the string is actually drawn.	If the
*	  bytecount is negative, then ShowStr just returns the
*	  height and width of the string concatenated into a long
*	  integer.
*
*
* Returns: A long value which is the concatenation of the
*	   text height and string width.
*
*	   The high-order word contains the string height,
*	   and the low-order word contains the string width.
*
*****************************************************************/

DWORD FAR PASCAL ShowStr(lpdv, ix, iy, lpbSrc, cb, lpdf, lpdm, lpft, lpdx)

LPDV	lpdv;	/* Far ptr the device descriptor */
int	ix;	/* The horizontal origin */
int	iy;	/* The vertical origin */
LPSTR	lpbSrc;	/* Far ptr to the source string */
int	cb;	/* Size of the source string */
LPDF	lpdf;	/* Far ptr to the FONTINFO (font metrics) structure */
LPDRAWMODE	lpdm;	/* Far ptr to the justification info, etc. */
LPFT	lpft;	/* Font transformation structure */
LPSHORT lpdx;	/* Far ptr to delta-x moves */
{
	long	cxStr;
	long	cxSrc;
	int	i, pi;
	int	iCh;
	int	cBrk;
	int	cxBrk;
	int	cxChar;
	int	runbreak;
	int	dfBreakChar;
	int	dfFirstChar, dfDefaultChar;
	int	dfLastChar;
	short	far *lpcxChar;
	BOOL fPrint;
	BOOL fVariablePitch;
	LPFX lpfx;
	LPSTR lpbStr;
	JUSTBREAKREC justifyLTR;
	JUSTBREAKREC justifyWB;
	JUSTBREAKTYPE justType;

	if (cb == 0)
		return(0);

        if (lpdf->dfType & TYPE_TRUETYPE)
            return TTShowStr(lpdv, ix, iy, lpbSrc, cb, lpdf, lpdm, lpft, lpdx);

	if (!(fPrint = (cb > 0))) {
		cb = -cb;
	}

        lpfx = LockFont(lpdf);

	/*	Change font if we're going to output the string.
	 */
	if (fPrint) {
		SelectFont(lpdv, lpdf, lpbSrc, cb);
	}

	/* If doing opaque background text, make a recursive call to
	 * ShowStr() to get the dimensions of the whole output line,
	 * including any adjustments we make for ExtTextOut(), then
	 * draw and opaque box using the dimensions of the line.  After
	 * that, fall through and really output the line.
	 */
	if (fPrint && (lpdm->bkMode == OPAQUE)) {
		long	opaq = ShowStr(lpdv, ix, iy, lpbSrc, (-cb), lpdf, lpdm, lpft, lpdx);
		if (LOWORD(opaq) && HIWORD(opaq)) {

			OpaqueBox(lpdv, TRUE, ix, iy, LOWORD(opaq), HIWORD(opaq), lpdm->bkColor);

		}
	}

	dfBreakChar = (int)(lpdf->dfBreakChar + lpdf->dfFirstChar) & 0xff;
	dfFirstChar = (int)lpdf->dfFirstChar & 0xff;
	dfLastChar = (int)lpdf->dfLastChar & 0xff;
    dfDefaultChar = (int)lpdf->dfDefaultChar & 0xff;

	SetJustification(lpdv, lpdm, &justType, &justifyWB, &justifyLTR);

	/* Get width table.  */
	if (lpdf->dfPitchAndFamily & 1) {
		fVariablePitch = TRUE;
		lpcxChar = (short far * ) lpfx->rgWidths;
	} else {
		fVariablePitch = FALSE;
		lpcxChar = 0L;
	}

	/*	For each character in the string.
	 */
	for (cxSrc = cxStr = 0L, lpbStr = lpbSrc,
	    cBrk = cxBrk = runbreak = pi = i = 0; i < cb; ++i, ++lpbSrc) {

		/* If doing ExtTextOut() and the previous character we
		 * examined did not have a true width matching the desired
		 * width, output a partial string (a "run") and advance
		 * by the difference, then continue width-getting.
		 */
		if (runbreak) 
        {
            if(lpdx)
            {
			    DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr + runbreak);
                    //  call a new version of DumpStr which doesnot perform
                    //  any breakchar processing , ignore return value
                    //  from DumpStr.

			    cxSrc += cxStr + runbreak;

			    /* necessary for escapement support */

			    ix += Scale((int)cxStr + runbreak, RCos(1000, lpfx->escapement), 1000);
			    iy -= Scale((int)cxStr + runbreak, RSin(1000, lpfx->escapement), 1000);
            }
            else
            {
			    /* bug fix for WinWord double underline stuff.
			    * the old runbreak space was not getting underlined.
			    */

			    cxStr = DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr);

			    cxSrc += cxStr;
			    /* necessary for escapement support */
			
			    ix += Scale((int)cxStr, RCos(1000, lpfx->escapement), 1000);
			    iy -= Scale((int)cxStr, RSin(1000, lpfx->escapement), 1000);
            }
			cxStr = 0L;
			runbreak = 0;
			pi = i;
			lpbStr = lpbSrc;
			cBrk = 0;
			cxBrk = 0;
		}

		/*	Pick up character and skip it if out of range.
		 */
		iCh = ((int) *lpbSrc) & 0x0ff;
		if (iCh < dfFirstChar || iCh > dfLastChar)
            iCh = dfFirstChar + dfDefaultChar;
			//continue;

		/*	Get character width
		 */
		if (fVariablePitch) {
			cxChar = *(lpcxChar + (iCh - dfFirstChar));
			if (lpdv->fIntWidths) {
				cxChar = Scale(cxChar, lpfx->sx, EM);
			}
		} else
			cxChar = lpdf->dfAvgWidth;

		/* If doing ExtTextOut(), pick up the difference between the
		 * desired character width and the true character width.
		 */
		if (lpdx) {
			if (fVariablePitch && !lpdv->fIntWidths) {
				runbreak = (int)*lpdx - Scale(cxChar, lpfx->sx, EM);
			} else {
				runbreak = (int)*lpdx - cxChar;
			}
			++lpdx;

			DBMSG(("runbreak: %d\n", runbreak));

			/*	Apply word justification.
		 	*/
/*			if (iCh == dfBreakChar) {
				cBrk++;
				cxBrk += runbreak;

				DBMSG(("Break char\n"));
			} else
				cxChar += runbreak;
*/

		} else {

			/*	Apply letter justification.
		 	*/
			if (justType == justifyletters) {

				DBMSG(("justifyletters\n"));

				cxChar += GetJustBreak(&justifyLTR, justType);

			} else {

				if ((justType == fromdrawmode) || !(justifyLTR.count) || 
			    	(++justifyLTR.ccount <= justifyLTR.count)) {

					cxChar += justifyLTR.extra;

				}
			}


			/*	Apply word justification.
		 	*/
			if (iCh == dfBreakChar) {
				cBrk++;
				cxBrk += GetJustBreak(&justifyWB, justType);

				DBMSG(("Break char\n"));
			}
		}

		cxStr += cxChar;
	}

	/*	Output string.
	 */
    if(lpdx)
    	DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr + runbreak);
    else
    	cxStr = DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr);

	/*  Add in the final runbreak value, just in case the last delta-x
	 *  move was not equal to the width of the last character.  This
	 *  is important to the extent of the line, but unimportant to
	 *  output of the line. (Totally Bogus, it makes no sense)
	cxSrc += cxStr + runbreak;
	 */

	cxSrc += cxStr ;

	if (!fPrint) {
		lpdm->BreakErr = justifyWB.err;

		if (justType != fromdrawmode) {
			lpdv->epJustWB.err = justifyWB.err;
			lpdv->epJustWB.ccount = justifyWB.ccount;
			lpdv->epJustLTR.err = justifyLTR.err;
			lpdv->epJustLTR.ccount = justifyLTR.ccount;
		}
	}

	return (MAKELONG(cxSrc, lpdf->dfPixHeight));
}


DWORD PASCAL TTShowStr(lpdv, ix, iy, lpbSrc, cb, lpdf, lpdm, lpft, lpdx)

LPDV	lpdv;	/* Far ptr the device descriptor */
int	ix;	/* The horizontal origin */
int	iy;	/* The vertical origin */
LPSTR	lpbSrc;	/* Far ptr to the source string */
int	cb;	/* Size of the source string */
LPDF	lpdf;	/* Far ptr to the FONTINFO (font metrics) structure */
LPDRAWMODE	lpdm;	/* Far ptr to the justification info, etc. */
LPFT	lpft;	/* Font transformation structure */
LPSHORT lpdx;	/* Far ptr to delta-x moves */
{
	long	cxStr;
	long	cxSrc;
	int	i, pi;
	int	iCh;
	int	cBrk;
	int	cxBrk;
	int	cxChar;
	int	runbreak;
	int	dfBreakChar;
	int	dfFirstChar;
	int	dfLastChar, dfDefaultChar;
	short	far *lpcxChar;
	BOOL fPrint;
	BOOL fVariablePitch;
	LPSTR lpbStr;
	JUSTBREAKREC justifyLTR;
	JUSTBREAKREC justifyWB;
	JUSTBREAKTYPE justType;
        LPTTFONTINFO lpttfi;

	if (!(fPrint = (cb > 0))) {
		cb = -cb;
	}

	/*	Change font if we're going to output the string.
	 */
	if (fPrint) {
		SelectFont(lpdv, lpdf, lpbSrc, cb);
	}

        lpttfi = (LPTTFONTINFO) ((LPSTR)lpdf + lpdf->dfBitsOffset);

	/* If doing opaque background text, make a recursive call to
	 * ShowStr() to get the dimensions of the whole output line,
	 * including any adjustments we make for ExtTextOut(), then
	 * draw and opaque box using the dimensions of the line.  After
	 * that, fall through and really output the line.
	 */
	if (fPrint && (lpdm->bkMode == OPAQUE)) {
		long	opaq = TTShowStr(lpdv, ix, iy, lpbSrc, (-cb), lpdf, lpdm, lpft, lpdx);
		if (LOWORD(opaq) && HIWORD(opaq)) {

			OpaqueBox(lpdv, TRUE, ix, iy, LOWORD(opaq), HIWORD(opaq), lpdm->bkColor);

		}
	}

	dfBreakChar = (int)(lpdf->dfBreakChar + lpdf->dfFirstChar) & 0xff;
	dfFirstChar = (int)lpdf->dfFirstChar & 0xff;
	dfLastChar = (int)lpdf->dfLastChar & 0xff;
    dfDefaultChar = (int)lpdf->dfDefaultChar & 0xff;

	SetJustification(lpdv, lpdm, &justType, &justifyWB, &justifyLTR);

	/* Get width table.  */
	if (lpdf->dfPitchAndFamily & 1) {
		fVariablePitch = TRUE;

                if (!(lpdf->dfType & TYPE_HAVEWIDTHS)) {
                    EngineGetCharWidth(lpdf, 0, 255, lpttfi->rgwWidths);
                    lpdf->dfType |= TYPE_HAVEWIDTHS;    
                }

		lpcxChar = lpttfi->rgwWidths;
	} else {
		fVariablePitch = FALSE;
		lpcxChar = 0L;
	}


	/*	For each character in the string.
	 */
	for (cxSrc = cxStr = 0L, lpbStr = lpbSrc,
	    cBrk = cxBrk = runbreak = pi = i = 0; i < cb; ++i, ++lpbSrc) {

		/* If doing ExtTextOut() and the previous character we
		 * examined did not have a true width matching the desired
		 * width, output a partial string (a "run") and advance
		 * by the difference, then continue width-getting.
		 */
		if (runbreak) 
        {
            if(lpdx)
            {
			    DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr + runbreak);
                  // replace with new version
			    cxSrc += cxStr + runbreak;
			    /* necessary for escapement support */
			
			    ix += Scale((int)cxStr + runbreak, RCos(1000, lpttfi->lfCopy.lfEscapement), 1000);
			    iy -= Scale((int)cxStr + runbreak, RSin(1000, lpttfi->lfCopy.lfEscapement), 1000);
            }
            else
            {
			    /* bug fix for WinWord double underline stuff.
			    * the old runbreak space was not getting underlined.
			    */

			    cxStr = DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr);

			    cxSrc += cxStr;
			    /* necessary for escapement support */
			
			    ix += Scale((int)cxStr, RCos(1000, lpttfi->lfCopy.lfEscapement), 1000);
			    iy -= Scale((int)cxStr, RSin(1000, lpttfi->lfCopy.lfEscapement), 1000);
            }

			cxStr = 0L;
			runbreak = 0;
			pi = i;
			lpbStr = lpbSrc;
			cBrk = 0;
			cxBrk = 0;
		}

		/*	Pick up character and skip it if out of range.
		    no that would be wrong!  convert it to a default char  */
		iCh = ((int) *lpbSrc) & 0x0ff;
		if (iCh < dfFirstChar || iCh > dfLastChar)
            iCh = dfFirstChar + dfDefaultChar;
			//  continue;

		/*	Get character width
		 */
		if (fVariablePitch) {
			cxChar = *(lpcxChar + iCh);
                        #if 0
			if (lpdv->fIntWidths) {
				cxChar = Scale(cxChar, lpttfi->sx, EM);
			}
                        #endif
		} else
			cxChar = lpdf->dfAvgWidth;

		/* If doing ExtTextOut(), pick up the difference between the
		 * desired character width and the true character width.
		 */
		if (lpdx) {
			if (fVariablePitch && !lpdv->fIntWidths) {
                        #if 0
				runbreak = (int)*lpdx - Scale(cxChar, lpttfi->sx, EM);
                        #else
                                runbreak = (int)*lpdx - cxChar;
                        #endif
			} else {
				runbreak = (int)*lpdx - cxChar;
			}
			++lpdx;

			DBMSG(("runbreak: %d\n", runbreak));

			/*	Apply word justification.
			if (iCh == dfBreakChar) {
				cBrk++;
				cxBrk += runbreak;

				DBMSG(("Break char\n"));
			} else
				cxChar += runbreak;
		 	*/

		} else {

			/*	Apply letter justification.
		 	*/
			if (justType == justifyletters) {

				DBMSG(("justifyletters\n"));

				cxChar += GetJustBreak(&justifyLTR, justType);

			} else {

				if ((justType == fromdrawmode) || !(justifyLTR.count) || 
			    	(++justifyLTR.ccount <= justifyLTR.count)) {

					cxChar += justifyLTR.extra;

				}
			}


			/*	Apply word justification.
		 	*/
			if (iCh == dfBreakChar) {
				cBrk++;
				cxBrk += GetJustBreak(&justifyWB, justType);

				DBMSG(("Break char\n"));
			}
		}

		cxStr += cxChar;
	}

	/*	Output string.
	 */
    if(lpdx)
    	DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr + runbreak);
    else
    	cxStr = DumpStr(lpdv, lpdf, lpdm, ix, iy, fPrint, lpbStr, cxStr, i - pi, cBrk, cxBrk,
                    (int)cxStr);

	/*  Add in the final runbreak value, just in case the last delta-x
	 *  move was not equal to the width of the last character.  This
	 *  is important to the extent of the line, but unimportant to
	 *  output of the line. 
	 */
	cxSrc += cxStr;

	if (!fPrint) {
		lpdm->BreakErr = justifyWB.err;

		if (justType != fromdrawmode) {
			lpdv->epJustWB.err = justifyWB.err;
			lpdv->epJustWB.ccount = justifyWB.ccount;
			lpdv->epJustLTR.err = justifyLTR.err;
			lpdv->epJustLTR.ccount = justifyLTR.ccount;
		}
	}

	return (MAKELONG(cxSrc, lpdf->dfPixHeight));
}

void FAR PASCAL OpaqueBox(LPDV lpdv, BOOL fExtTextOut, int x, int y, int dx, int dy, DWORD color)
{
	PrintChannel(lpdv, "%d %d %d %d %d %d %d %d OB\n",
		dx, dy, x, y,
		GetRComponent(lpdv, color),
		GetGComponent(lpdv, color),
		GetBComponent(lpdv, color),
      fExtTextOut);
}


/***************************************************************
* Name: ExtTextOut()
*
* Action: This routine is used to draw text strings on the display
*	  surface.  Most of the real work is done by the ShowStr routine.
*	  The main goal of ExtTextOut is to break the source string up
*	  into the pieces that can be handled directly by the printer
*	  and to simulate the characters that the printer can't handle.
*
******************************************************************/

DWORD FAR PASCAL ExtTextOut(lpdv, ix, iy, lprcClip, lpbSrc, cbSrc, lpdf, lpdm, lpft,
			    lpdx, lprcOpaq, options)

LPDV	lpdv;		/* Far ptr the device descriptor */
int	ix;		/* The horizontal origion */
int	iy;		/* The vertical origion */
LPRECT	lprcClip;	/* Far ptr to the clipping rectangle */
LPSTR	lpbSrc;		/* Far ptr to the source string */
int	cbSrc;		/* Size of the source string */
LPDF	lpdf;		/* Far ptr to the FONTINFO structure */
LPDRAWMODE lpdm;	/* Far ptr to the justification info, etc. */
LPTEXTXFORM lpft;	/* Font transformation structure */
LPSHORT	lpdx;		/* Far ptr to array of delta-x moves */
LPRECT	lprcOpaq;	/* Far ptr to opaque rectangle */
WORD	options;	/* 2.0+ option switches */
{
	RECT	cliprect, opaqrect;
	LPSTR	lpbRun;
	int	cbRun;
	int	iCh, iChPrev;
	long	lsize;
	LPFX	lpfx;	/* Far ptr to the extended device font structure */
	int	cxStr;	/* The accumulated string width */
	int	cxKern; /* The number of pixels to kern a character */
	DWORD	FillColor;
        short   escapement;

	if (lpdv->iType != 0x5750)
		return (0);	/* fail: this is not our PDEVICE */

    // Cannot output to closed channel
    if (cbSrc > 0 && lpdv->fh < 0)
	return FALSE;

    if(lpdf->dfType & TYPE_SUBSTITUTED  &&  cbSrc > 0)
    {
        lpdf = (LPFONTINFO)((char far *)lpdf + 
            lpdf->dfBitsOffset + sizeof(TTFONTINFO));
            //  give ExtTextOut the Device FontInfo  if TT is substituted.
    }

#ifdef PS_IGNORE
	if (lpdv->fSupressOutput)
		return 1;
#endif

	/* Make local copies of the clip and opaque rectangles so we
	 * may freely modify them. */

	if (lprcClip) {
		cliprect = *lprcClip;
		lprcClip = &cliprect;
	}
	if (lprcOpaq) {
		opaqrect = *lprcOpaq;
		lprcOpaq = &opaqrect;
	}

	/* Modify opaque and clip regions per options switches. */

	if (lprcOpaq) {
		if (options & EXTTEXT_D2) {
			/*	lprcOpaq should be used as a clipping rectangle.
			 */
			if (lprcClip)
				IntersectRect(lprcClip, lprcClip, lprcOpaq);
			else {
				cliprect = *lprcOpaq;
				lprcClip = &cliprect;
			}
		}

		if (options & EXTTEXT_D1) {
			/*	lprcOpaq should be used as an opaque rectangle.
			 */
			if (lprcClip)
				IntersectRect(lprcOpaq, lprcClip, lprcOpaq);
		} else
			lprcOpaq = NULL;
	}

        lpfx = LockFont(lpdf);

#if 0
	/* def OLDCODE */

	/* Raise the Courier font a little */
	if (!(lpdf->dfPitchAndFamily & 1)) {
		iy -= Scale(lpfx->sy, 1, 12);
		DBMSG(("ShowStr(): Courier tweak x=%d,y=%d\r", ix, iy));
	}
#endif

	/* Make sure that a font is selected in case first char is simulated */
    if (cbSrc > 0 || (!cbSrc && (options & EXTTEXT_D1)))
	    SelectFont(lpdv, lpdf, lpbSrc, cbSrc);

	if (cbSrc >= 0 && lprcOpaq) {

		OpaqueBox(lpdv, FALSE, lprcOpaq->left, lprcOpaq->top,
		    (lprcOpaq->right - lprcOpaq->left) - 1,
		    (lprcOpaq->bottom - lprcOpaq->top) - 1, lpdm->bkColor);
	}

	if (cbSrc < 0) {	// || ((lpdf->dfPitchAndFamily & 0x0f0) == FF_DECORATIVE))
		lsize = ShowStr(lpdv, ix, iy, lpbSrc, cbSrc, lpdf, lpdm, lpft, lpdx);
		goto DONE;
	}

        if (!cbSrc)
                goto DONE;

	// set the color of the font here

	if (lpdm)
		FillColor = lpdm->TextColor;
	else
		FillColor = 0L;	// black

	if (lpdv->FillColor != FillColor) {
		PrintChannel(lpdv, "%d %d %d fC\n",
			GetRComponent(lpdv, FillColor),
			GetGComponent(lpdv, FillColor),
			GetBComponent(lpdv, FillColor));
		lpdv->FillColor = FillColor;
	}


	cxStr = 0;
	ClipBox(lpdv, lprcClip);

	lpbRun = lpbSrc;
	cbRun = 0;
	iCh = 0;
	cxKern = 0;
	while (--cbSrc >= 0) {
		iChPrev = iCh;
		iCh = *lpbSrc++;

		if (lpdv->fPairKern && (lpdf->dfPitchAndFamily & 1)) {
			cxKern = PairKern(lpdv->iRes, lpfx->lpkp, iChPrev, iCh);
		}

		if (cxKern != 0) {
			/* Output the previous run of characters */
			cxStr += (int)LOWORD(ShowStr(lpdv, ix + cxStr, iy, lpbRun, cbRun, lpdf, lpdm, lpft, lpdx));
			cxStr += cxKern;
			cxKern = 0;

			cbRun = 1;
			lpbRun = lpbSrc - 1;
			continue;
		}
		++cbRun;
	}
	cxStr += (int)LOWORD(ShowStr(lpdv, ix + cxStr, iy, lpbRun, cbRun, lpdf, lpdm, lpft, lpdx));
	ClipBox(lpdv, NULL);
	lsize = MAKELONG(cxStr, lpdf->dfPixHeight);

DONE:

        if (lpdf->dfType & TYPE_TRUETYPE) {
            LPTTFONTINFO lpttfi;

            lpttfi = (LPTTFONTINFO) ((LPSTR)lpdf + lpdf->dfBitsOffset);
            escapement = lpttfi->lfCopy.lfEscapement;
        } else
            escapement = lpfx->escapement;

	return ProjectString(lsize, escapement);
}



/***************************************************************
 * Name: StrBlt()
 *
 * Action: Call ExtTextOut() with added (for Windows 2.0) structs
 *      set to zero.
 *
 *****************************************************************/

LONG FAR PASCAL StrBlt(lpdv, ix, iy, lprcClip, lpbSrc, cbSrc, lpdf, lpdm, lpft)

LPDV	lpdv;		/* Far ptr the device descriptor */
int	ix;		/* The horizontal origion */
int	iy;		/* The vertical origion */
LPRECT	lprcClip;	/* Far ptr to the clipping rectangle */
LPSTR	lpbSrc;		/* Far ptr to the source string */
int	cbSrc;		/* Size of the source string */
LPDF	lpdf;		/* Far ptr to the FONTINFO structure */
LPDRAWMODE lpdm;	/* Far ptr to the justification info, etc. */
LPTEXTXFORM lpft;	/* Font transformation structure */
{
#ifdef PS_IGNORE
	if (lpdv->fSupressOutput)
		return 1;
#endif
	if (!lpdv->iType) {
		return (dmStrBlt(lpdv, ix, iy, lprcClip, lpbSrc, cbSrc, lpdf, lpdm, lpft));
	}

	return ExtTextOut(lpdv, ix, iy, lprcClip, lpbSrc, cbSrc, lpdf, lpdm, lpft, NULL, NULL, NULL);
}


/*****************************************************
* Name: DownLineLoadFont()      *** rb BitStream ***
*
* Action: Check load status and ouput ps font if not
*         already loaded on current page. Use a bitmap font
*         if it exists, if not use the path specified in the
*         WIN.INI fontpath entry.
*
*         This is assumed to be a per-page temp font load
*         until someone adds the much needed UNLOAD-FONT
*         command to PostScript devices.
*         The user must take care not to request too many
*         soft fonts per page or printer will puke !!
*         Too many is currently > 3 fonts given 
*         virtual memory of 175k - 30k for pre-amble.
********************************************************/

void PASCAL DownLineLoadFont(lpdv, lpfx)
LPDV lpdv;	/* Far ptr to the device descriptor */
LPFX lpfx;	/* Far ptr to extra font info for driver */
{
	char	rgb[256];	/** why are these static? **/
	int	cbRead;
	int	fh;
	int	i;
	char	buf[16];
	BOOL	found = FALSE;
	BOOL	fAdobe = FALSE;


	/* First check to see if font already loaded by scanning
	 * the device status slots for a match on font instance id. */

	i = 0;
	while ((i <= lpdv->nextSlot) && (lpdv->nextSlot > -1)) {
		if (lpdv->slotsArray[i] == lpfx->iFont) {
			found = TRUE;
			break;
		} else
			++i;
	}

	/* If not found in device font slot array, open the font file
	 * and send the entire file to the output stream */

	if (!found) {

		++lpdv->nextSlot;                      /* bump to next */
		lpdv->slotsArray[lpdv->nextSlot] = lpfx->iFont;
		if (lpdv->nextSlot > MAXSOFTFONTS)
			lpdv->nextSlot = MAXSOFTFONTS; /* clip to max */

		if ((fh = _lopen((LPSTR)lpfx->sfLoadPath, READ)) > 0) {
                        PrintChannel(lpdv, "\n%%%%BeginResource: font %s\n",
                                     lpfx->lszFont);

			/* reset the file pointer--just to be sure */
			_llseek(fh, 0L, 0);

                        /* add size of softfont */
                        lpdv->dwCurVM += _llseek(fh, 0L, 2);
                        _llseek(fh, 0L, 0);

			/* check to see if the is an Adobe downloadable font */
			_llseek(fh, 6L, 0);
			if (_lread(fh, buf, sizeof(buf)) != sizeof(buf))
				return;	/* ERROR condition */
			buf[15] = '\0';
         if (!lstrcmpi(buf, "%!FontType1-1.0"))
            fAdobe = TRUE;
         buf[14] = '\0';
			if (!lstrcmpi(buf, "%!PS-AdobeFont"))
				fAdobe = TRUE;

			/* reset the file pointer */
			_llseek(fh, 0L, 0);

			if (!fAdobe) {
				while (TRUE) {
					cbRead = _lread(fh, rgb, sizeof(rgb));
					if (cbRead > 0)
						WriteChannel(lpdv, rgb, cbRead);
					else
						break;
				}
			} else {
				DoAdobeFont(lpdv, fh, rgb, sizeof(rgb));
			}
			_lclose(fh);
			PrintChannel(lpdv, "%%%%EndResource\n\n");

                        /* Add font to list of supplied fonts */
                        AddString(lpdv->slSupplied, lpfx->lszFont, 0L);
		}
	}
}


/*****************************************************
* Name: SelectFont()
*
* Action: Select the specified device font.  This function
*	  is called from StrBlt just before it outputs text.
*	  When a font change is requested (the font instance
*	  id is not the current one), then SelectFont outputs
*	  a command to the printer to select a new font.
*
* Note: There is a bug in the OEM adaptaion manual that states
*	the orientation and escapement vector are in degrees.
*	This is incorrect, they are actually in tenths of a degree.
*
********************************************************/

void PASCAL SelectFont(lpdv, lpdf, lpSrc, cb)
LPDV lpdv;	/* Far ptr to the device descriptor */
LPDF lpdf;	/* Far ptr to extra font info for driver */
LPSTR lpSrc;
int cb;
{
	LPFX lpfx;

	ASSERT(lpdv);
	ASSERT(lpdf);

        if (lpdf->dfType & TYPE_TRUETYPE) {
            TTSelectFont(lpdv, lpdf, lpSrc, cb);
            return;
        }

        lpfx = LockFont(lpdf);

	/* Check to see if the font instance handle has changed */
	if (lpfx->lid != lpdv->lidFont || lpfx->lid == -1L) {

		lpdv->lidFont = lpfx->lid;

		/* If softfont path specified, load font if appropriate */

		if (lpfx->sfLoadPath[0] != '\0') {    /*** rb BitStream ***/
			DownLineLoadFont(lpdv, lpfx);
		}

		/* Output the font change command to the printer */
		PrintChannel(lpdv, "%d %d %d %d %d %d %d %d %d ",
		    (lpdf->dfBreakChar + lpdf->dfFirstChar) & 0X0ff,
		    lpfx->escapement,
		    lpfx->orientation,

#if 0
		    /* This is an area to be careful around.
		     * If a user wants the Adobe font sizes, then leave this as is.
		     * However, if "true" point sizes are desired then the internal
		     * leading subtracted here should go away.  In the current
		     * implementation internal leading is 0.
		     */
		    lpfx->sx - lpdf->dfInternalLeading,
		    lpfx->sy - lpdf->dfInternalLeading,	/* not sure about this */
#else
		    // shouldn't the above happen in the ScaleFont()
		    // function?

		    lpfx->sx,
		    lpfx->sy,
#endif
		    lpdf->dfUnderline,
		    lpdf->dfStrikeOut,
          0,      /* not a Type-3 bitmap font */
		    lpdf->dfAscent);

		PrintChannel(lpdv, "/%s ", (LPSTR) lpfx->lszFont);

                /* Add font to list of needed fonts if it hasn't been 
                 * supplied in the print stream.
                 */
                if (!IsStringInList(lpdv->slSupplied, lpfx->lszFont))
                    AddString(lpdv->slNeeded, lpfx->lszFont, 0L);

                /* Add font to list of page resources.  There is no
                 * distinction between needed and supplied for this
                 * comment.
                 */
                AddString(lpdv->slPageResources, lpfx->lszFont, 0L);

		/* Don't remap the Symbol font or any non-ANSI font 
		 * using ANSI character set, assume encoded as is. 
		 */

/* this somewhat cryptic three part boolean condition checks to see if
 *	1. this is not a decorative font (so can't be a symbol font)
 *	2. this is declared as an ANSI font (as all resident fonts are)
 *	3. This is not a downloadable soft font if all of these are true,
 *
 * then we want to do the mapping of Windows ANSI character set onto the
 * Adobe Standard and Symbol character sets.  There is an additional
 * condition in which we want to do this mapping, which is a
 * downloadable soft font that happens to be an Adobe Standard character
 * set. This driver will recognize a font as such if the dfCharset field
 * of the PFM file is set to 4.
 * 	djm 12/11/87 Bitstream
 * 
 * A slight change in strategy here. Rather than a charset number that
 * is the Postscript stndard encoding, we will assign a number that
 * declares NO TRANSLATION, regardless of character set. When the PFM is
 * loaded, then we will check for this number, and if it is found, then
 * we will set dfCharSet to 0 (ANSI_CHARSET), and set lpfx->noTranslate
 * to TRUE. With the resident fonts, the DECORATIVE test is still
 * applied.
 * 	djm 12/20/87
 */

#if 0
		/* ORIGINAL
		/* Don't remap the Symbol font for the ANSI character set */
		if ((lpdf->dfPitchAndFamily & 0x0f0) != FF_DECORATIVE)
		* / 
		/* LATER
		if (((lpdf->dfPitchAndFamily & 0x0f0) != FF_DECORATIVE)
			&& (lpdf->dfCharSet != ANSI_CHARSET)
		)
		*/
#endif

		if (!lpfx->noTranslate) {
			PrintChannel(lpdv, "/font%d ANSIFont ", lpfx->iFont);
		}
		PrintChannel(lpdv, "font\n");
	}
}

void PASCAL TTSelectFont(lpdv, lpdf, lpSrc, cb)
LPDV lpdv;	/* Far ptr to the device descriptor */
LPDF lpdf;	/* Far ptr to extra font info for driver */
LPSTR lpSrc;
int cb;
{
    LPTTFONTINFO lpttfi;

    ASSERT(lpdv);
    ASSERT(lpdf);

    lpttfi = (LPTTFONTINFO) ((LPSTR)lpdf + lpdf->dfBitsOffset);

    /* Check to see if the font instance handle has changed */
    if (lpttfi->lid != lpdv->lidFont || lpttfi->lid == -1L) {

	    lpdv->lidFont = lpttfi->lid;

            TTDownLoadFont(lpdv, lpdf, lpdv->bDSC);

	    /* Output the font change command to the printer */
	    PrintChannel(lpdv, "%d %d %d %d %d %d %d %d %d ",
		(lpdf->dfBreakChar + lpdf->dfFirstChar) & 0X0ff,
      lpttfi->lfCopy.lfEscapement,
		lpttfi->lfCopy.lfOrientation,
		lpttfi->sx,
		lpttfi->sy,
		lpttfi->lfCopy.lfUnderline,
		lpttfi->lfCopy.lfStrikeOut,
      (lpdf->dfType & TYPE_OUTLINE) ? 0 : 1,
		lpdf->dfAscent);

	    PrintChannel(lpdv, "/%s font\n", (LPSTR) lpttfi->TTFaceName);

            /* Add font to list of needed fonts if it hasn't been 
                * supplied in the print stream.
                */
            if (!IsStringInList(lpdv->slSupplied, lpttfi->TTFaceName))
                AddString(lpdv->slNeeded, lpttfi->TTFaceName, 0L);

            /* Add font to list of page resources.  There is no
                * distinction between needed and supplied for this
                * comment.
                */
            AddString(lpdv->slPageResources, lpttfi->TTFaceName, 0L);
    }

    TTUpdateFont(lpdv, lpdf, lpSrc, cb);
}

/*------------------------------------------------------------------------
 * This routine is called by the DOWNLOADFACE escape and is designed to   
 * accomplish two different jobs based upon the value of mode.            
 * - Enables an application to query whether the current font is          
 *   automatically downloadable.                                          
 * - Initiates the in-line loading of the Postscript outline font into the
 *   current printing job.                                                
 * Returns: a positive value if successful or  -1 if the requested outline
 * font is not available.                                                 
 * Digby Horner - 9/15/88 Adobe Systems Inc.                              
 *
 * #ifndef ADOBE_11_1_88
 *
 *--------------------------------------------------------------------------*/

short FAR PASCAL PutFontInline(lpdv, lpetd, mode)
LPDV		lpdv; 	/* Far ptr to the device descriptor */
LPEXTTEXTDATA	lpetd;	/* Far ptr to the extended text data */
short		mode;   /* modes: 0=inform 1=perform */
{
	LPFX lpfx;
        LPDF lpdf;

	ASSERT(lpdv);
	ASSERT(lpetd);

        lpdf = lpetd->lpFont;

        if(lpdf->dfType & TYPE_SUBSTITUTED)
            lpdf = (LPFONTINFO)((char far *)lpdf + 
                lpdf->dfBitsOffset + sizeof(TTFONTINFO));
                //  give PutFontInline the Device FontInfo  if TT is substituted.

        if (lpdf->dfType & TYPE_TRUETYPE) {
            if (mode)
                TTDownLoadFont(lpdv, lpdf, TRUE);
            return 1;
        }

	if ((lpfx = LockFont(lpdf)) != NULL) {
		if (lpfx->sfLoadPath[0] != '\0') {
			if (mode) 
				DownLineLoadFont(lpdv, lpfx);
			return(1);
		}
	}
	return(-1);
}
