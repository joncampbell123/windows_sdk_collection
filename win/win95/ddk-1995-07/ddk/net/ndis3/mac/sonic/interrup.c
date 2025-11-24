/*++

Copyright (c) 1990-1995  Microsoft Corporation

Module Name:

    interrup.c

Abstract:

    This is a part of the driver for the National Semiconductor SONIC
    Ethernet controller.  It contains the interrupt-handling routines.
    This driver conforms to the NDIS 3.0 interface.

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

#undef DPR_INTERRUPTS

//
// We use the #define DPR_INTERRUPTS to determine how we handle
// disabling interrupts during the ISR and DPR
//
// If DPR_INTERRUPTS is not defined, then we mask off all interrupts
// in the ISR and reenable them when the DPR has run. If it is
// defined, then we keep interrupts enabled during the DPR.
//
// With DPR_INTERRUPTS undefined, there will only be one DPR running
// at one time time, and in addition the ISR and DPR will never be
// running at the same time. This saves us interrupts and calls to
// NdisSynchronizeWithInterrupt, but prevents parallelism on MP. It
// is also good programming practice for drivers that can cause the
// interrupt line to drop during the DPR, to prevent the "short interrupt"
// problem on some steppings of the 486 (the sonic driver as structured
// does not have this problem).
//


#ifdef DPR_INTERRUPTS

//
// We need this structures to call NdisSynchronizeWithInterrupt
// because it only takes one parameter.
//

typedef struct _SONIC_SYNCH_CONTEXT {

    //
    // Pointer to the sonic adapter for which interrupts are
    // being synchronized.
    //
    PSONIC_ADAPTER Adapter;

    //
    // Pointer used differently depending on the call;
    //
    // For GET_ISR_SINCE_LAST_PROCESSED, it points
    // to a local variable that will receive the value
    // of the adapter's IsrValue.
    // For REMOVE_EOL_AND_ACK, it points to the Link
    // field to remove the EOL bit from.
    // For WRITE_RWP_AND_ACK, it holds the value
    // to be written to the Current Write Pointer register.
    //
    PVOID Local;

    //
    // Pointer used by GET_ISR_SINCE_LAST_PROCESSED, it
    // determines which bits in the ISR we care about.
    //
    USHORT Parameter;

} SONIC_SYNCH_CONTEXT,*PSONIC_SYNCH_CONTEXT;

//
// NOTE: This macro is to synchronize execution with interrupts.  It
// gets the stored value of the ISR and clears the old value,
// only dealing with the bits specified by M.
//
#define GET_ISR_SINCE_LAST_PROCESSED(A,V,M) \
{ \
    PSONIC_ADAPTER _A = A; \
    SONIC_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.Local = (PVOID)(V); \
    _C.Parameter = (M); \
    NdisSynchronizeWithInterrupt( \
        &_A->Interrupt, \
        SonicSynchGetIsr, \
        &_C \
        ); \
}


//
// NOTE: This macro is to synchronize execution with interrupts.
// It clears the EOL bit in the Link field and acknowledges
// the ReceiveDescriptorsExhausted interrupt.
//
#define REMOVE_EOL_AND_ACK(A,L) \
{ \
    PSONIC_ADAPTER _A = A; \
    SONIC_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.Local = (PVOID)(L); \
    NdisSynchronizeWithInterrupt( \
        &_A->Interrupt, \
        SonicSynchRemoveEol, \
        &_C \
        ); \
}


//
// NOTE: This macro is to synchronize execution with interrupts.
// It writes the specified value to the RWP register and
// acknowledges the ReceiveBuffersExhausted interrupt.
//
#define WRITE_RWP_AND_ACK(A,RWP) \
{ \
    PSONIC_ADAPTER _A = A; \
    SONIC_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.Local = (PVOID)(RWP); \
    NdisSynchronizeWithInterrupt( \
        &_A->Interrupt, \
        SonicSynchWriteRwp, \
        &_C \
        ); \
}

#else  // DPR_INTERRUPTS

//
// Define the macros to do what the functions do directly,
// since the ISR and DPR won't be running concurrently. We
// also don't write to the interrupt mask register since it
// will be set to 0 when these calls are made (from the DPR).
//

#define GET_ISR_SINCE_LAST_PROCESSED(A,V,M) \
{ \
    PSONIC_ADAPTER _A = A; \
    *(V) |= ((_A)->IsrValue & (M)); \
    (_A)->IsrValue &= (USHORT)~(M); \
}

#define REMOVE_EOL_AND_ACK(A,L) \
{ \
    PSONIC_ADAPTER _A = A; \
    SONIC_REMOVE_END_OF_LIST(L); \
    if ((_A)->ReceiveDescriptorsExhausted) { \
        SONIC_WRITE_PORT((_A), SONIC_INTERRUPT_STATUS, \
            SONIC_INT_RECEIVE_DESCRIPTORS \
            ); \
        ++SonicReceiveDescExhausted; \
        (_A)->InterruptMaskRegister |= SONIC_INT_RECEIVE_DESCRIPTORS; \
        (_A)->ReceiveDescriptorsExhausted = FALSE; \
    } \
}

#define WRITE_RWP_AND_ACK(A,RWP) \
{ \
    PSONIC_ADAPTER _A = A; \
    SONIC_WRITE_PORT((_A), SONIC_RESOURCE_WRITE, (RWP)); \
    if ((_A)->ReceiveBuffersExhausted) { \
        SONIC_WRITE_PORT((_A), SONIC_INTERRUPT_STATUS, \
            SONIC_INT_RECEIVE_BUFFERS \
            ); \
        ++SonicReceiveBuffersExhausted; \
        (_A)->InterruptMaskRegister |= SONIC_INT_RECEIVE_BUFFERS; \
        (_A)->ReceiveBuffersExhausted = FALSE; \
    } \
}

#endif  // DPR_INTERRUPTS



STATIC
BOOLEAN
ProcessReceiveInterrupts(
    IN PSONIC_ADAPTER Adapter
    );

STATIC
BOOLEAN
ProcessTransmitInterrupts(
    IN PSONIC_ADAPTER Adapter
    );

#ifdef DPR_INTERRUPTS

STATIC
BOOLEAN
SonicSynchGetIsr(
    IN PVOID Context
    );

STATIC
BOOLEAN
SonicSynchRemoveEol(
    IN PVOID Context
    );

STATIC
BOOLEAN
SonicSynchWriteRwp(
    IN PVOID Context
    );

#endif  // DPR_INTERRUPTS

STATIC
VOID
ProcessInterrupt(
    IN PSONIC_ADAPTER Adapter
    );


#ifdef DPR_INTERRUPTS

//
// These functions are for use with NdisSynchronizeWithInterrupt.
//


STATIC
BOOLEAN
SonicSynchGetIsr(
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will or
    the value of the stored ISR into the other passed address
    in the context and clear the stored ISR value. It only
    returns and clears the bits specified in the parameter
    field of the context.

Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and a pointer to an address in which to place the contents
    of the ISR.

Return Value:

    Always returns true.

--*/

{

    PSONIC_SYNCH_CONTEXT C = Context;
    USHORT Mask = C->Parameter;

    *((PUSHORT)C->Local) = *((PUSHORT)C->Local) |
                            (C->Adapter->IsrValue & Mask);

    C->Adapter->IsrValue &= (USHORT)~Mask;

    return TRUE;

}

STATIC
BOOLEAN
SonicSynchRemoveEol(
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will clear
    the EOL bit from the specified link field in the receive
    descriptor, acknowledge the RDE interrupt, and reenable the
    interrupt.

Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and a pointer to the address of the link field.

Return Value:

    Always returns true.

--*/

{

    PSONIC_SYNCH_CONTEXT C = Context;

    SONIC_REMOVE_END_OF_LIST(
        (PSONIC_PHYSICAL_ADDRESS)C->Local
        );

    if (C->Adapter->ReceiveDescriptorsExhausted) {

        SONIC_WRITE_PORT(C->Adapter, SONIC_INTERRUPT_STATUS,
            SONIC_INT_RECEIVE_DESCRIPTORS
            );

        C->Adapter->InterruptMaskRegister |= SONIC_INT_RECEIVE_DESCRIPTORS;
        SONIC_WRITE_PORT(C->Adapter, SONIC_INTERRUPT_MASK,
                C->Adapter->InterruptMaskRegister
                );

        C->Adapter->ReceiveDescriptorsExhausted = FALSE;

    }

    return TRUE;

}

STATIC
BOOLEAN
SonicSynchWriteRwp(
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will write
    the specified value to the Current Write Pointer register
    (effectively adding a receive buffer), acknowledge the RBE
    interrupt, and reenable that interrupt.

Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and the new value of the RWP register.

Return Value:

    Always returns true.

--*/

{

    PSONIC_SYNCH_CONTEXT C = Context;

    SONIC_WRITE_PORT(C->Adapter, SONIC_RESOURCE_WRITE,
        (USHORT)C->Local
        );

    if (C->Adapter->ReceiveBuffersExhausted) {

        SONIC_WRITE_PORT(C->Adapter, SONIC_INTERRUPT_STATUS,
            SONIC_INT_RECEIVE_BUFFERS
            );

        C->Adapter->InterruptMaskRegister |= SONIC_INT_RECEIVE_BUFFERS;
        SONIC_WRITE_PORT(C->Adapter, SONIC_INTERRUPT_MASK,
                C->Adapter->InterruptMaskRegister
                );

        C->Adapter->ReceiveBuffersExhausted = FALSE;

    }

    return TRUE;

}
#endif  // DPR_INTERRUPTS

extern
BOOLEAN
SonicInterruptService(
    IN PVOID Context
    )

/*++

Routine Description:

    Interrupt service routine for the sonic.  It's main job is
    to get the value of ISR and record the changes in the
    adapters own list of interrupt reasons.

Arguments:

    Context - Really a pointer to the adapter.

Return Value:

    Returns true if the card ISR is non-zero.

--*/

{

    //
    // Will hold the value from the ISR.
    //
    USHORT LocalIsrValue;

    //
    // Holds the pointer to the adapter.
    //
    PSONIC_ADAPTER Adapter = Context;


    SONIC_READ_PORT(Adapter, SONIC_INTERRUPT_STATUS, &LocalIsrValue);

    if (LocalIsrValue != 0x0000) {

#if DBG
    if (SonicDbg) {
        if (LocalIsrValue & (
                SONIC_INT_BUS_RETRY |
                SONIC_INT_LOAD_CAM_DONE |
                SONIC_INT_PROG_INTERRUPT |
                SONIC_INT_TRANSMIT_ERROR |
                SONIC_INT_RECEIVE_DESCRIPTORS |
                SONIC_INT_RECEIVE_BUFFERS |
                SONIC_INT_RECEIVE_OVERFLOW |
                SONIC_INT_CRC_TALLY_ROLLOVER |
                SONIC_INT_FAE_TALLY_ROLLOVER |
                SONIC_INT_MP_TALLY_ROLLOVER
                )) {
            DbgPrint("ISR %x\n", LocalIsrValue);
        }
    }
#endif
        //
        // It's our interrupt. Clear only those bits that we got
        // in this read of ISR.  We do it this way in case any new
        // reasons for interrupts occur between the time that we
        // read ISR and the time that we clear the bits.
        //
        // Also, we don't clear the RDE or RBE bits, but set
        // the flags for later acknowledgement.
        //

#ifndef DPR_INTERRUPTS
        //
        // If desired, mask off all interrupts here. Do this
        // before acking them in the status register to remove
        // the possibility of another interrupt being generated.
        //

        SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK, 0);
#endif

        SONIC_WRITE_PORT(
            Adapter,
            SONIC_INTERRUPT_STATUS,
            (USHORT)(LocalIsrValue & SONIC_INT_IMMEDIATE_ACK)
            );


        //
        // See if we have a new RDE or RBE interrupt, if so
        // set the flag and mask that interrupt (otherwise,
        // since we don't ack it, we would get reinterrupted
        // right away).
        //

        if (LocalIsrValue & SONIC_INT_RECEIVE_DESCRIPTORS) {

            if (!Adapter->ReceiveDescriptorsExhausted) {

                Adapter->ReceiveDescriptorsExhausted = TRUE;

                Adapter->InterruptMaskRegister &=
                                            ~SONIC_INT_RECEIVE_DESCRIPTORS;
#ifdef DPR_INTERRUPTS
                //
                // Update the interrupt mask register unless we
                // have it masked off completely.
                //
                SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK,
                        Adapter->InterruptMaskRegister
                        );
#endif

            }

        }

        if (LocalIsrValue & SONIC_INT_RECEIVE_BUFFERS) {

            if (!Adapter->ReceiveBuffersExhausted) {

                Adapter->ReceiveBuffersExhausted = TRUE;

                Adapter->InterruptMaskRegister &=
                                            ~SONIC_INT_RECEIVE_BUFFERS;
#ifdef DPR_INTERRUPTS
                //
                // Update the interrupt mask register unless we
                // have it masked off completely.
                //
                SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK,
                        Adapter->InterruptMaskRegister
                        );
#endif

            }

        }


        //
        // If we got a LOAD_CAM_DONE interrupt, it may be
        // because our first initialization is complete.
        // We check this here because on some systems the
        // DeferredProcessing call might not interrupt
        // the initialization process.
        //

        if (LocalIsrValue & SONIC_INT_LOAD_CAM_DONE) {

            if (Adapter->FirstInitialization) {

                Adapter->FirstInitialization = FALSE;

#if DBG
                {
                    USHORT PortValue;

                    SONIC_READ_PORT(Adapter, SONIC_SILICON_REVISION, &PortValue);
                    if (SonicDbg) {
                        DbgPrint("SONIC Initialized: Revision %d\n", PortValue);
                    }
                }
#endif

#ifndef DPR_INTERRUPTS
                //
                // Re-enable the interrupts we just disabled.
                //

                SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK,
                        Adapter->InterruptMaskRegister
                        );
#endif
                //
                // No deferred processing is needed.
                //

                return FALSE;

            }
        }


        //
        // Or the ISR value into the adapter version of the ISR.
        //

        Adapter->IsrValue |= LocalIsrValue;


#ifdef DPR_INTERRUPTS
        //
        // For latched interrupts, we want to force an edge
        // in the case where interrupts appeared between when we
        // read the ISR and when we wrote it back out. We mask
        // the IMR off here, then reenable it during the
        // deferred processing routine.
        //

        if (Adapter->InterruptLatched) {
            SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK, 0);
        }
#endif

        return TRUE;

    } else {

        return FALSE;

    }

}
#if 23
ULONG SonicIndicatedPackets = 0;
ULONG SonicReceiveBuffersExhausted = 0;
ULONG SonicReceiveDescExhausted = 0;
#endif

extern
VOID
SonicDeferredProcessing(
    IN PVOID SystemSpecific1,
    IN PVOID Context,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )

/*++

Routine Description:

    This DPR routine is queued by the interrupt service routine
    and other routines within the driver that notice that
    some deferred processing needs to be done.  It's main
    job is to call the interrupt processing code.

Arguments:

    Context - Really a pointer to the adapter.

    SystemSpecific123 - None of these arguments is used.

Return Value:

    None.

--*/

{

    //
    // A pointer to the adapter object.
    //
    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)Context;

    //
    // Holds a value of the Interrupt Status register.
    //
    USHORT Isr = 0;

    //
    // TRUE if the main loop did something.
    //
    BOOLEAN DidSomething = TRUE;

    //
    // TRUE if ReceiveComplete needs to be indicated.
    //
    BOOLEAN IndicateReceiveComplete = FALSE;

#if 23
    LARGE_INTEGER Start, End, Diff;
#endif
    UNREFERENCED_PARAMETER(SystemSpecific1);
    UNREFERENCED_PARAMETER(SystemSpecific2);
    UNREFERENCED_PARAMETER(SystemSpecific3);


#ifdef DPR_INTERRUPTS
    //
    // For latched interrupts, unmask the IMR to force
    // an edge.
    //

    if (Adapter->InterruptLatched) {
        SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK,
                Adapter->InterruptMaskRegister
                );
    }
#endif


    //
    // Loop until there are no more processing sources.
    //

    NdisDprAcquireSpinLock(&Adapter->Lock);

#if DBG
    if (SonicDbg) {

        DbgPrint("In Dpr\n");
    }
#endif
#if 23
#ifdef NDIS_NT
    KeQueryTickCount (&Start);
#endif    
    SonicIndicatedPackets = 0;
    SonicReceiveBuffersExhausted = 0;
    SonicReceiveDescExhausted = 0;
#endif

    while (DidSomething) {

        //
        // Set this FALSE now, so if nothing happens we
        // will exit.
        //

        DidSomething = FALSE;

        //
        // Check for receive interrupts.
        //

        if (Adapter->ProcessingReceiveInterrupt) {

            goto DoneProcessingReceives;

        } else {

            GET_ISR_SINCE_LAST_PROCESSED(
                Adapter,
                &Isr,
                SONIC_INT_PACKET_RECEIVED
                );

            if (Isr & SONIC_INT_PACKET_RECEIVED) {

                Adapter->References++;
                Adapter->ProcessingReceiveInterrupt = TRUE;
                DidSomething = TRUE;

            } else {

                goto DoneProcessingReceives;

            }

        }

        NdisDprReleaseSpinLock(&Adapter->Lock);

        //
        // After we process any
        // other interrupt source we always come back to the top
        // of the loop to check if any more receive packets have
        // come in.  This is to lessen the probability that we
        // drop a receive.
        //
        // ProcessReceiveInterrupts may exit early if it has
        // processed too many receives in a row. In this case
        // it returns FALSE, we don't clear the PACKET_RECEIVED
        // bit, and we will loop through here again.
        //

        if (ProcessReceiveInterrupts(Adapter)) {
            Isr &= ~SONIC_INT_PACKET_RECEIVED;
        }

        IndicateReceiveComplete = TRUE;

        //
        // We set ProcessingReceiveInterrupt to FALSE here so
        // that we can issue new receive indications while
        // the rest of the loop is proceeding.
        //

        NdisDprAcquireSpinLock(&Adapter->Lock);

        Adapter->ProcessingReceiveInterrupt = FALSE;
        Adapter->References--;

DoneProcessingReceives:;

        //
        // NOTE: We have the spinlock here.
        //

        if (Adapter->ProcessingGeneralInterrupt) {

            goto DoneProcessingGeneral;

        } else {

            GET_ISR_SINCE_LAST_PROCESSED(
                Adapter,
                &Isr,
                (USHORT)~SONIC_INT_PACKET_RECEIVED
                );

            //
            // Check the interrupt source and other reasons
            // for processing.  If there are no reasons to
            // process then exit this loop.
            //

            if ((Isr & (SONIC_INT_LOAD_CAM_DONE |
                        SONIC_INT_PROG_INTERRUPT |
                        SONIC_INT_PACKET_TRANSMITTED |
                        SONIC_INT_TRANSMIT_ERROR |
                        SONIC_INT_CRC_TALLY_ROLLOVER |
                        SONIC_INT_FAE_TALLY_ROLLOVER |
                        SONIC_INT_MP_TALLY_ROLLOVER))) {

                Adapter->References++;
                Adapter->ProcessingGeneralInterrupt = TRUE;
                DidSomething = TRUE;

            } else {

                goto DoneProcessingGeneral;

            }

        }


        //
        // Check for a Load CAM completing.
        //
        // This can happen due to a change in the CAM, due to
        // initialization (in which case we won't save the bit
        // and will not come through this code), or a reset
        // (in which case ResetInProgress will be TRUE).
        //
        // Note that we come out of the synchronization above holding
        // the spinlock.
        //


        //
        // Check for non-packet related happenings.
        //

        if (Isr & SONIC_INT_LOAD_CAM_DONE) {

            Isr &= ~SONIC_INT_LOAD_CAM_DONE;

            if (Adapter->ResetInProgress) {

                PSONIC_OPEN Open;
                PLIST_ENTRY CurrentLink;

                //
                // This initialization is from a reset.
                //

                //
                // This will point to the open that
                // initiated the reset.
                //
                PSONIC_OPEN ResettingOpen;

                Adapter->ResetInProgress = FALSE;
                Adapter->IndicatingResetEnd = TRUE;

                //
                // Restart the chip.
                //

                SonicStartChip(Adapter);

                //
                // We save off the open that caused this reset incase
                // we get *another* reset while we're indicating the
                // last reset is done; that would overwrite
                // Adapter->ResettingOpen.
                //

                ResettingOpen = Adapter->ResettingOpen;

                //
                // If anything queued up while we were resetting
                // (in practice that could only be close requests)
                // then restart them now.
                //

                if (Adapter->FirstRequest) {
                    SonicProcessRequestQueue(Adapter);
                }

                //
                // We need to signal every open binding that the
                // reset is complete.  We increment the reference
                // count on the open binding while we're doing indications
                // so that the open can't be deleted out from under
                // us while we're indicating (recall that we can't own
                // the lock during the indication).
                //

                CurrentLink = Adapter->OpenBindings.Flink;

                while (CurrentLink != &Adapter->OpenBindings) {

                    Open = CONTAINING_RECORD(
                             CurrentLink,
                             SONIC_OPEN,
                             OpenList
                             );

                    Open->References++;
                    NdisDprReleaseSpinLock(&Adapter->Lock);

                    NdisIndicateStatus(
                        Open->NdisBindingContext,
                        NDIS_STATUS_RESET_END,
                        NULL,
                        0
                        );

                    NdisIndicateStatusComplete(Open->NdisBindingContext);

                    NdisDprAcquireSpinLock(&Adapter->Lock);
                    Open->References--;

                    CurrentLink = CurrentLink->Flink;

                }

                //
                // Look to see which open initiated the reset.
                //

                NdisDprReleaseSpinLock(&Adapter->Lock);

                NdisCompleteReset(
                    ResettingOpen->NdisBindingContext,
                    NDIS_STATUS_SUCCESS
                    );
                NdisDprAcquireSpinLock(&Adapter->Lock);

                ResettingOpen->References--;
                Adapter->IndicatingResetEnd = FALSE;

                if (Adapter->IndicatingResetStart) {

                    //
                    // Somebody has started a reset while we were
                    // indicating the reset end.
                    //

                    //
                    // Indicate Reset Start
                    //

                    CurrentLink = Adapter->OpenBindings.Flink;

                    while (CurrentLink != &Adapter->OpenBindings) {

                        Open = CONTAINING_RECORD(
                                 CurrentLink,
                                 SONIC_OPEN,
                                 OpenList
                                 );

                        Open->References++;
                        NdisDprReleaseSpinLock(&Adapter->Lock);

                        NdisIndicateStatus(
                            Open->NdisBindingContext,
                            NDIS_STATUS_RESET_START,
                            NULL,
                            0
                            );

                        NdisIndicateStatusComplete(Open->NdisBindingContext);

                        NdisDprAcquireSpinLock(&Adapter->Lock);

                        Open->References--;

                        CurrentLink = CurrentLink->Flink;

                    }

                    Adapter->IndicatingResetStart = FALSE;

                    SetupForReset(
                        Adapter,
                        Adapter->ResettingOpen
                        );

                }

            } else {    // ResetInProgress FALSE

                PNDIS_REQUEST Request;
                PSONIC_REQUEST_RESERVED Reserved;
                PSONIC_OPEN Open;

                Request = Adapter->FirstRequest;
                Reserved = PSONIC_RESERVED_FROM_REQUEST(Request);

                switch (Request->RequestType) {

                case NdisRequestQueryInformation:
                case NdisRequestSetInformation:

                    Adapter->FirstRequest = Reserved->Next;
                    Open = Reserved->OpenBlock;

                    NdisDprReleaseSpinLock(&Adapter->Lock);

                    NdisCompleteRequest(
                        Open->NdisBindingContext,
                        Request,
                        NDIS_STATUS_SUCCESS);

                    NdisDprAcquireSpinLock(&Adapter->Lock);

                    --Open->References;

                    break;

                case NdisRequestQueryStatistics:

                    Adapter->FirstRequest = Reserved->Next;

                    NdisDprReleaseSpinLock(&Adapter->Lock);

                    NdisCompleteQueryStatistics(
                        Adapter->NdisAdapterHandle,
                        Request,
                        NDIS_STATUS_SUCCESS);

                    NdisDprAcquireSpinLock(&Adapter->Lock);

                    break;

                case NdisRequestClose:

                    Adapter->FirstRequest = Reserved->Next;
                    Open = Reserved->OpenBlock;

                    //
                    // The close will get completed when the
                    // reference count goes to 0.
                    //

                    --Open->References;

                }

                //
                // Now continue processing requests if needed
                // (this is called with the lock held).
                //

                SonicProcessRequestQueue(Adapter);

            }

        }

        //
        // We hold the spinlock here.
        //

        if (Isr & (SONIC_INT_CRC_TALLY_ROLLOVER |
                   SONIC_INT_FAE_TALLY_ROLLOVER |
                   SONIC_INT_MP_TALLY_ROLLOVER)) {

            //
            // If any of the counters overflowed, then we update
            // the counter by adding one to the high sixteen bits
            // and reading the register for the low sixteen bits.
            //

            if (Isr & SONIC_INT_CRC_TALLY_ROLLOVER) {

                USHORT CrcError;
                SONIC_READ_PORT(Adapter, SONIC_CRC_ERROR, &CrcError);

                Adapter->GeneralOptional[GO_RECEIVE_CRC - GO_ARRAY_START] =
                    (Adapter->GeneralOptional[GO_RECEIVE_CRC - GO_ARRAY_START] & 0xffff0000) +
                    0x10000 +
                    CrcError;

            }

            if (Isr & SONIC_INT_FAE_TALLY_ROLLOVER) {

                USHORT FaError;
                SONIC_READ_PORT(Adapter, SONIC_FRAME_ALIGNMENT_ERROR, &FaError);

                Adapter->MediaMandatory[MM_RECEIVE_ERROR_ALIGNMENT] =
                    (Adapter->MediaMandatory[MM_RECEIVE_ERROR_ALIGNMENT] & 0xffff0000) +
                    0x10000 +
                    FaError;

            }

            if (Isr & SONIC_INT_MP_TALLY_ROLLOVER) {

                USHORT MissedPacket;
                SONIC_READ_PORT(Adapter, SONIC_MISSED_PACKET, &MissedPacket);

                Adapter->GeneralMandatory[GM_RECEIVE_NO_BUFFER] =
                    (Adapter->GeneralMandatory[GM_RECEIVE_NO_BUFFER] & 0xffff0000) +
                    0x10000 +
                    MissedPacket;

            }

            Isr &= ~(SONIC_INT_CRC_TALLY_ROLLOVER |
                     SONIC_INT_FAE_TALLY_ROLLOVER |
                     SONIC_INT_MP_TALLY_ROLLOVER);

        }

        //
        // Process the transmit interrupts if there are any.
        //

        if (Isr & (SONIC_INT_PROG_INTERRUPT |
                   SONIC_INT_PACKET_TRANSMITTED |
                   SONIC_INT_TRANSMIT_ERROR)) {

            {

                if (!ProcessTransmitInterrupts(Adapter)) {

                    //
                    // Process interrupts returns false if it
                    // finds no more work to do.  If this so we
                    // turn off the transmitter interrupt source.
                    //

                    Isr &= ~ (SONIC_INT_PROG_INTERRUPT |
                              SONIC_INT_PACKET_TRANSMITTED |
                              SONIC_INT_TRANSMIT_ERROR);

                }

            }

        }


        Adapter->ProcessingGeneralInterrupt = FALSE;
        Adapter->References--;


DoneProcessingGeneral:;

        //
        // Note that when we check the for processing sources
        // that we "carefully" check to see if we are already
        // processing the stage queue.  We do this
        // by checking the "AlreadyProcessingSendStage" variable
        // in the adapter.  If this is true then
        // we let whoever set that boolean take care of pushing
        // the packet through the stage queue.
        //

        if ((!Adapter->AlreadyProcessingSendStage) &&
            Adapter->FirstSendStagePacket &&
            Adapter->SendStageOpen) {

            Adapter->References++;
            DidSomething = TRUE;

        } else {

            goto DoneProcessingSend;

        }

        //
        // This routine is called with and returns with the
        // spinlock held, but may release it internally.
        //

        SonicStagedAllocation(Adapter);

        Adapter->References--;


DoneProcessingSend:;

        //
        // NOTE: We have the spinlock here.
        //


        if ((!Adapter->ProcessingDeferredOperations) &&
            (Adapter->FirstLoopBack ||
             (Adapter->ResetInProgress && (Adapter->References == 1)) ||
             (!IsListEmpty(&Adapter->CloseList)))) {

            Adapter->ProcessingDeferredOperations = TRUE;
            Adapter->References++;

            NdisSetTimer(
                &Adapter->DeferredTimer,
                0);

            DidSomething = TRUE;

        }


        //
        // NOTE: This code assumes that the above code left
        // the spinlock held, since the while loop assumes
        // it is held at the beginning.
        //

#ifndef DPR_INTERRUPTS

        //
        // If we have interrups disabled, then we won't get another
        // InterruptServiceRoutine, so we check here for any new
        // bits that are on. We don't worry about receive buffers
        // or descriptors being exhausted, we'll catch that case
        // with another interrupt.
        //

        {
            USHORT LocalIsrValue;

            SONIC_READ_PORT(Adapter, SONIC_INTERRUPT_STATUS, &LocalIsrValue);
            LocalIsrValue &= SONIC_INT_IMMEDIATE_ACK;

            if (LocalIsrValue != 0x0000) {

                //
                // Ack the bits and set DidSomething so we force
                // a loop back to the top.
                //

                SONIC_WRITE_PORT(
                    Adapter,
                    SONIC_INTERRUPT_STATUS,
                    LocalIsrValue
                    );

                Adapter->IsrValue |= LocalIsrValue;
                DidSomething = TRUE;
            }
        }

#endif

    }

#ifndef DPR_INTERRUPTS
    //
    // If we have had interrupts disabled this whole time,
    // then reenable them here to permit future interrupts.
    //
    SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK,
            Adapter->InterruptMaskRegister
            );
#endif
#if 23  
#ifdef NDIS_NT
    KeQueryTickCount (&End);
    Diff = RtlLargeIntegerSubtract(End, Start);
    if (RtlLargeIntegerGreaterThan(Diff, Adapter->SonicMaxDpcTime)) {
        Adapter->SonicMaxDpcTime = Diff;
    }
#endif
    if (SonicIndicatedPackets > Adapter->SonicMaxPackets) {
        Adapter->SonicMaxPackets = SonicIndicatedPackets;
    }
    if (SonicReceiveBuffersExhausted > Adapter->SonicMaxRbExhausted) {
        Adapter->SonicMaxRbExhausted = SonicReceiveBuffersExhausted;
    }
    if (SonicReceiveDescExhausted > Adapter->SonicMaxRdExhausted) {
        Adapter->SonicMaxRdExhausted = SonicReceiveDescExhausted;
    }  
#ifdef NDIS_NT
    KeQueryTickCount (&Start);
#endif    
#endif

    NdisDprReleaseSpinLock(&Adapter->Lock);

    if (IndicateReceiveComplete) {

        //
        // We have indicated at least one packet, we now
        // need to signal every open binding that the
        // receives are complete.
        //

        EthFilterIndicateReceiveComplete(Adapter->FilterDB);

    }
#if 23
#ifdef NDIS_NT
    KeQueryTickCount (&End);
    Diff = RtlLargeIntegerSubtract(End, Start);
    if (RtlLargeIntegerGreaterThan(Diff, Adapter->SonicMaxRcTime)) {
        Adapter->SonicMaxRcTime = Diff;
    }
#endif    
#endif

}

extern
VOID
SonicTimerProcess(
    IN PVOID SystemSpecific1,
    IN PVOID Context,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )

/*++

Routine Description:

    Process the operations that are deferred by SonicDeferredProcessing.

Arguments:

    Context - A pointer to the adapter.

Return Value:

    None.

--*/

{

    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)Context;

    UNREFERENCED_PARAMETER(SystemSpecific1);
    UNREFERENCED_PARAMETER(SystemSpecific2);
    UNREFERENCED_PARAMETER(SystemSpecific3);

    NdisDprAcquireSpinLock(&Adapter->Lock);

    while ((Adapter->FirstLoopBack ||
             (Adapter->ResetInProgress && (Adapter->References == 2)) ||
             (!IsListEmpty(&Adapter->CloseList)))) {

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

        if (Adapter->ResetInProgress && (Adapter->References == 2)) {

            Adapter->ProcessingDeferredOperations = FALSE;
            Adapter->References--;
            NdisDprReleaseSpinLock(&Adapter->Lock);

            StartAdapterReset(Adapter);
            return;

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

        if (Adapter->FirstLoopBack != NULL ) {

            SonicProcessLoopback(Adapter);

        }

        //
        // If there are any opens on the closing list and their
        // reference counts are zero then complete the close and
        // delete them from the list.
        //

        if (!IsListEmpty(&Adapter->CloseList)) {

            PSONIC_OPEN Open;

            Open = CONTAINING_RECORD(
                     Adapter->CloseList.Flink,
                     SONIC_OPEN,
                     OpenList
                     );

            if (!Open->References) {

                NDIS_HANDLE OpenBindingContext = Open->NdisBindingContext;

                RemoveEntryList(&Open->OpenList);
                SONIC_FREE_MEMORY(Open, sizeof(SONIC_OPEN));

                --Adapter->OpenCount;

                NdisDprReleaseSpinLock(&Adapter->Lock);

                NdisCompleteCloseAdapter(
                    OpenBindingContext,
                    NDIS_STATUS_SUCCESS
                    );

                NdisDprAcquireSpinLock(&Adapter->Lock);

            }

        }

        //
        // NOTE: We hold the spinlock here.
        //

    }

    Adapter->ProcessingDeferredOperations = FALSE;
    Adapter->References--;
    NdisDprReleaseSpinLock(&Adapter->Lock);

}

#define SONIC_RECEIVE_LIMIT          10


STATIC
BOOLEAN
ProcessReceiveInterrupts(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    Process the packets that have finished receiving.

    NOTE: This routine assumes that no other thread of execution
    is processing receives!

Arguments:

    Adapter - The adapter to indicate to.

Return Value:

    FALSE if we exit because we have indicated SONIC_RECEIVE_LIMIT
    packets, TRUE if there are no more packets.

--*/

{

    //
    // We don't get here unless there was a receive.  Loop through
    // the receive descriptors starting at the last known descriptor
    // owned by the hardware that begins a packet.
    //
    // Examine each receive ring descriptor for errors.
    //
    // We keep an array whose elements are indexed by the ring
    // index of the receive descriptors.  The arrays elements are
    // the virtual addresses of the buffers pointed to by
    // each ring descriptor.
    //
    // When we have the entire packet (and error processing doesn't
    // prevent us from indicating it), we give the routine that
    // processes the packet through the filter, the buffers virtual
    // address (which is always the lookahead size) and as the
    // MAC context the index to the first and last ring descriptors
    // comprising the packet.
    //


    //
    // Pointer to the receive descriptor being examined.
    //
    PSONIC_RECEIVE_DESCRIPTOR CurrentDescriptor =
                &Adapter->ReceiveDescriptorArea[
                    Adapter->CurrentReceiveDescriptorIndex];

    //
    // Index of the RBA that the next packet should
    // come out of.
    //
    UINT CurrentRbaIndex = Adapter->CurrentReceiveBufferIndex;

    //
    // Virtual address of the start of that RBA.
    //
    PVOID CurrentRbaVa = Adapter->ReceiveBufferArea[CurrentRbaIndex];

    //
    // Physical address of the start of that RBA.
    //
    SONIC_PHYSICAL_ADDRESS CurrentRbaPhysical =
        SONIC_GET_RECEIVE_RESOURCE_ADDRESS(&Adapter->ReceiveResourceArea[CurrentRbaIndex]);

    //
    // The size of the packet.
    //
    UINT PacketSize;

    //
    // The amount of data received in the RBA (will be PacketSize +
    // 4 for the CRC).

    USHORT ByteCount;

    //
    // The amount of lookahead data to indicate.
    //
    UINT LookAheadSize;

    //
    // The offset of the start of the packet in its receive buffer.
    //
    UINT PacketOffsetInRba;

    //
    // The Physical address of the packet.
    //
    SONIC_PHYSICAL_ADDRESS PacketPhysical;

    //
    // A pointer to the link field at the end of the receive
    // descriptor before the one we are processing.
    //
    PSONIC_PHYSICAL_ADDRESS PrevLinkFieldAddr;

    //
    // The virtual address of the packet.
    //
    PVOID PacketVa;

    //
    // The status of the packet.
    //
    USHORT ReceiveStatus;

    //
    // Is the descriptor in use by the sonic.
    //
    ULONG InUse;

    //
    // Used tempoerarily to determine PacketPhysical.
    //
    USHORT PacketAddress;

    //
    // How many packets we have indicated this time.
    //
    UINT PacketsIndicated = 0;

#if DBG
    //
    // For debugging, save the previous receive descriptor.
    //
    static SONIC_RECEIVE_DESCRIPTOR PreviousDescriptor;
#endif
#if 23
    LARGE_INTEGER Start, End, Diff;
#endif


    do {

        //
        // Ensure that the system memory copy of the
        // receive descriptor is up-to-date.
        //

        NdisUpdateSharedMemory(
            Adapter->NdisAdapterHandle,
            sizeof(SONIC_RECEIVE_DESCRIPTOR) *
                Adapter->NumberOfReceiveDescriptors,
            Adapter->ReceiveDescriptorArea,
            Adapter->ReceiveDescriptorAreaPhysical
            );


        //
        // Check to see whether we own the packet.  If
        // we don't then simply return to the caller.
        //

        NdisReadRegisterUlong(&CurrentDescriptor->InUse, &InUse);

        if (InUse != SONIC_OWNED_BY_SYSTEM) {

            return TRUE;

        }
#if 23
        ++SonicIndicatedPackets;
#endif


        //
        // Figure out the virtual address of the packet.
        //

        NdisReadRegisterUshort(&CurrentDescriptor->LowPacketAddress, &PacketAddress);
        PacketPhysical = PacketAddress;
        NdisReadRegisterUshort(&CurrentDescriptor->HighPacketAddress, &PacketAddress);
        PacketPhysical += PacketAddress << 16;

        if ((PacketPhysical < CurrentRbaPhysical) ||
            (PacketPhysical >
                    (CurrentRbaPhysical + SONIC_SIZE_OF_RECEIVE_BUFFERS))) {

            //
            // Something is wrong, the packet is not in the
            // receive buffer that we expect it in.
            //

            SONIC_PHYSICAL_ADDRESS ResourcePhysical;
            PSONIC_RECEIVE_RESOURCE CurrentReceiveResource;
            UINT i;

            if (Adapter->WrongRbaErrorLogCount++ < 5) {

                //
                // Log an error the first five times this happens.
                //

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_HARDWARE_FAILURE,
                    6,
                    processReceiveInterrupts,
                    SONIC_ERRMSG_WRONG_RBA,
                    (ULONG)CurrentRbaPhysical,
                    (ULONG)PacketPhysical,
                    (ULONG)CurrentDescriptor,
                    (ULONG)Adapter->ReceiveDescriptorArea
                    );

#if DBG
                DbgPrint("SONIC: RBA at %lx [%lx], Packet at %lx\n", CurrentRbaPhysical, CurrentRbaVa, PacketPhysical);
                DbgPrint("descriptor %lx, start %lx, prev %lx\n",
                            (ULONG)CurrentDescriptor,
                            (ULONG)Adapter->ReceiveDescriptorArea,
                            &PreviousDescriptor);
#endif
            }

            //
            // Attempt to recover by advancing the relevant pointers
            // to where the SONIC thinks the packet is. First we need
            // to find the receive buffer that matches the indicated
            // physical address.
            //

            for (
                i = 0, CurrentReceiveResource = Adapter->ReceiveResourceArea;
                i < Adapter->NumberOfReceiveBuffers;
                i++,CurrentReceiveResource++
                ) {

                ResourcePhysical = SONIC_GET_RECEIVE_RESOURCE_ADDRESS(CurrentReceiveResource);
                if ((PacketPhysical >= ResourcePhysical) &&
                    (PacketPhysical <
                            (ResourcePhysical + SONIC_SIZE_OF_RECEIVE_BUFFERS))) {

                    //
                    // We found the receive resource.
                    //
                    break;

                }

            }

#ifdef NDIS_NT
            if (i == Adapter->NumberOfReceiveBuffers) {
                KeBugCheck(NDIS_INTERNAL_ERROR);
            }
#endif

            //
            // Update our pointers.
            //

            Adapter->CurrentReceiveBufferIndex = i;
            CurrentRbaIndex = i;

            CurrentRbaVa = Adapter->ReceiveBufferArea[i];
            CurrentRbaPhysical =
                SONIC_GET_RECEIVE_RESOURCE_ADDRESS(&Adapter->ReceiveResourceArea[i]);

            //
            // Ensure that we release buffers before this one
            // back to the sonic.
            //

            WRITE_RWP_AND_ACK(
                Adapter,
                (USHORT)(CurrentRbaPhysical & 0xffff)
                );

        }


        PacketOffsetInRba = PacketPhysical - CurrentRbaPhysical;


        //
        // Check that the packet was received correctly...note that
        // we always compute PacketOffsetInRba and ByteCount,
        // which are needed to skip the packet even if we do not
        // indicate it.
        //

        NdisReadRegisterUshort(&CurrentDescriptor->ReceiveStatus, &ReceiveStatus);

        NdisReadRegisterUshort(&CurrentDescriptor->ByteCount, &ByteCount);

        if (!(ReceiveStatus & SONIC_RCR_PACKET_RECEIVED_OK)) {

#if DBG
            if (SonicDbg) {
                DbgPrint("SONIC: Skipping %lx\n", ReceiveStatus);
            }
#endif

            goto SkipIndication;

        }

        //
        // Prepare to indicate the packet.
        //

        PacketSize = ByteCount - 4;

        if (PacketSize > 1514) {
#if DBG
            DbgPrint("SONIC: Skipping packet, length %d\n", PacketSize);
#endif
            goto SkipIndication;
        }


        if (PacketSize < SONIC_INDICATE_MAXIMUM) {

            LookAheadSize = PacketSize;

        } else {

            LookAheadSize = SONIC_INDICATE_MAXIMUM;

        }

        PacketVa = (PUCHAR)CurrentRbaVa + PacketOffsetInRba;


        //
        // Check just before we do indications that we aren't
        // resetting.
        //

        if (Adapter->ResetInProgress) {
            return TRUE;
        }

        //
        // Ensure that the system memory version of this RBA is
        // up-to-date.
        //

        {
           NDIS_PHYSICAL_ADDRESS TempAddress;

           NdisSetPhysicalAddressLow (TempAddress,
               SONIC_GET_RECEIVE_RESOURCE_ADDRESS(&Adapter->ReceiveResourceArea[CurrentRbaIndex]));
           NdisSetPhysicalAddressHigh (TempAddress, 0);

            NdisUpdateSharedMemory(
                Adapter->NdisAdapterHandle,
                SONIC_SIZE_OF_RECEIVE_BUFFERS,
                Adapter->ReceiveBufferArea[CurrentRbaIndex],
                TempAddress
                );

        }
#if 23
#ifdef NDIS_NT
        KeQueryTickCount (&Start);
#endif        
#endif

        if (PacketSize < 14) {

            //
            // Must have at least the destination address
            //

            if (PacketSize >= ETH_LENGTH_OF_ADDRESS) {

                //
                // Runt packet
                //

                EthFilterIndicateReceive(
                    Adapter->FilterDB,
                    (NDIS_HANDLE)((PUCHAR)PacketVa + 14),  // context
                    PacketVa,                              // destination address
                    PacketVa,                              // header buffer
                    PacketSize,                            // header buffer size
                    NULL,                                  // lookahead buffer
                    0,                                     // lookahead buffer size
                    0                                      // packet size
                    );

            }

        } else {

            EthFilterIndicateReceive(
                Adapter->FilterDB,
                (NDIS_HANDLE)((PUCHAR)PacketVa + 14),  // context
                PacketVa,                              // destination address
                PacketVa,                              // header buffer
                14,                                    // header buffer size
                (PUCHAR)PacketVa + 14,                 // lookahead buffer
                LookAheadSize - 14,                    // lookahead buffer size
                PacketSize - 14                        // packet size
                );

        }
#if 23
#ifdef NDIS_NT
        KeQueryTickCount (&End);
        Diff = RtlLargeIntegerSubtract(End, Start);
        if (RtlLargeIntegerGreaterThan(Diff, Adapter->SonicMaxReceiveTime)) {
            Adapter->SonicMaxReceiveTime = Diff;
        }
#endif
#endif

SkipIndication:;

#if DBG
        SONIC_MOVE_MEMORY (&PreviousDescriptor, CurrentDescriptor, sizeof(SONIC_RECEIVE_DESCRIPTOR));
#endif

        //
        // Give the packet back to the hardware.
        //

        NdisWriteRegisterUlong(&CurrentDescriptor->InUse, SONIC_OWNED_BY_SONIC);

        //
        // And re-set the EOL fields correctly.
        //

        SONIC_SET_END_OF_LIST(
            &(CurrentDescriptor->Link)
            );



        if (CurrentDescriptor == Adapter->ReceiveDescriptorArea) {

            //
            // we are at the first one
            //

            PrevLinkFieldAddr = &(Adapter->LastReceiveDescriptor->Link);

        } else {

            PrevLinkFieldAddr = &((CurrentDescriptor-1)->Link);

        }


        REMOVE_EOL_AND_ACK(
            Adapter,
            PrevLinkFieldAddr
            );



        //
        // Now figure out if the RBA is done with.
        //

        if (ReceiveStatus & SONIC_RCR_LAST_PACKET_IN_RBA) {

            //
            // Advance which RBA we are looking at.
            //

            ++CurrentRbaIndex;

            if (CurrentRbaIndex == Adapter->NumberOfReceiveBuffers) {

                CurrentRbaIndex = 0;

            }

            Adapter->CurrentReceiveBufferIndex = CurrentRbaIndex;

            CurrentRbaVa = Adapter->ReceiveBufferArea[CurrentRbaIndex];
            CurrentRbaPhysical =
                SONIC_GET_RECEIVE_RESOURCE_ADDRESS(&Adapter->ReceiveResourceArea[CurrentRbaIndex]);


            WRITE_RWP_AND_ACK(
                Adapter,
                (USHORT)(CurrentRbaPhysical & 0xffff)
                );

        }

        //
        // Update statistics now based on the receive status.
        //

        if (ReceiveStatus & SONIC_RCR_PACKET_RECEIVED_OK) {

            ++Adapter->GeneralMandatory[GM_RECEIVE_GOOD];

            if (ReceiveStatus & SONIC_RCR_BROADCAST_RECEIVED) {

                ++Adapter->GeneralOptionalFrameCount[GO_BROADCAST_RECEIVES];
                SonicAddUlongToLargeInteger(
                    &Adapter->GeneralOptionalByteCount[GO_BROADCAST_RECEIVES],
                    PacketSize);

            } else if (ReceiveStatus & SONIC_RCR_MULTICAST_RECEIVED) {

                ++Adapter->GeneralOptionalFrameCount[GO_MULTICAST_RECEIVES];
                SonicAddUlongToLargeInteger(
                    &Adapter->GeneralOptionalByteCount[GO_MULTICAST_RECEIVES],
                    PacketSize);

            } else {

                ++Adapter->GeneralOptionalFrameCount[GO_DIRECTED_RECEIVES];
                SonicAddUlongToLargeInteger(
                    &Adapter->GeneralOptionalByteCount[GO_DIRECTED_RECEIVES],
                    PacketSize);

            }

        } else {

            ++Adapter->GeneralMandatory[GM_RECEIVE_BAD];

            if (ReceiveStatus & SONIC_RCR_CRC_ERROR) {
                ++Adapter->GeneralOptional[GO_RECEIVE_CRC - GO_ARRAY_START];
            } else if (ReceiveStatus & SONIC_RCR_FRAME_ALIGNMENT) {
                ++Adapter->MediaMandatory[MM_RECEIVE_ERROR_ALIGNMENT];
            }

        }

        //
        // Advance our pointers to the next packet.

        if (CurrentDescriptor == Adapter->LastReceiveDescriptor) {

            Adapter->CurrentReceiveDescriptorIndex = 0;

        } else {

            ++(Adapter->CurrentReceiveDescriptorIndex);

        }

        CurrentDescriptor = &Adapter->ReceiveDescriptorArea[
                        Adapter->CurrentReceiveDescriptorIndex];

        ++PacketsIndicated;

    } while (PacketsIndicated < SONIC_RECEIVE_LIMIT);

    //
    // Indicate that we returned because we indicated SONIC_RECEIVE_
    // LIMIT packets, not because we ran out of packets to indicate.
    //

    return FALSE;

}

STATIC
BOOLEAN
ProcessTransmitInterrupts(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    Process the packets that have finished transmitting.

    NOTE: Called with the lock held!

Arguments:

    Adapter - The adapter that was sent from.

Return Value:

    This function will return TRUE if it finished up the
    send on a packet.  It will return FALSE if for some
    reason there was no packet to process.

--*/

{
    //
    // Index into the ring to packet structure.  This index points
    // to the first ring entry for the first buffer used for transmitting
    // the packet.
    //
    UINT DescriptorIndex;

    //
    // The transmit desctiptor for the packet at Transmitting Descriptor
    //
    PSONIC_TRANSMIT_DESCRIPTOR TransmitDescriptor;

    //
    // Temporarily holds the transmit descriptor after TransmitDescriptor
    //
    PSONIC_TRANSMIT_DESCRIPTOR NextTransmitDescriptor;

    //
    // Pointer to the packet that started this transmission.
    //
    PNDIS_PACKET OwningPacket;

    //
    // Points to the reserved part of the OwningPacket.
    //
    PSONIC_PACKET_RESERVED Reserved;

    //
    // Used to hold the ring to packet mapping information so that
    // we can release the ring entries as quickly as possible.
    //
    SONIC_DESCRIPTOR_TO_PACKET SavedDescriptorMapping;

    //
    // The status of the transmit.
    //
    USHORT TransmitStatus;

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
#if 23
    LARGE_INTEGER Start, End, Diff;
#endif


    //
    // Get hold of the first transmitted packet.
    //

    //
    // First we check that this is a packet that was transmitted
    // but not already processed.  Recall that this routine
    // will be called repeatedly until this tests false, Or we
    // hit a packet that we don't completely own.
    //

    if (Adapter->TransmittingDescriptor !=
                                Adapter->FirstUncommittedDescriptor) {

        DescriptorIndex =
            Adapter->TransmittingDescriptor - Adapter->TransmitDescriptorArea;

    } else {

        return FALSE;

    }

    //
    // We put the mapping into a local variable so that we
    // can return the mapping as soon as possible.
    //

    SavedDescriptorMapping = Adapter->DescriptorToPacket[DescriptorIndex];

    //
    // Get a pointer to the transmit descriptor for this packet.
    //

    TransmitDescriptor = Adapter->TransmitDescriptorArea + DescriptorIndex;

    //
    // Get a pointer to the owning packet and the reserved part of
    // the packet.
    //

    OwningPacket = SavedDescriptorMapping.OwningPacket;

    Reserved = PSONIC_RESERVED_FROM_PACKET(OwningPacket);


    //
    // Check that status bits were written into the transmit
    // descriptor.
    //

    NdisReadRegisterUshort(&TransmitDescriptor->TransmitStatus, &TransmitStatus);

    if (!(TransmitStatus & SONIC_TCR_STATUS_MASK)) {

        //
        // The transmit has not completed.
        //

        return FALSE;

    } else {

        //
        // Holds whether the packet successfully transmitted or not.
        //
        BOOLEAN Successful = TRUE;

        Adapter->WakeUpTimeout = FALSE;

        if (SavedDescriptorMapping.UsedSonicBuffer) {

            //
            // This packet used adapter buffers.  We can
            // now return these buffers to the adapter.
            //

            //
            // The adapter buffer descriptor that was allocated to this packet.
            //
            PSONIC_BUFFER_DESCRIPTOR BufferDescriptor = Adapter->SonicBuffers +
                                                  SavedDescriptorMapping.SonicBuffersIndex;

            //
            // Index of the listhead that heads the list that the adapter
            // buffer descriptor belongs too.
            //
            INT ListHeadIndex = BufferDescriptor->Next;


            //
            // Put the adapter buffer back on the free list.
            //

            BufferDescriptor->Next = Adapter->SonicBufferListHeads[ListHeadIndex];
            Adapter->SonicBufferListHeads[ListHeadIndex] = SavedDescriptorMapping.SonicBuffersIndex;

        } else {

            //
            // Points to the current ndis buffer being walked.
            //
            PNDIS_BUFFER CurrentBuffer;

            //
            // Which map register we use for this buffer.
            //
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

            CurMapRegister = DescriptorIndex * SONIC_MAX_FRAGMENTS;

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
        // Now release the transmit descriptor, since we have
        // gotten all the information we need from it.
        //

        if (TransmitDescriptor == Adapter->LastTransmitDescriptor) {

            NextTransmitDescriptor = Adapter->TransmitDescriptorArea;

        } else {

            NextTransmitDescriptor = Adapter->TransmittingDescriptor + 1;

        }

        if (TransmitStatus &
               (SONIC_TCR_EXCESSIVE_DEFERRAL |
                SONIC_TCR_EXCESSIVE_COLLISIONS |
                SONIC_TCR_FIFO_UNDERRUN |
                SONIC_TCR_BYTE_COUNT_MISMATCH)) {

            //
            // If the packet completed with an abort state, then we
            // need to restart the transmitter unless we are the
            // last transmit queued up. We set CTDA to point after
            // this descriptor in any case.
            //

#if DBG
            if (SonicDbg) {
                DbgPrint ("SONIC: Advancing CTDA after abort\n");
            }
#endif

            SONIC_WRITE_PORT(Adapter, SONIC_CURR_TRANSMIT_DESCRIPTOR,
                    SONIC_GET_LOW_PART_ADDRESS(
                        NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical) +
                            ((PUCHAR)NextTransmitDescriptor -
                             (PUCHAR)Adapter->TransmitDescriptorArea))
                    );

            if (Adapter->FirstUncommittedDescriptor != NextTransmitDescriptor) {
#if DBG
                if (SonicDbg) {
                    DbgPrint ("SONIC: Restarting transmit after abort\n");
                }
#endif
                SONIC_WRITE_PORT(Adapter, SONIC_COMMAND, SONIC_CR_TRANSMIT_PACKETS);
            }

        }

        Adapter->TransmittingDescriptor = NextTransmitDescriptor;
        Adapter->NumberOfAvailableDescriptors++;

        //
        // Check if the packet completed OK, and update statistics.
        //

        if (!(TransmitStatus & SONIC_TCR_PACKET_TRANSMITTED_OK)) {

#if DBG
            if (SonicDbg) {
                DbgPrint("SONIC: Transmit failed: %lx\n", TransmitStatus);
            }
#endif
            Successful = FALSE;

            ++Adapter->GeneralMandatory[GM_TRANSMIT_BAD];

            if (TransmitStatus & SONIC_TCR_EXCESSIVE_COLLISIONS) {
                ++Adapter->MediaOptional[MO_TRANSMIT_MAX_COLLISIONS];
            }

            if (TransmitStatus & SONIC_TCR_FIFO_UNDERRUN) {
                ++Adapter->MediaOptional[MO_TRANSMIT_UNDERRUN];
            }

        } else {

            INT Collisions = (TransmitStatus & SONIC_TCR_COLLISIONS_MASK) >> SONIC_TCR_COLLISIONS_SHIFT;

            Successful = TRUE;

            ++Adapter->GeneralMandatory[GM_TRANSMIT_GOOD];

            if (Collisions > 0) {
                if (Collisions == 1) {
                    ++Adapter->MediaMandatory[MM_TRANSMIT_ONE_COLLISION];
                } else {
                    ++Adapter->MediaMandatory[MM_TRANSMIT_MORE_COLLISIONS];
                }
            }

            if (TransmitStatus &
                (SONIC_TCR_DEFERRED_TRANSMISSION |
                 SONIC_TCR_NO_CARRIER_SENSE |
                 SONIC_TCR_CARRIER_LOST |
                 SONIC_TCR_OUT_OF_WINDOW)) {

                if (TransmitStatus & SONIC_TCR_DEFERRED_TRANSMISSION) {
                    ++Adapter->MediaOptional[MO_TRANSMIT_DEFERRED];
                }
                if (TransmitStatus & SONIC_TCR_NO_CARRIER_SENSE) {
                    ++Adapter->MediaOptional[MO_TRANSMIT_HEARTBEAT_FAILURE];
                }
                if (TransmitStatus & SONIC_TCR_CARRIER_LOST) {
                    ++Adapter->MediaOptional[MO_TRANSMIT_TIMES_CRS_LOST];
                }
                if (TransmitStatus & SONIC_TCR_OUT_OF_WINDOW) {
                    ++Adapter->MediaOptional[MO_TRANSMIT_LATE_COLLISIONS];
                }
            }

            switch (Reserved->PacketType) {

            case SONIC_DIRECTED:

                ++Adapter->GeneralOptionalFrameCount[GO_DIRECTED_TRANSMITS];
                SonicAddUlongToLargeInteger(
                    &Adapter->GeneralOptionalByteCount[GO_DIRECTED_TRANSMITS],
                    Reserved->PacketLength);
                break;

            case SONIC_MULTICAST:

                ++Adapter->GeneralOptionalFrameCount[GO_MULTICAST_TRANSMITS];
                SonicAddUlongToLargeInteger(
                    &Adapter->GeneralOptionalByteCount[GO_MULTICAST_TRANSMITS],
                    Reserved->PacketLength);
                break;

            case SONIC_BROADCAST:

                ++Adapter->GeneralOptionalFrameCount[GO_BROADCAST_TRANSMITS];
                SonicAddUlongToLargeInteger(
                    &Adapter->GeneralOptionalByteCount[GO_BROADCAST_TRANSMITS],
                    Reserved->PacketLength);
                break;

            }

        }

        //
        // Remove packet from queue.
        //

        if (Adapter->LastFinishTransmit == OwningPacket) {

            Adapter->FirstFinishTransmit = NULL;
            Adapter->LastFinishTransmit = NULL;

        } else {

            Adapter->FirstFinishTransmit = Reserved->Next;
        }

#ifdef CHECK_DUP_SENDS
        {
            VOID SonicRemovePacketFromList(PSONIC_ADAPTER, PNDIS_PACKET);
            SonicRemovePacketFromList(Adapter, OwningPacket);
        }
#endif

        //
        // Now check if the packet needs to be loopbacked as well.  If not,
        // then we complete the send, else after it is loopbacked it will
        // get completed there.
        //

        NdisQueryPacket(
            OwningPacket,
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
        // The binding that is submitting this packet.
        //
        { PSONIC_OPEN Open =
            PSONIC_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

        if ((Open->ProtOptionFlags & NDIS_PROT_OPTION_NO_LOOPBACK) ||
            !EthShouldAddressLoopBack(
                   Adapter->FilterDB,
                   BufferVirtualAddress
                   )) {

            //
            // Along with at least one reference because of the coming
            // indication there should be a reference because of the
            // packet to indicate.
            //

            ASSERT(Open->References > 1);


            NdisDprReleaseSpinLock(&Adapter->Lock);

#if 23
#ifdef NDIS_NT
            KeQueryTickCount (&Start);
#endif
#endif
            NdisCompleteSend(
                Open->NdisBindingContext,
                OwningPacket,
                ((Successful)?(NDIS_STATUS_SUCCESS):(NDIS_STATUS_FAILURE))
                );

#if 23
#ifdef NDIS_NT
            KeQueryTickCount (&End);
            Diff = RtlLargeIntegerSubtract(End, Start);
            if (RtlLargeIntegerGreaterThan(Diff, Adapter->SonicMaxScTime)) {
                Adapter->SonicMaxScTime = Diff;
            }
#endif
#endif
            NdisDprAcquireSpinLock(&Adapter->Lock);

            //
            // We reduce the count by two to account for the fact
            // that we aren't indicating to the open and that one
            // less packet is owned by this open.
            //

            Open->References--;

        } else {

            //
            // Put it on the loopback queue.
            //

            SonicPutPacketOnLoopBack(
                Adapter,
                OwningPacket,
                Successful
                );

        }
        }
        //
        // Since we've given back some ring entries we should
        // open the send stage if it was closed and we are not resetting.
        //

        if ((!Adapter->SendStageOpen) && (!Adapter->ResetInProgress)) {

            Adapter->SendStageOpen = TRUE;

        }

        Adapter->PacketsSinceLastInterrupt = 0;

        return TRUE;
    }

}

VOID
SonicWakeUpDpc(
    IN PVOID SystemSpecific1,
    IN PVOID Context,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )

/*++

Routine Description:

    This DPC routine is queued every 5 seconds to check on the
    transmit descriptor ring. This is to solve problems where
    no status is written into the currently transmitting transmit
    descriptor, which hangs our transmit completion processing.
    If we detect this state, we simulate a transmit interrupt.

Arguments:

    Context - Really a pointer to the adapter.

Return Value:

    None.

--*/
{
    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)Context;
    UINT DescriptorIndex;
    PSONIC_TRANSMIT_DESCRIPTOR TransmitDescriptor;
    USHORT TransmitStatus;


    UNREFERENCED_PARAMETER(SystemSpecific1);
    UNREFERENCED_PARAMETER(SystemSpecific2);
    UNREFERENCED_PARAMETER(SystemSpecific3);

    NdisDprAcquireSpinLock(&Adapter->Lock);

    if (Adapter->WakeUpTimeout) {

        //
        // We had a pending send the last time we ran,
        // and it has not been completed...we need to fake
        // its completion.
        //

        ASSERT (Adapter->TransmittingDescriptor !=
                                    Adapter->FirstUncommittedDescriptor);

        DescriptorIndex =
            Adapter->TransmittingDescriptor - Adapter->TransmitDescriptorArea;

        TransmitDescriptor = Adapter->TransmitDescriptorArea + DescriptorIndex;
        NdisReadRegisterUshort(&TransmitDescriptor->TransmitStatus, &TransmitStatus);

        if (!(TransmitStatus & SONIC_TCR_STATUS_MASK)) {

            NdisWriteRegisterUshort (&TransmitDescriptor->TransmitStatus,
                SONIC_TCR_PACKET_TRANSMITTED_OK);

#if DBG
            DbgPrint ("SONIC: Woke up descriptor at %lx\n", TransmitDescriptor);
#endif

        }

        Adapter->IsrValue |= SONIC_INT_PACKET_TRANSMITTED;

        Adapter->WakeUpTimeout = FALSE;

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_HARDWARE_FAILURE,
            0
            );

    } else if (Adapter->TransmittingDescriptor !=
                                    Adapter->FirstUncommittedDescriptor) {

        DescriptorIndex =
            Adapter->TransmittingDescriptor - Adapter->TransmitDescriptorArea;

        TransmitDescriptor = Adapter->TransmitDescriptorArea + DescriptorIndex;
        NdisReadRegisterUshort(&TransmitDescriptor->TransmitStatus, &TransmitStatus);

        if (!(TransmitStatus & SONIC_TCR_STATUS_MASK)) {

            Adapter->WakeUpTimeout = TRUE;

        }

    }

    NdisDprReleaseSpinLock(&Adapter->Lock);

    //
    // Fire off another Dpc to execute after 5 seconds
    //

    NdisSetTimer(
        &Adapter->WakeUpTimer,
        5000
        );

}

