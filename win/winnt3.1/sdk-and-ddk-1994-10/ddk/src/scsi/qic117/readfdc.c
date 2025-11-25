/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    readfdc.c

Abstract:

    Read FDC result bytes.

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
Q117iReadFDC(
    IN PTAPE_EXTENSION TapeExtension,
    OUT UCHAR *Status,
    OUT SHORT *Length
    )

/*++

Routine Description:

    Read result data from the Floppy Disk Controller.  The result data
    is read during the result phase of the FDC command sequence.
n
        For each byte of response data, wait up to 3 msecs for the FDC to
        become ready.

        Read result data until the FDC is no longer sending data or until
        more than 7 result bytes have been read.  Seven is the maximum
        legal number of result bytes that the FDC is specified to send.

Arguments:

    TapeExtension -

    Status -

    Length -

Return Value:



--*/

{

    SHORT mainStatusRegister;
    SHORT waitCount;

    *Length = 0;

    DbgAddEntry(0x12345679);

    if (TapeExtension->QControllerData->CommandHasResultPhase) {

        *Status++ = TapeExtension->QControllerData->FifoByte;
        DbgAddEntry(TapeExtension->QControllerData->FifoByte);
        ++*Length;

    }


    do {

        waitCount = FDC_MSR_RETRIES;

        do {

            if ((mainStatusRegister =
                READ_CONTROLLER(
                    &TapeExtension->QControllerData->FDC_Addr->MSDSR.msr )) &
                MSR_RQM) {

                break;

            }

            Q117iShortTimer(t0012us);

        } while (--waitCount > 0);

        if (waitCount == 0) {

            return(NECFlt);

        }

        if (!(mainStatusRegister & MSR_DIO)) {

            break;

        }

        *Status = (CHAR)READ_CONTROLLER(
                            &TapeExtension->QControllerData->FDC_Addr->dr);


        DbgAddEntry(*Status);

        ++Status;

        if (++*Length > 7) {
            *Length = -1;
            break;

        }

        Q117iShortTimer(t0012us);

    } while (TRUE);

    return(NoErr);
}

