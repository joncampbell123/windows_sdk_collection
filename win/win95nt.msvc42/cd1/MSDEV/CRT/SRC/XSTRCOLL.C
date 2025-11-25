/***
*xstrcoll.c - Collate locale strings
*
*       Copyright (c) 1996, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Compare two strings using the locale LC_COLLATE information.
*
*Revision History:
*       01-XX-96  PJP   Created from strcoll.c January 1996 by P.J. Plauger
*       04-17-96  GJF   Updated for current locale locking. Also, reformatted
*                       and made several cosmetic changes.
*       05-14-96  JWM   Bug fix to _Strcoll(): error path failed to unlock.
*
*******************************************************************************/

#include <cruntime.h>
#include <string.h>
#include <xlocinfo.h>   /* for _Collvec, _Strcoll */

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
*int _Strcoll() - Collate locale strings
*
*Purpose:
*       Compare two strings using the locale LC_COLLATE information.
*       [ANSI].
*
*       Non-C locale support available under _INTL switch.
*       In the C locale, strcoll() simply resolves to strcmp().
*Entry:
*       const char *s1b = pointer to beginning of the first string
*       const char *s1e = pointer past end of the first string
*       const char *s2b = pointer to beginning of the second string
*       const char *s1e = pointer past end of the second string
*       const _Collvec *ploc = pointer to locale info
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

int __cdecl _Strcoll (
        const char *_string1,
        const char *_end1,
        const char *_string2,
        const char *_end2,
        const _Collvec *ploc
        )
{
#if defined (_WIN32)
        int ret;
        int coll_codepage;
        WCHAR wcstmp[MAX_CP_LEN];
        LCID handle;
#ifdef  _MT
        int local_lock_flag;
#endif
#endif

        size_t n1 = _end1 - _string1;
        size_t n2 = _end2 - _string2;

#if defined (_WIN32)
        if (ploc == 0)
            handle = __lc_handle[LC_COLLATE];
        else
            handle = ploc->_Hand;

        if (handle == _CLOCALEHANDLE) {
            int ans = memcmp(_string1, _string2, n1 < n2 ? n1 : n2);
            return ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1;
        }

        _lock_locale( local_lock_flag )

#ifdef _MT
        if (handle == _CLOCALEHANDLE) {
            int ans;
            _unlock_locale( local_lock_flag )
            ans = memcmp(_string1, _string2, n1 < n2 ? n1 : n2);
            return ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1;
        }
#endif  /* _MT */

        /*
         * Must use default code page for the LC_COLLATE category for
         * MB/WC conversion inside __crtxxx().
         */

        if (__crtGetLocaleInfoW(handle, LOCALE_IDEFAULTCODEPAGE,
            wcstmp, MAX_CP_LEN, 0) == 0) {
            _unlock_locale( local_lock_flag )
            return _NLSCMPERROR;
            }

        coll_codepage = (int)wcstol(wcstmp, NULL, 10);

        if (0 == (ret = __crtCompareStringA(handle, 0,
            _string1, n1, _string2, n2, coll_codepage)))
            goto error_cleanup;

	    _unlock_locale( local_lock_flag )
        return (ret - 2);

error_cleanup:

    	_unlock_locale( local_lock_flag )
        errno = EINVAL;
        return _NLSCMPERROR;

#else  /* defined (_WIN32) */

        int ans = memcmp(_string1, _string2, n1 < n2 ? n1 : n2);
        return ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1;

#endif  /* defined (_WIN32) */
}


/***
*_Collvec _Getcoll() - get collation info for current locale
*
*Purpose:
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

_Collvec _Getcoll()
{
        _Collvec coll;
        coll._Hand = __lc_handle[LC_COLLATE];
        coll._Page = __lc_codepage;
        return (coll);
}
