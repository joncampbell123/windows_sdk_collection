/******************************Module*Header*******************************\
* Module Name: Brush.c
*
* S3 Brush support
*
* Copyright (c) 1992 Microsoft Corporation
*
\**************************************************************************/

#include "driver.h"


/****************************************************************************
 *
 ***************************************************************************/
BOOL DrvRealizeBrush(
    BRUSHOBJ *pbo,
    SURFOBJ  *psoTarget,
    SURFOBJ  *psoPattern,
    SURFOBJ  *psoMask,
    XLATEOBJ *pxlo,
    ULONG    iHatch)
{
    S3BRUSH     s3Brush;
    PS3BRUSH    ps3Brush;
    INT         cjPattern;

    INT         i, j, cx, cy, lSrcDelta, lDestDelta;
    PBYTE       pbSrc, pbDest;
    FLONG       flXlate;
    PPDEV       ppdev;
    PULONG      pulXlate;


    DISPDBG((3, "S3.DLL!DrvRealizeBrush - Entry\n"));

    ppdev = (PPDEV)psoTarget->dhsurf;

    // Even if there is a mask accept the brush.
    // We will test the ROP when the brush is rendered and
    // and reject it at that time if we don't want to handle it.

#if DBG
    memset (&s3Brush, 0, sizeof(S3BRUSH));
#endif

    // Init the stack based s3 brush structure.

    s3Brush.nSize             = sizeof (S3BRUSH);
    s3Brush.iPatternID        = ++(ppdev->gBrushUnique);
    s3Brush.iBrushCacheID     = (ULONG) -1;
    s3Brush.iExpansionCacheID = (ULONG) -1;
    s3Brush.iType             = psoPattern->iType;
    s3Brush.iBitmapFormat     = psoPattern->iBitmapFormat;
    s3Brush.sizlPattern       = psoPattern->sizlBitmap;

    // Only handle standard bitmap format brushes.

    if (s3Brush.iType != STYPE_BITMAP)
        return (FALSE);

    // This selects the brush formats we support.
    // It's a switch statement so we can add more as improve the driver.

    switch (s3Brush.iBitmapFormat)
    {
        case BMF_1BPP:
        case BMF_8BPP:
            break;

        default:
            return(FALSE);

    }

    // For now, if this is not an 8 X 8 pattern then reject it. !!!
    // This will change to handle patterns up to the size of the !!!
    // source bitmap cache area. !!!

    if (s3Brush.sizlPattern.cx != 8 || s3Brush.sizlPattern.cy != 8)
        return (FALSE);

    // Note: In all cases the brush is just copied into some storage
    //       that we have allocated in GDI.  The expansion and/or
    //       color translation is done when the brush is put into the
    //       graphics memory cache.

    cjPattern      = psoPattern->cjBits;
    s3Brush.nSize += cjPattern;

    if (psoPattern->fjBitmap & BMF_TOPDOWN)
        s3Brush.lDeltaPattern = psoPattern->lDelta;
    else
        s3Brush.lDeltaPattern = -(psoPattern->lDelta);

    // If its a mono brush record the foreground and background colors.

    if (s3Brush.iBitmapFormat == BMF_1BPP)
    {
        if (pxlo->flXlate & XO_TABLE)
        {
            pulXlate = pxlo->pulXlate;
        }
        else
        {
            pulXlate = XLATEOBJ_piVector(pxlo);
        }

        s3Brush.ulForeColor = pulXlate[1];
        s3Brush.ulBackColor = pulXlate[0];
    }

    // Allocate some GDI storage for the Brush. !!!
    // Note: This should be moved up and the stack stuff should be !!!
    //       removed since we can now simply calculate the size of !!!
    //       the brush. !!!

    ps3Brush = (PS3BRUSH) BRUSHOBJ_pvAllocRbrush(pbo, s3Brush.nSize);

    // Assign all the static info we built on the stack. !!!
    // This should be removed. !!!

    *ps3Brush = s3Brush;

    // If there is an XlatObj, we may have to translate the
    // indicies.

    flXlate = 0;
    if (pxlo != NULL)
    {
        flXlate = pxlo->flXlate;

        if (flXlate & XO_TABLE)
        {
            pulXlate = pxlo->pulXlate;
        }
        else
        {
            pulXlate = XLATEOBJ_piVector(pxlo);
        }
    }

    // Note: We should be able to remove this if, since this is not !!!
    //       a time critcal spot in the code. !!!

    // We may have to invert the Y if it's not in a top-down format.
    // We have already adjusted the BrushDelta if this inversion is necessary.

    if (psoPattern->fjBitmap & BMF_TOPDOWN)
    {
        pbSrc  = psoPattern->pvBits;
        pbDest = ps3Brush->ajPattern;

        if ((flXlate & XO_TABLE) &&
            (psoPattern->iBitmapFormat == BMF_8BPP))
        {
            for (j = 0; j < cjPattern; j++)
            {
                pbDest[j] = (BYTE) pulXlate[pbSrc[j]];
            }
        }
        else
        {
            memcpy(ps3Brush->ajPattern, psoPattern->pvBits, cjPattern);
        }
    }
    else
    {

        cx = s3Brush.sizlPattern.cx;
        cy = s3Brush.sizlPattern.cy;

        pbSrc      = psoPattern->pvScan0;
        pbDest     = ps3Brush->ajPattern;
        lSrcDelta  = psoPattern->lDelta;
        lDestDelta = -lSrcDelta;

        for (i = 0; i < cy; i++)
        {
            // We may have to translate the indices.

            if ((flXlate & XO_TABLE) &&
                (psoPattern->iBitmapFormat == BMF_8BPP))
            {
                for (j = 0; j < cx; j++)
                {
                    pbDest[j] = (BYTE) pulXlate[pbSrc[j]];
                }
            }
            else
            {
                memcpy(pbDest, pbSrc, cx);
            }

            pbSrc  += lSrcDelta;
            pbDest += lDestDelta;

        }
    }

    return (TRUE);

}
