/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    erase.c

Abstract:

    Does a read pass over the entire tape to create the bad block map and
    writes the bad block map to the tape and zeros the tape directory.

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
q117VerifyFormat(
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:




Arguments:

    Context -

Return Value:


    NoErr,  UnusTape, any Driver error, <LinkBack>, RdncUnsc.


--*/

{
    STATUS status;

    Context->CurrentTape.CurBadListIndex = 0;

    status = q117FillTapeBlocks(
                DVerify,
                0,
                Context->CurrentTape.LastSegment,
                NULL,
                0,
                0,
                NULL,
                Context);

    Context->CurrentTape.CurBadListIndex = 0;

    return status;
}


STATUS
q117EraseQ (
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Writes the bad sector map already in memory (put there either by a
    call to Init() or Erase()) to the tape and stomps on each entry
    in the tape directory.

Arguments:

    Context -

Return Value:

    Possible return values:  NoErr, FMemErr, UnusTape, any Driver
    error, <LinkBack>, RdncUnsc.

--*/

{
    STATUS ret;       //  Return value from other routines called.
    LONG i;           //  generic loop index
    PVOLUME_TABLE_ENTRY scrbuf;
    PSEGMENT_BUFFER bufferInfo;
    PIO_REQUEST ioreq;

    if (!q117QueueEmpty(Context)) {

        return(FCodeErr);

    }

    q117ClearVolume(Context);

    scrbuf = (PVOLUME_TABLE_ENTRY)q117GetFreeBuffer(&bufferInfo,Context);

    if (ret = q117IssIOReq(
                scrbuf,
                DRead,
                (LONG)Context->CurrentTape.VolumeSegment * BLOCKS_PER_SEGMENT,
                bufferInfo,
                Context)) {

        return(ret);

    }

    ioreq = q117Dequeue(WaitForItem,Context);
    ret = ioreq->Status;

    if (ret == BadBlk || ret == NoErr) {

        //
        // correct data segment with Reed-Solomon and Heroic retries
        //
        ret = q117ReconstructSegment(ioreq,Context);

    }

    if (ret) {

        return ret;

    }

    //
    //  stomp on the signatures only
    //

    for (i = 0;
         i < ( ( DATA_BLOCKS_PER_SEGMENT * BYTES_PER_SECTOR ) /
                sizeof(VOLUME_TABLE_ENTRY));
         i++) {

        RtlZeroMemory(&scrbuf[i].Signature, sizeof(scrbuf->Signature));

    }

    //
    //  now write it out again!
    //

    if (ret = q117IssIOReq(
                scrbuf,
                DWrite,
                (LONG)Context->CurrentTape.VolumeSegment * BLOCKS_PER_SEGMENT,
                bufferInfo,
                Context)) {

        return(ret);

    }

    ret = q117Dequeue(WaitForItem,Context)->Status;

    q117ClearVolume(Context);

    q117SetTpSt(Context);

    return(ret);
}

STATUS
q117EraseS (
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Init() must be called before calling this routine.  Reads the tape
    directory to find the ending block of the last backup on the tape
    and writes 0's to every sector from the end of the tape directory
    up to and including the ending block of the last backup, and then 0's
    the tape directory and rewrites the bad sector map by calling
    EraseQ().

Arguments:

    Context -

Return Value:



--*/

{
    STATUS ret;         // Return value from other routines called.
    PVOID scrbuf;
    PSEGMENT_BUFFER bufferInfo;

    scrbuf = q117GetFreeBuffer(&bufferInfo, Context);

    RtlZeroMemory(scrbuf, BLOCKS_PER_SEGMENT * BYTES_PER_SECTOR);

    ret = q117FillTapeBlocks(
                DWrite,
                Context->CurrentTape.VolumeSegment,
                Context->CurrentTape.LastSegment,
                scrbuf,
                0,
                0,
                bufferInfo,
                Context);

    Context->CurrentOperation.EndOfUsedTape =
        Context->CurrentTape.LastUsedSegment =
            Context->CurrentTape.VolumeSegment;

    q117ClearVolume(Context);

    q117SetTpSt(Context);

    return(ret);
}

VOID
q117ClearVolume (
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Clear state information for any active volume.

Arguments:

    Context -

Return Value:

--*/
{
#ifndef NO_MARKS
    RtlZeroMemory(&Context->MarkArray,sizeof(Context->MarkArray));
    Context->CurrentMark = Context->MarkArray.TotalMarks;
    Context->MarkArray.MarkEntry[Context->CurrentMark].Offset = 0xffffffff;
#endif

    Context->CurrentOperation.Position = TAPE_REWIND;

    //
    // Destroy the active volume
    //

    RtlZeroMemory(&Context->ActiveVolume,sizeof(Context->ActiveVolume));
    Context->ActiveVolumeNumber = 0;

    //
    // Set tape to all available
    //
    Context->CurrentOperation.EndOfUsedTape =
        Context->CurrentTape.LastUsedSegment =
            Context->CurrentTape.VolumeSegment;

}








