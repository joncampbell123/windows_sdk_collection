// -----------------------------------------------------------------
// File name:  DLLINIT.C
//
// This is the DLL's initialization source file.  It contains DllEntry,
// the Win32 DLL's entry and exit point.
//
// Development Team:  Dan Ruder
//                    Mark Bader
//                    Hung H. Nguyen
//
// Written by Microsoft Product Support Services, Windows Developer Support.
//
// Copyright (c) 1991, 1992 Microsoft Corporation.  All rights reserved.
//
// History:
//          Date        Author         Reason
//          1/30/92     Dan Ruder      Created
//          12/9/92     Mark Bader     Ported to Win32 & added strict type checking
//          1/26/94     Hung Nguyen    Initialized ghDLLInst and changed entry
//                                     point from DLLEntry to DllMain
//          1/27/94     Hung Nguyen    Added data_seg section
// -----------------------------------------------------------------


#define     STRICT      // enable strict type checking

#include <windows.h>
#include "dibdll.h"

#pragma data_seg("._WINCAP")
    HWND ghWndMain = 0;         // Handle to main window -- used to post msgs
#pragma data_seg()


HANDLE ghDLLInst = 0;   // Handle to the DLL's instance.

// entry point for DLL loading and unloading

BOOL WINAPI DllMain (HANDLE hModule, DWORD dwFunction, LPVOID lpNot)
{
    ghDLLInst = hModule;

    switch (dwFunction)
    {
        case DLL_PROCESS_ATTACH:

               // If ya want to do something when we first get loaded,
               // do it here

        case DLL_PROCESS_DETACH:

               // When we are finally going away, do something here

        default:
            break;
    }
    return TRUE;
}
