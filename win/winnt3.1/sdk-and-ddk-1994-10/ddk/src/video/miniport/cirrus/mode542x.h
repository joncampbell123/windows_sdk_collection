/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    Mode542x.h

Abstract:

    This module contains all the global data used by the Cirrus Logic
   CL-542x driver.

Environment:

    Kernel mode

Revision History:

--*/

//
// The next set of tables are for the CL542x
// Note: all resolutions supported
//

//
// 640x480 16-color mode (BIOS mode 12) set command string for CL 542x.
//

USHORT CL542x_640x480[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    15,                             // count
    0x100,                          // start sync reset
    0x0101,0x0f02,0x0003,0x0604,    // program up sequencer
    0x0007,0x0008,0x4A0B,0x5B0C,
    0x450D,0x000E,0x2B1B,
    0x2F1C,0x301D,0x001E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x00,                      // and mask, xor mask

    OB,                             // misc. register
    MISC_OUTPUT_REG_WRITE_PORT,
    0xe3,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0x506,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x2011,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80,
    0x0b, 0x3e, 0x00, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xea, 0x8c,
    0xdf, 0x28, 0x00, 0xe7, 0x04, 0xe3,
    0xff, 0x00, 0x00, 0x00,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                            // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x0F, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

#endif
    OWM,
    GRAPH_ADDRESS_PORT,
    3,
    0x0009, 0x000a, 0x000b,      // no banking in 640x480 mode

    EOD                   
};

//
// 800x600 16-color (60Hz refresh) mode set command string for CL 542x.
//

USHORT CL542x_800x600[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    15,                             // count
    0x100,                          // start sync reset
    0x0101,0x0f02,0x0003,0x0604,    // program up sequencer
    0x0007,0x0008,0x4A0B,0x5B0C,
    0x450D,0x7E0E,0x2B1B,
    0x2F1C,0x301D,0x331E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x00,                      // and mask, xor mask

    OB,                             // misc. register
    MISC_OUTPUT_REG_WRITE_PORT,
    0x2F,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0x506,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x2011,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0x7b, 0x63, 0x64, 0x9e, 0x69, 0x92, // NANAO 9070
    0x6F, 0xf0, 0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x58, 0x8a,
    0x57, 0x32, 0x00, 0x58, 0x6f, 0xE3,
    0xff, 0x00, 0x00, 0x22,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                            // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x0F, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

#endif
    OWM,
    GRAPH_ADDRESS_PORT,
    3,
    0x0009, 0x000a, 0x000b,      // no banking in 800x600 mode

    EOD
};

//
// 1024x768 16-color (60Hz refresh) mode set command string for CL 542x.
//

USHORT CL542x_1024x768[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    15,                             // count
    0x100,                          // start sync reset
    0x0101,0x0f02,0x0003,0x0604,    // program up sequencer
    0x0007,0x0008,0x4A0B,0x5B0C,
    0x450D,0x3B0E,0x2B1B,
    0x2F1C,0x301D,0x1A1E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0f,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0x9f,0x40,                      // and mask, xor mask

    OB,                             // misc. register
    MISC_OUTPUT_REG_WRITE_PORT,
    0xEF,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0x506,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x2011,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0xa3, 0x7f, 0x80, 0x86, 0x85, 0x96, // SONY GDM-1952
    0x24, 0xfd, 0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x88,
    0xff, 0x40, 0x00, 0x00, 0x24, 0xE3,
    0xFF, 0x4A, 0x00, 0x22,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                           // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x0F, 0xFF, 


    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

#endif

    OWM,
    GRAPH_ADDRESS_PORT,
    3,
#if ONE_64K_BANK
    0x0009, 0x000a, 0x000b,
#endif
#if TWO_32K_BANKS
    0x0009, 0x000a, 0x010b,
#endif

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

    EOD
};

//-----------------------------
// standard VGA text modes here
// 80x25 at 640x350
//
//-----------------------------

//
// 80x25 text mode set command string for CL 542x.
// (720x400 pixel resolution; 9x16 character cell.)
//

USHORT CL542x_80x25Text[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    12,                             // count
    0x100,                          // start sync reset
    0x0001,0x0302,0x0003,0x0204,    // program up sequencer
    0x0007,0x000E,0x0010,
    0x0011,0x0013,0x0016,
    0x001E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x00,                      // and mask, xor mask

    OB,                             // misc. regiseter
    MISC_OUTPUT_REG_WRITE_PORT,
    0x67,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0xe06,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x0e11,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81,
    0xBF, 0x1F, 0x00, 0x4F, 0x0D, 0x0E,
    0x00, 0x00, 0x00, 0x00, 0x9C, 0x8E,
    0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
    0xFF, 0x00, 0x00, 0x00,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                            // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x0e, 0x00, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x14, 0x07, 0x38, 0x39, 0x3A, 0x3B,
    0x3C, 0x3D, 0x3E, 0x3F, 0x04, 0x00,
    0x0F, 0x08, 0x00,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

    IB,                             // prepare to write to Hidden DAC reg
    DAC_PIXEL_MASK_PORT,
    IB,                             // by reading this register four times
    DAC_PIXEL_MASK_PORT,
    IB,
    DAC_PIXEL_MASK_PORT,
    IB,
    DAC_PIXEL_MASK_PORT,
    OB,
    DAC_PIXEL_MASK_PORT,            // clear the hidden DAC register
    0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OW,
    SEQ_ADDRESS_PORT, 0x0006,       // turn off the extension registers

#endif
    OWM,
    GRAPH_ADDRESS_PORT,
    3,
    0x0009, 0x000a, 0x000b,      // no banking in text mode

    EOD
};

//
// 80x25 text mode set command string for CL 542x.
// (640x350 pixel resolution; 8x14 character cell.)
//

USHORT CL542x_80x25_14_Text[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    12,                             // count
    0x100,                          // start sync reset
    0x0101,0x0302,0x0003,0x0204,    // program up sequencer
    0x0007,0x000E,0x0010,
    0x0011,0x0013,0x0016,
    0x001E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x00,                      // and mask, xor mask

    OB,                             // misc. regiseter
    MISC_OUTPUT_REG_WRITE_PORT,
    0xa3,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0xe06,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x0511,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
    0x00, 0x4D, 0x0B, 0x0C, 0x00, 0x00, 0x00, 0x00,
    0x83, 0x85, 0x5D, 0x28, 0x1F, 0x63, 0xBA, 0xA3,
    0xFF,0x00, 0x00, 0x00,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                           // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x0e, 0x00, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x14, 0x07, 0x38, 0x39, 0x3A, 0x3B,
    0x3C, 0x3D, 0x3E, 0x3F, 0x08, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

    IB,                             // prepare to write to Hidden DAC reg
    DAC_PIXEL_MASK_PORT,
    IB,                             // by reading this register four times
    DAC_PIXEL_MASK_PORT,
    IB,
    DAC_PIXEL_MASK_PORT,
    IB,
    DAC_PIXEL_MASK_PORT,
    OB,
    DAC_PIXEL_MASK_PORT,            // clear the hidden DAC register
    0x00,

    OW,
    SEQ_ADDRESS_PORT,
    0x0006,                         // turn off the extension registers

#endif
    OWM,
    GRAPH_ADDRESS_PORT,
    3,
    0x0009, 0x000a, 0x000b,         // no banking in text mode

    EOD
};

//
// 1280x1024 16-color mode (BIOS mode 0x6C) set command string for CL 542x.
//

USHORT CL542x_1280x1024_I[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    15,                             // count
    0x100,                          // start sync reset
    0x0101,0x0f02,0x0003,0x0604,    // program up sequencer
    0x0007,0x0008,0x4A0B,0x5B0C,
    0x450D,0x6E0E,0x2B1B,
    0x2F1C,0x301D,0x2A1E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x00,                      // and mask, xor mask

    OB,                             // misc. register
    MISC_OUTPUT_REG_WRITE_PORT,
    0xEF,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0x506,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x2011,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0xBD, 0x9f, 0xA0, 0x80, 0xA4, 0x19,
    0x2A, 0xB2, 0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0B, 0x80,
    0xFF, 0x50, 0x00, 0x00, 0x2A, 0xE3,
    0xFF, 0x60, 0x01, 0x22,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                            // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x0F, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

#endif
    OWM,
    GRAPH_ADDRESS_PORT,
    3,
#if ONE_64K_BANK
    0x0009, 0x000a, 0x000b,
#endif
#if TWO_32K_BANKS
    0x0009, 0x000a, 0x010b,
#endif

    EOD
};

//
// 640x480 256-color mode (BIOS mode 0x5F) set command string for CL 542x.
//

USHORT CL542x_640x480_256[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    15,                             // count
    0x100,                          // start sync reset
    0x0101,0x0F02,0x0003,0x0E04,    // program up sequencer
    0x0107,0x0008,0x4A0B,0x5B0C,
    0x450D,0x7E0E,0x2B1B,
    0x2F1C,0x301D,0x331E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x20,                      // and mask, xor mask

    OB,                             // misc. register
    MISC_OUTPUT_REG_WRITE_PORT,
    0xe3,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0x506,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x2011,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80,
    0x0b, 0x3e, 0x00, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xea, 0x8c,
    0xdf, 0x50, 0x00, 0xe7, 0x04, 0xe3,
    0xff, 0x00, 0x00, 0x22,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                            // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x05, 0x0F, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

#endif
    OWM,
    GRAPH_ADDRESS_PORT,
    3,
#if ONE_64K_BANK
    0x0009, 0x000a, 0x000b,
#endif
#if TWO_32K_BANKS
    0x0009, 0x000a, 0x010b,
#endif

    EOD                   
};

//
// 800x600 256-color mode (BIOS mode 0x5C) set command string for CL 542x.
//

USHORT CL542x_800x600_256[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    15,                             // count
    0x100,                          // start sync reset
    0x0101,0x0F02,0x0003,0x0E04,    // program up sequencer
    0x0107,0x0008,0x4A0B,0x5B0C,
    0x450D,0x640E,0x2B1B,
    0x2F1C,0x301D,0x3A1E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x20,                      // and mask, xor mask

    OB,                             // misc. register
    MISC_OUTPUT_REG_WRITE_PORT,
    0x2F,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0x506,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x2011,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0x7D, 0x63, 0x64, 0x80, 0x6D, 0x1C,
    0x98, 0xF0, 0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7B, 0x80,
    0x57, 0x64, 0x00, 0x5F, 0x91, 0xe3,
    0xff, 0x00, 0x00, 0x22,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                            // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x05, 0x0F, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,
#endif

    OWM,
    GRAPH_ADDRESS_PORT,
    3,
#if ONE_64K_BANK
    0x0009, 0x000a, 0x000b,
#endif
#if TWO_32K_BANKS
    0x0009, 0x000a, 0x010b,
#endif

    EOD                   
};

//
// 1024x768 256-color mode (BIOS mode 0x60) set command string for CL 542x.
//

USHORT CL542x_1024x768_256[] = {
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    2,                             // count
    0x1206,                         // enable extensions
    0x0012,

#ifndef INT10_MODE_SET
    OWM,                            // begin setmode
    SEQ_ADDRESS_PORT,
    15,                             // count
    0x100,                          // start sync reset
    0x0101,0x0F02,0x0003,0x0E04,    // program up sequencer
    0x0107,0x0008,0x4A0B,0x5B0C,
    0x450D,0x3B0E,0x2B1B,
    0x2F1C,0x301D,0x1A1E,

    OB,                             // point sequencer index to ff
    SEQ_ADDRESS_PORT,
    0x0F,

    METAOUT+MASKOUT,                // masked out.
    SEQ_DATA_PORT,
    0xDF,0x20,                      // and mask, xor mask

    OB,                             // misc. register
    MISC_OUTPUT_REG_WRITE_PORT,
    0xEF,

    OW,                             // text/graphics bit
    GRAPH_ADDRESS_PORT,
    0x506,

    OW,                             // end sync reset
    SEQ_ADDRESS_PORT,
    0x300,

    OW,                             // unprotect crtc 0-7
    CRTC_ADDRESS_PORT_COLOR,
    0x2011,

    METAOUT+INDXOUT,                // program crtc registers
    CRTC_ADDRESS_PORT_COLOR,
    28,0,                           // count, startindex
    0xA3, 0x7F, 0x80, 0x86, 0x85, 0x96,
    0x24, 0xFD, 0x00, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x88,
    0xFF, 0x80, 0x00, 0x00, 0x24, 0xe3,
    0xff, 0x4A, 0x00, 0x22,

    METAOUT+INDXOUT,                // program gdc registers
    GRAPH_ADDRESS_PORT,
    9,0,                            // count, startindex
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x05, 0x0F, 0xFF, 

    IB,                             // prepare atc for writing
    INPUT_STATUS_1_COLOR,

    METAOUT+ATCOUT,                 // program atc registers
    ATT_ADDRESS_PORT,
    21,0,                           // count, startindex
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00,
    0x0F, 0x00, 0x00,

    OB,                             // turn video on.
    ATT_ADDRESS_PORT,
    0x20,

    OB,
    DAC_PIXEL_MASK_PORT,
    0xFF,

#endif
    OWM,
    GRAPH_ADDRESS_PORT,
    3,
#if ONE_64K_BANK
    0x0009, 0x000a, 0x000b,
#endif
#if TWO_32K_BANKS
    0x0009, 0x000a, 0x010b,
#endif

    EOD                   
};


