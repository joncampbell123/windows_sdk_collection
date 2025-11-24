/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/*++

Module Name:

    ndiswidgt.h

Abstract:

    Main header file for the widget wrapper

--*/


#ifndef _NDIS_WIDGET_
#define _NDIS_WIDGET_

#ifndef EXPORT
#define EXPORT 
#endif

//
//
//
//
// Begin definitions for Widgets
//
//
//
//

//#include <efilter.h>
//#include <tfilter.h>
//#include <ffilter.h>

#define NDIS_W_MAX_MULTI_LIST 32
#define NDIS_W_MAX_LOOKAHEAD 240

//
// declare these first since they point to each other
//
typedef USHORT WCHAR;
typedef WCHAR *PWSTR;

typedef struct _NDIS_W_DRIVER_BLOCK   NDIS_W_DRIVER_BLOCK,   * PNDIS_W_DRIVER_BLOCK;
typedef struct _NDIS_WIDGET_BLOCK     NDIS_WIDGET_BLOCK,     * PNDIS_WIDGET_BLOCK;
typedef struct _NDIS_W_PROTOCOL_BLOCK NDIS_W_PROTOCOL_BLOCK, * PNDIS_W_PROTOCOL_BLOCK;
typedef struct _NDIS_W_OPEN_BLOCK     NDIS_W_OPEN_BLOCK,     * PNDIS_W_OPEN_BLOCK;





//
// Function types for NDIS_WIDGET_CHARACTERISTICS
//



typedef
BOOLEAN
(*W_CHECK_FOR_HANG_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext
    );

typedef
VOID
(*W_DISABLE_INTERRUPT_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext
    );

typedef
VOID
(*W_ENABLE_INTERRUPT_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext
    );

typedef
VOID
(*W_HALT_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext
    );

typedef
VOID
(*W_HANDLE_INTERRUPT_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext
    );

typedef
NDIS_STATUS
(*W_INITIALIZE_HANDLER) (
    OUT PNDIS_STATUS OpenErrorStatus,
    OUT PUINT SelectedMediumIndex,
    IN PNDIS_MEDIUM MediumArray,
    IN UINT MediumArraySize,
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_HANDLE WrapperConfigurationContext
    );

typedef
VOID
(*W_ISR_HANDLER) (
    OUT PBOOLEAN InterruptRecognized,
    OUT PBOOLEAN QueueWidgetHandleInterrupt,
    IN NDIS_HANDLE WidgetAdapterContext
    );

typedef
NDIS_STATUS
(*W_QUERY_INFORMATION_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN ULONG InformationBufferLength,
    OUT PULONG BytesWritten,
    OUT PULONG BytesNeeded
    );

typedef
NDIS_STATUS
(*W_RECONFIGURE_HANDLER) (
    OUT PNDIS_STATUS OpenErrorStatus,
    IN NDIS_HANDLE WidgetAdapterContext,
    IN NDIS_HANDLE WrapperConfigurationContext
    );

typedef
NDIS_STATUS
(*W_RESET_HANDLER) (
    OUT PBOOLEAN AddressingReset,
    IN NDIS_HANDLE WidgetAdapterContext
    );

typedef
NDIS_STATUS
(*W_SEND_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext,
    IN PNDIS_PACKET Packet,
    IN UINT Flags
    );

typedef
NDIS_STATUS
(*W_SET_INFORMATION_HANDLER) (
    IN NDIS_HANDLE WidgetAdapterContext,
    IN NDIS_OID Oid,
    IN PVOID InformationBuffer,
    IN ULONG InformationBufferLength,
    OUT PULONG BytesRead,
    OUT PULONG BytesNeeded
    );

typedef
NDIS_STATUS
(*W_TRANSFER_DATA_HANDLER) (
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred,
    IN NDIS_HANDLE WidgetAdapterContext,
    IN NDIS_HANDLE WidgetReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer
    );


typedef struct _NDIS_WIDGET_CHARACTERISTICS {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;
    UCHAR Pad1;
    UCHAR Pad2;
    UINT Reserved;
    W_CHECK_FOR_HANG_HANDLER    CheckForHangHandler;
    W_DISABLE_INTERRUPT_HANDLER DisableInterruptHandler;
    W_ENABLE_INTERRUPT_HANDLER  EnableInterruptHandler;
    W_HALT_HANDLER              HaltHandler;
    W_HANDLE_INTERRUPT_HANDLER  HandleInterruptHandler;
    W_INITIALIZE_HANDLER        InitializeHandler;
    W_ISR_HANDLER               ISRHandler;
    W_QUERY_INFORMATION_HANDLER QueryInformationHandler;
    W_RECONFIGURE_HANDLER       ReconfigureHandler;
    W_RESET_HANDLER             ResetHandler;
    W_SEND_HANDLER              SendHandler;
    W_SET_INFORMATION_HANDLER   SetInformationHandler;
    W_TRANSFER_DATA_HANDLER     TransferDataHandler;
} NDIS_WIDGET_CHARACTERISTICS, *PNDIS_WIDGET_CHARACTERISTICS;

//
// one of these per Driver
//

typedef struct _kevent {
	UINT ke_pad1;
	UINT ke_pad2;
	UINT ke_pad3;
	UINT ke_pad4;
} KEVENT, *PKEVENT;

typedef struct _ntp {
	KEVENT ntp1;
	KEVENT ntp2;
	KEVENT ntp3;
	UINT ntp4;
	UINT ntp5;
	UINT ntp6;
} NTP, *PNTP;

struct _NDIS_W_DRIVER_BLOCK {
    PNDIS_WIDGET_BLOCK WidgetQueue;     // queue of widgets for this driver
    NDIS_HANDLE WidgetIdField;

    REFERENCE Ref;                      // contains spinlock for WidgetQueue
    UINT Length;                        // of this NDIS_DRIVER_BLOCK structure
    NDIS_WIDGET_CHARACTERISTICS WidgetCharacteristics;    // handler addresses
    NDIS_HANDLE NdisDriverInfo;  // Driver information.
    PNDIS_W_DRIVER_BLOCK NextDriver;
    PWRAPPER_MAC_BLOCK FakeMac;
    KEVENT WidgetsRemovedEvent;         // used to find when all widgets are gone.
    BOOLEAN Unloading;                  // TRUE if unloading

};

typedef PVOID PKINTERRUPT;
typedef ULONG KSPIN_LOCK, *PKSPIN_LOCK;
//
// DPC routine
//



typedef
VOID
(*PKDEFERRED_ROUTINE) (
    IN struct _KDPC *Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

//
// Deferred Procedure Call (DPC) object
//

typedef struct _KDPC {
    SHORT Type;
    SHORT Size;
    LIST_ENTRY DpcListEntry;
    PKDEFERRED_ROUTINE DeferredRoutine;
    PVOID DeferredContext;
    PVOID SystemArgument1;
    PVOID SystemArgument2;
    BOOLEAN Inserted;
} KDPC;



#if 0
typedef struct _NDIS_WIDGET_INTERRUPT {
    PKINTERRUPT InterruptObject;
    KSPIN_LOCK DpcCountLock;
    PVOID WidgetIdField;
    W_ISR_HANDLER WidgetIsr;
    W_HANDLE_INTERRUPT_HANDLER WidgetDpc;
    KDPC InterruptDpc;
    PNDIS_WIDGET_BLOCK Widget;
    UCHAR DpcCount;
    BOOLEAN Filler1;

    //
    // This is used to tell when all the Dpcs for the adapter are completed.
    //

    KEVENT DpcsCompletedEvent;

    BOOLEAN SharedInterrupt;
    BOOLEAN IsrRequested;

} NDIS_WIDGET_INTERRUPT, *PNDIS_WIDGET_INTERRUPT;
#endif
typedef struct _NDIS_WIDGET_INTERRUPT {
    UINT InterruptFlags;
    PVOID InterruptContext;
    W_ISR_HANDLER WidgetIsr;
    W_HANDLE_INTERRUPT_HANDLER WidgetDpc;
    PVOID   VPICD_Context;          //context for removing interrupt
    struct _NDIS_WIDGET_INTERRUPT *Next;   // link for shared interrupts
//    KSPIN_LOCK DpcCountLock;
//    PVOID WidgetIdField;
//    KDPC InterruptDpc;
    PNDIS_WIDGET_BLOCK Widget;
    UCHAR DpcCount;
    BOOLEAN Filler1;

    //
    // This is used to tell when all the Dpcs for the adapter are completed.
    //
    KEVENT DpcsCompletedEvent;
    BOOLEAN SharedInterrupt;
    BOOLEAN IsrRequested;

} NDIS_WIDGET_INTERRUPT, *PNDIS_WIDGET_INTERRUPT;

typedef struct _longint {
	DWORD lowpart;
	ULONG highpart;
} LI;

typedef struct _KTIMER {
    KEVENT kvt;
    LI largeint;
    LIST_ENTRY timerlistentry;
    PVOID *Dpc;
    BOOLEAN Inserted;
} KTIMER;


/*
typedef struct _KTIMER {
    DISPATCHER_HEADER Header;
    LI DueTime;
    LIST_ENTRY TimerListEntry;
    struct _KDPC *Dpc;
    BOOLEAN Inserted;
} KTIMER;
*/

typedef PVOID PNDIS_TIMER_FUNCTION;

typedef struct _NDIS_WIDGET_TIMER {
    NDIS_TIMER Timer;
    KDPC Dpc;
    PNDIS_TIMER_FUNCTION WidgetTimerFunction;
    PVOID WidgetTimerContext;
    PNDIS_WIDGET_BLOCK Widget;
    struct _NDIS_WIDGET_TIMER *NextDeferredTimer;
} NDIS_WIDGET_TIMER, *PNDIS_WIDGET_TIMER;


typedef struct _mapregister {
    PVOID	VirtAddr;
    PVOID	PhysAddr;
#ifdef DEBUG
    PVOID	MapXferFrom;
#endif
} SYSTEM_MAP_REGISTER, *PSYSTEM_MAP_REGISTER;

typedef struct _MAP_REGISTER_ENTRY {
    PSYSTEM_MAP_REGISTER MapRegister;
    BOOLEAN WriteToDevice;
} MAP_REGISTER_ENTRY, * PMAP_REGISTER_ENTRY;

//
// one of these per widget registered on a Driver
//
typedef PVOID PCM_RESOURCE_LIST;
typedef PVOID PFILE_OBJECT;
typedef PVOID PADAPTER_OBJECT;
#pragma pack(4)

struct _NDIS_WIDGET_BLOCK {
    ULONG Flags;                        // used to distinquish between MACs and Widgets
    PDEVICE_OBJECT DeviceObject;        // created by the wrapper
    PNDIS_W_DRIVER_BLOCK DriverHandle;  // pointer to our Driver block
    NDIS_HANDLE WidgetAdapterContext;   // context when calling widget functions
    NDIS_STRING WidgetName;             // how widget refers to us
    PNDIS_W_OPEN_BLOCK OpenQueue;       // queue of opens for this widget
    PNDIS_WIDGET_BLOCK NextWidget;      // used by driver's WidgetQueue
    REFERENCE Ref;                      // contains spinlock for OpenQueue
    ULONG pad1;				// TO MATCH NT!
    BOOLEAN BeingRemoved;               // TRUE if widget is being removed
    BOOLEAN HaltingWidget;              // TRUE if widget halt handler needs to be called
    BOOLEAN ProcessingDeferred;         // TRUE if processing deferred operations

    //
    // Synchronization stuff.
    //
    // The boolean is used to lock out several DPCs from running at the
    // same time.  The difficultly is if DPC A releases the spin lock
    // and DPC B tries to run, we want to defer B until after A has
    // exited.
    //
    BOOLEAN LockAcquired;
    NDIS_SPIN_LOCK Lock;
    ULONG pad2;				//TO MATCH NT!
    PNDIS_WIDGET_INTERRUPT Interrupt;

    //
    // Stuff that got deferred.
    //
    BOOLEAN RunDpc;
    BOOLEAN Timeout;
    BOOLEAN InAddDriver;
    BOOLEAN InInitialize;
    PNDIS_WIDGET_TIMER RunTimer;
    NDIS_TIMER DpcTimer;
    NTP	pad3;
    NDIS_TIMER WakeUpDpcTimer;
    NTP	pad4;
    DWORD  pad5;
    PETH_FILTER EthDB;
    PTR_FILTER TrDB;
    PFDDI_FILTER FddiDB;
    PVOID FilterDB;
//    PARC_FILTER ArcDB;  for when we do arcnet support
    NDIS_MEDIUM MediaType;

    UCHAR TrResetRing;
    UCHAR ArcnetAddress;
    BOOLEAN ArcnetBroadcastSet;
    PUCHAR ArcnetDataBuffer;
    NDIS_HANDLE ArcnetBufferPool;
    PVOID ArcnetBufferList;

    //
    // Resource information
    //
    PCM_RESOURCE_LIST Resources;

    //
    // contains widget information
    //
    ULONG BusNumber;
    NDIS_INTERFACE_TYPE BusType;
    NDIS_INTERFACE_TYPE AdapterType;
    BOOLEAN Master;

    //
    // Holds the map registers for this widget.
    //
    BOOLEAN Dma32BitAddresses;
    PMAP_REGISTER_ENTRY MapRegisters;
    ULONG PhysicalMapRegistersNeeded;
    ULONG MaximumPhysicalMapping;

    //
    // These two are used temporarily while allocating
    // the map registers.
    //
    KEVENT AllocationEvent;
    UINT CurrentMapRegister;
    PADAPTER_OBJECT SystemAdapterObject;

    //
    // Holds media specific information
    //
//    PARC_BUFFER_LIST ArcnetBufferList; more arcnet shit

    //
    // Send information
    //
    PNDIS_PACKET FirstPacket;
    PNDIS_PACKET LastPacket;
    PNDIS_PACKET FirstPendingPacket;
    PNDIS_PACKET LastWidgetPacket;
    ULONG SendResourcesAvailable;

    //
    // Transfer data information
    //
    PNDIS_PACKET FirstTDPacket;
    PNDIS_PACKET LastTDPacket;
    PNDIS_PACKET LoopbackPacket;
    UINT LoopbackPacketHeaderSize;

    //
    // Reset information
    //
    PNDIS_W_OPEN_BLOCK ResetRequested;
    PNDIS_W_OPEN_BLOCK ResetInProgress;

    //
    // RequestInformation
    //
    KEVENT RequestEvent;
    PNDIS_REQUEST FirstPendingRequest;
    PNDIS_REQUEST LastPendingRequest;
    PNDIS_REQUEST WidgetRequest;
    NDIS_REQUEST InternalRequest;
    NDIS_STATUS RequestStatus;
    UINT MaximumLongAddresses;
    UINT MaximumShortAddresses;
    UINT CurrentLookahead;
    UINT MaximumLookahead;
    UINT MacOptions;
    ULONG SupportedPacketFilters;
    BOOLEAN NeedToUpdateEthAddresses;
    BOOLEAN NeedToUpdatePacketFilter;
    BOOLEAN NeedToUpdateFunctionalAddress;
    BOOLEAN NeedToUpdateGroupAddress;
    BOOLEAN NeedToUpdateFddiLongAddresses;
    BOOLEAN NeedToUpdateFddiShortAddresses;
    BOOLEAN RunDoRequests;
    BOOLEAN ProcessOddDeferredStuff;
    UCHAR MulticastBuffer[NDIS_W_MAX_MULTI_LIST][6];
    UCHAR LookaheadBuffer[NDIS_W_MAX_LOOKAHEAD];

    //
    // Temp stuff for using the old NDIS functions
    //
    ULONG ChannelNumber;
    NDIS_HANDLE NdisAdapterHandle;
    BOOLEAN IsrRequested;
    BOOLEAN SharedInterrupt;
};

#pragma pack()

//
// one of these per open on an widget/protocol
//

struct _NDIS_W_OPEN_BLOCK {
    PNDIS_W_DRIVER_BLOCK DriverHandle;      // pointer to our driver
    PNDIS_WIDGET_BLOCK WidgetHandle;        // pointer to our widget
    PWRAPPER_PROTOCOL_BLOCK ProtocolHandle;    // pointer to our protocol
    PWRAPPER_OPEN_BLOCK FakeOpen;              // Pointer to fake open block
    NDIS_HANDLE ProtocolBindingContext;     // context when calling ProtXX funcs
    NDIS_HANDLE WidgetAdapterContext;       // context when calling WidgetXX funcs
    PNDIS_W_OPEN_BLOCK WidgetNextOpen;      // used by widget's OpenQueue
    PFILE_OBJECT FileObject;                // created by operating system
    BOOLEAN Closing;                        // TRUE when removing this struct
    NDIS_HANDLE CloseRequestHandle;         // 0 indicates an internal close
    NDIS_HANDLE FilterHandle;
    NDIS_SPIN_LOCK SpinLock;                // guards Closing
    ULONG References;
    UINT CurrentLookahead;
    ULONG ProtocolOptions;

    //
    // These are optimizations for getting to driver routines.  They are not
    // necessary, but are here to save a dereference through the Driver block.
    //

    W_SEND_HANDLER SendHandler;
    W_TRANSFER_DATA_HANDLER TransferDataHandler;

    //
    // These are optimizations for getting to PROTOCOL routines.  They are not
    // necessary, but are here to save a dereference through the PROTOCOL block.
    //

    SEND_COMPLETE_HANDLER SendCompleteHandler;
    TRANSFER_DATA_COMPLETE_HANDLER TransferDataCompleteHandler;
    RECEIVE_HANDLER ReceiveHandler;
    RECEIVE_COMPLETE_HANDLER ReceiveCompleteHandler;

};

//
// NOTE: THIS STRUCTURE MUST, MUST, MUST ALIGN WITH THE
// NDIS_USER_OPEN_CONTEXT STRUCTURE defined in the wrapper.
//
typedef struct _NDIS_W_USER_OPEN_CONTEXT {
    PDEVICE_OBJECT DeviceObject;
    PNDIS_WIDGET_BLOCK WidgetBlock;
    ULONG OidCount;
    PNDIS_OID OidArray;
} NDIS_W_USER_OPEN_CONTEXT, *PNDIS_W_USER_OPEN_CONTEXT;


typedef struct _NDIS_REQUEST_RESERVED {
    PNDIS_REQUEST Next;
    struct _NDIS_W_OPEN_BLOCK * Open;
} NDIS_REQUEST_RESERVED, *PNDIS_REQUEST_RESERVED;


#define PNDIS_RESERVED_FROM_PNDIS_REQUEST(_request) \
   ((PNDIS_REQUEST_RESERVED)((_request)->MacReserved))


#define WIDGET_LOCK_ACQUIRED(_widget) (_widget->LockAcquired)

BOOLEAN
NdisWIsr(
//    IN PKINTERRUPT KInterrupt,
    IN PVOID Context
    );

VOID
NdisWDpc(
    IN PVOID SystemSpecific1,
    IN PVOID InterruptContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

VOID
NdisWDpcTimer(
    IN PVOID SystemSpecific1,
    IN PVOID InterruptContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

VOID
NdisWWakeUpDpc(
    PKDPC Dpc,
    PVOID Context,
    PVOID SystemContext1,
    PVOID SystemContext2
    );

NDIS_STATUS
NdisWChangeEthAddresses(
    IN UINT OldAddressCount,
    IN CHAR OldAddresses[][6],
    IN UINT NewAddressCount,
    IN CHAR NewAddresses[][6],
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

NDIS_STATUS
NdisWChangeClass(
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

VOID
NdisWCloseAction(
    IN NDIS_HANDLE MacBindingHandle
    );

typedef DWORD TR_FUNCTIONAL_ADDRESS;

NDIS_STATUS
NdisWChangeFunctionalAddress(
    IN TR_FUNCTIONAL_ADDRESS OldFunctionalAddress,
    IN TR_FUNCTIONAL_ADDRESS NewFunctionalAddress,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

NDIS_STATUS
NdisWChangeGroupAddress(
    IN TR_FUNCTIONAL_ADDRESS OldGroupAddress,
    IN TR_FUNCTIONAL_ADDRESS NewGroupAddress,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

NDIS_STATUS
NdisWChangeFddiAddresses(
    IN UINT oldLongAddressCount,
    IN CHAR oldLongAddresses[][6],
    IN UINT newLongAddressCount,
    IN CHAR newLongAddresses[][6],
    IN UINT oldShortAddressCount,
    IN CHAR oldShortAddresses[][2],
    IN UINT newShortAddressCount,
    IN CHAR newShortAddresses[][2],
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

VOID
WidgetProcessDeferred(
    PNDIS_WIDGET_BLOCK Widget
    );

NDIS_STATUS
NdisWSend(
    IN NDIS_HANDLE NdisBindingHandle,
    IN PNDIS_PACKET Packet
    );

NDIS_STATUS
NdisWTransferData(
    IN NDIS_HANDLE NdisBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    IN OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );

NDIS_STATUS
NdisWReset(
    IN NDIS_HANDLE NdisBindingHandle
    );

NDIS_STATUS
NdisWRequest(
    IN NDIS_HANDLE NdisBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );


/*++
INTERNAL
BOOLEAN
NdisReferenceDriver(
    IN PNDIS_W_DRIVER_BLOCK DriverP
    );
--*/
#define NdisReferenceDriver(DriverP) \
    ReferenceRef(&(DriverP)->Ref)


VOID
NdisDereferenceDriver(
    PNDIS_W_DRIVER_BLOCK DriverP
    );

INTERNAL
BOOLEAN
NdisQueueWidgetOnDriver(
    PNDIS_WIDGET_BLOCK WidgetP,
    PNDIS_W_DRIVER_BLOCK DriverP
    );

INTERNAL
VOID
NdisDequeueWidgetOnDriver(
    PNDIS_WIDGET_BLOCK WidgetP,
    PNDIS_W_DRIVER_BLOCK DriverP
    );

INTERNAL
BOOLEAN
NdisQueueOpenOnWidget(
    PNDIS_W_OPEN_BLOCK OpenP,
    PNDIS_WIDGET_BLOCK WidgetP
    );

INTERNAL
VOID
NdisKillWidget(
    PNDIS_WIDGET_BLOCK OldWidgetP
    );

/*++
INTERNAL
BOOLEAN
NdisReferenceWidget(
    IN PNDIS_Widget_BLOCK WidgetP
    );
--*/
#define NdisReferenceWidget(WidgetP) \
    ReferenceRef(&(WidgetP)->Ref)


VOID
NdisDereferenceWidget(
    PNDIS_WIDGET_BLOCK WidgetP
    );

INTERNAL
VOID
NdisDeQueueOpenOnWidget(
    PNDIS_W_OPEN_BLOCK OpenP,
    PNDIS_WIDGET_BLOCK WidgetP
    );

#if DBG
#define WIDGET_AT_DPC_LEVEL (KeGetCurrentIrql() == DISPATCH_LEVEL)
#else
#define WIDGET_AT_DPC_LEVEL 1
#endif

#define LOCK_WIDGET(_W, _L) \
{                           \
    if (_W->LockAcquired) { \
        _L = FALSE;         \
    } else {                \
        _L = TRUE;          \
        _W->LockAcquired = TRUE; \
    }                       \
}

#define UNLOCK_WIDGET(_W, _L) \
{                             \
    if (_L) {                 \
        _W->LockAcquired = FALSE; \
    }                         \
}


//
// Operating System Requests
//

EXPORT
NDIS_STATUS
NdisWAllocateMapRegisters(
    IN  NDIS_HANDLE WidgetAdapterHandle,
    IN  UINT DmaChannel,
    IN  BOOLEAN Dma32BitAddresses,
    IN  ULONG PhysicalMapRegistersNeeded,
    IN  ULONG MaximumPhysicalMapping
    );

EXPORT
VOID
NdisWFreeMapRegisters(
    IN  NDIS_HANDLE WidgetAdapterHandle
    );

EXPORT
NDIS_STATUS
NdisWRegisterIoPortRange(
    OUT PVOID *PortOffset,
    IN  NDIS_HANDLE WidgetAdapterHandle,
    IN  UINT InitialPort,
    IN  UINT NumberOfPorts
    );

EXPORT
VOID
NdisWDeregisterIoPortRange(
    IN  NDIS_HANDLE WidgetAdapterHandle,
    IN  UINT InitialPort,
    IN  UINT NumberOfPorts,
    IN  PVOID PortOffset
    );
typedef struct _QINT {
	ULONG LowPart;
	ULONG HighPart;
} QINT;

EXPORT
NDIS_STATUS
NdisWMapIoSpace(
    OUT PVOID * VirtualAddress,
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN QINT PhysicalAddress,
    IN UINT Length
    );

#ifdef NDIS_MINI_DRIVER

VOID
NdisWUnmapIoSpace(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    );

#else

#ifdef _ALPHA_

/*++
VOID
NdisWUnmapIoSpace(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    )
--*/
#define NdisWUnmapIoSpace(Handle,VirtualAddress,Length) {}

#else

/*++
VOID
NdisWUnmapIoSpace(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    )
--*/
#define NdisWUnmapIoSpace(Handle,VirtualAddress,Length) \
            MmUnmapIoSpace((VirtualAddress), (Length));

#endif
#endif


EXPORT
NDIS_STATUS
NdisWRegisterInterrupt(
    OUT PNDIS_WIDGET_INTERRUPT Interrupt,
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN UINT InterruptVector,
    IN UINT InterruptLevel,
    IN BOOLEAN RequestIsr,
    IN BOOLEAN SharedInterrupt,
    IN NDIS_INTERRUPT_MODE InterruptMode
    );

#ifdef NDIS_MINI_DRIVER

VOID
NdisWDeregisterInterrupt(
    IN PNDIS_WIDGET_INTERRUPT Interrupt
    );

#else

/*++
VOID
NdisWDeregisterInterrupt(
    IN PNDIS_WIDGET_INTERRUPT Interrupt
    )
--*/
#define NdisWDeregisterInterrupt(_I) \
        NdisRemoveInterrupt((PNDIS_INTERRUPT)_I);

#endif

#ifdef NDIS_MINI_DRIVER

BOOLEAN
NdisWSynchronizeWithInterrupt(
    IN PNDIS_WIDGET_INTERRUPT Interrupt,
    IN PVOID SynchronizeFunction,
    IN PVOID SynchronizeContext
    );

#else

/*++
BOOLEAN
NdisWSynchronizeWithInterrupt(
    IN PNDIS_WIDGET_INTERRUPT Interrupt,
    IN PVOID SynchronizeFunction,
    IN PVOID SynchronizeContext
    )
--*/

#define NdisWSynchronizeWithInterrupt(Interrupt,Function,Context) \
            KeSynchronizeExecution( \
                (Interrupt)->InterruptObject,\
                (PKSYNCHRONIZE_ROUTINE)Function,\
                Context  \
                )

#endif

//
// Timers
//

/*++
VOID
NdisWSetTimer(
    IN PNDIS_WIDGET_TIMER Timer,
    IN UINT MillisecondsToDelay
    );
--*/
#define NdisWSetTimer(_Timer, _Delay) NdisSetTimer((PNDIS_TIMER)(_Timer), _Delay)

EXPORT
VOID
NdisWInitializeTimer(
    IN OUT PNDIS_WIDGET_TIMER Timer,
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN PNDIS_TIMER_FUNCTION TimerFunction,
    IN PVOID FunctionContext
    );

VOID
NdisWCancelTimer(
    IN PNDIS_WIDGET_TIMER Timer,
    OUT PBOOLEAN TimerCancelled
    );
#if 0
#define NdisWCancelTimer(_Timer, _Cancelled) \
            (*(_Cancelled) = KeCancelTimer(&((((PNDIS_TIMER)(_Timer))->Timer)))

#endif
//
// Physical Mapping
//


//
// VOID
// NdisWStartBufferPhysicalMapping(
//     IN NDIS_HANDLE WidgetAdapterHandle,
//     IN PNDIS_BUFFER Buffer,
//     IN ULONG PhysicalMapRegister,
//     IN BOOLEAN WriteToDevice,
//     OUT PNDIS_PHYSICAL_ADDRESS_UNIT PhysicalAddressArray,
//     OUT PUINT ArraySize
//     );
//

#define NdisWStartBufferPhysicalMapping(                                        \
              WidgetAdapterHandle,                                              \
              Buffer,                                                           \
              PhysicalMapRegister,                                              \
              Write,                                                            \
              PhysicalAddressArray,                                             \
              ArraySize                                                         \
              )                                                                 \
{                                                                               \
    PNDIS_WIDGET_BLOCK _WidgetP = (PNDIS_WIDGET_BLOCK)(WidgetAdapterHandle);    \
    ULONG _PhysicalMapRegister = (PhysicalMapRegister);                         \
    PNDIS_PHYSICAL_ADDRESS_UNIT _PhysicalAddressArray = (PhysicalAddressArray); \
    PNDIS_BUFFER _Buffer = (Buffer);                                            \
    BOOLEAN _WriteToDevice = (Write);                                           \
    PUINT _ArraySize = (ArraySize);                                             \
    PHYSICAL_ADDRESS _LogicalAddress;                                           \
    PUCHAR _VirtualAddress;                                                     \
    ULONG _LengthRemaining;                                                     \
    ULONG _LengthMapped;                                                        \
    UINT _CurrentArrayLocation;                                                 \
    _VirtualAddress = MmGetMdlVirtualAddress(_Buffer);                          \
    _LengthRemaining = MmGetMdlByteCount(_Buffer);                              \
    _CurrentArrayLocation = 0;                                                  \
    while (_LengthRemaining > 0) {                                              \
        _LengthMapped = _LengthRemaining;                                       \
        _LogicalAddress = IoMapTransfer(                                        \
                             NULL,                                              \
                             _Buffer,                                           \
                             _WidgetP->MapRegisters[_PhysicalMapRegister].MapRegister, \
                             _VirtualAddress,                                   \
                             &_LengthMapped,                                    \
                             _WriteToDevice);                                   \
        _PhysicalAddressArray[_CurrentArrayLocation].PhysicalAddress = _LogicalAddress; \
        _PhysicalAddressArray[_CurrentArrayLocation].Length = _LengthMapped;    \
        _LengthRemaining -= _LengthMapped;                                      \
        _VirtualAddress += _LengthMapped;                                       \
        ++_CurrentArrayLocation;                                                \
    }                                                                           \
    _WidgetP->MapRegisters[_PhysicalMapRegister].WriteToDevice = _WriteToDevice; \
    *(_ArraySize) = _CurrentArrayLocation;                                      \
}

//
// VOID
// NdisWCompleteBufferPhysicalMapping(
//     IN NDIS_HANDLE WidgetAdapterHandle,
//     IN PNDIS_BUFFER Buffer,
//     IN ULONG PhysicalMapRegister
//     );
//

#define NdisWCompleteBufferPhysicalMapping( \
    WidgetAdapterHandle,                    \
    Buffer,                                 \
    PhysicalMapRegister                     \
    )                                       \
{                                           \
    PNDIS_WIDGET_BLOCK _WidgetP = (PNDIS_WIDGET_BLOCK)WidgetAdapterHandle;\
    IoFlushAdapterBuffers(                                                \
        NULL,                                                             \
        Buffer,                                                           \
        _WidgetP->MapRegisters[PhysicalMapRegister].MapRegister,          \
        MmGetMdlVirtualAddress(Buffer),                                   \
        MmGetMdlByteCount(Buffer),                                        \
        _WidgetP->MapRegisters[PhysicalMapRegister].WriteToDevice);       \
}


//
// Shared memory
//

#ifdef NDIS_MINI_DRIVER

VOID
NdisWAllocateSharedMemory(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    OUT PVOID *VirtualAddress,
    OUT PNDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

#else

/*++
VOID
NdisWAllocateSharedMemory(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    OUT PVOID *VirtualAddress,
    OUT PNDIS_PHYSICAL_ADDRESS PhysicalAddress
    )
--*/
#define NdisWAllocateSharedMemory(_H, _L, _C, _V, _P) \
        NdisAllocateSharedMemory(_H, _L, _C, _V, _P)

#endif

/*++
VOID
NdisWUpdateSharedMemory(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN ULONG Length,
    IN PVOID VirtualAddress,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    )
--*/
#define NdisWUpdateSharedMemory(_H, _L, _V, _P) NdisUpdateSharedMemory(_H, _L, _V, _P)


#ifdef NDIS_MINI_DRIVER

VOID
NdisWFreeSharedMemory(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    IN PVOID VirtualAddress,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

#else

/*++
VOID
NdisWFreeSharedMemory(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    IN PVOID VirtualAddress,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    )
--*/
#define NdisWFreeSharedMemory(_H, _L, _C, _V, _P) \
        NdisFreeSharedMemory(_H, _L, _C, _V, _P)

#endif


//
// DMA operations.
//

EXPORT
NDIS_STATUS
NdisWRegisterDmaChannel(
    OUT PNDIS_HANDLE WidgetDmaHandle,
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN UINT DmaChannel,
    IN BOOLEAN Dma32BitAddresses,
    IN PNDIS_DMA_DESCRIPTION DmaDescription,
    IN ULONG MaximumLength
    );

#ifdef NDIS_MINI_DRIVER

VOID
NdisWDeregisterDmaChannel(
    IN PNDIS_HANDLE WidgetDmaHandle
    );

#else

/*++
VOID
NdisWDeregisterDmaChannel(
    IN PNDIS_HANDLE WidgetDmaHandle
    )
--*/
#define NdisWDeregisterDmaChannel(_H) \
        NdisFreeDmaChannel(_H)

#endif

/*++
VOID
NdisWSetupDmaTransfer(
    OUT PNDIS_STATUS Status,
    IN PNDIS_HANDLE WidgetDmaHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG Offset,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    )
--*/
#define NdisWSetupDmaTransfer(_S, _H, _B, _O, _L, _W) \
        NdisSetupDmaTransfer(_S, _H, _B, _O, _L, _W)

/*++
VOID
NdisWCompleteDmaTransfer(
    OUT PNDIS_STATUS Status,
    IN PNDIS_HANDLE WidgetDmaHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG Offset,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    )
--*/
#define NdisWCompleteDmaTransfer(_S, _H, _B, _O, _L, _W) \
        NdisCompleteDmaTransfer(_S, _H, _B, _O, _L, _W)

ULONG
NdisWReadDmaCounter(
    IN NDIS_HANDLE WidgetDmaHandle
    );
#if 0
#define NdisWReadDmaCounter(_WidgetDmaHandle) \
    HalReadDmaCounter(((PNDIS_DMA_BLOCK)(_WidgetDmaHandle))->SystemAdapterObject)
#endif


//
// Requests Used by Widget Drivers
//

#ifdef NDIS_MINI_DRIVER

VOID
NdisWInitializeWrapper(
    OUT PNDIS_HANDLE NdisWrapperHandle,
    IN PVOID SystemSpecific1,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

#else

/*++
VOID
NdisWInitializeWrapper(
    OUT PNDIS_HANDLE NdisWrapperHandle,
    IN PVOID SystemSpecific1,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )
--*/
#define NdisWInitializeWrapper(_H, _S1, _S2, _S3) \
        NdisInitializeWrapper(_H, _S1, _S2, _S3)

#endif


EXPORT
NDIS_STATUS
NdisWRegisterWidget(
    IN NDIS_HANDLE NdisWrapperHandle,
    IN PNDIS_WIDGET_CHARACTERISTICS WidgetCharacteristics,
    IN UINT CharacteristicsLength
    );

EXPORT
VOID
NdisWSetAttributes(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_HANDLE WidgetAdapterContext,
    IN BOOLEAN BusMaster,
    IN NDIS_INTERFACE_TYPE AdapterType
    );

EXPORT
VOID
NdisWSendComplete(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status
    );

EXPORT
VOID
NdisWSendResourcesAvailable(
    IN NDIS_HANDLE WidgetAdapterHandle
    );

EXPORT
VOID
NdisWTransferDataComplete(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status,
    IN UINT BytesTransferred
    );

EXPORT
VOID
NdisWResetComplete(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_STATUS Status,
    IN BOOLEAN AddressingReset
    );

EXPORT
VOID
NdisWSetInformationComplete(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_STATUS Status
    );

EXPORT
VOID
NdisWQueryInformationComplete(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_STATUS Status
    );


/*++

VOID
NdisWEthIndicateReceive(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_HANDLE WidgetReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookaheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    )

--*/
#define NdisWEthIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ ) \
{                                                                  \
    PNDIS_WIDGET_BLOCK _W = (PNDIS_WIDGET_BLOCK)(_H);              \
    PUCHAR _HB = (_B);                                             \
    ASSERT(WIDGET_LOCK_ACQUIRED(_W));                              \
    _W->Timeout = FALSE;                                           \
    EthFilterDprIndicateReceive(                                   \
        (PETH_FILTER)_W->FilterDB,                                 \
        _C,                                                        \
        _HB,                                                       \
        _HB,                                                       \
        _SZ,                                                       \
        _L,                                                        \
        _LSZ,                                                      \
        _PSZ                                                       \
        );                                                         \
}

/*++

VOID
NdisWTrIndicateReceive(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_HANDLE WidgetReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookaheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    )

--*/
#define NdisWTrIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ )  \
{                                                                  \
    PNDIS_WIDGET_BLOCK _W = (PNDIS_WIDGET_BLOCK)(_H);              \
    PUCHAR _HB = (_B);                                             \
    ASSERT(WIDGET_LOCK_ACQUIRED(_W));                              \
    _W->Timeout = FALSE;                                           \
    TrFilterDprIndicateReceive(                                    \
        (PTR_FILTER)_W->FilterDB,                                  \
        _C,                                                        \
        _HB,                                                       \
        _HB,                                                       \
        _SZ,                                                       \
        _L,                                                        \
        _LSZ,                                                      \
        _PSZ                                                       \
        );                                                         \
}

/*++

VOID
NdisWFddiIndicateReceive(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_HANDLE WidgetReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookaheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    )

--*/
#define NdisWFddiIndicateReceive( _H, _C, _B, _SZ, _L, _LSZ, _PSZ ) \
{                                                                  \
    PNDIS_WIDGET_BLOCK _W = (PNDIS_WIDGET_BLOCK)(_H);              \
    PUCHAR _HB = (_B);                                             \
    UCHAR FCByte;                                                  \
    ASSERT(WIDGET_LOCK_ACQUIRED(_W));                              \
    _W->Timeout = FALSE;                                           \
    NdisReadRegisterUchar(_HB, &FCByte);                           \
    if (FCByte & 0x40) {                                           \
        FddiFilterDprIndicateReceive(                              \
            (PFDDI_FILTER)_W->FilterDB,                            \
            _C,                                                    \
            _HB,                                                   \
            FDDI_LENGTH_OF_LONG_ADDRESS,                           \
            _HB,                                                   \
            _SZ,                                                   \
            _L,                                                    \
            _LSZ,                                                  \
            _PSZ                                                   \
            );                                                     \
    } else {                                                       \
        FddiFilterDprIndicateReceive(                              \
            (PFDDI_FILTER)_W->FilterDB,                            \
            _C,                                                    \
            _HB,                                                   \
            FDDI_LENGTH_OF_SHORT_ADDRESS,                          \
            _HB,                                                   \
            _SZ,                                                   \
            _L,                                                    \
            _LSZ,                                                  \
            _PSZ                                                   \
            );                                                     \
    }                                                              \
}

/*++

VOID
NdisWEthIndicateReceiveComplete(
    IN NDIS_HANDLE WidgetAdapterHandle
    );

--*/

#define NdisWEthIndicateReceiveComplete( _H )                      \
{                                                                  \
    PNDIS_WIDGET_BLOCK _W = (PNDIS_WIDGET_BLOCK)_H;                \
    ASSERT(WIDGET_LOCK_ACQUIRED(_W));                              \
    EthFilterDprIndicateReceiveComplete((PETH_FILTER)_W->FilterDB);\
}

/*++

VOID
NdisWTrIndicateReceiveComplete(
    IN NDIS_HANDLE WidgetAdapterHandle
    );

--*/

#define NdisWTrIndicateReceiveComplete( _H )                       \
{                                                                  \
    PNDIS_WIDGET_BLOCK _W = (PNDIS_WIDGET_BLOCK)_H;                \
    ASSERT(WIDGET_LOCK_ACQUIRED(_W));                              \
    TrFilterDprIndicateReceiveComplete((PTR_FILTER)_W->FilterDB);  \
}

/*++

VOID
NdisWFddiIndicateReceiveComplete(
    IN NDIS_HANDLE WidgetAdapterHandle
    );

--*/

#define NdisWFddiIndicateReceiveComplete( _H )                     \
{                                                                  \
    PNDIS_WIDGET_BLOCK _W = (PNDIS_WIDGET_BLOCK)_H;                \
    ASSERT(WIDGET_LOCK_ACQUIRED(_W));                              \
    FddiFilterDprIndicateReceiveComplete((PFDDI_FILTER)_W->FilterDB); \
}

EXPORT
VOID
NdisWIndicateStatus(
    IN NDIS_HANDLE WidgetAdapterHandle,
    IN NDIS_STATUS GeneralStatus,
    IN PVOID StatusBuffer,
    IN UINT StatusBufferSize
    );

EXPORT
VOID
NdisWIndicateStatusComplete(
    IN NDIS_HANDLE WidgetAdapterHandle
    );

//
// Portability extentions
//
#ifndef NDIS_INIT_FUNCTION
#define NDIS_INIT_FUNCTION(_F) alloc_text(init,_F)
#endif
#define NDIS_PAGABLE_FUNCTION(_F)


EXPORT
VOID
NdisImmediateReadPortUchar(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG Port,
    OUT PUCHAR Data
    );

EXPORT
VOID
NdisImmediateReadPortUshort(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG Port,
    OUT PUSHORT Data
    );

EXPORT
VOID
NdisImmediateReadPortUlong(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG Port,
    OUT PULONG Data
    );

EXPORT
VOID
NdisImmediateWritePortUchar(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG Port,
    IN UCHAR Data
    );

EXPORT
VOID
NdisImmediateWritePortUshort(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG Port,
    IN USHORT Data
    );

EXPORT
VOID
NdisImmediateWritePortUlong(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG Port,
    IN ULONG Data
    );

EXPORT
ULONG
NdisImmediateReadPciSlotInformation(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG SlotNumber,
    IN ULONG Offset,
    IN PVOID Buffer,
    IN ULONG Length
    );

EXPORT
ULONG
NdisImmediateWritePciSlotInformation(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG SlotNumber,
    IN ULONG Offset,
    IN PVOID Buffer,
    IN ULONG Length
    );

typedef struct _USTR {
    USHORT Length;
    USHORT MaximumLength;
    PUSHORT Buffer;
} USTR;

VOID
NdisWReadConfiguration(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_CONFIGURATION_PARAMETER *ParameterValue,
    IN NDIS_HANDLE ConfigurationHandle,
    IN USTR *Parameter,
    IN NDIS_PARAMETER_TYPE ParameterType
    );



NDIS_STATUS
NdisWAllocateMemory(
    OUT PVOID *Buffer,
    IN UINT Length,
    IN UINT MemoryFlags,
    IN QINT MaximumPhysicalAddress OPTIONAL

    );
#endif  // _NDIS_WIDGET
