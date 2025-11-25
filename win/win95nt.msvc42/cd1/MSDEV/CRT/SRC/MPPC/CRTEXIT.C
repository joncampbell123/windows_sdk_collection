/***
*crtexit.c
*
*   Copyright (c) 1989-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
************************************************************************/
#include <cruntime.h>
#include <msdos.h>
#include <stdio.h>
#include <stdlib.h>
#include <internal.h>
#include <fltintrn.h>

/*
 * pointers to initialization functions
 */

extern PFV __xi_a ;

extern PFV __xi_z ;

extern PFV __xc_a ;  /* C++ initializers */

extern PFV __xc_z ;

extern PFV __xp_a ;  /* C pre-terminators */

extern PFV __xp_z ;

extern PFV __xt_a ;   /* C terminators */

extern PFV __xt_z ;

extern PFV *__onexitbegin;
extern PFV *__onexitend;

/* worker routine prototype */
void _CALLTYPE4 doexit (int code, int quick, int retcaller);
extern void __cdecl _DoExitSpecial(int, int, PFV *, PFV *, PFV *, PFV *, PFV *, PFV *);

void _CALLTYPE1 exit (
        int code
        )
{

        /*
         * This will do special term for the App in the right order
         */
        _DoExitSpecial(code, 0, &__xp_a, &__xp_z, &__xt_a, &__xt_z, __onexitbegin, __onexitend );
}

void _CALLTYPE1 _exit (
        int code
        )
{
        /*
         * This will do special term for the App in the right order
         */
        _DoExitSpecial(code, 0, &__xp_a, &__xp_z, &__xt_a, &__xt_z, __onexitbegin, __onexitend );
}
