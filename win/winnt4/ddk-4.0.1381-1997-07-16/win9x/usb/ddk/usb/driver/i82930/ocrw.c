/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

   ocrw.c

Abstract:

   read/write io test code

Environment:

    kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:

    5-4-96 : created

--*/

#define DRIVER

#include "wdm.h"
#include "stdarg.h"
#include "stdio.h"

#include "usbdi.h"
#include "usbdlib.h"
#include "I82930.h"

#include "usb.h"


PURB
I82930_BuildIsoRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PI82930_PIPE PipeHandle,
    IN BOOLEAN Read
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to the device extension for this instance of the
                     82930 device.

    Irp -

    PipeHandle -

Return Value:

    initialized async urb.

--*/
{
    ULONG siz;
    ULONG length, packetSize, numPackets, i;
    PURB urb = NULL;

    I82930_KdPrint (("handle = 0x%x\n", PipeHandle));

    length = MmGetMdlByteCount(Irp->MdlAddress);

    I82930_KdPrint (("length = 0x%x\n", length));

    packetSize = PipeHandle->PipeInfo->MaximumPacketSize;
    numPackets = length/packetSize;
    if (numPackets*packetSize < length) {
        numPackets++;
    }

    siz = GET_ISO_URB_SIZE(numPackets);
    urb = ExAllocatePool(NonPagedPool, siz);

    I82930_KdPrint (("siz = 0x%x urb 0x%x\n", siz, urb));

    if (urb) {
        RtlZeroMemory(urb, siz);

        urb->UrbIsochronousTransfer.Length = (USHORT) siz;
        urb->UrbIsochronousTransfer.Function =
                    URB_FUNCTION_ISOCH_TRANSFER;
        urb->UrbIsochronousTransfer.PipeHandle =
                   PipeHandle->PipeInfo->PipeHandle;
        urb->UrbIsochronousTransfer.TransferFlags =
            Read ? USBD_TRANSFER_DIRECTION_IN : 0;

        urb->UrbIsochronousTransfer.TransferBufferMDL =
            Irp->MdlAddress;
        urb->UrbIsochronousTransfer.TransferBufferLength =
            length;

        // start sending/receiving righ away
        urb->UrbIsochronousTransfer.TransferFlags |=
                USBD_START_ISO_TRANSFER_ASAP;

        urb->UrbIsochronousTransfer.NumberOfPackets = numPackets;
        urb->UrbIsochronousTransfer.ReservedMBZ = 0;

        for (i=0; i< urb->UrbIsochronousTransfer.NumberOfPackets; i++) {
            urb->UrbIsochronousTransfer.IsoPacket[i].Offset
                        = i * packetSize;
        }

        I82930_KdPrint (("Init iso urb Length = 0x%x buf = 0x%x\n",
            urb->UrbIsochronousTransfer.TransferBufferLength,
            urb->UrbIsochronousTransfer.TransferBuffer));
    }

    I82930_KdPrint (("exit I82930_BuildIsoRequest\n"));

    return urb;
}


PURB
I82930_BuildAsyncRequest(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PI82930_PIPE PipeHandle,
    IN BOOLEAN Read
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to the device extension for this instance of the
                     82930 device.

    Irp -

    PipeHandle -

Return Value:

    initialized async urb.

--*/
{
    ULONG siz;
    ULONG length;
    PURB urb = NULL;

    I82930_KdPrint (("handle = 0x%x\n", PipeHandle));

    length = MmGetMdlByteCount(Irp->MdlAddress);

    I82930_KdPrint (("length = 0x%x\n", length));

    siz = sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
    urb = ExAllocatePool(NonPagedPool, siz);

    I82930_KdPrint (("siz = 0x%x urb 0x%x\n", siz, urb));

    if (urb) {
        RtlZeroMemory(urb, siz);

        urb->UrbBulkOrInterruptTransfer.Length = (USHORT) siz;
        urb->UrbBulkOrInterruptTransfer.Function =
                    URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;
        urb->UrbBulkOrInterruptTransfer.PipeHandle =
                   PipeHandle->PipeInfo->PipeHandle;
        urb->UrbBulkOrInterruptTransfer.TransferFlags =
            Read ? USBD_TRANSFER_DIRECTION_IN : 0;

        // short packet is not treated as an error.
        //urb->UrbBulkOrInterruptTransfer.TransferFlags |= 
        //   USBD_SHORT_TRANSFER_DIRECTION_OK;            
                
        //
        // no linkage for now
        //

        urb->UrbBulkOrInterruptTransfer.UrbLink = NULL;

        urb->UrbBulkOrInterruptTransfer.TransferBufferMDL =
            Irp->MdlAddress;
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength =
            length;

        I82930_KdPrint (("Init async urb Length = 0x%x buf = 0x%x\n",
            urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
            urb->UrbBulkOrInterruptTransfer.TransferBuffer));
    }

    I82930_KdPrint (("exit I82930_BuildAsyncRequest\n"));

    return urb;
}


NTSTATUS
I82930_AsyncReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:


Arguments:

    DeviceObject - Pointer to the device object for the i82930 device.

    Irp - Irp completed.

    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb = Context;

    //
    // set the length based on the TransferBufferLength
    // value in the URB
    //
    Irp->IoStatus.Information =
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

    ExFreePool(urb);

    return ntStatus;
}


NTSTATUS
I82930_IsoReadWrite_Complete(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )
/*++

Routine Description:


Arguments:

    DeviceObject - Pointer to the device object for the i82930 device.

    Irp - Irp completed.

    Context - Driver defined context.

Return Value:

    The function value is the final status from the operation.

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb = Context;

    //
    // BUGBUG check here for interesting iso error conditions
    //

    //
    // set the length based on the TransferBufferLength
    // value in the URB
    //
    Irp->IoStatus.Information =
        urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

    ExFreePool(urb);

    return ntStatus;
}


NTSTATUS
I82930_Read(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    devcice.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PI82930_PIPE pipeHandle = NULL;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack, nextStack;
    PDEVICE_EXTENSION deviceExtension;
    PURB urb;

    I82930_KdPrint (("enter I82930_Read\n"));

    deviceExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    pipeHandle =  fileObject->FsContext;

    if (!pipeHandle) {
       ntStatus = STATUS_INVALID_HANDLE;
       goto I82930_Read_Reject;
    }

    //
    // submit the write request to USB
    //

    switch (pipeHandle->PipeInfo->PipeType) {
    case UsbdPipeTypeIsochronous:
        TRAP();
        I82930_ResetPipe(DeviceObject, pipeHandle);
        urb = I82930_BuildIsoRequest(DeviceObject,
                                     Irp,
                                     pipeHandle,
                                     TRUE);
        if (urb) {
            IoMarkIrpPending(Irp);

            nextStack = IoGetNextIrpStackLocation(Irp);
            ASSERT(nextStack != NULL);
            ASSERT(DeviceObject->StackSize>1);

            nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            nextStack->Parameters.Others.Argument1 = urb;
            nextStack->Parameters.DeviceIoControl.IoControlCode =
                IOCTL_INTERNAL_USB_SUBMIT_URB;

            IoSetCompletionRoutine(Irp,
                                   I82930_IsoReadWrite_Complete,
                                   urb,
                                   TRUE,
                                   TRUE,
                                   TRUE);

            I82930_KdPrint (("IRP = 0x%x current = 0x%x next = 0x%x\n",
                Irp, irpStack, nextStack));

            ntStatus = IoCallDriver(deviceExtension->StackDeviceObject,
                                    Irp);
            goto I82930_Read_Done;
        } else {
            ntStatus = STATUS_NO_MEMORY;
        }
        break;
    case UsbdPipeTypeInterrupt:
    case UsbdPipeTypeBulk:
        urb = I82930_BuildAsyncRequest(DeviceObject,
                                       Irp,
                                       pipeHandle,
                                       TRUE);
        if (urb) {
            IoMarkIrpPending(Irp);

            nextStack = IoGetNextIrpStackLocation(Irp);
            ASSERT(nextStack != NULL);
            ASSERT(DeviceObject->StackSize>1);

            nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            nextStack->Parameters.Others.Argument1 = urb;
            nextStack->Parameters.DeviceIoControl.IoControlCode =
                IOCTL_INTERNAL_USB_SUBMIT_URB;

            IoSetCompletionRoutine(Irp,
                                   I82930_AsyncReadWrite_Complete,
                                   urb,
                                   TRUE,
                                   TRUE,
                                   TRUE);

            I82930_KdPrint (("IRP = 0x%x current = 0x%x next = 0x%x\n",
                Irp, irpStack, nextStack));

            ntStatus = IoCallDriver(deviceExtension->StackDeviceObject,
                                    Irp);
            goto I82930_Read_Done;
        } else {
            ntStatus = STATUS_NO_MEMORY;
        }

        break;
    default:
        ntStatus = STATUS_INVALID_PARAMETER;
        TRAP();
    }

I82930_Read_Reject:

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

I82930_Read_Done:

    return ntStatus;
}


NTSTATUS
I82930_Write(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    device.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PI82930_PIPE pipeHandle = NULL;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack, nextStack;
    PDEVICE_EXTENSION deviceExtension;
    PURB urb;

    I82930_KdPrint (("enter I82930_Write\n"));

    deviceExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

//    MmProbeAndLockPages(Irp->MdlAddress,
//                        KernelMode,
//                        IoReadAccess);

    pipeHandle =  fileObject->FsContext;
    if (!pipeHandle)
    {
       ntStatus = STATUS_INVALID_HANDLE;
       goto I82930_Write_Reject;
    }

    //
    // submit the write request to USB
    //

    switch (pipeHandle->PipeInfo->PipeType) {
    case UsbdPipeTypeIsochronous:
        TRAP();
        I82930_ResetPipe(DeviceObject, pipeHandle);
        urb = I82930_BuildIsoRequest(DeviceObject,
                                     Irp,
                                     pipeHandle,
                                     FALSE);
        if (urb) {
            IoMarkIrpPending(Irp);

            nextStack = IoGetNextIrpStackLocation(Irp);
            ASSERT(nextStack != NULL);
            ASSERT(DeviceObject->StackSize>1);

            nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            nextStack->Parameters.Others.Argument1 = urb;
            nextStack->Parameters.DeviceIoControl.IoControlCode =
                IOCTL_INTERNAL_USB_SUBMIT_URB;

            IoSetCompletionRoutine(Irp,
                                   I82930_IsoReadWrite_Complete,
                                   urb,
                                   TRUE,
                                   TRUE,
                                   TRUE);

            I82930_KdPrint (("IRP = 0x%x current = 0x%x next = 0x%x\n",
                Irp, irpStack, nextStack));

            ntStatus = IoCallDriver(deviceExtension->StackDeviceObject,
                                    Irp);
            goto I82930_Write_Done;
        } else {
            ntStatus = STATUS_NO_MEMORY;
        }
        break;
    case UsbdPipeTypeInterrupt:
    case UsbdPipeTypeBulk:
        urb = I82930_BuildAsyncRequest(DeviceObject,
                                       Irp,
                                       pipeHandle,
                                       FALSE);
        if (urb) {
            IoMarkIrpPending(Irp);

            nextStack = IoGetNextIrpStackLocation(Irp);
            ASSERT(nextStack != NULL);
            ASSERT(DeviceObject->StackSize>1);

            nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
            nextStack->Parameters.Others.Argument1 = urb;
            nextStack->Parameters.DeviceIoControl.IoControlCode =
                IOCTL_INTERNAL_USB_SUBMIT_URB;

            IoSetCompletionRoutine(Irp,
                                   I82930_AsyncReadWrite_Complete,
                                   urb,
                                   TRUE,
                                   TRUE,
                                   TRUE);

            I82930_KdPrint (("IRP = 0x%x current = 0x%x next = 0x%x\n",
                Irp, irpStack, nextStack));

            ntStatus = IoCallDriver(deviceExtension->StackDeviceObject,
                                    Irp);
            goto I82930_Write_Done;
        } else {
            ntStatus = STATUS_NO_MEMORY;
        }

        break;
    default:
        ntStatus = STATUS_INVALID_PARAMETER;
        TRAP();
    }

I82930_Write_Reject:

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

I82930_Write_Done:

    return ntStatus;
}


NTSTATUS
I82930_Close(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    devcice.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    PI82930_PIPE pipeHandle = NULL;

    I82930_KdPrint (("entering I82930_Close\n"));

    deviceExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    if (fileObject->FsContext) {
        // closing pipe handle
        pipeHandle =  fileObject->FsContext;
        I82930_KdPrint (("closing pipe %x\n", pipeHandle));

        pipeHandle->Opened = FALSE;
    }

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;


    ntStatus = Irp->IoStatus.Status;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    return ntStatus;
}


NTSTATUS
I82930_Create(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    //
    // Entry point for CreateFile calls
    // user mode apps may open "\\.\I82930-x\yy"
    // where yy is the internal pipe id
    //

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    devcice.


Return Value:

    NT status code

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PI82930_PIPE pipeHandle = NULL;
    PFILE_OBJECT fileObject;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_EXTENSION deviceExtension;
    ULONG i;

    I82930_KdPrint (("entering I82930_Create\n"));

    deviceExtension = DeviceObject->DeviceExtension;
    irpStack = IoGetCurrentIrpStackLocation (Irp);
    fileObject = irpStack->FileObject;

    // fscontext is null for device
    fileObject->FsContext = NULL;

    if (fileObject->FileName.Length != 0) {

        ntStatus = STATUS_NO_MEMORY;

        //
        // a name was specified, convert it to a pipe id
        //

        for (i=0; i<I82930_MAX_PIPES; i++) {
            if (RtlCompareMemory (fileObject->FileName.Buffer,
                                  deviceExtension->PipeList[i].Name,
                                  fileObject->FileName.Length)
                    == fileObject->FileName.Length &&
                !deviceExtension->PipeList[i].Opened) {
                //
                // found a match
                //
                pipeHandle = &deviceExtension->PipeList[i];
                I82930_ResetPipe(DeviceObject, pipeHandle);
                break;
            }
        }
    }

    if (pipeHandle) {
        I82930_KdPrint (("open pipe %x\n", pipeHandle));
        fileObject->FsContext = pipeHandle;
        pipeHandle->Opened = TRUE;
        ntStatus = STATUS_SUCCESS;
    }

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    I82930_KdPrint (("exit I82930_Create %x\n", ntStatus));

    return ntStatus;
}


