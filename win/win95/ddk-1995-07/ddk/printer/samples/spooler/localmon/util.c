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

/*++

Module Name:

    util.c

Abstract:

    This module provides all the utility functions.

--*/
#define NOMINMAX

#include <windows.h>
#include <winspool.h>
#include <winsplp.h>
#include <shlobj.h>

#include "spltypes.h"
#include "local.h"
#include "localmon.h"

DWORD    TlsIndex;


typedef struct _MB_DATA {
    HHOOK   hHookMB;
    HWND    hWndMB;
    UINT    idTimerMB;
    UINT    fuStyleMB;
} MB_DATA, *PMB_DATA;

#define SPOOL_MB_TIMEOUT    15000   // milliseconds

typedef struct _SPL_MSG {
    UINT    uMsg;
    LPARAM  lParam;
} SPL_MSG, *PSPL_MSG;


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



PINIPORT
FindPortFromList(
    PINIENTRY pIniEntry,
    LPSTR pName
)
{
    PINIPORT    pIniPort;
    LPSTR pNewName;
    DWORD   cb;

    pIniPort = (PINIPORT) FindIniKey(pIniEntry, (LPTSTR)(pName));

    if (pIniPort)
        return pIniPort;

    if (!pName || !*pName)
        return NULL;

    // we're assuming AllocSplMem zero init the memory

    cb = wcslen(pName);
    if (!(pNewName = AllocSplMem(cb + 2)))
        return NULL;

    wcscpy(pNewName, pName);

    if (pName[cb - 1] == ':')
    {
        // try without the colon

        pNewName[cb - 1] = 0;
    }
    else
    {
        // try with the colon

        pNewName[cb] = ':';
    }

    pIniPort = (PINIPORT) FindIniKey(pIniEntry, pNewName);
    FreeSplMem(pNewName, cb + 2);

    return pIniPort;
}


PINIENTRY
FindIniKey(
   PINIENTRY pIniEntry,
   LPTSTR pName
)
{
   if (!pName)
      return NULL;

   while (pIniEntry && wcsicmp(pName, pIniEntry->pName))
      pIniEntry = pIniEntry->pNext;

   return pIniEntry;
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


// -----------------------------------------------------------------------
// SpoolTimerProc
//
// Timer callback for the spooler error message box.
//
// -----------------------------------------------------------------------
VOID
CALLBACK
SpoolTimerProc(
    HWND hWnd,
    UINT uMsg,
    UINT idEvent,
    DWORD dwTime
)
{
    PMB_DATA pMBData = TlsGetValue(TlsIndex);

    // Thread storage is used for MB_DATA.
    // Put the message box back to foreground.

    SetForegroundWindow(pMBData->hWndMB);
}


// -----------------------------------------------------------------------
// SpoolCBTProc
//
// CBT callback for spooler error message box. We only hook HCBT_CREATEWND
// just to get the HWND of the message box, which we use in the Timer
// callback.
//
// -----------------------------------------------------------------------
LRESULT
CALLBACK
SpoolCBTProc(
    int nCode,
    WPARAM wParam,
    LPARAM lParam
)
{
    LRESULT ret = 0;
    PMB_DATA pMBData = TlsGetValue(TlsIndex);

    // Thread storage is used for MB_DATA.

    switch (nCode)
    {
        case HCBT_CREATEWND:
            // save the hWnd of the message box.
            // Unhook the CBT hook since we've got what we needed.
            // Start a timer.

            pMBData->hWndMB = (HWND)wParam;
            UnhookWindowsHookEx(pMBData->hHookMB);
            pMBData->idTimerMB = SetTimer(NULL, 0, SPOOL_MB_TIMEOUT, (TIMERPROC)SpoolTimerProc);
            break;
    }

    return ret;
}

// -----------------------------------------------------------------------
// Message
//
// Bring up a message box to display the text passed. CaptionID is the
// res ID for the message box caption. TextID is the res ID for the
// format of the message. Thru the magic of "cdecl" the rest of the
// arguments will be passed to wvsprintf and merged into the format of
// of the message.
//
// -----------------------------------------------------------------------
int cdecl Message(HWND hwnd, DWORD Type, int CaptionID, int TextID, ...)
{
    TCHAR   *MsgText;
    TCHAR   MsgFormat[120];
    TCHAR   MsgCaption[20];
    int ret = 0;
    MB_DATA MBData;

    if (!(MsgText = LocalAlloc(LPTR, 1024)))
        return 0;

    if ((LoadString(hInst, TextID, MsgFormat, sizeof(MsgFormat)) > 0) &&
        (LoadString(hInst, CaptionID, MsgCaption, sizeof(MsgCaption)) > 0))
    {
        wvsprintf(MsgText, MsgFormat, (LPSTR)(((LPSTR)(&TextID))+sizeof(TextID)) );
        lstrcat(MsgText, "\n");

        if (Type & MB_SYSTEMMODAL)
            ret = MessageBox(hwnd, MsgText, MsgCaption, Type);
        else
        {
            // Thread local storage is used to put the MB_DATA.
            // Set CBT hook to retrieve the hWnd for the message box.

            MBData.fuStyleMB = Type;
            MBData.idTimerMB = 0;
            TlsSetValue(TlsIndex, &MBData);
            MBData.hHookMB = SetWindowsHookEx(WH_CBT, SpoolCBTProc, NULL, GetCurrentThreadId());

            ret = MessageBox(hwnd, MsgText, MsgCaption, Type);

            // clear the thread local storage.

            TlsSetValue(TlsIndex, 0);

            // kill the timer if there is one.

            if (MBData.idTimerMB)
                KillTimer(NULL, MBData.idTimerMB);
        }
    }

    LocalFree(MsgText);

    return ret;
}


// -----------------------------------------------------------------------
//
// -----------------------------------------------------------------------
LPBYTE
PackStrings(
   LPTSTR FAR *pSource,
   LPBYTE pDest,
   DWORD FAR *DestOffsets,
   LPBYTE pEnd
)
{
   while (*DestOffsets != -1) {
      if (*pSource) {
         pEnd-=wcslen(*pSource)*sizeof(TCHAR) + sizeof(TCHAR);

         *(LPTSTR *)(pDest+*DestOffsets)=wcscpy((LPTSTR)pEnd, *pSource);

      } else

         *(LPTSTR *)(pDest+*DestOffsets)=0;

      pSource++;
      DestOffsets++;
   }

   return pEnd;
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
    TlsIndex = TlsAlloc();

    return TRUE;
}
