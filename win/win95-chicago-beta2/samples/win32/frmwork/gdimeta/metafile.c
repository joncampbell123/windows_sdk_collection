// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: metafile.c
//
//  PURPOSE: MetaFile manipulation functions.
//
//  FUNCTIONS:
//    PictureWidth - Finds the width of an enhanced metafile (EMF)
//    PictureHeight - Finds the height of an EMF
//    LoadMetaFile - Loads any Windows type metafile (returns enhanced format)
//    APMChecksum  - Calculate the checksum for a placeable metafile
//    LoadPlaceableMetaFile - Converts a placeable metafile to enhanced format
//    SavePlaceableMetaFile - Saves an enhanced metafile in placeable format
//    CreateMetaPalette - Creates a GDI palette from an EMF color table
//    GetDCFromEMF - Creates a new metafile DC and copies existing EMF to it
//
//  COMMENTS:
//


#include <windows.h>            // required for all Windows applications
#include <windowsx.h>
#include <commctrl.h>
#include "globals.h"            // prototypes specific to this application
#include <stdlib.h>             // for _splitpath
#include "metafile.h"           // prototypes specific to this module
#include "palette.h"            // for PALVERSION


//
//  FUNCTION: PictureWidth(HENHMETAFILE)
//
//  PURPOSE: Calculates the width in pixels of an enhanced metafile.
//
//  PARAMETERS:
//    hemf - The metafile
//
//  RETURN VALUE:
//    The width of the metafile, or 0 if not successful.
//
//  COMMENTS:
//    The width of the metafiles rclFrame rect (.01mm units) is converted
//    to pixels using the szlDevice and szlMillimeters members.
//

UINT PictureWidth(HENHMETAFILE hemf)
{
    ENHMETAHEADER emh;

    if (!hemf)
        return 0;

    if (GetEnhMetaFileHeader(hemf, sizeof(emh), &emh))
        return MulDiv(RECTWIDTH(&emh.rclFrame),
                      emh.szlDevice.cx,
                      emh.szlMillimeters.cx * 100);
    else
        return 0;
}


//
//  FUNCTION: PictureHeight(HENHMETAFILE)
//
//  PURPOSE: Calculates the height in pixels of an enhanced metafile.
//
//  PARAMETERS:
//    hemf - The metafile
//
//  RETURN VALUE:
//    The height of the metafile, or 0 if not successful.
//
//  COMMENTS:
//    The height of the metafiles rclFrame rect (.01mm units) is converted
//    to pixels using the szlDevice and szlMillimeters members.
//

UINT PictureHeight(HENHMETAFILE hemf)
{
    ENHMETAHEADER emh;

    if (!hemf)
        return 0;

    if (GetEnhMetaFileHeader(hemf, sizeof(emh), &emh))
        return MulDiv(RECTHEIGHT(&emh.rclFrame),
                      emh.szlDevice.cy,
                      emh.szlMillimeters.cy * 100);
    else
        return 0;
}

//
//  FUNCTION: LoadMetaFile(LPCSTR)
//
//  PURPOSE: Loads an Enhanced, Placeable or standard Windows MetaFile
//    from the given file, and converts it to enhanced format if necessary.
//
//  PARAMETERS:
//    lpszFile  - The file to load
//
//  RETURN VALUE:
//    The handle to an Enhanced MetaFile, or NULL if not successful.
//
//  COMMENTS:
//
//

HENHMETAFILE LoadMetaFile(LPCSTR lpszFile)
{
    HENHMETAFILE    hemf = NULL;
    FILEMAP         fMap;


    if (!MapFileReadOnly(lpszFile, NULL, &fMap))
    {
        OutputDebugString("LMF: MapFileReadOnly failed\r\n");
        return NULL;
    }

    //
    // First check to see if it is an enhanced metafile
    //
    if (((LPENHMETAHEADER)fMap.pFileMap)->dSignature == ENHMETA_SIGNATURE)
    {
        OutputDebugString("LMF: Loading Enhanced metafile\r\n");

        hemf = GetEnhMetaFile(lpszFile);

        if (!hemf)
            OutputDebugString("LMF: GetEnhMetaFile failed\r\n");
    }

    //
    // Not an enhanced metafile... check for Aldus Placeable Metafile (APM)
    //
    else if (*((LPDWORD)fMap.pFileMap) == APM_SIGNATURE)
    {
        OutputDebugString("LMF: Loading Placeable metafile\r\n");

        hemf = LoadPlaceableMetaFile((PAPMFILEHEADER)fMap.pFileMap);

        if (!hemf)
            OutputDebugString("LMF: LoadPlaceableMetaFile failed\r\n");
    }

    //
    // Not enhanced or placeable... must be a Windows 3x metafile (we hope)
    //
    else
    {
        OutputDebugString("LMF: Loading Windows metafile\r\n");

        hemf = SetWinMetaFileBits(GetFileSize(fMap.hFile, NULL), //uiSize,
                                  (PBYTE)fMap.pFileMap,
                                  NULL,
                                  NULL);

        if (!hemf)
            OutputDebugString("LMF: SetWinMetaFileBits failed\r\n");
    }

    CloseFileMapping(&fMap);
    return hemf;
}


//
//  FUNCTION: APMChecksum(PAPMFILEHEADER)
//
//  PURPOSE: Calculates the checksum for an Aldus Placeable Metafile (APM).
//
//  PARAMETERS:
//    papm - the placeable header to calculate the checksum for.
//
//  RETURN VALUE:
//    The checksum for the header.
//
//  COMMENTS:
//    The checksum is calculated by XOR-ing together the first 10 WORD's
//    of the header.
//
//

WORD APMChecksum(PAPMFILEHEADER papm)
{
    PWORD pw = (LPWORD)papm;
    WORD  wSum = 0;
    int   i;

    // The checksum in a Placeable Metafile header is calculated
    // by XOR-ing the first 10 words of the header.

    for (i = 0; i < 10; i++)
        wSum ^= *pw++;

    return wSum;
}


//
//  FUNCTION: LoadPlaceableMetaFile(PAPMFILEHEADER)
//
//  PURPOSE: Converts a Placeable MetaFile into Enhanced MetaFile format.
//
//  PARAMETERS:
//    papm - pointer to the placeable metafile to convert.
//
//  RETURN VALUE:
//    The handle to an Enhanced MetaFile, or NULL if not successful.
//
//  COMMENTS:
//
//

HENHMETAFILE LoadPlaceableMetaFile(PAPMFILEHEADER papm)
{
    PMETAHEADER  pmf = (PMETAHEADER)(papm + 1);     // Windows MetaFile
    METAFILEPICT mfp;
    HENHMETAFILE hemf;
    HDC          hdc;

#ifdef _DEBUG
    if (APMChecksum(papm) != papm->checksum)
        OutputDebugString("LPM: Placeable metafile checksum mismatch\r\n");
#endif

    //
    // Set up a METAFILEPICT with MM_ANISOTROPIC (requires xExt and
    // yExt to be in MM_HIMETRIC units).
    //
    // Note: papm->bbox   = bounding rect in metafile units
    //       papm->inch   = # of metafile units per inch
    //       HIMETRICINCH = # of hi-metric units (.01mm) per inch
    //
    mfp.mm   = MM_ANISOTROPIC;
    mfp.xExt = MulDiv(papm->bbox.right - papm->bbox.left,
                      HIMETRICINCH,
                      papm->inch);
    mfp.yExt = MulDiv(papm->bbox.bottom - papm->bbox.top,
                      HIMETRICINCH,
                      papm->inch);
    mfp.hMF  = NULL;


    // Reference DC for SetWinMetaFileBits
    hdc = GetDC(NULL);

    //
    // Finally, convert it to an enhanced metafile.
    //
    hemf = SetWinMetaFileBits(pmf->mtSize * 2, (PBYTE)pmf, hdc, &mfp);

    ReleaseDC(NULL, hdc);
    return hemf;
}


//
//  FUNCTION: SavePlaceableMetaFile(HENHMETAFILE, LPCSTR)
//
//  PURPOSE: Saves an Enhanced MetaFile in Placeable MetaFile format.
//
//  PARAMETERS:
//    hemf     - handle of the enhanced metafile to save.
//    lpszFile - name of file to save to.
//
//  RETURN VALUE:
//    TRUE if successful, FALSE otherwise.
//
//  COMMENTS:
//    Uses memory mapped files to save the metafile.
//
//

BOOL SavePlaceableMetaFile(HENHMETAFILE hemf, LPCSTR lpszFile)
{
    FILEMAP         fMap;
    ENHMETAHEADER   emh;
    PAPMFILEHEADER  papm;
    HDC             hdc;
    UINT            uiSize;
    BOOL            bRet = FALSE;
    int             nXdpi, nYdpi;


    if (!lpszFile || !*lpszFile)
    {
        OutputDebugString("SPM: No Filename specified\r\n");
        return FALSE;
    }

    // Get information about the metafile
    uiSize = GetEnhMetaFileHeader(hemf, sizeof(emh), &emh);
    if (0 == uiSize)
    {
        OutputDebugString("SPM: GetEnhMetaFileHeader failed\r\n");
        return FALSE;
    }

    // Reference DC for GetWinMetaFileBits
    hdc = GetDC(NULL);

    // Get the size of the metafile in Windows format
    uiSize = GetWinMetaFileBits(hemf, 0, NULL, MM_ANISOTROPIC, hdc);
    if (0 == uiSize)
    {
        OutputDebugString("SPM: GetWinMetaFileBits failed\r\n");

        ReleaseDC(NULL, hdc);
        return FALSE;
    }

    // Create a file mapping to write to
    if (!MapFileReadWrite(lpszFile,
                      NULL,
                      &fMap,
                      CREATE_ALWAYS,
                      sizeof(APMFILEHEADER) + uiSize))
    {
        OutputDebugString("SPM: MapFileReadWrite failed\r\n");

        ReleaseDC(NULL, hdc);
        return FALSE;
    }


    // Fill out the placeable metafile header

    nXdpi = GetDeviceCaps(hdc, LOGPIXELSX);
    nYdpi = GetDeviceCaps(hdc, LOGPIXELSY);
    papm  = (PAPMFILEHEADER)fMap.pFileMap;

    papm->key         = APM_SIGNATURE;
    papm->hmf         = 0;
    papm->inch        = 1000;   // somewhat arbitrary
    papm->bbox.left   = (short) MulDiv(emh.rclBounds.left,   papm->inch, nXdpi);
    papm->bbox.top    = (short) MulDiv(emh.rclBounds.top,    papm->inch, nYdpi);
    papm->bbox.right  = (short) MulDiv(emh.rclBounds.right,  papm->inch, nXdpi);
    papm->bbox.bottom = (short) MulDiv(emh.rclBounds.bottom, papm->inch, nYdpi);
    papm->reserved    = 0;
    papm->checksum    = APMChecksum(papm);


    // Have the system convert the metafile to windows format and store
    // it directly in the file.

    bRet = (uiSize == GetWinMetaFileBits(hemf,
                                         uiSize,
                                         (LPBYTE)(papm + 1),
                                         MM_ANISOTROPIC,
                                         hdc));

    ReleaseDC(NULL, hdc);       // clean up
    CloseFileMapping(&fMap);
    if (!bRet)
    {
        OutputDebugString("SPM: GetWinMetaFileBits failed\r\n");

        DeleteFile(lpszFile);
    }

    return bRet;
}

//
//  FUNCTION: CreateMetaPalette(HENHMETAFILE)
//
//  PURPOSE: Creates a logical palette from an enhanced metafile's
//    color table.
//
//  PARAMETERS:
//    hemf - handle of the enhanced metafile
//
//  RETURN VALUE:
//    The logical palette handle, or NULL if the metafile does not have
//    a color table.
//
//  COMMENTS:
//
//

HPALETTE CreateMetaPalette(HENHMETAFILE hemf)
{
    HPALETTE hPal = NULL;
    UINT nEntries;

    nEntries = GetEnhMetaFilePaletteEntries(hemf, 0, NULL);
    if (nEntries != GDI_ERROR && nEntries != 0)
    {
        PLOGPALETTE plogPal;

        plogPal = (PLOGPALETTE)GlobalAllocPtr(GPTR,
                        2 * sizeof(WORD) + sizeof(PALETTEENTRY) * nEntries);

        if (!plogPal)
        {
            OutputDebugString("CMP: Unable to allocate memory for LOGPALETTE\r\n");
            return NULL;
        }

        plogPal->palVersion = (WORD) PALVERSION;
        plogPal->palNumEntries = (WORD) nEntries;
        if (nEntries == GetEnhMetaFilePaletteEntries(hemf,
                                                     nEntries,
                                                     plogPal->palPalEntry))
            hPal = CreatePalette(plogPal);

        else
            OutputDebugString("CMP: GetEnhMetaFilePaletteEntries failed\r\n");

        GlobalFreePtr(plogPal);
    }

    return hPal;
}


//
//  FUNCTION: GetDCFromEMF(HWND, HENHMETAFILE, HPALETTE)
//
//  PURPOSE: Creates a new enhanced metafile dc and optionally
//    copies an existing metafile into the new dc.
//
//  PARAMETERS:
//    hwnd - window used for reference
//    hemf - existing metafile (or NULL)
//    hpal - palette to use for new metafile (or NULL)
//
//  RETURN VALUE:
//    The new metafile DC, or NULL if error
//
//  COMMENTS:
//    Always uses the screen as the reference dc for the new
//    metafile.
//
//

HDC GetDCFromEMF(HWND hwnd, HENHMETAFILE hemf, HPALETTE hpal)
{
    HDC     hdc;
    RECT    rc = {0, 0, 500, 500};
    TCHAR   szDesc[256];
    TCHAR   szTitle[256] = "";
    HCURSOR hcurSave = NULL;
    ENHMETAHEADER emh;


    // If we are copying an existing metafile, use its dimensions and title
    // for the new metafile.
    if (hemf)
    {
        // Copying the old metafile (later) can be slow, so put up hourglass
        hcurSave = SetCursor(hcursHourGlass);

        if (GetEnhMetaFileHeader(hemf, sizeof(emh), &emh))
        {
            // Get the rclFrame rect from the header (HiMetric units)
            CopyRect(&rc, (LPRECT)&emh.rclFrame);

            // Get the title from the metafile's description string
            GetEnhMetaFileDescription(hemf, sizeof(szDesc), szDesc);
            lstrcpy(szTitle, szDesc + lstrlen(szDesc) + 1);
        }
        else    // Must be an invalid metafile (or something drastic)
        {
            OutputDebugString("GetDCFromEMF: GetEnhMetaFileHeader failed!\r\n");
            SetCursor(hcurSave);
            return NULL;
        }
    }

    // If there's no existing metafile, try the window dimensions.
    else
    {
        if (hwnd)
        {
            GetClientRect(hwnd, &rc);
            rc.right--;       // EMF interprets right/bottom as being included
            rc.bottom--;
        }
        // else use defaults

        // Convert pixels to MM_HIMETRIC units for CreateEnhMetaFile

        hdc = GetDC(hwnd); // NULL hwnd is OK here
        rc.right  = MulDiv(rc.right,
                           HIMETRICINCH,
                           GetDeviceCaps(hdc, LOGPIXELSX));
        rc.bottom = MulDiv(rc.bottom,
                           HIMETRICINCH,
                           GetDeviceCaps(hdc, LOGPIXELSY));
        ReleaseDC(hwnd, hdc);
    }

    // If we don't have a title yet, use the filename. If we don't have a
    // filename, go with an empty title for now.
    if (szTitle[0] == 0 && GetFName()[0] != 0)
        _splitpath(GetFName(), NULL, NULL, szTitle, NULL);

    // New description = our app name + title
    wsprintf(szDesc, "%s\0%s\0\0", szAppName, szTitle);


    // Create the metafile DC (use screen for reference)
    hdc = CreateEnhMetaFile(NULL, NULL, &rc, szDesc);

    if (hdc)
    {
        // Use text mode to avoid rounding errors
        SetMapMode(hdc, MM_TEXT);

        // Set the metafile's color table
        if (hpal)
        {
            SelectPalette(hdc, hpal, FALSE);
            RealizePalette(hdc);
        }

        // Copy old metafile into new one
        if (hemf)
        {
            // Convert rect to logical units (pixels here) for PlayEnhMetaFile
            rc.left   = MulDiv(rc.left,
                               emh.szlDevice.cx,
                               emh.szlMillimeters.cx * 100);
            rc.top    = MulDiv(rc.top,
                               emh.szlDevice.cy,
                               emh.szlMillimeters.cy * 100);
            rc.right  = MulDiv(rc.right,
                               emh.szlDevice.cx,
                               emh.szlMillimeters.cx * 100);
            rc.bottom = MulDiv(rc.bottom,
                               emh.szlDevice.cy,
                               emh.szlMillimeters.cy * 100);

            PlayEnhMetaFile(hdc, hemf, &rc);
        }
    }

    // Restore original cursor
    if (hcurSave)
        SetCursor(hcurSave);

    return hdc;
}
