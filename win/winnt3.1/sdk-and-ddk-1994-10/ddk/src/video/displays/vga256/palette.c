/******************************Module*Header*******************************\
* Module Name: palette.c
*
* Palette support.
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "driver.h"

// Global Table defining the 20 Window Default Colors.  For 256 color
// palettes the first 10 must be put at the beginning of the palette
// and the last 10 at the end of the palette.

const PALETTEENTRY BASEPALETTE[20] =
{
    { 0,   0,   0,   0 },       // 0
    { 0x80,0,   0,   0 },       // 1
    { 0,   0x80,0,   0 },       // 2
    { 0x80,0x80,0,   0 },       // 3
    { 0,   0,   0x80,0 },       // 4
    { 0x80,0,   0x80,0 },       // 5
    { 0,   0x80,0x80,0 },       // 6
    { 0xC0,0xC0,0xC0,0 },       // 7
    { 192, 220, 192, 0 },       // 8
    { 166, 202, 240, 0 },       // 9
    { 255, 251, 240, 0 },       // 10
    { 160, 160, 164, 0 },       // 11
    { 0x80,0x80,0x80,0 },       // 12
    { 0xFF,0,   0   ,0 },       // 13
    { 0,   0xFF,0   ,0 },       // 14
    { 0xFF,0xFF,0   ,0 },       // 15
    { 0   ,0,   0xFF,0 },       // 16
    { 0xFF,0,   0xFF,0 },       // 17
    { 0,   0xFF,0xFF,0 },       // 18
    { 0xFF,0xFF,0xFF,0 },       // 19
};

BOOL bInitDefaultPalette(PPDEV ppdev);

/******************************Public*Routine******************************\
* bInitPaletteInfo
*
* Initializes the palette information for this PDEV.
*
* Called by DrvEnablePDEV.
*
\**************************************************************************/

BOOL bInitPaletteInfo(PPDEV ppdev)
{
    if (!bInitDefaultPalette(ppdev))
        return(FALSE);

    return(TRUE);
}

/******************************Public*Routine******************************\
* vDisablePalette
*
* Frees resources allocated by bInitPaletteInfo.
*
\**************************************************************************/

VOID vDisablePalette(PPDEV ppdev)
{
// Delete the default palette if we created one.

    if (ppdev->hpalDefault)
    {
        EngDeletePalette(ppdev->hpalDefault);
        ppdev->hpalDefault = (HPALETTE) 0;
    }

    if (ppdev->pPal != (PPALETTEENTRY)NULL)
        LocalFree((PVOID)ppdev->pPal);
}

/******************************Public*Routine******************************\
* bInitDefaultPalette
*
* Initializes default palette for PDEV.
*
\**************************************************************************/

BOOL bInitDefaultPalette(PPDEV ppdev)
{
    ULONG ulLoop;
    BYTE  jRed;
    BYTE  jGre;
    BYTE  jBlu;

    // Allocate our palette

    ppdev->pPal = (PPALETTEENTRY)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT,
            (sizeof(PALETTEENTRY) * 256));

    if ((ppdev->pPal) == NULL)
    {
        RIP("Couldn't allocate default palette");
        return(FALSE);
    }

    // Generate 256 (8*4*4) RGB combinations to fill the palette

    jRed = jGre = jBlu = 0;

    for (ulLoop = 0; ulLoop < 256; ulLoop++)
    {
        ppdev->pPal[ulLoop].peRed   = jRed;
        ppdev->pPal[ulLoop].peGreen = jGre;
        ppdev->pPal[ulLoop].peBlue  = jBlu;
        ppdev->pPal[ulLoop].peFlags = (BYTE)0;

        if (!((jRed += 8) & 0x3F))
            if (!((jGre += 8) & 0x3F))
                jBlu += 16;
    }

// Fill in Windows Reserved Colors from the WIN 3.0 DDK
// The Window Manager reserved the first and last 10 colors for
// painting windows borders and for non-palette managed applications.

    for (ulLoop = 0; ulLoop < 10; ulLoop++)
    {
    // First 10

        ppdev->pPal[ulLoop].peRed   = BASEPALETTE[ulLoop].peRed   ;
        ppdev->pPal[ulLoop].peGreen = BASEPALETTE[ulLoop].peGreen ;
        ppdev->pPal[ulLoop].peBlue  = BASEPALETTE[ulLoop].peBlue  ;
        ppdev->pPal[ulLoop].peFlags = BASEPALETTE[ulLoop].peFlags;

    // Last 10

        ppdev->pPal[246+ulLoop].peRed   = BASEPALETTE[ulLoop+10].peRed   ;
        ppdev->pPal[246+ulLoop].peGreen = BASEPALETTE[ulLoop+10].peGreen ;
        ppdev->pPal[246+ulLoop].peBlue  = BASEPALETTE[ulLoop+10].peBlue  ;
        ppdev->pPal[246+ulLoop].peFlags = BASEPALETTE[ulLoop+10].peFlags;
    }

// Create handle for palette.

    ppdev->hpalDefault = EngCreatePalette(PAL_INDEXED,
                                               256,
                                               (PULONG) ppdev->pPal,
                                               0,0,0);
    ppdev->pDevInfo->hpalDefault = ppdev->hpalDefault;

    if (ppdev->hpalDefault == (HPALETTE) 0)
    {
        RIP("Couldn't create default palette");
        LocalFree(ppdev->pPal);
        return(FALSE);
    }

// Initialize the hardware with the initial palette.

    return(TRUE);
}

/******************************Public*Routine******************************\
* bInit256ColorPalette
*
* Initialize the hardware's palette registers.
*
\**************************************************************************/

BOOL bInit256ColorPalette(PPDEV ppdev)
{
    BYTE          ajClutSpace[MAX_CLUT_SIZE];
    PVIDEO_CLUT   pScreenClut = (PVIDEO_CLUT) ajClutSpace;
    ULONG         ulReturnedDataLength;
    PALETTEENTRY* ppalFrom;
    PALETTEENTRY* ppalTo;
    PALETTEENTRY* ppalEnd;

    // Fill in pScreenClut header info

    pScreenClut->NumEntries = 256;
    pScreenClut->FirstEntry = 0;

    // Copy Colors in.

    ppalFrom = ppdev->pPal;
    ppalTo   = (PALETTEENTRY*) pScreenClut->LookupTable;
    ppalEnd  = &ppalTo[256];

    for (; ppalTo < ppalEnd; ppalFrom++, ppalTo++)
    {
        ppalTo->peRed   = ppalFrom->peRed   >> 2;
        ppalTo->peGreen = ppalFrom->peGreen >> 2;
        ppalTo->peBlue  = ppalFrom->peBlue  >> 2;
        ppalTo->peFlags = 0;
    }

    // Set palette registers

    if (!DeviceIoControl(ppdev->hDriver,
                         IOCTL_VIDEO_SET_COLOR_REGISTERS,
                         pScreenClut,
                         MAX_CLUT_SIZE,
                         NULL,
                         0,
                         &ulReturnedDataLength,
                         NULL))
    {
        return(FALSE);
    }

    return(TRUE);
}

/******************************Public*Routine******************************\
* DrvSetPalette
*
* DDI entry point for manipulating the palette.
*
\**************************************************************************/

BOOL DrvSetPalette(
IN DHPDEV dhpdev,
IN PALOBJ *ppalo,
IN FLONG  fl,
IN ULONG  iStart,
IN ULONG  cColors)
{
    BYTE          ajClutSpace[MAX_CLUT_SIZE];
    PVIDEO_CLUT   pScreenClut = (PVIDEO_CLUT) ajClutSpace;
    PALETTEENTRY* ppal;
    PALETTEENTRY* ppalEnd;

    UNREFERENCED_PARAMETER(fl);

// Fill in pScreenClut header info

    pScreenClut->NumEntries = (USHORT) cColors;
    pScreenClut->FirstEntry = (USHORT) iStart;

    ppal = (PPALETTEENTRY) (pScreenClut->LookupTable);

    if (cColors != PALOBJ_cGetColors(ppalo, iStart, cColors, (PULONG) ppal))
    {
        return(FALSE);
    }

// Set the high reserved byte in each palette entry to 0.

    for (ppalEnd = &ppal[cColors]; ppal < ppalEnd; ppal++)
    {
        ppal->peRed   >>= 2;
        ppal->peGreen >>= 2;
        ppal->peBlue  >>= 2;
        ppal->peFlags = 0;
    }

// Set palette registers

    if (!DeviceIoControl(((PPDEV)(dhpdev))->hDriver,
                          IOCTL_VIDEO_SET_COLOR_REGISTERS,
                          pScreenClut,
                          MAX_CLUT_SIZE,
                          NULL,
                          0,
                          &cColors,
                          NULL))
    {
        return(FALSE);
    }

    return(TRUE);
}
