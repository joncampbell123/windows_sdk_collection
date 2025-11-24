/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    loopback.c

Abstract:

    The routines here indicate packets on the loopback queue and are
    responsible for inserting and removing packets from the loopback
    queue and the send finishing queue.

Environment:

    Operates at dpc level - or the equivalent on os2 and dos.

--*/

//
// So we can trace things...
//
//#define STATIC

#include <ne3200sw.h>
#ifdef NDIS_WIN
    #pragma LCODE
#endif


VOID
NE3200ProcessLoopback(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is responsible for indicating *one* packet on
    the loopback queue either completing it or moving on to the
    finish send queue.

    NOTE: This routine is called with the spin lock held!!!

Arguments:

    Adapter - The adapter whose loopback queue we are processing.

Return Value:

    None.

--*/

{

    //
    // Packet at the head of the loopback list.
    //
    PNDIS_PACKET PacketToMove;

    //
    // The reserved portion of the above packet.
    //
    PNE3200_RESERVED Reserved;

    //
    // The first buffer in the ndis packet to be loopbacked.
    //
    PNDIS_BUFFER FirstBuffer;

    //
    // The total amount of user data in the packet to be
    // loopbacked.
    //
    UINT TotalPacketLength;

    //
    // Eventually the address of the data to be indicated
    // to the transport.
    //
    PVOID BufferAddress;

    //
    // Eventually the length of the data to be indicated
    // to the transport.
    //
    UINT BufferLength;

    //
    // Remove it from the list.
    //

    PacketToMove = Adapter->FirstLoopBack;

    Reserved = PNE3200_RESERVED_FROM_PACKET(PacketToMove);

    if (!Reserved->Next) {

        Adapter->LastLoopBack = NULL;

    }

    Adapter->FirstLoopBack = Reserved->Next;

    NdisReleaseSpinLock(&Adapter->Lock);

    //
    // See if we need to copy the data from the packet
    // into the loopback buffer.
    //
    // We need to copy to the local loopback buffer if
    // the first buffer of the packet is less than the
    // minimum loopback size AND the first buffer isn't
    // the total packet.
    //

    NdisQueryPacket(
        PacketToMove,
        NULL,
        NULL,
        &FirstBuffer,
        &TotalPacketLength
        );

    NdisQueryBuffer(
        FirstBuffer,
        &BufferAddress,
        &BufferLength
        );

    if ((BufferLength < NE3200_SIZE_OF_LOOPBACK) &&
        (BufferLength != TotalPacketLength)) {

        NE3200CopyFromPacketToBuffer(
            PacketToMove,
            0,
            NE3200_SIZE_OF_LOOPBACK,
            Adapter->Loopback,
            &BufferLength
            );

        BufferAddress = Adapter->Loopback;

    }

    //
    // Indicate the packet to every open binding
    // that could want it.
    //

    if (BufferLength < NE3200_HEADER_SIZE) {

        if (BufferLength >= ETH_LENGTH_OF_ADDRESS) {

            //
            // Runt packet
            //

            EthFilterIndicateReceive(
                Adapter->FilterDB,
                (NDIS_HANDLE)(((ULONG)PacketToMove) | 1),
                ((PCHAR)BufferAddress),
                BufferAddress,
                BufferLength,
                NULL,
                0,
                0
                );

        }

    } else {

        EthFilterIndicateReceive(
            Adapter->FilterDB,
            (NDIS_HANDLE)(((ULONG)PacketToMove) | 1),
            ((PCHAR)BufferAddress),
            BufferAddress,
            NE3200_HEADER_SIZE,
            ((PUCHAR)BufferAddress) + NE3200_HEADER_SIZE,
            BufferLength - NE3200_HEADER_SIZE,
            TotalPacketLength - NE3200_HEADER_SIZE
            );

    }

    NdisAcquireSpinLock(&Adapter->Lock);

    if (!Reserved->STAGE.STAGE4.ReadyToComplete) {

        //
        // We can decrement the reference count on the open by one since
        // it is no longer being "referenced" by the packet on the
        // loopback queue.
        //

        PNE3200_OPEN_FROM_BINDING_HANDLE(
            Reserved->MacBindingHandle
            )->References--;
        NE3200PutPacketOnFinishTrans(
            Adapter,
            PacketToMove
            );

    } else {

        PNE3200_OPEN Open;
        //
        // Increment the reference count on the open so that
        // it will not be deleted out from under us while
        // where indicating it.
        //

        Open = PNE3200_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

        NdisReleaseSpinLock(&Adapter->Lock);

        NdisCompleteSend(
            Open->NdisBindingContext,
            PacketToMove,
            ((Reserved->STAGE.STAGE4.SuccessfulTransmit)?
             (NDIS_STATUS_SUCCESS):(NDIS_STATUS_FAILURE))
            );

        NdisAcquireSpinLock(&Adapter->Lock);

        //
        // We can decrement the reference count for the packet being on
        // the queue.
        //

        Open->References--;

    }

}

