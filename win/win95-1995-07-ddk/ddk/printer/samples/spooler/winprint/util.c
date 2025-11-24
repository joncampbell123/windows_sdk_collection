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
#include <shlobj.h>

#include "local.h"

HANDLE hInst = NULL;



LPVOID
ReallocSplMem(
   LPVOID pOldMem,
   DWORD cbOld,
   DWORD cbNew
)
{
    LPVOID pNewMem;

    pNewMem=AllocSplMem(cbNew);

    if (pOldMem) {
        CopyMemory(pNewMem, pOldMem, min(cbNew, cbOld));
        FreeSplMem(pOldMem, cbOld);
    }

    return pNewMem;
}

// -----------------------------------------------------------------------
// AllocSplStr
//
// Routine Description:
// 
//     This function will allocate enough local memory to store the specified
//     string, and copy that string to the allocated memory
// 
// Arguments:
// 
//     pStr - Pointer to the string that needs to be allocated and stored
// 
// Return Value:
// 
//     NON-NULL - A pointer to the allocated memory containing the string
// 
//     FALSE/NULL - The operation failed. Extended error status is available
//     using GetLastError.
// -----------------------------------------------------------------------
LPTSTR
AllocSplStr(
    LPTSTR pStr
)
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
   return pStr ? !(BOOL)FreeSplMem(pStr, wcslen(pStr)*sizeof(TCHAR)+sizeof(TCHAR))
               : FALSE;
}

BOOL
ReallocSplStr(
   LPTSTR FAR *ppStr,
   LPTSTR pStr
)
{
   if (*ppStr)
        FreeSplStr(*ppStr);
   *ppStr=AllocSplStr(pStr);

   return TRUE;
}




#ifdef DEBUG

DWORD SplDbgLevel = 1;

VOID cdecl DbgMsg( LPSTR MsgFormat, ... )
{
    CHAR   MsgText[256];

    wvsprintf(MsgText,MsgFormat,(LPSTR)(((LPSTR)(&MsgFormat))+sizeof(MsgFormat)) );
    lstrcat( MsgText, "\r");

    OutputDebugString( MsgText );
}

#endif



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
