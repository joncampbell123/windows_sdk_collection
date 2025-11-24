/*++

Copyright (c) 1990-1995  Microsoft Corporation

Module Name:

    sonic.c

Abstract:

    This is the main file for the National Semiconductor SONIC
    Ethernet controller.  This driver conforms to the NDIS 3.0 interface.

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



//
// This variable is used to control debug output.
//

#if DBG
INT SonicDbg = 0;
#endif


STATIC
VOID
SonicShutdown(
    IN PVOID ShutdownContext
    );

STATIC
NDIS_STATUS
SonicOpenAdapter(
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

STATIC
NDIS_STATUS
SonicCloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    );


STATIC
VOID
SonicUnload(
    IN NDIS_HANDLE MacMacContext
    );

STATIC
NDIS_STATUS
SonicAddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    );

STATIC
VOID
SonicRemoveAdapter(
    IN NDIS_HANDLE MacAdapterContext
    );

STATIC
NDIS_STATUS
SonicReset(
    IN NDIS_HANDLE MacBindingHandle
    );

#if 0
STATIC
UINT
CalculateCRC(
    IN UINT NumberOfBytes,
    IN PCHAR Input
    );
#endif

STATIC
BOOLEAN
SonicSynchClearIsr(
    IN PVOID Context
    );

STATIC
VOID
SonicStopChip(
    IN PSONIC_ADAPTER Adapter
    );

STATIC
BOOLEAN
SetupRegistersAndInit(
    IN PSONIC_ADAPTER Adapter
    );

STATIC
BOOLEAN
SonicInitialInit(
    IN PSONIC_ADAPTER Adapter
    );


NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );


STATIC
NDIS_STATUS
SonicRegisterAdapter(
    IN NDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING DeviceName,
    IN PUCHAR NetworkAddress,
    IN UCHAR AdapterType,
    IN UINT SlotNumber,
    IN UINT Controller,
    IN UINT MultifunctionAdapter,
    IN UINT SonicInterruptVector,
    IN UINT SonicInterruptLevel,
    IN NDIS_INTERRUPT_MODE SonicInterruptMode,
    IN UINT MaximumOpenAdapters
    );

typedef enum {
    SonicHardwareOk,
    SonicHardwareChecksum,
    SonicHardwareConfig
} SONIC_HARDWARE_STATUS;

STATIC
SONIC_HARDWARE_STATUS
SonicHardwareGetDetails(
    IN PSONIC_ADAPTER Adapter,
    IN UINT SlotNumber,
    IN UINT Controller,
    IN UINT MultifunctionAdapter,
    OUT PULONG InitialPort,
    OUT PULONG NumberOfPorts,
    IN OUT PUINT InterruptVector,
    IN OUT PUINT InterruptLevel,
    OUT ULONG ErrorLogData[3]
    );

STATIC
BOOLEAN
SonicHardwareGetAddress(
    IN PSONIC_ADAPTER Adapter,
    OUT ULONG ErrorLogData[3]
    );

#ifdef SONIC_INTERNAL

//
// These routines are support reading the address for the
// sonic internal implementation on the R4000 motherboards.
//

STATIC
NTSTATUS
SonicHardwareSaveInformation(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    );

STATIC
BOOLEAN
SonicHardwareVerifyChecksum(
    IN PSONIC_ADAPTER Adapter,
    IN PUCHAR EthernetAddress,
    OUT ULONG ErrorLogData[3]
    );

#endif


//
// Use the alloc_text pragma to specify the driver initialization routines
// (they can be paged out).
//

#ifdef ALLOC_PRAGMA
#pragma alloc_text(init,DriverEntry)
#pragma alloc_text(init,SonicRegisterAdapter)
#pragma alloc_text(init,SonicInitialInit)
#pragma alloc_text(init,SonicAddAdapter)
#pragma alloc_text(init,SonicHardwareGetDetails)
#pragma alloc_text(init,SonicHardwareGetAddress)
#ifdef SONIC_INTERNAL
#pragma alloc_text(init,SonicHardwareSaveInformation)
#pragma alloc_text(init,SonicHardwareVerifyChecksum)
#endif
#endif



NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This is the primary initialization routine for the sonic driver.
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

    PSONIC_MAC SonicMac;

    NDIS_HANDLE SonicWrapperHandle;

    static const NDIS_STRING MacName = NDIS_STRING_CONST("SONIC");
    NDIS_MAC_CHARACTERISTICS SonicChar;

#if NDIS_WIN
    UCHAR pIds[sizeof (EISA_MCA_ADAPTER_IDS) + sizeof (ULONG)];
#endif

#if NDIS_WIN
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nEisaAdapters=1;
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nMcaAdapters=0;
    *(PULONG)(((PEISA_MCA_ADAPTER_IDS)pIds)->IdArray)=SONIC_COMPRESSED_ID;
    (PVOID) DriverObject = (PVOID) pIds;
#endif    


    //
    // Initialize the wrapper.
    //

    NdisInitializeWrapper(
                &SonicWrapperHandle,
                DriverObject,
                RegistryPath,
                NULL
                );

    //
    // Now allocate memory for our global structure.
    //

    SONIC_ALLOC_MEMORY(&Status, &SonicMac, sizeof(SONIC_MAC));

    if (Status != NDIS_STATUS_SUCCESS) {

         return NDIS_STATUS_RESOURCES;

    }

    SonicMac->WrapperHandle = SonicWrapperHandle;

    //
    // Initialize the MAC characteristics for the call to
    // NdisRegisterMac.
    //

    SonicChar.MajorNdisVersion = SONIC_NDIS_MAJOR_VERSION;
    SonicChar.MinorNdisVersion = SONIC_NDIS_MINOR_VERSION;
    SonicChar.OpenAdapterHandler = SonicOpenAdapter;
    SonicChar.CloseAdapterHandler = SonicCloseAdapter;
    SonicChar.SendHandler = SonicSend;
    SonicChar.TransferDataHandler = SonicTransferData;
    SonicChar.ResetHandler = SonicReset;
    SonicChar.RequestHandler = SonicRequest;
    SonicChar.QueryGlobalStatisticsHandler = SonicQueryGlobalStatistics;
    SonicChar.UnloadMacHandler = SonicUnload;
    SonicChar.AddAdapterHandler = SonicAddAdapter;
    SonicChar.RemoveAdapterHandler = SonicRemoveAdapter;
    SonicChar.Name = MacName;


    NdisRegisterMac(
        &Status,
        &SonicMac->MacHandle,
        SonicWrapperHandle,
        (NDIS_HANDLE)SonicMac,                   // MacMacContext
        &SonicChar,
        sizeof(SonicChar)
        );

    if (Status == NDIS_STATUS_SUCCESS) {

        return NDIS_STATUS_SUCCESS;

    }


    //
    // We can only get here if something went wrong with registering
    // the mac or *all* of the adapters.
    //

    NdisTerminateWrapper(SonicWrapperHandle, NULL);

    return NDIS_STATUS_FAILURE;

}
#if 23
PVOID SonicAdapterAddress;
#endif

STATIC
NDIS_STATUS
SonicRegisterAdapter(
    IN NDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING DeviceName,
    IN PUCHAR NetworkAddress,
    IN UCHAR AdapterType,
    IN UINT SlotNumber,
    IN UINT Controller,
    IN UINT MultifunctionAdapter,
    IN UINT SonicInterruptVector,
    IN UINT SonicInterruptLevel,
    IN NDIS_INTERRUPT_MODE SonicInterruptMode,
    IN UINT MaximumOpenAdapters
    )

/*++

Routine Description:

    This routine (and its interface) are not portable.  They are
    defined by the OS, the architecture, and the particular SONIC
    implementation.

    This routine is responsible for the allocation of the datastructures
    for the driver as well as any hardware specific details necessary
    to talk with the device.

Arguments:

    NdisMacHandle - The handle given back to the mac from ndis when
    the mac registered itself.

    ConfigurationHandle - Config handle passed to MacAddAdapter.

    DeviceName - The string containing the name to give to the
    device adapter.

    NetworkAddress - The network address, or NULL if the default
    should be used.

    AdapterType - The type of the adapter; currently SONIC_ADAPTER_TYPE_EISA
    and SONIC_ADAPTER_TYPE_INTERNAL are supported,

    SlotNumber - The slot number for the EISA card.

    Controller - The controller number for INTERNAL chips.

    MultifunctionAdapter - The INTERNAL bus number for INTERNAL chips.

    SonicInterruptVector - The interrupt vector to use for the adapter.

    SonicInterruptLevel - The interrupt request level to use for this
    adapter.

    SonicInterruptMode - The interrupt mode to be use for this adapter.

    MaximumOpenAdapters - The maximum number of opens at any one time.

Return Value:

    Returns a failure status if anything occurred that prevents the
    initialization of the adapter.

--*/

{

    //
    // Pointer for the adapter root.
    //
    PSONIC_ADAPTER Adapter;

    //
    // Status of various NDIS calls.
    //
    NDIS_STATUS Status;

    //
    // Holds information needed when registering the adapter.
    //
    NDIS_ADAPTER_INFORMATION AdapterInformation;

    //
    // Number of ports needed
    //
    ULONG InitialPort;
    ULONG NumberOfPorts;

    //
    // Returned from SonicHardwareGetDetails; if it failed,
    // we log an error and exit.
    //
    SONIC_HARDWARE_STATUS HardwareDetailsStatus;

    //
    // Used to store error log data from SonicHardwareGetDetails.
    //
    ULONG ErrorLogData[3];

    //
    // We put in this assertion to make sure that ushort are 2 bytes.
    // if they aren't then the initialization block definition needs
    // to be changed.
    //
    // Also all of the logic that deals with status registers assumes
    // that control registers are only 2 bytes.
    //

    ASSERT(sizeof(USHORT) == 2);

    //
    // The Sonic uses four bytes four physical addresses, so we
    // must ensure that this is the case (SONIC_PHYSICAL_ADDRESS)
    // is defined as a ULONG).
    //

    ASSERT(sizeof(SONIC_PHYSICAL_ADDRESS) == 4);

    //
    // Allocate the Adapter block.
    //

    SONIC_ALLOC_MEMORY(&Status, &Adapter, sizeof(SONIC_ADAPTER));

    if (Status == NDIS_STATUS_SUCCESS) {
#if 23
        SonicAdapterAddress = Adapter;
#endif

        SONIC_ZERO_MEMORY(
            Adapter,
            sizeof(SONIC_ADAPTER)
            );


        Adapter->AdapterType = AdapterType;
        if (SonicInterruptMode == NdisInterruptLatched) {
            Adapter->InterruptLatched = TRUE;
        }
        Adapter->PermanentAddressValid = FALSE;

        //
        // Set up the AdapterInformation structure; zero it out
        // first using sizeof (in case any fields are added).
        //

        SONIC_ZERO_MEMORY (&AdapterInformation, sizeof(NDIS_ADAPTER_INFORMATION));
        AdapterInformation.DmaChannel = 0;
        AdapterInformation.Master = TRUE;
        AdapterInformation.PhysicalMapRegistersNeeded =
            SONIC_MAX_FRAGMENTS * SONIC_NUMBER_OF_TRANSMIT_DESCRIPTORS;
        AdapterInformation.MaximumPhysicalMapping =
                             SONIC_LARGE_BUFFER_SIZE;

        //
        // This returns the I/O ports used by the Sonic and may
        // modify SonicInterruptVector and SonicInterruptLevel,
        // as well as modiying some fields in Adapter.
        //

        if ((HardwareDetailsStatus =
              SonicHardwareGetDetails(
                 Adapter,
                 SlotNumber,
                 Controller,
                 MultifunctionAdapter,
                 &InitialPort,
                 &NumberOfPorts,
                 &SonicInterruptVector,
                 &SonicInterruptLevel,
                 ErrorLogData)) != SonicHardwareOk) {

            //
            // If it fails, we call NdisRegisterAdapter anyway
            // (so we can log an error using the NdisAdapterHandle),
            // then return failure.
            //

            AdapterInformation.NumberOfPortDescriptors = 0;

        } else {

            AdapterInformation.NumberOfPortDescriptors = 1;
            AdapterInformation.PortDescriptors[0].InitialPort = InitialPort;
            AdapterInformation.PortDescriptors[0].NumberOfPorts = NumberOfPorts;
            AdapterInformation.PortDescriptors[0].PortOffset = (PVOID *)(&(Adapter->SonicPortAddress));

        }

        if (AdapterType == SONIC_ADAPTER_TYPE_EISA) {
            AdapterInformation.AdapterType = NdisInterfaceEisa;
        } else {
            AdapterInformation.AdapterType = NdisInterfaceInternal;
        }

        //
        // Register the adapter with NDIS.
        //

        if ((Status = NdisRegisterAdapter(
                &Adapter->NdisAdapterHandle,
                NdisMacHandle,
                Adapter,
                ConfigurationHandle,
                DeviceName,
                &AdapterInformation
                )) == NDIS_STATUS_SUCCESS) {

            if (HardwareDetailsStatus != SonicHardwareOk) {

                //
                // Log an error and exit.
                //

                if (HardwareDetailsStatus == SonicHardwareChecksum) {

                    NdisWriteErrorLogEntry(
                        Adapter->NdisAdapterHandle,
                        NDIS_ERROR_CODE_NETWORK_ADDRESS,
                        6,
                        hardwareDetails,
                        SONIC_ERRMSG_HARDWARE_ADDRESS,
                        NDIS_STATUS_FAILURE,
                        ErrorLogData[0],
                        ErrorLogData[1],
                        ErrorLogData[2]
                        );

                } else {

                    NdisWriteErrorLogEntry(
                        Adapter->NdisAdapterHandle,
                        NDIS_ERROR_CODE_MISSING_CONFIGURATION_PARAMETER,
                        0
                        );

                }

                NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
                return NDIS_STATUS_FAILURE;

            }


            //
            // Allocate memory for all of the adapter structures.
            //

            Adapter->NumberOfTransmitDescriptors =
                                SONIC_NUMBER_OF_TRANSMIT_DESCRIPTORS;
            Adapter->NumberOfReceiveBuffers =
                                SONIC_NUMBER_OF_RECEIVE_BUFFERS;
            Adapter->NumberOfReceiveDescriptors =
                                SONIC_NUMBER_OF_RECEIVE_DESCRIPTORS;


            if (AllocateAdapterMemory(Adapter)) {

                //
                // Get the network address. This writes
                // an error log entry if it fails. This routine
                // may do nothing on some systems, if
                // SonicHardwareGetDetails has already determined
                // the network address.
                //

                if (!SonicHardwareGetAddress(Adapter, ErrorLogData)) {

                    NdisWriteErrorLogEntry(
                        Adapter->NdisAdapterHandle,
                        NDIS_ERROR_CODE_NETWORK_ADDRESS,
                        6,
                        hardwareDetails,
                        SONIC_ERRMSG_HARDWARE_ADDRESS,
                        NDIS_STATUS_FAILURE,
                        ErrorLogData[0],
                        ErrorLogData[1],
                        ErrorLogData[2]
                        );

                    DeleteAdapterMemory(Adapter);
                    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                    SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
                    return NDIS_STATUS_FAILURE;

                }

                //
                // Initialize the current hardware address.
                //

                SONIC_MOVE_MEMORY(
                    Adapter->CurrentNetworkAddress,
                    (NetworkAddress != NULL) ?
                        NetworkAddress :
                        Adapter->PermanentNetworkAddress,
                    ETH_LENGTH_OF_ADDRESS);


                InitializeListHead(&Adapter->OpenBindings);
                Adapter->OpenCount = 0;
                Adapter->Removed = FALSE;
                InitializeListHead(&Adapter->CloseList);
                NdisAllocateSpinLock(&Adapter->Lock);


                Adapter->LastTransmitDescriptor =
                            Adapter->TransmitDescriptorArea +
                            (Adapter->NumberOfTransmitDescriptors-1);
                Adapter->NumberOfAvailableDescriptors =
                            Adapter->NumberOfTransmitDescriptors;
                Adapter->AllocateableDescriptor =
                            Adapter->TransmitDescriptorArea;
                Adapter->TransmittingDescriptor =
                            Adapter->TransmitDescriptorArea;
                Adapter->FirstUncommittedDescriptor =
                            Adapter->TransmitDescriptorArea;
                Adapter->PacketsSinceLastInterrupt = 0;

                Adapter->CurrentReceiveBufferIndex = 0;
                Adapter->CurrentReceiveDescriptorIndex = 0;
                Adapter->LastReceiveDescriptor =
                            &Adapter->ReceiveDescriptorArea[
                                Adapter->NumberOfReceiveDescriptors-1];

                Adapter->InterruptMaskRegister = SONIC_INT_DEFAULT_VALUE;
                Adapter->ReceiveDescriptorsExhausted = FALSE;
                Adapter->ReceiveBuffersExhausted = FALSE;
                Adapter->ReceiveControlRegister = SONIC_RCR_DEFAULT_VALUE;

                Adapter->ProcessingReceiveInterrupt = FALSE;
                Adapter->ProcessingGeneralInterrupt = FALSE;
                Adapter->ProcessingDeferredOperations = FALSE;
                Adapter->FirstLoopBack = NULL;
                Adapter->LastLoopBack = NULL;
                Adapter->FirstFinishTransmit = NULL;
                Adapter->LastFinishTransmit = NULL;
                Adapter->SendStageOpen = TRUE;
                Adapter->AlreadyProcessingSendStage = FALSE;
                Adapter->FirstSendStagePacket = NULL;
                Adapter->LastSendStagePacket = NULL;

                Adapter->References = 1;

                Adapter->ResetInProgress = FALSE;
                Adapter->IndicatingResetStart = FALSE;
                Adapter->IndicatingResetEnd = FALSE;
                Adapter->ResettingOpen = NULL;
                Adapter->FirstInitialization = TRUE;

                SONIC_ZERO_MEMORY (&Adapter->GeneralMandatory, GM_ARRAY_SIZE * sizeof(ULONG));
                SONIC_ZERO_MEMORY (&Adapter->GeneralOptionalByteCount, GO_COUNT_ARRAY_SIZE * sizeof(SONIC_LARGE_INTEGER));
                SONIC_ZERO_MEMORY (&Adapter->GeneralOptionalFrameCount, GO_COUNT_ARRAY_SIZE * sizeof(ULONG));
                SONIC_ZERO_MEMORY (&Adapter->GeneralOptional, (GO_ARRAY_SIZE - GO_ARRAY_START) * sizeof(ULONG));
                SONIC_ZERO_MEMORY (&Adapter->MediaMandatory, MM_ARRAY_SIZE * sizeof(ULONG));
                SONIC_ZERO_MEMORY (&Adapter->MediaOptional, MO_ARRAY_SIZE * sizeof(ULONG));

                //
                // Initialize the CAM and associated things.
                // At the beginning nothing is enabled since
                // our filter is 0, although we do store
                // our network address in the first slot.
                //

                Adapter->MulticastCamEnableBits = 0x0000;
                Adapter->CurrentPacketFilter = 0;
                Adapter->CamDescriptorArea->CamEnable = 0x0000;
                Adapter->CamDescriptorsUsed = 0x0001;
                Adapter->CamDescriptorAreaSize = 1;

                NdisInitializeTimer(
                    &Adapter->DeferredTimer,
                    SonicTimerProcess,
                    (PVOID)Adapter);

                SONIC_LOAD_CAM_FRAGMENT(
                    &Adapter->CamDescriptorArea->CamFragments[0],
                    0,
                    Adapter->CurrentNetworkAddress
                    );


                if (!EthCreateFilter(
                         SONIC_CAM_ENTRIES-1,    // maximum MC addresses
                         SonicChangeAddresses,
                         SonicChangeClass,
                         SonicCloseAction,
                         Adapter->CurrentNetworkAddress,
                         &Adapter->Lock,
                         &Adapter->FilterDB
                         )) {

                    NdisWriteErrorLogEntry(
                        Adapter->NdisAdapterHandle,
                        NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                        2,
                        registerAdapter,
                        SONIC_ERRMSG_CREATE_FILTER
                        );

                    DeleteAdapterMemory(Adapter);
                    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                    NdisFreeSpinLock(&Adapter->Lock);
                    SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
                    return NDIS_STATUS_RESOURCES;

                } else {

                    //
                    // Initialize the interrupt.
                    //

                    NdisInitializeInterrupt(
                        &Status,
                        &Adapter->Interrupt,
                        Adapter->NdisAdapterHandle,
                        SonicInterruptService,
                        Adapter,
                        SonicDeferredProcessing,
                        SonicInterruptVector,
                        SonicInterruptLevel,
                        TRUE,
                        SonicInterruptMode
                        );


                    if (Status != NDIS_STATUS_SUCCESS) {

                        NdisWriteErrorLogEntry(
                            Adapter->NdisAdapterHandle,
                            NDIS_ERROR_CODE_INTERRUPT_CONNECT,
                            2,
                            registerAdapter,
                            SONIC_ERRMSG_INIT_INTERRUPT
                            );

                        EthDeleteFilter(Adapter->FilterDB);
                        DeleteAdapterMemory(Adapter);
                        NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                        NdisFreeSpinLock(&Adapter->Lock);
                        SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
                        return Status;

                    }

                    //
                    // Start the card up. This writes an error
                    // log entry if it fails.
                    //

                    if (!SonicInitialInit(Adapter)) {

                        NdisRemoveInterrupt(&Adapter->Interrupt);
                        EthDeleteFilter(Adapter->FilterDB);
                        DeleteAdapterMemory(Adapter);
                        NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                        NdisFreeSpinLock(&Adapter->Lock);
                        SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
                        return NDIS_STATUS_FAILURE;

                    } else {

                        //
                        // Initialize the wake up timer to catch interrupts that
                        // don't complete. It fires continuously
                        // every 5 seconds, and we check if there are any
                        // uncompleted operations from the previous 5 second
                        // period.
                        //

                        Adapter->WakeUpDpc = (PVOID)SonicWakeUpDpc;

                        NdisInitializeTimer(&Adapter->WakeUpTimer,
                                            (PVOID)(Adapter->WakeUpDpc),
                                            Adapter );

                        NdisSetTimer(
                            &Adapter->WakeUpTimer,
                            5000
                            );

                        NdisRegisterAdapterShutdownHandler(
                            Adapter->NdisAdapterHandle,
                            (PVOID)Adapter,
                            SonicShutdown
                            );

                        return NDIS_STATUS_SUCCESS;

                    }

                }


            } else {

                //
                // Call to AllocateAdapterMemory failed.
                //

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                    2,
                    registerAdapter,
                    SONIC_ERRMSG_ALLOC_MEMORY
                    );

                DeleteAdapterMemory(Adapter);
                NdisDeregisterAdapter(Adapter->NdisAdapterHandle);
                SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
                return NDIS_STATUS_RESOURCES;

            }

        } else {

            //
            // Call to NdisRegisterAdapter failed.
            //

            SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));
            return Status;

        }

    } else {

        //
        // Couldn't allocate adapter object.
        //

        return Status;

    }

}

extern
BOOLEAN
SonicInitialInit(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine sets up the initial init of the driver.

Arguments:

    Adapter - The adapter for the hardware.

Return Value:

    None.

--*/

{

    UINT Time = 50;

    //
    // First we make sure that the device is stopped.
    //

    SonicStopChip(Adapter);

    //
    // Set up the registers.
    //

    if (!SetupRegistersAndInit(Adapter)) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
            3,
            registerAdapter,
            SONIC_ERRMSG_INITIAL_INIT
            );

        return FALSE;

    }


    //
    // Delay execution for 1/2 second to give the sonic
    // time to initialize.
    //

    while (Time > 0) {

        if (!Adapter->FirstInitialization) {
            break;
        }

        NdisStallExecution(10000);
        Time--;

    }


    //
    // The only way that first initialization could have
    // been turned off is if we actually initialized.
    //

    if (!Adapter->FirstInitialization) {

        //
        // We actually did get the initialization.
        //
        // We can start the chip.  We may not
        // have any bindings to indicate to but this
        // is unimportant.
        //

        SonicStartChip(Adapter);

        return TRUE;


    } else {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_TIMEOUT,
            2,
            registerAdapter,
            SONIC_ERRMSG_INITIAL_INIT
            );

        return FALSE;

    }

}

STATIC
BOOLEAN
SonicSynchClearIsr(
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is used during a reset. It ensures that no
    interrupts will come through, and that any DPRs that run
    will find no interrupts to process.

Arguments:

    Context - A pointer to a SONIC_ADAPTER structure.

Return Value:

    Always returns true.

--*/

{

    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)Context;

    SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_STATUS, 0xffff);
    Adapter->IsrValue = 0;

    return TRUE;

}

extern
VOID
SonicStartChip(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is used to start an already initialized sonic.

Arguments:

    Adapter - The adapter for the SONIC to start.

Return Value:

    None.

--*/

{

    //
    // Take us out of reset mode if we are in it.
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
        0x0000
        );

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
        SONIC_CR_RECEIVER_ENABLE
        );

}

STATIC
VOID
SonicStopChip(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is used to stop a sonic.

    This routine is *not* portable.  It is specific to the 386
    implementation of the sonic.  On the bus master card the ACON bit
    must be set in csr3, whereas on the decstation, csr3 remains clear.

Arguments:

    Adapter - The adapter for the SONIC to stop.

Return Value:

    None.

--*/

{

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
        SONIC_CR_RECEIVER_DISABLE |
        SONIC_CR_SOFTWARE_RESET
        );

}

extern
VOID
SonicStartCamReload(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine starts a CAM reload, which will cause an
    interrupt when it is done.

Arguments:

    Adapter - The adapter for the SONIC to reload.

Return Value:

    None.

--*/

{

    //
    // Move CAM Enable into the appropriate spot.
    //

    SONIC_LOAD_CAM_ENABLE(
        &Adapter->CamDescriptorArea->CamFragments[
                                        Adapter->CamDescriptorAreaSize],
        Adapter->CamDescriptorArea->CamEnable
        );


    //
    // Flush the CAM before we start the reload.
    //

    SONIC_FLUSH_WRITE_BUFFER(Adapter->CamDescriptorAreaFlushBuffer);


    SONIC_WRITE_PORT(Adapter, SONIC_CAM_DESCRIPTOR,
        SONIC_GET_LOW_PART_ADDRESS(Adapter->CamDescriptorAreaPhysical)
        );

    SONIC_WRITE_PORT(Adapter, SONIC_CAM_DESCRIPTOR_COUNT,
        (USHORT)Adapter->CamDescriptorAreaSize
        );


    //
    // Start the Load CAM, which will cause an interrupt
    // when it is done.
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
        SONIC_CR_LOAD_CAM
        );

}

STATIC
NDIS_STATUS
SonicOpenAdapter(
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

    OpenErrorStatus - Returns more information in some cases.

    MacBindingHandle - A pointer to a location in which the MAC stores
    a context value that it uses to represent this binding.

    SelectedMediumIndex - A pointer to a location in which the MAC stores
    the medium selected out of MediumArray.

    MediumArray - An array of media that the protocol can support.

    MediumArraySize - The number of elements in MediumArray.

    NdisBindingContext - A value to be recorded by the MAC and passed as
    context whenever an indication is delivered by the MAC for this binding.

    MacAdapterContext - The value associated with the adapter that is being
    opened when the MAC registered the adapter with NdisRegisterAdapter.

    OpenOptions - A bit mask containing flags with information about this
    binding.

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
    // The SONIC_ADAPTER that this open binding should belong too.
    //
    PSONIC_ADAPTER Adapter;

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    // Simple iteration variable, for scanning the medium array.
    //
    UINT i;

    //
    // Pointer to the space allocated for the binding.
    //
    PSONIC_OPEN NewOpen;

    //
    // Points to the MacReserved section of NewOpen->OpenCloseRequest.
    //
    PSONIC_REQUEST_RESERVED Reserved;


    //
    // If we are being removed, don't allow new opens.
    //

    Adapter = PSONIC_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    if (Adapter->Removed) {
        return NDIS_STATUS_FAILURE;
    }


    //
    // Search for the 802.3 media type
    //

    for (i=0; i<MediumArraySize; i++) {

        if (MediumArray[i] == NdisMedium802_3) {
            break;
        }

    }

    if (i == MediumArraySize) {

        return NDIS_STATUS_UNSUPPORTED_MEDIA;

    }

    *SelectedMediumIndex = i;


    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    NdisReleaseSpinLock(&Adapter->Lock);

    //
    // Allocate the space for the open binding.  Fill in the fields.
    //

    SONIC_ALLOC_MEMORY(&StatusToReturn, &NewOpen, sizeof(SONIC_OPEN));

    if (StatusToReturn == NDIS_STATUS_SUCCESS) {

        *MacBindingHandle = BINDING_HANDLE_FROM_PSONIC_OPEN(NewOpen);
        InitializeListHead(&NewOpen->OpenList);
        NewOpen->NdisBindingContext = NdisBindingContext;
        NewOpen->References = 1;
        NewOpen->BindingShuttingDown = FALSE;
        NewOpen->OwningSonic = Adapter;
        NewOpen->ProtOptionFlags = 0;

        NewOpen->OpenCloseRequest.RequestType = NdisRequestOpen;
        Reserved = PSONIC_RESERVED_FROM_REQUEST(&NewOpen->OpenCloseRequest);
        Reserved->OpenBlock = NewOpen;
        Reserved->Next = (PNDIS_REQUEST)NULL;

        NdisAcquireSpinLock(&Adapter->Lock);

        SonicQueueRequest(Adapter, &NewOpen->OpenCloseRequest);

        StatusToReturn = NDIS_STATUS_PENDING;

    } else {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            2,
            openAdapter,
            SONIC_ERRMSG_ALLOC_OPEN
            );


        NdisAcquireSpinLock(&Adapter->Lock);

    }


    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    SONIC_DO_DEFERRED(Adapter);
    return StatusToReturn;
}

STATIC
NDIS_STATUS
SonicCloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    This routine causes the MAC to close an open handle (binding).

Arguments:

    MacBindingHandle - The context value returned by the MAC when the
    adapter was opened.  In reality it is a PSONIC_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{

    //
    // The SONIC_ADAPTER that this open binding should belong too.
    //
    PSONIC_ADAPTER Adapter;

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    // Pointer to the space allocated for the binding.
    //
    PSONIC_OPEN Open;


    Adapter = PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the lock while we update the reference counts for the
    // adapter and the open.
    //

    NdisAcquireSpinLock(&Adapter->Lock);
    Adapter->References++;

    Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Don't do anything if this binding is already closing.
    //

    if (!Open->BindingShuttingDown) {

        PSONIC_REQUEST_RESERVED Reserved = PSONIC_RESERVED_FROM_REQUEST(&Open->OpenCloseRequest);

        Open->OpenCloseRequest.RequestType = NdisRequestClose;
        Reserved->OpenBlock = Open;
        Reserved->Next = (PNDIS_REQUEST)NULL;

        ++Open->References;

        SonicQueueRequest(Adapter, &Open->OpenCloseRequest);

        //
        // Remove the creation reference.
        //

        --Open->References;

        StatusToReturn = NDIS_STATUS_PENDING;

    } else {

        StatusToReturn = NDIS_STATUS_CLOSING;

    }

    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    SONIC_DO_DEFERRED(Adapter);
    return StatusToReturn;

}

STATIC
VOID
SonicUnload(
    IN NDIS_HANDLE MacMacContext
    )

/*++

Routine Description:

    SonicUnload is called when the MAC is to unload itself.

Arguments:

    None.

Return Value:

    None.

--*/

{

    NDIS_STATUS Status;

    PSONIC_MAC SonicMac = (PSONIC_MAC)MacMacContext;


    NdisDeregisterMac(
        &Status,
        SonicMac->MacHandle
        );

    NdisTerminateWrapper(
        SonicMac->WrapperHandle,
        NULL
        );

    return;

}


STATIC
NDIS_STATUS
SonicAddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    )

/*++

Routine Description:

    SonicAddAdapter adds an adapter to the list supported
    by this MAC.

Arguments:

    MacMacContext - The context passed to NdisRegisterMac (will be NULL).

    ConfigurationHandle - A handle to pass to NdisOpenConfiguration.

    AdapterName - The name to register with via NdisRegisterAdapter.

Return Value:

    NDIS_STATUS_SUCCESS
    NDIS_STATUS_PENDING

--*/

{

    NDIS_STATUS Status = NDIS_STATUS_SUCCESS;
    PSONIC_MAC SonicMac = (PSONIC_MAC)MacMacContext;
    NDIS_HANDLE ConfigHandle;
    NDIS_STRING AdapterTypeString = NDIS_STRING_CONST("AdapterType");
#ifdef SONIC_INTERNAL
    NDIS_STRING MultifunctionAdapterString = NDIS_STRING_CONST("MultifunctionAdapter");
    NDIS_STRING NetworkControllerString = NDIS_STRING_CONST("NetworkController");
#endif
    PNDIS_CONFIGURATION_PARAMETER ReturnedValue;
    PUCHAR NetworkAddress;
    UINT NetworkAddressLength;
    UCHAR AdapterType;
    UINT InterruptVector;
    UINT InterruptLevel;
    NDIS_INTERRUPT_MODE InterruptMode;
    UINT SlotNumber;
    UINT Controller = 0;
    UINT MultifunctionAdapter = 0;

    //
    // Open the configuration info.
    //

    NdisOpenConfiguration(
                    &Status,
                    &ConfigHandle,
                    ConfigurationHandle
                    );

    if (Status != NDIS_STATUS_SUCCESS) {
        return Status;
    }


    //
    // Check that adapter type is supported.
    // The default depends on the processor type.
    //

    AdapterType = SONIC_ADAPTER_TYPE_DEFAULT;

    NdisReadConfiguration(
                    &Status,
                    &ReturnedValue,
                    ConfigHandle,
                    &AdapterTypeString,
                    NdisParameterInteger
                    );

    if (Status == NDIS_STATUS_SUCCESS) {

        //
        // See if the adapter type is valid. We skip to AdapterTypeRecognized
        // if the AdapterType is known to this driver.
        //

#ifdef SONIC_EISA
        if (ReturnedValue->ParameterData.IntegerData == SONIC_ADAPTER_TYPE_EISA) {
            goto AdapterTypeRecognized;
        }
#endif

#ifdef SONIC_INTERNAL
        if (ReturnedValue->ParameterData.IntegerData == SONIC_ADAPTER_TYPE_INTERNAL) {
            goto AdapterTypeRecognized;
        }
#endif

        //
        // Card type not supported by this driver
        //

#if DBG
        DbgPrint("SONIC: Error in adapter type: %lx\n", ReturnedValue->ParameterData.IntegerData);
#endif
        NdisCloseConfiguration(ConfigHandle);
        return NDIS_STATUS_FAILURE;


AdapterTypeRecognized:

        AdapterType = (UCHAR)ReturnedValue->ParameterData.IntegerData;
    }

    switch (AdapterType) {

#ifdef SONIC_EISA

    case SONIC_ADAPTER_TYPE_EISA:
    {

        NDIS_EISA_FUNCTION_INFORMATION EisaData;
        USHORT Portzc88;
        UCHAR zc88Value;
        UCHAR Mask;
        UCHAR InitType;
        UCHAR PortValue;
        USHORT PortAddress;
        PUCHAR CurrentChar;
        BOOLEAN LastEntry;

        NdisReadEisaSlotInformation(
                                &Status,
                                ConfigurationHandle,
                                &SlotNumber,
                                &EisaData
                                );

        if (Status != NDIS_STATUS_SUCCESS) {

#if DBG
            DbgPrint("SONIC: Could not read EISA data\n");
#endif
            NdisCloseConfiguration(ConfigHandle);
            return NDIS_STATUS_FAILURE;

        }

        CurrentChar = EisaData.InitializationData;

        Portzc88 = (SlotNumber << 12) + 0xc88;

        LastEntry = FALSE;
        while (!LastEntry) {
            InitType = *(CurrentChar++);
            PortAddress = *((USHORT UNALIGNED *)CurrentChar);
            CurrentChar += sizeof(USHORT);

            if ((InitType & 0x80) == 0) {
                LastEntry = TRUE;
            }

            PortValue = *(CurrentChar++);

            if (InitType & 0x40) {
                Mask = *(CurrentChar++);
            } else {
                Mask = 0;
            }

            //
            // The only port we care about is zc88 (z is the
            // slot number) since it has the interrupt in it.
            //

            if (PortAddress != Portzc88) {
                continue;
            }

            zc88Value &= Mask;
            zc88Value |= PortValue;

        }

        switch ((zc88Value & 0x06) >> 1) {
        case 0:
            InterruptVector = 5; break;
        case 1:
            InterruptVector = 9; break;
        case 2:
            InterruptVector = 10; break;
        case 3:
            InterruptVector = 11; break;
        }

        InterruptLevel = InterruptVector;

        if ((zc88Value & 0x01) != 0) {
            InterruptMode = NdisInterruptLatched;
        } else {
            InterruptMode = NdisInterruptLevelSensitive;
        }

        break;

    }

#endif  // SONIC_EISA

#ifdef SONIC_INTERNAL

    case SONIC_ADAPTER_TYPE_INTERNAL:
    {

        //
        // For the internal adapter, we read the MultifunctionAdapter number
        // and NetworkController number, which are both optional. For
        // passing to SonicRegisterAdapter.
        //

        NdisReadConfiguration(
                        &Status,
                        &ReturnedValue,
                        ConfigHandle,
                        &MultifunctionAdapterString,
                        NdisParameterInteger
                        );

        if (Status == NDIS_STATUS_SUCCESS) {

            MultifunctionAdapter =  ReturnedValue->ParameterData.IntegerData;

        }

        NdisReadConfiguration(
                        &Status,
                        &ReturnedValue,
                        ConfigHandle,
                        &NetworkControllerString,
                        NdisParameterInteger
                        );

        if (Status == NDIS_STATUS_SUCCESS) {

            Controller =  ReturnedValue->ParameterData.IntegerData;

        }

        //
        // These are filled in by SonicHardwareGetDetails.
        //

        InterruptVector = 0;
        InterruptLevel = 0;

        //
        // The internal adapter is level-sensitive.
        //

        InterruptMode = NdisInterruptLevelSensitive;

        break;

    }

#endif  // SONIC_INTERNAL

    default:

        ASSERT(FALSE);
        break;

    }


    //
    // Read network address
    //

    NdisReadNetworkAddress(
        &Status,
        (PVOID *)&NetworkAddress,
        &NetworkAddressLength,
        ConfigHandle);


    //
    // Make sure that the address is the right length asnd
    // at least one of the bytes is non-zero.
    //

    if ((Status == NDIS_STATUS_SUCCESS) &&
        (NetworkAddressLength == ETH_LENGTH_OF_ADDRESS) &&
        ((NetworkAddress[0] |
          NetworkAddress[1] |
          NetworkAddress[2] |
          NetworkAddress[3] |
          NetworkAddress[4] |
          NetworkAddress[5]) != 0)) {

#if DBG
        if (SonicDbg) {
            DbgPrint("SONIC: New Address = %.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n",
                                    NetworkAddress[0],
                                    NetworkAddress[1],
                                    NetworkAddress[2],
                                    NetworkAddress[3],
                                    NetworkAddress[4],
                                    NetworkAddress[5]);
        }
#endif

    } else {

        //
        // Tells SonicRegisterAdapter to use the
        // burned-in address.
        //

        NetworkAddress = NULL;

    }

    //
    // Used passed-in adapter name to register.
    //

    Status = SonicRegisterAdapter(
                 SonicMac->MacHandle,
                 ConfigurationHandle,
                 AdapterName,
                 NetworkAddress,
                 AdapterType,
                 SlotNumber,
                 Controller,
                 MultifunctionAdapter,
                 InterruptVector,
                 InterruptLevel,
                 InterruptMode,
                 32);


    NdisCloseConfiguration(ConfigHandle);


    return Status;           // should be NDIS_STATUS_SUCCESS

}

STATIC
VOID
SonicRemoveAdapter(
    IN NDIS_HANDLE MacAdapterContext
    )

/*++

Routine Description:

    SonicRemoveAdapter removes an adapter previously registered
    with NdisRegisterAdapter.

Arguments:

    MacAdapterContext - The context value that the MAC passed
        to NdisRegisterAdapter; actually as pointer to a
        SONIC_ADAPTER.

Return Value:

    None.

--*/

{
    PSONIC_ADAPTER Adapter;
    BOOLEAN Canceled;

    Adapter = PSONIC_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    Adapter->Removed = TRUE;

    //
    // Stop the chip.
    //

    SonicStopChip (Adapter);

    NdisDeregisterAdapterShutdownHandler(Adapter->NdisAdapterHandle);

    ASSERT (Adapter->OpenCount == 0);

    //
    // There are no opens left, so remove ourselves.
    //

    //
    // Stop the deadman timer
    //

    NdisCancelTimer(&Adapter->WakeUpTimer, &Canceled);

    NdisStallExecution(2500000);

    NdisRemoveInterrupt(&Adapter->Interrupt);

    EthDeleteFilter(Adapter->FilterDB);

    DeleteAdapterMemory(Adapter);
    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

    NdisFreeSpinLock(&Adapter->Lock);
    SONIC_FREE_MEMORY(Adapter, sizeof(SONIC_ADAPTER));

}

STATIC
NDIS_STATUS
SonicReset(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    The SonicReset request instructs the MAC to issue a hardware reset
    to the network adapter.  The MAC also resets its software state.  See
    the description of NdisReset for a detailed description of this request.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to SONIC_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_PENDING;

    PSONIC_ADAPTER Adapter =
        PSONIC_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the locks while we update the reference counts on the
    // adapter and the open.
    //

    if (Adapter->Removed) {

        return(NDIS_STATUS_FAILURE);

    }

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    if (!Adapter->ResetInProgress && !Adapter->IndicatingResetStart) {

        PSONIC_OPEN Open;

        Open = PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

        if (!Open->BindingShuttingDown) {

            Adapter->IndicatingResetStart = TRUE;

            if (!Adapter->IndicatingResetEnd) {

                //
                // Loop through, indicating RESET_START; we
                // don't bother with the StatusComplete
                // indication until RESET_END is indicated.
                //

                PSONIC_OPEN Open;
                PLIST_ENTRY CurrentLink;

                CurrentLink = Adapter->OpenBindings.Flink;

                while (CurrentLink != &Adapter->OpenBindings) {

                    Open = CONTAINING_RECORD(
                             CurrentLink,
                             SONIC_OPEN,
                             OpenList
                             );

                    Open->References++;
                    NdisReleaseSpinLock(&Adapter->Lock);

                    NdisIndicateStatus(
                        Open->NdisBindingContext,
                        NDIS_STATUS_RESET_START,
                        NULL,
                        0
                        );

                    NdisAcquireSpinLock(&Adapter->Lock);
                    Open->References--;

                    CurrentLink = CurrentLink->Flink;

                }


                Adapter->IndicatingResetStart = FALSE;

                SetupForReset(
                    Adapter,
                    PSONIC_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)
                    );
            }

            Open->References++;
            Adapter->ResettingOpen = Open;

            StatusToReturn = NDIS_STATUS_PENDING;

        } else {

            StatusToReturn = NDIS_STATUS_CLOSING;

        }

    } else {

        StatusToReturn = NDIS_STATUS_RESET_IN_PROGRESS;

    }


    //
    // This macro assumes it is called with the lock held,
    // and releases it.
    //

    SONIC_DO_DEFERRED(Adapter);
    return StatusToReturn;

}

#if 0
STATIC
UINT
CalculateCRC(
    IN UINT NumberOfBytes,
    IN PCHAR Input
    )

/*++

Routine Description:

    Calculates a 32 bit crc value over the input number of bytes.

    NOTE: This routine assumes UINTs are 32 bits.

Arguments:

    NumberOfBytes - The number of bytes in the input.

    Input - An input "string" to calculate a CRC over.

Return Value:

    A 32 bit crc value.


--*/

{

    const UINT POLY = 0x04c11db6;
    UINT CRCValue = 0xffffffff;

    ASSERT(sizeof(UINT) == 4);

    for (
        ;
        NumberOfBytes;
        NumberOfBytes--
        ) {

        UINT CurrentBit;
        UCHAR CurrentByte = *Input;
        Input++;

        for (
            CurrentBit = 8;
            CurrentBit;
            CurrentBit--
            ) {

            UINT CurrentCRCHigh = CRCValue >> 31;

            CRCValue <<= 1;

            if (CurrentCRCHigh ^ (CurrentByte & 0x01)) {

                CRCValue ^= POLY;
                CRCValue |= 0x00000001;

            }

            CurrentByte >>= 1;

        }

    }

    return CRCValue;

}
#endif

extern
VOID
StartAdapterReset(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    This is the first phase of resetting the adapter hardware.

    It makes the following assumptions:

    1) That the hardware has been stopped.

    2) That it can not be preempted.

    3) That no other adapter activity can occur.

    When this routine is finished all of the adapter information
    will be as if the driver was just initialized.

Arguments:

    Adapter - The adapter whose hardware is to be reset.

Return Value:

    None.

--*/
{

    //
    // These are used for cleaning the rings.
    //

    PSONIC_RECEIVE_DESCRIPTOR CurrentReceiveDescriptor;
    PSONIC_TRANSMIT_DESCRIPTOR CurrentTransmitDescriptor;
    UINT i;
    SONIC_PHYSICAL_ADDRESS SonicPhysicalAdr;


    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete.
    //

    SonicStopChip(Adapter);

    //
    // Once the chip is stopped we can't get any more interrupts.
    // Any interrupts that are "queued" for processing could
    // only possibly service this reset.  It is therefore safe for
    // us to clear the adapter global csr value.
    //
    Adapter->IsrValue = 0;


    Adapter->LastTransmitDescriptor =
                Adapter->TransmitDescriptorArea +
                (Adapter->NumberOfTransmitDescriptors-1);
    Adapter->NumberOfAvailableDescriptors =
                Adapter->NumberOfTransmitDescriptors;
    Adapter->AllocateableDescriptor =
                Adapter->TransmitDescriptorArea;
    Adapter->TransmittingDescriptor =
                Adapter->TransmitDescriptorArea;
    Adapter->FirstUncommittedDescriptor =
                Adapter->TransmitDescriptorArea;
    Adapter->PacketsSinceLastInterrupt = 0;

    Adapter->CurrentReceiveBufferIndex = 0;
    Adapter->CurrentReceiveDescriptorIndex = 0;
    Adapter->LastReceiveDescriptor =
                &Adapter->ReceiveDescriptorArea[
                    Adapter->NumberOfReceiveDescriptors-1];

    Adapter->InterruptMaskRegister = SONIC_INT_DEFAULT_VALUE;
    Adapter->ReceiveDescriptorsExhausted = FALSE;
    Adapter->ReceiveBuffersExhausted = FALSE;
    Adapter->ReceiveControlRegister = SONIC_RCR_DEFAULT_VALUE;

    Adapter->SendStageOpen = TRUE;

    Adapter->AlreadyProcessingSendStage = FALSE;

    //
    // Clean the receive descriptors and initialize the link
    // fields.
    //

    SONIC_ZERO_MEMORY(
        Adapter->ReceiveDescriptorArea,
        (sizeof(SONIC_RECEIVE_DESCRIPTOR)*Adapter->NumberOfReceiveDescriptors)
        );

    for (
        i = 0, CurrentReceiveDescriptor = Adapter->ReceiveDescriptorArea;
        i < Adapter->NumberOfReceiveDescriptors;
        i++,CurrentReceiveDescriptor++
        ) {

        CurrentReceiveDescriptor->InUse = SONIC_OWNED_BY_SONIC;

        SonicPhysicalAdr = NdisGetPhysicalAddressLow(Adapter->ReceiveDescriptorAreaPhysical) +
                        (i * sizeof(SONIC_RECEIVE_DESCRIPTOR));

        if (i == 0) {

            Adapter->ReceiveDescriptorArea[
                Adapter->NumberOfReceiveDescriptors-1].Link =
                        SonicPhysicalAdr | SONIC_END_OF_LIST;

        } else {

            Adapter->ReceiveDescriptorArea[i-1].Link = SonicPhysicalAdr;

        }

    }


    //
    // Clean the transmit descriptors and initialize the link
    // fields.
    //

    SONIC_ZERO_MEMORY(
        Adapter->TransmitDescriptorArea,
        (sizeof(SONIC_TRANSMIT_DESCRIPTOR)*Adapter->NumberOfTransmitDescriptors)
        );

    for (
        i = 0, CurrentTransmitDescriptor = Adapter->TransmitDescriptorArea;
        i < Adapter->NumberOfTransmitDescriptors;
        i++,CurrentTransmitDescriptor++
        ) {

        SonicPhysicalAdr = NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical) +
                        (i * sizeof(SONIC_TRANSMIT_DESCRIPTOR));

        if (i == 0) {

            Adapter->TransmitDescriptorArea[Adapter->NumberOfTransmitDescriptors-1].Link = SonicPhysicalAdr;

        } else {

            (CurrentTransmitDescriptor-1)->Link = SonicPhysicalAdr;

        }

    }


    //
    // Recover all of the adapter buffers.
    //

    {

        UINT i;

        for (
            i = 0;
            i < (SONIC_NUMBER_OF_SMALL_BUFFERS +
                 SONIC_NUMBER_OF_MEDIUM_BUFFERS +
                 SONIC_NUMBER_OF_LARGE_BUFFERS);
            i++
            ) {

            Adapter->SonicBuffers[i].Next = i+1;

        }

        Adapter->SonicBufferListHeads[0] = -1;
        Adapter->SonicBufferListHeads[1] = 0;
        Adapter->SonicBuffers[SONIC_NUMBER_OF_SMALL_BUFFERS-1].Next = -1;
        Adapter->SonicBufferListHeads[2] = SONIC_NUMBER_OF_SMALL_BUFFERS;
        Adapter->SonicBuffers[(SONIC_NUMBER_OF_SMALL_BUFFERS+
                               SONIC_NUMBER_OF_MEDIUM_BUFFERS)-1].Next = -1;
        Adapter->SonicBufferListHeads[3] = SONIC_NUMBER_OF_SMALL_BUFFERS +
                                           SONIC_NUMBER_OF_MEDIUM_BUFFERS;
        Adapter->SonicBuffers[(SONIC_NUMBER_OF_SMALL_BUFFERS+
                               SONIC_NUMBER_OF_MEDIUM_BUFFERS+
                               SONIC_NUMBER_OF_LARGE_BUFFERS)-1].Next = -1;

    }

    //
    // Go through the various transmit lists and abort every packet.
    //

    {

        UINT i;
        PNDIS_PACKET Packet;
        PSONIC_PACKET_RESERVED Reserved;
        PSONIC_OPEN Open;
        PNDIS_PACKET Next;

        for (
            i = 0;
            i < 3;
            i++
            ) {

            switch (i) {

                case 0:
                    Next = Adapter->FirstLoopBack;
                    break;
                case 1:
                    Next = Adapter->FirstFinishTransmit;
                    break;
                case 2:
                    Next = Adapter->FirstSendStagePacket;
                    break;

            }


            while (Next) {

                Packet = Next;
                Reserved = PSONIC_RESERVED_FROM_PACKET(Packet);
                Next = Reserved->Next;
                Open =
                  PSONIC_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

                //
                // The completion of the packet is one less reason
                // to keep the open around.
                //

                ASSERT(Open->References);

                NdisCompleteSend(
                    Open->NdisBindingContext,
                    Packet,
                    NDIS_STATUS_REQUEST_ABORTED
                    );

                Open->References--;

            }

        }

        Adapter->FirstLoopBack = NULL;
        Adapter->LastLoopBack = NULL;
        Adapter->FirstFinishTransmit = NULL;
        Adapter->LastFinishTransmit = NULL;
        Adapter->FirstSendStagePacket = NULL;
        Adapter->LastSendStagePacket = NULL;
        Adapter->GeneralOptional[GO_TRANSMIT_QUEUE_LENGTH - GO_ARRAY_START] = 0;

    }

    (VOID)SetupRegistersAndInit(Adapter);

}

STATIC
BOOLEAN
SetupRegistersAndInit(
    IN PSONIC_ADAPTER Adapter
    )

/*++

Routine Description:

    It is this routines responsibility to make sure that the
    initialization block is filled and the chip is initialized
    *but not* started.

    NOTE: This routine assumes that it is called with the lock
    acquired OR that only a single thread of execution is working
    with this particular adapter.

Arguments:

    Adapter - The adapter whose hardware is to be initialized.

Return Value:

    TRUE if the registers are initialized successfully.

--*/
{

    USHORT CommandRegister;
    UINT Time;


    SONIC_WRITE_PORT(Adapter, SONIC_DATA_CONFIGURATION,
            Adapter->DataConfigurationRegister
            );

    SONIC_WRITE_PORT(Adapter, SONIC_RECEIVE_CONTROL,
            Adapter->ReceiveControlRegister
            );

    SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_MASK,
            Adapter->InterruptMaskRegister
            );

    SONIC_WRITE_PORT(Adapter, SONIC_INTERRUPT_STATUS,
            (USHORT)0xffff
            );



    SONIC_WRITE_PORT(Adapter, SONIC_UPPER_TRANSMIT_DESCRIPTOR,
            SONIC_GET_HIGH_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical))
            );

    SONIC_WRITE_PORT(Adapter, SONIC_CURR_TRANSMIT_DESCRIPTOR,
            SONIC_GET_LOW_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->TransmitDescriptorAreaPhysical))
            );


    SONIC_WRITE_PORT(Adapter, SONIC_UPPER_RECEIVE_DESCRIPTOR,
            SONIC_GET_HIGH_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->ReceiveDescriptorAreaPhysical))
            );

    SONIC_WRITE_PORT(Adapter, SONIC_CURR_RECEIVE_DESCRIPTOR,
            SONIC_GET_LOW_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->ReceiveDescriptorAreaPhysical))
            );


    //
    // The EOBC value cannot be odd (since the card register
    // wants it in words); in addition it appears that the
    // value in the register must be even, so this number
    // has to be a multiple of 4.
    //
    ASSERT((SONIC_END_OF_BUFFER_COUNT & 0x3) == 0);

    switch (Adapter->AdapterType) {

#ifdef SONIC_EISA

    case SONIC_ADAPTER_TYPE_EISA:

        //
        // For the EISA card, set EOBC to 2 words more than real
        // size.
        //

        SONIC_WRITE_PORT(Adapter, SONIC_END_OF_BUFFER_WORD_COUNT,
                (SONIC_END_OF_BUFFER_COUNT / 2) + 2
                );
        break;

#endif  // SONIC_EISA

#ifdef SONIC_INTERNAL

    case SONIC_ADAPTER_TYPE_INTERNAL:

        SONIC_WRITE_PORT(Adapter, SONIC_END_OF_BUFFER_WORD_COUNT,
                SONIC_END_OF_BUFFER_COUNT / 2
                );
        break;

#endif  // SONIC_INTERNAL

    default:

        ASSERT(FALSE);
        break;

    }


    SONIC_WRITE_PORT(Adapter, SONIC_UPPER_RECEIVE_RESOURCE,
            SONIC_GET_HIGH_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
            );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_START,
            SONIC_GET_LOW_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
            );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_END,
            (USHORT)(SONIC_GET_LOW_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical)) +
                sizeof(SONIC_RECEIVE_RESOURCE) *
                Adapter->NumberOfReceiveBuffers)
            );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_READ,
            SONIC_GET_LOW_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
            );

    SONIC_WRITE_PORT(Adapter, SONIC_RESOURCE_WRITE,
            SONIC_GET_LOW_PART_ADDRESS(
                NdisGetPhysicalAddressLow(Adapter->ReceiveResourceAreaPhysical))
            );


    //
    // Now take us out of reset mode...
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
        0x0000
        );

    //
    // ...and issue the Read RRA command.
    //

    SONIC_WRITE_PORT(Adapter, SONIC_COMMAND,
        SONIC_CR_READ_RRA
        );



    //
    // Wait for 1/5 second for Read RRA to finish.
    //

    Time = 20;

    while (Time > 0) {

        NdisStallExecution(10000);

        SONIC_READ_PORT(Adapter, SONIC_COMMAND, &CommandRegister);
        if ((CommandRegister & SONIC_CR_READ_RRA) == 0) {
            break;
        }

        Time--;

    }

    if (Time == 0) {

#if DBG
        DbgPrint("SONIC: Could not read RRA\n");
#endif
        return FALSE;

    }


    //
    // This will cause a LOAD_CAM interrupt when it is done.
    //

    SonicStartCamReload(Adapter);

    return TRUE;

}

extern
VOID
SetupForReset(
    IN PSONIC_ADAPTER Adapter,
    IN PSONIC_OPEN Open
    )

/*++

Routine Description:

    This routine is used to fill in the who and why a reset is
    being set up as well as setting the appropriate fields in the
    adapter.

    NOTE: This routine must be called with the lock acquired.

Arguments:

    Adapter - The adapter whose hardware is to be initialized.

    Open - A pointer to an sonic open structure.

Return Value:

    None.

--*/
{

    PNDIS_REQUEST CurrentRequest;
    PNDIS_REQUEST * CurrentNextLocation;
    PSONIC_OPEN TmpOpen;

    PSONIC_REQUEST_RESERVED Reserved;

    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete. We take it out of reset mode, however.
    //

    SonicStopChip(Adapter);


    //
    // Once the chip is stopped we can't get any more interrupts.
    // This call ensures that any ISR which is just about to run
    // will find no bits in the ISR, and any DPR which fires will
    // find nothing queued to do.
    //

    NdisSynchronizeWithInterrupt(
        &Adapter->Interrupt,
        SonicSynchClearIsr,
        (PVOID)Adapter);


    Adapter->ResetInProgress = TRUE;

    //
    // Shut down all of the transmit queues so that the
    // transmit portion of the chip will eventually calm down.
    //

    Adapter->SendStageOpen = FALSE;

    //
    // If there is a close at the top of the queue, then
    // it may be in two states:
    //
    // 1- Has interrupted, and the InterruptDpc got the
    // interrupt out of Adapter->IsrValue before we zeroed it.
    //
    // 2- Has interrupted, but we zeroed Adapter->IsrValue
    // before it read it, OR has not yet interrupted.
    //
    // In case 1, the interrupt will be processed and the
    // close will complete without our intervention. In
    // case 2, the open will not complete. In that case
    // the CAM will have been updated for that open, so
    // all that remains is for us to dereference the open
    // as would have been done in the interrupt handler.
    //
    // Closes that are not at the top of the queue we
    // leave in place; when we restart the queue after
    // the reset, they will get processed.
    //

    CurrentRequest = Adapter->FirstRequest;

    if (CurrentRequest) {

        Reserved = PSONIC_RESERVED_FROM_REQUEST(CurrentRequest);

        //
        // If the first request is a close, take it off the
        // queue, and "complete" it.
        //

        if (CurrentRequest->RequestType == NdisRequestClose) {
            Adapter->FirstRequest = Reserved->Next;
            --(Reserved->OpenBlock)->References;
            CurrentRequest = Adapter->FirstRequest;
        }

        CurrentNextLocation = &(Adapter->FirstRequest);

        while (CurrentRequest) {

            Reserved = PSONIC_RESERVED_FROM_REQUEST(CurrentRequest);

            if ((CurrentRequest->RequestType == NdisRequestClose) ||
                (CurrentRequest->RequestType == NdisRequestOpen)) {

                //
                // Opens are inoffensive, we just leave them
                // on the list. Closes that were not at the
                // head of the list were not processing and
                // can be left on also.
                //

                CurrentNextLocation = &(Reserved->Next);

            } else {

                //
                // Not a close, remove it from the list and
                // fail it.
                //

                *CurrentNextLocation = Reserved->Next;
                TmpOpen = Reserved->OpenBlock;

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteRequest(
                    TmpOpen->NdisBindingContext,
                    CurrentRequest,
                    NDIS_STATUS_RESET_IN_PROGRESS
                    );

                NdisAcquireSpinLock(&Adapter->Lock);

                --TmpOpen->References;

            }

            CurrentRequest = *CurrentNextLocation;

        }

        Adapter->RequestInProgress = FALSE;

    }

}

#ifdef SONIC_INTERNAL

//
// The next routines are to support reading the registry to
// obtain information about the internal sonic on the
// MIPS R4000 motherboards.
//

//
// This structure is used as the Context in the callbacks
// to SonicHardwareSaveInformation.
//

typedef struct _SONIC_HARDWARE_INFO {

    //
    // These are read out of the "Configuration Data"
    // data.
    //

    CCHAR InterruptVector;
    KIRQL InterruptLevel;
    USHORT DataConfigurationRegister;
    LARGE_INTEGER PortAddress;
    BOOLEAN DataValid;
    UCHAR EthernetAddress[8];
    BOOLEAN AddressValid;

    //
    // This is set to TRUE if "Identifier" is equal to
    // "SONIC".
    //

    BOOLEAN SonicIdentifier;

} SONIC_HARDWARE_INFO, *PSONIC_HARDWARE_INFO;


STATIC
NTSTATUS
SonicHardwareSaveInformation(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    )

/*++

Routine Description:

    This routine is a callback routine for RtlQueryRegistryValues.
    It is called back with the data for the "Identifier" value
    and verifies that it is "SONIC", then is called back with
    the resource list and records the ports, interrupt number,
    and DCR value.

Arguments:

    ValueName - The name of the value ("Identifier" or "Configuration
        Data").

    ValueType - The type of the value (REG_SZ or REG_BINARY).

    ValueData - The null-terminated data for the value.

    ValueLength - The length of ValueData (ignored).

    Context - A pointer to the SONIC_HARDWARE_INFO structure.

    EntryContext - FALSE for "Identifier", TRUE for "Configuration Data".

Return Value:

    STATUS_SUCCESS

--*/

{
    PSONIC_HARDWARE_INFO HardwareInfo = (PSONIC_HARDWARE_INFO)Context;

    if ((BOOLEAN)EntryContext) {

        //
        // This is the "Configuration Data" callback.
        //

        if ((ValueType == REG_BINARY || ValueType == REG_FULL_RESOURCE_DESCRIPTOR) &&
            (ValueLength >= sizeof(CM_FULL_RESOURCE_DESCRIPTOR))) {

            BOOLEAN InterruptRead = FALSE;
            BOOLEAN PortAddressRead = FALSE;
            BOOLEAN DeviceSpecificRead = FALSE;
            UINT i;

            PCM_PARTIAL_RESOURCE_LIST ResourceList;
            PCM_PARTIAL_RESOURCE_DESCRIPTOR ResourceDescriptor;
            PCM_SONIC_DEVICE_DATA SonicDeviceData;

            ResourceList =
               &((PCM_FULL_RESOURCE_DESCRIPTOR)ValueData)->PartialResourceList;

            for (i = 0; i < ResourceList->Count; i++) {

                ResourceDescriptor = &(ResourceList->PartialDescriptors[i]);

                switch (ResourceDescriptor->Type) {

                case CmResourceTypePort:

                    HardwareInfo->PortAddress = ResourceDescriptor->u.Port.Start;
                    PortAddressRead = TRUE;
                    break;

                case CmResourceTypeInterrupt:

                    HardwareInfo->InterruptVector = (CCHAR)ResourceDescriptor->u.Interrupt.Vector;
                    HardwareInfo->InterruptLevel = (KIRQL)ResourceDescriptor->u.Interrupt.Level;
                    InterruptRead = TRUE;
                    break;

                case CmResourceTypeDeviceSpecific:

                    if (i == ResourceList->Count-1) {

                        SonicDeviceData = (PCM_SONIC_DEVICE_DATA)
                            &(ResourceList->PartialDescriptors[ResourceList->Count]);

                        //
                        // Make sure we have enough room for each element we read.
                        //

                        if (ResourceDescriptor->u.DeviceSpecificData.DataSize >=
                            (ULONG)(FIELD_OFFSET (CM_SONIC_DEVICE_DATA, EthernetAddress[0]))) {

                            HardwareInfo->DataConfigurationRegister =
                                SonicDeviceData->DataConfigurationRegister;
                            DeviceSpecificRead = TRUE;

                            //
                            // Version.Revision later than 0.0 means that
                            // the ethernet address is there too.
                            //

                            if ((SonicDeviceData->Version != 0) ||
                                (SonicDeviceData->Revision != 0)) {

                                if (ResourceDescriptor->u.DeviceSpecificData.DataSize >=
                                    (ULONG)(FIELD_OFFSET (CM_SONIC_DEVICE_DATA, EthernetAddress[0]) + 8)) {

                                    SONIC_MOVE_MEMORY(
                                        HardwareInfo->EthernetAddress,
                                        SonicDeviceData->EthernetAddress,
                                        8);

                                    HardwareInfo->AddressValid = TRUE;

                                }

                            }

                        }

                    }

                    break;

                }

            }

            //
            // Make sure we got all we wanted.
            //

            if (PortAddressRead && InterruptRead && DeviceSpecificRead) {
                HardwareInfo->DataValid = TRUE;
            }

        }

    } else {

        static const WCHAR SonicString[] = L"SONIC";

        //
        // This is the "Identifier" callback.
        //

        if ((ValueType == REG_SZ) &&
            (ValueLength >= sizeof(SonicString)) &&
            (RtlCompareMemory (ValueData, (PVOID)&SonicString, sizeof(SonicString)) == sizeof(SonicString))) {

            HardwareInfo->SonicIdentifier = TRUE;

        }

    }

    return STATUS_SUCCESS;

}


STATIC
BOOLEAN
SonicHardwareVerifyChecksum(
    IN PSONIC_ADAPTER Adapter,
    IN PUCHAR EthernetAddress,
    OUT ULONG ErrorLogData[3]
    )

/*++

Routine Description:

    This routine verifies that the checksum on the address
    for an internal sonic on a MIPS R4000 system is correct.

Arguments:

    Adapter - The adapter which is being verified.

    EthernetAddress - A pointer to the address, with the checksum
        following it.

    ErrorLogData - If the checksum is bad, returns the address
        and the checksum we expected.

Return Value:

    TRUE if the checksum is correct.

--*/

{

    //
    // Iteration variable.
    //
    UINT i;

    //
    // Holds the checksum value.
    //
    USHORT CheckSum = 0;


    //
    // The network address is stored in the first 6 bytes of
    // EthernetAddress. Following that is a zero byte followed
    // by a value such that the sum of a checksum on the six
    // bytes and this value is 0xff. The checksum is computed
    // by adding together the six bytes, with the carry being
    // wrapped back to the first byte.
    //

    for (i=0; i<6; i++) {

        CheckSum += EthernetAddress[i];
        if (CheckSum > 0xff) {
            CheckSum -= 0xff;
        }

    }


    if ((EthernetAddress[6] != 0x00)  ||
        ((EthernetAddress[7] + CheckSum) != 0xff)) {

        ErrorLogData[0] = ((ULONG)(EthernetAddress[3]) << 24) +
                          ((ULONG)(EthernetAddress[2]) << 16) +
                          ((ULONG)(EthernetAddress[1]) << 8) +
                          ((ULONG)(EthernetAddress[0]));
        ErrorLogData[1] = ((ULONG)(EthernetAddress[7]) << 24) +
                          ((ULONG)(EthernetAddress[6]) << 16) +
                          ((ULONG)(EthernetAddress[5]) << 8) +
                          ((ULONG)(EthernetAddress[4]));
        ErrorLogData[2] = 0xff - CheckSum;

        return FALSE;

    }

    return TRUE;

}

#endif  // SONIC_INTERNAL


STATIC
SONIC_HARDWARE_STATUS
SonicHardwareGetDetails(
    IN PSONIC_ADAPTER Adapter,
    IN UINT SlotNumber,
    IN UINT Controller,
    IN UINT MultifunctionAdapter,
    OUT PULONG InitialPort,
    OUT PULONG NumberOfPorts,
    IN OUT PUINT InterruptVector,
    IN OUT PUINT InterruptLevel,
    OUT ULONG ErrorLogData[3]
    )

/*++

Routine Description:

    This routine gets the initial port and number of ports for
    the Sonic. It also sets Adapter->PortShift. The ports are
    numbered 0, 1, 2, etc. but may appear as 16- or 32-bit
    ports, so PortShift will be 1 or 2 depending on how wide
    the ports are.

    It also sets the value of Adapter->DataConfigurationRegister,
    and may modify InterruptVector, InterruptLevel, and
    Adapter->PermanentNetworkAddress.

Arguments:

    Adapter - The adapter in question.

    SlotNumber - For the EISA card this is the slot number that the
        card is in.

    Controller - For the internal version, it is the
        NetworkController number.

    MultifunctionAdapter - For the internal version, it is the adapter number.

    InitialPort - The base of the Sonic ports.

    NumberOfPorts - The number of bytes of ports to map.

    InterruptVector - A pointer to the interrupt vector. Depending
        on the card type, this may be passed in or returned by
        this function.

    InterruptLevel - A pointer to the interrupt level. Depending
        on the card type, this may be passed in or returned by
        this function.

    ErrorLogData - If the return status is SonicHardwareChecksum,
        this returns 3 longwords to be included in the error log.

Return Value:

    SonicHardwareOk if successful, SonicHardwareChecksum if the
    checksum is bad, SonicHardwareConfig for other problems.

--*/

{

    switch (Adapter->AdapterType) {

#ifdef SONIC_EISA

    case SONIC_ADAPTER_TYPE_EISA:

        *InitialPort = (SlotNumber << 12);
        *NumberOfPorts = 0xD00;
        Adapter->PortShift = 1;
        Adapter->DataConfigurationRegister =
            SONIC_DCR_PROGRAMMABLE_OUTPUT_1 |
            SONIC_DCR_USER_DEFINABLE_1 |
            SONIC_DCR_3_WAIT_STATE |
            SONIC_DCR_BLOCK_MODE_DMA |
            SONIC_DCR_32_BIT_DATA_WIDTH |
            SONIC_DCR_8_WORD_RECEIVE_FIFO |
            SONIC_DCR_8_WORD_TRANSMIT_FIFO;

        return SonicHardwareOk;
        break;

#endif  // SONIC_EISA

#ifdef SONIC_INTERNAL

    case SONIC_ADAPTER_TYPE_INTERNAL:
    {

        //
        // For MIPS R4000 systems, we have to query the registry to obtain
        // information about ports, interrupts, and the value to be
        // stored in the DCR register.
        //

        //
        // NOTE: The following code is NT-specific, since that is
        // currently the only system that runs on the MIPS R4000 hardware.
        //
        // We initialize an RTL_QUERY_TABLE to retrieve the Identifer
        // and ConfigurationData strings from the registry.
        //

        PWSTR ConfigDataPath = L"\\Registry\\Machine\\Hardware\\Description\\System\\MultifunctionAdapter\\#\\NetworkController\\#";
        PWSTR IdentifierString = L"Identifier";
        PWSTR ConfigDataString = L"Configuration Data";
        RTL_QUERY_REGISTRY_TABLE QueryTable[4];
        SONIC_HARDWARE_INFO SonicHardwareInfo;
        NTSTATUS Status;


        //
        // Set up QueryTable to do the following:
        //

        //
        // 1) Call SonicSaveHardwareInformation for the "Identifier"
        // value.
        //

        QueryTable[0].QueryRoutine = SonicHardwareSaveInformation;
        QueryTable[0].Flags = RTL_QUERY_REGISTRY_REQUIRED;
        QueryTable[0].Name = IdentifierString;
        QueryTable[0].EntryContext = (PVOID)FALSE;
        QueryTable[0].DefaultType = REG_NONE;

        //
        // 2) Call SonicSaveHardwareInformation for the "Configuration Data"
        // value.
        //

        QueryTable[1].QueryRoutine = SonicHardwareSaveInformation;
        QueryTable[1].Flags = RTL_QUERY_REGISTRY_REQUIRED;
        QueryTable[1].Name = ConfigDataString;
        QueryTable[1].EntryContext = (PVOID)TRUE;
        QueryTable[1].DefaultType = REG_NONE;

        //
        // 3) Stop
        //

        QueryTable[2].QueryRoutine = NULL;
        QueryTable[2].Flags = 0;
        QueryTable[2].Name = NULL;


        //
        // Modify ConfigDataPath to replace the two # symbols with
        // the MultifunctionAdapter number and NetworkController number.
        //

        ConfigDataPath[67] = (WCHAR)('0' + MultifunctionAdapter);
        ConfigDataPath[87] = (WCHAR)('0' + Controller);

        SonicHardwareInfo.DataValid = FALSE;
        SonicHardwareInfo.AddressValid = FALSE;
        SonicHardwareInfo.SonicIdentifier = FALSE;

        Status = RtlQueryRegistryValues(
                     RTL_REGISTRY_ABSOLUTE,
                     ConfigDataPath,
                     QueryTable,
                     (PVOID)&SonicHardwareInfo,
                     NULL);

        if (!NT_SUCCESS(Status)) {
#if DBG
            DbgPrint ("SONIC: Could not read hardware information\n");
#endif
            return SonicHardwareConfig;
        }

        if (SonicHardwareInfo.DataValid && SonicHardwareInfo.SonicIdentifier) {

            *InterruptVector = (UINT)SonicHardwareInfo.InterruptVector;
            *InterruptLevel = (UINT)SonicHardwareInfo.InterruptLevel;
            *InitialPort = SonicHardwareInfo.PortAddress.LowPart;
            *NumberOfPorts = 192;
            Adapter->PortShift = 2;
            Adapter->DataConfigurationRegister =
                SonicHardwareInfo.DataConfigurationRegister;

            if (SonicHardwareInfo.AddressValid) {

                if (!SonicHardwareVerifyChecksum(Adapter, SonicHardwareInfo.EthernetAddress, ErrorLogData)) {
#if DBG
                    DbgPrint("SONIC: Invalid registry network address checksum!!\n");
#endif
                    return SonicHardwareChecksum;
                }

                SONIC_MOVE_MEMORY(
                    Adapter->PermanentNetworkAddress,
                    SonicHardwareInfo.EthernetAddress,
                    8);
                Adapter->PermanentAddressValid = TRUE;

            }

            return SonicHardwareOk;

        } else {

#if DBG
            DbgPrint ("SONIC: Incorrect registry hardware information\n");
#endif
            return SonicHardwareConfig;

        }

        break;

    }

#endif  // SONIC_INTERNAL

    default:

        ASSERT(FALSE);
        break;

    }

    return SonicHardwareConfig;

}


STATIC
BOOLEAN
SonicHardwareGetAddress(
    IN PSONIC_ADAPTER Adapter,
    IN ULONG ErrorLogData[3]
    )

/*++

Routine Description:

    This routine gets the network address from the hardware.

Arguments:

    Adapter - Where to store the network address.

    ErrorLogData - If the checksum is bad, returns the address
        and the checksum we expected.

Return Value:

    TRUE if successful.

--*/

{
#define NVRAM_READ_ONLY_BASE 0x8000b000

    //
    // Iteration variable.
    //
    UINT i;


    switch (Adapter->AdapterType) {

#ifdef SONIC_EISA

    case SONIC_ADAPTER_TYPE_EISA:

        //
        // The EISA card has the address stored at ports xC90 to xC95,
        // where x is the slot number.
        //

        for (i = 0; i < 6; i++) {

            NdisRawReadPortUchar(
                Adapter->SonicPortAddress + 0xc90 + i,
                &Adapter->PermanentNetworkAddress[i]);

        }

        break;

#endif  // SONIC_EISA

#ifdef SONIC_INTERNAL

    case SONIC_ADAPTER_TYPE_INTERNAL:
    {

        NDIS_STATUS Status;
        USHORT SiliconRevision;

        if (!Adapter->PermanentAddressValid) {

            //
            // Physical addresses for call to NdisMapIoSpace.
            //

            NDIS_PHYSICAL_ADDRESS NvRamPhysical =
                                NDIS_PHYSICAL_ADDRESS_CONST(NVRAM_READ_ONLY_BASE, 0);

            //
            // Temporarily maps the NVRAM into our address space.
            //
            PVOID NvRamMapping;



            //
            // If PermanentAddressValid is still FALSE then the address
            // was not read by SonicHardwareGetDetails, so we must do it
            // here.
            //

            NdisMapIoSpace (
                  &Status,
                  &NvRamMapping,
                  Adapter->NdisAdapterHandle,
                  NvRamPhysical,
                  8
                  );

            if (Status != NDIS_STATUS_SUCCESS) {

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_RESOURCE_CONFLICT,
                    0
                    );

                return(FALSE);

            }

            //
            // Verify that the checksum matches.
            //

            if (!SonicHardwareVerifyChecksum(Adapter, (PUCHAR)NvRamMapping, ErrorLogData)) {

#if DBG
                DbgPrint("SONIC: Invalid NVRAM network address checksum!!\n");
#endif
                NdisUnmapIoSpace(Adapter->NdisAdapterHandle, NvRamMapping, 8);
                return FALSE;

            }

            //
            // Checksum is OK, save the address.
            //

            for (i=0; i<6; i++) {
                Adapter->PermanentNetworkAddress[i] = *((PUCHAR)NvRamMapping+i);
            }
            Adapter->PermanentAddressValid = TRUE;

            NdisUnmapIoSpace(Adapter->NdisAdapterHandle, NvRamMapping, 8);

        }

        //
        // The Data Configuration Register is already set up, but we
        // change the FIFO initialization for old revisions.
        //

        SONIC_READ_PORT(Adapter, SONIC_SILICON_REVISION, &SiliconRevision);

        if (SiliconRevision < 4) {

            Adapter->DataConfigurationRegister =
                (Adapter->DataConfigurationRegister & SONIC_DCR_FIFO_MASK) |
                SONIC_DCR_8_WORD_RECEIVE_FIFO |
                SONIC_DCR_8_WORD_TRANSMIT_FIFO;

        }

        break;

    }

#endif  // SONIC_INTERNAL

    default:

        ASSERT(FALSE);
        break;

    }


#if DBG
    if (SonicDbg) {
        DbgPrint("SONIC: ");
        DbgPrint("[ %x-%x-%x-%x-%x-%x ]\n",
            (UCHAR)Adapter->PermanentNetworkAddress[0],
            (UCHAR)Adapter->PermanentNetworkAddress[1],
            (UCHAR)Adapter->PermanentNetworkAddress[2],
            (UCHAR)Adapter->PermanentNetworkAddress[3],
            (UCHAR)Adapter->PermanentNetworkAddress[4],
            (UCHAR)Adapter->PermanentNetworkAddress[5]);
        DbgPrint("\n");
    }
#endif

    return TRUE;

}


VOID
SonicShutdown(
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
    PSONIC_ADAPTER Adapter = (PSONIC_ADAPTER)(ShutdownContext);

    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // Set the flag
    //

    Adapter->Removed = TRUE;

    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete.
    //

    SonicStopChip(Adapter);

    NdisStallExecution(250000);

    NdisReleaseSpinLock(&Adapter->Lock);

}
