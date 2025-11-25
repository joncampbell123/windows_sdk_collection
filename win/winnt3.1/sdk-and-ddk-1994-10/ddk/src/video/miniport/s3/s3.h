/*++

Copyright (c) 1990-1992  Microsoft Corporation

Module Name:

    s3.h

Abstract:

    This module contains the definitions for the S3 miniport driver.

Environment:

    Kernel mode

Revision History:

--*/

#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"


//
// Number of access ranges used by an S3.
//

#define NUM_S3_ACCESS_RANGES 22
#define S3_EXTENDED_RANGE_START 4


//
// Supported board definitions.
//

typedef enum _S3_BOARDS {
    S3_GENERIC = 0,
    S3_ORCHID,
    S3_NUMBER_NINE,
    S3_DELL,
    MAX_S3_BOARD
} S3_BOARDS;

//
// Chip type definitions
//

typedef enum _S3_CHIPSETS {
    S3_911 = 0,    // 911 and 924 boards
    S3_801,        // 801 and 805 boards
    S3_928,        // 928 boards
    MAX_S3_CHIPSET
} S3_CHIPSET;

//
// Characteristics of each mode
//

typedef struct _S3_VIDEO_MODES {

    UCHAR ModeValid;
    ULONG RequiredVideoMemory;
    USHORT Int10ModeNumber;

    PUSHORT CRTCTables[MAX_S3_CHIPSET];   // Array of mode table pointers.

    VIDEO_MODE_INFORMATION ModeInformation;

} S3_VIDEO_MODES, *PS3_VIDEO_MODES;


//
// Register definitions used with VideoPortRead/Write functions
//

#define DAC_PIXEL_MASK_REG     (PVOID) ((ULONG)( ((PHW_DEVICE_EXTENSION)HwDeviceExtension)->MappedAddress[2]) + (0x03C6 - 0x03C0))

#define DAC_ADDRESS_WRITE_PORT (PVOID) ((ULONG)(HwDeviceExtension->MappedAddress[2]) + (0x03C8 - 0x03C0))
#define DAC_DATA_REG_PORT      (PVOID) ((ULONG)(HwDeviceExtension->MappedAddress[2]) + (0x03C9 - 0x03C0))

#define MISC_OUTPUT_REG_WRITE  (PVOID) ((ULONG)(HwDeviceExtension->MappedAddress[2]) + (0x03C2 - 0x03C0))
#define MISC_OUTPUT_REG_READ   (PVOID) ((ULONG)(HwDeviceExtension->MappedAddress[2]) + (0x03CC - 0x03C0))

#define SEQ_ADDRESS_REG        (PVOID) ((ULONG)(HwDeviceExtension->MappedAddress[2]) + (0x03C4 - 0x03C0))
#define SEQ_DATA_REG           (PVOID) ((ULONG)(HwDeviceExtension->MappedAddress[2]) + (0x03C5 - 0x03C0))

#define CRT_ADDRESS_REG        (PVOID) ((PHW_DEVICE_EXTENSION)HwDeviceExtension)->MappedAddress[3]
#define CRT_DATA_REG           (PVOID) ((ULONG)( ((PHW_DEVICE_EXTENSION)HwDeviceExtension)->MappedAddress[3]) + (0x03D5 - 0x03D4))
#define SYSTEM_CONTROL_REG     (PVOID) ((ULONG)( ((PHW_DEVICE_EXTENSION)HwDeviceExtension)->MappedAddress[3]) + (0x03DA - 0x03D4))

#define ADV_FUNC_CTL    (PVOID) ((PHW_DEVICE_EXTENSION)HwDeviceExtension)->MappedAddress[5]

#define SUB_SYS_STAT    HwDeviceExtension->MappedAddress[4]      // 0x42E8
#define CUR_Y           HwDeviceExtension->MappedAddress[6]      // 0x82E8
#define CUR_X           HwDeviceExtension->MappedAddress[7]      // 0x86E8
#define RECT_WIDTH      HwDeviceExtension->MappedAddress[11]     // 0x96E8
#define GP_STAT         ((PHW_DEVICE_EXTENSION)HwDeviceExtension)->MappedAddress[12]        // 0x9AE8
#define CMD             HwDeviceExtension->MappedAddress[12]     // 0x9AE8
#define BKGD_COLOR      HwDeviceExtension->MappedAddress[14]     // 0xA2E8
#define FRGD_COLOR      HwDeviceExtension->MappedAddress[15]     // 0xA6E8
#define WRT_MASK        HwDeviceExtension->MappedAddress[16]     // 0xAAE8
#define RD_MASK         HwDeviceExtension->MappedAddress[17]     // 0xAEE8
#define BKGD_MIX        HwDeviceExtension->MappedAddress[18]     // 0xB6E8
#define FRGD_MIX        HwDeviceExtension->MappedAddress[19]     // 0xBAE8
#define MULTIFUNC_CNTL  HwDeviceExtension->MappedAddress[20]     // 0xBEE8
#define PIXEL_TRANSFER  HwDeviceExtension->MappedAddress[21]     // 0xE2E8

//
// Define device extension structure. This is device dependant/private
// information.
//

typedef struct _HW_DEVICE_EXTENSION {
    PVOID FrameAddress;
    PHYSICAL_ADDRESS PhysicalFrameAddress;
    ULONG FrameLength;
    PHYSICAL_ADDRESS PhysicalRegisterAddress;
    ULONG RegisterLength;
    UCHAR RegisterSpace;
    BOOLEAN bUsingInt10;
    ULONG ModeNumber;
    ULONG BoardID;
    ULONG ChipID;
    ULONG NumAvailableModes;
    ULONG AdapterMemorySize;
    PVOID MappedAddress[NUM_S3_ACCESS_RANGES];    
} HW_DEVICE_EXTENSION, *PHW_DEVICE_EXTENSION;


//
// Highest valid DAC color register index.
//

#define VIDEO_MAX_COLOR_REGISTER  0xFF


// Equates to handle the S3 graphics engine.

#define S3BM_WIDTH          1024
#define S3BM_HEIGHT         768


    // Command Types

#define NOP                     0x0000
#define DRAW_LINE               0x2000
#define RECTANGLE_FILL          0x4000
#define BITBLT                  0xC000

#define BYTE_SWAP               0x1000
#define BUS_SIZE_16             0x0200
#define BUS_SIZE_8              0x0000
#define WAIT                    0x0100

    // Drawing directions (radial)

#define DRAWING_DIRECTION_0     0x0000
#define DRAWING_DIRECTION_45    0x0020
#define DRAWING_DIRECTION_90    0x0040
#define DRAWING_DIRECTION_135   0x0060
#define DRAWING_DIRECTION_180   0x0080
#define DRAWING_DIRECTION_225   0x00A0
#define DRAWING_DIRECTION_270   0x00C0
#define DRAWING_DIRECTION_315   0x00E0

    // Drawing directions (x/y)

#define DRAWING_DIR_BTRLXM  0x0000
#define DRAWING_DIR_BTLRXM  0x0020
#define DRAWING_DIR_BTRLYM  0x0040
#define DRAWING_DIR_BTLRYM  0x0060
#define DRAWING_DIR_TBRLXM  0x0080
#define DRAWING_DIR_TBLRXM  0x00A0
#define DRAWING_DIR_TBRLYM  0x00C0
#define DRAWING_DIR_TBLRYM  0x00E0

    // Drawing Direction Bits

#define PLUS_X              0x0020
#define PLUS_Y              0x0080
#define MAJOR_Y             0x0040

    // Draw

#define DRAW                    0x0010

    // Direction type

#define DIR_TYPE_RADIAL         0x0008
#define DIR_TYPE_XY             0x0000

    // Last Pixel

#define LAST_PIXEL_OFF          0x0004
#define LAST_PIXEL_ON           0x0000

    // Pixel Mode

#define MULTIPLE_PIXELS         0x0002
#define SINGLE_PIXEL            0x0000

    // Read/Write

#define READ                    0x0000
#define WRITE                   0x0001


    // G.P. Status

#define HARDWARE_BUSY       0x200
#define READ_DATA_AVAILABLE 0x100

    // Fifo Status

#define FIFO_7_STATUS       0x080
#define FIFO_6_STATUS       0x040
#define FIFO_5_STATUS       0x020
#define FIFO_4_STATUS       0x010
#define FIFO_3_STATUS       0x008
#define FIFO_2_STATUS       0x004
#define FIFO_1_STATUS       0x002
#define FIFO_0_STATUS       0x001

    // Fifo status in terms of empty entries

#define FIFO_1_EMPTY FIFO_7_STATUS
#define FIFO_2_EMPTY FIFO_6_STATUS
#define FIFO_3_EMPTY FIFO_5_STATUS
#define FIFO_4_EMPTY FIFO_4_STATUS
#define FIFO_5_EMPTY FIFO_3_STATUS
#define FIFO_6_EMPTY FIFO_2_STATUS
#define FIFO_7_EMPTY FIFO_1_STATUS
#define FIFO_8_EMPTY FIFO_0_STATUS



// These are the defines for the multifunction control register.
// The 4 MSBs define the function of the register.

#define RECT_HEIGHT         0x0000

#define CLIP_TOP            0x1000
#define CLIP_LEFT           0x2000
#define CLIP_BOTTOM         0x3000
#define CLIP_RIGHT          0x4000


#define DATA_EXTENSION      0xA000
#define ALL_ONES            0x0000
#define CPU_DATA            0x0080
#define DISPLAY_MEMORY      0x00C0

    // Color source

#define BACKGROUND_COLOR    0x00
#define FOREGROUND_COLOR    0x20
#define SRC_CPU_DATA        0x40
#define SRC_DISPLAY_MEMORY  0x60

    // Mix modes

#define NOT_SCREEN              0x00
#define LOGICAL_0               0x01
#define LOGICAL_1               0x02
#define LEAVE_ALONE             0x03
#define NOT_NEW                 0x04
#define SCREEN_XOR_NEW          0x05
#define NOT_SCREEN_XOR_NEW      0x06
#define OVERPAINT               0x07
#define NOT_SCREEN_OR_NOT_NEW   0x08
#define SCREEN_OR_NOT_NEW       0x09
#define NOT_SCREEN_OR_NEW       0x0A
#define SCREEN_OR_NEW           0x0B
#define SCREEN_AND_NEW          0x0C
#define NOT_SCREEN_AND_NEW      0x0D
#define SCREEN_AND_NOT_NEW      0x0E
#define NOT_SCREEN_AND_NOT_NEW  0x0F

//
// General purpose support routines & macros.
//

#define FIFOWAIT(level) while (VideoPortReadPortUshort(GP_STAT) & level);
