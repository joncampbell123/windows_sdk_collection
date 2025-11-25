/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    packet.c

Abstract:


Author:


Environment:

    Kernel mode only.

Notes:


Future:



Revision History:

--*/

#include "stdarg.h"
#include "ntddk.h"
#include "ntiologc.h"
#include "ndis.h"

#include "debug.h"
#include "packet.h"


UINT ProtocolSupportedOids[] = {
    OID_GEN_SUPPORTED_LIST,                  // 0
    OID_GEN_HARDWARE_STATUS,                 // 1
    OID_GEN_MEDIA_SUPPORTED,                 // 2
    OID_GEN_MEDIA_IN_USE,                    // 3
    OID_GEN_MAXIMUM_LOOKAHEAD,               // 4
    OID_GEN_MAXIMUM_FRAME_SIZE,              // 5
    OID_GEN_MAXIMUM_TOTAL_SIZE,              // 6
    OID_GEN_LINK_SPEED,                      // 7
    OID_GEN_TRANSMIT_BUFFER_SPACE,           // 8
    OID_GEN_RECEIVE_BUFFER_SPACE,            // 9
    OID_GEN_TRANSMIT_BLOCK_SIZE,             //10
    OID_GEN_RECEIVE_BLOCK_SIZE,              //11
    OID_GEN_VENDOR_ID,                       // 2
    OID_GEN_DRIVER_VERSION,                  // 3
    OID_GEN_CURRENT_PACKET_FILTER,           // 4
    OID_GEN_CURRENT_LOOKAHEAD,               // 5
    OID_802_3_PERMANENT_ADDRESS,             // 6
    OID_802_3_CURRENT_ADDRESS,               // 7
    OID_802_3_MULTICAST_LIST,                // 8
    OID_802_3_MAXIMUM_LIST_SIZE              // 9
    };





NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );



NTSTATUS
PacketReadRegistry(
    IN  PUNICODE_STRING     MacDriverName,
    IN  PUNICODE_STRING     PacketDriverName,
    IN  PUNICODE_STRING     RegistryPath
    );


NTSTATUS
PacketCreateSymbolicLink(
    IN  PUNICODE_STRING  DeviceName,
    IN  BOOLEAN          Create
    );



#if DBG
//
// Declare the global debug flag for this driver.
//

ULONG PacketDebugFlag = PACKET_DEBUG_LOUD;

#endif

PDEVICE_EXTENSION GlobalDeviceExtension;



NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This routine initializes the Packet (i8250) mouse port driver.

Arguments:

    DriverObject - Pointer to driver object created by system.

    RegistryPath - Pointer to the Unicode name of the registry path
        for this driver.

Return Value:

    The function value is the final status from the initialization operation.

--*/

{

    NDIS_PROTOCOL_CHARACTERISTICS  ProtocolChar;

    UNICODE_STRING MacDriverName;
    UNICODE_STRING UnicodeDeviceName;

    PDEVICE_OBJECT DeviceObject = NULL;
    PDEVICE_EXTENSION DeviceExtension = NULL;

    NTSTATUS Status = STATUS_SUCCESS;
    NTSTATUS ErrorCode = STATUS_SUCCESS;
    NTSTATUS FailStatus;
    ULONG i;
    NDIS_STRING ProtoName = NDIS_STRING_CONST("PacketDriver");

    PNDIS_PACKET   pPacket;

    IF_LOUD(DbgPrint("\n\nPacket: DriverEntry\n");)

    //
    //  Allocate Memory for the unicode used to jold the Macdriver name
    //

    MacDriverName.MaximumLength    = 128*sizeof(WCHAR);
    MacDriverName.Length           = 0;
    MacDriverName.Buffer           = ExAllocatePool(NonPagedPool,128*sizeof(WCHAR));

    if (MacDriverName.Buffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    //  Allocate memory for the packet driver device object name
    //

    UnicodeDeviceName.MaximumLength = 128*sizeof(WCHAR);
    UnicodeDeviceName.Length        = 0;
    UnicodeDeviceName.Buffer        = ExAllocatePool(NonPagedPool,128*sizeof(WCHAR));

    if (UnicodeDeviceName.Buffer == NULL) {

        Status= STATUS_INSUFFICIENT_RESOURCES;
        goto Fail010;
    }

    //
    //  Get the name of the Packet driver and the name of the MAC driver
    //  to bind to from the registry
    //

    Status=PacketReadRegistry(
               &MacDriverName,
               &UnicodeDeviceName,
               RegistryPath
               );

    if (Status != STATUS_SUCCESS) {

        goto   Fail020;
    }


    IF_LOUD(DbgPrint("Packet: DeviceName=%ws  MacName=%ws\n",UnicodeDeviceName.Buffer,MacDriverName.Buffer);)


    //
    // Set up the device driver entry points.
    //

    DriverObject->MajorFunction[IRP_MJ_CREATE] = PacketOpen;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]  = PacketClose;
    DriverObject->MajorFunction[IRP_MJ_READ]   = PacketRead;
    DriverObject->MajorFunction[IRP_MJ_WRITE]  = PacketWrite;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]  = PacketCleanup;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = PacketIoControl;
    DriverObject->MajorFunction[IRP_MJ_SHUTDOWN]  = PacketShutdown;

    DriverObject->DriverUnload = PacketUnload;


    //
    //  Create the device object
    //

    Status = IoCreateDevice(
                DriverObject,
                sizeof(DEVICE_EXTENSION),
                &UnicodeDeviceName,
                FILE_DEVICE_PROTOCOL,
                0,
                TRUE,
                &DeviceObject
                );

    if (Status != STATUS_SUCCESS) {

        goto Fail020;
    }

    DeviceObject->Flags |= DO_DIRECT_IO | DO_EXCLUSIVE;
    DeviceExtension  =  (PDEVICE_EXTENSION) DeviceObject->DeviceExtension;
    DeviceExtension->DeviceObject = DeviceObject;

    IoRegisterShutdownNotification(DeviceObject);

    //
    //  Create a symbolic link for the device object so that the
    //  WIN32 app can open the device
    //

    Status=PacketCreateSymbolicLink(&UnicodeDeviceName,TRUE);

    if (Status != STATUS_SUCCESS) {
        IF_LOUD(DbgPrint("Packet: Failed to create dos device name\n");)
        goto Fail030;
    }

    //
    //  Save the the name of the MAC driver to open in the Device Extension
    //

    DeviceExtension->DeviceName=UnicodeDeviceName;
    DeviceExtension->AdapterName=MacDriverName;

    ProtocolChar.MajorNdisVersion            = 3;
    ProtocolChar.MinorNdisVersion            = 0;
    ProtocolChar.Reserved                    = 0;
    ProtocolChar.OpenAdapterCompleteHandler  = PacketOpenAdapterComplete;
    ProtocolChar.CloseAdapterCompleteHandler = PacketCloseAdapterComplete;
    ProtocolChar.SendCompleteHandler         = PacketSendComplete;
    ProtocolChar.TransferDataCompleteHandler = PacketTransferDataComplete;
    ProtocolChar.ResetCompleteHandler        = PacketResetComplete;
    ProtocolChar.RequestCompleteHandler      = PacketRequestComplete;
    ProtocolChar.ReceiveHandler              = PacketReceiveIndicate;
    ProtocolChar.ReceiveCompleteHandler      = PacketReceiveComplete;
    ProtocolChar.StatusHandler               = PacketStatus;
    ProtocolChar.StatusCompleteHandler       = PacketStatusComplete;
    ProtocolChar.Name                        = ProtoName;

    NdisRegisterProtocol(
        &Status,
        &DeviceExtension->NdisProtocolHandle,
        &ProtocolChar,
        sizeof(NDIS_PROTOCOL_CHARACTERISTICS));

    if (Status != NDIS_STATUS_SUCCESS) {
        IF_LOUD(DbgPrint("Packet: Failed to register protocol with NDIS\n");)
        goto Fail040;
    }



    //
    //  General Purpose Spinlock
    //
    KeInitializeSpinLock(&DeviceExtension->DeviceSpinLock);

    InitializeListHead(&DeviceExtension->ResetIrpList);

    //
    //  Initialize free packet list, Used for send packets and
    //  transfer data packets
    //
    KeInitializeSpinLock(&DeviceExtension->FreePacketListSpinLock);
    InitializeListHead(&DeviceExtension->FreePacketList);

    //
    //  Initialize list for holding pending read requests
    //
    KeInitializeSpinLock(&DeviceExtension->RcvQSpinLock);
    InitializeListHead(&DeviceExtension->RcvList);

    KeInitializeSpinLock(&DeviceExtension->RequestSpinLock);
    InitializeListHead(&DeviceExtension->RequestList);



    for (i=0;i<MAX_REQUESTS;i++) {
        ExInterlockedInsertTailList(
            &DeviceExtension->RequestList,
            &DeviceExtension->Requests[i].ListElement,
            &DeviceExtension->RequestSpinLock);

    }

    NdisAllocatePacketPool(
        &Status,
        &DeviceExtension->PacketPool,
        TRANSMIT_PACKETS,
        sizeof(PACKET_RESERVED));

    if (Status != NDIS_STATUS_SUCCESS) {
        IF_LOUD(DbgPrint("Packet: Failed to allocate packet pool\n");)
        goto Fail050;
    }

    for (i=0;i<TRANSMIT_PACKETS;i++) {

        NdisAllocatePacket(
            &Status,
            &pPacket,
            DeviceExtension->PacketPool);

        ExInterlockedInsertTailList(
            &DeviceExtension->FreePacketList,
            &RESERVED(pPacket)->ListElement,
            &DeviceExtension->FreePacketListSpinLock);

    }




    return STATUS_SUCCESS;




Fail050:
    NdisDeregisterProtocol(
        &FailStatus,
        DeviceExtension->NdisProtocolHandle
        );


Fail040:

    PacketCreateSymbolicLink(&DeviceExtension->AdapterName,FALSE);

Fail030:
    IoUnregisterShutdownNotification(DeviceObject);
    IoDeleteDevice(DeviceObject);

Fail020:
    ExFreePool(UnicodeDeviceName.Buffer);

Fail010:
    ExFreePool(MacDriverName.Buffer);

    return(Status);

}



VOID
PacketUnload(
    IN PDRIVER_OBJECT DriverObject
    )

{

    PDEVICE_OBJECT     DeviceObject;
    PDEVICE_EXTENSION  DeviceExtension;


    IF_LOUD(DbgPrint("Packet: Unload\n");)

    DeviceObject    = DriverObject->DeviceObject;

    DeviceExtension = DeviceObject->DeviceExtension;

    PacketCreateSymbolicLink(&DeviceExtension->DeviceName,FALSE);

    ExFreePool(DeviceExtension->AdapterName.Buffer);

    ExFreePool(DeviceExtension->DeviceName.Buffer);

    IoUnregisterShutdownNotification(DeviceObject);


    IoDeleteDevice(DeviceObject);


}




NTSTATUS
PacketIoControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )

/*++

Routine Description:

    This is the dispatch routine for create/open and close requests.
    These requests complete successfully.

Arguments:

    DeviceObject - Pointer to the device object.

    Irp - Pointer to the request packet.

Return Value:

    Status is returned.

--*/

{

    PIO_STACK_LOCATION  IrpSp;
    PDEVICE_EXTENSION   DeviceExtension;
    PLIST_ENTRY         RequestListEntry;
    PINTERNAL_REQUEST   pRequest;
    UINT                FunctionCode;

    NDIS_STATUS     Status;

    IF_LOUD(DbgPrint("Packet: IoControl\n");)

    DeviceExtension = DeviceObject->DeviceExtension;


    RequestListEntry=ExInterlockedRemoveHeadList(&DeviceExtension->RequestList,
                                                &DeviceExtension->RequestSpinLock);

    if (RequestListEntry == NULL) {
        Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
        return STATUS_UNSUCCESSFUL;
    }


    pRequest=CONTAINING_RECORD(RequestListEntry,INTERNAL_REQUEST,ListElement);
    pRequest->Irp=Irp;


    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    FunctionCode=(IrpSp->Parameters.DeviceIoControl.IoControlCode >> 2) & 0x00ff;

    IF_LOUD(DbgPrint("Packet: Function code is %02x  buff size=%08lx\n",FunctionCode,IrpSp->Parameters.DeviceIoControl.InputBufferLength);)

    if (FunctionCode == PACKET_RESET) {

        IF_LOUD(DbgPrint("Packet: IoControl - Reset request\n");)

        ExInterlockedInsertTailList(
                &DeviceExtension->ResetIrpList,
                &Irp->Tail.Overlay.ListEntry,
                &DeviceExtension->DeviceSpinLock);


        NdisReset(
            &Status,
            DeviceExtension->AdapterHandle
            );


        if (Status != NDIS_STATUS_PENDING) {

            IF_LOUD(DbgPrint("Packet: IoControl - ResetComplte being called\n");)

            PacketResetComplete(
                DeviceExtension,
                Status
                );


        }

    } else {


        if (FunctionCode & 0x40) {

            pRequest->Request.RequestType=NdisRequestSetInformation;
            pRequest->Request.DATA.SET_INFORMATION.Oid=ProtocolSupportedOids[FunctionCode & 0x3f];

            pRequest->Request.DATA.SET_INFORMATION.InformationBuffer=Irp->AssociatedIrp.SystemBuffer;
            pRequest->Request.DATA.SET_INFORMATION.InformationBufferLength=IrpSp->Parameters.DeviceIoControl.InputBufferLength;


        } else {

            pRequest->Request.RequestType=NdisRequestQueryInformation;
            pRequest->Request.DATA.QUERY_INFORMATION.Oid=ProtocolSupportedOids[FunctionCode & 0x3f];

            pRequest->Request.DATA.QUERY_INFORMATION.InformationBuffer=Irp->AssociatedIrp.SystemBuffer;
            pRequest->Request.DATA.QUERY_INFORMATION.InformationBufferLength=IrpSp->Parameters.DeviceIoControl.InputBufferLength;

        }



        NdisRequest(
            &Status,
            DeviceExtension->AdapterHandle,
            &pRequest->Request);

        if (Status != NDIS_STATUS_PENDING) {

            IF_LOUD(DbgPrint("Packet: Calling RequestCompleteHandler\n");)

            PacketRequestComplete(
                DeviceExtension,
                &pRequest->Request,
                Status
                );


        }

    }

    return(STATUS_PENDING);

}





VOID
PacketRequestComplete(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS   Status
    )

{
    PIO_STACK_LOCATION  IrpSp;
    PDEVICE_EXTENSION   DeviceExtension;
    PIRP                Irp;
    PINTERNAL_REQUEST   pRequest;
    UINT                FunctionCode;

    IF_LOUD(DbgPrint("Packet: RequestComplete\n");)

    DeviceExtension= (PDEVICE_EXTENSION)ProtocolBindingContext;

    pRequest=CONTAINING_RECORD(NdisRequest,INTERNAL_REQUEST,Request);
    Irp=pRequest->Irp;

    IrpSp = IoGetCurrentIrpStackLocation(Irp);

    FunctionCode=(IrpSp->Parameters.DeviceIoControl.IoControlCode >> 2) & 0x00ff;


    if (FunctionCode & 0x40) {

        Irp->IoStatus.Information=pRequest->Request.DATA.SET_INFORMATION.BytesRead;

    } else {

        Irp->IoStatus.Information=pRequest->Request.DATA.QUERY_INFORMATION.BytesWritten;
    }


    ExInterlockedInsertTailList(
        &DeviceExtension->RequestList,
        &pRequest->ListElement,
        &DeviceExtension->RequestSpinLock);


    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return;


}








VOID
PacketStatus(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN NDIS_STATUS   Status,
    IN PVOID         StatusBuffer,
    IN UINT          StatusBufferSize
    )

{

    IF_LOUD(DbgPrint("Packet: Status Indication\n");)

    return;

}



VOID
PacketStatusComplete(
    IN NDIS_HANDLE  ProtocolBindingContext
    )

{

    IF_LOUD(DbgPrint("Packet: StatusIndicationComplete\n");)

    return;

}





NTSTATUS
PacketReadRegistry(
    IN  PUNICODE_STRING     MacDriverName,
    IN  PUNICODE_STRING     PacketDriverName,
    IN  PUNICODE_STRING     RegistryPath
    )

{

    NTSTATUS   Status;

    RTL_QUERY_REGISTRY_TABLE ParamTable[4];

    PWSTR      Bind       = L"Bind";
    PWSTR      Export     = L"Export";
    PWSTR      Parameters = L"Parameters";

    PWCHAR     Path;


    Path=ExAllocatePool(
             PagedPool,
             RegistryPath->Length+sizeof(WCHAR)
             );

    if (Path == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(
        Path,
        RegistryPath->Length+sizeof(WCHAR)
        );

    RtlMoveMemory(
        Path,
        RegistryPath->Buffer,
        RegistryPath->Length
        );

    IF_LOUD(DbgPrint("Packet: Reg path is %ws\n",RegistryPath->Buffer);)

    RtlZeroMemory(
        ParamTable,
        sizeof(ParamTable)
        );



    //
    //  change to the parmeters key
    //

    ParamTable[0].QueryRoutine = NULL;
    ParamTable[0].Flags = RTL_QUERY_REGISTRY_SUBKEY;
    ParamTable[0].Name = Parameters;



    //
    //  Get the name of the mac driver we should bind to
    //

    ParamTable[1].QueryRoutine = NULL;
    ParamTable[1].Flags = RTL_QUERY_REGISTRY_REQUIRED |
                          RTL_QUERY_REGISTRY_NOEXPAND |
                          RTL_QUERY_REGISTRY_DIRECT;

    ParamTable[1].Name = Bind;
    ParamTable[1].EntryContext = (PVOID)MacDriverName;
    ParamTable[1].DefaultType = REG_SZ;

    //
    //  Get the name that we should use for the driver object
    //

    ParamTable[2].QueryRoutine = NULL;
    ParamTable[2].Flags = RTL_QUERY_REGISTRY_REQUIRED |
                          RTL_QUERY_REGISTRY_NOEXPAND |
                          RTL_QUERY_REGISTRY_DIRECT;

    ParamTable[2].Name = Export;
    ParamTable[2].EntryContext = (PVOID)PacketDriverName;
    ParamTable[2].DefaultType = REG_SZ;


    Status=RtlQueryRegistryValues(
               RTL_REGISTRY_ABSOLUTE,
               Path,
               ParamTable,
               NULL,
               NULL
               );


    ExFreePool(Path);

    return Status;




}


NTSTATUS
PacketCreateSymbolicLink(
    IN  PUNICODE_STRING  DeviceName,
    IN  BOOLEAN          Create
    )

{


    UNICODE_STRING UnicodeDosDeviceName;
    NTSTATUS       Status;

    if (DeviceName->Length < sizeof(L"\\Device\\")) {

        return STATUS_UNSUCCESSFUL;
    }

    RtlInitUnicodeString(&UnicodeDosDeviceName,NULL);

    UnicodeDosDeviceName.MaximumLength=DeviceName->Length+sizeof(L"\\DosDevices")+sizeof(UNICODE_NULL);

    UnicodeDosDeviceName.Buffer=ExAllocatePool(
                                    NonPagedPool,
                                    UnicodeDosDeviceName.MaximumLength
                                    );

    if (UnicodeDosDeviceName.Buffer != NULL) {

        RtlZeroMemory(
            UnicodeDosDeviceName.Buffer,
            UnicodeDosDeviceName.MaximumLength
            );

        RtlAppendUnicodeToString(
            &UnicodeDosDeviceName,
            L"\\DosDevices\\"
            );

        RtlAppendUnicodeToString(
            &UnicodeDosDeviceName,
            (DeviceName->Buffer+(sizeof("\\Device")))
            );

        IF_LOUD(DbgPrint("Packet: DosDeviceName is %ws\n",UnicodeDosDeviceName.Buffer);)

        if (Create) {

            Status=IoCreateSymbolicLink(&UnicodeDosDeviceName,DeviceName);

        } else {

            Status=IoDeleteSymbolicLink(&UnicodeDosDeviceName);
        }

        ExFreePool(UnicodeDosDeviceName.Buffer);

    }


    return Status;

}
