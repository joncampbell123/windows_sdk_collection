/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    pend.c

Abstract:

    Multicast and filter functions for the NDIS 3.0 Novell 2000 driver.

Environment:

    Kernel mode, FSD

--*/

#include <ndis.h>
#include <efilter.h>
#include "ne2000hw.h"
#include "ne2000sw.h"
#ifdef NDIS_WIN
    #pragma LCODE
#endif


VOID
HandlePendingOperations(
    IN PVOID SystemSpecific1,
    IN PVOID DeferredContext,       // will be a pointer to the adapter block
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )

/*++

Routine Description:

    Called by pending functions to process elements on the
    pending queue.

Arguments:

    DeferredContext - will be a pointer to the adapter block

Return Value:

    None.

--*/

{
    PNE2000_ADAPTER Adapter = ((PNE2000_ADAPTER)DeferredContext);
    PNE2000_PEND_DATA PendOp;
    PNE2000_OPEN TmpOpen;
    NDIS_STATUS Status;

    UNREFERENCED_PARAMETER(SystemSpecific1);
    UNREFERENCED_PARAMETER(SystemSpecific2);
    UNREFERENCED_PARAMETER(SystemSpecific3);

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    //
    // If an operation is being dispatched or a reset is running, exit.
    //

    if ((!Adapter->ResetInProgress) && (Adapter->PendOp == NULL)) {

        for (;;) {

            //
            // We hold SpinLock here.
            //

            if (Adapter->PendQueue != NULL) {

                //
                // Take the request off the queue and dispatch it.
                //

                PendOp = Adapter->PendQueue;

                Adapter->PendQueue = PendOp->Next;

                if (PendOp == Adapter->PendQTail) {

                    Adapter->PendQTail = NULL;

                }

                Adapter->PendOp = PendOp;

                NdisReleaseSpinLock(&Adapter->Lock);

                Status = ((PendOp->RequestType == NdisRequestClose) ||
                          (PendOp->RequestType == NdisRequestGeneric3)) ?
                              DispatchSetMulticastAddressList(Adapter)  :
                              DispatchSetPacketFilter(Adapter);

                TmpOpen = PendOp->Open;

                if ((PendOp->RequestType != NdisRequestGeneric1) &&
                    (PendOp->RequestType != NdisRequestClose)) {  // Close Adapter

                    //
                    // Complete it since it previously pended.
                    //

                    NdisCompleteRequest(PendOp->Open->NdisBindingContext,
                        PNDIS_REQUEST_FROM_PNE2000_PEND_DATA(PendOp),
                        Status);

                }

                //
                // This will call CompleteClose if necessary.
                //

                NdisAcquireSpinLock(&Adapter->Lock);

                TmpOpen->ReferenceCount--;

                if (Adapter->ResetInProgress) {

                    //
                    // We have to stop processing requests.
                    //

                    break;     // jump to BREAK_LOCATION
                }

            } else {

                break;     // jump to BREAK_LOCATION

            }
        }

        //
        // BREAK_LOCATION
        //
        // Hold Lock here.
        //

        Adapter->PendOp = NULL;

        if (Adapter->ResetInProgress) {

            //
            // Exited due to a reset, indicate that the DPC
            // handler is done for now.
            //

            Adapter->References--;

            NdisReleaseSpinLock(&Adapter->Lock);

            Ne2000ResetStageDone(Adapter, MULTICAST_RESET);

            return;
        }

    }

    if (Adapter->CloseQueue != NULL) {

        PNE2000_OPEN OpenP;
        PNE2000_OPEN TmpOpenP;
        PNE2000_OPEN PrevOpenP;

        //
        // Check for an open that may have closed
        //

        OpenP = Adapter->CloseQueue;
        PrevOpenP = NULL;

        while (OpenP != NULL) {

            if (OpenP->ReferenceCount > 0) {

                OpenP = OpenP->NextOpen;
                PrevOpenP = OpenP;

                continue;

            }

#if DBG

            if (!OpenP->Closing) {

                DbgPrint("BAD CLOSE: %d\n", OpenP->ReferenceCount);

                //DbgBreakPoint();

                OpenP = OpenP->NextOpen;
                PrevOpenP = OpenP;

                continue;

            }

#endif

            //
            // The last reference is completed; a previous call to ElnkiiCloseAdapter
            // will have returned NDIS_STATUS_PENDING, so things must be finished
            // off now.
            //

            //
            // Check if MaxLookAhead needs adjusting.
            //

            if (OpenP->LookAhead == Adapter->MaxLookAhead) {

                Ne2000AdjustMaxLookAhead(Adapter);

            }

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisCompleteCloseAdapter (OpenP->NdisBindingContext, NDIS_STATUS_SUCCESS);

            NdisAcquireSpinLock(&Adapter->Lock);

            //
            // Remove from close list
            //

            if (PrevOpenP != NULL) {

                PrevOpenP->NextOpen = OpenP->NextOpen;

            } else {

                Adapter->CloseQueue = OpenP->NextOpen;
            }

            //
            // Go to next one
            //

            TmpOpenP = OpenP;
            OpenP = OpenP->NextOpen;

            NdisFreeMemory(TmpOpenP, sizeof(NE2000_OPEN), 0);

        }

        if ((Adapter->CloseQueue == NULL) && (Adapter->OpenQueue == NULL)) {

            //
            // We can stop the card.
            //

            CardStop(Adapter);

        }

    }

    NE2000_DO_DEFERRED(Adapter);

}


NDIS_STATUS
DispatchSetPacketFilter(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    Sets the appropriate bits in the adapter filters
    and modifies the card Receive Configuration Register if needed.

Arguments:

    Adapter - Pointer to the adapter block

Return Value:

    The final status (always NDIS_STATUS_SUCCESS).

Notes:

  - Note that to receive all multicast packets the multicast
    registers on the card must be filled with 1's. To be
    promiscuous that must be done as well as setting the
    promiscuous physical flag in the RCR. This must be done
    as long as ANY protocol bound to this adapter has their
    filter set accordingly.

--*/

{
    UINT PacketFilter;

    PacketFilter = ETH_QUERY_FILTER_CLASSES(Adapter->FilterDB);

    //
    // See what has to be put on the card.
    //

    if (PacketFilter & (NDIS_PACKET_TYPE_ALL_MULTICAST | NDIS_PACKET_TYPE_PROMISCUOUS)) {

        //
        // need "all multicast" now.
        //

        NdisAcquireSpinLock(&Adapter->Lock);

        CardSetAllMulticast(Adapter);    // fills it with 1's

        NdisReleaseSpinLock(&Adapter->Lock);

    } else {

        //
        // No longer need "all multicast".
        //

        DispatchSetMulticastAddressList(Adapter);

    }

    //
    // The multicast bit in the RCR should be on if ANY protocol wants
    // multicast/all multicast packets (or is promiscuous).
    //

    if (PacketFilter & (NDIS_PACKET_TYPE_ALL_MULTICAST |
                        NDIS_PACKET_TYPE_MULTICAST |
                        NDIS_PACKET_TYPE_PROMISCUOUS)) {

        Adapter->NicReceiveConfig |= RCR_MULTICAST;

    } else {

        Adapter->NicReceiveConfig &= ~RCR_MULTICAST;

    }

    //
    // The promiscuous physical bit in the RCR should be on if ANY
    // protocol wants to be promiscuous.
    //

    if (PacketFilter & NDIS_PACKET_TYPE_PROMISCUOUS) {

        Adapter->NicReceiveConfig |= RCR_ALL_PHYS;

    } else {

        Adapter->NicReceiveConfig &= ~RCR_ALL_PHYS;

    }

    //
    // The broadcast bit in the RCR should be on if ANY protocol wants
    // broadcast packets (or is promiscuous).
    //

    if (PacketFilter & (NDIS_PACKET_TYPE_BROADCAST | NDIS_PACKET_TYPE_PROMISCUOUS)) {

        Adapter->NicReceiveConfig |= RCR_BROADCAST;

    } else {

        Adapter->NicReceiveConfig &= ~RCR_BROADCAST;

    }

    CardSetReceiveConfig(Adapter);

    return NDIS_STATUS_SUCCESS;
}


NDIS_STATUS
DispatchSetMulticastAddressList(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    Sets the multicast list for this open

Arguments:

    Adapter - Pointer to the adapter block

Return Value:

Implementation Note:

    When invoked, we are to make it so that the multicast list in the filter
    package becomes the multicast list for the adapter. To do this, we
    determine the required contents of the NIC multicast registers and
    update them.


--*/

{
    //
    // Update the local copy of the NIC multicast regs and copy them to the NIC
    //

    CardFillMulticastRegs(Adapter);

    NdisAcquireSpinLock(&Adapter->Lock);

    CardCopyMulticastRegs(Adapter);

    NdisReleaseSpinLock(&Adapter->Lock);

    return NDIS_STATUS_SUCCESS;
}

