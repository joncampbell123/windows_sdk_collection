/***
*commit.c - flush buffer to disk
*
*       Copyright (c) 1990-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       contains _commit() - flush buffer to disk
*
*******************************************************************************/

#ifdef _WIN32


#include <cruntime.h>
#include <oscalls.h>    /* for RESETBUFFER */
#include <errno.h>
#include <io.h>
#include <internal.h>
#include <msdos.h>      /* for FOPEN */
#include <mtdll.h>
#include <stdlib.h>     /* for _doserrno */

/***
*int _commit(filedes) - flush buffer to disk
*
*Purpose:
*       Flushes cache buffers for the specified file handle to disk
*
*Entry:
*       int filedes - file handle of file
/*
*Exit:
*       returns success code
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _commit (
        REG1 int filedes
        )
{
        REG2 int retval;

        /* if filedes out of range, complain */
#ifdef _WIN32
        if ( ((unsigned)filedes >= (unsigned)_nhandle) ||
             !(_osfile(filedes) & FOPEN) )
        {
#else  /* _WIN32 */
        if (filedes < 0 || filedes >= _nfile) {
#endif  /* _WIN32 */
                errno = EBADF;
                return (-1);
        }

        _lock_fh(filedes);

        /* if filedes open, try to commit, else fall through to bad */
        if (_osfile(filedes) & FOPEN) {

                if ( !FlushFileBuffers((HANDLE)_get_osfhandle(filedes)) ) {
                        retval = GetLastError();
                } else {
                        retval = 0;     /* return success */
                }

                /* map the OS return code to C errno value and return code */
                if (retval == 0) {
                        goto good;
                } else {
                        _doserrno = retval;
                        goto bad;
                }

        }

bad :
        errno = EBADF;
        retval = -1;
good :
        _unlock_fh(filedes);
        return (retval);
}


#else  /* _WIN32 */

#if defined (_M_MPPC) || defined (_M_M68K)


#include <cruntime.h>
#include <errno.h>
#include <io.h>
#include <internal.h>
#include <memory.h>
#include <msdos.h>      /* for FOPEN */
#include <stdlib.h>     /* for _doserrno */
#include <macos\files.h>
#include <macos\errors.h>

/***
*int _commit(fh) - flush buffer to disk
*
*Purpose:
*       Flushes cache buffers for the specified file handle to disk
*
*Entry:
*       int filedes - file handle of file
/*
*Exit:
*       returns success code
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _commit (
        int fh
        )
{
        ParamBlockRec parm;
        OSErr osErr = 0;

        if ((unsigned)fh >= (unsigned)_nfile || !(_osfile[fh] & FOPEN))
                {
                /* out of range -- return error */
                errno = EBADF;
                _macerrno = 0;
                return -1;
                }

        if (!(_osfile[fh] & FDEV))
                {
                memset(&parm, 0, sizeof(ParamBlockRec));
                parm.ioParam.ioRefNum = _osfhnd[fh];
                osErr = PBFlushFileSync(&parm);
                switch (osErr)
                        {
                        case noErr:
                                memset(&parm, 0, sizeof(ParamBlockRec));
                                parm.ioParam.ioVRefNum = _osVRefNum[fh];
                                osErr =  PBFlushVolSync(&parm);
                                if (osErr)
                                        {
                                        _dosmaperr(osErr);
                                        return -1;
                                        }
                                return 0;

                        default:
                                errno = EIO;
                                return -1;
                        }
                }
        return 0;
}


#endif  /* defined (_M_MPPC) || defined (_M_M68K) */

#endif  /* _WIN32 */
