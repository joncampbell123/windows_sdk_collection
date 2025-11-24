/*++

Copyright (c) 1990-1995  Microsoft Corporation

Module Name:

    loopback.c

Abstract:

    The routines here indicate packets on the loopback queue and are
    responsible for inserting and removing packets from the loopback
    queue and the send finishing queue.

Environment:

    Operates at dpc level - or the equivalent.

--*/

#include <ndis.h>
#include <efilter.h>

#include <sonichrd.h>
#include <sonicsft.h>
#ifdef NDIS_WIN
    #pragma LCODE
#endif


extern
VOID
SonicProcessLoopback(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is responsible for indicating *one* packet on
    the loopback queue either completing it or moving on to the
    finish send queue.

    NOTE : This routine is called with the lock held!

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
    PSONIC_PACKET_RESERVED Reserved;

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
    // The address of the first buffer in the packet.
    //

    PVOID BufferAddress;

    //
    // The address of the data to be indicated
    // to the transport.
    //
    PVOID DataAddress;

    //
    // Eventually the length of the data to be indicated
    // to the transport.
    //
    UINT BufferLength;

    //
    // Open that submitted the packet
    //
    PSONIC_OPEN Open;

    ASSERT(Adapter->FirstLoopBack != NULL);

    PacketToMove = Adapter->FirstLoopBack;

    Reserved = PSONIC_RESERVED_FROM_PACKET(PacketToMove);

    if (Reserved->Next == NULL) {

        Adapter->LastLoopBack = NULL;

    }

    Adapter->FirstLoopBack = Reserved->Next;

    Open = PSONIC_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

    Adapter->CurrentLoopbackPacket = PacketToMove;

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

    //
    // Indicate the packet to every open binding
    // that could want it. Since loopback indications
    // are seralized, we store the packet here
    // and use a NULL handle to indicate that it
    // is for a loopback packet.
    //

    if (BufferLength < 14) {

        //
        // Must have at least the destination address
        //

        if (BufferLength >= ETH_LENGTH_OF_ADDRESS) {

            //
            // Runt packet
            //

            EthFilterIndicateReceive(
                Adapter->FilterDB,
                (NDIS_HANDLE)NULL,
                ((PCHAR)BufferAddress),
                BufferAddress,                  // header
                BufferLength,                   // header size
                NULL,                           // lookahead
                0,                              // lookahead size
                0                               // packet size
                );

        }

    } else {

        //
        // Copy the data if the first buffer does not hold
        // the header plus the loopback amount required.
        //
        // NOTE: We could copy less if all the bindings had
        // a shorted lookahead length set.
        //

        if ((BufferLength < SONIC_LOOPBACK_MAXIMUM+14) &&
            (BufferLength != TotalPacketLength)) {

            SonicCopyFromPacketToBuffer(
                PacketToMove,
                14,
                SONIC_LOOPBACK_MAXIMUM,
                Adapter->Loopback,
                &BufferLength
                );

            DataAddress = Adapter->Loopback;

        } else {

            DataAddress = (PUCHAR)BufferAddress + 14;
            BufferLength -= 14;

        }

        EthFilterIndicateReceive(
            Adapter->FilterDB,
            (NDIS_HANDLE)NULL,
            ((PCHAR)BufferAddress),
            BufferAddress,                  // header
            14,                             // header size
            DataAddress,                    // lookahead
            BufferLength,                   // lookahead size
            TotalPacketLength - 14          // packet size
            );

    }

    //
    // Remove the packet from the loopback queue and
    // either indicate that it is finished or put
    // it on the finishing up queue for the real transmits.
    //

    NdisCompleteSend(
            Open->NdisBindingContext,
            PacketToMove,
            ((Reserved->SuccessfulTransmit)?
             (NDIS_STATUS_SUCCESS):(NDIS_STATUS_FAILURE))
            );

    NdisAcquireSpinLock(&Adapter->Lock);

#ifdef CHECK_DUP_SENDS
    {
        VOID SonicRemovePacketFromList(PSONIC_ADAPTER, PNDIS_PACKET);
        SonicRemovePacketFromList(Adapter, PacketToMove);
    }
#endif

    //
    // We can decrement the reference count on the open
    // since it is no longer being "referenced" by the
    // packet on the loopback queue.
    //

    Open->References--;

    //
    // If there is nothing else on the loopback queue
    // then indicate that reception is "done".
    //

    if (!Adapter->FirstLoopBack) {

        //
        // Indicate to every open binding that "receives"
        // are complete.
        //

        NdisReleaseSpinLock(&Adapter->Lock);

        EthFilterIndicateReceiveComplete(Adapter->FilterDB);

        NdisAcquireSpinLock(&Adapter->Lock);

    }

}
#ifdef CHECK_FINISH_TRANS
VOID
SonicShowFinishTrans(
    IN PSONIC_ADAPTER Adapter,
    IN UINT CheckLocation,
    IN PVOID PacketPointer
    )
{
    PNDIS_PACKET CurPointer;
    PSONIC_PACKET_RESERVED Reserved;

    DbgPrint("SONIC: at %d for %lx:  %lx to %lx\n", CheckLocation, PacketPointer,
                    Adapter->FirstFinishTransmit, Adapter->LastFinishTransmit);

    CurPointer = Adapter->FirstFinishTransmit;

    while (CurPointer) {

        Reserved = PSONIC_RESERVED_FROM_PACKET(CurPointer);
        DbgPrint("%lx:   %lx ->\n", CurPointer, Reserved->Next);

        CurPointer = Reserved->Next;

    }
}
VOID
SonicCheckFinishTrans(
    IN PSONIC_ADAPTER Adapter,
    IN UINT CheckLocation,
    IN PVOID PacketPointer
    )
{
    PNDIS_PACKET CurPointer, PrevPointer;
    PSONIC_PACKET_RESERVED Reserved;
    BOOLEAN BadQueue = FALSE;

    CurPointer = Adapter->FirstFinishTransmit;
    PrevPointer = (PNDIS_PACKET)NULL;


    while (CurPointer) {

        Reserved = PSONIC_RESERVED_FROM_PACKET(CurPointer);

        PrevPointer = CurPointer;
        CurPointer = Reserved->Next;

    }

    if (Adapter->LastFinishTransmit != PrevPointer) {

        BadQueue = TRUE;

    }

    if (BadQueue) {

        SonicShowFinishTrans(Adapter, CheckLocation, PacketPointer);

    }
}
#endif  // CHECK_FINISH_TRANS

extern
VOID
SonicPutPacketOnLoopBack(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    IN BOOLEAN SuccessfulTransmit
    )

/*++

Routine Description:

    Put the packet on the adapter wide loop back list.

    NOTE: This routine assumes that the lock is held.

Arguments:

    Adapter - The adapter that contains the loop back list.

    Packet - The packet to be put on loop back.

    SuccessfulTransmit - This value should be placed in the
    reserved section.

    NOTE: If ReadyToComplete == TRUE then the packets completion status
    field will also be set TRUE.

Return Value:

    None.

--*/

{

    PSONIC_PACKET_RESERVED Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);

    if (!Adapter->FirstLoopBack) {

        Adapter->FirstLoopBack = Packet;

    } else {

        PSONIC_RESERVED_FROM_PACKET(Adapter->LastLoopBack)->Next = Packet;

    }

    Reserved->SuccessfulTransmit = SuccessfulTransmit;

    Reserved->Next = NULL;
    Adapter->LastLoopBack = Packet;

}

