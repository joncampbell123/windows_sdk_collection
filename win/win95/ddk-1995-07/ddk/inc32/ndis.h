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

Module name:

  ndis.h

Abstract:

    Main header file for the NDIS 3.X Wrapper, Macs and Protocols

--*/


/* INC */

#ifndef _NDIS_
#define _NDIS_

#define NDIS_WIN     1
#define EXPORT 

/* NOINC */

#ifndef NDIS_STDCALL
#define NDIS_STDCALL    1
#endif

#ifdef NDIS_STDCALL
#define NDIS_API __stdcall
#else
#define NDIS_API
#endif

//
//    Segment definition macros.  These assume the segment groupings used by
//    Chicago/MS-DOS 7.
//

#define NDIS_LCODE code_seg("_LTEXT", "LCODE")
#define NDIS_LDATA data_seg("_LDATA", "LCODE")


#ifdef DEBUG
    #define NDIS_PCODE NDIS_LCODE
    #define NDIS_PDATA NDIS_LDATA
#else
    #define NDIS_PCODE code_seg("_PTEXT", "PCODE")
    #define NDIS_PDATA data_seg("_PDATA", "PCODE")
#endif

#define NDIS_ICODE NDIS_PCODE
#define NDIS_IDATA NDIS_PDATA


#ifndef NDIS_SEG_MACROS
    #define ICODE   NDIS_ICODE
    #define IDATA   NDIS_IDATA
    #define PCODE   NDIS_PCODE
    #define PDATA   NDIS_PDATA
    #define LCODE   NDIS_LCODE
    #define LDATA   NDIS_LDATA
#endif

#define NDIS_INIT_FUNCTION(f)   	alloc_text(_ITEXT,f)
#define NDIS_PAGEABLE_FUNCTION(f)   alloc_text(_PTEXT,f)
#define NDIS_LOCKED_FUNCTION(f)     alloc_text(_LTEXT,f)

/* INC */
#define NDIS_MAJOR_VERSION          0x03
#define NDIS_MINOR_VERSION          0x0A
/* NOINC */

/* INC */
/* ASM
;===========================================================================
;    Segment definition macros.  These assume the segment groupings used by
;    Chicago/MS-DOS 7.
;
;===========================================================================

LCODE_SEG   TEXTEQU <VXD_LOCKED_CODE_SEG>
LCODE_ENDS  TEXTEQU <VXD_LOCKED_CODE_ENDS>
LDATA_SEG   TEXTEQU <VXD_LOCKED_DATA_SEG>
LDATA_ENDS  TEXTEQU <VXD_LOCKED_DATA_ENDS>

IFDEF DEBUG
    PCODE_SEG   TEXTEQU <LCODE_SEG>
    PCODE_ENDS  TEXTEQU <LCODE_ENDS>
    PDATA_SEG   TEXTEQU <LDATA_SEG>
    PDATA_ENDS  TEXTEQU <LDATA_ENDS>
ELSE
	PCODE_SEG   TEXTEQU <VXD_PAGEABLE_CODE_SEG>
	PCODE_ENDS  TEXTEQU <VXD_PAGEABLE_CODE_ENDS>
 	PDATA_SEG   TEXTEQU <VXD_PAGEABLE_DATA_SEG>
	PDATA_ENDS  TEXTEQU <VXD_PAGEABLE_DATA_ENDS>
ENDIF

ICODE_SEG   TEXTEQU <PCODE_SEG>
ICODE_ENDS  TEXTEQU <PCODE_ENDS>
IDATA_SEG   TEXTEQU <PDATA_SEG>
IDATA_ENDS  TEXTEQU <PDATA_ENDS>


*/

#ifndef i386
#define i386
#endif

/* NOINC */

#ifdef DEBUG
    #define DEVL             1
#endif

/* INC */

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

/* NOINC */
#include <basedef.h>

#define ASSERT(a)       if (!(a)) DbgBreakPoint()

/* INC */
#define BUFFER_POOL_SIGN     (UINT)0x4C50424E /* NBPL */
#define BUFFER_SIGN          (UINT)0x4655424E /* NBUF */
#define PACKET_POOL_SIGN     (UINT)0x4C50504E /* NPPL */
#define PACKET_SIGN          (UINT)0x4B41504E /* NPAK */
#define MAC_SIGN             (UINT)0x43414D4E /* NMAC */
#define ADAPTER_SIGN         (UINT)0x5044414E /* NADP */
#define PROTOCOL_SIGN        (UINT)0x5452504E /* NPRT */
#define OPEN_SIGN            (UINT)0x4E504F4E /* NOPN */
/* NOINC */

#ifdef DEBUG
#define DbgBreakPoint() __asm { \
                         __asm int  3 \
                         }
void __cdecl DbgPrint();
#define DBG_PRINTF(A) DbgPrint A
#else
#define DbgBreakPoint()
#define DBG_PRINTF(A)
#endif

//
// Macros required by DOS to compensate for differences with NT.
//

#define IN
#define OUT
#define OPTIONAL
#define INTERNAL
#define UNALIGNED

typedef INT NDIS_SPIN_LOCK, * PNDIS_SPIN_LOCK;

typedef UCHAR BOOLEAN, *PBOOLEAN;

typedef struct _LARGE_INTEGER {
    ULONG LowPart;
    LONG HighPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _ULARGE_INTEGER {
	ULONG LowPart;
	ULONG HighPart;
} ULARGE_INTEGER;

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

typedef ULONG NDIS_PHYSICAL_ADDRESS, *PNDIS_PHYSICAL_ADDRESS;


#define MAX_MCA_CHANNELS    8

//
// Define Mca POS data block 
//

typedef struct _CM_MCA_POS_DATA {
    USHORT AdapterId;
    UCHAR PosData1;
    UCHAR PosData2;
    UCHAR PosData3;
    UCHAR PosData4;
} CM_MCA_POS_DATA, *PCM_MCA_POS_DATA;

//
// Ndis defines for configuration manager data structures
//

typedef CM_MCA_POS_DATA NDIS_MCA_POS_DATA, *PNDIS_MCA_POS_DATA;


// EISA structures
//
// Memory configuration of eisa data block structure
//

#define MAX_EISA_CHANNELS   16

typedef struct _EISA_MEMORY_TYPE {
    UCHAR ReadWrite: 1;
    UCHAR Cached : 1;
    UCHAR Reserved0 :1;
    UCHAR Type:2;
    UCHAR Shared:1;
    UCHAR Reserved1 :1;
    UCHAR MoreEntries : 1;
} EISA_MEMORY_TYPE, *PEISA_MEMORY_TYPE;

typedef struct _EISA_MEMORY_CONFIGURATION {
    EISA_MEMORY_TYPE ConfigurationByte;
    UCHAR DataSize;
    USHORT AddressLowWord;
    UCHAR AddressHighByte;
    USHORT MemorySize;
} EISA_MEMORY_CONFIGURATION, *PEISA_MEMORY_CONFIGURATION;


//
// Interrupt configurationn of eisa data block structure
//

typedef struct _EISA_IRQ_DESCRIPTOR {
    UCHAR Interrupt : 4;
    UCHAR Reserved :1;
    UCHAR LevelTriggered :1;
    UCHAR Shared : 1;
    UCHAR MoreEntries : 1;
} EISA_IRQ_DESCRIPTOR, *PEISA_IRQ_DESCRIPTOR;

typedef struct _EISA_IRQ_CONFIGURATION {
    EISA_IRQ_DESCRIPTOR ConfigurationByte;
    UCHAR Reserved;
} EISA_IRQ_CONFIGURATION, *PEISA_IRQ_CONFIGURATION;


//
// DMA description of eisa data block structure
//

typedef struct _DMA_CONFIGURATION_BYTE0 {
    UCHAR Channel : 3;
    UCHAR Reserved : 3;
    UCHAR Shared :1;
    UCHAR MoreEntries :1;
} DMA_CONFIGURATION_BYTE0;

typedef struct _DMA_CONFIGURATION_BYTE1 {
    UCHAR Reserved0 : 2;
    UCHAR TransferSize : 2;
    UCHAR Timing : 2;
    UCHAR Reserved1 : 2;
} DMA_CONFIGURATION_BYTE1;

typedef struct _EISA_DMA_CONFIGURATION {
    DMA_CONFIGURATION_BYTE0 ConfigurationByte0;
    DMA_CONFIGURATION_BYTE1 ConfigurationByte1;
} EISA_DMA_CONFIGURATION, *PEISA_DMA_CONFIGURATION;


//
// Port description of eisa data block structure
//

typedef struct _EISA_PORT_DESCRIPTOR {
    UCHAR NumberPorts : 5;
    UCHAR Reserved :1;
    UCHAR Shared :1;
    UCHAR MoreEntries : 1;
} EISA_PORT_DESCRIPTOR, *PEISA_PORT_DESCRIPTOR;

typedef struct _EISA_PORT_CONFIGURATION {
    EISA_PORT_DESCRIPTOR Configuration;
    USHORT PortAddress;
} EISA_PORT_CONFIGURATION, *PEISA_PORT_CONFIGURATION;


//
// Eisa slot information definition
// N.B. This structure is different from the one defined
//      in ARC eisa addendum.
//

typedef struct _CM_EISA_SLOT_INFORMATION {
    UCHAR ReturnCode;
    UCHAR ReturnFlags;
    UCHAR MajorRevision;
    UCHAR MinorRevision;
    USHORT Checksum;
    UCHAR NumberFunctions;
    UCHAR FunctionInformation;
    ULONG CompressedId;
} CM_EISA_SLOT_INFORMATION, *PCM_EISA_SLOT_INFORMATION;


//
// Eisa function information definition
//

typedef struct _CM_EISA_FUNCTION_INFORMATION {
    ULONG CompressedId;
    UCHAR IdSlotFlags1;
    UCHAR IdSlotFlags2;
    UCHAR MinorRevision;
    UCHAR MajorRevision;
    UCHAR Selections[26];
    UCHAR FunctionFlags;
    UCHAR TypeString[80];
    EISA_MEMORY_CONFIGURATION EisaMemory[9];
    EISA_IRQ_CONFIGURATION EisaIrq[7];
    EISA_DMA_CONFIGURATION EisaDma[4];
    EISA_PORT_CONFIGURATION EisaPort[20];
    UCHAR InitializationData[60];
} CM_EISA_FUNCTION_INFORMATION, *PCM_EISA_FUNCTION_INFORMATION;


//
// Masks for EISA function information
//

#define EISA_FUNCTION_ENABLED                   0x80
#define EISA_FREE_FORM_DATA                     0x40
#define EISA_HAS_PORT_INIT_ENTRY                0x20
#define EISA_HAS_PORT_RANGE                     0x10
#define EISA_HAS_DMA_ENTRY                      0x08
#define EISA_HAS_IRQ_ENTRY                      0x04
#define EISA_HAS_MEMORY_ENTRY                   0x02
#define EISA_HAS_TYPE_ENTRY                     0x01
#define EISA_HAS_INFORMATION                    EISA_HAS_PORT_RANGE + \
                                                EISA_HAS_DMA_ENTRY + \
                                                EISA_HAS_IRQ_ENTRY + \
                                                EISA_HAS_MEMORY_ENTRY + \
                                                EISA_HAS_TYPE_ENTRY

//
// Masks for EISA memory configuration
//

#define EISA_MORE_ENTRIES                       0x80
#define EISA_SYSTEM_MEMORY                      0x00
#define EISA_MEMORY_TYPE_RAM                    0x01

//
// Returned error code for EISA bios call
//

#define EISA_INVALID_SLOT                       0x80
#define EISA_INVALID_FUNCTION                   0x81
#define EISA_INVALID_CONFIGURATION              0x82
#define EISA_EMPTY_SLOT                         0x83
#define EISA_INVALID_BIOS_CALL                  0x86


typedef CM_EISA_SLOT_INFORMATION NDIS_EISA_SLOT_INFORMATION;
typedef CM_EISA_SLOT_INFORMATION *PNDIS_EISA_SLOT_INFORMATION;
typedef CM_EISA_FUNCTION_INFORMATION  NDIS_EISA_FUNCTION_INFORMATION;
typedef CM_EISA_FUNCTION_INFORMATION *PNDIS_EISA_FUNCTION_INFORMATION;  

//
// Defines the Type in the RESOURCE_DESCRIPTOR
//

typedef enum _CM_RESOURCE_TYPE {
    CmResourceTypeNull = 0,    // Reserved
    CmResourceTypePort,
    CmResourceTypeInterrupt,
    CmResourceTypeMemory,
    CmResourceTypeDma,
    CmResourceTypeDeviceSpecific,
    CmResourceTypeMaximum
} CM_RESOURCE_TYPE;

//
// Defines the ShareDisposition in the RESOURCE_DESCRIPTOR
//

typedef enum _CM_SHARE_DISPOSITION {
    CmResourceShareUndetermined = 0,    // Reserved
    CmResourceShareDeviceExclusive,
    CmResourceShareDriverExclusive,
    CmResourceShareShared
} CM_SHARE_DISPOSITION;

//
// Define the bit masks for Flags when type is CmResourceTypeInterrupt
//

#define CM_RESOURCE_INTERRUPT_LEVEL_SENSITIVE 0
#define CM_RESOURCE_INTERRUPT_LATCHED         1

//
// Define the bit masks for Flags when type is CmResourceTypeMemory
//

#define CM_RESOURCE_MEMORY_READ_WRITE       0x0000
#define CM_RESOURCE_MEMORY_READ_ONLY        0x0001
#define CM_RESOURCE_MEMORY_WRITE_ONLY       0x0002
#define CM_RESOURCE_MEMORY_PREFETCHABLE     0x0004

//
// Define the bit masks for Flags when type is CmResourceTypePort
//

#define CM_RESOURCE_PORT_MEMORY 0
#define CM_RESOURCE_PORT_IO 1



typedef struct _CM_PARTIAL_RESOURCE_DESCRIPTOR {
    UCHAR Type;
    UCHAR ShareDisposition;
    USHORT Flags;
    union {

        //
        // Range of port numbers, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;
            ULONG Length;
        } Port;

        //
        // IRQL and vector. Should be same values as were passed to
        // HalGetInterruptVector().
        //

        struct {
            ULONG Level;
            ULONG Vector;
            ULONG Affinity;
        } Interrupt;

        //
        // Range of memory addresses, inclusive. These are physical, bus
        // relative. The value should be the same as the one passed to
        // HalTranslateBusAddress().
        //

        struct {
            PHYSICAL_ADDRESS Start;    // 64 bit physical addresses.
            ULONG Length;
        } Memory;

        //
        // Physical DMA channel.
        //

        struct {
            ULONG Channel;
            ULONG Port;
            ULONG Reserved1;
        } Dma;

        //
        // Device Specific information defined by the driver.
        // The DataSize field indicates the size of the data in bytes. The
        // data is located immediately after the DeviceSpecificData field in
        // the structure.
        //

        struct {
            ULONG DataSize;
            ULONG Reserved1;
            ULONG Reserved2;
        } DeviceSpecificData;
    } u;
} CM_PARTIAL_RESOURCE_DESCRIPTOR, *PCM_PARTIAL_RESOURCE_DESCRIPTOR;

typedef struct _CM_PARTIAL_RESOURCE_LIST {
    USHORT Version;
    USHORT Revision;
    ULONG Count;
    CM_PARTIAL_RESOURCE_DESCRIPTOR PartialDescriptors[1];
} CM_PARTIAL_RESOURCE_LIST, *PCM_PARTIAL_RESOURCE_LIST;

typedef CM_PARTIAL_RESOURCE_LIST NDIS_RESOURCE_LIST, *PNDIS_RESOURCE_LIST ;



VOID NDIS_API
NdisMoveMemory(
    OUT PVOID destaddr,
    IN PVOID sourceaddr,
    IN ULONG len
    );

VOID NDIS_API
MoveOverlappedMemory(
    OUT PVOID destaddr,
    IN PVOID sourceaddr,
    IN ULONG len
    );

#define NdisMoveMappedMemory(Destination,Source,Length) NdisMoveMemory(Destination,Source,Length)
#define NdisZeroMemory(addr,len) memset(addr,0,len)
#define NdisZeroMappedMemory(Destination,Length) memset(Destination,'\0',Length)
#define NDIS_PORT_TO_PORT(Handle,Port) (Port)

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

//
// characteristics of an x86 page
//

#define PAGE_SIZE           0x1000L
#define PAGE_OFFSET_MASK    0x00000FFFL
#define PAGE_MASK           0xFFFFF000L
#define PAGE_SHIFT          12

//
// On current systems, these are the same.
//

#define NdisMoveToMappedMemory(Destination,Source,Length) NdisMoveMappedMemory(Destination,Source,Length)
#define NdisMoveFromMappedMemory(Destination,Source,Length) NdisMoveMappedMemory(Destination,Source,Length)


//
// Routine Description:
//
//     The ADDRESS_AND_SIZE_TO_SPAN_PAGES macro takes a virtual address and
//     size and returns the number of pages spanned by the size.
//

#define ADDRESS_AND_SIZE_TO_SPAN_PAGES(Va,Size) \
   ((Size) ? \
   ((((ULONG)((ULONG)Va + (ULONG)Size - 1UL) >> (ULONG)PAGE_SHIFT) - \
   ((ULONG)Va >> (ULONG) PAGE_SHIFT)) + 1UL) : \
   0)



typedef struct _MEMORY_DESCRIPTOR {
    PCHAR Address;
    UINT  Length;
    } MEMORY_DESCRIPTOR, *PMEMORY_DESCRIPTOR;

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PUCHAR Buffer;
} STRING, *PSTRING;

typedef STRING NDIS_STRING, *PNDIS_STRING;

//
// Not part of Ndis Spec.
//
BOOLEAN NDIS_API
NdisEqualString(
    PNDIS_STRING String1,
    PNDIS_STRING String2,
    BOOLEAN    CaseInsensitive
    );


typedef signed short WCH, *PWCH;

typedef unsigned char CCHAR, *PCCHAR;

typedef PVOID NDIS_HANDLE, *PNDIS_HANDLE;

typedef	DWORD			DEVNODE;

typedef struct _EISA_MCA_ADAPATER_IDS{
    USHORT  nEisaAdapters;
    USHORT  nMcaAdapters;
    UCHAR   IdArray[1];
    } EISA_MCA_ADAPTER_IDS, *PEISA_MCA_ADAPTER_IDS;

enum WRAPPER_CONFIGURATION_CONTEXT_DRIVER_TYPE{
    wccDriverTypeMac,
    wccDriverTypeProtocol,
    wccDriverTypeUnknown
};

typedef struct _PROTOCOL_INTERFACES{
    UINT    cInterfaces;
    NDIS_STRING ansInterfaces[1];
} PROTOCOL_INTERFACES, *PPROTOCOL_INTERFACES;


typedef struct _WRAPPER_CONFIGURATION_CONTEXT{
    PNDIS_STRING pModuleName;
    PEISA_MCA_ADAPTER_IDS pIds;
    enum WRAPPER_CONFIGURATION_CONTEXT_DRIVER_TYPE DriverType;
    PNDIS_STRING pProtocolLogicalName;
} WRAPPER_CONFIGURATION_CONTEXT, *PWRAPPER_CONFIGURATION_CONTEXT;


/* INC */
//#ifdef    NDIS_ASM
#ifdef  NDIS_ASM
struct  NDIS_STRING {
    USHORT  S_Length;
    USHORT  S_MaxLength;
    PUCHAR  S_Buffer;
};

struct  NDIS_STATUS {
    int ns_value;
};

struct  NDIS_HANDLE {
    PVOID   nh_value;
};

struct _WRAPPER_CONFIGURATION_CONTEXT{
        PVOID pModuleName;
        PVOID pIds;
        };
#endif



typedef ULONG NDIS_STATUS;
typedef NDIS_STATUS *PNDIS_STATUS;

// BUGBUG for compatibility with NT, ask them to remove it from
// Their drivers
typedef NDIS_STATUS NTSTATUS;
typedef CCHAR KIRQL;
typedef KIRQL *PKIRQL;
#define HIGH_LEVEL 31
#define PDRIVER_OBJECT PVOID
#define PUNICODE_STRING PVOID
#define PDEVICE_OBJECT PVOID
#define PKDPC PVOID


/* NOINC */

//
// Define the DMA transfer widths.
//
typedef enum _DMA_WIDTH {
    Width8Bits,
    Width16Bits,
    Width32Bits,
    MaximumDmaWidth
}DMA_WIDTH, *PDMA_WIDTH;

//
// Define DMA transfer speeds.
//

typedef enum _DMA_SPEED {
    Compatible,
    TypeA,
    TypeB,
    TypeC,
    MaximumDmaSpeed
}DMA_SPEED, *PDMA_SPEED;


typedef struct _NDIS_DMA_DESCRIPTION {
    BOOLEAN DemandMode;
    BOOLEAN AutoInitialize;
    BOOLEAN DmaChannelSpecified;
    DMA_WIDTH DmaWidth;
    DMA_SPEED DmaSpeed;
    ULONG DmaPort;
    ULONG DmaChannel;
} NDIS_DMA_DESCRIPTION, *PNDIS_DMA_DESCRIPTION;

typedef struct _NDIS_TIMER {
    PVOID TimerHandle;
    PVOID CallbackFn;
    PVOID CallbackContext;
#ifdef M7    
    ULONG ulFlags;
#endif    
    } NDIS_TIMER, *PNDIS_TIMER;

typedef enum {
    NdisInterruptLatched=1,
    NdisInterruptLevelSensitive=2
    } NDIS_INTERRUPT_MODE, *PNDIS_INTERRUPT_MODE;

typedef
BOOLEAN
(NDIS_API *PNDIS_INTERRUPT_SERVICE) (
    IN PVOID InterruptContext
    );

typedef
VOID
(NDIS_API *PNDIS_DEFERRED_PROCESSING) (
    IN PVOID SystemSpecific1,
    IN PVOID InterruptContext,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );

typedef struct _NDIS_INTERRUPT {
    UINT    InterruptFlags;         //Shared, DPC pending?
    PVOID   InterruptContext;       //context value for ISR and Dpc
    PVOID   IsrEntry;               //pointer to ISR for this int
    PVOID   DpcEntry;               //pointer to Dpc routine for this int
    PVOID   VPICD_Context;          //context for removing interrupt
    struct _NDIS_INTERRUPT *Next;   // link for shared interrupts
    } NDIS_INTERRUPT, *PNDIS_INTERRUPT;

struct _NDIS_PACKET;

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
// Define the I/O bus interface types. from NT (ntddk.h)
//
typedef enum _INTERFACE_TYPE {
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    MaximumInterfaceType
}INTERFACE_TYPE, *PINTERFACE_TYPE;

//
// Possible Hardware Architecture. Define these to
// match the HAL INTERFACE_TYPE enum (for NT compatibility).
//
typedef enum _NDIS_INTERFACE_TYPE {
    NdisInterfaceInternal = Internal,
    NdisInterfaceIsa = Isa,
    NdisInterfaceEisa = Eisa,
    NdisInterfaceMca = MicroChannel,
    NdisInterfaceTurboChannel = TurboChannel,
    NdisInterfacePci = PCIBus,
    NdisInterfacePcMcia = PCMCIABus
} NDIS_INTERFACE_TYPE, *PNDIS_INTERFACE_TYPE;


//
// Ndis Adapter Information
//
typedef
NDIS_STATUS
(NDIS_API *PNDIS_ACTIVATE_CALLBACK) (
    IN NDIS_HANDLE NdisAdatperHandle,
    IN NDIS_HANDLE MacAdapterContext,
    IN ULONG DmaChannel
    );
    
//
// Definition for shutdown handler
//

typedef
VOID
(NDIS_API *ADAPTER_SHUTDOWN_HANDLER) (
    IN PVOID ShutdownContext
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
// Ndis Buffer
//

struct _NDIS_BUFFER;

typedef struct _NDIS_BUFFER_POOL {
    UINT Signature;                     //character signature for debug "NBPL"
    NDIS_SPIN_LOCK SpinLock;            //to serialize access to the buffer pool
    struct _NDIS_BUFFER *FreeList;      //linked list of free slots in pool
    UINT BufferLength;                  //amount needed for each buffer descriptor
    UCHAR Buffer[1];                    //actual pool memory
    } NDIS_BUFFER_POOL, * PNDIS_BUFFER_POOL;


#ifdef NDIS_STDCALL
typedef struct _NDIS_BUFFER {
    struct _NDIS_BUFFER *Next;          //pointer to next buffer descriptor in chain
    PVOID VirtualAddress;               //linear address of this buffer
    PNDIS_BUFFER_POOL Pool;             //pointer to pool so we can free to correct pool
    UINT Length;                        //length of this buffer
    UINT Signature;                     //character signature for debug "NBUF"
} NDIS_BUFFER, * PNDIS_BUFFER;

#else

typedef struct _NDIS_BUFFER {
    UINT Signature;                     //character signature for debug "NBUF"
    struct _NDIS_BUFFER *Next;          //pointer to next buffer descriptor in chain
    PVOID VirtualAddress;               //linear address of this buffer
    PNDIS_BUFFER_POOL Pool;             //pointer to pool so we can free to correct pool
    UINT Length;                        //length of this buffer
} NDIS_BUFFER, * PNDIS_BUFFER;
#endif

/* INC */
#ifdef  NDIS_ASM

#ifdef NDIS_STDCALL
typedef struct _NDIS_BUFFER {
    PVOID NB_Next;                  //pointer to next buffer descriptor in chain
    PVOID NB_VirtualAddress;        //linear address of this buffer
    PVOID NB_Pool;                  //pointer to pool so we can free
    UINT NB_Length;                 //length of this buffer
    UINT NB_Signature;              //character signature for debug "NBUF"
} NDIS_BUFFER, * PNDIS_BUFFER;
#else
typedef struct _NDIS_BUFFER {
    UINT NB_Signature;              //character signature for debug "NBUF"
    PVOID NB_Next;                  //pointer to next buffer descriptor in chain
    PVOID NB_VirtualAddress;        //linear address of this buffer
    PVOID NB_Pool;                  //pointer to pool so we can free
    UINT NB_Length;                 //length of this buffer
} NDIS_BUFFER, * PNDIS_BUFFER;
#endif

#endif
/* NOINC */

//
// packet pool definition
//

typedef struct _NDIS_PACKET_POOL {
    UINT Signature;                     //character signature for debug "NPPL"
    NDIS_SPIN_LOCK SpinLock;
    struct _NDIS_PACKET *FreeList;  // linked list of free slots in pool
    UINT PacketLength;                  // amount needed in each packet
    UCHAR Buffer[1];                    // actual pool memory
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
    UCHAR Reserved[8];      // for future expansion
} NDIS_PACKET_PRIVATE, * PNDIS_PACKET_PRIVATE;


//
// packet definition
//

#ifdef NDIS_STDCALL

typedef struct _NDIS_PACKET {
    NDIS_PACKET_PRIVATE Private;
    union {

        struct {
            UCHAR WidgetReserved[8];
            UCHAR WrapperReserved[8];
        };

        struct {
            UCHAR MacReserved[16];
        };

    };
    UINT Signature;             //character signature for debug "NPAK"
    UCHAR ProtocolReserved[1];
} NDIS_PACKET, * PNDIS_PACKET;

#else

typedef struct _NDIS_PACKET {
    UINT Signature;             //character signature for debug "NPAK"
    NDIS_PACKET_PRIVATE Private;
    union {

        struct {
            UCHAR WidgetReserved[8];
            UCHAR WrapperReserved[8];
        };

        struct {
            UCHAR MacReserved[16];
        };

    };
    UCHAR ProtocolReserved[1];
} NDIS_PACKET, * PNDIS_PACKET;

#endif

/* INC */
#ifdef  NDIS_ASM
typedef struct _NDIS_PACKET_PRIVATE {
    UINT NPP_PhysicalCount;     // number of physical pages in packet.
    UINT NPP_TotalLength;       // Total amount of data in the packet.
    PVOID NPP_Head;             // first buffer in the chain
    PVOID NPP_Tail;             // last buffer in the chain

    // if Head is NULL the chain is empty; Tail doesn't have to be NULL also

    PVOID NPP_Pool;             // so we know where to free it back to
    UINT NPP_Count;
    ULONG NPP_Flags;
    UCHAR NPP_Reserved[8];      // for future expansion
} NDIS_PACKET_PRIVATE, * PNDIS_PACKET_PRIVATE;


//
// packet definition
//

#ifdef NDIS_STDCALL
typedef struct _NDIS_PACKET {
    struct _NDIS_PACKET_PRIVATE NP_Private;
    UCHAR NP_MacReserved[16];
    UINT NP_Signature;          //character signature for debug "NPAK"
    UCHAR NP_ProtocolReserved[1];
} NDIS_PACKET, * PNDIS_PACKET;
#else
typedef struct _NDIS_PACKET {
    UINT NP_Signature;          //character signature for debug "NPAK"
    struct _NDIS_PACKET_PRIVATE NP_Private;
    UCHAR NP_MacReserved[16];
    UCHAR NP_ProtocolReserved[1];
} NDIS_PACKET, * PNDIS_PACKET;
#endif

#endif
/* NOINC */

//
// Include an incomplete type for NDIS_PACKET structure so that
// function types can refer to a type to be defined later.
//
struct _NDIS_PACKET;

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
// Object ID
//
typedef ULONG NDIS_OID, *PNDIS_OID;


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
// Medium Ndis Driver is running on
//

typedef enum _NDIS_MEDIUM {
    NdisMedium802_3,
    NdisMedium802_5,
    NdisMediumFddi,
    NdisMediumWan,
    NdisMediumLocalTalk,
    NdisMediumDix,              // defined for convenience, not a real medium
    NdisMediumArcnetRaw,
    NdisMediumArcnet878_2
} NDIS_MEDIUM, *PNDIS_MEDIUM;                    

//
// Hardware status codes (OID_GEN_HARDWARE_STATUS).
//

typedef enum _NDIS_HARDWARE_STATUS {
    NdisHardwareStatusReady,
    NdisHardwareStatusInitializing,
    NdisHardwareStatusReset,
    NdisHardwareStatusClosing,
    NdisHardwareStatusNotReady
} NDIS_HARDWARE_STATUS, *PNDIS_HARDWARE_STATUS;


//
// Defines the attachment types for FDDI (OID_FDDI_ATTACHMENT_TYPE).
//

typedef enum _NDIS_FDDI_ATTACHMENT_TYPE {
    NdisFddiTypeIsolated = 1,
    NdisFddiTypeLocalA,
    NdisFddiTypeLocalB,
    NdisFddiTypeLocalAB,
    NdisFddiTypeLocalS,
    NdisFddiTypeWrapA,
    NdisFddiTypeWrapB,
    NdisFddiTypeWrapAB,
    NdisFddiTypeWrapS,
    NdisFddiTypeCWrapA,
    NdisFddiTypeCWrapB,
    NdisFddiTypeCWrapS,
    NdisFddiTypeThrough
} NDIS_FDDI_ATTACHMENT_TYPE, *PNDIS_FDDI_ATTACHMENT_TYPE;


//
// Defines the ring management states for FDDI (OID_FDDI_RING_MGT_STATE).
//

typedef enum _NDIS_FDDI_RING_MGT_STATE {
    NdisFddiRingIsolated = 1,
    NdisFddiRingNonOperational,
    NdisFddiRingOperational,
    NdisFddiRingDetect,
    NdisFddiRingNonOperationalDup,
    NdisFddiRingOperationalDup,
    NdisFddiRingDirected,
    NdisFddiRingTrace
} NDIS_FDDI_RING_MGT_STATE, *PNDIS_FDDI_RING_MGT_STATE;


//
// Defines the Lconnection state for FDDI (OID_FDDI_LCONNECTION_STATE).
//

typedef enum _NDIS_FDDI_LCONNECTION_STATE {
    NdisFddiStateOff = 1,
    NdisFddiStateBreak,
    NdisFddiStateTrace,
    NdisFddiStateConnect,
    NdisFddiStateNext,
    NdisFddiStateSignal,
    NdisFddiStateJoin,
    NdisFddiStateVerify,
    NdisFddiStateActive,
    NdisFddiStateMaintenance
} NDIS_FDDI_LCONNECTION_STATE, *PNDIS_FDDI_LCONNECTION_STATE;


//
// Defines the medium subtypes for WAN medium
//

typedef enum _NDIS_WAN_MEDIUM_SUBTYPE {
    NdisWanMediumHub,
    NdisWanMediumX_25,
    NdisWanMediumIsdn,
    NdisWanMediumSerial,
    NdisWanMediumFrameRelay
} NDIS_WAN_MEDIUM_SUBTYPE, *PNDIS_WAN_MEDIUM_SUBTYPE;

//
// Defines the header format for WAN medium
//

typedef enum _NDIS_WAN_HEADER_FORMAT {
    NdisWanHeaderNative,       // src/dest based on subtype, followed by NLPID
    NdisWanHeaderEthernet      // emulation of ethernet header
} NDIS_WAN_HEADER_FORMAT, *PNDIS_WAN_HEADER_FORMAT;

//
// Defines the line quality on an WAN line
//

typedef enum _NDIS_WAN_QUALITY {
    NdisWanRaw,
    NdisWanErrorControl,
    NdisWanReliable
} NDIS_WAN_QUALITY, *PNDIS_WAN_QUALITY;

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
// Defines the state of a token-ring adapter
//

typedef enum _NDIS_802_5_RING_STATE {
    NdisRingStateOpened = 1,
    NdisRingStateClosed,
    NdisRingStateOpening,
    NdisRingStateClosing,
    NdisRingStateOpenFailure,
    NdisRingStateRingFailure
} NDIS_802_5_RING_STATE, *PNDIS_802_5_RING_STATE;



//
// The ordering of entries in the physical address structure
// is important, DO NOT CHANGE (AO 20Aug91)
// dependant routine: NdisGetPhysicalAddress
//

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

#define NdisGetPhysicalAddressHigh(_PhysicalAddress) (0L)

/*++

VOID
NdisSetPhysicalAddressHigh(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG Value
    );

--*/

#define NdisSetPhysicalAddressHigh(_PhysicalAddress, _Value)


/*++

ULONG
NdisGetPhysicalAddressLow(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

--*/

#define NdisGetPhysicalAddressLow(_PhysicalAddress) (_PhysicalAddress)


/*++

VOID
NdisSetPhysicalAddressLow(
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress,
    IN ULONG Value
    );

--*/

#define NdisSetPhysicalAddressLow(_PhysicalAddress, _Value) \
    (_PhysicalAddress) = (_Value)


//
// Macro to initialize an NDIS_PHYSICAL_ADDRESS constant
//

#define NDIS_PHYSICAL_ADDRESS_CONST(_Low, _High) \
     (ULONG)(_Low)

//
// Function types for NDIS_PROTOCOL_CHARACTERISTICS
//
//

typedef
VOID
(NDIS_API *OPEN_ADAPTER_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status,
    IN NDIS_STATUS OpenErrorStatus
    );

typedef
VOID
(NDIS_API *CLOSE_ADAPTER_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );

typedef
VOID
(NDIS_API *SEND_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status
    );

typedef
VOID
(NDIS_API *TRANSFER_DATA_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status,
    IN UINT BytesTransferred
    );

typedef
VOID
(NDIS_API *RESET_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );

typedef
VOID
(NDIS_API *REQUEST_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    );

typedef
NDIS_STATUS
(NDIS_API *RECEIVE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_HANDLE MacReceiveContext,
    IN PVOID HeaderBuffer,
    IN UINT HeaderBufferSize,
    IN PVOID LookAheadBuffer,
    IN UINT LookaheadBufferSize,
    IN UINT PacketSize
    );

typedef
VOID
(NDIS_API *RECEIVE_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext
    );

typedef
VOID
(NDIS_API *STATUS_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS GeneralStatus,
    IN PVOID StatusBuffer,
    IN UINT StatusBufferSize
    );

typedef
VOID
(NDIS_API *STATUS_COMPLETE_HANDLER) (
    IN NDIS_HANDLE NdisBindingContext
    );


//
// BIND_ADAPTER_HANDLER, UNBIND_ADAPTER_HANDLER, UNLOAD_PROTOCOL
// are valid for NDIS 3.1 protocols
//

typedef
VOID
(NDIS_API *BIND_ADAPTER_HANDLER) (
    OUT PNDIS_STATUS    Status,
    IN  NDIS_HANDLE     BindAdapterContext,
    IN  PNDIS_STRING    AdapterName,
    IN  PVOID           SystemSpecific1,
    IN  PVOID           SystemSpecific2
    );


typedef
VOID
(NDIS_API *UNBIND_ADAPTER_HANDLER) (
    OUT PNDIS_STATUS    Status,
    IN  NDIS_HANDLE     ProtocolBindingContext,
    IN  NDIS_HANDLE     UnbindAdapterContext
    );

typedef
VOID
(NDIS_API *UNLOAD_PROTOCOL_HANDLER) ();

typedef struct _OLD_NDIS_PROTOCOL_CHARACTERISTICS {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;
    ULONG Reserved;
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
} OLD_NDIS_PROTOCOL_CHARACTERISTICS, *POLD_NDIS_PROTOCOL_CHARACTERISTICS;

typedef struct _NDIS_PROTOCOL_CHARACTERISTICS {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;
    ULONG Reserved;
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
    BIND_ADAPTER_HANDLER BindAdapterHandler;
    UNBIND_ADAPTER_HANDLER UnbindAdapterHandler;
    UNLOAD_PROTOCOL_HANDLER UnloadProtocolHandler;
    NDIS_STRING Name;
} NDIS_PROTOCOL_CHARACTERISTICS, *PNDIS_PROTOCOL_CHARACTERISTICS;


typedef
VOID
(NDIS_API *PNDIS_SYNCHRONIZE_ROUTINE) (
    IN PVOID SynchronizeContext
    );

//
// Function types for NDIS_MAC_CHARACTERISTICS
//


typedef
NDIS_STATUS
(NDIS_API *OPEN_ADAPTER_HANDLER) (
    OUT PNDIS_STATUS OpenErrorStatus,
    IN NDIS_HANDLE *MacBindingHandle,
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
(NDIS_API *CLOSE_ADAPTER_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle
    );

typedef
NDIS_STATUS
(NDIS_API *SEND_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_PACKET Packet
    );

typedef
NDIS_STATUS
(NDIS_API *TRANSFER_DATA_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT BytesOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );

typedef
NDIS_STATUS
(NDIS_API *RESET_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle
    );

typedef
NDIS_STATUS
(NDIS_API *REQUEST_HANDLER) (
    IN NDIS_HANDLE MacBindingHandle,
    IN PNDIS_REQUEST NdisRequest
    );

typedef
VOID
(NDIS_API *UNLOAD_MAC_HANDLER) (
    IN NDIS_HANDLE MacMacContext

    );

typedef
NDIS_STATUS
(NDIS_API *ADD_ADAPTER_HANDLER) (
    IN NDIS_HANDLE MacMacContext,
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN PNDIS_STRING AdapterName
    );

typedef
VOID
(NDIS_API *REMOVE_ADAPTER_HANDLER) (
    IN NDIS_HANDLE MacAdapterContext
    );

typedef struct _NDIS_MAC_CHARACTERISTICS {
    UCHAR MajorNdisVersion;
    UCHAR MinorNdisVersion;
    ULONG Reserved;
    OPEN_ADAPTER_HANDLER OpenAdapterHandler;
    CLOSE_ADAPTER_HANDLER CloseAdapterHandler;
    SEND_HANDLER SendHandler;
    TRANSFER_DATA_HANDLER TransferDataHandler;
    RESET_HANDLER ResetHandler;
    REQUEST_HANDLER RequestHandler;
    REQUEST_HANDLER QueryGlobalStatisticsHandler;
    UNLOAD_MAC_HANDLER UnloadMacHandler;
    ADD_ADAPTER_HANDLER AddAdapterHandler;
    REMOVE_ADAPTER_HANDLER RemoveAdapterHandler;
    NDIS_STRING Name;
} NDIS_MAC_CHARACTERISTICS, *PNDIS_MAC_CHARACTERISTICS;



//
// block used for references...
//

typedef struct _REFERENCE {
    NDIS_SPIN_LOCK SpinLock;    //  4 Bytes
    USHORT ReferenceCount;      //  2 Bytes
    BOOLEAN Closing;            //  1 Byte
    UCHAR   Reserved;           //  1 Byte Padding
} REFERENCE, * PREFERENCE;

//
// declare these first since they point to each other
//

typedef struct _WRAPPER_MAC_BLOCK      WRAPPER_MAC_BLOCK, * PWRAPPER_MAC_BLOCK;
typedef struct _WRAPPER_ADAPTER_BLOCK  WRAPPER_ADAPTER_BLOCK, * PWRAPPER_ADAPTER_BLOCK;
typedef struct _WRAPPER_PROTOCOL_BLOCK WRAPPER_PROTOCOL_BLOCK, * PWRAPPER_PROTOCOL_BLOCK;
typedef struct _WRAPPER_OPEN_BLOCK     WRAPPER_OPEN_BLOCK, * PWRAPPER_OPEN_BLOCK;
typedef struct _MODULE_LIST            MODULE_LIST, *PMODULE_LIST;

typedef enum _W_LOADER_TYPE{
    wVxdLoader,
    wPeLoader
    } W_LOADER_TYPE;    

struct _MODULE_LIST {
    W_LOADER_TYPE LoaderType;
    union{
        NDIS_STRING nsModuleName;
        DWORD pModuleHandle;
    } u;       
    PMODULE_LIST pNext;
};

//
// one of these per MAC
//

struct _WRAPPER_MAC_BLOCK {
    UINT Signature;                     // debugging signature NMAC  
    PWRAPPER_MAC_BLOCK pNext;           // Used by System for queing all macs
    PWRAPPER_ADAPTER_BLOCK AdapterQueue;// queue of adapters for this MAC
    REFERENCE Ref;                      // contains spinlock for AdapterQueue
    UINT Length;                        // of this WRAPPER_MAC_BLOCK structure
    NDIS_HANDLE MacMacContext;          // Mac handle
    NDIS_HANDLE NdisWrapperHandle;      // Wrapper Handle obtained in NdisRegisterMac
    UINT nLoadCount;
    MODULE_LIST ModuleList;             // list of the LE module names for this mac
    BOOLEAN IsWidget;                    // Is this a widget driver?
    BOOLEAN Reserved;                    // to force function table in MacCharacteristics to be dword aligned
    NDIS_MAC_CHARACTERISTICS MacCharacteristics;    // handler addresses
};


//
// one of these per adapter registered on a MAC
//


struct _WRAPPER_ADAPTER_BLOCK {
    UINT Signature;                     //Debugging Signature NADP
    PWRAPPER_ADAPTER_BLOCK pNext;          // Used by Wrapper
    PWRAPPER_MAC_BLOCK MacHandle;          // pointer to our MAC block
    NDIS_HANDLE MacAdapterContext;      // context when calling MacOpenAdapter
    NDIS_STRING AdapterName;            // adapter name string
    PWRAPPER_OPEN_BLOCK OpenQueue;         // queue of opens for this adapter
    PWRAPPER_ADAPTER_BLOCK NextAdapter;    // used by MAC's AdapterQueue
    REFERENCE Ref;                      // contains spinlock for OpenQueue
    ADAPTER_SHUTDOWN_HANDLER ShutdownHandler;
    PVOID ShutdownContext;
    NDIS_HANDLE DmaHandle;              // handle to DMA channel (if needed)
    PNDIS_ADAPTER_INFORMATION AdapterInfoP;
    DEVNODE dnDevNode;                  // Device node passed fron CM
    UINT bDriver;                       // type of driver Flag (STATIC, DYN, LOAD_SYN)
    UINT dwPnpFlags;                    // PnP flags (Removed,...)
    NDIS_STRING nsMacName;              // Mac Name
    NDIS_STRING nsAdapterRegName;       // Adapter name in registry
    UINT nCloseCount;                   // number of times protocols processed close on this adapter
    NDIS_STRING nsDeviceDesc;           // Yet another name to keep track of!  
    UINT cErrorCount;                   // number of times WriteErrorLog called for this adapter  
    NDIS_TIMER ShutdownTimer;
    PNDIS_RESOURCE_LIST pPCIResourceList;  // pointer to an array of resources allocated to this adapter
    NDIS_INTERFACE_TYPE BusType;
};

/* Possible Values for bDriver */
#define NO_DRIVER           0
#define STATIC_DRIVER       1
#define DYN_DRIVER          2
#define LOAD_DYN_DRIVER     3
#define NDIS2_DRIVER        4
#define ODI_DRIVER          5
#define WIDGET_DRIVER       6


/* Adapter PnP Flags */
#define NDISPNP_NODE_BEING_REMOVED          0x00000001
#define NDISPNP_NODE_REGISTERED             0x00000002
#define NDISPNP_NODE_STOPPED                0x00000004
#define NDISPNP_NODE_ALREADY_REMOVED        0x00000008
#define NDISPNP_NODE_TEST_REMOVE_SUCCEEDED  0x00000010
#define NDISPNP_NODE_INNEWDEVNODE           0x00000020
#define NDISPNP_NODE_RECEIVED_PREREMOVE     0x00000040
#define NDISPNP_NODE_DELAY_BINDING          0x00000080
#define NDISPNP_NODE_DONT_REMOVE_RESOURCES  0x00000100
#define NDISPNP_NODE_DISABLE_WARNING        0x00000200
//
// one of these per protocol registered
//

struct _WRAPPER_PROTOCOL_BLOCK {
    UINT Signature;                 	// debug Signature NPRT
    PWRAPPER_PROTOCOL_BLOCK pNext;      // Used by Wrapper
    PWRAPPER_OPEN_BLOCK OpenQueue;      // queue of opens for this protocol
    REFERENCE Ref;                      // contains spinlock for OpenQueue
    UINT Length;                        // of this WRAPPER_PROTOCOL_BLOCK struct
    UINT nLoadCount;
    MODULE_LIST ModuleList;             // list of the LE module names for this Protocol
    DEVNODE dnCurrentDevNode;           // Current Dev Node for Protocol
    UINT nTRSCount;                     // Number of times we incremented ref because of TEST_REMOVE_SUCCEEDED message
    USHORT Reserved;                    // to force function table in ProtocolCharacteristics to be dword aligned
    NDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics;  // handler addresses
};


typedef struct _WRAPPER_BIND_UNBIND_CONTEXT{
    NDIS_STATUS     Status;
    NDIS_STATUS     ErrorStatus;
    UINT            Complete;
    } WRAPPER_BIND_UNBIND_CONTEXT, *PWRAPPER_BIND_UNBIND_CONTEXT;
    
typedef struct _WRAPPER_DEVNODE_LIST       WRAPPER_DEVNODE_LIST, *PWRAPPER_DEVNODE_LIST;

struct _WRAPPER_DEVNODE_LIST{
    DEVNODE     dnDevNode;
    PWRAPPER_DEVNODE_LIST pNext;
};

//
// one of these per open on an adapter/protocol
//

struct _WRAPPER_OPEN_BLOCK {
    UINT Signature;                         // signature for debugging
    PWRAPPER_OPEN_BLOCK AdapterNextOpen;    // used by adapter's OpenQueue
    PWRAPPER_OPEN_BLOCK ProtocolNextOpen;   // used by protocol's OpenQueue
    PWRAPPER_MAC_BLOCK MacHandle;           // pointer to our MAC
    NDIS_HANDLE MacBindingHandle;           // context when calling MacXX funcs
    PWRAPPER_ADAPTER_BLOCK AdapterHandle;   // pointer to our adapter
    DEVNODE dnAdapterNode;                  // Adapter Dev node
    PWRAPPER_PROTOCOL_BLOCK ProtocolHandle; // pointer to our protocol
    DEVNODE dnProtocolNode;                 // Protocol Dev Node
    NDIS_HANDLE ProtocolBindingContext;     // context when calling ProtXX funcs
    PWRAPPER_BIND_UNBIND_CONTEXT BindUnbindContext; // context when calling ProtocolBind/Unbind
    BOOLEAN Closing;                        // TRUE when removing this struct
    UCHAR Reserved[3];
    NDIS_HANDLE CloseRequestHandle;         // 0 indicates an internal close
    NDIS_SPIN_LOCK SpinLock;                // guards Closing
    DWORD dwPnpFlags;                       //
    //
    // These are optimizations for getting to MAC routines. 
    //

    SEND_HANDLER SendHandler;
    TRANSFER_DATA_HANDLER TransferDataHandler;

    //
    // These are optimizations for getting to PROTOCOL routines. 
    //

    SEND_COMPLETE_HANDLER SendCompleteHandler;
    TRANSFER_DATA_COMPLETE_HANDLER TransferDataCompleteHandler;
    RECEIVE_HANDLER ReceiveHandler;
    RECEIVE_COMPLETE_HANDLER ReceiveCompleteHandler;

    PWRAPPER_DEVNODE_LIST pChildDevNodeList;
};

        


//
// Types of Memory (not mutually exclusive)
//

#define NDIS_MEMORY_CONTIGUOUS              0x00000001
#define NDIS_MEMORY_NONCACHED               0x00000002

//
// Open options
//
#define NDIS_OPEN_RECEIVE_NOT_REENTRANT         0x00000001


//
// NDIS_STATUS values
//

/* INC */
#define NDIS_STATUS_SUCCESS                 ((NDIS_STATUS)0x00000000L)
#define NDIS_STATUS_PENDING                 ((NDIS_STATUS)0x00000103L)
#define NDIS_STATUS_NOT_RECOGNIZED          ((NDIS_STATUS)0x00010001L)
#define NDIS_STATUS_NOT_COPIED              ((NDIS_STATUS)0x00010002L)
#define NDIS_STATUS_NOT_ACCEPTED            ((NDIS_STATUS)0x00010003L)
#define NDIS_STATUS_MAY_CLOSE               ((NDIS_STATUS)0x00010004L)

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

#define NDIS_STATUS_FAILURE                 ((NDIS_STATUS)0xC0000001L)
#define NDIS_STATUS_RESOURCES               ((NDIS_STATUS)0xC000009AL)

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
#define NDIS_STATUS_NOT_SUPPORTED           ((NDIS_STATUS)0xC00000BBL)
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
#define NDIS_STATUS_NO_CABLE                ((NDIS_STATUS)0xC001001FL)

#define NDIS_STATUS_TOKEN_RING_OPEN_ERROR   ((NDIS_STATUS)0xC0011000L)

// For NT Compatability
#define STATUS_SUCCESS NDIS_STATUS_SUCCESS
#define STATUS_UNSUCCESSFUL NDIS_STATUS_FAILURE


//
// used in error logging
//

#define NDIS_ERROR_CODE LONG

#define NDIS_ERROR_CODE_RESOURCE_CONFLICT               0xAA000001L //EVENT_NDIS_RESOURCE_CONFLICT
#define NDIS_ERROR_CODE_OUT_OF_RESOURCES                0xAA000002L //EVENT_NDIS_OUT_OF_RESOURCE
#define NDIS_ERROR_CODE_HARDWARE_FAILURE                0xAA000003L //EVENT_NDIS_HARDWARE_FAILURE
#define NDIS_ERROR_CODE_ADAPTER_NOT_FOUND               0xAA000004L //EVENT_NDIS_ADAPTER_NOT_FOUND
#define NDIS_ERROR_CODE_INTERRUPT_CONNECT               0xAA000005L //EVENT_NDIS_INTERRUPT_CONNECT
#define NDIS_ERROR_CODE_DRIVER_FAILURE                  0xAA000006L //EVENT_NDIS_DRIVER_FAILURE
#define NDIS_ERROR_CODE_BAD_VERSION                     0xAA000007L //EVENT_NDIS_BAD_VERSION
#define NDIS_ERROR_CODE_TIMEOUT                         0xAA000008L //EVENT_NDIS_TIMEOUT
#define NDIS_ERROR_CODE_NETWORK_ADDRESS                 0xAA000009L //EVENT_NDIS_NETWORK_ADDRESS
#define NDIS_ERROR_CODE_UNSUPPORTED_CONFIGURATION       0xAA00000AL //EVENT_NDIS_UNSUPPORTED_CONFIGURATION
#define NDIS_ERROR_CODE_INVALID_VALUE_FROM_ADAPTER      0xAA00000BL //EVENT_NDIS_INVALID_VALUE_FROM_ADAPTER
#define NDIS_ERROR_CODE_MISSING_CONFIGURATION_PARAMETER 0xAA00000CL //EVENT_NDIS_MISSING_CONFIGURATION_PARAMETER
#define NDIS_ERROR_CODE_BAD_IO_BASE_ADDRESS             0xAA00000DL //EVENT_NDIS_BAD_IO_BASE_ADDRESS
#define NDIS_ERROR_CODE_RECEIVE_SPACE_SMALL             0xAA00000EL //EVENT_NDIS_RECEIVE_SPACE_SMALL
#define NDIS_ERROR_CODE_ADAPTER_DISABLED                0xAA00000FL //EVENT_NDIS_ADAPTER_DISABLED



//
// Ndis Packet Filter Bits
//

#define NDIS_PACKET_TYPE_DIRECTED           0x0001
#define NDIS_PACKET_TYPE_MULTICAST          0x0002
#define NDIS_PACKET_TYPE_ALL_MULTICAST      0x0004
#define NDIS_PACKET_TYPE_BROADCAST          0x0008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING     0x0010
#define NDIS_PACKET_TYPE_PROMISCUOUS        0x0020
#define NDIS_PACKET_TYPE_SMT                0x0040
#define NDIS_PACKET_TYPE_MAC_FRAME          0x8000
#define NDIS_PACKET_TYPE_FUNCTIONAL         0x4000
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL     0x2000
#define NDIS_PACKET_TYPE_GROUP              0x1000

//
// Ndis Token-Ring Ring Status Codes
//

#define NDIS_RING_SIGNAL_LOSS               0x00008000
#define NDIS_RING_HARD_ERROR                0x00004000
#define NDIS_RING_SOFT_ERROR                0x00002000
#define NDIS_RING_TRANSMIT_BEACON           0x00001000
#define NDIS_RING_LOBE_WIRE_FAULT           0x00000800
#define NDIS_RING_AUTO_REMOVAL_ERROR        0x00000400
#define NDIS_RING_REMOVE_RECEIVED           0x00000200
#define NDIS_RING_COUNTER_OVERFLOW          0x00000100
#define NDIS_RING_SINGLE_STATION            0x00000080
#define NDIS_RING_RING_RECOVERY             0x00000040

//
// Ndis protocol option bits (OID_GEN_PROTOCOL_OPTIONS).
//

#define NDIS_PROT_OPTION_ESTIMATED_LENGTH   0x00000001
#define NDIS_PROT_OPTION_NO_LOOPBACK        0x00000002

//
// Ndis MAC option bits (OID_GEN_MAC_OPTIONS).
//

#define NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA             0x00000001
#define NDIS_MAC_OPTION_RECEIVE_SERIALIZED              0x00000002
#define NDIS_MAC_OPTION_TRANSFERS_NOT_PEND              0x00000004
#define NDIS_MAC_OPTION_NO_LOOPBACK                     0x00000008
//
// Object Identifiers used by NdisRequest Query/Set Information
//

//
// General Objects
//

#define OID_GEN_SUPPORTED_LIST              0x00010101
#define OID_GEN_HARDWARE_STATUS             0x00010102
#define OID_GEN_MEDIA_SUPPORTED             0x00010103
#define OID_GEN_MEDIA_IN_USE                0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD           0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE          0x00010106
#define OID_GEN_LINK_SPEED                  0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE       0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE        0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE         0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE          0x0001010B
#define OID_GEN_VENDOR_ID                   0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION          0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER       0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD           0x0001010F
#define OID_GEN_DRIVER_VERSION              0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE          0x00010111
#define OID_GEN_PROTOCOL_OPTIONS            0x00010112
#define OID_GEN_MAC_OPTIONS                 0x00010113

#define OID_GEN_XMIT_OK                     0x00020101
#define OID_GEN_RCV_OK                      0x00020102
#define OID_GEN_XMIT_ERROR                  0x00020103
#define OID_GEN_RCV_ERROR                   0x00020104
#define OID_GEN_RCV_NO_BUFFER               0x00020105

#define OID_GEN_DIRECTED_BYTES_XMIT         0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT        0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT        0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT       0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT        0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT       0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV          0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV         0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV         0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV        0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV         0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV        0x0002020C

#define OID_GEN_RCV_CRC_ERROR               0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH       0x0002020E


//
// 802.3 Objects (Ethernet)
//

#define OID_802_3_PERMANENT_ADDRESS         0x01010101
#define OID_802_3_CURRENT_ADDRESS           0x01010102
#define OID_802_3_MULTICAST_LIST            0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE         0x01010104

#define OID_802_3_RCV_ERROR_ALIGNMENT       0x01020101
#define OID_802_3_XMIT_ONE_COLLISION        0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS      0x01020103

#define OID_802_3_XMIT_DEFERRED             0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS       0x01020202
#define OID_802_3_RCV_OVERRUN               0x01020203
#define OID_802_3_XMIT_UNDERRUN             0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE    0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST       0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS      0x01020207


//
// 802.5 Objects (Token-Ring)
//

#define OID_802_5_PERMANENT_ADDRESS         0x02010101
#define OID_802_5_CURRENT_ADDRESS           0x02010102
#define OID_802_5_CURRENT_FUNCTIONAL        0x02010103
#define OID_802_5_CURRENT_GROUP             0x02010104
#define OID_802_5_LAST_OPEN_STATUS          0x02010105
#define OID_802_5_CURRENT_RING_STATUS       0x02010106
#define OID_802_5_CURRENT_RING_STATE        0x02010107

#define OID_802_5_LINE_ERRORS               0x02020101
#define OID_802_5_LOST_FRAMES               0x02020102

#define OID_802_5_BURST_ERRORS              0x02020201
#define OID_802_5_AC_ERRORS                 0x02020202
#define OID_802_5_ABORT_DELIMETERS          0x02020203
#define OID_802_5_FRAME_COPIED_ERRORS       0x02020204
#define OID_802_5_FREQUENCY_ERRORS          0x02020205
#define OID_802_5_TOKEN_ERRORS              0x02020206
#define OID_802_5_INTERNAL_ERRORS           0x02020207


//
// FDDI Objects
//

#define OID_FDDI_LONG_PERMANENT_ADDR        0x03010101
#define OID_FDDI_LONG_CURRENT_ADDR          0x03010102
#define OID_FDDI_LONG_MULTICAST_LIST        0x03010103
#define OID_FDDI_LONG_MAX_LIST_SIZE         0x03010104
#define OID_FDDI_SHORT_PERMANENT_ADDR       0x03010105
#define OID_FDDI_SHORT_CURRENT_ADDR         0x03010106
#define OID_FDDI_SHORT_MULTICAST_LIST       0x03010107
#define OID_FDDI_SHORT_MAX_LIST_SIZE        0x03010108


//
// WAN objects
//

#define OID_WAN_PERMANENT_ADDRESS           0x04010101
#define OID_WAN_CURRENT_ADDRESS             0x04010102
#define OID_WAN_QUALITY_OF_SERVICE          0x04010103
#define OID_WAN_PROTOCOL_TYPE               0x04010104
#define OID_WAN_MEDIUM_SUBTYPE              0x04010105
#define OID_WAN_HEADER_FORMAT               0x04010106


//
// LocalTalk objects
//

#define OID_LTALK_CURRENT_NODE_ID           0x05010102

#define OID_LTALK_IN_BROADCASTS             0x05020101
#define OID_LTALK_IN_LENGTH_ERRORS          0x05020102

#define OID_LTALK_OUT_NO_HANDLERS           0x05020201
#define OID_LTALK_COLLISIONS                0x05020202
#define OID_LTALK_DEFERS                    0x05020203
#define OID_LTALK_NO_DATA_ERRORS            0x05020204
#define OID_LTALK_RANDOM_CTS_ERRORS         0x05020205
#define OID_LTALK_FCS_ERRORS                0x05020206


//
// Arcnet objects
//
#define OID_ARCNET_PERMANENT_ADDRESS        0x06010101
#define OID_ARCNET_CURRENT_ADDRESS          0x06010102
#define OID_ARCNET_RECONFIGURATIONS         0x06020201


//
// Chicago Implementation Specific objects
//

#define OID_PRIVATE_PROTOCOL_HANDLE         0xff010207


/* NOINC */

//
// SpinLocks
//


#ifdef DEBUG

#define NdisAllocateSpinLock(SpinLock) \
    *(SpinLock)=0;

#define NdisFreeSpinLock(SpinLock) {\
    if(*(SpinLock)) DbgBreakPoint(); \
    }

#define NdisAcquireSpinLock(SpinLock) {\
    if(*(SpinLock)>0) {DbgBreakPoint();} \
    else *(SpinLock) = 1 ;\
    }

#define NdisReleaseSpinLock(SpinLock) {\
    if (*(SpinLock)==0) {DbgBreakPoint();} \
    else (*(SpinLock))--; \
    }

#else   // Retail

#define NdisAllocateSpinLock(SpinLock)
#define NdisFreeSpinLock(SpinLock)
#define NdisAcquireSpinLock(SpinLock)
#define NdisReleaseSpinLock(SpinLock)

#endif  // DEBUG

#define NdisDprAcquireSpinLock(SpinLock) NdisAcquireSpinLock(SpinLock)
#define NdisDprReleaseSpinLock(SpinLock) NdisReleaseSpinLock(SpinLock)


//
// Interlocked support functions
//

//
// NT style interlock functions
//
// Note: these functions will have to be rewritten if DOS RING 0 becomes
//       pre-emptive.  There should be an AcquireSpinLock and ReleaseSpinLock
//       surrounding each of these functions.
//
// Note: these functions have no implementation other than as macros

#define NdisInterlockedAddUlong(Addend,Increment,SpinLock) \
    ((*(Addend))+=(Increment));

#define NdisInterlockedInsertHeadList(ListHead,ListEntry,SpinLock) \
    InsertHeadList(ListHead,ListEntry);

#define NdisInterlockedInsertTailList(ListHead,ListEntry,SpinLock) \
    InsertTailList(ListHead,ListEntry);

#define NdisInterlockedRemoveHeadList(ListHead,SpinLock) \
    RemoveHeadList(ListHead);

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
// Configuration Requests
//

VOID NDIS_API
NdisOpenConfiguration(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE ConfigurationHandle,
    IN  NDIS_HANDLE WrapperConfigurationContext
    );

VOID NDIS_API
NdisReadConfiguration(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_CONFIGURATION_PARAMETER *ParameterValue,
    IN NDIS_HANDLE ConfigurationHandle,
    IN PNDIS_STRING Parameter,
    IN NDIS_PARAMETER_TYPE ParameterType
    );

VOID NDIS_API
NdisCloseConfiguration(
    IN NDIS_HANDLE ConfigurationHandle
    );

VOID NDIS_API
NdisReadMcaPosInformation(
        OUT PNDIS_STATUS Status,
        IN NDIS_HANDLE WrapperConfigurationContext,
        OUT PUINT ChannelNumber,
        OUT PNDIS_MCA_POS_DATA McaData
        );

VOID NDIS_API
NdisReadEisaSlotInformation(
        OUT PNDIS_STATUS Status,
        IN NDIS_HANDLE WrapperConfigurationContext,
        OUT PUINT SlotNumber,
        OUT PNDIS_EISA_FUNCTION_INFORMATION EisaData
        );
        

ULONG
NdisImmediateReadPciSlotInformation(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG SlotNumber,
    IN ULONG Offset,
    IN PVOID Buffer,
    IN ULONG Length
    );
    
ULONG
NdisImmediateWritePciSlotInformation(
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG SlotNumber,
    IN ULONG Offset,
    IN PVOID Buffer,
    IN ULONG Length
    );
    
ULONG
NdisReadPciSlotInformation(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG SlotNumber,
    IN ULONG Offset,
    IN PVOID Buffer,
    IN ULONG Length
    );
    
ULONG
NdisWritePciSlotInformation(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG SlotNumber,
    IN ULONG Offset,
    IN PVOID Buffer,
    IN ULONG Length
    );


NDIS_STATUS
NdisPciAssignResources(
    IN NDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE NdisWrapperHandle,
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN ULONG SlotNumber,
    OUT PNDIS_RESOURCE_LIST *AssignedResources
    );
    
        
VOID NDIS_API
NdisReadNetworkAddress(
    OUT PNDIS_STATUS Status,
    OUT PVOID * NetworkAddress,
    OUT PUINT NetworkAddressLength,
    IN NDIS_HANDLE ConfigurationHandle
    );

VOID NDIS_API
NdisReadBindingInformation(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_STRING * Binding,
    IN NDIS_HANDLE ConfigurationHandle
    );


//
// OS specific interface for reading adapter resources
//

typedef enum _WRAPPER_RESOURCE_TYPE{
    WrapperResourceTypeAll  = 0x00000000,
    WrapperResourceTypeMem  = 0x00000001,
    WrapperResourceTypeIO   = 0x00000002,
    WrapperResourceTypeDMA  = 0x00000003,
    WrapperResourceTypeIRQ  = 0x00000004
    } WRAPPER_RESOURCE_TYPE, *PWRAPPER_RESOURCE_TYPE;

typedef struct	wMem_Des_s {
	WORD			MD_Count;
	WORD			MD_Type;
	ULONG			MD_Alloc_Base;
	ULONG			MD_Alloc_End;
	WORD			MD_Flags;
	WORD			MD_Reserved;
} WRAPPER_MEM_DES, *PWRAPPER_MEM_DES;


typedef struct	wIO_Des_s {
	WORD			IOD_Count;
	WORD			IOD_Type;
	WORD			IOD_Alloc_Base;
	WORD			IOD_Alloc_End;
	WORD			IOD_DesFlags;
	BYTE			IOD_Alloc_Alias;
	BYTE			IOD_Alloc_Decode;
} WRAPPER_IO_DES, *PWRAPPER_IO_DES;

typedef struct	wDMA_Des_s {
	BYTE			DD_Flags;
	BYTE			DD_Alloc_Chan;	// Channel number allocated
	BYTE			DD_Req_Mask;	// Mask of possible channels
	BYTE			DD_Reserved;
} WRAPPER_DMA_DES, *PWRAPPER_DMA_DES;

typedef struct	wIRQ_Des_s {
	WORD			IRQD_Flags;
	WORD			IRQD_Alloc_Num;		// Allocated IRQ number
	WORD			IRQD_Req_Mask;		// Mask of possible IRQs
	WORD			IRQD_Reserved;
} WRAPPER_IRQ_DES, *PWRAPPER_IRQ_DES;

typedef union WRAPPER_ALL_RES_DES {
    WRAPPER_MEM_DES memResDes;
    WRAPPER_IO_DES  ioResDes;
    WRAPPER_DMA_DES dmaResDes;
    WRAPPER_IRQ_DES irqResDes;
} WRAPPER_ALL_RES_DES, *PWRAPPER_ALL_RES_DES;

VOID __cdecl
WrapperQueryAdapterResources(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN WRAPPER_RESOURCE_TYPE ResourceType,
    IN PVOID Buffer,
    IN UINT BufferSize,
    OUT PUINT BytesWritten,
    OUT PUINT BytesNeeded
    );


VOID NDIS_API
WrapperDelayBinding(
    IN NDIS_HANDLE NdisBindingContext
    );
    
VOID NDIS_API
WrapperResumeBinding(
    IN NDIS_HANDLE NdisBindingContext
    );

VOID NDIS_API
WrapperRemoveChildren(
    IN NDIS_HANDLE NdisBindingContext
    );
                                                        
VOID NDIS_API
NdisAllocatePacketPool(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE PoolHandle,
    IN UINT NumberOfDescriptors,
    IN UINT ProtocolReservedLength
    );


NDIS_STATUS NDIS_API
NdisFreePacketPool(
    IN NDIS_HANDLE PoolHandle
    );


VOID NDIS_API
NdisAllocateBufferPool(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE PoolHandle,
    IN UINT NumberOfDescriptors
    );


NDIS_STATUS NDIS_API
NdisFreeBufferPool(
    IN NDIS_HANDLE PoolHandle);


VOID NDIS_API
NdisAllocateBuffer(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_BUFFER * Buffer,
    IN NDIS_HANDLE PoolHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    );

VOID NDIS_API
NdisCopyBuffer(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_BUFFER * Buffer,
    IN NDIS_HANDLE PoolHandle,
    IN PVOID MemoryDescriptor,
    IN UINT Offset,
    IN UINT Length
    );


#ifndef NDISMACROS
VOID NDIS_API
NdisFreeBuffer(
    IN PNDIS_BUFFER Buffer
    );
#else
#define NdisFreeBuffer(Buffer) \
{\
    NdisAcquireSpinLock(&((Buffer)->Pool->SpinLock)); \
    (Buffer)->Next = (PNDIS_BUFFER)(Buffer)->Pool->FreeList;\
    (Buffer)->Pool->FreeList = Buffer;\
    NdisReleaseSpinLock(&((Buffer)->Pool->SpinLock));\
}
#endif


#ifndef NDISMACROS
VOID NDIS_API
NdisQueryBuffer(
    IN PNDIS_BUFFER Buffer,
    OUT PVOID *VirtualAddress,
    OUT PUINT Length
    );
#else
#define NdisQueryBuffer(_Buffer, _VirtualAddress, _Length)\
    *(_VirtualAddress) = (_Buffer)->VirtualAddress;\
    *(_Length) = (_Buffer)->Length;
#endif


VOID NDIS_API
NdisQueryBufferOffset(
    IN PNDIS_BUFFER Buffer,
    OUT PUINT Offset,
    OUT PUINT Length
    );


VOID NDIS_API
NdisGetBufferPhysicalAddress(
    IN PNDIS_BUFFER Buffer,
    IN OUT PNDIS_PHYSICAL_ADDRESS_UNIT PhysicalAddressArray,
    OUT PUINT ArraySize
    );

/*++

VOID NDIS_API
NdisGetBufferPhysicalArraySize(
    IN PNDIS_BUFFER Buffer,
    OUT PUINT ArraySize
    );

--*/

#define NdisGetBufferPhysicalArraySize(Buffer, ArraySize) \
    (*(ArraySize) = ADDRESS_AND_SIZE_TO_SPAN_PAGES(\
                    Buffer->VirtualAddress, \
                    Buffer->Length))

//
// NdisBufferGetSystemSpecific allows NDIS to give additional MDL info to
// the MAC and Protocol layers. This is not meaningful in DOS, converted
// to a NULL macro.
//
#define NdisBufferGetSystemSpecific(Buffer,VirtualAddress,Length)


VOID NDIS_API
NdisAllocatePacket(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_PACKET * Packet,
    IN NDIS_HANDLE PoolHandle
    );


#ifndef NDISMACROS
VOID NDIS_API
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


#ifndef NDISMACROS
VOID NDIS_API
NdisReinitializePacket(
    IN OUT PNDIS_PACKET Packet
    );
#else
#define NdisReinitializePacket(Packet) { \
    memset(((PUCHAR)Packet) + sizeof(NDIS_PACKET_PRIVATE), 0, \
            TmpPool->PacketLength - sizeof(NDIS_PACKET_PRIVATE) );\
    (Packet)->Private.Head = (PNDIS_BUFFER)NULL; \
    (Packet)->Private.Count = 0; \
    (Packet)->Private.PhysicalCount = 0; \
    (Packet)->Private.TotalLength = 0; \
}
#endif


#ifndef NDISMACROS
VOID NDIS_API
NdisChainBufferAtFront(
    IN OUT PNDIS_PACKET Packet,
    IN OUT PNDIS_BUFFER Buffer
    );
#else
#define NdisChainBufferAtFront(Packet, Buffer) { \
    PNDIS_BUFFER TmpBuffer = (Buffer); \
    UINT AddedTotalLength = 0, AddedPhysicalCount = 0, AddedCount=0; \
    for (;;) { \
    AddedTotalLength += TmpBuffer->Length; \
    AddedPhysicalCount += ADDRESS_AND_SIZE_TO_SPAN_PAGES( \
                   TmpBuffer->Address, \
                   TmpBuffer->Length); \
    ++AddedCount; \
    if (TmpBuffer->Next == (PNDIS_BUFFER)NULL) \
            break; \
    TmpBuffer = TmpBuffer->Next; \
    } \
    if ((Packet)->Private.Head == (PNDIS_BUFFER)NULL) { \
    (Packet)->Private.Tail = TmpBuffer; \
    } \
    TmpBuffer->Next = (Packet)->Private.Head; \
    (Packet)->Private.Head = (Buffer); \
    (Packet)->Private.Count += AddedCount; \
    (Packet)->Private.TotalLength += AddedTotalLength; \
    (Packet)->Private.PhysicalCount += AddedPhysicalCount; \
}
#endif

#ifndef NDISMACROS
VOID NDIS_API
NdisChainBufferAtBack(
    IN OUT PNDIS_PACKET Packet,
    IN OUT PNDIS_BUFFER Buffer
    );
#else
#define NdisChainBufferAtBack(Packet, Buffer) { \
    PNDIS_BUFFER TmpBuffer = (Buffer); \
    UINT AddedTotalLength = 0, AddedPhysicalCount = 0, AddedCount = 0; \
\
    for (;;) { \
    AddedTotalLength += TmpBuffer->Length; \
    AddedPhysicalCount += ADDRESS_AND_SIZE_TO_SPAN_PAGES( \
                   TmpBuffer->Address, \
                   TmpBuffer->Length); \
    ++AddedCount; \
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
    (Packet)->Private.Count += AddedCount; \
    (Packet)->Private.TotalLength += AddedTotalLength; \
    (Packet)->Private.PhysicalCount += AddedPhysicalCount; \
}
#endif

#ifndef NDISMACROS
VOID NDIS_API
NdisUnchainBufferAtFront(
    IN OUT PNDIS_PACKET Packet,
    OUT PNDIS_BUFFER * Buffer
    );
#else
#define NdisUnchainBufferAtFront(Packet, Buffer) \
    *(Buffer) = (Packet)->Private.Head; \
    if (*(Buffer) != (PNDIS_BUFFER)NULL) { \
    --((Packet)->Private.Count); \
    (Packet)->Private.Head = (*(Buffer))->Next; \
    (*(Buffer))->Next = (PNDIS_BUFFER)NULL; \
    (Packet)->Private.TotalLength -= (*(Buffer))->Length; \
    (Packet)->Private.PhysicalCount -= ADDRESS_AND_SIZE_TO_SPAN_PAGES( \
               (*(Buffer))->VirtualAddress, \
               (*(Buffer))->Length); \
    }
#endif

VOID NDIS_API
NdisUnchainBufferAtBack(
    IN OUT PNDIS_PACKET Packet,
    OUT PNDIS_BUFFER * Buffer
    );

#ifndef NDISMACROS
VOID NDIS_API
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
    if (_PhysicalBufferCount) *_PhysicalBufferCount = Packet->Private.PhysicalCount; \
    if (_BufferCount) *_BufferCount = Packet->Private.Count; \
    if (_FirstBuffer) *_FirstBuffer = Packet->Private.Head; \
    if (_TotalPacketLength) *_TotalPacketLength = Packet->Private.TotalLength; \
}
#endif


#ifndef NDISMACROS
VOID NDIS_API
NdisGetNextBuffer(
    IN PNDIS_BUFFER CurrentBuffer,
    OUT PNDIS_BUFFER * NextBuffer
    );
#else
#define NdisGetNextBuffer(CurrentBuffer, NextBuffer) {\
    *(NextBuffer) = (CurrentBuffer)->Next; \
}
#endif

VOID NDIS_API
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


VOID NDIS_API
NdisMapIoSpace(
    OUT PNDIS_STATUS Status,
    OUT PVOID * VirtualAddress,
    IN  NDIS_HANDLE NdisAdapterHandle,
    IN  NDIS_PHYSICAL_ADDRESS PhysicalAddress,
    IN  UINT Length
    );

/*++
VOID NDIS_API
NdisUnmapIoSpace(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PVOID VirtualAddress,
    IN UINT Length
    )
--*/
#define NdisUnmapIoSpace(Handle,VirtualAddress,Length) \
    //Unmapping IO Space in not required in DOS (see VDA guide)


NDIS_STATUS NDIS_API
NdisAllocateMemory(
    OUT PVOID *Buffer,
    IN UINT Length,
    IN UINT MemoryFlags,
    IN NDIS_PHYSICAL_ADDRESS MaximumPhysicalAddress OPTIONAL
    );

VOID NDIS_API
NdisFreeMemory(
    IN PVOID VirtualAddress,
    IN UINT Length,
    IN UINT MemoryFlags
    );

/*++
VOID NDIS_API
NdisInitializeTimer(
    IN OUT PNDIS_TIMER Timer,
    PVOID Function,
    PVOID Context
    )
--*/

#ifdef M7
#define NdisInitializeTimer(Timer, Function, Context)\
{\
    (Timer)->TimerHandle=NULL;\
    (Timer)->CallbackFn=(Function);\
    (Timer)->CallbackContext=(Context);\
    (Timer)->ulFlags=0;\
}
#else
#define NdisInitializeTimer(Timer, Function, Context)\
{\
    (Timer)->TimerHandle=NULL;\
    (Timer)->CallbackFn=(Function);\
    (Timer)->CallbackContext=(Context);\
}
#endif


VOID NDIS_API
NdisSetTimer(
    IN PNDIS_TIMER Timer,
    IN UINT MillisecondsToDelay
    );

VOID NDIS_API
NdisCancelTimer(
    IN PNDIS_TIMER Timer,
    OUT PBOOLEAN   TimerCancelled
    );

VOID NDIS_API
NdisStallExecution(
    IN UINT MillisecondsToStall
    );

VOID NDIS_API
NdisInitializeInterrupt(
    OUT PNDIS_STATUS Status,
    IN OUT PNDIS_INTERRUPT Interrupt,
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_INTERRUPT_SERVICE InterruptServiceRoutine,
    IN PVOID InterruptContext,
    IN PNDIS_DEFERRED_PROCESSING DefferredProcessingRoutine,
    IN UINT InterruptVector,
    IN UINT InterruptLevel,
    IN BOOLEAN SharedInterrupt,
    IN NDIS_INTERRUPT_MODE InterruptMode
    );

VOID NDIS_API
NdisRemoveInterrupt(
    IN PNDIS_INTERRUPT Interrupt
    );

BOOLEAN NDIS_API
NdisSynchronizeWithInterrupt(
    IN PNDIS_INTERRUPT Interrupt,
    IN PVOID SynchronizeFunction,
    IN PVOID SynchronizeContext
    );

//
// Simple I/O support
//

VOID NDIS_API
NdisOpenFile(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE FileHandle,
    OUT PUINT FileLength,
    IN PNDIS_STRING FileName,
    IN NDIS_PHYSICAL_ADDRESS HighestAcceptableAddress
    );

VOID NDIS_API
NdisCloseFile(
    IN NDIS_HANDLE FileHandle
    );

VOID NDIS_API
NdisMapFile(
    OUT PNDIS_STATUS Status,
    OUT PVOID * MappedBuffer,
    IN NDIS_HANDLE FileHandle
    );

VOID NDIS_API
NdisUnmapFile(
    IN NDIS_HANDLE FileHandle
    );


//
// Portability Extensions
//
/*++
VOID NDIS_API
NdisFlushBuffer(
    IN PNDIS_BUFFER Buffer,
    IN BOOLEAN WriteToDevice
    )
--*/

#define NdisFlushBuffer(Buffer,WriteToDevice)

//
//  Read/Write Buffer Ports, under DOS all I/O are raw, so map them to
//  NdisRaw routines to avoid passing extra parameters
//

VOID NDIS_API
NdisRawReadPortBufferUchar(
    IN ULONG Port,
    IN PUCHAR Buffer,
    IN ULONG Length
    );
#define NdisReadPortBufferUchar(NdisAdapterHandle,Port,Buffer,Length) \
        NdisRawReadPortBufferUchar((ULONG)(Port),(PUCHAR)(Buffer),(ULONG)(Length))

VOID NDIS_API
NdisRawReadPortBufferUshort(
    IN ULONG Port,
    IN PUSHORT Buffer,
    IN ULONG Length
    );
#define NdisReadPortBufferUshort(NdisAdapterHandle,Port,Buffer,Length) \
        NdisRawReadPortBufferUshort((ULONG)(Port),(PUSHORT)(Buffer),(ULONG)(Length))

VOID NDIS_API
NdisRawReadPortBufferUlong(
    IN ULONG Port,
    IN PULONG Buffer,
    IN ULONG Length
    );
#define NdisReadPortBufferUlong(NdisAdapterHandle,Port,Buffer,Length) \
        NdisRawReadPortBufferUlong((ULONG)(Port),(PULONG)(Buffer),(ULONG)(Length))

VOID NDIS_API
NdisRawWritePortBufferUchar(
    IN ULONG Port,
    IN PUCHAR Buffer,
    IN ULONG Length
    );
#define NdisWritePortBufferUchar(NdisAdapterHandle,Port,Buffer,Length) \
        NdisRawWritePortBufferUchar((ULONG)(Port),(PUCHAR)(Buffer),(ULONG)(Length))

VOID NDIS_API
NdisRawWritePortBufferUshort(
    IN ULONG Port,
    IN PUSHORT Buffer,
    IN ULONG Length
    );
#define NdisWritePortBufferUshort(NdisAdapterHandle,Port,Buffer,Length) \
        NdisRawWritePortBufferUshort((ULONG)(Port),(PUSHORT)(Buffer),(ULONG)(Length))


VOID NDIS_API
NdisRawWritePortBufferUlong(
    IN ULONG Port,
    IN PULONG Buffer,
    IN ULONG Length
    );
#define NdisWritePortBufferUlong(NdisAdapterHandle,Port,Buffer,Length) \
        NdisRawWritePortBufferUlong((ULONG)(Port),(PULONG)(Buffer),(ULONG)(Length))
        

//
// Write/Read Registers
//

/*++
VOID NDIS_API
NdisWriteRegisterUchar(
    IN PUCHAR Register,
    IN UCHAR Data
    )
--*/
#define NdisWriteRegisterUchar(Register,Data) \
    *(UCHAR *)(Register) = (UCHAR)(Data)

/*++
VOID NDIS_API
NdisWriteRegisterUshort(
    IN PUSHORT Register,
    IN USHORT Data
    )
--*/
#define NdisWriteRegisterUshort(Register,Data) \
    *(USHORT *)(Register) = (USHORT)(Data)

/*++
VOID NDIS_API
NdisWriteRegisterUlong(
    IN PULONG Register,
    IN ULONG Data
    )
--*/
#define NdisWriteRegisterUlong(Register,Data) \
    *(ULONG *)(Register) = (ULONG)(Data)

/*++
VOID NDIS_API
NdisReadRegisterUchar(
    IN PUCHAR Register,
    OUT PUCHAR Data
    )
--*/
#define NdisReadRegisterUchar(Register,Data) \
    *(UCHAR *)(Data) = (*((UCHAR *)(Register)))

/*++
VOID NDIS_API
NdisReadRegisterUshort(
    IN PUSHORT Register,
    OUT PUSHORT Data
    )
--*/
#define NdisReadRegisterUshort(Register,Data) \
    *(USHORT *)(Data) = (*((USHORT *)(Register)))

/*++
VOID NDIS_API
NdisReadRegisterUlong(
    IN PULONG Register,
    OUT PULONG Data
    )
--*/
#define NdisReadRegisterUlong(Register,Data) \
    *(ULONG *)(Data) = (*((ULONG *)(Register)))
    

//
// Write Ports
//

#define NdisWritePortUchar(NdisAdapterHandle,Port,Data)   	_outp(Port,Data)
#define NdisWritePortUshort(NdisAdapterHandle,Port,Data)  	_outpw(Port,Data)
#define NdisWritePortUlong(NdisAdapterHandle,Port,Data) 	_outpd(Port,Data)

#define NdisRawWritePortUchar(Port,Data)    _outp(Port,Data)
#define NdisRawWritePortUshort(Port,Data) 	_outpw(Port,Data)
#define NdisRawWritePortUlong(Port,Data)   	_outpd(Port,Data)

#define NdisImmediateWritePortUchar(WCC,Port,Data)      _outp(Port,Data)
#define NdisImmediateWritePortUshort(WCC,Port,Data)  	_outpw(Port,Data)
#define NdisImmediateWritePortUlong(WCC,Port,Data) 	    _outpd(Port,Data)

//
// Read Ports
//

#define NdisReadPortUchar(NdisAdapterHandle,Port,Data)  *(Data)=_inp(Port)
#define NdisReadPortUshort(NdisAdapterHandle,Port,Data) *(Data)=_inpw(Port)
#define NdisReadPortUlong(NdisAdapterHandle,Port,Data) 	*(Data)=_inpd(Port)

#define NdisRawReadPortUchar(Port,Data) 	*(Data)=_inp(Port)
#define NdisRawReadPortUshort(Port,Data)    *(Data)=_inpw(Port)
#define NdisRawReadPortUlong(Port,Data)    	*(Data)=_inpd(Port)

#define NdisImmediateReadPortUchar(WCC,Port,Data)   *(Data)=_inp(Port)
#define NdisImmediateReadPortUshort(WCC,Port,Data)  *(Data)=_inpw(Port)
#define NdisImmediateReadPortUlong(WCC,Port,Data) 	*(Data)=_inpd(Port)

//
// Physical Mapping
//
//
// The BufferPhysicalMapping functions are part of the NDIS 3 extensions.
// Since we dont need mapping registers, this is mapped to the old
// NdisGetBufferPhysicalArraySize service.
//
/*
VOID NDIS_API
NdisStartBufferPhysicalMapping(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG PhysicalMapRegister,
    IN BOOLEAN WirteToDevice,
    OUT PNDIS_PHYSICAL_ADDRESS_UNIT PhysicalAddressArray,
    OUT PULONG ArraySize
);
*/
#define NdisStartBufferPhysicalMapping(_a,_Buffer,_r,_w,_Array,_ArraySize) \
    NdisGetBufferPhysicalAddress(_Buffer,_Array,_ArraySize)


/*
VOID NDIS_API
NdisCompleteBufferPhysicalMapping(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG PhysicalMapRegister
);
*/
#define NdisCompleteBufferPhysicalMapping(_a,_b,_r)


//
// Shared memory
//

VOID NDIS_API
NdisAllocateSharedMemory(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    OUT PVOID *VirtualAddress,
    OUT PNDIS_PHYSICAL_ADDRESS PhysicalAddress
    );

/*
VOID NDIS_API
NdisUpdateSharedMemory(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG Length,
    IN PVOID VirtualAddress,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );
*/
#define NdisUpdateSharedMemory(a,b,c,d)

VOID NDIS_API
NdisFreeSharedMemory(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN ULONG Length,
    IN BOOLEAN Cached,
    IN PVOID VirtualAddress,
    IN NDIS_PHYSICAL_ADDRESS PhysicalAddress
    );


//
// DMA Operations
//


VOID NDIS_API
NdisAllocateDmaChannel(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE NdisDmaHandle,
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_DMA_DESCRIPTION DmaDescription,
    IN ULONG MaximumLength
    );

VOID NDIS_API
NdisFreeDmaChannel(
    IN PNDIS_HANDLE NdisDmaHandle
    );

VOID NDIS_API
NdisSetupDmaTransfer(
    OUT PNDIS_STATUS Status,
    IN PNDIS_HANDLE NdisDmaHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG Offset,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    );

VOID NDIS_API
NdisCompleteDmaTransfer(
    OUT PNDIS_STATUS Status,
    IN PNDIS_HANDLE NdisDmaHandle,
    IN PNDIS_BUFFER Buffer,
    IN ULONG Offset,
    IN ULONG Length,
    IN BOOLEAN WriteToDevice
    );

ULONG NDIS_API
NdisReadDmaCounter(
    IN PNDIS_HANDLE NdisDmaHandle
    );



//
// Requests used by Protocol Modules
//


VOID NDIS_API
NdisRegisterProtocol(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE NdisProtocolHandle,
    IN PNDIS_PROTOCOL_CHARACTERISTICS ProtocolCharacteristics,
    IN UINT CharacteristicsLength
    );

VOID NDIS_API
NdisDeregisterProtocol(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisProtocolHandle
    );

VOID NDIS_API
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

VOID NDIS_API
NdisCloseAdapter(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle
    );

#ifndef NDISMACROS
VOID NDIS_API
NdisSend(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle,
    IN PNDIS_PACKET Packet
    );
#else
#define NdisSend(Status,NdisBindingHandle,Packet) \
    { \
    *(Status) = \
        (((PWRAPPER_OPEN_BLOCK)(NdisBindingHandle))->SendHandler) ( \
        ((PWRAPPER_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle, \
        (Packet)); \
    }        

#endif    

#ifndef NDISMACROS
VOID NDIS_API
NdisTransferData(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle,
    IN NDIS_HANDLE MacReceiveContext,
    IN UINT ByteOffset,
    IN UINT BytesToTransfer,
    OUT PNDIS_PACKET Packet,
    OUT PUINT BytesTransferred
    );
#else
#define NdisTransferData(Status,NdisBindingHandle,MacReceiveContext,ByteOffset,BytesToTransfer,Packet,BytesTransferred) \
    { \
    *(Status) = \
        (((PWRAPPER_OPEN_BLOCK)(NdisBindingHandle))->TransferDataHandler) ( \
        ((PWRAPPER_OPEN_BLOCK)(NdisBindingHandle))->MacBindingHandle, \
        (MacReceiveContext), \
        (ByteOffset), \
        (BytesToTransfer), \
        (Packet), \
        (BytesTransferred)); \
    }       
    
#endif
VOID NDIS_API
NdisReset(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle
    );

VOID NDIS_API
NdisRequest(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisBindingHandle,
    IN NDIS_HANDLE RequestHandle
    );

VOID NDIS_API
NdisQueryGlobalStatistics(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisAdapterHandle,
    IN NDIS_HANDLE RequestHandle,
    IN NDIS_HANDLE RequestCompleteHandler
    );
    
//
// New to NDIS 3.1
//    
VOID NDIS_API
NdisOpenProtocolConfiguration(
    OUT PNDIS_STATUS    Status,
    OUT PNDIS_HANDLE    ConfigurationHandle,
    IN  PNDIS_STRING    ProtocolName
    );

VOID NDIS_API
NdisCompleteBindAdapter(
    IN  NDIS_HANDLE BindAdapterContext,
    IN  NDIS_STATUS Status,
    IN  NDIS_STATUS OpenErrorStatus
    );

VOID NDIS_API
NdisCompleteUnbindAdapter(
    IN  NDIS_HANDLE UnbindAdapterContext,
    IN  NDIS_STATUS Status
    );

//
// Requests Used by MAC Drivers
//

VOID NDIS_API
NdisInitializeWrapper(
    OUT PNDIS_HANDLE NdisWrapperHandle,
    IN PVOID SystemSpecific1,
    IN PVOID SystemSpecific2,
    IN PVOID SystemSpecific3
    );


VOID NDIS_API
NdisTerminateWrapper(
    IN NDIS_HANDLE NdisWrapperHandle,
    IN PVOID SystemSpecific
    );

VOID NDIS_API
NdisRegisterMac(
    OUT PNDIS_STATUS Status,
    OUT PNDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE NdisWrapperHandle,
    IN NDIS_HANDLE MacMacContext,
    IN PNDIS_MAC_CHARACTERISTICS MacCharacteristics,
    IN UINT CharacteristicsLength
    );

VOID NDIS_API
NdisDeregisterMac(
    OUT PNDIS_STATUS Status,
    IN NDIS_HANDLE NdisMacHandle
    );

NDIS_STATUS NDIS_API
NdisRegisterAdapter(
    OUT PNDIS_HANDLE NdisAdapterHandle,
    IN NDIS_HANDLE NdisMacHandle,
    IN NDIS_HANDLE MacAdapterContext,
    IN NDIS_HANDLE WrapperConfigurationContext,
    IN PNDIS_STRING AdapterName,
    IN PNDIS_ADAPTER_INFORMATION AdapterInformation
    );


NDIS_STATUS NDIS_API
NdisDeregisterAdapter(
    IN NDIS_HANDLE NdisAdapterHandle
    );


//
// moved to assembler 6.1, no need for this
//
// we have to do this because of the
// limitation on identifier name length in assembler

#define NdisRegisterAdapterShutdownHandler(a,b,c) \
        NdisRegAdaptShutdown(a,b,c)

#define NdisDeregisterAdapterShutdownHandler(a) \
        NdisDeregAdaptShutdown(a)


VOID NDIS_API
NdisRegAdaptShutdown(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PVOID ShutdownContext,
    IN ADAPTER_SHUTDOWN_HANDLER ShutdownHandler
    );

VOID NDIS_API
NdisDeregAdaptShutdown(
    IN NDIS_HANDLE NdisAdapterHandle
    );

VOID NDIS_API
NdisReleaseAdapterResources(
    IN NDIS_HANDLE NdisAdapterHandle
    );

VOID __cdecl
NdisWriteErrorLogEntry(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN NDIS_ERROR_CODE ErrorCode,
    IN ULONG NumberOfErrorValues,
    ...
    );

VOID NDIS_API
NdisCompleteOpenAdapter(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status,
    IN NDIS_STATUS OpenErrorStatus
    );


VOID NDIS_API
NdisCompleteCloseAdapter(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );

#ifndef NDISMACROS
VOID NDIS_API
NdisCompleteSend(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status
    );
#else
#define NdisCompleteSend(NdisBindingContext,Packet,Status) \
    (((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->SendCompleteHandler) ( \
        ((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
        (Packet), \
        (Status))
#endif    

#ifndef NDISMACROS
VOID NDIS_API
NdisCompleteTransferData(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_PACKET Packet,
    IN NDIS_STATUS Status,
    IN UINT BytesTransferred
    );
#else
#define NdisCompleteTransferData(NdisBindingContext,Packet,Status,BytesTransferred) \
    (((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->TransferDataCompleteHandler) ( \
        ((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
        (Packet), \
        (Status), \
        (BytesTransferred))
#endif    

VOID NDIS_API
NdisCompleteReset(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS Status
    );

VOID NDIS_API
NdisCompleteRequest(
    IN NDIS_HANDLE NdisBindingContext,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    );

#ifndef NDISMACROS
VOID NDIS_API
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
#define NdisIndicateReceive(Status,NdisBindingContext,MacReceiveContext,HeaderBuffer,HeaderBufferSize,LookaheadBuffer,LookaheadBufferSize,PacketSize) \
    { \
    *(Status) = \
        (((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->ReceiveHandler) ( \
            ((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext, \
            (MacReceiveContext), \
            (HeaderBuffer), \
            (HeaderBufferSize), \
            (LookaheadBuffer), \
            (LookaheadBufferSize), \
            (PacketSize)); \
    }
#endif    

#ifndef NDISMACROS
VOID NDIS_API
NdisIndicateReceiveComplete(
    IN NDIS_HANDLE NdisBindingContext
    );
#else
#define NdisIndicateReceiveComplete(NdisBindingContext) \
  (((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->ReceiveCompleteHandler) ( \
        ((PWRAPPER_OPEN_BLOCK)(NdisBindingContext))->ProtocolBindingContext)
#endif

VOID NDIS_API
NdisIndicateStatus(
    IN NDIS_HANDLE NdisBindingContext,
    IN NDIS_STATUS GeneralStatus,
    IN PVOID StatusBuffer,
    IN UINT StatusBufferLength
    );

VOID NDIS_API
NdisIndicateStatusComplete(
    IN NDIS_HANDLE NdisBindingContext
    );


VOID NDIS_API
NdisCompleteQueryStatistics(
    IN NDIS_HANDLE NdisAdapterHandle,
    IN PNDIS_REQUEST NdisRequest,
    IN NDIS_STATUS Status
    );


//
// Linked List Manipulation Functions
//

//
//  Doubly linked list structure.  Can be used as either a list head, or
//  as link words. - from NTDEF.H
//

/* INC */
typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY;
/* NOINC */
typedef LIST_ENTRY *PLIST_ENTRY;

//
// Calculate the address of the base of the structure given its type, and an
// address of a field within the structure.
//

#define CONTAINING_RECORD(address, type, field) ((type *)( \
                          (PCHAR)(address) - \
                          (PCHAR)(&((type *)0)->field)))

//
//  Doubly-linked list manipulation routines.  Implemented as macros
//  but logically these are procedures.
//

//
//  VOID
//  InitializeListHead(
//      PLIST_ENTRY ListHead
//      );
//

#define InitializeListHead(ListHead) (\
    (ListHead)->Flink = (ListHead)->Blink = (ListHead) )

//
//  BOOLEAN
//  IsListEmpty(
//      PLIST_ENTRY ListHead
//      );
//

#define IsListEmpty(ListHead) (\
    ( ((ListHead)->Flink == (ListHead)) ? TRUE : FALSE ) )

//
//  PLIST_ENTRY
//  RemoveHeadList(
//      PLIST_ENTRY ListHead
//      );
//

#define RemoveHeadList(ListHead) \
    (ListHead)->Flink;\
    {\
    PLIST_ENTRY FirstEntry;\
    FirstEntry = (ListHead)->Flink;\
    FirstEntry->Flink->Blink = (ListHead);\
    (ListHead)->Flink = FirstEntry->Flink;\
    }

//
//  VOID
//  RemoveEntryList(
//      PLIST_ENTRY Entry
//      );
//

#define RemoveEntryList(Entry) {\
    PLIST_ENTRY _EX_Entry;\
    _EX_Entry = (Entry);\
    _EX_Entry->Blink->Flink = _EX_Entry->Flink;\
    _EX_Entry->Flink->Blink = _EX_Entry->Blink;\
    }

//
//  VOID
//  InsertTailList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertTailList(ListHead,Entry) \
    (Entry)->Flink = (ListHead);\
    (Entry)->Blink = (ListHead)->Blink;\
    (ListHead)->Blink->Flink = (Entry);\
    (ListHead)->Blink = (Entry)

//
//  VOID
//  InsertHeadList(
//      PLIST_ENTRY ListHead,
//      PLIST_ENTRY Entry
//      );
//

#define InsertHeadList(ListHead,Entry) \
    (Entry)->Flink = (ListHead)->Flink;\
    (Entry)->Blink = (ListHead);\
    (ListHead)->Flink->Blink = (Entry);\
    (ListHead)->Flink = (Entry)



//
//
//  PSINGLE_LIST_ENTRY
//  PopEntryList(
//      PSINGLE_LIST_ENTRY ListHead
//      );
//

#define PopEntryList(ListHead) \
    (ListHead)->Next;\
    {\
    PSINGLE_LIST_ENTRY FirstEntry;\
    FirstEntry = (ListHead)->Next;\
    (ListHead)->Next = FirstEntry;\
    }


//
//  VOID
//  PushEntryList(
//      PSINGLE_LIST_ENTRY ListHead,
//      PSINGLE_LIST_ENTRY Entry
//      );
//

#define PushEntryList(ListHead,Entry) \
    (Entry)->Next = (ListHead)->Next; \
    (ListHead)->Next = (Entry)



//
// OS Specific Services, Macros, etc.
//

//
// Entry Points for Initialization phases - user defined
//
// Note: Hooking of interrupts and allocation of physical memory must take
//       place during the Sys_Critical_Init phase.
//       -Macs should use the Device_Init phase to do their non critical init.
//       -Protocols should use the Init_Done phase to do their non critical init.
//
// Return Values: must return NDIS_STATUS_SUCCESS or NDIS_STATUS_FAILURE
//                NDIS_STATUS_FAILURE will abort driver load!
//
// Under Dos environment both arguments are ignored
//

NTSTATUS
NDIS_API
DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

//
// String Manipulation Functions
//

#define NDIS_STRING_CONST(x) { sizeof(x),sizeof(x),x}


#define HexToBinary(_hex) ((_hex) > 57 ? ((_hex) > 90 ? (_hex)-87 : (_hex)-55): (_hex)-48)

#define ishexdigit(_digit) ( (_digit <= '9' && _digit >= '0') \
         || ( _digit <= 'f' && _digit >= 'a') \
         || ( _digit <= 'F' && _digit >= 'A'))

#define NdisNetworkAddressStringToBinary(Status, AddressString, NetworkAddress) \
{\
    PUCHAR _D = NetworkAddress;    \
    PUCHAR _S = (AddressString)->Buffer;   \
    int _i; \
    for ( _i = 0; _i < 6; _i++)     \
    _D[_i] = (HexToBinary(_S[_i<<1]) << 4) + HexToBinary(_S[(_i<<1)+1]);    \
    *Status = NDIS_STATUS_SUCCESS;  \
}

#define NdisGetCacheFillSize() 0x1000

#define NdisCreateLookaheadBufferFromSharedMemory(_S, _L, _B) \
  ((*(_B)) = (_S))

#define NdisDestroyLookaheadBufferFromSharedMemory(_B)

#define UNREFERENCED_PARAMETER(x) x

NDIS_STATUS __cdecl
WrapperStartNet(
    PVOID Context
    );

typedef enum _NET_COMPONENT_TYPE{
    ComponentTypeAdapter,
    ComponentTypeMac,
    ComponentTypeProtocol
    } NET_COMPONENT_TYPE, * PNET_COMPONENT_TYPE;

NDIS_STATUS __cdecl
WrapperGetComponentList(
    IN OUT PVOID *pList,
    IN NET_COMPONENT_TYPE NetComponentType
    );

typedef struct _PCI_SLOT_NUMBER {
    union {
        struct {
            ULONG   DeviceNumber:5;
            ULONG   FunctionNumber:3;
            ULONG   Reserved:24;
        } bits;
        ULONG   AsULONG;
    } u;
} PCI_SLOT_NUMBER, *PPCI_SLOT_NUMBER;

#define PCI_TYPE0_ADDRESSES             6
#define PCI_TYPE1_ADDRESSES             2

typedef struct _PCI_COMMON_CONFIG {
    USHORT  VendorID;                   // (ro)
    USHORT  DeviceID;                   // (ro)
    USHORT  Command;                    // Device control
    USHORT  Status;
    UCHAR   RevisionID;                 // (ro)
    UCHAR   ProgIf;                     // (ro)
    UCHAR   SubClass;                   // (ro)
    UCHAR   BaseClass;                  // (ro)
    UCHAR   CacheLineSize;              // (ro+)
    UCHAR   LatencyTimer;               // (ro+)
    UCHAR   HeaderType;                 // (ro)
    UCHAR   BIST;                       // Built in self test

    union {
        struct _PCI_HEADER_TYPE_0 {
            ULONG   BaseAddresses[PCI_TYPE0_ADDRESSES];
            ULONG   Reserved1[2];
            ULONG   ROMBaseAddress;
            ULONG   Reserved2[2];

            UCHAR   InterruptLine;      //
            UCHAR   InterruptPin;       // (ro)
            UCHAR   MinimumGrant;       // (ro)
            UCHAR   MaximumLatency;     // (ro)
        } type0;


    } u;

    UCHAR   DeviceSpecific[192];

} PCI_COMMON_CONFIG, *PPCI_COMMON_CONFIG;


#define PCI_COMMON_HDR_LENGTH (FIELD_OFFSET (PCI_COMMON_CONFIG, DeviceSpecific))

#define PCI_MAX_DEVICES                     32
#define PCI_MAX_FUNCTION                    8

#define PCI_INVALID_VENDORID                0xFFFF

//
// Bit encodings for  PCI_COMMON_CONFIG.HeaderType
//

#define PCI_MULTIFUNCTION                   0x80
#define PCI_DEVICE_TYPE                     0x00
#define PCI_BRIDGE_TYPE                     0x01

//
// Bit encodings for PCI_COMMON_CONFIG.Command
//

#define PCI_ENABLE_IO_SPACE                 0x0001
#define PCI_ENABLE_MEMORY_SPACE             0x0002
#define PCI_ENABLE_BUS_MASTER               0x0004
#define PCI_ENABLE_SPECIAL_CYCLES           0x0008
#define PCI_ENABLE_WRITE_AND_INVALIDATE     0x0010
#define PCI_ENABLE_VGA_COMPATIBLE_PALETTE   0x0020
#define PCI_ENABLE_PARITY                   0x0040  // (ro+)
#define PCI_ENABLE_WAIT_CYCLE               0x0080  // (ro+)
#define PCI_ENABLE_SERR                     0x0100  // (ro+)
#define PCI_ENABLE_FAST_BACK_TO_BACK        0x0200  // (ro)

//
// Bit encodings for PCI_COMMON_CONFIG.Status
//

#define PCI_STATUS_FAST_BACK_TO_BACK        0x0080  // (ro)
#define PCI_STATUS_DATA_PARITY_DETECTED     0x0100
#define PCI_STATUS_DEVSEL                   0x0600  // 2 bits wide
#define PCI_STATUS_SIGNALED_TARGET_ABORT    0x0800
#define PCI_STATUS_RECEIVED_TARGET_ABORT    0x1000
#define PCI_STATUS_RECEIVED_MASTER_ABORT    0x2000
#define PCI_STATUS_SIGNALED_SYSTEM_ERROR    0x4000
#define PCI_STATUS_DETECTED_PARITY_ERROR    0x8000


//
// Bit encodes for PCI_COMMON_CONFIG.u.type0.BaseAddresses
//

#define PCI_ADDRESS_IO_SPACE                0x00000001  // (ro)
#define PCI_ADDRESS_MEMORY_TYPE_MASK        0x00000006  // (ro)
#define PCI_ADDRESS_MEMORY_PREFETCHABLE     0x00000008  // (ro)

#define PCI_TYPE_32BIT      0
#define PCI_TYPE_20BIT      2
#define PCI_TYPE_64BIT      4

//
// Bit encodes for PCI_COMMON_CONFIG.u.type0.ROMBaseAddresses
//

#define PCI_ROMADDRESS_ENABLED              0x00000001


//
// Reference notes for PCI configuration fields:
//
// ro   these field are read only.  changes to these fields are ignored
//
// ro+  these field are intended to be read only and should be initialized
//      by the system to their proper values.  However, driver may change
//      these settings.
//
// ---
//
//      All resources comsumed by a PCI device start as unitialized
//      under NT.  An uninitialized memory or I/O base address can be
//      determined by checking it's corrisponding enabled bit in the
//      PCI_COMMON_CONFIG.Command value.  An InterruptLine is unitialized
//      if it contains the value of -1.
//
             

/* INC */
/* ASM

Begin_Service_Table Ndis

    Ndis_Service NdisGetVersion,LOCAL
    
    Ndis_Service NdisAllocateSpinLock,LOCAL
    Ndis_Service NdisFreeSpinLock,LOCAL
    Ndis_Service NdisAcquireSpinLock,LOCAL
    Ndis_Service NdisReleaseSpinLock,LOCAL
    
    Ndis_Service NdisOpenConfiguration,LOCAL
    Ndis_Service NdisReadConfiguration,LOCAL
    Ndis_Service NdisCloseConfiguration,LOCAL
    Ndis_Service NdisReadEisaSlotInformation, LOCAL
    Ndis_Service NdisReadMcaPosInformation,LOCAL

    Ndis_Service NdisAllocateMemory,LOCAL
    Ndis_Service NdisFreeMemory,LOCAL
    Ndis_Service NdisSetTimer,LOCAL
    Ndis_Service NdisCancelTimer,LOCAL
    Ndis_Service NdisStallExecution,LOCAL
    Ndis_Service NdisInitializeInterrupt,LOCAL
    Ndis_Service NdisRemoveInterrupt,LOCAL
    Ndis_Service NdisSynchronizeWithInterrupt,LOCAL
    Ndis_Service NdisOpenFile,LOCAL
    Ndis_Service NdisMapFile,LOCAL
    Ndis_Service NdisUnmapFile,LOCAL
    Ndis_Service NdisCloseFile,LOCAL

    Ndis_Service NdisAllocatePacketPool,LOCAL
    Ndis_Service NdisFreePacketPool,LOCAL
    Ndis_Service NdisAllocatePacket,LOCAL
    Ndis_Service NdisReinitializePacket,LOCAL
    Ndis_Service NdisFreePacket,LOCAL
    Ndis_Service NdisQueryPacket,LOCAL

    Ndis_Service NdisAllocateBufferPool,LOCAL
    Ndis_Service NdisFreeBufferPool,LOCAL
    Ndis_Service NdisAllocateBuffer,LOCAL
    Ndis_Service NdisCopyBuffer,LOCAL
    Ndis_Service NdisFreeBuffer,LOCAL
    Ndis_Service NdisQueryBuffer,LOCAL
    Ndis_Service NdisGetBufferPhysicalAddress,LOCAL
    Ndis_Service NdisChainBufferAtFront,LOCAL
    Ndis_Service NdisChainBufferAtBack,LOCAL
    Ndis_Service NdisUnchainBufferAtFront,LOCAL
    Ndis_Service NdisUnchainBufferAtBack,LOCAL
    Ndis_Service NdisGetNextBuffer,LOCAL
    Ndis_Service NdisCopyFromPacketToPacket,LOCAL

    Ndis_Service NdisRegisterProtocol,LOCAL
    Ndis_Service NdisDeregisterProtocol,LOCAL
    Ndis_Service NdisOpenAdapter,LOCAL
    Ndis_Service NdisCloseAdapter,LOCAL
    Ndis_Service NdisSend,LOCAL
    Ndis_Service NdisTransferData,LOCAL
    Ndis_Service NdisReset,LOCAL
    Ndis_Service NdisRequest,LOCAL

    Ndis_Service NdisInitializeWrapper,LOCAL
    Ndis_Service NdisTerminateWrapper,LOCAL
    Ndis_Service NdisRegisterMac,LOCAL
    Ndis_Service NdisDeregisterMac,LOCAL
    Ndis_Service NdisRegisterAdapter,LOCAL
    Ndis_Service NdisDeregisterAdapter,LOCAL
    Ndis_Service NdisCompleteOpenAdapter,LOCAL
    Ndis_Service NdisCompleteCloseAdapter,LOCAL
    Ndis_Service NdisCompleteSend,LOCAL
    Ndis_Service NdisCompleteTransferData,LOCAL
    Ndis_Service NdisCompleteReset,LOCAL
    Ndis_Service NdisCompleteRequest,LOCAL
    Ndis_Service NdisIndicateReceive,LOCAL
    Ndis_Service NdisIndicateReceiveComplete,LOCAL
    Ndis_Service NdisIndicateStatus,LOCAL
    Ndis_Service NdisIndicateStatusComplete,LOCAL
    Ndis_Service NdisCompleteQueryStatistics,LOCAL

    Ndis_Service NdisEqualString,LOCAL
    Ndis_Service NdisRegAdaptShutdown,LOCAL
    Ndis_Service NdisReadNetworkAddress,LOCAL

    Ndis_Service NdisWriteErrorLogEntry,LOCAL

    Ndis_Service NdisMapIoSpace,LOCAL
    Ndis_Service NdisDeregAdaptShutdown,LOCAL

    Ndis_Service NdisAllocateSharedMemory,LOCAL
    Ndis_Service NdisFreeSharedMemory, LOCAL

    Ndis_Service NdisAllocateDmaChannel, LOCAL
    Ndis_Service NdisSetupDmaTransfer, LOCAL
    Ndis_Service NdisCompleteDmaTransfer, LOCAL
    Ndis_Service NdisReadDmaCounter, LOCAL
    Ndis_Service NdisFreeDmaChannel, LOCAL
    Ndis_Service NdisReleaseAdapterResources, LOCAL
    Ndis_Service NdisQueryGlobalStatistics, LOCAL

    Ndis_Service NdisOpenProtocolConfiguration, LOCAL
    Ndis_Service NdisCompleteBindAdapter, LOCAL
    Ndis_Service NdisCompleteUnbindAdapter, LOCAL
    Ndis_Service WrapperStartNet, LOCAL
    Ndis_Service WrapperGetComponentList, LOCAL
    Ndis_Service WrapperQueryAdapterResources, Local
    Ndis_Service WrapperDelayBinding, Local
    Ndis_Service WrapperResumeBinding, Local
    Ndis_Service WrapperRemoveChildren, Local
    Ndis_Service NdisImmediateReadPciSlotInformation, Local
    Ndis_Service NdisImmediateWritePciSlotInformation, Local
    Ndis_Service NdisReadPciSlotInformation, Local
    Ndis_Service NdisWritePciSlotInformation, Local
    Ndis_Service NdisPciAssignResources, Local
    Ndis_Service NdisQueryBufferOffset, Local
End_Service_Table Ndis

*/

#endif  // _NDIS_

/* NOINC */
