/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    getstat.c

Abstract:

    Gets the status of the tape drive (low-level function)

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
Q117iGetStatus(
    IN PTAPE_EXTENSION TapeExtension,
    OUT UCHAR *StatusRegister3
    )

/*++

Routine Description:

    Get status byte from NEC floppy controller chip.

        Send the Sense Drive Status command to the floppy controller

        Read the response from the floppy controller which should be satus
        register 3.

Arguments:

    TapeExtension -

    StatusRegister3 -

Return Value:

--*/

{
    STATUS retval;
    SHORT statLength;
    struct sns_stat_cmd sendSt;
    FDC_STATUS status;

    sendSt.command = 0x04;
    sendSt.drive = (UCHAR)TapeExtension->DriveParms.DriveSelect;

    if((retval = Q117iProgramFDC(TapeExtension,
                                (CHAR *)&sendSt,
                                sizeof(sendSt),
                                FALSE)) != NoErr) {

        return(retval);

    }

    if((retval = Q117iReadFDC(TapeExtension,
                            (CHAR *)&status,
                            (SHORT *)&statLength)) != NoErr) {

        return(retval);

    }

    if(statLength != 1) {

        return(NECFlt);

    }

    *StatusRegister3 = status.ST0;
    return(retval);
}

