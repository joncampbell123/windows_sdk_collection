#include "pscript.h"

extern void FAR PASCAL lmemcpy(LPSTR, LPSTR, int);

/********************************************************************/
/*#define DEBUG_ON*/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

/********************************************************
* Name: RealizePen()
*
* Action: This routine is called from RealizeObject() to
*	  create a new physical pen.
*
* Returns: The amount of memory to allocate for the pen descriptor.
*
*********************************************************
*/
int PASCAL RealizePen(lpdv, lpLogPen, lppen)
DV FAR *lpdv;
LPLOGPEN lpLogPen;
PEN FAR *lppen;
    {
    ASSERT(lpdv!=NULL);

    if (lppen)
	{
	ASSERT(lpLogPen!=NULL);

	lppen->lid = ++(lpdv->dh.lid);
	lmemcpy((LPSTR) (&lppen->lopn), (LPSTR) lpLogPen, sizeof(LOGPEN));
	}
    return(sizeof(PEN));
    }





/********************************************************
* Name: RealizeBrush()
*
* Action: This routine is called from RealizeObject() to
*	  create a new physical brush.
*
* Returns: The amount of memory to allocate for the brush descriptor.
*
*********************************************************
*/

int PASCAL RealizeBrush(lpdv, lplb, lpbr)
DV FAR *lpdv;
LPLOGBRUSH lplb;
BR FAR *lpbr;
    {

    /* These are the 8 X 8 hatched brush patterns */
    static unsigned char rgbHatch[] = {
        0xff, 0xff, 0xff,    0, 0xff, 0xff, 0xff, 0xff,  /* Vertical   */
        0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef,  /* Horizontal */
        0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,  /* BDIAGONAL */
        0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f,  /* FDIAGONAL */
	    0xef, 0xef, 0xef, 0x00, 0xef, 0xef, 0xef, 0xef,  /* CROSS */
	    0x7e, 0xbd, 0xdb, 0xe7, 0xe7, 0xdb, 0xbd, 0x7e,  /* DIAGCROSS */
	};

    register char bPat;
    LPSTR lpb;
    int i, j;
    char bMask;
    BITMAP FAR *lpbm;	/* Far ptr to the pattern bitmap */


    ASSERT(lpdv!=NULL);

    if (lpbr)
	{
	ASSERT(lplb!=NULL);

	lpbr->lid = ++lpdv->dh.lid;
	lmemcpy((LPSTR) (&lpbr->lb), (LPSTR)lplb, sizeof(LOGBRUSH));

	if (lplb->lbStyle == BS_PATTERN)
	    {

	    /* Get the 8 byte brush pattern from the bitmap */
	    lpbm = (BITMAP FAR *) lplb->lbColor;

	    ASSERT(lpbm!=NULL);

            lpb = lpbm->bmBits;
            bMask = 1;


            lpb = lpbm->bmBits;
            for (i=0; i<8; ++i)
                {
                lpbr->rgbPat[i] = *lpb;
                lpb += lpbm->bmWidthBytes;
                }

	    }
	else if (lplb->lbStyle==BS_HATCHED)
	    {
	    /* Get the appropriate hatched brush pattern */
	    lpb = rgbHatch + (lplb->lbHatch<<3);
	    lmemcpy((LPSTR) lpbr->rgbPat, lpb, sizeof(lpbr->rgbPat));
	    }
	}
    return(sizeof(BR));
    }


#ifdef UNDEFINED
int PASCAL RealizeBrush(lpdv, lplb, lpbr)
DV FAR *lpdv;
LPLOGBRUSH lplb;
BR FAR *lpbr;
    {
    /* These are the 8 X 8 hatched brush patterns */
    static unsigned char rgbHatch[] =
	    {
	    0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef, 0xef,  /* Vertical */
	    0xff, 0xff, 0xff,	 0, 0xff, 0xff, 0xff, 0xff,  /* Horizontal */
	    0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f,  /* BDIAGONAL */
	    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,  /* FDIAGONAL */
	    0xef, 0xef, 0xef, 0x00, 0xef, 0xef, 0xef, 0xef,  /* CROSS */
	    0x7e, 0xbd, 0xdb, 0xe7, 0xe7, 0xdb, 0xbd, 0x7e,  /* DIAGCROSS */
	    };

    register char bPat;
    LPSTR lpb;
    int i, j;
    char bMask;
    BITMAP FAR *lpbm;	/* Far ptr to the pattern bitmap */


    ASSERT(lpdv!=NULL);

    if (lpbr)
	{
	ASSERT(lplb!=NULL);

	lpbr->lid = ++lpdv->dh.lid;
	lmemcpy((LPSTR) (&lpbr->lb), (LPSTR)lplb, sizeof(LOGBRUSH));

	if (lplb->lbStyle == BS_PATTERN)
	    {

	    /* Get the 8 byte brush pattern from the bitmap */
	    lpbm = (BITMAP FAR *) lplb->lbColor;

	    ASSERT(lpbm!=NULL);

	    lpb = lpbm->bmBits;
	    bMask = 1;

	    /* Rotate the pattern 90 degrees to match PostScript */
	    for (i=0; i<8; ++i)
		{
		bPat = 0;
		lpb = lpbm->bmBits + (lpbm->bmWidthBytes<<3);
		for (j=0; j<8; ++j)
		    {
		    lpb -= lpbm->bmWidthBytes;
		    bPat = (bPat >> 1) & 0x7f;
		    if (*lpb & bMask)
			bPat |= 0x80;
		    }
		lpbr->rgbPat[i] = bPat;
		bMask <<= 1;
		}
	    }
	else if (lplb->lbStyle==BS_HATCHED)
	    {
	    /* Get the appropriate hatched brush pattern */
	    lpb = rgbHatch + (lplb->lbHatch<<3);
	    lmemcpy((LPSTR) lpbr->rgbPat, lpb, sizeof(lpbr->rgbPat));
	    }
	}
    return(sizeof(BR));
    }

#endif




/**********************************************************
* Name: RealizeObject()
*
* Action: This routine creates a data strucuture containing
*	  required to realize a specific instance of an object.
*	  The object may be a pen, brush, or font.
*
************************************************************
*/
int FAR PASCAL RealizeObject(lpdv, iStyle, lpbIn, lpbOut, lptf)
	DV FAR *lpdv;	/* Far ptr to the device descriptor */
	int iStyle;	    /* The object type (pen, brush, font) */
	LPSTR lpbIn;	/* Far ptr to structure containing object attributes */
	LPSTR lpbOut;	/* Far ptr to structure for storing realized object */
	LPTEXTXFORM lptf; /* Far ptr. Contains additional object attributes */
{
    int cb;

	DBMSG(((LPSTR)">RealizeObject(): lpdv=%ld,iStyle=%d\r",lpdv,iStyle));

	if (!lpdv->dh.iType){
		if (iStyle==OBJ_FONT)
			return(0);
		else
			return(dmRealizeObject(lpdv, iStyle, lpbIn, lpbOut, lptf));
	}

	if (lpdv->dh.iType!=-1)
		return (0);				/* fail: this is not our PDEVICE */

    ASSERT(lpdv!=NULL);

    switch(iStyle)
	{
	case OBJ_PEN:
	    cb = RealizePen(lpdv, (LOGPEN FAR *)lpbIn, (PEN FAR *)lpbOut);
	    break;
	case OBJ_BRUSH:
	    cb = RealizeBrush(lpdv, (LOGBRUSH FAR *)lpbIn, (BR FAR *)lpbOut);
	    break;

	case OBJ_FONT:
	    cb = RealizeFont(  lpdv,
			       (LOGFONT FAR *)lpbIn,
			       (FONTINFO FAR *) lpbOut,
			       lptf
			    );
	    break;
	}

	DBMSG(((LPSTR)"<RealizeObject(): lpdv=%ld\r",lpdv));
    return(cb);
}
