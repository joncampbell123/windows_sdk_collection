/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    crash.h

Abstract:

    This module implements support for handling crash dump files.

    *** Use this file when linking againts crashxxx.lib

Author:

    Lou Perazzoli (Loup) 10-Nov-1993
    Wesley Witt   (wesw) 1-Dec-1993   (additional work)

Environment:

    NT 3.5

Revision History:

--*/

#ifndef _CRASHLIB_
#define _CRASHLIB_

#ifdef __cplusplus
#pragma warning(disable:4200)
extern "C" {
#endif


typedef struct _USERMODE_CRASHDUMP_HEADER {
    DWORD       Signature;
    DWORD       ValidDump;
    DWORD       MajorVersion;
    DWORD       MinorVersion;
    DWORD       MachineImageType;
    DWORD       ThreadCount;
    DWORD       ModuleCount;
    DWORD       MemoryRegionCount;
    DWORD       ThreadOffset;
    DWORD       ModuleOffset;
    DWORD       DataOffset;
    DWORD       MemoryRegionOffset;
    DWORD       ExceptionOffset;
} USERMODE_CRASHDUMP_HEADER, *PUSERMODE_CRASHDUMP_HEADER;

typedef struct _CRASH_MODULE {
    DWORD       BaseOfImage;
    DWORD       SizeOfImage;
    DWORD       ImageNameLength;
    CHAR        ImageName[0];
} CRASH_MODULE, *PCRASH_MODULE;


//
// usermode crash dump data types
//
#define DMP_EXCEPTION                 1
#define DMP_MEMORY_BASIC_INFORMATION  2
#define DMP_THREAD_CONTEXT            3
#define DMP_MODULE                    4
#define DMP_MEMORY_DATA               5

//
// usermode crashdump callback function
//
typedef BOOL  (*PDMP_CREATE_DUMP_CALLBACK)(
    DWORD       DataType,
    PVOID*      DumpData,
    LPDWORD     DumpDataLength,
    PVOID       UserData
    );

BOOL
DmpCreateUserDump(
    LPSTR                       CrashDumpName,
    PDMP_CREATE_DUMP_CALLBACK   DmpCallback,
    PVOID                       lpv
    );


BOOL
DmpInitialize (
    IN  LPSTR               FileName,
    OUT PCONTEXT            *Context,
    OUT PEXCEPTION_RECORD   *Exception,
    OUT PVOID               *DmpHeader
    );

VOID
DmpUnInitialize (
    VOID
    );

DWORD
DmpReadMemory (
    IN PVOID BaseAddress,
    IN PVOID Buffer,
    IN ULONG Size
    );

DWORD
DmpWriteMemory (
    IN PVOID BaseAddress,
    IN PVOID Buffer,
    IN ULONG Size
    );

PVOID
VaToLocation (
    IN PVOID VirtualAddress
    );

PVOID
PhysicalToLocation (
    IN PVOID PhysicalAddress
    );

PVOID
PageToLocation (
    IN ULONG Page
    );

ULONG
GetPhysicalPage (
    IN PVOID PhysicalAddress
    );

BOOL
MapDumpFile(
    IN  LPSTR  FileName
    );

ULONG
PteToPfn (
    IN ULONG Pte
    );

ULONG
GetPhysicalPage (
    IN PVOID PhysicalAddress
    );

DWORD
DmpReadPhysicalMemory (
    IN PVOID BaseAddress,
    IN PVOID Buffer,
    IN ULONG Size
    );

DWORD
DmpWritePhysicalMemory (
    IN PVOID BaseAddress,
    IN PVOID Buffer,
    IN ULONG Size
    );

BOOL
DmpReadControlSpace(
    USHORT  Processor,
    PVOID   TargetBaseAddress,
    PVOID   UserInterfaceBuffer,
    ULONG   TransferCount,
    PULONG  ActualBytesRead
    );

BOOL
DmpGetContext(
    IN  ULONG     Processor,
    OUT PVOID     Context
    );

INT
DmpGetCurrentProcessor(
    VOID
    );

#ifdef __cplusplus
}
#pragma warning(default:4200)
#endif

#endif
