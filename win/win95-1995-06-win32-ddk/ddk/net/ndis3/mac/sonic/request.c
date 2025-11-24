/*++

Copyright (c) 1990-1995  Microsoft Corporation

Module Name:

    request.c

Abstract:

    This is the cose to handle NdisRequestss for the National Semiconductor
    SONIC Ethernet controller.  This driver conforms to the NDIS 3.0 interface.

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
// This macro determines if the directed address
// filtering in the CAM is actually necessary given
// the current filter.
//

#define CAM_DIRECTED_SIGNIFICANT(_Filter) \
    ((((_Filter) & NDIS_PACKET_TYPE_DIRECTED) && \
    (!((_Filter) & NDIS_PACKET_TYPE_PROMISCUOUS))) ? 1 : 0)


//
// This macro determines if the multicast filtering in
// the CAM are actually necessary given the current filter.
//

#define CAM_MULTICAST_SIGNIFICANT(_Filter) \
    ((((_Filter) & NDIS_PACKET_TYPE_MULTICAST) && \
    (!((_Filter) & (NDIS_PACKET_TYPE_ALL_MULTICAST | \
                    NDIS_PACKET_TYPE_PROMISCUOUS)))) ? 1 : 0)


STATIC
NDIS_STATUS
SonicQueryInformation(
    IN PSONIC_ADAPTER Adapter,
    IN PSONIC_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN UINT InformationBufferLength,
    IN PUINT BytesWritten,
    IN PUINT BytesNeeded,
    IN BOOLEAN Global
    );

STATIC
NDIS_STATUS
SonicSetInformation(
    IN PSONIC_ADAPTER Adapter,
    IN PSONIC_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN UINT InformationBufferLength,
    IN PUINT BytesRead,
    IN PUINT BytesNeeded
    );

STATIC
MULTICAST_STATUS
ChangeClassDispatch(
    IN PSONIC_ADAPTER Adapter,
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN BOOLEAN Set
    );

STATIC
MULTICAST_STATUS
ChangeAddressDispatch(
    IN PSONIC_ADAPTER Adapter,
    IN UINT AddressCount,
    IN CHAR Addresses[][ETH_LENGTH_OF_ADDRESS],
    IN BOOLEAN Set
    );




extern
NDIS_STATUS
SonicRequest(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    The SonicRequest function handles general requests from the
    protocol. Currently these include SetInformation and
    QueryInformation, more may be added in the future.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to SONIC_OPEN.

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
    PSONIC_ADAPTER Adapter;

    //
    // Points to the MacReserved section of the request.
    //
    PSONIC_REQUEST_RESERVED Reserved = PSONIC_RESERVED_FROM_REQUEST(NdisRequest);


    Adapter = PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    if (!Adapter->ResetInProgress) {

        PSONIC_OPEN Open;

        Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

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

                SonicQueueRequest (Adapter, NdisRequest);

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

    SONIC_DO_DEFERRED(Adapter);
    return StatusOfRequest;
}

extern
VOID
SonicQueueRequest(
    IN PSONIC_ADAPTER Adapter,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    SonicQueueRequest takes an NDIS_REQUEST and ensures that it
    gets processed and completed. It processes the
    request immediately if nothing else is in progress, otherwise
    it queues it for later processing.

    THIS ROUTINE IS CALLED WITH THE LOCK HELD.

Arguments:

    Adapter - The adapter that the request is for.

    NdisRequest - The NDIS_REQUEST structure describing the request.
        The SonicReserved section is partially filled in, except
        for the queueing and current offset fields.

Return Value:

    NDIS_STATUS_PENDING if the request was queued.
    Otherwise, the return code from SonicProcessRequestQueue.
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

        PSONIC_RESERVED_FROM_REQUEST(Adapter->LastRequest)->Next = NdisRequest;
        Adapter->LastRequest = NdisRequest;

    } else {

        //
        // The queue if empty; if nothing is in progress, if we
        // are not resetting, then process this request; if
        // we are resetting, then after the reset the queue
        // will be restarted.
        //

        Adapter->FirstRequest = NdisRequest;
        Adapter->LastRequest = NdisRequest;

        if (!Adapter->RequestInProgress) {

            SonicProcessRequestQueue(Adapter);

        }


    }
}

extern
VOID
SonicProcessRequestQueue(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    SonicProcessRequestQueue takes the requests on the queue
    and processes them as much as possible. It will complete
    any requests that it fully processes. It will stop when
    the queue is empty or it finds a request that has to pend.

    THIS ROUTINE IS CALLED WITH THE LOCK HELD.

Arguments:

    Adapter - The adapter that the request is for.

Return Value:

    NDIS_STATUS_PENDING (probably should be VOID...)


--*/
{
    PNDIS_REQUEST Request;
    PSONIC_REQUEST_RESERVED Reserved;
    NDIS_STATUS Status;
    PSONIC_OPEN Open;


    Request = Adapter->FirstRequest;

    for (;;) {

        //
        // Loop until we exit, which happens when a
        // request pends, or we empty the queue.
        //

        if (Request == (PNDIS_REQUEST)NULL) {
            Adapter->RequestInProgress = FALSE;
            break;
        }

        if (Adapter->ResetInProgress) {
            Adapter->RequestInProgress = FALSE;
            break;
        }

        Adapter->RequestInProgress = TRUE;

        Reserved = PSONIC_RESERVED_FROM_REQUEST(Request);
        switch (Request->RequestType) {

        case NdisRequestClose:

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Close request\n");
            }
#endif

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
                // This means that a CAM reload is needed; the
                // dispatch routines defer this for a close,
                // so we need to do it now.
                //

                SonicStartCamReload(Adapter);

                //
                // When the request completes we will dereference the
                // open to account for the filter package's reference.
                //

            } else if (Status == NDIS_STATUS_CLOSING_INDICATING) {

                //
                // BUGBUG: Do we do a CAM reload here?
                //

                //
                // When we have this status it indicates that the filtering
                // code was currently doing an NdisIndicateReceive. Our
                // close action routine will get called when the filter
                // is done with us, we remove the reference there.
                //

                Status = NDIS_STATUS_PENDING;

                ;

            } else {

                ASSERT(0);

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

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Open request\n");
            }
#endif

            if (!EthNoteFilterOpenAdapter(
                    Open->OwningSonic->FilterDB,
                    Open,
                    Open->NdisBindingContext,
                    &Open->NdisFilterHandle
                    )) {

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteOpenAdapter(
                    Open->NdisBindingContext,
                    NDIS_STATUS_FAILURE,
                    0);


                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                    2,
                    openAdapter,
                    SONIC_ERRMSG_OPEN_DB
                    );

                SONIC_FREE_MEMORY(Open, sizeof(SONIC_OPEN));

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

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Query request\n");
            }
#endif

            Status = SonicQueryInformation(
                         Adapter,
                         Reserved->OpenBlock,
                         Request->DATA.QUERY_INFORMATION.Oid,
                         Request->DATA.QUERY_INFORMATION.InformationBuffer,
                         Request->DATA.QUERY_INFORMATION.InformationBufferLength,
                         &(Request->DATA.QUERY_INFORMATION.BytesWritten),
                         &(Request->DATA.QUERY_INFORMATION.BytesNeeded),
                         FALSE);

            break;

        case NdisRequestQueryStatistics:

            Status = SonicQueryInformation(
                         Adapter,
                         Reserved->OpenBlock,
                         Request->DATA.QUERY_INFORMATION.Oid,
                         Request->DATA.QUERY_INFORMATION.InformationBuffer,
                         Request->DATA.QUERY_INFORMATION.InformationBufferLength,
                         &(Request->DATA.QUERY_INFORMATION.BytesWritten),
                         &(Request->DATA.QUERY_INFORMATION.BytesNeeded),
                         TRUE);

            break;

        case NdisRequestSetInformation:

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Set request\n");
            }
#endif

            Status = SonicSetInformation(
                         Adapter,
                         Reserved->OpenBlock,
                         Request->DATA.SET_INFORMATION.Oid,
                         Request->DATA.SET_INFORMATION.InformationBuffer,
                         Request->DATA.SET_INFORMATION.InformationBufferLength,
                         &(Request->DATA.SET_INFORMATION.BytesRead),
                         &(Request->DATA.SET_INFORMATION.BytesNeeded));

            break;

        }


        //
        // If the operation pended, then stop processing the queue.
        //

        if (Status == NDIS_STATUS_PENDING) {
            return;
        }


        //
        // If we fall through here, we are done with this request.
        //

        Adapter->FirstRequest = Reserved->Next;

        if ((Request->RequestType == NdisRequestQueryInformation) ||
            (Request->RequestType == NdisRequestSetInformation)) {

            Open = Reserved->OpenBlock;

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisCompleteRequest(
                Open->NdisBindingContext,
                Request,
                Status);

            NdisAcquireSpinLock(&Adapter->Lock);

            --Open->References;

        } else if (Request->RequestType == NdisRequestQueryStatistics) {

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisCompleteQueryStatistics(
                Adapter->NdisAdapterHandle,
                Request,
                Status);

            NdisAcquireSpinLock(&Adapter->Lock);

            --Adapter->References;

        }

        Request = Adapter->FirstRequest;

        //
        // Now loop and continue on with the next request.
        //

    }

}

extern
NDIS_STATUS
SonicQueryGlobalStatistics(
    IN NDIS_HANDLE MacAdapterContext,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    SonicQueryGlobalStatistics handles a per-adapter query
    for statistics. It is similar to SonicQueryInformation,
    which is per-binding.

Arguments:

    MacAdapterContext - The context value that the MAC passed
        to NdisRegisterAdapter; actually as pointer to a
        SONIC_ADAPTER.

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
    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)MacAdapterContext;

    PSONIC_REQUEST_RESERVED Reserved = PSONIC_RESERVED_FROM_REQUEST(NdisRequest);

    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    if (!Adapter->ResetInProgress) {

        switch (NdisRequest->RequestType) {

        case NdisRequestQueryStatistics:

            //
            // Valid request.
            //

            Reserved->OpenBlock = (PSONIC_OPEN)NULL;
            Reserved->Next = (PNDIS_REQUEST)NULL;

            Adapter->References++;

            SonicQueueRequest (Adapter, NdisRequest);

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

    SONIC_DO_DEFERRED(Adapter);
    return StatusOfRequest;
}

STATIC
NDIS_STATUS
SonicQueryInformation(
    IN PSONIC_ADAPTER Adapter,
    IN PSONIC_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN UINT InformationBufferLength,
    IN PUINT BytesWritten,
    IN PUINT BytesNeeded,
    IN BOOLEAN Global
    )

/*++

Routine Description:

    SonicQueryInformation handles a query operation for a
    single OID.

    THIS ROUTINE IS CALLED WITH THE LOCK HELD.

Arguments:

    Adapter - The adapter that the query is for.

    Open - The binding that the query is for.

    Oid - The OID of the query.

    InformationBuffer - Holds the result of the query.

    InformationBufferLength - The length of InformationBuffer.

    BytesWritten - If the call is successful, returns the number
        of bytes written to InformationBuffer.

    BytesNeeded - If there is not enough room in InformationBuffer
        to satisfy the OID, returns the amount of storage needed.

    Global - TRUE if this is for a QueryGlobalInformation, FALSE for
        a protocol QueryInformation.

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING
    NDIS_STATUS_INVALID_LENGTH
    NDIS_STATUS_INVALID_OID

--*/

{

    INT i;
    PNDIS_OID SupportedOidArray;
    INT SupportedOids;
    NDIS_OID MaskOid;
    PVOID SourceBuffer;
    ULONG SourceBufferLength;
    ULONG GenericUlong;
    USHORT GenericUshort;
    UINT MulticastAddresses;
    NDIS_STATUS Status;
    UCHAR VendorId[4];
#ifdef SONIC_EISA
    static const UCHAR EisaDescriptor[] = "SONIC EISA Bus Master Ethernet Adapter (DP83932EB-EISA)";
#endif
#ifdef SONIC_INTERNAL
    static const UCHAR InternalDescriptor[] = "MIPS R4000 on-board network controller";
#endif

    static const NDIS_OID SonicGlobalSupportedOids[] = {
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
        OID_GEN_VENDOR_ID,
        OID_GEN_VENDOR_DESCRIPTION,
        OID_GEN_DRIVER_VERSION,
        OID_GEN_CURRENT_PACKET_FILTER,
        OID_GEN_CURRENT_LOOKAHEAD,
        OID_GEN_XMIT_OK,
        OID_GEN_RCV_OK,
        OID_GEN_XMIT_ERROR,
        OID_GEN_RCV_ERROR,
        OID_GEN_RCV_NO_BUFFER,
        OID_GEN_DIRECTED_BYTES_XMIT,
        OID_GEN_DIRECTED_FRAMES_XMIT,
        OID_GEN_MULTICAST_BYTES_XMIT,
        OID_GEN_MULTICAST_FRAMES_XMIT,
        OID_GEN_BROADCAST_BYTES_XMIT,
        OID_GEN_BROADCAST_FRAMES_XMIT,
        OID_GEN_DIRECTED_BYTES_RCV,
        OID_GEN_DIRECTED_FRAMES_RCV,
        OID_GEN_MULTICAST_BYTES_RCV,
        OID_GEN_MULTICAST_FRAMES_RCV,
        OID_GEN_BROADCAST_BYTES_RCV,
        OID_GEN_BROADCAST_FRAMES_RCV,
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

    static const NDIS_OID SonicProtocolSupportedOids[] = {
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
        OID_GEN_VENDOR_ID,
        OID_GEN_VENDOR_DESCRIPTION,
        OID_GEN_DRIVER_VERSION,
        OID_GEN_CURRENT_PACKET_FILTER,
        OID_GEN_CURRENT_LOOKAHEAD,
        OID_GEN_TRANSMIT_QUEUE_LENGTH,
        OID_802_3_PERMANENT_ADDRESS,
        OID_802_3_CURRENT_ADDRESS,
        OID_802_3_MULTICAST_LIST,
        OID_802_3_MAXIMUM_LIST_SIZE
        };

    //
    // Check that the OID is valid.
    //

    if (Global) {
        SupportedOidArray = (PNDIS_OID)SonicGlobalSupportedOids;
        SupportedOids = sizeof(SonicGlobalSupportedOids)/sizeof(ULONG);
    } else {
        SupportedOidArray = (PNDIS_OID)SonicProtocolSupportedOids;
        SupportedOids = sizeof(SonicProtocolSupportedOids)/sizeof(ULONG);
    }

    for (i=0; i<SupportedOids; i++) {
        if (Oid == SupportedOidArray[i]) {
            break;
        }
    }

    if (i == SupportedOids) {
        *BytesWritten = 0;
        return NDIS_STATUS_INVALID_OID;
    }

    //
    // Initialize these once, since this is the majority
    // of cases.
    //

    SourceBuffer = &GenericUlong;
    SourceBufferLength = sizeof(ULONG);

    switch (Oid & OID_TYPE_MASK) {

    case OID_TYPE_GENERAL_OPERATIONAL:

        switch (Oid) {

        case OID_GEN_MAC_OPTIONS:

            GenericUlong = (ULONG)(NDIS_MAC_OPTION_TRANSFERS_NOT_PEND   |
                                   NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA  |
                                   NDIS_MAC_OPTION_RECEIVE_SERIALIZED |
                                   NDIS_MAC_OPTION_NO_LOOPBACK
                                  );

            break;

        case OID_GEN_SUPPORTED_LIST:

            SourceBuffer =  SupportedOidArray;
            SourceBufferLength = SupportedOids * sizeof(ULONG);
            break;

        case OID_GEN_HARDWARE_STATUS:

            GenericUlong = NdisHardwareStatusReady;
            break;

        case OID_GEN_MEDIA_SUPPORTED:

            GenericUlong = NdisMedium802_3;
            break;

        case OID_GEN_MEDIA_IN_USE:

            GenericUlong = NdisMedium802_3;
            if (Global) {
                if (Adapter->OpenCount == 0) {
                    SourceBufferLength = 0;
                }
            }
            break;

        case OID_GEN_MAXIMUM_LOOKAHEAD:

            GenericUlong = (SONIC_INDICATE_MAXIMUM-14 < SONIC_LOOPBACK_MAXIMUM) ?
                            SONIC_INDICATE_MAXIMUM-14 : SONIC_LOOPBACK_MAXIMUM;
            break;

        case OID_GEN_MAXIMUM_FRAME_SIZE:

            GenericUlong = 1500;
            break;

        case OID_GEN_MAXIMUM_TOTAL_SIZE:

            GenericUlong = 1514;
            break;

        case OID_GEN_LINK_SPEED:

            GenericUlong = 100000;    // 10 Mbps in 100 bps units
            break;

        case OID_GEN_TRANSMIT_BUFFER_SPACE:

            GenericUlong = SONIC_LARGE_BUFFER_SIZE * SONIC_NUMBER_OF_TRANSMIT_DESCRIPTORS;
            break;

        case OID_GEN_RECEIVE_BUFFER_SPACE:

            GenericUlong = SONIC_LARGE_BUFFER_SIZE * SONIC_NUMBER_OF_RECEIVE_DESCRIPTORS;
            break;

        case OID_GEN_TRANSMIT_BLOCK_SIZE:

            GenericUlong = SONIC_LARGE_BUFFER_SIZE;
            break;

        case OID_GEN_RECEIVE_BLOCK_SIZE:

            GenericUlong = SONIC_LARGE_BUFFER_SIZE;
            break;

        case OID_GEN_VENDOR_ID:

            SONIC_MOVE_MEMORY(VendorId, Adapter->PermanentNetworkAddress, 3);
            VendorId[3] = 0x0;
            SourceBuffer = VendorId;
            SourceBufferLength = sizeof(VendorId);
            break;

        case OID_GEN_VENDOR_DESCRIPTION:

            switch (Adapter->AdapterType) {
#ifdef SONIC_EISA
            case SONIC_ADAPTER_TYPE_EISA:
                SourceBuffer = (PVOID)EisaDescriptor;
                SourceBufferLength = sizeof(EisaDescriptor);
                break;
#endif
#ifdef SONIC_INTERNAL
            case SONIC_ADAPTER_TYPE_INTERNAL:
                SourceBuffer = (PVOID)InternalDescriptor;
                SourceBufferLength = sizeof(InternalDescriptor);
                break;
#endif
            default:
                ASSERT(FALSE);
                break;
            }
            break;

        case OID_GEN_DRIVER_VERSION:

            GenericUshort = (SONIC_NDIS_MAJOR_VERSION << 8) + SONIC_NDIS_MINOR_VERSION;
            SourceBuffer = &GenericUshort;
            SourceBufferLength = sizeof(USHORT);
            break;

        case OID_GEN_CURRENT_PACKET_FILTER:

            if (Global) {
                GenericUlong = Adapter->CurrentPacketFilter;
            } else {
                GenericUlong = ETH_QUERY_PACKET_FILTER (Adapter->FilterDB, Open->NdisFilterHandle);
            }
            break;

        case OID_GEN_CURRENT_LOOKAHEAD:

            GenericUlong = (SONIC_INDICATE_MAXIMUM-14 < SONIC_LOOPBACK_MAXIMUM) ?
                            SONIC_INDICATE_MAXIMUM-14 : SONIC_LOOPBACK_MAXIMUM;
            break;

        default:

            ASSERT(FALSE);
            break;

        }

        break;

    case OID_TYPE_GENERAL_STATISTICS:

        MaskOid = (Oid & OID_INDEX_MASK) - 1;

        switch (Oid & OID_REQUIRED_MASK) {

        case OID_REQUIRED_MANDATORY:

            ASSERT (MaskOid < GM_ARRAY_SIZE);

            if (MaskOid == GM_RECEIVE_NO_BUFFER) {

                //
                // This one is read off the card, update unless our
                // counter is more (which indicates an imminent
                // overflow interrupt, so we don't update).
                //

                USHORT MissedPacket;
                SONIC_READ_PORT(Adapter, SONIC_FRAME_ALIGNMENT_ERROR, &MissedPacket);

                if ((Adapter->GeneralMandatory[GM_RECEIVE_NO_BUFFER] & 0xffff) <
                        MissedPacket) {

                    Adapter->GeneralMandatory[GM_RECEIVE_NO_BUFFER] =
                        (Adapter->GeneralMandatory[GM_RECEIVE_NO_BUFFER] & 0xffff0000) +
                        MissedPacket;

                }
            }

            GenericUlong = Adapter->GeneralMandatory[MaskOid];
            break;

        case OID_REQUIRED_OPTIONAL:

            ASSERT (MaskOid < GO_ARRAY_SIZE);

            if (MaskOid == GO_RECEIVE_CRC) {

                //
                // This one is read off the card, update unless our
                // counter is more (which indicates an imminent
                // overflow interrupt, so we don't update).
                //

                USHORT CrcError;
                SONIC_READ_PORT(Adapter, SONIC_FRAME_ALIGNMENT_ERROR, &CrcError);

                if ((Adapter->GeneralOptional[GO_RECEIVE_CRC - GO_ARRAY_START] & 0xffff) <
                        CrcError) {

                    Adapter->GeneralOptional[GO_RECEIVE_CRC - GO_ARRAY_START] =
                        (Adapter->GeneralOptional[GO_RECEIVE_CRC - GO_ARRAY_START] & 0xffff0000) +
                        CrcError;

                }
            }

            if ((MaskOid / 2) < GO_COUNT_ARRAY_SIZE) {

                if (MaskOid & 0x01) {
                    // Frame count
                    GenericUlong = Adapter->GeneralOptionalFrameCount[MaskOid / 2];
                } else {
                    // Byte count
                    SourceBuffer = &Adapter->GeneralOptionalByteCount[MaskOid / 2];
                    SourceBufferLength = sizeof(LARGE_INTEGER);
                }

            } else {

                GenericUlong = Adapter->GeneralOptional[MaskOid - GO_ARRAY_START];

            }

            break;

        default:

            ASSERT(FALSE);
            break;

        }

        break;

    case OID_TYPE_802_3_OPERATIONAL:

        switch (Oid) {

        case OID_802_3_PERMANENT_ADDRESS:

            SourceBuffer = Adapter->PermanentNetworkAddress;
            SourceBufferLength = 6;
            break;

        case OID_802_3_CURRENT_ADDRESS:

            SourceBuffer = Adapter->CurrentNetworkAddress;
            SourceBufferLength = 6;
            break;

        case OID_802_3_MULTICAST_LIST:

            if (Global) {

                EthQueryGlobalFilterAddresses(
                    &Status,
                    Adapter->FilterDB,
                    InformationBufferLength,
                    &MulticastAddresses,
                    (PVOID)InformationBuffer);

                SourceBuffer = (PVOID)InformationBuffer;
                SourceBufferLength = MulticastAddresses * ETH_LENGTH_OF_ADDRESS;

            } else {

                EthQueryOpenFilterAddresses(
                    &Status,
                    Adapter->FilterDB,
                    Open->NdisFilterHandle,
                    InformationBufferLength,
                    &MulticastAddresses,
                    (PVOID)InformationBuffer);

                if (Status == NDIS_STATUS_SUCCESS) {
                    SourceBuffer = (PVOID)InformationBuffer;
                    SourceBufferLength = MulticastAddresses * ETH_LENGTH_OF_ADDRESS;
                } else {
                    SourceBuffer = (PVOID)InformationBuffer;
                    SourceBufferLength = ETH_LENGTH_OF_ADDRESS *
                        EthNumberOfOpenFilterAddresses(
                            Adapter->FilterDB,
                            Open->NdisFilterHandle);
                }

            }

            break;

        case OID_802_3_MAXIMUM_LIST_SIZE:

            GenericUlong = SONIC_CAM_ENTRIES - 1;
            break;

        default:

            ASSERT(FALSE);
            break;

        }

        break;

    case OID_TYPE_802_3_STATISTICS:

        MaskOid = (Oid & OID_INDEX_MASK) - 1;

        switch (Oid & OID_REQUIRED_MASK) {

        case OID_REQUIRED_MANDATORY:

            ASSERT (MaskOid < MM_ARRAY_SIZE);

            if (MaskOid == MM_RECEIVE_ERROR_ALIGNMENT) {

                //
                // This one is read off the card, update unless our
                // counter is more (which indicates an imminent
                // overflow interrupt, so we don't update).
                //

                USHORT FaError;
                SONIC_READ_PORT(Adapter, SONIC_FRAME_ALIGNMENT_ERROR, &FaError);

                if ((Adapter->MediaMandatory[MM_RECEIVE_ERROR_ALIGNMENT] & 0xffff) <
                        FaError) {

                    Adapter->MediaMandatory[MM_RECEIVE_ERROR_ALIGNMENT] =
                        (Adapter->MediaMandatory[MM_RECEIVE_ERROR_ALIGNMENT] & 0xffff0000) +
                        FaError;

                }
            }

            GenericUlong = Adapter->MediaMandatory[MaskOid];
            break;

        case OID_REQUIRED_OPTIONAL:

            ASSERT (MaskOid < MO_ARRAY_SIZE);
            GenericUlong = Adapter->MediaOptional[MaskOid];
            break;

        default:

            ASSERT(FALSE);
            break;

        }

        break;

    }

    if (SourceBufferLength > InformationBufferLength) {
        *BytesNeeded = SourceBufferLength;
        return NDIS_STATUS_INVALID_LENGTH;
    }

    SONIC_MOVE_MEMORY (InformationBuffer, SourceBuffer, SourceBufferLength);
    *BytesWritten = SourceBufferLength;

    return NDIS_STATUS_SUCCESS;

}


STATIC
NDIS_STATUS
SonicSetInformation(
    IN PSONIC_ADAPTER Adapter,
    IN PSONIC_OPEN Open,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN UINT InformationBufferLength,
    IN PUINT BytesRead,
    IN PUINT BytesNeeded
    )

/*++

Routine Description:

    SonicQueryInformation handles a set operation for a
    single OID.

    THIS ROUTINE IS CALLED WITH THE LOCK HELD.

Arguments:

    Adapter - The adapter that the set is for.

    Open - The binding that the set is for.

    Oid - The OID of the set.

    InformationBuffer - Holds the data to be set.

    InformationBufferLength - The length of InformationBuffer.

    BytesRead - If the call is successful, returns the number
        of bytes read from InformationBuffer.

    BytesNeeded - If there is not enough data in InformationBuffer
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
#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Change Multicast List request\n");
            }
#endif


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

        return Status;

        break;

    case OID_GEN_CURRENT_PACKET_FILTER:

        if (InformationBufferLength != 4) {

           *BytesNeeded = 4;
           return NDIS_STATUS_INVALID_LENGTH;

        }

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Change Packet Filter request\n");
            }
#endif

        //
        // Now call the filter package to set the packet filter.
        //

        SONIC_MOVE_MEMORY ((PVOID)&PacketFilter, InformationBuffer, sizeof(ULONG));

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

            *BytesRead = 4;
            *BytesNeeded = 0;

            return NDIS_STATUS_NOT_SUPPORTED;

        }

        Status = EthFilterAdjust(
                     Adapter->FilterDB,
                     Open->NdisFilterHandle,
                     (PNDIS_REQUEST)NULL,
                     PacketFilter,
                     TRUE
                     );

        *BytesRead = 4;
        return Status;

        break;

    case OID_GEN_CURRENT_LOOKAHEAD:

        //
        // No need to record requested lookahead length since we
        // always indicate the whole packet.
        //

        *BytesRead = 4;
        return NDIS_STATUS_SUCCESS;
        break;

    case OID_GEN_PROTOCOL_OPTIONS:
        if (InformationBufferLength != 4) {
        
            *BytesNeeded = 4;
            Status = NDIS_STATUS_INVALID_LENGTH; 
            break;
        }

        NdisMoveMemory(&Open->ProtOptionFlags, InformationBuffer, 4);

        *BytesRead = 4;
        return NDIS_STATUS_SUCCESS;
        break;

    default:

        return NDIS_STATUS_INVALID_OID;
        break;

    }

}

extern
NDIS_STATUS
SonicChangeClass(
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
    adapter was opened.  In reality, it is a pointer to SONIC_OPEN.

    RequestHandle - A value supplied by the NDIS interface that the MAC
    must use when completing this request with the NdisCompleteRequest
    service, if the MAC completes this request asynchronously.

    Set - If true the change resulted from a set, otherwise the
    change resulted from a open closing.

Return Value:

    None.


--*/

{


    PSONIC_ADAPTER Adapter = PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // The open that made this request.
    //
    PSONIC_OPEN Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Holds the change that should be returned to the filtering package.
    //
    NDIS_STATUS StatusOfChange;


    if (Adapter->ResetInProgress) {

        StatusOfChange = NDIS_STATUS_RESET_IN_PROGRESS;

    } else {

        //
        // The whole purpose of this routine is to determine whether
        // the filtering changes need to result in the hardware being
        // reset.
        //

        ASSERT(OldFilterClasses != NewFilterClasses);


        if (ChangeClassDispatch(Adapter,
                OldFilterClasses,
                NewFilterClasses,
                Set
                ) == CAM_LOADED) {


#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Filter request pended\n");
            }
#endif

            StatusOfChange = NDIS_STATUS_PENDING;

        } else {

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Filter request succeeded\n");
            }
#endif

            StatusOfChange = NDIS_STATUS_SUCCESS;

        }

    }

    return StatusOfChange;

}

STATIC
MULTICAST_STATUS
ChangeClassDispatch(
    IN PSONIC_ADAPTER Adapter,
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN BOOLEAN Set
    )

/*++

Routine Description:

    Modifies the Receive Control Register and Cam Enable registers,
    then re-loads the CAM if necessary.

Arguments:

    Adapter - The adapter.

    Address - The address to load.

    CamIndex - The index of this address in the CAM.

    Set - TRUE if the dispatch is due to a Set, not a close.

Return Value:

    CAM_LOADED - if the CAM was reloaded.
    CAM_NOT_LOADED - otherwise.

--*/

{

    MULTICAST_STATUS DispatchStatus;

    //
    // Is a CAM re-load necessary.
    //
    BOOLEAN CamReloadNeeded;

    //
    // The new value for the RCR.
    //
    USHORT NewReceiveControl = SONIC_RCR_DEFAULT_VALUE;


    //
    // First take care of the Receive Control Register.
    //

    if (NewFilterClasses & NDIS_PACKET_TYPE_PROMISCUOUS) {

        NewReceiveControl |= SONIC_RCR_PROMISCUOUS_PHYSICAL |
                             SONIC_RCR_ACCEPT_BROADCAST |
                             SONIC_RCR_ACCEPT_ALL_MULTICAST;

    } else {

        if (NewFilterClasses & NDIS_PACKET_TYPE_ALL_MULTICAST) {

            NewReceiveControl |= SONIC_RCR_ACCEPT_ALL_MULTICAST;

        }

        if (NewFilterClasses & NDIS_PACKET_TYPE_BROADCAST) {

            NewReceiveControl |= SONIC_RCR_ACCEPT_BROADCAST;

        }

    }

    Adapter->ReceiveControlRegister = NewReceiveControl;
    SONIC_WRITE_PORT(Adapter, SONIC_RECEIVE_CONTROL,
            Adapter->ReceiveControlRegister
            );


    //
    // Now see if CamEnable has to be modified and the
    // CAM re-loaded.
    //

    CamReloadNeeded = FALSE;

    if (CAM_DIRECTED_SIGNIFICANT(OldFilterClasses) !=
        CAM_DIRECTED_SIGNIFICANT(NewFilterClasses)) {

        //
        // The NDIS_PACKET_TYPE_DIRECTED bit has changed.
        //

        CamReloadNeeded = TRUE;

        if (CAM_DIRECTED_SIGNIFICANT(NewFilterClasses)) {

            Adapter->CamDescriptorArea->CamEnable |= 1;

        } else {

            Adapter->CamDescriptorArea->CamEnable &= ~1;

        }

    }

    if (CAM_MULTICAST_SIGNIFICANT(OldFilterClasses) !=
        CAM_MULTICAST_SIGNIFICANT(NewFilterClasses)) {

        CamReloadNeeded = TRUE;

        if (CAM_MULTICAST_SIGNIFICANT(NewFilterClasses)) {

            Adapter->CamDescriptorArea->CamEnable |=
                                    Adapter->MulticastCamEnableBits;

        } else {

            Adapter->CamDescriptorArea->CamEnable &= 1;

        }

    }


    if (CamReloadNeeded) {

        //
        // This will cause a LOAD_CAM interrupt when it is done.
        //

        if (Set) {
            SonicStartCamReload(Adapter);
        }

        DispatchStatus = CAM_LOADED;

    } else {

        DispatchStatus = CAM_NOT_LOADED;

    }


    Adapter->CurrentPacketFilter = NewFilterClasses;

    return DispatchStatus;

}

extern
NDIS_STATUS
SonicChangeAddresses(
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
    adapter was opened.  In reality, it is a pointer to SONIC_OPEN.

    RequestHandle - A value supplied by the NDIS interface that the MAC
    must use when completing this request with the NdisCompleteRequest
    service, if the MAC completes this request asynchronously.

    Set - If true the change resulted from a set, otherwise the
    change resulted from a open closing.

Return Value:

    None.


--*/

{


    PSONIC_ADAPTER Adapter = PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // The open that made this request.
    //
    PSONIC_OPEN Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Holds the change that should be returned to the filtering package.
    //
    NDIS_STATUS StatusOfChange;


    if (Adapter->ResetInProgress) {

        StatusOfChange = NDIS_STATUS_RESET_IN_PROGRESS;

    } else {

        //
        // The whole purpose of this routine is to determine whether
        // the filtering changes need to result in the hardware being
        // reset.
        //

        if (ChangeAddressDispatch(Adapter,
                NewAddressCount,
                NewAddresses,
                Set
                ) == CAM_LOADED) {


#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Address request pended\n");
            }
#endif

            StatusOfChange = NDIS_STATUS_PENDING;

        } else {

            StatusOfChange = NDIS_STATUS_SUCCESS;

        }

    }

    return StatusOfChange;

}

STATIC
MULTICAST_STATUS
ChangeAddressDispatch(
    IN PSONIC_ADAPTER Adapter,
    IN UINT AddressCount,
    IN CHAR Addresses[][ETH_LENGTH_OF_ADDRESS],
    IN BOOLEAN Set
    )

/*++

Routine Description:

    Modifies the Receive Control Register and Cam Enable registers,
    then re-loads the CAM if necessary.

Arguments:

    Adapter - The adapter.

    AddressCount - The number of addresses in Addresses

    Addresses - The new multicast address list.

    Set - TRUE if the change is due to a Set, not a close.

Return Value:

    CAM_LOADED - if the CAM was reloaded.
    CAM_NOT_LOADED - otherwise.

--*/

{

    ULONG EnableBit;
    MULTICAST_STATUS DispatchStatus;
    UINT i;

    //
    // The first entry in the CAM is for our address.
    //

    Adapter->MulticastCamEnableBits = 1;
    EnableBit = 1;

    //
    // Loop through, copying the addresses into the CAM.
    //

    for (i=0; i<AddressCount; i++) {

        EnableBit <<= 1;
        Adapter->MulticastCamEnableBits |= EnableBit;

        SONIC_LOAD_CAM_FRAGMENT(
            &Adapter->CamDescriptorArea->CamFragments[i+1],
            i+1,
            Addresses[i]
            );

    }

    Adapter->CamDescriptorAreaSize = AddressCount + 1;

    //
    // Now see if we have to worry about re-loading the
    // CAM also.
    //

    if (CAM_MULTICAST_SIGNIFICANT(Adapter->CurrentPacketFilter)) {

        Adapter->CamDescriptorArea->CamEnable = Adapter->MulticastCamEnableBits;

        //
        // This will cause a LOAD_CAM interrupt when it is done.
        //

        if (Set) {
            SonicStartCamReload(Adapter);
        }

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Address request pended\n");
            }
#endif


        DispatchStatus = CAM_LOADED;

    } else {

#if DBG
            if (SonicDbg) {
                DbgPrint("Processing Address request succeeded\n");
            }
#endif

        DispatchStatus = CAM_NOT_LOADED;

    }

    return DispatchStatus;

}

extern
VOID
SonicCloseAction(
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
    adapter was opened.  In reality, it is a pointer to SONIC_OPEN.

Return Value:

    None.


--*/

{

    PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)->References--;

}
