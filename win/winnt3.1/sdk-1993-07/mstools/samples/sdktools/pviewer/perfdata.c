
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/


/******************************************************************************

                        P E R F O R M A N C E   D A T A

    Name:       perfdata.c

    Description:
        This module together with objdata.c, instdata.c, and cntrdata.c
        access the performance data.

******************************************************************************/

#include <windows.h>
#include <winperf.h>
#include "perfdata.h"
#include <stdlib.h>




LPTSTR      *gPerfTitleSz;
LPTSTR      TitleData;




//*********************************************************************
//
//  GetPerfData
//
//      Get a new set of performance data.
//
//      *ppData should be NULL initially.
//      This function will allocate a buffer big enough to hold the
//      data requested by szObjectIndex.
//
//      *pDataSize specifies the initial buffer size.  If the size is
//      too small, the function will increase it until it is big enough
//      then return the size through *pDataSize.  Caller should
//      deallocate *ppData if it is no longer being used.
//
//      Returns ERROR_SUCCESS if no error occurs.
//
//      Note: the trial and error loop is quite different from the normal
//            registry operation.  Normally if the buffer is too small,
//            RegQueryValueEx returns the required size.  In this case,
//            the perflib, since the data is dynamic, a buffer big enough
//            for the moment may not be enough for the next. Therefor,
//            the required size is not returned.
//
//            One should start with a resonable size to avoid the overhead
//            of reallocation of memory.
//
DWORD   GetPerfData    (HKEY        hPerfKey,
                        LPTSTR      szObjectIndex,
                        PPERF_DATA  *ppData,
                        DWORD       *pDataSize)
{
DWORD   DataSize;
DWORD   dwR;
DWORD   Type;


    if (!*ppData)
        *ppData = (PPERF_DATA) LocalAlloc (LMEM_FIXED, *pDataSize);


    do  {
        DataSize = *pDataSize;
        dwR = RegQueryValueEx (hPerfKey,
                               szObjectIndex,
                               NULL,
                               &Type,
                               (BYTE *)*ppData,
                               &DataSize);

        if (dwR == ERROR_MORE_DATA)
            {
            LocalFree (*ppData);
            *pDataSize += 1024;
            *ppData = (PPERF_DATA) LocalAlloc (LMEM_FIXED, *pDataSize);
            }

        if (!*ppData)
            {
            LocalFree (*ppData);
            return ERROR_NOT_ENOUGH_MEMORY;
            }

        } while (dwR == ERROR_MORE_DATA);

    return dwR;
}




#ifdef UNICODE

#define atoi    atoiW


//*********************************************************************
//
//  atoiW
//
//      Unicode version of atoi.
//
INT atoiW (LPTSTR s)
{
INT i = 0;

    while (isdigit (*s))
        {
        i = i*10 + (BYTE)*s - TEXT('0');
        s++;
        }

    return i;
}

#endif




//*********************************************************************
//
//  GetPerfTitleSz
//
//      Retrieves the performance data title strings.
//
//      This call retrieves english version of the title strings.  For
//      other locale the registry key should be changed to ..."perflib\\NNN"
//
//      Caller should provide two pointers, one for buffering the title
//      strings the other for indexing the title strings.  This function will
//      allocate memory for the TitleBuffer and TitleSz.  To get the title
//      string for a particular title index one would just index the TitleSz.
//      *TitleCount returns the highest index can be used.  If TitleSz[N] is
//      NULL then there is no Title for index N.
//
//      Example:  TitleSz[20] points to titile string for title index 20.
//
//      When done with the TitleSz, caller should LocalFree(*TitleBuffer).
//
//      This function returns ERROR_SUCCESS if no error.
//
DWORD   GetPerfTitleSz (HKEY    hKeyMachine,
                        LPTSTR  *TitleBuffer,
                        LPTSTR  *TitleSz[],
                        DWORD   *TitleCount)
{
HKEY    hKey;
DWORD   Type;
DWORD   DataSize;
DWORD   dwR;
DWORD   Len;
DWORD   Index;
LPTSTR  szTitle;




    // Get the last counter's index so we know how much memory to allocate for TitleSz
    dwR = RegOpenKeyEx (hKeyMachine,
                        TEXT("software\\microsoft\\windows nt\\currentversion\\perflib"),
                        0,
                        KEY_READ,
                        &hKey);

    if (dwR != ERROR_SUCCESS)
        return dwR;


    DataSize = sizeof (DWORD);
    dwR = RegQueryValueEx (hKey, TEXT("Last Counter"), 0, &Type, (LPBYTE)TitleCount, &DataSize);

    if (dwR != ERROR_SUCCESS)
        return dwR;





    // Now get the counter names and indexes.
    dwR = RegOpenKeyEx (hKeyMachine,
                        TEXT("software\\microsoft\\windows nt\\currentversion\\perflib\\009"),
                        0,
                        KEY_READ,
                        &hKey);

    if (dwR != ERROR_SUCCESS)
        return dwR;


    // Find out the size of the data.
    dwR = RegQueryValueEx (hKey, TEXT("Counters"), 0, &Type, 0, &DataSize);



    // Allocate memory
    *TitleBuffer = (LPTSTR)LocalAlloc (LMEM_FIXED, DataSize);
    if (!*TitleBuffer)
        return ERROR_NOT_ENOUGH_MEMORY;

    *TitleSz = (LPTSTR *)LocalAlloc (LPTR, (*TitleCount) * sizeof (LPTSTR));
    if (!*TitleSz)
        {
        LocalFree (*TitleBuffer);
        return ERROR_NOT_ENOUGH_MEMORY;
        }





    // Query the data
    dwR = RegQueryValueEx (hKey, TEXT("Counters"), 0, &Type, (BYTE *)*TitleBuffer, &DataSize);

    if (dwR != ERROR_SUCCESS)
        {
        LocalFree (*TitleBuffer);
        LocalFree (*TitleSz);
        return dwR;
        }




    // Setup the TitleSz array of pointers to point to beginning of each title string.
    // TitleBuffer is type REG_MULTI_SZ.

    szTitle = *TitleBuffer;

    while (Len = lstrlen (szTitle))
        {
        Index = atoi (szTitle);

        szTitle = szTitle + Len +1;

        (*TitleSz)[Index] = szTitle;

        szTitle = szTitle + lstrlen (szTitle) +1;
        }


    return dwR;

}
