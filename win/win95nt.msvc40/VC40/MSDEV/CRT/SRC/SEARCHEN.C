/***
*searchenv.c - find a file using paths from an environment variable
*
*       Copyright (c) 1987-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       to search a set a directories specified by an environment variable
*       for a specified file name.  If found the full path name is returned.
*
*******************************************************************************/

#include <cruntime.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <internal.h>
#include <tchar.h>

/***
*_searchenv() - search for file along paths from environment variable
*
*Purpose:
*       to search for a specified file in the directory(ies) specified by
*       a given environment variable, and, if found, to return the full
*       path name of the file.  The file is first looked for in the current
*       working directory, prior to looking in the paths specified by env_var.
*
*Entry:
*       fname - name of file to search for
*       env_var - name of environment variable to use for paths
*       path - pointer to storage for the constructed path name
*
*Exit:
*       path - pointer to constructed path name, if the file is found, otherwise
*              it points to the empty string.
*
*Exceptions:
*
*******************************************************************************/

void __cdecl _tsearchenv (
        const _TSCHAR *fname,
        const _TSCHAR *env_var,
        register _TSCHAR *path
        )
{
        register _TSCHAR *p;
        register int c;
        _TSCHAR *env_p;

        if (_taccess(fname, 0) == 0) {

#if defined (_WIN32)
            /* exists, convert it to a fully qualified pathname and
               return */
            if ( _tfullpath(path, fname, _MAX_PATH) == NULL )
                *path = _T('\0');
#else  /* defined (_WIN32) */
            /* exists in this directory - get cwd and concatenate file
               name */
            if (_tgetcwd(path, _MAX_PATH))
                {
                _tcscat(path, fname);
                }
#endif  /* defined (_WIN32) */

            return;
        }
        if ((env_p = _tgetenv(env_var)) == NULL) {
            /* no such environment var. and not in cwd, so return empty
               string */
            *path = _T('\0');
            return;
        }
#ifdef _UNICODE
        while ( (env_p = _wgetpath(env_p, path, 0)) && *path ) {
#else  /* _UNICODE */
        while ( (env_p = _getpath(env_p, path, 0)) && *path ) {
#endif  /* _UNICODE */
            /* path now holds nonempty pathname from env_p, concatenate
               the file name and go */

#ifdef _WIN32
            p = path + _tcslen(path);
            if (((c = *(p - 1)) != _T('/')) && (c != _T('\\')) && (c != _T(':'))) {
                /* add a trailing '\' */
                *p++ = _T('\\');
            }
            /* p now points to character following trailing '/', '\'
               or ':' */

#else  /* _WIN32 */
            if (_tcsstr(path, _T(":")) == NULL)
                {
                /*
                 *path contains single name as partial dir name
                 *prefix ':' to make it a partial path
                 */
                 memmove(path+1, path, _tcslen(path)+1);
                 *path = ':';
                 }
            p = path + _tcslen(path);
            if ((c = *(p - 1)) != _T(':')) {
                /* add a trailing ':' */
                *p++ = _T(':');
            }
            /* p now points to character following trailing ':' */
#endif  /* _WIN32 */

            _tcscpy(p, fname);
            if (_taccess(path, 0) == 0) {
                /* found a match, we're done */
                return;
            }
        }
        /* if we get here, we never found it, return empty string */
        *path = _T('\0');
}
