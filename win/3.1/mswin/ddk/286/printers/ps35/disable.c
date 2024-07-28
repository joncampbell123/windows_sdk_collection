/**[f******************************************************************
 * disable.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * DISABLE.C
 *
 * 12Aug87 sjp	Creation date: moved stuff from RESET.C
 *********************************************************************/

/* Copyright 1987, Aldus Corp. */

#include "pscript.h"
#include "driver.h"
#include "etm.h"
#include "fonts.h"
#include "profile.h"
#include "enum.h"


/**************************************************************
* Name: Disable()
*
* Action: This routine is called to close down the device operation
*	  when it is no longer needed.	It should free up any
*	  allocated memory and return the device to the inital state,
*	  etc.
*
****************************************************************/

int FAR PASCAL Disable(lpdv)
LPDV lpdv;	    /* ptr to the device descriptor */
{
    /* Throw out substitute table */
	FreeSubTable(lpdv);

    /* Throw out atom table (if it exists) */
    FreeTTFontTable(lpdv);

    /*  kill the string list if any exists  */
    if(lpdv->slTTFonts)
        DeleteStringList(lpdv->slTTFonts);

	DeleteFontDir(lpdv->iPrinter);
	return TRUE;
}
