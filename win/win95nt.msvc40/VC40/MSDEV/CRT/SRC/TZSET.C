/***
*tzset.c - set timezone information and see if we're in daylight time
*
*       Copyright (c) 1985-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _tzset() - set timezone and daylight saving time vars
*
*******************************************************************************/

#ifdef _WIN32


#include <cruntime.h>
#include <ctype.h>
#include <ctime.h>
#include <time.h>
#include <stdlib.h>
#include <internal.h>
#include <mtdll.h>
#include <windows.h>
#include <string.h>
#include <dbgint.h>

/*
 * Pointer to a saved copy of the TZ value obtained in the previous call
 * to tzset() set (if any).
 */
#ifdef DLL_FOR_WIN32S
#define lastTZ      (_GetPPD()->_ppd_lastTZ)
#else  /* DLL_FOR_WIN32S */
static char * lastTZ = NULL;
#endif  /* DLL_FOR_WIN32S */



/***
*void tzset() - sets timezone information and calc if in daylight time
*
*Purpose:
*       Sets the timezone information from the TZ environment variable
*       and then sets _timezone, _daylight, and _tzname. If we're in daylight
*       time is automatically calculated.
*
*Entry:
*       None, reads TZ environment variable.
*
*Exit:
*       sets _daylight, _timezone, and _tzname global vars, no return value
*
*Exceptions:
*
*******************************************************************************/


#ifdef _MT
static void __cdecl _tzset_lk(void);
#else  /* _MT */
#define _tzset_lk _tzset
#endif  /* _MT */

void __cdecl __tzset(void)
{
#ifdef DLL_FOR_WIN32S
        #define first_time  (_GetPPD()->_ppd_tzset_first_time)
#else  /* DLL_FOR_WIN32S */
        static int first_time = 0;
#endif  /* DLL_FOR_WIN32S */

        if ( !first_time ) {

            _mlock( _TIME_LOCK );

            if ( !first_time ) {
                _tzset_lk();
                first_time++;
            }

            _munlock(_TIME_LOCK );

        }
}


#ifdef _MT
void __cdecl _tzset (
        void
        )
{
        _mlock( _TIME_LOCK );

        _tzset_lk();

        _munlock( _TIME_LOCK );
}


static void __cdecl _tzset_lk (

#else  /* _MT */

void __cdecl _tzset (

#endif  /* _MT */

        void
        )
{
        REG1 char *TZ;
        REG2 int negdiff = 0;

        _mlock(_ENV_LOCK);

        /*
         * Fetch the value of the TZ environment variable.
         */
        if ( (TZ = _getenv_lk("TZ")) == NULL ) {
            /*
             * If there is no TZ environment variable, try to use the
             * time zone information from the system.
             */
            TIME_ZONE_INFORMATION tzinfo;

            if ( GetTimeZoneInformation( &tzinfo ) != 0xffffffff ) {
                /*
                 * Derive _timezone value from Bias and StandardBias fields.
                 */
                _timezone = tzinfo.Bias * 60L;

                if ( tzinfo.StandardDate.wMonth != 0 )
                    _timezone += (tzinfo.StandardBias * 60L);

                /*
                 * Check to see if there is a daylight time bias. If so, we
                 * ASSUME it is USA daylight saving time (that is all we
                 * support for rev. one).
                 */
                if ( (tzinfo.DaylightDate.wMonth != 0) &&
                     (tzinfo.DaylightBias != 0) )
                    _daylight = 1;
                else
                    _daylight = 0;

                /*
                 * Remove the default names, but don't try to determine
                 * suitable replacements (NT does not enforce any standard
                 * or convention for timezone naming).
                 */
                *_tzname[0] = '\0';
                *_tzname[1] = '\0';
            }

            /*
             * Time zone information is unavailable, just return.
             */
            _munlock(_ENV_LOCK);
            return;
        }


        if ( (*TZ == '\0') || ((lastTZ != NULL) && (strcmp(TZ, lastTZ) == 0)) )
        {
            /*
             * Either TZ is NULL, pointing to '\0', or is the unchanged
             * from a earlier call (to this function). In any case, there
             * is no work to do, so just return
             */
            _munlock(_ENV_LOCK);
            return;
        }

        /*
         * Update lastTZ
         */
        _free_crt(lastTZ);

        if ((lastTZ = _malloc_crt(strlen(TZ)+1)) == NULL)
        {
            _munlock(_ENV_LOCK);
            return;
        }
        strcpy(lastTZ, TZ);

        _munlock(_ENV_LOCK);

        /*
         * Process TZ value and update _tzname, _timezone and _daylight.
         */

        strncpy(_tzname[0], TZ, 3);

        /*
         * time difference is of the form:
         *
         *      [+|-]hh[:mm[:ss]]
         *
         * check minus sign first.
         */
        if ( *(TZ += 3) == '-' ) {
                negdiff++;
                TZ++;
        }

        /*
         * process, then skip over, the hours
         */
        _timezone = atol(TZ) * 3600L;

        while ( (*TZ == '+') || ((*TZ >= '0') && (*TZ <= '9')) ) TZ++;

        /*
         * check if minutes were specified
         */
        if ( *TZ == ':' ) {
            /*
             * process, then skip over, the minutes
             */
            _timezone += atol(++TZ) * 60L;
            while ( (*TZ >= '0') && (*TZ <= '9') ) TZ++;

            /*
             * check if seconds were specified
             */
            if ( *TZ == ':' ) {
                /*
                 * process, then skip over, the seconds
                 */
                _timezone += atol(++TZ);
                while ( (*TZ >= '0') && (*TZ <= '9') ) TZ++;
            }
        }

        if ( negdiff )
                _timezone = -_timezone;

        /*
         * finally, check for a DST zone suffix
         */
        if ( _daylight = *TZ )
            strncpy(_tzname[1], TZ, 3);
        else
            *_tzname[1] = '\0';

}


/***
*int _isindst(tb) - determine if broken-down time falls in DST
*
*Purpose:
*       Determine if the given broken-down time falls within DST. Only
*       modern, summer-time DST is handled (USA, post 1967).
*
*       This is the rule for years before 1987:
*       a time is in DST iff it is on or after 02:00:00 on the last Sunday
*       in April and before 01:00:00 on the last Sunday in October.
*       This is the rule for years starting with 1987:
*       a time is in DST iff it is on or after 02:00:00 on the first Sunday
*       in April and before 01:00:00 on the last Sunday in October.
*
*Entry:
*       struct tm *tb - structure holding broken-down time value
*
*Exit:
*       1, if time represented is in DST
*       0, otherwise
*
*******************************************************************************/

int __cdecl _isindst (
        REG1 struct tm *tb
        )
{
        int mdays;
        REG2 int yr;
        int critsun;

        /*
         * Handle easy cases.
         *
         * Modern DST was put into effect by Congress in 1967. Also, if the
         * month is before April or after October, it cannot be DST.
         */
        if ( (tb->tm_year < 67) || (tb->tm_mon < 3) || (tb->tm_mon > 9) )
            return(0);

        /*
         * If the month is after April and before October, it must be DST.
         */
        if ( (tb->tm_mon > 3) && (tb->tm_mon < 9) )
            return(1);

        /*
         * Now for the hard part.  Month is April or October; see if date
         * falls between appropriate Sundays.
         */

        /*
         * The objective for years before 1987 (after 1986) is to determine
         * if the day is on or after 2:00 am on the last (first) Sunday in
         * April, or before 1:00 am on the last Sunday in October.
         *
         * We know the year-day (0..365) of the current time structure. We must
         * determine the year-day of the last (first) Sunday in this month,
         * April or October, and then do the comparison.
         *
         * To determine the year-day of the last Sunday, we do the following:
         *        1. Get the year-day of the last day of the current month (Apr
         *           or Oct)
         *        2. Determine the week-day number of #1,
         *           which is defined as 0 = Sun, 1 = Mon, ... 6 = Sat
         *        3. Subtract #2 from #1
         *
         * To determine the year-day of the first Sunday, we do the following:
         *        1. Get the year-day of the 7th day of the current month
         *           (April)
         *        2. Determine the week-day number of #1,
         *           which is defined as 0 = Sun, 1 = Mon, ... 6 = Sat
         *        3. Subtract #2 from #1
         */

        /*
         * First we get #1. The year-days for each month are stored in _days[]
         * they're all off by -1
         */
        if ( ((yr = tb->tm_year) > 86) && (tb->tm_mon == 3) )
            mdays = 7 + _days[tb->tm_mon];
        else
            mdays = _days[tb->tm_mon+1];

        /*
         * if this is a leap-year, add an extra day
         */
        if ( !(yr & 3) )
            mdays++;

        /*
         * mdays now has #1
         */

        /* Now get #2. We know the week-day number of January 1, 1970, which is
         * defined as the constant _BASE_DOW. Add to this the number of elapsed
         * days since then, take the result mod 7, and we have our day number.
         *
         * To obtain #3, we just subtract this from mdays.
         */

        critsun = mdays - ((mdays + 365 * (yr - 70) + ((yr - 1) >> 2) -
                  _LEAP_YEAR_ADJUST + _BASE_DOW) % 7);

        /* Now we know 1 and 3; we're golden: */

        return ( (tb->tm_mon == 3)
                 ? ((tb->tm_yday > critsun) ||
                    ((tb->tm_yday == critsun) && (tb->tm_hour >= 2)))
                 : ((tb->tm_yday < critsun) ||
                    ((tb->tm_yday == critsun) && (tb->tm_hour < 1))) );
}




#else  /* _WIN32 */

#if defined (_M_MPPC) || defined (_M_M68K)


#include <cruntime.h>
#include <ctype.h>
#include <ctime.h>
#include <time.h>
#include <stdlib.h>
#include <internal.h>
#include <string.h>
#include <fltintrn.h>            /* PFV definition */
#include <macos\script.h>
#include <macos\osutils.h>

/* define the entry in initializer table */

#pragma data_seg(".CRT$XIC")

const PFV __pinittime = _inittime;

#pragma data_seg()


/***
*void tzset() - sets timezone information and calc if in daylight time
*
*Purpose:
*       Sets the timezone information from the TZ environment variable
*       and then sets _timezone, _daylight, and _tzname. If we're in daylight
*       time is automatically calculated.
*
*Entry:
*       None, reads TZ environment variable.
*
*Exit:
*       sets _daylight, _timezone, and _tzname global vars, no return value
*
*Exceptions:
*
*******************************************************************************/

void __cdecl _tzset (
        void
        )
{
        REG1 char *TZ;
        char *lastTZ=NULL;
        MachineLocation ml;
        long gmtDelta;
        REG2 int negdiff = 0;

        /*
         * Fetch the value of the TZ environment variable. If there is no TZ
         * environment variable, or if it is trivial, then the timezone
         * information will be taken from the OS.
         */

        if ( (TZ = getenv("TZ")) && (*TZ) ) {
            /*
             * TZ environment variable exists and is non-trivial. See if
             * it is unchanged from a previous _tzset call.
             */
            if ( (lastTZ == NULL) || (strcmp(TZ, lastTZ) != 0) ) {
                /*
                 * TZ has changed, or there has been no prior _tzset call.
                 * Update lastTZ value.
                 */
                free(lastTZ);
                lastTZ = _strdup(TZ);
            }
            else {
                /*
                 * Timezone environment variable hasn't changed since the
                 * last _tzset call, just return.
                 */
                return;

            }
        }
        else {
            /*
             * The TZ environment variable either does not exist, or is
             * trivial. Therefore, timezone information will be obtained
             * from the OS.
             */
            if ( lastTZ != NULL ) {
                free(lastTZ);
                lastTZ = NULL;
            }
            ReadLocation(&ml);
            //get gmtDelta from machinelocation in RAM
            gmtDelta = ml.u.gmtDelta & 0x00ffffff;

            if ((gmtDelta >> 23) & 1) //need to sign extend
                gmtDelta = gmtDelta | 0xff000000;

            //set timezone and daylight
            _timezone = - gmtDelta;
            _daylight = (ml.u.dlsDelta ? 1 : 0);
            *_tzname[0] = '\0';
            *_tzname[1] = '\0';
            return;
        }

        strncpy(_tzname[0], TZ, 3);

        /*
         * time difference is of the form:
         *
         *      [+|-]hh[:mm[:ss]]
         *
         * check minus sign first.
         */
        if ( *(TZ += 3) == '-' ) {
                negdiff++;
                TZ++;
        }

        /*
         * process, then skip over, the hours
         */
        _timezone = atol(TZ) * 3600L;

        while ( (*TZ == '+') || ((*TZ >= '0') && (*TZ <= '9')) ) TZ++;

        /*
         * check if minutes were specified
         */
        if ( *TZ == ':' ) {
            /*
             * process, then skip over, the minutes
             */
            _timezone += atol(++TZ) * 60L;
            while ( (*TZ >= '0') && (*TZ <= '9') ) TZ++;

            /*
             * check if seconds were specified
             */
            if ( *TZ == ':' ) {
                /*
                 * process, then skip over, the seconds
                 */
                _timezone += atol(++TZ);
                while ( (*TZ >= '0') && (*TZ <= '9') ) TZ++;
            }
        }
        if ( negdiff )
                _timezone = -_timezone;

        /*
         * finally, check for a DST zone suffix
         */
        if (*TZ)
                strncpy(_tzname[1], TZ, 3);
        else
                *_tzname[1] = '\0';
        _daylight = *_tzname[1] != '\0';
}

/*
 *  _isindst - Tells whether Xenix-type time value falls under DST
 *
 *  This is the rule for years before 1987:
 *  a time is in DST iff it is on or after 02:00:00 on the last Sunday
 *  in April and before 01:00:00 on the last Sunday in October.
 *  This is the rule for years starting with 1987:
 *  a time is in DST iff it is on or after 02:00:00 on the first Sunday
 *  in April and before 01:00:00 on the last Sunday in October.
 *
 *  ENTRY   tb  - 'time' structure holding broken-down time value
 *
 *  RETURN  1 if time represented is in DST, else 0
 */

int __cdecl _isindst (
        REG1 struct tm *tb
        )
{
        int mdays;
        REG2 int yr;
        int lastsun;

        /* If the month is before April or after October, then we know
         * immediately it can't be DST. */

        if (tb->tm_mon < 3 || tb->tm_mon > 9)
                return(0);

        /* If the month is after April and before October then we know
         * immediately it must be DST. */

        if (tb->tm_mon > 3 && tb->tm_mon < 9)
                return(1);
        /*
         * Now for the hard part.  Month is April or October; see if date
         * falls between appropriate Sundays.
         */

        /*
         * The objective for years before 1987 (after 1986) is to determine
         * if the day is on or after 2:00 am on the last (first) Sunday in
         * April, or before 1:00 am on the last Sunday in October.
         *
         * We know the year-day (0..365) of the current time structure. We must
         * determine the year-day of the last (first) Sunday in this month,
         * April or October, and then do the comparison.
         *
         * To determine the year-day of the last Sunday, we do the following:
         *      1. Get the year-day of the last day of the current month (Apr
         *         or Oct)
         *      2. Determine the week-day number of #1,
         *         which is defined as 0 = Sun, 1 = Mon, ... 6 = Sat
         *      3. Subtract #2 from #1
         *
         * To determine the year-day of the first Sunday, we do the following:
         *      1. Get the year-day of the 7th day of the current month (April)
         *      2. Determine the week-day number of #1,
         *         which is defined as 0 = Sun, 1 = Mon, ... 6 = Sat
         *      3. Subtract #2 from #1
         */

        yr = tb->tm_year + 1900;    /* To see if this is a leap-year */

        /* First we get #1. The year-days for each month are stored in _days[]
         * they're all off by -1 */

        if (yr > 1986 && tb->tm_mon == 3)
                mdays = 7 + _days[tb->tm_mon];
        else
                mdays = _days[tb->tm_mon+1];

        /* if this is a leap-year, add an extra day */
        if (!(yr & 3))
                mdays++;

        /* mdays now has #1 */

        yr = tb->tm_year - 70;

        /* Now get #2.  We know the week-day number of the beginning of the
         * epoch, Jan. 1, 1970, which is defined as the constant _BASE_DOW.  We
         * then add the number of days that have passed from _BASE_DOW to the day
         * of #2
         *      mdays + 365 * yr
         * correct for the leap years which intervened
         *      + (yr + 1)/ 4
         * and take the result mod 7, except that 0 must be mapped to 7.
         * This is #2, which we then subtract from #1, mdays
         */

        lastsun = mdays - ((mdays + 365*yr + ((yr+1)/4) + _BASE_DOW) % 7);

        /* Now we know 1 and 3; we're golden: */

        return (tb->tm_mon==3
                ? (tb->tm_yday > lastsun ||
                (tb->tm_yday == lastsun && tb->tm_hour >= 2))
                : (tb->tm_yday < lastsun ||
                (tb->tm_yday == lastsun && tb->tm_hour < 1)));
}




#endif  /* defined (_M_MPPC) || defined (_M_M68K) */

#endif  /* _WIN32 */
