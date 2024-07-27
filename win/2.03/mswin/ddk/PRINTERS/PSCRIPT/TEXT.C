/*****************************************************************************
 *
 * TEXT.C
 *
 * 8Jan87...Added checks for OEM_CHARSET in MapFont(), and RealizeFont().
 *			Also fixed error return in EnumDFont(), and polished up error
 *			handling in MapFont() and RealizeFont().  sjp
 * 9Jan87...Reformatted ShowStr() and StrBlt() and add debug messages to try
 *			to find the baseline problem.  sjp
 * 12Jan87..Changed text metrics stuff so that internal leading=150 added to
 *			a cell of 1000.  Also involved changing PFM compiler.  Added
 *			DBMSG macro.
 * 14Jan87..Fixed pair kern stuff.
 * 16Jan87..Fixed text metrics stuff so that font size requests are correct,
 *			that is, correct based on the Adobe font descriptions.  Also,
 *			fixed problem with text widths being scaled too large.
 * 19Jan87..Added SETALLJUSTVALUES escape support stuff.  See ShowStr()
 *			and StrWidth().
 * 22Jan87..Enhanced SETALLJUSTVALUES escape stuff.  Same changes made
 *			in Postscript driver as made in PCL driver by MSD.
 * 29Jan87..Minor change in GetJustBreak() argument list to fix non
 *			justification in Windows Write.  Same as PCL change by MSD.
 * 8Jun87...Added OEM layer routine GetCharWidth().
 *
 *****************************************************************************
 */
#include "globals.h"
#include "pscript.h"



int FAR PASCAL PairKern(int, LPKP, int, int);
int FAR PASCAL TrackKern(LPDV, LPFX, int);
int FAR PASCAL KernAmount(int, int, int, int, int, int);
void PASCAL ScaleFont(DV FAR *, LPFONTINFO, int, int);
void PASCAL SetLeading(LPFONTINFO, int, int, int);
void PASCAL SelectFont(DV FAR *, LPFX);
void FAR PASCAL ClipBox(DV FAR *, RECT FAR *);
void FAR PASCAL lstrcpy(LPSTR, LPSTR);
BOOL FAR PASCAL lstrcmp(LPSTR, LPSTR);
void FAR PASCAL lmemcpy(LPSTR, LPSTR, int);
int FAR PASCAL Lightness(RGB);
int FAR PASCAL LoadFont(LPDV, LPSTR, LPDF);
int FAR PASCAL IntersectRect(LPRECT, LPRECT, LPRECT);


long FAR PASCAL lmul(long, long);
long FAR PASCAL ldiv(long, long);

/* Only use this if the simulation procs are actually used.
 * at present GetSimulate() (in simulate.c) returns a NULL.
 * Remember if you change SIMULATE you have to fix the make
 * file and the .LNK file as well.
 */
#define SIMULATE 0
#if SIMULATE
extern FARPROC FAR PASCAL GetSimulate();
#endif

/* ExtTextOut() options switches */
#define EXTTEXT_D1 0x0002
#define EXTTEXT_D2 0x0004

#define IDF_SYMBOL   13 /* The font index for the Symbol font */


/****************************************************************************/
/*#define DEBUG_ON*/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

/*#define DEBUG_ON1*/
#ifdef DEBUG_ON1
#define DBMSG1(msg) printf msg
#else
#define DBMSG1(msg)
#endif

/* for SETALLJUSTVALUES testing */
/*#define DEBUG_ON2*/
#ifdef DEBUG_ON2
#define DBMSG2(msg) printf msg
#else
#define DBMSG2(msg)
#endif

/* for GetCharWidths testing */
/*#define DEBUG_ON3*/
#ifdef DEBUG_ON3
#define DBMSG3(msg) printf msg
#else
#define DBMSG3(msg)
#endif

/* for RealizeFont testing */
/*#define DEBUG_ON4*/
#ifdef DEBUG_ON4
#define DBMSG4(msg) printf msg
#else
#define DBMSG4(msg)
#endif
/*********************************************************************/

/********************************************************************
* Name: ProjectString()
*
* Action: Compute the projection of a string on the
*	  vertical and horizontal axis.
*
**********************************************************************
*/
long ProjectString(lsize, theta)
long lsize;
short theta;
    {
    short cx;
    short cy;
    short cxPrime;
    short cyPrime;
    short iCos;
    short iSin;

    extern short FAR PASCAL RCos(short, short);
    extern short FAR PASCAL RSin(short, short);


    if (theta)
	{
	/* If the escapement angle is non-zero, do the trig calculations */
	cx = LOWORD(lsize);	 /* Get the string width along escapement */
	cy = HIWORD(lsize);	 /* Get the font height */


	if ((iCos = RCos(1000, theta))<0)
	    iCos = - iCos;
	if ((iSin = RSin(1000, theta))<0)
	    iSin = - iSin;

	cxPrime = Scale(cx, iCos, 1000) + Scale(cy, iSin, 1000);
	cyPrime = Scale(cx, iSin, 1000) + Scale(cy, iCos, 1000);
	lsize = MAKELONG(cxPrime, cyPrime);
	}
    return(lsize);
    }


/***********************************************************************
 *SetJustification
 *
 *	Set up the justification records and return TRUE if justification
 *	will be required on the line.  The justification values can come
 *	from two places:
 *
 *		1. Windows GDI SetTextCharacterExtra() and
 *			SetTextJustification(), which handle only
 *			positive justification.
 *		2. The SETALLJUSTVALUES escape, which handles negative
 *			and positive justification.
 *
 *	Windows' justification parameters are stored in the DRAWMODE struct,
 *	while SETALLJUSTVALUES stuff comes from our DEVICE struct.
 *
 *Aldus Corporation 19 January 1987--sjp
 ***********************************************************************
 */
static BOOL SetJustification(LPDV, LPDM, LPJUSTBREAKTYPE,
		LPJUSTBREAKREC, LPJUSTBREAKREC);
static BOOL SetJustification(lpdv, lpdm, lpJustType,
		lpJustifyWB, lpJustifyLTR)
	LPDV lpdv;
	LPDM lpdm;
	LPJUSTBREAKTYPE lpJustType;
	LPJUSTBREAKREC lpJustifyWB;
	LPJUSTBREAKREC lpJustifyLTR;
{
	DBMSG2(((LPSTR)"***SetJustification(): epJust=%d,fdm=%d, tbe=%d\r",
		lpdv->dh.epJust,fromdrawmode,lpdm->TBreakExtra));
	if ((*lpJustType = lpdv->dh.epJust) == fromdrawmode){
		/*	Normal Windows justification.
		 */
		if (lpdm->TBreakExtra){
			lpJustifyWB->extra = lpdm->BreakExtra;
			lpJustifyWB->rem = lpdm->BreakRem;
			lpJustifyWB->err = lpdm->BreakErr;
			lpJustifyWB->count = lpdm->BreakCount;
			lpJustifyWB->ccount = 0;
		}else{
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
	}else{
		/*	SETALLJUSTVALUES -- the records were filled when the
		 *	escape was called, now make local copies.
		 */
		lmemcpy((LPSTR)lpJustifyWB, (LPSTR) &lpdv->dh.epJustWB,
			sizeof(JUSTBREAKREC));
		lmemcpy((LPSTR)lpJustifyLTR, (LPSTR) &lpdv->dh.epJustLTR,
			sizeof(JUSTBREAKREC));
	}
	DBMSG2(((LPSTR)"SetJust():x=%d,r=%d,e=%d,c=%d\r",
		lpJustifyWB->extra,lpJustifyWB->rem,lpJustifyWB->err,
		lpJustifyWB->count));
	DBMSG2(((LPSTR)"SetJust()***:x=%d,r=%d,e=%d,c=%d\r",
		lpJustifyLTR->extra,lpJustifyLTR->rem,lpJustifyLTR->err,
		lpJustifyLTR->count));

	/*	Advise the caller as to whether or not justification
	 *	adjustments will be required.
	 */
	if (lpJustifyWB->extra || lpJustifyWB->rem ||
		lpJustifyLTR->extra || lpJustifyLTR->rem
	){
		return TRUE;
	}else return FALSE;
}

/***********************************************************************
 *GetJustBreak
 *
 *	Calculate the additional pixels to add/subtract from the horizontal
 *	position.
 *Aldus Corporation 19 January 1987--sjp
 ***********************************************************************
 */
static short GetJustBreak (LPJUSTBREAKREC,JUSTBREAKTYPE);
static short GetJustBreak (lpJustBreak,justType)
	LPJUSTBREAKREC lpJustBreak;
	JUSTBREAKTYPE justType;
{
	short adjust = lpJustBreak->extra;

	DBMSG2(((LPSTR)"***GetJustBreak:bv=%d,e=%d,c=%d,cc=%d\r",
		adjust,lpJustBreak->err,lpJustBreak->count,lpJustBreak->ccount));
	if ((lpJustBreak->err -= lpJustBreak->rem) <= 0){
		++adjust;
		lpJustBreak->err += (short)lpJustBreak->count;
	}

	if ((justType != fromdrawmode) && lpJustBreak->count &&
		(++lpJustBreak->ccount > lpJustBreak->count)
	){
		/*	Not at a valid character position, return zero adjustment.
		 */
		adjust = 0;
	}
	DBMSG2(((LPSTR)"GetJustBreak***:bv=%d,e=%d,c=%d,cc=%d\r",
		adjust,lpJustBreak->err,lpJustBreak->count,lpJustBreak->ccount));

	return adjust;
}



/*********************************************************************
* Name: GetCharWidth()
*
* Action:	Get the widths of a range of characters.  This is an OEM
*			layer call.
*
* Returns:	The widths are returned in an array of shorts.
*
**********************************************************************
*/
short FAR PASCAL GetCharWidth(LPDV,LPSHORT,BYTE,BYTE,LPDF,LPDM,LPTEXTXFORM);
short FAR PASCAL GetCharWidth(lpdv,lpBuf,firstChar,lastChar,lpdf,lpdm,lpft)
	DV FAR *lpdv;		/* Far ptr the device descriptor */
	LPSHORT lpBuf;		/* Far ptr to the output width buffer */
	BYTE firstChar;
	BYTE lastChar;
	LPDF lpdf;			/* Far ptr to the font structure */
	DRAWMODE FAR *lpdm;	/* Far ptr to the justification info, etc. */
	LPTEXTXFORM lpft;	/* Font transformation structure */
{
	LPSHORT lpCharWidths;
	short defCharWidth;
	short lowerDefaultRange,upperDefaultRange;
	short i;
    LPFX lpfx;
	short rc=TRUE;

	/* get the extended font structure */
	lpfx=LockFont(lpdf);

	lpCharWidths = (LPSHORT) lpfx->rgWidths;

	/* get a scaled copy of the default character width */
	defCharWidth = Scale(
		*(lpCharWidths+(lpdf->dfDefaultChar-lpdf->dfFirstChar)),
		lpfx->sx, EM);
	DBMSG3(((LPSTR)">GetCharWidth(): first=%d,last=%d,sx=%d,EM=%d\r",
		firstChar,lastChar,lpfx->sx,EM));
	DBMSG3(((LPSTR)" GetCharWidth(): dffirst=%d,dflast=%d,dfavg=%d,def=%d\r",
		lpdf->dfFirstChar,lpdf->dfLastChar,lpdf->dfAvgWidth,defCharWidth));

	/* check to see if the last character is before the first...
	 * i.e. a parameter error.
	 */
	if(lastChar<firstChar){
		DBMSG3(((LPSTR)" GetCharWidth(): LAST BEFORE FIRST\r"));
		rc=FALSE;

	/* check to see if the entire range of desired characters is outside
	 * the font's specified range
	 */
	}else if((short)lpdf->dfFirstChar > (short)lastChar ||
		(short)lpdf->dfLastChar < (short)firstChar
	){
		/* all the requested characters are outside the font's
		 * specified range...return all default char widths
		 */
		DBMSG3(((LPSTR)" GetCharWidth(): ALL OUTSIDE\r"));
		for(i=firstChar;i<=lastChar;i++){
			*lpBuf = defCharWidth;
			lpBuf++;
		}

	}else{
		DBMSG3(((LPSTR)" GetCharWidth(): NORMAL\r"));

		/* check to see if any of the characters are below the
		 * font's specified first character.
		 * 	lowerDefaultRange > 0...if this is true
		 * 	lowerDefaultRange = 0...if this is false
		 */
		if((lowerDefaultRange=(short)lpdf->dfFirstChar-(short)firstChar)<=0){
			lowerDefaultRange=0;
		}

		/* check to see if any of the characters are above the
		 * font's specified last character.
		 * 	upperDefaultRange > 0...if this is true
		 * 	upperDefaultRange = 0...if this is false
		 */
		if((upperDefaultRange=(short)lastChar-(short)lpdf->dfLastChar)<=0){
			upperDefaultRange=0;
		}
		DBMSG3(((LPSTR)" GetCharWidth(): lower=%d,upper=%d\r",
			lowerDefaultRange,upperDefaultRange));

		/* set the widths of characters below the font's first char
		 * to defaults...if any
		 */
		for(i=firstChar;i<firstChar+lowerDefaultRange;i++){
			DBMSG3(((LPSTR)" GetCharWidth(): BELOW %d\r",i));
			*lpBuf = defCharWidth;
			lpBuf++;
		}

		/* set the widths of characters in the valid range
		 * ...if any
		 */
		for(i=firstChar+lowerDefaultRange;i<lastChar+1-upperDefaultRange;i++){
			if (lpdf->dfPitchAndFamily & 1){
				/* proportional font */
				*lpBuf = *(lpCharWidths + (i - lpdf->dfFirstChar));

				/* now scale it... */
				DBMSG3(((LPSTR)" GetCharWidth(): PROPORTIONAL..i=%d,unsc=%d,",
					i,*lpBuf));
				*lpBuf = Scale(*lpBuf, lpfx->sx, EM);
				DBMSG3(((LPSTR)"sc=%d\r",*lpBuf));
			}else{
				/* fixed pitch font...no need to scale this because
				 * it is already scaled
				 */
				*lpBuf = lpdf->dfAvgWidth;
				DBMSG3(((LPSTR)" GetCharWidth(): FIXED PITCH...i=%d,avg=%d\r",
					i,*lpBuf));
			}
			/* get ready for the next one */
			*lpBuf++;
		}

		/* set the widths of characters above the font's last char
		 * to defaults...if any
		 */
		for(i=lastChar+1-upperDefaultRange;i<lastChar+1;i++){
			DBMSG3(((LPSTR)" GetCharWidth(): ABOVE %d\r",i));
			*lpBuf=defCharWidth;
			*lpBuf++;
		}
	}
	DBMSG3(((LPSTR)"<GetCharWidth()\r"));
	return(rc);
}


/********************************************************
* Name: TrackKern()
*
* Action: Compute the amount of track kerning to add to
*	  the string length.
*
*********************************************************
*/
int FAR PASCAL TrackKern(lpdv, lpfx, cb)
LPDV lpdv;	/* Far ptr to the device descriptor */
LPFX lpfx;	/* Far ptr to the extended device font */
int cb; 	/* The un-kerned string width */

    {
    int iTrack;
    int iKernAmount;
    TRACK FAR *lpTrack;
    LPKT lpkt;

    /* Fail if this is not our font */
    if (!(lpfx->lpdf->dfType & 0x080))
        return(0);

    iKernAmount = 0;
    lpkt = lpfx->lpkt;
    iTrack = lpdv->dh.iTrack;
    if (lpkt && iTrack>0)
	{
	if (iTrack <= lpkt->cTracks)
	    {
	    lpTrack = &lpkt->rgTracks[lpdv->dh.iTrack - 1];

	    iKernAmount = KernAmount(
				lpTrack->iKernMin,
				lpTrack->iKernMax,
				lpTrack->iPtMax,
				lpTrack->iPtMin,
				Scale(lpfx->sy, 72, lpdv->dh.iRes),
				cb
				);

	    iKernAmount = Scale(iKernAmount, lpdv->dh.iRes, 7200);
	    }
	}
    return(iKernAmount);
    }


/****************************************************************
* Name: DumpStr()
*
* Action: Execute the Postscript calls necessary to output a
*     string of text.
*
* Returns: A long value which is the length of the string.
*
****************************************************************
*/
static long DumpStr(DV FAR *,LPFX,LPDM,int,int,BOOL,LPSTR,long,int,int,int);
static long DumpStr(lpdv,lpfx,lpdm,ix,iy,fPrint,lpbStr,cxStr,count,cBrk,cxBrk)

DV FAR *lpdv;	/* Far ptr the device descriptor */
LPFX lpfx;		/* Far ptr to the FONTINFO (font metrics) structure */
LPDM lpdm;		/* Far ptr to the justification info, etc. */
int ix;			/* The horizontal origin */
int iy;			/* The vertical origin */
BOOL fPrint;	/* TRUE if the string should be output */
LPSTR lpbStr;	/* Far ptr to the output string */
long cxStr;		/* Desired length of string less word adjustment */
int count;		/* Number of characters in the output string */
int cBrk; 		/* The number of break characters */
int cxBrk;		/* The pixel width added to break characters */
{
	LPDF lpdf = lpfx->lpdf;

    if ((lpdf->dfPitchAndFamily & 1) && !lpdv->dh.fIntWidths){
	    cxStr = ldiv(lmul(cxStr, (long) lpfx->sx), (long)EM);
	}
	cxStr += TrackKern(lpdv, lpfx, count);

	if (fPrint){
		if (cxBrk){
		    PrintChannel(lpdv, (LPSTR) "%d %d SetJustify\n", cxBrk, cBrk);
		}
		PrintChannel(lpdv, (LPSTR)"%d %d %d %q StrBlt\n",
			ix, iy+lpdf->dfAscent, (int) cxStr, lpbStr, count);
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
****************************************************************
*/
DWORD FAR PASCAL ShowStr(DV FAR *,int,int,LPSTR,int,LPFX,LPDM,LPFT,short FAR *);
DWORD FAR PASCAL ShowStr(lpdv, ix, iy, lpbSrc, cb, lpfx, lpdm, lpft, lpdx)

DV FAR *lpdv;	/* Far ptr the device descriptor */
int ix;			/* The horizontal origion */
int iy;			/* The vertical origion */
LPSTR lpbSrc;	/* Far ptr to the source string */
int cb;			/* Size of the source string */
LPFX lpfx;		/* Far ptr to the FONTINFO (font metrics) structure */
LPDM lpdm;		/* Far ptr to the justification info, etc. */
LPFT lpft;		/* Font transformation structure */
short FAR *lpdx;/* Far ptr to delta-x moves */
{
    long cxStr;
    long cxSrc;
    int i, pi;
    int iCh;
	int cBrk;
	int cxBrk;
    int cxChar;
	int runbreak;
    int dfBreakChar;
    int dfFirstChar;
    int dfLastChar;
    short far *lpcxChar;
    BOOL fPrint;
	BOOL fVariablePitch;
    LPDF lpdf;
	LPSTR lpbStr;
	JUSTBREAKREC justifyLTR;
	JUSTBREAKREC justifyWB;
	JUSTBREAKTYPE justType;

    if (cb==0)
		return(0);
    else if (!(fPrint = (cb>0))){
		cb = -cb;
	}

	if(fPrint) DBMSG2(((LPSTR)"***ShowStr(): x=%d,y=%d\r",ix,iy));

	/*	If doing opaque background text, make a recursive call to
	 *	ShowStr() to get the dimensions of the whole output line,
	 *	including any adjustments we make for ExtTextOut(), then
	 *	draw and opaque box using the dimensions of the line.  After
	 *	that, fall through and really output the line.
	 */
	if (fPrint && (lpdm->bkMode==OPAQUE)){
		long opaq = ShowStr(lpdv,ix,iy,lpbSrc,(-cb),lpfx,lpdm,lpft,lpdx);

		PrintChannel(lpdv, (LPSTR)"%d %d %d %d OpaqueBox\n",
                        ix, iy, (int)(LOWORD(opaq)), (int)(HIWORD(opaq)));
	}

    lpdf = lpfx->lpdf;
	dfBreakChar = (int)(lpdf->dfBreakChar + lpdf->dfFirstChar) & 0xff;
	dfFirstChar = (int)lpdf->dfFirstChar & 0xff;
	dfLastChar = (int)lpdf->dfLastChar & 0xff;

	SetJustification(lpdv,lpdm,
		(LPJUSTBREAKTYPE)&justType,
		(LPJUSTBREAKREC)&justifyWB,
		(LPJUSTBREAKREC)&justifyLTR
	);

    /*	Get width table.
	 */
    if (lpdf->dfPitchAndFamily & 1){
		fVariablePitch = TRUE;
		lpcxChar = (short far *) lpfx->rgWidths;
	}else{
		fVariablePitch = FALSE;
		lpcxChar = 0L;
	}

	/*	Change font if we're going to output the string.
	 */
    if (fPrint){
		SelectFont(lpdv, lpfx);
	}

	/*	For each character in the string.
	 */
    for (cxSrc=cxStr=0L, lpbStr=lpbSrc,
		cBrk=cxBrk=runbreak=pi=i=0; i < cb; ++i, ++lpbSrc){

		/*	If doing ExtTextOut() and the previous character we
		 *	examined did not have a true width matching the desired
		 *	width, output a partial string (a "run") and advance
		 *	by the difference, then continue width-getting.
		 */
		if (runbreak){
			cxStr = DumpStr(lpdv,lpfx,lpdm,ix,iy,fPrint,lpbStr,cxStr,i-pi,cBrk,cxBrk);
			cxSrc += cxStr + runbreak;
			ix += (int)cxStr + runbreak;
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
		    continue;

		/*	Get character width
		 */
		if (fVariablePitch){
		    cxChar = *(lpcxChar + (iCh - dfFirstChar));
		    if (lpdv->dh.fIntWidths){
				cxChar = Scale(cxChar, lpfx->sx, EM);
			}
		}else
			cxChar = lpdf->dfAvgWidth;

		/*	If doing ExtTextOut(), pick up the difference between the
		 *	desired character width and the true character width.
		 */
		if (lpdx){
		    if (fVariablePitch && !lpdv->dh.fIntWidths){
				runbreak = (int)*lpdx - Scale(cxChar, lpfx->sx, EM);
			}else{
				runbreak = (int)*lpdx - cxChar;
			}
			++lpdx;
		}

		/*	Apply letter justification.
		 */
		if (justType == justifyletters){
			cxChar += GetJustBreak(&justifyLTR, justType);
		}else{
			if ((justType == fromdrawmode) || !(justifyLTR.count) ||
				(++justifyLTR.ccount <= justifyLTR.count)
			){
				cxChar += justifyLTR.extra;
			}
		}

		/*	Apply word justification.
		 */
		if (iCh==dfBreakChar){
			++cBrk;
			cxBrk += GetJustBreak((LPJUSTBREAKREC) &justifyWB, justType);
		}

	    cxStr += cxChar;
	}

	/*	Output string.
	 */
	cxSrc += DumpStr(lpdv,lpfx,lpdm,ix,iy,fPrint,lpbStr,cxStr,i-pi,cBrk,cxBrk);

	/*	Add in the final runbreak value, just in case the last delta-x
	 *	move was not equal to the width of the last character.  This
	 *	is important to the extent of the line, but unimportant to
	 *	output of the line.
	 */
	cxSrc += runbreak;

	if (!fPrint){
		lpdm->BreakErr = justifyWB.err;

		if (justType != fromdrawmode){
			lpdv->dh.epJustWB.err = justifyWB.err;
			lpdv->dh.epJustWB.ccount = justifyWB.ccount;
			lpdv->dh.epJustLTR.err = justifyLTR.err;
			lpdv->dh.epJustLTR.ccount = justifyLTR.ccount;
		}
	}

    return (MAKELONG(cxSrc, lpdf->dfPixHeight));
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
*****************************************************************
*/
DWORD FAR PASCAL ExtTextOut(lpdv,ix,iy,lprcClip,lpbSrc,cbSrc,lpdf,lpdm,lpft,
	lpdx,lprcOpaq,options)

DV FAR *lpdv;		/* Far ptr the device descriptor */
int ix;				/* The horizontal origion */
int iy;				/* The vertical origion */
LPRECT lprcClip;	/* Far ptr to the clipping rectangle */
LPSTR lpbSrc;		/* Far ptr to the source string */
int cbSrc;			/* Size of the source string */
LPDF lpdf;			/* Far ptr to the FONTINFO structure */
DRAWMODE FAR *lpdm;	/* Far ptr to the justification info, etc. */
LPTEXTXFORM lpft;	/* Font transformation structure */
short FAR *lpdx;	/* Far ptr to array of delta-x moves */
LPRECT lprcOpaq;	/* Far ptr to opaque rectangle */
WORD options;		/* 2.0+ option switches */
    {
	RECT cliprect, opaqrect;
    LPSTR lpbRun;
    int cbRun;
    int iCh, iChPrev;
    FARPROC  lpfn;
    long lsize;
    LPFX lpfx;		/* Far ptr to the extended device font structure */
    BOOL fSimulate;	/* TRUE if a character must be simulated */
    int cxStr;		/* The accumulated string width */
    int cxKern; 	/* The number of pixels to kern a character */
    int cxChar; 	/* The pixel width of a character */

	if (lpdv->dh.iType!=-1)
		return (0);				/* fail: this is not our PDEVICE */

	/*	Make local copies of the clip and opaque rectangles so we
	 *	may freely modify them.
	 */
	if (lprcClip){
		lmemcpy((LPSTR) &cliprect, (LPSTR)lprcClip, sizeof(RECT));
		lprcClip = (LPRECT) &cliprect;
	}
	if (lprcOpaq){
		lmemcpy((LPSTR) &opaqrect, (LPSTR)lprcOpaq, sizeof(RECT));
		lprcOpaq = (LPRECT) &opaqrect;
	}

	/*	Modify opaque and clip regions per options switches.
	 */
	if (lprcOpaq)
		{
		if (options & EXTTEXT_D2)
			{
			/*	lprcOpaq should be used as a clipping rectangle.
			 */
			if (lprcClip)
				IntersectRect(lprcClip, lprcClip, lprcOpaq);
			else
				{
				lmemcpy((LPSTR) &cliprect, (LPSTR)lprcOpaq, sizeof (RECT));
				lprcClip = (LPRECT) &cliprect;
				}
			}

		if (options & EXTTEXT_D1)
			{
			/*	lprcOpaq should be used as an opaque rectangle.
			 */
			if (lprcClip)
				IntersectRect(lprcOpaq, lprcClip, lprcOpaq);
			}
		else
			lprcOpaq = 0L;
		}

    lpfx = LockFont(lpdf);

    if (lpdm) lpdv->dh.TextColor = lpdm->TextColor;

#ifdef OLDCODE
    /* Raise the Courier font a little */
    if (!(lpdf->dfPitchAndFamily&1)){
		iy -= Scale(lpfx->sy, 1, 12);
		DBMSG(((LPSTR)"ShowStr(): Courier tweak x=%d,y=%d\r",ix,iy));
	}
#endif

	if (cbSrc > 0 && lprcOpaq){
		PrintChannel(lpdv, (LPSTR)"%d %d %d %d OpaqueBox\n",
			lprcOpaq->left, lprcOpaq->top,
			(lprcOpaq->right - lprcOpaq->left),
			(lprcOpaq->bottom - lprcOpaq->top));
	}
		
    if ((cbSrc<0) || ((lpdf->dfPitchAndFamily & 0x0f0) == FF_DECORATIVE)){
		lsize = ShowStr(lpdv,ix,iy,lpbSrc,cbSrc,lpfx,lpdm,lpft,lpdx);
		goto DONE;
	}

    /* Make sure that a font is selected in case first char is simulated */
    SelectFont(lpdv, lpfx);

    cxStr = 0;
    ClipBox(lpdv, lprcClip);

    lpbRun = lpbSrc;
    cbRun = 0;
    iCh = 0;
    fSimulate = FALSE;
    cxKern = 0;
    while(--cbSrc>=0){
		iChPrev = iCh;
		iCh = ((int) *lpbSrc++) & 0x0ff;

#if SIMULATE
		if (lpfn = GetSimulate(iCh)){
		    fSimulate = (*lpfn)((LPDV)(long)NULL, ix+cxStr, iy, lpfx, cxChar);
		}
#endif
		if (lpdv->dh.fPairKern && (lpdf->dfPitchAndFamily & 1)){
		    cxKern = PairKern(lpdv->dh.iRes, lpfx->lpkp, iChPrev, iCh);
		}

		if (fSimulate || cxKern!=0){
		    /* Output the previous run of characters */
		    cxStr += (int)LOWORD(ShowStr(lpdv,ix+cxStr,iy,lpbRun,cbRun,lpfx,lpdm,lpft,lpdx));
		    cxStr += cxKern;
		    cxKern = 0;

		    if (fSimulate){
				if (lpdf->dfPitchAndFamily & 1){
				    cxChar = lpfx->rgWidths[iCh - lpdf->dfFirstChar];
				}
				else cxChar = lpdf->dfAvgWidth;

				cxChar += TrackKern(lpdv, lpfx, 1);
				(*lpfn)(lpdv, ix+cxStr, iy, lpfx, cxChar);
				cxStr += cxChar + lpdm->CharExtra;
				lpbRun = lpbSrc;
				cbRun = 0;
				fSimulate = FALSE;
			}else{
				cbRun = 1;
				lpbRun = lpbSrc - 1;
			}
		    continue;
		}
		++cbRun;
	}
    cxStr += (int)LOWORD(ShowStr(lpdv,ix+cxStr,iy,lpbRun,cbRun,lpfx,lpdm,lpft,lpdx));
    ClipBox(lpdv, (RECT FAR *) (long) NULL);
    lsize = MAKELONG(cxStr, lpdf->dfPixHeight);

DONE:
    return(ProjectString(lsize, lpfx->escapement));
	}



/***************************************************************
* Name: StrBlt()
*
* Action: Call ExtTextOut() with added (for Windows 2.0) structs
*      set to zero.
*
*****************************************************************
*/
DWORD FAR PASCAL StrBlt(lpdv,ix,iy,lprcClip,lpbSrc,cbSrc,lpdf,lpdm,lpft)

DV FAR *lpdv;	     /* Far ptr the device descriptor */
int ix; 	     /* The horizontal origion */
int iy; 	     /* The vertical origion */
LPRECT lprcClip;     /* Far ptr to the clipping rectangle */
LPSTR lpbSrc;	     /* Far ptr to the source string */
int cbSrc;	     /* Size of the source string */
LPDF lpdf;	     /* Far ptr to the FONTINFO structure */
DRAWMODE FAR *lpdm;	/* Far ptr to the justification info, etc. */
LPTEXTXFORM lpft;   /* Font transformation structure */
    {
	if (!lpdv->dh.iType){
		return (dmStrBlt(lpdv,ix,iy,lprcClip,lpbSrc,cbSrc,lpdf,lpdm,lpft));
	}

	return (ExtTextOut(lpdv,ix,iy,lprcClip,lpbSrc,cbSrc,lpdf,lpdm,lpft,
		(short FAR *)0L, (LPRECT)0L, (WORD)0));
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
*******************************************************
*/
void PASCAL SelectFont(lpdv, lpfx)
	DV FAR *lpdv;	    /* Far ptr to the device descriptor */
	LPFX lpfx;	   /* Far ptr to extra font info for driver */
{
    LPDF lpdf;

    ASSERT(lpdv!=(LPSTR) (long) NULL);
    ASSERT(lpfx!=(LPSTR) (long) NULL);

    /* Check to see if the font instance handle has changed */
    if (lpfx->lid != lpdv->dh.lidFont || lpfx->lid==-1L)
	{
	lpdf = lpfx->lpdf;

	lpdv->dh.lidFont = lpfx->lid;


	/* Output the font change command to the printer */
	PrintChannel(lpdv, (LPSTR) "%d %d %d %d %d %d %d %d ",
	    (lpdf->dfBreakChar + lpdf->dfFirstChar) & 0X0ff,
	    lpfx->escapement,
	    lpfx->orientation,

		/* This is an area to be careful around.
		 * If a user wants the Adobe font sizes, then leave this as is.
		 * However, if "true" point sizes are desired then the internal
		 * leading subtracted here should go away.  In the current
		 * implementation internal leading is 0.
		 */
	    lpfx->sx-lpdf->dfInternalLeading,
	    lpfx->sy-lpdf->dfInternalLeading,

	    lpdf->dfUnderline,
	    lpdf->dfStrikeOut,
	    Lightness(lpdv->dh.TextColor) < 500
	    );
	PrintChannel(lpdv, (LPSTR) "/%s ", (LPSTR) lpfx->lszFont);

	/* Don't remap the Symbol font for the ANSI character set */
	if ((lpdf->dfPitchAndFamily & 0x0f0) != FF_DECORATIVE)
	    PrintChannel(lpdv, (LPSTR) "/font%d ANSIFont ", lpfx->iFont);

	PrintChannel(lpdv, (LPSTR) "font\n");
	}

}

