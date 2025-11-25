/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    lancesft.h

Abstract:

    The main header for a LANCE (Local Area Network Controller
    Am 7990) MAC driver.

Author:

    Anthony V. Ercolano (tonye) creation-date 19-Jun-1990

Environment:

    This driver is expected to work in DOS, OS2 and NT at the equivalent
    of kernal mode.

    Architecturally, there is an assumption in this driver that we are
    on a little endian machine.

Notes:

    optional-notes

Revision History:

    31-Jul-1992  R.D. Lanser:

       Changed DECST card type to DECTC for the DEC TurboChannel option
       PMAD-AA (Lance ethernet).  This option will be available for all
       TurboChannel systems regardless of CPU type or system.

       Added  InterruptRequestLevel to the _LANCE_ADAPTER structure because
       'lance.c' was passing the InterruptVector as the IRQL to the interrupt
       connect routine which is not correct.  This works on JAZZ because the
       JAZZ HalGetInterruptVector is hardcoded to return a fixed IRQL for
       EISA devices.

       Removed PhysicalBuffersContained and UsedLanceBuffer field from
       _ADAPTER structure.  SeanSe says that the code related to this
       field was used for adevice that is no longer supported.  I removed
       the dependent code(or at least what was obvious) from 'send.c'.

--*/

#ifndef _LANCESFT_
#define _LANCESFT_

#define LANCE_NDIS_MAJOR_VERSION 3
#define LANCE_NDIS_MINOR_VERSION 0

#if DBG

#define LANCELOG 1
#define LANCE_TRACE 0

#else

#define LANCELOG 0
#define LANCE_TRACE 0

#endif

#if LANCELOG

#define LOG_SIZE 256

#define TIMER      '.'
#define IN_ISR     'i'
#define OUT_ISR    'I'
#define IN_DPC     'd'
#define OUT_DPC    'D'
#define RECEIVE    'R'
#define TRANSMIT   'x'
#define TRANSMIT_COMPLETE   'X'
#define PEND       'p'
#define UNPEND     'P'
#define INDICATE   'r'
#define IN_SEND    's'
#define OUT_SEND   'S'
#define START      'G'
#define RESET_STEP_1 '1'
#define RESET_STEP_2 '2'
#define RESET_SAVE_PACKET     'b'
#define RESET_RECOVER_PACKET  'B'
#define RESET_COMPLETE_PACKET 'c'
#define RESET_STEP_3 '3'



extern UCHAR Log[LOG_SIZE];

extern UCHAR LogPlace;
extern UCHAR LogWrapped;



#define LOG(c)     { Log[LogPlace] = (c); Log[(LogPlace+3) % 255] ='\0'; \
                     LogPlace = (LogPlace + 1) % 255; }

#else

#define LOG(c)

#endif



extern NDIS_PHYSICAL_ADDRESS HighestAcceptableMax;

//
// ZZZ These macros are peculiar to NT.
//

#define LANCE_ALLOC_PHYS(pp, s) NdisAllocateMemory((PVOID *)(pp),(s),0,HighestAcceptableMax)
#define LANCE_FREE_PHYS(p, s) NdisFreeMemory((PVOID)(p),(s),0)
#define LANCE_MOVE_MEMORY(Destination,Source,Length) NdisMoveMemory(Destination,Source,Length)
#define LANCE_ZERO_MEMORY(Destination,Length) NdisZeroMemory(Destination,Length)



//
// Definitions for all the different card types.
//


#define LANCE_DE100      0x01    // DE100 card
#define LANCE_DE201      0x02    // DE201 card
#define LANCE_DEPCA      0x04    // Card in a Dec PC x86 machine
#define LANCE_DECST      0x08    // Card in a decstation
#define LANCE_DE422      0x10    // DE422 card
#define LANCE_DECTC      0x20    // TurboChannel PMAD-AA option

#define LANCE_DE100_NAME LANCE_DEFAULT_NAME
#define LANCE_DE201_NAME LANCE_DEFAULT_NAME
#define LANCE_DEPCA_NAME LANCE_DEFAULT_NAME
#define LANCE_DECST_NAME LANCE_DEFAULT_NAME
#define LANCE_DE422_NAME LANCE_DEFAULT_NAME
#define LANCE_DECTC_NAME LANCE_DEFAULT_NAME
#define LANCE_DEFAULT_NAME "\\Device\\Lance01"


//
// This structure is passed as context from the receive interrupt
// processor.  Eventually it will be used as a parameter to
// LanceTransferData.  LanceTransferData can get two kinds of
// context.  It will receive either an ndis packet or it will
// receive a LANCE_RECEIVE_CONTEXT.  It will be able to tell
// the difference since the LANCE_RECEIVE_CONTEXT will have
// its low bit set.  No pointer to an ndis packet can have its low
// bit set.
//
typedef union _LANCE_RECEIVE_CONTEXT {

    UINT WholeThing;
    struct _INFO {
        //
        // Used to mark that this is context rather than a pointer
        // to a packet.
        //
        UINT IsContext:1;

        //
        // The first receive ring descriptor used to hold the packet.
        //
        UINT FirstBuffer:7;

        //
        // The last receive ring descriptor used to hold the packet.
        //
        UINT LastBuffer:7;
    } INFO;

} LANCE_RECEIVE_CONTEXT,*PLANCE_RECEIVE_CONTEXT;






//
// This record type is inserted into the MacReserved portion
// of the packet header when the packet is going through the
// staged allocation of buffer space prior to the actual send.
//
typedef struct _LANCE_RESERVED {

    //
    // Points to the next packet in the chain of queued packets
    // being allocated, loopbacked, or waiting for the finish
    // of transmission.
    //
    // The packet will either be on the stage list for allocation,
    // the loopback list for loopback processing, on an adapter
    // wide doubly linked list (see below) for post transmission
    // processing.
    //
    // We always keep the packet on a list so that in case the
    // the adapter is closing down or resetting, all the packets
    // can easily be located and "canceled".
    //
    PNDIS_PACKET Next;

    //
    // This field holds the binding handle of the open binding
    // that submitted this packet for send.
    //
    NDIS_HANDLE MacBindingHandle;

    //
    // This gives the index into the array of adapter buffer
    // descriptors that contains the packet information.
    //
    USHORT LanceBuffersIndex;

    //
    // When the hardware send is done this will record whether
    // the send was successful or not.
    //
    BOOLEAN SuccessfulTransmit;

} LANCE_RESERVED,*PLANCE_RESERVED;

//
// This macro will return a pointer to the lance reserved portion
// of a packet given a pointer to a packet.
//
#define PLANCE_RESERVED_FROM_PACKET(Packet) \
    ((PLANCE_RESERVED)((Packet)->MacReserved))

//
// This structure is used to map entries in the ring descriptors
// back to the packets from which the data in the ring descriptor
// originated.
//

typedef struct _LANCE_RING_TO_PACKET {

    //
    // Points to the packet from which data is being transmitted
    // through this ring entry.
    //
    PNDIS_PACKET OwningPacket;

    //
    // Index of the ring entry that is being used by the packet.
    //
    UINT RingIndex;

    //
    // When a packet is submitted to the hardware we record
    // here whether it used adapter buffers and if so, the buffer
    // index.
    //
    UINT LanceBuffersIndex;

} LANCE_RING_TO_PACKET,*PLANCE_RING_TO_PACKET;

//
// If an ndis packet does not meet the hardware contraints then
// an adapter buffer will be allocated.  Enough data will be copied
// out of the ndis packet so that by using a combination of the
// adapter buffer and remaining ndis buffers the hardware
// constraints are satisfied.
//
// In the LANCE_ADAPTER structure three threaded lists are kept in
// one array.  One points to a list of LANCE_BUFFER_DESCRIPTORS
// that point to small adapter buffers.  Another is for medium sized
// buffers and the last for full sized (large) buffers.
//
// The allocation is controlled via a free list head and
// the free lists are "threaded" by a field in the adapter buffer
// descriptor.
//
typedef struct _LANCE_BUFFER_DESCRIPTOR {

    //
    // A virtual pointer to a small, medium, or large buffer.
    //
    PVOID VirtualLanceBuffer;

    //
    // Threads the elements of an array of these descriptors into
    // a free list. -1 implies no more entries in the list.
    //
    INT Next;

    //
    // Holds the amount of space (in bytes) available in the buffer
    //
    UINT BufferSize;

    //
    // Holds the length of data placed into the buffer.  This
    // can (and likely will) be less that the actual buffers
    // length.
    //
    UINT DataLength;

} LANCE_BUFFER_DESCRIPTOR,*PLANCE_BUFFER_DESCRIPTOR;



//
// Used to contain a queued operation.
//

typedef struct _LANCE_PEND_DATA {
    PNDIS_REQUEST Next;
    struct _LANCE_OPEN * Open;
    NDIS_REQUEST_TYPE RequestType;
} LANCE_PEND_DATA, * PLANCE_PEND_DATA;

//
// This macro will return a pointer to the reserved area of
// a PNDIS_REQUEST.
//
#define PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(Request) \
   ((PLANCE_PEND_DATA)((PVOID)((Request)->MacReserved)))

//
// This macros returns the enclosing NdisRequest.
//
#define PNDIS_REQUEST_FROM_PLANCE_PEND_DATA(PendOp)\
   ((PNDIS_REQUEST)((PVOID)(PendOp)))



//
// Define the size of the ethernet header.
//
#define LANCE_HEADER_SIZE 14

//
// Define Maximum number of bytes a protocol can read during a
// receive data indication.
//
#define LANCE_MAX_LOOKAHEAD ( 248 - LANCE_HEADER_SIZE )





typedef struct _LANCE_ADAPTER {

    //
    // The card type of this adapter.
    //
    UCHAR LanceCard;

    //
    // Holds the interrupt object for this adapter.
    //
    NDIS_INTERRUPT Interrupt;

    //
    // Normal processing DPC.
    //
    PVOID InterruptDPC;

    //
    // Dpc for handling loopback.
    //
    PVOID LoopbackDPC;

    //
    // Non OS fields of the adapter.
    //

    //
    // Contains Address first byte of adapter memory is mapped to.
    //
    PVOID MmMappedBaseAddr;

    //
    // Contains address of the hardware memory.
    //
    PVOID HardwareBaseAddr;

    //
    // Offset for Init block from the Lance chip's point of view.
    //
    ULONG HardwareBaseOffset;

    //
    // Amount of memory
    //
    ULONG AmountOfHardwareMemory;

    //
    // For lance implementation that uses dual ported memory this
    // field is used to point to the first available memory.
    //
    PVOID CurrentMemoryFirstFree;

    //
    // Address of the first byte following the memory.
    //
    PVOID MemoryFirstUnavailable;

    //
    // Pointer to the RAP register.
    //
    ULONG RAP;

    //
    // Pointer to the RDP register.
    //
    ULONG RDP;

    //
    // Pointer to the NICSR register.
    //
    ULONG Nicsr;

    //
    // Slot Number the De422 is in.
    //
    UINT SlotNumber;

    //
    // Default information to add to the NICSR register value
    //
    USHORT NicsrDefaultValue;

    //
    // Have the interrupts from the card been turned off.
    //
    BOOLEAN InterruptsStopped;

    //
    // Address in memory of the network address
    //
    ULONG NetworkHardwareAddress;

    //
    // The network address from the hardware.
    //
    CHAR NetworkAddress[ETH_LENGTH_OF_ADDRESS];

    //
    // The network address from the hardware.
    //
    CHAR CurrentNetworkAddress[ETH_LENGTH_OF_ADDRESS];

    //
    // Interrupt number
    //
    CCHAR InterruptNumber;

    //
    // IRQL
    //
    CCHAR InterruptRequestLevel;

    // The following fields are accessed by the ISR and must be aligned to the
    // minimum granularity of the architecture on which it runs

#if defined(_ALPHA_)

    union {
        UQUAD _ForceQuadwordAlignment;
        struct {

#endif // defined(_ALPHA_)

            //
            // Holds a value of CSR0 from one or more interrupts.
            // This value is only set by the interrupt service routine
            // and cleared by the interrupt synchronization routine.
            //
            USHORT CSR0Value;

#if defined(_ALPHA_)

        };
    };

#endif // defined(_ALPHA_)

    // End of ISR access fields

    //
    // Holds the number of transmit ring entries.
    //
    // NOTE NOTE NOTE
    //
    // There is code that depends on the number of transmit entries
    // being a power of two.
    //
    UINT NumberOfTransmitRings;

    //
    // Holds the number of receive ring entries.
    //
    UINT NumberOfReceiveRings;

    //
    // Holds the size of receive buffers.
    //
    UINT SizeOfReceiveBuffer;

    //
    // Holds number of each buffer size.
    //
    UINT NumberOfSmallBuffers;
    UINT NumberOfMediumBuffers;
    UINT NumberOfLargeBuffers;

    //
    // The log base two of the number of transmit ring entries.
    //
    UINT LogNumberTransmitRings;

    //
    // The log base two of the number of receive ring entries.
    //
    UINT LogNumberReceiveRings;

    //
    // Keeps a reference count on the current number of uses of
    // this adapter block.  Uses is defined to be the number of
    // routines currently within the "external" interface.
    //
    UINT References;

    //
    // List head for all open bindings for this adapter.
    //
    LIST_ENTRY OpenBindings;

    //
    // List head for all opens that had outstanding references
    // when an attempt was made to close them.
    //
    LIST_ENTRY CloseList;

    //
    // Spinlock to protect fields in this structure..
    //
    NDIS_SPIN_LOCK Lock;

    //
    // Handle given by NDIS when the MAC registered itself.
    //
    NDIS_HANDLE NdisMacHandle;

    //
    // Handle given by NDIS when the adapter was registered.
    //
    NDIS_HANDLE NdisAdapterHandle;

    //
    // Timer for Deferred Processing.
    //
    NDIS_TIMER DeferredTimer;

    //
    // Flag if we have a ProcessInterrupt call in progress
    //
    BOOLEAN ProcessInterruptRunning;

    //
    // Pointer to the filter database for the MAC.
    //
    PETH_FILTER FilterDB;

    //
    // Pointer to the LANCE initialization block.
    //
    PLANCE_INITIALIZATION_BLOCK InitBlock;

    //
    // Counter that records the number of transmit rings currently
    // available for allocation.
    //
    UINT NumberOfAvailableRings;

    //
    // Pointer to transmit descriptor ring entry that is the
    // first ring entry available for allocation of transmit
    // buffers.
    //
    // Can only be accessed when the adapter lock
    // is held.
    //
    PLANCE_TRANSMIT_ENTRY AllocateableRing;

    //
    // Pointer to a transmit descriptor ring entry that is the
    // first ring entry that the MAC currently has made available
    // for transmission.
    //
    // Can only be accessed when the adapter lock
    // is held.
    //
    PLANCE_TRANSMIT_ENTRY TransmittingRing;

    //
    // Pointer to the first packet that has been allocated to
    // a transmit packet but has not yet been relinquished to
    // the hardware.  We need this pointer to keep the transmit
    // post processing from running into a packet that has not
    // been transmitted.
    //
    PLANCE_TRANSMIT_ENTRY FirstUncommittedRing;

    //
    // Pointer to an array of structs that map transmit ring entries
    // back to a packet.
    //
    PLANCE_RING_TO_PACKET RingToPacket;

    //
    // Pointer to the transmit ring.
    //
    PLANCE_TRANSMIT_ENTRY TransmitRing;

    //
    // Pointer to the last transmit ring entry.
    //
    PLANCE_TRANSMIT_ENTRY LastTransmitRingEntry;

    //
    // Pointer to the receive ring.
    //
    PLANCE_RECEIVE_ENTRY ReceiveRing;

    //
    // Pointer to the first packet on the loopback list.
    //
    // Can only be accessed when the adapter lock
    // is held.
    //
    PNDIS_PACKET FirstLoopBack;

    //
    // Pointer to the last packet on the loopback list.
    //
    // Can only be accessed when the adapter lock
    // is held.
    //
    PNDIS_PACKET LastLoopBack;

    //
    // Pointer to the first transmitting packet that is actually
    // sending, or done with the living on the loopback queue.
    //
    // Can only be accessed when the adapter lock
    // is held.
    //
    PNDIS_PACKET FirstFinishTransmit;

    //
    // Pointer to the last transmitting packet that is actually
    // sending, or done with the living on the loopback queue.
    //
    // Can only be accessed when the adapter lock
    // is held.
    //
    PNDIS_PACKET LastFinishTransmit;

    //
    // Listheads for the adapters buffers.  If the list
    // head is equal to -1 then there are no free elements
    // on the list.
    //
    // The list heads must only be accessed when the
    // adapter lock is held.
    //
    // Note that the listhead at index 0 will always be -1.
    //
    INT LanceBufferListHeads[4];

    //
    // Pointers to an array of adapter buffer descriptors.
    // The array will actually be threaded together by
    // three free lists.  The lists will be for small,
    // medium and full sized packets.
    //
    PLANCE_BUFFER_DESCRIPTOR LanceBuffers;

    //
    // These fields let the send allocation code know that it's
    // futile to even try to move a packet along to that stage.
    //
    // All of the stages would be closed to close a binding
    // or to reset the adapter.
    //
    // These variables can only be accessed when the adapter
    // lock is held.
    //
    BOOLEAN StageOpen;

    //
    // These AlreadyProcessingStage variables are set up to keep
    // more than one thread from accessing a particular thread
    // a one time.
    //
    // These variables can only be accessed when the adapter
    // lock is held.
    //
    BOOLEAN AlreadyProcessingStage;

    //
    // Did we indicate a packet?  Used to tell if NdisIndicateReceiveComplete
    // should be called.
    //
    BOOLEAN IndicatedAPacket;

    //
    // Pointers to the first and last packets at a particular stage
    // of allocation.  All packets in transmit are linked
    // via there next field.
    //
    // Can only be accessed when the adapter lock
    // is held.
    //
    PNDIS_PACKET FirstStage1Packet;
    PNDIS_PACKET LastStage1Packet;

    //
    // Pointer to an array of virtual addresses that describe
    // the virtual address of each receive ring descriptor buffer.
    //
    PVOID *ReceiveVAs;

    //
    // Index of the receive ring descriptor that started the
    // last packet not completely received by the hardware.
    //
    UINT CurrentReceiveIndex;

    //
    // Counters to hold the various number of errors/statistics for both
    // reception and transmission.
    //
    // Can only be accessed when the adapter lock is held.
    //
    UINT OutOfReceiveBuffers;
    UINT CRCError;
    UINT FramingError;
    UINT RetryFailure;
    UINT LostCarrier;
    UINT LateCollision;
    UINT UnderFlow;
    UINT Deferred;
    UINT OneRetry;
    UINT MoreThanOneRetry;

    //
    // Holds counts of more global errors for the driver.  If we
    // get a memory error then the device needs to be reset.
    //
    UINT MemoryError;
    UINT Babble;
    UINT MissedPacket;

    //
    // Holds other cool counts.
    //

    ULONG Transmit;
    ULONG Receive;

    //
    // Flag that when enabled lets routines know that a reset
    // is in progress.
    //
    BOOLEAN ResetInProgress;

    //
    // Flag that when enabled lets routines know that a reset
    // is in progress and the initialization needs doing.
    //
    BOOLEAN ResetInitStarted;

    //
    // Pointer to the binding that initiated the reset.  This
    // will be null if the reset is initiated by the MAC itself.
    //
    struct _LANCE_OPEN *ResettingOpen;

    //
    // The NdisRequest that is causing the reset (either set
    // packet filter or set multicast list)
    //
    PNDIS_REQUEST ResetNdisRequest;

    //
    // The type of the request that caused the adapter to reset.
    //
    NDIS_REQUEST_TYPE ResetRequestType;


    //
    // A queue of NdisRequests that were queued during a reset.
    //
    PNDIS_REQUEST PendQueue;
    PNDIS_REQUEST PendQueueTail;

    //
    // During an indication this is set to the current indications context
    //
    LANCE_RECEIVE_CONTEXT IndicatingMacReceiveContext;


    //
    // Look ahead information.
    //

    ULONG MaxLookAhead;

    //
    // Open information
    //
    UCHAR MaxMulticastList;

    //
    // Will be true the first time that the hardware is initialized
    // by the driver initialization.
    //
    BOOLEAN FirstInitialization;

    //
    // Will be true if the hardware fails for some reason
    //
    BOOLEAN HardwareFailure;

    //
    // For handling missing interrupts (caused by user mis-configs)
    //

    PVOID WakeUpDpc;
    NDIS_TIMER WakeUpTimer;
    BOOLEAN WakeUpTimeout;
    UCHAR TimeoutCount;

    //
    // Lookahead buffer for loopback packets
    //
    UCHAR Lookahead[LANCE_MAX_LOOKAHEAD + LANCE_HEADER_SIZE];

} LANCE_ADAPTER,*PLANCE_ADAPTER;

//
// Given a MacBindingHandle this macro returns a pointer to the
// LANCE_ADAPTER.
//
#define PLANCE_ADAPTER_FROM_BINDING_HANDLE(Handle) \
    (((PLANCE_OPEN)(Handle))->OwningLance)

//
// Given a MacContextHandle return the PLANCE_ADAPTER
// it represents.
//
#define PLANCE_ADAPTER_FROM_CONTEXT_HANDLE(Handle) \
    ((PLANCE_ADAPTER)(Handle))

//
// Given a pointer to a LANCE_ADAPTER return the
// proper MacContextHandle.
//
#define CONTEXT_HANDLE_FROM_PLANCE_ADAPTER(Ptr) \
    ((NDIS_HANDLE)(Ptr))


//
// One of these structures is created on each MacOpenAdapter.
//
typedef struct _LANCE_OPEN {

    //
    // Linking structure for all of the open bindings of a particular
    // adapter. This MUST be the first item in the structure.
    //
    LIST_ENTRY OpenList;

    //
    // The Adapter that requested this open binding.
    //
    PLANCE_ADAPTER OwningLance;

    //
    // Handle of this adapter in the filter database.
    //
    NDIS_HANDLE NdisFilterHandle;

    //
    // Given by NDIS when the adapter was opened.
    //
    NDIS_HANDLE NdisBindingContext;

    //
    // Minimum Number of bytes for a lookahead.
    //
    UINT LookAhead;

    //
    // Counter of all the different reasons that a open binding
    // couldn't be closed.  This would be incremented each time
    // for:
    //
    // While a particular interface routine is accessing this open
    //
    // During an indication.
    //
    // When the open causes a reset.
    //
    // A packet currently being sent.
    //
    // (Basically the above two mean any time the open has left
    //  some processing around to be accomplished later.)
    //
    // This field should only be accessed when the adapter lock is held.
    //
    UINT References;

    //
    // A flag indicating that this binding is in the process of closing.
    //
    BOOLEAN BindingShuttingDown;

    LANCE_PEND_DATA CloseMulticastRequest;

    LANCE_PEND_DATA CloseFilterRequest;

} LANCE_OPEN,*PLANCE_OPEN;

//
// This macro returns a pointer to a PLANCE_OPEN given a MacBindingHandle.
//
#define PLANCE_OPEN_FROM_BINDING_HANDLE(Handle) \
    ((PLANCE_OPEN)(Handle))

//
// This macro returns a NDIS_HANDLE from a PLANCE_OPEN
//
#define BINDING_HANDLE_FROM_PLANCE_OPEN(Open) \
    ((NDIS_HANDLE)(Open))


//
// This macro will act a "epilogue" to every routine in the
// *interface*.  It will check whether any requests need
// to defer their processing.  It will also decrement the reference
// count on the adapter.  If the reference count is zero and there
// is deferred work to do it will insert the interrupt processing
// routine in the DPC queue.
//
// Note that we don't need to include checking for blocked receives
// since blocked receives imply that there will eventually be an
// interrupt.
//
// NOTE: This macro assumes that it is called with the lock acquired.
//
// ZZZ This routine is NT specific.
//
#define LANCE_DO_DEFERRED(Adapter) \
{ \
    PLANCE_ADAPTER _A = (Adapter); \
    _A->References--; \
    if ((!_A->References) && \
        (_A->ResetInProgress || \
         _A->FirstLoopBack || \
         (!IsListEmpty(&_A->CloseList)))) { \
        NdisReleaseSpinLock(&_A->Lock); \
        NdisSetTimer(&_A->DeferredTimer, 0);\
    } else { \
        NdisReleaseSpinLock(&_A->Lock); \
    } \
}




//
// Procedures which log errors.
//

typedef enum _LANCE_PROC_ID {
    openAdapter,
    processInterrupt
} LANCE_PROC_ID;


#define LANCE_ERRMSG_NDIS_ALLOC_MEM      (ULONG)0x01














//
// This macro is used to "allocate" memory for the structures that
// must be shared with the hardware.  It assigns a pvoid that is
// at least quadword aligned.
//
#define LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(A,S,P) \
{ \
    PLANCE_ADAPTER _Adapter = (A); \
    UINT _Size = (((S) + 7)/8)*8; \
    PVOID _HighWater; \
    if (!_Size) { \
        *(P) = NULL; \
    } else { \
        _HighWater = ((PCHAR)_Adapter->CurrentMemoryFirstFree) + _Size; \
        if (((PUCHAR)_HighWater) > \
            (((PUCHAR)_Adapter->MemoryFirstUnavailable) + 1)) { \
            *(P) = NULL; \
        } else { \
            *(P) = _Adapter->CurrentMemoryFirstFree; \
            _Adapter->CurrentMemoryFirstFree = _HighWater; \
        } \
    } \
}

//
// We now convert the virtual address to the actual physical address.
//
#ifdef NDIS_NT

#define LANCE_GET_HARDWARE_PHYSICAL_ADDRESS(A,P) \
      (RtlConvertUlongToLargeInteger((ULONG) \
         ((ULONG)((PVOID)P) - (ULONG)((PVOID)A->MmMappedBaseAddr) + (A->HardwareBaseOffset))))

#else

#define LANCE_GET_HARDWARE_PHYSICAL_ADDRESS(A,P) \
      ((ULONG)((PUCHAR)(A->HardwareBaseAddr) +\
                 ((ULONG)((PVOID)P) - (ULONG)((PVOID)A->MmMappedBaseAddr))))

#endif //NDIS_NT


//
// This macro is used to "deallocate the memory from the hardware.
// Since this is hardware memory that is only allocated and deallocated
// once this macro really doesn't do anything.
//
#define LANCE_DEALLOCATE_MEMORY_FOR_HARDWARE(A,P)\
{\
}





//
// These are routines for synchronizing with the ISR
//


//
// This structure is used to synchronize reading and writing to ports
// with the ISR.
//

typedef struct _LANCE_SYNCH_CONTEXT {

    //
    // Pointer to the lance adapter for which interrupts are
    // being synchronized.
    //
    PLANCE_ADAPTER Adapter;

    //
    // Pointer to a local variable that will receive the value
    //
    PUSHORT LocalRead;

    //
    // Value to write
    //
    USHORT LocalWrite;

} LANCE_SYNCH_CONTEXT,*PLANCE_SYNCH_CONTEXT;



#define LANCE_WRITE_RAP(A,C)      { \
    PLANCE_ADAPTER _A = A; \
    LANCE_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.LocalWrite = (C); \
    NdisSynchronizeWithInterrupt( \
        &(_A)->Interrupt, \
        LanceSyncWriteRAP, \
        &_C \
        ); \
}


#define LANCE_WRITE_RDP(A,C) { \
    PLANCE_ADAPTER _A = A; \
    LANCE_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.LocalWrite = (C); \
    NdisSynchronizeWithInterrupt( \
        &(_A)->Interrupt, \
        LanceSyncWriteRDP, \
        &_C \
        ); \
}

#define LANCE_READ_RDP(A,C) { \
    PLANCE_ADAPTER _A = A; \
    LANCE_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.LocalRead = (C); \
    NdisSynchronizeWithInterrupt( \
        &(_A)->Interrupt, \
        LanceSyncReadRDP, \
        &_C \
        ); \
}

#define LANCE_WRITE_NICSR(A,C) { \
    PLANCE_ADAPTER _A = A; \
    LANCE_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.LocalWrite = (C | _A->NicsrDefaultValue); \
    NdisSynchronizeWithInterrupt( \
        &(_A)->Interrupt, \
        LanceSyncWriteNicsr, \
        &_C \
        ); \
}



BOOLEAN
LanceSyncWriteRAP(
    IN PVOID Context
    );

BOOLEAN
LanceSyncWriteRDP(
    IN PVOID Context
    );

BOOLEAN
LanceSyncReadRDP(
    IN PVOID Context
    );

BOOLEAN
LanceSyncWriteNicsr(
    IN PVOID Context
    );

BOOLEAN
LanceSyncGetInterruptsStopped(
    IN PVOID Context
    );

BOOLEAN
LanceSyncStopChip(
    IN PVOID Context
    );


//
// We define the external interfaces to the lance driver.
// These routines are only external to permit separate
// compilation.  Given a truely fast compiler they could
// all reside in a single file and be static.
//

extern
NDIS_STATUS
LanceTransferData(
    IN NDIS_HANDLE MacBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );

extern
NDIS_STATUS
LanceSend(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );


extern
VOID
LanceStagedAllocation(
    IN PLANCE_ADAPTER Adapter
    );

extern
VOID
LanceCopyFromPacketToBuffer(
    IN PNDIS_PACKET Packet,
    IN UINT Offset,
    IN UINT BytesToCopy,
    OUT PCHAR Buffer,
    OUT PUINT BytesCopied
    );

extern
VOID
LanceCopyFromPacketToPacket(
    IN PNDIS_PACKET Destination,
    IN UINT DestinationOffset,
    IN UINT BytesToCopy,
    IN PNDIS_PACKET Source,
    IN UINT SourceOffset,
    OUT PUINT BytesCopied
    );

extern
VOID
LanceProcessLoopback(
    IN PLANCE_ADAPTER Adapter
    );

extern
BOOLEAN
LanceHardwareDetails(
    IN PLANCE_ADAPTER Adapter
    );

extern
NDIS_STATUS
LanceRegisterAdapter(
    IN PLANCE_ADAPTER Adapter
    );

NDIS_STATUS
LanceAddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdaptName
    );

VOID
LanceRemoveAdapter(
    IN PVOID MacAdapterContext
    );

VOID
SetupAllocate(
    IN PLANCE_ADAPTER Adapter,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );

VOID
LanceWakeUpDpc(
    IN PVOID SystemSpecific1,
    IN PVOID Context,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

#endif // _LANCESFT_

