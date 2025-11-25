/******************************Module*Header*******************************\
* Module Name: bitblt.c
*
* Banked Frame Buffer bitblit
*
* Copyright (c) 1992 Microsoft Corporation
*
\**************************************************************************/

#include "driver.h"

#define SRCBM_CACHE 1

BOOL    bTest = FALSE;

ULONG   nColorBrushes,
        nColorBrushCacheHit,
        nColorBrushExpansionCacheHit,
        nColorBrushCacheInvalidations;

ULONG   nMonoBrushes,
        nMonoBrushCacheHit,
        nMonoBrushExpansionCacheHit,
        nMonoBrushCacheInvalidations;

ULONG   n8BppBitmaps,
        n8BppBmCacheHits;

ULONG   n1BppBitmaps,
        n1BppBmCacheHits;


extern  ULONG   nSsbMovedToHostFromSrcBmCache;

BYTE   ajMonoPatPlanes[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};


/// Define the A vector polynomial bits
//
// Each bit corresponds to one of the terms in the polynomial
//
// Rop(D,S,P) = a + a D + a S + a P + a  DS + a  DP + a  SP + a   DSP
//               0   d     s     p     ds      dp      sp      dsp

#define AVEC_NOT    0x01
#define AVEC_D      0x02
#define AVEC_S      0x04
#define AVEC_P      0x08
#define AVEC_DS     0x10
#define AVEC_DP     0x20
#define AVEC_SP     0x40
#define AVEC_DSP    0x80

#define AVEC_NEED_SOURCE  (AVEC_S | AVEC_DS | AVEC_SP | AVEC_DSP)
#define AVEC_NEED_PATTERN (AVEC_P | AVEC_DP | AVEC_SP | AVEC_DSP)

#define BB_TARGET_SCREEN    0x0001
#define BB_TARGET_ONLY      0x0002
#define BB_SOURCE_COPY      0x0004
#define BB_PATTERN_COPY     0x0008


/******************************Public*Data*********************************\
* ROP translation table
*
* Translates the usual ternary rop into A-vector notation.  Each bit in
* this new notation corresponds to a term in a polynomial translation of
* the rop.
*
* Rop(D,S,P) = a + a D + a S + a P + a  DS + a  DP + a  SP + a   DSP
*               0   d     s     p     ds      dp      sp      dsp
\**************************************************************************/

BYTE gajRop[] =
{
    0x00, 0xff, 0xb2, 0x4d, 0xd4, 0x2b, 0x66, 0x99,
    0x90, 0x6f, 0x22, 0xdd, 0x44, 0xbb, 0xf6, 0x09,
    0xe8, 0x17, 0x5a, 0xa5, 0x3c, 0xc3, 0x8e, 0x71,
    0x78, 0x87, 0xca, 0x35, 0xac, 0x53, 0x1e, 0xe1,
    0xa0, 0x5f, 0x12, 0xed, 0x74, 0x8b, 0xc6, 0x39,
    0x30, 0xcf, 0x82, 0x7d, 0xe4, 0x1b, 0x56, 0xa9,
    0x48, 0xb7, 0xfa, 0x05, 0x9c, 0x63, 0x2e, 0xd1,
    0xd8, 0x27, 0x6a, 0x95, 0x0c, 0xf3, 0xbe, 0x41,
    0xc0, 0x3f, 0x72, 0x8d, 0x14, 0xeb, 0xa6, 0x59,
    0x50, 0xaf, 0xe2, 0x1d, 0x84, 0x7b, 0x36, 0xc9,
    0x28, 0xd7, 0x9a, 0x65, 0xfc, 0x03, 0x4e, 0xb1,
    0xb8, 0x47, 0x0a, 0xf5, 0x6c, 0x93, 0xde, 0x21,
    0x60, 0x9f, 0xd2, 0x2d, 0xb4, 0x4b, 0x06, 0xf9,
    0xf0, 0x0f, 0x42, 0xbd, 0x24, 0xdb, 0x96, 0x69,
    0x88, 0x77, 0x3a, 0xc5, 0x5c, 0xa3, 0xee, 0x11,
    0x18, 0xe7, 0xaa, 0x55, 0xcc, 0x33, 0x7e, 0x81,
    0x80, 0x7f, 0x32, 0xcd, 0x54, 0xab, 0xe6, 0x19,
    0x10, 0xef, 0xa2, 0x5d, 0xc4, 0x3b, 0x76, 0x89,
    0x68, 0x97, 0xda, 0x25, 0xbc, 0x43, 0x0e, 0xf1,
    0xf8, 0x07, 0x4a, 0xb5, 0x2c, 0xd3, 0x9e, 0x61,
    0x20, 0xdf, 0x92, 0x6d, 0xf4, 0x0b, 0x46, 0xb9,
    0xb0, 0x4f, 0x02, 0xfd, 0x64, 0x9b, 0xd6, 0x29,
    0xc8, 0x37, 0x7a, 0x85, 0x1c, 0xe3, 0xae, 0x51,
    0x58, 0xa7, 0xea, 0x15, 0x8c, 0x73, 0x3e, 0xc1,
    0x40, 0xbf, 0xf2, 0x0d, 0x94, 0x6b, 0x26, 0xd9,
    0xd0, 0x2f, 0x62, 0x9d, 0x04, 0xfb, 0xb6, 0x49,
    0xa8, 0x57, 0x1a, 0xe5, 0x7c, 0x83, 0xce, 0x31,
    0x38, 0xc7, 0x8a, 0x75, 0xec, 0x13, 0x5e, 0xa1,
    0xe0, 0x1f, 0x52, 0xad, 0x34, 0xcb, 0x86, 0x79,
    0x70, 0x8f, 0xc2, 0x3d, 0xa4, 0x5b, 0x16, 0xe9,
    0x08, 0xf7, 0xba, 0x45, 0xdc, 0x23, 0x6e, 0x91,
    0x98, 0x67, 0x2a, 0xd5, 0x4c, 0xb3, 0xfe, 0x01
};

BOOL b8BppHostToScrnCachedWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop);

BOOL b1BppHostToScrnCachedWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop);

BOOL b8BppHostToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop);

VOID vLowLevel8BppHostToScrnWithRop(
    PPDEV   ppdev,
    PPOINT  pptTrg,
    PSIZE   psizBlt,
    PWORD   pwFirstWord,
    INT     lSrcDelta,
    XLATEOBJ *pxlo,
    WORD    s3Rop);

BOOL b1BppHostToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop);

VOID vLowLevel1BppHostToScrnWithRop(
    PPDEV   ppdev,
    PPOINT  pptTrg,
    PSIZE   psizBlt,
    INT     xSrc,
    PWORD   pwFirstWord,
    INT     lSrcDelta,
    XLATEOBJ *pxlo,
    WORD    s3Rop);

BOOL b4BppHostToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop);


VOID vLowLevel4BppHostToScrnWithRop(
    PPDEV   ppdev,
    PPOINT  pptTrg,
    PSIZE   psizBlt,
    INT     xSrc,
    PWORD   pwFirstWord,
    INT     lSrcDelta,
    XLATEOBJ *pxlo,
    WORD    s3Rop);


WORD wXlateSrcRop3toS3Rop(PPDEV pdev, WORD rop3);

ROP4 bXlatePatRop4toS3Rop(
    PPDEV pdev,
    ROP4  rop4);

BOOL bDownLoadBrushIntoColorCache(
    PPDEV      ppdev,
    PS3BRUSH   ps3Brush,
    PPOINT     ppt);

BOOL bDownLoadBrushIntoMonoCache(
    PPDEV      ppdev,
    PS3BRUSH   p3Brush,
    PXYZPOINT  pxyzPt);

BOOL bExpandColorBrushIntoHorzCache(
    PPDEV      ppdev,
    PPOINT     ppt);

BOOL bExpandMonoBrushIntoHorzCache(
    PPDEV      ppdev,
    PXYZPOINT  pxyzPtl,
    INT        zHorz);

BOOL bExpandColorBrushIntoVertCache(
    PPDEV      ppdev,
    PPOINT     ppt);

BOOL bExpandMonoBrushIntoVertCache(
    PPDEV      ppdev,
    PXYZPOINT  pxyzPt,
    INT        zVert);

BOOL bColorHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop);

BOOL bColorExpandHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    INT         zHorz,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop);

BOOL bCpyColorHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest);

BOOL bCpyColorExpandHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    INT         zHorz,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest);

BOOL bColorVertCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop);

BOOL bColorExpandVertCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    INT         zHorz,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop);


BOOL bPuntBlit(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     rop4);

BOOL bScrnToScrnPuntBlit(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     rop4);


BOOL bSpecialBlits(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     rop4);

BOOL bScrnToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop);


VOID S3ScrnToScrnBlt(
    PPDEV   ppdev,
    INT     xSrc,
    INT     ySrc,
    INT     xTrg,
    INT     yTrg,
    INT     width,
    INT     height,
    WORD    s3Rop,
    WORD    Cmd);

BOOL bPatternSolid(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     rop4);

VOID vS3SolidPattern(
    PPDEV   ppdev,
    INT     left,
    INT     top,
    INT     width,
    INT     height,
    INT     color,
    WORD    s3Mix);


BOOL bPatternBrush(
    SURFOBJ  *psoTrg,
    CLIPOBJ  *pco,
    RECTL    *prclTrg,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     rop4);



/*****************************************************************************
 * S3 (Banked Frame Buffer) DrvCopyBits
 *
 *  An I/O through the transfer register take 43 clocks.
 *  A Memory access on the S3 takes 23 clocks, the memory window wins,
 *  so this transfers bits through the memory window for some things.
 ****************************************************************************/
BOOL DrvCopyBits(
    SURFOBJ  *psoDest,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclDest,
    POINTL   *pptlSrc)
{
#if defined(_X86_) || defined(i386)

    INT     cy;
    RECTL   rclScans;
    RECTL   rclDest, rclTmp;
    SURFOBJ *pso;
    PPDEV   ppdev;
    BOOL    bRet, bResetBounds;

    DISPDBG((3,"S3.DLL!DrvCopyBits - Entry\n"));

    bResetBounds = FALSE;

    rclDest = *prclDest;

    cy = prclDest->bottom - prclDest->top;

    if (psoDest->iType == STYPE_DEVICE)
    {
        ppdev = (PPDEV) psoDest->dhsurf;

        GPWAIT();

        if (pco == NULL)
            pco = ppdev->pcoDefault;

        if (psoSrc->iType == STYPE_DEVICE)
        {
            // At this point we know it's a screen to screen copy.
            // If a color translation needs to be applied then go through ScrnToScrnPuntBlit

            if ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL))
            {
                bScrnToScrnWithRop(psoDest, psoSrc, pco, prclDest, pptlSrc, OVERPAINT);
                return (TRUE);
            }
            else
            {
                bRet = bScrnToScrnPuntBlit(psoDest,
                                           psoSrc,
                                           NULL,
                                           pco,
                                           pxlo,
                                           prclDest,
                                           pptlSrc,
                                           NULL,
                                           NULL,
                                           NULL,
                                           0x0000CCCC);

                return (bRet);
            }
        }

        else if (psoSrc->lDelta < 0)
        {
            rclScans.top    = prclDest->top;
            rclScans.bottom = rclScans.top + cy;

            pso = ppdev->pSurfObj;
            psoDest = pso;
            bBankEnumStart(ppdev, &rclScans, pso, pco);
        }

        else
        {
            switch (psoSrc->iBitmapFormat)
            {
                case BMF_DEVICE:
                case BMF_8BPP:
                    bRet = b8BppHostToScrnCachedWithRop(psoDest, psoSrc,
                                                        pco, pxlo,
                                                        prclDest, pptlSrc,
                                                        OVERPAINT);
                    break;

                case BMF_1BPP:
                    bRet = b1BppHostToScrnCachedWithRop(psoDest, psoSrc,
                                                        pco, pxlo,
                                                        prclDest, pptlSrc,
                                                        OVERPAINT);
                    break;

                case BMF_4BPP:
                    bRet = b4BppHostToScrnWithRop(psoDest, psoSrc, pco, pxlo,
                                              prclDest, pptlSrc, OVERPAINT);
                    break;

                case BMF_16BPP:
                case BMF_24BPP:
                case BMF_32BPP:
                case BMF_4RLE:
                case BMF_8RLE:
                default:
                    bRet = FALSE;
            }

            if (bRet == TRUE)
            {
                return (TRUE);
            }

            rclScans.top    = prclDest->top;
            rclScans.bottom = rclScans.top + cy;

            pso = ppdev->pSurfObj;
            psoDest = pso;
            bBankEnumStart(ppdev, &rclScans, pso, pco);
        }
    }
    else
    {
        rclScans.top    = pptlSrc->y;
        rclScans.bottom = pptlSrc->y + cy;

        ppdev = (PPDEV) psoSrc->dhsurf;

        GPWAIT();

        if (pco == NULL)
        {
            pco = ppdev->pcoDefault;

            // BUGBUG: This mucking around with pcoDefaults's rclBounds is
            // no longer required by bSrcBankEnumStart:

            rclTmp = pco->rclBounds;
            pco->rclBounds = *prclDest;
            bResetBounds = TRUE;
        }

        pso = ppdev->pSurfObj;
        psoSrc = pso;
        bSrcBankEnumStart(ppdev, &rclScans, pso, pco, prclDest);
    }

    do
    {
        bRet = EngCopyBits(psoDest, psoSrc, pco, pxlo, &rclDest, pptlSrc);

    } while (bRet && (bBankEnum(ppdev, &rclScans, pso, pco)));

    bBankEnumEnd(ppdev, pso, pco);

    if (bResetBounds)
    {
        ppdev->pcoDefault->rclBounds = rclTmp;
    }

    return(bRet);

#else

    // A DrvCopyBits is just a simplified DrvBitBlt:

    return(DrvBitBlt(psoDest,
                     psoSrc,
                     NULL,              // psoMask
                     pco,
                     pxlo,
                     prclDest,
                     pptlSrc,
                     NULL,              // pptlMask
                     NULL,              // pbo
                     NULL,              // pptlBrush
                     0x0000CCCC));      // mix = SRCCOPY

#endif
}




/*****************************************************************************
 * S3 DrvBitBlt
 ****************************************************************************/
BOOL DrvBitBlt(
SURFOBJ  *psoTrg,
SURFOBJ  *psoSrc,
SURFOBJ  *psoMask,
CLIPOBJ  *pco,
XLATEOBJ *pxlo,
RECTL  *prclTrg,
POINTL    *pptlSrc,
POINTL    *pptlMask,
BRUSHOBJ *pbo,
POINTL    *pptlBrush,
ROP4    rop4)

{
    BOOL    b;
    PPDEV   ppdev;

    DISPDBG((3,"S3.DLL!DrvBitBlt - Entry\n"));

    // Since we don't support Device-Format Bitmaps, either the source
    // or the destination (or both) will be the device surface:

    if (psoTrg->iType == STYPE_DEVICE)
        ppdev = (PPDEV) psoTrg->dhpdev;
    else
        ppdev = (PPDEV) psoSrc->dhpdev;

    // Protect the driver from a potentially NULL clip object.

    if (pco == NULL)
    {
        pco = ppdev->pcoDefault;
    }

    if (bTest == FALSE)
    {
        b = bSpecialBlits(psoTrg, psoSrc, psoMask,
                          pco, pxlo,
                          prclTrg, pptlSrc, pptlMask,
                          pbo, pptlBrush,
                          rop4);
    }
    else
    {
        b = FALSE;
    }

    if (b != TRUE)
    {
        bPuntBlit(psoTrg,
                  psoSrc,
                  psoMask,
                  pco,
                  pxlo,
                  prclTrg,
                  pptlSrc,
                  pptlMask,
                  pbo,
                  pptlBrush,
                  rop4);
    }

    return (TRUE);
}

/*****************************************************************************
 * S3 General purpose blit handler.  This routine will handle any blit.
 * Albeit slow, but it will be handled.
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL bPuntBlit(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     rop4)
{


#if defined(_X86_) || defined(i386)

    RECTL   rclScans;
    RECTL   rclTrg;
    PPDEV   ppdevTrg, ppdevSrc, ppdevBank;
    SURFOBJ *psoBank;
    BOOL    bRet;
    PPDEV   ppdev;

    DISPDBG((2,"S3.DLL!bPuntBlit - Entry\n"));

    if (psoTrg->iType == STYPE_DEVICE)
        ppdev = (PPDEV) psoTrg->dhsurf;
    else
        ppdev = (PPDEV) psoSrc->dhsurf;

    // Wait for the S3 coprocessor to finish with the VRAM before we touch it through
    // the apperature.

    GPWAIT();

    rclTrg = *prclTrg;

    rclScans.top    = prclTrg->top;
    rclScans.bottom = prclTrg->bottom;

    // Get the correct surface object for the target and the source

    if ((psoTrg->iType == STYPE_DEVICE) &&
        ((psoSrc == NULL) || (psoSrc->iType != STYPE_DEVICE)))
    {
        ppdevTrg  = (PPDEV) psoTrg->dhsurf;
        psoTrg    = ppdevTrg->pSurfObj;

        bBankEnumStart(ppdevTrg, &rclScans, psoTrg, pco);
        ppdevBank = ppdevTrg;
        psoBank = psoTrg;

    }

    else if ((psoTrg->iType != STYPE_DEVICE) &&
             (psoSrc != NULL) && (psoSrc->iType == STYPE_DEVICE))
    {
        ppdevSrc  = (PPDEV) psoSrc->dhsurf;
        psoSrc    = ppdevSrc->pSurfObj;

        rclScans.top    = pptlSrc->y;
        rclScans.bottom = pptlSrc->y + (prclTrg->bottom - prclTrg->top);

        bSrcBankEnumStart(ppdevSrc, &rclScans, psoSrc, pco, prclTrg);
        ppdevBank = ppdevSrc;
        psoBank = psoSrc;

    }
    else
    {
        bRet = bScrnToScrnPuntBlit(psoTrg,
                                   psoSrc,
                                   psoMask,
                                   pco,
                                   pxlo,
                                   prclTrg,
                                   pptlSrc,
                                   pptlMask,
                                   pbo,
                                   pptlBrush,
                                   rop4);
        return (bRet);
    }

    do
    {
        bRet = EngBitBlt(psoTrg,
                  psoSrc,
                  psoMask,
                  pco,
                  pxlo,
                  &rclTrg,
                  pptlSrc,
                  pptlMask,
                  pbo,
                  pptlBrush,
                  rop4);

    } while ((bBankEnum(ppdevBank, &rclScans, psoBank, pco)) && bRet);

    bBankEnumEnd(ppdevBank, psoBank, pco);

    return bRet;

#else

    BOOL    bRet;
    PPDEV   ppdev;
    RECTL   rclSrc;

    if (psoTrg->iType == STYPE_DEVICE)
    {
        ppdev = (PPDEV) psoTrg->dhsurf;

        if ((psoSrc != NULL) && (psoSrc->iType == STYPE_DEVICE))
        {
            // -------------------------------------------------------
            // Handle screen-to-screen blit:

            // We have to copy both the source rectangle and the destination
            // rectangle to the temporary bitmap (the destination rectangle
            // is needed if there's complex clipping or if there's a ROP that
            // affects need the destination).
            //
            // We simply copy the smallest entire rectangle containing both
            // the source and the destination rectangles:

            rclSrc.left   = min(pptlSrc->x, prclTrg->left);
            rclSrc.top    = min(pptlSrc->y, prclTrg->top);
            rclSrc.right  = max(pptlSrc->x + prclTrg->right - prclTrg->left,
                                prclTrg->right);
            rclSrc.bottom = max(pptlSrc->y + prclTrg->bottom - prclTrg->top,
                                prclTrg->bottom);

            vPuntGetBits(ppdev, psoSrc, &rclSrc);

            // Now do the copy entirely on the temporary bitmap surface:

            bRet = EngBitBlt(ppdev->psoTemp,
                     ppdev->psoTemp,
                     psoMask,
                     pco,
                     pxlo,
                     prclTrg,
                     pptlSrc,
                     pptlMask,
                     pbo,
                     pptlBrush,
                     rop4);

            // Just have to copy the destination back to the surface:

            if (bRet)
            {
                vPuntPutBits(ppdev, psoTrg, prclTrg);
            }
        }
        else
        {
            // -------------------------------------------------------
            // Handle bitmap-to-screen blit:

            // Make a copy of the destination rectangle in case the ROP
            // modifies the destination:

            vPuntGetBits(ppdev, psoTrg, prclTrg);

            // Do the blit operation on the temporary bitmap:

            bRet = EngBitBlt(ppdev->psoTemp,
                     psoSrc,
                     psoMask,
                     pco,
                     pxlo,
                     prclTrg,
                     pptlSrc,
                     pptlMask,
                     pbo,
                     pptlBrush,
                     rop4);

            // Copy everything back to the surface:

            if (bRet)
            {
                vPuntPutBits(ppdev, psoTrg, prclTrg);
            }
        }
    }
    else
    {
        // -------------------------------------------------------
        // Handle screen-to-bitmap blit:

        ppdev = (PPDEV) psoSrc->dhsurf;

        // Copy the source rectangle to the temporary bitmap, then get
        // GDI to blit to the target surface:

        rclSrc.left   = pptlSrc->x;
        rclSrc.top    = pptlSrc->y;
        rclSrc.right  = pptlSrc->x + (prclTrg->right - prclTrg->left);
        rclSrc.bottom = pptlSrc->y + (prclTrg->bottom - prclTrg->top);

        vPuntGetBits(ppdev, psoSrc, &rclSrc);

        bRet = EngBitBlt(psoTrg,
                 ppdev->psoTemp,
                 psoMask,
                 pco,
                 pxlo,
                 prclTrg,
                 pptlSrc,
                 pptlMask,
                 pbo,
                 pptlBrush,
                 rop4);
    }

    vResetS3Clipping(ppdev);    // Always reset after using vPunt routines

    return(bRet);

#endif
}

/******************************************************************************
 * bScrnToScrnPuntBlit - Screen To Screen Punt Blit
 *****************************************************************************/
BOOL bScrnToScrnPuntBlit(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     rop4)
{

    SIZEL       sizl;
    HSURF       hsurfBm;
    SURFOBJ     *psoTempSrc;
    WORD        cmd;
    INT         i, cxInWords;
    PBYTE       pb;
    PWORD       pw;
    PPDEV       ppdev;
    RECTL       rclScans, rclTrg;
    BOOL        bRet;
    POINTL      ptlTempSrc;

    DISPDBG((2,"S3.DLL!bScrnToScrnPuntBlit - Entry\n"));

    // Pickup our pdev and a pointer to the surface object.

    ppdev  = (PPDEV) psoTrg->dhsurf;
    psoTrg = ppdev->pSurfObj;

    // Set the default bounding rectangle for the bank manager.

    rclTrg = *prclTrg;

    // Create a temporary surface and bitmap.

    sizl.cx = prclTrg->right  - prclTrg->left;
    sizl.cy = prclTrg->bottom - prclTrg->top;

    hsurfBm = (HSURF) EngCreateBitmap(sizl,
                                      sizl.cx,
                                      BMF_8BPP,
                                      BMF_TOPDOWN,
                                      NULL);
    if (hsurfBm == NULL)
    {
        DISPDBG((0, "S3.DLL!bScrnToScrnPuntBlit - EngCreateBitmap failed\n"));
        return(FALSE);
    }

    // Get a pointer to the surface object.

    psoTempSrc = EngLockSurface(hsurfBm);

    // Copy the bits into it.

    if (!(sizl.cx & 0x01))
    {
        cmd =   RECTANGLE_FILL     | BYTE_SWAP      | BUS_SIZE_16 |
                DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
                DRAW               | LAST_PIXEL_ON  | READ;
    }
    else
    {
        cmd =   RECTANGLE_FILL     | BUS_SIZE_8 |
                DRAWING_DIR_TBLRXM | DIR_TYPE_XY    | WAIT |
                DRAW               | LAST_PIXEL_ON  | READ;
    }

    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_RD_MASK(0xff);
    OUTPW (MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW (CUR_X, pptlSrc->x);
    OUTPW (CUR_Y, pptlSrc->y);
    OUTPW (RECT_WIDTH, (sizl.cx - 1));
    OUTPW (MULTIFUNC_CNTL, (RECT_HEIGHT | (sizl.cy - 1)));

    GPWAIT();

    OUTPW (CMD, cmd);

    // Wait for the Data Available.

    while (!(inpw(GP_STAT) & READ_DATA_AVAILABLE));

    // Now transfer the data from the screen to the host memory bitmap.

    if (!(sizl.cx & 0x01))
    {
        cxInWords = (sizl.cx + 1) / 2;
        pw = (PWORD) psoTempSrc->pvScan0;
        for (i = 0; i < sizl.cy; i++)
        {
            vDataPortIn(ppdev, pw, cxInWords);
            ((PBYTE) pw) += psoTempSrc->lDelta;
        }
    }
    else
    {
        pb = psoTempSrc->pvScan0;
        for (i = 0; i < sizl.cy; i++)
        {
            vDataPortInB(ppdev, pb, sizl.cx);
            pb += psoTempSrc->lDelta;
        }
    }

    // Set the new source position

    ptlTempSrc.x = 0;
    ptlTempSrc.y = 0;

    // Do the banked blit operation.

    rclScans.top    = prclTrg->top;
    rclScans.bottom = prclTrg->bottom;

    bBankEnumStart(ppdev, &rclScans, psoTrg, pco);

    do
    {
        bRet = EngBitBlt(psoTrg,
                  psoTempSrc,
                  psoMask,
                  pco,
                  pxlo,
                  &rclTrg,
                  &ptlTempSrc,
                  pptlMask,
                  pbo,
                  pptlBrush,
                  rop4);

    } while ((bBankEnum(ppdev, &rclScans, psoTrg, pco)) && bRet);

    bBankEnumEnd(ppdev, psoTrg, pco);

    // free the temp bitmap.

    EngUnlockSurface(psoTempSrc);
    EngDeleteSurface(hsurfBm);

    return (bRet);

}

/*****************************************************************************
 * S3 Special case Blit handler
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL bSpecialBlits(
    SURFOBJ     *psoTrg,
    SURFOBJ     *psoSrc,
    SURFOBJ     *psoMask,
    CLIPOBJ     *pco,
    XLATEOBJ    *pxlo,
    RECTL   *prclTrg,
    POINTL  *pptlSrc,
    POINTL  *pptlMask,
    BRUSHOBJ    *pbo,
    POINTL  *pptlBrush,
    ROP4  rop4)
{
    BOOL    bRet;
    PPDEV   ppdev;
    ROP4    s3Rop4;
    WORD    avecRop, s3Rop;
    BRUSHOBJ bo;

    bRet = FALSE;

    if (psoTrg->iType == STYPE_DEVICE)
        ppdev = (PPDEV) psoTrg->dhsurf;
    else
        ppdev = (PPDEV) psoSrc->dhsurf;

    // NOTE: If the ForeRop and BackRop are the same implicitly
    //       there is no mask.

    // First test for copy opperations.

    if (rop4 == 0x0000CCCC)
    {
        if ((psoTrg->iType == STYPE_DEVICE) && (psoSrc->iType == STYPE_DEVICE))
        {
            if ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL))
            {
                bRet = bScrnToScrnWithRop(psoTrg, psoSrc, pco, prclTrg, pptlSrc, OVERPAINT);
                return(bRet);
            }
            else
            {
                return (FALSE);
            }
        }

    }

    // Check for a mask.  If there is one, at this time, reject the
    // acceleration.

    if (psoMask != NULL)
    {
        return (FALSE);
    }

    // If the background and foreground rops are not the same
    // reject the blit.

    if (((rop4 & 0xFF00) >> 8) != (rop4 & 0xff))
    {
        return (FALSE);
    }

    // Pickup the avec for some quick decision later.

    avecRop = gajRop[(rop4 & 0xff)];

    // Check for a DIB.  If this is a DIB reject the blit.
    // BUGBUG: At some future date (post BETA2) this should be optimized.

    if ((avecRop & AVEC_NEED_SOURCE) && (psoSrc->lDelta < 0))
    {
        return (FALSE);
    }

    // Check for whitness or blackness

    if (((rop4 & 0xff) == 0xff) ||
        ((rop4 & 0xff) == 0x00))
    {
        if ((rop4 & 0xff) == 0xff)
            s3Rop4 = LOGICAL_1;
        else
            s3Rop4 = LOGICAL_0;

        bRet = bPatternSolid(psoTrg, psoSrc, psoMask,
                          pco, pxlo,
                          prclTrg, pptlSrc, pptlMask,
                          pbo, pptlBrush,
                          s3Rop4);
        return (bRet);
    }

    // Check for Dest Invert

    if ((rop4 & 0xff) == 0x55)
    {
        s3Rop4 = SCREEN_XOR_NEW;
        bo.iSolidColor = 0xffff;

        bRet = bPatternSolid(psoTrg, psoSrc, psoMask,
                          pco, pxlo,
                          prclTrg, pptlSrc, pptlMask,
                          &bo, pptlBrush,
                          s3Rop4);
        return (bRet);

    }


    // Check for brushes.

    if ((avecRop & AVEC_NEED_PATTERN) && (!(avecRop & AVEC_NEED_SOURCE)))
    {
        // Translate the rop from GDI to S3.

        s3Rop4 = bXlatePatRop4toS3Rop(ppdev, rop4);

        // Check for a Solid Brush.

        if (pbo == NULL || pbo->iSolidColor != -1)
        {
            bRet = bPatternSolid(psoTrg, psoSrc, psoMask,
                              pco, pxlo,
                              prclTrg, pptlSrc, pptlMask,
                              pbo, pptlBrush,
                              s3Rop4);
        }

        // Handle this as a Pattern Brush.

        else
        {
            bRet = bPatternBrush(psoTrg, pco, prclTrg, pbo, pptlBrush, s3Rop4);
        }

    }

    // Check if we may be able to optimize for screen to screen or
    // host to screen blits.

    else if ((!(avecRop & AVEC_NEED_PATTERN)) &&
            (avecRop & AVEC_NEED_SOURCE))
    {
        s3Rop = wXlateSrcRop3toS3Rop(ppdev, LOWORD(rop4));
        if (s3Rop != 0)
        {
            if ((psoTrg->iType == STYPE_DEVICE) &&
                (psoSrc->iType == STYPE_DEVICE) &&
                ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL)))
            {
                bRet = bScrnToScrnWithRop(psoTrg, psoSrc, pco, prclTrg,
                                       pptlSrc, s3Rop);
            }
            else if (psoSrc->iType == STYPE_BITMAP)
            {
                switch (psoSrc->iBitmapFormat)
                {
                    case BMF_DEVICE:
                    case BMF_8BPP:
                        bRet = b8BppHostToScrnCachedWithRop(psoTrg,
                                                            psoSrc,
                                                            pco,
                                                            pxlo,
                                                            prclTrg,
                                                            pptlSrc,
                                                            s3Rop);
                        break;

                    case BMF_1BPP:
                        bRet = b1BppHostToScrnCachedWithRop(psoTrg,
                                                            psoSrc,
                                                            pco,
                                                            pxlo,
                                                            prclTrg,
                                                            pptlSrc,
                                                            s3Rop);
                        break;

                    case BMF_4BPP:
                        bRet = b4BppHostToScrnWithRop(psoTrg,
                                                      psoSrc,
                                                      pco,
                                                      pxlo,
                                                      prclTrg,
                                                      pptlSrc,
                                                      s3Rop);
                        break;

                    case BMF_16BPP:
                    case BMF_24BPP:
                    case BMF_32BPP:
                    case BMF_4RLE:
                    case BMF_8RLE:
                    default:
                        bRet = FALSE;
                        break;
                }
            }
        }

        else
        {
            DISPDBG((0,"S3.DLL!bSpecialBlits - Missed SrcBlt opportunity - rop: 0x%x\n",
                        rop4));
        }
    }

    else if ((rop4 & 0xff) == 0xc0)     // Merge Copy
    {
        if ((psoTrg->iType == STYPE_DEVICE) &&
            (psoSrc->iType == STYPE_DEVICE) &&
            ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL)))

        {
            bRet = bScrnToScrnWithRop(psoTrg, psoSrc,
                                      pco, prclTrg, pptlSrc,
                                      OVERPAINT);

            if (bRet)
            {
                if (pbo == NULL || pbo->iSolidColor != -1)
                {
                    bRet = bPatternSolid(psoTrg, psoSrc, psoMask,
                                      pco, pxlo,
                                      prclTrg, pptlSrc, pptlMask,
                                      pbo, pptlBrush,
                                      SCREEN_AND_NEW);
                }
                else
                {
                    bRet = bPatternBrush(psoTrg, pco, prclTrg,
                                         pbo, pptlBrush,
                                         (SCREEN_AND_NEW << 8) | SCREEN_AND_NEW);
                }
            }
        }
        else if (psoSrc->iType == STYPE_BITMAP)
        {
            switch (psoSrc->iBitmapFormat)
            {
                case BMF_DEVICE:
                case BMF_8BPP:
                    bRet = b8BppHostToScrnCachedWithRop(psoTrg,
                                                        psoSrc,
                                                        pco,
                                                        pxlo,
                                                        prclTrg,
                                                        pptlSrc,
                                                        OVERPAINT);
                    break;

                case BMF_1BPP:
                    bRet = b1BppHostToScrnCachedWithRop(psoTrg,
                                                        psoSrc,
                                                        pco,
                                                        pxlo,
                                                        prclTrg,
                                                        pptlSrc,
                                                        OVERPAINT);
                    break;

                case BMF_4BPP:
                    bRet = b4BppHostToScrnWithRop(psoTrg,
                                                  psoSrc,
                                                  pco,
                                                  pxlo,
                                                  prclTrg,
                                                  pptlSrc,
                                                  OVERPAINT);
                    break;

                case BMF_16BPP:
                case BMF_24BPP:
                case BMF_32BPP:
                case BMF_4RLE:
                case BMF_8RLE:
                default:
                    bRet = FALSE;
                    break;
            }

            if (bRet)
            {
                if (pbo == NULL || pbo->iSolidColor != -1)
                {
                    bRet = bPatternSolid(psoTrg, psoSrc, psoMask,
                                      pco, pxlo,
                                      prclTrg, pptlSrc, pptlMask,
                                      pbo, pptlBrush,
                                      SCREEN_AND_NEW);
                }
                else
                {
                    bRet = bPatternBrush(psoTrg, pco, prclTrg,
                                         pbo, pptlBrush,
                                         (SCREEN_AND_NEW << 8) | SCREEN_AND_NEW);
                }
            }
        }
    }



    return (bRet);
}


/*****************************************************************************
 * S3 Screen to Screen with a Rop
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL bScrnToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop)
{
    INT     width, height, xTrg, yTrg, xSrc, ySrc, cx, cy;
    WORD    cmd, wDirCode;
    RECTL   rclSrc, rclTrg;
    ULONG   DirClip;
    BOOL    bMore, bIntersect, bClipRequired;
    UINT    i;
    ENUMRECTS8   EnumRects8;
    PPDEV   ppdev;
    RECTL   rclBounds;
    BYTE    iDComplexity;

    // Get the pdev.

    ppdev = (PPDEV) psoTrg->dhpdev;

    // There are two completely different code paths.
    // for the complex and non-complex clip cases.
    // This is because most of the direction and intersection
    // calculations need to be done after the clipping.

    if ((iDComplexity = pco->iDComplexity) != DC_COMPLEX)
    {
        // Make a copy of the target, since we will have to change
        // it for clipping.

        rclTrg = *prclTrg;

        if (iDComplexity == DC_RECT)
        {
            rclBounds = pco->rclBounds;

            // First handle the trivial rejection.

            bClipRequired = bIntersectTest(&rclTrg, &rclBounds);

            // define the clipped target rectangle.

            if (bClipRequired)
            {
                rclTrg.left   = max (rclTrg.left, rclBounds.left);
                rclTrg.top    = max (rclTrg.top, rclBounds.top);
                rclTrg.right  = min (rclTrg.right, rclBounds.right);
                rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);
            }
            else
            {
                // The destination rectangle is completely clipped out,
                // so just return.

                return (TRUE);
            }
        }

        // define the cx & cy.

        width  = (rclTrg.right  - rclTrg.left) - 1;
        height = (rclTrg.bottom - rclTrg.top)  - 1;

        // calculate the cx & cy shift for the source.

        cx = rclTrg.left - prclTrg->left;
        cy = rclTrg.top  - prclTrg->top;

        // define the source rectangle.

        rclSrc.left   = pptlSrc->x + cx;
        rclSrc.top    = pptlSrc->y + cy;
        rclSrc.right  = pptlSrc->x + width + 1;
        rclSrc.bottom = pptlSrc->y + height + 1;

        // Determine the direction of the blit.
        // First, check for an intersection of the souce and dest rects.

        bIntersect = bIntersectTest(&rclTrg, &rclSrc);

        // Set defaults for the S3 blit direction and the
        // initial coordinates

        wDirCode = DRAWING_DIR_TBLRXM;

        xTrg = rclTrg.left;
        yTrg = rclTrg.top;
        xSrc = rclSrc.left;
        ySrc = rclSrc.top;

        // If the source & dest rects intersect adjust the
        // blit direction and initial coordinates.

        if (bIntersect)
        {
            // The horizontal copy direction.

            if (rclTrg.left > rclSrc.left)
            {
                // R to L

                xTrg = rclTrg.right - 1;
                xSrc = rclSrc.left + width;
                wDirCode &= ~PLUS_X;
            }

            // The vertical copy direction.

            if (rclTrg.top > rclSrc.top)
            {
                // B to T

                yTrg = rclTrg.bottom - 1;
                ySrc = rclSrc.top + height;
                wDirCode &= ~PLUS_Y;
            }
        }

        // Create the S3 Command.

        cmd  = BITBLT | DRAW | DIR_TYPE_XY | WRITE;
        cmd |= wDirCode;

        // Do the blit.

        S3ScrnToScrnBlt(ppdev, xSrc, ySrc, xTrg, yTrg, width, height, s3Rop, cmd);

    }
    else
    {
        // This a complex clip.

        DirClip  = CD_ANY;
        wDirCode = DRAWING_DIR_TBLRXM;

        if (prclTrg->left > pptlSrc->x)
            wDirCode &= ~PLUS_X;

        if (prclTrg->top > pptlSrc->y)
            wDirCode &= ~PLUS_Y;

        switch (wDirCode)
        {
            case DRAWING_DIR_TBLRXM:
                DirClip = CD_RIGHTDOWN;
                break;

            case DRAWING_DIR_TBRLXM:
                DirClip = CD_LEFTDOWN;
                break;

            case DRAWING_DIR_BTLRXM:
                DirClip = CD_RIGHTUP;
                break;

            case DRAWING_DIR_BTRLXM:
                DirClip = CD_LEFTUP;
                break;
        }

        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, DirClip, 0);

        do
        {
            bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8),
                                  (PULONG) &EnumRects8);

            for (i = 0; i < EnumRects8.c; i++)
            {
                rclTrg    = *prclTrg;
                rclBounds = EnumRects8.arcl[i];

                // handle the trivial rejection.

                bClipRequired = bIntersectTest(&rclTrg, &rclBounds);

                // define the clipped target rectangle.

                if (bClipRequired)
                {
                    rclTrg.left   = max (rclTrg.left, rclBounds.left);
                    rclTrg.top    = max (rclTrg.top, rclBounds.top);
                    rclTrg.right  = min (rclTrg.right, rclBounds.right);
                    rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);
                }
                else
                {
                    // The destination rectangle is completely clipped out,
                    // so use as an early out.

                    continue;
                }

                // define the cx & cy.

                width  = (rclTrg.right  - rclTrg.left) - 1;
                height = (rclTrg.bottom - rclTrg.top)  - 1;

                // calculate the cx & cy shift for the source.

                cx = rclTrg.left - prclTrg->left;
                cy = rclTrg.top  - prclTrg->top;

                // define the source rectangle.

                rclSrc.left   = pptlSrc->x  + cx;
                rclSrc.top    = pptlSrc->y  + cy;
                rclSrc.right  = rclSrc.left + width + 1;
                rclSrc.bottom = rclSrc.top  + height + 1;

                // Determine the direction of the blit.
                // First, check for an intersection of the souce and dest rects.

                bIntersect = bIntersectTest(&rclTrg, &rclSrc);

                // Set defaults for the S3 blit direction and the
                // initial coordinates

                wDirCode = DRAWING_DIR_TBLRXM;

                xTrg = rclTrg.left;
                yTrg = rclTrg.top;
                xSrc = rclSrc.left;
                ySrc = rclSrc.top;

                // If the source & dest rects intersect adjust the
                // blit direction and initial coordinates.

                if (bIntersect)
                {
                    // The horizontal copy direction.

                    if (rclTrg.left > rclSrc.left)
                    {
                        // R to L

                        xTrg = rclTrg.right - 1;
                        xSrc = rclSrc.left + width;
                        wDirCode &= ~PLUS_X;
                    }

                    // The vertical copy direction.

                    if (rclTrg.top > rclSrc.top)
                    {
                        // B to T

                        yTrg = rclTrg.bottom - 1;
                        ySrc = rclSrc.top + height;
                        wDirCode &= ~PLUS_Y;
                    }
                }

                // Create the S3 Command.

                cmd  = BITBLT | DRAW | DIR_TYPE_XY | WRITE;
                cmd |= wDirCode;

                // Do the blit.

                S3ScrnToScrnBlt(ppdev, xSrc, ySrc, xTrg, yTrg, width, height, s3Rop, cmd);

            }

        } while (bMore);
    }
    return(TRUE);
}


/*****************************************************************************
 * S3ScrnToScrnBlt
 ****************************************************************************/
VOID S3ScrnToScrnBlt(
    PPDEV   ppdev,
    INT     xSrc,
    INT     ySrc,
    INT     xTrg,
    INT     yTrg,
    INT     width,
    INT     height,
    WORD    s3Rop,
    WORD    cmd)
{

#if DBG

    if (cmd & 0x20)
    {
        ASSERTS3(((xTrg + width) < (INT) ppdev->cxScreen),
                 "S3.DLL!S3ScrnToScrnBlt - Blit Operation over right edge (1)\n");
    }

#endif
    // This is where the actual S3 registers are set.

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | s3Rop);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, width);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | height));
    OUTPW(CUR_X, xSrc);
    OUTPW(CUR_Y, ySrc);
    OUTPW(DEST_X, xTrg);
    OUTPW(DEST_Y, yTrg);

    OUTPW(CMD, cmd);
}


/*****************************************************************************
 * S3 Brush Pattern
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL bPatternBrush(
    SURFOBJ  *psoTrg,
    CLIPOBJ  *pco,
    RECTL    *prclTrg,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4      s3Rop4)           // Note that the least significant byte is
                                // the S3 foreground ROP, and the next byte
                                // is the S3 background ROP.  So you can't
                                // just pass in a plain S3 ROP constant!

{
    PPDEV       ppdev;
    PS3BRUSH    ps3Brush;
    POINT       pt;
    XYZPOINT    xyzPt;
    INT         zHorz, zVert, i;
    POINT       ptDest;
    SIZE        sizDest;
    WORD        s3ForeRop, s3BackRop;
    BOOL        bClipRequired, bMore;
    RECTL       rclTrg, rclBounds;
    BYTE        iDComplexity;
    ENUMRECTS8  EnumRects8;

    DISPDBG((3, "S3.DLL!bPatternBrush - Entry\n"));

    // Get the pdev.

    ppdev = (PPDEV) psoTrg->dhpdev;

    // Get the pointer to our drivers realization of the brush.

    if (pbo->pvRbrush != NULL)
    {
        ps3Brush = pbo->pvRbrush;
    }
    else
    {
        ps3Brush = BRUSHOBJ_pvGetRbrush(pbo);

        // Fail if we do not handle the brush.

        if (ps3Brush == NULL)
            return (FALSE);
    }

    s3ForeRop = (WORD) (s3Rop4 & 0xFF);
    s3BackRop = (WORD) ((s3Rop4 >> 8) & 0xFF);

    if (ps3Brush->iBitmapFormat == BMF_1BPP)
    {
        DISPDBG((3, "S3.DLL!bPatternBrush - 1BPP brush\n"));

        // If we have never seen this brush before then allocate a slot
        // for it.

        // Keep a count of number of brushes we have seen.
        // This will be used to determine if the cache system is adaquate.

        nMonoBrushes++;

        // If we have never seen this brush before or if the cache has
        // changed since the last time
        // we have seen it then allocate a new cache entry for it.

        if ((ps3Brush->iBrushCacheID == -1) ||
            (ps3Brush->iPatternID != ppdev->pulMonoBrushCacheEntries[ps3Brush->iBrushCacheID]))
        {
            // If we have run out of cache entries invalidate the entire cache.

            if (ppdev->iNextMonoBrushCacheSlot >= ppdev->iMaxCachedMonoBrushes)
            {
                nMonoBrushCacheInvalidations++;

                memset(ppdev->pulMonoBrushCacheEntries, 0, ppdev->iMaxCachedMonoBrushes * sizeof(ULONG));
                ppdev->iNextMonoBrushCacheSlot = 0;
            }

            // Get a slot in the cache for this brush.

            i = (ppdev->iNextMonoBrushCacheSlot)++;
            ps3Brush->iBrushCacheID = i;

            // Calculate the address of the brush in the cache.

            xyzPt.x = MONO_PATTERN_CACHE_X + ((i >>  3) * 16);
            xyzPt.y = MONO_PATTERN_CACHE_Y;
            xyzPt.z = ajMonoPatPlanes[i & 0x7];

            // Associate the brush with this cache entry.

            ppdev->pulMonoBrushCacheEntries[i] = ps3Brush->iPatternID;

            // Get an expansion cache slot for this brush.

            i = ppdev->iNextMonoBrushExpansionSlot;
            i = ++i % MAX_MONO_BRUSH_EXPANSION_SLOTS;
            ppdev->iNextMonoBrushExpansionSlot = i;
            ps3Brush->iExpansionCacheID = i;

            // Associate this brush with this expansion cache slot.

            ppdev->aulMonoExpansionCacheTag[i] = ps3Brush->iPatternID;

            // Set the expansion caches planes

            zHorz = zVert = ajMonoPatPlanes[i];

            // Down load the brush to the cache and expand it into
            // the horizontal and vertical caches.

            bDownLoadBrushIntoMonoCache(ppdev, ps3Brush, &xyzPt);
            bExpandMonoBrushIntoHorzCache(ppdev, &xyzPt, zHorz);
            bExpandMonoBrushIntoVertCache(ppdev, &xyzPt, zVert);

        }
        else
        {
            // We got a cache hit, so keep the statistic.

            nMonoBrushCacheHit++;

            // Calculate the address of the brush in the cache.

            i = ps3Brush->iBrushCacheID;
            xyzPt.x = MONO_PATTERN_CACHE_X + ((i >>  3) * 16);
            xyzPt.y = MONO_PATTERN_CACHE_Y;
            xyzPt.z = ajMonoPatPlanes[i & 0x7];

            // Check for an hit in the expansion cache.

            if (ps3Brush->iPatternID == ppdev->aulMonoExpansionCacheTag[(i = ps3Brush->iExpansionCacheID)])
            {
                zHorz = zVert = ajMonoPatPlanes[i];
                nMonoBrushExpansionCacheHit++;
            }
            else
            {
                // Get an expansion cache slot for this brush.

                i = ppdev->iNextMonoBrushExpansionSlot;
                i = ++i % MAX_MONO_BRUSH_EXPANSION_SLOTS;
                ppdev->iNextMonoBrushExpansionSlot = i;
                ps3Brush->iExpansionCacheID = i;

                // Associate this brush with this expansion cache slot.

                ppdev->aulMonoExpansionCacheTag[i] = ps3Brush->iPatternID;

                // Set the expansion caches planes

                zHorz = zVert = ajMonoPatPlanes[i];

                bExpandMonoBrushIntoHorzCache(ppdev, &xyzPt, zHorz);
                bExpandMonoBrushIntoVertCache(ppdev, &xyzPt, zVert);
            }
        }
    }
    else
    {
        DISPDBG((3, "S3.DLL!bPatternBrush - 8BPP brush\n"));

        // If we have never seen this brush before then allocate a slot
        // for it.

        // Keep a count of number of brushes we have seen.
        // This will be used to determine if the cache system is adaquate.

        nColorBrushes++;

        // If we have never seen this brush before or if the cache
        // has changed since the last time
        // we have seen it then allocate a new cache entry for it.

        if ((ps3Brush->iBrushCacheID == -1) ||
            (ps3Brush->iPatternID != ppdev->pulColorBrushCacheEntries[ps3Brush->iBrushCacheID]))
        {
            // If we have run out of cache entries invalidate the entire cache.

            if (ppdev->iNextColorBrushCacheSlot >= ppdev->iMaxCachedColorBrushes)
            {
                nColorBrushCacheInvalidations++;

                memset(ppdev->pulColorBrushCacheEntries, 0, ppdev->iMaxCachedColorBrushes * sizeof(ULONG));
                ppdev->iNextColorBrushCacheSlot = 0;
            }

            // Get a slot in the cache for this brush.

            i = (ppdev->iNextColorBrushCacheSlot)++;
            ps3Brush->iBrushCacheID = i;

            // Calculate the address of the brush in the cache.

            pt.x = COLOR_PATTERN_CACHE_X + ((i >>  2) * 16);
            pt.y = COLOR_PATTERN_CACHE_Y + ((i & 0x3) * 16);

            // Associate the brush with this cache entry.

            ppdev->pulColorBrushCacheEntries[i] = ps3Brush->iPatternID;
            ppdev->ulColorExpansionCacheTag     = ps3Brush->iPatternID;

            // Down load the brush to the cache and expand it into
            // the horizontal and vertical caches.

            bDownLoadBrushIntoColorCache(ppdev, ps3Brush, &pt);
            bExpandColorBrushIntoHorzCache(ppdev, &pt);
            bExpandColorBrushIntoVertCache(ppdev, &pt);
        }
        else
        {
            // We got a cache hit, so keep the statistic.

            nColorBrushCacheHit++;

            // Calculate the address of the brush in the cache.

            i = ps3Brush->iBrushCacheID;
            pt.x = COLOR_PATTERN_CACHE_X + ((i >>  2) * 16);
            pt.y = COLOR_PATTERN_CACHE_Y + ((i & 0x3) * 16);

            if (ps3Brush->iPatternID == ppdev->ulColorExpansionCacheTag)
            {
                nColorBrushExpansionCacheHit++;
            }
            else
            {
                ppdev->ulColorExpansionCacheTag = ps3Brush->iPatternID;

                bExpandColorBrushIntoHorzCache(ppdev, &pt);
                bExpandColorBrushIntoVertCache(ppdev, &pt);
            }
        }
    }

    // Handle the clipping.

    if ((iDComplexity = pco->iDComplexity) != DC_COMPLEX)
    {
        if (iDComplexity == DC_TRIVIAL)
        {
            rclTrg = *prclTrg;
        }
        else
        {
            rclTrg    = *prclTrg;
            rclBounds = pco->rclBounds;

            // First handle the trivial rejection.

            bClipRequired = bIntersectTest(&rclTrg, &rclBounds);

            if (bClipRequired)
            {
                rclTrg.left   = max (rclTrg.left,  rclBounds.left);
                rclTrg.top    = max (rclTrg.top,   rclBounds.top);
                rclTrg.right  = min(rclTrg.right,  rclBounds.right);
                rclTrg.bottom = min(rclTrg.bottom, rclBounds.bottom);
            }
            else
            {
                // The brush is completely clipped out.
                // so just return sucessfull.

                return (TRUE);
            }
        }

        ptDest.x   = rclTrg.left;
        ptDest.y   = rclTrg.top;
        sizDest.cx = rclTrg.right  - rclTrg.left;
        sizDest.cy = rclTrg.bottom - rclTrg.top;

        // Color expand the monochrome brush in the Hoizontal Cache to the
        // screen.

        if (s3ForeRop == OVERPAINT && s3BackRop == OVERPAINT)
        {
            if (ps3Brush->iBitmapFormat == BMF_1BPP)
            {
                bCpyColorExpandHorzCacheToScreen(ppdev, ps3Brush, zHorz,
                                                 (PPOINT) pptlBrush,
                                                 &ptDest, &sizDest);
            }
            else
            {
                // NOTE: The S3-911/924 requries the cx of any "rolling blt"
                //       to have a cx of >= 64 pels.  This is due to how the
                //       chip's internal FIFO works.  This test will catch all
                //       the souce copy operations that can be accelerated,
                //       with the rolling blit.

                if ((sizDest.cx >= 64) ||
                    (sizDest.cx > sizDest.cy))
                {
                    bCpyColorHorzCacheToScreen(ppdev, ps3Brush,
                                               (PPOINT) pptlBrush,
                                               &ptDest, &sizDest);
                }
                else
                {
                    bColorVertCacheToScreen(ppdev, ps3Brush,
                                            (PPOINT) pptlBrush,
                                            &ptDest, &sizDest,
                                            s3ForeRop, s3BackRop);
                }
            }
        }
        else
        {
            if (sizDest.cy > sizDest.cx)
            {
                if (ps3Brush->iBitmapFormat == BMF_1BPP)
                {
                    bColorExpandVertCacheToScreen(ppdev, ps3Brush, zVert,
                                                  (PPOINT) pptlBrush,
                                                  &ptDest, &sizDest,
                                                  s3ForeRop, s3BackRop);
                }
                else
                {
                    bColorVertCacheToScreen(ppdev, ps3Brush,
                                            (PPOINT) pptlBrush,
                                            &ptDest, &sizDest,
                                            s3ForeRop, s3BackRop);
                }
            }
            else
            {
                if (ps3Brush->iBitmapFormat == BMF_1BPP)
                {
                    bColorExpandHorzCacheToScreen(ppdev, ps3Brush, zHorz,
                                                  (PPOINT) pptlBrush,
                                                  &ptDest, &sizDest,
                                                  s3ForeRop, s3BackRop);
                }
                else
                {
                    bColorHorzCacheToScreen(ppdev, ps3Brush,
                                            (PPOINT) pptlBrush,
                                            &ptDest, &sizDest,
                                            s3ForeRop, s3BackRop);
                }
            }
        }
    }
    else
    {
        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do
        {
            bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8),
                                  (PULONG) &EnumRects8);

            for (i = 0; i < (INT) EnumRects8.c; i++)
            {
                rclBounds = EnumRects8.arcl[i];

                if (rclBounds.left < prclTrg->left)
                    rclBounds.left = prclTrg->left;

                if (rclBounds.right > prclTrg->right)
                    rclBounds.right = prclTrg->right;

                if (rclBounds.top < prclTrg->top)
                    rclBounds.top = prclTrg->top;

                if (rclBounds.bottom > prclTrg->bottom)
                    rclBounds.bottom = prclTrg->bottom;

                // Get the height and width for the blit.

                ptDest.x   = rclBounds.left;
                ptDest.y   = rclBounds.top;
                sizDest.cx = rclBounds.right -  rclBounds.left;
                sizDest.cy = rclBounds.bottom - rclBounds.top;

                // Color expand the monochrome brush in the Hoizontal Cache
                // to the screen.

                if (s3ForeRop == OVERPAINT && s3BackRop == OVERPAINT)
                {
                    if (ps3Brush->iBitmapFormat == BMF_1BPP)
                    {
                        bCpyColorExpandHorzCacheToScreen(ppdev, ps3Brush, zHorz,
                                                         (PPOINT) pptlBrush,
                                                         &ptDest, &sizDest);

                    }
                    else
                    {
                        if ((sizDest.cx >= 64) ||
                            (sizDest.cx > sizDest.cy))
                        {
                            bCpyColorHorzCacheToScreen(ppdev, ps3Brush,
                                                       (PPOINT) pptlBrush,
                                                       &ptDest, &sizDest);
                        }
                        else
                        {
                            bColorVertCacheToScreen(ppdev, ps3Brush,
                                                    (PPOINT) pptlBrush,
                                                    &ptDest, &sizDest,
                                                    s3ForeRop, s3BackRop);
                        }
                    }
                }
                else
                {
                    if (sizDest.cy > sizDest.cx)
                    {
                        if (ps3Brush->iBitmapFormat == BMF_1BPP)
                        {
                            bColorExpandVertCacheToScreen(ppdev, ps3Brush, zVert,
                                                          (PPOINT) pptlBrush,
                                                          &ptDest, &sizDest,
                                                          s3ForeRop, s3BackRop);
                        }
                        else
                        {
                            bColorVertCacheToScreen(ppdev, ps3Brush,
                                                    (PPOINT) pptlBrush,
                                                    &ptDest, &sizDest,
                                                    s3ForeRop, s3BackRop);
                        }
                    }
                    else
                    {
                        if (ps3Brush->iBitmapFormat == BMF_1BPP)
                        {
                            bColorExpandHorzCacheToScreen(ppdev, ps3Brush, zHorz,
                                                          (PPOINT) pptlBrush,
                                                          &ptDest, &sizDest,
                                                          s3ForeRop, s3BackRop);
                        }
                        else
                        {
                            bColorHorzCacheToScreen(ppdev, ps3Brush,
                                                    (PPOINT) pptlBrush,
                                                    &ptDest, &sizDest,
                                                    s3ForeRop, s3BackRop);
                        }

                    }
                }
            }
        } while (bMore);

    }

    return (TRUE);

}

/*****************************************************************************
 * wXlateSrcRop3toS3Rop - Translate the Source to Dest Rop 3 to an S3 rop
 ****************************************************************************/
WORD wXlateSrcRop3toS3Rop(PPDEV pdev, WORD rop3)
{
WORD    s3Rop;

    switch (LOBYTE(rop3))
    {
        case 0xBB:                      // DSno
            s3Rop = SCREEN_OR_NOT_NEW;
            break;

        case 0x33:                      // Sn
            s3Rop = NOT_NEW;
            break;

        case 0x44:                      // SDna
            s3Rop = NOT_SCREEN_AND_NEW;
            break;

        case 0x66:                      // DSx
            s3Rop = SCREEN_XOR_NEW;
            break;

        case 0x77:                      // DSan
            s3Rop = NOT_SCREEN_OR_NOT_NEW;
            break;

        case 0x88:                      // DSa
            s3Rop = SCREEN_AND_NEW;
            break;

        case 0x99:                      // DSxn
            s3Rop = NOT_SCREEN_XOR_NEW;
            break;

        case 0xDD:                      // SDno
            s3Rop = NOT_SCREEN_OR_NEW;
            break;

        case 0xCC:                      // S
            s3Rop = OVERPAINT;
            break;

        case 0x11:                      // DSon
            s3Rop = NOT_SCREEN_AND_NOT_NEW;
            break;

        case 0x22:                      // DSna
            s3Rop = SCREEN_AND_NOT_NEW;
            break;

        case 0xEE:                      // SDo
            s3Rop = SCREEN_OR_NEW;
            break;

        default:
            s3Rop = 0;
            break;
    }

    return (s3Rop);

}


/*****************************************************************************
 * Translate the ROP4 to S3 rops.
 ****************************************************************************/

ROP4 bXlatePatRop4toS3Rop(
    PPDEV pdev,
    ROP4  rop4)
{
    WORD    s3Rop;
    ROP4    s3Rop4;

    switch (rop4 & 0xff)
    {
        case 0x00:                      // 0
            s3Rop = LOGICAL_0;
            break;

        case 0x05:                      // ~(P | D)
            s3Rop = NOT_SCREEN_AND_NOT_NEW;
            break;

        case 0x0A:                      // ~P & D
            s3Rop = SCREEN_AND_NOT_NEW;
            break;

        case 0x0F:                      // ~P
            s3Rop = NOT_NEW;
            break;

        case 0x50:                      // P & ~D
            s3Rop = NOT_SCREEN_AND_NEW;
            break;

        case 0x55:                      // ~D
            s3Rop = NOT_SCREEN;
            break;

        case 0x5A:                      // P ^ D
            s3Rop = SCREEN_XOR_NEW;
            break;

        case 0x5F:                      // ~(P & D)
            s3Rop = NOT_SCREEN_OR_NOT_NEW;
            break;

        case 0xA0:                      // P & D
            s3Rop = SCREEN_AND_NEW;
            break;

        case 0xA5:                      // ~(P ^ D)
            s3Rop = NOT_SCREEN_XOR_NEW;
            break;

        case 0xAA:                      // D
            s3Rop = LEAVE_ALONE;
            break;

        case 0xAF:                      // ~P | D
            s3Rop = SCREEN_OR_NOT_NEW;
            break;

        case 0xF0:                      // P
            s3Rop = OVERPAINT;
            break;

        case 0xF5:                      // P | ~D
            s3Rop = NOT_SCREEN_OR_NEW;
            break;

        case 0xFA:                      // P | D
            s3Rop = SCREEN_OR_NEW;
            break;

        case 0xFF:                      // 1
            s3Rop = LOGICAL_1;
            break;
    }

    s3Rop4 = s3Rop | (s3Rop << 8);

    return(s3Rop4);
}

/*****************************************************************************
 * Download the Color Brush to the Color brush cache in
 * graphics memory.
 ****************************************************************************/
BOOL bDownLoadBrushIntoColorCache(
    PPDEV      ppdev,
    PS3BRUSH   ps3Brush,
    PPOINT     ppt)
{
    WORD    Cmd;

    DISPDBG((3, "S3.DLL!bDownLoadBrushIntoColorCache - Entry\n"));

    // Down load the initial pattern image, the upper left 8 X 8 cell.

    Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | WRITE;

    // Setup the S3 chip.

    FIFOWAIT(FIFO_6_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | OVERPAINT);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW(CUR_X, ppt->x);
    OUTPW(CUR_Y, ppt->y);

    OUTPW(RECT_WIDTH, 7);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 7));

    GPWAIT();
    OUTPW(CMD, Cmd);

    // Now transfer the data from host memory to graphics memory.

    CHECK_DATA_READY;

    vDataPortOutB(ppdev, ps3Brush->ajPattern, 64);

    CHECK_DATA_COMPLETE;

    // Make the pattern double wide. Make copy of the pattern to the right
    // of the original pattern.

    Cmd  = BITBLT          | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS | DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_8_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW(CUR_X,  ppt->x);
    OUTPW(CUR_Y,  ppt->y);
    OUTPW(DEST_X, ppt->x + 8);
    OUTPW(DEST_Y, ppt->y);

    OUTPW(CMD, Cmd);

    // Make the pattern double high. Make a copy of the double pattern
    // pattern below the original one.

    FIFOWAIT(FIFO_6_EMPTY);

    OUTPW(RECT_WIDTH, 15);

    OUTPW(CUR_X,  ppt->x);
    OUTPW(CUR_Y,  ppt->y);
    OUTPW(DEST_X, ppt->x);
    OUTPW(DEST_Y, ppt->y + 8);

    OUTPW(CMD, Cmd);

    return(TRUE);
}



/*****************************************************************************
 * Download the Monochrome Brush to the Monochrome brush cache in
 * graphics memory.
 ****************************************************************************/
BOOL bDownLoadBrushIntoMonoCache(
    PPDEV      ppdev,
    PS3BRUSH   p3Brush,
    PXYZPOINT  pxyzPt)
{
    WORD    Cmd;
    INT     i;
    BYTE    ajPattern[8];

    DISPDBG((3, "S3.DLL!bDownLoadBrushIntoMonoCache - Entry\n"));

    // Compress the dword aligned pattern to a byte aligned pattern.

    for (i = 0; i < 8; i++)
    {
        ajPattern[i] = p3Brush->ajPattern[i * p3Brush->lDeltaPattern];
    }

    // Down load the initial pattern image, the upper left 8 X 8 cell.

    Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | MULTIPLE_PIXELS    | WRITE;

    // Setup the S3 chip.

    FIFOWAIT(FIFO_8_EMPTY);

    TEST_AND_SET_FRGD_MIX(LOGICAL_1);
    TEST_AND_SET_BKGD_MIX(LOGICAL_0);

    TEST_AND_SET_WRT_MASK(LOWORD(pxyzPt->z));

    OUTPW(CUR_X, pxyzPt->x);
    OUTPW(CUR_Y, pxyzPt->y);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | CPU_DATA));
    OUTPW(RECT_WIDTH, 7);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 7));

    GPWAIT();
    OUTPW(CMD, Cmd);

    // Now transfer the data from host memory to graphics memory.

    CHECK_DATA_READY;

    vDataPortOutB(ppdev, ajPattern, 8);

    CHECK_DATA_COMPLETE;

    // Make the pattern double wide. Make copy of the pattern to the right
    // of the original pattern.

    Cmd  = BITBLT          | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS | DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_8_EMPTY);

    TEST_AND_SET_RD_MASK (LOWORD(pxyzPt->z));
    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW(CUR_X,  pxyzPt->x);
    OUTPW(CUR_Y,  pxyzPt->y);
    OUTPW(DEST_X, pxyzPt->x + 8);
    OUTPW(DEST_Y, pxyzPt->y);

    OUTPW(CMD, Cmd);

    // Make the pattern double high. Make a copy of the double pattern
    // pattern below the original one.

    FIFOWAIT(FIFO_6_EMPTY);

    OUTPW(RECT_WIDTH, 15);

    OUTPW(CUR_X,  pxyzPt->x);
    OUTPW(CUR_Y,  pxyzPt->y);
    OUTPW(DEST_X, pxyzPt->x);
    OUTPW(DEST_Y, pxyzPt->y + 8);

    OUTPW(CMD, Cmd);

    // Reset the read and write masks.

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);
    TEST_AND_SET_RD_MASK(0xff);

    return(TRUE);
}


/*****************************************************************************
 * Expand the color brush in the vertical dimension.
 ****************************************************************************/
BOOL bExpandColorBrushIntoVertCache(
    PPDEV      ppdev,
    PPOINT     ppt)
{
    WORD    Cmd;

    DISPDBG((3, "S3.DLL!bExpandColorBrushIntoVertCache - Entry\n"));

    // Copy the cached double wide, double high pattern to the Horizontal
    // expansion cache area.

    Cmd  = BITBLT          | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS | DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_1_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);

    FIFOWAIT(FIFO_8_EMPTY);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 15));
    OUTPW(RECT_WIDTH, 15);

    OUTPW(CUR_X,  ppt->x);
    OUTPW(CUR_Y,  ppt->y);
    OUTPW(DEST_X, COLOR_VERT_EXPANSION_CACHE_X);
    OUTPW(DEST_Y, COLOR_VERT_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Now the rolling blit to fill all the way.

    FIFOWAIT(FIFO_6_EMPTY);

    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | COLOR_VERT_EXPANSION_CACHE_CY - 17));

    OUTPW(CUR_X,  COLOR_VERT_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  COLOR_VERT_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, COLOR_VERT_EXPANSION_CACHE_X);
    OUTPW(DEST_Y, COLOR_VERT_EXPANSION_CACHE_Y + 16);

    OUTPW(CMD, Cmd);

    return(TRUE);

}



/*****************************************************************************
 * Expand the monochrome brush in the vertical dimension.
 ****************************************************************************/
BOOL bExpandMonoBrushIntoVertCache(
    PPDEV      ppdev,
    PXYZPOINT  pxyzPt,
    INT        zVert)
{
    WORD    Cmd;

    DISPDBG((3, "S3.DLL!bExpandMonoBrushIntoVertCache - Entry\n"));

    // Copy the cached double wide, double high pattern to the Horizontal
    // expansion cache area.

    Cmd  = BITBLT          | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS | DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | OVERPAINT);
    TEST_AND_SET_FRGD_COLOR(0xff);
    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | OVERPAINT);
    SET_BKGD_COLOR(0);

    TEST_AND_SET_RD_MASK(LOWORD(pxyzPt->z));
    TEST_AND_SET_WRT_MASK(LOWORD(zVert));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, 15);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 15));

    OUTPW(CUR_X,  pxyzPt->x);
    OUTPW(CUR_Y,  pxyzPt->y);
    OUTPW(DEST_X, MONO_VERT_EXPANSION_CACHE_X);
    OUTPW(DEST_Y, MONO_VERT_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Now the rolling blit to fill all the way.

    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_RD_MASK(zVert);

    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | MONO_VERT_EXPANSION_CACHE_CY - 17));

    OUTPW(CUR_X,  MONO_VERT_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  MONO_VERT_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, MONO_VERT_EXPANSION_CACHE_X);
    OUTPW(DEST_Y, MONO_VERT_EXPANSION_CACHE_Y + 16);

    OUTPW(CMD, Cmd);

    // Now reset the read and write plan masks.

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);
    TEST_AND_SET_RD_MASK(0xff);

    return(TRUE);

}

/*****************************************************************************
 * Expand the color brush in the horizontal dimension.
 ****************************************************************************/
BOOL bExpandColorBrushIntoHorzCache(
    PPDEV      ppdev,
    PPOINT     ppt)
{
    WORD    Cmd;

    DISPDBG((3, "S3.DLL!bExpandColorBrushIntoHorzCache - Entry\n"));

    // Copy the cached double wide, double high pattern to the Horizontal
    // expansion cache area.

    Cmd  = BITBLT          | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS | DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_1_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);

    FIFOWAIT(FIFO_8_EMPTY);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 15));
    OUTPW(RECT_WIDTH, 15);

    OUTPW(CUR_X,  ppt->x);
    OUTPW(CUR_Y,  ppt->y);
    OUTPW(DEST_X, COLOR_HORZ_EXPANSION_CACHE_X);
    OUTPW(DEST_Y, COLOR_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Expand the 16 X 16 double wide, double high.
    // First to a 32 X 16

    FIFOWAIT(FIFO_5_EMPTY);

    OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, COLOR_HORZ_EXPANSION_CACHE_X + 16);
    OUTPW(DEST_Y, COLOR_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Now 64 X 16

    FIFOWAIT(FIFO_6_EMPTY);

    OUTPW(RECT_WIDTH, 31);

    OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, COLOR_HORZ_EXPANSION_CACHE_X + 32);
    OUTPW(DEST_Y, COLOR_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Now the rolling blit to fill all the way to 512 pels.

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, COLOR_HORZ_EXPANSION_CACHE_CX - 65);

    OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, COLOR_HORZ_EXPANSION_CACHE_X + 64);
    OUTPW(DEST_Y, COLOR_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    return(TRUE);
}




/*****************************************************************************
 * Expand the monochrome brush in the horizontal dimension.
 ****************************************************************************/
BOOL bExpandMonoBrushIntoHorzCache(
    PPDEV      ppdev,
    PXYZPOINT  pxyzPt,
    INT        zHorz)
{
    WORD    Cmd;

    DISPDBG((3, "S3.DLL!bExpandMonoBrushIntoHorzCache - Entry\n"));

    // Copy the cached double wide, double high pattern to the Horizontal
    // expansion cache area.

    Cmd  = BITBLT          | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS | DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | OVERPAINT);
    TEST_AND_SET_FRGD_COLOR(0xff);
    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | OVERPAINT);
    SET_BKGD_COLOR(0);

    TEST_AND_SET_RD_MASK(LOWORD(pxyzPt->z));
    TEST_AND_SET_WRT_MASK(LOWORD(zHorz));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, 15);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 15));

    OUTPW(CUR_X,  pxyzPt->x);
    OUTPW(CUR_Y,  pxyzPt->y);
    OUTPW(DEST_X, MONO_HORZ_EXPANSION_CACHE_X);
    OUTPW(DEST_Y, MONO_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Expand the 16 X 16 double wide, double high.
    // First to a 32 X 16

    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_RD_MASK(LOWORD(zHorz));

    OUTPW(RECT_WIDTH, 15);

    OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, MONO_HORZ_EXPANSION_CACHE_X + 16);
    OUTPW(DEST_Y, MONO_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Now 64 X 16

    FIFOWAIT(FIFO_6_EMPTY);

    OUTPW(RECT_WIDTH, 31);

    OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, MONO_HORZ_EXPANSION_CACHE_X + 32);
    OUTPW(DEST_Y, MONO_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Now the rolling blit to fill all the way to 512 pels.

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, MONO_HORZ_EXPANSION_CACHE_CX - 65);

    OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
    OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y);
    OUTPW(DEST_X, MONO_HORZ_EXPANSION_CACHE_X + 64);
    OUTPW(DEST_Y, MONO_HORZ_EXPANSION_CACHE_Y);

    OUTPW(CMD, Cmd);

    // Now reset the read and write plan masks.

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);
    TEST_AND_SET_RD_MASK(0xff);

    return(TRUE);
}


/*****************************************************************************
 * Color brush in the Vertical Cache to the screen.
 ****************************************************************************/
BOOL bColorVertCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop)
{
    WORD    Cmd;
    INT     ixBlits, ixLast,
            iyFirst, iyBlits, iyLast,
            xOffset, yOffset,
            i, j;

    DISPDBG((3, "S3.DLL!bColorVertCacheToScreen - Entry\n"));

    // Take into account the pptBrushOrg.

    xOffset = pptDest->x - pptBrushOrg->x;
    yOffset = pptDest->y - pptBrushOrg->y;

    if (xOffset < 0)
        xOffset  = 8 - (-xOffset % 8);
    else
        xOffset %= 8;

    if (yOffset < 0)
        yOffset  = 8 - (-yOffset % 8);
    else
        yOffset %= 8;

    // Color Expand the monochrome vertical cache to the screen.
    // This is done in a loop to handle the general case ROPs.

    if (psizDest->cy < 8 - yOffset)
    {
        iyFirst = psizDest->cy;
        iyBlits = 0;
        iyLast  = 0;

    }
    else if (psizDest->cy - yOffset < COLOR_VERT_EXPANSION_CACHE_CY - 8)
    {
        iyFirst = 8 - yOffset;
        iyBlits = 0;
        iyLast  = psizDest->cy - iyFirst;
    }
    else
    {
        iyFirst = 8 - yOffset;
        iyBlits = (psizDest->cy - iyFirst) / (COLOR_VERT_EXPANSION_CACHE_CY - 8);
        iyLast  = (psizDest->cy - iyFirst) % (COLOR_VERT_EXPANSION_CACHE_CY - 8);
    }

    ixBlits = psizDest->cx / 8;
    ixLast  = psizDest->cx % 8;

    Cmd  = BITBLT         | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS| DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_4_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | s3ForeRop);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 7));
    OUTPW(RECT_WIDTH, 7);

    for(i = 0; i < ixBlits; i++)
    {
        ASSERTS3(((pptDest->x + (i * 8) + 7) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorVertCacheToScreen - Blit Operation over right edge (1)\n");

        if (iyFirst != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyFirst - 1));

            OUTPW(CUR_X,  COLOR_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  COLOR_VERT_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, pptDest->x + (i * 8));
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }

        if (iyBlits != 0)
        {
            FIFOWAIT(FIFO_1_EMPTY);
            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (COLOR_VERT_EXPANSION_CACHE_CY - 8) - 1));
        }

        for (j = 0; j <iyBlits; j++)
        {
            FIFOWAIT(FIFO_5_EMPTY);

            OUTPW(CUR_X,  COLOR_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  COLOR_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (i * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (j * (COLOR_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }

        if (iyLast != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyLast - 1));

            OUTPW(CUR_X,  COLOR_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  COLOR_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (i * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (iyBlits * (COLOR_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }
    }

    if (ixLast != 0)
    {
        FIFOWAIT(FIFO_1_EMPTY);
        OUTPW(RECT_WIDTH, ixLast - 1);

        ASSERTS3(((pptDest->x + (ixBlits * 8) + (ixLast - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorVertCacheToScreen - Blit Operation over right edge (2)\n");

        if (iyFirst != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyFirst - 1));

            OUTPW(CUR_X,  COLOR_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  COLOR_VERT_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, pptDest->x + (ixBlits * 8));
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }

        if (iyBlits != 0)
        {
            FIFOWAIT(FIFO_1_EMPTY);
            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (COLOR_VERT_EXPANSION_CACHE_CY - 8) - 1));
        }

        for (j = 0; j <iyBlits; j++)
        {
            FIFOWAIT(FIFO_5_EMPTY);

            OUTPW(CUR_X,  COLOR_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  COLOR_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (ixBlits * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (j * (COLOR_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }

        if (iyLast != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyLast - 1));

            OUTPW(CUR_X,  COLOR_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  COLOR_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (ixBlits * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (iyBlits * (COLOR_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }
    }

    return(TRUE);
}

/*****************************************************************************
 * Color expand the monochrome brush in the Vertical Cache to the screen.
 ****************************************************************************/
BOOL bColorExpandVertCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    INT         zVert,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop)
{
    WORD    Cmd;
    INT     ixBlits, ixLast,
            iyFirst, iyBlits, iyLast,
            xOffset, yOffset,
            i, j;

    DISPDBG((3, "S3.DLL!bColorExpandVertCacheToScreen - Entry\n"));

    // Take into account the pptBrushOrg.

    xOffset = pptDest->x - pptBrushOrg->x;
    yOffset = pptDest->y - pptBrushOrg->y;

    if (xOffset < 0)
        xOffset  = 8 - (-xOffset % 8);
    else
        xOffset %= 8;

    if (yOffset < 0)
        yOffset  = 8 - (-yOffset % 8);
    else
        yOffset %= 8;

    // Color Expand the monochrome vertical cache to the screen.
    // This is done in a loop to handle the general case ROPs.

    if (psizDest->cy < 8 - yOffset)
    {
        iyFirst = psizDest->cy;
        iyBlits = 0;
        iyLast  = 0;

    }
    else if (psizDest->cy - yOffset < MONO_VERT_EXPANSION_CACHE_CY - 8)
    {
        iyFirst = 8 - yOffset;
        iyBlits = 0;
        iyLast  = psizDest->cy - iyFirst;
    }
    else
    {
        iyFirst = 8 - yOffset;
        iyBlits = (psizDest->cy - iyFirst) / (MONO_VERT_EXPANSION_CACHE_CY - 8);
        iyLast  = (psizDest->cy - iyFirst) % (MONO_VERT_EXPANSION_CACHE_CY - 8);
    }

    ixBlits = psizDest->cx / 8;
    ixLast  = psizDest->cx % 8;

    Cmd  = BITBLT         | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS| DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_RD_MASK(zVert);

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | s3ForeRop);
    TEST_AND_SET_FRGD_COLOR(LOWORD(ps3Brush->ulForeColor));

    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | s3BackRop);
    SET_BKGD_COLOR(LOWORD(ps3Brush->ulBackColor));

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

    OUTPW(RECT_WIDTH, 7);

    for(i = 0; i < ixBlits; i++)
    {
        ASSERTS3(((pptDest->x + (i * 8) + 7) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorExpandVertCacheToScreen - Blit Operation over right edge (1)\n");

        if (iyFirst != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyFirst - 1));

            OUTPW(CUR_X,  MONO_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  MONO_VERT_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, pptDest->x + (i * 8));
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }

        if (iyBlits != 0)
        {
            FIFOWAIT(FIFO_1_EMPTY);
            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (MONO_VERT_EXPANSION_CACHE_CY - 8) - 1));
        }

        for (j = 0; j <iyBlits; j++)
        {
            FIFOWAIT(FIFO_5_EMPTY);

            OUTPW(CUR_X,  MONO_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  MONO_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (i * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (j * (MONO_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }

        if (iyLast != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyLast - 1));

            OUTPW(CUR_X,  MONO_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  MONO_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (i * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (iyBlits * (MONO_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }
    }

    if (ixLast != 0)
    {
        FIFOWAIT(FIFO_1_EMPTY);
        OUTPW(RECT_WIDTH, ixLast - 1);

        ASSERTS3(((pptDest->x + (ixBlits * 8) + (ixLast - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorExpandVertCacheToScreen - Blit Operation over right edge (2)\n");

        if (iyFirst != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyFirst - 1));

            OUTPW(CUR_X,  MONO_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  MONO_VERT_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, pptDest->x + (ixBlits * 8));
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }

        if (iyBlits != 0)
        {
            FIFOWAIT(FIFO_1_EMPTY);
            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (MONO_VERT_EXPANSION_CACHE_CY - 8) - 1));
        }

        for (j = 0; j <iyBlits; j++)
        {
            FIFOWAIT(FIFO_5_EMPTY);

            OUTPW(CUR_X,  MONO_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  MONO_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (ixBlits * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (j * (MONO_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }

        if (iyLast != 0)
        {
            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyLast - 1));

            OUTPW(CUR_X,  MONO_VERT_EXPANSION_CACHE_X + xOffset);
            OUTPW(CUR_Y,  MONO_VERT_EXPANSION_CACHE_Y);
            OUTPW(DEST_X, pptDest->x + (ixBlits * 8));
            OUTPW(DEST_Y, pptDest->y + iyFirst +
                          (iyBlits * (MONO_VERT_EXPANSION_CACHE_CY - 8)));

            OUTPW(CMD, Cmd);
        }
    }

    // Reset the Read Mask

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);
    TEST_AND_SET_RD_MASK(0xff);

    return(TRUE);
}


/*****************************************************************************
 * Color brush in the Hoizontal Cache to the screen.
 ****************************************************************************/
BOOL bColorHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop)
{
    WORD    Cmd;
    INT     xLeft, cxLeft, cxRight, ixBlits, i, j, x, cx,
            iyBlits, iyLast,
            xOffset, yOffset;

    DISPDBG((3, "S3.DLL!bColorHorzCacheToScreen - Entry\n"));

    // Take into account the pptBrushOrg.

    xOffset = pptDest->x - pptBrushOrg->x;
    yOffset = pptDest->y - pptBrushOrg->y;

    if (xOffset < 0)
        xOffset  = 8 - (-xOffset % 8);
    else
        xOffset %= 8;

    if (yOffset < 0)
        yOffset  = 8 - (-yOffset % 8);
    else
        yOffset %= 8;

    // Color Expand the monochrome horizontal cache to the screen.
    // This is done in a loop to handle the general case ROPs.

    xLeft  = pptDest->x;
    cxLeft = COLOR_HORZ_EXPANSION_CACHE_CX - xOffset;

    if ((cx = psizDest->cx - cxLeft) < 0)
    {
        cxLeft  = psizDest->cx;
        ixBlits = 0;
        cxRight = 0;
    }
    else
    {
        ixBlits = cx / COLOR_HORZ_EXPANSION_CACHE_CX;
        cxRight = cx % COLOR_HORZ_EXPANSION_CACHE_CX;
    }

    iyBlits = psizDest->cy / 8;
    iyLast  = psizDest->cy % 8;

    Cmd  = BITBLT         | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS| DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_3_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | s3ForeRop);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 7));

    for (i = 0; i < iyBlits; i++)
    {
        ASSERTS3(((pptDest->x + (cxLeft - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorHorzCacheToScreen - Blit Operation over right edge (1)\n");

        FIFOWAIT(FIFO_6_EMPTY);

        OUTPW(RECT_WIDTH, cxLeft - 1);

        OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X + xOffset);
        OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
        OUTPW(DEST_X, pptDest->x);
        OUTPW(DEST_Y, pptDest->y + (i * 8));

        OUTPW(CMD, Cmd);

        // If the the cxDest is wider than the cxHorzCache then fill in the
        // right side of the fill area.

        if (cx > 0)
        {
            FIFOWAIT(FIFO_1_EMPTY);
            OUTPW(RECT_WIDTH, COLOR_HORZ_EXPANSION_CACHE_CX - 1);

            for (j = 0; j < ixBlits; j++)
            {
                x = pptDest->x + cxLeft + (j * COLOR_HORZ_EXPANSION_CACHE_CX);

                ASSERTS3(((x + (COLOR_HORZ_EXPANSION_CACHE_CX - 1)) < (INT) ppdev->cxScreen),
                          "S3.DLL!bColorHorzCacheToScreen - Blit Operation over right edge (2)\n");

                FIFOWAIT(FIFO_5_EMPTY);

                OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
                OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
                OUTPW(DEST_X, x);
                OUTPW(DEST_Y, pptDest->y + (i * 8));

                OUTPW(CMD, Cmd);
            }

            if (cxRight != 0)
            {
                x = pptDest->x + cxLeft + (ixBlits * COLOR_HORZ_EXPANSION_CACHE_CX);

                ASSERTS3(((x + (cxRight - 1)) < (INT) ppdev->cxScreen),
                          "S3.DLL!bColorHorzCacheToScreen - Blit Operation over right edge (3)\n");

                FIFOWAIT(FIFO_6_EMPTY);

                OUTPW(RECT_WIDTH, cxRight - 1);

                OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
                OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
                OUTPW(DEST_X, x);
                OUTPW(DEST_Y, pptDest->y + (i * 8));

                OUTPW(CMD, Cmd);
            }
        }
    }

    if (iyLast != 0)
    {
        ASSERTS3(((pptDest->x + (cxLeft - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorHorzCacheToScreen - Blit Operation over right edge (4)\n");

        FIFOWAIT(FIFO_7_EMPTY);

        OUTPW(RECT_WIDTH, cxLeft - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyLast - 1));

        OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X + xOffset);
        OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
        OUTPW(DEST_X, pptDest->x);
        OUTPW(DEST_Y, pptDest->y + (iyBlits * 8));

        OUTPW(CMD, Cmd);

        // If the the cxDest is wider than the cxHorzCache then fill in the
        // right side of the fill area.

        if (cx > 0)
        {
            FIFOWAIT(FIFO_1_EMPTY);
            OUTPW(RECT_WIDTH, COLOR_HORZ_EXPANSION_CACHE_CX - 1);

            for (j = 0; j < ixBlits; j++)
            {
                x = pptDest->x + cxLeft + (j * COLOR_HORZ_EXPANSION_CACHE_CX);

                ASSERTS3(((x + (COLOR_HORZ_EXPANSION_CACHE_CX - 1)) < (INT) ppdev->cxScreen),
                          "S3.DLL!bColorHorzCacheToScreen - Blit Operation over right edge (5)\n");

                FIFOWAIT(FIFO_5_EMPTY);

                OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
                OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
                OUTPW(DEST_X, x);
                OUTPW(DEST_Y, pptDest->y + (iyBlits * 8));

                OUTPW(CMD, Cmd);
            }

            if (cxRight != 0)
            {
                x = pptDest->x + cxLeft + (ixBlits * COLOR_HORZ_EXPANSION_CACHE_CX);

                ASSERTS3(((x + (cxRight - 1)) < (INT) ppdev->cxScreen),
                          "S3.DLL!bColorHorzCacheToScreen - Blit Operation over right edge (6)\n");

                FIFOWAIT(FIFO_6_EMPTY);

                OUTPW(RECT_WIDTH, cxRight - 1);

                OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
                OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
                OUTPW(DEST_X, x);
                OUTPW(DEST_Y, pptDest->y + (iyBlits * 8));

                OUTPW(CMD, Cmd);
            }
        }
    }

    return(TRUE);
}

/*****************************************************************************
 * Color expand the monochrome brush in the Hoizontal Cache to the screen.
 ****************************************************************************/
BOOL bColorExpandHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    INT         zHorz,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest,
    WORD        s3ForeRop,
    WORD        s3BackRop)
{
    WORD    Cmd;
    INT     xLeft, cxLeft, xMiddle, cxMiddle, xRight, cxRight, i,
            iyBlits, iyLast,
            xOffset, yOffset;

    DISPDBG((3, "S3.DLL!bColorExpandHorzCacheToScreen - Entry\n"));

    // Take into account the pptBrushOrg.

    xOffset = pptDest->x - pptBrushOrg->x;
    yOffset = pptDest->y - pptBrushOrg->y;

    if (xOffset < 0)
        xOffset  = 8 - (-xOffset % 8);
    else
        xOffset %= 8;

    if (yOffset < 0)
        yOffset  = 8 - (-yOffset % 8);
    else
        yOffset %= 8;

    // Color Expand the monochrome horizontal cache to the screen.
    // This is done in a loop to handle the general case ROPs.

    xLeft  = pptDest->x;
    cxLeft = MONO_HORZ_EXPANSION_CACHE_CX - xOffset;

    if (psizDest->cx < cxLeft)
    {
        cxLeft  = psizDest->cx;
        xRight  = 0;
        cxRight = 0;
    }
    else
    {
        xRight  = pptDest->x + cxLeft;
        cxRight = psizDest->cx - cxLeft;
    }

    // This little hack is for 1280 mode.
    // Really the correct solution is to use the H/W support.

    if (cxRight > MONO_HORZ_EXPANSION_CACHE_CX)
    {
        xMiddle  = xRight;
        xRight  += MONO_HORZ_EXPANSION_CACHE_CX;
        cxMiddle = MONO_HORZ_EXPANSION_CACHE_CX;
        cxRight -= MONO_HORZ_EXPANSION_CACHE_CX;
    }
    else
    {
        xMiddle  = 0;
        cxMiddle = 0;
    }

    iyBlits = psizDest->cy / 8;
    iyLast  = psizDest->cy % 8;

    Cmd  = BITBLT         | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS| DRAWING_DIR_TBLRXM;

    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_RD_MASK(zHorz);

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | s3ForeRop);
    TEST_AND_SET_FRGD_COLOR(LOWORD(ps3Brush->ulForeColor));

    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | s3BackRop);
    SET_BKGD_COLOR(LOWORD(ps3Brush->ulBackColor));

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | 7));

    for (i = 0; i < iyBlits; i++)
    {
        ASSERTS3(((pptDest->x + (cxLeft - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorExpandHorzCacheToScreen - Blit Operation over right edge (1)\n");

        FIFOWAIT(FIFO_6_EMPTY);

        OUTPW(RECT_WIDTH, cxLeft - 1);

        OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X + xOffset);
        OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
        OUTPW(DEST_X, pptDest->x);
        OUTPW(DEST_Y, pptDest->y + (i * 8));

        OUTPW(CMD, Cmd);

        if (cxMiddle != 0)
        {
            ASSERTS3(((xMiddle + (cxMiddle - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bColorExpandHorzCacheToScreen - Blit Operation over right edge (2)\n");

            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(RECT_WIDTH, cxMiddle - 1);

            OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, xMiddle);
            OUTPW(DEST_Y, pptDest->y + (i * 8));

            OUTPW(CMD, Cmd);
        }

        // If the the cxDest is wider than the cxHorzCache then fill in the
        // right side of the fill area.

        if (cxRight != 0)
        {
            ASSERTS3(((xRight + (cxRight - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bColorExpandHorzCacheToScreen - Blit Operation over right edge (3)\n");

            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(RECT_WIDTH, cxRight - 1);

            OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, xRight);
            OUTPW(DEST_Y, pptDest->y + (i * 8));

            OUTPW(CMD, Cmd);
        }
    }

    if (iyLast != 0)
    {
        ASSERTS3(((pptDest->x + (cxLeft - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bColorExpandHorzCacheToScreen - Blit Operation over right edge (3)\n");

        FIFOWAIT(FIFO_7_EMPTY);

        OUTPW(RECT_WIDTH, cxLeft - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | iyLast - 1));

        OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X + xOffset);
        OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
        OUTPW(DEST_X, pptDest->x);
        OUTPW(DEST_Y, pptDest->y + (iyBlits * 8));

        OUTPW(CMD, Cmd);

        if (cxMiddle != 0)
        {
            ASSERTS3(((xMiddle + (cxMiddle - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bColorExpandHorzCacheToScreen - Blit Operation over right edge (4)\n");

            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(RECT_WIDTH, cxMiddle - 1);

            OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, xMiddle);
            OUTPW(DEST_Y, pptDest->y + (iyBlits * 8));

            OUTPW(CMD, Cmd);
        }

        // If the the cxDest is wider than the cxHorzCache then fill in the
        // right side of the fill area.

        if (cxRight != 0)
        {
            ASSERTS3(((xRight + (cxRight - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bColorExpandHorzCacheToScreen - Blit Operation over right edge (4)\n");

            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(RECT_WIDTH, cxRight - 1);

            OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, xRight);
            OUTPW(DEST_Y, pptDest->y + (iyBlits * 8));

            OUTPW(CMD, Cmd);
        }
    }

    // Reset the Read Mask

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);
    TEST_AND_SET_RD_MASK(0xff);

    return(TRUE);
}


/*****************************************************************************
 * Copy Optimized
 * Color brush in the Hoizontal Cache to the screen.
 ****************************************************************************/
BOOL bCpyColorHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest)
{
    WORD    Cmd;
    INT     cx, cxLeft, ixBlits, ixLast, i,
            cy, x, xOffset, yOffset;

    DISPDBG((3, "S3.DLL!bCpyColorHorzCacheToScreen - Entry\n"));

    // Take into account the pptBrushOrg.

    xOffset = pptDest->x - pptBrushOrg->x;
    yOffset = pptDest->y - pptBrushOrg->y;

    if (xOffset < 0)
        xOffset  = 8 - (-xOffset % 8);
    else
        xOffset %= 8;

    if (yOffset < 0)
        yOffset  = 8 - (-yOffset % 8);
    else
        yOffset %= 8;

    // First Color Expand the monochrome horizontal cache to the screen.

    cxLeft = COLOR_HORZ_EXPANSION_CACHE_CX - xOffset;
    if (psizDest->cx < cxLeft)
        cxLeft = psizDest->cx;

    cy = 8;
    if (psizDest->cy < cy)
        cy = psizDest->cy;

    Cmd  = BITBLT         | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS| DRAWING_DIR_TBLRXM;

    ASSERTS3(((pptDest->x + (cxLeft - 1)) < (INT) ppdev->cxScreen),
              "S3.DLL!bCpyColorHorzCacheToScreen - Blit Operation over right edge (1)\n");

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, cxLeft - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (cy - 1)));

    OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X + xOffset);
    OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
    OUTPW(DEST_X, pptDest->x);
    OUTPW(DEST_Y, pptDest->y);

    OUTPW(CMD, Cmd);

    // If the the cxDest is wider than the cxHorzCache then fill in the
    // right side of the fill area.

    if ((cx = psizDest->cx - cxLeft) > 0)
    {
        ixBlits = cx / COLOR_HORZ_EXPANSION_CACHE_CX;
        ixLast  = cx % COLOR_HORZ_EXPANSION_CACHE_CX;

        FIFOWAIT(FIFO_1_EMPTY);
        OUTPW(RECT_WIDTH, COLOR_HORZ_EXPANSION_CACHE_CX - 1);

        for (i = 0; i < ixBlits; i++)
        {
            x = pptDest->x + cxLeft + (i * COLOR_HORZ_EXPANSION_CACHE_CX);

            ASSERTS3(((x + (COLOR_HORZ_EXPANSION_CACHE_CX - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bCpyColorHorzCacheToScreen - Blit Operation over right edge (2)\n");

            FIFOWAIT(FIFO_5_EMPTY);

            OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, x);
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }

        if (ixLast != 0)
        {
            x = pptDest->x + cxLeft + (ixBlits * COLOR_HORZ_EXPANSION_CACHE_CX);

            ASSERTS3(((x + (ixLast - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bCpyColorHorzCacheToScreen - Blit Operation over right edge (3)\n");

            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(RECT_WIDTH, ixLast - 1);

            OUTPW(CUR_X,  COLOR_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  COLOR_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, x);
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }
    }

    // If the cyDest is taller than 8 pels do a rolling blit to fill in
    // the bottom of the fill area.

    if (psizDest->cy > 8)
    {
        ASSERTS3(((pptDest->x + (psizDest->cx - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bCpyColorHorzCacheToScreen - Blit Operation over right edge (4)\n");

        FIFOWAIT(FIFO_7_EMPTY);

        OUTPW(RECT_WIDTH, psizDest->cx - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | psizDest->cy - (cy + 1)));

        OUTPW(CUR_X,  pptDest->x);
        OUTPW(CUR_Y,  pptDest->y);
        OUTPW(DEST_X, pptDest->x);
        OUTPW(DEST_Y, pptDest->y + 8);

        OUTPW(CMD, Cmd);
    }

    return(TRUE);
}

/*****************************************************************************
 * Copy Optimized
 * Color expand the monochrome brush in the Hoizontal Cache to the screen.
 ****************************************************************************/
BOOL bCpyColorExpandHorzCacheToScreen(
    PPDEV       ppdev,
    PS3BRUSH    ps3Brush,
    INT         zHorz,
    PPOINT      pptBrushOrg,
    PPOINT      pptDest,
    PSIZE       psizDest)
{
    WORD    Cmd;
    INT     cx, cxLeft, ixBlits, ixLast, i,
            cy, x, xOffset, yOffset;

    DISPDBG((3, "S3.DLL!bCpyColorExpandHorzCacheToScreen - Entry\n"));

    // Take into account the pptBrushOrg.

    xOffset = pptDest->x - pptBrushOrg->x;
    yOffset = pptDest->y - pptBrushOrg->y;

    if (xOffset < 0)
        xOffset  = 8 - (-xOffset % 8);
    else
        xOffset %= 8;

    if (yOffset < 0)
        yOffset  = 8 - (-yOffset % 8);
    else
        yOffset %= 8;

    // First Color Expand the monochrome horizontal cache to the screen.

    cxLeft = MONO_HORZ_EXPANSION_CACHE_CX - xOffset;
    if (psizDest->cx < cxLeft)
        cxLeft = psizDest->cx;

    cy = 8;
    if (psizDest->cy < cy)
        cy = psizDest->cy;

    Cmd  = BITBLT         | DRAW | DIR_TYPE_XY | WRITE |
           MULTIPLE_PIXELS| DRAWING_DIR_TBLRXM;

    ASSERTS3(((pptDest->x + (cxLeft - 1)) < (INT) ppdev->cxScreen),
              "S3.DLL!bCpyColorExpandHorzCacheToScreen - Blit Operation over right edge (1)\n");

    FIFOWAIT(FIFO_6_EMPTY);

    TEST_AND_SET_RD_MASK(zHorz);

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | OVERPAINT);
    TEST_AND_SET_FRGD_COLOR(LOWORD(ps3Brush->ulForeColor));

    TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | OVERPAINT);
    SET_BKGD_COLOR(LOWORD(ps3Brush->ulBackColor));

    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | DISPLAY_MEMORY));

    FIFOWAIT(FIFO_7_EMPTY);

    OUTPW(RECT_WIDTH, cxLeft - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | (cy - 1)));

    OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X + xOffset);
    OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
    OUTPW(DEST_X, pptDest->x);
    OUTPW(DEST_Y, pptDest->y);

    OUTPW(CMD, Cmd);

    // If the the cxDest is wider than the cxHorzCache then fill in the
    // right side of the fill area.

    if ((cx = psizDest->cx - cxLeft) > 0)
    {
        ixBlits = cx / MONO_HORZ_EXPANSION_CACHE_CX;
        ixLast  = cx % MONO_HORZ_EXPANSION_CACHE_CX;

        FIFOWAIT(FIFO_1_EMPTY);
        OUTPW(RECT_WIDTH, MONO_HORZ_EXPANSION_CACHE_CX - 1);

        for (i = 0; i < ixBlits; i++)
        {
            x = pptDest->x + cxLeft + (i * MONO_HORZ_EXPANSION_CACHE_CX);

            ASSERTS3(((x + (COLOR_HORZ_EXPANSION_CACHE_CX - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bCpyColorExpandHorzCacheToScreen - Blit Operation over right edge (2)\n");

            FIFOWAIT(FIFO_5_EMPTY);

            OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, x);
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }

        if (ixLast != 0)
        {
            x = pptDest->x + cxLeft + (ixBlits * MONO_HORZ_EXPANSION_CACHE_CX);

            ASSERTS3(((x + (ixLast - 1)) < (INT) ppdev->cxScreen),
                      "S3.DLL!bCpyColorExpandHorzCacheToScreen - Blit Operation over right edge (3)\n");

            FIFOWAIT(FIFO_6_EMPTY);

            OUTPW(RECT_WIDTH, ixLast - 1);

            OUTPW(CUR_X,  MONO_HORZ_EXPANSION_CACHE_X);
            OUTPW(CUR_Y,  MONO_HORZ_EXPANSION_CACHE_Y + yOffset);
            OUTPW(DEST_X, x);
            OUTPW(DEST_Y, pptDest->y);

            OUTPW(CMD, Cmd);
        }
    }

    // If the cyDest is taller than 8 pels do a rolling blit to fill in
    // the bottom of the fill area.

    if (psizDest->cy > 8)
    {
        ASSERTS3(((pptDest->x + (psizDest->cx - 1)) < (INT) ppdev->cxScreen),
                  "S3.DLL!bCpyColorExpandHorzCacheToScreen - Blit Operation over right edge (4)\n");

        FIFOWAIT(FIFO_2_EMPTY);

        TEST_AND_SET_RD_MASK(0xff);
        TEST_AND_SET_FRGD_MIX(SRC_DISPLAY_MEMORY | OVERPAINT);

        FIFOWAIT(FIFO_8_EMPTY);

        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

        OUTPW(RECT_WIDTH, psizDest->cx - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | psizDest->cy - (cy + 1)));

        OUTPW(CUR_X,  pptDest->x);
        OUTPW(CUR_Y,  pptDest->y);
        OUTPW(DEST_X, pptDest->x);
        OUTPW(DEST_Y, pptDest->y + 8);

        OUTPW(CMD, Cmd);
    }

    // Reset the Read Mask

    FIFOWAIT(FIFO_2_EMPTY);

    TEST_AND_SET_WRT_MASK(0xff);
    TEST_AND_SET_RD_MASK(0xff);

    return(TRUE);
}


/*****************************************************************************
 * S3 Solid Pattern
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL bPatternSolid(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4     s3Rop4)
{
    INT     width,height;
    WORD    s3Mix;
    BOOL    bMore;
    UINT        i;
    ENUMRECTS8  EnumRects8;

    LONG    rleft;                          // render coordinates
    LONG    rtop;
    LONG    rright;
    LONG    rbottom;
    LONG    cleft;                          // clip coorindates
    LONG    ctop;
    LONG    cright;
    LONG    cbottom;
    LONG    left;
    LONG    top;
    RECTL   rclBounds;
    BOOL    bClipRequired, bDrawRequired;
    PPDEV   ppdev;

    DISPDBG((3,"S3.DLL!bPatternSolid - Entry\n"));

    // Get the pdev.

    ppdev = (PPDEV) psoTrg->dhpdev;

    // Just use the for ground mix.

    s3Mix = (WORD) (s3Rop4 & 0xFF);

    // Dereference the target rectangle.

    rleft   = prclTrg->left;
    rtop    = prclTrg->top;
    rright  = prclTrg->right;
    rbottom = prclTrg->bottom;

    // Use software clipping. It's faster than H/W clipping.

    if (pco->iDComplexity != DC_COMPLEX)
    {
        if (pco->iDComplexity == DC_TRIVIAL)
        {
            width  = (rright - rleft) - 1;
            height = (rbottom - rtop) - 1;

            bDrawRequired = TRUE;
        }
        else
        {
            bDrawRequired = FALSE;

            rclBounds = pco->rclBounds;

            // First handle the trivial rejection.

            bClipRequired = bIntersectTest(prclTrg, &rclBounds);

            if (bClipRequired)
            {
                rleft   = max (rleft, rclBounds.left);
                rtop    = max (rtop, rclBounds.top);
                width  = (min(rright, rclBounds.right) - rleft) - 1;
                height = (min(rbottom, rclBounds.bottom) - rtop) - 1;

                bDrawRequired = TRUE;
            }
        }

        if (bDrawRequired && width >= 0 && height >= 0)
        {
            vS3SolidPattern(ppdev,
                            rleft, rtop, width, height,
                            (pbo != NULL)? pbo->iSolidColor: 0, s3Mix);
        }
    }
    else
    {
        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do
        {
            bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8),
                                  (PULONG) &EnumRects8);
            for (i = 0; i < EnumRects8.c; i++)
            {
                // Do a trivial reject on Y.

                if (rtop > (cbottom = EnumRects8.arcl[i].bottom))
                    continue;
                if (rbottom < (ctop = EnumRects8.arcl[i].top))
                    continue;

                // Do a trivial reject on X.

                if (rleft > (cright = EnumRects8.arcl[i].right))
                    continue;
                if (rright < (cleft = EnumRects8.arcl[i].left))
                    continue;

                // It's not a trivial reject, so calculate the
                // minimal clip area.

                left   = max (rleft, cleft);
                top    = max (rtop, ctop);
                width  = (min(rright, cright) - left) - 1;
                height = (min(rbottom, cbottom) - top) - 1;

                if (width < 0 || height < 0)
                    continue;

                vS3SolidPattern(ppdev,
                                left, top,
                                width, height,
                                (pbo != NULL)? pbo->iSolidColor: 0, s3Mix);
            }
        } while (bMore);
    }

    return (TRUE);
}

/*****************************************************************************
 * vS3SolidPattern
 ****************************************************************************/
VOID vS3SolidPattern(
    PPDEV   ppdev,
    INT     left,
    INT     top,
    INT     width,
    INT     height,
    INT     color,
    WORD    s3Mix)
{
    FIFOWAIT(FIFO_8_EMPTY);

    ASSERTS3 (((left + width) < (INT) ppdev->cxScreen),
              "S3.DLL!vS3SolidPattern - (left + width) > (ppdev->cxScreen - 1)\n");

    TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | s3Mix);
    TEST_AND_SET_FRGD_COLOR(color);

    OUTPW (MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW (CUR_X, left);
    OUTPW (CUR_Y, top);
    OUTPW (RECT_WIDTH, width);
    OUTPW (MULTIFUNC_CNTL, (RECT_HEIGHT | height));

    OUTPW (CMD, RECTANGLE_FILL | DRAWING_DIR_TBLRXM |
                DRAW           | DIR_TYPE_XY        |
                LAST_PIXEL_ON  | MULTIPLE_PIXELS    |
                WRITE);
}

/*****************************************************************************
 * S3 8bpp Cached Managed Host to Screen Copy
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL b8BppHostToScrnCachedWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop)
{
    BOOL    bRet;
    HSURF   hsurf;
    PPDEV   ppdev;
    POINTL  ptlSrc;
    RECTL   rclTrg;
    SIZEL   sizlBitmap;

    PSAVEDSCRNBITSHDR   pssbhInPdev;
    PSAVEDSCRNBITS      pssbNewNode, pssbTemp;

    DISPDBG((3, "S3.DLL!b8BppHostToScrnCachedWithRop - entry\n"));

    n8BppBitmaps++;

    ppdev = (PPDEV) psoTrg->dhpdev;
    hsurf = psoSrc->hsurf;

#if 0
    DISPDBG((2, "\thsurf                : %x\n", hsurf));
    DISPDBG((2, "\tpsoSrc->iUniq        : %x\n", psoSrc->iUniq));
    DISPDBG((2, "\tpsoSrc->sizlBitmap.cx: %d\n", psoSrc->sizlBitmap.cx));
    DISPDBG((2, "\tpsoSrc->sizlBitmap.cy: %d\n", psoSrc->sizlBitmap.cy));
#endif

#if SRCBM_CACHE

    // Is this bitmap in the cache?

    if ((hsurf == ppdev->hsurfCachedBitmap) &&
        (psoSrc->iUniq == ppdev->iUniqCachedBitmap))
    {
        if ((((pxlo == NULL) ||
              (pxlo->flXlate & XO_TRIVIAL)) && (ppdev->iUniqXlate == 1)) ||
            (pxlo->iUniq == ppdev->iUniqXlate))
        {
            // The bitmap is in the cache
            // Keep a cache hit count.

            n8BppBmCacheHits++;

            // Blt from the cache.

            ptlSrc.x = pptlSrc->x + OFF_SCREEN_BITMAP_X;
            ptlSrc.y = pptlSrc->y + OFF_SCREEN_BITMAP_Y;

            return(bScrnToScrnWithRop(psoTrg, psoTrg, pco, prclTrg,
                                      &ptlSrc, s3Rop));
        }
    }

    // The bitmap is not in the cache.
    // Is it small enough to fit into the cache?

    sizlBitmap = psoSrc->sizlBitmap;

    if ((sizlBitmap.cx <= OFF_SCREEN_BITMAP_CX) &&
        (sizlBitmap.cy <= OFF_SCREEN_BITMAP_CY))
    {
        // It will fit in the cache.
        // If the cache is being used for some saved screen bits
        // move them to host memory.

        if (ppdev->SavedScreenBitsHeader.iUniq != -1)
        {
            nSsbMovedToHostFromSrcBmCache++;

            DISPDBG((1, "S3.DLL - Saved Screen Bits Moved to Host Memory from Source Bitmap Cache Manager \n"));

            // Move the actual bits to host memory.

            bRet = bMoveSaveScreenBitsToHost(ppdev, &pssbNewNode);

            if (bRet == FALSE)
                return(FALSE);

            // Connect this newNode to the beginning of the list of
            // save screen bits nodes.

            pssbhInPdev = &(ppdev->SavedScreenBitsHeader);

            pssbTemp                   = pssbhInPdev->pssbLink;
            pssbhInPdev->pssbLink      = pssbNewNode;
            pssbNewNode->ssbh.pssbLink = pssbTemp;

            // Invalidate the Save Screen bits in off screen memory

            ppdev->SavedScreenBitsHeader.iUniq = (ULONG) -1;
        }


        // Set the cache tags.

        ppdev->hsurfCachedBitmap = hsurf;
        ppdev->iUniqCachedBitmap = psoSrc->iUniq;

        if ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL))
        {
            ppdev->iUniqXlate = 1;
        }
        else
        {
            ppdev->iUniqXlate = pxlo->iUniq;
        }

        // Put the entire bitmap into the cache.

        rclTrg.left   = OFF_SCREEN_BITMAP_X;
        rclTrg.top    = OFF_SCREEN_BITMAP_Y;
        rclTrg.right  = OFF_SCREEN_BITMAP_X + sizlBitmap.cx;
        rclTrg.bottom = OFF_SCREEN_BITMAP_Y + sizlBitmap.cy;

        ptlSrc.x = 0;
        ptlSrc.y = 0;

        bRet = b8BppHostToScrnWithRop(psoTrg, psoSrc, ppdev->pcoFullRam,
                                      pxlo, &rclTrg, &ptlSrc, OVERPAINT);

        if (bRet == TRUE)
        {
            // Blt from the cache.

            ptlSrc.x = pptlSrc->x + OFF_SCREEN_BITMAP_X;
            ptlSrc.y = pptlSrc->y + OFF_SCREEN_BITMAP_Y;

            bRet = bScrnToScrnWithRop(psoTrg, psoTrg, pco, prclTrg,
                                      &ptlSrc, s3Rop);

        }
    }
    else
    {
        // The bitmap was too large to cache.
        // So, just blt it directly to the screen.

        bRet = b8BppHostToScrnWithRop(psoTrg, psoSrc, pco, pxlo,
                                      prclTrg, pptlSrc, s3Rop);
    }
#else

    bRet = b8BppHostToScrnWithRop(psoTrg, psoSrc, pco, pxlo,
                                  prclTrg, pptlSrc, s3Rop);

#endif

    return (bRet);
}

/*****************************************************************************
 * S3 8bpp Host to Screen Copy
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL b8BppHostToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop)
{
    LONG    lSrcDelta;
    PWORD   pwFirstWord;
    BOOL    bMore;
    PPDEV   ppdev;
    BYTE    iDComplexity;
    RECTL   rclTrg, rclBounds;
    POINT   ptSrc;
    INT     i;
    SIZE    sizBlt;

    ENUMRECTS8  EnumRects8;

    DISPDBG((3, "S3.DLL!b8BppHostToScrnWithRop - entry\n"));

    ppdev = (PPDEV) psoTrg->dhpdev;

    // Pickup some convienent locals.

    lSrcDelta = psoSrc->lDelta;

    if ((iDComplexity = pco->iDComplexity) != DC_COMPLEX)
    {
        // Make a copy of the target, since we will have to change
        // it for clipping.

        rclTrg = *prclTrg;

        if (iDComplexity == DC_RECT)
        {
            rclBounds = pco->rclBounds;

            // Handle the trivial rejection and
            // define the clipped target rectangle.

            if (bIntersectTest(&rclTrg, &rclBounds))
            {
                rclTrg.left   = max (rclTrg.left, rclBounds.left);
                rclTrg.top    = max (rclTrg.top, rclBounds.top);
                rclTrg.right  = min (rclTrg.right, rclBounds.right);
                rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);
            }
            else
            {
                // The destination rectangle is completely clipped out,
                // so just return.

                return (TRUE);
            }
        }

        // define the cx & cy for the blit.

        sizBlt.cx = rclTrg.right  - rclTrg.left;
        sizBlt.cy = rclTrg.bottom - rclTrg.top;

        // calculate the cx & cy shift (due to clipping) for the source.
        // and define the upper left corner of the source.

        ptSrc.x = pptlSrc->x + (rclTrg.left - prclTrg->left);
        ptSrc.y = pptlSrc->y + (rclTrg.top  - prclTrg->top);

        // Calculate the first word to blit

        pwFirstWord = (PWORD) (((PBYTE) psoSrc->pvScan0)
                     + (ptSrc.y * lSrcDelta) + ptSrc.x);

        vLowLevel8BppHostToScrnWithRop(ppdev, (PPOINT) &rclTrg, &sizBlt,
                                       pwFirstWord, lSrcDelta, pxlo, s3Rop);

    }
    else
    {
        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do
        {
            bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8), (PULONG) &EnumRects8);

            for (i = 0; i < (INT) EnumRects8.c; i++)
            {
                rclTrg = *prclTrg;
                rclBounds = EnumRects8.arcl[i];

                // Handle the trivial rejection and
                // define the clipped target rectangle.

                if (bIntersectTest(&rclTrg, &rclBounds))
                {
                    rclTrg.left   = max (rclTrg.left, rclBounds.left);
                    rclTrg.top    = max (rclTrg.top, rclBounds.top);
                    rclTrg.right  = min (rclTrg.right, rclBounds.right);
                    rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);

                    // define the cx & cy for the blit.

                    sizBlt.cx = rclTrg.right  - rclTrg.left;
                    sizBlt.cy = rclTrg.bottom - rclTrg.top;

                    // calculate the cx & cy shift (due to clipping) for the source.
                    // and define the upper left corner of the source.

                    ptSrc.x = pptlSrc->x + (rclTrg.left - prclTrg->left);
                    ptSrc.y = pptlSrc->y + (rclTrg.top  - prclTrg->top);

                    // Calculate the first word to blit

                    pwFirstWord = (PWORD) (((PBYTE) psoSrc->pvScan0)
                                 + (ptSrc.y * lSrcDelta) + ptSrc.x);

                    vLowLevel8BppHostToScrnWithRop(ppdev,
                                                   (PPOINT) &rclTrg,
                                                   &sizBlt,
                                                   pwFirstWord,
                                                   lSrcDelta,
                                                   pxlo,
                                                   s3Rop);
                }
            }
        } while (bMore);
    }

    return (TRUE);
}



/*****************************************************************************
 * Low Level 8bpp host to screen copy.
 ****************************************************************************/
VOID vLowLevel8BppHostToScrnWithRop(
    PPDEV   ppdev,
    PPOINT  pptTrg,
    PSIZE   psizBlt,
    PWORD   pwFirstWord,
    INT     lSrcDelta,
    XLATEOBJ *pxlo,
    WORD    s3Rop)
{
    PWORD   pw;
    INT     i, j;
    WORD    Cmd;
    PBYTE   pbSrc;
    INT     nSrc;
    PULONG  pulXlate;
    INT     cy, wpl;
    BYTE    LineBuff[DRIVERS_MAX_CX];

    // Setup the S3 chip.

    cy = psizBlt->cy;

    Cmd = RECTANGLE_FILL | BYTE_SWAP          | BUS_SIZE_16 | WAIT |
          DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
          LAST_PIXEL_ON  | SINGLE_PIXEL       | WRITE;

    FIFOWAIT(FIFO_7_EMPTY);

    TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | s3Rop);
    OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

    OUTPW(CUR_X, pptTrg->x);
    OUTPW(CUR_Y, pptTrg->y);
    OUTPW(RECT_WIDTH, psizBlt->cx - 1);
    OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cy - 1));

    GPWAIT();

    OUTPW(CMD, Cmd);

    CHECK_DATA_READY;

    wpl = (psizBlt->cx + 1) >> 1;

    // We may have to do some color translation.

    if ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL))
    {
        // Now transfer the data.

        // Note: It would nice to do the entire bitmap in one
        //       fell swoop, but there is no gaurantee source will
        //       wrap on a bitmap boundary.

        pw = pwFirstWord;
        for (i = 0; i < cy; i++)
        {
            vDataPortOut(ppdev, (PWORD) pw, wpl);
            ((PBYTE) pw) += lSrcDelta;
        }
    }
    else
    {
        if (pxlo->flXlate & XO_TABLE)
        {
            pulXlate = pxlo->pulXlate;
        }
        else
        {
            pulXlate = XLATEOBJ_piVector(pxlo);
        }

        pbSrc = (PBYTE) pwFirstWord;
        nSrc  = wpl * 2;

        for (i = 0; i < cy; i++)
        {
            for (j = 0; j < nSrc; j++)
            {
                LineBuff[j] = LOBYTE(pulXlate[pbSrc[j]]);
            }

            vDataPortOut(ppdev, (PWORD) LineBuff, wpl);
            pbSrc += lSrcDelta;
        }
    }

    CHECK_DATA_COMPLETE;

    return;

}


/*****************************************************************************
 * S3 1bpp Cached Managed Host to Screen With Rop
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL b1BppHostToScrnCachedWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop)
{
    BOOL    bRet;
    HSURF   hsurf;
    PPDEV   ppdev;
    POINTL  ptlSrc;
    RECTL   rclTrg;
    SIZEL   sizlBitmap;

    PSAVEDSCRNBITSHDR   pssbhInPdev;
    PSAVEDSCRNBITS      pssbNewNode, pssbTemp;

    DISPDBG((3, "S3.DLL!b1BppHostToScrnCachedWithRop - entry\n"));

    n1BppBitmaps++;

    ppdev = (PPDEV) psoTrg->dhpdev;
    hsurf = psoSrc->hsurf;

#if 1
    DISPDBG((2, "\thsurf                : %x\n", hsurf));
    DISPDBG((2, "\tpsoSrc->iUniq        : %x\n", psoSrc->iUniq));
    DISPDBG((2, "\tpsoSrc->sizlBitmap.cx: %d\n", psoSrc->sizlBitmap.cx));
    DISPDBG((2, "\tpsoSrc->sizlBitmap.cy: %d\n", psoSrc->sizlBitmap.cy));
#endif

#if SRCBM_CACHE

    // Is this bitmap in the cache?

    if ((hsurf == ppdev->hsurfCachedBitmap) &&
        (psoSrc->iUniq == ppdev->iUniqCachedBitmap))
    {
        if ((((pxlo == NULL) ||
              (pxlo->flXlate & XO_TRIVIAL)) && (ppdev->iUniqXlate == 1)) ||
            (pxlo->iUniq == ppdev->iUniqXlate))
        {
            // The bitmap is in the cache
            // Keep a cache hit count.

            n1BppBmCacheHits++;

            // Blt from the cache.

            ptlSrc.x = pptlSrc->x + OFF_SCREEN_BITMAP_X;
            ptlSrc.y = pptlSrc->y + OFF_SCREEN_BITMAP_Y;

            return(bScrnToScrnWithRop(psoTrg, psoTrg, pco, prclTrg,
                                      &ptlSrc, s3Rop));
        }
    }

    // The bitmap is not in the cache.
    // Is it small enough to fit into the cache?

    sizlBitmap = psoSrc->sizlBitmap;

    if ((sizlBitmap.cx <= OFF_SCREEN_BITMAP_CX) &&
        (sizlBitmap.cy <= OFF_SCREEN_BITMAP_CY))
    {
        // It will fit in the cache.
        // If the cache is being used for some saved screen bits
        // move them to host memory.

        if (ppdev->SavedScreenBitsHeader.iUniq != -1)
        {
            nSsbMovedToHostFromSrcBmCache++;

            DISPDBG((1, "S3.DLL - Saved Screen Bits Moved to Host Memory from Source Bitmap Cache Manager \n"));

            // Move the actual bits to host memory.

            bRet = bMoveSaveScreenBitsToHost(ppdev, &pssbNewNode);

            if (bRet == FALSE)
                return(FALSE);

            // Connect this newNode to the beginning of the list of
            // save screen bits nodes.

            pssbhInPdev = &(ppdev->SavedScreenBitsHeader);

            pssbTemp                   = pssbhInPdev->pssbLink;
            pssbhInPdev->pssbLink      = pssbNewNode;
            pssbNewNode->ssbh.pssbLink = pssbTemp;

            // Invalidate the Save Screen bits in off screen memory

            ppdev->SavedScreenBitsHeader.iUniq = (ULONG) -1;
        }


        // Set the cache tags.

        ppdev->hsurfCachedBitmap = hsurf;
        ppdev->iUniqCachedBitmap = psoSrc->iUniq;

        if ((pxlo == NULL) || (pxlo->flXlate & XO_TRIVIAL))
        {
            ppdev->iUniqXlate = 1;
        }
        else
        {
            ppdev->iUniqXlate = pxlo->iUniq;
        }

        // Put the entire bitmap into the cache.

        rclTrg.left   = OFF_SCREEN_BITMAP_X;
        rclTrg.top    = OFF_SCREEN_BITMAP_Y;
        rclTrg.right  = OFF_SCREEN_BITMAP_X + sizlBitmap.cx;
        rclTrg.bottom = OFF_SCREEN_BITMAP_Y + sizlBitmap.cy;

        ptlSrc.x = 0;
        ptlSrc.y = 0;

        bRet = b1BppHostToScrnWithRop(psoTrg, psoSrc, ppdev->pcoFullRam,
                                      pxlo, &rclTrg, &ptlSrc, OVERPAINT);

        if (bRet == TRUE)
        {
            // Blt from the cache.

            ptlSrc.x = pptlSrc->x + OFF_SCREEN_BITMAP_X;
            ptlSrc.y = pptlSrc->y + OFF_SCREEN_BITMAP_Y;

            bRet = bScrnToScrnWithRop(psoTrg, psoTrg, pco, prclTrg,
                                      &ptlSrc, s3Rop);

        }
    }
    else
    {
        // The bitmap was too large to cache.
        // So, just blt it directly to the screen.

        bRet = b1BppHostToScrnWithRop(psoTrg, psoSrc, pco, pxlo,
                                      prclTrg, pptlSrc, s3Rop);
    }
#else

    bRet = b1BppHostToScrnWithRop(psoTrg, psoSrc, pco, pxlo,
                                  prclTrg, pptlSrc, s3Rop);

#endif

    return (bRet);
}





/*****************************************************************************
 * S3 1bpp Host to Screen With ROP
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL b1BppHostToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop)
{
    LONG    lSrcDelta;
    PWORD   pwFirstWord;
    BOOL    bMore;
    PPDEV   ppdev;
    BYTE    iDComplexity;
    RECTL   rclTrg, rclBounds;
    POINT   ptSrc;
    INT     i;
    SIZE    sizBlt;

    ENUMRECTS8  EnumRects8;

    DISPDBG((3, "S3.DLL!b1BppHostToScrnWithRop - entry\n"));

    ppdev = (PPDEV) psoTrg->dhpdev;

    // Pickup some convienent locals.

    lSrcDelta = psoSrc->lDelta;

    if ((iDComplexity = pco->iDComplexity) != DC_COMPLEX)
    {
        // Make a copy of the target, since we will have to change
        // it for clipping.

        rclTrg = *prclTrg;

        if (iDComplexity == DC_RECT)
        {
            rclBounds = pco->rclBounds;

            // Handle the trivial rejection and
            // define the clipped target rectangle.

            if (bIntersectTest(&rclTrg, &rclBounds))
            {
                rclTrg.left   = max (rclTrg.left, rclBounds.left);
                rclTrg.top    = max (rclTrg.top, rclBounds.top);
                rclTrg.right  = min (rclTrg.right, rclBounds.right);
                rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);
            }
            else
            {
                // The destination rectangle is completely clipped out,
                // so just return.

                return (TRUE);
            }
        }

        // define the cx & cy for the blit.

        sizBlt.cx = rclTrg.right  - rclTrg.left;
        sizBlt.cy = rclTrg.bottom - rclTrg.top;

        // calculate the cx & cy shift (due to clipping) for the source.
        // and define the upper left corner of the source.

        ptSrc.x = pptlSrc->x + (rclTrg.left - prclTrg->left);
        ptSrc.y = pptlSrc->y + (rclTrg.top  - prclTrg->top);

        // Calculate the first word to blit

        pwFirstWord = (PWORD) (((PBYTE) psoSrc->pvScan0)
                     + (ptSrc.y * lSrcDelta) + (ptSrc.x / 8));

        vSetS3ClipRect(ppdev, &rclTrg);
        vLowLevel1BppHostToScrnWithRop(ppdev,
                                       (PPOINT) &rclTrg,
                                       &sizBlt,
                                       ptSrc.x,
                                       pwFirstWord,
                                       lSrcDelta,
                                       pxlo,
                                       s3Rop);

    }
    else
    {
        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do
        {
            bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8), (PULONG) &EnumRects8);

            for (i = 0; i < (INT) EnumRects8.c; i++)
            {
                rclTrg = *prclTrg;
                rclBounds = EnumRects8.arcl[i];

                // Handle the trivial rejection and
                // define the clipped target rectangle.

                if (bIntersectTest(&rclTrg, &rclBounds))
                {
                    rclTrg.left   = max (rclTrg.left, rclBounds.left);
                    rclTrg.top    = max (rclTrg.top, rclBounds.top);
                    rclTrg.right  = min (rclTrg.right, rclBounds.right);
                    rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);

                    // define the cx & cy for the blit.

                    sizBlt.cx = rclTrg.right  - rclTrg.left;
                    sizBlt.cy = rclTrg.bottom - rclTrg.top;

                    // calculate the cx & cy shift (due to clipping) for the source.
                    // and define the upper left corner of the source.

                    ptSrc.x = pptlSrc->x + (rclTrg.left - prclTrg->left);
                    ptSrc.y = pptlSrc->y + (rclTrg.top  - prclTrg->top);

                    // Calculate the first word to blit

                    pwFirstWord = (PWORD) (((PBYTE) psoSrc->pvScan0)
                                 + (ptSrc.y * lSrcDelta) + (ptSrc.x / 8));

                    vSetS3ClipRect(ppdev, &rclTrg);
                    vLowLevel1BppHostToScrnWithRop(ppdev,
                                                   (PPOINT) &rclTrg,
                                                   &sizBlt,
                                                   ptSrc.x,
                                                   pwFirstWord,
                                                   lSrcDelta,
                                                   pxlo,
                                                   s3Rop);
                }
            }
        } while (bMore);
    }

    vResetS3Clipping(ppdev);

    return (TRUE);
}


/*****************************************************************************
 * Low Level 1bpp host to screen copy.
 *
 * Note: This routine should be modified to handle separate foreground
 *       and background mix modes.
 ****************************************************************************/
VOID vLowLevel1BppHostToScrnWithRop(
    PPDEV   ppdev,
    PPOINT  pptTrg,
    PSIZE   psizBlt,
    INT     xSrc,
    PWORD   pwFirstWord,
    INT     lSrcDelta,
    XLATEOBJ *pxlo,
    WORD    s3Rop)
{
PBYTE   pb1bpp;
INT     i, x, cx, cy, nSrcBytes, xClip;
WORD    Cmd;
PULONG  pulXlate;

        if (pxlo->flXlate & XO_TABLE)
        {
            pulXlate = pxlo->pulXlate;
        }
        else
        {
            pulXlate = XLATEOBJ_piVector(pxlo);
        }


        x = pptTrg->x;
        cx = psizBlt->cx;

        // Take care of the non-byte aligned source case.

        if (xSrc & 0x7)
        {
            xClip = x;
            x  -= (xSrc & 0x7);
            cx += (xSrc & 0x7);

            FIFOWAIT(FIFO_1_EMPTY);
          outpw (MULTIFUNC_CNTL, (CLIP_LEFT   | xClip));
        }

        Cmd = RECTANGLE_FILL | BUS_SIZE_8         | WAIT |
              DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
              LAST_PIXEL_ON  | MULTIPLE_PIXELS    | WRITE;

        // Setup the S3 chip.

        cy = psizBlt->cy;

        FIFOWAIT(FIFO_4_EMPTY);

        TEST_AND_SET_FRGD_MIX(FOREGROUND_COLOR | s3Rop);
        TEST_AND_SET_FRGD_COLOR(LOWORD(pulXlate[1]));
        TEST_AND_SET_BKGD_MIX(BACKGROUND_COLOR | s3Rop);
        SET_BKGD_COLOR(LOWORD(pulXlate[0]));

        FIFOWAIT(FIFO_5_EMPTY);

        OUTPW(MULTIFUNC_CNTL, (DATA_EXTENSION | CPU_DATA));

        OUTPW(CUR_X, x);
        OUTPW(CUR_Y, pptTrg->y);
        OUTPW(RECT_WIDTH, cx - 1);
        OUTPW(MULTIFUNC_CNTL, (RECT_HEIGHT | cy - 1));

        GPWAIT();

        OUTPW(CMD, Cmd);

        CHECK_DATA_READY;

        pb1bpp = (PBYTE) pwFirstWord;
        nSrcBytes = (cx + 7) / 8;

        for (i = 0; i < cy; i++)
        {
            vDataPortOutB(ppdev, pb1bpp, nSrcBytes);
            pb1bpp += lSrcDelta;
        }

        CHECK_DATA_COMPLETE;

        return;

}


/*****************************************************************************
 * S3 4bpp Host to Screen Copy
 *
 *  Returns TRUE if the blit was handled.
 ****************************************************************************/
BOOL b4BppHostToScrnWithRop(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    WORD     s3Rop)
{
    LONG    lSrcDelta;
    PWORD   pwFirstWord;
    BOOL    bMore;
    PPDEV   ppdev;
    BYTE    iDComplexity;
    RECTL   rclTrg, rclBounds;
    POINT   ptSrc;
    INT     i;
    SIZE    sizBlt;

    ENUMRECTS8  EnumRects8;

    DISPDBG((2, "S3.DLL!b4BppHostToScrnWithRop - entry\n"));

    ppdev = (PPDEV) psoTrg->dhpdev;

    // Pickup some convienent locals.

    lSrcDelta = psoSrc->lDelta;

    if ((iDComplexity = pco->iDComplexity) != DC_COMPLEX)
    {
        // Make a copy of the target, since we will have to change
        // it for clipping.

        rclTrg = *prclTrg;

        if (iDComplexity == DC_RECT)
        {
            rclBounds = pco->rclBounds;

            // Handle the trivial rejection and
            // define the clipped target rectangle.

            if (bIntersectTest(&rclTrg, &rclBounds))
            {
                rclTrg.left   = max (rclTrg.left, rclBounds.left);
                rclTrg.top    = max (rclTrg.top, rclBounds.top);
                rclTrg.right  = min (rclTrg.right, rclBounds.right);
                rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);
            }
            else
            {
                // The destination rectangle is completely clipped out,
                // so just return.

                return (TRUE);
            }
        }

        // define the cx & cy for the blit.

        sizBlt.cx = rclTrg.right  - rclTrg.left;
        sizBlt.cy = rclTrg.bottom - rclTrg.top;

        // calculate the cx & cy shift (due to clipping) for the source.
        // and define the upper left corner of the source.

        ptSrc.x = pptlSrc->x + (rclTrg.left - prclTrg->left);
        ptSrc.y = pptlSrc->y + (rclTrg.top  - prclTrg->top);

        // Calculate the first word to blit

        pwFirstWord = (PWORD) (((PBYTE) psoSrc->pvScan0)
                     + (ptSrc.y * lSrcDelta) + (ptSrc.x / 2));

        vSetS3ClipRect(ppdev, &rclTrg);
        vLowLevel4BppHostToScrnWithRop(ppdev, (PPOINT) &rclTrg, &sizBlt,
                                   ptSrc.x,
                                   pwFirstWord, lSrcDelta, pxlo, s3Rop);

    }
    else
    {
        CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);

        do
        {
            bMore = CLIPOBJ_bEnum(pco, sizeof (ENUMRECTS8), (PULONG) &EnumRects8);

            for (i = 0; i < (INT) EnumRects8.c; i++)
            {
                rclTrg = *prclTrg;
                rclBounds = EnumRects8.arcl[i];

                // Handle the trivial rejection and
                // define the clipped target rectangle.

                if (bIntersectTest(&rclTrg, &rclBounds))
                {
                    rclTrg.left   = max (rclTrg.left, rclBounds.left);
                    rclTrg.top    = max (rclTrg.top, rclBounds.top);
                    rclTrg.right  = min (rclTrg.right, rclBounds.right);
                    rclTrg.bottom = min (rclTrg.bottom, rclBounds.bottom);

                    // define the cx & cy for the blit.

                    sizBlt.cx = rclTrg.right  - rclTrg.left;
                    sizBlt.cy = rclTrg.bottom - rclTrg.top;

                    // calculate the cx & cy shift (due to clipping) for the source.
                    // and define the upper left corner of the source.

                    ptSrc.x = pptlSrc->x + (rclTrg.left - prclTrg->left);
                    ptSrc.y = pptlSrc->y + (rclTrg.top  - prclTrg->top);

                    // Calculate the first word to blit

                    pwFirstWord = (PWORD) (((PBYTE) psoSrc->pvScan0)
                                 + (ptSrc.y * lSrcDelta) + (ptSrc.x / 2));

                    vSetS3ClipRect(ppdev, &rclTrg);
                    vLowLevel4BppHostToScrnWithRop(ppdev, (PPOINT) &rclTrg, &sizBlt,
                                               ptSrc.x,
                                               pwFirstWord, lSrcDelta, pxlo, s3Rop);
                }
            }
        } while (bMore);
    }

    vResetS3Clipping(ppdev);

    return (TRUE);

}




/*****************************************************************************
 * Low Level 4bpp host to screen copy.
 ****************************************************************************/
VOID vLowLevel4BppHostToScrnWithRop(
    PPDEV   ppdev,
    PPOINT  pptTrg,
    PSIZE   psizBlt,
    INT     xSrc,
    PWORD   pwFirstWord,
    INT     lSrcDelta,
    XLATEOBJ *pxlo,
    WORD    s3Rop)
{
PBYTE   pb4Bpp;
INT     i, j, k, x, cx, cy, nSrcBytes, xClip;
WORD    Cmd;
PULONG  pulXlate;

BYTE    LineBuff8Bpp[DRIVERS_MAX_CX];

        if (pxlo->flXlate & XO_TABLE)
        {
            pulXlate = pxlo->pulXlate;
        }
        else
        {
            pulXlate = XLATEOBJ_piVector(pxlo);
        }

        pb4Bpp = (PBYTE) pwFirstWord;

        x = pptTrg->x;
        cx = psizBlt->cx;
        cy = psizBlt->cy;

        // Take care of the non-byte aligned source case.

        if (xSrc & 0x1)
        {
            xClip = x;
            x--;
            cx++;

            FIFOWAIT(FIFO_1_EMPTY);
            outpw (MULTIFUNC_CNTL, (CLIP_LEFT   | xClip));
        }

        nSrcBytes = (cx + 1) / 2;

        // Setup the S3 chip.

        Cmd = RECTANGLE_FILL | BYTE_SWAP          | BUS_SIZE_16 | WAIT |
              DRAW           | DRAWING_DIR_TBLRXM | DIR_TYPE_XY |
              LAST_PIXEL_ON  | SINGLE_PIXEL       | WRITE;

        FIFOWAIT(FIFO_7_EMPTY);

        TEST_AND_SET_FRGD_MIX(SRC_CPU_DATA | s3Rop);
        outpw(MULTIFUNC_CNTL, (DATA_EXTENSION | ALL_ONES));

        outpw(CUR_X, x);
        outpw(CUR_Y, pptTrg->y);
        outpw(RECT_WIDTH, cx - 1);
        outpw(MULTIFUNC_CNTL, (RECT_HEIGHT | cy - 1));

        GPWAIT();

        outpw(CMD, Cmd);

        CHECK_DATA_READY;

        // Now transfer the data.

        for (i = 0; i < cy; i++)
        {
            for (k = 0, j = 0; j < nSrcBytes; j++)
            {
                LineBuff8Bpp[k++] = (BYTE) pulXlate[(pb4Bpp[j] & 0xF0) >> 4];
                LineBuff8Bpp[k++] = (BYTE) pulXlate[pb4Bpp[j] & 0x0F];
            }

            vDataPortOut(ppdev, (PWORD) LineBuff8Bpp, nSrcBytes);
            pb4Bpp += lSrcDelta;
        }

        CHECK_DATA_COMPLETE;

        return;

}
