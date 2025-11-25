/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    tapecmd.c

Abstract:


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
Q117iTapeCommands(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST IoRequestCurrent,
    IN OUT PIRP Irp
    )

/*++

Routine Description:

    Execute an IORequest command. TapeCommands is merely a command controller,
    calling the appropriate routines to execute the IORequiest commands.

Arguments:

    TapeExtension -

    IoRequestCurrent -

    Irp -

Return Value:


--*/

{
    int retval = NoErr;

    if (TapeExtension->Found == FALSE &&
        IoRequestCurrent->Command != DFndDrv &&
        IoRequestCurrent->Command != DGetFDC
            ) {

        return(NoDrive);

    }

    switch (IoRequestCurrent->Command) {

        case DFndDrv:
            retval = Q117iDFndDrv(TapeExtension);
            break;

        case DRead:
        case DWrite:
        case DVerify:
        case DRetry:
        case DWriteBad:
        case DReadBad:
            retval = Q117iReadWrite(TapeExtension, IoRequestCurrent, Irp);
            break;

        case DFmt:
            retval = Q117iDFmt(TapeExtension, Irp, IoRequestCurrent);
            break;

        case DEject:
            retval = Q117iDEject(TapeExtension);
            break;

        case DFast:
        case DSlow:
            retval = Q117iDFast_DSlow(TapeExtension, IoRequestCurrent->Command);
            break;

        case DSndWPro:
            retval = Q117iDSndWPro(TapeExtension);
            break;

        case DGetRev:
            retval = Q117iDGetRev(TapeExtension, IoRequestCurrent->Data);
            break;

        case DReten:
            retval = Q117iDReten(TapeExtension);
            break;

        case DGetCap:
            retval = Q117iDGetCap(TapeExtension, IoRequestCurrent->Data);
            break;

        case DChkDrv:
            retval = Q117iDChkDrv(TapeExtension, IoRequestCurrent->Data);
            break;

        case DSndReel:
            retval = Q117iDSndReel(TapeExtension);
            break;

        case DGetCart:
            retval = Q117iDGetCart(TapeExtension, IoRequestCurrent->Data);
            break;

        case DGetSpeed:
            retval = Q117iDGetSpeed(TapeExtension, IoRequestCurrent->Data);
            break;

        case DStatus:
            retval = Q117iDStatus(TapeExtension, IoRequestCurrent->Data);
            break;

        case DGetFDC:
            retval = Q117iDGetFDC(TapeExtension, IoRequestCurrent->Data);
            break;

        case DReportProtoVer:
            retval = Q117iDReportProtoVer(TapeExtension, IoRequestCurrent->Data);
            break;

        case DSelPart:
        case DGetPos:
        case DIgnore:
        case DUseLock:
            break;

        case DGetDriveInfo:
            retval = Q117iDGetDriveInfo(TapeExtension, IoRequestCurrent->Data);
            break;

        case DChkFmt:
            retval = Q117iDChkFmt(TapeExtension);
            break;

        case DDeselect:
            retval = Q117iDDeselect(TapeExtension);
            break;

        case DClearNewCart:
            CheckedDump(QIC117SHOWTD,("Q117i: DClearNewCart received\n"));
            TapeExtension->PersistentNewCart = FALSE;
            retval = NoErr;
            break;

        default:
            retval = InvalCmd;
            break;

    }

    return(retval);
}


STATUS
Q117iDEject(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Position the tape to the physical BOT and track 0.

Arguments:

    TapeExtension -

Return Value:


--*/

{
    STATUS retval;

    if ((retval = Q117iStopTape(TapeExtension)) != NoErr) {

        return(retval);

    }

    if((retval = Q117iSendByte(TapeExtension, Physical_Rev)) != NoErr) {

        return(retval);

    }

    if((retval = Q117iWaitCommandComplete(
        TapeExtension,
        TapeExtension->TapeParms.TimeOut[PHYSICAL])) != NoErr) {

        return(retval);

    }

    TapeExtension->TapePosition.C_Segment = 0;

    if (TapeExtension->DriveParms.Flavor ==
        CMS && !TapeExtension->DriveParms.Status.Referenced) {

        //
        // Park the head if the tape is not referenced and the drive is Jumbo
        // A. (Do nothing if tape not referenced and the drive is Jumbo B).
        //

        if (TapeExtension->DriveParms.Version < FIRM_VERSION_60) {

            if ((retval = Q117iSetDriveMode(TapeExtension, DIAGNOSTIC_1_MODE)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iSendByte(TapeExtension, Park_Head)) != NoErr) {

                return(retval);

            }

            //
            // Can't use Q117iWaitCommandComplete since the drive ready bit
            // of the status byte is not updated during the Park_Head command.
            // Because of this, you must sleep until the head is parked or
            // the drive will not "see" the set primary mode command.
            //

            Q117iSleep(TapeExtension, mt_wt007s, FALSE);
            Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);
            retval = Q117iWaitCommandComplete(TapeExtension, mt_wt007s);
            TapeExtension->TapePosition.C_Track = -1;

        }

    } else {

        if ((retval = Q117iSendByte(TapeExtension, Seek_Track)) != NoErr) {

            return(retval);

        }

        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

        if ((retval = Q117iSendByte(TapeExtension, 2 /* Track 0 */)) != NoErr) {

            return(retval);

        }

        retval = Q117iWaitCommandComplete(TapeExtension, mt_wt007s);
        TapeExtension->TapePosition.C_Track = 0;

    }

    return(retval);
}


STATUS
Q117iDReten(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Retension the tape by first going to physical EOT then turning around and
    going to physical BOT

Arguments:

    TapeExtension -

Return Value:


--*/

{
    STATUS retval;

    if ((retval = Q117iSendByte(TapeExtension, Physical_Fwd)) != NoErr) {

        return(retval);

    }

    if ((retval = Q117iWaitCommandComplete(
            TapeExtension,
            TapeExtension->TapeParms.TimeOut[PHYSICAL]))
            != NoErr) {

        return(retval);

    }

    if ((retval = Q117iSendByte(TapeExtension, Physical_Rev)) != NoErr) {

        return(retval);

    }

    if ((retval = Q117iWaitCommandComplete(
            TapeExtension,
            TapeExtension->TapeParms.TimeOut[PHYSICAL]))
            != NoErr) {

        return(retval);

    }

    TapeExtension->TapePosition.C_Segment = 0;
    return(NoErr);

}


STATUS
Q117iDFast_DSlow(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    )

/*++

Routine Description:

    Set the operating speed of the tape drive and the corresponding transfer
    rate of the FDC. Setting the operating speed can currently be done only
    on CMS drives. It is assumed that no ther drive manufacturers have
    multi-speed drive.

Arguments:

    TapeExtension -

    Command -

Return Value:


--*/

{
    STATUS retval;
    struct DriveConfiguration driveConfig;
    CHAR newSpeed;

    if ((retval = Q117iStopTape(TapeExtension)) != NoErr) {

        return(retval);

    }

    if (Command == DFast) {

        TapeExtension->XferRate.XferRate = FAST;

    } else {

        TapeExtension->XferRate.XferRate = SLOW;

    }

    if ((retval = Q117iSendByte(TapeExtension, Select_Speed)) != NoErr) {

        return(retval);

    }

    Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

    if (Command == DFast) {

        newSpeed = TapeExtension->XferRate.TapeFast + 2;

    } else {

        newSpeed = TapeExtension->XferRate.TapeSlow + 2;

    }

    if ((retval = Q117iSendByte(TapeExtension, newSpeed)) != NoErr) {

        return(retval);

    }

    Q117iConfigureFDC(TapeExtension);

    if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt010s)) != NoErr) {

        return(retval);

    }

    if ((retval = Q117iReport(TapeExtension,
                            Report_Confg,
                            (USHORT *)&driveConfig,
                            READ_BYTE,
                            NULL))
                            != NoErr) {

        return(retval);

    }

    if (Command == DFast ?
        driveConfig.XferRate != TapeExtension->XferRate.TapeFast :
        driveConfig.XferRate != TapeExtension->XferRate.TapeSlow) {

        retval = TapeFlt;
        TapeExtension->FirmwareError = Speed_not_Avail;

    }

    return(retval);
}


STATUS
Q117iDGetRev(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR Version
    )

/*++

Routine Description:

    Get the current version of firmware in the tape drive.

Arguments:

    TapeExtension -

    Version -

Return Value:


--*/

{
    STATUS retval;
    UCHAR ROM_Version;

    if ((retval = Q117iReport(TapeExtension,
                            Report_ROM,
                            (USHORT *)&ROM_Version,
                            READ_BYTE,
                            NULL))
                            != NoErr) {

        return(retval);

    }

    *Version = ROM_Version;
    return(NoErr);
}


STATUS
Q117iDGetCap(
    IN PTAPE_EXTENSION TapeExtension,
    struct S_O_DGetCap *Capacity
    )

/*++

Routine Description:

    Get the current tape type, which includes the tape format (if any) and
    the tape length ( in the case of a QIC-40 format tape). If the tape is
    not referenced then that information must be included.

Arguments:

    TapeExtension -

    Capacity -

Return Value:


--*/

{
    STATUS retval;

    retval = Q117iGetTapeParameters(TapeExtension);

    if (!retval) {

        //
        // floppy sectors per segment
        //
        Capacity->SectsPerSegment = TapeExtension->TapeParms.FsectSeg;

        //
        // segments per tape track
        //
        Capacity->SegmentsPerTrack = TapeExtension->TapeParms.SegTtrack;

        //
        // number of tape tracks
        //
        Capacity->TracksPerTape = TapeExtension->TapeParms.NumTtrack;

        //
        // segments per floppy track
        //
        Capacity->SegmentsPerFTrack = TapeExtension->TapeParms.SegFtrack;

        //
        // floppy tracks per floppy side
        //
        Capacity->FTrackPerFSide = TapeExtension->TapeParms.FtrackFside;

        //
        // floppy sectors per floppy track
        //
        Capacity->MaxFSector = TapeExtension->TapeParms.FsectFtrack;

        Capacity->drive_type = TapeExtension->DriveParms.DriveType;

        if (TapeExtension->DriveParms.Status.Referenced) {

            Capacity->referenced = TRUE;

        } else {

            Capacity->referenced = FALSE;

        }

        Capacity->TapeFormatCode = TapeExtension->TapeParms.TapeFormatCode;

        Capacity->FormattableSegments =
            TapeExtension->TapeParms.FormattableSegs;

    }

    return(retval);
}



STATUS
Q117iDComFirm(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PUCHAR CommandString
    )

/*++

Routine Description:

    Send and receive non-standard data to/from the tape drive.

Arguments:

    TapeExtension -

    CommandString -

Return Value:


--*/

{
    STATUS retval = NoErr;
    UCHAR tCount;
    CHAR sendData;
    USHORT receiveData;
    SHORT waitTime;
    SHORT receiveLength;

    tCount = *CommandString++;

    while (tCount) {

        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

        sendData = *CommandString++;

        if ((retval = Q117iSendByte(TapeExtension, sendData)) != NoErr) {

            return(retval);
        }

        --tCount;

    }

    tCount = *CommandString++;

    switch (tCount) {

    case 0xff:
        retval = Q117iWaitCommandComplete(TapeExtension, mt_wt300s);
        break;

    case 0xfe:
        waitTime = *(SHORT *)CommandString;

        if (waitTime) {

            Q117iSleep(TapeExtension, waitTime, FALSE);

        }

        break;

    default:
        if (tCount == 0xfd) {

            TapeExtension->NoPause = TRUE;
            tCount = *CommandString++;

        }

        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

        if (tCount != 0) {

            if (tCount == 1) {

                receiveLength = READ_BYTE;

            } else {

                receiveLength = READ_WORD;

            }

            if ((retval = Q117iReceiveByte(TapeExtension,
                                        receiveLength,
                                        &receiveData))
                                        != NoErr) {

                return(retval);

            }

            if (tCount == 1) {

                *CommandString = (CHAR)receiveData;

            } else {

                *(USHORT *)CommandString = receiveData;

            }

        }

        break;

    }
    return(retval);
}

STATUS
Q117iDSndWPro(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Report the write protect status of the tape.

Arguments:

    TapeExtension -

Return Value:


--*/

{
    STATUS retval;

    if((retval = Q117iGetDriveError(TapeExtension)) == NoErr) {

        if (TapeExtension->DriveParms.Status.WriteProtect) {

            retval = WProt;

        }

    }

    return(retval);
}

STATUS
Q117iDSndReel(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Return cartridge in status.

Arguments:

    TapeExtension -

Return Value:


--*/

{
    STATUS retval;

    retval = Q117iGetDriveError(TapeExtension);

    if (retval == NoErr || retval == NewCart) {

        if (TapeExtension->NewCart) {

            Q117iNewTape(TapeExtension);

        }

        retval = NoErr;

    } else {

        if (retval == NotRdy) {

            retval = NoTape;

        }

    }

    return(retval);
}

STATUS
Q117iDGetCart(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR TapeType
    )

/*++

Routine Description:

    Return new cartidge status as necessary.

Arguments:

    TapeExtension -

    TapeType -

Return Value:


--*/

{
    STATUS retval;

    retval = Q117iWaitCommandComplete(TapeExtension, mt_wt150s);

    if (retval == NoErr || retval == NewCart) {

        if (TapeExtension->NewCart) {

            //
            // If there is an illegitimate write protect status due to the
            // firmware 63 bug, the call to Q117iNewTape will clear the
            // write-protect bit from the status byte.
            //

            retval = Q117iNewTape(TapeExtension);

            if (retval == NoErr) {

                retval = NewCart;
            }

        }

    } else {

        //
        // This is to test to see if the tape drive erroneously thought the
        // tape was invalide media and correct the error if so
        //

        if (retval == TapeFlt && TapeExtension->FirmwareError == Inval_Media) {

            retval = Q117iNewTape(TapeExtension);

            if (retval == NoErr) {

                retval = NewCart;
            }

        }

    }

    *TapeType = TapeExtension->TapeParms.TapeType;
    return(retval);
}


STATUS
Q117iDChkDrv(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR DriveType
    )

/*++

Routine Description:

    Return the drive manufacturer (CMS or ALIEN).

Arguments:

    TapeExtension -

    DriveType -

Return Value:


--*/

{
    *DriveType = TapeExtension->DriveParms.Flavor;
    return(NoErr);
}

STATUS
Q117iDGetSpeed(
    IN PTAPE_EXTENSION TapeExtension,
    OUT struct S_O_DGetSpeed *TransferRate
    )

/*++

Routine Description:

    Return the current FDC transfer rate.

Arguments:

    TapeExtension -

    TransferRate-

Return Value:


--*/

{
    switch (TapeExtension->XferRate.XferRate ?
            TapeExtension->XferRate.TapeFast :
            TapeExtension->XferRate.TapeSlow) {

    case TAPE_250Kbps:
        TransferRate->CurSpeed = 250/8;
        break;

    case TAPE_500Kbps:
        TransferRate->CurSpeed = 500/8;
        break;

    case TAPE_1Mbps:
        TransferRate->CurSpeed = 1000/8;
        break;

        case TAPE_2Mbps:
        TransferRate->CurSpeed = 2000/8;
        break;

    }

    switch (TapeExtension->XferRate.MaxRate ?
            TapeExtension->XferRate.FDC_Fast :
            TapeExtension->XferRate.FDC_Slow) {

    case FDC_250Kbps:
        TransferRate->FmtSpeed = 250/8;
        break;

    case FDC_500Kbps:
        TransferRate->FmtSpeed = 500/8;
        break;

    case FDC_1Mbps:
        TransferRate->FmtSpeed = 1000/8;
        break;

    case FDC_2Mbps:
        TransferRate->FmtSpeed = 2000/8;
        break;

    }

    return(NoErr);
}

STATUS
Q117iDStatus(
    IN PTAPE_EXTENSION TapeExtension,
    OUT struct S_O_DStatus *DriveStatus
    )

/*++

Routine Description:



Arguments:

    TapeExtension -

    DriveStatus -

Return Value:


--*/

{
    DriveStatus->underruns = 0;
//    DriveStatus->retries = cmd_SoftErrors;

    return(NoErr);
}



STATUS
Q117iDGetFDC(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR FdcType
    )

/*++

Routine Description:

    Determine whether or not the floppy controller is an 82077.

Arguments:

    TapeExtension -

    FdcType -

Return Value:


--*/

{
    STATUS retval = NoErr;
    SHORT i, test;
    UCHAR status = 0;
    SHORT statLength = 0;
    struct version_cmd verCmd;
    struct national_cmd nscCmd;
    struct PerpMode perpMode;
    struct part_id_cmd partId;

    // Set up the Perp Mode Command

    perpMode.command = PERP_MODE_COMMAND;
    perpMode.drive_select = 0;
    perpMode.wgate = 0;
    perpMode.gap = 0;
    perpMode.reserved = 0;
    perpMode.over_write = TRUE;

    verCmd.command = FDC_VERSION;
    nscCmd.command = NSC_VERSION;
    partId.command = FDC_PART_ID;
    *FdcType = FDC_UNKNOWN;


    //
    // Check for an enhanced type controller by issuing the version command.
    //

    if ((retval = Q117iProgramFDC(TapeExtension,
                                (CHAR *)&verCmd,
                                sizeof(verCmd),
                                FALSE))
                                == NoErr) {

        if((retval = Q117iReadFDC(TapeExtension,
                                (CHAR *)&status,
                                (SHORT *)&statLength))
                                == NoErr) {

            if (statLength != 1) {

                retval = NECFlt;

            } else {

                if (status == VALID_NEC_FDC) {

                    *FdcType = FDC_ENHANCED;

                } else {

                    *FdcType = FDC_NORMAL;

                }
            }

        }
    }

    //
    // Determine if the controller is a National 8477 by issuing the NSC
    // command which is specific to National parts and returns 0x71. The
    // lower four bits are subject to change by National and will reflect
    // the version of the part in question.  At this point we will only test
    // the high four bits.
    //

    if (*FdcType == FDC_ENHANCED && retval == NoErr) {

        if ((retval = Q117iProgramFDC(TapeExtension,
                                    (CHAR *)&nscCmd,
                                    sizeof(nscCmd),
                                    FALSE))
                                    == NoErr) {

            if ((retval = Q117iReadFDC(TapeExtension,
                                    (CHAR *)&status,
                                    (SHORT *)&statLength))
                                    == NoErr) {

                if (statLength != 1) {

                    retval = NECFlt;

                } else {

                    if ((status & NSC_MASK) == NSC_PRIMARY_VERSION) {

                        *FdcType = FDC_NATIONAL;

                    }

                }

            }
        }
    }

    //
    // Determine if the controller is an 82077 by issuing the perpendicular
    // mode command which at this time is only valid on 82077's.
    //

    if (*FdcType == FDC_ENHANCED && retval == NoErr) {

        if ((retval = Q117iProgramFDC(TapeExtension,
                                    (CHAR *)&perpMode,
                                    sizeof(perpMode),
                                    FALSE))
                                    == NECFlt) {

            if ((retval = Q117iReadFDC(TapeExtension,
                                    (CHAR *)&status,
                                    (SHORT *)&statLength))
                                    == NoErr) {

                if (statLength != 1) {

                    retval = NECFlt;

                }

            }

        } else {

            *FdcType = FDC_82077;

        }
    }

    //
    // Determine if the controller is an 82077AA by setting the tdr to several
    // valid values and reading the results to determine if in fact the tdr
    // is active.  Only the AA parts have an active tdr.
    //

    if (*FdcType == FDC_82077 && retval == NoErr) {

        for (i = 0, test = 0; i < FDC_REPEAT; i++) {

            WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->tdr, i);

            if (i == (FDC_TDR_MASK & READ_CONTROLLER(
                        &TapeExtension->QControllerData->FDC_Addr->tdr))) {

                test++;

            }

        }

        if (test == FDC_REPEAT) {

            *FdcType = FDC_82077AA;
            CheckedDump(QIC117INFO,( "Q117i: FdcType = FDC_82077AA\n"));

        }

    }

#if DBG

    switch (*FdcType) {
    case FDC_NORMAL:
        CheckedDump(QIC117INFO,( "Q117i: FdcType = FDC_NORMAL\n"));
        break;
    case FDC_ENHANCED:
        CheckedDump(QIC117INFO,( "Q117i: FdcType = FDC_ENHANCED\n"));
        break;
    case FDC_NATIONAL:
        CheckedDump(QIC117INFO,( "Q117i: FdcType = FDC_NATIONAL\n"));
        break;
    case FDC_82077:
        CheckedDump(QIC117INFO,( "Q117i: FdcType = FDC_82077\n"));
        break;
    case FDC_82077AA:
        CheckedDump(QIC117INFO,( "Q117i: FdcType = FDC_82077AA\n"));
        break;
    default:
        CheckedDump(QIC117INFO,( "Q117i: FdcType = FDC_UNKNOWN\n"));
    }

#endif

    return(retval);
}


STATUS
Q117iDGetDriveInfo(
    IN PTAPE_EXTENSION TapeExtension,
    OUT struct S_O_DMiscInfo *MiscellaneousInformation
    )

/*++

Routine Description:

    Gets the followin information:
        1) Drive Type/Model
        2) Firmware Revision
        3) Info Exists Flag
        4) OEM Field
        5) Serial Number
        6) Date of manufacture

Arguments:

    TapeExtension -

    MiscellaneousInformation -

Return Value:



--*/

{
    CHAR    i;

    MiscellaneousInformation->drive_type =
        TapeExtension->MiscDriveInfo.DriveType;
    MiscellaneousInformation->ROM_Version =
        TapeExtension->MiscDriveInfo.ROM_Version;
    MiscellaneousInformation->info_exists =
        TapeExtension->MiscDriveInfo.InfoExists;

    for (i=0; i<SERIAL_NUM_LENGTH; ++i) {

        MiscellaneousInformation->serial_number[i] =
            TapeExtension->MiscDriveInfo.SerialNumber[i];

    }

    for (i=0; i<MAN_DATE_LENGTH; ++i) {

        MiscellaneousInformation->man_date[i] =
            TapeExtension->MiscDriveInfo.ManDate[i];

    }

    for (i=0; i<OEM_LENGTH; ++i) {

        MiscellaneousInformation->oem[i] =
            TapeExtension->MiscDriveInfo.Oem[i];

    }

    return(NoErr);
}


STATUS
Q117iDChkFmt(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Checks the tape format and compares it to the drive type.

    If the drive type is CMS and the firmware version is 80 or greater, the
    following status condition are returned:

        WrongFmt - Indicates that a QIC-40 tape was detected in a QIC-80
                drive.

        IncompTapeFmt - Indicates that a QIC-80 tape was detected in a QIC-40
                        drive.

    If the drive type is not CMS or the firmware version is earlier then the
    following error condition are returned:

        WrongFmt - Indicates that a QIC-40 tape was detected in a QIC-80
                drive.

        IncompTapeFmt - Indicates that the drive is not reference and it is
                        suspected that a QIC-80 tape is in a QIC-40 drive.

Arguments:

    TapeExtension -

Return Value:


--*/

{
    STATUS retval = NoErr;
    SHORT fmtStat = NoErr;
    CHAR currentMode;
    struct CmsStatus cmsStatus;


    if (TapeExtension->DriveParms.Flavor == CMS &&
        TapeExtension->DriveParms.Version >= FIRM_VERSION_80) {

        //
        // Save current drive mode and put drive into diagnostics mode
        //

        currentMode = TapeExtension->DriveParms.Mode;
        retval = Q117iSetDriveMode(TapeExtension, DIAGNOSTIC_1_MODE);

        if (retval != NoErr) {

            return(retval);

        }

        //
        // Get the drive type via the Rpt_cmsStatus command
        //

        retval = Q117iReport(TapeExtension,
                            Rpt_CMS_Status,
                            (USHORT *)&cmsStatus,
                            READ_BYTE,
                            NULL);

        if (retval != NoErr) {

            return(retval);

        }

        if (!cmsStatus.DriveType) {

            //
            // If drive type is QIC80, the drive_type bit is set low
            // and the eagle bit is low.
            //

            if (!cmsStatus.Eagle) {

                if ((TapeExtension->TapeParms.TapeType == QIC40_SHORT) ||
                    (TapeExtension->TapeParms.TapeType == QIC40_LONG) ||
                    (TapeExtension->TapeParms.TapeType == QICEST_40)) {

                        fmtStat = WrongFmt;

                }

            } else {

                // Drive Type is Eagle

                if ((TapeExtension->TapeParms.TapeType == QIC40_SHORT) ||
                    (TapeExtension->TapeParms.TapeType == QIC40_LONG) ||
                    (TapeExtension->TapeParms.TapeType == QICEST_40)) {

                        fmtStat = QIC40InEagle;

                }
                if ((TapeExtension->TapeParms.TapeType == QIC80_SHORT) ||
                    (TapeExtension->TapeParms.TapeType == QIC80_LONG) ||
                    (TapeExtension->TapeParms.TapeType == QICEST_80)) {

                        fmtStat = QIC80InEagle;

                }
            }

        } else {

            //
            // It must be a QIC40 Drive
            //

            if ((TapeExtension->TapeParms.TapeType == QIC80_SHORT) ||
                (TapeExtension->TapeParms.TapeType == QIC80_LONG) ||
                (TapeExtension->TapeParms.TapeType == QICEST_80)) {

                    fmtStat = IncompTapeFmt;

            }

        }

        //
        // To exit DIAGNOSTIC_1_MODE, put drive into PRIMARY_MODE
        //

        retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);

        if (retval != NoErr) {

            return(retval);

        }

        //
        // Restore drive to its original mode
        //

        if (currentMode != PRIMARY_MODE) {

            retval = Q117iSetDriveMode(TapeExtension, currentMode);

            if (retval != NoErr) {

                return(retval);

            }

        }

    } else {

        //
        // It is not a CMS Drive or it is not firmware version 80 or better
        //

        if ((TapeExtension->DriveParms.DriveType == QIC80_DRIVE) &&
            ((TapeExtension->TapeParms.TapeType == QIC40_SHORT) ||
            (TapeExtension->TapeParms.TapeType == QIC40_LONG) ||
            (TapeExtension->TapeParms.TapeType == QICEST_40))) {

            fmtStat = WrongFmt;

        }

    }

    return(fmtStat);
}


STATUS
Q117iDReportProtoVer(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR  PrototypeVersion
    )

/*++

Routine Description:

    Gets the Prototype firmware version from the drive.

Arguments:

    TapeExtension -

    PrototypeVersion -

Return Value:


--*/

{
    UBYTE protoVersion = 0;
    CHAR currentMode;
    STATUS retval = NoErr;

    if (TapeExtension->DriveParms.Flavor == CMS &&
        TapeExtension->DriveParms.Version >= FIRM_VERSION_80) {

        //
        // Save current drive mode and put drive into diagnostics mode
        //

        currentMode = TapeExtension->DriveParms.Mode;
        retval = Q117iSetDriveMode(TapeExtension, DIAGNOSTIC_1_MODE);

        //
        // Since we are only looking at the firmware, ignore all NoTape
        // errors.
        //

        if (retval != NoErr && retval != NoTape) {

            return(retval);

        }

        //
        // Get the firmware prototype version number
        //

        retval = Q117iReport(TapeExtension,
                            ReportProtoVer,
                            (USHORT *)&protoVersion,
                            READ_BYTE,
                            NULL);

        //
        // Since we are only looking at the firmware, ignore all NoTape errors
        //

        if (retval != NoErr && retval != NoTape) {

            return(retval);

        }

        //
        // To exit DIAGNOSTIC_1_MODE, put drive into PRIMARY_MODE
        //

        retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);

        //
        // Since we are only looking at the firmware, ignore all NoTape errors
        //

        if (retval != NoErr && retval != NoTape) {

            return(retval);

        }

        //
        // Restore drive to its original mode
        // Since we are only looking at the firmware, ignore all NoTape errors
        //

        if (currentMode != PRIMARY_MODE) {

            retval = Q117iSetDriveMode(TapeExtension, currentMode);

            if (retval != NoErr && retval != NoTape) {

                return(retval);

            }

        }

    }

    *PrototypeVersion = protoVersion;
    return(NoErr);
}
