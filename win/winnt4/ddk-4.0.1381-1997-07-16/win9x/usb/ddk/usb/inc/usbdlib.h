/*++

Copyright (c) 1995	Microsoft Corporation

Module Name:

	USBDLIB.H

Abstract:

   Services exported by USBD.

Environment:

    Kernel & user mode

Revision History:

    06-10-96 : created

--*/

#ifndef   __USBDLIB_H__
#define   __USBDLIB_H__

DECLSPEC_IMPORT
VOID 
USBD_Debug_LogEntry(
	IN CHAR *Name, 
	IN ULONG Info1, 
	IN ULONG Info2, 
	IN ULONG Info3
	);

DECLSPEC_IMPORT
PVOID
USBD_Debug_GetHeap(
	IN POOL_TYPE PoolType,
    IN ULONG NumberOfBytes,
    IN ULONG Signature,
    IN PLONG TotalAllocatedHeapSpace
    );	

DECLSPEC_IMPORT
VOID
USBD_Debug_RetHeap(
	IN PVOID P,
	IN ULONG Signature,
	IN PLONG TotalAllocatedHeapSpace
    ); 

DECLSPEC_IMPORT
PUSB_INTERFACE_DESCRIPTOR
USBD_ParseConfigurationDescriptor(
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor, 
    IN UCHAR InterfaceNumber, 
    IN UCHAR AlternateSetting
    );    

DECLSPEC_IMPORT
VOID
USBD_GetUSBDIVersion(
    PUSBD_VERSION_INFORMATION VersionInformation
    );

DECLSPEC_IMPORT
PURB
USBD_CreateConfigurationRequest(
    IN PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    IN OUT PUSHORT Siz
    );    
    
#endif /* __USBDLIB_H__ */
