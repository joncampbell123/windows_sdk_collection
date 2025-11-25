/*++

Copyright (c) 1991-1993 Microsoft Corporation

Module Name:

    video.h

Abstract:

    Contains all structure and routine definitions common to the video port
    driver and the video miniport drivers.

Notes:

Revision History:

--*/

//
// Define port driver status code.
// The values for these are the Win32 error codes
//

typedef LONG VP_STATUS;
typedef VP_STATUS *PVP_STATUS;

//
// Defines for registry information and synchronization.
//

typedef enum _VIDEO_DEVICE_DATA_TYPE {
    VpMachineData,
    VpCmosData,
    VpBusData,
    VpControllerData,
    VpMonitorData
} VIDEO_DEVICE_DATA_TYPE, *PVIDEO_DEVICE_DATA_TYPE;

typedef enum VIDEO_SYNCHRONIZE_PRIORITY {
    VpLowPriority,
    VpMediumPriority,
    VpHighPriority
} VIDEO_SYNCHRONIZE_PRIORITY, *PVIDEO_SYNCHRONIZE_PRIORITY;

//
// Define structure used to call the BIOS int 10 function
//

typedef struct _VIDEO_X86_BIOS_ARGUMENTS {
    ULONG Eax;
    ULONG Ebx;
    ULONG Ecx;
    ULONG Edx;
    ULONG Esi;
    ULONG Edi;
    ULONG Ebp;
} VIDEO_X86_BIOS_ARGUMENTS, *PVIDEO_X86_BIOS_ARGUMENTS;

//
// Debugging statements. This will remove all the debug information from the
// "free" version.
//

#if DBG
#define VideoDebugPrint(arg) VideoPortDebugPrint arg
#else
#define VideoDebugPrint(arg)
#endif


#ifndef _NTOS_

//
// These are the various function prototypes of the routines that are
// provided by the kernel driver to hook out access to io ports.
//

typedef
VP_STATUS
(*PDRIVER_IO_PORT_UCHAR ) (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PUCHAR Data
    );

typedef
VP_STATUS
(*PDRIVER_IO_PORT_UCHAR_STRING ) (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PUCHAR Data,
    ULONG DataLength
    );

typedef
VP_STATUS
(*PDRIVER_IO_PORT_USHORT ) (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PUSHORT Data
    );

typedef
VP_STATUS
(*PDRIVER_IO_PORT_USHORT_STRING ) (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PUSHORT Data,
    ULONG DataLength // number of words
    );

typedef
VP_STATUS
(*PDRIVER_IO_PORT_ULONG ) (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PULONG Data
    );

typedef
VP_STATUS
(*PDRIVER_IO_PORT_ULONG_STRING ) (
    ULONG Context,
    ULONG Port,
    UCHAR AccessMode,
    PULONG Data,
    ULONG DataLength  // number of dwords
    );

#endif // _NTOS_


//
// Definition of the request packet sent from the port driver to the
// miniport driver. It reflects the parameters passed from the
// DeviceIOControl call made by the windows display driver.
//

typedef struct _STATUS_BLOCK {

    //
    // Contains the status code of the operation.
    // This value in one of the Win32 error codes that are defined for use
    // in the video miniport drivers.
    //

    VP_STATUS Status;

    //
    // Information returned to the callee.
    // The meaning of the information varies from function to function. It
    // is generally used to return the minimum size for the input buffer if
    // the function takes an input buffer, or the amount of data transfered
    // back to the caller if the operation returns output.
    //

    ULONG Information;

} STATUS_BLOCK, *PSTATUS_BLOCK;

//
// BUGBUG
// What do we do about overlapping input and output buffers ?
// Do we want to create a separate output buffer or do we explain that there
// is a single buffer is used to perform the requests.
//

typedef struct _VIDEO_REQUEST_PACKET {

    //
    // The IO control code passed to the DeviceIoControl function by the
    // caller.
    //

    ULONG IoControlCode;

    //
    // Pointer to a status block provided by the caller. This should be
    // filled out by the callee with the appropriate information.
    //

    PSTATUS_BLOCK StatusBlock;

    //
    // Pointer to an input buffer which contains the information passed in
    // by the caller.
    //

    PVOID InputBuffer;

    //
    // Size of the input buffer
    //

    ULONG InputBufferLength;

    //
    // Pointer to an output buffer into which the data returned to the caller
    // should be stored.
    //

    PVOID OutputBuffer;

    //
    // Length of the output buffer. This buffer can not be grown by the
    // callee.
    //

    ULONG OutputBufferLength;

} VIDEO_REQUEST_PACKET, *PVIDEO_REQUEST_PACKET;


//
// The following structure is used to define access ranges. The ranges are
// used to indicate which ports and memory adresses are being used by the
// card.
//

typedef struct _VIDEO_ACCESS_RANGE {

    //
    // Indicates the starting memory address or port number of the range.
    // This values should be stored before being transformed by
    // VideoPortGetDeviceBase() which returns the logical address that must
    // be used by the miniport driver when referencing physical addresses.
    //

    PHYSICAL_ADDRESS RangeStart;

    //
    // Indicates the length in bytes, or number of ports in the range. This
    // value should indicate the range actually decoded by the adapter. For
    // example, if the adapter uses 7 registers but responds to eight, the
    // RangeLength should be set to 8.

    ULONG RangeLength;

    //
    // Indicates if the range is in IO space (TRUE) or in memory space (FALSE).
    //

    UCHAR RangeInIoSpace;

    //
    // Indicates if the range should be visible by the Windows display driver.
    // This is done so that a Windows display driver can access certain
    // video ports directly. This will only be allowed if the caller has the
    // required privileges (is a trusted subsystem) to access the range.
    //
    // Synchronization of access to ports or memory in the range must be
    // done explicitly by the miniport driver and the user mode process so
    // that they both don't try to program the device simultaneously.
    //
    // Non visible ranges should include video memory, ROM addresses, etc.
    // which are not required to program the device for output purposes.
    //
    //

    UCHAR RangeVisible;

    //
    // This field determines if the range can be shared with another device.
    // The rule should be applied as follow.
    //
    // - If the range of memory or IO ports should be "owned" by this driver,
    //   and that any other driver trying to access this range may cause
    //   a problem, FALSE should be returned.
    //
    // - If the range can be shared with another co-operating device driver,
    //   then the share field should be set to TRUE.
    //
    // As a guideline, the VGA miniport driver will claim all of its resources
    // as shareable so that it can be used as a VGA compatible device with
    // any other driver (such as an S3 or XGA.
    //
    // Super VGA miniport drivers that implement all the VGA functionality
    // (declared in the Registry as VGACOMPATIBLE=1) should claim the range
    // as non-shareable since they don't want the VGA to run at the same time.
    //
    // Miniports for cards such as an S3 or XGA that have an XGA on the board
    // but do not implement the VGA functionality will run with the VGA
    // miniport loaded and should therefore claim all the resources shared
    // with the VGA as shareable.
    //
    // Miniports for cards that work with a pass-through and that can be
    // connected to any VGA/SVGA card should not be using any VGA ports or
    // memory ranges ! ... but if they do they should not claim those
    // resources since they will cause a conflict in the system because the
    // SVGA cards will have claimed them as non-shareable ...
    //

    UCHAR RangeShareable;

} VIDEO_ACCESS_RANGE, *PVIDEO_ACCESS_RANGE;


//
// This structure contains the specific configuration information about the
// device. The information is initialized by the port driver and it should
// be completed by the mniport driver.
// The information is used to setup the device, as weel as providing
// information to the port driver so it can perform some of the requests on
// behalf of the miniport driver.
//

typedef struct _VIDEO_PORT_CONFIG_INFO {

    //
    // Specifies the length of the PVIDEO_PORT_CONFIG_INFO structure as
    // returned by sizeof(). Since this structure may grow in later
    // releases, the miniport driver should check that the length of the
    // structure is greater than or equal to the length it expects (since
    // it is guaranteed that defined fields will not change).
    //
    // This field is always initialized by the port driver.
    //

    ULONG Length;

    //
    // Specifies which IO bus is tp be scanned. This field is used as a
    // parameter to some VideoPortXXX calls.
    //
    // This field is always initialized by the port driver.
    //

    ULONG SystemIoBusNumber;

    //
    // Specifies the type of bus being scanned. This field is equal to the
    // value being passed into VideoPortInitialize in the
    // VIDEO_HW_INITIALIZATION_DATA structure.
    //
    // This field is always initialized by the port driver.
    //

    INTERFACE_TYPE AdapterInterfaceType;

    //
    // Specifies the bus interrupt request level. This level corresponds to
    // the IRQL on ISA and MCA buses.
    // This value is only used if the device supports interrupts, which is
    // determined by the presence of an interrupt service routine in the
    // VIDEO_HW_INITIALIZATION_DATA structure.
    //
    // The preset default value for this field is zero. Otherwise, it is the
    // value found in the device configuration information.
    //

    ULONG BusInterruptLevel;

    //
    // Specifies the bus vector returned by the adapter. This is used for
    // systems which have IO buses that use interrupt vectors. For ISA, MCA
    // and EISA buses, this field is unused.
    //
    // The preset default value for this field is zero.
    //

    ULONG BusInterruptVector;

    //
    // Specifies whether this adapter uses latched or edge-triggered type
    // interrupts.
    //
    // This field is always initialized by the port driver.
    //

    KINTERRUPT_MODE InterruptMode;

    //
    // NOTE:
    // DMA information is not included since it is not yet supported by the
    // video port driver.
    //

    //
    // Specifies the number of emulator access entries that the adapter
    // uses. This value is the same as was passed into VideoPortInitialize()
    // in the VIDEO_HW_INITIALIZATION_DATA structure. It indicates the
    // number of array elements in the following field.
    //
    // This field can be reinitialized with the number of entries in the
    // EmulatorAccessEntries structure if the structure is statically
    // defined in the miniport driver. The EmulatorAccessEntries fields
    // should also be updated.
    //

    ULONG NumEmulatorAccessEntries;

    //
    // Supplies a pointer to an array of EMULATOR_ACCESS_ENTRY structures.
    // The number of elements in the array is indicated by the
    // NumEmulatorAccessEntries field. The driver should fill out each entry
    // for the adapter.
    //
    // The uninitialized value for the structure is NULL.
    // EmulatorAccessEntries will be NULL if NumEmulatorAccessEntries is
    // zero.
    //
    // A poiner to an array of emulator access entries can be passed back
    // if such a structure is defined statically in the miniport driver. The
    // NumEmulatorAccessEntries field should also be updated.
    //

    PEMULATOR_ACCESS_ENTRY EmulatorAccessEntries;

    //
    // This is a context values that is passed with each call to the
    // emulator/validator functions defined in the EmulatorAccessEntries
    // defined above.
    // This parameter should in general be a pointer to the miniports
    // device extension or other such storage location.
    //
    // This pointer will allow the miniport to save some state temporarily
    // to allow for the batching of IO requests.
    //

    ULONG EmulatorAccessEntriesContext;

    //
    // Physical address of the video memory that must be mapped into a VDM's
    // address space for proper BIOS support
    //

    PHYSICAL_ADDRESS VdmPhysicalVideoMemoryAddress;

    //
    // Length of the video memory that must be mapped into a VDM's addres
    // space for proper BIOS support.
    //

    ULONG VdmPhysicalVideoMemoryLength;

    //
    // Determines the minimum size required to store the hardware state
    // information returned by IOCTL_VIDEO_SAVE_HARDWARE_STATE.
    //
    // The uninitialized value for this field is zero.
    //
    // If the field is left to zero, SAVE_HARDWARE_STATE will return an
    // ERROR_INVALID_FUNCTION status code.
    //

    ULONG HardwareStateSize;

} VIDEO_PORT_CONFIG_INFO, *PVIDEO_PORT_CONFIG_INFO;


//
// Video Adapter Dependent Routines.
//

typedef
VP_STATUS
(*PVIDEO_HW_FIND_ADAPTER) (
    PVOID HwDeviceExtension,
    PVOID HwContext,
    PWSTR ArgumentString,
    PVIDEO_PORT_CONFIG_INFO ConfigInfo,
    PUCHAR Again
    );

typedef
BOOLEAN
(*PVIDEO_HW_INITIALIZE) (
    PVOID HwDeviceExtension
    );

typedef
BOOLEAN
(*PVIDEO_HW_INTERRUPT) (
    PVOID HwDeviceExtension
    );

typedef
BOOLEAN
(*PVIDEO_HW_START_IO) (
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );


//
// Structure passed by the miniport entry point to the video port
// initialization routine.
//

typedef struct _VIDEO_HW_INITIALIZATION_DATA {

    //
    // Supplies the size of the structure in bytes as determined by sizeof().
    //

    ULONG HwInitDataSize;

    //
    // Indicates the bus type the adapter works with, such as Eisa, Isa, MCA.
    //

    INTERFACE_TYPE AdapterInterfaceType;

    //
    // Supplies a pointer to the miniport driver's find adapter routine.
    //

    PVIDEO_HW_FIND_ADAPTER HwFindAdapter;

    //
    // Supplies a pointer to the miniport driver's initialization routine.
    //

    PVIDEO_HW_INITIALIZE HwInitialize;

    //
    // Supplies a pointer to the miniport driver's interrupt service routine.
    //

    PVIDEO_HW_INTERRUPT HwInterrupt;

    //
    // Supplies a pointer to the miniport driver's start io routine.
    //

    PVIDEO_HW_START_IO HwStartIO;

    //
    // Supplies the size in bytes required for the miniport driver's private
    // device extension. This storage is used by the miniport driver to hold
    // per-adapter information. A pointer to this storage is provided with
    // every call made to the miniport driver. This data storage is
    // initialized to zero by the port driver.
    //

    ULONG HwDeviceExtensionSize;

    //
    // Supplies the number with which device numbering should be started.
    // The device numbering is used to determine which \DeviceX entry under
    // the \Parameters section in the registry should be used for parameters
    // to the miniport driver.
    // The number is *automatically* incremented when the miniport is called
    // back in it's FindAdapter routine due to an appropriate _Again_
    // parameter.
    //

    ULONG StartingDeviceNumber;

} VIDEO_HW_INITIALIZATION_DATA, *PVIDEO_HW_INITIALIZATION_DATA;


//
// Port driver routines called by miniport driver
//

ULONG
VideoPortCompareMemory(
    PVOID Source1,
    PVOID Source2,
    ULONG Length
    );

VOID
VideoPortDebugPrint(
    ULONG DebugPrintLevel,
    PCHAR DebugMessage,
    ...
    );

VP_STATUS
VideoPortDisableInterrupt(
    PVOID HwDeviceExtension
    );

VP_STATUS
VideoPortEnableInterrupt(
    PVOID HwDeviceExtension
    );

VOID
VideoPortFreeDeviceBase(
    PVOID HwDeviceExtension,
    PVOID MappedAddress
    );

PVOID
VideoPortGetDeviceBase(
    PVOID HwDeviceExtension,
    PHYSICAL_ADDRESS IoAddress,
    ULONG NumberOfUchars,
    UCHAR InIoSpace
    );

typedef
VP_STATUS
(*PMINIPORT_QUERY_DEVICE_ROUTINE)(
    PVOID HwDeviceExtension,
    PVOID Context,
    VIDEO_DEVICE_DATA_TYPE DeviceDataType,
    PVOID Identifier,
    ULONG IdentiferLength,
    PVOID ConfigurationData,
    ULONG ConfigurationDataLength,
    PVOID ComponentInformation,
    ULONG ComponentInformationLength
    );

VP_STATUS
VideoPortGetDeviceData(
    PVOID HwDeviceExtension,
    VIDEO_DEVICE_DATA_TYPE DeviceDataType,
    PMINIPORT_QUERY_DEVICE_ROUTINE CallbackRoutine,
    PVOID Context
    );

typedef
VP_STATUS
(*PMINIPORT_GET_REGISTRY_ROUTINE)(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

VP_STATUS
VideoPortGetRegistryParameters(
    PVOID HwDeviceExtension,
    PWSTR ParamterName,
    UCHAR IsParameterFileName,
    PMINIPORT_GET_REGISTRY_ROUTINE GetRegistryRoutine,
    PVOID Context
    );

VP_STATUS
VideoPortInitialize(
    PVOID Argument1,
    PVOID Argument2,
    PVIDEO_HW_INITIALIZATION_DATA HwInitializationData,
    PVOID HwContext
    );

VP_STATUS
VideoPortInt10(
    PVOID HwDeviceExtension,
    PVIDEO_X86_BIOS_ARGUMENTS BiosArguments
    );

VOID
VideoPortLogError(
    IN PVOID HwDeviceExtension,
    IN PVIDEO_REQUEST_PACKET Vrp OPTIONAL,
    IN VP_STATUS ErrorCode,
    IN ULONG UniqueId
    );

VP_STATUS
VideoPortMapMemory(
    PVOID HwDeviceExtension,
    PHYSICAL_ADDRESS PhysicalAddress,
    PULONG Length,
    PULONG InIoSpace,
    PVOID *VirtualAddress
    );

VOID
VideoPortMoveMemory(
    PVOID Destination,
    PVOID Source,
    ULONG Length
    );

UCHAR
VideoPortReadPortUchar(
    PUCHAR Port
    );

USHORT
VideoPortReadPortUshort(
    PUSHORT Port
    );

ULONG
VideoPortReadPortUlong(
    PULONG Port
    );

VOID
VideoPortReadPortBufferUchar(
    PUCHAR Port,
    PUCHAR Buffer,
    ULONG Count
    );

VOID
VideoPortReadPortBufferUshort(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG Count
    );

VOID
VideoPortReadPortBufferUlong(
    PULONG Port,
    PULONG Buffer,
    ULONG Count
    );

UCHAR
VideoPortReadRegisterUchar(
    PUCHAR Register
    );

USHORT
VideoPortReadRegisterUshort(
    PUSHORT Register
    );

ULONG
VideoPortReadRegisterUlong(
    PULONG Register
    );

VOID
VideoPortReadRegisterBufferUchar(
    PUCHAR Register,
    PUCHAR Buffer,
    ULONG Count
    );

VOID
VideoPortReadRegisterBufferUshort(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG Count
    );

VOID
VideoPortReadRegisterBufferUlong(
    PULONG Register,
    PULONG Buffer,
    ULONG Count
    );

BOOLEAN
VideoPortScanRom(
    PVOID HwDeviceExtension,
    PUCHAR RomBase,
    ULONG RomLength,
    PUCHAR String
    );

VP_STATUS
VideoPortSetRegistryParameters(
    PVOID HwDeviceExtension,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

VP_STATUS
VideoPortSetTrappedEmulatorPorts(
    PVOID HwDeviceExtension,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRange
    );

VOID
VideoPortStallExecution(
    ULONG Microseconds
    );

typedef
BOOLEAN
(*PMINIPORT_SYNCHRONIZE_ROUTINE)(
    PVOID Context
    );

BOOLEAN
VideoPortSynchronizeExecution(
    PVOID HwDeviceExtension,
    VIDEO_SYNCHRONIZE_PRIORITY Priority,
    PMINIPORT_SYNCHRONIZE_ROUTINE synchronizeRoutine,
    PVOID Context
    );

VP_STATUS
VideoPortUnmapMemory(
    PVOID HwDeviceExtension,
    PVOID VirtualAddress,
    ULONG InIoSpace
    );

VP_STATUS
VideoPortVerifyAccessRanges(
    PVOID HwDeviceExtension,
    ULONG NumAccessRanges,
    PVIDEO_ACCESS_RANGE AccessRanges
    );

VOID
VideoPortWritePortUchar(
    PUCHAR Port,
    UCHAR Value
    );

VOID
VideoPortWritePortUshort(
    PUSHORT Port,
    USHORT Value
    );

VOID
VideoPortWritePortUlong(
    PULONG Port,
    ULONG Value
    );

VOID
VideoPortWritePortBufferUchar(
    PUCHAR Port,
    PUCHAR Buffer,
    ULONG Count
    );

VOID
VideoPortWritePortBufferUshort(
    PUSHORT Port,
    PUSHORT Buffer,
    ULONG Count
    );

VOID
VideoPortWritePortBufferUlong(
    PULONG Port,
    PULONG Buffer,
    ULONG Count
    );

VOID
VideoPortWriteRegisterUchar(
    PUCHAR Register,
    UCHAR Value
    );

VOID
VideoPortWriteRegisterUshort(
    PUSHORT Register,
    USHORT Value
    );

VOID
VideoPortWriteRegisterUlong(
    PULONG Register,
    ULONG Value
    );

VOID
VideoPortWriteRegisterBufferUchar(
    PUCHAR Register,
    PUCHAR Buffer,
    ULONG Count
    );

VOID
VideoPortWriteRegisterBufferUshort(
    PUSHORT Register,
    PUSHORT Buffer,
    ULONG Count
    );

VOID
VideoPortWriteRegisterBufferUlong(
    PULONG Register,
    PULONG Buffer,
    ULONG Count
    );

VOID
VideoPortZeroMemory(
    PVOID Destination,
    ULONG Length
    );
