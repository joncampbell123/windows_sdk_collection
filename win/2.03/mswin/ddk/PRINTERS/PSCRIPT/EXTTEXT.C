/*********************************************************************
 * EXTTEXT.C
 *
 * 20Aug87	sjp		Transferred TrackKern() to TEXT.C.
 *					Transferred PairKern() to KERN.C.
 *********************************************************************
 */

#include "pscript.h"


DWORD FAR PASCAL StrBlt(LPDV, int, int, LPRECT, LPSTR, int, LPDF, LPDM, LPFT);
LPFX FAR PASCAL LockFont(LPDF);
int FAR PASCAL KernAmount(int, int, int, int, int, int);

typedef WORD FAR *LPWORD;

/****************************************************************************/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif
/****************************************************************************/



/*************************************************************
* Name: GetExtTable()
*
* Action: Get the character widths in a given range.
*
**************************************************************
*/
BOOL FAR PASCAL GetExtTable(lpdv, lpetd, lpwOut)
LPDV lpdv;		/* Far ptr to the device descriptor */
LPEXTTEXTDATA lpetd;	/* Far ptr to the extended text data */
short far *lpwOut;
    {
    int iCh;
    int iChLast;
    int cb;
    LPFX lpfx;
    LPDF lpdf;
    short far *lpw;


    lpfx = LockFont(lpetd->lpFont);
    lpdf = lpfx->lpdf;


    /* Fail if this is not our font */
    if (!(lpdf->dfType & 0x080))
        return(FALSE);

	lpw = (short far *) lpetd->lpInData;
/*	lpw = (LPSTR) lpetd->lpInData;
 */
    iCh = *lpw++ & 0x0ff;
    iChLast = *lpw++ &0x0ff;

    /* Set all extents less than the first character to zero */
    while (iCh < lpdf->dfFirstChar)
	{
	*lpwOut++ = 0;
	++iCh;
	}

    /* For fixed pitch fonts, all widths are the average character width */
    if (!(lpdf->dfPitchAndFamily & 1))
	{
	while (iCh<=iChLast && iCh<=lpdf->dfLastChar)
	    {
#ifdef WIN20
            /* LF 11/6/87 Fixed code to return unscaled average char width */
            *lpwOut++ = lpfx->fxAvgWidth;
#else
	    *lpwOut++ = lpdf->dfAvgWidth;
#endif
	    ++iCh;
	    }
	}
    else
	{
	/* Variable pitch font */
	lpw = lpfx->rgWidths - lpdf->dfFirstChar;
	while (iCh<=iChLast && iCh<=lpdf->dfLastChar)
	    {
	    *lpwOut++ = *(lpw + iCh);
	    ++iCh;
	    }
	}

    while (iCh++<=iChLast)
	*lpwOut++ = 0;
    return(TRUE);
    }


/*******************************************************************
* Name: GetEtm()
*
* Action: Get the extended text metrics information from the
*	  device font.
*
*********************************************************************
*/
short FAR PASCAL GetEtm(lpdv, lpetd, lpbOut)
LPDV lpdv;		/* Far ptr to the device descriptor */
LPEXTTEXTDATA lpetd;	/* Far ptr to the extended text data */
LPSTR lpbOut;
    {
    LPSTR lpbSrc;
    int cb, cbT;
    LPDF lpdf;


    lpdf = lpetd->lpFont;

    /* Fail if this is not our font */
    if (!(lpdf->dfType & 0x080))
        return(0);

    if (lpdf->dfExtMetricsOffset)
	{
	lpbSrc = ((LPSTR)lpdf) + lpdf->dfExtMetricsOffset;

	cb = *((short far *)lpetd->lpInData);
	if (cb > sizeof(ETM))
	    cb = sizeof(ETM);

	cbT = cb;
	while (--cbT>=0)
	    *lpbOut++ = *lpbSrc++;

	return(cb);
	}
    else
	return(0);
    }


/**************************************************************
* Name: GetPairKern()
*
* Action: Get the pair kerning table from the device font structure.
*
***************************************************************
*/
short FAR PASCAL GetPairKern(lpdv, lpetd, lpbDst)
LPDV lpdv;		/* Far ptr to the device descriptor */
LPEXTTEXTDATA lpetd;	/* Far ptr to the extended text data */
LPSTR lpbDst;
    {
    LPWORD lpbSrc;
    LPWORD lpbLim;
    LPKP lpkp;
    LPFX lpfx;

	int dfHeight,temp;
	BOOL flag;

    lpfx = LockFont(lpetd->lpFont);

    /* Fail if this is not our font */
    if (!(lpfx->lpdf->dfType & 0x080))
        return(0);


	DBMSG(((LPSTR)"***GetPK():\r"));

    lpkp = lpfx->lpkp;

	dfHeight=lpfx->lpdf->dfPixHeight /* -lpfx->lpdf->dfInternalLeading */ ;
	DBMSG(((LPSTR)"dfPixHt=%d,dIntLead=%d\r",
		lpfx->lpdf->dfPixHeight,lpfx->lpdf->dfInternalLeading));


    if (lpkp){
		lpbSrc = (LPWORD) lpkp->rgPairs;
		lpbLim = (LPWORD) &lpkp->rgPairs[lpkp->cPairs];

		flag=FALSE;
		while (lpbSrc<lpbLim){
			temp=*lpbSrc++;
			if(flag){
				DBMSG(((LPSTR)"%d,",temp));
				temp=Scale(temp,dfHeight,1000);
				DBMSG(((LPSTR)"%d\r",temp));
			}else{
				char c1,c2;
				c1=(char)(temp>>8);
				c2=(char)(temp&0x00ff);
				DBMSG(((LPSTR)"pair:%c,%c ",c1,c2));
			}
			*((LPWORD)lpbDst)++ = temp;
			flag = !flag;
		}
		return(lpkp->cPairs);
	}else return(0);
}
