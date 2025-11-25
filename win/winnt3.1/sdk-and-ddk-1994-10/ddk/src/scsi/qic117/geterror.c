/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    geterror.c

Abstract:

    Converts QIC117 firmware errors.

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
Q117iConvertFirmwareError(
    IN FIRMWARE_ERROR FirmwareError
    )

/*++

Routine Description:

Arguments:

    FirmwareError -

Return Value:

--*/

{
    CheckedDump(QIC117INFO,( "Q117i - Firmware error %d (decimal)\n", FirmwareError ));

    switch (FirmwareError) {

    case 0:                  // 0 - no error
        return(NoErr);
        break;

    case 1:                 // 1 - command receive while drive not ready
        return(InvalCmd);
        break;

    case 2:                 // 2 - cartridge not present or removed
        return(NoTape);
        break;

    case 3:                 // 3 - motor speed error
    case 4:                 // 4 - motor speed anomaly (motor jammed or gross speed error)
        return(TapeFlt);
        break;

    case 5:                 // 5 - cartridge write protected
        return(WProt);
        break;

    case 6:                 // 6 - undefined or reserved command code
        return(InvalCmd);
        break;

    case 7:                 // 7 - illegal track address specified for Seek
        CheckedDump(QIC117WARN,( "SeekErr - illegal track address specified for Seek\n" ));
        return(SeekErr);
        break;

    case 8:                 // 8 - illegal command in report subcontext
    case 9:                 // 9 - illegal entry attempt into a diagnostic mode
        return(InvalCmd);
        break;

    case 10:                // 10 - broken tape detected (based on hole and motor sensors)
    case 11:                // 11 - Warning -- read gain setting error
        return(TapeFlt);
        break;

    case 12:                // 12 - command Received while error status pending
    case 13:                // 13 - command received while new cartridge pending
    case 14:                // 14 - command illegal or undefined in primary mode
    case 15:                // 15 - command illegal or undefined in format mode
    case 16:                // 16 - command illegal or undefined in verify mode
        return(InvalCmd);
        break;

    case 17:                // 17 - logical forward not a logical BOT in format mode
        CheckedDump(QIC117WARN,( "SeekErr - logical forward not a logical BOT in format mode\n" ));
        return(SeekErr);
        break;

    case 18:                // 18 - logical EOT before all segments generated -- format mode
        return(EndTapeErr);
        break;

    case 19:                // 19 - command illegal when cartridge not referenced
        return(InvalCmd);
        break;

    case 20:                // 20 - self-diagnostic failed -- NOTE: this error CANNOT BE CLEARED
    case 21:                // 21 - Warning -- EEPROM not initialized default settings in effect
    case 22:                // 22 - EEPROM contents corrupted or EEPROM hardware malfunction
    case 23:                // 23 - tape motion timeout (max EOT-to-BOT time exceeded)
    case 24:                // 24 - data segment too long -- logical forward or pause
    case 25:                // 25 - command transmit overrun (usually a firmware bug)
    case 26:                // 26 - power on reset occurred
    case 27:                // 27 - software initiated reset occurred
    case 28:                // 28 - model-dependant diagnostic error
    case 29:                // 29 - model-independant diagnostic error
        return(TapeFlt);
        break;

    case 30:                // 30 - command received during non-interruptible process
        return(InvalCmd);
        break;

    case 31:                // 31 - speed selection requested is not available on this drive
        return(UnspRate);
        break;

    case 32:                // 32 - command illegal while in high speed mode
        return(InvalCmd);
        break;

    case 33:                // 33 - illegal segment specified for Seek
        CheckedDump(QIC117WARN,( "SeekErr - illegal segment specified for Seek\n" ));
        return(SeekErr);
        break;

    case 34:                // 34 - invalid tape media
    case 35:                // 35 - head reference failure
    case 36:                // 36 - edge seek error
    case 37:                // 37 - Missing Training Table
    case 38:                // 38 - Invalid Format
    case 39:                // 39 - EOT/BOT Sensor Failure
    case 40:                // 40 - Training Table Checksum Error
    case 41:                // 41 - Watchdog Timer Reset Occurred
    case 42:                // 42 - Illegal Error Number
        return(TapeFlt);
        break;

    default:
        return(TapeFlt);

    }
}


STATUS
Q117iGetDriveError(
    IN PTAPE_EXTENSION TapeExtension
    )
/*++

Routine Description:

    Read the tape drive Status byte and, if necessary, the tape drive
    Error information.

        Read the Drive Status byte from the tape drive.

        If the drive status indicates that the tape drive has an error to
        report, read the error information which includes both the error
        code and the command that was being executed when the error occurred.

Arguments:

    TapeExtension -

Return Value:

--*/

{
    STATUS retval = 0;
    struct DriveStatus drvStat;
    USHORT drvErr;
    BOOLEAN repeatReport;
    BOOLEAN newCart;
    BOOLEAN ESD_Retry = FALSE;

    TapeExtension->FirmwareError = NoErr;
    newCart = FALSE;

    do {

    repeatReport = FALSE;

        if ((retval = Q117iReport(TapeExtension,
                                Report_Status,
                                (USHORT *)&drvStat,
                                READ_BYTE,
                                &ESD_Retry)) != NoErr) {

            return(retval);

        }

        CheckedDump(QIC117DRVSTAT,( "QIC117: Drv status = %02x\n",drvStat ));

        TapeExtension->DriveParms.Status = drvStat;

        if (!drvStat.Ready) {

            return(NotRdy);

        }

        if (!drvStat.CartPresent) {

            TapeExtension->NoCart = TRUE;
            TapeExtension->PersistentNewCart = FALSE;

        }

        if (TapeExtension->DriveParms.Flavor == IOMEGA) {

            if (drvStat.CartPresent &&
                TapeExtension->NoCart) {

                newCart = TRUE;
                TapeExtension->NewCart = TRUE;
                TapeExtension->NoCart = FALSE;

            }

        }

        if (drvStat.NewCart) {

            newCart = TRUE;
            TapeExtension->NewCart = TRUE;

        }

        if (drvStat.NewCart || drvStat.Error) {

            if((retval = Q117iReport(TapeExtension,
                                    Report_Error,
                                    &drvErr,
                                    READ_WORD,
                                    &ESD_Retry)) != NoErr) {

                return(retval);

            }

            CheckedDump(QIC117DRVSTAT,( "QIC117: Drv error  = 0x%02x\n",drvErr ));

            if (drvStat.Error) {

                TapeExtension->FirmwareError = (UCHAR)drvErr;

                if (TapeExtension->FirmwareError == Cmd_on_New_Cart) {

                    newCart = TRUE;
                    TapeExtension->NewCart = TRUE;

                }

            } else {

                TapeExtension->FirmwareError = NoErr;

            }

            if (TapeExtension->FirmwareError) {

                if ((TapeExtension->FirmwareError == Cmd_in_Rpt) &&
                    ESD_Retry) {

                    ESD_Retry = FALSE;
                    repeatReport = TRUE;

                } else {

                    return(Q117iConvertFirmwareError(
                                TapeExtension->FirmwareError));

                }
            }
        }

    } while (repeatReport);

    if (newCart) {

        TapeExtension->PersistentNewCart = TRUE;

    }


	 if (TapeExtension->Found) {

        if (!drvStat.CartPresent) {

            return(NoTape);

        }

        if (newCart) {

            return(NewCart);

        }
    }

    return(retval);
}


STATUS
Q117iReport(
    IN OUT PTAPE_EXTENSION TapeExtension,
    IN CHAR Command,
    IN USHORT *ReportData,
    IN SHORT ReportSize,
    IN OUT CHAR *EsdRetry
    )

/*++

Routine Description:

    Send a report command to the tape drive and get the response data.  If
    a communication failure occurs, then we assume that it is a result of
    an ESD hit and retry the communication.

Arguments:

TapeExtension -

Command -

ReportData -

ReportSize -

EsdRetry -

Return Value:

--*/

{
    SHORT i = 0;
    STATUS ret;
    STATUS ret1 = NoErr;

    do {

        if (TapeExtension->QControllerData->EndFormatMode) {

            TapeExtension->QControllerData->EndFormatMode = FALSE;
            ret = NoErr;

        } else {

            ret = Q117iSendByte(TapeExtension, Command);

        }

        if (ret == NoErr) {

            if ((ret = Q117iReceiveByte(TapeExtension, ReportSize, ReportData))
                != NoErr) {

                if ((ret == DriveFlt) ||
                (TapeExtension->FirmwareError == Xmit_Overrun)) {

                    if (EsdRetry != NULL) {

                        *EsdRetry = TRUE;
                        ret1 = Q117iSelectDrive(TapeExtension);

                    }
                }
            }
        }

    } while (++i < REPORT_RPT && ret != NoErr && ret1 == NoErr);

    if (ret1 != NoErr) {

        return(ret1);

    } else {

        return(ret);

    }
}
