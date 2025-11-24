/*++

Copyright (c) 1992-1993  Microsoft Corporation

Module Name:

    wdbgexts.h

Abstract:

    This file contains procedure prototypes and structures needed to port
    NTSD debugger extensions so that they can be invoked remotely in WinDbg
    command window. This file is to be included by cmdexec0.c and wdbgexts.c.
    To maintain compatibilty with NTSD extensions(or to call the original NTSD
    extensions like "ntsdexts" without modification), definitions in this file
    are incremental by first doing **#include <ntsdexts.h>**.

Author:

    Peter Sun (t-petes) 29-July-1992

Environment:

    runs in the Win32 WinDbg environment.

Revision History:

--*/

#ifndef _WDBGEXTS_
#define _WDBGEXTS_

#ifdef __cplusplus
extern "C" {
#endif

#include <ntsdexts.h>


typedef
BOOL
(*PWINDBG_READ_PROCESS_MEMORY_ROUTINE)(
    DWORD   offset,
    LPVOID  lpBuffer,
    DWORD   cb,
    LPDWORD lpcbBytesRead
    );

typedef
BOOL
(*PWINDBG_WRITE_PROCESS_MEMORY_ROUTINE)(
    DWORD   offset,
    LPVOID  lpBuffer,
    DWORD   cb,
    LPDWORD lpcbBytesWritten
    );

typedef
BOOL
(*PWINDBG_GET_THREAD_CONTEXT_ROUTINE)(
    LPCONTEXT   lpContext,
    DWORD       cbSizeOfContext
    );

typedef
BOOL
(*PWINDBG_SET_THREAD_CONTEXT_ROUTINE)(
    LPCONTEXT   lpContext,
    DWORD       cbSizeOfContext
    );

typedef struct _WINDBG_EXTENSION_APIS {
    DWORD nSize;
    PNTSD_OUTPUT_ROUTINE lpOutputRoutine;
    PNTSD_GET_EXPRESSION lpGetExpressionRoutine;
    PNTSD_GET_SYMBOL lpGetSymbolRoutine;
    PNTSD_DISASM lpDisasmRoutine;
    PNTSD_CHECK_CONTROL_C lpCheckControlCRoutine;
    /*
    **  The above are identical to _NTSD_EXTENSION_API(see ntsdexts.h) to
    **  maintain compatibilty.
    */
    PWINDBG_READ_PROCESS_MEMORY_ROUTINE lpReadProcessMemoryRoutine;
    PWINDBG_WRITE_PROCESS_MEMORY_ROUTINE lpWriteProcessMemoryRoutine;
    PWINDBG_GET_THREAD_CONTEXT_ROUTINE lpGetThreadContextRoutine;
    PWINDBG_SET_THREAD_CONTEXT_ROUTINE lpSetThreadContextRoutine;
} WINDBG_EXTENSION_APIS, *PWINDBG_EXTENSION_APIS;


typedef
VOID
(*PWINDBG_EXTENSION_ROUTINE)(
    HANDLE hCurrentProcess,
    HANDLE hCurrentThread,
    DWORD dwCurrentPc,
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    LPSTR lpArgumentString
    );

#ifdef __cplusplus
}
#endif

#endif // _WDBGEXTS_
