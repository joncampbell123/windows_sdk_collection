// -----------------------------------------------------------------
// File name:  DLLINIT.C
//
// This is the DLL's initialization source file.  It contains DllEntry,
// the Win32 DLL's entry and exit point.
//
// Written by Microsoft Product Support Services, Windows Developer Support.
//
// Copyright (c) 1991-1996 Microsoft Corporation.  All rights reserved.
//
// -----------------------------------------------------------------


#define     STRICT      // enable strict type checking

#include <windows.h>
#include "dibdll.h"

#pragma data_seg("._WINCAP")
    HWND ghWndMain = 0;         // Handle to main window -- used to post msgs
#pragma data_seg()


HANDLE ghDLLInst = 0;   // Handle to the DLL's instance.
WORD nLangID;

// entry point for DLL loading and unloading

BOOL WINAPI DllMain (HANDLE hModule, DWORD dwFunction, LPVOID lpNot)
{
    ghDLLInst = hModule;

    switch (dwFunction)
    {
        case DLL_PROCESS_ATTACH:
            nLangID = PRIMARYLANGID(GetUserDefaultLangID());            
            break;

        case DLL_PROCESS_DETACH:

               // When we are finally going away, do something here

        default:
            break;
    }
    return TRUE;
}
