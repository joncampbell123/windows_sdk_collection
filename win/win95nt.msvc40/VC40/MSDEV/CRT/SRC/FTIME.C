/***
*ftime.c - return system time
*
*       Copyright (c) 1985-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Returns the system date/time in a structure form.
*
*******************************************************************************/

#ifdef _WIN32



#include <cruntime.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <dostypes.h>
#include <msdos.h>
#include <dos.h>
#include <stdlib.h>
#include <oscalls.h>
#include <internal.h>

/***
*void _ftime(timeptr) - return DOS time in a structure
*
*Purpose:
*       returns the current DOS time in a struct timeb structure
*
*Entry:
*       struct timeb *timeptr - structure to fill in with time
*
*Exit:
*       no return value -- fills in structure
*
*Exceptions:
*
*******************************************************************************/

_CRTIMP void __cdecl _ftime (
        struct _timeb *tp
        )
{
        struct tm tb;
        SYSTEMTIME dt;

        __tzset();

        tp->timezone = (short)(_timezone / 60);

        GetLocalTime(&dt);

        tp->millitm = (unsigned short)(dt.wMilliseconds);

        tb.tm_year  = dt.wYear - 1900;
        tb.tm_mday  = dt.wDay;
        tb.tm_mon   = dt.wMonth - 1;
        tb.tm_hour  = dt.wHour;
        tb.tm_min   = dt.wMinute;
        tb.tm_sec   = dt.wSecond;
        tb.tm_isdst = -1;

        /*
         * Call mktime() to compute time_t value and Daylight Savings Time
         * flag.
         */
        tp->time = mktime(&tb);

        tp->dstflag = (short)(tb.tm_isdst);
}



#else  /* _WIN32 */

#if defined (_M_MPPC) || defined (_M_M68K)


#include <cruntime.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <stdlib.h>
#include <internal.h>
#include <macos\osutils.h>      /* get DataTimeRec type */
#include <macos\memory.h>
#include <macos\lowmem.h>

/***
*void _ftime(timeptr) - return DOS time in a structure
*
*Purpose:
*       returns the current DOS time in a struct timeb structure
*
*Entry:
*       struct timeb *timeptr - structure to fill in with time
*
*Exit:
*       no return value -- fills in structure
*
*Exceptions:
*
*******************************************************************************/

void __cdecl _ftime (
        struct _timeb *tp
        )
{
        struct tm tb;
        DateTimeRec dt;

        _tzset();
        tp->timezone = (short)(_timezone / 60);

        GetTime(&dt);

        /*set milliseconds*/

        tp->millitm = (unsigned short)( ((LMGetTicks() % 60) * 50) / 3);

        tb.tm_year = dt.year - 1900;
        tb.tm_mday = dt.day;
        tb.tm_mon = dt.month - 1;         //[1-12]=>[0-11]
        tb.tm_hour = dt.hour;
        tb.tm_min   = dt.minute;
        tb.tm_sec   = dt.second;
        tb.tm_wday = dt.dayOfWeek - 1;    //[1-7]=>[0-6]
        tb.tm_isdst = -1;

        /*
         * Call mktime() to compute time_t value and Daylight Savings Time
         * flag.
         */
        tp->time = mktime(&tb);

        tp->dstflag = (short)(tb.tm_isdst);
}


#endif  /* defined (_M_MPPC) || defined (_M_M68K) */

#endif  /* _WIN32 */
