/*
**
*
* REVERSE.C - Sample API routine.
*
* Copyright (c) 1989-1993 Microsoft Corporation as an unpublished
* licensed proprietary work.  All rights reserved.
*
* Description: Contains one external routine REVERSE(<expC>).
*   Returns <expC> reversed.
*
**
*/
#include <pro_ext.h>



void FAR reverse(ParamBlk FAR *parm)
{
        int i;
        MHANDLE mh_out;
        char FAR *  in_string;
        char FAR * out_string;


    // Check to see if we can allocate the memory needed

    if ((mh_out = _AllocHand(parm->p[0].val.ev_length+1)) == 0)
        _Error(182);             /* "Insufficient memory." */

    /*  Since this routine does not call any functions which cause memory
        reorganization, it is not necessary to _HLock the handles prior to
        dereferencing them (_HandToPtr).                                */

    in_string = _HandToPtr(parm->p[0].val.ev_handle);
    out_string = (char FAR *) _HandToPtr(mh_out) + parm->p[0].val.ev_length;

    *(out_string--) = '\0';         /* _RetChar() needs null terminated string */

    for (i = 0; i < parm->p[0].val.ev_length; i++)
        *(out_string--) = *(in_string++);

    _HLock(mh_out);                 /* Lock MHANDLE during callback. */
    _RetChar(out_string+1);

    _FreeHand(mh_out);              /* Free MHANDLEs we allocate, but not handles
                                       passed in ParamBlk. */
}



FoxInfo myFoxInfo[] =
{
	{"REVERSE", (FPFI) reverse, 1, "C"},
};

FoxTable _FoxTable =
{
    (FoxTable FAR *)0, sizeof(myFoxInfo) / sizeof(FoxInfo), myFoxInfo
};
