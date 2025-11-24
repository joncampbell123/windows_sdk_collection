//----------------------------------------------------------------------------
// File: install.c
//  This file contains routines for handling installing printers.
//
//----------------------------------------------------------------------------
#define PRINTDRIVER
#define NOPQ
#define NOINSTALLFILES
#include <print.h>
#include "device.h"
#include "dialog.h"
#include "profile.h"

BOOL NEAR PASCAL CopyProfileSection(LPSTR, LPSTR);

//-------------------------*DevInstall*------------------------------------
// Action: Install or de-install a device.
//
//----------------------------------------------------------------------------

int WINAPI DevInstall(hWnd, lpDevName, lpOldPort, lpNewPort)
HWND    hWnd;
LPSTR   lpDevName;
LPSTR   lpOldPort, lpNewPort;
{
    char szOldSec[MAX_STRING_LENGTH];

    if (!lpOldPort)
        return 1;

    // Compose the profile section name: <device-name,port-name>
    if (lstrlen(lpDevName) > MAX_STRING_LENGTH)
        return -1;

    MakeAppName((LPSTR)szOldSec, lpDevName, lpOldPort, sizeof(szOldSec));

    // copy the device settings from the old port to the new port.
    if (lpNewPort)
    {
        char szNewSec[MAX_STRING_LENGTH];

        MakeAppName((LPSTR)szNewSec, lpDevName, lpNewPort, sizeof(szNewSec));
        if (!lstrcmp((LPSTR)szOldSec, (LPSTR)szNewSec))
            return 1;
        if (!CopyProfileSection((LPSTR)szOldSec, (LPSTR)szNewSec))
            return -1;
    }
    // remove its device setttings from the old section after moving them
    // to the new section or when asked to de-install the device.
    WriteProfileString((LPSTR)szOldSec, NULL, NULL);

    return 1;
}

#define KEY_BUF_SIZE_INC    256

BOOL NEAR PASCAL CopyProfileSection(lpOldSec, lpNewSec)
LPSTR   lpOldSec, lpNewSec;
{
    HANDLE  hMem;
    LPSTR   lpKeyBuf;
    int     nSize;
    char    szTmp[MAX_STRING_LENGTH];

    // read in all the key names under the old section
    nSize = KEY_BUF_SIZE_INC;
    if (!(hMem = GlobalAlloc(GHND, (long)nSize)))
        return FALSE;
    lpKeyBuf = GlobalLock(hMem);
    while (GetProfileString(lpOldSec, (LPSTR)NULL, "", lpKeyBuf, nSize)
           == nSize - 2)
    {
        // not enough buffer space for all the key names.
        GlobalUnlock(hMem);
        nSize += KEY_BUF_SIZE_INC;
        if (!(hMem = GlobalReAlloc(hMem, (long)nSize, GHND)))
            return FALSE;
        lpKeyBuf = GlobalLock(hMem);
    }
    // loop through each key and copy the value from the old to the new.
    while ((nSize = lstrlen(lpKeyBuf)) > 0)
    {
        GetProfileString(lpOldSec, lpKeyBuf, "", (LPSTR)szTmp, sizeof(szTmp));
        WriteProfileString(lpNewSec, lpKeyBuf, (LPSTR)szTmp);
        lpKeyBuf += (nSize + 1);
    }
    GlobalUnlock(hMem);
    GlobalFree(hMem);

    return TRUE;
}
