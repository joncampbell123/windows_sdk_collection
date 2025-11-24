/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    ne3200sw.h

Abstract:

    Software specific values for the Novell NE3200 NDIS 3.0 driver.

Environment:

    This driver is expected to work in DOS, OS2 and NT at the equivalent
    of kernal mode.

    Architecturally, there is an assumption in this driver that we are
    on a little endian machine.

Notes:

    optional-notes

--*/

#ifndef _NE3200SOFTWARE_
#define _NE3200SOFTWARE_

#include <ndis.h>
#include <efilter.h>
#include <ne3200hw.h>

//
// Debugging flags.
//

#if DBG

#define IF_NE3200DBG(flag) if (NE3200Debug & (NE3200_DEBUG_ ## flag))

#define IF_LOG(ch) { Ne3200Log[Ne3200LogPlace++] = (ch); Ne3200Log[Ne3200LogPlace+2] = (UCHAR)'\0'; }
extern UCHAR Ne3200Log[];
extern UCHAR Ne3200LogSave[10][256];
extern UCHAR Ne3200LogSavePlace;
extern UCHAR Ne3200LogPlace;

extern ULONG NE3200Debug;

#define NE3200_DEBUG_DUMP_LOOKAHEAD     0x00000001  // dump lookahead buffer
#define NE3200_DEBUG_DUMP_TRANSFER      0x00000002  // dump transfer buffer
#define NE3200_DEBUG_DUMP_SEND          0x00000004  // dump send packet
#define NE3200_DEBUG_DUMP_COMMAND       0x00000008  // dump command block & buffer

#define NE3200_DEBUG_ACQUIRE            0x00000010  // NE3200AcquireCommandBlock activity
#define NE3200_DEBUG_SUBMIT             0x00000020  // NE3200SubmitCommandBlock activity
#define NE3200_DEBUG_ASSIGN             0x00000040  // NE3200AssignPacketToCommandBlock activity
#define NE3200_DEBUG_RECEIVE            0x00000080  // ProcessReceiveInterrupts activity

#define NE3200_DEBUG_LOUD               0x00000100  // print things
#define NE3200_DEBUG_VERY_LOUD          0x00000200  // print lots of things

#define DPrint1(fmt) DbgPrint(fmt)
#define DPrint2(fmt,v1) DbgPrint(fmt,v1)
#define DPrint3(fmt,v1,v2) DbgPrint(fmt,v1,v2)
#define DPrint4(fmt,v1,v2,v3) DbgPrint(fmt,v1,v2,v3)
#else // DBG

#define IF_LOG(ch)

#define IF_NE3200DBG(flag) if (0)
#define DPrint1(fmt)
#define DPrint2(fmt,v1)
#define DPrint3(fmt,v1,v2)
#define DPrint4(fmt,v1,v2,v3)

#endif // DBG

#define STATIC

#define NE3200_NDIS_MAJOR_VERSION 3
#define NE3200_NDIS_MINOR_VERSION 0


#define NE3200_ALLOC_PHYS(_Status, _pBuffer, _Length) \
{ \
    NDIS_PHYSICAL_ADDRESS MinusOne = NDIS_PHYSICAL_ADDRESS_CONST (-1, -1); \
    *(_Status) = NdisAllocateMemory( \
                     (PVOID*)(_pBuffer), \
                     (_Length), \
                     0, \
                     MinusOne); \
}

#define NE3200_FREE_PHYS(_Buffer) NdisFreeMemory((_Buffer), 0, 0)

#define NE3200_ALLOC_CONTIG(_Status, _pbuffer, _length) \
{ \
    NDIS_PHYSICAL_ADDRESS MinusOne = NDIS_PHYSICAL_ADDRESS_CONST (-1, -1); \
    *(_Status) = NdisAllocateMemory( \
                     (PVOID*)(_pbuffer), \
                     (_length), \
                     NDIS_MEMORY_CONTIGUOUS, \
                     MinusOne); \
}

#define NE3200_FREE_CONTIG(_Buffer) NdisFreeMemory((_Buffer), 0, NDIS_MEMORY_CONTIGUOUS)

#define NE3200_ALLOC_PHYS_NONCACHED(_pbuffer, _length) \
{ \
    NDIS_PHYSICAL_ADDRESS MinusOne = NDIS_PHYSICAL_ADDRESS_CONST (-1, -1); \
    *(_Status) = NdisAllocateMemory( \
                     (PVOID*)(_pbuffer), \
                     (_length), \
                     NDIS_MEMORY_NONCACHED, \
                     MinusOne); \
}

#define NE3200_FREE_PHYS_NONCACHED(_buffer, _length) \
                NdisFreeMemory((PVOID)(_buffer), _length, NDIS_MEMORY_NONCACHED)

#define NE3200_MOVE_MEMORY(Destination,Source,Length) NdisMoveMemory(Destination,Source,Length)

//
// Size of ethernet header
//

#define NE3200_HEADER_SIZE 14

//
// Size of lookahead buffer for loopback packets
//
#define NE3200_SIZE_OF_LOOPBACK 256


//
// The implementation of RESET (for both MacReset and MacInitialize)
// is peculiar to NT.
//
// The NE3200 must be "held by the hand" during the reset & download
// operations.  Typically, the reset (or download) is initiated and
// the status ports are POLLED, waiting for pass/fail status.  This
// is unacceptable in NT.
//
// To handle this cleanly in NT, the reset & download operations will
// be controlled by a state machine.  This state machine will be
// contained in a Kernel DPC routine driven by a Kernel Timer Object.
//
// This ENUM represents the current state of the reset operation.
//

typedef enum _NE3200_RESET_STATE {

    NE3200ResetStateStarting,
    NE3200ResetStateResetting,
    NE3200ResetStateDownloading,
    NE3200ResetStateReloadPacketFilters,
    NE3200ResetStateReloadMulticast,
    NE3200ResetStateReloadAddress,
    NE3200ResetStateComplete

} NE3200_RESET_STATE, *PNE3200_RESET_STATE;

//
// This ENUM represents the result of the reset operation.
//

typedef enum _NE3200_RESET_RESULT {

    NE3200ResetResultSuccessful,
    NE3200ResetResultResetFailure,
    NE3200ResetResultResetTimeout,
    NE3200ResetResultInitializationFailure,
    NE3200ResetResultInitializationTimeout,
    NE3200ResetResultInvalidState,
    NE3200ResetResultResources

} NE3200_RESET_RESULT, *PNE3200_RESET_RESULT;


struct _NE3200_ADAPTER;

//
// This structure defines the global data needed by the driver.
//
typedef struct _NE3200_GLOBAL_DATA {

    //
    // Spinlock to protect fields in this structure.
    //
    NDIS_SPIN_LOCK Lock;

    //
    // We need to allocate a buffer to contain the MAC.BIN code to be
    // downloaded to the NE3200 adapter(s).  This field will contain
    // the virtual address of this buffer.
    //
    PVOID MacBinVirtualAddress;
    NDIS_PHYSICAL_ADDRESS MacBinPhysicalAddress;

    //
    // The handle of the adapter used for the allocaton of
    // the MAC.BIN buffer (the first one added for this MAC).
    //
    NDIS_HANDLE MacBinAdapterHandle;

    //
    // This field contains the actual length (in bytes) of MAC.BIN.
    //
    USHORT MacBinLength;

    //
    // Chain of Adapters
    //
    LIST_ENTRY AdapterList;

    //
    // Handle to our Mac
    //
    NDIS_HANDLE NE3200MacHandle;

    //
    // Handle for NdisTerminateWrapper
    //
    NDIS_HANDLE NE3200NdisWrapperHandle;

    //
    // Driver object
    //
    PDRIVER_OBJECT NE3200DriverObject;

} NE3200_GLOBAL_DATA, *PNE3200_GLOBAL_DATA;

//
// In addition to the Command Block fields which the NE3200
// defines, we need some additional fields for our own purposes.
// To ensure that these fields are properly aligned (and to
// ensure that the actual Command Block is properly aligned)
// we'll defined a Super Command Block.  This structure will
// contain a "normal" NE3200 Command Block plus some additional
// fields.
//
typedef struct _NE3200_SUPER_COMMAND_BLOCK {

    //
    // The actual NE3200 Command Block.
    //
    NE3200_COMMAND_BLOCK Hardware;

    //
    // This contains the physical address of the above Command Block.
    //
    NDIS_PHYSICAL_ADDRESS Self;

    //
    // This contains the virtual address of the next pending command.
    //
    struct _NE3200_SUPER_COMMAND_BLOCK *NextCommand;

    //
    // Points to the packet from which data is being transmitted
    // through this Command Block.
    //
    PNDIS_PACKET OwningPacket;

    //
    // When a packet is submitted to the hardware we record
    // here whether it used adapter buffers and if so, the buffer
    // index.
    //
    UINT NE3200BuffersIndex;
    BOOLEAN UsedNE3200Buffer;

    //
    // Is this from a set
    //
    BOOLEAN Set;

    //
    // If this is a public (adapter-wide) command block, then
    // this will contain this block's index into the adapter's
    // command queue.
    //
    USHORT CommandBlockIndex;

    //
    // This points to the owning open binding if this is a private
    // Command Block.  Otherwise (this is a public Command Block)
    // this field is NULL.
    //
    struct _NE3200_OPEN *OwningOpenBinding;

    //
    // This field is used to timestamp the command blocks
    // as they are placed into the command queue.  If a
    // block fails to execute, the adapter will get a kick in the ass to
    // start it up again.
    //
    BOOLEAN Timeout;

    //
    // Count of the number of times we have retried a command.
    //
    UCHAR TimeoutCount;

} NE3200_SUPER_COMMAND_BLOCK, *PNE3200_SUPER_COMMAND_BLOCK;

//
// In addition to the Receive Entry fields which the NE3200
// defines, we need some additional fields for our own purposes.
// To ensure that these fields are properly aligned (and to
// ensure that the actual Receive Entry is properly aligned)
// we'll defined a Super Receive Entry.  This structure will
// contain a "normal" NE3200 Receive Entry plus some additional
// fields.
//
typedef struct _NE3200_SUPER_RECEIVE_ENTRY {

    //
    // The actual NE3200 Receive Entry.
    //
    NE3200_RECEIVE_ENTRY Hardware;

    //
    // This contains the physical address of the above Receive Entry.
    //
    NDIS_PHYSICAL_ADDRESS Self;

    //
    // This contains the virtual address of this Receive Entry's
    // frame buffer.
    //
    PVOID ReceiveBuffer;
    NDIS_PHYSICAL_ADDRESS ReceiveBufferPhysical;

    //
    // This contains the virtual address of the next
    // Receive Entry in the Receive Queue.
    //
    struct _NE3200_SUPER_RECEIVE_ENTRY *NextEntry;

    //
    // Points to an Mdl which points to this buffer
    //
    PNDIS_BUFFER FlushBuffer;

} NE3200_SUPER_RECEIVE_ENTRY, *PNE3200_SUPER_RECEIVE_ENTRY;



//
// This record type is inserted into the MacReserved portion
// of the ndis request
//
typedef struct _NE3200_REQUEST_RESERVED {
    PNDIS_REQUEST Next;
    struct _NE3200_OPEN * OpenBlock;
} NE3200_REQUEST_RESERVED, *PNE3200_REQUEST_RESERVED;

//
// This macro will return a pointer to the NE3200 reserved portion
// of an ndis request
//
#define PNE3200_RESERVED_FROM_REQUEST(Request) \
    ((PNE3200_REQUEST_RESERVED)((Request)->MacReserved))

//
// This record type is inserted into the MacReserved portion
// of the packet header when the packet is going through the
// staged allocation of buffer space prior to the actual send.
//
typedef struct _NE3200_RESERVED {

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
    // The particular request handle for the send of this packet.
    //
    NDIS_HANDLE RequestHandle;

    //
    // The following union elements are adjusted at each stage
    // of the allocation.  Each union element should only be accessed
    // during it's own stage.
    //

    union _STAGE {
        UINT ClearStage;
        struct _STAGE1 {

            //
            // A value of zero indicates that the packet needs
            // no adjustment.
            //
            // A value of 1 means it only requires a small packet.
            //
            // A value of 2 means it only requires a medium packet.
            //
            // A value of 3 means it must use a large packet.
            //
            UINT MinimumBufferRequirements:2;

            //
            // The number of ndis buffers to copy into the buffer.
            //
            UINT NdisBuffersToMove:14;

        } STAGE1;
        struct _STAGE2 {

            //
            // If TRUE then the packet caused an adapter buffer to
            // be allocated.
            //
            UINT UsedNE3200Buffer:1;

            //
            // If the previous field was TRUE then this gives the
            // index into the array of adapter buffer descriptors that
            // contains the old packet information.
            //
            UINT NE3200BuffersIndex:15;

            //
            // Gives the index of the Command Block as well as the
            // command block to packet structure.
            //
            UINT CommandBlockIndex:16;

        } STAGE2;

        //
        // When the packet is submitted to the hardware and/or
        // placed on the loopback queue these two fields of the
        // union are used.
        //
        // It is always desired to keep the packet linked on
        // one list.
        //
        // Here's how the fields are used.
        //
        // If the packet is just going on the hardware transmit
        // or it is just going on the loopback then the ReadyToComplete
        // flag will be set TRUE immediately.  If it is just going on the
        // loopback it also sets the status field in stage4 to successful.
        //
        // In the above situations, if the packet just went on the
        // loopback queue, when the packet was finished with loopback
        // the code would see that it was ready to complete.  It would
        // also know that it is in loopback processing.  Since the packet
        // can only be on one queue at a time it could simply remove
        // the packet from the loopback queue and indicate the send
        // as complete.
        //
        // If the packet not going on the loopback queue it would
        // be placed on an adapter wide queue.  It would use as a
        // forward pointer the Next field.  As a backward pointer it
        // would overlay the stage 4 field with the backward pointer.
        // Note that this is safe since no PNDIS_PACKET is ever odd
        // byte aligned, and therefore the low bit would always be clear.
        //
        // We put the packet on a doubly linked list since we could
        // never be quite sure of the order that we would remove packets
        // from this list.  (This will be clear shortly.)
        //
        // If the packet needs to be transmitted as well as loopbacked
        // then the following occurs.
        //
        // The packets buffers are relinquished to the hardware.  At the
        // same time the packet is placed on the loopback queue.  The
        // stage4 field ReadyToComplete is set to false.
        //
        // If the packet finishes transmission and the ReadyToComplete
        // flag is false that means it still hasn't finished loopback
        // and therefore is still on the loopback list.  The code
        // simply sets ReadyToComplete to true and the status of the
        // operation to true or false (depending on the result.)
        // When that packet does finish loopback it notes that the
        // ready to complete is true.  It recovers that status from stage
        // 4.  It can then remove the packet from the loopback list and
        // signal completion for that packet.
        //
        // If the packet finishes transmission and ReadyToComplete is true
        // it simply removes it from the doubly linked adapter wide queue
        // and signals its completion with the status that has been
        // determined in the trasmission complete code.
        //
        // If the loopback code finishes processing the packet and it finds
        // the ReadyToComplete TRUE it simply removes it from the loopback
        // list and signals with the saved status in STAGE4.
        //
        // If the loopback code finishes processing the packet and it finds
        // the ReadyToComplete FALSE it simply puts the packet on the adapter
        // wide doubly linked list with ReadyToComplete set to TRUE.
        //
        // The main reason this is a doubly linked list is that there is no
        // real way to predict when a packet will finish loopback and no
        // real way to predict whether a packet even will be loopbacked.
        // With this lack of knowledge, and the fact that the above packets
        // may end up on the same list, the packet at the front of that
        // list may not be the first packet to complete first.  With
        // a doubly linked list it is much easier to pull a packet out of
        // the middle of that list.
        //

        struct _STAGE4 {

            //
            // Under the protection of the transmit queue lock
            // this value will be examined by both the loopback
            // completion code and the hardware send completion
            // code.  If either of them find the value to be true
            // they will send the transmit complete.
            //
            // Note that if the packet didn't have to be loopbacked
            // or if the packet didn't need to go out on the wire
            // the this value will be initialized to true.  Otherwise
            // this value will be set to false just before it is
            // relinquished to the hardware and to the loopback queue.
            //
            UINT ReadyToComplete:1;

            //
            // When the hardware send is done this will record whether
            // the send was successful or not.  It is only used if
            // ReadyToComplete is FALSE.
            //
            // By definition loopback can never fail.
            //
            UINT SuccessfulTransmit:1;

        } STAGE4;

        //
        // Used as a back pointer in a doubly linked list if the
        // packet needs to go on an adapter wide queue to finish
        // processing.
        //
        PNDIS_PACKET BackPointer;

    } STAGE;

} NE3200_RESERVED,*PNE3200_RESERVED;

//
// This macro will return a pointer to the NE3200 reserved portion
// of a packet given a pointer to a packet.
//
#define PNE3200_RESERVED_FROM_PACKET(Packet) \
    ((PNE3200_RESERVED)((Packet)->MacReserved))

//
// If an ndis packet does not meet the hardware contraints then
// an adapter buffer will be allocated.  Enough data will be copied
// out of the ndis packet so that by using a combination of the
// adapter buffer and remaining ndis buffers the hardware
// constraints are satisfied.
//
// In the NE3200_ADAPTER structure three threaded lists are kept in
// one array.  One points to a list of NE3200_BUFFER_DESCRIPTORS
// that point to small adapter buffers.  Another is for medium sized
// buffers and the last for full sized (large) buffers.
//
// The allocation is controlled via a free list head and
// the free lists are "threaded" by a field in the adapter buffer
// descriptor.
//
typedef struct _NE3200_BUFFER_DESCRIPTOR {

    //
    // A physical pointer to a small, medium, or large buffer.
    //
    NDIS_PHYSICAL_ADDRESS PhysicalNE3200Buffer;

    //
    // A virtual pointer to a small, medium, or large buffer.
    //
    PVOID VirtualNE3200Buffer;

    //
    // Flush buffer
    //
    PNDIS_BUFFER FlushBuffer;

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

} NE3200_BUFFER_DESCRIPTOR,*PNE3200_BUFFER_DESCRIPTOR;

//
// ZZZ
//

typedef struct _NE3200_ADAPTER {

    //
    // Used for filter and statistics operations
    //

    PNE3200_SUPER_COMMAND_BLOCK PublicCommandBlock;
    NDIS_PHYSICAL_ADDRESS PublicCommandBlockPhysical;
    //
    // Used for padding short packets
    //

    PUCHAR PaddingVirtualAddress;
    NDIS_PHYSICAL_ADDRESS PaddingPhysicalAddress;

    //
    // Current card filter
    //
    UINT CurrentPacketFilter;

    //
    // Request queue pointers
    //
    PNDIS_REQUEST FirstRequest;
    PNDIS_REQUEST LastRequest;

    //
    // Points to the card multicast entry table
    //
    PUCHAR CardMulticastTable;
    NDIS_PHYSICAL_ADDRESS CardMulticastTablePhysical;

    //
    // Holds the interrupt object for this adapter.
    //
    NDIS_INTERRUPT Interrupt;

    //
    // Spinlock for the interrupt.
    //
    NDIS_SPIN_LOCK InterruptLock;

    //
    // Normal processing DPC.
    //
    PVOID InterruptDpc;

    //
    // The EISA Slot Number for this adapter.
    //
    USHORT EisaSlot;

    //
    // Various I/O Port Addresses for this adapter.
    //
    PUCHAR ResetPort;
    PUCHAR SystemInterruptPort;
    PUCHAR LocalDoorbellInterruptPort;
    PUCHAR SystemDoorbellMaskPort;
    PUCHAR SystemDoorbellInterruptPort;
    PUCHAR BaseMailboxPort;

    //
    // TRUE if a remove adapter has been called for this adapter
    //
    BOOLEAN BeingRemoved;

    //
    // TRUE when a receive interrupt is received
    //
    BOOLEAN ReceiveInterrupt;

    //
    // Count of WakeUpDpc's that have fired without a receive interrupt.
    //
    UCHAR NoReceiveInterruptCount;

    //
    // TRUE when a send interrupt is received
    //
    BOOLEAN SendInterrupt;

    // This boolean is used as a gate to ensure that only one thread
    // of execution is actually processing interrupts or some other
    // source of deferred processing.
    //
    BOOLEAN DoingProcessing;

    //
    // Current processing requests
    //
    BOOLEAN ProcessingRequests;

    //
    // Are we doing deferred operations
    //
    BOOLEAN ProcessingDeferredOperations;

    //
    // Is True right after a reset but becomes false after the first open
    //
    BOOLEAN FirstOpen;

    //
    // Should we use an alternative address
    //
    BOOLEAN AddressChanged;

    //
    // The network address from the hardware.
    //
    CHAR NetworkAddress[ETH_LENGTH_OF_ADDRESS];
    CHAR CurrentAddress[ETH_LENGTH_OF_ADDRESS];

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
    // Pointer to the filter database for the MAC.
    //
    PETH_FILTER FilterDB;

    //
    // Pointer to the Receive Queue.
    //
    PNE3200_SUPER_RECEIVE_ENTRY ReceiveQueue;
    NDIS_PHYSICAL_ADDRESS ReceiveQueuePhysical;

    //
    // Pointer to the Command Queue.
    //
    PNE3200_SUPER_COMMAND_BLOCK CommandQueue;
    NDIS_PHYSICAL_ADDRESS CommandQueuePhysical;

    //
    // Total number of Command Blocks in the Command Queue.
    //
    UINT NumberOfCommandBlocks;

    //
    // Total number of Receive Buffers.
    //
    UINT NumberOfReceiveBuffers;

    //
    // Total number of Transmit Buffers.
    //
    UINT NumberOfTransmitBuffers;

    //
    // Number of available Command Blocks in the Command Queue.
    //
    UINT NumberOfAvailableCommandBlocks;

    //
    // Pointer to the next available Command Block.
    //
    PNE3200_SUPER_COMMAND_BLOCK NextCommandBlock;

    //
    // Pointer to the next command to complete execution.
    //
    PNE3200_SUPER_COMMAND_BLOCK FirstCommandOnCard;

    //
    // Pointer to the most recently submitted command.
    //
    PNE3200_SUPER_COMMAND_BLOCK LastCommandOnCard;

    //
    // Pointer to the first command waiting to be put on the list to the card.
    //
    PNE3200_SUPER_COMMAND_BLOCK FirstWaitingCommand;

    //
    // Pointer to the last command waiting to be put on the list to the card.
    //
    PNE3200_SUPER_COMMAND_BLOCK LastWaitingCommand;

    //
    // Pointer to the head of the Receive Queue.
    //
    PNE3200_SUPER_RECEIVE_ENTRY ReceiveQueueHead;

    //
    // Pointer to the tail of the Receive Queue.
    //
    PNE3200_SUPER_RECEIVE_ENTRY ReceiveQueueTail;

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
    // The Flush buffer pool
    //
    PNDIS_HANDLE FlushBufferPoolHandle;

    //
    // List head for the adapters buffers.  If the list
    // head is equal to -1 then there are no free elements
    // on the list.
    //
    // The list head must only be accessed when the
    // adapter lock is held.
    //
    INT NE3200BufferListHead;

    //
    // Pointers to an array of adapter buffer descriptors.
    // The array will actually be threaded together by
    // three free lists.  The lists will be for small,
    // medium and full sized packets.
    //
    PNE3200_BUFFER_DESCRIPTOR NE3200Buffers;

    //
    // These fields let the send allocation code know that it's
    // futile to even try to move a packet along to that stage.
    //
    BOOLEAN Stage2Open;

    //
    // These AlreadyProcessingStageX variables are set up to keep
    // more than one thread from accessing a particular thread
    // a one time.
    //
    BOOLEAN AlreadyProcessingStage2;

    //
    // Flag that when enabled lets routines know that a reset
    // is in progress.
    //
    BOOLEAN ResetInProgress;

    //
    // Is the reset to be done asynchronously?
    //
    BOOLEAN ResetAsynchronous;

    //
    // Used to store the command block for asynchronous resetting.
    //
    PNE3200_SUPER_COMMAND_BLOCK ResetHandlerCommandBlock;

    //
    // Pointers to the first and last packets at a particular stage
    // of allocation.  All packets in transmit are linked
    // via there next field.
    //
    PNDIS_PACKET FirstStage1Packet;
    PNDIS_PACKET LastStage1Packet;

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

    //
    // Packet counts
    //

    UINT GoodTransmits;
    UINT GoodReceives;
    UINT TransmitsQueued;

    //
    // Count of transmit errors
    //

    UINT RetryFailure;
    UINT LostCarrier;
    UINT UnderFlow;
    UINT NoClearToSend;
    UINT Deferred;
    UINT OneRetry;
    UINT MoreThanOneRetry;

    //
    // Count of receive errors
    //

    UINT CrcErrors;
    UINT AlignmentErrors;
    UINT OutOfResources;
    UINT DmaOverruns;


    //
    // Pointer to the binding that initiated the reset.  This
    // will be null if the reset is initiated by the MAC itself.
    //
    struct _NE3200_OPEN *ResettingOpen;

    //
    // This holds the current state of the reset operation.
    //
    NE3200_RESET_STATE ResetState;

    //
    // This hold the result of the reset operation.
    //
    NE3200_RESET_RESULT ResetResult;

    //
    // This is a timeout counter.  Before a timed operation is
    // started, a positive value is placed in this field.  Every
    // time the particular state is entered in the ResetDpc, this
    // value is decremented.  If this value becomes zero, then
    // the operation has timed-out and the adapter is toast.
    //
    UINT ResetTimeoutCounter;

    //
    // This timer object will be used to drive the wakeup call
    //
    NDIS_TIMER WakeUpTimer;

    //
    // This timer object will be used to queue the deferred processing routine
    //
    NDIS_TIMER DeferredTimer;

    //
    // This timer is for handling resets from when the card is dead.
    //
    NDIS_TIMER ResetTimer;

    //
    // Place for holding command block for pending commands during
    // reset processing.
    //
    PNE3200_SUPER_COMMAND_BLOCK ResetCommandBlock;

    //
    // This is a pointer to the Configuration Block for this
    // adapter.  The Configuration Block will be modified during
    // changes to the packet filter.
    //
    PNE3200_CONFIGURATION_BLOCK ConfigurationBlock;
    NDIS_PHYSICAL_ADDRESS ConfigurationBlockPhysical;

    //
    // This points to the next adapter registered for the same Mac
    //
    LIST_ENTRY AdapterList;

    //
    // Number of opens
    //
    UINT OpenCount;

    //
    // Lookahead buffer for loopback packets
    //
    UCHAR Loopback[NE3200_SIZE_OF_LOOPBACK];

} NE3200_ADAPTER,*PNE3200_ADAPTER;

//
// Given a MacBindingHandle this macro returns a pointer to the
// NE3200_ADAPTER.
//
#define PNE3200_ADAPTER_FROM_BINDING_HANDLE(Handle) \
    (((PNE3200_OPEN)(Handle))->OwningAdapter)

//
// Given a MacContextHandle return the PNE3200_ADAPTER
// it represents.
//
#define PNE3200_ADAPTER_FROM_CONTEXT_HANDLE(Handle) \
    ((PNE3200_ADAPTER)(Handle))

//
// Given a pointer to a NE3200_ADAPTER return the
// proper MacContextHandle.
//
#define CONTEXT_HANDLE_FROM_PNE3200_ADAPTER(Ptr) \
    ((NDIS_HANDLE)(Ptr))

//
// One of these structures is created on each MacOpenAdapter.
//
typedef struct _NE3200_OPEN {

    //
    // Linking structure for all of the open bindings of a particular
    // adapter.
    //
    LIST_ENTRY OpenList;

    //
    // The Adapter that requested this open binding.
    //
    PNE3200_ADAPTER OwningAdapter;

    //
    // Handle of this adapter in the filter database.
    //
    NDIS_HANDLE NdisFilterHandle;

    //
    // Given by NDIS when the adapter was opened.
    //
    NDIS_HANDLE NdisBindingContext;

    //
    // Counter of all the different reasons that a open binding
    // couldn't be closed.
    UINT References;

    //
    // A flag indicating that this binding is in the process of closing.
    //
    BOOLEAN BindingShuttingDown;

    //
    // A flag indicating whether this has a deferred close
    //
    BOOLEAN ClosePending;

    //
    // TRUE if we can pass a receive indication to the protocol while
    // another recieve is in progress.
    //
    BOOLEAN ReceiveIsReentrant;

    //
    // command block for this open
    //
    PNE3200_SUPER_COMMAND_BLOCK PrivateCommandBlock;

    //
    // Used for opening and closing
    //
    NDIS_REQUEST OpenCloseRequest;

    UINT ProtOptionFlags;

} NE3200_OPEN,*PNE3200_OPEN;

//
// This macro returns a pointer to a PNE3200_OPEN given a MacBindingHandle.
//
#define PNE3200_OPEN_FROM_BINDING_HANDLE(Handle) \
    ((PNE3200_OPEN)(Handle))

//
// This macro returns a NDIS_HANDLE from a PNE3200_OPEN
//
#define BINDING_HANDLE_FROM_PNE3200_OPEN(Open) \
    ((NDIS_HANDLE)(Open))


//
// This is a fake value that no open can have for doing internal
// changes of packet filters.
//
#define NE3200_FAKE_OPEN (PNE3200_OPEN)0x01

//
// This macro will act a "epilogue" to every routine in the
// *interface*.  It will check whether there any requests needed
// to defer there processing.  It will also decrement the reference
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
#if 0
#define NE3200_DO_DEFERRED(Adapter) \
{ \
    PNE3200_ADAPTER _A = (Adapter); \
    if ((!_A->ProcessingDeferredOperations) && \
        (((_A->References == 2) && \
        _A->ResetInProgress) || \
         _A->FirstLoopBack || \
         (!IsListEmpty(&_A->CloseList)))) { \
        _A->ProcessingDeferredOperations = TRUE; \
        NdisReleaseSpinLock(&_A->Lock); \
        NdisSetTimer(&_A->DeferredTimer, 0); \
    } else { \
        _A->References--; \
        NdisReleaseSpinLock(&_A->Lock); \
    } \
}
#else
#define NE3200_DO_DEFERRED(Adapter) \
{ \
    PNE3200_ADAPTER _A = (Adapter); \
    _A->References--; \
    if ((_A->References == 1) && \
        (_A->ResetInProgress || \
         _A->FirstLoopBack || \
         (!IsListEmpty(&_A->CloseList)))) { \
        NdisReleaseSpinLock(&_A->Lock); \
        NdisSetTimer(&_A->DeferredTimer,0); \
    } else { \
        NdisReleaseSpinLock(&_A->Lock); \
    } \
}
#endif

//
// Procedures which do error logging
//

typedef enum _NE3200_PROC_ID{
    allocateAdapterMemory,
    initialInit,
    setConfigurationBlockAndInit,
    registerAdapter,
    openAdapter,
    wakeUpDpc,
    resetDpc
}NE3200_PROC_ID;

//
// Error log codes.
//

#define NE3200_ERRMSG_ALLOC_MEM         (ULONG)0x01
#define NE3200_ERRMSG_INIT_INTERRUPT    (ULONG)0x02
#define NE3200_ERRMSG_NO_DELAY          (ULONG)0x03
#define NE3200_ERRMSG_INIT_DB           (ULONG)0x04
#define NE3200_ERRMSG_OPEN_DB           (ULONG)0x05
#define NE3200_ERRMSG_BAD_STATE         (ULONG)0x06
#define NE3200_ERRMSG_                  (ULONG)0x06


//
// Define our block of global data.  The actual data resides in NE3200.C.
//
extern NE3200_GLOBAL_DATA NE3200Globals;

#include <ne3200pr.h>

#endif // _NE3200SOFTWARE_
