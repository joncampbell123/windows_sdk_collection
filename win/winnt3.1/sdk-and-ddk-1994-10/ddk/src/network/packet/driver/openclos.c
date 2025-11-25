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




NTSTATUS
PacketOpen(
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

    PDEVICE_EXTENSION DeviceExtension;

    NDIS_STATUS     Status;
    NDIS_STATUS     ErrorStatus;
    UINT            Medium;
    NDIS_MEDIUM     MediumArray=NdisMedium802_3;

    IF_LOUD(DbgPrint("Packet: OpenAdapter\n");)

    DeviceExtension = DeviceObject->DeviceExtension;

    //
    //  Save the Irp here. This should be ok as I have opened this
    //  device as exclusive
    //
    DeviceExtension->OpenCloseIrp=Irp;

    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

    //
    //  Try to open the MAC
    //
    NdisOpenAdapter(
        &Status,
        &ErrorStatus,
        &DeviceExtension->AdapterHandle,
        &Medium,
        &MediumArray,
        1,
        DeviceExtension->NdisProtocolHandle,
        DeviceExtension,
        &DeviceExtension->AdapterName,
        0,
        NULL);


    if (Status != NDIS_STATUS_PENDING) {

        PacketOpenAdapterComplete(
            DeviceExtension,
            Status,
            NDIS_STATUS_SUCCESS
            );


    }



    return(STATUS_PENDING);

}










VOID
PacketOpenAdapterComplete(
    IN NDIS_HANDLE  ProtocolBindingContext,
    IN NDIS_STATUS  Status,
    IN NDIS_STATUS  OpenErrorStatus
    )

{

    PDEVICE_EXTENSION DeviceExtension;
    PIRP              Irp;

    IF_LOUD(DbgPrint("Packet: OpenAdapterComplete\n");)

    DeviceExtension= (PDEVICE_EXTENSION)ProtocolBindingContext;
    Irp=DeviceExtension->OpenCloseIrp;

    Irp->IoStatus.Status = Status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return;

}




NTSTATUS
PacketClose(
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

    PDEVICE_EXTENSION DeviceExtension;

    NDIS_STATUS     Status;

    IF_LOUD(DbgPrint("Packet: CloseAdapter\n");)

    DeviceExtension = DeviceObject->DeviceExtension;

    //
    //  Save the IRP
    //
    DeviceExtension->OpenCloseIrp=Irp;

    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

    NdisCloseAdapter(
        &Status,
        DeviceExtension->AdapterHandle
        );


    if (Status != NDIS_STATUS_PENDING) {

        PacketCloseAdapterComplete(
            DeviceExtension,
            Status
            );


    }



    return(STATUS_PENDING);

}





VOID
PacketCloseAdapterComplete(
    IN NDIS_HANDLE  ProtocolBindingContext,
    IN NDIS_STATUS  Status
    )

{
    PDEVICE_EXTENSION DeviceExtension;
    PIRP              Irp;

    IF_LOUD(DbgPrint("Packet: CloseAdapterComplete\n");)

    DeviceExtension= (PDEVICE_EXTENSION)ProtocolBindingContext;

    Irp=DeviceExtension->OpenCloseIrp;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return;

}











NTSTATUS
PacketCleanup(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP FlushIrp
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

    PDEVICE_EXTENSION   DeviceExtension;
    PLIST_ENTRY         PacketListEntry;
    PNDIS_PACKET        pPacket;
    NDIS_STATUS         Status;

    IF_LOUD(DbgPrint("Packet: Cleanup\n");)

    DeviceExtension = DeviceObject->DeviceExtension;

    //
    //  The open instance of the device is about to close
    //  We need to complete all pending Irp's
    //  First we complete any pending read requests
    //
    while ((PacketListEntry=ExInterlockedRemoveHeadList(
                                &DeviceExtension->RcvList,
                                &DeviceExtension->RcvQSpinLock
                                )) != NULL) {

        IF_LOUD(DbgPrint("Packet: CleanUp - Completeing read\n");)

        pPacket=CONTAINING_RECORD(PacketListEntry,NDIS_PACKET,ProtocolReserved);

        //
        //  complete normally
        //
        PacketTransferDataComplete(
            DeviceExtension,
            pPacket,
            NDIS_STATUS_SUCCESS,
            0
            );


    }

    IoMarkIrpPending(FlushIrp);
    FlushIrp->IoStatus.Status = STATUS_PENDING;

    //
    //  We now place the Irp on the Reset list
    //
    ExInterlockedInsertTailList(
            &DeviceExtension->ResetIrpList,
            &FlushIrp->Tail.Overlay.ListEntry,
            &DeviceExtension->DeviceSpinLock);


    //
    //  Now reset the adapter, the mac driver will complete any
    //  pending requests we have made to it.
    //
    NdisReset(
        &Status,
        DeviceExtension->AdapterHandle
        );


    if (Status != NDIS_STATUS_PENDING) {

        IF_LOUD(DbgPrint("Packet: Cleanup - ResetComplte being called\n");)

        PacketResetComplete(
            DeviceExtension,
            Status
            );


    }



    return(STATUS_PENDING);


}



VOID
PacketResetComplete(
    IN NDIS_HANDLE  ProtocolBindingContext,
    IN NDIS_STATUS  Status
    )

{
    PDEVICE_EXTENSION   DeviceExtension;
    PIRP                Irp;

    PLIST_ENTRY         ResetListEntry;

    IF_LOUD(DbgPrint("Packet: PacketResetComplte\n");)

    DeviceExtension= (PDEVICE_EXTENSION)ProtocolBindingContext;

    //
    //  remove the reset IRP from the list
    //
    ResetListEntry=ExInterlockedRemoveHeadList(&DeviceExtension->ResetIrpList,
                                                &DeviceExtension->DeviceSpinLock);

#if DBG
    if (ResetListEntry == NULL) {
        DbgBreakPoint();
        return;
    }
#endif

    Irp=CONTAINING_RECORD(ResetListEntry,IRP,Tail.Overlay.ListEntry);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    IF_LOUD(DbgPrint("Packet: PacketResetComplte exit\n");)

    return;

}




NTSTATUS
PacketShutdown(
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

    PDEVICE_EXTENSION   DeviceExtension;

    IF_LOUD(DbgPrint("Packet: Shutdown\n");)

    DeviceExtension = DeviceObject->DeviceExtension;

    Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
    return STATUS_UNSUCCESSFUL;


}
