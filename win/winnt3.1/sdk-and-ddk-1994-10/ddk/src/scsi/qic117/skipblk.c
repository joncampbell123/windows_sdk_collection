/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    skipblk.c

Abstract:

    Performs a forward skip of x bytes.  Reverse seeks are not handled by this routine.

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
q117SkipBlock (
    IN OUT ULONG *HowMany,
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Skip forward X number of data bytes

Arguments:

    HowMany - points to count of bytes to skip,  returns with actual amount
                skipped.

Return Value:

    Status - NoErr, any Driver error except BadBlk, LinkRC,
    EndOfVol, RdncUnsc.

--*/

{
    STATUS ret;              // Return value from other routine called.
    SEGMENT cur_seg;
    SEGMENT skippedSegments;
    ULONG bytesLeft;

    if (*HowMany >= Context->CurrentOperation.BytesOnTape) {

                  //
                  // Flag end of tape.  This will allow for an append.
                  //
        Context->CurrentOperation.Position = TAPE_SPACE_END_OF_DATA;

         }

    if (*HowMany > Context->CurrentOperation.BytesOnTape) {

        return(EndOfVol);
    }


    ret = NoErr;

    bytesLeft = *HowMany;

    //
    // count number of segments to skip
    //
    skippedSegments = 0;
    cur_seg = Context->CurrentOperation.LastSegmentRead;

    while (bytesLeft >= (ULONG)Context->CurrentOperation.SegmentBytesRemaining
        && !ret) {

        bytesLeft -= Context->CurrentOperation.SegmentBytesRemaining;
        Context->CurrentOperation.BytesOnTape -=
            Context->CurrentOperation.SegmentBytesRemaining;
        Context->CurrentOperation.BytesRead +=
            Context->CurrentOperation.SegmentBytesRemaining;
        ++skippedSegments;

        if (++cur_seg > Context->CurrentOperation.LastSegment) {

            q117ClearQueue(Context);
            ret = EndTapeErr;
            skippedSegments = 0;
            cur_seg = Context->CurrentOperation.LastSegmentRead;

        } else {

            Context->CurrentOperation.SegmentBytesRemaining =
                q117GoodDataBytes(cur_seg,Context);

        }
    }

    if ((ULONG)skippedSegments > Context->SegmentBuffersAvailable) {

        //
        // We skipped outside the range of buffers that we
        // save queued up to the lower level driver,  so
        // Clear the lower level driver's requests,  and
        // start reading the segment we skipped to.
        //
        q117ClearQueue(Context);
        Context->CurrentOperation.SegmentBytesRemaining = 0;
        Context->CurrentOperation.CurrentSegment =
            Context->CurrentOperation.LastSegmentRead + skippedSegments;

    } else {

        //
        // The request has already been made to read the segment
        // that we skipped to,  so just de-queue all requests in
        // front of that request and throw away the data.
        //
        cur_seg = Context->CurrentOperation.LastSegmentRead;

        while (skippedSegments && !ret) {

            --skippedSegments;

            if (Context->CurrentOperation.SegmentBytesRemaining =
                q117GoodDataBytes(++cur_seg,Context)) {

                ret = q117NewTrkRC(Context);

                //
                // Ignore Error correction failures on segments that
                // we are skipping.
                //
                if (ret == RdncUnsc) {
                    ret = NoErr;
                }
            }

        }
    }

    if (!ret) {

#ifndef NO_MARKS
        //
        // Force the q117ReadTape code to not hit any file marks
        //
        Context->CurrentMark = Context->MarkArray.TotalMarks;
#endif
        //
        // Now,  skip the number of bytes within the segment we skipped to
        //
        ret = q117ReadTape((PVOID)NULL,&bytesLeft,Context);

        if ( ret == RdncUnsc ) {
            ret = NoErr;
        }

    }


#ifndef NO_MARKS

    //
    // Find our current position in the mark array.
    //
    Context->CurrentMark = 0;
    while (Context->CurrentOperation.BytesRead >
        Context->MarkArray.MarkEntry[Context->CurrentMark].Offset
        ) {

        ++Context->CurrentMark;

    }
#endif

    *HowMany = 0;

    return(ret);
}
