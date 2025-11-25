/***************************************************************************

Copyright (c) 1995  Microsoft Corporation

Module Name:

	usbloop.c

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
	8-13-96 : started support for ReadFile, WriteFile interface
	8-15-96 : started support for opening, reading and writing individual
			  pipes
	9-25-96 : First cut of async support finished

	The USBLOOP driver now supports a ReadFile WriteFile interface to the
	pipes on the test board. The DeviceIOControl interface is still
	supported, so you can use it for configuration of the test board.
	Now however, you should use ReadFile and WriteFile calls to read and
	write data for pipes. To open the device for ioctl, open as before
	"USBLOOPXXX". To open an individual pipe, open the device with
	the following tacked on:

	\configuration\interface\alt-interface\pipenum

	Here is what one might look like:

	\USBLOOPXXX\0\0\0\1

	Each part of the device number is a decimal integer representing
	the configuration, interface, alt-interface, and pipe number.

	You cannot use ReadFile and WriteFile calls on the device itself
	("USBLOOPXXX"), it is only used for ioctls.


	The ReadFile WriteFile interface offers a few advantages:

	1. Makes USB pipe on test board look more like a "real" device.
	   This allows you to use statndard ReadFile and WriteFile calls
	   without stuffing a data structure and doing an ioctl
	2. Allows reading and writing pipe in separate operations


	There is an extreme kludge in the interface stuff for now. This
	needs to be reshot a little support multiple interfaces.

****************************************************************************/


#define DRIVER
 
#include "usbloop.h"

// #include "iso.h"

ULONG ulNumLogDev = 0;

NTSTATUS
DriverEntry(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
	)
/****************************************************************************

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

****************************************************************************/
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PDEVICE_OBJECT deviceObject = NULL;

	USBLOOP_KdPrint(("entering (USBLOOP) DriverEntry\n"));
  
		// Create dispatch points for device control, create, close.

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = 
	DriverObject->MajorFunction[IRP_MJ_CREATE]		   =
	DriverObject->MajorFunction[IRP_MJ_CLOSE]		   =
	DriverObject->MajorFunction[IRP_MJ_PNP_POWER]	   = USBLOOP_Dispatch;
	DriverObject->MajorFunction[IRP_MJ_WRITE]		   = USBLOOP_Write;
	DriverObject->MajorFunction[IRP_MJ_READ]		   = USBLOOP_Read;
	DriverObject->DriverUnload						   = USBLOOP_Unload;
	DriverObject->DriverExtension->AddDevice = USBLOOP_PnPAddDevice;
	
	USBLOOP_KdPrint(("exiting (USBLOOP) DriverEntry (%x)\n", ntStatus));

	return ntStatus;
}


NTSTATUS
USBLOOP_Dispatch(
	IN PDEVICE_OBJECT DeviceObject,
	IN PIRP		   Irp
	)
/**************************************************************************

Routine Description:

	Process the IRPs sent to this device.

Arguments:

	DeviceObject - pointer to a device object
	Irp		  - pointer to an I/O Request Packet

Return Value:

	STATUS_SUCCESS if successful,
	STATUS_UNSUCCESSFUL otherwise
	
**************************************************************************/
{

	PIO_STACK_LOCATION irpStack, nextStack;
	PDEVICE_EXTENSION deviceExtension;
	PVOID ioBuffer;
	ULONG inputBufferLength;
	ULONG outputBufferLength;
	ULONG ioControlCode;
	NTSTATUS ntStatus;
   	UNICODE_STRING deviceLinkUnicodeString;
	ULONG Instance, ulSiz, i;
	USHORT usDiv1, usDiv2, usInst;
	USBD_VERSION_INFORMATION sVersionInfo;
	PUCHAR pchBuff;
	PUSBD_INTERFACE_INFORMATION pusbInterfaceInfo, pusbDevExtInterface;
	
	USBLOOP_KdPrint(("Entering USBLOOP_Dispatch\n"));
	
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

		// Get a pointer to the current location in the Irp. This is where
		//	 the function codes and parameters are located.
	
	irpStack = IoGetCurrentIrpStackLocation(Irp);

		// Get a pointer to the device extension

	deviceExtension = DeviceObject->DeviceExtension;

		// save the instance data for PnP removal
		
	Instance = deviceExtension->ulInstance;

		// Get the pointer to the input/output buffer and it's length

	ioBuffer		   = Irp->AssociatedIrp.SystemBuffer;
	inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
	outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

	switch(irpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:

		USBLOOP_KdPrint(("IRP_MJ_CREATE\n"));

		// see if we are opening a pipe for this device, or the device itself

		// see if there is a pipe name attached

		// must be trying to open a pipe
		if(irpStack->FileObject->FileName.Length)
		{
			ULONG ulPipeNum;
			ULONG pipeAttr;
			PUSBD_INTERFACE_INFORMATION pInterface;

			USBLOOP_KdPrint(("IRP_MJ_CREATE opening pipe\n"));

			// get the attributes for this pipe, config, interface, pipe number
			ntStatus = USBLOOP_getPipeAttr(irpStack->FileObject->FileName.Buffer,
										   &pipeAttr);

			// check and see if we got attributes O.K.
			if(!NT_SUCCESS(ntStatus))
			{
				USBLOOP_KdPrint(("Attribute problem in create\n"));
				Irp->IoStatus.Status = ntStatus;
				break;
			}

			// extract pipe number from pipe attributes
			ulPipeNum = PIPENUM(pipeAttr);

			// find the interface for this pipe
			pInterface = (PUSBD_INTERFACE_INFORMATION)
				deviceExtension->Interface[INTERFACE(pipeAttr)];

			// this is a kludge for now and really only works on
			// the first interface. We will have to fix later.
			// See if this is a valid pipe
			if(ulPipeNum >= pInterface->NumberOfPipes)
			{
				USBLOOP_KdPrint(("Not a valid pipe\n"));
				// trying to open pipe that does not exist, error
				Irp->IoStatus.Status = STATUS_NO_SUCH_DEVICE;
				break;
			}

			// see if this pipe is in use, if it is, this is an error
			// DEBUG - we will need to do other checks here,
			// like interface, config, valid pipe, etc.
			if(deviceExtension->pipes[ulPipeNum].bPipeInUse == TRUE)
			{
				USBLOOP_KdPrint(("Pipe in use in create\n"));
				// trying to open pipe that is already open, error
				Irp->IoStatus.Status = STATUS_DEVICE_BUSY;
				break;
			}

			// set FsContext to attributes of pipe, config, interface
			irpStack->FileObject->FsContext = (PVOID) pipeAttr;
			
			// now fill in our pipe descr
			deviceExtension->pipes[ulPipeNum].bPipeInUse = TRUE;
			// this will get set to same thing as FsContext
			deviceExtension->pipes[ulPipeNum].PipeAttr = pipeAttr;

			// now increment the number of pipes in use for this device
			deviceExtension->numPipesInUse++;
		}
		// they must be opening the device
		else
		{
			USBLOOP_KdPrint(("IRP_MJ_CREATE opening device\n"));
			// set FsContext to -1 for device, can't read or write from device
			irpStack->FileObject->FsContext = (PVOID) -1;
		}
		break;

	case IRP_MJ_CLOSE:
		// see if they are closing a pipe
		if((int)irpStack->FileObject->FsContext != -1)
		{
			ULONG ulPipeNum;

			// extract pipe number
			ulPipeNum = PIPENUM(irpStack->FileObject->FsContext);

			// get the pipe number
			// they are closing a pipe, so set in use flag
			deviceExtension->pipes[ulPipeNum].bPipeInUse = FALSE;

			// now decrement the number of pipes in use for this device
			deviceExtension->numPipesInUse--;
		}
			
			
		USBLOOP_KdPrint(("IRP_MJ_CLOSE\n"));
		break;

	case IRP_MJ_DEVICE_CONTROL:
		
	   	USBLOOP_KdPrint(("IRP_MJ_DEVICE_CONTROL\n"));

		ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

		switch(ioControlCode)
		{

			case GET_VERSION:

				USBLOOP_KdPrint(("GET_VERSION\n"));

				// BUG BUG - not sure why, but this call went away from USBD
				// library. Should fix later.
#if 0
				USBD_GetUSBDIVersion(&sVersionInfo);
#endif
				USBLOOP_KdPrint(("USBD Version: 0x%x\n", 
							sVersionInfo.USBDI_Version));
						
				USBLOOP_KdPrint(("Support USB Spec Version: 0x%x\n", 
							sVersionInfo.Supported_USB_Version));

				pchBuff = (PUCHAR) ioBuffer;
				ulSiz = sizeof(sVersionInfo);
				
				RtlCopyMemory(pchBuff,
								(PUCHAR) &sVersionInfo,
								ulSiz);
				
				Irp->IoStatus.Information = ulSiz;
				break;

			case GET_DEVICE_DESCRIPTOR:

				USBLOOP_KdPrint(("GET_DEVICE_DESCRIPTOR\n"));

				USBLOOP_GetDescriptor(DeviceObject,
									  USB_DEVICE_DESCRIPTOR_TYPE, 
									ioBuffer);
				
				Irp->IoStatus.Information = sizeof(USB_DEVICE_DESCRIPTOR);
				break;

			case GET_CONFIG_DESCRIPTOR:

				USBLOOP_KdPrint(("GET_CONFIGURATION_DESCRIPTOR\n"));

				USBLOOP_GetDescriptor(DeviceObject, 
								USB_CONFIGURATION_DESCRIPTOR_TYPE, 
								ioBuffer);

		            // BUGBUG we should change from adding
		            // 256 to this value

				Irp->IoStatus.Information = sizeof(USB_CONFIGURATION_DESCRIPTOR) +256;
				break;

			case GET_NUM_DEVICES:

					// simply return the number of log devices to the ring3
					// app so that he can search for devices
					
				USBLOOP_KdPrint(("GET_NUMBER_DEVICES\n"));

                pchBuff = (PUCHAR) ioBuffer;
                ulSiz = sizeof(ulNumLogDev);

                RtlCopyMemory(pchBuff,
                                (PUCHAR) ulNumLogDev,
                                ulSiz);

                Irp->IoStatus.Information = ulSiz;                 
				break;
								
			case GET_DEVICE_INFO:

					// copy the device extension info into a a ring3 
					// buffer so that the ring3 app has all the data 
					// that he needs
					
				USBLOOP_KdPrint(("GET_DEVICE_INFO\n"));

                                        // BUGBUG this is broken

                USBLOOP_TRAP();

                pchBuff = (PUCHAR) ioBuffer;
                ulSiz = sizeof(DEVICE_EXTENSION);

                RtlCopyMemory(pchBuff,
                                (PUCHAR) deviceExtension,
                                ulSiz);

                Irp->IoStatus.Information = ulSiz;              
				break;

			case GET_INTERFACE_INFO:

					// Get the interface information so the app
					// can choose the appropriate endpoints
					
				USBLOOP_KdPrint(("GET_INTERFACE_INFO\n"));


				pusbDevExtInterface = deviceExtension->Interface[0];
				
				pusbInterfaceInfo = (PUSBD_INTERFACE_INFORMATION) ioBuffer;
				ulSiz = sizeof(USBD_INTERFACE_INFORMATION) +
					(pusbDevExtInterface->NumberOfPipes * sizeof(USBD_PIPE_INFORMATION));

				RtlCopyMemory(pusbInterfaceInfo,
								deviceExtension->Interface[0],
								sizeof(USBD_INTERFACE_INFORMATION));

				for(i=0; i < pusbDevExtInterface->NumberOfPipes; i++)
				{
					pusbInterfaceInfo->Pipes[i].MaximumPacketSize =
						pusbDevExtInterface->Pipes[i].MaximumPacketSize;

					pusbInterfaceInfo->Pipes[i].EndpointAddress =
						pusbDevExtInterface->Pipes[i].EndpointAddress;

					pusbInterfaceInfo->Pipes[i].Interval =
						pusbDevExtInterface->Pipes[i].Interval;

					pusbInterfaceInfo->Pipes[i].PipeType =
						pusbDevExtInterface->Pipes[i].PipeType;

					pusbInterfaceInfo->Pipes[i].PipeHandle =
						pusbDevExtInterface->Pipes[i].PipeHandle;

					pusbInterfaceInfo->Pipes[i].MaximumTransferSize =
						pusbDevExtInterface->Pipes[i].MaximumTransferSize;
						
					pusbInterfaceInfo->Pipes[i].PipeFlags =
						pusbDevExtInterface->Pipes[i].PipeFlags;
						
				}						
							
				USBLOOP_KdPrint(("iBuffLen = 0x%x   oBuffLen = 0x%x   ulSiz = 0x%x\n", inputBufferLength, outputBufferLength, ulSiz));

				Irp->IoStatus.Information = ulSiz;
				break;

			/* case USBLOOP_START_ISO_TEST:
				USBLOOP_KdPrint(("USBLOOP_START_ISO_TEST\n"));

				//Iso test routine will set the .information field in the irp
				//Iso test routine will not complete the Irp, so this should do it
				ntStatus = USBLOOP_StartIsoTest (DeviceObject, Irp);  
				Irp->IoStatus.Status = ntStatus;
				break;
			*/

			default:			
				Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;			
		}		

		break;
		
	case IRP_MJ_INTERNAL_DEVICE_CONTROL:		

			// pass through to USBD
				
		USBLOOP_KdPrint(("IRP_MJ_INTERNAL_DEVICE_CONTROL\n"));
		
		nextStack = IoGetNextIrpStackLocation(Irp);
		ASSERT(nextStack != NULL);
		RtlCopyMemory(nextStack, irpStack, sizeof(IO_STACK_LOCATION));
		
		IoMarkIrpPending(Irp);

		ntStatus = IoCallDriver(deviceExtension->StackDeviceObject, Irp);   

		USBLOOP_KdPrint(("Passed PnP Irp down, status = %x\n", ntStatus));								

		goto USBLOOP_Dispatch_Done;
		break;
		
	case IRP_MJ_PNP_POWER:
	
		USBLOOP_KdPrint(("IRP_MJ_PNP_POWER\n"));
	
		switch(irpStack->MinorFunction)
		{
			case IRP_MN_START_DEVICE:

				USBLOOP_KdPrint(("IRP_MN_START_DEVICE\n"));
				ntStatus = USBLOOP_StartDevice(DeviceObject);
				break;
			
			case IRP_MN_STOP_DEVICE:

				USBLOOP_KdPrint(("IRP_MN_STOP_DEVICE\n"));
				ntStatus = USBLOOP_StopDevice(DeviceObject);

				break;

			case IRP_MN_REMOVE_DEVICE:

				USBLOOP_KdPrint(("IRP_MN_REMOVE_DEVICE\n"));
				
					// at this point the device is gone			
					// fix up device name s based on Instance
					// value which we saved on entry.

				usDiv2 = (USHORT) (Instance / 100);
				usDiv1 = (USHORT) ((Instance - (usDiv2 * 100)) / 10);
				usInst = (USHORT) (Instance - ((usDiv2 * 100) + (usDiv1 * 10)));

				if(Instance == 0)
					break;
				else 
				{
					deviceLinkBuffer[19] = (USHORT) ('0' + usDiv2);  
					deviceLinkBuffer[20] = (USHORT) ('0' + usDiv1);
					deviceLinkBuffer[21] = (USHORT) ('0' + usInst);
					deviceLinkBuffer[22] = 0;
				}

					// Undo anything we did for ADD_DEVICE
				
				if(!deviceExtension->Stopped) 
					ntStatus = USBLOOP_StopDevice(DeviceObject);
												 
					// Delete the FDO we created
	
				USBLOOP_KdPrint(("removing device object\n"));

					// detach device and delete links and device
					
				IoDetachDevice(deviceExtension->StackDeviceObject);			
				RtlInitUnicodeString(&deviceLinkUnicodeString,
									 deviceLinkBuffer);
				IoDeleteSymbolicLink(&deviceLinkUnicodeString);
				IoDeleteDevice(DeviceObject);

					// need to let the global variable now that we have
					// 1 less deviceobject in the system
//				ulNumLogDev--; 		

				break;

			default:
				USBLOOP_KdPrint(("PNP IOCTL not handled \n"));
				
		} /* case */

			// All PNP POWER messages get passed to StackDeviceObject.

		nextStack = IoGetNextIrpStackLocation(Irp);
		ASSERT(nextStack != NULL);
		RtlCopyMemory(nextStack, irpStack, sizeof(IO_STACK_LOCATION));
		
		IoMarkIrpPending(Irp);

		ntStatus = IoCallDriver(deviceExtension->StackDeviceObject, Irp);   

		USBLOOP_KdPrint(("Passed PnP Irp down, status = %x\n", ntStatus));								

		goto USBLOOP_Dispatch_Done;
		break;
				
	default:

		Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;	
		
	}

	ntStatus = Irp->IoStatus.Status;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

USBLOOP_Dispatch_Done:

	USBLOOP_KdPrint(("Leaving USBLOOP_Dispatch (%x)\n", ntStatus));
	
	return ntStatus;
}


VOID
USBLOOP_Unload(
	IN PDRIVER_OBJECT DriverObject
	)
/**************************************************************************

Routine Description:

	Free all the allocated resources, etc.

Arguments:

	DriverObject - pointer to a driver object

**************************************************************************/
{
	USBLOOP_KdPrint(("enter USBLOOP_Unload\n"));

		// Free any global resources

		// note: this is a hack. i ought to be calling JD's enumerate
		// function to get a list of the devices
		

			
	USBLOOP_KdPrint(("exit USBLOOP_Unload\n"));
}


NTSTATUS
USBLOOP_StartDevice(
	IN  PDEVICE_OBJECT DeviceObject
	)
/**************************************************************************

Routine Description:

	Initializes a given instance of the UTB device on the USB.
	
	Also gets all the data needed for the device extension. In this case
	that includes getting all the descriptors for the device.

Arguments:

	DeviceObject - pointer to the device object for this instance of a UTB

Return Value:

	NT status code

**************************************************************************/
{
	PDEVICE_EXTENSION deviceExtension;
	NTSTATUS ntStatus;
	PURB urb;
	ULONG siz;
	PUSB_DEVICE_DESCRIPTOR deviceDesc;
	
	USBLOOP_KdPrint(("enter USBLOOP_StartDevice\n"));	

	deviceExtension = DeviceObject->DeviceExtension;

	
		// First, get the device descriptor
	
	siz = sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST);
	urb = ExAllocatePool(NonPagedPool, 
						 sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

	if(urb) 
	{

		siz = sizeof(USB_DEVICE_DESCRIPTOR);
		deviceDesc = ExAllocatePool(NonPagedPool, siz);

		if(deviceDesc)
		{

			UsbBuildGetDescriptorRequest(urb, 
						(USHORT) sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
						USB_DEVICE_DESCRIPTOR_TYPE,
						0,
						0,
						deviceDesc,
						NULL,
						siz,
						NULL);

			ntStatus = USBLOOP_CallUSBD(DeviceObject, urb);
			
			if(NT_SUCCESS(ntStatus))
			{
				USBLOOP_KdPrint(("Dev Descriptor = %x, type %x len %x\n", 
							deviceDesc, 
							USB_DEVICE_DESCRIPTOR_TYPE,
							urb->UrbControlDescriptorRequest.TransferBufferLength));
				// USBLOOP_TRAP();

				deviceExtension->pDeviceDescriptor = ExAllocatePool(NonPagedPool, 
						 deviceDesc->bLength);
						 
					// copy the device descriptor to the device extension

				RtlCopyMemory(deviceExtension->pDeviceDescriptor, deviceDesc, 
					sizeof(USB_DEVICE_DESCRIPTOR));
				
 				deviceExtension->Stopped = FALSE;	 				
			}
			else
				ntStatus = STATUS_NO_MEMORY;
			
			ExFreePool(deviceDesc);											
		} 
		else
			ntStatus = STATUS_NO_MEMORY;

		ExFreePool(urb);							
	}					

	if(NT_SUCCESS(ntStatus))
		ntStatus = USBLOOP_ConfigureDevice(DeviceObject);

	USBLOOP_KdPrint(("Leaving USBLOOP_StartDevice (%x)\n", ntStatus));

	return ntStatus;
}


NTSTATUS
USBLOOP_ConfigureDevice(
	IN  PDEVICE_OBJECT DeviceObject
	)
/**************************************************************************

Routine Description:

	Initializes a given instance of the device on the USB.

Arguments:

	DeviceObject - pointer to the device object for this instance of the 82930
					devcice.
					

Return Value:

	NT status code

**************************************************************************/
{
	PDEVICE_EXTENSION deviceExtension;
	NTSTATUS ntStatus;
	PURB urb;
	ULONG siz;
	PUSB_CONFIGURATION_DESCRIPTOR configDesc = NULL;

	USBLOOP_KdPrint(("enter I82930_ConfigureDevice\n"));	

	deviceExtension = DeviceObject->DeviceExtension;


		// Next get the configuration Descriptor

	urb = ExAllocatePool(NonPagedPool, 
						 sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));

		// BUGBUG need to find out how many configs this device
		// has and fix up a buffer accordingly
		
	if(urb) 
	{

			// BUGBUG currently in 82930, it chokes if we do not ask
			// with a large enough buffer
			
		siz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;
		
get_config_descriptor_retry:

		deviceExtension->pUsbConfigDesc = ExAllocatePool(NonPagedPool, siz);
		configDesc = ExAllocatePool(NonPagedPool, siz);

		if(configDesc)
		{

			UsbBuildGetDescriptorRequest(urb, 
						(USHORT) sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST),
						USB_CONFIGURATION_DESCRIPTOR_TYPE,
						0,
						0,
						configDesc,
						NULL,
						siz,
						NULL);
						
			ntStatus = USBLOOP_CallUSBD(DeviceObject, urb);
			
			USBLOOP_KdPrint(("Config Descriptor = %x, type %x len %x\n", configDesc, 
						USB_CONFIGURATION_DESCRIPTOR_TYPE,
						urb->UrbControlDescriptorRequest.TransferBufferLength));

			// USBLOOP_TRAP();
			
				// copy the config descriptor to the device extension

				// BUGBUG this needs to change! Need to make it 
				// a ptr and not a array

				// note using siz below instead of structure size because
				// the real config descriptor can be greater than the
				// structure size
				
			RtlCopyMemory(deviceExtension->pUsbConfigDesc, configDesc, siz);
			
		}
		// allocating config descriptor failed
		else
		{
			ExFreePool(urb);			
			return STATUS_NO_MEMORY;
		}
			
			// if we got some data see if it was enough.
			// NOTE: we may get an error in URB because of buffer overrun
		
		if(urb->UrbControlDescriptorRequest.TransferBufferLength>0 &&
				configDesc->wTotalLength > siz) 
		{

		   	USBLOOP_KdPrint(("Config TotalLen = %x siz = %x\n", 
				configDesc->wTotalLength, siz));
			siz = configDesc->wTotalLength;
			ExFreePool(deviceExtension->pUsbConfigDesc);
			ExFreePool(configDesc);
			configDesc = NULL;
			deviceExtension->pUsbConfigDesc = NULL;
			goto get_config_descriptor_retry;
		}			

		ExFreePool(urb);			
	}
	// failed to allocate urb
	else
	{
		return STATUS_NO_MEMORY;
	}

	// get the pipes

	ntStatus = USBLOOP_SelectInterfaces(DeviceObject, configDesc);

	ExFreePool(configDesc);

	USBLOOP_KdPrint(("exit USBLOOP_ConfigureDevice (%x)\n", ntStatus));
	return ntStatus;
}



NTSTATUS
USBLOOP_SelectInterfaces(
	IN PDEVICE_OBJECT					DeviceObject,
	IN PUSB_CONFIGURATION_DESCRIPTOR	configDesc
	)
/**************************************************************************

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

**************************************************************************/
{
	PDEVICE_EXTENSION			deviceExtension;
	NTSTATUS					ntStatus;
	PURB						urb;
	ULONG						x, lTotalInterfaces, j, i, TempLen;
	USHORT						siz;
	UCHAR						ucNumPipes = 0, ucInterfaceNum = 0, ucAltSetting = 0;
	PUCHAR						pch;
	PUSB_INTERFACE_DESCRIPTOR	interfaceDescriptor[MAX_INTERFACE];
	PUSBD_INTERFACE_INFORMATION	interface[MAX_INTERFACE];

	USBLOOP_KdPrint(("enter USBLOOP_SelectInterfaces\n"));	

	deviceExtension = DeviceObject->DeviceExtension;

		// get number of interfaces from the config desc
		
	lTotalInterfaces = configDesc->bNumInterfaces;
	USBLOOP_KdPrint(("number of interfaces %x\n", lTotalInterfaces));
	
		// loop through each interface, storing the descriptor
		// into the local buffer

	urb = USBD_CreateConfigurationRequest(configDesc, &siz);

	if(urb)
	{
		for(x = 0; x < lTotalInterfaces; x++)
		{
			ucAltSetting = 0;
			interfaceDescriptor[x] = USBD_ParseConfigurationDescriptor(
												configDesc, 
												ucInterfaceNum, 
												ucAltSetting);
											
			interface[x] = &urb->UrbSelectConfiguration.Interface;
			for (i=0; i< interface[x]->NumberOfPipes; i++) 
				interface[x]->Pipes[i].MaximumTransferSize = USBD_DEFAULT_MAXIMUM_TRANSFER_SIZE;

				// keep track of total number of pipes so that we can 
				// allocate the correct amount of buffer space
			
			ucNumPipes += interfaceDescriptor[x]->bNumEndpoints;	
			ucInterfaceNum++;
		
			USBLOOP_KdPrint(("NumberOfPipes %x for interfaces %x\n", 
				ucNumPipes, ucInterfaceNum));
		}
	}
	else
		return STATUS_NO_MEMORY;


		// select the appropriate interface 
		// and alterntate setting

	pch = (PUCHAR) &urb->UrbSelectConfiguration.Interface;

	for(i=0; i<lTotalInterfaces; i++) 
	{
		interface[i] = (PUSBD_INTERFACE_INFORMATION) pch;

			// setup the input parameters in our interface request
			// structure.

			// note USBD files in the number of pipes
				
		interface[i]->Length = GET_USBD_INTERFACE_SIZE(interfaceDescriptor[i]->bNumEndpoints);
		interface[i]->InterfaceNumber = interfaceDescriptor[i]->bInterfaceNumber;
		interface[i]->AlternateSetting = interfaceDescriptor[i]->bAlternateSetting;  

		USBLOOP_KdPrint(("size of interface request = %x\n", interface[i]->Length));

			// point to next interface
			
			// note that the length value here includes the length
			// of the endpt descriptors
			
		pch += interface[i]->Length;	
	}			

	UsbBuildSelectConfigurationRequest(urb, (USHORT) siz, configDesc);

	// USBLOOP_TRAP();
	USBLOOP_KdPrint(("size of interface request = %x\n", interface[0]->Length));
									 
	ntStatus = USBLOOP_CallUSBD(DeviceObject, urb);

	USBLOOP_KdPrint(("size of interface request = %x\n", interface[0]->Length));
		
	if(NT_SUCCESS(ntStatus) && USBD_SUCCESS(urb->UrbSelectConfiguration.Status))
	{
		deviceExtension->ConfigurationHandle = 
			urb->UrbSelectConfiguration.ConfigurationHandle;

		for(x=0; x < lTotalInterfaces; x++)
		{

			// USBLOOP_TRAP();

				// BUGBUG major hack here, due to bug in USB  stack
			TempLen = interface[x]->Length + 16;
			deviceExtension->Interface[x] = 
				ExAllocatePool(NonPagedPool, TempLen);
					
			if(deviceExtension->Interface[x]) 
			{

					// BUGBUG major hack here, due to bug in USB  stack

					// save a copy of the interface information returned
					// RtlCopyMemory(deviceExtension->Interface[x], 
							//interface[x], interface[x]->Length);
							
				RtlCopyMemory(deviceExtension->Interface[x], 
						interface[x], TempLen);
					
					// Dump the iterface to the debugger
				USBLOOP_KdPrint(("---------\n")); 
				USBLOOP_KdPrint(("NumberOfPipes 0x%x\n", interface[x]->NumberOfPipes));
				USBLOOP_KdPrint(("Length 0x%x\n", interface[x]->Length));  
				USBLOOP_KdPrint(("Alt Setting 0x%x\n", interface[x]->AlternateSetting));  
				USBLOOP_KdPrint(("Interface Number 0x%x\n", interface[x]->InterfaceNumber)); 
				USBLOOP_KdPrint(("---------\n")); 
			
					// Dump the pipe info

				for(j=0; j<interface[x]->NumberOfPipes; j++) 
				{
					PUSBD_PIPE_INFORMATION pipeInformation;

					pipeInformation = &interface[x]->Pipes[j];

//					RtlCopyMemory(&deviceExtension->Interface[x]->Pipes[j],
//									&interface[x]->Pipes[j],
//									sizeof(USBD_PIPE_INFORMATION));
									
					USBLOOP_KdPrint(("---------\n")); 
					USBLOOP_KdPrint(("PipeType 0x%x\n", pipeInformation->PipeType));
					USBLOOP_KdPrint(("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
					USBLOOP_KdPrint(("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
					USBLOOP_KdPrint(("Interval 0x%x\n", pipeInformation->Interval));
					USBLOOP_KdPrint(("Handle 0x%x\n", pipeInformation->PipeHandle));					   
				}

				USBLOOP_KdPrint(("---------\n")); 
					
			}
		}

	}			
		
	ExFreePool(urb);									

	USBLOOP_KdPrint(("exit USBLOOP_SelectInterfaces (%x)\n", ntStatus));

	return ntStatus;
}


NTSTATUS
USBLOOP_StopDevice(
	IN  PDEVICE_OBJECT DeviceObject
	)
/**************************************************************************

Routine Description:

	Stops a given instance of a UTB device on the USB.

Arguments:

	DeviceObject - pointer to the device object for this instance of a UTB 

Return Value:

	NT status code

**************************************************************************/
{
	PDEVICE_EXTENSION	deviceExtension;
	NTSTATUS			ntStatus;
	PURB				urb;
	ULONG				siz;

	USBLOOP_KdPrint(("enter USBLOOP_StopDevice\n"));	

	deviceExtension = DeviceObject->DeviceExtension;

	if(deviceExtension->Stopped) 
		return STATUS_SUCCESS;
	
		// send the select configuration urb with
		// a NULL pointer for the configuration handle
		// this closes the configuration and puts the 
		// device in the 'unconfigured' state.

	siz = sizeof(struct _URB_SELECT_CONFIGURATION);
	
	urb = ExAllocatePool(NonPagedPool, 
						 siz);
						 
	if(urb) 
	{

		UsbBuildSelectConfigurationRequest(urb, (USHORT) siz, NULL);

		ntStatus = USBLOOP_CallUSBD(DeviceObject, urb);

		if(NT_SUCCESS(ntStatus))		 
			USBLOOP_KdPrint(("Device Configuration Closed.\n"));

		ExFreePool(urb);							
	} 
	else
		ntStatus = STATUS_NO_MEMORY;		

	ExFreePool(deviceExtension->pDeviceDescriptor);
	
	USBLOOP_KdPrint(("exit USBLOOP_StopDevice (%x)\n", ntStatus));

	return ntStatus;
}


NTSTATUS
USBLOOP_CallUSBD(
	IN PDEVICE_OBJECT	DeviceObject,
	IN PURB				Urb
	)
/**************************************************************************

Routine Description:

	Passes a URB to the USBD class driver

Arguments:

	DeviceObject	- pointer to the device object for this instance of a UTB

	Urb				- pointer to Urb request block

Return Value:

	STATUS_SUCCESS if successful,
	STATUS_UNSUCCESSFUL otherwise

**************************************************************************/
{
	NTSTATUS			ntStatus, status;
	PDEVICE_EXTENSION	deviceExtension;
	PIRP				irp;
	KEVENT				event;
	IO_STATUS_BLOCK		ioStatus;
	PIO_STACK_LOCATION	nextStack;

	USBLOOP_KdPrint(("enter USBLOOP_CallUSBD\n"));	

	deviceExtension = DeviceObject->DeviceExtension;

		// issue a synchronous request to read the UTB 

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

		// Call the class driver to perform the operation.  If the returned status
		// is PENDING, wait for the request to complete.

	nextStack = IoGetNextIrpStackLocation(irp);
	ASSERT(nextStack != NULL);

		// pass the URB to the USBD 'class driver'
		
	nextStack->Parameters.Others.Argument1 = Urb;

	USBLOOP_KdPrint(("calling USBD\n"));

	ntStatus = IoCallDriver(deviceExtension->StackDeviceObject, irp);

	USBLOOP_KdPrint(("return from IoCallDriver USBD %x\n", ntStatus));

	if(ntStatus == STATUS_PENDING) 
	{
		// Setup timer stuff so we can timeout commands to unresponsive devices
		LARGE_INTEGER dueTime;
		KTIMER	TimeoutTimer;
		KDPC	TimeoutDpc;

		KeInitializeTimer(&TimeoutTimer);  //build the timer object
		
		KeInitializeDpc(&TimeoutDpc,	   					//setup the DPC call based
						USBLOOP_SyncTimeoutDPC,			 //DPC func
						irp);							   //context ptr to pass into DPC func

		dueTime.QuadPart = -10000 * DEADMAN_TIMEOUT;

		KeSetTimer(&TimeoutTimer,		  //Set the timer params up
				   dueTime,								 //This is how long to wait
				   &TimeoutDpc);		   //This is the DPC object we created

		
		USBLOOP_KdPrint (("Waiting for single object\n"));
		status = KeWaitForSingleObject(
	   				   &event,
					   Suspended,
					   KernelMode,
					   FALSE,
					   NULL);	

		USBLOOP_KdPrint (("Wait for single object, returned %x\n", status));

		KeCancelTimer(&TimeoutTimer);

	} 
	else
		ioStatus.Status = ntStatus;		

	USBLOOP_KdPrint(("URB status = %x status = %x irp status %x\n", 
		Urb->UrbHeader.Status, status, ioStatus.Status));

	
		// USBD maps the error code for us
	
	ntStatus = ioStatus.Status;

	USBLOOP_KdPrint(("exit USBLOOP_CallUSBD (%x)\n", ntStatus));

	return ntStatus;
}



VOID
USBLOOP_SyncTimeoutDPC(
	IN PKDPC Dpc,
	IN PVOID DeferredContext,
	IN PVOID SystemArgument1,
	IN PVOID SystemArgument2
	)
/*++
Routine Description:
	Whenever an IRP is passed down the the USB class driver, a 5 second timer
	is setup. If something goes terribly wrong and the timer expires, the IRP
	is cancelled. The Context parameter passes in a pointer to a data
	structure that contains more than you will ever want to know about
	the failed call.

	This routine runs at DISPATCH_LEVEL IRQL. 
Arguments:
	Dpc				- Pointer to the DPC object.
	DeferredContext - passed in to IOS by caller as context (we use AsyncTransfer struct)
	SystemArgument1 - not used.
	SystemArgument2 - not used.
Return Value:
	None.
--*/
{
	BOOLEAN			status;
	pAsyncTransfer	pAsyncXfer = DeferredContext; 
	
	USBLOOP_KdPrint(("enter USBLOOP_SyncTimeoutDPC\n"));

	pAsyncXfer->bTimerExpired = TRUE;

	// BUG BUG - what should we do if this call fails
	status = IoCancelIrp(pAsyncXfer->irp);

	USBLOOP_KdPrint(("exit USBLOOP_SyncTimeoutDPC\n"));

	return;
}

NTSTATUS
USBLOOP_AsyncReadWrite_Complete(
	IN PDEVICE_OBJECT	DeviceObject,
	IN PIRP				Irp,
	IN PVOID			Context
	)
/*++
Routine Description:
	Completion routine for ReadFile and WriteFile calls. Returns length
	of transfer and cancels timer that we set for this call the USB
	driver.
Arguments:
	Context - pointer to AsyncTransfer struct, with urb and timer to cancel
Return Value:
	NTSTATUS.
--*/
{
	NTSTATUS		ntStatus = STATUS_SUCCESS;
	pAsyncTransfer	pAsyncXfer = Context;
	PURB			urb;

	USBLOOP_KdPrint(("enter USBLOOP_AsyncReadWrite_Complete\n"));

	// see if our timer expired
	if(pAsyncXfer->bTimerExpired)
	{
		USBLOOP_KdPrint(("USBLOOP_AsyncReadWrite_Complete: timer expired\n"));
		Irp->IoStatus.Information = 0;
		ntStatus = STATUS_IO_TIMEOUT;
		Irp->IoStatus.Status = ntStatus;
	}
	else
	{
		USBLOOP_KdPrint(("USBLOOP_AsyncReadWrite_Complete: completed O.K.\n"));

		// get a pointer to the urb, so we can extract length
		urb = (PURB) &pAsyncXfer->urb;

		//
		// set the length based on the TransferBufferLength
		// value in the URB
		//
		Irp->IoStatus.Information = 
			urb->UrbBulkOrInterruptTransfer.TransferBufferLength;

		// cancel pending timer
		KeCancelTimer(&pAsyncXfer->TimeoutTimer);
	}

	// free up data structure
	ExFreePool(pAsyncXfer);	
	
	USBLOOP_KdPrint(("exit USBLOOP_AsyncReadWrite_Complete\n"));
	return ntStatus;
}




NTSTATUS
USBLOOP_Read(
	IN PDEVICE_OBJECT	DeviceObject,
	IN PIRP				Irp
	)
/**************************************************************************

Routine Description:

	Process the IRP sent to this device by ReadFile.

Arguments:

	DeviceObject - pointer to a device object
	Irp			 - pointer to an I/O Request Packet

Return Value:

	STATUS_SUCCESS if successful,
	STATUS_UNSUCCESSFUL otherwise
	
**************************************************************************/
{
	PDEVICE_EXTENSION			deviceExtension;
	ULONG						ulPipeNum;
	USBD_PIPE_TYPE				ucPipeType;
	PUSBD_INTERFACE_INFORMATION	pInterface;
	PIO_STACK_LOCATION			irpStack, nextStack;
	NTSTATUS					ntStatus;
	PFILE_OBJECT				fileObject;
	pAsyncTransfer				pAsyncXfer;

	USBLOOP_KdPrint(("enter USBLOOP_Read\n"));

	deviceExtension = DeviceObject->DeviceExtension;
	irpStack = IoGetCurrentIrpStackLocation(Irp);
	fileObject = irpStack->FileObject;

	// if they are calling device, context is set to -1 else, its a pipe
	ulPipeNum = PIPENUM(fileObject->FsContext);

	// if the pipe number is -1, must be using the device, can't read
	if((int) ulPipeNum == -1)
	{
		// there is no pipe associated with just the device
		ntStatus = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Status = ntStatus;	
		// let O.S. know we are done
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		USBLOOP_KdPrint(("exit USBLOOP_Read: trying to read from a device\n"));
		return ntStatus;
	}

	// find the interface for this pipe
	pInterface = (PUSBD_INTERFACE_INFORMATION)
		deviceExtension->Interface[INTERFACE(irpStack->FileObject->FsContext)];

	ucPipeType = pInterface->Pipes[ulPipeNum].PipeType;	
	
	USBLOOP_KdPrint(("Transfer for pipe %d EndpointType = %x Handle = %x\n", 
		ulPipeNum, pInterface->Pipes[ulPipeNum].PipeType,
		pInterface->Pipes[ulPipeNum].PipeHandle));	

	// build our data structure with urb, timer etc. to be passed
	// to completion routines and USB driver
	pAsyncXfer = USBLOOP_BuildAsyncRequest(DeviceObject, 
							   			   Irp, 
										   pInterface->Pipes[ulPipeNum].PipeHandle,
										   TRUE);
	if (pAsyncXfer)
	{
		// mark the IROP pending, IOComplete comes after completion
		// routine called
		IoMarkIrpPending(Irp);

		// save our IRP for completion routines
		// BUG BUG - do we really need to hang onto IRP, could just pass
		// IRP into timer routine, and we get it for read write completion
		// routine
		pAsyncXfer->irp = Irp;
	
		nextStack = IoGetNextIrpStackLocation(Irp);
		ASSERT(nextStack != NULL);
		ASSERT(DeviceObject->StackSize>1);

		nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
		nextStack->Parameters.Others.Argument1 = &pAsyncXfer->urb;
		nextStack->Parameters.DeviceIoControl.IoControlCode = 
			IOCTL_INTERNAL_USB_SUBMIT_URB;

		// setup completion routine for this IRP
		IoSetCompletionRoutine(Irp,
							   USBLOOP_AsyncReadWrite_Complete,
							   pAsyncXfer,
							   TRUE,
							   TRUE,
							   TRUE);
								
		ntStatus = IoCallDriver(deviceExtension->StackDeviceObject, 
								Irp);

		ASSERT(ntStatus == STATUS_PENDING);
		USBLOOP_KdPrint(("exit USBLOOP_Read: Status = PENDING\n"));

		// we must be pending, so completion routine will finish things
		return ntStatus;									
	}
	// couldn't create our data structure
	else
		ntStatus = STATUS_NO_MEMORY;

	// couldn't create data structure, so this one fails
	Irp->IoStatus.Status = ntStatus;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return ntStatus;			

	USBLOOP_KdPrint(("exit USBLOOP_Read: USBLOOP_BuildAsyncRequest call failed.\n"));
	return ntStatus;
}

NTSTATUS
USBLOOP_Write(
	IN PDEVICE_OBJECT	DeviceObject,
	IN PIRP				Irp
	)
/**************************************************************************

Routine Description:

	Process the IRP sent to this device by WriteFile.

Arguments:

	DeviceObject	- pointer to a device object
	Irp				- pointer to an I/O Request Packet

Return Value:

	STATUS_SUCCESS if successful,
	STATUS_UNSUCCESSFUL otherwise
	
**************************************************************************/
{
	PDEVICE_EXTENSION			deviceExtension;
	ULONG						ulPipeNum;
	USBD_PIPE_TYPE				ucPipeType;
	PUSBD_INTERFACE_INFORMATION	pInterface;
	PIO_STACK_LOCATION			irpStack, nextStack;
	NTSTATUS					ntStatus;
	PFILE_OBJECT				fileObject;
	pAsyncTransfer				pAsyncXfer;

	USBLOOP_KdPrint(("enter USBLOOP_Write\n"));

	deviceExtension = DeviceObject->DeviceExtension;
	irpStack = IoGetCurrentIrpStackLocation(Irp);
	fileObject = irpStack->FileObject;

	// if they are calling device, context is set to -1 else, its a pipe
	// extract pipe number, int for now, ULONG later
	ulPipeNum = PIPENUM(fileObject->FsContext);

	// if the pipe number is -1, must be using the device, can't write
	if((int) ulPipeNum == -1)
	{
		// there is no pipe associated with just the device
		ntStatus = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Status = ntStatus;	
		// let O.S. know we are done
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		USBLOOP_KdPrint(("exit USBLOOP_Write: trying to write to a device\n"));
		return ntStatus;
	}

	// find the interface for this pipe
	pInterface = (PUSBD_INTERFACE_INFORMATION)
		deviceExtension->Interface[INTERFACE(irpStack->FileObject->FsContext)];

	ucPipeType = pInterface->Pipes[ulPipeNum].PipeType;	
	
	USBLOOP_KdPrint(("Transfer for pipe %d EndpointType = %x Handle = %x\n", 
		ulPipeNum, pInterface->Pipes[ulPipeNum].PipeType,
		pInterface->Pipes[ulPipeNum].PipeHandle));	

	// build our data structure with urb, timer etc. to be passed
	// to completion routines and USB driver
	pAsyncXfer = USBLOOP_BuildAsyncRequest(DeviceObject, 
			 							   Irp, 
										   pInterface->Pipes[ulPipeNum].PipeHandle,
										   FALSE);
	if (pAsyncXfer)
	{
		// mark the IROP pending, IOComplete comes after completion
		// routine called
		IoMarkIrpPending(Irp);

		// save our IRP for completion routines
		// BUG BUG - do we really need to hang onto IRP, could just pass
		// IRP into timer routine, and we get it for read write completion
		// routine
		pAsyncXfer->irp = Irp;

		nextStack = IoGetNextIrpStackLocation(Irp);
		ASSERT(nextStack != NULL);
		ASSERT(DeviceObject->StackSize>1);

		nextStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
		nextStack->Parameters.Others.Argument1 = &pAsyncXfer->urb;
		nextStack->Parameters.DeviceIoControl.IoControlCode = 
			IOCTL_INTERNAL_USB_SUBMIT_URB;

		// setup completion routine for this IRP
		IoSetCompletionRoutine(Irp,
							   USBLOOP_AsyncReadWrite_Complete,
							   pAsyncXfer,
							   TRUE,
							   TRUE,
							   TRUE);
								
		ntStatus = IoCallDriver(deviceExtension->StackDeviceObject, 
								Irp);

		ASSERT(ntStatus == STATUS_PENDING);
		USBLOOP_KdPrint(("exit USBLOOP_Write: Status = PENDING\n"));

		// we must be pending, so completion routine will finish things
		return ntStatus;									
	}
	// couldn't create our data structure
	else
		ntStatus = STATUS_NO_MEMORY;

	// couldn't create data structure, so this one fails
	Irp->IoStatus.Status = ntStatus;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	USBLOOP_KdPrint(("exit USBLOOP_Write: USBLOOP_BuildAsyncRequest call failed.\n"));

	return ntStatus;			
}

NTSTATUS
USBLOOP_getPipeAttr(
	IN PCWSTR	pDeviceName,
	OUT PULONG	pPipeAttr
	)
/**************************************************************************

Routine Description:

	Extracts information about an opened pipe from device name passed in.
	Returns info in pPipeAttr. This routine could probably be a lot better,
	but not sure what wide char routines I can use in kernel. This routine
	could also do lots of error checking to see if the stuff passed in 
	device name is valid.

Arguments:

Return Value:
	STATUS_SUCCESS if successful,
	STATUS_UNSUCCESSFUL otherwise
	
**************************************************************************/
{
	UNICODE_STRING	unicodeString;
	NTSTATUS		ntStatus;
	ULONG			attr[4];
	PWSTR			pTmp[4];
	PWSTR			pBuff;
	int				index;

	USBLOOP_KdPrint (("enter USBLOOP_getPipeAttr\n"));

	// convert name of device from wide char to UNICODE
	RtlInitUnicodeString(&unicodeString, pDeviceName);

	// init our buffer pointer to point at buffer in unicode string
	pBuff = unicodeString.Buffer;

	// turn all of '\' chars into '\0', save 4 pointers to wide strings
	for(index = 0; index < NUM_ATTR_BYTES; index++)
	{

		// check for leading '\' character, turn into NULL terrminator
		if(*pBuff != '\\')
		{
			// must not be a valid pipe name, so fail it
			USBLOOP_KdPrint (("exit USBLOOP_getPipeAttr: not a valid pipe name\n"));
			return STATUS_NO_SUCH_DEVICE;
		}
		else
			*pBuff = '\0';

		// increment past this, save pointer to this string
		pTmp[index] = ++pBuff;

		// now increment past string
		while(*pBuff != '\0' && *pBuff != '\\')
			pBuff++;
	}

	// turn all the wide char strings into integers
	for(index = 0; index < NUM_ATTR_BYTES; index++)
	{
		// change to UNICODE first, not sure what routines for wide
		// char support are in kernel
		RtlInitUnicodeString(&unicodeString, pTmp[index]);

		ntStatus = RtlUnicodeStringToInteger(&unicodeString, 10,
											 (int *)&attr[index]);
	}

	// fill in all of the attributes for this pipe
	*pPipeAttr = MAKEPIPEATTR(attr[0], attr[1], attr[2], attr[3]);

	USBLOOP_KdPrint (("exit USBLOOP_getPipeAttr\n"));
	return ntStatus;
}

pAsyncTransfer
USBLOOP_BuildAsyncRequest(
	IN PDEVICE_OBJECT	DeviceObject,
	IN PIRP				Irp,
	IN USBD_PIPE_HANDLE	PipeHandle,
	IN BOOLEAN			Read
	)
/*++

Routine Description:
	Builds a URB and sets up a timer for this call, so if it takes too
	long we cancel IRP
Arguments:

	DeviceObject	- pointer to the device extension for this instance of the 
					  USBLOOP device.

	Irp				- Well, the irp
	
	PipeHandle		- PipeHandle for endpoint

	Read			- read flag for transfer

Return Value:

	initialized async data structure with urb, timer etc.

--*/
{
	ULONG			siz;
	ULONG			length;
	pAsyncTransfer	pAsyncXfer;
	PURB			urb;

	USBLOOP_KdPrint (("enter USBLOOP_BuildAsyncRequest\n"));

	length = MmGetMdlByteCount(Irp->MdlAddress);

	// we will allocate a structure that contains urb, timer, etc
	siz = sizeof(AsyncTransfer);
	pAsyncXfer = ExAllocatePool(NonPagedPool, siz);

	// get a pointer to the urb in data structure
	urb = (PURB) &pAsyncXfer->urb;

	if (pAsyncXfer)
	{	   
		LARGE_INTEGER dueTime;

		RtlZeroMemory(pAsyncXfer, siz);
	
		// setup urb
		urb->UrbBulkOrInterruptTransfer.Length = 
			(USHORT) sizeof(struct _URB_BULK_OR_INTERRUPT_TRANSFER);
		urb->UrbBulkOrInterruptTransfer.Function = 
					URB_FUNCTION_BULK_OR_INTERRUPT_TRANSFER;
		urb->UrbBulkOrInterruptTransfer.PipeHandle = PipeHandle;
		urb->UrbBulkOrInterruptTransfer.TransferFlags = 
			Read ? USBD_TRANSFER_DIRECTION_IN : 0;
				
		//
		// no linkage for now
		//
		
		urb->UrbBulkOrInterruptTransfer.UrbLink = NULL;

		urb->UrbBulkOrInterruptTransfer.TransferBufferMDL = 
			Irp->MdlAddress;
		urb->UrbBulkOrInterruptTransfer.TransferBufferLength = 
			length;
			
		USBLOOP_KdPrint (("Init async urb Length = 0x%x buf = 0x%x\n", 
			urb->UrbBulkOrInterruptTransfer.TransferBufferLength,
			urb->UrbBulkOrInterruptTransfer.TransferBuffer));

		pAsyncXfer->bTimerExpired = FALSE;

		// now let's setup a timer in case this does not complete
		KeInitializeTimer(&pAsyncXfer->TimeoutTimer);  //build the timer object
		
		KeInitializeDpc(&pAsyncXfer->TimeoutDpc,	//setup the DPC call based
						USBLOOP_SyncTimeoutDPC,		//DPC func
						pAsyncXfer);				//context ptr to pass into DPC func

		// 5 second timer
		dueTime.QuadPart = -10000 * DEADMAN_TIMEOUT;

		KeSetTimer(&pAsyncXfer->TimeoutTimer,	//Set the timer params up
				   dueTime,						//This is how long to wait
				   &pAsyncXfer->TimeoutDpc);	//This is the DPC object we created
	} 

	USBLOOP_KdPrint (("exit USBLOOP_BuildAsyncRequest\n"));		

	// return pointer to data structure
	return pAsyncXfer;
}
