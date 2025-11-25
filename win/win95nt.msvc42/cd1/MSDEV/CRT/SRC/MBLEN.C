/***
*mblen.c - length of multibyte character
*
*       Copyright (c) 1990-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Return the number of bytes contained in a multibyte character.
*
*******************************************************************************/

#ifdef _WIN32
#include <internal.h>
#include <locale.h>
#include <setlocal.h>
#endif  /* _WIN32 */
#include <cruntime.h>
#include <stdlib.h>
#include <ctype.h>
#include <mtdll.h>
#include <dbgint.h>

/***
*int mblen() - length of multibyte character
*
*Purpose:
*       Return the number of bytes contained in a multibyte character.
*       [ANSI].
*
*Entry:
*       const char *s = pointer to multibyte character
*       size_t      n = maximum length of multibyte character to consider
*
*Exit:
*       If s = NULL, returns 0, indicating we use (only) state-independent
*       character encodings.
*       If s != NULL, returns:   0 (if *s = null char)
*                   -1 (if the next n or fewer bytes not valid mbc)
*                    number of bytes contained in multibyte char
*
*Exceptions:
*
*******************************************************************************/

int __cdecl mblen
        (
        const char * s,
        size_t n
        )
{
        _ASSERTE (MB_CUR_MAX == 1 || MB_CUR_MAX == 2);

        if ( !s || !(*s) || (n == 0) )
            /* indicate do not have state-dependent encodings,
               empty string length is 0 */
            return 0;


        if ( isleadbyte((unsigned char)*s) )
        {
            /* multi-byte char */

            /* verify valid MB char */
#ifdef _WIN32
            if ( MB_CUR_MAX <= 1 || (int)n < MB_CUR_MAX ||
            MultiByteToWideChar(__lc_codepage, MB_PRECOMPOSED|MB_ERR_INVALID_CHARS,
            s, MB_CUR_MAX, NULL, 0) == 0 )
#else  /* _WIN32 */
            /* validate high byte of mbcs char */
            if ((n<(size_t)MB_CUR_MAX) || (!*(s+1)))
#endif  /* _WIN32 */
                /* bad MB char */
                return -1;
            else
                return MB_CUR_MAX;
        }
        else {
            /* single byte char */

#ifdef _WIN32
            /* verify valid SB char */
            if ( MultiByteToWideChar(__lc_codepage, MB_PRECOMPOSED|MB_ERR_INVALID_CHARS,
            s, 1, NULL, 0) == 0 )
                return -1;
#endif  /* _WIN32 */
            return sizeof(char);
        }

}
