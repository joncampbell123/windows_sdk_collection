/***
*strtod.c - convert string to floating point number
*
*       Copyright (c) 1985-1991, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Convert character string to floating point number
*
*******************************************************************************/

#include <cruntime.h>
#include <stdlib.h>
#include <fltintrn.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

/***
*long double _strtod(nptr, endptr) - convert string to long double
*
*Purpose:
*       strtold recognizes an optional string of tabs and spaces,
*       then an optional sign, then a string of digits optionally
*       containing a decimal point, then an optional e or E followed
*       by an optionally signed integer, and converts all this to
*       to a floating point number.  The first unrecognized
*       character ends the string, and is pointed to by endptr.
*
*Entry:
*       nptr - pointer to string to convert
*
*Exit:
*       returns value of character string
*       char **endptr - if not NULL, points to character which stopped
*                       the scan
*
*Exceptions:
*
*******************************************************************************/

long double _CALLTYPE1 _strtold (
        const char *nptr,
        REG2 char **endptr
        )
{

        FLTL     answer;
        long double          tmp;
        unsigned int flags;
        REG1 char *ptr = (char *) nptr;

        /* scan past leading space/tab characters */

        while (isspace(*ptr))
                ptr++;

        /* let _fltinl routine do the rest of the work */

        answer = _fltinl(ptr, strlen(ptr), 0, 0);

        if ( endptr != NULL )
                *endptr = (char *) ptr + answer->nbytes;

        flags = answer->flags;
        if ( flags & (512 | 64)) {
                /* no digits found or invalid format:
                   ANSI says return 0.0, and *endptr = nptr */
                tmp = 0.0;
                if ( endptr != NULL )
                        *endptr = (char *) nptr;
        }
        else if ( flags & (128 | 1) ) {
                if ( *ptr == '-' )
                        tmp = -HUGE_VAL;        /* negative overflow */
                else
                        tmp = HUGE_VAL;         /* positive overflow */
                errno = ERANGE;
        }
        else if ( flags & 256 ) {
                tmp = 0.0;                      /* underflow */
                errno = ERANGE;
        }
        else
                tmp = answer->ldval;

        return(tmp);
}
