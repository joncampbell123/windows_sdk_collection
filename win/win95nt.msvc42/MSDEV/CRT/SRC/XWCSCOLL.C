/***
*xwcscoll.c - Collate wide-character locale strings
*
*       Copyright (c) 1996, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Compare two wchar_t strings using the locale LC_COLLATE information.
*
*Revision History:
*       01-XX-96  GJF   Created from wcscoll.c January 1996 by P.J. Plauger
*       04-18-96  GJF   Updated for current locale locking. Also, reformatted
*                       and made several cosmetic changes.
*
*******************************************************************************/


#include <cruntime.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <setlocal.h>
#include <mtdll.h>
#include <errno.h>
#include <awint.h>
#include <xlocinfo.h>   /* for _Collvec, _Wcscoll */

/***
*static int _Wmemcmp(s1, s2, n) - compare wchar_t s1[n], s2[n]
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

static int _Wmemcmp(
        const wchar_t *s1, 
        const wchar_t *s2, 
        size_t n
        )
{
        for (; 0 < n; ++s1, ++s2, --n)
             if (*s1 != *s2)
               return (*s1 < *s2 ? -1 : +1);
        return (0);
}

/***
*int _Wcscoll() - Collate wide-character locale strings
*
*Purpose:
*       Compare two wchar_t strings using the locale LC_COLLATE information.
*       In the C locale, wcscmp() is used to make the comparison.
*
*Entry:
*       const wchar_t *_string1 = pointer to beginning of the first string
*       const wchar_t *_end1    = pointer past end of the first string
*       const wchar_t *_string2 = pointer to beginning of the second string
*       const wchar_t *_end2    = pointer past end of the second string
*       const _Collvec *ploc = pointer to locale info
*
*Exit:
*       -1 = first string less than second string
*        0 = strings are equal
*        1 = first string greater than second string
*       This range of return values may differ from other *cmp/*coll functions.
*
*Exceptions:
*       _NLSCMPERROR    = error
*       errno = EINVAL
*
*******************************************************************************/

int __cdecl _Wcscoll (
        const wchar_t *_string1,
        const wchar_t *_end1,
        const wchar_t *_string2,
        const wchar_t *_end2,
        const _Collvec *ploc
        )
{

        size_t n1 = _end1 - _string1;
        size_t n2 = _end2 - _string2;
        int ret;
        int coll_codepage;
        WCHAR wcstmp[MAX_CP_LEN];
        LCID handle;
#ifdef _MT
        int local_lock_flag;
#endif

        if (ploc == 0)
            handle = __lc_handle[LC_COLLATE];
        else
            handle = ploc->_Hand;

        if (handle == _CLOCALEHANDLE) {
            int ans = _Wmemcmp(_string1, _string2, n1 < n2 ? n1 : n2);
            return ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1;
        }

        _lock_locale( local_lock_flag )

#ifdef _MT
        if (handle == _CLOCALEHANDLE) {
            int ans;
            _unlock_locale( local_lock_flag )
            ans = _Wmemcmp(_string1, _string2, n1 < n2 ? n1 : n2);
            return ans != 0 || n1 == n2 ? ans : n1 < n2 ? -1 : +1;
        }
#endif  /* _MT */

        /*
         * Must use default code page for the LC_COLLATE category for
         * MB/WC conversion inside __crtxxx().
         */

        if (__crtGetLocaleInfoW(handle, LOCALE_IDEFAULTCODEPAGE,
            wcstmp, MAX_CP_LEN, 0) == 0)
            return _NLSCMPERROR;

        coll_codepage = (int)wcstol(wcstmp, NULL, 10);

        if (0 == (ret = __crtCompareStringW(handle, 
                                            0, 
                                            _string1, 
                                            n1,
                                            _string2, 
                                            n2, 
                                            coll_codepage)))
        {
            _unlock_locale( local_lock_flag )
            errno = EINVAL;
            return _NLSCMPERROR;
        }

        _unlock_locale( local_lock_flag )
        return (ret - 2);

}
