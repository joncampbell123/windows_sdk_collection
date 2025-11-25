/******************************Module*Header*******************************\
* Module Name: TextOut.c
*
* S3 Text accelerations
*
* Copyright (c) 1992 Microsoft Corporation
*
\**************************************************************************/

#include "driver.h"
#include "memory.h"

// Part of the fix to limit the amount of resources allocated for fonts

#define MAX_GLYPHS_TO_ALLOC 256

// number of bytes in the glyph bitmap scanline

#define CJ_SCAN(cx) (((cx) + 7) >> 3)

#define TRIVIAL_ACCEPT      0x00000001
#define MONO_SPACED_FONT    0x00000002
#define MONO_SIZE_VALID     0x00000004
#define MONO_FIRST_TIME     0x00000008

PCACHEDFONT pCachedFontsRoot;             // Cached Fonts list root.

WORD   iPlaneBits[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

BOOL bOpaqueRect(
    PPDEV ppdev,
    RECTL *prclOpaque,
    RECTL *prclBounds,
    BRUSHOBJ *pboOpaque);

BOOL bSetS3TextColorAndMix(
    PPDEV ppdev,
    MIX mix,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque);

VOID vBlowCache(PPDEV ppdev);

VOID vInitGlyphAlloc(PPDEV ppdev);

BOOL bAllocGlyphMemory(PPDEV ppdev,
                       PSIZEL psizlGlyph,
                       PXYZPOINTL pxyzGlyph);

PCACHEDGLYPH pCacheFont(PPDEV ppdev,
                        STROBJ *pstro,
                        FONTOBJ  *pfo,
                        PCACHEDFONT *ppCachedFont);

BOOL bAllocateGlyph(PPDEV ppdev,
                    HGLYPH hg,
                    GLYPHBITS *pgb,
                    PCACHEDGLYPH pcg);

BOOL bHandleNonCachedFonts(SURFOBJ  *pso,
                           STROBJ   *pstro,
                           FONTOBJ  *pfo,
                           RECTL    *prclClip,
                           RECTL    *prclExtra,
                           RECTL    *prclOpaque,
                           BRUSHOBJ *pboFore,
                           BRUSHOBJ *pboOpaque,
                           POINTL   *pptlOrg,
                           MIX      mix);

BOOL bHandleCachedFonts(SURFOBJ  *pso,
                        STROBJ   *pstro,
                        RECTL    *prclClip,
                        FONTOBJ  *pfo,
                        RECTL    *prclOpaque,
                        BRUSHOBJ *pboFore,
                        BRUSHOBJ *pboOpaque,
                        POINTL   *pptlOrg,
                        MIX      mix);

#if DBG

ULONG   nGlyphs, nOverlaps;

#define TFIFOWAIT(level)    nGlyphs++;                         \
                            if (INPW(GP_STAT) & level) {         \
                                nOverlaps++;                   \
                                while (INPW(GP_STAT) & level); \
                             }
#else

#define TFIFOWAIT(level) FIFOWAIT(level)

#endif


/****************************************************************************
 * DrvDestroyFont
 ***************************************************************************/
VOID DrvDestroyFont(FONTOBJ *pfo)
{
    PCACHEDFONT pCachedFont, pcfLast;
    PCACHEDGLYPH pcg, pcgNext;
    INT nGlyphs, i;

    DISPDBG((1, "S3.DLL!DrvDestroyFont - Entry\n"));

    if (((pCachedFont = ((PCACHEDFONT) pfo->pvConsumer)) != NULL))
    {
        if (pfo->iUniq == pCachedFont->iUniq)
        {
            // We have found our font.

            DISPDBG((1, "S3.DLL: Destroying font: pfo->iUniq: %x\n",
                         pfo->iUniq));

            // First free any nodes in the collision list.

            nGlyphs = pCachedFont->cGlyphs+1;
            for (i = 0; i < nGlyphs; i++)
            {
                // get a pointer to this glyph node.

                pcg = &(pCachedFont->pCachedGlyphs[i]);

                // get a pointer to this glyphs collision list

                pcg = pcg->pcgCollisionLink;
                for (; pcg != NULL; pcg = pcgNext)
                {
                    pcgNext = pcg->pcgCollisionLink;
                    LocalFree(pcg);
                }
            }

            // Now free the cached glyph array

            LocalFree(pCachedFont->pCachedGlyphs);

            // Now remove the font node from the list of fonts
            // and free it.

            pcfLast = (PCACHEDFONT) &pCachedFontsRoot;
            for (pCachedFont = pCachedFontsRoot;
                 pCachedFont != NULL;
                 pCachedFont = pCachedFont->pcfNext)
            {
                if (pCachedFont->iUniq == pfo->iUniq)
                {
                    pcfLast->pcfNext = pCachedFont->pcfNext;
                    LocalFree(pCachedFont);
                    break;
                }
                pcfLast = pCachedFont;
            }

            if (pCachedFont == NULL)
            {
                RIP("S3.DLL!DrvDestroyFont - pCachedFont not found\n");
            }
        }
        else
        {
            RIP("S3.DLL!DrvDestroyFont - pfo->pvConsumer error\n");
        }
    }

    // In all cases we want to zero out the pvConsumer field.

    pfo->pvConsumer = NULL;

}


/****************************************************************************
 * DrvTextOut
 ***************************************************************************/
BOOL DrvTextOut(
    SURFOBJ  *pso,
    STROBJ   *pstro,
    FONTOBJ  *pfo,
    CLIPOBJ  *pco,
    RECTL    *prclExtra,
    RECTL    *prclOpaque,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque,
    POINTL   *pptlOrg,
    MIX      mix)
{
    BOOL        b, bMore;
    UINT        i;
    ENUMRECTS8  EnumRects8;
    PPDEV       ppdev;

    DISPDBG((3, "S3.DLL: DrvTextOut - Entry\n"));

    // Pickup the ppdev.

    ppdev = (PPDEV) pso->dhpdev;

    // Protect the code path from a potentially NULL clip object
    // This also gives a maximum rclBounds which we use later:

    if (pco == NULL || pco->iDComplexity == DC_TRIVIAL)
    {
        pco = ppdev->pcoDefault;
    }

    // Determine if we can cache this string.
    // This is done by checking the size of glyph.

    b = TRUE;

    if ((pfo->cxMax > GLYPH_CACHE_CX) ||
        ((pstro->rclBkGround.bottom - pstro->rclBkGround.top) > GLYPH_CACHE_CY))
    {
        b = FALSE;
    }

    // If the glyphs in this string will fit in the font cache
    // then try to render it as a cached font.

    if (b == TRUE)
    {
        // Take care of the clipping.

        if (pco->iDComplexity != DC_COMPLEX)
        {
            b = bHandleCachedFonts(pso, pstro, &(pco->rclBounds), pfo,
                                   prclOpaque, pboFore,
                                   pboOpaque, pptlOrg, mix);

        }
        else
        {
            CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

            do
            {
                bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8),
                        (PULONG)&EnumRects8);

                for (i = 0; i < EnumRects8.c; i++)
                {
                    b = bHandleCachedFonts(pso, pstro, &(EnumRects8.arcl[i]),
                            pfo, prclOpaque, pboFore, pboOpaque, pptlOrg, mix);
                    if (!b)
                    {
                        // If we failed, stop immediately, so we can fall back
                        // to bHandleNonCachedFonts. Otherwise, if we just blew
                        // the cache, the next call in this loop will succeed,
                        // and we'll never redo this rectangle properly. Note
                        // that this is very inefficient, forcing the whole
                        // rest of the call to go through
                        // bHandleNonCachedFonts
                        break;
                    }
                }

            } while (bMore & b);
        }
    }

    // If something went wrong with rendering the string as a cached
    // font then render it as a large font.

    if (b == FALSE)
    {
        if (pco->iDComplexity != DC_COMPLEX)
        {
            b = bHandleNonCachedFonts(pso, pstro, pfo, &(pco->rclBounds),
                    prclExtra, prclOpaque, pboFore, pboOpaque, pptlOrg, mix);

        }
        else
        {
            CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

            do
            {
                bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8),
                        (PULONG)&EnumRects8);

                for (i = 0; i < EnumRects8.c; i++)
                {
                    b = bHandleNonCachedFonts(pso, pstro, pfo,
                            &(EnumRects8.arcl[i]), prclExtra, prclOpaque,
                            pboFore, pboOpaque, pptlOrg, mix);
                }

            } while (bMore);
        }

        vResetS3Clipping(ppdev);
    }

    return (b);
}


/****************************************************************************
 * bHandleCachedFonts
 ***************************************************************************/
BOOL bHandleCachedFonts(
    SURFOBJ  *pso,
    STROBJ   *pstro,
    RECTL    *prclClip,
    FONTOBJ  *pfo,
    RECTL    *prclOpaque,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque,
    POINTL   *pptlOrg,
    MIX      mix)
{
    BOOL        b, bMoreGlyphs, bFound;
    ULONG       iGlyph, cGlyphs;
    POINTL      ptl;
    GLYPHPOS    *pgp;
    INT         ihGlyph, cxGlyph, cyGlyph;
    PCACHEDGLYPH pCachedGlyphs, pcg, pcgNew;
    WORD        Cmd;
    XYZPOINTL   xyzGlyph;
    PCACHEDFONT pCachedFont;
    PPDEV       ppdev;
    INT         i, culRcl;
    ULONG       flAccel;
    RECTL       arclTmp[4];
    ULONG       ulCharInc;
    ULONG       yMonoStart, xMonoPosition;
    POINTL      ptlSrc;
    RECTL       rclGlyph;
    HGLYPH      hg;
    GLYPHBITS   *pgb;
    ULONG       fl = 0;
    BYTE        jBackgroundRop;

    DISPDBG((4, "S3.DLL:bHandleCachedFonts - Entry\n"));

    ppdev = (PPDEV) pso->dhpdev;

    //
    // If we have seen this font before then pvConsumer will be non-NULL.
    //

    if (((pCachedFont = ((PCACHEDFONT) pfo->pvConsumer)) != NULL))
    {
        pCachedGlyphs = pCachedFont->pCachedGlyphs;

        if (pfo->iUniq != pCachedFont->iUniq)
        {
            DISPDBG((1, "S3.DLL: pfo->iUniq: %x, pCachedFont->iUniq: %x\n",
                                 pfo->iUniq, pCachedFont->iUniq));
            return (FALSE);
        }
    }

    else
    {
        DISPDBG((2, "S3.DLL!bHandleCachedFonts - Caching Font: %d\n", pfo->iUniq));

        pCachedGlyphs = pCacheFont(ppdev, pstro, pfo, &pCachedFont);

        if (pCachedGlyphs == NULL)
        {
            DISPDBG((1, "S3.DLL!bHandleCachedFonts - pCacheFont failed once\n"));

            vBlowCache(ppdev);

            pCachedGlyphs = pCacheFont(ppdev, pstro, pfo, &pCachedFont);
            if (pCachedGlyphs == NULL)
            {
                DISPDBG((1, "S3.DLL!bHandleCachedFonts - pCacheFont failed twice\n"));
                return(FALSE);
            }
        }

        (PCACHEDFONT)(pfo->pvConsumer) = pCachedFont;
    }

    // If this string has a Zero Bearing and the string object's
    // opaque rectangle is the same size as the opaque rectangle then
    // and opaque mode is requested, then lay down the glyphs in
    // opaque mode.

    if (((flAccel = pstro->flAccel) & SO_ZERO_BEARINGS) &&
        ((flAccel & (SO_HORIZONTAL | SO_VERTICAL | SO_REVERSED)) == SO_HORIZONTAL) &&
        (flAccel & SO_CHAR_INC_EQUAL_BM_BASE) &&
        (prclOpaque != NULL))
    {
        // If the Opaque rect and the string rect match then
        // were done.  If not then we have to fill in the strips
        // (top, bottom, left, and right) around the text.

        culRcl = 0;

        // Top fragment

        if (pstro->rclBkGround.top > prclOpaque->top)
        {
            arclTmp[culRcl].top      = prclOpaque->top;
            arclTmp[culRcl].left     = prclOpaque->left;
            arclTmp[culRcl].right    = prclOpaque->right;
            arclTmp[culRcl++].bottom = pstro->rclBkGround.top;
        }

        // Left fragment

        if (pstro->rclBkGround.left > prclOpaque->left)
        {
            arclTmp[culRcl].top      = pstro->rclBkGround.top;
            arclTmp[culRcl].left     = prclOpaque->left;
            arclTmp[culRcl].right    = pstro->rclBkGround.left;
            arclTmp[culRcl++].bottom = pstro->rclBkGround.bottom;
        }

        // Right fragment

        if (pstro->rclBkGround.right < prclOpaque->right)
        {
            arclTmp[culRcl].top      = pstro->rclBkGround.top;
            arclTmp[culRcl].right    = prclOpaque->right;
            arclTmp[culRcl].left     = pstro->rclBkGround.right;
            arclTmp[culRcl++].bottom = pstro->rclBkGround.bottom;
        }

        // Bottom fragment

        if (pstro->rclBkGround.bottom < prclOpaque->bottom)
        {
            arclTmp[culRcl].bottom = prclOpaque->bottom;
            arclTmp[culRcl].left   = prclOpaque->left;
            arclTmp[culRcl].right  = prclOpaque->right;
            arclTmp[culRcl++].top  = pstro->rclBkGround.bottom;
        }

        // Fill any fringe rectangles we found

        for (i = 0; i < culRcl; i++)
        {
            bOpaqueRect(ppdev, &(arclTmp[i]), prclClip, pboOpaque);
        }

        // Set the mix mode for opaque text.

        mix = (mix & 0x0F) | (R2_COPYPEN << 8);
        b = bSetS3TextColorAndMix(ppdev, mix, pboFore, pboOpaque);
        if (b == FALSE)
            return (b);

    }
    else
    {
        // Take care of any opaque rectangles.

        if (prclOpaque != NULL)
        {
            bOpaqueRect(ppdev, prclOpaque, prclClip, pboOpaque);
        }

        jBackgroundRop = R2_NOP;

        // Take care of the glyph attributes, color and mix.

        mix = (mix & 0x0F) | (jBackgroundRop << 8);
        b = bSetS3TextColorAndMix(ppdev, mix, pboFore, pboOpaque);
        if (b == FALSE)
            return (b);
    }

    // Test for a trivial accept of the string rect.

    if (bTrivialAcceptTest(&(pstro->rclBkGround), prclClip))
        fl |= TRIVIAL_ACCEPT;

    // Test and setup for a mono-spaced font.

    if ((ulCharInc = pstro->ulCharInc) != 0)
        fl |= MONO_SPACED_FONT;

    // Set the S3 command.

    Cmd  = BITBLT         | DRAW               | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS| DRAWING_DIR_TBLRXM;

    // This does not change in the inner loop so it is now out of it.

    FIFOWAIT(FIFO_1_EMPTY);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

    // Get the Glyph Handles.

    if ((pstro->pgp) == NULL)
        STROBJ_vEnumStart(pstro);

    do
    {
        if (pstro->pgp == NULL)
        {
            bMoreGlyphs = STROBJ_bEnum(pstro, &cGlyphs, &pgp);

        }
        else
        {
            pgp = pstro->pgp;
            cGlyphs = pstro->cGlyphs;
            bMoreGlyphs = FALSE;
        }

        // For mono space fonts this is non-zero.

        if (fl & MONO_SPACED_FONT)
        {
            xMonoPosition = pgp[0].ptl.x;
            yMonoStart    = pgp[0].ptl.y;
        }

        for (iGlyph = 0; iGlyph < cGlyphs; iGlyph++)
        {
            // Get the Glyph Handle.
            // If there was a hash table hit for the glygph
            // then were "golden", if not then we have to search
            // the collision list.

            ihGlyph = pgp[iGlyph].hg & pCachedFont->cGlyphs;

            pcg = &(pCachedGlyphs[ihGlyph]);

            if ((hg = pgp[iGlyph].hg) != pcg->hg)
            {
                if (!(pcg->fl & VALID_GLYPH))
                {
                    // Allocate a place in the cache.

                    pgb = pgp[iGlyph].pgdf->pgb;
                    b = bAllocateGlyph(ppdev, hg, pgb, pcg);
                    if (b == FALSE)
                    {
                        vBlowCache(ppdev);
                        return (FALSE);
                    }

                    b = bSetS3TextColorAndMix(ppdev, mix, pboFore, pboOpaque);
                    if (b == FALSE)
                        return (FALSE);

                    FIFOWAIT(FIFO_1_EMPTY);
                    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

                }
                else
                {
                    // Search the collision list.

                    DISPDBG((1, "S3.DLL!bHandleCachedFonts - searching collision list\n"));

                    bFound = FALSE;
                    while (pcg->pcgCollisionLink != END_COLLISIONS)
                    {
                        pcg = pcg->pcgCollisionLink;

                        if (pcg->hg == pgp[iGlyph].hg)
                        {
                            bFound = TRUE;
                            break;
                        }
                    }

                    if (!bFound)
                    {
                        // Allocate a new font glyph node.

                        pcgNew = (PCACHEDGLYPH) LocalAlloc(LPTR, sizeof(CACHEDGLYPH));
                        if (pcgNew == NULL)
                        {
                            DISPDBG((1, "S3.DLL!bHandleCachedFont - Local Alloc (pcgNew) failed\n"));
                            return (FALSE);
                        }

                        // Connect the end of the collision list to the new
                        // glyph node.

                        pcg->pcgCollisionLink = pcgNew;

                        // Set up the pointer to the node where going to init.

                        pcg = pcgNew;

                        pgb = pgp[iGlyph].pgdf->pgb;
                        b = bAllocateGlyph(ppdev, hg, pgb, pcg);
                        if (b == FALSE)
                        {
                            vBlowCache(ppdev);
                            return (FALSE);
                        }

                        b = bSetS3TextColorAndMix(ppdev, mix, pboFore, pboOpaque);
                        if (b == FALSE)
                            return (FALSE);

                        FIFOWAIT(FIFO_1_EMPTY);
                        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

                    }
                }
            }

            // Adjust the placement of the glyph.
            // And if this is a mono-spaced font set the blt height & width.

            if (fl & MONO_SPACED_FONT)
            {
                ptl.x = xMonoPosition + pcg->ptlOrigin.x;
                ptl.y = yMonoStart + pcg->ptlOrigin.y;
                xMonoPosition += ulCharInc;

                if ((!(fl & MONO_SIZE_VALID) && (fl & TRIVIAL_ACCEPT)) ||
                    (!(fl & MONO_FIRST_TIME)))
                {
                    fl |= MONO_SIZE_VALID;

                    TFIFOWAIT(FIFO_2_EMPTY);
                    OUTPW(RECT_WIDTH, pcg->sizlBitmap.cx - 1);
                    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (pcg->sizlBitmap.cy - 1)));
                }

            }
            else
            {
                ptl.x = pgp[iGlyph].ptl.x + pcg->ptlOrigin.x;
                ptl.y = pgp[iGlyph].ptl.y + pcg->ptlOrigin.y;
            }

            if (fl & TRIVIAL_ACCEPT)
            {
                // Blit the glyph

                if (!(fl & MONO_SPACED_FONT))
                {
                    TFIFOWAIT(FIFO_2_EMPTY);
                    OUTPW(RECT_WIDTH, pcg->sizlBitmap.cx - 1);
                    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (pcg->sizlBitmap.cy - 1)));
                }

                TFIFOWAIT(FIFO_6_EMPTY);
                TEST_AND_SET_RD_MASK(LOWORD(pcg->xyzGlyph.z));

                OUTPW(CUR_X, pcg->xyzGlyph.x);
                OUTPW(CUR_Y, pcg->xyzGlyph.y);
                OUTPW(DEST_X, ptl.x);
                OUTPW(DEST_Y, ptl.y);
                OUTPW(CMD, Cmd);

            }
            else
            {
                xyzGlyph = pcg->xyzGlyph;

                // Clip each character,

                rclGlyph.left   = ptl.x;
                rclGlyph.top    = ptl.y;
                rclGlyph.right  = ptl.x + pcg->sizlBitmap.cx;
                rclGlyph.bottom = ptl.y + pcg->sizlBitmap.cy;

                if (bIntersectTest(&rclGlyph, prclClip))
                {
                    rclGlyph.left   = max (rclGlyph.left,   prclClip->left);
                    rclGlyph.top    = max (rclGlyph.top,    prclClip->top);
                    rclGlyph.right  = min (rclGlyph.right,  prclClip->right);
                    rclGlyph.bottom = min (rclGlyph.bottom, prclClip->bottom);

                    cxGlyph  = (rclGlyph.right - rclGlyph.left) -  1;
                    cyGlyph  = (rclGlyph.bottom - rclGlyph.top) - 1;

                    if (cxGlyph < 0 || cyGlyph < 0)
                        continue;

                    ptlSrc.x = xyzGlyph.x + (rclGlyph.left - ptl.x);
                    ptlSrc.y = xyzGlyph.y + (rclGlyph.top - ptl.y);

                    // Blit the glyph

                    TFIFOWAIT(FIFO_8_EMPTY);
                    TEST_AND_SET_RD_MASK(LOWORD(pcg->xyzGlyph.z));

                    OUTPW(RECT_WIDTH, cxGlyph);
                    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyGlyph));
                    OUTPW(CUR_X, ptlSrc.x);
                    OUTPW(CUR_Y, ptlSrc.y);
                    OUTPW(DEST_X, rclGlyph.left);
                    OUTPW(DEST_Y, rclGlyph.top);
                    OUTPW(CMD, Cmd);

                }
                else
                {
                    continue;
                }
            }
        }

    } while(bMoreGlyphs);

    return (TRUE);

}


/****************************************************************************
 * bHandleNonCachedFonts
 ***************************************************************************/
BOOL bHandleNonCachedFonts(
    SURFOBJ  *pso,
    STROBJ   *pstro,
    FONTOBJ  *pfo,
    RECTL    *prclClip,
    RECTL    *prclExtra,
    RECTL    *prclOpaque,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque,
    POINTL   *pptlOrg,
    MIX      mix)
{
    BOOL        b,
                bMoreGlyphs;
    ULONG       iGlyph,
                cGlyphs;
    GLYPHBITS   *pgb;
    POINTL      ptl;
    GLYPHPOS    *pgp;
    LONG        cyGlyph,
                cjGlyph,
                GlyphBmPitchInBytes;
    WORD        S3Cmd;
    INT         i, culRcl;
    ULONG       flAccel;
    RECTL       arclTmp[4];

    ULONG       ulCharInc;
    ULONG       yMonoStart, xMonoPosition;
    PPDEV       ppdev;
    BYTE        jBackgroundRop;
    RECTL       rclClip;

    ULONG       fl = 0;

    DISPDBG((4, "S3.DLL!bHandleNonCachedFonts\n"));

    ppdev = (PPDEV) pso->dhpdev;

    // We need to reset the clipping so any opaque rectangles are
    // rendered correctly.

    vResetS3Clipping(ppdev);

    // If this string has a Zero Bearing and the string object's
    // opaque rectangle is the same size as the opaque rectangle then
    // and opaque mode is requested, then lay down the glyphs in
    // opaque mode.

    if (((flAccel = pstro->flAccel) & SO_ZERO_BEARINGS) &&
        ((flAccel & (SO_HORIZONTAL | SO_VERTICAL | SO_REVERSED)) == SO_HORIZONTAL) &&
        (flAccel & SO_CHAR_INC_EQUAL_BM_BASE) &&
        (prclOpaque != NULL))
    {
        // If the Opaque rect and the string rect match then
        // were done.  If not then we have to fill in the strips
        // (top, bottom, left, and right) around the text.

        culRcl = 0;

        // Top fragment

        if (pstro->rclBkGround.top > prclOpaque->top)
        {
            arclTmp[culRcl].top      = prclOpaque->top;
            arclTmp[culRcl].left     = prclOpaque->left;
            arclTmp[culRcl].right    = prclOpaque->right;
            arclTmp[culRcl++].bottom = pstro->rclBkGround.top;
        }

        // Left fragment

        if (pstro->rclBkGround.left > prclOpaque->left)
        {
            arclTmp[culRcl].top      = pstro->rclBkGround.top;
            arclTmp[culRcl].left     = prclOpaque->left;
            arclTmp[culRcl].right    = pstro->rclBkGround.left;
            arclTmp[culRcl++].bottom = pstro->rclBkGround.bottom;
        }

        // Right fragment

        if (pstro->rclBkGround.right < prclOpaque->right)
        {
            arclTmp[culRcl].top      = pstro->rclBkGround.top;
            arclTmp[culRcl].right    = prclOpaque->right;
            arclTmp[culRcl].left     = pstro->rclBkGround.right;
            arclTmp[culRcl++].bottom = pstro->rclBkGround.bottom;
        }

        // Bottom fragment

        if (pstro->rclBkGround.bottom < prclOpaque->bottom)
        {
            arclTmp[culRcl].bottom = prclOpaque->bottom;
            arclTmp[culRcl].left   = prclOpaque->left;
            arclTmp[culRcl].right  = prclOpaque->right;
            arclTmp[culRcl++].top  = pstro->rclBkGround.bottom;
        }

        // Fill any fringe rectangles we found

        for (i = 0; i < culRcl; i++)
        {
            bOpaqueRect(ppdev, &(arclTmp[i]), prclClip, pboOpaque);
        }

        // Set the mix mode for opaque text.

        mix = (mix & 0x0F) | (R2_COPYPEN << 8);
        b = bSetS3TextColorAndMix(ppdev, mix, pboFore, pboOpaque);
        if (b == FALSE)
            return (b);
    }
    else
    {
        // Take care of any opaque rectangles.

        if (prclOpaque != NULL)
        {
            bOpaqueRect(ppdev, prclOpaque, prclClip, pboOpaque);
        }

        jBackgroundRop = R2_NOP;

        // Take care of the glyph attributes, color and mix.

        mix = (mix & 0x0F) | (jBackgroundRop << 8);
        b = bSetS3TextColorAndMix(ppdev, mix, pboFore, pboOpaque);
        if (b == FALSE)
            return (b);
    }

    rclClip = *prclClip;
    rclClip.bottom;
    rclClip.right;
    vSetS3ClipRect(ppdev, &rclClip);

    // Test and setup for a mono-spaced font.

    if ((ulCharInc = pstro->ulCharInc) != 0)
        fl |= MONO_SPACED_FONT;

    // Setup the Command Word for the S3.

    S3Cmd = RECTANGLE_FILL  | BUS_SIZE_8         |
            WAIT            | DRAWING_DIR_TBLRXM | DRAW |
            LAST_PIXEL_ON   | MULTIPLE_PIXELS    | WRITE;

    FIFOWAIT(FIFO_2_EMPTY)

    // Enable the write mask for all planes.
    // and set the S3 for CPU Data.

    TEST_AND_SET_WRT_MASK(0xFF);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | CPU_DATA));

    // Get the Glyph Handles.

    if ((pstro->pgp) == NULL)
        STROBJ_vEnumStart(pstro);

    do
    {
        if (pstro->pgp == NULL)
        {
            bMoreGlyphs = STROBJ_bEnum(pstro, &cGlyphs, &pgp);
        }
        else
        {
            pgp = pstro->pgp;
            cGlyphs = pstro->cGlyphs;
            bMoreGlyphs = FALSE;
        }

        // For mono space fonts this is non-zero.

        if (fl & MONO_SPACED_FONT)
        {
            xMonoPosition = pgp[0].ptl.x;
            yMonoStart    = pgp[0].ptl.y;
        }

        for (iGlyph = 0; iGlyph < cGlyphs; iGlyph++)
        {
            // Get a pointer to the GlyphBits.

            pgb = pgp[iGlyph].pgdf->pgb;

            // Adjust the placement of the glyph.
            // If this is a mono-spaced font set the blt height & width only
            // once for the string.

            if (fl & MONO_SPACED_FONT)
            {
                ptl.x = xMonoPosition + pgb->ptlOrigin.x;
                ptl.y = yMonoStart + pgb->ptlOrigin.y;
                xMonoPosition += ulCharInc;

                if (!(fl & MONO_SIZE_VALID))
                {
                    fl |= MONO_SIZE_VALID;

                    // Calculate the number of bytes in this glyph.

                    cyGlyph = pgb->sizlBitmap.cy;

                    GlyphBmPitchInBytes = CJ_SCAN(pgb->sizlBitmap.cx);
                    cjGlyph = GlyphBmPitchInBytes * cyGlyph;

                    TFIFOWAIT(FIFO_2_EMPTY);
                    OUTPW(RECT_WIDTH, pgb->sizlBitmap.cx - 1);
                    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyGlyph - 1));
                }

                FIFOWAIT(FIFO_3_EMPTY)
            }
            else
            {
                ptl.x = pgp[iGlyph].ptl.x + pgb->ptlOrigin.x;
                ptl.y = pgp[iGlyph].ptl.y + pgb->ptlOrigin.y;

                // Calculate the number of bytes in this glyph.

                cyGlyph = pgb->sizlBitmap.cy;

                GlyphBmPitchInBytes = CJ_SCAN(pgb->sizlBitmap.cx);
                cjGlyph = GlyphBmPitchInBytes * cyGlyph;

                FIFOWAIT(FIFO_5_EMPTY)

                // Set up for the image transfer.

                OUTPW(RECT_WIDTH, pgb->sizlBitmap.cx - 1);
                OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyGlyph - 1));
            }

            // If a character is way off to the left then continue

            if ((ptl.x < 0) && ((ptl.x + pgb->sizlBitmap.cx) < 0))
                continue;

            // Yes, believe it or not we have to check for chipping on the right also.

            if ((ptl.x > (LONG) ppdev->cxScreen) && ((ptl.x + pgb->sizlBitmap.cx) > (LONG) ppdev->cxScreen))
                continue;

            // Set up for the image transfer.

            OUTPW(CUR_X, LOWORD(ptl.x));
            OUTPW(CUR_Y, LOWORD(ptl.y));

            GPWAIT();

            OUTPW(CMD, S3Cmd);

            CHECK_DATA_READY;

            vDataPortOutB(ppdev, pgb->aj, cjGlyph);

            CHECK_DATA_COMPLETE;

        }

    } while(bMoreGlyphs);

    return (TRUE);

}


/*****************************************************************************
 * S3 Solid Opaque Rect.
 *
 *  Returns TRUE if the Opaque Rect was handled.
 ****************************************************************************/
BOOL bOpaqueRect(
    PPDEV ppdev,
    RECTL *prclOpaque,
    RECTL *prclBounds,
    BRUSHOBJ *pboOpaque)
{
    INT     width, height;
    WORD    S3Cmd;
    ULONG   iSolidColor;
    RECTL   rclClipped;
    BOOL    bClipRequired;

    DISPDBG((4, "S3.DLL!bOpaqueRect - Entry\n"));

    rclClipped = *prclOpaque;

    // First handle the trivial rejection.

    bClipRequired = bIntersectTest(&rclClipped, prclBounds);

    // define the clipped target rectangle.

    if (bClipRequired)
    {
        rclClipped.left   = max (rclClipped.left,   prclBounds->left);
        rclClipped.top    = max (rclClipped.top,    prclBounds->top);
        rclClipped.right  = min (rclClipped.right,  prclBounds->right);
        rclClipped.bottom = min (rclClipped.bottom, prclBounds->bottom);
    }
    else
        return (TRUE);

    // Set the color

    iSolidColor = pboOpaque->iSolidColor;
    if (iSolidColor == -1)
        return(FALSE);

    width  = (rclClipped.right - rclClipped.left) - 1;
    height = (rclClipped.bottom - rclClipped.top) - 1;

    if (width >= 0 && height >= 0)
    {

        FIFOWAIT(FIFO_8_EMPTY)

        TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | OVERPAINT);
        TEST_AND_SET_FRGD_COLOR(LOWORD(iSolidColor));
        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
        OUTPW(CUR_X, LOWORD(rclClipped.left));
        OUTPW(CUR_Y, LOWORD(rclClipped.top));
        OUTPW(RECT_WIDTH, width);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | height));

        S3Cmd = RECTANGLE_FILL  | DRAWING_DIR_TBLRXM | DRAW |
                DIR_TYPE_XY     | LAST_PIXEL_ON      |
                MULTIPLE_PIXELS | WRITE;

        OUTPW(CMD, S3Cmd);
    }

    return (TRUE);
}



/******************************************************************************
 * bSetS3TextColorAndMix - Setup the S3's Text Colors and mix modes
 *
 *  Note: We will always set the mode to transparent.  We will assume the
 *        opaque rectangle will take care of any opaqueing we may need.
 *****************************************************************************/
BOOL bSetS3TextColorAndMix(
    PPDEV ppdev,
    MIX mix,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque)
{
    ULONG       ulForeSolidColor, ulBackSolidColor;
    BYTE        jS3ForeMix, jS3BackMix;

    // Pickup all the glyph attributes.

    jS3ForeMix       = Rop2ToS3Rop[(mix & 0xF) - R2_BLACK];
    ulForeSolidColor = pboFore->iSolidColor;

    jS3BackMix       = Rop2ToS3Rop[((mix >> 8) & 0xF) - R2_BLACK];
    ulBackSolidColor = pboOpaque->iSolidColor;

    // For now let the engine handle the non-solid brush cases. !!!
    // We should use S3 when we get some more time !!!

    if (ulForeSolidColor == -1 || ulBackSolidColor == -1)
        return(FALSE);

    FIFOWAIT(FIFO_4_EMPTY)

    // Set the S3 Attributes.

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | jS3ForeMix);
    TEST_AND_SET_FRGD_COLOR(LOWORD(ulForeSolidColor));

    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | jS3BackMix);
    TEST_AND_SET_BKGD_COLOR(LOWORD(ulBackSolidColor));

    return (TRUE);

}

/*****************************************************************************
 * pCacheFont - Make sure the glyphs we need in this font are cached.
 *              Return a pointer to the array of glyph caches.
 *
 *              if there is an error, return NULL.
 ****************************************************************************/
PCACHEDGLYPH pCacheFont(
    PPDEV ppdev,
    STROBJ *pstro,
    FONTOBJ  *pfo,
    PCACHEDFONT *ppCachedFont)
{
    FONTINFO    fi;
    PCACHEDFONT pCachedFont;
    ULONG       cFntGlyphs;
    UINT        nSize;

    BOOL    bFoundBit, bEven;
    ULONG   mask, mask1;
    INT     i, j;

    DISPDBG((3, "S3.DLL!pCacheFont - Entry\n"));

    // Allocate a Font Cache node.

    pCachedFont = (PCACHEDFONT) LocalAlloc(LPTR, sizeof(CACHEDFONT));
    if (pCachedFont == NULL)
    {
        DISPDBG((1, "S3.DLL!pCacheFont - LocalAlloc of pCachedFont failed\n"));
        return(NULL);
    }

    // Add this font to the beginning of the font list.

    pCachedFont->pcfNext = pCachedFontsRoot;
    pCachedFontsRoot     = pCachedFont;

    // Set the font ID for the font.

    pCachedFont->iUniq = pfo->iUniq;

    // Allocate the glyph cache.

    FONTOBJ_vGetInfo(pfo, sizeof(FONTINFO), &fi);
    cFntGlyphs = fi.cGlyphsSupported;

    // This is where we clamp the size of the Font structures we are allocating.

    if (cFntGlyphs > MAX_GLYPHS_TO_ALLOC)
        cFntGlyphs = MAX_GLYPHS_TO_ALLOC;

    // Round up to the next power of 2.

    bFoundBit = FALSE;
    mask = 0x80000000;
    for (i = 32; i != 0 && !bFoundBit; i--)
    {
        if (cFntGlyphs & mask)
        {
            bFoundBit = TRUE;
            mask1 = mask >> 1;
            bEven = TRUE;
            for (j = i - 1; j != 0; j--)
            {
                if (cFntGlyphs & mask1 )
                {
                    bEven = FALSE;
                    break;
                }
                mask1 >>= 1;
            }
        }
        else
            mask >>= 1;
    }

    if (bEven)
        cFntGlyphs = mask;
    else
        cFntGlyphs = mask << 1;

    // Get the font info.

    pCachedFont->cGlyphs = cFntGlyphs - 1;

    // Allocate memory for the CachedGlyphs of this font.

    nSize = cFntGlyphs * sizeof(CACHEDGLYPH);

    pCachedFont->pCachedGlyphs = (PCACHEDGLYPH) LocalAlloc(LPTR, nSize);
    if (pCachedFont->pCachedGlyphs == NULL)
    {
        DISPDBG((1, "S3.DLL!pCacheFont - LocalAlloc of pCachedGlyphs failed\n"));
        pCachedFont->cGlyphs = 0;
        return(NULL);
    }

    pCachedFont->pCachedGlyphs[0].hg = (HGLYPH)-1;

    // Return the pointer to the cached font.  This is required
    // by the collision handling code.

    *ppCachedFont = pCachedFont;

    return(pCachedFont->pCachedGlyphs);

}


/*****************************************************************************
 * bAllocateGlyph - Allocate and initialize the cached glyph
 ****************************************************************************/
BOOL bAllocateGlyph(
    PPDEV ppdev,
    HGLYPH hg,
    GLYPHBITS *pgb,
    PCACHEDGLYPH pcg)
{
    BOOL   b;
    ULONG  cyGlyph, GlyphBmPitchInPels, GlyphBmPitchInBytes;
    XYZPOINTL  xyzGlyph;
    WORD  Cmd;

    DISPDBG((3, "S3.DLL!bAllocateGlyph - Entry\n"));

    cyGlyph = pgb->sizlBitmap.cy;

    GlyphBmPitchInBytes = CJ_SCAN(pgb->sizlBitmap.cx);
    GlyphBmPitchInPels  = GlyphBmPitchInBytes * 8;

    // Allocate memory for the glyph data on the S3.

    b = bAllocGlyphMemory(ppdev, &(pgb->sizlBitmap), &xyzGlyph);
    if (b == FALSE)
    {
        DISPDBG((1, "S3.DLL!bAllocateGlyph - hCpAlloc failed\n"));
        return(FALSE);
    }

    // Initialize the Glyph Cache node.

    pcg->fl              |= VALID_GLYPH;
    pcg->hg               = hg;
    pcg->pcgCollisionLink = END_COLLISIONS;
    pcg->ptlOrigin        = pgb->ptlOrigin;
    pcg->sizlBitmap       = pgb->sizlBitmap;
    pcg->BmPitchInPels    = GlyphBmPitchInPels;
    pcg->BmPitchInBytes   = GlyphBmPitchInBytes;
    pcg->xyzGlyph         = xyzGlyph;

    // Initialize the Glyph Cache data in S3 memory.

    Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | MULTIPLE_PIXELS    | WRITE;

    // Setup the S3 chip.

    FIFOWAIT(FIFO_5_EMPTY);

    TEST_AND_SET_FRGD_MIX(LOGICAL_1);
    TEST_AND_SET_BKGD_MIX(LOGICAL_0);
    TEST_AND_SET_WRT_MASK(LOWORD(xyzGlyph.z));
    OUTPW(CUR_X, xyzGlyph.x);
    OUTPW(CUR_Y, xyzGlyph.y);

    FIFOWAIT(FIFO_4_EMPTY);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | CPU_DATA));
    OUTPW(RECT_WIDTH, GlyphBmPitchInPels - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cyGlyph - 1));
    GPWAIT();
    OUTPW(CMD, Cmd);

    CHECK_DATA_READY;

    // Now transfer the data.

    vDataPortOutB(ppdev, pgb->aj, GlyphBmPitchInBytes * cyGlyph);

    CHECK_DATA_COMPLETE;

    // Need to reset the write mask.

    FIFOWAIT(FIFO_1_EMPTY);

    TEST_AND_SET_WRT_MASK(0xFF);

    return (TRUE);
}


/****************************************************************************
 * vBlowCache - Blow Away the Cache
 ***************************************************************************/
VOID vBlowCache(PPDEV ppdev)
{
    PCACHEDFONT pcf;
    PCACHEDGLYPH pcg, pcgNext;
    INT nGlyphs, i;

    DISPDBG((2, "S3.DLL!vBlowCache - Entry\n"));

    // Traverse the CachedFonts list.
    // Free the collision nodes, and invalidate the cached glyphs

    for (pcf = pCachedFontsRoot; pcf != NULL; pcf = pcf->pcfNext)
    {
        // If there are any collision nodes for this glyph
        // free them.

        nGlyphs = pcf->cGlyphs+1;

        for (i = 0; i < nGlyphs; i++)
        {
            pcg = &(pcf->pCachedGlyphs[i]);
            pcg = pcg->pcgCollisionLink;
            for (; pcg != NULL; pcg = pcgNext)
            {
                pcgNext = pcg->pcgCollisionLink;
                LocalFree(pcg);
            }
            pcf->pCachedGlyphs[i].pcgCollisionLink = NULL;

        }

        // Invalidate all the glyphs in the glyph array.

        pcg = pcf->pCachedGlyphs;
        for (i = 0; i < nGlyphs; i++)
        {
            pcg[i].hg  = (HGLYPH) -1;
            pcg[i].fl &= ~VALID_GLYPH;
        }
    }

    // Now ReInitialize the S3 Heap.

    memset((PVOID)ppdev->ajGlyphAllocBitVector,
            0, CACHED_GLYPHS_ROWS * GLYPHS_PER_ROW);

    return;
}

/******************************************************************************
 * bAllocGlyphMemory -
 *
 *  Allocate the some memory for the glyph.
 *
 *
 *  return: TRUE    - if memory was allocated.
 *          FALSE   - if there was no more memory.
 *
 *****************************************************************************/
BOOL bAllocGlyphMemory(
    PPDEV ppdev,
    PSIZEL psizlGlyph,
    PXYZPOINTL pxyzGlyph)
{
    BOOL    bFound;
    INT     iPlane, iRow, iGlyph;
    BYTE    jPlaneBitVector;


    // Search the bit vector

    bFound = FALSE;
    for (iPlane = 0; iPlane < 8; iPlane++)
    {
        jPlaneBitVector = (BYTE) iPlaneBits[iPlane];

        for (iRow = 0; iRow < CACHED_GLYPHS_ROWS; iRow++)
        {
            for (iGlyph = 0; iGlyph < GLYPHS_PER_ROW; iGlyph++)
            {
                if (!(ppdev->ajGlyphAllocBitVector[iRow][iGlyph] & jPlaneBitVector))
                {
                    bFound = TRUE;
                    ppdev->ajGlyphAllocBitVector[iRow][iGlyph] |=
                            jPlaneBitVector;

                    pxyzGlyph->z = jPlaneBitVector;
                    pxyzGlyph->x = iGlyph * GLYPH_CACHE_CX;
                    pxyzGlyph->y = (iRow * GLYPH_CACHE_CY) + GLYPH_CACHE_Y;

                    DISPDBG((5, "S3.DLL!bAllocGlyphMemory\n"));
                    DISPDBG((5, "\t pxyzGlyph->z: %0x\n", pxyzGlyph->z));
                    DISPDBG((5, "\t pxyzGlyph->x: %d\n",  pxyzGlyph->x));
                    DISPDBG((5, "\t pxyzGlyph->y: %d\n",  pxyzGlyph->y));

                    return (bFound);
                }
            }
        }
    }

    return (bFound);
}

