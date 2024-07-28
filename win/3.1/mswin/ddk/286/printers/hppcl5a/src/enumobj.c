/**[f******************************************************************
* enumobj.c -
*
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
*             All rights reserved.
*
**f]*****************************************************************/
  
/**********************************************************************
*
*  2 Oct 89    clarkc  Wrote it.  Removed from stubs.c (rewritten from
*          scratch)
*/
  
#include "generic.h"
#include "enumobj.h"
  
int far pascal EnumObj(lpdv, iStyle, lpFn, lpData)
LPDEVICE lpdv;     /* Far ptr to the device descriptor */
int iStyle;        /* The style (brush or pen) */
FARPROC lpFn;      /* Far ptr to the callback function */
LPSTR lpData;      /* Far ptr to the client data (passed to callback) */
{
    static DWORD rgrgb[MAXCOLORS] = {RGB_BLACK, RGB_WHITE};
  
    short iRetVal;     /* Set to 0 until final item is enumerated */
    LOGPEN lopn;       /* The logical pen */
    LOGBRUSH lb;       /* The logical brush */
    int i;             /* Counter in for loop */
#ifdef DEBUG_FUNCT
    DB(("Entering EnumObj\n"));
#endif
  
    iRetVal = FALSE;
    switch(iStyle)
    {
        case OBJ_PEN:
            lopn.lopnWidth.xcoord = 0;
            lopn.lopnWidth.ycoord = 0;
  
            /* Enumerate each pen style for all possible pen colors */
            for (lopn.lopnStyle = 0; lopn.lopnStyle < PS_NULL; ++lopn.lopnStyle)
            {
                for (i = 0; i < MAXCOLORS; ++i)
                {
                    lopn.lopnColor = rgrgb[i];
                    if (!(*lpFn)((LOGPEN far *) &lopn, lpData))
                        goto DONE;
                }
            }
            /* Enumerate the NULL pen last.  Since it has no color, it must be outside */
            /* the for loop.  Do it last because it facilitates the fall thru to the   */
            /* return statement.                                                       */
            lopn.lopnStyle = PS_NULL;
            iRetVal = (*lpFn)((LOGPEN far *) &lopn, lpData);
            break;
  
        case OBJ_BRUSH:
  
            /* Enumerate all the possible colors for a solid brush */
            lb.lbStyle = BS_SOLID;
            for (i = 0; i < MAXCOLORS; ++i)
            {
                lb.lbColor = rgrgb[i];
                if (!(*lpFn)((LOGBRUSH far *) &lb, lpData))
                    goto DONE;
            }
  
            /* Enumerate all possible hatch brushes for black on white */
            /* Added White on Black, only hours after initial checkin  */
            lb.lbStyle = BS_HATCHED;
            for (i = 0; i < MAXCOLORS; ++i)
            {
                lb.lbColor = rgrgb[i];
                for (lb.lbHatch = 0; lb.lbHatch < MAXHATCHSTYLE; ++lb.lbHatch)
                    if (!(*lpFn)((LOGBRUSH far *) &lb, lpData))
                        goto DONE;
            }
  
            /* Enumerate the HOLLOW brush last.  Since it has no color, it must be    */
            /* outside the for loop.  Do it last because it facilitates the fall thru */
            /* to the return statement.                                               */
            lb.lbStyle = BS_HOLLOW;
            lb.lbColor = RGB_WHITE;
            lb.lbHatch = 0;
            iRetVal = (*lpFn)((LOGBRUSH far *) &lb, lpData);
            break;
    }
  
    DONE:
#ifdef DEBUG_FUNCT
    DB(("Exiting EnumObj\n"));
#endif
    return(iRetVal);
}
  
