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
PacketWrite(
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

//    PIO_STACK_LOCATION IrpSp;
    PDEVICE_EXTENSION  DeviceExtension;
    PLIST_ENTRY         PacketListEntry;
    PNDIS_PACKET       pPacket;

    NDIS_STATUS     Status;

    IF_LOUD(DbgPrint("Packet: SendAdapter\n");)

    DeviceExtension = DeviceObject->DeviceExtension;

    //
    //  Get a free packet from our list
    //
    PacketListEntry=ExInterlockedRemoveHeadList(&DeviceExtension->FreePacketList,
                                                &DeviceExtension->FreePacketListSpinLock);

    if (PacketListEntry == NULL) {
        //
        //  No free packets
        //
        Irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
        return STATUS_UNSUCCESSFUL;
    }



    pPacket=CONTAINING_RECORD(PacketListEntry,NDIS_PACKET,ProtocolReserved);
    RESERVED(pPacket)->Irp=Irp;

    //
    //  Attach the writes buffer to the packet
    //
    NdisChainBufferAtFront(pPacket,Irp->MdlAddress);

    IoMarkIrpPending(Irp);
    Irp->IoStatus.Status = STATUS_PENDING;

    //
    //  Call the MAC
    //
    NdisSend(
        &Status,
        DeviceExtension->AdapterHandle,
        pPacket);


    if (Status != NDIS_STATUS_PENDING) {
        //
        //  The send didn't pend so call the completion handler now
        //
        PacketSendComplete(
            DeviceExtension,
            pPacket,
            Status
            );


    }



    return(STATUS_PENDING);

}



VOID
PacketSendComplete(
    IN NDIS_HANDLE   ProtocolBindingContext,
    IN PNDIS_PACKET  pPacket,
    IN NDIS_STATUS   Status
    )

{
    PDEVICE_EXTENSION DeviceExtension;
    PIRP              Irp;

    IF_LOUD(DbgPrint("Packet: SendComplete\n");)

    DeviceExtension= (PDEVICE_EXTENSION)ProtocolBindingContext;
    Irp=RESERVED(pPacket)->Irp;

    //
    //  recyle the packet
    //
    NdisReinitializePacket(pPacket);

    //
    //  Put the packet back on the free list
    //
    ExInterlockedInsertTailList(
        &DeviceExtension->FreePacketList,
        &RESERVED(pPacket)->ListElement,
        &DeviceExtension->FreePacketListSpinLock);


    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return;

}
