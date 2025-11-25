/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    endback.c

Abstract:

    High level end write operations.  Flushes buffers and zero-fills
    to the end of a segment.

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
q117EndBack(
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:

    This routine finishes writing all information to the tape for pending
    update.

    Uses global variable CurrentOperation.SegmentBytesRemaining, the number of bytes left in last
    segment on the tape.

Arguments:

    Context -

Return Value:

    Possible return values:  NoErr, any Driver error,
    <EndOfVol>, <LinkRC>, RdncUnscError

--*/

{
    USHORT end_fill,remainder;
    STATUS ret;                 // Return value from other routines called.
    PIO_REQUEST ioreq;

    //
    // clear the end of the last segment buffer
    //

    if (Context->CurrentOperation.SegmentBytesRemaining) {

        //
        // clear the rest of the data
        //

        RtlZeroMemory(Context->CurrentOperation.SegmentPointer,Context->CurrentOperation.SegmentBytesRemaining);
        end_fill = Context->CurrentOperation.SegmentBytesRemaining;

    } else {

        end_fill = 0;

    }

    //
    // Set fill bytes (so bad sector re-mapping knows how much of this segment
    // is zeros)
    //

    Context->CurrentOperation.BytesZeroFilled = end_fill;

    //
    // write out last block
    //

    if (!(ret = q117IssIOReq(
                    (PVOID)NULL,
                    DWrite,
                    (LONG)Context->CurrentOperation.CurrentSegment * BLOCKS_PER_SEGMENT,
                    NULL,
                    Context))) {

        ++Context->CurrentOperation.CurrentSegment;

        //
        // wait for all blocks to be written
        //

        while (!ret && !q117QueueEmpty(Context)) {

            ioreq = q117Dequeue(WaitForItem, Context);

            if (ioreq->Status && ioreq->Status != BadBlk) {

                ret = ioreq->Status;

            } else {

                if (ioreq->Status == BadBlk) {

                    if (!(ret = q117MapBadBlock(
                                    ioreq,
                                    &Context->CurrentOperation.SegmentPointer,
                                    &Context->CurrentOperation.SegmentBytesRemaining,
                                    &Context->CurrentOperation.CurrentSegment,
                                    &remainder,
                                    Context))) {

                        if (remainder) {

                            if (Context->CurrentOperation.SegmentBytesRemaining) {

                                //
                                // clear the rest of the data
                                //

                                RtlZeroMemory(Context->CurrentOperation.SegmentPointer,Context->CurrentOperation.SegmentBytesRemaining);
                                Context->CurrentOperation.BytesZeroFilled = Context->CurrentOperation.SegmentBytesRemaining;
                            }

                            //
                            // write out overflow data
                            //

                            if (!(ret=q117IssIOReq(
                                        (PVOID)NULL,
                                        DWrite,
                                        (LONG)Context->CurrentOperation.CurrentSegment *
                                            BLOCKS_PER_SEGMENT,
                                        NULL,
                                        Context))) {

                                ++Context->CurrentOperation.CurrentSegment;
                            }
                        }
                    }
                }
            }
        }
    }
    return(ret);
}
