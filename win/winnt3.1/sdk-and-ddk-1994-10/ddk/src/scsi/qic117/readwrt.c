/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    readwrt.c

Abstract:

    Performs low-level read and write operations (with position logic).

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
Q117iReadWrite(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST IoRequestCurrent,
    IN OUT PIRP Irp
    )

/*++


Routine Description:

    Process a data read or write.

    The first Wrong Cylinder error is ignored incase of a bad seek. On the
    system 50, 60, 80 the first N Over Run errors are ignored due to the VGA
    16-bit access bug (see PS/2 compatability manual).

Arguments:

    TapeExtension -

    IoRequestCurrent -

    Irp -

Return Value:



--*/
{
    STATUS retval = NoErr;
    UCHAR i;
    BOOLEAN dmaDir;
    STATUS status = NoErr;
    STATUS sleep_ret = NoErr;
    RDV_COMMAND         rdWrCmd;
    PIO_STACK_LOCATION irpSp;
    struct PerpMode perpMode;

    perpMode.command = PERP_MODE_COMMAND;
    perpMode.drive_select = TapeExtension->QControllerData->PerpModeSelect;
    perpMode.reserved = 0;
    perpMode.over_write = TRUE;

    irpSp = IoGetCurrentIrpStackLocation( Irp );

    Q117iGetRetryCounts(TapeExtension, IoRequestCurrent->Command);

    IoRequestCurrent->RetryList = 0l;
    TapeExtension->RdWrOp.BytesTransferredSoFar = 0l;
    TapeExtension->RdWrOp.TotalBytesOfTransfer = 0l;
    TapeExtension->RdWrOp.RetryCount = TapeExtension->RdWrOp.RetryTimes;
    TapeExtension->RdWrOp.RetrySectorId = 0;
    TapeExtension->RdWrOp.SeekFlag = TRUE;

    if ((retval = Q117iCalcPosition(
                        TapeExtension,
                        IoRequestCurrent->Block,
                        IoRequestCurrent->Number)) != NoErr) {

        return(retval);

    }

    TapeExtension->RdWrOp.CurLst = IoRequestCurrent->BadList;

    TapeExtension->RdWrOp.Scount = 0;

    for (i = 0; i < IoRequestCurrent->Number; i++) {

        if ((TapeExtension->RdWrOp.CurLst & 1) == 0) {

            TapeExtension->RdWrOp.Scount++;

        }

        TapeExtension->RdWrOp.CurLst = TapeExtension->RdWrOp.CurLst >> 1;

    }

    TapeExtension->RdWrOp.CurLst = IoRequestCurrent->BadList;
    TapeExtension->RdWrOp.DataAmount = 0;

    while (TapeExtension->RdWrOp.Scount != 0 ||
        TapeExtension->RdWrOp.DataAmount != 0) {

        if (TapeExtension->RdWrOp.DataAmount == 0) {

            Q117iNextGoodSectors(TapeExtension);

        }

        if ((TapeExtension->TapePosition.D_Track !=
                TapeExtension->TapePosition.C_Track) ||
            (TapeExtension->TapePosition.C_Segment >
                TapeExtension->TapePosition.D_Segment) ||
            ((TapeExtension->TapePosition.C_Segment !=
                TapeExtension->TapePosition.D_Segment) &&
            ((TapeExtension->TapePosition.LogFwd != TRUE) ||
            ((TapeExtension->TapePosition.D_Segment - 1) !=
                TapeExtension->TapePosition.C_Segment))))

            if ((retval = Q117iSeek(TapeExtension)) != NoErr) {

                return(retval);

            }

        if (IoRequestCurrent->Command == DWrite ||
            IoRequestCurrent->Command == DWriteBad) {

            dmaDir = DMA_READ;

            if ((TapeExtension->DriveParms.DriveType == QIC500_DRIVE) &&
                !TapeExtension->QControllerData->PerpendicularMode) {

                // Enable Perpendicular Mode
                perpMode.wgate = 1;
                perpMode.gap = 1;

                if ((retval = Q117iProgramFDC(
                                TapeExtension,
                                (CHAR *)&perpMode,
                                sizeof(perpMode),
                                FALSE)) != NoErr) {

                    Q117iResetFDC(TapeExtension);
                    Q117iPauseTape(TapeExtension);
                    return(retval);
                }

                TapeExtension->QControllerData->PerpendicularMode = TRUE;
            }

        } else {

            dmaDir = DMA_WRITE;

            if (TapeExtension->QControllerData->PerpendicularMode) {

                // Disable Perpendicular Mode
                perpMode.wgate = 0;
                perpMode.gap = 0;

                if ((retval = Q117iProgramFDC(
                                TapeExtension,
                                (CHAR *)&perpMode,
                                sizeof(perpMode),
                                FALSE)) != NoErr) {

                    Q117iResetFDC(TapeExtension);
                    Q117iPauseTape(TapeExtension);
                    return(retval);
                }

                TapeExtension->QControllerData->PerpendicularMode = FALSE;
            }

        }

        TapeExtension->RdWrOp.TotalBytesOfTransfer =
            (ULONG)(TapeExtension->RdWrOp.DataAmount * PHY_SIZ);

        hio_ProgramDMA(TapeExtension, Irp, dmaDir);

        switch (IoRequestCurrent->Command) {

        case DWrite:
            rdWrCmd.command = WRITE;
            break;

        case DWriteBad:
            rdWrCmd.command = WRTDEL;
            break;

        default:
            rdWrCmd.command = READ;

        }

        rdWrCmd.drive = (UCHAR)TapeExtension->DriveParms.DriveSelect;
        rdWrCmd.C = TapeExtension->RdWrOp.D_FTK;
        rdWrCmd.H = TapeExtension->RdWrOp.D_Head;
        rdWrCmd.R = TapeExtension->RdWrOp.D_Sect;
        rdWrCmd.N = WRT_BPS;
        rdWrCmd.EOT = (UCHAR)TapeExtension->TapeParms.FsectFtrack;
        rdWrCmd.GPL = (UCHAR)TapeExtension->TapeParms.RwGapLength;
        rdWrCmd.DTL = 0xff;

        if ((retval = Q117iStartTape(TapeExtension)) != NoErr) {

            hio_FlushDMA(TapeExtension, Irp, dmaDir);
            return(retval);

        }

        (VOID) Q117iResetInterruptEvent(TapeExtension);
        if ((retval = Q117iProgramFDC(
                            TapeExtension,
                            (CHAR *)&rdWrCmd,
                            sizeof(RDV_COMMAND),
                            TRUE)) != NoErr) {

            hio_FlushDMA(TapeExtension, Irp, dmaDir);
            Q117iResetFDC(TapeExtension);
            Q117iPauseTape(TapeExtension);
            return(retval);
        }

        sleep_ret = Q117iSleep(TapeExtension, mt_wttrks, TRUE);
        hio_FlushDMA(TapeExtension, Irp, dmaDir);

        switch (sleep_ret) {

        case TimeOut:
            if ((retval = Q117iRW_Timeout(
                                TapeExtension,
                                IoRequestCurrent,
                                &status)) != NoErr) {

                    return(retval);

            }

            if (TapeExtension->RdWrOp.NoDat == 0) {

                    return(status);

            }

            break;

        case NoErr:

            if ((retval = Q117iRW_Normal(
                                TapeExtension,
                                IoRequestCurrent,
                                &status)) != NoErr) {

                    return(retval);

            }

            if (status == BadMark || TapeExtension->RdWrOp.NoDat == 0) {

                    return(status);

            }

            break;

        }
    } /* end of while loop */

    if (IoRequestCurrent->Command == DRetry) {

        if (TapeExtension->TapePosition.LogFwd == TRUE) {

            retval = Q117iGetDriveError(TapeExtension);

            if (retval && (retval != NotRdy)) {

                return(retval);

            }

            Q117iPauseTape(TapeExtension);
        }

        if ((retval = Q117iSetBack(TapeExtension, IoRequestCurrent->Command)) !=
            NoErr) {

            return(retval);

        }
    }


#if DBG
    if ((QIC117DebugLevel & QIC117MAKEBAD) && status == NoErr &&
        IoRequestCurrent->Command == DWrite) {

        LARGE_INTEGER time;


        // Use the system time as a "Randomizer" to fake bad sectors
        KeQuerySystemTime(&time);
        time.LowPart /= 10;
        if (time.LowPart & 1) {
            status = BadBlk;

            IoRequestCurrent->BadList |= 1 << (time.LowPart & 31);
            if (time.LowPart & 0x100) {
                IoRequestCurrent->BadList |= 1 << ((time.LowPart>>6) & 31);
            }
            if ((time.LowPart & 0xa00) == 0xa00) {
                IoRequestCurrent->BadList |= 1 << ((time.LowPart>>10) & 31);
            }
            CheckedDump(QIC117MAKEBAD, ("badblk generated\n"));
        } else {
            CheckedDump(QIC117MAKEBAD, ("no sim\n"));
        }
    }
#endif

    return(status);
}


STATUS
Q117iCalcPosition(
    IN PTAPE_EXTENSION TapeExtension,
    IN ULONG Block,
    IN UCHAR Number
    )

/*++

Routine Description:

    Calculate the desired tape position from the Logical Sector Number
    in the I/O Request.

Arguments:

    TapeExtension -

    Block -

    Number -

Return Value:



--*/

{
    ULONG logSect;

    //
    // First check if the desired sector is a legal one, i.e. is less
    // than the maximum number of sectors on the tape.
    //

    CheckedDump(QIC117STOP,( "q117i: requested block %lx\n", Block ));

    if (Block >= TapeExtension->TapeParms.LogSectors) {

        return(BadReq);

    }

    //
    // Now we need to determine the sector ID information so that we
    // can properly program the FDC.  The ID information required is the
    // head, cylinder, and sector numbers.  The head number is calculated
    // as logSect / (floppy sectors per floppy side).  This calculation
    // is done in the while loop which also determines the logical floppy
    // sector on the floppy side.  The cylinder number (d_FTK) and the
    // sector number (d_sect) are calculated last as (logical floppy sector)
    //  / (floppy sectors per floppy cylinder).
    //

    logSect = Block;
    TapeExtension->RdWrOp.D_Head = 0;

    while (logSect >= TapeExtension->TapeParms.FsectFside) {

        logSect -= TapeExtension->TapeParms.FsectFside;
        TapeExtension->RdWrOp.D_Head++;

    }

    TapeExtension->RdWrOp.D_FTK = (UCHAR)(logSect / FSC_FTK);

    //
    // fast logSect % 128
    //

    TapeExtension->RdWrOp.D_Sect = (UCHAR)(logSect % FSC_FTK) + 1;
    TapeExtension->RdWrOp.S_Sect = (UCHAR)TapeExtension->RdWrOp.D_Sect;

    //
    // Next, we need the tape positioning data.  This is the data that
    // we need to find out where, physically, on the tape we need to be.
    // The tape track is determined first as
    // (logical sector number) / (floppy sectors per tape track).  The
    // remainder from this calculation is the absolute sector number
    // on the tape track.  Lastly, the physical segment is determined by
    // dividing the physical sector number by the number of sectors per
    // segment.
    //

    logSect = Block;
    TapeExtension->TapePosition.D_Track = 0;

    while (logSect >= TapeExtension->TapeParms.FsectTtrack) {

        logSect -= TapeExtension->TapeParms.FsectTtrack;
        TapeExtension->TapePosition.D_Track++;

    }

    logSect = logSect / FSC_SEG;
    TapeExtension->TapePosition.D_Segment = (SHORT)logSect;

    //
    // Finally, if the IO Request requests a read that will cross a segment
    // boundary then an error is returned.
    //

    if((((TapeExtension->RdWrOp.D_Sect - 1) & 0x1f) + Number) >
    TapeExtension->TapeParms.FsectSeg) {

        return(BadReq);

    }

    return(NoErr);
}


VOID
Q117iGetRetryCounts(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    )

/*++

Routine Description:

    Determine the retry count information according to the drive command
    eiher read, write, verify, or retry.

Arguments:

    TapeExtension -

    Command -

Return Value:

    None

--*/

{
    switch (Command) {

    case DWrite:
    case DWriteBad:
        TapeExtension->RdWrOp.RetryTimes = WTIMES;
        TapeExtension->RdWrOp.NoDat = 3;
        break;

    case DRead:
    case DReadBad:
        TapeExtension->RdWrOp.RetryTimes = ANTIMES;
        TapeExtension->RdWrOp.NoDat = 3;
        break;

    case DVerify:
        TapeExtension->RdWrOp.RetryTimes = VTIMES;
        TapeExtension->RdWrOp.NoDat = 2;
        break;

    case DRetry:
        TapeExtension->RdWrOp.RetryTimes = ARTIMES;
        TapeExtension->RdWrOp.NoDat = ARTIMES;
        TapeExtension->RetrySeqNum = 0;

    }
}


void
Q117iNextGoodSectors(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Determine the next block of good sectors to read/write/verify.

Arguments:

    TapeExtension -

Return Value:

    None

--*/

{
    TapeExtension->RdWrOp.DataAmount = 0;

    while (TapeExtension->RdWrOp.CurLst & 1) {

        TapeExtension->RdWrOp.CurLst = TapeExtension->RdWrOp.CurLst >> 1;
        TapeExtension->RdWrOp.D_Sect++;

    }

    do {

        TapeExtension->RdWrOp.DataAmount++;
        TapeExtension->RdWrOp.Scount--;
        TapeExtension->RdWrOp.CurLst = TapeExtension->RdWrOp.CurLst >> 1;

    } while (!(TapeExtension->RdWrOp.CurLst & 1) &&
            TapeExtension->RdWrOp.Scount);
}


STATUS
Q117iRW_Timeout(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST IoRequestCurrent,
    OUT STATUS *Status
    )

/*++

Routine Description:

    Process a TIMEOUT error while reading/writing/verifying. If the FDC
    does not report any status within the amount of time that the tape
    would pass apporximately 4 segments, it must be assumed that there is no
    data on the tape.

Arguments:

    TapeExtension -

    IoRequestCurrent -

    Status -

Return Value:



--*/

{
    STATUS retval;

    Q117iResetFDC(TapeExtension);

    if ((retval = Q117iStopTape(TapeExtension)) != NoErr) {

        return(retval);

    }

    if ((retval = Q117iChangeTrack(
                        TapeExtension,
                        TapeExtension->TapePosition.D_Track)) != NoErr) {

        return(retval);

    }

    if (TapeExtension->DriveParms.Status.BOT ||
        TapeExtension->DriveParms.Status.EOT) {

        TapeExtension->TapePosition.C_Segment =
            TapeExtension->TapeParms.SegTtrack;

    }

    if (--TapeExtension->RdWrOp.NoDat == 0) {

        if ((retval = Q117iSetBack(TapeExtension, IoRequestCurrent->Command))
            != NoErr) {

            return(retval);

        }

        *Status = BadBlk;
        IoRequestCurrent->BadList = -
            1l << (TapeExtension->RdWrOp.D_Sect -
                TapeExtension->RdWrOp.S_Sect);

    }
    return(retval);
}


STATUS
Q117iRW_Normal(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST IoRequestCurrent,
    OUT STATUS *Status
    )

/*++


Routine Description:

    Process a read/write/verify operation that has returned normally from
    the FDC.

Arguments:

    TapeExtension -

    IoRequestCurrent -

    Status -

Return Value:



--*/

{
    STATUS retval;
    SHORT statLength;

    if ((retval = Q117iReadFDC(
                    TapeExtension,
                    (CHAR *)&TapeExtension->QControllerData->FdcStat,
                    &statLength)) != NoErr) {

        Q117iPauseTape(TapeExtension);
        return(retval);

    }

    if (statLength != 7) {

        Q117iResetFDC(TapeExtension);
        Q117iGetDriveError(TapeExtension);
        Q117iPauseTape(TapeExtension);

        if (TapeExtension->RdWrOp.NoDat == 0) {

            if ((retval = Q117iSetBack(TapeExtension, IoRequestCurrent->Command))
                != NoErr) {

                return(retval);

            }

            *Status = BadBlk;
            IoRequestCurrent->BadList =
                -1l << (TapeExtension->RdWrOp.D_Sect -
                        TapeExtension->RdWrOp.S_Sect);
        }
        return(NoErr);
    }

    if (TapeExtension->QControllerData->FdcStat.ST1 == 0 &&
        TapeExtension->QControllerData->FdcStat.ST2 == 0) {

        //
        // no errors
        //

        DbgAddEntry(TapeExtension->RdWrOp.D_Sect);
        DbgAddEntry(TapeExtension->RdWrOp.DataAmount);
        DbgAddEntry(TapeExtension->RdWrOp.CurLst);
        DbgAddEntry(TapeExtension->RdWrOp.BytesTransferredSoFar);

        TapeExtension->RdWrOp.SeekFlag = TRUE;
        TapeExtension->TapePosition.C_Segment =
            TapeExtension->TapePosition.D_Segment;
        TapeExtension->RdWrOp.D_Sect +=
            (UCHAR)TapeExtension->RdWrOp.DataAmount;
        TapeExtension->RdWrOp.BytesTransferredSoFar +=
            (ULONG)(TapeExtension->RdWrOp.DataAmount*PHY_SIZ);
        TapeExtension->RdWrOp.DataAmount = 0;
        return(NoErr);
    }

    if ((IoRequestCurrent->Command == DReadBad) &&
        (TapeExtension->QControllerData->FdcStat.ST2 & ST2_CM)) {

        IoRequestCurrent->BadList = 0xffffffff;
        *Status = BadMark;
        return(NoErr);

    }

    if ((TapeExtension->QControllerData->FdcStat.ST2 & ST2_WC) ||
        (TapeExtension->QControllerData->FdcStat.ST1 &
            (ST1_OR | ST1_ND | ST1_MA))) {

        if (TapeExtension->RdWrOp.SeekFlag == TRUE) {

            TapeExtension->TapePosition.C_Segment =
                TapeExtension->TapePosition.D_Segment + 1;
            TapeExtension->RdWrOp.SeekFlag = FALSE;
            return(NoErr);

        }
    }

    if ((IoRequestCurrent->Command == DVerify) &&
        (TapeExtension->QControllerData->FdcStat.ST1 & ST1_MA)) {

        if (TapeExtension->RdWrOp.SeekFlag == TRUE) {

            retval = Q117iGetDriveError(TapeExtension);

            if (retval && (retval != NotRdy)) {

                return(retval);

            }

            if ((retval = Q117iPauseTape(TapeExtension)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iReadIDRepeat(TapeExtension)) != NoErr) {

                return(retval);

            }

            TapeExtension->RdWrOp.SeekFlag = FALSE;
            return(NoErr);
        }
    }

    TapeExtension->TapePosition.C_Segment =
        TapeExtension->TapePosition.D_Segment;

    retval = Q117iRetryCode(
                TapeExtension,
                IoRequestCurrent,
                &TapeExtension->QControllerData->FdcStat,
                Status);

    return(retval);
}


STATUS
Q117iRetryCode(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST IoRequestCurrent,
    OUT FDC_STATUS *FdcStatus,
    OUT STATUS *Status
    )

/*++

Routine Description:

    Orchestrate retries for read/write/verify (and retyr) commands.

Arguments:

    TapeExtension -

    IoRequestCurrent -

    FdcStatus -

    Status -

Return Value:



--*/
{
    STATUS retval;
    SHORT sectorsRead;

    if (TapeExtension->RdWrOp.RetryTimes != 0) {

        if (IoRequestCurrent->Command != DRetry) {

            retval = Q117iGetDriveError(TapeExtension);

            if (retval && (retval != NotRdy)) {

                return(retval);

            }

            if ((retval = Q117iPauseTape(TapeExtension)) != NoErr) {

                return(retval);

            }

        }

        if ((retval = Q117iReadIDRepeat(TapeExtension)) != NoErr) {

            return(retval);

        }

    } else {

        TapeExtension->RdWrOp.SeekFlag = TRUE;

    }

    if ((TapeExtension->RdWrOp.RetryTimes == 0) ||
        (FdcStatus->R == TapeExtension->RdWrOp.RetrySectorId)) {

        if ((TapeExtension->RdWrOp.RetryTimes == 0) ||
            (--TapeExtension->RdWrOp.RetryCount == 0)) {

            TapeExtension->RdWrOp.SeekFlag = TRUE;
            IoRequestCurrent->BadList |=
                1l << (FdcStatus->R - TapeExtension->RdWrOp.S_Sect);
            *Status = BadBlk;
            sectorsRead = FdcStatus->R + 1 - TapeExtension->RdWrOp.D_Sect;
            TapeExtension->RdWrOp.D_Sect = FdcStatus->R + 1;
            TapeExtension->RdWrOp.DataAmount -= sectorsRead;
            TapeExtension->RdWrOp.BytesTransferredSoFar +=
                sectorsRead * PHY_SIZ;
            TapeExtension->RdWrOp.RetryCount =
                TapeExtension->RdWrOp.RetryTimes;
            TapeExtension->RdWrOp.RetrySectorId = 0;
            retval = Q117iSetBack(TapeExtension, IoRequestCurrent->Command);

        } else {

            retval = Q117iNextTry(TapeExtension, IoRequestCurrent->Command);

        }

    } else {

        IoRequestCurrent->RetryList |=
            1l << (FdcStatus->R - TapeExtension->RdWrOp.S_Sect);
        TapeExtension->RdWrOp.RetrySectorId = FdcStatus->R;
        TapeExtension->RdWrOp.RetryCount =
            TapeExtension->RdWrOp.RetryTimes;
        sectorsRead = FdcStatus->R - TapeExtension->RdWrOp.D_Sect;
        TapeExtension->RdWrOp.D_Sect = FdcStatus->R;
        TapeExtension->RdWrOp.DataAmount -= sectorsRead;
        TapeExtension->RdWrOp.BytesTransferredSoFar +=
            sectorsRead * PHY_SIZ;

    }

    return(retval);
}



STATUS
Q117iNextTry(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    )

/*++

Routine Description:

    Determines the next tape drive head position during off-track retries.

Arguments:

    TapeExtension -

    Command -

Return Value:



--*/

{
    STATUS retval = NoErr;

    if (Command == DRetry) {

        retval = Q117iGetDriveError(TapeExtension);

        if (retval && (retval != NotRdy)) {

            return(retval);

        }

        TapeExtension->RetrySeqNum++;

        if (TapeExtension->RdWrOp.RetryCount > 3) {

            retval = Q117iSendByte(TapeExtension, Pause);

        } else {

            retval = Q117iSendByte(TapeExtension, Micro_Pause);

        }

        if (retval) {

            return(retval);

        }

        retval = Q117iWaitCommandComplete(TapeExtension, mt_wt016s);
        TapeExtension->TapePosition.LogFwd = FALSE;

    }

    return(retval);
}


STATUS
Q117iSetBack(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    )

/*++

Routine Description:

    Reset any tape drive head offset due to off-track retries.

Arguments:

    TapeExtension -

    Command -

Return Value:



--*/
{
    STATUS retval = NoErr;

    if (Command == DRetry) {

        if (TapeExtension->TapePosition.LogFwd == TRUE) {

            retval = Q117iGetDriveError(TapeExtension);

            if (retval && (retval != NotRdy)) {

                return(retval);

            }

            if ((retval = Q117iPauseTape(TapeExtension)) != NoErr) {

                return(retval);

            }

        }

        if ((retval = Q117iSendByte(TapeExtension, Seek_Track)) != NoErr) {

            return(retval);

        }

        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

        if ((retval = Q117iSendByte(
                        TapeExtension,
                        (CHAR)(TapeExtension->TapePosition.C_Track + 2))) !=
            NoErr) {

            return(retval);

        }

        retval = Q117iWaitCommandComplete(TapeExtension, mt_wt007s);
    }

    return(retval);
}
