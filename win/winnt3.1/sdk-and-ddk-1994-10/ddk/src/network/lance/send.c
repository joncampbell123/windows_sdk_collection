/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    send.c

Abstract:

    This file contains the code for putting a packet through the
    staged allocation for transmission.

    This is a process of

    1) Calculating the what would need to be done to the
    packet so that the packet can be transmitted on the hardware.

    2) Potentially allocating adapter buffers and copying user data
    to those buffers so that the packet data is transmitted under
    the hardware constraints.

    3) Allocating enough hardware ring entries so that the packet
    can be transmitted.

    4) Relinquish thos ring entries to the hardware.

Author:

    Anthony V. Ercolano (Tonye) 12-Sept-1990

Environment:

    Kernel Mode - Or whatever is the equivalent on OS/2 and DOS.

Revision History:

    31-Jul-1992  R.D. Lanser:

       Removed PhysicalBuffersContained and UsedLanceBuffer field from
       _ADAPTER structure.  SeanSe says that the code related to this
       field was used for adevice that is no longer supported.  I removed
       the dependent code(or at least what was obvious) from 'send.c'.
       This old code was generating an erroneous ring buffer count on the
       MIPS R3000.  I did not test it on the MIPS R4000.  The problem goes
       away with the removal of the offending code.

--*/

#include <ndis.h>
#include <efilter.h>
#include <lancehrd.h>
#include <lancesft.h>


//
// Minimum packet size that a transport can send.  We subtract 4 bytes
// because we add a 4 byte CRC on the end.
//

#define MIN_SINGLE_BUFFER ((UINT)LANCE_SMALL_BUFFER_SIZE - 4)


//
// It will poke the lance hardware into noticing that there is a packet
// available for transmit.
//
// Note that there is the assumption that the register address
// port (RAP) is already set to zero.
//
#define PROD_TRANSMIT(A) \
    LANCE_WRITE_RDP( \
    A, \
    LANCE_CSR0_TRANSMIT_DEMAND | LANCE_CSR0_INTERRUPT_ENABLE \
    );

VOID
SetupAllocate(
    IN PLANCE_ADAPTER Adapter,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );

VOID
StagedAllocation(
    IN PLANCE_ADAPTER Adapter
    );

VOID
CopyPacketIntoBuffer(
    IN PLANCE_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    IN PLANCE_BUFFER_DESCRIPTOR BufferDescriptor
    );



extern
NDIS_STATUS
LanceSend(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    The LanceSend request instructs a MAC to transmit a packet through
    the adapter onto the medium.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to LANCE_OPEN.

    Packet - A pointer to a descriptor for the packet that is to be
    transmitted.

Return Value:

    The function value is the status of the operation.


--*/

{

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_PENDING;

    //
    // Pointer to the adapter.
    //
    PLANCE_ADAPTER Adapter;

    PLANCE_OPEN Open;

    Open = PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

#if LANCE_TRACE
    DbgPrint("In LanceSend\n");
#endif

    Adapter = PLANCE_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    if (Adapter->HardwareFailure) {

        return(NDIS_STATUS_FAILURE);

    }

    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    LOG(IN_SEND);

    if (!Adapter->ResetInProgress) {

        if (!Open->BindingShuttingDown) {

            UINT TotalPacketSize;

            //
            // Increment the references on the open while we are
            // accessing it in the interface.
            //

            Open->References++;

            //
            // It is reasonable to do a quick check and fail if the packet
            // is larger than the maximum an ethernet can handle.
            //

            NdisQueryPacket(
                Packet,
                NULL,
                NULL,
                NULL,
                &TotalPacketSize
                );

            if ((!TotalPacketSize) ||
                (TotalPacketSize > LANCE_LARGE_BUFFER_SIZE)) {

                StatusToReturn = NDIS_STATUS_RESOURCES;

            } else {

                //
                // There is an assumption in the code that no pointer
                // (which are really handles) to an ndis packet will have
                // its low bit set. (Always have even byte alignment.)
                //

                ASSERT(!((UINT)Packet & 1));

                SetupAllocate(
                    Adapter,
                    MacBindingHandle,
                    Packet
                    );

                //
                // Add a reference for the pending send
                //

                Open->References++;

                //
                // Only try to push it through the stage queues
                // if somebody else isn't already doing it and
                // there is some hope of moving some packets
                // ahead.
                //

                while ((!Adapter->AlreadyProcessingStage
                       ) &&
                       (Adapter->FirstStage1Packet &&
                        Adapter->StageOpen
                       )
                      ) {

                    LanceStagedAllocation(Adapter);

                }

            }

            //
            // The interface is no longer referencing the open.
            //

            Open->References--;

        } else {

            StatusToReturn = NDIS_STATUS_CLOSING;

        }

    } else if (Adapter->ResetRequestType == NdisRequestGeneric1) {

        StatusToReturn = NDIS_STATUS_RESET_IN_PROGRESS;

    } else {

        //
        // Reset is from a SetInfo or internal call and this packet needs
        // to be pended.
        //

        SetupAllocate(
                        Adapter,
                        MacBindingHandle,
                        Packet
                        );

        //
        // Add one to reference fore pending send
        //
        Open->References++;

        StatusToReturn = NDIS_STATUS_PENDING;

    }

    LOG(OUT_SEND);

    LANCE_DO_DEFERRED(Adapter);

#if LANCE_TRACE
    DbgPrint("Out LanceSend\n");
#endif

    return StatusToReturn;
}

VOID
SetupAllocate(
    IN PLANCE_ADAPTER Adapter,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    This sets up the MAC reserved portion of the packet so that
    later allocation routines can determine what is left to be
    done in the allocation cycle.

    NOTE: This assumes it is called with the spinlock held!!

Arguments:

    Adapter - The adapter that this packet is coming through.

    MacBindingHandle - Points to the open binding structure.

    Packet - The packet that is to be transmitted.

Return Value:

    None.

--*/

{

    //
    // Points to the MAC reserved portion of this packet.  This
    // interpretation of the reserved section is only valid during
    // the allocation phase of the packet.
    //
    PLANCE_RESERVED Reserved = PLANCE_RESERVED_FROM_PACKET(Packet);


    ASSERT(sizeof(LANCE_RESERVED) <=
           sizeof(Packet->MacReserved));

    Reserved->LanceBuffersIndex = 0;
    Reserved->MacBindingHandle = MacBindingHandle;

    //
    // Put on the stage 1 queue.
    //

    if (!Adapter->LastStage1Packet) {

        Adapter->FirstStage1Packet = Packet;

    } else {

        PLANCE_RESERVED_FROM_PACKET(Adapter->LastStage1Packet)->Next = Packet;

    }

    Adapter->LastStage1Packet = Packet;

    Reserved->Next = NULL;

}

extern
VOID
LanceStagedAllocation(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine attempts to take a packet through a stage of allocation.

    NOTE: THIS IS CALLED WITH THE LOCK HELD!!

    NOTE: MUST BE CALLED WITH

        Adapter->StageOpen &&
        !Adapter->AlreadyProcessingStage &&
        Adapter->FirstStage1Packet



Arguments:

    Adapter - The adapter that the packets are coming through.

Return Value:

    None.

--*/

{
    //
    // Pointer to the ring entry to be filled with buffer information.
    //
    PLANCE_TRANSMIT_ENTRY CurrentRingElement;

    //
    // Pointer to the ring to packet entry that records the info about
    // this packet.
    //
    PLANCE_RING_TO_PACKET RingToPacket;

    //
    // Length of the packet
    //
    ULONG TotalVirtualLength;

    //
    // Holds the adapter buffer index available for allocation.
    //
    INT LanceBuffersIndex;

    //
    // Points to a successfully allocated adapter buffer descriptor.
    //
    PLANCE_BUFFER_DESCRIPTOR BufferDescriptor;

    //
    // Simple iteration variable.
    //
    INT i;

    //
    // Size of Lance Buffer needed (1==Small, 2==Medium, 3==Large)
    //
    UCHAR BufferSize;

    //
    // If we successfully acquire some ring entries, this
    // is the index of the first one.
    //
    UINT RingIndex;

    PNDIS_PACKET FirstPacket;

    PLANCE_RESERVED Reserved;


    if (Adapter->NumberOfAvailableRings == 0) {

        Adapter->StageOpen = FALSE;
        return ;

    }

    Adapter->AlreadyProcessingStage = TRUE;

    FirstPacket = Adapter->FirstStage1Packet;

    //
    // Determine if and how much adapter space would need to be allocated
    // to meet hardware constraints.
    //

    NdisQueryPacket(
        FirstPacket,
        NULL,
        NULL,
        NULL,
        &TotalVirtualLength
        );

    //
    // Certain hardware implementation (Decstation) use a dual ported
    // memory to communicate with the hardware.  This is reasonable since
    // it reduces bus contention.  When using the dual ported memory, all
    // send data must be moved to buffers allocated from the dual ported
    // memory.
    //

    if (TotalVirtualLength <= LANCE_SMALL_BUFFER_SIZE) {

        BufferSize = 1;

    } else if (TotalVirtualLength <= LANCE_MEDIUM_BUFFER_SIZE) {

        BufferSize = 2;

    } else {

        BufferSize = 3;

    }

    //
    // Find a buffer
    //
    for (
        i = BufferSize;
        i <= 3;
        i++
        ) {

        if ((LanceBuffersIndex = Adapter->LanceBufferListHeads[i]) != -1) {

            BufferDescriptor = Adapter->LanceBuffers + LanceBuffersIndex;
            Adapter->LanceBufferListHeads[i] = BufferDescriptor->Next;
            break;

        }

    }

    if (LanceBuffersIndex == -1) {

        //
        // Nothing available for the packet.
        //

        Adapter->StageOpen = FALSE;
        Adapter->AlreadyProcessingStage = FALSE;

        return;

    }

    //
    // We need to save in the packet which adapter buffere descriptor
    // it is using so that we can deallocate it later.  We also store
    // the number of buffers contained so that we can allocate ring
    // entries.
    //

    Reserved = PLANCE_RESERVED_FROM_PACKET(FirstPacket);

    Reserved->LanceBuffersIndex = LanceBuffersIndex;

    //
    // Now remove this packet from the queue
    //

    Adapter->FirstStage1Packet = Reserved->Next;

    if (Adapter->FirstStage1Packet == NULL) {

        Adapter->LastStage1Packet = NULL;

    }

    //
    // Save the list head index in the buffer descriptor
    // to permit easy deallocation later.
    //
    BufferDescriptor->Next = i;

    //
    // Now Acquire the ring
    //
    RingIndex = Adapter->AllocateableRing - Adapter->TransmitRing;

    //
    // Store the info
    //
    CurrentRingElement = Adapter->AllocateableRing;

    //
    // NOTE NOTE NOTE NOTE NOTE NOTE
    //
    // We can do the next calculation because we know that the number
    // or ring entries is a power of two!
    //

    Adapter->AllocateableRing = Adapter->TransmitRing +
            (((RingIndex) + 1) &
             (Adapter->NumberOfTransmitRings-1));

    Adapter->NumberOfAvailableRings--;

    //
    // Copy into buffer
    //
    CopyPacketIntoBuffer(Adapter, FirstPacket, BufferDescriptor);

    //
    // Get the position for mapping ring entries to packets.
    // We record the owning packet information in the ring packet packet
    // structure.
    //
    RingToPacket = Adapter->RingToPacket + RingIndex;
    RingToPacket->OwningPacket = FirstPacket;
    RingToPacket->LanceBuffersIndex = Reserved->LanceBuffersIndex;
    RingToPacket->RingIndex = RingIndex;

    //
    // Make sure that the ring descriptor is clean.
    //

    LANCE_ZERO_MEMORY_FOR_HARDWARE(CurrentRingElement,sizeof(LANCE_TRANSMIT_ENTRY));

    LANCE_SET_TRANSMIT_BUFFER_LENGTH(
        CurrentRingElement,
        BufferDescriptor->DataLength
        );


    LANCE_SET_TRANSMIT_BUFFER_ADDRESS(
            Adapter,
            CurrentRingElement,
            BufferDescriptor->VirtualLanceBuffer
            );

    LOG(TRANSMIT);

    //
    // We update the ring ownership of the last packet under
    // the protection of the lock so that the uncommitted packet
    // pointer can be updated before the transmit post processing
    // can examine it.
    //

    LANCE_SET_RING_BITS(
      CurrentRingElement->TransmitSummaryBits,
      LANCE_TRANSMIT_START_OF_PACKET |
      LANCE_TRANSMIT_OWNED_BY_CHIP |
      LANCE_TRANSMIT_END_OF_PACKET
      );


    if (RingIndex == (Adapter->NumberOfTransmitRings-1)) {

        Adapter->FirstUncommittedRing = Adapter->TransmitRing ;

    } else {

        Adapter->FirstUncommittedRing = Adapter->TransmitRing + RingIndex + 1;

    }

    //
    // Prod the chip into checking for packets to send.
    //

    PROD_TRANSMIT(Adapter);

    if (Adapter->LastFinishTransmit) {

        PLANCE_RESERVED LastReserved =
           PLANCE_RESERVED_FROM_PACKET(Adapter->LastFinishTransmit);

        LastReserved->Next = FirstPacket;

    }

    Reserved->Next = NULL;

    Adapter->LastFinishTransmit = FirstPacket;

    if (!Adapter->FirstFinishTransmit) {

        Adapter->FirstFinishTransmit = FirstPacket;

    }

    Adapter->AlreadyProcessingStage = FALSE;

}


VOID
CopyPacketIntoBuffer(
    IN PLANCE_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    IN PLANCE_BUFFER_DESCRIPTOR BufferDescriptor
    )

/*++

Routine Description:

    Copy a packet down to the hardware.

    Note : MUST BE CALLED WITH LOCK HELD!

Arguments:

    Adapter - The adapter the packet is coming through.

    Packet - The packet whose buffers are to be copied.

Return Value:

    none

--*/

{
    //
    // Will point into the virtual address space addressed
    // by the adapter buffer if one was successfully allocated.
    //
    PCHAR CurrentDestination;

    //
    // Will hold the total amount of data copied to the
    // adapter buffer.
    //
    UINT TotalVirtualLength;

    //
    // Will point to the current source buffer.
    //
    PNDIS_BUFFER SourceBuffer;

    //
    // Points to the virtual address of the source buffers data.
    //
    PVOID SourceData;

    //
    // Will point to the number of bytes of data in the source
    // buffer.
    //
    UINT SourceLength;

    NdisReleaseSpinLock(&Adapter->Lock);

    //
    // Fill in the adapter buffer with the data from the users
    // buffers.
    //

    CurrentDestination = BufferDescriptor->VirtualLanceBuffer;

    NdisQueryPacket(
        Packet,
        NULL,
        NULL,
        &SourceBuffer,
        &TotalVirtualLength
        );

    while ( SourceBuffer != NULL ) {

        NdisQueryBuffer(
                SourceBuffer,
                &SourceData,
                &SourceLength
                );

        LANCE_MOVE_MEMORY_TO_HARDWARE(
            CurrentDestination,
            SourceData,
            SourceLength
            );

        CurrentDestination = (PCHAR)CurrentDestination + SourceLength;

        NdisGetNextBuffer(
                SourceBuffer,
                &SourceBuffer
                );

    }

    //
    // If the packet is less then the minimum size then we
    // need to zero out the rest of the packet.
    //

    if (TotalVirtualLength < MIN_SINGLE_BUFFER) {

        LANCE_ZERO_MEMORY_FOR_HARDWARE(
            CurrentDestination,
            MIN_SINGLE_BUFFER - TotalVirtualLength
            );

        BufferDescriptor->DataLength = MIN_SINGLE_BUFFER;

    } else {

        BufferDescriptor->DataLength = TotalVirtualLength;

    }

    NdisAcquireSpinLock(&Adapter->Lock);

    return;
}

