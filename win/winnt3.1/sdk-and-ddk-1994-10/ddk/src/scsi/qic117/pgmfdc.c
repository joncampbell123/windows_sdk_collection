/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    pgmfdc.c

Abstract:

    FDC program logic.

Revision History:




--*/

//
// include files
//

#include <ntddk.h>                        // various NT definitions
#include <ntdddisk.h>                    // disk device driver I/O control codes
#include <ntiologc.h>
#include "common.h"
#include "drvtask.h"                     // this driver's data declarations
#include "mt1defs.h"                     // this driver's data declarations
#include "mt1strc.h"                     // this driver's data declarations
#include "q117data.h"                    // this driver's data declarations


STATUS
Q117iProgramFDC(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT UCHAR *Command,
    IN SHORT Length,
    IN BOOLEAN Result
    )

/*++

Routine Description:

Send a command to the Floppy Disk Controller.  Commands are of
various lengths as defined by the FDC spec (NEC uPD765A).

For each byte in the command string, program_nec must wait for the
FDC to become ready to read command data.  Program_nec will wait
up to approx. 3 msecs before giving up and declaring an error.

Arguments:

TapeExtension -

Command -

Length -

Result -

Return Value:



--*/

{
    SHORT i;
    SHORT waitCount;

    TapeExtension->QControllerData->CommandHasResultPhase = Result;

    DbgAddEntry(0x12345678);

    for (i = 0; i < Length; i++) {

        waitCount = FDC_MSR_RETRIES;

        do {

            if ((READ_CONTROLLER(
                    &TapeExtension->QControllerData->FDC_Addr->MSDSR.msr) &
                (MSR_RQM | MSR_DIO)) == MSR_RQM) {

                break;

            }

            Q117iShortTimer(t0012us);

        } while (--waitCount > 0);

        if (waitCount == 0) {

            return(NECFlt);

        }

        WRITE_CONTROLLER(
            &TapeExtension->QControllerData->FDC_Addr->dr,
            (SHORT)Command[i]);

        DbgAddEntry(Command[i]);

        Q117iShortTimer(t0012us);
    }

    return(NoErr);
}
