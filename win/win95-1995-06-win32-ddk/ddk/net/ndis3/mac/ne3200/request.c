/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    request.c

Abstract:

    This file contains code to implement MacRequest and
    MacQueryGlobalStatistics. This driver conforms to the
    NDIS 3.0 interface.

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


extern
NDIS_STATUS
NE3200QueryInformation(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN UINT InformationBufferLength,
    IN PUINT BytesWritten,
    IN PUINT BytesNeeded
    );

extern
NDIS_STATUS
NE3200SetInformation(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN INT InformationBufferLength,
    IN PUINT BytesRead,
    IN PUINT BytesNeeded
    );

extern
VOID
NE3200QueueRequest(
    IN PNE3200_ADAPTER Adapter,
    IN PNDIS_REQUEST NdisRequest
    );

extern
VOID
NE3200RemoveAdapter(
    IN NDIS_HANDLE MacAdapterContext
    );

extern
VOID
NE3200ProcessRequestQueue(
    IN PNE3200_ADAPTER Adapter,
    IN BOOLEAN StatisticsUpdated
    );

extern
NDIS_STATUS
NE3200ChangeClass(
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    )

/*++

Routine Description:

    Action routine that will get called when a particular filter
    class is first used or last cleared.

    NOTE: This routine assumes that it is called with the lock
    acquired.

Arguments:

    OldFilterClasses - The values of the class filter before it
    was changed.

    NewFilterClasses - The current value of the class filter

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE3200_OPEN.

    RequestHandle - A value supplied by the NDIS interface that the MAC
    must use when completing this request with the NdisCompleteRequest
    service, if the MAC completes this request asynchronously.

    Set - If true the change resulted from a set, otherwise the
    change resulted from a open closing.

Return Value:

    None.


--*/

{


    PNE3200_ADAPTER Adapter = PNE3200_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Holds the change that should be returned to the filtering package.
    //
    NDIS_STATUS StatusOfChange;

    NdisRequest;

    if (Adapter->ResetInProgress) {

        StatusOfChange = NDIS_STATUS_RESET_IN_PROGRESS;

    } else {

        //
        // This local will hold the actual changes that occurred
        // in the packet filtering that are of real interest.
        //
        UINT PacketChanges;


        //
        // The whole purpose of this routine is to determine whether
        // the filtering changes need to result in the hardware being
        // reset.
        //

        ASSERT(OldFilterClasses != NewFilterClasses);

        //
        // The NE3200 has no method for easily disabling multicast
        // packets.  Therefore, we'll only reconfigure the 82586
        // when there is a change in either directed, broadcast, or
        // promiscuous filtering.
        //

        PacketChanges = (OldFilterClasses ^ NewFilterClasses) &
                        (NDIS_PACKET_TYPE_PROMISCUOUS |
                         NDIS_PACKET_TYPE_BROADCAST |
                         NDIS_PACKET_TYPE_DIRECTED);

        StatusOfChange = NDIS_STATUS_SUCCESS;

        if (PacketChanges) {

            //
            // This points to the open binding initiating this request.
            //
            PNE3200_OPEN Open = PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

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
            // Use the generic command block
            //

            IF_LOG('F');

            NE3200AcquirePublicCommandBlock(
                                Adapter,
                                &CommandBlock
                                );

            //
            // Is this from a set?
            //

            CommandBlock->Set = Set;

            //
            // Setup the command block.
            //

            CommandBlock->OwningOpenBinding = Open;
            CommandBlock->NextCommand = NULL;

            CommandBlock->Hardware.State = NE3200_STATE_WAIT_FOR_ADAPTER;
            CommandBlock->Hardware.Status = 0;
            CommandBlock->Hardware.NextPending = NE3200_NULL;
            CommandBlock->Hardware.CommandCode =
                    NE3200_COMMAND_CONFIGURE_82586;
            CommandBlock->Hardware.PARAMETERS.CONFIGURE.ConfigurationBlock =
                    NdisGetPhysicalAddressLow(Adapter->ConfigurationBlockPhysical);

            //
            // Update the configuration block to reflect the new
            // packet filtering.
            //

            Adapter->CurrentPacketFilter = NewFilterClasses;

            if (NewFilterClasses == 0) {

                ConfigurationBlock->PromiscuousMode = 0;
                ConfigurationBlock->MacBinPromiscuous = 0;
                ConfigurationBlock->DisableBroadcast = 1;
//                ConfigurationBlock->MacBinEnablePacketReception = 0;

            } else {

                ConfigurationBlock->MacBinEnablePacketReception = 1;

                if (PacketChanges & NDIS_PACKET_TYPE_PROMISCUOUS) {

                    ConfigurationBlock->PromiscuousMode ^= 1;
                    ConfigurationBlock->MacBinPromiscuous ^= 1;

                }

                if (PacketChanges & NDIS_PACKET_TYPE_BROADCAST) {

                    ConfigurationBlock->DisableBroadcast ^= 1;

                }

            }

            //
            // Now that we've got the command block built,
            // let's do it!
            //

            NE3200SubmitCommandBlock(Adapter, CommandBlock);

            StatusOfChange = NDIS_STATUS_PENDING;

        }

    }
    return StatusOfChange;
}

extern
NDIS_STATUS
NE3200ChangeAddresses(
    IN UINT OldAddressCount,
    IN CHAR OldAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN UINT NewAddressCount,
    IN CHAR NewAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    )

/*++

Routine Description:

    Action routine that will get called when the multicast address
    list has changed.

    NOTE: This routine assumes that it is called with the lock
    acquired.

Arguments:

    OldAddressCount - The number of addresses in OldAddresses.

    OldAddresses - The old multicast address list.

    NewAddressCount - The number of addresses in NewAddresses.

    NewAddresses - The new multicast address list.

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE3200_OPEN.

    RequestHandle - A value supplied by the NDIS interface that the MAC
    must use when completing this request with the NdisCompleteRequest
    service, if the MAC completes this request asynchronously.

    Set - If true the change resulted from a set, otherwise the
    change resulted from a open closing.

Return Value:

    None.


--*/

{


    PNE3200_ADAPTER Adapter = PNE3200_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // The open that made this request.
    //
    PNE3200_OPEN Open = PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Holds the change that should be returned to the filtering package.
    //
    NDIS_STATUS StatusOfChange;

    OldAddressCount; OldAddresses; NdisRequest;

    if (Adapter->ResetInProgress) {

        StatusOfChange = NDIS_STATUS_RESET_IN_PROGRESS;

    } else {

        //
        // we are referencing this open
        //

        StatusOfChange = NE3200UpdateMulticastTable(
                                                Adapter,
                                                Open,
                                                NewAddressCount,
                                                NewAddresses,
                                                Set
                                                );

    }

    return StatusOfChange;

}

STATIC
VOID
NE3200CloseAction(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    Action routine that will get called when a particular binding
    was closed while it was indicating through NdisIndicateReceive

    All this routine needs to do is to decrement the reference count
    of the binding.

    NOTE: This routine assumes that it is called with the lock acquired.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE3200_OPEN.

Return Value:

    None.


--*/

{
    PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)->References--;
}



NDIS_STATUS
NE3200Request(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    The NE3200Request function handles general requests from the
    protocol. Currently these include SetInformation and
    QueryInformation, more may be added in the future.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE3200_OPEN.

    NdisRequest - A structure describing the request. In the case
    of asynchronous completion, this pointer will be used to
    identify the request that is completing.

Return Value:

    The function value is the status of the operation.


--*/

{
    //
    // This holds the status we will return.
    //

    NDIS_STATUS StatusOfRequest;

    //
    // Points to the adapter that this request is coming through.
    //
    PNE3200_ADAPTER Adapter;

    //
    // Pts to the reserved section of the request
    //
    PNE3200_REQUEST_RESERVED Reserved = PNE3200_RESERVED_FROM_REQUEST(NdisRequest);

    Adapter = PNE3200_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);
    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    IF_LOG('[');

    if (!Adapter->ResetInProgress) {

        PNE3200_OPEN Open;

        Open = PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

        if (!Open->BindingShuttingDown) {

            switch (NdisRequest->RequestType) {

            case NdisRequestSetInformation:
            case NdisRequestQueryInformation:

                //
                // This is a valid request, queue it.
                //

                Open->References++;

                Reserved->OpenBlock = Open;
                Reserved->Next = (PNDIS_REQUEST)NULL;

                NE3200QueueRequest (Adapter, NdisRequest);

                StatusOfRequest = NDIS_STATUS_PENDING;
                break;

            default:

                //
                // Unknown request
                //

                StatusOfRequest = NDIS_STATUS_NOT_SUPPORTED;
                break;

            }

        } else {

            StatusOfRequest = NDIS_STATUS_CLOSING;

        }

    } else {

        StatusOfRequest = NDIS_STATUS_RESET_IN_PROGRESS;

    }


    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    NE3200_DO_DEFERRED(Adapter);
    return StatusOfRequest;
}

extern
VOID
NE3200QueueRequest(
    IN PNE3200_ADAPTER Adapter,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    NE3200QueueRequest takes an NDIS_REQUEST and ensures that it
    gets processed and completed. It processes the
    request immediately if nothing else is in progress, otherwise
    it queues it for later processing.

    SpinLock assumed held.
Arguments:

    Adapter - The adapter that the request is for.

    NdisRequest - The NDIS_REQUEST structure describing the request.
        The NE3200Reserved section is partially filled in, except
        for the queueing and current offset fields.

Return Value:

    NDIS_STATUS_PENDING if the request was queued.
    Otherwise, the return code from NE3200ProcessRequestQueue.
    This will be NDIS_STATUS_PENDING if the request was queued
    to the adapter, otherwise the status of the request.


--*/

{

    //
    // Queue the request.
    //

    if (Adapter->FirstRequest != (PNDIS_REQUEST)NULL) {

        //
        // Something else on the queue, just queue it.
        //

        PNE3200_RESERVED_FROM_REQUEST(Adapter->LastRequest)->Next = NdisRequest;
        Adapter->LastRequest = NdisRequest;

    } else {

        //
        // The queue if empty, so nothing is in progress.
        //

        Adapter->FirstRequest = NdisRequest;
        Adapter->LastRequest = NdisRequest;

        NE3200ProcessRequestQueue(Adapter, FALSE);

    }
}

extern
VOID
NE3200ProcessRequestQueue(
    IN PNE3200_ADAPTER Adapter,
    IN BOOLEAN StatisticsUpdated
    )

/*++

Routine Description:

    NE3200ProcessRequestQueue takes the requests on the queue
    and processes them as much as possible. It will complete
    any requests that it fully processes. It will stop when
    the queue is empty or it finds a request that has to pend.

    SpinLock assumed held.

Arguments:

    Adapter - The adapter that the request is for.
    StatisticsUpdated - is TRUE if the adapter has updated the card
        statistics.

Return Value:

    NDIS_STATUS_PENDING (probably should be VOID...)


--*/
{
    PNDIS_REQUEST Request;
    PNE3200_REQUEST_RESERVED Reserved;
    PNE3200_OPEN Open;
    NDIS_STATUS Status;

    if (Adapter->ProcessingRequests) {
        return;
    } else {
        Adapter->ProcessingRequests = TRUE;
    }

    Request = Adapter->FirstRequest;

    for (;;) {

        //
        // Loop until we exit, which happens when a
        // request pends, or we empty the queue.
        //

        if ((Request == (PNDIS_REQUEST)NULL) || Adapter->ResetInProgress) {

            break;
        }

        Reserved = PNE3200_RESERVED_FROM_REQUEST(Request);

        switch (Request->RequestType) {

        case NdisRequestClose:

            Open = Reserved->OpenBlock;

            Status = EthDeleteFilterOpenAdapter(
                             Adapter->FilterDB,
                             Open->NdisFilterHandle,
                             NULL
                             );


            //
            // If the status is successful that merely implies that
            // we were able to delete the reference to the open binding
            // from the filtering code.
            //
            // The delete filter routine can return a "special" status
            // that indicates that there is a current NdisIndicateReceive
            // on this binding.  See below.
            //

            if (Status == NDIS_STATUS_SUCCESS) {

                //
                // Account for the filter's reference to this open.
                //

                Open->References--;

            } else if (Status == NDIS_STATUS_PENDING) {

                //
                // When the request completes we will dereference the
                // open to account for the filter package's reference.
                //

                //
                // Since we do not check for the success of the filter's
                // actions, we can just go on and close this guy.
                //

                Open->References--;
                Status = NDIS_STATUS_SUCCESS;

            } else if (Status == NDIS_STATUS_CLOSING_INDICATING) {

                //
                // When we have this status it indicates that the filtering
                // code was currently doing an NdisIndicateReceive. Our
                // close action routine will get called when the filter
                // is done with us, we remove the reference there.
                //

                Adapter->FirstRequest = Reserved->Next;
                Status = NDIS_STATUS_PENDING;

            } else {

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_DRIVER_FAILURE,
                    0
                    );

            }

            //
            // This flag prevents further requests on this binding.
            //

            Open->BindingShuttingDown = TRUE;

            //
            // Remove the reference kept for the fact that we
            // had something queued.
            //

            Open->References--;

            //
            // Remove the open from the open list and put it on
            // the closing list. This list is checked after every
            // request, and when the reference count goes to zero
            // the close is completed.
            //

            RemoveEntryList(&Open->OpenList);
            InsertTailList(&Adapter->CloseList,&Open->OpenList);

            break;

         case NdisRequestOpen:

            Open = Reserved->OpenBlock;

            if (!EthNoteFilterOpenAdapter(
                    Open->OwningAdapter->FilterDB,
                    Open,
                    Open->NdisBindingContext,
                    &Open->NdisFilterHandle
                    )) {

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteOpenAdapter(
                    Open->NdisBindingContext,
                    NDIS_STATUS_FAILURE,
                    0);

                NE3200_FREE_PHYS(Open);

                NdisAcquireSpinLock(&Adapter->Lock);

            } else {

                //
                // Everything has been filled in.  Synchronize access to the
                // adapter block and link the new open adapter in and increment
                // the opens reference count to account for the fact that the
                // filter routines have a "reference" to the open.
                //

                InsertTailList(&Adapter->OpenBindings,&Open->OpenList);
                Adapter->OpenCount++;
                Open->References++;

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteOpenAdapter(
                    Open->NdisBindingContext,
                    NDIS_STATUS_SUCCESS,
                    0);

                NdisAcquireSpinLock(&Adapter->Lock);

            }

            //
            // Set this, since we want to continue processing
            // the queue.
            //

            Status = NDIS_STATUS_SUCCESS;

            break;

        case NdisRequestQueryInformation:

            IF_LOG('Q');

            Status = NE3200QueryInformation(
                         Adapter,
                         Reserved->OpenBlock,
                         Request->DATA.QUERY_INFORMATION.Oid,
                         Request->DATA.QUERY_INFORMATION.InformationBuffer,
                         Request->DATA.QUERY_INFORMATION.InformationBufferLength,
                         &(Request->DATA.QUERY_INFORMATION.BytesWritten),
                         &(Request->DATA.QUERY_INFORMATION.BytesNeeded)
                         );

            break;

        case NdisRequestSetInformation:

            IF_LOG('q');

            Status = NE3200SetInformation(
                         Adapter,
                         Reserved->OpenBlock,
                         Request->DATA.SET_INFORMATION.Oid,
                         Request->DATA.SET_INFORMATION.InformationBuffer,
                         Request->DATA.SET_INFORMATION.InformationBufferLength,
                         &(Request->DATA.SET_INFORMATION.BytesRead),
                         &(Request->DATA.SET_INFORMATION.BytesNeeded));
            break;

        case NdisRequestQueryStatistics:

            //
            // Fire off a command first. This always returns TRUE.
            //

            if (!StatisticsUpdated) {

                PNE3200_SUPER_COMMAND_BLOCK CommandBlock;

                NE3200AcquirePublicCommandBlock(Adapter, &CommandBlock);

                //
                // Store the request that uses this CB
                //
                CommandBlock->Set = TRUE;
                CommandBlock->OwningOpenBinding = (PVOID)NE3200_FAKE_OPEN;

                //
                // Setup the command block.
                //
                CommandBlock->NextCommand = NULL;

                CommandBlock->Hardware.State =
                     NE3200_STATE_WAIT_FOR_ADAPTER;
                CommandBlock->Hardware.Status = 0;
                CommandBlock->Hardware.NextPending = NE3200_NULL;
                CommandBlock->Hardware.CommandCode =
                    NE3200_COMMAND_READ_ADAPTER_STATISTICS;

                //
                // Now that we're set up, let's do it!
                //

                NE3200SubmitCommandBlock(Adapter, CommandBlock);

                //
                // Catch the ball at the interrupt dpc
                //

                Adapter->ProcessingRequests = FALSE;
                return;

            }

            Status = NE3200QueryInformation(
                         Adapter,
                         (PNE3200_OPEN)NULL,
                         Request->DATA.QUERY_INFORMATION.Oid,
                         Request->DATA.QUERY_INFORMATION.InformationBuffer,
                         Request->DATA.QUERY_INFORMATION.InformationBufferLength,
                         &(Request->DATA.QUERY_INFORMATION.BytesWritten),
                         &(Request->DATA.QUERY_INFORMATION.BytesNeeded)
                         );
            break;

        }

        //
        // see if operation pended
        //

        if (Status == NDIS_STATUS_PENDING) {

            Adapter->ProcessingRequests = FALSE;
            return;

        }


        //
        // If we fall through here, we are done with this request.
        //

        Adapter->FirstRequest = Reserved->Next;

        if (Request->RequestType == NdisRequestQueryStatistics) {

            Adapter->References++;

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisCompleteQueryStatistics(
                Adapter->NdisAdapterHandle,
                Request,
                Status
                );

            NdisAcquireSpinLock(&Adapter->Lock);

            Adapter->References--;

        } else if ((Request->RequestType == NdisRequestQueryInformation) ||
                   (Request->RequestType == NdisRequestSetInformation)) {

            Open = Reserved->OpenBlock;

            IF_LOG(']');

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisCompleteRequest(
                Open->NdisBindingContext,
                Request,
                Status
                );

            NdisAcquireSpinLock(&Adapter->Lock);

            Open->References--;

        }

        Request = Adapter->FirstRequest;

        //
        // Now loop and continue on with the next request.
        //

    }
    Adapter->ProcessingRequests = FALSE;
}


extern
NDIS_STATUS
NE3200SetInformation(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN INT InformationBufferLength,
    IN PUINT BytesRead,
    IN PUINT BytesNeeded
    )

/*++

Routine Description:

    NE3200QueryInformation handles a set operation for a
    single OID.

Arguments:

    Adapter - The adapter that the set is for.

    Request - The ndis request structure from the protocol

    BytesNeeded - If there is not enough data in OvbBuffer
        to satisfy the OID, returns the amount of storage needed.

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING
    NDIS_STATUS_INVALID_LENGTH
    NDIS_STATUS_INVALID_OID

--*/

{

    NDIS_STATUS Status;
    ULONG PacketFilter;
    BytesNeeded;

    //
    // Now check for the most common OIDs
    //

    switch (Oid) {

    case OID_802_3_MULTICAST_LIST:

        if (InformationBufferLength % ETH_LENGTH_OF_ADDRESS != 0) {

            //
            // The data must be a multiple of the Ethernet
            // address size.
            //

            return NDIS_STATUS_INVALID_DATA;

        }

        //
        // Now call the filter package to set up the addresses.
        //

        Status = EthChangeFilterAddresses(
                     Adapter->FilterDB,
                     Open->NdisFilterHandle,
                     (PNDIS_REQUEST)NULL,
                     InformationBufferLength / ETH_LENGTH_OF_ADDRESS,
                     InformationBuffer,
                     TRUE
                     );

        *BytesRead = InformationBufferLength;
        break;

    case OID_GEN_CURRENT_PACKET_FILTER:

        if (InformationBufferLength != 4) {

           return NDIS_STATUS_INVALID_DATA;

        }

        //
        // Now call the filter package to set the packet filter.
        //

        NdisMoveMemory ((PVOID)&PacketFilter, InformationBuffer, sizeof(ULONG));

        //
        // Verify bits
        //

        if (PacketFilter & (NDIS_PACKET_TYPE_SOURCE_ROUTING |
                            NDIS_PACKET_TYPE_SMT |
                            NDIS_PACKET_TYPE_MAC_FRAME |
                            NDIS_PACKET_TYPE_FUNCTIONAL |
                            NDIS_PACKET_TYPE_ALL_FUNCTIONAL |
                            NDIS_PACKET_TYPE_GROUP
                           )) {

            Status = NDIS_STATUS_NOT_SUPPORTED;

            *BytesRead = 4;
            *BytesNeeded = 0;

            break;

        }

        Status = EthFilterAdjust(
                     Adapter->FilterDB,
                     Open->NdisFilterHandle,
                     (PNDIS_REQUEST)NULL,
                     PacketFilter,
                     TRUE
                     );

        *BytesRead = InformationBufferLength;

        break;

    case OID_GEN_CURRENT_LOOKAHEAD:

        *BytesRead = 4;
        Status = NDIS_STATUS_SUCCESS;
        break;

    case OID_GEN_PROTOCOL_OPTIONS:
        if (InformationBufferLength != 4) {

            Status = NDIS_STATUS_INVALID_LENGTH;
            *BytesNeeded = 4;
            break;

        }

        NdisMoveMemory (&Open->ProtOptionFlags, InformationBuffer, 4);
        *BytesRead = 4;
        Status = NDIS_STATUS_SUCCESS;
        break;

    default:

        Status = NDIS_STATUS_INVALID_OID;
        break;

    }

    return Status;
}



STATIC
NDIS_STATUS
NE3200QueryInformation(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN UINT InformationBufferLength,
    OUT PUINT BytesWritten,
    OUT PUINT BytesNeeded
)

/*++

Routine Description:

    The NE3200QueryProtocolInformation process a Query request for
    NDIS_OIDs that are specific to a binding about the MAC.  Note that
    some of the OIDs that are specific to bindings are also queryable
    on a global basis.  Rather than recreate this code to handle the
    global queries, I use a flag to indicate if this is a query for the
    global data or the binding specific data.

Arguments:

    Adapter - a pointer to the adapter.

    Open - a pointer to the open instance.  If null, then return
            global statistics.

    Oid - the NDIS_OID to process.

    InformationBuffer -  a pointer into the
    NdisRequest->InformationBuffer into which store the result of the query.

    InformationBufferLength - a pointer to the number of bytes left in the
    InformationBuffer.

    BytesWritten - a pointer to the number of bytes written into the
    InformationBuffer.

    BytesNeeded - If there is not enough room in the information buffer
    then this will contain the number of bytes needed to complete the
    request.

Return Value:

    The function value is the status of the operation.

--*/

{

static
NDIS_OID NE3200GlobalSupportedOids[] = {
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_LOOKAHEAD,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BUFFER_SPACE,
    OID_GEN_RECEIVE_BUFFER_SPACE,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_CURRENT_LOOKAHEAD,
    OID_GEN_XMIT_OK,
    OID_GEN_RCV_OK,
    OID_GEN_XMIT_ERROR,
    OID_GEN_RCV_ERROR,
    OID_GEN_RCV_NO_BUFFER,
    OID_GEN_RCV_CRC_ERROR,
    OID_GEN_TRANSMIT_QUEUE_LENGTH,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_RCV_ERROR_ALIGNMENT,
    OID_802_3_XMIT_ONE_COLLISION,
    OID_802_3_XMIT_MORE_COLLISIONS,
    OID_802_3_XMIT_DEFERRED,
    OID_802_3_XMIT_MAX_COLLISIONS,
    OID_802_3_RCV_OVERRUN,
    OID_802_3_XMIT_UNDERRUN,
    OID_802_3_XMIT_HEARTBEAT_FAILURE,
    OID_802_3_XMIT_TIMES_CRS_LOST,
    OID_802_3_XMIT_LATE_COLLISIONS
    };

static
NDIS_OID NE3200ProtocolSupportedOids[] = {
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_LOOKAHEAD,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BUFFER_SPACE,
    OID_GEN_RECEIVE_BUFFER_SPACE,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_CURRENT_LOOKAHEAD,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE
    };


    NDIS_MEDIUM Medium = NdisMedium802_3;
    UINT GenericUlong;
    USHORT GenericUShort;
    UCHAR GenericArray[6];
    UINT MulticastAddresses;

    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    // Common variables for pointing to result of query
    //

    PVOID MoveSource = (PVOID)(&GenericUlong);
    ULONG MoveBytes = sizeof(ULONG);

    NDIS_HARDWARE_STATUS HardwareStatus;
    UINT Filter;

    *BytesWritten = 0;
    *BytesNeeded = 0;

    //
    // Switch on request type
    //

    switch(Oid){

        case OID_GEN_MAC_OPTIONS:

            GenericUlong = (ULONG)(NDIS_MAC_OPTION_TRANSFERS_NOT_PEND   |
                                   NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA  |
                                   NDIS_MAC_OPTION_RECEIVE_SERIALIZED |
                                   NDIS_MAC_OPTION_NO_LOOPBACK
                                  );

            break;

        case OID_GEN_SUPPORTED_LIST:

            if (Open == NULL) {
                MoveSource = (PVOID)(NE3200GlobalSupportedOids);
                MoveBytes = sizeof(NE3200GlobalSupportedOids);
            } else {
                MoveSource = (PVOID)(NE3200ProtocolSupportedOids);
                MoveBytes = sizeof(NE3200ProtocolSupportedOids);
            }
            break;

        case OID_GEN_HARDWARE_STATUS:


            if (Adapter->ResetInProgress){

                HardwareStatus = NdisHardwareStatusReset;

            } else {

                HardwareStatus = NdisHardwareStatusReady;

            }


            MoveSource = (PVOID)(&HardwareStatus);
            MoveBytes = sizeof(NDIS_HARDWARE_STATUS);

            break;

        case OID_GEN_MEDIA_SUPPORTED:
        case OID_GEN_MEDIA_IN_USE:

            MoveSource = (PVOID) (&Medium);
            MoveBytes = sizeof(NDIS_MEDIUM);
            break;

        case OID_GEN_MAXIMUM_LOOKAHEAD:
        case OID_GEN_CURRENT_LOOKAHEAD:
        case OID_GEN_MAXIMUM_FRAME_SIZE:

            GenericUlong = (ULONG) (MAXIMUM_ETHERNET_PACKET_SIZE - NE3200_HEADER_SIZE);

            break;

        case OID_GEN_MAXIMUM_TOTAL_SIZE:
        case OID_GEN_TRANSMIT_BLOCK_SIZE:
        case OID_GEN_RECEIVE_BLOCK_SIZE:

            GenericUlong = (ULONG) (MAXIMUM_ETHERNET_PACKET_SIZE);

            break;


        case OID_GEN_LINK_SPEED:

            //
            // 10 Mbps
            //

            GenericUlong = (ULONG)100000;

            break;


        case OID_GEN_TRANSMIT_BUFFER_SPACE:

            GenericUlong = (ULONG) MAXIMUM_ETHERNET_PACKET_SIZE *
                                 NE3200_NUMBER_OF_TRANSMIT_BUFFERS;

            break;

        case OID_GEN_RECEIVE_BUFFER_SPACE:

            GenericUlong = (ULONG) MAXIMUM_ETHERNET_PACKET_SIZE *
                                 NE3200_NUMBER_OF_RECEIVE_BUFFERS;

            break;


        case OID_GEN_VENDOR_DESCRIPTION:

            MoveSource = (PVOID)"NE3200 ";
            MoveBytes = 8;
            break;

        case OID_GEN_DRIVER_VERSION:

            GenericUShort = (USHORT)0x0300;

            MoveSource = (PVOID)(&GenericUShort);
            MoveBytes = sizeof(GenericUShort);
            break;


        case OID_GEN_CURRENT_PACKET_FILTER:

            if (Open == NULL) {

                Filter = ETH_QUERY_FILTER_CLASSES(Adapter->FilterDB);

                GenericUlong = (ULONG)(Filter);

            } else {

                Filter = ETH_QUERY_PACKET_FILTER(Adapter->FilterDB,
                                                 Open->NdisFilterHandle);

                GenericUlong = (ULONG)(Filter);

            }

            break;


        case OID_802_3_PERMANENT_ADDRESS:

            ETH_COPY_NETWORK_ADDRESS(
                (PCHAR)GenericArray,
                Adapter->NetworkAddress
                );

            MoveSource = (PVOID)(GenericArray);
            MoveBytes = ETH_LENGTH_OF_ADDRESS;
            break;

        case OID_802_3_CURRENT_ADDRESS:
            ETH_COPY_NETWORK_ADDRESS(
                (PCHAR)GenericArray,
                Adapter->CurrentAddress
                );

            MoveSource = (PVOID)(GenericArray);
            MoveBytes = ETH_LENGTH_OF_ADDRESS;
            break;

        case OID_802_3_MULTICAST_LIST:

            if (Open == NULL) {

                NDIS_STATUS Status;
                EthQueryGlobalFilterAddresses(
                    &Status,
                    Adapter->FilterDB,
                    InformationBufferLength,
                    &MulticastAddresses,
                    (PVOID)InformationBuffer);

                MoveSource = (PVOID)InformationBuffer;
                MoveBytes = MulticastAddresses * ETH_LENGTH_OF_ADDRESS;

            } else {

                NDIS_STATUS Status;
                EthQueryOpenFilterAddresses(
                    &Status,
                    Adapter->FilterDB,
                    Open->NdisFilterHandle,
                    InformationBufferLength,
                    &MulticastAddresses,
                    (PVOID)InformationBuffer);

                if (Status == NDIS_STATUS_SUCCESS) {
                    MoveSource = (PVOID)InformationBuffer;
                    MoveBytes = MulticastAddresses * ETH_LENGTH_OF_ADDRESS;
                } else {
                    MoveSource = (PVOID)InformationBuffer;
                    MoveBytes = ETH_LENGTH_OF_ADDRESS *
                        EthNumberOfOpenFilterAddresses(
                            Adapter->FilterDB,
                            Open->NdisFilterHandle);
                }

            }
            break;

        case OID_802_3_MAXIMUM_LIST_SIZE:

            GenericUlong = (ULONG) NE3200_MAXIMUM_MULTICAST;

            break;

        default:

            if (Open != NULL) {

                StatusToReturn = NDIS_STATUS_NOT_SUPPORTED;
                break;

            }

            switch(Oid){

                case OID_GEN_XMIT_OK:
                    GenericUlong = (ULONG) Adapter->GoodTransmits;
                    break;

                case OID_GEN_RCV_OK:
                        GenericUlong = (ULONG) Adapter->GoodReceives;
                        break;

                case OID_GEN_XMIT_ERROR:
                        GenericUlong = (ULONG) (Adapter->RetryFailure +
                                                Adapter->LostCarrier +
                                                Adapter->UnderFlow +
                                                Adapter->NoClearToSend);
                        break;

                case OID_GEN_RCV_ERROR:
                        GenericUlong = (ULONG) (Adapter->CrcErrors +
                                                Adapter->AlignmentErrors +
                                                Adapter->OutOfResources +
                                                Adapter->DmaOverruns);
                        break;

                case OID_GEN_RCV_NO_BUFFER:
                        GenericUlong = (ULONG) Adapter->OutOfResources;
                        break;

                case OID_GEN_RCV_CRC_ERROR:
                        GenericUlong = (ULONG) Adapter->CrcErrors;
                        break;

                case OID_GEN_TRANSMIT_QUEUE_LENGTH:
                        GenericUlong = (ULONG) Adapter->TransmitsQueued;
                        break;

                case OID_802_3_RCV_ERROR_ALIGNMENT:
                        GenericUlong = (ULONG) Adapter->AlignmentErrors;
                        break;

                case OID_802_3_XMIT_ONE_COLLISION:
                        GenericUlong = (ULONG) Adapter->OneRetry;
                        break;

                case OID_802_3_XMIT_MORE_COLLISIONS:
                        GenericUlong = (ULONG) Adapter->MoreThanOneRetry;
                        break;

                case OID_802_3_XMIT_DEFERRED:
                        GenericUlong = (ULONG) Adapter->Deferred;
                        break;

                case OID_802_3_XMIT_MAX_COLLISIONS:
                        GenericUlong = (ULONG) Adapter->RetryFailure;
                        break;

                case OID_802_3_RCV_OVERRUN:
                        GenericUlong = (ULONG) Adapter->DmaOverruns;
                        break;

                case OID_802_3_XMIT_UNDERRUN:
                        GenericUlong = (ULONG) Adapter->UnderFlow;
                        break;

                case OID_802_3_XMIT_HEARTBEAT_FAILURE:
                        GenericUlong = (ULONG) Adapter->NoClearToSend;
                        break;

                case OID_802_3_XMIT_TIMES_CRS_LOST:
                        GenericUlong = (ULONG) Adapter->LostCarrier;
                        break;

                default:
                    StatusToReturn = NDIS_STATUS_NOT_SUPPORTED;
                    break;

            }

    }

    if (StatusToReturn == NDIS_STATUS_SUCCESS) {

        if (MoveBytes > InformationBufferLength) {

            //
            // Not enough room in InformationBuffer. Punt
            //

            *BytesNeeded = MoveBytes;

            StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

        } else {

            //
            // Copy result into InformationBuffer
            //

            *BytesWritten = MoveBytes;
            if (MoveBytes > 0) {
                NE3200_MOVE_MEMORY(
                        InformationBuffer,
                        MoveSource,
                        MoveBytes
                        );
            }
        }
    }

    return(StatusToReturn);
}


extern
NDIS_STATUS
NE3200QueryGlobalStatistics(
    IN NDIS_HANDLE MacAdapterContext,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    NE3200QueryGlobalStatistics handles a per-adapter query
    for statistics. It is similar to NE3200QueryInformation,
    which is per-binding.

Arguments:

    MacAdapterContext - The context value that the MAC passed
        to NdisRegisterAdapter; actually as pointer to a
        NE3200_ADAPTER.

    NdisRequest - Describes the query request.

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING

--*/

{

    //
    // This holds the status we will return.
    //

    NDIS_STATUS StatusOfRequest;

    //
    // Points to the adapter that this request is coming through.
    //
    PNE3200_ADAPTER Adapter = (PNE3200_ADAPTER)MacAdapterContext;

    PNE3200_REQUEST_RESERVED Reserved = PNE3200_RESERVED_FROM_REQUEST(NdisRequest);

    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    if (!Adapter->ResetInProgress) {

        switch (NdisRequest->RequestType) {

        case NdisRequestQueryStatistics:

            //
            // Valid request.
            //

            Reserved->OpenBlock = (PNE3200_OPEN)NULL;
            Reserved->Next = (PNDIS_REQUEST)NULL;

            NE3200QueueRequest (Adapter, NdisRequest);

            StatusOfRequest = NDIS_STATUS_PENDING;
            break;

        default:

            //
            // Unknown request
            //

            StatusOfRequest = NDIS_STATUS_NOT_SUPPORTED;
            break;

        }

    } else {

        StatusOfRequest = NDIS_STATUS_RESET_IN_PROGRESS;

    }


    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    NE3200_DO_DEFERRED(Adapter);
    return StatusOfRequest;
}


STATIC
NDIS_STATUS
NE3200UpdateMulticastTable(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open,
    IN UINT CurrentAddressCount,
    IN CHAR CurrentAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN BOOLEAN Set
    )

/*++

Routine Description:

    This routine is called by either NE3200AddMulticast or
    NE3200DeleteMulticast whenever the adapter's multicast address
    table must be updated.

    NOTE: This routine assumes that it is called with the lock
    acquired.

Arguments:

    Adapter - The adapter where the multicast is to be changed.

    Open - Binding which made this request.

    CurrentAddressCount - The number of addresses in the address array.

    CurrentAddresses - An array of multicast addresses.  Note that this
    array already contains the new address.

    Set - This is from a set.

Return Value:

    None.


--*/

{

    //
    // This points to the open binding's private Command Block.
    //
    PNE3200_SUPER_COMMAND_BLOCK CommandBlock;

    //
    // Holds the status that should be returned to the filtering package.
    //
    NDIS_STATUS StatusOfUpdate;

    //
    // Multicast address table
    //
    PUCHAR MulticastAddressTable;

    //
    // See if we can acquire a private command block.
    //

    IF_LOG('f');

    NE3200AcquirePublicCommandBlock(Adapter, &CommandBlock);

    //
    // Store the request that uses this CB
    //
    CommandBlock->Set = Set;

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

        OriginalAddress = &CurrentAddresses[0][0];
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

        CommandBlock->OwningOpenBinding = Open;
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

        StatusOfUpdate = NDIS_STATUS_PENDING;

    }

    return StatusOfUpdate;

}
