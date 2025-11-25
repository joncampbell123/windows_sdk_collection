/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    gettcap.c

Abstract:

    Gets the capacity of the tape drive (by querying the low-level
    driver).

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
q117GetTapeCapacity(
    struct S_O_DGetCap *ptr,
    PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Get the media's capacity from q117i driver

Arguments:

    Ptr -

    Context -


Return Value:


--*/

{
    struct S_O_DGetCap capacity;
    IO_REQUEST req;
    LONG ret;

    ret = q117DoCmd((PVOID)&req, DGetCap, (PVOID)&capacity, Context);
    if (!ret) {

        if (ptr) {

            *ptr = capacity;

        } else {

            Context->CurrentTape.LastSegment = capacity.FormattableSegments - 1;
            Context->CurrentTape.TapeFormatCode = capacity.TapeFormatCode;

            //
            // If more than what can fit in the bad sector map,  then
            // we got some real problems.
            //
            if ((Context->CurrentTape.LastSegment > MAX_BAD_BLOCKS) &&
                (Context->CurrentTape.TapeFormatCode == QIC_FORMAT)) {

                ret = FCodeErr;

            }

            Context->drive_type = capacity.drive_type;
        }
    }
    return(ret);
}
