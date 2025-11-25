/******************************Module*Header*******************************\
* Module Name: copybits.c
*
* DrvCopyBits
*
* Copyright (c) 1992 Microsoft Corporation
\**************************************************************************/
#include "driver.h"
#include "bitblt.h"

BOOL DrvCopyBits
(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    PRECTL    prclTrg,
    PPOINTL   pptlSrc
)
{
    PDEVSURF    pdsurf;             // Pointer to a device surface

    LONG        lDelta;             // Delta to next scan of destination
    PVOID       pjDstScan0;         // Pointer to scan 0 of destination DIB
    ULONG      *pulXlate;           // Pointer to color xlate vector

    BOOL        bMore;              // Clip continuation flag
    ULONG       ircl;               // Clip enumeration rectangle index
    RECT_ENUM   cben;               // Clip enumerator
    RECTL       rclTemp;
    PRECTL      prcl;
    POINTL      ptlTemp;
    DEVSURF     dsurfSrc;
    PDEVSURF    pdsurfTrg;          // Pointer for target
    PDEVSURF    pdsurfSrc;          // Pointer for source if present
    INT iCopyDir;
    PFN_ScreenToScreenBlt pfn_Blt;
    RECT_ENUM   bben;               // Clip enumerator
    BYTE        jClipping;
    UCHAR      *pucDIB4ToVGAConvTables;


//    ASSERT(psoTrg  != (SURFOBJ *) NULL, "DrvCopyBits: NULL Pointer for Target\n");
//    ASSERT(psoSrc  != (SURFOBJ *) NULL, "DrvCopyBits: NULL Pointer for Source\n");
//    ASSERT(prclTrg != (RECTL *)   NULL, "DrvCopyBits: NULL Pointer for Rect\n");
//    ASSERT(pptlSrc != (POINTL *)  NULL, "DrvCopyBits: NULL Pointer for Point\n");

//    ASSERT(((psoTrg->dhsurf != (DHSURF) 0) || (psoSrc->dhsurf != (DHSURF) 0)),
//            "DrvCopyBits: No device surface involved\n");

// Check for device surface to device surface

    if ((psoTrg->iType == STYPE_DEVICE) && (psoSrc->iType == STYPE_DEVICE))
    {
        pdsurfTrg = (PDEVSURF) psoTrg->dhsurf;
        pdsurfSrc = (PDEVSURF) psoSrc->dhsurf;

    // It's a screen-to-screen aligned SRCCOPY; special-case it

    // Determine the direction in which the copy must proceed
    // Note that although we could detect cases where the source
    // and dest don't overlap and handle them top to bottom, all
    // copy directions are equally fast, so there's no reason to go
    // top to bottom except possibly that it looks better. But it
    // also takes time to detect non-overlap, so I'm not doing it

    // Set up the clipping type

        if (pco == (CLIPOBJ *) NULL)
        {
        // No CLIPOBJ provided, so we don't have to worry about clipping

            jClipping = DC_TRIVIAL;
        }
        else
        {
        // Use the CLIPOBJ-provided clipping

            jClipping = pco->iDComplexity;
        }


        if (pptlSrc->y >= prclTrg->top) {
            if (pptlSrc->x >= prclTrg->left) {
                iCopyDir = CD_RIGHTDOWN;
            } else {
                iCopyDir = CD_LEFTDOWN;
            }
        } else {
            if (pptlSrc->x >= prclTrg->left) {
                iCopyDir = CD_RIGHTUP;
            } else {
                iCopyDir = CD_LEFTUP;
            }
        }

        // These values are expected by vAlignedSrcCopy
//        ASSERT(((CD_RIGHTDOWN == 0) && (CD_LEFTDOWN == 1) &&
//               (CD_RIGHTUP == 2) && (CD_LEFTUP == 3)),
//               "DrvBitBlt: Bad clip enumeration direction constants");

        switch(jClipping) {

            case DC_TRIVIAL:
                // Just copy the rectangle
                if ((((prclTrg->left ^ pptlSrc->x) & 0x07) == 0)) {
                    vAlignedSrcCopy(pdsurfTrg, prclTrg,
                                    pptlSrc, iCopyDir);
                } else {
                    vNonAlignedSrcCopy(pdsurfTrg, prclTrg,
                                       pptlSrc, iCopyDir);
                }
                break;

            case DC_RECT:
                // Clip the solid fill to the clip rectangle
                if (!DrvIntersectRect(&rclTemp, prclTrg,
                                      &pco->rclBounds)) {
                    // Nothing to draw; completely clipped
                    return TRUE;
                }

                // Adjust the source point for clipping too
                ptlTemp.x = pptlSrc->x + rclTemp.left - prclTrg->left;
                ptlTemp.y = pptlSrc->y + rclTemp.top - prclTrg->top;

                // Copy the clipped rectangle
                if ((((prclTrg->left ^ pptlSrc->x) & 0x07) == 0)) {
                    vAlignedSrcCopy(pdsurfTrg, &rclTemp, &ptlTemp,
                                    iCopyDir);
                } else {
                    vNonAlignedSrcCopy(pdsurfTrg, &rclTemp, &ptlTemp,
                                       iCopyDir);
                }
                break;

            case DC_COMPLEX:

                if ((((prclTrg->left ^ pptlSrc->x) & 0x07) == 0)) {
                    pfn_Blt = vAlignedSrcCopy;
                } else {
                    pfn_Blt = vNonAlignedSrcCopy;
                }

                CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES,
                                   iCopyDir, ENUM_RECT_LIMIT);

                do {
                    bMore = CLIPOBJ_bEnum(pco,(ULONG) sizeof(bben),
                                          (PVOID) &bben);

                    prcl = bben.arcl;
                    for (ircl = 0; ircl < bben.c; ircl++, prcl++) {

                        DrvIntersectRect(prcl,prcl,prclTrg);
                        // Adjust the source point for clipping too
                        ptlTemp.x = pptlSrc->x + prcl->left -
                                prclTrg->left;
                        ptlTemp.y = pptlSrc->y + prcl->top -
                                prclTrg->top;
                        pfn_Blt(pdsurfTrg, prcl,
                                &ptlTemp, iCopyDir);

                    }
                } while(bMore);
                break;
        }
        return TRUE;
    }

    if (psoSrc->iType == STYPE_BITMAP) {

        // DIB to screen

        switch(psoSrc->iBitmapFormat)
        {
        case BMF_4BPP:  // special case compatible DIBs with no translation

            if ((pxlo == NULL) || (pxlo->flXlate == XO_TRIVIAL)) {

                pucDIB4ToVGAConvTables =
                            ((PDEVSURF) psoTrg->dhsurf)->ppdev->
                            pucDIB4ToVGAConvTables;

                // Make just enough of a fake DEVSURF for the source so that
                // the DIB to VGA code can work

                dsurfSrc.lNextScan = psoSrc->lDelta;
                dsurfSrc.pvBitmapStart = psoSrc->pvScan0;

                // Clip as needed

                if ((pco == NULL) || (pco->iDComplexity == DC_TRIVIAL)) {

                    // No clipping, just copy the DIB to the VGA

                    vDIB2VGA((PDEVSURF) psoTrg->dhsurf, &dsurfSrc, prclTrg,
                            pptlSrc, pucDIB4ToVGAConvTables);

                } else if (pco->iDComplexity == DC_RECT) {

                    // Clip the destination to the clip rectangle; we
                    // should never get a NULL result
                    if (DrvIntersectRect(&rclTemp, prclTrg, &pco->rclBounds)) {

                        // Adjust the source point for clipping too
                        ptlTemp.x = pptlSrc->x + rclTemp.left - prclTrg->left;
                        ptlTemp.y = pptlSrc->y + rclTemp.top - prclTrg->top;

                        // Blt the clipped rectangle
                        vDIB2VGA((PDEVSURF) psoTrg->dhsurf, &dsurfSrc,
                                &rclTemp, &ptlTemp, pucDIB4ToVGAConvTables);
                    }
                    return(TRUE);

                } else {    // DC_COMPLEX:

                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES,
                                       CD_ANY, ENUM_RECT_LIMIT);

                    do {
                        bMore = CLIPOBJ_bEnum(pco,(ULONG) sizeof(bben),
                                              (PVOID) &bben);
                        prcl = bben.arcl;
                        for (ircl = 0; ircl < bben.c; ircl++, prcl++) {

                            // Clip the destination to the clip rectangle;
                            // we should never get a NULL result
                            DrvIntersectRect(prcl,prcl,prclTrg);

                            // Adjust the source point for clipping too
                            ptlTemp.x = pptlSrc->x + prcl->left -
                                    prclTrg->left;
                            ptlTemp.y = pptlSrc->y + prcl->top -
                                    prclTrg->top;

                            // Blt the clipped rectangle
                            vDIB2VGA((PDEVSURF) psoTrg->dhsurf,
                                    &dsurfSrc, prcl, &ptlTemp,
                                    pucDIB4ToVGAConvTables);
                        }
                    } while(bMore);

                }

                return(TRUE);
            }

        case BMF_1BPP:
        case BMF_8BPP:

            return(DrvBitBlt(psoTrg,
                             psoSrc,
                             (SURFOBJ *) NULL,
                             pco,
                             pxlo,
                             prclTrg,
                             pptlSrc,
                             (POINTL *) NULL,
                             (BRUSHOBJ *) NULL,
                             (POINTL *) NULL,
                             0x0000cccc));

        case BMF_8RLE:
        case BMF_4RLE:
            return(bRleBlt(psoTrg, psoSrc, pco, pxlo, prclTrg, pptlSrc));

        }
    }
    else
    {
    // screen to DIB

//        ASSERT(psoTrg->iType == STYPE_BITMAP, "ERROR CopyBits got 2 DIBs");

        if (psoTrg->iBitmapFormat == BMF_4BPP)
        {
            pdsurf = (PDEVSURF) psoSrc->dhsurf;

        // Get the data for the destination DIB.

            lDelta = psoTrg->lDelta;
            pjDstScan0 = (PBYTE) psoTrg->pvScan0;

        // Setup for any color translation which may be needed !!! Is any needed at all?

            if (pxlo == NULL)
            {
                pulXlate = NULL;
            }
            else
            {
                if (pxlo->flXlate & XO_TABLE)
                    pulXlate = pxlo->pulXlate;
                else
                {
//                    ASSERT(pxlo->flXlate & XO_TRIVIAL, "DrvCopyBits: Hopelessly complex translation\n");
                    pulXlate = (PULONG) NULL;
                }
            }

        // Set up for clip enumeration.

            if (pco != (CLIPOBJ *) NULL)
            {
                switch(pco->iDComplexity)
                {
                case DC_TRIVIAL:
                    bMore = FALSE;
                    cben.c = 1;
                    cben.arcl[0] = *prclTrg;        // Use the target for clipping
                    break;

                case DC_RECT:
                    bMore = FALSE;
                    cben.c = 1;
                    cben.arcl[0] = pco->rclBounds; // Use the bounds for clipping
                    break;

                case DC_COMPLEX:
                    bMore = TRUE;
                    cben.c = 0;
                    CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, ENUM_RECT_LIMIT);
                    break;
                }
            }
            else
            {
                bMore = FALSE;
                cben.c = 1;
                cben.arcl[0] = *prclTrg;            // Use the target for clipping
            }

        // Call the VGA conversion routine, adjusted for each rectangle

            do
            {
                LONG    xSrc;
                LONG    ySrc;
                RECTL  *prcl;

                if (bMore)
                    bMore = CLIPOBJ_bEnum(pco,(ULONG) sizeof(cben), (PVOID) &cben);

                for (ircl = 0; ircl < cben.c; ircl++)
                {
                    prcl = &cben.arcl[ircl];

                    xSrc = pptlSrc->x + prcl->left - prclTrg->left;
                    ySrc = pptlSrc->y + prcl->top  - prclTrg->top;

                    vConvertVGA2DIB(pdsurf,
                                    xSrc,
                                    ySrc,
                                    pjDstScan0,
                                    prcl->left,
                                    prcl->top,
                                    prcl->right - prcl->left,
                                    prcl->bottom - prcl->top,
                                    lDelta,
                                    psoTrg->iBitmapFormat,
                                    pulXlate);
                }
            } while (bMore);

            return(TRUE);
        }
    }

// This is how we do any formats that we don't support in our inner loops.

    return(SimCopyBits(psoTrg, psoSrc, pco, pxlo, prclTrg, pptlSrc));
}

/******************************Public*Routine******************************\
* SimCopyBits
*
* This function simulates CopyBits for the driver when the driver is asked
* to blt to formats it does not support.  It converts any blt to be between
* the device's preferred format and the screen.
*
\**************************************************************************/

BOOL SimCopyBits
(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    PRECTL    prclTrg,
    PPOINTL   pptlSrc
)
{
    HBITMAP  hbmTmp;
    SURFOBJ *psoTmp;
    RECTL    rclTmp;
    SIZEL    sizlTmp;
    BOOL     bReturn = FALSE;
    static POINTL ptl00 = {0,0};

    rclTmp.top = rclTmp.left = 0;
    rclTmp.right  = sizlTmp.cx = prclTrg->right - prclTrg->left;
    rclTmp.bottom = sizlTmp.cy = prclTrg->bottom - prclTrg->top;

// Create bitmap in our compatible format.

    hbmTmp = EngCreateBitmap(sizlTmp, sizlTmp.cx / 2, BMF_4BPP, 0, NULL);

    if (hbmTmp)
    {
        if ((psoTmp = EngLockSurface((HSURF)hbmTmp)) != NULL)
        {
            if (psoSrc->iType == STYPE_BITMAP)
            {
            // blting from DIB to screen

                if (EngCopyBits(psoTmp, psoSrc, NULL, pxlo, &rclTmp, pptlSrc))
                {
                // Let DrvCopyBits do this easy case copy to screen.

                    bReturn = DrvCopyBits(psoTrg, psoTmp, pco, NULL, prclTrg, &ptl00);
                }
            }
            else
            {
            // blting from screen to DIB

                if (DrvCopyBits(psoTmp, psoSrc, NULL, NULL, &rclTmp, pptlSrc))
                {
                // Let EngCopyBits copy between DIBS

                    bReturn = EngCopyBits(psoTrg, psoTmp, pco, pxlo, prclTrg, &ptl00);
                }
            }

            EngUnlockSurface(psoTmp);
        }

        EngDeleteSurface((HSURF)hbmTmp);
    }

    return(bReturn);
}
