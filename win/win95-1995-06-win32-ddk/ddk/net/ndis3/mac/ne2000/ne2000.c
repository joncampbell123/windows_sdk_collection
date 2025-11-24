/*++

Copyright (c) 1993-95  Microsoft Corporation

Module Name:

    ne2000.c

Abstract:

    This is the main file for the Novell 2000
    Ethernet controller.  This driver conforms to the NDIS 3.0 interface.

Environment:

    Kernel Mode - Or whatever is the equivalent on OS/2 and DOS.

--*/

#include <ndis.h>
#include <efilter.h>
#include "ne2000hw.h"
#include "ne2000sw.h"

#if DBG
#define STATIC
#else
#define STATIC static
#endif

#if DBG

extern ULONG Ne2000SendsCompletedForReset;
ULONG Ne2000DebugFlag=NE2000_DEBUG_LOG ;
extern ULONG Ne2000SendsCompletedForReset;

#endif

//
// This constant is used for places where NdisAllocateMemory
// needs to be called and the HighestAcceptableAddress does
// not matter.
//

NDIS_PHYSICAL_ADDRESS HighestAcceptableMax =
    NDIS_PHYSICAL_ADDRESS_CONST(-1,-1);

//
// The global MAC block.
//

MAC_BLOCK Ne2000MacBlock={0};

//
// If you add to this, make sure to add the
// a case in Ne2000FillInGlobalData() and in
// Ne2000QueryGlobalStatistics() if global
// information only or
// Ne2000QueryProtocolStatistics() if it is
// protocol queriable information.
//

STATIC UINT Ne2000GlobalSupportedOids[] = {
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
// a case in Ne2000QueryGlobalStatistics() and in
// Ne2000QueryProtocolInformation()
//

STATIC UINT Ne2000ProtocolSupportedOids[] = {
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
// Determines whether failing the initial card test will prevent
// the adapter from being registered.
//

#ifdef CARD_TEST

BOOLEAN InitialCardTest = TRUE;

#else  // CARD_TEST

BOOLEAN InitialCardTest = FALSE;

#endif // CARD_TEST


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

    This is the transfer address of the driver. It initializes
    Ne2000MacBlock and calls NdisInitializeWrapper() and
    NdisRegisterMac().

Arguments:

Return Value:

    Indicates the success or failure of the initialization.

--*/

{
    NDIS_HANDLE NdisWrapperHandle;
    PMAC_BLOCK NewMac = &Ne2000MacBlock;
    NDIS_STATUS Status;

#ifdef NE2000
    NDIS_STRING MacName = NDIS_STRING_CONST("Ne2000");
#else
    NDIS_STRING MacName = NDIS_STRING_CONST("Ne1000");
#endif

#if NDIS_WIN
    UCHAR pIds[sizeof (EISA_MCA_ADAPTER_IDS) + 3 * sizeof (USHORT)];
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nEisaAdapters=0;
    ((PEISA_MCA_ADAPTER_IDS)pIds)->nMcaAdapters=3;
    *((PUSHORT)(((PEISA_MCA_ADAPTER_IDS)pIds)->IdArray) + 0)=AE2_ADAPTER_ID;
    *((PUSHORT)(((PEISA_MCA_ADAPTER_IDS)pIds)->IdArray) + 1)=UB_ADAPTER_ID;
    *((PUSHORT)(((PEISA_MCA_ADAPTER_IDS)pIds)->IdArray) + 2)=NE2_ADAPTER_ID;
    (PVOID) DriverObject = (PVOID) pIds;
#endif

    //
    // Ensure that the MAC_RESERVED structure will fit in the
    // MacReserved section of a packet.
    //

    ASSERT(sizeof(MAC_RESERVED) <= sizeof(((PNDIS_PACKET)NULL)->MacReserved));

    //
    // Pass the wrapper a pointer to the device object.
    //

    NdisInitializeWrapper(&NdisWrapperHandle,
              DriverObject,
              RegistryPath,
              NULL
             );

    //
    // Set up the driver object.
    //

    NewMac->DriverObject = DriverObject;
    NdisAllocateSpinLock(&NewMac->SpinLock);
    NewMac->NdisWrapperHandle = NdisWrapperHandle;
    NewMac->Unloading = FALSE;
    NewMac->AdapterQueue = (PNE2000_ADAPTER)NULL;

    //
    // Prepare to call NdisRegisterMac.
    //

    NewMac->MacCharacteristics.MajorNdisVersion = NE2000_NDIS_MAJOR_VERSION;
    NewMac->MacCharacteristics.MinorNdisVersion = NE2000_NDIS_MINOR_VERSION;
    NewMac->MacCharacteristics.Reserved = 0;
    NewMac->MacCharacteristics.OpenAdapterHandler  = Ne2000OpenAdapter;
    NewMac->MacCharacteristics.CloseAdapterHandler = Ne2000CloseAdapter;
    NewMac->MacCharacteristics.SendHandler        =  Ne2000Send;
    NewMac->MacCharacteristics.TransferDataHandler = Ne2000TransferData;
    NewMac->MacCharacteristics.ResetHandler        = Ne2000Reset;
    NewMac->MacCharacteristics.RequestHandler        = Ne2000Request;
    NewMac->MacCharacteristics.QueryGlobalStatisticsHandler =
              Ne2000QueryGlobalStatistics;
    NewMac->MacCharacteristics.UnloadMacHandler       = Ne2000Unload;
    NewMac->MacCharacteristics.AddAdapterHandler      = Ne2000AddAdapter;
    NewMac->MacCharacteristics.RemoveAdapterHandler   = Ne2000RemoveAdapter;
    NewMac->MacCharacteristics.Name = MacName;
    
    NdisRegisterMac(&Status,
        &NewMac->NdisMacHandle,
        NdisWrapperHandle,
        (NDIS_HANDLE)&Ne2000MacBlock,
        &NewMac->MacCharacteristics,
        sizeof(NewMac->MacCharacteristics));

    if (Status != NDIS_STATUS_SUCCESS) {

        //
        // NdisRegisterMac failed.
        //

        NdisFreeSpinLock(&NewMac->SpinLock);
        NdisTerminateWrapper(NdisWrapperHandle,
                            (PVOID) NULL
                            );
                
        IF_LOUD( DbgPrint( "NdisRegisterMac failed with code 0x%x\n", Status ); )
        
        return Status;
    }

    IF_LOUD( DbgPrint( "NdisRegisterMac succeeded\n" );)

    IF_LOUD( DbgPrint("Adapter Initialization Complete\n");)

    return NDIS_STATUS_SUCCESS;
}



NDIS_STATUS
Ne2000AddAdapter(
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING AdapterName
    )
/*++
Routine Description:

    This is the Ne2000 MacAddAdapter routine.    The system calls this routine
    to add support for a particular Ne2000 adapter.  This routine extracts
    configuration information from the configuration data base and registers
    the adapter with NDIS.

Arguments:

    See Ndis3.0 spec...

Return Value:

    NDIS_STATUS_SUCCESS - Adapter was successfully added.
    NDIS_STATUS_FAILURE - Adapter was not added, also MAC deregistered.

--*/

{
    PNE2000_ADAPTER Adapter;
    NDIS_HANDLE ConfigHandle;
    PNDIS_CONFIGURATION_PARAMETER ReturnedValue;

    NDIS_STRING IOAddressStr = NDIS_STRING_CONST("IOBASE");
    NDIS_STRING InterruptStr = NDIS_STRING_CONST("INTERRUPT");
    NDIS_STRING MaxMulticastListStr = NDIS_STRING_CONST("MAXMULTICAST");
    NDIS_STRING NetworkAddressStr = NDIS_STRING_CONST("NETADDRESS");
    NDIS_STRING BusTypeStr = NDIS_STRING_CONST("BusType");
    
    BOOLEAN ConfigError = FALSE;
    ULONG ConfigErrorValue = 0;

    UINT SlotNumber = 0;
    NDIS_MCA_POS_DATA McaData;
    BOOLEAN SkipIobaseAndInterrupt = FALSE;

    PMAC_BLOCK NewMac = (PMAC_BLOCK)(MacMacContext);
    NDIS_STATUS Status;
    PVOID NetAddress;
    ULONG Length;

    //
    // These are used when calling Ne2000RegisterAdapter.
    //

    PVOID IoBaseAddr;
    CCHAR InterruptNumber;
    UINT MaxMulticastList;

    //
    // Set default values.
    //

    IoBaseAddr = DEFAULT_IOBASEADDR;
    InterruptNumber = DEFAULT_INTERRUPTNUMBER;
    MaxMulticastList = DEFAULT_MULTICASTLISTMAX;

    //
    // Allocate memory for the adapter block now.
    //

    Status = NdisAllocateMemory( (PVOID *)&Adapter,
                 sizeof(NE2000_ADAPTER),
                 0,
                 HighestAcceptableMax
                 );

    if (Status != NDIS_STATUS_SUCCESS) {

        return(Status);

    }

    NdisZeroMemory (Adapter, sizeof(NE2000_ADAPTER));

    NdisOpenConfiguration(
            &Status,
            &ConfigHandle,
            ConfigurationHandle
            );

    if (Status != NDIS_STATUS_SUCCESS) {

        NdisFreeMemory(Adapter, sizeof(NE2000_ADAPTER), 0);

        return NDIS_STATUS_FAILURE;

    }

#if NDIS_NT

    //
    // Disallow multiple adapters in the same MP machine because of hardware
    // problems this results in random packet corruption.
    //

    if ((*KeNumberProcessors > 1) && (Ne2000MacBlock.AdapterQueue != NULL)) {

        ConfigError = TRUE;
        ConfigErrorValue = (ULONG)NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;
        goto RegisterAdapter;

        return(NDIS_STATUS_FAILURE);

    }

#endif

    //
    // Read Bus Type (for NE2/AE2 MCA support)
    //

    NdisReadConfiguration(
            &Status,
            &ReturnedValue,
            ConfigHandle,
            &BusTypeStr,
            NdisParameterHexInteger
            );

    if (Status == NDIS_STATUS_SUCCESS) {

        Adapter->BusType = ReturnedValue->ParameterData.IntegerData;

    }

    if (Adapter->BusType == NdisInterfaceMca) {

        NdisReadMcaPosInformation(
                    &Status,
                    ConfigurationHandle,
                    &SlotNumber,
                    &McaData
                    );

//  The following code is valid, except for this particular driver, there
//  are MCA cards in the field that are NE2000 clones, but are not listed
//  within this driver or do not support the ability to read POS data.  These 
//  cards will be treated as if they where ISA cards FOR THIS DRIVER ONLY!
  
//        if (Status != NDIS_STATUS_SUCCESS) {

//            ConfigError = TRUE;
//            ConfigErrorValue = NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION;
//            goto RegisterAdapter;

//        }

        //
        // Interpret POS data
        //
        if (McaData.AdapterId == AE2_ADAPTER_ID ){ 
            SkipIobaseAndInterrupt = TRUE;
            switch ((McaData.PosData1 & MC_IO_BASE_MASK)>>1) {
                case 0x01:
                    IoBaseAddr = (PVOID)0x1000;
                    break;
                case 0x02:
                    IoBaseAddr = (PVOID)0x2020;
                    break;
                case 0x03:
                    IoBaseAddr = (PVOID)0x8020;
                    break;
                case 0x04:
                    IoBaseAddr = (PVOID)0x0300;
                    break;
                case 0x05:
                    IoBaseAddr = (PVOID)0x0320;
                    break;
                case 0x06:
                    IoBaseAddr = (PVOID)0x0340;
                    break;
                case 0x07:
                    IoBaseAddr = (PVOID)0x0360;
                    break;
            }
            switch ((McaData.PosData1 & MC_IRQ_MASK)>>5) {
                case 0x00:
                    InterruptNumber = 3;
                    break;
                case 0x01:
                    InterruptNumber = 4;
                    break;
                case 0x02:
                    InterruptNumber = 5;
                    break;
                case 0x03:
                    InterruptNumber = 9;
                    break;
                }
                
        } else if (McaData.AdapterId == NE2_ADAPTER_ID ){ 
            SkipIobaseAndInterrupt = TRUE;
            switch ((McaData.PosData1 & MC_IO_BASE_MASK)>>1) {
                case 0x01:
                    IoBaseAddr = (PVOID)0x1000;
                    break;
                case 0x02:
                    IoBaseAddr = (PVOID)0x2020;
                    break;
                case 0x03:
                    IoBaseAddr = (PVOID)0x8020;
                    break;
                case 0x04:
                    IoBaseAddr = (PVOID)0xa0a0;
                    break;
                case 0x05:
                    IoBaseAddr = (PVOID)0xb0b0;
                    break;
                case 0x06:
                    IoBaseAddr = (PVOID)0xc0c0;
                    break;
                case 0x07:
                    IoBaseAddr = (PVOID)0xc3d0;
                    break;
            }
            switch ((McaData.PosData1 & MC_IRQ_MASK)>>5) {
                case 0x00:
                    InterruptNumber = 4;
                    break;
                case 0x01:
                    InterruptNumber = 5;
                    break;
                case 0x02:
                    InterruptNumber = 9;
                    break;
                case 0x03:
                    InterruptNumber = 10;
                    break;
                case 0x04:
                    InterruptNumber = 11;
                    break;
                case 0x05:
                    InterruptNumber = 12;
                    break;
                case 0x06:
                    InterruptNumber = 15;
                    break;
            }

        }
#if 0 // apparently, you can't read the pos info for a UB EtherNext.  H/W problem
     // This is one of the cards I mentioned above.                                 
        } else {  //UB EtherNext card
                switch ((McaData.PosData1 & MC_IO_BASE_MASK_UB)>>5) {
                    case 0x01:
                        IoBaseAddr = (PVOID)0x0340;
                        break;
                    case 0x02:
                        IoBaseAddr = (PVOID)0x0320;
                        break;
                    case 0x03:
                        IoBaseAddr = (PVOID)0x0360;
                        break;
                    case 0x04:
                        IoBaseAddr = (PVOID)0x1300;
                        break;
                    case 0x05:
                        IoBaseAddr = (PVOID)0x1340;
                        break;
                    case 0x06:
                        IoBaseAddr = (PVOID)0x1320;
                        break;
                    case 0x07:
                        IoBaseAddr = (PVOID)0x1360;
                        break;
                }
                switch ((McaData.PosData1 & MC_IRQ_MASK_UB)>>1) {
                    case 0x00:
                        InterruptNumber = 3;
                        break;
                    case 0x01:
                        InterruptNumber = 4;
                        break;
                    case 0x02:
                        InterruptNumber = 5;
                        break;
                    case 0x03:
                        InterruptNumber = 10;
                        break;
                    case 0x04:
                        InterruptNumber = 11;
                        break;
                    case 0x05:
                        InterruptNumber = 12;
                        break;
                    case 0x06:
                        InterruptNumber = 15;
                        break;
                }
        }
#endif // 0
    } 

    if (!SkipIobaseAndInterrupt) {
    
        //
        // Read I/O Address
        //

        NdisReadConfiguration(
                &Status,
                &ReturnedValue,
                ConfigHandle,
                &IOAddressStr,
                NdisParameterHexInteger
                );

        if (Status == NDIS_STATUS_SUCCESS) {

            IoBaseAddr = (PVOID)(ReturnedValue->ParameterData.IntegerData);

        }

        if ((IoBaseAddr < (PVOID)MIN_IOBASEADDR) ||
            (IoBaseAddr > (PVOID)MAX_IOBASEADDR)) {

            ConfigError = TRUE;
            ConfigErrorValue = (ULONG)IoBaseAddr;
            goto RegisterAdapter;

        }

        //
        // Read interrupt number
        //
        
#if NDIS_NT
        NdisReadConfiguration(
                &Status,
                &ReturnedValue,
                ConfigHandle,
                &InterruptStr,
                NdisParameterHexInteger
                );
#endif

#if NDIS_WIN            
        NdisReadConfiguration(
                &Status,
                &ReturnedValue,
                ConfigHandle,
                &InterruptStr,
                NdisParameterInteger    // NDIS 2 driver uses decimal, not hex,
                                      // so we use what they use
                );
#endif

        if (Status == NDIS_STATUS_SUCCESS) {

            InterruptNumber = (CCHAR)(ReturnedValue->ParameterData.IntegerData);

        }

        if ((InterruptNumber < MIN_IRQ) ||
            (InterruptNumber > MAX_IRQ)) {

            ConfigError = TRUE;
            ConfigErrorValue = (ULONG)InterruptNumber;
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
            Adapter->StationAddress,
            NetAddress
            );

    }

RegisterAdapter:

    NdisCloseConfiguration(ConfigHandle);

    IF_LOUD( DbgPrint( "Registering adapter # buffers %ld, "
        "I/O base addr 0x%lx, interrupt number %ld, "
        "max multicast %ld\n",
        DEFAULT_NUMBUFFERS, IoBaseAddr,
        InterruptNumber,
        DEFAULT_MULTICASTLISTMAX );)

    //
    // Set up the parameters.
    //
    Adapter->NumBuffers = DEFAULT_NUMBUFFERS;
    Adapter->IoBaseAddr = IoBaseAddr;
    Adapter->InterruptNumber = InterruptNumber;
    Adapter->MulticastListMax = MaxMulticastList;

    if (Ne2000RegisterAdapter(Adapter,
          AdapterName,
          ConfigurationHandle,
          ConfigError,
          ConfigErrorValue
          ) != NDIS_STATUS_SUCCESS) {

        //
        // Ne2000RegisterAdapter failed.
        //
        NdisFreeMemory(Adapter, sizeof(NE2000_ADAPTER), 0);
    
        return NDIS_STATUS_FAILURE;
        
    }

    IF_LOUD( DbgPrint( "Ne2000RegisterAdapter succeeded\n" );)

    return NDIS_STATUS_SUCCESS;
}



NDIS_STATUS
Ne2000RegisterAdapter(
    IN PNE2000_ADAPTER Adapter,
    IN PNDIS_STRING AdapterName,
    IN NDIS_HANDLE ConfigurationHandle,
    IN BOOLEAN ConfigError,
    IN ULONG ConfigErrorValue
    )

/*++

Routine Description:

    Called when a new adapter should be registered. It allocates space for
    the adapter and open blocks, initializes the adapters block, and
    calls NdisRegisterAdapter().

Arguments:

    Adapter - The adapter structure.

    AdapterName - Pointer to the name for this adapter.

    ConfigurationHandle - Handle passed to MacAddAdapter.

    ConfigError - Was there an error during configuration reading.

    ConfigErrorValue - Value to log if there is an error.

Return Value:

    Indicates the success or failure of the registration.

--*/

{
    UINT i;

    NDIS_STATUS status;    //general purpose return from NDIS calls
    NDIS_ADAPTER_INFORMATION AdapterInformation;  // needed to register adapter

    //
    // check that NumBuffers <= MAX_XMIT_BUFS
    //

    if (Adapter->NumBuffers > MAX_XMIT_BUFS) {

        status = NDIS_STATUS_RESOURCES;

        goto fail1;

    }

    // Adapter->OpenQueue = (PNE2000_OPEN)NULL;

    //
    // The adapter is initialized, register it with NDIS.
    // This must occur before interrupts are enabled since the
    // InitializeInterrupt routine requires the NdisAdapterHandle
    //

    //
    // Set up the AdapterInformation structure; zero it
    // first in case it is extended later.
    //
    
    NdisZeroMemory (&AdapterInformation, sizeof(NDIS_ADAPTER_INFORMATION));

    AdapterInformation.AdapterType = NdisInterfaceIsa;
    AdapterInformation.NumberOfPortDescriptors = 1;
    AdapterInformation.PortDescriptors[0].InitialPort = (ULONG)Adapter->IoBaseAddr;
    AdapterInformation.PortDescriptors[0].NumberOfPorts = 0x20;

    if ((status = NdisRegisterAdapter(&Adapter->NdisAdapterHandle,
                Ne2000MacBlock.NdisMacHandle,
                (NDIS_HANDLE)Adapter,
                ConfigurationHandle,
                AdapterName,
                &AdapterInformation))
        != NDIS_STATUS_SUCCESS) {

        //
        // NdisRegisterAdapter failed.
        //

        goto fail2;
    }

    //
    // Allocate the Spin lock.
    //
    NdisAllocateSpinLock(&Adapter->Lock);

    if (ConfigError) {

       //
        // Log Error and exit.
        //

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION,
            1,
            ConfigErrorValue
            );

        goto fail3;

    }

    //
    // Initialize Pending information
    //
    // Adapter->PendQueue   = (PNE2000_PEND_DATA)NULL;
    // Adapter->PendQTail   = (PNE2000_PEND_DATA)NULL;
    // Adapter->PendOp      = (PNE2000_PEND_DATA)NULL;
    Adapter->DeferredDpc = (PVOID)HandlePendingOperations;

    //
    // Initialize References.
    //
    // Adapter->References = 0;

    NdisInitializeTimer(&(Adapter->DeferredTimer),
            Adapter->DeferredDpc,
            Adapter);

    //
    // For now, zero out the copy of the card multicast registers...
    //

    // for (i = 0; i < 8; i++) {
    //
    //    Adapter->NicMulticastRegs[i] = 0;
    //
    // }

    //
    // Set up the port addresses of the card.
    //

    Adapter->IoPAddr = (ULONG)Adapter->IoBaseAddr;

    //
    // Check that the IoBaseAddress seems to be correct.
    //

    IF_VERY_LOUD( DbgPrint("Checking Parameters\n"); )

    if (!CardCheckParameters(Adapter)) {

        //
        // The card does not seem to be there, fail silently.
        //

        IF_VERY_LOUD( DbgPrint("  -- Failed\n"); )

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
            0
            );

        status = NDIS_STATUS_ADAPTER_NOT_FOUND;

        goto fail3;

    }

    IF_VERY_LOUD( DbgPrint("  -- Success\n"); )

    //
    // Initialize the card.
    //

    IF_VERY_LOUD( DbgPrint("CardInitialize\n"); )

    if (!CardInitialize(Adapter)) {

        //
        // Card seems to have failed.
        //

        IF_VERY_LOUD( DbgPrint("  -- Failed\n"); )

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
            0
            );

        status = NDIS_STATUS_ADAPTER_NOT_FOUND;

        goto fail3;

    }

    IF_VERY_LOUD( DbgPrint("  -- Success\n"); )

    //
    //
    // For programmed I/O, we will refer to transmit/receive memory in
    // terms of offsets in the card's 64K address space.
    //

    Adapter->XmitStart = Adapter->RamBase;

    //
    // For the NicXXX fields, always use the addressing system
    // containing the MSB only).
    //

    Adapter->NicXmitStart = (UCHAR)(((ULONG)Adapter->XmitStart) >> 8);

    //
    // The start of the receive space.
    //

    Adapter->PageStart = Adapter->XmitStart +
            (Adapter->NumBuffers * TX_BUF_SIZE);

    Adapter->NicPageStart = Adapter->NicXmitStart +
            (UCHAR)(Adapter->NumBuffers * BUFS_PER_TX);

    ASSERT(Adapter->PageStart < (Adapter->RamBase + Adapter->RamSize));

    //
    // The end of the receive space.
    //

    Adapter->PageStop = Adapter->XmitStart + Adapter->RamSize;
    Adapter->NicPageStop = Adapter->NicXmitStart + (UCHAR)(Adapter->RamSize >> 8);

    ASSERT(Adapter->PageStop <= (Adapter->RamBase + Adapter->RamSize));

    IF_LOUD( DbgPrint("Xmit Start (0x%x, 0x%x) : Rcv Start (0x%x, 0x%x) : Rcv End (0x%x, 0x%x)\n",
              Adapter->XmitStart,
              Adapter->NicXmitStart,
              Adapter->PageStart,
              Adapter->NicPageStart,
              (ULONG)Adapter->PageStop,
              Adapter->NicPageStop
             );
       )

    //
    // Initialize the receive variables.
    //

    Adapter->NicReceiveConfig = RCR_REJECT_ERR;
    Adapter->DpcInProgress = FALSE;
    // Adapter->NicNextPacket  = 0;
    // Adapter->Current = 0;

    //
    // Initialize the transmit buffer control.
    //
    Adapter->CurBufXmitting = -1;
    
    // Adapter->TransmitInterruptPending = FALSE;
    // Adapter->OctoCount = 0;
    // Adapter->OverflowRestartXmitDpc = FALSE;
    //

    for (i=0; i<Adapter->NumBuffers; i++) {

        Adapter->BufferStatus[i] = EMPTY;
        //    Adapter->Packets[i] = (PNDIS_PACKET)NULL;
        //    Adapter->PacketLens[i] = 0;

    }

    // Adapter->ResetInProgress = FALSE;
    // Adapter->TransmitInterruptPending = FALSE;
    // Adapter->OctoCount = 0;

    Adapter->WakeUpFoundTransmit = FALSE;

    //
    // Clear Interrupt Information
    //

    Adapter->Ne2000HandleXmitCompleteRunning = FALSE;

    //
    // The transmit and loopback queues start out empty.
    //

    // Adapter->XmitQueue = (PNDIS_PACKET)NULL;
    // Adapter->XmitQTail = (PNDIS_PACKET)NULL;
    // Adapter->LoopbackQueue = (PNDIS_PACKET)NULL;
    // Adapter->LoopbackQTail = (PNDIS_PACKET)NULL;
    // Adapter->LoopbackPacket= (PNDIS_PACKET)NULL;

    //
    // Clear the tally counters.
    //

    // Adapter->FramesXmitGood = 0;
    // Adapter->FramesRcvGood = 0;
    // Adapter->FramesXmitBad = 0;
    // Adapter->FramesXmitOneCollision = 0;
    // Adapter->FramesXmitManyCollisions = 0;
    // Adapter->FrameAlignmentErrors = 0;
    // Adapter->CrcErrors = 0;
    // Adapter->MissedPackets = 0;

    //
    // Read the Ethernet address off of the PROM.
    //

    CardReadEthernetAddress(Adapter);

    //
    // Initialize Filter Database
    //
    if (!EthCreateFilter(    Adapter->MulticastListMax,
                 Ne2000ChangeMulticastAddresses,
                 Ne2000ChangeFilterClasses,
                 Ne2000CloseAction,
                 Adapter->StationAddress,
                 &Adapter->Lock,
                 &Adapter->FilterDB
                 )) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            0
            );

        status = NDIS_STATUS_FAILURE;

        goto fail3;

    }

    //
    // Now initialize the NIC and Gate Array registers.
    //

    Adapter->NicInterruptMask =
        IMR_RCV | IMR_XMIT | IMR_XMIT_ERR | IMR_OVERFLOW;

    //
    // Link us on to the chain of adapters for this MAC.
    //

    Adapter->MacBlock = &Ne2000MacBlock;

    NdisAcquireSpinLock(&Ne2000MacBlock.SpinLock);

    Adapter->NextAdapter = Ne2000MacBlock.AdapterQueue;
    Ne2000MacBlock.AdapterQueue = Adapter;

    NdisReleaseSpinLock(&Ne2000MacBlock.SpinLock);

    //
    // Setup the card based on the initialization information
    //

    IF_VERY_LOUD( DbgPrint("Setup\n"); )

    if (!CardSetup(Adapter)) {

        //
        // The NIC could not be written to.
        //

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_ADAPTER_NOT_FOUND,
            0
            );

        IF_VERY_LOUD( DbgPrint("  -- Failed\n"); )

        status = NDIS_STATUS_ADAPTER_NOT_FOUND;

        goto fail6;
        
    }

    IF_VERY_LOUD( DbgPrint("  -- Success\n"); )

    //
    // Setup handlers
    //

    NdisInitializeInterrupt(&status,           // status of call
        &Adapter->NdisInterrupt,       // interrupt info str
        Adapter->NdisAdapterHandle,    // NDIS adapter handle
        Ne2000Isr,                     // ptr to ISR
        Adapter,                       // context for ISR, DPC
        Ne2000Dpc,                     // ptr to int DPC
        Adapter->InterruptNumber,      // vector
        Adapter->InterruptNumber,      // level
        FALSE,                         // NOT shared
        NdisInterruptLatched           // InterruptMode
        );

    if (status != NDIS_STATUS_SUCCESS) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_INTERRUPT_CONNECT,
            0
            );

        goto fail6;
        
    }

    NdisInitializeTimer(&Adapter->XmitInterruptTimer,
                (PVOID)Ne2000Dpc, Adapter );
                
    NdisInitializeTimer(&Adapter->LoopbackQueueTimer,
                (PVOID)Ne2000Dpc, Adapter );

    //
    // Initialize the wake up timer to catch transmits that
    // don't interrupt when complete. It fires continuously
    // every two seconds, and we check if there are any
    // uncompleted sends from the previous two-second
    // period.
    //

    Adapter->WakeUpDpc = (PVOID)Ne2000WakeUpDpc;

    NdisInitializeTimer(&Adapter->WakeUpTimer,
            (PVOID)(Adapter->WakeUpDpc),
            Adapter );

    NdisSetTimer(
            &Adapter->WakeUpTimer,
            2000
            );

    Adapter->Removed = FALSE;

    IF_LOUD( DbgPrint("Interrupt Connected\n");)

    //
    // Initialization completed successfully.
    //

    IF_LOUD( DbgPrint(" [ Ne2000 ] : OK\n");)

    return NDIS_STATUS_SUCCESS;

    //
    // Code to unwind what has already been set up when a part of
    // initialization fails, which is jumped into at various
    // points based on where the failure occured. Jumping to
    // a higher-numbered failure point will execute the code
    // for that block and all lower-numbered ones.
    //

fail6:
    NdisAcquireSpinLock(&Ne2000MacBlock.SpinLock);

    //
    // Take us out of the AdapterQueue.
    //

    if (Ne2000MacBlock.AdapterQueue == Adapter) {

        Ne2000MacBlock.AdapterQueue = Adapter->NextAdapter;

    } else {

        PNE2000_ADAPTER TmpAdapter = Ne2000MacBlock.AdapterQueue;

        while (TmpAdapter->NextAdapter != Adapter) {

            TmpAdapter = TmpAdapter->NextAdapter;

        }

        TmpAdapter->NextAdapter = TmpAdapter->NextAdapter->NextAdapter;
        
    }

    NdisReleaseSpinLock(&Ne2000MacBlock.SpinLock);

    //
    // We already enabled the interrupt on the card, so
    // turn it off.
    //

    NdisRawWritePortUchar(
               Adapter->IoPAddr+NIC_COMMAND,
               CR_STOP
              );

    EthDeleteFilter(Adapter->FilterDB);

fail3:
    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

fail2:
    NdisFreeSpinLock(&Adapter->Lock);

fail1:

    return status;
}




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
    )

/*++

Routine Description:

    NDIS function. It initializes the open block and links it in
    the appropriate lists.

Arguments:

    See NDIS 3.0 spec.

--*/

{
    PNE2000_ADAPTER Adapter = ((PNE2000_ADAPTER)MacAdapterContext);
    PNE2000_OPEN Open;
    NDIS_STATUS Status;

    //
    // Don't use extended error or OpenOptions for Ne2000
    //

    UNREFERENCED_PARAMETER(OpenOptions);

    *OpenErrorStatus=NDIS_STATUS_SUCCESS;

    IF_LOUD( DbgPrint("In Open Adapter\n");)

    //
    // Scan the media list for our media type (802.3)
    //

    *SelectedMediumIndex = (UINT)-1;

    while (MediumArraySize > 0) {

        if (MediumArray[--MediumArraySize] == NdisMedium802_3 ) {

            *SelectedMediumIndex = MediumArraySize;

            break;
        }
        
    }

    if (*SelectedMediumIndex == -1) {

        return NDIS_STATUS_UNSUPPORTED_MEDIA;

    }

    //
    // Allocate memory for the open.
    //

    Status = NdisAllocateMemory((PVOID *)&Open,
                sizeof(NE2000_OPEN),
                0,
                HighestAcceptableMax
                );

    if (Status != NDIS_STATUS_SUCCESS) {

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            0
            );

        return(NDIS_STATUS_RESOURCES);

    }

    //
    // Link this open to the appropriate lists.
    //

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    if ((Adapter->OpenQueue == NULL) && (Adapter->CloseQueue == NULL)) {

        //
        // The first open on this adapter.
        //

        CardStart(Adapter);

    }

    Open->NextOpen = Adapter->OpenQueue;
    Adapter->OpenQueue = Open;

    if (Adapter->ResetInProgress || !EthNoteFilterOpenAdapter(
                          Adapter->FilterDB,
                          Open,
                          NdisBindingContext,
                          &Open->NdisFilterHandle
                          )) {

        Adapter->References--;

        Adapter->OpenQueue = Open->NextOpen;

        NdisReleaseSpinLock(&Adapter->Lock);

        NdisFreeMemory(Open, sizeof(NE2000_OPEN), 0);

        NdisWriteErrorLogEntry(
            Adapter->NdisAdapterHandle,
            NDIS_ERROR_CODE_OUT_OF_RESOURCES,
            0
            );

        return NDIS_STATUS_FAILURE;

    }

    //
    // Set up the open block.
    //

    Open->Adapter = Adapter;
    Open->MacBlock = Adapter->MacBlock;
    Open->NdisBindingContext = NdisBindingContext;
    Open->AddressingInformation = AddressingInformation;
    Open->ProtOptionFlags = 0;

    //
    // set the Request Queue empty
    //

    Open->Closing = FALSE;
    Open->LookAhead = NE2000_MAX_LOOKAHEAD;

    Adapter->MaxLookAhead = NE2000_MAX_LOOKAHEAD;

    Open->ReferenceCount = 1;

    *MacBindingHandle = (NDIS_HANDLE)Open;

    NE2000_DO_DEFERRED(Adapter);

    IF_LOUD( DbgPrint("Out Open Adapter\n");)

    return NDIS_STATUS_SUCCESS;
}

#ifdef NDIS_WIN
    #pragma LCODE
#endif


VOID
Ne2000AdjustMaxLookAhead(
    IN PNE2000_ADAPTER Adapter
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
    PNE2000_OPEN CurrentOpen;

    CurrentOpen = Adapter->OpenQueue;

    while (CurrentOpen != NULL) {

        if (CurrentOpen->LookAhead > CurrentMax) {

            CurrentMax = CurrentOpen->LookAhead;

        }

        CurrentOpen = CurrentOpen->NextOpen;
        
    }

    if (CurrentMax == 0) {

        CurrentMax = NE2000_MAX_LOOKAHEAD;

    }

    Adapter->MaxLookAhead = CurrentMax;

}


NDIS_STATUS
Ne2000CloseAdapter(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    NDIS function. Unlinks the open block and frees it.

Arguments:

    See NDIS 3.0 spec.

--*/

{
    PNE2000_OPEN Open = ((PNE2000_OPEN)MacBindingHandle);
    PNE2000_ADAPTER Adapter = Open->Adapter;
    PNE2000_OPEN TmpOpen;
    NDIS_STATUS StatusToReturn;

    NdisAcquireSpinLock(&Adapter->Lock);

    if (Open->Closing) {

        //
        // The open is already being closed.
        //

        NdisReleaseSpinLock(&Adapter->Lock);

        return NDIS_STATUS_CLOSING;
        
    }

    Adapter->References++;

    Open->ReferenceCount++;

    //
    // Remove this open from the list for this adapter.
    //

    if (Open == Adapter->OpenQueue) {

        Adapter->OpenQueue = Open->NextOpen;

    } else {

        TmpOpen = Adapter->OpenQueue;

        while (TmpOpen->NextOpen != Open) {

            TmpOpen = TmpOpen->NextOpen;

        }

        TmpOpen->NextOpen = Open->NextOpen;
        
    }

    //
    // Remove from Filter package to block all receives.
    //

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

        if (Open->ReferenceCount != 2) {

            //
            // We are not the only reference to the open.  Remove
            // it from the open list and delete the memory.
            //

            Open->Closing = TRUE;

            //
            // Account for this routines reference to the open
            // as well as reference because of the original open.
            //

            Open->ReferenceCount -= 2;

            //
            // Change the status to indicate that we will
            // be closing this later.
            //

            StatusToReturn = NDIS_STATUS_PENDING;

        } else {

            Open->ReferenceCount -= 2;

        }

    } else if (StatusToReturn == NDIS_STATUS_PENDING) {

        Open->Closing = TRUE;

        //
        // Account for this routines reference to the open
        // as well as reference because of the original open.
        //

        Open->ReferenceCount -= 2;

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

        Open->Closing = TRUE;

        //
        // This status is private to the filtering routine.  Just
        // tell the caller the the close is pending.
        //

        StatusToReturn = NDIS_STATUS_PENDING;

        //
        // Account for this routines reference to the open.
        //

        Open->ReferenceCount--;

    } else {

        //
        // Account for this routines reference to the open.
        //

        Open->ReferenceCount--;

    }

    //
    // See if this is the last reference to this open.
    //

    if (Open->ReferenceCount == 0) {

        //
        // Check if the MaxLookAhead needs adjustment.
        //

        if (Open->LookAhead == Adapter->MaxLookAhead) {

            Ne2000AdjustMaxLookAhead(Adapter);

        }

        //
        // Done, free the open.
        //

        NdisFreeMemory(Open, sizeof(NE2000_OPEN), 0);

        if ((Adapter->OpenQueue == NULL ) && (Adapter->CloseQueue == NULL)) {

            //
            // We can disable the card.
            //

            CardStop(Adapter);

        }

    } else {

        //
        // Add it to the close list
        //

        Open->NextOpen = Adapter->CloseQueue;
        Adapter->CloseQueue = Open;

        //
        // Will get removed when count drops to zero.
        //

        StatusToReturn = NDIS_STATUS_PENDING;

    }

    NE2000_DO_DEFERRED(Adapter);

    return(StatusToReturn);

}


NDIS_STATUS
Ne2000Request(
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    This routine allows a protocol to query and set information
    about the MAC.

Arguments:

    MacBindingHandle - The context value returned by the MAC when the
    adapter was opened.  In reality, it is a pointer to PNE2000_OPEN.

    NdisRequest - A structure which contains the request type (Set or
    Query), an array of operations to perform, and an array for holding
    the results of the operations.

Return Value:

    The function value is the status of the operation.

--*/

{
    NDIS_STATUS StatusToReturn = NDIS_STATUS_SUCCESS;

    PNE2000_OPEN Open = (PNE2000_OPEN)(MacBindingHandle);
    PNE2000_ADAPTER Adapter = (Open->Adapter);

    IF_LOUD( DbgPrint("In Request\n");)

    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // Ensure that the open does not close while in this function.
    //

    Open->ReferenceCount++;
    Adapter->References++;

    //
    // Process request
    //

    if (Open->Closing) {

       NdisReleaseSpinLock(&Adapter->Lock);

        StatusToReturn = NDIS_STATUS_CLOSING;

    } else if (NdisRequest->RequestType == NdisRequestQueryInformation) {

        NdisReleaseSpinLock(&Adapter->Lock);
    
        StatusToReturn = Ne2000QueryInformation(Adapter, Open, NdisRequest);

    } else if (NdisRequest->RequestType == NdisRequestSetInformation) {


        //
        // Make sure Adapter is in a valid state.
        //

        //
        // All requests are rejected during a reset.
        //

        if (Adapter->ResetInProgress) {

            NdisReleaseSpinLock(&Adapter->Lock);

            StatusToReturn = NDIS_STATUS_RESET_IN_PROGRESS;

        } else {

            NdisReleaseSpinLock(&Adapter->Lock);

            StatusToReturn = Ne2000SetInformation(Adapter,Open,NdisRequest);

        }

    } else {

        NdisReleaseSpinLock(&Adapter->Lock);

        StatusToReturn = NDIS_STATUS_NOT_RECOGNIZED;

    }


    NdisAcquireSpinLock(&Adapter->Lock);

    Open->ReferenceCount--;

    NE2000_DO_DEFERRED(Adapter);

    IF_LOUD( DbgPrint("Out Request\n");)

    return(StatusToReturn);

}


NDIS_STATUS
Ne2000QueryProtocolInformation(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN NDIS_OID Oid,
    IN BOOLEAN GlobalMode,
    IN PVOID InfoBuffer,
    IN UINT BytesLeft,
    OUT PUINT BytesNeeded,
    OUT PUINT BytesWritten
)

/*++

Routine Description:

    The Ne2000QueryProtocolInformation process a Query request for
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

    PVOID MoveSource = (PVOID)(&GenericULong);
    ULONG MoveBytes = sizeof(ULONG);

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

    IF_LOUD( DbgPrint("In QueryProtocol\n");)

    //
    // Make sure no changes occur while processing.
    //

    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // Switch on request type
    //

    switch (Oid) {

    case OID_GEN_MAC_OPTIONS:

        GenericULong = (ULONG)(NDIS_MAC_OPTION_TRANSFERS_NOT_PEND  |
                               NDIS_MAC_OPTION_RECEIVE_SERIALIZED  |
                               NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA |
                               NDIS_MAC_OPTION_NO_LOOPBACK
                               );

        break;
    
    case OID_GEN_SUPPORTED_LIST:

        if (!GlobalMode) {

            MoveSource = (PVOID)(Ne2000ProtocolSupportedOids);
            MoveBytes = sizeof(Ne2000ProtocolSupportedOids);

        } else {

            MoveSource = (PVOID)(Ne2000GlobalSupportedOids);
            MoveBytes = sizeof(Ne2000GlobalSupportedOids);

        }
        
        break;

    case OID_GEN_HARDWARE_STATUS:

        if (Adapter->ResetInProgress) {

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

        GenericULong = NE2000_MAX_LOOKAHEAD;
        break;

    case OID_GEN_MAXIMUM_FRAME_SIZE:

        GenericULong = (ULONG)(1514 - NE2000_HEADER_SIZE);
        break;

    case OID_GEN_MAXIMUM_TOTAL_SIZE:

        GenericULong = (ULONG)(1514);
        break;

    case OID_GEN_LINK_SPEED:

        GenericULong = (ULONG)(100000);
        break;

    case OID_GEN_TRANSMIT_BUFFER_SPACE:

        GenericULong = (ULONG)(Adapter->NumBuffers * TX_BUF_SIZE);
        break;

    case OID_GEN_RECEIVE_BUFFER_SPACE:

        GenericULong = (ULONG)(0x2000 - (Adapter->NumBuffers * TX_BUF_SIZE));
        break;

    case OID_GEN_TRANSMIT_BLOCK_SIZE:

        GenericULong = (ULONG)(TX_BUF_SIZE);
        break;

    case OID_GEN_RECEIVE_BLOCK_SIZE:

        GenericULong = (ULONG)(256);
        break;

    case OID_GEN_VENDOR_ID:

        NdisMoveMemory(
            (PVOID)&GenericULong,
            Adapter->PermanentAddress,
            3
            );
            
        GenericULong &= 0xFFFFFF00;

        MoveSource = (PVOID)(&GenericULong);
        MoveBytes = sizeof(GenericULong);
        break;
            
    case OID_GEN_VENDOR_DESCRIPTION:

#ifdef NE2000
        MoveSource = (PVOID)"Novell 2000 Adapter.";
#else       
        MoveSource = (PVOID)"Novell 1000 Adapter.";
#endif      
        MoveBytes = 21;

        break;

    case OID_GEN_DRIVER_VERSION:

        GenericUShort = ((USHORT)NE2000_NDIS_MAJOR_VERSION << 8) |
                NE2000_NDIS_MINOR_VERSION;

        MoveSource = (PVOID)(&GenericUShort);
        MoveBytes = sizeof(GenericUShort);
        break;

    case OID_GEN_CURRENT_PACKET_FILTER:

        if (GlobalMode) {

            UINT Filter;

            Filter = ETH_QUERY_FILTER_CLASSES(Adapter->FilterDB);

            GenericULong = (ULONG)(Filter);

        } else {

            UINT Filter;

            Filter = ETH_QUERY_PACKET_FILTER(Adapter->FilterDB,
                         Open->NdisFilterHandle);

            GenericULong = (ULONG)(Filter);

        }

        break;

    case OID_GEN_CURRENT_LOOKAHEAD:

        if ( GlobalMode ) {

            GenericULong = (ULONG)(Adapter->MaxLookAhead);

        } else {

            GenericULong = Open->LookAhead;

        }

        break;

    case OID_802_3_PERMANENT_ADDRESS:
    
        NE2000_MOVE_MEM((PCHAR)GenericArray,
                    Adapter->PermanentAddress,
                    ETH_LENGTH_OF_ADDRESS);

        MoveSource = (PVOID)(GenericArray);
        MoveBytes = sizeof(Adapter->PermanentAddress);
        break;


    case OID_802_3_CURRENT_ADDRESS:

        NE2000_MOVE_MEM((PCHAR)GenericArray,
                    Adapter->StationAddress,
                    ETH_LENGTH_OF_ADDRESS);

        MoveSource = (PVOID)(GenericArray);
        MoveBytes = sizeof(Adapter->StationAddress);
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
        
        break;

    case OID_802_3_MAXIMUM_LIST_SIZE:

        GenericULong = (ULONG) (Adapter->MulticastListMax);

        break;

    default:

        StatusToReturn = NDIS_STATUS_NOT_SUPPORTED;
        break;
    }

    if ((StatusToReturn == NDIS_STATUS_SUCCESS) &&
        (Oid != OID_802_3_MULTICAST_LIST)) {

        if (MoveBytes > BytesLeft) {

            //
            // Not enough room in InformationBuffer. Punt
            //

            *BytesNeeded = MoveBytes;

            StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

        } else {

            //
            // Store result.
            //

            NE2000_MOVE_MEM(InfoBuffer, MoveSource, MoveBytes);

            (*BytesWritten) += MoveBytes;

        }
        
    }

    NdisReleaseSpinLock(&Adapter->Lock);

    IF_LOUD( DbgPrint("Out QueryProtocol\n");)

    return(StatusToReturn);
    
}


NDIS_STATUS
Ne2000QueryInformation(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    )
/*++

Routine Description:

    The Ne2000QueryInformation is used by Ne2000Request to query information
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

    IF_LOUD( DbgPrint("In QueryInfor\n");)

    StatusToReturn = Ne2000QueryProtocolInformation(
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

    IF_LOUD( DbgPrint("Out QueryInfor\n");)

    return(StatusToReturn);
    
}


NDIS_STATUS
Ne2000SetInformation(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest
    )
/*++

Routine Description:

    The Ne2000SetInformation is used by Ne2000Request to set information
    about the MAC.

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

    IF_LOUD( DbgPrint("In SetInfo\n");)

    //
    // Get Oid and Length of request
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
            break;

        }

        StatusToReturn = Ne2000SetMulticastAddresses(
                    Adapter,
                    Open,
                    NdisRequest,
                    (UINT)(OidLength / ETH_LENGTH_OF_ADDRESS),
                    (PVOID)InfoBuffer
                    );

        break;

    case OID_GEN_CURRENT_PACKET_FILTER:

        //
        // Verify length
        //

        if (OidLength != 4 ) {

            StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

            NdisRequest->DATA.SET_INFORMATION.BytesRead = 0;
            NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;
            break;

        }
    
        NE2000_MOVE_MEM(&Filter, InfoBuffer, 4);
        
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

        StatusToReturn = Ne2000SetPacketFilter(Adapter,
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

        NE2000_MOVE_MEM(&LookAhead, InfoBuffer, 4);

        if (LookAhead <= NE2000_MAX_LOOKAHEAD) {

            if (LookAhead > Adapter->MaxLookAhead) {

                Adapter->MaxLookAhead = LookAhead;

                Open->LookAhead = LookAhead;

            } else {

                if ((Open->LookAhead == Adapter->MaxLookAhead) &&
                    (LookAhead < Open->LookAhead)) {

                    Open->LookAhead = LookAhead;

                    Ne2000AdjustMaxLookAhead(Adapter);

                } else {

                Open->LookAhead = LookAhead;

                }

            }

        } else {

            StatusToReturn = NDIS_STATUS_INVALID_LENGTH;

        }

        break;

    case OID_GEN_PROTOCOL_OPTIONS:
    
        if (OidLength != 4) {

            StatusToReturn = NDIS_STATUS_INVALID_LENGTH;
    
            NdisRequest->DATA.SET_INFORMATION.BytesRead = 0;
            NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;
            break;

        }

        NE2000_MOVE_MEM(&Open->ProtOptionFlags, InfoBuffer, 4);
        StatusToReturn = NDIS_STATUS_SUCCESS;
        break;

    default:

        StatusToReturn = NDIS_STATUS_INVALID_OID;

        NdisRequest->DATA.SET_INFORMATION.BytesRead = 0;
        NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;
        break;

    }


    if (StatusToReturn == NDIS_STATUS_SUCCESS) {

        NdisRequest->DATA.SET_INFORMATION.BytesRead = BytesLeft;
        NdisRequest->DATA.SET_INFORMATION.BytesNeeded = 0;

    }


    IF_LOUD( DbgPrint("Out SetInfo\n");)

    return(StatusToReturn);
    
}


STATIC
NDIS_STATUS
Ne2000SetPacketFilter(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN UINT PacketFilter
    )

/*++

Routine Description:

    The Ne2000SetPacketFilter request allows a protocol to control the types
    of packets that it receives from the MAC.

Arguments:

    Adapter - A pointer to the adapter structure.

    Open - A pointer to the open block giving the request.

    NdisRequest - The NDIS_REQUEST with the set packet filter command in it.

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

    NdisAcquireSpinLock(&Adapter->Lock);

    IF_LOUD( DbgPrint("In SetFilter\n");)

    if (!Adapter->ResetInProgress) {

        if (!Open->Closing) {

            //
            // Increment the open while it is going through the filtering
            // routines.
            //

            Open->ReferenceCount++;

            StatusOfFilterChange = EthFilterAdjust(
                       Adapter->FilterDB,
                       Open->NdisFilterHandle,
                       NdisRequest,
                       PacketFilter,
                       TRUE
                       );

            Open->ReferenceCount--;

        } else {

            StatusOfFilterChange = NDIS_STATUS_CLOSING;

        }

    } else {

        StatusOfFilterChange = NDIS_STATUS_RESET_IN_PROGRESS;

    }

    NdisReleaseSpinLock(&Adapter->Lock);

    IF_LOUD( DbgPrint("Out SetFilter\n");)

    return StatusOfFilterChange;
    
}


STATIC
NDIS_STATUS
Ne2000SetMulticastAddresses(
    IN PNE2000_ADAPTER Adapter,
    IN PNE2000_OPEN Open,
    IN PNDIS_REQUEST NdisRequest,
    IN UINT NumAddresses,
    IN CHAR AddressList[][ETH_LENGTH_OF_ADDRESS]
    )

/*++

Routine Description:

    This function calls into the filter package in order to set the
    multicast address list for the card to the specified list.

Arguments:

    Adapter - A pointer to the adapter block.

    Open - A pointer to the open block submitting the request.

    NdisRequest - The NDIS_REQUEST with the set multicast address list command
    in it.

    NumAddresses - A count of the number of addresses in the addressList.

    AddressList - An array of multicast addresses that this open instance
    wishes to accept.


Return Value:

    The function value is the status of the operation.

--*/

{
    //
    // Keeps track of the *MAC's* status.  The status will only be
    // reset if the filter change action routine is called.
    //
    NDIS_STATUS StatusOfFilterChange = NDIS_STATUS_SUCCESS;

    IF_LOUD( DbgPrint("In SetMulticast\n");)

    NdisAcquireSpinLock(&Adapter->Lock);

    if (!Adapter->ResetInProgress) {

        if (!Open->Closing) {

            //
            // Increment the open while it is going through the filtering
            // routines.
            //

            Open->ReferenceCount++;

            StatusOfFilterChange = EthChangeFilterAddresses(
                    Adapter->FilterDB,
                    Open->NdisFilterHandle,
                    NdisRequest,
                    NumAddresses,
                    AddressList,
                    TRUE
                    );

            Open->ReferenceCount--;

        } else {

            StatusOfFilterChange = NDIS_STATUS_CLOSING;

        }

    } else {

        StatusOfFilterChange = NDIS_STATUS_RESET_IN_PROGRESS;

    }

    NdisReleaseSpinLock(&Adapter->Lock);

    IF_LOUD( DbgPrint("Out SetMulticast\n");)

    return StatusOfFilterChange;
    
}


NDIS_STATUS
Ne2000FillInGlobalData(
    IN PNE2000_ADAPTER Adapter,
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
    UINT MoveBytes = sizeof(UINT) * 2 + sizeof(NDIS_OID);

    //
    // Make sure that int is 4 bytes.  Else GenericULong must change
    // to something of size 4.
    //
    ASSERT(sizeof(UINT) == 4);

    StatusToReturn = Ne2000QueryProtocolInformation(
                    Adapter,
                    NULL,
                    NdisRequest->DATA.QUERY_INFORMATION.Oid,
                    TRUE,
                    InfoBuffer,
                    BytesLeft,
                    &BytesNeeded,
                    &BytesWritten
                    );

    if (StatusToReturn == NDIS_STATUS_NOT_SUPPORTED) {

        //
        // Switch on request type
        //

        StatusToReturn = NDIS_STATUS_SUCCESS;

        switch (NdisRequest->DATA.QUERY_INFORMATION.Oid) {

        case OID_GEN_XMIT_OK:

            GenericULong = (UINT)(Adapter->FramesXmitGood);
            break;

        case OID_GEN_RCV_OK:

            GenericULong = (UINT)(Adapter->FramesRcvGood);
            break;

        case OID_GEN_XMIT_ERROR:

            GenericULong = (UINT)(Adapter->FramesXmitBad);
            break;

        case OID_GEN_RCV_ERROR:

            GenericULong = (UINT)(Adapter->CrcErrors);
            break;

        case OID_GEN_RCV_NO_BUFFER:

            GenericULong = (UINT)(Adapter->MissedPackets);
            break;

        case OID_802_3_RCV_ERROR_ALIGNMENT:

            GenericULong = (UINT)(Adapter->FrameAlignmentErrors);
            break;

        case OID_802_3_XMIT_ONE_COLLISION:

            GenericULong = (UINT)(Adapter->FramesXmitOneCollision);
            break;

        case OID_802_3_XMIT_MORE_COLLISIONS:

            GenericULong = (UINT)(Adapter->FramesXmitManyCollisions);
            break;

        default:

            StatusToReturn = NDIS_STATUS_INVALID_OID;
            break;

        }

        //
        // Check to make sure there is enough room in the
        // buffer to store the result.
        //

        if ((BytesLeft >= sizeof(ULONG)) &&
            (StatusToReturn == NDIS_STATUS_SUCCESS)) {

            //
            // Store the result.
            //

            NE2000_MOVE_MEM(
                    (PVOID)InfoBuffer,
                    (PVOID)(&GenericULong),
                    sizeof(ULONG)
                    );

            BytesWritten += sizeof(ULONG);

        }

    }

    NdisRequest->DATA.QUERY_INFORMATION.BytesWritten = BytesWritten;
    NdisRequest->DATA.QUERY_INFORMATION.BytesNeeded = BytesNeeded;

    return(StatusToReturn);
    
}


NDIS_STATUS
Ne2000QueryGlobalStatistics(
    IN NDIS_HANDLE MacAdapterContext,
    IN PNDIS_REQUEST NdisRequest
    )

/*++

Routine Description:

    The Ne2000QueryGlobalStatistics is used by the protocol to query
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

    PNE2000_ADAPTER Adapter = (PNE2000_ADAPTER)(MacAdapterContext);

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

    NdisInterlockedAddUlong(&Adapter->References, 1, &Adapter->Lock);

    if (StatusToReturn == NDIS_STATUS_SUCCESS) {

        StatusToReturn = Ne2000FillInGlobalData(Adapter, NdisRequest);

    }

    NdisAcquireSpinLock(&Adapter->Lock);

    NE2000_DO_DEFERRED(Adapter);

    return(StatusToReturn);
    
}


VOID
Ne2000Unload(
    IN NDIS_HANDLE MacMacContext
    )

/*++

Routine Description:

    Ne2000Unload is called when the MAC is to unload itself.

Arguments:

    None.

Return Value:

    None.

--*/

{
    NDIS_STATUS InitStatus;

    UNREFERENCED_PARAMETER(MacMacContext);

    NdisDeregisterMac(
        &InitStatus,
        Ne2000MacBlock.NdisMacHandle
        );

    NdisFreeSpinLock(&Ne2000MacBlock.SpinLock);

    NdisTerminateWrapper(
        Ne2000MacBlock.NdisWrapperHandle,
        (PVOID) NULL
        );

    return;
    
}


VOID
Ne2000RemoveAdapter(
    IN PVOID MacAdapterContext
    )
/*++

Routine Description:

    Ne2000RemoveAdapter removes an adapter previously registered
    with NdisRegisterAdapter.

Arguments:

    MacAdapterContext - The context value that the MAC passed
    to NdisRegisterAdapter; actually as pointer to an
    NE2000_ADAPTER.

Return Value:

    None.

--*/

{
    PNE2000_ADAPTER Adapter;

    Adapter = PNE2000_ADAPTER_FROM_CONTEXT_HANDLE(MacAdapterContext);

    Adapter->Removed = TRUE;

    ASSERT(Adapter->OpenQueue == (PNE2000_OPEN)NULL);

    //
    // There are no opens left, so remove ourselves.
    //

    //
    // Take us out of the AdapterQueue.
    //

    NdisAcquireSpinLock(&Ne2000MacBlock.SpinLock);

    if (Ne2000MacBlock.AdapterQueue == Adapter) {

        Ne2000MacBlock.AdapterQueue = Adapter->NextAdapter;

    } else {

        PNE2000_ADAPTER TmpAdapter = Ne2000MacBlock.AdapterQueue;

        while (TmpAdapter->NextAdapter != Adapter) {

            TmpAdapter = TmpAdapter->NextAdapter;

        }

        TmpAdapter->NextAdapter = TmpAdapter->NextAdapter->NextAdapter;
        
    }

    NdisReleaseSpinLock(&Ne2000MacBlock.SpinLock);

    {
    
        BOOLEAN Canceled;
        NdisCancelTimer(&Adapter->WakeUpTimer, &Canceled);

        NdisStallExecution(2500000);
        
    }


    EthDeleteFilter(Adapter->FilterDB);

    NdisRemoveInterrupt(&Adapter->NdisInterrupt);

    NdisDeregisterAdapter(Adapter->NdisAdapterHandle);

    NdisFreeSpinLock(&Adapter->Lock);

    NdisFreeMemory(Adapter, sizeof(NE2000_ADAPTER), 0);

}


NDIS_STATUS
Ne2000Reset(
    IN NDIS_HANDLE MacBindingHandle
    )

/*++

Routine Description:

    NDIS function.

Arguments:

    See NDIS 3.0 spec.

--*/

{
    PNE2000_OPEN Open = ((PNE2000_OPEN)MacBindingHandle);
    PNE2000_OPEN TmpOpen;
    PNE2000_ADAPTER Adapter = Open->Adapter;
    NDIS_STATUS Status;

    if (Open->Closing) {

        return(NDIS_STATUS_CLOSING);

    }

    NdisAcquireSpinLock(&Adapter->Lock);

    //
    // Ensure that the open does not close while in this function.
    //

    Open->ReferenceCount++;

    Adapter->References++;

    //
    // Check that nobody is resetting this adapter, block others.
    //

    if (Adapter->ResetInProgress) {

        Open->ReferenceCount--;

        Adapter->References--;

        NdisReleaseSpinLock(&Adapter->Lock);

        return NDIS_STATUS_RESET_IN_PROGRESS;
        
    }

    //
    // Indicate Reset Start
    //

    TmpOpen = Adapter->OpenQueue;

    Status = NDIS_STATUS_SUCCESS;

    while (TmpOpen != (PNE2000_OPEN)NULL) {

        PNE2000_OPEN NextOpen;

        TmpOpen->ReferenceCount++;

        NdisReleaseSpinLock(&Adapter->Lock);

        NdisIndicateStatus(TmpOpen->NdisBindingContext,
               NDIS_STATUS_RESET_START,
               &Status,
               sizeof(Status)
              );

        NdisAcquireSpinLock(&Adapter->Lock);

        NextOpen = TmpOpen->NextOpen;

        TmpOpen->ReferenceCount--;

        TmpOpen = NextOpen;
        
    }

    //
    // Set Reset Flag
    //

    Adapter->ResetInProgress = TRUE;
    Adapter->NextResetStage = NONE;

    //
    // Needed in case the reset pends somewhere along the line.
    //

    Adapter->ResetOpen = Open;
    NdisReleaseSpinLock(&Adapter->Lock);

    //
    // This will take things from here.
    //

    Status = Ne2000Stage2Reset(Adapter);

    NdisAcquireSpinLock(&Adapter->Lock);

    Open->ReferenceCount--;

    NE2000_DO_DEFERRED(Adapter);

    return(Status);

}


NDIS_STATUS
Ne2000Stage2Reset(
    PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    The second stage of a reset.
    It removes all requests on the pend queue.
    Ne2000Stage3Reset will be called when CurBufXmitting goes to -1.

Arguments:

    Adapter - The adapter being reset.

Return Value:

    NDIS_STATUS_PENDING if the card is currently transmitting.
    The result of Ne2000Stage3Reset otherwise.

--*/

{
    NDIS_STATUS Status;
    PNE2000_PEND_DATA Op;
    PNE2000_OPEN TmpOpen;

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    //
    // kill the pend queue.
    //

    while (Adapter->PendQueue != (PNE2000_PEND_DATA)NULL) {

        Op = Adapter->PendQueue;

        Adapter->PendQueue = Op->Next;

        TmpOpen = Op->Open;
    
        NdisReleaseSpinLock(&Adapter->Lock);

        Status = NDIS_STATUS_REQUEST_ABORTED;

        if ((Op->RequestType != NdisRequestClose) &&
            (Op->RequestType != NdisRequestGeneric1)) { // Not a close Request

            NdisCompleteRequest(Op->Open->NdisBindingContext,
            PNDIS_REQUEST_FROM_PNE2000_PEND_DATA(Op),
            NDIS_STATUS_REQUEST_ABORTED);

        }

        //
        // This will call NdisCompleteClose if necessary.
        //

        NdisAcquireSpinLock(&Adapter->Lock);

        TmpOpen->ReferenceCount--;

    }

    if (Adapter->CurBufXmitting != -1) {

        //
        // Ne2000HandleXmitComplete will call Ne2000Stage3Reset.
        //

        Adapter->NextResetStage = XMIT_STOPPED;

        Adapter->References--;

        NdisReleaseSpinLock(&Adapter->Lock);

        return NDIS_STATUS_PENDING;

    }

    Adapter->References--;

    NdisReleaseSpinLock(&Adapter->Lock);

    return Ne2000Stage3Reset(Adapter);

}


NDIS_STATUS
Ne2000Stage3Reset(
    PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    The third stage of a reset. When called, CurBufXmitting has
    gone to -1. Ne2000Stage4Reset is called when call the
    transmit buffers are emptied (i.e. any threads that were
    filling them have finished).

Arguments:

    Adapter - The adapter being reset.

Return Value:

    NDIS_STATUS_PENDING if there are still transmit buffers being filled.
    The result of Ne2000Stage4Reset otherwise.

--*/

{
    UINT i;

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    //
    // Reset these for afterwards.
    //

    Adapter->NextBufToFill = 0;

    Adapter->NextBufToXmit = 0;

    //
    // Make sure all buffer filling operations are done.
    //

    for (i=0; i<Adapter->NumBuffers; i++) {

        if (Adapter->BufferStatus[i] != EMPTY) {

            //
            // Ne2000Send or Ne2000CopyAndSend will call Ne2000Stage4Reset.
            //

            Adapter->NextResetStage = BUFFERS_EMPTY;

            Adapter->References--;

            NdisReleaseSpinLock(&Adapter->Lock);

            return NDIS_STATUS_PENDING;
        }
        
    }

    Adapter->References--;

    NdisReleaseSpinLock(&Adapter->Lock);

    return Ne2000Stage4Reset(Adapter);
    
}


NDIS_STATUS
Ne2000Stage4Reset(
    PNE2000_ADAPTER Adapter
    )

/*++

Routine Description:

    The fourth stage of a reset. When called, the last transmit
    buffer has been marked empty. At this point the reset can
    proceed.

Arguments:

    Adapter - The adapter being reset.

Return Value:

    NDIS_STATUS_SUCCESS if the reset of the card succeeds.
    NDIS_STATUS_FAILURE otherwise.

--*/

{
    UINT i;
    PNDIS_PACKET CurPacket;
    PMAC_RESERVED Reserved;
    PNE2000_OPEN TmpOpen;
    NDIS_STATUS Status;

    //
    // Complete any packets that are waiting in transmit buffers,
    // but are not in the loopback queue.
    //

    NdisAcquireSpinLock(&Adapter->Lock);

    Adapter->References++;

    for (i=0; i<Adapter->NumBuffers; i++) {

        if (Adapter->Packets[i] != (PNDIS_PACKET)NULL) {

            Reserved = RESERVED(Adapter->Packets[i]);

            NdisReleaseSpinLock(&Adapter->Lock);

#if DBG
            Ne2000SendsCompletedForReset++;
#endif

            TmpOpen = Reserved->Open;

            NdisCompleteSend(Reserved->Open->NdisBindingContext,
                Adapter->Packets[i],
                NDIS_STATUS_REQUEST_ABORTED);

            NdisAcquireSpinLock(&Adapter->Lock);

            TmpOpen->ReferenceCount--;

            Adapter->Packets[i] = (PNDIS_PACKET)NULL;
            
        }
        
    }

    //
    // Kill any packets waiting in the transmit queue,
    // but are not in the loopback queue.
    //

    while ((CurPacket = Adapter->XmitQueue) != (PNDIS_PACKET)NULL) {

        Reserved = RESERVED(CurPacket);

        Adapter->XmitQueue = Reserved->NextPacket;

        NdisReleaseSpinLock(&Adapter->Lock);

#if DBG
        Ne2000SendsCompletedForReset++;
#endif

        TmpOpen = Reserved->Open;

        NdisCompleteSend(Reserved->Open->NdisBindingContext,
                CurPacket,
                NDIS_STATUS_REQUEST_ABORTED);

        NdisAcquireSpinLock(&Adapter->Lock);

        TmpOpen->ReferenceCount--;

    }


    //
    // Now kill everything in the loopback queue.
    //

    while ((CurPacket = Adapter->LoopbackQueue) != (PNDIS_PACKET)NULL) {

        Reserved = RESERVED(CurPacket);

        Adapter->LoopbackQueue = Reserved->NextPacket;

        NdisReleaseSpinLock(&Adapter->Lock);

#if DBG
        Ne2000SendsCompletedForReset++;
#endif

        TmpOpen = Reserved->Open;

        NdisCompleteSend(Reserved->Open->NdisBindingContext,
                CurPacket,
                NDIS_STATUS_REQUEST_ABORTED);

        NdisAcquireSpinLock(&Adapter->Lock);

        TmpOpen->ReferenceCount--;

    }

    //
    // Wait for packet reception to stop  -- this might happen if we
    // really blaze through the reset code before the ReceiveDpc gets
    // a chance to run. NOTE: We must be on an MP machine to get here
    // since a DPC is running and we are in this code simultaneously.
    //

    while (Adapter->DpcInProgress) {

        NdisStallExecution(10000);

    }

    //
    // Physically reset the card.
    //

    Adapter->NicInterruptMask =
        IMR_RCV | IMR_XMIT | IMR_XMIT_ERR | IMR_OVERFLOW;

    Status = CardReset(Adapter) ? NDIS_STATUS_SUCCESS : NDIS_STATUS_FAILURE;

    //
    // Set the "resetting" flag back to FALSE.
    //

    Adapter->ResetInProgress = FALSE;

    //
    // Indicate the reset complete to all the protocols.
    //

    TmpOpen = Adapter->OpenQueue;

    while (TmpOpen != (PNE2000_OPEN)NULL) {

        PNE2000_OPEN NextOpen;

        TmpOpen->ReferenceCount++;

        NdisReleaseSpinLock(&Adapter->Lock);

        if (Status != NDIS_STATUS_SUCCESS) {

            NdisIndicateStatus(TmpOpen->NdisBindingContext,
                   NDIS_STATUS_CLOSED,
                   NULL,
                   0
                   );

        }

        NdisIndicateStatus(TmpOpen->NdisBindingContext,
               NDIS_STATUS_RESET_END,
               &Status,
               sizeof(Status)
               );

        NdisIndicateStatusComplete(TmpOpen->NdisBindingContext);

        NdisAcquireSpinLock(&Adapter->Lock);
    
        NextOpen = TmpOpen->NextOpen;

        TmpOpen->ReferenceCount--;

        TmpOpen = NextOpen;
        
    }

    Adapter->References--;

    NdisReleaseSpinLock(&Adapter->Lock);

    return Status;
    
}


VOID
Ne2000ResetStageDone(
    PNE2000_ADAPTER Adapter,
    RESET_STAGE StageDone
    )

/*++

Routine Description:

    Indicates that a stage in the reset is done. Called by
    routines that the reset pended waiting for, to indicate
    that they are done. A central clearing house for determining
    what the next stage is and calling the appropriate routine.
    If a stage completes before it is being pended on, then
    StageDone will not equal Adapter->NextResetStage and no
    action will be taken.

Arguments:

    Adapter - The adapter being reset.
    StageDone - The stage that was just completed.

Return Value:

    None.

--*/

{
    NDIS_STATUS Status;
    UINT i;

    //
    // Make sure this is the stage that was being waited on.
    //

    if (Adapter->NextResetStage != StageDone) {
        return;
    }

    switch (StageDone) {
    case MULTICAST_RESET:
    
        Status = Ne2000Stage2Reset(Adapter);
        break;

    case XMIT_STOPPED:
    
        Status = Ne2000Stage3Reset(Adapter);
        break;

    case BUFFERS_EMPTY:

        //
        // Only continue if this is the last buffer waited for.
        //

        NdisAcquireSpinLock(&Adapter->Lock);

        Adapter->References++;

        for (i=0; i<Adapter->NumBuffers; i++) {

            if (Adapter->BufferStatus[i] != EMPTY) {

                Adapter->References--;

                NdisReleaseSpinLock(&Adapter->Lock);

                return;

            }

        }

        Adapter->References--;

        NdisReleaseSpinLock(&Adapter->Lock);

        Status = Ne2000Stage4Reset(Adapter);
        break;

    }

    if (Status != NDIS_STATUS_PENDING) {

        NdisCompleteReset(
            Adapter->ResetOpen->NdisBindingContext,
            Status);

        NdisAcquireSpinLock(&Adapter->Lock);

        Adapter->ResetOpen->ReferenceCount--;

    } else {

        NdisAcquireSpinLock(&Adapter->Lock);

    }

    NE2000_DO_DEFERRED(Adapter);
    
}


STATIC
NDIS_STATUS
Ne2000ChangeMulticastAddresses(
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


    OldFilterCount - The number of addresses that used to be on the card.

    OldAddresses - A list of all the addresses that used to be on the card.

    NewFilterCount - The number of addresses that should now be on the card.

    NewAddresses - A list of addresses that should be put on the card.

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE2000_OPEN.

    NdisRequest - The request which submitted the filter change.
    Must use when completing this request with the NdisCompleteRequest
    service, if the MAC completes this request asynchronously.

    Set - If true the change resulted from a set, otherwise the
    change resulted from a open closing.

Return Value:

    None.


--*/

{
    PNE2000_ADAPTER Adapter = PNE2000_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);
    PNE2000_PEND_DATA PendOp = PNE2000_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest);

    //
    // The open that made this request.
    //
    PNE2000_OPEN Open = PNE2000_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Holds the status that should be returned to the filtering package.
    //
    NDIS_STATUS StatusOfAdd;

    UNREFERENCED_PARAMETER(OldFilterCount);
    UNREFERENCED_PARAMETER(OldAddresses);
    UNREFERENCED_PARAMETER(NewFilterCount);
    UNREFERENCED_PARAMETER(NewAddresses);

    if (NdisRequest == NULL) {

        NdisRequest = &(Open->CloseAddressRequest);
        PendOp = PNE2000_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest);

    }

    //
    // Check to see if the device is already resetting.  If it is
    // then reject this add.
    //

    if (Adapter->ResetInProgress) {

        StatusOfAdd = NDIS_STATUS_RESET_IN_PROGRESS;

    } else {


        PendOp->Open = Open;

        //
        // We need to add this to the hardware multicast filtering.
        // So pend an operation to do it.
        //

        //
        // Add one to reference count, to be subtracted when the
        // operation get completed.
        //

        PendOp->Open->ReferenceCount++;
        PendOp->RequestType = Set ?
                 NdisRequestGeneric3 : // Means SetMulticastAddresses
                 NdisRequestClose ;    // Means CloseMulticast
        PendOp->Next = NULL;


        if (Adapter->PendQueue == (PNE2000_PEND_DATA)NULL) {

            Adapter->PendQueue = Adapter->PendQTail = PendOp;

        } else {

            Adapter->PendQTail->Next = PendOp;
            Adapter->PendQTail = PendOp;

        }

        StatusOfAdd = NDIS_STATUS_PENDING;

    }

    return StatusOfAdd;

}


STATIC
NDIS_STATUS
Ne2000ChangeFilterClasses(
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

    OldFilterClasses - A bit mask that is currently on the card telling
    which packet types to accept.

    NewFilterClasses - A bit mask that should be put on the card telling
    which packet types to accept.

    MacBindingHandle - The context value returned by the MAC  when the
    adapter was opened.  In reality, it is a pointer to NE2000_OPEN.

    NdisRequest - The NDIS_REQUEST which submitted the filter change command.

    Set - A flag telling if the command is a result of a close or not.

Return Value:

    Status of the change (successful or pending).


--*/

{
    PNE2000_ADAPTER Adapter = PNE2000_ADAPTER_FROM_BINDING_HANDLE(MacBindingHandle);
    PNE2000_PEND_DATA PendOp = PNE2000_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest);

    //
    // The open that made this request.
    //
    PNE2000_OPEN Open = PNE2000_OPEN_FROM_BINDING_HANDLE(MacBindingHandle);

    //
    // Holds the status that should be returned to the filtering package.
    //
    NDIS_STATUS StatusOfAdd;

    UNREFERENCED_PARAMETER(OldFilterClasses);
    UNREFERENCED_PARAMETER(NewFilterClasses);

    if (NdisRequest == NULL) {

        NdisRequest = &(Open->CloseFilterRequest);
        PendOp = PNE2000_PEND_DATA_FROM_PNDIS_REQUEST(NdisRequest);

    }

    //
    // Check to see if the device is already resetting.  If it is
    // then reject this add.
    //

    if (Adapter->ResetInProgress) {

        StatusOfAdd = NDIS_STATUS_RESET_IN_PROGRESS;

    } else {

        PendOp->Open = Open;

        //
        // We need to add this to the hardware multicast filtering.
        // So queue a request.
        //

        PendOp->Open->ReferenceCount++;
        PendOp->RequestType = Set ?
                 NdisRequestGeneric2 : // Means SetPacketFilter
                 NdisRequestGeneric1 ; // Means CloseFilter
        PendOp->Next = NULL;

        if (Adapter->PendQueue == (PNE2000_PEND_DATA)NULL) {

            Adapter->PendQueue = Adapter->PendQTail = PendOp;

        } else {

            Adapter->PendQTail->Next = PendOp;
            Adapter->PendQTail = PendOp;

        }

        StatusOfAdd = NDIS_STATUS_PENDING;

    }

    return StatusOfAdd;

}


STATIC
VOID
Ne2000CloseAction(
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
    adapter was opened.  In reality, it is a pointer to NE2000_OPEN.

Return Value:

    None.


--*/

{
    PNE2000_OPEN_FROM_BINDING_HANDLE(MacBindingHandle)->ReferenceCount--;

}
