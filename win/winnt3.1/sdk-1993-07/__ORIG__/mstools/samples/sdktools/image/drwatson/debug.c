/*++

Copyright (c) 1993  Microsoft Corporation

Module Name:

    debug.c

Abstract:

    This file implements the debug module for drwatson.  This module
    processes all debug events and generates the postmortem dump.

Author:

    Wesley Witt (wesw) 1-May-1993

Environment:

    User Mode

--*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "drwatson.h"
#include "proto.h"
#include "messages.h"
#include "resource.h"

typedef struct _tagSYSINFO {
    char    szUserName[MAX_PATH];
    char    szMachineName[MAX_PATH];
} SYSINFO, *PSYSINFO;


#define DBG_EXCEPTION_HANDLED           ((DWORD)0x00010001L)
#define STATUS_POSSIBLE_DEADLOCK        ((DWORD)0xC0000194L)
#define STATUS_SEGMENT_NOTIFICATION     ((DWORD)0x40000005L)
#define STATUS_VDM_EVENT                STATUS_SEGMENT_NOTIFICATION

PMODULEINFO AllocMi( PDEBUGPACKET dp );
PTHREADCONTEXT AllocTctx( PDEBUGPACKET dp );
void PostMortemDump( PDEBUGPACKET dp, LPEXCEPTION_DEBUG_INFO ed );
void AttachToActiveProcess ( PDEBUGPACKET dp );
void ProcessCreateProcess( PDEBUGPACKET dp, LPDEBUG_EVENT de );
void ProcessCreateThread( PDEBUGPACKET dp, LPDEBUG_EVENT de );
void ProcessLoadDll( PDEBUGPACKET dp, LPDEBUG_EVENT de );
void LogSystemInformation( PDEBUGPACKET dp );
DWORD SysInfoThread( PSYSINFO si );
void LogDisassembly( PDEBUGPACKET dp, PCRASHES pCrash );
void LogStackWalk( PDEBUGPACKET dp );
void LogStackDump( PDEBUGPACKET dp );
char * GetExceptionText( DWORD dwExceptionCode );


DWORD
DispatchDebugEventThread( PDEBUGPACKET dp )

/*++

Routine Description:

    This is the entry point for DRWTSN32

Arguments:

    None.

Return Value:

    None.

--*/

{
    DEBUG_EVENT   de;
    DWORD         rc = 0;
    char          szLogFileName[1024];
    char          buf[1024];


    if (dp->dwPidToDebug == 0) {
        rc = 1;
        goto exit;
    }

    SetErrorMode( SEM_FAILCRITICALERRORS |
                  SEM_NOGPFAULTERRORBOX  |
                  SEM_NOOPENFILEERRORBOX   );

    AttachToActiveProcess( dp );

    strcpy( szLogFileName, dp->options.szLogPath );
    MakeLogFileName( szLogFileName );
    OpenLogFile( szLogFileName,
                 dp->options.fAppendToLogFile,
                 dp->options.fVisual
               );

    while (TRUE) {
        if (!WaitForDebugEvent( &de, 10000 )) {
            rc = GetLastError();
            goto exit;
        }
        switch (de.dwDebugEventCode) {
            case EXCEPTION_DEBUG_EVENT:
                if (de.u.Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT) {
                    if (de.u.Exception.dwFirstChance) {
                        if ( dp->hEventToSignal ) {
                            SetEvent(dp->hEventToSignal);
                            dp->hEventToSignal = 0L;
                        }
                        ContinueDebugEvent( de.dwProcessId, de.dwThreadId, DBG_EXCEPTION_HANDLED );
                        continue;
                    }
                }
                if (dp->options.fVisual) {
                    //
                    // this notification is necessary because the shell must know when
                    // the debugee has been attached.  if it doesn't know and the user is
                    // allowed to terminate drwatson then the system may intervene with
                    // a popup.
                    //
                    SendMessage( dp->hwnd, WM_ATTACHCOMPLETE, 0, 0 );
                    wsprintf( buf,
                              LoadRcString( IDS_AE_TEXT ),
                              GetExceptionText(de.u.Exception.ExceptionRecord.ExceptionCode),
                              de.u.Exception.ExceptionRecord.ExceptionCode,
                              de.u.Exception.ExceptionRecord.ExceptionAddress );
                    SendMessage( dp->hwnd, WM_EXCEPTIONINFO, 0, (LPARAM) buf );
                }
                PostMortemDump( dp, &de.u.Exception );
                ContinueDebugEvent( de.dwProcessId, de.dwThreadId, DBG_EXCEPTION_NOT_HANDLED );
                continue;

            case CREATE_THREAD_DEBUG_EVENT:
                ProcessCreateThread( dp, &de );
                break;

            case CREATE_PROCESS_DEBUG_EVENT:
                ProcessModuleLoad( dp, &de );
                de.u.CreateThread.hThread = de.u.CreateProcessInfo.hThread;
                ProcessCreateThread( dp, &de );
                break;

            case EXIT_THREAD_DEBUG_EVENT:
                break;

            case EXIT_PROCESS_DEBUG_EVENT:
                goto exit;
                break;

            case LOAD_DLL_DEBUG_EVENT:
                ProcessModuleLoad( dp, &de );
                break;

            case UNLOAD_DLL_DEBUG_EVENT:
                break;

            case OUTPUT_DEBUG_STRING_EVENT:
                break;

            case RIP_EVENT:
                break;

            default:
                lprintf( MSG_INVALID_DEBUG_EVENT, de.dwDebugEventCode );
                break;
        }
        ContinueDebugEvent( de.dwProcessId, de.dwThreadId, DBG_CONTINUE );
    }

exit:
    CloseLogFile();

    if (dp->options.fVisual) {
        SendMessage( dp->hwnd, WM_DUMPCOMPLETE, 0, 0 );
    }

    return 0;
}

PTHREADCONTEXT
AllocTctx( PDEBUGPACKET dp )
{
    PTHREADCONTEXT ptctx;

    ptctx = (PTHREADCONTEXT) malloc( sizeof(THREADCONTEXT) );
    if (ptctx == NULL) {
        if (dp->options.fVisual) {
            FatalError( LoadRcString(IDS_MEMORY) );
        }
        else {
            ExitProcess( 1 );
        }
    }
    memset( ptctx, 0, sizeof(THREADCONTEXT) );

    if (dp->tctxHead == NULL) {
        dp->tctxHead = dp->tctxTail = ptctx;
    }
    else {
        dp->tctxTail->next = ptctx;
        dp->tctxTail = ptctx;
    }

    return ptctx;
}

void
ProcessCreateThread( PDEBUGPACKET dp, LPDEBUG_EVENT de )
{
    dp->tctx              = AllocTctx( dp );
    dp->tctx->hThread     = de->u.CreateThread.hThread;
    dp->tctx->dwThreadId  = de->dwThreadId;

    return;
}

void
PostMortemDump( PDEBUGPACKET dp, LPEXCEPTION_DEBUG_INFO ed )
{
    PMODULEINFO       mi = dp->miHead;
    PTHREADCONTEXT    ptctx;
    char              dbuf[1024];
    char              szDate[20];
    char              szTime[20];
    CRASHES           crash;
    DWORD             dwThreadId;
    HANDLE            hThread;


    GetLocalTime( &crash.time );
    crash.dwExceptionCode = ed->ExceptionRecord.ExceptionCode;
    crash.dwAddress = (DWORD)ed->ExceptionRecord.ExceptionAddress;
    strcpy( crash.szAppName, dp->miHead->szName );

    lprintf( MSG_APP_EXCEPTION );
    wsprintf( dbuf, "%d", dp->dwPidToDebug );
    lprintf( MSG_APP_EXEP_NAME, crash.szAppName, dbuf );
    wsprintf( szDate, "%d/%d/%d", crash.time.wMonth,
                                  crash.time.wDay,
                                  crash.time.wYear );
    wsprintf( szTime, "%d:%d:%d.%d", crash.time.wHour,
                                     crash.time.wMinute,
                                     crash.time.wSecond,
                                     crash.time.wMilliseconds );
    lprintf( MSG_APP_EXEP_WHEN, szDate, szTime );
    wsprintf( dbuf, "%08lx", ed->ExceptionRecord.ExceptionCode );
    lprintf( MSG_EXCEPTION_NUMBER, dbuf );


    lprintfs( "(%s)\r\n\r\n",
              GetExceptionText(ed->ExceptionRecord.ExceptionCode) );

    LogSystemInformation( dp );

    LogTaskList();

    mi = dp->miHead;
    lprintf( MSG_MODULE_LIST );
    while (mi) {
        lprintfs( "(%08x - %08x) %s\r\n",
                  (DWORD)mi->dwLoadAddress,
                  (DWORD)mi->dwLoadAddress + mi->dwImageSize,
                  mi->szName
                );
        mi = mi->next;
    }
    lprintfs( "\r\n" );

    ptctx = dp->tctxHead;
    while (ptctx) {
        dp->tctx = ptctx;

        GetContextForThread( dp );

        if (ptctx->pc == (DWORD)ed->ExceptionRecord.ExceptionAddress) {
            ptctx->fFaultingContext = TRUE;
        }

        if ((!ptctx->fFaultingContext && dp->options.fDumpAllThreads) || ptctx->fFaultingContext) {
            wsprintf( dbuf, "%x", dp->tctx->dwThreadId );
            lprintf( MSG_STATE_DUMP, dbuf );
            OutputAllRegs( dp );
            LogDisassembly( dp, &crash );
            LogStackWalk( dp );
            LogStackDump( dp );
        }

        ptctx = ptctx->next;
        if (ptctx->next == NULL) {
            // this forces the loop to ignore the last thread
            // we want to do this because the last thread is a
            // remote thread created by kernel32.dll for the purpose
            // of attaching the debugger to the faulting process
            break;
        }
    }

    if (dp->options.fDumpSymbols) {
        DumpSymbols( dp );
    }

    ElSaveCrash( &crash, dp->options.dwMaxCrashes );

    hThread = CreateThread( NULL,
                            16000,
                            (LPTHREAD_START_ROUTINE)TerminationThread,
                            dp,
                            THREAD_SET_INFORMATION,
                            (LPDWORD)&dwThreadId
                          );

    WaitForSingleObject( hThread, 30000 );

    return;
}

void
LogStackDump( PDEBUGPACKET dp )
{
    DWORD   i;
    DWORD   j;
    BYTE    stack[1024];



    memset( stack, 0, sizeof(stack) );
    if (!ReadProcessMemory( dp->hProcess,
                            (LPVOID)dp->tctx->stack,
                            (LPVOID)stack,
                            sizeof(stack),
                            (LPDWORD)&i )) {
        return;
    }

    lprintf( MSG_STACK_DUMP_HEADER );

    for( i = 0; i < 20; i++ ) {
        j = i * 16;
        lprintfs( "%08x  %02x %02x %02x %02x %02x %02x %02x %02x - %02x %02x %02x %02x %02x %02x %02x %02x  %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\r\n",
                  j + dp->tctx->stack,
                  stack[ j +  0 ],
                  stack[ j +  1 ],
                  stack[ j +  2 ],
                  stack[ j +  3 ],
                  stack[ j +  4 ],
                  stack[ j +  5 ],
                  stack[ j +  6 ],
                  stack[ j +  7 ],
                  stack[ j +  8 ],
                  stack[ j +  9 ],
                  stack[ j + 10 ],
                  stack[ j + 11 ],
                  stack[ j + 12 ],
                  stack[ j + 13 ],
                  stack[ j + 14 ],
                  stack[ j + 15 ],
                  isprint( stack[ j +  0 ]) ? stack[ j +  0 ] : '.',
                  isprint( stack[ j +  1 ]) ? stack[ j +  1 ] : '.',
                  isprint( stack[ j +  2 ]) ? stack[ j +  2 ] : '.',
                  isprint( stack[ j +  3 ]) ? stack[ j +  3 ] : '.',
                  isprint( stack[ j +  4 ]) ? stack[ j +  4 ] : '.',
                  isprint( stack[ j +  5 ]) ? stack[ j +  5 ] : '.',
                  isprint( stack[ j +  6 ]) ? stack[ j +  6 ] : '.',
                  isprint( stack[ j +  7 ]) ? stack[ j +  7 ] : '.',
                  isprint( stack[ j +  8 ]) ? stack[ j +  8 ] : '.',
                  isprint( stack[ j +  9 ]) ? stack[ j +  9 ] : '.',
                  isprint( stack[ j + 10 ]) ? stack[ j + 10 ] : '.',
                  isprint( stack[ j + 11 ]) ? stack[ j + 11 ] : '.',
                  isprint( stack[ j + 12 ]) ? stack[ j + 12 ] : '.',
                  isprint( stack[ j + 13 ]) ? stack[ j + 13 ] : '.',
                  isprint( stack[ j + 14 ]) ? stack[ j + 14 ] : '.',
                  isprint( stack[ j + 15 ]) ? stack[ j + 15 ] : '.'
                );
    }

    lprintfs( "\r\n" );
    return;
}

void
LogStackWalk( PDEBUGPACKET dp )
{
    STACKWALK         stk;
    PSYMBOL           psym;
    DWORD             dwDisplacement = 0;
    DWORD             frames = 0;
    char              *szSymName;


    lprintf( MSG_STACKTRACE );

    stk.frame = dp->tctx->frame;
    stk.pc = dp->tctx->pc;
    stk.ul = 0;

    if (!StackWalkInit(&stk, dp)) {
        lprintf( MSG_STACKTRACE_FAIL );
        return;
    }

    lprintf( MSG_STACKTRACE_HEADER );

    do {
        psym = GetSymFromAddrAllContexts( stk.pc, &dwDisplacement, dp );
        if (psym) {
            szSymName = UnDName( &psym->szName[1] );
        }
        else {
            szSymName = "<nosymbols>";
        }
        lprintfs( "%08x %08x %08x %08x %08x %08x ",
                  stk.pc,
                  stk.frame,
                  stk.params[0],
                  stk.params[1],
                  stk.params[2],
                  stk.params[3]
                );

        lprintfs( "%s\r\n", szSymName );
    } while (StackWalkNext(&stk, dp) && frames++ < 100);

    lprintfs( "\r\n" );

    return;
}

void
LogDisassembly( PDEBUGPACKET dp, PCRASHES pCrash )
{
    PSYMBOL           psym;
    DWORD             dwFuncAddr;
    DWORD             dwFuncSize;
    DWORD             dwDisplacement = 0;
    char              *szSymName;
    DWORD             offset;
    int               i;
    int               j;
    char              dbuf[1024];
    BOOL              fFaultingInst;
    int               dwStartDis;
    int               dwEndDis;


    psym = GetSymFromAddr( dp->tctx->pc, &dwDisplacement, dp->tctx->mi );

    if (psym) {
        dwFuncAddr = psym->addr;
        dwFuncSize = psym->size;
        szSymName = UnDName( &psym->szName[1] );
    }
    else {
        dwFuncAddr = dp->tctx->pc - 50;
        dwFuncSize = 100;
        szSymName = "<nosymbols>";
    }

    if (dp->tctx->fFaultingContext) {
        strcpy( pCrash->szFunction, szSymName );
    }

    lprintf( MSG_FUNCTION, szSymName );

tryagain:
    //
    // count the number of instructions in the function
    // also, save the instruction number of context's pc
    //
    for (i=0,offset=dwFuncAddr,j=-1; offset<dwFuncAddr+dwFuncSize; i++) {
        if (offset == dp->tctx->pc) {
            j = i;
        }
        if (!disasm( dp, &offset, dbuf, TRUE )) {
            break;
        }
    }

    if (j == -1) {
        //
        // we didn't find a match for the current pc
        // this because we don't have symbols for the current pc and
        // therefore had to just backup and start disassembling.  we try
        // to recover by adding 1 to the func addr and do it again.
        // eventually we will hit the pc and we will be a-ok.
        //
        dwFuncAddr++;
        goto tryagain;
    }

    //
    // print the disassemled instructions.  only print the number
    // of instructions before and after the current pc that the
    // user specified in the registry options.
    //
    dwStartDis = max(0,j - (int)dp->options.dwInstructions);
    dwEndDis = j+(int)dp->options.dwInstructions;
    fFaultingInst = FALSE;
    for (i=0,offset=dwFuncAddr; offset<dwFuncAddr+dwFuncSize; i++) {
        if (offset == dp->tctx->pc) {
            fFaultingInst = TRUE;
        }
        if (!disasm( dp, &offset, dbuf, TRUE )) {
            break;
        }
        if (i >= dwStartDis) {
            if (fFaultingInst && dp->tctx->fFaultingContext) {
                fFaultingInst = FALSE;
                lprintf( MSG_FAULT );
            }
            else {
                lprintfs( "        " );
            }
            lprintfs( "%s\r\n", dbuf );
        }
        if (i > dwEndDis) {
            break;
        }
    }

    lprintfs( "\r\n" );
    return;
}

void
AttachToActiveProcess ( PDEBUGPACKET dp )
{
    HANDLE              Token;
    PTOKEN_PRIVILEGES   NewPrivileges;
    BYTE                OldPriv[1024];
    PBYTE               pbOldPriv;
    ULONG               cbNeeded;
    BOOLEAN             fRc;
    LUID                LuidPrivilege;

    //
    // Make sure we have access to adjust and to get the old token privileges
    //
    if (!OpenProcessToken( GetCurrentProcess(),
                           TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                           &Token)) {
        if (dp->options.fVisual) {
            FatalError( LoadRcString(IDS_DEBUGPRIV) );
        }
        else {
            ExitProcess( 1 );
        }
    }

    cbNeeded = 0;

    //
    // Initialize the privilege adjustment structure
    //

    LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &LuidPrivilege );

    NewPrivileges =
        (PTOKEN_PRIVILEGES)LocalAlloc(LMEM_ZEROINIT,
                                      sizeof(TOKEN_PRIVILEGES) +
                                          (1 - ANYSIZE_ARRAY) *
                                          sizeof(LUID_AND_ATTRIBUTES));
    if (NewPrivileges == NULL) {
        if (dp->options.fVisual) {
            FatalError( LoadRcString(IDS_DEBUGPRIV) );
        }
        else {
            ExitProcess( 1 );
        }
    }

    NewPrivileges->PrivilegeCount = 1;
    NewPrivileges->Privileges[0].Luid = LuidPrivilege;
    NewPrivileges->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    //
    // Enable the privilege
    //

    pbOldPriv = OldPriv;
    fRc = AdjustTokenPrivileges( Token,
                                 FALSE,
                                 NewPrivileges,
                                 1024,
                                 (PTOKEN_PRIVILEGES)pbOldPriv,
                                 &cbNeeded );

    if (!fRc) {

        //
        // If the stack was too small to hold the privileges
        // then allocate off the heap
        //
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {

            pbOldPriv = LocalAlloc(LMEM_FIXED, cbNeeded);
            if (pbOldPriv == NULL) {
                if (dp->options.fVisual) {
                    FatalError( LoadRcString(IDS_DEBUGPRIV) );
                }
                else {
                    ExitProcess( 1 );
                }
            }

            fRc = AdjustTokenPrivileges( Token,
                                         FALSE,
                                         NewPrivileges,
                                         cbNeeded,
                                         (PTOKEN_PRIVILEGES)pbOldPriv,
                                         &cbNeeded );
        }
    }


    if (!DebugActiveProcess( dp->dwPidToDebug )) {
        FatalError( LoadRcString(IDS_ATTACHFAIL) );
        if (dp->options.fVisual) {
            FatalError( LoadRcString(IDS_ATTACHFAIL) );
        }
        else {
            ExitProcess( 1 );
        }
    }

    return;
}

void
LogSystemInformation( PDEBUGPACKET dp )
{
    char          buf[1024];
    SYSTEM_INFO   si;
    DWORD         ver;
    SYSINFO       mySi;
    DWORD         dwThreadId;
    HANDLE        hThread;

    lprintf( MSG_SYSINFO_HEADER );
    hThread = CreateThread( NULL,
                            16000,
                            (LPTHREAD_START_ROUTINE)SysInfoThread,
                            &mySi,
                            THREAD_SET_INFORMATION,
                            (LPDWORD)&dwThreadId
                          );
    Sleep( 0 );
    if (WaitForSingleObject( hThread, 30000 ) == WAIT_TIMEOUT) {
        Assert(TerminateThread( hThread, 0 ) == TRUE);
    }
    lprintf( MSG_SYSINFO_COMPUTER, mySi.szMachineName );
    lprintf( MSG_SYSINFO_USER, mySi.szUserName );
    GetSystemInfo( &si );
    wsprintf( buf, "%d", si.dwNumberOfProcessors );
    lprintf( MSG_SYSINFO_NUM_PROC, buf );
    lprintf( MSG_SYSINFO_PROC_TYPE );
    switch(si.dwProcessorType) {
        case PROCESSOR_INTEL_386:
            lprintf( MSG_SYSINFO_I386 );
            break;

        case PROCESSOR_INTEL_486:
            lprintf( MSG_SYSINFO_I486 );
            break;

        case PROCESSOR_INTEL_860:
            lprintf( MSG_SYSINFO_I860 );
            break;

        case PROCESSOR_MIPS_R2000:
            lprintf( MSG_SYSINFO_R2000 );
            break;

        case PROCESSOR_MIPS_R3000:
            lprintf( MSG_SYSINFO_R3000 );
            break;

        case PROCESSOR_MIPS_R4000:
            lprintf( MSG_SYSINFO_R4000 );
            break;

        case PROCESSOR_ALPHA_21064:
            lprintf( MSG_SYSINFO_A21064 );
            break;

        default:
            lprintf( MSG_SYSINFO_UNKNOWN );
            break;
    }
    ver = GetVersion();
    wsprintf( buf, "%d.%d", LOBYTE(LOWORD(ver)), HIBYTE(LOWORD(ver)) );
    lprintf( MSG_SYSINFO_WINVER, buf );
    RegLogCurrentVersion();
    lprintfs( "\r\n" );
}

DWORD
SysInfoThread( PSYSINFO si )
{
    DWORD len;

    strcpy( si->szMachineName, "<unknown machine name>" );
    strcpy( si->szUserName,    "<unknown user name>" );
    len = sizeof(si->szMachineName);
    GetComputerName( si->szMachineName, &len );
    len = sizeof(si->szUserName);
    GetUserName( si->szUserName, &len );

    return 0;
}

DWORD
TerminationThread( PDEBUGPACKET dp )
{
    HANDLE hProcess;

    hProcess = OpenProcess( PROCESS_TERMINATE, FALSE, dp->dwPidToDebug );
    if (hProcess != NULL) {
        TerminateProcess( hProcess, 0 );
        CloseHandle( hProcess );
    }

    return 0;
}

char *
GetExceptionText( DWORD dwExceptionCode )
{
    static char buf[80];
    DWORD dwFormatId = 0;

    memset( buf, 0, sizeof(buf) );

    switch (dwExceptionCode) {
        case STATUS_SINGLE_STEP:
            dwFormatId = MSG_SINGLE_STEP_EXCEPTION;
            break;

        case DBG_CONTROL_C:
            dwFormatId = MSG_CONTROLC_EXCEPTION;
            break;

        case DBG_CONTROL_BREAK:
            dwFormatId = MSG_CONTROL_BRK_EXCEPTION;
            break;

        case STATUS_ACCESS_VIOLATION:
            dwFormatId = MSG_ACCESS_VIOLATION_EXCEPTION;
            break;

        case STATUS_STACK_OVERFLOW:
            dwFormatId = MSG_STACK_OVERFLOW_EXCEPTION;
            break;

        case STATUS_INTEGER_DIVIDE_BY_ZERO:
            dwFormatId = MSG_INTEGER_DIVIDE_BY_ZERO_EXCEPTION;
            break;

        case STATUS_PRIVILEGED_INSTRUCTION:
            dwFormatId = MSG_PRIVILEGED_INSTRUCTION_EXCEPTION;
            break;

        case STATUS_ILLEGAL_INSTRUCTION:
            dwFormatId = MSG_ILLEGAL_INSTRUCTION_EXCEPTION;
            break;

        case STATUS_IN_PAGE_ERROR:
            dwFormatId = MSG_IN_PAGE_IO_EXCEPTION;
            break;

        case STATUS_DATATYPE_MISALIGNMENT:
            dwFormatId = MSG_DATATYPE_EXCEPTION;
            break;

        case STATUS_POSSIBLE_DEADLOCK:
            dwFormatId = MSG_DEADLOCK_EXCEPTION;
            break;

        case STATUS_VDM_EVENT:
            dwFormatId = MSG_VDM_EXCEPTION;
            break;

        case STATUS_BREAKPOINT:
            dwFormatId = MSG_BREAKPOINT_EXCEPTION;
            break;

        default:
            lprintfs( "\r\n" );
            break;
    }

    FormatMessage( FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                   NULL,
                   dwFormatId,
                   0, // GetUserDefaultLangID(),
                   buf,
                   sizeof(buf),
                   NULL
                 );

    return buf;
}
