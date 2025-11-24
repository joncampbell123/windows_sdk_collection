/*++

Copyright (c) 1990-1995  Microsoft Corporation

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

Environment:

    Kernel Mode - Or whatever is the equivalent.

--*/


#include <ndis.h>

#include <efilter.h>
#include <sonichrd.h>
#include <sonicsft.h>
#ifdef NDIS_WIN
    #pragma LCODE
#endif

//
// This macro will poke the sonic hardware into noticing that
// there is a packet available for transmit.
//

#define START_TRANSMIT(_Adapter) \
    SONIC_WRITE_PORT(_Adapter, SONIC_COMMAND, SONIC_CR_TRANSMIT_PACKETS)


STATIC
BOOLEAN
DeterminePacketAddressing(
    IN PNDIS_PACKET Packet
    );

STATIC
VOID
StagedAllocation(
    IN PSONIC_ADAPTER Adapter
    );

STATIC
VOID
AssignPacketToDescriptor(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    IN UINT DescriptorIndex
    );

STATIC
VOID
RelinquishPacket(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    IN UINT RingIndex
    );

STATIC
VOID
CalculatePacketConstraints(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    );

STATIC
BOOLEAN
ConstrainPacket(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    );


#ifdef CHECK_DUP_SENDS

#define PACKET_LIST_SIZE 20

PNDIS_PACKET SonicPacketList[PACKET_LIST_SIZE];
UINT SonicPacketListSize = 0;

VOID
SonicAddPacketToList(
    PSONIC_ADAPTER Adapter,
    PNDIS_PACKET NewPacket
    )
{
    INT i;

    for (i=0; i<SonicPacketListSize; i++) {

        if (SonicPacketList[i] == NewPacket) {

            DbgPrint("SONIC: dup send of %lx\n", NewPacket);

        }

    }

    SonicPacketList[SonicPacketListSize] = NewPacket;
    ++SonicPacketListSize;

}

VOID
SonicRemovePacketFromList(
    PSONIC_ADAPTER Adapter,
    PNDIS_PACKET OldPacket
    )
{
    INT i;

    for (i=0; i<SonicPacketListSize; i++) {

        if (SonicPacketList[i] == OldPacket) {

            break;

        }

    }

    if (i == SonicPacketListSize) {

        DbgPrint("SONIC: bad remove of %lx\n", OldPacket);

    } else {

        --SonicPacketListSize;
        SonicPacketList[i] = SonicPacketList[SonicPacketListSize];

    }

}
#endif  // CHECK_DUP_SENDS


extern
NDIS_STATUS
SonicSend(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    The SonicSend request instructs a MAC to transmit a packet through
    the adapter onto the medium.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to SONIC_OPEN.

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
    PSONIC_ADAPTER Adapter;


    Adapter = PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    NdisAcquireSpinLock(&Adapter->Lock);

    if (Adapter->Removed) {

        NdisReleaseSpinLock(&Adapter->Lock);

        return(NDIS_STATUS_FAILURE);

    }

    Adapter->References++;

    if (!Adapter->ResetInProgress) {

        PSONIC_OPEN Open;
        PSONIC_PACKET_RESERVED Reserved;

        Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

        if (!Open->BindingShuttingDown) {

            //
            // Increment the references on the open while we are
            // accessing it in the interface.
            //

#ifdef CHECK_DUP_SENDS
            SonicAddPacketToList(Adapter, Packet);
#endif

            Open->References++;

            //
            // Check to see if the packet should even make it out to
            // the media.  The primary reason this shouldn't *actually*
            // be sent is if the destination is equal to the source
            // address.
            //
            // If it doesn't need to be placed on the wire then we can
            // simply put it onto the loopback queue.
            //

            Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);

            ASSERT(sizeof(SONIC_PACKET_RESERVED) <=
                   sizeof(Packet->MacReserved));

            Reserved->MacBindingHandle = MacBindingHandle;

            if (DeterminePacketAddressing(Packet)) {

                //
                // The packet needs to be placed out on the wire.
                //
                //
                // Determine if and how much adapter space would need to be allocated
                // to meet hardware constraints.
                //

                CalculatePacketConstraints(
                    Adapter,
                    Packet
                    );

                //
                // Put on the send stage queue.
                //

                if (!Adapter->LastSendStagePacket) {

                    Adapter->FirstSendStagePacket = Packet;

                } else {

                    PSONIC_RESERVED_FROM_PACKET(Adapter->LastSendStagePacket)->Next = Packet;

                }

                Adapter->LastSendStagePacket = Packet;

                ++Adapter->GeneralOptional[GO_TRANSMIT_QUEUE_LENGTH - GO_ARRAY_START];

                Reserved->Next = NULL;

                //
                // Only try to push it through the send stage queue
                // if somebody else isn't already doing it and
                // there is some hope of moving some packets
                // ahead.
                //

                while ((!Adapter->AlreadyProcessingSendStage) &&
                       Adapter->FirstSendStagePacket &&
                       Adapter->SendStageOpen) {

                    SonicStagedAllocation(Adapter);

                }

            } else {

                //
                // It is a packet directed to ourselves.  Put it directly
                // on the loopback queue.
                //
                if (!(Open->ProtOptionFlags & NDIS_PROT_OPTION_NO_LOOPBACK)){
                
                    SonicPutPacketOnLoopBack(
                        Adapter,
                        Packet,
                        TRUE
                        );

                    //
                    // Tally statistics now; assume that loopback
                    // always "succeeds". These packets are always
                    // directed (to us), so add to those counts.
                    //

                    ++Adapter->GeneralMandatory[GM_TRANSMIT_GOOD];

                    ++Adapter->GeneralOptionalFrameCount[GO_DIRECTED_TRANSMITS];

                    SonicAddUlongToLargeInteger(
                        &Adapter->GeneralOptionalByteCount[GO_DIRECTED_TRANSMITS],
                        Reserved->PacketLength);
                }

            }


        } else {

            StatusToReturn = NDIS_STATUS_CLOSING;

        }

    } else {

        StatusToReturn = NDIS_STATUS_RESET_IN_PROGRESS;

    }


    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    SONIC_DO_DEFERRED(Adapter);
    return StatusToReturn;
}

STATIC
BOOLEAN
DeterminePacketAddressing(
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    Calculates the packet type for this packet. It also determines
    if this packet should go out on the wire.

Arguments:

    Packet - Packet whose source and destination addresses are tested.

Return Value:

    Returns FALSE if the source is equal to the destination.


--*/

{
    //
    // MacReserved section of the packet.
    //
    PSONIC_PACKET_RESERVED Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);

    //
    // Hold the first Ndis buffer;
    //
    PNDIS_BUFFER FirstBuffer;

    //
    // Holds the address of the data in the first buffer
    //
    PVOID BufferVirtualAddress;

    //
    // Number of bytes in the first buffer.
    //
    UINT BufferLength;

    //
    // Holds result of address check
    //
    UINT Result;

    NdisQueryPacket(
            Packet,
            NULL,
            NULL,
            &FirstBuffer,
            NULL
            );

    //
    // Get VA of first buffer
    //

    NdisQueryBuffer(
            FirstBuffer,
            &BufferVirtualAddress,
            &BufferLength
            );

    //
    // Ndis spec says that the ethernet header must be in the first
    // buffer.
    //

    ASSERT(BufferLength >= 14);

    if (ETH_IS_MULTICAST(BufferVirtualAddress)) {

        if (ETH_IS_BROADCAST(BufferVirtualAddress)) {

            Reserved->PacketType = SONIC_BROADCAST;

        } else {

            Reserved->PacketType = SONIC_MULTICAST;

        }

    } else {

        Reserved->PacketType = SONIC_DIRECTED;

        ETH_COMPARE_NETWORK_ADDRESSES(
            ((PUCHAR)BufferVirtualAddress) + ETH_LENGTH_OF_ADDRESS,
            (PUCHAR)BufferVirtualAddress,
            &Result
            );

        if (!Result) {

            Reserved->PacketType = SONIC_LOOPBACK;

        }

    }

    //
    // If the two addresses are equal then the
    // packet shouldn't go out on the wire.
    //

    return ((Reserved->PacketType == SONIC_LOOPBACK) ? FALSE : TRUE);

}

extern
VOID
SonicStagedAllocation(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine attempts to take a packet through a stage of allocation.

    NOTE : It is called with the lock held and returns with the lock held.

    NOTE : It is called with the stage open, a packet on the queue and
          no one already processing the stage.

Arguments:

    Adapter - The adapter that the packets are coming through.

Return Value:

    None.

--*/

{
    //
    // Holds whether the packet has been constrained
    // to the hardware requirements.
    //
    BOOLEAN SuitableForHardware;

    //
    // Packet to process
    //
    PNDIS_PACKET FirstPacket = Adapter->FirstSendStagePacket;

    //
    // MacReserved section of the packet.
    //
    PSONIC_PACKET_RESERVED Reserved = PSONIC_RESERVED_FROM_PACKET(FirstPacket);

    //
    // If we successfully acquire some ring entries, this
    // is the index of the first one.
    //
    UINT DescriptorIndex;

    ASSERT(Adapter->SendStageOpen &&
           !Adapter->AlreadyProcessingSendStage &&
           Adapter->FirstSendStagePacket
          );


    //
    // We look to see if there are enough ring entries.
    // If there aren't then stage will close.
    //

    if (Adapter->NumberOfAvailableDescriptors > 1) {

        DescriptorIndex = Adapter->AllocateableDescriptor - Adapter->TransmitDescriptorArea;

        if (Adapter->AllocateableDescriptor == Adapter->LastTransmitDescriptor) {

            Adapter->AllocateableDescriptor = Adapter->TransmitDescriptorArea;

        } else {

            ++(Adapter->AllocateableDescriptor);

        }

        --(Adapter->NumberOfAvailableDescriptors);

        if (Reserved->UsedSonicBuffer == TRUE) {

            //
            // ConstrainPacket returns FALSE if an adapter buffer
            // is needed and none is available such that the
            // spinlock will not have been release. It will also close
            // stage in that case.
            //
            // If it did use an adapter buffer it will release the spinlock
            // to do the copy to the buffer.
            //

            Adapter->AlreadyProcessingSendStage = TRUE;

            SuitableForHardware = ConstrainPacket(
                                      Adapter,
                                      FirstPacket
                                      );

            Adapter->AlreadyProcessingSendStage = FALSE;

            if (!SuitableForHardware) {

                //
                // Return transmit descriptor
                //

                Adapter->AllocateableDescriptor = Adapter->TransmitDescriptorArea +
                                                  DescriptorIndex;


                ++(Adapter->NumberOfAvailableDescriptors);

                return;


            }

        }

        //
        // Remove packet from wait queue and put it on transmit complete queue.
        //

        if (Adapter->LastSendStagePacket == FirstPacket) {

            Adapter->FirstSendStagePacket = NULL;
            Adapter->LastSendStagePacket = NULL;

        } else {

            Adapter->FirstSendStagePacket = Reserved->Next;

        }

        if (Adapter->FirstFinishTransmit == NULL) {

            Adapter->FirstFinishTransmit = FirstPacket;

        } else {

            PSONIC_RESERVED_FROM_PACKET(Adapter->LastFinishTransmit)->Next = FirstPacket;

        }

        Adapter->LastFinishTransmit = FirstPacket;

        Reserved->Next = NULL;

        --Adapter->GeneralOptional[GO_TRANSMIT_QUEUE_LENGTH - GO_ARRAY_START];

        //
        // We have the number of buffers that we need.
        // We assign all of the buffers to the ring entries.
        //

        AssignPacketToDescriptor(
            Adapter,
            FirstPacket,
            DescriptorIndex
            );

        RelinquishPacket(
            Adapter,
            FirstPacket,
            DescriptorIndex
            );

    } else {

        Adapter->SendStageOpen = FALSE;

    }

}

STATIC
BOOLEAN
ConstrainPacket(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    Given a packet and necessary attempt to acquire adapter
    buffer resources so that the packet meets sonic hardware
    contraints. If a buffer is needed and is not available then
    stage is closed.

    The constraints are that the packet must have SONIC_MAX_FRAGMENTS
    or fewer physical pieces and no piece may be less than
    SONIC_MIN_PIECE_SIZE bytes. The first constraint is based on
    the size of the SONIC_TRANSMIT_DESCRIPTOR, and the second
    is to prevent underflow in the Silo.

    If a packet violates either of the constraints then it
    will be copied in its entirety into an adapter buffer.

    NOTE: Called with lock held!!

Arguments:

    Adapter - The adapter the packet is coming through.

    Packet - The packet whose buffers are to be constrained.
             The packet reserved section is filled with information
             detailing how the packet needs to be adjusted.

Return Value:

    Returns TRUE if the packet is suitable for the hardware.

--*/

{

    //
    // Pointer to the reserved section of the packet to be contrained.
    //
    PSONIC_PACKET_RESERVED Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);

    //
    // Holds the adapter buffer index available for allocation.
    //
    INT SonicBuffersIndex;

    //
    // Points to a successfully allocated adapter buffer descriptor.
    //
    PSONIC_BUFFER_DESCRIPTOR BufferDescriptor;

    //
    // Will point into the virtual address space addressed
    // by the adapter buffer if one was successfully allocated.
    //
    PCHAR CurrentDestination;

    //
    // Will hold the total amount of data copied to the
    // adapter buffer.
    //
    UINT TotalDataMoved = 0;

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

    //
    // Simple iteration variable.
    //
    INT i;

    if (Reserved->PacketLength <= SONIC_SMALL_BUFFER_SIZE) {

        i = 1;

    } else if (Reserved->PacketLength <= SONIC_MEDIUM_BUFFER_SIZE) {

        i = 2;

    } else {

        i = 3;

    }


    for (
        ;
        i <= 3;
        i++
        ) {

        if ((SonicBuffersIndex = Adapter->SonicBufferListHeads[i]) != -1) {

            BufferDescriptor = Adapter->SonicBuffers + SonicBuffersIndex;
            Adapter->SonicBufferListHeads[i] = BufferDescriptor->Next;
            break;

        }

    }

    if (SonicBuffersIndex == -1) {

        //
        // Nothing available for the packet.
        //

        Adapter->SendStageOpen = FALSE;

        return FALSE;

    }

    NdisReleaseSpinLock(&Adapter->Lock);

    //
    // Save the list head index in the buffer descriptor
    // to permit easy deallocation later.
    //

    BufferDescriptor->Next = i;

    //
    // Fill in the adapter buffer with the data from the users
    // buffers.
    //

    CurrentDestination = BufferDescriptor->VirtualSonicBuffer;

    NdisQueryPacket(
        Packet,
        NULL,
        NULL,
        &SourceBuffer,
        NULL
        );

    while (SourceBuffer) {

        NdisQueryBuffer(
            SourceBuffer,
            &SourceData,
            &SourceLength
            );

        SONIC_MOVE_MEMORY(
            CurrentDestination,
            SourceData,
            SourceLength
            );

        CurrentDestination = (PCHAR)CurrentDestination + SourceLength;

        TotalDataMoved += SourceLength;

        NdisGetNextBuffer(
            SourceBuffer,
            &SourceBuffer
        );

    }

    //
    // If the packet is less then the minimum size then we
    // need to zero out the rest of the packet.
    //

    if (TotalDataMoved < SONIC_MIN_PACKET_SIZE) {

        SONIC_ZERO_MEMORY(
            CurrentDestination,
            SONIC_MIN_PACKET_SIZE - TotalDataMoved
            );

        BufferDescriptor->DataLength = SONIC_MIN_PACKET_SIZE;

    } else {

        BufferDescriptor->DataLength = TotalDataMoved;

    }

    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // We need to save in the packet which adapter buffer descriptor
    // it is using so that we can deallocate it later.
    //

    Reserved->SonicBuffersIndex = SonicBuffersIndex;

    return TRUE;
}

STATIC
VOID
CalculatePacketConstraints(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    Given a packet calculate how the packet will have to be
    adjusted to meet with hardware constraints.

    The constraints are that the packet must have SONIC_MAX_FRAGMENTS
    or fewer physical pieces and no piece may be less than
    SONIC_MIN_FRAGMENT_SIZE bytes. The first constraint is based on
    the size of the SONIC_TRANSMIT_DESCRIPTOR, and the second
    is to prevent underflow in the Silo.

    If the packet is found to violate the constraints, then
    UsedSonicBuffer will be set to TRUE. This will cause the entire packet to
    be copied into the adapter buffer (which is guaranteed
    to be physically contiguous).

    Note: This is called with the lock held!  Merely for convience to
    reduce the number of times we acquire and release the spinlock.

Arguments:

    Adapter - The adapter the packet is coming through.

    Packet - The packet whose buffers are to be reallocated.
             The packet reserved section is filled with information
             detailing how the packet needs to be adjusted.

Return Value:

    None.

--*/

{

    //
    // Points to the MacReserved portion of the packet.
    //
    PSONIC_PACKET_RESERVED Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);

    //
    // The number of physical buffers in the entire packet.
    //
    UINT PacketPhysicalSegments;

    //
    // Points to the current ndis buffer being walked.
    //
    PNDIS_BUFFER CurrentBuffer;

    //
    // The virtual address of the current ndis buffer.
    //
    PVOID BufferVirtualAddress;

    //
    // The length in bytes of the current ndis buffer.
    //
    UINT BufferVirtualLength;

    //
    // The total amount of data contained within the ndis packet.
    //
    UINT PacketVirtualLength;

    //
    // The number of physical buffers in a single buffer.
    //
    UINT BufferPhysicalSegments;

    //
    // TRUE once we find a constraint violation
    //
    BOOLEAN ViolatedConstraints = FALSE;

#ifndef NO_CHIP_FIXUP
    //
    // Used to keep track of the total number of fragments
    // that the packet will occupy when assigned to a
    // transmit descriptor (may be more than PacketPhysicalSegments
    // if we have to worry about packets starting or ending
    // on non-longword boundaries.
    //
    UINT TotalTransmitSegments = 0;
#endif



    //
    // Get the first buffer in the packet.
    //

    NdisQueryPacket(
        Packet,
        &PacketPhysicalSegments,
        NULL,
        &CurrentBuffer,
        &PacketVirtualLength
        );

    //
    // Save this value for later
    //

    Reserved->PacketLength = PacketVirtualLength;

    //
    // We only allow SONIC_MAX_FRAGMENTS physical pieces.
    //

    if (PacketPhysicalSegments > SONIC_MAX_FRAGMENTS) {
        ViolatedConstraints = TRUE;
        goto DoneExamining;
    }

    //
    // For short packets we can only allow SONIC_MAX_FRAGMENTS-1
    // (to allow for the blank padding buffer). Also, we can't
    // allow the padding itself to be less than the minimum
    // fragment size.
    //

    if (PacketVirtualLength < SONIC_MIN_PACKET_SIZE  &&
            ((PacketPhysicalSegments > (SONIC_MAX_FRAGMENTS-1)) ||
             (PacketVirtualLength >
                    (SONIC_MIN_PACKET_SIZE - SONIC_MIN_FRAGMENT_SIZE)))) {
        ViolatedConstraints = TRUE;
        goto DoneExamining;
    }


    //
    // Now loop making sure no fragment is less than
    // SONIC_MIN_FRAGMENT_SIZE bytes.
    //

    while (CurrentBuffer) {

        NdisQueryBuffer(
            CurrentBuffer,
            &BufferVirtualAddress,
            &BufferVirtualLength
            );


        //
        // See if there is only one piece in the buffer.
        //

        NdisGetBufferPhysicalArraySize(
            CurrentBuffer,
            &BufferPhysicalSegments
            );

        if (BufferPhysicalSegments == 1) {

            //
            // Only one piece, make sure it is large enough.
            //

            if (BufferVirtualLength < SONIC_MIN_FRAGMENT_SIZE) {
                ViolatedConstraints = TRUE;
                goto DoneExamining;
            }

#ifndef NO_CHIP_FIXUP

            //
            // See if the beginning AND end of this piece are
            // not longword-aligned.
            //

            if (((ULONG)BufferVirtualAddress & 0x03)  &&
                (((ULONG)BufferVirtualAddress + BufferVirtualLength) & 0x03)) {

                //
                // Now see if this piece is large enough to
                // be split into two.
                //

                if (BufferVirtualLength >
                    (UINT)(4 - ((ULONG)BufferVirtualAddress & 0x03) +
                        (2*SONIC_MIN_FRAGMENT_SIZE))) {

                    //
                    // Have enough to let the first fragment be
                    // SONIC_MIN_FRAGMENT_SIZE plus the extra
                    // few bytes at the beginning, and the
                    // second piece SONIC_MIN_FRAGMENT_SIZE.
                    //

                    TotalTransmitSegments += 2;

                } else {

                    ViolatedConstraints = TRUE;
                    goto DoneExamining;

                }

            } else {

                //
                // This piece won't have to be split, so
                // just count it as one.
                //

                TotalTransmitSegments += 1;

            }

#endif

        } else {

            //
            // Multiple pieces. We assume that the relevant low bits
            // will be the same in a physical and virtual address, so
            // we can check using the virtual address whether a
            // physical segment may be too short (we are being over-
            // cautious here, but this allows us to avoid actually
            // querying the physical addresses here.
            //

            //
            // See if this buffer starts less than MIN_FRAGMENT_SIZE
            // bytes before a page boundary.
            //

            if (PAGE_SIZE - ((ULONG)BufferVirtualAddress & (PAGE_SIZE-1)) <
                        SONIC_MIN_FRAGMENT_SIZE) {
                ViolatedConstraints = TRUE;
                goto DoneExamining;
            }

            //
            // See if this buffer ends less than MIN_FRAGMENT_SIZE
            // bytes after a page boundary.
            //

            if (((ULONG)BufferVirtualAddress + BufferVirtualLength) & (PAGE_SIZE-1) <
                    SONIC_MIN_FRAGMENT_SIZE) {
                ViolatedConstraints = TRUE;
                goto DoneExamining;
            }

#ifndef NO_CHIP_FIXUP

            //
            // Add the number of fragments in this piece.
            // We assume that physical gaps will always be
            // on at least a 4 byte boundary, so we won't
            // need to split this piece.
            //

            TotalTransmitSegments += BufferPhysicalSegments;

#endif

        }


        NdisGetNextBuffer(
            CurrentBuffer,
            &CurrentBuffer
            );

    }


#ifndef NO_CHIP_FIXUP

    //
    // If the packet is short, we have to allow for the
    // padding fragment at the end.
    //

    if (PacketVirtualLength < SONIC_MIN_PACKET_SIZE) {

        TotalTransmitSegments += 1;

    }

#endif


DoneExamining: ;

#ifndef NO_CHIP_FIXUP
    if (ViolatedConstraints || (TotalTransmitSegments > SONIC_MAX_FRAGMENTS)) {
#else
    if (ViolatedConstraints) {
#endif

        Reserved->UsedSonicBuffer = TRUE;

    } else {

        Reserved->UsedSonicBuffer = FALSE;

    }

}

STATIC
VOID
AssignPacketToDescriptor(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    IN UINT DescriptorIndex
    )

/*++

Routine Description:

    Given a packet and a ring index, assign all of the buffers
    in the packet to ring entries.

    NOTE : Called with lock held!!

Arguments:

    Adapter - The adapter that the packets are coming through.

    Packet - The packet whose buffers are to be assigned
    ring entries.

    DescriptorIndex - The index of the start of the ring entries to
    be assigned buffers.

Return Value:

    None.

--*/

{

    //
    // Points to the reserved portion of the packet.
    //
    PSONIC_PACKET_RESERVED Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);

    //
    // Pointer to the ring entry to be filled with buffer information.
    //
    PSONIC_TRANSMIT_DESCRIPTOR TransmitDescriptor = Adapter->TransmitDescriptorArea
                                               + DescriptorIndex;

    //
    // Pointer to the ring to packet entry that records the info about
    // this packet.
    //
    PSONIC_DESCRIPTOR_TO_PACKET DescriptorToPacket = Adapter->DescriptorToPacket + DescriptorIndex;

    //
    // The total amount of data in the ndis packet.
    //
    UINT TotalDataLength;

    //
    // Points to the current ndis buffer being walked.
    //
    PNDIS_BUFFER CurrentBuffer;

    //
    // The number of physical segments in this buffer.
    //
    UINT BufferPhysicalSegments;

    //
    // An array to hold the physical segments.
    //
    NDIS_PHYSICAL_ADDRESS_UNIT PhysicalSegmentArray[SONIC_MAX_FRAGMENTS];

    //
    // We record the owning packet information in the ring packet packet
    // structure.
    //


    DescriptorToPacket->OwningPacket = Packet;
    DescriptorToPacket->UsedSonicBuffer = (BOOLEAN)
                                Reserved->UsedSonicBuffer;
    DescriptorToPacket->SonicBuffersIndex =
                                Reserved->SonicBuffersIndex;


    //
    // First initialize the fields that don't depend on
    // how many fragments there are in the packet.
    //

    TransmitDescriptor->TransmitStatus = 0;

    //
    // Set the programmable interrupt if it has been a long
    // time since transmit complete interrupts were processed.
    //

    if (Adapter->PacketsSinceLastInterrupt >=
            (SONIC_NUMBER_OF_TRANSMIT_DESCRIPTORS/2)) {

        TransmitDescriptor->TransmitConfiguration = (UINT)SONIC_TCR_PROG_INTERRUPT;
        Adapter->PacketsSinceLastInterrupt = 0;

    } else {

        TransmitDescriptor->TransmitConfiguration = 0;
        ++Adapter->PacketsSinceLastInterrupt;

    }


    //
    // Now check to see if the packet has been copied into an
    // adapter buffer.
    //

    if (Reserved->UsedSonicBuffer) {

        //
        // Points to the adapter buffer descriptor allocated
        // for this packet.
        //
        PSONIC_BUFFER_DESCRIPTOR BufferDescriptor;

        BufferDescriptor = Adapter->SonicBuffers
                           + Reserved->SonicBuffersIndex;

        TransmitDescriptor->FragmentCount = 1;
        TransmitDescriptor->PacketSize = (UINT)BufferDescriptor->DataLength;

        SONIC_SET_TRANSMIT_FRAGMENT_ADDRESS(
            &(TransmitDescriptor->Fragments[0]),
            NdisGetPhysicalAddressLow(BufferDescriptor->PhysicalSonicBuffer)
            );

        SONIC_SET_TRANSMIT_FRAGMENT_LENGTH(
            &(TransmitDescriptor->Fragments[0]),
            BufferDescriptor->DataLength
            );


        //
        // This sets end-of-list for this descriptor.
        //

        SONIC_SET_TRANSMIT_LINK(
            &(TransmitDescriptor->Fragments[1]),
            TransmitDescriptor->Link
            );

        DescriptorToPacket->LinkPointer = (SONIC_PHYSICAL_ADDRESS *)
                            &(TransmitDescriptor->Fragments[1]);


        //
        // Flush the buffer that contains the packet.
        //

        SONIC_FLUSH_WRITE_BUFFER(BufferDescriptor->FlushBuffer);

    } else {

        //
        // The total length of the packet (including padding)
        //
        UINT TotalPacketLength;

        //
        // Which fragment we are filling;
        //
        UINT CurFragment;

        //
        // Which map register we use for this buffer.
        //
        UINT CurMapRegister;

        //
        // Simple iteration variable.
        //
        UINT i;


        CurFragment = 0;
        CurMapRegister = DescriptorIndex * SONIC_MAX_FRAGMENTS;

        NdisQueryPacket(
            Packet,
            NULL,
            NULL,
            &CurrentBuffer,
            &TotalDataLength
            );


        while (CurrentBuffer) {

            NdisStartBufferPhysicalMapping(
                Adapter->NdisAdapterHandle,
                CurrentBuffer,
                CurMapRegister,
                TRUE,
                PhysicalSegmentArray,
                &BufferPhysicalSegments
                );

            ++CurMapRegister;

            //
            // Put the physical segments for this buffer into
            // the transmit descriptors.
            //

            for (i=0; i<BufferPhysicalSegments; i++) {

                ASSERT (NdisGetPhysicalAddressHigh(PhysicalSegmentArray[i].PhysicalAddress) == 0);

                SONIC_SET_TRANSMIT_FRAGMENT_ADDRESS(
                    &(TransmitDescriptor->Fragments[CurFragment]),
                    NdisGetPhysicalAddressLow(PhysicalSegmentArray[i].PhysicalAddress)
                    );

                SONIC_SET_TRANSMIT_FRAGMENT_LENGTH(
                    &(TransmitDescriptor->Fragments[CurFragment]),
                    PhysicalSegmentArray[i].Length
                    );

                ++CurFragment;

#ifndef NO_CHIP_FIXUP

                //
                // If the fragment starts and ends not on a longword
                // boundary, split it into two fragments, the first
                // being SONIC_MIN_FRAGMENT_SIZE plus the extra bits
                // at the beginning, the other the rest.
                //

                if ((NdisGetPhysicalAddressLow(PhysicalSegmentArray[i].PhysicalAddress) & 0x03) &&
                    ((NdisGetPhysicalAddressLow(PhysicalSegmentArray[i].PhysicalAddress) + PhysicalSegmentArray[i].Length) & 0x03)) {

                    UINT FirstSegmentLength;

                    FirstSegmentLength = (4 - ((NdisGetPhysicalAddressLow(PhysicalSegmentArray[i].PhysicalAddress) & 0x03))) +
                                            SONIC_MIN_FRAGMENT_SIZE;

                    SONIC_SET_TRANSMIT_FRAGMENT_LENGTH(
                        &(TransmitDescriptor->Fragments[CurFragment-1]),
                        FirstSegmentLength
                        );

                    SONIC_SET_TRANSMIT_FRAGMENT_ADDRESS(
                        &(TransmitDescriptor->Fragments[CurFragment]),
                        SONIC_GET_TRANSMIT_FRAGMENT_ADDRESS(
                            &(TransmitDescriptor->Fragments[CurFragment-1])) +
                                                    FirstSegmentLength
                        );

                    SONIC_SET_TRANSMIT_FRAGMENT_LENGTH(
                        &(TransmitDescriptor->Fragments[CurFragment]),
                        PhysicalSegmentArray[i].Length - FirstSegmentLength
                        );

                    ++CurFragment;

                }
#endif
            }


            SONIC_FLUSH_WRITE_BUFFER (CurrentBuffer);

            NdisGetNextBuffer(
                CurrentBuffer,
                &CurrentBuffer
                );

        }

        if (TotalDataLength < SONIC_MIN_PACKET_SIZE) {

            SONIC_SET_TRANSMIT_FRAGMENT_ADDRESS(
                &(TransmitDescriptor->Fragments[CurFragment]),
                NdisGetPhysicalAddressLow(Adapter->BlankBufferAddress)
                );

            SONIC_SET_TRANSMIT_FRAGMENT_LENGTH(
                &(TransmitDescriptor->Fragments[CurFragment]),
                SONIC_MIN_PACKET_SIZE - TotalDataLength
                );

            //
            // Note that BlankBuffer has already been flushed.
            //

            ++CurFragment;

            TotalPacketLength = SONIC_MIN_PACKET_SIZE;

        } else {

            TotalPacketLength = TotalDataLength;

        }

        //
        // Make sure we didn't mess up and use up too
        // many fragments.
        //
        ASSERT(CurFragment <= SONIC_MAX_FRAGMENTS);

        TransmitDescriptor->FragmentCount = (UINT)CurFragment;
        TransmitDescriptor->PacketSize = (UINT)TotalPacketLength;


        //
        // This sets end-of-list for this descriptor.
        //

        SONIC_SET_TRANSMIT_LINK(
            &(TransmitDescriptor->Fragments[CurFragment]),
            TransmitDescriptor->Link
            );

        DescriptorToPacket->LinkPointer = (SONIC_PHYSICAL_ADDRESS *)
                            &(TransmitDescriptor->Fragments[CurFragment]);

    }

    if (DescriptorIndex == (Adapter->NumberOfTransmitDescriptors-1)) {

        Adapter->DescriptorToPacket->PrevLinkPointer = DescriptorToPacket->LinkPointer;

    } else {

        (DescriptorToPacket+1)->PrevLinkPointer = DescriptorToPacket->LinkPointer;

    }

    Reserved->DescriptorIndex = DescriptorIndex;

}

STATIC
VOID
RelinquishPacket(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    IN UINT RingIndex
    )

/*++

Routine Description:

    Relinquish the ring entries owned by the packet to the chip.
    We also update the first uncommitted ring pointer.

    NOTE: Called with the lock held!!

Arguments:

    Adapter - The adapter that points to the ring entry structures.

    Packet - The packet contains the ring index of the ring
    entry for the packet.

    RingIndex - Holds the index of the ring entry used
    by this packet.

Return Value:

    None.

--*/

{

    //
    // Holds the previous link pointer, where we turn off
    // end-of-list.
    //

    PSONIC_PHYSICAL_ADDRESS PrevLinkPointer;

#ifdef NDIS_NT

    //
    // NOTE: We have to raise the IRQL to POWER_LEVEL around the
    // calls to SONIC_REMOVE_END_OF_LIST and START_TRANSMIT.
    // This is to prevent a delay between these two instructions.
    // If a delay happens right after SONIC_REMOVE_END_OF_LIST, the
    // Sonic could transmit the packet and stop, then the call
    // to START_TRANSMIT would cause it to retransmit all the
    // packets in the descriptor ring.
    //
    KIRQL OldIrql;

#endif


    PrevLinkPointer = Adapter->DescriptorToPacket[RingIndex].PrevLinkPointer;

#ifdef NDIS_NT

    //
    // See NOTE above.
    //

    KeRaiseIrql(POWER_LEVEL, &OldIrql);

#endif


    //
    // Turn off END_OF_LIST for the last one.
    //

    SONIC_REMOVE_END_OF_LIST(PrevLinkPointer);

    //
    // This turns on the correct bit in the SONIC_CONTROL
    // register.
    //

    START_TRANSMIT(Adapter);


#ifdef NDIS_NT

    //
    // See NOTE above.
    //

    KeLowerIrql(OldIrql);

#endif

    //
    // We want FirstUncommittedDescriptor to point to right after us.
    //

    if (RingIndex == (Adapter->NumberOfTransmitDescriptors-1)) {

        Adapter->FirstUncommittedDescriptor = Adapter->TransmitDescriptorArea;

    } else {

        Adapter->FirstUncommittedDescriptor =
                    Adapter->TransmitDescriptorArea + (RingIndex + 1);

    }

}


