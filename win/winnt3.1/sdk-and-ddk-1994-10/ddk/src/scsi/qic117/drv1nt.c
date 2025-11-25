/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    drv1nt.c

Abstract:

    Error translation,  and NT interface routines for needed driver functions.

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
#include "q117log.h"


NTSTATUS
Q117iTranslateError(
   IN PDEVICE_OBJECT DeviceObject,
   IN STATUS ReturnValue
   )
/*++


Routine Description:



Arguments:

   ReturnValue -

Return Value:



--*/

{
    NTSTATUS ntStatus;
    PTAPE_EXTENSION tapeExtension;

    tapeExtension = DeviceObject->DeviceExtension;

    if (tapeExtension->PersistentNewCart &&
        (ReturnValue == NoErr)) {

        ReturnValue = NewCart;

    }

    if (ReturnValue) {

        ntStatus = (NTSTATUS)(STATUS_SEVERITY_WARNING << 30);
        ntStatus |= (FILE_DEVICE_TAPE << 16) & 0x3fff0000;
        ntStatus |= ReturnValue & 0x0000ffff;

        if (!tapeExtension->Found) {
            NTSTATUS logStatus = q117MapStatus(ReturnValue);
    
            if (logStatus != QIC117_NOTAPE) {
                q117LogError(
                    DeviceObject,
                    tapeExtension->ErrorSequence++,
                    0,
                    0,
                    ReturnValue,
                    ntStatus,
                    logStatus
                    );
            }
        }

    } else {

        ntStatus = STATUS_SUCCESS;

    }

    return(ntStatus);
}


STATUS
Q117iSleep(
   IN PTAPE_EXTENSION TapeExtension,
   IN QIC_TIME WaitTime,
   IN BOOLEAN InterruptSleep
   )

/*++

Routine Description:



Arguments:

   TapeExtension -

   WaitTime -

   InterruptSleep -

Return Value:



--*/

{
   NTSTATUS ntStatus;
   LARGE_INTEGER timeout;

   timeout = RtlLargeIntegerNegate(
                RtlEnlargedIntegerMultiply (
                    (LONG)(10 * 1000),
                    (LONG)WaitTime)
               );

   if (InterruptSleep) {

      ntStatus = KeWaitForSingleObject(
            &TapeExtension->QControllerData->InterruptEvent,
            Executive,
            KernelMode,
            FALSE,
            &timeout );

      if (ntStatus == STATUS_TIMEOUT) {

            return TimeOut;

      } else {

            return NoErr;

      }

   } else {

      (VOID) KeDelayExecutionThread(
            KernelMode,
            FALSE,
            &timeout );

      return TimeOut;

   }

   return NoErr;
}


VOID
Q117iShortTimer(
   IN QIC_TIME StallTime
   )

/*++

Routine Description:



Arguments:

   StallTime -

Return Value:

   None

--*/

{
   KeStallExecutionProcessor( StallTime );

   return;
}


VOID
Q117iResetInterruptEvent(
   IN PTAPE_EXTENSION TapeExtension
   )

/*++

Routine Description:



Arguments:

   TapeExtension -

Return Value:

   None

--*/

{
   TapeExtension->QControllerData->CurrentDeviceObject =
      TapeExtension->QDeviceObject;
   (VOID) KeResetEvent( &TapeExtension->QControllerData->InterruptEvent );

return;
}


VOID
Q117iDLockUnlockDMA(
   IN PTAPE_EXTENSION TapeExtension,
   IN BOOLEAN Lock
   )

/*++

Routine Description:



Arguments:

   TapeExtension -

   AdapterInfo -

   Lock -

Return Value:

   None

--*/

{
   KIRQL oldIrql;

   if (TapeExtension->QControllerData->AdapterObject) {

      if (Lock) {

            if (!TapeExtension->QControllerData->AdapterLocked) {

               //
               // Allocate an adapter channel for the I/O.
               //

               (VOID) KeResetEvent(
                  &TapeExtension->QControllerData->AllocateAdapterChannelEvent );

               KeRaiseIrql( DISPATCH_LEVEL, &oldIrql );


               IoAllocateAdapterChannel(
                  TapeExtension->QControllerData->AdapterObject,
                  TapeExtension->QDeviceObject,
                  TapeExtension->QControllerData->NumberOfMapRegisters,
                  Q117iTapeAllocateAdapterChannel,
                  TapeExtension->QControllerData );

               KeLowerIrql( oldIrql );

               //
               // Wait for the adapter to be allocated.  No
               // timeout; we trust the system to do it
               // properly - so KeWaitForSingleObject can't
               // return an error.
               //

               (VOID) KeWaitForSingleObject(
                  &TapeExtension->QControllerData->AllocateAdapterChannelEvent,
                  Executive,
                  KernelMode,
                  FALSE,
                  (PLARGE_INTEGER) NULL );

               TapeExtension->QControllerData->AdapterLocked = TRUE;
            }

      } else {

            if (TapeExtension->QControllerData->AdapterLocked) {

               //
               // Free the adapter channel that we just used.
               //

               KeRaiseIrql( DISPATCH_LEVEL, &oldIrql );

               IoFreeAdapterChannel( TapeExtension->QControllerData->AdapterObject );

               KeLowerIrql( oldIrql );

               TapeExtension->QControllerData->AdapterLocked = FALSE;
#if DBG
               if (QIC117DebugLevel & QIC117SHOWQD) {
                  while (TapeExtension->DbgHead != TapeExtension->DbgTail) {

                     switch(TapeExtension->DbgCommand[TapeExtension->DbgHead]) {
                           case 0x12345678:
                              DbgPrint("\nPgmFdc: ");
                              break;
                           case 0x12345679:
                              DbgPrint("\nReadFdc: ");
                              break;
                           case 0x1234567a:
                              DbgPrint("\nPgmDMA: ");
                              break;
                           case 0x1234567b:
                              DbgPrint("\nSendByte: ");
                              break;
                           case 0x1234567c:
                              DbgPrint("   ReceiveByte: ");
                              break;
                           case 0x1234567d:
                              DbgPrint("\nI/O Status: ");
                              break;
                           default:
                              // Dump command history
                              DbgPrint("%02x ", TapeExtension->DbgCommand[TapeExtension->DbgHead]);

                     }

                     TapeExtension->DbgHead++;
                     if (TapeExtension->DbgHead >= DBG_SIZE) {
                           TapeExtension->DbgHead = 0;
                     }


                  }
                  DbgPrint("\n");
               }

#endif


            }
      }
   }


   return;
}


VOID
hio_ProgramDMA(
   IN PTAPE_EXTENSION TapeExtension,
   IN PIRP Irp,
   IN BOOLEAN WriteOperation
   )

/*++

Routine Description:


Arguments:

   TapeExtension -

   Irp -

   WriteOperation -

Return Value:

   None

--*/

{

   PHYSICAL_ADDRESS val;

   Q117iDLockUnlockDMA(TapeExtension, TRUE);

   //
   // Map the transfer through the DMA hardware.
   //

   KeFlushIoBuffers( Irp->MdlAddress, !WriteOperation, TRUE );

   DbgAddEntry(0x1234567a);
   DbgAddEntry((ULONG)TapeExtension->QControllerData->AdapterObject);
   DbgAddEntry((ULONG)Irp->MdlAddress);
   DbgAddEntry((ULONG)TapeExtension->QControllerData->MapRegisterBase);
   DbgAddEntry((ULONG) MmGetMdlVirtualAddress(Irp->MdlAddress)
            + TapeExtension->RdWrOp.BytesTransferredSoFar );
   DbgAddEntry(TapeExtension->RdWrOp.TotalBytesOfTransfer);
   DbgAddEntry(WriteOperation);

   val = IoMapTransfer(
      TapeExtension->QControllerData->AdapterObject,
      Irp->MdlAddress,
      TapeExtension->QControllerData->MapRegisterBase,
      (PVOID)( (ULONG) MmGetMdlVirtualAddress(Irp->MdlAddress)
            + TapeExtension->RdWrOp.BytesTransferredSoFar ),
      &TapeExtension->RdWrOp.TotalBytesOfTransfer,
      WriteOperation );

   DbgAddEntry(val.HighPart);
   DbgAddEntry(val.LowPart);
   DbgAddEntry(TapeExtension->RdWrOp.TotalBytesOfTransfer);
}


VOID
hio_FlushDMA(
   IN PTAPE_EXTENSION TapeExtension,
   IN PIRP Irp,
   IN BOOLEAN WriteOperation
   )

/*++

Routine Description:



Arguments:

   TapeExtension -

   Irp -

   WriteOperation -

Return Value:

   None

--*/

{
   IoFlushAdapterBuffers(
      TapeExtension->QControllerData->AdapterObject,
      Irp->MdlAddress,
      TapeExtension->QControllerData->MapRegisterBase,
      (PVOID)( (ULONG) MmGetMdlVirtualAddress( Irp->MdlAddress )
            + TapeExtension->RdWrOp.BytesTransferredSoFar ),
      TapeExtension->RdWrOp.TotalBytesOfTransfer,
      WriteOperation );
}


STATUS
Q117iDDeselect(
   IN PTAPE_EXTENSION TapeExtension
   )

/*++

Routine Description:



Arguments:

   TapeExtension -

Return Value:



--*/

{
    STATUS retval = NoErr;                 // return value

    if (TapeExtension->QControllerData->AdapterLocked) {

        Q117iDLockUnlockDMA(TapeExtension, FALSE);

    }

    (VOID) Q117iResetFDC(TapeExtension);
    (VOID) Q117iSelectDrive(TapeExtension);
    (VOID) Q117iStopTape(TapeExtension);
    (VOID) Q117iDeselectDrive(TapeExtension);

    TapeExtension->NoPause = TRUE;
    TapeExtension->NewCart = FALSE;
    TapeExtension->QControllerData->CurrentInterrupt = FALSE;

    CheckedDump(QIC117INFO,( "Q117iDDeselect: Setting Controller Event\n"));

    (VOID) KeSetEvent(
       TapeExtension->QControllerData->ControllerEvent,
       (KPRIORITY) 0,
       FALSE );

    return retval;
}


NTSTATUS
Q117iClearIO(
   IN OUT PIRP Irp
   )

/*++

Routine Description:



Arguments:

   Irp -

Return Value:



--*/

{
   PTAPE_EXTENSION tapeExtension;
   PIO_STACK_LOCATION  irpStack = IoGetCurrentIrpStackLocation(Irp);


   tapeExtension = irpStack->DeviceObject->DeviceExtension;
   tapeExtension->QDeviceObject = irpStack->DeviceObject;

   tapeExtension->QControllerData->AbortRequested = FALSE;

   if (Q117iGetDriveError(tapeExtension) == NotRdy) {

      Q117iPauseTape(tapeExtension);

   }

   if (tapeExtension->NewCart) {

      Q117iNewTape(tapeExtension);

   }

   return STATUS_SUCCESS;
}

