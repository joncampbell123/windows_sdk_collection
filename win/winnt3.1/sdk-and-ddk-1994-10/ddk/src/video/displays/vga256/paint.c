/******************************Module*Header*******************************\
* Module Name: paint.c
*
* Copyright (c) 1992 Microsoft Corporation
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

/******************************Public*Routine******************************\
* vTrgTrap(ppdev, ptrap, mix, iColor)
*
* Blit to a trapezoid.
*
\**************************************************************************/

VOID vTrgTrap
(
    PPDEV        ppdev,
    TRAPEZOID   *ptrap,
    MIX          mix,
    RBRUSH_COLOR rbc,
    POINTL      *pptlBrush,
    PFNFILL      pfnFill
)
{
    DDAENUM ddae;
    RECTL   rcl[MAX_DDA_RECTS], *prcl;
    ULONG   culrcl;
    LONG   *px;
    LONG    yRow;

    if (!DDAOBJ_bEnum(ppdev->pdda, (PVOID) ptrap, sizeof(ddae),
                      (DDALIST *) &ddae, JD_ENUM_TRAPEZOID))
        return;

    do {
        culrcl = 0;
        prcl = rcl;
        px = &ddae.axPairs[0];

        // Accumulate the rectangles for this trapezoid enumeration burst,
        // then send them to the solid filler as a group
        for (yRow = ddae.yTop; yRow < ddae.yBottom; yRow++) {
            if (*px < *(px+1)) {
                prcl->top    = yRow;
                prcl->left   = *px;
                prcl->bottom = yRow + 1;
                prcl++->right  = *(px+1);
                culrcl++;
            }

            px += 2;

        }

        // Draw the rectangles, if there are any
        if (culrcl > 0) {
            (*pfnFill)(ppdev, culrcl, rcl, mix, rbc, pptlBrush);
        }

    } while (DDAOBJ_bEnum(ppdev->pdda, (PVOID) NULL, sizeof(ddae),
            (DDALIST *) &ddae, JD_ENUM_TRAPEZOID));
}

/******************************Public*Routine******************************\
* bPaintRgn
*
* Paint the clipping region with the specified color and mode
*
\**************************************************************************/

BOOL bPaintRgn
(
    SURFOBJ      *pso,
    CLIPOBJ      *pco,
    MIX          mix,
    RBRUSH_COLOR rbc,
    POINTL       *pptlBrush,
    PFNFILL      pfnFill
)
{
    BBENUM      bben;
    BBENUMTRAP  bbent;
    PPDEV       ppdev;
    ULONG       iRT;
    BOOL        bMore;

// Get the target surface information.

    ppdev = (PPDEV) pso->dhsurf;

    switch(pco->iMode) {

        case TC_RECTANGLES:

            // Rectangular clipping can be handled without enumeration.
            // Note that trivial clipping is not possible, since the clipping
            // region defines the area to fill

            if (pco->iDComplexity == DC_RECT)
            {
                (*pfnFill)(ppdev, 1, &pco->rclBounds, mix, rbc, pptlBrush);

            } else {

                // Enumerate all the rectangles and draw them

                CLIPOBJ_cEnumStart(pco,TRUE,CT_RECTANGLES,CD_ANY,BB_RECT_LIMIT);

                do {
                    bMore = CLIPOBJ_bEnum(pco, sizeof(bben), (PVOID) &bben);

                    (*pfnFill)(ppdev, bben.c, &bben.arcl[0], mix, rbc, pptlBrush);

                } while (bMore);
            }

            return(TRUE);

        case TC_TRAPEZOIDS:

            // Enumerate all the trapezoids and draw them

            CLIPOBJ_cEnumStart(pco,TRUE,CT_TRAPEZOIDS,CD_ANY,BB_RECT_LIMIT);

            do {
                bMore = CLIPOBJ_bEnum(pco, sizeof(bbent), (PVOID) &bbent);

                for (iRT = 0; iRT < bbent.c; iRT++)
                    vTrgTrap(ppdev, &bbent.atrap[iRT], mix, rbc, pptlBrush, pfnFill);

            } while (bMore);

            return(TRUE);

        default:
            RIP("bPaintRgn: unhandled TC_xxx\n");
            return(FALSE);
    }
}


/**************************************************************************\
* DrvPaint
*
* Paint the clipping region with the specified brush
*
\**************************************************************************/

BOOL DrvPaint
(
    SURFOBJ  *pso,
    CLIPOBJ  *pco,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    MIX       mix
)
{
    ROP4         rop4;
    RBRUSH_COLOR rbc;
    PFNFILL      pfnFill;

    // If this adapter supports planar mode and the foreground and background
    // mixes are the same,
    // LATER or if there's no brush mask
    // then see if we can use the brush accelerators
    // LATER handle non-planar also

    if ((((PPDEV) pso->dhsurf)->fl & DRIVER_PLANAR_CAPABLE) &&
        ((mix & 0xFF) == ((mix >> 8) & 0xFF))) {

        switch (mix & 0xFF) {
            case 0:
                break;

            // vTrgBlt can only handle solid color fills where if the
            // destination is inverted, no other action is also required

            case R2_MASKNOTPEN:
            case R2_NOTCOPYPEN:
            case R2_XORPEN:
            case R2_MASKPEN:
            case R2_NOTXORPEN:
            case R2_MERGENOTPEN:
            case R2_COPYPEN:
            case R2_MERGEPEN:
            case R2_NOTMERGEPEN:
            case R2_MASKPENNOT:
            case R2_NOTMASKPEN:
            case R2_MERGEPENNOT:

                // vTrgBlt can only handle solid color fills

                if (pbo->iSolidColor != 0xffffffff)
                {
                    rbc.iSolidColor = pbo->iSolidColor;
                    pfnFill = vTrgBlt;
                }
                else
                {
                    rbc.prb = (RBRUSH*) pbo->pvRbrush;
                    if (rbc.prb == NULL)
                    {
                        rbc.prb = (RBRUSH*) BRUSHOBJ_pvGetRbrush(pbo);
                        if (rbc.prb == NULL)
                        {
                        // If we haven't realized the brush, punt the call:

                            break;
                        }
                    }
                    if (!(rbc.prb->fl & RBRUSH_BLACKWHITE) &&
                        ((mix & 0xff) != R2_COPYPEN))
                    {
                    // Only black/white brushes can handle ROPs other
                    // than COPYPEN:

                        break;
                    }

                    if (rbc.prb->fl & RBRUSH_NCOLOR)
                        pfnFill = vColorPat;
                    else
                        pfnFill = vMonoPat;
                }

                return(bPaintRgn(pso, pco, mix, rbc, pptlBrush, pfnFill));

            // Rops that are implicit solid colors

            case R2_NOT:
            case R2_WHITE:
            case R2_BLACK:

                // Brush color parameter doesn't matter for these rops

                return(bPaintRgn(pso, pco, mix, rbc, NULL, vTrgBlt));

            case R2_NOP:
                return(TRUE);

            default:
                break;
        }
    }

    rop4  = (gaMix[(mix >> 8) & 0x0F]) << 8;
    rop4 |= ((ULONG) gaMix[mix & 0x0F]);

    return(DrvBitBlt(
        pso,
        (SURFOBJ *) NULL,
        (SURFOBJ *) NULL,
        pco,
        (XLATEOBJ *) NULL,
        &pco->rclBounds,
        (POINTL *)  NULL,
        (POINTL *)  NULL,
        pbo,
        pptlBrush,
        rop4));
}

