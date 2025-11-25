//--------------------------------------------------------------------------
//
// Module Name:  PATHS.C
//
// Brief Description:  This module contains the PSCRIPT driver's path
// rendering functions and related routines.
//
// Author:  Kent Settle (kentse)
// Created: 02-May-1991
//
//  26-Mar-1992 Thu 23:53:12 updated  -by-  Daniel Chou (danielc)
//      add the prclBound parameter to the bDoClipObj()
//
// Copyright (c) 1991 - 1992 Microsoft Corporation
//
// This Module contains the following functions:
//	DrvStrokePath
//	DrvFillPath
//	DrvStrokeAndFillPath
//--------------------------------------------------------------------------

#include "pscript.h"
#include "enable.h"

extern ULONG   PSMonoPalette[];
extern ULONG   PSColorPalette[];

extern BOOL bDoClipObj(PDEVDATA, CLIPOBJ *, RECTL *, RECTL *, BOOL *, BOOL *, DWORD);

BOOL DrvCommonPath(PDEVDATA, PATHOBJ *);

//--------------------------------------------------------------------------
// BOOL DrvStrokePath(pso, ppo, pco, pxo, pbo, pptlBrushOrg, plineattrs, mix)
// SURFOBJ	 *pso;
// PATHOBJ	 *ppo;
// CLIPOBJ	 *pco;
// XFORMOBJ  *pxo;
// BRUSHOBJ  *pbo;
// PPOINTL	  pptlBrushOrg;
// PLINEATTRS plineattrs;
// MIX	  mix;
//
//
// Parameters:
//
// Returns:
//   This function returns TRUE.
//
// History:
//   02-May-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvStrokePath(pso, ppo, pco, pxo, pbo, pptlBrushOrg, plineattrs, mix)
SURFOBJ   *pso;
PATHOBJ   *ppo;
CLIPOBJ   *pco;
XFORMOBJ  *pxo;
BRUSHOBJ  *pbo;
PPOINTL    pptlBrushOrg;
PLINEATTRS plineattrs;
MIX        mix;
{
    PDEVDATA	pdev;
    BOOL        bClipping;      // TRUE if there is a clip region.
    ULONG       ulColor;
    BOOL        bMoreClipping;  // TRUE if there is more clipping to handle.
    BOOL        bFirstClipPass;
#ifdef INDEX_PAL
    DEVBRUSH   *pBrush;
    ULONG      *pulColors;
#endif
    RECTFX      rcfxBound;
    RECTL       rclBound;

    UNREFERENCED_PARAMETER(mix);

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (bValidatePDEV(pdev) == FALSE)
    {
	RIP("PSCRIPT!DrvStrokePath: invalid pdev.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
	return(FALSE);
    }

    // deal with LINEATTRS.

    if (!(ps_setlineattrs(pdev, plineattrs, pxo)))
        return(FALSE);

    // output the line color to stroke with.  do this before we handle
    // clipping, so the line color will remain beyond the gsave/grestore.

#ifdef INDEX_PAL
    // just output the solid color if there is one.

    if ((pdev->pntpd->flFlags & COLOR_DEVICE) &&
        (pdev->psdm.dm.dmColor == DMCOLOR_COLOR))
        pulColors = PSColorPalette;
    else
        pulColors = PSMonoPalette;

    if (pbo->iSolidColor != NOT_SOLID_COLOR)
    {
        ps_setrgbcolor(pdev, (PALETTEENTRY *)pulColors + pbo->iSolidColor);
    }
    else
    {
        // get the device brush to draw with.

        pBrush = (DEVBRUSH *)BRUSHOBJ_pvGetRbrush(pbo);

        if (!pBrush)
        {
#if DBG
            DbgPrint("DrvStrokePath: NULL pBrush.\n");
#endif
            // something is wrong! just output black path.

            ulColor = RGB_BLACK;
            ps_setrgbcolor(pdev, (PALETTEENTRY *)&ulColor);
        }
        else
        {
            if (pBrush->iSolidColor == NOT_SOLID_COLOR)
            {
                // get the foreground color.

                ps_setrgbcolor(pdev, ((PALETTEENTRY *)pulColors +
                               *(ULONG *)((PBYTE)pBrush + pBrush->offsetXlate +
                               sizeof(ULONG))));
            }
            else
            {
                ps_setrgbcolor(pdev, (PALETTEENTRY *)&pBrush->iSolidColor);
            }
        }
    }
#else
    if (pbo->iSolidColor == NOT_SOLID_COLOR)
    {
//!!! this needs to be fixed!!! -kentse.
        ulColor = RGB_GRAY;

        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&ulColor);
    }
    else
    {
        // we have a solid brush, so simply output the line color.

        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&pbo->iSolidColor);
    }
#endif

    // get the bounding rectangle for the path.  this is used to checked
    // against the clipping for optimization.

    PATHOBJ_vGetBounds(ppo, &rcfxBound);

    // get a RECTL which is guaranteed to bound the path.

    rclBound.left = FXTOL(rcfxBound.xLeft);
    rclBound.top = FXTOL(rcfxBound.yTop);
    rclBound.right = FXTOL(rcfxBound.xRight + FIX_ONE);
    rclBound.bottom = FXTOL(rcfxBound.yBottom + FIX_ONE);

    bMoreClipping = TRUE;
    bFirstClipPass = TRUE;

    while (bMoreClipping)
    {
        // handle the clipping.

        if (bClipping = bDoClipObj(pdev, pco, NULL, &rclBound, &bMoreClipping,
                                   &bFirstClipPass, MAX_CLIP_RECTS))
            ps_clip(pdev, TRUE);

        if (!(DrvCommonPath(pdev, ppo)))
            return(FALSE);

        // now transform for geometric lines if necessary.

        if (plineattrs->fl & LA_GEOMETRIC)
            ps_geolinexform(pdev, plineattrs, pxo);

        // now stroke the path.

        ps_stroke(pdev, pbo, pptlBrushOrg);

        // restore the CTM if a transform for a geometric line was in effect.

        if (pdev->cgs.dwFlags & CGS_GEOLINEXFORM)
        {
            PrintString(pdev, "SM\n");
            pdev->cgs.dwFlags &= ~CGS_GEOLINEXFORM;
        }

        // restore the clip path to what it was before this call.

        if (bClipping)
            ps_restore(pdev, TRUE);
    }

    return(TRUE);
}


//--------------------------------------------------------------------------
// BOOL DrvFillPath(pso, ppo, pco, pbo, pptlBrushOrg, mix, flOptions)
// SURFOBJ	*pso;
// PATHOBJ	*ppo;
// CLIPOBJ	*pco;
// BRUSHOBJ *pbo;
// PPOINTL	 pptlBrushOrg;
// MIX	 mix;
// FLONG	 flOptions;
//
// Parameters:
//
// Returns:
//   This function returns TRUE.
//
// History:
//   03-May-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvFillPath(pso, ppo, pco, pbo, pptlBrushOrg, mix, flOptions)
SURFOBJ  *pso;
PATHOBJ  *ppo;
CLIPOBJ  *pco;
BRUSHOBJ *pbo;
PPOINTL   pptlBrushOrg;
MIX       mix;
FLONG     flOptions;
{
    PDEVDATA	pdev;
    RECTL       rclBounds;
    RECTFX      rcfxBounds;
    BOOL        bClipping;
    BOOL        bMoreClipping;
    BOOL        bFirstClipPass;

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (bValidatePDEV(pdev) == FALSE)
        return(FALSE);

    bMoreClipping = TRUE;
    bFirstClipPass = TRUE;

    while (bMoreClipping)
    {
        // get the bounding rectangle of the path to pass to ps_patfill.

        PATHOBJ_vGetBounds(ppo, &rcfxBounds);

        rclBounds.left = FXTOL(rcfxBounds.xLeft);
        rclBounds.right = FXTOL(rcfxBounds.xRight) + 1;
        rclBounds.top = FXTOL(rcfxBounds.yTop);
        rclBounds.bottom = FXTOL(rcfxBounds.yBottom) + 1;

        // if there is a clip region, clip to it.  we want to keep this
        // separate from the clip path.

        if (bClipping = bDoClipObj(pdev, pco, NULL, &rclBounds, &bMoreClipping,
                                   &bFirstClipPass, MAX_CLIP_RECTS))
        {
            if (flOptions & FP_WINDINGMODE)
                ps_clip(pdev, TRUE);
            else
                ps_clip(pdev, FALSE);
        }

        // if there was no clip region, we need to output a gsave before we
        // send the clip path, so we can blow it away when we are done.

        if (!bClipping)
            ps_save(pdev, TRUE);

        if (!(DrvCommonPath(pdev, ppo)))
        {
            RIP("PSCRIPT!DrvFillPath: invalid pdev.\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return(FALSE);
        }

        // now fill the path.

        if (!ps_patfill(pdev, pso, flOptions, pbo, pptlBrushOrg, mix, &rclBounds,
                        FALSE, TRUE))
            return(FALSE);

        ps_restore(pdev, TRUE);
    }

    return(TRUE);
}


//--------------------------------------------------------------------------
// BOOL DrvStrokeAndFillPath(pso, ppo, pco, pxo, pboStroke, plineattrs,
//			     pboFill, pptlBrushOrg, mixFill, flOptions)
// SURFOBJ	 *pso;
// PATHOBJ	 *ppo;
// CLIPOBJ	 *pco;
// XFORMOBJ  *pxo;
// BRUSHOBJ  *pboStroke;
// PLINEATTRS plineattrs;
// BRUSHOBJ  *pboFill;
// PPOINTL	  pptlBrushOrg;
// MIX	  mixFill;
// FLONG	  flOptions;
//
// Parameters:
//
// Returns:
//   This function returns TRUE.
//
// History:
//   03-May-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvStrokeAndFillPath(pso, ppo, pco, pxo, pboStroke, plineattrs,
			  pboFill, pptlBrushOrg, mixFill, flOptions)
SURFOBJ   *pso;
PATHOBJ   *ppo;
CLIPOBJ   *pco;
XFORMOBJ  *pxo;
BRUSHOBJ  *pboStroke;
PLINEATTRS plineattrs;
BRUSHOBJ  *pboFill;
PPOINTL    pptlBrushOrg;
MIX        mixFill;
FLONG      flOptions;
{
    PDEVDATA	pdev;
    RECTL       rclBounds;
    RECTFX      rcfxBounds;
    BOOL	bClipping;
    ULONG       ulColor;
    BOOL        bMoreClipping;
    BOOL        bFirstClipPass;
#ifdef INDEX_PAL
    DEVBRUSH   *pBrush;
    ULONG      *pulColors;
#endif

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (bValidatePDEV(pdev) == FALSE)
        return(FALSE);

    // deal with LINEATTRS.

    if (!(ps_setlineattrs(pdev, plineattrs, pxo)))
        return(FALSE);

    // output the line color to stroke with.  do this before we handle
    // clipping, so the line color will remain beyond the gsave/grestore.

#ifdef INDEX_PAL
    // just output solid color if there is one.

    if ((pdev->pntpd->flFlags & COLOR_DEVICE) &&
        (pdev->psdm.dm.dmColor == DMCOLOR_COLOR))
        pulColors = PSColorPalette;
    else
        pulColors = PSMonoPalette;

    if (pboStroke->iSolidColor != NOT_SOLID_COLOR)
    {
        ps_setrgbcolor(pdev, (PALETTEENTRY *)pulColors + pboStroke->iSolidColor);
    }
    else
    {
        // get the device brush to draw with.

        pBrush = (DEVBRUSH *)BRUSHOBJ_pvGetRbrush(pboStroke);

        if (!pBrush)
        {
#if DBG
            DbgPrint("DrvStrokeAndFillPath: NULL pBrush.\n");
#endif
            // something is wrong! stroke with black.

            ulColor = RGB_BLACK;
            ps_setrgbcolor(pdev, (PALETTEENTRY *)&ulColor);
        }
        else
        {
            if (pBrush->iSolidColor == NOT_SOLID_COLOR)
            {
                // get the foreground color.

                ps_setrgbcolor(pdev, ((PALETTEENTRY *)pulColors +
                               *(ULONG *)((PBYTE)pBrush + pBrush->offsetXlate +
                               sizeof(ULONG))));
            }
            else
            {
                ps_setrgbcolor(pdev, (PALETTEENTRY *)&pBrush->iSolidColor);
            }
        }
    }
#else
    if (pboStroke->iSolidColor == NOT_SOLID_COLOR)
    {
//!!! this needs to be fixed!!! -kentse.
        ulColor = RGB_GRAY;

        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&ulColor);
    }
    else
    {
        // we have a solid brush, so simply output the line color.

        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&pboStroke->iSolidColor);
    }
#endif

    bMoreClipping = TRUE;
    bFirstClipPass = TRUE;

    while (bMoreClipping)
    {
        // get the bounding rectangle of the path to pass to ps_patfill.

        PATHOBJ_vGetBounds(ppo, &rcfxBounds);

        rclBounds.left = FXTOL(rcfxBounds.xLeft);
        rclBounds.right = FXTOL(rcfxBounds.xRight) + 1;
        rclBounds.top = FXTOL(rcfxBounds.yTop);
        rclBounds.bottom = FXTOL(rcfxBounds.yBottom) + 1;

        // if there is a clip region, clip to it.  we want to keep this
        // separate from the clip path.

        if (bClipping = bDoClipObj(pdev, pco, NULL, &rclBounds, &bMoreClipping,
                                   &bFirstClipPass, MAX_CLIP_RECTS))
        {
            if (flOptions & FP_WINDINGMODE)
                ps_clip(pdev, TRUE);
            else
                ps_clip(pdev, FALSE);
        }

        // if there was no clip region, we need to output a gsave before we
        // send the clip path, so we can blow it away when we are done.

        if (!bClipping)
            ps_save(pdev, TRUE);

        if (!(DrvCommonPath(pdev, ppo)))
        {
            RIP("PSCRIPT!DrvStrokeAndFillPath: invalid pdev.\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return(FALSE);
        }

        // save the path.  then fill it.  then restore the path which
        // was wiped out when it was filled so we can stroke it.  TRUE
        // means to do a gsave, not a save command.

        if (!ps_save(pdev, TRUE))
            return(FALSE);

        if (!ps_patfill(pdev, pso, flOptions, pboFill, pptlBrushOrg, mixFill,
                        &rclBounds, FALSE, TRUE))
            return(FALSE);

        if (!ps_restore(pdev, TRUE))
            return(FALSE);

        // now transform for geometric lines if necessary.

        if (plineattrs->fl & LA_GEOMETRIC)
            ps_geolinexform(pdev, plineattrs, pxo);

        // now stroke the path.  remember that ps_patfill will have marked that
        // the path no longer exists.  since we surrounded it with gsave -
        // grestore, we know otherwise.

        pdev->cgs.dwFlags |= CGS_PATHEXISTS;
        ps_stroke(pdev, pboStroke, pptlBrushOrg);

        // restore the CTM if a transform for a geometric line was in effect.

        if (pdev->cgs.dwFlags & CGS_GEOLINEXFORM)
        {
            PrintString(pdev, "SM\n");
            pdev->cgs.dwFlags &= ~CGS_GEOLINEXFORM;
        }

        ps_restore(pdev, TRUE);
    }

    return(TRUE);
}


//--------------------------------------------------------------------------
// BOOL DrvCommonPath(pdev, ppo)
// PDEVDATA   pdev;
// PATHOBJ   *ppo;
//
//
// Parameters:
//
// Returns:
//   This function returns TRUE.
//
// History:
//   02-May-1991    -by-    Kent Settle     [kentse]
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvCommonPath(pdev, ppo)
PDEVDATA   pdev;
PATHOBJ   *ppo;
{
    PATHDATA	pathdata;
    POINTL	ptl, ptl1, ptl2;
    POINTFIX    *pptfx;
    LONG	cPoints;
    BOOL	bMore;

    // before we enumerate the path, let's make sure we have a clean start.

    ps_newpath(pdev);

    // enumerate the path, doing what needs to be done along the way.

    PATHOBJ_vEnumStart(ppo);

    do
    {
        bMore = PATHOBJ_bEnum(ppo, &pathdata);

        // get a local pointer to the array of POINTFIX's.

        pptfx = pathdata.pptfx;
        cPoints = (LONG)pathdata.count;

        if (pathdata.flags & PD_BEGINSUBPATH)
        {
            // the first path begins a new subpath.  it is not connected
            // to the previous subpath.  note that if this flag is not
            // set, then the starting point for the first curve to be
            // drawn from this data is the last point returned in the
            // previous call.

#if 0
            if (pathdata.flags & PD_RESETSTYLE)
            {
		// this bit is defined only if this record begins a new
                // subpath.  if set, it indicates that the style state
                // should be reset to zero at the beginning of the subpath.
                // if not set, the style state is defined by the
                // LINEATTRS, or continues from the previous path.

#if DBG
		DbgPrint("DrvCommonPath: PD_RESETSTYLE flag set.\n");
#endif
                //!!! fill in here - kentse
            }
#endif

            // begin the subpath within the printer by issuing a moveto
            // command.

            ptl.x = FXTOL(pptfx->x);
            ptl.y = FXTOL(pptfx->y);
            pptfx++;
            cPoints--;

            ps_moveto(pdev, &ptl);
        }

        if (pathdata.flags & PD_BEZIERS)
        {
            // if set, then each set of three control points returned for
            // this call describe a Bezier curve.  if clear then each
            // control point describes a line segment.	a starting point
            // for either type is either explicit at the beginning of the
            // subpath, or implicit as the endpoint of the previous curve.

            // there had better be the correct number of points if we are
            // going to draw curves.

            if ((cPoints % 3) != 0)
            {
		RIP("PSCRIPT!DrvCommonPath: incompatible number of points.\n");
		SetLastError(ERROR_INVALID_PARAMETER);
                return(FALSE);
            }

            // now draw the bezier for each set of points.

            while (cPoints > 0)
            {
                ptl.x = FXTOL(pptfx->x);
                ptl.y = FXTOL(pptfx->y);
                pptfx++;
                ptl1.x = FXTOL(pptfx->x);
                ptl1.y = FXTOL(pptfx->y);
                pptfx++;
                ptl2.x = FXTOL(pptfx->x);
                ptl2.y = FXTOL(pptfx->y);
                pptfx++;

                ps_curveto(pdev, &ptl, &ptl1, &ptl2);
                cPoints -= 3;
            }
        }
        else
        {
            // draw the line segment for each point.

            while (cPoints-- > 0)
            {
                ptl.x = FXTOL(pptfx->x);
                ptl.y = FXTOL(pptfx->y);
                pptfx++;

                ps_lineto(pdev, &ptl);
            }
        }
    } while(bMore);

    if (pathdata.flags & PD_ENDSUBPATH)
    {
        // the last point in the array ends the subpath.  this subpath
        // may be open or closed depending on the PD_CLOSEFIGURE flag.
        // if there is more data to be returned in the path, then the
        // next record will begin a new subpath.  note that a single
        // record might begin and end a subpath.

        if (pathdata.flags & PD_CLOSEFIGURE)
        {
            // this bit is only defined if the record ends a subpath.  if
            // set, then there is an implicit line segment connecting
            // the last point of the subpath with the first point.  if
            // such a closed subpath is being stroked, then joins are used
            // all around the path, and there are no end caps.	if this
            // flag is not set then the subpath is considered open, even
            // if the first and last points happen to be coincident.  in
            // that case, end caps should be drawn.  this flag is not
            // relevant for filling, since all subpaths are assumed closed
            // when a path is filled.

            ps_closepath(pdev);
        }
    }

    return(TRUE);
}
