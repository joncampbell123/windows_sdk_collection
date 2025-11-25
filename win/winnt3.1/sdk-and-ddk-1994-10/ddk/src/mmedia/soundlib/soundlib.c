/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    soundlib.c

Abstract:

    This module contains common code for Sound Kernel mode device
    drivers.

Author:

    Robin Speed (robinsp) 16-October-1992

Environment:

    Kernel mode

Revision History:


--*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>             // For vsprintf
#include <stdarg.h>            // For va_list
#include <soundlib.h>          // Definition of what's in here

//
// Internal routine definintions
//

NTSTATUS SoundConfigurationCallout(
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
);

//
// Remove initialization stuff from resident memory
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(init,SoundGetBusNumber)
#pragma alloc_text(init,SoundConfigurationCallout)
#pragma alloc_text(init,SoundReportResourceUsage)
#pragma alloc_text(init,SoundCreateDevice)
#pragma alloc_text(init,SoundMapPortAddress)
#pragma alloc_text(init,SoundConnectInterrupt)
#pragma alloc_text(init,SoundSetErrorCode)
#pragma alloc_text(init,SoundSaveRegistryPath)
#endif


NTSTATUS
SoundGetBusNumber(
    IN OUT  INTERFACE_TYPE InterfaceType,
    OUT PULONG BusNumber
)
/*++

Routine Description :

    Find the bus of the type we are looking for - and hope this is
    the one with our card on!  Actually if bus is not
    bus number 0 we fail.

Arguments :

    BusNumber - Where to put the answer

Return Value :

    NT status code - STATUS_SUCCESS if no problems

--*/
{
    ULONG TestBusNumber = 0;
    NTSTATUS Status;
    BOOLEAN Ok = FALSE;   // Must match type passed by reference to
                          // SoundConfigurationCallout by
                          // IoQueryDeviceDescription.

    //
    // See if our bus type exists by calling IoQueryDeviceDescription
    //


    Status = IoQueryDeviceDescription(
                 &InterfaceType,
                 &TestBusNumber,
                 NULL,
                 NULL,
                 NULL,
                 NULL,
                 SoundConfigurationCallout,
                 (PVOID)&Ok);


    if (Ok) {
        *BusNumber = TestBusNumber;
    }

    return Ok ? STATUS_SUCCESS : STATUS_DEVICE_DOES_NOT_EXIST;
}




NTSTATUS
SoundConfigurationCallout(
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

Routine Description :

    Set the return to OK to indicate we've found the bus type
    we were looking for.  Merely being called is confirmation
    that our bus is there.

Arguments :

    See the arguments defined by PIO_QUERY_DEVICE_ROUTINE

Return Value :

    STATUS_SUCCESS if no problems

--*/
{
    *(PBOOLEAN)Context = (BOOLEAN)TRUE;
    return STATUS_SUCCESS;
}




NTSTATUS SoundReportResourceUsage(
    IN PDEVICE_OBJECT DeviceObject,
    IN INTERFACE_TYPE BusType,
    IN ULONG BusNumber,
    IN PULONG InterruptNumber OPTIONAL,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN InterruptShareDisposition,
    IN PULONG DmaChannel OPTIONAL,
    IN PULONG FirstIoPort OPTIONAL,
    IN ULONG IoPortLength
)
/*++

Routine Description :

    Calls IoReportResourceUsage for the device and resources
    passed in.  NOTE that this supercedes previous resources
    declared for this device.

    It is assumed that all resources owned by the device cannot
    be shared, except for level-sensitive interrupts which can be
    shared.

Arguments :

    DeviceObject - The device which 'owns' the resources
                   This can also be a pointer to a driver object
    BusType      - The type of bus on which the device lives
    BusNumber    - The bus number (of type BusType) where the device is
    InterruptNumber - The interrupt the devices uses (if any)
    DmaChannel   - The DMA channel the device uses
    FirstIoPort  - The start Io port for the device
    IoPortLength - The number of bytes of IO space the device uses
                   (starting at FirstIoPort)

Return Value :

    STATUS_SUCCESS if no problems
    The return from IoReportResourceUsage if this fails
    STATUS_DEVICE_CONFIGURATION_ERROR is IoReportResourceUsage reports
        a conflict

--*/

{
    NTSTATUS Status;

    //
    // Our resource list to report back to the system
    //

    /*

     Compiler rejects this

    UCHAR ResBuffer[FIELD_OFFSET(
                       CM_RESOURCE_LIST,
                       List[0].PartialResourceList.PartialDescriptors[3].Type)];

     */

    UCHAR ResBuffer[3 * sizeof(CM_RESOURCE_LIST)];

    BOOLEAN ResourceConflict;

    PCM_RESOURCE_LIST ResourceList;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor;

    ResourceList = (PCM_RESOURCE_LIST)ResBuffer;
    Descriptor = ResourceList->List[0].PartialResourceList.PartialDescriptors;
    ResourceConflict = FALSE;

    //
    // Zero out any unused data
    //

    RtlZeroMemory(ResBuffer, sizeof(ResBuffer));

    //
    // We assume there's only 1 bus so we only need one list.
    // Fill in the bus description
    //

    ResourceList->Count = 1;
    ResourceList->List[0].InterfaceType = BusType;
    ResourceList->List[0].BusNumber = BusNumber;

    //
    // If the device is using IO Ports add this to the list
    //

    if (ARGUMENT_PRESENT(FirstIoPort)) {
        PHYSICAL_ADDRESS PortAddress;
        ULONG MemType;
        PHYSICAL_ADDRESS MappedAddress;

        PortAddress.LowPart = *FirstIoPort;
        PortAddress.HighPart = 0;
        MemType = 1;

        HalTranslateBusAddress(
                    BusType,
                    BusNumber,
                    PortAddress,
                    &MemType,
                    &MappedAddress);


        ResourceList->List[0].PartialResourceList.Count++;

        Descriptor->Type = CmResourceTypePort;

        Descriptor->ShareDisposition = CmResourceShareDeviceExclusive;

        Descriptor->u.Port.Start.LowPart = *FirstIoPort;

        Descriptor->u.Port.Length = IoPortLength;

        Descriptor->Flags = MemType == 0 ? CM_RESOURCE_PORT_MEMORY :
                                           CM_RESOURCE_PORT_IO;

        //
        // Move on to next resource descriptor entry
        //

        Descriptor++;
    }

    //
    // Add interrupt information (if any) to the list
    //

    if (ARGUMENT_PRESENT(InterruptNumber)) {
        ResourceList->List[0].PartialResourceList.Count++;

        Descriptor->Type = CmResourceTypeInterrupt;

        Descriptor->ShareDisposition = InterruptShareDisposition ?
                                       CmResourceShareShared :
                                       CmResourceShareDeviceExclusive;

        Descriptor->Flags =
           InterruptMode == Latched ? CM_RESOURCE_INTERRUPT_LATCHED :
                                      CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE;

        Descriptor->u.Interrupt.Level = *InterruptNumber;

        Descriptor->u.Interrupt.Vector = *InterruptNumber;

        //
        // Move on to next resource descriptor entry
        //

        Descriptor++;
    }

    //
    // Add DMA description if any
    //

    if (ARGUMENT_PRESENT(DmaChannel)) {
        ResourceList->List[0].PartialResourceList.Count++;

        Descriptor->Type = CmResourceTypeDma;

        Descriptor->ShareDisposition = CmResourceShareDeviceExclusive;

        Descriptor->u.Dma.Channel = *DmaChannel;

        Descriptor->u.Dma.Port = 0;  // ???

        //
        // Move on to next resource descriptor entry
        //

        Descriptor++;
    }

    //
    // Report our resource usage and detect conflicts
    //

    switch (DeviceObject->Type) {
    case IO_TYPE_DEVICE:
        Status = IoReportResourceUsage(NULL,
                                       DeviceObject->DriverObject,
                                       NULL,
                                       0,
                                       DeviceObject,
                                       ResourceList,
                                       (PUCHAR)Descriptor - (PUCHAR)ResourceList,
                                       FALSE,
                                       &ResourceConflict);
        break;
    case IO_TYPE_DRIVER:
        Status = IoReportResourceUsage(NULL,
                                       (PDRIVER_OBJECT)DeviceObject,
                                       ResourceList,
                                       (PUCHAR)Descriptor - (PUCHAR)ResourceList,
                                       NULL,
                                       NULL,
                                       0,
                                       FALSE,
                                       &ResourceConflict);
        break;

    default:
        ASSERTMSG("SoundReportResourceUsage - invalid object", FALSE);
    }

    if (ResourceConflict) {
        dprintf1(("Resource conflict reported"));
        Status = STATUS_DEVICE_CONFIGURATION_ERROR;
    }

    return Status;
}




VOID
SoundFreeDevice(
    IN  PDEVICE_OBJECT DeviceObject
)
/*++

Routine Description :

    Free the all resources related to this device :

        The device object itself

        Any declared hardware resources (via IoReportResourceUsage)

        Any symbolic link related to this device

Arguments :

    DeviceObject - the device to free

Return Value :

    None

--*/
{
    CM_RESOURCE_LIST NullResourceList;
    BOOLEAN ResourceConflict;


    //
    // Free the device if any
    //

    if (DeviceObject != NULL) {


        //
        // Undeclare any resources used by the device
        // (delete anything the driver has at the same time!)
        //

        NullResourceList.Count = 0;

        IoReportResourceUsage(NULL,
                              DeviceObject->DriverObject,
                              &NullResourceList,
                              sizeof(ULONG),
                              DeviceObject,
                              &NullResourceList,
                              sizeof(ULONG),
                              FALSE,
                              &ResourceConflict);

        //
        // Remove the device's symbolic link
        //

        {
            PLOCAL_DEVICE_INFO pLDI;
            UNICODE_STRING DeviceName;
            WCHAR Number[8];
            WCHAR TestName[SOUND_MAX_DEVICE_NAME];

            pLDI = DeviceObject->DeviceExtension;

            DeviceName.Buffer = TestName;
            DeviceName.MaximumLength = sizeof(TestName);
            DeviceName.Length = 0;

            RtlAppendUnicodeToString(&DeviceName, L"\\DosDevices");

            RtlAppendUnicodeToString(
                &DeviceName,
                pLDI->DeviceInit->PrototypeName +
                    (sizeof(L"\\Device") - sizeof(UNICODE_NULL)) /
                         sizeof(UNICODE_NULL));

            if (!(pLDI->CreationFlags & SOUND_CREATION_NO_NAME_RANGE)) {
                UNICODE_STRING UnicodeNum;
                WCHAR Number[8];
                UnicodeNum.MaximumLength = sizeof(Number);
                UnicodeNum.Buffer = Number;

                RtlIntegerToUnicodeString(pLDI->DeviceNumber, 10, &UnicodeNum);
                RtlAppendUnicodeStringToString(&DeviceName, &UnicodeNum);
            }

            IoDeleteSymbolicLink(&DeviceName);
        }

        //
        // Delete the device object
        //

        IoDeleteDevice(DeviceObject);
    }
}


NTSTATUS
SoundCreateDevice(
    IN   PSOUND_DEVICE_INIT DeviceInit,
    IN   UCHAR CreationFlags,
    IN   PDRIVER_OBJECT pDriverObject,
    IN   PVOID pGDI,
    IN   PVOID DeviceSpecificData,
    IN   PVOID pHw,
    IN   int i,
    OUT  PDEVICE_OBJECT *ppDevObj
)

/*++

Routine Description:

    Create a new device using a name derived from szPrototypeName
    by adding a number on to the end such that the no device with the
    qualified name exists.

    A symbolic link in \DosDevices is also created

Arguments:

    DeviceInit - device initialization data

    NoRange - if this is set then no number is concatenated, the
          explicit name is used.

    pDriverObject - our driver

    pGDI - global context

    pHw - hardware context

    i - device number for back reference

    ppDevObj - where to write back the device object pointer

Return Value:

    An NTSTATUS code.

--*/

{

    int DeviceNumber;
    NTSTATUS Status;
    UNICODE_STRING DeviceName;
    UNICODE_STRING UnicodeNum;
    WCHAR TestName[SOUND_MAX_DEVICE_NAME];
    WCHAR Number[8];
    OBJECT_ATTRIBUTES ObjectAttributes;
    PLOCAL_DEVICE_INFO pLDI;

#ifdef SOUND_DIRECTORIES
    HANDLE DirectoryHandle = NULL;

    //
    // Create the directory for this device type.
    //

    RtlInitUnicodeString(&DeviceName, DeviceInit->PrototypeName);
    InitializeObjectAttributes(&ObjectAttributes,
                               &DeviceName,
                               OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
                               NULL,
                               (PSECURITY_DESCRIPTOR)NULL);

    //
    // We create the directory if it doesn't exist.
    // We must keep this handle open until we create something as
    // we're not making it permanent.  This means that if we unload
    // the system may be able to get rid of the directory
    //

    Status = ZwCreateDirectoryObject(&DirectoryHandle,
                                     GENERIC_READ,
                                     &ObjectAttributes);

    if (!NT_SUCCESS(Status) && Status != STATUS_OBJECT_NAME_COLLISION) {
        dprintf1(("Return code from NtCreateDirectoryObject = %x", Status));
        return Status;
    } else {
        //
        // Directory is permanent so it won't go away.
        //
        ZwClose(DirectoryHandle);
    }
#endif // SOUND_DIRECTORIES

    for (DeviceNumber = 0; DeviceNumber < SOUND_MAX_DEVICES; DeviceNumber ++) {

        //
        // Create our test name
        //

        DeviceName.Length = 0;
        DeviceName.Buffer = TestName;
        DeviceName.MaximumLength = sizeof(TestName);

        Status = RtlAppendUnicodeToString(&DeviceName, DeviceInit->PrototypeName);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

#ifdef SOUND_DIRECTORIES
        RtlAppendUnicodeToString(&DeviceName, "\\");
#else
#endif // SOUND_DIRECTORIES

        //
        // Append the device number if required
        //

        if (!(CreationFlags & SOUND_CREATION_NO_NAME_RANGE)) {
            UnicodeNum.MaximumLength = sizeof(Number);
            UnicodeNum.Buffer = Number;
            RtlIntegerToUnicodeString(DeviceNumber, 10, &UnicodeNum);

            RtlAppendUnicodeStringToString(&DeviceName, &UnicodeNum);
        } else {
            DeviceNumber = 255;
        }

        Status = IoCreateDevice(
                     pDriverObject,
                     sizeof(LOCAL_DEVICE_INFO),
                     &DeviceName,
                     DeviceInit->Type,
                     0,
                     FALSE,                      // Non-Exclusive
                     ppDevObj
                     );

        if (NT_SUCCESS(Status)) {
            dprintf2(("Created device %d", DeviceNumber));

            //
            // Set up the rest of the device stuff
            //

            (*ppDevObj)->Flags |= DeviceInit->IoMethod;
            (*ppDevObj)->AlignmentRequirement = FILE_BYTE_ALIGNMENT;

            if (DeviceInit->DeferredRoutine) {
                IoInitializeDpcRequest((*ppDevObj), DeviceInit->DeferredRoutine);
            }

            pLDI = (*ppDevObj)->DeviceExtension;
            RtlZeroMemory(pLDI, sizeof(*pLDI));

            //
            // Try to create a symbolic link object for this device
            //
            // No security
            //
            // We make (eg)
            //    \DosDevices\WaveOut0
            // Point to
            //    \Device\WaveOut0
            //

            {
                UNICODE_STRING LinkObject;
                WCHAR LinkName[80];
                ULONG DeviceSize;

                LinkName[0] = UNICODE_NULL;

                RtlInitUnicodeString(&LinkObject, LinkName);

                LinkObject.MaximumLength = sizeof(LinkName);

                RtlAppendUnicodeToString(&LinkObject, L"\\DosDevices");

                DeviceSize = sizeof(L"\\Device") - sizeof(UNICODE_NULL);
                DeviceName.Buffer += DeviceSize / sizeof(WCHAR);
                DeviceName.Length -= DeviceSize;

                RtlAppendUnicodeStringToString(&LinkObject, &DeviceName);

                DeviceName.Buffer -= DeviceSize / sizeof(WCHAR);
                DeviceName.Length += DeviceSize;

                Status = IoCreateSymbolicLink(&LinkObject, &DeviceName);

                if (!NT_SUCCESS(Status)) {
                    dprintf1(("Failed to create symbolic link object"));
                    IoDeleteDevice(*ppDevObj);
                    *ppDevObj = NULL;
                    return Status;
                }
            }

            //
            // Fill in the rest of the device information
            //

#ifdef VOLUME_NOTIFY
            InitializeListHead(&pLDI->VolumeQueue);
#endif // VOLUME_NOTIFY

            pLDI->DeviceNumber = DeviceNumber;
            pLDI->DeviceInit = DeviceInit;
            pLDI->CreationFlags = CreationFlags;

            pLDI->Key = *(PULONG)DeviceInit->Key;
            pLDI->DeviceType = DeviceInit->DeviceType;
            pLDI->DeviceIndex = (UCHAR)i;
            pLDI->pGlobalInfo = pGDI;
            pLDI->DeviceSpecificData = DeviceSpecificData;
            pLDI->HwContext = pHw;

            return STATUS_SUCCESS;
        }
    }
    //
    // Failed !
    //

    return STATUS_INSUFFICIENT_RESOURCES;
}




PUCHAR
SoundMapPortAddress(
    INTERFACE_TYPE BusType,
    ULONG BusNumber,
    ULONG PortBase,
    ULONG Length,
    PULONG MemType
)
/*++

Routine Description :

    Map a physical device port address to an address we can pass
    to READ/WRITE_PORT_UCHAR/USHORT etc

Arguments :
    BusType - type of bus
    BusNumber - bus number
    PortBase - The port start address
    Length - how many bytes of port space to map (needed by MmMapIoSpace)

Return Value :

    The virtual port address

--*/
{
    PHYSICAL_ADDRESS PortAddress;
    PHYSICAL_ADDRESS MappedAddress;

    *MemType = 1;                 // IO space
    PortAddress.LowPart = PortBase;
    PortAddress.HighPart = 0;
    HalTranslateBusAddress(
                BusType,
                BusNumber,
                PortAddress,
                MemType,
                &MappedAddress);

    if (*MemType == 0) {
        //
        // Map memory type IO space into our address space
        //
        return (PUCHAR)MmMapIoSpace(MappedAddress, Length, FALSE);
    } else {
        return (PUCHAR)MappedAddress.LowPart;
    }
}



NTSTATUS
SoundConnectInterrupt(
    IN ULONG InterruptNumber,
    IN INTERFACE_TYPE BusType,
    IN ULONG BusNumber,
    IN PKSERVICE_ROUTINE Isr,
    IN PVOID ServiceContext,
    IN KINTERRUPT_MODE InterruptMode,
    IN BOOLEAN ShareVector,
    OUT PKINTERRUPT *Interrupt
)
/*++

Routine Description :

    Connect to an interrupt.  From this point on our interrupt service
    routine can receive interrupts

    We assume that floating point arithmetic will not be used in the
    service routine.

Arguments :

    InterruptNumber - the interrupt number we're using
    BusType - Our bus type
    BusNumber - the number of our buse (of type BusType)
    Isr - the interrupt service routine
    ServiceContext - a value passed to the interrupt service routine
    InterruptMode - whether it's latched or level sensitive
    ShareVector - whether the interrupt can be shared
    Interrupt - Returns the pointer to the interrupt object

Return Value :

    An NTSTATUS return value - STATUS_SUCCESS if OK.

--*/
{
    KAFFINITY Affinity;
    KIRQL InterruptRequestLevel;
    ULONG InterruptVector;
    NTSTATUS Status;

    //
    // Call HalGetInterruptVector to get the interrupt vector,
    // processor affinity and  request level to pass to IoConnectInterrupt
    //

    InterruptVector = HalGetInterruptVector(Isa,
                                            BusNumber,
                                            InterruptNumber,
                                            InterruptNumber,
                                            &InterruptRequestLevel,
                                            &Affinity);


    Status = IoConnectInterrupt(
                   Interrupt,
                   Isr,
                   ServiceContext,
                   (PKSPIN_LOCK)NULL,
                   InterruptVector,
                   InterruptRequestLevel,
                   InterruptRequestLevel,
                   InterruptMode,
                   ShareVector,
                   Affinity,
                   FALSE                      // No floating point save
                   );

    return Status == STATUS_INVALID_PARAMETER ?
                       STATUS_DEVICE_CONFIGURATION_ERROR : Status;
}



NTSTATUS
SoundSetErrorCode(
    IN   PWSTR RegistryPath,
    IN   ULONG Value
)
/*++

Routine Description :

    Write the given DWORD into the registry using the path
    with a value name of SOUND_REG_CONFIGERROR

Arguments :

    RegistryPath- path to registry key

    Value - value to store

Return Value :

    NTSTATUS code

--*/
{
    return SoundWriteRegistryDWORD(RegistryPath, SOUND_REG_CONFIGERROR, Value);
}


NTSTATUS
SoundWriteRegistryDWORD(
    IN   PWSTR RegistryPath,
    IN   PWSTR ValueName,
    IN   ULONG Value
)
/*++

Routine Description :

    Write the given DWORD into the registry using the path and
    value name supplied by calling RtlWriteRegistryValue

Arguments :

    RegistryPath- path to registry key

    ValueName - name of value to write

    Value - value to store

Return Value :

    NTSTATUS code

--*/
{
    NTSTATUS Status;

    Status = RtlWriteRegistryValue(
                 RTL_REGISTRY_ABSOLUTE,
                 RegistryPath,
                 ValueName,
                 REG_DWORD,
                 &Value,
                 sizeof(Value));

    if (!NT_SUCCESS(Status)) {
        dprintf1(("Writing parameter %ls to registry failed status %8X",
                  ValueName, Status));
    }

    return Status;
}


VOID
SoundDelay(
    IN ULONG Milliseconds
)
/*++

Routine Description :

    Stall the current thread for the given number of milliseconds AT LEAST

Arguments :

    Milliseconds - number of milliseconds to delay

Return Value :

    None

--*/
{
    LARGE_INTEGER Delay;

    //
    // Can't call SoundDelay() from high irql
    //

    if (KeGetCurrentIrql() >= DISPATCH_LEVEL) {
        return;
    }

    //
    // First a tiny delay to synch us up with the timer otherwise we
    // may wait up to 15ms less time than we expected!
    //

    Delay = RtlConvertLongToLargeInteger(-1);

    KeDelayExecutionThread(KernelMode,
                           FALSE,         // Not alertable
                           &Delay);

    Delay = RtlConvertLongToLargeInteger((-(LONG)Milliseconds) * 10000);

    KeDelayExecutionThread(KernelMode,
                           FALSE,         // Not alertable
                           &Delay);
}


LARGE_INTEGER
SoundGetTime(
    VOID
)
/*++

Routine Description:

    Get an accurate estimate of the current time by calling
    KeQueryPerformanceCounter and converting the result to 100ns units

Arguments:

    ErrorText - text of message

Return Value:

--*/
{
    static LARGE_INTEGER StartTime100ns, StartTimeTicks, TicksPerSecond;
    static BOOLEAN Initialized;
    static ULONG Multiplier;
    ULONG Remainder;

    if (!Initialized) {

        Initialized = TRUE;

        KeQuerySystemTime(&StartTime100ns);
        StartTimeTicks = KeQueryPerformanceCounter(&TicksPerSecond);

        Multiplier = 10000000;

        while (TicksPerSecond.HighPart != 0) {
            Multiplier = Multiplier / 10;
            TicksPerSecond =
                RtlExtendedLargeIntegerDivide(TicksPerSecond, 10, &Remainder);
        }
    }

    //
    // Convert ticks to 100ns units (and hope we don't overflow!)
    //

    return RtlLargeIntegerAdd(
              RtlExtendedLargeIntegerDivide(
                  RtlExtendedIntegerMultiply(
                      RtlLargeIntegerSubtract(
                          KeQueryPerformanceCounter(NULL),
                          StartTimeTicks
                      ),
                      Multiplier
                  ),
                  TicksPerSecond.LowPart,
                  &Remainder
              ),
              StartTime100ns
           );
}



VOID
SoundFreeQ(
    PLIST_ENTRY ListHead,
    NTSTATUS IoStatus
)
/*++

Routine Description:

    Free a list of Irps - setting the specified status and completing
    them.  The list will be empty on exit.

Arguments:

    pListNode - the list to free
    IoStatus  - the status to set in each Irp.

Return Value:

    None.

--*/
{
    //
    // Remove all the queue entries, completing all
    // the Irps represented by the entries
    //

    for (;;) {
        PIRP pIrp;

        //
        // The queue may be cancellable so use our routine to get the Irp
        //

        pIrp = SoundRemoveFromCancellableQ(ListHead);

        if (pIrp == NULL) {
            break;
        }

        pIrp->IoStatus.Status = IoStatus;

        //
        // Bump priority here because the application may still be trying
        // to be real-time
        //
        IoCompleteRequest(pIrp, IO_SOUND_INCREMENT);
    }
}


VOID
SoundRemoveAndComplete(
    PDEVICE_OBJECT pDO,
    PIRP Irp
)
/*++

Routine Description:

    Removes the Irp from any queue it's on
    Completes it as cancelled.

Arguments:

    Irp - the Irp
    Cancellable - If it is to be made cancellable

Return Value:

    None.

Notes:

    This routine is called with the cancel spin lock held

--*/
{
    dprintf2(("Cancelling Irp from cancel routine"));

    //
    // Remove the Irp from the queue
    //

    RemoveEntryList(&Irp->Tail.Overlay.ListEntry);

    //
    // Release the cancel spin lock
    //

    IoReleaseCancelSpinLock(Irp->CancelIrql);

    //
    // Set status and complete
    //

    Irp->IoStatus.Status = STATUS_CANCELLED;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
}



VOID
SoundAddIrpToCancellableQ(
    PLIST_ENTRY QueueHead,
    PIRP Irp,
    BOOLEAN Head
)
/*++

Routine Description:


    Add the Irp to the queue and set the cancel routine under the
    protection of the cancel spin lock.

Arguments:

    QueueHead - the queue to add it to
    Irp - the Irp
    Head - if TRUE insert at the head of the queue

Return Value:

    None.

--*/
{
    KIRQL OldIrql;

    //
    // Get the cancel spin lock so we can mess with the cancel stuff
    //

    IoAcquireCancelSpinLock(&OldIrql);

    //
    // Well, it may ALREADY be cancelled!
    //

    if (Irp->Cancel) {

        dprintf2(("Irp already cancelled"));

        //
        // Release the cancel spin lock
        //

        IoReleaseCancelSpinLock(OldIrql);

        //
        // Set status and complete
        //

        Irp->IoStatus.Status = STATUS_CANCELLED;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return;
    }

    //
    // Set the cancel routine
    //

    IoSetCancelRoutine(Irp, SoundRemoveAndComplete);

    //
    // Insert it in the queue
    //

    if (Head) {
        InsertHeadList(QueueHead, &Irp->Tail.Overlay.ListEntry);
    } else {
        InsertTailList(QueueHead, &Irp->Tail.Overlay.ListEntry);
    }

    //
    // Free the spin lock
    //

    IoReleaseCancelSpinLock(OldIrql);
}


PIRP
SoundRemoveFromCancellableQ(
    PLIST_ENTRY QueueHead
)
/*++

Routine Description:


    Remove the Irp to the queue and remove the cancel routine under the
    protection of the cancel spin lock.

Arguments:

    QueueHead - the queue to remove it from

Return Value:

    The Irp at the head of the queue or NULL if the queue is empty.

--*/
{
    KIRQL OldIrql;
    PIRP Irp;
    LIST_ENTRY ListNode;

    //
    // Get the cancel spin lock so we can mess with the cancel stuff
    //

    IoAcquireCancelSpinLock(&OldIrql);


    if (IsListEmpty(QueueHead)) {
        Irp = NULL;
    } else {

        PLIST_ENTRY ListNode;
        ListNode = RemoveHeadList(QueueHead);
        Irp = CONTAINING_RECORD(ListNode, IRP, Tail.Overlay.ListEntry);

        //
        // Remove the cancel routine
        //

        IoSetCancelRoutine(Irp, NULL);
    }

    //
    // Free the spin lock
    //

    IoReleaseCancelSpinLock(OldIrql);

    //
    // Return IRP (if any)
    //

    return Irp;
}



NTSTATUS
SoundSaveRegistryPath(
    IN    PUNICODE_STRING RegistryPathName,
    OUT   PWSTR *SavedString
)
/*++

Routine Description:

    Save the driver's registry path, appending the 'Parameters' key.

    NOTE this registry path must only be accessed BELOW dispatch level
    as the save area is allocated from paged pool.

    The caller must free the unicode string buffer if the driver unloads.

Arguments:

    RegistryPathName - Our driver's registry entry
    SavedString - Saved version of RegistryPathName with the 'Parameters'
         subkey string appended

Return Value:

    NTSTATUS code

--*/
{
    int Length;

    ASSERT(*SavedString == NULL);

    Length =
        RegistryPathName->Length + sizeof(PARMS_SUBKEY) +
              sizeof(UNICODE_NULL);                // Include backslash


    *SavedString =
        ExAllocatePool(PagedPool, Length); // Only access on caller thread

    if (*SavedString == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Copy the character data
    //

    RtlCopyMemory(*SavedString, RegistryPathName->Buffer,
                  RegistryPathName->Length);

    (*SavedString)[RegistryPathName->Length / sizeof(WCHAR)] = L'\\';
    (*SavedString)[RegistryPathName->Length / sizeof(WCHAR) + 1] = UNICODE_NULL;

    //
    // Append the parameters suffix prepended by a backslash
    //

    wcscat(*SavedString, PARMS_SUBKEY);

    return STATUS_SUCCESS;
}


VOID
SoundFlushRegistryKey(
    IN    PWSTR RegistryPathName
)
/*++

Routine Description:

    Flush a key - usually the driver's parameters key -
    used mainly to save volume settings.

Arguments:

    RegistryPathName - Our driver's parameters registry entry

Return Value:

    None

--*/
{

    HANDLE KeyHandle;
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING RegistryPathString;

    RtlInitUnicodeString(&RegistryPathString, RegistryPathName);

    InitializeObjectAttributes(&ObjectAttributes,
                               &RegistryPathString,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               (PSECURITY_DESCRIPTOR)NULL);

    //
    // Just open the key and flush it.  Not much we can do if this
    // fails.
    //

    if (NT_SUCCESS(ZwOpenKey(&KeyHandle,
                             KEY_WRITE,
                             &ObjectAttributes))) {
       ZwFlushKey(KeyHandle);
       ZwClose(KeyHandle);
    } else {
        dprintf1(("Could not open device's key for flushing"));
    }
}

#if 0

VOID
SoundRaiseHardError(
    PWSTR ErrorText
)
/*++

Routine Description:

    Cause a pop-up - note this doesn't seem to work!

Arguments:

    ErrorText - text of message

Return Value:

    None

--*/
{
    UNICODE_STRING String;
    RtlInitUnicodeString(&String, ErrorText);
    IoRaiseInformationalHardError(STATUS_SUCCESS,
                                  &String,
                                  NULL);
}
#endif


#if DBG

char *DriverName = "Unknown Sound Driver";
ULONG SoundDebugLevel = 1;

void dDbgOut(char * szFormat, ...)
{
    char buf[256];
    va_list va;

    va_start(va, szFormat);
    vsprintf(buf, szFormat, va);
    va_end(va);
    DbgPrint("SOUND: %s\n", buf);
}

#endif // DBG



