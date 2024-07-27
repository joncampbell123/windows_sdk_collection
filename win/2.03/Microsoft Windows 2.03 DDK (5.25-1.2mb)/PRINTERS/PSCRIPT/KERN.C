/*********************************************************************
 * KERN.C
 *
 * 20Aug87	sjp		Creation:  from EXTTEXT.C.
 *********************************************************************
 */

#include "pscript.h"

int FAR PASCAL PairKern(int, LPKP, int, int);


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


