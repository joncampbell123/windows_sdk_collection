/******************************Module*Header*******************************\
* Module Name: Stroke.c
*
* DrvStrokePath for S3 driver
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/

#include "driver.h"
#include "lines.h"

VOID (*gapfnStrip[])(PPDEV, STRIP*, LINESTATE*) = {
    vrlSolidHorizontal,
    vrlSolidVertical,
    vrlSolidDiagonalHorizontal,
    vrlSolidDiagonalVertical,

// Should be NUM_STRIP_DRAW_DIRECTIONS = 4 strip drawers in every group

    vssSolidHorizontal,
    vssSolidVertical,
    vssSolidDiagonalHorizontal,
    vssSolidDiagonalVertical,

// Should be NUM_STRIP_DRAW_STYLES = 8 strip drawers in total for doing
// solid lines, and the same number for non-solid lines:

    vStripStyledHorizontal,
    vStripStyledVertical,
    vStripStyledVertical,       // Diagonal goes here
    vStripStyledVertical,       // Diagonal goes here

    vStripStyledHorizontal,
    vStripStyledVertical,
    vStripStyledVertical,       // Diagonal goes here
    vStripStyledVertical,       // Diagonal goes here
};

// Style array for alternate style (alternates one pixel on, one pixel off):

STYLEPOS gaspAlternateStyle[] = { 1 };

/******************************Public*Routine******************************\
* BOOL DrvStrokePath(pso, ppo, pco, pxo, pbo, pptlBrushOrg, pla, mix)
*
* Strokes the path.
*
\**************************************************************************/

BOOL DrvStrokePath(
    SURFOBJ*   pso,
    PATHOBJ*   ppo,
    CLIPOBJ*   pco,
    XFORMOBJ*  pxo,
    BRUSHOBJ*  pbo,
    POINTL*    pptlBrushOrg,
    LINEATTRS* pla,
    MIX        mix)
{
    STYLEPOS  aspLtoR[STYLE_MAX_COUNT];
    STYLEPOS  aspRtoL[STYLE_MAX_COUNT];
    LINESTATE ls;
    PFNSTRIP* apfn;
    FLONG     fl;
    PPDEV     ppdev;
    RECTL     arclClip[4];                  // For rectangular clipping

    UNREFERENCED_PARAMETER(pxo);
    UNREFERENCED_PARAMETER(pptlBrushOrg);

    ppdev = (PPDEV) pso->dhsurf;

// Get the device ready:

    vSetStrips(ppdev, pla, pbo->iSolidColor, mix);

// x86 has special case ASM code for accelerating solid lines:

#if defined(_X86_) || defined(i386)

    if ((pla->pstyle == NULL) && !(pla->fl & LA_ALTERNATE))
    {
    // We can accelerate solid lines:

        if (pco->iDComplexity == DC_TRIVIAL)
        {
            vFastLine(ppdev, ppo, NULL, &gapfnStrip[0], 0);

            return(TRUE);
        }
        else if (pco->iDComplexity == DC_RECT)
        {
            RECTL  rclBounds;
            RECTFX rcfxBounds;

        // We have to be sure that we don't overflow the hardware registers
        // for current position, line length, or DDA terms.  We check
        // here to make sure that the current position and line length
        // values won't overflow (for integer lines, this check is
        // sufficient to ensure that the DDA terms won't overflow; for GIQ
        // lines, we specifically check on every line in vFastLines that we
        // don't overflow).

            PATHOBJ_vGetBounds(ppo, &rcfxBounds);

            if (rcfxBounds.xLeft   >= (MIN_INTEGER_BOUND * F) &&
                rcfxBounds.yTop    >= (MIN_INTEGER_BOUND * F) &&
                rcfxBounds.xRight  <= (MAX_INTEGER_BOUND * F) &&
                rcfxBounds.yBottom <= (MAX_INTEGER_BOUND * F))
            {
                arclClip[0]        =  pco->rclBounds;

            // FL_FLIP_D:

                arclClip[1].top    =  pco->rclBounds.left;
                arclClip[1].left   =  pco->rclBounds.top;
                arclClip[1].bottom =  pco->rclBounds.right;
                arclClip[1].right  =  pco->rclBounds.bottom;

            // FL_FLIP_V:

                arclClip[2].top    = -pco->rclBounds.bottom + 1;
                arclClip[2].left   =  pco->rclBounds.left;
                arclClip[2].bottom = -pco->rclBounds.top + 1;
                arclClip[2].right  =  pco->rclBounds.right;

            // FL_FLIP_V | FL_FLIP_D:

                arclClip[3].top    =  pco->rclBounds.left;
                arclClip[3].left   = -pco->rclBounds.bottom + 1;
                arclClip[3].bottom =  pco->rclBounds.right;
                arclClip[3].right  = -pco->rclBounds.top + 1;

                rclBounds.left   = pco->rclBounds.left;
                rclBounds.top    = pco->rclBounds.top;
                rclBounds.right  = pco->rclBounds.right;
                rclBounds.bottom = pco->rclBounds.bottom;

                vSetS3ClipRect(ppdev, &rclBounds);

                vFastLine(ppdev, ppo, &arclClip[0], &gapfnStrip[0],
                          FL_SIMPLE_CLIP);

                vResetS3Clipping(ppdev);
                return(TRUE);
            }
        }
    }

#endif

    fl = 0;

// Look after styling initialization:

    if (pla->fl & LA_ALTERNATE)
    {
        ls.cStyle      = 1;
        ls.spTotal     = 1;
        ls.spTotal2    = 2;
        ls.spRemaining = 1;
        ls.aspRtoL     = &gaspAlternateStyle[0];
        ls.aspLtoR     = &gaspAlternateStyle[0];
        ls.spNext      = HIWORD(pla->elStyleState.l);
        ls.xyDensity   = 1;
        fl            |= FL_ARBITRARYSTYLED;
        ls.ulStartMask = 0L;
    }
    else if (pla->pstyle != (FLOAT_LONG*) NULL)
    {
        PFLOAT_LONG pstyle;
        STYLEPOS*   pspDown;
        STYLEPOS*   pspUp;

        pstyle = &pla->pstyle[pla->cstyle];

        ls.xyDensity = STYLE_DENSITY;
        ls.spTotal   = 0;
        while (pstyle-- > pla->pstyle)
        {
            ls.spTotal += pstyle->l;
        }
        ls.spTotal *= STYLE_DENSITY;
        ls.spTotal2 = 2 * ls.spTotal;

    // Compute starting style position (this is guaranteed not to overflow):

        ls.spNext = HIWORD(pla->elStyleState.l) * STYLE_DENSITY +
                    LOWORD(pla->elStyleState.l);

        fl        |= FL_ARBITRARYSTYLED;
        ls.cStyle  = pla->cstyle;
        ls.aspRtoL = aspRtoL;
        ls.aspLtoR = aspLtoR;

        if (pla->fl & LA_STARTGAP)
            ls.ulStartMask = 0xffffffffL;
        else
            ls.ulStartMask = 0L;

        pstyle  = pla->pstyle;
        pspDown = &ls.aspRtoL[ls.cStyle - 1];
        pspUp   = &ls.aspLtoR[0];

        while (pspDown >= &ls.aspRtoL[0])
        {
            *pspDown = pstyle->l * STYLE_DENSITY;
            *pspUp   = *pspDown;

            pspUp++;
            pspDown--;
            pstyle++;
        }
    }

    apfn = &gapfnStrip[NUM_STRIP_DRAW_STYLES *
                            ((fl & FL_STYLE_MASK) >> FL_STYLE_SHIFT)];

// Set up to enumerate the path:

#if defined(_X86_) || defined(i386)

// x86 ASM bLines supports DC_RECT clipping:

    if (pco->iDComplexity != DC_COMPLEX)

#else

// Non-x86 ASM bLines don't support DC_RECT clipping:

    if (pco->iDComplexity == DC_TRIVIAL)

#endif

    {
        PATHDATA  pd;
        RECTL*    prclClip = (RECTL*) NULL;
        BOOL      bMore;
        ULONG     cptfx;
        POINTFIX  ptfxStartFigure;
        POINTFIX  ptfxLast;
        POINTFIX* pptfxFirst;
        POINTFIX* pptfxBuf;

#if defined(_X86_) || defined(i386)

        if (pco->iDComplexity == DC_RECT)
        {
            fl |= FL_SIMPLE_CLIP;

            arclClip[0]        =  pco->rclBounds;

        // FL_FLIP_D:

            arclClip[1].top    =  pco->rclBounds.left;
            arclClip[1].left   =  pco->rclBounds.top;
            arclClip[1].bottom =  pco->rclBounds.right;
            arclClip[1].right  =  pco->rclBounds.bottom;

        // FL_FLIP_V:

            arclClip[2].top    = -pco->rclBounds.bottom + 1;
            arclClip[2].left   =  pco->rclBounds.left;
            arclClip[2].bottom = -pco->rclBounds.top + 1;
            arclClip[2].right  =  pco->rclBounds.right;

        // FL_FLIP_V | FL_FLIP_D:

            arclClip[3].top    =  pco->rclBounds.left;
            arclClip[3].left   = -pco->rclBounds.bottom + 1;
            arclClip[3].bottom =  pco->rclBounds.right;
            arclClip[3].right  = -pco->rclBounds.top + 1;

            prclClip = arclClip;
        }

#endif

        pd.flags = 0;

        do {
            bMore = PATHOBJ_bEnum(ppo, &pd);

            cptfx = pd.count;
            if (cptfx == 0)
            {
                break;
            }

            if (pd.flags & PD_BEGINSUBPATH)
            {
                ptfxStartFigure  = *pd.pptfx;
                pptfxFirst       = pd.pptfx;
                pptfxBuf         = pd.pptfx + 1;
                cptfx--;
            }
            else
            {
                pptfxFirst       = &ptfxLast;
                pptfxBuf         = pd.pptfx;
            }

            if (pd.flags & PD_RESETSTYLE)
                ls.spNext = 0;

            if (cptfx > 0)
            {
                if (!bLines(ppdev,
                            pptfxFirst,
                            pptfxBuf,
                            (RUN*) NULL,
                            cptfx,
                            &ls,
                            prclClip,
                            apfn,
                            fl))
                    return(FALSE);
            }

            ptfxLast = pd.pptfx[pd.count - 1];

            if (pd.flags & PD_CLOSEFIGURE)
            {
                if (!bLines(ppdev,
                            &ptfxLast,
                            &ptfxStartFigure,
                            (RUN*) NULL,
                            1,
                            &ls,
                            prclClip,
                            apfn,
                            fl))
                    return(FALSE);
            }
        } while (bMore);

        if (fl & FL_STYLED)
        {
        // Save the style state:

            ULONG ulHigh;
            ULONG ulLow;

        // Masked styles don't normalize the style state.  It's a good
        // thing to do, so let's do it now:

            if ((ULONG) ls.spNext >= (ULONG) ls.spTotal2)
                ls.spNext = (ULONG) ls.spNext % (ULONG) ls.spTotal2;

            ulHigh = ls.spNext / ls.xyDensity;
            ulLow  = ls.spNext % ls.xyDensity;

            pla->elStyleState.l = MAKELONG(ulLow, ulHigh);
        }
    }
    else
    {
    // Local state for path enumeration:

        BOOL bMore;
        union {
            BYTE     aj[offsetof(CLIPLINE, arun) + RUN_MAX * sizeof(RUN)];
            CLIPLINE cl;
        } cl;

        fl |= FL_COMPLEX_CLIP;

    // We use the clip object when non-simple clipping is involved:

        PATHOBJ_vEnumStartClipLines(ppo, pco, pso, pla);

        do {
            bMore = PATHOBJ_bEnumClipLines(ppo, sizeof(cl), &cl.cl);
            if (cl.cl.c != 0)
            {
                if (fl & FL_STYLED)
                {
                    ls.spComplex = HIWORD(cl.cl.lStyleState) * ls.xyDensity
                                 + LOWORD(cl.cl.lStyleState);
                }
                if (!bLines(ppdev,
                            &cl.cl.ptfxA,
                            &cl.cl.ptfxB,
                            &cl.cl.arun[0],
                            cl.cl.c,
                            &ls,
                            (RECTL*) NULL,
                            apfn,
                            fl))
                    return(FALSE);
            }
        } while (bMore);
    }

    return(TRUE);
}



