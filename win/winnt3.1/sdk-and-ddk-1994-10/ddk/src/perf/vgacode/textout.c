/******************************Module*Header*******************************\
* Module Name: textout.c
*
* VGA DrvTextOut Entry point
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/
#include "driver.h"
#include "textout.h"
#include "winperf.h"        // performance API definitions (PERFCTR)

#define SO_MASK                \
(                              \
SO_FLAG_DEFAULT_PLACEMENT |    \
SO_ZERO_BEARINGS          |    \
SO_CHAR_INC_EQUAL_BM_BASE |    \
SO_MAXEXT_EQUAL_BM_SIDE        \
)

// accelerator masks for four canonical directions of
// writing (multiples of 90 degrees)

#define SO_LTOR          (SO_MASK | SO_HORIZONTAL)
#define SO_RTOL          (SO_LTOR | SO_REVERSED)
#define SO_TTOB          (SO_MASK | SO_VERTICAL)
#define SO_BTOT          (SO_TTOB | SO_REVERSED)

/**************************************************************************\
* Function Declarations
\**************************************************************************/

VOID  vGlyphBlt (PDEVSURF,PRECTL,ULONG,PGLYPHPOS,ULONG,ULONG,ULONG,FLONG);
VOID  vStringBlt(PDEVSURF,PRECTL,ULONG,PGLYPHPOS,ULONG,ULONG,ULONG,FLONG);
ULONG ulSetXParentRegs(ROP4,PULONG,BOOL);
VOID  vResetVGARegs(void);
BOOL  bIdenticalRect(PRECTL,PRECTL);
FLONG flClipRect(PRECTL,PRECTL,PRECTL);
VOID  lclFillRect(CLIPOBJ *pco, ULONG culRcl, PRECTL prcl, PDEVSURF pdsurf,
    INT iColor);
BOOL DrvIntersectRect(PRECTL prcDst, PRECTL prcSrc1, PRECTL prcSrc2);
VOID vFastText(GLYPHPOS *, ULONG, PBYTE, ULONG, ULONG, DEVSURF *, RECTL *,
    RECTL *, INT, INT, ULONG, RECTL *);

// definition of counter data area for performance counters (PERFCTR)
extern PPERF_COUNTER_BLOCK pCounterBlock;

/**************************************************************************\
* Mix mode-Rop4 Mapping Table
\**************************************************************************/

static ROP4 arop4[] = {             // !!! This table needs serious fixing
    0x0000,     /*  0       */
    0x0000,     /* DPon     */
    0x0000,     /* DPna     */
    0x0f0f,     /* Pn       */
    0x0000,     /* PDna     */
    0x5555,     /* Dn       */
    0x0000,     /* DPx      */
    0x0000,     /* DPan     */
    0x0000,     /* DPa      */
    0x0000,     /* DPxn     */
    0x0000,     /* D        */
    0x0000,     /* DPno     */
    0xf0f0,     /* P        */
    0x0000,     /* PDno     */
    0x0000,     /* DPo      */
    0xffff      /*  1       */
};

#define B_ORDERED_RECT(prcl) (((prcl) == (PRECTL)NULL) ? 1 : (((prcl)->left <= (prcl)->right) && ((prcl)->top <= (prcl)->bottom)))


/******************************Public*Routine******************************\
* BOOL DrvTextOut(pso,pstro,pfo,pco,prclExtra,prcOpaque,
*                 pvFore,pvBack,pptOrg,r2Fore,r2Back)
*
\**************************************************************************/

BOOL DrvTextOut(
 SURFOBJ  *pso,
 STROBJ   *pstro,
 FONTOBJ  *pfo,
 CLIPOBJ  *pco,
 PRECTL    prclExtra,
 PRECTL    prclOpaque,
 BRUSHOBJ *pboFore,
 BRUSHOBJ *pboOpaque,
 PPOINTL   pptlOrg,
 MIX       mix)
{
    PDEVSURF        pdsurf;                 // Pointer to device surface

    ULONG           iClip;                  // Clip object's complexity
    ULONG           iClipTmp;               // Clip object's complexity
    ULONG           ircl;                   // index to current clip rect
    BOOL            bMore;                  // Flag for clip enumeration
    RECT_ENUM       txen;                   // Clip enumeration object

    GLYPHPOS       *pgp;                    // pointer to the 1st glyph
    GLYPHBITS      *pgb;                    // pointer to glyph bits.

    BOOL            bMoreGlyphs;            // Glyph enumeration flag
    ULONG           cGlyph;                 // number of glyphs in one batch
    ULONG           iGlyph;                 // index to current glyph
    RECTL           rclGlyph;               // string/glyph cell
    RECTL           rclTmp;                 // Temporary rectangle
    RECTL           rclBankTmp;             // Temporary rectangle when bank
                                            //  clipping

    ROP4            rop4;                   // Rop tranlated from mix mode
    ULONG           iSolidTextColor;        // Solid foreground text color
    ULONG           iSolidForeColor;        // Solid foreground color
    ULONG           iSolidBkColor;          // Solid background color

    FLONG           flStr = 0;              // Accelator flag for DrvTextOut()

    ULONG           ulMixMode;              // Glyph mix mode
    PFN             pfnBlt;                 // function to output glyph/string
    FLONG           flOption = 0;           // Accelator flag for pfnBlt
    FLONG           flOptionTemp;           // Working version of flOption

    RECTL           arclTmp[4];             // Temp storage for portions of
                                            //  opaquing rect
    ULONG           culRcl;                 // Temp rectangle count

    PDWORD          pdwCounter;		        // Pointer to counter to increment (PERFCTR)

    // Increment BitBlt counter (PERFCTR)
    pdwCounter = ( (PDWORD) pCounterBlock  ) + 1;
    (*pdwCounter)++;


    //---------------------------------------------------------------------
    // Get the target surface information
    //---------------------------------------------------------------------

    // Now find out if the target is the screen
    pdsurf = (PDEVSURF) pso->dhsurf;

    //---------------------------------------------------------------------
    // Get information about clip object.
    //---------------------------------------------------------------------

    iClip = DC_TRIVIAL;

    if (pco != NULL) {
        iClip = pco->iDComplexity;
    }

    //---------------------------------------------------------------------
    // Get text color.
    //---------------------------------------------------------------------

    iSolidForeColor = iSolidTextColor = pboFore->iSolidColor;

    //---------------------------------------------------------------------
    // See if this is text we can handle faster with special-case code.
    //---------------------------------------------------------------------

    if ((((prclOpaque != NULL) && (iClip != DC_COMPLEX)) ||
                                            // handle opaque both non-clipped &
                                            //  rectangle-clipped
         ((prclOpaque == NULL) && (iClip == DC_TRIVIAL))) &&
                                            // handle xpar non-clipped only
                                            //  for now
            (pstro->pgp != NULL) &&         // no glyph enumeration for now
            (prclExtra == NULL) &&          // no extra rects for now
            ((pstro->flAccel & (SO_HORIZONTAL | SO_VERTICAL | SO_REVERSED)) ==
             SO_HORIZONTAL)) {              // only left-to-right text for now

        ULONG ulBufferWidthInBytes;
        ULONG ulBufferBytes;
        BOOL  bTextPerfectFit;
        ULONG fDrawFlags;
        BYTE *pjTempBuffer;
        BOOL  bTempAlloc;

        // It's the type of text we can special-case; see if the temp buffer is
        // big enough for the text; if not, try to allocate enough memory.
        // Round up to the nearest dword multiple.

        ulBufferWidthInBytes = ((((pstro->rclBkGround.right + 15) & ~0x0F) -
                (pstro->rclBkGround.left & ~0x0F)) >> 3);
        ulBufferBytes = ((ulBufferWidthInBytes *
                (pstro->rclBkGround.bottom - pstro->rclBkGround.top)) + 3)
                & ~0x03;

        if (ulBufferBytes <= pdsurf->ulTempBufferSize) {
            // The temp buffer is big enough, so we'll use it
            pjTempBuffer = pdsurf->pvBankBufferPlane0;
            bTempAlloc = FALSE;
        } else {
            // The temp buffer isn't big enough, so we'll try to allocate
            // enough memory
            if ((pjTempBuffer =
                    LocalAlloc(LMEM_FIXED, ulBufferBytes)) == NULL) {
                // We couldn't get enough memory, so fall back to the general,
                // slow code
                    goto NoFastText;
            }

            // Mark that we have to free the buffer when we're done
            bTempAlloc = TRUE;
        }

        // One way or another, we've found a buffer that's big enough; set up
        // for accelerated text drawing

        // Set fixed pitch, overlap, and top & bottom Y alignment flags
        fDrawFlags = ((pstro->ulCharInc != 0) ? 0x01 : 0) |
                     (((pstro->flAccel & (SO_ZERO_BEARINGS |
                      SO_FLAG_DEFAULT_PLACEMENT)) !=
                      (SO_ZERO_BEARINGS | SO_FLAG_DEFAULT_PLACEMENT))
                      ? 0x02 : 0) |
                     (((pstro->flAccel & (SO_ZERO_BEARINGS |
                      SO_FLAG_DEFAULT_PLACEMENT |
                      SO_MAXEXT_EQUAL_BM_SIDE)) ==
                      (SO_ZERO_BEARINGS | SO_FLAG_DEFAULT_PLACEMENT |
                      SO_MAXEXT_EQUAL_BM_SIDE)) ? 0x04 : 0);

        // If there's an opaque rectangle, we'll do as much opaquing as
        // possible as we do the text. If the opaque rectangle is larger than
        // the text rectangle, then we'll do the fringe areas right now, and
        // the text and associated background areas together later
        if (prclOpaque != (PRECTL) NULL) {

            // This driver only handles solid brushes
            iSolidBkColor = pboOpaque->iSolidColor;

            // See if we have fringe areas to do. If so, build a list of
            // rectangles to fill, in rightdown order

            culRcl = 0;

            // Top fragment
            if (pstro->rclBkGround.top > prclOpaque->top) {
                arclTmp[culRcl].top = prclOpaque->top;
                arclTmp[culRcl].left = prclOpaque->left;
                arclTmp[culRcl].right = prclOpaque->right;
                arclTmp[culRcl++].bottom = pstro->rclBkGround.top;
            }

            // Left fragment
            if (pstro->rclBkGround.left > prclOpaque->left) {
                arclTmp[culRcl].top = pstro->rclBkGround.top;
                arclTmp[culRcl].left = prclOpaque->left;
                arclTmp[culRcl].right = pstro->rclBkGround.left;
                arclTmp[culRcl++].bottom = pstro->rclBkGround.bottom;
            }

            // Right fragment
            if (pstro->rclBkGround.right < prclOpaque->right) {
                arclTmp[culRcl].top = pstro->rclBkGround.top;
                arclTmp[culRcl].right = prclOpaque->right;
                arclTmp[culRcl].left = pstro->rclBkGround.right;
                arclTmp[culRcl++].bottom = pstro->rclBkGround.bottom;
            }

            // Bottom fragment
            if (pstro->rclBkGround.bottom < prclOpaque->bottom) {
                arclTmp[culRcl].bottom = prclOpaque->bottom;
                arclTmp[culRcl].left = prclOpaque->left;
                arclTmp[culRcl].right = prclOpaque->right;
                arclTmp[culRcl++].top = pstro->rclBkGround.bottom;
            }

            // Fill any fringe rectangles we found
            if (culRcl != 0) {
                if (iClip == DC_TRIVIAL) {
                    vTrgBlt(pdsurf, culRcl, arclTmp, R2_COPYPEN,
                            iSolidBkColor);
                } else {
                    lclFillRect(pco, culRcl, arclTmp, pdsurf,
                                iSolidBkColor);
                }
            }
        }

        // We're done with separate opaquing; any further opaquing will happen
        // as part of the text drawing

        // Clear the buffer if the text isn't going to set every bit
        bTextPerfectFit = (pstro->flAccel & (SO_ZERO_BEARINGS |
                SO_FLAG_DEFAULT_PLACEMENT | SO_MAXEXT_EQUAL_BM_SIDE |
                SO_CHAR_INC_EQUAL_BM_BASE)) ==
                (SO_ZERO_BEARINGS | SO_FLAG_DEFAULT_PLACEMENT |
                SO_MAXEXT_EQUAL_BM_SIDE | SO_CHAR_INC_EQUAL_BM_BASE);

        if (!bTextPerfectFit) {
            // Note that we already rounded up to a dword multiple size.
            vClearMemDword((ULONG *)pjTempBuffer, ulBufferBytes >> 2);
        }

        // Draw the text into the temp buffer, and thence to the screen
        vFastText(pstro->pgp,
                pstro->cGlyphs,
                ((PBYTE) pjTempBuffer) +
                 ((pstro->rclBkGround.left & 0x08) ? 1 : 0),
                ulBufferWidthInBytes,
                pstro->ulCharInc,
                pdsurf,
                &pstro->rclBkGround,
                prclOpaque,
                iSolidForeColor,
                iSolidBkColor,
                fDrawFlags,
                (iClip == DC_TRIVIAL) ? NULL : &pco->rclBounds);

        // Free up any memory we allocated for the temp buffer
        if (bTempAlloc) {
            LocalFree(pjTempBuffer);
        }

        return(TRUE);

    }

NoFastText:

//--------------------------------------------------------------------------
// Get the foreground and background colors
//--------------------------------------------------------------------------

    rop4 = arop4[(mix & 255) - R2_BLACK];    // Get ROP4 from mix mode

    // This driver only handles solid brushes

    // If no background rectangle, we need to paint only the glyph foreground.
    // Set VGA registers for the current rop and foreground color at once and
    // maintain them while blting the glyphs.

    if (prclOpaque == (PRECTL) NULL)
    {
        ulMixMode = ulSetXParentRegs(rop4, &iSolidForeColor, TRUE);
        flStr |= TO_NO_OPAQUE_RECT;
    } else {

        // This driver only handles solid brushes
        iSolidBkColor = pboOpaque->iSolidColor;
//        ASSERT ( iSolidBkColor != 0xFFFFFFFFL, "Non solid opaque brush\n" );
    }

//--------------------------------------------------------------------------
// Enumerate glyphs for the string, and output to screen one batch at a time.
// Here is how we will output background rectangle and glyphs:-
//
//  if glyphs are non-justified and horizontally aligned
//      if opaque background rectangle doesn't exactly match the string bound
//          Paint portions of background that are outside text cells
//      Output the string one batch at a time, drawing both fg & bg in cells
//  else
//      Paint opaque rectangle only once
//      Output to screen one glyph at a time
//
//--------------------------------------------------------------------------

// Set up the STROBJ for enumerating the glyphs

    // STROBJ_vEnumStart(pstro);    // This is automatic.

    if ((pstro->flAccel == SO_LTOR) && (pstro->ulCharInc != 0))
    {
        flStr |= TO_FIXED_PITCH;
        if ((pstro->ulCharInc & 7) == 0)
            flStr |= TO_MULTIPLE_BYTE;
    }

    do {

        // Get the next batch of glyphs

        if (pstro->pgp != NULL) {
            // There's only the one batch of glyphs, so save ourselves a call
            pgp = pstro->pgp;
            cGlyph = pstro->cGlyphs;
            bMoreGlyphs = FALSE;
        } else {
            bMoreGlyphs = STROBJ_bEnum(pstro,&cGlyph,&pgp);
        }

        //--------------------------------------------------------------------
        //  No glyph, no work!
        //--------------------------------------------------------------------

        if ( cGlyph ) {
            // Collect information about the string.

            pgb = pgp->pgdf->pgb;   // Locate the bitmap info.

            if ((flStr & TO_MULTIPLE_BYTE) &&
                ((pgb->ptlOrigin.x + pgp->ptl.x) & 7) == 0) {
                flStr |= TO_BYTE_ALIGNED;
            }

            if ((pstro->flAccel == SO_LTOR) &&
                    (pgb->sizlBitmap.cy <= MAX_GLYPH_HEIGHT)) {
                // We can blt horizontally aligned string with non-justified
                // text at once.

                pfnBlt = (PFN)vStringBlt;
                flOption |= VGB_ENTIRE_STRING_BLT;

                // we can special-case an aligned byte-size string

                flOption |= (flStr & (VGB_BYTE_ALIGNED | VGB_MULTIPLE_BYTE));

                // If there's an opaque rectangle, then if the opaque
                // rectangle exactly matches the text rectangle, we'll do the
                // opaque rectangle as we do the text. If the opaque rectangle
                // is larger than the text rectangle, then we'll do the fringe
                // areas right now, and the text and associated background
                // areas together, later.
                if (!(flStr & TO_NO_OPAQUE_RECT)) {

                    // See if we have fringe areas to do. If so, build a
                    // list of rectangles to fill, in rightdown order

                    culRcl = 0;

                    // Top fragment
                    if (pstro->rclBkGround.top > prclOpaque->top) {
                        arclTmp[culRcl].top = prclOpaque->top;
                        arclTmp[culRcl].left = prclOpaque->left;
                        arclTmp[culRcl].right = prclOpaque->right;
                        arclTmp[culRcl++].bottom = pstro->rclBkGround.top;
                    }

                    // Left fragment
                    if (pstro->rclBkGround.left > prclOpaque->left) {
                        arclTmp[culRcl].top = pstro->rclBkGround.top;
                        arclTmp[culRcl].left = prclOpaque->left;
                        arclTmp[culRcl].right = pstro->rclBkGround.left;
                        arclTmp[culRcl++].bottom =
                                pstro->rclBkGround.bottom;
                    }

                    // Right fragment
                    if (pstro->rclBkGround.right < prclOpaque->right) {
                        arclTmp[culRcl].top = pstro->rclBkGround.top;
                        arclTmp[culRcl].right = prclOpaque->right;
                        arclTmp[culRcl].left = pstro->rclBkGround.right;
                        arclTmp[culRcl++].bottom =
                                pstro->rclBkGround.bottom;
                    }

                    // Bottom fragment
                    if (pstro->rclBkGround.bottom < prclOpaque->bottom) {
                        arclTmp[culRcl].bottom = prclOpaque->bottom;
                        arclTmp[culRcl].left = prclOpaque->left;
                        arclTmp[culRcl].right = prclOpaque->right;
                        arclTmp[culRcl++].top = pstro->rclBkGround.bottom;
                    }

                    // Fill any fringe rectangles we found
                    if (culRcl > 0) {
                        if (iClip == DC_TRIVIAL) {
                            vTrgBlt(pdsurf, culRcl, arclTmp, R2_COPYPEN,
                                    iSolidBkColor);
                        } else {
                            lclFillRect(pco, culRcl, arclTmp, pdsurf,
                                        iSolidBkColor);
                        }
                    }


                    // The opaque rectangle exactly matches the text
                    // background cells. We can simultaneously paint it
                    // with the glyph. Then, pretend no background
                    // rectangle.

                    flStr |= TO_NO_OPAQUE_RECT;
                    flOption |= VGB_OPAQUE_BKGRND;

                    // Although we do not want to set the VGA regs, we
                    // need to know their mode when we actually blt.

                    ulMixMode = ulSetXParentRegs(rop4, &iSolidForeColor, FALSE);

                }

                // It is foolishly assumed in the clipping vStrBlt that GDI
                // puts all positions in the GLYPHPOS array even when the font
                // is fixed pitch.

                if (pstro->ulCharInc != 0) {
                    UINT ii;
                    LONG x,y;

                    x = pgp[0].ptl.x;
                    y = pgp[0].ptl.y;

                    for (ii=1; ii<cGlyph; ii++) {
                        x += pstro->ulCharInc;
                        pgp[ii].ptl.x = x;
                        pgp[ii].ptl.y = y;
                    }
                }
            } else {
                // We need to output one glyph at a time.

                pfnBlt = (PFN) vGlyphBlt;

                // It is foolishly assumed in vGlyphBlt that GDI puts all
                // positions in the GLYPHPOS array even when the font is
                // fixed pitch.

                if (pstro->ulCharInc != 0) {
                    UINT ii;
                    LONG x,y;

                    x = pgp[0].ptl.x;
                    y = pgp[0].ptl.y;
                    for (ii=1; ii<cGlyph; ii++) {
                        x += pstro->ulCharInc;
                        pgp[ii].ptl.x = x;
                        pgp[ii].ptl.y = y;
                    }
                }
            }
        }

        //------------------------------------------------------------------
        // Paint background rectangle if exists.
        //------------------------------------------------------------------

        if (!(flStr & TO_NO_OPAQUE_RECT))
        {
            switch ( iClip )
            {
                case DC_RECT:

                // Intersect opaque and clip rectangles

                    flClipRect(&rclTmp, prclOpaque, &pco->rclBounds);

                    if (BINVALIDRECT(rclTmp))
                    {
                        break;
                    };
                    prclOpaque = &rclTmp;

                case DC_TRIVIAL:
                    vTrgBlt(pdsurf, 1, prclOpaque, R2_COPYPEN, iSolidBkColor);
                    break;

                case DC_COMPLEX:

                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY,
                            ENUM_RECT_LIMIT);
                    bMore = TRUE;

                    do
                    {
                        if (bMore) {
                            bMore = CLIPOBJ_bEnum(pco, (ULONG)
                                    sizeof(txen), (PVOID) &txen);
                        }

                        for (ircl = 0; ircl < txen.c; ircl++)
                        {

                            // Clip only if we have clipping region

                            flClipRect ( &rclTmp, prclOpaque, &txen.arcl[ircl] );

                                if (!(BINVALIDRECT(rclTmp)))
                            {
                                // rop4 <--> PATCOPY

                                vTrgBlt(pdsurf, 1, &rclTmp,
                                        R2_COPYPEN, iSolidBkColor);
                            };
                        }

                    } while (bMore);
                    break;
            }

        // We need to paint the background only once. So pretend it
        // does not exist anymore.

            flStr |= TO_NO_OPAQUE_RECT;

        // Set VGA registers for transparent glyphs.

            ulMixMode = ulSetXParentRegs(rop4, &iSolidForeColor, TRUE);

        }

    //----------------------------------------------------------------------
    // We are ready to output glyphs to screen. By now either the background
    // rectangle (if any) was painted or we will paint it while outputting
    // each glyph/string.
    //----------------------------------------------------------------------

        for (iGlyph = 0; iGlyph < cGlyph; iGlyph++)
        {
            pgb = pgp[iGlyph].pgdf->pgb;
            iClipTmp = iClip;

            if (!(flOption & VGB_ENTIRE_STRING_BLT))
            {
                pgp[iGlyph].ptl.x += pgb->ptlOrigin.x;
                pgp[iGlyph].ptl.y += pgb->ptlOrigin.y;

                rclGlyph.left   = pgp[iGlyph].ptl.x;
                rclGlyph.right  = rclGlyph.left + pgb->sizlBitmap.cx;
                rclGlyph.top    = pgp[iGlyph].ptl.y;
                rclGlyph.bottom = rclGlyph.top  + pgb->sizlBitmap.cy;

                if ((iClipTmp == DC_TRIVIAL) &&
                    ((rclGlyph.left   < pco->rclBounds.left)  ||
                     (rclGlyph.top    < pco->rclBounds.top)   ||
                     (rclGlyph.right  > pco->rclBounds.right) ||
                     (rclGlyph.bottom > pco->rclBounds.bottom)))
                    iClipTmp = DC_RECT;
            } else {
                // Build the correct bounding box for the current batch of
                // glyphs (the bounding box in the STROBJ is for the entire
                // string, not on a per-batch basis)
                rclGlyph.left   = pgp[iGlyph].ptl.x;
                rclGlyph.right  = pgp[iGlyph + cGlyph - 1].ptl.x +
                        pgp[iGlyph + cGlyph - 1].pgdf->pgb->sizlBitmap.cx;
                rclGlyph.top = pstro->rclBkGround.top;
                rclGlyph.bottom = pstro->rclBkGround.bottom;
            }

            flOption &= ~(VGB_VERT_CLIPPED_GLYPH | VGB_HORIZ_CLIPPED_GLYPH);

            switch ( iClipTmp )
            {
                case DC_RECT:

                    // Intersect glyph and clip rectangles

                    flOption |= flClipRect(&rclGlyph, &rclGlyph, &pco->rclBounds);
                    if (BINVALIDRECT(rclGlyph))
                        break;

                case DC_TRIVIAL:
                    // Cycle through all banks that the (possibly clipped)
                    // glpyh rect spans

                    // If the proper bank for the top scan line of
                    // the clipped glyph isn't mapped in, map it in
                    if ((rclGlyph.top < pdsurf->rcl1WindowClip.top) ||
                            (rclGlyph.top >= pdsurf->rcl1WindowClip.bottom)) {

                        // Map in the bank containing the top (possibly
                        // clipped) glyph line
                        pdsurf->pfnBankControl(pdsurf,
                                               rclGlyph.top,
                                               JustifyTop);
                    }

                    // Now draw the part of the rect that's in each
                    // bank

                    for (;;) {

                        flOptionTemp = flOption |
                              flClipRect(&rclBankTmp, &rclGlyph,
                              &pdsurf->rcl1WindowClip);

                        (*pfnBlt)(pdsurf, &rclBankTmp, cGlyph,
                                  &pgp[iGlyph], iSolidForeColor, iSolidBkColor,
                                  ulMixMode, flOptionTemp);

                        // Done if this bank contains the last line
                        // of the fill
                        if (rclGlyph.bottom <= pdsurf->rcl1WindowClip.bottom) {
                            break;
                        }

                        // Map in the next bank
                        pdsurf->pfnBankControl(pdsurf,
                                               pdsurf->rcl1WindowClip.bottom,
                                               JustifyTop);

                    }

                    break;

                case DC_COMPLEX:
                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY,
                            ENUM_RECT_LIMIT);
                    bMore = TRUE;

                    do
                    {
                        if (bMore) {
                            // Enumerate more clip rects
                            bMore = CLIPOBJ_bEnum(pco, (ULONG) sizeof(txen),
                                    (PVOID) &txen);
                        }

                        // Draw the portion of the glyph that intersects each
                        // clip rect in turn
                        for (ircl = 0; ircl < txen.c; ircl++)
                        {
                            // Clip the glyph to the next clip rect, and set
                            // our clipping flags
                            flOption |= flClipRect ( &rclTmp, &rclGlyph,
                                                     &txen.arcl[ircl] );

                            // If we have a valid rectangle to draw, draw it
                            if (!(BINVALIDRECT(rclTmp))) {
                                // Cycle through all banks that the clipped
                                // glpyh rect spans

                                // If the proper bank for the top scan line of
                                // the clipped glyph isn't mapped in, map it in
                                if ((rclTmp.top <
                                        pdsurf->rcl1WindowClip.top) ||
                                        (rclTmp.top >=
                                        pdsurf->rcl1WindowClip.bottom)) {

                                    // Map in the bank containing the top
                                    // clipped glyph line
                                    pdsurf->pfnBankControl(pdsurf,
                                                           rclTmp.top,
                                                           JustifyTop);
                                }

                                // Now draw the part of the rect that's in each
                                // bank

                                for (;;) {

                                    flOptionTemp = flOption |
                                            flClipRect(&rclBankTmp, &rclTmp,
                                            &pdsurf->rcl1WindowClip);


                                    (*pfnBlt)(pdsurf, &rclBankTmp, cGlyph,
                                              &pgp[iGlyph], iSolidForeColor,
                                              iSolidBkColor, ulMixMode,
                                              flOptionTemp);

                                    // Done if this bank contains the last line
                                    // of the fill
                                    if (rclTmp.bottom <=
                                            pdsurf->rcl1WindowClip.bottom) {
                                        break;
                                    }

                                    // Map in the next bank
                                    pdsurf->pfnBankControl(pdsurf,
                                               pdsurf->rcl1WindowClip.bottom,
                                               JustifyTop);

                                }
                            }

                            flOption &= ~(VGB_VERT_CLIPPED_GLYPH |
                                          VGB_HORIZ_CLIPPED_GLYPH);
                        }
                    } while (bMore);
                    break;
            }

            // We might have blted the entire string.

            if (flOption & VGB_ENTIRE_STRING_BLT)
                break;
        }

    } while (bMoreGlyphs);

// Reset VGA registers

    vResetVGARegs();

//--------------------------------------------------------------------------
// Now handle the 'extra' rectangles.
//--------------------------------------------------------------------------

    if (prclExtra != (PRECTL) NULL)
    {
        while (prclExtra->left != prclExtra->right)
        {
            switch ( iClip )
            {
                case DC_TRIVIAL:
                    vTrgBlt(pdsurf, 1, prclExtra, mix, iSolidTextColor);
                    break;

                case DC_RECT:

                // Intersect opaque and clip rectangles

                    flClipRect ( &rclTmp, prclExtra, &pco->rclBounds );

                    if (BINVALIDRECT(rclTmp))
                        break;

                    vTrgBlt(pdsurf, 1, &rclTmp, mix, iSolidTextColor);
                    break;

                case DC_COMPLEX:
                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY,
                            ENUM_RECT_LIMIT);
                    bMore = TRUE;

                    do
                    {
                        if (bMore) {
                            bMore = CLIPOBJ_bEnum(pco, (ULONG) sizeof(txen),
                                    (PVOID) &txen);
                        }

                        for (ircl = 0; ircl < txen.c; ircl++)
                        {
                            flClipRect ( &rclTmp, prclExtra, &txen.arcl[ircl] );

                            if (!(BINVALIDRECT(rclTmp)))
                            {
                                vTrgBlt(pdsurf, 1, &rclTmp,
                                        mix, iSolidTextColor);
                            };
                        }
                    } while (bMore);
                    break;
            };

            prclExtra++;             // Advance to next extra rectangle
        }
    }

//--------------------------------------------------------------------------
//  Clean up the surface
//--------------------------------------------------------------------------

    pptlOrg;
    return(TRUE);
}


//--------------------------------------------------------------------------
// Fills the specified rectangles on the specified surface with the
// specified color, honoring the requested clipping. No more than four
// rectangles should be passed in. Intended for drawing the areas of the
// opaquing rectangle that extended beyond the text box. The rectangles must
// be in left to right, top to bottom order. Assumes there is at least one
// rectangle in the list.
//--------------------------------------------------------------------------

VOID lclFillRect(
 CLIPOBJ *pco,
 ULONG culRcl,
 PRECTL prcl,
 PDEVSURF pdsurf,
 INT iColor)
{
    BOOL  bMore;                  // Flag for clip enumeration
    RECT_ENUM txen;                // Clip enumeration object
    ULONG i, j;
    RECTL arclTmp[4];
    ULONG culRclTmp;
    RECTL *prclTmp, *prclClipTmp;
    INT   iLastBottom;
    RECTL *pClipRcl;
    INT iClip;

//    ASSERT(culRcl <= 4, "Too many rects");

    iClip = DC_TRIVIAL;

    if (pco != NULL) {
        iClip = pco->iDComplexity;
    }

    switch ( iClip ) {

        case DC_TRIVIAL:

            vTrgBlt(pdsurf, culRcl, prcl, R2_COPYPEN, iColor);

            break;

        case DC_RECT:

            prclTmp = &pco->rclBounds;

            // Generate a list of clipped rects
            for (culRclTmp=0, i=0; i<culRcl; i++, prcl++) {

                // Intersect fill and clip rectangles
                if (DrvIntersectRect(&arclTmp[culRclTmp], prcl, prclTmp)) {

                    // Add to list if anything's left to draw
                    culRclTmp++;
                }
            }

            // Draw the clipped rects
            if (culRclTmp != 0) {
                vTrgBlt(pdsurf, culRclTmp, arclTmp, R2_COPYPEN, iColor);
            }

            break;

        case DC_COMPLEX:

            // Bottom of last rectangle to fill
            iLastBottom = prcl[culRcl-1].bottom;

            // Initialize the clip rectangle enumeration to rightdown so we can
            // take advantage of the rectangle list being rightdown
            CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_RIGHTDOWN,
                    ENUM_RECT_LIMIT);

            // Scan through all the clip rectangles, looking for intersects
            // of fill areas with region rectangles
            do {

                // Get a batch of region rectangles
                bMore = CLIPOBJ_bEnum(pco, (ULONG)sizeof(txen), (PVOID)&txen);

                // Clip the rect list to each region rect
                for (j = txen.c, pClipRcl = txen.arcl; j-- > 0; pClipRcl++) {

                    // Since the rectangles and the region enumeration are both
                    // rightdown, we can zip through the region until we reach
                    // the first fill rect, and are done when we've passed the
                    // last fill rect.

                    if (pClipRcl->top >= iLastBottom) {
                        // Past last fill rectangle; nothing left to do
                        return;
                    }

                    // Do intersection tests only if we've reached the top of
                    // the first rectangle to fill
                    if (pClipRcl->bottom > prcl->top) {

                        // We've reached the top Y scan of the first rect, so
                        // it's worth bothering checking for intersection

                        // Generate a list of the rects clipped to this region
                        // rect
                        prclTmp = prcl;
                        prclClipTmp = arclTmp;
                        for (i = culRcl, culRclTmp=0; i-- > 0; prclTmp++) {

                            // Intersect fill and clip rectangles
                            if (DrvIntersectRect(prclClipTmp, prclTmp,
                                    pClipRcl)) {

                                // Add to list if anything's left to draw
                                culRclTmp++;
                                prclClipTmp++;
                            }
                        }

                        // Draw the clipped rects
                        if (culRclTmp != 0) {
                            vTrgBlt(pdsurf, culRclTmp, arclTmp, R2_COPYPEN,
                                    iColor);
                        }
                    }
                }
            } while (bMore);

            break;
    }

}
