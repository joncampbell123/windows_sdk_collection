/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    pfmon.c

Abstract:

    USAGE: pfmon [pfmon switches] command-line-of-application


Platform:

   PFMON will run only on Windows NT.
    It requires psapi.dll which is distributed with the Win32 SDK.

Author:

    Mark Lucovsky (markl) 26-Jan-1995

--*/

#include "pfmonp.h"

#define WORKING_SET_BUFFER_ENTRYS 4096
PSAPI_WS_WATCH_INFORMATION WorkingSetBuffer[WORKING_SET_BUFFER_ENTRYS];

int
main ()
{
    CHAR Line[256];

    if (!InitializePfmon()) {
        ExitProcess( 1 );
        }
    else {
        DebugEventLoop();
        sprintf(Line,"\n PFMON: Total Faults %d  (Soft %d, Hard %d, Code %d, Data %d)\n",
            TotalSoftFaults + TotalHardFaults,
            TotalSoftFaults,
            TotalHardFaults,
            TotalCodeFaults,
            TotalDataFaults
            );
        fprintf(stdout,"%s",Line);
        if ( LogFile ) {
            fprintf(LogFile,"%s",Line);
            fclose(LogFile);
            }
        ExitProcess( 0 );
        }

    return 0;
}

VOID
ProcessPfMonData(
    VOID
    )
{
    BOOL b;
    BOOL DidOne;
    INT i;
    PMODULE_INFO PcModule;
    PMODULE_INFO VaModule;
    PIMAGEHLP_SYMBOL PcSymbol;
    PIMAGEHLP_SYMBOL VaSymbol;
    DWORD PcOffset;
    DWORD VaOffset;
    CHAR PcLine[256];
    CHAR VaLine[256];
    LPVOID Pc;
    LPVOID Va;
    BOOL SoftFault;
    BOOL CodeFault;
    BOOL KillLog;

    b = GetWsChanges(hProcess,WorkingSetBuffer,sizeof(WorkingSetBuffer));

    if ( b ) {
        DidOne = FALSE;
        i = 0;
        while (WorkingSetBuffer[i].FaultingPc) {
            if ( WorkingSetBuffer[i].FaultingVa ) {
                Pc = WorkingSetBuffer[i].FaultingPc;
                Va = WorkingSetBuffer[i].FaultingVa;

                if ( (ULONG)Va & 1 ) {
                    TotalSoftFaults++;
                    SoftFault = TRUE;
                    }
                else {
                    TotalHardFaults++;
                    SoftFault = FALSE;
                    }
                Va = (LPVOID)( (ULONG)Va & 0xfffffffe);
                if ( (LPVOID)((ULONG)Pc & 0xfffffffe) == Va ) {
                    CodeFault = TRUE;
                    TotalCodeFaults++;
                    }
                else {
                    TotalDataFaults++;
                    CodeFault = FALSE;
                    }


                PcModule = FindModuleContainingAddress(Pc);
                VaModule = FindModuleContainingAddress(Va);

                if ( PcModule ) {
                    PcModule->NumberCausedFaults++;
                    }

                if ( VaModule ) {
                    if ( SoftFault ) {
                        VaModule->NumberFaultedSoftVas++;
                        }
                    else {
                        VaModule->NumberFaultedHardVas++;
                        }
                    }

                PcSymbol = SymGetSymFromAddr(hProcess,(DWORD)Pc, &PcOffset);
                VaSymbol = SymGetSymFromAddr(hProcess,(DWORD)Va, &VaOffset);

                if ( PcSymbol ) {
                    if ( PcOffset ) {
                        sprintf(PcLine,"%s+0x%x : ",&PcSymbol->szName[1],PcOffset);
                        }
                    else {
                        sprintf(PcLine,"%s : ",&PcSymbol->szName[1]);
                        }
                    }
                else {
                    sprintf(PcLine,"0x%08x : ",Pc);
                    }

                if ( VaSymbol ) {
                    if ( VaOffset ) {
                        sprintf(VaLine,"%s+0x%x",&VaSymbol->szName[1],VaOffset);
                        }
                    else {
                        sprintf(VaLine,"%s",&VaSymbol->szName[1]);
                        }
                    }
                else {
                    sprintf(VaLine,"0x%08x",Va);
                    }

                KillLog = FALSE;
                if ( fCodeOnly && !CodeFault ) {
                    KillLog = TRUE;
                    }
                if ( fHardOnly && SoftFault ) {
                    KillLog = TRUE;
                    }

                if ( !KillLog ) {
                    if ( !fLogOnly ) {
                        fprintf(stdout,"%s%s%s\n",SoftFault ? "SOFT: " : "HARD: ",PcLine,VaLine);
                        }
                    if ( LogFile ) {
                        fprintf(LogFile,"%s%s%s\n",SoftFault ? "SOFT: " : "HARD: ",PcLine,VaLine);
                        }
                    DidOne = TRUE;
                    }
                }
            i++;
            }
        if ( DidOne ) {
            if ( !fLogOnly ) {
                fprintf(stdout,"\n");
                }
            }

        }
}
