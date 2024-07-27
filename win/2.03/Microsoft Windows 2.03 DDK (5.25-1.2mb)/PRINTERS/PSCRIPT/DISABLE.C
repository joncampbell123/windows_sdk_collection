/*********************************************************************
 * DISABLE.C
 *
 * 12Aug87	sjp		Creation date: moved stuff from RESET.C
 *********************************************************************
 */

/* Copyright 1987, Aldus Corp. */

#include "pscript.h"
#include "printers.h"

extern int rgDirLink[];
extern HANDLE rghFontDir[];


/*******************************************************************
 * Name: DeleteFontDir()
 *
 * Action: Delete a font directory and free its memory if this is
 *	  the last reference to it.
 *
 ********************************************************************
 */
void FAR PASCAL DeleteFontDir(iPrinter)
	int iPrinter;
{
    if (iPrinter<CPRINTERS && rghFontDir[iPrinter]){
		if (--rgDirLink[iPrinter] <= 0){
		    GlobalFree(rghFontDir[iPrinter]);
	    	rgDirLink[iPrinter] = 0;
	    	rghFontDir[iPrinter] = NULL;
	    }
	}
}


/**************************************************************
* Name: Disable()
*
* Action: This routine is called to close down the device operation
*	  when it is no longer needed.	It should free up any
*	  allocated memory and return the device to the inital state,
*	  etc.
*
****************************************************************
*/
int FAR PASCAL Disable(lpdv)
	DV FAR *lpdv;	    /* Far ptr to the device-specific device descriptor*/
{
    DeleteFontDir(lpdv->dh.iPrinter);
    return(TRUE);
}

