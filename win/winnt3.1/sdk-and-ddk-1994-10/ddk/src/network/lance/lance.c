/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    lance.c

Abstract:

    This is the main file for the Advanced Micro Devices LANCE (Am 7990)
    Ethernet controller.  This driver conforms to the NDIS 3.0 interface.

    The idea for handling loopback and sends simultaneously is largely
    adapted from the EtherLink II NDIS driver by Adam Barr.

Author:

    Anthony V. Ercolano (Tonye) 20-Jul-1990

Environment:

    Kernel Mode - Or whatever is the equivalent on OS/2 and DOS.

Revision History:

    31-Jul-1992  R.D. Lanser:

       Changed DECST card type to DECTC for the DEC TurboChannel option
       PMAD-AA (Lance ethernet).  This option will be available for all
       TurboChannel systems regardless of CPU type or system.

       Removed UsedLanceBuffer conditional (always true).

       Added  InterruptRequestLevel to the _LANCE_ADAPTER structure because
       'lance.c' was passing the InterruptVector as the IRQL to the interrupt
       connect routine which is not correct.  This works on JAZZ because the
       JAZZ HalGetInterruptVector is hardcoded to return a fixed IRQL for
       EISA devices.

       Remove hardcoded configuration information and added gets from the
       registry for the base address, interrupt vector, and interrupt
       request level.


--*/

#include <ndis.h>
#include <efilter.h>
#include "lancehrd.h"
#include "lancesft.h"
#include "dectc.h"

#if DBG
#define STATIC
#else
#define STATIC static
#endif


#if DBG

UCHAR LanceSendFails[256];
UCHAR LanceSendFailPlace = 0;

#endif


NDIS_HANDLE LanceNdisWrapperHandle;
NDIS_HANDLE LanceMacHandle;
PDRIVER_OBJECT LanceDriverObject;

//
// This constant is used for places where NdisAllocateMemory
// needs to be called and the HighestAcceptableAddress does
// not matter.
//

NDIS_PHYSICAL_ADDRESS HighestAcceptableMax =
    NDIS_PHYSICAL_ADDRESS_CONST(-1,-1);


#if LANCELOG

NDIS_TIMER LogTimer;
BOOLEAN LogTimerRunning = FALSE;

UCHAR Log[LOG_SIZE];

UCHAR LogPlace = 0;
UCHAR LogWrapped = 0;

UCHAR LancePrintLog = 0;

void
LogDpc(
    IN PVOID SystemSpecific1,
    PVOID Context,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
   )
{

   UNREFERENCED_PARAMETER(Context);
   UNREFERENCED_PARAMETER(SystemSpecific1);
   UNREFERENCED_PARAMETER(SystemSpecific2);
   UNREFERENCED_PARAMETER(SystemSpecific3);

   LOG(TIMER);

   NdisSetTimer(&LogTimer, 1000);

}


#endif




#define MAX_MULTICAST_ADDRESS ((UINT)32)

//
// Used for accessing the filter package multicast address list.
//

static CHAR MulticastAddresses[MAX_MULTICAST_ADDRESS][ETH_LENGTH_OF_ADDRESS];



//
// If you add to this, make sure to add the
// a case in LanceFillInGlobalData() and in
// LanceQueryGlobalStatistics() if global
// information only or
// LanceQueryProtocolStatistics() if it is
// protocol queriable information.
//
UINT LanceGlobalSupportedOids[] = {
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_LOOKAHEAD,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BUFFER_SPACE,
    OID_GEN_RECEIVE_BUFFER_SPACE,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_CURRENT_LOOKAHEAD,
    OID_GEN_XMIT_OK,
    OID_GEN_RCV_OK,
    OID_GEN_XMIT_ERROR,
    OID_GEN_RCV_ERROR,
    OID_GEN_RCV_NO_BUFFER,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE,
    OID_802_3_RCV_ERROR_ALIGNMENT,
    OID_802_3_XMIT_ONE_COLLISION,
    OID_802_3_XMIT_MORE_COLLISIONS
    };

//
// If you add to this, make sure to add the
// a case in LanceQueryGlobalStatistics() and in
// LanceQueryProtocolInformation()
//
UINT LanceProtocolSupportedOids[] = {
    OID_GEN_SUPPORTED_LIST,
    OID_GEN_HARDWARE_STATUS,
    OID_GEN_MEDIA_SUPPORTED,
    OID_GEN_MEDIA_IN_USE,
    OID_GEN_MAXIMUM_LOOKAHEAD,
    OID_GEN_MAXIMUM_FRAME_SIZE,
    OID_GEN_MAXIMUM_TOTAL_SIZE,
    OID_GEN_MAC_OPTIONS,
    OID_GEN_PROTOCOL_OPTIONS,
    OID_GEN_LINK_SPEED,
    OID_GEN_TRANSMIT_BUFFER_SPACE,
    OID_GEN_RECEIVE_BUFFER_SPACE,
    OID_GEN_TRANSMIT_BLOCK_SIZE,
    OID_GEN_RECEIVE_BLOCK_SIZE,
    OID_GEN_VENDOR_ID,
    OID_GEN_VENDOR_DESCRIPTION,
    OID_GEN_DRIVER_VERSION,
    OID_GEN_CURRENT_PACKET_FILTER,
    OID_GEN_CURRENT_LOOKAHEAD,
    OID_802_3_PERMANENT_ADDRESS,
    OID_802_3_CURRENT_ADDRESS,
    OID_802_3_MULTICAST_LIST,
    OID_802_3_MAXIMUM_LIST_SIZE
    };



//
// This macro is to synchronize execution with interrupts.  It
// gets the stored value of the CSR 0 and clears the old value.
//
#define GET_CSR0_SINCE_LAST_PROCESSED(A,V) \
{ \
    PLANCE_ADAPTER _A = A; \
    LANCE_SYNCH_CONTEXT _C; \
    _C.Adapter = _A; \
    _C.LocalRead = (V); \
    NdisSynchronizeWithInterrupt( \
        &(_A)->Interrupt, \
        LanceInterruptSynch, \
        &_C \
        ); \
}

//
// We define a constant csr0 value that is useful for initializing
// an already stopped LANCE.
//
// This also enables the chip for interrupts.
//
#define LANCE_CSR0_INIT_CHIP ((USHORT)0x41)

//
// We define a constant csr0 value that is useful for clearing all of
// the interesting bits that *could* be set on an interrupt.
//
#define LANCE_CSR0_CLEAR_INTERRUPT_BITS ((USHORT)0x7f00)

STATIC
NDIS_STATUS
LanceOpenAdapter(
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


VOID
LanceUnload(
    IN NDIS_HANDLE MacMacContext
    );

STATIC
NDIS_STATUS
LanceCloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    );


STATIC
NDIS_STATUS
LanceRequest(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );

NDIS_STATUS
LanceQueryProtocolInformation(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN NDIS_OID Oid,
    IN BOOLEAN GlobalMode,
    IN PVOID InfoBuffer,
    IN UINT BytesLeft,
    OUT PUINT BytesNeeded,
    OUT PUINT BytesWritten
    );

NDIS_STATUS
LanceQueryInformation(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    );

NDIS_STATUS
LanceSetInformation(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    );

STATIC
NDIS_STATUS
LanceReset(
    IN NDIS_HANDLE MacBindingHandle
    );


STATIC
NDIS_STATUS
LanceSetPacketFilter(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN UINT PacketFilter
    );


NDIS_STATUS
LanceFillInGlobalData(
    IN PLANCE_ADAPTER Adapter,
    IN PNDIS_REQUEST NdisRequest
    );


STATIC
NDIS_STATUS
LanceQueryGlobalStatistics(
    IN NDIS_HANDLE MacAdapterContext,
    IN PNDIS_REQUEST NdisRequest
    );

NDIS_STATUS
LanceChangeMulticastAddresses(
    IN UINT OldFilterCount,
    IN CHAR OldAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN UINT NewFilterCount,
    IN CHAR NewAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

STATIC
NDIS_STATUS
LanceChangeFilterClasses(
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    );

STATIC
VOID
LanceCloseAction(
    IN NDIS_HANDLE MacBindingHandle
    );

STATIC
BOOLEAN
AllocateAdapterMemory(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
DeleteAdapterMemory(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
ReturnAdapterResources(
    IN PLANCE_ADAPTER Adapter,
    IN UINT BufferIndex
    );

STATIC
VOID
RelinquishReceivePacket(
    IN PLANCE_ADAPTER Adapter,
    IN UINT StartingIndex,
    IN UINT NumberOfBuffers
    );

STATIC
BOOLEAN
ProcessReceiveInterrupts(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
BOOLEAN
ProcessTransmitInterrupts(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
LanceStandardInterruptDPC(
    IN PVOID SystemSpecific,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );


extern
BOOLEAN
LanceISR(
    IN PVOID Context
    );

STATIC
BOOLEAN
LanceInterruptSynch(
    IN PVOID Context
    );

STATIC
UINT
CalculateCRC(
    IN UINT NumberOfBytes,
    IN PCHAR Input
    );

STATIC
VOID
ProcessInterrupt(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
LanceStartChip(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
LanceSetInitializationBlock(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
SetInitBlockAndInit(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
StartAdapterReset(
    IN PLANCE_ADAPTER Adapter
    );

STATIC
VOID
SetupForReset(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_REQUEST_TYPE RequestType
    );

STATIC
NDIS_STATUS
LanceInitialInit(
    IN PLANCE_ADAPTER Adapter
    );


STATIC
VOID
FinishPendOp(
    IN PLANCE_ADAPTER Adapter,
    IN BOOLEAN Successful
    );


//
// Non portable interface.
//

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )

/*++

Routine Description:

    This is the primary initialization routine for the lance driver.
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
    NDIS_STATUS InitStatus;

    NDIS_HANDLE NdisMacHandle;

    NDIS_HANDLE NdisWrapperHandle;

    char Tmp[sizeof(NDIS_MAC_CHARACTERISTICS)];
    PNDIS_MAC_CHARACTERISTICS LanceChar = (PNDIS_MAC_CHARACTERISTICS)Tmp;

    NDIS_STRING MacName = NDIS_STRING_CONST("Lance");

    //
    // Initialize the wrapper.
    //

    NdisInitializeWrapper(&NdisWrapperHandle,
                          DriverObject,
                          RegistryPath,
                          NULL
                          );

    //
    // Initialize the MAC characteristics for the call to
    // NdisRegisterMac.
    //

    LanceChar->MajorNdisVersion = LANCE_NDIS_MAJOR_VERSION;
    LanceChar->MinorNdisVersion = LANCE_NDIS_MINOR_VERSION;
    LanceChar->OpenAdapterHandler = LanceOpenAdapter;
    LanceChar->CloseAdapterHandler = LanceCloseAdapter;
    LanceChar->SendHandler = LanceSend;
    LanceChar->TransferDataHandler = LanceTransferData;
    LanceChar->ResetHandler = LanceReset;
    LanceChar->RequestHandler = LanceRequest;
    LanceChar->AddAdapterHandler = LanceAddAdapter;
    LanceChar->UnloadMacHandler = LanceUnload;
    LanceChar->RemoveAdapterHandler = LanceRemoveAdapter;
    LanceChar->QueryGlobalStatisticsHandler = LanceQueryGlobalStatistics;

    LanceChar->Name = MacName;

    LanceDriverObject = DriverObject;
    LanceNdisWrapperHandle = NdisWrapperHandle;

    NdisRegisterMac(
        &InitStatus,
        &NdisMacHandle,
        NdisWrapperHandle,
        &NdisMacHandle,
        LanceChar,
        sizeof(*LanceChar)
        );

    LanceMacHandle = NdisMacHandle;

    if (InitStatus == NDIS_STATUS_SUCCESS) {

        return NDIS_STATUS_SUCCESS;

    }

    NdisTerminateWrapper(NdisWrapperHandle, NULL);

    return NDIS_STATUS_FAILURE;

}

NDIS_STATUS
LanceAddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    )
/*++
Routine Description:

    This is the Wd MacAddAdapter routine.    The system calls this routine
    to add support for a particular WD adapter.  This routine extracts
    configuration information from the configuration data base and registers
    the adapter with NDIS.

Arguments:

    see NDIS 3.0 spec...

Return Value:

    NDIS_STATUS_SUCCESS - Adapter was successfully added.
    NDIS_STATUS_FAILURE - Adapter was not added, also MAC deregistered.

--*/
{
    //
    // Pointer for the adapter root.
    //
    PLANCE_ADAPTER Adapter;


    NDIS_HANDLE ConfigHandle;
    PNDIS_CONFIGURATION_PARAMETER ReturnedValue;
    NDIS_STRING IoAddressStr = NDIS_STRING_CONST("IoBaseAddress");
    NDIS_STRING MaxMulticastListStr = NDIS_STRING_CONST("MaximumMulticastList");
    NDIS_STRING NetworkAddressStr = NDIS_STRING_CONST("NetworkAddress");
    NDIS_STRING InterruptStr = NDIS_STRING_CONST("InterruptNumber");
    NDIS_STRING CardStr = NDIS_STRING_CONST("CardType");
    NDIS_STRING MemoryBaseAddrStr = NDIS_STRING_CONST("MemoryMappedBaseAddress");

    NDIS_HANDLE NdisMacHandle = (NDIS_HANDLE)(*((PNDIS_HANDLE)MacMacContext));

    NDIS_STATUS Status;

    NDIS_ADAPTER_INFORMATION AdapterInformation;  // needed to register adapter
    NDIS_EISA_FUNCTION_INFORMATION EisaData;
    USHORT ConfigValue = 0;
    UCHAR HiBaseValue = 0;

    UINT MaxMulticastList = 32;
    PVOID NetAddress;
    UINT Length;

    USHORT RegUshort;
    UCHAR RegUchar;
    UINT LanceSlot = 1;

    BOOLEAN ConfigError = FALSE;
    NDIS_STATUS ConfigErrorCode;


    //
    // Allocate the Adapter block.
    //

    LANCE_ALLOC_PHYS(&Adapter, sizeof(LANCE_ADAPTER));

    if (Adapter == NULL){

        return NDIS_STATUS_RESOURCES;

    }

    LANCE_ZERO_MEMORY(
            Adapter,
            sizeof(LANCE_ADAPTER)
            );

    Adapter->NdisMacHandle = NdisMacHandle;


    //
    // Start with the default card
    //

    Adapter->LanceCard = LANCE_DE201;

    Adapter->RAP = LANCE_DE201_PRI_RAP_ADDRESS;
    Adapter->RDP = LANCE_DE201_PRI_RDP_ADDRESS;
    Adapter->Nicsr = LANCE_DE201_PRI_NICSR_ADDRESS;
    Adapter->NicsrDefaultValue = 0;
    Adapter->NetworkHardwareAddress = LANCE_DE201_PRI_NETWORK_ADDRESS;
    Adapter->HardwareBaseAddr = LANCE_DE201_BASE;
    Adapter->AmountOfHardwareMemory = LANCE_DE201_HARDWARE_MEMORY;
    Adapter->InterruptNumber = LANCE_DE201_INTERRUPT_VECTOR;
    Adapter->InterruptRequestLevel = LANCE_DE201_INTERRUPT_VECTOR;

    NdisOpenConfiguration(
                    &Status,
                    &ConfigHandle,
                    ConfigurationHandle
                    );

    if (Status != NDIS_STATUS_SUCCESS) {

        return NDIS_STATUS_FAILURE;

    }

    //
    // Read Card Type
    //

    NdisReadConfiguration(
                    &Status,
                    &ReturnedValue,
                    ConfigHandle,
                    &CardStr,
                    NdisParameterInteger
                    );

    if (Status == NDIS_STATUS_SUCCESS) {

        if (ReturnedValue->ParameterData.IntegerData == 2) {

            Adapter->LanceCard = LANCE_DE201;

        } else if (ReturnedValue->ParameterData.IntegerData == 1) {

            Adapter->LanceCard = LANCE_DE100;

        } else if (ReturnedValue->ParameterData.IntegerData == 3) {

            Adapter->LanceCard = LANCE_DEPCA;

#ifndef i386
        } else if (ReturnedValue->ParameterData.IntegerData == 4) {

            Adapter->LanceCard = LANCE_DECTC;

            ConfigErrorCode = LanceDecTcGetConfiguration(ConfigHandle, Adapter);

            if ( ConfigErrorCode != NDIS_STATUS_SUCCESS ) {
                ConfigError = TRUE;
            }
#endif // i386

        } else if (ReturnedValue->ParameterData.IntegerData == 5) {

            Adapter->LanceCard = LANCE_DE422;

        } else if (ReturnedValue->ParameterData.IntegerData == 6) {

            //
            // This is the De200, but it operates exactly like the 201.
            //

            Adapter->LanceCard = LANCE_DE201;

        } else if (ReturnedValue->ParameterData.IntegerData == 7) {

            //
            // This is the De101, but it operates exactly like the 100.
            //

            Adapter->LanceCard = LANCE_DE100;

        } else {

            ConfigError = TRUE;
            ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

            goto RegisterAdapter;
        }

    }

    //
    // Read MaxMulticastList
    //

    NdisReadConfiguration(
                    &Status,
                    &ReturnedValue,
                    ConfigHandle,
                    &MaxMulticastListStr,
                    NdisParameterInteger
                    );

    if (Status == NDIS_STATUS_SUCCESS) {

        MaxMulticastList = ReturnedValue->ParameterData.IntegerData;

    }

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
                Adapter->CurrentNetworkAddress,
                NetAddress
                );

    }

    if (Adapter->LanceCard & (LANCE_DE201 | LANCE_DE100)) {

        //
        // Read IoAddress
        //

        NdisReadConfiguration(
                    &Status,
                    &ReturnedValue,
                    ConfigHandle,
                    &IoAddressStr,
                    NdisParameterInteger
                    );

        if (Status == NDIS_STATUS_SUCCESS) {

            if (ReturnedValue->ParameterData.IntegerData == 1) {

                Adapter->RAP = LANCE_DE201_PRI_RAP_ADDRESS;

                Adapter->RDP = LANCE_DE201_PRI_RDP_ADDRESS;

                Adapter->Nicsr = LANCE_DE201_PRI_NICSR_ADDRESS;

                Adapter->NetworkHardwareAddress = LANCE_DE201_PRI_NETWORK_ADDRESS;

            } else if (ReturnedValue->ParameterData.IntegerData == 2) {

                Adapter->RAP = LANCE_DE201_SEC_RAP_ADDRESS;

                Adapter->RDP = LANCE_DE201_SEC_RDP_ADDRESS;

                Adapter->Nicsr = LANCE_DE201_SEC_NICSR_ADDRESS;

                Adapter->NetworkHardwareAddress = LANCE_DE201_SEC_NETWORK_ADDRESS;

            } else {

                ConfigError = TRUE;
                ConfigErrorCode = NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS;

                goto RegisterAdapter;
            }

        }




        //
        // Read Interrupt
        //

        NdisReadConfiguration(
                    &Status,
                    &ReturnedValue,
                    ConfigHandle,
                    &InterruptStr,
                    NdisParameterInteger
                    );

        if (Status == NDIS_STATUS_SUCCESS) {

            Adapter->InterruptNumber = (CCHAR)(ReturnedValue->ParameterData.IntegerData);
            Adapter->InterruptRequestLevel = Adapter->InterruptNumber;

            if (Adapter->LanceCard == LANCE_DE201) {

                if (!((Adapter->InterruptNumber == 5) ||
                      (Adapter->InterruptNumber == 9) ||
                      (Adapter->InterruptNumber == 10) ||
                      (Adapter->InterruptNumber == 11) ||
                      (Adapter->InterruptNumber == 15))) {

                    ConfigError = TRUE;
                    ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

                    goto RegisterAdapter;

                }

            } else {

                if (!((Adapter->InterruptNumber == 2) ||
                      (Adapter->InterruptNumber == 3) ||
                      (Adapter->InterruptNumber == 4) ||
                      (Adapter->InterruptNumber == 5) ||
                      (Adapter->InterruptNumber == 7))) {

                    ConfigError = TRUE;
                    ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

                    goto RegisterAdapter;

                }

            }

        }



        //
        // Read MemoryBaseAddress
        //


        NdisReadConfiguration(
                    &Status,
                    &ReturnedValue,
                    ConfigHandle,
                    &MemoryBaseAddrStr,
                    NdisParameterHexInteger
                    );

        if (Status == NDIS_STATUS_SUCCESS) {

            Adapter->HardwareBaseAddr = (PVOID)(ReturnedValue->ParameterData.IntegerData);

            if (!((Adapter->HardwareBaseAddr == (PVOID)0xC0000) ||
                  (Adapter->HardwareBaseAddr == (PVOID)0xC8000) ||
                  (Adapter->HardwareBaseAddr == (PVOID)0xD0000) ||
                  (Adapter->HardwareBaseAddr == (PVOID)0xD8000) ||
                  (Adapter->HardwareBaseAddr == (PVOID)0xE0000) ||
                  (Adapter->HardwareBaseAddr == (PVOID)0xE8000))) {

                ConfigError = TRUE;
                ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

                goto RegisterAdapter;

            }

        }


        if (((ULONG)Adapter->HardwareBaseAddr) & 0x8000) {

            Adapter->AmountOfHardwareMemory = 0x8000;
            Adapter->HardwareBaseOffset = 0x8000;

        } else {

            Adapter->AmountOfHardwareMemory = 0x10000;
            Adapter->HardwareBaseOffset = 0x0;

        }

    } else if (Adapter->LanceCard == LANCE_DEPCA) {

        Adapter->NetworkHardwareAddress = LANCE_DEPCA_EPROM_ADDRESS;
        Adapter->InterruptNumber = LANCE_DEPCA_INTERRUPT_VECTOR;
        Adapter->InterruptRequestLevel = LANCE_DEPCA_INTERRUPT_VECTOR;
        Adapter->RAP = LANCE_DEPCA_RAP_ADDRESS;
        Adapter->RDP = LANCE_DEPCA_RDP_ADDRESS;
        Adapter->AmountOfHardwareMemory = LANCE_DEPCA_HARDWARE_MEMORY;
        Adapter->HardwareBaseAddr = LANCE_DEPCA_BASE;
        Adapter->Nicsr = LANCE_DEPCA_NICSR_ADDRESS;
        Adapter->LanceCard = LANCE_DE100;

    } else if (Adapter->LanceCard == LANCE_DE422) {

        PUCHAR CurrentChar;
        BOOLEAN LastEntry;
        UCHAR InitType;
        USHORT PortAddress, PortValue, Mask;

        NdisReadEisaSlotInformation(
                                &Status,
                                ConfigurationHandle,
                                &Adapter->SlotNumber,
                                &EisaData
                                );

        if (Status != NDIS_STATUS_SUCCESS) {

            ConfigError = TRUE;
            ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

            goto RegisterAdapter;

        }

        //
        // Setup Ports
        //

        Adapter->RAP = (((ULONG)Adapter->SlotNumber) << 12) +
                       LANCE_DE422_RAP_ADDRESS;

        Adapter->RDP = (((ULONG)Adapter->SlotNumber) << 12) +
                       LANCE_DE422_RDP_ADDRESS;

        Adapter->Nicsr = (((ULONG)Adapter->SlotNumber) << 12) +
                         LANCE_DE422_NICSR_ADDRESS;

        Adapter->NetworkHardwareAddress = (((ULONG)Adapter->SlotNumber) << 12) +
                         LANCE_DE422_NETWORK_ADDRESS;

        CurrentChar = (PUCHAR) (EisaData.InitializationData);
        LastEntry = FALSE;

        while (!LastEntry) {

            InitType = *(CurrentChar++);
            PortAddress = *((USHORT UNALIGNED *) CurrentChar++);

            CurrentChar++;

            if ((InitType & 0x80) == 0) {
                LastEntry = TRUE;
            }



            if (PortAddress == (USHORT)(Adapter->NetworkHardwareAddress)) {

                PortValue = *((USHORT UNALIGNED *) CurrentChar++);

            } else if (PortAddress == ((Adapter->SlotNumber << 12) +
                                       LANCE_DE422_EXTENDED_MEMORY_BASE_ADDRESS)) {

                PortValue = (USHORT)(*(CurrentChar++));

            } else {

                continue;

            }



            if (InitType & 0x40) {

                if (PortAddress == Adapter->NetworkHardwareAddress) {

                    Mask = *((USHORT UNALIGNED *) CurrentChar++);

                } else {

                    Mask = (USHORT)(*(CurrentChar++));

                }

            } else {

                Mask = 0;

            }

            if (PortAddress == Adapter->NetworkHardwareAddress) {

                ConfigValue &= Mask;
                ConfigValue |= PortValue;

            } else {

                HiBaseValue &= (UCHAR)Mask;
                HiBaseValue |= (UCHAR)PortValue;

            }

        }

        //
        // Interpret values
        //

        switch (ConfigValue & 0x78) {

            case 0x40:

                Adapter->InterruptNumber = 11;
                break;

            case 0x20:

                Adapter->InterruptNumber = 10;
                break;

            case 0x10:

                Adapter->InterruptNumber = 9;
                break;

            case 0x08:

                Adapter->InterruptNumber = 5;
                break;

            default:

                ConfigError = TRUE;
                ConfigErrorCode = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;

                goto RegisterAdapter;

        }

        Adapter->InterruptRequestLevel = Adapter->InterruptNumber;
        //
        // We postpone the rest of the processing since we have to read from
        // the NICSR to get the amount of hardware memory and cannot do that
        // until after we have called NdisRegisterAdater.
        //

    }

RegisterAdapter:

    NdisCloseConfiguration(ConfigHandle);

    //
    // The adapter is initialized, register it with NDIS.
    // This must occur before interrupts are enabled since the
    // InitializeInterrupt routine requires the NdisAdapterHandle
    //

    //
    // Set up the AdapterInformation structure; zero it
    // first in case it is extended later.
    //

    LANCE_ZERO_MEMORY(&AdapterInformation, sizeof(NDIS_ADAPTER_INFORMATION));

    if (Adapter->LanceCard & (LANCE_DE100 | LANCE_DE201)) {

        AdapterInformation.AdapterType = NdisInterfaceIsa;

    } else if (Adapter->LanceCard == LANCE_DE422) {

        AdapterInformation.AdapterType = NdisInterfaceEisa;

    }

    AdapterInformation.NumberOfPortDescriptors = 1;

#ifndef i386

    if (Adapter->LanceCard == LANCE_DECTC) {

        Status = LanceDecTcGetInformation(Adapter, &AdapterInformation);
        if (Status != NDIS_STATUS_SUCCESS) {
            return Status;
        }

    } else

#endif

    if (Adapter->LanceCard & (LANCE_DE100 | LANCE_DE201)) {

        AdapterInformation.PortDescriptors[0].InitialPort = (ULONG)Adapter->Nicsr;
        AdapterInformation.PortDescriptors[0].NumberOfPorts = 0x10;

    } else if (Adapter->LanceCard == LANCE_DE422) {

        AdapterInformation.PortDescriptors[0].InitialPort = (ULONG)(Adapter->Nicsr);
        AdapterInformation.PortDescriptors[0].NumberOfPorts = 0x90;

    }

    if ((Status = NdisRegisterAdapter(
                    &Adapter->NdisAdapterHandle,
                    Adapter->NdisMacHandle,
                    Adapter,
                    ConfigurationHandle,
                    AdapterName,
                    &AdapterInformation
                    )) != NDIS_STATUS_SUCCESS) {

        LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

        return Status;

    }


    if (ConfigError) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            ConfigErrorCode,
            0
            );

        NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

        LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

        return NDIS_STATUS_FAILURE;

    }

    //
    // Now we get the rest of the information necessary for the DE422.
    //

    if (Adapter->LanceCard == LANCE_DE422) {

        //
        // Verify card is a DE422
        //

        NdisReadPortUshort(Adapter->NdisAdapterHandle,
                           (((ULONG)(Adapter->SlotNumber)) << 12) +
                               LANCE_DE422_EISA_IDENTIFICATION,
                           &RegUshort
                          );

        if (RegUshort != 0xA310) {

            //
            // Not a DE422 card
            //

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
                0
                );

            NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

            LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

            return NDIS_STATUS_FAILURE;

        }

        NdisReadPortUshort(Adapter->NdisAdapterHandle,
                           (((ULONG)(Adapter->SlotNumber)) << 12) +
                               LANCE_DE422_EISA_IDENTIFICATION + 2,
                           &RegUshort
                          );

        if (RegUshort != 0x2042) {

            //
            // Not a DE422 card
            //

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
                0
                );

            NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

            LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

            return NDIS_STATUS_FAILURE;

        }

        //
        // Check that the card is enabled.
        //

        NdisReadPortUchar(Adapter->NdisAdapterHandle,
                          (((ULONG)(Adapter->SlotNumber)) << 12) +
                              LANCE_DE422_EISA_CONTROL,
                          &RegUchar
                          );

        if (!(RegUchar & 0x1)) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_ADAPTER_DISABLED,
                0
                );

            NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

            LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

            return NDIS_STATUS_FAILURE;

        }

        //
        // Get Memory size
        //

        NdisReadPortUshort(Adapter->NdisAdapterHandle,
                           Adapter->Nicsr,
                           &RegUshort
                          );

        if (RegUshort & LANCE_NICSR_BUFFER_SIZE) {

            Adapter->AmountOfHardwareMemory = 0x8000;

        } else if (RegUshort & LANCE_NICSR_128K) {

            Adapter->AmountOfHardwareMemory = 0x20000;
            Adapter->NicsrDefaultValue = LANCE_NICSR_128K;

        } else {

            Adapter->AmountOfHardwareMemory = 0x10000;

        }

        //
        // Get Base memory address
        //

        switch (Adapter->AmountOfHardwareMemory) {

            case 0x8000:

                switch (ConfigValue & 0x07) {

                    case 0x04:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xC8000);
                        Adapter->HardwareBaseOffset = 0x8000;
                        break;

                    case 0x05:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xE8000);
                        Adapter->HardwareBaseOffset = 0x8000;
                        break;

                    case 0x06:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xD8000);
                        Adapter->HardwareBaseOffset = 0x8000;
                        break;

                    case 0x07:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xF8000);
                        Adapter->HardwareBaseOffset = 0x8000;
                        break;

                    default:

                        NdisWriteErrorLogEntry(
                            Adapter->NdisAdapterHandle,
                            NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION,
                            0
                            );

                        NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

                        LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

                        return NDIS_STATUS_FAILURE;

                }
                break;

            case 0x10000:

                switch (ConfigValue & 0x07) {

                    case 0x00:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xC0000);
                        Adapter->HardwareBaseOffset = 0x0000;
                        break;

                    case 0x01:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xE0000);
                        Adapter->HardwareBaseOffset = 0x0000;
                        break;

                    case 0x02:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xD0000);
                        Adapter->HardwareBaseOffset = 0x0000;
                        break;

                    case 0x03:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xF0000);
                        Adapter->HardwareBaseOffset = 0x0000;
                        break;

                    default:

                        NdisWriteErrorLogEntry(
                            Adapter->NdisAdapterHandle,
                            NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION,
                            0
                            );

                        NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

                        LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

                        return NDIS_STATUS_FAILURE;

                }
                break;

            case 0x20000:

                switch (ConfigValue & 0x07) {

                    case 0x00:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xC0000);
                        Adapter->HardwareBaseOffset = 0x0000;
                        break;

                    case 0x01:

                        Adapter->HardwareBaseAddr = (PVOID)((HiBaseValue << 24) + 0xD0000);
                        Adapter->HardwareBaseOffset = 0x0000;
                        break;

                    default:

                        NdisWriteErrorLogEntry(
                            Adapter->NdisAdapterHandle,
                            NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION,
                            0
                            );

                        NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

                        LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

                        return NDIS_STATUS_FAILURE;

                }

                break;

        }

    }

    //
    // Set the port addresses and the network address.
    //

    Adapter->InterruptsStopped = FALSE;
    Adapter->MaxMulticastList = MaxMulticastList;

    Status = LanceRegisterAdapter(
                           Adapter
                           );

    if (Status != NDIS_STATUS_SUCCESS) {



        //
        // LanceRegisterAdapter failed.
        //

        NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

        LANCE_FREE_PHYS(Adapter, sizeof(LANCE_ADAPTER));

        return NDIS_STATUS_FAILURE;

    }

    return NDIS_STATUS_SUCCESS;
}


VOID
LanceRemoveAdapter(
    IN PVOID MacAdapterContext
    )
/*++

Routine Description:

    LanceRemoveAdapter removes an adapter previously registered
    with NdisRegisterAdapter.

Arguments:

    MacAdapterContext - The context value that the MAC passed
        to NdisRegisterAdapter; actually as pointer to an
        LANCE_ADAPTER.

Return Value:

    None.

--*/
{

    PLANCE_ADAPTER Adapter;
    BOOLEAN Canceled;

    Adapter = PLANCE_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    //
    // There are no opens left, so remove ourselves.
    //

#if LANCELOG
    if (LogTimerRunning) {
        NdisCancelTimer(&LogTimer, &Canceled);
        LogTimerRunning = FALSE;
    }
#endif

    NdisCancelTimer(&Adapter->WakeUpTimer, &Canceled);

    if ( !Canceled ) {

        NdisStallExecution(500000);
    }

    NdisRemoveInterrupt(&(Adapter->Interrupt));

    NdisUnmapIoSpace(
           Adapter->NdisAdapterHandle,
           Adapter->MmMappedBaseAddr,
           Adapter->AmountOfHardwareMemory
           );

    EthDeleteFilter(Adapter->FilterDB);

    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

    NdisFreeSpinLock(&Adapter->Lock);

    NdisFreeMemory(Adapter, sizeof(LANCE_ADAPTER), 0);

    return;
}

VOID
LanceUnload(
    IN NDIS_HANDLE MacMacContext
    )

/*++

Routine Description:

    LanceUnload is called when the MAC is to unload itself.

Arguments:

    MacMacContext - not used.

Return Value:

    None.

--*/

{
    NDIS_STATUS InitStatus;

    UNREFERENCED_PARAMETER(MacMacContext);

    NdisDeregisterMac(
            &InitStatus,
            LanceMacHandle
            );

    NdisTerminateWrapper(
            LanceNdisWrapperHandle,
            NULL
            );

    return;
}

LanceRegisterAdapter(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine (and its interface) are not portable.  They are
    defined by the OS, the architecture, and the particular LANCE
    implementation.

    This routine is responsible for the allocation of the datastructures
    for the driver as well as any hardware specific details necessary
    to talk with the device.

Arguments:

    Adapter - Pointer to the adapter block.

Return Value:

    Returns false if anything occurred that prevents the initialization
    of the adapter.

--*/
{

    //
    // Result of Ndis Calls.
    //
    NDIS_STATUS Status;


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
    // This assertion checks that the network address in the initialization
    // block does start on the third byte of the initalization block.
    //
    // If this is true then other fields in the initialization block
    // and the send and receive descriptors should be at their correct
    // locations.
    //

    ASSERT(FIELD_OFFSET(LANCE_INITIALIZATION_BLOCK,PhysicalAddress[0]) == 2);


    //
    // Allocate memory for all of the adapter structures.
    //

    Adapter->NumberOfTransmitRings = LANCE_NUMBER_OF_TRANSMIT_RINGS;
    Adapter->LogNumberTransmitRings = LANCE_LOG_TRANSMIT_RINGS;

#ifndef i386

    if (Adapter->LanceCard == LANCE_DECTC) {

        Status = LanceDecTcSoftwareDetails(Adapter);
        if (Status != NDIS_STATUS_SUCCESS) {
            return Status;
        }

    } else


#endif

    {

        if (Adapter->AmountOfHardwareMemory == 0x20000) {

            ASSERT(Adapter->LanceCard == LANCE_DE422);

            Adapter->SizeOfReceiveBuffer  = LANCE_128K_SIZE_OF_RECEIVE_BUFFERS;
            Adapter->NumberOfSmallBuffers = LANCE_128K_NUMBER_OF_SMALL_BUFFERS;
            Adapter->NumberOfMediumBuffers= LANCE_128K_NUMBER_OF_MEDIUM_BUFFERS;
            Adapter->NumberOfLargeBuffers = LANCE_128K_NUMBER_OF_LARGE_BUFFERS;

            Adapter->NumberOfReceiveRings = LANCE_128K_NUMBER_OF_RECEIVE_RINGS;
            Adapter->LogNumberReceiveRings = LANCE_128K_LOG_RECEIVE_RINGS;

        } else if (Adapter->AmountOfHardwareMemory == 0x10000) {

            Adapter->NumberOfReceiveRings = LANCE_64K_NUMBER_OF_RECEIVE_RINGS;
            Adapter->LogNumberReceiveRings = LANCE_64K_LOG_RECEIVE_RINGS;

            Adapter->SizeOfReceiveBuffer  = LANCE_64K_SIZE_OF_RECEIVE_BUFFERS;
            Adapter->NumberOfSmallBuffers = LANCE_64K_NUMBER_OF_SMALL_BUFFERS;
            Adapter->NumberOfMediumBuffers= LANCE_64K_NUMBER_OF_MEDIUM_BUFFERS;
            Adapter->NumberOfLargeBuffers = LANCE_64K_NUMBER_OF_LARGE_BUFFERS;

        } else {

            Adapter->NumberOfReceiveRings = LANCE_32K_NUMBER_OF_RECEIVE_RINGS;
            Adapter->LogNumberReceiveRings = LANCE_32K_LOG_RECEIVE_RINGS;

            Adapter->SizeOfReceiveBuffer  = LANCE_32K_SIZE_OF_RECEIVE_BUFFERS;
            Adapter->NumberOfSmallBuffers = LANCE_32K_NUMBER_OF_SMALL_BUFFERS;
            Adapter->NumberOfMediumBuffers= LANCE_32K_NUMBER_OF_MEDIUM_BUFFERS;
            Adapter->NumberOfLargeBuffers = LANCE_32K_NUMBER_OF_LARGE_BUFFERS;

        }

    }

#ifndef i386

    if (((Adapter->LanceCard == LANCE_DECTC) &&
        (LanceDecTcHardwareDetails(Adapter) == NDIS_STATUS_SUCCESS)) ||
        LanceHardwareDetails(Adapter)) {

#else

    if (LanceHardwareDetails(Adapter)) {

#endif

        NDIS_PHYSICAL_ADDRESS PhysicalAddress;

        //
        // Get hold of the RAP and RDP address as well
        // as filling in the hardware assigned network
        // address.
        //

        if ((Adapter->CurrentNetworkAddress[0] == 0x00) &&
            (Adapter->CurrentNetworkAddress[1] == 0x00) &&
            (Adapter->CurrentNetworkAddress[2] == 0x00) &&
            (Adapter->CurrentNetworkAddress[3] == 0x00) &&
            (Adapter->CurrentNetworkAddress[4] == 0x00) &&
            (Adapter->CurrentNetworkAddress[5] == 0x00)) {

            Adapter->CurrentNetworkAddress[0] = Adapter->NetworkAddress[0];
            Adapter->CurrentNetworkAddress[1] = Adapter->NetworkAddress[1];
            Adapter->CurrentNetworkAddress[2] = Adapter->NetworkAddress[2];
            Adapter->CurrentNetworkAddress[3] = Adapter->NetworkAddress[3];
            Adapter->CurrentNetworkAddress[4] = Adapter->NetworkAddress[4];
            Adapter->CurrentNetworkAddress[5] = Adapter->NetworkAddress[5];

        }

        NdisSetPhysicalAddressHigh(PhysicalAddress, 0);
        NdisSetPhysicalAddressLow(PhysicalAddress, (ULONG)(Adapter->HardwareBaseAddr));

        NdisMapIoSpace(
           &Status,
           &(Adapter->MmMappedBaseAddr),
           Adapter->NdisAdapterHandle,
           PhysicalAddress,
           Adapter->AmountOfHardwareMemory
           );

        if (Status != NDIS_STATUS_SUCCESS) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_RESOURCE_CONFLICT,
                0
                );

            return(Status);

        }

        Adapter->CurrentMemoryFirstFree = Adapter->MmMappedBaseAddr;


        Adapter->MemoryFirstUnavailable =
                (PUCHAR)(Adapter->CurrentMemoryFirstFree) +
                Adapter->AmountOfHardwareMemory;

        if (!AllocateAdapterMemory(Adapter)) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                0
                );

            return( NDIS_STATUS_ADAPTER_NOT_FOUND );

        }

        InitializeListHead(&Adapter->OpenBindings);
        InitializeListHead(&Adapter->CloseList);

        NdisAllocateSpinLock(&Adapter->Lock);

        Adapter->InterruptDPC = (PVOID)LanceStandardInterruptDPC;
        Adapter->LoopbackDPC  = (PVOID)LanceStandardInterruptDPC;


        Adapter->AllocateableRing = Adapter->TransmitRing;
        Adapter->TransmittingRing = Adapter->TransmitRing;
        Adapter->FirstUncommittedRing = Adapter->TransmitRing;
        Adapter->NumberOfAvailableRings = Adapter->NumberOfTransmitRings;
        Adapter->LastTransmitRingEntry = Adapter->TransmitRing +
                (Adapter->NumberOfTransmitRings-1);

        Adapter->FirstLoopBack = NULL;
        Adapter->LastLoopBack = NULL;
        Adapter->FirstFinishTransmit = NULL;
        Adapter->LastFinishTransmit = NULL;
        Adapter->StageOpen = TRUE;
        Adapter->AlreadyProcessingStage = FALSE;
        Adapter->FirstStage1Packet = NULL;
        Adapter->LastStage1Packet = NULL;
        Adapter->CurrentReceiveIndex = 0;
        Adapter->OutOfReceiveBuffers = 0;
        Adapter->CRCError = 0;
        Adapter->FramingError = 0;
        Adapter->RetryFailure = 0;
        Adapter->LostCarrier = 0;
        Adapter->LateCollision = 0;
        Adapter->UnderFlow = 0;
        Adapter->Deferred = 0;
        Adapter->OneRetry = 0;
        Adapter->MoreThanOneRetry = 0;
        Adapter->ResetInProgress = FALSE;
        Adapter->ResetInitStarted = FALSE;
        Adapter->ResettingOpen = NULL;
        Adapter->FirstInitialization = TRUE;
        Adapter->HardwareFailure = FALSE;
        Adapter->PendQueue = NULL;
        Adapter->PendQueueTail = NULL;

        //
        // First we make sure that the device is stopped.  We call
        // directly since we don't have an Interrupt object yet.
        //

        LanceSyncStopChip(Adapter);


        //
        // Initialize the interrupt.
        //

        NdisInitializeInterrupt(
                &Status,
                &Adapter->Interrupt,
                Adapter->NdisAdapterHandle,
                (PNDIS_INTERRUPT_SERVICE)LanceISR,
                Adapter,
                (PNDIS_DEFERRED_PROCESSING)Adapter->InterruptDPC,
                Adapter->InterruptNumber,
                Adapter->InterruptRequestLevel,
                FALSE,
                NdisInterruptLatched
                );

        if (Status != NDIS_STATUS_SUCCESS){

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_INTERRUPT_CONNECT,
                0
                );

            NdisUnmapIoSpace(
                Adapter->NdisAdapterHandle,
                Adapter->MmMappedBaseAddr,
                Adapter->AmountOfHardwareMemory);

            DeleteAdapterMemory(Adapter);

            return Status;
        }


        if (!EthCreateFilter(
                         Adapter->MaxMulticastList,
                         LanceChangeMulticastAddresses,
                         LanceChangeFilterClasses,
                         LanceCloseAction,
                         Adapter->CurrentNetworkAddress,
                         &Adapter->Lock,
                         &Adapter->FilterDB
                         )) {

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                0
                );

            NdisUnmapIoSpace(
                Adapter->NdisAdapterHandle,
                Adapter->MmMappedBaseAddr,
                Adapter->AmountOfHardwareMemory);

            DeleteAdapterMemory(Adapter);

            NdisRemoveInterrupt(&Adapter->Interrupt);

            return NDIS_STATUS_RESOURCES;

        }

        if ((Status = LanceInitialInit(Adapter)) != NDIS_STATUS_SUCCESS) {


            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_HARDWARE_FAILURE,
                0
                );

            EthDeleteFilter(Adapter->FilterDB);

            NdisUnmapIoSpace(
                Adapter->NdisAdapterHandle,
                Adapter->MmMappedBaseAddr,
                Adapter->AmountOfHardwareMemory);

            DeleteAdapterMemory(Adapter);

            NdisRemoveInterrupt(&Adapter->Interrupt);

            return Status;

        }

        //
        // Initialize the wake up timer to catch interrupts that
        // don't complete. It fires continuously
        // every 5 seconds, and we check if there are any
        // uncompleted operations from the previous two-second
        // period.
        //

        Adapter->WakeUpDpc = (PVOID)LanceWakeUpDpc;

        NdisInitializeTimer(&Adapter->WakeUpTimer,
                            (PVOID)(Adapter->WakeUpDpc),
                            Adapter );

        NdisSetTimer(
            &Adapter->WakeUpTimer,
            5000
            );

        return Status;

    } else {

        NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
                0
                );

        return NDIS_STATUS_FAILURE;

    }

}

extern
NDIS_STATUS
LanceInitialInit(
    IN PLANCE_ADAPTER Adapter
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

    if (Adapter->LanceCard & (LANCE_DE201 | LANCE_DE100 | LANCE_DE422)) {

        //
        // Allow interrupts
        //

        Adapter->InterruptsStopped = FALSE;

        LOG(UNPEND);

        LANCE_WRITE_NICSR(Adapter, LANCE_NICSR_INT_ON);

    }

    SetInitBlockAndInit(Adapter);

    //
    // Delay execution for 1/2 second to give the lance
    // time to initialize.
    //


    NdisStallExecution( 500000 );

    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // The only way that first initialization could have
    // been turned off is if we actually initialized.
    //

    if (!Adapter->FirstInitialization) {

        //
        // Initialize the Deferred processing timer.
        //

        NdisInitializeTimer(&Adapter->DeferredTimer,
                            Adapter->LoopbackDPC,
                            Adapter);

#if LANCELOG

        if (!LogTimerRunning) {

            NdisInitializeTimer(&LogTimer,
                                (PVOID)LogDpc,
                                (PVOID)(&LogTimer)
                               );

            NdisSetTimer(&LogTimer,1000);

            LogTimerRunning = TRUE;

        }

#endif

        //
        // We actually did get the initialization.
        //

        NdisReleaseSpinLock(&Adapter->Lock);

        //
        // We can start the chip.  We may not
        // have any bindings to indicateto but this
        // is unimportant.
        //

        LanceStartChip(Adapter);


        return NDIS_STATUS_SUCCESS;


    } else {

        NdisReleaseSpinLock(&Adapter->Lock);

        return NDIS_STATUS_FAILURE;

    }

}

STATIC
VOID
LanceStartChip(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine is used to start an already initialized lance.

Arguments:

    Adapter - The adapter for the LANCE to start.

Return Value:

    None.

--*/

{

    if (Adapter->ResetInProgress) {

        return;

    }

    //
    // Set the RAP to csr0.
    //

    LANCE_WRITE_RAP(
        Adapter,
        LANCE_SELECT_CSR0
        );

    //
    // Set the RDP to a start chip.
    //

    LANCE_WRITE_RDP(
        Adapter,
        LANCE_CSR0_START | LANCE_CSR0_INTERRUPT_ENABLE
        );

}

STATIC
BOOLEAN
LanceInterruptSynch(
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will or
    the value of the stored csr 0 into the other passed address
    in the context and clear the stored csr 0 value.
    block and clear the stored csr0.

Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and a pointer to an address in which to place the contents
    of csr 0.

Return Value:

    Always returns true.

--*/

{

    PLANCE_SYNCH_CONTEXT C = Context;

    *(C->LocalRead) = *(C->LocalRead) | C->Adapter->CSR0Value;

    C->Adapter->CSR0Value = 0;

    return TRUE;

}

extern
BOOLEAN
LanceISR(
    IN PVOID Context
    )

/*++

Routine Description:

    Interrupt service routine for the lance.  It's main job is
    to get the value of CSR0 and record the changes in the
    adapters own list of interrupt reasons.

Arguments:

    Context - Really a pointer to the adapter.

Return Value:

    Returns true if the INTR bit of CSR0 of the lance is enabled.

--*/

{

    //
    // Will hold the value from the csr.
    //
    USHORT LocalCSR0Value;

    //
    // Holds the pointer to the adapter.
    //
    PLANCE_ADAPTER Adapter = Context;

    BOOLEAN StoppedInterrupts=FALSE;

    LOG(IN_ISR);

    if (Adapter->LanceCard & (LANCE_DE201 | LANCE_DE100 | LANCE_DE422)) {

        //
        // If not currently pended, pend interrupts...
        //

        if (!(Adapter->InterruptsStopped)){

            //
            // Pend interrupts
            //

            StoppedInterrupts = TRUE;

            Adapter->InterruptsStopped = TRUE;

            LOG(PEND);

            LANCE_ISR_WRITE_NICSR(Adapter,
                LANCE_NICSR_IMASK | LANCE_NICSR_LED_ON | LANCE_NICSR_INT_ON);

        }

    }

    //
    // We don't need to select csr0, as the only way we could get
    // an interrupt is to have already selected 0.
    //

    LANCE_ISR_READ_RDP(Adapter, &LocalCSR0Value);

    if (LocalCSR0Value & LANCE_CSR0_INTERRUPT_FLAG) {

        //
        // It's our interrupt. Clear only those bits that we got
        // in this read of csr0.  We do it this way incase any new
        // reasons for interrupts occur between the time that we
        // read csr0 and the time that we clear the bits.
        //

        LANCE_ISR_WRITE_RDP(
            Adapter,
            (USHORT)((LANCE_CSR0_CLEAR_INTERRUPT_BITS & LocalCSR0Value) |
               LANCE_CSR0_INTERRUPT_ENABLE)
            );


        if (Adapter->FirstInitialization &&
            (LocalCSR0Value & LANCE_CSR0_INITIALIZATION_DONE)) {

            Adapter->FirstInitialization = FALSE;

            if ((Adapter->LanceCard & (LANCE_DE201 | LANCE_DE100 | LANCE_DE422)) &&
                StoppedInterrupts) {

                //
                // Allow interrupts
                //

                Adapter->InterruptsStopped = FALSE;

                LOG(UNPEND);

                LANCE_ISR_WRITE_NICSR(Adapter, LANCE_NICSR_INT_ON);

            }

            LOG(OUT_ISR);

            return FALSE;

        } else {

            //
            // Or the csr value into the adapter version of csr 0.
            //

            Adapter->CSR0Value |= LocalCSR0Value;

            LOG(OUT_ISR);

            return TRUE;

        }

    } else {

        if ((Adapter->LanceCard & (LANCE_DE201 | LANCE_DE100 | LANCE_DE422)) &&
            StoppedInterrupts) {

            //
            // Allow interrupts
            //

            Adapter->InterruptsStopped = FALSE;

            LOG(UNPEND);

            LANCE_ISR_WRITE_NICSR(Adapter, LANCE_NICSR_INT_ON);

        }

        LOG(OUT_ISR);

        return FALSE;

    }

}

STATIC
VOID
LanceStandardInterruptDPC(
    IN PVOID SystemSpecific,
    IN PVOID Context,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )

/*++

Routine Description:

    This DPC routine is queued by the interrupt service routine
    and other routines within the driver that notice that
    some deferred processing needs to be done.  It's main
    job is to call the interrupt processing code.

Arguments:

    SystemSpecific - not used.

    Context - Really a pointer to the adapter.


    SystemArgument1 - Flag if we should turn interrupts on when done
    processing.

    SystemArgument2 - Not used.

Return Value:

    None.

--*/

{
    //
    // Holds the pointer to the adapter.
    //
    PLANCE_ADAPTER Adapter = Context;

    USHORT InterruptsStopped;

    UNREFERENCED_PARAMETER(SystemSpecific);
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    LOG(IN_DPC);

    NdisDprAcquireSpinLock(&Adapter->Lock);

    if (Adapter->ProcessInterruptRunning) {

        LOG(OUT_DPC);

        NdisDprReleaseSpinLock(&Adapter->Lock);

        return;

    }

    Adapter->ProcessInterruptRunning = TRUE;

    ProcessInterrupt(Adapter);

    InterruptsStopped = Adapter->InterruptsStopped;

    if ((Adapter->LanceCard & (LANCE_DE201 | LANCE_DE100 | LANCE_DE422)) &&
        InterruptsStopped) {

        //
        // Allow interrupts
        //

        LOG(UNPEND);

        LOG(OUT_DPC);


        //
        // Safe to write here since the ISR will not fire with interrupts stopped.
        //

        Adapter->InterruptsStopped = FALSE;

        LANCE_WRITE_NICSR(Adapter, LANCE_NICSR_INT_ON);

    } else {

        LOG(OUT_DPC);

    }

    NdisDprReleaseSpinLock(&Adapter->Lock);

}

STATIC
BOOLEAN
AllocateAdapterMemory(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine allocates memory for:

    - Transmit ring entries

    - Receive ring entries

    - Receive buffers

    - Adapter buffers for use if user transmit buffers don't meet hardware
      contraints

    - Structures to map transmit ring entries back to the packets.

Arguments:

    Adapter - The adapter to allocate memory for.

Return Value:

    Returns FALSE if some memory needed for the adapter could not
    be allocated.

--*/

{

    //
    // Pointer to a transmit ring entry.  Used while initializing
    // the ring.
    //
    PLANCE_TRANSMIT_ENTRY CurrentTransmitEntry;

    //
    // Pointer to a receive ring entry.  Used while initializing
    // the ring.
    //
    PLANCE_RECEIVE_ENTRY CurrentReceiveEntry;

    //
    // Simple iteration variable.
    //
    UINT i;

    //
    // These variables exist to reduce the amount of checking below.
    //

    ULONG NumberOfSmallBuffers;
    ULONG NumberOfMediumBuffers;
    ULONG NumberOfLargeBuffers;


    NumberOfSmallBuffers = Adapter->NumberOfSmallBuffers;
    NumberOfMediumBuffers = Adapter->NumberOfMediumBuffers;
    NumberOfLargeBuffers = Adapter->NumberOfLargeBuffers;



    //
    // Allocate memory for the initialization block.  Note that
    // this memory can not cross a page boundary.
    //

    LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(
        Adapter,
        sizeof(LANCE_INITIALIZATION_BLOCK),
        &Adapter->InitBlock
        );

    if (Adapter->InitBlock == NULL) {
        DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // Allocate the transmit ring descriptors.
    //

    LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(
        Adapter,
        sizeof(LANCE_TRANSMIT_ENTRY)*Adapter->NumberOfTransmitRings,
        &Adapter->TransmitRing
        )

    if (Adapter->TransmitRing == NULL) {
        DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // We have the transmit ring descriptors.  Make sure each is
    // in a clean state.
    //

    for (
        i = 0, CurrentTransmitEntry = Adapter->TransmitRing;
        i < Adapter->NumberOfTransmitRings;
        i++,CurrentTransmitEntry++
        ) {

        LANCE_ZERO_MEMORY_FOR_HARDWARE(
             CurrentTransmitEntry,
             sizeof(LANCE_TRANSMIT_ENTRY)
             );

    }


    //
    // Allocate all of the receive ring entries
    //

    LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(
        Adapter,
        sizeof(LANCE_RECEIVE_ENTRY)*Adapter->NumberOfReceiveRings,
        &Adapter->ReceiveRing
        )

    if (Adapter->ReceiveRing == NULL) {
        DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // We have the receive ring descriptors.  Allocate an
    // array to hold the virtual addresses of each receive
    // buffer.
    //

    LANCE_ALLOC_PHYS(
          &(Adapter->ReceiveVAs),
          sizeof(PVOID) * Adapter->NumberOfReceiveRings
          );

    if (Adapter->ReceiveVAs == NULL) {
        DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // Clean the above memory
    //

    LANCE_ZERO_MEMORY(
        Adapter->ReceiveVAs,
        (sizeof(PVOID)*Adapter->NumberOfReceiveRings)
        );


    //
    // We have the receive ring descriptors.  Allocate a buffer
    // for each descriptor and make sure descriptor is in a clean state.
    //
    // While we're at it, relinquish ownership of the ring discriptors to
    // the lance.
    //

    for (
        i = 0, CurrentReceiveEntry = Adapter->ReceiveRing;
        i < Adapter->NumberOfReceiveRings;
        i++,CurrentReceiveEntry++
        ) {

        LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(
            Adapter,
            Adapter->SizeOfReceiveBuffer,
            &Adapter->ReceiveVAs[i]
            );

        if (Adapter->ReceiveVAs[i] == NULL) {
            DeleteAdapterMemory(Adapter);
            return FALSE;
        }


        LANCE_SET_RECEIVE_BUFFER_ADDRESS(
                Adapter,
                CurrentReceiveEntry,
                Adapter->ReceiveVAs[i]
                );


        LANCE_SET_RECEIVE_BUFFER_LENGTH(
            CurrentReceiveEntry,
            Adapter->SizeOfReceiveBuffer
            );

        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             CurrentReceiveEntry->ReceiveSummaryBits,
             LANCE_RECEIVE_OWNED_BY_CHIP
             );


    }

    //
    // Allocate the ring to packet structure.
    //

    LANCE_ALLOC_PHYS(
        &(Adapter->RingToPacket),
        sizeof(LANCE_RING_TO_PACKET) * Adapter->NumberOfTransmitRings
        );

    if (Adapter->RingToPacket == NULL) {
        DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    LANCE_ZERO_MEMORY(
        Adapter->RingToPacket,
        sizeof(LANCE_RING_TO_PACKET) * Adapter->NumberOfTransmitRings
        );

    //
    // Allocate the array of buffer descriptors.
    //

    LANCE_ALLOC_PHYS(
       &(Adapter->LanceBuffers),
       sizeof(LANCE_BUFFER_DESCRIPTOR) *
         (NumberOfSmallBuffers +
          NumberOfMediumBuffers +
          NumberOfLargeBuffers)
       );

    if (Adapter->LanceBuffers == NULL) {
        DeleteAdapterMemory(Adapter);
        return FALSE;
    }

    //
    // Zero the memory of all the descriptors so that we can
    // know which buffers wern't allocated incase we can't allocate
    // them all.
    //
    LANCE_ZERO_MEMORY(
        Adapter->LanceBuffers,
        sizeof(LANCE_BUFFER_DESCRIPTOR)*
         (NumberOfSmallBuffers +
          NumberOfMediumBuffers +
          NumberOfLargeBuffers)
        );

    //
    // Allocate each of the small lance buffers and fill in the
    // buffer descriptor.
    //

    Adapter->LanceBufferListHeads[0] = -1;
    Adapter->LanceBufferListHeads[1] = 0;

    for (
        i = 0;
        i < NumberOfSmallBuffers;
        i++
        ) {

        LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(
            Adapter,
            LANCE_SMALL_BUFFER_SIZE,
            &Adapter->LanceBuffers[i].VirtualLanceBuffer
            );

        if (Adapter->LanceBuffers[i].VirtualLanceBuffer == NULL) {
            DeleteAdapterMemory(Adapter);
            return FALSE;
        }

        Adapter->LanceBuffers[i].Next = i+1;
        Adapter->LanceBuffers[i].BufferSize = LANCE_SMALL_BUFFER_SIZE;

    }

    //
    // Make sure that the last buffer correctly terminates the free list.
    //

    Adapter->LanceBuffers[i-1].Next = -1;

    //
    // Do the medium buffers now.
    //

    Adapter->LanceBufferListHeads[2] = i;

    for (
        ;
        i < NumberOfSmallBuffers + NumberOfMediumBuffers;
        i++
        ) {

        LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(
            Adapter,
            LANCE_MEDIUM_BUFFER_SIZE,
            &Adapter->LanceBuffers[i].VirtualLanceBuffer
            );

        if (Adapter->LanceBuffers[i].VirtualLanceBuffer == NULL) {
            DeleteAdapterMemory(Adapter);
            return FALSE;
        }


        Adapter->LanceBuffers[i].Next = i+1;
        Adapter->LanceBuffers[i].BufferSize = LANCE_MEDIUM_BUFFER_SIZE;

    }

    //
    // Make sure that the last buffer correctly terminates the free list.
    //

    Adapter->LanceBuffers[i-1].Next = -1;


    Adapter->LanceBufferListHeads[3] = i;

    for (
        ;
        i < NumberOfSmallBuffers + NumberOfMediumBuffers + NumberOfLargeBuffers;
        i++
        ) {


        LANCE_ALLOCATE_MEMORY_FOR_HARDWARE(
            Adapter,
            LANCE_LARGE_BUFFER_SIZE,
            &Adapter->LanceBuffers[i].VirtualLanceBuffer
            );

        if (Adapter->LanceBuffers[i].VirtualLanceBuffer == NULL) {
            DeleteAdapterMemory(Adapter);
            return FALSE;
        }


        Adapter->LanceBuffers[i].Next = i+1;
        Adapter->LanceBuffers[i].BufferSize = LANCE_LARGE_BUFFER_SIZE;

    }

    //
    // Make sure that the last buffer correctly terminates the free list.
    //

    Adapter->LanceBuffers[i-1].Next = -1;

    return TRUE;

}

STATIC
VOID
DeleteAdapterMemory(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine deallocates memory for:

    - Transmit ring entries

    - Receive ring entries

    - Receive buffers

    - Adapter buffers for use if user transmit buffers don't meet hardware
      contraints

    - Structures to map transmit ring entries back to the packets.

Arguments:

    Adapter - The adapter to deallocate memory for.

Return Value:

    None.

--*/

{

    //
    // These variables exist to reduce the amount of checking below.
    //

    ULONG NumberOfSmallBuffers;
    ULONG NumberOfMediumBuffers;
    ULONG NumberOfLargeBuffers;

    NumberOfSmallBuffers = Adapter->NumberOfSmallBuffers;
    NumberOfMediumBuffers = Adapter->NumberOfMediumBuffers;
    NumberOfLargeBuffers = Adapter->NumberOfLargeBuffers;


    if (Adapter->InitBlock) {

        LANCE_DEALLOCATE_MEMORY_FOR_HARDWARE(
            Adapter,
            Adapter->InitBlock
            );

    }

    if (Adapter->TransmitRing) {

        LANCE_DEALLOCATE_MEMORY_FOR_HARDWARE(
            Adapter,
            Adapter->TransmitRing
            );

    }

    if (Adapter->ReceiveRing) {

        LANCE_DEALLOCATE_MEMORY_FOR_HARDWARE(
            Adapter,
            Adapter->ReceiveRing
            );

    }

    if (Adapter->ReceiveVAs) {

        UINT i;

        for (
            i = 0;
            i < Adapter->NumberOfReceiveRings;
            i++
            ) {

            if (Adapter->ReceiveVAs[i]) {

                LANCE_DEALLOCATE_MEMORY_FOR_HARDWARE(
                    Adapter,
                    Adapter->ReceiveVAs[i]
                    );

            } else {

                break;

            }

        }

        LANCE_FREE_PHYS(
          Adapter->ReceiveVAs,
          sizeof(PVOID) * Adapter->NumberOfReceiveRings
          );

    }

    if (Adapter->RingToPacket) {

        LANCE_FREE_PHYS(
            Adapter->RingToPacket,
            sizeof(LANCE_RING_TO_PACKET) * Adapter->NumberOfTransmitRings
            );

    }

    if (Adapter->LanceBuffers) {

        UINT i;

        for (
            i = 0;
            i < NumberOfSmallBuffers + NumberOfMediumBuffers + NumberOfLargeBuffers;
            i++) {

            if (Adapter->LanceBuffers[i].VirtualLanceBuffer) {

                LANCE_DEALLOCATE_MEMORY_FOR_HARDWARE(
                    Adapter,
                    Adapter->LanceBuffers[i].VirtualLanceBuffer
                    );

            } else {

                break;

            }

        }

        LANCE_FREE_PHYS(
            Adapter->LanceBuffers,
            sizeof(LANCE_BUFFER_DESCRIPTOR) *
               (NumberOfSmallBuffers +
                NumberOfMediumBuffers +
                NumberOfLargeBuffers)
            );

    }

}

STATIC
NDIS_STATUS
LanceOpenAdapter(
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

    SelectedMediumIndex - Index of MediumArray which this adapter supports.

    MediumArray - Array of Medium types which the protocol is requesting.

    MediumArraySize - Number of entries in MediumArray.

    NdisBindingContext - A value to be recorded by the MAC and passed as
    context whenever an indication is delivered by the MAC for this binding.

    MacAdapterContext - The value associated with the adapter that is being
    opened when the MAC registered the adapter with NdisRegisterAdapter.

    OpenOptions - A bit mask of flags.  Not used.

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
    // The LANCE_ADAPTER that this open binding should belong too.
    //
    PLANCE_ADAPTER Adapter;

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    PLANCE_OPEN NewOpen;


    UNREFERENCED_PARAMETER(OpenOptions);
    UNREFERENCED_PARAMETER(OpenErrorStatus);
    UNREFERENCED_PARAMETER(AddressingInformation);


    //
    // Search for correct medium.
    //

    for (; MediumArraySize > 0; MediumArraySize--){

        if (MediumArray[MediumArraySize - 1] == NdisMedium802_3){

            MediumArraySize--;

            break;

        }

    }

    if (MediumArray[MediumArraySize] != NdisMedium802_3){

        return(NDIS_STATUS_UNSUPPORTED_MEDIA);

    }

    *SelectedMediumIndex = MediumArraySize;


    Adapter = PLANCE_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    NdisInterlockedAddUlong(&Adapter->References, 1, &Adapter->Lock);

    //
    // Allocate the space for the open binding.  Fill in the fields.
    //

    LANCE_ALLOC_PHYS(&NewOpen,sizeof(LANCE_OPEN));

    if (NewOpen != NULL){

        *MacBindingHandle = BINDING_HANDLE_FROM_PLANCE_OPEN(NewOpen);

        InitializeListHead(&NewOpen->OpenList);

        NewOpen->NdisBindingContext = NdisBindingContext;
        NewOpen->References = 0;
        NewOpen->BindingShuttingDown = FALSE;
        NewOpen->OwningLance = Adapter;

        NewOpen->LookAhead = LANCE_MAX_LOOKAHEAD;

        NdisAcquireSpinLock(&Adapter->Lock);

        Adapter->MaxLookAhead = LANCE_MAX_LOOKAHEAD;

        if (!EthNoteFilterOpenAdapter(
                                      NewOpen->OwningLance->FilterDB,
                                      NewOpen,
                                      NdisBindingContext,
                                      &NewOpen->NdisFilterHandle
                                      )) {

            NdisReleaseSpinLock(&Adapter->Lock);

            LANCE_FREE_PHYS(NewOpen, sizeof(LANCE_OPEN));

            StatusToReturn = NDIS_STATUS_FAILURE;

            NdisAcquireSpinLock(&Adapter->Lock);

        } else {

            //
            // Everything has been filled in.  Synchronize access to the
            // adapter block and link the new open adapter in and increment
            // the opens reference count to account for the fact that the
            // filter routines have a "reference" to the open.
            //

            InsertTailList(&Adapter->OpenBindings,&NewOpen->OpenList);
            NewOpen->References++;

        }

    } else {

        NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_OUT_OF_RESOURCES,
                1,
                (ULONG)openAdapter
                );


        StatusToReturn = NDIS_STATUS_RESOURCES;

        NdisAcquireSpinLock(&Adapter->Lock);

    }

    LANCE_DO_DEFERRED(Adapter);

    return StatusToReturn;
}

VOID
LanceAdjustMaxLookAhead(
    IN PLANCE_ADAPTER Adapter
    )
/*++

Routine Description:

    This routine finds the open with the maximum lookahead value and
    stores that in the adapter block.

Arguments:

    Adapter - A pointer to the adapter block.

Returns:

    None.

--*/
{
    ULONG CurrentMax = 0;
    PLIST_ENTRY Link = &(Adapter->OpenBindings);

    while (Link->Flink != &(Adapter->OpenBindings)) {

        if (((PLANCE_OPEN)(Link->Flink))->LookAhead > CurrentMax) {

            CurrentMax = ((PLANCE_OPEN)(Link->Flink))->LookAhead;

        }

        Link = Link->Flink;

    }

    if (CurrentMax == 0) {

        CurrentMax = LANCE_MAX_LOOKAHEAD;

    }

    Adapter->MaxLookAhead = CurrentMax;

}


STATIC
NDIS_STATUS
LanceCloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    This routine causes the MAC to close an open handle (binding).

Arguments:

    MacBindingHandle - The context value returned by the MAC when the
    adapter was opened.  In reality it is a PLANCE_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{

    PLANCE_ADAPTER Adapter;

    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    PLANCE_OPEN Open;

    Adapter = PLANCE_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the lock while we update the reference counts for the
    // adapter and the open.
    //

    Open = PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    if (!Open->BindingShuttingDown) {

        Open->References++;

        StatusToReturn = EthDeleteFilterOpenAdapter(
                                 Adapter->FilterDB,
                                 Open->NdisFilterHandle,
                                 NULL
                                 );

        //
        // If the status is successful that merely implies that
        // we were able to delete the reference to the open binding
        // from the filtering code.  If we have a successful status
        // at this point we still need to check whether the reference
        // count to determine whether we can close.
        //
        //
        // The delete filter routine can return a "special" status
        // that indicates that there is a current NdisIndicateReceive
        // on this binding.  See below.
        //

        if (StatusToReturn == NDIS_STATUS_SUCCESS) {

            //
            // Check whether the reference count is two.  If
            // it is then we can get rid of the memory for
            // this open.
            //
            // A count of two indicates one for this routine
            // and one for the filter which we *know* we can
            // get rid of.
            //

            if (Open->References == 2) {

                //
                // We are the only reference to the open.  Remove
                // it from the open list and delete the memory.
                //

                RemoveEntryList(&Open->OpenList);

                if (Adapter->MaxLookAhead == Open->LookAhead) {

                    LanceAdjustMaxLookAhead(Adapter);

                }

                LANCE_FREE_PHYS(Open, sizeof(LANCE_OPEN));

            } else {

                Open->BindingShuttingDown = TRUE;

                //
                // Remove the open from the open list and put it on
                // the closing list.
                //

                RemoveEntryList(&Open->OpenList);
                InsertTailList(&Adapter->CloseList,&Open->OpenList);

                //
                // Account for this routines reference to the open
                // as well as reference because of the filtering.
                //

                Open->References -= 2;

                //
                // Change the status to indicate that we will
                // be closing this later.
                //

                StatusToReturn = NDIS_STATUS_PENDING;

            }

        } else if (StatusToReturn == NDIS_STATUS_PENDING) {

            Open->BindingShuttingDown = TRUE;

            //
            // Remove the open from the open list and put it on
            // the closing list.
            //

            RemoveEntryList(&Open->OpenList);
            InsertTailList(&Adapter->CloseList,&Open->OpenList);

            //
            // Account for this routines reference to the open
            // as well as original open reference.
            //

            Open->References -= 2;

        } else if (StatusToReturn == NDIS_STATUS_CLOSING_INDICATING) {

            //
            // When we have this status it indicates that the filtering
            // code was currently doing an NdisIndicateReceive.  It
            // would not be wise to delete the memory for the open at
            // this point.  The filtering code will call our close action
            // routine upon return from NdisIndicateReceive and that
            // action routine will decrement the reference count for
            // the open.
            //

            Open->BindingShuttingDown = TRUE;

            //
            // This status is private to the filtering routine.  Just
            // tell the caller the the close is pending.
            //

            StatusToReturn = NDIS_STATUS_PENDING;

            //
            // Remove the open from the open list and put it on
            // the closing list.
            //

            RemoveEntryList(&Open->OpenList);
            InsertTailList(&Adapter->CloseList,&Open->OpenList);

            //
            // Account for this routines reference to the open. CloseAction
            // will remove the second reference.
            //

            Open->References--;

        } else {

            //
            // Account for this routines reference to the open.
            //

            Open->References--;

        }

    } else {

        StatusToReturn = NDIS_STATUS_CLOSING;

    }


    LANCE_DO_DEFERRED(Adapter);

    return StatusToReturn;

}

NDIS_STATUS
LanceRequest(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    This routine allows a protocol to query and set information
    about the MAC.

Arguments:

    MacBindingHandle - The context value returned by the MAC when the
    adapter was opened.  In reality, it is a pointer to PLANCE_OPEN.

    NdisRequest - A structure which contains the request type (Set or
    Query), an array of operations to perform, and an array for holding
    the results of the operations.

Return Value:

    The function value is the status of the operation.

--*/

{
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    PLANCE_OPEN Open = (PLANCE_OPEN)(MacBindingHandle);
    PLANCE_ADAPTER Adapter = (Open->OwningLance);

    if (Adapter->HardwareFailure) {

        return(NDIS_STATUS_FAILURE);

    }


    //
    // Ensure that the open does not close while in this function.
    //

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

#if LANCE_TRACE
    DbgPrint("In LanceRequest\n");
#endif

    //
    // Process request
    //

    if (NdisRequest->RequestType == NdisRequestQueryInformation) {

        StatusToReturn = LanceQueryInformation(Adapter, Open, NdisRequest);

    } else {

        if (NdisRequest->RequestType == NdisRequestSetInformation) {

            StatusToReturn = LanceSetInformation(Adapter,Open,NdisRequest);

        } else {

            StatusToReturn = NDIS_STATUS_NOT_RECOGNIZED;

        }

    }

    LANCE_DO_DEFERRED(Adapter);

#if LANCE_TRACE
    DbgPrint("Out LanceRequest %x\n",StatusToReturn);
#endif

    return(StatusToReturn);

}

NDIS_STATUS
LanceQueryProtocolInformation(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN NDIS_OID Oid,
    IN BOOLEAN GlobalMode,
    IN PVOID InfoBuffer,
    IN UINT BytesLeft,
    OUT PUINT BytesNeeded,
    OUT PUINT BytesWritten
)

/*++

Routine Description:

    The LanceQueryProtocolInformation process a Query request for
    NDIS_OIDs that are specific to a binding about the MAC.  Note that
    some of the OIDs that are specific to bindings are also queryable
    on a global basis.  Rather than recreate this code to handle the
    global queries, I use a flag to indicate if this is a query for the
    global data or the binding specific data.

Arguments:

    Adapter - a pointer to the adapter.

    Open - a pointer to the open instance.

    Oid - the NDIS_OID to process.

    GlobalMode - Some of the binding specific information is also used
    when querying global statistics.  This is a flag to specify whether
    to return the global value, or the binding specific value.

    PlaceInInfoBuffer - a pointer into the NdisRequest->InformationBuffer
     into which store the result of the query.

    BytesLeft - the number of bytes left in the InformationBuffer.

    BytesNeeded - If there is not enough room in the information buffer
    then this will contain the number of bytes needed to complete the
    request.

    BytesWritten - a pointer to the number of bytes written into the
    InformationBuffer.

Return Value:

    The function value is the status of the operation.

--*/

{
    NDIS_MEDIUM Medium = NdisMedium802_3;
    ULONG GenericULong;
    USHORT GenericUShort;
    UCHAR GenericArray[6];

    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    // Common variables for pointing to result of query
    //

    PVOID MoveSource;
    ULONG MoveBytes;

    NDIS_HARDWARE_STATUS HardwareStatus = NdisHardwareStatusReady;

    //
    // General Algorithm:
    //
    //      Switch(Request)
    //         Get requested information
    //         Store results in a common variable.
    //      Copy result in common variable to result buffer.
    //

    //
    // Make sure that ulong is 4 bytes.  Else GenericULong must change
    // to something of size 4.
    //
    ASSERT(sizeof(ULONG) == 4);


#if LANCE_TRACE
    DbgPrint("In LanceQueryProtocolInfo\n");
#endif

    //
    // Set default values
    //

    MoveSource = (PVOID)(&GenericULong);
    MoveBytes = sizeof(GenericULong);

    //
    // Switch on request type
    //

    switch (Oid) {

        case OID_GEN_MAC_OPTIONS:

            GenericULong = (ULONG)(NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |
                                   NDIS_MAC_OPTION_RECEIVE_SERIALIZED
                                  );

            break;

        case OID_GEN_SUPPORTED_LIST:

            if (!GlobalMode){

                MoveSource = (PVOID)(LanceProtocolSupportedOids);
                MoveBytes = sizeof(LanceProtocolSupportedOids);

            } else {

                MoveSource = (PVOID)(LanceGlobalSupportedOids);
                MoveBytes = sizeof(LanceGlobalSupportedOids);

            }
            break;

        case OID_GEN_HARDWARE_STATUS:


            if (Adapter->ResetInProgress){

                HardwareStatus = NdisHardwareStatusReset;

            } else {

                HardwareStatus = NdisHardwareStatusReady;

            }


            MoveSource = (PVOID)(&HardwareStatus);
            MoveBytes = sizeof(NDIS_HARDWARE_STATUS);

            break;

        case OID_GEN_MEDIA_SUPPORTED:
        case OID_GEN_MEDIA_IN_USE:

            MoveSource = (PVOID) (&Medium);
            MoveBytes = sizeof(NDIS_MEDIUM);
            break;

        case OID_GEN_MAXIMUM_LOOKAHEAD:

            GenericULong = LANCE_MAX_LOOKAHEAD;

            break;


        case OID_GEN_MAXIMUM_FRAME_SIZE:

            GenericULong = (ULONG)(LANCE_LARGE_BUFFER_SIZE - LANCE_HEADER_SIZE);

            break;


        case OID_GEN_MAXIMUM_TOTAL_SIZE:

            GenericULong = (ULONG)(LANCE_LARGE_BUFFER_SIZE);

            break;


        case OID_GEN_LINK_SPEED:

            GenericULong = (ULONG)(100000);

            break;


        case OID_GEN_TRANSMIT_BUFFER_SPACE:

            GenericULong = (ULONG)((LANCE_SMALL_BUFFER_SIZE * Adapter->NumberOfSmallBuffers) +
                                   (LANCE_MEDIUM_BUFFER_SIZE * Adapter->NumberOfMediumBuffers) +
                                   (LANCE_LARGE_BUFFER_SIZE * Adapter->NumberOfLargeBuffers));


            break;

        case OID_GEN_RECEIVE_BUFFER_SPACE:

            GenericULong = (ULONG)(Adapter->NumberOfReceiveRings *
                                   Adapter->SizeOfReceiveBuffer);

            break;

        case OID_GEN_TRANSMIT_BLOCK_SIZE:

            GenericULong = (ULONG)(LANCE_SMALL_BUFFER_SIZE);

            break;

        case OID_GEN_RECEIVE_BLOCK_SIZE:

            GenericULong = (ULONG)(Adapter->SizeOfReceiveBuffer);

            break;

        case OID_GEN_VENDOR_ID:

            NdisMoveMemory(
                (PVOID)&GenericULong,
                Adapter->NetworkAddress,
                3
                );

            GenericULong &= 0xFFFFFF00;

            if (Adapter->LanceCard == LANCE_DE201) {

                GenericULong |= 0x01;

            } else if (Adapter->LanceCard == LANCE_DE100) {

                GenericULong |= 0x02;

            } else if (Adapter->LanceCard == LANCE_DE422) {

                GenericULong |= 0x03;

            } else {

                GenericULong |= 0x04;

            }

            MoveSource = (PVOID)(&GenericULong);
            MoveBytes = sizeof(GenericULong);
            break;

        case OID_GEN_VENDOR_DESCRIPTION:

            if (Adapter->LanceCard == LANCE_DE201) {

                MoveSource = (PVOID)"DEC Etherworks Turbo Adapter";
                MoveBytes = 29;

            } else if (Adapter->LanceCard == LANCE_DE100) {

                MoveSource = (PVOID)"DEC Etherworks Adapter";
                MoveBytes = 23;

            } else if (Adapter->LanceCard == LANCE_DE422) {

                MoveSource = (PVOID)"DEC Etherworks Turbo EISA Adapter";
                MoveBytes = 34;

            } else {

                MoveSource = (PVOID)"Lance Adapter";
                MoveBytes = 15;

            }

            break;

        case OID_GEN_DRIVER_VERSION:

            GenericUShort = (USHORT)0x0301;

            MoveSource = (PVOID)(&GenericUShort);
            MoveBytes = sizeof(GenericUShort);
            break;


        case OID_GEN_CURRENT_PACKET_FILTER:

            if (GlobalMode ) {

                GenericULong = ETH_QUERY_FILTER_CLASSES(
                                Adapter->FilterDB
                                );

            } else {

                GenericULong = ETH_QUERY_PACKET_FILTER(
                                Adapter->FilterDB,
                                Open->NdisFilterHandle
                                );

            }

            break;

        case OID_GEN_CURRENT_LOOKAHEAD:

            if ( GlobalMode ) {

                GenericULong = Adapter->MaxLookAhead;

            } else {

                GenericULong = Open->LookAhead;

            }

            break;

        case OID_802_3_PERMANENT_ADDRESS:

            LANCE_MOVE_MEMORY((PCHAR)GenericArray,
                              Adapter->NetworkAddress,
                              ETH_LENGTH_OF_ADDRESS
                              );

            MoveSource = (PVOID)(GenericArray);
            MoveBytes = sizeof(Adapter->NetworkAddress);
            break;

        case OID_802_3_CURRENT_ADDRESS:


            LANCE_MOVE_MEMORY((PCHAR)GenericArray,
                              Adapter->CurrentNetworkAddress,
                              ETH_LENGTH_OF_ADDRESS
                              );

            MoveSource = (PVOID)(GenericArray);
            MoveBytes = sizeof(Adapter->CurrentNetworkAddress);
            break;

        case OID_802_3_MULTICAST_LIST:


            {
                UINT NumAddresses;

                if (GlobalMode) {

                    NumAddresses = ETH_NUMBER_OF_GLOBAL_FILTER_ADDRESSES(Adapter->FilterDB);

                    if ((NumAddresses * ETH_LENGTH_OF_ADDRESS) > BytesLeft) {

                        *BytesNeeded = (NumAddresses * ETH_LENGTH_OF_ADDRESS);

                        StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

                        break;

                    }

                    EthQueryGlobalFilterAddresses(
                        &StatusToReturn,
                        Adapter->FilterDB,
                        BytesLeft,
                        &NumAddresses,
                        InfoBuffer
                        );

                    *BytesWritten = NumAddresses * ETH_LENGTH_OF_ADDRESS;

                    //
                    // Should not be an error since we held the spinlock
                    // nothing should have changed.
                    //

                    ASSERT(StatusToReturn == NDIS_STATUS_SUCCESS);

                } else {

                    NumAddresses = EthNumberOfOpenFilterAddresses(
                                        Adapter->FilterDB,
                                        Open->NdisFilterHandle
                                        );

                    if ((NumAddresses * ETH_LENGTH_OF_ADDRESS) > BytesLeft) {

                        *BytesNeeded = (NumAddresses * ETH_LENGTH_OF_ADDRESS);

                        StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

                        break;

                    }

                    EthQueryOpenFilterAddresses(
                        &StatusToReturn,
                        Adapter->FilterDB,
                        Open->NdisFilterHandle,
                        BytesLeft,
                        &NumAddresses,
                        InfoBuffer
                        );

                    //
                    // Should not be an error since we held the spinlock
                    // nothing should have changed.
                    //

                    ASSERT(StatusToReturn == NDIS_STATUS_SUCCESS);

                    *BytesWritten = NumAddresses * ETH_LENGTH_OF_ADDRESS;

                }

            }

            MoveSource = (PVOID)NULL;
            MoveBytes = 0;

            break;

        case OID_802_3_MAXIMUM_LIST_SIZE:

            GenericULong = Adapter->MaxMulticastList;

            break;



        default:

            StatusToReturn = NDIS_STATUS_NOT_SUPPORTED;
            break;
    }

    if (StatusToReturn == NDIS_STATUS_SUCCESS){

        if (MoveBytes > BytesLeft){

            //
            // Not enough room in InformationBuffer. Punt
            //

            *BytesNeeded = MoveBytes;

            StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

        } else {

            //
            // Store result.
            //

            LANCE_MOVE_MEMORY(InfoBuffer, MoveSource, MoveBytes);

            (*BytesWritten) += MoveBytes;

        }
    }

#if LANCE_TRACE
    DbgPrint("Out LanceQueryProtocolInfo\n");
#endif

    return(StatusToReturn);
}

NDIS_STATUS
LanceQueryInformation(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    )
/*++

Routine Description:

    The LanceQueryInformation is used by LanceRequest to query information
    about the MAC.

Arguments:

    Adapter - A pointer to the adapter.

    Open - A pointer to a particular open instance.

    NdisRequest - A structure which contains the request type (Query),
    an array of operations to perform, and an array for holding
    the results of the operations.

Return Value:

    The function value is the status of the operation.

--*/

{

    UINT BytesWritten = 0;
    UINT BytesNeeded = 0;
    UINT BytesLeft = NdisRequest->DATA.QUERY_INFORMATION.InformationBufferLength;
    PUCHAR InfoBuffer = (PUCHAR)(NdisRequest->DATA.QUERY_INFORMATION.InformationBuffer);

    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;


#if LANCE_TRACE
    DbgPrint("In LanceQueryInfo\n");
#endif


    StatusToReturn = LanceQueryProtocolInformation(
                                Adapter,
                                Open,
                                NdisRequest->DATA.QUERY_INFORMATION.Oid,
                                FALSE,
                                InfoBuffer,
                                BytesLeft,
                                &BytesNeeded,
                                &BytesWritten
                                );


    NdisRequest->DATA.QUERY_INFORMATION.BytesWritten = BytesWritten;

    NdisRequest->DATA.QUERY_INFORMATION.BytesNeeded = BytesNeeded;

#if LANCE_TRACE
    DbgPrint("Out LanceQueryInfo\n");
#endif

    return(StatusToReturn);
}

NDIS_STATUS
LanceSetInformation(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    )
/*++

Routine Description:

    The LanceSetInformation is used by LanceRequest to set information
    about the MAC.

    Note: Assumes it is called with the lock held.

Arguments:

    Adapter - A pointer to the adapter.

    Open - A pointer to an open instance.

    NdisRequest - A structure which contains the request type (Set),
    an array of operations to perform, and an array for holding
    the results of the operations.

Return Value:

    The function value is the status of the operation.

--*/

{

    //
    // General Algorithm:
    //
    //  For each request
    //     Verify length
    //     Switch(Request)
    //        Process Request
    //

    UINT BytesRead = 0;
    UINT BytesNeeded = 0;
    UINT BytesLeft = NdisRequest->DATA.SET_INFORMATION.InformationBufferLength;
    PUCHAR InfoBuffer = (PUCHAR)(NdisRequest->DATA.SET_INFORMATION.InformationBuffer);

    //
    // Variables for a particular request
    //

    NDIS_OID Oid;
    UINT OidLength;

    //
    // Variables for holding the new values to be used.
    //

    ULONG LookAhead;
    ULONG Filter;

    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;


#if LANCE_TRACE
    DbgPrint("In LanceSetInfo\n");
#endif


    //
    // Get Oid and Length of next request
    //

    Oid = NdisRequest->DATA.SET_INFORMATION.Oid;

    OidLength = BytesLeft;

    switch (Oid) {


        case OID_802_3_MULTICAST_LIST:

            //
            // Verify length
            //

            if ((OidLength % ETH_LENGTH_OF_ADDRESS) != 0){

                StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

                NdisRequest->DATA.SET_INFORMATION.BytesRead = 0;
                NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;

                return(StatusToReturn);

            }

            //
            // Call into filter package.
            //

            if (!Open->BindingShuttingDown) {

                //
                // Increment the open while it is going through the filtering
                // routines.
                //

                Open->References++;

                StatusToReturn = EthChangeFilterAddresses(
                                         Adapter->FilterDB,
                                         Open->NdisFilterHandle,
                                         NdisRequest,
                                         OidLength / ETH_LENGTH_OF_ADDRESS,
                                         (CHAR **)InfoBuffer,
                                         TRUE
                                         );

                Open->References--;

            } else {

                StatusToReturn = NDIS_STATUS_CLOSING;

            }

            break;


        case OID_GEN_CURRENT_PACKET_FILTER:

            //
            // Verify length
            //

            if (OidLength != 4) {

                StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

                NdisRequest->DATA.SET_INFORMATION.BytesRead = 0;
                NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;

                break;

            }


            LANCE_MOVE_MEMORY(&Filter, InfoBuffer, 4);

            //
            // Verify bits
            //

            if (Filter & (NDIS_PACKET_TYPE_SOURCE_ROUTING |
                          NDIS_PACKET_TYPE_SMT |
                          NDIS_PACKET_TYPE_MAC_FRAME |
                          NDIS_PACKET_TYPE_FUNCTIONAL |
                          NDIS_PACKET_TYPE_ALL_FUNCTIONAL |
                          NDIS_PACKET_TYPE_GROUP
                         )) {

                StatusToReturn = NDIS_STATUS_NOT_SUPPORTED;

                NdisRequest->DATA.SET_INFORMATION.BytesRead = 4;
                NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;

                break;

            }

            StatusToReturn = LanceSetPacketFilter(Adapter,
                                                  Open,
                                                  NdisRequest,
                                                  Filter);

            break;

        case OID_GEN_CURRENT_LOOKAHEAD:

            //
            // Verify length
            //

            if (OidLength != 4) {

                StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

                NdisRequest->DATA.SET_INFORMATION.BytesRead = 0;
                NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;

                break;

            }

            LANCE_MOVE_MEMORY(&LookAhead, InfoBuffer, 4);

            if (LookAhead <= (LANCE_MAX_LOOKAHEAD)) {

                if (Open->LookAhead > Adapter->MaxLookAhead) {

                    Open->LookAhead = LookAhead;

                    Adapter->MaxLookAhead = LookAhead;

                } else {

                    if ((Open->LookAhead == Adapter->MaxLookAhead) &&
                        (LookAhead < Adapter->MaxLookAhead)) {

                        Open->LookAhead = LookAhead;

                        LanceAdjustMaxLookAhead(Adapter);

                    } else {

                        Open->LookAhead = LookAhead;

                    }

                }

            } else {

                StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

            }

            break;

        case OID_GEN_PROTOCOL_OPTIONS:

            StatusToReturn = NDIS_STATUS_SUCCESS;
            break;

        default:

            StatusToReturn = NDIS_STATUS_INVALID_OID;

            NdisRequest->DATA.SET_INFORMATION.BytesRead = 0;
            NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;

            break;
    }

    if (StatusToReturn == NDIS_STATUS_SUCCESS){

        NdisRequest->DATA.SET_INFORMATION.BytesRead = OidLength;
        NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;

    }

#if LANCE_TRACE
    DbgPrint("Out LanceSetInfo\n");
#endif

    return(StatusToReturn);
}

STATIC
NDIS_STATUS
LanceSetPacketFilter(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN UINT PacketFilter
    )

/*++

Routine Description:

    The LanceSetPacketFilter request allows a protocol to control the types
    of packets that it receives from the MAC.

    Note : Assumes that the lock is currently held.

Arguments:

    Adapter - Pointer to the LANCE_ADAPTER.

    Open - Pointer to the instance of LANCE_OPEN for Ndis.

    NdisRequest - Pointer to the NDIS_REQUEST which submitted the set
    packet filter command.

    PacketFilter - A bit mask that contains flags that correspond to specific
    classes of received packets.  If a particular bit is set in the mask,
    then packet reception for that class of packet is enabled.  If the
    bit is clear, then packets that fall into that class are not received
    by the client.  A single exception to this rule is that if the promiscuous
    bit is set, then the client receives all packets on the network, regardless
    of the state of the other flags.

Return Value:

    The function value is the status of the operation.

--*/

{

    //
    // Keeps track of the *MAC's* status.  The status will only be
    // reset if the filter change action routine is called.
    //
    NDIS_STATUS StatusOfFilterChange = NDIS_STATUS_SUCCESS;


#if LANCE_TRACE
    DbgPrint("In LanceSetPacketFilter\n");
#endif

    Adapter->References++;

    if (!Open->BindingShuttingDown) {

        //
        // Increment the open while it is going through the filtering
        // routines.
        //

        Open->References++;

        StatusOfFilterChange = EthFilterAdjust(
                                       Adapter->FilterDB,
                                       Open->NdisFilterHandle,
                                       NdisRequest,
                                       PacketFilter,
                                       TRUE
                                       );

        Open->References--;

    } else {

        StatusOfFilterChange = NDIS_STATUS_CLOSING;

    }

    Adapter->References--;

#if LANCE_TRACE
    DbgPrint("Out LanceSetPacketFilter\n");
#endif

    return StatusOfFilterChange;
}


NDIS_STATUS
LanceFillInGlobalData(
    IN PLANCE_ADAPTER Adapter,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    This routine completes a GlobalStatistics request.  It is critical that
    if information is needed from the Adapter->* fields, they have been
    updated before this routine is called.

Arguments:

    Adapter - A pointer to the Adapter.

    NdisRequest - A structure which contains the request type (Global
    Query), an array of operations to perform, and an array for holding
    the results of the operations.

Return Value:

    The function value is the status of the operation.

--*/
{
    //
    //   General Algorithm:
    //
    //      Switch(Request)
    //         Get requested information
    //         Store results in a common variable.
    //      default:
    //         Try protocol query information
    //         If that fails, fail query.
    //
    //      Copy result in common variable to result buffer.
    //   Finish processing

    UINT BytesWritten = 0;
    UINT BytesNeeded = 0;
    UINT BytesLeft = NdisRequest->DATA.QUERY_INFORMATION.InformationBufferLength;
    PUCHAR InfoBuffer = (PUCHAR)(NdisRequest->DATA.QUERY_INFORMATION.InformationBuffer);

    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    // This variable holds result of query
    //

    ULONG GenericULong;
    ULONG MoveBytes = sizeof(ULONG) * 2 + sizeof(NDIS_OID);

    //
    // Make sure that int is 4 bytes.  Else GenericULong must change
    // to something of size 4.
    //
    ASSERT(sizeof(ULONG) == 4);


    StatusToReturn = LanceQueryProtocolInformation(
                                    Adapter,
                                    NULL,
                                    NdisRequest->DATA.QUERY_INFORMATION.Oid,
                                    TRUE,
                                    InfoBuffer,
                                    BytesLeft,
                                    &BytesNeeded,
                                    &BytesWritten
                                    );


    if (StatusToReturn == NDIS_STATUS_NOT_SUPPORTED){

        StatusToReturn = NDIS_STATUS_SUCCESS;

        NdisAcquireSpinLock(&Adapter->Lock);

        //
        // Switch on request type
        //

        switch (NdisRequest->DATA.QUERY_INFORMATION.Oid) {

            case OID_GEN_XMIT_OK:

                GenericULong = (ULONG)(Adapter->Transmit +
                                           Adapter->LateCollision);

                break;

            case OID_GEN_RCV_OK:

                GenericULong = (ULONG)(Adapter->Receive);

                break;

            case OID_GEN_XMIT_ERROR:

                GenericULong = (ULONG)(Adapter->LostCarrier);

                break;

            case OID_GEN_RCV_ERROR:

                GenericULong = (ULONG)(Adapter->CRCError);

                break;

            case OID_GEN_RCV_NO_BUFFER:

                GenericULong = (ULONG)(Adapter->OutOfReceiveBuffers);

                break;

            case OID_802_3_RCV_ERROR_ALIGNMENT:

                GenericULong = (ULONG)(Adapter->FramingError);

                break;

            case OID_802_3_XMIT_ONE_COLLISION:

                GenericULong = (ULONG)(Adapter->OneRetry);

                break;

            case OID_802_3_XMIT_MORE_COLLISIONS:

                GenericULong = (ULONG)(Adapter->MoreThanOneRetry);

                break;


            default:

                StatusToReturn = NDIS_STATUS_INVALID_OID;

                break;

        }

        NdisReleaseSpinLock(&Adapter->Lock);

        if (StatusToReturn == NDIS_STATUS_SUCCESS){

            //
            // Check to make sure there is enough room in the
            // buffer to store the result.
            //

            if (BytesLeft >= sizeof(ULONG)){

                //
                // Store the result.
                //

                LANCE_MOVE_MEMORY(
                           (PVOID)InfoBuffer,
                           (PVOID)(&GenericULong),
                           sizeof(UINT)
                           );

                BytesWritten += sizeof(ULONG);

            } else {

                StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

            }

        }

    }

    NdisRequest->DATA.QUERY_INFORMATION.BytesWritten = BytesWritten;

    NdisRequest->DATA.QUERY_INFORMATION.BytesNeeded = BytesNeeded;

    return(StatusToReturn);
}

NDIS_STATUS
LanceQueryGlobalStatistics(
    IN NDIS_HANDLE MacAdapterContext,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    The LanceQueryGlobalStatistics is used by the protocol to query
    global information about the MAC.

Arguments:

    MacAdapterContext - The value associated with the adapter that is being
    opened when the MAC registered the adapter with NdisRegisterAdapter.

    NdisRequest - A structure which contains the request type (Query),
    an array of operations to perform, and an array for holding
    the results of the operations.

Return Value:

    The function value is the status of the operation.

--*/

{

    //
    // General Algorithm:
    //
    //
    //   Check if a request is going to pend...
    //      If so, pend the entire operation.
    //
    //   Else
    //      Fill in the request block.
    //
    //

    PLANCE_ADAPTER Adapter = (PLANCE_ADAPTER)(MacAdapterContext);

    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    //
    //   Check if a request is valid and going to pend...
    //      If so, pend the entire operation.
    //


    //
    // Switch on request type
    //

    switch (NdisRequest->DATA.QUERY_INFORMATION.Oid) {
        case OID_GEN_SUPPORTED_LIST:
        case OID_GEN_HARDWARE_STATUS:
        case OID_GEN_MEDIA_SUPPORTED:
        case OID_GEN_MEDIA_IN_USE:
        case OID_GEN_MAXIMUM_LOOKAHEAD:
        case OID_GEN_MAXIMUM_FRAME_SIZE:
        case OID_GEN_MAXIMUM_TOTAL_SIZE:
        case OID_GEN_MAC_OPTIONS:
        case OID_GEN_LINK_SPEED:
        case OID_GEN_TRANSMIT_BUFFER_SPACE:
        case OID_GEN_RECEIVE_BUFFER_SPACE:
        case OID_GEN_TRANSMIT_BLOCK_SIZE:
        case OID_GEN_RECEIVE_BLOCK_SIZE:
        case OID_GEN_VENDOR_ID:
        case OID_GEN_VENDOR_DESCRIPTION:
        case OID_GEN_DRIVER_VERSION:
        case OID_GEN_CURRENT_PACKET_FILTER:
        case OID_GEN_CURRENT_LOOKAHEAD:
        case OID_802_3_PERMANENT_ADDRESS:
        case OID_802_3_CURRENT_ADDRESS:
        case OID_802_5_CURRENT_FUNCTIONAL:
        case OID_GEN_XMIT_OK:
        case OID_GEN_RCV_OK:
        case OID_GEN_XMIT_ERROR:
        case OID_GEN_RCV_ERROR:
        case OID_GEN_RCV_NO_BUFFER:
        case OID_802_3_MULTICAST_LIST:
        case OID_802_3_MAXIMUM_LIST_SIZE:
        case OID_802_3_RCV_ERROR_ALIGNMENT:
        case OID_802_3_XMIT_ONE_COLLISION:
        case OID_802_3_XMIT_MORE_COLLISIONS:

            break;

        default:

            StatusToReturn = NDIS_STATUS_INVALID_OID;

            break;

    }

    if (StatusToReturn == NDIS_STATUS_SUCCESS){

        StatusToReturn = LanceFillInGlobalData(Adapter, NdisRequest);

    }

    return(StatusToReturn);
}



STATIC
NDIS_STATUS
LanceReset(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    The LanceReset request instructs the MAC to issue a hardware reset
    to the network adapter.  The MAC also resets its software state.  See
    the description of NdisReset for a detailed description of this request.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to LANCE_OPEN.

Return Value:

    The function value is the status of the operation.


--*/

{

    //
    // Holds the status that should be returned to the caller.
    //
    NDIS_STATUS StatusToReturn = NDIS_STATUS_PENDING;

    PLANCE_ADAPTER Adapter =
        PLANCE_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Hold the locks while we update the reference counts on the
    // adapter and the open.
    //

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    if (!Adapter->ResetInProgress) {

        PLANCE_OPEN Open;

        Open = PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

        if (!Open->BindingShuttingDown) {

            //
            // Is was a reset request
            //

            PLIST_ENTRY CurrentLink;

            Open->References++;

            CurrentLink = Adapter->OpenBindings.Flink;

            while (CurrentLink != &Adapter->OpenBindings) {

                Open = CONTAINING_RECORD(
                                         CurrentLink,
                                         LANCE_OPEN,
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

            Open = PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

#if LANCE_TRACE
            DbgPrint("Starting reset for 0x%x\n", Open);
#endif

            SetupForReset(
                Adapter,
                Open,
                NULL,
                NdisRequestGeneric1 // Means Reset
                );

            Open->References--;

        } else {

            StatusToReturn = NDIS_STATUS_CLOSING;

        }

    } else {

        StatusToReturn = NDIS_STATUS_SUCCESS;

    }

    LANCE_DO_DEFERRED(Adapter);

    return StatusToReturn;

}

STATIC
VOID
LanceSetInitializationBlock(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    This routine simply fills the initialization block
    with the information necessary for initialization.

    NOTE: This routine assumes that it is called with the lock
    acquired OR that only a single thread of execution is accessing
    the particular adapter.

Arguments:

    Adapter - The adapter which holds the initialization block
    to initialize.

Return Value:

    None.


--*/

{

    PHYSICAL_ADDRESS PhysAdr;

    UINT PacketFilters;

    PLANCE_RECEIVE_ENTRY CurrentEntry = Adapter->ReceiveRing;
    USHORT Mode;
    UCHAR RingNumber;
    UCHAR i;

#if LANCE_TRACE
    DbgPrint("in SetInitBlock\n");
#endif

    LANCE_ZERO_MEMORY_FOR_HARDWARE(
        Adapter->InitBlock,
        sizeof(LANCE_INITIALIZATION_BLOCK)
        );

    for (i=0; i < ETH_LENGTH_OF_ADDRESS; i++) {

        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
            Adapter->InitBlock->PhysicalAddress[i],
            Adapter->CurrentNetworkAddress[i]
            );

    }

    PhysAdr = LANCE_GET_HARDWARE_PHYSICAL_ADDRESS(Adapter, Adapter->TransmitRing);


    LANCE_WRITE_HARDWARE_LOW_PART_ADDRESS(
         Adapter->InitBlock->LowTransmitRingAddress,
         LANCE_GET_LOW_PART_ADDRESS(PhysAdr.LowPart)
         );
    LANCE_WRITE_HARDWARE_HIGH_PART_ADDRESS(
         Adapter->InitBlock->HighTransmitRingAddress,
         LANCE_GET_HIGH_PART_ADDRESS(PhysAdr.LowPart)
         );

    PhysAdr = LANCE_GET_HARDWARE_PHYSICAL_ADDRESS(Adapter, Adapter->ReceiveRing);

    //
    // Set that the chip owns each entry in the ring
    //

    for (CurrentEntry = Adapter->ReceiveRing, RingNumber = 0;
         RingNumber < Adapter->NumberOfReceiveRings ;
         RingNumber++, CurrentEntry++) {

        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             CurrentEntry->ReceiveSummaryBits,
             LANCE_RECEIVE_OWNED_BY_CHIP
             );

    }


    LANCE_WRITE_HARDWARE_LOW_PART_ADDRESS(
         Adapter->InitBlock->LowReceiveRingAddress,
         LANCE_GET_LOW_PART_ADDRESS(PhysAdr.LowPart)
         );
    LANCE_WRITE_HARDWARE_HIGH_PART_ADDRESS(
         Adapter->InitBlock->HighReceiveRingAddress,
         LANCE_GET_HIGH_PART_ADDRESS(PhysAdr.LowPart)
         );

    LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
        Adapter->InitBlock->TransmitLengthLow5BitsReserved,
        (UCHAR)(Adapter->LogNumberTransmitRings << 5)
        );

    LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
         Adapter->InitBlock->ReceiveLengthLow5BitsReserved,
         (UCHAR)(Adapter->LogNumberReceiveRings << 5)
         );

    //
    // Set up the address filtering.
    //
    // First get hold of the combined packet filter.
    //

    PacketFilters = ETH_QUERY_FILTER_CLASSES(Adapter->FilterDB);

#if LANCE_TRACE
    DbgPrint("Filters 0x%x\n", PacketFilters);
#endif

    if (PacketFilters & NDIS_PACKET_TYPE_PROMISCUOUS) {

        //
        // If one binding is promiscuous there is no point in
        // setting up any other filtering.  Every packet is
        // going to be accepted by the hardware.
        //

        LANCE_READ_HARDWARE_MEMORY_USHORT(
                 Adapter->InitBlock->ModeRegister,
                 &Mode
                 );

        LANCE_WRITE_HARDWARE_MEMORY_USHORT(
             Adapter->InitBlock->ModeRegister,
             Mode | LANCE_MODE_PROMISCUOUS
             );

    } else if (PacketFilters & NDIS_PACKET_TYPE_ALL_MULTICAST) {

        //
        // We turn on all the bits in the filter since one binding
        // wants every multicast address.
        //

        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[0],
             0xff
             );
        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[1],
             0xff
             );
        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[2],
             0xff
             );
        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[3],
             0xff
             );
        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[4],
             0xff
             );
        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[5],
             0xff
             );
        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[6],
             0xff
             );
        LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
             Adapter->InitBlock->LogicalAddressFilter[7],
             0xff
             );

    } else if (PacketFilters & NDIS_PACKET_TYPE_MULTICAST) {

        //
        // At least one open binding wants multicast addresses.
        //
        // We get the multicast addresses from the filter and
        // put each one through a CRC.  We then take the high
        // order 6 bits from the 32 bit CRC and set that bit
        // in the logical address filter.
        //

        UINT NumberOfAddresses;

        NDIS_STATUS Status;

        EthQueryGlobalFilterAddresses(
            &Status,
            Adapter->FilterDB,
            MAX_MULTICAST_ADDRESS * ETH_LENGTH_OF_ADDRESS,
            &NumberOfAddresses,
            MulticastAddresses
            );


        ASSERT(Status == NDIS_STATUS_SUCCESS);

        ASSERT(sizeof(ULONG) == 4);

        for (
            ;
            NumberOfAddresses;
            NumberOfAddresses--
            ) {

            UINT CRCValue;

            UINT HashValue = 0;

            CRCValue = CalculateCRC(
                           6,
                           MulticastAddresses[NumberOfAddresses-1]
                           );

            HashValue |= ((CRCValue & 0x00000001)?(0x00000020):(0x00000000));
            HashValue |= ((CRCValue & 0x00000002)?(0x00000010):(0x00000000));
            HashValue |= ((CRCValue & 0x00000004)?(0x00000008):(0x00000000));
            HashValue |= ((CRCValue & 0x00000008)?(0x00000004):(0x00000000));
            HashValue |= ((CRCValue & 0x00000010)?(0x00000002):(0x00000000));
            HashValue |= ((CRCValue & 0x00000020)?(0x00000001):(0x00000000));

            LANCE_READ_HARDWARE_MEMORY_UCHAR(
                      Adapter->InitBlock->LogicalAddressFilter[HashValue >> 3],
                      &RingNumber
                      );

            LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
                  Adapter->InitBlock->LogicalAddressFilter[HashValue >> 3],
                  RingNumber | (1 << (HashValue & 0x00000007))
                  );

        }

    }

#if LANCE_TRACE
    DbgPrint("out SetInitBlock\n");
#endif
}

STATIC
UINT
CalculateCRC(
    IN UINT NumberOfBytes,
    IN PCHAR Input
    )

/*++

Routine Description:

    Calculates a 32 bit crc value over the input number of bytes.

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

STATIC
NDIS_STATUS
LanceChangeMulticastAddresses(
    IN UINT OldFilterCount,
    IN CHAR OldAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN UINT NewFilterCount,
    IN CHAR NewAddresses[][ETH_LENGTH_OF_ADDRESS],
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    )

/*++

Routine Description:

    Action routine that will get called when a particular filter
    class is first used or last cleared.

    NOTE: This routine assumes that it is called with the lock
    acquired.

Arguments:


    OldFilterCount - Number of Addresses in the old list of multicast
    addresses.

    OldAddresses - An array of all the multicast addresses that used
    to be on the adapter.

    NewFilterCount - Number of Addresses that should be put on the adapter.

    NewAddresses - An array of all the multicast addresses that should
    now be used.

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to LANCE_OPEN.

    NdisRequest - The request which submitted the filter change.
    Must use when completing this request with the NdisCompleteRequest
    service, if the MAC completes this request asynchronously.

    Set - If true the change resulted from a set, otherwise the
    change resulted from a open closing.

Return Value:

    None.


--*/

{


    PLANCE_ADAPTER Adapter = PLANCE_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    UNREFERENCED_PARAMETER(OldFilterCount);
    UNREFERENCED_PARAMETER(OldAddresses);
    UNREFERENCED_PARAMETER(NewFilterCount);
    UNREFERENCED_PARAMETER(NewAddresses);

#if LANCE_TRACE
    DbgPrint("In LanceChangeMultiAdresses\n");
#endif

    if (Adapter->HardwareFailure) {

        return(NDIS_STATUS_SUCCESS);

    }

    if (NdisRequest == NULL) {

        //
        // It's a close request.
        //

        NdisRequest = (PNDIS_REQUEST)
           &(PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)->CloseMulticastRequest);

    }

    //
    // Check to see if the device is already resetting.  If it is
    // then pend this add.
    //

    if (Adapter->ResetInProgress) {

        if (Adapter->PendQueue == NULL) {

            Adapter->PendQueue = Adapter->PendQueueTail = NdisRequest;

        } else {

            PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(Adapter->PendQueueTail)->Next =
                NdisRequest;

        }

        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->Next = NULL;

        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->Open =
            PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->RequestType =
            Set ? NdisRequestGeneric2 : NdisRequestClose;

        return(NDIS_STATUS_PENDING);

    } else {

        //
        // We need to add this to the hardware multicast filtering.
        //

        SetupForReset(
                    Adapter,
                    PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle),
                    NdisRequest,
                    Set ? NdisRequestGeneric2 : // Means SetMulticastAddress
                          NdisRequestClose
                    );

    }

#if LANCE_TRACE
    DbgPrint("Out LanceChangeMultiAdresses\n");
#endif

    return NDIS_STATUS_PENDING;

}

STATIC
NDIS_STATUS
LanceChangeFilterClasses(
    IN UINT OldFilterClasses,
    IN UINT NewFilterClasses,
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN BOOLEAN Set
    )

/*++

Routine Description:

    Action routine that will get called when an address is added to
    the filter that wasn't referenced by any other open binding.

    NOTE: This routine assumes that it is called with the lock
    acquired.

Arguments:

    OldFilterClasses - The filter mask that used to be on the adapter.

    NewFilterClasses - The new filter mask to be put on the adapter.

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to LANCE_OPEN.

    NdisRequest - The request which submitted the filter change.
    Must use when completing this request with the NdisCompleteRequest
    service, if the MAC completes this request asynchronously.

    Set - If true the change resulted from a set, otherwise the
    change resulted from a open closing.

--*/

{

    PLANCE_ADAPTER Adapter = PLANCE_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);

    UNREFERENCED_PARAMETER(OldFilterClasses);
    UNREFERENCED_PARAMETER(NewFilterClasses);

#if LANCE_TRACE
    DbgPrint("In LanceChangeFilterClasses\n");
#endif


    if (Adapter->HardwareFailure) {

        return(NDIS_STATUS_SUCCESS);

    }


    if (NdisRequest == NULL) {

        //
        // It's a close request.
        //

        NdisRequest = (PNDIS_REQUEST)
           &(PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)->CloseFilterRequest);

    }

    //
    // Check to see if the device is already resetting.  If it is
    // then pend this add.
    //


    if (Adapter->ResetInProgress) {

        if (Adapter->PendQueue == NULL) {

            Adapter->PendQueue = Adapter->PendQueueTail = NdisRequest;

        } else {

            PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(Adapter->PendQueueTail)->Next =
                NdisRequest;

        }

        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->Next = NULL;

        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->Open =
            PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->RequestType =
            Set ? NdisRequestGeneric2 : NdisRequestClose;

        return(NDIS_STATUS_PENDING);

    } else {

        //
        // We need to add this to the hardware multicast filtering.
        //

        SetupForReset(
                    Adapter,
                    PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle),
                    NdisRequest,
                    Set ? NdisRequestGeneric3 : // Means SetPacketFilter
                          NdisRequestClose
                    );

    }

#if LANCE_TRACE
    DbgPrint("Out LanceChangeFilterClasses\n");
#endif

    return NDIS_STATUS_PENDING;

}

STATIC
VOID
LanceCloseAction(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    Action routine that will get called when a particular binding
    was closed while it was indicating through NdisIndicateReceive

    All this routine needs to do is to decrement the reference count
    of the binding.

    NOTE: This routine assumes that it is called with the lock acquired.

Arguments:

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to LANCE_OPEN.

Return Value:

    None.


--*/

{

    PLANCE_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)->References--;

}

STATIC
VOID
ProcessInterrupt(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    Main routine for processing interrupts.

    NOTE: MUST BE CALLED WITH LOCK HELD!

Arguments:

    Adapter - The Adapter to process interrupts for.

Return Value:

    None.

--*/

{

    //
    // Holds a value of csr0.
    //
    USHORT Csr = 0;

    //
    // Loop until there are no more processing sources.
    //

    while (TRUE) {

        Adapter->WakeUpTimeout = FALSE;

        GET_CSR0_SINCE_LAST_PROCESSED(
            Adapter,
            &Csr
            );

        //
        // Check the interrupt source and other reasons
        // for processing.  If there are no reasons to
        // process then exit this loop.
        //
        // Note that when we check the for processing sources
        // that we "carefully" check to see if we are already
        // processing one of the stage queues.  We do this
        // by checking the "AlreadyProcessingStageX" variables
        // in the adapter.  If any of these are true then
        // we let whoever set that boolean take care of pushing
        // the packet through the stage queues.
        //
        // By checking the "AlreadyProcessingStageX" variables
        // we can prevent a possible priority inversion where
        // we get "stuck" behind something that is processing
        // at a lower priority level.
        //

        if (((!Adapter->ResetInitStarted) &&
             ((Csr & (LANCE_CSR0_MEMORY_ERROR |
                      LANCE_CSR0_MISSED_PACKET |
                      LANCE_CSR0_BABBLE |
                      LANCE_CSR0_RECEIVER_INTERRUPT |
                      LANCE_CSR0_TRANSMITTER_INTERRUPT)) ||
              (Adapter->FirstLoopBack) ||
              (Adapter->ResetInProgress && (!Adapter->References)) ||
              ((!Adapter->AlreadyProcessingStage) &&
               (Adapter->StageOpen && Adapter->FirstStage1Packet)))) ||
            (Csr & LANCE_CSR0_INITIALIZATION_DONE)) {

            Adapter->References++;

        } else {

            break;

        }

        //
        // Check for initialization.
        //
        // Note that we come out of the synchronization above holding
        // the spinlock.
        //

        if (Csr & LANCE_CSR0_INITIALIZATION_DONE) {

            //
            // This will point (possibly null) to the open that
            // initiated the reset.
            //
            PLANCE_OPEN ResettingOpen;

            //
            // Possibly undefined reason why the reset was requested.
            //
            // It is undefined if the adapter initiated the reset
            // request on its own.  It could do that if there
            // were some sort of error not associated with any particular
            // open.
            //
            NDIS_REQUEST_TYPE ResetRequestType;

            //
            // Possibly undefined request for the reset request.
            //
            PNDIS_REQUEST ResetNdisRequest;

            LOG(RESET_STEP_3);

            ASSERT(!Adapter->FirstInitialization);

            Csr &= ~LANCE_CSR0_INITIALIZATION_DONE;

            Adapter->ResetInProgress = FALSE;
            Adapter->ResetInitStarted = FALSE;

            //
            // We save off the open that caused this reset incase
            // we get *another* reset while we're indicating the
            // last reset is done.
            //

            ResettingOpen = Adapter->ResettingOpen;
            ResetRequestType = Adapter->ResetRequestType;
            ResetNdisRequest = Adapter->ResetNdisRequest;

            //
            // We need to signal every open binding that the
            // reset is complete.  We increment the reference
            // count on the open binding while we're doing indications
            // so that the open can't be deleted out from under
            // us while we're indicating (recall that we can't own
            // the lock during the indication).
            //

            {

                PLANCE_OPEN Open;

                //
                // Look to see which open initiated the reset.
                //
                // If the reset was initiated by an open because it
                // was closing we will let the closing binding loop
                // further on in this routine indicate that the
                // request was complete.
                //
                // If the reset was initiated for some obscure hardware
                // reason that can't be associated with a particular
                // open (e.g. memory error on receiving a packet) then
                // we won't have an initiating request so we can't
                // indicate.  (The ResettingOpen pointer will be
                // NULL in this case.)
                //

#if LANCE_TRACE
                DbgPrint("0x%x 0x%x 0x%x %d %d\n",
                                          Adapter,
                                          ResettingOpen,
                                          ResetNdisRequest,
                                          ResetRequestType,
                                          NdisRequestClose
                                         );
#endif

                if ((ResettingOpen != NULL) &&
                    (ResetRequestType != NdisRequestClose)) {

                    if (ResetNdisRequest != NULL) {

                        //
                        // It was a request submitted by a protocol.
                        //

                        FinishPendOp(Adapter, TRUE);

                    } else {

                        //
                        // It was a request submitted by the MAC or
                        // a reset command.
                        //

                        if (ResetRequestType == NdisRequestGeneric1) {

                            //
                            // Is was a reset request
                            //

                            PLIST_ENTRY CurrentLink;

                            CurrentLink = Adapter->OpenBindings.Flink;

                            while (CurrentLink != &Adapter->OpenBindings) {

                                Open = CONTAINING_RECORD(
                                         CurrentLink,
                                         LANCE_OPEN,
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

                                NdisIndicateStatusComplete(
                                    Open->NdisBindingContext
                                    );

                                NdisDprAcquireSpinLock(&Adapter->Lock);

                                Open->References--;

                                CurrentLink = CurrentLink->Flink;

                            }

#if DBG
                            Adapter->ResettingOpen = NULL;
                            Adapter->ResetNdisRequest = NULL;
                            Adapter->ResetRequestType = (NDIS_REQUEST_TYPE)0;
#endif

#if LANCE_TRACE
                            DbgPrint("Completing reset\n");
#endif

                            NdisDprReleaseSpinLock(&Adapter->Lock);

                            NdisCompleteReset(
                                 ResettingOpen->NdisBindingContext,
                                 NDIS_STATUS_SUCCESS
                                 );


                            NdisDprAcquireSpinLock(&Adapter->Lock);

                            ResettingOpen->References--;

                        }


                    }

                    //
                    // Restart the chip.
                    //

                    LanceStartChip(Adapter);

                } else {

                    //
                    // It was a close that pended (if there is
                    // a ResettingOpen).  Subtract 2, one for the
                    // reset and one for the pended operation.
                    //

                    if (ResettingOpen) {

                        ResettingOpen->References--;

#if DBG
                        Adapter->ResettingOpen = NULL;
                        Adapter->ResetNdisRequest = NULL;
                        Adapter->ResetRequestType = (NDIS_REQUEST_TYPE)0;
#endif

                    }

                    if (!Adapter->ResetInProgress) {

                        LanceStartChip(Adapter);

                    }

                }

            }

            //
            // Fire off any pending operations...
            //

            if (Adapter->PendQueue != NULL) {

                PNDIS_REQUEST NdisRequest;

                NdisRequest = Adapter->PendQueue;

                Adapter->PendQueue =
                    PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->Next;

                if (NdisRequest == Adapter->PendQueueTail) {

                    Adapter->PendQueueTail = NULL;

                }

                if (PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->RequestType ==
                    NdisRequestClose) {

                    SetupForReset(
                        Adapter,
                        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->Open,
                        NULL,
                        NdisRequestClose
                        );

                } else {

                    SetupForReset(
                        Adapter,
                        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->Open,
                        NdisRequest,
                        PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest)->RequestType
                        );

                }

            }

            goto LoopBottom;

        }

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

        if (Adapter->ResetInProgress &&
            !Adapter->ResetInitStarted &&
            (Adapter->References == 1)) {

#if LANCE_TRACE
            DbgPrint("Starting Initialization.\n");
#endif

            StartAdapterReset(Adapter);

            Adapter->ResetInitStarted = TRUE;
            goto LoopBottom;

        }

        //
        // Check for non-packet related errors.
        //

        if (Csr & (LANCE_CSR0_MEMORY_ERROR |
                   LANCE_CSR0_MISSED_PACKET |
                   LANCE_CSR0_BABBLE)) {

            if (Csr & LANCE_CSR0_MISSED_PACKET) {

                Adapter->MissedPacket++;

            } else if (Csr & LANCE_CSR0_BABBLE) {

                //
                // A babble error implies that we've sent a
                // packet that is greater than the ethernet length.
                // This implies that the driver is broken.
                //

                Adapter->Babble++;

                NdisWriteErrorLogEntry(
                    Adapter->NdisAdapterHandle,
                    NDIS_ERROR_CODE_DRIVER_FAILURE,
                    2,
                    (ULONG)processInterrupt,
                    (ULONG)0x1
                    );


            } else {

                //
                // Could only be a memory error.  This shuts down
                // the receiver and the transmitter.  We have to
                // reset to get the device started again.
                //

                Adapter->MemoryError++;

                SetupForReset(
                    Adapter,
                    NULL,
                    NULL,
                    NdisRequestGeneric4 // Means MAC issued
                    );

            }

            Csr &= ~LANCE_CSR0_ERROR_BITS;

        }

        //
        // Check the interrupt vector and see if there are any
        // more receives to process.  After we process any
        // other interrupt source we always come back to the top
        // of the loop to check if any more receive packets have
        // come in.  This is to lessen the probability that we
        // drop a receive.
        //


        if (Csr & LANCE_CSR0_RECEIVER_INTERRUPT) {

            if (ProcessReceiveInterrupts(Adapter)) {

                Csr &= ~LANCE_CSR0_RECEIVER_INTERRUPT;

            }

        }

        //
        // Process the transmit interrupts if there are any.
        //

        if (Csr & LANCE_CSR0_TRANSMITTER_INTERRUPT) {

            //
            // We need to check if the transmitter has
            // stopped as a result of an error.  If it
            // has then we really need to reset the adapter.
            //

            if (!(Csr & LANCE_CSR0_TRANSMITTER_ON)) {

                //
                // Might as well turn off the transmitter interrupt
                // source since we won't ever be processing them
                // and we don't want to come back here again.
                //

                Csr &= ~LANCE_CSR0_TRANSMITTER_INTERRUPT;

                //
                // Before we setup for the reset make sure that
                // we aren't already resetting.
                //

                if (!Adapter->ResetInProgress) {

                    SetupForReset(
                        Adapter,
                        NULL,
                        0,
                        NdisRequestGeneric4 // means MAC issued
                        );

                }

                goto LoopBottom;

            } else {

                if (!ProcessTransmitInterrupts(Adapter)) {

                    //
                    // Process interrupts returns false if it
                    // finds no more work to do.  If this so we
                    // turn off the transmitter interrupt source.
                    //

                    Csr &= ~LANCE_CSR0_TRANSMITTER_INTERRUPT;

                }

            }

        }


        //
        // Only try to push a packet through the stage queues
        // if somebody else isn't already doing it and
        // there is some hope of moving some packets
        // ahead.
        //

        if ((!Adapter->AlreadyProcessingStage) &&
            (Adapter->StageOpen && Adapter->FirstStage1Packet)) {

            LanceStagedAllocation(Adapter);

        }

        //
        // Process the loopback queue.
        //
        //

        if (Adapter->FirstLoopBack != NULL) {

            LanceProcessLoopback(Adapter);

        }

        //
        // If there are any opens on the closing list and their
        // reference counts are zero then complete the close and
        // delete them from the list.
        //
        //

LoopBottom:;

        //
        // NOTE: This code assumes that the above code left
        // the spinlock acquired.
        //
        // Bottom of the interrupt processing loop.  Another dpc
        // could be coming in at this point to process interrupts.
        // We clear the flag that says we're processing interrupts
        // so that some invocation of the DPC can grab it and process
        // any further interrupts.
        //

        if (!IsListEmpty(&Adapter->CloseList)) {

            PLANCE_OPEN Open;
            PLIST_ENTRY Link = &(Adapter->CloseList);


            while (Link->Flink != &(Adapter->CloseList)) {

                Open = CONTAINING_RECORD(
                     Link->Flink,
                     LANCE_OPEN,
                     OpenList
                     );


                if (!Open->References) {

                    NdisDprReleaseSpinLock(&Adapter->Lock);

                    NdisCompleteCloseAdapter(
                        Open->NdisBindingContext,
                        NDIS_STATUS_SUCCESS
                        );

                    NdisDprAcquireSpinLock(&Adapter->Lock);

                    RemoveEntryList(&Open->OpenList);

                    if (Adapter->MaxLookAhead == Open->LookAhead) {

                        LanceAdjustMaxLookAhead(Adapter);

                    }

                    LANCE_FREE_PHYS(Open, sizeof(LANCE_OPEN));

                }

                Link = Link->Flink;

            }


        }

        Adapter->References--;

    }

    Adapter->ProcessInterruptRunning = FALSE;

    //
    // Check if we indicated any packets.
    //
    // Note: The only way to get out of the loop (via the break above) is
    // while we're still holding the spin lock.
    //

    if (Adapter->IndicatedAPacket) {

        Adapter->IndicatedAPacket = FALSE;

        NdisDprReleaseSpinLock(&Adapter->Lock);

        EthFilterIndicateReceiveComplete(Adapter->FilterDB);

        NdisDprAcquireSpinLock(&Adapter->Lock);

    }

}

STATIC
BOOLEAN
ProcessReceiveInterrupts(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    Process the packets that have finished receiving.

    NOTE: This routine assumes that no other thread of execution
    is processing receives!  THE LOCK MUST BE HELD

Arguments:

    Adapter - The adapter to indicate to.

Return Value:

    Whether to clear the interrupt or not.

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
    // Index of the ring descriptor in the ring.
    //
    UINT CurrentIndex = Adapter->CurrentReceiveIndex;

    //
    // Pointer to the ring descriptor being examined.
    //
    PLANCE_RECEIVE_ENTRY CurrentEntry = Adapter->ReceiveRing + CurrentIndex;

    //
    // Hold in a local the top receive ring index so that we don't
    // need to get it from the adapter all the time.
    //
    const UINT TopReceiveIndex = Adapter->NumberOfReceiveRings - 1;

    //
    // Boolean to record the fact that we've finished processing
    // one packet and we're about to start a new one.
    //
    BOOLEAN NewPacket = FALSE;

    //
    // Count of the number of buffers in the current packet.
    //
    UINT NumberOfBuffers = 1;

    //
    // Pointer to host addressable space for the lookahead buffer
    //
    PUCHAR LookaheadBuffer;

    ULONG ReceivePacketCount = 0;

    while (TRUE) {

        UCHAR ReceiveSummaryBits;


        //
        // Check to see whether we own the packet.  If
        // we don't then simply return to the caller.
        //

        LANCE_READ_HARDWARE_MEMORY_UCHAR(
            CurrentEntry->ReceiveSummaryBits,
            &ReceiveSummaryBits
            );

        if (ReceiveSummaryBits & LANCE_RECEIVE_OWNED_BY_CHIP) {

            LOG(RECEIVE);

            return TRUE;

        } else if (ReceivePacketCount > 10) {

            LOG(RECEIVE)

            return FALSE;

        } else if (ReceiveSummaryBits & LANCE_RECEIVE_ERROR_SUMMARY) {


            //
            // We have an error in the packet.  Record
            // the details of the error.
            //

            //
            // Synch with the set/query information routines.
            //

            if (ReceiveSummaryBits & LANCE_RECEIVE_BUFFER_ERROR) {

                //
                // Probably ran out of descriptors.
                //

                Adapter->OutOfReceiveBuffers++;

            } else if (ReceiveSummaryBits & LANCE_RECEIVE_CRC_ERROR) {

                Adapter->CRCError++;

            } else if (ReceiveSummaryBits & LANCE_RECEIVE_OVERFLOW_ERROR) {

                Adapter->OutOfReceiveBuffers++;

            } else if (ReceiveSummaryBits & LANCE_RECEIVE_FRAMING_ERROR) {

                Adapter->FramingError++;

            }

            ReceivePacketCount++;

            //
            // Give the packet back to the hardware.
            //

            RelinquishReceivePacket(
                Adapter,
                Adapter->CurrentReceiveIndex,
                NumberOfBuffers
                );

            NewPacket = TRUE;

        } else if (ReceiveSummaryBits & LANCE_RECEIVE_END_OF_PACKET) {

            //
            // We've reached the end of the packet.  Prepare
            // the parameters for indication, then indicate.
            //

            UINT PacketSize;
            UINT LookAheadSize;

            LANCE_RECEIVE_CONTEXT Context;

            ASSERT(sizeof(LANCE_RECEIVE_CONTEXT) == sizeof(NDIS_HANDLE));

            //
            // Check just before we do indications that we aren't
            // resetting.
            //

            if (Adapter->ResetInProgress) {

                return TRUE;
            }

            Context.INFO.IsContext = TRUE;
            Context.INFO.FirstBuffer = Adapter->CurrentReceiveIndex;
            Context.INFO.LastBuffer = CurrentIndex;

            LANCE_GET_MESSAGE_SIZE(CurrentEntry, PacketSize);

            LookAheadSize = PacketSize;

            //
            // Find amount to indicate.
            //

            LookAheadSize = ((LookAheadSize < Adapter->SizeOfReceiveBuffer) ?
                             LookAheadSize :
                             Adapter->SizeOfReceiveBuffer);

            LookAheadSize -= LANCE_HEADER_SIZE;

            //
            // Increment the number of packets succesfully received.
            //

            Adapter->Receive++;

            LOG(INDICATE);

            Adapter->IndicatingMacReceiveContext = Context;

            Adapter->IndicatedAPacket = TRUE;

            NdisDprReleaseSpinLock(&Adapter->Lock);

            NdisCreateLookaheadBufferFromSharedMemory(
                (PVOID)(Adapter->ReceiveVAs[Adapter->CurrentReceiveIndex]),
                LookAheadSize + LANCE_HEADER_SIZE,
                &LookaheadBuffer
                );

            if (LookaheadBuffer != NULL) {

                if (PacketSize < LANCE_HEADER_SIZE) {

                    if (PacketSize >= ETH_LENGTH_OF_ADDRESS) {

                        //
                        // Runt packet
                        //

                        EthFilterIndicateReceive(
                            Adapter->FilterDB,
                            (NDIS_HANDLE)Context.WholeThing,
                            LookaheadBuffer,
                            LookaheadBuffer,
                            PacketSize,
                            NULL,
                            0,
                            0
                            );

                    }

                } else {

                    EthFilterIndicateReceive(
                        Adapter->FilterDB,
                        (NDIS_HANDLE)Context.WholeThing,
                        LookaheadBuffer,
                        LookaheadBuffer,
                        LANCE_HEADER_SIZE,
                        LookaheadBuffer + LANCE_HEADER_SIZE,
                        LookAheadSize,
                        PacketSize - LANCE_HEADER_SIZE
                        );

                }

                NdisDestroyLookaheadBufferFromSharedMemory(
                    LookaheadBuffer
                    );

            }

            NdisDprAcquireSpinLock(&Adapter->Lock);

            ReceivePacketCount++;

            //
            // Give the packet back to the hardware.
            //

            RelinquishReceivePacket(
                Adapter,
                Adapter->CurrentReceiveIndex,
                NumberOfBuffers
                );

            NewPacket = TRUE;

        }

        //
        // We're at some indermediate packet. Advance to
        // the next one.
        //

        if (CurrentIndex == TopReceiveIndex) {

            CurrentIndex = 0;
            CurrentEntry = Adapter->ReceiveRing;

        } else {

            CurrentIndex++;
            CurrentEntry++;

        }

        if (NewPacket) {

            Adapter->CurrentReceiveIndex = CurrentIndex;
            NewPacket = FALSE;
            NumberOfBuffers = 0;

        }

        NumberOfBuffers++;

        if (NumberOfBuffers > (TopReceiveIndex + 1)) {

            //
            // Error!  For some reason we wrapped without ever seeing
            // the end of packet.  The card is hosed.  Stop the
            // whole process.
            //

            //
            // There are opens to notify
            //

            PLIST_ENTRY CurrentLink;
            PLANCE_OPEN Open;
            BOOLEAN Cancel;

            Adapter->HardwareFailure = TRUE;

            CurrentLink = Adapter->OpenBindings.Flink;

            while (CurrentLink != &Adapter->OpenBindings) {

                Open = CONTAINING_RECORD(
                                         CurrentLink,
                                         LANCE_OPEN,
                                         OpenList
                                        );

                Open->References++;

                NdisDprReleaseSpinLock(&Adapter->Lock);

                NdisIndicateStatus(
                                   Open->NdisBindingContext,
                                   NDIS_STATUS_CLOSING,
                                   NULL,
                                   0
                                  );

                NdisIndicateStatusComplete(
                                   Open->NdisBindingContext
                                   );

                NdisDprAcquireSpinLock(&Adapter->Lock);

                Open->References--;

                CurrentLink = CurrentLink->Flink;

            }

#if LANCELOG

            if (LogTimerRunning) {

                NdisCancelTimer(&LogTimer, &Cancel);
                NdisStallExecution(500000);
                LogTimerRunning = FALSE;

            }

#endif


            NdisRemoveInterrupt(&(Adapter->Interrupt));

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_HARDWARE_FAILURE,
                0
                );

            NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

            return TRUE;

        }

    }

}

STATIC
VOID
RelinquishReceivePacket(
    IN PLANCE_ADAPTER Adapter,
    IN UINT StartingIndex,
    IN UINT NumberOfBuffers
    )

/*++

Routine Description:

    Gives a range of receive descriptors back to the hardware.

    NOTE: MUST BE CALLED WITH THE LOCK HELD!!

Arguments:

    Adapter - The adapter that the ring works with.

    StartingIndex - The first ring to return.  Note that since
    we are dealing with a ring, this value could be greater than
    the EndingIndex.

    NumberOfBuffers - The number of buffers (or ring descriptors) in
    the current packet.

Return Value:

    None.

--*/

{

    //
    // Index of the ring descriptor in the ring.
    //
    UINT CurrentIndex = StartingIndex;

    //
    // Pointer to the ring descriptor being returned.
    //
    PLANCE_RECEIVE_ENTRY CurrentEntry = Adapter->ReceiveRing + CurrentIndex;

    //
    // Hold in a local so that we don't need to access via the adapter.
    //
    const UINT TopReceiveIndex = Adapter->NumberOfReceiveRings - 1;

    UCHAR Tmp;

    LANCE_READ_HARDWARE_MEMORY_UCHAR(
        CurrentEntry->ReceiveSummaryBits,
        &Tmp
        );

    ASSERT(!(Tmp & LANCE_RECEIVE_OWNED_BY_CHIP));
    ASSERT(Tmp & LANCE_RECEIVE_START_OF_PACKET);

    for (
        ;
        NumberOfBuffers;
        NumberOfBuffers--
        ) {

            LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
                CurrentEntry->ReceiveSummaryBits,
                LANCE_RECEIVE_OWNED_BY_CHIP
                );

        if (CurrentIndex == TopReceiveIndex) {

            CurrentEntry = Adapter->ReceiveRing;
            CurrentIndex = 0;

        } else {

            CurrentEntry++;
            CurrentIndex++;

        }

    }

}

STATIC
BOOLEAN
ProcessTransmitInterrupts(
    IN PLANCE_ADAPTER Adapter
    )

/*++

Routine Description:

    Process the packets that have finished transmitting.

    NOTE: This routine assumes that it is being executed in a
    single thread of execution.  CALLED WITH LOCK HELD!!!

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
    UINT FirstIndex;

    //
    // Pointer to the last ring entry for the packet to be transmitted.
    // This pointer might actually point to a ring entry before the first
    // ring entry for the packet since the ring structure is, simply, a ring.
    //
    PLANCE_TRANSMIT_ENTRY LastRingEntry;

    //
    // Pointer to the packet that started this transmission.
    //
    PNDIS_PACKET OwningPacket;

    //
    // First Buffer
    //
    PNDIS_BUFFER FirstBuffer;

    //
    // Virtual address of first buffer
    //
    PVOID BufferVA;

    //
    // Length of the first buffer
    //
    UINT Length;

    //
    // Points to the reserved part of the OwningPacket.
    //
    PLANCE_RESERVED Reserved;

    UCHAR TransmitSummaryBits;
    USHORT ErrorSummaryInfo;

    //
    // Used to hold the ring to packet mapping information so that
    // we can release the ring entries as quickly as possible.
    //
    LANCE_RING_TO_PACKET SavedRingMapping;


    //
    // Get hold of the first transmitted packet.
    //

    //
    // First we check that this is a packet that was transmitted
    // but not already processed.  Recall that this routine
    // will be called repeatedly until this tests false, Or we
    // hit a packet that we don't completely own.
    //

    //
    // NOTE: I found a problem where FirstUncommitedRing wraps around
    // and becomes equal to TransmittingRing.  This only happens when
    // NumberOfAvailableRings is 0 (JohnsonA)
    //

    if ((Adapter->TransmittingRing == Adapter->FirstUncommittedRing) &&
        (Adapter->NumberOfAvailableRings > 0)) {

        return FALSE;

    } else {

        FirstIndex = Adapter->TransmittingRing - Adapter->TransmitRing;

    }


    //
    // We put the mapping into a local variable so that we
    // can return the mapping as soon as possible.
    //

    SavedRingMapping = Adapter->RingToPacket[FirstIndex];

    //
    // Get a pointer to the last ring entry for this packet.
    //

    LastRingEntry = Adapter->TransmitRing +
                     SavedRingMapping.RingIndex;

    //
    // Get a pointer to the owning packet and the reserved part of
    // the packet.
    //

    OwningPacket = SavedRingMapping.OwningPacket;

    SavedRingMapping.OwningPacket = NULL;

    if (OwningPacket == NULL) {

        //
        // We seem to be in a messed up state.  Ignore this interrupt and
        // the wake up dpc will reset the card if necessary.
        //

        ASSERT(OwningPacket != NULL);
        return(FALSE);

    }

    if (Adapter->FirstFinishTransmit == NULL) {

        //
        // We seem to be in a messed up state.  Ignore this interrupt and
        // the wake up dpc will reset the card if necessary.
        //

        ASSERT(Adapter->FirstFinishTransmit != NULL);
        return(FALSE);

    }

    Reserved = PLANCE_RESERVED_FROM_PACKET(OwningPacket);

    //
    // Check that the host does indeed own this entire packet.
    //

    LANCE_READ_HARDWARE_MEMORY_UCHAR(
        LastRingEntry->TransmitSummaryBits,
        &TransmitSummaryBits
        );

    if (TransmitSummaryBits & LANCE_TRANSMIT_OWNED_BY_CHIP) {

        //
        // We don't own this last packet.  We return FALSE to indicate
        // that we don't have any more packets to work on.
        //

        return FALSE;

    } else {

        //
        // Pointer to the current ring descriptor being examine for errors
        // and the statistics accumulated during its transmission.
        //
        PLANCE_TRANSMIT_ENTRY CurrentRingEntry;

        //
        // The binding that is submitting this packet.
        //
        PLANCE_OPEN Open;

        //
        // Holds whether the packet successfully transmitted or not.
        //
        BOOLEAN Successful = TRUE;

        LOG(TRANSMIT_COMPLETE);

        CurrentRingEntry = Adapter->TransmitRing + FirstIndex;

        //
        // now return these buffers to the adapter.
        //

        ReturnAdapterResources(
            Adapter,
            SavedRingMapping.LanceBuffersIndex
            );

        //
        // Since the host owns the entire packet check the ring
        // entries from first to last for any errors in transmission.
        // Any errors found or multiple tries should be recorded in
        // the information structure for the adapter.
        //
        // We treat Late Collisions as success since the packet was
        // fully transmitted and may have been received.
        //

        while (TRUE) {

            LANCE_READ_HARDWARE_MEMORY_UCHAR(
                    CurrentRingEntry->TransmitSummaryBits,
                    &TransmitSummaryBits
                    );

            LANCE_READ_HARDWARE_MEMORY_USHORT(
                    CurrentRingEntry->ErrorSummaryInfo,
                    &ErrorSummaryInfo
                    );

            if ((TransmitSummaryBits & LANCE_TRANSMIT_ANY_ERRORS) &&
                !(ErrorSummaryInfo & LANCE_TRANSMIT_LATE_COLLISION)) {

                if (ErrorSummaryInfo & LANCE_TRANSMIT_RETRY) {

                    Adapter->RetryFailure++;

                } else if (ErrorSummaryInfo & LANCE_TRANSMIT_LOST_CARRIER) {

                    Adapter->LostCarrier++;

                } else if (ErrorSummaryInfo & LANCE_TRANSMIT_UNDERFLOW) {

                    Adapter->UnderFlow++;

                }

#if DBG
                LanceSendFails[LanceSendFailPlace] =
                         (UCHAR)(ErrorSummaryInfo);
                LanceSendFailPlace++;


#endif

#if LANCE_TRACE
                DbgPrint("Unsuccessful Transmit 0x%x\n",
                         ErrorSummaryInfo);
#endif

                Successful = FALSE;

                //
                // Move the pointer to transmitting but unprocessed
                // ring entries to after this packet, and recover
                // the remaining now available ring entries.
                //

                Adapter->NumberOfAvailableRings +=
                (CurrentRingEntry <= LastRingEntry)?
                 ((LastRingEntry - CurrentRingEntry)+1):
                 ((Adapter->LastTransmitRingEntry - CurrentRingEntry) +
                  (LastRingEntry-Adapter->TransmitRing) + 2);

                if (LastRingEntry == Adapter->LastTransmitRingEntry) {

                    Adapter->TransmittingRing = Adapter->TransmitRing;

                } else {

                    Adapter->TransmittingRing = LastRingEntry + 1;

                }

                break;

            } else {

                //
                // Logical variable that records whether this
                // is the last packet.
                //
                BOOLEAN DoneWithPacket =
                    TransmitSummaryBits &
                    LANCE_TRANSMIT_END_OF_PACKET;

                if (ErrorSummaryInfo &
                           LANCE_TRANSMIT_LATE_COLLISION) {

                    Adapter->LateCollision++;

                }

                if (TransmitSummaryBits &
                    LANCE_TRANSMIT_START_OF_PACKET) {

                    //
                    // Collect some statistics on how many tries were needed.
                    //

                    if (TransmitSummaryBits & LANCE_TRANSMIT_DEFERRED) {

                        Adapter->Deferred++;

                    } else if (TransmitSummaryBits & LANCE_TRANSMIT_ONE_RETRY) {

                        Adapter->OneRetry++;

                    } else if (TransmitSummaryBits & LANCE_TRANSMIT_MORE_THAN_ONE_RETRY) {

                        Adapter->MoreThanOneRetry++;


                    }

                }

                if (CurrentRingEntry == Adapter->LastTransmitRingEntry) {

                    CurrentRingEntry = Adapter->TransmitRing;

                } else {

                    CurrentRingEntry++;

                }

                Adapter->TransmittingRing = CurrentRingEntry;
                Adapter->NumberOfAvailableRings++;

                if (DoneWithPacket) {

                    break;

                }

            }

        }

        //
        // Store result
        //

        if (Successful) {

            //
            // Increment number of packets successfully sent.
            //

            Adapter->Transmit++;

        }

        //
        // Remove packet from sending queue
        //

        Adapter->FirstFinishTransmit = Reserved->Next;

        if (Adapter->FirstFinishTransmit == NULL) {

            Adapter->LastFinishTransmit = NULL;

        }

        //
        // Do a quick check to see if the packet has a high likelyhood
        // of needing to loopback.  (NOTE: This means that if the packet
        // must be loopbacked then this function will return true.  If
        // the packet doesn't need to be loopbacked then the function
        // will probably return false.)
        //

        //
        // Get first buffer
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
            &BufferVA,
            &Length
            );

        if (EthShouldAddressLoopBack(
                Adapter->FilterDB,
                BufferVA
                )) {

            Reserved->SuccessfulTransmit = Successful;

            if (!Adapter->FirstLoopBack) {

                Adapter->FirstLoopBack = OwningPacket;

            } else {

                PLANCE_RESERVED_FROM_PACKET(Adapter->LastLoopBack)->Next = OwningPacket;

            }

            Reserved->Next = NULL;
            Adapter->LastLoopBack = OwningPacket;

        } else {


            Open = PLANCE_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

            //
            // Along with at least one reference because of the coming
            // indication there should be a reference because of the
            // packet to indicate.
            //

            ASSERT(Open->References > 0);

            NdisDprReleaseSpinLock(&Adapter->Lock);

            NdisCompleteSend(
                Open->NdisBindingContext,
                OwningPacket,
                ((Successful)?(NDIS_STATUS_SUCCESS):(NDIS_STATUS_FAILURE))
                );

            NdisDprAcquireSpinLock(&Adapter->Lock);

            //
            // Remove reference for packet
            //

            Open->References--;

        }

        //
        // Since we've given back some ring entries we should
        // open of the stage if it was closed and we are not resetting.
        //

        if ((!Adapter->StageOpen) && (!Adapter->ResetInProgress)) {

            Adapter->StageOpen = TRUE;

        }

        return TRUE;
    }

}

STATIC
VOID
ReturnAdapterResources(
    IN PLANCE_ADAPTER Adapter,
    IN UINT BufferIndex
    )

/*++

Routine Description:

    Given that a packet has used adapter resources (which the routine
    asserts), return those resources to the adapter.

    NOTE: CALLED WITH LOCK HELD!!!

Arguments:

    Adapter - The adapter that the packet came through.

    BufferIndex - The adapter buffer descriptor index to put back on the
    free list.


Return Value:

    None.

--*/
{

    //
    // The adapter buffer descriptor that was allocated to this packet.
    //
    PLANCE_BUFFER_DESCRIPTOR BufferDescriptor = Adapter->LanceBuffers +
                                                  BufferIndex;

    //
    // Index of the listhead that heads the list that the adapter
    // buffer descriptor belongs too.
    //
    INT ListHeadIndex = BufferDescriptor->Next;

    //
    // Put the adapter buffer back on the free list.
    //

    BufferDescriptor->Next = Adapter->LanceBufferListHeads[ListHeadIndex];
    Adapter->LanceBufferListHeads[ListHeadIndex] = BufferIndex;

    //
    // If the stage was closed and we aren't resetting then open
    // it back up.
    //

    if ((!Adapter->StageOpen) && (!Adapter->ResetInProgress)) {

        Adapter->StageOpen = TRUE;

    }

}

STATIC
VOID
StartAdapterReset(
    IN PLANCE_ADAPTER Adapter
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
    UINT i;
    PNDIS_PACKET Packet;
    PLANCE_RESERVED Reserved;
    PLANCE_OPEN Open;
    PNDIS_PACKET Next;


#if LANCE_TRACE
    DbgPrint("In StartAdapterReset\n");
#endif

    LOG(RESET_STEP_2);

    //
    // Go through the various transmit lists and....
    //


    for (
        i = 3;
        i > 0;
        i--
        ) {

        switch (i) {

            case 1:
                Next = Adapter->FirstFinishTransmit;
                break;
            case 2:
                Next = Adapter->FirstStage1Packet;
                break;
            case 3:
                Next = Adapter->FirstLoopBack;
                break;

        }


        while (Next) {

            Packet = Next;
            Reserved = PLANCE_RESERVED_FROM_PACKET(Packet);
            Next = Reserved->Next;
            Open =
              PLANCE_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);


            if (Adapter->ResetRequestType == NdisRequestGeneric1) {

                //
                // Abort the packet
                //
                // The completion of the packet is one less reason
                // to keep the open around.
                //

                ASSERT(Open->References);

                Open->References--;

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteSend(
                    Open->NdisBindingContext,
                    Packet,
                    NDIS_STATUS_REQUEST_ABORTED
                    );

                NdisAcquireSpinLock(&Adapter->Lock);

            } else if ((i == 1) || (i ==3)) {

                //
                // Complete these sends
                //
                // The completion of the packet is one less reason
                // to keep the open around.
                //

                ASSERT(Open->References);

                Adapter->Transmit++;

                LOG(RESET_COMPLETE_PACKET);

                NdisReleaseSpinLock(&Adapter->Lock);

                NdisCompleteSend(
                    Open->NdisBindingContext,
                    Packet,
                    NDIS_STATUS_SUCCESS
                    );

                NdisAcquireSpinLock(&Adapter->Lock);

                Open->References--;

            }

        }

    }



    Adapter->CSR0Value = 0;
    Adapter->NumberOfAvailableRings = Adapter->NumberOfTransmitRings;
    Adapter->AllocateableRing = Adapter->TransmitRing;
    Adapter->TransmittingRing = Adapter->TransmitRing;
    Adapter->FirstUncommittedRing = Adapter->TransmitRing;


    Adapter->StageOpen = TRUE;

    Adapter->AlreadyProcessingStage = FALSE;

    Adapter->CurrentReceiveIndex = 0;

    //
    // Clean all of the receive ring entries.
    //

    {

        PLANCE_RECEIVE_ENTRY CurrentReceive = Adapter->ReceiveRing;
        const PLANCE_RECEIVE_ENTRY After = Adapter->ReceiveRing+
                                           Adapter->NumberOfReceiveRings;

        do {

            LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
                CurrentReceive->ReceiveSummaryBits,
                LANCE_RECEIVE_OWNED_BY_CHIP
                );
            CurrentReceive++;


        } while (CurrentReceive != After);

    }


    //
    // Clean all of the transmit ring entries.
    //

    {

        PLANCE_TRANSMIT_ENTRY CurrentTransmit = Adapter->TransmitRing;
        const PLANCE_TRANSMIT_ENTRY After = Adapter->TransmitRing+
                                            Adapter->NumberOfTransmitRings;

        do {

            LANCE_WRITE_HARDWARE_MEMORY_UCHAR(
                CurrentTransmit->TransmitSummaryBits,
                0x00
                );
            CurrentTransmit++;


        } while (CurrentTransmit != After);

    }

    //
    // Recover all of the adapter buffers.
    //

    for (
        i = 0;
        i < (Adapter->NumberOfSmallBuffers +
             Adapter->NumberOfMediumBuffers +
             Adapter->NumberOfLargeBuffers);
        i++
        ) {

        Adapter->LanceBuffers[i].Next = i+1;

    }

    Adapter->LanceBufferListHeads[0] = -1;
    Adapter->LanceBufferListHeads[1] = 0;
    Adapter->LanceBuffers[(Adapter->NumberOfSmallBuffers)-1].Next = -1;
    Adapter->LanceBufferListHeads[2] = Adapter->NumberOfSmallBuffers;
    Adapter->LanceBuffers[(Adapter->NumberOfSmallBuffers +
                           Adapter->NumberOfMediumBuffers)-1].Next = -1;
    Adapter->LanceBufferListHeads[3] = Adapter->NumberOfSmallBuffers +
                                       Adapter->NumberOfMediumBuffers;
    Adapter->LanceBuffers[(Adapter->NumberOfSmallBuffers+
                           Adapter->NumberOfMediumBuffers+
                           Adapter->NumberOfLargeBuffers)-1].Next = -1;

    //
    // If it was a Reset from LanceReset(), clear out all queues.
    //

    if (Adapter->ResetRequestType == NdisRequestGeneric1) {

        Adapter->FirstLoopBack = NULL;
        Adapter->LastLoopBack = NULL;
        Adapter->FirstFinishTransmit = NULL;
        Adapter->LastFinishTransmit = NULL;
        Adapter->FirstStage1Packet = NULL;
        Adapter->LastStage1Packet = NULL;

    } else  {

        //
        // It was a reset from a SetInfo,
        // Clear out only the intermediate queues - the
        // packets were moved out and back to stage1. (sigh)
        //

        Adapter->FirstFinishTransmit = NULL;
        Adapter->LastFinishTransmit = NULL;

        //
        // Re-allocate all the stage 1 packets.
        //

        Next = Adapter->FirstStage1Packet;

        Adapter->FirstStage1Packet = NULL;
        Adapter->LastStage1Packet = NULL;

        while (Next != NULL) {

            LOG(RESET_RECOVER_PACKET);

            Packet = Next,
            Reserved = PLANCE_RESERVED_FROM_PACKET(Packet);
            Next = Reserved->Next;

            SetupAllocate(Adapter, Reserved->MacBindingHandle, Packet);

        }

    }


    SetInitBlockAndInit(Adapter);

#if LANCE_TRACE
    DbgPrint("Out StartAdapterReset\n");
#endif

}

STATIC
VOID
SetInitBlockAndInit(
    IN PLANCE_ADAPTER Adapter
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

    None.

--*/
{

    PHYSICAL_ADDRESS PhysAdr;

    //
    // Fill in the adapters initialization block.
    //

    LanceSetInitializationBlock(Adapter);

    PhysAdr = LANCE_GET_HARDWARE_PHYSICAL_ADDRESS(Adapter,Adapter->InitBlock);

    //
    // Make sure that it does have even byte alignment.
    //

    ASSERT(!(PhysAdr.LowPart & 0x01));

    //
    // Write the address of the initialization block to csr1 and csr2.
    //

    LANCE_WRITE_RAP(
        Adapter,
        LANCE_SELECT_CSR1
        );

    LANCE_WRITE_RDP(
        Adapter,
        LANCE_GET_LOW_PART_ADDRESS(PhysAdr.LowPart)
        );

    LANCE_WRITE_RAP(
        Adapter,
        LANCE_SELECT_CSR2
        );

    LANCE_WRITE_RDP(
        Adapter,
        LANCE_GET_HIGH_PART_ADDRESS(PhysAdr.LowPart)
        );

    //
    // Write to csr0 to initialize the chip.
    //

    LANCE_WRITE_RAP(
        Adapter,
        LANCE_SELECT_CSR0
        );

    LANCE_WRITE_RDP(
        Adapter,
        LANCE_CSR0_INIT_CHIP
        );

}

STATIC
VOID
SetupForReset(
    IN PLANCE_ADAPTER Adapter,
    IN PLANCE_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_REQUEST_TYPE RequestType
    )

/*++

Routine Description:

    This routine is used to fill in the who and why a reset is
    being set up as well as setting the appropriate fields in the
    adapter.

    NOTE: This routine must be called with the lock acquired.

Arguments:

    Adapter - The adapter whose hardware is to be initialized.

    Open - A (possibly NULL) pointer to an lance open structure.
    The reason it could be null is if the adapter is initiating the
    reset on its own.

    NdisRequest - A pointer to the NDIS_REQUEST which requested the reset.

    RequestType - If the open is not null then the request type that
    is causing the reset.

Return Value:

    None.

--*/
{

#if LANCE_TRACE
    DbgPrint("In SetupForReset\n");
#endif

    LOG(RESET_STEP_1);

    //
    // Shut down the chip.  We won't be doing any more work until
    // the reset is complete.
    //

    NdisSynchronizeWithInterrupt(
        &Adapter->Interrupt,
        LanceSyncStopChip,
        (PVOID)Adapter
        );

    //
    // Once the chip is stopped we can't get any more interrupts.
    // Any interrupts that are "queued" for processing could
    // only possibly service this reset.  It is therefore safe for
    // us to clear the adapter global csr value.
    //

    Adapter->CSR0Value = 0;

    Adapter->ResetInProgress = TRUE;
    Adapter->ResetInitStarted = FALSE;

    //
    // Shut down all of the transmit queues so that the
    // transmit portion of the chip will eventually calm down.
    //

    Adapter->StageOpen = FALSE;

    Adapter->ResetNdisRequest = NdisRequest;
    Adapter->ResettingOpen = Open;
    Adapter->ResetRequestType = RequestType;

    //
    // If there is a valid open we should up the reference count
    // so that the open can't be deleted before we indicate that
    // their request is finished.
    //

    if (Open) {

        Open->References++;

    }

#if LANCE_TRACE
    DbgPrint("Out SetupForReset\n");
#endif

}


STATIC
VOID
FinishPendOp(
    IN PLANCE_ADAPTER Adapter,
    IN BOOLEAN Successful
    )

/*++

Routine Description:

    This routine is called when a pended operation completes.
    It calles CompleteRequest if needed and does any other
    cleanup required.

    NOTE: This routine is called with the lock held and
    returns with it held.

    NOTE: This routine assumes that the pended operation to
    be completed was specifically requested by the protocol.


Arguments:

    Adapter - The adapter.

    Successful - Was the pended operation completed successfully.

Return Value:

    None.

--*/

{
    ASSERT(Adapter->ResetNdisRequest != NULL);


    //
    // It was a request for filter change or multicastlist change.
    //

    if (Successful) {

        //
        // complete the operation.
        //


        NdisReleaseSpinLock(&(Adapter->Lock));

        NdisCompleteRequest(
                            Adapter->ResettingOpen->NdisBindingContext,
                            Adapter->ResetNdisRequest,
                            NDIS_STATUS_SUCCESS
                            );

        NdisAcquireSpinLock(&(Adapter->Lock));

        Adapter->ResetNdisRequest = NULL;

        Adapter->ResettingOpen->References--;

    } else {


        //
        // complete the operation.
        //


        NdisReleaseSpinLock(&(Adapter->Lock));

        NdisCompleteRequest(
                            Adapter->ResettingOpen->NdisBindingContext,
                            Adapter->ResetNdisRequest,
                            NDIS_STATUS_FAILURE
                            );

        NdisAcquireSpinLock(&(Adapter->Lock));

        Adapter->ResetNdisRequest = NULL;

        Adapter->ResettingOpen->References--;

    }

    return;

}



STATIC
BOOLEAN
LanceSyncWriteRAP(
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will write
    to the RAP


Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and a pointer to the value to write.


Return Value:

    Always returns true.

--*/

{


    PLANCE_SYNCH_CONTEXT C = Context;

    LANCE_ISR_WRITE_RAP(C->Adapter, C->LocalWrite);

    return FALSE;

}

BOOLEAN
LanceSyncWriteRDP(
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will write
    into the RDP register


Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and a pointer to the value to write.

Return Value:

    Always returns true.

--*/

{


    PLANCE_SYNCH_CONTEXT C = Context;

    LANCE_ISR_WRITE_RDP(C->Adapter, C->LocalWrite);

    return FALSE;


}

STATIC
BOOLEAN
LanceSyncReadRDP(
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will read
    from the RDP.

Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and a pointer to a USHORT to store the value into.

Return Value:

    Always returns true.

--*/

{


    PLANCE_SYNCH_CONTEXT C = Context;

    LANCE_ISR_READ_RDP(C->Adapter, C->LocalRead);

    return FALSE;

}

STATIC
BOOLEAN
LanceSyncWriteNicsr(
    IN PVOID Context
    )
/*++

Routine Description:

    This routine is used by the normal interrupt processing routine
    to synchronize with interrupts from the card.  It will
    Write to the NIC Status Register.

Arguments:

    Context - This is really a pointer to a record type peculiar
    to this routine.  The record contains a pointer to the adapter
    and a pointer to an address which holds the value to write.

Return Value:

    Always returns false.

--*/

{

    PLANCE_SYNCH_CONTEXT C = Context;

    LANCE_ISR_WRITE_NICSR(C->Adapter, C->LocalWrite);

    return FALSE;

}

STATIC
BOOLEAN
LanceSyncStopChip(
    IN PVOID Context
    )

/*++

Routine Description:

    This routine is used to stop a lance.



Arguments:

    Adapter - The adapter for the LANCE to stop.

Return Value:

    FALSE

--*/

{

    PLANCE_ADAPTER Adapter = (PLANCE_ADAPTER)Context;

    //
    // Set the RAP to csr0.
    //

    LANCE_ISR_WRITE_RAP(
        Adapter,
        LANCE_SELECT_CSR0
        );

    //
    // Set the RDP to stop chip.
    //

    LANCE_ISR_WRITE_RDP(
        Adapter,
        LANCE_CSR0_STOP
        );

    if (Adapter->LanceCard & (LANCE_DE201 | LANCE_DE100 | LANCE_DE422)) {

        //
        // Always reset the ACON bit after a stop.
        //

        LANCE_ISR_WRITE_RAP(
            Adapter,
            LANCE_SELECT_CSR3
            );

        LANCE_ISR_WRITE_RDP(
            Adapter,
            LANCE_CSR3_ACON
            );

    }

    //
    // Select CSR0 again.
    //

    LANCE_ISR_WRITE_RAP(
        Adapter,
        LANCE_SELECT_CSR0
        );

    return(FALSE);
}


VOID
LanceWakeUpDpc(
    IN PVOID SystemSpecific1,
    IN PVOID Context,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    )

/*++

Routine Description:

    This DPC routine is queued every 5 seconds to check on the
    queues. If an interrupt was not received
    in the last 5 seconds and there should have been one,
    then we abort all operations.

Arguments:

    Context - Really a pointer to the adapter.

Return Value:

    None.

--*/
{
    PLANCE_ADAPTER Adapter = (PLANCE_ADAPTER)Context;
    PLANCE_OPEN TmpOpen;
    PNDIS_PACKET TransmitPacket;
    PLANCE_PEND_DATA PendOp;
    PNDIS_REQUEST Request;
    PLANCE_RESERVED Reserved;

    UNREFERENCED_PARAMETER(SystemSpecific1);
    UNREFERENCED_PARAMETER(SystemSpecific2);
    UNREFERENCED_PARAMETER(SystemSpecific3);

    NdisDprAcquireSpinLock(&Adapter->Lock);

    if ((Adapter->WakeUpTimeout) &&
        ((Adapter->FirstFinishTransmit != NULL) ||
         (Adapter->FirstStage1Packet != NULL) ||
         (Adapter->PendQueue != NULL) ||
         (Adapter->ResetNdisRequest != NULL))) {

        //
        // We had a pending operation the last time we ran,
        // and it has not been completed...we need to complete
        // it now.

        Adapter->WakeUpTimeout = FALSE;

        Adapter->TimeoutCount++;

        if (Adapter->TimeoutCount < 10) {

            //
            // Limit the number of error log entries
            //

            NdisWriteErrorLogEntry(
                Adapter->NdisAdapterHandle,
                NDIS_ERROR_CODE_HARDWARE_FAILURE,
                0
                );


        }

        if ((Adapter->ResettingOpen != NULL) &&
            (Adapter->ResetRequestType != NdisRequestClose)) {

            if (Adapter->ResetNdisRequest != NULL) {

                //
                // It was a request submitted by a protocol.
                //

                TmpOpen = Adapter->ResettingOpen;

                NdisDprReleaseSpinLock(&(Adapter->Lock));

                NdisCompleteRequest(
                            Adapter->ResettingOpen->NdisBindingContext,
                            Adapter->ResetNdisRequest,
                            NDIS_STATUS_SUCCESS
                            );

                NdisDprAcquireSpinLock(&(Adapter->Lock));

                TmpOpen->References--;

            }

        }

        while (Adapter->PendQueue) {

            Request = Adapter->PendQueue;
            PendOp = PLANCE_PEND_DATA_FROM_PNDIS_REQUEST(Request);

            Adapter->PendQueue = PendOp->Next;

            if (Adapter->PendQueue == NULL){

                //
                // We have just emptied the list.
                //

                Adapter->PendQueueTail = NULL;

            }


            if ((PendOp->Open != NULL) &&
                (PendOp->RequestType != NdisRequestClose)) {

                //
                // It was a request submitted by a protocol.
                //

                TmpOpen = PendOp->Open;

                NdisDprReleaseSpinLock(&(Adapter->Lock));

                NdisCompleteRequest(
                            PendOp->Open->NdisBindingContext,
                            Request,
                            NDIS_STATUS_SUCCESS
                            );

                NdisDprAcquireSpinLock(&(Adapter->Lock));

                TmpOpen->References--;

            }

        }

        while (Adapter->FirstFinishTransmit != NULL) {

            TransmitPacket = Adapter->FirstFinishTransmit;

            Reserved = PLANCE_RESERVED_FROM_PACKET(TransmitPacket);

            Adapter->FirstFinishTransmit = Reserved->Next;

            if (Adapter->FirstFinishTransmit == NULL) {

                Adapter->LastFinishTransmit = NULL;

            }

            TmpOpen = PLANCE_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

            NdisDprReleaseSpinLock(&Adapter->Lock);

            NdisCompleteSend(
                    TmpOpen->NdisBindingContext,
                    TransmitPacket,
                    NDIS_STATUS_SUCCESS
                    );

            NdisDprAcquireSpinLock(&Adapter->Lock);

            TmpOpen->References--;

        }

        while (Adapter->FirstStage1Packet != NULL) {

            TransmitPacket = Adapter->FirstStage1Packet;

            Reserved = PLANCE_RESERVED_FROM_PACKET(TransmitPacket);

            //
            // Remove the packet from the queue.
            //

            Adapter->FirstStage1Packet = Reserved->Next;

            if (Adapter->FirstStage1Packet == NULL) {

                Adapter->LastStage1Packet = NULL;

            }

            TmpOpen = PLANCE_OPEN_FROM_BINDING_HANDLE(Reserved->MacBindingHandle);

            NdisDprReleaseSpinLock(&Adapter->Lock);

            NdisCompleteSend(
                    TmpOpen->NdisBindingContext,
                    TransmitPacket,
                    NDIS_STATUS_SUCCESS
                    );

            NdisDprAcquireSpinLock(&Adapter->Lock);

            TmpOpen->References--;

        }

        Adapter->WakeUpTimeout = FALSE;

        SetupForReset(
            Adapter,
            NULL,
            NULL,
            NdisRequestGeneric4 // Means MAC issued
            );

        NdisSetTimer(&Adapter->DeferredTimer, 0);

        NdisDprReleaseSpinLock(&Adapter->Lock);

    } else {

        if ((Adapter->FirstFinishTransmit != NULL) ||
            (Adapter->FirstStage1Packet != NULL) ||
            (Adapter->PendQueue != NULL) ||
            (Adapter->ResetNdisRequest != NULL)) {

            Adapter->WakeUpTimeout = TRUE;

        }

        NdisDprReleaseSpinLock(&Adapter->Lock);


    }

    //
    // Fire off another Dpc to execute after 5 seconds
    //

    NdisSetTimer(
        &Adapter->WakeUpTimer,
        5000
        );

}



