/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    select.c

Abstract:

    Code to select the tape drive (using one of many different schemes

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
Q117iSelectDrive(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Select the tape drive by making the select line active (low).

Agruments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;

    if (TapeExtension->QControllerData->DriveSelect.Selected == FALSE) {

        WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->dor,TapeExtension->QControllerData->DriveSelect.SelectByte);

        if ((TapeExtension->QControllerData->DriveSelect.SelectByte == selu ||
            TapeExtension->QControllerData->DriveSelect.SelectByte == selub) &&
            TapeExtension->DriveParms.DriveSelect != curb) {

            if (TapeExtension->DriveParms.Flavor == CMS) {

                if ((retval = Q117iSendByte(TapeExtension, Select_Drive)) == NoErr) {

                    Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);
                    retval = Q117iSendByte(TapeExtension, 2);

                }

            }

            if ((retval == NoErr) && (TapeExtension->DriveParms.Flavor == SUMMIT ||
                TapeExtension->DriveParms.Flavor == ARCHIVE ||
                TapeExtension->DriveParms.Flavor == CORE)) {

                retval = Q117iMtnPreamble(TapeExtension, TRUE);

            }

        }
        if (retval == NoErr) {

            TapeExtension->QControllerData->DriveSelect.Selected = TRUE;

        }

        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);
    }

    return(retval);
}


STATUS
Q117iDeselectDrive(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Deselect the tape drive by making the select line inactive (high).

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;

    if (TapeExtension->QControllerData->DriveSelect.Selected == TRUE) {

        if ((TapeExtension->QControllerData->DriveSelect.SelectByte == selu ||
            TapeExtension->QControllerData->DriveSelect.SelectByte == selub) &&
            TapeExtension->DriveParms.DriveSelect != curb) {

            if (TapeExtension->DriveParms.Flavor == CMS ||
                TapeExtension->DriveParms.Flavor == UNKNOWN) {

                retval = Q117iSendByte(TapeExtension, Deselect_Drive);

            }

            if ((retval == NoErr) &&
                (TapeExtension->DriveParms.Flavor == SUMMIT ||
                TapeExtension->DriveParms.Flavor == ARCHIVE ||
                TapeExtension->DriveParms.Flavor == CORE)) {

                retval = Q117iMtnPreamble(TapeExtension, FALSE);

            }

        }

        WRITE_CONTROLLER(
            &TapeExtension->QControllerData->FDC_Addr->dor,
            TapeExtension->QControllerData->DriveSelect.DeselectByte);
        TapeExtension->QControllerData->DriveSelect.Selected = FALSE;
        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);
    }

    return(retval);
}
