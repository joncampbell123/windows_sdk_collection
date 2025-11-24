/*
 *  DT.C
 *
 *  Win32 date and time support for Win16.
 *  Most borrowed from the Win32 library.
 *  Some implemented specifically for MAPI.
 *
 *  Copyright 1993-1995 Microsoft Corporation. All Rights Reserved.
 */

#pragma warning(disable:4001)   /* single line comments */
#pragma warning(disable:4054)   /* cast function pointer to data pointer */
#pragma warning(disable:4100)   /* unreferenced formal parameter */
#pragma warning(disable:4127)   /* conditional expression is constant */
#pragma warning(disable:4201)   /* nameless struct/union */
#pragma warning(disable:4204)   /* non-constant aggregate initializer */
#pragma warning(disable:4209)   /* benign typedef redefinition */
#pragma warning(disable:4214)   /* bit field types other than int */
#pragma warning(disable:4505)   /* unreferenced local function removed */
#pragma warning(disable:4514)   /* unreferenced inline function removed */
#pragma warning(disable:4702)   /* unreachable code */
#pragma warning(disable:4704)   /* inline assembler turns off global optimizer */
#pragma warning(disable:4705)   /* statement has no effect */
#pragma warning(disable:4706)   /* assignment within conditional expression */
#pragma warning(disable:4710)   /* function not expanded */
#pragma warning(disable:4115)   /* named type def in parens */

#include <windows.h>
#pragma warning(disable:4001)   /* single line comments */
#include <windowsx.h>
#include <mapiwin.h>
#include <compobj.h>
#include <mapidbg.h>
#include <mapidefs.h>
#include <mapiutil.h>
#include <mapiperf.h>

#include <_mapiwin.h>
#include <memory.h>
#include <_memcpy.h>

#if defined(WIN16)

#pragma SEGMENT(MAPI_Conv)

#pragma warning (disable: 4704)

BOOL FValidBias(LONG lBias);
BOOL FValidSysTime(SYSTEMTIME FAR *pst, BOOL fRelativeOK);
BOOL FValidTimeZoneInformation(TIME_ZONE_INFORMATION FAR *ptz);

/*** From TIME.C */

/*  The following two tables map a day offset within a year to the month */
/*  containing the day.  Both tables are zero based.  For example, day */
/*  offset of 0 to 30 map to 0 (which is Jan). */

UCHAR LeapYearDayToMonth[366] = {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* January */
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,        /* February */
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  /* March */
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,     /* April */
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  /* May */
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,     /* June */
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  /* July */
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  /* August */
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,     /* September */
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  /* October */
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,     /* November */
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11}; /* December */

UCHAR NormalYearDayToMonth[365] = {
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  /* January */
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,           /* February */
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  /* March */
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,     /* April */
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,  /* May */
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,     /* June */
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,  /* July */
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,  /* August */
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,     /* September */
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  /* October */
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,     /* November */
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11}; /* December */

/*  The following two tables map a month index to the number of days preceding */
/*  the month in the year.  Both tables are zero based.  For example, 1 (Feb) */
/*  has 31 days preceding it.  To help calculate the maximum number of days */
/*  in a month each table has 13 entries, so the number of days in a month */
/*  of index i is the table entry of i+1 minus the table entry of i. */

SHORT LeapYearDaysPrecedingMonth[13] = {
    0,                                 /* January */
    31,                                /* February */
    31+29,                             /* March */
    31+29+31,                          /* April */
    31+29+31+30,                       /* May */
    31+29+31+30+31,                    /* June */
    31+29+31+30+31+30,                 /* July */
    31+29+31+30+31+30+31,              /* August */
    31+29+31+30+31+30+31+31,           /* September */
    31+29+31+30+31+30+31+31+30,        /* October */
    31+29+31+30+31+30+31+31+30+31,     /* November */
    31+29+31+30+31+30+31+31+30+31+30,  /* December */
    31+29+31+30+31+30+31+31+30+31+30+31};

SHORT NormalYearDaysPrecedingMonth[13] = {
    0,                                 /* January */
    31,                                /* February */
    31+28,                             /* March */
    31+28+31,                          /* April */
    31+28+31+30,                       /* May */
    31+28+31+30+31,                    /* June */
    31+28+31+30+31+30,                 /* July */
    31+28+31+30+31+30+31,              /* August */
    31+28+31+30+31+30+31+31,           /* September */
    31+28+31+30+31+30+31+31+30,        /* October */
    31+28+31+30+31+30+31+31+30+31,     /* November */
    31+28+31+30+31+30+31+31+30+31+30,  /* December */
    31+28+31+30+31+30+31+31+30+31+30+31};

/*  The following definitions and declarations are some important constants */
/*  used in the time conversion routines */

/*  This is the week day that January 1st, 1601 fell on (a Monday) */

#define WEEKDAY_OF_1601                  1

/*  These are the magic numbers needed to do our extended division.  The
 *  only numbers we ever need to divide by are
 *
 *      10,000 = convert 100ns tics to millisecond tics
 *
 *      10,000,000 = convert 100ns tics to one second tics
 *
 *      86,400,000 = convert Millisecond tics to one day tics
 */

FILETIME Magic10000    = {0xe219652c, 0xd1b71758};
#define SHIFT10000                       13

FILETIME Magic10000000 = {0xe57a42bd, 0xd6bf94d5};
#define SHIFT10000000                    23

FILETIME Magic86400000 = {0xfa67b90e, 0xc6d750eb};
#define SHIFT86400000                    26

/*  To make the code more readable we'll also define some macros to */
/*  do the actual division for us */

#define Convert100nsToMilliseconds(TIME)            \
    (FtDivFtBogus ((TIME), Magic10000, SHIFT10000))

#define ConvertMillisecondsTo100ns(MILLISECONDS)    \
    (FtMulDw (10000, (MILLISECONDS)))

#define Convert100nsToSeconds(TIME)                 \
    (FtDivFtBogus ((TIME), Magic10000000, SHIFT10000000))

#define ConvertSecondsTo100ns(SECONDS)              \
    (FtMulDw (10000000, (SECONDS)))

#define ConvertMillisecondsToDays(TIME)             \
    (FtDivFtBogus ((TIME), Magic86400000, SHIFT86400000))

#define ConvertDaysToMilliseconds(DAYS)             \
    (FtMulDwDw ((DAYS), 86400000))

/*
 *  ULONG
 *  ElapsedDaysToYears (ULONG ElapsedDays);
 *
 *  To be completely true to the Gregorian calendar the equation to
 *  go from days to years is really
 *
 *      ElapsedDays / 365.2425
 *
 *  But because we are doing the computation in ulong integer arithmetic
 *  and the TIME variable limits the number of expressible days to around
 *  11,000,000 we use the following computation
 *
 *      (ElapsedDays * 128 + 127) / (365.2425 * 128)
 *
 *  which will be off from the Gregorian calendar in about 150,000 years
 *  but that doesn't really matter because TIME can only express around
 *  30,000 years
 */

#define ElapsedDaysToYears(DAYS) (((DAYS) * 128 + 127) / 46751)

/*
 *
 *  ULONG
 *  NumberOfLeapYears (ULONG ElapsedYears);
 *
 *  The number of leap years is simply the number of years divided by 4
 *  minus years divided by 100 plus years divided by 400.  This says
 *  that every four years is a leap year except centuries, and the
 *  exception to the exception is the quadricenturies
 */

#define NumberOfLeapYears(YEARS) \
        (((YEARS) / 4) - ((YEARS) / 100) + ((YEARS) / 400))

/*
 *  ULONG
 *  ElapsedYearsToDays (ULONG ElapsedYears);
 *
 *  The number of days contained in elapsed years is simply the number
 *  of years times 365 (because every year has at least 365 days) plus
 *  the number of leap years there are (i.e., the number of 366 days years)
 */

#define ElapsedYearsToDays(YEARS) (((YEARS) * 365) + NumberOfLeapYears(YEARS))

/*
 *  BOOLEAN
 *  IsLeapYear (ULONG ElapsedYears);
 *
 *  If it is an even 400 or a non century leapyear then the
 *  answer is true otherwise it's false
 */

#define IsLeapYear(YEARS)                           \
    ((((YEARS) % 400 == 0) ||                       \
     ((YEARS) % 100 != 0) && ((YEARS) % 4 == 0))    \
    ? TRUE : FALSE)
/*
 *  ULONG
 *  MaxDaysInMonth (ULONG Year, ULONG Month);
 *
 *  The maximum number of days in a month depend on the year and month.
 *  It is the difference between the days to the month and the days
 *  to the following month
 */

#define MaxDaysInMonth(YEAR,MONTH)                                          \
    (IsLeapYear(YEAR) ?                                                     \
        LeapYearDaysPrecedingMonth[(MONTH) + 1] -                           \
                                    LeapYearDaysPrecedingMonth[(MONTH)]     \
    :                                                                       \
        NormalYearDaysPrecedingMonth[(MONTH) + 1] -                         \
                                    NormalYearDaysPrecedingMonth[(MONTH)])


/*  Internal Support routine */

VOID
TimeToDaysAndFraction (const FILETIME FAR * Time,
    ULONG FAR * ElapsedDays,
    ULONG FAR * Milliseconds)

/*++

Routine Description:

    This routine converts an input 64-bit time value to the number
    of total elapsed days and the number of milliseconds in the
    partial day.

Arguments:

    Time - Supplies the input time to convert from

    ElapsedDays - Receives the number of elapsed days

    Milliseconds - Receives the number of milliseconds in the partial day

Return Value:

    None

--*/

{
    FILETIME TotalMilliseconds;
    FILETIME Temp;

    /*  Convert the input time to total milliseconds */

    TotalMilliseconds = Convert100nsToMilliseconds( *Time );

    /*  Convert milliseconds to total days */

    Temp = ConvertMillisecondsToDays( TotalMilliseconds );

    /*  Set the elapsed days from temp, we've divided it enough so that */
    /*  the high part must be zero. */

    *ElapsedDays = Temp.dwLowDateTime;

    /*  Calculate the exact number of milliseconds in the elapsed days */
    /*  and subtract that from the total milliseconds to figure out */
    /*  the number of milliseconds left in the partial day */

    Temp = ConvertDaysToMilliseconds( *ElapsedDays );

    Temp = FtSubFt( TotalMilliseconds, Temp );

    /*  Set the fraction part from temp, the total number of milliseconds in */
    /*  a day guarantees that the high part must be zero. */

    *Milliseconds = Temp.dwLowDateTime;

    /*  And return to our caller */

    return;
}

/*  Internal Support routine */

VOID
DaysAndFractionToTime (ULONG ElapsedDays,
    ULONG Milliseconds,
    FILETIME FAR * Time)

/*++

Routine Description:

    This routine converts an input elapsed day count and partial time
    in milliseconds to a 64-bit time value.

Arguments:

    ElapsedDays - Supplies the number of elapsed days

    Milliseconds - Supplies the number of milliseconds in the partial day

    Time - Receives the output time to value

Return Value:

    None

--*/

{
    FILETIME Temp;
    FILETIME Temp2;

    /*  Calculate the exact number of milliseconds in the elapsed days. */

    Temp = ConvertDaysToMilliseconds( ElapsedDays );

    /*  Convert milliseconds to a large integer */

    Temp2.dwLowDateTime = Milliseconds;
    Temp2.dwHighDateTime = 0;

    /*  add milliseconds to the whole day milliseconds */

    Temp = FtAddFt( Temp, Temp2 );

    /*  Finally convert the milliseconds to 100ns resolution */

    *Time = ConvertMillisecondsTo100ns( Temp );

    /*  and return to our caller */

    return;
}



BOOL __export WINAPI
FileTimeToSystemTime (const FILETIME FAR * Time,
    SYSTEMTIME FAR *TimeFields)

/*++

Routine Description:

    This routine converts an input 64-bit TIME variable to its corresponding
    time field record.  It will tell the caller the year, month, day, hour,
    minute, second, millisecond, and weekday corresponding to the input time
    variable.

Arguments:

    Time - Supplies the time value to interpret

    TimeFields - Receives a value corresponding to Time

Return Value:

    None

--*/

{
    ULONG Years;
    ULONG Month;
    ULONG Days;

    ULONG Hours;
    ULONG Minutes;
    ULONG Seconds;
    ULONG Milliseconds;
    
    /* parameter validation */
    
    AssertSz( Time && !IsBadReadPtr( Time, sizeof( FILETIME ) ),
            "Time fails address check" );
            
    AssertSz( TimeFields && !IsBadWritePtr( TimeFields, sizeof( SYSTEMTIME ) ),
            "TimeFields fails address check" );

    /*  First divide the input time 64 bit time variable into */
    /*  the number of whole days and part days (in milliseconds) */

    TimeToDaysAndFraction( Time, &Days, &Milliseconds );

    /*  Compute which weekday it is and save it away now in the output */
    /*  variable.  We add the weekday of the base day to bias our computation */
    /*  which means that if one day has elapsed then we the weekday we want */
    /*  is the Jan 2nd, 1601. */

    TimeFields->wDayOfWeek= (SHORT)((Days + WEEKDAY_OF_1601) % 7);

    /*  Calculate the number of whole years contained in the elapsed days */
    /*  For example if Days = 500 then Years = 1 */

    Years = ElapsedDaysToYears( Days );

    /*  And subtract the number of whole years from our elapsed days */
    /*  For example if Days = 500, Years = 1, and the new days is equal */
    /*  to 500 - 365 (normal year). */

    Days = Days - ElapsedYearsToDays( Years );

    /*  Now test whether the year we are working on (i.e., The year */
    /*  after the total number of elapsed years) is a leap year */
    /*  or not. */

    if (IsLeapYear( Years + 1 )) {

        /*  The current year is a leap year, so figure out what month */
        /*  it is, and then subtract the number of days preceding the */
        /*  month from the days to figure out what day of the month it is */

        Month = LeapYearDayToMonth[Days];
        Days = Days - LeapYearDaysPrecedingMonth[Month];

    } else {

        /*  The current year is a normal year, so figure out the month */
        /*  and days as described above for the leap year case */

        Month = NormalYearDayToMonth[Days];
        Days = Days - NormalYearDaysPrecedingMonth[Month];

    }

    /*  Now we need to compute the elapsed hour, minute, second, milliseconds */
    /*  from the millisecond variable.  This variable currently contains */
    /*  the number of milliseconds in our input time variable that did not */
    /*  fit into a whole day.  To compute the hour, minute, second part */
    /*  we will actually do the arithmetic backwards computing milliseconds */
    /*  seconds, minutes, and then hours.  We start by computing the */
    /*  number of whole seconds left in the day, and then computing */
    /*  the millisecond remainder. */

    Seconds = Milliseconds / 1000;
    Milliseconds = Milliseconds % 1000;

    /*  Now we compute the number of whole minutes left in the day */
    /*  and the number of remainder seconds */

    Minutes = Seconds / 60;
    Seconds = Seconds % 60;

    /*  Now compute the number of whole hours left in the day */
    /*  and the number of remainder minutes */

    Hours = Minutes / 60;
    Minutes = Minutes % 60;

    /*  As our final step we put everything into the time fields */
    /*  output variable */

    TimeFields->wYear         = (SHORT)(Years + 1601);
    TimeFields->wMonth        = (SHORT)(Month + 1);
    TimeFields->wDay          = (SHORT)(Days + 1);
    TimeFields->wHour         = (SHORT)Hours;
    TimeFields->wMinute       = (SHORT)Minutes;
    TimeFields->wSecond       = (SHORT)Seconds;
    TimeFields->wMilliseconds = (SHORT)Milliseconds;

    /*  and return to our caller */

    return TRUE;
}

BOOL __export WINAPI
SystemTimeToFileTime (const SYSTEMTIME FAR *TimeFields,
    FILETIME FAR * Time)

/*++

Routine Description:

    This routine converts an input Time Field variable to a 64-bit NT time
    value.  It ignores the WeekDay of the time field.

Arguments:

    TimeFields - Supplies the time field record to use

    Time - Receives the NT Time corresponding to TimeFields

Return Value:

    BOOLEAN - TRUE if the Time Fields is well formed and within the
        range of time expressible by TIME and FALSE otherwise.

--*/

{
    ULONG Year;
    ULONG Month;
    ULONG Day;
    ULONG Hour;
    ULONG Minute;
    ULONG Second;
    ULONG Milliseconds;

    ULONG ElapsedDays;
    ULONG ElapsedMilliseconds;

    /* parameter validation */
    
    AssertSz( TimeFields && !IsBadReadPtr( TimeFields, sizeof( SYSTEMTIME ) ),
            "TimeFields fails address check" );
            
    AssertSz( Time && !IsBadWritePtr( Time, sizeof( FILETIME ) ),
            "Time fails address check" );
            
    /*  Load the time field elements into local variables.  This should */
    /*  ensure that the compiler will only load the input elements */
    /*  once, even if there are alias problems.  It will also make */
    /*  everything (except the year) zero based.  We cannot zero base the */
    /*  year because then we can't recognize cases where we're given a year */
    /*  before 1601. */

    Year         = TimeFields->wYear;
    Month        = TimeFields->wMonth - 1;
    Day          = TimeFields->wDay - 1;
    Hour         = TimeFields->wHour;
    Minute       = TimeFields->wMinute;
    Second       = TimeFields->wSecond;
    Milliseconds = TimeFields->wMilliseconds;

    /*  Check that the time field input variable contains */
    /*  proper values. */

    if ((Year < 1601)                        ||
        (Month > 11)                         ||
        ((SHORT)Day >= MaxDaysInMonth(Year, Month)) ||
        (Hour > 23)                          ||
        (Minute > 59)                        ||
        (Second > 59)                        ||
        (Milliseconds > 999)) {

        return FALSE;

    }

    /*  Compute the total number of elapsed days represented by the */
    /*  input time field variable */

    ElapsedDays = ElapsedYearsToDays( Year - 1601 );

    if (IsLeapYear( Year - 1600 )) {

        ElapsedDays += LeapYearDaysPrecedingMonth[ Month ];

    } else {

        ElapsedDays += NormalYearDaysPrecedingMonth[ Month ];

    }

    ElapsedDays += Day;

    /*  Now compute the total number of milliseconds in the fractional */
    /*  part of the day */

    ElapsedMilliseconds = (((Hour*60) + Minute)*60 + Second)*1000 + Milliseconds;

    /*  Given the elapsed days and milliseconds we can now build */
    /*  the output time variable */

    DaysAndFractionToTime( ElapsedDays, ElapsedMilliseconds, Time );

    /*  And return to our caller */

    return TRUE;
}



BOOL
__export WINAPI
FileTimeToDosDateTime(const FILETIME FAR * lpFileTime,
    LPWORD lpFatDate,
    LPWORD lpFatTime)

/*++

Routine Description:

    This function converts a 64-bit file time into DOS date and time value
    which is represented as two 16-bit unsigned integers.

    Since the DOS date format can only represent dates between 1/1/80 and
    12/31/2099, this conversion can fail if the input file time is outside
    of this range.

Arguments:

    lpFileTime - Supplies the 64-bit file time to convert to DOS date and
        time format.

    lpFatDate - Returns the 16-bit DOS representation of date.

    lpFatTime - Returns the 16-bit DOS representation of time.

Return Value:

    TRUE - The file time was successfully converted.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    SYSTEMTIME  TimeFields;
    BOOL        fRet = TRUE;

    /* parameter validation */
    
    AssertSz( lpFileTime && !IsBadReadPtr( lpFileTime, sizeof( FILETIME ) ),
            "lpFileTime fails address check" );
            
    AssertSz( lpFatDate && !IsBadWritePtr( lpFatDate, sizeof( LPWORD ) ),
            "lpFatDate fails address check" );

    AssertSz( lpFatTime && !IsBadWritePtr( lpFatTime, sizeof( LPWORD ) ),
            "lpFatTime fails address check" );
            
    FileTimeToSystemTime(lpFileTime, &TimeFields);

    if (TimeFields.wYear < 1980 || TimeFields.wYear > 2099) {
    /* BaseSetLastNTError(STATUS_INVALID_PARAMETER); */
    fRet = FALSE;
    goto Exit;
        }

    *lpFatDate = (WORD)( ((USHORT)(TimeFields.wYear-(SHORT)1980) << 9) |
                         ((USHORT)TimeFields.wMonth << 5) |
                         (USHORT)TimeFields.wDay
                       );

    *lpFatTime = (WORD)( ((USHORT)TimeFields.wHour << 11) |
                         ((USHORT)TimeFields.wMinute << 5) |
                         ((USHORT)TimeFields.wSecond >> 1)
                       );

Exit:
    return fRet;
}


BOOL
__export WINAPI
DosDateTimeToFileTime(
    WORD wFatDate,
    WORD wFatTime,
    FILETIME FAR * lpFileTime
    )

/*++

Routine Description:

    This function converts a DOS date and time value, which is
    represented as two 16-bit unsigned integers, into a 64-bit file
    time.

Arguments:

    lpFatDate - Supplies the 16-bit DOS representation of date.

    lpFatTime - Supplies the 16-bit DOS representation of time.

    lpFileTime - Returns the 64-bit file time converted from the DOS
        date and time format.

Return Value:

    TRUE - The Dos date and time were successfully converted.

    FALSE - The operation failed. Extended error status is available
        using GetLastError.

--*/
{
    SYSTEMTIME  TimeFields;
    BOOL        fRet;

    /* parameter validation */

    AssertSz( lpFileTime && !IsBadWritePtr( lpFileTime, sizeof( FILETIME ) ),
            "lpFileTime fails address check" );
            
            
    TimeFields.wYear        = (SHORT)(((wFatDate & 0xFE00) >> 9) + 1980);
    TimeFields.wMonth        = (SHORT)((wFatDate & 0x01E0) >> 5);
    TimeFields.wDay          = (SHORT)((wFatDate & 0x001F) >> 0);
    TimeFields.wHour         = (SHORT)((wFatTime & 0xF800) >> 11);
    TimeFields.wMinute       = (SHORT)((wFatTime & 0x07E0) >>  5);
    TimeFields.wSecond      = (SHORT)((wFatTime & 0x001F) <<  1);
    TimeFields.wMilliseconds = 0;

    fRet = SystemTimeToFileTime(&TimeFields, lpFileTime);
    return (fRet);
}

LONG
__export WINAPI
CompareFileTime(
    const FILETIME FAR * t1,
    const FILETIME FAR * t2
    )

/*++

Routine Description:

    This function compares two 64-bit file times.

Arguments:

    lpFileTime1 - pointer to a 64-bit file time.

    lpFileTime2 - pointer to a 64-bit file time.

Return Value:

    -1 - *lpFileTime1 <  *lpFileTime2

     0 - *lpFileTime1 == *lpFileTime2

    +1 - *lpFileTime1 >  *lpFileTime2

--*/

{
    LONG Ret;

    /* parameter validation */
    
    AssertSz( t1 && !IsBadReadPtr( t1, sizeof( FILETIME ) ),
            "t1 fails address check" );
            
    AssertSz( t2 && !IsBadReadPtr( t2, sizeof( FILETIME ) ),
            "t2 fails address check" );

    /* common case is high words equal, test for that first */
    if (t1->dwHighDateTime == t2->dwHighDateTime) {

    if (t1->dwLowDateTime < t2->dwLowDateTime)
        Ret = -1;
    else if (t1->dwLowDateTime > t2->dwLowDateTime)
        Ret = 1;
    else
        Ret = 0;
    } else if (t1->dwHighDateTime < t2->dwHighDateTime)
    Ret = -1;
    else        /* since high words aren't equal, */
    Ret = 1;    /* and t1 isn't less than t2, it must be greater */

    return (Ret);
}

/*
 *  Dynamic global variables involved in GMT <=> local time conversions.
 *
 *  fDaylight:
 *      0 if we're in standard time
 *      1 if were in daylight savings time
    -1 if nothing has been initialized yet
 *
 *  ftStandardBias:
 *      Delta between GMT and local standard time.
 *
 *  ftDaylightBias:
 *      Delta between GMT and local daylight time.
 *
 *  __tz:
 *
 *  stUSST:
 *      U.S. rule for beginning of standard time (last Sunday in
 *      October)
 *
 *  stUSDT:
 *      U.S. rule for beginning of daylight savings time (first
 *      Sunday in April)
 */
int         fDaylight           = -1;
FILETIME    ftStandardBias      = { 0xFFFFFFFF, 0xFFFFFFFF };
FILETIME    ftDaylightBias      = { 0xFFFFFFFF, 0xFFFFFFFF };
TIME_ZONE_INFORMATION __tz      = { 0, {0}, {0}, 0, {0}, {0}, 0 };
SYSTEMTIME  stUSST              = { 0, 10, 0, 5, 2, 0, 0, 0 };
SYSTEMTIME  stUSDT              = { 0, 4,  0, 1, 2, 0, 0, 0 };


/*
 *  Initializes the global variables related to the local timezone,
 *  and detects ST <=> DST transition.
 */
void
CheckTimezone(SYSTEMTIME FAR *pst)
{
    int         fDSTNow;
    FILETIME    ft;
    SYSTEMTIME  st;
    DWORD       dw;
    LONG        l;

    /* validate parameters */
    
    AssertSz( !pst || !IsBadReadPtr( pst, sizeof( SYSTEMTIME ) ),
            "pst fails address check" );

    DOSTime(&st);

    if (fDaylight == -1)
        (void) GetTimeZoneInformation(&__tz);

    fDSTNow = FDSTOfSystemTime(st, &__tz);

    /*  This will kick in, usually, only once (when initializing). */
    /*  Rarely, when we're running while the transition to or from */
    /*  DST happens, it will kick in a second time. */

    if (fDaylight != fDSTNow)
    {
        fDaylight = fDSTNow;

        /*  Get the local timezone from the environment. */
        dw = GetTimeZoneInformation(&__tz);

        /*  Note: the Win32 TIME_ZONE_INFORMATION structure */
        /*  specifies the bias from GMT as positive for the */
        /*  Western hemisphere. Our code *adds* the bias to make */
        /*  local time from GMT, so we have to negate what we get. */

        /*  Set up the standard time variables. */
        l = __tz.Bias + __tz.StandardBias;
        ft.dwLowDateTime = 60 * (l >= 0 ? l : -l);
        ft.dwHighDateTime = 0L;
        ft = ConvertSecondsTo100ns(ft);
        ftStandardBias = (l >= 0 ? FtNegFt(ft) : ft);

        /*  Set up the daylight time variables. */
        l = __tz.Bias + __tz.DaylightBias;
        ft.dwLowDateTime = 60 * (l >= 0 ? l : -l);
        ft.dwHighDateTime = 0L;
        ft = ConvertSecondsTo100ns(ft);
        ftDaylightBias = (l >= 0 ? FtNegFt(ft) : ft);
    }

    if (pst)
        *pst = st;
}


/*
 *  Fills in a TIME_ZONE_INFORMATION structure based on the TZ
 *  environment variable. Returns TRUE if the variable was found
 *  and parsed OK, otherwise FALSE.
 *
 *  Daylight savings bias, start and end date follow U.S. rules
 *  (hardcoded), i.e. DST runs from the first Sunday in April
 *  through the last Sunday in October.
 *
 *  //$ BUG? reported not to work properly under the NT WoW subsystem.
 *
 */
BOOL
FParseTZ(LPTIME_ZONE_INFORMATION ptz)
{
    LPSTR   sz;
    LPSTR   szStandardName;
    int     h = 0;
    int     m = 0;
    BOOL    fEast = FALSE;

    Assert(!IsBadWritePtr(ptz, sizeof(TIME_ZONE_INFORMATION)));
    ZeroMemory(ptz, sizeof(TIME_ZONE_INFORMATION));

    /*  Find the value of the TZ environment variable */
    sz = GetDOSEnvironment();
    while (sz && *sz)
    {
        if (lstrlen(sz) > 5 &&
            (sz[0] == 't' || sz[0] == 'T') &&
            (sz[1] == 'z' || sz[1] == 'Z') &&
            sz[2] == '=')
        {
            /*  Skip over "TZ=" */
            sz += 3;
            break;
        }
        sz += lstrlen(sz) + 1;
    }

    if (!sz || !*sz)
        return FALSE;       /*  Not found. */

    /*  sz now points at the value of the variable. */
    /*  Extract the numeric part. */
    szStandardName = sz;
    while (*sz && *sz != '+' && *sz != '-' && (*sz < '0' || *sz > '9'))
        ++sz;

    /*  Parse out the standard timezone name */
    if (sz > szStandardName)
        MemCopy(ptz->StandardName, szStandardName, sz - szStandardName);

    /*  Parse out the sign */
    if (*sz == '-')
    {
        fEast = TRUE;
        ++sz;
    }
    else if (*sz == '+')
        ++sz;

    /*  Parse out the hours */
    while (*sz >= '0' && *sz <= '9')
    {
        h = h * 10 + (*sz - '0');
        ++sz;
    }

    /*  Parse out the minutes */
    if (*sz == ':')
    {
        while (*sz >= '0' && *sz <= '9')
        {
            m = m * 10 + (*sz - '0');
            ++sz;
        }
    }

    /*  Parse out the daylight timezone name */
    if (*sz)
        lstrcpy(ptz->DaylightName, sz);

    /*  Calculate the bias in minutes (still unsigned) */
    m += h * 60;

    /*  Populate the structure. DST rules are U.S. rules. */
    ptz->Bias = (fEast ? -m : m);
    ptz->StandardBias = 0;
    ptz->DaylightBias = -60;
    MemCopy((LPBYTE)&ptz->StandardDate, (LPBYTE)&stUSST, sizeof(SYSTEMTIME));
    MemCopy((LPBYTE)&ptz->DaylightDate, (LPBYTE)&stUSDT, sizeof(SYSTEMTIME));
    return TRUE;
}

void
DOSTime(SYSTEMTIME FAR *pst)
{
    WORD segst = SELECTOROF(pst);
    WORD offst = OFFSETOF(pst);

    /*  Get the current date from DOS. */
    _asm {
        push ds

        mov ax, segst               ; point ds:bx at the SYSTEMTIME
        mov ds, ax
        mov bx, offst

        mov ah, 2Ah                 ; get date
        call DOS3Call               ;
        mov ah, 0
        mov [bx]SYSTEMTIME.wDayOfWeek,ax        ; 0 = Sunday
        mov [bx]SYSTEMTIME.wYear, cx            ; 1980 forward
        mov al, dh
        mov [bx]SYSTEMTIME.wMonth, ax           ; 1 = January
        mov al, dl
        mov [bx]SYSTEMTIME.wDay, ax         ; 1 through 31

        mov ah, 2Ch                 ; get time
        call DOS3Call               ;
        mov ah, 0
        mov al, ch
        mov [bx]SYSTEMTIME.wHour, ax            ; 0 - 23
        mov al, cl
        mov [bx]SYSTEMTIME.wMinute, ax      ; 0 - 59
        mov al, dh
        mov [bx]SYSTEMTIME.wSecond, ax      ; 0 - 59
        mov al, dl                          ; 0 - 99
        mov [bx]SYSTEMTIME.wMilliseconds, ax    ; hundredths

        pop ds
    }

    pst->wMilliseconds *= 10;
}

/*
 *  Interprets a SYSTEMTIME structure that defines the begin date
 *  and time for DST or ST, according to the Win32 rules (see the
 *  entry for TIME_ZONE_INFORMATION in the Win32 docs).
 *
 *  Returns a FILETIME equal to the begin date/time for the year
 *  specified in stTarget.
 */
FILETIME
FtBegin(SYSTEMTIME stTarget, SYSTEMTIME stBegin)
{
    FILETIME    ft;
    WORD        wDayOfWeek;
    WORD        wDayOf1;
    WORD        iWeek;
    WORD        wDay;
    WORD        wLastDay;
    BOOL        fFive = FALSE;

    if (stBegin.wYear == 0)
    {
        /*  It's a relative time. */
        /*  Get the weekday for the first of the month, and use that */
        /*  to compute the date we need. */

        /*  Remember the magic values: the weekday and week ordinal. */
        /*  A week ordinal of 5 means the last such weekday in the month. */
        wDayOfWeek = stBegin.wDayOfWeek;
        iWeek = stBegin.wDay;
        if (iWeek == 5)
        {
            fFive = TRUE;
            iWeek = 4;      /*  every month has 4 of each day */
        }

        /*  Scope out the first day of the month. */
        /*  Tricky: converting to filetime and back gets us the day of week. */
        stBegin.wYear = stTarget.wYear;
        stBegin.wDay = 1;
        SideAssert(SystemTimeToFileTime(&stBegin, &ft));
        SideAssert(FileTimeToSystemTime(&ft, &stBegin));
        wDayOf1 = stBegin.wDayOfWeek;

        /*  Compute the day corresponding to our week and weekday. */
        wDay = 1 + (7 * iWeek) + (wDayOfWeek - wDayOf1);
        wLastDay = (WORD) MaxDaysInMonth(stBegin.wYear, stBegin.wMonth-1);
        if (wDay > wLastDay)
            wDay -= 7;
        Assert(wDay < 31);
        if (fFive && (wDay + 7 <= wLastDay))
            wDay += 7;

        /*  Re-convert to filetime */
        stBegin.wDay = wDay;
        SideAssert(SystemTimeToFileTime(&stBegin, &ft));
    }
    else
    {
        /*  It's an absolute date and time. */
        /*  Just substitute the current year and return it. */
        /* //$  SPEC Does the absolute year mark a cutoff of some sort? */
        /* //$  e.g. no DST prior to 1986 (when the current rules went */
        /* //$  into effect) ? */
        stBegin.wYear = stTarget.wYear;
        SideAssert(SystemTimeToFileTime(&stBegin, &ft));
    }

    return ft;
}

/*
 *  Determines whether 'st' falls in daylight savings or standard
 *  time, according to 'ptz'.
 */
BOOL
FDSTOfSystemTime(SYSTEMTIME st, LPTIME_ZONE_INFORMATION ptz)
{
    FILETIME    ft;
    FILETIME    ftDT;
    FILETIME    ftST;
    int         n0;
    int         n1;
    int         n2;

    /* if the month for standard time is 0, no DST */
    if (ptz->StandardDate.wMonth == 0)
        return FALSE;

    /*  If we don't know anything about DST, say we're not in DST. */
    if (ptz->StandardBias == ptz->DaylightBias)
        return FALSE;

    /*  Compute begin dates for DT and ST in the year we're interested in. */
    /* //$  BUG Adjust for bias? Target time is local. */
    ftST = FtBegin(st, ptz->StandardDate);
    ftDT = FtBegin(st, ptz->DaylightDate);
    n0 = (int) CompareFileTime(&ftDT, &ftST);
    if (n0 == 0)
        /*  pathological: start of DT == start of ST. Say no. */
        return FALSE;
    /*  n0 ==  1 if DT begins after ST */
    /*  n0 == -1 if ST begins after DT */

    SideAssert(SystemTimeToFileTime(&st, &ft));
    n1 = (int) CompareFileTime(&ft, &ftDT);
    n2 = (int) CompareFileTime(&ft, &ftST);
    if (n1 == n2)
        /*  The target time is not between DT and ST. */
        /*  It's DT if DT starts after ST. */
        return n0 > 0;
    else
        /*  The target time is between DT and ST. */
        /*  It's DT if ST starts after DT. */
        return n0 < 0;

}

BOOL
FDSTOfFileTime(FILETIME ft)
{
    SYSTEMTIME st;

    if (!FileTimeToSystemTime(&ft, &st))
        return FALSE;
    return FDSTOfSystemTime(st, &__tz);
}

BOOL
__export WINAPI
FileTimeToLocalFileTime (const FILETIME FAR * lpft, FILETIME FAR * lpftLocal)
{
    CheckTimezone(NULL);

    /*  This used to check FDSTOfFileTime(*lpft) instead of */
    /*  simply fDaylight. It was changed in order to be compatible */
    /*  with Win32. */
    *lpftLocal = FtAddFt(*lpft, fDaylight ? ftDaylightBias : ftStandardBias);

    return(TRUE) ;
}


BOOL
__export WINAPI
LocalFileTimeToFileTime (const FILETIME FAR * lpftLocal, FILETIME FAR * lpft)
{

    AssertSz( lpftLocal && !IsBadReadPtr( lpftLocal, sizeof( FILETIME ) ),
            "lpftLocal fails address check" );
    
    AssertSz( lpft && !IsBadWritePtr( lpft, sizeof( FILETIME ) ),
            "lpft fails address check" );
            
    CheckTimezone(NULL);

    /*  This used to check FDSTOfFileTime(*lpft) instead of */
    /*  simply fDaylight. It was changed in order to be compatible */
    /*  with Win32. */
    *lpft = FtSubFt(*lpftLocal,
        fDaylight ? ftDaylightBias : ftStandardBias);

    return(TRUE) ;
}

/*
 *  GetSystemTime
 *
 *  Returns the current time as GMT.
 */
void __export WINAPI
GetSystemTime(SYSTEMTIME FAR *pst)
{
    SYSTEMTIME  st;
    FILETIME    ft;
    
    AssertSz( pst && !IsBadWritePtr( pst, sizeof( SYSTEMTIME ) ),
            "pst fails address check" );

    CheckTimezone(&st);
    SystemTimeToFileTime(&st, &ft);
    ft = FtSubFt(ft, fDaylight ? ftDaylightBias : ftStandardBias);
    FileTimeToSystemTime(&ft, pst);
}

/*
 *  GetLocalTime
 *
 *  Returns the current time as local time.
 *  This is easy: DOS keeps local time.
 */
void __export WINAPI
GetLocalTime(SYSTEMTIME FAR *pst)
{
    /* validate parameters */
    
    AssertSz( pst && !IsBadWritePtr( pst, sizeof( SYSTEMTIME ) ),
            "pst fails address check" );
            
    DOSTime(pst);
}

/* //$  Read-only data */
/* //$  TCHAR szFileTZ              = "msmail.ini"; //$ maybe? */
TCHAR   szSectionTZ[]           = "MAPI 1.0 Time Zone";
TCHAR   szKeyBias[]             = "Bias";
TCHAR   szKeyStandardName[]     = "StandardName";
TCHAR   szKeyStandardStart[]    = "StandardStart";
TCHAR   szKeyStandardBias[]     = "StandardBias";
TCHAR   szKeyDaylightName[]     = "DaylightName";
TCHAR   szKeyDaylightStart[]    = "DaylightStart";
TCHAR   szKeyDaylightBias[]     = "DaylightBias";
TCHAR   __szEmpty[]             = "";

/*
 *  Reads time zone parameters from WIN.INI, if they are present
 *  there. If they are not, tries the TZ environment variable.
 */
DWORD __export WINAPI
GetTimeZoneInformation(LPTIME_ZONE_INFORMATION ptz)
{
    int         cb;
    DWORD       dw;
    CHAR        rgchBias[12];
    CHAR        rgchSD[50];
    CHAR        rgchSB[12];
    CHAR        rgchDD[50];
    CHAR        rgchDB[12];
    SYSTEMTIME  st;

    Assert(!IsBadWritePtr(ptz, sizeof(TIME_ZONE_INFORMATION)));
    ZeroMemory(ptz, sizeof(TIME_ZONE_INFORMATION));

    /*  First check for the base bias. */
    if (GetProfileString(szSectionTZ, szKeyBias, __szEmpty,
        rgchBias, sizeof(rgchBias)))
    {
        ptz->Bias = (LONG) UlFromSzHex(rgchBias);
    }
    else if (!FParseTZ(ptz))
    {
        DebugTrace("GetTimeZoneInformation: using TZ\n");
    }
    else
    {
        /*  If the base bias is missing, don't worry about the rest. */
        DebugTrace("GetTimeZoneInformation: nothing.\n");
        dw = TIME_ZONE_ID_INVALID;
        goto ret;
    }

    /*  Check for full timezone info. */
    /*  Missing names don't matter. */
    GetProfileString(szSectionTZ, szKeyStandardName,
        __szEmpty, ptz->StandardName, sizeof(ptz->StandardName));
    GetProfileString(szSectionTZ, szKeyDaylightName,
        __szEmpty, ptz->DaylightName, sizeof(ptz->DaylightName));
    if (!(cb = GetProfileString(szSectionTZ, szKeyStandardStart,
            __szEmpty, rgchSD, sizeof(rgchSD))) ||
        !(cb = GetProfileString(szSectionTZ, szKeyStandardBias,
            __szEmpty, rgchSB, sizeof(rgchSB))) ||
        !(cb = GetProfileString(szSectionTZ, szKeyDaylightStart,
            __szEmpty, rgchDD, sizeof(rgchDD))) ||
        !(cb = GetProfileString(szSectionTZ, szKeyDaylightBias,
            __szEmpty, rgchDB, sizeof(rgchDB))))
    {
        DebugTrace("GetTimeZoneInformation: incomplete timezone info\n");
        /*  just use the basic bias */
    }
    else
    {
        /*  We got full config information. */
        ptz->StandardBias = (LONG) UlFromSzHex(rgchSB);
        ptz->DaylightBias = (LONG) UlFromSzHex(rgchDB);
        SideAssert(FBinFromHex(rgchSD, (LPBYTE)&ptz->StandardDate));
        SideAssert(FBinFromHex(rgchDD, (LPBYTE)&ptz->DaylightDate));
    }

    /*  If there is no difference in the biases, we don't know about */
    /*  DST. Otherwise, check whether we're currently in DST. */
    if (ptz->StandardBias == ptz->DaylightBias)
    {
        dw = TIME_ZONE_ID_UNKNOWN;
    }
    else
    {
        DOSTime(&st);
        dw = FDSTOfSystemTime(st, ptz) ?
            TIME_ZONE_ID_DAYLIGHT :
            TIME_ZONE_ID_STANDARD;
    }

ret:
    return dw;
}

/*
 *  Writes time zone parameters to WIN.INI.
 */
BOOL __export WINAPI
SetTimeZoneInformation(const TIME_ZONE_INFORMATION FAR *ptz)
{
    CHAR        rgchBias[12];
    CHAR        rgchSD[50];
    CHAR        rgchSB[12];
    CHAR        rgchDD[50];
    CHAR        rgchDB[12];

    if (!FValidTimeZoneInformation((TIME_ZONE_INFORMATION FAR *)ptz))
    {
        DebugTraceArg(SetTimeZoneInformation, "bogus TZ info");
        return FALSE;
    }

    wsprintf(rgchBias, "%lx", ptz->Bias);
    wsprintf(rgchSB, "%lx", ptz->StandardBias);
    wsprintf(rgchDB, "%lx", ptz->DaylightBias);
    HexFromBin((LPBYTE)&ptz->StandardDate, sizeof(SYSTEMTIME), rgchSD);
    HexFromBin((LPBYTE)&ptz->DaylightDate, sizeof(SYSTEMTIME), rgchDD);

    if ((!WriteProfileString(szSectionTZ, szKeyBias, rgchBias)) ||
        (!WriteProfileString(szSectionTZ, szKeyStandardName, ptz->StandardName)) ||
        (!WriteProfileString(szSectionTZ, szKeyStandardStart, rgchSD)) ||
        (!WriteProfileString(szSectionTZ, szKeyStandardBias, rgchSB)) ||
        (!WriteProfileString(szSectionTZ, szKeyDaylightName, ptz->DaylightName)) ||
        (!WriteProfileString(szSectionTZ, szKeyDaylightStart, rgchDD)) ||
        (!WriteProfileString(szSectionTZ, szKeyDaylightBias, rgchDB)))
    {
        DebugTrace("SetTimeZoneInformation: WriteProfileString failed\n");
        return FALSE;
    }

    return TRUE;
}

BOOL
FValidTimeZoneInformation(TIME_ZONE_INFORMATION FAR *ptz)
{
    if (IsBadReadPtr(ptz, sizeof(TIME_ZONE_INFORMATION)))
    {
        DebugTraceArg(FValidTimeZoneInformation, "ptz fails address check");
        return FALSE;
    }
    if (IsBadStringPtr(ptz->StandardName, sizeof(ptz->StandardName)) ||
        IsBadStringPtr(ptz->StandardName, sizeof(ptz->DaylightName)))
    {
        DebugTraceArg(FValidTimeZoneInformation, "one of two names fails address check");
        return FALSE;
    }
    if (!FValidBias(ptz->Bias) ||
        !FValidBias(ptz->DaylightBias) ||
        !FValidBias(ptz->StandardBias))
    {
        DebugTraceArg(FValidTimeZoneInformation, "one of three biases is out of range");
        return FALSE;
    }
    if (!FValidSysTime(&ptz->StandardDate, TRUE) ||
        !FValidSysTime(&ptz->DaylightDate, TRUE))
    {
        DebugTraceArg(FValidTimeZoneInformation, "one of two start dates is out of range");
        return FALSE;
    }

    return TRUE;
}

BOOL
FValidBias(LONG lBias)
{
    if (lBias > 0 && lBias >= 24*60)
        return FALSE;
    if (lBias < 0 && lBias <= -24*60)
        return FALSE;

    return TRUE;
}

BOOL
FValidSysTime(SYSTEMTIME FAR *pst, BOOL fRelativeOK)
{
    if (IsBadReadPtr(pst, sizeof(SYSTEMTIME)))
        return FALSE;

    if (pst->wYear < 1600)
    {
        if (pst->wYear == 0 && fRelativeOK)
        {
            /*  This is a weird relative time for Get/SetTZInfo; */
            /*  validate as such */

            if (pst->wMonth == 0)       /* nothing here */
                return TRUE;

            if (pst->wMonth > 12 ||
                pst->wDayOfWeek > 6 ||
                (pst->wDay == 0 || pst->wDay > 5) ||
                pst->wHour > 24 ||
                pst->wMinute >= 60 ||
                pst->wSecond >= 60)
                return FALSE;

            return TRUE;
        }
        return FALSE;
    }
    if (pst->wMonth == 0 || pst->wMonth > 12)
        return FALSE;
    /*  don't validate day of week */
    if (pst->wDay == 0 || pst->wDay > 31)
        return FALSE;
    if (pst->wHour > 24)
        return FALSE;
    if (pst->wMinute >= 60)
        return FALSE;
    if (pst->wSecond >= 60)
        return FALSE;
    if (pst->wMilliseconds >= 1000)
        return FALSE;

    return TRUE;
}

#pragma warning (default: 4704)

#endif  /* WIN16 */
