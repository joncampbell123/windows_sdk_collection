/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    pause.c

Abstract:

    stops the tape and rewinds 3 blocks.

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
Q117iPauseTape(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Stop the tape by issuing a Pause command to the tape drive. The Pause
    command will both stop the tape and rewind it back a few blocks.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;

    if ((retval = Q117iSendByte(TapeExtension, Pause)) == NoErr) {

        if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt016s)) == NoErr) {
            
            TapeExtension->TapePosition.LogFwd = FALSE;

        }
    }

    if (retval == TimeOut) {
        
        if((retval = Q117iSendByte(TapeExtension, Pause)) == NoErr) {

            if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt016s)) == NoErr) {
                
                TapeExtension->TapePosition.LogFwd = FALSE;

            }
        }

    }

    Q117iDLockUnlockDMA(TapeExtension, FALSE);


    return(retval);
}
