//--------------------------------------------------------------------------
//
// Module Name:  TEXTOUT.C
//
// Brief Description:  This module contains the PSCRIPT driver's DrvTextOut
// function and related routines.
//
// Author:  Kent Settle (kentse)
// Created: 12-Feb-1991
//
//  26-Mar-1992 Thu 23:29:37 updated  -by-  Daniel Chou (danielc)
//      Add another parameter to bDoClipObj() so it also return the bounding
//      rectangle to the clip region for halftone purpose.
//
// Copyright (c) 1991 - 1992 Microsoft Corporation
//--------------------------------------------------------------------------

#include "stdlib.h"
#include <string.h>
#include "pscript.h"
#include "enable.h"
#include "mapping.h"
#include "resource.h"

extern HMODULE     ghmodDrv;            // GLOBAL MODULE HANDLE.

extern TT_FONT_MAPPING TTFamilyTable[]; // tables.h.
extern TT_FONT_MAPPING TTFontTable[];   // tables.h.
extern BOOL DrvCommonPath(PDEVDATA, PATHOBJ *);
extern VOID ps_show(PDEVDATA, STROBJ *, FLONG, TEXTDATA *);
extern DWORD PSFIXToBuffer(CHAR *, PS_FIX);
extern PS_FIX GetPointSize(PDEVDATA, FONTOBJ *, XFORM *);

#ifdef INDEX_PAL
extern ULONG   PSMonoPalette[];
extern ULONG   PSColorPalette[];
#endif

PSZ apszRemapCode[] =
    {
    "/reencode {findfont begin currentdict d length dict begin {",
    "1 index /FID ne {def} {pop pop} ifelse} forall /FontName exch def",
    "d length 0 ne {/Encoding Encoding 256 array copy def 0 exch {",
    "d type /nametype eq {Encoding 2 index 2 index put pop 1 add",
    "}{exch pop} ifelse} forall} if pop currentdict d end end",
    "/FontName get exch definefont pop} bd",
    NULL
    } ;

typedef struct
{
    WCHAR       wc;         // UNICODE code point.
    POINTL      ptlpgp;     // pointl as defined by GLYPHPOS structs.
    POINTFIX    ptfxorg;    // pointfix as defined by original font.
} TEXTDELTA;

#define MAX_LINE_LENGTH     70

// macro for scaling between TrueType and Adobe fonts.

#define TTTOADOBE(x)    (((x) * ADOBE_FONT_UNITS) / pifi->fwdUnitsPerEm)

// declaration of routines residing in this module.

BOOL bDoClipObj(PDEVDATA, CLIPOBJ *, RECTL *, RECTL *, BOOL *, BOOL *, DWORD);
BOOL DrawGlyphs(PDEVDATA, DWORD, GLYPHPOS *, FONTOBJ *, STROBJ *, TEXTDATA *, PWSZ);
BOOL RemapDeviceChar(PDEVDATA, PCHAR, STROBJ *, FLONG, BOOL, TEXTDATA *);
BOOL RemapUnicodeChar(PDEVDATA, PWCHAR, STROBJ *, FLONG, BOOL, TEXTDATA *);
BOOL RemapGDIChar(PDEVDATA, STROBJ *, GLYPHPOS *, DLFONT *, BOOL *, FLONG, TEXTDATA *);
BOOL SelectFont(PDEVDATA, FONTOBJ *, TEXTDATA *);
VOID RemapFont(PDEVDATA, STROBJ *, FLONG, BOOL, TEXTDATA *);
VOID CharBitmap(PDEVDATA, GLYPHPOS *);
BOOL DownloadBitmapFont(PDEVDATA, FONTOBJ *);
BOOL DownloadOutlineFont(PDEVDATA, FONTOBJ *);
BOOL SetFontRemap(PDEVDATA, DWORD);
BOOL QueryFontRemap(PDEVDATA, DWORD);
DWORD SubstituteIFace(PDEVDATA, FONTOBJ *);
LONG iHipot(LONG, LONG);
BOOL IsJustifiedText(PDEVDATA, FONTOBJ *, STROBJ *, POINTPSFX *,
                     POINTPSFX *, TEXTDATA *);
BOOL FillDeltaArray(PDEVDATA, FONTOBJ *, GLYPHPOS *, STROBJ *, TEXTDELTA *,
                    DWORD, PWSZ);
PWSTR GetUnicodeString(PDEVDATA, FONTOBJ *, STROBJ *);
VOID ConstructUCString(GLYPHPOS *, PUCMap, DWORD, PWSTR);
BOOL GetDeviceWidths(PDEVDATA, FONTOBJ *, GLYPHDATA  *, HGLYPH);

//--------------------------------------------------------------------------
// BOOL DrvTextOut (pso, pstro, pfo, pco, prclExtra, prclOpaque, pboFore,
//         pboOpaque, pptBrushOrg, mix)
// SURFOBJ        *pso;
// STROBJ        *pstro;
// FONTOBJ        *pfo;
// CLIPOBJ        *pco;
// RECTL        *prclExtra;
// RECTL        *prclOpaque;
// BRUSHOBJ    *pboFore;
// BRUSHOBJ    *pboOpaque;
// POINTL        *pptlBrushOrg;
// MIX          mix;
//
// The graphics engine will call this routine to render a set of glyphs at
// specified positions. In order to make things clear, we will make a short
// mathematical detour. The parameters of the function unambiguously divides
// the entire set of device space pixels into three proper subsets denoted
// foreground, background, and transparent. When the text is rendered to the
// surface, the foreground pixels are rendered with a foreground brush, the
// background pixels the background brush, and the transparent pixels are left
// untouched. The foreground pixels are defined by first forming the union of
// all the glyph pixels with the pixels of the extra rectangles, then
// intersecting the result with the pixels of the clipping region. The set
// of pixels comprising the extra rectangles are defined by (up to) three
// rectangles pointed to by prclExt. These rectangles are used to simulate
// effects like underlining and strike through glyphs. The background set of
// pixels is defined as the intersection of three sets: (1) the pixels of the
// background rectangle; (2) the complement of the foreground; (3) the
// clipping region. Any pixels that are not part of either the foreground or
// background sets are defined to be transparent. The input parameters to
// DrvTextOut define two sets of pixels foreground and opaque. The driver must
// render the surface so that the result is identical to a process where the
// opaque pixels are rendered first with the opaque brush, then the foreground
// pixels are rendered with the foreground brush. Each of these operations is
// limited by clipping. The foreground set of pixels is defined to be the
// union of the pixels of the glyphs and the pixels of the "extra" rectangles
// at prclExtra. These extra rectangles are used to simulate strike-through or
// underlines. The opaque pixels are defined by the opaque rectangle at
// prclOpaque. The foreground and opaque pixels are regarded as a screen
// through which color is brushed onto the surface.  The glyphs of the font
// do not have color in themselves. The input parameters to DrvTextOut
// define the set of glyph pixels, the set of extra rectangles, the opaque
// rectangle, and the clipping region. It is the responsibility of the driver
// to calculate and then render the set of foreground and opaque pixels.
//
// Parameters:
//   pso
//     Pointer to a SURFOBJ.
//
//   pstro
//     Pointer to a STROBJ. This defines the glyphs to be rendered and the
//     positions where they are to be placed.
//
//   pfo
//     Pointer to a FONTOBJ. This is used to retrieve information about the
//     font and its glyphs.
//
//   pco
//     Pointer to a CLIPOBJ. This defines the clipping region through which
//     all rendering must be done.  No pixels can be affected outside the
//     clipping region.
//
//   prclExtra
//     Pointer to a null terminated array of rectangles.  These rectangles
//     are bottom right exclusive.  The pixels of the rectangles are to be
//     combined with the pixels of the glyphs to produce the foreground
//     pixels. The extra rectangles are used to simulate underlining or strike
//     out. If prclExtra is NULL then there are no extra rectangles to be
//     rendered.  If the prclExtra is not NULL then the rectangles are read
//     until a null rectangle is reached.  A null rectangle has both coordinates
//     of both points set to zero.
//
//   prclOpaque
//     Pointer to a single opaque rectangle.  This rectangle is bottom
//     right exclusive. Pixels within this rectangle (that are not foreground
//     and not clipped) are to be rendered with the opaque brush.  If this
//     argument is NULL then no opaque pixels are to be rendered.
//
//   pboFore
//     Pointer to the brush object to be used for the foreground pixels.
//     The fill pattern is defined by this brush.  A GDI service,
//     BRUSHOBJ_pvDevBrush, is provided to find the device's realization of
//     the brush.
//
//   pboOpaque
//     Pointer to the brush object for the opaque pixels.  Both the foreground
//     and background mix modes for this brush are assumed to be overpaint.
//
//   pptlBrushOrg
//     Pointer to a POINTL defining the brush origin for both brushes.
//
//   mix
//     Foreground and background raster operations (mix modes) for pboFore.
//
// Returns:
//   TRUE if successful.  Otherwise FALSE, and an error code is logged.
//
// History:
//   12-Feb-1991     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrvTextOut(
SURFOBJ    *pso,
STROBJ     *pstro,
FONTOBJ    *pfo,
CLIPOBJ    *pco,
PRECTL      prclExtra,
PRECTL      prclOpaque,
BRUSHOBJ   *pboFore,
BRUSHOBJ   *pboOpaque,
PPOINTL     pptlOrg,
MIX         mix)
{
    PDEVDATA    pdev;
    BOOL        bMore;
    DEVBRUSH   *pBrush;
    RECTL       rclBounds;
    FONTINFO    fi;
    DWORD       cGlyphs;
    GLYPHPOS   *pgp;
    BOOL	bClipping;
    TEXTDATA   *pdata;
    BOOL        bFirstClipPass;
    ULONG       ulColor;
    ULONG      *pulColors;

    // make sure we have been given valid pointers.

    if ((pso == (SURFOBJ *)NULL) || (pstro == (STROBJ *)NULL) ||
        (pfo == (FONTOBJ *)NULL) || (pco == (CLIPOBJ *)NULL))
    {
	RIP("PSCRIPT!DrvTextOut: NULL pointer passed in.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }

    // get the pointer to our DEVDATA structure and make sure it is ours.

    pdev = (PDEVDATA) pso->dhpdev;

    if (bValidatePDEV(pdev) == FALSE)
    {
	RIP("PSCRIPT!DrvTextOut: invalid PDEV.");
	SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }

    // make sure we have been given a valid font.

    if ( (pfo->flFontType & DEVICE_FONTTYPE) &&
         (pfo->iFace > (pdev->cDeviceFonts + pdev->cSoftFonts)) )
    {
	RIP("PSCRIPT!DrvTextOut: invalid iFace.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }

    // allocate and initialize a TEXTDATA structure.

    if (!(pdata = (TEXTDATA *)HeapAlloc(pdev->hheap, 0, sizeof(TEXTDATA))))
    {
        RIP("PSCRIPT!DrvTextOut: HeapAlloc for TEXTDATA failed.");
        return(FALSE);
    }

    pdata->iFace = pfo->iFace;
    pdata->bFontSubstitution = FALSE;
    pdata->ptSpace.x = 0;
    pdata->ptSpace.y = 0;
    pdata->ptNonSpace.x = 0;
    pdata->ptNonSpace.y = 0;

    if ((pfo->flFontType & TRUETYPE_FONTTYPE) &&
        (pdev->psdm.dwFlags & PSDEVMODE_FONTSUBST))
    {
        if (pdata->iFace = SubstituteIFace(pdev, pfo))
            pdata->bFontSubstitution = TRUE;
        else
            pdata->iFace = pfo->iFace;
    }

    pdata->bDeviceFont = (pdata->bFontSubstitution ||
                          (pfo->flFontType & DEVICE_FONTTYPE) );

    pdata->bJustification = IsJustifiedText(pdev, pfo, pstro, &pdata->ptSpace,
                                            &pdata->ptNonSpace, pdata);

    // clear the font download threshold flag.

    pdev->cgs.dwFlags &= ~CGS_DLFONTTHRESHOLD;

    // select the current font in the printer from the given FONTOBJ.

    if (!SelectFont(pdev, pfo, pdata))
    {
#if DBG
        DbgPrint("PSCRIPT!DrvTextOut: SelectFont failed.\n");
#endif
        HeapFree(pdev->hheap, 0, (PVOID)pdata);
        return(FALSE);
    }

    // handle the clip object passed in.
    bFirstClipPass = TRUE;

    bClipping = bDoClipObj(pdev, pco, NULL, NULL, NULL,
                           &bFirstClipPass, MAX_CLIP_RECTS);

    if (bClipping)
        ps_clip(pdev, TRUE);

    // output the Opaque rectangle if necessary.  this is a background
    // rectangle that goes behind the foreground text, therefore, send
    // it to the printer before the text.

    if (prclOpaque)
    {
        // define the opaque rectangle in the printer.

        ps_newpath(pdev);
        ps_box(pdev, prclOpaque);

        // call the driver's filling routine.  this routine will do the
        // right thing with the brush.

        if (!ps_patfill(pdev, pso, (FLONG)FP_WINDINGMODE, pboOpaque, pptlOrg, mix,
                        prclOpaque, FALSE, FALSE))
        {
            HeapFree(pdev->hheap, 0, (PVOID)pdata);
            return(FALSE);
        }
    }

    // output the text color to draw with.

#ifdef INDEX_PAL
    // just output solid color if there is one.

    if ((pdev->pntpd->flFlags & COLOR_DEVICE) &&
        (pdev->psdm.dm.dmColor == DMCOLOR_COLOR))
        pulColors = PSColorPalette;
    else
        pulColors = PSMonoPalette;

    if (pboFore->iSolidColor != NOT_SOLID_COLOR)
    {
        ps_setrgbcolor(pdev, (PALETTEENTRY *)pulColors + pboFore->iSolidColor);
    }
    else
    {
        // get the device brush to draw with.

        pBrush = (DEVBRUSH *)BRUSHOBJ_pvGetRbrush(pboFore);

        if (!pBrush)
        {
#if DBG
            DbgPrint("DrvTextOut: NULL pBrush.\n");
#endif
            // something is wrong!  let's print some black text.

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
    if (pboFore->iSolidColor == NOT_SOLID_COLOR)
    {
        // this is not a solid brush, so get a pointer to the
        // realized brush.

#if DBG
        DbgPrint("DrvTextOut: non-solid text brush, defaulting to black.\n");
#endif
        ulColor = RGB_BLACK;
        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&ulColor);
    }
    else
    {
        // we have a solid brush, so simply output the line color.

        ps_setrgbcolor(pdev, (BGR_PAL_ENTRY *)&pboFore->iSolidColor);
    }
#endif


    // get some information about the font.

    FONTOBJ_vGetInfo(pfo, sizeof(FONTINFO), &fi);

    // get the GLYPHPOS's, directly or indirectly.

    if (pstro->pgp)
    {
        if (!DrawGlyphs(pdev, pstro->cGlyphs, pstro->pgp, pfo, pstro, pdata, pstro->pwszOrg))
        {
            RIP("PSCRIPT!DrvTextOut: DrawGlyphs failed.\n");
            HeapFree(pdev->hheap, 0, (PVOID)pdata);
            return(FALSE);
        }
    }
    else
    {
        PWSZ pwszCur = pstro->pwszOrg;

        // prepare to enumerate the string properly.

        STROBJ_vEnumStart(pstro);

        // now draw the text.

        do
        {
            bMore = STROBJ_bEnum(pstro, &cGlyphs, &pgp);

            if (!DrawGlyphs(pdev, cGlyphs, pgp, pfo, pstro, pdata, pwszCur))
            {
                RIP("PSCRIPT!DrvTextOut: DrawGlyphs failed.\n");
                HeapFree(pdev->hheap, 0, (PVOID)pdata);
                return(FALSE);
            }

            pwszCur += cGlyphs;

        } while (bMore);
    }
    // invalidate the current position so it will get updated next time.

    pdev->cgs.ptlCurPos.x = -1;
    pdev->cgs.ptlCurPos.y = -1;

    // output the extra rectangles if necessary.  These rectangles are
    // bottom right exclusive.    the pels of the rectangles are to be
    // combined with the pixels of the glyphs to produce the foreground
    // pels.  the extra rectangles are used to simulate underlining or
    // strikeout.

    if (prclExtra)
    {
        // output a newpath command to the printer.

        ps_newpath(pdev);

        // set up bounding rectangle.

        rclBounds = *prclExtra;

        // output each Extra rectangle until we find the terminating
        // retangle with all NULL coordinates.

        while ((prclExtra->right != prclExtra->left) ||
               (prclExtra->top != prclExtra->bottom) ||
               (prclExtra->right != 0L) ||
               (prclExtra->top != 0L))
        {
            ps_box(pdev, prclExtra);

            // update the bounding rectangle if necessary.

            if (prclExtra->left < rclBounds.left)
                rclBounds.left = prclExtra->left;
            if (prclExtra->right > rclBounds.right)
                rclBounds.right = prclExtra->right;
            if (prclExtra->top < rclBounds.top)
                rclBounds.top = prclExtra->top;
            if (prclExtra->bottom > rclBounds.bottom)
                rclBounds.bottom = prclExtra->bottom;

            prclExtra++;
        }

        // call the driver's filling routine.  this routine will do the
        // right thing with the brush.

        if (!ps_patfill(pdev, pso, (FLONG)FP_WINDINGMODE, pboFore, pptlOrg, mix,
                        &rclBounds, FALSE, FALSE))
        {
            HeapFree(pdev->hheap, 0, (PVOID)pdata);
            return(FALSE);
        }
    }

    if (bClipping)
	ps_restore(pdev, TRUE);

    // if we have hit the downloaded font threshold, then we are doing
    // a save/restore around every textout call.

    if (pdev->cgs.dwFlags & CGS_DLFONTTHRESHOLD)
        ps_restore(pdev, FALSE);

    // free up memory.

    HeapFree(pdev->hheap, 0, (PVOID)pdata);

    return(TRUE);
}

//--------------------------------------------------------------------------
// BOOL bDoClipObj(pdev, pco, prclClipBound, pbMoreClipping,
//                 pbFirstPass, cRectLimit)
// PDEVDATA    pdev;
// CLIPOBJ    *pco;
// RECTL      *prclClipBound;
// BOOL       *pbMoreClipping;
// BOOL       *pbFirstPass;
// DWORD       cRectLimit;
//
// This routine will determine the clipping region as defined in pco, and
// send the appropriate commands to the printer to set the clip region
// in the printer.
//
// Parameters:
//   pdev
//     Pointer to our DEVDATA structure.
//
//   pco
//     Pointer to a CLIPOBJ. This defines the clipping region through which
//     all rendering must be done.  No pixels can be affected outside the
//     clipping region.
//
//  bBitblt
//      True if called from bitblt function
//
//  prclBound
//      If not NULL then it return the bounding rectangle for the clipping
//      region, the returning rclBound only valid if return value is TRUE.
//
// Returns:
//   This routine returns TRUE if a clippath was sent to the printer,
//   otherwise FALSE.
//
// History:
//  26-Mar-1992 Thu 23:33:58 updated  -by-  Daniel Chou (danielc)
//      add prclBound to accumulate the bounding rectangle for the clipping
//      region.
//
//   12-Feb-1991     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL bDoClipObj(pdev, pco, prclClipBound, prclTarget, pbMoreClipping,
                pbFirstPass, cRectLimit)
PDEVDATA    pdev;
CLIPOBJ    *pco;
RECTL      *prclClipBound;
RECTL      *prclTarget;
BOOL       *pbMoreClipping;
BOOL       *pbFirstPass;
DWORD       cRectLimit;
{
    short       iComplex;
    ENUMRECTS   buffer;
    BOOL        bMore;
    ULONG       cRects;
    RECTL       rclClipBound;

    // assume all clipping will be done within this one call.

    if (pbMoreClipping)
        *pbMoreClipping = FALSE;

    if (pco == NULL)
	return(FALSE);

    iComplex = (short)pco->iDComplexity;

    switch(iComplex)
    {
        case DC_TRIVIAL:
            // in this case, there is no clipping.  Therefore, we have
            // no commands to send to the printer.

	    return (FALSE);

        case DC_RECT:
            // check to see if the target rectangle fits inside the clip
            // rectangle.  if it does, don't do clipping.

            if (prclTarget)
            {
                if ((pco->rclBounds.left <= prclTarget->left) &&
                    (pco->rclBounds.top <= prclTarget->top) &&
                    (pco->rclBounds.right >= prclTarget->right) &&
                    (pco->rclBounds.bottom >= prclTarget->bottom))
                {
                    // I see no reason to clip this, do you?

                    return(FALSE);
                }
            }

            // in this case, we are clipping to a single rectangle.
            // get it from the CLIPOBJ, then send it to the printer.

            buffer.arcl[0] = pco->rclBounds;

            // send a newpath command, then the clip rectangle to
	    // the printer.  TRUE means to do a gsave, not a save command.

	    if (!ps_save(pdev, TRUE))
		return(FALSE);

            ps_newpath(pdev);
            ps_box(pdev, &buffer.arcl[0]);

            if (prclClipBound)
                *prclClipBound = buffer.arcl[0];        // this is the bound

            break;

        case DC_COMPLEX:
            // in this case, we are clipping to a complex clip region.
            // enumerate the clip region from the CLIPOBJ, and send the
            // entire clip region to the printer.

            //
            // 26-Mar-1992 Thu 23:49:59 updated  -by-  Daniel Chou (danielc)
            //
            // Initialize the empty bound then acculate it later,
            //
            //  !!!! This may changed, we need to check with engine guys to
            //       see if you can just pick up the clip region bounding
            //       rectangle from pco->rclBounds, if so we need to deleted
            //       all accumulation codes.
            //

            if (prclClipBound)
            {
                rclClipBound.top    =
                rclClipBound.left   = 0x7fffffffL;
                rclClipBound.right  =
                rclClipBound.bottom = 0;
            }

            // send a newpath command to the printer.

	    if (!ps_save(pdev, TRUE))
		return(FALSE);

            ps_newpath(pdev);

            // set up to enumerate the clip region, but just do this once.

            if (*pbFirstPass == TRUE)
            {
                cRects = CLIPOBJ_cEnumStart(pco, FALSE, CT_RECTANGLES, CD_ANY, 0);
                *pbFirstPass = FALSE;
            }

            // now get each clipping rectangle from the engine, and
            // send it down to the printer.

            bMore = TRUE;

            cRects = 0;

            while(bMore)
            {
                bMore = CLIPOBJ_bEnum(pco, sizeof(buffer), &buffer.c);

                if (buffer.c == 0)
                {
                    bMore = FALSE;
                    break;
                }

                ps_box(pdev, &buffer.arcl[0]);

                if (prclClipBound)
                {
                    if (rclClipBound.top > buffer.arcl[0].top)
                        rclClipBound.top = buffer.arcl[0].top;

                    if (rclClipBound.left > buffer.arcl[0].left)
                        rclClipBound.left = buffer.arcl[0].left;

                    if (rclClipBound.right < buffer.arcl[0].right)
                        rclClipBound.right = buffer.arcl[0].right;

                    if (rclClipBound.bottom < buffer.arcl[0].bottom)
                        rclClipBound.bottom = buffer.arcl[0].bottom;
                }

                if (pbMoreClipping)
                {
                    cRects++;
                    if (cRects >= cRectLimit)
                    {
                        *pbMoreClipping = TRUE;
                        break;
                    }
                }
            }

            // now intersect our new complex region with the existing
            // clipping region.

            if (prclClipBound)
                *prclClipBound = rclClipBound;

            break;

        default:
            // if we get here, we have been passed an invalid pco->iDComplexity.
            // in this case, we will RIP, then treat as trivial clipping case.

            RIP("vDoClipObj: invalid pco->iDComplexity.\n");
    }

    return(TRUE);
}


//--------------------------------------------------------------------------
// BOOL DrawGlyphs(pdev, cGlyphs, pgp, pfo, pstro, pdata, pwsz)
// PDEVDATA         pdev;
// DWORD            cGlyphs;
// GLYPHPOS        *pgp;
// FONTOBJ         *pfo;
// STROBJ          *pstro;
// TEXTDATA        *pdata;
// PWSZ             pwsz;
//
// This routine will output the given glyph at the given position.
//
// Parameters:
//   pdev
//     Pointer to our DEVDATA structure.
//
// Returns:
//   This routine returns no value.
//
// History:
//   26-Apr-1991     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL DrawGlyphs(pdev, cGlyphs, pgp, pfo, pstro, pdata, pwsz)
PDEVDATA         pdev;
DWORD            cGlyphs;
GLYPHPOS        *pgp;
FONTOBJ         *pfo;
STROBJ          *pstro;
TEXTDATA        *pdata;
PWSZ             pwsz;
{
    DWORD	    cTmp;
    PS_FIX	    psfxWidth, psfxHeight;
    DLFONT         *pDLFont;
    BOOL            bString;
    DWORD           i;
    FLONG           flAccel;
    PWSTR           pwstrString;

    // get a local copy of the accelerators to munge with.

    flAccel = pstro->flAccel;

    // if we have a TrueType font, and we are doing font substitution,
    // ignore the SO_FLAG_DEFAULT_PLACEMENT flag.  ie, let the engine tell
    // us where to place each character.

    if (pdata->bFontSubstitution)
        flAccel &= ~SO_FLAG_DEFAULT_PLACEMENT;

    if (cGlyphs != 0)
    {
        // position the first character of the string.

        ps_moveto(pdev, &pgp->ptl);

        // if we have non-standard spacing for the device font,
        // output an array of character widths, and push the
        // current point on the stack for use in the kshow command.

        if (!(flAccel & SO_FLAG_DEFAULT_PLACEMENT))
        {
            cTmp = cGlyphs - 1;

            // we need to handle the different STROBJ accelerators
            // here.  these accelerators only affect us if the font
            // is not using the default placement.

            if (flAccel & SO_HORIZONTAL)
            {
                if (pdata->bJustification && (cGlyphs > 1))
                {
                    // if we did the justification calculations and found
                    // that the justification widths were zero, do nothing
                    // here.

                    if ((pdata->ptSpace.x != 0) || (pdata->ptNonSpace.x != 0))
                    {
                        PrintPSFIX(pdev, 2, pdata->ptSpace.x, pdata->ptSpace.y);
                        PrintString(pdev, " 8#040 ");
                        PrintPSFIX(pdev, 2, pdata->ptNonSpace.x, pdata->ptNonSpace.y);
                        PrintString(pdev, "\n");
                    }
                }
                else
                {
                    // set up to output array of widths in reverse order,
                    // since we will get them off the stack.

                    pgp += cGlyphs - 1;

#if 0
//!!! This is just plain wrong!!! -kentse.
                    // deal with fixed pitch fonts.

                    if (pstro->ulCharInc)
                    {
                        // we have a fixed pitch font, and the fixed
                        // character increment can be found in pstro->ulCharInc.

                        lCharInc = (LONG)pstro->ulCharInc;

                        if (flAccel & SO_REVERSED)
                            lCharInc *= -1;

                        // output the increment value.

                        PrintDecimal(pdev, 1, lCharInc);
                        PrintString(pdev, " ");
                    }
                    else // must be a proportional font.
                    {
                        // output the character width for each glyph.

                        i = 0;
                        while (cTmp--)
                        {
                            psfxWidth = X72DPI(pgp->ptl.x) - X72DPI((pgp - 1)->ptl.x);
                            pgp--;
                            PrintPSFIX(pdev, 1, psfxWidth);
                            PrintString(pdev, " ");

                            // make it readable.

                            if (i++ == 10)
                            {
                                PrintString(pdev, "\n");
                                i = 0;
                            }
                        }
                    }
#endif
                    // output the character width for each glyph.

                    i = 0;
                    while (cTmp--)
                    {
                        psfxWidth = X72DPI(pgp->ptl.x) - X72DPI((pgp - 1)->ptl.x);
                        pgp--;
                        PrintPSFIX(pdev, 1, psfxWidth);
                        PrintString(pdev, " ");

                        // make it readable.

                        if (i++ == 10)
                        {
                            PrintString(pdev, "\n");
                            i = 0;
                        }
                    }
                }
            }

            else if (flAccel & SO_VERTICAL)
            {
                // set up to output array of widths in reverse order,
                // since we will get them off the stack.

                pgp += cGlyphs - 1;

#if 0
//!!! this is just plain wrong!!!  -kentse.
                // deal with fixed pitch fonts.

                if (pstro->ulCharInc)
                {
                    // we have a fixed pitch font, and the fixed
                    // character increment can be found in pstro->ulCharInc.

                    lCharInc = (LONG)pstro->ulCharInc;

                    if (flAccel & SO_REVERSED)
                        lCharInc *= -1;

                    // output the increment value.

                    PrintDecimal(pdev, 1, lCharInc);
                    PrintString(pdev, " ");
                }
                else // must be a proportional font.
                {
                    // output the character width for each glyph.

                    while (cTmp--)
                    {
                        psfxHeight = X72DPI(pgp->ptl.y) - X72DPI((pgp - 1)->ptl.y);
                        pgp--;
                        PrintPSFIX(pdev, 1, psfxHeight);
                        PrintString(pdev, " ");
                    }
                }
#endif
                // output the character width for each glyph.

                while (cTmp--)
                {
                    psfxHeight = X72DPI(pgp->ptl.y) - X72DPI((pgp - 1)->ptl.y);
                    pgp--;
                    PrintPSFIX(pdev, 1, psfxHeight);
                    PrintString(pdev, " ");
                }
            }
            else // the general case.
            {
                // in the general case, we are printing a string which
                // does not use the default character spacing, and is
                // not horizontal or vertical.  in this case, we must
                // do a moveto and a show command for every character.

                if (pdata->bDeviceFont)
                {
                    // it should be noted that pwstrString is only used
                    // if substitution is on, so we do not have to worry
                    // about the fact that pstro->pwszOrg is not filled
                    // in for device fonts.  we do not use the pstro->pwszOrg
                    // directly because the STROBJ_bEnum may be "chunking"
                    // the data.  therefore we will use the current pos.
                    // string pointer passed in.  it is the caller's
                    // responsibility to keep this current.

                    pwstrString = pwsz;

                    while (cGlyphs--)
                    {
                        ps_moveto(pdev, &pgp->ptl);

                        // output a left paren to open the string.

                        PrintString(pdev, "(");

                        // output each character of the string to the printer.
                        // if we are doing font substitution then we are substituting
                        // a device font for a truetype font.  it is assumed that
                        // the hglyphs for the truetype font are the unicode character
                        // codes.

                        if (pdata->bFontSubstitution)
                        {
                            if (!(RemapUnicodeChar(pdev, pwstrString++, pstro,
                                                   flAccel, FALSE, pdata)))
                            {
                                RIP("PSCRIPT!DrawGlyphs: RemapUnicodeChar failed.\n");
                                return(FALSE);
                            }
                        }
                        else
                        {
                            if (!(RemapDeviceChar(pdev, (CHAR *)&pgp->hg, pstro,
                                                  flAccel, FALSE, pdata)))
                            {
                                RIP("PSCRIPT!DrawGlyphs: RemapDeviceChar failed.\n");
                                return(FALSE);
                            }
                        }

                        // close the string and send out the show command,
                        // abreviated by 't'.

                        PrintString(pdev, ")t\n");

                        // point to the next character.

                        pgp++;
                    }

                }
                else // must be a GDI font.
                {
                    // get a pointer to our downloaded font structure for this
                    // font.

                    pDLFont = pdev->cgs.pDLFonts;

                    for (i = 0; i < pdev->iDLFonts; i++)
                    {
                        // is this entry the one we are looking for?

                        if (pDLFont->iUniq == pfo->iUniq)
                            break;

                        pDLFont++;
                    }

                    while (cGlyphs--)
                    {
                        ps_moveto(pdev, &pgp->ptl);

                        // output a left paren to open the string.

                        PrintString(pdev, "(");

                        // set a flag stating that we are now within a string.

                        bString = TRUE;

                        // output each character of the string to the printer.

                        if (!(RemapGDIChar(pdev, pstro, pgp, pDLFont, &bString,
                                           flAccel, pdata)))
                        {
                            RIP("PSCRIPT!DrawGlyphs: RemapGDIChar failed.\n");
                            return(FALSE);
                        }

                        // close the string and send out the show command,
                        // abreviated by 't'.

                        PrintString(pdev, ")t\n");

                        // point to the next character.

                        pgp++;
                    }
                }
            }

            if ((pstro->cGlyphs != 1) && (!pdata->bJustification) &&
                ((flAccel & SO_HORIZONTAL) || (flAccel & SO_VERTICAL)))
            {
                // pgp should now point back to where it did before
                // we entered this if statement.

                PrintString(pdev, "a\n");  // 'a' is abrev for currentpoint.
            }
        }   // end of !SO_FLAG_DEFAULT_PLACEMENT.

        if ((flAccel & SO_FLAG_DEFAULT_PLACEMENT) ||
            (flAccel & SO_HORIZONTAL) ||
            (flAccel & SO_VERTICAL))
        {
            // output a left paren to open the string.

            PrintString(pdev, "(");

            // set a flag stating that we are NOT within a string, until
            // the first character is actually output.  this is necessary
            // to prevent problems in the HORIZONTAL and VERTICAL cases,
            // when remapping a font.

            bString = FALSE;

            if (pdata->bDeviceFont)
            {
                // output each character of the string to the printer.
                // if we are doing font substitution then we are substituting
                // a device font for a truetype font.  it is assumed that
                // the hglyphs for the truetype font are the unicode character
                // codes.

                if (pdata->bFontSubstitution)
                {

                    pwstrString = pwsz;

                    // output the first character of the string, then
                    // set the flag stating that we have actually
                    // sent out a character.  this prevents us from
                    // setting the flag within the loop.

                    if (!(RemapUnicodeChar(pdev, pwstrString++, pstro,
                                           flAccel, bString, pdata)))
                    {
                        RIP("PSCRIPT!DrawGlyphs: RemapUnicodeChar failed.\n");
                        return(FALSE);
                    }

                    // set the flag stating that we have actually printed
                    // a character.

                    bString = TRUE;
                    cGlyphs--;

                    while (cGlyphs--)
                    {
                        if (!(RemapUnicodeChar(pdev, pwstrString++, pstro,
                                               flAccel, bString, pdata)))
                        {
                            RIP("PSCRIPT!DrawGlyphs: RemapUnicodeChar failed.\n");
                            return(FALSE);
                        }

                        // point to the next character.

                        pgp++;
                    }
                }
                else
                {
                    // output the first character of the string, then
                    // set the flag stating that we have actually
                    // sent out a character.  this prevents us from
                    // setting the flag within the loop.

                    if (!(RemapDeviceChar(pdev, (CHAR *)&pgp->hg, pstro, flAccel,
                                          bString, pdata)))
                    {
                        RIP("PSCRIPT!DrawGlyphs: RemapDeviceChar failed.\n");
                        return(FALSE);
                    }

                    // set the flag stating that we have actually printed
                    // a character.

                    bString = TRUE;
                    cGlyphs--;

                    // point to the next character.

                    pgp++;

                    while (cGlyphs--)
                    {
                        if (!(RemapDeviceChar(pdev, (CHAR *)&pgp->hg, pstro, flAccel,
                                              bString, pdata)))
                        {
                            RIP("PSCRIPT!DrawGlyphs: RemapDeviceChar failed.\n");
                            return(FALSE);
                        }

                        // point to the next character.

                        pgp++;
                    }
                }
            }
            else // must be a GDI font.
            {
                // get a pointer to our downloaded font structure for this
                // font.

                pDLFont = pdev->cgs.pDLFonts;

                for (i = 0; i < pdev->iDLFonts; i++)
                {
                    // is this entry the one we are looking for?


                    if (pDLFont->iUniq == pfo->iUniq)
                        break;

                    pDLFont++;
                }

                // in the GDI font case, bString simply means we have
                // begun a string, not necessaryly output a character.

                bString = TRUE;

                while (cGlyphs--)
                {
                    if (!(RemapGDIChar(pdev, pstro, pgp, pDLFont, &bString,
                                       flAccel, pdata)))
                    {
                        RIP("PSCRIPT!DrawGlyphs: RemapGDIChar failed.\n");
                        return(FALSE);
                    }

                    // point to the next character.

                    pgp++;
                }
            }

            if (bString)
                ps_show(pdev, pstro, flAccel, pdata);
        }
    }

    return(TRUE);
}


//--------------------------------------------------------------------
// BOOL RemapDeviceChar(pdev, pChar, pstro, flAccel, bString, pdata)
// PDEVDATA    pdev;
// PCHAR	    pChar;
// STROBJ     *pstro;
// FLONG       flAccel;
// BOOL     bString;
// TEXTDATA   *pdata;
//
// This routine is passed a pointer to a PostScript character code.
// This routine will output the proper string to the printer, representing
// the specified character code. '(', ')' and '\' are preceded by a
// backslash.  Characters which do not require a font remapping are
// output directly.  All other characters are output with there octal
// representation of their character code.
//
// Return:
//   This routine returns TRUE for success, FALSE for failure.
//
// History:
//   26-Apr-1991     -by-     Kent Settle     (kentse)
//  Wrote it.
//   14-Nov-1991    -by-    Kent Settle     [kentse]
//  re-wrote it (got rid of text buffer).
//--------------------------------------------------------------------

BOOL RemapDeviceChar(pdev, pChar, pstro, flAccel, bString, pdata)
PDEVDATA    pdev;
PCHAR	    pChar;
STROBJ     *pstro;
FLONG       flAccel;
BOOL        bString;
TEXTDATA   *pdata;
{
    PUCMap      pmap;
    BYTE        jChar;
    BOOL        bFound;
    CHAR	Buffer[4];
    CHAR       *pBuffer;

    // format each character for output to the printer.

    pBuffer = Buffer;

    switch(*pChar)
    {
	case '(':
        case ')':
        case '\\':
	    // precede each of the following characters with a backslash,
	    // then output to printer.

	    *pBuffer++ = '\\';
	    *pBuffer = *pChar;
	    if (!bPSWrite(pdev, Buffer, 2))
	    {
		RIP("PSCRIPT!RemapDeviceChar: bPSWrite failed.\n");
		return(FALSE);
	    }
            break;

        default:
            // at this point we should check to see if the high
            // bit of the usPSValue in mapping.h is set.  if it
            // is, remap the font, then output character.  otherwise,
            // just output character.

            // get local pointer to mapping table for current font.

            pmap = pdev->cgs.pmap;

            // assume character not found in font.

            bFound = FALSE;

            while (pmap->szChar)
            {
                // search for the matching code in mapping.h.  remember
                // that the high bit is used to indicate the font needs
                // to be remapped.  so ignore the high bit while checking
                // for a character match.

                if (*pChar == (CHAR)pmap->usPSValue)
                {
                    bFound = TRUE;

                    // now check the high bit of the character code in
                    // mapping.h.  if it is set, we need to remap the
                    // font for this character.

                    jChar = *pChar;

                    if ((pmap->usPSValue & 0x8000) || (jChar > 0x7F))
                    {
                        if (pmap->usPSValue & 0x8000)
                        {
                            // remap the font here.

                            RemapFont(pdev, pstro, flAccel, bString, pdata);
                        }

                        // we have an extended character.  convert the
			// non-printable ASCII to backslash octal, and
			// output to printer.

                        *pBuffer++ = '\\';
                        *pBuffer++ = (BYTE)((jChar >> 6) + '0');
                        jChar &= 63;
                        *pBuffer++ = (BYTE)((jChar >> 3) + '0');
			*pBuffer = (BYTE)((jChar & 7) + '0');

			if (!bPSWrite(pdev, Buffer, 4))
			{
			    RIP("PSCRIPT!RemapDeviceChar: bPSWrite failed.\n");
			    return(FALSE);
			}
                    }
                    else
                    {
			// simply write out the character.

			*pBuffer = *pChar;

			if (!bPSWrite(pdev, Buffer, 1))
			{
			    RIP("PSCRIPT!RemapDeviceChar: bPSWrite failed.\n");
			    return(FALSE);
			}
                    }

                    break;
                }

                // point to the next character in mapping.h.

                pmap++;
            } // while.

            // if the character was not found in the font, output
	    // a period.

            if (!bFound)
            {
		*pBuffer = '.';

		if (!bPSWrite(pdev, Buffer, 1))
		{
		    RIP("PSCRIPT!RemapDeviceChar: bPSWrite failed.\n");
		    return(FALSE);
		}
            }

            break;
    } // switch.

    return(TRUE);
}


//--------------------------------------------------------------------
// BOOL RemapUnicodeChar(pdev, pWChar, pstro, flAccel, bString, pdata)
// PDEVDATA pdev;
// PWCHAR   pWChar;
// STROBJ  *pstro;
// FLONG    flAccel;
// BOOL     bString;
// TEXTDATA *pdata;
//
// This routine is passed a pointer to a UNICODE character code.
// This routine will output the proper string to the printer, representing
// the specified character code. '(', ')' and '\' are preceded by a
// backslash.  Characters which do not require a font remapping are
// output directly.  All other characters are output with there octal
// representation of their character code.
//
// Return:
//   This routine returns TRUE for success, FALSE for failure.
//
// History:
//   27-Sep-1992     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL RemapUnicodeChar(pdev, pWChar, pstro, flAccel, bString, pdata)
PDEVDATA    pdev;
PWCHAR	    pWChar;
STROBJ     *pstro;
FLONG       flAccel;
BOOL        bString;
TEXTDATA   *pdata;
{
    PUCMap      pmap;
    BYTE        jChar;
    BOOL        bFound;
    CHAR	Buffer[4];
    CHAR       *pBuffer;
    CHAR       *pChar;

    // begin by finding the UNICODE character code in our mapping table.

    pmap = pdev->cgs.pmap;

    // assume character is NOT found in the font.

    bFound = FALSE;

    while (pmap->szChar)
    {
        if (*pWChar == pmap->usUCValue)
        {
            bFound = TRUE;
            break;
        }

        // this was not the character we wanted, check the next one.

        pmap++;
    }

    // if the character was not found, output a period.

    pBuffer = Buffer;

    if (!bFound)
    {
        *pBuffer = '.';

        if (!bPSWrite(pdev, Buffer, 1))
        {
            RIP("PSCRIPT!RemapUnicodeChar: bPSWrite failed.\n");
            return(FALSE);
        }

        return(TRUE);
    }

    // now that we have found the UNICODE character code, get the
    // corresponding PostScript character code.

    pChar = (CHAR *)&pmap->usPSValue;

    // output the character to the printer in its proper format.

    switch(*pChar)
    {
	case '(':
        case ')':
        case '\\':
	    // precede each of the following characters with a backslash,
	    // then output to printer.

	    *pBuffer++ = '\\';
	    *pBuffer = *pChar;
	    if (!bPSWrite(pdev, Buffer, 2))
	    {
		RIP("PSCRIPT!RemapUnicodeChar: bPSWrite failed.\n");
		return(FALSE);
	    }
            break;

        default:
            // at this point we should check to see if the high
            // bit of the usPSValue in mapping.h is set.  if it
            // is, remap the font, then output character.  otherwise,
            // just output character.

            jChar = *pChar;

            if ((pmap->usPSValue & 0x8000) || (jChar > 0x7F))
            {
                if (pmap->usPSValue & 0x8000)
                {
		    // remap the font here.

                    RemapFont(pdev, pstro, flAccel, bString, pdata);
                }

                // we have an extended character.  convert the
		// non-printable ASCII to backslash octal, and
		// output to printer.

                *pBuffer++ = '\\';
                *pBuffer++ = (BYTE)((jChar >> 6) + '0');
                jChar &= 63;
                *pBuffer++ = (BYTE)((jChar >> 3) + '0');
		*pBuffer = (BYTE)((jChar & 7) + '0');

		if (!bPSWrite(pdev, Buffer, 4))
		{
                    RIP("PSCRIPT!RemapUnicodeChar: bPSWrite failed.\n");
		    return(FALSE);
		}
            }
            else
            {
		// simply write out the character.

		*pBuffer = *pChar;

		if (!bPSWrite(pdev, Buffer, 1))
		{
                    RIP("PSCRIPT!RemapUnicodeChar: bPSWrite failed.\n");
		    return(FALSE);
		}
            }
    } // switch

    return(TRUE);
}


//--------------------------------------------------------------------
// BOOL RemapGDIChar(pdev, pstro, pgp, pDLFont, pbString, flAccel, pdata)
// PDEVDATA    pdev;
// STROBJ     *pstro;
// GLYPHPOS   *pgp;
// DLFONT     *pDLFont;
// BOOL       *pbString;
// FLONG       flAccel;
// TEXTDATA   *pdata;
//
// This routine is passed a pointer to a GLYPHPOS structure.
// This routine will output the proper string to the printer, representing
// the specified character code. '(', ')' and '\' are preceded by a
// backslash.  Characters which are located within the downloaded font are
// output directly.  Any other character will cause the current string
// to be closed, and a show command to be issued.  Then the current character
// will be drawn, either by bitblt or a path.
//
// Return:
//   This routine returns TRUE for success, FALSE for failure.
//
// History:
//   27-Mar-1992    -by-        Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL RemapGDIChar(pdev, pstro, pgp, pDLFont, pbString, flAccel, pdata)
PDEVDATA    pdev;
STROBJ     *pstro;
GLYPHPOS   *pgp;
DLFONT     *pDLFont;
BOOL       *pbString;
FLONG       flAccel;
TEXTDATA   *pdata;
{
    BOOL        bFound;
    CHAR	Buffer[4];
    CHAR       *pBuffer;
    DWORD       i;
    HGLYPH      hglyph;
    HGLYPH     *phg;
    BYTE        jCurrent;

    // point to internal buffer to build character code into.

    pBuffer = Buffer;

    // get the handle for the current glyph.  then find the corresponding
    // character code, as defined in the downloaded font.

    bFound = FALSE;
    hglyph = pgp->hg;

#if DBG
    if (hglyph == HGLYPH_INVALID)
    {
        RIP("PSCRIPT!RemapGDIChar: hglyph is zero, we're hosed.\n");
        return(FALSE);
    }
#endif

    phg = pDLFont->phgVector;

    for (i = 0; i < pDLFont->cGlyphs; i++)
    {
        if (*phg == hglyph)
        {
            bFound = TRUE;
            break;
        }

        phg++;
    }

    // i contains the character code for the printer, assuming it was
    // found in the downloaded font.

    jCurrent = (BYTE)i;

    // if the character was found in the downloaded font, we will be
    // outputting it as part of a string.  otherwise, we will be drawing
    // it via imagemask or as a path.

    if (bFound)
    {
        // the character is part of the downloaded font.  so output the
        // character code as part of a string.  we must, however, check
        // to see if a string already exists to add it to, or if we must
        // start a new one.

        if (*pbString == FALSE)
        {
            // we are not in the middle of a string, so we must begin
            // a new one.

            ps_moveto(pdev, &pgp->ptl);

            // output a left paren to open the string.

            PrintString(pdev, "(");
        }

        // we are now guaranteed to be in the middle of a string, so
        // simply output the character code.

        switch(jCurrent)
        {
	    case '(':
            case ')':
            case '\\':
	        // precede each of the following characters with a backslash,
	        // then output to printer.

                *pBuffer++ = '\\';
                *pBuffer = jCurrent;
                if (!bPSWrite(pdev, Buffer, 2))
                {
                    RIP("PSCRIPT!RemapGDIChar: bPSWrite failed.\n");
                    return(FALSE);
                }
                break;

            default:
                // if the character code is within the printable ASCII
                // range, simply write out the character.  otherwise,
                // we need to output the three digit octal character code.

                if ((jCurrent >= 0x20) && (jCurrent <= 0x7F))
                {
                    if (!bPSWrite(pdev, &jCurrent, 1))
                    {
                        RIP("PSCRIPT!RemapGDIChar: bPSWrite failed.\n");
                        return(FALSE);
                    }
                }
                else
                {
                    // convert the non-printable ASCII to backslash octal,
                    // and output to the printer.

                    *pBuffer++ = '\\';
                    *pBuffer++ = (BYTE)((jCurrent >> 6) + '0');
                    jCurrent &= 63;
                    *pBuffer++ = (BYTE)((jCurrent >> 3) + '0');
                    *pBuffer = (BYTE)((jCurrent & 7) + '0');

                    if (!bPSWrite(pdev, Buffer, 4))
                    {
                        RIP("PSCRIPT!RemapGDIChar: bPSWrite failed.\n");
                        return(FALSE);
                    }
                }
                break;
        } // end of switch.

        // set the flag stating that we are now within a string.

        *pbString = TRUE;
    }
    else    // character not found in downloaded font.
    {
        // the character is NOT part of the downloaded font.  so we must
        // actually draw the character.  we must, however, check to see
        // if an open string already exists in the printer, and close it
        // if it does.

        if (*pbString)
        {
            // we are in the middle of a string, so we must end it before
            // we can draw the current character.

            ps_show(pdev, pstro, flAccel, pdata);
        }

        // we are now ready to draw the character.

//!!! for now only bitmap fonts are supported.  we will support vector
//!!! fonts whenever the engine does.  -kentse.

        CharBitmap(pdev, pgp);

        // set the flag stating that we are now NOT within a string.

        *pbString = FALSE;
    }

    return(TRUE);
}

//--------------------------------------------------------------------
// BOOL SelectFont(pdev, pfo, pdata)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// TEXTDATA   *pdata;
//
// This routine selects the font specified in the FONTOBJ, and selects
// it as the current font in the printer.  If the specified font is
// already the current font in the printer, then this routine does
// nothing.
//
// History:
//   15-Jul-1992    -by-    Kent Settle     (kentse)
//  Added Font Substitution support.
//   27-Feb-1992    -by-    Kent Settle     (kentse)
//  Added support for downloading, ie caching, GDI fonts.
//   20-Feb-1992    -by-    Kent Settle     (kentse)
//  Added support for softfonts.
//   26-Apr-1991    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL SelectFont(pdev, pfo, pdata)
PDEVDATA    pdev;
FONTOBJ    *pfo;
TEXTDATA   *pdata;
{
    PNTFM       pntfm;
    BYTE       *pSoftFont;
    DWORD       cDownloadedFonts;
    DLFONT     *pDLFont;
    PS_FIX      psfxM11, psfxM12, psfxM21, psfxM22, psfxtmp;
    XFORM      *pxform;

    // do not select the font in the printer if it is currently selected,
    // including the same point size.

    if (pfo->iUniq == pdev->cgs.lidFont)
	return(TRUE);

    // get the point size, and fill in the font xform.

    pdev->cgs.psfxScaleFactor = GetPointSize(pdev, pfo, &pdev->cgs.FontXform);

#if DBG
    if (pdev->cgs.psfxScaleFactor == 0)
        RIP("PSCRIPT!SelectFont: Zero point size!\n");
#endif

    // do not select the font into the printer if it is a GDI font that
    // we will not be caching.  according to the DDI spec, if pfo->iUniq
    // is zero, the GDI font should not be cached.

    if (!pdata->bDeviceFont && (pfo->iUniq == 0))
    {
#if DBG
        DbgPrint("A non-cached GDI font made it to SelectFont, should it have?\n");
#endif
        return(TRUE);
    }

    // select the proper font name for the new font.  if this is a
    // device font, get the name from the NTFM structure.  if this
    // is a GDI font that we are caching, we will create a name for
    // it at the time we download it to the printer.

    if (pdata->bDeviceFont)
    {
        // get the font metrics for the specified font.

        pntfm = pdev->pfmtable[pdata->iFace - 1].pntfm;

        // if the font is a softfont, and it has not yet been downloaded,
        // download it.

        if ((pdata->iFace > pdev->cDeviceFonts) &&
             !((BYTE)pdev->cgs.pSFArray[pdata->iFace >> 3] &
                                        (BYTE)(1 << (pdata->iFace & 0x07))))
        {
            pSoftFont = (BYTE *)pntfm + pntfm->loSoftFont;

            // if we have reached our downloaded font threshold, then
            // we will surround ever textout call with a save/restore.

            cDownloadedFonts = min(pdev->cgs.cDownloadedFonts, pdev->iDLFonts);

            if (cDownloadedFonts == pdev->iDLFonts)
            {
                ps_save(pdev, FALSE);
                pdev->cgs.dwFlags |= CGS_DLFONTTHRESHOLD;
            }

            if (!bPSWrite(pdev, pSoftFont, pntfm->cjSoftFont))
            {
                RIP("PSCRIPT!SelectFont: downloading of softfont failed.\n");
                return(FALSE);
            }

            // set the bit saying this font has been downloaded.

            (BYTE)pdev->cgs.pSFArray[pdata->iFace >> 3] |=
                                     (BYTE)(1 << (pdata->iFace & 0x07));

            // if we have hit our limit of Fonts we can download, simply overwrite
            // the last one with the new one.  this is to try to conserve on
            // memory consumption.

            pDLFont = pdev->cgs.pDLFonts;
            pDLFont += cDownloadedFonts;

            pDLFont->iFace = pdata->iFace;
            pDLFont->iUniq = pfo->iUniq;
            pDLFont->cGlyphs = 0;
            pDLFont->phgVector = NULL;

            pdev->cgs.cDownloadedFonts++;
        }

        // select the font in the printer.

        strcpy(pdev->cgs.szFont, (char *)pntfm + pntfm->loszFontName);
    }
    else // must be a GDI font we will be caching.
    {
//!!! the engine is not ready for outline fonts yet!!!  -kentse.
#if 0
        // if this font has not yet been downloaded to the printer,
        // do it now.

        if (pfo->flFontType & TRUETYPE_FONTTYPE)
        {
            // determine the point size.

            ulPointSize = (ETOL(pdev->cgs.FontXform.eM22 * 72000) /
                          pdev->psdm.dm.dmPrintQuality);

            if (ulPointSize < 10)
                DownloadBitmapFont(pdev, pfo);
            else
                DownloadOutlineFont(pdev, pfo);
        }
        else if (pfo->flFontType & RASTER_FONTTYPE)
            DownloadBitmapFont(pdev, pfo);
#endif
        if ( (pfo->flFontType & TRUETYPE_FONTTYPE) ||
             (pfo->flFontType & RASTER_FONTTYPE) )
            DownloadBitmapFont(pdev, pfo);
        else
        {
            RIP("PSCRIPT!SelectFont: invalid pfo->flFontType.\n");
            return(FALSE);
        }
    }

    // select the proper font, depending on whether or not the
    // font in question has been reencoded.  also, output a scalefont
    // command if only scaling is ocurring, otherwise output a
    // makefont command.

#if 0
    PrintPSFIX(pdev, 1, pdev->cgs.psfxScaleFactor);

    if (QueryFontRemap(pdev, pfo->iUniq))
        PrintString(pdev, " /_");
    else
        PrintString(pdev, " /");

    PrintString(pdev, pdev->cgs.szFont);
    PrintString(pdev, " SF\n");
#endif

    pxform = &pdev->cgs.FontXform;

    if
    (
        ((pxform->eM11 == pxform->eM22) &&
        (pxform->eM12 == 0)            &&
        (pxform->eM21 == 0)            &&
        !((*((ULONG *)&pxform->eM11)) & 0x80000000)) // if positive
        ||
        ((pfo->flFontType & TRUETYPE_FONTTYPE) && !pdata->bFontSubstitution) // all scaling done already by tt driver
    )
    {
        PrintPSFIX(pdev, 1, pdev->cgs.psfxScaleFactor);

        if (QueryFontRemap(pdev, pfo->iUniq))
            PrintString(pdev, " /_");
        else
            PrintString(pdev, " /");

        PrintString(pdev, pdev->cgs.szFont);
        PrintString(pdev, " SF\n");
    }
    else
    {
        // normalize the font transform by the emheight;

        psfxtmp = pdev->cgs.fwdEmHeight * PS_FIX_RESOLUTION;

        psfxM11 = (LONG)((pdev->cgs.FontXform.eM11 * (FLOAT)psfxtmp) /
                          pdev->psdm.dm.dmPrintQuality);
        psfxM12 = (LONG)((pdev->cgs.FontXform.eM12 * (FLOAT)psfxtmp) /
                          pdev->psdm.dm.dmPrintQuality);
        psfxM21 = (LONG)((pdev->cgs.FontXform.eM21 * (FLOAT)psfxtmp) /
                          pdev->psdm.dm.dmPrintQuality);
        psfxM22 = (LONG)((pdev->cgs.FontXform.eM22 * (FLOAT)psfxtmp) /
                          pdev->psdm.dm.dmPrintQuality);

        PrintString(pdev, "[");
        PrintPSFIX(pdev, 6, psfxM11, -psfxM12, -psfxM21, psfxM22,0,0);

        if (QueryFontRemap(pdev, pfo->iUniq))
            PrintString(pdev, "] /_");
        else
            PrintString(pdev, "] /");

        PrintString(pdev, pdev->cgs.szFont);
        PrintString(pdev, " MF\n");
    }

    // update the font in our current graphics state.

    pdev->cgs.lidFont = pfo->iUniq;

    // if we have a device font, point to the appropriate mapping table
    // in mapping.h.

    if (pdata->bDeviceFont)
    {
        if (!strcmp((char *)pntfm + pntfm->loszFontName, "Symbol"))
            pdev->cgs.pmap = SymbolMap;
        else if (!strcmp((char *)pntfm + pntfm->loszFontName, "ZapfDingbats"))
            pdev->cgs.pmap = DingbatsMap;
        else
            pdev->cgs.pmap = LatinMap;
    }

    return(TRUE);
}


//--------------------------------------------------------------------
// VOID RemapFont(pdev, pstro, flAccel, bString, pdata)
// PDEVDATA    pdev;
// STROBJ     *pstro;
// FLONG       flAccel;
// BOOL        bString;
// TEXTDATA   *pdata;
//
// This routine is only called if we have a character which does not
// have the standard PostScript character code.  This is determined
// by checking the high bit of the usPSValue in the proper table in
// mapping.h.  If necessary, this routine will download a new
// encoding vector.  It will then reencode the current font to the
// new encoding vector.
//
// History:
//   12-Sep-1991     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

VOID RemapFont(pdev, pstro, flAccel, bString, pdata)
PDEVDATA    pdev;
STROBJ     *pstro;
FLONG       flAccel;
BOOL        bString;
TEXTDATA   *pdata;
{
    int         i;
    PSZ        *pszEncode;
    PSZ         pszVector;
    int         cbLength;
    PUCMap      pmap;
    BOOL        bFound;
    FLONG       flEncoding;
    BOOL	bRemapDone = FALSE;
    XFORM      *pxform;
    PS_FIX      psfxM11, psfxM12, psfxM21, psfxM22, psfxtmp;

//!!! All this font remapping stuff gets blown away by a gsave/grestore
//!!! which is done from DrvTextout when clipping.  Perhaps move flags from
//!!! CGS to PDEV.

    // if the font remapping header has not been downloaded to the
    // printer, do it now.  this header is sent at most once per job.

    if (!(pdev->dwFlags & PDEV_FONTREDEFINED))
    {
	// we are in the middle of outputting a string.  close
	// the current string and output a show command to the
	// printer, output the remapping, then begin a new string.

        ps_show(pdev, pstro, flAccel, pdata);
	bRemapDone = TRUE;

        pszEncode = apszRemapCode;
	while (*pszEncode)
	{
	    PrintString(pdev, (PSZ)*pszEncode++);
	    PrintString(pdev, "\n");
	}

        pdev->dwFlags |= PDEV_FONTREDEFINED;
    }

    // select the proper encoding vector to download.

    if (pdev->cgs.pmap == SymbolMap)
    {
        pszVector = "SYMENC";
        flEncoding = PDEV_SYMENCODED;
    }
    else if (pdev->cgs.pmap == DingbatsMap)
    {
        pszVector = "DINGENC";
        flEncoding = PDEV_DINGENCODED;
    }
    else
    {
        pszVector = "LATENC";
        flEncoding = PDEV_LATINENCODED;
    }

    // define the new font encoding if it has not already been done.

    if ((pdev->dwFlags & flEncoding) == 0)
    {
	if (!bRemapDone)
	{
	    // we are in the middle of outputting a string.  close
	    // the current string and output a show command to the
	    // printer, output the remapping, then begin a new string.

            ps_show(pdev, pstro, flAccel, pdata);
	    bRemapDone = TRUE;
	}

	// download the new encoding vector.  just to make things readable,
        // let's limit the line length.

        cbLength = 0;

	PrintString(pdev, "/");
	PrintString(pdev, pszVector);
	PrintString(pdev, " [0\n");

        // PostScript fonts containg 256 characters.  for each character,
        // do a lookup in the appropriate mapping table in mapping.h to
        // get the ASCII name of the character to output in the encoding
        // vector.  if the character is not found in the encoding vector,
        // output ".notdef" for that character.

        for (i = 0; i < 256; i++)
        {
            if (cbLength > MAX_LINE_LENGTH)
            {
                // skip to the next line.

		PrintString(pdev, "\n");
                cbLength = 0;
            }

            // get local pointer to mapping table for current font.

            pmap = pdev->cgs.pmap;

            // assume character not found in font.

            bFound = FALSE;

            while (pmap->szChar)
            {
                // search for the matching code in mapping.h.  remember
                // that the high bit is used to indicate the font needs
                // to be remapped.  so ignore the high bit while checking
                // for a character match.

                if ((CHAR)i == (CHAR)pmap->usPSValue)
                {
                    bFound = TRUE;

		    PrintString(pdev, "/");
		    PrintString(pdev, pmap->szChar);

                    cbLength += (strlen(pmap->szChar) + 1);
                    break;
                }

                // point to the next character in mapping.h.

                pmap++;
            } // while.

            // if the character was not found in the font, output .notdef.

            if (!bFound)
            {
		PrintString(pdev, "/.notdef");
                cbLength += 8;
            }
        }

        // we are done downloading the encoding vector.

	PrintString(pdev, "\n]def\n");

        // mark that this encoding vector is now defined in the printer.

        pdev->dwFlags |= flEncoding;
    }

    // output the PostScript commands to reencode the current font
    // using the proper encoding vector, if the current font has
    // not already been reencoded.

    if (!QueryFontRemap(pdev, pdev->cgs.lidFont))
    {
	if (!bRemapDone)
	{
	    // we are in the middle of outputting a string.  close
	    // the current string and output a show command to the
	    // printer, output the remapping, then begin a new string.

            ps_show(pdev, pstro, flAccel, pdata);
	    bRemapDone = TRUE;
	}

	PrintString(pdev, pszVector);
	PrintString(pdev, " /_");
	PrintString(pdev, pdev->cgs.szFont);
	PrintString(pdev, " /");
	PrintString(pdev, pdev->cgs.szFont);
	PrintString(pdev, " reencode\n");

        // select the newly reencoded font.

        pxform = &pdev->cgs.FontXform;

        if
        (
            (pxform->eM11 == pxform->eM22) &&
            (pxform->eM12 == 0)            &&
            (pxform->eM21 == 0)            &&
            !((*((ULONG *)&pxform->eM11)) & 0x80000000) // if positive
        )
        {
            PrintPSFIX(pdev, 1, pdev->cgs.psfxScaleFactor);
            PrintString(pdev, " /_");
            PrintString(pdev, pdev->cgs.szFont);
            PrintString(pdev, " SF\n");
        }
        else
        {
            // normalize the font transform by the emheight;

            psfxtmp = pdev->cgs.fwdEmHeight * PS_FIX_RESOLUTION;

            psfxM11 = (LONG)((pdev->cgs.FontXform.eM11 * (FLOAT)psfxtmp) /
                        pdev->psdm.dm.dmPrintQuality);
            psfxM12 = (LONG)((pdev->cgs.FontXform.eM12 * (FLOAT)psfxtmp) /
                        pdev->psdm.dm.dmPrintQuality);
            psfxM21 = (LONG)((pdev->cgs.FontXform.eM21 * (FLOAT)psfxtmp) /
                        pdev->psdm.dm.dmPrintQuality);
            psfxM22 = (LONG)((pdev->cgs.FontXform.eM22 * (FLOAT)psfxtmp) /
                        pdev->psdm.dm.dmPrintQuality);

            PrintString(pdev, "[");
            PrintPSFIX(pdev, 6, psfxM11, -psfxM12, -psfxM21, psfxM22,0,0);

            PrintString(pdev, "] /_");
            PrintString(pdev, pdev->cgs.szFont);
            PrintString(pdev, " MF\n");
        }

        // set a flag saying that the current font has been reencoded.

        SetFontRemap(pdev, pdev->cgs.lidFont);
    }

    // if any remapping was done, start a new string.

    if (bRemapDone)
    {
        if ((flAccel == 0) || (flAccel & SO_FLAG_DEFAULT_PLACEMENT))
	    PrintString(pdev, "(");
        else if ((flAccel & SO_HORIZONTAL) || (flAccel & SO_VERTICAL))
        {
            // if we have a vertical or horizontal, non default spacing,
            // case, and we have just remapped a font, and we have actuall
            // printed at least one character within the string, then we need
            // to get rid of the with between the last character printed, then
            // get the current position on the stack before we begin our
            // string.

            if (pdata->bJustification)
            {
                // if we did the justification calculations and found
                // that the justification widths were zero, do nothing
                // here.

                if ((pdata->ptSpace.x != 0) || (pdata->ptNonSpace.x != 0))
                {
                    PrintPSFIX(pdev, 2, pdata->ptSpace.x, pdata->ptSpace.y);
                    PrintString(pdev, " 8#040 ");
                    PrintPSFIX(pdev, 2, pdata->ptNonSpace.x, pdata->ptNonSpace.y);
                    PrintString(pdev, "\n");
                }

                PrintString(pdev, "(");
            }
            else if (bString)
                PrintString(pdev, "pop a (");
            else
            {
                if(pstro->cGlyphs != 1)
                    PrintString(pdev, "a (");
                else
                    PrintString(pdev, "(");
            }
        }
    }
}


//--------------------------------------------------------------------
// VOID CharBitmap(pdev, pgp)
// PDEVDATA        pdev;
// GLYPHPOS       *pgp;
//
// This routine downloads the bitmap for the given character to
// the printer.
//
// History:
//   26-Sep-1991     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

VOID CharBitmap(pdev, pgp)
PDEVDATA        pdev;
GLYPHPOS       *pgp;
{
    POINTPSFX   ptpsfx;
    int         cjWidth;
    int         i;
    BYTE       *pjBits;
    LONG        cx, cy;

    // adjust the (x, y) coordinates to adjust for displacement of
    // character origin from bitmap origin.

    pgp->ptl.x += pgp->pgdf->pgb->ptlOrigin.x;
    pgp->ptl.y += pgp->pgdf->pgb->ptlOrigin.y;

    // position the image on the page, remembering to flip the image
    // from top to bottom.

    ptpsfx.x = X72DPI(pgp->ptl.x);
    ptpsfx.y = Y72DPI(pgp->ptl.y);

    // output PostScript user coordinates to the printer.

    PrintString(pdev, "save ");
    PrintPSFIX(pdev, 2, ptpsfx.x, ptpsfx.y);
    PrintString(pdev, " translate\n");

    // scale the image.
    //!!! I do not know how this ever worked if it did. [Bodind]

    // cx = pgp->pgdf->pgb->aj[0];
    // cy = pgp->pgdf->pgb->aj[1];

    cx = pgp->pgdf->pgb->sizlBitmap.cx;
    cy = pgp->pgdf->pgb->sizlBitmap.cy;

    ptpsfx.x = (cx * PS_FIX_RESOLUTION) / pdev->psdm.dm.dmPrintQuality;
    ptpsfx.y = (cy * PS_FIX_RESOLUTION) / pdev->psdm.dm.dmPrintQuality;

    PrintPSFIX(pdev, 2, ptpsfx.x, ptpsfx.y);
    PrintString(pdev, " scale\n");

    // output the image operator and the scan data.  true means to
    // paint the '1' bits with the foreground color.

    PrintDecimal(pdev, 2, cx, cy);
    PrintString(pdev, " true [");
    PrintDecimal(pdev, 1, cx);
    PrintString(pdev, " 0 0 ");
    PrintDecimal(pdev, 1, -cy);
    PrintString(pdev, " 0 0]\n{<");

    // how wide is the destination in bytes?  postscript bitmaps are padded
    // to 8bit boundaries.

    cjWidth = (cx + 7) >> 3;

//!!! this seems to be wrong, do not know why it ever worked if it did
//!!! [bodind]
    // pjBits = (BYTE *)&pgp->pgdf->pgb->aj[2];

    pjBits = pgp->pgdf->pgb->aj;

    for (i = 0; i < cy; i++)
    {
        // output each scanline to the printer.

        vHexOut(pdev, pjBits, cjWidth);
        pjBits += cjWidth;
    }

    PrintString(pdev, ">} im restore\n");
}


//--------------------------------------------------------------------
// BOOL DownloadBitmapFont(pdev, pfo)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
//
// This routine downloads the font definition for the given bitmap font,
// if it has not already been done.  The font is downloaded as an
// Adobe Type 3 font.
//
// This routine return TRUE if the font is successfully, or has already
// been, downloaded to the printer.  It returns FALSE if it fails.
//
// History:
//   27-Feb-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL DownloadBitmapFont(pdev, pfo)
PDEVDATA    pdev;
FONTOBJ    *pfo;
{
    DLFONT     *pDLFont;
    DWORD       i, j;
    DWORD       cDownloadedFonts;
    DWORD       cGlyphs, cTmp;
    HGLYPH     *phg;
    HGLYPH     *phgSave;
    GLYPHDATA  *pglyphdata;
    POINTL      ptlTL, ptlBR, ptl1;
    PIFIMETRICS pifi;
    PS_FIX      psfxXtrans, psfxYtrans;
    CHAR        szFaceName[MAX_STRING];
    PSZ         pszFaceName;
    PWSTR       pwstr;
    LONG        cjWidth;
    BYTE       *pjBits;
    LONG        EmHeight;
    XFORMOBJ   *pxo;
    POINTFIX    ptfx;
    PS_FIX      psfxPointSize;
    XFORM       fontxform;

    // search through our list of downloaded GDI fonts to see if the
    // current font has already been downloaded to the printer.

    pDLFont = pdev->cgs.pDLFonts;

    cDownloadedFonts = min(pdev->cgs.cDownloadedFonts, pdev->iDLFonts);

    for (i = 0; i < cDownloadedFonts; i++)
    {
        // is this entry the one we are looking for?  simply return if so.

        if (pDLFont->iUniq == pfo->iUniq)
        {
            // update the fontname and size in our current graphics state.

            strcpy(pdev->cgs.szFont, pDLFont->strFont);
            pdev->cgs.lidFont = pDLFont->iUniq;

            return(TRUE);
        }

        pDLFont++;
    }

    // we did not find that this font has been downloaded yet, so we must
    // do it now.

    // if we have reached our downloaded font threshold, then
    // we will surround ever textout call with a save/restore.

    if (cDownloadedFonts >= pdev->iDLFonts)
    {
        ps_save(pdev, FALSE);
        pdev->cgs.dwFlags |= CGS_DLFONTTHRESHOLD;
        cDownloadedFonts = pdev->iDLFonts;
    }

    pDLFont = pdev->cgs.pDLFonts;
    pDLFont += cDownloadedFonts;

    memset(pDLFont, 0, sizeof(DLFONT));

    pDLFont->iFace = pfo->iFace;
    pDLFont->iUniq = pfo->iUniq;

    // if we have made it this far, we should simply be able to
    // download the font now.

    // we will be downloading an Adobe TYPE 3 font.

    // allocate a dictionary for the font.

    PrintString(pdev, "9 dict d begin\n");

    // set FontType to 3 indicating user defined font.

    PrintString(pdev, "/FontType 3 def\n");

    // allocate memory for and get the handles for each glyph of the font.

    if (!(cGlyphs = FONTOBJ_cGetAllGlyphHandles(pfo, NULL)))
    {
        RIP("PSCRIPT!DownloadBitmapFont: cGetAllGlyphHandles failed.\n");
        return(FALSE);
    }

    if (!(phg = (HGLYPH *)HeapAlloc(pdev->hheap, 0, sizeof(HGLYPH) * cGlyphs)))
    {
        RIP("PSCRIPT!DownloadBitmapFont: HeapAlloc failed.\n");
        return(FALSE);
    }

    phgSave = phg;

    cTmp = FONTOBJ_cGetAllGlyphHandles(pfo, phg);

    ASSERTPS(cTmp == cGlyphs, "PSCRIPT!DownloadBitmapFont: inconsistent cGlyphs\n");

    // how many characters will we define in this font?
    // keep in mind that we can only do 256 at a time.
    // remember to leave room for the .notdef character.

    cGlyphs = min(255, cGlyphs);

    // run through the array, looking at the bounding box for each
    // glyph, in order to create the bounding box for the entire
    // font.

    ptlTL.x = ADOBE_FONT_UNITS;
    ptlTL.y = ADOBE_FONT_UNITS;
    ptlBR.x = 0;
    ptlBR.y = 0;

    for (i = 0; i < cGlyphs; i++)
    {
        // get the GLYPHDATA structure for each glyph.

        if (!(cTmp = FONTOBJ_cGetGlyphs(pfo, FO_GLYPHBITS, 1, phg, (PVOID *)&pglyphdata)))
        {
            RIP("PSCRIPT!DownloadBitmapFont: cGetGlyphs failed.\n");
            HeapFree(pdev->hheap, 0, (PVOID)phgSave);
            return(FALSE);
        }

        ptlTL.x = min(ptlTL.x, pglyphdata->rclInk.left);
        ptlTL.y = min(ptlTL.y, pglyphdata->rclInk.top);
        ptlBR.x = max(ptlBR.x, pglyphdata->rclInk.right);
        ptlBR.y = max(ptlBR.y, pglyphdata->rclInk.bottom);

        // point to the next glyph handle.

        phg++;
    }

    // get the IFIMETRICS for the font.

    if (!(pifi = FONTOBJ_pifi(pfo)))
    {
        RIP("PSCRIPT!DownloadBitmapFont: pifi failed.\n");
        HeapFree(pdev->hheap, 0, (PVOID)phgSave);
        return(FALSE);
    }

    // get the Notional to Device transform.  this is needed to
    // determine the point size.

    pxo = FONTOBJ_pxoGetXform(pfo);

    if (pxo == NULL)
    {
        RIP("PSCRIPT!DownloadBitmapFont: pxo == NULL.\n");
        return(FALSE);
    }

    // apply the notional to device transform.

    ptl1.x = 0;
    ptl1.y = pifi->fwdUnitsPerEm;

    XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl1, &ptfx);

    // now get the length of the vector.

    EmHeight = FXTOL(iHipot(ptfx.x, ptfx.y));

    // we have filled in the GLYPHPOS for each glyph in the font.
    // reset the pointer to the first glyph.

    phg = phgSave;

    PrintString(pdev, "/FontMatrix [1 ");
    PrintDecimal(pdev, 1, EmHeight);
    PrintString(pdev, " div 0 0 1 ");
    PrintDecimal(pdev, 1, EmHeight);
    PrintString(pdev, " div 0 0] def\n");

    // define the bounding box for the font, defined in 1 unit
    // character space (since FontMatrix = identity).

    PrintString(pdev, "/FontBBox [");
    PrintDecimal(pdev, 4, ptlTL.x, ptlTL.y, ptlBR.x, ptlBR.y);
    PrintString(pdev, " ] def\n");

    // allocate array for encoding vector, then initialize
    // all characters in encoding vector with '.notdef'.

    PrintString(pdev, "/Encoding 256 array def\n");
    PrintString(pdev, "0 1 255 {Encoding exch /.notdef put} for\n");

    // allocate space to store the HGLYPH<==>character code mapping.

    if (!(pDLFont->phgVector = (HGLYPH *)HeapAlloc(pdev->hheap, 0,
                      sizeof(HGLYPH) * cGlyphs)))
    {
        RIP("PSCRIPT!DownloadBitmapFont: HeapAlloc for phgVector failed.\n");
        HeapFree(pdev->hheap, 0, (PVOID)phgSave);
        return(FALSE);
    }

    // fill in the HGLYPH encoding vector, and output
    // the encoding vector to the printer.

    pDLFont->cGlyphs = cGlyphs;
    memcpy(pDLFont->phgVector, phg, cGlyphs * sizeof(HGLYPH));

    PrintString(pdev, "Encoding\n");

    for (i = 0; i < cGlyphs; i++)
    {
        PrintString(pdev, "d ");
        PrintDecimal(pdev, 1, i);
        PrintString(pdev, " /c");
        PrintDecimal(pdev, 1, i);
        PrintString(pdev, " p\n");
    }

    // under level 1 of PostScript, the 'BuildChar' procedure is called
    // every time a character from the font is constructed.  under
    // level 2, 'BuildGlyph' is called instead.  therefore, we will
    // define a 'BuildChar' procedure, which basically calls
    // 'BuildGlyph'.  this will provide us support for both level 1
    // and level 2 of PostScript.

    // define the 'BuildGlyph' procedure.  start by getting the
    // character name and the font dictionary from the stack.

    PrintString(pdev, "/BuildGlyph {0 begin /cn ed /fd ed\n");

    // retrieve the character information from the CharData (CD)
    // dictionary.

    PrintString(pdev, "/CI fd /CD get cn get def\n");

    // get the width and the bounding box from the CharData.
    // remember to divide the width by 16.

    PrintString(pdev, "/wx CI 0 get def /cbb CI 1 4 getinterval def\n");

    // enable each character to be cached.

    PrintString(pdev, "wx 0 cbb aload pop setcachedevice\n");

    // get the width and height of the bitmap, set invert bool to true
    // specifying reverse image.

    PrintString(pdev, "CI 5 get CI 6 get true\n");

    // insert x and y translation components into general imagemask
    // matrix.

    PrintString(pdev, "[1 0 0 -1 0 0] d 4 CI 7 get p d 5 CI 8 get p\n");

    // get hex string bitmap, convert into procedure, then print
    // the bitmap image.

    PrintString(pdev, "CI 9 1 getinterval cvx im end } def\n");

    // create local storage for 'BuildGlyph' procedure.

    PrintString(pdev, "/BuildGlyph load 0 5 dict put\n");

    // the semantics of 'BuildChar' differ from 'BuildGlyph' in the
    // following way:  'BuildChar' is called with the font dictionary
    // and character code on the stack, 'BuildGlyph' is called with
    // the font dictionary and character name on the stack.  the
    // following 'BuildChar' procedure calls 'BuildGlyph', and retains
    // compatiblity with level 1 PostScript.

    PrintString(pdev, "/BuildChar {1 index /Encoding get exch get\n");
    PrintString(pdev, "1 index /BuildGlyph get exec} bind def\n");

    // now create a dictionary containing information on each character.

    PrintString(pdev, "/CD ");
    PrintDecimal(pdev, 1, cGlyphs + 1);
    PrintString(pdev, " dict def CD begin\n");

    for (i = 0; i < cGlyphs; i++)
    {
        // get the GLYPHDATA structure for each glyph.

        if (!(cTmp = FONTOBJ_cGetGlyphs(pfo, FO_GLYPHBITS, 1, phg, (PVOID *)&pglyphdata)))
        {
            RIP("PSCRIPT!DownloadBitmapFont: cGetGlyphs failed.\n");
            HeapFree(pdev->hheap, 0, (PVOID)phgSave);
            HeapFree(pdev->hheap, 0, (PVOID)pDLFont->phgVector);
            return(FALSE);
        }

        // the first number in the character description is the width
        // in 1 unit font space.  the next four numbers are the bounding
        // box in 1 unit font space.  the next two numbers are the width
        // and height of the bitmap.  the next two numbers are the x and
        // y translation values for the matrix given to imagemask.
        // this is followed by the bitmap itself.

        // first, output the character name.

        PrintString(pdev, "/c");
        PrintDecimal(pdev, 1, i);

        // output the character description array.  the width and
        // bounding box need to be normalized to 1 unit font space.

        // the width will be sent to the printer as the actual width
        // multiplied by 16 so as not to lose any precision when
        // normalizing.

        PrintString(pdev, " [");
        PrintPSFIX(pdev, 1, (pglyphdata->fxD << 4));
        PrintString(pdev, " ");
        PrintDecimal(pdev, 4, pglyphdata->rclInk.left,
                     -pglyphdata->rclInk.top, pglyphdata->rclInk.right,
                     -pglyphdata->rclInk.bottom);
        PrintString(pdev, " ");

        // output the width and height of the bitmap itself.

        PrintDecimal(pdev, 2, pglyphdata->gdf.pgb->sizlBitmap.cx,
                     pglyphdata->gdf.pgb->sizlBitmap.cy);
        PrintString(pdev, " ");

        // output the translation values for the transform matrix.
        // the x component is usually the equivalent of the left
        // sidebearing in pixels.  the y component is always the height
        // of the bitmap minus any displacement factor (such as for characters
        // with descenders.

        psfxXtrans = -pglyphdata->gdf.pgb->ptlOrigin.x << 8;
        psfxYtrans = -pglyphdata->gdf.pgb->ptlOrigin.y << 8;

        PrintPSFIX(pdev, 2, psfxXtrans, psfxYtrans);
        PrintString(pdev, "\n<");

        // now output the bits.  calculate how many bytes each source scanline
        // contains.  remember that the bitmap will be padded to 32bit bounds.

        // protect ourselves.

        if ((pglyphdata->gdf.pgb->sizlBitmap.cx < 1) ||
            (pglyphdata->gdf.pgb->sizlBitmap.cy < 1))
        {
            RIP("PSCRIPT!DownloadBitmapFont: Invalid glyphdata!!!.\n");
            return(FALSE);
        }

        pjBits = pglyphdata->gdf.pgb->aj;

        for (j = 0; j < (DWORD)pglyphdata->gdf.pgb->sizlBitmap.cy; j++)
        {
            cjWidth = (LONG)(pglyphdata->gdf.pgb->sizlBitmap.cx + 7) >> 3;
            vHexOut(pdev, pjBits, cjWidth);
            pjBits += cjWidth;
        }

        PrintString(pdev, ">]def\n");

        // point to the next glyph handle.

        phg++;
    }

    // don't forget the .notdef character.

    PrintString(pdev, "/.notdef [.24 0 0 0 1 1 0 0 <>]def end\n");

    // create a unique ID for the font, then name it.

    pwstr = (PWSTR)((BYTE *)pifi + pifi->dpwszFaceName);
    cTmp = wcslen(pwstr);

    // get the font name from the UNICODE font name.

    memset(szFaceName, 0, sizeof(szFaceName));
    pszFaceName = szFaceName;

    while (cTmp--)
    {
        *pszFaceName = (CHAR)*pwstr++;

        // replace any spaces in the font name with underscores.

        if (*pszFaceName == ' ')
            *pszFaceName = '_';

        // replace any parens in the font name with asterisks.

        if ((*pszFaceName == '(') || (*pszFaceName == ')'))
            *pszFaceName = '*';

        // point to the next character.

        *pszFaceName++;
    }

    // add the point size to the font name, so we can distinguish
    // different point sizes of the same font.

    // NOTE  Grab a new point size here, rather than reading if from
    // the CGS.  This is necessary to support DrvDownloadFace.

    psfxPointSize = GetPointSize(pdev, pfo, &fontxform);

    cTmp = PSFIXToBuffer(pszFaceName, psfxPointSize);

    // update the buffer pointer.

    pszFaceName += cTmp;

    // output the NULL terminator.

    *pszFaceName = '\0';

    // output the unique id, which, in a postscript printer, can
    // be in the range from 0 to 16777215.

    PrintString(pdev, "/UniqueID ");
    PrintDecimal(pdev, 1, (pfo->iUniq & 0xFFFFF));

    PrintString(pdev, " def end pop /");
    PrintString(pdev, szFaceName);
    PrintString(pdev, " exch definefont pop\n");

    // update the fontname in our current graphics state.

    strcpy(pdev->cgs.szFont, szFaceName);

    // update information for this downloaded font.

    strcpy(pDLFont->strFont, szFaceName);
    pDLFont->psfxScaleFactor = psfxPointSize;

    // update the downloaded font counter.

    pdev->cgs.cDownloadedFonts++;

    // free up some memory.

    if (phgSave)
        HeapFree(pdev->hheap, 0, (PVOID)phgSave);
}


//--------------------------------------------------------------------
// BOOL DownloadOutlineFont(pdev, pfo)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
//
// This routine downloads the font definition for the given bitmap font,
// if it has not already been done.  The font is downloaded as an
// Adobe Type 3 font.
//
// This routine return TRUE if the font is successfully, or has already
// been, downloaded to the printer.  It returns FALSE if it fails.
//
// History:
//   27-Feb-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL DownloadOutlineFont(pdev, pfo)
PDEVDATA    pdev;
FONTOBJ    *pfo;
{
    DLFONT     *pDLFont;
    DWORD       i;
    DWORD       cDownloadedFonts;
    DWORD       cGlyphs, cTmp;
    HGLYPH     *phg;
    HGLYPH     *phgSave;
    GLYPHDATA  *pglyphdata;
    POINTL      ptlTL, ptlBR;
    PIFIMETRICS pifi;
    LONG        iCharWidth;
    CHAR        szFaceName[MAX_STRING];
    PSZ         pszFaceName;
    PWSTR       pwstr;

    // search through our list of downloaded GDI fonts to see if the
    // current font has already been downloaded to the printer.

    pDLFont = pdev->cgs.pDLFonts;

    cDownloadedFonts = min(pdev->cgs.cDownloadedFonts, pdev->iDLFonts);

    for (i = 0; i < pdev->iDLFonts; i++)
    {
        // is this entry the one we are looking for?  simply return if so.

        if (pDLFont->iUniq == pfo->iUniq)
        {
            // update the fontname and size in our current graphics state.

            strcpy(pdev->cgs.szFont, pDLFont->strFont);
            pdev->cgs.psfxScaleFactor = pDLFont->psfxScaleFactor;
            pdev->cgs.lidFont = pDLFont->iUniq;

            return(TRUE);
        }

        pDLFont++;
    }

    // we did not find that this font has been downloaded yet, so we must
    // do it now.

    // if we have reached our downloaded font threshold, then
    // we will surround ever textout call with a save/restore.

    if (cDownloadedFonts == pdev->iDLFonts)
    {
        ps_save(pdev, FALSE);
        pdev->cgs.dwFlags |= CGS_DLFONTTHRESHOLD;
        cDownloadedFonts--;
    }

    pDLFont = pdev->cgs.pDLFonts;
    pDLFont += cDownloadedFonts;

    // free up the memory for the hglyph array for the old font.

    if (pDLFont->phgVector)
    {
        HeapFree(pdev->hheap, 0, (PVOID)pDLFont->phgVector);
        pDLFont->phgVector = (HGLYPH *)NULL;
    }

    memset(pDLFont, 0, sizeof(DLFONT));

    pDLFont->iFace = pfo->iFace;
    pDLFont->iUniq = pfo->iUniq;

    // if we have made it this far, we should simply be able to
    // download the font now.

    // we will be downloading an Adobe TYPE 3 font.

    // allocate a dictionary for the font.

    PrintString(pdev, "10 dict d begin\n");

    // set FontType to 3 indicating user defined font.

    PrintString(pdev, "/FontType 3 def\n");

    // all font coordinate systems will be defined in units of 1000.
    // specify a FontMatrix which transforms the 1000 unit font to a
    // 1 unit font.

    PrintString(pdev, "/FontMatrix [.001 0 0 .001 0 0] def\n");

    // allocate memory for and get the handles for each glyph of the font.

    if (!(cGlyphs = FONTOBJ_cGetAllGlyphHandles(pfo, NULL)))
    {
        RIP("PSCRIPT!DownloadOutlineFont: cGetAllGlyphHandles failed.\n");
        return(FALSE);
    }

    if (!(phg = (HGLYPH *)HeapAlloc(pdev->hheap, 0, sizeof(HGLYPH) * cGlyphs)))
    {
        RIP("PSCRIPT!DownloadOutlineFont: HeapAlloc failed.\n");
        return(FALSE);
    }

    phgSave = phg;

    cTmp = FONTOBJ_cGetAllGlyphHandles(pfo, phg);

    ASSERTPS(cTmp == cGlyphs, "PSCRIPT!DownloadOutlineFont: inconsistent cGlyphs\n");

    // how many characters will we define in this font?
    // keep in mind that we can only do 256 at a time.
    // remember to leave room for the .notdef character.

    cGlyphs = min(255, cGlyphs);

    // get the IFIMETRICS for the font, used in TTTOADOBE.

    if (!(pifi = FONTOBJ_pifi(pfo)))
    {
        RIP("PSCRIPT!DownloadOutlineFont: pifi failed.\n");
        HeapFree(pdev->hheap, 0, (PVOID)phg);
        return(FALSE);
    }

    // run through the array, looking at the bounding box for each
    // glyph, in order to create the bounding box for the entire
    // font.

    ptlTL.x = ADOBE_FONT_UNITS;
    ptlTL.y = ADOBE_FONT_UNITS;
    ptlBR.x = 0;
    ptlBR.y = 0;

    for (i = 0; i < cGlyphs; i++)
    {
        // get the GLYPHDATA structure for each glyph.

        if (!(cTmp = FONTOBJ_cGetGlyphs(pfo, FO_PATHOBJ, 1, phg, (PVOID *)&pglyphdata)))
        {
            RIP("PSCRIPT!DownloadOutlineFont: cGetGlyphs failed.\n");
            HeapFree(pdev->hheap, 0, (PVOID)phgSave);
            return(FALSE);
        }

        ptlTL.x = min(ptlTL.x, TTTOADOBE(pglyphdata->rclInk.left));
        ptlTL.y = min(ptlTL.y, TTTOADOBE(pglyphdata->rclInk.top));
        ptlBR.x = max(ptlBR.x, TTTOADOBE(pglyphdata->rclInk.right));
        ptlBR.y = max(ptlBR.y, TTTOADOBE(pglyphdata->rclInk.bottom));

        // point to the next glyph handle.

        phg++;
    }

    // we have filled in the GLYPHPOS for each glyph in the font.
    // reset the pointer to the first glyph.

    phg = phgSave;

    // define the bounding box for the font.

    PrintString(pdev, "/FontBBox [");
    PrintDecimal(pdev, 4, ptlTL.x, ptlTL.y, ptlBR.x, ptlBR.y);
    PrintString(pdev, "] def\n");

    // allocate array for encoding vector, then initialize
    // all characters in encoding vector with '.notdef'.

    PrintString(pdev, "/Encoding 256 array def\n");
    PrintString(pdev, "0 1 255 {Encoding exch /.notdef p} for\n");

    // allocate space to store the HGLYPH<==>character code mapping.

    if (!(pDLFont->phgVector = (HGLYPH *)HeapAlloc(pdev->hheap, 0,
                      sizeof(HGLYPH) * cGlyphs)))
    {
        RIP("PSCRIPT!DownloadOutlineFont: HeapAlloc for phgVector failed.\n");
        HeapFree(pdev->hheap, 0, (PVOID)phg);
        return(FALSE);
    }

    // initialize array.

    pDLFont->cGlyphs = cGlyphs;

    // fill in the HGLYPH encoding vector, and output
    // the encoding vector to the printer.

    memcpy(pDLFont->phgVector, phg, cGlyphs * sizeof(HGLYPH));

    PrintString(pdev, "Encoding\n");

    for (i = 0; i < cGlyphs; i++)
    {
        PrintString(pdev, "d ");
        PrintDecimal(pdev, 1, i);
        PrintString(pdev, " /c");
        PrintDecimal(pdev, 1, i);
        PrintString(pdev, " p\n");
    }

    // under level 1 of PostScript, the 'BuildChar' procedure is called
    // every time a character from the font is constructed.  under
    // level 2, 'BuildGlyph' is called instead.  therefore, we will
    // define a 'BuildChar' procedure, which basically calls
    // 'BuildGlyph'.  this will provide us support for both level 1
    // and level 2 of PostScript.

    // define the 'BuildGlyph' procedure.  start by getting the
    // character name and the font dictionary from the stack.

    PrintString(pdev, "/BuildGlyph {0 begin /cn ed /fd ed\n");

    // retrieve the character information from the CharData (CD)
    // dictionary.

    PrintString(pdev, "/CI fd /CD get cn get def\n");

    // get the width and the bounding box from the CharData.

    PrintString(pdev, "/wx CI 0 get def /cbb CI 1 4 getinterval def\n");

    // enable each character to be cached.

    PrintString(pdev, "wx 0 cbb aload pop setcachedevice\n");

    // get the procedure for rendering the character and execute it.

    PrintString(pdev, "CI 5 get exec end } def\n");

    // create local storage for 'BuildGlyph' procedure.

    PrintString(pdev, "/BuildGlyph load 0 5 dict p\n");

    // the semantics of 'BuildChar' differ from 'BuildGlyph' in the
    // following way:  'BuildChar' is called with the font dictionary
    // and character code on the stack, 'BuildGlyph' is called with
    // the font dictionary and character name on the stack.  the
    // following 'BuildChar' procedure calls 'BuildGlyph', and retains
    // compatiblity with level 1 PostScript.

    PrintString(pdev, "/BuildChar {1 index /Encoding get exch get\n");
    PrintString(pdev, "1 index /BuildGlyph get exec} bd\n");

    // now create a dictionary containing information on each character.

    PrintString(pdev, "/CD ");
    PrintDecimal(pdev, 1, cGlyphs + 1);
    PrintString(pdev, " dict def CD begin\n");

    for (i = 0; i < cGlyphs; i++)
    {
        // get the GLYPHDATA structure for each glyph.

DbgPrint("calling cGetGlyphs.\n");
DbgBreakPoint();

        if (!(cTmp = FONTOBJ_cGetGlyphs(pfo, FO_PATHOBJ, 1, phg, (PVOID *)&pglyphdata)))
        {
            RIP("PSCRIPT!DownloadOutlineFont: cGetGlyphs failed.\n");
            HeapFree(pdev->hheap, 0, (PVOID)phgSave);
            return(FALSE);
        }

        // the first number in the character description is the width
        // in 1000 unit font space.  the next four numbers are the bounding
        // box in 1000 unit font space.  these are followed by the
        // procedure used to draw the character itself.

        // first, output the character name.

        PrintString(pdev, "/c");
        PrintDecimal(pdev, 1, i);

        // output the character description array.

        // the width will be sent to the printer as the actual width
        // multiplied by 16 so as not to lose any percision when
        // normalizing.

        iCharWidth = TTTOADOBE((LONG)pglyphdata->fxD);

        ptlTL.x = TTTOADOBE(pglyphdata->rclInk.left);
        ptlTL.y = TTTOADOBE(pglyphdata->rclInk.top);
        ptlBR.x = TTTOADOBE(pglyphdata->rclInk.right);
        ptlBR.y = TTTOADOBE(pglyphdata->rclInk.bottom);

        // remember to flip over the y coordinates.

        PrintString(pdev, " [");
        PrintDecimal(pdev, 5, iCharWidth, ptlTL.x, -ptlTL.y, ptlBR.x, -ptlBR.y);

        // output the procedure to draw the character itself.

        PrintString (pdev, " {");
        DrvCommonPath(pdev, pglyphdata->gdf.ppo);
        PrintString(pdev, "fill }]def\n");

        // point to the next glyph handle.

        phg++;
    }

    // don't forget the .notdef character.

    PrintString(pdev, "/.notdef [0 0 0 0 0 {}]def end\n");

    // create a unique ID for the font, then name it.

    pwstr = (PWSTR)((BYTE *)pifi + pifi->dpwszFaceName);
    cTmp = wcslen(pwstr);

    // get the font name from the UNICODE font name.

    pszFaceName = szFaceName;

    while (cTmp--)
    {
        *pszFaceName = (CHAR)*pwstr++;

        // replace any spaces in the font name with underscores.

        if (*pszFaceName == ' ')
            *pszFaceName = '_';

        // replace any parens in the font name with asterisks.

        if ((*pszFaceName == '(') || (*pszFaceName == ')'))
            *pszFaceName = '*';

        // point to the next character.

        *pszFaceName++;
    }

    // NULL terminate the font name.

    *pszFaceName = '\0';

    // output the unique id, which, in a postscript printer, can
    // be in the range from 0 to 16777215.

    PrintString(pdev, "/UniqueID ");
    PrintDecimal(pdev, 1, (pfo->iUniq & 0xFFFFF));

    PrintString(pdev, " def end pop /");
    PrintString(pdev, szFaceName);
    PrintString(pdev, " exch definefont pop\n");

    // update the fontname in our current graphics state.

    strcpy(pdev->cgs.szFont, szFaceName);

    // update the downloaded font counter.

    pdev->cgs.cDownloadedFonts++;

    // free up some memory.

    if (phgSave)
        HeapFree(pdev->hheap, 0, (PVOID)phgSave);
}


//--------------------------------------------------------------------
// BOOL SetFontRemap(pdev, iFontID)
// PDEVDATA    pdev;
// DWORD       iFontID;
//
// This routine adds the specified font to the list of remapped fonts.
//
// This routine return TRUE for success, FALSE otherwise.
//
// History:
//   11-Jun-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL SetFontRemap(pdev, iFontID)
PDEVDATA    pdev;
DWORD       iFontID;
{
    FREMAP *pfremap;

    // add the specified font id to the list of remapped fonts.

    pfremap = &pdev->cgs.FontRemap;

    // find the end of the list.

    if (pfremap->pNext)
    {
        while (pfremap->pNext)
            pfremap = (PFREMAP)pfremap->pNext;

        // allocate the next entry in the list.

        if (!(pfremap->pNext = (struct _FREMAP *)HeapAlloc(pdev->hheap, 0, sizeof(FREMAP))))
        {
            RIP("PSCRIPT!SetFontRemap: HeapAlloc failed.\n");
            return(FALSE);
        }

        pfremap = (PFREMAP)pfremap->pNext;
    }

    // now that we have found the last entry in the list, fill it in.

    pfremap->iFontID = iFontID;
    pfremap->pNext = NULL;

    return(TRUE);
}


//--------------------------------------------------------------------
// BOOL QueryFontRemap(pdev, iFontID)
// PDEVDATA    pdev;
// DWORD       iFontID;
//
// This routine scans the list of remapped fonts.
//
// This routine return TRUE if iFontID is found, FALSE otherwise.
//
// History:
//   11-Jun-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL QueryFontRemap(pdev, iFontID)
PDEVDATA    pdev;
DWORD       iFontID;
{
    FREMAP *pfremap;

    // add the specified font id to the list of remapped fonts.

    pfremap = &pdev->cgs.FontRemap;

    // search the list for iFontID.

    do
    {
        // return TRUE if we have found the font.

        if (pfremap->iFontID == iFontID)
            return(TRUE);

        // we have not found the font, point to the next entry.

        pfremap = (PFREMAP)pfremap->pNext;

    } while (pfremap);

    // we did not find the font.

    return(FALSE);
}


//--------------------------------------------------------------------
// DWORD SubstituteIFace(pdev, pfo)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
//
// This routine takes the TrueType font specified by pfo, and returns
// the iFace of the device font to substitute.  This routine returns
// zero if no font is found for substitution.
//
// History:
//   14-Jul-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

DWORD SubstituteIFace(pdev, pfo)
PDEVDATA    pdev;
FONTOBJ    *pfo;
{
    DWORD               iFace = 0;
    DWORD               iTmp;
    CHAR                strDevFont[MAX_FONT_NAME];
    WCHAR               wstrDevFont[MAX_FONT_NAME];
    PSTR                pstrDevFont;
    PWSTR               pwstrDevFont;
    IFIMETRICS         *pifiTT;
    DWORD               cTmp;
    PWSTR               pwstr, pwstrTT;
    PNTFM               pntfm;
    BOOL                bFound;

#if DBG
    // make sure we have a TrueType font.

    if (!(pfo->flFontType & TRUETYPE_FONTTYPE))
    {
        DbgPrint("PSCRIPT!SubstituteIFace: Trying to substitute for non-TT font.\n");
        return(0);
    }

    // make sure we are supposed to be doing font substitution.

    if (!(pdev->psdm.dwFlags & PSDEVMODE_FONTSUBST))
    {
        DbgPrint("PSCRIPT!SubstituteIFace: not supposed to font substitute.\n");
        return(0);
    }
#endif

    // get the TrueType font name from the IFIMETRICS structure.

    if (!(pifiTT = FONTOBJ_pifi(pfo)))
    {
        RIP("PSCRIPT!SubstituteIFace: FONTOBJ_pifiTT failed.\n");
        return(0);
    }

    pwstrTT = (PWSTR)((BYTE *)pifiTT + pifiTT->dpwszFaceName);

    // now search the font substitution table for a matching TrueType font.
    // the substitution table is in the following format:  a NULL terminated
    // UNICODE TrueType font name followed by the matching NULL terminated
    // device font name.  this sequence is repeated until a double NULL
    // terminator ends the table.

    pwstr = pdev->pTTSubstTable;
    bFound = FALSE;

    while (*pwstr)
    {
        if (!(wcscmp(pwstr, pwstrTT)))
        {
            // we found the TrueType font, now get the matching device font.

            pwstr += (wcslen(pwstr) + 1);
            wcsncpy(wstrDevFont, pwstr,
                    (sizeof(wstrDevFont) / sizeof(wstrDevFont[0])));
            bFound = TRUE;
            break;
        }
        else
        {
            // this was not the font in question.  skip over both font names.

            pwstr += (wcslen(pwstr) + 1);
            pwstr += (wcslen(pwstr) + 1);
        }
    }

    // if we could not get a corresponding device font for any reason,
    // simply return zero for the iFace.

    if (!bFound)
        return(0);

    // get an ANSI version of the font name.

    cTmp = wcslen(wstrDevFont);
    cTmp++;

    pwstrDevFont = wstrDevFont;
    pstrDevFont = strDevFont;

    while (cTmp--)
        *pstrDevFont++ = (CHAR)*pwstrDevFont++;

    *pstrDevFont = '\0';

    // at this point we have a mapping between a TrueType font name,
    // and a device font name.  we need to determine the iFace of
    // the font name.

    for (iTmp = 1; iTmp <= (pdev->cDeviceFonts + pdev->cSoftFonts); iTmp++)
    {
        // get the font metrics for the specified font.

        pntfm = pdev->pfmtable[iTmp - 1].pntfm;

        // see if it is the font family we are looking for.  if it is,
        // select it into the current graphics state, and return its iFace.

        if (!strcmp(strDevFont, ((char *)pntfm + pntfm->loszFontName)))
        {
            // select the device font in the current graphics state.

            strcpy(pdev->cgs.szFont, strDevFont);
            iFace = iTmp;
            break;
        }
    }

    // return the iFace of the device font to the caller.

    return(iFace);
}



//--------------------------------------------------------------------
// ULONG DrvGetGlyphMode(dhpdev, pfo, iMode)
// DHPDEV      dhpdev;
// FONTOBJ    *pfo;
// ULONG       iMode;
//
// This routine returns to the engine, the type of font caching the
// engine should do.
//
// History:
//   22-Jul-1992    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

ULONG DrvGetGlyphMode(dhpdev, pfo)
DHPDEV      dhpdev;
FONTOBJ    *pfo;
{
    return(SO_MONOBITMAP);
//    return(SO_GLYPHHANDLES);
}



//--------------------------------------------------------------------
// LONG iHipot(x, y)
// LONG x;
// LONG y;
//
// This routine returns the hypoteneous of a right triangle.
//
// FORMULA:
//          use sq(x) + sq(y) = sq(hypo);
//          start with MAX(x, y),
//          use sq(x + 1) = sq(x) + 2x + 1 to incrementally get to the
//          target hypotenouse.
//
// History:
//   10-Feb-1993    -by-    Kent Settle     (kentse)
//  Stole from RASDD.
//   21-Aug-1991    -by-    Lindsay Harris  (lindsayh)
//  Cleaned up UniDrive version, added comments etc.
//--------------------------------------------------------------------

LONG iHipot(x, y)
LONG x;
LONG y;
{
    register int  hypo;         /* Value to calculate */
    register int  delta;        /* Used in the calculation loop */

    int   target;               /* Loop limit factor */

// quick exit for frequent trivial cases [bodind]

    if (x == 0)
    {
        return ((y > 0) ? y : -y);
    }

    if (y == 0)
    {
        return ((x > 0) ? x : -x);
    }

    if (x < 0)
        x = -x;

    if (y < 0)
        y = -y;

    if(x > y)
    {
        hypo = x;
        target = y * y;
    }
    else
    {
        hypo = y;
        target = x * x;
    }

    for (delta = 0; delta < target; hypo++)
        delta += hypo << 1 + 1;

    return hypo;
}


//--------------------------------------------------------------------------
// BOOL IsJustifiedText(pdev, pfo, pstro, pptSpace, pptNonSpace, pdata)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// STROBJ     *pstro;
// POINTPSFX  *pptSpace;
// POINTPSFX  *pptNonSpace;
// TEXTDATA   *pdata;
//
// This routine analyzes the string defined in pstro.  If the spacing after
// each non-space character is the same (with in 1), and the spacing after
// each space character is the same, this routine will fill in the
// POINTPSFX for each value, and return TRUE, otherwise it returns FALSE.
//
// History:
//   31-Mar-1993     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------


BOOL IsJustifiedText(pdev, pfo, pstro, pptSpace, pptNonSpace, pdata)
PDEVDATA    pdev;
FONTOBJ    *pfo;
STROBJ     *pstro;
POINTPSFX  *pptSpace;
POINTPSFX  *pptNonSpace;
TEXTDATA   *pdata;
{
    DWORD       i;
    TEXTDELTA  *pdelta;
    TEXTDELTA  *pdeltaSave;
    POINTFIX    ptfxSpace, ptfxNonSpace, ptfxTmp;
    POINTFIX    ptfxSExtra, ptfxNSExtra;
    GLYPHPOS   *pgp;
    WCHAR       wcSpace;
    FLONG       flAccel;
    LONG        cSpace, cNonSpace, denom;
    DWORD       cGlyphs;
    BOOL        bMore;
    PWSZ        pwszOrg;

    flAccel = pstro->flAccel;

    // currently, just handle horizontal text.  justification for a string
    // with one character makes no sense.  and, justification is
    // obviously not happening, if we are using default placement.
    // finally, we must individually place the characters if substitution
    // is in effect, because it is the only way we have of getting the
    // true character widths.

    if ((!(flAccel & SO_HORIZONTAL)) || (pstro->cGlyphs <= 1) ||
        (flAccel & SO_FLAG_DEFAULT_PLACEMENT) || pdata->bFontSubstitution)
        return(FALSE);

    // now the ugly part.  we must go through the string a character at a
    // time, comparing the widths given in the pstro with the actual font
    // character widths.  if the difference is the same for each space
    // character, and the same for each non-space character, then
    // justification, as we know it, is in effect.

    // first start off by allocating an array of TEXTDELTAs, so we can fill
    // them in with the deltas between each character.

    if (!(pdelta = (TEXTDELTA *)HeapAlloc(pdev->hheap, 0,
                                      (sizeof(TEXTDELTA) * (pstro->cGlyphs - 1)))))
    {
#if DBG
        DbgPrint("PSCRIPT!IsJustifiedText: HeapAlloc for pdelta failed.\n");
#endif
        return(FALSE);
    }

    pdeltaSave = pdelta;

    // get the original UNICODE string.

    if (pfo->flFontType & DEVICE_FONTTYPE)
        pwszOrg = GetUnicodeString(pdev, pfo, pstro);
    else
        pwszOrg = pstro->pwszOrg;

    // fill in the TEXTDELTA structures.

    if (pstro->pgp)
    {
        if (!FillDeltaArray(pdev, pfo, pstro->pgp, pstro, pdelta, pstro->cGlyphs, pwszOrg))
        {
            RIP("PSCRIPT!IsJustifiedText: FillDeltaArray failed.\n");
            HeapFree(pdev->hheap, 0, (PVOID)pdeltaSave);
            return(FALSE);
        }
    }
    else
    {
        // prepare to enumerate the string properly.

        STROBJ_vEnumStart(pstro);

        // now draw the text.

        do
        {
            bMore = STROBJ_bEnum(pstro, &cGlyphs, &pgp);

            if (!FillDeltaArray(pdev, pfo, pgp, pstro, pdelta, cGlyphs, pwszOrg))
            {
                RIP("PSCRIPT!IsJustifiedText: FillDeltaArray failed.\n");
                HeapFree(pdev->hheap, 0, (PVOID)pdeltaSave);
                return(FALSE);
            }

            pdelta += cGlyphs;
            pwszOrg += cGlyphs;

        } while (bMore);
    }

    // reset pointer to first structure.

    pdelta = pdeltaSave;

    // get the unicode value for the space character.

    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)" ", 1, (LPWSTR)&wcSpace, 1);

    // now get the widths of the characters themselves.

    ptfxSpace.x = -1;
    ptfxNonSpace.x = -1;
    ptfxSExtra.x = 0;
    ptfxSExtra.y = 0;
    ptfxNSExtra.x = 0;
    ptfxNSExtra.y = 0;
    cSpace = 0;
    cNonSpace = 0;

    for (i = 0; i < (pstro->cGlyphs - 1); i++)
    {
        ptfxTmp.x = abs(pdelta->ptfxorg.x - LTOFX(pdelta->ptlpgp.x));
        ptfxTmp.y = abs(pdelta->ptfxorg.y - LTOFX(pdelta->ptlpgp.y));

        if (pdelta->wc == wcSpace)
        {
            if (ptfxSpace.x == -1)
            {
                ptfxSpace = ptfxTmp;
                ptfxSExtra.x = LTOFX(pdelta->ptlpgp.x) - pdelta->ptfxorg.x;
                ptfxSExtra.y = LTOFX(pdelta->ptlpgp.y) - pdelta->ptfxorg.y;
            }
            else
            {
                if ((abs(ptfxSpace.x - ptfxTmp.x) > LTOFX(1)) ||
                    (abs(ptfxSpace.y - ptfxTmp.y) > LTOFX(1)))
                {
                    HeapFree(pdev->hheap, 0, (PVOID)pdeltaSave);
                    return(FALSE);
                }

                ptfxSExtra.x += (LTOFX(pdelta->ptlpgp.x) - pdelta->ptfxorg.x);
                ptfxSExtra.y += (LTOFX(pdelta->ptlpgp.y) - pdelta->ptfxorg.y);
            }

            cSpace++;
        }
        else    // non spacing character.
        {
            if (ptfxNonSpace.x == -1)
            {
                ptfxNonSpace = ptfxTmp;
                ptfxNSExtra.x = LTOFX(pdelta->ptlpgp.x) - pdelta->ptfxorg.x;
                ptfxNSExtra.y = LTOFX(pdelta->ptlpgp.y) - pdelta->ptfxorg.y;
            }
            else
            {
                if ((abs(ptfxNonSpace.x - ptfxTmp.x) > LTOFX(1)) ||
                    (abs(ptfxNonSpace.y - ptfxTmp.y) > LTOFX(1)))
                {
                    HeapFree(pdev->hheap, 0, (PVOID)pdeltaSave);
                    return(FALSE);
                }

                ptfxNSExtra.x += (LTOFX(pdelta->ptlpgp.x) - pdelta->ptfxorg.x);
                ptfxNSExtra.y += (LTOFX(pdelta->ptlpgp.y) - pdelta->ptfxorg.y);
            }

            cNonSpace++;
        }

        pdelta++;
    }

    // fill in the justification amounts.

    if (cSpace == 0)
    {
        pptSpace->x = 0;
        pptSpace->y = 0;
    }
    else
    {
        denom = pdev->psdm.dm.dmPrintQuality * cSpace;

        pptSpace->x = ((ptfxSExtra.x << 4) * PS_RESOLUTION) / denom;
        pptSpace->y = ((ptfxSExtra.y << 4) * PS_RESOLUTION) / denom;
    }

    if (cNonSpace == 0)
    {
        pptNonSpace->x = 0;
        pptNonSpace->y = 0;
    }
    else
    {
        denom = pdev->psdm.dm.dmPrintQuality * cNonSpace;

        pptNonSpace->x = ((ptfxNSExtra.x << 4) * PS_RESOLUTION) / denom;
        pptNonSpace->y = ((ptfxNSExtra.y << 4) * PS_RESOLUTION) / denom;
    }

    // free some memory.

    HeapFree(pdev->hheap, 0, (PVOID)pdeltaSave);

    return(TRUE);
}


//--------------------------------------------------------------------------
// BOOL FillDeltaArray(pdev, pfo, pgp, pstro, pdelta, cGlyphs, pwsz)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// GLYPHPOS   *pgp;
// STROBJ     *pstro;
// TEXTDELTA  *pdelta;
// DWORD       cGlyphs;
// PWSZ        pwsz;
//
// This routine fills in the given TEXTDELTA array from the given STROBJ.
//
// History:
//   31-Mar-1993     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL FillDeltaArray(pdev, pfo, pgp, pstro, pdelta, cGlyphs, pwsz)
PDEVDATA    pdev;
FONTOBJ    *pfo;
GLYPHPOS   *pgp;
STROBJ     *pstro;
TEXTDELTA  *pdelta;
DWORD       cGlyphs;
PWSZ        pwsz;
{
    DWORD       i;
    GLYPHDATA  *pglyphdata;
    DWORD       cReturned;
    GLYPHDATA   glyphdata;

    if (!pwsz)
    {
#if DBG
        DbgPrint("PSCRIPT!FillDeltaArray: NULL pwsz.\n");
#endif
        return(FALSE);
    }

    // initialize pglyphdata for device font case.

    pglyphdata = &glyphdata;

    // fill in the TEXTDELTA structures.

    for (i = 0; i < (cGlyphs - 1); i++)
    {
        // get the UNICODE code point.

        pdelta->wc = *pwsz++;

        // get the delta vector as defined by the GLYPHPOS.

        pdelta->ptlpgp.x = (pgp + 1)->ptl.x - pgp->ptl.x;
        pdelta->ptlpgp.y = (pgp + 1)->ptl.y - pgp->ptl.y;

        if (pfo->flFontType & DEVICE_FONTTYPE)
        {
            if (!GetDeviceWidths(pdev, pfo, &glyphdata, pgp->hg))
            {
                RIP("PSCRIPT!FillDeltaArray: GetDeviceWidth failed.\n");
                return(FALSE);
            }
        }
        else
        {
            // now get the delta vector for the glyph itself.

            if (!(cReturned = FONTOBJ_cGetGlyphs(pfo, FO_GLYPHBITS, 1,
                                                 &pgp->hg,
                                                 (PVOID *)&pglyphdata)))
            {
                RIP("PSCRIPT!FillDeltaArray: cGetGlyphs failed.\n");
                return(FALSE);
            }
        }

        pdelta->ptfxorg.x = pglyphdata->ptqD.x.HighPart;
        pdelta->ptfxorg.y = pglyphdata->ptqD.y.HighPart;

        pgp++;
        pdelta++;
    }

    return(TRUE);
}


//--------------------------------------------------------------------------
// PWSTR GetUnicodeString(pdev, pfo, pstro)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// STROBJ     *pstro;
//
// This routine allocates a UNICODE string, and fills it is from the pstro.
// This routine is ONLY called for device fonts.
//
// History:
//   10-May-1993     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

PWSTR GetUnicodeString(pdev, pfo, pstro)
PDEVDATA    pdev;
FONTOBJ    *pfo;
STROBJ     *pstro;
{
    PWSTR       pwstr, pwstrSave;
    BOOL        bMore;
    PUCMap      pmap;
    PNTFM       pntfm;
    DWORD       cGlyphs;
    GLYPHPOS   *pgp;

    // allocate the UNICODE string, leaving room for NULL terminator.

    if (!(pwstr = (PWSTR)HeapAlloc(pdev->hheap, 0,
                                   ((pstro->cGlyphs + 1) * sizeof(WCHAR)))))
    {
        RIP("PSCRIPT!GetUnicodeString: HeapAlloc for pwstr failed.\n");
        return((PWSTR)NULL);
    }

    pwstrSave = pwstr;

    // prepare to enumerate the string.

    STROBJ_vEnumStart(pstro);

    // get the GLYPHPOS structure for each glyph.  from that, get the
    // HGLYPH.  from that, get the UNICODE codepoint, and jam that into
    // our allocate UNICODE string.

    // get the font metrics for the specified font.

    pntfm = pdev->pfmtable[pfo->iFace - 1].pntfm;

    // get local pointer to mapping table for current font.

    if (!strcmp((char *)pntfm + pntfm->loszFontName, "Symbol"))
        pmap = SymbolMap;
    else if (!strcmp((char *)pntfm + pntfm->loszFontName, "ZapfDingbats"))
        pmap = DingbatsMap;
    else
        pmap = LatinMap;

    if (pstro->pgp)
        ConstructUCString(pstro->pgp, pmap, pstro->cGlyphs, pwstr);
    else
    {
        do
        {
            bMore = STROBJ_bEnum(pstro, &cGlyphs, &pgp);

            ConstructUCString(pgp, pmap, cGlyphs, pwstr);

            pwstr += cGlyphs;

        } while (bMore);
    }

    return(pwstrSave);
}


//--------------------------------------------------------------------------
// VOID ConstructUCString(pgp, pmap, cGlyphs, pwstr)
// GLYPHPOS   *pgp;
// PUCMap      pmap;
// DWORD       cGlyphs;
// PWSTR       pwstr;
//
// This routine constructs a UNICODE string from the pstro.
// This routine is ONLY called for device fonts.
//
// History:
//   10-May-1993     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

VOID ConstructUCString(pgp, pmap, cGlyphs, pwstr)
GLYPHPOS   *pgp;
PUCMap      pmap;
DWORD       cGlyphs;
PWSTR       pwstr;
{
    BOOL        bFound;
    DWORD       i;
    PUCMap      pmapReset;
    WCHAR      *pwc;
    CHAR        jChar;

    // assume character not found in font.

    bFound = FALSE;

    pwc = pwstr;
    pmapReset = pmap;

    for (i = 0; i < cGlyphs; i++)
    {
        pmap = pmapReset;

        jChar = (CHAR)pgp->hg;

        while (pmap->szChar)
        {
            // search for the matching code in mapping.h.  remember
            // that the high bit is used to indicate the font needs
            // to be remapped.  so ignore the high bit while checking
            // for a character match.

            if (jChar == (CHAR)pmap->usPSValue)
            {
                bFound = TRUE;

                *pwc = (WCHAR)pmap->usUCValue;
                break;
            }

            // point to the next character in mapping.h.

            pmap++;
        } // while.

        // if the character in question was not found, replace it with
        // our default character.

        if (!bFound)
        {
            // get the unicode value for the period character.

            MultiByteToWideChar(CP_ACP, 0, (LPCSTR)".", 1, (LPWSTR)pwc, 1);
        }

        // point to the next glyph to fill in.

        pwc++;

        // point to the next GLYPHPOS structure.

        pgp++;
    }

    // add the NULL terminator.

    *pwc = (WCHAR)'\0';
}


//--------------------------------------------------------------------------
// BOOL GetDeviceWidths(pdev, pfo, pgd, hg)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// GLYPHDATA  *pgd;
// HGLYPH      hg;
//
// This routine fills in the width parameters of a GLYPHDATA
// structure for the given HGLYPH.  This is currently needed since
// the journaling code does not allow us to do engine callbacks
// such as FONTOBJ_cGetGlyphs with a device font.
//
// History:
//   24-May-1993     -by-     Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------------

BOOL GetDeviceWidths(pdev, pfo, pgd, hg)
PDEVDATA    pdev;
FONTOBJ    *pfo;
GLYPHDATA  *pgd;
HGLYPH      hg;
{
    PNTFM       pntfm;
    XFORMOBJ   *pxo;
    PBYTE	pCharCode;
    PBYTE	pCode;
    PUSHORT     pCharWidth;
    int         i;
    POINTL      ptl;
    POINTFIX    ptfx;
    LONG        lWidth;

    // make sure we have been given a valid font.

    if ((pfo->iFace == 0) || (pfo->iFace > (pdev->cDeviceFonts + pdev->cSoftFonts)))
    {
        RIP("PSCRIPT!GetDeviceWidths: invalid iFace.\n");
	SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
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

    XFORMOBJ_iGetXform(pxo, &pdev->cgs.FontXform);

    // fill in the glyph widths portions of the GLYPHDATA structure.

    // We don't actually return bitmaps, but we give them
    // metrics via this call.

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

#if 0
    // now fill in the GLYPHDATA structure.
    // remember under NT 0,0 is top left, while under
    // PostScript 0,0 is bottom left.  return device coords.

    ptl.x = pntfm->rcBBox.left;
    ptl.y = 0;

    XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

    pgd->rclInk.left = FXTOL(iHipot(ptfx.x, ptfx.y));

    ptl.x = pntfm->rcBBox.bottom;
    ptl.y = 0;

    XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

    pgd->rclInk.top = FXTOL(iHipot(ptfx.x, ptfx.y));

    ptl.x = pntfm->rcBBox.right;
    ptl.y = 0;

    XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

    pgd->rclInk.right = FXTOL(iHipot(ptfx.x, ptfx.y));

    ptl.x = pntfm->rcBBox.top;
    ptl.y = 0;

    XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

    pgd->rclInk.bottom = FXTOL(iHipot(ptfx.x, ptfx.y));
#endif

    ptl.x = lWidth;
    ptl.y = 0;

    XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

    pgd->fxD = iHipot(ptfx.x, ptfx.y);

    pgd->ptqD.x.HighPart = ptfx.x;
    pgd->ptqD.x.LowPart = 0;
    pgd->ptqD.y.HighPart = ptfx.y;
    pgd->ptqD.y.LowPart = 0;

#if 0
    pgd->fxA = 0;
    pgd->fxAB = pgd->fxD;

    pgd->fxInkTop    = - LTOFX(pgd->rclInk.top);
    pgd->fxInkBottom = - LTOFX(pgd->rclInk.bottom);
#endif

    pgd->hg = hg;

    return(TRUE);
}
