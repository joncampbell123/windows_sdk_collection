/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    interrup.c

Abstract:

    This module contains the interrupt-processing code for the Novell
    NE3200 NDIS 3.0 driver.

Environment:

    Kernel Mode.

--*/

//
// So we can trace things...
//
#define STATIC

#include <ne3200sw.h>
#ifdef NDIS_WIN
    #pragma LCODE
#endif

extern NE3200_GLOBAL_DATA NE3200Globals;


#if DBG

PrintLogFile()
{
    UCHAR i;
    UCHAR j = 0;

    for (i = Ne3200LogPlace-1; i != Ne3200LogPlace; i--, j++) {

        DbgPrint("%c", Ne3200Log[i]);

        if (j == 15) {

            j=0;

            DbgPrint("\n");

        }

    }

}

#endif

extern
VOID
NE3200ProcessRequestQueue(
    IN PNE3200_ADAPTER Adapter,
    IN BOOLEAN StatisticsUpdated
    );

STATIC
BOOLEAN
NE3200RelinquishReceivePacket(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_RECEIVE_ENTRY CurrentEntry
    );

STATIC
BOOLEAN
NE3200ProcessReceiveInterrupts(
    IN PNE3200_ADAPTER Adapter
    );

STATIC
VOID
NE3200ProcessCommandInterrupts(
    IN PNE3200_ADAPTER Adapter
    );


BOOLEAN
NE3200Isr(
    IN PVOID Context
    )

/*++

Routine Description:

    Interrupt service routine for the NE3200.  It's main job is
    to get the value of the System Doorbell Register and record the
    changes in the adapters own list of interrupt reasons.

Arguments:

    Interrupt - Interrupt object for the NE3200.

    Context - Really a pointer to the adapter.

Return Value:

    Returns true if the interrupt really was from our NE3200.

--*/

{

    //
    // Will hold the value from the System Doorbell Register.
    //
    UCHAR SystemDoorbell;

    //
    // Holds the pointer to the adapter.
    //
    PNE3200_ADAPTER Adapter = Context;

    IF_LOG('i');

    NE3200_READ_SYSTEM_DOORBELL_INTERRUPT(Adapter, &SystemDoorbell);

    if (SystemDoorbell & NE3200_SYSTEM_DOORBELL_MASK) {

        IF_LOG(SystemDoorbell);

        //
        // It's our interrupt.  Disable further interrupts.
        //

        NE3200_WRITE_SYSTEM_DOORBELL_MASK(
            Adapter,
            0
            );

        IF_LOG('I');

        return TRUE;

    } else {

        IF_LOG('I');

        return FALSE;

    }

}



STATIC
VOID
NE3200ProcessInterrupt(
    IN PVOID SystemArgument1,
    IN PVOID InterruptContext,
    IN PVOID SystemArgument2,
    IN PVOID SystemArgument3
    )

/*++

Routine Description:

    Main routine for processing interrupts.

Arguments:

    Adapter - The Adapter to process interrupts for.

Return Value:

    None.

--*/

{
    PNE3200_ADAPTER Adapter = ((PNE3200_ADAPTER)InterruptContext);

    //
    // Holds a value of SystemDoorbellInterrupt.
    //
    USHORT SystemDoorbell = 0;

    BOOLEAN IndicateReceiveComplete = FALSE;

    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    UNREFERENCED_PARAMETER(SystemArgument3);

    //
    // Loop until there are no more processing sources.
    //

    NdisDprAcquireSpinLock(&Adapter->Lock);

#if DBG
    IF_LOG('p');
#endif

    if (Adapter->DoingProcessing) {

        IF_LOG('P');

        NdisDprReleaseSpinLock(&Adapter->Lock);

        return;

    }

    while (TRUE) {

        if (SystemDoorbell == 0) {

            NE3200_READ_SYSTEM_DOORBELL_INTERRUPT(Adapter, &SystemDoorbell);

            //
            // We can do this without a SyncWithISR() because the ISR only
            // reads from this port. If it reads just after this write, then
            // all interrupts will be ack'd and no DPC will fire.  If it
            // reads before this write, then it will queue a DPC, but the
            // DPC will exit because the DoingProcessing flag will be set.
            //

            NE3200_WRITE_SYSTEM_DOORBELL_INTERRUPT(
                Adapter,
                SystemDoorbell
                );

            SystemDoorbell &= NE3200_SYSTEM_DOORBELL_MASK;

        }

        //
        // Check the interrupt source and other reasons
        // for processing.  If there are no reasons to
        // process then exit this loop.
        //
        // Note that when we check the for processing sources
        // that we "carefully" check to see if we are already
        // processing one of the stage queues.  We do this
        // by checking the "AlreadyProcessingStageX" variables
        // in the adapter.  If any of these are true then
        // we let whoever set that boolean take care of pushing
        // the packet through the stage queues.
        //
        // By checking the "AlreadyProcessingStageX" variables
        // we can prevent a possible priority inversion where
        // we get "stuck" behind something that is processing
        // at a lower priority level.
        //

        if ((SystemDoorbell & (NE3200_SYSTEM_DOORBELL_PACKET_RECEIVED |
                               NE3200_SYSTEM_DOORBELL_COMMAND_COMPLETE)) ||
            (!Adapter->AlreadyProcessingStage2
              &&
             (Adapter->FirstStage1Packet && Adapter->Stage2Open)) ||
            Adapter->FirstLoopBack ||
            (Adapter->ResetInProgress &&
             (Adapter->References == 1) &&
             (Adapter->ResetState == NE3200ResetStateComplete)) ||
            (!IsListEmpty(&Adapter->CloseList))) {

            Adapter->References++;
            Adapter->DoingProcessing = TRUE;

        } else {

            break;

        }

        //
        // Note that the following code depends on the fact that
        // code above left the spinlock held.
        //

        //
        // If we have a reset in progress and the adapters reference
        // count is 1 (meaning no routine is in the interface and
        // we are the only "active" interrupt processing routine) then
        // it is safe to start the reset.
        //

        if (Adapter->ResetInProgress &&
            (Adapter->References == 2) &&
            (Adapter->ResetState == NE3200ResetStateComplete)) {

            NE3200DoAdapterReset(Adapter);
            Adapter->DoingProcessing = FALSE;
            Adapter->References--;
            break;

        }

        //
        // Check the interrupt vector and see if there are any
        // more receives to process.  After we process any
        // other interrupt source we always come back to the top
        // of the loop to check if any more receive packets have
        // come in.  This is to lessen the probability that we
        // drop a receive.
        //

        if (SystemDoorbell & NE3200_SYSTEM_DOORBELL_PACKET_RECEIVED) {

            IF_LOG('r');

            Adapter->ReceiveInterrupt = TRUE;

            NdisDprReleaseSpinLock(&Adapter->Lock);

            if (NE3200ProcessReceiveInterrupts(Adapter)) {

                SystemDoorbell &= ~NE3200_SYSTEM_DOORBELL_PACKET_RECEIVED;

            }

            NdisDprAcquireSpinLock(&Adapter->Lock);

            IF_LOG('R');

            IndicateReceiveComplete = TRUE;

        }


        //
        // First we check that this is a packet that was transmitted
        // but not already processed.  Recall that this routine
        // will be called repeatedly until this tests false, Or we
        // hit a packet that we don't completely own.
        //

        if ((Adapter->FirstCommandOnCard == NULL) ||
            (Adapter->FirstCommandOnCard->Hardware.State != NE3200_STATE_EXECUTION_COMPLETE)) {

            //
            // No more work to do.
            //

            SystemDoorbell &= ~NE3200_SYSTEM_DOORBELL_COMMAND_COMPLETE;

        } else {

            IF_LOG('c');

            Adapter->SendInterrupt = TRUE;

            NE3200ProcessCommandInterrupts(Adapter);

            IF_LOG('C');

        }


        //
        // Only try to push a packet through the stage queues
        // if somebody else isn't already doing it and
        // there is some hope of moving some packets
        // ahead.
        //

        if (!Adapter->AlreadyProcessingStage2
             &&
            (Adapter->FirstStage1Packet && Adapter->Stage2Open)
           ) {

            NE3200StagedAllocation(Adapter);

        }

        //
        // Process the loopback queue.
        //
        // NOTE: Incase anyone ever figures out how to make this
        // loop more reentriant, special care needs to be taken that
        // loopback packets and regular receive packets are NOT being
        // indicated at the same time.  While the filter indication
        // routines can handle this, I doubt that the transport can.
        //

        if (Adapter->FirstLoopBack) {

            IF_LOG('l');

            NE3200ProcessLoopback(Adapter);

            IndicateReceiveComplete = TRUE;

            IF_LOG('L');

        }

        //
        // If there are any opens on the closing list and their
        // reference counts are zero then complete the close and
        // delete them from the list.
        // We need to this here because a completed send might cause
        // the ref count to become zero.
        //

        if (!IsListEmpty(&Adapter->CloseList)) {

            PNE3200_OPEN Open;

            Open = CONTAINING_RECORD(
                     Adapter->CloseList.Flink,
                     NE3200_OPEN,
                     OpenList
                     );

            if (!Open->References) {

                NdisDprReleaseSpinLock(&Adapter->Lock);

                NdisCompleteCloseAdapter(
                    Open->NdisBindingContext,
                    NDIS_STATUS_SUCCESS
                    );

                NdisDprAcquireSpinLock(&Adapter->Lock);
                RemoveEntryList(&Open->OpenList);
                NE3200_FREE_PHYS(Open);

                Adapter->OpenCount--;

            }

        }

        //
        // NOTE: This code assumes that the above code left
        // the spinlock acquired.
        //
        // Bottom of the interrupt processing loop.  Another dpc
        // could be coming in at this point to process interrupts.
        // We clear the flag that says we're processing interrupts
        // so that some invocation of the DPC can grab it and process
        // any further interrupts.
        //

        Adapter->DoingProcessing = FALSE;
        Adapter->References--;

    }

    IF_LOG('P');

    //
    // Enable further interrupts.
    //

    NE3200_WRITE_SYSTEM_DOORBELL_MASK(
            Adapter,
            NE3200_SYSTEM_DOORBELL_MASK
            );

    NdisDprReleaseSpinLock(&Adapter->Lock);

    if (IndicateReceiveComplete) {
        EthFilterIndicateReceiveComplete(Adapter->FilterDB);
    }

}

STATIC
BOOLEAN
NE3200ProcessReceiveInterrupts(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    Process the packets that have finished receiving.

    NOTE: This routine assumes that no other thread of execution
    is processing receives!

Arguments:

    Adapter - The adapter to indicate to.

Return Value:

    Whether to clear interrupt or not

--*/

{

    //
    // We don't get here unless there was a receive.  Loop through
    // the receive blocks starting at the last known block owned by
    // the hardware.
    //
    // Examine each receive block for errors.
    //
    // We keep an array whose elements are indexed by the block
    // index of the receive blocks.  The arrays elements are the
    // virtual addresses of the buffers pointed to by each block.
    //
    // After we find a packet we give the routine that process the
    // packet through the filter, the buffers virtual address (which
    // is always the lookahead size) and as the MAC Context the
    // index to the receive block.
    //

    //
    // Pointer to the receive block being examined.
    //
    PNE3200_SUPER_RECEIVE_ENTRY CurrentEntry = Adapter->ReceiveQueueHead;

    //
    // Pointer to last receive block in the queue.
    //
    PNE3200_SUPER_RECEIVE_ENTRY LastEntry;

    ULONG ReceivePacketCount = 0;

    while (TRUE) {

        //
        // Ensure that our Receive Entry is on an even boundary.
        //
        ASSERT(!(NdisGetPhysicalAddressLow(CurrentEntry->Self) & 1));

        //
        // Check to see whether we own the packet.  If
        // we don't then simply return to the caller.
        //

        if (CurrentEntry->Hardware.State == NE3200_STATE_FREE) {

            Adapter->ReceiveQueueHead = CurrentEntry;

            return TRUE;

        }

        //
        // We've found a packet.  Prepare the parameters
        // for indication, then indicate.
        //

        //
        // Check just before we do indications that we aren't
        // resetting.
        //

        if (Adapter->ResetInProgress) {

            Adapter->ReceiveQueueHead = CurrentEntry;

            return TRUE;
        }

        if (ReceivePacketCount > 10) {

            Adapter->ReceiveQueueHead = CurrentEntry;

            return FALSE;

        }

        ReceivePacketCount++;

        Adapter->GoodReceives++;

        NdisFlushBuffer(CurrentEntry->FlushBuffer, FALSE);

        if ((UINT)(CurrentEntry->Hardware.FrameSize) < NE3200_HEADER_SIZE) {

            if ((UINT)(CurrentEntry->Hardware.FrameSize) >= ETH_LENGTH_OF_ADDRESS) {

                //
                // Runt Packet
                //

                EthFilterIndicateReceive(
                    Adapter->FilterDB,
                    (NDIS_HANDLE)(CurrentEntry->ReceiveBuffer),
                    ((PCHAR)CurrentEntry->ReceiveBuffer),
                    CurrentEntry->ReceiveBuffer,
                    (UINT)CurrentEntry->Hardware.FrameSize,
                    NULL,
                    0,
                    0
                    );

            }

        } else {

            EthFilterIndicateReceive(
                Adapter->FilterDB,
                (NDIS_HANDLE)(CurrentEntry->ReceiveBuffer),
                ((PCHAR)CurrentEntry->ReceiveBuffer),
                CurrentEntry->ReceiveBuffer,
                NE3200_HEADER_SIZE,
                ((PUCHAR)CurrentEntry->ReceiveBuffer) + NE3200_HEADER_SIZE,
                (UINT)CurrentEntry->Hardware.FrameSize - NE3200_HEADER_SIZE,
                (UINT)CurrentEntry->Hardware.FrameSize - NE3200_HEADER_SIZE
                );

        }

        //
        // Give the packet back to the hardware.
        //

        //
        // Chain the current block onto the tail of the Receive Queue.
        //

        CurrentEntry->Hardware.NextPending = NE3200_NULL;
        CurrentEntry->Hardware.State = NE3200_STATE_FREE;

        LastEntry = Adapter->ReceiveQueueTail;

        LastEntry->Hardware.NextPending =
            NdisGetPhysicalAddressLow(CurrentEntry->Self);

        //
        // Update the queue tail.
        //

        Adapter->ReceiveQueueTail = LastEntry->NextEntry;

        //
        // Advance to the next block.
        //

        CurrentEntry = CurrentEntry->NextEntry;

        //
        // See if the adapter needs to be restarted.
        //

        if (LastEntry->Hardware.State != NE3200_STATE_FREE) {

#if DBG
            NdisDprAcquireSpinLock(&Adapter->Lock);
            IF_LOG('!');
            NdisDprReleaseSpinLock(&Adapter->Lock);
#endif
            //
            // We've exhausted all Receive Blocks.  Now we
            // must restart the adapter.
            //

            NE3200StartChip(Adapter, Adapter->ReceiveQueueTail);

        }

    }

}

STATIC
VOID
NE3200ProcessCommandInterrupts(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    Process the Command Complete interrupts.

    NOTE: This routine assumes that it is being executed in a
    single thread of execution.

    NOTE: Must be called with the spin lock held!!

Arguments:

    Adapter - The adapter that was sent from.

Return Value:

    None.

--*/

{

    //
    // Pointer to command block being processed.
    //
    PNE3200_SUPER_COMMAND_BLOCK CurrentCommandBlock = Adapter->FirstCommandOnCard;

    //
    // Holds whether the packet successfully transmitted or not.
    //
    BOOLEAN Successful;

    //
    // Pointer to the packet that started this transmission.
    //
    PNDIS_PACKET OwningPacket;

    //
    // Points to the reserved part of the OwningPacket.
    //
    PNE3200_RESERVED Reserved;

    //
    // Get hold of the first transmitted packet.
    //

    //
    // Ensure that the Command Block is on an even boundary.
    //
    ASSERT(!(NdisGetPhysicalAddressLow(CurrentCommandBlock->Self) & 1));

    IF_LOG('t');

    if (CurrentCommandBlock->Hardware.CommandCode == NE3200_COMMAND_TRANSMIT) {

        //
        // The current command block is from a transmit.
        //

        //
        // Get a pointer to the owning packet and the reserved part of
        // the packet.
        //

        OwningPacket = CurrentCommandBlock->OwningPacket;

        Reserved = PNE3200_RESERVED_FROM_PACKET(OwningPacket);

        if (CurrentCommandBlock->UsedNE3200Buffer) {

            //
            // This packet used adapter buffers.  We can
            // now return these buffers to the adapter.
            //

            //
            // Put the adapter buffer back on the free list.
            //

            //
            // The adapter buffer descriptor that was allocated to this packet.
            //
            PNE3200_BUFFER_DESCRIPTOR BufferDescriptor = Adapter->NE3200Buffers +
                                                  CurrentCommandBlock->NE3200BuffersIndex;

            BufferDescriptor->Next = Adapter->NE3200BufferListHead;
            Adapter->NE3200BufferListHead = CurrentCommandBlock->NE3200BuffersIndex;

            //
            // If stage 2 was closed and we aren't resetting then open
            // it back up.
            //

            if ((!Adapter->Stage2Open) && (!Adapter->ResetInProgress)) {

                Adapter->Stage2Open = TRUE;

            }

        } else {

            PNDIS_BUFFER CurrentBuffer;
            UINT CurMapRegister;

            //
            // The transmit is finished, so we can release
            // the physical mapping used for it.
            //

            NdisQueryPacket(
                OwningPacket,
                NULL,
                NULL,
                &CurrentBuffer,
                NULL
                );

            CurMapRegister = CurrentCommandBlock->CommandBlockIndex *
                        NE3200_MAXIMUM_BLOCKS_PER_PACKET;

            while (CurrentBuffer) {

                NdisCompleteBufferPhysicalMapping(
                    Adapter->NdisAdapterHandle,
                    CurrentBuffer,
                    CurMapRegister
                    );

                ++CurMapRegister;

                NdisGetNextBuffer(
                    CurrentBuffer,
                    &CurrentBuffer
                    );

            }

        }

        //
        // If there was an error transmitting this
        // packet, update our error counters.
        //

        if (CurrentCommandBlock->Hardware.Status & NE3200_STATUS_FATALERROR_MASK) {

            if (CurrentCommandBlock->Hardware.Status &
                NE3200_STATUS_MAXIMUM_COLLISIONS) {

                Adapter->RetryFailure++;

            } else if (CurrentCommandBlock->Hardware.Status &
                NE3200_STATUS_NO_CARRIER) {

                Adapter->LostCarrier++;

            } else if (CurrentCommandBlock->Hardware.Status &
                NE3200_STATUS_HEART_BEAT) {

                Adapter->NoClearToSend++;

            } else if (CurrentCommandBlock->Hardware.Status &
                NE3200_STATUS_DMA_UNDERRUN) {

                Adapter->UnderFlow++;

            }

            Successful = FALSE;

        } else {

            Adapter->GoodTransmits++;

            if (CurrentCommandBlock->Hardware.Status & NE3200_STATUS_TRANSMIT_DEFERRED) {

                Adapter->Deferred++;

            } else if ((CurrentCommandBlock->Hardware.Status & NE3200_STATUS_COLLISION_MASK) == 1) {

                Adapter->OneRetry++;

            } else if ((CurrentCommandBlock->Hardware.Status & NE3200_STATUS_COLLISION_MASK) > 1) {

                Adapter->MoreThanOneRetry++;

            }

            Successful = TRUE;

        }

        if (Reserved->STAGE.STAGE4.ReadyToComplete) {

            //
            // The binding that is submitting this packet.
            //
            PNE3200_OPEN Open =
                PNE3200_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);


            //
            // Next packet on list
            //

            PNDIS_PACKET Forward = Reserved->Next;
            PNDIS_PACKET Back;

            //
            // While we're indicating we dont increment the reference
            // count because there already is a reference for the packet
            // and so the open will not be deleted from under us.
            //

            //
            // Along with at least one reference because of the coming
            // indication there should be a reference because of the
            // packet to indicate.
            //

            ASSERT(Open->References > 1);

            //
            // Either the packet is done with loopback or
            // the packet didn't need to be loopbacked.  In
            // any case we can let the protocol know that the
            // send is complete after we remove the packet from
            // the finish transmit queue.
            //

            ASSERT(sizeof(UINT) == sizeof(PNDIS_PACKET));

            //
            // Get rid of the low bit that is set in the backpointer by
            // the routine that inserted this packet on the finish
            // transmission list.
            //

            Reserved->STAGE.STAGE4.ReadyToComplete = FALSE;
            Back = Reserved->STAGE.BackPointer;

            if (!Back) {

                Adapter->FirstFinishTransmit = Forward;

            } else {

                PNE3200_RESERVED_FROM_PACKET(Back)->Next = Forward;

            }

            if (!Forward) {

                Adapter->LastFinishTransmit = Back;

            } else {

                PNE3200_RESERVED_FROM_PACKET(Forward)->STAGE.BackPointer = Back;
                PNE3200_RESERVED_FROM_PACKET(Forward)->STAGE.STAGE4.ReadyToComplete = TRUE;

            }

            NdisDprReleaseSpinLock(&Adapter->Lock);

            NdisCompleteSend(
                Open->NdisBindingContext,
                OwningPacket,
                ((Successful)?(NDIS_STATUS_SUCCESS):(NDIS_STATUS_FAILURE))
                );

            NdisDprAcquireSpinLock(&Adapter->Lock);

            //
            // We reduce the count by one to account for the original
            // reference
            //

            Open->References--;

        } else {

            //
            // Let the loopback queue know that the hardware
            // is finished with the packet, and record whether
            // it could transmit or not.
            //

            Reserved->STAGE.STAGE4.ReadyToComplete = TRUE;
            Reserved->STAGE.STAGE4.SuccessfulTransmit = Successful;

            //
            // Decrement the reference count by one since it
            // was incremented by one when the packet was given
            // to be transmitted.
            //

            PNE3200_OPEN_FROM_BINDING_HANDLE(
                Reserved->MacBindingHandle
                )->References--;

        }

        //
        // Release the command block.
        //

        NE3200RelinquishCommandBlock(Adapter, CurrentCommandBlock);

        //
        // Since we've given back a command block we should
        // open of stage2 if it was closed and we are not resetting.
        //

        if ((!Adapter->Stage2Open) && (!Adapter->ResetInProgress)) {

            Adapter->Stage2Open = TRUE;

        }

    } else if (CurrentCommandBlock->Hardware.CommandCode ==
        NE3200_COMMAND_READ_ADAPTER_STATISTICS) {

        //
        // Release the command block.
        //

        Adapter->OutOfResources =
            CurrentCommandBlock->Hardware.PARAMETERS.STATISTICS.ResourceErrors;

        Adapter->CrcErrors =
            CurrentCommandBlock->Hardware.PARAMETERS.STATISTICS.CrcErrors;

        Adapter->AlignmentErrors =
            CurrentCommandBlock->Hardware.PARAMETERS.STATISTICS.AlignmentErrors;

        Adapter->DmaOverruns =
            CurrentCommandBlock->Hardware.PARAMETERS.STATISTICS.OverrunErrors;


        NE3200ProcessRequestQueue(Adapter, TRUE);
        NE3200RelinquishCommandBlock(Adapter, CurrentCommandBlock);

    } else if (CurrentCommandBlock->Hardware.CommandCode ==
        NE3200_COMMAND_CLEAR_ADAPTER_STATISTICS) {

        //
        // Release the command block.
        //

        NE3200RelinquishCommandBlock(Adapter, CurrentCommandBlock);

    } else if (CurrentCommandBlock->Hardware.CommandCode ==
        NE3200_COMMAND_SET_STATION_ADDRESS) {
        //
        // Ignore
        //

    } else {

        //
        // The current command block is not from a transmit.
        // Indicate completion to whoever initiated this command.
        //

        //
        // This points to the open binding which initiated this
        // command.
        //

        PNE3200_OPEN Open = CurrentCommandBlock->OwningOpenBinding;

        if ((Open != NULL) && (Open != NE3200_FAKE_OPEN)) {

            //
            // Complete the request.
            //
            // if the CurrentCommandBlock->NdisRequest is NULL,
            // it means this multicast operation was not caused by
            // an NdisRequest
            //

            if (CurrentCommandBlock->Set) {

                PNDIS_REQUEST Request;
                PNE3200_REQUEST_RESERVED Reserved;
                PNE3200_OPEN Open;

                Request = Adapter->FirstRequest;
                Reserved = PNE3200_RESERVED_FROM_REQUEST(Request);
                Open = Reserved->OpenBlock;

                Adapter->FirstRequest = Reserved->Next;

                Open = Reserved->OpenBlock;

                IF_LOG(']');

                NdisDprReleaseSpinLock(&Adapter->Lock);

                NdisCompleteRequest(
                    Open->NdisBindingContext,
                    Request,
                    NDIS_STATUS_SUCCESS);

                NdisDprAcquireSpinLock(&Adapter->Lock);

                Open->References--;

                //
                // Now continue processing requests if needed.
                //

                NE3200ProcessRequestQueue(Adapter, FALSE);

            }

        }

#if DBG
        else {

            IF_LOG('}');

        }
#endif

        //
        // Release the command block.
        //

        NE3200RelinquishCommandBlock(Adapter, CurrentCommandBlock);

    }

    IF_LOG('T');
}



VOID
NE3200WakeUpDpc(
    IN PVOID SystemArgument1,
    IN PVOID Context,
    IN PVOID SystemArgument2,
    IN PVOID SystemArgument3
    )

/*++

Routine Description:

    This DPC routine is queued every 2 seconds to check on the
    head of the command block queue.  It will fire off the
    queue if the head has been sleeping on the job for more
    than 2 seconds.  It will fire off another dpc after doing the
    check.

Arguments:

    Context - Really a pointer to the adapter.

Return Value:

    None.

--*/
{
    PNE3200_ADAPTER Adapter = Context;

    //
    // Now that we're done handling the interrupt,
    // let's see if the first pending command has
    // timed-out.  If so, kick the adapter in the ass.
    //

    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    UNREFERENCED_PARAMETER(SystemArgument3);

    NdisDprAcquireSpinLock(&Adapter->Lock);

    {

        //
        // Blow this off if there's nothing waiting to complete.
        // Also blow it off if this command block is not waiting
        // for the adapter.
        //

        PNE3200_SUPER_COMMAND_BLOCK FirstPending = Adapter->FirstCommandOnCard;

        if( (FirstPending != NULL) &&
            (FirstPending->Hardware.State == NE3200_STATE_WAIT_FOR_ADAPTER) ) {

            //
            // See if the command block has timed-out.
            //

            if ( FirstPending->Timeout ) {

                if ( FirstPending->TimeoutCount >= 2) {

                    //
                    // Give up, the card appears dead.
                    //

#if DBG

                    if (NE3200Debug & NE3200_DEBUG_LOUD) {
                        DbgPrint("NE3200.SYS : Hung command block\n");
                    }
                    if (NE3200Debug & NE3200_DEBUG_VERY_LOUD) {
                        PrintLogFile();
                    }
                    //
                    // Save the current log file
                    //

                    NE3200_MOVE_MEMORY(&(Ne3200LogSave[Ne3200LogSavePlace][0]), Ne3200Log, 256);

                    if (++Ne3200LogSavePlace == 10) {

                        Ne3200LogSavePlace = 0;

                    }

#endif

                    //
                    // SetUp the chip for reset
                    //

                    NE3200SetupForReset(Adapter, NULL);

                    Adapter->ResetAsynchronous = TRUE;

                    //
                    // This will actually call Ne3200DoAdapterReset
                    //

                    Adapter->SendInterrupt = FALSE;
                    Adapter->NoReceiveInterruptCount = 0;

                    NdisSetTimer(&Adapter->DeferredTimer, 0);

                } else {

#if DBG
                    if (NE3200Debug & NE3200_DEBUG_LOUD) {
                        DbgPrint("NE3200.SYS : Resubmit command block\n");
                    }
                    if (NE3200Debug & NE3200_DEBUG_VERY_LOUD) {
                        PrintLogFile();
                    }
#endif

                    //
                    // Re-sumbit the block.
                    //

                    NE3200_WRITE_COMMAND_POINTER(
                        Adapter,
                        NdisGetPhysicalAddressLow(FirstPending->Self)
                        );

                    NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
                        Adapter,
                        NE3200_LOCAL_DOORBELL_NEW_COMMAND
                        );

                    IF_LOG('+');

                    FirstPending->TimeoutCount++;

                }

            } else {

                IF_LOG('0');
                FirstPending->Timeout = TRUE;
                FirstPending->TimeoutCount = 0;

            }

        }

        //
        // Check if the receive side has died.
        //

        if ((!Adapter->ReceiveInterrupt) && (Adapter->SendInterrupt)) {

            //
            // If we go 10 seconds with no receives, but we are sending then
            // we will reset the adapter.
            //

            if ((Adapter->NoReceiveInterruptCount == (10000 / NE3200_TIMEOUT_COMMAND)) &&
                !(Adapter->ResetInProgress)){

                //
                // We've waited long enough
                //

#if DBG
                if (NE3200Debug & NE3200_DEBUG_LOUD) {
                    DbgPrint("NE3200.SYS : Hung receives\n");
                }
                if (NE3200Debug & NE3200_DEBUG_VERY_LOUD) {
                    PrintLogFile();
                }
                //
                // Save the current log file
                //

                NE3200_MOVE_MEMORY(&(Ne3200LogSave[Ne3200LogSavePlace][0]), Ne3200Log, 256);

                if (++Ne3200LogSavePlace == 10) {

                    Ne3200LogSavePlace = 0;

                }

#endif

                //
                // SetUp the chip for reset
                //

                NE3200SetupForReset(Adapter, NULL);

                Adapter->ResetAsynchronous = TRUE;

                //
                // This will actually call Ne3200DoAdapterReset
                //

                Adapter->SendInterrupt = FALSE;
                Adapter->NoReceiveInterruptCount = 0;

                NdisSetTimer(&Adapter->DeferredTimer, 0);

            } else {

                Adapter->NoReceiveInterruptCount++;

            }

        } else {

            //
            // If we got a receive or there are no sends, doesn't matter.  Reset
            // the state.
            //

            Adapter->SendInterrupt = FALSE;
            Adapter->ReceiveInterrupt = FALSE;
            Adapter->NoReceiveInterruptCount = 0;

        }

        NdisDprReleaseSpinLock(&Adapter->Lock);

        //
        // Fire off another Dpc to execute after TIMEOUT milliseconds
        //

        NdisSetTimer(
                 &Adapter->WakeUpTimer,
                 NE3200_TIMEOUT_COMMAND
                 );
    }
}

BOOLEAN
SyncNE3200ClearDoorbellInterrupt(
    IN PVOID SyncContext
    )
/*++

Routine Description:

    Clears the Doorbell Interrupt Port.

Arguments:

    SyncContext - pointer to the adapter block

Return Value:

    Always TRUE

--*/

{

    PNE3200_ADAPTER Adapter = (PNE3200_ADAPTER)SyncContext;

    NdisRawWritePortUchar(
        (ULONG)(Adapter->SystemDoorbellInterruptPort),
        (UCHAR)0
        );

    return(FALSE);
}

