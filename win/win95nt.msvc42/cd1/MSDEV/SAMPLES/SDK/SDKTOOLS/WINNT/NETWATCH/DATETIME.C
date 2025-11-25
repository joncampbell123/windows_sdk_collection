/*
 *  datetime.c
 *  
 *  Purpose:
 *      mostly stolen from winfile
 *      converted to unicode and added PutCounterTime
 *  
 *  Owner:
 *      MikeSart
 */
#define UNICODE 1

#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include "netwatch.h"

TCHAR   szShortDate[11]     = TEXT("MM/DD/YY");
TCHAR   szTime[4]           = TEXT(":");
TCHAR   sz1159[9]           = TEXT("AM");
TCHAR   sz2359[9]           = TEXT("PM");
TCHAR   szInternational[]   = TEXT("Intl");
TCHAR   szComma[4]          = TEXT(",");
TCHAR   szDecimal[4]        = TEXT(".");
//INT     iTime               = 1;           // Default to 24-hour time
INT     iTime               = 0;           // Default to 12-hour time
INT     iTLZero             = FALSE;       // Default to non-leading zeros
//INT     iTLZero             = TRUE;        // Default to leading zeros

#define COUNTOF(_sz)    (sizeof(_sz) / sizeof(TCHAR))

//--------------------------------------------------------------------------
//
//  GetPict() -
//
//--------------------------------------------------------------------------

//  This gets the number of consecutive chrs of the same kind.  This is used
//  to parse the time picture.  Returns 0 on error.
UINT
GetPict(TCHAR ch, TCHAR *szStr)
{
    register UINT  count;

    count = 0;
    while(ch == *szStr++)
        count++;

    return count;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  GetInternational() -                                                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
VOID
GetInternational(VOID)
{
    GetProfileString(szInternational, TEXT("sShortDate"), szShortDate, szShortDate, COUNTOF(szShortDate));
    if(szShortDate[0])
        CharUpper(szShortDate);
    GetProfileString(szInternational, TEXT("sTime"), szTime, szTime, COUNTOF(szTime));
    GetProfileString(szInternational, TEXT("s1159"), sz1159, sz1159, COUNTOF(sz1159));
    GetProfileString(szInternational, TEXT("s2359"), sz2359, sz2359, COUNTOF(sz2359));
    GetProfileString(szInternational, TEXT("sThousand"), szComma, szComma, COUNTOF(szComma));
    GetProfileString(szInternational, TEXT("sDecimal"), szDecimal, szDecimal, COUNTOF(szDecimal));

    iTime   = GetProfileInt(szInternational, TEXT("iTime"), iTime);
    iTLZero = GetProfileInt(szInternational, TEXT("iTLZero"), iTLZero);
}

/////////////////////////////////////////////////////////////////////
//
// Name:     CreateDate
//
// Synopsis: Converts a local time to a localized string
//
// IN        lpst       LPSYSTEMTIME  local system time
// INOUT     lpszOutStr --            string of date
//
// Return:   INT   length of date
//
// Assumes:  lpszOutStr is large enough for the date string.
//           Separator is one character long
//
// Effects:  lpszOutStr modified.
//
//
// Notes:    albertt (255 1.26)
//
/////////////////////////////////////////////////////////////////////
INT
CreateDate(LPSYSTEMTIME lpst, TCHAR *szOutStr)
{
    INT              i;
    UINT             cchPictPart;
    UINT             uDigits, uVal, uValT;
    register TCHAR   *pszPict;
    register TCHAR   *pszInStr;
    TCHAR            *pChar;
    BOOL             bClipping = FALSE;

    pszPict = szShortDate;
    pszInStr = szOutStr;

    for(i = 0; i < 3; i++) 
    {
        cchPictPart = GetPict(*pszPict, pszPict);
        switch(*pszPict) 
        {
        case 'M':
            uVal = lpst->wMonth;
            goto CDDoIt;

        case 'D':
            uVal = lpst->wDay;
            goto CDDoIt;

        case 'Y':
            uVal = lpst->wYear;
            bClipping = TRUE;

            // goto CDDoIt;
CDDoIt:

            //
            // Move pszPict to the separator
            //
            pszPict += cchPictPart;

            //
            // First calculate how many digits there are
            //
            for(uDigits=0, uValT=uVal; uValT;uDigits++) 
            {
                uValT /= 10;
            }

            //
            // Store any necessary leading zeros
            //
            while(uDigits < cchPictPart) 
            {
                *pszInStr++ = '0';
                cchPictPart--;
            }

            //
            // Allow clipping (1993 -> 93) IF bClipping on
            // (Only for Year)
            //
            if(bClipping && uDigits > cchPictPart) 
            {
                uDigits = cchPictPart;
            }
            bClipping = FALSE;

            //
            // Write out the number now one digit at a time
            // From end to beginning..
            //
            // Set pszInStr to the end of the date string
            //
            pszInStr += uDigits;

            //
            // Now walk backwards:
            // We must pre-decrement pChar since it now points to
            // the next character AFTER our string.
            //
            for(pChar = pszInStr; uDigits; uVal /= 10, uDigits--) 
            {
                *--pChar = (TCHAR) ((uVal % 10) + '0');
            }

            //
            // Add the separator
            //
            if(*pszPict)
                *pszInStr++ = *pszPict;

            break;
        }
        pszPict++;
    }

    *pszInStr = 0;

    return lstrlen(szOutStr);
}

/////////////////////////////////////////////////////////////////////
//
// Name:     CreateTime
//
// Synopsis: Creates a localized time string for local time
//
// IN    lpst       LPSYSTEMTIME  local time
// INOUT lpszOutStr --            String
//
// Return:   INT  length of string
//
//
// Assumes:   lpszOutStr is big enough for all times
//
// Effects:   lpszOutStr modified.
//            Separator is 1 character
//
//
// Notes:
//
/////////////////////////////////////////////////////////////////////
INT
CreateTime(LPSYSTEMTIME lpst, TCHAR *szOutStr, BOOL fCounter)
{
    INT             i;
    BOOL            bAM;
    UINT            uHourMinSec;
    register UINT   uDigit;
    register TCHAR  *pszInStr;
    UINT            uValArray[3];

    uDigit = uValArray[0] = lpst->wHour;
    uValArray[1] = lpst->wMinute;
    uValArray[2] = lpst->wSecond;

    pszInStr = szOutStr;

    bAM = (uDigit < 12);

    if(!fCounter && !iTime) 
    {
        if(uDigit >= 12)
            uDigit -= 12;

        if(!uDigit)
            uDigit = 12;
    }

    uValArray[0] = uDigit;
    for(i = 0; i < 3; i++) 
    {
        uHourMinSec = uValArray[i];

        if((i == 0) && fCounter)
        {
            wsprintf(pszInStr, szFmtNum, uValArray[0]);
            pszInStr += lstrlen(pszInStr);
        }
        else
        {
            // This assumes that the values are of two digits only.
            uDigit = uHourMinSec / 10;

            if (i > 0)
                *pszInStr++ = (TCHAR)(uDigit + '0');
            else if (uDigit || iTLZero)
                *pszInStr++ = (TCHAR)(uDigit + '0');

            *pszInStr++ = (TCHAR)((uHourMinSec % 10) + '0');
        }

        // Assumes time sep. is 1 char long
        if(i < 2)
            *pszInStr++ = *szTime;
    }

    if(fCounter)
    {
        *pszInStr = 0;
    }
    else
    {
        if(bAM)
            lstrcpy(pszInStr, sz1159);
        else
            lstrcpy(pszInStr, sz2359);
    }

    return lstrlen(szOutStr);
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  PutDate() -                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
TCHAR *
PutDate(time_t tm_t, TCHAR *szStr)
{
    SYSTEMTIME  st;
    struct tm   *ptm;

    ptm = localtime(&tm_t);

    st.wYear        = ptm->tm_year + 1900;
    st.wMonth       = ptm->tm_mon + 1;
    st.wDay         = ptm->tm_mday;

    CreateDate(&st, szStr);
    return szStr;
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  PutTime() -                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
TCHAR *
PutTime(time_t tm_t, TCHAR *szStr)
{
    SYSTEMTIME  st;
    struct tm   *ptm;

    ptm = localtime(&tm_t);

    st.wHour            = ptm->tm_hour;
    st.wMinute          = ptm->tm_min;
    st.wSecond          = ptm->tm_sec;
    st.wMilliseconds    = 0;

    CreateTime(&st, szStr, FALSE);
    return szStr;
}

TCHAR *
PutCounterTime(DWORD dwTime, TCHAR *szStr)
{
    SYSTEMTIME  st;

    st.wHour            = (WORD)(dwTime / 3600);
    dwTime              -= (st.wHour * 3600);
    st.wMinute          = (WORD)(dwTime / 60);
    dwTime              -= (st.wMinute * 60);
    st.wSecond          = (WORD)dwTime;
    st.wMilliseconds    = 0;

    CreateTime(&st, szStr, TRUE);
    return szStr;
}
