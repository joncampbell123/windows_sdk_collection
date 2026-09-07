/***
*time.c - get current system time
*
*       Copyright (c) 1989-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines time() - gets the current system time and converts it to
*                        internal (time_t) format time.
*
*******************************************************************************/


#include <cruntime.h>
#include <time.h>
#include <internal.h>

#ifdef _WIN32
#include <windows.h>
#else  /* _WIN32 */
#if defined (_M_MPPC) || defined (_M_M68K)
#include <macos\osutils.h>     /* get DataTimeRec type */
#endif  /* defined (_M_MPPC) || defined (_M_M68K) */
#endif  /* _WIN32 */

/***
*time_t time(timeptr) - Get current system time and convert to time_t value.
*
*Purpose:
*       Gets the current date and time and stores it in internal (time_t)
*       format. The time is returned and stored via the pointer passed in
*       timeptr. If timeptr == NULL, the time is only returned, not stored in
*       *timeptr. The internal (time_t) format is the number of seconds since
*       00:00:00, Jan 1 1970 (UTC).
*
*       Note: We cannot use GetSystemTime since its return is ambiguous. In
*       Windows NT, in return UTC. In Win32S, probably also Win32C, it
*       returns local time.
*
*Entry:
*       time_t *timeptr - pointer to long to store time in.
*
*Exit:
*       returns the current time.
*
*Exceptions:
*
*******************************************************************************/

time_t __cdecl time (
        time_t *timeptr
        )
{
        time_t tim;

#ifdef _WIN32

        SYSTEMTIME dt;

        /* ask Win32 for the time, no error possible */

        GetLocalTime(&dt);

        /* convert using our private routine */
        tim = __loctotime_t((int)dt.wYear,
                           (int)dt.wMonth,
                           (int)dt.wDay,
                           (int)dt.wHour,
                           dt.wMinute,
                           dt.wSecond);

#else  /* _WIN32 */
#if defined (_M_MPPC) || defined (_M_M68K)

        DateTimeRec dt;

        GetTime(&dt);
        /* convert using our private routine */
        tim = _gmtotime_t((int)dt.year,
                          (int)dt.month,
                          (int)dt.day,
                          (int)dt.hour,
                          dt.minute,
                          dt.second);

#endif  /* defined (_M_MPPC) || defined (_M_M68K) */
#endif  /* _WIN32 */

        if (timeptr)
                *timeptr = tim;         /* store time if requested */

        return tim;
}

