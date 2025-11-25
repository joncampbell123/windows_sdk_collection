//*******************************************************************************************
//
// Filename : OS.cpp
//	
//				Implementation file for CFileTime
//
// Copyright (c) 1994 - 1996 Microsoft Corporation. All rights reserved
//
//*******************************************************************************************

#include "pch.h"

#include "os.h"

void CFileTime::DateTimeToString(WORD wDate, WORD wTime, LPSTR pszText)
{
    FILETIME ft;

    // Netware directories do not have dates...
    if (wDate == 0)
    {
        *pszText='\0';
        return;
    }

    DosDateTimeToFileTime(wDate, wTime, &ft);
    FileTimeToDateTimeString(&ft, pszText);
}


void CFileTime::FileTimeToDateTimeString(LPFILETIME lpft, LPSTR pszText)
{
    SYSTEMTIME st;

    FileTimeToLocalFileTime(lpft, lpft);
    FileTimeToSystemTime(lpft, &st);
    GetDateFormatA(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, pszText, 64);
    pszText += lstrlen(pszText);
    *pszText++ = ' ';
    GetTimeFormatA(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, pszText, 64);
}
