/*++

Copyright (c) 1992-1993  Microsoft Corporation

Module Name:

    wdbgexts.h

Abstract:

    This file contains the necessary prototypes and data types for a user
    to write a debugger extension DLL.  This header file is also included
    by the NT debuggers (WINDBG & KD).

    This header file must be included after "windows.h" and "imagehlp.h".

    Please see the NT DDK documentation for specific information about
    how to write your own debugger extension DLL.

Author:

    Wesley Witt (wesw) 1-july-1994

Environment:

    Win32 only.

Revision History:

--*/

#ifndef _WDBGEXTS_
#define _WDBGEXTS_

#ifdef __cplusplus
extern "C" {
#endif


typedef
VOID
(*PWINDBG_OUTPUT_ROUTINE)(
    PSTR lpFormat,
    ...
    );

typedef
ULONG
(*PWINDBG_GET_EXPRESSION)(
    PSTR lpExpression
    );

typedef
VOID
(*PWINDBG_GET_SYMBOL)(
    PVOID   offset,
    PUCHAR  pchBuffer,
    PULONG  pDisplacement
    );

typedef
ULONG
(*PWINDBG_DISASM)(
    PULONG lpOffset,
    PSTR   lpBuffer,
    ULONG  fShowEfeectiveAddress
    );

typedef
ULONG
(*PWINDBG_CHECK_CONTROL_C)(
    VOID
    );

typedef
ULONG
(*PWINDBG_READ_PROCESS_MEMORY_ROUTINE)(
    ULONG  offset,
    PVOID  lpBuffer,
    ULONG  cb,
    PULONG lpcbBytesRead
    );

typedef
ULONG
(*PWINDBG_WRITE_PROCESS_MEMORY_ROUTINE)(
    ULONG  offset,
    PVOID  lpBuffer,
    ULONG  cb,
    PULONG lpcbBytesWritten
    );

typedef
ULONG
(*PWINDBG_GET_THREAD_CONTEXT_ROUTINE)(
    ULONG       Processor,
    PCONTEXT    lpContext,
    ULONG       cbSizeOfContext
    );

typedef
ULONG
(*PWINDBG_SET_THREAD_CONTEXT_ROUTINE)(
    ULONG       Processor,
    PCONTEXT    lpContext,
    ULONG       cbSizeOfContext
    );

typedef
ULONG
(*PWINDBG_IOCTL_ROUTINE)(
    USHORT   IoctlType,
    PVOID   lpvData,
    ULONG    cbSize
    );

typedef
ULONG
(*PWINDBG_OLDKD_READ_PHYSICAL_MEMORY)(
    PHYSICAL_ADDRESS address,
    PVOID            buffer,
    ULONG            count,
    PULONG           bytesread
    );

typedef
ULONG
(*PWINDBG_OLDKD_WRITE_PHYSICAL_MEMORY)(
    PHYSICAL_ADDRESS address,
    PVOID           buffer,
    ULONG            length,
    PULONG           byteswritten
    );


typedef struct _tagEXTSTACKTRACE {
    ULONG       FramePointer;
    ULONG       ProgramCounter;
    ULONG       ReturnAddress;
    ULONG       Args[4];
} EXTSTACKTRACE, *PEXTSTACKTRACE;


typedef
ULONG
(*PWINDBG_STACKTRACE_ROUTINE)(
    ULONG             FramePointer,
    ULONG             StackPointer,
    ULONG             ProgramCounter,
    PEXTSTACKTRACE    StackFrames,
    ULONG             Frames
    );

typedef struct _WINDBG_EXTENSION_APIS {
    ULONG                                  nSize;
    PWINDBG_OUTPUT_ROUTINE                 lpOutputRoutine;
    PWINDBG_GET_EXPRESSION                 lpGetExpressionRoutine;
    PWINDBG_GET_SYMBOL                     lpGetSymbolRoutine;
    PWINDBG_DISASM                         lpDisasmRoutine;
    PWINDBG_CHECK_CONTROL_C                lpCheckControlCRoutine;
    PWINDBG_READ_PROCESS_MEMORY_ROUTINE    lpReadProcessMemoryRoutine;
    PWINDBG_WRITE_PROCESS_MEMORY_ROUTINE   lpWriteProcessMemoryRoutine;
    PWINDBG_GET_THREAD_CONTEXT_ROUTINE     lpGetThreadContextRoutine;
    PWINDBG_SET_THREAD_CONTEXT_ROUTINE     lpSetThreadContextRoutine;
    PWINDBG_IOCTL_ROUTINE                  lpIoctlRoutine;
    PWINDBG_STACKTRACE_ROUTINE             lpStackTraceRoutine;
} WINDBG_EXTENSION_APIS, *PWINDBG_EXTENSION_APIS;

typedef struct _WINDBG_OLD_EXTENSION_APIS {
    ULONG                                  nSize;
    PWINDBG_OUTPUT_ROUTINE                 lpOutputRoutine;
    PWINDBG_GET_EXPRESSION                 lpGetExpressionRoutine;
    PWINDBG_GET_SYMBOL                     lpGetSymbolRoutine;
    PWINDBG_DISASM                         lpDisasmRoutine;
    PWINDBG_CHECK_CONTROL_C                lpCheckControlCRoutine;
} WINDBG_OLD_EXTENSION_APIS, *PWINDBG_OLD_EXTENSION_APIS;

typedef struct _WINDBG_OLDKD_EXTENSION_APIS {
    ULONG                                  nSize;
    PWINDBG_OUTPUT_ROUTINE                 lpOutputRoutine;
    PWINDBG_GET_EXPRESSION                 lpGetExpressionRoutine;
    PWINDBG_GET_SYMBOL                     lpGetSymbolRoutine;
    PWINDBG_DISASM                         lpDisasmRoutine;
    PWINDBG_CHECK_CONTROL_C                lpCheckControlCRoutine;
    PWINDBG_READ_PROCESS_MEMORY_ROUTINE    lpReadVirtualMemRoutine;
    PWINDBG_WRITE_PROCESS_MEMORY_ROUTINE   lpWriteVirtualMemRoutine;
    PWINDBG_OLDKD_READ_PHYSICAL_MEMORY     lpReadPhysicalMemRoutine;
    PWINDBG_OLDKD_WRITE_PHYSICAL_MEMORY    lpWritePhysicalMemRoutine;
} WINDBG_OLDKD_EXTENSION_APIS, *PWINDBG_OLDKD_EXTENSION_APIS;

typedef
VOID
(*PWINDBG_OLD_EXTENSION_ROUTINE)(
    HANDLE                  hCurrentProcess,
    HANDLE                  hCurrentThread,
    ULONG                   dwCurrentPc,
    PWINDBG_EXTENSION_APIS  lpExtensionApis,
    PSTR                    lpArgumentString
    );

typedef
VOID
(*PWINDBG_EXTENSION_ROUTINE)(
    HANDLE                  hCurrentProcess,
    HANDLE                  hCurrentThread,
    ULONG                   dwCurrentPc,
    ULONG                   dwProcessor,
    PSTR                    lpArgumentString
    );

typedef
VOID
(*PWINDBG_OLDKD_EXTENSION_ROUTINE)(
    ULONG                        dwCurrentPc,
    PWINDBG_OLDKD_EXTENSION_APIS lpExtensionApis,
    PSTR                        lpArgumentString
    );

typedef
VOID
(*PWINDBG_EXTENSION_DLL_INIT)(
    PWINDBG_EXTENSION_APIS lpExtensionApis,
    USHORT                 MajorVersion,
    USHORT                 MinorVersion
    );

typedef
ULONG
(*PWINDBG_CHECK_VERSION)(
    VOID
    );

#define EXT_API_VERSION_NUMBER 2

typedef struct EXT_API_VERSION {
    USHORT  MajorVersion;
    USHORT  MinorVersion;
    USHORT  Revision;
    USHORT  Reserved;
} EXT_API_VERSION, *LPEXT_API_VERSION;

typedef
LPEXT_API_VERSION
(*PWINDBG_EXTENSION_API_VERSION)(
    VOID
    );

#define IG_KD_CONTEXT               1
#define IG_READ_CONTROL_SPACE       2
#define IG_WRITE_CONTROL_SPACE      3
#define IG_READ_IO_SPACE            4
#define IG_WRITE_IO_SPACE           5
#define IG_READ_PHYSICAL            6
#define IG_WRITE_PHYSICAL           7
#define IG_READ_IO_SPACE_EX         8
#define IG_WRITE_IO_SPACE_EX        9

typedef struct _tagPROCESSORINFO {
    USHORT      Processor;                // current processor
    USHORT      NumberProcessors;         // total number of processors
} PROCESSORINFO, *PPROCESSORINFO;

typedef struct _tagREADCONTROLSPACE {
    ULONG       Processor;
    ULONG       Address;
    ULONG       BufLen;
    UCHAR       Buf[1];
} READCONTROLSPACE, *PREADCONTROLSPACE;

typedef struct _tagIOSPACE {
    ULONG       Address;
    ULONG       Length;                   // 1, 2, or 4 bytes
    ULONG       Data;
} IOSPACE, *PIOSPACE;

typedef struct _tagIOSPACE_EX {
    ULONG       Address;
    ULONG       Length;                   // 1, 2, or 4 bytes
    ULONG       Data;
    ULONG       InterfaceType;
    ULONG       BusNumber;
    ULONG       AddressSpace;
} IOSPACE_EX, *PIOSPACE_EX;

typedef struct _tagPHYSICAL {
    PHYSICAL_ADDRESS       Address;
    ULONG                  BufLen;
    UCHAR                  Buf[1];
} PHYSICAL, *PPHYSICAL;


#ifdef __cplusplus
#define CPPMOD extern "C"
#else
#define CPPMOD
#endif


#define DECLARE_API(s)                             \
    CPPMOD VOID                                    \
    s(                                             \
        HANDLE                 hCurrentProcess,    \
        HANDLE                 hCurrentThread,     \
        ULONG                  dwCurrentPc,        \
        ULONG                  dwProcessor,        \
        PSTR                  args                \
     )

#ifndef NOEXTAPI

#define dprintf          (ExtensionApis.lpOutputRoutine)
#define GetExpression    (ExtensionApis.lpGetExpressionRoutine)
#define GetSymbol        (ExtensionApis.lpGetSymbolRoutine)
#define Disassm          (ExtensionApis.lpDisasmRoutine)
#define CheckControlC    (ExtensionApis.lpCheckControlCRoutine)
#define ReadMemory       (ExtensionApis.lpReadProcessMemoryRoutine)
#define WriteMemory      (ExtensionApis.lpWriteProcessMemoryRoutine)
#define GetContext       (ExtensionApis.lpGetThreadContextRoutine)
#define SetContext       (ExtensionApis.lpSetThreadContextRoutine)
#define Ioctl            (ExtensionApis.lpIoctlRoutine)
#define StackTrace       (ExtensionApis.lpStackTraceRoutine)

#define GetKdContext(ppi) \
    Ioctl( IG_KD_CONTEXT, (PVOID)ppi, sizeof(*ppi) )

#define ReadControlSpace(processor,address,buf,size) \
    { \
        PREADCONTROLSPACE prc; \
        prc = malloc( sizeof(*prc) + size ); \
        ZeroMemory( prc->Buf, size ); \
        prc->Processor = (ULONG)processor; \
        prc->Address = (ULONG)address; \
        prc->BufLen = size; \
        Ioctl( IG_READ_CONTROL_SPACE, (PVOID)prc, sizeof(*prc) + size ); \
        memcpy( buf, prc->Buf, size ); \
        free( prc ); \
    }

#define ReadIoSpace(address,data,size) \
    { \
        IOSPACE is; \
        is.Address = (ULONG)address; \
        is.Length = *size; \
        is.Data = 0; \
        Ioctl( IG_READ_IO_SPACE, (PVOID)&is, sizeof(is) ); \
        *data = is.Data; \
        *size = is.Length; \
    }

#define WriteIoSpace(address,data,size) \
    { \
        IOSPACE is; \
        is.Address = (ULONG)address; \
        is.Length = *size; \
        is.Data = data; \
        Ioctl( IG_WRITE_IO_SPACE, (PVOID)&is, sizeof(is) ); \
        *size = is.Length; \
    }

#define ReadIoSpaceEx(address,data,size,interfacetype,busnumber,addressspace) \
    { \
        IOSPACE_EX is; \
        is.Address = (ULONG)address; \
        is.Length = *size; \
        is.Data = 0; \
        is.InterfaceType = interfacetype; \
        is.BusNumber = busnumber; \
        is.AddressSpace = addressspace; \
        Ioctl( IG_READ_IO_SPACE_EX, (PVOID)&is, sizeof(is) ); \
        *data = is.Data; \
        *size = is.Length; \
    }

#define WriteIoSpaceEx(address,data,size,interfacetype,busnumber,addressspace) \
    { \
        IOSPACE_EX is; \
        is.Address = (ULONG)address; \
        is.Length = *size; \
        is.Data = data; \
        is.InterfaceType = interfacetype; \
        is.BusNumber = busnumber; \
        is.AddressSpace = addressspace; \
        Ioctl( IG_WRITE_IO_SPACE_EX, (PVOID)&is, sizeof(is) ); \
        *size = is.Length; \
    }


#define ReadPhysical(address,buf,size,sizer) \
    { \
        PPHYSICAL phy; \
        phy = malloc( sizeof(*phy) + size ); \
        ZeroMemory( phy->Buf, size ); \
        phy->Address = address; \
        phy->BufLen = size; \
        Ioctl( IG_READ_PHYSICAL, (PVOID)phy, sizeof(*phy) + size ); \
        *sizer = phy->BufLen; \
        memcpy( buf, phy->Buf, *sizer ); \
        free( phy ); \
    }

#define WritePhysical(address,buf,size,sizew) \
    { \
        PPHYSICAL phy; \
        phy = malloc( sizeof(*phy) + size ); \
        ZeroMemory( phy->Buf, size ); \
        phy->Address = address; \
        phy->BufLen = size; \
        memcpy( phy->Buf, buf, size ); \
        Ioctl( IG_WRITE_PHYSICAL, (PVOID)phy, sizeof(*phy) + size ); \
        *sizew = phy->BufLen; \
        free( phy ); \
    }
#endif


#ifdef __cplusplus
}
#endif

#endif // _WDBGEXTS_
