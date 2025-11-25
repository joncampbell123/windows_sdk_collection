//--------------------------------------------------------------------------
//
// Module Name:  FNTMANAG.C
//
// Brief Description:  This module contains the PSCRIPT driver's
// DrvFontManagement function and related routines.
//
// Author:  Kent Settle (kentse)
// Created: 07-May-1993
//
// Copyright (c) 1993 Microsoft Corporation
//--------------------------------------------------------------------------

#include "stdlib.h"
#include <string.h>
#include "pscript.h"
#include "enable.h"
#include "winbase.h"

// declarations of external routines.

extern LONG iHipot(LONG, LONG);
extern DWORD PSFIXToBuffer(CHAR *, PS_FIX);

// declarations of routines residing within this module.

BOOL ForceLoadFont(PDEVDATA, FONTOBJ *, DWORD, HGLYPH *);
BOOL GrabFaceName(PDEVDATA, FONTOBJ *, CHAR *, DWORD);
PS_FIX GetPointSize(PDEVDATA, FONTOBJ *, XFORM *);
BOOL DownloadANSIBitmapFont(PDEVDATA, FONTOBJ *, HGLYPH *);
BOOL SendCharBitmap(PDEVDATA, FONTOBJ *, HGLYPH, DWORD, BOOL);

//--------------------------------------------------------------------------
// BOOL DrvFontManagement(pfo, iType, pvIn, cjIn, pvOut, cjOut)
// FONTOBJ    *pfo;
// DWORD       iType;
// PVOID       pvIn;
// DWORD       cjIn;
// PVOID       pvOut;
// DWORD       cjOut;
//
// This routine handles multiple font management related functions,
// depending on iType.
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

ULONG DrvFontManagement(pso, pfo, iType, cjIn, pvIn, cjOut, pvOut)
SURFOBJ    *pso;
FONTOBJ    *pfo;
DWORD       iType;
DWORD       cjIn;
PVOID       pvIn;
DWORD       cjOut;
PVOID       pvOut;
{
    PDEVDATA    pdev;

    // pso may be NULL if QUERYESCSUPPORT.

    if (iType != QUERYESCSUPPORT)
    {
        // get the pointer to our DEVDATA structure and make sure it is ours.

        pdev = (PDEVDATA) pso->dhpdev;

        if (bValidatePDEV(pdev) == FALSE)
        {
            RIP("PSCRIPT!DrvFontManagement: invalid pdev.\n");
            SetLastError(ERROR_INVALID_PARAMETER);
            return(FALSE);
        }
    }

    // handle the different cases.

    switch (iType)
    {
        case QUERYESCSUPPORT:
            // when querying escape support, the function in question is
            // passed in the ULONG passed in pvIn.

            switch (*(PULONG)pvIn)
            {
                case QUERYESCSUPPORT:
                case DOWNLOADFACE:
                case GETFACENAME:
                    return(1);

                default:
                    // return 0 if the escape in question is not supported.

		    return(0);
            }

        case DOWNLOADFACE:
            // call ForceLoadFont to do the work.

            return(ForceLoadFont(pdev, pfo, cjIn, (PHGLYPH)pvIn));

        case GETFACENAME:
            // call GrabFaceName to do the work.

            return(GrabFaceName(pdev, pfo, (CHAR *)pvOut, cjOut));

        default:
            return(FALSE);
    }
    return(TRUE);
}


//--------------------------------------------------------------------
// BOOL ForceLoadFont(pdev, pfo)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
//
// This routine downloads the specified font to the printer, no
// questions asked.
//
// History:
//   07-May-1993    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL ForceLoadFont(pdev, pfo, cjIn, phglyphs)
PDEVDATA    pdev;
FONTOBJ    *pfo;
DWORD       cjIn;
HGLYPH     *phglyphs;
{
    PNTFM       pntfm;
    BYTE       *pSoftFont;
    BOOL        bDeviceFont;
    XFORM       fontxform;
    PS_FIX      psfxScaleFactor;
    BOOL        bProcSet;

    // make sure we have our hglyph => ANSI translation table.
    // the table consists of 256 HGLYPHS, plus two WORDS at the
    // beginning.  The first WORD states whether to always download
    // the font, or just if it has not yet been done.  The second
    // WORD is simply padding for alignment.

    if (cjIn < (sizeof(HGLYPH) * 257))
    {
        RIP("PSCRIPT!ForceLoadFont: invalid cjIn.\n");
        SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
    }
    // get the point size, and fill in the font xform.

    psfxScaleFactor = GetPointSize(pdev, pfo, &fontxform);

    // is this a device font?

    bDeviceFont = (pfo->flFontType & DEVICE_FONTTYPE);

    // select the proper font name for the new font.  if this is a
    // device font, get the name from the NTFM structure.  if this
    // is a GDI font that we are caching, we will create a name for
    // it at the time we download it to the printer.

    if (bDeviceFont)
    {
        // get the font metrics for the specified font.

        pntfm = pdev->pfmtable[pfo->iFace - 1].pntfm;

// !!! NOTE NOTE the following assumption is invalid.  I need to look at the
// !!! first word of phglyph to decide whether to always download the font or
// !!! only download it if it has not yet been downloaded.

//!!! I am writing this with the assumption, that the application will worry
//!!! about printer memory.  In other words, I will just blindly download
//!!! a font when I am told to, and not worry about killing the printer.
//!!! Is this a valid assumption???

//!!! I am also assuming that I do not have to keep track of which fonts
//!!! have been downloaded.

        // if the font is a softfont, download it.

        if (pfo->iFace > pdev->cDeviceFonts)
        {
            pSoftFont = (BYTE *)pntfm + pntfm->loSoftFont;

            // we need to make sure that our header does not get downloaded
            // by bPSWrite

            // note whether or not the procset has already been sent.

            if (pdev->dwFlags & PDEV_PROCSET)
                bProcSet = TRUE;
            else
            {
                pdev->dwFlags |= PDEV_PROCSET;
                bProcSet = FALSE;
            }


            if (!bPSWrite(pdev, pSoftFont, pntfm->cjSoftFont))
            {
                RIP("PSCRIPT!SelectFont: downloading of softfont failed.\n");
                return(FALSE);
            }

            // if the procset was not sent, then clear the flag.

            if (!bProcSet)
                pdev->dwFlags &= ~PDEV_PROCSET;
        }
    }
    else // must be a GDI font we will be caching.
    {
        // we need to make sure that our header does not get downloaded
        // by DownloadANSIBitmapFont.

        // note whether or not the procset has already been sent.

        if (pdev->dwFlags & PDEV_PROCSET)
            bProcSet = TRUE;
        else
        {
            pdev->dwFlags |= PDEV_PROCSET;
            bProcSet = FALSE;
        }

//!!! the engine is not ready for outline fonts yet!!!  -kentse.
#if 0
        // if this font has not yet been downloaded to the printer,
        // do it now.

        if (pfo->flFontType & TRUETYPE_FONTTYPE)
        {
            // determine the point size.

            ulPointSize = (ETOL(fontxform.eM22 * 72000) /
                          pdev->psdm.dm.dmPrintQuality);

            if (ulPointSize < 10)
                DownloadANSIBitmapFont(pdev, pfo, phglyphs);
            else
                DownloadOutlineFont(pdev, pfo);
        }
        else if (pfo->flFontType & RASTER_FONTTYPE)
            DownloadANSIBitmapFont(pdev, pfo, phglyphs);
#endif
        if ( (pfo->flFontType & TRUETYPE_FONTTYPE) ||
             (pfo->flFontType & RASTER_FONTTYPE) )
            DownloadANSIBitmapFont(pdev, pfo, phglyphs);
        else
        {
            RIP("PSCRIPT!SelectFont: invalid pfo->flFontType.\n");
            return(FALSE);
        }

        // if the procset was not sent, then clear the flag.

        if (!bProcSet)
            pdev->dwFlags &= ~PDEV_PROCSET;
    }

    return(TRUE);
}


//--------------------------------------------------------------------
// BOOL GrabFaceName(pdev, pfo, pbuffer, cb)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// CHAR       *pbuffer;
// DWORD       cb;
//
// This routine returns the driver's internal facename (ie the name
// which is sent to the printer) to the caller.  pbuffer, is filled
// in with the face name, being sure to not write more than cb bytes
// to the buffer.
//
// History:
//   07-May-1993    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL GrabFaceName(pdev, pfo, pbuffer, cb)
PDEVDATA    pdev;
FONTOBJ    *pfo;
CHAR       *pbuffer;
DWORD       cb;
{
    PNTFM       pntfm;
    BOOL        bDeviceFont;
    PIFIMETRICS pifi;
    XFORM       fontxform;
    PS_FIX      psfxScaleFactor;
    PWSTR       pwstr;
    DWORD       cTmp;
    CHAR        szFaceName[MAX_STRING];
    PSZ         pszFaceName;

    // get the point size, and fill in the font xform.

    psfxScaleFactor = GetPointSize(pdev, pfo, &fontxform);

    // is this a device font?

    bDeviceFont = (pfo->flFontType & DEVICE_FONTTYPE);

    // select the proper font name for the new font.  if this is a
    // device font, get the name from the NTFM structure.  if this
    // is a GDI font that we are caching, we will create a name for
    // it at the time we download it to the printer.

    if (bDeviceFont)
    {
        // get the font metrics for the specified font.

        pntfm = pdev->pfmtable[pfo->iFace - 1].pntfm;

        // copy the font name to the buffer.

        strncpy(pbuffer, (char *)pntfm + pntfm->loszFontName, cb);
    }
    else // must be a GDI font we will be caching.
    {
        if ( (pfo->flFontType & TRUETYPE_FONTTYPE) ||
             (pfo->flFontType & RASTER_FONTTYPE) )
        {
            // create the ASCII name for this font which will get used
            // to select this font in the printer.

            if (!(pifi = FONTOBJ_pifi(pfo)))
            {
                RIP("PSCRIPT!SelectFont: pifi failed.\n");
                return(FALSE);
            }

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

            cTmp = PSFIXToBuffer(pszFaceName, psfxScaleFactor);

            // update the buffer pointer.

            pszFaceName += cTmp;

            // output the NULL terminator.

            *pszFaceName = '\0';

            // copy to the output buffer.

            strncpy(pbuffer, szFaceName, cb);
        }
        else
        {
            RIP("PSCRIPT!GrabFaceName: invalid pfo->flFontType.\n");
            return(FALSE);
        }
    }
}


//--------------------------------------------------------------------
// PS_FIX GetPointSize(pdev, pfo, pxform)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// XFORM      *pxform;
//
// This routine returns the point size for the specified font.
//
// History:
//   11-May-1993    -by-    Kent Settle     (kentse)
//  Broke out into a separate routine.
//--------------------------------------------------------------------

PS_FIX GetPointSize(pdev, pfo, pxform)
PDEVDATA    pdev;
FONTOBJ    *pfo;
XFORM      *pxform;
{
    XFORMOBJ   *pxo;
    ULONG       ulComplex;
    BOOL        bDeviceFont;
    POINTFIX    ptfx;
    POINTL      ptl;
    FIX         fxVector;
    IFIMETRICS *pifi;
    PS_FIX      psfxPointSize;

    // get the Notional to Device transform.  this is needed to
    // determine the point size.

    pxo = FONTOBJ_pxoGetXform(pfo);

    if (pxo == NULL)
    {
        RIP("PSCRIPT!GrabFaceName: pxo == NULL.\n");
        return((PS_FIX)-1);
    }

    ulComplex = XFORMOBJ_iGetXform(pxo, pxform);

    bDeviceFont = (pfo->flFontType & DEVICE_FONTTYPE);

    // determine the notional space point size of the new font.

    if (bDeviceFont)
    {
        // PSCRIPT font's em height is hardcoded to be 1000 (see quryfont.c).

        pdev->cgs.fwdEmHeight = ADOBE_FONT_UNITS;
    }
    else
    {
        // If its not a device font, we'll have to call back and ask.

        if (!(pifi = FONTOBJ_pifi(pfo)))
        {
            RIP("PSCRIPT!SelectFont: pifi failed.\n");
            return((PS_FIX)-1);
        }

        pdev->cgs.fwdEmHeight = pifi->fwdUnitsPerEm;
    }

    // apply the notional to device transform.

    ptl.x = 0;
    ptl.y = pdev->cgs.fwdEmHeight;

    XFORMOBJ_bApplyXform(pxo, XF_LTOFX, 1, &ptl, &ptfx);

    // now get the length of the vector.

    fxVector = iHipot(ptfx.x, ptfx.y);

    // make it a PS_FIX 24.8 number.

    fxVector <<= 4;

    psfxPointSize = (PS_FIX)(MulDiv(fxVector, PS_RESOLUTION,
                                   pdev->psdm.dm.dmPrintQuality));

    return(psfxPointSize);
}

//--------------------------------------------------------------------
// BOOL DownloadANSIBitmapFont(pdev, pfo, phglyphs)
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// HGLYPH     *phglyphs;
//
// This routine downloads the font definition for the given bitmap font,
// if it has not already been done.  The font is downloaded as an
// Adobe Type 3 font.
//
// This routine return TRUE if the font is successfully, or has already
// been, downloaded to the printer.  It returns FALSE if it fails.
//
// History:
//   13-May-1993    -by-    Kent Settle     (kentse)
//  Wrote it.
//--------------------------------------------------------------------

BOOL DownloadANSIBitmapFont(pdev, pfo, phglyphs)
PDEVDATA    pdev;
FONTOBJ    *pfo;
HGLYPH     *phglyphs;
{
    DLFONT     *pDLFont;
    DWORD       i;
    DWORD       cDownloadedFonts;
    DWORD       cTmp;
    HGLYPH     *phg;
    GLYPHDATA  *pglyphdata;
    POINTL      ptlTL, ptlBR, ptl1;
    PIFIMETRICS pifi;
    CHAR        szFaceName[MAX_STRING];
    PSZ         pszFaceName;
    PWSTR       pwstr;
    LONG        EmHeight;
    XFORMOBJ   *pxo;
    POINTFIX    ptfx;
    PS_FIX      psfxPointSize;
    XFORM       fontxform;
    HGLYPH      hgDefault;

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

    PrintString(pdev, "9 dict dup begin\n");

    // set FontType to 3 indicating user defined font.

    PrintString(pdev, "/FontType 3 def\n");

    // get a pointer to our array of HGLYPHS.  remember to skip over the
    // first two WORDS.

    phg = phglyphs + 1;

//!!! for now - assuming first hglyph is the default one.

    hgDefault = *phg;

    // run through the array, looking at the bounding box for each
    // glyph, in order to create the bounding box for the entire
    // font.

    ptlTL.x = ADOBE_FONT_UNITS;
    ptlTL.y = ADOBE_FONT_UNITS;
    ptlBR.x = 0;
    ptlBR.y = 0;

    for (i = 0; i < 256; i++)
    {
        // get the GLYPHDATA structure for each glyph.

        if (!(cTmp = FONTOBJ_cGetGlyphs(pfo, FO_GLYPHBITS, 1, phg, (PVOID *)&pglyphdata)))
        {
            RIP("PSCRIPT!DownloadANSIBitmapFont: cGetGlyphs failed.\n");
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
        RIP("PSCRIPT!DownloadANSIBitmapFont: pifi failed.\n");
        return(FALSE);
    }

    // get the Notional to Device transform.  this is needed to
    // determine the point size.

    pxo = FONTOBJ_pxoGetXform(pfo);

    if (pxo == NULL)
    {
        RIP("PSCRIPT!DownloadANSIBitmapFont: pxo == NULL.\n");
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
                      sizeof(HGLYPH) * 256)))
    {
        RIP("PSCRIPT!DownloadANSIBitmapFont: HeapAlloc for phgVector failed.\n");
        return(FALSE);
    }

    // reset pointer to our array of HGLYPHS.  remember to skip over the
    // first two WORDS.

    phg = phglyphs + 1;

    // fill in the HGLYPH encoding vector, and output
    // the encoding vector to the printer.  leave the first position for
    // the .notdef character.

    pDLFont->cGlyphs = 256;
    memcpy(pDLFont->phgVector, phg, 256 * sizeof(HGLYPH));

    PrintString(pdev, "Encoding\n");

    for (i = 1; i < 256; i++)
    {
        PrintString(pdev, "dup ");
        PrintDecimal(pdev, 1, i);
        PrintString(pdev, " /c");
        PrintDecimal(pdev, 1, i);
        PrintString(pdev, " put\n");
    }

    // under level 1 of PostScript, the 'BuildChar' procedure is called
    // every time a character from the font is constructed.  under
    // level 2, 'BuildGlyph' is called instead.  therefore, we will
    // define a 'BuildChar' procedure, which basically calls
    // 'BuildGlyph'.  this will provide us support for both level 1
    // and level 2 of PostScript.

    // define the 'BuildGlyph' procedure.  start by getting the
    // character name and the font dictionary from the stack.

    PrintString(pdev, "/BuildGlyph {0 begin /cn exch def /fd exch def\n");

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

    PrintString(pdev, "[1 0 0 -1 0 0] dup 4 CI 7 get put dup 5 CI 8 get put\n");

    // get hex string bitmap, convert into procedure, then print
    // the bitmap image.

    PrintString(pdev, "CI 9 1 getinterval cvx imagemask end } def\n");

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

    PrintString(pdev, "/CD 256 dict def CD begin\n");

    // send out the definition of the default (.notdef) character.

    if (!SendCharBitmap(pdev, pfo, *phg++, 0, TRUE))
    {
        RIP("PSCRIPT!DownloadANSIBitmapFont: SendCharBitmap failed.\n");
        HeapFree(pdev->hheap, 0, (PVOID)pDLFont->phgVector);
        return(FALSE);
    }

    for (i = 1; i < 256; i++)
    {
        // don't send out duplicates of the .notdef definition.

        if (*phg != hgDefault)
        {
            if (!SendCharBitmap(pdev, pfo, *phg, i, FALSE))
            {
                RIP("PSCRIPT!DownloadANSIBitmapFont: SendCharBitmap failed.\n");
                HeapFree(pdev->hheap, 0, (PVOID)pDLFont->phgVector);
                return(FALSE);
            }
        }

        // point to the next HGLYPH.

        phg++;
    }

    // close the definition of the font.

    PrintString(pdev, "end\n");

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
}


//--------------------------------------------------------------------
// BOOL SendCharBitmap(pdev, pfo, hglyph, index, bnotdef);
// PDEVDATA    pdev;
// FONTOBJ    *pfo;
// HGLYPH      hglyph;
// DWORD       index;
// BOOL        bnotdef;
//
// This routine downloads the character bitmap definition to the printer.
//
// This routine return TRUE for success, FALSE otherwise.
//
// History:
//   13-May-1993    -by-    Kent Settle     (kentse)
//  Broke out of DownloadANSIBitmapFont.
//--------------------------------------------------------------------

BOOL SendCharBitmap(pdev, pfo, hglyph, index, bnotdef)
PDEVDATA    pdev;
FONTOBJ    *pfo;
HGLYPH      hglyph;
DWORD       index;
BOOL        bnotdef;
{
    DWORD       cTmp;
    GLYPHDATA  *pglyphdata;
    PS_FIX      psfxXtrans, psfxYtrans;
    BYTE       *pjBits;
    DWORD       j;
    LONG        cjWidth;

    // get the GLYPHDATA structure for each glyph.

    if (!(cTmp = FONTOBJ_cGetGlyphs(pfo, FO_GLYPHBITS, 1, &hglyph,
                                    (PVOID *)&pglyphdata)))
    {
        RIP("PSCRIPT!SendCharBitmap: cGetGlyphs failed.\n");
        return(FALSE);
    }

    // the first number in the character description is the width
    // in 1 unit font space.  the next four numbers are the bounding
    // box in 1 unit font space.  the next two numbers are the width
    // and height of the bitmap.  the next two numbers are the x and
    // y translation values for the matrix given to imagemask.
    // this is followed by the bitmap itself.

    // first, output the character name.

    if (bnotdef)
        PrintString(pdev, "/.notdef");
    else
    {
        PrintString(pdev, "/c");
        PrintDecimal(pdev, 1, index);
    }

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
    // with descenders.  both components have .5 subtracted from the original
    // values to avoid roundoff errors.

    psfxXtrans = ETOPSFX((FLOAT)FXTOLROUND(-pglyphdata->fxA) - 0.5);
    psfxYtrans = ETOPSFX(-(FLOAT)pglyphdata->gdf.pgb->ptlOrigin.y - 0.5);

    PrintPSFIX(pdev, 2, psfxXtrans, psfxYtrans);
    PrintString(pdev, "\n<");

    // now output the bits.  calculate how many bytes each source scanline
    // contains.  remember that the bitmap will be padded to 32bit bounds.

    // protect ourselves.

    if ((pglyphdata->gdf.pgb->sizlBitmap.cx < 1) ||
        (pglyphdata->gdf.pgb->sizlBitmap.cy < 1))
    {
        RIP("PSCRIPT!SendCharBitmap: Invalid glyphdata!!!.\n");
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

    return(TRUE);
}
