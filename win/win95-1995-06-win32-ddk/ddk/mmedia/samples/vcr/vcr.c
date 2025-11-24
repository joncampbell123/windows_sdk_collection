/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <stdlib.h>

#ifndef _WIN32
BOOL FAR PASCAL LibMain (HANDLE hModule, int cbHeap, LPSTR lpchCmdLine)
{
    return TRUE;
}
#else
BOOL WINAPI DLLEntryPoint(PVOID hModule, ULONG Reason, PCONTEXT pContext);
BOOL WINAPI DLLEntryPoint(PVOID hModule, ULONG Reason, PCONTEXT pContext)
{
    if (Reason == DLL_PROCESS_ATTACH)
    {
    } else
    {
        if (Reason == DLL_PROCESS_DETACH)
        {
        }
    }
    return TRUE;
}
#endif
