/*++

Copyright (c) 1990-1992  Microsoft Corporation

Module Name:

    xga.c

Abstract:

    This module contains the code that implements the XGA miniport driver.

Environment:

    Kernel mode

Revision History:

--*/

#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"
#include "xga.h"
#include "xgaioctl.h"

#include "xgaloger.h"


//
// Function Prototypes
//
// Functions that start with 'Xga' are entry points for the OS port driver.
//

VP_STATUS
XgaFindAdapter(
    PVOID HwDeviceExtension,
    PVOID HwContext,
    PWSTR ArgumentString,
    PVIDEO_PORT_CONFIG_INFO ConfigInfo,
    PUCHAR Again
    );

BOOLEAN
XgaInitialize(
    PVOID HwDeviceExtension
    );

BOOLEAN
XgaStartIO(
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );

//
// Define device driver procedure prototypes.
//

VP_STATUS
XgaGetPosData(
    PVOID HwDeviceExtension,
    PVOID Context,
    VIDEO_DEVICE_DATA_TYPE DeviceDataType,
    PVOID Identifier,
    ULONG IdentifierLength,
    PVOID ConfigurationData,
    ULONG ConfigurationDataLength,
    PVOID ComponentInformation,
    ULONG ComponentInformationLength
    );

ULONG
XgaFindXga(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PCM_MCA_POS_DATA PosData
    );

VP_STATUS
XgaSetMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG ModeNum
    );

VP_STATUS
XgaGetRegistryParamaterCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

//
// Last slot at which an adapter was found.
//

ULONG XgaSlot;
LONG BusNumber = -1;


ULONG
DriverEntry (
    PVOID Context1,
    PVOID Context2
    )

/*++

Routine Description:

    Installable driver initialization entry point.
    This entry point is called directly by the I/O system.

Arguments:

    Context1 - First context value passed by the operating system. This is
        the value with which the miniport driver calls VideoPortInitialize().

    Context2 - Second context value passed by the operating system. This is
        the value with which the miniport driver calls VideoPortInitialize().

Return Value:

    Status from VideoPortInitialize()

--*/

{

    VIDEO_HW_INITIALIZATION_DATA hwInitData;

    //
    // Zero out structure.
    //

    VideoPortZeroMemory(&hwInitData, sizeof(VIDEO_HW_INITIALIZATION_DATA)) ;

    //
    // Specify sizes of structure and extension.
    //

    hwInitData.HwInitDataSize = sizeof(VIDEO_HW_INITIALIZATION_DATA);

    //
    // Set entry points.
    //

    hwInitData.HwFindAdapter = XgaFindAdapter;
    hwInitData.HwInitialize  = XgaInitialize;
    hwInitData.HwInterrupt   = NULL;
    hwInitData.HwStartIO     = XgaStartIO;

    //
    // Determine the size we require for the device extension.
    //

    hwInitData.HwDeviceExtensionSize = sizeof(HW_DEVICE_EXTENSION);

    //
    // Always start with parameters for device0 in this case.
    //

//    hwInitData.StartingDeviceNumber = 0;

    //
    // This device only supports the internal bus type. So return the status
    // value directly to the operating system.
    //

    hwInitData.AdapterInterfaceType = MicroChannel;

    return VideoPortInitialize(Context1,
                               Context2,
                               &hwInitData,
                               NULL);

} // end DriverEntry()

VP_STATUS
XgaFindAdapter(
    PVOID HwDeviceExtension,
    PVOID HwContext,
    PWSTR ArgumentString,
    PVIDEO_PORT_CONFIG_INFO ConfigInfo,
    PUCHAR Again
    )

/*++

Routine Description:

    This routine is called to determine if the adapter for this driver
    is present in the system.
    If it is present, the function fills out some information describing
    the adapter.

Arguments:

    HwDeviceExtension - Supplies the miniport driver's adapter storage. This
        storage is initialized to zero before this call.

    HwContext - Supplies the context value which was passed to
        VideoPortInitialize().

    ArgumentString - Suuplies a NULL terminated ASCII string. This string
        originates from the user.

    ConfigInfo - Returns the configuration information structure which is
        filled by the miniport driver. This structure is initialized with
        any knwon configuration information (such as SystemIoBusNumber) by
        the port driver. Where possible, drivers should have one set of
        defaults which do not require any supplied configuration information.

    Again - Indicates if the miniport driver wants the port driver to call
        its VIDEO_HW_FIND_ADAPTER function again with a new device extension
        and the same config info. This is used by the miniport drivers which
        can search for several adapters on a bus.

Return Value:

    This routine must return:

    NO_ERROR - Indicates a host adapter was found and the
        configuration information was successfully determined.

    ERROR_INVALID_PARAMETER - Indicates an adapter was found but there was an
        error obtaining the configuration information. If possible an error
        should be logged.

    ERROR_DEV_NOT_EXIST - Indicates no host adapter was found for the
        supplied configuration information.

--*/

{

//
// Number of entries in the access range structure passed to the port driver.
//

#define NUM_XGA_ACCESS_RANGES 6

    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    CM_MCA_POS_DATA posData;
    ULONG ioAddress;
    VP_STATUS status;
    VIDEO_ACCESS_RANGE accessRange[NUM_XGA_ACCESS_RANGES];
    PHYSICAL_ADDRESS A0000PhysicalAddress;
    PHYSICAL_ADDRESS passThroughPort;

    A0000PhysicalAddress.LowPart = 0x000A0000;
    A0000PhysicalAddress.HighPart = 0x00000000;

    passThroughPort.LowPart = 0x000003C3;
    passThroughPort.HighPart = 0x00000000;

    //
    // Indicate we do not wish to be called over
    //

    *Again = 0;

    //
    // Make sure the size of the structure is at least as large as what we
    // are expecting (check version of the config info structure).
    //

    if (ConfigInfo->Length < sizeof(VIDEO_PORT_CONFIG_INFO)) {

        return ERROR_INVALID_PARAMETER;

    }

    //
    // Reset the Slot number being examined to zero if we are processing
    // a new bus number.
    //

    if (BusNumber != (LONG)ConfigInfo->SystemIoBusNumber) {

        BusNumber = ConfigInfo->SystemIoBusNumber;
        XgaSlot = 0;

    }

    //
    // This picks up the POS registers.
    //

    if (NO_ERROR != VideoPortGetDeviceData(hwDeviceExtension,
                                           VpBusData,
                                           XgaGetPosData,
                                           &posData)) {

        VideoDebugPrint((2, "Xga: GetDeviceData returned error - probably no XGAs\n"));

        return ERROR_DEV_NOT_EXIST;
    }

    //
    // Since we got the data properly, make sure we get called again to find
    // the next XGA adapter.
    //

    *Again = 1;

    //
    // Determine the XGA registers by munging the POS register data.
    //

    ioAddress = (posData.PosData1 &0x0E) >> 1;

    hwDeviceExtension->PhysicalRomBaseAddress.LowPart =
        (((posData.PosData1 & 0xF0) >> 4) * 0x2000) + 0xC0000 ;
    hwDeviceExtension->PhysicalRomBaseAddress.HighPart = 0x00000000 ;

    hwDeviceExtension->PhysicalIoRegBaseAddress.LowPart = 0x2100 +
        (ioAddress << 4) ;
    hwDeviceExtension->PhysicalIoRegBaseAddress.HighPart = 0x00000000;

    //
    // Choose a size of 1 MEG for video memory as a default
    //

    hwDeviceExtension->PhysicalVideoMemoryLength = 0x00100000;

    //
    // Find the virtual address of the frame buffer. Get the 4Meg aperture
    // if it is available, otherwise take the 1MEG.
    //

    if (posData.PosData3 & 0x01) {

        VideoDebugPrint((1, "Xga: using the 4 MEG Aperture\n"));

        hwDeviceExtension->PhysicalVideoMemoryAddress.LowPart =
            ((posData.PosData3 &0xFE) << 24) | (ioAddress << 22) ;
        hwDeviceExtension->PhysicalVideoMemoryAddress.HighPart =
              0x00000000 ;

    } else {

        VideoDebugPrint((1, "Xga: trying the 1 MEG aperture\n"));

        hwDeviceExtension->PhysicalVideoMemoryAddress.LowPart =
            (posData.PosData4 &0x0F) << 20 ;
        hwDeviceExtension->PhysicalVideoMemoryAddress.HighPart = 0x00000000 ;

    }

    //
    // If none of these apertures are open, (64K aperture) then fail.
    // 1 MEG aperture is disabled if the base is 0
    //

    if (!hwDeviceExtension->PhysicalVideoMemoryAddress.LowPart) {

        VideoDebugPrint((1, "Xga: 64K aperture\n"));

        VideoPortLogError(hwDeviceExtension,
                          NULL,
                          XGA_WRONG_APERTURE,
                          __LINE__);

        return ERROR_INVALID_PARAMETER;

    }

    //
    // Get the address of the Co processor registers.
    //

    hwDeviceExtension->PhysicalCoProcessorAddress.LowPart = 0x80 * ioAddress
        + 0x1C00 + hwDeviceExtension->PhysicalRomBaseAddress.LowPart;
    hwDeviceExtension->PhysicalCoProcessorAddress.HighPart = 0x00000000 ;

    //
    // Save the data for access ranges
    //

    //
    // Io Ports
    //

    accessRange[0].RangeStart = hwDeviceExtension->PhysicalIoRegBaseAddress;
    accessRange[0].RangeLength = XGA_IO_REGS_SIZE;
    accessRange[0].RangeInIoSpace = TRUE;
    accessRange[0].RangeVisible = TRUE;
    accessRange[0].RangeShareable = FALSE;

    //
    // Video Memory
    //

    accessRange[1].RangeStart = hwDeviceExtension->PhysicalVideoMemoryAddress;
    accessRange[1].RangeLength = hwDeviceExtension->PhysicalVideoMemoryLength;
    accessRange[1].RangeInIoSpace = FALSE;
    accessRange[1].RangeVisible = TRUE;
    accessRange[1].RangeShareable = FALSE;

    //
    // ROM Location
    //

    accessRange[2].RangeStart = hwDeviceExtension->PhysicalRomBaseAddress;
    accessRange[2].RangeLength = XGA_ROM_SIZE;
    accessRange[2].RangeInIoSpace = FALSE;
    accessRange[2].RangeVisible = TRUE;
    accessRange[2].RangeShareable = FALSE;

    //
    // Co-Processor Location
    //

    accessRange[3].RangeStart = hwDeviceExtension->PhysicalCoProcessorAddress;
    accessRange[3].RangeLength = XGA_CO_PROCESSOR_REGS_SIZE;
    accessRange[3].RangeInIoSpace = FALSE;
    accessRange[3].RangeVisible = TRUE;
    accessRange[3].RangeShareable = FALSE;

    //
    // Resources shared with the VGA
    //

    //
    // Io Ports
    //

    accessRange[4].RangeStart = passThroughPort;
    accessRange[4].RangeLength = 0x00000001;
    accessRange[4].RangeInIoSpace = TRUE;
    accessRange[4].RangeVisible = FALSE;
    accessRange[4].RangeShareable = TRUE;

    //
    // Video Memory
    //

    accessRange[5].RangeStart = A0000PhysicalAddress;
    accessRange[5].RangeLength = 0x00010000;
    accessRange[5].RangeInIoSpace = FALSE;
    accessRange[5].RangeVisible = FALSE;
    accessRange[5].RangeShareable = TRUE;

    //
    // Check to see if there is a hardware resource conflict.
    //

    status = VideoPortVerifyAccessRanges(HwDeviceExtension,
                                         NUM_XGA_ACCESS_RANGES,
                                         accessRange);

    if (status != NO_ERROR) {

        return status;

    }

    //
    // Clear out the Emulator entries and the state size since this driver
    // does not support them.
    //

    ConfigInfo->NumEmulatorAccessEntries = 0;
    ConfigInfo->EmulatorAccessEntries    = NULL;
    ConfigInfo->EmulatorAccessEntriesContext = 0;

    ConfigInfo->VdmPhysicalVideoMemoryAddress.LowPart = 0x00000000;
    ConfigInfo->VdmPhysicalVideoMemoryAddress.HighPart = 0x00000000;
    ConfigInfo->VdmPhysicalVideoMemoryLength = 0x00000000;

    ConfigInfo->HardwareStateSize = 0;

    //
    // Map the video memory into the system virtual address space so we can
    // clear it out.
    //

    if ( (hwDeviceExtension->FrameAddress =
              VideoPortGetDeviceBase(hwDeviceExtension,
                                     hwDeviceExtension->PhysicalVideoMemoryAddress,
                                     hwDeviceExtension->PhysicalVideoMemoryLength,
                                     FALSE)) == NULL) {

        return ERROR_INVALID_PARAMETER;

    }

    //
    // Map in the memory at A0000 so we can test the card for the amount of
    // memory afterwards.
    //

    if ( ( hwDeviceExtension->A0000MemoryAddress =
              VideoPortGetDeviceBase(hwDeviceExtension,
                                     A0000PhysicalAddress,
                                     0x00010000,
                                     FALSE)) == NULL) {

        VideoDebugPrint((2, "XgaFindAdapter - Fail to get A0000 aperture address\n"));

        return ERROR_INVALID_PARAMETER;

    }

    //
    // Map in the IO registers so we can access them.
    //

    if ( ( hwDeviceExtension->IoRegBaseAddress = (ULONG)
              VideoPortGetDeviceBase(hwDeviceExtension,
                                     hwDeviceExtension->PhysicalIoRegBaseAddress,
                                     XGA_IO_REGS_SIZE,
                                     TRUE)) == 0) {

        VideoDebugPrint((2, "XgaFindAdapter - Fail to get io register addresses\n"));

        return ERROR_INVALID_PARAMETER;

    }

    //
    // Map in the pass-throught port.
    //

    if ( ( hwDeviceExtension->PassThroughPort =
              VideoPortGetDeviceBase(hwDeviceExtension,
                                     passThroughPort,
                                     1,
                                     TRUE)) == 0) {

        VideoDebugPrint((2, "XgaFindAdapter - Fail to get io register addresses\n"));

        return ERROR_INVALID_PARAMETER;

    }

    //
    // Indicate a successful completion status.
    //

    return NO_ERROR;

} // end XgaFindAdapter()


VP_STATUS
XgaGetPosData(
    PVOID HwDeviceExtension,
    PVOID Context,
    VIDEO_DEVICE_DATA_TYPE DeviceDataType,
    PVOID Identifier,
    ULONG IdentifierLength,
    PVOID ConfigurationData,
    ULONG ConfigurationDataLength,
    PVOID ComponentInformation,
    ULONG ComponentInformationLength
    )

/*++

Routine Description:

    Callback for the GetDeviceData function.
    This routine will scan for an XGA adapter, and if it finds one, will
    save the POS information in the device extension so it can be further
    processed in the FindAdapter() routine.

Arguments:

    HwDeviceExtension - Pointer to the miniport drivers device extension.

    Context - Context value passed to the VideoPortGetDeviceData function.

    DeviceDataType - The type of data that was requested in
        VideoPortGetDeviceData.

    Identifier - Pointer to a string that contains the name of the device,
        as setup by the ROM or ntdetect.

    IdentifierLength - Length of the Identifier string.

    ConfigurationData - Pointer to the configuration data for the device or
        BUS.

    ConfigurationDataLength - Length of the data in the configurationData
        field.

    ComponentInformation - Undefined.

    ComponentInformationLength - Undefined.

Return Value:

    Returns NO_ERROR if the function completed properly.
    Returns ERROR_DEV_NOT_EXIST if we did not find the device.
    Returns ERROR_INVALID_PARAMETER otherwise.

--*/

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    USHORT adapterId;
    BOOLEAN found = FALSE;
    PCM_MCA_POS_DATA posData = Context;

    //
    // Find the XGA.
    // Loop through all the POS data in the PosData array
    //

    while ((XgaSlot + 1) * sizeof(CM_MCA_POS_DATA) <= ConfigurationDataLength) {

        adapterId = *((PUSHORT)((PCM_MCA_POS_DATA)ConfigurationData + XgaSlot));

        VideoDebugPrint((3, "PosAdapterId for slot %d is %x \n", XgaSlot,
                         adapterId));

        switch ( adapterId ) {

        case 0x8FDB:

            hwDeviceExtension->BoardType = XGA_TYPE_1;

            found = TRUE;
            break;

        case 0x8FDA:

            hwDeviceExtension->BoardType = XGA_TYPE_2;

            found = TRUE;
            break;

        default:
            break;

        }

        //
        // Go to the next slot.
        //

        XgaSlot++;

        //
        // We have found an XGA adapter. Save the data in the device extension
        // so we can process it further.
        //

        if (found) {

            *posData = *((PCM_MCA_POS_DATA)ConfigurationData + XgaSlot - 1);

            return NO_ERROR;
        }
    }

    return ERROR_DEV_NOT_EXIST;

}

BOOLEAN
XgaInitialize(
    PVOID HwDeviceExtension
    )

/*++

Routine Description:

    This routine does one time initialization of the device.

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

Return Value:


    Always returns TRUE since this routine can never fail.

--*/

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    volatile PUCHAR videoMemory;
    UCHAR testValue = 0xA5;
    ULONG i;
    ULONG color;

    videoMemory = hwDeviceExtension->A0000MemoryAddress;

    //
    // Disable interrupts on the card since they will not be used
    //

    VideoPortWritePortUchar(
        (PUCHAR) (hwDeviceExtension->IoRegBaseAddress | INT_ENABLE_REG), 0x00);

    //
    // Blank the palette so we get a black screen.
    //

    VideoPortWritePortUchar(
        (PUCHAR) (hwDeviceExtension->IoRegBaseAddress | INDEX_REG), 0x64);

    VideoPortWritePortUchar(
        (PUCHAR) (hwDeviceExtension->IoRegBaseAddress | DATA_IN_REG), 0x00);

    //
    // Put the adapter in a temporary Extended Graphics Mode, set up video
    // memory at A0000, put the aperture at 768 K higher and try to access
    // it to determine how much memory is present.
    //

    VideoPortWritePortUchar(
        (PUCHAR) (hwDeviceExtension->IoRegBaseAddress | OP_MODE_REG), 0x04);

    VideoPortWritePortUchar(
        (PUCHAR) (hwDeviceExtension->IoRegBaseAddress | APP_CTL_REG), 0x01);

    VideoPortWritePortUchar(
        (PUCHAR) (hwDeviceExtension->IoRegBaseAddress | APP_INDEX_REG), 0x0C);

    //
    // To test if memory is present, use a test value (make sure the same
    // value is not currently stored in video memory, otherwise change it),
    // store it in video memory, and read it back. If the value is identical
    // to the value we stored then there must be memory present at that
    // location (which, in this case, means we have 1 MEG of Video RAM).
    //

    if (*videoMemory == testValue) {

        testValue >>= 1;

    }

    *videoMemory = testValue;

    if (*videoMemory == testValue) {

        hwDeviceExtension->PhysicalVideoMemoryLength = 0x00100000;

    } else {

        hwDeviceExtension->PhysicalVideoMemoryLength = 0x00080000;

    }

    VideoDebugPrint((2, "XgaInitialize\n  The amount of memory on the card is %d K\n",
                   hwDeviceExtension->PhysicalVideoMemoryLength >> 10));

    //
    // For now, we only support color devices
    //

    hwDeviceExtension->Color = TRUE;

    //
    // Now compute the number of available modes based on the infromation
    // we found out.
    //

    hwDeviceExtension->NumAvailableModes = 0;

    for (i = 0; i < XGA_NUM_MODES; i++) {

        //
        // If the right amount of memory and the right board type is present ...
        //

        if (XgaModes[i].minimumRequiredMemory <=
               hwDeviceExtension->PhysicalVideoMemoryLength) {

            //
            // Check we have the right type of board.
            //

            if ( (XgaModes[i].Xga1Mode &&
                     (hwDeviceExtension->BoardType & XGA_TYPE_1)) ||
                 (XgaModes[i].Xga2Mode &&
                     (hwDeviceExtension->BoardType & XGA_TYPE_2)) ) {

                color = XgaModes[i].modeInformation.AttributeFlags &
                        VIDEO_MODE_COLOR;

                //
                // check that both the mode and what we support is the same
                // in terms of color.
                //

                if ( (color && (ULONG)hwDeviceExtension->Color) ||  // color
                     (color == (ULONG)hwDeviceExtension->Color) ) { // monochrome

                    hwDeviceExtension->Valid[i] = TRUE;
                    hwDeviceExtension->NumAvailableModes++;

                }
            }
        }
    }

    VideoPortGetRegistryParameters(hwDeviceExtension,
                                   L"Monitor",
                                   TRUE,
                                   XgaGetRegistryParamaterCallback,
                                   NULL);

    return TRUE;

} // end XgaInitialize()


VP_STATUS
XgaGetRegistryParamaterCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )

/*++

Routine Description:

    Callback routine to process the information coming back from the registry.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    Context - Context parameter passed to the callback routine.

    ValueName - Pointer to the name of the requested data field.

    ValueData - Pointer to a buffer containing the information.

    ValueLength - Size of the data.

Return Value:


Environment:

    Can only be called During initialization.

--*/

{

    VideoDebugPrint((3, "In the GetRegistryParameters callback routine\n"));

    return NO_ERROR;

}


BOOLEAN
XgaStartIO(
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    )

/*++

Routine Description:

    This routine is the main execution routine for the miniport driver. It
    acceptss a Video Request Packet, performs the request, and then returns
    with the appropriate status.

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

    RequestPacket - Pointer to the video request packet. This structure
        contains all the parameters passed to the VideoIoControl function.

Return Value:


--*/

{
    VP_STATUS status;
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;

    PVIDEO_MEMORY_INFORMATION memoryInformation;
    PVIDEO_MODE_INFORMATION modeInformation;
    ULONG inIoSpace;

    PVIDEO_CLUT clutBuffer;
    PVIDEO_CLUTDATA pClutData;

    USHORT i;
    ULONG length;
    PHYSICAL_ADDRESS physicalAddress;
    ULONG offset;


    //
    // Switch on the IoContolCode in the RequestPacket. It indicates which
    // function must be performed by the driver.
    //

    switch (RequestPacket->IoControlCode) {


    case IOCTL_VIDEO_MAP_VIDEO_MEMORY:

        VideoDebugPrint((2, "XgaStartIO - MapVideoMemory\n"));

        if ( (RequestPacket->OutputBufferLength <
              (RequestPacket->StatusBlock->Information =
                                     sizeof(VIDEO_MEMORY_INFORMATION))) ||
             (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY)) ) {

            status = ERROR_INSUFFICIENT_BUFFER;
        }

        memoryInformation = RequestPacket->OutputBuffer;

        memoryInformation->VideoRamBase = ((PVIDEO_MEMORY)
                (RequestPacket->InputBuffer))->RequestedVirtualAddress;

        memoryInformation->VideoRamLength =
                hwDeviceExtension->PhysicalVideoMemoryLength;

        inIoSpace = 0;

        status = VideoPortMapMemory(hwDeviceExtension,
                                    hwDeviceExtension->PhysicalVideoMemoryAddress,
                                    &(memoryInformation->VideoRamLength),
                                    &inIoSpace,
                                    &(memoryInformation->VideoRamBase));

        //
        // The frame buffer and virtual memory and equivalent in this
        // case.
        //

        memoryInformation->FrameBufferBase =
            memoryInformation->VideoRamBase;

        memoryInformation->FrameBufferLength =
            XgaModes[hwDeviceExtension->CurrentMode].modeInformation.VisScreenHeight *
            XgaModes[hwDeviceExtension->CurrentMode].modeInformation.ScreenStride;

        VideoDebugPrint((3, "Xga IOCLT_MAP_MEMORY\n physical = %x\n \
virtual = %x\n length = %x\n framebase = %x\n frameLength = %x\n",
                           hwDeviceExtension->PhysicalVideoMemoryAddress.LowPart,
                           memoryInformation->VideoRamBase,
                           memoryInformation->VideoRamLength,
                           memoryInformation->FrameBufferBase,
                           memoryInformation->FrameBufferLength));

        break;


    case IOCTL_VIDEO_UNMAP_VIDEO_MEMORY:

        VideoDebugPrint((2, "XgaStartIO - UnMapVideoMemory\n"));

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY)) {

            status = ERROR_INSUFFICIENT_BUFFER;
        }

        status = VideoPortUnmapMemory(hwDeviceExtension,
                                      ((PVIDEO_MEMORY)
                                       (RequestPacket->InputBuffer))->
                                           RequestedVirtualAddress,
                                      0);
        break;


    case IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES:

        VideoDebugPrint((2, "XgaStartIO - QueryNumberAvaialbleModes\n"));

        if (RequestPacket->OutputBufferLength <
            (RequestPacket->StatusBlock->Information =
                sizeof(VIDEO_NUM_MODES)) ) {

            status = ERROR_INSUFFICIENT_BUFFER;

        } else {

            ((PVIDEO_NUM_MODES)RequestPacket->OutputBuffer)->NumModes =
                hwDeviceExtension->NumAvailableModes;
            ((PVIDEO_NUM_MODES)RequestPacket->OutputBuffer)->ModeInformationLength =
                sizeof(VIDEO_MODE_INFORMATION);

            status = NO_ERROR;
        }

        break;


    case IOCTL_VIDEO_QUERY_AVAIL_MODES:

        VideoDebugPrint((2, "XgaStartIO - QueryAvailableModes\n"));

        if (RequestPacket->OutputBufferLength <
            (RequestPacket->StatusBlock->Information =
                 hwDeviceExtension->NumAvailableModes *
                 sizeof(VIDEO_MODE_INFORMATION)) ) {

            status = ERROR_INSUFFICIENT_BUFFER;

        } else {

            modeInformation = RequestPacket->OutputBuffer;

            for (i = 0; i < XGA_NUM_MODES; i++) {

                if (hwDeviceExtension->Valid[i]) {

                    *modeInformation = XgaModes[i].modeInformation;
                    modeInformation++;

                }
            }

            status = NO_ERROR;
        }

        break;


    case IOCTL_VIDEO_QUERY_CURRENT_MODE:

        VideoDebugPrint((2, "XgaStartIO - QueryCurrentMode\n"));

        if (RequestPacket->OutputBufferLength <
            (RequestPacket->StatusBlock->Information =
            sizeof(VIDEO_MODE_INFORMATION)) ) {

            status = ERROR_INSUFFICIENT_BUFFER;

        } else {

            *((PVIDEO_MODE_INFORMATION)RequestPacket->OutputBuffer) =
                XgaModes[hwDeviceExtension->CurrentMode].modeInformation;

            status = NO_ERROR;

        }

        break;






    case IOCTL_VIDEO_SET_CURRENT_MODE:

        VideoDebugPrint((2, "XgaStartIO - SetCurrentMode\n"));

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_MODE)) {

            status = ERROR_INSUFFICIENT_BUFFER;

        } else {

            status = XgaSetMode(hwDeviceExtension,
                                ((PVIDEO_MODE)RequestPacket->InputBuffer)->
                                RequestedMode);

        }

        break;


    case IOCTL_VIDEO_SET_COLOR_REGISTERS:

        VideoDebugPrint((2, "XgaStartIO - SetColorRegs\n"));

        clutBuffer = RequestPacket->InputBuffer;

        /*   Load simple 16 entry palette */

        XGAIDXOUT(INDEX_REG, 0x66, 0x0) ;

        /* Start at beginning of palette */

        XGAIDXOUT(INDEX_REG, 0x60, clutBuffer->FirstEntry) ;
        XGAIDXOUT(INDEX_REG, 0x61, 0x0) ;


        for ( i = 0 ; i < clutBuffer->NumEntries; i++ ) {

           pClutData = &clutBuffer->LookupTable[i].RgbArray ;

           XGAOUT(INDEX_REG, 0x65) ;
           XGAOUT(0x0B, pClutData->Red) ;

           XGAOUT(INDEX_REG, 0x65) ;
           XGAOUT(0x0B, pClutData->Green) ;

           XGAOUT(INDEX_REG, 0x65) ;
           XGAOUT(0x0B, pClutData->Blue) ;
        }

        status = NO_ERROR;

        break;

    case IOCTL_VIDEO_XGA_MAP_COPROCESSOR:

        if (RequestPacket->OutputBufferLength <
            (RequestPacket->StatusBlock->Information =
            sizeof(VIDEO_XGA_COPROCESSOR_INFORMATION)) ) {

            status = ERROR_INSUFFICIENT_BUFFER;
            break;

        }

        length = XGA_CO_PROCESSOR_REGS_SIZE;

        // Map in at the page ganularity, then add in the page offset
        // to the linear address.

        offset = hwDeviceExtension->PhysicalCoProcessorAddress.LowPart & 0x00000FFF ;

        physicalAddress.LowPart =
            hwDeviceExtension->PhysicalCoProcessorAddress.LowPart & 0xFFFFF000 ;

        physicalAddress.HighPart = 0x00000000;

        ((PVIDEO_XGA_COPROCESSOR_INFORMATION)RequestPacket->OutputBuffer)->
            CoProcessorVirtualAddress = 0;

        inIoSpace = 0;

        status = VideoPortMapMemory(hwDeviceExtension,
                                    physicalAddress,
                                    &length,
                                    &inIoSpace,
                                    &((PVIDEO_XGA_COPROCESSOR_INFORMATION)RequestPacket->
                                        OutputBuffer)->CoProcessorVirtualAddress);

        (PCHAR) ((PVIDEO_XGA_COPROCESSOR_INFORMATION)
             RequestPacket->OutputBuffer)->CoProcessorVirtualAddress += offset;

        //
        // Also return the physical address of the video memory so the
        // co-processor can access it.
        //


        (ULONG) ((PVIDEO_XGA_COPROCESSOR_INFORMATION)
            RequestPacket->OutputBuffer)->PhysicalVideoMemoryAddress =
                hwDeviceExtension->PhysicalVideoMemoryAddress.LowPart;

        //
        // Also return the XGA IO Register Base Address.
        //

        (ULONG) ((PVIDEO_XGA_COPROCESSOR_INFORMATION)
            RequestPacket->OutputBuffer)->XgaIoRegisterBaseAddress =
                hwDeviceExtension->PhysicalIoRegBaseAddress.LowPart;


        VideoDebugPrint((3, "Xga IOCLT_MAP_CO_PROCESSOR\n physicalco = %x\n virtualco = %x\n physicalmem = %x\n",
                         hwDeviceExtension->PhysicalCoProcessorAddress.LowPart,
                         ((PVIDEO_XGA_COPROCESSOR_INFORMATION)
                             RequestPacket->OutputBuffer)->CoProcessorVirtualAddress,
                         hwDeviceExtension->PhysicalVideoMemoryAddress.LowPart));

        break ;


    case IOCTL_VIDEO_RESET_DEVICE:

        XGAOUT(APP_CTL_REG, 0x00) ;

        XGAOUT(INT_ENABLE_REG, 0x00) ;

        XGAOUT(INT_STATUS_REG, 0xff) ;

        // Now init all the index registers.

        for (i = 0 ; XgaResetToVga[i].PortIndex != 0xff ; i++) {

            XGAIDXOUT(0x0A,
                      XgaResetToVga[i].PortIndex,
                      XgaResetToVga[i].Data) ;
        }

        XGAOUT(OP_MODE_REG, 0x1) ;

        VideoPortWritePortUchar(hwDeviceExtension->PassThroughPort, 0x01);

        status = NO_ERROR;

        break ;

    //
    // if we get here, an invalid IoControlCode was specified.
    //

    default:

        VideoDebugPrint((1, "Fell through Xga startIO routine - invalid command\n"));

        status = ERROR_INVALID_FUNCTION ;

        break;

    }

    RequestPacket->StatusBlock->Status = status;

    return TRUE;

} // end XgaStartIO()


VP_STATUS
XgaSetMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    ULONG ModeNum
    )

/*++

Routine Description:

    This routine is the main execution routine for the miniport driver. It
    acceptss a Video Request Packet, performs the request, and then returns
    with the appropriate status.

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

    ModeNum - Index of the mode in which the adapter must be programmed.

Return Value:

    NO_ERROR if the function completed successfully
    ERROR_INVALID_PARAMETER if the requested mode is invalid for the device.

--*/

{
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    PUCHAR indexRegister = (PUCHAR) (hwDeviceExtension->IoRegBaseAddress + INDEX_REG);
    PMODE_REGISTER_DATA_TABLE modeTable;
    UCHAR input;

    VideoDebugPrint((1, "XgaSetMode: the mode being set is %d\n", ModeNum));

    //
    // Get the right mode table
    //

    if (hwDeviceExtension->BoardType & XGA_TYPE_1) {

        modeTable = XgaModes[ModeNum].Xga1Mode;

    } else {

        if (hwDeviceExtension->BoardType & XGA_TYPE_2) {

            modeTable = XgaModes[ModeNum].Xga2Mode;

        } else {

            VideoDebugPrint((0, "XGA: Internal mode flags are invalid"));
            return ERROR_INVALID_PARAMETER;

        }
    }

    //
    // Program the registers.
    //

    while (modeTable->Port != END_OF_SWITCH) {

        switch (modeTable->Port) {

        case INDEX_REG:

            VideoPortWritePortUchar(indexRegister,
                                    modeTable->IndexPort);

            VideoPortWritePortUchar(indexRegister + 1,
                                    modeTable->Data);

            break;

        case INDEX_OR_REG:

            VideoPortWritePortUchar(indexRegister,
                                    modeTable->IndexPort);

            input = VideoPortReadPortUchar((PUCHAR) hwDeviceExtension->IoRegBaseAddress +
                                           DATA_IN_REG);

            input |= modeTable->Data;

            VideoPortWritePortUchar(indexRegister + 1, input);

            break;

        default:

            VideoPortWritePortUchar((PUCHAR) hwDeviceExtension->IoRegBaseAddress +
                                        modeTable->Port,
                                    modeTable->Data);

        }

        modeTable++;

    }

    //
    // Save the current mode for future reference.
    //

    hwDeviceExtension->CurrentMode = (UCHAR) ModeNum;

    return NO_ERROR;

} // end XgaSetMode();
