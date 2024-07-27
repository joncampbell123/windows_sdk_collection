/*********************************************************************
 * FONTDIR.C
 *
 * 20Aug87	sjp		Creation: from RESET.C.
 *********************************************************************
 */

#include "pscript.h"
#include "printers.h"

extern HANDLE rghFontDir[];

/********************************************************
 * Name: LockFontDir()
 *
 * Action: Lock the specified font directory and return a pointer to it.
 *
 * Returns: A pointer to the locked font directory.
 *
 **********************************************************
 */
LPSTR FAR PASCAL LockFontDir(iPrinter)
	int iPrinter;	/* The printer who's font dir should be locked */
{
    LPSTR lpb;

    if (iPrinter<CPRINTERS && rghFontDir[iPrinter])
		lpb = GlobalLock(rghFontDir[iPrinter]);
    else
		lpb = (LPSTR) (long) NULL;
    return(lpb);
}


/**************************************************************
 * Name: UnlockFontDir()
 *
 * Action: This routine unlocks the specified font directory.
 *
 ***************************************************************
 */
void FAR PASCAL UnlockFontDir(iPrinter)
	int iPrinter;	/* The printer who's font dirctory should be unlocked */
{
    if (iPrinter<CPRINTERS && rghFontDir[iPrinter])
		GlobalUnlock(rghFontDir[iPrinter]);
}


/*******************************************************************
* Name: LockFont()
*
* Action: This routine takes a pointer to a device font and
*	  returns a pointer to the extended font info structure.
*	  It also converts several offsets in the device font
*	  to pointers.
*
********************************************************************
*/
LPFX FAR PASCAL LockFont(lpdf)
LPDF lpdf;
    {
    LPFX lpfx;

    if (lpdf->dfDriverInfo)
	{
	lpfx = (LPFX) (((LPSTR) lpdf) + lpdf->dfDriverInfo);
	lpfx->lpdf = lpdf;
	lpfx->lszFont = ((LPSTR)lpdf) + lpfx->dfFont;
	lpfx->lszFace = ((LPSTR)lpdf) + lpdf->dfFace;
	lpfx->lszDevice = ((LPSTR)lpdf) + lpdf->dfDevice;
	lpfx->rgWidths = (short far *) (((LPSTR)lpdf) + lpdf->dfExtentTable);
	if (lpdf->dfTrackKernTable)
	    lpfx->lpkt = (LPKT)  (((LPSTR)lpdf) + lpdf->dfTrackKernTable);
	else
	    lpfx->lpkt = (LPKT)(long)NULL;
	if (lpdf->dfPairKernTable)
	    lpfx->lpkp = (LPKP) (((LPSTR)lpdf) + lpdf->dfPairKernTable);
	else
	    lpfx->lpkp = (LPKP)(long)NULL;
	}
    else
	lpfx = (LPSTR) (long) NULL;

    return(lpfx);
    }


