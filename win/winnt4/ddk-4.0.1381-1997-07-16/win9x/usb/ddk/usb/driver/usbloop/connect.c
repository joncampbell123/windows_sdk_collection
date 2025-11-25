/**************************************************************************

Copyright (c) 1995  Microsoft Corporation

Module Name:

		connect.c

Abstract:

		USB device driver for UTB

Environment:

		kernel mode only

Notes:

  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
  PURPOSE.

  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.


Revision History:

		1-10-96 : created
		8-15-96 : Adding support for multiple pipes with ReadFile, WriteFile
						  interface.
**************************************************************************/


#define DRIVER

#include <usbloop.h>

NTSTATUS
USBLOOP_CreateDeviceObject(
		IN PDRIVER_OBJECT DriverObject,
		IN PDEVICE_OBJECT *DeviceObject,
		ULONG Instance
		)
/**************************************************************************

Routine Description:

		Creates a deviceObject

Arguments:

		DriverObject - pointer to the driver object for USBLOOP
		DeviceObject - the physical device object representing the bus

Return Value:

		STATUS_SUCCESS if successful,
		STATUS_UNSUCCESSFUL otherwise

**************************************************************************/
{
	NTSTATUS ntStatus;
	UNICODE_STRING deviceLinkUnicodeString;
	UNICODE_STRING deviceNameUnicodeString;
	ANSI_STRING deviceNameAnsiString;
	PDEVICE_EXTENSION deviceExtension;
	USHORT usDiv1 = 0, usDiv2 = 0, usInst = 0;

	USBLOOP_KdPrint(("enter USBLOOP_CreateDeviceObject (%d)\n", Instance));


	// creation of multiple instances of the driver

	usDiv2 = (USHORT) (Instance / 100);
	usDiv1 = (USHORT) ((Instance - (usDiv2 * 100)) / 10);
	usInst = (USHORT) (Instance - ((usDiv2 * 100) + (usDiv1 * 10)));

	deviceLinkBuffer[19] = deviceNameBuffer[15] = (L'0' + usDiv2);
	deviceLinkBuffer[20] = deviceNameBuffer[16] = (L'0' + usDiv1);
	deviceLinkBuffer[21] = deviceNameBuffer[17] = (L'0' + usInst);
	deviceLinkBuffer[22] = deviceNameBuffer[18] = 0;

	RtlInitUnicodeString(&deviceNameUnicodeString,
						 deviceNameBuffer);

	ntStatus = RtlUnicodeStringToAnsiString(&deviceNameAnsiString,
											  &deviceNameUnicodeString,
											  TRUE);

	USBLOOP_KdPrint(("Create Device name (%s)\n", deviceNameAnsiString.Buffer));

	if(NT_SUCCESS(ntStatus))
		RtlFreeAnsiString(&deviceNameAnsiString);

	// create the device object

	ntStatus = IoCreateDevice(DriverObject,
							  sizeof (DEVICE_EXTENSION),
							  &deviceNameUnicodeString,
							  FILE_DEVICE_UNKNOWN,
							  0,
							  FALSE,
							  DeviceObject);




	if(NT_SUCCESS(ntStatus))
	{

		// create symbolic links

		RtlInitUnicodeString(&deviceLinkUnicodeString,
									  deviceLinkBuffer);
		

		ntStatus = RtlUnicodeStringToAnsiString(&deviceNameAnsiString,
												  &deviceLinkUnicodeString,
												  TRUE);

		USBLOOP_KdPrint(("Symbolic Link name (%s)\n", deviceNameAnsiString.Buffer));

		if(NT_SUCCESS(ntStatus))
			RtlFreeAnsiString(&deviceNameAnsiString);


		ntStatus = IoCreateUnprotectedSymbolicLink(&deviceLinkUnicodeString,
										 &deviceNameUnicodeString);

		deviceExtension = (PDEVICE_EXTENSION) ((*DeviceObject)->DeviceExtension);
		deviceExtension->Stopped = TRUE;
		deviceExtension->ConfigurationHandle = NULL;

	}

	USBLOOP_KdPrint(("exit USBLOOP_CreateDeviceObject (%x)\n", ntStatus));

	return ntStatus;
}

NTSTATUS
USBLOOP_PnPAddDevice(
		IN PDRIVER_OBJECT DriverObject,
		IN PDEVICE_OBJECT PhysicalDeviceObject
		)
/**************************************************************************

Routine Description:

		This routine is called to create a new instance of a USB device

Arguments:

		DriverObject - pointer to the driver object for this instance of USBLOOP_

		PhysicalDeviceObject - pointer to a device object created by the bus

Return Value:

		STATUS_SUCCESS if successful,
		STATUS_UNSUCCESSFUL otherwise

**************************************************************************/
{
	NTSTATUS			ntStatus = STATUS_SUCCESS;
	PDEVICE_OBJECT		deviceObject = NULL, deviceObjectParent = NULL;
	PDEVICE_EXTENSION	deviceExtension;

	USBLOOP_KdPrint(("enter USBLOOP_PnPAddDevice with %ld\n", ulNumLogDev));

	// create our funtional device object

	ulNumLogDev++;
		
	ntStatus = USBLOOP_CreateDeviceObject(DriverObject, &deviceObject, ulNumLogDev);

	if((deviceObject != NULL)  && (PhysicalDeviceObject != NULL))
	{
		deviceExtension = deviceObject->DeviceExtension;

	    // we support direct io for read/write
		deviceObject->Flags |= DO_DIRECT_IO;
		deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

		deviceExtension->PhysicalDeviceObject = PhysicalDeviceObject;
		deviceExtension->ulInstance = ulNumLogDev;


	    // Attach to the PDO
		deviceExtension->StackDeviceObject = 
			IoAttachDeviceToDeviceStack(deviceObject, PhysicalDeviceObject);
	}
	
	USBLOOP_KdPrint(("exit USBLOOP_PnPAddDevice (%x)\n", ntStatus));

	return ntStatus;
}


