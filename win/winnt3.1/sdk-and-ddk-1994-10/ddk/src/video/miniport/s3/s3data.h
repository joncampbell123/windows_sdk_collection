/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    s3data.h

Abstract:

    This module contains all the global data used by the S3 driver.

Environment:

    Kernel mode

Revision History:


--*/


#include "cmdcnst.h"


/*****************************************************************************
 * Command table to get ready for VGA mode
 * this is only used for the 911/924 chips
 ****************************************************************************/
USHORT s3_set_vga_mode[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the SC regs
    0x3d4, 0xa539,

    OB,                                 // Enable the S3 graphics engine
    0x3d4, 0x40,

    METAOUT+MASKOUT,
    0x3d5, 0xfe, 0x01,

    SELECTACCESSRANGE + ADVANCEDFUNCTIONCONTROL,

    OB,                                 // reset to normal VGA operation
    0x4ae8, 0x02,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OB,                                 // Disable the S3 graphics engine
    0x3d4, 0x40,

    METAOUT+MASKOUT,
    0x3d5, 0xfe, 0x00,

    OB,                                 // Memory Control
    0x3d4, 0x31,

    METAOUT+MASKOUT,
    0x3d5, 0x75, 0x85,

    OB,                                 // Backward Compat 1
    0x3d4, 0x32,

    METAOUT+MASKOUT,
    0x3d5, 0x40, 0x00,

    OW,                                 // Backward Compat 2
    0x3d4, 0x0033,

    OW,                                 // Backward Compat 3
    0x3d4, 0x0034,

    OW,                                 // CRTC Lock
    0x3d4, 0x0035,

    OB,                                 // S3 Misc 1
    0x3d4, 0x3a,

    METAOUT+MASKOUT,
    0x3d5, 0x88, 0x05,

    OW,                                 // Data Transfer Exec Pos
    0x3d4, 0x5a3b,

    OW,                                 // Interlace Retrace start
    0x3d4, 0x103c,

    OW,                                 // Extended Mode
    0x3d4, 0x0043,

    OW,                                 // HW graphics Cursor Mode
    0x3d4, 0x0045,

    OW,                                 // HW graphics Cursor Orig x
    0x3d4, 0x0046,

    OW,                                 // HW graphics Cursor Orig x
    0x3d4, 0xff47,

    OW,                                 // HW graphics Cursor Orig y
    0x3d4, 0xfc48,

    OW,                                 // HW graphics Cursor Orig y
    0x3d4, 0xff49,

    OW,                                 // HW graphics Cursor Orig y
    0x3d4, 0xff4a,

    OW,                                 // HW graphics Cursor Orig y
    0x3d4, 0xff4b,

    OW,                                 // HW graphics Cursor Orig y
    0x3d4, 0xff4c,

    OW,                                 // HW graphics Cursor Orig y
    0x3d4, 0xff4d,

    OW,                                 // Dsp Start x pixel pos
    0x3d4, 0xff4e,

    OW,                                 // Dsp Start y pixel pos
    0x3d4, 0xdf4d,

    OB,                                 // MODE-CNTL
    0x3d4, 0x42,

    METAOUT+MASKOUT,
    0x3d5, 0xdf, 0x00,

    EOD

};

//
// Mode tables are only necessary for non-x86.
// This allows us to save lots of space in memory

#ifndef i386

/*****************************************************************************
 * S3 - 911 Enhanced mode init.
 ****************************************************************************/
USHORT  S3_911_Enhanced_Mode[] = {
    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Make the screen dark
    0x3c6, 0x00,

    OW,                                 // Turn off the screen
    0x3c4, 0x2101,

    METAOUT+VBLANK,                     // Wait for the 911 to settle down.
    METAOUT+VBLANK,

    OW,                                 // Async Reset
    0x3c4, 0x0100,

    OWM,                                // Sequencer Registers
    0x3c4,
    4,
    0x2101, 0x0F02, 0x0003, 0x0e04,

    METAOUT+SETCRTC,                    // Program the CRTC regs

    SELECTACCESSRANGE + SYSTEMCONTROL,

    IB,                                 // Prepare to prgram the ACT
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    METAOUT+ATCOUT,                     // Program the ATC
    0x3c0,
    21, 0,
    0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x41, 0x00, 0x0f, 0x00,
    0x00,

    OW,                                 // Start the sequencer
    0x3c4, 0x300,

    OWM,                                // Program the GDC
    0x3ce,
    9,
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004,
    0x0005, 0x0506, 0x0f07, 0xff08,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    IB,                                 // Set ATC FF to index
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Enable the palette
    0x3c0, 0x20,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock S3 SC regs
    0x3d4, 0xa039,

    OB,                                 // Enable 8514/a reg access
    0x3d4, 0x40,

    METAOUT+MASKOUT,
    0x3d5, 0xfe, 0x01,

    OB,                                 // Turn off H/W Graphics Cursor
    0x3d4, 0x45,

    METAOUT+MASKOUT,
    0x3d5, 0xfe, 0x0,

    OW,                                 // Set the graphic cursor fg color
    0x3d4, 0xff0e,

    OW,                                 // Set the graphic cursor bg color
    0x3d4, 0x000f,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OB,                                 // Set the Misc 1 reg
    0x3d4, 0x3a,

    METAOUT+MASKOUT,
    0x3d5, 0xe2, 0x15,

    OB,                                 // Disable 2K X 1K X 4 plane
    0x3d4, 0x31,

    METAOUT+MASKOUT,
    0x3d5, 0xe4, 0x08,

    OB,                                 // Disable multiple pages
    0x3d4, 0x32,

    METAOUT+MASKOUT,
    0x3d5, 0xbf, 0x0,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    SELECTACCESSRANGE + ADVANCEDFUNCTIONCONTROL,

    OW,                                 // Set either 800X600 or 1024X768
    0x4ae8, 0x07,                       // hi-res mode.

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Set Misc out reg for external clock
    0x3c2, 0x2f,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the SC regs
    0x3d4, 0xa039,

    METAOUT+SETCLK,                     // Set the clock for 65 Mhz

    METAOUT+VBLANK,                     // Wait for the clock to settle down
    METAOUT+VBLANK,                     // S3 product alert Synchronization &
    METAOUT+VBLANK,                     // Clock Skew.
    METAOUT+VBLANK,
    METAOUT+VBLANK,
    METAOUT+VBLANK,

    OW,                                 // Lock the SC regs
    0x3d4, 0x0039,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Turn on the screen - in the sequencer
    0x3c4, 0x01,

    METAOUT+MASKOUT,
    0x3c5, 0xdf, 0x0,

    METAOUT+VBLANK,                     // Wait the monitor to settle down
    METAOUT+VBLANK,

    OW,                                 // Enable all the planes through the DAC
    0x3c6, 0xff,

    EOD

};

/*****************************************************************************
 * S3 - 801 Enhanced mode init.
 ****************************************************************************/
USHORT  S3_801_Enhanced_Mode[] = {

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Make the screen dark
    0x3c6, 0x00,

    OW,                                 // Turn off the screen
    0x3c4, 0x2101,

    METAOUT+VBLANK,                     // Wait for the 911 to settle down.
    METAOUT+VBLANK,

    OW,                                 // Async Reset
    0x3c4, 0x0100,

    OWM,                                // Sequencer Registers
    0x3c4,
    4,
    0x2101, 0x0F02, 0x0003, 0x0e04,

    METAOUT+SETCRTC,                    // Program the CRTC regs

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OWM,
    0x3d4,
    17,
    0xA039, 0x0e42, 0x403c, 0x8931, 0x153a,
    0x0050, 0x4854, 0x2f60, 0x8161, 0x0062,
    0x0058, 0x0033, 0x0043, 0x8013, 0x0051,
    0x005c, 0x1034,

    OW,
    0x3d4, 0x0a5a,                      // Set the low byte of the LAW

    OW,
    0x3d4, 0x0059,                      // Set the high byte of the LAW

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Lock more S3 specific regs
    0x3d4, 0x0039,

    IB,                                 // Prepare to prgram the ACT
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    METAOUT+ATCOUT,                     // Program the ATC
    0x3c0,
    21, 0,
    0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x41, 0x00, 0x0f, 0x00,
    0x00,

    OW,                                 // Start the sequencer
    0x3c4, 0x300,

    OWM,                                // Program the GDC
    0x3ce,
    9,
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004,
    0x0005, 0x0506, 0x0f07, 0xff08,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    IB,                                 // Set ATC FF to index
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Enable the palette
    0x3c0, 0x20,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock S3 SC regs
    0x3d4, 0xa039,

    OB,                                 // Enable 8514/a reg access
    0x3d4, 0x40,

    METAOUT+MASKOUT,
    0x3d5, 0xfe, 0x01,

    OB,                                 // Turn off H/W Graphics Cursor
    0x3d4, 0x45,

    METAOUT+MASKOUT,
    0x3d5, 0xfe, 0x0,

    OW,                                 // Set the graphic cursor fg color
    0x3d4, 0xff0e,

    OW,                                 // Set the graphic cursor bg color
    0x3d4, 0x000f,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OB,                                 // Set the Misc 1 reg
    0x3d4, 0x3a,

    METAOUT+MASKOUT,
    0x3d5, 0xe2, 0x15,

    OB,                                 // Disable 2K X 1K X 4 plane
    0x3d4, 0x31,

    METAOUT+MASKOUT,
    0x3d5, 0xe4, 0x08,

    OB,                                 // Disable multiple pages
    0x3d4, 0x32,

    METAOUT+MASKOUT,
    0x3d5, 0xbf, 0x0,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    SELECTACCESSRANGE + ADVANCEDFUNCTIONCONTROL,

    OW,                                 // Set either 800X600 or 1024X768
    0x4ae8, 0x07,                       // hi-res mode.

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Set Misc out reg for external clock
    0x3c2, 0xef,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the SC regs
    0x3d4, 0xa039,

    METAOUT+SETCLK,                     // Set the clock for 65 Mhz

    METAOUT+VBLANK,                     // Wait for the clock to settle down
    METAOUT+VBLANK,                     // S3 product alert Synchronization &
    METAOUT+VBLANK,                     // Clock Skew.
    METAOUT+VBLANK,
    METAOUT+VBLANK,
    METAOUT+VBLANK,

    OW,                                 // Lock the SC regs
    0x3d4, 0x0039,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Turn on the screen - in the sequencer
    0x3c4, 0x01,

    METAOUT+MASKOUT,
    0x3c5, 0xdf, 0x0,

    METAOUT+VBLANK,                     // Wait the monitor to settle down
    METAOUT+VBLANK,

    OW,                                 // Enable all the planes through the DAC
    0x3c6, 0xff,

    EOD

};

/*****************************************************************************
 * S3 - 928 1024 X 768, 800 X 600, & 640 X 480 Enhanced mode init.
 ****************************************************************************/
USHORT  S3_928_Enhanced_Mode[] = {

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Make the screen dark
    0x3c6, 0x00,

    OW,                                 // Async Reset
    0x3c4, 0x0100,

    OWM,                                // Sequencer Registers
    0x3c4, 5,
    0x0300, 0x0101, 0x0F02, 0x0003, 0x0e04,

    METAOUT+INDXOUT,                    // Program the GDC
    0x3ce,
    9, 0,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x05, 0x0f, 0xff,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    METAOUT+SETCRTC,                    // Program the CRTC regs

    OW,                                 // Set mode control
    0X3D4, 0x0242,                      // dot clock select

    OW,                                 // memory configuration reg
    0X3D4, 0x8D31,

    OW,                                 // extended system control reg
    0X3D4, 0x0050,

    OW,                                 // backward compatibility 2 reg
    0X3D4, 0x2033,

    OB,                                 // extended mode reg
    0x3D4, 0x43,

    METAOUT+MASKOUT,
    0x3D5, 0x10, 0x00,

    OW,                                 // extended system control reg 2
    0X3D4, 0x4051,

    OW,                                 // general output port
    0X3D4, 0x025c,

    // METAOUT+BUSTEST,

    OW,
    0x3d4, 0x0a5a,                      // Set the low byte of the LAW

    OW,
    0x3d4, 0x0059,                      // Set the high byte of the LAW

    IB,                                 // Prepare to prgram the ACT
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    METAOUT+ATCOUT,                     // Program the ATC
    0x3c0,
    21, 0,
    0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x41, 0x00, 0x0f, 0x00,
    0x00,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    IB,                                 // Set ATC FF to index
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Enable the palette
    0x3c0, 0x20,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Enable 8514/a reg access
    0x3d4, 0x0140,

    OB,                                 // Turn off H/W Graphics Cursor
    0x3d4, 0x45,

    METAOUT+MASKOUT,
    0x3d5, 0xfe, 0x0,

    OW,                                 // Set the graphic cursor fg color
    0x3d4, 0xff0e,

    OW,                                 // Set the graphic cursor bg color
    0x3d4, 0x000f,

    OB,                                 // Set the Misc 1 reg
    0x3d4, 0x3a,

    METAOUT+MASKOUT,
    0x3d5, 0x62, 0x15,

    OB,                                 // Disable 2K X 1K X 4 plane
    0x3d4, 0x31,

    METAOUT+MASKOUT,
    0x3d5, 0xe4, 0x08,

    OB,                                 // Disable multiple pages
    0x3d4, 0x32,

    METAOUT+MASKOUT,
    0x3d5, 0xbf, 0x0,

    SELECTACCESSRANGE + ADVANCEDFUNCTIONCONTROL,

    OW,                                 // Set either 800X600 or 1024X768
    0x4ae8, 0x07,                       // hi-res mode.

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Set Misc out reg for external clock
    0x3c2, 0xef,

    METAOUT+SETCLK,                     // Set the clock

    METAOUT+DELAY,                      // Wait for the clock to settle down
    0x400,                              // S3 product alert Synchronization &
                                        // Clock Skew.
    METAOUT+VBLANK,
    METAOUT+VBLANK,

    METAOUT+MASKOUT,
    0x3c5, 0xdf, 0x0,

    METAOUT+DELAY,                      // Wait for about 1 millisecond
    0x400,                              // for the monitor to settle down

    OW,                                 // Enable all the planes through the DAC
    0x3c6, 0xff,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Lock more S3 specific regs
    0x3d4, 0x0039,

    EOD

};


/*****************************************************************************
 * S3 - 928 1280 X 1024 Enhanced mode init.
 ****************************************************************************/
USHORT  S3_928_1280_Enhanced_Mode[] = {

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Make the screen dark
    0x3c6, 0x00,

    OW,                                 // Async Reset
    0x3c4, 0x0100,

    OWM,                                // Sequencer Registers
    0x3c4,
    5,
    0x0300, 0x0101, 0x0F02, 0x0003, 0x0e04,

    METAOUT+INDXOUT,                    // Program the GDC
    0x3ce,
    9, 0,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x05, 0x0f, 0xff,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    METAOUT+SETCRTC,                    // Program the CRTC regs

    // Set the Bt 485 DAC.

    OW,                                 // hardware graphics cursor mode reg
    0X3D4, 0x2045,

    OW,                                 // Enable access to Bt 485 CmdReg3
    0x3D4, 0x2955,                      // disable the DAC

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,
    0x3C6, 0x80,                        // Bt 485 - CR0

    METAOUT+DELAY,
    0x400,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // S3 extended video DAC control reg
    0x3D4, 0x2A55,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,
    0x3C8, 0x40,                        // Bt 485 - CR1

    METAOUT+DELAY,
    0x400,

    OB,
    0x3C9, 0x30,                        // Bt 485 - CR2

    METAOUT+DELAY,
    0x400,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // S3 extened video DAC control reg
    0x3D4, 0x2855,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Bt 485
    0x3c8, 0x01,

    METAOUT+DELAY,
    0x400,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // S3 extened video DAC control reg
    0x3D4, 0x2A55,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Bt 485 - CR3
    0x3c6, 0x08,

    METAOUT+DELAY,
    0x400,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Reset the palette index
    0x3d4, 0x2855,

    OW,                                 // Set mode control
    0X3D4, 0x0242,                      // dot clock select

    METAOUT+DELAY,
    0x400,

    OW,                                 // memory configuration
    0X3D4, 0x8f31,

    OW,
    0X3D4, 0x153a,

    OW,                                 // extended system control reg
    0X3D4, 0x0050,

    OW,                                 // backward compatibility reg
    0X3D4, 0x2033,

    OB,                                 // extended mode reg
    0x3D4, 0x43,

    METAOUT+MASKOUT,
    0x3D5, 0x10, 0x00,

    OW,                                 // extended system control reg 2
    0X3D4, 0x5051,

    OW,
    0X3D4, 0x025c,                      // flash bits, 20 packed mode.

    // METAOUT+BUSTEST,

    OW,
    0x3d4, 0x0a5a,                      // Set the low byte of the LAW

    OW,
    0x3d4, 0x0059,                      // Set the high byte of the LAW

    IB,                                 // Prepare to prgram the ATC
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    METAOUT+ATCOUT,                     // Program the ATC
    0x3c0,
    21, 0,
    0x00, 0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x41, 0x00, 0x0f, 0x00,
    0x00,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    IB,                                 // Set ATC FF to index
    0x3da,

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Enable the palette
    0x3c0, 0x20,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Enable 8514/a reg access
    0x3d4, 0x0140,

    SELECTACCESSRANGE + ADVANCEDFUNCTIONCONTROL,

    OW,                                 // Galen said set to 0
    0x4ae8, 0x03,                       //

    SELECTACCESSRANGE + VARIOUSVGA,

    OB,                                 // Set Misc out reg for external clock
    0x3c2, 0xef,

    METAOUT+SETCLK,                     // Set the clock

    METAOUT+DELAY,                      // Wait for the clock to settle down
    0x400,                              // S3 product alert Synchronization &
                                        // Clock Skew.
    METAOUT+VBLANK,
    METAOUT+VBLANK,

    METAOUT+MASKOUT,
    0x3c5, 0xdf, 0x0,

    METAOUT+DELAY,                      // Wait for about 1 millisecond
    0x400,                              // for the monitor to settle down

    OW,                                 // Enable all the planes through the DAC
    0x3c6, 0xff,

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Lock more S3 specific regs
    0x3d4, 0x0039,

    EOD

};

/******************************************************************************
 * 911/924 CRTC Values
 *****************************************************************************/

USHORT crtc911_640x480x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x5a3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x5f, 0x4f, 0x50, 0x82, 0x54,
    0x80, 0x0b, 0x3e, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xea, 0x8c, 0xdf, 0x80,
    0x60, 0xe7, 0x04, 0xab, 0xff,

    EOD
};

USHORT crtc911_800x600x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x7a3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x7f, 0x63, 0x64, 0x82, 0x6a,
    0x1a, 0x74, 0xf0, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x58, 0x8c, 0x57, 0x80,
    0x00, 0x57, 0x73, 0xe3, 0xff,


    EOD
};

USHORT crtc911_1024x768x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x9f3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC
    0x3d4,
    25, 0,
    0xa4, 0x7f, 0x80, 0x87, 0x84,
    0x95, 0x25, 0xf5, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x87, 0xff, 0x80,
    0x60, 0xff, 0x21, 0xab, 0xff,

    EOD
};



USHORT crtc911_640x480x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x5e3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x63, 0x4f, 0x50, 0x86, 0x53,
    0x97, 0x07, 0x3e, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xe8, 0x8b, 0xdf, 0x80,
    0x60, 0xdf, 0x07, 0xab, 0xff,


    EOD
};

USHORT crtc911_800x600x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x783b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x7d, 0x63, 0x64, 0x80, 0x69,
    0x1a, 0x98, 0xf0, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x7c, 0xa2, 0x57, 0x80,
    0x00, 0x57, 0x98, 0xe3, 0xff,



    EOD
};

USHORT crtc911_1024x768x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x9d3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Lock S3 specific regs
    0x3d4, 0x0038,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0xa2, 0x7f, 0x80, 0x85, 0x84,
    0x95, 0x24, 0xf5, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x02, 0x88, 0xff, 0x80,
    0x60, 0xff, 0x24, 0xab, 0xff,

    EOD
};

/*****************************************************************************
 * 801 / 805 CRTC values
 ****************************************************************************/

USHORT crtc801_640x480x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x5a3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x5f, 0x4f, 0x50, 0x82, 0x54,
    0x80, 0x0b, 0x3e, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xea, 0x8c, 0xdf, 0x80,
    0x60, 0xe7, 0x04, 0xab, 0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};

USHORT crtc801_640x480x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x5e3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x63, 0x4f, 0x50, 0x86, 0x53,
    0x97, 0x07, 0x3e, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xe8, 0x8b, 0xdf, 0x80,
    0x60, 0xdf, 0x07, 0xab, 0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};



USHORT crtc801_800x600x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x7a3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x7f, 0x63, 0x64, 0x82,
    0x6a, 0x1a, 0x74, 0xf0,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x58, 0x8c, 0x57, 0x80,
    0x00, 0x57, 0x73, 0xe3,
    0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};

USHORT crtc801_800x600x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x783b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x7d, 0x63, 0x64, 0x80,
    0x6c, 0x1b, 0x98, 0xf0,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x7c, 0xa2, 0x57, 0x80,
    0x00, 0x57, 0x98, 0xe3,
    0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};




USHORT crtc801_1024x768x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x9d3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0xa3, 0x7f, 0x80, 0x86,
    0x84, 0x95, 0x25, 0xf5,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x02, 0x87, 0xff, 0x80,
    0x60, 0xff, 0x21, 0xeb,
    0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};

USHORT crtc801_1024x768x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x9d3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0xa1, 0x7f, 0x80, 0x84,
    0x84, 0x95, 0x24, 0xf5,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x0b, 0x00,
    0x02, 0x88, 0xff, 0x80,
    0x60, 0xff, 0x24, 0xeb,
    0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};

/*****************************************************************************
 * 928 CRTC values
 ****************************************************************************/

USHORT crtc928_640x480x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x5a3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x5f, 0x4f, 0x50, 0x82, 0x54,
    0x80, 0x0b, 0x3e, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xea, 0x8c, 0xdf, 0x80,
    0x60, 0xe7, 0x04, 0xab, 0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};

USHORT crtc928_640x480x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x5e3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x63, 0x4f, 0x50, 0x86, 0x53,
    0x97, 0x07, 0x3e, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xe8, 0x8b, 0xdf, 0x80,
    0x60, 0xdf, 0x07, 0xab, 0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};



USHORT crtc928_800x600x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x7a3b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x7f, 0x63, 0x64, 0x82,
    0x6a, 0x1a, 0x74, 0xf0,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x58, 0x8c, 0x57, 0x80,
    0x00, 0x57, 0x73, 0xe3,
    0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};

USHORT crtc928_800x600x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // Unlock the S3 specific regs
    0x3d4, 0x4838,

    OW,                                 // Unlock the more S3 specific regs
    0x3d4, 0xA039,

    OW,                                 // Data Xfer Execution Position reg
    0x3d4, 0x783b,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x1034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x7d, 0x63, 0x64, 0x80,
    0x6c, 0x1b, 0x98, 0xf0,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x7c, 0xa2, 0x57, 0x80,
    0x00, 0x57, 0x98, 0xe3,
    0xff,

    OW,
    0X3D4, 0x005d,

    OW,
    0X3D4, 0x005e,

    EOD
};



/******************************************************************************
 * CRTC values for S3-928 in 1024x768 @ 60Hz
 *****************************************************************************/
USHORT crtc928_1024x768x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x0034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0xa3, 0x7f, 0x80, 0x86,
    0x84, 0x95, 0x25, 0xf5,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x02, 0x07, 0xff, 0x80,
    0x60, 0xff, 0x21, 0xeb,
    0xff,

    OW,                                 // overlfow regs
    0X3D4, 0x005d,

    OW,                                 // more overflow regs
    0X3D4, 0x005e,

    EOD
};

/******************************************************************************
 * CRTC values for S3-928 in 1024x768 @ 70Hz
 *****************************************************************************/
USHORT crtc928_1024x768x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x0034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0xa1, 0x7f, 0x80, 0x84,
    0x84, 0x95, 0x24, 0xf5,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x0b, 0x00,
    0x02, 0x88, 0xff, 0x80,
    0x60, 0xff, 0x24, 0xeb,
    0xff,

    OW,                                 // overflow regs
    0X3D4, 0x005d,

    OW,                                 // more overflow regs
    0X3D4, 0x405e,

    EOD
};


/******************************************************************************
 * CRTC values for S3-928 in 1280X1024 @ 60Hz
 *****************************************************************************/
USHORT crtc928_1280x1024x60Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x0034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x30, 0x27, 0x29, 0x96,
    0x29, 0x8d, 0x28, 0x5a,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x05, 0x09, 0xff, 0x00,             // reg 19 == 50 for packed
    0x00, 0xff, 0x29, 0xe3,
    0xff,

    OW,                                 // overflow regs
    0X3D4, 0x005d,

    OW,                                 // more overflow regs
    0X3D4, 0x515e,

    EOD
};


/******************************************************************************
 * CRTC values for S3-928 in 1280X1024 @ 70Hz
 *****************************************************************************/
USHORT crtc928_1280x1024x70Hz[] = {

    SELECTACCESSRANGE + SYSTEMCONTROL,

    OW,                                 // S3R4 - Backwards Compatibility 3
    0x3d4, 0x0034,

    OW,                                 // Unprotect CRTC regs
    0x3d4, 0x0011,

    METAOUT+INDXOUT,                    // Program the CRTC regs
    0x3d4,
    25, 0,
    0x2f, 0x27, 0x29, 0x95,
    0x29, 0x8d, 0x28, 0x5a,
    0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0xff, 0x00,
    0x05, 0x09, 0xff, 0x00,             // reg 19 == 50 for packed
    0x00, 0xff, 0x29, 0xe3,
    0xff,

    OW,                                 // overflow regs
    0X3D4, 0x005d,

    OW,                                 // more overflow regs
    0X3D4, 0x515e,

    EOD

};

/****************************************************************************
 *  Chip & Board Specific Mode Selection Tables
 ***************************************************************************/


ULONG aulGenericClk[] = {
        0,                          // 640  x 480 @ 60Hz
        0,                          // 640  x 480 @ 60Hz
        0,                          // 640  x 480 @ 60Hz
        0,                          // 640  x 480 @ 60Hz
        0xB,                        // 640  x 480 @ 72Hz
        2,                          // 800  x 600 @ 60Hz
        2,                          // 800  x 600 @ 60Hz
        2,                          // 800  x 600 @ 60Hz
        2,                          // 800  x 600 @ 60Hz
        4,                          // 800  x 600 @ 72Hz
        0xD,                        // 1024 x 768 @ 60Hz
        0xD,                        // 1024 x 768 @ 60Hz
        0xD,                        // 1024 x 768 @ 60Hz
        0xD,                        // 1024 x 768 @ 60Hz
        0xE                         // 1024 x 768 @ 72Hz
};

ULONG aulOrchidClk[] = {
        0,                          // 640  x 480 @ 60Hz
        0,                          // 640  x 480 @ 60Hz
        0,                          // 640  x 480 @ 60Hz
        0,                          // 640  x 480 @ 60Hz
        2,                          // 640  x 480 @ 72Hz
        4,                          // 800  x 600 @ 60Hz
        4,                          // 800  x 600 @ 60Hz
        4,                          // 800  x 600 @ 60Hz
        4,                          // 800  x 600 @ 60Hz
        6,                          // 800  x 600 @ 72Hz
        7,                          // 1024 x 768 @ 60Hz
        7,                          // 1024 x 768 @ 60Hz
        7,                          // 1024 x 768 @ 60Hz
        7,                          // 1024 x 768 @ 60Hz
        0xB                         // 1024 x 768 @ 72Hz
};

ULONG aulNumberNineClk[] = {
        25175000,                   // 640  x 480 @ 60Hz
        25175000,                   // 640  x 480 @ 60Hz
        25175000,                   // 640  x 480 @ 60Hz
        25175000,                   // 640  x 480 @ 60Hz
        31500000,                   // 640  x 480 @ 72Hz
        40000000,                   // 800  x 600 @ 60Hz
        40000000,                   // 800  x 600 @ 60Hz
        40000000,                   // 800  x 600 @ 60Hz
        40000000,                   // 800  x 600 @ 60Hz
        50000000,                   // 800  x 600 @ 72Hz
        65000000,                   // 1024 x 768 @ 60Hz
        65000000,                   // 1024 x 768 @ 60Hz
        65000000,                   // 1024 x 768 @ 60Hz
        65000000,                   // 1024 x 768 @ 60Hz
        77000000,                   // 1024 x 768 @ 72Hz
        55000000,                   // 1280 X 1024 @ 60Hz
        55000000,                   // 1280 X 1024 @ 60Hz
        55000000,                   // 1280 X 1024 @ 60Hz
        55000000,                   // 1280 X 1024 @ 60Hz
        64000000                    // 1280 X 1024 @ 72Hz
};

#endif

//
// Video mode table - Lists the information about each individual mode
//

//
// BUGBUG
// Need to create mode table values for 43Hz and 56Hz for all
// resolutions.
//

S3_VIDEO_MODES S3Modes[] = {
    {
      FALSE,            // Is this mode currently supported
      0x00100000,       // Required Video memory for this mode
      0x0201,           // Int 10 mode number
      NULL, NULL, NULL, // CRTC tables for NON-int10 modes.
        {
          sizeof(VIDEO_MODE_INFORMATION), // Size of the mode informtion structure
          0,                              // Mode index used in setting the mode
          640,                            // X Resolution, in pixels
          480,                            // Y Resolution, in pixels
          1024,                           // Screen stride, in bytes (distance
                                          // between the start point of two
                                          // consecutive scan lines, in bytes)
          1,                              // Number of video memory planes
          8,                              // Number of bits per plane
          1,                              // Screen Frequency, in Hertz
          330,                            // Horizontal size of screen in millimeters
          240,                            // Vertical size of screen in millimeters
          6,                              // Number Red pixels in DAC
          6,                              // Number Green pixels in DAC
          6,                              // Number Blue pixels in DAC
          0x00000000,                     // Mask for Red Pixels in non-palette modes
          0x00000000,                     // Mask for Green Pixels in non-palette modes
          0x00000000,                     // Mask for Blue Pixels in non-palette modes
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
              VIDEO_MODE_MANAGED_PALETTE, // Mode description flags.
          1024,                           // Video Memory Bitmap Width
          1024                            // Video Memory Bitmap Height
        }
    },

    {
      FALSE,
      0x00100000,
      0x0201,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          640,
          480,
          1024,                
          1,
          8,                        
          43,                        
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
              VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0201,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          640,
          480,
          1024,
          1,
          8,
          56,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
              VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0201,
#ifndef i386
      crtc911_640x480x60Hz,  crtc801_640x480x60Hz,  crtc928_640x480x60Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          640,
          480,
          1024,
          1,
          8,
          60,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
              VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0201,
#ifndef i386
      crtc911_640x480x70Hz,  crtc801_640x480x70Hz,  crtc928_640x480x70Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          640,
          480,
          1024,
          1,
          8,
          72,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
              VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0203,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          800,
          600,
          1024,
          1,
          8,
          1,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0203,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          800,
          600,
          1024,
          1,
          8,
          43,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0203,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          800,
          600,
          1024,
          1,
          8,
          56,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0203,
#ifndef i386
      crtc911_800x600x60Hz,  crtc801_800x600x60Hz,  crtc928_800x600x60Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          800,
          600,
          1024,
          1,
          8,
          60,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0203,
#ifndef i386
      crtc911_800x600x70Hz,  crtc801_800x600x70Hz,  crtc928_800x600x70Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          800,
          600,
          1024,
          1,
          8,
          72,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0205,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1024,
          768,
          1024,
          1,
          8,
          1,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0205,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1024,
          768,
          1024,
          1,
          8,
          43,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0205,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1024,
          768,
          1024,
          1,
          8,
          56,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0205,
#ifndef i386
      crtc911_1024x768x60Hz, crtc801_1024x768x60Hz, crtc928_1024x768x60Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1024,
          768,
          1024,
          1,
          8,
          60,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00100000,
      0x0205,
#ifndef i386
      crtc911_1024x768x70Hz, crtc801_1024x768x70Hz, crtc928_1024x768x70Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1024,
          768,
          1024,
          1,
          8,
          72,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          1024,
          1024
        }
    },

    {
      FALSE,
      0x00300000,
      0x0107,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1280,
          1024,
          2048,
          1,
          8,
          1,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          2048,
          1536
        }
    },

    {
      FALSE,
      0x00300000,
      0x0107,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1280,
          1024,
          2048,
          1,
          8,
          43,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          2048,
          1536
        }
    },

    {
      FALSE,
      0x00300000,
      0x0107,
      NULL, NULL, NULL,
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1280,
          1024,
          2048,
          1,
          8,
          56,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          2048,
          1536
        }
    },

    {
      FALSE,
      0x00300000,
      0x0107,
#ifndef i386
      NULL, NULL, crtc928_1280x1024x60Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1280,
          1024,
          2048,
          1,
          8,
          60,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          2048,
          1536
        }
    },

    {
      FALSE,
      0x00300000,
      0x0107,
#ifndef i386
      NULL, NULL, crtc928_1280x1024x70Hz,
#else
      NULL, NULL, NULL,
#endif
        {
          sizeof(VIDEO_MODE_INFORMATION),
          0,
          1280,
          1024,
          2048,
          1,
          8,
          72,
          330,
          240,
          6,
          6,
          6,
          0x00000000,
          0x00000000,
          0x00000000,
          VIDEO_MODE_COLOR | VIDEO_MODE_GRAPHICS | VIDEO_MODE_PALETTE_DRIVEN |
          VIDEO_MODE_MANAGED_PALETTE,
          2048,
          1536
        }
    }
};


ULONG NumS3VideoModes = sizeof(S3Modes) / sizeof(S3_VIDEO_MODES);
