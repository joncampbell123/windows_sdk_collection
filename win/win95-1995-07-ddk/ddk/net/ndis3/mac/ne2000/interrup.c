/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    interrup.c

Abstract:

    This is a part of the driver for the National Semiconductor Novell 2000
    Ethernet controller.  It contains the interrupt-handling routines.
    This driver conforms to the NDIS 3.0 interface.


Environment:

    Kernel Mode - Or whatever is the equivalent on OS/2 and DOS.

--*/

#include <ndis.h>
#include <efilter.h>
#include "ne2000hw.h"
#include "ne2000sw.h"

#ifdef NDIS_WIN
    #pragma LCODE
#endif

#if DBG
#define STATIC
#else
#define STATIC static
#endif

#if DBG
extern ULONG Ne2000SendsCompletedAfterPendOk;
extern ULONG Ne2000SendsCompletedAfterPendFail;
#endif

UCHAR Ne2000BroadcastAddress[ETH_LENGTH_OF_ADDRESS] =
                                        {0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

//
// This is used to pad short packets.
//

static UCHAR BlankBuffer[60] = "                                                            ";

#if DBG
ULONG Ne2000SendsIssued = 0;
ULONG Ne2000SendsFailed = 0;
ULONG Ne2000SendsPended = 0;
ULONG Ne2000SendsCompletedImmediately = 0;
ULONG Ne2000SendsCompletedAfterPendOk = 0;
ULONG Ne2000SendsCompletedAfterPendFail = 0;
ULONG Ne2000SendsCompletedForReset = 0;
#endif

#if DBG

#define NE2000_LOG_SIZE 256
UCHAR Ne2000LogBuffer[NE2000_LOG_SIZE]={0};
UCHAR Ne2000LogSaveBuffer[NE2000_LOG_SIZE]={0};
UINT Ne2000LogLoc = 0;
BOOLEAN Ne2000LogSave = FALSE;
UINT Ne2000LogSaveLoc = 0;
UINT Ne2000LogSaveLeft = 0;
 
extern
VOID
Ne2000Log(UCHAR c) {

    Ne2000LogBuffer[Ne2000LogLoc++] = c;

    Ne2000LogBuffer[(Ne2000LogLoc + 4) % NE2000_LOG_SIZE] = '\0';

    if (Ne2000LogLoc >= NE2000_LOG_SIZE) Ne2000LogLoc = 0;
}

#endif

#if DBG

#define PACKET_LIST_SIZE 256

static PNDIS_PACKET PacketList[PACKET_LIST_SIZE] = {0};
static PacketListSize = 0;

VOID
AddPacketToList(
    PNE2000_ADAPTER Adapter,
    PNDIS_PACKET NewPacket
    )
    
{
    INT i;

    UNREFERENCED_PARAMETER(Adapter);

    for (i=0; i<PacketListSize; i++) {

        if (PacketList[i] == NewPacket) {

            IF_LOUD(DbgPrint("dup send of %lx\n", NewPacket);)

        }

    }

    PacketList[PacketListSize] = NewPacket;

    ++PacketListSize;

}

VOID
RemovePacketFromList(
    PNE2000_ADAPTER Adapter,
    PNDIS_PACKET OldPacket
    )
    
{
    INT i;

    UNREFERENCED_PARAMETER(Adapter);

    for (i=0; i<PacketListSize; i++) {

        if (PacketList[i] == OldPacket) {

            break;

        }

    }

    if (i == PacketListSize) {

        IF_LOUD(DbgPrint("bad remove of %lx\n", OldPacket);)

    } else {

        --PacketListSize;

        PacketList[i] = PacketList[PacketListSize];

    }

}

#endif  // DBG


BOOLEAN
Ne2000Isr(
    IN PVOID ServiceContext
    )

/*++

Routine Description:

    This is the interrupt handler which is registered with the operating
    system. If several are pending (i.e. transmit complete and receive),
    handle them all.  Block new interrupts until all pending interrupts
    are handled.

Arguments:

    ServiceContext - pointer to the adapter object

Return Value:

    TRUE, if the DPC is to be executed, otherwise FALSE.

--*/

{
    PNE2000_ADAPTER Adapter = ((PNE2000_ADAPTER)ServiceContext);

    IF_LOUD( DbgPrint("In Ne2000ISR\n");)

    IF_LOG( Ne2000Log('i'); )

    IF_VERY_LOUD( DbgPrint( "Ne2000InterruptHandler entered\n" );)

    //
    // Force the INT signal from the chip low. When all
    // interrupts are acknowledged interrupts will be unblocked,
    //

    IF_LOUD( DbgPrint( " blocking interrupts\n" ); )

    CardBlockInterrupts(Adapter); 

    IF_LOG( Ne2000Log('I'); )

    return (TRUE);
}


VOID
Ne2000Dpc(
    IN PVOID SystemSpecific1,
    IN PVOID InterruptContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )
/*++

Routine Description:

    This is the defered processing routine for interrupts, it examines the
    '&Adapter->InterruptStatus' to determine what deffered processing is necessary
    and dispatches control to the Rcv and Xmt handlers.

Arguments:
    SystemSpecific1, SystemSpecific2, SystemSpecific3 - not used
    InterruptContext - a handle to the adapter block.

Return Value:

    NONE.

--*/

{
    PNE2000_ADAPTER Adapter = ((PNE2000_ADAPTER)InterruptContext);

    INTERRUPT_TYPE InterruptType;

    UNREFERENCED_PARAMETER(SystemSpecific1);
    UNREFERENCED_PARAMETER(SystemSpecific2);
    UNREFERENCED_PARAMETER(SystemSpecific3);

    IF_LOUD( DbgPrint("==>IntDpc\n");)

    NdisDprAcquireSpinLock(&Adapter->Lock);

    if (Adapter->DpcInProgress) {

        NdisDprReleaseSpinLock(&Adapter->Lock);

        return;

    }

    IF_LOG( Ne2000Log('d'); )

    Adapter->DpcInProgress;

    Adapter->References++;

    //
    // Get the interrupt bits
    //

    CardGetInterruptStatus(Adapter, &Adapter->InterruptStatus);

    if (Adapter->InterruptStatus != ISR_EMPTY) {

        NdisRawWritePortUchar(Adapter->IoPAddr+NIC_INTR_STATUS, 
                            Adapter->InterruptStatus);

    }
    
    //
    // Return the type of the most important interrupt waiting on the card.
    // Order of importance is COUNTER, OVERFLOW, TRANSMIT,and RECEIVE.
    //
    
    InterruptType = CARD_GET_INTERRUPT_TYPE(Adapter, Adapter->InterruptStatus);

    //
    // Adapter->InterruptStatus bits are used to dispatch to correct DPC and are then cleared
    //

    do {

        while ((InterruptType != UNKNOWN) ||
                ((Adapter->LoopbackQueue != NULL) &&
                !Adapter->ResetInProgress)) {

            //
            // Handle the interrupts
            //

            switch (InterruptType) {

            case COUNTER:

                //
                // One of the counters' MSB has been set, read in all
                // the values just to be sure (and then exit below).
                //

                IF_LOUD( DbgPrint("DPC got COUNTER\n");)

                SyncCardUpdateCounters((PVOID)Adapter);

                // Clear the COUNTER interrupt bit
                Adapter->InterruptStatus &= ~ISR_COUNTER;  

                break;
                
            case OVERFLOW:

                //
                // Overflow interrupts are handled as part of a receive interrupt, 
                // so set a flag and then pretend to be a receive, in case there
                // is no receive already being handled.
                //

                Adapter->BufferOverflow = TRUE;

                IF_LOUD( DbgPrint("Overflow Int\n"); )
                IF_VERY_LOUD( DbgPrint(" overflow interrupt\n"); )

                Adapter->InterruptStatus &= ~ISR_OVERFLOW;

            case RECEIVE:

                //
                // For receives, call this to handle the receive
                //

                IF_LOG( Ne2000Log('R'); )
                IF_LOUD( DbgPrint("DPC got RCV\n"); )
                
                if (Ne2000RcvDpc(Adapter)) {
                    
                    Adapter->InterruptStatus &= ~(ISR_RCV | ISR_RCV_ERR);

                }
                
                IF_LOG( Ne2000Log('r'); )
                
                //
                // Check if we need to process any sends that might have been
                // queued during the receive process
                //

                Ne2000CopyAndSend(Adapter);

                break;

            case TRANSMIT:

                IF_LOG( Ne2000Log('X'); )

#if DBG
                Ne2000LogSave = FALSE;
#endif // DBG

                SyncCardGetXmitStatus((PVOID)Adapter);

                Adapter->WakeUpFoundTransmit = FALSE;

                //
                // This may be false if the card is currently handling an
                // overflow and will restart the Dpc itself.
                //

                ASSERT(!Adapter->OverflowRestartXmitDpc);

                if (Adapter->Ne2000HandleXmitCompleteRunning) {

#if DBG
                    //DbgBreakPoint();
#endif

                } else {

                    Adapter->TransmitInterruptPending = FALSE;
                    Adapter->OctoCount = 0;

                    IF_LOUD( DbgPrint( "DPC got XMIT\n"); )
                    
                    if (Adapter->InterruptStatus & ISR_XMIT_ERR) {

                        IF_LOUD(DbgPrint("Xmit Err\n");)
                        OctogmetusceratorRevisited(Adapter);
                        
                    }

                    if (Adapter->InterruptStatus & ISR_XMIT) {
                    
                        Ne2000XmitDpc(Adapter);

                    }

                }
        
                Adapter->InterruptStatus &= ~(ISR_XMIT | ISR_XMIT_ERR);

                break;

            default:

                IF_LOUD( DbgPrint("unhandled interrupt type: %x\n", InterruptType); )

                break;

            }

            //
            // Handle loopback
            //

            if  ((Adapter->LoopbackQueue != NULL) &&
                !Adapter->ResetInProgress) {

                IF_LOUD( DbgPrint("Dpc doing loopback\n"); )
                
                Ne2000RcvDpc(Adapter);

            }

            InterruptType = CARD_GET_INTERRUPT_TYPE(Adapter, Adapter->InterruptStatus);

        }

        CardGetInterruptStatus(Adapter, &Adapter->InterruptStatus);

        if (Adapter->InterruptStatus != ISR_EMPTY) {

            NdisRawWritePortUchar(Adapter->IoPAddr+NIC_INTR_STATUS, 
                                Adapter->InterruptStatus);

        }

        InterruptType = CARD_GET_INTERRUPT_TYPE(Adapter, Adapter->InterruptStatus);

    } while (InterruptType != UNKNOWN);  // ISR says nothing left to do

    Adapter->DpcInProgress = FALSE;
    
    //
    // Turn IMR back on.
    //
    
    Adapter->NicInterruptMask = IMR_RCV | IMR_XMIT_ERR | IMR_XMIT | IMR_OVERFLOW;

    IF_LOG( Ne2000Log('D'); )

    CardUnblockInterrupts(Adapter);

    NE2000_DO_DEFERRED(Adapter);
    
    IF_LOUD( DbgPrint("<==IntDpc\n"); )

}


BOOLEAN
Ne2000RcvDpc(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    This is the real interrupt handler for receive/overflow interrupt.

    Called when a receive interrupt is received. It first indicates
    all packets on the card and finally indicates
    ReceiveComplete() to every protocol.

    NOTE : THIS IS CALLED WITH THE LOCK HELD!!

Arguments:

    Adapter - Pointer to the adapter block.

Return Value:

    TRUE if done with all receives, else FALSE.

--*/

{
    PNE2000_OPEN TmpOpen;
    PNDIS_PACKET LPacket;
    PMAC_RESERVED Reserved;
    BOOLEAN TransmitInterruptWasPending = FALSE;
    INDICATE_STATUS IndicateStatus = INDICATE_OK;
    BOOLEAN Done = TRUE;
                
    IF_LOUD( DbgPrint( "Ne2000RcvDpc entered\n" );)

    Adapter->IndicateReceiveDone = FALSE;

    //
    // At this point, receive interrupts are disabled.
    //
    
    SyncCardGetCurrent((PVOID)Adapter);

    if (!Adapter->ResetInProgress && Adapter->BufferOverflow) {

        NdisSynchronizeWithInterrupt(
                &(Adapter->NdisInterrupt),
                (PVOID)SyncCardHandleOverflow,
                (PVOID)Adapter
                );

#if DBG
        if (Adapter->OverflowRestartXmitDpc) {

            IF_LOG( Ne2000Log('O');)
            IF_LOUD( DbgPrint ("Adapter->OverflowRestartXmitDpc set:RcvDpc\n"); )

        }
#endif // DBG

    }

    //
    // Loop
    //
    
    while (!Adapter->ResetInProgress) {

        if (Adapter->InterruptStatus & ISR_RCV_ERR) {

            IF_LOUD( DbgPrint ("RCV_ERR, IR=%x\n",Adapter->InterruptStatus); )

            //
            // Skip this packet
            //

            SyncCardGetCurrent((PVOID)Adapter);
            
            Adapter->NicNextPacket = Adapter->Current;

            CardSetBoundary(Adapter);

            break;

        }
        
        if (Adapter->Current == Adapter->NicNextPacket) {

            //
            // Acknowledge previous packet before the check for new ones,
            // then read in the Current register.
            // The card register Current used to point to
            // the end of the packet just received; read
            // the new value off the card and see if it
            // still does.
            //
            // This will store the value in Adapter->Current and acknowledge
            // the receive interrupt.
            //
            //

            SyncCardGetCurrent((PVOID)Adapter);

            if (Adapter->Current == Adapter->NicNextPacket) {

                //
                // End of Loop -- no more packets
                //

                break;
            }

        }

            //
            // A packet was found on the card, indicate it.
            //

            Adapter->ReceivePacketCount++;

            Adapter->LoopbackPacket = (PNDIS_PACKET)NULL;

            if (Ne2000PacketOK(Adapter)) {

                ULONG PacketLen;

                PacketLen = (Adapter->PacketHeader[2]) + ((Adapter->PacketHeader[3])*256) - 4;

                PacketLen = (PacketLen < Adapter->MaxLookAhead)?
                             PacketLen :
                             Adapter->MaxLookAhead;

                if (!CardCopyUp(Adapter,
                                Adapter->Lookahead,
                                Adapter->PacketHeaderLoc,
                                PacketLen + NE2000_HEADER_SIZE
                                )) {

                    IndicateStatus = SKIPPED;

                } else {

                    IndicateStatus = Ne2000IndicatePacket(Adapter);

                    if (IndicateStatus != CARD_BAD) {

                        Adapter->FramesRcvGood++;
                        
                    }

                }
                
            } else {  
            
                IF_LOUD( DbgPrint("Packet did not pass OK check\n"); )

                IndicateStatus = SKIPPED;

            }

        if (IndicateStatus == CARD_BAD) {


            IF_LOG( Ne2000Log('W');)

            //
            // Start off with receive interrupts disabled.
            //

            Adapter->NicInterruptMask =
                    IMR_XMIT | IMR_XMIT_ERR | IMR_OVERFLOW;

            CardReset(Adapter);

            //
            // Since the adapter was just reset, stop indicating packets.
            //

            break;

        }

        //
        // (IndicateStatus == SKIPPED) is OK, just move to next packet.
        //

        if (IndicateStatus == SKIPPED) {

            SyncCardGetCurrent((PVOID)Adapter);
            
            Adapter->NicNextPacket = Adapter->Current;

            CardSetBoundary(Adapter);
        
        } else {

            //
            // Free the space used by packet on card.
            //

            Adapter->NicNextPacket = Adapter->PacketHeader[1];

            //
            // This will set BOUNDARY to one behind NicNextPacket.
            //

            CardSetBoundary(Adapter);

        }

        if (Adapter->ReceivePacketCount > 10) {

            //
            // Give transmit interrupts a chance
            //
            Done = FALSE;
            Adapter->ReceivePacketCount = 0;
            
            break;

        }

    }   

    //
    // See if a buffer overflow occured previously.
    //

    if (Adapter->BufferOverflow) {

        //
        // ... and set a flag to restart the card after receiving
        // a packet.
        //

        Adapter->BufferOverflow = FALSE;
        
        NdisSynchronizeWithInterrupt(
                &(Adapter->NdisInterrupt),
                (PVOID)SyncCardAcknowledgeOverflow,
                (PVOID)Adapter
                );

        //
        // Undo loopback mode
        //

        CardStart(Adapter);

        IF_LOG( Ne2000Log('f'); )

        // 
        // Check if transmission needs to be queued or not
        //       

        if (Adapter->OverflowRestartXmitDpc && Adapter->CurBufXmitting != -1) {

            IF_LOUD( DbgPrint("queueing xmit in RcvDpc\n"); )

            Adapter->OverflowRestartXmitDpc = FALSE;

            Adapter->WakeUpFoundTransmit = FALSE;

            Adapter->TransmitInterruptPending = TRUE;

            CardStartXmit(Adapter);
            
        }   
        
    }

    //
    // Now handle loopback packets.
    //

    IF_LOUD( DbgPrint( " checking loopback queue\n" );)

    while (Adapter->LoopbackQueue && !Adapter->ResetInProgress) {

        IF_LOG( Ne2000Log('L'); )

        //
        // Take the first packet off the loopback queue...
        //

        LPacket = Adapter->LoopbackQueue;

        Reserved = RESERVED(LPacket);

        Adapter->LoopbackQueue = RESERVED(Adapter->LoopbackQueue)->NextPacket;

        Adapter->LoopbackPacket = LPacket;

        Adapter->FramesXmitGood++;

        //
        // Save this, since once we complete the send
        // Reserved is no longer valid.
        //

        TmpOpen = Reserved->Open;

#if DBG
        IF_NE2000DEBUG( NE2000_DEBUG_CHECK_DUP_SENDS ) {

            RemovePacketFromList(Adapter, LPacket);

        }

        Ne2000SendsCompletedAfterPendOk++;
#endif

        //
        // ... and indicate it.
        //

        NdisDprReleaseSpinLock(&Adapter->Lock);

        Ne2000IndicateLoopbackPacket(Adapter, Adapter->LoopbackPacket);

        //
        // Complete the packet send.
        //

        NdisCompleteSend(
                Reserved->Open->NdisBindingContext,
                LPacket,
                NDIS_STATUS_SUCCESS
                );


        NdisDprAcquireSpinLock(&Adapter->Lock);

        TmpOpen->ReferenceCount--;

    }

    //
    // All receives are now done.
    //

    if (Adapter->ResetInProgress) {

        IF_LOG( Ne2000Log('B'); )

        return Done;

    }
    
    //
    // Finally, indicate ReceiveComplete to all protocols which received packets
    //
    
    if (Adapter->IndicateReceiveDone) {
    
        NdisDprReleaseSpinLock(&Adapter->Lock);

        EthFilterIndicateReceiveComplete(Adapter->FilterDB);

        NdisDprAcquireSpinLock(&Adapter->Lock);

        Adapter->IndicateReceiveDone = FALSE;

    }

    IF_LOUD( DbgPrint( "Ne2000RcvDpc exiting\n" );)
    
    return (Done);

}


VOID
Ne2000XmitDpc(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    This is the real interrupt handler for a transmit complete interrupt.
    Ne2000Dpc queues a call to it.

    Called after a transmit complete interrupt. It checks the
    status of the transmission, completes the send if needed,
    and sees if any more packets are ready to be sent.

    NOTE:  CALLED WITH LOCK HELD!

Arguments:

    Adapter  - Pointer to the adapter block.

Return Value:

    None.

--*/

{
    PNDIS_PACKET Packet;
    PMAC_RESERVED Reserved;
    PNE2000_OPEN TmpOpen;
    ULONG Len;
    UINT i;
    
    IF_VERY_LOUD( DbgPrint( "Ne2000XmitDpc entered\n" );)

    IF_LOG( Ne2000Log('C');)

    Adapter->Ne2000HandleXmitCompleteRunning = TRUE;

    if ( Adapter->CurBufXmitting == -1 ) {

        Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

#if DBG
        IF_LOUD(DbgPrint( "Ne2000HandleXmitComplete called with nothing transmitting!\n" );)
        
#endif

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_DRIVER_FAILURE,
            1,
            NE2000_ERRMSG_HANDLE_XMIT_COMPLETE
            );

        return;
    }

    //
    // Statistics
    //
    
    SyncCardGetXmitStatus((PVOID)Adapter);

    if (Adapter->XmitStatus & TSR_XMIT_OK) {

        Adapter->FramesXmitGood++;

#if DBG
        Ne2000SendsCompletedAfterPendOk++;

#endif

    } else {

        Adapter->FramesXmitBad++;

#if DBG
        Ne2000SendsCompletedAfterPendFail++;
#endif

    }

    //
    // Mark the current transmit as done.
    //

    Len = (Adapter->PacketLens[Adapter->CurBufXmitting] + 255) >> 8;

    ASSERT (Len != 0);

    for (i = Adapter->CurBufXmitting; i < Adapter->CurBufXmitting + Len; i++) {
    
                Adapter->BufferStatus[i] = EMPTY;
                
    }

    Adapter->NextBufToXmit += Len;
    
    if (Adapter->NextBufToXmit == MAX_XMIT_BUFS) { 
    
        Adapter->NextBufToXmit = 0;
        
    }
        
    if (Adapter->BufferStatus[Adapter->NextBufToXmit] == EMPTY && 
        Adapter->NextBufToFill != Adapter->NextBufToXmit) {

        Adapter->NextBufToXmit = 0;
        
    }
        
    Adapter->Packets[Adapter->CurBufXmitting] = (PNDIS_PACKET)NULL;

    //
    // See what to do next.
    //

    switch (Adapter->BufferStatus[Adapter->NextBufToXmit]) {

    case FULL:

        //
        // The next packet is ready to go -- only happens with
        // more than one transmit buffer.
        //

        IF_LOUD( DbgPrint( " next packet ready to go\n" );)

        if (Adapter->ResetInProgress) {

            //
            // A reset just started, abort.
            //

            Adapter->CurBufXmitting = -1;

            Adapter->BufferStatus[Adapter->NextBufToXmit] = EMPTY;

            Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

            NdisDprReleaseSpinLock(&Adapter->Lock);

            Ne2000ResetStageDone(Adapter, XMIT_STOPPED);

            NdisDprAcquireSpinLock(&Adapter->Lock);

        } else {

            //
            // Start the transmission and check for more.
            //
 
            Adapter->CurBufXmitting = Adapter->NextBufToXmit;

            IF_LOG( Ne2000Log('2');)

#if DBG

            Ne2000LogSave = TRUE;
            Ne2000LogSaveLeft = 20;

#endif
            Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

            //
            // This is used to check if stopping the chip prevented
            // a transmit complete interrupt from coming through (it
            // is cleared in the ISR if a transmit DPC is queued).
            //

            Adapter->TransmitInterruptPending = TRUE;

            CardStartXmit(Adapter);

            Ne2000CopyAndSend(Adapter);

        }

        break;

    case FILLING:

        //
        // The next packet will be started when copying down is finished.
        //
        
        Adapter->CurBufXmitting = -1;

        Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

        Ne2000CopyAndSend(Adapter);

        break;

    case EMPTY:

        //
        // No packet is ready to transmit.
        //

        IF_LOUD( DbgPrint( " next packet empty\n" );)

        if (Adapter->ResetInProgress) {

            //
            // A reset has just started, exit.
            //

            Adapter->CurBufXmitting = -1;

            Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

            NdisDprReleaseSpinLock(&Adapter->Lock);

            Ne2000ResetStageDone(Adapter, XMIT_STOPPED);

            NdisDprAcquireSpinLock(&Adapter->Lock);

            break;

        }

        if (Adapter->XmitQueue != (PNDIS_PACKET)NULL) {

            //
            // Take the packet off the head of the queue.
            //

            IF_LOUD( DbgPrint( " transmit queue not empty\n" );)

            Packet = Adapter->XmitQueue;

            Adapter->XmitQueue = RESERVED(Adapter->XmitQueue)->NextPacket;

            NdisQueryPacket(Packet, NULL, NULL, NULL, &Len);

            Len = (Len + 255) >> 8; // packet size in 256-byte pages

            if (Adapter->NextBufToFill + Len > MAX_XMIT_BUFS){
            
                Adapter->NextBufToFill = 0;
                
            }

            Adapter->NextBufToXmit = Adapter->NextBufToFill;
            
            //
            // Set this now, to avoid having to get spinlock between
            // copying and transmission start.
            //

            for (i = Adapter->NextBufToFill; i < Adapter->NextBufToFill + Len; i++) {
            
                Adapter->BufferStatus[i] = FULL;
                
            }

            Adapter->Packets[Adapter->NextBufToFill] = Packet;
            
            Adapter->CurBufXmitting = Adapter->NextBufToXmit;

#if DBG

            Ne2000LogSave = TRUE;
            Ne2000LogSaveLeft = 20;

#endif

            //
            // Copy down the data, pad short packets with blanks.
            //

            if (CardCopyDownPacket(Adapter, Packet, 
                        &Adapter->PacketLens[Adapter->NextBufToFill]) == FALSE) {

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteSend(
                        RESERVED(Packet)->Open->NdisBindingContext,
                        Packet,
                        NDIS_STATUS_FAILURE
                        );

                NdisAcquireSpinLock(&Adapter->Lock);

                for (i = Adapter->NextBufToFill; i < Adapter->NextBufToFill + Len; i++) {
                
                    Adapter->BufferStatus[i] = EMPTY;
                    
                }

                Adapter->Packets[Adapter->NextBufToFill] = NULL;

            } else {    

                if (Adapter->PacketLens[Adapter->NextBufToFill] < 60) {

                    (VOID)CardCopyDown(
                        Adapter,
                        ((PUCHAR)Adapter->XmitStart +
                        Adapter->NextBufToFill*TX_BUF_SIZE +
                        Adapter->PacketLens[Adapter->NextBufToFill]),
                        BlankBuffer,
                        60-Adapter->PacketLens[Adapter->NextBufToFill]
                        );

                }

                Adapter->NextBufToFill += Len;
                
                if (Adapter->NextBufToFill == MAX_XMIT_BUFS) {
                
                    Adapter->NextBufToFill = 0;
                    
                }

                Adapter->Ne2000HandleXmitCompleteRunning = FALSE;
        
                //
                // If we are currently handling an overflow, then we need to let
                // the overflow handler send this packet...
                //

                if (Adapter->BufferOverflow) {

                    Adapter->OverflowRestartXmitDpc = TRUE;
    
                    IF_LOG( Ne2000Log('O');)
                    IF_LOUD(DbgPrint ("Adapter->OverflowRestartXmitDpc set:XmitDpc EMPTY");)

                } else {

                    //
                    // This is used to check if stopping the chip prevented
                    // a transmit complete interrupt from coming through (it
                    // is cleared in the ISR if a transmit DPC is queued).
                    //

                    Adapter->TransmitInterruptPending = TRUE;
                    
                    CardStartXmit(Adapter);

                }

                //
                // Ack the send immediately.  If for some reason it
                // should fail, the protocol should be able to handle
                // the retransmit.
                //
    
                ASSERT(Packet != (PNDIS_PACKET)NULL);

                Reserved = RESERVED(Packet);

                if (!Reserved->Loopback) {

                    //
                    // Complete the send if it is not to be loopbacked.
                    //

                    //
                    // Save this, since once we complete the send
                    // Reserved is no longer valid.
                    //

                    TmpOpen = Reserved->Open;

                    IF_LOG( Ne2000Log('p');)

                    NdisDprReleaseSpinLock(&Adapter->Lock);

                    NdisCompleteSend(Reserved->Open->NdisBindingContext,
                            Packet,
                            NDIS_STATUS_SUCCESS);

                    NdisDprAcquireSpinLock(&Adapter->Lock);

                    TmpOpen->ReferenceCount--;

                } else {

                    //
                    // Put it on the loopback queue
                    //

                    if (Adapter->LoopbackQueue == (PNDIS_PACKET)NULL) {

                        Adapter->LoopbackQueue = Packet;
                        Adapter->LoopbackQTail = Packet;

                    } else {

                        RESERVED(Adapter->LoopbackQTail)->NextPacket = Packet;
                        Adapter->LoopbackQTail = Packet;

                    }

                    RESERVED(Packet)->NextPacket = (PNDIS_PACKET)NULL;

                }

            }  

            Ne2000CopyAndSend(Adapter);    

        } else {

            //
            // No packets are waiting on the transmit queue.
            //

            Adapter->CurBufXmitting = -1;

            Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

        }

        break;

        default:

            Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

    }

    IF_VERY_LOUD( DbgPrint( "Ne2000XmitDpc exiting\n" );)

}


INDICATE_STATUS
Ne2000IndicateLoopbackPacket(
    IN PNE2000_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    Indicates an NDIS_format packet to the protocols. This is used
    for indicating packets from the loopback queue.

Arguments:

    Adapter - pointer to the adapter block
    Packet - the packet to be indicated

Return Value:

    SKIPPED if it is a run packet
    INDICATE_OK otherwise.

--*/

{
    UINT IndicateLen;
    UINT PacketLen;

    //
    // Indicate up to 252 bytes.
    //

    NdisQueryPacket(Packet, NULL, NULL, NULL, &PacketLen);

    if (PacketLen < ETH_LENGTH_OF_ADDRESS) {

        //
        // A runt packet.
        //

        return SKIPPED;

    }

    IndicateLen = (PacketLen > (Adapter->MaxLookAhead + NE2000_HEADER_SIZE)) ?
                           (Adapter->MaxLookAhead + NE2000_HEADER_SIZE) :
                           PacketLen;

    //
    // Copy the lookahead data into a contiguous buffer.
    //

    Ne2000CopyOver(Adapter->Lookahead,
                    Packet,
                    0,
                    IndicateLen
                  );

    //
    // Indicate packet
    //

    if (IndicateLen < NE2000_HEADER_SIZE) {

        if (IndicateLen >= ETH_LENGTH_OF_ADDRESS) {
            //
            // Runt packet
            //

            EthFilterIndicateReceive(
                    Adapter->FilterDB,
                    (NDIS_HANDLE)Adapter,
                    (PCHAR)Adapter->Lookahead,
                    Adapter->Lookahead,
                    IndicateLen,
                    NULL,
                    0,
                    0
                    );
        }

    } else {

        EthFilterIndicateReceive(
                Adapter->FilterDB,
                (NDIS_HANDLE)Adapter,
                (PCHAR)Adapter->Lookahead,
                Adapter->Lookahead,
                NE2000_HEADER_SIZE,
                Adapter->Lookahead + NE2000_HEADER_SIZE,
                IndicateLen - NE2000_HEADER_SIZE,
                PacketLen - NE2000_HEADER_SIZE
                );
    }

    Adapter->IndicateReceiveDone = TRUE;

    IF_LOG( Ne2000Log('L'); )
        
    return INDICATE_OK;
    
}


UINT
Ne2000CopyOver(
    OUT PUCHAR Buf,                 // destination
    IN PNDIS_PACKET Packet,         // source packet
    IN UINT Offset,                 // offset in packet
    IN UINT Length                  // number of bytes to copy
    )

/*++

Routine Description:

    Copies bytes from a packet into a buffer. Used to copy data
    out of a packet during loopback indications.

Arguments:

    Buf - the destination buffer
    Packet - the source packet
    Offset - the offset in the packet to start copying at
    Length - the number of bytes to copy

Return Value:

    The actual number of bytes copied; will be less than Length if
    the packet length is less than Offset+Length.

--*/

{
    PNDIS_BUFFER CurBuffer;
    UINT BytesCopied;
    PUCHAR BufVA;
    UINT BufLen;
    UINT ToCopy;
    UINT CurOffset;

    BytesCopied = 0;

    //
    // First find a spot Offset bytes into the packet.
    //

    CurOffset = 0;

    NdisQueryPacket(Packet, NULL, NULL, &CurBuffer, NULL);

    while (CurBuffer != (PNDIS_BUFFER)NULL) {

        NdisQueryBuffer(CurBuffer, (PVOID *)&BufVA, &BufLen);

        if (CurOffset + BufLen > Offset) {

            break;

        }

        CurOffset += BufLen;

        NdisGetNextBuffer(CurBuffer, &CurBuffer);

    }

    //
    // See if the end of the packet has already been passed.
    //

    if (CurBuffer == (PNDIS_BUFFER)NULL) {

        return 0;

    }

    //
    // Now copy over Length bytes.
    //

    BufVA += (Offset - CurOffset);

    BufLen -= (Offset - CurOffset);

    for (;;) {

        ToCopy = (BytesCopied+BufLen > Length) ? Length - BytesCopied : BufLen;

        NE2000_MOVE_MEM(Buf+BytesCopied, BufVA, ToCopy);

        BytesCopied += ToCopy;

        if (BytesCopied == Length) {

            return BytesCopied;

        }

        NdisGetNextBuffer(CurBuffer, &CurBuffer);

        if (CurBuffer == (PNDIS_BUFFER)NULL) {

            break;

        }

        NdisQueryBuffer(CurBuffer, (PVOID *)&BufVA, &BufLen);

    }

    return BytesCopied;

}


BOOLEAN
Ne2000PacketOK(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    Reads a packet off the card -- checking if the CRC is good.  This is
    a workaround for a bug where bytes in the data portion of the packet
    are shifted either left or right by two in some weird 8390 cases.

    This routine is a combination of Ne2000TransferData (to copy up data
    from the card), CardCalculateCrc and CardCalculatePacketCrc.

Arguments:

    Adapter - pointer to the adapter block.

Return Value:

    TRUE if the packet seems ok, else false.

--*/

{
    
    UINT BytesNow, PacketLen, PacketLength;
    PUCHAR CurCardLoc, PacketLoc, CurBufLoc;

    // Header Validation Variables
    BOOLEAN FrameAlign;
    PUCHAR PacketRcvStatus;
    PUCHAR NextPacket;
    PUCHAR PacketLenLo;
    PUCHAR PacketLenHi;
    PUCHAR ReceiveDestAddrLo;
    UINT FrameAlignCount;
    UCHAR OldPacketLenHi;
    UCHAR TempPacketHeader[6];
    PUCHAR BeginPacketHeader;
    UCHAR Temp;

    //
    // First copy up the four-byte header the card attaches
    // plus first two bytes of the data packet (which contain
    // the destination address of the packet).  We use the extra
    // two bytes in case the packet was shifted right 1 or 2 bytes
    //

    PacketLoc = Adapter->PageStart +
        256*(Adapter->NicNextPacket-Adapter->NicPageStart);

    if (!CardCopyUp(Adapter, TempPacketHeader, PacketLoc, 6)) {

        return FALSE;

    }

    PacketLoc += 4;

    //
    // Validate the header
    //

    FrameAlignCount = 0;
    BeginPacketHeader = TempPacketHeader;

    do {

        PacketRcvStatus = BeginPacketHeader;
        NextPacket = BeginPacketHeader + 1;
        PacketLenLo = BeginPacketHeader + 2;
        PacketLenHi = BeginPacketHeader + 3;
        OldPacketLenHi = *PacketLenHi;
        ReceiveDestAddrLo = BeginPacketHeader + 4;
        FrameAlign = FALSE;

        if (*PacketRcvStatus & 0x05E){

            FrameAlign = TRUE;

        } else if ((*PacketRcvStatus & RSR_MULTICAST)   // If a multicast packet
                && (!FrameAlignCount)              // and hasn't been aligned
                && !(*ReceiveDestAddrLo & 1)      // and lsb is set on dest addr
                ){

                FrameAlign = TRUE;

        } else {

            // compare high and low address bytes.  If the same, the low
            // byte may have been copied into the high byte.

            if (*PacketLenLo == *PacketLenHi){
            
                //Save the old packetlenhi
                OldPacketLenHi = *PacketLenHi;

                //Compute new packet length
                *PacketLenHi = *NextPacket - Adapter->NicNextPacket - 1;

                if (*PacketLenHi < 0) {

                    *PacketLenHi = (Adapter->NicPageStop - Adapter->NicNextPacket) +
                        (*NextPacket - Adapter->NicPageStart) - 1;

                }

                if (*PacketLenLo > 0xFC) {

                    *PacketLenHi++;
                }

            }

            PacketLen = (*PacketLenLo) + ((*PacketLenHi)*256) - 4;

            if ((PacketLen > 1514) || (PacketLen < 60)){

                //Bad length.  Restore the old packetlenhi
                *PacketLenHi = OldPacketLenHi;
                
                FrameAlign = TRUE;

            }

            if (!FrameAlign && ((*NextPacket < Adapter->NicPageStart) ||
                (*NextPacket > Adapter->NicPageStop))) {

                IF_LOUD( DbgPrint ("Packet address invalid in HeaderValidation\n"); )

                FrameAlign = TRUE;

            }

        }

        //
        // FrameAlignment - if first time through, shift packetheader right 1 or 2 bytes.
        // If second time through, shift it back to where it was and let it through.
        // This compensates for a known bug in the 8390D chip.
        //
        if (FrameAlign){

            switch (FrameAlignCount){

            case 0:

                BeginPacketHeader++;
                PacketLoc++;
                if (!Adapter->EightBitSlot){

                    BeginPacketHeader++;
                    PacketLoc++;

                }
                break;

            case 1:

                BeginPacketHeader--;
                PacketLoc--;
                if (!Adapter->EightBitSlot){
                    BeginPacketHeader--;
                    PacketLoc--;
                }
                break;

            }

            FrameAlignCount++;

        }

    } while ( (FrameAlignCount < 2) && FrameAlign );

    Adapter->PacketHeader[0] = *BeginPacketHeader;
    BeginPacketHeader++;
    Adapter->PacketHeader[1] = *BeginPacketHeader;
    BeginPacketHeader++;
    Adapter->PacketHeader[2] = *BeginPacketHeader;
    BeginPacketHeader++;
    Adapter->PacketHeader[3] = *BeginPacketHeader;

    //
    // Packet length is in bytes 3 and 4 of the header.
    //

    Adapter->PacketHeaderLoc = PacketLoc;
    PacketLen = (Adapter->PacketHeader[2]) + ((Adapter->PacketHeader[3])*256) - 4;

    if ((PacketLen > 1514) || (PacketLen < 60)){

        if ((Adapter->PacketHeader[1] < Adapter->NicPageStart) ||
            (Adapter->PacketHeader[1] > Adapter->NicPageStop)) {

            //
            // Return TRUE here since IndicatePacket will notice the error
            // and handle it correctly.
            //

            return(TRUE);

        }

        return(FALSE);

    }

    return(TRUE);

}


INDICATE_STATUS
Ne2000IndicatePacket(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    Indicates the first packet on the card to the protocols.

    NOTE: This assumes that the packet header has been
    read into Adapter->PacketHeader and the minimal lookahead stored in
    Adapter->Lookahead

    NOTE: Called with lock held!!!

Arguments:

    Adapter - pointer to the adapter block.

Return Value:

    CARD_BAD if the card should be reset;
    INDICATE_OK otherwise.

--*/

{
    UINT PacketLen;
    UINT IndicateLen;
    UCHAR PossibleNextPacket1, PossibleNextPacket2;
    
    //
    // Check if the next packet byte agress with the length.
    // The start of the packet plus the MSB of the length must
    // be equal to the start of the next packet minus one or two.
    // Otherwise the header is considered corrupted, and the
    // card must be reset.
    //

    PossibleNextPacket1 =
                Adapter->NicNextPacket + Adapter->PacketHeader[3] + (UCHAR)1;

    if (PossibleNextPacket1 >= Adapter->NicPageStop) {

        PossibleNextPacket1 -= (Adapter->NicPageStop - Adapter->NicPageStart);

    }

    if (PossibleNextPacket1 != Adapter->PacketHeader[1]) {

        PossibleNextPacket2 = PossibleNextPacket1+(UCHAR)1;

        if (PossibleNextPacket2 == Adapter->NicPageStop) {

            PossibleNextPacket2 = Adapter->NicPageStart;

        }

        if (PossibleNextPacket2 != Adapter->PacketHeader[1]) {

            IF_LOUD( DbgPrint("First CARD_BAD check failed\n"); )
            return SKIPPED;
        }

    }

    //
    // Check that the Next is valid
    //

    if ((Adapter->PacketHeader[1] < Adapter->NicPageStart) ||
        (Adapter->PacketHeader[1] > Adapter->NicPageStop)) {

        IF_LOUD( DbgPrint("Second CARD_BAD check failed\n"); )
        return(SKIPPED);

    }

    PacketLen = Adapter->PacketHeader[2] + Adapter->PacketHeader[3]*256 - 4;

    if (PacketLen > 1514) {
    
        IF_LOUD( DbgPrint("Third CARD_BAD check failed\n"); )
        return(SKIPPED);

    }

#if DBG

    IF_NE2000DEBUG( NE2000_DEBUG_WORKAROUND1 ) {
        //
        // Now check for the high order 2 bits being set. If either
        // of the two high order bits is set in the receive status byte
        // in the packet header, the packet should be skipped (but
        // the adapter does not need to be reset).
        //

        if (Adapter->PacketHeader[0] & (RSR_DISABLED|RSR_DEFERRING)) {

            IF_LOUD (DbgPrint("H");)

            return SKIPPED;

        }

    }

#endif

    IndicateLen = (PacketLen > (Adapter->MaxLookAhead + NE2000_HEADER_SIZE)) ?
                           (Adapter->MaxLookAhead + NE2000_HEADER_SIZE) :
                           PacketLen;

    //
    // Indicate packet
    //

    Adapter->PacketLen = PacketLen;

    NdisDprReleaseSpinLock(&Adapter->Lock);

    if (IndicateLen < NE2000_HEADER_SIZE) {

        //
        // Runt Packet
        //

        EthFilterIndicateReceive(
                Adapter->FilterDB,
                (NDIS_HANDLE)Adapter,
                (PCHAR)(Adapter->Lookahead),
                (PCHAR)(Adapter->Lookahead),
                IndicateLen,
                NULL,
                0,
                0
                );

    } else {

        EthFilterIndicateReceive(
                Adapter->FilterDB,
                (NDIS_HANDLE)Adapter,
                (PCHAR)(Adapter->Lookahead),
                (PCHAR)(Adapter->Lookahead),
                NE2000_HEADER_SIZE,
                (PCHAR)(Adapter->Lookahead) + NE2000_HEADER_SIZE,
                IndicateLen - NE2000_HEADER_SIZE,
                PacketLen - NE2000_HEADER_SIZE
                );

    }

    NdisDprAcquireSpinLock(&Adapter->Lock);

    Adapter->IndicateReceiveDone = TRUE;
                                                                                                                                                                                                                        
    return INDICATE_OK;
    
}


NDIS_STATUS
Ne2000TransferData(
    IN NDIS_HANDLE MacBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    )

/*++

Routine Description:

    NDIS function.

Arguments:

    see NDIS 3.0 spec.

Notes:

  - The MacReceiveContext will be a pointer to the open block for
    the packet.
  - The LoopbackPacket field in the adapter block will be NULL if this
    is a call for a normal packet, otherwise it will be set to point
    to the loopback packet.

--*/

{
    UINT BytesLeft, BytesNow, BytesWanted;
    PUCHAR CurLookaheadLoc;
    PNDIS_BUFFER CurBuffer;
    PUCHAR BufVA, BufStart;
    UINT BufLen, BufOff, Copied;
    UINT CurOff;
    PNE2000_ADAPTER Adapter = ((PNE2000_ADAPTER)MacReceiveContext);

    UNREFERENCED_PARAMETER(MacBindingHandle);

    IF_LOG( Ne2000Log('t');)

    ByteOffset += NE2000_HEADER_SIZE;

    //
    // Determine whether this was a loopback indication.
    //

    if (Adapter->LoopbackPacket != (PNDIS_PACKET)NULL) {

        //
        // Yes, have to copy data from Adapter->LoopbackPacket into Packet.
        //

        NdisQueryPacket(Packet, NULL, NULL, &CurBuffer, NULL);

        CurOff = ByteOffset;

        while (CurBuffer != (PNDIS_BUFFER)NULL) {

            NdisQueryBuffer(CurBuffer, (PVOID *)&BufVA, &BufLen);

            Copied =
                Ne2000CopyOver(BufVA, Adapter->LoopbackPacket, CurOff, BufLen);

            CurOff += Copied;

            if (Copied < BufLen) {

                break;

            }

            NdisGetNextBuffer(CurBuffer, &CurBuffer);

        }

        //
        // We are done, return.
        //

        if (CurOff < ByteOffset) {

            *BytesTransferred = 0;
            IF_LOG( Ne2000Log('T');)
            return(NDIS_STATUS_FAILURE);

        }

        *BytesTransferred = CurOff - ByteOffset;

        IF_LOG( Ne2000Log('T');)

        return NDIS_STATUS_SUCCESS;

    }

    //
    // NOT a loopback packet.
    // See how much data there is to transfer.
    //

    if (ByteOffset+BytesToTransfer > Adapter->PacketLen) {

        if (Adapter->PacketLen < ByteOffset) {

           *BytesTransferred = 0;
           IF_LOG( Ne2000Log('T');)
            return(NDIS_STATUS_FAILURE);

        }

        BytesWanted = Adapter->PacketLen - ByteOffset;

    } else {

        BytesWanted = BytesToTransfer;

    }

    BytesLeft = BytesWanted;

    {

        PUCHAR CurCardLoc;

        //
        // Copy data from the card -- it is not completely stored in the
        // adapter structure.
        //
        // Determine where the copying should start.
        //

        CurCardLoc = Adapter->PacketHeaderLoc + ByteOffset;

        if (CurCardLoc > Adapter->PageStop) {

            CurCardLoc = CurCardLoc - (Adapter->PageStop - Adapter->PageStart);

        }

        NdisQueryPacket(Packet, NULL, NULL, &CurBuffer, NULL);

        NdisQueryBuffer(CurBuffer, (PVOID *)&BufStart, &BufLen);

        BufOff = 0;

        //
        // Loop, filling each buffer in the packet until there
        // are no more buffers or the data has all been copied.
        //

        NdisAcquireSpinLock(&Adapter->Lock);

        while (BytesLeft > 0) {

            //
            // See how much data to read into this buffer.
            //

            if ((BufLen-BufOff) > BytesLeft) {

                BytesNow = BytesLeft;

            } else {

                BytesNow = (BufLen - BufOff);

            }

            //
            // See if the data for this buffer wraps around the end
            // of the receive buffers (if so filling this buffer
            // will use two iterations of the loop).
            //

            if (CurCardLoc + BytesNow > Adapter->PageStop) {

                BytesNow = Adapter->PageStop - CurCardLoc;

            }

            //
            // Copy up the data.
            //

            if (!CardCopyUp(Adapter, BufStart+BufOff, CurCardLoc, BytesNow)) {

                *BytesTransferred = BytesWanted - BytesLeft;

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_HARDWARE_FAILURE,
                    1,
                    0x2
                    );

                NdisReleaseSpinLock(&Adapter->Lock);

                return NDIS_STATUS_FAILURE;

            }

            CurCardLoc += BytesNow;

            BytesLeft -= BytesNow;

            //
            // Is the transfer done now?
            //

            if (BytesLeft == 0) {

                break;

            }

            //
            // Wrap around the end of the receive buffers?
            //

            if (CurCardLoc == Adapter->PageStop) {

                CurCardLoc = Adapter->PageStart;

            }

            //
            // Was the end of this packet buffer reached?
            //

            BufOff += BytesNow;

            if (BufOff == BufLen) {

                NdisGetNextBuffer(CurBuffer, &CurBuffer);

                if (CurBuffer == (PNDIS_BUFFER)NULL) {

                    break;

                }

                NdisQueryBuffer(CurBuffer, (PVOID *)&BufStart, &BufLen);

                BufOff = 0;

            }

        }

        *BytesTransferred = BytesWanted - BytesLeft;

        NdisReleaseSpinLock(&Adapter->Lock);

        //
        // Did a transmit complete while we were doing what we were doing?
        // If so, and another transmit is waiting, start it!
        //
        
        if (!Adapter->BufferOverflow && Adapter->CurBufXmitting != -1) {
        
            ULONG Len;
            UINT i;
            UCHAR Status;
    
            CardGetInterruptStatus(Adapter, &Status);

            if (Status & ISR_XMIT_ERR) {

                IF_LOUD(DbgPrint("Xmit Err\n");)
                OctogmetusceratorRevisited(Adapter);
            
            } 
        
            if (Status & (ISR_XMIT)) {


                IF_LOG( Ne2000Log('*'); )
            
                // Ack the transmit
                NdisRawWritePortUchar(Adapter->IoPAddr+NIC_INTR_STATUS, (ISR_XMIT));
                SyncCardGetXmitStatus((PVOID)Adapter);
                Adapter->TransmitInterruptPending = FALSE;
                Adapter->OctoCount = 0;
                        
                // Statistics
                if (Adapter->XmitStatus & TSR_XMIT_OK) {

                    Adapter->FramesXmitGood++;
                    
                    #if DBG
                    Ne2000SendsCompletedAfterPendOk++;
                    #endif
                    
                } else {

                    Adapter->FramesXmitBad++;
                    
                    #if DBG
                        Ne2000SendsCompletedAfterPendFail++;
                    #endif
                    
                }
            
                // Update NextBufToXmit
                Len = (Adapter->PacketLens[Adapter->CurBufXmitting] + 255) >> 8;

                for (i = Adapter->CurBufXmitting; i < Adapter->CurBufXmitting + Len; i++) {
                
                    Adapter->BufferStatus[i] = EMPTY;
                    
                }

                Adapter->NextBufToXmit += Len;

                if (Adapter->NextBufToXmit == MAX_XMIT_BUFS) { 
                
                    Adapter->NextBufToXmit = 0;
                    
                }
        
                if (Adapter->BufferStatus[Adapter->NextBufToXmit] == EMPTY && 
                    Adapter->NextBufToFill != Adapter->NextBufToXmit) {
                    
                    Adapter->NextBufToXmit = 0;
                    
                }

                Adapter->Packets[Adapter->CurBufXmitting] = (PNDIS_PACKET)NULL;
                Adapter->WakeUpFoundTransmit = FALSE;
                
                // If the next packet is ready to go, start it.
                if (Adapter->BufferStatus[Adapter->NextBufToXmit] == FULL) {

                    Adapter->CurBufXmitting = Adapter->NextBufToXmit;  
                    Adapter->TransmitInterruptPending = TRUE;

                    CardStartXmit(Adapter);

                } else {
 
                    Adapter->CurBufXmitting = -1;

                }

            } 

        }

        return NDIS_STATUS_SUCCESS;

    }

}


NDIS_STATUS
Ne2000Send(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    )

/*++

Routine Description:

    NDIS function.

Arguments:

    See NDIS 3.0 spec.

Notes:


--*/

{
    PNE2000_OPEN Open = ((PNE2000_OPEN)MacBindingHandle);
    PNE2000_ADAPTER Adapter = Open->Adapter;
    PMAC_RESERVED Reserved = RESERVED(Packet);
    PNDIS_BUFFER CurrentBuffer;
    PUCHAR Address;
    ULONG i;

#if DBG
    Ne2000SendsIssued++;
#endif

    //
    // Ensure that the open won't close during this function.
    //

    if (Open->Closing) {

#if DBG
        Ne2000SendsFailed++;
#endif

        return NDIS_STATUS_CLOSING;
    }

    //
    // All requests are rejected during a reset.
    //

    NdisAcquireSpinLock(&Adapter->Lock);

    if (Adapter->ResetInProgress) {

#if DBG
        Ne2000SendsFailed++;
#endif

        NdisReleaseSpinLock(&Adapter->Lock);

        return NDIS_STATUS_RESET_IN_PROGRESS;

    }

    Open->ReferenceCount++;

    Adapter->References++;

    //
    // Set up the MacReserved section of the packet.
    //

    Reserved->Open = Open;

    //
    // Set Reserved->Loopback
    //

    NdisQueryPacket(
        Packet,
        NULL,
        NULL,
        &CurrentBuffer,
        NULL
        );

    //
    // Get address from first buffer.
    //

    NdisQueryBuffer(
        CurrentBuffer,
        (PVOID)&Address,
        &i
        );

    if (Open->ProtOptionFlags & NDIS_PROT_OPTION_NO_LOOPBACK){
    
        Reserved->Loopback = FALSE;        
        
    } else {
    
        Reserved->Loopback = EthShouldAddressLoopBack(
                                    Adapter->FilterDB,
                                    Address
                                    );
    }

#if DBG
    IF_NE2000DEBUG( NE2000_DEBUG_CHECK_DUP_SENDS ) {

        AddPacketToList(Adapter, Packet);

    }
#endif

    //
    // Put it on the loopback queue only.  All packets go through the
    // loopback queue first, and then on to the xmit queue.
    //

#if DBG
    Ne2000SendsPended++;
#endif

    //
    // We do not Open->ReferenceCount-- because that will be done when
    // then send completes.
    //

    //
    // Put Packet on queue to hit the wire.
    //

    IF_VERY_LOUD( DbgPrint("Putting 0x%x on, after 0x%x\n",Packet,Adapter->XmitQTail); )

    if (Adapter->XmitQueue != NULL) {

        RESERVED(Adapter->XmitQTail)->NextPacket = Packet;

    } else {

        Adapter->XmitQueue = Packet;

    }

    Adapter->XmitQTail = Packet;

    Reserved->NextPacket = NULL;

    NE2000_DO_DEFERRED(Adapter);

    return NDIS_STATUS_PENDING;
    
}


VOID
Ne2000CopyAndSend(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    Copies packets from the transmit queue to the board and starts
    transmission as long as there is data waiting. Must be called
    with Lock held.

Arguments:

    Adapter - pointer to the adapter block

Return Value:

    None.

--*/

{
    XMIT_BUF TmpBuf1;
    PNDIS_PACKET Packet;
    ULONG Len;
    UINT i;
    PMAC_RESERVED Reserved;
    PNE2000_OPEN TmpOpen;

    //
    // Loop as long as there is data on the transmit queue
    // and space for it on the card.
    //

    while  (Adapter->XmitQueue != NULL && 
        Adapter->BufferStatus[Adapter->NextBufToFill] == EMPTY) {
            
        NdisQueryPacket(Adapter->XmitQueue, NULL, NULL, NULL, &Len);

        Len = (Len + 255) >> 8; // packet size in 256-byte pages
              
        if (Adapter->CurBufXmitting == -1) { 
            if (Adapter->BufferStatus[Adapter->NextBufToXmit] == EMPTY) {
                if (Adapter->NextBufToFill + Len > MAX_XMIT_BUFS) {
                    Adapter->NextBufToFill = 0;
                }
            } else {
                if (Adapter->NextBufToXmit > Adapter->NextBufToFill) {
                    if (Adapter->NextBufToFill + Len > Adapter->NextBufToXmit) {
                        return;
                    }
                } else {
                    if (Adapter->NextBufToFill + Len > MAX_XMIT_BUFS) {
                        Adapter->NextBufToFill = 0;
                        if (Adapter->NextBufToFill + Len > Adapter->NextBufToXmit){
                            return;
                        }
                    }            
                }
            }        
        } else {
            if (Adapter->CurBufXmitting > Adapter->NextBufToFill) {
                if (Adapter->NextBufToFill + Len > Adapter->CurBufXmitting) {
                    return;
                }
            } else {
                if (Adapter->NextBufToFill + Len > MAX_XMIT_BUFS) {
                    Adapter->NextBufToFill = 0;
                    if (Adapter->NextBufToFill + Len > Adapter->CurBufXmitting){
                        return;
                    }
                }            
            }
        }
        
        TmpBuf1 = Adapter->NextBufToFill;
        
        //
        // Take the packet off of the transmit queue.
        //

        Packet = Adapter->XmitQueue;

        IF_VERY_LOUD( DbgPrint("Removing 0x%x, New Head is 0x%x\n",Packet,RESERVED(Packet)->NextPacket); )

        Adapter->XmitQueue = RESERVED(Packet)->NextPacket;
            
        for (i = TmpBuf1; i < TmpBuf1 + Len; i++) {
        
            Adapter->BufferStatus[i] = FILLING;
            
        }
        
        Adapter->Packets[TmpBuf1] = Packet;

        //
        // copy down the data, pad short packets with blanks.
        //

        if (CardCopyDownPacket(Adapter, Packet, 
                        &Adapter->PacketLens[TmpBuf1]) == FALSE) {

            NdisReleaseSpinLock(&Adapter->Lock);

            NdisCompleteSend(
                        RESERVED(Packet)->Open->NdisBindingContext,
                        Packet,
                        NDIS_STATUS_FAILURE
                        );

            NdisAcquireSpinLock(&Adapter->Lock);

            for (i = TmpBuf1; i < TmpBuf1 + Len; i++) {
            
                Adapter->BufferStatus[i] = EMPTY;
                
            }
            
            RESERVED(Packet)->Open->ReferenceCount--;

            Adapter->Packets[TmpBuf1] = NULL;

            continue;

        }

        if (Adapter->PacketLens[TmpBuf1] < 60) {

            (VOID)CardCopyDown(
                    Adapter,
                    ((PUCHAR)Adapter->XmitStart +
                    TmpBuf1*TX_BUF_SIZE +
                    Adapter->PacketLens[TmpBuf1]),
                    BlankBuffer,
                    60-Adapter->PacketLens[TmpBuf1]
                    );

        }

        if (Adapter->ResetInProgress) {

            PNE2000_OPEN TmpOpen = (PNE2000_OPEN)((RESERVED(Packet)->Open));

            //
            // A reset just started, abort.
            //
            //
            
            for (i = TmpBuf1; i < TmpBuf1 + Len; i++) {
            
                Adapter->BufferStatus[i] = EMPTY;
                
            }

            NdisReleaseSpinLock(&Adapter->Lock);

            //
            // Complete the send.
            //

            NdisCompleteSend(
                        RESERVED(Packet)->Open->NdisBindingContext,
                        Packet,
                        NDIS_STATUS_SUCCESS
                        );

            Ne2000ResetStageDone(Adapter, BUFFERS_EMPTY);

            NdisAcquireSpinLock(&Adapter->Lock);

            TmpOpen->ReferenceCount--;

            return;
            
        }

        for (i = TmpBuf1; i < TmpBuf1 + Len; i++) {
        
                Adapter->BufferStatus[i] = FULL;
                
        }

        Adapter->NextBufToFill += Len;
        if (Adapter->NextBufToFill == MAX_XMIT_BUFS) {
        
            Adapter->NextBufToFill = 0;
            
        }
        
        //
        // See whether to start the transmission.
        //

        if (Adapter->CurBufXmitting == -1) {

            //
            // OK to start transmission.
            //
            
            if (Adapter->BufferStatus[Adapter->NextBufToXmit] == EMPTY && 
                Adapter->NextBufToFill != Adapter->NextBufToXmit) {
                
                Adapter->NextBufToXmit = 0;
                
            }

            Adapter->CurBufXmitting = Adapter->NextBufToXmit;
    
#if DBG

            Ne2000LogSave = TRUE;
            Ne2000LogSaveLeft = 20;

#endif

            //
            // If we are currently handling an overflow, then we need to let
            // the overflow handler send this packet...
            //

            if (Adapter->BufferOverflow) {

                Adapter->OverflowRestartXmitDpc = TRUE;

                IF_LOG( Ne2000Log('O');)
                IF_LOUD(DbgPrint ("Adapter->OverflowRestartXmitDpc set:copy and send");)
                
            } else {

                //
                // This is used to check if stopping the chip prevented
                // a transmit complete interrupt from coming through (it
                // is cleared in the ISR if a transmit DPC is queued).
                //

                Adapter->TransmitInterruptPending = TRUE;

                CardStartXmit(Adapter);

            }

        }

        //
        // Ack the send immediately.  If for some reason it
        // should fail, the protocol should be able to handle
        // the retransmit.
        //
    
        ASSERT(Packet != (PNDIS_PACKET)NULL);

        Reserved = RESERVED(Packet);

        if (!Reserved->Loopback) {

            //
            // Complete the send if it is not to be loopbacked.
            //

            //
            // Save this, since once we complete the send
            // Reserved is no longer valid.
            //

            TmpOpen = Reserved->Open;
            
            IF_LOG( Ne2000Log('p');)

            NdisDprReleaseSpinLock(&Adapter->Lock);

            NdisCompleteSend(Reserved->Open->NdisBindingContext,
                    Packet,
                    NDIS_STATUS_SUCCESS);

            NdisDprAcquireSpinLock(&Adapter->Lock);

            TmpOpen->ReferenceCount--;

        } else {

            //
            // Put it on the loopback queue
            //

            if (Adapter->LoopbackQueue == (PNDIS_PACKET)NULL) {

                Adapter->LoopbackQueue = Packet;
                Adapter->LoopbackQTail = Packet;

            } else {

                RESERVED(Adapter->LoopbackQTail)->NextPacket = Packet;
                Adapter->LoopbackQTail = Packet;

            }

            RESERVED(Packet)->NextPacket = (PNDIS_PACKET)NULL;

        }

    } 

}


VOID
Ne2000WakeUpDpc(
    IN PVOID SystemSpecific1,
    IN PVOID Context,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )

/*++

Routine Description:

    This DPC routine is queued every 2 seconds to check on the
    transmit queue. If a transmit interrupt was not received
    in the last two seconds and there is a transmit in progress,
    then we complete the transmit.

Arguments:

    Context - Really a pointer to the adapter.

Return Value:

    None.

--*/
{
    PNE2000_ADAPTER Adapter = (PNE2000_ADAPTER)Context;

    UNREFERENCED_PARAMETER(SystemSpecific1);
    UNREFERENCED_PARAMETER(SystemSpecific2);
    UNREFERENCED_PARAMETER(SystemSpecific3);

    if ((Adapter->WakeUpFoundTransmit) &&
        (Adapter->CurBufXmitting != -1)) {

        NdisAcquireSpinLock(&Adapter->Lock);
        
        //
        // We had a transmit pending the last time we ran,
        // and it has not been completed...we need to complete
        // it now.

        Adapter->WakeUpFoundTransmit = FALSE;

        IF_LOG( Ne2000Log('K');)

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_HARDWARE_FAILURE,
            1,
            0x3
            );

        //
        // We stop and start the card, then queue a DPC to
        // handle the receive.
        //
#if DBG        
        IF_LOUD(DbgPrint("@\n"); )
#endif
        CardStop(Adapter);
        
        //
        // If after 5 kickstarts it is still dead, do a complete reset
        //
        if (Adapter->OctoCount > 4){
        
            CardReset(Adapter); 
            Adapter->OctoCount = 0;
            
        }
        
        OctogmetusceratorRevisited(Adapter);

        Adapter->OctoCount++;
        
        NdisReleaseSpinLock(&Adapter->Lock);

    } else {

        if (Adapter->CurBufXmitting != -1) {

            Adapter->WakeUpFoundTransmit = TRUE;

            IF_LOG( Ne2000Log('L');)

        }

    }

    //
    // Fire off another Dpc to execute after 2 seconds
    //

    NdisSetTimer(
        &Adapter->WakeUpTimer,
        2000
        );

}

VOID
OctogmetusceratorRevisited(
    IN PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    Recovers the card from a transmit error.
    
Arguments:

    Adapter - pointer to the adapter block

Return Value:

    None.

--*/

{

    IF_LOUD( DbgPrint("Octogmetuscerator called!"); )

    // Ack the interrupt, if needed
    NdisRawWritePortUchar(Adapter->IoPAddr+NIC_INTR_STATUS, ISR_XMIT_ERR);

    // Stop the card
    SyncCardStop(Adapter);

    // Wait up to 2 milliseconds for any receives to finish
    NdisStallExecution(2000);

    // Place the card in Loopback
    NdisRawWritePortUchar(Adapter->IoPAddr+NIC_XMIT_CONFIG, TCR_LOOPBACK);

    // Start the card in Loopback
    NdisRawWritePortUchar(Adapter->IoPAddr+NIC_COMMAND, CR_START | CR_NO_DMA);

    // Get out of loopback and start the card
    CardStart(Adapter);

    // If there was a packet waiting to get sent, send it.
    if (Adapter->CurBufXmitting != -1) {

        Adapter->TransmitInterruptPending = TRUE;
    
        CardStartXmit(Adapter);
        
    }        

}

