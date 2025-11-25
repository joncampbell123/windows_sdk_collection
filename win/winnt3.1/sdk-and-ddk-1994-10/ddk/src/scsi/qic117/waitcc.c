/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    waitcc.c

Abstract:

    Waits for the drive to become ready (command complete)


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
Q117iWaitCommandComplete(
    IN PTAPE_EXTENSION TapeExtension,
    IN QIC_TIME WaitTime
    )

/*++

Routine Description:

    Wait a specified amount of time for the tape drive to become ready after
    executing a command.

        Read the Drive Status byte from the tape drive.

        If the drive is not ready then wait 1/2 second and try again until
        the specified time has elapsed.

Arguments:

    TapeExtension -

    WaitTime -

Return Value:


--*/
{
    STATUS retval;

    do {

        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

        if ((retval = Q117iGetDriveError(TapeExtension)) != NotRdy) {

            return(retval);

        }

        WaitTime -= mt_wt200ms;

    } while (WaitTime > 0l);

    retval = TimeOut;

    if (TapeExtension->DriveParms.Status.NewCart) {

        retval = NewCart;

    }

    return(retval);
}

