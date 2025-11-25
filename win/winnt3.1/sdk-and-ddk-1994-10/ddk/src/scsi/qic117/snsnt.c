/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    snsnt.c

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
Q117iSenseSpeed(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Sense the transfer rate of the FDC.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    UCHAR  fdcType;                         // flag to indicate an 82077 FDC
    struct DriveConfiguration driveConfig;
    STATUS retval;                           // return value
    SHORT statLength;
#if DBG

    UBYTE rate;

#endif

    Q117iResetFDC(TapeExtension);
    if ((retval = Q117iDGetFDC(TapeExtension, &fdcType)) != NoErr) {

        return(retval);

    }

    if ((retval = Q117iReport(
                    TapeExtension,
                    Report_Confg,
                    (USHORT *)&driveConfig,
                    READ_BYTE,
                    NULL)) != NoErr) {

        return(retval);

    }

    if (TapeExtension->DriveParms.DriveType == QIC40_DRIVE) {

        TapeExtension->XferRate.XferRate = (UBYTE) FAST;
        TapeExtension->XferRate.MaxRate  = (UBYTE) FAST;
        TapeExtension->XferRate.TapeSlow = (UBYTE) TAPE_250Kbps;
        TapeExtension->XferRate.TapeFast = (UBYTE) TAPE_500Kbps;
        TapeExtension->XferRate.FDC_Slow  = (UBYTE) FDC_250Kbps;
        TapeExtension->XferRate.FDC_Fast  = (UBYTE) FDC_500Kbps;
        TapeExtension->XferRate.SRT_Slow  = (UBYTE) SRT_250Kbps;
        TapeExtension->XferRate.SRT_Fast  = (UBYTE) SRT_500Kbps;

    } else {

        TapeExtension->XferRate.XferRate = (UBYTE) SLOW;
        TapeExtension->XferRate.MaxRate  = (UBYTE) SLOW;
        TapeExtension->XferRate.TapeSlow = (UBYTE) TAPE_500Kbps;
        TapeExtension->XferRate.TapeFast = (UBYTE) TAPE_1Mbps;
        TapeExtension->XferRate.FDC_Slow  = (UBYTE) FDC_500Kbps;
        TapeExtension->XferRate.FDC_Fast  = (UBYTE) FDC_1Mbps;
        TapeExtension->XferRate.SRT_Slow  = (UBYTE) SRT_500Kbps;
        TapeExtension->XferRate.SRT_Fast  = (UBYTE) SRT_1Mbps;

    }

    switch (TapeExtension->DriveParms.Flavor) {

    case CMS:
    case IOMEGA:
    case SUMMIT:
    case WANGTEK:

        if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

            if (fdcType == FDC_82077 ||
                    fdcType == FDC_82077AA ||
                    fdcType == FDC_82078_44 ||
                    fdcType == FDC_82078_64 ||
                    fdcType == FDC_NATIONAL) {

                CheckedDump(QIC117INFO,( "Q117i: sns spd megabit fdc\n"));

                TapeExtension->XferRate.XferRate = (UBYTE) FAST;
                TapeExtension->XferRate.MaxRate  = (UBYTE) FAST;

            }

        }

        break;

    case ARCHIVE:

        if (TapeExtension->DriveParms.ArchiveNativeMode) {

            if (TapeExtension->DriveParms.DriveType == QIC80_DRIVE) {

                if ((fdcType == FDC_82077 ||
                    fdcType == FDC_82077AA ||
                    fdcType == FDC_82078_44 ||
                    fdcType == FDC_82078_64 ||
                    fdcType == FDC_NATIONAL) &&
                    ((TapeExtension->DriveParms.ArchiveNativeMode &
                    ARCHIVE_1MB_XFER) != 0)) {

                        TapeExtension->XferRate.XferRate = (UBYTE) FAST;
                        TapeExtension->XferRate.MaxRate  = (UBYTE) FAST;

                }

            }

        } else {

            switch (driveConfig.XferRate) {

            case TAPE_1Mbps:

                if ((fdcType == FDC_82077 ||
                    fdcType == FDC_82077AA ||
                    fdcType == FDC_82078_44 ||
                    fdcType == FDC_82078_64 ||
                    fdcType == FDC_NATIONAL) &&
                    (TapeExtension->DriveParms.DriveType == QIC80_DRIVE)) {

                    TapeExtension->XferRate.XferRate = (UBYTE) FAST;
                    TapeExtension->XferRate.MaxRate  = (UBYTE) FAST;

                } else {

                    CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = UNSUPPORTED_RATE\n"));
                    return(UnspRate);

                }

            case TAPE_500Kbps:

                if (TapeExtension->DriveParms.DriveType == QIC40_DRIVE) {

                    TapeExtension->XferRate.XferRate = (UBYTE) FAST;
                    TapeExtension->XferRate.MaxRate = (UBYTE) FAST;

                } else {

                    TapeExtension->XferRate.XferRate = (UBYTE) SLOW;
                    TapeExtension->XferRate.MaxRate = (UBYTE) SLOW;

                }
                break;

            case TAPE_250Kbps:

                if (TapeExtension->DriveParms.DriveType == QIC40_DRIVE) {

                    TapeExtension->XferRate.XferRate = (UBYTE) SLOW;
                    TapeExtension->XferRate.MaxRate = (UBYTE) SLOW;

                } else {

                    return(UnspRate);

                }
                break;

            default:

                CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = UNSUPPORTED_RATE\n"));
                return(UnspRate);

            }

        }

        break;

    default:

        switch (driveConfig.XferRate) {

        case TAPE_1Mbps:

            if ((fdcType == FDC_82077 ||
                fdcType == FDC_82077AA ||
                fdcType == FDC_82078_44 ||
                fdcType == FDC_82078_64 ||
                fdcType == FDC_NATIONAL) &&
                (TapeExtension->DriveParms.DriveType == QIC80_DRIVE)) {

                TapeExtension->XferRate.XferRate = (UBYTE) FAST;
                TapeExtension->XferRate.MaxRate  = (UBYTE) FAST;

            } else {

                CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = UNSUPPORTED_RATE\n"));
                return(UnspRate);

            }

        case TAPE_500Kbps:

            if (TapeExtension->DriveParms.DriveType == QIC40_DRIVE) {

                TapeExtension->XferRate.XferRate = (UBYTE) FAST;
                TapeExtension->XferRate.MaxRate = (UBYTE) FAST;

            } else {

                TapeExtension->XferRate.XferRate = (UBYTE) SLOW;
                TapeExtension->XferRate.MaxRate = (UBYTE) SLOW;

            }
            break;

        case TAPE_250Kbps:

            if (TapeExtension->DriveParms.DriveType == QIC40_DRIVE) {

                TapeExtension->XferRate.XferRate = (UBYTE) SLOW;
                TapeExtension->XferRate.MaxRate = (UBYTE) SLOW;

            } else {

                return(UnspRate);

            }
            break;

        default:

            CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = UNSUPPORTED_RATE\n"));
            return(UnspRate);

        }

    }


#if DBG

    if (TapeExtension->XferRate.MaxRate == FAST) {
        rate = TapeExtension->XferRate.TapeFast;
    } else {
        rate = TapeExtension->XferRate.TapeSlow;
    }

    switch (rate) {
    case TAPE_250Kbps:
        CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = 250Kbps\n"));
        break;
    case TAPE_500Kbps:
        CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = 500Kbps\n"));
        break;
    case TAPE_1Mbps:
        CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = 1Mbps\n"));
        break;
    case TAPE_2Mbps:
        CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = 2Mbps\n"));
        break;
    default:
        CheckedDump(QIC117INFO,( "Q117i: Transfer Rate = UNSUPPORTED_RATE\n"));
    }

#endif

    return(Q117iConfigureFDC(TapeExtension));
}



VOID
Q117iDCR_Out(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT speed
    )

/*++

Routine Description:

    Output control data to the FDC digital control register.

Arguments:

    TapeExtension -

    Speed -

Return Value:

    None

--*/

{
    speed = (SHORT)((UCHAR)speed & 0x03);
    WRITE_CONTROLLER(&TapeExtension->QControllerData->FDC_Addr->dcr,speed);
}
