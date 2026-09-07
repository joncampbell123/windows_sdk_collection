/***
*remove.c - MAC remove a file or dir
*
*       Copyright (c) 1989-1992, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines remove() - deletes a file or dir
*
*******************************************************************************/

#include <cruntime.h>
//#include <oscalls.h>
#include <internal.h>
#include <io.h>
#include <errno.h>
#include <stdlib.h>
#include <macos\files.h>
#include <macos\errors.h>
#include <string.h>

/***
*int remove(path) - delete the given file or directory
*
*Purpose:
*       This version deletes the given file because there are no
*       links under OS/2.
*
*
*Entry:
*       char *path -    file to delete
*
*Exit:
*       returns 0 if successful
*       returns -1 and sets errno if unsuccessful
*
*Exceptions:
*
*******************************************************************************/

int _CALLTYPE1 remove (
        const char *path
        )
{
        ParamBlockRec parm;
        OSErr osErr;
        char stPath[256];

        if (!*path || strlen(path) > 255)
                {
                errno = ENOENT;
                return -1;
                }
        strcpy(stPath, path);
        _c2pstr(stPath);

        memset(&parm, '\0', sizeof(ParamBlockRec));
        parm.fileParam.ioNamePtr = stPath;
        /* parm.fileParam.ioVRefNum = 0; */
        osErr = PBDeleteSync(&parm);

        if (osErr)
                {
                /* error occured -- map error code and return */
                _dosmaperr(osErr);
                return -1;
                }
        return 0;

}

