/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    ne3200proc.h

Abstract:

    Procedure declarations for the Novell NE3200 NDIS 3.0 driver.
    Moved most of these from ne3200sw.h

Environment:

    This driver is expected to work in DOS, OS2 and NT at the equivalent
    of kernal mode.

    Architecturally, there is an assumption in this driver that we are
    on a little endian machine.

Notes:

    optional-notes

--*/

#ifndef _NE3200PROC_
#define _NE3200PROC_
//
// We define the external interfaces to the NE3200 driver.
// These routines are only external to permit separate
// compilation.  Given a truely fast compiler they could
// all reside in a single file and be static.
//

extern
NDIS_STATUS
NE3200TransferData(
    IN NDIS_HANDLE MacBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );

extern
NDIS_STATUS
NE3200Send(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );


extern
VOID
NE3200StagedAllocation(
    IN PNE3200_ADAPTER Adapter
    );

extern
VOID
NE3200CopyFromBufferToPacket(
    IN PCHAR Buffer,
    IN UINT BytesToCopy,
    IN PNDIS_PACKET Packet,
    IN UINT Offset,
    OUT PUINT BytesCopied
    );

extern
VOID
NE3200CopyFromPacketToBuffer(
    IN PNDIS_PACKET Packet,
    IN UINT Offset,
    IN UINT BytesToCopy,
    OUT PCHAR Buffer,
    OUT PUINT BytesCopied
    );

extern
VOID
NE3200CopyFromPacketToPacket(
    IN PNDIS_PACKET Destination,
    IN UINT DestinationOffset,
    IN UINT BytesToCopy,
    IN PNDIS_PACKET Source,
    IN UINT SourceOffset,
    OUT PUINT BytesCopied
    );

extern
VOID
NE3200ProcessLoopback(
    IN PNE3200_ADAPTER Adapter
    );

extern
VOID
NE3200Shutdown(
    IN PVOID ShutdownContext
    );

/*++

VOID
NE3200PutPacketOnFinishTrans(
    IN PNE3200_ADAPTER Adapter,
    IN PNDIS_PACKET Packet
    )

Routine Description:

    Put the packet on the adapter wide queue for packets that
    are transmitting.

    NOTE: This routine assumes that the lock is held.

    NOTE: By definition any packet given to this routine is ready
    to complete.

Arguments:

    Adapter - The adapter that contains the queue.

    Packet - The packet to be put on the queue.

Return Value:

    None.

--*/

#define NE3200PutPacketOnFinishTrans(Adapter, Packet)                            \
{                                                                                \
    PNE3200_RESERVED _Reserved = PNE3200_RESERVED_FROM_PACKET(Packet);           \
    PNE3200_ADAPTER _Adapter = Adapter;                                          \
    if (_Adapter->LastFinishTransmit) {                                          \
        PNE3200_RESERVED _LastReserved =                                         \
           PNE3200_RESERVED_FROM_PACKET(_Adapter->LastFinishTransmit);           \
        _LastReserved->Next = (Packet);                                          \
        _Reserved->STAGE.BackPointer = _Adapter->LastFinishTransmit;             \
    } else {                                                                     \
        _Reserved->STAGE.BackPointer = NULL;                                     \
    }                                                                            \
    _Reserved->STAGE.STAGE4.ReadyToComplete = TRUE;                              \
    _Reserved->Next = NULL;                                                      \
    _Adapter->LastFinishTransmit = (Packet);                                     \
    if (!_Adapter->FirstFinishTransmit) {                                        \
        _Adapter->FirstFinishTransmit = (Packet);                                \
    }                                                                            \
    _Adapter->TransmitsQueued--;                                                 \
}

extern
VOID
NE3200GetStationAddress(
    IN PNE3200_ADAPTER Adapter
    );

extern
VOID
NE3200StopChip(
    IN PNE3200_ADAPTER Adapter,
    IN BOOLEAN SynchronizeWithInterrupt
    );

extern
BOOLEAN
NE3200StartAdapters(
    IN NDIS_HANDLE NdisMacHandle
    );

extern
BOOLEAN
NE3200RegisterAdapter(
    IN NDIS_HANDLE NdisMacHandle,
    IN PNDIS_STRING DeviceName,
    IN UINT EisaSlot,
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN UINT InterruptVector,
    IN NDIS_INTERRUPT_MODE InterruptMode,
    IN PUCHAR CurrentAddress,
    IN UINT MaximumMulticastAddresses,
    IN UINT MaximumOpenAdapters,
    IN BOOLEAN ConfigError,
    IN NDIS_STATUS ConfigErrorCode
    );

extern
BOOLEAN
NE3200InitializeMacBin(
    VOID
    );

extern
VOID
NE3200DestroyMacBin(
    VOID
    );

extern
NDIS_PHYSICAL_ADDRESS
NE3200LockMacBin(
    IN PNE3200_ADAPTER Adapter
    );

extern
VOID
NE3200UnlockMacBin(
    IN PNE3200_ADAPTER Adapter
    );

extern
BOOLEAN
NE3200Isr(
    IN PVOID Context
    );

extern
VOID
NE3200StandardInterruptDpc(
    IN PKDPC Dpc,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

extern
BOOLEAN
NE3200InterruptSynch(
    IN PVOID Context
    );

extern
BOOLEAN
NE3200AcquireCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    OUT PNE3200_SUPER_COMMAND_BLOCK * CommandBlock
    );

extern
VOID
NE3200AcquirePublicCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    OUT PNE3200_SUPER_COMMAND_BLOCK * CommandBlock
    );

extern
VOID
NE3200RelinquishCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_COMMAND_BLOCK CommandBlock
    );

extern
VOID
NE3200SubmitCommandBlock(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_COMMAND_BLOCK CommandBlock
    );

extern
VOID
NE3200StartChip(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_SUPER_RECEIVE_ENTRY FirstReceiveEntry
    );

extern
VOID
NE3200DoAdapterReset(
    IN PNE3200_ADAPTER Adapter
    );

extern
VOID
NE3200SetupForReset(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open
    );

extern
NDIS_STATUS
NE3200Request(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );

extern
VOID
NE3200WakeUpDpc(
    IN PVOID SystemArgument1,
    IN PVOID Context,
    IN PVOID SystemArgument2,
    IN PVOID SystemArgument3
    );

#if DBG

VOID
NE3200DumpMemory(
    IN PVOID VirtualPseudoAddress,
    IN ULONG DisplayStartingAddress,
    IN ULONG Length,
    IN PSZ AddressPrefix
    );

VOID
NE3200DumpVirtualMemory(
    IN PVOID VirtualAddress,
    IN ULONG Length
    );

VOID
NE3200DumpPhysicalMemory(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG Length
    );

VOID
NE3200DumpPacket(
    IN PNDIS_PACKET Packet
    );
#endif // DBG

VOID
NE3200UpdateStatistics(
    IN PNE3200_ADAPTER Adapter
    );

STATIC
NDIS_STATUS
NE3200QueryGlobalStatistics(
    NDIS_HANDLE MacAdapterContext,
    PNDIS_REQUEST NdisRequest
    );

STATIC
NDIS_STATUS
NE3200CloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    );

STATIC
NDIS_STATUS
NE3200UpdateMulticastTable(
    IN PNE3200_ADAPTER Adapter,
    IN PNE3200_OPEN Open,
    IN UINT CurrentAddressCount,
    IN CHAR CurrentAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN BOOLEAN Set
    );

VOID
NE3200ResetVariables(
    IN PNE3200_ADAPTER Adapter
    );

STATIC
VOID
NE3200ProcessInterrupt(
    IN PVOID SystemArgument1,
    IN PVOID InterruptContext,
    IN PVOID SystemArgument2,
    IN PVOID SystemArgument3
    );

extern
BOOLEAN
SyncNE3200ClearDoorbellInterrupt(
    IN PVOID SyncContext
    );

VOID
NE3200ResetHandler(
    IN PVOID SystemSpecific1,
    IN PNE3200_ADAPTER Adapter,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );


#endif  //_NE3200PROC_
