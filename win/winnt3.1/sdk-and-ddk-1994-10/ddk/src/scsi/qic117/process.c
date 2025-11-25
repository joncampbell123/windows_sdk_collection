/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    process.c

Abstract:

    Processes one low-level request.

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



NTSTATUS
Q117iProcessItem(
   IN OUT PIRP Irp
   )

/*++

Routine Description:

   Determine type of I/O operation being requested, Call appropriate
   subroutines.

   In block mode operation this routine returns when done processing
   the queue.  However, in concurrent operation (task switching or
   non-block mode) the routine NEVER returns.  Therefore, it is up
   to ClearIO to stop the task.

Arguments:

   Irp -

Return Value:



--*/

{
   PTAPE_EXTENSION tapeExtension;
   STATUS retval = NoErr;
   PIO_REQUEST Item;
   PIO_STACK_LOCATION  irpStack = IoGetCurrentIrpStackLocation(Irp);


   tapeExtension = irpStack->DeviceObject->DeviceExtension;
   tapeExtension->QDeviceObject = irpStack->DeviceObject;
   tapeExtension->NoPause = FALSE;
   Item = irpStack->Parameters.DeviceIoControl.Type3InputBuffer;

   if ((tapeExtension->QControllerData->DriveSelect.Selected == FALSE) &&
      tapeExtension->QControllerData->CurrentInterrupt &&
      tapeExtension->Found) {

      retval = Q117iSelectDrive(tapeExtension);

   }

   if (retval == NoErr) {

      switch (Item->Command) {

      case DFmt:
            retval = Q117iSetDriveMode(tapeExtension, FORMAT_MODE);
            break;

      case DVerify:
            retval = Q117iSetDriveMode(tapeExtension, VERIFY_MODE);
            break;

      default:
            retval = Q117iSetDriveMode(tapeExtension, PRIMARY_MODE);

      }

      if (retval == NoErr) {

            retval = Q117iTapeCommands(tapeExtension, Item, Irp);

      }

      Item->FirmwareError = tapeExtension->FirmwareError;

      if (tapeExtension->QControllerData->QueueEmpty) {

            if (tapeExtension->NoPause == FALSE) {

               if (Q117iGetDriveError(tapeExtension) == NotRdy) {

                  Q117iPauseTape(tapeExtension);

               }
            }

            if (tapeExtension->NewCart) {

               Q117iNewTape(tapeExtension);

            }
      }
   }

   Item->Status = retval;

#if DBG
#define TapeExtension tapeExtension
   DbgAddEntry(0x1234567d);
   DbgAddEntry(Item->Command);
   DbgAddEntry(Item->Status);
   TapeExtension->DbgLockout = FALSE;
#endif

   return Q117iTranslateError(tapeExtension->QDeviceObject, retval);
}

STATUS
Q117iSetDriveMode(
   IN PTAPE_EXTENSION TapeExtension,
   CHAR Mode
   )

/*++

Routine Description:

Set the mode of the tape drive according to the command to the
driver.

Arguments:

TapeExtension -

Mode -

Return Value:



--*/

{
   STATUS retval = 0;
   UCHAR modeCmd;

   if (Mode == TapeExtension->DriveParms.Mode) {

      return(NoErr);

   }

   if (TapeExtension->DriveParms.Mode == PRIMARY_MODE ||
      TapeExtension->DriveParms.Mode == VERIFY_MODE ||
      TapeExtension->DriveParms.Mode == FORMAT_MODE) {

      retval = Q117iStopTape(TapeExtension);

      if (retval != NoErr && retval != NoTape) {

            return(retval);

      }
   }

   switch (Mode) {

   case PRIMARY_MODE:
      modeCmd = Primary_Mode;
      break;

   case VERIFY_MODE:
      modeCmd = Verify_Mode;
      break;

   case FORMAT_MODE:
      modeCmd = Format_Mode;
      break;

   case DIAGNOSTIC_1_MODE:
      modeCmd = Diag_1_Mode;
      break;

   case DIAGNOSTIC_2_MODE:
      modeCmd = Diag_2_Mode;
      break;

   default:
      return(InvalCmd);

   }

   if ((retval = Q117iSendByte(TapeExtension, modeCmd)) != NoErr) {

      return(retval);

   }

   Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

   if (Mode == DIAGNOSTIC_1_MODE || Mode == DIAGNOSTIC_2_MODE) {

      if ((retval = Q117iSendByte(TapeExtension, modeCmd)) != NoErr) {

            return(retval);

      }

      Q117iSleep(TapeExtension, mt_wt2ticks, FALSE);

   } else {

      if ((retval = Q117iGetDriveError(TapeExtension)) == NotRdy) {

            retval = DriveFlt;

      }
   }

   TapeExtension->DriveParms.Mode = Mode;
   return(retval);
}
