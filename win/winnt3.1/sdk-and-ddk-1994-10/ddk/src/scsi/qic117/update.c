/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    update.c

Abstract:

    Performs the various tape updating functions.

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
q117UpdateHeader(
    IN PTAPE_HEADER Header,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    This routine updates the tape header.

Arguments:

    Header -

    Context -

Return Value:



--*/

{
    STATUS ret;
    PVOID scrbuf;
    PSEGMENT_BUFFER bufferInfo;

    //
    // put saved logical part of header into transfer buffer
    //
    scrbuf = q117GetFreeBuffer(&bufferInfo,Context);
    RtlMoveMemory(scrbuf, Header, sizeof(TAPE_HEADER));

    //
    // write out the TapeHeader structure
    //
    ret = q117FillTapeBlocks(
                DWriteBad,
                (SEGMENT)0,
                Header->DupHeaderSegment,
                scrbuf,
                Header->HeaderSegment,
                Header->DupHeaderSegment,
                bufferInfo,
                Context);
    return(ret);
}

STATUS
q117Update(
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:

    This routine updates tape directory with cur_vol.

Arguments:

    Link -

    Context -

Return Value:



--*/
{
    STATUS ret;     // Return value from other routines called.

    Context->ActiveVolume.DataSize = Context->CurrentOperation.BytesOnTape;

    //
    // update volume table entry (to be written to tape directory)
    //
    Context->ActiveVolume.EndingSegment = (USHORT)Context->CurrentOperation.CurrentSegment-1;


    if (Context->CurrentOperation.UpdateBadMap) {
        if (ret = q117DoUpdateBad(Context))
            return(ret);
    }

    //
    // update volume directory
    //
    // thevoldir->endblock was set to 0 at StartBack().
    //
    ret=q117AppVolTD(&Context->ActiveVolume,Context);
    if (ret==NoErr)  {
        Context->CurrentOperation.EndOfUsedTape = Context->ActiveVolume.EndingSegment;
#ifndef NO_MARKS
        ret = q117DoUpdateMarks(Context);
#endif
    } else {
        Context->CurrentOperation.EndOfUsedTape=0;
    }

    //
    // Set the tape status.
    //
    q117SetTpSt(Context);
    return(ret);
}


STATUS
q117DoUpdateBad(
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:



Arguments:

    Context -

Return Value:



--*/
{
    STATUS ret;
    PVOID scrbuf;
    PSEGMENT_BUFFER bufferInfo;
    PTAPE_HEADER hdr;

    //
    //rdr - Beta fix
    //

//    return(BadTape);

    CheckedDump(QIC117INFO,( "Q117i: Starting DoUpdateBad\n"));


    //
    // read the header segment in
    //
    //if (ret = q117ReadHeaderSegment(&hdr,Context)) {
    //
    //    return(ret);
    //
    //}
    hdr = Context->CurrentTape.TapeHeader;

    //
    // put in the new bad sector map
    //

    //RtlMoveMemory(&(hdr->BadMap),
    //    Context->CurrentTape.BadMapPtr,
    //    sizeof(BAD_MAP));

    scrbuf = q117GetFreeBuffer(&bufferInfo,Context);

    //
    // put saved logical part of header into transfer buffer
    //

    RtlMoveMemory(scrbuf, hdr, sizeof(TAPE_HEADER));

    //
    // write out the TapeHeader structure
    //

    if ( ret = q117FillTapeBlocks(
                DWriteBad,
                (SEGMENT)0,
                hdr->DupHeaderSegment,
                scrbuf,
                hdr->HeaderSegment,
                hdr->DupHeaderSegment,
                bufferInfo,
                Context) ) {

        return(UpdErr);

    }

    //
    // tape directory potentialy corrupted by FillTapeBlocks(), so just
    // re-read it
    //
    Context->tapedir = (PIO_REQUEST)NULL;

    CheckedDump(QIC117INFO,( "Q117i: Ending DoUpdateBad (Success)\n"));
    return(NoErr);
}

#ifndef NO_MARKS

STATUS
q117DoUpdateMarks(
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:



Arguments:

    Context -

Return Value:



--*/
{
    STATUS ret;
    PSEGMENT_BUFFER bufferInfo;
    PVOID scrbuf;
    PIO_REQUEST ioreq;


    scrbuf = q117GetFreeBuffer(&bufferInfo,Context);

    //
    // Fill in the mark list
    //
    RtlZeroMemory(scrbuf,
        q117GoodDataBytes(
            (SEGMENT)Context->ActiveVolume.DirectorySize, Context )
        );

    RtlMoveMemory(scrbuf, &Context->MarkArray, sizeof(Context->MarkArray));

    ret=q117IssIOReq(scrbuf,DWrite,
        Context->ActiveVolume.DirectorySize * BLOCKS_PER_SEGMENT,bufferInfo,Context);

    if (!ret) {
        //
        // Wait for data to be written
        //
        ioreq=q117Dequeue(WaitForItem,Context);

        ret = ioreq->Status;

    }

    return(ret);
}

STATUS
q117GetMarks(
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:



Arguments:

    Context -

Return Value:



--*/
{
    STATUS ret;
    PSEGMENT_BUFFER bufferInfo;
    PVOID scrbuf;
    PIO_REQUEST ioreq;


    scrbuf = q117GetFreeBuffer(&bufferInfo,Context);

    //
    // Read this data block into memory
    //
    ret=q117IssIOReq(scrbuf,DRead,
        Context->ActiveVolume.DirectorySize * BLOCKS_PER_SEGMENT,bufferInfo,Context);

    if (!ret) {
        // Wait for data to be read
        ioreq=q117Dequeue(WaitForItem,Context);

        if (ioreq->Status != BadBlk && ioreq->Status) {

            ret = ioreq->Status;

        } else {

            /* correct data segment with Reed-Solomon and Heroic retries */
            ret = q117ReconstructSegment(ioreq,Context);
        }

        if (!ret) {
            RtlMoveMemory(&Context->MarkArray, scrbuf, sizeof(Context->MarkArray));
        }

    }

    return(ret);
}
#endif

STATUS
q117FillTapeBlocks(
    IN OUT DRIVER_COMMAND Command,
    IN SEGMENT CurrentSegment,
    IN SEGMENT EndSegment,
    IN OUT PVOID Buffer,
    IN SEGMENT FirstGood,
    IN SEGMENT SecondGood,
    IN PSEGMENT_BUFFER BufferInfo,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:



Arguments:

    Command -

    CurrentSegment -

    EndSegment -

    Buffer -

    FirstGood -

    SecondGood -

    BufferInfo -

    Context -

Return Value:



--*/
{
    STATUS ret;
    CHAR iocmd;
    PIO_REQUEST ioreq;
    ULONG cur_seg = 0;     // The current segment being processed
    BAD_LIST badList[BLOCKS_PER_SEGMENT];
    ULONG listEntry;
    USHORT listIndex;

    //
    // set queue into single buffer mode
    //
    q117QueueSingle(Context);

    //
    // get pointer to free buffer
    //
    if (Buffer == NULL) {
        Buffer = q117GetFreeBuffer(&BufferInfo,Context);
    }

    do {
        while(!q117QueueFull(Context) && CurrentSegment <= EndSegment) {
            if (Command == DWriteBad && (CurrentSegment == FirstGood || CurrentSegment == SecondGood)) {
                iocmd = DWrite;
            } else {
                iocmd = Command;
            }

            //
            // We need to skip segments with no data area (less than 4
            // good segments)
            //
            while (q117GoodDataBytes(CurrentSegment,Context) <= 0) {
                ++CurrentSegment;
            }
            if (ret=q117IssIOReq(Buffer,iocmd,(LONG)CurrentSegment * BLOCKS_PER_SEGMENT,BufferInfo,Context)) {

                return(ret);
            }
            ++CurrentSegment;
        }

        ioreq = q117Dequeue(WaitForItem,Context);

        if ((ioreq->Status!=NoErr) &&
            (ioreq->Status!=BadBlk) &&
            (ioreq->Status!=BadMark)) {

            //
            // Any Driver error except BadBlk.
            //
            return(ioreq->Status);
        }
        if (Command == DVerify) {

            if (Context->CurrentTape.TapeFormatCode == QIC_FORMAT) {

                Context->CurrentTape.BadMapPtr->BadSectors[(ioreq->Block)/BLOCKS_PER_SEGMENT] =
                    ioreq->BadList|ioreq->RetryList;

            } else {

                if ((ioreq->BadList|ioreq->RetryList) != 0l) {

                    q117BadMapToBadList(
                        (SEGMENT)((ioreq->Block)/BLOCKS_PER_SEGMENT),
                        (ioreq->BadList | ioreq->RetryList),
                        badList);

                    listIndex = 0;

                    do {

                        RtlMoveMemory(
                            Context->CurrentTape.BadMapPtr->BadList[Context->CurrentTape.CurBadListIndex++].ListEntry,
                            badList[listIndex++].ListEntry,
                            (ULONG)LIST_ENTRY_SIZE);

                        listEntry = q117BadListEntryToSector(badList[listIndex].ListEntry);

                    } while (listEntry &&
                            (listIndex < BLOCKS_PER_SEGMENT));

                }

            }

        }

    //
    // Till nothing left in the queue.
    //
    } while (!q117QueueEmpty(Context) || CurrentSegment <= EndSegment);

    q117QueueNormal(Context);
    return(NoErr);
}
