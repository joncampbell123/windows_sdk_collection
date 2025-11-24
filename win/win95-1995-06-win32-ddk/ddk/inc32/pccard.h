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

/******************************************************************************
 *
 *  Title:  PCCARD.H - Card Services function definitions.
 *
 */


/******************************************************************************
 *
 *  PCCARD VxD Declarations.
 *
 *****************************************************************************/

#define PCCARD_DEVICE_ID    0x097C


// PCCARD Services

/*XLATOFF*/
#define PCCARD_Service	    Declare_Service
/*XLATON*/

#ifndef IS_16

/*MACROS*/
Begin_Service_Table(PCCARD, VxD)

PCCARD_Service	    (PCCARD_Get_Version, LOCAL)
PCCARD_Service	    (PCCARD_Card_Services, LOCAL)

End_Service_Table(PCCARD, VxD)
/*ENDMACROS*/

#endif


/******************************************************************************
 *
 *  Card Services Function Codes.
 *
 *****************************************************************************/

#define F_CLOSE_MEMORY			0x00
#define F_COPY_MEMORY			0x01
#define F_DEREGISTER_CLIENT		0x02
#define F_GET_CLIENT_INFO		0x03
#define F_GET_CONFIGURATION_INFO	0x04
#define F_GET_FIRST_PARTITION		0x05
#define F_GET_FIRST_REGION		0x06
#define F_GET_FIRST_TUPLE		0x07
#define F_GET_NEXT_PARTITION		0x08
#define F_GET_NEXT_REGION		0x09
#define F_GET_NEXT_TUPLE		0x0A
#define F_GET_CARD_SERVICES_INFO	0x0B
#define F_CS_GET_STATUS 		0x0C
#define F_GET_TUPLE_DATA		0x0D
#define F_GET_FIRST_CLIENT		0x0E
#define F_REGISTER_ERASE_QUEUE		0x0F
#define F_REGISTER_CLIENT		0x10
#define F_RESET_CARD			0x11
#define F_MAP_LOG_SOCKET		0x12
#define F_MAP_LOG_WINDOW		0x13
#define F_MAP_MEM_PAGE			0x14
#define F_MAP_PHY_SOCKET		0x15
#define F_MAP_PHY_WINDOW		0x16
#define F_MODIFY_WINDOW 		0x17
#define F_OPEN_MEMORY			0x18
#define F_READ_MEMORY			0x19
#define F_REGISTER_MTD			0x1A
#define F_RELEASE_IO			0x1B
#define F_RELEASE_IRQ			0x1C
#define F_RELEASE_WINDOW		0x1D
#define F_RELEASE_CONFIGURATION 	0x1E
#define F_REQUEST_IO			0x1F
#define F_REQUEST_IRQ			0x20
#define F_REQUEST_WINDOW		0x21
#define F_REQUEST_SOCKET_MASK		0x22
#define F_RETURN_SS_ENTRY		0x23
#define F_WRITE_MEMORY			0x24
#define F_DEREGISTER_ERASE_QUEUE	0x25
#define F_CHECK_ERASE_QUEUE		0x26
#define F_MODIFY_CONFIGURATION		0x27
#define F_REGISTER_TIMER		0x28
#define F_SET_REGION			0x29
#define F_GET_NEXT_CLIENT		0x2A
#define F_VALIDATE_CIS			0x2B
#define F_REQUEST_EXCLUSIVE		0x2C
#define F_RELEASE_EXCLUSIVE		0x2D
#define F_GET_EVENT_MASK		0x2E
#define F_RELEASE_SOCKET_MASK		0x2F
#define F_REQUEST_CONFIGURATION 	0x30
#define F_SET_EVENT_MASK		0x31
#define F_ADD_SOCKET_SERVICES		0x32
#define F_REPLACE_SOCKET_SERVICES	0x33
#define F_CS_VENDOR_SPECIFIC		0x34
#define F_ADJUST_RESOURCE_INFO		0x35
#define F_ACCESS_CONFIGURATION_REGISTER 0x36
#define MAX_CS_FUNCTIONS		0x37


/******************************************************************************
 *
 *  Card Services Event Codes.
 *
 *****************************************************************************/

#define EV_BATTERY_DEAD 		0x01
#define EV_BATTERY_LOW			0x02
#define EV_CARD_LOCK			0x03
#define EV_CARD_READY			0x04
#define EV_CARD_REMOVAL 		0x05
#define EV_CARD_UNLOCK			0x06
#define EV_EJECTION_COMPLETE		0x07
#define EV_EJECTION_REQUEST		0x08
#define EV_INSERTION_COMPLETE		0x09
#define EV_INSERTION_REQUEST		0x0A
#define EV_PM_RESUME			0x0B
#define EV_PM_SUSPEND			0x0C
#define EV_EXCLUSIVE_COMPLETE		0x0D
#define EV_EXCLUSIVE_REQUEST		0x0E
#define EV_RESET_PHYSICAL		0x0F
#define EV_RESET_REQUEST		0x10
#define EV_CARD_RESET			0x11
#define EV_MTD_REQUEST			0x12
#define EV_CLIENT_INFO			0x14
#define EV_TIMER_EXPIRED		0x15
#define EV_SS_UPDATED			0x16
#define EV_WRITE_PROTECT		0x17
#define EV_CARD_INSERTION		0x40
#define EV_RESET_COMPLETE		0x80
#define EV_ERASE_COMPLETE		0x81
#define EV_REGISTRATION_COMPLETE	0x82


/******************************************************************************
 *
 *  Card Services and Socket Services Return Codes.
 *
 *****************************************************************************/

#define R_SUCCESS			0x00
#define R_BAD_ADAPTER			0x01
#define R_BAD_ATTRIBUTE 		0x02
#define R_BAD_BASE			0x03
#define R_BAD_EDC			0x04
#define R_RESERVED_05			0x05
#define R_BAD_IRQ			0x06
#define R_BAD_OFFSET			0x07
#define R_BAD_PAGE			0x08
#define R_READ_FAILURE			0x09
#define R_BAD_SIZE			0x0A
#define R_BAD_SOCKET			0x0B
#define R_RESERVED_0C			0x0C
#define R_BAD_TYPE			0x0D
#define R_BAD_VCC			0x0E
#define R_BAD_VPP			0x0F
#define R_RESERVED_10			0x10
#define R_BAD_WINDOW			0x11
#define R_WRITE_FAILURE 		0x12
#define R_RESERVED_13			0x13
#define R_NO_CARD			0x14
#define R_UNSUPPORTED_FUNCTION		0x15
#define R_UNSUPPORTED_MODE		0x16
#define R_BAD_SPEED			0x17
#define R_BUSY				0x18
#define R_GENERAL_FAILURE		0x19
#define R_WRITE_PROTECTED		0x1A
#define R_BAD_ARG_LENGTH		0x1B
#define R_BAD_ARGS			0x1C
#define R_CONFIGURATION_LOCKED		0x1D
#define R_IN_USE			0x1E
#define R_NO_MORE_ITEMS 		0x1F
#define R_OUT_OF_RESOURCE		0x20
#define R_BAD_HANDLE			0x21


/******************************************************************************
 *
 *  Card Services Interface Structures.
 *  (Must be packed to one byte to guarantee conformance to the
 *   card services specification).
 *
 *****************************************************************************/

/* XLATOFF */
#pragma pack(1)
/* XLATON */


typedef WORD LOG_SOCKET;
typedef BYTE PHY_ADAPTER;
typedef BYTE PHY_SOCKET;
typedef BYTE PHY_WINDOW;


//-----------------------------------------------------------------------------
// Access Configuration Register Structure.

typedef struct AccessConfigurationRegister_s {
    LOG_SOCKET	Socket;
    BYTE	Action;
    BYTE	RegOffset;
    BYTE	Value;
} ACCESS_CONFIG_REG_PKT;

// Access Configuration Register Action definitions.

#define ACRA_READ   0
#define ACRA_WRITE  1


//-----------------------------------------------------------------------------
// Add Socket Services Structure.

typedef struct AddSSData_s {
//    DEVNODE devnode;
    DWORD devnode;
} ADD_SS_DATA;

typedef struct AddSocketServices_s {
    WORD	Attributes;
    ADD_SS_DATA *DataPointer;
} ADD_SS_PKT;

// Add Socket Services Attributes definitions.

#define SSA_REAL_MODE		0
#define SSA_1616_PROTECT_MODE	1
#define SSA_1632_PROTECT_MODE	2
#define SSA_FLAT_PROTECT_MODE	3


//-----------------------------------------------------------------------------
//// Adjust Resource Info Structure.
//
//typedef struct AdjustResourceInfo_s {
//    BYTE    Action;
//    BYTE    Resource;
//} ADJUST_RES_INFO_PKT;
//
//// Adjust Resource Info Action definitions.
//
//#define ARIA_REMOVE_MANAGED_RESOURCE	  0
//#define ARIA_ADD_MANAGED_RESOURCE	  1
//#define ARIA_GET_FIRST_MANAGED_RESOURCE 2
//#define ARIA_GET_NEXT_MANAGED_RESOURCE  3

// Adjust Resource Info Resource definitions.

#define ARIR_MEM_RANGE	0
#define ARIR_IO_RANGE	1
#define ARIR_IRQ	2


//-----------------------------------------------------------------------------
// Copy Memory Structure.

typedef struct CopyMemory_s {
    DWORD   SourceOffset;
    DWORD   DestOffset;
    DWORD   Count;
    WORD    Attributes;
} COPY_MEM_PKT;

// Copy Memory Attributes bit definitions.

#define MEMA_DISABLE_ERASE  0x0004
#define MEMA_VERIFY	    0x0008


//-----------------------------------------------------------------------------
// Get Card Services Info Structure.
//
//  NOTE: - The get cs info packet is appended with a variable length
//	    ASCIIZ vendor string buffer area.

typedef struct GetCardServicesInfo_s {
    WORD    InfoLen;
    BYTE    Signature[2];
    WORD    Count;
    WORD    Revision;
    WORD    CSLevel;
    WORD    VStrOff;
    WORD    VStrLen;
//  BYTE    VendorString[];
} GET_CS_INFO_PKT;


//-----------------------------------------------------------------------------
// Get Client Info Structure.

typedef struct GetClientInfo_s {
    WORD    MaxLen;
    WORD    InfoLen;
    WORD    Attributes;
    WORD    Revision;
    WORD    CSLevel;
    WORD    RevDate;
    WORD    NameOff;
    WORD    NameLen;
    WORD    VStringOff;
    WORD    VStringLen;
} GET_CLT_INFO_PKT;

// Client Info Attributes bit definitions.

#define CLTA_MEM_DRIVER         0x0001
#define CLTA_MTD_DRIVER         0x0002
#define CLTA_IO_DRIVER          0x0004
#define CLTA_INSERT_SHARED	0x0008
#define CLTA_INSERT_EXCLUSIVE	0x0010
#define CLTA_INFO_SUBFUNCTION   0xFF00
#define CLTA_DRIVER_MASK	(CLTA_MEM_DRIVER |		       \
				 CLTA_MTD_DRIVER |                     \
				 CLTA_IO_DRIVER)


//-----------------------------------------------------------------------------
// Get Configuration Info Structure.

typedef struct IOParms_s {
    WORD    BasePort;
    BYTE    NumPorts;
    BYTE    Attributes;
} IO_PARMS;

// I/O Params Attributes bit definitions.

#define IOPA_SHARED		0x01
#define IOPA_FIRST_SHARED	0x02
#define IOPA_FORCE_ALIAS	0x04
#define IOPA_16BIT		0x08

#define MAX_IO_WDWS_PER_SKT	2

typedef struct GetConfigurationInfo_s {
    LOG_SOCKET		Socket;
    WORD		Attributes;
    BYTE		Vcc;
    BYTE		Vpp1;
    BYTE		Vpp2;
    BYTE		IntType;
    DWORD		ConfigBase;
    BYTE		Status;
    BYTE		Pin;
    BYTE		Copy;
    BYTE		ConfigIndex;	    // Option;
    BYTE		Present;
    BYTE		FirstDevType;
    BYTE		FuncCode;
    BYTE		SysInitMask;
    WORD		ManufCode;
    WORD		ManufInfo;
    BYTE		CardValues;
    BYTE		AssignedIRQ;
    WORD		IRQAttributes;
    struct IOParms_s	IOParms[MAX_IO_WDWS_PER_SKT];
    BYTE		IOAddrLines;
} GET_CONFIG_INFO_PKT;

// Get Configuration Attribute bit definitions.

#define CFGA_EXCLUSIVELY_USED	0x0001
#define CFGA_ENABLE_IRQ 	0x0002
#define CFGA_VALID_CLIENT	0x0100

// Get Configuration CardValue bit definitions.

#define CNFG_OPTION_VALID	0x01
#define CNFG_STATUS_VALID	0x02
#define CNFG_PIN_VALID		0x04
#define CNFG_COPY_VALID 	0x08
#define CNFG_PRESENT_MASK	0x0F


//-----------------------------------------------------------------------------
//  Get/Set Event Mask Structure.

typedef struct EventMask_s {
    WORD	Attributes;
    WORD	EventMask;
    LOG_SOCKET	Socket;
} EVT_MASK_PKT;

// Event Mask Attribute bit definitions.

#define EVTA_SOCKET_ONLY        0x0001

// Event Mask EventMask bit definitions.

#define EVTM_WRITE_PROTECT	0x0001
#define EVTM_CARD_LOCK_CHG	0x0002
#define EVTM_EJECT_REQUEST	0x0004
#define EVTM_INSERT_REQUEST	0x0008
#define EVTM_BATTERY_DEAD	0x0010
#define EVTM_BATTERY_LOW	0x0020
#define EVTM_READY_CHG          0x0040
#define EVTM_CARD_DETECT_CHG	0x0080
#define EVTM_PWR_MGMT_CHG       0x0100
#define EVTM_RESET_EVENTS	0x0200
#define EVTM_SS_UPDATED         0x0400


//-----------------------------------------------------------------------------
// Get First/Next Client Structure.

typedef struct GetClient_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
} GET_CLT_PKT;

// Get Client Attributes bit definitions.

#define CLTA_SOCKET_ONLY	0x0001


//-----------------------------------------------------------------------------
// Get First/Next Partition Structure.

typedef struct GetPartition_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	TupleMask;
    BYTE	AccessSpeed;
    WORD	Flags;
    DWORD	LinkOffset;
    DWORD	CISOffset;
    DWORD	CardOffset;
    DWORD	PartSize;
    DWORD	EffBlockSize;
    WORD	PartMultiple;
    WORD	JedecID;
    WORD	PartType;
} GET_PARTITION_PKT;

// Get Partition Attributes bit definitions.

#define MEMA_ATTRIB_MEM 	0x0001
#define MEMA_VIRTUAL		0x0100
#define MEMA_WRITE_ERASE_MASK	0x0600
#define MEMA_WE_WRITE		0x0200
#define MEMA_WE_WRITE_ERASE	0x0400
#define MEMA_WE_DISABLE_ERASE	0x0600
#define MEMA_WRITE_VERIFY	0x0800
#define MEMA_ERASE_SUPPORTED	0x1000

// Get Partition TupleMask bit definitions.

#define MEMM_ACCESS_SPEED	0x0001
#define MEMM_CARD_OFFSET	0x0002
#define MEMM_PART_SIZE		0x0004
#define MEMM_EFF_BLK_SIZE	0x0008
#define MEMM_PART_MULTIPLE	0x0010
#define MEMM_JEDEC_ID		0x0020
#define MEMM_PART_TYPE		0x0040

// Get Partition AccessSpeed bit definitions.

#define ASP_CODE_EXP		0x07	// Device speed code or exponent mask
#define ASP_MANTISSA		0x71	// Speed mantissa mask
#define ASP_WAIT		0x80

// Device Speed Code Definitions.

#define ASPC_250NS		0x01
#define ASPC_200NS		0x02
#define ASPC_150NS		0x03
#define ASPC_100NS		0x04

// Get Partition PartType definitions.

#define PRTT_NO_PART		0x0000
#define PRTT_DOS_PART		0x0001
#define PRTT_FFS1_PART		0x0002
#define PRTT_FFS2_PART		0x0003
#define PRTT_XIP_PART		0x0004
#define PRTT_UNKNOWN_PART	0x7FFF


//-----------------------------------------------------------------------------
// Get First/Next Region Structure.

typedef struct GetRegion_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	TupleMask;
    BYTE	AccessSpeed;
    WORD	Flags;
    DWORD	LinkOffset;
    DWORD	CISOffset;
    DWORD	CardOffset;
    DWORD	RegionSize;
    DWORD	EffBlockSize;
    WORD	PartMultiple;
    WORD	JedecID;
} GET_REGION_PKT;

// Get Region Attributes bit definitions.
//  These are the same as the Get Partition Attributes bit definitions.

// Get Region TupleMask bit definitions.
//  These are the same as the Get Partition TupleMask bit definitions.

// Get Region AccessSpeed bit definitions.
//  These are the same as the Get Partition AccessSpeed bit definitions.


//-----------------------------------------------------------------------------
// Get First/Next Tuple Structure.

typedef struct GetTuple_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	DesiredTuple;
    BYTE	Reserved;
    WORD	Flags;
    DWORD	LinkOffset;
    DWORD	CISOffset;
    BYTE	TupleCode;
    BYTE	TupleLink;
} GET_TUPLE_PKT;

// Get Tuple Attributes bit definitions.

#define TPLA_RET_LINKS	0x0001

// Get Tuple DesiredTuple Code definitions.

#define TPLC_ANY_TUPLE	0xFF


//-----------------------------------------------------------------------------
// Get Status Structure.

typedef struct GetStatus_s {
    LOG_SOCKET	Socket;
    WORD	CardState;
    WORD	SocketState;
} GET_STATUS_PKT;

// Get Status CardState bit definitions.

#define CRDS_WRITE_PROTECTED	0x0001
#define CRDS_CARD_LOCKED	0x0002
#define CRDS_EJECT_REQUEST	0x0004
#define CRDS_INSERT_REQUEST	0x0008
#define CRDS_BATTERY_DEAD	0x0010
#define CRDS_BATTERY_WARNING	0x0020
#define CRDS_READY		0x0040
#define CRDS_CARD_DETECTED	0x0080

// Get Status SocketState bit definitions.

#define SKTS_WRITE_PROTECT	0x0001
#define SKTS_CARD_LOCK		0x0002
#define SKTS_EJECT_REQUEST	0x0004
#define SKTS_INSERT_REQUEST	0x0008
#define SKTS_BATTERY_DEAD	0x0010
#define SKTS_BATTERY_WARNING	0x0020
#define SKTS_READY		0x0040
#define SKTS_CARD_DETECT	0x0080


//-----------------------------------------------------------------------------
// Get Tuple Data Structure.

typedef struct GetTupleData_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	DesiredTuple;
    BYTE	TupleOffset;
    WORD	Flags;
    DWORD	LinkOffset;
    DWORD	CISOffset;
    WORD	TupleDataMax;
    WORD	TupleDataLen;
//  BYTE	TupleData[];
} GET_TUPLE_DATA_PKT;


//-----------------------------------------------------------------------------
// Map Logical/Physical Socket Structure.

typedef struct MapSocket_s {
    LOG_SOCKET	    LogSocket;
    PHY_ADAPTER     PhyAdapter;
    PHY_SOCKET	    PhySocket;
} MAP_SKT_PKT;


//-----------------------------------------------------------------------------
// Map Logical/Physical Window Structure.

typedef struct MapWindow_s {
    PHY_ADAPTER     PhyAdapter;
    PHY_WINDOW	    PhyWindow;
} MAP_WDW_PKT;


//-----------------------------------------------------------------------------
// Map Memory Page Structure.

typedef struct MapMemPage_s {
    DWORD   CardOffset;
    BYTE    PageNum;
} MAP_MEM_PAGE_PKT;


//-----------------------------------------------------------------------------
// Modify Configuration Structure.

typedef struct ModifyConfiguration_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	Vcc;
    BYTE	Vpp1;
    BYTE	Vpp2;
} MODIFY_CONFIG_PKT;

// Modify Configuration Attribute bit definitions.

//	CFGA_ENABLE_IRQ     0x0002
#define CFGA_IRQ_CHG	    0x0004
#define CFGA_VCC_CHG	    0x0008
#define CFGA_VPP1_CHG	    0x0010
#define CFGA_VPP2_CHG	    0x0020


//-----------------------------------------------------------------------------
// Modify Window Structure.

typedef struct ModifyWindow_s {
    WORD   Attributes;
    BYTE   AccessSpeed;
} MODIFY_WDW_PKT;

// Modify Window Attribute bit definitions.

#define WDWA_ATTRIB_MEM 	0x0002
#define WDWA_ENABLED            0x0004
#define WDWA_SPEED_VALID        0x0008

// Modify Window AccessSpeed bit definitions.
//  These are the same as the Get Partition AccessSpeed bit definitions.


//-----------------------------------------------------------------------------
// Open Memory Structure.

typedef struct OpenMemory_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    DWORD	CardOffset;
} OPEN_MEM_PKT;

// Open Memory Attributes bit definitions.

#define OPMA_ATTRIB_MEM     0x0001
#define OPMA_EXCLUSIVE	    0x0002


//-----------------------------------------------------------------------------
// Read Memory Structure.

typedef struct ReadMemory_s {
    DWORD   CardOffset;
    DWORD   Count;
} READ_MEM_PKT;


//-----------------------------------------------------------------------------
// Register Client Structure.

typedef struct ClientData_s {
    WORD    ReferenceData;
    WORD    ReservedWord;
    DWORD   DataOffset;
} CLIENT_DATA;

typedef struct RegisterClient_s{
    WORD    Attributes;
    WORD    EventMask;
    struct ClientData_s ClientData;
    WORD    Version;
} REGISTER_CLT_PKT;

// Register Client Attributes bit definitions.
//  These are the same as the Client Info Attributes bit definitions.

// Register Client EventMask bit definitions.
//  These are the same as the Event Mask EventMask bit definitions.


//-----------------------------------------------------------------------------
// Register Erase Queue Structure.

typedef struct QueueEntry_s {
    WORD    MemoryHandle;
    BYTE    EntryState;
    BYTE    EraseSize;
    DWORD   EraseOffset;
//  BYTE    Optional[];
} ERASE_Q_ENTRY;

typedef struct RegisterEraseQueue_s {
    WORD		QueueEntryLen;
    WORD		QueueEntryCnt;
//  struct QueueEntry_s QueueEntryArray[];
} REGISTER_ERASE_Q_PKT;

// Register Erase Queue EntryState definitions.

#define EQES_QUEUED_FOR_ERASE	    0x00
#define EQES_ERASE_IN_PROGRESS	    0x01  // 0x01 - 0x7F

#define EQES_MEDIA_MISSING	    0x80
#define EQES_MEDIA_WRITE_PROTECTED  0x84
#define EQES_MEDIA_NOT_ERASABLE     0x86
#define EQES_MEDIA_NOT_WRITABLE     0x87

#define EQES_BAD_OFFSET 	    0xC1
#define EQES_BAD_TECHNOLOGY	    0xC2
#define EQES_BAD_SOCKET 	    0xC3
#define EQES_BAD_VCC		    0xC4
#define EQES_BAD_VPP		    0xC5
#define EQES_BAD_SIZE		    0xC6

#define EQES_ERASE_PASSED	    0xE0
#define EQES_ERASE_FAILED	    0xE1

#define EQES_IDLE		    0xFF


//-----------------------------------------------------------------------------
// Register MTD Structure.

typedef struct RegisterMTD_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    DWORD	CardOffset;
    WORD	MTDMediaID;
} REGISTER_MTD_PKT;

// Register MTD Attributes bit definitions.
//  These are the same as the Get Partition Attributes bit definitions.


//-----------------------------------------------------------------------------
// MTD Request Callback Structure.

typedef struct MTDRequest_s  {
    WORD    PktLength;
    WORD    Socket;
    DWORD   SourceOffset;
    DWORD   DestOffset;
    DWORD   XferLength;
    UCHAR   Function;
    UCHAR   AccessSpeed;
    WORD    MTDID;
    WORD    MTDStatus;
    WORD    TimeoutCount;
    void *  MATDataPtr;
    void *  CardSetAddressPtr;
    void *  CardSetAutoIncPtr;
    void *  CardReadBytePtr;
    void *  CardReadWordPtr;
    void *  CardReadWordsPtr;
    void *  CardReadByteAIPtr;
    void *  CardReadWordAIPtr;
    void *  CardReadWordsAIPtr;
    void *  CardWriteBytePtr;
    void *  CardWriteWordPtr;
    void *  CardWriteWordsPtr;
    void *  CardWriteByteAIPtr;
    void *  CardWriteWordAIPtr;
    void *  CardWriteWordsAIPtr;
    void *  CardCompareBytePtr;
    void *  CardCompareByteAIPtr;
    void *  CardCompareWordsPtr;
    void *  CardCompareWordsAIPtr;
    void *  MTDHelperEntryPtr;
} MTD_REQUEST;

// MTD Request Function definitions.

#define MTDF_FUNC		0x03
#define MTDF_ERASE		0x00
#define MTDF_READ		0x01
#define MTDF_WRITE		0x02
#define MTDF_COPY		0x03

#define MTDF_DISABLE_ERASE	0x04
#define MTDF_VERIFY		0x08
#define MTDF_READY_CONT 	0x10
#define MTDF_TIMEOUT_CONT	0x20
#define MTDF_FIRST		0x40
#define MTDF_LAST		0x80

// MTD Request MTDStatus definitions.

#define MTDS_WAITREQ		0
#define MTDS_WAITTIMER		1
#define MTDS_WAITRDY		2
#define MTDS_WAITPOWER		3


//-----------------------------------------------------------------------------
// MTD Initialization Data Structure.

typedef struct MTDInit_s {
    LOG_SOCKET	Socket;
    DWORD	CardOffset;
} MTD_INIT;


//-----------------------------------------------------------------------------
// Register Timer Structure.

typedef struct RegisterTimer_s {
    WORD    WaitTicks;
} REGISTER_TIMER_PKT;


//-----------------------------------------------------------------------------
// Release Configuration Structure.

typedef struct ReleaseConfiguration_s {
    LOG_SOCKET	Socket;
} RELEASE_CONFIG_PKT;


//-----------------------------------------------------------------------------
// Release/Request Exclusive Structure.

typedef struct Exclusive_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
} EXCLUSIVE_PKT;


//-----------------------------------------------------------------------------
// Release I/O Structure.

typedef struct ReleaseIO_s {
    LOG_SOCKET		Socket;
    struct IOParms_s	IOParms[MAX_IO_WDWS_PER_SKT];
    BYTE		IOAddrLines;
} RELEASE_IO_PKT;


//-----------------------------------------------------------------------------
// Release IRQ Structure.

typedef struct ReleaseIRQ_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	AssignedIRQ;
} RELEASE_IRQ_PKT;

// Release IRQ Attributes bit definitions.

#define IRQA_IRQ_TYPE_MASK      0x0003
#define IRQA_EXCLUSIVE          0
#define IRQA_TIME_MUX           1
#define IRQA_DYNAMIC            2

#define IRQA_FORCE_PULSE        0x0004
#define IRQA_FIRST_SHARED       0x0008
#define IRQA_PULSE_ALLOC        0x0100


//-----------------------------------------------------------------------------
// Release Socket Mask Structure.

typedef struct ReleaseSocketMask_s {
    LOG_SOCKET	Socket;
} RELEASE_SKT_MASK_PKT;


//-----------------------------------------------------------------------------
// Replace Socket Services Structure.

typedef struct ReplaceSocketServices_s {
    LOG_SOCKET	Socket;
    WORD	NumSockets;
    WORD	Attributes;
    VOID	*DataPointer;
} REPLACE_SS_PKT;


//-----------------------------------------------------------------------------
// Request Configuration Structure.

typedef struct RequestConfiguration_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	Vcc;
    BYTE	Vpp1;
    BYTE	Vpp2;
    BYTE	IntType;
    DWORD	ConfigBase;
    BYTE	Status;
    BYTE	Pin;
    BYTE	Copy;
    BYTE	ConfigIndex;
    BYTE	Present;
} REQUEST_CONFIG_PKT;

// Request Configuration Attributes bit definitions.

//	CFGA_ENABLE_IRQ     0x0002	// Same as defined previously

// Request Configuration Present bit definitions.
//  These are the same as the Get Configuration CardValue bit definitions.

// Request Configuration IntType bit definitions.

#define CFGI_MEMORY	    0x01
#define CFGI_MEMORY_AND_IO  0x02


//-----------------------------------------------------------------------------
// Request I/O Structure.

typedef struct RequestIO_s {
    LOG_SOCKET		Socket;
    struct IOParms_s	IOParms[MAX_IO_WDWS_PER_SKT];
    BYTE		IOAddrLines;
} REQUEST_IO_PKT;


//-----------------------------------------------------------------------------
// Request IRQ Structure.

typedef struct RequestIRQ_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    BYTE	AssignedIRQ;
    BYTE	IRQInfo1;
    WORD	IRQInfo2;
} REQUEST_IRQ_PKT;

// Request IRQ Attributes bit definitions.
// These are the same as the Release IRQ Attributes bit definitions.

// Request IRQ IRQInfo1 bit definitions.

#define IRQI_NUMBER_MASK        0x0F

#define IRQI_NMI		0x01
#define IRQI_LOCK		0x02
#define IRQI_BERR		0x04
#define IRQI_VEND		0x08

#define IRQI_MASK		0x10
#define IRQI_LEVEL              0x20
#define IRQI_PULSE              0x40
#define IRQI_SHARE              0x80


//-----------------------------------------------------------------------------
// Request Socket Mask Structure.

typedef struct RequestSocketMask_s {
    LOG_SOCKET	Socket;
    WORD	EventMask;
} REQUEST_SKT_MASK_PKT;

// Request Socket Mask EventMask bit definitions.
//  These are the same as the Event Mask EventMask bit definitions.


//-----------------------------------------------------------------------------
// Request Window Structure.

typedef struct RequestWindow_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    DWORD	Base;
    DWORD	MemSize;
    BYTE	AccessSpeed;
} REQUEST_WDW_PKT;

// Request Window Attributes bit definitions.

//	WDWA_ATTRIB_MEM 	0x0002	// Same as defined previously
//	WDWA_ENABLED		0x0004	// Same as defined previously
#define WDWA_16BIT              0x0008
#define WDWA_PAGED              0x0010
#define WDWA_SHARED             0x0020
#define WDWA_FIRST_SHARED       0x0040
#define WDWA_OFFSETS_SIZED      0x0100

// Request Window AccessSpeed bit definitions.
//  These are the same as the Get Partition AccessSpeed bit definitions.


//-----------------------------------------------------------------------------
// Reset Card Structure.

typedef struct ResetCard_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
} RESET_CARD_PKT;


//-----------------------------------------------------------------------------
// Return Socket Services Entry Structure.

typedef struct ReturnSSEntry_s {
    WORD    Attributes;
} RETURN_SS_ENTRY_PKT;

// Return Socket Services Entry Attributes definitions.
//  These are the same as the Add Socket Services Attributes definitions.


//-----------------------------------------------------------------------------
// Set Region Structure.

typedef struct SetRegion_s {
    LOG_SOCKET	Socket;
    WORD	Attributes;
    DWORD	CardOffset;
    DWORD	RegionSize;
    DWORD	EffBlockSize;
    WORD	PartMultiple;
    WORD	JedecID;
    DWORD	BiasOffset;
    BYTE	AccessSpeed;
} SET_REGION_PKT;

// Set Region Attributes bit definitions.

//	MEMA_ATTRIB_MEM 	0x0001	// Same as defined previously
#define MEMA_DELETE_REGION	0x0002
//	MEMA_VIRTUAL		0x0100	// Same as defined previously

// Set Region AccessSpeed bit definitions.
//  These are the same as the Get Partition AccessSpeed bit definitions.


//-----------------------------------------------------------------------------
// Validate CIS Structure.

typedef struct ValidateCIS_s {
    LOG_SOCKET	Socket;
    WORD	Chains;
} VALIDATE_CIS_PKT;


//-----------------------------------------------------------------------------
// Vendor Specific Structure.

//typedef struct VendorSpecific_s {
//    WORD    InfoLen;
//    BYTE    VendorData[];
//} VENDOR_SPECIFIC_PKT;


//-----------------------------------------------------------------------------
// Write Memory Structure.

typedef struct WriteMemory_s {
    DWORD   CardOffset;
    DWORD   Count;
    WORD    Attributes;
} WRITE_MEM_PKT;

// Copy Memory Attributes bit definitions.
//  These are the same as the Copy Memory Attributes bit definitions.


/* XLATOFF */
#pragma pack()				/* return packing to normal   */
/* XLATON */
