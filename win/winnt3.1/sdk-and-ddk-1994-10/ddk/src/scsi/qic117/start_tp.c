/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    start_tp.c

Abstract:


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
Q117iStartTape(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Start the tape in the logical forward mode if it is not already.

Arguments:

    TapeExtension -

Return Value:



--*/

{

    STATUS retval = NoErr;

    if (!TapeExtension->TapePosition.LogFwd) {

        if((retval = Q117iSendByte(TapeExtension, Logical_Fwd)) == NoErr) {

            TapeExtension->TapePosition.LogFwd = TRUE;

        }

    }

    return(retval);

}

