/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    gtendblk.c

Abstract:

    Calls SelectTD() once and ReadVolumeEntry() as many times as needed to
    find out what the CurrentOperation.EndOfUsedTape is.  Also calls EndRest() which will
    verify the entire tape directory.


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
q117GetEndBlock (
    OUT PVOLUME_TABLE_ENTRY TheVolumeTable,
    OUT LONG *NumberVolumes,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Calls SelectTD() once and ReadVolumeEntry() as many times as needed to
    find out what the CurrentOperation.EndOfUsedTape is.  Also calls EndRest() which will
    verify the entire tape directory.

Arguments:

    TheVolumeTable - Last volume on the tape

    NumberVolumes - Number of volumes on the tape

    Context - Current context of the driver.

Return Value:

    NoErr,  any Driver error except BadBlk,
    <EndOfVol>, <LinkRC>, RdncUnsc.

--*/

{
    VOLUME_TABLE_ENTRY cur_table;
    STATUS ret;                     // Return value from other routines called.

    *NumberVolumes = 0;

    //
    // This will initialize 'ActiveVolumeNumber' and 'CurrentOperation.EndOfUsedTape'.
    // FMemErr, any Driver error except BadBlk, <EndOfVol>, RdncUnsc.
    //

    ret=q117SelectTD(Context);

    while (ret == NoErr) {

        if (ret = q117ReadVolumeEntry(&cur_table,Context)) {

            //
            // <FMemErr>, any Driver error except BadBlk, <LinkRC>,
            // <EndOfVol>, RdncUnsc.
            //

            if (ret != EndOfVol) {

                return(ret);

            }

        }

        if (ret != EndOfVol) {

            ++(*NumberVolumes);
            *TheVolumeTable = cur_table;

        }
    }

    if (ret && ret != EndOfVol) {

        return(ret);

    }

    ret=q117EndRest(Context);

    return(ret);
}

