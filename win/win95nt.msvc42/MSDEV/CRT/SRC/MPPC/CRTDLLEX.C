/***
*crtdllex.c
*
*   Copyright (c) 1989-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*
************************************************************************/

#include <cruntime.h>
#include <internal.h>
#include <stdlib.h>
#include <fltintrn.h>

/*
 * routine in DLL to do initialization (in this case, C++ constructors)
 */
extern void _DllExit(void);
extern void _CALLTYPE4 _initterm(PFV *, PFV *);

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

/*this globals are defined in DLL */
extern int __argc;

extern char **__argv;

/* define global destructors for C++ objects in this DLL
 * this code will be in msvcrt.lib and pulled in by linking with DLL
 * so per DLL destructor can be done in right order
 */

extern PFV *__onexitbegin;
extern PFV *__onexitend;

/***
*void mainCRTStartup(void)
*
*Purpose:
*       This routine does the C runtime initialization, calls main(), and
*       then exits.  It never returns.
*
*Entry:
*
*Exit:
*
*******************************************************************************/

void _DllMainCRTExit(
        void
        )
{

        _DllExit();

        /*
         * do _onexit/atexit() terminators
         * (if there are any) including C++ destructors
         */

        if (__onexitbegin)
                {
                        _initterm(__onexitbegin, __onexitend);

                        /*
                         * free the block holding onexit table to
                         * avoid memory leaks.  Also zero the ptr
                         * variable so that it is clearly cleaned up.
                         */

                        free ( __onexitbegin ) ;
                        __onexitbegin = NULL ;
                }

        /*
         * This will do special term for the DLL if any
         */
        _initterm(&__xp_a, &__xp_z);

        _initterm(&__xt_a, &__xt_z);

}

