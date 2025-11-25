/******************************Module*Header*******************************\
* Module Name: paint.c
*
*
*
* Copyright (c) 1992 Microsoft Corporation
*
\**************************************************************************/

#include "driver.h"

/******************************Public*Data*********************************\
* MIX translation table
*
* Translates a mix 1-16, into an old style Rop 0-255.
*
\**************************************************************************/

BYTE gaMix[] =
{
    0xFF,  // R2_WHITE        - Allow rop = gaMix[mix & 0x0F]
    0x00,  // R2_BLACK
    0x05,  // R2_NOTMERGEPEN
    0x0A,  // R2_MASKNOTPEN
    0x0F,  // R2_NOTCOPYPEN
    0x50,  // R2_MASKPENNOT
    0x55,  // R2_NOT
    0x5A,  // R2_XORPEN
    0x5F,  // R2_NOTMASKPEN
    0xA0,  // R2_MASKPEN
    0xA5,  // R2_NOTXORPEN
    0xAA,  // R2_NOP
    0xAF,  // R2_MERGENOTPEN
    0xF0,  // R2_COPYPEN
    0xF5,  // R2_MERGEPENNOT
    0xFA,  // R2_MERGEPEN
    0xFF   // R2_WHITE
};

/*****************************************************************************
 * DrvPaint -
 ****************************************************************************/
BOOL DrvPaint(
    SURFOBJ  *pso,
    CLIPOBJ  *pco,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrushOrg,
    MIX      mix)
{
    INT     i;
    ROP4    rop4;
    BOOL    bRet,
            bMoreClips;

    PPDEV       ppdev;
    ENUMTRAPS8  traps8;
    DDAENUM     ddae;
    LONG        yRow;
    RECTL       rcl;
    PULONG      px;

    DISPDBG((3, "S3.DLL: DrvPaint - Entry\n"));

    ppdev = (PPDEV) pso->dhsurf;

    // Protect against a potentially NULL clip object.

    if (pco == NULL)
        return FALSE;

    rop4  = (gaMix[(mix >> 8) & 0x0F]) << 8;
    rop4 |= ((ULONG) gaMix[mix & 0x0F]);

    bRet = FALSE;

    switch (pco->iMode)
    {
        case TC_RECTANGLES:

            bRet = DrvBitBlt(pso,
                             (SURFOBJ *) NULL,
                             (SURFOBJ *) NULL,
                             pco,
                             (XLATEOBJ *) NULL,
                             &(pco->rclBounds),
                             (PPOINTL) NULL,
                             (PPOINTL) NULL,
                             pbo,
                             pptlBrushOrg,
                             rop4);
            break;

        case TC_TRAPEZOIDS:

            // Enumerate all the trapezodial clip objects.

            CLIPOBJ_cEnumStart(pco, TRUE, CT_TRAPEZOIDS, CD_ANY, 8);

            do
            {
                bMoreClips = CLIPOBJ_bEnum(pco, sizeof(traps8), (PULONG) &traps8);

                for (i = 0; i < (INT) traps8.c; i++)
                {
                    // Enumerate all the rectangles in this trapezoid.

                    if (!(DDAOBJ_bEnum(ppdev->pdda, &(traps8.atrap[i]),
                                       sizeof(ddae), (DDALIST *) &ddae,
                                       JD_ENUM_TRAPEZOID)))
                    {
                        continue;
                    }

                    do
                    {
                        px = (PULONG) (&ddae.axPairs[0]);

                        for (yRow = ddae.yTop; yRow < ddae.yBottom; yRow++)
                        {
                            if (*px < *(px + 1))
                            {
                                rcl.top    = yRow;
                                rcl.left   = *px;
                                rcl.bottom = yRow + 1;
                                rcl.right  = *(px + 1);

                                bRet = DrvBitBlt(pso,
                                                 (SURFOBJ *) NULL,
                                                 (SURFOBJ *) NULL,
                                                 ppdev->pcoDefault,
                                                 (XLATEOBJ *) NULL,
                                                 &rcl,
                                                 (PPOINTL) NULL,
                                                 (PPOINTL) NULL,
                                                 pbo,
                                                 pptlBrushOrg,
                                                 rop4);
                            }
                            px += 2;
                        }
                    } while (DDAOBJ_bEnum(ppdev->pdda, NULL,
                                            sizeof(ddae), (DDALIST *) &ddae,
                                            JD_ENUM_TRAPEZOID));
                }

            } while (bMoreClips);

            bRet = TRUE;
            break;

        default:

            DISPDBG((0, "S3.DLL!DrvPaint - Unhandled TC_xxxx\n"));

            break;
    }

    return (bRet);
}

