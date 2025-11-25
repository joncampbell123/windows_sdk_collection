/******************************Module*Header*******************************\
* Module Name: brush.c
*
* Contains the brush realization and dithering code.
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/
#include "driver.h"

#define OBR_REALIZED 1
#define OBR_4BPP     2

// aulDefBitMapping is used to translate packed pel into Planar

extern  ULONG aulDefBitMapping[8];
extern  ULONG gRealizedBrushHeight[HS_DDI_MAX*2];
extern  BYTE  gaajRealizedPat[HS_DDI_MAX][32];

// Asm routines

ULONG CountColors(VOID *, ULONG, WORD *, DWORD);
BOOL bQuickPattern(BYTE *, ULONG);
BOOL bShrinkPattern(BYTE *, ULONG);

VOID vMono16Wide(DWORD *, DWORD *, DWORD);
VOID vMono8Wide(DWORD *, DWORD *, DWORD);
VOID vMono4Wide(DWORD *, DWORD *, DWORD);
VOID vMono2Wide(DWORD *, DWORD *, DWORD);
VOID vBrush2ColorToMono(BYTE *, BYTE *, DWORD, DWORD, BYTE);
VOID vConvert4BppToPlanar(BYTE *, BYTE *, DWORD, DWORD *);
VOID vConvert8BppToPlanar(BYTE *, BYTE *, DWORD, DWORD *);
VOID vCopyOrgBrush(BYTE   *pDest, BYTE   *pSrc, LONG   lScan, XLATEOBJ *pxlo);
VOID vCreatePlaneMasks(BYTE *, BYTE *);
SURFOBJ *DrvConvertBrush(SURFOBJ *psoPattern, HBITMAP *phbmTmp, XLATEOBJ *pxlo,
    ULONG cx, ULONG cy);

typedef VOID (*PFNV)();

/******************************Public*Routine******************************\
* DrvRealizeBrush
*
*
\**************************************************************************/

BOOL DrvRealizeBrush(
BRUSHOBJ *pbo,
SURFOBJ  *psoTarget,
SURFOBJ  *psoPattern,
SURFOBJ  *psoMask,
XLATEOBJ *pxlo,
ULONG    iHatch)
{
    ULONG           cx;                 // Height of pattern surface
    ULONG           cy;                 // Width  of pattern surface
    LONG            cbScan;             // Width in bytes of one scan
    PVOID           pvBits;             // Source bits
    ULONG          *pulXlate;           // Color translation
    HBITMAP         hbmTmp;             // Temp bmp handle for brush conversion
    BRUSHINST      *pbri;               // pointer to where realization goes
    BYTE            jBkColor, jFgColor; // local copies of mono attributes
    PFNV            pfnConvert;         // function pointer to mono conversion
    BYTE            jColors[2];         // place holder for special color->mono
                                        // conversion.
    BOOL            bConversion = FALSE; // True if we converted to 4bpp

    cx = psoPattern->sizlBitmap.cx;
    cy = psoPattern->sizlBitmap.cy;

    if ((cy != 8) || (cx > 16))
    {
        return(FALSE);
    }

    pbri = BRUSHOBJ_pvAllocRbrush(pbo,sizeof(BRUSHINST));
    if (pbri == (BRUSHINST *)NULL)
        return(FALSE);

    pbri->RealWidth = (BYTE)cx;

    switch (psoPattern->iBitmapFormat)
    {
    case BMF_1BPP:
    case BMF_4BPP:
    case BMF_8BPP:
        break;

    default:
        if ((cx != 8) && (cx != 16))
            return(FALSE);

        // Convert to 4bpp

        psoPattern = DrvConvertBrush(psoPattern, &hbmTmp, pxlo, cx, cy);
        if (psoPattern == (SURFOBJ *)NULL)
            return(FALSE);

        bConversion = TRUE;
        break;
    }

    //
    // Setup the pointer to the bits, and the scan-to-scan advance direction.
    //
    cbScan = psoPattern->lDelta;
    pvBits = psoPattern->pvScan0;

    //
    // If this is a hatch brush, we already have it in realized form.
    //
    if ((iHatch < HS_DDI_MAX) && (psoPattern->iBitmapFormat == BMF_1BPP))
    {
        pbri->usStyle = BRI_MONO_PATTERN;
        pbri->fjAccel = 0;
        pbri->Width = 16;
        pbri->Height = gRealizedBrushHeight[iHatch*2];
        pbri->YShiftValue = (BYTE)gRealizedBrushHeight[(iHatch*2)+1];
        pbri->pPattern = (BYTE *)&(gaajRealizedPat[iHatch]);
        pbri->jBkColor =  (BYTE)pxlo->pulXlate[0];
        pbri->jFgColor =  (BYTE)pxlo->pulXlate[1];
        pbri->jOldBrushRealized = 0;

        return(TRUE);
    }

    if (psoPattern->iBitmapFormat == BMF_1BPP)
    {
        switch (cx) {
            case 16:
                if ((bShrinkPattern)((BYTE *)pvBits, cbScan))
                {
                    cx = 8;
                    pbri->RealWidth = (BYTE)cx;
                } else {
                    pfnConvert = vMono16Wide;
                    break;
                }
            case 8:
                pfnConvert = vMono8Wide;
                break;

            case 2:
                pfnConvert = vMono2Wide;
                break;

            case 4:
                pfnConvert = vMono4Wide;
                break;

            default:
                return(FALSE);
        }

        pbri->usStyle = BRI_MONO_PATTERN;
        pbri->fjAccel = 0;
        pbri->jBkColor =  (BYTE)pxlo->pulXlate[0];
        pbri->jFgColor =  (BYTE)pxlo->pulXlate[1];
        pbri->Width = 16;
        pbri->pPattern = (BYTE *)&(pbri->ajPattern[0]);
        pbri->jOldBrushRealized = 0;

        (*pfnConvert)(pbri->pPattern, pvBits, cbScan);

        if (bQuickPattern(pbri->pPattern, 8))
        {
            pbri->Height = 2;
            pbri->YShiftValue = 1;
        }
        else
        {
            pbri->Height = 8;
            pbri->YShiftValue = 3;
        }

        return(TRUE);
    }

    if ((cx != 8) && (cx != 16))
        return(FALSE);

    if (pxlo->flXlate & XO_TABLE)
        pulXlate = pxlo->pulXlate;
    else
        pulXlate = (PULONG)NULL;

    if ((psoPattern->iBitmapFormat == BMF_4BPP) &&
            (CountColors(pvBits, cx, (WORD *)&jColors, cbScan) == 2)) {

        if ((cx == 16) && (bShrinkPattern)((BYTE *)pvBits, cbScan))
        {
            cx = 8;
            pbri->RealWidth = (BYTE)cx;
        }

        pbri->usStyle = BRI_MONO_PATTERN;
        pbri->Height = 8;
        pbri->YShiftValue = 3;
        pbri->Width = 16;
        pbri->fjAccel = 1;
        pbri->pPattern = (BYTE *)&(pbri->ajPattern[0]);
        pbri->jOldBrushRealized = OBR_4BPP;

        if (pulXlate != (PULONG)NULL) {

            jBkColor = (BYTE)pulXlate[jColors[0]];
            jFgColor = (BYTE)pulXlate[jColors[1]];
        } else {

            jBkColor = jColors[0];
            jFgColor = jColors[1];
        }

        if (jBkColor > jFgColor) {
            pbri->jBkColor = jBkColor;
            pbri->jFgColor = jFgColor;
        } else {
            pbri->jBkColor = jFgColor;
            pbri->jFgColor = jBkColor;
        }


        vBrush2ColorToMono(pbri->pPattern, (BYTE *)pvBits, cbScan,
                cx, pbri->jBkColor);

        if (bQuickPattern(pbri->pPattern, 8))
        {
            pbri->Height = 2;
            pbri->YShiftValue = 1;
        }

        vCopyOrgBrush(&(pbri->ajC0[0]),pvBits, cbScan, pxlo);

        if (bConversion) {
            EngUnlockSurface(psoPattern);
            EngDeleteSurface((HSURF)hbmTmp);
        }

        return(TRUE);
    }
    else if (cx != 8)
        return(FALSE);

    pbri->pPattern = (BYTE *)&(pbri->ajC0[0]);

    // At this point we know we have an 8x8 color pattern in either a
    // 4bpp or 8bpp format.

    if (psoPattern->iBitmapFormat == BMF_4BPP)
        vConvert4BppToPlanar(pbri->pPattern, (BYTE *)pvBits, cbScan, pulXlate);

    else // 8bpp
        vConvert8BppToPlanar(pbri->pPattern, (BYTE *)pvBits, cbScan, pulXlate);
#ifdef FAST_PLANE
    vCreatePlaneMasks(&(pbri->ajPlaneMasks),pbri->pPattern);
#endif
    // Set proper accelerators in the brush for the output code.

    pbri->usStyle = BRI_COLOR_PATTERN;  // Brush style is arbitrary pattern
    pbri->fjAccel = 0;                  // Accelerator flags - no special casing
    pbri->Height = 8;
    pbri->YShiftValue = 3;
    pbri->Width = 8;
    pbri->fjAccel = 1;
    pbri->jOldBrushRealized = OBR_REALIZED|OBR_4BPP;

    if (bConversion) {
        EngUnlockSurface(psoPattern);
        EngDeleteSurface((HSURF)hbmTmp);
    }

    return(TRUE);
}

/****************************************************************************\
* DrvvConvertBrush()
*
* Converts a brush to a 4bpp bmp
*
\****************************************************************************/

SURFOBJ *DrvConvertBrush(
    SURFOBJ  *psoPattern,
    HBITMAP  *phbmTmp,
    XLATEOBJ *pxlo,
    ULONG    cx,
    ULONG    cy)
{
    SURFOBJ *psoTmp;
    RECTL    rclTmp;
    SIZEL    sizlTmp;
    POINTL   ptl;

    ptl.x       = 0;
    ptl.y       = 0;
    rclTmp.top  = 0;
    rclTmp.left = 0;
    rclTmp.right  = cx;
    sizlTmp.cx = cx;
    rclTmp.bottom = cy;
    sizlTmp.cy = cy;

    // Create bitmap in our compatible format.

    *phbmTmp = EngCreateBitmap(sizlTmp, cx / 2, BMF_4BPP, 0, NULL);

    if ((*phbmTmp) && ((psoTmp = EngLockSurface((HSURF)*phbmTmp)) != NULL))
    {
        if (EngCopyBits(psoTmp, psoPattern, NULL, pxlo, &rclTmp, &ptl))
            return(psoTmp);

        EngUnlockSurface(psoTmp);
        EngDeleteSurface((HSURF)*phbmTmp);
    }

    return((SURFOBJ *)NULL);
}


/****************************************************************************\
* vCopyOrgBrush
*
* When we realize a mono or 2 color brush, we copy the original 4bpp brush
* to the ajC0 area of the realized brush. If we are called to do a
* rop that we don't directly support, vConvertBrush will be called to
* convert the orginal 4bpp brush to a planar brush that the blt compiler can
* use. Since this is a rare event, we do this on request instead of at
* realization time.
*
\****************************************************************************/

VOID vCopyOrgBrush(
    BYTE   *pDest,
    BYTE   *pSrc,
    LONG   lScan,
    XLATEOBJ *pxlo)
{
    ULONG *pulXlate, *pulDest;
    BYTE jByte, jColor;
    int i;

    if (pxlo->flXlate & XO_TABLE) {
        pulXlate = pxlo->pulXlate;

        for (i=0;i<32;i++) {

            jColor = *pSrc;         // Get Next byte
            jByte = jColor;

            jByte = pulXlate[jByte & 0xf];
            jByte = pulXlate[(jColor >> 4) & 0xf] << 4;
            *pDest = jByte;

            pSrc += lScan;
            pDest++;
        }
    } else {
        pulDest = (ULONG *)pDest;

        for (i=0;i<8;i++) {
            *pulDest = *(ULONG *)pSrc;

            pSrc += lScan;
            pulDest++;
        }

    }
}


/****************************************************************************\
* vConvertBrush()
*
* This called when we are going to do a rop3 with a non-solid brush. We have
* to convert our brush back to the old blt compiler format in order for this
* blt to work properly.
*
\****************************************************************************/

BOOL bConvertBrush(
    BRUSHINST   *pbri)
{
    BYTE jPattern[32];
    if (pbri->jOldBrushRealized & OBR_REALIZED)
        return(TRUE);

    //
    // The blt compiler only handles 8x8
    //
    if (pbri->RealWidth != 8)
        return(FALSE);

    if (pbri->jOldBrushRealized & OBR_4BPP) {
        memcpy(&(jPattern[0]), &(pbri->ajC0[0]), 32);
        vConvert4BppToPlanar(&(pbri->ajC0[0]), &(jPattern[0]), 4, NULL);
        pbri->jOldBrushRealized |= OBR_REALIZED;

        return(TRUE);
    }

    return(FALSE);
}
