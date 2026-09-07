/***
*strnicoll.c - Collate locale strings without regard to case
*
*   Copyright (c) 1988-1994, Microsoft Corporation. All rights reserved.
*
*Purpose:
*   Compare two strings using the locale LC_COLLATE information.
*   Compares at most n characters of two strings.
*
*******************************************************************************/

#include <cruntime.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <locale.h>
#include <setlocal.h>
#include <mtdll.h>
#include <errno.h>
#include <awint.h>
#endif  /* _WIN32 */

/***
*int _strnicoll() - Collate locale strings without regard to case
*
*Purpose:
*       Compare two strings using the locale LC_COLLATE information
*       without regard to case.
*       Compares at most n characters of two strings.
*
*Entry:
*       const char *s1 = pointer to the first string
*       const char *s2 = pointer to the second string
*       size_t count - maximum number of characters to compare
*
*Exit:
*       Less than 0    = first string less than second string
*       0              = strings are equal
*       Greater than 0 = first string greater than second string
*
*Exceptions:
*       _NLSCMPERROR    = error
*       errno = EINVAL
*
*******************************************************************************/

int __cdecl _strnicoll (
        const char *_string1,
        const char *_string2,
        size_t count
        )
{
#if defined (_WIN32)

        int ret;
        int coll_codepage;
        WCHAR wcstmp[MAX_CP_LEN];

        if (!count)
            return 0;

        _mlock (_LC_COLLATE_LOCK);

        if (__lc_handle[LC_COLLATE] == _CLOCALEHANDLE) {
                _munlock (_LC_COLLATE_LOCK);
                return _strnicmp(_string1, _string2, count);
        }

        /*
         * Must use default code page for the LC_COLLATE category for
         * MB/WC conversion inside __crtxxx().
         */

        if (__crtGetLocaleInfoW(__lc_handle[LC_COLLATE], LOCALE_IDEFAULTCODEPAGE,
            wcstmp, MAX_CP_LEN, 0) == 0)
                return _NLSCMPERROR;

        coll_codepage = (int)wcstol(wcstmp, NULL, 10);

        if (0 == (ret = __crtCompareStringA(__lc_handle[LC_COLLATE], NORM_IGNORECASE,
                _string1, count, _string2, count, coll_codepage)))
            goto error_cleanup;

        _munlock (_LC_COLLATE_LOCK);
        return (ret - 2);

error_cleanup:
        _munlock (_LC_COLLATE_LOCK);
        errno = EINVAL;
        return _NLSCMPERROR;

#else  /* defined (_WIN32) */

        return _strnicmp(_string1, _string2, count);

#endif  /* defined (_WIN32) */
}
