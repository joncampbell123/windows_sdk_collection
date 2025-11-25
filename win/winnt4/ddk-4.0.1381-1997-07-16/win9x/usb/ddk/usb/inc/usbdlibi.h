/*++

Copyright (c) 1995	Microsoft Corporation

Module Name:

	USBDLIBI.H

Abstract:

   Services exported by USBD for use by USB port drivers and
   the usb hub driver.

Environment:

    Kernel & user mode

Revision History:

    01-27-96 : created

--*/

#ifndef   __USBDLIBI_H__
#define   __USBDLIBI_H__

typedef PVOID PUSBD_DEVICE_DATA;

//
// Services exported by USBD
//

DECLSPEC_IMPORT
VOID 
USBD_RegisterHostController(
	IN PDEVICE_OBJECT PhysicalDeviceObject, 
	IN PDEVICE_OBJECT HcdDeviceObject,
	IN PDRIVER_OBJECT HcdDriverObject,
	IN ULONG HcdDeviceNameHandle
	); 

DECLSPEC_IMPORT
BOOLEAN
USBD_Dispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp,
    PDEVICE_OBJECT *HcdDeviceObject,
    NTSTATUS *NtStatus
    );

DECLSPEC_IMPORT
VOID
USBD_CompleteRequest(
    PIRP Irp,
    CCHAR PriorityBoost
    );

DECLSPEC_IMPORT
NTSTATUS
USBD_CreateDevice(
	IN OUT PUSBD_DEVICE_DATA *DeviceData,
	IN PDEVICE_OBJECT DeviceObject,
	IN BOOLEAN DeviceIsLowSpeed,
	IN ULONG MaxPacketSize_Endpoint0,
	IN OUT PBOOLEAN NeedReset
	);

DECLSPEC_IMPORT
NTSTATUS
USBD_InitializeDevice(
	IN PUSBD_DEVICE_DATA DeviceData,
	IN PDEVICE_OBJECT DeviceObject,
	IN OUT PUSB_DEVICE_DESCRIPTOR DeviceDescriptor,
	IN ULONG DeviceDescriptorLength
	);	

DECLSPEC_IMPORT
NTSTATUS
USBD_RemoveDevice(
	IN PUSBD_DEVICE_DATA DeviceData,
	IN PDEVICE_OBJECT DeviceObject
	);	

DECLSPEC_IMPORT
ULONG
USBD_AllocateDeviceName(
    PUNICODE_STRING DeviceNameUnicodeString
    );	

DECLSPEC_IMPORT
VOID
USBD_FreeDeviceName(
    ULONG DeviceNameHandle
    );    

#endif /* __USBDLIBI_H__ */
