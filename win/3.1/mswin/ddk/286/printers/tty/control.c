/*/  CONTROL.C for TTY
 +-----------------------------------------+
 | Fralc Consultores (c), Mexico 1986-1989 |
 +-----------------------------------------+
/*/

// ------------------------------------------------------------------------
// Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
// ------------------------------------------------------------------------
//	Microsoft History (latest first)
//	23 jan 90	peterbe		Fix abort for NEWFRAME case.  Call
//					DeleteJob() and clear status, hJob
//					when WriteSpool() returns error.
//	27 dec 89	peterbe		Remove InitQueue() call, change
//					DeleteQueue() calls to TextDump().
//	14 dec 89	peterbe		Call EndSpoolPage() in ENDDOC.
//					Change various bogus tests on
//					spooler function return values.
//					Translate a few more comments.
//	07 dec 89	peterbe		Use DeleteQueue() call now in
//					ABORTDOC.
//	05 dec 89	peterbe		Added calls to priority queue
//					routines.  Added debug #ifdef's.
//	20 nov 89	peterbe		Mainly, use SetDefaultWidth() to
//					output escape for default width.
//					In draft mode, only set Pica if we
//					have it. We use bHasFont[].
//	30 oct 89	peterbe		Add 14 oct updates from Fralc.
//	20 oct 89	peterbe		Checked in.
// ------------------------------------------------------------------------

#include "generic.h"
#include <stdio.h>
#include <bios.h>


#ifdef DEBUG
#include "debug.h"
#define DBGctl(msg) DBMSG(msg)
#else
#define DBGctl(msg) /* zip */
#endif

extern	BOOL bHasFont[];		// TRUE if font [i] is supported.

void FAR PASCAL FillBuffer(LPSTR, WORD, WORD);

// forward

FAR PASCAL Control(
    LPDV lpdv,	// Apuntador largo al a estruc. PDEVICE
    short   function, 	// SubFunciones de Control() 
    LPSTR   lpInData, 	// Apuntador largo a los datos de entrada de la subfunc.
    LPSTR   lpOutData)	// Apuntador largo a los datos de salida de la subfunc.
{
    short y;
    unsigned i;

    switch (function){
	case NEXTBAND:

	    DBGctl(("NEXTBAND\n"));
	    if (!lpdv->hJob)
		return lpdv->status;

	    if (lpdv->nBand)
		{
		lpdv->nBand = 0;
		DoMaxBand(lpdv);
		SetRectEmpty((LPRECT) lpOutData);
		DoFirstBand(lpdv);
		}
	    else
		{
		lpdv->nBand = 1;
		// nothing to do, all the initialization are either
		// done during StartDoc or after the MaxBand.
		SetRect((LPRECT) lpOutData, 0, 0, lpdv->epPageWidth,
			lpdv->epPageHeight);
		}

	    return lpdv->status;

	case NEWFRAME:
	    DBGctl(("NEWFRAME\n"));
	    DoMaxBand(lpdv);
	    // reset in preparation for the next page;
	    DoFirstBand(lpdv);
	    return lpdv->status;

	case SETABORTPROC:
	    DBGctl(("SETABORTPROC\n"));
	    lpdv->hDC = *(HANDLE far *)lpInData;
	    break;

	case DRAFTMODE:
	    DBGctl(("DRAFTMODE\n"));
	    /* draft mode no es soportado en landscape */
	    /* este if no vale. no usamos landscape */
	    // draft mode isn't supported in landscape: so
	    // this 'if' is useless, we don't use landscape.

	    return !(*(short far *)lpInData);

	case STARTDOC:
	    DBGctl(("STARTDOC\n"));
	    if (!lpdv->hJob)
		{
		// take output port name from lpOutData ...
		if (lpOutData)
		    lpOutData = ((LPDOCINFO)lpOutData)->lpszOutput;

		if (!lpOutData)
		    lpOutData = lpdv->epPort;

		lpdv->hJob = OpenJob((LPSTR)lpOutData, lpInData, lpdv->hDC);
		}

	    // check for error conditions
	    if (lpdv->hJob & 0x8000)
		{
		lpdv->status = lpdv->hJob;
		lpdv->hJob = 0;
		}
	    else
		{
		lpdv->status = StartSpoolPage(lpdv->hJob);
		DoFirstBand(lpdv);
		}

	    return lpdv->status;

	case ABORTDOC:
	    DBGctl(("ABORTDOC\n"));
	    if (lpdv->hJob)
		DeleteJob(lpdv->hJob, 0);
	    lpdv->hJob = 0;
	    break;

	case ENDDOC:
	    DBGctl(("ENDDOC\n"));
	    /* cerrar el trabajo */
	    // close the job
	    if (lpdv->hJob)
		{
		lpdv->status = EndSpoolPage(lpdv->hJob);
		CloseJob(lpdv->hJob);
		lpdv->hJob = 0;
		}
	    break;

	case QUERYESCSUPPORT:
	    DBGctl(("QUERYESCSUPPORT\n"));
	    i = * (short far *) lpInData;
	    switch(i){
		case DRAFTMODE:
		case NEWFRAME:	      /* requeridos */
		case SETABORTPROC:    /* requeridos */
		case ABORTDOC:	      /* requeridos */
		case NEXTBAND:	      /* requeridos */
		case QUERYESCSUPPORT: /* requeridos */
		case STARTDOC:	      /* requeridos */
		case ENDDOC:	      /* requeridos */
		case GETPHYSPAGESIZE:
		case GETPRINTINGOFFSET:
		    return TRUE;
		default:
		    return FALSE;
	    }

	case GETPHYSPAGESIZE:
	    DBGctl(("GETPHYSPAGESIZE\n"));
	    ((LPPOINT)lpOutData)->x = lpdv->pf->XPhys;
	    ((LPPOINT)lpOutData)->y = lpdv->pf->YPhys;
	    break;

	case GETPRINTINGOFFSET:
	    DBGctl(("GETPRINTINGOFFSET\n"));
	    ((LPPOINT)lpOutData)->x = 0;  // assume widest possible printable region
	    ((LPPOINT)lpOutData)->y = 0;  // user can crop by setting margins in WordPro.
	    break;

	default:
	    return FALSE;
    }
    return TRUE;

}	// end Control()


// Set width to default.
// if we have 12 cpi font, that's it.
// otherwise we use 10 or 17.

short	NEAR PASCAL doFirstBand(lpdv)
LPDV	lpdv;
{
    lpdv->epCount = 0;
    lpdv->oSpool = 0;
    lpdv->sCurx = 0;   // 30 oct 89
    lpdv->epXCursPos = lpdv->sCury = 0;

    lpdv->bFirstCharacter = TRUE;
    myWrite(lpdv, ESCEXP(lpdv->escapecode.reset));

    // center the printing area along the length of the paper.
    /* centrar el area imprimible a la longitud de el papel */

    YMoveTo(lpdv, lpdv->pf->VOffset);
}

short	NEAR PASCAL doMaxBand(lpdv)
LPDV	lpdv;
{
    TextDump(lpdv, TRUE);

    if(lpdv->bPageBreak || lpdv->bCutSheet)
	{
	myWrite(lpdv, (LPSTR)EP_CR);
	myWrite(lpdv, (LPSTR)EP_FF);
	}
    else
	YMoveTo(lpdv, lpdv->pf->YPhys);

    // This is the first character of the following page.
    if (lpdv->hJob)
	{
	FlushSpoolBuf(lpdv);
	lpdv->status = EndSpoolPage(lpdv->hJob);
	lpdv->status = StartSpoolPage(lpdv->hJob);
	}

    lpdv->epHPptr = 0;
}
