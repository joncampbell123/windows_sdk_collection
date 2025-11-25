/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    do_stop.c

Abstract:

    stops the tape motion (low level function).

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
Q117iStopTape(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Stop the tape if it is moving.

Arguments:

    TapeExtension -

Return Value:



--*/

{
STATUS retval;

    //
    // This first call to GetDriveError must be done to clear any errors that can
    // occur due to ESD.  Specifically, the select line may go away and if we
    // do not reset it the Stop_Tape command will be ignored.
    //

    retval = Q117iGetDriveError(TapeExtension);

    if (retval == NoErr || retval == NotRdy) {

        if ((retval = Q117iSendByte(TapeExtension, Stop_Tape)) == NoErr) {

            if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt005s)) == NoErr) {
                
                TapeExtension->TapePosition.LogFwd = FALSE;

            }
        }

    }

    if (retval == TimeOut) {
        
        retval = Q117iSendByte(TapeExtension, Stop_Tape);

        if ((retval = Q117iSendByte(TapeExtension, Stop_Tape)) == NoErr) {

            if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt005s)) == NoErr) {
                
                TapeExtension->TapePosition.LogFwd = FALSE;

            }
        }
    }

    Q117iDLockUnlockDMA(TapeExtension, FALSE);

    return(retval);
}
