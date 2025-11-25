/******************************Module*Header*******************************\
* Module Name: driver.h
*
* contains prototypes for the frame buffer driver.
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "stddef.h"
#include "windows.h"
#include "winddi.h"
#include "devioctl.h"
#include "ntddvdeo.h"
#include "debug.h"

#include "s3mem.h"

// Cursor state flags

#define CURSOR_EXCLUDED 0       // Cursor is not visible
#define CURSOR_VISIBLE  1       // Cursor is visible

#define CURSOR_DISABLED 0       // No cursor set.
#define CURSOR_HARDWARE 1       // Cursor is handled in hardware
#define CURSOR_SOFTWARE 2       // Cursor is handled in software

typedef struct _bank {
    PVOID   pvScan0;
    UINT    Bank;
    RECTL   rclClip;
} BANK;

typedef BANK *PBANK;

typedef struct {
    BYTE    red;
    BYTE    green;
    BYTE    blue;
} DACDATA;

typedef DACDATA *PDACDATA;

typedef struct _ClipNode {
    struct _ClipNode *pcnNext;
    RECTL    rclClip;
} CLIPNODE;

typedef CLIPNODE *PCLIPNODE;

typedef struct {
    LONG    x;
    LONG    y;
    LONG    z;
} XYZPOINTL;

typedef XYZPOINTL *PXYZPOINTL;
typedef XYZPOINTL XYZPOINT;
typedef XYZPOINT  *PXYZPOINT;

// Brush Stuff.

typedef struct {
    ULONG   nSize;
    ULONG   iPatternID;
    ULONG   iBrushCacheID;
    ULONG   iExpansionCacheID;
    ULONG   fl;
    ULONG   iType;
    ULONG   iBitmapFormat;
    ULONG   ulForeColor;
    ULONG   ulBackColor;
    SIZEL   sizlPattern;
    LONG    lDeltaPattern;
    BYTE    ajPattern[1];
} S3BRUSH;

typedef S3BRUSH *PS3BRUSH;

#define MAX_MONO_BRUSH_EXPANSION_SLOTS  8

// Save Screen Bits Stuff

typedef struct _savedscrnbitshdr {
    struct  _savedscrnbits *pssbLink;       // Link to next set of bits
    ULONG   iUniq;                          // Unique ID for these bits
    INT     x,                              // Screen x & y coordinates
            y,
            cx,                             // cx & cy of bit block.
            cy;
} SAVEDSCRNBITSHDR;

typedef SAVEDSCRNBITSHDR *PSAVEDSCRNBITSHDR;

typedef struct _savedscrnbits {
    SAVEDSCRNBITSHDR ssbh;
    DWORD            aBits[1];
} SAVEDSCRNBITS;

typedef SAVEDSCRNBITS *PSAVEDSCRNBITS;

// Font & Text stuff

typedef struct _cachedGlyph {
    HGLYPH      hg;
    struct      _cachedGlyph  *pcgCollisionLink;
    ULONG       fl;
    POINTL      ptlOrigin;
    SIZEL       sizlBitmap;
    ULONG       BmPitchInPels;
    ULONG       BmPitchInBytes;
    XYZPOINTL   xyzGlyph;
} CACHEDGLYPH, *PCACHEDGLYPH;

#define VALID_GLYPH     0x01

#define END_COLLISIONS  0

typedef struct _cachedFont {
    struct _cachedFont *pcfNext;
    ULONG           iUniq;
    ULONG           cGlyphs;
    ULONG           cjMaxGlyph1;
    PCACHEDGLYPH    pCachedGlyphs;
} CACHEDFONT, *PCACHEDFONT;

// The Physical Device data structure.

typedef struct  _PDEV
{

    // Enhanced mode register addresses.

    WORD        cur_x,
                cur_y,
                dest_x,
                dest_y,
                axstp,
                diastp,
                rect_width,
                line_max,
                err_term,
                gp_stat,
                cmd,
                short_stroke_reg,
                multifunc_cntl,
                bkgd_color,
                frgd_color,
                bkgd_mix,
                frgd_mix,
                wrt_mask,
                rd_mask,
                pixel_transfer;

    // -------------------------------------------------------------------
    // NOTE: Changes up to here in the PDEV structure must be reflected in
    // i386\strucs.inc (assuming you're on an x86, of course)!

    HANDLE  hDriver;                    // Handle to \Device\Screen
    HDEV    hdevEng;                    // Engine's handle to PDEV
    HSURF   hsurfEng;                   // Engine's handle to surface
    HANDLE  hsurfBm;                    // Handle to the "punt" surface
    SURFOBJ *pSurfObj;                  // pointer to the locked "punt" surface
    SURFOBJ *psoTemp;                   // pointer to the locked temp "punt" surface

    HPALETTE hpalDefault;               // Handle to the default palette for device.

    PBYTE   pjScreen;                   // This is pointer to base screen address

    ULONG   cxScreen;                   // Visible screen width
    ULONG   cyScreen;                   // Visible screen height
    ULONG   cxMaxRam;                   // Width of Video RAM
    ULONG   cyMaxRam;                   // Height of Video RAM
    ULONG   ulMode;                     // Mode the mini-port driver is in.
    LONG    lDeltaScreen;               // Distance from one scan to the next.

    PUCHAR  pucCsrBase;			// pointer to port 0, mapped into our address space
                                        // USED FOR PORTING purposes

    FLONG   flRed;                      // For bitfields device, Red Mask
    FLONG   flGreen;                    // For bitfields device, Green Mask
    FLONG   flBlue;                     // For bitfields device, Blue Mask
    ULONG   ulBitCount;                 // # of bits per pel 8,16,32 are only supported.

    POINTL  ptlHotSpot;                 // Pointer hot spot
    SIZEL   szlPointer;                 // Extent of the pointer
    POINTL  ptlLastPosition;            // Last position of pointer
    ULONG   flPointer;                  // Pointer specific flags.
    BYTE    CrtcIndex;                  // Crtc Index Value.
    WORD    MonoPointerData;            // Index to Mono Pointer Data

    ULONG   cPatterns;                  // Count of bitmap patterns created
    HBITMAP ahbmPat[HS_DDI_MAX];        // Engine handles to standard patterns

    GDIINFO *pGdiInfo;                  // Pointer to temporary buffer for GDIINFO struct
    DEVINFO *pDevInfo;                  // Pointer to temporary buffer for DEVINFO struct
    PALETTEENTRY *pPal;                 // If this is pal managed, this is the pal

    // Off Screen Save Stuff.

    PWORD    pOffScreenSaveBuffer;      // Pointer to the Off Screen Save Buffer
    PDACDATA pDacDataSaveBuffer;        // Pointer to the Dac Data Save Buffer

    // Bank Manager Stuff

    INT     MaxBanks,                   // Max number of banks for driver instance
            BankSize,                   // size of each bank in bytes
            ScansPerBank;               // Number of scans per bank

    PRECT   prclBanks;                  // defines S3-911's banks.
    PBANK   pBanks;                     // Bank control structures
    UINT    cBanks,                     // Number of banks to enumerate
            iBank;                      // Bank currently enumerated

    PVOID   pvOrgScan0;                 // Original pvScan0

    RECTL   rclOrgBounds;               // Clip Object's original bounds
    BYTE    iOrgDComplexity;            // Clip Object's original complexity
    BYTE    fjOptions;                  // Clip Object's original flags.

    BYTE    jS3R5;                      // S3's S3R5 (need to preserve high bits)

    // Clip Acceleration Stuff

    PCLIPNODE   pcnClipNodeRoot;        // Clip Node List Root
    PCLIPNODE   pcnFirstClip;           // First clip node in Y
    PCLIPNODE   pcnLastClip;            // Last clip node in Y

    INT         nClipNodes;             // Number of Clip nodes.

    // Font Stuff

    BYTE   ajGlyphAllocBitVector[CACHED_GLYPHS_ROWS][GLYPHS_PER_ROW];

    // Strip drawing (long lines) stuff

    BYTE        iFormat;                // BMF_*, BMF_PHYSDEVICE
    SIZE_T      lNextScan;              // Offset from scan n to n+1

    // Clipping optimization stuff

    LONG        ClipTop;                // S3 clip states.
    LONG        ClipLeft;
    LONG        ClipRight;
    LONG        ClipBottom;

    // Default clip object

    CLIPOBJ     *pcoDefault,                // ptr to a default clip obj
                *pcoFullRam;                // Clip Object for the full RAM

    DDAOBJ      *pdda;                       // DDA Oject used by traps

    // Registers shadows for the 911

    WORD    ForegroundMix,
            BackgroundMix,
            ForegroundColor,
            BackgroundColor,
            WriteMask,
            ReadMask;

    // Brush optimization stuff

    ULONG   gBrushUnique;               // Unique Brush ID source.
    INT     iMaxCachedColorBrushes,
            iNextColorBrushCacheSlot;
    PULONG  pulColorBrushCacheEntries;
    ULONG   ulColorExpansionCacheTag;

    INT     iMaxCachedMonoBrushes,
            iNextMonoBrushCacheSlot,
            iNextMonoBrushExpansionSlot;
    PULONG  pulMonoBrushCacheEntries;
    ULONG   aulMonoExpansionCacheTag[MAX_MONO_BRUSH_EXPANSION_SLOTS];

    // Host to Source copy optimization stuff.

    HSURF   hsurfCachedBitmap;
    ULONG   iUniqCachedBitmap;
    ULONG   iUniqXlate;

    // 801/805/928 bank control stuff.

    WORD        SysCnfg,                // System Config register
                LawCtl;                 // Linear Address Window Control register
    BYTE        ExtSysCtl2;             // Extended System Control 2 Register

    // 928 / Bt485 control stuff;

    WORD        ExtDacCtl;              // Exteneded DAC control.
    BYTE        Bt485CmdReg0,           // Bt485 Command Reg 0
                Bt485CmdReg1,           // Bt485 Command Reg 1
                Bt485CmdReg2,           // Bt485 Command Reg 2
                Bt485CmdReg3;           // Bt485 Command Reg 3

    BYTE        s3ChipID;
    BOOL        bBt485Dac;              // True if Bt485 is present.
    BOOL        bNewBankControl;

    // Pointer (Hardware Cursor Control) shadows.

    WORD        HgcMode;                // Hgc Register.

    // Save Screen Bits stuff.

    INT                 iUniqeSaveScreenBits;

    SAVEDSCRNBITSHDR    SavedScreenBitsHeader;

} PDEV, *PPDEV;



#define BMF_PHYSDEVICE    0xff

#define VALID_SAVE_BUFFER 0x1
#define COLOR_POINTER     0x2
#define TAKE_DOWN_POINTER 0X4
#define ANIMATEUPDATE     0X8

BOOL bInitPDEV(PPDEV,PDEVMODEW);
BOOL bInitSURF(PPDEV,BOOL);
BOOL bInitPaletteInfo(PPDEV);
BOOL bInitPointer(PPDEV);
BOOL bInit256ColorPalette(PPDEV);
BOOL bInitPatterns(PPDEV, INT);
VOID vDisablePalette(PPDEV);
VOID vDisablePatterns(PPDEV);
VOID vDisableSURF(PPDEV);
DWORD getAvailableModes(HANDLE, PVIDEO_MODE_INFORMATION *, DWORD *);


// Blt punt stuff

VOID vPuntGetBits(PPDEV ppdev, SURFOBJ *psoTrg, RECTL *prclTrg);
VOID vPuntPutBits(PPDEV ppdev, SURFOBJ *psoTrg, RECTL *prclTrg);


// Bank Manager Stuff

BOOL bBankInit(PPDEV ppdev, BOOL fFirstTime);
BOOL bBankEnumStart(PPDEV ppdev, PRECTL prclScans, SURFOBJ *pso, CLIPOBJ *pco);
BOOL bSrcBankEnumStart(PPDEV ppdev, PRECTL prclScans, SURFOBJ *pso, CLIPOBJ *pco, RECTL *prclDest);
BOOL bBankEnum(PPDEV ppdev, PRECTL prclScans, SURFOBJ *pso, CLIPOBJ *pco);
BOOL bBankEnumEnd(PPDEV ppdev, SURFOBJ *pso, CLIPOBJ *pco);


// Pointer function prototypes

VOID vMoveHardwarePointer(SURFOBJ *,LONG,LONG);
BOOL bSetHardwarePointerShape(SURFOBJ  *,SURFOBJ *,SURFOBJ  *, XLATEOBJ *,
                              LONG, LONG, LONG, LONG, FLONG);

#define MAX_CLUT_SIZE (sizeof(VIDEO_CLUT) + (sizeof(ULONG) * 256))

// Size of the DrierExtra information in the DEVMODE structure for the S3

#define DRIVER_EXTRA_SIZE 0

#include "s3.h"
