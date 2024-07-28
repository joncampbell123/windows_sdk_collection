//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//  init.c
//
//  Description:
//
//
//
//==========================================================================;

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <msacmdrv.h>

#include "codec.h"
#include "debug.h"


HINSTANCE   ghinst;             // DLL instance handle (module in Win 16)


//==========================================================================;
//
//  WIN 16 SPECIFIC SUPPORT
//
//==========================================================================;

#ifndef WIN32

//--------------------------------------------------------------------------;
//
//  int LibMain
//
//  Description:
//      Library initialization code.
//
//  Arguments:
//      HINSTANCE hinst: Our module handle.
//
//      WORD wDataSeg: Specifies the DS value for this DLL.
//
//      WORD cbHeapSize: The heap size from the .def file.
//
//      LPSTR pszCmdLine: The command line.
//
//  Return (int):
//      Returns non-zero if the initialization was successful and 0 otherwise.
//
//  History:
//      11/15/92    Created. 
//
//--------------------------------------------------------------------------;

int FNGLOBAL LibMain
(
    HINSTANCE               hinst, 
    WORD                    wDataSeg, 
    WORD                    cbHeapSize,
    LPSTR                   pszCmdLine
)
{
    DbgInitialize(TRUE);

    //
    //  if debug level is 5 or greater, then do a DebugBreak() to debug
    //  loading of this driver
    //
    DPF(1, "LibMain(hinst=%.4Xh, wDataSeg=%.4Xh, cbHeapSize=%u, pszCmdLine=%.8lXh)",
        hinst, wDataSeg, cbHeapSize, pszCmdLine);
    DPF(5, "!*** break for debugging ***");

    //
    //  everything looks good to go in Win 16 land.
    //
    ghinst = hinst;

    return (TRUE);
} // LibMain()


//--------------------------------------------------------------------------;
//  
//  int WEP
//  
//  Description:
//  
//  
//  Arguments:
//      WORD wUselessParam:
//  
//  Return (int):
//  
//  History:
//      03/28/93    Created.
//  
//--------------------------------------------------------------------------;

EXTERN_C int FNEXPORT WEP
(
    WORD                    wUselessParam
)
{
    DPF(1, "WEP(wUselessParam=%u)", wUselessParam);

    //
    //  always return 1.
    //
    return (1);
} // WEP()

#else


//==========================================================================;
//
//  WIN 32 SPECIFIC SUPPORT
//
//==========================================================================;

//--------------------------------------------------------------------------;
//
//  BOOL DllEntryPoint
//
//  Description:
//      This is the standard DLL entry point for Win 32.
//
//  Arguments:
//      HANDLE hinst: Our DLL instance handle.
//
//      DWORD dwReason: The reason we've been called--process/thread attach
//      and detach.
//
//      LPVOID pReserved: Reserved. Should be NULL--so ignore it.
//
//  Return (BOOL):
//      Returns non-zero if the initialization was successful and 0 otherwise.
//
//  History:
//      11/15/92    Created. 
//
//--------------------------------------------------------------------------;

EXTERN_C BOOL FNEXPORT DllEntryPoint
(
    HANDLE                  hinst,
    DWORD                   dwReason,
    LPVOID                  pReserved
)
{
    DbgInitialize(TRUE);

    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            DPF(1, "DllEntryPoint(hinst=%.08lXh, DLL_PROCESS_ATTACH)", hinst);
            return (TRUE);

        case DLL_PROCESS_DETACH:
            DPF(1, "DllEntryPoint(hinst=%.08lXh, DLL_PROCESS_DETACH)", hinst);
            return (TRUE);

        case DLL_THREAD_ATTACH:
            DPF(1, "DllEntryPoint(hinst=%.08lXh, DLL_THREAD_ATTACH)", hinst);
            return (TRUE);

        case DLL_THREAD_DETACH:
            DPF(1, "DllEntryPoint(hinst=%.08lXh, DLL_THREAD_DETACH)", hinst);
            return (TRUE);
    }

    return (TRUE);
} // DllEntryPoint()

#endif

