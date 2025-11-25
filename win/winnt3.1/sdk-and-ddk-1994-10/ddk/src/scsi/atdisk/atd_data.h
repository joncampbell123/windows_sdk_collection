/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    atd_data.h

Abstract:

    This file includes data and hardware (non-platform-dependent)
    declarations for the AT disk (aka ST506 and standard hard disk)
    driver for NT.

Author:

    Chad Schwitters (chads) 21-Feb-1991.

Environment:

    Kernel mode only.

Notes:

Revision History:

--*/



//
// Define our out dbg print routines.
//

#if DBG
extern ULONG AtDebugLevel;
#define ATBUGCHECK           ((ULONG)0x80000000)
#define ATDIAG1              ((ULONG)0x00000001)
#define ATDIAG2              ((ULONG)0x00000002)
#define ATERRORS             ((ULONG)0x00000004)
#define ATINIT               ((ULONG)0x00000008)
#define AtDump(LEVEL,STRING) \
        do { \
            ULONG _level = (LEVEL); \
            if (AtDebugLevel & _level) { \
                DbgPrint STRING; \
            } \
            if (_level == ATBUGCHECK) { \
                ASSERT(FALSE); \
            } \
        } while (0)
#else
#define AtDump(LEVEL,STRING) do {NOTHING;} while (0)
#endif

//BUGBUG delete TemporaryArcNamePrefix when HalGetNumberOfBusses is defined

#if defined(i386) || (defined(MIPS) && defined(COMPAQ))
static CCHAR TemporaryArcNamePrefix[] = { "\\ArcName\\multi(0)" };
#else
static CCHAR TemporaryArcNamePrefix[] = { "\\ArcName\\eisa(0)" };
#endif

//
// The number of controllers is arbitrary.  No machines with more than
// two are known, but we'll use a larger number to be safe.  The number
// of disks per controller is a hardware limitation, and some of the code
// takes advantage of the fact that it's known to be 2.
//

#define MAXIMUM_NUMBER_OF_CONTROLLERS           6
#define MAXIMUM_NUMBER_OF_DISKS_PER_CONTROLLER  2

//
// For device name manipulation, we allocate a buffer since there is no
// preset limit on name size.  The system hands us a prefix, and we
// allocate the buffer to be the size of that prefix plus the delta defined
// below.  This size gives us room for
//     PREFIX + <disk number> + \partition + <partition number>
// with up to five digits per number.
//

#define DEVICE_NAME_DELTA            20

//
// Partitions need to be linked to ARC names, in case we're booting off
// the partition.  The system hands us a prefix, and we allocate the
// buffer to be the size of that prefix plus the delta defined below.
// This size gives us room for
//      PREFIX + disk(<#>)rdisk(<#>)partition(<#>)
// with up to five digits per number.
//

#define ARC_NAME_DELTA               39

//
// When writing a message to the error log file, we need to allocate a
// buffer for the message.  This is the message structure we impose, along
// with some values for the first field.
//

typedef struct _ERROR_LOG_ENTRY {
    CCHAR ErrorType;
    CCHAR Data1;
    CCHAR Data2;
    CCHAR Data3;
    CCHAR Data4;
    CCHAR Data5;
} ERROR_LOG_ENTRY;
typedef ERROR_LOG_ENTRY *PERROR_LOG_ENTRY;

#define ERROR_LOG_ENTRY_LENGTH       sizeof( ERROR_LOG_ENTRY )

#define ERROR_LOG_TYPE_CORRECTABLE_ERROR    1
#define ERROR_LOG_TYPE_ERROR                2
#define ERROR_LOG_TYPE_TIMEOUT              3
#define ERROR_LOG_TYPE_TIMEOUT_DURING_RESET 4

//
// If the hardware state gets messed up, we'll retry the current packet.
// This says how many times we'll retry before giving up and returning
// an error.  Note that the hardware invisibly retries 8 times.
//

#define RETRY_IRP_MAXIMUM_COUNT 10

//
// When we're resetting the controller, we have to go through some states
// as various operations finish (that is, interrupt).  Here's the states
// in order.
//

#define RESET_NOT_RESETTING                 0
#define RESET_FIRST_DRIVE_SET               1
#define RESET_FIRST_DRIVE_RECALIBRATED      2
#define RESET_SECOND_DRIVE_SET              3
#define RESET_SECOND_DRIVE_RECALIBRATED     4

//
// The I/O system calls our timer routine once every second.  If the timer
// counter is -1, the timer is "off" and the timer routine will just return.
// When we want to make sure a hardware event will occur, we set the timer.
// The timer routine decrements the counter once a second.  The timer
// expires if the counter hits 0.  We give the timer extra time for
// recalibrates since they're so slow.
//

#define CANCEL_TIMER                 -1
#define EXPIRED_TIMER                0
#define START_TIMER                  9
#define START_TIMER_FOR_RECALIBRATE  11
#define START_BUSY_COUNTDOWN 60

//
// ST506 controller commands for the command register.
//

#define READ_COMMAND                 0x20 // retries enabled
#define WRITE_COMMAND                0x30 // retries enabled
#define RECALIBRATE_COMMAND          0x10 // move drive heads to track 0
#define SEEK_COMMAND                 0x70 // normally implied in read/write
#define SET_DRIVE_PARAMETERS_COMMAND 0x91 // set drive parameters
#define IDENTIFY_COMMAND             0xEC // identify drive parameters

//
// ST506 controller bit masks for the status register.
//

#define BUSY_STATUS                  0x80 // busy bit in status register
#define ERROR_STATUS                 0x01 // error bit in status register
#define CORRECTED_ERROR_STATUS       0x04 // corrected error in status register
#define DATA_REQUEST_STATUS          0x08 // data request bit in status register

//
// ST506 controller bit masks for the drive select/head register.
//

#define DRIVE_1                      0xA0 // drive 1 (C:), 512 bytes/sector
                                          // DRIVE/HEAD port, ext bit on
#define DRIVE_2_SELECTED             0x10 // bit that says drive 2 is selected

#define DRIVE_2                      DRIVE_1 | DRIVE_2_SELECTED

//
// Define object extensions used by the driver.  The first is a controller
// extension, which starts at the end of each controller object.
//

typedef struct _CONTROLLER_EXTENSION {
    PCONTROLLER_OBJECT ControllerObject;  // "extension" points to object
    struct _CONTROLLER_EXTENSION *NextControllerExtension; // link 'em together
    struct _DISK_EXTENSION *Disk1;        // have to access the disks from ISR
    struct _DISK_EXTENSION *Disk2;        // this one's NULL if only one disk
    PKPOWER_NOTIFY PowerNotifyObject;     // ptr to power notify object
    PKPOWER_STATUS PowerStatusObject1;    // ptr to 1st power status object
    PKPOWER_STATUS PowerStatusObject2;    // ptr to 2nd power status object
    PKDPC PowerNotifyDpcObject;           // ptr to DPC object for power notify
    PDEVICE_OBJECT WhichDeviceObject;     // set to D.O. that expects interrupt
    PDEVICE_OBJECT FirstFailingDeviceObject; // for timeout while resetting
    PUCHAR ControllerAddress;             // base addr of controller registers
    PUCHAR ControlPortAddress;
    LONG InterruptTimer;                  // used to time out operations
    CCHAR ResettingController;            // >0 while controller is being reset
    CCHAR ControlFlags;                   // OR into CONTROL_PORT
    BOOLEAN InterruptRequiresDpc;         // true if ISR should queue DPC
    BOOLEAN PowerFailed;                  // system sets to TRUE @ power fail
    BOOLEAN PowerRecoveryNotDone;
    BOOLEAN ControllerAddressMapped;
    BOOLEAN ControllerPortMapped;
    struct _DISK_EXTENSION *BusyDevice;   // Set when we find the device is
                                          // busy (e.g. spin up on laptop)
    LONG BusyCountDown;
    PKINTERRUPT InterruptObject;          // only one needed per controller
    CCHAR GarbageCan[1];                  // THIS MUST BE AT THE END OF
                                          // OF THE CONTROLLER EXTENSION!!!
                                          // When the controller extension
                                          // is allocated we will actually
                                          // allocate
                                          // sizeof(CONTROLLER_EXTENSION) +
                                          // (BYTES_PER_INTERRUPT-1) where
                                          // GarbageCan will be an array
                                          // of size BYTES_PER_INTERRUPT.
                                          // This array is used to fill
                                          // and empty the controller cache
                                          // in case of an error.
} CONTROLLER_EXTENSION;

typedef CONTROLLER_EXTENSION *PCONTROLLER_EXTENSION;

//
// This is the disk extension, which is attached to all partition 0
// device objects (which represent the disk).  Note that the first three
// fields are identical to those of the partition extension, so that the
// same code can access the disk via partition 0 or partition n.
//

typedef struct _DISK_EXTENSION {
    struct _DISK_EXTENSION *Partition0;   // ptr to self; must be first field
    LARGE_INTEGER StartingOffset;         // first disk sector of this partition
    LARGE_INTEGER PartitionLength;        // number of sectors in partition
    PCONTROLLER_EXTENSION ControllerExtension; // ptr to disk's controller
    PDEVICE_OBJECT DeviceObject;          // ptr to this disk's object
    PDEVICE_OBJECT PartitionObjects;      // ptr to chain of partition objects
    struct _DISK_EXTENSION *OtherDiskExtension; // other disk on controller
    ULONG FirstSectorOfRequest;           // start sector of whole request
    ULONG FirstSectorOfTransfer;          // start sector for current transfer
    ULONG RemainingRequestLength;         // # of sectors left in current op
    ULONG TotalTransferLength;            // length of current transfer
    ULONG RemainingTransferLength;        // length left in current transfer
    ULONG SequenceNumber;                 // Sequence number that is incremented
                                          // on every new irp for this device.
    HANDLE DirectoryHandle;               // handle to disk's device directory
    PCCHAR CurrentAddress;                // working address in user's buffer
    USHORT BytesPerSector;                // disk-specific values
    USHORT SectorsPerTrack;               // ...
    USHORT PretendSectorsPerTrack;        // ...
    USHORT NumberOfCylinders;             // ...
    USHORT PretendNumberOfCylinders;      // ...
    USHORT TracksPerCylinder;             // ...
    USHORT PretendTracksPerCylinder;      // ...
    USHORT WritePrecomp;                  // ...
    USHORT BytesPerInterrupt;             // ...
    CCHAR ByteShiftToSector;              // ...
    CCHAR ReadCommand;                    // ...
    CCHAR WriteCommand;                   // ...
    CCHAR VerifyCommand;                  // ...
    CCHAR OperationType;                  // current command (ie IRP_MJ_READ)
    UCHAR DeviceUnit;                     // which disk we are to the controller
    CCHAR IrpRetryCount;                  // count of retries by driver
    BOOLEAN PacketIsBeingRetried;         // when driver calls AtDiskStartIo
} DISK_EXTENSION;

typedef DISK_EXTENSION *PDISK_EXTENSION;

//
// This is the partition extension, which is attached to all partition
// "n" device objects - except for partition 0, which gets a disk
// extension.  Note that the first three fields are identical to those of
// the disk extension, so that the same code can access the disk via
// partition 0 or partition n.
//

typedef struct _PARTITION_EXTENSION {
    PDISK_EXTENSION Partition0;           // ptr to partn. 0, must be 1st field
    LARGE_INTEGER StartingOffset;         // first disk sector of this partition
    LARGE_INTEGER PartitionLength;        // number of sectors in partition
    LARGE_INTEGER HiddenSectors;          // number of hidden sectors
    PDEVICE_OBJECT NextPartitionObject;   // ptr to next partition's object
    ULONG PartitionNumber;                // 1-based number of partition
    UCHAR PartitionType;                  // type of this partition
    BOOLEAN BootIndicator;                // indicates partition bootable or not
} PARTITION_EXTENSION;
typedef PARTITION_EXTENSION *PPARTITION_EXTENSION;

//
// Fixed disk parameter table structure
//

#pragma pack(1)                                 // This structure is packed
typedef struct _FIXED_DISK_PARAMETER_TABLE {
    USHORT MaxCylinders;
    CCHAR  MaxHeads;
    UCHAR  Signature;
    UCHAR  TranslatedSectorsPerTrack;
    USHORT StartWritePrecomp;
    CCHAR  EccBurstLength;
    CCHAR  ControlFlags;
    USHORT TranslatedMaxCylinders;
    UCHAR  TranslatedMaxHeads;
    USHORT LandingZone;
    CCHAR  SectorsPerTrack;
    UCHAR  ReservedForFuture;
    } FIXED_DISK_PARAMETER_TABLE;

typedef FIXED_DISK_PARAMETER_TABLE *PFIXED_DISK_PARAMETER_TABLE;

#pragma pack()

//
// This structure holds all of the configuration data; it's filled in by
// AtGetConfigInfo() and read by other initialization routines.
//

typedef struct _DRIVE_DATA {
    PFIXED_DISK_PARAMETER_TABLE ParameterTableAddress;
    USHORT NumberOfCylinders;
    USHORT TracksPerCylinder;
    USHORT SectorsPerTrack;
    USHORT PretendNumberOfCylinders;
    USHORT PretendTracksPerCylinder;
    USHORT PretendSectorsPerTrack;
    USHORT BytesPerSector;
    USHORT BytesPerInterrupt;
    USHORT WritePrecomp;
    UCHAR DriveType;
    CCHAR ReadCommand;
    CCHAR WriteCommand;
    CCHAR VerifyCommand;
    BOOLEAN DisableReadCache;
} DRIVE_DATA, *PDRIVE_DATA;

typedef struct _CONTROLLER_DATA {
    DRIVE_DATA Disk[MAXIMUM_NUMBER_OF_DISKS_PER_CONTROLLER];
    PUCHAR ControllerBaseAddress;
    PUCHAR ControlPortAddress;
    PHYSICAL_ADDRESS OriginalControllerBaseAddress;
    PHYSICAL_ADDRESS OriginalControlPortAddress;
    KINTERRUPT_MODE InterruptMode;
    KAFFINITY ProcessorNumber;
    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    ULONG RangeOfControllerBase;
    ULONG RangeOfControlPort;
    BOOLEAN SaveFloatState;
    BOOLEAN SharableVector;
    BOOLEAN ControllerBaseMapped;
    BOOLEAN ControlPortMapped;
    BOOLEAN OkToUseThisController;
    KIRQL ControllerIrql;
    KIRQL OriginalControllerIrql;
    ULONG ControllerVector;
    CCHAR ControlFlags;
    UCHAR OriginalControllerVector;
} CONTROLLER_DATA, *PCONTROLLER_DATA;


typedef struct _CONFIG_DATA {
    PCCHAR NtNamePrefix;
    PCCHAR ArcNamePrefix;
    PULONG HardDiskCount;
    HANDLE DeviceKey;
    CONTROLLER_DATA Controller[MAXIMUM_NUMBER_OF_CONTROLLERS];
} CONFIG_DATA, *PCONFIG_DATA;

//
// IDENTIFY data
//

typedef struct _IDENTIFY_DATA {
    USHORT GeneralConfiguration;            // 00
    USHORT NumberOfCylinders;               // 02
    USHORT Reserved1;                       // 04
    USHORT NumberOfHeads;                   // 06
    USHORT UnformattedBytesPerTrack;        // 08
    USHORT UnformattedBytesPerSector;       // 0A
    USHORT SectorsPerTrack;                 // 0C
    USHORT VendorUnique1[3];                // 0E
    USHORT SerialNumber[10];                // 14
    USHORT BufferType;                      // 28
    USHORT BufferSectorSize;                // 2A
    USHORT NumberOfEccBytes;                // 2C
    USHORT FirmwareRevision[4];             // 2E
    USHORT ModelNumber[20];                 // 36
    UCHAR  MaximumBlockTransfer;            // 5E
    UCHAR  VendorUnique2;                   // 5F
    USHORT DoubleWordIo;                    // 60
    USHORT Capabilities;                    // 62
    USHORT Reserved2;                       // 64
    UCHAR  VendorUnique3;                   // 66
    UCHAR  PioCycleTimingMode;              // 67
    UCHAR  VendorUnique4;                   // 68
    UCHAR  DmaCycleTimingMode;              // 69
    USHORT TranslationFieldsValid:1;        // 6A
    USHORT Reserved3:15;
    USHORT NumberOfCurrentCylinders;        // 6C
    USHORT NumberOfCurrentHeads;            // 6E
    USHORT CurrentSectorsPerTrack;          // 70
    ULONG  CurrentSectorCapacity;           // 72
    USHORT Reserved4[197];                  // 76
} IDENTIFY_DATA, *PIDENTIFY_DATA;

//
// Prototypes of external routines.
//

int
sprintf(
    char *s,
    const char *format,
    ...
    );

//
// Device driver routine declarations.
//

NTSTATUS
DriverEntry(
    IN OUT PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    );

NTSTATUS
AtGetConfigInfo(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath,
    IN OUT PCONFIG_DATA ConfigData
    );

NTSTATUS
AtInitializeController(
    IN struct _CONFIG_DATA *ConfigData,
    IN CCHAR ControllerNumber,
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
AtInitializePowerFail(
    IN PCONTROLLER_EXTENSION ControllerExtension
    );

NTSTATUS
AtInitializeDisk(
    IN struct _CONFIG_DATA *ConfigData,
    IN CCHAR ContNumber,
    IN CCHAR DiskNumber,
    IN PDRIVER_OBJECT DriverObject,
    IN PCONTROLLER_EXTENSION ControllerExtension
    );

NTSTATUS
AtDiskDispatchCreateClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
AtDiskDispatchDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
AtDiskDispatchReadWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID
AtDiskStartIo(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

IO_ALLOCATION_ACTION
AtInitiate(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PVOID MapRegisterBase,
    IN PVOID Context
    );

BOOLEAN
AtStartDevice(
    IN PVOID Context
    );

BOOLEAN
AtDiskInterruptService(
    IN PKINTERRUPT Interrupt,
    IN PVOID Context
    );

VOID
AtDiskDeferredProcedure(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

BOOLEAN
AtDiskStartReset(
    IN PVOID Context
    );

VOID
AtDiskUnloadDriver(
    IN PDRIVER_OBJECT DriverObject
    );

VOID
AtDiskCheckTimer(
    IN PDEVICE_OBJECT DeviceObject,
    IN PVOID TimerCounter
    );

BOOLEAN
AtCheckTimerSync(
    IN PVOID Context
    );

NTSTATUS
AtWaitControllerBusy(
    PUCHAR StatusRegisterAddress,
    ULONG MicrosecondsToDelay,
    ULONG TimesToDelay
    );

NTSTATUS
AtWaitControllerReady(
    IN PCONTROLLER_EXTENSION ControllerExtension,
    IN ULONG MicrosecondsToDelay,
    IN ULONG TimesToDelay
    );

VOID
AtLogError(
    IN PDEVICE_OBJECT DeviceObject,
    IN ULONG SequenceNumber,
    IN UCHAR MajorFunctionCode,
    IN UCHAR RetryCount,
    IN ULONG UniqueErrorValue,
    IN NTSTATUS FinalStatus,
    IN NTSTATUS SpecificIOStatus,
    IN CCHAR ErrorType,
    IN UCHAR Parameter1,
    IN UCHAR Parameter2,
    IN UCHAR Parameter3,
    IN UCHAR Parameter4,
    IN UCHAR Parameter5,
    IN UCHAR Parameter6,
    IN UCHAR Parameter7
    );

PVOID
AtGetTranslatedMemory(
    IN INTERFACE_TYPE BusType,
    IN ULONG BusNumber,
    IN PHYSICAL_ADDRESS IoAddress,
    IN ULONG NumberOfBytes,
    IN BOOLEAN InIoSpace,
    OUT PBOOLEAN MappedAddress
    );

BOOLEAN
AtReportUsage(
    IN PCONFIG_DATA ConfigData,
    IN UCHAR ControllerNumber,
    IN PDRIVER_OBJECT DriverObject
    );

VOID
AtBuildDeviceMap(
    IN ULONG ControllerNumber,
    IN ULONG DiskNumber,
    IN PHYSICAL_ADDRESS ControllerAddress,
    IN KIRQL Irql,
    IN PDRIVE_DATA Disk,
    IN PIDENTIFY_DATA DiskData
    );

BOOLEAN
AtDiskControllerInfo(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath,
    IN ULONG WhichController,
    IN OUT PCONTROLLER_DATA Controller,
    IN PHYSICAL_ADDRESS DefaultBaseAddress,
    IN PHYSICAL_ADDRESS DefaultPortAddress,
    IN KIRQL DefaultIrql,
    IN INTERFACE_TYPE DefaultInterfaceType,
    IN ULONG DefaultBusNumber,
    IN BOOLEAN UseDefaults
    );

BOOLEAN
AtResetController(
    IN PUCHAR StatusRegAddress,
    IN PUCHAR DriveControlAddress,
    IN CCHAR ControlFlags
    );

BOOLEAN
AtControllerPresent(
    PCONTROLLER_DATA ControllerData
    );
