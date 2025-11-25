//--------------------------------------------------------------------------
//
// Module Name:  FONTDATA.C
//
// Brief Description:  Device font data querying routines
//
// Author:  Kent Settle (kentse)
// Created: 25-Apr-1991
//
// Copyright (C) 1991 - 1992 Microsoft Corporation.
//
// This module contains DrvQueryFontData and related routines.
//
// History:
//   25-Apr-1991    -by-    Kent Settle       (kentse)
// Created.
//--------------------------------------------------------------------------

#include "pscript.h"
#include "enable.h"
#include "resource.h"
#include <memory.h>
#include "winbase.h"

#define WIN31_INTEGER_METRICS

extern HMODULE     ghmodDrv;    // GLOBAL MODULE HANDLE.
extern LONG iHipot(LONG, LONG);

//--------------------------------------------------------------------------
// LONG DrvQueryFontData (dhpdev,pfo,iMode,cData,pvIn,pvOut)
// DHPDEV     dhpdev;
// PFONTOBJ   pfo;
// ULONG      iMode;
// ULONG      cData;
// PVOID      pvIn;
// PVOID      pvOut;
//
// This function is used to return information about a realized font.
// GDI provides a pointer to an array of glyph or kerning handles and
// the driver returns information about the glyphs or kerning pairs.
// The driver may assume that all handles in the array are valid.
//
// Parameters:
//   dhpdev
//     This is a PDEV handle returned from a call to DrvEnablePDEV.
//
//   pfo
//     This is a pointer to a FONTOBJ.  Details of the font realization
//     can be queried from this object.
//
//   iMode
//     This defines the information requested.  This may take one of the
//     following values.
//
//       QFD_GLYPH    GDI has placed an array of cData HGLYPHs at pvIn.
//                    If pvOut is not NULL the driver must return an array
//                    of corresponding GLYPHDATA structures to the buffer
//                    at pvOut.  If pvOut is NULL then the driver should return
//                    only the size of the buffer needed for output.
//
//       QFD_KERNPAIR GDI has placed an array of cData HKERNs at pvIn.
//                    If pvOut is not NULL the driver must return an array of
//                    corresponding POINTLs to the buffer at pvOUt, each of
//                    which describes a kerning adjustment.  If pvOut if NULL
//                    the driver should return only the size of the buffer
//                    needed for output.
//
//
//   cData
//     A count of input items at pvIn.  The interpretation of this count
//     depends on iMode.
//
//   pvIn
//     This is a pointer to an array of cData handles.If iMode is equal to
//     DRV_FONTDATA_GLYPH then these are glyph handles (HGLYPHs).  If iMode
//     is DRV_FONTDATA_KERNPAIR then the handles identify kerning pairs
//     (HKERNS).  The driver may assume that all handles are valid.
//
// Returns:
//   The return value is either the number of BYTEs needed or the number of
//   BYTEs written, depending on iMode.  In all cases a return value of
//   -1 indicates an error, and an error code is logged.
//
//
// History:
//   25-Apr-1991    -by-    Kent Settle       (kentse)
// Wrote it.
//--------------------------------------------------------------------------

LONG DrvQueryFontData (dhpdev,pfo,iMode,hg,pgd,pv,cjSize)
DHPDEV     dhpdev;
FONTOBJ   *pfo;
ULONG      iMode;
HGLYPH     hg;
GLYPHDATA  *pgd;
PVOID      pv;
ULONG	   cjSize;
{
    PNTFM               pntfm = NULL;
    LONG                lWidth;
    PBYTE               pCharCode;
    PBYTE               pCode;
    PUSHORT             pCharWidth;
    int                 i;
    PDEVDATA            pdev;
    XFORMOBJ           *pxo;
    ULONG               ulComplex;
    PIFIMETRICS         pifi;
    POINTL              ptl1, ptl2;
    POINTFIX            ptfx1;
    FD_DEVICEMETRICS   *pdm = (FD_DEVICEMETRICS *)pv;
    FIX                 fxLength, fxExtLeading;
    LONG                lfHeight, InternalLeading;
    FD_REALIZEEXTRA    *pre = NULL;
    DWORD               dwPointSize, dwLeadSuggest;

#ifdef TESTING
    DbgPrint("Entering DrvQueryFontData.\n");
#endif

    pdev = (PDEVDATA)dhpdev;

    if (bValidatePDEV(pdev) == FALSE)
	return(-1);

    // make sure we have been given a valid font.

    if ((pfo->iFace == 0) || (pfo->iFace > (pdev->cDeviceFonts + pdev->cSoftFonts)))
    {
	RIP("PSCRIPT!DrvQueryFontData: invalid iFace.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return(-1);
    }

    // get the metrics for the given font.

    pntfm = pdev->pfmtable[pfo->iFace - 1].pntfm;

    // get the Notional to Device transform.

    if(!(pxo = FONTOBJ_pxoGetXform(pfo)))
    {
        RIP("PSCRIPT!DrvQueryFontData: pxo == NULL.\n");
        return(-1);
    }

    // get the font transform information.

    ulComplex = XFORMOBJ_iGetXform(pxo, &pdev->cgs.FontXform);

    // get local pointer to IFIMETRICS.

    pifi = (PIFIMETRICS)((CHAR *)pntfm + pntfm->loIFIMETRICS);

    // fill in the appropriate data, depending on iMode.

    switch(iMode)
    {
        case QFD_GLYPHANDBITMAP:
            // We don't actually return bitmaps, but we give them
	    // metrics via this call.

            if (pgd)
            {
                // get a pointer to the character codes.

                pCharCode = ((PBYTE)pntfm + pntfm->loCharMetrics +
                             DWORDALIGN(pntfm->cCharacters * sizeof(USHORT)));

		// get a pointer to the character widths.

                pCharWidth = (PUSHORT)((PBYTE)pntfm + pntfm->loCharMetrics);

                // now get the character width for the given GLYPH.
                // we set our HGLYPHs to be equal to the PostScript
                // character code for that given glyph.  this allows
                // us to quickly search through our font metrics and
                // find the width for the given character.

                // first get a pointer to the character codes.  once
                // the character has been found, we know how much to
                // index into the character widths and find the width
                // in question.

		pCode = pCharCode;

                for (i = 0; (USHORT)i < pntfm->cCharacters; i++)
                {
                    if (*pCharCode++ == (BYTE)hg)
                        break;
                }

		// now get the width in question.

                lWidth = (LONG)pCharWidth[i];

//!!! currently, I am just putting the BBox for the font here, not
//!!! for the character.   Does anyone care?  - kentse.

                // now fill in the GLYPHDATA structure.
                // remember under NT 0,0 is top left, while under
		// PostScript 0,0 is bottom left.  return device coords.

            #ifdef THIS_IS_WRONG
            //!!! but fortunatelly it is also useless except for tt fonts [bodind]
            //!!! rclInk is measured along x,y, not along base,side unit vectors

                ptl1.x = pntfm->rcBBox.left;
                ptl1.y = 0;
                XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);
                pgd->rclInk.left = FXTOL(iHipot(ptfx1.x, ptfx1.y));

                ptl1.x = pntfm->rcBBox.right;
                ptl1.y = 0;
                XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);
                pgd->rclInk.right = FXTOL(iHipot(ptfx1.x, ptfx1.y));

                ptl1.x = 0;
                ptl1.y = -pntfm->rcBBox.bottom;
                XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);
                pgd->rclInk.bottom = FXTOL(iHipot(ptfx1.x, ptfx1.y));


                ptl1.x = 0;
                ptl1.y = -pntfm->rcBBox.top;
                XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);
                pgd->rclInk.top = FXTOL(iHipot(ptfx1.x, ptfx1.y));

            #endif // THIS_IS_WRONG

            // fxInkBottom,Top are measured along pteSide vector:

                ptl1.x = 0;
                ptl1.y = pntfm->rcBBox.bottom;
                XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);
                pgd->fxInkBottom = iHipot(ptfx1.x, ptfx1.y);
                if (pntfm->rcBBox.bottom < 0)
                    pgd->fxInkBottom = -pgd->fxInkBottom;

                ptl1.x = 0;
                ptl1.y = pntfm->rcBBox.top;
                XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);
                pgd->fxInkTop = iHipot(ptfx1.x, ptfx1.y);
                if (pntfm->rcBBox.top < 0)
                    pgd->fxInkTop = -pgd->fxInkTop;

#ifdef WIN31_INTEGER_METRICS
                pgd->fxInkBottom = ROUNDFIX(pgd->fxInkBottom);
                pgd->fxInkTop = ROUNDFIX(pgd->fxInkTop);
#endif

           // aw info, ideally we would like more precission than 28.4
           // in case of text at an angle. [bodind]

                ptl1.x = lWidth;
                ptl1.y = 0;

                XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);

                pgd->fxD = iHipot(ptfx1.x, ptfx1.y);

                pgd->ptqD.x.HighPart = ptfx1.x;
                pgd->ptqD.x.LowPart = 0;
                pgd->ptqD.y.HighPart = ptfx1.y;
                pgd->ptqD.y.LowPart = 0;

#ifdef WIN31_INTEGER_METRICS
                pgd->fxD = ROUNDFIX(pgd->fxD);
                pgd->ptqD.x.HighPart = ROUNDFIX(pgd->ptqD.x.HighPart);
                pgd->ptqD.y.HighPart = ROUNDFIX(pgd->ptqD.y.HighPart);
#endif

            //!!! often wrong but it seems win31 is doing the same thing
            //!!! this may cause char to stick outside the computed
            //!!! background box. try ZapfChancery on NT and Win31

                pgd->fxA = 0;
                pgd->fxAB = pgd->fxD;

                pgd->hg = hg;
            }

	    // size is now just the size of the (in this case non-existant)
	    // bitmap
            return 0;


        case QFD_MAXEXTENTS:
            // if there is no output buffer, just return the size needed.

            if (pv == NULL)
                return((LONG)sizeof(FD_DEVICEMETRICS));

            ASSERTPS(
                cjSize >= sizeof(FD_DEVICEMETRICS),
                "pscript! cjSize < sizeof(FD_DEVICEMETRICS)\n"
                );

            // we have a large enough buffer, so fill it in.

            pdm->flRealizedType = 0;

            // base and side, as used below, basically are vectors
            // describing the orientation of the font.  for example,
            // for a left to right font, the base vector would be
            // (1,0).  the side vector should be in the direction of
            // the ascender, so in the standard case the side vector
            // would be (0,-1), since 0 is up in Windows.

            ptl1.x = 1000;
            ptl1.y = 0;

            XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);

            fxLength = iHipot(ptfx1.x, ptfx1.y);

            pdm->pteBase.x = (FLOAT)ptfx1.x / fxLength;
            pdm->pteBase.y = (FLOAT)ptfx1.y / fxLength;

            ptl1.x = 0;
            ptl1.y = -1000;

            XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);

            fxLength = iHipot(ptfx1.x, ptfx1.y);

            pdm->pteSide.x = (FLOAT)ptfx1.x / fxLength;
            pdm->pteSide.y = (FLOAT)ptfx1.y / fxLength;

            // munge with the FD_REALIZEEXTRA external leading field for
            // win31 compatability.

            if (pgd)
            {
                // -fxLength is the FIX 28.4 lfHeight in pels.

                lfHeight = abs(FXTOL(fxLength));

                // get point size as win31 does.

                dwPointSize = (DWORD)MulDiv(lfHeight, PS_RESOLUTION,
                                            pdev->psdm.dm.dmPrintQuality);

                if (pifi->jWinPitchAndFamily & FF_ROMAN)
                {
                    dwLeadSuggest = 2;
                }
                else if (pifi->jWinPitchAndFamily & FF_SWISS)
                {
                    if (dwPointSize <= 12)
                        dwLeadSuggest = 2;
                    else if (dwPointSize < 14)
                        dwLeadSuggest = 3;
                    else
                        dwLeadSuggest = 4;
                }
                else
                {
                    // default to 19.6%.

                    dwLeadSuggest = (DWORD)MulDiv(dwPointSize, 196,
                                                  ADOBE_FONT_UNITS);
                }

                // get notional internal leading.

                InternalLeading = (pntfm->rcBBox.top - pntfm->rcBBox.bottom) -
                                  ADOBE_FONT_UNITS;

                // make it device coordinates.

                InternalLeading = MulDiv(InternalLeading, lfHeight,
                                         ADOBE_FONT_UNITS);

                if (InternalLeading < 0)
                    InternalLeading = 0;

                fxExtLeading = LTOFX(MulDiv(dwLeadSuggest,
                                      pdev->psdm.dm.dmPrintQuality,
                                      PS_RESOLUTION) - InternalLeading);

                // if the external leading was calculated to be negative, or
                // if this is a fixed pitch font, set external leading to
                // zero.

                if ((fxExtLeading < 0) ||
                    (pifi->jWinPitchAndFamily & FIXED_PITCH))
                    fxExtLeading = 0;

                // fill in the leading field of the FD_REALIZEEXTRA struct.

                ((FD_REALIZEEXTRA *)pgd)->lExtLeading = (LONG)fxExtLeading;

                if (pifi->jWinPitchAndFamily & FIXED_PITCH)
                    ((FD_REALIZEEXTRA *)pgd)->alReserved[0] = 0;
            }

            // cxMax the same as max char width for a and c's are zero:

            ptl1.x = pifi->fwdMaxCharInc;
            ptl1.y = 0;

            XFORMOBJ_bApplyXform(pxo, XF_LTOL, 1, &ptl1, &ptl2);

            // now get the length of the vector.

            pdm->cxMax = iHipot(ptl2.x, ptl2.y);

            // lD is the advance width if the font is fixed pitch,
            // otherwise, set to zero.

            if(pifi->fwdMaxCharInc != pifi->fwdAveCharWidth)
                pdm->lD = 0;
            else
            {
                pdm->lD = (LONG)pdm->cxMax;
            }

            // calculate the max ascender.

            ptl1.x = 0;
            ptl1.y = pntfm->rcBBox.top;

            XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);

            pdm->fxMaxAscender = iHipot(ptfx1.x, ptfx1.y);

            // calculate the max descender.

            ptl1.x = 0;
            ptl1.y = pntfm->rcBBox.bottom;

            if (ptl1.y < 0)
                ptl1.y = -ptl1.y;

            // do the ugly fixed pitch means zero internal leading hack.

            if (pifi->jWinPitchAndFamily & FIXED_PITCH)
            {
                // get notional internal leading.

                InternalLeading = (pntfm->rcBBox.top - pntfm->rcBBox.bottom) -
                                  ADOBE_FONT_UNITS;

                // WFW seems to make all the adjustment here in the
                // MaxDescender, so we will too.

                ptl1.y -= InternalLeading;

                if (ptl1.y < 0)
                    ptl1.y = 0;
            }

            XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx1);

            pdm->fxMaxDescender = iHipot(ptfx1.x, ptfx1.y);

#ifdef WIN31_INTEGER_METRICS
            pdm->fxMaxAscender = ROUNDFIX(pdm->fxMaxAscender);
            pdm->fxMaxDescender = ROUNDFIX(pdm->fxMaxDescender);
#endif

            // calculate the underline position for this font instance.

            ptl1.x = 0;
            ptl1.y = - pifi->fwdUnderscorePosition;

            XFORMOBJ_bApplyXform(pxo, XF_LTOL, 1, &ptl1, &pdm->ptlUnderline1);

            // calculate the strikeout position for this font instance.

            ptl1.x = 0;
            ptl1.y = - (pntfm->fwdLowerCaseAscent / 2);

            XFORMOBJ_bApplyXform(pxo, XF_LTOL, 1, &ptl1, &pdm->ptlStrikeOut);

            // calculate the line thickness.

            ptl1.x = 0;
            ptl1.y = pifi->fwdUnderscoreSize;

            XFORMOBJ_bApplyXform(pxo, XF_LTOL, 1, &ptl1, &pdm->ptlULThickness);

            pdm->ptlSOThickness = pdm->ptlULThickness;

            return(sizeof(FD_DEVICEMETRICS));


        default:
	    RIP("PSCRIPT!DrvQueryFontData: invalid iMode.\n");
	    return(-1);
    }
}


//--------------------------------------------------------------------------
// BOOL DrvQueryAdvanceWidths(
// DHPDEV   dhpdev,
// FONTOBJ *pfo,
// ULONG    iMode,
// HGLYPH  *phg,
// PVOID    plWidths,
// ULONG    cGlyphs);
//
// This routine returns an array of FIX (28.4) widths in pels of the
// indicated glyphs.
//
// History:
//   23-Jan-1993    -by-    Kent Settle       (kentse)
// Wrote it.
//--------------------------------------------------------------------------

BOOL DrvQueryAdvanceWidths(
DHPDEV   dhpdev,
FONTOBJ *pfo,
ULONG    iMode,
HGLYPH  *phg,
PVOID    plWidths,
ULONG    cGlyphs)
{
    PDEVDATA        pdev;
    ULONG           i, j;
    PNTFM           pntfm;
    HGLYPH         *phglyph;
    USHORT         *pwidth;
    USHORT         *pCharWidth;
    PBYTE           pCharCode;
    PBYTE           pCode;
    XFORMOBJ       *pxo;
    POINTL          ptl;
    POINTFIX        ptfx;
#ifdef WIN31_INTEGER_METRICS
    POINTL          ptl2;
    FIX             fxLength;
    LONG            lfHeight;
#endif

    pdev = (PDEVDATA)dhpdev;

    if (bValidatePDEV(pdev) == FALSE)
        return(FALSE);

    // for now we will treat GETWIDTHS and GETEASYWIDTHS the same.

    if ((iMode != QAW_GETWIDTHS) && (iMode != QAW_GETEASYWIDTHS))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
#if DBG
        DbgPrint("PSCRIPT!DrvQueryAdvanceWidths: invalid iMode.\n");
#endif
        return(FALSE);
    }

    // see if there is anything to do.

    if (!cGlyphs)
        return(TRUE);

    // make sure we have been given a valid font.

    if ((pfo->iFace == 0) ||
        (pfo->iFace > (pdev->cDeviceFonts + pdev->cSoftFonts)))
    {
        RIP("PSCRIPT!DrvQueryAdvanceWidths: invalid iFace.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }

    // get the metrics for the given font.

    pntfm = pdev->pfmtable[pfo->iFace - 1].pntfm;

    // get a pointer to the character widths.

    pCharWidth = (PUSHORT)((PBYTE)pntfm + pntfm->loCharMetrics);

    // get a pointer to the character codes.

    pCharCode = ((PBYTE)pntfm + pntfm->loCharMetrics +
                 DWORDALIGN(pntfm->cCharacters * sizeof(USHORT)));

    // get the Notional to Device transform.

    if(!(pxo = FONTOBJ_pxoGetXform(pfo)))
    {
        RIP("PSCRIPT!DrvQueryAdvancedWidths: pxo == NULL.\n");
        return(FALSE);
    }

    // get some local pointers to munge with.

    phglyph = phg;
    pwidth = (USHORT *) plWidths;

    for (i = 0; i < cGlyphs; i++)
    {
        pCode = pCharCode;

        for (j = 0; (USHORT)j < pntfm->cCharacters; j++)
        {
            if (*pCode++ == (BYTE)*phglyph)
                break;
        }

        // now get the width in question.

#ifdef WIN31_INTEGER_METRICS
        ptl.x = 0;
        ptl.y = 1000;

        XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

        fxLength = iHipot(ptfx.x, ptfx.y);

        lfHeight = abs(FXTOL(fxLength));

        *pwidth = LTOFX(MulDiv(pCharWidth[j], lfHeight, ADOBE_FONT_UNITS));
#else
        ptl.x = (LONG)pCharWidth[j];
        ptl.y = 0;

        XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

        *pwidth = (USHORT)iHipot(ptfx.x, ptfx.y);
#endif

        // get the next glyph.

        phglyph++;
        pwidth++;
    }

    return(TRUE);
}
