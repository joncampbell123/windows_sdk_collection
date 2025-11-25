/***
*_file.c - Definition of _iob[], initializer and terminator.
*
*       Copyright (c) 1985-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines _iob[], the array of stdio control structures. Also defines
*       the initializer and terminator routines for stdio.
*
*******************************************************************************/

#ifdef _WIN32


#include <cruntime.h>
#include <windows.h>
#include <stdio.h>
#include <file2.h>
#include <internal.h>
#include <malloc.h>
#include <rterr.h>
#include <dbgint.h>


/*
 * Buffer for stdin.
 */

char _bufin[_INTERNAL_BUFSIZ];

/*
 * FILE descriptors; preset for stdin/out/err (note that the __tmpnum field
 * is not initialized)
 */
FILE _iob[_IOB_ENTRIES] = {
        /* _ptr, _cnt, _base,  _flag, _file, _charbuf, _bufsiz */

        /* stdin (_iob[0]) */

        { _bufin, 0, _bufin, _IOREAD | _IOYOURBUF, 0, 0, _INTERNAL_BUFSIZ },

        /* stdout (_iob[1]) */

        { NULL, 0, NULL, _IOWRT, 1, 0, 0 },

        /* stderr (_iob[3]) */

        { NULL, 0, NULL, _IOWRT, 2, 0, 0 },

};

/*
 * Pointer to array of FILE * or _FILEX * structures.
 */
void ** __piob;

/*
 * Number of open streams (set to _NSTREAM by default)
 */
#ifdef CRTDLL
int _nstream = _NSTREAM_;
#else  /* CRTDLL */
int _nstream;
#endif  /* CRTDLL */



/*
 * Initializer and terminator for stdio
 */
void __cdecl __initstdio(void);
void __cdecl __endstdio(void);

#ifdef _MSC_VER

#pragma data_seg(".CRT$XIC")
static _PVFV pinit = __initstdio;

#pragma data_seg(".CRT$XPX")
static _PVFV pterm = __endstdio;

#pragma data_seg()

#endif  /* _MSC_VER */

#ifndef CRTDLL
/*
 * _cflush is a dummy variable used to pull in _endstdio() when any STDIO
 * routine is included in the user program.
 */
int _cflush = 0;
#endif  /* CRTDLL */


/***
* __initstdio - Initialize the stdio system
*
*Purpose:
*       Create and initialize the __piob array.
*
*Entry: <void>
*
*Exit:  <void>
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __initstdio(void)
{
        int i;

#ifndef CRTDLL
        /*
         * If the user has not supplied a definition of _nstream, set it
         * to _NSTREAM_. If the user has supplied a value that is too small
         * set _nstream to the minimum acceptable value (_IOB_ENTRIES).
         */
        if ( _nstream ==  0 )
            _nstream = _NSTREAM_;
        else if ( _nstream < _IOB_ENTRIES )
            _nstream = _IOB_ENTRIES;
#endif  /* CRTDLL */

        /*
         * Allocate the __piob array. Try for _nstream entries first. If this
         * fails then reset _nstream to _IOB_ENTRIES and try again. If it
         * still fails, bail out with an RTE.
         */
        if ( (__piob = (void **)_calloc_crt( _nstream, sizeof(void *) )) ==
             NULL ) {
            _nstream = _IOB_ENTRIES;
            if ( (__piob = (void **)_calloc_crt( _nstream, sizeof(void *) ))
                 == NULL )
                _amsg_exit( _RT_STDIOINIT );
        }

        /*
         * Initialize the first _IOB_ENTRIES to point to the corresponding
         * entries in _iob[].
         */
        for ( i = 0 ; i < _IOB_ENTRIES ; i++ )
            __piob[i] = (void *)&_iob[i];

        for ( i = 0 ; i < 3 ; i++ ) {
            if ( (_osfhnd(i) == (long)INVALID_HANDLE_VALUE) ||
                 (_osfhnd(i) == 0L) )
            {
                _iob[i]._file = -1;
            }
        }
}


/***
* __endstdio - Terminate the stdio system
*
*Purpose:
*       Terminate the stdio system
*
*       (1) Flush all streams.  (Do this even if we're going to
*       call fcloseall since that routine won't do anything to the
*       std streams.)
*
*       (2) If returning to caller, close all streams.  This is
*       not necessary if the exe is terminating because the OS will
*       close the files for us (much more efficiently, too).
*
*Entry: <void>
*
*Exit:  <void>
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

void __cdecl __endstdio(void)
{
        /* flush all streams */
        _flushall();

        /* if in callable exit, close all streams */
        if (_exitflag)
                _fcloseall();
}


#else  /* _WIN32 */

#if defined (_M_MPPC) || defined (_M_M68K)


#include <cruntime.h>
#include <stdio.h>
#include <file2.h>

/*
 * Buffer for stdin.
 */

char _bufin[BUFSIZ];


/*
 * FILE descriptors; preset for stdin/out/err (note that the __tmpnum field
 * is not initialized)
 */

FILE _iob[ _NSTREAM_ ] = {

        /* _ptr, _cnt, _base,  _flag, _file, _charbuf, _bufsiz */

        /* stdin (_iob[0]) */

        { _bufin, 0, _bufin, _IOREAD | _IOYOURBUF, 0, 0, BUFSIZ },

        /* stdout (_iob[1]) */

        { NULL, 0, NULL, _IOWRT, 1, 0, 0 },

        /* stderr (_iob[3]) */

        { NULL, 0, NULL, _IOWRT, 2, 0, 0 },

};


/*
 * pointer to end of descriptors
 */

FILE * _lastiob = &_iob[_NSTREAM_ - 1];


#endif  /* defined (_M_MPPC) || defined (_M_M68K) */

#endif  /* _WIN32 */
