/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

#define NOMINMAX
#include <windows.h>
#include <winspool.h>
#include <winsplp.h>
#include "spltypes.h"
#include "local.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

HANDLE hInst = NULL;

#ifdef DEBUG
VOID
SplInSem(
   VOID
)
{
}


VOID
SplOutSem(
   VOID
)
{
}
#endif


VOID
EnterSplSem(
   VOID
)
{
    EnterCriticalSection(&pjlMonSection);
}


VOID
LeaveSplSem(
   VOID
)
{
    LeaveCriticalSection(&pjlMonSection);
}


#ifdef DEBUG

LPVOID
AllocSplMem(
    DWORD cb
)
/*++

Routine Description:

    This function will allocate local memory. It will possibly allocate extra
    memory and fill this with debugging information for the debugging version.

Arguments:

    cb - The amount of memory to allocate

Return Value:

    NON-NULL - A pointer to the allocated memory

    FALSE/NULL - The operation failed. Extended error status is available
    using GetLastError.

--*/
{
    PDWORD  pMem;
    DWORD    cbNew;

SplInSem();

    cbNew = cb+2*sizeof(DWORD);
    if (cbNew & 3)
        cbNew += sizeof(DWORD) - (cbNew & 3);

    pMem=(PDWORD)LocalAlloc(LPTR, cbNew);

    if (!pMem) {
        DBGMSG(DBG_ERROR, ("pjlMon: Heap Allocation failed for %d bytes\n", cbNew));
        return 0;
    }

    *pMem=cb;
    *(PDWORD)((PBYTE)pMem+cbNew-sizeof(DWORD))=0xdeadbeef;

    return (LPVOID)(pMem+1);
}


BOOL
FreeSplMem(
   LPVOID pMem,
   DWORD  cb
)
{
    DWORD   cbNew;
    LPDWORD pNewMem;

SplInSem();

    pNewMem = pMem;
    pNewMem--;

    cbNew = cb+2*sizeof(DWORD);
    if (cbNew & 3)
        cbNew += sizeof(DWORD) - (cbNew & 3);

    if ((*pNewMem != cb) ||
       (*(LPDWORD)((LPBYTE)pNewMem + cbNew - sizeof(DWORD)) != 0xdeadbeef)) {
        DBGMSG(DBG_ERROR, ("Corrupt Memory in pjlmon: %0lx\n", pNewMem));
    }

    LocalFree((HLOCAL)pNewMem);

    return TRUE;
}

#endif


LPTSTR
AllocSplStr(
    LPTSTR pStr
)
/*++

Routine Description:

    This function will allocate enough local memory to store the specified
    string, and copy that string to the allocated memory

Arguments:

    pStr - Pointer to the string that needs to be allocated and stored

Return Value:

    NON-NULL - A pointer to the allocated memory containing the string

    FALSE/NULL - The operation failed. Extended error status is available
    using GetLastError.

--*/
{
   LPTSTR pMem;

   if (!pStr)
      return 0;

   if (pMem = AllocSplMem( wcslen(pStr)*sizeof(TCHAR) + sizeof(TCHAR) ))
      wcscpy(pMem, pStr);

   return pMem;
}


BOOL
FreeSplStr(
   LPTSTR pStr
)
{
   return pStr ? (BOOL)FreeSplMem(pStr, wcslen(pStr)*sizeof(TCHAR)+sizeof(TCHAR))
               : FALSE;
}


PINIENTRY
FindIniKey(
   PINIENTRY pIniEntry,
   LPTSTR pName
)
{
   if (!pName)
      return NULL;

   SplInSem();

   while (pIniEntry && wcsicmp(pName, pIniEntry->pName))
      pIniEntry = pIniEntry->pNext;

   return pIniEntry;
}


// -----------------------------------------------------------------------
//
// DEBUG Stuff
//
// -----------------------------------------------------------------------
#ifdef DEBUG

DWORD SplDbgLevel = 0;

VOID cdecl DbgMsg( LPSTR MsgFormat, ... )
{
    CHAR   MsgText[1024];

    wvsprintf(MsgText,MsgFormat,(LPSTR)(((LPSTR)(&MsgFormat))+sizeof(MsgFormat)) );
    lstrcat( MsgText, "\r");

    OutputDebugString( MsgText );
}

#endif


// -----------------------------------------------------------------------
//
// String helper function to remove crt dependency
//
// -----------------------------------------------------------------------
int
mystrnicmp(
    LPSTR cs,
    LPSTR ct,
    int n
)
{
    char ret;

    while (n--)
    {
        ret = *cs - *ct;

        if (ret)
            break;

        cs++;
        ct++;
    }

    return (int)ret;
}


LPSTR
mystrchr(
    LPSTR cs,
    char c
)
{
    while (*cs != 0)
    {
#ifndef DBCS // Non DBCS Version
        if (*cs == c)
            return cs;

#else
        if (IsDBCSLeadByte(*cs))
          cs++;
        else
        if (*cs == c)
            return cs;
#endif
        cs++;
    }

    // fail to find c in cs
    return NULL;
}


int
mystrncmp(
    LPSTR cs,
    LPSTR ct,
    int n
)
{
    char ret;

    while (n--)
    {
        ret = *cs - *ct;

        if (ret)
            break;

        cs++;
        ct++;
    }

    return (int)ret;
}
BOOL
WINAPI
DllEntryPoint(
    HINSTANCE hInstDll,
    DWORD fdwReason,
    LPVOID lpcReserved
)
{
    if (fdwReason != DLL_PROCESS_ATTACH)
        return TRUE;

    hInst = hInstDll;

    return TRUE;
}
