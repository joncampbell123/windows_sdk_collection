
/******************************Module*Header*******************************\
* Module Name: s3mem.h
*
* contains definitions for S3's off-screen memory usage.
*
* Copyright (c) 1993 Microsoft Corporation
\**************************************************************************/

// Basic Memory definitions.

#define MEMORY_APERTURE_SIZE    0x10000

#define DRIVERS_MAX_CX          0x500

#define S3_MAX_RAM_WIDTH    ppdev->cxMaxRam
#define S3_MAX_RAM_HEIGHT   ppdev->cyMaxRam

#define S3BM_WIDTH          ppdev->cxScreen
#define S3BM_HEIGHT         ppdev->cyScreen

#define S3BM_WIDTH_HARD_VAL 1024

#define OFF_SCREEN_Y        S3BM_HEIGHT

//#define OFF_SCREEN_Y        512

#define OFF_SCREEN_CX       (S3BM_WIDTH)
#define OFF_SCREEN_CY       (S3_MAX_RAM_HEIGHT - S3BM_HEIGHT)

// Glyph cache memory definitions

#define GLYPH_CACHE_Y       OFF_SCREEN_Y
#define GLYPH_CACHE_CX      32
#define GLYPH_CACHE_CY      32

#define CACHED_GLYPHS_ROWS  2
#define GLYPHS_PER_ROW      ((S3BM_WIDTH_HARD_VAL/2) / GLYPH_CACHE_CX)

// Color pointer definitions.

#define COLOR_POINTER_Y     ((GLYPH_CACHE_CY * CACHED_GLYPHS_ROWS) + GLYPH_CACHE_Y)
#define COLOR_POINTER_CX    64
#define COLOR_POINTER_CY    64

#define COLOR_POINTER_AND_PLANE 0x01
#define COLOR_POINTER_XOR_PLANE 0x02

#define COLOR_POINTER_COLOR_DATA_X      (COLOR_POINTER_CX)
#define COLOR_POINTER_WORK_X            (COLOR_POINTER_CX * 2)
#define COLOR_POINTER_SAVE_X            (COLOR_POINTER_CX * 3)

// Double wide, double high pattern definitions.

#define OFF_SCREEN_EXPANSION_CX         512
#define OFF_SCREEN_EXPANSION_CY         176

#define MAX_DOUBLE_PATTERN_CX           16
#define MAX_DOUBLE_PATTERN_CY           16

#define COLOR_PATTERN_CACHE_X           0
#define COLOR_PATTERN_CACHE_Y           (COLOR_POINTER_Y + COLOR_POINTER_CY)
#define COLOR_PATTERN_CACHE_CX          480
#define COLOR_PATTERN_CACHE_CY          64

#define MAX_COLOR_PATTERNS              ((COLOR_PATTERN_CACHE_CX / 16) * (COLOR_PATTERN_CACHE_CY / 16))

#define MONO_PATTERN_CACHE_X            0
#define MONO_PATTERN_CACHE_Y            (COLOR_PATTERN_CACHE_Y + COLOR_PATTERN_CACHE_CY)
#define MONO_PATTERN_CACHE_CX           480
#define MONO_PATTERN_CACHE_CY           16

#define MAX_MONO_PATTERNS               ((MONO_PATTERN_CACHE_CX / 16) * (MONO_PATTERN_CACHE_CY / 16) * 8)

#define COLOR_HORZ_EXPANSION_CACHE_X    0
#define COLOR_HORZ_EXPANSION_CACHE_Y    (MONO_PATTERN_CACHE_Y + MONO_PATTERN_CACHE_CY)
#define COLOR_HORZ_EXPANSION_CACHE_CX   (OFF_SCREEN_EXPANSION_CX - MONO_VERT_EXPANSION_CACHE_CX)
#define COLOR_HORZ_EXPANSION_CACHE_CY   16

#define COLOR_VERT_EXPANSION_CACHE_X    480
#define COLOR_VERT_EXPANSION_CACHE_Y    COLOR_POINTER_Y
#define COLOR_VERT_EXPANSION_CACHE_CX   16
#define COLOR_VERT_EXPANSION_CACHE_CY   (OFF_SCREEN_EXPANSION_CY - MONO_HORZ_EXPANSION_CACHE_CY)

#define MONO_HORZ_EXPANSION_CACHE_X     0
#define MONO_HORZ_EXPANSION_CACHE_Y     (COLOR_HORZ_EXPANSION_CACHE_Y + COLOR_HORZ_EXPANSION_CACHE_CY)
#define MONO_HORZ_EXPANSION_CACHE_CX    OFF_SCREEN_EXPANSION_CX
#define MONO_HORZ_EXPANSION_CACHE_CY    16

#define MONO_VERT_EXPANSION_CACHE_X     496
#define MONO_VERT_EXPANSION_CACHE_Y     COLOR_POINTER_Y
#define MONO_VERT_EXPANSION_CACHE_CX    16
#define MONO_VERT_EXPANSION_CACHE_CY    OFF_SCREEN_EXPANSION_CY

// Offscreen bitmap cache

#define OFF_SCREEN_BITMAP_X             512
#define OFF_SCREEN_BITMAP_Y             OFF_SCREEN_Y
#define OFF_SCREEN_BITMAP_CX            512
#define OFF_SCREEN_BITMAP_CY            254

// Storage for HW monochrome pointer data
// Note that we ping-pong between two storage areas for the pointer, one on
// the bottom scan of memory and one on the second to bottom scan. However,
// we arrive at the two scans by subtracting either 0 or 1 from PTR_DATA_Y,
// so PTR_DATA_Y just points to the last scan of display memory.

#define PTR_DATA_X          0
#define PTR_DATA_Y          (S3_MAX_RAM_HEIGHT - 1)





