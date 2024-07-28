/**[f*****************************************************************
 * utils.c -
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 *
 *
 *
 * 10/26/88 - David Rogers - modified BinaryPort to return true based on
 *            BinaryImage flag in win.ini for each port
 *
 **f]*****************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "channel.h"
#include "resource.h"
#include "utils.h"
#include "debug.h"
#include "psdata.h"
#include "atstuff.h"
#include "driver.h"


#define USA_COUNTRYCODE		1
#define FRCAN_COUNTRYCODE	2


#define SIZE 40

extern PSDEVMODE CurEnv;

/**************************************************
* Name: lmul()
*
* Action: long multiply.  This is only an auxilary
*	  routine used to access the long arithmetic
*	  library functions as a FAR procedure.
*
***************************************************/
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
    LPSTR lpEndS2, lpTmp;
    int nChars;
    char chTmp;

    /* compute # chars that can be copied from s2 */
    nChars = s1size - lstrlen(s1) - 1;
    lpEndS2 = s2 + nChars;
    for (lpTmp = s2; *lpTmp && lpTmp < lpEndS2; lpTmp = AnsiNext(lpTmp))
        ;
    if (lpTmp > lpEndS2)
        lpTmp -= 2;

    /* truncate s2 and concatenate it to s1 */
    chTmp = *lpTmp;
    *lpTmp = '\0';
    lstrcat(s1, s2);
    *lpTmp = chTmp;
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
    return(iCountry==USA_COUNTRYCODE || iCountry==FRCAN_COUNTRYCODE);
    }


/*******************************************************************
* Name: ClipBox()
*
* Action: Send the necessary PostScript commands to clip to a rectangular
*	  region or restore the clip region.  If the pointer to the
*	  clip rectangle is NULL, then PostScript's clip region is reset.
*
********************************************************************/

void FAR PASCAL ClipBox(lpdv, lprc)
LPDV	lpdv;	    /* Far ptr to the device descriptor */
LPRECT	lprc;
{
	if (lprc) {
		if (lprc->left>0 || lprc->top>0 ||
		    lprc->right<lpdv->paper.cxPage ||
		    lprc->bottom<lpdv->paper.cyPage)
		{
			/* gs = gsave, CB == clip box */
			PrintChannel(lpdv, "gs %d %d %d %d CB\n",
				lprc->right - lprc->left,	/* dx */
				lprc->bottom - lprc->top,	/* dy */
				lprc->left, lprc->top);		/* x, y */
			lpdv->fIsClipped = TRUE;
		}
	} else if (lpdv->fIsClipped) {
		/* gr == grestore */
		PrintChannel(lpdv, "gr\n");
		lpdv->fIsClipped = FALSE;
	}
}

#if 0	/* these are no longer used.  see fonts.c and enum.c for replacements */


/*********************************************************************
* Name: lsfpfmcopy()            *** rb BitStream ***
*
* Action: Copies softfont PFM path from softfonts path(s) string
*         Copies from begin of source until comma or null
*         Destination is terminated with a null
*
**********************************************************************
*/
void FAR PASCAL lsfpfmcopy(lszDst, lszSrc)
	LPSTR lszDst;       /* Ptr to the destination area */
	LPSTR lszSrc;       /* Ptr to the source string */
{
	while ((*lszSrc != '\0') & (*lszSrc != ',')) {
		*lszDst++ = *lszSrc++;
	}
	*lszDst = 0;
}


/*********************************************************************
* Name: lsfloadpathcopy()       *** rb BitStream ***
*
* Action: Copies softfont OUTLINE or BITMAP path from softfonts path(s)
*	  string
*	  Copies from 1st non_blank char after comma until null.
*         Destination is terminated with a null
*         If no comma, return null string.
**********************************************************************
*/
void FAR PASCAL lsfloadpathcopy(lszDst, lszSrc)
	LPSTR lszDst;       /* Ptr to the destination area */
	LPSTR lszSrc;       /* Ptr to the source string */
{
	while ((*lszSrc != '\0') & (*lszSrc != ',')) {
		++lszSrc;
	}

	if (*lszSrc == 0)
		*lszDst = 0;
	else{
		++lszSrc;
		while (*lszDst++ = *lszSrc++);
	}
}

/*********************************************************************
* Name: lbitmapext()       *** rb BitStream ***
*
* Action: Inserts bitmap extension (.PSB) in passed LPSTR if . is found
*         Inserts nothing if period is not found
**********************************************************************
*/
void FAR PASCAL lbitmapext(lszSrc)
	LPSTR lszSrc;       /* Ptr to the source string */
{
	while ((*lszSrc != '\0') & (*lszSrc != '.')){
		++lszSrc;
	}

	if (*lszSrc != 0){
		++lszSrc;
		*lszSrc = 'p';
		++lszSrc;
		*lszSrc = 's';
		++lszSrc;
		*lszSrc = 'b';
	}
}

#endif

/**************************************************************
* Name: SetKey()
*
* Action: This function forms a key for indexing into the
*	  win.ini file to retrieve information about a
*	  PostScript printer on a specific port.
*
*    The section name is just the device name,lpFile
*
* In:
*	lpKey	put the key here
*   lpDevName Nickname of printer as defined in PPD file
*	lpFile	output file used to make key 
*
* Output:
*	lpKey is returned
*
**************************************************************/


LPSTR FAR PASCAL SetKey(LPSTR lpKey, LPSTR  lpDevName, LPSTR lpFile)
{
    WORD  len;

	lstrcpy(lpKey, lpDevName);
    lstrcat(lpKey, ",");
    lstrcat(lpKey, lpFile);
    len = lstrlen(lpKey) - 1;
    if(lpKey[len] == ':')
        lpKey[len] = '\0';  //  remove terminating : if any.

	return lpKey;
}



#if  0

void FAR PASCAL GetProfileStringFromResource(idsAppl,keyMode,idsKey,
idsDef,returnName,sizeReturnName)
	short idsAppl;
	short keyMode;
	short idsKey;
	short idsDef;
	LPSTR returnName;
	short sizeReturnName;
{
	char szKey[60];
	char applName[SIZE];
	char keyName[SIZE];
	char defName[SIZE];
	LPSTR lpApplName;

	LoadString(ghInst, idsAppl,applName,sizeof(applName));
	LoadString(ghInst, idsKey, keyName, sizeof(keyName));
	LoadString(ghInst, idsDef, defName, sizeof(defName));

	if (keyMode) {
		lpApplName = SetKey(szKey, applName);
	} else
		lpApplName = applName;

	GetProfileString(lpApplName, keyName, defName, returnName, sizeReturnName);

	DBMSG(("*GetProfileStringFromResource(): %d,%ls,%ls,%ls,%ls\n",
		keyMode, (LPSTR)lpApplName, (LPSTR)keyName, (LPSTR)defName, returnName));
}


#endif
