/*++

Copyright (c) 1990-1993  Microsoft Corporation

Module Name:

    s3.c

Abstract:

    This module contains the code that implements the S3 miniport driver.

Environment:

    Kernel mode

Revision History:

--*/

#include "s3.h"
#include "s3data.h"
#include "s3logerr.h"

#include <string.h>


#define MAX_CONTROL_HISTORY 512

ULONG giControlCode;
ULONG gaIOControlCode[MAX_CONTROL_HISTORY];

#ifdef i386

/*************************************************************************************************
 * 801/805, 928 mode mask bit table.
 ************************************************************************************************/
UCHAR   ModeMaskBits[] = {(UCHAR) ~0x00,    // 640 X 480   DEFAULT
                          (UCHAR) ~0x03,    // 640 X 480 @ 43Hz
                          (UCHAR) ~0x03,    // 640 X 480 @ 56Hz
                          (UCHAR) ~0x03,    // 640 X 480 @ 60Hz
                          (UCHAR) ~0x03,    // 640 X 480 @ 72Hz

                          (UCHAR) ~0x00,    // 800 X 600   DEFAULT
                          (UCHAR) ~0x0C,    // 800 X 600 @ 43Hz
                          (UCHAR) ~0x0C,    // 800 X 600 @ 56Hz
                          (UCHAR) ~0x0C,    // 800 X 600 @ 60Hz
                          (UCHAR) ~0x0C,    // 800 X 600 @ 72Hz

                          (UCHAR) ~0x00,    // 1024 X 768   DEFAULT
                          (UCHAR) ~0x30,    // 1024 X 768 @ 43Hz
                          (UCHAR) ~0x30,    // 1024 X 768 @ 56Hz
                          (UCHAR) ~0x30,    // 1024 X 768 @ 60Hz
                          (UCHAR) ~0x30,    // 1024 X 768 @ 72Hz

                          (UCHAR) ~0x00,    // 1280 X 1024   DEFAULT
                          (UCHAR) ~0xC0,    // 1280 X 1024 @ 43Hz
                          (UCHAR) ~0xC0,    // 1280 X 1024 @ 56Hz
                          (UCHAR) ~0xC0,    // 1280 X 1024 @ 60Hz
                          (UCHAR) ~0xC0     // 1280 X 1024 @ 72Hz
};

/*************************************************************************************************
 * 801/805, 928 mode set bits table
 ************************************************************************************************/
UCHAR   GenericModeBits[] = { (UCHAR) 0x00,    // 640 X 480   DEFAULT
                          (UCHAR) 0x00,    // 640 X 480 @ 43Hz
                          (UCHAR) 0x01,    // 640 X 480 @ 56Hz
                          (UCHAR) 0x02,    // 640 X 480 @ 60Hz
                          (UCHAR) 0x03,    // 640 X 480 @ 72Hz

                          (UCHAR) 0x00,    // 800 X 600   DEFAULT
                          (UCHAR) 0x00,    // 800 X 600 @ 43Hz
                          (UCHAR) 0x04,    // 800 X 600 @ 56Hz
                          (UCHAR) 0x08,    // 800 X 600 @ 60Hz
                          (UCHAR) 0x0C,    // 800 X 600 @ 72Hz

                          (UCHAR) 0x00,    // 1024 X 768   DEFAULT
                          (UCHAR) 0x00,    // 1024 X 768 @ 43Hz
                          (UCHAR) 0x10,    // 1024 X 768 @ 56Hz
                          (UCHAR) 0x20,    // 1024 X 768 @ 60Hz
                          (UCHAR) 0x30,    // 1024 X 768 @ 72Hz

                          (UCHAR) 0x00,    // 1280 X 1024   DEFAULT
                          (UCHAR) 0x00,    // 1280 X 1024 @ 43Hz
                          (UCHAR) 0x40,    // 1280 X 1024 @ 56Hz
                          (UCHAR) 0x80,    // 1280 X 1024 @ 60Hz
                          (UCHAR) 0xC0     // 1280 X 1024 @ 72Hz
};

/*************************************************************************************************
 * Number Nine Computer 928 mode set bits table
 *  BIOS versions before 1.10.04 have the following refresh index to
 *  vertical refresh rate association:
 *
 *      0   default (56/60 depending upon mode)
 *      1   70 Hz
 *      2   72 Hz
 *      3   76 Hz
 *
 *  This miniport will always map down (better safe than sorry)
 *  The mapping is:
 *
 *  index   registry value     #9 value
 *    0         43          56/60 (default)
 *    1         56              "
 *    2         60              "
 *    3         72              72
 ************************************************************************************************/
UCHAR   NumberNineModeBits[] = { (UCHAR) 0x00,    // 640 X 480   DEFAULT
                          (UCHAR) 0x00,    // 640 X 480 @ 43Hz
                          (UCHAR) 0x00,    // 640 X 480 @ 56Hz
                          (UCHAR) 0x00,    // 640 X 480 @ 60Hz
                          (UCHAR) 0x02,    // 640 X 480 @ 72Hz

                          (UCHAR) 0x00,    // 800 X 600   DEFAULT
                          (UCHAR) 0x00,    // 800 X 600 @ 43Hz
                          (UCHAR) 0x00,    // 800 X 600 @ 56Hz
                          (UCHAR) 0x00,    // 800 X 600 @ 60Hz
                          (UCHAR) 0x08,    // 800 X 600 @ 72Hz

                          (UCHAR) 0x00,    // 1024 X 768   DEFAULT
                          (UCHAR) 0x00,    // 1024 X 768 @ 43Hz
                          (UCHAR) 0x00,    // 1024 X 768 @ 56Hz
                          (UCHAR) 0x00,    // 1024 X 768 @ 60Hz
                          (UCHAR) 0x20,    // 1024 X 768 @ 72Hz

                          (UCHAR) 0x00,    // 1280 X 1024   DEFAULT
                          (UCHAR) 0x00,    // 1280 X 1024 @ 43Hz
                          (UCHAR) 0x00,    // 1280 X 1024 @ 56Hz
                          (UCHAR) 0x00,    // 1280 X 1024 @ 60Hz
                          (UCHAR) 0x80     // 1280 X 1024 @ 72Hz
};

/*************************************************************************************************
 * DELL 805 mode set bits table
 *
 *  Dell has a different mapping for each resolution.
 *
 *  index   registry    640     800     1024    1280
 *    0         43       60      56      43      43
 *    1         56       72      60      60      --
 *    2         60       --      72      70      --
 *    3         72       56      56      72      --
 ************************************************************************************************/
UCHAR   DellModeBits[] = {(UCHAR) 0x00,    // 640 X 480   DEFAULT
                          (UCHAR) 0x00,    // 640 X 480 @ 43Hz
                          (UCHAR) 0x03,    // 640 X 480 @ 56Hz
                          (UCHAR) 0x00,    // 640 X 480 @ 60Hz
                          (UCHAR) 0x01,    // 640 X 480 @ 72Hz

                          (UCHAR) 0x00,    // 800 X 600   DEFAULT
                          (UCHAR) 0x00,    // 800 X 600 @ 43Hz
                          (UCHAR) 0x00,    // 800 X 600 @ 56Hz
                          (UCHAR) 0x04,    // 800 X 600 @ 60Hz
                          (UCHAR) 0x08,    // 800 X 600 @ 72Hz

                          (UCHAR) 0x00,    // 1024 X 768   DEFAULT
                          (UCHAR) 0x00,    // 1024 X 768 @ 43Hz
                          (UCHAR) 0x10,    // 1024 X 768 @ 56Hz
                          (UCHAR) 0x10,    // 1024 X 768 @ 60Hz
                          (UCHAR) 0x30,    // 1024 X 768 @ 72Hz

                          (UCHAR) 0x00,    // 1280 X 1024   DEFAULT
                          (UCHAR) 0x00,    // 1280 X 1024 @ 43Hz
                          (UCHAR) 0x00,    // 1280 X 1024 @ 56Hz
                          (UCHAR) 0x00,    // 1280 X 1024 @ 60Hz
                          (UCHAR) 0x00     // 1280 X 1024 @ 72Hz
};


// This table is only used for the 801/805 and 928 chips.
// When this table was used on the Stealth or Orchid 911 boards the
// full screen had two images side by side.

typedef struct {
    UCHAR   port,
            val;
} REGVALS;

REGVALS   arvExtendedRegs[] = {{0x30, 0x0},
                               {0x31, 0x0},
                               {0x32, 0x0},
                               {0x33, 0x0},
                               {0x34, 0x0},
                               {0x35, 0x0},
                               {0x36, 0x0},
                               {0x37, 0x0},
                               {0x3A, 0x0},
                               {0x3B, 0x0},
                               {0x3C, 0x0},

                               {0x40, 0x0},
                               {0x41, 0x0},
                               {0x42, 0x0},
                               {0x43, 0x0},
                               {0x44, 0x0},
                               {0x45, 0x0},
                               {0x46, 0x0},
                               {0x47, 0x0},
                               {0x48, 0x0},
                               {0x49, 0x0},
                               {0x4A, 0x0},
                               {0x4B, 0x0},
                               {0x4C, 0x0},
                               {0x4D, 0x0},
                               {0x4E, 0x0},
                               {0x4F, 0x0},

                               {0x50, 0x0},
                               {0x51, 0x0},
                               {0x52, 0x0},
                               {0x53, 0x0},
                               {0x54, 0x0},
                               {0x55, 0x0},
                               {0x56, 0x0},
                               {0x57, 0x0},
                               {0x58, 0x0},
                               {0x59, 0x0},
                               {0x5A, 0x0},
                               {0x5B, 0x0},
                               {0x5C, 0x0},
                               {0x5D, 0x0},
                               {0x5E, 0x0},
                               {0x5F, 0x0}};

ULONG nExtendedRegs = sizeof(arvExtendedRegs) / sizeof (REGVALS);

#endif



#ifndef i386

long calc_clock(long, int);
VOID set_clock(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    LONG clock_value);

#endif


//
// Function Prototypes
//
// Functions that start with 'S3' are entry points for the OS port driver.
//

VP_STATUS
S3FindAdapter(
    PVOID HwDeviceExtension,
    PVOID HwContext,
    PWSTR ArgumentString,
    PVIDEO_PORT_CONFIG_INFO ConfigInfo,
    PUCHAR Again
    );

BOOLEAN
S3Initialize(
    PVOID HwDeviceExtension
    );

BOOLEAN
S3StartIO(
    PVOID HwDeviceExtension,
    PVIDEO_REQUEST_PACKET RequestPacket
    );

VP_STATUS
S3SetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    );

VOID
ZeroMemAndDac(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

VOID
SetHWMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PUSHORT pusCmdStream
    );

VP_STATUS
S3RegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

BOOLEAN
Reset_Bt485(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

//
// Non-x86 platform support
//

#ifndef i386

VP_STATUS
Set_Oem_Clock(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

VP_STATUS
Wait_VSync(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

BOOLEAN
Bus_Test(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );

#endif // i386


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
    ULONG initializationStatus;
    ULONG status;

    //
    // Zero out structure.
    //

    VideoPortZeroMemory(&hwInitData, sizeof(VIDEO_HW_INITIALIZATION_DATA));

    //
    // Specify sizes of structure and extension.
    //

    hwInitData.HwInitDataSize = sizeof(VIDEO_HW_INITIALIZATION_DATA);

    //
    // Set entry points.
    //

    hwInitData.HwFindAdapter = S3FindAdapter;
    hwInitData.HwInitialize = S3Initialize;
    hwInitData.HwInterrupt = NULL;
    hwInitData.HwStartIO = S3StartIO;

    //
    // Determine the size we require for the device extension.
    //

    hwInitData.HwDeviceExtensionSize = sizeof(HW_DEVICE_EXTENSION);

    //
    // Always start with parameters for device0 in this case.
    //

//    hwInitData.StartingDeviceNumber = 0;

    //
    // This device only supports many bus types.
    //

    hwInitData.AdapterInterfaceType = Isa;

    initializationStatus = VideoPortInitialize(Context1,
                                               Context2,
                                               &hwInitData,
                                               NULL);

    hwInitData.AdapterInterfaceType = Eisa;

    status = VideoPortInitialize(Context1,
                                     Context2,
                                     &hwInitData,
                                     NULL);

    if (initializationStatus > status) {
        initializationStatus = status;
    }

    hwInitData.AdapterInterfaceType = Internal;

    status = VideoPortInitialize(Context1,
                                 Context2,
                                 &hwInitData,
                                 NULL);

    if (initializationStatus > status) {
        initializationStatus = status;
    }

    return initializationStatus;

} // end DriverEntry()


VP_STATUS
S3FindAdapter(
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
    // Size of the ROM we map in
    //

    #define MAX_ROM_SCAN    512

    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    ULONG i;
    VP_STATUS status;
    UCHAR jChipID, s3MemSizeCode;
    UCHAR DataReg, IndexReg, reg38, reg39;
    UCHAR reg40, reg43;
    BOOLEAN useAlternateRegisters = FALSE;

    USHORT usRomSignature;
    PVOID romAddress;
    BOOLEAN bMatch;
    UCHAR   *pjBiosVersion, *pjRefString, offset;
    LONG    iCmpRet;

    VIDEO_ACCESS_RANGE accessRange[] = {
        {0x000C0000, 0x00000000, 0x00008000, 0, 0, 0}, // 0 ROM location
        {0x000A0000, 0x00000000, 0x00010000, 0, 0, 1}, // 1 Frame buf
        {0x000003C0, 0x00000000, 0x00000010, 1, 1, 1}, // 2 Various VGA regs
        {0x000003D4, 0x00000000, 0x00000008, 1, 1, 1}, // 3 System Control Registers
        {0x000042E8, 0x00000000, 0x00000002, 1, 1, 0}, // 4 SubSys-Stat/Cntl
        {0x00004AE8, 0x00000000, 0x00000002, 1, 1, 0}, // 5 AdvFunc-Cntl
        {0x000082E8, 0x00000000, 0x00000002, 1, 1, 0}, // 6 Cur-Y
        {0x000086E8, 0x00000000, 0x00000002, 1, 1, 0}, // 7 Cur-X
        {0x00008AE8, 0x00000000, 0x00000002, 1, 1, 0}, // 8 DestY-AxStp
        {0x00008EE8, 0x00000000, 0x00000002, 1, 1, 0}, // 9 DestX-SiaStp
        {0x000092E8, 0x00000000, 0x00000002, 1, 1, 0}, // 10 Err-Term
        {0x000096E8, 0x00000000, 0x00000002, 1, 1, 0}, // 11 Maj-Axis-Pcnt(Rec-Width)
        {0x00009AE8, 0x00000000, 0x00000002, 1, 1, 0}, // 12 Gp-Stat/Cmd
        {0x00009EE8, 0x00000000, 0x00000002, 1, 1, 0}, // 13 Short-Stroke
        {0x0000A2E8, 0x00000000, 0x00000002, 1, 1, 0}, // 14 Bkgd-Color
        {0x0000A6E8, 0x00000000, 0x00000002, 1, 1, 0}, // 15 Frgd-Color
        {0x0000AAE8, 0x00000000, 0x00000002, 1, 1, 0}, // 16 Wrt_Mask
        {0x0000AEE8, 0x00000000, 0x00000002, 1, 1, 0}, // 17 Rd-Mask
        {0x0000B6E8, 0x00000000, 0x00000002, 1, 1, 0}, // 18 Bkgd-Mix
        {0x0000BAE8, 0x00000000, 0x00000002, 1, 1, 0}, // 19 Frgd-Mix
        {0x0000BEE8, 0x00000000, 0x00000002, 1, 1, 0}, // 20 Mulitfucn_Cntl
        {0x0000E2E8, 0x00000000, 0x00000002, 1, 1, 0}  // 21 Pix-Trans
    };

    //
    // Make sure the size of the structure is at least as large as what we
    // are expecting (check version of the config info structure).
    //

    if (ConfigInfo->Length < sizeof(VIDEO_PORT_CONFIG_INFO)) {

        return (ERROR_INVALID_PARAMETER);

    }


#ifdef MIPS

    //
    // For MIPS machine with an Internal Bus, adjust the access ranges.
    //

    if (ConfigInfo->AdapterInterfaceType == Internal) {

        #define INTERNAL_BUS_VIDEO_MEMORY_BASE 0x40000000
        #define INTERNAL_BUS_IO_PORT_BASE      0x60000000

        VideoDebugPrint((1, "S3: Internal Bus, get new IO bases\n"));

        //
        // Adjust memory location
        //

        accessRange[0].RangeStart.LowPart += INTERNAL_BUS_VIDEO_MEMORY_BASE;
        accessRange[1].RangeStart.LowPart += INTERNAL_BUS_VIDEO_MEMORY_BASE;

        //
        // Adjust io port locations, and change IO port from IO port to memory.
        //

        for (i = 2; i < NUM_S3_ACCESS_RANGES; i++) {

            accessRange[i].RangeStart.LowPart += INTERNAL_BUS_IO_PORT_BASE;
            accessRange[i].RangeInIoSpace = 0;

        }
    }

#endif


    //
    // Check to see if there is a hardware resource conflict.
    //

    status = VideoPortVerifyAccessRanges(hwDeviceExtension,
                                         NUM_S3_ACCESS_RANGES,
                                         accessRange);

    if (status != NO_ERROR) {

        VideoDebugPrint((1, "S3: Access Range conflict\n"));
        return status;

    }

    //
    // Get the mapped addresses for all the registers.
    //
    // NOTE: the ROM is not mapped here. It will only be mapped later, if
    // we really need it (non int10 initialization).
    //

    for (i = 1; i < NUM_S3_ACCESS_RANGES; i++) {

        if ( (hwDeviceExtension->MappedAddress[i] =
                  VideoPortGetDeviceBase(hwDeviceExtension,
                                         accessRange[i].RangeStart,
                                         accessRange[i].RangeLength,
                                         accessRange[i].RangeInIoSpace)) == NULL) {

            VideoDebugPrint((1, "S3: DeviceBase mapping failed\n"));
            return ERROR_INVALID_PARAMETER;

        }
    }

    //
    // Save the initial value of the S3 lock registers.
    // it's possible, a non-s3 bios may expect them in a state
    // define in POST.
    //

    DataReg = VideoPortReadPortUchar(CRT_DATA_REG);
    IndexReg = VideoPortReadPortUchar(CRT_ADDRESS_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x38);
    reg38 = VideoPortReadPortUchar(CRT_DATA_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x39);
    reg39 = VideoPortReadPortUchar(CRT_DATA_REG);

    //
    // Now unlock all the S3 registers, for use in this routine.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x4838);
    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0xA039);

    //
    // Make sure we're working with an S3
    // And while were at it, pickup the chip ID
    //

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x30);
    jChipID = VideoPortReadPortUchar(CRT_DATA_REG);

    //
    // In this driver we just care about the major chip ID, not the revision
    //

    jChipID &= 0xf0;

    switch(jChipID) {

        case 0x80: // 911 or 924

            VideoDebugPrint((2, "S3: 911 Chip Set\n"));
            hwDeviceExtension->ChipID = S3_911;
            break;


        case 0x90: // 928

            VideoDebugPrint((2, "S3: 928 Chip Set\n"));
            hwDeviceExtension->ChipID = S3_928;
            break;

        case 0xA0: // 801/805

            VideoDebugPrint((2, "S3: 801/805 Chip Set\n"));
            hwDeviceExtension->ChipID = S3_801;
            break;

        default:

            VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg38 << 8) | 0x38));
            VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg39 << 8) | 0x39));
            VideoPortWritePortUchar(CRT_ADDRESS_REG, IndexReg);
            VideoPortWritePortUchar(CRT_DATA_REG, DataReg);

            //
            // Log an error to the event log. The unique Id of 0 is only used
            // to identify which call to VideoPortLogError actually generated
            // the message; a line # would also be appropriate.
            //

            //
            // NOTE
            // This message has been removed because we hit it each time an
            // S3 is not detected.
            //

            // VideoPortLogError(hwDeviceExtension,
            //                  NULL,
            //                  S3_UNSUPPORTED_CHIP,
            //                  __LINE__);

            VideoDebugPrint((2, "S3: unsupported chip set\n"));
            return ERROR_DEV_NOT_EXIST;
            break;

    }

    //
    // At this point we know we have an S3 chip in the system.
    // Set the defaults for the board type.
    //

    hwDeviceExtension->BoardID = S3_GENERIC;

    //
    // Get the Size the Video Memory.
    //

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x36);
    s3MemSizeCode = VideoPortReadPortUchar(CRT_DATA_REG) >> 5;

    if (hwDeviceExtension->ChipID == S3_911) {

        //
        // This is for the 911 & 924
        //

        if (s3MemSizeCode & 0x01) {

            VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg38 << 8) | 0x38));
            VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg39 << 8) | 0x39));
            VideoPortWritePortUchar(CRT_ADDRESS_REG, IndexReg);

            return ERROR_DEV_NOT_EXIST;

        } else {

            hwDeviceExtension->AdapterMemorySize = 0x00100000;

        }

    } else {

        //
        // This is for the 801/805, and 928.
        // If an invalid size or there is less than 1 meg on the card fail.
        //

        switch (s3MemSizeCode) {

        case 0x0:   // 4 meg

            hwDeviceExtension->AdapterMemorySize = 0x00400000;
            break;

        case 0x2:   // 3 meg

            hwDeviceExtension->AdapterMemorySize = 0x00300000;
            break;

        case 0x4:   // 2 meg

            hwDeviceExtension->AdapterMemorySize = 0x00200000;
            break;

        case 0x6:   // 1 meg

            hwDeviceExtension->AdapterMemorySize = 0x00100000;
            break;

        case 0x1:   // invalid
        case 0x3:   // invalid
        case 0x5:   // invalid
        case 0x7:   // 1/2 meg
        default:

            VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg38 << 8) | 0x38));
            VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg39 << 8) | 0x39));
            VideoPortWritePortUchar(CRT_ADDRESS_REG, IndexReg);

            VideoPortLogError(hwDeviceExtension,
                              NULL,
                              S3_NOT_ENOUGH_VRAM,
                              __LINE__);

            return ERROR_INVALID_PARAMETER;

        }
    }

    //
    // From the size of video memory calculate which modes are valid.
    //

    hwDeviceExtension->NumAvailableModes = 0;

    for (i=0; i < NumS3VideoModes; i++) {

        //
        // Set the mode so it does not have to be hardcoded in the mode table.
        //

        S3Modes[i].ModeInformation.ModeIndex = i;

        //
        // Set the mode to valid if it is.
        //

        if (S3Modes[i].RequiredVideoMemory <=
            hwDeviceExtension->AdapterMemorySize) {

#ifdef i386
            if (S3Modes[i].Int10ModeNumber) {
#else
            if (S3Modes[i].CRTCTables[hwDeviceExtension->ChipID]) {
#endif

                S3Modes[i].ModeValid = TRUE;
                hwDeviceExtension->NumAvailableModes++;
            }
        }
    }

    //
    // Check if we need to remap the registers of the S3 due to a register
    // contention with the COM3 COM4.
    //
    // Allow allow for override to use alternate register set from the
    // registry.
    //

    if (NO_ERROR == VideoPortGetRegistryParameters(hwDeviceExtension,
                                                   L"UseAlternateRegisterSet",
                                                   FALSE,
                                                   S3RegistryCallback,
                                                   NULL)) {

        useAlternateRegisters = TRUE;

    }

    //
    // This assumes the S3 registers are unlocked.
    //

    //
    // Get the original register values.
    //

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x40);
    reg40 = VideoPortReadPortUchar(CRT_DATA_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x43);
    reg43 = VideoPortReadPortUchar(CRT_DATA_REG);

//
// Using the BIOS and registers to detect conflict does appear to work *ONLY*
// on x86 based machines
//
// We will keep the manual override (above) for everyone, just in case ....
//

#ifdef i386

    //
    // The 801/805 and 928 allow quesrying of the extended registers to
    // determine if the registers should be remapped
    //

    switch(hwDeviceExtension->ChipID) {

    case S3_801:
    case S3_928:

        {
        LONG i;

        //
        // Disable the enhanced registers.
        //

        VideoPortWritePortUshort(CRT_ADDRESS_REG,
                                 ((USHORT)(((USHORT) (reg40 & ~0x01) << 8) | 0x40)));

        //
        // Make sure there is no remapping.
        //

        reg43 &= ~0x10;

        VideoPortWritePortUshort(CRT_ADDRESS_REG,
                                 ((USHORT)(((USHORT) reg43 << 8) | 0x43)));

        //
        // Read each of the enhanced registers.  If any of them do not
        // come back 0xff, then we have an I/O conflict, and we should remap
        // the I/O registers, by setting bit 4 of register 0x43.
        //

        for (i = S3_EXTENDED_RANGE_START; i < NUM_S3_ACCESS_RANGES; i++) {

            if (VideoPortReadPortUshort(
                    (PUSHORT) hwDeviceExtension->MappedAddress[i]) != 0xFFFF) {

                //
                // We have detected an I/O conflict,
                // So we must remap the registers.
                //

                useAlternateRegisters = TRUE;

                break;

            }
        }

        break;
        }

    default:

        break;

    }

#endif

    //
    // If we do use the alternate, update all of our resources.
    //

    if (useAlternateRegisters) {

        VideoDebugPrint((1, "S3: S3 registers relocated to support COM3 COM4\n"));

        //
        // Calculate the updated value for reg 43.
        //

        reg43 |= 0x10;

        for (i = 1; i < NUM_S3_ACCESS_RANGES; i++) {

            //
            // First free the mapped address.
            //

            VideoPortFreeDeviceBase(hwDeviceExtension,
                                    hwDeviceExtension->MappedAddress[i]);

            //
            // Get the new address (by XORing 3A0)
            //

            accessRange[i].RangeStart.LowPart ^= 0x3A0;

            //
            // Map the new address.
            //

            if ( (hwDeviceExtension->MappedAddress[i] =
                      VideoPortGetDeviceBase(hwDeviceExtension,
                                             accessRange[i].RangeStart,
                                             accessRange[i].RangeLength,
                                             accessRange[i].RangeInIoSpace)) == NULL) {

                return ERROR_INVALID_PARAMETER;

            }
        }
    }

    //
    // Reset regs to the original (or modified) value.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG,
                             ((USHORT)(((USHORT) reg43 << 8) | 0x43)));

    VideoPortWritePortUshort(CRT_ADDRESS_REG,
                             ((USHORT)(((USHORT) reg40 << 8) | 0x40)));


    //
    // Were done mucking about with the S3 chip, so lock all the registers.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg38 << 8) | 0x38));
    VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg39 << 8) | 0x39));
    VideoPortWritePortUchar(CRT_ADDRESS_REG, IndexReg);


    //
    // We will try to recognize a few boards.
    // If this fails we must exit since we do not know how to initialize the
    // board
    //

    bMatch = FALSE;

    //
    // Look for brand name signatures in the ROM.
    //

    romAddress = VideoPortGetDeviceBase(hwDeviceExtension,
                                        accessRange[0].RangeStart,
                                        accessRange[0].RangeLength,
                                        accessRange[0].RangeInIoSpace);

    if (romAddress) {

        usRomSignature = VideoPortReadRegisterUshort(romAddress);

        if (usRomSignature == 0xAA55) {

            bMatch = VideoPortScanRom(hwDeviceExtension,
                                      romAddress,
                                      MAX_ROM_SCAN,
                                      "Number Nine Computer");

            if (bMatch == TRUE) {

                hwDeviceExtension->BoardID = S3_NUMBER_NINE;

                //
                // We know (at least we think) this is number nine board.
                // there was a bios change at #9 to change the refresh rate
                // mapping.  This change was made at Microsofts request.  the
                // problem is that the change has not make into production at
                // the time this driver was written.  For this reason, we must
                // check the bios version number, before we special case the
                // card as the number nine card.
                //
                // There is a byte in the bios at offset 0x190, that is the
                // offset from the beginning of the bios for the bios version
                // number.  the bios version number is a string.  all the
                // bios versions before 1.10.04 need this special translation.
                // all the other bios's use a translation closer to the s3
                // standard.
                //

                //
                // We only do this on x86 because on MIPS we do not use the
                // BIOS.
                //

#ifdef i386

                offset = * (((PUCHAR) romAddress) + 0x190);
                pjBiosVersion = (PUCHAR) romAddress + offset;

                pjRefString = "1.10.04";
                iCmpRet = strncmp(pjBiosVersion,
                                  pjRefString,
                                  strlen(pjRefString));

                if (iCmpRet >= 0) {

                    hwDeviceExtension->BoardID = S3_GENERIC;

                }

#endif

            } else {

                //
                // Look for an Orchid board
                //

                bMatch = VideoPortScanRom(hwDeviceExtension,
                                          romAddress,
                                          MAX_ROM_SCAN,
                                          "Orchid Technology Fahrenheit 1280");

                if (bMatch == TRUE) {

                    hwDeviceExtension->BoardID = S3_ORCHID;

                } else {

                    //
                    // Look for the DELL trademark
                    //

                    bMatch = VideoPortScanRom(hwDeviceExtension,
                                              romAddress,
                                              MAX_ROM_SCAN,
                                              "DELL");

                    if (bMatch == TRUE) {

                        hwDeviceExtension->BoardID = S3_DELL;

                    }
                }

            }

        }

        //
        // Free the ROM address since we do not need it anymore.
        //

        VideoPortFreeDeviceBase(hwDeviceExtension,
                                romAddress);

    }

#ifdef i386

    //
    // We use int10 on x86
    //

    hwDeviceExtension->bUsingInt10 = TRUE;

    //
    // We have this so that the int10 will also work on the VGA also if we
    // use it in this driver.
    //

    ConfigInfo->VdmPhysicalVideoMemoryAddress.LowPart  = 0x000A0000;
    ConfigInfo->VdmPhysicalVideoMemoryAddress.HighPart = 0x00000000;
    ConfigInfo->VdmPhysicalVideoMemoryLength           = 0x00020000;

#else

    //
    // On non-x86 platforms we can not use int 10.
    //

    hwDeviceExtension->bUsingInt10 = FALSE;

    ConfigInfo->VdmPhysicalVideoMemoryAddress.LowPart  = 0x00000000;
    ConfigInfo->VdmPhysicalVideoMemoryAddress.HighPart = 0x00000000;
    ConfigInfo->VdmPhysicalVideoMemoryLength           = 0x00000000;

#endif // i386

    //
    // Clear out the Emulator entries and the state size since this driver
    // does not support them.
    //

    ConfigInfo->NumEmulatorAccessEntries     = 0;
    ConfigInfo->EmulatorAccessEntries        = NULL;
    ConfigInfo->EmulatorAccessEntriesContext = 0;

    //
    // This driver does not do SAVE/RESTORE of hardware state.
    //

    ConfigInfo->HardwareStateSize = 0;

    //
    // Frame buffer information
    //

    hwDeviceExtension->PhysicalFrameAddress = accessRange[1].RangeStart;
    hwDeviceExtension->FrameLength          = accessRange[1].RangeLength;

    //
    // IO Port information
    // Get the base address, starting at zero and map all registers
    //

    hwDeviceExtension->PhysicalRegisterAddress = accessRange[2].RangeStart;
    hwDeviceExtension->PhysicalRegisterAddress.LowPart &= 0xFFFF0000;

    hwDeviceExtension->RegisterLength = 0x10000;
    hwDeviceExtension->RegisterSpace = accessRange[2].RangeInIoSpace;

    //
    // Indicate we do not wish to be called over
    //

    *Again = 0;

    //
    // Indicate a successful completion status.
    //

    return NO_ERROR;

} // end S3FindAdapter()


VP_STATUS
S3RegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    )

/*++

Routine Description:

    This routine determines if the alternate register set was requested via
    the registry.

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

    Context - Context value passed to the get registry paramters routine.

    ValueName - Name of the value requested.

    ValueData - Pointer to the requested data.

    ValueLength - Length of the requested data.

Return Value:

    returns NO_ERROR if the paramter was TRUE.
    returns ERROR_INVALID_PARAMETER otherwise.

--*/

{

    if (ValueLength && *((PULONG)ValueData)) {

        return NO_ERROR;

    } else {

        return ERROR_INVALID_PARAMETER;

    }

} // end S3RegistryCallback()


BOOLEAN
S3Initialize(
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
#if i386

    UCHAR   IndexReg, reg38, reg39;
    ULONG   i;

    IndexReg = VideoPortReadPortUchar(CRT_ADDRESS_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x38);
    reg38 = VideoPortReadPortUchar(CRT_DATA_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x39);
    reg39 = VideoPortReadPortUchar(CRT_DATA_REG);

    //
    // Unlock the S3 registers.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x4838);
    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0xA039);

    //
    // read all the extended registers.
    //

    for (i = 0; i < nExtendedRegs; i++) {

        VideoPortWritePortUchar(CRT_ADDRESS_REG, arvExtendedRegs[i].port);
        arvExtendedRegs[i].val = VideoPortReadPortUchar(CRT_DATA_REG);

    }

    //
    // restore the chip to the same state is was in when we got here.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg38 << 8) | 0x38));
    VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg39 << 8) | 0x39));
    VideoPortWritePortUchar(CRT_ADDRESS_REG, IndexReg);

#endif

    return TRUE;

    UNREFERENCED_PARAMETER(HwDeviceExtension);

} // end S3Initialize()

BOOLEAN
S3StartIO(
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
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    VP_STATUS status;
    ULONG inIoSpace;
    PVIDEO_MODE_INFORMATION modeInformation;
    PVIDEO_MEMORY_INFORMATION memoryInformation;
    PVIDEO_CLUT clutBuffer;
    ULONG i;
    UCHAR byte, ModeControlByte;

    VIDEO_X86_BIOS_ARGUMENTS biosArguments;
    ULONG modeNumber;
    PUCHAR pModeBits;

    //
    // Keep a history of the commands.
    // This will help track down the chip being in a DOS session while
    // GDI and the S3 display driver "think" it's in GUI mode.

    gaIOControlCode[giControlCode++] = RequestPacket->IoControlCode;
    giControlCode                   %= MAX_CONTROL_HISTORY;

    //
    // Switch on the IoContolCode in the RequestPacket. It indicates which
    // function must be performed by the driver.
    //

    switch (RequestPacket->IoControlCode) {


    case IOCTL_VIDEO_MAP_VIDEO_MEMORY:

        VideoDebugPrint((2, "S3tartIO - MapVideoMemory\n"));

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
                hwDeviceExtension->FrameLength;

        inIoSpace = 0;

        status = VideoPortMapMemory(hwDeviceExtension,
                                    hwDeviceExtension->PhysicalFrameAddress,
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
            memoryInformation->VideoRamLength;

        break;


    case IOCTL_VIDEO_UNMAP_VIDEO_MEMORY:

        VideoDebugPrint((2, "S3StartIO - UnMapVideoMemory\n"));

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY)) {

            status = ERROR_INSUFFICIENT_BUFFER;
        }

        status = VideoPortUnmapMemory(hwDeviceExtension,
                                      ((PVIDEO_MEMORY)
                                       (RequestPacket->InputBuffer))->
                                           RequestedVirtualAddress,
                                      0);

        break;


    case IOCTL_VIDEO_QUERY_PUBLIC_ACCESS_RANGES:

        VideoDebugPrint((2, "S3StartIO - QueryPublicAccessRanges\n"));

        {

           PVIDEO_PUBLIC_ACCESS_RANGES portAccess;
           ULONG physicalPortLength;

           if ( RequestPacket->OutputBufferLength <
                 (RequestPacket->StatusBlock->Information =
                                        sizeof(VIDEO_PUBLIC_ACCESS_RANGES)) ) {

               status = ERROR_INSUFFICIENT_BUFFER;
           }

           portAccess = RequestPacket->OutputBuffer;

           portAccess->VirtualAddress  = (PVOID) NULL;    // Requested VA
           portAccess->InIoSpace       = hwDeviceExtension->RegisterSpace;
           portAccess->MappedInIoSpace = portAccess->InIoSpace;

           physicalPortLength = hwDeviceExtension->RegisterLength;

           status = VideoPortMapMemory(hwDeviceExtension,
                                       hwDeviceExtension->PhysicalRegisterAddress,
                                       &physicalPortLength,
                                       &(portAccess->MappedInIoSpace),
                                       &(portAccess->VirtualAddress));
        }

        break;


    case IOCTL_VIDEO_FREE_PUBLIC_ACCESS_RANGES:

        VideoDebugPrint((2, "S3StartIO - FreePublicAccessRanges\n"));

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_MEMORY)) {

            status = ERROR_INSUFFICIENT_BUFFER;
        }

        status = VideoPortUnmapMemory(hwDeviceExtension,
                                      ((PVIDEO_MEMORY)
                                       (RequestPacket->InputBuffer))->
                                           RequestedVirtualAddress,
                                      0);

        break;


    case IOCTL_VIDEO_QUERY_AVAIL_MODES:

        VideoDebugPrint((2, "S3StartIO - QueryAvailableModes\n"));

        if (RequestPacket->OutputBufferLength <
            (RequestPacket->StatusBlock->Information =
                 hwDeviceExtension->NumAvailableModes
                 * sizeof(VIDEO_MODE_INFORMATION)) ) {

            status = ERROR_INSUFFICIENT_BUFFER;

        } else {

            modeInformation = RequestPacket->OutputBuffer;

            for (i = 0; i < NumS3VideoModes; i++) {

                if (S3Modes[i].ModeValid) {

                    *modeInformation = S3Modes[i].ModeInformation;
                    modeInformation++;

                }
            }

            status = NO_ERROR;
        }

        break;


     case IOCTL_VIDEO_QUERY_CURRENT_MODE:

        VideoDebugPrint((2, "S3StartIO - QueryCurrentModes\n"));

        if (RequestPacket->OutputBufferLength <
            (RequestPacket->StatusBlock->Information =
            sizeof(VIDEO_MODE_INFORMATION)) ) {

            status = ERROR_INSUFFICIENT_BUFFER;

        } else {

            *((PVIDEO_MODE_INFORMATION)RequestPacket->OutputBuffer) =
                S3Modes[hwDeviceExtension->ModeNumber].ModeInformation;

            status = NO_ERROR;

        }

        break;



    case IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES:

        VideoDebugPrint((2, "S3StartIO - QueryNumAvailableModes\n"));

        //
        // Find out the size of the data to be put in the the buffer and
        // return that in the status information (whether or not the
        // information is there). If the buffer passed in is not large
        // enough return an appropriate error code.
        //

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


    case IOCTL_VIDEO_SET_CURRENT_MODE:

        VideoDebugPrint((2, "S3StartIO - SetCurrentMode\n"));

        //
        // Check if the size of the data in the input buffer is large enough.
        //

        if (RequestPacket->InputBufferLength < sizeof(VIDEO_MODE)) {

            return ERROR_INSUFFICIENT_BUFFER;

        }

        //
        // Check to see if we are requesting a valid mode
        //

        modeNumber = ((PVIDEO_MODE) RequestPacket->InputBuffer)->RequestedMode;

        if ( (modeNumber >= NumS3VideoModes) ||
             (!S3Modes[modeNumber].ModeValid) ) {

            return ERROR_INVALID_PARAMETER;

        }

#ifdef i386

        VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));

        //
        // Unlock the S3 registers.
        //

        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x4838);
        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0xA039);

        /*
         Important Note: The Actix 911 board did not boot when the low order
                         bits of reg 41 were changed in the 640 X 480 or the
                         800 X 600 modes.  The Stealth did not change to 60Hz
                         in the 1024 X 768 mode when the low order bits were
                         set, it stayed at 70Hz.  And the Orchid would come up
                         in interlaced in 1024 X 768 when it should have been
                         60Hz.  Since the 911 is at the end of it's product
                         life cycle it would be unreasonable to expect the
                         board manufactures to upgrade the ROMS.  And the user
                         currently know how to set the board dip switches, so
                         the V refresh rate control is being removed for the
                         911 and 924 based boards.
        */

        //
        // If this is a 801/805 or 928 pickup the refresh control byte from
        // the chip, set the mode we want, and put the bits back in the chip.
        //

        //
        // A refresh rate of 1 indicates to use whatever is left in the BIOS
        // data area ...
        //

        if (S3Modes[modeNumber].ModeInformation.Frequency != 1) {

            switch(hwDeviceExtension->ChipID) {

            case S3_801:
            case S3_928:

                VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x52);

                ModeControlByte  = VideoPortReadPortUchar(CRT_DATA_REG);
                ModeControlByte &= ModeMaskBits[modeNumber];

                switch(hwDeviceExtension->BoardID) {

                case S3_NUMBER_NINE:

                    pModeBits = NumberNineModeBits;
                    break;

                case S3_DELL:

                    pModeBits = DellModeBits;
                    break;

                // case S3_GENERIC:
                default:

                    pModeBits = GenericModeBits;
                    break;

                }

                ModeControlByte |= pModeBits[modeNumber];

                VideoPortWritePortUchar(CRT_DATA_REG, ModeControlByte);

                break;

            default:

                break;

            }
        }

        biosArguments.Ebx = S3Modes[modeNumber].Int10ModeNumber;
        biosArguments.Eax = 0x4f02;

        status = VideoPortInt10(HwDeviceExtension, &biosArguments);

        //
        // If we get an error, return it.
        //

        if (status != NO_ERROR) {

            break;

        }

        //
        // Save the mode number since we know the rest will work.
        //

        hwDeviceExtension->ModeNumber = modeNumber;

        //
        // Unlock the S3 registers,  we need to unlock the registers a second time,
        // since we do not know what state the locks were in after the Int10.
        //

        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x4838);
        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0xA039);

        //
        // If this is a 1280 mode then set to non-contiguous memory mode.
        //

        if (S3Modes[modeNumber].ModeInformation.VisScreenWidth == 1280) {

            //
            // Make sure we are not in linear address mode
            //

            VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x58);
            byte = VideoPortReadPortUchar(CRT_DATA_REG);
            byte &= ~0x10;
            VideoPortWritePortUchar(CRT_DATA_REG, byte);

            //
            // Turn on the graphics engine.
            //

            VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x40);
            byte = VideoPortReadPortUchar(CRT_DATA_REG);
            byte |= 0x01;
            VideoPortWritePortUchar(CRT_DATA_REG, byte);

            //
            // Set the 2K by 1K memory map.
            //

            VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x0013);
            VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x8f31);
            VideoPortWritePortUshort(ADV_FUNC_CTL,    0x0003);
            VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x0050);
            VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x5051);

            //
            // Some of the number nine boards do set the chip up correctly
            // for an external cursor. We must OR in the bits, because if we
            // don't the metheus board will not init.
            //

            VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x45);
            byte = VideoPortReadPortUchar(CRT_DATA_REG);
            byte |= 0x20;
            VideoPortWritePortUchar(CRT_DATA_REG, byte);

            VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x55);
            byte = VideoPortReadPortUchar(CRT_DATA_REG);
            byte |= 0x20;
            VideoPortWritePortUchar(CRT_DATA_REG, byte);

        }

#else
        //
        // For MIPS program according to the mode tables
        //

        //
        // Save the mode number since we will reference it in SetHWMode.
        //

        hwDeviceExtension->ModeNumber = modeNumber;

        //
        // Select the Enhanced mode init depending upon the type of chip
        // found.

        if ( (hwDeviceExtension->BoardID == S3_NUMBER_NINE) &&
             (S3Modes[modeNumber].ModeInformation.VisScreenWidth == 1280) ) {

            SetHWMode(hwDeviceExtension, S3_928_1280_Enhanced_Mode);

        } else {

            //
            // Use defaults for all other boards
            //

            switch(hwDeviceExtension->ChipID) {

            case S3_911:

                SetHWMode(hwDeviceExtension, S3_911_Enhanced_Mode);
                break;

            case S3_801:

                SetHWMode(hwDeviceExtension, S3_801_Enhanced_Mode);
                break;

            case S3_928:

                SetHWMode(hwDeviceExtension, S3_928_Enhanced_Mode);
                break;

            default:

                VideoDebugPrint((0, "S3: Bad chip type for these boards"));
                break;
            }

        }

        //
        // Unlock the S3 registers,  we need to unlock the registers a second
        // time since the interperter has them locked when it returns to us.
        //

        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x4838);
        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0xA039);

#endif  // i386

        //
        // On a certain version of the DELL BIOS reg 5A was set to 0.
        // This will ensure that the linear address window is always set
        // where I expect it to be, regardless of the bios.
        //

        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x0059);
        VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x0A5A);

        //
        // Enable the Graphics engine.
        //

        VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x40);
        byte = VideoPortReadPortUchar(CRT_DATA_REG);
        byte |= 0x01;
        VideoPortWritePortUchar(CRT_DATA_REG, byte);

#ifndef i386

        //
        // Zero the DAC and the Screen buffer memory.
        //

        ZeroMemAndDac(HwDeviceExtension);
#endif

        status = NO_ERROR;

        break;

    case IOCTL_VIDEO_SET_COLOR_REGISTERS:

        VideoDebugPrint((2, "S3StartIO - SetColorRegs\n"));

        clutBuffer = RequestPacket->InputBuffer;

        status = S3SetColorLookup(HwDeviceExtension,
                                   (PVIDEO_CLUT) RequestPacket->InputBuffer,
                                   RequestPacket->InputBufferLength);
        break;


#ifdef i386

    case IOCTL_VIDEO_RESET_DEVICE:

        VideoDebugPrint((2, "S3StartIO - RESET_DEVICE\n"));

        //
        // Wait for the GP to become idle,
        //

        while (VideoPortReadPortUshort(GP_STAT) & 0x0200);

        //
        // Reset the board to a default mode
        //

        SetHWMode(HwDeviceExtension, s3_set_vga_mode);

        VideoDebugPrint((2, "S3 RESET_DEVICE - About to do int10\n"));

        //
        // Do an Int10 to mode 3 will put the board to a known state.
        //

        VideoPortZeroMemory(&biosArguments, sizeof(VIDEO_X86_BIOS_ARGUMENTS));

        biosArguments.Eax = 0x0003;

        VideoPortInt10(HwDeviceExtension,
                       &biosArguments);

        VideoDebugPrint((2, "S3 RESET_DEVICE - Did int10\n"));

        status = NO_ERROR;
        break;

#endif

    //
    // if we get here, an invalid IoControlCode was specified.
    //

    default:

        VideoDebugPrint((1, "Fell through S3 startIO routine - invalid command\n"));

        status = ERROR_INVALID_FUNCTION;

        break;

    }

    //
    // Keep a history of the commands.
    // This will help track down the chip being in a DOS session while
    // GDI and the S3 display driver "think" it's in GUI mode.

    gaIOControlCode[giControlCode++] = 0x00005555;
    giControlCode                   %= MAX_CONTROL_HISTORY;

    VideoDebugPrint((2, "Leaving S3 startIO routine\n"));

    RequestPacket->StatusBlock->Status = status;

    return TRUE;

} // end S3StartIO()


VP_STATUS
S3SetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    )

/*++

Routine Description:

    This routine sets a specified portion of the color lookup table settings.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    ClutBufferSize - Length of the input buffer supplied by the user.

    ClutBuffer - Pointer to the structure containing the color lookup table.

Return Value:

    None.

--*/

{
    USHORT i;

    //
    // Check if the size of the data in the input buffer is large enough.
    //

    if ( (ClutBufferSize < sizeof(VIDEO_CLUT) - sizeof(ULONG)) ||
         (ClutBufferSize < sizeof(VIDEO_CLUT) +
                     (sizeof(ULONG) * (ClutBuffer->NumEntries - 1)) ) ) {

        return ERROR_INSUFFICIENT_BUFFER;

    }

    //
    // Check to see if the parameters are valid.
    //

    if ( (ClutBuffer->NumEntries == 0) ||
         (ClutBuffer->FirstEntry > VIDEO_MAX_COLOR_REGISTER) ||
         (ClutBuffer->FirstEntry + ClutBuffer->NumEntries >
                                     VIDEO_MAX_COLOR_REGISTER + 1) ) {

    return ERROR_INVALID_PARAMETER;

    }

    //
    //  Set CLUT registers directly on the hardware
    //

    for (i = 0; i < ClutBuffer->NumEntries; i++) {

        VideoPortWritePortUchar(DAC_ADDRESS_WRITE_PORT, (UCHAR) (ClutBuffer->FirstEntry + i));
        VideoPortWritePortUchar(DAC_DATA_REG_PORT, (UCHAR) (((ClutBuffer->LookupTable[i].RgbArray.Red)) >> 2));
        VideoPortWritePortUchar(DAC_DATA_REG_PORT, (UCHAR) (((ClutBuffer->LookupTable[i].RgbArray.Green)) >> 2));
        VideoPortWritePortUchar(DAC_DATA_REG_PORT, (UCHAR) (((ClutBuffer->LookupTable[i].RgbArray.Blue)) >> 2));

    }

    return NO_ERROR;

} // end S3SetColorLookup()


VOID
SetHWMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PUSHORT pusCmdStream
    )

/*++

Routine Description:

    Interprets the appropriate command array to set up VGA registers for the
    requested mode. Typically used to set the VGA into a particular mode by
    programming all of the registers

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

    pusCmdStream - pointer to a command stream to execute.

Return Value:

    The status of the operation (can only fail on a bad command); TRUE for
    success, FALSE for failure.

--*/

{
    ULONG ulCmd;
    ULONG ulPort;
    UCHAR jValue;
    USHORT usValue;
    ULONG culCount;
    ULONG ulIndex,
          Microseconds;
    ULONG mappedAddressIndex;
    ULONG mappedAddressOffset;

    //
    // If there is no command string, just return
    //

    if (!pusCmdStream) {

        return;

    }

    while ((ulCmd = *pusCmdStream++) != EOD) {

        //
        // Determine major command type
        //

        switch (ulCmd & 0xF0) {

        case SELECTACCESSRANGE:

            //
            // Determine which address range to use for commands that follow
            //

            switch (ulCmd & 0x0F) {

            case VARIOUSVGA:

                //
                // Used for registers in the range 0x3c0 - 0x3cf
                //

                mappedAddressIndex  = 2;
                mappedAddressOffset = 0x3c0;

                break;

            case SYSTEMCONTROL:

                //
                // Used for registers in the range 0x3d4 - 0x3df
                //

                mappedAddressIndex  = 3;
                mappedAddressOffset = 0x3d4;

                break;

            case ADVANCEDFUNCTIONCONTROL:

                //
                // Used for registers in the range 0x4ae8-0x4ae9
                //

                mappedAddressIndex  = 5;
                mappedAddressOffset = 0x4ae8;

                break;

            }

            break;


        case OWM:

            ulPort   = *pusCmdStream++;
            culCount = *pusCmdStream++;

            while (culCount--) {
                usValue = *pusCmdStream++;
                VideoPortWritePortUshort((PUSHORT)((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort),
                                         usValue);
            }

            break;


        // Basic input/output command

        case INOUT:

            // Determine type of inout instruction
            if (!(ulCmd & IO)) {

                // Out instruction
                // Single or multiple outs?
                if (!(ulCmd & MULTI)) {

                    // Single out
                    // Byte or word out?
                    if (!(ulCmd & BW)) {

                        // Single byte out
                        ulPort = *pusCmdStream++;
                        jValue = (UCHAR) *pusCmdStream++;
                        VideoPortWritePortUchar((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort,
                                                jValue);

                    } else {

                        // Single word out
                        ulPort = *pusCmdStream++;
                        usValue = *pusCmdStream++;
                        VideoPortWritePortUshort((PUSHORT)((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort),
                                                usValue);

                    }

                } else {

                    // Output a string of values
                    // Byte or word outs?
                    if (!(ulCmd & BW)) {

                        // String byte outs. Do in a loop; can't use
                        // VideoPortWritePortBufferUchar because the data
                        // is in USHORT form
                        ulPort = *pusCmdStream++;
                        culCount = *pusCmdStream++;
                        while (culCount--) {
                            jValue = (UCHAR) *pusCmdStream++;
                            VideoPortWritePortUchar((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort,
                                                    jValue);

                        }

                    } else {

                        // String word outs
                        ulPort = *pusCmdStream++;
                        culCount = *pusCmdStream++;
                        VideoPortWritePortBufferUshort((PUSHORT)((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort),
                                                       pusCmdStream,
                                                       culCount);
                        pusCmdStream += culCount;

                    }
                }

            } else {

                // In instruction

                // Currently, string in instructions aren't supported; all
                // in instructions are handled as single-byte ins

                // Byte or word in?
                if (!(ulCmd & BW)) {

                    // Single byte in
                    ulPort = *pusCmdStream++;
                    jValue = VideoPortReadPortUchar((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort);

                } else {

                    // Single word in
                    ulPort = *pusCmdStream++;
                    usValue = VideoPortReadPortUshort((PUSHORT)((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort));
                }

            }

            break;


        // Higher-level input/output commands

        case METAOUT:

            // Determine type of metaout command, based on minor command field
            switch (ulCmd & 0x0F) {

                // Indexed outs
                case INDXOUT:

                    ulPort = *pusCmdStream++;
                    culCount = *pusCmdStream++;
                    ulIndex = *pusCmdStream++;

                    while (culCount--) {

                        usValue = (USHORT) (ulIndex +
                                  (((ULONG)(*pusCmdStream++)) << 8));
                        VideoPortWritePortUshort((PUSHORT)((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort),
                                             usValue);

                        ulIndex++;

                    }

                    break;


                // Masked out (read, AND, XOR, write)
                case MASKOUT:

                    ulPort = *pusCmdStream++;
                    jValue = VideoPortReadPortUchar((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort);
                    jValue &= *pusCmdStream++;
                    jValue ^= *pusCmdStream++;
                    VideoPortWritePortUchar((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort,
                                            jValue);
                    break;


                // Attribute Controller out
                case ATCOUT:

                    ulPort = *pusCmdStream++;
                    culCount = *pusCmdStream++;
                    ulIndex = *pusCmdStream++;

                    while (culCount--) {

                        // Write Attribute Controller index
                        VideoPortWritePortUchar((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort,
                                                (UCHAR)ulIndex);

                        // Write Attribute Controller data
                        jValue = (UCHAR) *pusCmdStream++;
                        VideoPortWritePortUchar((PUCHAR)HwDeviceExtension->MappedAddress[mappedAddressIndex] - mappedAddressOffset + ulPort,
                                                jValue);

                        ulIndex++;

                    }

                    break;

                case DELAY:

                    Microseconds = (ULONG) *pusCmdStream++;
                    VideoPortStallExecution(Microseconds);

                    break;

                case BT485RESET:

                    Reset_Bt485(HwDeviceExtension);

                    break;

#ifndef i386
                case VBLANK:

                    Wait_VSync(HwDeviceExtension);

                    break;

                case SETCLK:

                    Set_Oem_Clock(HwDeviceExtension);

                    break;

                case SETCRTC:

                    //
                    // NOTE:
                    // beware: recursive call ...
                    //

                    SetHWMode(HwDeviceExtension,
                              S3Modes[HwDeviceExtension->ModeNumber].
                                  CRTCTables[HwDeviceExtension->ChipID]);


                    break;

                case BUSTEST:

                    Bus_Test(HwDeviceExtension);

                    break;
#endif // i386

                // None of the above; error
                default:

                    return;

            }

            break;


        // NOP

        case NCMD:

            break;


        // Unknown command; error

        default:

            return;

        }

    }

    return;

} // end SetHWMode()


BOOLEAN
Reset_Bt485(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )

/*++

Routine Description:

    This routine detects the presence of a 928 and a Bt485 and if both are
    found it makes sure the DAC is resets all the command registers, to make
    sure the DAC will function in the VGA mode.

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

Return Value:

    TRUE  -
    FALSE -

--*/
{
    UCHAR OriginalReg55, Reg55;
    UCHAR reg38, reg39;

    //
    // This routine assumes we are dealing with an S3.
    // Make sure the registers are unlocked.
    //

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x38);
    reg38 = VideoPortReadPortUchar(CRT_DATA_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x39);
    reg39 = VideoPortReadPortUchar(CRT_DATA_REG);

    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x4838);
    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0xA039);

    //
    // First make sure this is a 928.
    //

    if (HwDeviceExtension->ChipID != S3_928) {

        return FALSE;

    }

    //
    // It's a 928 so now we have to make sure this is Bt485
    //

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x55);
    OriginalReg55 = VideoPortReadPortUchar(CRT_DATA_REG);
    OriginalReg55 &= ~0x03;

    //
    // BUGBUG
    // The ID for the BT485 did not work on all DACs so I'll switch off the
    // resolution.
    // This ID changed between the prototype chips and the release component,
    // at the time of this release there are too many prototype chips in the
    // field to rely on this ID.
    //

    if (S3Modes[HwDeviceExtension->ModeNumber].ModeInformation.VisScreenWidth
        != 1280) {

        return FALSE;

    }

    //
    // We know the board uses a Bt485 DAC.
    //

    //
    // Zero out Command register 3.
    //

    Reg55 = OriginalReg55 | 0x01;
    VideoPortWritePortUchar(CRT_DATA_REG, Reg55);
    VideoPortWritePortUchar((PUCHAR) 0x3C6, 0x80);

    Reg55 = OriginalReg55;
    VideoPortWritePortUchar(CRT_DATA_REG, Reg55);
    VideoPortWritePortUchar((PUCHAR) 0x3C8, 0x01);

    Reg55 = OriginalReg55 | 0x02;
    VideoPortWritePortUchar(CRT_DATA_REG, Reg55);
    VideoPortWritePortUchar((PUCHAR) 0x3C6, 0x00);

    //
    // Now zero out command register 2, 1, & 0
    //

    VideoPortWritePortUchar((PUCHAR) 0x3C9, 0x00);
    VideoPortWritePortUchar((PUCHAR) 0x3C8, 0x00);

    Reg55 = OriginalReg55 | 0x01;
    VideoPortWritePortUchar(CRT_DATA_REG, Reg55);
    VideoPortWritePortUchar((PUCHAR) 0x3C6, 0x00);

    //
    // Now set the extended DAC control back to it's original val.
    //

    VideoPortWritePortUchar(CRT_DATA_REG, OriginalReg55);

    //
    // Were done mucking about with the S3 chip, so lock all the registers.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg38 << 8) | 0x38));
    VideoPortWritePortUshort(CRT_ADDRESS_REG, (USHORT)(((USHORT) reg39 << 8) | 0x39));

    return TRUE;

}

#ifndef i386


VOID
ZeroMemAndDac(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )

/*++

Routine Description:

    Initialize the DAC to 0 (black).

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

Return Value:

    None

--*/

{
    ULONG i;
    USHORT Cmd;

    //
    // Turn off the screen at the DAC.
    //

    VideoPortWritePortUchar(DAC_PIXEL_MASK_REG, 0x0);

    for (i = 0; i < 256; i++) {

        VideoPortWritePortUchar(DAC_ADDRESS_WRITE_PORT, (UCHAR)i);
        VideoPortWritePortUchar(DAC_DATA_REG_PORT, 0x0);
        VideoPortWritePortUchar(DAC_DATA_REG_PORT, 0x0);
        VideoPortWritePortUchar(DAC_DATA_REG_PORT, 0x0);

    }

    //
    // Zero the memory.
    //
    // First open up the clipping.
    //

    FIFOWAIT(FIFO_4_EMPTY);

    VideoPortWritePortUshort (MULTIFUNC_CNTL, (CLIP_TOP    | 0));
    VideoPortWritePortUshort (MULTIFUNC_CNTL, (CLIP_LEFT   | 0));
    VideoPortWritePortUshort (MULTIFUNC_CNTL, (CLIP_BOTTOM | S3BM_HEIGHT));
    VideoPortWritePortUshort (MULTIFUNC_CNTL, (CLIP_RIGHT  | S3BM_WIDTH));

    //
    // Now do the blit.
    //
    // Zero out coprocessor memory for the masks, color data,
    // and the save area.
    //

    Cmd = RECTANGLE_FILL |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | SINGLE_PIXEL       | WRITE;

    FIFOWAIT(FIFO_3_EMPTY);

    VideoPortWritePortUshort(FRGD_MIX, LOGICAL_0);
    VideoPortWritePortUshort(WRT_MASK, 0xff);
    VideoPortWritePortUshort(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    FIFOWAIT(FIFO_5_EMPTY);

    VideoPortWritePortUshort(CUR_X, 0);
    VideoPortWritePortUshort(CUR_Y, 0);
    VideoPortWritePortUshort(RECT_WIDTH, S3BM_WIDTH - 1);
    VideoPortWritePortUshort(MULTIFUNC_CNTL, (RECT_HEIGHT | S3BM_HEIGHT - 1));
    VideoPortWritePortUshort(CMD, Cmd);

    //
    // Turn on the screen at the DAC
    //

    VideoPortWritePortUchar(DAC_PIXEL_MASK_REG, 0x0ff);

    return;

}

VP_STATUS
Set_Oem_Clock(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )

/*++

Routine Description:

    Set the clock chip on each of the supported cards.

Arguments:

    HwDeviceExtension - Pointer to the miniport driver's device extension.

Return Value:

    Always TRUE

--*/

{
    ULONG ul;
    ULONG clock_numbers;

    switch(HwDeviceExtension->BoardID) {

    case S3_NUMBER_NINE:

        VideoPortStallExecution(1000);

        // Jerry said to make the M clock not multiple of the P clock
        // on the 3 meg (level 12) board.  This solves the shimmy
        // problem.

        if (HwDeviceExtension->AdapterMemorySize == 0x00300000) {

            ul = 49000000;
            clock_numbers = calc_clock(ul, 3);
            set_clock(HwDeviceExtension, clock_numbers);
            VideoPortStallExecution(3000);

        }

        ul = aulNumberNineClk[HwDeviceExtension->ModeNumber];
        clock_numbers = calc_clock(ul, 2);
        set_clock(HwDeviceExtension, clock_numbers);

        VideoPortStallExecution(3000);

        break;

    case S3_ORCHID:

        //
        // Only the 911 Orchid board needs specific init parameters.
        // Otherwise, fall through the generic function.
        //

        if (HwDeviceExtension->ChipID == S3_911) {

            ul = aulOrchidClk[HwDeviceExtension->ModeNumber];
            VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x42);
            VideoPortWritePortUchar(CRT_DATA_REG, (UCHAR) ul);

            break;

        }

        //
        // Genric S3 board.
        //

    case S3_GENERIC:
    default:

        ul = aulGenericClk[HwDeviceExtension->ModeNumber];
        VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x42);
        VideoPortWritePortUchar(CRT_DATA_REG, (UCHAR) ul);
        break;

    }

    return TRUE;
}


VP_STATUS
Wait_VSync(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )

/*++

Routine Description:

    Wait for the vertical blanking interval on the chip

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

Return Value:

    Always TRUE

--*/

{

    ULONG i;
    UCHAR byte;

    // It's real possible that this routine will get called
    // when the 911 is in a zombie state, meaning there is no
    // vertical sync being generated.  This is why we have some long
    // time out loops here.
    // !!! What is the correct NT way to do this type of time out?

    // First wait for getting into vertical blanking.

    for (i = 0; i < 0x100000; i++) {

        byte = VideoPortReadPortUchar(SYSTEM_CONTROL_REG);
        if (byte & 0x08)
            break;

    }

    //
    // We are either in a vertical blaning interval or we have timmed out.
    // Wait for the Vertical display interval.
    // This is done to make sure we exit this routine at the beginning
    // of a vertical blanking interval, and not in the middle or near
    // the end of one.
    //

    for (i = 0; i < 0x100000; i++) {

        byte = VideoPortReadPortUchar(SYSTEM_CONTROL_REG);
        if (!(byte & 0x08))
            break;

    }

    //
    // Now wait to get into the vertical blank interval again.
    //

    for (i = 0; i < 0x100000; i++) {

        byte = VideoPortReadPortUchar(SYSTEM_CONTROL_REG);
        if (byte & 0x08)
            break;

    }

    return (TRUE);

}

BOOLEAN
Bus_Test(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )

/*++

Routine Description:

    Test the bus to determine in the ISA address latch needs to be
    enabled.

Arguments:

    HwDeviceExtension - Supplies a pointer to the miniport's device extension.

Return Value:

    TRUE - need to Enable the ISA Address Latch.
           And it is enabled when we return.

    FALSE - do not enable the ISA Address Latch.
            And it's disabled when we return.

    NOTE: The test #9 gave me; to set the ISA Address Latch,
          then do the test failed on a DELL 50Mhz machine
          Now, we'll try resetting the ISA Address Latch.
          The reset of the latch bit passed on the DELL, but failed on
          the OPTI chip set.  Now I'll change the test to writting
          a pattern to the entire Video Memory and try to read it back,
          with a default state of the ISA latch bit being reset.

          3/26/93

          This routine does not seem to necessary with the later versions of the BIOS.
          The new Number Nine BIOS seems to detect the need to set or reset the ISA address
          latch.  AT this point I will leave it in to cover all the older #9 boards in
          the field.
--*/
{


    UCHAR reg35, reg51, reg58, reg59, reg5a;
    UCHAR *pRamAddr;
    PHYSICAL_ADDRESS memoryAddress;

    volatile ULONG *pulRamAddr;

    LONG i;
    LONG iBank;
    LONG nBanks;

    ULONG ulTestPat;

    BOOLEAN bEnbLa;

    //
    // Note: this routine assumes the S3 registers are unlocked
    //             when it is called.
    //
    // Map in 64K of VRAM address space at 0xA000:0
    //

    memoryAddress.HighPart = 0;
    memoryAddress.LowPart  = 0xA0000;

    pRamAddr = VideoPortGetDeviceBase(HwDeviceExtension,
                                      memoryAddress,
                                      0x10000,
                                      0);

    //
    // Wait for the GP to be idle
    //

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x40);

    if (VideoPortReadPortUchar(CRT_DATA_REG) & 0x1) {

        while (VideoPortReadPortUshort(GP_STAT) & 0x0200);

    }

    //
    // Get the original values for all the registers.
    // So we can restore them before we leave the driver.
    //

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x35);
    reg35 = VideoPortReadPortUchar(CRT_DATA_REG) & 0xF0;

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x51);
    reg51 = VideoPortReadPortUchar(CRT_DATA_REG) & ~0x0C;

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x58);
    reg58 = VideoPortReadPortUchar(CRT_DATA_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x59);
    reg59 = VideoPortReadPortUchar(CRT_DATA_REG);

    VideoPortWritePortUchar(CRT_ADDRESS_REG, 0x5A);
    reg5a = VideoPortReadPortUchar(CRT_DATA_REG);

    //
    // Set the Linear Address Window to A0000.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x0059);
    VideoPortWritePortUshort(CRT_ADDRESS_REG, 0x0a5a);

    //
    // Do the test for enabling the ISA address latch feature on the chip.
    //

    bEnbLa = FALSE;

    VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)((((reg58 & ~0x08) | 0x10) << 8) | 0x58)));

    //
    // Write out the test pattern to the Video memory.
    // This is a dword write.
    //

    ulTestPat = 0x55AA6699;
    pulRamAddr= (ULONG *) pRamAddr;

    nBanks = HwDeviceExtension->AdapterMemorySize >> 16;

    for (iBank = 0; iBank < nBanks; iBank++) {

        //
        // Set the bank address.
        //

        VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)((((reg51 | ((0x30 & iBank) << 6)) << 8) | 0x51))));
        VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)((((reg35 | (0x0F & iBank)) << 8) | 0x35))));

        //
        // Write 64K of pattern.
        //

        for (i = 0; i < 0x1000; i++) {

            pulRamAddr[i] = ulTestPat;
        }

    }

    //
    // Write 64 of Zeros to the first bank, this should take care of bus
    // float.
    //

    iBank = 0;

    VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)((((reg51 | ((0x30 & iBank) << 6)) << 8) | 0x51))));
    VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)((((reg35 | (0x0F & iBank)) << 8) | 0x35))));

    //
    // Write 64K of 0.
    //

    for (i = 0; i < 0x1000; i++) {

        pulRamAddr[i] = 0;
    }

    //
    // Now read back the pattern.
    // If the data is different at any point then the ISA latch addr.
    // should be enabled.
    //

    for (iBank = 1; iBank < nBanks; iBank++) {

        //
        // Set the bank address.
        //

        VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)((((reg51 | ((0x30 & iBank) << 6)) << 8) | 0x51))));
        VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)((((reg35 | (0x0F & iBank)) << 8) | 0x35))));

        //
        // Set the address for the beginning of this bank.
        //

        pulRamAddr= (ULONG *) pRamAddr;

        for (i = 0; i < 0x1000; i++) {

            if (pulRamAddr[i] != ulTestPat) {

                bEnbLa = TRUE;
                break;

            }
        }

        if (bEnbLa == TRUE) {

            break;

        }
    }

    //
    // Set register 5A, this includes the ISA address latch.
    //

    if (bEnbLa == TRUE) {

        VideoPortWritePortUshort(CRT_ADDRESS_REG,
                                 ((USHORT)(((reg58 | 0x8) << 8) | 0x58)));

    } else {

        VideoPortWritePortUshort(CRT_ADDRESS_REG,
                                 ((USHORT)(((reg58 & ~0x8) << 8) | 0x58)));

    }

    //
    // Restore the registers we used to test memory.
    //

    VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)(((USHORT) reg35 << 8) | 0x35)));
    VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)(((USHORT) reg51 << 8) | 0x51)));
    VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)(((USHORT) reg59 << 8) | 0x59)));
    VideoPortWritePortUshort(CRT_ADDRESS_REG, ((USHORT)(((USHORT) reg5a << 8) | 0x5A)));

    //
    // Free the memory mapping we used for the RAM access
    //

    VideoPortFreeDeviceBase(HwDeviceExtension, pRamAddr);

    //
    // return the results.
    //

    return bEnbLa;

}

#endif // i386
