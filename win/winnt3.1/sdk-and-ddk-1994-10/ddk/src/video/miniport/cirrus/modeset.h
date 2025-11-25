/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    Modeset.h

Abstract:

    This module contains all the global data used by the Cirrus Logic
   CL-6410 and CL-6420 driver.

Environment:

    Kernel mode

Revision History:

--*/
//---------------------------------------------------------------------------
//
// only one banking variable must be defined
//
#if TWO_32K_BANKS
#if ONE_64K_BANK
#error !!ERROR: two types of banking defined!
#endif
#elif ONE_64K_BANK
#else
#error !!ERROR: banking type must be defined!
#endif

//---------------------------------------------------------------------------

#define INT10_MODE_SET

#ifndef INT10_MODE_SET
#error !!ERROR: int10_mode_set not defined!
#endif

#include "cmdcnst.h"

//---------------------------------------------------------------------------
//
//        The actual register values for the supported modes are in chipset-specific
//        include files:
//
//                mode64xx.h has values for CL6410 and CL6420
//                mode542x.h has values for CL5422, CL5424, and CL5426
//
#include "mode6410.h"
#include "mode6420.h"
#include "mode542x.h"

USHORT MODESET_1K_WIDE[] = {
    OW,                             // stretch scans to 1k
    CRTC_ADDRESS_PORT_COLOR,
    0x8013,

    EOD
};

//---------------------------------------------------------------------------
//
// Memory map table -
//
// These memory maps are used to save and restore the physical video buffer.
//

//
// Memory map table definition
//

typedef struct {
    ULONG   MaxSize;        // Maximum addressable size of memory
    ULONG   Start;          // Start address of display memory
} MEMORYMAPS;

MEMORYMAPS MemoryMaps[] = {

//               length      start
//               ------      -----
    {           0x08000,    0xB0000},   // all mono text modes (7)
    {           0x08000,    0xB8000},   // all color text modes (0, 1, 2, 3,
    {           0x20000,    0xA0000},   // all VGA graphics modes
};

//
// Video mode table - contains information and commands for initializing each
// mode. These entries must correspond with those in VIDEO_MODE_VGA. The first
// entry is commented; the rest follow the same format, but are not so
// heavily commented.
//

VIDEOMODE ModesVGA[] = {

// Mode index 0
// Color text mode 3, 720x400, 9x16 char cell (VGA).
//
{
  VIDEO_MODE_COLOR,  // flags that this mode is a color mode, but not graphics
  4,                 // four planes
  1,                 // one bit of colour per plane
  80, 25,            // 80x25 text resolution
  720, 400,          // 720x400 pixels on screen
  160, 0x10000,      // 160 bytes per scan line, 64K of CPU-addressable bitmap
  0, 0,              // only support one frequency, non-interlaced
  0,                 // montype is 'dont care' for text modes
  NoBanking,         // no banking supported or needed in this mode
  MemMap_CGA,        // the memory mapping is the standard CGA memory mapping
                     //  of 32K at B8000
  CL6410 | CL6420 | CL542x,
  crt | panel,
  FALSE,              // ModeValid default is always off
  { 3,3,3},          // int10 BIOS modes
  { CL6410_80x25Text_crt, CL6410_80x25Text_panel,
   CL6420_80x25Text_crt, CL6420_80x25Text_panel,
   CL542x_80x25Text, 0 },
},

//
// Mode index 1.
// Color text mode 3, 640x350, 8x14 char cell (EGA).
//
{  VIDEO_MODE_COLOR,  // flags that this mode is a color mode, but not graphics
  4,                 // four planes
  1,                 // one bit of colour per plane
  80, 25,            // 80x25 text resolution
  640, 350,          // 640x350 pixels on screen
  160, 0x10000,      // 160 bytes per scan line, 64K of CPU-addressable bitmap
  0, 0,              // only support one frequency, non-interlaced
  0,                 // montype is 'dont care' for text modes
  NoBanking,         // no banking supported or needed in this mode
  MemMap_CGA,        // the memory mapping is the standard CGA memory mapping
                     //  of 32K at B8000
   CL6410 | CL6420 | CL542x,
   crt | panel,
   FALSE,              // ModeValid default is always off
  { 3,3,3},             // int10 BIOS modes
   { CL6410_80x25_14_Text_crt, CL6410_80x25_14_Text_panel,
     CL6420_80x25_14_Text_crt, CL6420_80x25_14_Text_panel,
     CL542x_80x25_14_Text, 0 },
},
//
//
// Mode index 2
// Standard VGA Color graphics mode 0x12, 640x480 16 colors.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 80, 30,
  640, 480, 80, 0x10000, 
  60, 0,              // 60hz, non-interlaced
  3,                  // montype 
  NoBanking, MemMap_VGA,
  CL6410 | CL6420 | CL542x,
  crt | panel,
  FALSE,                      // ModeValid default is always off
  { 0x12,0x12,0x12},          // int10 BIOS modes
  { CL6410_640x480_crt, CL6410_640x480_panel,
   CL6420_640x480_crt, CL6420_640x480_panel,
   CL542x_640x480, 0 },
},

// Mode index 2b
// Standard VGA Color graphics mode 0x12, 640x480 16 colors.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 80, 30,
  640, 480, 80, 0x10000, 
  72, 0,              // 72hz, non-interlaced
  4,                  // montype 
  NoBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                      // ModeValid default is always off
  { 0,0,0x12},                // int10 BIOS modes
  { NULL, NULL,
   NULL, NULL,
   CL542x_640x480, 0 },
},


//
// Beginning of SVGA modes
//

//
// Mode index 3
// 800x600 16 colors.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 100, 37,
  800, 600, 100, 0x10000, 
  56, 0,              // 56hz, non-interlaced
  3,                  // montype 
  NoBanking, MemMap_VGA,
  CL6410 | CL6420 | CL542x,
  crt,
  FALSE,                   // ModeValid default is always off
  { 0x6a,0x6a,0x6a},       // int10 BIOS modes
  { CL6410_800x600_crt, NULL,
   CL6420_800x600_crt, NULL,
   CL542x_800x600, 0 },
},

//
// Mode index 3b
// 800x600 16 colors.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 100, 37,
  800, 600, 100, 0x10000, 
  60, 0,              // 60hz, non-interlaced
  4,                  // montype 
  NoBanking, MemMap_VGA,
  CL6420 | CL542x,
  crt,
  FALSE,                   // ModeValid default is always off
  { 0,0x6a,0x6a},          // int10 BIOS modes
  { NULL, NULL,
   CL6420_800x600_crt, NULL,
   CL542x_800x600, 0 },
},

//
// Mode index 3c
// 800x600 16 colors.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 100, 37,
  800, 600, 100, 0x10000, 
  72, 0,              // 72hz, non-interlaced
  5,                  // montype 
  NoBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                   // ModeValid default is always off
  { 0,0,0x6a},             // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_800x600, 0 },
},

//
// Mode index 4
// 1024x768 non-interlaced 16 colors.
// Assumes 512K.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 128, 48,
  1024, 768, 128, 0x20000, 
  60, 0,              // 60hz, non-interlaced
  5,                  // montype 
  NormalBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x5d},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_1024x768, 0 },
},

//
// Mode index 4b
// 1024x768 non-interlaced 16 colors.
// Assumes 512K.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 128, 48,
  1024, 768, 128, 0x20000, 
  70, 0,              // 70hz, non-interlaced
  6,                  // montype 
  NormalBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x5d},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
   CL542x_1024x768, 0 },
},

//
// Mode index 4c
// 1024x768 non-interlaced 16 colors.
// Assumes 512K.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 128, 48,
  1024, 768, 128, 0x20000, 
  72, 0,              // 72hz, non-interlaced
  7,                  // montype 
  NormalBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x5d},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_1024x768, 0 },
},

//
// Mode index 4d
// 1024x768 interlaced 16 colors.
// Assumes 512K.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 128, 48,
  1024, 768, 128, 0x20000, 
  45, 1,              // 45hz, interlaced
  4,                  // montype 
  NormalBanking, MemMap_VGA,
  CL6420 | CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0x37,0x5d},       // int10 BIOS modes
  { NULL, NULL,
   CL6420_1024x768_crt, NULL,
   CL542x_1024x768, 0 },
},

//
// Mode index 5
// 1280x1024 interlaced 16 colors.
// Assumes 1meg required.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 4, 1, 128, 48,
  1280, 1024, 160, 0x30000, 
  45, 1,              // 45Hz, interlaced
  5,                  // montype 
  NormalBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x6c},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_1280x1024_I, 0},
},

//
//
// Mode index 6
// VGA Color graphics,        640x480 256 colors. 1K scan line
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 80, 30,
  640, 480, 1024, 0x80000, 
  60, 0,              // 60hz, non-interlaced
  3,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL6420 | CL542x,
  crt | panel,
  FALSE,                // ModeValid default is always off
  { 0,0x2e,0x5f},       // int10 BIOS modes
  { NULL, NULL,
    CL6420_640x480_256color_crt, CL6420_640x480_256color_panel,
    CL542x_640x480_256, MODESET_1K_WIDE },
},

//
//
// Mode index 6b
// VGA Color graphics,        640x480 256 colors. 1K scan line
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 80, 30,
  640, 480, 1024, 0x80000, 
  72, 0,              // 72hz, non-interlaced
  4,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x5f},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_640x480_256, MODESET_1K_WIDE },
},

//
// Mode index 7
// 800x600 256 colors. 1K scan line requires 1 MEG
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 100, 37,
  800, 600, 1024, 0x100000, 
  56, 0,              // 56hz, non-interlaced
  3,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL6420 | CL542x,
  crt,
  FALSE,                   // ModeValid default is always off
  { 0,0x30,0x5c},          // int10 BIOS modes
  { NULL, NULL,
    CL6420_800x600_256color_crt, NULL,
    CL542x_800x600_256, MODESET_1K_WIDE },
},

//
// Mode index 7b
// 800x600 256 colors. 1K scan line requires 1 MEG
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 100, 37,
  800, 600, 1024, 0x100000, 
  60, 0,              // 60hz, non-interlaced
  4,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL6420 | CL542x,
  crt,
  FALSE,                   // ModeValid default is always off
  { 0,0x30,0x5c},          // int10 BIOS modes
  { NULL, NULL,
    CL6420_800x600_256color_crt, NULL,
    CL542x_800x600_256, MODESET_1K_WIDE },
},

//
// Mode index 7c
// 800x600 256 colors. 1K scan line requires 1 MEG
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 100, 37,
  800, 600, 1024, 0x100000, 
  72, 0,              // 72hz, non-interlaced
  5,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                   // ModeValid default is always off
  { 0,0,0x5c},             // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_800x600_256, MODESET_1K_WIDE },
},
//
// Mode index 8
// 1024x768 non-interlaced 256 colors.
// Assumes 1Meg.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 128, 48,
  1024, 768, 1024, 0x100000, 
  60, 0,              // 60hz, non-interlaced
  5,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x60},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_1024x768_256, 0 },
},

//
// Mode index 8b
// 1024x768 non-interlaced 256 colors.
// Assumes 1Meg.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 128, 48,
  1024, 768, 1024, 0x100000, 
  70, 0,              // 70hz, non-interlaced
  6,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x60},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_1024x768_256, 0 },
},

//
// Mode index 8
// 1024x768 non-interlaced 256 colors.
// Assumes 1Meg.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 128, 48,
  1024, 768, 1024, 0x100000, 
  72, 0,              // 72hz, non-interlaced
  7,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0,0x60},          // int10 BIOS modes
  { NULL, NULL,
    NULL, NULL,
    CL542x_1024x768_256, 0 },
},

//
// Mode index 8
// 1024x768 interlaced 256 colors.
// Assumes 1Meg.
//
{ VIDEO_MODE_COLOR+VIDEO_MODE_GRAPHICS, 1, 8, 128, 48,
  1024, 768, 1024, 0x100000, 
  45, 1,              // 45hz, interlaced
  4,                  // montype 
  PlanarHCBanking, MemMap_VGA,
  CL6420 | CL542x,
  crt,
  FALSE,                // ModeValid default is always off
  { 0,0x38,0x60},       // int10 BIOS modes
  { NULL, NULL,
    CL6420_1024x768_256color_crt, NULL,
    CL542x_1024x768_256, 0 },
},

};


ULONG NumVideoModes = sizeof(ModesVGA) / sizeof(VIDEOMODE);


//
//
// Data used to set the Graphics and Sequence Controllers to put the
// VGA into a planar state at A0000 for 64K, with plane 2 enabled for
// reads and writes, so that a font can be loaded, and to disable that mode.
//

// Settings to enable planar mode with plane 2 enabled.
//

USHORT EnableA000Data[] = {
    OWM,
    SEQ_ADDRESS_PORT,
    1,
    0x0100,

    OWM,
    GRAPH_ADDRESS_PORT,
    3,
    0x0204,     // Read Map = plane 2
    0x0005, // Graphics Mode = read mode 0, write mode 0
    0x0406, // Graphics Miscellaneous register = A0000 for 64K, not odd/even,
            //  graphics mode
    OWM,
    SEQ_ADDRESS_PORT,
    3,
    0x0402, // Map Mask = write to plane 2 only
    0x0404, // Memory Mode = not odd/even, not full memory, graphics mode
    0x0300,  // end sync reset
    EOD
};

//
// Settings to disable the font-loading planar mode.
//

USHORT DisableA000Color[] = {
    OWM,
    SEQ_ADDRESS_PORT,
    1,
    0x0100,

    OWM,
    GRAPH_ADDRESS_PORT,
    3,
    0x0004, 0x1005, 0x0E06,

    OWM,
    SEQ_ADDRESS_PORT,
    3,
    0x0302, 0x0204, 0x0300,  // end sync reset
    EOD

};
