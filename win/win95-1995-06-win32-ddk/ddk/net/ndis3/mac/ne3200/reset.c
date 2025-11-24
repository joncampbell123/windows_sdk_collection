/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    reset.c

Abstract:

    This is the  file containing the reset code for the Novell NE3200 EISA
    Ethernet adapter.    This driver conforms to the NDIS 3.0 interface.

Environment:

    Kernel Mode - Or whatever is the equivalent on OS/2 and DOS.

--*/

#include <ne3200sw.h>
#include <eisa.h>

extern NE3200_GLOBAL_DATA NE3200Globals;

STATIC
VOID
NE3200SetConfigurationBlock(
    IN PNE3200_ADAPTER Adapter
    );

STATIC
BOOLEAN
NE3200SetConfigurationBlockAndInit(
    IN PNE3200_ADAPTER Adapter
    );

NDIS_STATUS
NE3200ChangeCurrentAddress(
    IN PNE3200_ADAPTER Adapter
    );

VOID
NE3200ResetCommandBlocks(
    IN PNE3200_ADAPTER Adapter
    );

extern
VOID
NE3200GetStationAddress(
    IN PNE3200_ADAPTER Adapter
    );

extern
VOID
NE3200SetupForReset(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open
    );

STATIC
VOID
NE3200DoResetIndications(
    IN PNE3200_ADAPTER Adapter,
    IN NDIS_STATUS Status
    );

VOID
NE3200ResetHandler(
    IN PVOID SystemSpecific1,
    IN PNE3200_ADAPTER Adapter,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

#ifdef NDIS_WIN
    #pragma ICODE
#endif

BOOLEAN
NE3200InitialInit(
    IN PNE3200_ADAPTER Adapter,
    IN UINT NE3200InterruptVector,
    IN NDIS_INTERRUPT_MODE NE3200InterruptMode
    )

/*++

Routine Description:

    This routine sets up the initial init of the driver.

Arguments:

    Adapter - The adapter for the hardware.

Return Value:

    None.

--*/

{
    NDIS_STATUS Status;

    //
    // First we make sure that the device is stopped.
    //

    NE3200StopChip(Adapter, FALSE);


    //
    // Initialize the interrupt.
    //

    NdisInitializeInterrupt(
        &Status,
        &Adapter->Interrupt,
        Adapter->NdisAdapterHandle,
        NE3200Isr,
        Adapter,
        NE3200ProcessInterrupt,
        NE3200InterruptVector,
        NE3200InterruptVector,
        FALSE,
        NE3200InterruptMode
        );

    if (Status == NDIS_STATUS_SUCCESS) {

        NdisAcquireSpinLock(&Adapter->Lock);

        if (!NE3200SetConfigurationBlockAndInit(Adapter)) {

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_TIMEOUT,
                2,
                initialInit,
                NE3200_ERRMSG_NO_DELAY
                );

            NdisRemoveInterrupt(&Adapter->Interrupt);
            return FALSE;

        }

        NdisReleaseSpinLock(&Adapter->Lock);

        //
        // Get hardware assigned network address.
        //

        NE3200GetStationAddress(
            Adapter
            );

        //
        // We can start the chip.  We may not
        // have any bindings to indicate to but this
        // is unimportant.
        //

        Status = NE3200ChangeCurrentAddress(Adapter);

        return(Status == NDIS_STATUS_SUCCESS);

    } else {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_INTERRUPT_CONNECT,
            2,
            initialInit,
            NE3200_ERRMSG_INIT_INTERRUPT
            );

        return(FALSE);
    }
}
#ifdef NDIS_WIN
    #pragma LCODE
#endif

VOID
NE3200StartChip(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_RECEIVE_ENTRY FirstReceiveEntry
    )

/*++

Routine Description:

    This routine is used to start an already initialized NE3200.

Arguments:

    Adapter - The adapter for the NE3200 to start.

    FirstReceiveEntry - Pointer to the first receive entry to be
    used by the adapter.

Return Value:

    None.

--*/

{

    IF_LOG('s');

    if (Adapter->BeingRemoved) {

        return;

    }

    //
    // Write the new receive pointer.
    //

    NE3200_WRITE_RECEIVE_POINTER(
        Adapter,
        NdisGetPhysicalAddressLow(FirstReceiveEntry->Self)
        );

    NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
        Adapter,
        NE3200_LOCAL_DOORBELL_NEW_RECEIVE
        );

    //
    // Initialize the doorbell & system interrupt masks
    //

    NE3200_WRITE_SYSTEM_INTERRUPT(
        Adapter,
        NE3200_SYSTEM_INTERRUPT_ENABLE
        );

    NE3200_WRITE_SYSTEM_DOORBELL_MASK(
        Adapter,
        NE3200_SYSTEM_DOORBELL_MASK
        );

    NE3200_WRITE_SYSTEM_DOORBELL_INTERRUPT(
        Adapter,
        NE3200_SYSTEM_DOORBELL_MASK
        );

    NE3200_WRITE_MAILBOX_UCHAR(
        Adapter,
        NE3200_MAILBOX_STATUS,
        0
        );

    IF_LOG('S');

}

VOID
NE3200StartChipAndDisableInterrupts(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_RECEIVE_ENTRY FirstReceiveEntry
    )

/*++

Routine Description:

    This routine is used to start an already initialized NE3200.

Arguments:

    Adapter - The adapter for the NE3200 to start.

    FirstReceiveEntry - Pointer to the first receive entry to be
    used by the adapter.

Return Value:

    None.

--*/

{

    //
    // Write the new receive pointer.
    //

    NE3200_WRITE_RECEIVE_POINTER(
        Adapter,
        NdisGetPhysicalAddressLow(FirstReceiveEntry->Self)
        );

    NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
        Adapter,
        NE3200_LOCAL_DOORBELL_NEW_RECEIVE
        );

    //
    // Initialize the doorbell & system interrupt masks
    //

    NE3200_WRITE_SYSTEM_INTERRUPT(
        Adapter,
        0
        );

    NE3200_WRITE_SYSTEM_DOORBELL_MASK(
        Adapter,
        0
        );

    NE3200_WRITE_SYSTEM_DOORBELL_INTERRUPT(
        Adapter,
        0
        );

    NE3200_WRITE_MAILBOX_UCHAR(
        Adapter,
        NE3200_MAILBOX_STATUS,
        0
        );

}

VOID
NE3200EnableInterrupts(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is used to start an already initialized NE3200.

Arguments:

    Adapter - The adapter for the NE3200 to start.

    FirstReceiveEntry - Pointer to the first receive entry to be
    used by the adapter.

Return Value:

    None.

--*/

{

    //
    // Initialize the doorbell & system interrupt masks
    //

    NE3200_WRITE_SYSTEM_INTERRUPT(
        Adapter,
        NE3200_SYSTEM_INTERRUPT_ENABLE
        );

    NE3200_WRITE_SYSTEM_DOORBELL_MASK(
        Adapter,
        NE3200_SYSTEM_DOORBELL_MASK
        );

    NE3200_WRITE_SYSTEM_DOORBELL_INTERRUPT(
        Adapter,
        NE3200_SYSTEM_DOORBELL_MASK
        );

    NE3200_WRITE_MAILBOX_UCHAR(
        Adapter,
        NE3200_MAILBOX_STATUS,
        0
        );

}

VOID
NE3200StopChip(
    IN PNE3200_ADAPTER Adapter,
    IN BOOLEAN SynchronizeWithInterrupt
    )

/*++

Routine Description:

    This routine is used to stop the NE3200.

Arguments:

    Adapter - The NE3200 adapter to stop.

    SynchronizeWithInterrupt - A value that says whether it is crucial that
    we synchronize with the ISR.

Return Value:

    None.

--*/

{

    IF_LOG('a');

    //
    // Packet reception can be stopped by writing a
    // (ULONG)-1 to the Receive Packet Mailbox port.
    // Also, commands can be stopped by writing a -1
    // to the Command Pointer Mailbox port.
    //

    NE3200_WRITE_RECEIVE_POINTER(
        Adapter,
        NE3200_NULL
    );

    NE3200_WRITE_COMMAND_POINTER(
        Adapter,
        NE3200_NULL
    );

    NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
        Adapter,
        NE3200_LOCAL_DOORBELL_NEW_RECEIVE | NE3200_LOCAL_DOORBELL_NEW_COMMAND
        );

    //
    // Disable the doorbell & system interrupt masks.
    //

    NE3200_WRITE_SYSTEM_INTERRUPT(
        Adapter,
        0
        );

    NE3200_WRITE_SYSTEM_DOORBELL_MASK(
        Adapter,
        0
        );

    if (SynchronizeWithInterrupt) {

        NE3200_SYNC_CLEAR_SYSTEM_DOORBELL_INTERRUPT(
            Adapter
            );

    } else {

        NE3200_WRITE_SYSTEM_DOORBELL_INTERRUPT(
            Adapter,
            0
            );

    }

    IF_LOG('A');

}


STATIC
NDIS_STATUS
NE3200Reset(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    The NE3200Reset request instructs the MAC to issue a hardware reset
    to the network adapter.  The MAC also resets its software state.  See
    the description of NdisReset for a detailed description of this request.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE3200_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_PENDING;

    PNE3200_ADAPTER Adapter =
        PNE3200_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the locks while we update the reference counts on the
    // adapter and the open.
    //

    NdisAcquireSpinLock(&Adapter->Lock);

    IF_LOG('w');

    Adapter->References++;

    if (!Adapter->ResetInProgress && !Adapter->BeingRemoved) {

        PNE3200_OPEN Open;

        Open = PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);


        if (!Open->BindingShuttingDown) {


            Open->References++;

            //
            // We need to signal every open binding that the
            // adapter is being reset.
            //

            {

                PNE3200_OPEN Open;
                PLIST_ENTRY CurrentLink;

                CurrentLink = Adapter->OpenBindings.Flink;

                while (CurrentLink != &Adapter->OpenBindings) {

                    Open = CONTAINING_RECORD(
                             CurrentLink,
                             NE3200_OPEN,
                             OpenList
                             );

                    if (Open->BindingShuttingDown) {

                        CurrentLink = CurrentLink->Flink;
                        continue;

                    }

                    Open->References++;

                    NdisReleaseSpinLock(&Adapter->Lock);

                    NdisIndicateStatus(
                        Open->NdisBindingContext,
                        NDIS_STATUS_RESET_START,
                        NULL,
                        0
                        );

                    NdisAcquireSpinLock(&Adapter->Lock);

                    Open->References--;

                    CurrentLink = CurrentLink->Flink;

                }
            }


            NE3200SetupForReset(
                Adapter,
                PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)
                );

            Open->References--;

        } else {

            StatusToReturn = NDIS_STATUS_CLOSING;

        }

    } else {

        if (Adapter->BeingRemoved) {

            StatusToReturn = NDIS_STATUS_ADAPTER_REMOVED;

        } else {

            StatusToReturn = NDIS_STATUS_RESET_IN_PROGRESS;

        }
    }

    IF_LOG('W');

    NE3200_DO_DEFERRED(Adapter);
    return StatusToReturn;

}

STATIC
VOID
NE3200SetConfigurationBlock(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine simply fills the configuration block
    with the information necessary for initialization.

    NOTE: this routine assumes a single thread of execution is accessing
    the particular adapter.

Arguments:

    Adapter - The adapter which holds the initialization block
    to initialize.

Return Value:

    None.


--*/

{

    PNE3200_CONFIGURATION_BLOCK Configuration;

    UINT PacketFilters;

    Configuration = Adapter->ConfigurationBlock;

    NdisZeroMemory(
        Configuration,
        sizeof(NE3200_CONFIGURATION_BLOCK)
        );

    Configuration->ByteCount = 12;
    Configuration->FifoThreshold = 8;
    // Configuration->Synchronization = 0;
    // Configuration->SaveBadFrames = 0;
    Configuration->AddressLength = 6;
    Configuration->SeparateAddressAndLength = 1;
    Configuration->PreambleLength = 2;
    // Configuration->InternalLoopback = 0;
    // Configuration->ExternalLoopback = 0;
    // Configuration->LinearPriority = 0;
    // Configuration->ExponentialPriority = 0;
    // Configuration->ExponentialBackoffMethod = 0;
    Configuration->InterframeSpacing = 96;
    Configuration->SlotTime = 512;
    Configuration->MaximumRetries = 15;
    // Configuration->PromiscuousMode = 0;
    Configuration->DisableBroadcast = 1;
    // Configuration->EncodingMethod = 0;
    // Configuration->TransmitOnNoCarrier = 0;
    // Configuration->DisableCrcInsertion = 0;
    // Configuration->CrcType = 0;
    // Configuration->BitStuffingMethod = 0;
    // Configuration->EnablePadding = 0;
    // Configuration->CarrierSenseFilter = 0;
    // Configuration->CarrierSenseSource = 0;
    // Configuration->CollisionDetectFilter = 0;
    // Configuration->CollisionDetectSource = 0;
    Configuration->MinimumFrameLength = 64;

    //
    // Set up the address filtering.
    //
    // First get hold of the combined packet filter.
    //

    if (Adapter->FilterDB != NULL) {

        PacketFilters = ETH_QUERY_FILTER_CLASSES(Adapter->FilterDB);

    } else {

        PacketFilters = 0;

    }

    if (PacketFilters != 0) {

        //
        // If *any* packets are to be received, then we must
        // enable MAC.BIN's packet reception.
        //

        Configuration->MacBinEnablePacketReception = 1;

    }

    if (PacketFilters & NDIS_PACKET_TYPE_PROMISCUOUS) {

        //
        // If one binding is promiscuous there is no point in
        // setting up any other filtering.  Every packet is
        // going to be accepted by the hardware.
        //

        Configuration->PromiscuousMode = 1;
        Configuration->MacBinPromiscuous = 1;

    } else if (PacketFilters & NDIS_PACKET_TYPE_ALL_MULTICAST) {

        //
        // No support for AllMulticast filter class!
        // We can probably simulate it by setting promiscuous mode
        // and letting the filter package do all the work
        //

        Configuration->PromiscuousMode = 1;
        Configuration->MacBinPromiscuous = 1;

    } else if (PacketFilters & NDIS_PACKET_TYPE_BROADCAST) {

        //
        // Enable broadcast packets.
        //

        Configuration->DisableBroadcast = 0;

    }

}

VOID
NE3200DoAdapterReset(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This is the resetting the adapter hardware.

    It makes the following assumptions:

    1) That the hardware has been stopped.

    2) That it can not be preempted.

    3) That no other adapter activity can occur.

    When this routine is finished all of the adapter information
    will be as if the driver was just initialized.

    NOTE: This assumes it is called with the lock held.

Arguments:

    Adapter - The adapter whose hardware is to be reset.

Return Value:

    not.

--*/
{

    //
    // Recover all of the adapter buffers.
    //

    {

        UINT i;

        for (
            i = 0;
            i < NE3200_NUMBER_OF_TRANSMIT_BUFFERS;
            i++
            ) {

            Adapter->NE3200Buffers[i].Next = i+1;

        }

        Adapter->NE3200BufferListHead = 0;
        Adapter->NE3200Buffers[NE3200_NUMBER_OF_TRANSMIT_BUFFERS-1].Next = -1;

    }

    //
    // Go through the various transmit lists and abort every packet.
    //

    {

        UINT i;
        PNDIS_PACKET Packet;
        PNE3200_RESERVED Reserved;
        PNE3200_OPEN Open;
        PNDIS_PACKET Next;

        for (
            i = 0;
            i < 3;
            i++
            ) {

            switch (i) {

                case 0:
                    Next = Adapter->FirstLoopBack;
                    break;
                case 1:
                    Next = Adapter->FirstFinishTransmit;
                    break;
                case 2:
                    Next = Adapter->FirstStage1Packet;
                    break;

            }


            while (Next) {

                Packet = Next;
                Reserved = PNE3200_RESERVED_FROM_PACKET(Packet);
                Next = Reserved->Next;
                Open =
                  PNE3200_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

                //
                // The completion of the packet is one less reason
                // to keep the open around.
                //

                ASSERT(Open->References);

                Open->References--;

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteSend(
                    Open->NdisBindingContext,
                    Packet,
                    NDIS_STATUS_REQUEST_ABORTED
                    );

                NdisAcquireSpinLock(&Adapter->Lock);

            }

        }
    }

    NE3200ResetVariables(Adapter);
    NE3200ResetCommandBlocks(Adapter);
    NE3200SetConfigurationBlockAndInit(Adapter);

}

STATIC
BOOLEAN
NE3200SetConfigurationBlockAndInit(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    It is this routine's responsibility to make sure that the
    Configuration block is filled and the adapter is initialized
    *but not* started.

    NOTE: This assumes it is called with the lock held.


Arguments:

    Adapter - The adapter whose hardware is to be initialized.

Return Value:

    If ResetAsynchoronous is FALSE, then returns TRUE if reset successful,
    FALSE if reset unsuccessful.

    If ResetAsynchoronous is TRUE, then always returns TRUE.

--*/
{

    //
    // Fill in the adapter's initialization block.
    //

    NE3200SetConfigurationBlock(Adapter);

    //
    // Set the initial state for the ResetDpc state machine.
    //

    Adapter->ResetState = NE3200ResetStateStarting;

    if (Adapter->ResetAsynchronous) {

        NdisReleaseSpinLock(&Adapter->Lock);

    }

    NE3200ResetHandler(NULL, Adapter, NULL, NULL);

    if (Adapter->ResetAsynchronous) {

        NdisAcquireSpinLock(&Adapter->Lock);

    }

    if (!Adapter->ResetAsynchronous) {

        return((Adapter->ResetResult == NE3200ResetResultSuccessful));

    } else {

        return(TRUE);

    }

}

VOID
NE3200ResetHandler(
    IN PVOID SystemSpecific1,
    IN PNE3200_ADAPTER Adapter,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )

/*++

Routine Description:

    This manages the reset/download process.  It is
    responsible for resetting the adapter, waiting for proper
    status, downloading MAC.BIN, waiting for MAC.BIN initialization,
    and optionally sending indications to the appropriate protocol.

    Since the NE3200's status registers must be polled during the
    reset/download process, this is implemented as a state machine.

Arguments:

    Adapter - The adapter whose hardware is to be initialized.

Return Value:

    None.

--*/
{

    //
    // Physical address of the MAC.BIN buffer.
    //
    NDIS_PHYSICAL_ADDRESS MacBinPhysicalAddress;

    //
    // Status from the adapter.
    //
    UCHAR Status;

    //
    // Simple iteration counter.
    //
    UINT i;

    if (Adapter->ResetAsynchronous) {

        NdisAcquireSpinLock(&Adapter->Lock);

    }

    while (Adapter->ResetState != NE3200ResetStateComplete) {

        switch (Adapter->ResetState) {

            case NE3200ResetStateStarting :

                //
                // Unfortunately, a hardware reset to the NE3200 does *not*
                // reset the BMIC chip.  To ensure that we read a proper status,
                // we'll clear all of the BMIC's registers.
                //

                NE3200_WRITE_SYSTEM_INTERRUPT(
                    Adapter,
                    0
                    );

                //
                // I changed this to ff since the original 0 didn't work for
                // some cases. since we don't have the specs....
                //

                NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
                    Adapter,
                    0xff
                    );

                NE3200_WRITE_SYSTEM_DOORBELL_MASK(
                    Adapter,
                    0
                    );

                NE3200_SYNC_CLEAR_SYSTEM_DOORBELL_INTERRUPT(
                    Adapter
                    );

#if 0
                NE3200_WRITE_SYSTEM_DOORBELL_INTERRUPT(
                    Adapter,
                    0
                    );

#endif

                for (i = 0 ; i < 16 ; i += 4 ) {

                    NE3200_WRITE_MAILBOX_ULONG(
                        Adapter,
                        i,
                        0L
                        );

                }

                //
                // Toggle the NE3200's reset line.
                //

                NE3200_WRITE_RESET(
                    Adapter,
                    NE3200_RESET_BIT_ON
                    );

                NE3200_WRITE_RESET(
                    Adapter,
                    NE3200_RESET_BIT_OFF
                    );

                //
                // Switch to the next state.
                //

                Adapter->ResetState = NE3200ResetStateResetting;
                Adapter->ResetTimeoutCounter = NE3200_TIMEOUT_RESET;

                //
                // Go to next state.
                //

                break;

            case NE3200ResetStateResetting :

                //
                // Read the status mailbox.
                //

                 NE3200_READ_MAILBOX_UCHAR(Adapter, NE3200_MAILBOX_RESET_STATUS, &Status);

                if (Status == NE3200_RESET_PASSED) {

                    //
                    // We have good reset.  Initiate the MAC.BIN download.
                    //

                    //
                    // The station address for this adapter can be forced to
                    // a specific value at initialization time.  When MAC.BIN
                    // first gets control, it reads mailbox 10.  If this mailbox
                    // contains a 0xFF, then the burned-in PROM station address
                    // is used.  If this mailbox contains any value other than
                    // 0xFF, then mailboxes 10-15 are read.  The six bytes
                    // stored in these mailboxes then become the station address.
                    //
                    // Since we have no need for this feature, we will always
                    // initialize mailbox 10 with a 0xFF.
                    //

                    NE3200_WRITE_MAILBOX_UCHAR(
                        Adapter,
                        NE3200_MAILBOX_STATION_ID,
                        0xFF
                        );


                    //
                    // Lock the MAC.BIN buffer.
                    //

                    MacBinPhysicalAddress = NE3200Globals.MacBinPhysicalAddress;

                    //
                    // Download MAC.BIN to the card.
                    //

                    NE3200_WRITE_MAILBOX_USHORT(
                        Adapter,
                        NE3200_MAILBOX_MACBIN_LENGTH,
                        NE3200Globals.MacBinLength
                        );

                    NE3200_WRITE_MAILBOX_UCHAR(
                        Adapter,
                        NE3200_MAILBOX_MACBIN_DOWNLOAD_MODE,
                        NE3200_MACBIN_DIRECT
                        );

                    NE3200_WRITE_MAILBOX_ULONG(
                        Adapter,
                        NE3200_MAILBOX_MACBIN_POINTER,
                        NdisGetPhysicalAddressLow(MacBinPhysicalAddress)
                        );

                    NE3200_WRITE_MAILBOX_USHORT(
                        Adapter,
                        NE3200_MAILBOX_MACBIN_TARGET,
                        NE3200_MACBIN_TARGET_ADDRESS >> 1
                        );

                    //
                    // This next OUT "kicks" the loader into action.
                    //

                    NE3200_WRITE_MAILBOX_UCHAR(
                        Adapter,
                        NE3200_MAILBOX_RESET_STATUS,
                        0
                        );

                    //
                    // Switch to the next state.
                    //

                    Adapter->ResetState = NE3200ResetStateDownloading;
                    Adapter->ResetTimeoutCounter = NE3200_TIMEOUT_DOWNLOAD;

                    //
                    // go to next state
                    //

                } else if (Status == NE3200_RESET_FAILED) {

                    //
                    // Reset failure.  Notify the authorities and
                    // next of kin.
                    //

                    Adapter->ResetResult = NE3200ResetResultResetFailure;
                    Adapter->ResetState = NE3200ResetStateComplete;

                    NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                } else {

                    //
                    // See if we've timed-out.
                    //

                    Adapter->ResetTimeoutCounter--;

                    if (Adapter->ResetTimeoutCounter == 0) {

                        //
                        // We've timed-out.  Bad news.
                        //

                        Adapter->ResetResult = NE3200ResetResultResetTimeout;
                        Adapter->ResetState = NE3200ResetStateComplete;

                        NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                    } else {

                        if (!Adapter->ResetAsynchronous) {

                            //
                            // Otherwise, wait and try again.
                            //

                            NdisStallExecution(100000);

                        } else{

                            NdisReleaseSpinLock(&Adapter->Lock);

                            NdisSetTimer(&Adapter->ResetTimer, 100);

                            return;

                        }

                    }

                }

                break;

            case NE3200ResetStateDownloading :

                //
                // Read the download status.
                //

                NE3200_READ_MAILBOX_UCHAR(Adapter, NE3200_MAILBOX_STATUS, &Status);

                if (Status == NE3200_INITIALIZATION_PASSED) {

                    //
                    // According to documentation from Compaq, this next port
                    // write will (in a future MAC.BIN) tell MAC.BIN whether or
                    // not to handle loopback internally.  This value is currently
                    // not used, but must still be written to the port.
                    //

                    NE3200_WRITE_MAILBOX_UCHAR(
                        Adapter,
                        NE3200_MAILBOX_STATUS,
                        1
                        );

                    //
                    // Initialization is good, the card is ready.
                    //

                    NE3200StartChipAndDisableInterrupts(Adapter,
                                                        Adapter->ReceiveQueueHead
                                                       );

                    if (!Adapter->FirstOpen) {

                        //
                        // Do the work for updating the mulitcast table
                        //

                        //
                        // This points to the open binding's private Command Block.
                        //
                        PNE3200_SUPER_COMMAND_BLOCK CommandBlock;

                        //
                        // This points to the adapter's configuration block.
                        //
                        PNE3200_CONFIGURATION_BLOCK ConfigurationBlock =
                            Adapter->ConfigurationBlock;

                        //
                        // See if we can acquire a private command block.
                        //

                        NE3200AcquirePublicCommandBlock(Adapter, &CommandBlock);

                        Adapter->ResetHandlerCommandBlock = CommandBlock;

                        //
                        // Setup the command block.
                        //

                        CommandBlock->NextCommand = NULL;

                        CommandBlock->OwningOpenBinding = NE3200_FAKE_OPEN;
                        CommandBlock->Hardware.State = NE3200_STATE_WAIT_FOR_ADAPTER;
                        CommandBlock->Hardware.Status = 0;
                        CommandBlock->Hardware.NextPending = NE3200_NULL;
                        CommandBlock->Hardware.CommandCode =
                            NE3200_COMMAND_CONFIGURE_82586;
                        CommandBlock->Hardware.PARAMETERS.CONFIGURE.ConfigurationBlock =
                            NdisGetPhysicalAddressLow(Adapter->ConfigurationBlockPhysical);

                        //
                        // Now that we've got the command block built,
                        // let's do it!
                        //

                        NE3200SubmitCommandBlock(Adapter, CommandBlock);

                        Adapter->ResetState = NE3200ResetStateReloadPacketFilters;
                        Adapter->ResetTimeoutCounter = NE3200_TIMEOUT_DOWNLOAD;

                    } else {

                        Adapter->ResetResult = NE3200ResetResultSuccessful;
                        Adapter->ResetState = NE3200ResetStateComplete;

                        NE3200DoResetIndications(Adapter, NDIS_STATUS_SUCCESS);

                    }

                } else if (Status == NE3200_INITIALIZATION_FAILED) {

                    //
                    // Initialization failed.
                    //

                    Adapter->ResetResult = NE3200ResetResultInitializationFailure;
                    Adapter->ResetState = NE3200ResetStateComplete;

                    NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                } else {

                    //
                    // See if we've timed-out.
                    //

                    Adapter->ResetTimeoutCounter--;

                    if (Adapter->ResetTimeoutCounter == 0) {

                        //
                        // We've timed-out.  Bad news.
                        //

                        Adapter->ResetResult = NE3200ResetResultInitializationTimeout;
                        Adapter->ResetState = NE3200ResetStateComplete;

                        NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                    } else {

                        if (!Adapter->ResetAsynchronous) {

                            //
                            // Otherwise, wait and try again.
                            //

                            NdisStallExecution(100000);

                        } else{

                            NdisReleaseSpinLock(&Adapter->Lock);

                            NdisSetTimer(&Adapter->ResetTimer, 100);
                            return;

                        }

                    }

                }

                break;

            case NE3200ResetStateReloadPacketFilters :

                //
                // Read the command block status.
                //

                if (Adapter->ResetHandlerCommandBlock->Hardware.State ==
                    NE3200_STATE_EXECUTION_COMPLETE) {

                    //
                    // current packet filters
                    //
                    UINT PacketFilters;

                    //
                    // return this command block
                    //
                    NE3200RelinquishCommandBlock(Adapter,
                                                 Adapter->ResetHandlerCommandBlock
                                                );

                    Adapter->ResetHandlerCommandBlock = NULL;

                    PacketFilters = ETH_QUERY_FILTER_CLASSES(Adapter->FilterDB);

                    //
                    // Modify the card multicast addresses if needed.
                    //

                    if (PacketFilters & NDIS_PACKET_TYPE_MULTICAST) {

                        UINT CurrentAddressCount;
                        PUCHAR CurrentAddresses;
                        NDIS_STATUS Status;

                        NE3200_ALLOC_PHYS(
                            &Status,
                            &CurrentAddresses,
                            ETH_LENGTH_OF_ADDRESS *
                            NE3200_MAXIMUM_MULTICAST
                            );

                        if (Status != NDIS_STATUS_SUCCESS) {

                            //
                            // Fail
                            //

                            Adapter->ResetResult = NE3200ResetResultResources;
                            Adapter->ResetState = NE3200ResetStateComplete;

                            NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                            break;

                        }

                        EthQueryGlobalFilterAddresses(
                                                &Status,
                                                Adapter->FilterDB,
                                                ETH_LENGTH_OF_ADDRESS * NE3200_MAXIMUM_MULTICAST,
                                                &CurrentAddressCount,
                                                (PVOID)CurrentAddresses
                                                );

                        if (Status == NDIS_STATUS_SUCCESS) {

                             //
                             // Holds the status that should be returned to the filtering package.
                             //
                             NDIS_STATUS StatusOfUpdate;

                             //
                             // Multicast address table
                             //
                             PUCHAR MulticastAddressTable;

                             PNE3200_SUPER_COMMAND_BLOCK CommandBlock;

                             //
                             // See if we can acquire a private command block.
                             //

                             NE3200AcquirePublicCommandBlock(Adapter, &CommandBlock);

                             Adapter->ResetHandlerCommandBlock = CommandBlock;

                             //
                             // Store the request that uses this CB
                             //
                             CommandBlock->Set = FALSE;

                             MulticastAddressTable = Adapter->CardMulticastTable;

                             NdisZeroMemory(
                                     MulticastAddressTable,
                                     CurrentAddressCount * NE3200_SIZE_OF_MULTICAST_TABLE_ENTRY
                                     );

                             {

                                 //
                                 // Simple iteration counter.
                                 //
                                 UINT i;

                                 //
                                 // Pointer into the multicast address table.
                                 //
                                 PCHAR OriginalAddress;

                                 //
                                 // Pointer into our temporary buffer.
                                 //
                                 PCHAR MungedAddress;

                                 //
                                 // Munge the address to 16 bytes per entry.
                                 //

                                 OriginalAddress = CurrentAddresses;
                                 MungedAddress = MulticastAddressTable;

                                 for ( i = CurrentAddressCount ; i > 0 ; i-- ) {

                                     ETH_COPY_NETWORK_ADDRESS(
                                         MungedAddress,
                                         OriginalAddress
                                         );

                                     OriginalAddress += ETH_LENGTH_OF_ADDRESS;
                                     MungedAddress += NE3200_SIZE_OF_MULTICAST_TABLE_ENTRY;

                                 }


                                 //
                                 // Setup the command block.
                                 //

                                 CommandBlock->OwningOpenBinding = NE3200_FAKE_OPEN;
                                 CommandBlock->NextCommand = NULL;

                                 CommandBlock->Hardware.State =
                                     NE3200_STATE_WAIT_FOR_ADAPTER;
                                 CommandBlock->Hardware.Status = 0;
                                 CommandBlock->Hardware.NextPending = NE3200_NULL;
                                 CommandBlock->Hardware.CommandCode =
                                     NE3200_COMMAND_SET_MULTICAST_ADDRESS;
                                 CommandBlock->Hardware.PARAMETERS.MULTICAST.NumberOfMulticastAddresses =
                                     (USHORT)CurrentAddressCount;

                                 if (CurrentAddressCount == 0) {

                                     CommandBlock->Hardware.PARAMETERS.MULTICAST.MulticastAddressTable =
                                         (NE3200_PHYSICAL_ADDRESS)NULL;

                                 } else {

                                     CommandBlock->Hardware.PARAMETERS.MULTICAST.MulticastAddressTable =
                                         NdisGetPhysicalAddressLow(Adapter->CardMulticastTablePhysical);

                                 }

                                 //
                                 // Now that we're set up, let's do it!
                                 //

                                 NE3200SubmitCommandBlock(Adapter, CommandBlock);

                                 //
                                 // Move to next stage
                                 //

                                 Adapter->ResetState = NE3200ResetStateReloadMulticast;
                                 Adapter->ResetTimeoutCounter = NE3200_TIMEOUT_DOWNLOAD;

                             }

                        } else {

                            //
                            // Move to next stage
                            //

                            Adapter->ResetState = NE3200ResetStateReloadMulticast;
                            Adapter->ResetTimeoutCounter = NE3200_TIMEOUT_DOWNLOAD;

                        }

                        NE3200_FREE_PHYS(CurrentAddresses);

                    } else {

                        //
                        // Move to next stage
                        //

                        Adapter->ResetState = NE3200ResetStateReloadMulticast;
                        Adapter->ResetTimeoutCounter = NE3200_TIMEOUT_DOWNLOAD;

                    }

                } else {

                    //
                    // See if we've timed-out.
                    //

                    Adapter->ResetTimeoutCounter--;

                    if (Adapter->ResetTimeoutCounter == 0) {

                        //
                        // We've timed-out.  Bad news.
                        //

                        Adapter->ResetResult = NE3200ResetResultInitializationTimeout;
                        Adapter->ResetState = NE3200ResetStateComplete;

                        NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                    } else {

                        if (!Adapter->ResetAsynchronous) {

                            //
                            // Otherwise, wait and try again.
                            //

                            NdisStallExecution(100000);

                        } else{

                            NdisReleaseSpinLock(&Adapter->Lock);

                            NdisSetTimer(&Adapter->ResetTimer, 100);
                            return;

                        }

                    }

                }

                break;

            case NE3200ResetStateReloadMulticast :

                //
                // Read the command block status.
                //

                if ((Adapter->ResetHandlerCommandBlock == NULL) ||
                    (Adapter->ResetHandlerCommandBlock->Hardware.State == NE3200_STATE_EXECUTION_COMPLETE)) {

                    if (Adapter->ResetHandlerCommandBlock != NULL) {

                        //
                        // return this command block
                        //
                        NE3200RelinquishCommandBlock(Adapter,
                                                 Adapter->ResetHandlerCommandBlock
                                                );

                    }

                    //
                    // Modify the card address if needed
                    //

                    if (Adapter->AddressChanged) {

                        PNE3200_SUPER_COMMAND_BLOCK CommandBlock;
                        UINT i;

                        NE3200AcquirePublicCommandBlock(Adapter, &CommandBlock);

                        Adapter->ResetHandlerCommandBlock = CommandBlock;

                        //
                        // Setup the command block.
                        //

                        CommandBlock->NextCommand = NULL;

                        CommandBlock->OwningOpenBinding = NE3200_FAKE_OPEN;
                        CommandBlock->Hardware.State = NE3200_STATE_WAIT_FOR_ADAPTER;
                        CommandBlock->Hardware.Status = 0;
                        CommandBlock->Hardware.NextPending = NE3200_NULL;
                        CommandBlock->Hardware.CommandCode =
                            NE3200_COMMAND_SET_STATION_ADDRESS;

                        ETH_COPY_NETWORK_ADDRESS(
                            CommandBlock->Hardware.PARAMETERS.SET_ADDRESS.NewStationAddress,
                            Adapter->CurrentAddress
                            );

                        //
                        // Now that we've got the command block built,
                        // let's do it!
                        //

                        NE3200SubmitCommandBlock(Adapter, CommandBlock);

                        Adapter->ResetState = NE3200ResetStateReloadAddress;
                        Adapter->ResetTimeoutCounter = NE3200_TIMEOUT_DOWNLOAD;

                    } else {

                        //
                        // We are done
                        //

                        Adapter->ResetResult = NE3200ResetResultSuccessful;
                        Adapter->ResetState = NE3200ResetStateComplete;

                        NE3200DoResetIndications(Adapter, NDIS_STATUS_SUCCESS);

                    }

                } else {

                    //
                    // See if we've timed-out.
                    //

                    Adapter->ResetTimeoutCounter--;

                    if (Adapter->ResetTimeoutCounter == 0) {

                        //
                        // We've timed-out.  Bad news.
                        //

                        Adapter->ResetResult = NE3200ResetResultInitializationTimeout;
                        Adapter->ResetState = NE3200ResetStateComplete;

                        NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                    } else {

                        if (!Adapter->ResetAsynchronous) {

                            //
                            // Otherwise, wait and try again.
                            //

                            NdisStallExecution(100000);

                        } else{

                            NdisReleaseSpinLock(&Adapter->Lock);

                            NdisSetTimer(&Adapter->ResetTimer, 100);
                            return;

                        }

                    }

                }

                break;

            case NE3200ResetStateReloadAddress :

                //
                // Read the command block status.
                //

                if (Adapter->ResetHandlerCommandBlock->Hardware.State ==
                    NE3200_STATE_EXECUTION_COMPLETE) {

                    //
                    // return this command block
                    //
                    NE3200RelinquishCommandBlock(Adapter,
                                                 Adapter->ResetHandlerCommandBlock
                                                );

                    //
                    // Reset is complete.  Do those indications.
                    //
                    Adapter->ResetResult = NE3200ResetResultSuccessful;
                    Adapter->ResetState = NE3200ResetStateComplete;

                    NE3200DoResetIndications(Adapter, NDIS_STATUS_SUCCESS);

                } else {

                    //
                    // See if we've timed-out.
                    //

                    Adapter->ResetTimeoutCounter--;

                    if (Adapter->ResetTimeoutCounter == 0) {

                        //
                        // We've timed-out.  Bad news.
                        //

                        Adapter->ResetResult = NE3200ResetResultInitializationTimeout;
                        Adapter->ResetState = NE3200ResetStateComplete;

                        NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                    } else {

                        if (!Adapter->ResetAsynchronous) {

                            //
                            // Otherwise, wait and try again.
                            //

                            NdisStallExecution(100000);

                        } else{

                            NdisReleaseSpinLock(&Adapter->Lock);

                            NdisSetTimer(&Adapter->ResetTimer, 100);
                            return;

                        }

                    }

                }

                break;

            default :

                //
                // Somehow, we reached an invalid state.
                //

                //
                // We'll try to salvage our way out of this.
                //

                Adapter->ResetResult = NE3200ResetResultInvalidState;
                Adapter->ResetState = NE3200ResetStateComplete;

                NE3200DoResetIndications(Adapter, NDIS_STATUS_HARD_ERRORS);

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_HARDWARE_FAILURE,
                    3,
                    resetDpc,
                    NE3200_ERRMSG_BAD_STATE,
                    (ULONG)(Adapter->ResetState)
                    );

                break;
        }

    }

    if (Adapter->ResetAsynchronous) {

        NdisReleaseSpinLock(&Adapter->Lock);

    }

}

STATIC
VOID
NE3200DoResetIndications(
    IN PNE3200_ADAPTER Adapter,
    IN NDIS_STATUS Status
    )

/*++

Routine Description:

    This routine is called by NE3200ResetDpc to perform any
    indications which need to be done after a reset.  Note that
    this routine will be called after either a successful reset
    or a failed reset.

    NOTE: This routine assumes it is called with the lock held.

Arguments:

    Adapter - The adapter whose hardware has been initialized.

    Status - The status of the reset to send to the protocol(s).

Return Value:

    None.

--*/
{
    //
    // This will point (possibly null) to the open that
    // initiated the reset.
    //
    PNE3200_OPEN ResettingOpen;

    //
    // We save off the open that caused this reset incase
    // we get *another* reset while we're indicating the
    // last reset is done.
    //

    ResettingOpen = Adapter->ResettingOpen;

    //
    // Reinitialize multicasts and filter
    //

    if (Status == NDIS_STATUS_SUCCESS) {
        NE3200EnableInterrupts(Adapter);
    } else {
        NE3200StopChip(Adapter, FALSE);
    }

    Adapter->ResetInProgress = FALSE;

    //
    // We need to signal every open binding that the
    // reset is complete.  We increment the reference
    // count on the open binding while we're doing indications
    // so that the open can't be deleted out from under
    // us while we're indicating (recall that we can't own
    // the lock during the indication).
    //

    {

        PNE3200_OPEN Open;
        PLIST_ENTRY CurrentLink;

        CurrentLink = Adapter->OpenBindings.Flink;

        while (CurrentLink != &Adapter->OpenBindings) {

            Open = CONTAINING_RECORD(
                     CurrentLink,
                     NE3200_OPEN,
                     OpenList
                     );

            Open->References++;

            NdisReleaseSpinLock(&Adapter->Lock);

            if (Status != NDIS_STATUS_SUCCESS) {

                //
                // The card is now hosed.  Tell everyone to close down.
                //

                NdisIndicateStatus(
                    Open->NdisBindingContext,
                    NDIS_STATUS_CLOSED,
                    NULL,
                    0
                    );

            }

            NdisIndicateStatus(
                Open->NdisBindingContext,
                NDIS_STATUS_RESET_END,
                &Status,
                sizeof(Status)
                );

            NdisIndicateStatusComplete(Open->NdisBindingContext);

            NdisAcquireSpinLock(&Adapter->Lock);

            Open->References--;

            CurrentLink = CurrentLink->Flink;

        }


        //
        // Look to see which open initiated the reset.
        //
        // If the reset was initiated for some obscure hardware
        // reason that can't be associated with a particular
        // open (e.g. memory error on receiving a packet) then
        // we won't have an initiating request so we can't
        // indicate.  (The ResettingOpen pointer will be
        // NULL in this case.)
        //

        if (ResettingOpen != NULL) {

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisCompleteReset(
                ResettingOpen->NdisBindingContext,
                Status
                );

            NdisAcquireSpinLock(&Adapter->Lock);

            ResettingOpen->References--;

        }

    }

}

extern
VOID
NE3200SetupForReset(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open
    )

/*++

Routine Description:

    This routine is used to fill in the who and why a reset is
    being set up as well as setting the appropriate fields in the
    adapter.

    NOTE: This routine must be called with the lock acquired.

Arguments:

    Adapter - The adapter whose hardware is to be initialized.

    Open - A (possibly NULL) pointer to an ne3200 open structure.
    The reason it could be null is if the adapter is initiating the
    reset on its own.

Return Value:

    None.

--*/
{

    BOOLEAN Cancelled;
    PNDIS_REQUEST CurrentRequest;
    PNDIS_REQUEST * CurrentNextLocation;
    PNE3200_OPEN TmpOpen;

    PNE3200_REQUEST_RESERVED Reserved;

    Adapter->ResetInProgress = TRUE;

    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete.
    //

    NE3200StopChip(Adapter, TRUE);

    //
    // Shut down all of the transmit queues so that the
    // transmit portion of the chip will eventually calm down.
    //

    Adapter->Stage2Open = FALSE;

    //
    // Set default reset method
    //
    Adapter->ResetAsynchronous = FALSE;

    //
    // If there is a close at the top of the queue, then
    // it may be in two states:
    //
    // 1- Has interrupted, and the InterruptDpc got the
    // interrupt out of Adapter->IsrValue before we zeroed it.
    //
    // 2- Has interrupted, but we zeroed Adapter->IsrValue
    // before it read it, OR has not yet interrupted.
    //
    // In case 1, the interrupt will be processed and the
    // close will complete without our intervention. In
    // case 2, the open will not complete. In that case
    // the CAM will have been updated for that open, so
    // all that remains is for us to dereference the open
    // as would have been done in the interrupt handler.
    //
    // Closes that are not at the top of the queue we
    // leave in place; when we restart the queue after
    // the reset, they will get processed.
    //

    NdisReleaseSpinLock(&Adapter->Lock);

    NdisCancelTimer(&Adapter->WakeUpTimer,&Cancelled);

    NdisAcquireSpinLock(&Adapter->Lock);

    CurrentRequest = Adapter->FirstRequest;

    if (CurrentRequest) {

        Reserved = PNE3200_RESERVED_FROM_REQUEST(CurrentRequest);

        //
        // If the first request is a close, take it off the
        // queue, and "complete" it.
        //

        if (CurrentRequest->RequestType == NdisRequestClose) {
            Adapter->FirstRequest = Reserved->Next;
            --(Reserved->OpenBlock)->References;
            CurrentRequest = Adapter->FirstRequest;
        }

        CurrentNextLocation = &(Adapter->FirstRequest);

        while (CurrentRequest) {

            Reserved = PNE3200_RESERVED_FROM_REQUEST(CurrentRequest);

            if ((CurrentRequest->RequestType == NdisRequestClose) ||
                (CurrentRequest->RequestType == NdisRequestOpen)) {

                //
                // Opens are inoffensive, we just leave them
                // on the list. Closes that were not at the
                // head of the list were not processing and
                // can be left on also.
                //

                CurrentNextLocation = &(Reserved->Next);

            } else {

                //
                // Not a close, remove it from the list and
                // fail it.
                //

                *CurrentNextLocation = Reserved->Next;
                TmpOpen = Reserved->OpenBlock;

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteRequest(
                    TmpOpen->NdisBindingContext,
                    CurrentRequest,
                    NDIS_STATUS_REQUEST_ABORTED
                    );

                NdisAcquireSpinLock(&Adapter->Lock);

                TmpOpen->References--;

            }

            CurrentRequest = *CurrentNextLocation;
        }

    }

    Adapter->ResettingOpen = Open;

    //
    // If there is a valid open we should up the reference count
    // so that the open can't be deleted before we indicate that
    // their request is finished.
    //

    if (Open) {

        Open->References++;

    }

}


#ifdef NDIS_WIN
    #pragma ICODE
#endif

VOID
NE3200GetStationAddress(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine gets the network address from the hardware.

    NOTE: This routine assumes that it is called *immediately*
    after MAC.BIN has been downloaded.  It should only be called
    immediately after SetConfigurationBlockAndInit() has completed.

Arguments:

    Adapter - Where to store the network address.

Return Value:

    None.

--*/

{

    UINT Result;

    NE3200_READ_MAILBOX_UCHAR(
                            Adapter,
                            NE3200_MAILBOX_STATION_ID,
                            &Adapter->NetworkAddress[0]
                            );

    NE3200_READ_MAILBOX_UCHAR(
                            Adapter,
                            NE3200_MAILBOX_STATION_ID + 1,
                            &Adapter->NetworkAddress[1]
                            );
    NE3200_READ_MAILBOX_UCHAR(
                            Adapter,
                            NE3200_MAILBOX_STATION_ID + 2,
                            &Adapter->NetworkAddress[2]
                            );
    NE3200_READ_MAILBOX_UCHAR(
                            Adapter,
                            NE3200_MAILBOX_STATION_ID + 3,
                            &Adapter->NetworkAddress[3]
                            );
    NE3200_READ_MAILBOX_UCHAR(
                            Adapter,
                            NE3200_MAILBOX_STATION_ID + 4,
                            &Adapter->NetworkAddress[4]
                            );
    NE3200_READ_MAILBOX_UCHAR(
                            Adapter,
                            NE3200_MAILBOX_STATION_ID +5,
                            &Adapter->NetworkAddress[5]
                            );

    if (!Adapter->AddressChanged) {
        ETH_COPY_NETWORK_ADDRESS(
                Adapter->CurrentAddress,
                Adapter->NetworkAddress
                );
    } else {
        //
        // Is it really different ?
        //

        ETH_COMPARE_NETWORK_ADDRESSES(
            Adapter->CurrentAddress,
            Adapter->NetworkAddress,
            &Result
            );

        if (Result == 0) {

            //
            // No change, really.
            //
            Adapter->AddressChanged = FALSE;
        }
    }
}
#ifdef NDIS_WIN
    #pragma LCODE
#endif

VOID
NE3200ResetVariables(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine sets variables to their proper value after a reset.

Arguments:

    Adapter - Adapter we are resetting.

Return Value:

    None.

--*/

{
    Adapter->FirstLoopBack = NULL;
    Adapter->LastLoopBack = NULL;
    Adapter->FirstFinishTransmit = NULL;
    Adapter->LastFinishTransmit = NULL;
    Adapter->FirstStage1Packet = NULL;
    Adapter->LastStage1Packet = NULL;

    Adapter->Stage2Open = TRUE;

    Adapter->AlreadyProcessingStage2 = FALSE;

    Adapter->FirstCommandOnCard = NULL;
    Adapter->LastCommandOnCard = NULL;
    Adapter->FirstWaitingCommand = NULL;
    Adapter->LastWaitingCommand = NULL;
    Adapter->ReceiveQueueHead = Adapter->ReceiveQueue;
    Adapter->ReceiveQueueTail =
        Adapter->ReceiveQueue + Adapter->NumberOfReceiveBuffers - 1;
    Adapter->NumberOfAvailableCommandBlocks = Adapter->NumberOfCommandBlocks;
    Adapter->NextCommandBlock = Adapter->CommandQueue;

    Adapter->TransmitsQueued = 0;
    Adapter->CurrentReceiveIndex = 0;

}

VOID
NE3200ResetCommandBlocks(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine sets command block elementsto their proper value after a reset.

Arguments:

    Adapter - Adapter we are resetting.

Return Value:

    None.

--*/

{

    //
    // Pointer to a Receive Entry.  Used while initializing
    // the Receive Queue.
    //
    PNE3200_SUPER_RECEIVE_ENTRY CurrentReceiveEntry;

    //
    // Pointer to a Command Block.  Used while initializing
    // the Command Queue.
    //
    PNE3200_SUPER_COMMAND_BLOCK CurrentCommandBlock;

    //
    // Simple iteration variable.
    //
    UINT i;

    //
    // Put the Command Blocks into a known state.
    //

    for(
        i = 0, CurrentCommandBlock = Adapter->CommandQueue;
        i < Adapter->NumberOfCommandBlocks;
        i++, CurrentCommandBlock++
        ) {

        CurrentCommandBlock->Hardware.State = NE3200_STATE_FREE;
        CurrentCommandBlock->Hardware.NextPending = NE3200_NULL;

        CurrentCommandBlock->NextCommand = NULL;
        CurrentCommandBlock->OwningOpenBinding = NULL;
        CurrentCommandBlock->Timeout = FALSE;
    }

    //
    // Allocate the receive buffers and attach them to the Receive
    // Queue entries.
    //

    for(
        i = 0, CurrentReceiveEntry = Adapter->ReceiveQueue;
        i < Adapter->NumberOfReceiveBuffers;
        i++, CurrentReceiveEntry++
        ) {


        //
        // Initialize receive buffers
        //

        CurrentReceiveEntry->Hardware.State = NE3200_STATE_FREE;
        CurrentReceiveEntry->Hardware.NextPending =
                NdisGetPhysicalAddressLow(Adapter->ReceiveQueuePhysical) +
                (i + 1) * sizeof(NE3200_SUPER_RECEIVE_ENTRY);
        CurrentReceiveEntry->NextEntry = CurrentReceiveEntry + 1;

    }

    //
    // Make sure the last entry is properly terminated.
    //

    (CurrentReceiveEntry - 1)->Hardware.NextPending = NE3200_NULL;
    (CurrentReceiveEntry - 1)->NextEntry = Adapter->ReceiveQueue;

}


NDIS_STATUS
NE3200ChangeCurrentAddress(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is used to modify the card address.

Arguments:

    Adapter - The adapter for the NE3200 to change address.

Return Value:

    NDIS_STATUS_SUCCESS, if everything went ok
    NDIS_STATUS_FAILURE, otherwise

--*/
{

    //
    // Modify the card address if needed
    //

    if (Adapter->AddressChanged) {

        PNE3200_SUPER_COMMAND_BLOCK CommandBlock;
        UINT i;

        NE3200AcquirePublicCommandBlock(Adapter, &CommandBlock);

        //
        // Setup the command block.
        //

        CommandBlock->NextCommand = NULL;

        CommandBlock->OwningOpenBinding = NE3200_FAKE_OPEN;
        CommandBlock->Hardware.State = NE3200_STATE_WAIT_FOR_ADAPTER;
        CommandBlock->Hardware.Status = 0;
        CommandBlock->Hardware.NextPending = NE3200_NULL;
        CommandBlock->Hardware.CommandCode =
            NE3200_COMMAND_SET_STATION_ADDRESS;

        ETH_COPY_NETWORK_ADDRESS(
            CommandBlock->Hardware.PARAMETERS.SET_ADDRESS.NewStationAddress,
            Adapter->CurrentAddress
            );

        //
        // Now that we've got the command block built,
        // let's do it!
        //

        NE3200SubmitCommandBlock(Adapter, CommandBlock);

        //
        // Wait for the command block to finish
        //

        for (i = 0; i < 100; i++) {
            NdisStallExecution(100000);
            if (CommandBlock->Hardware.State == NE3200_STATE_EXECUTION_COMPLETE) {
                break;
            }
        }

        if (CommandBlock->Hardware.State != NE3200_STATE_EXECUTION_COMPLETE) {

            //
            // Failed
            //

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_HARDWARE_FAILURE,
                0
                );

            return NDIS_STATUS_FAILURE;

        }

        //
        // return this command block
        //
        NE3200RelinquishCommandBlock(Adapter, CommandBlock);

    }
    return NDIS_STATUS_SUCCESS;
}
