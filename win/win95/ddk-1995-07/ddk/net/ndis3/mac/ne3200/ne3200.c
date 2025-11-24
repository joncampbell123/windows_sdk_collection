/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    ne3200.c

Abstract:

    This is the main file for the Novell NE3200 EISA Ethernet adapter.
    This driver conforms to the NDIS 3.0 interface.

Environment:

    Kernel Mode.

--*/


//
// So we can trace things...
//
#define STATIC

#include <ne3200sw.h>
#include <macbin.h>

//
// Remove this when the Mythical Configuration Manager is available . . .
//

// but we want to use it later........
#define FAKE_CONFIGURATION_MANAGER

#ifdef  FAKE_CONFIGURATION_MANAGER
#include "eisa.h"
#endif  // FAKE_CONFIGURATION_MANAGER


//
// Global data block.  This structure has a spinlock (Lock)
// for protecting critical data.
//
NE3200_GLOBAL_DATA NE3200Globals;
BOOLEAN FirstAdd = TRUE;

//
// Debugging flags.
//
ULONG NE3200Debug = 0; // NE3200_DEBUG_LOUD | NE3200_DEBUG_VERY_LOUD;
ULONG NE3200MaxDumpSize = 192;

#if DBG
UCHAR Ne3200Log[256];
UCHAR Ne3200LogSave[10][256];
UCHAR Ne3200LogSavePlace = 0;
UCHAR Ne3200LogPlace = 0;
#endif

#ifndef i386

//
// On non-x86 systems, we hardcode the following as the config
// port value. It sets up edge-triggered interrupts and interrupt
// 10.
//

#define NON_X86_INIT_PORT_VALUE  0x23

#endif


STATIC
NDIS_STATUS
NE3200OpenAdapter(
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

extern
VOID
NE3200QueueRequest(
    IN PNE3200_ADAPTER Adapter,
    IN PNDIS_REQUEST NdisRequest
    );

STATIC
NDIS_STATUS
NE3200Reset(
    IN NDIS_HANDLE MacBindingHandle
    );

STATIC
VOID
NE3200UnloadMac(
    IN NDIS_HANDLE MacMacContext
    );

STATIC
NDIS_STATUS
NE3200AddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    );

STATIC
VOID
NE3200RemoveAdapter(
    IN PVOID MacAdapterContext
    );

extern
NDIS_STATUS
NE3200ChangeAddresses(
    IN UINT OldAddressCount,
    IN CHAR OldAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN UINT NewAddressCount,
    IN CHAR NewAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

STATIC
NDIS_STATUS
NE3200ChangeClass(
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );


STATIC
VOID
NE3200CloseAction(
    IN NDIS_HANDLE MacBindingHandle
    );

STATIC
BOOLEAN
NE3200AllocateAdapterMemory(
    IN PNE3200_ADAPTER Adapter
    );

STATIC
VOID
NE3200DeleteAdapterMemory(
    IN PNE3200_ADAPTER Adapter
    );

STATIC
BOOLEAN
NE3200InitialInit(
    IN PNE3200_ADAPTER Adapter,
    IN UINT NE3200InterruptVector,
    IN NDIS_INTERRUPT_MODE NE3200InterruptMode
    );

STATIC
BOOLEAN
NE3200InitializeGlobals(
    OUT PNE3200_GLOBAL_DATA Globals,
    IN NDIS_HANDLE AdapterHandle
    );

STATIC
VOID
NE3200DestroyGlobals(
    IN PNE3200_GLOBAL_DATA Globals
    );


//
// Non portable interface.
//
#ifdef NDIS_WIN
    #pragma ICODE
#endif


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This is the primary initialization routine for the NE3200 driver.
    It is simply responsible for the intializing the wrapper and registering
    the MAC.  It then calls a system and architecture specific routine that
    will initialize and register each adapter.

Arguments:

    DriverObject - Pointer to driver object created by the system.

Return Value:

    The status of the operation.

--*/

{


    //
    // Receives the status of the NdisRegisterMac operation.
    //
    NDIS_STATUS Status;

    static NDIS_STRING MacName = NDIS_STRING_CONST("NE3200");
    CHAR Tmp[sizeof(NDIS_MAC_CHARACTERISTICS)];
    PNDIS_MAC_CHARACTERISTICS NE3200Char = (PNDIS_MAC_CHARACTERISTICS)Tmp;

    NDIS_HANDLE NdisWrapperHandle;
    
#if NDIS_WIN
    UCHAR pIds[sizeof (EISA_MCA_ADAPTER_IDS) + sizeof (ULONG)];
#endif

#if NDIS_WIN
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nEisaAdapters=1;
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nMcaAdapters=0;
    *(PULONG)(((PEISA_MCA_ADAPTER_IDS)pIds)->IdArray)=EISA_NE3200_IDENTIFICATION;
    (PVOID)DriverObject=(PVOID)pIds;
#endif    

    //
    // Initialize the wrapper.
    //

    NdisInitializeWrapper(
                &NdisWrapperHandle,
                DriverObject,
                RegistryPath,
                NULL
                );

    //
    // Initialize the MAC characteristics for the call to
    // NdisRegisterMac.
    //

    NE3200Globals.NE3200NdisWrapperHandle = NdisWrapperHandle;
    NE3200Globals.NE3200DriverObject = DriverObject;

    NE3200Char->MajorNdisVersion = NE3200_NDIS_MAJOR_VERSION;
    NE3200Char->MinorNdisVersion = NE3200_NDIS_MINOR_VERSION;
    NE3200Char->OpenAdapterHandler = NE3200OpenAdapter;
    NE3200Char->CloseAdapterHandler = NE3200CloseAdapter;
    NE3200Char->SendHandler = NE3200Send;
    NE3200Char->RequestHandler = NE3200Request;
    NE3200Char->TransferDataHandler = NE3200TransferData;
    NE3200Char->ResetHandler = NE3200Reset;
    NE3200Char->QueryGlobalStatisticsHandler = NE3200QueryGlobalStatistics;
    NE3200Char->UnloadMacHandler = NE3200UnloadMac;
    NE3200Char->AddAdapterHandler = NE3200AddAdapter;
    NE3200Char->RemoveAdapterHandler = NE3200RemoveAdapter;
    NE3200Char->Name = MacName;

    NdisRegisterMac(
        &Status,
        &NE3200Globals.NE3200MacHandle,
        NdisWrapperHandle,
        NULL,
        NE3200Char,
        sizeof(*NE3200Char)
        );

    if (Status == NDIS_STATUS_SUCCESS) {

        return STATUS_SUCCESS;

    }

    //
    // We can only get here if something went wrong with registering
    // the mac or *all* of the adapters.
    //

    NE3200DestroyGlobals(&NE3200Globals);
    NdisTerminateWrapper(NdisWrapperHandle, NULL);
    return STATUS_UNSUCCESSFUL;

}



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
    )

/*++

Routine Description:

    This routine (and its interface) are not portable.  They are
    defined by the OS and the architecture.

    This routine is responsible for the allocation of the datastructures
    for the driver as well as any hardware specific details necessary
    to talk with the device.

Arguments:

    NdisMacHandle - The handle given back to the mac from ndis when
    the mac registered itself.

    DeviceName - Unicode string with the adapter name.  This has to
    be allocated by the caller.

    EisaSlot - The EISA Slot Number for this NE3200 adapter.

    WrapperConfigurationContext - Context passed to MacAddAdapter, to be
    passed to NdisRegisterAdapter.

    InterruptVector - The interrupt vector to used for the adapter.

    InterruptMode - The interrupt mode (Latched or LevelSensitive)
    used for this adapter.

    CurrentAddress - The address the card will assume. If this is NULL,
    then the card will use the BIA.

    MaximumMulticastAddresses - The maximum number of multicast
    addresses to filter at any one time.

    MaximumOpenAdapters - The maximum number of opens at any one time.

    ConfigError - Was there a configuration error

    ConfigErrorCode - The NDIS_ERROR_CODE to log as the error.

Return Value:

    Returns false if anything occurred that prevents the initialization
    of the adapter.

--*/

{


    //
    // Status of nt calls
    //
    NDIS_STATUS Status;

    //
    // Pointer for the adapter root.
    //
    PNE3200_ADAPTER Adapter;

    //
    // All of the code that manipulates physical addresses depends
    // on the fact that physical addresses are 4 byte quantities.
    //

    ASSERT(sizeof(NE3200_PHYSICAL_ADDRESS) == 4);

    //
    // Allocate the Adapter block.
    //

    NE3200_ALLOC_PHYS(&Status, &Adapter, sizeof(NE3200_ADAPTER));

    if ( Status == NDIS_STATUS_SUCCESS ) {

        PNDIS_ADAPTER_INFORMATION AdapterInformation;
        PUCHAR AdapterIoBase;
        PUCHAR MappedIoBase;

        NE3200_ALLOC_PHYS(&Status,
                          &AdapterInformation,
                          sizeof(NDIS_ADAPTER_INFORMATION) +
                          sizeof(NDIS_PORT_DESCRIPTOR)
                         );

        if ( Status != NDIS_STATUS_SUCCESS ) {

            NE3200_FREE_PHYS(Adapter);

            return(FALSE);

        }

        NdisZeroMemory(
            Adapter,
            sizeof(NE3200_ADAPTER)
            );

        NdisZeroMemory(
            AdapterInformation,
            sizeof(NDIS_ADAPTER_INFORMATION) +
            sizeof(NDIS_PORT_DESCRIPTOR)
            );

        Adapter->NdisMacHandle = NdisMacHandle;

        AdapterIoBase = (PUCHAR)(EisaSlot << 12);


        //
        // Set the port addresses.
        //

        //
        // Register the adapter.
        //

        AdapterInformation->Master = TRUE;
        AdapterInformation->DmaChannel = 0;   // not used for EISA master
        AdapterInformation->AdapterType = NdisInterfaceEisa;
        AdapterInformation->PhysicalMapRegistersNeeded =
                                NE3200_NUMBER_OF_COMMAND_BLOCKS *
                                NE3200_MAXIMUM_BLOCKS_PER_PACKET;

        AdapterInformation->MaximumPhysicalMapping =
                                MAXIMUM_ETHERNET_PACKET_SIZE;

        AdapterInformation->NumberOfPortDescriptors = 2;
        AdapterInformation->PortDescriptors[0].InitialPort = (ULONG)AdapterIoBase;
        AdapterInformation->PortDescriptors[0].NumberOfPorts = 0x4;
        AdapterInformation->PortDescriptors[0].PortOffset =
                             (PVOID *)&(Adapter->ResetPort);
        AdapterInformation->PortDescriptors[1].InitialPort =
                             (ULONG)(AdapterIoBase + NE3200_ID_PORT);
        AdapterInformation->PortDescriptors[1].NumberOfPorts = 0x20;
        AdapterInformation->PortDescriptors[1].PortOffset =
                             (PVOID *)&(MappedIoBase);

        if (NdisRegisterAdapter(
            &Adapter->NdisAdapterHandle,
            Adapter->NdisMacHandle,
            Adapter,
            WrapperConfigurationContext,
            DeviceName,
            AdapterInformation
            ) != NDIS_STATUS_SUCCESS) {

            NE3200_FREE_PHYS(Adapter);
            return FALSE;

        }

#ifndef i386

        //
        // on non-x86 machines, assume that we have to write
        // the config data out ourselves.
        //

        NdisRawWritePortUchar(
            (ULONG)(Adapter->ResetPort + 0x800),
            NON_X86_INIT_PORT_VALUE
            );

#endif

        if (ConfigError) {

            NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    ConfigErrorCode,
                    1,
                    registerAdapter
                    );

            NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

            NE3200_FREE_PHYS(Adapter);

            return(FALSE);

        }

        //
        // Should an alternative address be used?
        //

        if (CurrentAddress != NULL) {
            Adapter->AddressChanged = TRUE;
            ETH_COPY_NETWORK_ADDRESS(
                        Adapter->CurrentAddress,
                        CurrentAddress
                        );
        } else {
            Adapter->AddressChanged = FALSE;
        }

        Adapter->EisaSlot = EisaSlot;

        //
        // ResetPort is set by NdisRegisterAdapter.
        // MappedIoBase is set in NdisRegisterAdapter to be the mapped
        //   of NE3200_ID_PORT.  Now we set the other ports based on
        //   this offset.
        //

        Adapter->SystemInterruptPort = MappedIoBase + NE3200_SYSTEM_INTERRUPT_PORT - NE3200_ID_PORT;
        Adapter->LocalDoorbellInterruptPort = MappedIoBase + NE3200_LOCAL_DOORBELL_INTERRUPT_PORT - NE3200_ID_PORT;
        Adapter->SystemDoorbellMaskPort = MappedIoBase + NE3200_SYSTEM_DOORBELL_MASK_PORT - NE3200_ID_PORT;
        Adapter->SystemDoorbellInterruptPort = MappedIoBase + NE3200_SYSTEM_DOORBELL_INTERRUPT_PORT - NE3200_ID_PORT;
        Adapter->BaseMailboxPort = MappedIoBase + NE3200_BASE_MAILBOX_PORT - NE3200_ID_PORT;

        //
        // Allocate the global resources if needed.
        //

        if (FirstAdd) {
            if (!NE3200InitializeGlobals(&NE3200Globals, Adapter->NdisAdapterHandle)) {

                NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                NE3200_FREE_PHYS(Adapter);
                return FALSE;

            }
            FirstAdd = FALSE;
        }


        //
        // Allocate memory for all of the adapter structures.
        //

        Adapter->NumberOfTransmitBuffers = NE3200_NUMBER_OF_TRANSMIT_BUFFERS;
        Adapter->NumberOfReceiveBuffers  = NE3200_NUMBER_OF_RECEIVE_BUFFERS;
        Adapter->NumberOfCommandBlocks = NE3200_NUMBER_OF_COMMAND_BLOCKS;

        if (NE3200AllocateAdapterMemory(Adapter)) {

            InitializeListHead(&Adapter->OpenBindings);
            InitializeListHead(&Adapter->CloseList);
            NdisAllocateSpinLock(&Adapter->Lock);

            Adapter->References = 1;
            // Adapter->ProcessingRequests = FALSE;
            // Adapter->ProcessingDeferredOperations = FALSE;
            Adapter->BeingRemoved = FALSE;
            Adapter->FirstOpen = TRUE;

            // Adapter->CurrentPacketFilter = 0;

            // Adapter->GoodTransmits = 0;
            // Adapter->GoodReceives = 0;

            // Adapter->RetryFailure = 0;
            // Adapter->LostCarrier = 0;
            // Adapter->UnderFlow = 0;
            // Adapter->NoClearToSend = 0;
            // Adapter->Deferred = 0;
            // Adapter->OneRetry = 0;
            // Adapter->MoreThanOneRetry = 0;

            // Adapter->CrcErrors = 0;
            // Adapter->AlignmentErrors = 0;
            // Adapter->OutOfResources = 0;
            // Adapter->DmaOverruns = 0;

            // Adapter->ResetInProgress = FALSE;
            // Adapter->ResettingOpen = NULL;

            NE3200ResetVariables(Adapter);

            NdisInitializeTimer(
                &Adapter->DeferredTimer,
                (PVOID) NE3200ProcessInterrupt,
                (PVOID) Adapter
                );

            NdisInitializeTimer(
                &Adapter->WakeUpTimer,
                (PVOID) NE3200WakeUpDpc,
                (PVOID) Adapter
                );

            NdisInitializeTimer(
                &Adapter->ResetTimer,
                (PVOID) NE3200ResetHandler,
                (PVOID) Adapter
                );

            Adapter->FilterDB = NULL;

            //
            // We start this at TRUE so that we do not try to indicate any
            // packets before we have created the FilterDB
            //
            Adapter->DoingProcessing = TRUE;

            if (!NE3200InitialInit(
                                Adapter,
                                InterruptVector,
                                InterruptMode
                                )) {

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
                    0
                    );

                NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                NdisFreeSpinLock(&Adapter->Lock);
                NE3200DeleteAdapterMemory(Adapter);
                NE3200_FREE_PHYS(Adapter);
                return FALSE;

            }

            if (!EthCreateFilter(
                     MaximumMulticastAddresses,
                     NE3200ChangeAddresses,
                     NE3200ChangeClass,
                     NE3200CloseAction,
                     Adapter->CurrentAddress,
                     &Adapter->Lock,
                     &Adapter->FilterDB
                     )) {

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                    2,
                    registerAdapter,
                    NE3200_ERRMSG_INIT_DB
                    );


                NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                NdisFreeSpinLock(&Adapter->Lock);
                NE3200DeleteAdapterMemory(Adapter);
                NE3200_FREE_PHYS(Adapter);
                return FALSE;

            }

            //
            // GO!
            //
            Adapter->DoingProcessing = FALSE;

            //
            // Enable further interrupts.
            //

            NE3200_WRITE_SYSTEM_DOORBELL_MASK(
                Adapter,
                NE3200_SYSTEM_DOORBELL_MASK
                );

            //
            // Record it in the global adapter list.
            //

            NdisInterlockedInsertTailList(&NE3200Globals.AdapterList,
                                              &Adapter->AdapterList,
                                              &NE3200Globals.Lock
                                             );

            NdisRegisterAdapterShutdownHandler(
                    Adapter->NdisAdapterHandle,
                    (PVOID)Adapter,
                    NE3200Shutdown
                    );

            return(TRUE);

        } else {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                2,
                registerAdapter,
                NE3200_ERRMSG_ALLOC_MEM
                );

        }

    } else {

        return FALSE;

    }

}


STATIC
BOOLEAN
NE3200AllocateAdapterMemory(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine allocates memory for:

    - Configuration Block

    - Command Queue

    - Receive Queue

    - Receive Buffers

    - Transmit Buffers for use if user transmit buffers don't meet hardware
      contraints

    - Structures to map Command Queue entries back to the packets.

Arguments:

    Adapter - The adapter to allocate memory for.

Return Value:

    Returns FALSE if some memory needed for the adapter could not
    be allocated.

--*/

{

    //
    // Pointer to a Receive Entry.  Used while initializing
    // the Receive Queue.
    //
    PNE3200_SUPER_RECEIVE_ENTRY CurrentReceiveEntry;

    //
    // Pointer to a Command Block.  Used while initializing
    // the Command Queue.
    //
    PNE3200_SUPER_COMMAND_BLOCK CurrentCommandBlock;

    //
    // Simple iteration variable.
    //
    UINT i;

    //
    // Status of allocation
    //
    NDIS_STATUS Status;

    //
    // Allocate the public command block
    //

    NdisAllocateSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_SUPER_COMMAND_BLOCK),
                FALSE,
                (PVOID *) &Adapter->PublicCommandBlock,
                &Adapter->PublicCommandBlockPhysical
                );

    if (Adapter->PublicCommandBlock == NULL) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            1
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;

    }

    //
    // Allocate the card multicast table
    //

    NdisAllocateSharedMemory(
                Adapter->NdisAdapterHandle,
                NE3200_SIZE_OF_MULTICAST_TABLE_ENTRY *
                NE3200_MAXIMUM_MULTICAST,
                FALSE,                                  // noncached
                (PVOID*)&Adapter->CardMulticastTable,
                &Adapter->CardMulticastTablePhysical
                );

    if (Adapter->CardMulticastTable == NULL) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            2
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;

    }

    //
    // Allocate the Configuration Block.
    //

    NdisAllocateSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_CONFIGURATION_BLOCK),
                FALSE,                                  // noncached
                (PVOID*)&Adapter->ConfigurationBlock,
                &Adapter->ConfigurationBlockPhysical
                );

    if (Adapter->ConfigurationBlock == NULL) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            3
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // Allocate the Padding Buffer used to pad very short
    // packets to the minimum Ethernet packet size (60 bytes).
    //
    NdisAllocateSharedMemory(
                Adapter->NdisAdapterHandle,
                MINIMUM_ETHERNET_PACKET_SIZE,
                FALSE,
                (PVOID*)&Adapter->PaddingVirtualAddress,
                &Adapter->PaddingPhysicalAddress
                );

    if (Adapter->PaddingVirtualAddress == NULL) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            4
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;

    }

    //
    // Zero the Padding Buffer so we don't pad with garbage.
    //
    NdisZeroMemory(
        Adapter->PaddingVirtualAddress,
        MINIMUM_ETHERNET_PACKET_SIZE
        );


    //
    // Allocate the Command Queue.
    //

    NdisAllocateSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_SUPER_COMMAND_BLOCK) *
                    Adapter->NumberOfCommandBlocks,
                FALSE,
                (PVOID*)&Adapter->CommandQueue,
                &Adapter->CommandQueuePhysical
                );


    if (!Adapter->CommandQueue) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            5
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    NdisZeroMemory(
        Adapter->CommandQueue,
        sizeof(NE3200_SUPER_COMMAND_BLOCK) * Adapter->NumberOfCommandBlocks
        );

    //
    // Put the Command Blocks into a known state.
    //

    for(
        i = 0, CurrentCommandBlock = Adapter->CommandQueue;
        i < Adapter->NumberOfCommandBlocks;
        i++, CurrentCommandBlock++
        ) {

        CurrentCommandBlock->Hardware.State = NE3200_STATE_FREE;
        CurrentCommandBlock->Hardware.NextPending = NE3200_NULL;

        CurrentCommandBlock->NextCommand = NULL;

        NdisSetPhysicalAddressHigh (CurrentCommandBlock->Self, 0);
        NdisSetPhysicalAddressLow(
            CurrentCommandBlock->Self,
            NdisGetPhysicalAddressLow(Adapter->CommandQueuePhysical) +
                i * sizeof(NE3200_SUPER_COMMAND_BLOCK));

        CurrentCommandBlock->OwningOpenBinding = NULL;
        CurrentCommandBlock->CommandBlockIndex = (USHORT)i;
        CurrentCommandBlock->Timeout = FALSE;
    }


    //
    // Allocate Flush Buffer Pool
    //

    NdisAllocateBufferPool(
                    &Status,
                    (PVOID*)&Adapter->FlushBufferPoolHandle,
                    NE3200_NUMBER_OF_TRANSMIT_BUFFERS +
                    Adapter->NumberOfReceiveBuffers
                    );

    if (Status != NDIS_STATUS_SUCCESS) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            6
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // Allocate the Receive Queue.
    //

    NdisAllocateSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_SUPER_RECEIVE_ENTRY) *
                Adapter->NumberOfReceiveBuffers,
                FALSE,
                (PVOID*)&Adapter->ReceiveQueue,
                &Adapter->ReceiveQueuePhysical
                );

    if (!Adapter->ReceiveQueue) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            7
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;

    }

    NdisZeroMemory(
        Adapter->ReceiveQueue,
        sizeof(NE3200_SUPER_RECEIVE_ENTRY) * Adapter->NumberOfReceiveBuffers
        );


    //
    // Allocate the receive buffers and attach them to the Receive
    // Queue entries.
    //

    for(
        i = 0, CurrentReceiveEntry = Adapter->ReceiveQueue;
        i < Adapter->NumberOfReceiveBuffers;
        i++, CurrentReceiveEntry++
        ) {

        NdisAllocateSharedMemory(
                    Adapter->NdisAdapterHandle,
                    NE3200_SIZE_OF_RECEIVE_BUFFERS,
                    TRUE,
                    &CurrentReceiveEntry->ReceiveBuffer,
                    &CurrentReceiveEntry->ReceiveBufferPhysical
                    );

        if (!CurrentReceiveEntry->ReceiveBuffer) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                3,
                allocateAdapterMemory,
                NE3200_ERRMSG_ALLOC_MEM,
                8
                );

            NE3200DeleteAdapterMemory(Adapter);
            return FALSE;

        }

        //
        // Build flush buffers
        //

        NdisAllocateBuffer(
                    &Status,
                    &CurrentReceiveEntry->FlushBuffer,
                    Adapter->FlushBufferPoolHandle,
                    CurrentReceiveEntry->ReceiveBuffer,
                    NE3200_SIZE_OF_RECEIVE_BUFFERS
                    );

        if (Status != NDIS_STATUS_SUCCESS) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                3,
                allocateAdapterMemory,
                NE3200_ERRMSG_ALLOC_MEM,
                9
                );

            NE3200DeleteAdapterMemory(Adapter);
            return FALSE;
        }

        //
        // Initialize receive buffers
        //

        NdisFlushBuffer(CurrentReceiveEntry->FlushBuffer, FALSE);

        CurrentReceiveEntry->Hardware.State = NE3200_STATE_FREE;
        CurrentReceiveEntry->Hardware.FrameSize =
                NE3200_SIZE_OF_RECEIVE_BUFFERS;
        CurrentReceiveEntry->Hardware.NextPending =
                NdisGetPhysicalAddressLow(Adapter->ReceiveQueuePhysical) +
                (i + 1) * sizeof(NE3200_SUPER_RECEIVE_ENTRY);

        CurrentReceiveEntry->Hardware.BufferDescriptor.BlockLength =
                NE3200_SIZE_OF_RECEIVE_BUFFERS;
        CurrentReceiveEntry->Hardware.BufferDescriptor.PhysicalAddress =
                NdisGetPhysicalAddressLow(CurrentReceiveEntry->ReceiveBufferPhysical);

        NdisSetPhysicalAddressHigh (CurrentReceiveEntry->Self, 0);
        NdisSetPhysicalAddressLow(
            CurrentReceiveEntry->Self,
            NdisGetPhysicalAddressLow(Adapter->ReceiveQueuePhysical) +
                i * sizeof(NE3200_SUPER_RECEIVE_ENTRY));

        CurrentReceiveEntry->NextEntry = CurrentReceiveEntry + 1;

    }

    //
    // Make sure the last entry is properly terminated.
    //

    (CurrentReceiveEntry - 1)->Hardware.NextPending = NE3200_NULL;
    (CurrentReceiveEntry - 1)->NextEntry = Adapter->ReceiveQueue;

    //
    // Allocate the array of buffer descriptors.
    //

    NE3200_ALLOC_PHYS(
        &Status,
        &Adapter->NE3200Buffers,
        sizeof(NE3200_BUFFER_DESCRIPTOR)*
        (NE3200_NUMBER_OF_TRANSMIT_BUFFERS)
        );

    if (Status != NDIS_STATUS_SUCCESS) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            3,
            allocateAdapterMemory,
            NE3200_ERRMSG_ALLOC_MEM,
            0xA
            );

        NE3200DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // Zero the memory of all the descriptors so that we can
    // know which buffers weren't allocated incase we can't allocate
    // them all.
    //

    NdisZeroMemory(
        Adapter->NE3200Buffers,
        sizeof(NE3200_BUFFER_DESCRIPTOR)*
         (NE3200_NUMBER_OF_TRANSMIT_BUFFERS)
        );


    //
    // Allocate each of the buffers and fill in the
    // buffer descriptor.
    //

    Adapter->NE3200BufferListHead = 0;

    for (
        i = 0;
        i < NE3200_NUMBER_OF_TRANSMIT_BUFFERS;
        i++
        ) {



        NdisAllocateSharedMemory(
                    Adapter->NdisAdapterHandle,
                    NE3200_SIZE_OF_TRANSMIT_BUFFERS,
                    TRUE,
                    &Adapter->NE3200Buffers[i].VirtualNE3200Buffer,
                    &Adapter->NE3200Buffers[i].PhysicalNE3200Buffer
                    );

        if (!Adapter->NE3200Buffers[i].VirtualNE3200Buffer) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                3,
                allocateAdapterMemory,
                NE3200_ERRMSG_ALLOC_MEM,
                0xB
                );

            NE3200DeleteAdapterMemory(Adapter);
            return FALSE;

        }

        //
        // Build flush buffers
        //

        NdisAllocateBuffer(
                    &Status,
                    &Adapter->NE3200Buffers[i].FlushBuffer,
                    Adapter->FlushBufferPoolHandle,
                    &Adapter->NE3200Buffers[i].VirtualNE3200Buffer,
                    NE3200_SIZE_OF_TRANSMIT_BUFFERS
                    );

        if (Status != NDIS_STATUS_SUCCESS) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                3,
                allocateAdapterMemory,
                NE3200_ERRMSG_ALLOC_MEM,
                0xC
                );

            NE3200DeleteAdapterMemory(Adapter);
            return FALSE;
        }

        Adapter->NE3200Buffers[i].Next = i+1;
        Adapter->NE3200Buffers[i].BufferSize = NE3200_SIZE_OF_TRANSMIT_BUFFERS;

    }

    //
    // Make sure that the last buffer correctly terminates the free list.
    //

    Adapter->NE3200Buffers[i-1].Next = -1;

    return TRUE;

}
#ifdef NDIS_WIN
    #pragma LCODE
#endif


STATIC
VOID
NE3200DeleteAdapterMemory(
    IN PNE3200_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine deallocates memory for:

    - Command Queue.

    - Receive Queue.

    - Receive buffers

    - Transmit Buffers for use if user transmit buffers don't meet hardware
      contraints

    - Structures to map transmit ring entries back to the packets.

Arguments:

    Adapter - The adapter to deallocate memory for.

Return Value:

    None.

--*/

{

    if (Adapter->PublicCommandBlock) {
        NdisFreeSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_SUPER_COMMAND_BLOCK),
                FALSE,
                Adapter->PublicCommandBlock,
                Adapter->PublicCommandBlockPhysical
                );
    }


    if (Adapter->CardMulticastTable) {
        NdisFreeSharedMemory(
                Adapter->NdisAdapterHandle,
                NE3200_SIZE_OF_MULTICAST_TABLE_ENTRY *
                NE3200_MAXIMUM_MULTICAST,
                FALSE,
                Adapter->CardMulticastTable,
                Adapter->CardMulticastTablePhysical
                );

    }

    if (Adapter->ConfigurationBlock) {

        NdisFreeSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_CONFIGURATION_BLOCK),
                FALSE,
                Adapter->ConfigurationBlock,
                Adapter->ConfigurationBlockPhysical
                );

    }

    if (Adapter->PaddingVirtualAddress) {

        NdisFreeSharedMemory(
                Adapter->NdisAdapterHandle,
                MINIMUM_ETHERNET_PACKET_SIZE,
                FALSE,
                Adapter->PaddingVirtualAddress,
                Adapter->PaddingPhysicalAddress
                );

    }

    if (Adapter->CommandQueue) {
        NdisFreeSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_SUPER_COMMAND_BLOCK) *
                    Adapter->NumberOfCommandBlocks,
                FALSE,
                Adapter->CommandQueue,
                Adapter->CommandQueuePhysical
                );




    if (Adapter->ReceiveQueue) {

        //
        // Pointer to current Receive Entry being deallocated.
        //
        PNE3200_SUPER_RECEIVE_ENTRY CurrentReceiveEntry;

        //
        // Simple iteration counter.
        //
        UINT i;


        for(
            i = 0, CurrentReceiveEntry = Adapter->ReceiveQueue;
            i < Adapter->NumberOfReceiveBuffers;
            i++, CurrentReceiveEntry++
            ) {


            if (CurrentReceiveEntry->ReceiveBuffer) {
                NdisFreeSharedMemory(
                    Adapter->NdisAdapterHandle,
                    NE3200_SIZE_OF_RECEIVE_BUFFERS,
                    TRUE,
                    CurrentReceiveEntry->ReceiveBuffer,
                    CurrentReceiveEntry->ReceiveBufferPhysical
                    );


                if (CurrentReceiveEntry->FlushBuffer) {
                    NdisFreeBuffer(
                        CurrentReceiveEntry->FlushBuffer
                        );
                }
            }

        }

        NdisFreeSharedMemory(
                Adapter->NdisAdapterHandle,
                sizeof(NE3200_SUPER_RECEIVE_ENTRY) *
                Adapter->NumberOfReceiveBuffers,
                FALSE,
                Adapter->ReceiveQueue,
                Adapter->ReceiveQueuePhysical
                );

    }



    if (Adapter->NE3200Buffers) {

        UINT i;

        for (
            i = 0;
            i < NE3200_NUMBER_OF_TRANSMIT_BUFFERS;
            i++
            ) {


            if (Adapter->NE3200Buffers[i].VirtualNE3200Buffer) {
                NdisFreeSharedMemory(
                    Adapter->NdisAdapterHandle,
                    NE3200_SIZE_OF_TRANSMIT_BUFFERS,
                    TRUE,
                    Adapter->NE3200Buffers[i].VirtualNE3200Buffer,
                    Adapter->NE3200Buffers[i].PhysicalNE3200Buffer
                    );

                if (Adapter->NE3200Buffers[i].FlushBuffer)
                    NdisFreeBuffer(
                        Adapter->NE3200Buffers[i].FlushBuffer
                        );
                }
            }

        // fix for 3498    
        NE3200_FREE_PHYS(Adapter->NE3200Buffers);

        }

    }


    if (Adapter->FlushBufferPoolHandle) {
        NdisFreeBufferPool(
                    Adapter->FlushBufferPoolHandle
                    );
    }

}

#ifdef NDIS_WIN
    #pragma ICODE
#endif

STATIC
NDIS_STATUS
NE3200OpenAdapter(
    OUT PNDIS_STATUS OpenErrorStatus,
    OUT NDIS_HANDLE *MacBindingHandle,
    OUT PUINT SelectedMediumIndex,
    IN PNDIS_MEDIUM MediumArray,
    IN UINT MediumArraySize,
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacAdapterContext,
    IN UINT OpenOptions,
    IN PSTRING AddressingInformation OPTIONAL
    )

/*++

Routine Description:

    This routine is used to create an open instance of an adapter, in effect
    creating a binding between an upper-level module and the MAC module over
    the adapter.

Arguments:

    MacBindingHandle - A pointer to a location in which the MAC stores
    a context value that it uses to represent this binding.

    SelectedMediumIndex - Index in MediumArray of the medium type that
        the MAC wishes to be viewed as.
    MediumArray - Array of medium types which a protocol supports.
    MediumArraySize - Number of elements in MediumArray.

    NdisBindingContext - A value to be recorded by the MAC and passed as
    context whenever an indication is delivered by the MAC for this binding.

    MacAdapterContext - The value associated with the adapter that is being
    opened when the MAC registered the adapter with NdisRegisterAdapter.

    OpenOptions - bit mask.

    AddressingInformation - An optional pointer to a variable length string
    containing hardware-specific information that can be used to program the
    device.  (This is not used by this MAC.)

Return Value:

    The function value is the status of the operation.  If the MAC does not
    complete this request synchronously, the value would be
    NDIS_STATUS_PENDING.


--*/

{


    //
    // The NE3200_ADAPTER that this open binding should belong too.
    //
    PNE3200_ADAPTER Adapter;

    //
    // for index
    //

    UINT i;

    //
    // return status
    //

    NDIS_STATUS Status;

    //
    // Pointer to the space allocated for the binding.
    //
    PNE3200_OPEN NewOpen;

    //
    // Pointer to the reserved portion of ndisrequest
    //
    PNE3200_REQUEST_RESERVED Reserved;

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;
    OpenErrorStatus; AddressingInformation; OpenOptions;

    //
    // If we are being removed, don't allow new opens.
    //

    Adapter = PNE3200_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    if (Adapter->BeingRemoved) {
        return NDIS_STATUS_ADAPTER_REMOVED;
    }


    //
    // Search for the medium type (802.3)
    //

    for (i = 0; i < MediumArraySize; i++){

        if (MediumArray[i] == NdisMedium802_3){

            break;

        }

    }

    if (i == MediumArraySize){

        return(NDIS_STATUS_UNSUPPORTED_MEDIA);

    }

    *SelectedMediumIndex = i;




    NdisInterlockedAddUlong(&Adapter->References, 1, &Adapter->Lock);

    //
    // Allocate the space for the open binding.  Fill in the fields.
    //

    NE3200_ALLOC_PHYS(&Status, &NewOpen, sizeof(NE3200_OPEN));

    if (Status != NDIS_STATUS_SUCCESS) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            0
            );

        NdisAcquireSpinLock(&Adapter->Lock);

        StatusToReturn = NDIS_STATUS_RESOURCES;
        goto OpenDoDeferred;

    }

    *MacBindingHandle = BINDING_HANDLE_FROM_PNE3200_OPEN(NewOpen);
    InitializeListHead(&NewOpen->OpenList);
    NewOpen->NdisBindingContext = NdisBindingContext;
    NewOpen->References = 1;
    NewOpen->BindingShuttingDown = FALSE;
    NewOpen->OwningAdapter = Adapter;
    NewOpen->ProtOptionFlags = 0;

    NewOpen->OpenCloseRequest.RequestType = NdisRequestOpen;
    Reserved = PNE3200_RESERVED_FROM_REQUEST(&NewOpen->OpenCloseRequest);
    Reserved->OpenBlock = NewOpen;
    Reserved->Next = (PNDIS_REQUEST)NULL;

    NdisAcquireSpinLock(&Adapter->Lock);

    NE3200QueueRequest(Adapter, &NewOpen->OpenCloseRequest);

    StatusToReturn = NDIS_STATUS_PENDING;

    //
    // Fire off the timer for the next state.
    //

    if (Adapter->FirstOpen && !Adapter->BeingRemoved) {

        Adapter->FirstOpen = FALSE;

        NdisSetTimer(
             &Adapter->WakeUpTimer,
             NE3200_TIMEOUT_COMMAND
             );
    }

OpenDoDeferred:
    NE3200_DO_DEFERRED(Adapter);
    return StatusToReturn;
}
#ifdef NDIS_WIN
    #pragma LCODE
#endif

STATIC
NDIS_STATUS
NE3200CloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    This routine causes the MAC to close an open handle (binding).

Arguments:

    MacBindingHandle - The context value returned by the MAC when the
    adapter was opened.  In reality it is a PNE3200_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{
    //
    // NE3200 Adapter this open belongs to
    //
    PNE3200_ADAPTER Adapter;

    //
    // Pointer to the space allocated for the binding
    //
    PNE3200_OPEN Open;

    //
    // Status to return to the caller
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    Adapter = PNE3200_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the lock while we update the reference counts for the
    // adapter and the open.
    //

    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    Open = PNE3200_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Don't do anything if it's closing
    //

    if (!Open->BindingShuttingDown) {

        PNE3200_REQUEST_RESERVED Reserved = PNE3200_RESERVED_FROM_REQUEST(&Open->OpenCloseRequest);

        Open->OpenCloseRequest.RequestType = NdisRequestClose;
        Reserved->OpenBlock = Open;
        Reserved->Next = (PNDIS_REQUEST)NULL;

        Open->References++;

        NE3200QueueRequest(Adapter, &Open->OpenCloseRequest);

        //
        // Remove the creation reference.
        //

        Open->References--;

        StatusToReturn = NDIS_STATUS_PENDING;

    } else {

        StatusToReturn = NDIS_STATUS_CLOSING;

    }

    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    NE3200_DO_DEFERRED(Adapter);
    return StatusToReturn;

}

#ifdef NDIS_WIN
    #pragma ICODE
#endif

STATIC
BOOLEAN
NE3200InitializeGlobals(
    OUT PNE3200_GLOBAL_DATA Globals,
    IN NDIS_HANDLE AdapterHandle
    )

/*++

Routine Description:

    This routine will initialize the global data structure used
    by all adapters managed by this driver.  This routine is only
    called once, when the driver is initializing the first
    adapter.

Arguments:

    Globals - Pointer to the global data structure to initialize.

    AdapterHandle - The handle to be used to allocate shared memory.

Return Value:

    None.

--*/

{
    //
    // Allocate our SpinLock for protecting this structure.
    //
    NdisAllocateSpinLock(&Globals->Lock);

    //
    // Allocate the buffer.
    //

    Globals->MacBinAdapterHandle = AdapterHandle;

    NdisAllocateSharedMemory(
                AdapterHandle,
                NE3200_MACBIN_LENGTH,
                FALSE,
                &Globals->MacBinVirtualAddress,
                &Globals->MacBinPhysicalAddress
                );

    if (Globals->MacBinVirtualAddress == NULL) {
        NE3200DestroyGlobals(Globals);
        return FALSE;
    }

    Globals->MacBinLength = NE3200_MACBIN_LENGTH;

    NE3200_MOVE_MEMORY(
                Globals->MacBinVirtualAddress,
                NE3200MacBinImage,
                NE3200_MACBIN_LENGTH
                );

    //
    // AdapterListHead is initially empty
    //
    InitializeListHead(&Globals->AdapterList);

    //
    // Everything's cool!
    //
    return TRUE;

}
#ifdef NDIS_WIN
    #pragma LCODE
#endif

STATIC
VOID
NE3200DestroyGlobals(
    IN PNE3200_GLOBAL_DATA Globals
    )

/*++

Routine Description:

    This routine frees all global data allocated with
    NE3200InitializeGlobals.  This routine is called just before
    the driver is unloaded.

Arguments:

    Globals - Pointer to the global data structure to destroy.

Return Value:

    None.

--*/

{

    //
    // Make sure there's only one thread executing this.
    //

    if (Globals->MacBinVirtualAddress != NULL) {
        NdisFreeSharedMemory(
                Globals->MacBinAdapterHandle,
                NE3200_MACBIN_LENGTH,
                FALSE,
                Globals->MacBinVirtualAddress,
                Globals->MacBinPhysicalAddress
                );
    }

    NdisFreeSpinLock(&Globals->Lock);

}


extern
VOID
NE3200UnloadMac(
    IN NDIS_HANDLE MacMacContext
    )

/*++

Routine Description:

    NE3200Unload is called when the MAC is to unload itself.

Arguments:

    None.

Return Value:

    None.

--*/

{
    NDIS_STATUS InitStatus;
    PNE3200_ADAPTER Adapter;

    UNREFERENCED_PARAMETER(MacMacContext);

    NdisDeregisterMac(
            &InitStatus,
            NE3200Globals.NE3200MacHandle
            );

    NdisFreeSpinLock(&NE3200Globals.Lock);

    NdisTerminateWrapper(
            NE3200Globals.NE3200NdisWrapperHandle,
            NULL
            );

    NE3200DestroyGlobals(&NE3200Globals);

    return;
}

#ifdef NDIS_WIN
    #pragma ICODE
#endif

extern
NDIS_STATUS
NE3200AddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    )

/*++

Routine Description:

    NE3200AddAdapter adds an adapter to the list supported
    by this MAC.

Arguments:


Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING

--*/

{
    NDIS_HANDLE ConfigHandle;
    NDIS_STATUS Status;
    PNDIS_CONFIGURATION_PARAMETER ReturnedValue;
    NDIS_STRING NetAddrStr = NDIS_STRING_CONST("NETADDRESS");
    UCHAR NetworkAddress[ETH_LENGTH_OF_ADDRESS];
    PUCHAR CurrentAddress;
    NDIS_INTERRUPT_MODE InterruptType;
    UINT InterruptVector;
    UINT EisaSlot;
    USHORT Portz800;
    UCHAR z800Value;
    UCHAR Mask;
    UCHAR InitType;
    UCHAR PortValue;
    USHORT PortAddress;
    PUCHAR CurrentChar;
    BOOLEAN LastEntry;
    NDIS_EISA_FUNCTION_INFORMATION EisaData;
    PVOID NetAddress;
    ULONG Length;

#ifndef i386
    //
    // For non-x86 machines, until we can put EISA config data
    // in the registry, assume these values. This sets up
    // edge-triggered interrupts, thicknet, interrupt 10.
    //

    static UCHAR MipsEisaData[4] = { 0x00, 0x00, 0x08, NON_X86_INIT_PORT_VALUE };

    // The port number is put into this nibble. ---^
#endif


    BOOLEAN ConfigError = FALSE;
    NDIS_STATUS ConfigErrorCode;

    NdisOpenConfiguration(
                    &Status,
                    &ConfigHandle,
                    ConfigurationHandle
                    );

    if (Status != NDIS_STATUS_SUCCESS) {
        return NDIS_STATUS_FAILURE;
    }

    CurrentAddress = NULL;

    //
    // Read net address
    //

    NdisReadNetworkAddress(
                    &Status,
                    &NetAddress,
                    &Length,
                    ConfigHandle
                    );

    if ((Length == ETH_LENGTH_OF_ADDRESS) && (Status == NDIS_STATUS_SUCCESS)) {

        ETH_COPY_NETWORK_ADDRESS(
                NetworkAddress,
                NetAddress
                );

        CurrentAddress = NetworkAddress;

    }

    NdisReadEisaSlotInformation(
                            &Status,
                            ConfigurationHandle,
                            &EisaSlot,
                            &EisaData
                            );

    if (Status != NDIS_STATUS_SUCCESS) {

        ConfigError = TRUE;
        ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

        goto RegisterAdapter;

    }

#ifdef i386

    CurrentChar = EisaData.InitializationData;

#else

    //
    // Write the slot number into the high nibble of the third byte
    // of fake EISA config data.
    //

    MipsEisaData[2] |= (EisaSlot << 4);
    CurrentChar = MipsEisaData;

#if DBG
    DbgPrint ("NE3200: EISA data is %x %x %x %x\n",
        CurrentChar[0],
        CurrentChar[1],
        CurrentChar[2],
        CurrentChar[3]);
#endif

#endif

    Portz800 = (EisaSlot << 12) + 0x800;

    LastEntry = FALSE;
    while (!LastEntry) {
        InitType = *(CurrentChar++);
        PortAddress = *((USHORT UNALIGNED *) CurrentChar++);
        CurrentChar++;

        if ((InitType & 0x80) == 0) {
            LastEntry = TRUE;
        }

        if (PortAddress != Portz800) {
            continue;
        }

        PortValue = *(CurrentChar++);

        if (InitType & 0x40) {
            Mask = *(CurrentChar++);
        } else {
            Mask = 0;
        }

        z800Value &= Mask;
        z800Value |= PortValue;
    }

    //
    // Now, interpret the port data
    //


    //
    // get interrupt
    //

    switch (z800Value & 0x07) {
    case 0x00:

        ConfigError = TRUE;
        ConfigErrorCode = NDIS_ERROR_CODE_HARDWARE_FAILURE;

        goto RegisterAdapter;

    case 0x01:
        InterruptVector = 5;
        break;
    case 0x02:
        InterruptVector = 9;
        break;
    case 0x03:
        InterruptVector = 10;
        break;
    case 0x04:
        InterruptVector = 11;
        break;
    case 0x05:
        InterruptVector = 15;
        break;
    default:

        ConfigError = TRUE;
        ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

        goto RegisterAdapter;

    }

    //
    // get interrupt mode
    //

    if (z800Value & 0x20) {
        InterruptType = NdisInterruptLatched;
    } else {
        InterruptType = NdisInterruptLevelSensitive;
    }

RegisterAdapter:

    if (NE3200RegisterAdapter(
                            NE3200Globals.NE3200MacHandle,
                            AdapterName,
                            EisaSlot,
                            ConfigurationHandle,
                            InterruptVector,
                            InterruptType,
                            CurrentAddress,
                            NE3200_MAXIMUM_MULTICAST,
                            32,
                            ConfigError,
                            ConfigErrorCode
                            )) {

        Status = NDIS_STATUS_SUCCESS;

    } else {

        Status = NDIS_STATUS_FAILURE;

    }

    NdisCloseConfiguration(ConfigHandle);

    return Status;
}
#ifdef NDIS_WIN
    #pragma LCODE
#endif

extern
VOID
NE3200RemoveAdapter(
    IN NDIS_HANDLE MacAdapterContext
    )

/*++

Routine Description:

    NE3200RemoveAdapter removes an adapter previously registered
    with NdisRegisterAdapter.

Arguments:

    MacAdapterContext - The context value that the MAC passed
        to NdisRegisterAdapter; actually as pointer to a
        NE3200_ADAPTER.

Return Value:

    None.

--*/

{
    PNE3200_ADAPTER Adapter;
    BOOLEAN Cancelled;

    Adapter = PNE3200_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    //
    // There are no opens left, so remove ourselves.
    //

    NdisDeregisterAdapterShutdownHandler(Adapter->NdisAdapterHandle);

    NdisCancelTimer(&Adapter->WakeUpTimer,&Cancelled);

    NdisStallExecution(2500000);

    EthDeleteFilter(Adapter->FilterDB);
    NdisRemoveInterrupt(&Adapter->Interrupt);

    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
    NdisFreeSpinLock(&Adapter->Lock);

    NdisAcquireSpinLock(&NE3200Globals.Lock);
    RemoveEntryList(&Adapter->AdapterList);
    NdisReleaseSpinLock(&NE3200Globals.Lock);

    NE3200_FREE_PHYS(Adapter);

    return;
}


VOID
NE3200Shutdown(
    IN PVOID ShutdownContext
    )

/*++

Routine Description:

    Turns off the card during a powerdown of the system.

Arguments:

    ShutdownContext - Really a pointer to the adapter structure.

Return Value:

    None.

--*/

{
    PNE3200_ADAPTER Adapter = (PNE3200_ADAPTER)(ShutdownContext);
    UINT i;

    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // Set the flag
    //

    Adapter->BeingRemoved = TRUE;

    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete.
    //

    NE3200StopChip(Adapter, TRUE);

    NdisStallExecution(250000);

                //
                // Unfortunately, a hardware reset to the NE3200 does *not*
                // reset the BMIC chip.  To ensure that we read a proper status,
                // we'll clear all of the BMIC's registers.
                //

                NE3200_WRITE_SYSTEM_INTERRUPT(
                    Adapter,
                    0
                    );

                //
                // I changed this to ff since the original 0 didn't work for
                // some cases. since we don't have the specs....
                //

                NE3200_WRITE_LOCAL_DOORBELL_INTERRUPT(
                    Adapter,
                    0xff
                    );

                NE3200_WRITE_SYSTEM_DOORBELL_MASK(
                    Adapter,
                    0
                    );

                NE3200_SYNC_CLEAR_SYSTEM_DOORBELL_INTERRUPT(
                    Adapter
                    );

                for (i = 0 ; i < 16 ; i += 4 ) {

                    NE3200_WRITE_MAILBOX_ULONG(
                        Adapter,
                        i,
                        0L
                        );

                }

                //
                // Toggle the NE3200's reset line.
                //

                NE3200_WRITE_RESET(
                    Adapter,
                    NE3200_RESET_BIT_ON
                    );

    NdisReleaseSpinLock(&Adapter->Lock);

}
