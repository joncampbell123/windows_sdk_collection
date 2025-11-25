/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    fnddrv.c

Abstract:

    scans the floppy bus for a qic117 device.

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
Q117iDFndDrv(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Find if and where tape drive is (B or D). Configure the drive and tape
    drive as necessary.

Arguments:

    TapeExtension -

Return Value:



--*/

{

    STATUS retval = NoErr;     // return value
    SHORT i;                  // loop variable
    LARGE_INTEGER controllerWait;
    NTSTATUS waitStatus;


    controllerWait = RtlLargeIntegerNegate(
                        RtlConvertLongToLargeInteger(
                        (LONG)(10 * 1000 * 15000)
                        )
                        );

    waitStatus = STATUS_SUCCESS;

    CheckedDump(QIC117INFO,( "Q117i: Waiting Controller Event\n"));

    waitStatus = KeWaitForSingleObject(
        TapeExtension->QControllerData->ControllerEvent,
        Executive,
        KernelMode,
        FALSE,
        &controllerWait);


    if (waitStatus == STATUS_TIMEOUT) {

        CheckedDump(QIC117INFO,( "Q117i: Timeout Controller Event\n"));
        return (ControllerBusy);

    }

    CheckedDump(QIC117INFO,( "Q117i: Have Controller Event\n"));

    TapeExtension->QControllerData->CurrentInterrupt = TRUE;

    if (!TapeExtension->Found) {

        //
        // If the tape drive address is already known, there is no need to
        // look for it again.
        //

        if ((retval = Q117iDLocateDrv(TapeExtension)) == NoErr) {

            //
            // Now that we know where the tape drive is we must prepare
            // it for the forthcoming operations.  First thing is to make
            // sure that it is in Primary mode so there are no Invalid Command
            // surprises.  Once in Primary mode, we can determine what flavor
            // of drive is out there (CMS or alien; QIC-40, QIC-80, XR4, etc).
            // Next, we need to determine the speed of the FDC so we can set the
            // corresponding speed on our drive (currently this only applies to
            // the CMS drive since we are the only multiple speed drive out
            // there).  Finally, armed with the drive type and the FDC speed,
            // we need to set the necessary speed in the tape drive which is
            // done in ConfigureDrive.
            //

            if (retval == NoErr) {

                retval = Q117iSelectDrive(TapeExtension);
                TapeExtension->QControllerData->PerpModeSelect =
                    (UCHAR)(1 << (TapeExtension->QControllerData->DriveSelect.SelectByte &
                    DRIVE_ID_MASK));

            }

            if (retval != NoErr) {

                if (TapeExtension->QControllerData->AdapterLocked) {

                    Q117iDLockUnlockDMA(TapeExtension, FALSE);

                }

                (VOID) Q117iResetFDC(TapeExtension);

                CheckedDump(QIC117INFO,( "Q117iDFndDrv: Setting Controller Event\n"));

                TapeExtension->NoPause = TRUE;
                TapeExtension->NewCart = FALSE;
                TapeExtension->QControllerData->CurrentInterrupt = FALSE;

                (VOID) KeSetEvent(
                    TapeExtension->QControllerData->ControllerEvent,
                    (KPRIORITY) 0,
                    FALSE );

                return(retval);

            }

            //
            // This fixes the Jumbo B firmware bug where a tape put into
            // the drive slowly is perceive (incorrectly) as invalid media.
            // Since there is no way of knowing the maker of the drive
            // (e.g. CMS, Irwin, etc.), or the type of drive (QIC40, QIC80),
            // it is assumed that it is a CMS QIC80 drive, and Q117ifrmware_fix
            // is called.
            //

            if (TapeExtension->FirmwareError == Inval_Media) {

                if ((retval = Q117iClearTapeError(TapeExtension)) != NoErr) {

                    Q117iDDeselect(TapeExtension);
                    return(retval);

                }
            }

            TapeExtension->DriveParms.Mode = DIAGNOSTIC_1_MODE;
            retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);

            if (retval && retval != NoTape) {

                Q117iDDeselect(TapeExtension);
                return(retval);

            }

            TapeExtension->DriveParms.Flavor = UNKNOWN;
            if ((retval = Q117iGetDriveType(TapeExtension)) != NoErr) {

                CheckedDump(QIC117INFO,( "Q117i: GetDriveType Failed %d (decimal)\n", retval));
                Q117iDDeselect(TapeExtension);
                return(retval);

            }

            if ((retval = Q117iSenseSpeed(TapeExtension)) != NoErr) {

                CheckedDump(QIC117INFO,( "Q117i: SenseSpeed Failed %d (decimal)\n", retval));
                Q117iDDeselect(TapeExtension);
                return(retval);

            }

            if ((retval = Q117iConfigureDrive(TapeExtension)) != NoErr) {

                CheckedDump(QIC117INFO,( "Q117i: ConfigureDrive Failed %d (decimal)\n", retval));
                Q117iDDeselect(TapeExtension);
                return(retval);

            }

            TapeExtension->Found = TRUE;

        }

    } else {

        if ((retval = Q117iSelectDrive(TapeExtension)) != NoErr) {

            CheckedDump(QIC117INFO,( "Q117i: Select Failed %d (decimal)\n", retval));
            Q117iDDeselect(TapeExtension);
            return(retval);

        }

    }

    if (retval == NoErr) {

        retval = Q117iNewTape(TapeExtension);

        if (retval && (retval != NoTape) && (retval != NewCart)) {

            Q117iDDeselect(TapeExtension);
            return(retval);

        } else {

            retval = NoErr;

        }

    }

    return(retval);
}


STATUS
Q117iDLocateDrv(
   IN PTAPE_EXTENSION TapeExtension
   )

/*++

Routine Description:



Arguments:

   TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;  // return value
    SHORT i;                // loop variable

    //
    // If the tape drive address is already known, there is no need to look for
    // it again.
    //

    if (!TapeExtension->Found) {

        //
        // Reset the FDC to start in a known state.
        //

        Q117iResetFDC(TapeExtension);

        //
        // Look for the tape drive at drive B then at drive D.  Since the PCN
        // of each channel of the FDC is independant, the global 'pcn must be
        // manually maintained when switching between channels.  We can assume
        // that the drive has been found (i.e. we can communicate) when we get
        // any response except communication errors (DriveFlt or CmdFlt).
        //

        Q117iSleep(TapeExtension, mt_wt001s, FALSE);

        for (i = 0; i < FIND_RETRIES; i++) {

            TapeExtension->DriveParms.Flavor = UNKNOWN;
            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVEU, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            TapeExtension->DriveParms.Flavor = CMS;
            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVEU, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            TapeExtension->DriveParms.Flavor = SUMMIT;
            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVEU, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            TapeExtension->DriveParms.Flavor = UNKNOWN;
            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVEB, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVED, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            TapeExtension->DriveParms.Flavor = UNKNOWN;
            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVEUB, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            TapeExtension->DriveParms.Flavor = CMS;
            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVEUB, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            TapeExtension->DriveParms.Flavor = SUMMIT;
            Q117iResetFDC(TapeExtension);
            retval = Q117iLookForDrive(TapeExtension, DRIVEUB, TRUE);

            if (retval != DriveFlt && retval != CmdFlt) {

                break;

            }

            Q117iSleep(TapeExtension, mt_wt001s, FALSE);

        }

        //
        // Sort out the results of the drive address search.  A DriveFlt or a
        // CmdFlt indicate that we could never successfully communicate with
        // the tape drive at either address so we must assume that there is
        // no tape drive present. A NECFlt indicates that we had serious
        // trouble talking to the FDC so we must assume that it is either
        // broken or not there.  The last thing to consider here is a TapeFlt.
        // If the TapeFlt indicates either a hardware or software reset it is
        // save to continue and the error can be ignored (since we must be
        // starting a tape session neither of these errors should bother us).
        // If the TapeFlt indicates any other error, it probably means some
        // badness has happened.
        //

        switch (retval) {

        case DriveFlt:
        case CmdFlt:
                CheckedDump(QIC117INFO,( "Q117i: DLocateDrv Failed %d (decimal)\n", NoDrive));
                return(NoDrive);

        case NECFlt:
                CheckedDump(QIC117INFO,( "Q117i: DLocateDrv Failed %d (decimal)\n", NoFDC));
                return(NoFDC);

        case NotRdy:
        case InvalCmd:
                retval = NoErr;
                break;

        case TapeFlt:

                if (TapeExtension->FirmwareError == Swre_Reset ||
                    TapeExtension->FirmwareError == Pwr_On_Reset ||
                    TapeExtension->FirmwareError == Inval_Media ||
                    TapeExtension->FirmwareError == Wtchdog_Reset) {

                    retval = NoErr;

                    break;

                }

                if (TapeExtension->FirmwareError == Xmit_Overrun) {

                    retval = NoDrive;

                }

                CheckedDump(QIC117INFO,( "Q117i: DLocateDrv Failed %d (decimal)\n", retval));
                return(retval);

        default:
                break;

        }
    }

#if DBG

    if (retval) {

        CheckedDump(QIC117INFO,( "Q117i: DLocateDrv Failed %d (decimal)\n", retval));

    }

#endif

   return retval;
}


STATUS
Q117iClearTapeError(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    To correct errors in the Jumbo B drive and firmware version 63.

    This piece of code added due to the face that the Jumbo B drives with
    firmware 63 have a bug where you put a tape in very slowly, they sense
    that they have a tape and engage the motor before the tape is actually
    in. It may also cause the drive to think that the tape is write protected
    when it actually is not. Sending it the New tape command causes it to go
    through the tape loading sequence and fixes these 2 bugs.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval;

    //
    // Send the NewTape command, and then clear the error byte.
    //

    if ((retval = Q117iSendByte(TapeExtension, New_Tape)) == NoErr) {

        retval = Q117iWaitCommandComplete(TapeExtension, mt_wt150s);

    }

    return(retval);
}


STATUS
Q117iLookForDrive(
    IN PTAPE_EXTENSION TapeExtension,
    IN UCHAR DriveSelector,
    IN BOOLEAN WaitForTape
    )
/*++

Routine Description:



Arguments:

    TapeExtension -

    DriveSelector -

Return Value:



--*/

{
    STATUS retval;     // Return value

    //
    // Set the drive select parameters according to the desired target drive
    // selector.
    //

    switch (DriveSelector) {

    case DRIVEB:
        TapeExtension->QControllerData->DriveSelect.SelectByte = selb;
        TapeExtension->QControllerData->DriveSelect.DeselectByte = dselb;
        TapeExtension->DriveParms.DriveSelect = curb;
        break;

    case DRIVED:
        TapeExtension->QControllerData->DriveSelect.SelectByte = seld;
        TapeExtension->QControllerData->DriveSelect.DeselectByte = dseld;
        TapeExtension->DriveParms.DriveSelect = curd;
        break;

    case DRIVEU:
        TapeExtension->QControllerData->DriveSelect.SelectByte = selu;
        TapeExtension->QControllerData->DriveSelect.DeselectByte = dselu;
        TapeExtension->DriveParms.DriveSelect = curu;
        break;

    case DRIVEUB:
        TapeExtension->QControllerData->DriveSelect.SelectByte = selub;
        TapeExtension->QControllerData->DriveSelect.DeselectByte = dselub;
        TapeExtension->DriveParms.DriveSelect = curub;
        break;

    }

    //
    // Try to communicate with the tape drive by requesting drive status.
    // If we can successfully communicate with the drive, wait up to the
    // approximate maximum autoload time (150 seconds) for the tape drive
    // to become ready. This should cover a new tape being inserted
    // immediatley before starting a tape session.
    //

    if ((retval = Q117iSelectDrive(TapeExtension)) == NoErr) {

        if (WaitForTape) {
            retval = Q117iWaitCommandComplete(TapeExtension, mt_wt460s);
        } else {
            retval = Q117iGetDriveError(TapeExtension);
        }

        Q117iDeselectDrive(TapeExtension);

    }

    return(retval);
}

STATUS
Q117iGetDriveType(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Determine what flavor tape drive the drive is talking to. Specifically,
    is the current drive a CMS drive or a non-CMS drive.

Arguments:

    TapeExtension -

Return Value:



--*/

{

    STATUS retval;           // return value
    USHORT vendorId = 0;    // vendor id number from tape drive
    UCHAR signature;
    BOOLEAN reportFailed=FALSE;
    struct CmsStatus cmsStatus;

    //
    // Assume that the tape drive is not a CMS drive and get the ROM version
    // number.
    //

    if ((retval = Q117iReport(
                        TapeExtension,
                        Report_ROM,
                        (USHORT *)&TapeExtension->DriveParms.Version,
                        READ_BYTE, NULL)) != NoErr) {

        return(retval);

    }

    CheckedDump(QIC117INFO,( "Q117i: FW Version %x\n", TapeExtension->DriveParms.Version));

    if ((retval = Q117iSendByte(TapeExtension, Report_Vendor32)) !=
        NoErr) {

        return(retval);

    }

    if ((retval = Q117iReceiveByte(
                        TapeExtension,
                        READ_WORD,
                        (USHORT *)&vendorId)) != NoErr) {

        Q117iGetDriveError(TapeExtension);

        if ((retval = Q117iSendByte(TapeExtension, Report_Vendor32)) !=
            NoErr) {

            return(retval);

        }

        if ((retval = Q117iReceiveByte(
                            TapeExtension,
                            READ_BYTE,
                            (USHORT *)&vendorId)) != NoErr) {

            Q117iGetDriveError(TapeExtension);

            if ((retval = Q117iSendByte(TapeExtension, Report_Vendor)) !=
                NoErr) {

                return(retval);

            }

            if ((retval = Q117iReceiveByte(
                                TapeExtension,
                                READ_BYTE,
                                (USHORT *)&vendorId)) != NoErr) {

                Q117iGetDriveError(TapeExtension);
                retval = NoErr;
                reportFailed = TRUE;
                CheckedDump(QIC117INFO,( "Q117i: Report Vendor ID Failed\n"));

            }

        }

    }

    CheckedDump(QIC117INFO,( "Q117i: Vendor ID %x\n", vendorId));

    if (vendorId == CMS_VEND_NO || reportFailed) {

        if ((retval = Q117iSetDriveMode(TapeExtension, DIAGNOSTIC_1_MODE)) !=
            NoErr) {

            return(retval);

        }

        if ((retval = Q117iSendByte(TapeExtension, Rpt_Signature)) != NoErr) {

            return(retval);

        }

        if ((retval = Q117iReceiveByte(
                        TapeExtension,
                        READ_BYTE,
                        (USHORT *)&signature)) != NoErr) {

            Q117iGetDriveError(TapeExtension);
            retval = NoErr;

        }

        retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);

        if (retval != NoErr && retval != NoTape) {

            return(retval);

        }

    }

    CheckedDump(QIC117INFO,( "Q117i: Signature %x\n", signature));

    if (vendorId == CMS_VEND_NO) {

        if (signature == CMS_SIG) {

            // Issue the Rpt_cmsStatus command to determine if the drive
            // is an IOMEGA.

            if ((retval = Q117iSetDriveMode(TapeExtension,
                                            DIAGNOSTIC_1_MODE)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iReport(
                            TapeExtension,
                            Rpt_CMS_Status,
                            (USHORT *)&cmsStatus,
                            READ_BYTE,
                            NULL)) != NoErr) {

                if ( TapeExtension->DriveParms.Version >= FIRM_VERSION_63 ) {

                    TapeExtension->DriveParms.Flavor = IOMEGA;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Vendor IOMEGA\n"));

                } else {

                    TapeExtension->DriveParms.Flavor = CMS;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Vendor CMS\n"));

                }
                Q117iGetDriveError(TapeExtension);
                retval = NoErr;

            } else {

                TapeExtension->DriveParms.Flavor = CMS;
                CheckedDump(QIC117INFO,( "Q117i: Drive Vendor CMS\n"));

            }

            //
            // Put drive back into its original mode.
            //

            if ((retval = Q117iSetDriveMode(TapeExtension,
                                            PRIMARY_MODE)) != NoErr) {

                return(retval);

            }


        } else {

            TapeExtension->DriveParms.Flavor = UNSUPPORTED;
            CheckedDump(QIC117INFO,( "Q117i: Drive Vendor UNSUPPORTED\n"));

        }

    } else {

        if (vendorId == ARCHIVE_VEND_NO_OLD) {

            TapeExtension->DriveParms.Flavor = ARCHIVE;
            CheckedDump(QIC117INFO,( "Q117i: Drive Vendor ARCHIVE (old version)\n"));

        } else {

            switch (vendorId & VENDOR_MASK) {
            case SUMMIT_VEND_NO:
                TapeExtension->DriveParms.Flavor = SUMMIT;
                CheckedDump(QIC117INFO,( "Q117i: Drive Vendor SUMMIT\n"));
                break;

            case IOMEGA_VEND_NO:
                TapeExtension->DriveParms.Flavor = IOMEGA;
                CheckedDump(QIC117INFO,( "Q117i: Drive Vendor IOMEGA\n"));
                break;

            case WANGTEK_VEND_NO:
                TapeExtension->DriveParms.Flavor = WANGTEK;
                CheckedDump(QIC117INFO,( "Q117i: Drive Vendor WANGTEK\n"));
                break;

            case CORE_VEND_NO:
                TapeExtension->DriveParms.Flavor = CORE;
                CheckedDump(QIC117INFO,( "Q117i: Drive Vendor CORE\n"));
                break;

            case ARCHIVE_VEND_NO_NEW:
                TapeExtension->DriveParms.Flavor = ARCHIVE;
                CheckedDump(QIC117INFO,( "Q117i: Drive Vendor ARCHIVE (new version)\n"));
                break;

            default:
                TapeExtension->DriveParms.Flavor = UNSUPPORTED;
                CheckedDump(QIC117INFO,( "Q117i: Drive Vendor UNSUPPORTED\n"));

                if (reportFailed && (signature == CMS_SIG)) {
                    TapeExtension->DriveParms.Flavor = CMS_ENHANCEMENTS;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Vendor CMS_ENHANCEMENTS\n"));
                }
                if (reportFailed && (signature != CMS_SIG)) {
                    TapeExtension->DriveParms.Flavor = WANGTEK;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Vendor WANGTEK\n"));
                }
            }
        }
    }

    //
    // Get the miscellaneous drive train information from the drive
    //

    if ((retval = Q117iGetDriveInfo(TapeExtension)) != NoErr) {

        CheckedDump(QIC117INFO,( "Q117i: GetDriveInfo Failed %d (decimal)\n", retval));
        return(retval);

    }

    if ((retval = Q117iGetDriveSize(
                TapeExtension,
                reportFailed,
                vendorId,
                signature)) != NoErr) {

        CheckedDump(QIC117INFO,( "Q117i: GetDriveSize Failed %d (decimal)\n", retval));
        return(retval);

    }

    q117iUpdateRegistryInfo( TapeExtension );

    return(retval);
}


STATUS
Q117iGetDriveSize(
    IN PTAPE_EXTENSION TapeExtension,
    IN BOOLEAN ReportFailed,
    IN USHORT VendorId,
    IN UCHAR Signature
    )

/*++

Routine Description:

    Determine the size of the tape drive (QIC40 or QIC80).

    To determine the drive type, an attempt is made to set the drive to
    250Kbs speed. If the drive is a QIC40 this is a valid speed. If the drive
    is a QIC80 this is an invalid speed and an error is returned.



Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval=NoErr;
    struct DriveConfiguration driveConfig;
    struct CmsStatus cmsStatus;
    CHAR orgMode;
         USHORT manDate=0;

    TapeExtension->DriveParms.ArchiveNativeMode = 0;
    TapeExtension->DriveParms.SeekMode = SEEK_TIMED;

    switch (TapeExtension->DriveParms.Flavor) {

    case CMS:

        TapeExtension->SpeedChangeOK = TRUE;
        TapeExtension->PegasusSupported = FALSE;

        if (TapeExtension->DriveParms.Version >= FIRM_VERSION_60) {

            TapeExtension->DriveParms.SeekMode = SEEK_SKIP;

            if (TapeExtension->DriveParms.Version >= FIRM_VERSION_87) {

                TapeExtension->DriveParms.SeekMode = SEEK_SKIP_EXTENDED;

            }

        }

        if (TapeExtension->DriveParms.Version < FIRM_VERSION_110) {

            if ((retval = Q117iSendByte(TapeExtension, Select_Speed)) == NoErr) {

                Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

                if ((retval = Q117iSendByte(TapeExtension, TAPE_250Kbps + 2)) ==
                    NoErr) {

                    retval = Q117iWaitCommandComplete(TapeExtension, mt_wt010s);

                    if (retval == NoErr) {

                        TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
                        CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE\n"));

                    } else if (retval == UnspRate) {

                        TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
                        CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE\n"));
                        retval = NoErr;

                    }
                }
            }

        } else {

            // For versions of 110 and greater, the drive type may be QIC500.
            // Let's just issue the Rpt_cmsStatus command.

            orgMode = TapeExtension->DriveParms.Mode;

            if ((retval = Q117iSetDriveMode(TapeExtension,
                                            DIAGNOSTIC_1_MODE)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iReport(
                            TapeExtension,
                            Rpt_CMS_Status,
                            (USHORT *)&cmsStatus,
                            READ_BYTE,
                            NULL)) != NoErr) {

                return(retval);

            }

            //
            // Put drive back into its original mode.
            //

            if ((retval = Q117iSetDriveMode(TapeExtension,
                                            PRIMARY_MODE)) != NoErr) {

                return(retval);

            }

            if (orgMode != PRIMARY_MODE) {

                if ((retval = Q117iSetDriveMode(TapeExtension,
                                                orgMode)) != NoErr) {

                    return(retval);

                }
            }

            if (!cmsStatus.DriveType) {

                // If drive type is QIC80, the drive_type bit is set low
                // and the eagle bit is high.

                if (!cmsStatus.Eagle) {

                    TapeExtension->DriveParms.DriveType = QIC500_DRIVE;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC500_DRIVE\n"));

                } else {

                    // Drive Type is QIC80

                    TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE\n"));

                }

            } else {

                 // It must be a QIC40 Drive

                 TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
                 CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE\n"));

            }
        }

        manDate = (UBYTE)TapeExtension->MiscDriveInfo.ManDate[0];
        manDate <<= 8;
        manDate |= (UBYTE)TapeExtension->MiscDriveInfo.ManDate[1];

        CheckedDump(QIC117INFO,( "Q117i: CMS Manufacture Date %d (decimal)\n",
            manDate));

        if ((TapeExtension->DriveParms.Version >= FIRM_VERSION_87) &&
            (manDate >= PEGASUS_START_DATE) &&
                                (TapeExtension->DriveParms.DriveType != QIC40_DRIVE)) {

            CheckedDump(QIC117INFO,( "Q117i: Pegasus Drive Supported (CMS)\n"));
            TapeExtension->PegasusSupported = TRUE;

        }
        break;

    case SUMMIT:

        TapeExtension->DriveParms.SeekMode = SEEK_SKIP;

        if ((retval = Q117iSendByte(TapeExtension, Select_Speed)) == NoErr) {

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

            if ((retval = Q117iSendByte(TapeExtension, (TAPE_1Mbps + 2))) == NoErr) {

                retval = Q117iWaitCommandComplete(TapeExtension, mt_wt010s);

                if (retval == NoErr) {

                    if ((retval = Q117iReport(TapeExtension,
                                            Report_Confg,
                                            (USHORT *)&driveConfig,
                                            READ_BYTE,
                                            NULL))
                                            == NoErr) {

                        if (driveConfig.XferRate == TAPE_1Mbps) {

                            TapeExtension->SpeedChangeOK = TRUE;
                            TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
                            CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE\n"));

                        } else {

                            TapeExtension->SpeedChangeOK = FALSE;
                            TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
                            CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE\n"));

                        }

                    }

                } else if (retval == UnspRate) {

                    TapeExtension->SpeedChangeOK = FALSE;
                    TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE\n"));
                    retval = NoErr;

                }
            }
        }

        break;

    case WANGTEK:

        TapeExtension->DriveParms.SeekMode = SEEK_SKIP;

        if (!ReportFailed &&
            ((VendorId & ~VENDOR_MASK) == WANGTEK_QIC80)) {

            TapeExtension->SpeedChangeOK = TRUE;
            TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
            CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE\n"));

        } else {

            TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
            CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE\n"));

        }
        break;

    case CORE:

        if ((VendorId & ~VENDOR_MASK) == CORE_QIC80) {

            TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
            CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE\n"));

        } else {

            TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
            CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE\n"));

        }
        break;

    case IOMEGA:

        TapeExtension->DriveParms.SeekMode = SEEK_SKIP;
        TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
        TapeExtension->SpeedChangeOK = TRUE;
        CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE\n"));
        break;

    case ARCHIVE:

        TapeExtension->DriveParms.SeekMode = SEEK_SKIP;

        if ((retval = Q117iSetDriveMode(TapeExtension, DIAGNOSTIC_1_MODE)) !=
            NoErr) {

            return(retval);

        }

        retval = Q117iGetDriveError(TapeExtension);

        if (retval == NoErr || retval == NoTape) {

            if ((retval = Q117iSendByte(TapeExtension, Rpt_Archive_Native_Mode)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iReceiveByte(
                            TapeExtension,
                            READ_WORD,
                            (USHORT *)&TapeExtension->DriveParms.ArchiveNativeMode
                            )) == NoErr) {

                TapeExtension->SpeedChangeOK = TRUE;

                CheckedDump(QIC117INFO,( "Q117i: Archive Native Mode = %x)\n",
                    TapeExtension->DriveParms.ArchiveNativeMode));

                if ((TapeExtension->DriveParms.ArchiveNativeMode &
                        ARCHIVE_MODEL_XKEII) != 0) {

                    TapeExtension->DriveParms.SeekMode = SEEK_SKIP_EXTENDED;

                }

                if ((TapeExtension->DriveParms.ArchiveNativeMode &
                        ARCHIVE_20_TRACK) != 0) {

                    TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE (Archive Native Mode)\n"));

                }

                if ((TapeExtension->DriveParms.ArchiveNativeMode &
                        ARCHIVE_28_TRACK) != 0) {

                    TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
                    CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE (Archive Native Mode)\n"));

                }

                retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);

                if (retval != NoErr && retval != NoTape) {

                    return(retval);

                }

            }

        } else {

            if (retval == InvalCmd) {

                if ((retval = Q117iSendByte(TapeExtension, Soft_Reset)) == NoErr) {

                    Q117iSleep(TapeExtension, (mt_wt001s + mt_wt2ticks), FALSE);

                    if ((retval = Q117iSelectDrive(TapeExtension)) == NoErr) {

                        if ((retval = Q117iReport(
                                            TapeExtension,
                                            Report_Confg,
                                            (USHORT *)&driveConfig,
                                            READ_BYTE,
                                            NULL)) == NoErr) {

                            if (driveConfig.QIC80) {

                                TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
                                CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE (Archive Soft Reset)\n"));

                            } else {

                                TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
                                CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE (Archive Soft Reset)\n"));

                            }
                        }
                    }
                }

            } else {

                return(retval);

            }
        }


        break;

    default:

        if ((retval = Q117iSendByte(TapeExtension, Soft_Reset)) == NoErr) {

            Q117iSleep(TapeExtension, (mt_wt001s + mt_wt2ticks), FALSE);

            if ((retval = Q117iSelectDrive(TapeExtension)) == NoErr) {

                if ((retval = Q117iReport(
                                    TapeExtension,
                                    Report_Confg,
                                    (USHORT *)&driveConfig,
                                    READ_BYTE,
                                    NULL)) == NoErr) {

                    if (driveConfig.QIC80) {

                        TapeExtension->DriveParms.DriveType = QIC80_DRIVE;
                        CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC80_DRIVE\n"));

                    } else {

                        TapeExtension->DriveParms.DriveType = QIC40_DRIVE;
                        CheckedDump(QIC117INFO,( "Q117i: Drive Type QIC40_DRIVE\n"));

                    }
                }
            }
        }
    }

    return(retval);
}

VOID
q117iUpdateRegistryInfo(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    This function updates the devicemap.

Arguments:

    TapeExtension -

Return Value:

   Returns the status of the operation.

--*/

{
    HANDLE          lunKey;
    HANDLE          unitKey;
    UNICODE_STRING  ntUnicodeString;
    UNICODE_STRING  name;
    OBJECT_ATTRIBUTES objectAttributes;
    UNICODE_STRING string;
    UNICODE_STRING stringNum;
    WCHAR bufferNum[16];
    WCHAR buffer[64];
    NTSTATUS status;

    //
    // Create the Tape key in the device map.
    //

    RtlInitUnicodeString(
        &name,
        L"\\Registry\\Machine\\Hardware\\DeviceMap\\Tape"
        );

    //
    // Initialize the object for the key.
    //

    InitializeObjectAttributes( &objectAttributes,
                                &name,
                                OBJ_CASE_INSENSITIVE,
                                NULL,
                                (PSECURITY_DESCRIPTOR) NULL );

    //
    // Create the key or open it.
    //

    status = ZwOpenKey(&lunKey,
                        KEY_READ | KEY_WRITE,
                        &objectAttributes );

    if (!NT_SUCCESS(status)) {
        return;
    }

    //
    // Copy the Prefix into a string.
    //

    string.Length = 0;
    string.MaximumLength=64;
    string.Buffer = buffer;

    RtlInitUnicodeString(&stringNum,
        L"Unit ");

    RtlCopyUnicodeString(&string, &stringNum);

    //
    // Create a port number key entry.
    //

    stringNum.Length = 0;
    stringNum.MaximumLength = 16;
    stringNum.Buffer = bufferNum;

    CheckedDump(QIC117INFO,( "Q117i: Tape Device Number %d (decimal)\n", TapeExtension->TapeNumber));
    status = RtlIntegerToUnicodeString(TapeExtension->TapeNumber, 10, &stringNum);

    if (!NT_SUCCESS(status)) {
        return;
    }

    //
    // Append the prefix and the numeric name.
    //

    RtlAppendUnicodeStringToString(&string, &stringNum);

    InitializeObjectAttributes( &objectAttributes,
                                &string,
                                OBJ_CASE_INSENSITIVE,
                                lunKey,
                                (PSECURITY_DESCRIPTOR) NULL );

    status = ZwOpenKey(&unitKey,
                        KEY_READ | KEY_WRITE,
                        &objectAttributes );

    ZwClose(lunKey);

    switch (TapeExtension->DriveParms.Flavor) {

    case CMS:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Colorado Memory Systems Jumbo 250");

        } else {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Colorado Memory Systems Jumbo 120");

        }

        break;

    case SUMMIT:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Summit QIC-80 floppy tape drive");

        } else {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Summit QIC-40 floppy tape drive");

        }

        break;

    case WANGTEK:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Wangtek QIC-80 floppy tape drive");

        } else {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Wangtek QIC-40 floppy tape drive");

        }

        break;

    case CORE:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Core QIC-80 floppy tape drive");

        } else {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Core QIC-40 floppy tape drive");

        }

        break;

    case IOMEGA:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Iomega QIC-80 floppy tape drive");

        } else {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Iomega QIC-40 floppy tape drive");

        }

        break;

    case CMS_ENHANCEMENTS:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            RtlInitUnicodeString(&ntUnicodeString,
                L"CMS Enhancements QIC-80 floppy tape drive");

        } else {

            RtlInitUnicodeString(&ntUnicodeString,
                L"CMS Enhancements QIC-40 floppy tape drive");

        }

        break;

    case ARCHIVE:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Conner QIC-80 floppy tape drive");

        } else {

            RtlInitUnicodeString(&ntUnicodeString,
                L"Conner QIC-40 floppy tape drive");

        }

        break;

    default:

        RtlInitUnicodeString(&ntUnicodeString,
            L"QIC-40/QIC-80 floppy tape drive");
    }

    //
    // Add Identifier value.
    //

    RtlInitUnicodeString(&name, L"Identifier");

    status = ZwSetValueKey(
        unitKey,
        &name,
        0,
        REG_SZ,
        ntUnicodeString.Buffer,
        ntUnicodeString.Length
        );

    ZwClose(unitKey);

    return;

} // end q117iUpdateRegistryInfo

STATUS
Q117iNewTape(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Determine the format (if any) on a tape cartridge. This routine is
    executed whenever the driver detects that a new tape has been inserted
    into the tape drive.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval;
    SHORT writeProtect;

    //
    // if we have a qic80 of firmware below a certain value
    // (FIRM_VERSION_63), and if we have an
    // unreferenced tape, and a cartridge is in the drive,
    // then we re-try the reference burst. Only if the
    // retry fails do we return an error.
    //
    // The firmware should do the retry automatically, but at the moment
    // it does not. This will be corrected, and the retry not commanded,
    // at some time to be determined. At that time, it should only be
    // necessary to change the #define used to determine the firmware
    // rev level.  --  crc
    //
    // Check to see if the drive is a Jumbo B with firmware 63.  This
    // firmware has 2 bugs.  If the tape is put in slowly, it can incorrectly
    // return an invalid media error or write protect status.
    //

    if (TapeExtension->DriveParms.Version <= FIRM_VERSION_63 &&
        TapeExtension->DriveParms.Version >= FIRM_VERSION_60 &&
        TapeExtension->DriveParms.Flavor == CMS) {

        retval = Q117iGetDriveError(TapeExtension);

        if (retval == TapeFlt && TapeExtension->FirmwareError == Inval_Media) {

            //
            // Fix both the invalid media error by sending a NewTape command
            // to the drive.

            retval = Q117iClearTapeError(TapeExtension);

        } else {

            //
            // Read the write protect status directly from port 2 on
            // the Jumbo B processor to determine if the drive got a
            // bogus write protect error.
            //

            if (TapeExtension->DriveParms.Status.WriteProtect &&
                (retval = Q117iReadWrtProtect(TapeExtension, &writeProtect))
                == NoErr) {

                if (writeProtect == FALSE) {

                    retval = Q117iClearTapeError(TapeExtension);

                }
            }
        }

        if (retval != NoErr) {

            return(retval);

        }
    }

    retval = Q117iGetDriveError(TapeExtension);

    if (TapeExtension->DriveParms.Status.Referenced == FALSE &&
        TapeExtension->DriveParms.Status.CartPresent == TRUE &&
        TapeExtension->DriveParms.Flavor == CMS &&
        TapeExtension->DriveParms.Version >= FIRM_VERSION_60 &&
        TapeExtension->DriveParms.Version <= FIRM_VERSION_63)  {

        //
        // command: seek reference burst. N.b: Non-interruptible!
        //

        if ((retval = Q117iSendByte(TapeExtension, Seek_LP)) != NoErr) {

            return(retval);

        }

        //
        // Wait for the drive to become ready again.
        //

        if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt105s)) !=
            NoErr) {

            return(retval);

        }
    }

    //
    // Rewind tape -- needed for Archive drives
    //

    if ((TapeExtension->DriveParms.Status.CartPresent == TRUE) &&
        ((TapeExtension->DriveParms.Flavor == IOMEGA) ||
        (TapeExtension->DriveParms.Flavor == ARCHIVE))) {

        retval = Q117iGetDriveError(TapeExtension);

        if (TapeExtension->DriveParms.Status.BOT == FALSE) {

            if ((retval = Q117iSendByte(TapeExtension, Physical_Rev)) != NoErr) {

                return(retval);

            }

            //
            //  Wait for the drive to become ready again.
            //

            if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt460s)) !=
                NoErr)  {

                return(retval);

            }
        }

        if (TapeExtension->DriveParms.Status.Referenced == FALSE) {

            //
            //  command: seek reference burst. N.b: Non-interruptible!
            //

            if ((retval = Q117iSendByte(TapeExtension, Seek_LP)) != NoErr) {

                return(retval);

            }

            //
            //  Wait for the drive to become ready again.
            //

            if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt460s)) !=
                NoErr) {

                return(retval);

            }
        }
    }


    if ((retval = Q117iGetTapeParameters(TapeExtension)) != NoErr) {

        return(retval);

    }

    TapeExtension->TapePosition.C_Track = -1;
    TapeExtension->DriveParms.Mode = PRIMARY_MODE;
    TapeExtension->NewCart = FALSE;

    if  ((TapeExtension->TapeParms.TapeType == QIC80_SHORT ||
            TapeExtension->TapeParms.TapeType == QIC80_LONG ||
            TapeExtension->TapeParms.TapeType == QICEST_80) &&
            (TapeExtension->DriveParms.DriveType == QIC40_DRIVE)) {

        return(WrongFmt);

    }

    if (TapeExtension->XferRate.TapeFast == TAPE_2Mbps) {

        if  (TapeExtension->TapeParms.TapeType == QIC40_SHORT ||
             TapeExtension->TapeParms.TapeType == QIC40_LONG ||
             TapeExtension->TapeParms.TapeType == QICEST_40) {

            TapeExtension->XferRate.XferRate = SLOW;
            TapeExtension->XferRate.MaxRate = SLOW;
            TapeExtension->XferRate.TapeSlow = TAPE_500Kbps;
            TapeExtension->XferRate.FDC_Slow = FDC_500Kbps;
            TapeExtension->XferRate.SRT_Slow = SRT_500Kbps;
            retval = Q117iDFast_DSlow(TapeExtension, DSlow);

                } else {

            if (TapeExtension->TapeParms.TapeType == QIC80_SHORT ||
                TapeExtension->TapeParms.TapeType == QIC80_LONG ||
                TapeExtension->TapeParms.TapeType == QICEST_80) {

                TapeExtension->XferRate.XferRate = SLOW;
                retval = Q117iDFast_DSlow(TapeExtension, DSlow);

                        }
                }
        }

    if (TapeExtension->XferRate.TapeFast == TAPE_1Mbps) {

        if  (TapeExtension->TapeParms.TapeType == QIC40_SHORT ||
            TapeExtension->TapeParms.TapeType == QIC40_LONG ||
            TapeExtension->TapeParms.TapeType == QICEST_40) {

            TapeExtension->XferRate.XferRate = SLOW;
            retval = Q117iDFast_DSlow(TapeExtension, DSlow);

        } else {

            TapeExtension->XferRate.XferRate = TapeExtension->XferRate.MaxRate;

            if (TapeExtension->XferRate.XferRate == SLOW) {

                retval = Q117iDFast_DSlow(TapeExtension, DSlow);

            } else {

                retval = Q117iDFast_DSlow(TapeExtension, DFast);

            }
        }
    }

    return(retval);
}


STATUS
Q117iReadWrtProtect(
    IN PTAPE_EXTENSION TapeExtension,
    OUT SHORT *WriteProtect
    )

/*++

Routine Description:

    Reads the write protect status from the drive processor itself.

    This procedure is used due to the firmware 63 error where a tape put
    into the drive very slowly can cause the status byte to say (incorrectly)
    that the tape is write protected. It uses a Diagnostic command to read
    port 2 of the processor on the drive.

Arguments:

    TapeExtension -

    WriteProtect -

Return Value:



--*/

{
    STATUS retval;
    UCHAR port2Val;     // The port value from which the "real"
                        // write protect is read (bit 5)

    if ((retval = Q117iSetDriveMode(TapeExtension, DIAGNOSTIC_1_MODE)) ==
        NoErr) {

        if ((retval = Q117iReport(
                            TapeExtension,
                            Read_Port2,
                            (USHORT *)&port2Val,
                            READ_BYTE,
                            NULL)) == NoErr) {

            //
            // If bit 5 of port 2 on the return byte is 1, then the tape
            // is "really" write protected.
            //

            if (port2Val & WRITE_PROTECT_MASK) {

                *WriteProtect = TRUE;

            } else {

                *WriteProtect = FALSE;

            }

            retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);
        }
    }

    return(retval);
}


STATUS
Q117iGetTapeParameters(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Sets up the necessary tape capacity parameters in the driver according
    to the tape type (QIC40 or QIC80) and tape length (normal or extra length).

Arguments :

    TapeExtension -

Return Value:



--*/

{
    STATUS retval;
    CHAR orgMode;
    struct DriveConfiguration driveConfig;
    struct CmsStatus cmsStatus;
    struct TapeFormatLgth tapeFormatLength;
    BOOLEAN reportFailed = FALSE;

    if ((retval = Q117iReport(
                    TapeExtension,
                    Report_Confg,
                    (USHORT *)&driveConfig,
                    READ_BYTE,
                    NULL)) != NoErr) {

        return(retval);

    }

    /* Make a call to Report Tape Status */
    if ((retval = Q117iReport(
                    TapeExtension,
                    Report_Tape_Stat,
                    (USHORT *)&tapeFormatLength,
                    READ_BYTE,
                    NULL)) != NoErr) {

        Q117iGetDriveError(TapeExtension);
        retval = NoErr;
        reportFailed = TRUE;
        tapeFormatLength.Format = 0;
        tapeFormatLength.Length = 0;
    }

    if (!driveConfig.XL_Tape) {

        if (!driveConfig.QIC80) {

            if (!reportFailed && tapeFormatLength.Format == QIC_500) {

                CheckedDump(QIC117INFO,( "Q117i: Tape Type QIC500_SHORT\n"));
                TapeExtension->TapeParms.TapeType =             QIC500_SHORT;
                TapeExtension->TapeParms.FtrackFside =          FTK_FSD_500;
                TapeExtension->TapeParms.SegTtrack =            SEG_TTRK_500;
                TapeExtension->TapeParms.FsectFside =           FSC_FTK * FTK_FSD_500;
                TapeExtension->TapeParms.LogSectors =           FSC_SEG * SEG_TTRK_500 * NUM_TTRK_500;
                TapeExtension->TapeParms.FsectTtrack =          FSC_SEG * SEG_TTRK_500;
                TapeExtension->TapeParms.TimeOut[L_SLOW] =      mt_wt130s;
                TapeExtension->TapeParms.TimeOut[L_FAST] =      mt_wt065s;
                TapeExtension->TapeParms.TimeOut[PHYSICAL] =    mt_wt065s;

            } else {

                // Assume that the tape is a Standard length QIC40
                CheckedDump(QIC117INFO,( "Q117i: Tape Type QIC40_SHORT\n"));
                TapeExtension->TapeParms.TapeType =             QIC40_SHORT;
                TapeExtension->TapeParms.FtrackFside =          FTK_FSD_40;
                TapeExtension->TapeParms.SegTtrack =            SEG_TTRK_40;
                TapeExtension->TapeParms.FsectFside =           FSC_FTK * FTK_FSD_40;
                TapeExtension->TapeParms.LogSectors =           FSC_SEG * SEG_TTRK_40 * NUM_TTRK_40;
                TapeExtension->TapeParms.FsectTtrack =          FSC_SEG * SEG_TTRK_40;
                TapeExtension->TapeParms.TimeOut[L_SLOW] =      mt_wt130s;
                TapeExtension->TapeParms.TimeOut[L_FAST] =      mt_wt065s;
                TapeExtension->TapeParms.TimeOut[PHYSICAL] =    mt_wt065s;

            }


        } else {

            CheckedDump(QIC117INFO,( "Q117i: Tape Type QIC80_SHORT\n"));
            TapeExtension->TapeParms.TapeType = QIC80_SHORT;
            TapeExtension->TapeParms.FtrackFside = FTK_FSD_80;
            TapeExtension->TapeParms.SegTtrack = SEG_TTRK_80;
            TapeExtension->TapeParms.FsectFside = FSC_FTK * FTK_FSD_80;
            TapeExtension->TapeParms.LogSectors = (ULONG)FSC_SEG *
                                                    (ULONG)SEG_TTRK_80 *
                                                    (ULONG)NUM_TTRK_80;
            TapeExtension->TapeParms.FsectTtrack = FSC_SEG * SEG_TTRK_80;
            TapeExtension->TapeParms.TimeOut[L_SLOW] =   mt_wt100s;
            TapeExtension->TapeParms.TimeOut[L_FAST] =   mt_wt050s;
            TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt050s;

        }

    } else {


        if (!driveConfig.QIC80) {

            if (!reportFailed) {

                switch (tapeFormatLength.Length) {

                case (QICEST_900):

                    CheckedDump(QIC117INFO,( "Q117i: Tape Type QICEST_500\n"));
                    TapeExtension->TapeParms.TapeType = QICEST_500;
                    TapeExtension->TapeParms.FtrackFside = FTK_FSD_QICEST_500;
                    TapeExtension->TapeParms.SegTtrack = SEG_TTRK_500;
                    TapeExtension->TapeParms.FsectFside = FSC_FTK * FTK_FSD_QICEST_500;
                    TapeExtension->TapeParms.LogSectors = FSC_SEG * SEG_TTRK_500 * NUM_TTRK_500;
                    TapeExtension->TapeParms.FsectTtrack = FSC_SEG * SEG_TTRK_500;
                    TapeExtension->TapeParms.TimeOut[L_SLOW] = mt_wt700s;
                    TapeExtension->TapeParms.TimeOut[L_FAST] = mt_wt350s;
                    TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt350s;
                    break;

                case (QICEST):

                    CheckedDump(QIC117INFO,( "Q117i: Tape Type QICEST_40\n"));
                    TapeExtension->TapeParms.TapeType = QICEST_40;
                    TapeExtension->TapeParms.FtrackFside = FTK_FSD_QICEST_40;
                    TapeExtension->TapeParms.SegTtrack = SEG_TTRK_QICEST_40;
                    TapeExtension->TapeParms.FsectFside = FSC_FTK * FTK_FSD_QICEST_40;
                    TapeExtension->TapeParms.LogSectors = (ULONG)FSC_SEG *
                                                            (ULONG)SEG_TTRK_QICEST_40 *
                                                            (ULONG)NUM_TTRK_40;
                    TapeExtension->TapeParms.FsectTtrack = FSC_SEG * SEG_TTRK_QICEST_40;
                    TapeExtension->TapeParms.TimeOut[L_SLOW] =   mt_wt700s;
                    TapeExtension->TapeParms.TimeOut[L_FAST] =   mt_wt350s;
                    TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt350s;
                    break;

                case (QIC_LONG):

                    CheckedDump(QIC117INFO,( "Q117i: Tape Type QIC40_LONG\n"));
                    TapeExtension->TapeParms.TapeType = QIC40_LONG;
                    TapeExtension->TapeParms.FtrackFside = FTK_FSD_40L;
                    TapeExtension->TapeParms.SegTtrack = SEG_TTRK_40L;
                    TapeExtension->TapeParms.FsectFside = FSC_FTK * FTK_FSD_40L;
                    TapeExtension->TapeParms.LogSectors = FSC_SEG * SEG_TTRK_40L * NUM_TTRK_40;
                    TapeExtension->TapeParms.FsectTtrack = FSC_SEG * SEG_TTRK_40L;
                    TapeExtension->TapeParms.TimeOut[L_SLOW] =   mt_wt200s;
                    TapeExtension->TapeParms.TimeOut[L_FAST] =   mt_wt100s;
                    TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt100s;
                    break;
                }

            } else {

                // Assume that the tape type is QIC40_Long
                CheckedDump(QIC117INFO,( "Q117i: Tape Type QIC40_LONG\n"));
                TapeExtension->TapeParms.TapeType = QIC40_LONG;
                TapeExtension->TapeParms.FtrackFside = FTK_FSD_40L;
                TapeExtension->TapeParms.SegTtrack = SEG_TTRK_40L;
                TapeExtension->TapeParms.FsectFside =
                    FSC_FTK * FTK_FSD_40L;
                TapeExtension->TapeParms.LogSectors =
                    FSC_SEG * SEG_TTRK_40L * NUM_TTRK_40;
                TapeExtension->TapeParms.FsectTtrack =
                    FSC_SEG * SEG_TTRK_40L;
                TapeExtension->TapeParms.TimeOut[L_SLOW] =   mt_wt200s;
                TapeExtension->TapeParms.TimeOut[L_FAST] =   mt_wt100s;
                TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt100s;
            }

        } else {

            // drive_config.QIC80 == TRUE

            if (!reportFailed) {

                switch (tapeFormatLength.Length) {

                case (QICEST):

                    CheckedDump(QIC117INFO,( "Q117i: Tape Type QICEST_80\n"));
                    TapeExtension->TapeParms.TapeType = QICEST_80;
                    TapeExtension->TapeParms.FtrackFside = FTK_FSD_QICEST_80;
                    TapeExtension->TapeParms.SegTtrack = SEG_TTRK_QICEST_80;
                    TapeExtension->TapeParms.FsectFside = FSC_FTK * FTK_FSD_QICEST_80;
                    TapeExtension->TapeParms.LogSectors = (ULONG)FSC_SEG *
                                        (ULONG)SEG_TTRK_QICEST_80 *
                                        (ULONG)NUM_TTRK_80;
                    TapeExtension->TapeParms.FsectTtrack = FSC_SEG * SEG_TTRK_QICEST_80;
                    TapeExtension->TapeParms.TimeOut[L_SLOW] =   mt_wt475s;
                    TapeExtension->TapeParms.TimeOut[L_FAST] =   mt_wt250s;
                    TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt250s;
                    break;

                case (QIC_LONG):

                    CheckedDump(QIC117INFO,( "Q117i: Tape Type QIC80_LONG\n"));
                    TapeExtension->TapeParms.TapeType = QIC80_LONG;
                    TapeExtension->TapeParms.FtrackFside = FTK_FSD_80L;
                    TapeExtension->TapeParms.SegTtrack = SEG_TTRK_80L;
                    TapeExtension->TapeParms.FsectFside = FSC_FTK * FTK_FSD_80L;
                    TapeExtension->TapeParms.LogSectors = (ULONG)FSC_SEG *
                                        (ULONG)SEG_TTRK_80L *
                                        (ULONG)NUM_TTRK_80;
                    TapeExtension->TapeParms.FsectTtrack = FSC_SEG * SEG_TTRK_80L;
                    TapeExtension->TapeParms.TimeOut[L_SLOW] =   mt_wt130s;
                    TapeExtension->TapeParms.TimeOut[L_FAST] =   mt_wt065s;
                    TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt065s;
                    break;

                }

            } else {

                // Assume it is a QIC80_LONG

                CheckedDump(QIC117INFO,( "Q117i: Tape Type QIC80_LONG\n"));
                TapeExtension->TapeParms.TapeType = QIC80_LONG;
                TapeExtension->TapeParms.FtrackFside = FTK_FSD_80L;
                TapeExtension->TapeParms.SegTtrack = SEG_TTRK_80L;
                TapeExtension->TapeParms.FsectFside = FSC_FTK * FTK_FSD_80L;
                TapeExtension->TapeParms.LogSectors = (ULONG)FSC_SEG *
                                        (ULONG)SEG_TTRK_80L *
                                        (ULONG)NUM_TTRK_80;
                TapeExtension->TapeParms.FsectTtrack = FSC_SEG * SEG_TTRK_80L;
                TapeExtension->TapeParms.TimeOut[L_SLOW] =   mt_wt130s;
                TapeExtension->TapeParms.TimeOut[L_FAST] =   mt_wt065s;
                TapeExtension->TapeParms.TimeOut[PHYSICAL] = mt_wt065s;

            }
        }
    }

    TapeExtension->TapeParms.FsectSeg = FSC_SEG;
    TapeExtension->TapeParms.SegFtrack = SEG_FTK;
    TapeExtension->TapeParms.FsectFtrack = FSC_FTK;
    TapeExtension->TapeParms.RwGapLength = WRT_GPL;


    if ((TapeExtension->DriveParms.Flavor == CMS) &&
        ((TapeExtension->DriveParms.Version >= FIRM_VERSION_60) &&
        (TapeExtension->DriveParms.Version < FIRM_VERSION_87 ))) {

        if (driveConfig.XL_Tape) {

            if ((retval = Q117iSetDriveMode(TapeExtension,
                                            DIAGNOSTIC_1_MODE)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iReport(
                            TapeExtension,
                            Rpt_CMS_Status,
                            (USHORT *)&cmsStatus,
                            READ_BYTE,
                            NULL)) != NoErr) {

                return(retval);

            } else {

                if (cmsStatus.Pegasus) {

                    tapeFormatLength.Length = QICEST;

                }


            }

            //
            // Put drive back into its original mode.
            //

            if ((retval = Q117iSetDriveMode(TapeExtension,
                                            PRIMARY_MODE)) != NoErr) {

                return(retval);

            }


        }

    }

    // Determine the Tape Format Code

    if (tapeFormatLength.Length == QICEST ||
        tapeFormatLength.Length == QICEST_900) {

        CheckedDump(QIC117INFO,( "Q117i: Tape Format Code QICEST_FORMAT\n"));
        TapeExtension->TapeParms.TapeFormatCode = QICEST_FORMAT;

        if ( TapeExtension->DriveParms.Flavor == WANGTEK) {

            TapeExtension->DriveParms.SeekMode = SEEK_SKIP_EXTENDED;

        }

        if (!TapeExtension->PegasusSupported) {

            retval = TapeFlt;

        } else {

            if ( (TapeExtension->DriveParms.Flavor == CMS) &&
                (TapeExtension->TapeParms.TapeType == QICEST_40) ) {

                retval = TapeFlt;

            }

        }
    } else {

        CheckedDump(QIC117INFO,( "Q117i: Tape Format Code QIC_FORMAT\n"));
        TapeExtension->TapeParms.TapeFormatCode = QIC_FORMAT;

    }

    Q117iCalcFmtSegmentsAndTracks( TapeExtension );

    return(retval);
}


VOID
Q117iCalcFmtSegmentsAndTracks(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Calculate the number of formattable segments given the current tape and
    drive type, and the number of tracks.

Arguments:

    TapeExtension -

    FormattableSegments -

    Tracks -

Return Value:

    None

--*/

{
    switch (TapeExtension->DriveParms.DriveType) {

        case (QIC80_DRIVE):

        //
        //Choose the segments per tape track value according to the length
        // of the tape.
        //

        switch (TapeExtension->TapeParms.TapeType) {

                case (QIC40_SHORT):
                case (QIC80_SHORT):

                        TapeExtension->TapeParms.FormattableSegs = (USHORT)SEG_TTRK_80;
                        break;

                case (QIC40_LONG):
                case (QIC80_LONG):

                        TapeExtension->TapeParms.FormattableSegs = (USHORT)SEG_TTRK_80L;
                        break;

                case (QICEST_40):
                case (QICEST_80):

                        TapeExtension->TapeParms.FormattableSegs = (USHORT)SEG_TTRK_QICEST_80;
                        break;

                }

        if ((TapeExtension->NumTracks > NUM_TTRK_80) ||
            (TapeExtension->NumTracks == 0)) {

            TapeExtension->TapeParms.FormattableSegs *= (USHORT)NUM_TTRK_80;
            TapeExtension->TapeParms.NumTtrack = (USHORT)NUM_TTRK_80;

        } else {

            TapeExtension->TapeParms.FormattableSegs *= (USHORT)TapeExtension->NumTracks;
            TapeExtension->TapeParms.NumTtrack = (USHORT)TapeExtension->NumTracks;

        }
                break;

        case (QIC40_DRIVE):

        //
        // Since a QIC40 drive can not detect a QIC80 formatted tape,
        // the seg_ttrack field in Q117itape_parms is correct.
        //

        TapeExtension->TapeParms.FormattableSegs = (USHORT)TapeExtension->TapeParms.SegTtrack;

        if ((TapeExtension->NumTracks == 0) ||
            (TapeExtension->NumTracks > NUM_TTRK_40)) {

            TapeExtension->TapeParms.FormattableSegs *= (USHORT)NUM_TTRK_40;
            TapeExtension->TapeParms.NumTtrack = (USHORT)NUM_TTRK_40;

        } else {

            TapeExtension->TapeParms.FormattableSegs *= (USHORT)TapeExtension->NumTracks;
            TapeExtension->TapeParms.NumTtrack = (USHORT)TapeExtension->NumTracks;

        }
                break;

    case (QIC500_DRIVE):

        if (TapeExtension->TapeParms.TapeType == QICEST_500 ||
            TapeExtension->TapeParms.TapeType == QIC500_SHORT) {

            TapeExtension->TapeParms.FormattableSegs = (USHORT)TapeExtension->TapeParms.SegTtrack;

        } else {

            // A QIC40 or a QIC80 tape was detected in a QIC500_DRIVE drive

            TapeExtension->TapeParms.FormattableSegs = (USHORT)0;

        }

        if ((TapeExtension->NumTracks == 0) ||
            (TapeExtension->NumTracks > NUM_TTRK_500)) {

            TapeExtension->TapeParms.FormattableSegs *= (USHORT)NUM_TTRK_500;
            TapeExtension->TapeParms.NumTtrack = (USHORT)NUM_TTRK_500;

        } else {

            TapeExtension->TapeParms.FormattableSegs *= (USHORT)TapeExtension->NumTracks;
            TapeExtension->TapeParms.NumTtrack = (USHORT)TapeExtension->NumTracks;

        }
        break;

    }

}


STATUS
Q117iMtnPreamble(
    IN PTAPE_EXTENSION TapeExtension,
    IN BOOLEAN Select
    )

/*++

Routine Description:


Arguments:

    TapeExtension -

Return Value:

    None

--*/

{
    STATUS retval;

    if (Select) {

        if ((retval = Q117iSendByte(TapeExtension, Mtn_Select_1)) == NoErr) {

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);
            retval = Q117iSendByte(TapeExtension, Mtn_Select_2);
            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

        }

    } else {

        retval = Q117iSendByte(TapeExtension, Mtn_Deselect);
        Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

    }

    return(retval);
}


STATUS
Q117iGetDriveInfo(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Gets the following Drive information:

        1) Drive Type/Model
        2) Firmware Revision
        3) OEM Field Flag
        4) OEM Field
        5) Serial Number
        6) Date of manufacture

        The OEM, Serial Number and the Date of manufacture are miscellaneous
        drive train information that is embedded in the drive's firmware.

    If the drive type is not CMS and/or the firmware revision is pre-80
    then zeros are returned in these fields. The drive type is obtained by
    making a call to Rpt_Cms_Status. This has been done to support the Jumbo
    B platform.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    struct CmsStatus cmsStatus;
    CHAR currentMode;
    UCHAR bitBucket;
    SHORT i;
    STATUS retval;

    //
    // Initialize the Drive Train Miscellaneous Information value to NULL
    //

    TapeExtension->MiscDriveInfo.InfoExists        = FALSE;
    TapeExtension->MiscDriveInfo.SerialNumber[0]   = '\0';
    TapeExtension->MiscDriveInfo.ManDate[0]        = '\0';
    TapeExtension->MiscDriveInfo.ManDate[1]        = '\0';
    TapeExtension->MiscDriveInfo.Oem[0]             = '\0';

    //
    // Get the firmware revision number and store it in
    // TapeExtension->MiscDriveInfo.ROM_Version
    //

    TapeExtension->MiscDriveInfo.ROM_Version =
        TapeExtension->DriveParms.Version;

    if (TapeExtension->DriveParms.Flavor == CMS) {

        if (TapeExtension->MiscDriveInfo.ROM_Version >=
            FIRM_VERSION_60) {

            //
            // Save the current drive mode
            //

            currentMode = TapeExtension->DriveParms.Mode;
            retval = Q117iSetDriveMode(TapeExtension, DIAGNOSTIC_1_MODE);

            //
            // Since we are only looking at the firmware, ignore all
            // NoTape errors.
            //

            if (retval != NoErr && retval != NoTape) {

                return(retval);

            }

            //
            // In order to support the Jumbo-B platform, the drive type
            // is gotten from cmsStatus and the drive type obtained from
            // the drive train info is thrown away.
            //

            //
            // Get the Drive Type and store it in
            // TapeExtension->MiscDriveInfo.DriveType
            //

            retval = Q117iReport(
                            TapeExtension,
                            Rpt_CMS_Status,
                            (USHORT *)&cmsStatus,
                            READ_BYTE,
                            NULL);

            //
            // Since we are only looking at the firmware, ignore all
            // NoTape errors.
            //

            if (retval != NoErr && retval != NoTape) {

                return(retval);

            }

            TapeExtension->MiscDriveInfo.DriveType =
                (CHAR)cmsStatus.DriveType;

        } else {

            TapeExtension->MiscDriveInfo.DriveType = -1;

        }

        if (TapeExtension->MiscDriveInfo.ROM_Version >= FIRM_VERSION_80) {

            //
            // Send the Get Drive Training Information Command to the Drive.
            //

            retval = Q117iSendByte(TapeExtension, Dtrain_Info);

            //
            // Since we are only looking at the firmware, ignore all
            // NoTape errors.
            //

            if (retval != NoErr && retval != NoTape) {

                return(retval);

            }

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

            //
            // Send the Get Descriptive Info Command to the Drive.
            //

            retval = Q117iSendByte(TapeExtension, Gdesp_Info);

            //
            // Since we are only looking at the firmware, ignore all
            // NoTape errors.
            //

            if (retval != NoErr && retval != NoTape) {

                return(retval);

            }

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

            //
            // Set the info_exists flag to true.
            //

            TapeExtension->MiscDriveInfo.InfoExists = TRUE;

            //
            // Get the Drive Type and through it in the bit_bucket --
            // just to keep the data in sync.
            //

            retval = Q117iReport(
                            TapeExtension,
                            Read_Ram,
                            (USHORT *)&bitBucket,
                            READ_BYTE,
                            NULL);

            //
            // Since we are only looking at the firmware, ignore all
            // NoTape errors.
            //

            if (retval != NoErr && retval != NoTape) {

                return(retval);

            }

            //
            // Get the Serial Number from the drive and store it in
            // TapeExtension->MiscDriveInfo.SerialNumber[]
            //

            for (i=0; i<SERIAL_NUM_LENGTH; ++i) {

                retval = Q117iReport(
                            TapeExtension,
                            Read_Ram,
                            (USHORT *)&TapeExtension->MiscDriveInfo.SerialNumber[i],
                            READ_BYTE,
                            NULL);

                //
                // Since we are only looking at the firmware, ignore
                // all NoTape errors.
                //

                if (retval != NoErr && retval != NoTape) {

                    return(retval);

                }
            }

            //
            // Get the Manufacturing date from the drive and store it in
            // TapeExtension->MiscDriveInfo.ManDate[]
            //

            for (i=0; i<MAN_DATE_LENGTH; ++i) {

                retval = Q117iReport(
                            TapeExtension,
                            Read_Ram,
                            (USHORT *)&TapeExtension->MiscDriveInfo.ManDate[i],
                            READ_BYTE,
                            NULL);

                //
                // Since we are only looking at the firmware, ignore
                // all NoTape errors.
                //

                if (retval != NoErr && retval != NoTape) {

                    return(retval);

                }

            }

            //
            // Get the OEM field from the drive and store it in
            // TapeExtension->MiscDriveInfo.Oem[]
            //

            for (i=0; i<OEM_LENGTH; ++i) {

                retval = Q117iReport(
                            TapeExtension,
                            Read_Ram,
                            (USHORT *)&TapeExtension->MiscDriveInfo.Oem[i],
                            READ_BYTE,
                            NULL);

                //
                // Since we are only looking at the firmware, ignore
                // all NoTape errors.
                //

                if (retval != NoErr && retval != NoTape) {

                    return(retval);

                }

                if (TapeExtension->MiscDriveInfo.Oem[i] == '\0') {

                    break;

                }
            }
        }
    }

    if ((TapeExtension->DriveParms.Flavor == CMS) &&
        (TapeExtension->MiscDriveInfo.ROM_Version >= FIRM_VERSION_60)) {

        //
        // Exit the Diagnostics Mode by entering the Primary Mode
        //

        retval = Q117iSetDriveMode(TapeExtension, PRIMARY_MODE);

        //
        // Since we are only looking at the firmware, ignore all NoTape errors
        //

        if (retval != NoErr && retval != NoTape) {

            return(retval);

        }

        //
        // Restore drive to original mode (current_mode)
        //

        if (currentMode != PRIMARY_MODE){

            retval = Q117iSetDriveMode(TapeExtension, currentMode);

            //
            // Since we are only looking at the firmware, ignore all NoTape errors
            //

            if (retval != NoErr && retval != NoTape) {

                return(retval);

            }
        }
    }

    return(NoErr);
}
