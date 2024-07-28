/**[f******************************************************************
 * exttext.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * EXTTEXT.C
 *
 * 20Aug87 sjp		Transferred TrackKern() to TEXT.C.
 *			Transferred PairKern() to KERN.C.
 * 15Sept88 dgh		Added GetPSName() -- GETFACENAME.
 **********************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "etm.h"
#include "fonts.h"
#include "debug.h"
#include "utils.h"
#include "exttext.h"
#include "truetype.h"


/*************************************************************
* Name: GetExtTable()
*
* Action: Get the character widths in a given range.
*
***************************************************************/

BOOL FAR PASCAL GetExtTable(lpdv, lpetd, lpwOut)
LPDV lpdv;		/* Far ptr to the device descriptor */
LPEXTTEXTDATA lpetd;	/* Far ptr to the extended text data */
LPSHORT lpwOut;
{
	int	iCh, lesser, greater;
    int  defaultWidth, i, nCount;
	int	iChLast, tgtStart, srcStart;
	LPFX lpfx;
	LPDF lpdf;
	LPSHORT lpw;

	lpw = (LPSHORT) lpetd->lpInData;
	iCh = *lpw++ & 0x0ff;
	iChLast = *lpw++ & 0x0ff;

	if (iChLast < iCh)
	   return FALSE;

    lpdf = lpetd->lpFont;

    lesser = (lpdf->dfLastChar < (BYTE)iChLast) ? (lpdf->dfLastChar) : (iChLast) ;
    greater = (lpdf->dfFirstChar > (BYTE)iCh) ? (lpdf->dfFirstChar) : (iCh) ;
    nCount = lesser - greater + 1;
    if(nCount <= 0)
        nCount = 0;

    tgtStart = lpdf->dfFirstChar - iCh;

    if(tgtStart > 0)
        srcStart = 0;
    else
    {
        srcStart = -tgtStart;
        tgtStart = 0;
    }

    if (lpdf->dfType & TYPE_TRUETYPE)
    {
        LPTTFONTINFO lpttfi = (LPTTFONTINFO) ((LPSTR)lpdf + lpdf->dfBitsOffset);

        if (!(lpdf->dfType & TYPE_HAVEWIDTHS)) {
            EngineGetCharWidth(lpdf, 0, 
                                255, lpttfi->rgwWidths);
            lpdf->dfType |= TYPE_HAVEWIDTHS;    
        }

        defaultWidth = lpttfi->rgwWidths[lpdf->dfDefaultChar];

        //  initialize entire array to default width
	    for (i = 0 ; i <= iChLast - iCh ; i++)
		    lpwOut[i] = defaultWidth;

        lmemcpy((LPSTR)(lpwOut + tgtStart), (LPSTR) (lpttfi->rgwWidths + srcStart),
                nCount * sizeof(WORD));

        return TRUE;
    }


	lpfx = LockFont(lpetd->lpFont);
	lpdf = lpfx->lpdf;


	/* Fail if this is not our font */
	if (!(lpdf->dfType & 0x080))
		return(FALSE);


    //  initialize entire array to default width
	for (i = 0 ; i <= iChLast - iCh ; i++)
		lpwOut[i] = defaultWidth;


	/* For fixed pitch fonts, all widths are the average character width */
	if (!(lpdf->dfPitchAndFamily & 1)) 
    {
        //  initialize entire array to default width
	    for (i = 0 ; i <= iChLast - iCh ; i++)
		    lpwOut[i] = lpfx->fxUnscaledAvgWidth;

			/* #ifdef NOV18 */
			/* LF 11/6/87 Fixed code to return unscaled average char width */

	} 
    else 	/* Variable pitch font */
    {
	
        defaultWidth = lpfx->rgWidths[lpdf->dfDefaultChar];

        //  initialize entire array to default width
	    for (i = 0 ; i <= iChLast - iCh ; i++)
		    lpwOut[i] = defaultWidth;

        lmemcpy((LPSTR)(lpwOut + tgtStart), (LPSTR) (lpfx->rgWidths + srcStart),
                nCount * sizeof(WORD));
	}

	return(TRUE);
}


/*******************************************************************
* Name: GetEtm()
*
* Action: Get the extended text metrics information from the
*	  device font.
*
*********************************************************************/

short FAR PASCAL GetEtm(lpdv, lpetd, lpbOut)
LPDV lpdv;		/* Far ptr to the device descriptor */
LPEXTTEXTDATA lpetd;	/* Far ptr to the extended text data */
LPSTR lpbOut;
{
	LPSTR lpbSrc;
	int	cb, cbT;
	LPDF lpdf;

	if (!(lpdf = lpetd->lpFont))
		return 0;

        /* Currently not supported for TrueType */
        if (lpdf->dfType & TYPE_TRUETYPE)
            return 0;

	/* Fail if this is not our font */
	if (!(lpdf->dfType & 0x080))
		return 0;

	if (lpdf->dfExtMetricsOffset) {
		lpbSrc = ((LPSTR)lpdf) + lpdf->dfExtMetricsOffset;

		cb = *((short far * )lpetd->lpInData);
		if (cb > sizeof(ETM))
			cb = sizeof(ETM);

		cbT = cb;
		while (--cbT >= 0)
			*lpbOut++ = *lpbSrc++;

		return(cb);
	} else
		return(0);
}


/**************************************************************
* Name: GetPairKern()
*
* Action: Get the pair kerning table from the device font structure.
*
****************************************************************/

short FAR PASCAL GetPairKern(lpdv, lpetd, lpbDst)
LPDV lpdv;		/* Far ptr to the device descriptor */
LPEXTTEXTDATA lpetd;	/* Far ptr to the extended text data */
LPSTR lpbDst;
{
	LPWORD lpbSrc;
	LPWORD lpbLim;
	LPKP lpkp;
	LPFX lpfx;
        LPDF lpdf;

	int	dfHeight, temp;
	BOOL flag;

   // Not currently supported for TrueType
   lpdf = lpetd->lpFont;
   if ((lpdf->dfType & TYPE_TRUETYPE) || (lpdf->dfType & TYPE_SUBSTITUTED))
       return 0;

	lpfx = LockFont(lpetd->lpFont);

	/* Fail if this is not our font */
	if (!(lpfx->lpdf->dfType & 0x080))
		return(0);


	DBMSG(((LPSTR)"***GetPK():\r"));

	lpkp = lpfx->lpkp;

	dfHeight = lpfx->lpdf->dfPixHeight /* -lpfx->lpdf->dfInternalLeading */ ;
	DBMSG(((LPSTR)"dfPixHt=%d,dIntLead=%d\r",
	    lpfx->lpdf->dfPixHeight, lpfx->lpdf->dfInternalLeading));


	if (lpkp) {
		lpbSrc = (LPWORD) lpkp->rgPairs;
		lpbLim = (LPWORD) & lpkp->rgPairs[lpkp->cPairs];

		flag = FALSE;
		while (lpbSrc < lpbLim) {
			temp = *lpbSrc++;
			if (flag) {
				DBMSG(((LPSTR)"%d,", temp));
				temp = Scale(temp, dfHeight, 1000);
				DBMSG(((LPSTR)"%d\r", temp));
			} else {
				char	c1, c2;
				c1 = (char)(temp >> 8);
				c2 = (char)(temp & 0x00ff);
				DBMSG(((LPSTR)"pair:%c,%c ", c1, c2));
			}
			*((LPWORD)lpbDst)++ = temp;
			flag = !flag;
		}
		return(lpkp->cPairs);
	} else 
		return(0);
}


/*---------------------------------------------------------------------------
 * This routine is called by the GETFACENAME escape and is designed to        
 * provide the Postscript face name of the current physical font.             
 * Returns: a positive value if successful or -1 otherwise.                   
 * Digby Horner - 9/15/88 Adobe Systems Inc.                                  
 *
 * #ifndef ADOBE_11_1_88
 *
 *--------------------------------------------------------------------------*/

short FAR PASCAL GetPSName(lpdv, lpetd, lpwOut)
LPDV lpdv;		/* Far ptr to the device descriptor */
LPEXTTEXTDATA lpetd;	/* Far ptr to the extended text data */
LPSTR lpwOut;		/* Far ptr to a 60 byte buffer */
{
	LPFX lpfx;
	LPDF lpdf;

	ASSERT(lpdv);
	ASSERT(lpetd);

    lpdf = lpetd->lpFont;

    if(lpdf->dfType & TYPE_SUBSTITUTED)
        lpdf = (LPFONTINFO)((char far *)lpdf + 
            lpdf->dfBitsOffset + sizeof(TTFONTINFO));
            //  give GetFaceName the Device FontInfo  if TT is substituted.

    if (lpdf->dfType & TYPE_TRUETYPE) 
    {
        LPTTFONTINFO lpttfi = (LPTTFONTINFO) ((LPSTR)lpdf + lpdf->dfBitsOffset);

        lstrcpy(lpwOut, lpttfi->TTFaceName);
        return 1;
    }

    if ((lpfx = LockFont(lpdf)) != NULL) 
    {
		lpdf = lpfx->lpdf;
    	if (lpdf->dfType & 0x080) 
        {
	    	lstrcpy(lpwOut, lpfx->lszFont);
		    return 1;
		}
	}
	return - 1;
}

