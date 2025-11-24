/*++

Copyright (c) 1993-95  Microsoft Corporation

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

    4) Relinquish those ring entries to the hardware.

Environment:

    Kernel Mode - Or whatever is the equivalent on OS/2 and DOS.

--*/

//
// So we can trace things...
//
#define STATIC

#include <ne3200sw.h>
#ifdef NDIS_WIN
    #pragma LCODE
#endif


STATIC
BOOLEAN
NE3200PacketShouldBeSent(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );

STATIC
VOID
NE3200SetupAllocate(
    IN PNE3200_ADAPTER Adapter,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );

STATIC
PNE3200_SUPER_COMMAND_BLOCK
NE3200RemovePacketFromStage3(
    IN PNE3200_ADAPTER Adapter
    );

STATIC
VOID
NE3200ConstrainPacket(
    IN PNE3200_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    );


NDIS_STATUS
NE3200Send(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    The NE3200Send request instructs a MAC to transmit a packet through
    the adapter onto the medium.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE3200_OPEN.

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
    PNE3200_ADAPTER Adapter;
    PNE3200_OPEN Open;

    Adapter = PNE3200_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);
    Open = PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);
    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    if (!Adapter->ResetInProgress &&
        !Adapter->BeingRemoved &&
        !Open->BindingShuttingDown) {

        //
        // NOTE NOTE NOTE !!!!!!
        //
        // There is an assumption in the code that no pointer
        // (which are really handles) to an ndis packet will have
        // its low bit set. (Always have even byte alignment.)
        //

        ASSERT(!((UINT)Packet & 1));

        NE3200SetupAllocate(
                Adapter,
                MacBindingHandle,
                Packet
                );

        //
        // Only try to push it through the stage queues
        // if somebody else isn't already doing it and
        // there is some hope of moving some packets
        // ahead.
        //

        while (!Adapter->AlreadyProcessingStage2
                &&
                (Adapter->FirstStage1Packet && Adapter->Stage2Open)
              ) {

            NE3200StagedAllocation(Adapter);

        }

    } else {

        if (!Adapter->BeingRemoved) {

            StatusToReturn = NDIS_STATUS_RESET_IN_PROGRESS;

        } else if (Open->BindingShuttingDown) {

            StatusToReturn = NDIS_STATUS_CLOSING;

        } else {

            StatusToReturn = NDIS_STATUS_ADAPTER_REMOVED;

        }

    }

    NE3200_DO_DEFERRED(Adapter);
    return StatusToReturn;
}

STATIC
VOID
NE3200SetupAllocate(
    IN PNE3200_ADAPTER Adapter,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    This sets up the MAC reserved portion of the packet so that
    later allocation routines can determine what is left to be
    done in the allocation cycle.

    NOTE: This must be called with the spinlock held!!

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
    PNE3200_RESERVED Reserved = PNE3200_RESERVED_FROM_PACKET(Packet);

    //
    // The number of ndis buffers in the packet.
    //
    UINT NdisBufferCount;

    //
    // The number of physical buffers in the entire packet.
    //
    UINT PhysicalBufferCount;

    //
    // The total amount of data contained within the ndis packet.
    //
    UINT TotalVirtualLength;


    ASSERT(sizeof(NE3200_RESERVED) <=
           sizeof(Packet->MacReserved));

    Reserved->STAGE.ClearStage = 0;
    Reserved->MacBindingHandle = MacBindingHandle;

    //
    // Determine if and how much adapter space would need to be allocated
    // to meet hardware constraints.
    //

    NdisQueryPacket(
        Packet,
        &PhysicalBufferCount,
        &NdisBufferCount,
        NULL,
        &TotalVirtualLength
        );


    //
    // See if the packet exceeds NE3200_MAXIMUM_BLOCKS_PER_PACKET.
    // Keep in mind that if the total virtual packet length is less than
    // MINIMUM_ETHERNET_PACKET_SIZE then we'll have to chain on an
    // additional buffer to pad the packet out to the minimum size.
    //

    if (PhysicalBufferCount > (NE3200_MAXIMUM_BLOCKS_PER_PACKET - 1)) {

        //
        // Possible merge necessary.  Now check specific conditions
        //

        if ((PhysicalBufferCount > NE3200_MAXIMUM_BLOCKS_PER_PACKET) ||
            (TotalVirtualLength < MINIMUM_ETHERNET_PACKET_SIZE)) {

            Reserved->STAGE.STAGE1.MinimumBufferRequirements = 3;
            Reserved->STAGE.STAGE1.NdisBuffersToMove = NdisBufferCount;

        }

    }

    //
    // Put on the stage 1 queue.
    //

    if (!Adapter->LastStage1Packet) {

        Adapter->FirstStage1Packet = Packet;

    } else {

        PNE3200_RESERVED_FROM_PACKET(Adapter->LastStage1Packet)->Next = Packet;

    }

    Adapter->LastStage1Packet = Packet;

    Reserved->Next = NULL;

    //
    // Increment the reference on the open since it
    // will be leaving this packet around on the transmit
    // queues.
    //

    PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)->References++;
    Adapter->TransmitsQueued++;

}

VOID
NE3200StagedAllocation(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine attempts to take a packet through a stage of allocation.

    NOTE: This routine is called with the spin lock held!!!

Arguments:

    Adapter - The adapter that the packets are coming through.

Return Value:

    None.

--*/

{
    //
    // If we successfully acquire a command block, this
    // is a pointer to it.
    //
    PNE3200_SUPER_COMMAND_BLOCK CommandBlock;

    PNDIS_PACKET FirstPacket = Adapter->FirstStage1Packet;

    //
    // Points to the reserved portion of the packet.
    //
    PNE3200_RESERVED Reserved = PNE3200_RESERVED_FROM_PACKET(FirstPacket);

    //
    // Pointer to the NE3200 data block descriptor being filled.
    //
    PNE3200_DATA_BLOCK DataBlock;

    //
    // The total amount of data in the ndis packet.
    //
    UINT TotalDataLength;

    //
    // The number of ndis buffers in the packet.
    //
    UINT NdisBufferCount;

    //
    // Points to the current ndis buffer being walked.
    //
    PNDIS_BUFFER CurrentBuffer;

    //
    // Holds whether the packet has been constrained
    // to the hardware requirements.
    //
    BOOLEAN SuitableForHardware;

    //
    // Pointer to address
    //
    PUCHAR Address;

    //
    // Check that we have a merge buffer if one will be necessary.
    //

    if ((PNE3200_RESERVED_FROM_PACKET(FirstPacket)->STAGE.STAGE1.MinimumBufferRequirements) &&
        (Adapter->NE3200BufferListHead == -1)) {

        //
        // Not enough space for the packet -- return and wait.
        //

        Adapter->Stage2Open = FALSE;

        return;

    }

    //
    // We look to see if there is an available Command Block.
    // If there isn't then stage 3 will close.
    //

    if (NE3200AcquireCommandBlock(
           Adapter,
           &CommandBlock
           )) {

        //
        // We have a command block.  Assign all packet
        // buffers to the command block.
        //

        if (PNE3200_RESERVED_FROM_PACKET(FirstPacket)->STAGE.STAGE1.MinimumBufferRequirements) {

            //
            // Now we merge the packet into a buffer
            //

            Adapter->AlreadyProcessingStage2 = TRUE;

            NE3200ConstrainPacket(Adapter, FirstPacket);

            Adapter->AlreadyProcessingStage2 = FALSE;

        }

        //
        // Get a pointer to the the first data block descriptor
        // in the Command Block.
        //

        DataBlock = &CommandBlock->Hardware.TransmitDataBlocks[0];

        //
        // We record the owning packet information in the ring packet packet
        // structure.
        //

        CommandBlock->OwningPacket = FirstPacket;
        CommandBlock->UsedNE3200Buffer =
            (BOOLEAN)Reserved->STAGE.STAGE2.UsedNE3200Buffer;
        CommandBlock->NE3200BuffersIndex = Reserved->STAGE.STAGE2.NE3200BuffersIndex;
        CommandBlock->NextCommand = NULL;
        CommandBlock->OwningOpenBinding = NULL;

        //
        // Initialize the various fields of the Command Block.
        //

        CommandBlock->Hardware.State = NE3200_STATE_WAIT_FOR_ADAPTER;
        CommandBlock->Hardware.Status = 0;
        CommandBlock->Hardware.NextPending = NE3200_NULL;
        CommandBlock->Hardware.CommandCode = NE3200_COMMAND_TRANSMIT;
        CommandBlock->Hardware.PARAMETERS.TRANSMIT.ImmediateDataLength = 0;

        //
        // Check to see if the packet needed to use an adapter buffer.
        //

        if (Reserved->STAGE.STAGE2.UsedNE3200Buffer) {

            //
            // Points to the adapter buffer descriptor allocated
            // for this packet.
            //
            PNE3200_BUFFER_DESCRIPTOR BufferDescriptor;

            BufferDescriptor = Adapter->NE3200Buffers
                               + Reserved->STAGE.STAGE2.NE3200BuffersIndex;

            Address = (PUCHAR)(BufferDescriptor->VirtualNE3200Buffer);

            //
            // Since this packet used one of the adapter buffers, the
            // following is known:
            //
            //     o  There is exactly one physical buffer for this packet.
            //     o  The buffer's length is the transmit frame size.
            //

            //
            // Set the number of data blocks and the transmit frame size.
            //

            NdisFlushBuffer(BufferDescriptor->FlushBuffer, TRUE);
            CommandBlock->Hardware.NumberOfDataBlocks = 1;
            CommandBlock->Hardware.TransmitFrameSize =
                (USHORT)BufferDescriptor->DataLength;

            //
            // Initialize the (one) data block for this transmit.
            //

            DataBlock->BlockLength = (USHORT)BufferDescriptor->DataLength;
            DataBlock->PhysicalAddress = NdisGetPhysicalAddressLow(BufferDescriptor->PhysicalNE3200Buffer);

        } else {

            //
            // Array to hold the physical segments
            //
            NDIS_PHYSICAL_ADDRESS_UNIT PhysicalSegmentArray[NE3200_MAXIMUM_BLOCKS_PER_PACKET];

            //
            // Number of physical segments in the buffer
            //
            UINT BufferPhysicalSegments;

            //
            // map register to use for this buffer
            //
            UINT CurMapRegister;

            //
            // for index
            //
            UINT i;


            //
            // Get the first buffer as well as the number of ndis buffers in
            // the packet.
            //

            NdisQueryPacket(
                FirstPacket,
                NULL,
                &NdisBufferCount,
                &CurrentBuffer,
                &TotalDataLength
                );

            CommandBlock->Hardware.NumberOfDataBlocks = 0;
            CommandBlock->Hardware.TransmitFrameSize = (USHORT)TotalDataLength;

            //
            // Set the map registers to use
            //

            CurMapRegister = CommandBlock->CommandBlockIndex *
                            NE3200_MAXIMUM_BLOCKS_PER_PACKET;


            //
            // Get address from first buffer.
            //

            NdisQueryBuffer(
                CurrentBuffer,
                (PVOID)&Address,
                &i
                );

            //
            // Go through all of the buffers in the packet getting
            // the actual physical buffers from each MDL.
            //

            while (CurrentBuffer != NULL) {

                NdisStartBufferPhysicalMapping(
                                Adapter->NdisAdapterHandle,
                                CurrentBuffer,
                                CurMapRegister,
                                TRUE,
                                PhysicalSegmentArray,
                                &BufferPhysicalSegments
                                );

                CurMapRegister++;

                //
                // Store segments into command block
                //

                for (i = 0; i < BufferPhysicalSegments ; i++, DataBlock++ ) {

                    DataBlock->BlockLength = (USHORT)PhysicalSegmentArray[i].Length;
                    DataBlock->PhysicalAddress = NdisGetPhysicalAddressLow(PhysicalSegmentArray[i].PhysicalAddress);

                }

                CommandBlock->Hardware.NumberOfDataBlocks += BufferPhysicalSegments;

                NdisFlushBuffer(CurrentBuffer, TRUE);

                NdisGetNextBuffer(
                        CurrentBuffer,
                        &CurrentBuffer
                        );

            }

            //
            // If the total packet length is less than MINIMUM_ETHERNET_PACKET_SIZE
            // then we must chain the Padding buffer onto the end and update
            // the transfer size.
            //

            if (TotalDataLength < MINIMUM_ETHERNET_PACKET_SIZE) {

                    DataBlock->BlockLength =
                        (USHORT)(MINIMUM_ETHERNET_PACKET_SIZE - TotalDataLength);

                    DataBlock->PhysicalAddress = NdisGetPhysicalAddressLow(Adapter->PaddingPhysicalAddress);

                    DataBlock++;
                    CommandBlock->Hardware.NumberOfDataBlocks++;

                    CommandBlock->Hardware.TransmitFrameSize = MINIMUM_ETHERNET_PACKET_SIZE;

            }

        }

        Reserved->STAGE.STAGE2.CommandBlockIndex = CommandBlock->CommandBlockIndex;

        NE3200SubmitCommandBlock(
            Adapter,
            CommandBlock
            );

        //
        // First remove it from the stage 1 queue;
        //

        Adapter->FirstStage1Packet =
            PNE3200_RESERVED_FROM_PACKET(FirstPacket)->Next;

        if (!Adapter->FirstStage1Packet) {

            Adapter->LastStage1Packet = NULL;

        }

        //
        // Do a quick check to see if the packet has a high likelyhood
        // of needing to loopback.  (NOTE: This means that if the packet
        // must be loopbacked then this function will return true.  If
        // the packet doesn't need to be loopbacked then the function
        // will probably return false.)
        //

        if (!(PNE3200_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle)->ProtOptionFlags
             & NDIS_PROT_OPTION_NO_LOOPBACK) 
            && EthShouldAddressLoopBack(
                Adapter->FilterDB,
                Address
                )) {

            if (!Adapter->FirstLoopBack) {

                Adapter->FirstLoopBack = FirstPacket;

            } else {

                PNE3200_RESERVED_FROM_PACKET(Adapter->LastLoopBack)->Next = FirstPacket;

            }

            Reserved->STAGE.STAGE4.ReadyToComplete = FALSE;

            Reserved->Next = NULL;
            Adapter->LastLoopBack = FirstPacket;

            //
            // Increment the reference count on the open since it will be
            // leaving a packet on the loopback queue.
            //

            PNE3200_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle)->References++;

        } else {

            NE3200PutPacketOnFinishTrans(
                Adapter,
                FirstPacket
                );

        }

    }

}

STATIC
VOID
NE3200ConstrainPacket(
    IN PNE3200_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    Given a packet and if necessary attempt to acquire adapter
    buffer resources so that the packet meets NE3200 hardware/MAC.BIN
    contraints.

    NOTE : MUST BE CALLED WITH LOCK HELD!!  MUST BE CALLED WITH
    NE3200BufferListHead != -1!!

Arguments:

    Adapter - The adapter the packet is coming through.

    Packet - The packet whose buffers are to be constrained.
             The packet reserved section is filled with information
             detailing how the packet needs to be adjusted.

Return Value:

    None.

--*/

{

    //
    // Pointer to the reserved section of the packet to be contrained.
    //
    PNE3200_RESERVED Reserved = PNE3200_RESERVED_FROM_PACKET(Packet);

    //
    // Holds the adapter buffer index available for allocation.
    //
    INT NE3200BuffersIndex;

    //
    // Points to a successfully allocated adapter buffer descriptor.
    //
    PNE3200_BUFFER_DESCRIPTOR BufferDescriptor;

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
    // The total amount of data contained within the ndis packet.
    //
    UINT TotalVirtualLength;

    //
    // Simple iteration variable.
    //
    INT i;

    NE3200BuffersIndex = Adapter->NE3200BufferListHead;

    BufferDescriptor = Adapter->NE3200Buffers + NE3200BuffersIndex;
    Adapter->NE3200BufferListHead = BufferDescriptor->Next;

    NdisReleaseSpinLock(&Adapter->Lock);

    //
    // Fill in the adapter buffer with the data from the users
    // buffers.
    //

    CurrentDestination = BufferDescriptor->VirtualNE3200Buffer;

    NdisQueryPacket(
        Packet,
        NULL,
        NULL,
        &SourceBuffer,
        &TotalVirtualLength
        );

    NdisQueryBuffer(
        SourceBuffer,
        &SourceData,
        &SourceLength
        );

    BufferDescriptor->DataLength = TotalVirtualLength;

    for (
        i = Reserved->STAGE.STAGE1.NdisBuffersToMove;
        i;
        i--
        ) {

        NE3200_MOVE_MEMORY(
            CurrentDestination,
            SourceData,
            SourceLength
            );

        CurrentDestination = (PCHAR)CurrentDestination + SourceLength;

        TotalDataMoved += SourceLength;

        if (i > 1) {

            NdisGetNextBuffer(
                SourceBuffer,
                &SourceBuffer
                );

            NdisQueryBuffer(
                SourceBuffer,
                &SourceData,
                &SourceLength
                );

        }

    }

    //
    // If the packet is less than the minimum Ethernet
    // packet size, then clear the remaining part of
    // the buffer up to the minimum packet size.
    //

    if (TotalVirtualLength < MINIMUM_ETHERNET_PACKET_SIZE) {

        NdisZeroMemory(
            CurrentDestination,
            MINIMUM_ETHERNET_PACKET_SIZE - TotalVirtualLength
            );

    }

    //
    // We need to save in the packet which adapter buffer descriptor
    // it is using so that we can deallocate it later.
    //

    Reserved->STAGE.STAGE2.NE3200BuffersIndex = NE3200BuffersIndex;
    Reserved->STAGE.STAGE2.UsedNE3200Buffer = TRUE;

    NdisAcquireSpinLock(&Adapter->Lock);

}

