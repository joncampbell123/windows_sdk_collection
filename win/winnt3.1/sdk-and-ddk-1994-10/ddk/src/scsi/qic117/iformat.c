/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    iformat.c

Abstract:

    Performs low level format of tape.

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
Q117iDFmt(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp,
    OUT PIO_REQUEST IoRequestCurrent
    )

/*++

Routine Description:



Arguments:

    TapeExtension -

    Irp -

    IoRequestCurrent -

Return Value:



--*/

{
    STATUS retval;   // return value
    SHORT trk = 0;  // current track counter
    UCHAR speed;    // holds change speed request
    struct PerpMode perpMode;

    perpMode.command = PERP_MODE_COMMAND;
    perpMode.drive_select = TapeExtension->QControllerData->PerpModeSelect;
    perpMode.reserved = 0;
    perpMode.over_write = TRUE;

    IoRequestCurrent->Data = MmGetSystemAddressForMdl(Irp->MdlAddress);

    //
    // Set up NumTracks depending on drive type.
    //

    switch (TapeExtension->DriveParms.DriveType) {

    case QIC40_DRIVE:

        TapeExtension->NumTracks = NUM_TTRK_40;
        break;

    case QIC80_DRIVE:

        TapeExtension->NumTracks = NUM_TTRK_80;
        break;

    case QIC500_DRIVE:

        TapeExtension->NumTracks = NUM_TTRK_500;
        break;
    }

    //
    // Reset the FDC to make sure it is not in perpendicular Mode.
    //

    Q117iResetFDC(TapeExtension);

    //
    // Make sure that the tape drive is stopped and ready to start the format
    // operation.
    //

    if ((retval = Q117iStopTape(TapeExtension)) == NoErr) {

        //
        // Retension the tape before each format since this is not now
        // regularly done when a tape is inserted in the drive.
        //

        if (retval == NoErr && TapeExtension->DriveParms.Version >=
            FIRM_VERSION_60) {

            if ((retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE)) ==
                NoErr); {

                if ((retval = Q117iDReten(TapeExtension)) == NoErr); {

                    retval = Q117iSetDriveMode(TapeExtension, FORMAT_MODE);

                }
            }
        }

        if (retval == NoErr) {

            if ((retval = Q117iWriteReferenceBurst(TapeExtension)) == NoErr) {

                //
                // This is for the case where a QIC40 tape is fomatted in
                // a QIC80 drive.  The speed is set to FAST if the FDC
                // supports the faster transfer rate.
                //

                if (TapeExtension->XferRate.XferRate == SLOW &&
                    TapeExtension->XferRate.MaxRate == FAST) {

                    speed = DFast;
                    retval = Q117iDFast_DSlow(TapeExtension, speed);

                }

                //
                // Find out what the new tape format will be.  This is
                // necessary in case a QIC-40 tape is being formatted in
                // a QIC-80 drive.
                //

                if (retval == NoErr) {

                    if ((retval = Q117iSetDriveMode(
                                    TapeExtension,
                                    PRIMARY_MODE)) == NoErr); {

                        if ((retval = Q117iGetTapeParameters(TapeExtension)) == NoErr); {

                            retval = Q117iSetDriveMode(TapeExtension, FORMAT_MODE);

                        }

                    }

                    if (retval == NoErr) {

                        //
                        // Now set up the FDC format command data.  This is
                        // done now since it only needs to be done once and
                        // we don't want to use up any more time between
                        // segments than we have to.
                        //

                        TapeExtension->QControllerData->FmtCmd.command = 0x4d;
                        TapeExtension->QControllerData->FmtCmd.N = FMT_BPS;
                        TapeExtension->QControllerData->FmtCmd.SC = FSC_SEG;
                        TapeExtension->QControllerData->FmtCmd.GPL = FMT_GPL;
                        TapeExtension->QControllerData->FmtCmd.drive =
                            (UCHAR)TapeExtension->DriveParms.DriveSelect;
                        TapeExtension->QControllerData->FmtCmd.D =
                            *(PUCHAR)(IoRequestCurrent->Data);

                        CheckedDump(QIC117INFO,( "Q117i: Format pattern %x\n",
                            TapeExtension->QControllerData->FmtCmd.D));

                        if (retval == NoErr) {

                            do {

                                // Enable Perpendicular Mode
                                if ((TapeExtension->DriveParms.DriveType == QIC500_DRIVE) &&
                                    !TapeExtension->QControllerData->PerpendicularMode) {

                                    perpMode.wgate = 1;
                                    perpMode.gap = 1;

                                    if ((retval = Q117iProgramFDC(
                                                    TapeExtension,
                                                    (CHAR *)&perpMode,
                                                    sizeof(perpMode),
                                                    FALSE)) != NoErr) {

                                        Q117iResetFDC(TapeExtension);
                                        Q117iPauseTape(TapeExtension);

                                    } else {

                                        TapeExtension->QControllerData->PerpendicularMode = TRUE;

                                    }
                                }

                                if (retval == NoErr) {

                                    *(PSHORT)IoRequestCurrent->Data = trk;
                                    retval = Q117iFormatTrack(TapeExtension,
                                                                Irp,
                                                                trk,
                                                                IoRequestCurrent);

                                    CheckedDump(QIC117INFO,( "Q117i: Format track return %d (decimal)\n", retval));

                                }

                                if ((retval == BadFmt) ||
                                    (retval == TapeFlt) ||
                                    (retval == TimeOut)) {

                                    if ((retval = Q117iSetDriveMode(
                                                    TapeExtension,
                                                    PRIMARY_MODE)) == NoErr); {

                                        if ((retval = Q117iLogicalBOT(TapeExtension)) == NoErr); {

                                            retval = Q117iSetDriveMode(TapeExtension, FORMAT_MODE);

                                        }

                                    }

                                    if (retval == NoErr) {

                                        // Enable Perpendicular Mode
                                        if ((TapeExtension->DriveParms.DriveType == QIC500_DRIVE) &&
                                            !TapeExtension->QControllerData->PerpendicularMode) {

                                            perpMode.wgate = 1;
                                            perpMode.gap = 1;

                                            if ((retval = Q117iProgramFDC(
                                                            TapeExtension,
                                                            (CHAR *)&perpMode,
                                                            sizeof(perpMode),
                                                            FALSE)) != NoErr) {

                                                Q117iResetFDC(TapeExtension);
                                                Q117iPauseTape(TapeExtension);

                                            } else {

                                                TapeExtension->QControllerData->PerpendicularMode = TRUE;

                                            }
                                        }

                                        if (retval == NoErr) {

                                            *(PSHORT)IoRequestCurrent->Data = trk;
                                            retval = Q117iFormatTrack(TapeExtension,
                                                                        Irp,
                                                                        trk,
                                                                        IoRequestCurrent);

                                            CheckedDump(QIC117INFO,( "Q117i: Format track retry return %d (decimal)\n", retval));
                                        }

                                    }

                                }

                            } while (++trk < (SHORT)TapeExtension->NumTracks && retval == NoErr);

                        }

                        //
                        // Finish up the format by putting the tape drive
                        // back into primary mode, and ejecting the tape.
                        //

                        if (retval == NoErr) {

                            if ((retval = Q117iSetDriveMode(
                                            TapeExtension,
                                            PRIMARY_MODE)) == NoErr) {

                                retval = Q117iDEject(TapeExtension);

                            }

                        }

                    }

                }

            }

        }

    }

    if (TapeExtension->DriveParms.DriveType == QIC500_DRIVE) {

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

    return(retval);
}


STATUS
Q117iFormatTrack(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp,
    IN SHORT Track,
    IN OUT PIO_REQUEST IoRequestCurrent
    )

/*++

Routine Description:

    Format a track.

    This routine must first calculate the floppy id information for
    the first sector on the requested tape track.  First, the logical sector
    is calculated.  Next the head, cylinder, and starting sector are
    calculated as follows:

                    logical sector
    head  =  -----------------------
                sectors per floppy side

                    logical sector  %  sectors per floppy side
    cylinder  =  ------------------------------------------
                            floppy sectors per floppy track

    sector  =  logical sector  %  sectors per floppy side  +  1



Arguments:

    TapeExtension -

    Irp -

    Track - tape track to format.

    IoRequestCurrent - current Request Queue entry

Return Value:



--*/

{
    STATUS retval;                     // return value
    LONG logSector;                    // logical sector number
    SHORT i;                           // loop counter
    union format_header hdrData;       // sector id data
    ULONG  *hdrPtr;                    // pointer to sector id data for format
    FDC_STATUS fStat;                  // FDC status response
    SHORT statLength;                  // length of FDC status response
    struct fdc_result result;


    TapeExtension->FmtOp.Segments = TapeExtension->FmtOp.Head = 0;
    logSector = (LONG)Track * (LONG)TapeExtension->TapeParms.FsectTtrack;

    while (logSector >= (SHORT)TapeExtension->TapeParms.FsectFside) {

        logSector -= TapeExtension->TapeParms.FsectFside;
        TapeExtension->FmtOp.Head++;

    }

    TapeExtension->FmtOp.Cylinder = (UCHAR)((SHORT)logSector /
                (SHORT)TapeExtension->TapeParms.FsectFtrack);
    TapeExtension->FmtOp.Sector = (UCHAR)(((SHORT)logSector %
                (SHORT)TapeExtension->TapeParms.FsectFtrack) + 1);

    TapeExtension->FmtOp.MdlAddress = Irp->MdlAddress;
    TapeExtension->FmtOp.HdrPtr = (ULONG  *)(IoRequestCurrent->Data);

    //
    // Set the tape drive to the specified tape track.
    //

    if ((retval = Q117iChangeTrack(TapeExtension, (SHORT)Track)) == NoErr) {

        //
        // start the tape
        //

        TapeExtension->FmtOp.retval = NoErr;
        TapeExtension->TapePosition.LogFwd = TRUE;
        TapeExtension->QControllerData->StartFormatMode = TRUE;
        TapeExtension->QControllerData->EndFormatMode = FALSE;

        Q117iDLockUnlockDMA(TapeExtension, TRUE);

        retval = Q117iSendByte(TapeExtension, Logical_Fwd);

        CheckedDump(QIC117INFO,( "Q117i: FmtOp int retval %d (decimal)\n", TapeExtension->FmtOp.retval));
        CheckedDump(QIC117INFO,( "Q117i: FmtOp retval %d (decimal)\n", retval));

        if (retval) {

            Q117iResetFDC(TapeExtension);
            retval = Q117iStopTape(TapeExtension);

            if (retval == NoErr) {

                retval = BadFmt;

            }

        } else {

            //
            // Complete the send byte we started during an interrupt
            //

            if ((retval = Q117iReadFDC(TapeExtension, (CHAR *)&result, &statLength))
                == NoErr) {

                if (! (result.ST0 & ST0_IC)) {

                    if( TapeExtension->QControllerData->InterfaceType != MicroChannel) {

                        if (result.ST0 !=
                            (UCHAR)(TapeExtension->DriveParms.DriveSelect | ST0_SE)) {

                            retval = NECFlt;

                        }
                    }

                    if (TapeExtension->FmtOp.NCN != result.PCN) {

                        retval = CmdFlt;

                    }

                    TapeExtension->QControllerData->FDC_Pcn = result.PCN;

                } else {

                    retval = NECFlt;

                }

            }

            Q117iSleep(TapeExtension, mt_wt090ms, FALSE);

            //
            // If the tape drive is ready when all of the segments have been
            // formatted we must assume something went wrong (probably missed
            // index pulses).
            //

            if (Q117iGetDriveError(TapeExtension) == NotRdy) {

                if ((retval = Q117iWaitCommandComplete(
                                    TapeExtension,
                                    mt_wt035s)) == NoErr) {

                    TapeExtension->TapePosition.LogFwd = FALSE;

                    if (!TapeExtension->DriveParms.Status.BOT
                        && !TapeExtension->DriveParms.Status.EOT) {

                        TapeExtension->FirmwareError = Bad_Log_Fwd;
                        retval = TapeFlt;

                    }

                } else {

                    Q117iStopTape(TapeExtension);
                    TapeExtension->FirmwareError = Motion_Timeout;

                }

            } else {

                retval = BadFmt;

            }

        }

    }

    TapeExtension->QControllerData->StartFormatMode = FALSE;
    TapeExtension->QControllerData->EndFormatMode = FALSE;

    return(retval);
}

STATUS
Q117iWriteReferenceBurst(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Write the regerence burst on the tape.

    This operation may be attempted more than once if necessary to
    successfully write the regerence burst. If the drive reports an
    unreferenced tape after this operation then abort the format.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval;    // return value
    SHORT i = 0;     // loop counter

    do {

        if ((retval = Q117iSendByte(TapeExtension, Write_Ref)) == NoErr) {

            retval = Q117iWaitCommandComplete(TapeExtension, mt_wt460s);

        }

    } while (++i < WRITE_REF_RPT &&
            !TapeExtension->DriveParms.Status.Referenced &&
            retval == NoErr);

    if ((retval == NoErr) && !TapeExtension->DriveParms.Status.Referenced) {

        retval = BadFmt;

    }

    return(retval);
}
