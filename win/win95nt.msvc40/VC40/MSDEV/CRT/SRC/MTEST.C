/***
* mtest.c - Multi-thread debug testing module
*
*       Copyright (c) 19xx-1988, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       This source contains a group of routines used for multi-thread
*       testing.  In order to use the debug flavor of these routines, you
*       MUST link special debug versions of multi-thread crt0dat.obj and
*       mlock.obj into your program.
*
*       [NOTE:  This source module is NOT included in the C runtime library;
*       it is used only for testing and must be explicitly linked into the
*       test program.]
*
*******************************************************************************/

#ifdef _M_IX86
#ifdef STACKALLOC
#error Can't define STACKALLOC in 386 mode
#endif  /* STACKALLOC */
#endif  /* _M_IX86 */

#ifdef _M_IX86
#ifdef _DOSCREATETHREAD_
#error Currently can't define _DOSCREATETHREAD_ in 386 mode
#endif  /* _DOSCREATETHREAD_ */
#endif  /* _M_IX86 */

#ifdef _DOSCREATETHREAD_
#ifndef STACKALLOC
#error Can't define _DOSCREATETHREAD_ without STACKALLOC
#endif  /* STACKALLOC */
#endif  /* _DOSCREATETHREAD_ */

/*
Multi-thread core tester module.
*/
#include <malloc.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <io.h>
#include <mtest.h>

/* Define FAR to be blank for the 386 and far otherwise. */

#undef  FAR
#ifdef _M_IX86
#define FAR
#else  /* _M_IX86 */
#define FAR     far
#endif  /* _M_IX86 */

/* define stack size */
#ifdef _M_IX86
#define _STACKSIZE_ 8192
#else  /* _M_IX86 */
#define _STACKSIZE_ 2048
#endif  /* _M_IX86 */


/* routines */
#ifdef _M_IX86
unsigned _syscall DOSSLEEP (unsigned long) ;
#else  /* _M_IX86 */
unsigned FAR pascal DOSSLEEP (unsigned long) ;
#endif  /* _M_IX86 */
int main ( int argc , char * * argv ) ;
int minit(void);
void childcode ( void FAR * arg ) ;
#ifdef _DOSCREATETHREAD_
#ifndef _M_IX86
void childcode ( void ) ;
unsigned FAR pascal DOSCREATETHREAD (void FAR *, void FAR *, void FAR *);
#endif  /* _M_IX86 */
#else  /* _DOSCREATETHREAD_ */
void childcode ( void FAR * arg ) ;
#endif  /* _DOSCREATETHREAD_ */
int mterm(void);

/* global data */
char Result [ _THREADMAX_ ] ;
unsigned Synchronize ;



/***
* main() - Main mthread testing shell
*
*Purpose:
*       Provides a general purpose shell for mthread testing.
*       The module does the following:
*
*               (1) Call minit() to perform test initialization operations.
*
*               (2) Begins one thread for each argument passed to the
*               program.  Each thread is passed the corresponding argument.
*               Thread begin location is assumed to be at routine childcode();
*
*               (3) Waits for all threads to terminate.
*
*               (4) Calls mterm() to perform termination operations.
*
*       Note that minit(), childcode(), and mterm() are routines that
*       are external to this source.  Again, this source doesn't care
*       what their purpose or operation is.
*
*       Also, childcode() is expected to conform to the following rules:
*
*               (1) The childcode should not start running until
*               the variable 'Synchronize' becomes non-zero.
*
*               (2) When the thread is done executing, it should set
*               the value Result[threadid] to a non-zero value so the
*               parent (i.e., this routine) knows it has completed.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

int main ( int argc , char * * argv )
{
    int rc ;
    unsigned result = 0 ;
    long ChildCount ;
    int NumThreads ;
    int t ;
    int r ;
    int MaxThread = 0 ;
    long LoopCount ;
#ifdef THREADLOOP
    char **argvsave;
#endif  /* THREADLOOP */
#ifndef _M_IX86
    char * stackbottom ;
#endif  /* _M_IX86 */


    if ( -- argc > (_THREADMAX_-1) )
    {
        printf ( "*** Error: Too many arguments***\n" ) ;
        return (-1) ;
    }

        /* Call the initiation routine */

        if (minit() != 0) {
                printf("*** Error: From minit() routine ***\n");
                return(-1);
                }

        /* Bring up the threads */

    printf ( "Process ID = %u, Thread ID = %d, ArgCount= %d\r\n" ,
        getpid ( ) , * _threadid , argc ) ;

#ifndef _M_IX86
#ifdef STACKALLOC
        printf( "(thread stacks allocated explicilty by mtest suite)\r\n");
#else  /* STACKALLOC */
        printf( "(thread stacks allocated implicitly via _beginthread)\r\n");
#endif  /* STACKALLOC */
#endif  /* _M_IX86 */

#ifdef THREADLOOP
    /* Bring up all the threads several times (so tids get re-used) */
    argvsave=argv;
    for (threadloop=1;threadloop<=_THREADLOOPCNT_;threadloop++) {
        printf("\nThreadloop = %i\n", threadloop);
        argv=argvsave;
#endif  /* THREADLOOP */

    NumThreads = 0 ;

    while ( * ++ argv )
    {

        ChildCount = atol ( * argv ) ;

#ifdef _M_IX86

        rc = _beginthread ( (void FAR *) childcode , _STACKSIZE_ ,
                (void FAR *) ChildCount ) ;

        if ( rc == -1 )

#else  /* _M_IX86 */

#ifdef STACKALLOC
        if ( ! ( stackbottom = _fmalloc ( _STACKSIZE_ ) ) )
        {
            printf ( "*** Error: Could not allocate a stack ***\n" ) ;
            break ;
        }
#else  /* STACKALLOC */
        stackbottom = (void FAR *) NULL;
#endif  /* STACKALLOC */

#ifdef _DOSCREATETHREAD_
        stackbottom+=_STACKSIZE_-16;      /* point to end of malloc'd block */
        rc1 = DOSCREATETHREAD( (void FAR *) childcode, &rc,
                (void FAR *) stackbottom);

        if (rc1 != 0)
#else  /* _DOSCREATETHREAD_ */
        rc = _beginthread ( (void FAR *) childcode , (void FAR *) stackbottom ,
            _STACKSIZE_ , (void FAR *) ChildCount ) ;

        if ( rc == -1 )
#endif  /* _DOSCREATETHREAD_ */

#endif  /* _M_IX86 */

        {
            printf ("*** Error: Could not Spawn %d-th Thread (argument=%ld) ***\n" ,
                NumThreads + 1 , ChildCount ) ;
            break ;
        }

        if ( rc > MaxThread )
            MaxThread = rc ;

        printf ( "Spawning %d-th Thread %d with argument=%ld\r\n" ,
            ++ NumThreads , rc , ChildCount ) ;
    }

    printf ( "NumThreads = %d, MaxThread = %d\r\n" ,
        NumThreads, MaxThread ) ;

        /* Let the threads begin and wait for them to term. */

    LoopCount = 0L ;

    Synchronize = 1 ;

    for ( t = 0 ; t < NumThreads ; ++ t )
    {
        r = 0 ;
        while ( ! Result [ r ] )
        {
            DOSSLEEP ( 0L ) ;
            if ( ++ r > MaxThread )
            {
                r = 0 ;
                printf ( "%ld\r" , LoopCount ++ ) ;
            }
        }

        printf ( "%d: Thread %d Done.\r\n" , t , r) ;

        Result [ r ] = '\0' ;
    }
#ifdef THREADLOOP
    }
#endif  /* THREADLOOP */

        /* All the threads have completed.  Call the term routine and return. */

        if (mterm() != 0) {
                printf("*** Error: From mterm() routine ***\n");
                return(-1);
                }

        printf("\nDone!\n");
    return 0 ;
}


