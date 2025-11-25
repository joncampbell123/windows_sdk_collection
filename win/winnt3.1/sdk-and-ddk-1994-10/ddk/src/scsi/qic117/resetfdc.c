/*++

Copyright (c) 1993 - Colorado Memory Systems,  Inc.
All Rights Reserved

Module Name:

    resetfdc.c

Abstract:

    Resets the FDC and cleans up the sense status.

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


VOID
Q117iResetFDC(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    To reset the floppy controller chip.

Arguments:

    TapeExtension -

Return Value -

    None.

--*/

{
    UCHAR resetByte;
    SHORT statLength;
    struct sns_int_cmd senseInt;
    FDC_STATUS rStat;
    SHORT resetRetry = 5;

    senseInt.command = FDC_SNS_INT;

    TapeExtension->QControllerData->CommandHasResultPhase = FALSE;
    (VOID) Q117iResetInterruptEvent(TapeExtension);

    if (!TapeExtension->Found) {

        TapeExtension->QControllerData->DriveSelect.Selected = FALSE;

        WRITE_CONTROLLER(
            &TapeExtension->QControllerData->FDC_Addr->dor, alloff);
        Q117iShortTimer(t0010us);
        WRITE_CONTROLLER(
            &TapeExtension->QControllerData->FDC_Addr->dor,dselb);

    } else {

        if (TapeExtension->QControllerData->DriveSelect.Selected == TRUE) {

            resetByte =
                TapeExtension->QControllerData->DriveSelect.SelectByte;

        } else {

            resetByte =
                TapeExtension->QControllerData->DriveSelect.DeselectByte;

        }

        resetByte &= 0xfb;
        WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->dor,
                        (SHORT)resetByte);
        Q117iShortTimer(t0010us);
        resetByte |= 0x0c;
        WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->dor,
                        (SHORT)resetByte);

    }

    if (Q117iSleep(TapeExtension, mt_wt500ms, TRUE) == NoErr) {

        Q117iReadFDC(TapeExtension, (CHAR *)&rStat, (SHORT *)&statLength);
        --resetRetry;

    }

    do {

        if (Q117iProgramFDC(TapeExtension,
                            (CHAR *)&senseInt,
                            sizeof(senseInt),
                            FALSE) != NoErr) {

            TapeExtension->QControllerData->FDC_Pcn = 0;
            return;

        }

        Q117iReadFDC(TapeExtension, (CHAR *)&rStat, (SHORT *)&statLength);
        --resetRetry;
        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

    } while (((rStat.ST0 & ST0_US) < 3) && resetRetry);

    Q117iConfigureFDC(TapeExtension);
    TapeExtension->QControllerData->FDC_Pcn = 0;
    TapeExtension->QControllerData->PerpendicularMode = FALSE;
}

