/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    receive.c

Abstract:

    Recieve qic117 data bytes. 

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
Q117iReceiveByte(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT ReceiveLength,
    OUT USHORT *ReceiveData
    )

/*++

Routine Description:

    Read a byte/word of response data from the FDC.  Response data can
    be drive error/status information or drive configuration information.

        Wait for Track 0 from the tape drive to go active.  This indicates
        that the drive is ready to start sending data.

        Alternate sending Report Next Bit commands to the tape drive and
        sampling Track 0 (response data) from the tape drive until the
        proper number of response data bits have been read.

        Read one final data bit from the tape drive which is the confirmation
        bit.  This bit must be a 1 to confirm the transmission.

Arguments:

        TapeExtension -

        ReceiveLength - Type short used to indicate the desired data length

        RecieveData - Type unsigned short pointer used to return the received
        data.

Return Value:



--*/

{
    STATUS retval = 0;
    CHAR i = 0;
    UCHAR stat3;
    USHORT fdcData= 0;
#if DBG
    BOOLEAN save;

    // Lockout commands used to receive the status
    save = TapeExtension->DbgLockout;
    TapeExtension->DbgLockout = TRUE;
#endif

    if((retval = Q117iWaitActive(TapeExtension)) != NoErr) {

        return(retval);

    }

    do {

        if((retval = Q117iSendByte(TapeExtension, Rpt_Next_Bit)) != NoErr) {

            return(retval);

        }

        Q117iSleep(TapeExtension,
                   mt_wt2ticks,
                   FALSE
                   );


        if((retval = Q117iGetStatus(TapeExtension, &stat3)) != NoErr) {

            return(retval);

        }

        fdcData >>= 1;
        if (stat3 & ST3_T0) {

            fdcData |= 0x8000;

        }

        i++;

    } while (i < ReceiveLength);

    //
    // If the received data is only one byte wide, then shift data to the low
    // byte of fdcData.
    //

    if (ReceiveLength == READ_BYTE) {

        fdcData >>= READ_BYTE;

    }

    //
    // Return the low byte to the caller.
    //

    ((UCHAR *)ReceiveData)[LOW_BYTE] =
        ((UCHAR *)&fdcData)[LOW_BYTE];

    //
    // If the FDC data is a word, then return it to the caller.
    //

    if (ReceiveLength == READ_WORD) {

        ((UCHAR *)ReceiveData)[HI_BYTE] =
            ((UCHAR *)&fdcData)[HI_BYTE];

    }

    if((retval = Q117iSendByte(TapeExtension, Rpt_Next_Bit)) != NoErr) {

        return(retval);

    }

    Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

    if((retval = Q117iGetStatus(TapeExtension, &stat3)) != NoErr) {

        return(retval);

    }

    if(!(stat3 & (UCHAR)ST3_T0)) {

        TapeExtension->FirmwareError = Xmit_Overrun;
        return(TapeFlt);

    }

#if DBG
    TapeExtension->DbgLockout = save;
    DbgAddEntry(0x1234567c);
    DbgAddEntry(fdcData);
#endif

    return(retval);
}

STATUS
Q117iWaitActive(
    IN PTAPE_EXTENSION TapeExtension
    )

/*++

Routine Description:

    Wait up to 10ms for tape drive's TRK0 line to go active.  10 ms plus
    the 5 ms at the end of the Report command (in send_byte) is the
    specified 15 ms delay for this parameter.

Arguments:

    TapeExtension -

Return Value:



--*/


{
    STATUS retval;
    UCHAR stat3;

    Q117iSleep(TapeExtension, mt_wt2ticks , FALSE);

    if ((retval = Q117iGetStatus(TapeExtension, &stat3)) != NoErr) {

        return(retval);

    }

    if (!(stat3 & ST3_T0)) {

        CheckedDump(QIC117WARN,( "Wait active drive fault...\n" ));
        retval = DriveFlt;

    }

    return(retval);
}
