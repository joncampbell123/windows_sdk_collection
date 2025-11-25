/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    q117i_nt.c

Abstract:

    NT device driver entry point,  NT interface routines,
    and thread for low-level irp functions.

Revision History:




--*/

#include "ntddk.h"                        // various NT definitions
#include "ntdddisk.h"                    // disk device driver I/O control codes
#include "common.h"
#include "drvtask.h"                    // this driver's data declarations
#include "mt1defs.h"                    // this driver's data declarations
#include "mt1strc.h"                    // this driver's data declarations
#include "q117data.h"               // this driver's data declarations
#include "hilevel.h"

//
// This is the actual definition of FloppyDebugLevel.
// Note that it is only defined if this is a "debug"
// build.
//
#if DBG
// extern ULONG QIC117DebugLevel = QIC117DBGP | QIC117WARN | QIC117INFO |
//                                QIC117SHOWTD | QIC117SHOWQD |
//                                QIC117SHOWPOLL | QIC117STOP |
//                                QIC117MAKEBAD | QIC117SHOWBAD |
//                                QIC117DRVSTAT;

extern ULONG QIC117DebugLevel = 0x00000000;
//extern ULONG QIC117DebugLevel =  QIC117SHOWTD | QIC117DBGP | QIC117WARN | QIC117INFO;

#endif

#ifdef ALLOC_PRAGMA
#pragma alloc_text(init,DriverEntry)
#endif


NTSTATUS
DriverEntry(
   IN OUT PDRIVER_OBJECT DriverObject,
   IN PUNICODE_STRING RegistryPath
   )

/*++

Routine Description:

   This routine is the driver's entry point, called by the I/O system
   to load the driver. This routine can be called any number of times,
   as long as the IO system and the configuration manager conspire to
   give it an unmanaged controller to support at each call.    It could
   also be called a single time and given all of the controllers at
   once.

   It initializes the passed-in driver object, calls the configuration
   manager to learn about the devices that it is to support, and for
   each controller to be supported it calls a routine to initialize the
   controller (and all drives attached to it).

Arguments:

   DriverObject - a pointer to the object that represents this device
   driver.

Return Value:

   If we successfully initialize at least one drive, STATUS_SUCCESS is
   returned.

   If we don't (because the configuration manager returns an error, or
   the configuration manager says that there are no controllers or
   drives to support, or no controllers or drives can be successfully
   initialized), then the last error encountered is propogated.

--*/

{
   PCONFIG_DATA configData;            // pointer to config mgr's returned data
   NTSTATUS ntStatus;
   UCHAR controllerNumber;
   BOOLEAN partlySuccessful = FALSE;   // TRUE if any controller init'd properly

   UNREFERENCED_PARAMETER(RegistryPath);
   CheckedDump(QIC117INFO,( "DriverEntry...\n" ));

   //
   // Ask configuration manager for information on the hardware that
   // we're supposed to support.
   //

#if DBG

//    DbgBreakPoint();

#endif

   ntStatus = Q117iGetConfigurationInformation( &configData );

   //
   // If Q117iGetConfigurationInformation() failed, just exit and propogate
   // the error.   If it said that there are no controllers to support,
   // return an error.
   // Otherwise, try to init the controllers.  If at least one succeeds,
   // return STATUS_SUCCESS, otherwise return the last error.
   //

   configData->FloppyTapeCount = 0;

   if ( NT_SUCCESS( ntStatus ) ) {

      //
      // Call Q117iInitializeController() for each controller (and its
      // attached drives) that we're supposed to support.
      //
      // Return success if we successfully initialize at least one
      // device; propogate error otherwise.   Set an error first in
      // case there aren't any controllers.
      //

      ntStatus = STATUS_NO_SUCH_DEVICE;

      for ( controllerNumber = 0;
               controllerNumber < configData->NumberOfControllers;
               controllerNumber++ ) {

            ntStatus = Q117iInitializeController(
               configData,
               controllerNumber,
               DriverObject,
               RegistryPath );

            if ( NT_SUCCESS( ntStatus ) ) {

               partlySuccessful = TRUE;
            }
      }

      if ( partlySuccessful ) {

            ntStatus = STATUS_SUCCESS;

            //
            // Initialize the driver object with this driver's entry points.
            //

            DriverObject->MajorFunction[IRP_MJ_READ] =
               q117Read;
            DriverObject->MajorFunction[IRP_MJ_WRITE] =
               q117Write;
            DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] =
               q117DeviceControl;
            DriverObject->MajorFunction[IRP_MJ_CREATE] =
               q117Create;
            DriverObject->MajorFunction[IRP_MJ_CLOSE] =
               q117Close;
            //DriverObject->MajorFunction[IRP_MJ_CLEANUP] =
            //    q117Cleanup;
            DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] =
               Q117iTapeDispatchInternalDeviceControl;
      }
   }

   //
   // NOTE: FUTURE delete configdata, if config mgr design calls for it
   //

#if DBG

   if ( !NT_SUCCESS( ntStatus ) ) {

      CheckedDump((QIC117INFO | QIC117DBGP),( "q117i: exiting with error %lx\n", ntStatus ));
   }

#endif

   if (configData) {

      ExFreePool(configData);

   }

   return ntStatus;
}

NTSTATUS
Q117iTapeDispatchInternalDeviceControl(
   IN PDEVICE_OBJECT DeviceObject,
   IN OUT PIRP Irp
   )

/*++

Routine Description:

   This routine is called by the I/O system to perform a device I/O
   control function.

Arguments:

   DeviceObject - a pointer to the object that represents the device
   that I/O is to be done on.

   Irp - a pointer to the I/O Request Packet for this request.

Return Value:

   STATUS_SUCCESS or STATUS_PENDING if recognized I/O control code,
   STATUS_INVALID_DEVICE_REQUEST otherwise.

--*/

{
   PIO_STACK_LOCATION irpSp;
   NTSTATUS ntStatus;
   PTAPE_EXTENSION tapeExtension;

   tapeExtension = DeviceObject->DeviceExtension;

   irpSp = IoGetCurrentIrpStackLocation( Irp );


   switch( irpSp->Parameters.DeviceIoControl.IoControlCode) {

   case IOCTL_QIC117_CLEAR_QUEUE:

   (VOID) KeResetEvent( &tapeExtension->QControllerData->ClearQueueEvent );

   tapeExtension->QControllerData->ClearQueue = TRUE;
   tapeExtension->QControllerData->AbortRequested = TRUE;

            IoMarkIrpPending( Irp );

      (VOID) ExInterlockedInsertTailList(
            &tapeExtension->QControllerData->ListEntry,
            &Irp->Tail.Overlay.ListEntry,
            &tapeExtension->QControllerData->ListSpinLock );

      (VOID) KeReleaseSemaphore(
            &tapeExtension->QControllerData->RequestSemaphore,
            (KPRIORITY) 0,
            1,
            FALSE );


   (VOID) KeWaitForSingleObject(
            (PVOID) &tapeExtension->QControllerData->ClearQueueEvent,
            Suspended,
            KernelMode,
            FALSE,
            (PLARGE_INTEGER) NULL );

   tapeExtension->QControllerData->ClearQueue = FALSE;
      ntStatus = STATUS_SUCCESS;

      break;

   case IOCTL_QIC117_DRIVE_REQUEST:

            IoMarkIrpPending( Irp );

      (VOID) ExInterlockedInsertTailList(
            &tapeExtension->QControllerData->ListEntry,
            &Irp->Tail.Overlay.ListEntry,
            &tapeExtension->QControllerData->ListSpinLock );

      (VOID) KeReleaseSemaphore(
            &tapeExtension->QControllerData->RequestSemaphore,
            (KPRIORITY) 0,
            1,
            FALSE );

      ntStatus = STATUS_PENDING;
      break;

   default:
      CheckedDump(QIC117DBGP,("q117i: invalid device request %x\n",
      irpSp->Parameters.DeviceIoControl.IoControlCode ));

      ntStatus = STATUS_INVALID_DEVICE_REQUEST;

   }

   return ntStatus;
}

BOOLEAN
Q117iTapeInterruptService(
   IN PKINTERRUPT Interrupt,
   IN OUT PVOID Context
   )

/*++

Routine Description:

   This routine is called at DIRQL by the system when the controller
   interrupts.

Arguments:

   Interrupt - a pointer to the interrupt object.

   Context - a pointer to our controller data area for the controller
   that interrupted.   (This was set up by the call to
   IoConnectInterrupt).

Return Value:

   Normally returns TRUE, but will return FALSE if this interrupt was
   not expected.

--*/

{
    PTAPE_CONTROLLER_DATA controllerData;
    PDEVICE_OBJECT currentDeviceObject;
    SHORT i;
    UCHAR statusByte;
    BOOLEAN controllerStateError = FALSE;

    UNREFERENCED_PARAMETER( Interrupt );

    KeStallExecutionProcessor(10);

    controllerData = (PTAPE_CONTROLLER_DATA) Context;

    //
    // CurrentDeviceObject is set to the device object that is
    // expecting an interrupt.
    //

    currentDeviceObject = controllerData->CurrentDeviceObject;

    if (currentDeviceObject == NULL &&
        !controllerData->CurrentInterrupt) {

        return FALSE;

    } else {

			controllerData->CurrentDeviceObject = NULL;

			if ( controllerData->CommandHasResultPhase ) {

				 //
				 // Result phase of previous command.    (Note that we can't trust
				 // the CMD_BUSY bit in the status register to tell us whether
				 // there's result bytes or not; it's sometimes wrong).
				 // By reading the first result byte, we reset the interrupt.
				 // The other result bytes will be read by a thread.
				 // Note that we want to do this even if the interrupt is
				 // unexpected, to make sure the interrupt is dismissed.
				 //

				 if ( ( READ_CONTROLLER( &controllerData->FDC_Addr->MSDSR.msr )
					  & (MSR_RQM | MSR_DIO) ) == (MSR_RQM | MSR_DIO) ) {

					  controllerData->FifoByte =
							READ_CONTROLLER( &controllerData->FDC_Addr->dr );

				 } else {

					  //
					  // Should never get here.   If we do, DON'T wake up the thread;
					  // let it time out and reset the controller, or let another
					  // interrupt handle this.
					  //

					  CheckedDump(QIC117DBGP,( "q117i: controller not ready to be read in ISR\n" ));

					  controllerStateError = TRUE;

				 }

			} else {

				 //
				 // Previous command doesn't have a result phase. To read how it
				 // completed, issue a sense interrupt command.  Don't read
				 // the result bytes from the sense interrupt; that is the
				 // responsibility of the calling thread.
				 // Note that we want to do this even if the interrupt is
				 // unexpected, to make sure the interrupt is dismissed.
				 //

				 i = FDC_MSR_RETRIES;

				 do {

					  if ((READ_CONTROLLER(&controllerData->FDC_Addr->MSDSR.msr) & (MSR_RQM | MSR_DIO)) == MSR_RQM) {

								 break;

					  }

					  KeStallExecutionProcessor( 12 );

				 } while (--i > 0);

				 if (i != 0) {

					  WRITE_CONTROLLER(
					  &controllerData->FDC_Addr->dr,
					  FDC_SNS_INT );

					  KeStallExecutionProcessor( 12 );

					  //
					  // Wait for the controller to ACK the SenseInterrupt command, by
					  // showing busy.    On very fast machines we can end up running
					  // driver's system-thread before the controller has had time to
					  // set the busy bit.
					  //

					  for (i = FDC_MSR_RETRIES; i; i--) {

							if (READ_CONTROLLER(&controllerData->FDC_Addr->MSDSR.msr) & MSR_CB)
									  break;

							KeStallExecutionProcessor( 12 );

					  }

					  // Need to optimize the previous section

					  if ( currentDeviceObject == NULL ) {

							//
							// This is an unexpected interrupt, so nobody's going to
							// read the result bytes.   Read them now.
							//

							READ_CONTROLLER( &controllerData->FDC_Addr->dr );
							READ_CONTROLLER( &controllerData->FDC_Addr->dr );

					  }

				 } else {

					  //
					  // Shouldn't get here.  If we do, DON'T wake up the thread;
					  // let it time out and reset the controller, or let another
					  // interrupt take care of it.
					  //

					  CheckedDump(QIC117DBGP,( "q117i: no result, but can't write SenseIntr\n" ));

					  controllerStateError = TRUE;
				 }

			}

			//
			// We've written to the controller, and we're about to leave.   On
			// machines with levelsensitive interrupts, we'll get another interrupt
			// if we RETURN before the port is flushed. To make sure that doesn't
			// happen, we'll do a read here.
			//

			statusByte = READ_CONTROLLER( &controllerData->FDC_Addr->MSDSR.msr );

			//
			// Let the interrupt settle.
			//

			KeStallExecutionProcessor(10);

			if ( currentDeviceObject == NULL ) {

				 //
				 // We didn't expect this interrupt. We've dismissed it just
				 // in case, but now return FALSE withOUT waking up the thread.
				 //

				 CheckedDump(QIC117DBGP,( "q117i: unexpected interrupt\n" ));

				 return FALSE;

			}

			if ( !controllerStateError ) {

				 //
				 // Request a DPC for execution later to get the remainder of the
				 // floppy state.
				 //

				 IoRequestDpc(
				 currentDeviceObject,
				 currentDeviceObject->CurrentIrp,
				 (PVOID) NULL );

			}

			return TRUE;

    }

}

VOID
Q117iTapeDeferredProcedure(
   IN PKDPC Dpc,
   IN PVOID DeferredContext,
   IN PVOID SystemArgument1,
   IN PVOID SystemArgument2
   )

/*++

Routine Description:

   This routine is called at DISPATCH_LEVEL by the system at the
   request of Q117iTapeInterruptService(). It simply sets the interrupt
   event, which wakes up the floppy thread.

Arguments:

   Dpc - a pointer to the DPC object used to invoke this routine.

   DeferredContext - a pointer to the device object associated with this
   DPC.

   SystemArgument1 - unused.

   SystemArgument2 - unused.

Return Value:

   None.

--*/

{
    PDEVICE_OBJECT deviceObject;
    PTAPE_EXTENSION tapeExtension;
    union format_header hdrData;      // sector id data
    FDC_STATUS fStat;                 // FDC status response
    SHORT statLength;                 // length of FDC status response
    ULONG  *hdrPtr;                   // pointer to sector id data for format
    PHYSICAL_ADDRESS val;
    struct seek_cmd seek;
    struct fdc_result result;
    SHORT i;

   UNREFERENCED_PARAMETER( Dpc );
   UNREFERENCED_PARAMETER( SystemArgument1 );
   UNREFERENCED_PARAMETER( SystemArgument2 );

   deviceObject = (PDEVICE_OBJECT) DeferredContext;
   tapeExtension = deviceObject->DeviceExtension;


    if (!tapeExtension->QControllerData->StartFormatMode) {

        (VOID) KeSetEvent(
				&tapeExtension->QControllerData->InterruptEvent,
				(KPRIORITY) 0,
				FALSE );

	  } else {

			//
			// Format all of the segments on the tape track.  Whenever a boundary
			// condition is reached (e.g. sectors > sectors per floppy track)
			// update the sector id information as necessary.
			//

			if (tapeExtension->FmtOp.Segments == (SHORT)tapeExtension->TapeParms.SegTtrack) {

				 IoFlushAdapterBuffers(
					  tapeExtension->QControllerData->AdapterObject,
					  tapeExtension->FmtOp.MdlAddress,
					  tapeExtension->QControllerData->MapRegisterBase,
					  (PVOID)( (ULONG) MmGetMdlVirtualAddress( tapeExtension->FmtOp.MdlAddress )
							+ tapeExtension->RdWrOp.BytesTransferredSoFar ),
					  tapeExtension->RdWrOp.TotalBytesOfTransfer,
					  DMA_READ );

				 if ((tapeExtension->FmtOp.retval = Q117iReadFDC(
									  tapeExtension,
									  (CHAR *)&fStat,
									  &statLength)) == NoErr) {

					  if (fStat.ST0 & ST0_IC) {

							tapeExtension->FmtOp.retval = TimeOut;

                     (VOID) KeSetEvent(
				             &tapeExtension->QControllerData->InterruptEvent,
				             (KPRIORITY) 0,
				             FALSE );

					  }

				 }

				 tapeExtension->QControllerData->StartFormatMode = FALSE;

				 if (tapeExtension->QControllerData->FDC_Pcn < 128) {

					  seek.NCN = tapeExtension->QControllerData->FDC_Pcn + Report_Status;

				 } else {

					  seek.NCN = tapeExtension->QControllerData->FDC_Pcn - Report_Status;

				 }

				 seek.cmd = 0x0f;
				 seek.drive = (UCHAR)tapeExtension->DriveParms.DriveSelect;
				 tapeExtension->FmtOp.NCN = seek.NCN;

		       tapeExtension->QControllerData->CurrentDeviceObject =
				     tapeExtension->QDeviceObject;

				 if ((tapeExtension->FmtOp.retval = Q117iProgramFDC(
									  tapeExtension,
									  (CHAR *)&seek,
									  sizeof(seek),
									  FALSE)) == NoErr) {

					  tapeExtension->QControllerData->EndFormatMode = TRUE;

				 } else {

					  //
					  // Request a DPC for execution later to get the remainder of the
					  // floppy state.
					  //

                 (VOID) KeSetEvent(
				         &tapeExtension->QControllerData->InterruptEvent,
				         (KPRIORITY) 0,
				         FALSE );

				 }

			} else if (tapeExtension->FmtOp.Segments == 0) {

				  if ((tapeExtension->FmtOp.retval = Q117iReadFDC(
															 tapeExtension,
															 (CHAR *)&result,
															 (SHORT *)&statLength)) == NoErr) {

						if ( !(result.ST0 & ST0_IC)) {

							 //
							 // If we timed out, then we did the sense interrupt status
							 // without clearing the interrupt from the interrupt controller.
							 // Since the FDC did not indicate an error, we assume that we
							 // missed the interrupt and send the EOI. Only needed for an
							 // 82072.
							 //

							 if (tapeExtension->QControllerData->InterfaceType != MicroChannel) {

								  if (result.ST0 !=
										(UCHAR)(tapeExtension->DriveParms.DriveSelect | ST0_SE)) {

										tapeExtension->FmtOp.retval = NECFlt;

								  }
							 }

							 if (tapeExtension->FmtOp.NCN != result.PCN) {

								  tapeExtension->FmtOp.retval = CmdFlt;

							 }

							 tapeExtension->QControllerData->FDC_Pcn = result.PCN;

						} else {

							 tapeExtension->FmtOp.retval = NECFlt;

						}

				  }

				 if (tapeExtension->FmtOp.retval == NoErr) {

					  hdrPtr = tapeExtension->FmtOp.HdrPtr;
					  hdrData.hdr_struct.C = tapeExtension->FmtOp.Cylinder;
					  hdrData.hdr_struct.H = tapeExtension->FmtOp.Head;
					  hdrData.hdr_struct.N = FMT_BPS;

					  for (i = 0; i < tapeExtension->TapeParms.FsectSeg; i++) {

							hdrData.hdr_struct.R = tapeExtension->FmtOp.Sector++;
							*hdrPtr = hdrData.hdr_all;
							++hdrPtr;

					  }

					  //
					  // Start the format by programming the DMA, starting the tape, and
					  // starting the floppy controller.
					  //
					  // Map the transfer through the DMA hardware.
					  //

					  tapeExtension->RdWrOp.BytesTransferredSoFar = 0l;
					  tapeExtension->RdWrOp.TotalBytesOfTransfer =
							tapeExtension->TapeParms.FsectSeg * sizeof(ULONG);

					  KeFlushIoBuffers( tapeExtension->FmtOp.MdlAddress, !DMA_READ, TRUE );

					  val = IoMapTransfer(
							tapeExtension->QControllerData->AdapterObject,
							tapeExtension->FmtOp.MdlAddress,
							tapeExtension->QControllerData->MapRegisterBase,
							(PVOID)( (ULONG) MmGetMdlVirtualAddress(tapeExtension->FmtOp.MdlAddress)
								 + tapeExtension->RdWrOp.BytesTransferredSoFar ),
							&tapeExtension->RdWrOp.TotalBytesOfTransfer,
							DMA_READ );

   				  tapeExtension->QControllerData->CurrentDeviceObject =
					      tapeExtension->QDeviceObject;

					  if ((tapeExtension->FmtOp.retval = Q117iProgramFDC(
																 tapeExtension,
																 (CHAR *)&tapeExtension->QControllerData->FmtCmd,
																 sizeof(FORMAT_CMD),
																 TRUE)) != NoErr) {

							IoFlushAdapterBuffers(
								 tapeExtension->QControllerData->AdapterObject,
								 tapeExtension->FmtOp.MdlAddress,
								 tapeExtension->QControllerData->MapRegisterBase,
								 (PVOID)( (ULONG) MmGetMdlVirtualAddress( tapeExtension->FmtOp.MdlAddress )
									  + tapeExtension->RdWrOp.BytesTransferredSoFar ),
								 tapeExtension->RdWrOp.TotalBytesOfTransfer,
								 DMA_READ );

                     (VOID) KeSetEvent(
				             &tapeExtension->QControllerData->InterruptEvent,
				             (KPRIORITY) 0,
				             FALSE );

					  }

					  tapeExtension->FmtOp.Segments++;
						  
				 } else {

                 (VOID) KeSetEvent(
				         &tapeExtension->QControllerData->InterruptEvent,
				         (KPRIORITY) 0,
				         FALSE );

				 }


			} else {

				 IoFlushAdapterBuffers(
					  tapeExtension->QControllerData->AdapterObject,
					  tapeExtension->FmtOp.MdlAddress,
					  tapeExtension->QControllerData->MapRegisterBase,
					  (PVOID)( (ULONG) MmGetMdlVirtualAddress( tapeExtension->FmtOp.MdlAddress )
							+ tapeExtension->RdWrOp.BytesTransferredSoFar ),
					  tapeExtension->RdWrOp.TotalBytesOfTransfer,
					  DMA_READ );

				 if ((tapeExtension->FmtOp.retval = Q117iReadFDC(
									  tapeExtension,
									  (CHAR *)&fStat,
									  &statLength)) == NoErr) {

					  if (fStat.ST0 & ST0_IC) {

							tapeExtension->FmtOp.retval = TimeOut;

					  }
				 }

				 if (tapeExtension->FmtOp.retval != NoErr) {

                 (VOID) KeSetEvent(
				         &tapeExtension->QControllerData->InterruptEvent,
				         (KPRIORITY) 0,
				         FALSE );

				 }

				 if (tapeExtension->FmtOp.Sector > tapeExtension->TapeParms.FsectFtrack) {

					  tapeExtension->FmtOp.Sector = 1;
					  tapeExtension->FmtOp.Cylinder++;

					  if (tapeExtension->FmtOp.Cylinder ==
							(UCHAR)tapeExtension->TapeParms.FtrackFside) {

							tapeExtension->FmtOp.Cylinder = 0;
							tapeExtension->FmtOp.Head++;

					  }
				 }

				 //
				 // Set up the sector id information for this segment.
				 //


				 hdrPtr = tapeExtension->FmtOp.HdrPtr;
				 hdrData.hdr_struct.C = tapeExtension->FmtOp.Cylinder;
				 hdrData.hdr_struct.H = tapeExtension->FmtOp.Head;
				 hdrData.hdr_struct.N = FMT_BPS;

				 for (i = 0; i < tapeExtension->TapeParms.FsectSeg; i++) {

					  hdrData.hdr_struct.R = tapeExtension->FmtOp.Sector++;
					  *hdrPtr = hdrData.hdr_all;
					  ++hdrPtr;

				 }

				 //
				 // Start the format by programming the DMA, starting the tape, and
				 // starting the floppy controller.
				 //
				 // Map the transfer through the DMA hardware.
				 //

				 tapeExtension->RdWrOp.BytesTransferredSoFar = 0l;
				 tapeExtension->RdWrOp.TotalBytesOfTransfer =
					  tapeExtension->TapeParms.FsectSeg * sizeof(ULONG);

				 KeFlushIoBuffers( tapeExtension->FmtOp.MdlAddress, !DMA_READ, TRUE );

				 val = IoMapTransfer(
					  tapeExtension->QControllerData->AdapterObject,
					  tapeExtension->FmtOp.MdlAddress,
					  tapeExtension->QControllerData->MapRegisterBase,
					  (PVOID)( (ULONG) MmGetMdlVirtualAddress(tapeExtension->FmtOp.MdlAddress)
							+ tapeExtension->RdWrOp.BytesTransferredSoFar ),
					  &tapeExtension->RdWrOp.TotalBytesOfTransfer,
					  DMA_READ );

				 tapeExtension->QControllerData->CurrentDeviceObject =
				     tapeExtension->QDeviceObject;

				 if ((tapeExtension->FmtOp.retval = Q117iProgramFDC(
				                                 tapeExtension,
				                                 (CHAR *)&tapeExtension->QControllerData->FmtCmd,
				                                 sizeof(FORMAT_CMD),
				                                 TRUE)) != NoErr) {

					  IoFlushAdapterBuffers(
							tapeExtension->QControllerData->AdapterObject,
							tapeExtension->FmtOp.MdlAddress,
							tapeExtension->QControllerData->MapRegisterBase,
							(PVOID)( (ULONG) MmGetMdlVirtualAddress( tapeExtension->FmtOp.MdlAddress )
								 + tapeExtension->RdWrOp.BytesTransferredSoFar ),
							tapeExtension->RdWrOp.TotalBytesOfTransfer,
							DMA_READ );

                 (VOID) KeSetEvent(
				         &tapeExtension->QControllerData->InterruptEvent,
				         (KPRIORITY) 0,
				         FALSE );

				 }	

				 tapeExtension->FmtOp.Segments++;
					 
			}

	  }

}

VOID
Q117iTapeUnloadDriver(
   IN PDRIVER_OBJECT DriverObject
   )

/*++

Routine Description:

   This routine is called by the system to remove the driver from memory.

   When this routine is called, there is no I/O being done to this device.
   The driver object is passed in, and from this the driver can find and
   delete all of its device objects, extensions, etc.

Arguments:

   DriverObject - a pointer to the object associated with this device
   driver.

Return Value:

   None.

--*/

{
   UNREFERENCED_PARAMETER( DriverObject );

//  signal Q117iTapeThread() to unload itself
//  disable interrupts from controller(s?)
//  delete everything that's been allocated
}

VOID
Q117iTapeThread(
   PTAPE_CONTROLLER_DATA ControllerData
   )

/*++

Routine Description:

   This is the code executed by the system thread created when the
   floppy driver initializes.  This thread loops forever (or until a
   flag is set telling the thread to kill itself) processing packets
   put into the queue by the dispatch routines.

   For each packet, this thread calls appropriate routines to process
   the request, and then calls FlFinishOperation() to complete the
   packet.

Arguments:

   ControllerData - a pointer to our data area for the controller being
   supported (there is one thread per controller).

Return Value:

   None.

--*/

{
   PIRP irp;
   PIO_STACK_LOCATION irpSp;
   PLIST_ENTRY request;
   NTSTATUS ntStatus = 0;

   //
   // Set thread priority to lowest realtime level.
   //

   KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

   do {

      //
      // Wait for a request from the dispatch routines.
      // KeWaitForSingleObject won't return error here - this thread
      // isn't alertable and won't take APCs, and we're not passing in
      // a timeout.
      //

      (VOID) KeWaitForSingleObject(
            (PVOID) &ControllerData->RequestSemaphore,
            UserRequest,
            KernelMode,
            FALSE,
            (PLARGE_INTEGER) NULL );

      if ( ControllerData->UnloadingDriver ) {

            CheckedDump(QIC117INFO,( "q117i: Thread asked to kill itself\n" ));

            PsTerminateSystemThread( STATUS_SUCCESS );
      }

      while ( !IsListEmpty( &( ControllerData->ListEntry ) ) ) {

            //
            // Get the request from the queue. We know there is one,
            // because of the check above.
            //

               request = ExInterlockedRemoveHeadList(
               &ControllerData->ListEntry,
               &ControllerData->ListSpinLock );


            ControllerData->QueueEmpty =
                  IsListEmpty( &( ControllerData->ListEntry ) );

            irp = CONTAINING_RECORD( request, IRP, Tail.Overlay.ListEntry );

            irpSp = IoGetCurrentIrpStackLocation( irp );

            if ( ControllerData->ClearQueue ||
               irpSp->Parameters.DeviceIoControl.IoControlCode ==
               IOCTL_QIC117_CLEAR_QUEUE) {

               if (irpSp->Parameters.DeviceIoControl.IoControlCode == IOCTL_QIC117_CLEAR_QUEUE) {

                  CheckedDump(QIC117INFO,("Q117i: processing IOCTL_QIC117_CLEAR_QUEUE : TRUE\n"));

                  irp->IoStatus.Status = Q117iClearIO( irp );

                  // NOTE: This is temporary until we ca find how to
                  // correctly free the Mdl using the io subsytem.
                  if (irp->MdlAddress != NULL) {
                        IoFreeMdl(irp->MdlAddress);
                        irp->MdlAddress = NULL;
                  }

                  IoCompleteRequest( irp, IO_DISK_INCREMENT );

                  (VOID) KeSetEvent(
                        &ControllerData->ClearQueueEvent,
                        (KPRIORITY) 0,
                        FALSE );

               } else {

                  CheckedDump(QIC117INFO,("Q117i: processing IOCTL_QIC117_DRIVE_REQUEST : TRUE\n"));

                  irp->IoStatus.Status = STATUS_CANCELLED;

                  // NOTE: This is temporary until we ca find how to
                  // correctly free the Mdl using the io subsytem.
                  if (irp->MdlAddress != NULL) {
                        IoFreeMdl(irp->MdlAddress);
                        irp->MdlAddress = NULL;
                  }

                  IoCompleteRequest( irp, IO_DISK_INCREMENT );

               }

            } else {

               irp->IoStatus.Status = Q117iProcessItem( irp );

               // NOTE: This is temporary until we ca find how to
               // correctly free the Mdl using the io subsytem.
               if (irp->MdlAddress != NULL) {
                  IoFreeMdl(irp->MdlAddress);
                  irp->MdlAddress = NULL;
               }

               IoCompleteRequest( irp, IO_DISK_INCREMENT );

            }

      }                                   //while there's packets to process

   } while ( TRUE );
}

IO_ALLOCATION_ACTION
Q117iTapeAllocateAdapterChannel(
   IN PDEVICE_OBJECT DeviceObject,
   IN PIRP Irp,
   IN PVOID MapRegisterBase,
   IN PVOID Context
   )

/*++

Routine Description:

   This DPC is called whenever the floppy thread is trying to allocate
   the adapter channel (like before doing a read or write).    It saves
   the MapRegisterBase in the controller data area, and sets the
   AllocateAdapterChannelEvent to awaken the thread.

Arguments:

   DeviceObject - unused.

   Irp - unused.

   MapRegisterBase - the base of the map registers that can be used
   for this transfer.

   Context - a pointer to our controller data area.

Return Value:

   Returns Allocation Action 'KeepObject' which means that the adapter
   object will be held for now (to be released explicitly later).

--*/
{
   PTAPE_CONTROLLER_DATA controllerData = (PTAPE_CONTROLLER_DATA) Context;

   UNREFERENCED_PARAMETER( DeviceObject );
   UNREFERENCED_PARAMETER( Irp );

   controllerData->MapRegisterBase = MapRegisterBase;

   (VOID) KeSetEvent(
      &controllerData->AllocateAdapterChannelEvent,
      0L,
      FALSE );

   return KeepObject;
}

NTSTATUS
Q117iConfigCallBack(
   IN PVOID Context,
   IN PUNICODE_STRING PathName,
   IN INTERFACE_TYPE BusType,
   IN ULONG BusNumber,
   IN PKEY_VALUE_FULL_INFORMATION *BusInformation,
   IN CONFIGURATION_TYPE ControllerType,
   IN ULONG ControllerNumber,
   IN PKEY_VALUE_FULL_INFORMATION *ControllerInformation,
   IN CONFIGURATION_TYPE PeripheralType,
   IN ULONG PeripheralNumber,
   IN PKEY_VALUE_FULL_INFORMATION *PeripheralInformation
   )

/*++

Routine Description:

   This routine is used to acquire all of the configuration
   information for each floppy disk controller and the
   peripheral driver attached to that controller.

Arguments:

   Context - Pointer to the confuration information we are building
               up.

   PathName - unicode registry path.   Not Used.

   BusType - Internal, Isa, ...

   BusNumber - Which bus if we are on a multibus system.

   BusInformation - Configuration information about the bus. Not Used.

   ControllerType - Should always be DiskController.

   ControllerNumber - Which controller if there is more than one
                        controller in the system.

   ControllerInformation - Array of pointers to the three pieces of
                           registry information.

   PeripheralType - Should always be FloppyDiskPeripheral.

   PeripheralNumber - Which floppy if this controller is maintaining
                        more than one.

   PeripheralInformation - Array of pointers to the three pieces of
                           registry information.

Return Value:

   STATUS_SUCCESS if everything went ok, or STATUS_INSUFFICIENT_RESOURCES
   if it couldn't map the base csr or acquire the adapter object, or
   all of the resource information couldn't be acquired.

--*/

{

   //
   // So we don't have to typecast the context.
   //
   PCONFIG_DATA config = Context;

   //
   // Simple iteration variable.
   //
   ULONG i;

   //
   // This boolean will be used to denote whether we've seen this
   // controller before.
   //
   BOOLEAN newController;

   //
   // This will be used to denote whether we even have room
   // for a new controller.
   //
   BOOLEAN outOfRoom;

   //
   // Iteration variable that will end up indexing to where
   // the controller information should be placed.
   //
   ULONG ControllerSlot;

   //
   // Short hand for referencing the particular controller config
   // information that we are building up.
   //
   PCONFIG_CONTROLLER_DATA controller;

   PCM_FULL_RESOURCE_DESCRIPTOR peripheralData = (PCM_FULL_RESOURCE_DESCRIPTOR)
      (((PUCHAR)PeripheralInformation[IoQueryDeviceConfigurationData]) +
      PeripheralInformation[IoQueryDeviceConfigurationData]->DataOffset);

   //
   // These three boolean will tell us whether we got all the
   // information that we needed.
   //
   BOOLEAN foundPort = FALSE;
   BOOLEAN foundInterrupt = FALSE;
   BOOLEAN foundDma = FALSE;

   ASSERT(ControllerType == DiskController);
   ASSERT(PeripheralType == FloppyDiskPeripheral);

   //
   // Loop through the "slots" that we have for a new controller.
   // Determine if this is a controller that we've already seen,
   // or a new controller.
   //

   outOfRoom = TRUE;
   for (
      ControllerSlot = 0;
      ControllerSlot < MAXIMUM_CONTROLLERS_PER_MACHINE;
      ControllerSlot++
      ) {

      if (config->Controller[ControllerSlot].ActualControllerNumber == -1) {

            newController = TRUE;
            outOfRoom = FALSE;
            config->Controller[ControllerSlot].ActualControllerNumber =
               ControllerNumber;
            config->NumberOfControllers++;
            break;

      } else if (config->Controller[ControllerSlot].ActualControllerNumber
                  == (LONG)ControllerNumber) {

            newController = FALSE;
            outOfRoom = FALSE;
            break;

      }

   }

   if (outOfRoom) {

      //
      // Just return and ignore the controller.
      //

      return STATUS_SUCCESS;

   }

   controller = &config->Controller[ControllerSlot];

   if (newController) {

      PCM_FULL_RESOURCE_DESCRIPTOR controllerData =
            (PCM_FULL_RESOURCE_DESCRIPTOR)
            (((PUCHAR)ControllerInformation[IoQueryDeviceConfigurationData]) +
            ControllerInformation[IoQueryDeviceConfigurationData]->DataOffset);

      //
      // We have the pointer. Save off the interface type and
      // the busnumber for use when we call the Hal and the
      // Io System.
      //

      controller->InterfaceType = BusType;
      controller->BusNumber = BusNumber;
      controller->SharableVector = TRUE;
      controller->SaveFloatState = FALSE;

      //
      // We need to get the following information out of the partial
      // resource descriptors.
      //
      // The irql and vector.
      //
      // The dma channel.
      //
      // The base address and span covered by the floppy controllers
      // registers.
      //
      // It is not defined how these appear in the partial resource
      // lists, so we will just loop over all of them.    If we find
      // something we don't recognize, we drop that information on
      // the floor.   When we have finished going through all the
      // partial information, we validate that we got the above
      // three.
      //

      CheckedDump(QIC117INFO,("Q117i: adding controller: %x slot: %x\n",ControllerNumber,ControllerSlot));

      for (
            i = 0;
            i < controllerData->PartialResourceList.Count;
            i++
            ) {

            PCM_PARTIAL_RESOURCE_DESCRIPTOR partial =
               &controllerData->PartialResourceList.PartialDescriptors[i];

            switch (partial->Type) {

               case CmResourceTypePort: {

                  BOOLEAN inIoSpace =
#ifdef i386
                              TRUE;
#else
                              FALSE;
#endif

                  foundPort = TRUE;

                  //
                  // Save of the pointer to the partial so
                  // that we can later use it to report resources
                  // and we can also use this later in the routine
                  // to make sure that we got all of our resources.
                  //

                  controller->SpanOfControllerAddress =
                        partial->u.Port.Length;
                  controller->OriginalBaseAddress =
                        partial->u.Port.Start;
                  controller->ControllerBaseAddress = (PTAPE_ADDRESS)
                        Q117iGetControllerBase(
                           BusType,
                           BusNumber,
                           partial->u.Port.Start,
                           controller->SpanOfControllerAddress,
                           inIoSpace,
                           &controller->MappedAddress
                           );

                  if (!controller->ControllerBaseAddress) {

                        return STATUS_INSUFFICIENT_RESOURCES;

                  }

                  break;
               }
               case CmResourceTypeInterrupt: {

                  foundInterrupt = TRUE;
                  if (partial->Flags & CM_RESOURCE_INTERRUPT_LATCHED) {

                        controller->InterruptMode = Latched;

                  } else {

                        controller->InterruptMode = LevelSensitive;

                  }

                  controller->OriginalIrql =  partial->u.Interrupt.Level;
                  controller->OriginalVector = partial->u.Interrupt.Vector;
                  controller->ControllerVector =
                        HalGetInterruptVector(
                           BusType,
                           BusNumber,
                           partial->u.Interrupt.Level,
                           partial->u.Interrupt.Vector,
                           &controller->ControllerIrql,
                           &controller->ProcessorMask
                           );

                  break;
               }
               case CmResourceTypeDma: {

                  DEVICE_DESCRIPTION deviceDesc;

                  RtlZeroMemory(&deviceDesc,sizeof(deviceDesc));
                  foundDma = TRUE;

                  controller->OriginalDmaChannel = partial->u.Dma.Channel;

                  deviceDesc.Version = DEVICE_DESCRIPTION_VERSION;
                  deviceDesc.DmaWidth = Width8Bits;
                  deviceDesc.DemandMode = TRUE;
                  deviceDesc.MaximumLength = 32l*1024l;
                  deviceDesc.AutoInitialize = FALSE;
                  deviceDesc.ScatterGather = FALSE;
                  deviceDesc.DmaChannel = partial->u.Dma.Channel;
                  deviceDesc.InterfaceType = BusType;
                  deviceDesc.DmaSpeed = TypeA;
                  controller->NumberOfMapRegisters = BYTES_TO_PAGES(32l*1024l);
                  controller->AdapterObject =
                        HalGetAdapter(
                           &deviceDesc,
                           &controller->NumberOfMapRegisters
                           );

                  CheckedDump(QIC117INFO,( "Q117i: Bus Type = %d\n",
                        BusType ));
                  CheckedDump(QIC117INFO,( "Q117i: Number of map registers = %d\n",
                        controller->NumberOfMapRegisters ));

                  if (!controller->AdapterObject) {

                        return STATUS_INSUFFICIENT_RESOURCES;

                  }

                  break;

               }
               default: {

                  break;

               }

            }

      }

      //
      // If we didn't get all the information then we return
      // insufficient resources.
      //

      if ((!foundPort) ||
            (!foundInterrupt) ||
            (!foundDma)) {

            return STATUS_INSUFFICIENT_RESOURCES;

      }
      controller->NumberOfTapeDrives++;
      controller->OkToUseThisController = TRUE;

   }


   return STATUS_SUCCESS;
}

NTSTATUS
Q117iGetConfigurationInformation(
   OUT PCONFIG_DATA *ConfigData
   )

/*++

Routine Description:

   This routine is called by DriverEntry() to get information about the
   devices to be supported from configuration mangement and/or the
   hardware architecture layer (HAL).

Arguments:

   ConfigData - a pointer to the pointer to a data structure that
   describes the controllers and the drives attached to them

Return Value:

   Returns STATUS_SUCCESS unless there is no drive 0 or we didn't get
   any configuration information.
   NOTE: FUTURE return values may change when config mgr is finished.

--*/

{
   INTERFACE_TYPE InterfaceType;
   NTSTATUS Status;
   ULONG i;

   *ConfigData = ExAllocatePool(
                        PagedPool,
                        sizeof(CONFIG_DATA)
                        );

   if (!*ConfigData) {

      return STATUS_INSUFFICIENT_RESOURCES;

   }

   //
   // Zero out the config structure and fill in the actual
   // controller numbers with -1's so that the callback routine
   // can recognize a new controller.
   //

   RtlZeroMemory(
      *ConfigData,
      sizeof(CONFIG_DATA)
      );

   for (
      i = 0;
      i < MAXIMUM_CONTROLLERS_PER_MACHINE;
      i++
      ) {

      (*ConfigData)->Controller[i].ActualControllerNumber = -1;

   }

   //
   // Go through all of the various bus types looking for
   // disk controllers.    The disk controller sections of the
   // hardware registry only deal with the floppy drives.
   // The callout routine that can get called will then
   // look for information pertaining to a particular
   // device on the controller.
   //

   for (
      InterfaceType = 0;
      InterfaceType < MaximumInterfaceType;
      InterfaceType++
      ) {

      CONFIGURATION_TYPE Dc = DiskController;
      CONFIGURATION_TYPE Fp = FloppyDiskPeripheral;

      Status = IoQueryDeviceDescription(
                     &InterfaceType,
                     NULL,
                     &Dc,
                     NULL,
                     &Fp,
                     NULL,
                     Q117iConfigCallBack,
                     *ConfigData
                     );

      if (!NT_SUCCESS(Status) && (Status != STATUS_OBJECT_NAME_NOT_FOUND)) {

            ExFreePool(*ConfigData);
            *ConfigData = NULL;
            return Status;

      }

   }

   return STATUS_SUCCESS;
}

NTSTATUS
Q117iInitializeController(
   IN PCONFIG_DATA ConfigData,
   IN UCHAR ControllerNumber,
   IN PDRIVER_OBJECT DriverObject,
   IN PUNICODE_STRING RegistryPath
   )

/*++

Routine Description:

   This routine is called at initialization time by DriverEntry() -
   once for each controller that the configuration manager tells it we
   have to support.

   When this routine is called, the configuration data has already been
   filled in.

Arguments:

   ConfigData - a pointer to the structure that describes the
   controller and the disks attached to it, as given to us by the
   configuration manager.

   ControllerNumber - which controller in ConfigData we are
   initializing.

   DriverObject - a pointer to the object that represents this device
   driver.

Return Value:

   STATUS_SUCCESS if this controller and at least one of its disks were
   initialized; an error otherwise.

--*/

{
   PTAPE_CONTROLLER_DATA controllerData;
   PVOID threadObject;
   NTSTATUS ntStatus;
   NTSTATUS ntStatus2;
   HANDLE threadHandle = 0;
   BOOLEAN partlySuccessful;
   LARGE_INTEGER timeout;
   UCHAR ntNameBuffer[256];
   STRING ntNameString;
   UNICODE_STRING ntUnicodeString;

   CheckedDump(QIC117INFO,( "Q117iInitializeController...\n" ));

   //
   // This routine will take attempt to "append" the resources
   // used by this controller into the resource map of the
   // registry.    If there was a conflict with previously "declared"
   // data, then this routine will return false, in which case we
   // will NOT try to initialize this particular controller.
   //

   if (!Q117iReportResources(
            DriverObject,
            ConfigData,
            ControllerNumber
            )) {

      return STATUS_INSUFFICIENT_RESOURCES;

   }

   //
   // Allocate and zero-initialize data to describe this controller
   //

   controllerData = (PTAPE_CONTROLLER_DATA) ExAllocatePool(
      NonPagedPool,
      sizeof( TAPE_CONTROLLER_DATA ) );

   if ( controllerData == NULL ) {

      return STATUS_INSUFFICIENT_RESOURCES;
   }

   RtlZeroMemory( controllerData, sizeof( TAPE_CONTROLLER_DATA ) );

   (VOID) sprintf(
      ntNameBuffer,
      "\\Device\\FloppyControllerEvent%d",
      ControllerNumber );

   RtlInitString( &ntNameString, ntNameBuffer );

   ntStatus = RtlAnsiStringToUnicodeString(
      &ntUnicodeString,
      &ntNameString,
      TRUE );

   controllerData->ControllerEvent = IoCreateSynchronizationEvent(
      &ntUnicodeString,
      &controllerData->ControllerEventHandle);

   RtlFreeUnicodeString( &ntUnicodeString );

   if ( controllerData->ControllerEvent == NULL ) {
      return STATUS_INSUFFICIENT_RESOURCES;
   }

   //
   // Fill in some items that we got from configuration management and
   // the HAL.
   //

   controllerData->FDC_Addr = (PTAPE_ADDRESS)
      ConfigData->Controller[ControllerNumber].ControllerBaseAddress;
   controllerData->InterfaceType =
      ConfigData->Controller[ControllerNumber].InterfaceType;
   controllerData->ActualControllerNumber =
      ConfigData->Controller[ControllerNumber].ActualControllerNumber;

   controllerData->DriveSelect.Selected = FALSE;
   controllerData->DriveSelect.DeselectByte = dselb;
   controllerData->DriveSelect.SelectByte = selb;

   ntStatus = IoConnectInterrupt(
      (PKINTERRUPT *) &controllerData->InterruptObject,
      (PKSERVICE_ROUTINE) Q117iTapeInterruptService,
      (PVOID) controllerData,
      (PKSPIN_LOCK)NULL,
      ConfigData->Controller[ControllerNumber].ControllerVector,
      ConfigData->Controller[ControllerNumber].ControllerIrql,
      ConfigData->Controller[ControllerNumber].ControllerIrql,
      ConfigData->Controller[ControllerNumber].InterruptMode,
      ConfigData->Controller[ControllerNumber].SharableVector,
      ConfigData->Controller[ControllerNumber].ProcessorMask,
      ConfigData->Controller[ControllerNumber].SaveFloatState);


   if ( NT_SUCCESS( ntStatus ) ) {
      //
      // Initialize the interlocked request queue, including a
      // counting semaphore to indicate items in the queue
      //

      KeInitializeSemaphore(
            &controllerData->RequestSemaphore,
            0L,
            MAXLONG );

      KeInitializeSpinLock( &controllerData->ListSpinLock );

      InitializeListHead( &controllerData->ListEntry );

      //
      // Initialize events to signal interrupts and adapter object
      // allocation
      //

      KeInitializeEvent(
            &controllerData->InterruptEvent,
            SynchronizationEvent,
            FALSE);


      KeInitializeEvent(
            &controllerData->AllocateAdapterChannelEvent,
            NotificationEvent,
            FALSE );


      KeInitializeEvent(
            &controllerData->ClearQueueEvent,
            SynchronizationEvent,
            FALSE);



      //
      // Create a thread with entry point Q117iTapeThread()
      //

      ntStatus = PsCreateSystemThread(
            &threadHandle,
            (ACCESS_MASK) 0L,
            (POBJECT_ATTRIBUTES) NULL,
            (HANDLE) 0L,
            (PCLIENT_ID) NULL,
            (PKSTART_ROUTINE) Q117iTapeThread,
            (PVOID) controllerData );

#if DBG
      if ( !NT_SUCCESS( ntStatus ) ) {

            CheckedDump(QIC117DBGP,( "q117i: error %x creating thread\n", ntStatus ));
      }
#endif

      if ( NT_SUCCESS( ntStatus ) ) {

            CheckedDump(QIC117INFO,("Q117iThread = %x\n",threadHandle));

            //
            // Call Q117iInitializeDrive() for each drive on the
            // controller
            //

            ConfigData->Controller[ControllerNumber].NumberOfTapeDrives++;

            ntStatus = STATUS_NO_SUCH_DEVICE;
            partlySuccessful = FALSE;

            ntStatus = Q117iInitializeDrive(
               ConfigData,
               controllerData,
               ControllerNumber,
               DriverObject,
               RegistryPath );
      }

   }

   //
   // If we're exiting with an error, clean up first.
   //

   if ( !NT_SUCCESS( ntStatus ) ) {

      CheckedDump(QIC117DBGP,( "q117i: InitializeController failing\n" ));

      //
      // If we created the thread, wake it up and tell it to kill itself.
      // Wait until it's dead.    (Note that since it's a system thread,
      // it has to kill itself - we can't do it).
      //

      if ( threadHandle != 0 ) {

            controllerData->UnloadingDriver = TRUE;

            ntStatus2 = ObReferenceObjectByHandle(
               threadHandle,
               THREAD_ALL_ACCESS,
               NULL,
               KernelMode,
               (PVOID *) &threadObject,
               NULL );

            (VOID) KeReleaseSemaphore(
               &controllerData->RequestSemaphore,
               (KPRIORITY) 0,
               1,
               FALSE );

            if ( NT_SUCCESS( ntStatus2 ) ) {

               //
               // The thread object will be signalled when it dies.
               //

               ntStatus2 = KeWaitForSingleObject(
                  (PVOID) threadObject,
                  Suspended,
                  KernelMode,
                  FALSE,
                  (PLARGE_INTEGER) NULL );

               ASSERT( ntStatus2 == STATUS_SUCCESS );

               ObDereferenceObject( threadObject );

            } else {

               //
               // We can't get the thread object for some reason; just
               // block for a while to give the thread a chance to run
               // and die.
               //

               CheckedDump(QIC117DBGP,( "q117i: couldn't get thread object\n" ));


               timeout =
                  RtlLargeIntegerNegate(
                  RtlEnlargedIntegerMultiply(
                        10,
                        10l * 1000l
                  )
               );
               (VOID) KeDelayExecutionThread(
                  KernelMode,
                  FALSE,
                  &timeout );
            }
      }


      if ( controllerData->InterruptObject != NULL ) {

          (VOID) KeSetEvent(
                    controllerData->ControllerEvent,
                    (KPRIORITY) 0,
                    FALSE );

          IoDisconnectInterrupt( controllerData->InterruptObject );
      }

      ExFreePool( controllerData );

   }

   return ntStatus;
}

BOOLEAN
Q117iReportResources(
   IN PDRIVER_OBJECT DriverObject,
   IN PCONFIG_DATA ConfigData,
   IN UCHAR ControllerNumber
   )

/*++

Routine Description:

   This routine will build up a resource list using the
   data for this particular controller as well as all
   previous *successfully* configured controllers.

   N.B.  This routine assumes that it called in controller
   number order.

Arguments:

   DriverObject - a pointer to the object that represents this device
   driver.

   ConfigData - a pointer to the structure that describes the
   controller and the disks attached to it, as given to us by the
   configuration manager.

   ControllerNumber - which controller in ConfigData we are
   about to try to report.

Return Value:

   TRUE if no conflict was detected, FALSE otherwise.

--*/

{


   ULONG sizeOfResourceList;
   ULONG numberOfFrds;
   LONG i;
   PCM_RESOURCE_LIST resourceList;
   PCM_FULL_RESOURCE_DESCRIPTOR nextFrd;

   //
   // Loop through all of the controllers previous to this
   // controller.  If the controllers previous to this one
   // didn't have a conflict, then accumulate the size of the
   // CM_FULL_RESOURCE_DESCRIPTOR associated with it.
   //

   for (
      i = 0,numberOfFrds = 0,sizeOfResourceList = 0;
      i <= ControllerNumber;
      i++
      ) {

      if (ConfigData->Controller[i].OkToUseThisController) {

            sizeOfResourceList += sizeof(CM_FULL_RESOURCE_DESCRIPTOR);

            //
            // The full resource descriptor already contains one
            // partial.  Make room for three more.
            //
            // It will hold the irq "prd", the controller "csr" "prd" which
            // is actually in two pieces since we don't use one of the
            // registers, and the controller dma "prd".
            //

            sizeOfResourceList += 3*sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);
            numberOfFrds++;

      }

   }

   //
   // Now we increment the length of the resource list by field offset
   // of the first frd.   This will give us the length of what preceeds
   // the first frd in the resource list.
   //

   sizeOfResourceList += FIELD_OFFSET(
                              CM_RESOURCE_LIST,
                              List[0]
                              );

   resourceList = ExAllocatePool(
                     PagedPool,
                     sizeOfResourceList
                     );

   if (!resourceList) {

      return FALSE;

   }

   //
   // Zero out the field
   //

   RtlZeroMemory(
      resourceList,
      sizeOfResourceList
      );

   resourceList->Count = numberOfFrds;
   nextFrd = &resourceList->List[0];

   for (
      i = 0;
      numberOfFrds;
      i++
      ) {

      if (ConfigData->Controller[i].OkToUseThisController) {

            PCM_PARTIAL_RESOURCE_DESCRIPTOR partial;

            nextFrd->InterfaceType = ConfigData->Controller[i].InterfaceType;
            nextFrd->BusNumber = ConfigData->Controller[i].BusNumber;

            //
            // We are only going to report 4 items no matter what
            // was in the original.
            //

            nextFrd->PartialResourceList.Count = 4;

            //
            // Now fill in the port data.  We don't wish to share
            // this port range with anyone
            //

            partial = &nextFrd->PartialResourceList.PartialDescriptors[0];

            partial->Type = CmResourceTypePort;
            partial->ShareDisposition = CmResourceShareShared;
            partial->Flags = 0;
            partial->u.Port.Start =
               ConfigData->Controller[i].OriginalBaseAddress;
            partial->u.Port.Length = 6;

            partial++;

            partial->Type = CmResourceTypePort;
            partial->ShareDisposition = CmResourceShareShared;
            partial->Flags = 0;
            partial->u.Port.Start = RtlLargeIntegerAdd(
                        ConfigData->Controller[i].OriginalBaseAddress,
                        RtlConvertUlongToLargeInteger((ULONG)7)
                        );
            partial->u.Port.Length = 1;

            partial++;

            partial->Type = CmResourceTypeDma;
            partial->ShareDisposition = CmResourceShareShared;
            partial->Flags = 0;
            partial->u.Dma.Channel =
               ConfigData->Controller[i].OriginalDmaChannel;

            partial++;

            //
            // Now fill in the irq stuff.
            //

            partial->Type = CmResourceTypeInterrupt;
            partial->ShareDisposition = CmResourceShareShared;
            partial->u.Interrupt.Level =
               ConfigData->Controller[i].OriginalIrql;
            partial->u.Interrupt.Vector =
               ConfigData->Controller[i].OriginalVector;

            if (ConfigData->Controller[i].InterruptMode == Latched) {

               partial->Flags = CM_RESOURCE_INTERRUPT_LATCHED;

            } else {

               partial->Flags = CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE;

            }

            partial++;

            nextFrd = (PVOID)partial;

            numberOfFrds--;

      }

   }

   IoReportResourceUsage(
      NULL,
      DriverObject,
      resourceList,
      sizeOfResourceList,
      NULL,
      NULL,
      0,
      FALSE,
      &ConfigData->Controller[ControllerNumber].OkToUseThisController
      );

   //
   // The above routine sets the boolean the parameter
   // to TRUE if a conflict was detected.
   //

   ConfigData->Controller[ControllerNumber].OkToUseThisController =
      !ConfigData->Controller[ControllerNumber].OkToUseThisController;

   ExFreePool(resourceList);

   return ConfigData->Controller[ControllerNumber].OkToUseThisController;

}

NTSTATUS
Q117iInitializeDrive(
   IN PCONFIG_DATA ConfigData,
   IN PTAPE_CONTROLLER_DATA ControllerData,
   IN UCHAR ControllerNum,
   IN PDRIVER_OBJECT DriverObject,
   IN PUNICODE_STRING RegistryPath
   )

/*++

Routine Description:

   This routine is called at initialization time by
   Q117iInitializeController(), once for each disk that we are supporting
   on the controller.

Arguments:

   ConfigData - a pointer to the structure that describes the
   controller and the disks attached to it, as given to us by the
   configuration manager.

   ControllerData - a pointer to our data area for this controller.

   ControllerNum - which controller in ConfigData we're working on.

   DisketteNum - which logical disk on the current controller we're
   working on.

   DisketteUnit - which physical disk on the current controller we're
   working on. Only different from DisketteNum when we're creating a
   secondary device object for a previously initialized drive.

   DriverObject - a pointer to the object that represents this device
   driver.

Return Value:

   STATUS_SUCCESS if this disk is initialized; an error otherwise.

--*/

{
   UCHAR ntNameBuffer[256];
   STRING ntNameString;
   UNICODE_STRING ntUnicodeString;
   NTSTATUS ntStatus;
   PDEVICE_OBJECT deviceObject = NULL;
   PTAPE_EXTENSION tapeExtension;
   STATUS retval;

   CheckedDump(QIC117INFO,( "Q117iInitializeDrive...\n" ));

   (VOID) sprintf(
      ntNameBuffer,
      "\\Device\\q117i%d", ConfigData->FloppyTapeCount);

   RtlInitString( &ntNameString, ntNameBuffer );

   ntStatus = RtlAnsiStringToUnicodeString(
      &ntUnicodeString,
      &ntNameString,
      TRUE );

   if ( NT_SUCCESS( ntStatus ) ) {

      //
      // Create a device object for this floppy drive.
      //

      ntStatus = IoCreateDevice(
            DriverObject,
            sizeof( TAPE_EXTENSION ),
            &ntUnicodeString,
            FILE_DEVICE_TAPE,
            FILE_REMOVABLE_MEDIA,
            FALSE,
            &deviceObject );

      RtlFreeUnicodeString(&ntUnicodeString);

   }



   if ( NT_SUCCESS( ntStatus ) ) {

      IoInitializeDpcRequest( deviceObject, Q117iTapeDeferredProcedure );

      tapeExtension = deviceObject->DeviceExtension;
      tapeExtension->SpeedChangeOK = FALSE;
      tapeExtension->PegasusSupported = TRUE;
      tapeExtension->Found = FALSE;
      tapeExtension->NoCart = TRUE;
      tapeExtension->ErrorSequence = 0;
      tapeExtension->TapeNumber = IoGetConfigurationInformation()->TapeCount;
      tapeExtension->DriveParms.Mode = PRIMARY_MODE;
      tapeExtension->DriveParms.Flavor = (UCHAR) UNKNOWN;
      tapeExtension->QDeviceObject = deviceObject;
      tapeExtension->QControllerData = ControllerData;
      tapeExtension->QControllerData->StartFormatMode = FALSE;
      tapeExtension->QControllerData->EndFormatMode = FALSE;
      tapeExtension->QControllerData->ClearQueue = FALSE;
      tapeExtension->QControllerData->AbortRequested = FALSE;
      tapeExtension->QControllerData->AdapterLocked = FALSE;
      tapeExtension->QControllerData->PerpendicularMode = FALSE;
      tapeExtension->XferRate.XferRate = SLOW;
      tapeExtension->XferRate.TapeSlow = TAPE_250Kbps;
      tapeExtension->XferRate.TapeFast = TAPE_500Kbps;
      tapeExtension->XferRate.FDC_Slow = FDC_250Kbps;
      tapeExtension->XferRate.FDC_Fast = FDC_500Kbps;
      tapeExtension->XferRate.SRT_Slow = SRT_250Kbps;
      tapeExtension->XferRate.SRT_Fast = SRT_500Kbps;

      tapeExtension->QControllerData->AdapterObject =
            ConfigData->Controller[ControllerNum].AdapterObject;

      tapeExtension->QControllerData->NumberOfMapRegisters =
            ConfigData->Controller[ControllerNum].NumberOfMapRegisters;

      tapeExtension->QControllerData->TapeExtension = tapeExtension;

#if DBG
      tapeExtension->DbgHead = tapeExtension->DbgTail = 0;
#endif

      tapeExtension->QControllerData->CurrentInterrupt = TRUE;


		retval = Q117iDLocateDrv(tapeExtension);

      tapeExtension->PersistentNewCart = FALSE;
      ntStatus = Q117iTranslateError( deviceObject, retval );
      tapeExtension->PersistentNewCart = TRUE;

      tapeExtension->QControllerData->CurrentInterrupt = FALSE;
   }

   //
   // Initialize the filer level tape device
   //

   if ( NT_SUCCESS( ntStatus ) ) {

      ntStatus = q117Initialize(
                        DriverObject,
                        deviceObject,
                        RegistryPath,
                        ConfigData->Controller[ControllerNum].AdapterObject,
                        ConfigData->Controller[ControllerNum].NumberOfMapRegisters
                        );

   }

   if ( NT_SUCCESS( ntStatus ) ) {

      ConfigData->FloppyTapeCount++;

   } else {

      //
      // If we're failing, clean up and delete the device object.
      //

     CheckedDump(QIC117DBGP,( "Q117i: InitializeDrive failing %x\n", ntStatus ));

      if ( deviceObject != NULL ) {

            IoDeleteDevice( deviceObject );
      }
   }

   return ntStatus;
}

ULONG
Q117iGetControllerBase(
   IN INTERFACE_TYPE BusType,
   IN ULONG BusNumber,
   PHYSICAL_ADDRESS IoAddress,
   ULONG NumberOfBytes,
   BOOLEAN InIoSpace,
   PBOOLEAN MappedAddress
   )

/*++

Routine Description:

   This routine maps an IO address to system address space.

Arguments:

   BusType - what type of bus - eisa, mca, isa
   IoBusNumber - which IO bus (for machines with multiple buses).
   IoAddress - base device address to be mapped.
   NumberOfBytes - number of bytes for which address is valid.
   InIoSpace - indicates an IO address.
   MappedAddress - indicates whether the address was mapped.
                  This only has meaning if the address returned
                  is non-null.

Return Value:

   Mapped address

--*/

{
   PHYSICAL_ADDRESS cardAddress;
   ULONG addressSpace = InIoSpace;
   ULONG Address;

   HalTranslateBusAddress(
            BusType,
            BusNumber,
            IoAddress,
            &addressSpace,
            &cardAddress
            );

   //
   // Map the device base address into the virtual address space
   // if the address is in memory space.
   //

   if (!addressSpace) {

      Address = (ULONG)MmMapIoSpace(
                        cardAddress,
                        NumberOfBytes,
                        FALSE
                        );

      *MappedAddress = (BOOLEAN)((Address)?(TRUE):(FALSE));


   } else {

      Address = (ULONG)cardAddress.LowPart;
   }

   return Address;

}

