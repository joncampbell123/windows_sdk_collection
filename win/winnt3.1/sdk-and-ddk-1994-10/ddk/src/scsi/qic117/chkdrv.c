/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    chkdrv.c

Abstract:

    Sends a low-level command to search for the drive,  and perform
    configuration.

Revision History:




--*/

//
// include files
//

#include <ntddk.h>
#include <ntddtape.h>
#include "common.h"
#include "q117.h"
#include "protos.h"


STATUS
q117CheckDrive(
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Checks the drive.

Arguments:

    Context - what to check drive for.

Return Value:

    function value - ioreq.status or TooNoisy

--*/

{
    IO_REQUEST ioreq;
    STATUS stat;

    //
    // Find the drive
    //

    stat = q117DoCmd(&ioreq, DFndDrv, NULL, Context);

    if (stat == NewCart) {

        stat = NoErr;

    }

    return(stat);
}
