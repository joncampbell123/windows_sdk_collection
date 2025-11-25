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

typedef struct _RUNTIME_FUNCTION {
    ULONG BeginAddress;
    ULONG EndAddress;
    PVOID ExceptionHandler;
    PVOID HandlerData;
    ULONG PrologEndAddress;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;

typedef struct _tagOPTIONS {
    char                        szLogPath[MAX_PATH];
    char                        szWaveFile[MAX_PATH];
    BOOL                        fDumpSymbols;
    BOOL                        fDumpAllThreads;
    BOOL                        fAppendToLogFile;
    BOOL                        fVisual;
    BOOL                        fSound;
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

typedef struct _tagSYMBOL {
    struct _tagSYMBOL   *next;
    DWORD               addr;
    DWORD               size;
    unsigned char       szName[1];
} SYMBOL, *PSYMBOL;

typedef struct _tagMODULEINFO {
    struct _tagMODULEINFO     *next;          // pointer to next module
    DWORD                     dwBaseOfImage;  // based address
    DWORD                     dwLoadAddress;  // actual load address
    DWORD                     dwImageSize;    // size of image in bytes
    PFPO_DATA                 pFpoData;       // pointer to fpo data (x86)
    PRUNTIME_FUNCTION         pExceptionData; // pointer to pdata (risc)
    DWORD                     dwEntries;      // number of fpo or pdata recs
    PSYMBOL                   *symbolTable;   // pointer to symbol table
    DWORD                     numsyms;        // number of symbols in table
    char                      szName[1024];   // module name
} MODULEINFO, *PMODULEINFO;

typedef struct _tagSTACKWALK {
    DWORD               pc;
    DWORD               frame;
    long                ul;
    DWORD               params[4];
    PFPO_DATA           pFpoData;
} STACKWALK, *PSTACKWALK;

typedef struct _tagTHREADCONTEXT {
    struct _tagTHREADCONTEXT     *next;
    HANDLE                       hThread;
    DWORD                        dwThreadId;
    DWORD                        pc;
    DWORD                        frame;
    DWORD                        stack;
    CONTEXT                      context;
    PMODULEINFO                  mi;                   // valid to be NULL
    DWORD                        stackBase;
    DWORD                        stackRA;
    BOOL                         fFaultingContext;
} THREADCONTEXT, *PTHREADCONTEXT;

typedef struct _tagDEBUGPACKET {
    HWND                hwnd;
    HANDLE              hEvent;
    OPTIONS             options;
    DWORD               dwPidToDebug;
    HANDLE              hEventToSignal;
    HANDLE              hProcess;
    DWORD               dwProcessId;
    PMODULEINFO         miHead;
    PMODULEINFO         miTail;
    PTHREADCONTEXT      tctxHead;
    PTHREADCONTEXT      tctxTail;
    PTHREADCONTEXT      tctx;
    DWORD               stackBase;
    DWORD               stackRA;
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
