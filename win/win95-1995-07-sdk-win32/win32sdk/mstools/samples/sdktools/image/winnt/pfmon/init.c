/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    init.c

Abstract:

    This is the initialization module for the pfmon program.

Author:

    Mark Lucovsky (markl) 26-Jan-1995

Revision History:

--*/

#include "pfmonp.h"

BOOL
InitializePfmon( VOID )
{
    LPTSTR CommandLine;
    BOOL fShowUsage;

    fShowUsage = FALSE;
    CommandLine = GetCommandLine();
    while (*CommandLine > ' ') {
        CommandLine += 1;
        }
    while (TRUE) {
        while (*CommandLine <= ' ') {
            if (*CommandLine == '\0') {
                break;
                }
            else {
                CommandLine += 1;
                }
            }

        if (!strnicmp( CommandLine, "/v", 2 ) || !strnicmp( CommandLine, "-v", 2 )) {
            CommandLine += 2;
            fVerbose = TRUE;
            }
        else if (!strnicmp( CommandLine, "/?", 2 ) || !strnicmp( CommandLine, "-?", 2 )) {
            CommandLine += 2;
            fShowUsage = TRUE;
            goto showusage;
            }
        else if (!strnicmp( CommandLine, "/c", 2 ) || !strnicmp( CommandLine, "-c", 2 )) {
            CommandLine += 2;
            fCodeOnly = TRUE;
            }
        else if (!strnicmp( CommandLine, "/h", 2 ) || !strnicmp( CommandLine, "-h", 2 )) {
            CommandLine += 2;
            fHardOnly = TRUE;
            }
        else if (!strnicmp( CommandLine, "/n", 2 ) || !strnicmp( CommandLine, "-n", 2 )) {
            CommandLine += 2;
            LogFile = fopen("pfmon.log","wt");
            fLogOnly = TRUE;
            }
        else if (!strnicmp( CommandLine, "/l", 2 ) || !strnicmp( CommandLine, "-l", 2 )) {
            CommandLine += 2;
            LogFile = fopen("pfmon.log","wt");
            }
        else {
            break;
            }
        }
showusage:
    if ( fShowUsage ) {
        fprintf(stdout,"Usage: PFMON [switches] application-command-line\n");
        fprintf(stdout,"             [-?] display this message\n");
        fprintf(stdout,"             [-n] don't display running faults, just log to pfmon.log\n");
        fprintf(stdout,"             [-l] log faults to pfmon.log\n");
        fprintf(stdout,"             [-c] only show code faults\n");
        fprintf(stdout,"             [-h] only show hard faults\n");
        return FALSE;
        };
    if ( !LoadApplicationForDebug( CommandLine ) ) {
        return FALSE;
        }

    return TRUE;
}

BOOL
LoadApplicationForDebug(
    LPSTR CommandLine
    )
{
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInformation;

    ZeroMemory( &StartupInfo, sizeof( StartupInfo ) );
    StartupInfo.cb = sizeof(StartupInfo);

    InitializeListHead( &ProcessListHead );
    InitializeListHead( &ModuleListHead );
    SetSymbolSearchPath();

    PfmonModuleHandle = GetModuleHandle( NULL );

    if (!CreateProcess( NULL,
                        CommandLine,
                        NULL,
                        NULL,
                        FALSE,                          // No handles to inherit
                        DEBUG_PROCESS,
                        NULL,
                        NULL,
                        &StartupInfo,
                        &ProcessInformation
                      )
       ) {
        DeclareError( PFMON_CANT_DEBUG_PROGRAM,
                      GetLastError(),
                      CommandLine
                    );
        return FALSE;
        }
    else {
        hProcess = ProcessInformation.hProcess;
        SymInitialize(hProcess,NULL,FALSE);
        return InitializeProcessForWsWatch(hProcess);
        return TRUE;
        }
}
