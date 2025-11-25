/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    seek.c

Abstract:

    positions tape to correct location to be before the desired track and block

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
Q117iSeek(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Reposition tape for desired track and block.

        Change track first if necessary.

        Seek at high speed to approximately get to the specified area on
        the tape.

        Read ID marks from the tape until the tape is positioned 1 block in
        front of (logically) the desired block.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;
    BOOLEAN newTrack;
    SHORT seekCount;
    BOOLEAN retensionFlag = FALSE;

    newTrack = FALSE;
    seekCount = MAX_SEEK_COUNT;

    do {

        if (TapeExtension->TapePosition.D_Segment == 0)

            if ((retval = Q117iLogicalBOT(TapeExtension)) != NoErr) {

                return(retval);

            }

        if (TapeExtension->TapePosition.D_Track !=
            TapeExtension->TapePosition.C_Track) {

            if ((retval = Q117iChangeTrack(
                            TapeExtension,
                            TapeExtension->TapePosition.D_Track)) != NoErr) {

                return(retval);

            }

            if (!TapeExtension->DriveParms.Status.BOT &&
                !TapeExtension->DriveParms.Status.EOT) {

                newTrack = TRUE;

            }

        }

        if (TapeExtension->TapePosition.D_Segment == 0)
            return(NoErr);

        if (newTrack == TRUE) {

            retval = Q117iReadIDRepeat(TapeExtension);

        }

        if (retval == NoErr) {

            retval = Q117iHighSpeedSeek(TapeExtension);

        }

        if (retval == NoErr) {

            retval = Q117iReadIDRepeat(TapeExtension);

        }

        --seekCount;

        if ((retval == SeekErr || seekCount == 0) && retensionFlag == FALSE) {

            if ((retval = Q117iStopTape(TapeExtension)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iDReten(TapeExtension)) != NoErr) {

                return(retval);

            }

            seekCount = MAX_SEEK_COUNT;
            retensionFlag = TRUE;
            retval = NoErr;

        }

        if (retval != NoErr) {

            return(retval);

        }

    } while (!((0 <= ((TapeExtension->TapePosition.D_Segment - 1) -
                    TapeExtension->TapePosition.C_Segment)) &&
            (((TapeExtension->TapePosition.D_Segment - 1) -
                TapeExtension->TapePosition.C_Segment) <= 10)) &&
            (seekCount > 0));

    if (seekCount == 0) {

        CheckedDump(QIC117WARN,( "SeekErr - seekCount = 0\n" ));
        return(SeekErr);

    }

    do {

        if ((retval = Q117iReadIDRepeat(TapeExtension)) != NoErr) {

            return(retval);

        }


    } while ((TapeExtension->TapePosition.D_Segment - 1) >
            TapeExtension->TapePosition.C_Segment);

    return(retval);
}


STATUS
Q117iChangeTrack(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT DestinationTrack
    )

/*++

Routine Description:

    Position the tape drive head to a new track.

Arguments:

    TapeExtension -

    DestinationTrack -

Return Value:



--*/

{
    STATUS retval;

    if ((retval = Q117iStopTape(TapeExtension)) != NoErr) {

        return(retval);

    }

    if ((retval = Q117iSendByte(TapeExtension, Seek_Track)) != NoErr) {

        return(retval);

    }

    Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

    if ((retval = Q117iSendByte(TapeExtension, (CHAR)(DestinationTrack + 2)))
        != NoErr) {

        return(retval);

    }

    if ((retval = Q117iWaitCommandComplete(TapeExtension, mt_wt007s)) !=
        NoErr) {

        return(retval);

    }

    TapeExtension->TapePosition.C_Track =
        TapeExtension->TapePosition.D_Track;

    if (TapeExtension->DriveParms.Status.BOT ||
        TapeExtension->DriveParms.Status.EOT) {

        if (((TapeExtension->TapePosition.C_Track / 2 * 2 ==
                TapeExtension->TapePosition.C_Track) &&
                (TapeExtension->DriveParms.Status.BOT)) ||
            ((TapeExtension->TapePosition.C_Track / 2 * 2 !=
                TapeExtension->TapePosition.C_Track) &&
                (TapeExtension->DriveParms.Status.EOT))) {

            TapeExtension->TapePosition.C_Segment = 0;

        } else {

            TapeExtension->TapePosition.C_Segment =
                TapeExtension->TapeParms.SegTtrack;

        }

    }
    return(retval);
}


STATUS
Q117iReadIDRepeat(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Read an ID field off of the tape with the FDC. Read_id_repeat will
    attempt 10 times to read a legal ID field before a failure is returned.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval;
    SHORT readIdCnt;
    FDC_STATUS status;

    readIdCnt = NumBad;

    while (readIdCnt > 0) {

        --readIdCnt;

        if ((retval = Q117iStartTape(TapeExtension)) != NoErr) {

            return(retval);

        }

        retval = Q117iDoReadID(TapeExtension, mt_wttrks, &status);

        if (retval && retval != TimeOut) {

            return(retval);

        }

        if (!retval && !(status.ST0 & ST0_IC) &&
            (TapeExtension->QControllerData->InterfaceType != MicroChannel ||
                !(status.ST1 & ST1_MA))) {

            break;

        } else {

            if ((retval = Q117iChangeTrack(
                            TapeExtension,
                            TapeExtension->TapePosition.D_Track)) != NoErr) {

                return(retval);

            }

            if (TapeExtension->DriveParms.Status.BOT ||
                TapeExtension->DriveParms.Status.EOT) {

                if (((TapeExtension->TapePosition.C_Track / 2 * 2 ==
                        TapeExtension->TapePosition.C_Track) &&
                        (TapeExtension->DriveParms.Status.BOT)) ||
                    ((TapeExtension->TapePosition.C_Track / 2 * 2 !=
                        TapeExtension->TapePosition.C_Track) &&
                        (TapeExtension->DriveParms.Status.EOT))) {

                    TapeExtension->TapePosition.C_Segment = 0;
                    return(NoErr);

                } else {

                    TapeExtension->TapePosition.C_Segment =
                        TapeExtension->TapeParms.SegTtrack;
                    return(NoErr);

                }
            }
        }
    }

    if (readIdCnt == 0) {
        CheckedDump(QIC117WARN,( "SeekErr - readIdCnt = 0\n" ));
        return(SeekErr);
    }

    TapeExtension->TapePosition.C_Segment =
        (status.H * TapeExtension->TapeParms.FtrackFside +
        status.C) * SEG_FTK + (status.R - 1) / FSC_SEG;
    TapeExtension->TapePosition.C_Track =
        TapeExtension->TapePosition.C_Segment /
        TapeExtension->TapeParms.SegTtrack;
    TapeExtension->TapePosition.C_Segment =
        TapeExtension->TapePosition.C_Segment %
        TapeExtension->TapeParms.SegTtrack;

    return(retval);
}


STATUS
Q117iDoReadID(
    IN PTAPE_EXTENSION TapeExtension,
    IN QIC_TIME ReadIdDelay,
    IN FDC_STATUS *ReadIdStatus
    )

/*++

Routine Description:

    Try to read an ID fiedl off of the tape via the FDC.

Arguments:

    TapeExtension -

    ReadIdDelay -

    ReadIdStatus -

Return Value:



--*/

{
    STATUS retval;
    SHORT statLength;
    struct read_id_cmd readId;

    readId.command = 0x4a;
    readId.drive = (UCHAR)TapeExtension->DriveParms.DriveSelect;

    (VOID) Q117iResetInterruptEvent(TapeExtension);
    if ((retval = Q117iProgramFDC(
                    TapeExtension,
                    (CHAR *)&readId,
                    sizeof(readId),
                    TRUE)) != NoErr) {

        Q117iResetFDC(TapeExtension);
        return(retval);

    }

    if ((retval = Q117iSleep(
                    TapeExtension,
                    ReadIdDelay,
                    TRUE)) == TimeOut) {

        Q117iResetFDC(TapeExtension);
        return(retval);

    }

    if ((retval = Q117iReadFDC(
                    TapeExtension,
                    (CHAR *)ReadIdStatus,
                    &statLength)) != NoErr) {

        return(retval);

    }

    return(retval);
}


VOID
Q117iGetComFirmStr(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT CHAR **Ptr,
    IN SHORT Index
    )

/*++

Routine Description:

    Takes a string and fills it with a requested DComFirm string.

Arguments:

    TapeExtension -

    Ptr - a byte pointer that points to memory into which the requested
    DComFirm string is to be placed.

    Index - a number that indicates which DcomFirm string to place in the
    memory pointed to by the Ptr parameter.

Return Value:

    None

--*/

{
    SHORT numChars;  // number of chars in requested DComfirm str


    switch(Index) {

    case DCOMFIRM_READ_RAM_REQ:
        *Ptr = DCOMFIRM_READ_RAM_STRING;
        numChars = DCOMFIRM_NUM_READ_RAM_CHARS;
        break;

    case DCOMFIRM_SET_RAM_REQ:
        *Ptr = DCOMFIRM_SET_RAM_STRING;
        numChars = DCOMFIRM_NUM_SET_RAM_CHARS;
        break;

    case DCOMFIRM_WRITE_RAM_REQ:
        *Ptr = DCOMFIRM_WRITE_RAM_STRING;
        numChars = DCOMFIRM_NUM_WRITE_RAM_CHARS;
        break;

    }

}


STATUS
Q117iLogicalBOT(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Go at high speed to logical BOT. Logical BOT is physical BOT for even
    numbered tracks and physical EOT for odd numbered tracks.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval;
    CHAR *string;      // holds DComFirm string
    SHORT direction;   // tells physical direction of tape movement


    if ((retval = Q117iStopTape(TapeExtension)) != NoErr) {

        return(retval);

    }

    if ((TapeExtension->TapePosition.D_Track / 2 * 2) ==
        TapeExtension->TapePosition.D_Track) {

        retval = Q117iSendByte(TapeExtension, Physical_Rev);
        direction = REVERSE;

    } else {

        retval = Q117iSendByte(TapeExtension, Physical_Fwd);
        direction = FORWARD;

    }

    if (retval) {

        return(retval);

    }

    //
    // This is part of the Sankyo motor "hack."  While the motor is moving
    // at high speed to the end of the tape, the driver sets the RAM pointer
    // on the 8051 to point to the byte in memory that contains the bit that
    // tells whether or not the EOT/BOT sensor is over a hole.  This
    // concurrent operation is purely for performance enhancement (saves
    // ~220 msec).
    //

    if ((TapeExtension->DriveParms.Version == FIRM_VERSION_64) &&
            (TapeExtension->DriveParms.Flavor == CMS)) {

        Q117iSleep(TapeExtension, mt_wt200ms, FALSE);

        //
        // Set the ram ptr to the byte with the hole_flag bit.
        //

        Q117iGetComFirmStr(TapeExtension, &string, DCOMFIRM_SET_RAM_REQ);
        string[DCOMFIRM_SETRAM_HI_NIB_INDEX] =
            HOLE_FLAG_BYTE_ADD_UPPER_NIBBLE;
        string[DCOMFIRM_SETRAM_LOW_NIB_INDEX] =
            HOLE_FLAG_BYTE_ADD_LOWER_NIBBLE;

        if ((retval = Q117iDComFirm(TapeExtension, (CHAR  *)string)) !=
            NoErr) {

            return(retval);

        }
    }

    if ((retval = Q117iWaitCommandComplete(
                        TapeExtension,
                        TapeExtension->TapeParms.TimeOut[PHYSICAL])) !=
        NoErr) {

        return(retval);

    }

    if (TapeExtension->DriveParms.Version == FIRM_VERSION_64) {

        //
        // Prepare the communication string to read the byte with the
        // hole_flag bit in it.
        //

        //
        // Wait for the motor to stop.
        //

        Q117iSleep(TapeExtension, mt_wt260ms, FALSE);


        //
        // Read the byte with the hole flag bit in it.  If the bit is 0,
        // that means the drive has stopped over a hole.  In that case,
        // the tape zone counter must be adjusted and written to the drive,
        // and the driver saves the day.
        //

        Q117iGetComFirmStr(TapeExtension, &string,DCOMFIRM_READ_RAM_REQ);

        if ((retval = Q117iDComFirm(TapeExtension, (CHAR  *)string)) !=
            NoErr) {

            return(retval);

        }

        if (!(string[DCOMFIRM_READ_RTRN_BYTE_INDEX] & HOLE_INDICATOR_MASK)) {

            if (direction == REVERSE) {

                //
                // If at BOT, the only cause for concern is when the EOT/BOT
                // sensor is over the rightmost hole of the BOT pair.  To
                // differentiate this from the case where the sensor is
                // sitting over the leftmost hole, read the double hole
                // distance counter.  If it is non-zero, do nothing.
                //

                //
                // Set the ram ptr to 0x3B, the double hole counter,
                //

                Q117iGetComFirmStr(
                    TapeExtension, &string, DCOMFIRM_SET_RAM_REQ);
                string[DCOMFIRM_SETRAM_HI_NIB_INDEX] =
                    DBL_HOLE_CNTER_ADD_UPPER_NIBBLE;
                string[DCOMFIRM_SETRAM_LOW_NIB_INDEX] =
                    DBL_HOLE_CNTER_ADD_LOWER_NIBBLE;
                if ( (retval = Q117iDComFirm(
                                    TapeExtension,
                                    (CHAR  *)string)) != NoErr) {

                    return(retval);

                }

                //
                // Read the double hole counter.
                //

                Q117iGetComFirmStr(
                    TapeExtension, &string,DCOMFIRM_READ_RAM_REQ);

                if ((retval = Q117iDComFirm(
                                TapeExtension,
                                (CHAR  *)string)) != NoErr) {

                    return(retval);

                }

                if (!(string[DCOMFIRM_READ_RTRN_BYTE_INDEX])) {

                    if ((retval = Q117iAdjustTapeZone(TapeExtension, (SHORT) AT_BOT))
                        != NoErr) {

                        return(retval);

                    }
                }

            } else {

                if ((retval = Q117iAdjustTapeZone(TapeExtension, (SHORT) AT_EOT)) !=
                    NoErr) {

                    return(retval);

                }
            }
        }
    }
    if ((TapeExtension->TapePosition.D_Track / 2 * 2) ==
        TapeExtension->TapePosition.D_Track) {

        if (!TapeExtension->DriveParms.Status.BOT) {

            CheckedDump(QIC117WARN,( "SeekErr - not at BOT\n" ));
            return(SeekErr);

        }

    } else {

        if (!TapeExtension->DriveParms.Status.EOT) {

            CheckedDump(QIC117WARN,( "SeekErr - not at EOT\n" ));
            return(SeekErr);

        }
    }

    TapeExtension->TapePosition.C_Segment = 0;
    return(NoErr);
}


STATUS
Q117iAdjustTapeZone(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT TapePosition
    )

/*++

Routine Description:

    Adjust the tape zone counter to compensate for stopping with the EOT/BOT
    sensor over a hole.

Arguments:

    TapeExtension -

    TapePosition - tells wheather EOT or BOT

Return Value:



--*/

{

    CHAR highNibble;
    CHAR lowNibble;
    CHAR *string;
    STATUS retval = NoErr;

    //
    // Set the pointer in the 8051 RAM to the tape zone counter.
    //

    Q117iGetComFirmStr(TapeExtension, &string, DCOMFIRM_SET_RAM_REQ);
    string[DCOMFIRM_SETRAM_HI_NIB_INDEX] = TAPE_ZONE_BYTE_ADD_UPPER_NIBBLE;
    string[DCOMFIRM_SETRAM_LOW_NIB_INDEX] = TAPE_ZONE_BTYE_ADD_LOWER_NIBBLE;

    retval = Q117iDComFirm(TapeExtension, (CHAR  *)string);

    if (retval == NoErr) {

        if (TapePosition == AT_EOT) {

            highNibble = EOT_ZONE_COUNTER_UPPER_NIBBLE;
            lowNibble = EOT_ZONE_COUNTER_LOWER_NIBBLE;

        } else {

            highNibble = BOT_ZONE_COUNTER_UPPER_NIBBLE;
            lowNibble = BOT_ZONE_COUNTER_LOWER_NIBBLE;

        }

        //
        // Write the upper and lower nybbles of the tape zone counter.
        //

        Q117iGetComFirmStr(TapeExtension, &string, DCOMFIRM_WRITE_RAM_REQ);
        string[DCOMFIRM_WRTRAM_HI_NIB_INDEX] = highNibble;
        string[DCOMFIRM_WRTRAM_LOW_NIB_INDEX] = lowNibble;
        retval = Q117iDComFirm(TapeExtension, (CHAR  *)string);
    }

    return(retval);
}


STATUS
Q117iHighSpeedSeek(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Execute a High Speed Seek.  There are two methods of doing this now.
    First, if the Skip commands are not implemented, the high speed seek
    is accomplished by calculating the approximate amount of time needed
    at high speed to reach the target position and allowing the tape drive
    to go at 90 ips for that amount of time.  If the Skip commands are
    implemented, the high speed seek is merely the proper command with
    a calculated offset.

    The Seeking is done by using either the Skip_N_Segments command or by using
    the time seeking algorithm provided by Q117iWaitSeek. The Skip_N_Segments commands
    are not reliable in all versions of the firmware. Only in versions for JUMBO_B
    and greater are the commands available at all and only in versions of 65 and
    greater are the Skip_N_Segment commands reliable for skipping past the
    DC erased gap.

Arguments:

    TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;
    SHORT seekOffset;
    SHORT i;
    CHAR seekDir;
    CHAR skipNSegs;
    USHORT skip;

    //
    // Determine the logical direction that the tape needs to be moved
    //

    if ((seekOffset = (TapeExtension->TapePosition.D_Segment - 1) -
                            TapeExtension->TapePosition.C_Segment) >= 0) {

        seekDir = FWD;

    } else {

        seekDir = REV;
        seekOffset = 0 - seekOffset;

    }

    if (seekDir == REV || seekOffset > STOP_LEN) {

        if((retval = Q117iStopTape(TapeExtension)) != NoErr) {

                return(retval);

        }

        if (seekDir == FWD) {

            if (TapeExtension->DriveParms.SeekMode == SEEK_TIMED) {

                seekOffset -= SEEK_SLOP;

            } else {

                seekOffset -= 1;

            }

        } else {

            //
            // seek direction is reverse
            //

            if (TapeExtension->DriveParms.Status.BOT ||
                TapeExtension->DriveParms.Status.EOT) {

                switch (TapeExtension->TapeParms.TapeType) {

                case QIC40_SHORT:
                case QIC80_SHORT:
                    seekOffset += QIC_REV_OFFSET;
                    break;

                case QIC40_LONG:
                case QIC80_LONG:
                case QIC500_SHORT:
                    seekOffset += QIC_REV_OFFSET_L;
                    break;

                case QICEST_40:
                case QICEST_80:
                case QICEST_500:
                    seekOffset += QICEST_REV_OFFSET;
                    break;

                }

            } else {

                if (TapeExtension->DriveParms.SeekMode == SEEK_TIMED) {

                    seekOffset += SEEK_SLOP;

                } else {

                    seekOffset += 1;

                }

            }
        }

        switch (TapeExtension->DriveParms.SeekMode) {

        case SEEK_SKIP:
            //
            // Determine the offset to be used for the Skip_N_Segment commands
            //

            CheckedDump(QIC117SHOWQD,( "Q117i: Skip_N_Segments Seek\n"));

            if (seekDir == FWD) {

                skipNSegs = Skip_N_Fwd;

            } else {

                skipNSegs = Skip_N_Rev;

            }

            // Skip the first bytes worth of segments

            if ((retval = Q117iSendByte(
                    TapeExtension,
                    skipNSegs)) != NoErr) {

                return(retval);

            }

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

            if ((retval = Q117iSendByte(
                    TapeExtension,
                    ((seekOffset & 0xf) + 2))) != NoErr) {

                return(retval);

            }

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

            seekOffset >>= 4;

            if ((retval = Q117iSendByte(
                    TapeExtension,
                    ((seekOffset & 0x0f)+2))) != NoErr) {

                return(retval);

            }

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

            if ((retval = Q117iWaitCommandComplete(
                    TapeExtension,
                    TapeExtension->TapeParms.TimeOut[PHYSICAL])) != NoErr) {

                return(retval);

            }

            seekOffset >>= 4;

            for (;seekOffset != 0; --seekOffset) {

                // Skip the second bytes worth of segments

                for (i=0; i<2; ++i) {
                    if (i) {

                        skip = 1;

                    } else {

                        skip = MAX_SKIP;

                    }

                    if ((retval = Q117iSendByte(
                            TapeExtension,
                            skipNSegs)) != NoErr) {

                        return(retval);

                    }

                    Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

                    if ((retval = Q117iSendByte(
                            TapeExtension,
                            ((skip & 0xf) + 2))) != NoErr) {

                        return(retval);

                    }

                    Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

                    if ((retval = Q117iSendByte(
                            TapeExtension,
                            ((skip >> 4) + 2))) != NoErr) {

                        return(retval);

                    }

                    Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

                    if ((retval = Q117iWaitCommandComplete(
                            TapeExtension,
                            TapeExtension->TapeParms.TimeOut[PHYSICAL])) != NoErr) {

                        return(retval);

                    }
                }
            }

            break;

        case SEEK_SKIP_EXTENDED:

            if (seekDir == FWD) {

                skipNSegs = Skip_N_Fwd_Ext;

            } else {

                skipNSegs = Skip_N_Rev_Ext;

            }


            if ((retval = Q117iSendByte(
                    TapeExtension,
                    skipNSegs)) != NoErr) {

                return(retval);

            }

            Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

            for (i = 0; i < MAX_SEEK_NIBBLES; i++) {

                if ((retval = Q117iSendByte(
                        TapeExtension,
                        ((seekOffset & 0xf) + 2))) != NoErr) {

                    return(retval);

                }

                Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

                seekOffset >>= 4;

            }

            if ((retval = Q117iWaitCommandComplete(
                    TapeExtension,
                    TapeExtension->TapeParms.TimeOut[PHYSICAL])) != NoErr) {

                return(retval);

            }

            break;

        default: // SEEK_TIMED

            //
            // Skip segments commands are not available
            //

            CheckedDump(QIC117SHOWQD,( "Q117i: Timed Seek\n"));


            if (((seekDir == FWD) &&
                    (TapeExtension->TapePosition.C_Track / 2 * 2 ==
                    TapeExtension->TapePosition.C_Track)) ||
                    ((seekDir == REV) &&
                    (TapeExtension->TapePosition.C_Track / 2 * 2 !=
                    TapeExtension->TapePosition.C_Track))) {

                if ((retval = Q117iSendByte(TapeExtension, Physical_Fwd)) !=
                        NoErr) {

                    return(retval);

                }

            } else {

                if ((retval = Q117iSendByte(TapeExtension, Physical_Rev)) !=
                        NoErr) {

                    return(retval);

                }
            }

            if ((retval = Q117iWaitSeek(TapeExtension, seekOffset)) != NoErr) {

                return(retval);

            }

            if ((retval = Q117iStopTape(TapeExtension)) != NoErr) {

                return(retval);

            }
        }
    }
    return(retval);
}


STATUS
Q117iWaitSeek(
    IN PTAPE_EXTENSION TapeExtension,
    SHORT SeekDelay
    )

/*++

Routine Description:

    Execute a timed high speed seek.  This routine is used for CMS drives
    that have not implemented the Skip commands (all those before firmware
    version 34) and all non-CMS drives.

Arguments:

    TapeExtension -

    SeekDelay -

Return Value:



--*/

{
    STATUS retval;

    do {

        Q117iSleep(TapeExtension, mt_wt200ms, FALSE);
        retval = Q117iGetDriveError(TapeExtension);

        if (retval && retval != NotRdy) {

            return(retval);

        }

        if (retval == NoErr) {

            if (TapeExtension->DriveParms.Status.BOT ||
                TapeExtension->DriveParms.Status.EOT) {

                break;

            }

            return(DriveFlt);
        }

        --SeekDelay;

    } while (SeekDelay > 0);

    return(NoErr);
}
