/*/  PHYSICAL.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	23 jan 90	peterbe		Mostly added debug messages.
//	08 dec 89	peterbe		Changed SetWidth() parameters.
//	07 dec 89	peterbe		Added debug statements.
//					Renamed SetMode() to SetWidth().
//	14 nov 89	peterbe		In XMoveTo(), ONLY change to Elite
//					font for cursor positioning if it's
//					supported by this printer.
//	30 oct 89	peterbe		Added check for error about line 55
//					(from Fralc.)
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"

#ifdef DEBUG
#include "debug.h"
#define DBGphys(msg) DBMSG(msg)
#else
#define DBGphys(msg) /* zip */
#endif

short FAR PASCAL myWrite(lpdv, lpBuf, cch)
LPDV lpdv;
LPSTR lpBuf;
short cch;
{
    register short status = 0;

    if (!lpdv->hJob  || cch <= 0)
	return status;

    if(lpdv->bFirstCharacter && lpdv->bCutSheet)
	{
	char	 buffer[80]; /* debe ser suficiente */
	short	 cch;

	if (cch = LoadString(hInst, IDS_MSG_FEED, buffer, sizeof(buffer)))
	    {
	    WriteDialog(lpdv->hJob, buffer, cch);
	    lpdv->bFirstCharacter = FALSE;
	    }
	}

    if (lpdv->oSpool + cch > CCHSPOOL)
	{
	FlushSpoolBuf(lpdv);

	// flush the buffer and write directly to the spooler
	if (cch > CCHSPOOL)
	    {
	    status = WriteSpool(lpdv->hJob, lpBuf, cch);
	    if (status == cch)
		lpdv->oSpool = 0;
	    else
		// write failed ...
		{
		DeleteJob(lpdv->hJob, 0);
		lpdv->oSpool = lpdv->hJob = 0;
		lpdv->status = status;
		}
	    return cch;
	    }
	}

    // buffer up the output
    Copy(lpdv->chSpool + lpdv->oSpool, lpBuf, cch);
    lpdv->oSpool += cch;

    return cch;
}

//----------------------------------*FlushSpoolBuf*---------------------------//
// Action:  Called at the end of the page to write out whatever is in the spool
//	    buffer.
//
// Return:  Nothing.
//
// History: Created LinS
//	    12/9/91 Copied from Unidrv.dll
//----------------------------------------------------------------------------//

short FAR PASCAL FlushSpoolBuf(lpdv)
LPDV lpdv;
{
    short status;

    // check if hJob is still valid

    if (!lpdv->hJob)
	return -1;

    if (lpdv->oSpool)
	{
	status = WriteSpool( lpdv->hJob, lpdv->chSpool, lpdv->oSpool);

	if (status != lpdv->oSpool)
	    {
	    DeleteJob(lpdv->hJob, 0);
	    lpdv->status = status;
	    lpdv->hJob = 0;
	    }
	lpdv->oSpool = 0;
	}

    return TRUE;
}



// move the cursor to the specified Y position, and update
// lpdv->sCury to reflect the change.
// This can only handle increasing Y.
// If Y changes, a CR is forced, so Y positioning must be done before
// X positioning.

FAR PASCAL YMoveTo(lpdv, y)
LPDV lpdv;
short y;
{
    short diff;

    /* Clipping */
    if(y >= lpdv->epPageHeight)
	{
	DBGphys(("YMoveTo(): Error, past end of page.\n"));
	return FALSE;
	}

    diff = y - lpdv->sCury;

    if (!diff)
	return TRUE;

    if(diff < 0)
	{ // we can't move UP the page (that's why we queue in DraftStrBlt).
	DBGphys(("YMoveTo(): Error, can't move cursor UP\n"));
	return FALSE;
	}

    // 10/19/90 need to send CR before line feeds [LinS]

    lpdv->sCury = y;
    myWrite(lpdv, EP_CR);
    lpdv->sCurx = 0;

    while(diff--)
	myWrite(lpdv, (LPSTR)"\x0a", 1);

    return TRUE;
}

// move the cursor to the specified X position, with or without
// microspace justification, as specified by MS.
// returns the difference between the current position of the cursor
// and the requested X position (cursor positioning is done by outputting
// blanks after a carriage return, usually after selecting the Elite font).

/* mueve la poscion del cursor a x, con o sin "microspace justification"
   como esta especificado por ms.
   retona la diferencia entre la actual posicion del cursor y la x
   posicion pedida */

short FAR PASCAL XMoveTo(lpdv, x, ms)
LPDV lpdv;
short x;
short ms;	    /* usar "ms justification" o no */
{
    short   curwidth;
    short   charsp;

    if (x < 0 || x > lpdv->epPageWidth) /* nunca debe pasar */
	return -1;

    if (x < lpdv->sCurx){
	myWrite(lpdv, EP_CR);
	lpdv->epCount = 0;
	lpdv->sCurx = 0;
	charsp = x;
    }else
	charsp = x - lpdv->sCurx;

    if (! charsp)
	return 0;

    if (bHasFont[1])
	// current font isn't Elite, and we support Elite, so
	// change to Elite to position characters. (14 nov 89)
	SetWidth(lpdv, FALSE, ELITE_WIDTH);
    else
	SetWidth(lpdv, FALSE, PICA_WIDTH);

    curwidth = lpdv->epXcurwidth;

    for (; charsp >= curwidth; charsp -= curwidth)
	myWrite(lpdv, (LPSTR) EP_BLANK);

    lpdv->sCurx = x - charsp;

    return charsp;
}
