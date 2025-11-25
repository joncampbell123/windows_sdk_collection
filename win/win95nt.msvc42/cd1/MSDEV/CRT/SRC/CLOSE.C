/***
*close.c - close file handle for Windows NT
*
*       Copyright (c) 1989-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _close() - close a file handle
*
*******************************************************************************/

#ifdef _WIN32


#include <cruntime.h>
#include <oscalls.h>
#include <io.h>
#include <mtdll.h>
#include <errno.h>
#include <stdlib.h>
#include <msdos.h>
#include <internal.h>

/***
*int _close(fh) - close a file handle
*
*Purpose:
*       Closes the file associated with the file handle fh.
*
*Entry:
*       int fh - file handle to close
*
*Exit:
*       returns 0 if successful, -1 (and sets errno) if fails
*
*Exceptions:
*
*******************************************************************************/

#ifdef _MT

/* define normal version that locks/unlocks, validates fh */

int __cdecl _close (
        int fh
        )
{
        int r;                          /* return value */

        /* validate file handle */
        if ( ((unsigned)fh >= (unsigned)_nhandle) ||
             !(_osfile(fh) & FOPEN) )
         {
                /* bad file handle, set errno and abort */
                errno = EBADF;
                _doserrno = 0;
                return -1;
        }

        _lock_fh(fh);                   /* lock file */
        r = _close_lk(fh);
        _unlock_fh(fh);                 /* unlock the file */

        return r;
}

/* now define version that doesn't lock/unlock, validate fh */
int __cdecl _close_lk (
        int fh
        )
{
        DWORD dosretval;

#else  /* _MT */

/* now define normal version */
int __cdecl _close (
        int fh
        )
{
        DWORD dosretval;

        /* validate file handle */
        if ( ((unsigned)fh >= (unsigned)_nhandle) ||
             !(_osfile(fh) & FOPEN) )
        {
                /* bad file handle, set errno and abort */
                errno = EBADF;
                _doserrno = 0;  /* no o.s. error */
                return -1;
        }
#endif  /* _MT */
        /*
         * Close the underlying OS file handle. Note that if fh is STDOUT
         * or STDERR, and if STDOUT and STDERR are mapped to the same OS
         * file handle, the close is skipped (without error). STDOUT and
         * STDERR are the only handles for which this support is provided.
         * Other handles are mapped to the same OS file handle only at the
         * programmer's risk.
         */
        if ( ( ((fh == 1) || (fh == 2)) &&
               (_get_osfhandle(1) == _get_osfhandle(2)) ) ||
             CloseHandle( (HANDLE)_get_osfhandle(fh) ) )
        {

                dosretval = 0L;
        }
        else
                dosretval = GetLastError();

        _free_osfhnd(fh);

        if (dosretval) {
                /* OS error */
                _dosmaperr(dosretval);
                return -1;
        }
        _osfile(fh) = 0;                /* clear file flags */

        return 0;                       /* good return */
}


#else  /* _WIN32 */

#if defined (_M_MPPC) || defined (_M_M68K)


#include <cruntime.h>
#include <msdos.h>
#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <internal.h>
#include <macos\files.h>
#include <macos\errors.h>

/***
*int _close(fh) - close a file handle
*
*Purpose:
*       Closes the file associated with the file handle fh.
*
*Entry:
*       int fh - file handle to close
*
*Exit:
*       returns 0 if successful, -1 (and sets errno) if fails
*
*Exceptions:
*
*******************************************************************************/


/* now define normal version */
int __cdecl _close (
        int fh
        )
{
        ParamBlockRec parm;
        OSErr osErr = 0;

        /* validate file handle */
        if ((unsigned)fh >= (unsigned)_nfile || !(_osfile[fh] & FOPEN))
                {
                /* bad file handle, set errno and abort */
                errno = EBADF;
                _macerrno = 0;
                return -1;
                }


        if (!(_osfile[fh] & FDEV))       /*no need to close console*/
                {
                if (_osfhnd[fh] == -1 )
                        {
                        /*an open file with a -1 handle indicates a pseudo open
                                for stdin, stdout, or stderr -- close with no error*/

                        _osfhnd[fh] = 0;  /*only allowed to close once*/
                        }
                else
                        {
                        parm.ioParam.ioRefNum = _osfhnd[fh];
                        parm.ioParam.ioNamePtr = NULL;
                        osErr = PBCloseSync(&parm);


                        if (osErr)
                                {
                                _dosmaperr(osErr);
                                return -1;
                                }

                        memset(&parm, 0, sizeof(ParamBlockRec));
                        parm.ioParam.ioVRefNum = _osVRefNum[fh];
                        osErr =  PBFlushVolSync(&parm);
                        }
                }

        _osfile[fh] = 0;                /* clear file flags */
        if (osErr)
                {
                _dosmaperr(osErr);
                return -1;
                }
        return 0;                       /* good return */
}


#endif  /* defined (_M_MPPC) || defined (_M_M68K) */

#endif  /* _WIN32 */
