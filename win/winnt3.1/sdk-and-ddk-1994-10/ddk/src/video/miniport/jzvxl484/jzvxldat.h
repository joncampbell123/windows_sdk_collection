/*++

Copyright (c) 1991-1993  Microsoft Corporation

Module Name:

    jzvxldat.h

Abstract:

    This module contains all the global data used by the driver.

Environment:

    Kernel mode

Revision History:


--*/


#define BOARD_TYPE_BT484 0x01
#define BOARD_TYPE_BT485 0x02


//
// Video mode table - Lists the information about each individual mode
//

typedef struct _JZVXL_VIDEO_MODES {
    ULONG SupportedBoard;
    ULONG minimumMemoryRequired;
    PVOID ModeSetTable;
    VIDEO_MODE_INFORMATION modeInformation;
} JZVXL_VIDEO_MODES, PJZVXL_VIDEO_MODES;

//
// List of mode indexes.
//

typedef enum _JAG_MODE_LIST {
    mode640_480_32_60 = 0,
    mode640_480_16_60,
    mode640_480_8_60,
    mode640_480_32_72,
    mode640_480_16_72,
    mode640_480_8_72,
    mode800_600_32_60,
    mode800_600_16_60,
    mode800_600_8_60,
    mode800_600_32_72,
    mode800_600_16_72,
    mode800_600_8_72,
    mode1280_1024_8_60,
    mode1280_1024_8_72,
    mode1152_900_16_60,
    mode1152_900_8_60,
    mode1152_900_8_72,
    mode1024_768_16_60,
    mode1024_768_8_60,
    mode1024_768_16_72,
    mode1024_768_8_72,
    JAG_MAX_MODE
} JAG_MODE_LIST;


JZVXL_VIDEO_MODES JagModes[JAG_MAX_MODE] = {

{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485, // Supported Board Types
  0x00080000,                       // Required Video memory for this mode
  v640_480_32_60,                   // Pointer to the modeset info table
    {
      sizeof(VIDEO_MODE_INFORMATION), // Size of the mode informtion structure
      mode640_480_32_60,            // Mode index used in setting the mode
      640,                          // X Resolution, in pixels
      480,                          // Y Resolution, in pixels
      2560,                         // Screen stride, in bytes (distance
                                    // between the start point of two
                                    // consecutive scan lines, in bytes)
      1,                            // Number of video memory planes
      24,                           // Number of bits per plane
      60,                           // Screen Frequency, in Hertz
      330,                          // Horizontal size of screen in millimeters
      240,                          // Vertical size of screen in millimeters
      8,                            // Number Red pixels in DAC
      8,                            // Number Green pixels in DAC
      8,                            // Number Blue pixels in DAC
      0x00ff0000,                   // Mask for Red Pixels in non-palette modes
      0x0000ff00,                   // Mask for Green Pixels in non-palette modes
      0x000000ff,                   // Mask for Blue Pixels in non-palette modes
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR           // Mode description flags.
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v640_480_16_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode640_480_16_60,
      640,
      480,
      1280,
      1,
      16,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00007c00,
      0x000003e0,
      0x0000001f,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v640_480_8_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode640_480_8_60,
      640,
      480,
      640,
      1,
      8,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v640_480_32_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode640_480_32_72,
      640,
      480,
      2560,
      1,
      24,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00ff0000,
      0x0000ff00,
      0x000000ff,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v640_480_16_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode640_480_16_72,
      640,
      480,
      1280,
      1,
      16,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00007c00,
      0x000003e0,
      0x0000001f,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v640_480_8_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode640_480_8_72,
      640,
      480,
      640,
      1,
      8,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v800_600_32_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode800_600_32_60,
      800,
      600,
      3200,
      1,
      24,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00ff0000,
      0x0000ff00,
      0x000000ff,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v800_600_16_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode800_600_16_60,
      800,
      600,
      1600,
      1,
      16,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00007c00,
      0x000003e0,
      0x0000001f,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v800_600_8_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode800_600_8_60,
      800,
      600,
      800,
      1,
      8,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v800_600_32_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode800_600_32_72,
      800,
      600,
      3200,
      1,
      24,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00ff0000,
      0x0000ff00,
      0x000000ff,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v800_600_16_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode800_600_16_72,
      800,
      600,
      1600,
      1,
      16,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00007c00,
      0x000003e0,
      0x0000001f,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v800_600_8_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode800_600_8_72,
      800,
      600,
      800,
      1,
      8,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT485,
  0x00080000,
  v1280_1024_8_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1280_1024_8_60,
      1280,
      1024,
      1280,
      1,
      8,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT485,
  0x00080000,
  v1280_1024_8_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1280_1024_8_72,
      1280,
      1024,
      1280,
      1,
      8,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v1152_900_16_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1152_900_16_60,
      1152,
      900,
      2304,
      1,
      16,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00007c00,
      0x000003e0,
      0x0000001f,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v1152_900_8_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1152_900_8_60,
      1152,
      900,
      1152,
      1,
      8,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v1152_900_8_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1152_900_8_72,
      1152,
      900,
      1152,
      1,
      8,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v1024_768_16_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1024_768_16_60,
      1024,
      768,
      2048,
      1,
      16,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00007c00,
      0x000003e0,
      0x0000001f,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v1024_768_8_60,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1024_768_8_60,
      1024,
      768,
      1024,
      1,
      8,
      60,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v1024_768_16_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1024_768_16_72,
      1024,
      768,
      2048,
      1,
      16,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00007c00,
      0x000003e0,
      0x0000001f,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR
    }
},
{
  BOARD_TYPE_BT484 | BOARD_TYPE_BT485,
  0x00080000,
  v1024_768_8_72,
    {
      sizeof(VIDEO_MODE_INFORMATION),
      mode1024_768_8_72,
      1024,
      768,
      1024,
      1,
      8,
      72,
      330,
      240,
      8,
      8,
      8,
      0x00000000,
      0x00000000,
      0x00000000,
      VIDEO_MODE_GRAPHICS | VIDEO_MODE_COLOR | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE             
    }
} 

};
