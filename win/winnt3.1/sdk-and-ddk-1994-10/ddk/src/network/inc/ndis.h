/*++ BUILD Version: 0002    // Increment this if a change has global effects

Copyright (c) 1992  Microsoft Corporation

Module Name:

  ndis.h

Abstract:

    Main header file for the wrapper

Author:

    Adam Barr (adamba)    26-Jul-1990
    Johnson R Apacible (JohnsonA)  10-Jul-1991

Revision History:

    26-Feb-1991     JohnsonA        Debug version of wrapper
    10-Jul-1991     JohnsonA        Implement revised NDIS 3.0 specification
    01-Nov-1991     SeanSe          Correct and Expand NDIS 3.1 specification

--*/


#ifndef _NDIS_
#define _NDIS_


#define NDIS_NT 1

#include <ntddk.h>
#include <netevent.h>


#ifndef _WINDEF_    // these are defined in windows.h too
typedef signed int INT, *PINT;
typedef unsigned int UINT, *PUINT;
#endif
typedef UNICODE_STRING NDIS_STRING, *PNDIS_STRING;


//
// This file contains the definition of an NDIS_OID as
// well as #defines for all the current OID values.
//

#include <ntddndis.h>


#ifdef NDIS_DOS
#undef NDIS_DOS
#endif

#ifdef IF_ERROR_CHK
#undef IF_ERROR_CHK
#endif

#ifdef NDISDBG
#ifdef MEMPRINT
#include "memprint.h"   //DavidTr's memprint program at ntos\srv
#endif // MEMPRINT

extern int      NdisMsgLevel;
extern BOOLEAN  NdisChkErrorFlag;
extern ULONG    PendingSends;

#define TRACE_NONE         0x0000
#define TRACE_IMPT         0x0001
#define TRACE_ALL          0x0002

#define IF_TRACE(level) if ( NdisMsgLevel >= (level) )   //for tracing

#define IF_ERROR_CHK  if (NdisChkErrorFlag)       // for parameter checking

#define DbgIsMdl(_Buffer)   \
    ( ((ULONG)(_Buffer)->Size) == MmSizeOfMdl(MmGetMdlVirtualAddress(_Buffer), \
                                              MmGetMdlByteCount(_Buffer)) )

#define DbgIsNonPaged(_Address) \
    ( MmIsNonPagedSystemAddressValid((PVOID)(_Address)) )

#define DbgIsPacket(_Packet) \
    ( ((_Packet)->Private.Pool->PacketLength) > sizeof(_Packet) )

#define DbgIsNull(_Ptr)  ( ((PVOID)(_Ptr)) == NULL )

#define NdisPrint1(fmt)                DbgPrint(fmt)
#define NdisPrint2(fmt,v1)             DbgPrint(fmt,v1)
#define NdisPrint3(fmt,v1,v2)          DbgPrint(fmt,v1,v2)
#define NdisPrint4(fmt,v1,v2,v3)       DbgPrint(fmt,v1,v2,v3)
#define NdisPrint5(fmt,v1,v2,v3,v4)    DbgPrint(fmt,v1,v2,v3,v4)

#else // NDISDBG
#define IF_TRACE(level)   if (FALSE)
#define IF_ERROR_CHK      if (FALSE)
#define DbgIsMdl(_Buffer)       TRUE
#define DbgIsNonPaged(_Address) TRUE
#define DbgIsPacket(_Packet)    TRUE
#define DbgIsNull(_Ptr)         FALSE

#define NdisPrint1(fmt)
#define NdisPrint2(fmt,v1)
#define NdisPrint3(fmt,v1,v2)
#define NdisPrint4(fmt,v1,v2,v3)
#define NdisPrint5(fmt,v1,v2,v3,v4)
#endif // NDISDBG

//
// Ndis defines for configuration manager data structures
//

typedef CM_MCA_POS_DATA NDIS_MCA_POS_DATA, *PNDIS_MCA_POS_DATA;
typedef CM_EISA_SLOT_INFORMATION NDIS_EISA_SLOT_INFORMATION;
typedef CM_EISA_SLOT_INFORMATION *PNDIS_EISA_SLOT_INFORMATION;
typedef CM_EISA_FUNCTION_INFORMATION  NDIS_EISA_FUNCTION_INFORMATION;
typedef CM_EISA_FUNCTION_INFORMATION *PNDIS_EISA_FUNCTION_INFORMATION;

//
//
// Ndis defines and data structures
//
//


//
// Ndis macros that are supplied to Device Drivers to help
// with portability.  They are not published in the NT spec
// as yet.  For now you should supply two versions of each
// macros - an NT specific one and a portable one.
//

#if NDIS_NT

//
// ZZZ These macros are peculiar to NT.
//

#define NdisMoveMemory(Destination,Source,Length) RtlCopyMemory(Destination,Source,Length)
#define NdisZeroMemory(Destination,Length) RtlZeroMemory(Destination,Length)
#define NdisRetrieveUlong(Destination,Source) RtlRetrieveUlong(Destination,Source)
#define NdisStoreUlong(Destination,Value) RtlStoreUlong(Destination,Value)
#define NDIS_STRING_CONST(x)   {sizeof(L##x)-2, sizeof(L##x), L##x}

//
// On a MIPS machine, I/O mapped memory can't be accessed with
// the Rtl routines.
//

#ifdef _M_IX86
#define NdisMoveMappedMemory(Destination,Source,Length) RtlCopyMemory(Destination,Source,Length)
#define NdisZeroMappedMemory(Destination,Length) RtlZeroMemory(Destination,Length)
#elif defined(_M_MRX000) || defined(_MIPS_)
#define NdisMoveMappedMemory(Destination,Source,Length) \
{ \
    PUCHAR _Src = (Source); \
    PUCHAR _Dest = (Destination); \
    PUCHAR _End = _Dest + (Length); \
    while (_Dest < _End) { \
        *_Dest++ = *_Src++; \
    } \
}
#define NdisZeroMappedMemory(Destination,Length) \
{ \
    PUCHAR _Dest = (Destination); \
    PUCHAR _End = _Dest + (Length); \
    while (_Dest < _End) { \
        *_Dest++ = 0; \
    } \
}
#elif defined(_ALPHA_)

#define NdisMoveMappedMemory(Destination,Source,Length) \
{ \
    PUCHAR _Src = (Source); \
    PUCHAR _Dest = (Destination); \
    PUCHAR _End = _Dest + (Length); \
    while (_Dest < _End) { \
        *_Dest++ = *_Src++; \
    } \
}
#define NdisZeroMappedMemory(Destination,Length) \
{ \
    PUCHAR _Dest = (Destination); \
    PUCHAR _End = _Dest + (Length); \
    while (_Dest < _End) { \
        NdisWriteRegisterUchar(_Dest,0); \
        _Dest++;        \
    } \
}

#endif


#else

//
// These macros MUST be portable.
//

#define NdisMoveMemory(Destination,Source,Length) memcpy(Destination,Source,Length)
#define NdisZeroMemory(Destination,Length) memset(Destination,'\0',Length)
#define NdisMoveMappedMemory(Destination,Source,Length) memcpy(Destination,Source,Length)
#define NdisZeroMappedMemory(Destination,Length) memset(Destination,'\0',Length)
#define NdisRetrieveUlong(Destination,Source)\
   {\
       UCHAR _S = 0;\
       for (; _S < sizeof(ULONG) ; _S++ ) {\
           ((PUCHAR)Destination)[_S] = ((PUCHAR)Source)[_S];\
       }\
   }
#define NdisStoreUlong(Destination,Value)\
   {\
       UCHAR _S = 0;\
       for (; _S < sizeof(ULONG) ; _S++ ) {\
           ((PUCHAR)Destination)[_S] = ((UCHAR)(Value >> (_S * 8)));\
       }\
   }


#endif


//
// On Mips and Intel systems, these are the same. On Alpha, they are different.
//

#ifdef _ALPHA_

#define NdisMoveToMappedMemory(Destination,Source,Length) WRITE_REGISTER_BUFFER_UCHAR(Destination,Source,Length)
#define NdisMoveFromMappedMemory(Destination,Source,Length) READ_REGISTER_BUFFER_UCHAR(Source,Destination,Length)

#else

#define NdisMoveToMappedMemory(Destination,Source,Length) NdisMoveMappedMemory(Destination,Source,Length)
#define NdisMoveFromMappedMemory(Destination,Source,Length) NdisMoveMappedMemory(Destination,Source,Length)

#endif



//
// This macro is used to determine how many physical pieces
// an NDIS_BUFFER will take up when mapped.
//

#define NDIS_BUFFER_TO_SPAN_PAGES(_Buffer) \
    (ADDRESS_AND_SIZE_TO_SPAN_PAGES(\
         MmGetMdlVirtualAddress(_Buffer), \
         MmGetMdlByteCount(_Buffer)))


//
// definition of the basic spin lock structure
//

typedef struct _NDIS_SPIN_LOCK {
    KSPIN_LOCK SpinLock;
    KIRQL OldIrql;
} NDIS_SPIN_LOCK, * PNDIS_SPIN_LOCK;


typedef PVOID NDIS_HANDLE, *PNDIS_HANDLE;

typedef int NDIS_STATUS, *PNDIS_STATUS; // note default size

typedef struct _NDIS_TIMER {
    KTIMER Timer;
    KDPC Dpc;
} NDIS_TIMER, *PNDIS_TIMER;


#define NdisInterruptLatched Latched
#define NdisInterruptLevelSensitive LevelSensitive
typedef KINTERRUPT_MODE NDIS_INTERRUPT_MODE, *PNDIS_INTERRUPT_MODE;


typedef
BOOLEAN
(*PNDIS_INTERRUPT_SERVICE) (
    IN PVOID InterruptContext
    );

typedef
VOID
(*PNDIS_DEFERRED_PROCESSING) (
    IN PVOID SystemSpecific1,
    IN PVOID InterruptContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );


typedef struct _NDIS_INTERRUPT {
    PKINTERRUPT InterruptObject;
    KSPIN_LOCK DpcCountLock;
    PNDIS_INTERRUPT_SERVICE MacIsr;     // Pointer to Mac ISR routine
    PNDIS_DEFERRED_PROCESSING MacDpc;   // Pointer to Mac DPC routine
    KDPC InterruptDpc;
    PVOID InterruptContext;             // Pointer to context for calling
                                        // adapters ISR and DPC.
    UCHAR DpcCount;
    BOOLEAN Removing;                   // TRUE if removing interrupt

    //
    // This is used to tell when all the Dpcs for the adapter are completed.
    //
    KEVENT DpcsCompletedEvent;

} NDIS_INTERRUPT, *PNDIS_INTERRUPT;

struct _NDIS_PACKET;


//
// Configuration definitions
//

//
// Possible data types
//
typedef enum _NDIS_PARAMETER_TYPE {
    NdisParameterInteger,
    NdisParameterHexInteger,
    NdisParameterString,
    NdisParameterMultiString
} NDIS_PARAMETER_TYPE, *PNDIS_PARAMETER_TYPE;

//
// To store configuration information
//
typedef struct _NDIS_CONFIGURATION_PARAMETER {
    NDIS_PARAMETER_TYPE ParameterType;
    union {
        ULONG IntegerData;
        NDIS_STRING StringData;
    } ParameterData;
} NDIS_CONFIGURATION_PARAMETER, *PNDIS_CONFIGURATION_PARAMETER;


//
// Definitions for the "ProcessorType" keyword
//
typedef enum _NDIS_PROCESSOR_TYPE {
    NdisProcessorX86,
    NdisProcessorMips,
    NdisProcessorAlpha
} NDIS_PROCESSOR_TYPE, *PNDIS_PROCESSOR_TYPE;

//
// Definitions for the "Environment" keyword
//
typedef enum _NDIS_ENVIRONMENT_TYPE {
    NdisEnvironmentWindows,
    NdisEnvironmentWindowsNt
} NDIS_ENVIRONMENT_TYPE, *PNDIS_ENVIRONMENT_TYPE;


//
// Possible Hardware Architecture. Define these to
// match the HAL INTERFACE_TYPE enum.
//
typedef enum _NDIS_INTERFACE_TYPE {
    NdisInterfaceInternal = Internal,
    NdisInterfaceIsa = Isa,
    NdisInterfaceEisa = Eisa,
    NdisInterfaceMca = MicroChannel,
    NdisInterfaceTurboChannel = TurboChannel
} NDIS_INTERFACE_TYPE, *PNDIS_INTERFACE_TYPE;


//
// The structure passed up on a WAN_LINE_UP indication
//

typedef struct _NDIS_WAN_LINE_UP {
    ULONG LinkSpeed;                // 100 bps units
    ULONG MaximumTotalSize;         // suggested max for send packets
    NDIS_WAN_QUALITY Quality;
    USHORT SendWindow;              // suggested by the MAC
    UCHAR Address[1];               // variable length, depends on address type
} NDIS_WAN_LINE_UP, *PNDIS_WAN_LINE_UP;

//
// The structure passed up on a WAN_LINE_DOWN indication
//

typedef struct _NDIS_WAN_LINE_DOWN {
    UCHAR Address[1];               // variable length, depends on address type
} NDIS_WAN_LINE_DOWN, *PNDIS_WAN_LINE_DOWN;

//
// The structure passed up on a WAN_FRAGMENT indication
//

typedef struct _NDIS_WAN_FRAGMENT {
    UCHAR Address[1];               // variable length, depends on address type
} NDIS_WAN_FRAGMENT, *PNDIS_WAN_FRAGMENT;


//
// Ndis Adapter Information
//

typedef
NDIS_STATUS
(*PNDIS_ACTIVATE_CALLBACK) (
    IN NDIS_HANDLE NdisAdatperHandle,
    IN NDIS_HANDLE MacAdapterContext,
    IN ULONG DmaChannel
    );

typedef struct _NDIS_PORT_DESCRIPTOR {
    ULONG InitialPort;
    ULONG NumberOfPorts;
    PVOID *PortOffset;
} NDIS_PORT_DESCRIPTOR, *PNDIS_PORT_DESCRIPTOR;

typedef struct _NDIS_ADAPTER_INFORMATION {
    ULONG DmaChannel;
    BOOLEAN Master;
    BOOLEAN Dma32BitAddresses;
    PNDIS_ACTIVATE_CALLBACK ActivateCallback;
    NDIS_INTERFACE_TYPE AdapterType;
    ULONG PhysicalMapRegistersNeeded;
    ULONG MaximumPhysicalMapping;
    ULONG NumberOfPortDescriptors;
    NDIS_PORT_DESCRIPTOR PortDescriptors[1];   // as many as needed
} NDIS_ADAPTER_INFORMATION, *PNDIS_ADAPTER_INFORMATION;

//
// DMA Channel information
//
typedef struct _NDIS_DMA_DESCRIPTION {
    BOOLEAN DemandMode;
    BOOLEAN AutoInitialize;
    BOOLEAN DmaChannelSpecified;
    DMA_WIDTH DmaWidth;
    DMA_SPEED DmaSpeed;
    ULONG DmaPort;
    ULONG DmaChannel;
} NDIS_DMA_DESCRIPTION, *PNDIS_DMA_DESCRIPTION;

//
// Internal structure representing an NDIS DMA channel
//
typedef struct _NDIS_DMA_BLOCK {
    PVOID MapRegisterBase;
    KEVENT AllocationEvent;
    PADAPTER_OBJECT SystemAdapterObject;
    BOOLEAN InProgress;
} NDIS_DMA_BLOCK, *PNDIS_DMA_BLOCK;


//
// Ndis Buffer is actually an Mdl
//
typedef MDL NDIS_BUFFER, * PNDIS_BUFFER;

//
// packet pool definition
//
typedef struct _NDIS_PACKET_POOL {
    NDIS_SPIN_LOCK SpinLock;
    struct _NDIS_PACKET *FreeList;  // linked list of free slots in pool
    UINT PacketLength;      // amount needed in each packet
    UCHAR Buffer[1];        // actual pool memory
} NDIS_PACKET_POOL, * PNDIS_PACKET_POOL;


//
// wrapper-specific part of a packet
//

typedef struct _NDIS_PACKET_PRIVATE {
    UINT PhysicalCount;     // number of physical pages in packet.
    UINT TotalLength;       // Total amount of data in the packet.
    PNDIS_BUFFER Head;      // first buffer in the chain
    PNDIS_BUFFER Tail;      // last buffer in the chain

    // if Head is NULL the chain is empty; Tail doesn't have to be NULL also

    PNDIS_PACKET_POOL Pool; // so we know where to free it back to
    UINT Count;
    ULONG Flags;
    BOOLEAN ValidCounts;
} NDIS_PACKET_PRIVATE, * PNDIS_PACKET_PRIVATE;


//
// packet definition
//

typedef struct _NDIS_PACKET {
    NDIS_PACKET_PRIVATE Private;
    UCHAR MacReserved[16];
    UCHAR ProtocolReserved[1];
} NDIS_PACKET, * PNDIS_PACKET;


//
// Request types used by NdisRequest; constants are added for
// all entry points in the MAC, for those that want to create
// their own internal requests.
//

typedef enum _NDIS_REQUEST_TYPE {
    NdisRequestQueryInformation,
    NdisRequestSetInformation,
    NdisRequestQueryStatistics,
    NdisRequestOpen,
    NdisRequestClose,
    NdisRequestSend,
    NdisRequestTransferData,
    NdisRequestReset,
    NdisRequestGeneric1,
    NdisRequestGeneric2,
    NdisRequestGeneric3,
    NdisRequestGeneric4
} NDIS_REQUEST_TYPE, *PNDIS_REQUEST_TYPE;


//
// Structure of requests sent via NdisRequest
//

typedef struct _NDIS_REQUEST {
    UCHAR MacReserved[16];
    NDIS_REQUEST_TYPE RequestType;
    union _DATA {

        struct _QUERY_INFORMATION {
            NDIS_OID Oid;
            PVOID InformationBuffer;
            UINT InformationBufferLength;
            UINT BytesWritten;
            UINT BytesNeeded;
        } QUERY_INFORMATION;

        struct _SET_INFORMATION {
            NDIS_OID Oid;
            PVOID InformationBuffer;
            UINT InformationBufferLength;
            UINT BytesRead;
            UINT BytesNeeded;
        } SET_INFORMATION;

    } DATA;

} NDIS_REQUEST, *PNDIS_REQUEST;


//
// Definitions for physical address.
//

typedef PHYSICAL_ADDRESS NDIS_PHYSICAL_ADDRESS, *PNDIS_PHYSICAL_ADDRESS;
typedef struct _NDIS_PHYSICAL_ADDRESS_UNIT {
    NDIS_PHYSICAL_ADDRESS PhysicalAddress;
    UINT Length;
} NDIS_PHYSICAL_ADDRESS_UNIT, *PNDIS_PHYSICAL_ADDRESS_UNIT;


/*++

ULONG
NdisGetPhysicalAddressHigh(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

--*/

#define NdisGetPhysicalAddressHigh(_PhysicalAddress)\
        ((_PhysicalAddress).HighPart)


/*++

VOID
NdisSetPhysicalAddressHigh(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG Value
    );

--*/

#define NdisSetPhysicalAddressHigh(_PhysicalAddress, _Value)\
     ((_PhysicalAddress).HighPart) = (_Value)


/*++

ULONG
NdisGetPhysicalAddressLow(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

--*/

#define NdisGetPhysicalAddressLow(_PhysicalAddress) \
    ((_PhysicalAddress).LowPart)


/*++

VOID
NdisSetPhysicalAddressLow(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG Value
    );

--*/

#define NdisSetPhysicalAddressLow(_PhysicalAddress, _Value) \
    ((_PhysicalAddress).LowPart) = (_Value)


//
// Macro to initialize an NDIS_PHYSICAL_ADDRESS constant
//

#define NDIS_PHYSICAL_ADDRESS_CONST(_Low, _High) \
    { (ULONG)(_Low), (LONG)(_High) }


//
// Include an incomplete type for NDIS_PACKET structure so that
// function types can refer to a type to be defined later.
//
struct _NDIS_PACKET;

//
// Function types for NDIS_PROTOCOL_CHARACTERISTICS
//
//

typedef
VOID
(*OPEN_ADAPTER_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_STATUS Status,
    IN NDIS_STATUS OpenErrorStatus
    );

typedef
VOID
(*CLOSE_ADAPTER_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_STATUS Status
    );

typedef
VOID
(*SEND_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status
    );

typedef
VOID
(*TRANSFER_DATA_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status,
    IN UINT BytesTransferred
    );

typedef
VOID
(*RESET_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_STATUS Status
    );

typedef
VOID
(*REQUEST_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    );

typedef
NDIS_STATUS
(*RECEIVE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_HANDLE MacReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookAheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    );

typedef
VOID
(*RECEIVE_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext
    );

typedef
VOID
(*STATUS_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext,
    IN NDIS_STATUS GeneralStatus,
    IN PVOID StatusBuffer,
    IN UINT StatusBufferSize
    );

typedef
VOID
(*STATUS_COMPLETE_HANDLER) (
    IN NDIS_HANDLE ProtocolBindingContext
    );


typedef struct _NDIS_PROTOCOL_CHARACTERISTICS {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;
    UINT Reserved;
    OPEN_ADAPTER_COMPLETE_HANDLER OpenAdapterCompleteHandler;
    CLOSE_ADAPTER_COMPLETE_HANDLER CloseAdapterCompleteHandler;
    SEND_COMPLETE_HANDLER SendCompleteHandler;
    TRANSFER_DATA_COMPLETE_HANDLER TransferDataCompleteHandler;
    RESET_COMPLETE_HANDLER ResetCompleteHandler;
    REQUEST_COMPLETE_HANDLER RequestCompleteHandler;
    RECEIVE_HANDLER ReceiveHandler;
    RECEIVE_COMPLETE_HANDLER ReceiveCompleteHandler;
    STATUS_HANDLER StatusHandler;
    STATUS_COMPLETE_HANDLER StatusCompleteHandler;
    NDIS_STRING Name;
} NDIS_PROTOCOL_CHARACTERISTICS, *PNDIS_PROTOCOL_CHARACTERISTICS;


typedef
VOID
(*PNDIS_SYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext
    );

//
// Function types for NDIS_MAC_CHARACTERISTICS
//


typedef
NDIS_STATUS
(*OPEN_ADAPTER_HANDLER) (
    OUT PNDIS_STATUS OpenErrorStatus,
    OUT NDIS_HANDLE *MacBindingHandle,
    OUT PUINT SelectedMediumIndex,
    IN PNDIS_MEDIUM MediumArray,
    IN UINT MediumArraySize,
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacAdapterContext,
    IN UINT OpenOptions,
    IN PSTRING AddressingInformation OPTIONAL
    );

typedef
NDIS_STATUS
(*CLOSE_ADAPTER_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle
    );

typedef
NDIS_STATUS
(*SEND_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );

typedef
NDIS_STATUS
(*TRANSFER_DATA_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );

typedef
NDIS_STATUS
(*RESET_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle
    );

typedef
NDIS_STATUS
(*REQUEST_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );

typedef
NDIS_STATUS
(*QUERY_GLOBAL_STATISTICS_HANDLER) (
    IN NDIS_HANDLE MacAdapterContext,
    IN PNDIS_REQUEST NdisRequest
    );

typedef
VOID
(*UNLOAD_MAC_HANDLER) (
    IN NDIS_HANDLE MacMacContext
    );

typedef
NDIS_STATUS
(*ADD_ADAPTER_HANDLER) (
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN PNDIS_STRING AdapterName
    );

typedef
VOID
(*REMOVE_ADAPTER_HANDLER) (
    IN NDIS_HANDLE MacAdapterContext
    );


typedef struct _NDIS_MAC_CHARACTERISTICS {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;
    UINT Reserved;
    OPEN_ADAPTER_HANDLER OpenAdapterHandler;
    CLOSE_ADAPTER_HANDLER CloseAdapterHandler;
    SEND_HANDLER SendHandler;
    TRANSFER_DATA_HANDLER TransferDataHandler;
    RESET_HANDLER ResetHandler;
    REQUEST_HANDLER RequestHandler;
    QUERY_GLOBAL_STATISTICS_HANDLER QueryGlobalStatisticsHandler;
    UNLOAD_MAC_HANDLER UnloadMacHandler;
    ADD_ADAPTER_HANDLER AddAdapterHandler;
    REMOVE_ADAPTER_HANDLER RemoveAdapterHandler;
    NDIS_STRING Name;
} NDIS_MAC_CHARACTERISTICS, *PNDIS_MAC_CHARACTERISTICS;


//
// Definition for shutdown handler
//

typedef
VOID
(*ADAPTER_SHUTDOWN_HANDLER) (
    IN PVOID ShutdownContext
    );



//
// block used for references...
//

typedef struct _REFERENCE {
    NDIS_SPIN_LOCK SpinLock;
    USHORT ReferenceCount;
    BOOLEAN Closing;
} REFERENCE, * PREFERENCE;


//
// This holds a map register entry.
//

typedef struct _MAP_REGISTER_ENTRY {
    PVOID MapRegister;
    BOOLEAN WriteToDevice;
} MAP_REGISTER_ENTRY, * PMAP_REGISTER_ENTRY;

//
// declare these first since they point to each other
//

typedef struct _NDIS_WRAPPER_HANDLE NDIS_WRAPPER_HANDLE, * PNDIS_WRAPPER_HANDLE;
typedef struct _NDIS_MAC_BLOCK      NDIS_MAC_BLOCK, * PNDIS_MAC_BLOCK;
typedef struct _NDIS_ADAPTER_BLOCK  NDIS_ADAPTER_BLOCK, * PNDIS_ADAPTER_BLOCK;
typedef struct _NDIS_PROTOCOL_BLOCK NDIS_PROTOCOL_BLOCK, * PNDIS_PROTOCOL_BLOCK;
typedef struct _NDIS_OPEN_BLOCK     NDIS_OPEN_BLOCK, * PNDIS_OPEN_BLOCK;


//
// MAC specific considerations.
//

struct _NDIS_WRAPPER_HANDLE {

    //
    // These store the PDRIVER_OBJECT that
    // the MAC passes to NdisInitializeWrapper until it can be
    // used by NdisRegisterMac and NdisTerminateWrapper.
    //

    PDRIVER_OBJECT NdisWrapperDriver;

    HANDLE NdisWrapperConfigurationHandle;

};




//
// one of these per MAC
//

struct _NDIS_MAC_BLOCK {
    PNDIS_ADAPTER_BLOCK AdapterQueue;   // queue of adapters for this MAC
    NDIS_HANDLE MacMacContext;          // Context for calling MACUnload and
                                        //    MACAddAdapter.

    REFERENCE Ref;                      // contains spinlock for AdapterQueue
    UINT Length;                        // of this NDIS_MAC_BLOCK structure
    NDIS_MAC_CHARACTERISTICS MacCharacteristics;    // handler addresses
    PNDIS_WRAPPER_HANDLE NdisMacInfo;   // Mac information.
    PNDIS_MAC_BLOCK NextMac;
    KEVENT AdaptersRemovedEvent;        // used to find when all adapters are gone.
    BOOLEAN Unloading;                  // TRUE if unloading

};

//
// one of these per adapter registered on a MAC
//

struct _NDIS_ADAPTER_BLOCK {
    PDEVICE_OBJECT DeviceObject;        // created by NdisRegisterAdapter
    PNDIS_MAC_BLOCK MacHandle;          // pointer to our MAC block
    NDIS_HANDLE MacAdapterContext;      // context when calling MacOpenAdapter
    NDIS_STRING AdapterName;            // how NdisOpenAdapter refers to us
    PNDIS_OPEN_BLOCK OpenQueue;         // queue of opens for this adapter
    PNDIS_ADAPTER_BLOCK NextAdapter;    // used by MAC's AdapterQueue
    REFERENCE Ref;                      // contains spinlock for OpenQueue
    BOOLEAN BeingRemoved;               // TRUE if adapter is being removed

    //
    // Resource information
    //
    PCM_RESOURCE_LIST Resources;

    //
    // Handling of shutdown
    //
    ADAPTER_SHUTDOWN_HANDLER ShutdownRoutine;
    PVOID ShutdownContext;

    //
    // contains adapter information
    //
    ULONG BusNumber;
    NDIS_INTERFACE_TYPE BusType;
    ULONG ChannelNumber;
    NDIS_INTERFACE_TYPE AdapterType;
    BOOLEAN Master;
    ULONG PhysicalMapRegistersNeeded;
    ULONG MaximumPhysicalMapping;
    ULONG InitialPort;
    ULONG NumberOfPorts;

    //
    // Holds the mapping for ports, if needed.
    //
    PUCHAR InitialPortMapping;

    //
    // TRUE if InitialPortMapping was mapped with NdisMapIoSpace.
    //
    BOOLEAN InitialPortMapped;

    //
    // This is the offset added to the port passed to NdisXXXPort to
    // get to the real value to be passed to the NDIS_XXX_PORT macros.
    // It equals InitialPortMapping - InitialPort; that is, the
    // mapped "address" of port 0, even if we didn't actually
    // map port 0.
    //
    PUCHAR PortOffset;

    //
    // Holds the map registers for this adapter.
    //
    PMAP_REGISTER_ENTRY MapRegisters;

    //
    // These two are used temporarily while allocating
    // the map registers.
    //
    KEVENT AllocationEvent;
    UINT CurrentMapRegister;
    PADAPTER_OBJECT SystemAdapterObject;

};

//
// one of these per protocol registered
//

struct _NDIS_PROTOCOL_BLOCK {
    PNDIS_OPEN_BLOCK OpenQueue;         // queue of opens for this protocol
    REFERENCE Ref;                      // contains spinlock for OpenQueue
    UINT Length;                        // of this NDIS_PROTOCOL_BLOCK struct
    NDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics;  // handler addresses
};

//
// one of these per open on an adapter/protocol
//

struct _NDIS_OPEN_BLOCK {
    PNDIS_MAC_BLOCK MacHandle;              // pointer to our MAC
    NDIS_HANDLE MacBindingHandle;           // context when calling MacXX funcs
    PNDIS_ADAPTER_BLOCK AdapterHandle;      // pointer to our adapter
    PNDIS_PROTOCOL_BLOCK ProtocolHandle;    // pointer to our protocol
    NDIS_HANDLE ProtocolBindingContext;     // context when calling ProtXX funcs
    PNDIS_OPEN_BLOCK AdapterNextOpen;       // used by adapter's OpenQueue
    PNDIS_OPEN_BLOCK ProtocolNextOpen;      // used by protocol's OpenQueue
    PFILE_OBJECT FileObject;                // created by operating system
    BOOLEAN Closing;                        // TRUE when removing this struct
    NDIS_HANDLE CloseRequestHandle;         // 0 indicates an internal close
    NDIS_SPIN_LOCK SpinLock;                // guards Closing

    //
    // These are optimizations for getting to MAC routines.  They are not
    // necessary, but are here to save a dereference through the MAC block.
    //

    SEND_HANDLER SendHandler;
    TRANSFER_DATA_HANDLER TransferDataHandler;

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
// Types of Memory (not mutually exclusive)
//

#define NDIS_MEMORY_CONTIGUOUS              0x00000001
#define NDIS_MEMORY_NONCACHED               0x00000002

//
// Open options
//
#define NDIS_OPEN_RECEIVE_NOT_REENTRANT     0x00000001

//
// NDIS_STATUS values
//

#define NDIS_STATUS_SUCCESS                 ((NDIS_STATUS) STATUS_SUCCESS)
#define NDIS_STATUS_PENDING                 ((NDIS_STATUS) STATUS_PENDING)
#define NDIS_STATUS_NOT_RECOGNIZED          ((NDIS_STATUS)0x00010001L)
#define NDIS_STATUS_NOT_COPIED              ((NDIS_STATUS)0x00010002L)

#define NDIS_STATUS_ONLINE                  ((NDIS_STATUS)0x40010003L)
#define NDIS_STATUS_RESET_START             ((NDIS_STATUS)0x40010004L)
#define NDIS_STATUS_RESET_END               ((NDIS_STATUS)0x40010005L)
#define NDIS_STATUS_RING_STATUS             ((NDIS_STATUS)0x40010006L)
#define NDIS_STATUS_CLOSED                  ((NDIS_STATUS)0x40010007L)
#define NDIS_STATUS_WAN_LINE_UP             ((NDIS_STATUS)0x40010008L)
#define NDIS_STATUS_WAN_LINE_DOWN           ((NDIS_STATUS)0x40010009L)
#define NDIS_STATUS_WAN_FRAGMENT            ((NDIS_STATUS)0x4001000AL)

#define NDIS_STATUS_NOT_RESETTABLE          ((NDIS_STATUS)0x80010001L)
#define NDIS_STATUS_SOFT_ERRORS             ((NDIS_STATUS)0x80010003L)
#define NDIS_STATUS_HARD_ERRORS             ((NDIS_STATUS)0x80010004L)

#define NDIS_STATUS_FAILURE                 ((NDIS_STATUS) STATUS_UNSUCCESSFUL)
#define NDIS_STATUS_RESOURCES               ((NDIS_STATUS) \
                                                STATUS_INSUFFICIENT_RESOURCES)
#define NDIS_STATUS_CLOSING                 ((NDIS_STATUS)0xC0010002L)
#define NDIS_STATUS_BAD_VERSION             ((NDIS_STATUS)0xC0010004L)
#define NDIS_STATUS_BAD_CHARACTERISTICS     ((NDIS_STATUS)0xC0010005L)
#define NDIS_STATUS_ADAPTER_NOT_FOUND       ((NDIS_STATUS)0xC0010006L)
#define NDIS_STATUS_OPEN_FAILED             ((NDIS_STATUS)0xC0010007L)
#define NDIS_STATUS_DEVICE_FAILED           ((NDIS_STATUS)0xC0010008L)
#define NDIS_STATUS_MULTICAST_FULL          ((NDIS_STATUS)0xC0010009L)
#define NDIS_STATUS_MULTICAST_EXISTS        ((NDIS_STATUS)0xC001000AL)
#define NDIS_STATUS_MULTICAST_NOT_FOUND     ((NDIS_STATUS)0xC001000BL)
#define NDIS_STATUS_REQUEST_ABORTED         ((NDIS_STATUS)0xC001000CL)
#define NDIS_STATUS_RESET_IN_PROGRESS       ((NDIS_STATUS)0xC001000DL)
#define NDIS_STATUS_CLOSING_INDICATING      ((NDIS_STATUS)0xC001000EL)
#define NDIS_STATUS_NOT_SUPPORTED           ((NDIS_STATUS)STATUS_NOT_SUPPORTED)
#define NDIS_STATUS_INVALID_PACKET          ((NDIS_STATUS)0xC001000FL)
#define NDIS_STATUS_OPEN_LIST_FULL          ((NDIS_STATUS)0xC0010010L)
#define NDIS_STATUS_ADAPTER_NOT_READY       ((NDIS_STATUS)0xC0010011L)
#define NDIS_STATUS_ADAPTER_NOT_OPEN        ((NDIS_STATUS)0xC0010012L)
#define NDIS_STATUS_NOT_INDICATING          ((NDIS_STATUS)0xC0010013L)
#define NDIS_STATUS_INVALID_LENGTH          ((NDIS_STATUS)0xC0010014L)
#define NDIS_STATUS_INVALID_DATA            ((NDIS_STATUS)0xC0010015L)
#define NDIS_STATUS_BUFFER_TOO_SHORT        ((NDIS_STATUS)0xC0010016L)
#define NDIS_STATUS_INVALID_OID             ((NDIS_STATUS)0xC0010017L)
#define NDIS_STATUS_ADAPTER_REMOVED         ((NDIS_STATUS)0xC0010018L)
#define NDIS_STATUS_UNSUPPORTED_MEDIA       ((NDIS_STATUS)0xC0010019L)
#define NDIS_STATUS_GROUP_ADDRESS_IN_USE    ((NDIS_STATUS)0xC001001AL)
#define NDIS_STATUS_FILE_NOT_FOUND          ((NDIS_STATUS)0xC001001BL)
#define NDIS_STATUS_ERROR_READING_FILE      ((NDIS_STATUS)0xC001001CL)
#define NDIS_STATUS_ALREADY_MAPPED          ((NDIS_STATUS)0xC001001DL)
#define NDIS_STATUS_RESOURCE_CONFLICT       ((NDIS_STATUS)0xC001001EL)

#define NDIS_STATUS_TOKEN_RING_OPEN_ERROR   ((NDIS_STATUS)0xC0011000L)


//
// used in error logging
//

#define NDIS_ERROR_CODE ULONG

#define NDIS_ERROR_CODE_RESOURCE_CONFLICT          EVENT_NDIS_RESOURCE_CONFLICT
#define NDIS_ERROR_CODE_OUT_OF_RESOURCES           EVENT_NDIS_OUT_OF_RESOURCE
#define NDIS_ERROR_CODE_HARDWARE_FAILURE           EVENT_NDIS_HARDWARE_FAILURE
#define NDIS_ERROR_CODE_ADAPTER_NOT_FOUND          EVENT_NDIS_ADAPTER_NOT_FOUND
#define NDIS_ERROR_CODE_INTERRUPT_CONNECT          EVENT_NDIS_INTERRUPT_CONNECT
#define NDIS_ERROR_CODE_DRIVER_FAILURE             EVENT_NDIS_DRIVER_FAILURE
#define NDIS_ERROR_CODE_BAD_VERSION                EVENT_NDIS_BAD_VERSION
#define NDIS_ERROR_CODE_TIMEOUT                    EVENT_NDIS_TIMEOUT
#define NDIS_ERROR_CODE_NETWORK_ADDRESS            EVENT_NDIS_NETWORK_ADDRESS
#define NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION  EVENT_NDIS_UNSUPPORTED_CONFIGURATION
#define NDIS_ERROR_CODE_INVALID_VALUE_FROM_ADAPTER EVENT_NDIS_INVALID_VALUE_FROM_ADAPTER
#define NDIS_ERROR_CODE_MISSING_CONFIGURATION_PARAMETER  EVENT_NDIS_MISSING_CONFIGURATION_PARAMETER
#define NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS        EVENT_NDIS_BAD_IO_BASE_ADDRESS
#define NDIS_ERROR_CODE_RECEIVE_SPACE_SMALL        EVENT_NDIS_RECEIVE_SPACE_SMALL
#define NDIS_ERROR_CODE_ADAPTER_DISABLED           EVENT_NDIS_ADAPTER_DISABLED



#define INTERNAL



//
// Ndis Spin Locks
//

/*++

VOID
NdisAllocateSpinLock(
    IN PNDIS_SPIN_LOCK SpinLock;
    );

--*/


#ifdef NDISDBG
#define NdisAllocateSpinLock(_SpinLock) { \
    IF_TRACE(TRACE_ALL) {  \
        NdisPrint1("==>NdisAllocateSpinLock\n"); \
    } \
    KeInitializeSpinLock(&(_SpinLock)->SpinLock); \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("<==NdisAllocateSpinLock\n"); \
    } \
}
#else
#define NdisAllocateSpinLock(_SpinLock) { \
    KeInitializeSpinLock(&(_SpinLock)->SpinLock); \
}
#endif

/*++

VOID
NdisFreeSpinLock(
    IN PNDIS_SPIN_LOCK SpinLock;
    );

--*/

#ifdef NDISDBG
#define NdisFreeSpinLock(_SpinLock) { \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("==>NdisFreeSpinLock\n"); \
    } \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("<==NdisFreeSpinLock\n");   \
    } \
}
#else
#define NdisFreeSpinLock(_SpinLock) { \
}
#endif


/*++

VOID
NdisAcquireSpinLock(
    IN PNDIS_SPIN_LOCK SpinLock;
    );

--*/

#ifdef NDISDBG
#define NdisAcquireSpinLock(_SpinLock) { \
    KIRQL OldIrql; \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("==>NdisAcquireSpinLock\n"); \
    } \
    KeAcquireSpinLock(&(_SpinLock)->SpinLock, &OldIrql); \
    (_SpinLock)->OldIrql = OldIrql; \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("<==NdisAcquireSpinLock\n"); \
    } \
}
#else
#define NdisAcquireSpinLock(_SpinLock) { \
    KeAcquireSpinLock(&(_SpinLock)->SpinLock, &(_SpinLock)->OldIrql); \
}
#endif


/*++

VOID
NdisReleaseSpinLock(
    IN PNDIS_SPIN_LOCK SpinLock;
    );

--*/

#ifdef NDISDBG
#define NdisReleaseSpinLock(_SpinLock) { \
    KIRQL CurrentIrql = (_SpinLock)->OldIrql; \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("==>NdisReleaseSpinLock \n"); \
    } \
    KeReleaseSpinLock(&(_SpinLock)->SpinLock, CurrentIrql); \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("<==NdisReleaseSpinLock\n"); \
    }\
}
#else
#define NdisReleaseSpinLock(_SpinLock) { \
    KeReleaseSpinLock(&(_SpinLock)->SpinLock,(_SpinLock)->OldIrql); \
}
#endif

/*++

VOID
NdisDprAcquireSpinLock(
    IN PNDIS_SPIN_LOCK SpinLock;
    );

--*/

#ifdef NDISDBG
#define NdisDprAcquireSpinLock(_SpinLock) { \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("==>NdisDprAcquireSpinLock\n"); \
    } \
    KeAcquireSpinLockAtDpcLevel(&(_SpinLock)->SpinLock); \
    (_SpinLock)->OldIrql = DISPATCH_LEVEL; \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("<==NdisDprAcquireSpinLock\n"); \
    } \
}
#else
#define NdisDprAcquireSpinLock(_SpinLock) { \
    KeAcquireSpinLockAtDpcLevel(&(_SpinLock)->SpinLock); \
    (_SpinLock)->OldIrql = DISPATCH_LEVEL; \
}
#endif


/*++

VOID
NdisDprReleaseSpinLock(
    IN PNDIS_SPIN_LOCK SpinLock;
    );

--*/

#ifdef NDISDBG
#define NdisDprReleaseSpinLock(_SpinLock) { \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("==>NdisDprReleaseSpinLock \n"); \
    } \
    KeReleaseSpinLockFromDpcLevel(&(_SpinLock)->SpinLock); \
    IF_TRACE(TRACE_ALL) { \
        NdisPrint1("<==NdisDprReleaseSpinLock\n"); \
    }\
}
#else
#define NdisDprReleaseSpinLock(_SpinLock) { \
    KeReleaseSpinLockFromDpcLevel(&(_SpinLock)->SpinLock); \
}
#endif



//
// Interlocked support functions
//

/*++

VOID
NdisInterlockedAddUlong(
    IN PULONG Addend,
    IN ULONG Increment,
    IN PNDIS_SPIN_LOCK SpinLock
    );

--*/

#define NdisInterlockedAddUlong(_Addend, _Increment, _SpinLock) \
    ExInterlockedAddUlong(_Addend, _Increment, &(_SpinLock)->SpinLock)


/*++

PLIST_ENTRY
NdisInterlockedInsertHeadList(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PNDIS_SPIN_LOCK SpinLock
    );

--*/

#define NdisInterlockedInsertHeadList(_ListHead, _ListEntry, _SpinLock) \
    ExInterlockedInsertHeadList(_ListHead, _ListEntry, &(_SpinLock)->SpinLock)


/*++

PLIST_ENTRY
NdisInterlockedInsertTailList(
    IN PLIST_ENTRY ListHead,
    IN PLIST_ENTRY ListEntry,
    IN PNDIS_SPIN_LOCK SpinLock
    );

--*/

#define NdisInterlockedInsertTailList(_ListHead, _ListEntry, _SpinLock) \
    ExInterlockedInsertTailList(_ListHead, _ListEntry, &(_SpinLock)->SpinLock)


/*++

PLIST_ENTRY
NdisInterlockedRemoveHeadList(
    IN PLIST_ENTRY ListHead,
    IN PNDIS_SPIN_LOCK SpinLock
    );

--*/

#define NdisInterlockedRemoveHeadList(_ListHead, _SpinLock) \
    ExInterlockedRemoveHeadList(_ListHead, &(_SpinLock)->SpinLock)


//
// Routines to access packet flags
//

/*++

VOID
NdisSetSendFlags(
    IN PNDIS_PACKET Packet,
    IN UINT Flags
    );

--*/

#define NdisSetSendFlags(_Packet,_Flags) \
    (_Packet)->Private.Flags = (_Flags)

/*++

VOID
NdisQuerySendFlags(
    IN PNDIS_PACKET Packet,
    OUT PUINT Flags
    );

--*/

#define NdisQuerySendFlags(_Packet,_Flags) \
    *(_Flags) = (_Packet)->Private.Flags




//
// general reference/dereference functions
//

INTERNAL
BOOLEAN
NdisReferenceRef(
    IN PREFERENCE RefP
    );


INTERNAL
BOOLEAN
NdisDereferenceRef(
    PREFERENCE RefP
    );


INTERNAL
VOID
NdisInitializeRef(
    PREFERENCE RefP
    );


INTERNAL
BOOLEAN
NdisCloseRef(
    PREFERENCE RefP
    );


/*++
INTERNAL
BOOLEAN
NdisReferenceProtocol(
    IN PNDIS_PROTOCOL_BLOCK ProtP
    );
--*/

#define NdisReferenceProtocol(ProtP) \
    NdisReferenceRef(&(ProtP)->Ref)



INTERNAL
BOOLEAN
QueueOpenOnProtocol(
    IN PNDIS_OPEN_BLOCK OpenP,
    IN PNDIS_PROTOCOL_BLOCK ProtP
    );


/*++
INTERNAL
VOID
NdisDereferenceProtocol(
    PNDIS_PROTOCOL_BLOCK ProtP
    );
--*/
#define NdisDereferenceProtocol(ProtP) { \
    if (NdisDereferenceRef(&(ProtP)->Ref)) { \
        ExFreePool((PVOID)(ProtP)); \
    } \
}



INTERNAL
VOID
NdisDeQueueOpenOnProtocol(
    PNDIS_OPEN_BLOCK OpenP,
    PNDIS_PROTOCOL_BLOCK ProtP
    );


INTERNAL
BOOLEAN
NdisFinishOpen(
    PNDIS_OPEN_BLOCK OpenP
    );


INTERNAL
VOID
NdisKillOpenAndNotifyProtocol(
    PNDIS_OPEN_BLOCK OldOpenP
    );


INTERNAL
BOOLEAN
NdisKillOpen(
    PNDIS_OPEN_BLOCK OldOpenP
    );

/*++
INTERNAL
BOOLEAN
NdisReferenceMac(
    IN PNDIS_MAC_BLOCK MacP
    );
--*/
#define NdisReferenceMac(MacP) \
    NdisReferenceRef(&(MacP)->Ref)

static
VOID
NdisDereferenceMac(
    PNDIS_MAC_BLOCK MacP
    );

INTERNAL
BOOLEAN
NdisQueueAdapterOnMac(
    PNDIS_ADAPTER_BLOCK AdaptP,
    PNDIS_MAC_BLOCK MacP
    );

INTERNAL
VOID
NdisDeQueueAdapterOnMac(
    PNDIS_ADAPTER_BLOCK AdaptP,
    PNDIS_MAC_BLOCK MacP
    );

/*++
INTERNAL
BOOLEAN
NdisReferenceAdapter(
    IN PNDIS_ADAPTER_BLOCK AdaptP
    );
--*/
#define NdisReferenceAdapter(AdaptP) \
    NdisReferenceRef(&(AdaptP)->Ref)


INTERNAL
BOOLEAN
NdisQueueOpenOnAdapter(
    PNDIS_OPEN_BLOCK OpenP,
    PNDIS_ADAPTER_BLOCK AdaptP
    );

INTERNAL
VOID
NdisKillAdapter(
    PNDIS_ADAPTER_BLOCK OldAdaptP
    );

INTERNAL
VOID
NdisDereferenceAdapter(
    PNDIS_ADAPTER_BLOCK AdaptP
    );

INTERNAL
VOID
NdisDeQueueOpenOnAdapter(
    PNDIS_OPEN_BLOCK OpenP,
    PNDIS_ADAPTER_BLOCK AdaptP
    );

INTERNAL
NDIS_STATUS
NdisCallDriverAddAdapter(
    IN PNDIS_MAC_BLOCK NewMacP
    );

//
// Configuration Requests
//

VOID
NdisOpenConfiguration(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE ConfigurationHandle,
    IN  NDIS_HANDLE WrapperConfigurationContext
    );

VOID
NdisReadConfiguration(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_CONFIGURATION_PARAMETER *ParameterValue,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING Keyword,
    IN NDIS_PARAMETER_TYPE ParameterType
    );

VOID
NdisCloseConfiguration(
    IN NDIS_HANDLE ConfigurationHandle
    );

VOID
NdisReadNetworkAddress(
    OUT PNDIS_STATUS Status,
    OUT PVOID * NetworkAddress,
    OUT PUINT NetworkAddressLength,
    IN NDIS_HANDLE ConfigurationHandle
    );

VOID
NdisReadBindingInformation(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_STRING * Binding,
    IN NDIS_HANDLE ConfigurationHandle
    );


VOID
NdisReadEisaSlotInformation(
        OUT PNDIS_STATUS Status,
        IN  NDIS_HANDLE WrapperConfigurationContext,
        OUT PUINT SlotNumber,
        OUT PNDIS_EISA_FUNCTION_INFORMATION EisaData
        );

VOID
NdisReadEisaSlotInformationEx(
        OUT PNDIS_STATUS Status,
        IN  NDIS_HANDLE WrapperConfigurationContext,
        OUT PUINT SlotNumber,
        OUT PNDIS_EISA_FUNCTION_INFORMATION *EisaData,
        OUT PUINT NumberOfFunctions
        );

VOID
NdisReadMcaPosInformation(
        OUT PNDIS_STATUS Status,
        IN  NDIS_HANDLE WrapperConfigurationContext,
        IN  PUINT ChannelNumber,
        OUT PNDIS_MCA_POS_DATA McaData
        );

//
// Packet and Buffer Requests
//

VOID
NdisAllocatePacketPool(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE PoolHandle,
    IN UINT NumberOfDescriptors,
    IN UINT ProtocolReservedLength
    );


#ifdef NDISDBG
VOID
NdisFreePacketPool(
    IN NDIS_HANDLE PoolHandle
    );
#else
#define NdisFreePacketPool(PoolHandle) {\
    NdisFreeSpinLock(&((PNDIS_PACKET_POOL)PoolHandle)->SpinLock);\
    ExFreePool(PoolHandle); \
}
#endif

VOID
NdisAllocateBufferPool(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE PoolHandle,
    IN UINT NumberOfDescriptors
    );

VOID
NdisFreeBufferPool(
    IN NDIS_HANDLE PoolHandle
    );

VOID
NdisAllocateBuffer(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_BUFFER * Buffer,
    IN NDIS_HANDLE PoolHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    );


VOID
NdisCopyBuffer(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_BUFFER * Buffer,
    IN NDIS_HANDLE PoolHandle,
    IN PVOID MemoryDescriptor,
    IN UINT Offset,
    IN UINT Length
    );


/*++

VOID
NdisFreeBuffer(
    IN PNDIS_BUFFER Buffer
    );

--*/

#define NdisFreeBuffer(Buffer) \
            IoFreeMdl(Buffer)



VOID
NdisQueryBuffer(
    IN PNDIS_BUFFER Buffer,
    OUT PVOID * VirtualAddress,
    OUT PUINT Length
    );


/*++

VOID
NdisGetBufferPhysicalArraySize(
    IN PNDIS_BUFFER Buffer,
    OUT PUINT ArraySize
    );

--*/

#define NdisGetBufferPhysicalArraySize(Buffer, ArraySize) \
    (*(ArraySize) = NDIS_BUFFER_TO_SPAN_PAGES(Buffer))




/*++
VOID
NdisBufferGetSystemSpecific(
    IN PNDIS_BUFFER Buffer,
    OUT PVOID * SystemSpecific
    );
--*/

//
//BUGBUG: What's system specific about this buffer anyway....
// Ask adam when he comes back.
#define NdisBufferGetSystemSpecific(Buffer, SystemSpecific) \
            *(SystemSpecific) = (Buffer)


/*++

VOID
NdisAdjustBufferLength(
    IN PNDIS_BUFFER Buffer,
    IN UINT Length
    );

--*/

#define NdisAdjustBufferLength(Buffer, Length) \
    (((Buffer)->ByteCount) = (Length))


/*++

NDIS_BUFFER_LINKAGE(
    IN PNDIS_BUFFER Buffer
    );

--*/

#define NDIS_BUFFER_LINKAGE(Buffer) \
    ((Buffer)->Next)



VOID
NdisAllocatePacket(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_PACKET * Packet,
    IN NDIS_HANDLE PoolHandle
    );


#ifdef NDISDBG
VOID
NdisFreePacket(
    IN PNDIS_PACKET Packet
    );
#else
#define NdisFreePacket(Packet) {\
    NdisAcquireSpinLock(&(Packet)->Private.Pool->SpinLock); \
    (Packet)->Private.Head = (PNDIS_BUFFER)(Packet)->Private.Pool->FreeList; \
    (Packet)->Private.Pool->FreeList = (Packet); \
    NdisReleaseSpinLock(&(Packet)->Private.Pool->SpinLock); \
}
#endif


#ifdef NDISDBG
VOID
NdisReinitializePacket( \
    IN OUT PNDIS_PACKET Packet \
    );
#else
#define NdisReinitializePacket(Packet) { \
    (Packet)->Private.Head = (PNDIS_BUFFER)NULL; \
    (Packet)->Private.ValidCounts = FALSE; \
}
#endif


/*++

VOID
NdisRecalculatePacketCounts(
    IN OUT PNDIS_PACKET Packet
    );

--*/

#define NdisRecalculatePacketCounts(Packet) { \
    { \
        PNDIS_BUFFER TmpBuffer = (Packet)->Private.Head; \
        if (TmpBuffer) { \
            while (TmpBuffer->Next) { \
                TmpBuffer = TmpBuffer->Next; \
            } \
            (Packet)->Private.Tail = TmpBuffer; \
        } \
        (Packet)->Private.ValidCounts = FALSE; \
    } \
}


#ifdef NDISDBG
VOID
NdisChainBufferAtFront(
    IN OUT PNDIS_PACKET Packet,
    IN OUT PNDIS_BUFFER Buffer
    );
#else
#define NdisChainBufferAtFront(Packet, Buffer) { \
    PNDIS_BUFFER TmpBuffer = (Buffer); \
\
    for (;;) { \
        if (TmpBuffer->Next == (PNDIS_BUFFER)NULL) \
            break; \
        TmpBuffer = TmpBuffer->Next; \
    } \
    if ((Packet)->Private.Head == (PNDIS_BUFFER)NULL) { \
        (Packet)->Private.Tail = TmpBuffer; \
    } \
    TmpBuffer->Next = (Packet)->Private.Head; \
    (Packet)->Private.Head = (Buffer); \
    (Packet)->Private.ValidCounts = FALSE; \
}
#endif

#ifdef NDISDBG
VOID
NdisChainBufferAtBack(
    IN OUT PNDIS_PACKET Packet,
    IN OUT PNDIS_BUFFER Buffer
    );
#else
#define NdisChainBufferAtBack(Packet, Buffer) { \
    PNDIS_BUFFER TmpBuffer = (Buffer); \
\
    for (;;) { \
        if (TmpBuffer->Next == (PNDIS_BUFFER)NULL) \
            break; \
        TmpBuffer = TmpBuffer->Next; \
    } \
    if ((Packet)->Private.Head != (PNDIS_BUFFER)NULL) { \
        (Packet)->Private.Tail->Next = (Buffer); \
    } else { \
        (Packet)->Private.Head = (Buffer); \
    } \
    (Packet)->Private.Tail = TmpBuffer; \
    TmpBuffer->Next = (PNDIS_BUFFER)NULL; \
    (Packet)->Private.ValidCounts = FALSE; \
}
#endif

VOID
NdisUnchainBufferAtFront(
    IN OUT PNDIS_PACKET Packet,
    OUT PNDIS_BUFFER * Buffer
    );

VOID
NdisUnchainBufferAtBack(
    IN OUT PNDIS_PACKET Packet,
    OUT PNDIS_BUFFER * Buffer
    );


#ifdef NDISDBG
VOID
NdisQueryPacket(
    IN PNDIS_PACKET Packet,
    OUT PUINT PhysicalBufferCount OPTIONAL,
    OUT PUINT BufferCount OPTIONAL,
    OUT PNDIS_BUFFER * FirstBuffer OPTIONAL,
    OUT PUINT TotalPacketLength OPTIONAL
    );
#else
#define NdisQueryPacket(Packet, PhysicalBufferCount, BufferCount, FirstBuffer, TotalPacketLength) \
{ \
    PNDIS_PACKET _Packet = Packet; \
    PUINT _PhysicalBufferCount = PhysicalBufferCount; \
    PUINT _BufferCount = BufferCount; \
    PNDIS_BUFFER *_FirstBuffer = FirstBuffer; \
    PUINT _TotalPacketLength = TotalPacketLength; \
 \
    if (_FirstBuffer) *_FirstBuffer = (Packet)->Private.Head; \
    if (_PhysicalBufferCount || _BufferCount || _TotalPacketLength) { \
    if (!(Packet)->Private.ValidCounts) { \
        PNDIS_BUFFER TmpBuffer = (Packet)->Private.Head; \
        UINT PTotalLength = 0, PPhysicalCount = 0, PAddedCount = 0; \
    \
        while (TmpBuffer != (PNDIS_BUFFER)NULL) { \
            PTotalLength += MmGetMdlByteCount(TmpBuffer); \
            PPhysicalCount += NDIS_BUFFER_TO_SPAN_PAGES(TmpBuffer); \
            ++PAddedCount; \
            TmpBuffer = TmpBuffer->Next; \
        } \
        (Packet)->Private.Count = PAddedCount; \
        (Packet)->Private.TotalLength = PTotalLength; \
        (Packet)->Private.PhysicalCount = PPhysicalCount; \
        (Packet)->Private.ValidCounts = TRUE; \
    } \
    if (_PhysicalBufferCount) *_PhysicalBufferCount = (Packet)->Private.PhysicalCount; \
    if (_BufferCount) *_BufferCount = (Packet)->Private.Count; \
    if (_TotalPacketLength) *_TotalPacketLength = (Packet)->Private.TotalLength; \
    } \
}
#endif


#ifdef NDISDBG
VOID
NdisGetNextBuffer(
    IN PNDIS_BUFFER CurrentBuffer,
    OUT PNDIS_BUFFER * NextBuffer
    );
#else
#define NdisGetNextBuffer(CurrentBuffer, NextBuffer) {\
    *(NextBuffer) = (CurrentBuffer)->Next; \
}
#endif


VOID
NdisCopyFromPacketToPacket(
    IN PNDIS_PACKET Destination,
    IN UINT DestinationOffset,
    IN UINT BytesToCopy,
    IN PNDIS_PACKET Source,
    IN UINT SourceOffset,
    OUT PUINT BytesCopied
    );


//
// Operating System Requests
//

VOID
NdisMapIoSpace(
    OUT PNDIS_STATUS Status,
    OUT PVOID * VirtualAddress,
    IN NDIS_HANDLE NdisAdapterHandle,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
    IN UINT Length
    );

#ifdef _ALPHA_

/*++
VOID
NdisUnmapIoSpace(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    )
--*/
#define NdisUnmapIoSpace(Handle,VirtualAddress,Length) {}

#else

/*++
VOID
NdisUnmapIoSpace(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    )
--*/
#define NdisUnmapIoSpace(Handle,VirtualAddress,Length) \
            MmUnmapIoSpace((VirtualAddress), (Length));

#endif


NDIS_STATUS
NdisAllocateMemory(
    OUT PVOID *VirtualAddress,
    IN UINT Length,
    IN UINT MemoryFlags,
    IN NDIS_PHYSICAL_ADDRESS HighestAcceptableAddress
    );


VOID
NdisFreeMemory(
    IN PVOID VirtualAddress,
    IN UINT Length,
    IN UINT MemoryFlags
    );

typedef
VOID
(*PNDIS_TIMER_FUNCTION) (
    IN PVOID SystemSpecific1,
    IN PVOID FunctionContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

VOID
NdisInitializeTimer(
    IN OUT PNDIS_TIMER Timer,
    IN PNDIS_TIMER_FUNCTION TimerFunction,
    IN PVOID FunctionContext
    );

VOID
NdisSetTimer(
    IN PNDIS_TIMER Timer,
    IN UINT MillisecondsToDelay
    );

/*++
VOID
NdisCancelTimer(
    IN PNDIS_TIMER Timer,
    OUT PBOOLEAN TimerCancelled
    )
--*/
#define NdisCancelTimer(NdisTimer,TimerCancelled) \
            (*(TimerCancelled) = KeCancelTimer(&((NdisTimer)->Timer)))

/*++
VOID
NdisStallExecution(
    IN UINT MicrosecondsToStall
    )
--*/

#define NdisStallExecution(MicroSecondsToStall) \
        KeStallExecutionProcessor(MicroSecondsToStall)


VOID
NdisInitializeInterrupt(
    OUT PNDIS_STATUS Status,
    IN OUT PNDIS_INTERRUPT Interrupt,
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_INTERRUPT_SERVICE InterruptServiceRoutine,
    IN PVOID InterruptContext,
    IN PNDIS_DEFERRED_PROCESSING DeferredProcessingRoutine,
    IN UINT InterruptVector,
    IN UINT InterruptLevel,
    IN BOOLEAN SharedInterrupt,
    IN NDIS_INTERRUPT_MODE InterruptMode
    );

VOID
NdisRemoveInterrupt(
    IN PNDIS_INTERRUPT Interrupt
    );

/*++
BOOLEAN
NdisSynchronizeWithInterrupt(
    IN PNDIS_INTERRUPT Interrupt,
    IN PVOID SynchronizeFunction,
    IN PVOID SynchronizeContext
    )
--*/

#define NdisSynchronizeWithInterrupt(Interrupt,Function,Context) \
            KeSynchronizeExecution( \
                (Interrupt)->InterruptObject,\
                (PKSYNCHRONIZE_ROUTINE)Function,\
                Context  \
                )

//
// Simple I/O support
//

VOID
NdisOpenFile(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE FileHandle,
    OUT PUINT FileLength,
    IN PNDIS_STRING FileName,
    IN NDIS_PHYSICAL_ADDRESS HighestAcceptableAddress
    );

VOID
NdisCloseFile(
    IN NDIS_HANDLE FileHandle
    );

VOID
NdisMapFile(
    OUT PNDIS_STATUS Status,
    OUT PVOID * MappedBuffer,
    IN NDIS_HANDLE FileHandle
    );

VOID
NdisUnmapFile(
    IN NDIS_HANDLE FileHandle
    );


//
// Portability extensions
//

/*++
VOID
NdisFlushBuffer(
        IN PNDIS_BUFFER Buffer,
        IN BOOLEAN WriteToDevice
        )
--*/

#define NdisFlushBuffer(Buffer,WriteToDevice) \
        KeFlushIoBuffers((Buffer),!(WriteToDevice), TRUE)


/*++
ULONG
NdisGetCacheFillSize(
    )
--*/
#define NdisGetCacheFillSize() \
        HalGetDmaAlignmentRequirement()

//
// This macro is used to convert a port number as the caller
// thinks of it, to a port number as it should be passed to
// READ/WRITE_PORT.
//

#define NDIS_PORT_TO_PORT(Handle,Port)  (((PNDIS_ADAPTER_BLOCK)(Handle))->PortOffset + (Port))


//
// Write Port
//

/*++
VOID
NdisWritePortUchar(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        IN UCHAR Data
        )
--*/
#define NdisWritePortUchar(Handle,Port,Data) \
        WRITE_PORT_UCHAR((PUCHAR)(NDIS_PORT_TO_PORT(Handle,Port)),(UCHAR)(Data))

/*++
VOID
NdisWritePortUshort(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        IN USHORT Data
        )
--*/
#define NdisWritePortUshort(Handle,Port,Data) \
        WRITE_PORT_USHORT((PUSHORT)(NDIS_PORT_TO_PORT(Handle,Port)),(USHORT)(Data))


/*++
VOID
NdisWritePortUlong(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        IN ULONG Data
        )
--*/
#define NdisWritePortUlong(Handle,Port,Data) \
        WRITE_PORT_ULONG((PULONG)(NDIS_PORT_TO_PORT(Handle,Port)),(ULONG)(Data))


//
// Write Port Buffers
//

/*++
VOID
NdisWritePortBufferUchar(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        IN PUCHAR Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisWritePortBufferUchar(Handle,Port,Buffer,Length) \
        WRITE_PORT_BUFFER_UCHAR((PUCHAR)(NDIS_PORT_TO_PORT(Handle,Port)),(PUCHAR)(Buffer),(Length))
#else
#define NdisWritePortBufferUchar(Handle,Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)NDIS_PORT_TO_PORT(Handle, Port); \
        PUCHAR _Current = (Buffer); \
        PUCHAR _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            WRITE_PORT_UCHAR((PUCHAR)_Port,*_Current); \
        } \
}
#endif

/*++
VOID
NdisWritePortBufferUshort(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        IN PUSHORT Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisWritePortBufferUshort(Handle,Port,Buffer,Length) \
        WRITE_PORT_BUFFER_USHORT((PUSHORT)(NDIS_PORT_TO_PORT(Handle,Port)),(PUSHORT)(Buffer),(Length))
#else
#define NdisWritePortBufferUshort(Handle,Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)NDIS_PORT_TO_PORT(Handle, Port); \
        PUSHORT _Current = (Buffer); \
        PUSHORT _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            WRITE_PORT_USHORT((PUSHORT)_Port,*(UNALIGNED USHORT *)_Current); \
        } \
}
#endif


/*++
VOID
NdisWritePortBufferUlong(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        IN PULONG Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisWritePortBufferUlong(Handle,Port,Buffer,Length) \
        WRITE_PORT_BUFFER_ULONG((PULONG)(NDIS_PORT_TO_PORT(Handle,Port)),(PULONG)(Buffer),(Length))
#else
#define NdisWritePortBufferUlong(Handle,Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)NDIS_PORT_TO_PORT(Handle, Port); \
        PULONG _Current = (Buffer); \
        PULONG _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            WRITE_PORT_ULONG((PULONG)_Port,*(UNALIGNED ULONG *)_Current); \
        } \
}
#endif


//
// Read Ports
//

/*++
VOID
NdisReadPortUchar(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        OUT PUCHAR Data
        )
--*/
#define NdisReadPortUchar(Handle,Port, Data) \
        *(Data) = READ_PORT_UCHAR((PUCHAR)(NDIS_PORT_TO_PORT(Handle,Port)))

/*++
VOID
NdisReadPortUshort(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        OUT PUSHORT Data
        )
--*/
#define NdisReadPortUshort(Handle,Port,Data) \
        *(Data) = READ_PORT_USHORT((PUSHORT)(NDIS_PORT_TO_PORT(Handle,Port)))


/*++
VOID
NdisReadPortUlong(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        OUT PULONG Data
        )
--*/
#define NdisReadPortUlong(Handle,Port,Data) \
        *(Data) = READ_PORT_ULONG((PULONG)(NDIS_PORT_TO_PORT(Handle,Port)))

//
// Read Buffer Ports
//

/*++
VOID
NdisReadPortBufferUchar(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        OUT PUCHAR Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisReadPortBufferUchar(Handle,Port,Buffer,Length) \
        READ_PORT_BUFFER_UCHAR((PUCHAR)(NDIS_PORT_TO_PORT(Handle,Port)),(PUCHAR)(Buffer),(Length))
#else
#define NdisReadPortBufferUchar(Handle,Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)NDIS_PORT_TO_PORT(Handle, Port); \
        PUCHAR _Current = (Buffer); \
        PUCHAR _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            *_Current = READ_PORT_UCHAR((PUCHAR)_Port); \
        } \
}
#endif

/*++
VOID
NdisReadPortBufferUshort(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        OUT PUSHORT Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisReadPortBufferUshort(Handle,Port,Buffer,Length) \
        READ_PORT_BUFFER_USHORT((PUSHORT)(NDIS_PORT_TO_PORT(Handle,Port)),(PUSHORT)(Buffer),(Length))
#else
#define NdisReadPortBufferUshort(Handle,Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)NDIS_PORT_TO_PORT(Handle, Port); \
        PUSHORT _Current = (Buffer); \
        PUSHORT _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            *(UNALIGNED USHORT *)_Current = READ_PORT_USHORT((PUSHORT)_Port); \
        } \
}
#endif


/*++
VOID
NdisReadPortBufferUlong(
        IN NDIS_HANDLE NdisAdapterHandle,
        IN ULONG Port,
        OUT PULONG Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisReadPortBufferUlong(Handle,Port,Buffer) \
        READ_PORT_BUFFER_ULONG((PULONG)(NDIS_PORT_TO_PORT(Handle,Port)),(PULONG)(Buffer),(Length))
#else
#define NdisReadPortBufferUlong(Handle,Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)NDIS_PORT_TO_PORT(Handle, Port); \
        PULONG _Current = (Buffer); \
        PULONG _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            *(UNALIGNED ULONG *)_Current = READ_PORT_ULONG((PULONG)_Port); \
        } \
}
#endif


//
// Raw Routines
//

//
// Write Port Raw
//

/*++
VOID
NdisRawWritePortUchar(
        IN ULONG Port,
        IN UCHAR Data
        )
--*/
#define NdisRawWritePortUchar(Port,Data) \
        WRITE_PORT_UCHAR((PUCHAR)(Port),(UCHAR)(Data))

/*++
VOID
NdisRawWritePortUshort(
        IN ULONG Port,
        IN USHORT Data
        )
--*/
#define NdisRawWritePortUshort(Port,Data) \
        WRITE_PORT_USHORT((PUSHORT)(Port),(USHORT)(Data))


/*++
VOID
NdisRawWritePortUlong(
        IN ULONG Port,
        IN ULONG Data
        )
--*/
#define NdisRawWritePortUlong(Port,Data) \
        WRITE_PORT_ULONG((PULONG)(Port),(ULONG)(Data))


//
// Raw Write Port Buffers
//

/*++
VOID
NdisRawWritePortBufferUchar(
        IN ULONG Port,
        IN PUCHAR Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisRawWritePortBufferUchar(Port,Buffer,Length) \
        WRITE_PORT_BUFFER_UCHAR((PUCHAR)(Port),(PUCHAR)(Buffer),(Length))
#else
#define NdisRawWritePortBufferUchar(Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)(Port); \
        PUCHAR _Current = (Buffer); \
        PUCHAR _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            WRITE_PORT_UCHAR((PUCHAR)_Port,*_Current); \
        } \
}
#endif

/*++
VOID
NdisRawWritePortBufferUshort(
        IN ULONG Port,
        IN PUSHORT Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisRawWritePortBufferUshort(Port,Buffer,Length) \
        WRITE_PORT_BUFFER_USHORT((PUSHORT)(Port),(PUSHORT)(Buffer),(Length))
#else
#define NdisRawWritePortBufferUshort(Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)(Port); \
        PUSHORT _Current = (Buffer); \
        PUSHORT _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            WRITE_PORT_USHORT((PUSHORT)_Port,*(UNALIGNED USHORT *)_Current); \
        } \
}
#endif


/*++
VOID
NdisRawWritePortBufferUlong(
        IN ULONG Port,
        IN PULONG Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisRawWritePortBufferUlong(Port,Buffer,Length) \
        WRITE_PORT_BUFFER_ULONG((PULONG)(Port),(PULONG)(Buffer),(Length))
#else
#define NdisRawWritePortBufferUlong(Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)(Port); \
        PULONG _Current = (Buffer); \
        PULONG _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            WRITE_PORT_ULONG((PULONG)_Port,*(UNALIGNED ULONG *)_Current); \
        } \
}
#endif


//
// Raw Read Ports
//

/*++
VOID
NdisRawReadPortUchar(
        IN ULONG Port,
        OUT PUCHAR Data
        )
--*/
#define NdisRawReadPortUchar(Port, Data) \
        *(Data) = READ_PORT_UCHAR((PUCHAR)(Port))

/*++
VOID
NdisRawReadPortUshort(
        IN ULONG Port,
        OUT PUSHORT Data
        )
--*/
#define NdisRawReadPortUshort(Port,Data) \
        *(Data) = READ_PORT_USHORT((PUSHORT)(Port))


/*++
VOID
NdisRawReadPortUlong(
        IN ULONG Port,
        OUT PULONG Data
        )
--*/
#define NdisRawReadPortUlong(Port,Data) \
        *(Data) = READ_PORT_ULONG((PULONG)(Port))

//
// Raw Read Buffer Ports
//

/*++
VOID
NdisRawReadPortBufferUchar(
        IN ULONG Port,
        OUT PUCHAR Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisRawReadPortBufferUchar(Port,Buffer,Length) \
        READ_PORT_BUFFER_UCHAR((PUCHAR)(Port),(PUCHAR)(Buffer),(Length))
#else
#define NdisRawReadPortBufferUchar(Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)(Port); \
        PUCHAR _Current = (Buffer); \
        PUCHAR _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            *_Current = READ_PORT_UCHAR((PUCHAR)_Port); \
        } \
}
#endif

/*++
VOID
NdisRawReadPortBufferUshort(
        IN ULONG Port,
        OUT PUSHORT Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisRawReadPortBufferUshort(Port,Buffer,Length) \
        READ_PORT_BUFFER_USHORT((PUSHORT)(Port),(PUSHORT)(Buffer),(Length))
#else
#define NdisRawReadPortBufferUshort(Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)(Port); \
        PUSHORT _Current = (Buffer); \
        PUSHORT _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            *(UNALIGNED USHORT *)_Current = READ_PORT_USHORT((PUSHORT)_Port); \
        } \
}
#endif


/*++
VOID
NdisRawReadPortBufferUlong(
        IN ULONG Port,
        OUT PULONG Buffer,
        IN ULONG Length
        )
--*/
#ifdef _M_IX86
#define NdisRawReadPortBufferUlong(Port,Buffer,Length) \
        READ_PORT_BUFFER_ULONG((PULONG)(Port),(PULONG)(Buffer),(Length))
#else
#define NdisRawReadPortBufferUlong(Port,Buffer,Length) \
{ \
        ULONG _Port = (ULONG)(Port); \
        PULONG _Current = (Buffer); \
        PULONG _End = _Current + (Length); \
        for ( ; _Current < _End; ++_Current) { \
            *(UNALIGNED ULONG *)_Current = READ_PORT_ULONG((PULONG)_Port); \
        } \
}
#endif






//
// Write Registers
//

/*++
VOID
NdisWriteRegisterUchar(
        IN PUCHAR Register,
        IN UCHAR Data
        )
--*/

#ifdef  _M_IX86

#define NdisWriteRegisterUchar(Register,Data) \
    *((PUCHAR)(Register)) = (Data)

#else

#define NdisWriteRegisterUchar(Register,Data) \
{ \
        WRITE_REGISTER_UCHAR((Register),(Data)); \
        READ_REGISTER_UCHAR(Register); \
}

#endif

/*++
VOID
NdisWriteRegisterUshort(
        IN PUSHORT Register,
        IN USHORT Data
        )
--*/

#ifdef  _M_IX86

#define NdisWriteRegisterUshort(Register,Data) \
    *((PUSHORT)(Register)) = (Data)

#else

#define NdisWriteRegisterUshort(Register,Data) \
{ \
        WRITE_REGISTER_USHORT((Register),(Data)); \
        READ_REGISTER_USHORT(Register); \
}

#endif

/*++
VOID
NdisWriteRegisterUlong(
        IN PULONG Register,
        IN ULONG Data
        )
--*/

#ifdef  _M_IX86

#define NdisWriteRegisterUlong(Register,Data) \
    *((PULONG)(Register)) = (Data)

#else

#define NdisWriteRegisterUlong(Register,Data) \
{ \
        WRITE_REGISTER_ULONG((Register),(Data)); \
        READ_REGISTER_ULONG(Register); \
}

#endif

/*++
VOID
NdisReadRegisterUchar(
        IN PUCHAR Register,
        OUT PUCHAR Data
        )
--*/
#ifdef  _M_IX86

#define NdisReadRegisterUchar(Register,Data) \
    *((PUCHAR)(Data)) = *(Register)

#else

#define NdisReadRegisterUchar(Register,Data) \
        *(Data) = READ_REGISTER_UCHAR((PUCHAR)(Register))
#endif

/*++
VOID
NdisReadRegisterUshort(
        IN PUSHORT Register,
        OUT PUSHORT Data
        )
--*/
#ifdef  _M_IX86

#define NdisReadRegisterUshort(Register,Data) \
    *((PUSHORT)(Data)) = *(Register)

#else

#define NdisReadRegisterUshort(Register,Data) \
        *(Data) = READ_REGISTER_USHORT((PUSHORT)(Register))
#endif

/*++
VOID
NdisReadRegisterUlong(
        IN PULONG Register,
        OUT PULONG Data
        )
--*/
#ifdef  _M_IX86

#define NdisReadRegisterUlong(Register,Data) \
    *((PULONG)(Data)) = *(Register)

#else

#define NdisReadRegisterUlong(Register,Data) \
        *(Data) = READ_REGISTER_ULONG((PULONG)(Register))
#endif

//
// BOOLEAN
// NdisEqualString(
//          IN PNDIS_STRING _String1,
//          IN PNDIS_STRING _String2,
//          IN BOOLEAN CaseInsensitive
//          );
//

#define NdisEqualString(_String1,_String2,CaseInsensitive) \
            RtlEqualUnicodeString((_String1), (_String2), CaseInsensitive)


//
// Physical Mapping
//

//
// VOID
// NdisStartBufferPhysicalMapping(
//     IN NDIS_HANDLE NdisAdapterHandle,
//     IN PNDIS_BUFFER Buffer,
//     IN ULONG PhysicalMapRegister,
//     IN BOOLEAN WriteToDevice,
//     OUT PNDIS_PHYSICAL_ADDRESS_UNIT PhysicalAddressArray,
//     OUT PUINT ArraySize
//     );
//

#define NdisStartBufferPhysicalMapping(                                         \
              NdisAdapterHandle,                                                \
              Buffer,                                                           \
              PhysicalMapRegister,                                              \
              Write,                                                            \
              PhysicalAddressArray,                                             \
              ArraySize                                                         \
              )                                                                 \
{                                                                               \
    PNDIS_ADAPTER_BLOCK _AdaptP = (PNDIS_ADAPTER_BLOCK)(NdisAdapterHandle);     \
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
                             _AdaptP->MapRegisters[_PhysicalMapRegister].MapRegister,   \
                             _VirtualAddress,                                   \
                             &_LengthMapped,                                    \
                             _WriteToDevice);                                   \
        _PhysicalAddressArray[_CurrentArrayLocation].PhysicalAddress = _LogicalAddress; \
        _PhysicalAddressArray[_CurrentArrayLocation].Length = _LengthMapped;    \
        _LengthRemaining -= _LengthMapped;                                      \
        _VirtualAddress += _LengthMapped;                                       \
        ++_CurrentArrayLocation;                                                \
    }                                                                           \
    _AdaptP->MapRegisters[_PhysicalMapRegister].WriteToDevice = _WriteToDevice; \
    *(_ArraySize) = _CurrentArrayLocation;                                      \
}


//
// VOID
// NdisCompleteBufferPhysicalMapping(
//     IN NDIS_HANDLE NdisAdapterHandle,
//     IN PNDIS_BUFFER Buffer,
//     IN ULONG PhysicalMapRegister
//     );
//

#define NdisCompleteBufferPhysicalMapping( \
    NdisAdapterHandle,                     \
    Buffer,                                \
    PhysicalMapRegister                    \
    )                                      \
{                                          \
    PNDIS_ADAPTER_BLOCK _AdaptP = (PNDIS_ADAPTER_BLOCK)NdisAdapterHandle; \
    IoFlushAdapterBuffers(                                                \
        NULL,                                                             \
        Buffer,                                                           \
        _AdaptP->MapRegisters[PhysicalMapRegister].MapRegister,           \
        MmGetMdlVirtualAddress(Buffer),                                   \
        MmGetMdlByteCount(Buffer),                                        \
        _AdaptP->MapRegisters[PhysicalMapRegister].WriteToDevice);        \
}


//
// Shared memory
//

VOID
NdisAllocateSharedMemory(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    OUT PVOID *VirtualAddress,
    OUT PNDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

VOID
NdisUpdateSharedMemory(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG Length,
    IN PVOID VirtualAddress,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

VOID
NdisFreeSharedMemory(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    IN PVOID VirtualAddress,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );


//
// DMA operations.
//

VOID
NdisAllocateDmaChannel(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE NdisDmaHandle,
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_DMA_DESCRIPTION DmaDescription,
    IN ULONG MaximumLength
    );

VOID
NdisFreeDmaChannel(
    IN PNDIS_HANDLE NdisDmaHandle
    );

VOID
NdisSetupDmaTransfer(
    OUT PNDIS_STATUS Status,
    IN PNDIS_HANDLE NdisDmaHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG Offset,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    );

VOID
NdisCompleteDmaTransfer(
    OUT PNDIS_STATUS Status,
    IN PNDIS_HANDLE NdisDmaHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG Offset,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    );

/*++
ULONG
NdisReadDmaCounter(
    IN NDIS_HANDLE NdisDmaHandle
    )
--*/

#define NdisReadDmaCounter(_NdisDmaHandle) \
    HalReadDmaCounter(((PNDIS_DMA_BLOCK)(_NdisDmaHandle))->SystemAdapterObject)



//
// Requests used by Protocol Modules
//



VOID
NdisRegisterProtocol(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE NdisProtocolHandle,
    IN PNDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics,
    IN UINT CharacteristicsLength
    );

VOID
NdisDeregisterProtocol(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisProtocolHandle
    );


VOID
NdisOpenAdapter(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_STATUS OpenErrorStatus,
    OUT PNDIS_HANDLE NdisBindingHandle,
    OUT PUINT SelectedMediumIndex,
    IN PNDIS_MEDIUM MediumArray,
    IN UINT MediumArraySize,
    IN NDIS_HANDLE NdisProtocolHandle,
    IN NDIS_HANDLE ProtocolBindingContext,
    IN PNDIS_STRING AdapterName,
    IN UINT OpenOptions,
    IN PSTRING AddressingInformation OPTIONAL
    );


VOID
NdisCloseAdapter(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle
    );


#ifdef NDISDBG
VOID
NdisSend(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle,
    IN PNDIS_PACKET Packet
    );
#else
#define NdisSend(Status, \
    NdisBindingHandle, \
    Packet \
    ) \
{\
    *(Status) = \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->SendHandler) ( \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle, \
            (Packet)); \
}
#endif


#ifdef NDISDBG
VOID
NdisTransferData(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    IN OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );
#else
#define NdisTransferData( \
    Status, \
    NdisBindingHandle, \
    MacReceiveContext, \
    ByteOffset, \
    BytesToTransfer, \
    Packet, \
    BytesTransferred \
    ) \
{\
    *(Status) = \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->TransferDataHandler) ( \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle, \
            (MacReceiveContext), \
            (ByteOffset), \
            (BytesToTransfer), \
            (Packet), \
            (BytesTransferred)); \
}
#endif


#ifdef NDISDBG
VOID
NdisReset(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle
    );
#else
#define NdisReset( \
    Status, \
    NdisBindingHandle \
    ) \
{ \
    *(Status) = \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacHandle->MacCharacteristics.ResetHandler) ( \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle); \
}
#endif

#ifdef NDISDBG
VOID
NdisRequest(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );
#else
#define NdisRequest( \
    Status,\
    NdisBindingHandle, \
    NdisRequest \
    ) \
{ \
    *(Status) = \
        (((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacHandle->MacCharacteristics.RequestHandler) ( \
            ((PNDIS_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle, \
            (NdisRequest)); \
}
#endif

//
// Requests Used by MAC Drivers
//

extern PDRIVER_OBJECT Driver;       // set by initialization routine


VOID
NdisInitializeWrapper(
    OUT PNDIS_HANDLE NdisWrapperHandle,
    IN PVOID SystemSpecific1,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );


VOID
NdisTerminateWrapper(
    IN NDIS_HANDLE NdisWrapperHandle,
    IN PVOID SystemSpecific
    );

VOID
NdisRegisterMac(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE NdisWrapperHandle,
    IN NDIS_HANDLE MacMacContext,
    IN PNDIS_MAC_CHARACTERISTICS MacCharacteristics,
    IN UINT CharacteristicsLength
    );

VOID
NdisDeregisterMac(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisMacHandle
    );


NDIS_STATUS
NdisRegisterAdapter(
    OUT PNDIS_HANDLE NdisAdapterHandle,
    IN NDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE MacAdapterContext,
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN PNDIS_STRING AdapterName,
    IN PVOID AdapterInformation
    );

NDIS_STATUS
NdisDeregisterAdapter(
    IN NDIS_HANDLE NdisAdapterHandle
    );

VOID
NdisRegisterAdapterShutdownHandler(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PVOID ShutdownContext,
    IN ADAPTER_SHUTDOWN_HANDLER ShutdownHandler
    );

VOID
NdisDeregisterAdapterShutdownHandler(
    IN NDIS_HANDLE NdisAdapterHandle
    );

VOID
NdisReleaseAdapterResources(
    IN NDIS_HANDLE NdisAdapterHandle
    );

VOID
NdisWriteErrorLogEntry(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN NDIS_ERROR_CODE ErrorCode,
    IN ULONG NumberOfErrorValues,
    ...
    );

VOID
NdisCompleteOpenAdapter(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status,
    IN NDIS_STATUS OpenErrorStatus
    );


VOID
NdisCompleteCloseAdapter(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );


#ifdef NDISDBG
VOID
NdisCompleteSend(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status
    );
#else
#define NdisCompleteSend( \
    NdisBindingContext, \
    Packet, \
    Status \
    ) \
{\
    (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->SendCompleteHandler) ( \
        ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
        (Packet), \
        (Status)); \
}
#endif


#ifdef NDISDBG
VOID
NdisCompleteTransferData(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status,
    IN UINT BytesTransferred
    );
#else
#define NdisCompleteTransferData( \
    NdisBindingContext, \
    Packet, \
    Status, \
    BytesTransferred \
    ) \
{\
    (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->TransferDataCompleteHandler) ( \
        ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
        (Packet), \
        (Status), \
        (BytesTransferred)); \
}
#endif

#ifdef NDISDBG
VOID
NdisCompleteReset(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );
#else
#define NdisCompleteReset( \
    NdisBindingContext, \
    Status \
    ) \
{ \
    (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolHandle->ProtocolCharacteristics.ResetCompleteHandler) ( \
        ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
        Status); \
}
#endif


#ifdef NDISDBG
VOID
NdisCompleteRequest(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    );
#else
#define NdisCompleteRequest( \
    NdisBindingContext, \
    NdisRequest, \
    Status) \
{ \
    (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolHandle->ProtocolCharacteristics.RequestCompleteHandler) ( \
        ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
        NdisRequest, \
        Status); \
}
#endif

#ifdef NDISDBG
VOID
NdisIndicateReceive(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookaheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    );
#else
#define NdisIndicateReceive( \
    Status, \
    NdisBindingContext, \
    MacReceiveContext, \
    HeaderBuffer, \
    HeaderBufferSize, \
    LookaheadBuffer, \
    LookaheadBufferSize, \
    PacketSize \
    ) \
{\
    *(Status) = \
        (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ReceiveHandler) ( \
            ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
            (MacReceiveContext), \
            (HeaderBuffer), \
            (HeaderBufferSize), \
            (LookaheadBuffer), \
            (LookaheadBufferSize), \
            (PacketSize)); \
}
#endif


#ifdef NDISDBG
VOID
NdisIndicateReceiveComplete(
    IN NDIS_HANDLE NdisBindingContext
    );
#else
#define NdisIndicateReceiveComplete( \
    NdisBindingContext \
    ) \
    (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ReceiveCompleteHandler) ( \
        ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext);
#endif


#ifdef NDISDBG
VOID
NdisIndicateStatus(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS GeneralStatus,
    IN PVOID StatusBuffer,
    IN UINT StatusBufferSize
    );
#else
#define NdisIndicateStatus( \
    NdisBindingContext, \
    GeneralStatus, \
    StatusBuffer, \
    StatusBufferSize \
    ) \
{\
    (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolHandle->ProtocolCharacteristics.StatusHandler) ( \
        ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
        (GeneralStatus), \
        (StatusBuffer), \
        (StatusBufferSize)); \
}
#endif


#ifdef NDISDBG
VOID
NdisIndicateStatusComplete(
    IN NDIS_HANDLE NdisBindingContext
    );
#else
#define NdisIndicateStatusComplete( \
    NdisBindingContext \
    ) \
{ \
    (((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolHandle->ProtocolCharacteristics.StatusCompleteHandler) ( \
        ((PNDIS_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext); \
}
#endif

VOID
NdisCompleteQueryStatistics(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    );

#define NdisInitializeString(Destination,Source) \
{\
    PNDIS_STRING _D = (Destination);\
    UCHAR *_S = (Source);\
    WCHAR *_P;\
    _D->Length = (strlen(_S)) * sizeof(WCHAR);\
    _D->MaximumLength = _D->Length + sizeof(WCHAR);\
    NdisAllocateMemory((PVOID *)&(_D->Buffer), _D->MaximumLength, 0, (-1));\
    _P = _D->Buffer;\
    while(*_S != '\0'){\
        *_P = (WCHAR)(*_S);\
        _S++;\
        _P++;\
    }\
    *_P = UNICODE_NULL;\
}


#define NdisFreeString(String) NdisFreeMemory((String).Buffer, (String).MaximumLength, 0)

#define NdisPrintString(String) DbgPrint("%ls",(String).Buffer)


#ifdef _M_IX86
/*++

    VOID
    NdisCreateLookaheadBufferFromSharedMemory(
        IN PVOID pSharedMemory,
        IN UINT LookaheadLength,
        OUT PVOID *pLookaheadBuffer
        );

--*/

#define NdisCreateLookaheadBufferFromSharedMemory(_S, _L, _B) \
  ((*(_B)) = (_S))



/*++

    VOID
    NdisDestroyLookaheadBufferFromSharedMemory(
        IN PVOID pLookaheadBuffer
        );

--*/

#define NdisDestroyLookaheadBufferFromSharedMemory(_B)

#else
#ifdef mips

/*++

    VOID
    NdisCreateLookaheadBufferFromSharedMemory(
        IN PVOID pSharedMemory,
        IN UINT LookaheadLength,
        OUT PVOID *pLookaheadBuffer
        );

--*/

#define NdisCreateLookaheadBufferFromSharedMemory(_S, _L, _B) \
  ((*(_B)) = (_S))



/*++

    VOID
    NdisDestroyLookaheadBufferFromSharedMemory(
        IN PVOID pLookaheadBuffer
        );

--*/

#define NdisDestroyLookaheadBufferFromSharedMemory(_B)

#else

VOID
NdisCreateLookaheadBufferFromSharedMemory(
    IN PVOID pSharedMemory,
    IN UINT LookaheadLength,
    OUT PVOID *pLookaheadBuffer
    );

VOID
NdisDestroyLookaheadBufferFromSharedMemory(
    IN PVOID pLookaheadBuffer
    );

#endif // mips
#endif // i386


//
//*\\ Stubs to compile with Ndis 3.0 Kernel
//

extern
NDIS_STATUS
EthAddFilterAddress();

extern
NDIS_STATUS
EthDeleteFilterAddress();

extern
NDIS_STATUS
NdisInitializePacketPool();

// ZZZZ

#ifdef NDISDBG
VOID
DbgChkPktLength(
  IN PNDIS_PACKET Packet
  );
#endif

#endif  // _NDIS_
