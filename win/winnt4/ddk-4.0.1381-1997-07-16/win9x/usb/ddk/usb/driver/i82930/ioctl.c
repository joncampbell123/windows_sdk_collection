/*++

Copyright (c) 1995  Microsoft Corporation

Module Name:

   ioctl.c

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
#include "I82930.h"

#include "usb.h"
#include "ioctl.h"

#if 0
NTSTATUS
I82930_SelectInterface(
    IN PDEVICE_OBJECT DeviceObject,
    IN UCHAR InterfaceNumber,
    IN UCHAR AlternateSetting
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
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;

    I82930_KdPrint (("enter I82930_SelectInterface if-%d alt-%d\n", InterfaceNumber, 
        AlternateSetting));    

    deviceExtension = DeviceObject->DeviceExtension;

    configurationDescriptor = 
            I82930_GetConfigDescriptor(DeviceObject);

    if (configurationDescriptor)  {

        interfaceDescriptor = 
            USBD_ParseConfigurationDescriptor(configurationDescriptor, 
                                              InterfaceNumber, 
                                              AlternateSetting);
        //
        // Allocate a URB big enough for this request, use the interface 
        // descriptor for the new interface
        // 
        
        siz = GET_SELECT_INTERFACE_REQUEST_SIZE(interfaceDescriptor->bNumEndpoints);

        ExFreePool(configurationDescriptor);                          
        
        I82930_KdPrint (("size of interface request Urb = %d\n", siz));
        
        urb = ExAllocatePool(NonPagedPool, 
                             siz);
                             
        if (urb) {

            UsbBuildSelectInterfaceRequest(urb,
                                           (USHORT) siz,
                                           deviceExtension->ConfigurationHandle,
                                           InterfaceNumber,
                                           AlternateSetting);
                                         
            ntStatus = I82930_CallUSBD(DeviceObject, urb);

            if (NT_SUCCESS(ntStatus) && USBD_SUCCESS(urb->UrbSelectInterface.Status)) {

                PUSBD_INTERFACE_INFORMATION interface;

                //
                // current interface is no longer valid,
                // free it
                //
                
                ExFreePool(deviceExtension->Interface[InterfaceNumber]);
                deviceExtension->Interface[InterfaceNumber] = NULL;

                interface = &urb->UrbSelectInterface.Interface;
                
                deviceExtension->Interface[InterfaceNumber] = ExAllocatePool(NonPagedPool,
                                                                             interface->Length);

                if (deviceExtension->Interface[InterfaceNumber]) {
                
                    ULONG j;
                        
                    //
                    // save a copy of the interface information returned
                    //
                    RtlCopyMemory(deviceExtension->Interface[InterfaceNumber], interface, interface->Length);

                    //
                    // Dump the interface to the debugger
                    //
                    I82930_KdPrint (("---------\n")); 
                    I82930_KdPrint (("NumberOfPipes 0x%x\n", deviceExtension->Interface[InterfaceNumber]->NumberOfPipes));
                    I82930_KdPrint (("Length 0x%x\n", deviceExtension->Interface[InterfaceNumber]->Length));  
                    I82930_KdPrint (("Alt Setting 0x%x\n", deviceExtension->Interface[InterfaceNumber]->AlternateSetting));  
                    I82930_KdPrint (("Interface Number 0x%x\n", deviceExtension->Interface[InterfaceNumber]->InterfaceNumber)); 

                    // Dump the pipe info

                    for (j=0; j<interface->NumberOfPipes; j++) {
                        PUSBD_PIPE_INFORMATION pipeInformation;

                        pipeInformation = &deviceExtension->Interface[InterfaceNumber]->Pipes[j];

                        I82930_KdPrint (("---------\n")); 
                        I82930_KdPrint (("PipeType 0x%x\n", pipeInformation->PipeType));
                        I82930_KdPrint (("EndpointAddress 0x%x\n", pipeInformation->EndpointAddress));
                        I82930_KdPrint (("MaxPacketSize 0x%x\n", pipeInformation->MaximumPacketSize));
                        I82930_KdPrint (("Interval 0x%x\n", pipeInformation->Interval));
                        I82930_KdPrint (("Handle 0x%x\n", pipeInformation->PipeHandle));                       
                    }

                    I82930_KdPrint (("---------\n"));                     
                }
            }            
            
            ExFreePool(urb);
            
        } else {
            ntStatus = STATUS_NO_MEMORY;        
        }        

    } else {
        /* could not get config descriptor */
        ntStatus = STATUS_NO_MEMORY;   
    }
        
    return ntStatus;
}  
#endif


PUSB_CONFIGURATION_DESCRIPTOR
I82930_GetConfigDescriptor(
    IN PDEVICE_OBJECT DeviceObject
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
    NTSTATUS ntStatus;
    PURB urb;
    ULONG siz;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor = NULL;

    I82930_KdPrint (("enter I82930_GetConfigDescriptor\n"));    

    deviceExtension = DeviceObject->DeviceExtension;
    
    urb = ExAllocatePool(NonPagedPool, 
                         sizeof(struct _URB_CONTROL_DESCRIPTOR_REQUEST));
                         
    if (urb) {

        // BUGBUG 82930 chokes if on the next command if you don't get
        // the entire descriptor on the first try
        
        siz = sizeof(USB_CONFIGURATION_DESCRIPTOR)+256;

get_config_descriptor_retry2:
        
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
            goto get_config_descriptor_retry2;
        }            

        ExFreePool(urb);
        
    } else {
        ntStatus = STATUS_NO_MEMORY;        
    }        
    
    I82930_KdPrint (("enter I82930_GetConfigDescriptor\n"));    

    return configurationDescriptor;
}    


NTSTATUS
I82930_ProcessIOCTL(
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
    PIO_STACK_LOCATION irpStack, nextStack;
    PVOID ioBuffer;
    ULONG inputBufferLength;
    ULONG outputBufferLength;
    PDEVICE_EXTENSION deviceExtension;
    ULONG ioControlCode;
    NTSTATUS ntStatus;
    ULONG length, i;
    PUCHAR pch;
    PULONG altSetting;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;
     
    I82930_KdPrint (("IRP_MJ_DEVICE_CONTROL\n"));
    
    //
    // Get a pointer to the current location in the Irp. This is where
    //     the function codes and parameters are located.
    //

    irpStack = IoGetCurrentIrpStackLocation (Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    //
    // Get a pointer to the device extension
    //

    deviceExtension = DeviceObject->DeviceExtension;

    ioBuffer           = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength  = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;

    ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

    // 
    // Handle Ioctls from User mode
    //

    switch (ioControlCode) {

    case IOCTL_I82930_SET_PIPE_PARAMETER:
        
        break;

    case IOCTL_I82930_GET_PIPE_INFO:

        {
        PI82930_INTERFACE_INFO interfaceInfo;
        UNICODE_STRING unicodePipeName;
        ANSI_STRING ansiPipeName;
        ULONG pipeCount = 0;
         
        //
        // inputs  - none
        // outputs - a list of pipes available for io testing
        //

        interfaceInfo = ioBuffer;

        I82930_KdPrint (("info buffer = 0x%x, pipelist 0x%x\n", interfaceInfo,
            &deviceExtension->PipeList[0]));    

        for (i=0; i<I82930_MAX_PIPES; i++) {
            PI82930_PIPE pipe;

            pipe = &deviceExtension->PipeList[i];

            if (pipe->PipeInfo == NULL) {
                continue;
            }

        	switch(pipe->PipeInfo->PipeType) {
            case UsbdPipeTypeInterrupt:
        		interfaceInfo->Pipes[pipeCount].PipeType = INTERRUPT;
        		break;
        	case UsbdPipeTypeBulk:
        	    interfaceInfo->Pipes[pipeCount].PipeType = BULK;
        	    break;
        	case UsbdPipeTypeIsochronous:
    	    	interfaceInfo->Pipes[pipeCount].PipeType = ISO;
    	    	break;
        	case UsbdPipeTypeControl:
        	    interfaceInfo->Pipes[pipeCount].PipeType = CONTROL;    
        	    break;
        	default:
    	    	TRAP();	
        	}
        	
            interfaceInfo->Pipes[pipeCount].EndpointAddress = 
                pipe->PipeInfo->EndpointAddress;
                
            interfaceInfo->Pipes[pipeCount].MaximumPacketSize = 
                pipe->PipeInfo->MaximumPacketSize;                

            interfaceInfo->Pipes[pipeCount].MaximumTransferSize = 
                pipe->PipeInfo->MaximumTransferSize;                     

            interfaceInfo->Pipes[pipeCount].Interval = 
                pipe->PipeInfo->Interval;                  

            interfaceInfo->Pipes[pipeCount].In = 
                pipe->PipeInfo->EndpointAddress & 0x80;
                
            //
            // copy the name
            //
            
            RtlCopyMemory(interfaceInfo->Pipes[pipeCount].Name, 
                          "\\PIPE00",
                          8);  

            RtlInitUnicodeString(&unicodePipeName, 
                                 pipe->Name);     
                                 
            RtlInitAnsiString(&ansiPipeName, interfaceInfo->Pipes[pipeCount].Name);             
            
            RtlUnicodeStringToAnsiString(&ansiPipeName, &unicodePipeName, FALSE);

            pipeCount++;
        }

        interfaceInfo->PipeCount = pipeCount;

        length = sizeof(I82930_INTERFACE_INFO) + sizeof(I82930_PIPE_INFO) * 
                        interfaceInfo->PipeCount;    
        
        Irp->IoStatus.Information = length;                
        Irp->IoStatus.Status = STATUS_SUCCESS;
        }
        
        break;

     case IOCTL_I82930_GET_CONFIG_DESCRIPTOR:

        //
        // This api returns a copy of the configuration descriptor
        // and all endpoint/interface descriptors.
        //

        //
        // inputs  - none
        // outputs - configuration descriptor plus interface 
        //          and endpoint descriptors
        //
    
        length = 0;
        pch = (PUCHAR) ioBuffer;

        configurationDescriptor = 
            I82930_GetConfigDescriptor(DeviceObject);

        if (configurationDescriptor)  {
            length = configurationDescriptor->wTotalLength;

            RtlCopyMemory(pch, 
                          (PUCHAR) configurationDescriptor,
                          length);  

            ExFreePool(configurationDescriptor);                          
        }            
        
        
        Irp->IoStatus.Information = length;                
        Irp->IoStatus.Status = STATUS_SUCCESS;
        
        break;

    default:
        
        Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;            
    }  

    ntStatus = Irp->IoStatus.Status;

    IoCompleteRequest (Irp,
                       IO_NO_INCREMENT
                       );

    return ntStatus;                       
        
}




