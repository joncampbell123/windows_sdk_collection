#include "pscript.h"

#define USA_COUNTRYCODE 1


long FAR PASCAL lmul(long, long);
long FAR PASCAL ldiv(long, long);
long FAR PASCAL lmod(long, long);


/**************************************************
* Name: lmul()
*
* Action: long multiply.  This is only an auxilary
*	  routine used to access the long arithmetic
*	  library functions as a FAR procedure.
*
***************************************************
*/
long FAR PASCAL lmul(lval1, lval2)
long lval1;
long lval2;
    {
    return(lval1 * lval2);
    }


/**************************************************
* Name: ldiv()
*
* Action: long divide.	This is only an auxilary
*	  routine used to access the long arithmetic
*	  library functions as a FAR procedure.
*
***************************************************
*/
long FAR PASCAL ldiv(lval1, lval2)
long lval1;
long lval2;
    {
    return(lval1/lval2);
    }


/**************************************************
* Name: lmod()
*
* Action: long modulo divide.	This is only an auxilary
*	  routine used to access the long arithmetic
*	  library functions as a FAR procedure.
*
***************************************************
*/
long FAR PASCAL lmod(lval1, lval2)
long lval1;
long lval2;
    {
    return(lval1%lval2);
    }


/*****************************************************
* Name: lstrlen()
*
* Action: Compute the length of a null terminated string.
*	  Note that the byte count excludes the null terminator.
*
******************************************************
*/
int FAR PASCAL lstrlen(lsz)
LPSTR lsz;
    {
    int cb;

    for (cb=0; *lsz++; ++cb)
	;
    return(cb);
    }


/***********************************************************
* Name: lstrcmp()
*
* Action: Compare two null terminated strings.
*
* Returns: TRUE if the two strings are equal.
*	   FALSE if the two strings are different.
*
************************************************************
*/
BOOL FAR PASCAL lstrcmp(lsz1, lsz2)
	LPSTR lsz1;
	LPSTR lsz2;
{
    char bCh;

    do{
		if ((bCh = *lsz1++) != *lsz2++)
			return(TRUE);
	} while (bCh);
    return(FALSE);
}


/*********************************************************************
* Name: lstrcpy()
*
* Action: Copy a null terminated string.
*
**********************************************************************
*/
void FAR PASCAL lstrcpy(lszDst, lszSrc)
LPSTR lszDst;	    /* Ptr to the destination area */
LPSTR lszSrc;	    /* Ptr to the source string */
    {
    if (lszSrc)
	while (*lszDst++ = *lszSrc++)
	    ;
    else
	*lszDst = 0;
    }


/******************************************************************
* Name: lmemcpy()
*
* Action: Copy an array of bytes.
*
********************************************************************
*/
void FAR PASCAL lmemcpy(lpbDst, lpbSrc, cb)
	LPSTR lpbSrc;	    /* Ptr to the source bytes */
	LPSTR lpbDst;	    /* Ptr to the destination area */
	int cb;		 	    /* The byte count */
{
    while (--cb>=0)
		*lpbDst++ = *lpbSrc++;
}


/********************************************************************
* Name: lmemIsEqual()
*
* Action: Compare two byte arrays for equality.
*
* Returns: TRUE if the two arrays are equal.
*	   FALSE if the two arrays are different.
*
*********************************************************************
*/
BOOL FAR PASCAL lmemIsEqual(lpb1, lpb2, cb)
LPSTR lpb1;	    /* Ptr to the first array */
LPSTR lpb2;	    /* Ptr to the second array */
int cb; 	    /* The size of the arrays */
    {
    while (--cb>=0)
	if (*lpb1++ != *lpb2++)
	    return(FALSE);
    return(TRUE);
    }


/********************************************************************/
/* concatenate string s2 onto s1 up to a max of s1size, or len(s1)
 * if s1size > len(s1)
 *
 */
void FAR PASCAL lstrncat(s1,s2,s1size)
	LPSTR s1;
	LPSTR s2;
	int s1size;
{
	int i,j;

	i=lstrlen(s1);
	j=0;

	while( i<s1size && (s1[i++]=s2[j++]) != '\0' );
	s1[s1size-1]='\0';	/* just to be sure */
}


/****************************************************************
* Name: KernAmount()
*
* Action: Calculate the amount to kern a string in hundreths of a point.
*
* Note: This routine is defined here because it references the
*	long arithmetic library.
*
*****************************************************************
*/
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


/********************************************************
* Name: isUSA()
*
* Action: This function reads win.ini to determine the
*	  county setting and returns TRUE if it is the
*	  USA.
*
*********************************************************
*/
BOOL FAR PASCAL isUSA()
    {
    int iCountry;

    iCountry = GetProfileInt((LPSTR) "intl", (LPSTR)"icountry", USA_COUNTRYCODE);
    return(iCountry==USA_COUNTRYCODE);
    }


/***************************************************************
* Name: Lightness()
*
* Action: Compute the lightness factor of an rgb color using the
*	  RGB to HLS method.  In windows world, this is just the
*	  average of the three primary color components. Note that the
*	  standard RGB to HLS algorithm takes the average of the
*	  brightest and darkest color components.
*
* Returns: An intensity such that 0 <= i <= 1000
*
****************************************************************
*/
int FAR PASCAL Lightness(rgb)
RGB rgb;	/* The rgb color */
    {
    int iVal;

    iVal = (int)( (rgb & 0x0ffL) + ((rgb >> 8) & 0x0ffL) + ((rgb >> 16) & 0x0ffL) );
    iVal = (int)( (((long) iVal) * 1000L) / (long)(3 * 255) );

    return(iVal);
    }


/*******************************************************************
* Name: ClipBox()
*
* Action: Send the necessary PostScript commands to clip to a rectangular
*	  region or restore the clip region.  If the pointer to the
*	  clip rectangle is NULL, then PostScript's clip region is reset.
*
***********************************************************************
*/
void FAR PASCAL ClipBox(lpdv, lprc)
DV FAR *lpdv;	    /* Far ptr to the device descriptor */
RECT FAR *lprc;
{
    if (lprc){
		if (lprc->left>0 || lprc->top>0 ||
		    lprc->right<lpdv->dh.paper.cxPage ||
		    lprc->bottom<lpdv->dh.paper.cyPage
		){
		    PrintChannel(lpdv, (LPSTR) "gsave %d %d %d %d ClipBox\n",
			    lprc->left, lprc->top, lprc->right, lprc->bottom);
		    lpdv->dh.fIsClipped = TRUE;
	    }
	}else if (lpdv->dh.fIsClipped){
		PrintChannel(lpdv, (LPSTR) "grestore\n");
		lpdv->dh.fIsClipped = FALSE;
	}
}

