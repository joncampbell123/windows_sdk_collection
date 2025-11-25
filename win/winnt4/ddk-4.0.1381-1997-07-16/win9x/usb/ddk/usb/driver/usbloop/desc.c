/****************************************************************************

Copyright (c) 1995  Microsoft Corporation

Module Name:

    desc.c

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

****************************************************************************/


#define DRIVER
 
#include "usbloop.h"


NTSTATUS
USBLOOP_GetDescriptor(
    IN  PDEVICE_OBJECT DeviceObject,
    IN  UCHAR DescType,
    OUT  PVOID pvBuffer
    )
/**************************************************************************

Routine Description:

	Returns the desired descriptor in the buffer passed.
	
Arguments:

    DeviceObject - pointer to the device object for this instance of a UTB

Return Value:

	NT status code

**************************************************************************/
{
    NTSTATUS ntStatus;
	PURB urb;
	ULONG descSiz;
    PUCHAR pchBuff;
	
	USBLOOP_KdPrint (("enter USBLOOP_GetDescriptor for %ld\n", DescType));	

    pchBuff = pvBuffer;

	switch(DescType)
	{
		case USB_DEVICE_DESCRIPTOR_TYPE:
			descSiz = sizeof(USB_DEVICE_DESCRIPTOR);
			break;

            // BUGBUG we should change from adding
            // 256 to this value

		case USB_CONFIGURATION_DESCRIPTOR_TYPE:
			descSiz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;;
			break;

        case USB_INTERFACE_DESCRIPTOR_TYPE:
	        USBLOOP_KdPrint(("\n Interface Descriptor not currently implemented\n"));
	        USBLOOP_TRAP();
			break;

        case USB_ENDPOINT_DESCRIPTOR_TYPE:
            USBLOOP_KdPrint(("\n Endpoint Descriptor not currently implemented\n"));
            USBLOOP_TRAP();
			break;

		default:
            USBLOOP_KdPrint(("\n Invalid parameter.\n"));
            USBLOOP_TRAP();
			return STATUS_INVALID_PARAMETER;
			break;
	}
		
	if((urb = ExAllocatePool(NonPagedPool, 
                    sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST))) && (pchBuff))
	{
		UsbBuildGetDescriptorRequest(urb, 
					(USHORT) sizeof (struct _URB_CONTROL_DESCRIPTOR_REQUEST),
					DescType,
					0,
					0,
                    pchBuff,
					NULL,
					descSiz,
					NULL);

		ntStatus = USBLOOP_CallUSBD(DeviceObject, urb);
		
		if(NT_SUCCESS(ntStatus))
		{
            ASSERT(pchBuff != NULL);

			USBLOOP_KdPrint (("Descriptor = %x, type %x len %x\n", 
                                                pchBuff, DescType, 
						urb->UrbControlDescriptorRequest.TransferBufferLength));

            RtlCopyMemory(pvBuffer,
                        pchBuff,
                        descSiz);
		}

		ExFreePool(urb);							
	}					
    else
        ntStatus = STATUS_NO_MEMORY;

	USBLOOP_KdPrint (("Leaving USBLOOP_GetDescriptor (%x)\n", ntStatus));
	return ntStatus;
}


				



