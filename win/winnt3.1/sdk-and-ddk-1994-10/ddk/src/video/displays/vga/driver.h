/******************************Module*Header*******************************\
* Module Name: driver.h
*
* driver prototypes
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "stddef.h"
#include "windows.h"
#include "winddi.h"
#include <devioctl.h>
#include <ntddvdeo.h>

#include "debug.h"

#include "brush.h"
/* gflDrv */

#define DRV_ENABLED_ONCE        0x00000001
#define DRV_ENABLED_PDEV        0x00000002

// # of bytes at end of display memory required for the pointer work and save
// areas

//BUGBUG  not really sure about these
#define POINTER_WORK_AREA_SIZE   1024
#define POINTER_SAVE_AREA_SIZE   256

#define MIN_TEMP_BUFFER_SIZE    0x4000  // Minimum size of buffer used to
                                        //  build text (may be bigger because
                                        //  it's shared with blt buffer)

// Space required for working storage when working with banking on adapters
// that support only one window, with no independent read and write bank
// selection. The largest supported bank is 64K; this constant provides for
// storing four 64K planes.

#define BANK_BUFFER_PLANE_SIZE  0x10000
#define BANK_BUFFER_SIZE_1RW    (((ULONG)BANK_BUFFER_PLANE_SIZE)*4)
#define PLANE_0_OFFSET          0
#define PLANE_1_OFFSET          BANK_BUFFER_PLANE_SIZE
#define PLANE_2_OFFSET          (BANK_BUFFER_PLANE_SIZE*2)
#define PLANE_3_OFFSET          (BANK_BUFFER_PLANE_SIZE*3)

// Space required for working storage when working with banking on adapters
// that support one readable window and one writable window, but not two
// independently read/write addressable windows. This is space for storing
// one bank's worth of edge words for each of four planes, or for the text
// building buffer, whichever is larger.

#define BANK_MAX_HEIGHT 512     // tallest supported bank
#define BANK_BUFFER_SIZE_1R1W   (max((((ULONG)BANK_MAX_HEIGHT)*4*2), \
                                    MIN_TEMP_BUFFER_SIZE))

// On a 2RW or unbanked adapter, just make space for the text building buffer.
#define BANK_BUFFER_SIZE_UNBANKED MIN_TEMP_BUFFER_SIZE
#define BANK_BUFFER_SIZE_2RW      MIN_TEMP_BUFFER_SIZE

// Amount of space to reserve for saved screen bits buffer preallocated at
// EnablePDEV time, so we usually won't have to allocate and deallocate the
// memory. 36K should be big enough for most menus (not 32K because the big
// system menu on consoles is just slightly larger than 32K).
#define PREALLOC_SSB_SIZE   0x10000

/* This device can have only one PDEV */

#define DRV_ONE_PDEV    1


//
// Sizes assumed for 1-window and 2 RW-window banks.
//
#define BANK_SIZE_1_WINDOW      0x10000
#define BANK_SIZE_2RW_WINDOW    0x08000

//
// The following value allows us to set rounding for cursor exclusion.
//

#define POINTER_ROUNDING_SIZE 8

#define POINTER_ROUND (POINTER_ROUNDING_SIZE - 1)
#define POINTER_MASK  (-POINTER_ROUNDING_SIZE)

//
// Determines the size of the DriverExtra information in the DEVMODE
// structure passed to and from the display driver.
//

#define DRIVER_EXTRA_SIZE 0

//
// Miscellaneous driver flags in pdev.fl
// Must be mirrored in i386\driver.inc
//

#define DRIVER_USE_OFFSCREEN    0x02L  // if not set, don't use offscreen memory

//
// General Rectangle Enumeration structure
//

#define ENUM_RECT_LIMIT   50

typedef struct _RECT_ENUM
{
    ULONG   c;
    RECTL   arcl[ENUM_RECT_LIMIT];
} RECT_ENUM;


/**************************************************************************\
*
* Descriptor for a saved screen bits block
*
\**************************************************************************/

typedef struct  _SAVED_SCREEN_BITS
{
    BOOL  bFlags;
    PBYTE pjBuffer;  // pointer to save buffer start
    ULONG ulSize;    // size of save buffer (per plane; display memory only)
    ULONG ulSaveWidthInBytes; // # of bytes across save area (including
                              //  partial edge bytes, if any)
    ULONG ulDelta;   // # of bytes from end of one saved scan's saved bits to
                     //  start of next (system memory only)
    PVOID pvNextSSB; // pointer to next saved screen bits block
                     // for system memory blocks, saved bits start immediately
                     //  after this structure
} SAVED_SCREEN_BITS, *PSAVED_SCREEN_BITS;
#define SSB_IN_ADAPTER_MEMORY   0x01    // true if block saved in adapter mem
#define SSB_IN_PREALLOC_BUFFER  0x02    // true if block saved in preallocated
                                        //  buffer

/**************************************************************************\
*
* Bank clipping info
*
\**************************************************************************/

typedef struct {
    RECTL rclBankBounds;    // describes pixels addressable in this bank
    ULONG ulBankOffset;     // offset of bank start from bitmap start, if
                            // the bitmap were linearly addressable
} BANK_INFO, *PBANK_INFO;


/**************************************************************************\
*
* Bank control function vector
*
\**************************************************************************/

typedef VOID (*PFN_BankControl)(PDEVSURF, ULONG, BANK_JUST);


/**************************************************************************\
*
* Physical device description block
*
\**************************************************************************/

#define CURSOR_DOWN      0x00000001
#define CURSOR_COLOR     0x00000004
#define CURSOR_HW        0x00000010
#define CURSOR_HW_ACTIVE 0x00000020
#define CURSOR_ANIMATE   0x00000040

// The XYPAIR structure is used to allow ATOMIC read/write of the cursor
// position.  NEVER, NEVER, NEVER get or set the cursor position without
// doing this.  There is no semaphore protection of this data structure,
// nor will there ever be any. [donalds]

typedef struct  _XYPAIR
{
    USHORT  x;
    USHORT  y;
} XYPAIR;

// Must be mirrored in i386\strucs.inc

typedef struct  _PDEV
{
    FLONG   fl;                         // Driver flags (DRIVER_xxx)
    IDENT   ident;                      // Identifier
    HANDLE  hDriver;                    // Handle to the miniport
    HDEV    hdevEng;                    // Engine's handle to PDEV
    HSURF   hsurfEng;                   // Engine's handle to surface
    PVOID   pdsurf;                     // Associated surface
    SIZEL   sizlSurf;                   // Displayed size of the surface
    DWORD   ulIs386;                    // 1 if 386, 0 if 486 or higher
    PBYTE   pjScreen;                   // Pointer to the frame buffer base
    XYPAIR  xyCursor;                   // Where the cursor should be
    POINTL  ptlExtent;                  // Cursor extent
    ULONG   cExtent;                    // Effective cursor extent.
    XYPAIR  xyHotSpot;                  // Cursor hot spot
    FLONG   flCursor;                   // Cursor status
    DEVINFO devinfo;                    // Device info
    HBITMAP ahbmPat[HS_DDI_MAX];        // Engine handles to standard patterns
    GDIINFO GdiInfo;                    // Device capabilities
    ULONG   ulModeNum;                  // Mode index for current VGA mode
    PVIDEO_POINTER_ATTRIBUTES pPointerAttributes; // HW Pointer Attributes
    ULONG   XorMaskStartOffset;         // Start offset of hardware pointer
                                        //  XOR mask relative to AND mask for
                                        //  passing to HW pointer
    DWORD   cjPointerAttributes;        // Size of buffer allocated
    DWORD   flPreallocSSBBufferInUse;   // True if preallocated saved screen
                                        //  bits buffer is in use
    PUCHAR  pjPreallocSSBBuffer;        // Pointer to preallocated saved screen
                                        //  bits buffer, if there is one
    ULONG   ulPreallocSSBSize;          // Size of preallocated saved screen
                                        //  bits buffer
    VIDEO_POINTER_CAPABILITIES PointerCapabilities; // HW pointer abilities
    PUCHAR  pucDIB4ToVGAConvBuffer;     // Pointer to DIB4->VGA conversion
                                        //  table buffer
    PUCHAR  pucDIB4ToVGAConvTables;     // Pointer to DIB4->VGA conversion
                                        //  table start in buffer (must be on a
                                        //  256-byte boundary)
} PDEV, *PPDEV;


/**************************************************************************\
*
* Specifies desired justification for requestion scan line within bank window
*
\**************************************************************************/

typedef enum {
    JustifyTop = 0,
    JustifyBottom,
} BANK_JUST;


/**************************************************************************\
*
* Specifies which window is to be mapped by two-window bank handler.
*
\**************************************************************************/

typedef enum {
    MapSourceBank = 0,
    MapDestBank,
} BANK_JUST;


/**************************************************************************\
*
* Definition of a surface as seen and used by the various VGA drivers
*
\**************************************************************************/

// Must be mirrored in i386\strucs.inc

typedef struct _DEVSURF                 /* dsurf */
{
    IDENT       ident;                  // Identifier for debugging ease
    ULONG       flSurf;                 // DS_ flags as defined below
    BYTE        iColor;                 // Solid color surface if DS_SOLIDBRUSH

// If DS_SOLIDBRUSH, the following fields are undefined and not guaranteed to
// have been allocated!

    BYTE        iFormat;                // BMF_*, BMF_PHYSDEVICE
    BYTE        jReserved1;             // Reserved
    BYTE        jReserved2;             // Reserved
    PPDEV       ppdev;                  // Pointer to associated PDEV
    SIZEL       sizlSurf;               // Size of the surface
    SIZE_T      lNextScan;              // Offset from scan  "n" to "n+1"
    SIZE_T      lNextPlane;             // Offset from plane "n" to "n+1"
    PVOID       pvScan0;                // Pointer to scan 0 of bitmap
                                        //  (actual address of start of bank,
                                        //  for banked VGA surface)
    PVOID       pvStart;                // Pointer to start of bitmap
    PVOID       pvConv;                 // Pointer to DIB/Planer conversion buffer
                                        // Banking variables; used only for
                                        //  banked VGA surfaces
    PVIDEO_BANK_SELECT pBankSelectInfo; // Pointer to bank select info
                                        //  returned by miniport
    ULONG       ulBank2RWSkip;          // Offset from one bank index to next
                                        //  to make two 32K banks appear to be
                                        //  one seamless 64K bank
    PFN         pfnBankSwitchCode;      // Pointer to bank switch code
    VIDEO_BANK_TYPE vbtBankingType;     // Type of banking
    ULONG       ulBitmapSize;           // Length of bitmap if there were no
                                        //  banking, in CPU addressable bytes
    ULONG       ulPtrBankScan;          // Last scan line in pointer work bank
    RECTL       rcl1WindowClip;         // Single-window banking clip rect
    RECTL       rcl2WindowClip[2];      // Double-window banking clip rects for
                                        //  windows 0 & 1
    ULONG       ulWindowBank[2];        // Current banks mapped into windows
                                        //  0 & 1 (used in 2 window mode only)
    PBANK_INFO  pbiBankInfo;            // Pointer to array of bank clip info
    ULONG       ulBankInfoLength;       // Length of pbiBankInfo, in entries
    PBANK_INFO  pbiBankInfo2RW;         // Same as above, but for 2RW window
    ULONG       ulBankInfo2RWLength;    // case
    PFN_BankControl pfnBankControl;     // Pointer to bank control function
    PFN_BankControl pfnBankControl2Window; // Pointer to double-window bank
                                        //  control function
    PVOID       pvBitmapStart;          // Single-window bitmap start pointer
                                        //  (adjusted as necessary to make
                                        //  window map in at proper offset)
    PVOID       pvBitmapStart2Window[2]; // Double-window window 0 and 1 bitmap
                                         // start
    PVOID       pvBankBufferPlane0;     // Pointer to temp buffer capable of
                                        //  storing one full bank for plane 0
                                        //  for 1 R/W case; capable of storing
                                        //  one full bank height of edge bytes
                                        //  for all four planes for the 1R/1W
                                        //  case. Also used to point to text
                                        //  building buffer in all cases
                                        // This is the pointer used to dealloc
                                        //  bank working storage for all four
                                        //  planes
                                        // The following 3 pointers used by
                                        //  1 R/W banked devices
    PVOID       pvBankBufferPlane1;     // Like above, but for plane 1
    PVOID       pvBankBufferPlane2;     // Like above, but for plane 2
    PVOID       pvBankBufferPlane3;     // Like above, but for plane 3
    ULONG       ulTempBufferSize;       // Full size of temp buffer pointed to
                                        //  by pvBankBufferPlane0

    ULONG       ajBits[1];              // Bits will start here for device bitmaps
    PSAVED_SCREEN_BITS ssbList;         // Pointer to start of linked list of
                                        //  saved screen bit blocks
    PBYTE       pjAdapterHeapStart;     // First byte of display memory
                                        //  available for temporary storage
    PBYTE       pjAdapterHeapEnd;       // Last byte of display memory
                                        //  available for temporary storage
    PBYTE       pjAdapterHeapTop;       // Last byte of display memory
                                        // currently available for temporary
                                        // storage
} DEVSURF, * PDEVSURF;


// Identifier stored in ds.ident for debugging

#define PDEV_IDENT      ('V' + ('P' << 8) + ('D' << 16) + ('V' << 24))
#define DEVSURF_IDENT   ('V' + ('S' << 8) + ('R' << 16) + ('F' << 24))


// Definition of the BMF_PHYSDEVICE format type

#define BMF_PHYSDEVICE  0xFF


// Flags for flSurf

#define DS_SOLIDBRUSH   0x00000001      // Surface is a solid color representing a brush
#define DS_GREYBRUSH    0x00000002      // Surface is a grey color representing a brush
#define DS_BRUSH        0x00000004      // Surface is a brush
#define DS_DIB          0x00000008      // Surface is supporting an Engine DIB

/**************************************************************************\
*
* Function prototypes shared by all C files.
*
\**************************************************************************/


VOID vInitRegs(void);
BOOL bInitVGA(PPDEV, BOOL);
BOOL bInitPDEV(PPDEV, DEVMODEW *, GDIINFO *, DEVINFO *);

DWORD getAvailableModes(HANDLE, PVIDEO_MODE_INFORMATION *, DWORD *);
typedef VOID (*PFN_ScreenToScreenBlt)(PDEVSURF, PRECTL, PPOINTL, INT);

VOID vDIB2VGA( DEVSURF * pdsurfDst, DEVSURF * pdsurfSrc,
              RECTL * prclDst, POINTL * pptlSrc, UCHAR * pucConv);
BOOL DrvIntersectRect(PRECTL prcDst, PRECTL prcSrc1, PRECTL prcSrc2);
FLONG flClipRect(PRECTL,PRECTL,PRECTL);
VOID vMonoPatBlt(PDEVSURF,ULONG,PRECTL,MIX, BRUSHINST *,PPOINTL);
VOID vClrPatBlt(PDEVSURF,ULONG,PRECTL,MIX, BRUSHINST *,PPOINTL);
VOID vTrgBlt(PDEVSURF,ULONG,PRECTL,MIX,ULONG);
VOID vAlignedSrcCopy(PDEVSURF,PRECTL,PPOINTL,INT);
VOID vNonAlignedSrcCopy(PDEVSURF,PRECTL,PPOINTL,INT);
VOID vDibToDev(PDEVSURF pdsurf, SURFOBJ *pso, PVOID pvConv);
VOID vConvertVGA2DIB(PDEVSURF, LONG, LONG, VOID *, LONG, LONG, ULONG, ULONG,
    LONG, ULONG, ULONG *);
BOOL SimCopyBits(SURFOBJ *psoTrg, SURFOBJ *psoSrc, CLIPOBJ *pco,
                 XLATEOBJ *pxlo, PRECTL prclTrg, PPOINTL pptlSrc);
VOID vConvertVGA2DIB(PDEVSURF, LONG, LONG, VOID *, LONG, LONG, ULONG, ULONG, LONG,
    ULONG, ULONG *);
BOOL bRleBlt(SURFOBJ *,SURFOBJ *, CLIPOBJ *, XLATEOBJ *, RECTL *, POINTL *);
VOID vSetDIB4ToVGATables(UCHAR *);
BOOL DrvFillPath (SURFOBJ *pso, PATHOBJ *ppo, CLIPOBJ *pco, BRUSHOBJ *pbo,
    POINTL *pptlBrush, MIX mix, FLONG flOptions);
VOID vClearMemDword(ULONG * pulBuffer, ULONG ulDwordCount);
VOID vForceBank0(PPDEV ppdev);

