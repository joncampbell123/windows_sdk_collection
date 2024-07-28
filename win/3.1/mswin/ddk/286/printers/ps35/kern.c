/**[f******************************************************************
 * kern.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * KERN.C
 *
 * 20Aug87	sjp		Creation:  from EXTTEXT.C.
 *********************************************************************
 */

#include "pscript.h"
#include "etm.h"
#include "utils.h"
#include "kern.h"


#pragma alloc_text(_TEXT, KernAmount)	/* for long math */

/****************************************************************
* Name: KernAmount()
*
* Action: Calculate the amount to kern a string in hundreths of a point.
*
* Note: This routine is defined here because it references the
*	long arithmetic library.
*
******************************************************************/
int FAR PASCAL KernAmount(k0, k1, p0, p1, x, cb)
int k0, k1;	/* The minimum, maximum kern amount */
int p0, p1;	/* The minimum, maximum point size */
int x;		/* The point size of the font being kerned */
int cb; 	/* The number of bytes to kern */
{
    int iKernAmount;

    long lk0, lk1;
    long lp0, lp1;
    long lx, lcb;
    long lnum, ldiv;


    if (x<p0)
		iKernAmount = cb * k0;
    else if (x > p1)
		iKernAmount = cb * k1;
    else{
		lk0 = k0;  lk1 = k1;
		lp0 = p0;  lp1 = p1;
		lx = x;    lcb = cb;

		lnum = ((lk1 - lk0) * lx  + (lk0 * lp1 - lk1 * lp0)) * lcb;
		ldiv = lp1 - lp0;
		if (lnum<0)
		    lnum -= ldiv/2;
		else
		    lnum += ldiv/2;
		iKernAmount = (int)(lnum/ldiv);
	}
    return(iKernAmount);
}




/**********************************************************************
* Name: PairKern()
*
* Action: Compute the amount to kern between two characters.
*
***********************************************************************
*/
int FAR PASCAL PairKern(iRes, lpkp, iCh1, iCh2)
int iRes;	    /* The device resolution */
LPKP lpkp;	    /* Far ptr to the pair kerning table */
int iCh1;	    /* The first character of the kerning pair */
int iCh2;	    /* The second character of the kerning pair */
{
    int iMax;
    int iMin;
    int i;
    int iKey;
    int iKernAmount;

    iKey = (iCh2<<8) | (iCh1 & 0x0ff);

    iMin = 0;
    iMax = lpkp->cPairs;
    i = iMax/2;
    while (iMax > iMin){
		if (iKey > lpkp->rgPairs[i].iKey)
		    iMin = i + 1;
		else if (iKey < lpkp->rgPairs[i].iKey)
		    iMax = i - 1;
		else
		    break;
		i = iMin + (iMax - iMin)/2;
	};

    if (lpkp->rgPairs[i].iKey==iKey)
		iKernAmount = Scale(lpkp->rgPairs[i].iKernAmount, iRes, 7200);
    else
		iKernAmount = 0;
    return(iKernAmount);
}


