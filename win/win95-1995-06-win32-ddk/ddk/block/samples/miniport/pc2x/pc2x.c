/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*************************************************************************
* 
* Copyright (c) 1993 Iomega Corporation
* 
* Module Name:
* 
*    PC2x.c
* 
* Abstract:
* 
*    This is the miniport driver for the Iomega PC2x 8-bit SCSI adapter card.
* 
* Environment:
* 
*    kernel mode only
* 
*/

#define DBG 1

#include "miniport.h"
#include "stdarg.h"
#include "pc2x.h"      // includes scsi.h

//
// Logical Unit states.
//
typedef enum _LU_STATE {

   LS_UNDETERMINED,
   LS_SELECT,
   LS_COMMAND,
   LS_DATA,
   LS_STATUS,
   LS_MSG_IN,
   LS_MSG_OUT,
   LS_COMPLETE

} LU_STATE, *PLU_STATE;

//
// Logical Unit extension
//
typedef struct _SPECIFIC_LU_EXTENSION {

   LU_STATE            LuState;            // State information.
   ULONG               SavedDataPointer;   // Current data pointer.
   ULONG               SavedDataLength;    // Current data lenght.
   PSCSI_REQUEST_BLOCK ActiveLuRequest;    // Active Srb for this LUN.

} SPECIFIC_LU_EXTENSION, *PSPECIFIC_LU_EXTENSION;

//
// Device extension
//
typedef struct _SPECIFIC_DEVICE_EXTENSION {

   PUCHAR              BaseAddress;        // Port address of adapter.
   ULONG               CurDataPointer;     // Current pointer for active LUN.
   ULONG               CurDataLength;      // Bytes left to xfer to this LUN.
   PSPECIFIC_LU_EXTENSION  ActiveLu;       // Currently active LUN.
   UCHAR               PathId;
   UCHAR               ControlRegister;    // Current val of PC2 control reg.
   BOOLEAN             InterruptPending;   // Waiting for interrupt.
   BOOLEAN             DmaPending;         // Waiting for DMA setup.

} SPECIFIC_DEVICE_EXTENSION, *PSPECIFIC_DEVICE_EXTENSION;


//
// Function declarations
//

ULONG
DriverEntry(
   IN PVOID DriverObject,
   IN PVOID Argument2
   );

ULONG
PC2xFindAdapter(
   PSPECIFIC_DEVICE_EXTENSION              DeviceExtension,
   IN PVOID                                Context,
   IN PVOID                                BusInformation,
   IN PCHAR                                ArgumentString,
   IN OUT PPORT_CONFIGURATION_INFORMATION  ConfigInfo,
   OUT PBOOLEAN                            Again
   );

ULONG
PC2xDetermineInstalled(
   PSPECIFIC_DEVICE_EXTENSION              DeviceExtension,
   IN OUT PPORT_CONFIGURATION_INFORMATION  ConfigInfo,
   IN OUT PULONG                           AdapterCount,
   OUT PBOOLEAN                            Again
   );

VOID
PC2xTimer(
   IN PVOID Context
   );

BOOLEAN
PC2xInitialize(
   IN PVOID Context
   );

BOOLEAN
PC2xStartIo(
   IN PVOID Context,
   IN PSCSI_REQUEST_BLOCK Srb
   );

BOOLEAN
PC2xInterrupt(
   IN PVOID Context
   );

VOID
PC2xDmaStarted(
   IN PVOID Context
   );

BOOLEAN
PC2xResetBus(
   IN PVOID Context,
   IN ULONG PathId
   );




#if DBG

//
// Globals and externals used for debugging.
//

//
// PC2xDebug affects which debug prints are enabled:
//
//     0x001   Arbitration and selection
//     0x002   Command, status
//     0x004   Data transfer
//     0x008   Message in
//     0x010   Miniport entry points and completion
//     0x020   Initialization and interrupt
//     0x040   StatusCheck and WaitForRequest
//     0x080   Control register manipulation
//     0x100   Completion
//     0x200   Phase info
//     0x400   Temporary
//     0x800   Panic
//

ULONG PC2xDebug = ( 0x000 );

#define PC2DebugPrint(MASK, ARGS)    \
       if (MASK & PC2xDebug) {    \
       ScsiDebugPrint ARGS;    \
       }

#else

#define PC2DebugPrint(MASK, ARGS)

#endif

 


/*************************************************************************
* Routine Name:
*
*    PC2xWriteControl
*
* Routine Description:
* 
*    This routine sets the control register on the adapter and remembers
*    the value set in the device extension.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
*    Value           - New value for the control register.
* 
* Return Value:
* 
*    None
*/
VOID
PC2xWriteControl(
   IN PSPECIFIC_DEVICE_EXTENSION   DeviceExtension,
   IN UCHAR                        Value
   )
{
   PC2DebugPrint(0x80,
                 (0, "WriteControl: Dev= %x, NewVal= %x\n",
                 DeviceExtension,
                 Value));

   DeviceExtension->ControlRegister = Value;

   PC2X_SET_CONTROL(DeviceExtension->BaseAddress,
                      Value);

} // end PC2xWriteControl()



/*************************************************************************
* Routine Name:
*
*    PC2xWaitForRequestLine
*
* Routine Description:
* 
*    Spin checking the status of the PC2X adapter until it indicates that
*    the SCSI REQUEST line is high.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    TRUE    - indicates that the SCSI REQUEST line was asserted in time.
*    FALSE   - indicates timeout occurred while waiting for the SCSI REQUEST
*              line.
*/
BOOLEAN
PC2xWaitForRequestLine(
   PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   PUCHAR  baseAddress = DeviceExtension->BaseAddress;
   ULONG   spinCount   = REQUEST_SPIN_WAIT;

   PC2DebugPrint(0x40, (0, "PC2WaitForReq: "));

   do {

       if (PC2X_READ_STATUS(baseAddress) & S_REQUEST) {

           PC2DebugPrint(0x40, (0, "Got REQUEST\n"));
           return TRUE;
       }

       ScsiPortStallExecution(1);

   } while (spinCount--);

   PC2DebugPrint(0x04, (0, "TIMEOUT\n"));

   if (DeviceExtension->ActiveLu) {

       ScsiPortLogError(DeviceExtension,
                        DeviceExtension->ActiveLu->ActiveLuRequest,
                        DeviceExtension->ActiveLu->ActiveLuRequest->PathId,
                        DeviceExtension->ActiveLu->ActiveLuRequest->TargetId,
                        DeviceExtension->ActiveLu->ActiveLuRequest->Lun,
                        SP_REQUEST_TIMEOUT,
                        (ULONG) DeviceExtension);
   }

   return FALSE;

} // end PC2xWaitForRequestLine()


/*************************************************************************
* Routine Name:
*
*    PC2xStatusCheck
*
* Routine Description:
* 
*    Spin checking the status of the PC2X adapter until it matches
*    the desired value.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
*    Mask            - Bits to mask out of the value returned by adapter
*                       status register.
*    Compare         - Desired result of status register after is is "Mask"ed.
*    SpinMax         - Number of times to test adapter's status register.
* 
* Return Value:
* 
*    TRUE indicates desired value was returned by the adapter status register.
*    FALSE indicates timeout occurred and desired value was not returned.
*/
BOOLEAN
PC2xStatusCheck(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension,
   IN UCHAR Mask,
   IN UCHAR Compare,
   IN ULONG SpinMax
   )
{
   PUCHAR  baseAddress = DeviceExtension->BaseAddress;

   PC2DebugPrint(0x40, (0, "PC2StatusCheck: "));

   PC2DebugPrint(0x40,
                 (0, "Dev=%x Status=%x Mask=%x Compare=%x ",
                 DeviceExtension,
                 PC2X_READ_STATUS(baseAddress),
                 Mask,
                 Compare));

   do {

       if ((UCHAR)(PC2X_READ_STATUS(baseAddress) & Mask) == Compare) {

           PC2DebugPrint(0x40, (0, "Got Compare\n"));
           return TRUE;
       }

       ScsiPortStallExecution(1);

   } while (SpinMax--);

   PC2DebugPrint(0x40,
                 (0, "TIMEOUT Status=%x\n",
                 PC2X_READ_STATUS(baseAddress)));

   return FALSE;

} // end PC2xStatusCheck()


/*************************************************************************
* Routine Name:
*
*    PC2xDetermineNextState
*
* Routine Description:
* 
*    This routine determines the next logical state of this state machine
*    based on the current SCSI bus phase.  It is typically called because
*    from the current state, the next state can be one of a number of other
*    states.  For example, after issuing the last command byte of COMMAND
*    phase, the device may want to drive us into DATA_IN, DATA_OUT, STATUS,
*    or MSG_IN (i.e., transfer data, report an error, or disconnect).
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    None
*/
VOID
PC2xDetermineNextState(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   PC2DebugPrint(0x01,
                 (0, "PC2NextPhase: phase = %x ",
                 PC2X_READ_PHASE(DeviceExtension->BaseAddress)));


   switch (PC2X_READ_PHASE(DeviceExtension->BaseAddress)) {

       case BP_COMMAND:

           PC2DebugPrint(0x200, (0, "LS_COMMAND\n"));
           DeviceExtension->ActiveLu->LuState = LS_COMMAND;
           break;

       case BP_DATA_IN:
       case BP_DATA_OUT:

           PC2DebugPrint(0x200, (0, "LS_DATA\n"));
           DeviceExtension->ActiveLu->LuState = LS_DATA;
           break;

       case BP_MESSAGE_IN:

           PC2DebugPrint(0x200, (0, "LS_MSG_IN\n"));
           DeviceExtension->ActiveLu->LuState = LS_MSG_IN;
           break;

       case BP_MESSAGE_OUT:

           PC2DebugPrint(0x200, (0, "LS_MSG_OUT\n"));
           DeviceExtension->ActiveLu->LuState = LS_MSG_OUT;
           break;

       case BP_STATUS:

           PC2DebugPrint(0x200, (0, "LS_STATUS\n"));
           DeviceExtension->ActiveLu->LuState = LS_STATUS;
           break;

       case BP_BUS_FREE:

           PC2DebugPrint(0x200, (0, "LS_BUS_FREE\n"));

           DeviceExtension->ActiveLu->ActiveLuRequest->SrbStatus =
                                      SRB_STATUS_UNEXPECTED_BUS_FREE;

           DeviceExtension->ActiveLu->LuState = LS_COMPLETE;
           break;

       default:

           //
           // This will get handled in RunPhase.
           //
           PC2DebugPrint(0x01, (0, "N/A\n"));
           break;
   }
} // end PC2xDetermineNextState()



/*************************************************************************
* Routine Name:
*
*    PC2xSelect
*
* Routine Description:
* 
*    Perform selection process on SCSI bus.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    None
*/
VOID
PC2xSelect(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   PC2DebugPrint(0x01,
                 (0, "PC2Select: Dev= %x, TID = %x ",
                 DeviceExtension,
                 DeviceExtension->ActiveLu->ActiveLuRequest->TargetId));

   PC2X_WRITE_DATA(DeviceExtension->BaseAddress,
                    (1 << SCSI_INITIATOR_ID) |
                    (1 << (DeviceExtension->ActiveLu->ActiveLuRequest->TargetId)));

   ScsiPortStallExecution(2);  // Delay 1200 nanoseconds (Bus-Settle + Bus-Clear).

   PC2xWriteControl(DeviceExtension, C_SELECT );

   ScsiPortStallExecution(1);  // Delay 90 nanoseconds (2 * Bus-Deskew).

   if (PC2xStatusCheck(DeviceExtension,
                        S_BUSY,
                        S_BUSY,
                        SELECTION_DELAY)) {

       ScsiPortStallExecution(1);  // Delay 90 nanoseconds (2 * Bus-Deskew).

       //
       // Selection is Ok.  Negate Select.
       //
       PC2xWriteControl(DeviceExtension, C_IDLE );

       ScsiPortStallExecution(1);  // Delay 90 nanoseconds (2 * Bus-Deskew).

       DeviceExtension->ActiveLu->LuState = LS_UNDETERMINED;
        // The next state is undetermined here because some old Alpha drives
        // want to drive the bus directly from Sel to Status, then Msg_in!

       PC2DebugPrint(0x01,
                     (0, "SELECT OK %x\n",
                     DeviceExtension->ActiveLu->ActiveLuRequest->TargetId));
       return;
   }

   //
   // Selection failed.  Force SCSI bus back to Bus-Free.
   //
   PC2xWriteControl(DeviceExtension, C_IDLE);

   PC2DebugPrint(0x01,
                 (0, "SELECT FAILED %x\n",
                 DeviceExtension->ActiveLu->ActiveLuRequest->TargetId));

   DeviceExtension->ActiveLu->LuState = LS_COMPLETE;
   DeviceExtension->ActiveLu->ActiveLuRequest->SrbStatus =
                                               SRB_STATUS_SELECTION_TIMEOUT;
} // end PC2xSelect()



/*************************************************************************
* Routine Name:
*
*    PC2xSendCDB
*
* Routine Description:
* 
*    Send the SCSI Command Descriptor Block (CDB) to the indicated target/lun.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    None
* 
*/
VOID
PC2xSendCDB(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   UCHAR   cdbLength = DeviceExtension->ActiveLu->ActiveLuRequest->CdbLength;
   PUCHAR  cdb = DeviceExtension->ActiveLu->ActiveLuRequest->Cdb;
   PUCHAR  baseAddress = DeviceExtension->BaseAddress;

   PC2DebugPrint(0x002, (0, "\nSendCommand: "));

   PC2DebugPrint(0x002, (0, "%x \n", *cdb));

   while ((cdbLength-- != 0) &&
          PC2xWaitForRequestLine(DeviceExtension) &&
          (PC2X_READ_PHASE(baseAddress) == BP_COMMAND)) {

       PC2DebugPrint(0x02, (0, "%x ", *cdb));

       PC2X_WRITE_DATA(baseAddress,
                        *cdb++);
   }

   DeviceExtension->ActiveLu->LuState = LS_UNDETERMINED;

   //
   // Set up the running data pointer info for a possible data transfer.
   //
   DeviceExtension->CurDataPointer = DeviceExtension->ActiveLu->SavedDataPointer;
   DeviceExtension->CurDataLength = DeviceExtension->ActiveLu->SavedDataLength;

   PC2DebugPrint(0x02, (0, "New phase=%x\n", PC2X_READ_PHASE(baseAddress)));

} // end PC2xSendCDB()


/*************************************************************************
* Routine Name:
*
*    PC2xDataPhase
*
* Routine Description:
* 
*    This routine sets the DmaPending flag in the device extension,
*    calls the OS-specific driver to set up the system DMA chip,
*    and returns. Control is received again in DmaStarted when the DMA
*    setup is complete.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    TRUE if DMA started
*    FALSE if missed request
* 
*/
BOOLEAN
PC2xDataPhase(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   PUCHAR  baseAddress = DeviceExtension->BaseAddress;
   
   if (PC2xWaitForRequestLine(DeviceExtension) == FALSE) {

       DeviceExtension->ActiveLu->LuState = LS_UNDETERMINED;
       DeviceExtension->DmaPending = FALSE;
       return(FALSE);
   }

   PC2DebugPrint(0x04, (0, "PC2DataPhase: CurSRB=%x\n",DeviceExtension->ActiveLu->ActiveLuRequest));
   PC2DebugPrint(0x04, (0, "PC2DataPhase: CurDataPointer=%x\n",DeviceExtension->CurDataPointer));
   PC2DebugPrint(0x04, (0, "PC2DataPhase: CurDataLength=%lx\n",DeviceExtension->CurDataLength));

   DeviceExtension->DmaPending = TRUE;

   ScsiPortIoMapTransfer (DeviceExtension,
                          DeviceExtension->ActiveLu->ActiveLuRequest,
                          (PVOID)DeviceExtension->CurDataPointer,
                          DeviceExtension->CurDataLength );

	return(TRUE);

} // end PC2xDataPhase()


/*************************************************************************
* Routine Name:
*
*    PC2xStatus
*
* Routine Description:
* 
*    This routine will obtain the status from the target.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    None
* 
*/
VOID
PC2xStatus(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   UCHAR               status;
   UCHAR               srbStatus;
   PSCSI_REQUEST_BLOCK srb = DeviceExtension->ActiveLu->ActiveLuRequest;

   if (PC2xWaitForRequestLine(DeviceExtension) == FALSE) {

       DeviceExtension->ActiveLu->LuState = LS_UNDETERMINED;
       return;
   }

   status = ( PC2X_READ_DATA(DeviceExtension->BaseAddress) &0xf );

   //
   // Save this away for the driver above.
   //
   srb->ScsiStatus = status;

   PC2DebugPrint(0x02, (0, "\nPC2Status: Returned status byte=%x\n", status));

   switch (status) {

       case SCSISTAT_GOOD:
       case SCSISTAT_CONDITION_MET:
       case SCSISTAT_INTERMEDIATE:
       case SCSISTAT_INTERMEDIATE_COND_MET:

           srbStatus = SRB_STATUS_SUCCESS;
           break;

       case SCSISTAT_CHECK_CONDITION:
       case SCSISTAT_COMMAND_TERMINATED:

           srbStatus = SRB_STATUS_ERROR;
           break;

       case SCSISTAT_BUSY:
       case SCSISTAT_RESERVATION_CONFLICT:
       case SCSISTAT_QUEUE_FULL:
       default:

           srbStatus = SRB_STATUS_BUSY;
           break;

   }

   //
   // If some error condition already occurred (e.g., parity error), we'll
   // let that one take priority.
   //
   if (srb->SrbStatus == SRB_STATUS_PENDING)
       srb->SrbStatus = srbStatus;


   DeviceExtension->ActiveLu->LuState = LS_UNDETERMINED;

} // end PC2xStatus()


/*************************************************************************
* Routine Name:
*
*    PC2xMessageIn
*
* Routine Description:
* 
*    This routine will receive the message from the target.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    None
* 
*/
VOID
PC2xMessageIn(
    IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
    )
{
   PSPECIFIC_LU_EXTENSION  luExtension = DeviceExtension->ActiveLu;
   PSCSI_REQUEST_BLOCK     srb = luExtension->ActiveLuRequest;
   PUCHAR                  baseAddress = DeviceExtension->BaseAddress;
   UCHAR                   msg;

   
   if (PC2xWaitForRequestLine(DeviceExtension) == FALSE) {

       DeviceExtension->ActiveLu->LuState = LS_UNDETERMINED;
       return;
   }


   luExtension->LuState = LS_COMPLETE;

   msg = PC2X_READ_DATA(baseAddress);

   PC2DebugPrint(0x08, (0, "PC2MessageIn: message=%x -- ", msg));

   return;

} // end PC2xMessageIn()


/*************************************************************************
* Routine Name:
*
*    PC2xNotifyCompletion
*
* Routine Description:
* 
*    This routine will perform any clean up operations for the Srb
*    and notify the ScsiPort driver of completion.
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    None
* 
*/
VOID
PC2xNotifyCompletion(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   PSPECIFIC_LU_EXTENSION     luExtension = DeviceExtension->ActiveLu;
   PSCSI_REQUEST_BLOCK        srb = luExtension->ActiveLuRequest;

   PC2DebugPrint(0x100,
        (0, "\nPC2Complete Dev = %x, Srb = %x, Srbstat = %x, Scsistat = %x\n",
            DeviceExtension,
            srb,
            srb->SrbStatus,
            srb->ScsiStatus));

   luExtension->ActiveLuRequest = NULL;
   DeviceExtension->ActiveLu = NULL;

   //
   // Call notification routine.
   //
   ScsiPortNotification(RequestComplete,
                        (PVOID) DeviceExtension,
                        srb);

   //
   // Adapter ready for next request.
   //
   ScsiPortNotification(NextRequest,
                        DeviceExtension,
                        NULL);

} // end PC2xNotifyCompletion()



/*************************************************************************
* Routine Name:
*
*    PC2xRunPhase
*
* Routine Description:
* 
*    This routine runs through the bus phases until some type of completion
*    indication is received.  
* 
* Arguments:
* 
*    DeviceExtension - Device adapter context pointer.
* 
* Return Value:
* 
*    None.
* 
*/
VOID
PC2xRunPhase(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension
   )
{
   PUCHAR  baseAddress = DeviceExtension->BaseAddress;
   ULONG   undetermined = 0;


   PC2DebugPrint(0x10, (0, "\nRunPhase Entered...\n"));


   while ( DeviceExtension->ActiveLu != NULL ) {

       // We seem to get ahead of the PC2 here if we check the status
       // too quickly. This needs further investigation. 
       // Until we know why, stall for 256 microseconds between phase 
       // changes. -tmt-
       ScsiPortStallExecution(256);

       switch (DeviceExtension->ActiveLu->LuState) {


           case LS_UNDETERMINED:

               PC2xDetermineNextState(DeviceExtension);

               if (undetermined++ == REQUEST_SPIN_WAIT) {

	               PC2DebugPrint(0x200, (0, "NO REQUEST\n"));
                  goto PC2xRunPhase_PhaseSequenceFailure;
               }
               break;

           case LS_SELECT:
               PC2xSelect(DeviceExtension);
               break;

           case LS_COMMAND:
               PC2xSendCDB(DeviceExtension);
               break;

           case LS_DATA:
					// 
             	// If TRUE, we started a DMA transfer.
					//	Return with SRB_STATUS_PENDING...
					//
               if (PC2xDataPhase(DeviceExtension) == TRUE) {
						return;
               }
					// Otherwise, we missed the Request line, continue.
               break;

           case LS_STATUS:
               PC2xStatus(DeviceExtension);
               break;

           case LS_MSG_IN:
               PC2xMessageIn(DeviceExtension);
               break;
         
           case LS_MSG_OUT:
               break;

           case LS_COMPLETE:
               PC2xNotifyCompletion(DeviceExtension);
               break;

           default:

               PC2xRunPhase_PhaseSequenceFailure:

               DeviceExtension->ActiveLu->LuState = LS_COMPLETE;
               DeviceExtension->ActiveLu->ActiveLuRequest->SrbStatus =
                                       SRB_STATUS_PHASE_SEQUENCE_FAILURE;

               ScsiPortLogError(DeviceExtension,
                       DeviceExtension->ActiveLu->ActiveLuRequest,
                       DeviceExtension->ActiveLu->ActiveLuRequest->PathId,
                       DeviceExtension->ActiveLu->ActiveLuRequest->TargetId,
                       DeviceExtension->ActiveLu->ActiveLuRequest->Lun,
                       SP_PROTOCOL_ERROR,
                       (ULONG) DeviceExtension);
               break;
       }
   }

   PC2DebugPrint(0x10, (0, "\nRunPhase Exit...\n"));


} // end PC2xRunPhase()


/*************************************************************************
* Routine Name:
*
*    PC2xTimer
*
* Routine Description:
*     This routine requests a "one-shot" timer call from the port driver.
*     This is used as a backup for the interrupt. Timer calls the ISR
*     and if the interrupt is claimed, ends. If the interrupt is not
*     claimed, the timer is rescheduled.
*
* Arguments:
* 
*     Context - Device adapter context pointer.
* 
* Return Value
* 
*     TRUE indicates that the interrupt was from this PC2x adapter,
*     FALSE indicates that this interrupt was NOT from us.
* 
*/
VOID
PC2xTimer(
   IN PVOID Context
   )
{
   PSPECIFIC_DEVICE_EXTENSION deviceExtension = Context;
   BOOLEAN                    restartTimer = TRUE;

   if (deviceExtension->InterruptPending == FALSE) {
       return;
   }

   restartTimer = PC2xInterrupt(Context) == FALSE ? TRUE : FALSE;

   if (restartTimer == TRUE) {

       //
       // Not claimed.  Set timer for another call.
       //
       ScsiPortNotification(RequestTimerCall,
                            deviceExtension,
                            PC2xTimer,
                            PC2X_TIMER_VALUE);
   }

} //end PC2xTimer()



/*************************************************************************
* Routine Name:
*
*    PC2xInterrupt
*
* Routine Description:
* 
*    This routine handles the interrupts for the PC2X.  The intention is to
*    quickly determine the cause of the interrupt, clear the interrupt, and
*    setup to process the SCSI command that is affected by the
*    interrupt.
* 
* Arguments:
* 
*    Context - Device adapter context pointer.
* 
* Return Value:
* 
*    TRUE indicates that the interrupt was from this PC2x adapter,
*    FALSE indicates that this interrupt was NOT from us.
* 
*/
BOOLEAN
PC2xInterrupt(
   IN PVOID Context
   )
{
   PSPECIFIC_DEVICE_EXTENSION deviceExtension = Context;

   PC2DebugPrint(0x20,
                 (0, "\nPC2Interrupt: Dev= %x ",
                 deviceExtension));

   if (PC2xStatusCheck(deviceExtension,
                        BP_STATUS,
                        BP_STATUS,
                        1) == FALSE) {

       //
       // Spurious interrupt or for some other device.
       //
       PC2DebugPrint(0x800,
                     (0, "Interrupt Denied: Status = %x, Last CtrlReg = %x\n",
                     PC2X_READ_STATUS(deviceExtension->BaseAddress),
                     deviceExtension->ControlRegister));
       PC2DebugPrint(0x800,(0, ".",deviceExtension));

       return FALSE;

   } else {
       PC2DebugPrint(0x20,
                   (0, "Interrupt Claimed: Status = %x, Last CtrlReg = %x",
                   PC2X_READ_STATUS(deviceExtension->BaseAddress),
                   deviceExtension->ControlRegister));


       deviceExtension->InterruptPending = FALSE;

       // Disable interrupts, clear DMA_ENABLE
       PC2xWriteControl(deviceExtension, C_IDLE);
       // Clear interrupt
       PC2X_READ_STATUS(deviceExtension->BaseAddress);
   
       ScsiPortFlushDma(deviceExtension);

		 // BUGCHECK!!!!
		 // At this point, we need to check for any leftover bytes that
       // the DMA xfer might have missed. Not sure yet how to do this...
		 // Microsoft claims that this info isn't available to the miniport.
		 
   
       deviceExtension->ActiveLu->LuState = LS_UNDETERMINED;
       
       PC2xRunPhase(deviceExtension);

       return TRUE;
   }
} // end PC2xInterrupt()



/*************************************************************************
* Routine Name:
*
*    PC2xDmaStarted
*
* Routine Description:
* 
*    This routine sets up the PC2x for DMA data transfer. This routine is
*    called by the OS specific port driver when the DMA chip has been
*    initialized from the DataPhase routine. 
* 
* Arguments:
* 
*    Context - Device adapter context pointer.
* 
* Return Value:
* 
*    Nothing
* 
*/
VOID
PC2xDmaStarted(
   IN PVOID Context
   )
{
   PSPECIFIC_DEVICE_EXTENSION deviceExtension = Context;

   PC2DebugPrint(0x04, (0, "PC2: Entered DMA Started\n"));

   //
   // If no data tranfer is expected then ignore the notification.
   //
   if (deviceExtension->DmaPending != TRUE) {
      return;
   }
   deviceExtension->DmaPending = FALSE;                              
   deviceExtension->InterruptPending = TRUE;


// Keep interrupts disabled and poll with timer. I see no reason to tie up
// an IRQ resource for this card.
// This adapter probably performs as well in 
// NT polling mode as it does using interrupts...  (Poorly)

   PC2xWriteControl(deviceExtension, C_DMA_ENABLE);

   PC2DebugPrint(0x04, (0, "PC2: Waiting for Interrupt...\n"));

   ScsiPortNotification(RequestTimerCall,
                        deviceExtension,
                        PC2xTimer,
                        PC2X_TIMER_VALUE);

} // end PC2xDmaStarted()


/*************************************************************************
* Routine Name:
*
*    PC2xStartExecution
*
* Routine Description:
* 
*    This routine will start the execution of a SCSI request.
* 
* Arguments:
* 
*    DeviceExtension -   Device adapter context pointer.
*    LuExtension -       The logical unit specific information.
*    Srb -               The Srb command to execute.
* 
* Return Value:
* 
*    None.
* 
*/
VOID
PC2xStartExecution(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension,
   IN PSPECIFIC_LU_EXTENSION     LuExtension,
   IN PSCSI_REQUEST_BLOCK        Srb
   )
{
   PC2DebugPrint(0x10,
                 (0, "\nPC2StartEx Dev= %x, LuExt= %x, Srb= %x ",
                 DeviceExtension,
                 LuExtension,
                 Srb));
   PC2DebugPrint(0x10,
                 (0, "ID = %x, Lun = %x.\n",
                 Srb->TargetId,
                 Srb->Lun));

   //
   // Setup the context for this adapter.
   //
   DeviceExtension->ActiveLu = LuExtension;
   
   // Check for Bus Free
   if (PC2xStatusCheck(DeviceExtension,
                        S_DMA,
                        S_DMA, BUS_FREE_DELAY) == FALSE) {

      Srb->SrbStatus = SRB_STATUS_TIMEOUT;
      PC2DebugPrint(0x400,(0, "Bus Free Timeout!\n"));

      ScsiPortNotification(RequestComplete,
                           (PVOID) DeviceExtension,
                           Srb);
      ScsiPortNotification(NextRequest,
                           (PVOID) DeviceExtension,
                           NULL);
      return;

   }


   PC2xRunPhase(DeviceExtension);
} // end PC2xStartExecution()



/*************************************************************************
* Routine Name:
*
*    PC2xAbort
*
* Routine Description:
* 
*    Attempt to abort a command on a SCSI target. Since there is no way to
*    abort a command on the PC2, (no message support) this routine can only
*    return with an error.
* 
* Arguments:
* 
*    DeviceExtension -   The adapter specific information.
*    LuExtension     -   The specific target/logical unit information.
*    Srb -               The Srb command to execute.
* 
* Return Value:
* 
*     None.
* 
*/
VOID
PC2xAbort(
   IN PSPECIFIC_DEVICE_EXTENSION DeviceExtension,
   IN PSPECIFIC_LU_EXTENSION     LuExtension,
   IN PSCSI_REQUEST_BLOCK        Srb
   )
{
   PUCHAR              baseAddress = DeviceExtension->BaseAddress;
   PSCSI_REQUEST_BLOCK srbBeingAborted = LuExtension->ActiveLuRequest;

   PC2DebugPrint(0x10, (0, "PC2Abort: failed.\n"));

   Srb->SrbStatus = SRB_STATUS_ABORT_FAILED;


} // end PC2xAbort()


/*************************************************************************
* Routine Name:
*
*    PC2xResetBus
*
* Routine Description:
* 
*    Reset PC2 SCSI adapter (no action for this)
*    and SCSI bus.
* 
* Arguments:
* 
*    Context for the reset.
*    PathId
* 
* Return Value:
* 
*    Nothing.
* 
*/
BOOLEAN
PC2xResetBus(
   IN PVOID Context,
   IN ULONG PathId
   )
{
   PSPECIFIC_DEVICE_EXTENSION deviceExtension = Context;

   PC2DebugPrint(0x800, (0, "PC2ResetBus: Reset PC2x and SCSI bus\n"));

   if (deviceExtension->DmaPending == TRUE) {
      deviceExtension->DmaPending = FALSE;
      ScsiPortFlushDma(deviceExtension);
   }
   deviceExtension->InterruptPending = FALSE;

   //
   // RESET SCSI bus.
   //
   PC2xWriteControl(deviceExtension, C_IDLE);
   PC2X_READ_STATUS(deviceExtension->BaseAddress);
   PC2xWriteControl(deviceExtension, C_RESET);
   ScsiPortStallExecution(RESET_HOLD_TIME);

   //
   // Complete all outstanding requests with SRB_STATUS_BUS_RESET.
   //

   ScsiPortFlushDma(deviceExtension);

   ScsiPortCompleteRequest(deviceExtension,
                           (UCHAR) PathId,
                           (UCHAR) -1,
                           (UCHAR) -1,
                           SRB_STATUS_BUS_RESET);

   ScsiPortNotification(NextRequest,
                           deviceExtension,
                           NULL);

   return TRUE;
} // end PC2xResetBus()


/*************************************************************************
* Routine Name:
*
*    PC2xStartIo
*
* Routine Description:
* 
*    This routine is called from the SCSI port driver synchronized
*    with the kernel with a request to be executed.
* 
* Arguments:
*    Context -   The adapter specific information.
*    Srb -       The Srb command to execute.
* 
* Return Value:
* 
*    TRUE
* 
*/
BOOLEAN
PC2xStartIo(
   IN PVOID               Context,
   IN PSCSI_REQUEST_BLOCK Srb
   )
{
   PSPECIFIC_DEVICE_EXTENSION  deviceExtension = Context;
   PSPECIFIC_LU_EXTENSION      luExtension;

   PC2DebugPrint(0x10,
                 (0, "\nPC2StartIo: Dev= %x, Srb = %x\n",
                 deviceExtension,
                 Srb));

   //
   // Determine the logical unit that this request is for.
   //
   deviceExtension->PathId = Srb->PathId;
   luExtension = ScsiPortGetLogicalUnit(deviceExtension,
                                        deviceExtension->PathId,
                                        Srb->TargetId,
                                        Srb->Lun);
   Srb->SrbStatus = SRB_STATUS_PENDING;

   switch (Srb->Function) {

       case SRB_FUNCTION_ABORT_COMMAND:

           PC2DebugPrint(0x10, (0, "ABORT COMMAND.\n"));
           PC2xAbort(deviceExtension, luExtension, Srb);
           //
           // Adapter ready for next request.
           //
           ScsiPortNotification(NextRequest,
                                deviceExtension,
                                NULL);
           break;

       case SRB_FUNCTION_RESET_BUS:

           PC2DebugPrint(0x10, (0, "RESET BUS.\n"));
           //
           // Reset PC2x and SCSI bus.
           //
           PC2xResetBus(deviceExtension, Srb->PathId);
           Srb->SrbStatus = SRB_STATUS_SUCCESS;
           //
           // "next request" notification is handled in PC2xResetBus
			  //

           break;

       case SRB_FUNCTION_EXECUTE_SCSI:

           PC2DebugPrint(0x10, (0, "EXECUTE SCSI.\n"));

           //
           // Setup the context for this target/lun.
           //
           luExtension->ActiveLuRequest = Srb;
           luExtension->LuState = LS_SELECT;
           luExtension->SavedDataPointer = (ULONG) Srb->DataBuffer;
           luExtension->SavedDataLength = Srb->DataTransferLength;

           //
           // Initiate a SCSI request.
           //
           PC2xStartExecution(deviceExtension, luExtension, Srb);
           break;

       default:

           Srb->SrbStatus = SRB_STATUS_INVALID_REQUEST;
           ScsiPortNotification(RequestComplete,
                                (PVOID) deviceExtension,
                                Srb);
           //
           // Adapter ready for next request.
           //
           ScsiPortNotification(NextRequest,
                                deviceExtension,
                                NULL);
           break;

   }

   return TRUE;

} // end PC2xStartIo()


/*************************************************************************
* Routine Name:
*
*    PC2xInitialize
*
* Routine Description:
* 
*    Inititialize PC2x adapter.
* 
* Arguments:
* 
*    Context - Adapter object device extension.
* 
* Return Value:
* 
*    Status.
* 
*/
BOOLEAN
PC2xInitialize(
   IN PVOID Context
   )
{
   PSPECIFIC_DEVICE_EXTENSION deviceExtension = Context;

   //
   // Reset PC2x and SCSI bus.
   //

   PC2DebugPrint(0x20, (0, "\nPC2Init Entered...\n"));
   
   PC2xWriteControl(deviceExtension, C_IDLE);
   PC2X_READ_STATUS(deviceExtension->BaseAddress);
   PC2xWriteControl(deviceExtension, C_RESET);
   ScsiPortStallExecution(RESET_HOLD_TIME);

   ScsiPortNotification(ResetDetected,
                        (PVOID) deviceExtension);

   deviceExtension->InterruptPending = FALSE;
   deviceExtension->DmaPending = FALSE;

   return TRUE;
} // end PC2xInitialize()


/*************************************************************************
* Routine Name:
*
*    PC2xCheckBaseAddress
*
* Routine Description:
* 
*    This routine will check to see if there is an adapter at the
*    provided base address.  It does this by asserting and clearing
*    the DMA bit in the control register while checking the DMA bit in
*    the status register. If the DMA bit can be wiggled both ways, then
*    there is a PC2x adapter at that base address.
* 
* Arguments:
* 
*    DeviceExtension - Necessary to attempt the reads.
*    BaseAddress     - The address to attempt to use for the base of the
*                      port
* 
* Return Value:
* 
*    TRUE    - There is an adapter present at the base address provided.
*    FALSE   - There is no adapter present at the base address provided.
* 
*/
BOOLEAN
PC2xCheckBaseAddress(
   IN PSPECIFIC_DEVICE_EXTENSION deviceExtension,
   IN PUCHAR                     BaseAddress
   )
{
   UCHAR  testValue;
   UCHAR  oldValue;
   USHORT offset;

   PC2xWriteControl(deviceExtension, C_DMA_ENABLE);
   if (PC2X_READ_STATUS(BaseAddress) == 0)
   {
       PC2xWriteControl(deviceExtension, C_IDLE);
       if (PC2X_READ_STATUS(BaseAddress) == S_DMA)
       {
           PC2DebugPrint(0x20, (0, "\nPresence test passed. \n"));
           return TRUE;
           
       }
   }
   
   PC2xWriteControl(deviceExtension, C_IDLE);
   PC2DebugPrint(0x20, (0, "\nPresence test failed. \n"));
   return FALSE;

} // end PC2xCheckBaseAddress()



/*************************************************************************
* Routine Name:
*
*    PC2xDetermineInstalled
*
* Routine Description:
* 
*    This routine is called by PC2xFindAdapter and attempts to find
*    the adapter boards in the system by checking each base I/O address.
*    If an adapter is found, the BaseAddress is initialized.
* 
* Arguments:
* 
*    DeviceExtension -   The device specific context for the call.
*    ConfigInfo      -   Pointer to the configuration information structure to
*                        be filled in.
*    AdapterCount    -   Supplies the count of adapter slots which have been tested.
*    Again           -   Returns back a request to call this function again.
* 
* Return Value:
* 
*    SP_RETURN_FOUND     - if an adapter is found.
*    SP_RETURN_NOT_FOUND - if no adapter is found.
* 
*/
ULONG
PC2xDetermineInstalled(
   PSPECIFIC_DEVICE_EXTENSION              DeviceExtension,
   IN OUT PPORT_CONFIGURATION_INFORMATION  ConfigInfo,
   IN OUT PULONG                           AdapterCount,
   OUT PBOOLEAN                            Again
   )
{
   PUCHAR         baseAddress;
   PUCHAR         ioSpace;
   ULONG          i;
   ULONG          j;
   ULONG          basePort;
   ULONG          numPorts;

	//
	// Honor passed-in config info for Chicago, and only check
	// that port.
	//
	basePort = ScsiPortConvertPhysicalAddressToUlong((*ConfigInfo->AccessRanges)[0].RangeStart);
   if (basePort != 0) {
		numPorts = 1;
   }
	// If nothing passed-in, scan entire range...
	else {
		basePort = BASE_PORT;
		numPorts = NUMBR_PORTS;
	}

   for (i=basePort, j=0 ; j < numPorts ; i+=BASE_WIDTH, j++ ) {

      ioSpace = ScsiPortGetDeviceBase(
                DeviceExtension,
                ConfigInfo->AdapterInterfaceType,
                ConfigInfo->SystemIoBusNumber,
                ScsiPortConvertUlongToPhysicalAddress(i),
                BASE_WIDTH,       // NumberOfBytes
                (BOOLEAN) TRUE);  // InIoSpace

      baseAddress = (PUCHAR)(ioSpace);
      DeviceExtension->BaseAddress = baseAddress;

      if (PC2xCheckBaseAddress(DeviceExtension, baseAddress) == TRUE) {

           //
           // We found an adapter at this baseAddress.
           //
           DeviceExtension->BaseAddress = baseAddress;

           *Again = FALSE; // Get it working with one adapter

           //
           // Fill in the access array information.
           //
           (*ConfigInfo->AccessRanges)[0].RangeStart =
               ScsiPortConvertUlongToPhysicalAddress((ULONG)baseAddress);
           (*ConfigInfo->AccessRanges)[0].RangeLength = NUMBR_PORTS;
           (*ConfigInfo->AccessRanges)[0].RangeInMemory = FALSE;
           
           PC2DebugPrint(0x40, (0, "\nPC2FindAdp:RangeStrt=%x\n",(*ConfigInfo->AccessRanges)[0].RangeStart));
           PC2DebugPrint(0x40, (0, "\nPC2FindAdp:RangeLen=%x\n",(*ConfigInfo->AccessRanges)[0].RangeLength));


           *(AdapterCount)++;

           return(SP_RETURN_FOUND);

      } // end if

      ScsiPortFreeDeviceBase( DeviceExtension, ioSpace );

   } // end for

   // The entire table has been searched and no adapters found. 
   // No need to call again.
   // Clear the adapter count for the next bus.

   *Again = FALSE;
   *(AdapterCount) = 0;

   return(SP_RETURN_NOT_FOUND);

} // end PC2xDetermineInstalled()


/*************************************************************************
* Routine Name:
*
*    PC2xFindAdapter
*
* Routine Description:
*    Called by the OS-specific port driver after the necessary storage has
*    been allocated, to gather information about the adapter's configuration.
*       
* Arguments:
* 
*    DeviceExtension -   The device specific context for the call.
*    Context   -         Passed through from the driver entry as additional
*                        context for the call.
*    BusInformation  -   Unused.
*    ArgumentString  -   Points to the potential IRQ for this adapter.
*    ConfigInfo      -   Pointer to the configuration information structure to
*                        be filled in.
*    Again           -   Returns back a request to call this function again.
* 
* Return Value:
* 
*    SP_RETURN_FOUND     - if an adapter is found.
*    SP_RETURN_NOT_FOUND - if no adapter is found.
* 
*/
ULONG
PC2xFindAdapter(
   PSPECIFIC_DEVICE_EXTENSION              DeviceExtension,
   IN PVOID                                Context,
   IN PVOID                                BusInformation,
   IN PCHAR                                ArgumentString,
   IN OUT PPORT_CONFIGURATION_INFORMATION  ConfigInfo,
   OUT PBOOLEAN                            Again
   )
{
   PSPECIFIC_DEVICE_EXTENSION  deviceExtension = DeviceExtension;
   ULONG   status;
   PUCHAR  ioSpace;
   PUCHAR  baseAddress;
   UCHAR   switches;

   PC2DebugPrint(0x20, (0, "\nFindAdapter Entered...\n"));

   status = PC2xDetermineInstalled(deviceExtension,
               ConfigInfo,
               Context,
               Again);

   if (status != SP_RETURN_FOUND) {
       return(status);
   }

   baseAddress = deviceExtension->BaseAddress;
   
   // Determine DMA Channel
   switches = PC2X_READ_SWITCHES(baseAddress);
   
   switch ((switches >> 2) & 0x01) {
       case 0:
           ConfigInfo->DmaChannel = 1;
           break;
       case 1:
           ConfigInfo->DmaChannel = 3;
           break;
   }
   ConfigInfo->DmaWidth = Width8Bits;

   // Determine IRQ
   // Note: In future release of NT (after .404 March Beta), this value
   //       can be returned as 0 and NT will assume we are polling and won't
   //       reserve an IRQ for us. Until then, we need to give him something.
   switches = PC2X_READ_SWITCHES(baseAddress);
   

   switch ((switches >> 5) & 0x01) {
       case 0:
           ConfigInfo->BusInterruptLevel = 5;
           break;
       case 1:
           ConfigInfo->BusInterruptLevel = 7;
           break;
   }

	// See Note Above
   // ConfigInfo->BusInterruptLevel = 0;

   PC2DebugPrint(0x40, (0, "\nPC2:IRQ=%x\n", ConfigInfo->BusInterruptLevel));
   PC2DebugPrint(0x40, (0, "PC2:DMA=%x\n", ConfigInfo->DmaChannel));

   ConfigInfo->ScatterGather = FALSE;
   ConfigInfo->Master = FALSE;
   ConfigInfo->Dma32BitAddresses = FALSE;
   ConfigInfo->DemandMode = TRUE;
   ConfigInfo->NumberOfBuses = 1;
   ConfigInfo->InitiatorBusId[0] = SCSI_INITIATOR_ID;
   ConfigInfo->MaximumTransferLength = MAX_TRANSFER_LENGTH;
   ConfigInfo->NumberOfPhysicalBreaks = 1;
   ConfigInfo->InterruptMode = Latched;

   PC2DebugPrint(0x20, (0, "Exit FindAdapter.\n"));

   return(SP_RETURN_FOUND);

} // end PC2xFindAdapter()



/*************************************************************************
* Routine Name:
*
*    DriverEntry
*
* Routine Description:
* 
*    Driver initialization entry point for system.
* 
* Arguments:
* 
*    DriverObject - The driver specific object pointer
*    Argument2    - not used.
* 
* Return Value:
* 
*    Status from ScsiPortInitialize()
* 
*/
ULONG
DriverEntry(
   IN PVOID DriverObject,
   IN PVOID Argument2
   )
{
   HW_INITIALIZATION_DATA  hwInitializationData;
   ULONG                   i;
   ULONG                   rc;

   PC2DebugPrint(0x20, (0, "\nIomega PC2x 8-bit SCSI Miniport Driver\n"));

   //
   // Zero out the hwInitializationData structure.
   //
   for (i = 0; i < sizeof(HW_INITIALIZATION_DATA); i++) {

       *(((PUCHAR)&hwInitializationData + i)) = 0;
   }

   hwInitializationData.HwInitializationDataSize = sizeof(HW_INITIALIZATION_DATA);

   //
   // Set entry points.
   //
   hwInitializationData.HwInitialize   = PC2xInitialize;
   hwInitializationData.HwStartIo      = PC2xStartIo;
   hwInitializationData.HwInterrupt    = PC2xInterrupt;
   hwInitializationData.HwFindAdapter  = PC2xFindAdapter;
   hwInitializationData.HwResetBus     = PC2xResetBus;
   hwInitializationData.HwDmaStarted   = PC2xDmaStarted;

   //
   // Specify size of device extension.
   //
   hwInitializationData.DeviceExtensionSize = sizeof(SPECIFIC_DEVICE_EXTENSION);

   //
   // Specify size of logical unit extension.
   //
   hwInitializationData.SpecificLuExtensionSize = sizeof(SPECIFIC_LU_EXTENSION);

   hwInitializationData.NumberOfAccessRanges   = 1;

   hwInitializationData.MapBuffers             = FALSE;
   hwInitializationData.NeedPhysicalAddresses  = FALSE;

   hwInitializationData.TaggedQueuing          = FALSE;
   hwInitializationData.AutoRequestSense       = FALSE;
   hwInitializationData.MultipleRequestPerLu   = FALSE;
   hwInitializationData.ReceiveEvent           = FALSE;

   //
   // The fourth parameter below (i.e., "i") will show up as the
   // "Context" parameter when FindAdapter() is called.
   //

   PC2DebugPrint(0x20, (0, "Trying ISA...\n"));
   hwInitializationData.AdapterInterfaceType = Isa;

   i = 0;
   rc = (ScsiPortInitialize(DriverObject,
                          Argument2,
                          &hwInitializationData,
                          &(i) ) );

   PC2DebugPrint(0x20, (0, "Exit DriverEntry. rc=%x\n", rc));

   return(rc);

} // end DriverEntry()

