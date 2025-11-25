/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    config.c

Abstract:

    configures the FDC and drive to the correct speed.

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
Q117iConfigureDrive(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Configure the tape drive with a pre-defined state.

        Put the tape drive into the Primary mode.  This command should work
        regardless of the current state of the drive.

        Set or determine the tape speed depending upon whether or not the
        tape drive is a CMS drive.

        Read the current track or set the current track to 0 also depending
        upon whether or not the tape drive is a CMS drive.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;       // return value
    CHAR newSpeed;             // tape drive transfer rate

    //
    // Make sure that the tape drive is out there.
    //

    retval = Q117iGetDriveError(TapeExtension);

    if (retval && retval != NoTape) {

        return(retval);

    }

    //
    // We will only configure (set the transfer rate) the tape drive if we
    // know that it is ours.  This is because we assume that our drive is
    // the only variable speed drive available.
    //

    if (TapeExtension->SpeedChangeOK) {

    //
    // Send the Select_Speed command to the tape drive.   This ommand is sent
    // in 2 parts (cmd - arg) and we must be sure that the drive is ready to
    // receive the argument before sending it.  The drive is ready either when
    // it starts generating index pusles or after approximately 20 ms.
    //

        if ((retval = Q117iSendByte(TapeExtension, Select_Speed)) != NoErr) {

            return(retval);

        }

        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

        if (TapeExtension->XferRate.XferRate == FAST) {

            newSpeed = TapeExtension->XferRate.TapeFast + 2;

        } else {

            newSpeed = TapeExtension->XferRate.TapeSlow + 2;

        }

        if ((retval = Q117iSendByte(TapeExtension, newSpeed)) != NoErr) {

            return(retval);

        }

        //
        // Wait for the drive to become ready again.  Specified time is 10
        // secs.
        //

        retval = Q117iWaitCommandComplete(TapeExtension, mt_wt010s);

        if (retval && retval != NoTape) {

            return(retval);

        }

        //
        // Set the current track indicator to an unknow position.  This will
        // force a track change command on the first read write to the tape.
        //

        TapeExtension->TapePosition.C_Track = -1;
    }

    return(NoErr);
}


STATUS
Q117iConfigureFDC(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    To configure the floppy controller chip according to the current
    FDC parameters.

Arguments:

    TapeExtension -

Return Value:


--*/

{
    STATUS retval;
    CHAR fdcType;                  // flag to indicate an 82077 FDC
    struct specify_cmd setup_N;
    struct config_cmd configure;

    if (TapeExtension->XferRate.XferRate == FAST) {

        Q117iDCR_Out(TapeExtension, TapeExtension->XferRate.FDC_Fast);

    } else {

        Q117iDCR_Out(TapeExtension, TapeExtension->XferRate.FDC_Slow);

    }


    // Determine if the FDC is an 82077

    if ((retval = Q117iDGetFDC(TapeExtension, (PUCHAR)&fdcType)) != NoErr) {

        return(retval);

    }

    if (fdcType == FDC_82077 ||
        fdcType == FDC_82077AA ||
        fdcType == FDC_82078_44 ||
        fdcType == FDC_82078_64 ||
        fdcType == FDC_NATIONAL) {

        WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->tdr,
                        curu);

        if (TapeExtension->XferRate.XferRate == FAST) {

            WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->MSDSR.dsr,
                            TapeExtension->XferRate.FDC_Fast);

            if (TapeExtension->XferRate.FDC_Fast == FDC_500Kbps) {

                WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->tdr,
                                curb);

            }

        } else {

            WRITE_CONTROLLER(
                &TapeExtension->QControllerData->FDC_Addr->MSDSR.dsr,
                TapeExtension->XferRate.FDC_Slow);

            if (TapeExtension->XferRate.FDC_Slow == FDC_250Kbps) {

                WRITE_CONTROLLER(
                    &TapeExtension->QControllerData->FDC_Addr->tdr,
                    curb);
            }

            if (TapeExtension->XferRate.FDC_Slow == FDC_500Kbps) {

                WRITE_CONTROLLER(
                    &TapeExtension->QControllerData->FDC_Addr->tdr,
                    curb);

            }
        }

        configure.cmd = FDC_CONFIG;
        configure.czero = 0;
        configure.FIFOTHR = FDC_FIFO;
        configure.POLL = 0;
        configure.EFIFO = 0;
        configure.EIS = 0;
        configure.reserved = 0;
        configure.PRETRK = 0x00;

        if (retval = Q117iProgramFDC(TapeExtension,
                                    (CHAR *)&configure,
                                    sizeof(configure),
                                    FALSE)) {

            return(retval);

        }
    }

    setup_N.command = FDC_SPECIFY;
    setup_N.SRT_HUT = TapeExtension->XferRate.XferRate ?
                    TapeExtension->XferRate.SRT_Fast :
                    TapeExtension->XferRate.SRT_Slow;
    setup_N.HLT_ND = 0x02;
    retval = Q117iProgramFDC(TapeExtension,
                            (CHAR *)&setup_N,
                            sizeof(setup_N),
                            FALSE);

    return(retval);
}
