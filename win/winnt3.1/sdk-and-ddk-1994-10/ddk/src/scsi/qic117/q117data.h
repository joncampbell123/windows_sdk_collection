/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    q117data.h

Abstract:


Revision History:




--*/



//
// Define the maximum number of controllers and floppies per controller
// that this driver will support.
//
// The number of floppies per controller is fixed at 4, since the
// controllers don't have enough bits to select more than that (and
// actually, many controllers will only support 2).  The number of
// controllers per machine is arbitrary; 3 should be more than enough.
//

#define MAXIMUM_CONTROLLERS_PER_MACHINE    3

//
// Macros to access the controller.  Note that the *_PORT_UCHAR macros
// work on all machines, whether the I/O ports are separate or in
// memory space.
//

#define READ_CONTROLLER( Address )                         \
    READ_PORT_UCHAR( ( PUCHAR )Address )

#define WRITE_CONTROLLER( Address, Value )                 \
    WRITE_PORT_UCHAR( ( PUCHAR )Address, ( UCHAR )Value )


//
// Floppy register structure.  The base address of the controller is
// passed in by configuration management.  Note that this is the 82077
// structure, which is a superset of the PD765 structure.  Not all of
// the registers are used.
//

typedef struct _TAPE_ADDRESS {
    UCHAR StatusA;
    UCHAR StatusB;
    UCHAR dor;
    UCHAR tdr;
    union {
        UCHAR msr;
        UCHAR dsr;
    } MSDSR;
    UCHAR dr;
    UCHAR Reserved2;
    UCHAR dcr;
} TAPE_ADDRESS, *PTAPE_ADDRESS;


//
// Define the maximum number of tape drives per controller
// that this driver will support.
//
// The number of tape drives per controller is fixed at 1, since the
// software select schemes generally work for one drive only.
//

#define MAXIMUM_TAPE_DRIVES_PER_CONTROLLER 1


//
// This structure holds all of the configuration data.  It is filled in
// by FlGetConfigurationInformation(), which gets the information from
// the configuration manager or the hardware architecture layer (HAL).
//

typedef struct _CONFIG_CONTROLLER_DATA {
    PHYSICAL_ADDRESS OriginalBaseAddress;
    PTAPE_ADDRESS   ControllerBaseAddress;
    PADAPTER_OBJECT AdapterObject;
    ULONG           SpanOfControllerAddress;
    ULONG           NumberOfMapRegisters;
    ULONG           BusNumber;
    ULONG           OriginalIrql;
    ULONG           OriginalVector;
    ULONG           OriginalDmaChannel;
    PKEVENT         ControllerEvent;
    LONG            ActualControllerNumber;
    INTERFACE_TYPE  InterfaceType;
    KINTERRUPT_MODE InterruptMode;
    KAFFINITY       ProcessorMask;
    KIRQL           ControllerIrql;
    BOOLEAN         SaveFloatState;
    BOOLEAN         SharableVector;
    BOOLEAN         MappedAddress;
    BOOLEAN         OkToUseThisController;
    ULONG           ControllerVector;
    UCHAR           NumberOfTapeDrives;
    UCHAR           DriveType[MAXIMUM_TAPE_DRIVES_PER_CONTROLLER];
} CONFIG_CONTROLLER_DATA,*PCONFIG_CONTROLLER_DATA;

typedef struct _CONFIG_DATA {
    ULONG           FloppyTapeCount;
    UCHAR           NumberOfControllers;
    CONFIG_CONTROLLER_DATA Controller[MAXIMUM_CONTROLLERS_PER_MACHINE];
} CONFIG_DATA;

typedef CONFIG_DATA *PCONFIG_DATA;




typedef struct _TAPE_CONTROLLER_DATA {
    LIST_ENTRY          ListEntry;
    KEVENT              ClearQueueEvent;
    KEVENT              InterruptEvent;
    KEVENT              AllocateAdapterChannelEvent;
    HANDLE              ControllerEventHandle;
    PKEVENT             ControllerEvent;
    KSEMAPHORE          RequestSemaphore;
    KSPIN_LOCK          ListSpinLock;
    PKINTERRUPT         InterruptObject;
    PVOID               MapRegisterBase;
    LONG                ActualControllerNumber;
    PADAPTER_OBJECT     AdapterObject;
    ULONG               NumberOfMapRegisters;
    PDEVICE_OBJECT      CurrentDeviceObject;
    PTAPE_ADDRESS       FDC_Addr;
    LONG                FloppyControllerEventTimer;
    PVOID               TapeExtension;
    INTERFACE_TYPE      InterfaceType;
    DRIVE_SELECT        DriveSelect;
    FORMAT_CMD          FmtCmd;
    FDC_STATUS          FdcStat;
    BOOLEAN             CommandHasResultPhase;
    BOOLEAN             UnloadingDriver;
    UCHAR               NumberOfTapeDrives;
    UCHAR               FDC_Pcn;
    UCHAR               FifoByte;
    BOOLEAN             QueueEmpty;
    BOOLEAN             ClearQueue;
    BOOLEAN             AbortRequested;
    BOOLEAN             CurrentInterrupt;
    BOOLEAN             PerpendicularMode;
    BOOLEAN             StartFormatMode;
    BOOLEAN             EndFormatMode;
    BOOLEAN             AdapterLocked;
    UCHAR               PerpModeSelect;
} TAPE_CONTROLLER_DATA;

typedef TAPE_CONTROLLER_DATA *PTAPE_CONTROLLER_DATA;

typedef struct _TAPE_OPERATION_STATUS {
    ULONG               BytesTransferredSoFar;
    ULONG               TotalBytesOfTransfer;
    ULONG               CurLst;
    SHORT               DataAmount;
    SHORT               Scount;
    SHORT               NoDat;
    SHORT               D_Amt;
    SHORT               RetryCount;
    SHORT               RetryTimes;
    UCHAR               D_FTK;
    UCHAR               D_Sect;
    CHAR                D_Head;
    UCHAR               RetrySectorId;
    CHAR                SeekFlag;
    UCHAR               S_Sect;
} TAPE_OPERATION_STATUS,*PTAPE_OPERATION_STATUS;


typedef struct _TAPE_EXTENSION {
    PDEVICE_OBJECT          QDeviceObject;
    PTAPE_CONTROLLER_DATA   QControllerData;
    ULONG                   BytesPerSector;
    ULONG                   ByteCapacity;
    ULONG                   ErrorSequence;
    ULONG                   TapeNumber;
    MEDIA_TYPE              MediaType;
    DRIVE_PARAMETERS        DriveParms;
    TRANSFER_RATE           XferRate;
    TAPE_PARAMETERS         TapeParms;
    TAPE_POSITION           TapePosition;
    MISC_DRIVE_INFO         MiscDriveInfo;
    TAPE_OPERATION_STATUS   RdWrOp;
    FMT_PARAMETERS          FmtOp;
    SHORT                   RetrySeqNum;
    USHORT                  NumTracks;
    FIRMWARE_ERROR          FirmwareError;
    UCHAR                   DriveType;
    UCHAR                   DeviceUnit;
    UCHAR                   DriveOnValue;
    BOOLEAN                 NoPause;
    BOOLEAN                 PersistentNewCart;
    BOOLEAN                 NewCart;
    BOOLEAN                 NoCart;
    BOOLEAN                 SpeedChangeOK;
    BOOLEAN                 Found;
    BOOLEAN                 PegasusSupported;
#if DBG
#define DBG_SIZE  4096
    ULONG DbgCommand[DBG_SIZE];
    int DbgHead;
    int DbgTail;
    BOOLEAN DbgLockout;
#define DbgAddEntry(data) \
        TapeExtension->DbgCommand[TapeExtension->DbgTail] = (data); \
\
        if (!TapeExtension->DbgLockout) \
            ++TapeExtension->DbgTail; \
\
        if (TapeExtension->DbgTail >= DBG_SIZE) { \
            TapeExtension->DbgTail = 0; \
        }

#else
#define DbgAddEntry(data)
#endif

} TAPE_EXTENSION;

typedef TAPE_EXTENSION *PTAPE_EXTENSION;

//
// Prototypes of driver routines.
//

NTSTATUS
DriverEntry(
    IN OUT PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

NTSTATUS
Q117iConfigCallBack(
    IN PVOID Context,
    IN PUNICODE_STRING PathName,
    IN INTERFACE_TYPE BusType,
    IN ULONG BusNumber,
    IN PKEY_VALUE_FULL_INFORMATION *BusInformation,
    IN CONFIGURATION_TYPE ControllerType,
    IN ULONG ControllerNumber,
    IN PKEY_VALUE_FULL_INFORMATION *ControllerInformation,
    IN CONFIGURATION_TYPE PeripheralType,
    IN ULONG PeripheralNumber,
    IN PKEY_VALUE_FULL_INFORMATION *PeripheralInformation
    );

NTSTATUS
Q117iGetConfigurationInformation(
    OUT PCONFIG_DATA *ConfigData
    );

NTSTATUS
Q117iInitializeController(
    IN PCONFIG_DATA ConfigData,
    IN UCHAR ControllerNumber,
   IN PDRIVER_OBJECT DriverObject,
   IN PUNICODE_STRING RegistryPath
    );

BOOLEAN
Q117iReportResources(
    IN PDRIVER_OBJECT DriverObject,
    IN PCONFIG_DATA ConfigData,
    IN UCHAR ControllerNumber
    );

NTSTATUS
Q117iInitializeControllerHardware(
   IN PTAPE_CONTROLLER_DATA ControllerData,
    IN PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
Q117iInitializeDrive(
    IN PCONFIG_DATA ConfigData,
   IN PTAPE_CONTROLLER_DATA ControllerData,
    IN UCHAR ControllerNum,
    IN PDRIVER_OBJECT DriverObject,
   IN PUNICODE_STRING RegistryPath
    );

NTSTATUS
Q117iTapeDispatchInternalDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

NTSTATUS
Q117iTapeDispatchReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PIRP Irp
    );

BOOLEAN
Q117iTapeInterruptService(
    IN PKINTERRUPT Interrupt,
    IN OUT PVOID Context
    );

VOID
Q117iTapeDeferredProcedure(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

VOID
Q117iTapeUnloadDriver(
    IN PDRIVER_OBJECT DriverObject
    );

VOID
Q117iTapeMotorOffDpc(
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID InterruptTimer
    );

NTSTATUS
Q117iRecalibrateDrive(
    IN PTAPE_EXTENSION TapeExtension
    );

NTSTATUS
Q117iDatarateSpecifyConfigure(
    IN PTAPE_EXTENSION TapeExtension
    );

NTSTATUS
Q117iResetController(
    IN OUT PTAPE_CONTROLLER_DATA ControllerData,
    IN UCHAR DataRate
    );

NTSTATUS
Q117iStartDrive(
    IN OUT PTAPE_EXTENSION TapeExtension,
    IN BOOLEAN WriteOperation,
    IN BOOLEAN SetUpMedia
    );

NTSTATUS
Q117iDetermineMediaType(
    IN OUT PTAPE_EXTENSION TapeExtension
    );

VOID
Q117iTapeThread(
    IN OUT PTAPE_CONTROLLER_DATA ControllerData
    );

IO_ALLOCATION_ACTION
Q117iTapeAllocateAdapterChannel(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID MapRegisterBase,
    IN PVOID Context
    );

NTSTATUS
Q117iFormat(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp
    );

NTSTATUS
Q117iIssueCommand(
    IN DRIVER_COMMAND Command,
    IN OUT PTAPE_EXTENSION TapeExtension
    );

ULONG
Q117iGetControllerBase(
    IN INTERFACE_TYPE BusType,
    IN ULONG BusNumber,
    IN PHYSICAL_ADDRESS IoAddress,
    IN ULONG NumberOfBytes,
    IN BOOLEAN InIoSpace,
    OUT PBOOLEAN MappedAddress
    );

NTSTATUS
Q117iProcessItem(
    IN OUT PIRP Irp
    );

NTSTATUS
Q117iClearIO(
    IN OUT PIRP Irp
    );

NTSTATUS
Q117iTranslateError(
    IN PDEVICE_OBJECT DeviceObject,
    IN STATUS Retval
    );

VOID
Q117iResetInterruptEvent(
    IN PTAPE_EXTENSION TapeExtension
    );

VOID
Q117iResetFDC(
    IN PTAPE_EXTENSION TapeExtension
    );

VOID
Q117iDLockUnlockDMA(
    IN PTAPE_EXTENSION TapeExtension,
    IN BOOLEAN Lock
    );

VOID
hio_ProgramDMA(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp,
    IN BOOLEAN WriteOperation
    );

VOID
hio_FlushDMA(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp,
    IN BOOLEAN WriteOperation
    );

VOID
Q117iShortTimer(
    IN QIC_TIME StallTime
    );

STATUS
Q117iSleep(
    IN PTAPE_EXTENSION TapeExtension,
    IN QIC_TIME WaitTime,
    IN BOOLEAN InterruptSleep
    );

STATUS
Q117iDLocateDrv(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDDeselect(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iConvertFirmwareError(
    IN FIRMWARE_ERROR FirmwareError
    );

STATUS
Q117iSetDriveMode(
    IN PTAPE_EXTENSION TapeExtension,
    IN CHAR Mode
    );

STATUS
Q117iProgramFDC(
    IN PTAPE_EXTENSION TapeExtension,
    IN UCHAR *Command,
    IN SHORT Length,
    IN BOOLEAN Result
    );

STATUS
Q117iConfigureDrive(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iConfigureFDC(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iSelectDrive(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDeselectDrive(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDFmt(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp,
    IN OUT PIO_REQUEST Ioqucrnt
    );

STATUS
Q117iFormatTrack(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp,
    IN SHORT Track,
    IN OUT PIO_REQUEST Ioqucrnt
    );

STATUS
Q117iFormatSegment(
    IN PTAPE_EXTENSION TapeExtension,
    IN PIRP Irp,
    IN OUT PIO_REQUEST Ioqucrnt,
    IN UCHAR Cylinder,
    IN UCHAR Head,
    IN UCHAR Sector
    );

STATUS
Q117iWriteReferenceBurst(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iReadWrite(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST Ioqucrnt,
    IN OUT PIRP Irp
    );

STATUS
Q117iCalcPosition(
    IN PTAPE_EXTENSION TapeExtension,
    IN ULONG Block,
    IN UCHAR Number
    );

VOID
Q117iGetRetryCounts(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    );

VOID
Q117iNextGoodSectors(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iRW_Timeout(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST Ioqucrnt,
    IN STATUS *Status
    );

STATUS
Q117iRW_Normal(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST Ioqucrnt,
    OUT STATUS *Status
    );

STATUS
Q117iRetryCode(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST Ioqucrnt,
    OUT FDC_STATUS *FdcStatus,
    OUT STATUS *Status
    );

STATUS
Q117iNextTry(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    );

STATUS
Q117iSetBack(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    );

STATUS
Q117iStartTape(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iReadFDC(
    IN PTAPE_EXTENSION TapeExtension,
    OUT UCHAR *Status,
    OUT SHORT *Length
    );

VOID
Q117iResetFDC(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iGetDriveError(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iReport(
    IN PTAPE_EXTENSION TapeExtension,
    IN CHAR Command,
    OUT USHORT  *ReportData,
    IN SHORT   ReportSize,
    OUT CHAR *ESDRetry
    );

STATUS
Q117iWaitCommandComplete(
    IN PTAPE_EXTENSION TapeExtension,
    IN QIC_TIME WaitTime
    );


STATUS
Q117iGetStatus(
    IN PTAPE_EXTENSION TapeExtension,
    OUT UCHAR *StatReg3
    );

STATUS
Q117iPauseTape(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iSeek(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iChangeTrack(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT dest_track
    );

STATUS
Q117iReadIDRepeat(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDoReadID(
    IN PTAPE_EXTENSION TapeExtension,
    IN QIC_TIME ReadIdDelay,
    OUT FDC_STATUS *ReadIdStatus
    );

VOID
Q117iGetComFirmStr(
    IN PTAPE_EXTENSION TapeExtension,
    OUT CHAR **ptr,
    IN SHORT index
    );

STATUS
Q117iLogicalBOT(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iAdjustTapeZone(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT TapePosition
    );

STATUS
Q117iHighSpeedSeek(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iWaitSeek(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT SeekDelay
    );

STATUS
Q117iReceiveByte(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT ReceiveLength,
    OUT USHORT *ReceiveData
    );

STATUS
Q117iWaitActive(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iTapeCommands(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PIO_REQUEST Ioqucrnt,
    IN OUT PIRP Irp
    );

STATUS
Q117iDEject(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDReten(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDFast_DSlow(
    IN PTAPE_EXTENSION TapeExtension,
    IN DRIVER_COMMAND Command
    );

STATUS
Q117iDGetRev(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR Version
    );

STATUS
Q117iDGetCap(
    IN PTAPE_EXTENSION TapeExtension,
    OUT struct S_O_DGetCap *Capacity
    );

STATUS
Q117iDComFirm(
    IN PTAPE_EXTENSION TapeExtension,
    IN OUT PUCHAR CommandString
    );

STATUS
Q117iDSndWPro(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDSndReel(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDGetCart(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR TapeType
    );

STATUS
Q117iDChkDrv(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR DriveType
    );

STATUS
Q117iDGetSpeed(
    IN PTAPE_EXTENSION TapeExtension,
    OUT struct S_O_DGetSpeed *XferRate
    );

STATUS
Q117iDStatus(
    IN PTAPE_EXTENSION TapeExtension,
    OUT struct S_O_DStatus *DriveStatus
    );

STATUS
Q117iDGetFDC(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR FdcType
    );

STATUS
Q117iDGetDriveInfo(
    IN PTAPE_EXTENSION TapeExtension,
    OUT struct S_O_DMiscInfo *MiscInfoPtr
    );

STATUS
Q117iDChkFmt(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iDReportProtoVer(
    IN PTAPE_EXTENSION TapeExtension,
    OUT PUCHAR  ProtoVer
    );

STATUS Q117iDFndDrv(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iClearTapeError(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS Q117iLookForDrive(
    IN PTAPE_EXTENSION TapeExtension,
    IN UCHAR DriveSelector,
    IN BOOLEAN WaitForTape
    );

STATUS Q117iGetDriveType(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS Q117iGetDriveSize(
    IN PTAPE_EXTENSION TapeExtension,
    IN BOOLEAN ReportFailed,
    IN USHORT VendorId,
    IN UCHAR Signature
    );

STATUS Q117iNewTape(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iReadWrtProtect(
    IN PTAPE_EXTENSION TapeExtension,
    OUT SHORT *WriteProtect
    );

STATUS
Q117iGetTapeParameters(
    IN PTAPE_EXTENSION TapeExtension
    );

VOID
Q117iCalcFmtSegmentsAndTracks(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iMtnPreamble(
    IN PTAPE_EXTENSION TapeExtension,
    IN BOOLEAN Select
    );

STATUS
Q117iGetDriveInfo(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iStopTape(
    IN PTAPE_EXTENSION TapeExtension
    );

STATUS
Q117iSendByte(
    IN PTAPE_EXTENSION TapeExtension,
    IN FIRMWARE_CMD Command
    );

STATUS
Q117iSenseSpeed(
    IN PTAPE_EXTENSION TapeExtension
    );

VOID
Q117iDCR_Out(
    IN PTAPE_EXTENSION TapeExtension,
    IN SHORT Speed
    );

VOID
Q117iExtractFloppyConfig(
    IN OUT PCONFIG_DATA ConfigData,
    IN PDEVICE_OBJECT DeviceObject
    );

STATUS
Q117iRecalibrateFDC(
    IN PTAPE_EXTENSION TapeExtension
    );

VOID
q117iUpdateRegistryInfo(
    IN PTAPE_EXTENSION TapeExtension
    );
