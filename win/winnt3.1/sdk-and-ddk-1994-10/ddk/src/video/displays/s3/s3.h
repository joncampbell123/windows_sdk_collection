/******************************Module*Header*******************************\
* Module Name: S3.h
*
* All the S3 specific driver h file stuff
*
* Copyright (c) 1992 Microsoft Corporation
*
\**************************************************************************/

#define CLIPOBJ_vGetrclBounds(pco, prcl)	(*(prcl) = (pco)->rclBounds)
#define CLIPOBJ_vSetrclBounds(pco, prcl)	((pco)->rclBounds = *(prcl))

#define CLIPOBJ_vGetDComplexity(pco)		(*(pco)->iDComplexity)
#define CLIPOBJ_vSetDComplexity(pco, j) 	(*(pco)->iDComplexity = (j))

#define CLIPOBJ_fjGetClipState(pco)		((pco)->ajPad)
#define CLIPOBJ_vSetClipState(pco, fj)		((pco)->ajPad == (fj))

#define SURFOBJ_vGetpvScan0(pso, ppv)		(*(ppv) = (pso)->pvScan0)
#define SURFOBJ_vSetpvScan0(pso, pv)		((pso)->pvScan0 = (pv))


// debug stuff

#if 0

#define LOG_OUTS    1

#else

#define CATCHIT_ON 1
#define CATCHIT (CATCHIT_ON && DBG)

#endif

// Porting stuff.

#if !defined(i386)

// The code makes extensive use of the inp, inpw, outp and outpw x86
// intrinsic functions. Since these don't exist on the Alpha platform,
// map them into something we can handle.  Since the CSRs are mapped
// on Alpha, we have to add the register base to the register number
// passed in the source.

#include "ioaccess.h"

extern PUCHAR gpucCsrBase;

#define inp(p)		READ_PORT_UCHAR (gpucCsrBase + (p))
#define inpw(p)		READ_PORT_USHORT ((PUSHORT)(gpucCsrBase + (p)))
#define outp(p,v)	WRITE_PORT_UCHAR (gpucCsrBase + (p), (v))
#define outpw(p,v)	WRITE_PORT_USHORT ((PUSHORT)(gpucCsrBase + (p)), (v))


#endif // !defined(i386)


#if CATCHIT

VOID vCheckDataReady(PPDEV);
VOID vCheckDataComplete(PPDEV);

#define CHECK_DATA_READY    vCheckDataReady(ppdev)
#define CHECK_DATA_COMPLETE vCheckDataComplete(ppdev)

VOID    outpw_test(unsigned, unsigned);
VOID    outp_test(unsigned, int);
BYTE    inp_test(WORD);
WORD    inpw_test(WORD);

#define OUTPW(port, val)    outpw_test(port, val)
#define OUTP(port, val)     outp_test(port, val)
#define INPW(port)	    inpw_test(port)
#define INP(port)	    inp_test(port)

#define OUTPW1(port, val)   outpw(port, val)
#define OUTP1(port, val)    outp(port, val)
#define INPW1(port)	    inpw(port)
#define INP1(port)	    inp(port)

#else

#define CHECK_DATA_READY
#define CHECK_DATA_COMPLETE

#if LOG_OUTS

VOID    outpw_log(unsigned, unsigned);
VOID    outp_log(unsigned, int);

#define OUTPW(port, val)    outpw_log(port, val)
#define OUTP(port, val)	    outp_log(port, val)
#define INPW(port)	    inpw(port)
#define INP(port)	    inp(port)

#define OUTPW1(port, val)   outpw_log(port, val)
#define OUTP1(port, val)    outp_log(port, val)
#define INPW1(port)	    inpw(port)
#define INP1(port)	    inp(port)

#else

#define OUTPW(port, val)    outpw(port, val)
#define OUTP(port, val)	    outp(port, val)
#define INPW(port)	    inpw(port)
#define INP(port)	    inp(port)

#define OUTPW1(port, val)   outpw(port, val)
#define OUTP1(port, val)    outp(port, val)
#define INPW1(port)	    inpw(port)
#define INP1(port)	    inp(port)

#endif
#endif

#if LOG_OUTS

#define TEST_928(s)                 \
    if (INP1(0x3d4) == 0xff)        \
    {                               \
        RIP(s);  \
    }

#else

#define TEST_928(s)

#endif

// S3 Chip equates.

#define STATUS_1            0x3DA
#define VSY_NOT             0x08

#define CRTC_INDEX          0x3D4
#define CRTC_DATA           0x3D5

#define S3R8                0x38
#define S3R9                0x39
#define S3R1                0x31
#define S3R5                0x35

#define CR39                0x39
#define CR4C                0x4C
#define CR4D                0x4D

#define HGC_MODE            0x45
#define HGC_ENABLE          0x01
#define HGC_DISABLE         0x00


#define HGC_ORGX_LSB        0x47
#define HGC_ORGX_MSB        0x46
#define HGC_ORGY_LSB        0x49
#define HGC_ORGY_MSB        0x48

#define HGC_DX              0x4E
#define HGC_DY              0x4F


#define REG_UNLOCK_1        0x48
#define CPUA_BASE           0X01

#define SYSCTL_UNLOCK       0xA0
#define SYSCTL_LOCK         0x00

#define SYS_CNFG            0x40
#define LAW_CTL             0x58
#define EX_SCTL_2           0x51
#define EX_DAC_CT           0x55

#define MISC_1              0x3A

// Brooktree 485 defines.


    // Registers.

#define BT485_ADDR_CMD_REG0        0x3c6
#define BT485_ADDR_CMD_REG1        0x3c8
#define BT485_ADDR_CMD_REG2        0x3c9
#define BT485_ADDR_CMD_REG3        0x3c6


#define BT485_CMD_REG_3_ACCESS     0x80

#define BT485_ADDR_CUR_COLOR_WRITE 0x3c8
#define BT485_CUR_COLOR_DATA       0x3c9
#define BT485_ADDR_CUR_RAM_WRITE   0x3c8
#define BT485_CUR_RAM_ARRAY_DATA   0X3c7



#define BT485_CURSOR_COLOR_1       0x1
#define BT485_CURSOR_COLOR_2       0x2

#define BT485_CURSOR_X_LOW         0x3c8
#define BT485_CURSOR_X_HIGH        0x3c9

#define BT485_CURSOR_Y_LOW         0x3c6
#define BT485_CURSOR_Y_HIGH        0x3c7

    // Commands.

#define BT485_CURSOR_DISABLE    (~0x03)
#define BT485_CURSOR_MODE2      0x02

#define BT485_64X64_CURSOR      0x04


// Current position & source position registers.

#define CUR_X               ppdev->cur_x
#define CUR_Y               ppdev->cur_y

// Destination register.

#define DEST_X              ppdev->dest_x
#define DEST_Y              ppdev->dest_y

#define AXSTP               ppdev->axstp
#define DIASTP              ppdev->diastp

#define RECT_WIDTH          ppdev->rect_width
#define LINE_MAX            ppdev->line_max

#define ERR_TERM            ppdev->err_term

// Graphic Processor status and command registers.

#define GP_STAT             ppdev->gp_stat
#define CMD                 ppdev->cmd
#define SHORT_STROKE_REG    ppdev->short_stroke_reg

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

#define MULTIFUNC_CNTL      ppdev->multifunc_cntl

#define RECT_HEIGHT         0x0000

#define CLIP_TOP            0x1000
#define CLIP_LEFT           0x2000
#define CLIP_BOTTOM         0x3000
#define CLIP_RIGHT          0x4000

#define DATA_EXTENSION      0xA000
#define ALL_ONES            0x0000
#define CPU_DATA            0x0080
#define DISPLAY_MEMORY      0x00C0

// Foreground & Background Color Registers.

#define BKGD_COLOR            ppdev->bkgd_color
#define FRGD_COLOR            ppdev->frgd_color

// Foreground & Background Mix register defines.

    // Registers

#define BKGD_MIX            ppdev->bkgd_mix
#define FRGD_MIX            ppdev->frgd_mix

    // Color source

#define BACKGROUND_COLOR        0x00
#define FOREGROUND_COLOR        0x20
#define SRC_CPU_DATA            0x40
#define SRC_DISPLAY_MEMORY      0x60

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

// Read and write masks

#define WRT_MASK                ppdev->wrt_mask
#define RD_MASK                 ppdev->rd_mask

// Data port

#define PIXEL_TRANSFER          ppdev->pixel_transfer

// General purpose support routines & macros.

#define FIFOWAIT(level) while (INPW(GP_STAT) & level);

#define GPWAIT() while (INPW(GP_STAT) & HARDWARE_BUSY);

// 801/805/928 specific macros.

#define ENABLE_DIRECT_ACCESS                            \
    if (ppdev->bNewBankControl)                         \
    {                                                   \
        TEST_928 ("S3.DLL!pre enable direct access 928 failure\n"); \
        while (inpw(GP_STAT) & 0x200);                  \
        OUTPW1(CRTC_INDEX, (ppdev->SysCnfg | 0x0800));   \
        OUTPW1(CRTC_INDEX, (ppdev->LawCtl  | 0x1000));   \
        TEST_928 ("S3.DLL!post enable direct access 928 failure\n");\
    }


#define ENABLE_S3_ENGINE                                \
    if (ppdev->bNewBankControl)                         \
    {                                                   \
        TEST_928 ("S3.DLL!pre enable S3 engine 928 failure\n");     \
        OUTPW1(CRTC_INDEX, ppdev->LawCtl);               \
        OUTPW1(CRTC_INDEX, (ppdev->SysCnfg | 0x0100));   \
        TEST_928 ("S3.DLL!post enable S3 engine 928 failure\n");    \
    }

VOID vSetS3ClipRect(PPDEV ppdev, PRECTL prclClip);
VOID vResetS3Clipping(PPDEV ppdev);

VOID vDataPortOut (PPDEV ppdev, PWORD pw, UINT count);
VOID vDataPortOutB(PPDEV ppdev, PBYTE pb, UINT count);
VOID vDataPortIn  (PPDEV ppdev, PWORD pw, UINT count);
VOID vDataPortInB (PPDEV ppdev, PBYTE pb, UINT count);

// Shadow macros

#define TEST_AND_SET_FRGD_MIX(val)              \
            if (val != ppdev->ForegroundMix)    \
            {                                   \
                ppdev->ForegroundMix = val;     \
                OUTPW(FRGD_MIX, val);           \
            }

#define TEST_AND_SET_BKGD_MIX(val)              \
            if (val != ppdev->BackgroundMix)    \
            {                                   \
                ppdev->BackgroundMix = val;     \
                OUTPW(BKGD_MIX, val);           \
            }

#define TEST_AND_SET_FRGD_COLOR(val)            \
            if (val != ppdev->ForegroundColor)  \
            {                                   \
                ppdev->ForegroundColor = val;   \
                OUTPW(FRGD_COLOR, val);         \
            }

#define TEST_AND_SET_BKGD_COLOR(val)            \
            if (val != ppdev->BackgroundColor)  \
            {                                   \
                ppdev->BackgroundColor = val;   \
                OUTPW(BKGD_COLOR, val);         \
            }

#define TEST_AND_SET_WRT_MASK(val)              \
            if (val != ppdev->WriteMask)        \
            {                                   \
                ppdev->WriteMask = val;         \
                OUTPW(WRT_MASK, val);           \
            }

#define TEST_AND_SET_RD_MASK(val)               \
            if (val != ppdev->ReadMask)         \
            {                                   \
                ppdev->ReadMask = val;          \
                OUTPW(RD_MASK, val);            \
            }

#define SET_FRGD_MIX(val)                       \
            {                                   \
                ppdev->ForegroundMix = val;     \
                OUTPW(FRGD_MIX, val);           \
            }

#define SET_BKGD_MIX(val)                       \
            {                                   \
                ppdev->BackgroundMix = val;     \
                OUTPW(BKGD_MIX, val);           \
            }

#define SET_FRGD_COLOR(val)                     \
            {                                   \
                ppdev->ForegroundColor = val;   \
                OUTPW(FRGD_COLOR, val);         \
            }

#define SET_BKGD_COLOR(val)                     \
            {                                   \
                ppdev->BackgroundColor = val;   \
                OUTPW(BKGD_COLOR, val);         \
            }

#define SET_WRT_MASK(val)                       \
            {                                   \
                ppdev->WriteMask = val;         \
                OUTPW(WRT_MASK, val);           \
            }

#define SET_RD_MASK(val)                        \
            {                                   \
                ppdev->ReadMask = val;          \
                OUTPW(RD_MASK, val);            \
            }



#define SCRN_TO_SCRN_CPY    0x01
#define SOLID_PATTERN       0x02
#define BRUSH_PATTERN       0x04
#define BLACK_AND_WHITE     0x08
#define HOST_TO_SCRN_CPY    0x10

// Clipping Control Stuff

typedef struct {
    ULONG   c;
    RECTL   arcl[8];
} ENUMRECTS8;

typedef ENUMRECTS8 *PENUMRECTS8;

typedef struct {
    ULONG       c;
    TRAPEZOID   atrap[8];
} ENUMTRAPS8;

typedef ENUMTRAPS8 *PENUMTRAPS8;

#define MAX_DDA_RECTS   40  // most rects we want enumerated by DDAOBJ at once

typedef struct _DDAENUM
{
    LONG    yTop;
    LONG    yBottom;
    LONG    axPairs[MAX_DDA_RECTS];
} DDAENUM;

BOOL bIntersectTest(PRECTL prcl1, PRECTL prcl2);
BOOL bTrivialAcceptTest(PRECTL prcl1, PRECTL prcl2);

// ROP translation stuff.

extern BYTE Rop2ToS3Rop[];

// Full screen support stuff

VOID vSaveOffScreenMemory(PPDEV);
VOID vRestoreOffScreenMemory(PPDEV);

// SaveScreenBits and source bitmap cache stuff.

BOOL bMoveSaveScreenBitsToHost(PPDEV ppdev, PSAVEDSCRNBITS *ppssbNewNode);

// Hooks and Driver function table.

#define HOOKS_BMF8BPP   (HOOK_BITBLT     | HOOK_TEXTOUT     | HOOK_FILLPATH | \
                         HOOK_COPYBITS   | HOOK_STROKEPATH  | HOOK_PAINT)

#define HOOKS_BMF16BPP 0
