/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    rcnsttrk.c

Abstract:

    This is the tape class driver.

Revision History:




--*/

//
// Includes
//

#include <ntddk.h>
#include <ntddtape.h>
#include "common.h"
#include "q117.h"
#include "protos.h"


STATUS
q117ReconstructSegment(
    IN PIO_REQUEST IoReq,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Reconstructs a segment of data by performing error correction
    and heroic retries.

Arguments:

Return Value:

    Any driver error
    RdncUnsc - If error correction failed
    NoErr - If correction complete

--*/

{
    IO_REQUEST req;
    ULONG badbits,mapbits;
    STATUS ret;

    mapbits = q117ReadBadSectorList(Context, (SEGMENT)(IoReq->Block/BLOCKS_PER_SEGMENT));
    badbits = IoReq->BadList;

    if ( ret = q117DoCorrect(IoReq->Data,mapbits,badbits) ) {

        CheckedDump(QIC117DBGP,("Track Reconstruction required\n"));
        CheckedDump(QIC117DBGP,("map: %08x bad:%08x\n",mapbits,badbits));

        //
        // if error correction failed then we must re-read the data
        //   because it has been corrupted by the Reed-Solomon routines
        //

        req.Data = IoReq->Data;
        req.Block = IoReq->Block;
        req.BadList = mapbits;
        req.Command = DRetry;
        req.Number = BLOCKS_PER_SEGMENT;

        ret = q117DoIO((PIO_REQUEST)&req, IoReq->BufferInfo,Context);

        if (ret) {

            return (ret);

        }

        if (req.Status != BadBlk && req.Status) {

            return(req.Status);

        }

        badbits = req.BadList;

        ret = q117DoCorrect(IoReq->Data, mapbits, badbits);
    }
    return(ret);
}

STATUS
q117DoCorrect(
    IN PVOID DataBuffer,
    IN ULONG BadSectorMap,
    IN ULONG SectorsInError
    )

/*++

Routine Description:

    does the error correction for a segment (using Reed-Solomon
    module)

Arguments:

Return Value:

    RdncUnsc - If error correction failed
    NoErr - If correction complete

--*/

{
    LONG i,j;
    UCHAR offset,num_map,num_bad;
    UCHAR s[ECC_BLOCKS_PER_SEGMENT];
    ULONG bit_i;

    //
    // Turn off bits for any correspondingly mapped out sectors.
    //
    SectorsInError &= ~BadSectorMap;

    num_map = q117CountBits(NULL, 0, BadSectorMap);
    num_bad = q117CountBits(NULL, 0, SectorsInError);

    if (num_bad > ECC_BLOCKS_PER_SEGMENT) {

        CheckedDump(QIC117DBGP,("DoCorrect: Too many bad sectors\n"));

        return(RdncUnsc);

    }

    if (num_bad == 0) {

        if (q117RdsReadCheck(DataBuffer,(UCHAR)(BLOCKS_PER_SEGMENT-num_map)) == NoErr) {

            return(NoErr);

        }

        CheckedDump(QIC117DBGP,("CRC failure detected\n"));


    }

    j = offset = 0;
    bit_i = 1;

    //
    // get offset into buffer (note: offset excludes bad sectors)
    //
    for (i = 0; i < BLOCKS_PER_SEGMENT; ++i) {

        if (!(BadSectorMap & bit_i)) {

            if (SectorsInError & bit_i) {

                s[j++] = offset;

            }

            ++offset;

        }

        //
        // shift bit left one (same as mult by two or add to self)
        //
        bit_i += bit_i;
    }

    CheckedDump(QIC117INFO,("Correct( s0: %x s1: %x s2: %x) ... ",s[0],s[1],s[2]));

    if ( q117RdsCorrect(DataBuffer,(UCHAR)(BLOCKS_PER_SEGMENT-num_map),
            num_bad,s[0],s[1],s[2]) == NoErr ) {

        CheckedDump(QIC117INFO,("OK"));

        return(NoErr);

    } else {

        CheckedDump(QIC117INFO,("failed"));

        return(RdncUnsc);

    }
}

UCHAR
q117CountBits(
    IN PQ117_CONTEXT Context,
    IN SEGMENT Segment,
    ULONG Map
    )

/*++

Routine Description:

    Counts the number of bad sectors for the segment set in "Segment" argument

Arguments:


Return Value:

    Number of bits set

--*/

{
    USHORT i;
    UCHAR numBits;
    ULONG tmp;
    ULONG allBits;


    numBits = 0;

    if (Context == NULL) {

        allBits = Map;

    } else {

       allBits = q117ReadBadSectorList(Context, Segment);

    }

    //
    // Optimization (no bits set)
    //

    if (allBits != 0) {

        tmp = 1;

        //
        // Loop through checking all the bits
        //
        for (i = 0; i < BLOCKS_PER_SEGMENT; ++i) {

            if ( allBits & tmp ) {
                ++numBits;
            }


            //
            // shift left one (tmp *= 2 optimized)
            //
            tmp += tmp;

        }

    }

    return numBits;
}

ULONG q117ReadBadSectorList (
    IN PQ117_CONTEXT Context,
    IN SEGMENT Segment
    )

/*++

Routine Description:


Arguments:

    Context - Context of the driver

    Segment -


Return Value:



--*/

{
    ULONG  listEntry=0l;
    USHORT  listIndex = 0;
    ULONG  startSector;
    ULONG  endSector;
    ULONG  badSectorMap = 0l;
    ULONG  mapFlag;

    if (Context->CurrentTape.TapeFormatCode == QIC_FORMAT) {

        badSectorMap = Context->CurrentTape.BadMapPtr->BadSectors[Segment];

    } else {

        listIndex = Context->CurrentTape.CurBadListIndex;

        listEntry = 
            q117BadListEntryToSector(
                Context->CurrentTape.BadMapPtr->BadList[listIndex].ListEntry
            );

        //
        // if there is no bad sector list.
        // 
        if ((listIndex == 0) && (listEntry == 0l)) {

            badSectorMap = 0l;

        } else {

            //
            // get the start and end sectors for the segment we are looking
            // for.
            //
            startSector = (Segment * BLOCKS_PER_SEGMENT) + 1;
            endSector = (startSector + BLOCKS_PER_SEGMENT) - 1;


            //
            // position to the start of this list
            //

            //
            // if we are ahead of the entry we are looking for
            // scoot back.
            //
            while (listEntry > startSector && listIndex > 0) {

                --listIndex;

                listEntry = 
                    q117BadListEntryToSector(
                        Context->CurrentTape.BadMapPtr->BadList[listIndex].ListEntry
                    );

            }

            //
            // Now look forward for sectors within the range.
            //
            while (listEntry &&
                    (listEntry <= endSector) &&
                    (listIndex < MAX_BAD_LIST)) {

                if (listEntry >= startSector && listEntry <= endSector) {

                    mapFlag = 1l << (listEntry - startSector);
                    badSectorMap |= mapFlag;

                }

                listEntry = q117BadListEntryToSector(Context->CurrentTape.BadMapPtr->BadList[listIndex++].ListEntry);

            }

            //
            // If we walked off the end of the list (listEntry = 0),
            // put us back on.
            //
            if (listEntry == 0) {
                listIndex--;
            }

            Context->CurrentTape.CurBadListIndex = listIndex;
        }
    }

    return (badSectorMap);
}

USHORT
q117GoodDataBytes(
    IN SEGMENT Segment,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Calculates the number of bytes (excluding bad sectors and
    error correction sectors) within a givin segment.

Arguments:

    Segment - segment to look up in Context->CurrentTape.BadSectorMap

Return Value:

    Number of bytes.

--*/

{
    int val;

    val = BLOCKS_PER_SEGMENT - ECC_BLOCKS_PER_SEGMENT -
        q117CountBits(Context, Segment, 0l);

    //
    // Value could be negitave
    //
    if ( val <= 0 ) {

        return(0);

    }

    return val * BYTES_PER_SECTOR;
}
