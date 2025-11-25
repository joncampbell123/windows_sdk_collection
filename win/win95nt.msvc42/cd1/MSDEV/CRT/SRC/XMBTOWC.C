/***
*xmbtowc.c - Convert multibyte char to wide char.
*
*       Copyright (c) 1995-1996, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert a multibyte character into the equivalent wide character.
*
*Revison History:
*       12-XX-95  PJP   Created from mbtowc.c December 1995 by P.J. Plauger
*       04-17-96  GJF   Updated for current locale locking. Also, reformatted
*                       and made several cosmetic changes.
*
*******************************************************************************/

#include <cruntime.h>
#include <stdlib.h>
#include <mtdll.h>
#include <errno.h>
#include <dbgint.h>
#include <ctype.h>
#include <limits.h>              /* for INT_MAX */
#include <stdio.h>               /* for EOF */
#include <xlocinfo.h>            /* for _Cvtvec, _Mbrtowc */
#ifdef _WIN32
#include <internal.h>
#include <locale.h>
#include <setlocal.h>
#endif  /* _WIN32 */

/***
*int _Mbrtowc() - Convert multibyte char to wide character.
*
*Purpose:
*       Convert a multi-byte character into the equivalent wide character,
*       according to the specified LC_CTYPE category, or the current locale.
*       [ANSI].
*
*       NOTE:  Currently, the C libraries support the "C" locale only.
*              Non-C locale support now available under _INTL switch.
*Entry:
*       wchar_t  *pwc = pointer to destination wide character
*       const char *s = pointer to multibyte character
*       size_t      n = maximum length of multibyte character to consider
*               mbstate_t *pst          = pointer to state
*       const _Cvtvec *     = pointer to locale info
*
*Exit:
*       If s = NULL, returns 0, indicating we only use state-independent
*       character encodings.
*       If s != NULL, returns:  0 (if *s = null char)
*                               -1 (if the next n or fewer bytes not valid mbc)
*                               number of bytes comprising converted mbc
*
*Exceptions:
*
*******************************************************************************/

#ifdef _MT
int __cdecl _Mbrtowc_lk
        (
        wchar_t  *pwc,
        const char *s,
        size_t n,
        mbstate_t *pst,
        const _Cvtvec *ploc
        );

int __cdecl _Mbrtowc(
        wchar_t  *pwc,
        const char *s,
        size_t n,
        mbstate_t *pst,
        const _Cvtvec *ploc
        )
{
        int retval;
        int local_lock_flag;

        _lock_locale( local_lock_flag )
        retval = _Mbrtowc_lk(pwc, s, n, pst, ploc);
        _unlock_locale( local_lock_flag )
        return retval;
}
#endif  /* _MT */
#ifdef _MT
int __cdecl _Mbrtowc_lk
#else  /* _MT */
int __cdecl _Mbrtowc
#endif  /* _MT */
        (
        wchar_t  *pwc,
        const char *s,
        size_t n,
        mbstate_t *pst,
        const _Cvtvec *ploc
        )
{
        _ASSERTE (MB_CUR_MAX == 1 || MB_CUR_MAX == 2);

        if ( !s || n == 0 )
            /* indicate do not have state-dependent encodings,
               handle zero length string */
            return 0;

        if ( !*s )
        {
            /* handle NULL char */
            if (pwc)
                *pwc = 0;
            return 0;
        }

#ifdef _WIN32

        {   /* perform locale-dependent parse */
            LCID handle;
            UINT codepage;

            if (ploc == 0)
            {
                handle = __lc_handle[LC_CTYPE];
                codepage = __lc_codepage; 
            }
            else
            {
                handle = ploc->_Hand;
                codepage = ploc->_Page; 
            }

            if ( handle == _CLOCALEHANDLE )
            {
                if (pwc)
                    *pwc = (wchar_t)(unsigned char)*s;
                return sizeof(char);
            }

            if (*pst != 0)
            {   /* complete two-byte multibyte character */
                ((char *)pst)[1] = *s;
                if (MB_CUR_MAX <= 1 || (MultiByteToWideChar(codepage,
                    MB_PRECOMPOSED|MB_ERR_INVALID_CHARS,
                    (char *)pst, 2, pwc, (pwc) ? 1 : 0) == 0))
                {   /* translation failed */
                    *pst = 0;
                    errno = EILSEQ;
                    return -1;
                }
                *pst = 0;
            }
            else if ( isleadbyte((unsigned char)*s) )
            {
                /* multi-byte char */
                if (n < MB_CUR_MAX)
                {   /* save partial multibyte character */
                    ((char *)pst)[0] = *s;
                    return (-2);
                }
                else if ( MB_CUR_MAX <= 1 ||
                          (MultiByteToWideChar( codepage, 
                                                MB_PRECOMPOSED |
                                                    MB_ERR_INVALID_CHARS,
                                                s, 
                                                MB_CUR_MAX, 
                                                pwc, 
                                                (pwc) ? 1 : 0) == 0) )
                {
                    /* validate high byte of mbcs char */
                    if (!*(s+1))
                    {
                        *pst = 0;
                        errno = EILSEQ;
                        return -1;
                    }
/*                  else translation failed with no complaint? [pjp] */
                }
                return MB_CUR_MAX;
            }
            else {
                /* single byte char */

                if ( MultiByteToWideChar( codepage, 
                                          MB_PRECOMPOSED|MB_ERR_INVALID_CHARS,
                                          s, 
                                          1, 
                                          pwc, 
                                          (pwc) ? 1 : 0) == 0 )
                {
                    errno = EILSEQ;
                    return -1;
                }

                return sizeof(char);
            }
        }

#else  /* _WIN32 */

        /* stuck the "C" locale again */
        if (pwc)
            *pwc = (wchar_t)(unsigned char)*s;
        return sizeof(char);

#endif  /* _WIN32 */
}


/***
*wint_t btowc(c) - translate single byte to wide char 
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

wint_t (btowc) (
        int c
        )
{
        if (c == EOF)
            return (WEOF);
        else
        {   /* convert as one-byte string */
            char ch = c;
            mbstate_t mbst = 0;
            wchar_t wc;
            return (_Mbrtowc(&wc, &ch, 1, &mbst, 0) < 0 ? WEOF : wc);
        }
}


/***
*size_t mbrlen(s, n, pst) - determine next multibyte code, restartably
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

size_t (mbrlen) (
        const char *s, 
        size_t n, 
        mbstate_t *pst
        )
{
        static mbstate_t mbst = {0};    /* NOT SAFE FOR MT */
        return (_Mbrtowc(0, s != 0 ? s : 0, n, pst ? pst : &mbst, 0));
}


/***
*size_t mbrtowc(pwc, s, n, pst) - translate multibyte to wchar_t, restartably
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

size_t (mbrtowc) (
        wchar_t *pwc, 
        const char *s, 
        size_t n, 
        mbstate_t *pst
        )
{
        static mbstate_t mbst = {0};    /* NOT SAFE FOR MT */
        return (s != 0 ? _Mbrtowc(pwc, s, n, pst ? pst : &mbst, 0)
                : _Mbrtowc(0, "", n, pst ? pst : &mbst, 0));
}


/***
*size_t mbsrtowcs(wcs, ps, n, pst) - translate multibyte string to wide, 
*       restartably
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

size_t (mbsrtowcs) (
        wchar_t *wcs, 
        const char **ps, 
        size_t n, 
        mbstate_t *pst
        )
{
        const char *s = *ps;
        int i;
        size_t nwc = 0;
        static mbstate_t mbst = {0};    /* NOT SAFE FOR MT */

        if (pst == 0)
            pst = &mbst;

        if (wcs == 0)
            for (; ; ++nwc, s += i)
            {   /* translate but don't store */
                wchar_t wc;
                if ((i = _Mbrtowc(&wc, s, INT_MAX, pst, 0)) < 0)
                      return ((size_t)-1);
                else if (i == 0)
                      return (nwc);
            }

        for (; 0 < n; ++nwc, s += i, ++wcs, --n)
        {   /* translate and store */
            if ((i = _Mbrtowc(wcs, s, INT_MAX, pst, 0)) < 0)
            {   /* encountered invalid sequence */
                nwc = (size_t)-1;
                break;
            }
            else if (i == 0)
            {   /* encountered terminating null */
                s = 0;
                break;
            }
        }

        *ps = s;
        return (nwc);
}
