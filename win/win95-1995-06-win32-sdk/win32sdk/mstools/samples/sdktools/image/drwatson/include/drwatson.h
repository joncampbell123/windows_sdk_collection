/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    drwatson.h

Abstract:

    Common header file for drwatson data structures.

Author:

    Wesley Witt (wesw) 1-May-1993

Environment:

    User Mode

--*/

#include <imagehlp.h>

typedef struct _tagOPTIONS {
    CHAR                        szLogPath[MAX_PATH];
    CHAR                        szWaveFile[MAX_PATH];
    CHAR                        szCrashDump[MAX_PATH];
    BOOL                        fDumpSymbols;
    BOOL                        fDumpAllThreads;
    BOOL                        fAppendToLogFile;
    BOOL                        fVisual;
    BOOL                        fSound;
    BOOL                        fCrash;
    DWORD                       dwInstructions;
    DWORD                       dwMaxCrashes;
} OPTIONS, *POPTIONS;

typedef struct _tagCRASHES {
    char                        szAppName[256];
    char                        szFunction[256];
    SYSTEMTIME                  time;
    DWORD                       dwExceptionCode;
    DWORD                       dwAddress;
} CRASHES, *PCRASHES;

typedef struct _tagCRASHINFO {
    HWND       hList;
    CRASHES    crash;
    HDC        hdc;
    DWORD      cxExtent;
    DWORD      dwIndex;
    DWORD      dwIndexDesired;
    char       *pCrashData;
    DWORD      dwCrashDataSize;
} CRASHINFO, *PCRASHINFO;

typedef struct _tagTHREADCONTEXT {
    struct _tagTHREADCONTEXT     *next;
    HANDLE                       hThread;
    DWORD                        dwThreadId;
    DWORD                        pc;
    DWORD                        frame;
    DWORD                        stack;
    CONTEXT                      context;
    DWORD                        stackBase;
    DWORD                        stackRA;
    BOOL                         fFaultingContext;
} THREADCONTEXT, *PTHREADCONTEXT;

typedef struct _tagDEBUGPACKET {
    HWND                    hwnd;
    HANDLE                  hEvent;
    OPTIONS                 options;
    DWORD                   dwPidToDebug;
    HANDLE                  hEventToSignal;
    HANDLE                  hProcess;
    DWORD                   dwProcessId;
    PTHREADCONTEXT          tctxHead;
    PTHREADCONTEXT          tctxTail;
    PTHREADCONTEXT          tctx;
    DWORD                   stackBase;
    DWORD                   stackRA;
    EXCEPTION_DEBUG_INFO    ExceptionInfo;
} DEBUGPACKET, *PDEBUGPACKET;

typedef BOOL (CALLBACK* CRASHESENUMPROC)(PCRASHINFO);

#if DBG
#define Assert(exp)    if(!(exp)) {AssertError(#exp,__FILE__,__LINE__);}
#else
#define Assert(exp)
#endif

#define WM_DUMPCOMPLETE       WM_USER+500
#define WM_EXCEPTIONINFO      WM_USER+501
#define WM_ATTACHCOMPLETE     WM_USER+502
