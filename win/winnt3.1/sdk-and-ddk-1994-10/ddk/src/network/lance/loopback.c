/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    loopback.c

Abstract:

    The routines here indicate packets on the loopback queue and are
    responsible for inserting and removing packets from the loopback
    queue and the send finishing queue.

Author:

    Anthony V. Ercolano (Tonye) 12-Sept-1990

Environment:

    Operates at dpc level - or the equivalent on os2 and dos.

Revision History:


--*/

#include <ndis.h>
#include <efilter.h>
#include <lancehrd.h>
#include <lancesft.h>


extern
VOID
LanceProcessLoopback(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is responsible for indicating *one* packet on
    the loopback queue either completing it or moving on to the
    finish send queue.

    NOTE: THIS IS CALLED WITH THE SPIN LOCK HELD!!

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
    PLANCE_RESERVED Reserved;

    //
    // Eventually the length of the data to be indicated
    // to the transport.
    //
    UINT BufferLength;

    //
    // Length of the packet
    //
    UINT TotalPacketLength;

    PLANCE_OPEN Open;

    PacketToMove = Adapter->FirstLoopBack;

    Adapter->IndicatingMacReceiveContext.WholeThing = (UINT)PacketToMove;

    Adapter->IndicatedAPacket = TRUE;

    Reserved = PLANCE_RESERVED_FROM_PACKET(PacketToMove);

    Open = PLANCE_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

    if (!Reserved->Next) {

        Adapter->LastLoopBack = NULL;

    }

    Adapter->FirstLoopBack = Reserved->Next;

    Adapter->IndicatingMacReceiveContext.WholeThing = (UINT)PacketToMove;

    NdisReleaseSpinLock(&Adapter->Lock);

    //
    // See if we need to copy the data from the packet
    // into the lookahead buffer.
    //
    // We need to copy to the adapter lookahead buffer if
    // the first buffer of the packet is less than the
    // minimum loopback size AND the first buffer isn't
    // the total packet.
    //

    NdisQueryPacket(
        PacketToMove,
        NULL,
        NULL,
        NULL,
        &TotalPacketLength
        );

    BufferLength = ((TotalPacketLength < (Adapter->MaxLookAhead + LANCE_HEADER_SIZE))?
                    TotalPacketLength :
                    Adapter->MaxLookAhead + LANCE_HEADER_SIZE);

    LanceCopyFromPacketToBuffer(
            PacketToMove,
            0,
            Adapter->SizeOfReceiveBuffer,
            Adapter->Lookahead,
            &BufferLength
            );

    //
    // Indicate the packet to every open binding
    // that could want it.
    //

    if (BufferLength < LANCE_HEADER_SIZE) {

        if (BufferLength >= ETH_LENGTH_OF_ADDRESS) {

            //
            // Runt packet
            //

            EthFilterIndicateReceive(
                Adapter->FilterDB,
                PacketToMove,
                ((PCHAR)(Adapter->Lookahead)),
                Adapter->Lookahead,
                BufferLength,
                NULL,
                0,
                0
                );
        }

    } else {

        EthFilterIndicateReceive(
            Adapter->FilterDB,
            PacketToMove,
            ((PCHAR)(Adapter->Lookahead)),
            Adapter->Lookahead,
            LANCE_HEADER_SIZE,
            Adapter->Lookahead + LANCE_HEADER_SIZE,
            BufferLength - LANCE_HEADER_SIZE,
            TotalPacketLength - LANCE_HEADER_SIZE
            );

    }

    NdisCompleteSend(
            Open->NdisBindingContext,
            PacketToMove,
            ((Reserved->SuccessfulTransmit)?
             (NDIS_STATUS_SUCCESS):(NDIS_STATUS_FAILURE))
            );


    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // Remove reference for packet
    //

    Open->References--;

}

