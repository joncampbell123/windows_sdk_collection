/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    diskperf.c

Abstract:

    This driver monitors disk accesses capturing performance data.

Authors:

    Mike Glass
    Bob Rinne

Environment:

    kernel mode only

Notes:

Revision History:

--*/

#include "ntddk.h"
#include "stdarg.h"
#include "stdio.h"
#include "ntdddisk.h"

//
// This macro has the effect of Bit = log2(Data)
// and is used to calculate sector shifts.
//

#define WHICH_BIT(Data, Bit) {        \
    for (Bit = 0; Bit < 32; Bit++) {  \
        if ((Data >> Bit) == 1) {     \
            break;                    \
        }                             \
    }                                 \
}

//
// Device Extension
//

typedef struct _DEVICE_EXTENSION {

    //
    // Back pointer to device object
    //

    PDEVICE_OBJECT DeviceObject;

    //
    // Target Device Object
    //

    PDEVICE_OBJECT TargetDeviceObject;

    //
    // Physical Device Object
    //

    PDEVICE_OBJECT PhysicalDevice;

    //
    // Sector size
    //

    ULONG SectorSize;

    //
    // Sector Shift Count
    //

    ULONG SectorShift;

    //
    // Disk performance counters
    //

    DISK_PERFORMANCE DiskCounters;

    //
    // Spinlock for counters (physical disks only)
    //

    KSPIN_LOCK Spinlock;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define DEVICE_EXTENSION_SIZE sizeof(DEVICE_EXTENSION)


//
// Function declarations
//

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

VOID
DiskPerfInitialize(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID NextDisk,
    IN ULONG Count
    );

NTSTATUS
DiskPerfCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskPerfReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskPerfIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    );

NTSTATUS
DiskPerfDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DiskPerfShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This is the routine called by the system to initialize the disk
    performance driver. The driver object is set up and then the
    driver calls DiskPerfInitialize to attach to the boot devices.

Arguments:

    DriverObject - The disk performance driver object.

Return Value:

    NTSTATUS

--*/

{

    //
    // Set up the device driver entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DiskPerfCreate;
    DriverObject->MajorFunction[IRP_MJ_READ] = DiskPerfReadWrite;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = DiskPerfReadWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DiskPerfDeviceControl;
    DriverObject->MajorFunction[IRP_MJ_SHUTDOWN] = DiskPerfShutdownFlush;
    DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS] = DiskPerfShutdownFlush;

    //
    // Call the initialization routine for the first time.
    //

    DiskPerfInitialize(DriverObject, 0, 0);

    return(STATUS_SUCCESS);

} // DriverEntry


VOID
DiskPerfInitialize(
    IN PDRIVER_OBJECT DriverObject,
    IN PVOID NextDisk,
    IN ULONG Count
    )

/*++

Routine Description:

    Attach to new disk devices and partitions.
    Set up device objects for counts and times.
    If this is the first time this routine is called,
    then register with the IO system to be called
    after all other disk device drivers have initiated.

Arguments:

    DriverObject - Disk performance driver object.
    NextDisk - Starting disk for this part of the initialization.
    Count - Not used. Number of times this routine has been called.

Return Value:

    NTSTATUS

--*/

{
    PCONFIGURATION_INFORMATION configurationInformation;
    CCHAR ntNameBuffer[256];
    STRING ntNameString;
    UNICODE_STRING ntUnicodeString;
    PDEVICE_OBJECT deviceObject;
    PDEVICE_OBJECT physicalDevice;
    PDEVICE_EXTENSION deviceExtension;
    PFILE_OBJECT fileObject;
    PIRP irp;
    PDISK_GEOMETRY diskGeometry;
    PDRIVE_LAYOUT_INFORMATION  partitionInfo;
    KEVENT event;
    IO_STATUS_BLOCK ioStatusBlock;
    NTSTATUS status;
    ULONG diskNumber;
    ULONG partNumber;

    //
    // Get the configuration information.
    //

    configurationInformation = IoGetConfigurationInformation();

    //
    // Find disk devices.
    //

    for (diskNumber = (ULONG)NextDisk;
         diskNumber < configurationInformation->DiskCount;
         diskNumber++) {

        //
        // Create device name for the physical disk.
        //

        sprintf(ntNameBuffer,
                "\\Device\\Harddisk%d\\Partition0",
                diskNumber);

        RtlInitAnsiString(&ntNameString,
                          ntNameBuffer);

        RtlAnsiStringToUnicodeString(&ntUnicodeString,
                                     &ntNameString,
                                     TRUE);

        //
        // Create device object for partition 0.
        //

        status = IoCreateDevice(DriverObject,
                                sizeof(DEVICE_EXTENSION),
                                NULL,
                                FILE_DEVICE_DISK,
                                0,
                                FALSE,
                                &physicalDevice);

        physicalDevice->Flags |= DO_DIRECT_IO;

        //
        // Point device extension back at device object.
        //

        deviceExtension = physicalDevice->DeviceExtension;
        deviceExtension->DeviceObject = physicalDevice;

        //
        // This is the physical device object.
        //

        deviceExtension->PhysicalDevice = physicalDevice;

        //
        // Attach to partition0. This call links the newly created
        // device to the target device, returning the target device object.
        //

        status = IoAttachDevice(physicalDevice,
                                &ntUnicodeString,
                                &deviceExtension->TargetDeviceObject);

        if (!NT_SUCCESS(status)) {
            IoDeleteDevice(physicalDevice);
            break;
        }

        RtlFreeUnicodeString(&ntUnicodeString);

        //
        // Allocate buffer for drive geometry.
        //

        diskGeometry = ExAllocatePool(NonPagedPool,
                                      sizeof(DISK_GEOMETRY));

        //
        // Create IRP for get drive geometry device control.
        //

        irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                            deviceExtension->TargetDeviceObject,
                                            NULL,
                                            0,
                                            diskGeometry,
                                            sizeof(DISK_GEOMETRY),
                                            FALSE,
                                            &event,
                                            &ioStatusBlock);

        //
        // Set the event object to the unsignaled state.
        // It will be used to signal request completion.
        //

        KeInitializeEvent(&event,
                          NotificationEvent,
                          FALSE);

        //
        // No need to check the following two returned statuses as
        // ioBlockStatus will have ending status.
        //

        IoCallDriver(deviceExtension->TargetDeviceObject,
                     irp);

        KeWaitForSingleObject(&event,
                              Suspended,
                              KernelMode,
                              FALSE,
                              NULL);

        if (!NT_SUCCESS(ioStatusBlock.Status)) {
            ExFreePool(diskGeometry);
            IoDeleteDevice(physicalDevice);
            break;
        }

        //
        // Store number of bytes per sector.
        //

        deviceExtension->SectorSize = diskGeometry->BytesPerSector;

        //
        // Calculate and store sector shift.
        //

        WHICH_BIT(deviceExtension->SectorSize, deviceExtension->SectorShift);

        //
        // Initialize spinlock for performance measures.
        //

        KeInitializeSpinLock(&deviceExtension->Spinlock);

        //
        // Allocate buffer for drive layout.
        //

        partitionInfo = ExAllocatePool(NonPagedPool,
                                       (26 * sizeof(PARTITION_INFORMATION) + 4));

        //
        // Create IRP for get drive layout device control.
        //

        irp = IoBuildDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_LAYOUT,
                                            deviceExtension->TargetDeviceObject,
                                            NULL,
                                            0,
                                            partitionInfo,
                                            (26 * sizeof(PARTITION_INFORMATION) + 4),
                                            FALSE,
                                            &event,
                                            &ioStatusBlock);

        //
        // Set the event object to the unsignaled state.
        // It will be used to signal request completion.
        //

        KeInitializeEvent(&event,
                          NotificationEvent,
                          FALSE);

        //
        // No need to check the following two returned statuses as
        // ioBlockStatus will have ending status.
        //

        IoCallDriver(deviceExtension->TargetDeviceObject,
                     irp);

        KeWaitForSingleObject(&event,
                              Suspended,
                              KernelMode,
                              FALSE,
                              NULL);

        if (!NT_SUCCESS(ioStatusBlock.Status)) {
            ExFreePool(partitionInfo);
            ExFreePool(diskGeometry);
            IoDeleteDevice(physicalDevice);
            break;
        }

        for (partNumber = 1;
             partNumber < partitionInfo->PartitionCount;
             partNumber++) {

            //
            // Create device name for partition.
            //

            sprintf(ntNameBuffer,
                    "\\Device\\Harddisk%d\\Partition%d",
                    diskNumber,
                    partNumber);

            RtlInitAnsiString(&ntNameString,
                              ntNameBuffer);

            RtlAnsiStringToUnicodeString(&ntUnicodeString,
                                         &ntNameString,
                                         TRUE);

            //
            // Get target device object.
            //

            status = IoGetDeviceObjectPointer(&ntUnicodeString,
                                              FILE_READ_ATTRIBUTES,
                                              &fileObject,
                                              &deviceObject);

            if (!NT_SUCCESS(status)) {
                RtlFreeUnicodeString(&ntUnicodeString);
                continue;
            }

            //
            // Check if this device is already mounted.
            //

            if (!deviceObject->Vpb ||
                (deviceObject->Vpb->Flags & VPB_MOUNTED)) {

                //
                // Can't attach to a device that is already mounted.
                //

                ObDereferenceObject(fileObject);
                RtlFreeUnicodeString(&ntUnicodeString);
                continue;
            }

            ObDereferenceObject(fileObject);

            //
            // Create device object for this partition.
            //

            status = IoCreateDevice(DriverObject,
                                    sizeof(DEVICE_EXTENSION),
                                    NULL,
                                    FILE_DEVICE_DISK,
                                    0,
                                    FALSE,
                                    &deviceObject);

            deviceObject->Flags |= DO_DIRECT_IO;

            //
            // Point device extension back at device object.
            //

            deviceExtension = deviceObject->DeviceExtension;
            deviceExtension->DeviceObject = deviceObject;

            //
            // Store pointer to physical device.
            //

            deviceExtension->PhysicalDevice = physicalDevice;

            //
            // Attach to the partition. This call links the newly created
            // device to the target device, returning the target device object.
            //

            status = IoAttachDevice(deviceObject,
                                    &ntUnicodeString,
                                    &deviceExtension->TargetDeviceObject);

            RtlFreeUnicodeString(&ntUnicodeString);

            if (!NT_SUCCESS(status)) {
                ExFreePool(diskGeometry);
                IoDeleteDevice(deviceObject);
                break;
            }
        }
    }

    //
    // Check if this is the first time this routine has been called.
    //

    if (!NextDisk) {

        //
        // Register with IO system to be called a second time after all
        // other device drivers have initialized.
        //

        IoRegisterDriverReinitialization(DriverObject,
                                         DiskPerfInitialize,
                                         (PVOID)configurationInformation->DiskCount);
    }

    return;

} // end DiskPerfInitialize()


NTSTATUS
DiskPerfCreate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine services open commands. It establishes
    the driver's existance by returning status success.

Arguments:

    DeviceObject - Context for the activity.
    Irp          - The device control argument block.

Return Value:

    NT Status

--*/

{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;

    IoCompleteRequest(Irp, 0);
    return STATUS_SUCCESS;

} // end DiskPerfCreate()


NTSTATUS
DiskPerfReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This is the driver entry point for read and write requests
    to disks to which the diskperf driver has attached.
    This driver collects statistics and then sets a completion
    routine so that it can collect additional information when
    the request completes. Then it calls the next driver below
    it.

Arguments:

    DeviceObject
    Irp

Return Value:

    NTSTATUS

--*/

{
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PDEVICE_EXTENSION physicalDisk =
        deviceExtension->PhysicalDevice->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);

    //
    // Increment queue depth counter.
    //

    ExInterlockedIncrementLong(&deviceExtension->DiskCounters.QueueDepth,
                               &physicalDisk->Spinlock);

    //
    // Now get the physical disk counters and increment queue depth.
    //

    ExInterlockedIncrementLong(&physicalDisk->DiskCounters.QueueDepth,
                               &physicalDisk->Spinlock);

    //
    // Copy current stack to next stack.
    //

    *nextIrpStack = *currentIrpStack;

    //
    // Time stamp current request start.
    //

    currentIrpStack->Parameters.Read.ByteOffset = KeQueryPerformanceCounter((PVOID)NULL);

    //
    // Set completion routine callback.
    //

    IoSetCompletionRoutine(Irp,
                           DiskPerfIoCompletion,
                           DeviceObject,
                           TRUE,
                           TRUE,
                           TRUE);

    //
    // Return the results of the call to the disk driver.
    //

    return IoCallDriver(deviceExtension->TargetDeviceObject,
                        Irp);

} // end DiskPerfReadWrite()


NTSTATUS
DiskPerfIoCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID Context
    )

/*++

Routine Description:

    This routine will get control from the system at the completion of an IRP.
    It will calculate the difference between the time the IRP was started
    and the current time, and decrement the queue depth.

Arguments:

    DeviceObject - for the IRP.
    Irp          - The I/O request that just completed.
    Context      - Not used.

Return Value:

    The IRP status.

--*/

{
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PDEVICE_EXTENSION physicalDisk =
        deviceExtension->PhysicalDevice->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PDISK_PERFORMANCE partitionCounters = &deviceExtension->DiskCounters;
    PDISK_PERFORMANCE diskCounters = &physicalDisk->DiskCounters;
    LARGE_INTEGER timeStampStart =
        irpStack->Parameters.Read.ByteOffset;
    LARGE_INTEGER timeStampComplete;
    KIRQL currentIrql;

    UNREFERENCED_PARAMETER(Context);

    //
    // Determine if STATUS_PENDING was returned in the dispatch routine.
    //

    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }
    //
    // Time stamp current request complete.
    //

    timeStampComplete = KeQueryPerformanceCounter((PVOID)NULL);

    //
    // Update counters under spinlock protection.
    //

    KeAcquireSpinLock(&physicalDisk->Spinlock, &currentIrql);

    //
    // Decrement the queue depth counters for the volume and physical disk.
    //

    partitionCounters->QueueDepth--;
    diskCounters->QueueDepth--;

#if DBG

    //
    // Verify that the information field, which is used to
    // return bytes transferred, is being updated by lower drivers.
    //

    if (NT_SUCCESS(Irp->IoStatus.Status)) {
        ASSERT(Irp->IoStatus.Information);
    }

#endif

    if (irpStack->MajorFunction == IRP_MJ_READ) {

        //
        // Add bytes in this request to bytes read counters.
        //

        partitionCounters->BytesRead = RtlLargeIntegerAdd(partitionCounters->BytesRead,
            RtlConvertUlongToLargeInteger(Irp->IoStatus.Information));

        diskCounters->BytesRead = RtlLargeIntegerAdd(diskCounters->BytesRead,
            RtlConvertUlongToLargeInteger(Irp->IoStatus.Information));

        //
        // Increment read requests processed counters.
        //

        partitionCounters->ReadCount++;
        diskCounters->ReadCount++;

        //
        // Calculate request processing time.
        //

        partitionCounters->ReadTime = RtlLargeIntegerAdd(partitionCounters->ReadTime,
            RtlLargeIntegerSubtract(timeStampComplete, timeStampStart));

        diskCounters->ReadTime = RtlLargeIntegerAdd(diskCounters->ReadTime,
            RtlLargeIntegerSubtract(timeStampComplete, timeStampStart));

    } else {

        //
        // Add bytes in this request to bytes write counters.
        //

        partitionCounters->BytesWritten = RtlLargeIntegerAdd(partitionCounters->BytesWritten,
            RtlConvertUlongToLargeInteger(Irp->IoStatus.Information));

        diskCounters->BytesWritten = RtlLargeIntegerAdd(diskCounters->BytesWritten,
            RtlConvertUlongToLargeInteger(Irp->IoStatus.Information));

        //
        // Increment write requests processed counters.
        //

        partitionCounters->WriteCount++;
        diskCounters->WriteCount++;

        //
        // Calculate request processing time.
        //

        partitionCounters->WriteTime = RtlLargeIntegerAdd(partitionCounters->WriteTime,
            RtlLargeIntegerSubtract(timeStampComplete, timeStampStart));

        diskCounters->WriteTime = RtlLargeIntegerAdd(diskCounters->WriteTime,
            RtlLargeIntegerSubtract(timeStampComplete, timeStampStart));
    }

    //
    // Release spinlock.
    //

    KeReleaseSpinLock(&physicalDisk->Spinlock, currentIrql);

    return Irp->IoStatus.Status;

} // DiskPerfIoCompletion


NTSTATUS
DiskPerfDeviceControl(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )

/*++

Routine Description:

    This device control dispatcher handles only the disk performance
    device control. All others are passed down to the disk drivers.
    The disk performane device control returns a current snapshot of
    the performance data.

Arguments:

    DeviceObject - Context for the activity.
    Irp          - The device control argument block.

Return Value:

    Status is returned.

--*/

{
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    KIRQL currentIrql;

    if (currentIrpStack->Parameters.DeviceIoControl.IoControlCode ==
        IOCTL_DISK_PERFORMANCE) {

        NTSTATUS status;

        //
        // Verify user buffer is large enough for the performance data.
        //

        if (currentIrpStack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(DISK_PERFORMANCE)) {

            //
            // Indicate unsuccessful status and no data transferred.
            //

            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;

        } else {

            PDEVICE_EXTENSION physicalDisk =
                deviceExtension->PhysicalDevice->DeviceExtension;

            //
            // Copy disk counters to buffer under spinlock protection.
            //

            KeAcquireSpinLock(&physicalDisk->Spinlock, &currentIrql);

            RtlMoveMemory(Irp->AssociatedIrp.SystemBuffer,
                          &deviceExtension->DiskCounters,
                          sizeof(DISK_PERFORMANCE));

            KeReleaseSpinLock(&physicalDisk->Spinlock, currentIrql);

            //
            // Set IRP status to success and indicate bytes transferred.
            //

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(DISK_PERFORMANCE);
        }

        //
        // Complete request.
        //

        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, 0);
        return status;

    }  else {

        PIO_STACK_LOCATION nextIrpStack    = IoGetNextIrpStackLocation(Irp);

#if 0
        //
        // Copy stack parameters to next stack.
        //

        RtlMoveMemory(nextIrpStack,
                      currentIrpStack,
                      sizeof(IO_STACK_LOCATION));

        //
        // Set IRP so IoComplete does not call completion routine
        // for this driver.
        //

        IoSetCompletionRoutine(Irp,
                               NULL,
                               DeviceObject,
                               FALSE,
                               FALSE,
                               FALSE);
#endif

        //
        // Set current stack back one.
        //

        Irp->CurrentLocation++,
        Irp->Tail.Overlay.CurrentStackLocation++;

        //
        // Pass unrecognized device control requests
        // down to next driver layer.
        //

        return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    }

} // end DiskPerfDeviceControl()


NTSTATUS
DiskPerfShutdownFlush(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This routine is called for a shutdown and flush IRPs.  These are sent by the
    system before it actually shuts down or when the file system does a flush.

Arguments:

    DriverObject - Pointer to device object to being shutdown by system.
    Irp          - IRP involved.

Return Value:

    NT Status

--*/

{
    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;

    //
    // Set current stack back one.
    //

    Irp->CurrentLocation++,
    Irp->Tail.Overlay.CurrentStackLocation++;

    return IoCallDriver(deviceExtension->TargetDeviceObject, Irp);

} // end DiskPerfShutdownFlush()

