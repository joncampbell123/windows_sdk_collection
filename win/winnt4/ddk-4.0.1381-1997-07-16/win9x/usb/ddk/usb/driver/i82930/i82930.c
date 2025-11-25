/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

    I82930.c

Abstract:

    USB device driver for Intel 82930 USB test board

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

//
// Global pointer to Driver Object
//

PDRIVER_OBJECT I82930_DriverObject;

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    DriverObject - pointer to the driver object

    RegistryPath - pointer to a unicode string representing the path
                   to driver-specific key in the registry

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT deviceObject = NULL;

    I82930_KdPrint (("entering (I82930) DriverEntry\n"));

    I82930_DriverObject = DriverObject;

    //
    // Create dispatch points for device control, create, close.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE] = I82930_Create;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = I82930_Close;
    DriverObject->DriverUnload = I82930_Unload;

    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = I82930_ProcessIOCTL;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = I82930_Write;
    DriverObject->MajorFunction[IRP_MJ_READ] = I82930_Read;

    DriverObject->MajorFunction[IRP_MJ_PNP_POWER] = I82930_Dispatch;
    DriverObject->DriverExtension->AddDevice = I82930_PnPAddDevice;

    I82930_KdPrint (("exiting (I82930) DriverEntry (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_Dispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp
    )
/*++

Routine Description:

    Process the IRPs sent to this device.

Arguments:

    DeviceObject - pointer to a device object

    Irp          - pointer to an I/O Request Packet

Return Value:


--*/
{

    PIO_STACK_LOCATION irpStack, nextStack;
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PDEVICE_OBJECT stackDeviceObject;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    //

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    //
    // Get a pointer to the device extension
    //

    deviceExtension = DeviceObject->DeviceExtension;
    stackDeviceObject = deviceExtension->StackDeviceObject;

    switch (irpStack->MajorFunction) {

    case IRP_MJ_PNP_POWER:

        I82930_KdPrint (("IRP_MJ_PNP_POWER\n"));

        switch (irpStack->MinorFunction) {
        case IRP_MN_START_DEVICE:

            I82930_KdPrint (("IRP_MN_START_DEVICE\n"));

            ntStatus = I82930_StartDevice(DeviceObject);

            break;

        case IRP_MN_STOP_DEVICE:

            I82930_KdPrint (("IRP_MN_STOP_DEVICE\n"));

            I82930_Cleanup(DeviceObject);
            ntStatus = I82930_StopDevice(DeviceObject);

            break;

        case IRP_MN_REMOVE_DEVICE:

            I82930_KdPrint (("IRP_MN_REMOVE_DEVICE\n"))

            I82930_Cleanup(DeviceObject);
            ntStatus = I82930_RemoveDevice(DeviceObject);

            //
            // Delete the link and FDO we created
            //

            IoDetachDevice(deviceExtension->StackDeviceObject);

            IoDeleteDevice (DeviceObject);

            break;

        case IRP_MN_SET_POWER:

            switch (irpStack->Parameters.Power.Type) {
            case SystemPowerState:
            case DeviceSpecificPowerState:
                // pass on to PDO
                break;

            case DevicePowerState:
                switch (irpStack->Parameters.Power.State.DeviceState) {
                case PowerDeviceD3:
                    I82930_KdPrint (("IRP_MN_SET_D3\n"));
                    break;
                case PowerDeviceD2:
                    I82930_KdPrint (("IRP_MN_SET_D2\n"));
                    break;
                case PowerDeviceD1:
                    I82930_KdPrint (("IRP_MN_SET_D1\n"));
                    break;
                case PowerDeviceD0:
                    I82930_KdPrint (("IRP_MN_SET_D0\n"));
                    break;
                }
            }
            // pass on to PDO
            break;

         case IRP_MN_QUERY_POWER:

            switch (irpStack->Parameters.Power.Type) {
            case SystemPowerState:
            case DeviceSpecificPowerState:
                // pass on to PDO
                break;

            case DevicePowerState:
                switch (irpStack->Parameters.Power.State.DeviceState) {
                case PowerDeviceD2:
                    I82930_KdPrint (("IRP_MN_QUERY_D2\n"));
                    break;
                case PowerDeviceD1:
                    I82930_KdPrint (("IRP_MN_QUERY_D1\n"));
                    break;
                case PowerDeviceD3:
                    I82930_KdPrint (("IRP_MN_QUERY_D3\n"));
                    break;
                }
            }
            // pass on to PDO
            break;

        case IRP_MN_QUERY_STOP_DEVICE:
            I82930_KdPrint (("IRP_MN_QUERY_STOP_DEVICE\n"));
            break;
        case IRP_MN_QUERY_REMOVE_DEVICE:
            I82930_KdPrint (("IRP_MN_QUERY_REMOVE_DEVICE\n"));
            break;
        case IRP_MN_CANCEL_STOP_DEVICE:
            I82930_KdPrint (("IRP_MN_CANCEL_STOP_DEVICE\n"));
            break;
        case IRP_MN_CANCEL_REMOVE_DEVICE:
            I82930_KdPrint (("IRP_MN_CANCEL_REMOVE_DEVICE\n"));
            break;
        default:

            I82930_KdPrint (("PnP IOCTL not handled\n"));
        } /* case MajorFunction == IRP_MJ_PNP_POWER  */

        nextStack = IoGetNextIrpStackLocation(Irp);
        ASSERT(nextStack != NULL);
        RtlCopyMemory(nextStack, irpStack, sizeof(IO_STACK_LOCATION));

        //
        // All PNP_POWER messages get passed to the StackDeviceObject
        // we were given in PnPAddDevice
        //

        IoMarkIrpPending(Irp);

        I82930_KdPrint (("Passing PnP Irp down, status = %x\n", ntStatus));

        ntStatus = IoCallDriver(stackDeviceObject,
                                Irp);

        goto I82930_Dispatch_Done;
        break;

    default:
        I82930_KdPrint (("MAJOR IOCTL not handled\n"));
        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;

    } /* case MajorFunction */


    ntStatus = Irp->IoStatus.Status;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

I82930_Dispatch_Done:

    I82930_KdPrint (("Exit I82930_Dispatch %x\n", ntStatus));

    return ntStatus;
}


VOID
I82930_Unload(
    IN PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Free all the allocated resources, etc.

Arguments:

    DriverObject - pointer to a driver object

Return Value:


--*/
{
    I82930_KdPrint (("enter I82930_Unload\n"));

    //
    // Free any global resources allocated
    // in DriverEntry
    //

    //BUGBUG to see if we ever het called
    TRAP();

    I82930_KdPrint (("exit I82930_Unload\n"));
}


NTSTATUS
I82930_StartDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Initializes a given instance of the device on the USB.
    All we do here is get the device descriptor and store it

Arguments:

    DeviceObject - pointer to the device object for this instance of a
                    82930

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PUSB_DEVICE_DESCRIPTOR deviceDescriptor = NULL;
    PURB urb;
    ULONG siz;

    I82930_KdPrint (("enter I82930_StartDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;
    deviceExtension->NeedCleanup = TRUE;

    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if (urb) {

        siz = sizeof(USB_DEVICE_DESCRIPTOR);

        deviceDescriptor = ExAllocatePool(NonPagedPool,
                                          siz);

        if (deviceDescriptor) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_DEVICE_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         deviceDescriptor,
                                         NULL,
                                         siz,
                                         NULL);

            ntStatus = I82930_CallUSBD(DeviceObject, urb);

            if (NT_SUCCESS(ntStatus)) {
                I82930_KdPrint (("Device Descriptor = %x, len %x\n",
                                deviceDescriptor,
                                urb->UrbControlDescriptorRequest.TransferBufferLength));

                I82930_KdPrint (("I82930 Device Descriptor:\n"));
                I82930_KdPrint (("-------------------------\n"));
                I82930_KdPrint (("bLength %d\n", deviceDescriptor->bLength));
                I82930_KdPrint (("bDescriptorType 0x%x\n", deviceDescriptor->bDescriptorType));
                I82930_KdPrint (("bcdUSB 0x%x\n", deviceDescriptor->bcdUSB));
                I82930_KdPrint (("bDeviceClass 0x%x\n", deviceDescriptor->bDeviceClass));
                I82930_KdPrint (("bDeviceSubClass 0x%x\n", deviceDescriptor->bDeviceSubClass));
                I82930_KdPrint (("bDeviceProtocol 0x%x\n", deviceDescriptor->bDeviceProtocol));
                I82930_KdPrint (("bMaxPacketSize0 0x%x\n", deviceDescriptor->bMaxPacketSize0));
                I82930_KdPrint (("idVendor 0x%x\n", deviceDescriptor->idVendor));
                I82930_KdPrint (("idProduct 0x%x\n", deviceDescriptor->idProduct));
                I82930_KdPrint (("bcdDevice 0x%x\n", deviceDescriptor->bcdDevice));
                I82930_KdPrint (("iManufacturer 0x%x\n", deviceDescriptor->iManufacturer));
                I82930_KdPrint (("iProduct 0x%x\n", deviceDescriptor->iProduct));
                I82930_KdPrint (("iSerialNumber 0x%x\n", deviceDescriptor->iSerialNumber));
                I82930_KdPrint (("bNumConfigurations 0x%x\n", deviceDescriptor->bNumConfigurations));
            }
        } else {
            ntStatus = STATUS_NO_MEMORY;
        }

        if (NT_SUCCESS(ntStatus)) {
            deviceExtension->DeviceDescriptor = deviceDescriptor;
        } else if (deviceDescriptor) {
            ExFreePool(deviceDescriptor);
        }

        ExFreePool(urb);

    } else {
        ntStatus = STATUS_NO_MEMORY;
    }

    if (NT_SUCCESS(ntStatus)) {
        ntStatus = I82930_ConfigureDevice(DeviceObject);
    }

    I82930_KdPrint (("exit I82930_StartDevice (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_RemoveDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Stops a given instance of a 82930 device on the USB.

Arguments:

    DeviceObject - pointer to the device object for this instance of a 82930

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;

    I82930_KdPrint (("enter I82930_RemoveDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Free device descriptor structure
    //

    if (deviceExtension->DeviceDescriptor) {
        ExFreePool(deviceExtension->DeviceDescriptor);
    }

    //
    // Free up any interface structures
    //

    if (deviceExtension->Interface) {
        ExFreePool(deviceExtension->Interface);
    }

    I82930_KdPrint (("exit I82930_RemoveDevice (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_StopDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Stops a given instance of a 82930 device on the USB, this is only
    stuff we need to do if the device is still present.

Arguments:

    DeviceObject - pointer to the device object for this instance of a 82930

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PURB urb;
    ULONG siz;

    I82930_KdPrint (("enter I82930_StopDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // Send the select configuration urb with a NULL pointer for the configuration
    // handle, this closes the configuration and puts the device in the 'unconfigured'
    // state.
    //

    siz = sizeof(struct _URB_SELECT_CONFIGURATION);

    urb = ExAllocatePool(NonPagedPool,
                         siz);

    if (urb) {
        NTSTATUS status;

        UsbBuildSelectConfigurationRequest(urb,
                                          (USHORT) siz,
                                          NULL);

        status = I82930_CallUSBD(DeviceObject, urb);

        I82930_KdPrint (("Device Configuration Closed status = %x usb status = %x.\n",
                        status, urb->UrbHeader.Status));

        ExFreePool(urb);
    } else {
        ntStatus = STATUS_NO_MEMORY;
    }

    I82930_KdPrint (("exit I82930_StopDevice (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_PnPAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Routine Description:

    This routine is called to create a new instance of the device

Arguments:

    DriverObject - pointer to the driver object for this instance of I82930

    PhysicalDeviceObject - pointer to a device object created by the bus

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS                ntStatus = STATUS_SUCCESS;
    PDEVICE_OBJECT          deviceObject = NULL;
    PDEVICE_EXTENSION       deviceExtension;

    I82930_KdPrint(("enter I82930_PnPAddDevice\n"));

    //
    // create our funtional device object (FDO)
    //

    ntStatus =
        I82930_CreateDeviceObject(DriverObject, &deviceObject, 0);

    if (NT_SUCCESS(ntStatus)) {
        deviceExtension = deviceObject->DeviceExtension;

        deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

        //
        // we support direct io for read/write
        //
        deviceObject->Flags |= DO_DIRECT_IO;


        //** initialize our device extension
        //
        // remember the Physical device Object
        //
        deviceExtension->PhysicalDeviceObject=PhysicalDeviceObject;

        //
        // Attach to the PDO
        //

        deviceExtension->StackDeviceObject =
            IoAttachDeviceToDeviceStack(deviceObject, PhysicalDeviceObject);
    }


    I82930_KdPrint(("exit I82930_PnPAddDevice (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_CreateDeviceObject(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT *DeviceObject,
    LONG Instance
    )
/*++

Routine Description:

    Creates a Functional DeviceObject

Arguments:

    DriverObject - pointer to the driver object for device

    DeviceObject - pointer to DeviceObject pointer to return
                    created device object.

    Instance - instnace of the device create.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus;
    WCHAR deviceLinkBuffer[]  = L"\\DosDevices\\I82930-0";
    UNICODE_STRING deviceLinkUnicodeString;
    WCHAR deviceNameBuffer[]  = L"\\Device\\I82930-0";
    UNICODE_STRING deviceNameUnicodeString;
    PDEVICE_EXTENSION deviceExtension;
    ULONG i;

    I82930_KdPrint(("enter I82930_CreateDeviceObject instance = %d\n", Instance));

    //
    // fix up device names based on Instance
    //

    //
    // BUGBUG we eventually want to use a name derived from PnP

    deviceLinkBuffer[19] = (USHORT) ('0' + Instance);
    deviceNameBuffer[15] = (USHORT) ('0' + Instance);

    I82930_KdPrint(("Create Device name (%ws)\n", deviceNameBuffer));

    RtlInitUnicodeString (&deviceNameUnicodeString,
                          deviceNameBuffer);

    ntStatus = IoCreateDevice (DriverObject,
                               sizeof (DEVICE_EXTENSION),
                               &deviceNameUnicodeString,
                               FILE_DEVICE_UNKNOWN,
                               0,
                               FALSE,
                               DeviceObject);


    if (NT_SUCCESS(ntStatus)) {
        RtlInitUnicodeString (&deviceLinkUnicodeString,
                              deviceLinkBuffer);

        I82930_KdPrint(("Create DosDevice name (%ws)\n", deviceLinkBuffer));

        ntStatus = IoCreateSymbolicLink (&deviceLinkUnicodeString,
                                         &deviceNameUnicodeString);

        //
        // Initialize our device extension
        //

        deviceExtension = (PDEVICE_EXTENSION) ((*DeviceObject)->DeviceExtension);

        RtlCopyMemory(deviceExtension->DeviceLinkNameBuffer,
                      deviceLinkBuffer,
                      sizeof(deviceLinkBuffer));
        deviceExtension->DeviceDescriptor = NULL;
        deviceExtension->Interface = NULL;
        deviceExtension->NeedCleanup = FALSE;
        deviceExtension->ConfigurationHandle = NULL;
        for (i=0; i<I82930_MAX_PIPES; i++) {
            deviceExtension->PipeList[i].PipeInfo = NULL;
        }
    }

    I82930_KdPrint(("exit I82930_CreateDeviceObject (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_CallUSBD(
    IN PDEVICE_OBJECT DeviceObject,
    IN PURB Urb
    )
/*++

Routine Description:

    Passes a URB to the USBD class driver

Arguments:

    DeviceObject - pointer to the device object for this instance of an 82930

    Urb - pointer to Urb request block

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise

--*/
{
    NTSTATUS ntStatus, status = STATUS_SUCCESS;
    PDEVICE_EXTENSION deviceExtension;
    PIRP irp;
    KEVENT event;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION nextStack;

    I82930_KdPrint (("enter I82930_CallUSBD\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // issue a synchronous request
    //

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    irp = IoBuildDeviceIoControlRequest(
                IOCTL_INTERNAL_USB_SUBMIT_URB,
                deviceExtension->StackDeviceObject,
                NULL,
                0,
                NULL,
                0,
                TRUE, /* INTERNAL */
                &event,
                &ioStatus);

    //
    // Call the class driver to perform the operation.  If the returned status
    // is PENDING, wait for the request to complete.
    //

    nextStack = IoGetNextIrpStackLocation(irp);
    ASSERT(nextStack != NULL);

    //
    // pass the URB to the USB driver stack
    //
    nextStack->Parameters.Others.Argument1 = Urb;


    I82930_KdPrint (("calling USBD\n"));

    ntStatus = IoCallDriver(deviceExtension->StackDeviceObject,
                            irp);

    I82930_KdPrint (("return from IoCallDriver USBD %x\n", ntStatus));

    if (ntStatus == STATUS_PENDING) {
        I82930_KdPrint (("Wait for single object\n"));

        status = KeWaitForSingleObject(
                       &event,
                       Suspended,
                       KernelMode,
                       FALSE,
                       NULL);

        I82930_KdPrint (("Wait for single object, returned %x\n", status));

    } else {
        ioStatus.Status = ntStatus;
    }

    I82930_KdPrint (("URB status = %x status = %x irp status %x\n",
        Urb->UrbHeader.Status, status, ioStatus.Status));

    //
    // USBD maps the error code for us
    //
    ntStatus = ioStatus.Status;

    I82930_KdPrint(("exit I82930_CallUSBD (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_ConfigureDevice(
    IN  PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    Initializes a given instance of the device on the USB and selects the
    configuration.

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    devcice.


Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;

    I82930_KdPrint (("enter I82930_ConfigureDevice\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    //
    // first configure the device
    //

    urb = ExAllocatePool(NonPagedPool,
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

    if (urb) {

        // BUGBUG 82930 chokes if on the next command if you don't get
        // the entire descriptor on the first try

        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;

get_config_descriptor_retry:

        configurationDescriptor = ExAllocatePool(NonPagedPool,
                                                 siz);

        if (configurationDescriptor) {

            UsbBuildGetDescriptorRequest(urb,
                                         (USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
                                         USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                         0,
                                         0,
                                         configurationDescriptor,
                                         NULL,
                                         siz,
                                         NULL);

            ntStatus = I82930_CallUSBD(DeviceObject, urb);

            I82930_KdPrint (("Configuration Descriptor = %x, len %x\n",
                            configurationDescriptor,
                            urb->UrbControlDescriptorRequest.TransferBufferLength));
        } else {
            ntStatus = STATUS_NO_MEMORY;
        }

        //
        // if we got some data see if it was enough.
        //
        // NOTE: we may get an error in URB because of buffer overrun
        if (urb->UrbControlDescriptorRequest.TransferBufferLength>0 &&
                configurationDescriptor->wTotalLength > siz) {

            siz = configurationDescriptor->wTotalLength;
            ExFreePool(configurationDescriptor);
            configurationDescriptor = NULL;
            goto get_config_descriptor_retry;
        }

        ExFreePool(urb);

    } else {
        ntStatus = STATUS_NO_MEMORY;
    }

    if (configurationDescriptor) {

        //
        // We have the configuration descriptor for the configuration
        // we want.
        //
        // Now we issue the select configuration command to get
        // the  pipes associated with this configuration.
        //

        ntStatus = I82930_SelectInterface(DeviceObject,
            configurationDescriptor, NULL);

        ExFreePool(configurationDescriptor);

    }

    if (NT_SUCCESS(ntStatus)) {
        ntStatus = I82930_BuildPipeList(DeviceObject);
    }

    I82930_KdPrint (("exit I82930_ConfigureDevice (%x)\n", ntStatus));

    return ntStatus;
}


NTSTATUS
I82930_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    PUSBD_INTERFACE_INFORMATION Interface
    )
/*++

Routine Description:

    Initializes an 82930 with multiple interfaces

Arguments:

    DeviceObject - pointer to the device object for this instance of the 82930
                    devcice.

    ConfigurationDescriptor - pointer to the USB configuration
                    descriptor containing the interface and endpoint
                    descriptors.

Return Value:

    NT status code

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    NTSTATUS ntStatus;
    PURB urb = NULL;
    ULONG i;
    UCHAR alternateSetting, interfaceNumber;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor = NULL;
    USHORT siz;

    I82930_KdPrint (("enter I82930_SelectInterface\n"));

    deviceExtension = DeviceObject->DeviceExtension;

    if (Interface == NULL) {

        //
        // I82930 driver only supports one interface, we must search
        // the configuration descriptor for the interface we want
        // and remember the pipes.
        //

        urb = USBD_CreateConfigurationRequest(ConfigurationDescriptor, &siz);

        if (urb) {

            //
            // search thru all the interfaces
            // and find any we are interested in
            //

            interfaceNumber = 0;
            alternateSetting = 0;

            interfaceDescriptor =
                USBD_ParseConfigurationDescriptor(ConfigurationDescriptor,
                                                  interfaceNumber,
                                                  alternateSetting);

            Interface = &urb->UrbSelectConfiguration.Interface;

        //    for (i=0; i< Interface->NumberOfPipes; i++) {
                //
                // perform any pipe initialization here
                //
                //interface->Pipes[i].MaximumTransferSize = 0;
       //     }

            UsbBuildSelectConfigurationRequest(urb,
                                              (USHORT) siz,
                                              ConfigurationDescriptor);

            ntStatus = I82930_CallUSBD(DeviceObject, urb);

            deviceExtension->ConfigurationHandle =
                urb->UrbSelectConfiguration.ConfigurationHandle;

        } else {
            ntStatus = STATUS_NO_MEMORY;
        }

    } else {
        //
        // we were give an interface already set up
        //
        // do a selectinterface here if we want to change any of the
        // pipe parameters.
        //

        TRAP();

    }

    ASSERT(Interface != NULL);

    if (NT_SUCCESS(ntStatus)) {

        //
        // Save the configuration handle for this device
        //

        deviceExtension->ConfigurationHandle =
            urb->UrbSelectConfiguration.ConfigurationHandle;

        deviceExtension->Interface = ExAllocatePool(NonPagedPool,
                                                    Interface->Length);

        if (deviceExtension->Interface) {
            ULONG j;

            //
            // save a copy of the interface information returned
            //
            RtlCopyMemory(deviceExtension->Interface, Interface, Interface->Length);

            //
            // Dump the interface to the debugger
            //
            I82930_KdPrint (("---------\n"));
            I82930_KdPrint (("NumberOfPipes 0x%x\n", deviceExtension->Interface->NumberOfPipes));
            I82930_KdPrint (("Length 0x%x\n", deviceExtension->Interface->Length));
            I82930_KdPrint (("Alt Setting 0x%x\n", deviceExtension->Interface->AlternateSetting));
            I82930_KdPrint (("Interface Number 0x%x\n", deviceExtension->Interface->InterfaceNumber));
            I82930_KdPrint (("Class, subclass, protocol 0x%x 0x%x 0x%x\n",
                deviceExtension->Interface->Class,
                deviceExtension->Interface->SubClass,
                deviceExtension->Interface->Protocol));

            // Dump the pipe info

            for (j=0; j<Interface->NumberOfPipes; j++) {
                PUSBD_PIPE_INFORMATION pipeInformation;

                pipeInformation = &deviceExtension->Interface->Pipes[j];

                I82930_KdPrint (("---------\n"));
                I82930_KdPrint (("PipeType 0x%x\n", pipeInformation->PipeType));
                I82930_KdPrint (("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
                I82930_KdPrint (("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
                I82930_KdPrint (("Interval 0x%x\n", pipeInformation->Interval));
                I82930_KdPrint (("Handle 0x%x\n", pipeInformation->PipeHandle));
                I82930_KdPrint (("MaximumTransferSize 0x%x\n", pipeInformation->MaximumTransferSize));
            }

            I82930_KdPrint (("---------\n"));
        }
    }

    if (urb) {
        ExFreePool(urb);
    }
    I82930_KdPrint (("exit I82930_SelectInterface (%x)\n", ntStatus));

    return ntStatus;
}


VOID
I82930_Cleanup(
    PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

Arguments:

Return Value:

    None.

--*/
{
    PDEVICE_EXTENSION deviceExtension;
    UNICODE_STRING deviceLinkUnicodeString;

    deviceExtension = DeviceObject->DeviceExtension;

    if (deviceExtension->NeedCleanup) {

        deviceExtension->NeedCleanup = FALSE;

        RtlInitUnicodeString (&deviceLinkUnicodeString,
                              deviceExtension->DeviceLinkNameBuffer);

        IoDeleteSymbolicLink(&deviceLinkUnicodeString);
    }
}


NTSTATUS
I82930_BuildPipeList(
    IN  PDEVICE_OBJECT DeviceObject
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
    PDEVICE_EXTENSION deviceExtension;
    ULONG i;
    WCHAR Name[] = L"\\PIPE00";
    PUSBD_INTERFACE_INFORMATION interface;

    deviceExtension = DeviceObject->DeviceExtension;
    interface = deviceExtension->Interface;

    I82930_KdPrint (("enter I82930_BuildPipeList\n"));

    deviceExtension = DeviceObject->DeviceExtension;

     for (i=0; i<I82930_MAX_PIPES; i++) {

        Name[6] = 'X';
        RtlCopyMemory(deviceExtension->PipeList[i].Name,
                      Name,
                      sizeof(Name));

    }

    //
    // build a list of pipe names based on the interface
    //

    for (i=0; i<interface->NumberOfPipes; i++) {

        Name[6] = '0' + (USHORT) i;
        RtlCopyMemory(deviceExtension->PipeList[i].Name,
                      Name,
                      sizeof(Name));

        deviceExtension->PipeList[i].PipeInfo =
            &interface->Pipes[i];

        deviceExtension->PipeList[i].Opened = FALSE;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
I82930_ResetPipe(
    IN PDEVICE_OBJECT DeviceObject,
    IN PI82930_PIPE Pipe
    )
/*++

Routine Description:

    Reset both pipes associated with a video channel on the
    camera.

Arguments:

Return Value:


--*/
{
    NTSTATUS ntStatus;
        PURB urb;

    I82930_KdPrint (("Reset Pipe %x\n", Pipe));

        urb = ExAllocatePool(NonPagedPool,
                                                 sizeof(struct _URB_PIPE_REQUEST));

        if (urb) {

        urb->UrbHeader.Length = (USHORT) sizeof (struct _URB_PIPE_REQUEST);
        urb->UrbHeader.Function = URB_FUNCTION_RESET_PIPE;
        urb->UrbPipeRequest.PipeHandle =
            Pipe->PipeInfo->PipeHandle;

        ntStatus = I82930_CallUSBD(DeviceObject, urb);
        } else {
                ntStatus = STATUS_NO_MEMORY;
    }

    return ntStatus;
}

