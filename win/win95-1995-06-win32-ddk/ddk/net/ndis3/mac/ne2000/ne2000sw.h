/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    ne2000sw.h

Abstract:

    Software definitions for a Novell 2000 MAC driver.

Environment:

    This driver is expected to work in DOS, OS2 and NT at the equivalent
    of kernal mode.

    Architecturally, there is an assumption in this driver that we are
    on a little endian machine.

Notes:

    optional-notes

--*/

#ifndef _NE2000SFT_
#define _NE2000SFT_

#define NE2000_NDIS_MAJOR_VERSION 3
#define NE2000_NDIS_MINOR_VERSION 0

#define NE2000

//
// This macro is used along with the flags to selectively
// turn on debugging.
//

#if DBG

#define IF_NE2000DEBUG(f) if (Ne2000DebugFlag & (f))
extern ULONG Ne2000DebugFlag;

#define NE2000_DEBUG_LOUD               0x00000001  // debugging info
#define NE2000_DEBUG_VERY_LOUD          0x00000002  // excessive debugging info
#define NE2000_DEBUG_LOG                0x00000004  // enable Ne2000Log
#define NE2000_DEBUG_CHECK_DUP_SENDS    0x00000008  // check for duplicate sends
#define NE2000_DEBUG_TRACK_PACKET_LENS  0x00000010  // track directed packet lens
#define NE2000_DEBUG_WORKAROUND1        0x00000020  // drop DFR/DIS packets
#define NE2000_DEBUG_CARD_BAD           0x00000040  // dump data if CARD_BAD
#define NE2000_DEBUG_CARD_TESTS         0x00000080  // print reason for failing

//
// Macro for deciding whether to dump lots of debugging information.
//

#define IF_LOUD(A) IF_NE2000DEBUG( NE2000_DEBUG_LOUD ) { A }
#define IF_VERY_LOUD(A) IF_NE2000DEBUG( NE2000_DEBUG_VERY_LOUD ) { A }

#else

#define IF_LOUD(A)
#define IF_VERY_LOUD(A)

#endif

//
// Whether to use the Ne2000Log
//

#if DBG

#define IF_LOG(A) IF_NE2000DEBUG( NE2000_DEBUG_LOG ) { A }
extern VOID Ne2000Log(UCHAR);

#else

#define IF_LOG(A)

#endif

//
// Whether to do loud init failure
//

#if DBG
#define IF_INIT(A) A
#else
#define IF_INIT(A)
#endif

//
// Whether to do loud card test failures
//

#if DBG
#define IF_TEST(A) IF_NE2000DEBUG( NE2000_DEBUG_CARD_TESTS ) { A }
#else
#define IF_TEST(A)
#endif

//
// Adapter->NumBuffers
//
// controls the number of transmit buffers on the packet.
// Choices are 1 through 12.
//

#define DEFAULT_NUMBUFFERS 12

#define NE2000_MOVE_MEM(dest,src,size) NdisMoveMemory(dest,src,size)

//
// A broadcast address (for comparing with other addresses).
//

extern UCHAR Ne2000BroadcastAddress[];

//
// The status of transmit buffers.
//

typedef enum {  EMPTY = 0x00, 
               FILLING = 0x01, 
               FULL = 0x02 } BUFFER_STATUS;

//
// Type of an interrupt.
//

typedef enum { RECEIVE    = 0x01,
           TRANSMIT   = 0x02,
           OVERFLOW   = 0x04,
           COUNTER    = 0x08,
           UNKNOWN    = 0x10} INTERRUPT_TYPE;

//
// Result of Ne2000Indicate[Loopback]Packet().
//

typedef enum { INDICATE_OK, SKIPPED, ABORT, CARD_BAD } INDICATE_STATUS;

//
// Stages in a reset.
//

typedef enum { NONE, MULTICAST_RESET, XMIT_STOPPED, BUFFERS_EMPTY } RESET_STAGE;

//
// Size of the ethernet header
//

#define NE2000_HEADER_SIZE 14

//
// Number of bytes allowed in a lookahead (max)
//

#define NE2000_MAX_LOOKAHEAD (252 - NE2000_HEADER_SIZE)

//
// Maximum number of transmit buffers on the card.
//

#define MAX_XMIT_BUFS   12

//
// A transmit buffer.
//

typedef UINT XMIT_BUF;

//
// Number of 256-byte buffers in a transmit buffer.
//

#define BUFS_PER_TX 1

//
// Size of a single transmit buffer.
//

#define TX_BUF_SIZE (BUFS_PER_TX*256)

//
// Only have one of these structures.
//

typedef struct _MAC_BLOCK {

    //
    // NDIS wrapper information.
    //

    NDIS_HANDLE NdisMacHandle;          // returned from NdisRegisterMac
    NDIS_HANDLE NdisWrapperHandle;      // returned from NdisInitializeWrapper
    NDIS_MAC_CHARACTERISTICS MacCharacteristics;

    //
    // Adapters registered for this MAC.
    //

    struct _NE2000_ADAPTER * AdapterQueue;
    NDIS_SPIN_LOCK SpinLock;            // guards NumAdapter and AdapterQueue

    PDRIVER_OBJECT DriverObject;

    BOOLEAN Unloading;

} MAC_BLOCK, * PMAC_BLOCK;

//
// Used to contain a queued operation.
//

typedef struct _NE2000_PEND_DATA {
    struct _NE2000_PEND_DATA * Next;
    struct _NE2000_OPEN * Open;
    NDIS_REQUEST_TYPE RequestType;
} NE2000_PEND_DATA, * PNE2000_PEND_DATA;

//
// This macro will return a pointer to the reserved area of
// a PNDIS_REQUEST.
//
#define PNE2000_PEND_DATA_FROM_PNDIS_REQUEST(Request) \
   ((PNE2000_PEND_DATA)((PVOID)((Request)->MacReserved)))

//
// This macros returns the enclosing NdisRequest.
//
#define PNDIS_REQUEST_FROM_PNE2000_PEND_DATA(PendOp)\
   ((PNDIS_REQUEST)((PVOID)(PendOp)))

//
// One of these structures per adapter registered.
//

typedef struct _NE2000_ADAPTER {

    //
    // Spin lock for adapter structure
    //
    NDIS_SPIN_LOCK Lock;

    //
    // NDIS wrapper information.
    //

    NDIS_HANDLE NdisAdapterHandle;      // returned from NdisRegisterAdapter
    NDIS_INTERRUPT NdisInterrupt;    // interrupt info used by wrapper

    //
    // Links with our MAC.
    //

    PMAC_BLOCK MacBlock;
    struct _NE2000_ADAPTER * NextAdapter;    // used by MacBlock->AdapterQueue

    //
    // Opens for this adapter.
    //

    struct _NE2000_OPEN * OpenQueue;

    //
    // Opens for this adapter that are waiting for closes to finish.
    //

    struct _NE2000_OPEN * CloseQueue;

    //
    // Number of references to the adapter.
    //
    ULONG References;

    ULONG ReceivePacketCount;
    
    //
    // Configuration information
    //

    UINT NumBuffers;
    PVOID IoBaseAddr;
    CHAR InterruptNumber;
    UINT MulticastListMax;
    UCHAR BusType;

    //
    // InterruptType tracks interrupt sources that still need to be serviced,
    // it is the logical OR of all card interrupts that have been received and not
    // processed and cleared. (see also INTERRUPT_TYPE definition in ne2000.h)
    //
    UCHAR InterruptType;

    //
    // Transmit queue.
    //

    PNDIS_PACKET XmitQueue;             // packets waiting to be transmitted
    PNDIS_PACKET XmitQTail;

    //
    // Transmit information.
    //

    XMIT_BUF NextBufToFill;             // where to copy next packet to
    XMIT_BUF NextBufToXmit;             // valid if CurBufXmitting is -1
    XMIT_BUF CurBufXmitting;            // -1 if none is
    BOOLEAN TransmitInterruptPending;   // transmitting, but DPC not yet queued
    BOOLEAN OverflowRestartXmitDpc;     // transmitting, but DPC not yet queued
    BUFFER_STATUS BufferStatus[MAX_XMIT_BUFS];
    PNDIS_PACKET Packets[MAX_XMIT_BUFS];  // as passed to MacSend
    UINT PacketLens[MAX_XMIT_BUFS];
    PUCHAR XmitStart;                   // start of card transmit area
    PUCHAR PageStart;                   // start of card receive area
    PUCHAR PageStop;                    // end of card receive area
    UCHAR NicXmitStart;                 // MSB, LSB assumed 0
    UCHAR NicPageStart;                 // MSB, LSB assumed 0
    UCHAR NicPageStop;                  // MSB, LSB assumed 0

    //
    // Receive information
    //

    UCHAR NicNextPacket;                // MSB, LSB assumed 0
    UCHAR Current;                      // MSB, LSB assumed 0 (last known value)
    UCHAR XmitStatus;                   // status of last transmit

    //
    // Operational information.
    //

    ULONG IoPAddr;                      // physical address of NIC ports
    UCHAR InterruptStatus;
    UCHAR StationAddress[ETH_LENGTH_OF_ADDRESS];    // filled in at init time
    UCHAR PermanentAddress[ETH_LENGTH_OF_ADDRESS];  // filled in at init time
    PUCHAR RamBase;                     // Base address of card address space
    ULONG RamSize;                      // Amount of ram in card
    BOOLEAN BufferOverflow;             // does an overflow need to be handled
    BOOLEAN ReceiveInProgress;          // to prevent reentering indications
    BOOLEAN IndicateReceiveDone;
    BOOLEAN Ne2000HandleXmitCompleteRunning;
    BOOLEAN EightBitSlot;
    BOOLEAN DpcInProgress;              // To prevent multiple DPCs and to allow
                                        // reset to known if to pause.
    
    //
    // Statistics used by Set/QueryInformation.
    //

    ULONG FramesXmitGood;               // Good Frames Transmitted
    ULONG FramesRcvGood;                // Good Frames Received
    ULONG FramesXmitBad;                // Bad Frames Transmitted
    ULONG FramesXmitOneCollision;       // Frames Transmitted with one collision
    ULONG FramesXmitManyCollisions;     // Frames Transmitted with > 1 collision
    ULONG FrameAlignmentErrors;         // FAE errors counted
    ULONG CrcErrors;                    // CRC errors counted
    ULONG MissedPackets;                // missed packet counted

    //
    // Reset information.
    //

    BOOLEAN ResetInProgress;            // TRUE during a reset
    RESET_STAGE NextResetStage;         // where in the reset we are
    struct _NE2000_OPEN * ResetOpen;    // who called Ne2000Reset
    UINT OctoCount;                    // keeps track of how often wakeupdpc has triggered

    //
    // Pointer to the filter database for the MAC.
    //
    PETH_FILTER FilterDB;
    
    UCHAR NicMulticastRegs[8];          // contents of card MC registers
    UINT ByteToWrite;                   // temp storage
    UCHAR NicReceiveConfig;             // contents of NIC RCR
    UCHAR NicInterruptMask;             // contents of NIC IMR

    //
    // Look Ahead information.
    //

    ULONG MaxLookAhead;

    //
    // Loopback information
    //

    PNDIS_PACKET LoopbackQueue;         // queue of packets to loop back
    PNDIS_PACKET LoopbackQTail;
    PNDIS_PACKET LoopbackPacket;        // current one we are looping back

    //
    // Pending operations
    //

    PNE2000_PEND_DATA PendQueue;        // List of operations to complete
    PNE2000_PEND_DATA PendQTail;
    PNE2000_PEND_DATA PendOp;           // Outstanding operation

    NDIS_TIMER DeferredTimer;
    PVOID DeferredDpc;

    NDIS_TIMER XmitInterruptTimer;      // handles transmit complete interrupt
    NDIS_TIMER LoopbackQueueTimer;      // handles loopback queue

    PVOID WakeUpDpc;
    NDIS_TIMER WakeUpTimer;
    BOOLEAN WakeUpFoundTransmit;

    BOOLEAN Removed;

    //
    // These are for the current packet being indicated.
    //

    UCHAR PacketHeader[4];              // the NIC appended header
    PUCHAR PacketHeaderLoc;             // Ne2000 address of the beginning of the packet.

    UCHAR Lookahead[NE2000_MAX_LOOKAHEAD + NE2000_HEADER_SIZE];

    UINT PacketLen;                     // the overall length of the packet

} NE2000_ADAPTER, * PNE2000_ADAPTER;

//
// Given a MacBindingHandle this macro returns a pointer to the
// NE2000_ADAPTER.
//
#define PNE2000_ADAPTER_FROM_BINDING_HANDLE(Handle) \
    (((PNE2000_OPEN)(Handle))->Adapter)

//
// Given a MacContextHandle return the PNE2000_ADAPTER
// it represents.
//
#define PNE2000_ADAPTER_FROM_CONTEXT_HANDLE(Handle) \
    ((PNE2000_ADAPTER)(Handle))

//
// Given a pointer to a NE2000_ADAPTER return the
// proper MacContextHandle.
//
#define CONTEXT_HANDLE_FROM_PNE2000_ADAPTER(Ptr) \
    ((NDIS_HANDLE)(Ptr))

//
// Macros to extract high and low bytes of a word.
//

#define MSB(Value) ((UCHAR)((((ULONG)Value) >> 8) & 0xff))
#define LSB(Value) ((UCHAR)(((ULONG)Value) & 0xff))

//
// One of these per open on an adapter.
//

typedef struct _NE2000_OPEN {

    //
    // NDIS wrapper information.
    //

    NDIS_HANDLE NdisBindingContext;     // passed to MacOpenAdapter
    PSTRING AddressingInformation;      // not used currently

    //
    // Links to our adapter.
    //

    PNE2000_ADAPTER Adapter;
    struct _NE2000_OPEN * NextOpen;

    //
    // Links to our MAC.
    //

    PMAC_BLOCK MacBlock;            // faster than using AdapterBlock->MacBlock

    //
    // Index of this adapter in the filter database.
    //
    NDIS_HANDLE NdisFilterHandle;

    //
    // Indication information
    //

    UINT LookAhead;

    //
    // Reset/Close information.
    //

    UINT ReferenceCount;             // number of reasons this open can't close
    BOOLEAN Closing;                 // is a close pending

    NDIS_REQUEST CloseFilterRequest; // Holds Requests for pending close op
    NDIS_REQUEST CloseAddressRequest;// Holds Requests for pending close op
    
    UINT ProtOptionFlags;           //Holds the value of OID_GENERAL_PROTOCOL_OPTIONS

} NE2000_OPEN, * PNE2000_OPEN;

//
// This macro returns a pointer to a PNE2000_OPEN given a MacBindingHandle.
//
#define PNE2000_OPEN_FROM_BINDING_HANDLE(Handle) \
    ((PNE2000_OPEN)(Handle))

//
// This macro returns a NDIS_HANDLE from a PNE2000_OPEN
//
#define BINDING_HANDLE_FROM_PNE2000_OPEN(Open) \
    ((NDIS_HANDLE)(Open))

typedef struct _NE2000_REQUEST_RESERVED {
    PNDIS_REQUEST Next;    // Next NDIS_REQUEST in chain for this binding
    ULONG OidsLeft;        // Number of Oids left to process
    PUCHAR BufferPointer;  // Next available byte in information buffer
} NE2000_REQUEST_RESERVED, *PNE2000_REQUEST_RESERVED;

// A MACRO to return a pointer to the reserved portion of an NDIS request
#define PNE2000_RESERVED_FROM_REQUEST(Request) \
    ((PNE2000_REQUEST_RESERVED)((Request)->MacReserved)

//
// What we map into the reserved section of a packet.
// Cannot be more than 16 bytes (see ASSERT in ne2000.c).
//

typedef struct _MAC_RESERVED {
    PNDIS_PACKET NextPacket;    // used to link in the queues (4 bytes)
    PNE2000_OPEN Open;          // open that called Ne2000Send (4 bytes)
    BOOLEAN Loopback;           // is this a loopback packet (1 byte)
} MAC_RESERVED, * PMAC_RESERVED;

//
// These appear in the status field of MAC_RESERVED; they are
// used because there is not enough room for a full NDIS_HANDLE.
//

#define RESERVED_SUCCESS   ((USHORT)0)
#define RESERVED_FAILURE   ((USHORT)1)

//
// Retrieve the MAC_RESERVED structure from a packet.
//

#define RESERVED(Packet) ((PMAC_RESERVED)((Packet)->MacReserved))

//
// Procedures which log errors.
//

typedef enum _NE2000_PROC_ID {
    openAdapter,
    cardReset,
    cardCopyDownPacket,
    cardCopyDownBuffer,
    cardCopyUp
} NE2000_PROC_ID;

#define NE2000_ERRMSG_CARD_SETUP          (ULONG)0x01
#define NE2000_ERRMSG_DATA_PORT_READY     (ULONG)0x02
#define NE2000_ERRMSG_MAX_OPENS           (ULONG)0x03
#define NE2000_ERRMSG_HANDLE_XMIT_COMPLETE (ULONG)0x04

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
//

#define NE2000_DO_DEFERRED(Adapter) \
{ \
    PNE2000_ADAPTER _A = (Adapter); \
    _A->References--;               \
    if ((!_A->References) &&        \
        (_A->ResetInProgress ||     \
         (_A->PendQueue != NULL) || \
         (_A->CloseQueue != NULL))) {\
        NdisReleaseSpinLock(&_A->Lock); \
        NdisSetTimer(&_A->DeferredTimer, 1);\
    } else if ((_A->XmitQueue != NULL) && \
               (_A->BufferStatus[_A->NextBufToFill] == EMPTY) && \
               !_A->DpcInProgress) {\
        Ne2000CopyAndSend(_A);\
        NdisReleaseSpinLock(&_A->Lock); \
    } else { \
        NdisReleaseSpinLock(&_A->Lock); \
    } \
}

//
// Declarations for functions in ne2000.c.
//

VOID
Ne2000AdjustMaxLookAhead(
    IN PNE2000_ADAPTER Adapter
    );

NDIS_STATUS
Ne2000RegisterAdapter(
    IN PNE2000_ADAPTER NewAdapter,
    IN PNDIS_STRING AdapterName,
    IN NDIS_HANDLE ConfigurationHandle,
    IN BOOLEAN ConfigError,
    IN ULONG ConfigErrorValue
    );

BOOLEAN
Ne2000Isr(
    IN PVOID ServiceContext         // will be a pointer to the adapter block
    );

VOID
Ne2000Dpc(
    IN PVOID SystemSpecific1,
    IN PVOID InterruptContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

VOID
Ne2000XmitDpc(
    IN PNE2000_ADAPTER Adapter
    );

BOOLEAN
Ne2000RcvDpc(
    IN PNE2000_ADAPTER Adapter
    );

VOID
Ne2000WakeUpDpc(
    IN PVOID SystemSpecific1,
    IN PVOID InterruptContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

NDIS_STATUS
Ne2000OpenAdapter(
    OUT PNDIS_STATUS OpenErrorStatus,
    OUT NDIS_HANDLE * MacBindingHandle,
    OUT PUINT SelectedMediumIndex,
    IN PNDIS_MEDIUM MediumArray,
    IN UINT MediumArraySize,
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacAdapterContext,
    IN UINT OpenOptions,
    IN PSTRING AddressingInformation OPTIONAL
    );

NDIS_STATUS
Ne2000CloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    );

NDIS_STATUS
Ne2000Reset(
    IN NDIS_HANDLE MacBindingHandle
    );

NDIS_STATUS
Ne2000Request(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );

NDIS_STATUS
Ne2000QueryInformation(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    );

NDIS_STATUS
Ne2000SetInformation(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    );

NDIS_STATUS
Ne2000SetMulticastAddresses(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN UINT NumAddresses,
    IN CHAR AddressList[][ETH_LENGTH_OF_ADDRESS]
    );

NDIS_STATUS
Ne2000SetPacketFilter(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN UINT PacketFilter
    );

NDIS_STATUS
Ne2000QueryGlobalStatistics(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );

VOID
Ne2000Unload(
    IN NDIS_HANDLE MacMacContext
    );

NDIS_STATUS
Ne2000AddAdapter(
    IN NDIS_HANDLE NdisMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    );

VOID
Ne2000RemoveAdapter(
    IN PVOID MacAdapterContext
    );

NDIS_STATUS
Ne2000Stage1Reset(
    PNE2000_ADAPTER Adapter
    );

NDIS_STATUS
Ne2000Stage2Reset(
    PNE2000_ADAPTER Adapter
    );

NDIS_STATUS
Ne2000Stage3Reset(
    PNE2000_ADAPTER Adapter
    );

NDIS_STATUS
Ne2000Stage4Reset(
    PNE2000_ADAPTER Adapter
    );

VOID
Ne2000ResetStageDone(
    PNE2000_ADAPTER Adapter,
    RESET_STAGE StageDone
    );

NDIS_STATUS
Ne2000ChangeMulticastAddresses(
    IN UINT OldFilterCount,
    IN CHAR OldAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN UINT NewFilterCount,
    IN CHAR NewAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

NDIS_STATUS
Ne2000ChangeFilterClasses(
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

VOID
Ne2000CloseAction(
    IN NDIS_HANDLE MacBindingHandle
    );

//
// functions in interrup.c
//

INDICATE_STATUS
Ne2000IndicateLoopbackPacket(
    IN PNE2000_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    );

UINT
Ne2000CopyOver(
    OUT PUCHAR Buf,                 // destination
    IN PNDIS_PACKET Packet,         // source packet
    IN UINT Offset,                 // offset in packet
    IN UINT Length                  // number of bytes to copy
    );

BOOLEAN
Ne2000PacketOK(
    IN PNE2000_ADAPTER Adapter
    );

INDICATE_STATUS
Ne2000IndicatePacket(
    IN PNE2000_ADAPTER Adapter
    );

NDIS_STATUS
Ne2000TransferData(
    IN NDIS_HANDLE MacBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );

VOID
OctogmetusceratorRevisited(
    IN PNE2000_ADAPTER Adapter
    );
    
//
// Declarations for functions in pend.c
//

VOID
HandlePendingOperations(
    IN PVOID SystemSpecific1,
    IN PVOID DeferredContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

NDIS_STATUS
DispatchSetPacketFilter(
    IN PNE2000_ADAPTER Adapter
    );

NDIS_STATUS
DispatchSetMulticastAddressList(
    IN PNE2000_ADAPTER Adapter
    );

//
// Declarations for functions in send.c.
//

NDIS_STATUS
Ne2000Send(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );

VOID
Ne2000SetLoopbackFlag(
    IN PNE2000_ADAPTER Adapter,
    IN OUT PNDIS_PACKET Packet
    );

VOID
Ne2000HandleXmitComplete(
    IN PNE2000_ADAPTER Adapter
    );

VOID
Ne2000CopyAndSend(
    IN PNE2000_ADAPTER Adapter
    );

//
// Declarations of functions in card.c.
//

BOOLEAN
CardCheckParameters(
    IN PNE2000_ADAPTER Adapter
    );

BOOLEAN
CardInitialize(
    IN PNE2000_ADAPTER Adapter
    );

VOID
CardReadEthernetAddress(
    IN PNE2000_ADAPTER Adapter
    );

BOOLEAN
CardSetup(
    IN PNE2000_ADAPTER Adapter
    );

VOID
CardStop(
    IN PNE2000_ADAPTER Adapter
    );

BOOLEAN
CardTest(
    IN PNE2000_ADAPTER Adapter
    );

BOOLEAN
CardReset(
    IN PNE2000_ADAPTER Adapter
    );

BOOLEAN
CardCopyDownPacket(
    IN PNE2000_ADAPTER Adapter,
    IN PNDIS_PACKET Packet,
    OUT UINT * Length
    );

BOOLEAN
CardCopyDown(
    IN PNE2000_ADAPTER Adapter,
    IN PUCHAR TargetBuffer,
    IN PUCHAR SourceBuffer,
    IN UINT Length
    );

BOOLEAN
CardCopyUp(
    IN PNE2000_ADAPTER Adapter,
    IN PUCHAR Target,
    IN PUCHAR Source,
    IN UINT Length
    );

ULONG
CardComputeCrc(
    IN PUCHAR Buffer,
    IN UINT Length
    );

VOID
CardGetPacketCrc(
    IN PUCHAR Buffer,
    IN UINT Length,
    OUT UCHAR Crc[4]
    );

VOID
CardGetMulticastBit(
    IN UCHAR Address[ETH_LENGTH_OF_ADDRESS],
    OUT UCHAR * Byte,
    OUT UCHAR * Value
    );

VOID
CardFillMulticastRegs(
    IN PNE2000_ADAPTER Adapter
    );

VOID
CardSetBoundary(
    IN PNE2000_ADAPTER Adapter
    );

VOID
CardStartXmit(
    IN PNE2000_ADAPTER Adapter
    );

//
// These are the functions that are defined in sync.c and
// are meant to be called through NdisSynchronizeWithInterrupt().
//

BOOLEAN
SyncCardStop(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardGetXmitStatus(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardGetCurrent(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardSetReceiveConfig(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardSetAllMulticast(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardCopyMulticastRegs(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardSetInterruptMask(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardAcknowledgeOverflow(
    IN PVOID SynchronizeContext
    );
    
BOOLEAN
SyncCardUpdateCounters(
    IN PVOID SynchronizeContext
    );

BOOLEAN
SyncCardHandleOverflow(
    IN PVOID SynchronizeContext
    );

/*++

Routine Description:

    Determines the type of the interrupt on the card. The order of
    importance is overflow, then transmit complete, then receive.
    Counter MSB is handled first since it is simple.

Arguments:

    AdaptP - pointer to the adapter block

    InterruptStatus - Current Interrupt Status.

Return Value:

    The type of the interrupt

--*/
#define CARD_GET_INTERRUPT_TYPE(_A, _I)                 \
  (_I & ISR_COUNTER) ?                               \
      COUNTER :                                      \
      (_I & ISR_OVERFLOW ) ?                         \
      SyncCardUpdateCounters(_A), OVERFLOW :                 \
        (_I & (ISR_XMIT|ISR_XMIT_ERR)) ?           \
          TRANSMIT :                                     \
        (_I & ISR_RCV) ?                               \
          RECEIVE :                                  \
        (_I & ISR_RCV_ERR) ?                           \
              SyncCardUpdateCounters(_A), RECEIVE :  \
              UNKNOWN
                      
#endif // NE2000SFT

