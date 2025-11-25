/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    format.c

Abstract:

    Tells the driver to format the tape and calls Erase().

    If DoFormat is TRUE, does format pass and then bad sector mapping; else
    does only the bad sector mapping.


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

#define QIC80_TRACKS_NUM             (VU_80TRACKS_PER_CART * 10)
#define TOTAL_GAS_BLOCKS             20
#define QIC80_TRACKS_DENOM_FACTOR    10

STATUS
q117Format(
    OUT LONG *NumberBad,
    IN UCHAR DoFormat,
    IN PQIC40_VENDOR_UNIQUE VendorUnique,
    IN OUT PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Formats the tape.

Arguments:

    NumberBad - number of bad sectors

    DoFormat - flag of actually do a QIC40 format

    VendorUnique -

    Context -

Return Value:

    NT Status

    NoErr, any Driver error, UnusTape, RdncUnsc, <LinkBack>


--*/

{
    STATUS ret;                     // Return value from other routines called.
    PIO_REQUEST ioreq;
    IO_REQUEST wproIoreq;
    SEGMENT headerSegment[2];
    SEGMENT segment;
    ULONG numberFound;
    PTAPE_HEADER hdr;
    PVOID scrbuf;
    struct S_O_DGetCap tparms;      // tape parameters from the driver
    PSEGMENT_BUFFER bufferInfo;

    //
    // Check for write protected cart,  DSndWPro will
    // return stat = WProt in this case.
    //
    if (ret = q117DoCmd(&wproIoreq, DSndWPro, NULL, Context)) {

        return ret;

    }

    if (ret = q117InitFiler(Context)) {

        return(ret);

    }

    scrbuf = q117GetFreeBuffer(&bufferInfo,Context);

    if (DoFormat)  {

        //
        // Force the tape header,  volume list etc,  to be re-loaded
        //
        Context->CurrentTape.State = NeedInfoLoaded;

        *(UCHAR *)scrbuf=FORMAT_BYTE;

        q117GetTapeCapacity(&tparms,Context);

        if (ret=q117IssIOReq(scrbuf,DFmt,1l,bufferInfo,Context)) {

            //
            // <FMemErr>
            //

            return(ret);

        }

        ioreq = q117Dequeue(WaitForItem,Context);

        if (ioreq->Status) {

            return(ioreq->Status);

        }

        if (ret = q117GetTapeCapacity(NULL,Context)) {

            return(ret);

        }

        //
        // clear the bad block map (erase or's in new bad sectors)
        //

        RtlZeroMemory(
            Context->CurrentTape.BadMapPtr,
            sizeof(BAD_MAP));

        //
        // verify the tape (this will set up the bad sector map)
        //

        if (ret=q117VerifyFormat(Context)) {

            //
            // FMemErr, UnusTape, any Driver error, <LinkBack>, RdncUnsc.
            //

            return(ret);

        }

        //
        // find the first two error free segments for the tape headers
        //

        numberFound = segment = 0;
        while (segment <= Context->CurrentTape.LastSegment && numberFound < 2) {

            if (q117CountBits(Context, segment, 0l) == 0) {

                headerSegment[numberFound++] = segment;

            }
            ++segment;
        }

        if (segment > Context->CurrentTape.LastSegment) {

            return(UnusTape);

        }

        //
        // find the first segment that data can be stored in (more than
        // 3 good sectors).
        //

        while (q117CountBits(Context, segment, 0l) >=
            (BLOCKS_PER_SEGMENT-ECC_BLOCKS_PER_SEGMENT) && segment <= Context->CurrentTape.LastSegment) {

            ++segment;

        }

        Context->CurrentTape.VolumeSegment = segment;

        //
        // count up all bad sectors and find the last segment that data
        // can be stored in.
        //

        q117GetBadSectors(Context);

        *NumberBad = Context->CurrentTape.BadSectors;

        if (Context->CurrentTape.VolumeSegment >= Context->CurrentTape.LastSegment) {

            return(UnusTape);

        }

        //
        // create the tape header
        //

        hdr = (PTAPE_HEADER)scrbuf;

        if (ret = q117BuildHeader(VendorUnique,headerSegment,hdr,Context)) {

            return(ret);

        }

        q117UpdateHeader(hdr,Context);

        //
        // Make memory image of header up to date after format.
        //
        RtlMoveMemory(
            Context->CurrentTape.TapeHeader,
            hdr,
            sizeof(*Context->CurrentTape.TapeHeader) );

        if (!ret) {

            //
            // erase the tape directory
            //

            ret=q117EraseQ(Context);

        }
    }
    return(ret);
}


STATUS
q117BuildHeader(
    OUT PQIC40_VENDOR_UNIQUE VendorUnique,
    IN SEGMENT *HeaderSect,
    IN OUT PTAPE_HEADER Header,
    IN PQ117_CONTEXT Context
    )

/*++

Routine Description:

    Builds the tape header for a format.

Arguments:

    VendorUnique - vendor unique section

    HeaderSect - header segment array

    Header - JUMBO header

Return Value:



--*/

{
    STATUS ret = NoErr;         // Return value from other routines called.
    LONG divisor, dividend;
    struct S_O_DGetCap capacity;

    //
    //      In two of the following calculations, we are changing from the number
    //  of items to the highest possible value for that item. Floppy sectors are
    //  numbered from 1 count, so the count is also the highest possible number.
    //  The rest are numbered 0 through count-1, so the highest possible number
    //  is count-1.
    //      The only fancy calculation is the number of floppy sides. This is
    //  obtained by obtaining the number of logical tape segments (SEG in the
    //  QIC-80 spec), and dividing by the number of segments per track. In addition,
    //  it may be necessary to round up in case of a remainder. To handle the
    //  remainder and the decrement simultaneuously, we decrement only if there
    //  is no remainder.
    //      See the QIC-80 spec, sec 5.3.1 on the identification of sectors, and
    //  7.1, bytes 24 through 29, for the defined values. Wouldn't it be nice
    //  if this stuff were better documented?   -- crc.
    //

    if (!(ret = q117GetTapeCapacity(&capacity,Context))) {

        RtlZeroMemory(
            VendorUnique,
            sizeof(*VendorUnique));

        //
        // Maximum floppy sectors. Warning: #define!
        //

        VendorUnique->correct_name.MaxFlopSect =
            capacity.MaxFSector;

        //
        // Tape segments per tape track
        //

        VendorUnique->correct_name.TrackSeg =
            (USHORT)capacity.SegmentsPerTrack;

        //
        // Tape tracks per cartridge
        //

        VendorUnique->correct_name.CartTracks =
            (UCHAR)capacity.TracksPerTape;

        //
        // Maximum floppy tracks
        //

        VendorUnique->correct_name.MaxFlopTrack =
            capacity.FTrackPerFSide-1;

        //
        // Maximum floppy sides
        //

        dividend = capacity.TracksPerTape * capacity.SegmentsPerTrack;
        divisor = capacity.SegmentsPerFTrack * capacity.FTrackPerFSide;
        VendorUnique->correct_name.MaxFlopSide =
            (UCHAR)(dividend / divisor);

        if( !(dividend % divisor) ) {

            VendorUnique->correct_name.MaxFlopSide--;

        }

    } else {

        return(ret);

    }

    //
    // zero the tape header structure to start with
    //

    RtlZeroMemory(Header,sizeof(TAPE_HEADER));

    //
    // fill in the valid info
    //

    Header->Signature = TapeHeaderSig;                 // set to 0xaa55aa55l
    Header->FormatCode = capacity.TapeFormatCode;      // set to 0x02
    Header->HeaderSegment = (USHORT)HeaderSect[0];     // segment number of header
    Header->DupHeaderSegment = (USHORT)HeaderSect[1];  // segment number of duplicate header
    Header->FirstSegment = (USHORT)Context->CurrentTape.VolumeSegment;  // segment number of Data area
    Header->LastSegment = (USHORT)Context->CurrentTape.LastSegment; // segment number of End of Data area

    //
    // time of most recent format
    //
    Header->CurrentFormat = 0l;    // lbt_qictime()

    //
    // time of most recent write to cartridge
    //
    Header->CurrentUpdate = 0l;    // lbt_qictime()

    //
    // tape name and name change date
    //
    Header->VendorUnique = *VendorUnique;

    Header->ReformatError = 0;     // 0xff if any of remaining data is lost
    Header->SegmentsUsed = 0;      // incremented every time a segment is used
    Header->InitialFormat = 0l;    //lbt_qictime()   time of initial format
    Header->FormatCount = 1;       // number of times tape has been formatted
    Header->FailedSectors = 0;     // the number entries in failed sector log

    //
    //  Set the name of manufacturer that formatted
    //

    strcpy(Header->ManufacturerName,"Microsoft Windows NT (CMS Driver)");
    q117SpacePadString(
        Header->ManufacturerName,
        sizeof(Header->ManufacturerName));

    //
    // set format lot code
    //

    if (Context->drive_type == QIC40_DRIVE) {

        strcpy(Header->LotCode,"User Formatted,  QIC 40 rev G.");

    } else {

        strcpy(Header->LotCode,"User Formatted,  QIC 80 rev D.");

    }

    q117SpacePadString(Header->LotCode,sizeof(Header->LotCode));

    //
    // fill in bad sector map just generated
    //

    RtlMoveMemory(
        &(Header->BadMap),
        Context->CurrentTape.BadMapPtr,
        sizeof(BAD_MAP));

    return(ret);
}
