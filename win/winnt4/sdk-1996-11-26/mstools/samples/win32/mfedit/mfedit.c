/******************************Module*Header*******************************\
* Module Name: mfedit.c
*
* Main module for the Enhanced Metafile Editor
*       contains everything
*
* Copyright (c) 1992-1996 Microsoft Corporation
*
* The Enhanced Metafile Editor serves to demonstrate the enhanced metafile
* APIs in Windows NT.
*
* The Editor provides the following functions:
*       1.  Playback and recording of GDI calls
*       2.  Embedding bitmap and enhanced metafile into another enhanced
*           metafile with transformation
*       3.  Hit-testing against enhanced metafile records
*       4.  Random access playback
*       5.  Playback metafile records one-by-one
*       6.  Selective recording of existing enhanced metafile records into
*           a new enhanced metafile
*       7.  drawing with pen, text, bezier, line, ellipse, rectangle and
*           embedding bitmap and enhanced metafile tools
*
*
\**************************************************************************/
#include "precomp.h"


/******************************Public*Routine******************************\
*
* FUNCTION:  PolyDraw95(HDC, LPPOINT, LPBYTE, int)
*
* PURPOSE:   Draws the points returned from a call to GetPath()
*            to an HDC
*
* NOTES:     This function is similar to the Windows NT PolyDraw
*            function, which draws a set of line segments and Bezier
*            curves. Because PolyDraw is not supported in Windows 95
*            this PolyDraw95 function is used instead.
*
\**************************************************************************/

BOOL PolyDraw95(HDC  hdc,               // handle of a device context
                CONST LPPOINT lppt,     // array of points
                CONST LPBYTE lpbTypes,  // line and curve identifiers
                int  cCount)            // count of points
{
  int i;

  for (i=0; i<cCount; i++)
    switch (lpbTypes[i]) {
      case PT_MOVETO :
         MoveToEx(hdc, lppt[i].x, lppt[i].y, NULL);
         break;

      case PT_LINETO | PT_CLOSEFIGURE:
      case PT_LINETO :
         LineTo(hdc, lppt[i].x, lppt[i].y);
         break;

      case PT_BEZIERTO | PT_CLOSEFIGURE:
      case PT_BEZIERTO :
        PolyBezierTo(hdc, &lppt[i], 3);
       i+=2;
         break;
    }

   return TRUE;
}


/**************************************************************************\
*
* vErrOut
*
* Effects:  Put up a message box with the error string and error code
*           returned.
*
\**************************************************************************/

void vErrOut(char errstring[MAX_ERR_STRING])
{
    DWORD   dwError, dwThrdId;
    char    str[MAX_ERR_STRING];


    if (gbDebug) {
        dwError = GetLastError();
        dwThrdId = GetCurrentThreadId();
        wsprintf((LPSTR) str, "%s, tid = %lx, Error code = %d\n",
                 errstring, dwThrdId, dwError);
        OutputDebugString((LPCTSTR)str);
        //MessageBox(GetFocus(), (LPSTR)str, "MfEdit Error", MB_OK);
    }
}


/**************************************************************************\
*
* FlushPalette
*
\**************************************************************************/

static void
FlushPalette(HDC hdc, int nColors)
{
    LOGPALETTE *pPal;
    HPALETTE hpal, hpalOld;
    int i;

    if (nColors == 256)
    {
        pPal = (LOGPALETTE *) LocalAlloc(LMEM_FIXED|LMEM_ZEROINIT,
                                         sizeof(LOGPALETTE) + (nColors-1) * sizeof(PALETTEENTRY));

        if (pPal)
        {
	    pPal->palVersion = 0x300;
	    pPal->palNumEntries = nColors;

        // Mark everything PC_NOCOLLAPSE and PC_RESERVED to force every thing
        // into the palette.  Colors are already black because we zero initialized
        // during memory allocation.

            for (i = 0; i < nColors; i++)
            {
                pPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE | PC_RESERVED;
            }

            hpal = CreatePalette(pPal);
            LocalFree(pPal);

            hpalOld = SelectPalette(hdc, hpal, FALSE);
            RealizePalette(hdc);

            SelectPalette(hdc, hpalOld, FALSE);
            DeleteObject(hpal);
        }
    }
}


/**************************************************************************\
*
* UpdateStaticMapping
*
\**************************************************************************/

static void
UpdateStaticMapping(PALETTEENTRY *pe332Palette)
{
    HPALETTE hpalStock;
    int iStatic, i332;
    int iMinDist, iDist;
    int iDelta;
    int iMinEntry;
    PALETTEENTRY *peStatic, *pe332;

    hpalStock = GetStockObject(DEFAULT_PALETTE);

    // The system should always have one of these
    //TKASSERT(hpalStock != NULL);
    // Make sure there's the correct number of entries
    //TKASSERT(GetPaletteEntries(hpalStock, 0, 0, NULL) == STATIC_COLORS);

    // Get the current static colors
    GetPaletteEntries(hpalStock, 0, STATIC_COLORS, apeDefaultPalEntry);

    // Zero the flags in the static colors because they are used later
    peStatic = apeDefaultPalEntry;
    for (iStatic = 0; iStatic < STATIC_COLORS; iStatic++)
    {
        peStatic->peFlags = 0;
        peStatic++;
    }

    // Zero the flags in the incoming palette because they are used later
    pe332 = pe332Palette;
    for (i332 = 0; i332 < 256; i332++)
    {
        pe332->peFlags = 0;
        pe332++;
    }

    // Try to match each static color exactly
    // This saves time by avoiding the least-squares match for each
    // exact match
    peStatic = apeDefaultPalEntry;
    for (iStatic = 0; iStatic < STATIC_COLORS; iStatic++)
    {
        pe332 = pe332Palette;
        for (i332 = 0; i332 < 256; i332++)
        {
            if (peStatic->peRed == pe332->peRed &&
                peStatic->peGreen == pe332->peGreen &&
                peStatic->peBlue == pe332->peBlue)
            {
                //TKASSERT(pe332->peFlags != COLOR_USED);
                
                peStatic->peFlags = EXACT_MATCH;
                pe332->peFlags = COLOR_USED;
                aiDefaultOverride[iStatic] = i332;
                
                break;
            }

            pe332++;
        }

        peStatic++;
    }
    
    // Match each static color as closely as possible to an entry
    // in the 332 palette by minimized the square of the distance
    peStatic = apeDefaultPalEntry;
    for (iStatic = 0; iStatic < STATIC_COLORS; iStatic++)
    {
        // Skip colors already matched exactly
        if (peStatic->peFlags == EXACT_MATCH)
        {
            peStatic++;
            continue;
        }
        
        iMinDist = MAX_COL_DIST+1;
#if DBG
        iMinEntry = -1;
#endif

        pe332 = pe332Palette;
        for (i332 = 0; i332 < 256; i332++)
        {
            // Skip colors already used
            if (pe332->peFlags == COLOR_USED)
            {
                pe332++;
                continue;
            }
            
            // Compute Euclidean distance squared
            iDelta = pe332->peRed-peStatic->peRed;
            iDist = iDelta*iDelta;
            iDelta = pe332->peGreen-peStatic->peGreen;
            iDist += iDelta*iDelta;
            iDelta = pe332->peBlue-peStatic->peBlue;
            iDist += iDelta*iDelta;

            if (iDist < iMinDist)
            {
                iMinDist = iDist;
                iMinEntry = i332;
            }

            pe332++;
        }

        //TKASSERT(iMinEntry != -1);

        // Remember the best match
        aiDefaultOverride[iStatic] = iMinEntry;
        pe332Palette[iMinEntry].peFlags = COLOR_USED;
        
        peStatic++;
    }

    // Zero the flags in the static colors because they may have been
    // set.  We want them to be zero so the colors can be remapped
    peStatic = apeDefaultPalEntry;
    for (iStatic = 0; iStatic < STATIC_COLORS; iStatic++)
    {
        peStatic->peFlags = 0;
        peStatic++;
    }

    // Reset the 332 flags because we may have set them
    pe332 = pe332Palette;
    for (i332 = 0; i332 < 256; i332++)
    {
        pe332->peFlags = PC_NOCOLLAPSE;
        pe332++;
    }

#if 0
    for (iStatic = 0; iStatic < STATIC_COLORS; iStatic++)
    {
        PrintMessage("Static color %2d maps to %d\n",
                     iStatic, aiDefaultOverride[iStatic]);
    }
#endif
}

#define SwapPalE(i,j) { \
    PALETTEENTRY palE; \
    palE = pPal->palPalEntry[i]; \
    pPal->palPalEntry[i] = pPal->palPalEntry[j]; \
    pPal->palPalEntry[j] = palE; }


/**************************************************************************\
*
* SaveStaticEntries
*
\**************************************************************************/

VOID SaveStaticEntries(HDC hDC)
{
    int i;

    if (gbSystemColorsInUse) {
        for (i = COLOR_SCROLLBAR; i <= COLOR_BTNHIGHLIGHT; i++)
            gacrSave[i - COLOR_SCROLLBAR] = GetSysColor(i);

        gbStaticSaved = TRUE;
    }
}

/**************************************************************************\
*
* UseStaticEntries
*
\**************************************************************************/

static VOID UseStaticEntries(HDC hdc)
{
    SetSysColors(NUM_STATIC_COLORS, gaiStaticIndex, gacrBlackAndWhite);
    gbSystemColorsInUse = TRUE;

    PostMessage(HWND_BROADCAST, WM_SYSCOLORCHANGE, 0, 0);
}

/**************************************************************************\
*
* hCreateRGBPalette
*
\**************************************************************************/

HPALETTE hCreateRGBPalette(HDC hDC, int index)
{
    PIXELFORMATDESCRIPTOR   pfd;
    PLOGPALETTE             pLogPal, pPal;
    int                     nClr, i, n;
    HPALETTE                hPal;
    BOOL                    bUseStaticClr;


    if (DescribePixelFormat(hDC, index, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0) {
        return (HPALETTE)NULL;
    }

    bUseStaticClr = pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE;

    if (pfd.dwFlags & PFD_NEED_PALETTE) {
        nClr = 1 << pfd.cColorBits;

        if ((pLogPal = (PLOGPALETTE) LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
                        (nClr-1) * sizeof(PALETTEENTRY))) != NULL) {

            pLogPal->palVersion = 0x300;
            pLogPal->palNumEntries = nClr;

            for (i = 0; i < nClr; i++) {
                pLogPal->palPalEntry[i].peRed   =
		    ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
                pLogPal->palPalEntry[i].peGreen =
		    ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
                pLogPal->palPalEntry[i].peBlue  =
		    ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
                pLogPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
            }

            if ( nClr == 256) {
                if (bUseStaticClr) {
                    pLogPal->palPalEntry[0].peFlags = 0;
                    pLogPal->palPalEntry[255].peFlags = 0;

                    SaveStaticEntries(hDC);
                    SetSystemPaletteUse(hDC, SYSPAL_NOSTATIC);
                } else {
                    if ((pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
                        (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
                        (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)) {
                        UpdateStaticMapping(pLogPal->palPalEntry);

                        for (i = 0; i < 20; i++) {
                            pLogPal->palPalEntry[aiDefaultOverride[i]] =
                                apeDefaultPalEntry[i];
                        }

                    }
                }
            }

            FlushPalette(hDC, nClr);

            hPal = CreatePalette(pLogPal);
            LocalFree(pLogPal);
            SelectPalette(hDC, hPal, FALSE);
            RealizePalette(hDC);


            if (bUseStaticClr)
                UseStaticEntries(hDC);

        }
    }
    // set up logical indices for CI mode
    else if( pfd.iPixelType == PFD_TYPE_COLORINDEX ) {
        if (pfd.cColorBits == 4) {

            // for 4-bit, create a logical palette with 16 entries

            n = 16;
            pPal = (PLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
                    (n-1) * sizeof(PALETTEENTRY));
            pPal->palVersion = 0x300;
            pPal->palNumEntries = n;

            for( i = 0; i < 8; i ++) {
                pPal->palPalEntry[i] = apeDefaultPalEntry[i];
            }
            for (i = 8; i < 16; i++) {
                pPal->palPalEntry[i] = apeDefaultPalEntry[i+4];
            }

            // conform expects indices 0..3 to be BLACK,RED,GREEN,BLUE, so
            //  we rearrange the table for now.

            SwapPalE(1,9)
            SwapPalE(2,10)
            SwapPalE(3,12)

        } else if (pfd.cColorBits == 8) {

            // for 8-bit, create a logical palette with 256 entries, making
            // sure that the 20 system colors exist in the palette

            n = 256;
            pPal = (PLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
                    (n-1) * sizeof(PALETTEENTRY));
            pPal->palVersion = 0x300;
            pPal->palNumEntries = n;

            hPal = GetStockObject (DEFAULT_PALETTE);

            // start by copying default palette into new one

            GetPaletteEntries( hPal, 0, 20, pPal->palPalEntry);

            // conform expects indices 0..3 to be BLACK,RED,GREEN,BLUE, so
            // we rearrange the table for now.

            SwapPalE(1,13)
            SwapPalE(2,14)
            SwapPalE(3,16)

            for( i = 20; i < n; i ++) {
                pPal->palPalEntry[i].peRed   = (BYTE) (i - 1);
                pPal->palPalEntry[i].peGreen = (BYTE) (i - 2);
                pPal->palPalEntry[i].peBlue  = (BYTE) (i - 3);
                pPal->palPalEntry[i].peFlags = (BYTE) 0;
            }


            // If we are taking possession of the system colors,
            // must guarantee that 0 and 255 are black and white
            // (respectively), so that they can remap to the
            // remaining two static colors.  All other entries must
            // be marked as PC_NOCOLLAPSE.

            if ( gbUseStaticColors )
            {
                pPal->palPalEntry[0].peRed =
                pPal->palPalEntry[0].peGreen =
                pPal->palPalEntry[0].peBlue = 0x00;

                pPal->palPalEntry[255].peRed =
                pPal->palPalEntry[255].peGreen =
                pPal->palPalEntry[255].peBlue = 0xFF;

                pPal->palPalEntry[0].peFlags =
                pPal->palPalEntry[255].peFlags = 0;

                for ( i = 1 ; i < 255 ; i++ )
                {
                    pPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;

                // This is a workaround for a GDI palette "feature".  If
                // any of the static colors are repeated in the palette,
                // those colors will map to the first occurance.  So, for
                // our case where there are only two static colors (black
                // and white), if a white color appears anywhere in the
                // palette other than in the last  entry, the static white
                // will remap to the first white.  This destroys the nice
                // one-to-one mapping we are trying to achieve.
                //
                // There are two ways to workaround this.  The first is to
                // simply not allow a pure white anywhere but in the last
                // entry.  Such requests are replaced with an attenuated
                // white of (0xFE, 0xFE, 0xFE).
                //
                // The other way is to mark these extra whites with
                // PC_RESERVED which will cause GDI to skip these entries
                // when mapping colors.  This way the app gets the actual
                // colors requested, but can have side effects on other
                // apps.

                    if ( pPal->palPalEntry[i].peRed   == 0xFF &&
                         pPal->palPalEntry[i].peGreen == 0xFF &&
                         pPal->palPalEntry[i].peBlue  == 0xFF )
                    {
                        pPal->palPalEntry[i].peFlags |= PC_RESERVED;
                    }
                }

                SaveStaticEntries(hDC);
                SetSystemPaletteUse(hDC, SYSPAL_NOSTATIC);
            }

        } else {
            // for pixel formats > 8 bits deep, create a logical palette with
            // 4096 entries

            n = 4096;
            pPal = (PLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
                    (n-1) * sizeof(PALETTEENTRY));
            pPal->palVersion = 0x300;
            pPal->palNumEntries = n;

            for( i = 0; i < n; i ++) {
                pPal->palPalEntry[i].peRed   = (BYTE) ((i & 0x000f) << 4);
                pPal->palPalEntry[i].peGreen = (BYTE) (i & 0x00f0);
                pPal->palPalEntry[i].peBlue  = (BYTE) ((i & 0x0f00) >> 4);
                pPal->palPalEntry[i].peFlags = (BYTE) 0;
            }

            // conform expects indices 0..3 to be BLACK,RED,GREEN,BLUE, so
            //  we rearrange the table for now.

            SwapPalE(1,0xf)
            SwapPalE(2,0xf0)
            SwapPalE(3,0xf00)
        }

        hPal = CreatePalette(pPal);
        LocalFree(pPal);


        FlushPalette(hDC, n);

        SelectPalette(hDC, hPal, FALSE);
        n = RealizePalette(hDC);

        if ( gbUseStaticColors )
            UseStaticEntries(hDC);
    }

    return hPal;
}

/******************************Public*Routine******************************\
*
* bSetupRC
*
* Effects: Sets up the OpenGL rendering context
*
* Warnings:
*
\**************************************************************************/

#ifdef OPENGL_EMF
BOOL bSetupRC(HDC hDC, PIXELFORMATDESCRIPTOR *ppfdIn)
{
    PIXELFORMATDESCRIPTOR   pfd, *ppfd;
    INT                     iPfmt;
    BOOL                    bRet=FALSE;
    HPALETTE                hPal=(HPALETTE)NULL;
    HGLRC                   hRC;
    char                    text[128];

    if ((ppfdIn != (PIXELFORMATDESCRIPTOR*)NULL) && gbUseMfPFD) {
        ppfd = ppfdIn;
    } else {
        ppfd = &pfd;
        pfd.nSize   = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion= 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
        pfd.iPixelType  = PFD_TYPE_RGBA;
        pfd.cColorBits  = 24;
        pfd.cRedBits    =
        pfd.cRedShift   =
        pfd.cGreenBits  =
        pfd.cGreenShift =
        pfd.cBlueBits   =
        pfd.cBlueShift  =
        pfd.cAlphaBits  =
        pfd.cAlphaShift = 0;
        pfd.cAccumBits  = 24;
        pfd.cAccumRedBits   =
        pfd.cAccumGreenBits =
        pfd.cAccumBlueBits  =
        pfd.cAccumAlphaBits = 0;
        pfd.cDepthBits  = 24;
        pfd.cStencilBits = 24;
        pfd.cAuxBuffers =
        pfd.iLayerType  =
        pfd.bReserved   = 0;
        pfd.dwLayerMask = PFD_MAIN_PLANE;
        pfd.dwVisibleMask = 0;
        pfd.dwDamageMask = 0;
    }

    if ((iPfmt = ChoosePixelFormat(hDC, ppfd)) != 0) {
        bRet = SetPixelFormat(hDC, iPfmt, ppfd);

        gbDB = (ppfd->dwFlags & PFD_DOUBLEBUFFER) ? TRUE : FALSE;

        if ((hPal = hCreateRGBPalette(hDC, iPfmt)) != (HPALETTE)NULL) {
            SelectPalette(hDC, hPal, FALSE);
            RealizePalette(hDC);
            bRet = bRet && TRUE;
        } else {
            bRet = FALSE;
        }

        hRC = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hRC);

        wsprintf(text, "bSetupRC: hDC=%lx, hRC=%lx", hDC, hRC);
        vErrOut(text);

    }
    return bRet;
}

/**************************************************************************\
*
* bSetRC2MatchEmfRC
*
* Effects:  If the bOpenGL flag is not set in the metafile header,
*           return FALSE.
*
*           If the bOpenGL flag is set, retrieve the pfd from the enhanced
*           metafile, if exists.  Set the pixel format with the retrieved
*           pfd on the hDC.  If the pfd is not recorded, let bSetupRC pick
*           the pfd.
*
*           If ppfd is not null, copy the pfd in the metafile record,
*           to the ppfd.  If ppfd->nSize == 0, that means the pfd is not
*           found in the metafile record.
*
* Warnings:
*
* History:
*  05-May-1996
* Wrote it.
\**************************************************************************/

BOOL bSetRC2MatchEmfRC(HDC                      hDC,
                       ENHMETAHEADER            EnhMetaHdr,
                       HENHMETAFILE             hEnhMf,
                       PIXELFORMATDESCRIPTOR    *ppfdIn)
{
    BOOL                    bHasPFD = FALSE, bRCSet = FALSE;
    char                    text[128];
    PIXELFORMATDESCRIPTOR   pfd;

    if (EnhMetaHdr.bOpenGL) {

        if (gpfnGetEnhMetaFilePixelFormat != (PROC)NULL) {

            bHasPFD = ((gpfnGetEnhMetaFilePixelFormat)(hEnhMf,
                                      sizeof(PIXELFORMATDESCRIPTOR),
                                      &pfd) == 0) ? FALSE : TRUE;

            bRCSet = bSetupRC(hDC, (bHasPFD ? &pfd : NULL));

            wsprintf(text,
                     "bSetRC2MatchEmfRC: bRCSet=%ld, bHasPFD=%ld",
                     bRCSet, bHasPFD);
            vErrOut(text);

        } else {
            vErrOut("bSetRC2MatchEmfRC: gpfnGetEnhMetaFilePixelFormat is NULL");
        }

    } else {
        vErrOut("bSetRC2MatchEmfRC: EnhMetaHdr.bOpenGL is FALSE");
    }

    if (ppfdIn != (PIXELFORMATDESCRIPTOR*)NULL) {
        if (bHasPFD) {
            memcpy(ppfdIn, &pfd, sizeof(PIXELFORMATDESCRIPTOR));
        } else {
            ppfdIn->nSize = 0;
        }
    }

    return bRCSet;
}


/******************************Public*Routine******************************\
*
* bCleanUpRC
*
* Effects:  Cleans up the OpenGL rendering context
*
* Warnings:
*
\**************************************************************************/

BOOL bCleanUpRC(VOID)
{
    HGLRC       hRC;
    HDC         hDC;
    char        text[128];

    hRC = wglGetCurrentContext();
    hDC = wglGetCurrentDC();

    wsprintf(text, "bCleanUpRC: hDC=%lx, hRC=%lx", hDC, hRC);
    vErrOut(text);

    wglMakeCurrent(hDC, NULL);
    wglDeleteContext(hRC);
    return TRUE;

}

#else
BOOL bSetupRC(HDC hDC, PIXELFORMATDESCRIPTOR *ppfdIn)
{
    return TRUE;
}

BOOL bSetRC2MatchEmfRC(HDC                      hDC,
                       ENHMETAHEADER            EnhMetaHdr,
                       HENHMETAFILE             hEnhMf,
                       PIXELFORMATDESCRIPTOR    *ppfd)
{
    return TRUE;
}

BOOL bCleanUpRC(VOID)
{
    return TRUE;
}
#endif

/**************************************************************************\
*
* WinMain
*
* Effects:
*
* Warnings:
*
* History:
*  06-May-1996  Retrieve the proc addr for GetEnhMetaFilePixelFormat.
* Wrote it.
\**************************************************************************/

int WINAPI WinMain(
           HINSTANCE hInstance,
           HINSTANCE hPrevInstance,
           LPSTR lpCmdLine,
           int nShowCmd)
{
    MSG      msg;
    HANDLE   hAccel;
    HMODULE  hMod;

    ghModule = GetModuleHandle(NULL);

//
// GDI32 may not support the OpenGL enhanced metafile (Win95 doesn't), in
// which case, the entry point doesn't exist, so we will do a GetProcAddress
// instead.
//
    hMod = LoadLibrary("GDI32.DLL");
    if (hMod != (HMODULE)NULL) {

        gpfnGetEnhMetaFilePixelFormat =
                GetProcAddress(hMod, "GetEnhMetaFilePixelFormat");

        if (gpfnGetEnhMetaFilePixelFormat == (PROC)NULL)
            vErrOut("Main: GetProcAddress(GetEnhMetaFilePixelFormat) failed");

    } else {
        vErrOut("Main: LoadLibrary(GDI32) failed");
    }

    if (!InitializeApp()) {
        MessageBox(ghwndMain,
            GetStringRes(IDS_INITAPPFAIL), GetStringRes(IDS_ERROR), MB_OK);
        return 0;
    }

    if (!(hAccel = LoadAccelerators (ghModule, MAKEINTRESOURCE(ACCEL_ID))))
        MessageBox(ghwndMain,
            GetStringRes(IDS_LOADACCFAIL), GetStringRes(IDS_ERROR), MB_OK);


    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator( ghwndMain, hAccel, &msg) ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 1;

    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
}


/***************************************************************************\
* InitializeApp
*
\***************************************************************************/

BOOL InitializeApp(void)
{
    WNDCLASS wc;
    int index;
    HDC hDC;

    wc.style            = CS_DBLCLKS;
    wc.lpfnWndProc      = (WNDPROC)MainWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra	= sizeof(DWORD);
    wc.hInstance        = ghModule;
    wc.hIcon            = LoadIcon(ghModule, MAKEINTRESOURCE(APP_ICON));
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)(COLOR_APPWORKSPACE + 1);
    wc.lpszMenuName     = "MainMenu";
    wc.lpszClassName	= "MetafDemoClass";

    if (!RegisterClass(&wc))
	return FALSE;

    wc.style            = CS_OWNDC | CS_SAVEBITS;
    wc.lpfnWndProc      = (WNDPROC)DrawSurfWndProc;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "DrawSurfClass";

    if (!RegisterClass(&wc))
	return FALSE;

    wc.style		= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc	= (WNDPROC)TextWndProc;
    wc.hIcon		= NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszMenuName	= NULL;
    wc.lpszClassName	= "Text";

    if (!RegisterClass(&wc))
            return FALSE;



    hMenu	= LoadMenu(ghModule, "MainMenu");

    for (index = 0; index < OD_BTN_CNT; index++) {
        ghBmpDn[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_BASED+index));
        ghBmpUp[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_BASEU+index));
    }
    for (index = 0; index < OD_TOOL_CNT; index++) {
        ghToolBmpDn[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_TOOLBASED+index));
        ghToolBmpUp[index] = (PVOID)LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_TOOLBASEU+index));

    }

    ghwndMain = CreateWindowEx(0L,
            GetStringRes(IDS_MFDEMOCL), GetStringRes2(IDS_EMF),
	    WS_OVERLAPPED   | WS_CAPTION     | WS_BORDER       |
	    WS_THICKFRAME   | WS_MAXIMIZEBOX | WS_MINIMIZEBOX  |
	    WS_CLIPCHILDREN | WS_VISIBLE     | WS_SYSMENU,
            80, 70, 600, 300,
	    NULL, hMenu, ghModule, NULL);

    if (ghwndMain == NULL)
	return FALSE;

    SetWindowLong(ghwndMain, GWL_USERDATA, 0L);
    ghwndNext = SetClipboardViewer(ghwndMain);

    if (gbFit2Wnd)
        CheckMenuItem(hMenu, MM_FIT2WND, MF_CHECKED);
    else
        CheckMenuItem(hMenu, MM_FIT2WND, MF_UNCHECKED);

    if (gbImport3X)
        CheckMenuItem(hMenu, MM_IMPORT_3X, MF_CHECKED);
    else
        CheckMenuItem(hMenu, MM_IMPORT_3X, MF_UNCHECKED);

    if (gbExport3X)
        CheckMenuItem(hMenu, MM_EXPORT_3X, MF_CHECKED);
    else
        CheckMenuItem(hMenu, MM_EXPORT_3X, MF_UNCHECKED);

    SetFocus(ghwndMain);    /* set initial focus */

    gDib.ulFiles = gDib.ulFrames = 0;
    hDC = GetDC(NULL);
    ghHT = CreateHalftonePalette(hDC);
    ReleaseDC(NULL, hDC);

    return TRUE;
}

/******************************Public*Routine******************************\
*
* lProcessWmCreate
*
* Effects:  process WM_CREATE message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessWmCreate(HWND hWnd)
{
    SetWindowLong(hWnd, 0, (LONG)NULL);
    ghDCMem = CreateCompatibleDC(NULL);

    ghwndCtrlPanel = CreateDialog(ghModule,
                                    (LPCSTR)MAKEINTRESOURCE(DID_CTRLPANEL),
                                    hWnd,
                                    (DLGPROC) CtrlPanelDlgProc);

    ghwndDrawSurf = CreateWindow("DrawSurfClass",
                                    NULL,
                                    WS_BORDER | WS_CHILD | WS_VISIBLE,
                                    0, 0, 0, 0,
                                    hWnd,
                                    NULL,
                                    ghModule,
                                    NULL);

    ghTextWnd = CreateWindow("Text",
                                NULL,
                                WS_BORDER | SS_LEFT | WS_CHILD | WS_VISIBLE,
                                0, 0, 0, 0,
                                hWnd,
                                NULL,               //(HMENU) 2,
                                ghModule,
                                NULL);

    ghbrRed     = CreateSolidBrush(RGB(255, 0, 0));
    ghbrAppBkgd = CreateSolidBrush(GetSysColor(COLOR_BACKGROUND));
    ghpnWide    = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
    return 0L;
}


/******************************Public*Routine******************************\
*
* lProcessWmDrawClip
*
* Effects:  process the WM_DRAWCLIPBOARD message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessWmDrawClip(UINT message, DWORD wParam, LONG lParam)
{
    if ((IsClipboardFormatAvailable(CF_METAFILEPICT)) ||
        (IsClipboardFormatAvailable(CF_ENHMETAFILE)) )
        EnableMenuItem(hMenu, MM_PASTE, MF_ENABLED);
    else
        EnableMenuItem(hMenu, MM_PASTE,  MF_GRAYED);

    if (ghwndNext)
        SendMessage(ghwndNext, message, wParam, lParam);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessWmSize
*
* Effects:  process the WM_SIZE message
*
* Warnings:
*
\**************************************************************************/

LRESULT lProcessWmSize(HWND hWnd, UINT message, DWORD wParam, LONG lParam)
{
    RECT        rc;
    LONG        lcyCtrlPanel, lcyDrawSurf;

    GetWindowRect(ghwndCtrlPanel, &rc);
    lcyCtrlPanel = rc.bottom-rc.top;
    lcyDrawSurf = HIWORD(lParam) - lcyCtrlPanel - glcyStatus;

    //
    // CR!! Alternatively, this window can be created with cy
    //      equals to cy of the screen and saving this call
    //      altogether.
    //
    MoveWindow(ghwndCtrlPanel,
               0, 0, LOWORD(lParam), lcyCtrlPanel, TRUE);

    //
    // This ordering guarantees the text window paints correctly
    //
    MoveWindow(ghTextWnd,
               0, lcyCtrlPanel + lcyDrawSurf,
               LOWORD(lParam),                    // cx of hwnd
               glcyStatus, TRUE);

    MoveWindow(ghwndDrawSurf,
               0, lcyCtrlPanel,
               LOWORD(lParam),                    // cx of hwnd
               lcyDrawSurf, TRUE);

    return DefWindowProc(hWnd, message, wParam, lParam);
}

/******************************Public*Routine******************************\
*
* lProcessWmDestroy
*
* Effects:  process the WM_DESTROY message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessWmDestroy(VOID)
{
    if (ghDCMem != (HDC)NULL)
        DeleteDC(ghDCMem);
    if (ghMetaf != (HENHMETAFILE)NULL)
        DeleteEnhMetaFile(ghMetaf);
    DestroyWindow(ghwndCtrlPanel);
    DeleteObject(ghbrRed);
    DeleteObject(ghbrCur);
    DeleteObject(ghpnCur);
    DeleteObject(ghbrAppBkgd);
    DeleteObject(ghpnWide);
    if (ghHT)
        DeleteObject(ghHT);
    ChangeClipboardChain(ghwndMain, ghwndNext);
    bFreeDibFile(&gDib);
    PostQuitMessage(0);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessDIDPlayStep
*
* Effects:  process the WM_COMMAND DID_X messages
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDPlayStep(DWORD wParam, INT *piPlus)
{
    HDC           hDCDrawSurf;
    ENHMETAHEADER EnhMetaHdr;
    RECT          rcClientDS;
    int           iRecord;
    PLAYINFO      PlayInfo;
    BOOL          bRCSet = FALSE;

    if (ghMetaf == 0)
        return 0L;

    GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
    iRecord = LOWORD(wParam) - DID_ZERO + *piPlus;
    SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, iRecord, FALSE);
    PlayInfo.iRecord = iRecord;
    PlayInfo.bPlayContinuous = FALSE;
    *piPlus = 0;

    if ((EnhMetaHdr.nRecords > 1) && (iRecord > 0) &&
        (iRecord <= (INT) EnhMetaHdr.nRecords)) {
        hDCDrawSurf = GetDC(ghwndDrawSurf);

        bRCSet = bSetRC2MatchEmfRC(hDCDrawSurf, EnhMetaHdr, ghMetaf, NULL);
        if (!bRCSet) {
            vErrOut("lProcessDIDPlayStep failed");
        }

        if (gbFit2Wnd) {
            GetClientRect(ghwndDrawSurf, &rcClientDS);
            EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, &rcClientDS);
        } else {
            EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT)&EnhMetaHdr.rclBounds);
        }
        //
        // Enabling the user to record a metafile record selectively
        //

        if ((gbRecording) && (ghDCMetaf != NULL)) {
            EnumEnhMetaFile(ghDCMetaf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT)&EnhMetaHdr.rclBounds);
        }

        if (bRCSet) {
            bCleanUpRC();
        }

        ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessDIDPlayTenPlus
*
* Effects:  process the WM_COMMAND DID_TEN_PLUS message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDPlayTenPlus(INT *piPlus)
{
    if (ghMetaf == 0)
        return 0L;

    *piPlus += 10;
    SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, *piPlus, FALSE);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMCopy
*
* Effects:  process the WM_COMMAND MM_COPY message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMCopy(VOID)
{
    if ((ghMetaf == 0) && (ghmf == 0)) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_NOMFFORCPY));
        return 0L;
    }

    OpenClipboard(ghwndMain);
    EmptyClipboard();

    if (gbExport3X)
    {
        HGLOBAL         hmem;
        LPMETAFILEPICT  lpmfp;
        RECT            rcClientDS;
        DWORD           x, y, mm;
        HDC             hDCDrawSurf;
        LPBYTE          pjData;
        UINT            uiSize;

        hDCDrawSurf = GetDC(ghwndDrawSurf);

        if (ghmf == 0) {
            SetWindowText(ghTextWnd, GetStringRes(IDS_CNVEMFTO3X));

            if (!(uiSize = GetWinMetaFileBits(ghMetaf, 0, NULL, MM_ANISOTROPIC, hDCDrawSurf))) {
                MessageBox(ghwndMain, GetStringRes(IDS_GETWMFBTSFAIL),
                    GetStringRes(IDS_ERROR), MB_OK);
                goto COPY_3X_EXIT;
            }

            if ((pjData = (LPBYTE) LocalAlloc(LMEM_FIXED, uiSize)) == NULL) {
                MessageBox(ghwndMain, GetStringRes(IDS_MALLOCFAIL),
                    GetStringRes(IDS_ERROR), MB_OK);
                goto COPY_3X_EXIT;
            }

            if (!(uiSize = GetWinMetaFileBits(ghMetaf, uiSize, pjData, MM_ANISOTROPIC, hDCDrawSurf))) {
                MessageBox(ghwndMain, GetStringRes(IDS_GETWMFBTSFAIL2),
                    GetStringRes(IDS_ERROR), MB_OK);
                LocalFree(pjData);
                goto COPY_3X_EXIT;
            }

            ghmf = SetMetaFileBitsEx(uiSize, (LPBYTE) pjData);
            LocalFree(pjData);
        }

        if ((hmem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,
                            sizeof(METAFILEPICT))) == NULL) {

            SetWindowText(ghTextWnd, GetStringRes(IDS_MALLOCFAIL2));
            goto COPY_3X_EXIT;

        }

        lpmfp = (LPMETAFILEPICT)GlobalLock(hmem);
        lpmfp->mm = mm = MM_ANISOTROPIC;

        GetClientRect(ghwndDrawSurf, &rcClientDS);
        x = rcClientDS.right - rcClientDS.left;
        x *= 2540;
        x /= GetDeviceCaps(hDCDrawSurf, LOGPIXELSX);
        lpmfp->xExt = x;                                // ie. in 0.01mm

        y = rcClientDS.bottom - rcClientDS.top;
        y *= 2540;
        y /= GetDeviceCaps(hDCDrawSurf, LOGPIXELSY);
        lpmfp->yExt = y;                                // ie. in 0.01mm

        lpmfp->hMF = CopyMetaFile(ghmf, NULL);

        GlobalUnlock(hmem);
        SetWindowText(ghTextWnd, GetStringRes(IDS_CPY3XMFTOCLP));
        SetClipboardData(CF_METAFILEPICT, hmem);

COPY_3X_EXIT:
        ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
        goto COPY_EXIT;

    }

    //
    // gbExport3X == FALSE
    //
    if (ghMetaf == 0)           // requires conversion
    {
        UINT            uiSize;
        LPVOID          pvData;
        HDC             hDCDrawSurf;

        SetWindowText(ghTextWnd, "Converting 3X Metafile to Enhanced Metafile format");
        if (!(uiSize = GetMetaFileBitsEx(ghmf, 0, NULL))) {
            MessageBox(ghwndMain, GetStringRes(IDS_GETMFBTSXFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
            SetWindowText(ghTextWnd, GetStringRes(IDS_CNVFAIL));
            goto COPY_EXIT;
        }

        if ((pvData = (LPVOID) LocalAlloc(LMEM_FIXED, uiSize)) == NULL) {
            MessageBox(ghwndMain, GetStringRes(IDS_MALLOCFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
            SetWindowText(ghTextWnd, GetStringRes(IDS_CNVFAIL));
            goto COPY_EXIT;
        }

        if (!(uiSize = GetMetaFileBitsEx(ghmf, uiSize, pvData))) {
            MessageBox(ghwndMain, GetStringRes(IDS_GETWMFBTSFAIL2X),
                GetStringRes(IDS_ERROR), MB_OK);
            goto COPY_ENH_EXIT;
        }

        hDCDrawSurf = GetDC(ghwndDrawSurf);

        // !!! provide the correct picture extents in the METAFILEPICT structure
        // where possible
        ghMetaf = SetWinMetaFileBits(uiSize, (LPBYTE)pvData, hDCDrawSurf, NULL);

COPY_ENH_EXIT:
        LocalFree(pvData);
        ReleaseDC(ghwndDrawSurf ,hDCDrawSurf);

        if (ghMetaf == 0) {
            SetWindowText(ghTextWnd, GetStringRes(IDS_CNVFAIL));
            goto COPY_EXIT;
        }

    }

    //
    // No Conversion required
    //
    {

        HENHMETAFILE hEmfTmp;

        hEmfTmp = CopyEnhMetaFile(ghMetaf, NULL);

        if (hEmfTmp) {
            SetWindowText(ghTextWnd, GetStringRes(IDS_CPYEMFTOCLP));
            SetClipboardData(CF_ENHMETAFILE, hEmfTmp);
            DeleteEnhMetaFile(hEmfTmp);
        }

    }

COPY_EXIT:

    CloseClipboard();
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMPaste
*
* Effects:  process the WM_COMMAND MM_PASTE message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMPaste(HWND hwnd, BOOL *pbReset)
{

    OpenClipboard(ghwndMain);

    if (gbImport3X)
    {
        HANDLE      hmem;
        DWORD       dwXSugExt, dwYSugExt, dwMM;
        HDC         hDCDrawSurf;
        RECT        rc;
        INT         iSavedDC;


        hmem = GetClipboardData(CF_METAFILEPICT);

        if (hmem)
        {
            LPMETAFILEPICT lpmfp;

            SetWindowText(ghTextWnd, GetStringRes(IDS_PST3XMF));
            lpmfp = (LPMETAFILEPICT)GlobalLock(hmem);
            ghmf  = lpmfp->hMF;
            dwMM  = lpmfp->mm;
            dwXSugExt = lpmfp->xExt;        // in 0.01 mm
            dwYSugExt = lpmfp->yExt;
            GlobalUnlock(hmem);

            hDCDrawSurf = GetDC(ghwndDrawSurf);

            iSavedDC = SaveDC(hDCDrawSurf);

            GetClientRect(ghwndDrawSurf, &rc);

            SetMapMode(hDCDrawSurf, dwMM);
            if ((dwXSugExt > 0 )&& (dwYSugExt > 0))
            {                               // suggested width & height of image
                DWORD x;
                DWORD y;

                // no. of pixels in x and y
                x = dwXSugExt;
                x *= GetDeviceCaps(hDCDrawSurf,LOGPIXELSX);
                x /= 2540;

                y = dwYSugExt;
                y *= GetDeviceCaps(hDCDrawSurf,LOGPIXELSY);
                y /= 2540;

                SetWindowExtEx(hDCDrawSurf, x, y, NULL);

                if (gbFit2Wnd)
                    SetViewportExtEx(hDCDrawSurf, rc.right, rc.bottom, NULL);
                else
                    SetViewportExtEx(hDCDrawSurf, x, y, NULL);

            } else {
                SetWindowText(ghTextWnd, GetStringRes(IDS_NOINFO3XMFX));
                SetWindowExtEx(hDCDrawSurf, rc.right, rc.bottom, NULL);
                SetViewportExtEx(hDCDrawSurf, rc.right, rc.bottom, NULL);
            }

            SetViewportOrgEx(hDCDrawSurf, 0, 0, NULL);
            SetWindowOrgEx(hDCDrawSurf, 0, 0, NULL);

            SetBoundsRect(hDCDrawSurf, NULL, DCB_ENABLE | DCB_SET);
            PlayMetaFile(hDCDrawSurf, ghmf);
            {
            UINT    uiRC;
            char    text[128];

            wsprintf(text, "dwMM = %d\n", dwMM);
            OutputDebugString(text);
            wsprintf(text, "dwXSugExt = %d\n", dwXSugExt);
            OutputDebugString(text);
            wsprintf(text, "dwYSugExt = %d\n", dwYSugExt);
            OutputDebugString(text);

            uiRC = GetBoundsRect(hDCDrawSurf, &rc, DCB_RESET); // in logical coordinates
            wsprintf(text, "GetBoundsRect = %d\n", uiRC);
            OutputDebugString(text);
            wsprintf(text, "left     = %d\n", rc.left);
            OutputDebugString(text);
            wsprintf(text, "right    = %d\n", rc.right);
            OutputDebugString(text);
            wsprintf(text, "top      = %d\n", rc.top);
            OutputDebugString(text);
            wsprintf(text, "bottom   = %d\n", rc.bottom);
            OutputDebugString(text);
            }

// !!!
// saving the wmf as an Aldus mf
//
{
OPENFILENAME    ofn;
char            szFile[256], szFileTitle[256];
static char     *szFilter;
UINT            uiSize;
HANDLE          hFile, hMapFile;
LPVOID          pMapFile;
DWORD           dwHigh, dwLow;

BuildFilterStrs(-1);
szFilter = BuildFilterStrs(IDS_FT_WMF);

strcpy(szFile, "*.wmf\0");
ofn.lStructSize = sizeof(OPENFILENAME);
ofn.hwndOwner = hwnd;
ofn.lpstrFilter = szFilter;
ofn.lpstrCustomFilter = (LPSTR) NULL;
ofn.nMaxCustFilter = 0L;
ofn.nFilterIndex = 1;
ofn.lpstrFile = szFile;
ofn.nMaxFile = sizeof(szFile);
ofn.lpstrFileTitle = szFileTitle;
ofn.nMaxFileTitle = sizeof(szFileTitle);
ofn.lpstrInitialDir = NULL;
ofn.lpstrTitle = GetStringRes(IDS_SAVEMF);
ofn.Flags = 0L;
ofn.nFileOffset = 0;
ofn.nFileExtension = 0;
ofn.lpstrDefExt = "WMF";

if (!GetOpenFileName(&ofn))
    return 0L;

uiSize = GetMetaFileBitsEx(ghmf, 0, NULL);
dwHigh = 0;
dwLow  = uiSize;

if ((hFile = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == (HANDLE)-1) {
    MessageBox(ghwndMain, GetStringRes(IDS_FILEOPNFAIL),
        GetStringRes(IDS_ERROR), MB_OK);
    return 0L;
}

//
// Create a map file of the opened file
//
if ((hMapFile = CreateFileMapping(hFile, NULL,
                         PAGE_READWRITE, dwHigh, dwLow, "MapF")) == NULL) {
    MessageBox(ghwndMain, GetStringRes(IDS_MAPFCREFAIL),
        GetStringRes(IDS_ERROR), MB_OK);
    goto ErrorExit1;
}

//
// Map a view of the whole file
//
if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, uiSize)) == NULL) {
    MessageBox(ghwndMain, GetStringRes(IDS_MAPVWMAPFOFAIL),
        GetStringRes(IDS_ERROR), MB_OK);
    goto ErrorExit2;
}

if (uiSize) {
    APMFILEHEADER   AldHdr;
    PAPMFILEHEADER  pAldHdr;
    PBYTE           pjTmp;
    INT             iSize;
    char            text[128];

    AldHdr.key = ALDUS_ID;
    AldHdr.hmf = 0;                                 // Unused; must be zero
    AldHdr.bbox.Left   = 0;                         // in metafile units
    AldHdr.bbox.Top    = 0;
    //AldHdr.bbox.Right  = rc.right - rc.left;        // in logical coordinates
    //AldHdr.bbox.Bottom = rc.bottom - rc.top;

    switch (dwMM) {
        case MM_HIENGLISH:
            AldHdr.inch = 1000;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_HIMETRIC:
            AldHdr.inch = 1440;
            AldHdr.bbox.Right  = (SHORT)(dwXSugExt / 2540 * 1440);
            AldHdr.bbox.Bottom = (SHORT)(dwYSugExt / 2540 * 1440);
            break;
        case MM_LOENGLISH:
            AldHdr.inch = 100;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_LOMETRIC:
            AldHdr.inch = 254;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_TEXT:
            AldHdr.inch = (WORD) (GetDeviceCaps(hDCDrawSurf, HORZRES) * 25.4 /
                          GetDeviceCaps(hDCDrawSurf, HORZSIZE));
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        case MM_TWIPS:
            AldHdr.inch = 1440;
            AldHdr.bbox.Right  = (SHORT)dwXSugExt;
            AldHdr.bbox.Bottom = (SHORT)dwYSugExt;
            break;
        default:
            AldHdr.inch = 1440;
            AldHdr.bbox.Right  = (SHORT)(dwXSugExt / 2540 * 1440);
            AldHdr.bbox.Bottom = (SHORT)(dwYSugExt / 2540 * 1440);
            break;
    }

    wsprintf(text, "MM           = %d\n", dwMM);
    OutputDebugString(text);
    wsprintf(text, "AldHdr.inch  = %d\n", AldHdr.inch);
    OutputDebugString(text);

    AldHdr.reserved = 0;
    AldHdr.checksum = 0;
    {
    WORD    *p;

    for (p = (WORD *)&AldHdr, AldHdr.checksum = 0;
            p < (WORD *)&(AldHdr.checksum); ++p)
        AldHdr.checksum ^= *p;
    }

    pAldHdr = &AldHdr;
    pjTmp = (PBYTE)pMapFile;

    iSize = 22;

    //!!! use memcpy...
    while (iSize--) {
        *(((PBYTE)pjTmp)++) = *(((PBYTE)pAldHdr)++);
    }

    pMapFile = (PBYTE)pMapFile + 22;
    GetMetaFileBitsEx(ghmf, uiSize, pMapFile);
}


UnmapViewOfFile(pMapFile);

ErrorExit2:
  CloseHandle(hMapFile);
ErrorExit1:
  CloseHandle(hFile);
}

            RestoreDC(hDCDrawSurf, iSavedDC);
            ReleaseDC(ghwndDrawSurf, hDCDrawSurf);

        } else {
            SetWindowText(ghTextWnd, GetStringRes(IDS_GET3XFRMCLPFAIL));
        }

        goto PASTE_EXIT;

    }

    //
    // gbImport3X == FALSE
    //
    {
        HENHMETAFILE hEmfTmp;
        ENHMETAHEADER EnhMetaHdr;

        hEmfTmp = GetClipboardData(CF_ENHMETAFILE);
        if (hEmfTmp) {
            SetWindowText(ghTextWnd, GetStringRes(IDS_PSTEMF));
            if (ghMetaf != (HENHMETAFILE)NULL)
                DeleteEnhMetaFile(ghMetaf);
            ghMetaf = CopyEnhMetaFile(hEmfTmp, NULL);
            DeleteEnhMetaFile(hEmfTmp);
            GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
            SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, EnhMetaHdr.nRecords, FALSE);
            *pbReset = TRUE;
        } else {
            SetWindowText(ghTextWnd, GetStringRes(IDS_GETEMFFROMCLPFAIL));
        }
    }
PASTE_EXIT:

    CloseClipboard();
    EnableMenuItem(hMenu, MM_COPY,  MF_ENABLED);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMDel
*
* Effects:  process the WM_COMMAND MM_DEL message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMDel(VOID)
{
    OpenClipboard(ghwndMain);
    EmptyClipboard();
    CloseClipboard();
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMPen
*
* Effects:  process the WM_COMMAND MM_PEN message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMPen(HWND hwnd)
{
    HDC     hDC;
    DWORD   dwRGB;

    if (bChooseNewColor(hwnd, &dwRGB)) {
        hDC = GetDC(ghwndDrawSurf);
        if (ghpnCur != NULL)
            DeleteObject(ghpnCur);
        ghpnCur = CreatePen(PS_SOLID, 1, dwRGB);
        SelectObject(hDC, ghpnCur);
        if (ghDCMetaf != NULL)
            SelectObject(ghDCMetaf, ghpnCur);
        ReleaseDC(ghwndDrawSurf, hDC);
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMBrush
*
* Effects:  process the WM_COMMAND MM_BRUSH message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMBrush(HWND hwnd)
{
    HDC     hDC;
    static DWORD   dwRGB=RGB(255, 255, 255);

    if (bChooseNewColor(hwnd, &dwRGB)) {
        hDC = GetDC(ghwndDrawSurf);
        if (ghbrCur != NULL)
            DeleteObject(ghbrCur);
        ghbrCur = hBrCreateBrush(hDC, dwRGB);
        SelectObject(hDC, ghbrCur);
        if (ghDCMetaf != NULL)
            SelectObject(ghDCMetaf, ghbrCur);
        ReleaseDC(ghwndDrawSurf, hDC);
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMFont
*
* Effects:  process the WM_COMMAND MM_FONT message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMFont(VOID)
{
    HDC     hDC;
    char    text[128];

    if (bChooseNewFont(ghwndMain, &glf, &gCrText)) {
        ghCurFont = CreateFontIndirect(&glf);

        hDC = GetDC(ghwndDrawSurf);
        EnumFonts(hDC, glf.lfFaceName, (FONTENUMPROC)iTT, (LPARAM)&gbTT);
        wsprintf(text, "gbTT = %d\n", gbTT);
        //OutputDebugString(text);
        ReleaseDC(ghwndDrawSurf, hDC);

        if (ghDCMetaf != NULL)
            SelectObject(ghDCMetaf, ghCurFont);
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMStrokeFill
*
* Effects:  process the WM_COMMAND MM_TTOUTLN_STROKEFILL message
*
* Warnings:
*
* History:
*  03-Jan-1996 -by- Petrus Wong [PetrusW]
* Wrote it.
\**************************************************************************/

LONG lProcessMMStrokeFill(VOID)
{
    gbSFOutln = (gbSFOutln ? FALSE : TRUE);
    if (gbSFOutln) {
        CheckMenuItem(hMenu, MM_TTOUTLN_STROKEFILL, MF_CHECKED);
        CheckMenuItem(hMenu, MM_TTOUTLN_POLYDRAW, MF_UNCHECKED);
        gbPDOutln = FALSE;
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMPolyDraw
*
* Effects:  process the WM_COMMAND MM_TTOUTLN_POLYDRAW message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMPolyDraw(VOID)
{
    gbPDOutln = (gbPDOutln ? FALSE : TRUE);
    if (gbPDOutln) {
        CheckMenuItem(hMenu, MM_TTOUTLN_STROKEFILL, MF_UNCHECKED);
        CheckMenuItem(hMenu, MM_TTOUTLN_POLYDRAW, MF_CHECKED);
        gbSFOutln = FALSE;
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMMltfmts
*
* Effects:  process the WM_COMMAND MM_C_MLTFMTS message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMMltfmts(VOID)
{
    CMTMLTFMT *pMfmt;

    if ((gbRecording) && (ghDCMetaf != NULL)) {
	if ((pMfmt = pLoadMltFmtFile()) != NULL) {
	    GdiComment(ghDCMetaf,
		       sizeof(CMTMLTFMT)+pMfmt->aemrformat[0].cbData,
		       (CONST BYTE *) pMfmt);
	    Free(pMfmt);
	}
	else
	    MessageBox(ghwndMain,
                       GetStringRes(IDS_LDMFMTFILEFAIL),
		       GetStringRes(IDS_ERROR),
		       MB_OK);
    }

    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMHittest
*
* Effects:  process the WM_COMMAND MM_HITTEST message
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMHittest(VOID)
{
    static BOOL bHitTest=FALSE;
    HWND	hwndRecBtn;

    bHitTest = (bHitTest ? FALSE : TRUE);
    hwndRecBtn = GetDlgItem(ghwndCtrlPanel, DID_RECORD);
    if (bHitTest) {
	CheckMenuItem(hMenu, MM_HITTEST, MF_CHECKED);
	EnableMenuItem(hMenu, MM_RECORD, MF_GRAYED);
	EnableWindow(hwndRecBtn, FALSE);
	gbHitTest = TRUE;
    } else {
	CheckMenuItem(hMenu, MM_HITTEST, MF_UNCHECKED);
	EnableMenuItem(hMenu, MM_RECORD, MF_ENABLED);
	EnableWindow(hwndRecBtn, TRUE);
	gbHitTest = FALSE;
	return 0L;
    }

    if (ghMetaf == 0) {
	SetWindowText(ghTextWnd, GetStringRes(IDS_NOMFLDFORHTST));
	return 0L;
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMLEAbout
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMLEAbout(VOID)
{
    if (DialogBox(ghModule, (LPCSTR)"AboutBox", ghwndMain, (DLGPROC)About) == -1)
	MessageBox(ghwndMain, GetStringRes(IDS_DLGABOUTCREFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
	return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMAbout
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMAbout(VOID)
{
    if (DialogBox(ghModule, "AboutBox", ghwndMain, (DLGPROC)About) == -1)
       MessageBox(ghwndMain, GetStringRes(IDS_DLGABOUTCREFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMLoadMaskBmp
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMLoadMaskBmp(HWND hwnd)
{
    SetWindowText(ghTextWnd, GetStringRes(IDS_LDMSKBM));
    bGetBMP(hwnd, TRUE);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMLoadBmp
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMLoadBmp(HWND hwnd)
{
    SetWindowText(ghTextWnd, GetStringRes(IDS_LDBM));
    bGetBMP(hwnd, FALSE);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMSaveBmp
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMSaveBmp(VOID)
{
    SetWindowText(ghTextWnd, GetStringRes(IDS_SVDRAWSFASBM));
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessDIDOpen
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDOpen(HWND hwnd, CHAR *szLoadedMetaf, BOOL *pbReset)
{
    ENHMETAHEADER EnhMetaHdr;
    HENHMETAFILE  hEmfTmp;

    SetWindowText(ghTextWnd, GetStringRes(IDS_LDMF));
    //
    // If user hit cancel, we still have the original metafile
    //
    //DeleteEnhMetaFile(ghMetaf);
    //ghMetaf = hemfLoadMetafile(hwnd);
    hEmfTmp = hemfLoadMetafile(hwnd);
    if (hEmfTmp != 0) {
	char	 szDesc[256];

	if (ghMetaf != (HENHMETAFILE)NULL)
            DeleteEnhMetaFile(ghMetaf);
	ghMetaf = CopyEnhMetaFile(hEmfTmp, NULL);
	GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
	SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, EnhMetaHdr.nRecords, FALSE);
	DeleteEnhMetaFile(hEmfTmp);
	EnableMenuItem(hMenu, MM_COPY,	MF_ENABLED);
	if (GetEnhMetaFileDescription(ghMetaf, 256, szDesc) != 0) {
	    char    szText[256];
	    char    *szTmp, szSource[256];

	    szTmp = (char *)strtok(szDesc, "\\0");
	    strcpy(szSource, szTmp);
	    szTmp = (char *)strtok(NULL, "\\0\\0");
	    wsprintf(szText, "Source: %s  Title: %s", szSource, szTmp);
	    SetWindowText(ghTextWnd, szText);
	    strcpy(szLoadedMetaf, szTmp);
	} else {
	    strcpy(szLoadedMetaf, "");
	}
    //} else {
    //	  SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, 0, FALSE);
    }
    *pbReset = TRUE;
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMRec
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMRec(HWND hwnd, CHAR *szFilename)
{
    if (gbHitTest) {
	SetWindowText(ghTextWnd, GetStringRes(IDS_MUSTESCHTSTMD));
	return 0L;
    }

    SetWindowText(ghTextWnd, GetStringRes(IDS_REC));
    if (!gbRecording) {
	ghDCMetaf = hDCRecordMetafileAs(hwnd, szFilename);
    }

    if (ghDCMetaf == NULL) {
       SetWindowText(ghTextWnd, GetStringRes(IDS_CREMFDCFAIL));
       return 0L;
    }

    // Parse the szFilename for the title and GdiComment the metafile with it.
    {
	char	szComment[256];
	char	*szTmp, szTmp2[256];

	szTmp = (char *)strtok(szFilename, "\\");
	strcpy(szTmp2, szTmp);
	while (szTmp != NULL) {
	    szTmp = (char *)strtok(NULL, "\\");
	    if (szTmp != NULL) {
		strcpy(szTmp2, szTmp);
	    }
	}
	szTmp = (char *)strtok(szTmp2, ".");
	wsprintf((LPSTR) szComment, "MfEdit:\\0%s\\0\\0", szTmp);
#if 0
	if (!GdiComment(ghDCMetaf, 256, szComment)) {
	    MessageBox(ghwndMain, GetStringRes(IDS_ADDCMNTFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
	}
#endif
    }

    gbRecording = TRUE;

    if (ghpnCur != NULL)
	SelectObject(ghDCMetaf, ghpnCur);

    if (ghbrCur != NULL)
	SelectObject(ghDCMetaf, ghbrCur);

    if (ghCurFont != NULL)
	SelectObject(ghDCMetaf, ghCurFont);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessDIDRec
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDRec(HWND hwnd, INT *piMetafCnt, char *szFilename)
{
    int  iWidthMM, iHeightMM, iWidthPels, iHeightPels, iMMPerPelX, iMMPerPelY;
    char szComment[256];
    char szTitle[256];
    RECT rc;
    HDC  hDC;


    if (gbHitTest) {
	SetWindowText(ghTextWnd, GetStringRes(IDS_MUSTESCHTSTMD));
	return 0L;
    }

    SetWindowText(ghTextWnd, GetStringRes(IDS_REC));
    if (!gbRecording) {

	hDC = GetDC(hwnd);
	iWidthMM    = GetDeviceCaps(hDC, HORZSIZE);
	iHeightMM   = GetDeviceCaps(hDC, VERTSIZE);
	iWidthPels  = GetDeviceCaps(hDC, HORZRES);
	iHeightPels = GetDeviceCaps(hDC, VERTRES);
	ReleaseDC(hwnd, hDC);
	iMMPerPelX  = (iWidthMM * 100)/iWidthPels;
	iMMPerPelY  = (iHeightMM * 100)/iHeightPels;
	GetClientRect(ghwndDrawSurf, &rc);
	rc.left   = rc.left * iMMPerPelX;
	rc.top	  = rc.top * iMMPerPelY;
	rc.right  = rc.right * iMMPerPelX;
	rc.bottom = rc.bottom * iMMPerPelY;

	{
	   char szFilenameWithExt[256];
	   char suffix[20];
	   char szDesc[256];
	   char *szTmp, szTmp2[256];

	   //
	   // assemble a new metafile name with the emf extension from
	   // the generic szFilename
	   //
	   wsprintf((LPSTR) suffix, "%d.emf", *piMetafCnt);
	   *piMetafCnt++;
	   strcpy(szFilenameWithExt, szFilename);
	   strcat(szFilenameWithExt, suffix);

	   //
	   // parse szFilename for the title for description
	   //
	   szTmp = (char *)strtok(szFilename, "\\");
	   strcpy(szTmp2, szTmp);
	   while (szTmp != NULL) {
	       szTmp = (char *)strtok(NULL, "\\");
	       if (szTmp != NULL) {
		   strcpy(szTmp2, szTmp);
	       }
	   }
	   szTmp = (char *)strtok(szTmp2, ".");
	   strcpy(szTitle, szTmp);
	   wsprintf(szDesc, "SDK Enhanced Metafile Editor\\0%s\\0\\0", szTitle);
	   ghDCMetaf = CreateEnhMetaFile((HDC)NULL, szFilenameWithExt, (LPRECT)&rc, (LPSTR)szDesc);
           if (gb3D)
                bSetupRC(ghDCMetaf, NULL);
	}

        bSetAdvancedGraphics(ghDCMetaf);
    }

    if (ghDCMetaf == NULL) {
       SetWindowText(ghTextWnd, GetStringRes(IDS_CREMFDCFAIL));
       return 0L;
    }
    wsprintf((LPSTR) szComment, "MfEdit:\\0%s\\0\\0", szTitle);
#if 0
    if (!GdiComment(ghDCMetaf, 256, szComment)) {
	MessageBox(ghwndMain, GetStringRes(IDS_ADDCMNTFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
    }
#endif
    gbRecording = TRUE;

    if (ghpnCur != NULL)
	SelectObject(ghDCMetaf, ghpnCur);

    if (ghbrCur != NULL)
	SelectObject(ghDCMetaf, ghbrCur);

    if (ghCurFont != NULL)
	SelectObject(ghDCMetaf, ghCurFont);

    return 0L;
}


/******************************Public*Routine******************************\
*
* lProcessDIDStop
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDStop(VOID)
{
    SetWindowText(ghTextWnd, GetStringRes(IDS_STOP));
    if (gbRecording) {
        if (gb3D && gbRCSet)
            bCleanUpRC();
	ghMetaf = CloseEnhMetaFile(ghDCMetaf);
	gbRecording = FALSE;
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessDIDPlay
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDPlay(char *szLoadedMetaf)
{
    HDC                     hDCDrawSurf;
    ENHMETAHEADER           EnhMetaHdr;
    RECT                    rcClientDS;
    int                     iEntries;
    PLOGPALETTE             plogPal;
    PBYTE                   pjTmp;
    HPALETTE                hPal;
    char                    szTmp[256];
    BOOL		    bRCSet = FALSE;
#ifdef OPENGL_EMF
    PIXELFORMATDESCRIPTOR   pfd;
#endif
    BOOL                    bHasPFD = TRUE;


    wsprintf(szTmp, GetStringRes(IDS_FMT_PLAYMF), szLoadedMetaf);
    SetWindowText(ghTextWnd, szTmp);
    if (ghMetaf != NULL) {
	hDCDrawSurf = GetDC(ghwndDrawSurf);
	GetEnhMetaFileHeader(ghMetaf, sizeof(ENHMETAHEADER), &EnhMetaHdr);

	iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

	if (iEntries) {
	    if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
		    sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
		MessageBox(ghwndMain, GetStringRes(IDS_CREPALFAIL),
                    GetStringRes(IDS_ERROR), MB_OK);
	    }

	    plogPal->palVersion = 0x300;
	    plogPal->palNumEntries = (WORD) iEntries;
	    pjTmp = (PBYTE) plogPal;
	    pjTmp += 4;

	    GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
	    hPal = CreatePalette(plogPal);
	    GlobalFree(plogPal);

	    SelectPalette(hDCDrawSurf, hPal, FALSE);
	    RealizePalette(hDCDrawSurf);
	}

#ifdef OPENGL_EMF
        bRCSet = bSetRC2MatchEmfRC(hDCDrawSurf, EnhMetaHdr, ghMetaf, &pfd);
        bHasPFD = (pfd.nSize != 0) ? TRUE : FALSE;
        if (!bRCSet) {
            vErrOut("lProcessDIDPlay (DspDC) failed");
        }
#endif
	if (gbFit2Wnd) {
	    GetClientRect(ghwndDrawSurf, &rcClientDS);
	    if (!PlayEnhMetaFile( hDCDrawSurf, ghMetaf, (LPRECT) &rcClientDS)) {
		char	text[128];

		wsprintf(text, GetStringRes(IDS_FMT_PLAYEMFFAIL), GetLastError());
		OutputDebugString(text);
	    } else {
                if (gbDB)
                   SwapBuffers(hDCDrawSurf);
            }
	} else {
	    RECT rc;

	    rc.top = rc.left = 0;
	    rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
	    rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
	    if (!PlayEnhMetaFile( hDCDrawSurf, ghMetaf, (LPRECT) &rc)) {
		char	text[128];

		wsprintf(text, GetStringRes(IDS_FMT_PLAYEMFFAIL), GetLastError());
		OutputDebugString(text);
	    } else {
                if (gbDB)
                   SwapBuffers(hDCDrawSurf);
            }
	}

        if (bRCSet) {
            bCleanUpRC();
            bRCSet = FALSE;
        }

	//
	// Enabling the user to embed another metafile
	//
	if ((gbRecording) && (ghDCMetaf != NULL)) {
	    if (hPal != (HPALETTE)NULL) {
		SelectPalette(ghDCMetaf, hPal, FALSE);
		RealizePalette(ghDCMetaf);
	    }

#ifdef OPENGL_EMF
            if (EnhMetaHdr.bOpenGL && !gbRCSet) {
                char        text[128];

                bRCSet = bSetupRC(ghDCMetaf, (bHasPFD ? &pfd : NULL));
                wsprintf(text,
                         "lProcessDIDPlay: (MetaDC) bRCSet=%ld, bHasPFD=%ld",
                         bRCSet, bHasPFD);
                vErrOut(text);
            }
#endif
	    {
	    RECT rc;

	    rc.top = rc.left = 0;
	    rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
	    rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
	    if (!PlayEnhMetaFile( ghDCMetaf, ghMetaf, (LPRECT) &rc)) {
		char	text[128];

		wsprintf(text, GetStringRes(IDS_FMT_PLAYEMFFAIL), GetLastError());
		OutputDebugString(text);
	    } else {
                if (gbDB)
                   SwapBuffers(ghDCMetaf);
            }


            if (bRCSet) {
                bCleanUpRC();
            }

	    }
	}

	ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
    } else {
	SetWindowText(ghTextWnd, GetStringRes(IDS_NOMFFORPLAY));
    }

    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessDIDFf
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDFf(BOOL *pbReset)
{
    HDC 	    hDCDrawSurf;
    ENHMETAHEADER   EnhMetaHdr;
    RECT	    rcClientDS;
    static int	    iRecord = 0;
    PLAYINFO	    PlayInfo;
    int 	    iEntries;
    PLOGPALETTE     plogPal;
    PBYTE	    pjTmp;
    HPALETTE	    hPal;
    static BOOL     bInitRC = FALSE;
    static BOOL     bRCSet = FALSE;
    BOOL	    bHasPFD = FALSE;

    if (ghMetaf == 0)
	return 0L;

    PlayInfo.iRecord = ++iRecord;
    PlayInfo.bPlayContinuous = TRUE;

    GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);
    SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, iRecord, FALSE);
    if ((EnhMetaHdr.nRecords > 1) && (iRecord <= (INT)EnhMetaHdr.nRecords)) {
	hDCDrawSurf = GetDC(ghwndDrawSurf);

	iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

	if (iEntries) {
	    if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
		    sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
		MessageBox(ghwndMain, GetStringRes(IDS_CREPALFAIL),
                    GetStringRes(IDS_ERROR), MB_OK);
	    }

	    plogPal->palVersion = 0x300;
	    plogPal->palNumEntries = (WORD) iEntries;
	    pjTmp = (PBYTE) plogPal;
	    pjTmp += 4;

	    GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
	    hPal = CreatePalette(plogPal);
	    GlobalFree(plogPal);

	    SelectPalette(hDCDrawSurf, hPal, FALSE);
	    RealizePalette(hDCDrawSurf);
	}

        if (!bInitRC) {
            bRCSet = bSetRC2MatchEmfRC(hDCDrawSurf, EnhMetaHdr, ghMetaf, NULL);
            bInitRC = TRUE;
            if (!bRCSet) {
                vErrOut("lProcessDIDFf failed");
            }
        }

	if (gbFit2Wnd) {
	    GetClientRect(ghwndDrawSurf, &rcClientDS);
	    EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT) &rcClientDS);
	} else {
	    EnumEnhMetaFile(hDCDrawSurf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT) &EnhMetaHdr.rclBounds);
	}
	//
	// Enabling the user to record a metafile records selectively
	//
	if ((gbRecording) && (ghDCMetaf != NULL)) {
	    SelectPalette(ghDCMetaf, hPal, FALSE);
	    RealizePalette(ghDCMetaf);
	    EnumEnhMetaFile(ghDCMetaf, ghMetaf, (ENHMFENUMPROC)bPlayRecord, (LPVOID) &PlayInfo, (LPRECT)&EnhMetaHdr.rclBounds);
	}

	ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
    }

    if ((iRecord == (INT) EnhMetaHdr.nRecords) || *pbReset) {
	iRecord = 0;
	if (*pbReset)
	    SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, 0, FALSE);
	*pbReset = FALSE;

        if (bRCSet) {
            bCleanUpRC();
            bRCSet = FALSE;
        }

        bInitRC = FALSE;
    }
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessDIDClear
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessDIDClear(VOID)
{
    HDC     hDCDrawSurf;
    HGDIOBJ hObjOld;
    RECT    rcDrawSurf;

    SetWindowText(ghTextWnd, GetStringRes(IDS_DRAWSFCLR));
    hDCDrawSurf = GetDC(ghwndDrawSurf);
    ghbrAppBkgd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    hObjOld = SelectObject(hDCDrawSurf, ghbrAppBkgd);
    GetClientRect(ghwndDrawSurf, &rcDrawSurf);
    PatBlt(hDCDrawSurf, 0, 0, rcDrawSurf.right, rcDrawSurf.bottom, PATCOPY);
    ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
    SelectObject(hDCDrawSurf, hObjOld);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMImport3X
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMImport3X(VOID)
{
    gbImport3X = (gbImport3X ? FALSE : TRUE);

    if (gbImport3X)
	CheckMenuItem(hMenu, MM_IMPORT_3X, MF_CHECKED);
    else
	CheckMenuItem(hMenu, MM_IMPORT_3X, MF_UNCHECKED);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMExport3X
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMExport3X(VOID)
{
    gbExport3X = (gbExport3X ? FALSE : TRUE);

    if (gbExport3X)
	CheckMenuItem(hMenu, MM_EXPORT_3X, MF_CHECKED);
    else
	CheckMenuItem(hMenu, MM_EXPORT_3X, MF_UNCHECKED);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMFit2Wnd
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMFit2Wnd(VOID)
{
    gbFit2Wnd = (gbFit2Wnd ? FALSE : TRUE);

    if (gbFit2Wnd)
	CheckMenuItem(hMenu, MM_FIT2WND, MF_CHECKED);
    else
	CheckMenuItem(hMenu, MM_FIT2WND, MF_UNCHECKED);
    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessMMPrint
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessMMPrint(VOID)
{
    DWORD   dwThrdID;
    HANDLE  hThrd;
    PPRTDATA pPrtData;

    if (ghMetaf == 0) {
	SetWindowText(ghTextWnd, GetStringRes(IDS_NOMFFORPRT));
	return 0L;
    }

    //
    // bPrintMf is supposed to free up the memory when done.
    //
    if ((pPrtData = (PPRTDATA) GlobalAlloc(GPTR, sizeof(PRTDATA))) == NULL) {
	SetWindowText(ghTextWnd, GetStringRes(IDS_MALLOCFAIL2));
	return 0L;
    }

    pPrtData->hMetaf = ghMetaf;
    pPrtData->bFit2Wnd = gbFit2Wnd;

    hThrd = CreateThread(NULL, 0,
		 (LPTHREAD_START_ROUTINE)bPrintMf,
		 pPrtData, STANDARD_RIGHTS_REQUIRED,
		 &dwThrdID);

    //
    // Free the memory if CreateThread fails...
    //
    if (hThrd == NULL) {
	SetWindowText(ghTextWnd, GetStringRes(IDS_CREPRTTHRDFAIL));
	GlobalFree(pPrtData);
    }

    return 0L;
}

/******************************Public*Routine******************************\
*
* lProcessWmCommand
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

LONG lProcessWmCommand(HWND     hwnd,
                       UINT     message,
                       DWORD    wParam,
                       LONG     lParam,
                       INT      *piMetafCnt,
                       BOOL     *pbReset,
                       char     *szFilename,
                       char     *szLoadedMetaf)
{
    static int     iPlus=0;

    switch (LOWORD(wParam)) {
        case DID_ZERO:
        case DID_ONE:
        case DID_TWO:
        case DID_THREE:
        case DID_FOUR:
        case DID_FIVE:
        case DID_SIX:
        case DID_SEVEN:
        case DID_EIGHT:
        case DID_NINE:
            return lProcessDIDPlayStep(wParam, &iPlus);

        case DID_TEN_PLUS:
            return lProcessDIDPlayTenPlus(&iPlus);

        case MM_PAGESETUP:
        case MM_CUT:
            return 0L;

        case MM_COPY:
            return lProcessMMCopy();

        case MM_PASTE:
            return lProcessMMPaste(hwnd, pbReset);

        case MM_DEL:
            return lProcessMMDel();

        case MM_PEN:
            return lProcessMMPen(hwnd);

        case MM_BRUSH:
            return lProcessMMBrush(hwnd);

        case MM_FONT:
            return lProcessMMFont();

        case MM_TTOUTLN_STROKEFILL:
            return lProcessMMStrokeFill();

        case MM_TTOUTLN_POLYDRAW:
            return lProcessMMPolyDraw();

        case MM_C_WND_MF:
	    return 0L;

        case MM_C_BEGIN_GP:
        case MM_C_END_GP:
	    return 0L;

	case MM_C_MLTFMTS:
	    return lProcessMMMltfmts();

	case MM_HITTEST:
	    return lProcessMMHittest();

	case MM_LEABOUT:
	    return lProcessMMLEAbout();

	case MM_ABOUT:
	    return lProcessMMAbout();

	case MM_LOAD_MASKBMP:
	    return lProcessMMLoadMaskBmp(hwnd);

	case MM_LOAD_BMP:
	    return lProcessMMLoadBmp(hwnd);

	case MM_SAVE_BMP:
	    return lProcessMMSaveBmp();

        case MM_LOAD:
	case DID_OPEN:
	    return lProcessDIDOpen(hwnd, szLoadedMetaf, pbReset);

	case MM_RECORD:
	    return lProcessMMRec(hwnd, szFilename);

	case DID_RECORD:
	    return lProcessDIDRec(hwnd, piMetafCnt, szFilename);

	case DID_STOP:
	    return lProcessDIDStop();

	case DID_PLAY:
	    return lProcessDIDPlay(szLoadedMetaf);

	case DID_FF:
	    return lProcessDIDFf(pbReset);

	case DID_CLEAR:
	    return lProcessDIDClear();

        case DID_PEN:
            SetWindowText(ghTextWnd, "Pen");
            return 0L;
        case DID_TEXT:
            SetWindowText(ghTextWnd, "Text");
            return 0L;
        case DID_RECT:
            SetWindowText(ghTextWnd, "Rectangle");
            return 0L;
        case DID_FILLRECT:
            SetWindowText(ghTextWnd, "Filled Rectangle");
            return 0L;
        case DID_ELLIPSE:
            SetWindowText(ghTextWnd, "Ellipse");
            return 0L;
        case DID_FILLELLIPSE:
            SetWindowText(ghTextWnd, "Filled Ellipse");
            return 0L;
        case DID_LINE:
            SetWindowText(ghTextWnd, "Line");
            return 0L;
        case DID_BEZIER:
            SetWindowText(ghTextWnd,
                "Bezier: Click with Left button for placing control points");
            return 0L;
        case DID_BMPOBJ:
            SetWindowText(ghTextWnd,
                "Bitmap: Click three points for the destination of the bitmap");
            return 0L;
        case DID_METAF:
            SetWindowText(ghTextWnd,
                "External Metafile: Click three points for the destination of the Metafile");
            return 0L;
	case MM_IMPORT_3X:
	    return lProcessMMImport3X();

	case MM_EXPORT_3X:
	    return lProcessMMExport3X();

	case MM_FIT2WND:
	    return lProcessMMFit2Wnd();

	case MM_PRINT:
	    return lProcessMMPrint();

	default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

/***************************************************************************\
* MainWndProc
*
\***************************************************************************/

long APIENTRY MainWndProc(
    HWND hwnd,
    UINT message,
    DWORD wParam,
    LONG lParam)
{
    static int         iMetafCnt=0;
    static char        szFilename[256] = "c:\\metaf";
    static BOOL        bReset=FALSE;
    static char        szLoadedMetaf[256] = " ";

    switch (message) {

      case WM_CREATE: {
        return lProcessWmCreate(hwnd);
      }
      case WM_DRAWCLIPBOARD: {
        return lProcessWmDrawClip(message, wParam, lParam);
      }
      case WM_SIZE: {
        return lProcessWmSize(hwnd, message, wParam, lParam);
      }

      case WM_DESTROY: {
        return lProcessWmDestroy();
      }
      case WM_COMMAND: {
        return lProcessWmCommand(hwnd,
                                 message,
                                 wParam,
                                 lParam,
                                 &iMetafCnt,
                                 &bReset,
                                 szFilename,
                                 szLoadedMetaf);
      }
      default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}

/******************************Public*Routine******************************\
*
* DrawSurfWndProc
*       Drawing surface window procedure
*
* Effects:  Trapping all mouse messages and call the DrawStuff appropriately
*           for drawing to the drawing surface DC and metafile DC as needed.
*
* Warnings:
*
\**************************************************************************/

long APIENTRY DrawSurfWndProc(
    HWND hwnd,
    UINT message,
    DWORD wParam,
    LONG lParam)
{
    static BOOL    bTrack = FALSE;
    static int     OrgX, OrgY;
    static int     PrevX, PrevY;
    static HDC     hDC;
    static HCURSOR hCurArrow, hCurHT;
    static BOOL    b3DSetup = FALSE;

    switch (message) {
	case WM_CREATE: {
	    RECT                    rect;

	    hDC = GetDC(hwnd);

            {
            char text[128];

            wsprintf(text, "DrawSurfWndProc: hDC=%lx", hDC);
            vErrOut(text);
            }

            bSetAdvancedGraphics(hDC);

            if (gb3D)
                b3DSetup = bSetupRC(hDC, NULL);

	    ReleaseDC(hwnd, hDC);

	    GetClientRect(GetParent(hwnd), &rect);

	    SetWindowPos(hwnd, NULL,
		    0,
		    30,
		    rect.right-rect.left,
		    rect.bottom-rect.top-30,
		    SWP_NOZORDER | SWP_NOMOVE);
            CreateCaret(hwnd, NULL, 1, CARET_HEIGHT);
#if 0
	    //CreateCaret(hwnd, NULL, 1, 12);
	    //hbmp = LoadBitmap(ghModule, (LPCSTR)MAKEINTRESOURCE(BMID_TOOLBASED));

	    hPal = CreatePalette((LOGPALETTE *)&LogPal);
	    hDC = GetDC(hwnd);
	    hOldPal = SelectPalette(hDC, hPal, FALSE);
	    RealizePalette(hDC);

	    hbmp = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)&DIBHdr, CBM_CREATEDIB, &dib, (LPBITMAPINFO)&DIBHdr, DIB_PAL_COLORS);
	    CreateCaret(hwnd, hbmp, 1, 12);

	    ReleaseDC(hwnd, hDC);
#endif

	    ghCurFont = GetStockObject(SYSTEM_FONT);
	    GetObject(ghCurFont, sizeof(LOGFONT), &glf);
	    hCurArrow = LoadCursor(NULL, IDC_ARROW);
	    hCurHT = LoadCursor(NULL, IDC_CROSS);
	    break;
	}

	case WM_LBUTTONDOWN: {
	    int    x, y;

	    x = (int) LOWORD(lParam);
	    y = (int) HIWORD(lParam);

	    if (gbHitTest) {
		hDC = GetDC(hwnd);
		bHitTest(hDC, x, y);
		ReleaseDC(hwnd, hDC);
		break;
	    }

	    bTrack = TRUE;
	    OrgX = PrevX = x;
	    OrgY = PrevY = y;

	    hDC = GetDC(hwnd);
	    SetCapture(hwnd);
	    break;
	}

	case WM_MOUSEMOVE: {
	    RECT rectClient;
	    int NextX;
	    int NextY;

	    if (gbHitTest) {
		SetCursor(hCurHT);
	    } else {
		SetCursor(hCurArrow);
	    }

	    // Update the selection region
	    if (bTrack) {
		NextX = (SHORT) LOWORD(lParam);
		NextY = (SHORT) HIWORD(lParam);

		// Do not draw outside the window's client area

		GetClientRect (hwnd, &rectClient);
		if (NextX < rectClient.left) {
		    NextX = rectClient.left;
		} else if (NextX >= rectClient.right) {
		    NextX = rectClient.right - 1;
		}
		if (NextY < rectClient.top) {
		    NextY = rectClient.top;
		} else if (NextY >= rectClient.bottom) {
		    NextY = rectClient.bottom - 1;
		}
		if ((NextX != PrevX) || (NextY != PrevY)) {
		   SetROP2(hDC, R2_NOT);	   // Erases the previous box
		   bDrawStuff(hDC, OrgX, OrgY, PrevX, PrevY, TRUE, TRUE, FALSE, NULL);

		   //
		   // Optimization.  Do not record in metafile DC if it is going
		   // to be erased.  So only call bDrawStuff with the PEN tool.
		   //
		   if (gbRecording && (ghDCMetaf != NULL) && (gdwCurTool == DID_PEN)) {
		       bDrawStuff(ghDCMetaf, OrgX, OrgY, PrevX, PrevY, TRUE, TRUE, FALSE, NULL);
		   }


		// Get the current mouse position
		   PrevX = NextX;
		   PrevY = NextY;

		   //
		   // SetROP2(hDC, R2_COPYPEN);
		   //	This is commented out because we don't want to erase
		   //	the background as it sweeps.
		   //
		   bDrawStuff(hDC, OrgX, OrgY, PrevX, PrevY, FALSE, TRUE, FALSE, NULL);

		   if (gbRecording && (ghDCMetaf != NULL) && (gdwCurTool == DID_PEN)) {
		       bDrawStuff(ghDCMetaf, OrgX, OrgY, PrevX, PrevY, FALSE, TRUE, FALSE, NULL);
		   }

		}
	    }
	    break;

	}

      case WM_LBUTTONUP: {
        int NextX;
        int NextY;

        if (!bTrack)
           break;

        // End the selection
           ReleaseCapture();
           bTrack = FALSE;

        // Erases the box
           //
           // SetROP2(hDC, R2_NOT);
           //   This is assumed to be R2_NOT, thus unnecessary
           //
           bDrawStuff(hDC, OrgX, OrgY, PrevX, PrevY, TRUE, FALSE, FALSE, NULL);

           if (gbRecording && (ghDCMetaf != NULL) && (gdwCurTool == DID_PEN)) {
               bDrawStuff(ghDCMetaf, OrgX, OrgY, PrevX, PrevY, TRUE, FALSE, FALSE, NULL);
           }

           //
           // Apparently, at buttonup time, the mouse location is not clamped to
           // the client area.  So, the final ellipse or rectangle will be extended
           // outside the client area.
           //
           // NextX = (SHORT) LOWORD(lParam);
           // NextY = (SHORT) HIWORD(lParam);
           NextX = PrevX;
           NextY = PrevY;

        // Draws the new box
           SetROP2(hDC, R2_COPYPEN);
           bDrawStuff(hDC, OrgX, OrgY, NextX, NextY, FALSE, FALSE, TRUE, NULL);

           ReleaseDC(hwnd, hDC);

           if (gbRecording && (ghDCMetaf != NULL)) {
                bDrawStuff(ghDCMetaf, OrgX, OrgY, NextX, NextY, FALSE, FALSE, FALSE, NULL);
           }

        break;
      } // case WM_LBUTTONUP

#ifdef JAPAN
      case WM_IME_REPORT:
        if (IR_STRING == wParam) {
            LPSTR   lpsz, lpszMeta;

            if (!(lpsz = lpszMeta = GlobalLock((HANDLE)lParam))) {
                return FALSE;
            }
            hDC = GetDC(hwnd);
	    while(*lpsz) { // Draw all characters in the string
		bDrawStuff(hDC, 0, 0, 0, 0, TRUE, FALSE, FALSE, lpsz);
		IsDBCSLeadByte((BYTE)*lpsz) ? lpsz += 2 : ++lpsz;
	    }
            ReleaseDC(hwnd, hDC);
            if (gbRecording && (ghDCMetaf != NULL)) {
		while(*lpszMeta) { // Draw all characters in the string
		    bDrawStuff(ghDCMetaf, 0, 0, 0, 0, TRUE, FALSE, FALSE, lpszMeta);
		    IsDBCSLeadByte((BYTE)*lpszMeta) ? lpszMeta += 2 : ++lpszMeta;
		}
            }
            GlobalUnlock((HANDLE)lParam);
            return TRUE;
        }
	return DefWindowProc(hwnd, message, wParam, lParam);
        break;
#endif

      case WM_CHAR: {

        if (gdwCurTool != DID_TEXT)
            break;

        hDC = GetDC(hwnd);
        bDrawStuff(hDC, 0, 0, 0, 0, TRUE, FALSE, FALSE, (LPSTR)&wParam);
        ReleaseDC(hwnd, hDC);

        if (gbRecording && (ghDCMetaf != NULL)) {
            bDrawStuff(ghDCMetaf, 0, 0, 0, 0, TRUE, FALSE, FALSE, (LPSTR)&wParam);
        }

        break;
      }

      case WM_DESTROY: {
        HDC         hDC;
        HPALETTE    hPal;

        DestroyCaret();
        DeleteObject(ghCurFont);

        if (gb3D && b3DSetup) {
            bCleanUpRC();
        }

        hDC = GetDC(hwnd);
        hPal = SelectObject(hDC, GetStockObject(DEFAULT_PALETTE));
        DeleteObject(hPal);
        ReleaseDC(hwnd, hDC);

	PostQuitMessage(0);
	return 0L;
      }

      default:
	return DefWindowProc(hwnd, message, wParam, lParam);
    }
}


/***************************************************************************\
* About
*
* About dialog proc.
*
\***************************************************************************/

BOOL CALLBACK About (
    HWND hDlg,
    UINT message,
    DWORD wParam,
    LONG lParam)
{
    switch (message) {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (wParam == IDOK)
            EndDialog(hDlg, wParam);
        break;
    }

    return FALSE;

    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(hDlg);
}

/***************************************************************************\
*
* TextWndProc
*
* Text Window procedure for displaying miscellaneous messages to user.
*
\***************************************************************************/

LONG APIENTRY TextWndProc (HWND hwnd, UINT message, DWORD wParam, LONG lParam)
{
    static HFONT hFont = (HFONT) NULL;

    switch (message)
    {
    case WM_CREATE:
        {
	    LOGFONT    lf;
	    HDC        hDC;
	    HFONT      hOldFont;
            TEXTMETRIC tm;
	    //RECT       rect;

            SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(lf), (PVOID) &lf, (UINT)FALSE);

	    hDC = GetDC(hwnd);
	    // this is the height for 8 point size font in pixels
	    lf.lfHeight = 8 * GetDeviceCaps(hDC, LOGPIXELSY) / 72;

	    hFont = CreateFontIndirect(&lf);
	    hOldFont = SelectObject(hDC, hFont);
	    GetTextMetrics(hDC, &tm);

	    // base the height of the window on size of text
	    glcyStatus = tm.tmHeight+6*GetSystemMetrics(SM_CYBORDER)+2;
            ReleaseDC(hwnd, hDC);
            break;
        }

    case WM_DESTROY:
	    if (hFont)
		DeleteObject(hFont);
	    break;

    case WM_SETTEXT:
            DefWindowProc(hwnd, message, wParam, lParam);
            InvalidateRect(hwnd,NULL,FALSE);
            UpdateWindow(hwnd);
            return 0L;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            RECT   rc;
            char   ach[128];
            int    len, nxBorder, nyBorder;
            HFONT  hOldFont = NULL;

            BeginPaint(hwnd, &ps);

            GetClientRect(hwnd,&rc);

            nxBorder = GetSystemMetrics(SM_CXBORDER);
	    rc.left  += 9*nxBorder;
            rc.right -= 9*nxBorder;

            nyBorder = GetSystemMetrics(SM_CYBORDER);
	    rc.top    += 3*nyBorder;
	    rc.bottom -= 3*nyBorder;

	    // 3D Text
            len = GetWindowText(hwnd, ach, sizeof(ach));
	    SetBkColor(ps.hdc, GetSysColor(COLOR_BTNFACE));

	    SetBkMode(ps.hdc, TRANSPARENT);
	    SetTextColor(ps.hdc, RGB(64,96,96));
	    if (hFont)
		hOldFont = SelectObject(ps.hdc, hFont);
	    ExtTextOut(ps.hdc, rc.left+2*nxBorder+2, rc.top+2, ETO_OPAQUE | ETO_CLIPPED,
	                &rc, ach, len, NULL);

	    SetTextColor(ps.hdc, RGB(128,128,128));
  	    if (hFont)
		hOldFont = SelectObject(ps.hdc, hFont);
	    ExtTextOut(ps.hdc, rc.left+2*nxBorder+1, rc.top+1, ETO_CLIPPED,
			&rc, ach, len, NULL);

	    SetTextColor(ps.hdc, RGB(255,255,255));
	    if (hFont)
		hOldFont = SelectObject(ps.hdc, hFont);
	    ExtTextOut(ps.hdc, rc.left+2*nxBorder, rc.top, ETO_CLIPPED,
			&rc, ach, len, NULL);

	    SetBkMode(ps.hdc, OPAQUE);

	    if (hOldFont)
		SelectObject(ps.hdc, hOldFont);

            EndPaint(hwnd, &ps);
            return 0L;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

/******************************Public*Routine******************************\
*
* CtrlPanelDlgProc
*       The Control Panel dialog procedure
*
* Effects:  Responsible for drawing the owner draw buttons.  Notifying
*           parent of user's action.
*
* Warnings:
*
\**************************************************************************/

LONG APIENTRY CtrlPanelDlgProc(HWND hwnd, UINT msg, DWORD dwParam, LONG lParam)
{
    switch (msg) {
        case WM_INITDIALOG: {
            int index;

            for (index = 0; index < OD_BTN_CNT; index++) {
                grHwndCtrlBtn[index] = (PVOID)GetDlgItem(hwnd, (INT)(ID_OD_BTN_BASE+index));
            }
            for (index = 0; index < OD_TOOL_CNT; index++) {
                grHwndToolBtn[index] = (PVOID)GetDlgItem(hwnd, (INT)(ID_OD_TOOL_BASE+index));
            }
            return TRUE;
        }

        case WM_DRAWITEM: {
            PDRAWITEMSTRUCT pDIS = (PDRAWITEMSTRUCT) lParam;
            HBITMAP hBmpOld;
            BITMAP  bm;
            HANDLE  hCtl;
            HDC     hDCCtl;

            if (pDIS->CtlID == gdwCurCtrl) {
                GetObject((HBITMAP) ghBmpDn[pDIS->CtlID - ID_OD_BTN_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP) ghBmpDn[pDIS->CtlID - ID_OD_BTN_BASE]);
            }

            if (pDIS->CtlID == gdwCurTool) {
                GetObject((HBITMAP)ghToolBmpDn[pDIS->CtlID - ID_OD_TOOL_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP)ghToolBmpDn[pDIS->CtlID - ID_OD_TOOL_BASE]);
            }

            if ((pDIS->CtlID < ID_OD_TOOL_BASE) && (pDIS->CtlID != gdwCurCtrl)) {
                GetObject((HBITMAP)ghBmpUp[pDIS->CtlID - ID_OD_BTN_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP)ghBmpUp[pDIS->CtlID - ID_OD_BTN_BASE]);
            }

            if ((pDIS->CtlID >= ID_OD_TOOL_BASE) && (pDIS->CtlID != gdwCurTool)) {
                GetObject((HBITMAP)ghToolBmpUp[pDIS->CtlID - ID_OD_TOOL_BASE], sizeof(BITMAP), (LPSTR)&bm);
                hBmpOld = SelectObject(ghDCMem, (HBITMAP)ghToolBmpUp[pDIS->CtlID - ID_OD_TOOL_BASE]);
            }

            //
            // pDIS->hDC is clipped to the update region but unfortunately
            // that doesn't work well with StretchBlt.  StretchBlt is used
            // because I don't have to make sure that the bitmap size is
            // exactly the same as the size of the button.
            //
            hCtl   = GetDlgItem(hwnd, pDIS->CtlID);
            hDCCtl = GetDC(hCtl);
            StretchBlt(hDCCtl,                                //pDIS->hDC,
                   pDIS->rcItem.left, pDIS->rcItem.top,
                   pDIS->rcItem.right - pDIS->rcItem.left,
                   pDIS->rcItem.bottom - pDIS->rcItem.top,
                   ghDCMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
            ReleaseDC(hCtl, hDCCtl);
            SelectObject(ghDCMem, hBmpOld);
            break;
        }

        case WM_COMMAND: {
            DWORD dwOldCtrl = gdwCurCtrl;
            DWORD dwOldTool = gdwCurTool;

            switch (dwParam) {
                case DID_ONE:
                case DID_TWO:
                case DID_THREE:
                case DID_FOUR:
                case DID_FIVE:
                case DID_SIX:
                case DID_SEVEN:
                case DID_EIGHT:
                case DID_NINE:
                case DID_ZERO:
                case DID_TEN_PLUS:
                    //SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, dwParam - DID_ZERO, FALSE);
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    break;
                case DID_OPEN:
                case DID_RECORD:
                case DID_STOP:
                case DID_PLAY:
                case DID_FF:
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    gdwCurCtrl = dwParam;
                    InvalidateRect((HWND)grHwndCtrlBtn[dwOldCtrl - ID_OD_BTN_BASE], NULL, FALSE);
                    InvalidateRect((HWND)grHwndCtrlBtn[gdwCurCtrl - ID_OD_BTN_BASE], NULL, FALSE);
                    break;
                case DID_CLEAR:
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    break;
                case DID_TEXT:
                case DID_PEN:
                case DID_RECT:
                case DID_FILLRECT:
                case DID_ELLIPSE:
                case DID_FILLELLIPSE:
                case DID_LINE:
                case DID_BEZIER:
                case DID_BMPOBJ:
                case DID_METAF:
                    SendMessage(ghwndMain, WM_COMMAND, dwParam, lParam);
                    gdwCurTool = dwParam;
                    InvalidateRect((HWND)grHwndToolBtn[dwOldTool - ID_OD_TOOL_BASE], NULL, FALSE);
                    InvalidateRect((HWND)grHwndToolBtn[gdwCurTool - ID_OD_TOOL_BASE], NULL, FALSE);
                    break;
            }
            break;
        }


        case WM_PAINT:
            {
                HDC hdc;
                RECT rc, rcDlg;
                PAINTSTRUCT ps;
                HPEN hpenWindowFrame, hpenDarkGray;
                int  icyDlg;
                int  icyBorder;

                icyBorder = GetSystemMetrics(SM_CYBORDER);

                GetWindowRect(hwnd, &rcDlg);
                icyDlg = rcDlg.right - rcDlg.left;

                /*
                 * Draw our border lines.
                 */
                GetClientRect(hwnd, &rc);
                hdc = BeginPaint(hwnd, &ps);

                SelectObject(hdc, GetStockObject(WHITE_PEN));
                MoveToEx(hdc, rc.left, rc.top, NULL);
                LineTo(hdc, rc.right, rc.top);

                hpenDarkGray = CreatePen(PS_SOLID, 1, DARKGRAY);
                SelectObject(hdc, hpenDarkGray);
                MoveToEx(hdc, rc.left, (rc.top + icyDlg) - icyBorder - 1, NULL);
                LineTo(hdc, rc.right, (rc.top + icyDlg) - icyBorder - 1);

                hpenWindowFrame = CreatePen(PS_SOLID, icyBorder,
                        GetSysColor(COLOR_WINDOWFRAME));
                SelectObject(hdc, hpenWindowFrame);
                MoveToEx(hdc, rc.left, (rc.top + icyDlg) - icyBorder, NULL);
                LineTo(hdc, rc.right, (rc.top + icyDlg) - icyBorder);

                EndPaint(hwnd, &ps);
                DeleteObject(hpenWindowFrame);
                DeleteObject(hpenDarkGray);
            }

            break;


        //case WM_CTLCOLOR:
        case WM_CTLCOLORDLG:
        //case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSTATIC:
            switch (GET_WM_CTLCOLOR_TYPE(dwParam, lParam, msg)) {
                case CTLCOLOR_DLG:
                //case CTLCOLOR_LISTBOX:
                    return (BOOL)GetStockObject(LTGRAY_BRUSH);

                case CTLCOLOR_STATIC:
                    SetBkMode(GET_WM_CTLCOLOR_HDC(dwParam, lParam, msg),
                              TRANSPARENT);
                //    SetTextColor(GET_WM_CTLCOLOR_HDC(dwParam, lParam, msg),
                //              RGB(255,0,0));
                //    SetBkColor(GET_WM_CTLCOLOR_HDC(dwParam, lParam, msg),
                //            LIGHTGRAY);
                //              RGB(255, 255,0));
                    return (BOOL)GetStockObject(DKGRAY_BRUSH);
            }
            //return (BOOL)NULL;
            return (BOOL)GetStockObject(LTGRAY_BRUSH);

        default:
            return FALSE;
    }

    return FALSE;
}



/******************************Public*Routine******************************\
*
* bDrawStuff
*
* Effects:  The drawing routines are localized here.
*           bErase is TRUE if this fcn is called for erasing previous object.
*           (as in tracking objects.)  It is FALSE, otherwise.
*
*           bMove is TRUE if this fcn is called inside the WM_MOUSEMOVE (as
*           in tracking objects.)  It is FALSE, otherwise.
*
*           bCntPt is TRUE if this fcn is to increment either the iCnt or
*           iCntMF counter (used only in processing metafile or bezier.)
*           It is FALSE, otherwise.
*
*           lpstr contains the character to be drawn by TextOut when it is
*           not NULL.
*
* Warnings: Metafile and Bezier assume that the caller is calling this fcn
*           to draw in the screen DC first. Then draw it to the metafile DC.
*           Thus, when it is called to draw on the metafile DC, the points
*           would have been set already.
*
\**************************************************************************/

BOOL bDrawStuff(HDC hDC, INT OrgX, INT OrgY,
                         INT NextX, INT NextY,
                         BOOL bErase,
                         BOOL bMove,
                         BOOL bCntPt,
                         LPSTR lpstr) {
    BOOL bSuccess;
    HGDIOBJ hObjOld;
    static POINT rgPts[MAX_POINTS], rgPtsMF[MAX_POINTS_MF], rgPtsBMP[MAX_POINTS_BMP];
    static int   iCnt=0, iCntMF=0, iCntBMP=0;
    static BOOL  bCaretShown=FALSE;

    bSuccess = TRUE;
    if (bCaretShown) {
        HideCaret(ghwndDrawSurf);
        bCaretShown = FALSE;
    }

    switch (gdwCurTool) {
        case DID_PEN:
            if (bErase) {
                MoveToEx(hDC, NextX, NextY, NULL);
            } else {
                //
                // Override the ROP2 st. the pen won't erase its track
                //
                SetROP2(hDC, R2_COPYPEN);
                LineTo(hDC, NextX, NextY);
            }
            break;
        case DID_TEXT: {
            POINT   Pt;
#if 0
	    HDC hDCMem;
#endif

            if (lpstr == NULL) {
                ShowCaret(ghwndDrawSurf);
                bCaretShown = TRUE;
                SetCaretPos(NextX, NextY-CARET_HEIGHT);
                MoveToEx(hDC, NextX, NextY, NULL);

#if 0
		StretchDIBits(hDC, 20, 20+120, 64, 64, 0, 64, 64, -64, &dib,
							(LPBITMAPINFO)&DIBHdr, DIB_PAL_COLORS, SRCCOPY);

		hDCMem = CreateCompatibleDC(hDC);
		SelectPalette(hDCMem, hPal, FALSE);
		RealizePalette(hDCMem);
		SelectObject(hDCMem, hbmp);
		BitBlt(hDC, 0,0,64,64,hDCMem, 0,0,SRCCOPY);
		DeleteDC(hDCMem);
#endif

		    SetFocus(ghwndDrawSurf);
		    break;
            }

            SetTextAlign(hDC, TA_BASELINE | TA_LEFT | TA_UPDATECP);
            hObjOld = SelectObject(hDC, ghCurFont);
            SetTextColor(hDC, gCrText);

            if ((gbSFOutln || gbPDOutln) && gbTT) {
                // get rid of the char box
                SetBkMode(hDC, TRANSPARENT);
                BeginPath(hDC);
                TextOut(hDC, NextX, NextY, lpstr, IsDBCSLeadByte((BYTE)*lpstr) ? 2 : 1);
                EndPath(hDC);

                if (gbSFOutln) {
                    StrokeAndFillPath(hDC);
                    goto DT_UPDATE;
                }

                //
                // Get path and polydraw
                //
                {
                int     iNumPt;
                PBYTE   pjTypes;
                PPOINT  pPts;

                if (iNumPt = GetPath(hDC, NULL, NULL, 0)) {
                    if ((pPts = (PPOINT)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                                sizeof(POINT)*iNumPt )) == NULL) {
                        MessageBox(ghwndMain, GetStringRes(IDS_CREPNTFAIL),
                            GetStringRes(IDS_ERROR), MB_OK);
                        break;
                    }

                    if ((pjTypes = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                                                sizeof(BYTE)*iNumPt )) == NULL) {
                        MessageBox(ghwndMain, GetStringRes(IDS_CREPBYTEFAIL),
                            GetStringRes(IDS_ERROR), MB_OK);
                        goto GP_EXIT1;
                    }

                    GetPath(hDC, pPts, pjTypes, iNumPt);
                }
                PolyDraw95(hDC, pPts, pjTypes, iNumPt);
                LocalFree(pjTypes);

GP_EXIT1:
                LocalFree(pPts);

                }

            } else {
                TextOut(hDC, NextX, NextY, lpstr, IsDBCSLeadByte((BYTE)*lpstr) ? 2 : 1);
            }
DT_UPDATE:
            //
            // Updating current position
            //
            {
            LONG    lHeight;
            LONG    lWidth;
            TEXTMETRIC     tm;

                if (GetTextMetrics(hDC, &tm)) {
                    lHeight = tm.tmHeight;
                    lWidth  = tm.tmMaxCharWidth;
                }

                GetCurrentPositionEx(hDC, (LPPOINT) &Pt);
                SetCaretPos(Pt.x, Pt.y-CARET_HEIGHT);

            }
            ShowCaret(ghwndDrawSurf);
            bCaretShown = TRUE;

            break;

#if 0

#define PT_LINECLOSE     (PT_LINETO | PT_CLOSEFIGURE)
#define PT_BEZIERCLOSE (PT_BEZIERTO | PT_CLOSEFIGURE)

            hpnRed = CreatePen(PS_SOLID, 0, RGB(255,0,0));
            SelectObject(hDC, hpnRed);

            while (iNumPt--) {
                static POINT pPnt[3];
                static int   iCnt=0;

                switch (*pjTypes++) {
                    case PT_MOVETO: {
                        MoveToEx(hDC, pPts->x, pPts->y, NULL);
                        pPts++;
                        break;
                    }
                    case PT_LINETO: {
                        LineTo(hDC, pPts->x, pPts->y);
                        pPts++;
                        break;

                    }
                    case PT_LINECLOSE: {
                        LineTo(hDC, pPts->x, pPts->y);
                        pPts++;
                        goto GP_EXIT2;
                    }
                    case PT_BEZIERTO: {
                        pPnt[iCnt].x = pPts->x;
                        pPnt[iCnt].y = pPts->y;
                        pPts++;

                        if (iCnt < 2) {
                            iCnt++;
                        } else {
                            PolyBezierTo(hDC, pPnt, 3);
                            iCnt = 0;
                        }
                        break;
                    }
                    case PT_BEZIERCLOSE: {
                        pPnt[iCnt].x = pPts->x;
                        pPnt[iCnt].y = pPts->y;
                        pPts++;

                        if (iCnt < 2) {
                            iCnt++;
                        } else {
                            PolyBezierTo(hDC, pPnt, 3);
                            iCnt = 0;
                        }
                        goto GP_EXIT2;
                    }

                    default:
                        break;
                }

            }

#endif
        }
        case DID_RECT:
            hObjOld = SelectObject(hDC, GetStockObject(NULL_BRUSH));
            Rectangle(hDC, OrgX, OrgY, NextX, NextY);
            SelectObject(hDC, hObjOld);
            break;
        case DID_FILLRECT:
            Rectangle(hDC, OrgX, OrgY, NextX, NextY);
            break;
        case DID_ELLIPSE:
            hObjOld = SelectObject(hDC, GetStockObject(NULL_BRUSH));
            Ellipse(hDC, OrgX, OrgY, NextX, NextY);
            SelectObject(hDC, hObjOld);
            break;
        case DID_FILLELLIPSE:
            Ellipse(hDC, OrgX, OrgY, NextX, NextY);
            break;
        case DID_LINE:
            MoveToEx(hDC, OrgX, OrgY, NULL);
            LineTo(hDC, NextX, NextY);
            break;
        case DID_BEZIER:
            if (bErase || bMove)
                return bSuccess;

            if (bCntPt) {
                rgPts[iCnt].x = NextX;
                rgPts[iCnt].y = NextY;
                iCnt++;

                if (iCnt == MAX_POINTS - 1)
                    iCnt = 0;
            }

            if ((iCnt % 3) == 1) {              // (iCnt + 1) % 3 == 1
                //
                // Override the ROP2 st. the pen won't erase its track
                //
                SetROP2(hDC, R2_COPYPEN);
                PolyBezier(hDC, (LPPOINT)&rgPts, (DWORD) iCnt);
            }
            return bSuccess;

        case DID_BMPOBJ: {
            static BOOL          bBltReady = FALSE;

            if (bErase || bMove)
                return bSuccess;

            if (ghBmp == NULL) {
                SetWindowText(ghTextWnd, GetStringRes(IDS_NOBMFOREMBED));
                return bSuccess;
            }

            if (bCntPt) {
                bBltReady = FALSE;
                rgPtsBMP[iCntBMP].x = NextX;
                rgPtsBMP[iCntBMP].y = NextY;
                iCntBMP++;

                if (iCntBMP < MAX_POINTS_BMP) {
                    return bSuccess;
                }
            } else {
                //
                // Caller don't want to increment counter, so must be doing
                // recording, so we just Blt again...
                //
                // But, if the Blt data is no good, bail out...
                //
                if (!bBltReady) {
                    return bSuccess;
                }
                bPlgBlt(hDC, rgPtsBMP);
                return bSuccess;
            }
            bBltReady = TRUE;

            bPlgBlt(hDC, rgPtsBMP);
            iCntBMP = 0;                         // reset
            return bSuccess;
        }

        case DID_METAF: {
            ENHMETAHEADER EnhMetaHdr;
            RECT          rcClientDS;
            static XFORM         xform;
            static BOOL          bXformReady = FALSE;
            int           iEntries;
            PLOGPALETTE     plogPal;
            PBYTE           pjTmp;
            HPALETTE        hPal;
            static BOOL     bRCSet = FALSE;


            if (bErase || bMove)
                return bSuccess;

            if (ghMetaf == NULL) {
                SetWindowText(ghTextWnd, GetStringRes(IDS_NOMFFOREMBED));
                return bSuccess;
            }

            if (bCntPt) {
                bXformReady = FALSE;
                rgPtsMF[iCntMF].x = NextX;
                rgPtsMF[iCntMF].y = NextY;
                iCntMF++;

                if (iCntMF < MAX_POINTS_MF) {
                    return bSuccess;
                }
            } else {
                //
                // Caller don't want to increment counter, so must be doing
                // recording, so we just set xform and play it again...
                //
                // But, if the xform data is no good, bail out...
                //
                if (!bXformReady) {
                    return bSuccess;
                }

                GetEnhMetaFileHeader(ghMetaf, sizeof(ENHMETAHEADER), &EnhMetaHdr);
                SetWorldTransform(hDC, &xform);
                GetClientRect(ghwndDrawSurf, &rcClientDS);

                iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

                if (iEntries) {
                    if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                            sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
                        MessageBox(ghwndMain, GetStringRes(IDS_CREPALFAIL),
                            GetStringRes(IDS_ERROR), MB_OK);
                    }

                    plogPal->palVersion = 0x300;
                    plogPal->palNumEntries = (WORD) iEntries;
                    pjTmp = (PBYTE) plogPal;
                    pjTmp += 4;

                    GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
                    hPal = CreatePalette(plogPal);
                    GlobalFree(plogPal);

                    SelectPalette(hDC, hPal, FALSE);
                    RealizePalette(hDC);
                }

                bRCSet = bSetRC2MatchEmfRC(hDC, EnhMetaHdr, ghMetaf, NULL);
                if (!bRCSet) {
                    vErrOut("DID_METAF failed");
                }

                //PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rcClientDS);
                {
                RECT rc;

                rc.top = rc.left = 0;
                rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
                rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
                if (!PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rc)) {
                    char    text[128];

                    wsprintf(text, GetStringRes(IDS_FMT_PLAYEMFFAIL), GetLastError());
                    OutputDebugString(text);
                } else {
                    if (gbDB)
                       SwapBuffers(hDC);
                }

                }
                ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);

                if (bRCSet) {
                    bCleanUpRC();
                    bRCSet = FALSE;
                }

                return bSuccess;
            }

            GetEnhMetaFileHeader(ghMetaf, sizeof(ENHMETAHEADER), &EnhMetaHdr);
            //
            // Based on the three points, top-left, top-right and bottom-left
            // (in this order), of the destination, solve equations for the
            // elements of the transformation matrix.
            //
            xform.eDx = (float) rgPtsMF[0].x;
            xform.eDy = (float) rgPtsMF[0].y;
            xform.eM11 = (rgPtsMF[1].x - xform.eDx)/(EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left);
            xform.eM12 = (rgPtsMF[1].y - xform.eDy)/(EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left);
            xform.eM21 = (rgPtsMF[2].x - xform.eDx)/(EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top);
            xform.eM22 = (rgPtsMF[2].y - xform.eDy)/(EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top);

            bXformReady = TRUE;
            SetWorldTransform(hDC, &xform);
            GetClientRect(ghwndDrawSurf, &rcClientDS);

            iEntries = GetEnhMetaFilePaletteEntries(ghMetaf, 0, NULL);

            if (iEntries) {
                if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                        sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
                    MessageBox(ghwndMain, GetStringRes(IDS_CREPALFAIL),
                        GetStringRes(IDS_ERROR), MB_OK);
                }

                plogPal->palVersion = 0x300;
                plogPal->palNumEntries = (WORD) iEntries;
                pjTmp = (PBYTE) plogPal;
                pjTmp += 4;

                GetEnhMetaFilePaletteEntries(ghMetaf, iEntries, (PPALETTEENTRY)pjTmp);
                hPal = CreatePalette(plogPal);
                GlobalFree(plogPal);

                SelectPalette(hDC, hPal, FALSE);
                RealizePalette(hDC);
            }

            if (!bRCSet) {
                bRCSet = bSetRC2MatchEmfRC(hDC, EnhMetaHdr, ghMetaf, NULL);
                if (!bRCSet) {
                    vErrOut("DID_METAF failed");
                }
            }

            //PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rcClientDS);
            {
            RECT rc;

            rc.top = rc.left = 0;
            rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
            rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
            if (!PlayEnhMetaFile(hDC, ghMetaf, (LPRECT) &rc)) {
                 char    text[128];

                 wsprintf(text, GetStringRes(IDS_FMT_PLAYEMFFAIL), GetLastError());
                 OutputDebugString(text);
            } else {
                if (gbDB)
                   SwapBuffers(hDC);
            }

            }
            ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
            iCntMF = 0;                         // reset
            if (bRCSet) {
                bCleanUpRC();
                bRCSet = FALSE;
            }
            return bSuccess;
        }
        default:
            break;
    }
    //
    // Reset counter, user has selected other tools.
    //
    iCnt = 0;
    iCntMF = 0;
    iCntBMP = 0;
    return bSuccess;
}

/******************************Public*Routine******************************\
*
* hemfLoadMetafile
*
* Effects:   Brings up the Open file common dialog
*            Get the enhanced metafile spec'd by user
*            returns the handle to the enhanced metafile if successfull
*               otherwise, returns 0.
*
* Warnings:
*
\**************************************************************************/

HENHMETAFILE hemfLoadMetafile(HWND hwnd) {
    OPENFILENAME    ofn;
    char            szFile[256], szFileTitle[256];
    static char     *szFilter;

    HMETAFILE       hmf;
    UINT            uiSize;
    LPVOID          pvData;
    HDC             hDCDrawSurf;
    HENHMETAFILE    hemf;

    HANDLE                  hFile, hMapFile;
    LPVOID                  pMapFile;
    LPENHMETAHEADER         pemh;

    BOOL        bSuccess;
    char            text[128];


    bSuccess = TRUE;

    BuildFilterStrs(-1);
    BuildFilterStrs(IDS_FT_EMF);
  	szFilter = BuildFilterStrs(IDS_FT_WMF);

    strcpy(szFile, "*.emf\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = GetStringRes(IDS_LDMF);
    ofn.Flags = 0L;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "EMF";

    if (!GetOpenFileName(&ofn))
        return 0L;

    if ((hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) == (HANDLE)-1) {
        wsprintf(text, GetStringRes(IDS_FMT_FILEOPNFAIL), GetLastError());
        MessageBox(ghwndMain, text, GetStringRes(IDS_ERROR), MB_OK);
        return 0L;
    }

    //
    // Create a map file of the opened file
    //
    if ((hMapFile = CreateFileMapping(hFile, NULL,
                             PAGE_READONLY, 0, 0, "MapF")) == NULL) {
        wsprintf(text, GetStringRes(IDS_FMT_CREMAPFILEFAIL), GetLastError());
        MessageBox(ghwndMain, text, GetStringRes(IDS_ERROR), MB_OK);
        bSuccess = FALSE;
        goto ErrorExit1;
    }

    //
    // Map a view of the whole file
    //
    if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0)) == NULL) {
        wsprintf(text, GetStringRes(IDS_FMT_MAPVWMAPFOFAIL), GetLastError());
        MessageBox(ghwndMain, text, GetStringRes(IDS_ERROR), MB_OK);
        bSuccess = FALSE;
        goto ErrorExit2;
    }

    //
    // First check that if it is an enhanced metafile
    //
    pemh = (LPENHMETAHEADER) pMapFile;
    if (pemh->dSignature == META32_SIGNATURE) {
        hemf = GetEnhMetaFile(szFile);
        goto HLM_EXIT;
    }

    //
    // If it has an ALDUS header skip it
    // Notice: APMSIZE is used because the HANDLE and RECT of the structure
    //         depends on the environment
    //
    if (*((LPDWORD)pemh) == ALDUS_ID) {
        //METAFILEPICT    mfp;

        MessageBox(ghwndMain, GetStringRes(IDS_ALDUSMF), "Hey!", MB_OK);
        uiSize = *((LPDWORD) ((PBYTE)pMapFile + APMSIZE + 6));
        hDCDrawSurf = GetDC(ghwndDrawSurf);

        // Notice: mtSize is size of the file in word.
        // if LPMETAFILEPICT is NULL
        //    MM_ANISOTROPIC mode and default device size will be used.
        hemf = SetWinMetaFileBits(uiSize*2L, (PBYTE)pMapFile + APMSIZE, hDCDrawSurf, NULL);
#if 0
        switch ( ((PAPMFILEHEADER) pMapFile)->inch ) {
            // !!! End up in an upside down image
            //
            case 1440:
                mfp.mm = MM_TWIPS;
                OutputDebugString("MM_TWIPS\n");
                break;
            case 2540:
                OutputDebugString("MM_HIMETRIC\n");
                mfp.mm = MM_HIMETRIC;
                break;
            case 254:
                OutputDebugString("MM_LOMETRIC\n");
                mfp.mm = MM_LOMETRIC;
                break;
            case 1000:
                OutputDebugString("MM_HIENGLISH\n");
                mfp.mm = MM_HIENGLISH;
                break;
            case 100:
                OutputDebugString("MM_LOENGLISH\n");
                mfp.mm = MM_LOENGLISH;
                break;
            default:
                // !!! In addition, text is too small
                //
                OutputDebugString("MM_ANISOTROPIC\n");
                mfp.mm = MM_ANISOTROPIC;
                mfp.xExt = (((PAPMFILEHEADER) pMapFile)->bbox.Right - ((PAPMFILEHEADER) pMapFile)->bbox.Left)
                           * ((PAPMFILEHEADER) pMapFile)->inch * 2560;
                mfp.yExt = (((PAPMFILEHEADER) pMapFile)->bbox.Bottom - ((PAPMFILEHEADER) pMapFile)->bbox.Top)
                           * ((PAPMFILEHEADER) pMapFile)->inch * 2560;
                break;
        }
        mfp.hMF = 0;
        hemf = SetWinMetaFileBits(uiSize*2L, (PBYTE)pMapFile + APMSIZE, hDCDrawSurf, &mfp);
#endif

        if (!hemf) {
            char text[256];

            wsprintf(text, GetStringRes(IDS_FMT_SETWMFBTSFAIL), GetLastError());
            MessageBox(ghwndMain, text, GetStringRes(IDS_ERROR), MB_OK);
        }

        ghmf = SetMetaFileBitsEx(uiSize*2L, (PBYTE)pMapFile + APMSIZE);
        if (!ghmf) {
            char text[256];

            wsprintf(text, GetStringRes(IDS_FMT_SETWMFBTSXFAIL), GetLastError());
            MessageBox(ghwndMain, text, GetStringRes(IDS_ERROR), MB_OK);
        }

// !!! Displaying the Windows format metafile
//if (!PlayMetaFile(hDCDrawSurf, ghmf)) {
//    wsprintf(text, GetStringRes(IDS_FMT_PLAYMFFAIL), GetLastError());
//    MessageBox(ghwndMain, text, GetStringRes(IDS_ERROR), MB_OK);
//}
        ReleaseDC(ghwndDrawSurf, hDCDrawSurf);
        goto HLM_EXIT;
    }


    //
    // It is a Windows 3x format metafile (hopefully)
    //
    if (!(hmf = GetMetaFile((LPCSTR)szFile))) {
        char text[256];

        wsprintf(text, GetStringRes(IDS_FMT_GETMFFAIL), GetLastError());
        MessageBox(ghwndMain, text, GetStringRes(IDS_ERROR), MB_OK);
        bSuccess = FALSE;
        goto ErrorExit3;
    }

    if (!(uiSize = GetMetaFileBitsEx(hmf, 0, NULL))) {
        MessageBox(ghwndMain, GetStringRes(IDS_GETMFBTSXFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
        return NULL;
    }

    if ((pvData = (LPVOID) LocalAlloc(LMEM_FIXED, uiSize)) == NULL) {
        MessageBox(ghwndMain, GetStringRes(IDS_MALLOCFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
        bSuccess = FALSE;
        goto ErrorExit3;
    }

    if (!(uiSize = GetMetaFileBitsEx(hmf, uiSize, pvData))) {
        MessageBox(ghwndMain, GetStringRes(IDS_GETMFBTSXFAIL2),
            GetStringRes(IDS_ERROR), MB_OK);
        bSuccess = FALSE;
        goto ErrorExit3;
    }

    DeleteMetaFile(hmf);

    hDCDrawSurf = GetDC(ghwndDrawSurf);
    hemf = SetWinMetaFileBits(uiSize, (LPBYTE)pvData, hDCDrawSurf, NULL);
    ghmf = SetMetaFileBitsEx(uiSize, (LPBYTE) pvData);

    LocalFree(pvData);

    ReleaseDC(ghwndDrawSurf ,hDCDrawSurf);

HLM_EXIT:
ErrorExit3:
    UnmapViewOfFile(pMapFile);

ErrorExit2:
    CloseHandle(hMapFile);
ErrorExit1:
    CloseHandle(hFile);

    if (bSuccess)
        return hemf;
    else
        return 0L;
}

/******************************Public*Routine******************************\
*
* hDCRecordMetafileAs
*
* Effects:   Brings up the SaveAs common dialog
*            Creates the enhanced metafile with the filename spec'd by user
*            Modifies the second arg to reflect the new default filename
*            less extension
*            returns the created metafile DC if successful, otherwise, 0
*
* Warnings:
*
\**************************************************************************/

HDC hDCRecordMetafileAs(HWND hwnd, LPSTR szFilename) {
    OPENFILENAME ofn;
    char szFile[256], szFileTitle[256];
    static char *szFilter;
    char *szTmp, szTmp2[256];
    HDC  hDCMeta;

    int iWidthMM, iHeightMM, iWidthPels, iHeightPels, iMMPerPelX, iMMPerPelY;
    RECT rc;
    HDC hDC;

    BuildFilterStrs(-1);
    szFilter = BuildFilterStrs(IDS_FT_EMF);
    strcpy(szFile, "*.emf\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 0L;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = GetStringRes(IDS_SAVEMFAS);
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = (LPSTR)NULL;

    if (!GetSaveFileName(&ofn)) {
        return 0L;
    }


    hDC = GetDC(hwnd);
    iWidthMM = GetDeviceCaps(hDC, HORZSIZE);
    iHeightMM = GetDeviceCaps(hDC, VERTSIZE);
    iWidthPels = GetDeviceCaps(hDC, HORZRES);
    iHeightPels = GetDeviceCaps(hDC, VERTRES);
    ReleaseDC(hwnd, hDC);
    iMMPerPelX = (iWidthMM * 100)/iWidthPels;
    iMMPerPelY = (iHeightMM * 100)/iHeightPels;
    GetClientRect(ghwndDrawSurf, &rc);
    rc.left = rc.left * iMMPerPelX;
    rc.top = rc.top * iMMPerPelY;
    rc.right = rc.right * iMMPerPelX;
    rc.bottom = rc.bottom * iMMPerPelY;


    //hDCMeta = CreateEnhMetaFile((HDC)NULL, szFile, (LPRECT)NULL, (LPSTR)NULL);
    {
        CHAR    szDesc[256];

        wsprintf(szDesc, "SDK Enhanced Metafile Editor\\0%s\\0\\0", szFileTitle);
        hDCMeta = CreateEnhMetaFile((HDC)NULL, szFile, (LPRECT)&rc, (LPSTR)szDesc);

        if (gb3D)
            gbRCSet = bSetupRC(hDCMeta, NULL);

        bSetAdvancedGraphics(hDCMeta);
    }

    //
    // parses the new filename, removes the extension and copy it into
    // szFilename
    //
    strcpy(szFilename, "");
    szTmp = (char *)My_mbstok(szFile, "\\");
    strcpy(szTmp2, szTmp);
    while (szTmp != NULL) {
        szTmp = (char *)My_mbstok(NULL, "\\");
        if (szTmp != NULL) {
            strcat(szFilename, szTmp2);
            strcpy(szTmp2, szTmp);
            strcat(szFilename, "\\");
        }
    }
    szTmp = (char *)My_mbstok(szTmp2, ".");
    strcat(szFilename, szTmp);

    return hDCMeta;
}


/******************************Public*Routine******************************\
*
* bPlayRecord
*
* Effects:  Play metafile
*           if PlayInfo.bPlayContinuous is TRUE
*               play metafile from 1st record up to the PlayInfo.iRecord th
*                   record
*           else only play the PlayInfo.iRecord th record and those preceding
*               records that are relevant like MoveTo, etc.
*           Terminates enumeration after playing up to the
*               PlayInfo.iRecord th record
*
* Warnings:
*
\**************************************************************************/

BOOL APIENTRY bPlayRecord(HDC hDC, LPHANDLETABLE lpHandleTable,
                                   LPENHMETARECORD lpEnhMetaRecord,
                                   UINT nHandles,
                                   LPVOID lpData) {
    BOOL bSuccess;
    static int  iCnt=0;
    int         i;
    char        ach[128];
    char        achTmp[128];
    LONG        lNumDword;

    bSuccess = TRUE;

    lNumDword = (lpEnhMetaRecord->nSize-8) / 4;

    iCnt++;
    if (((PLAYINFO *) lpData)->bPlayContinuous) {
        bSuccess = PlayEnhMetaFileRecord(hDC, lpHandleTable,
                                             lpEnhMetaRecord, nHandles);
        if (iCnt == ((PLAYINFO *) lpData)->iRecord) {
            wsprintf((LPSTR) ach, "%s", rgMetaName[lpEnhMetaRecord->iType]);
            for (i=0; i < lNumDword; i++) {
                wsprintf((LPSTR) achTmp, "%ld ", lpEnhMetaRecord->dParm[i]);
                if ((strlen(ach)+strlen(achTmp))/sizeof(char) >= 128)
                    break;
                strcat(ach, achTmp);
            }
        SetWindowText(ghTextWnd, ach);
        }
    } else {

        switch (lpEnhMetaRecord->iType) {
            case EMR_SETWINDOWEXTEX:
            case EMR_SETWINDOWORGEX:
            case EMR_SETVIEWPORTEXTEX:
            case EMR_SETVIEWPORTORGEX:
            case EMR_SETBRUSHORGEX:
            case EMR_SETMAPMODE:
            case EMR_SETBKMODE:
            case EMR_SETPOLYFILLMODE:
            case EMR_SETROP2:
            case EMR_SETSTRETCHBLTMODE:
            case EMR_SETTEXTALIGN:
            case EMR_SETTEXTCOLOR:
            case EMR_SETBKCOLOR:
            case EMR_OFFSETCLIPRGN:
            case EMR_MOVETOEX:
            case EMR_SETMETARGN:
            case EMR_EXCLUDECLIPRECT:
            case EMR_INTERSECTCLIPRECT:
            case EMR_SCALEVIEWPORTEXTEX:
            case EMR_SCALEWINDOWEXTEX:
            case EMR_SAVEDC:
            case EMR_RESTOREDC:
            case EMR_SETWORLDTRANSFORM:
            case EMR_MODIFYWORLDTRANSFORM:
            case EMR_SELECTOBJECT:
            case EMR_CREATEPEN:
            case EMR_CREATEBRUSHINDIRECT:
            case EMR_DELETEOBJECT:
            case EMR_SELECTPALETTE:
            case EMR_CREATEPALETTE:
            case EMR_SETPALETTEENTRIES:
            case EMR_RESIZEPALETTE:
            case EMR_REALIZEPALETTE:
            case EMR_SETARCDIRECTION:
            case EMR_SETMITERLIMIT:
            case EMR_BEGINPATH:
            case EMR_ENDPATH:
            case EMR_CLOSEFIGURE:
            case EMR_SELECTCLIPPATH:
            case EMR_ABORTPATH:
            case EMR_EXTCREATEFONTINDIRECTW:
            case EMR_CREATEMONOBRUSH:
            case EMR_CREATEDIBPATTERNBRUSHPT:
            case EMR_EXTCREATEPEN:
                goto PlayRec;
            default:
                break;
        } //switch

        if (iCnt == ((PLAYINFO *) lpData)->iRecord) {
PlayRec:
            bSuccess = PlayEnhMetaFileRecord(hDC, lpHandleTable,
                                             lpEnhMetaRecord, nHandles);
            wsprintf((LPSTR) ach, "%s", rgMetaName[lpEnhMetaRecord->iType]);
            for (i=0; i < lNumDword; i++) {
                wsprintf((LPSTR) achTmp, "%ld ", lpEnhMetaRecord->dParm[i]);
                if ((strlen(ach)+strlen(achTmp))/sizeof(char) >= 128)
                    break;
                strcat(ach, achTmp);
            }
            SetWindowText(ghTextWnd, ach);
        }
    }

    if (iCnt == ((PLAYINFO *) lpData)->iRecord) {
        iCnt = 0;

        if (gbDB)
            SwapBuffers(hDC);

        return FALSE;
    }
    return bSuccess;
}

/******************************Public*Routine******************************\
*
* LoadBitmapFile
*
* Effects:  Loads the bitmap from file and return the bitmap
*
* Warnings: pszFileName contains the full path
*
\**************************************************************************/

HBITMAP hBmpLoadBitmapFile(HDC hDC, PSTR pszFileName)
{
    HANDLE              hFile, hMapFile;
    LPVOID              pMapFile, pMapFileTmp;
    LPBITMAPINFOHEADER  pbmh;
    LPBITMAPINFO        pbmi;
    PBYTE               pjTmp;
    ULONG               sizBMI;
    HBITMAP             hBitmap;
    INT                 iNumClr;
    BOOL                bCoreHdr;
    WORD                wBitCount;
    PFILEINFO           pFileInfo;

    hBitmap = NULL;

    if ((hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) == (HANDLE)-1) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_FILEOPNFAIL));
        goto ErrExit1;
    }

    //
    // Create a map file of the opened file
    //
    if ((hMapFile = CreateFileMapping(hFile, NULL,
                             PAGE_READONLY, 0, 0, NULL)) == (HANDLE)-1) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_CREMAPFILEFAIL));
        goto ErrExit2;

    }

    //
    // Map a view of the whole file
    //
    if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0)) == NULL) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_MAPVWOFMAPFOFAIL));
        goto ErrExit3;
    }

    pMapFileTmp = pMapFile;

    //
    // First check that it is a bitmap file
    //
    if (*((PWORD)pMapFile) != 0x4d42) {              // 'BM'
        MessageBox(ghwndMain, GetStringRes(IDS_NOTDIBBMFILE),
            GetStringRes(IDS_ERROR), MB_OK);
        goto ErrExit3;
    }

    //
    // The file header doesn't end on DWORD boundary...
    //
    pbmh = (LPBITMAPINFOHEADER)((PBYTE)pMapFile + sizeof(BITMAPFILEHEADER));

    {
        BITMAPCOREHEADER bmch, *pbmch;
        BITMAPINFOHEADER bmih, *pbmih;
        PBYTE            pjTmp;
        ULONG            ulSiz;

        pbmch = &bmch;
        pbmih = &bmih;

        pjTmp = (PBYTE)pbmh;
        ulSiz = sizeof(BITMAPCOREHEADER);
        while (ulSiz--) {
            *(((PBYTE)pbmch)++) = *(((PBYTE)pjTmp)++);
        }

        pjTmp = (PBYTE)pbmh;
        ulSiz = sizeof(BITMAPINFOHEADER);
        while (ulSiz--) {
            *(((PBYTE)pbmih)++) = *(((PBYTE)pjTmp)++);
        }

        //
        // Use the size to determine if it is a BitmapCoreHeader or
        // BitmapInfoHeader
        //
        // Does PM supports 16 and 32 bpp? How?
        //
        if (bmch.bcSize == sizeof(BITMAPCOREHEADER))
        {
            wBitCount = bmch.bcBitCount;
            iNumClr = ((wBitCount == 24) ? 0 : (1 << wBitCount));
            sizBMI = sizeof(BITMAPCOREHEADER)+sizeof(RGBTRIPLE)*iNumClr;
            bCoreHdr = TRUE;
        }
        else            // BITMAPINFOHEADER
        {
            wBitCount = bmih.biBitCount;
            switch (wBitCount) {
                case 16:
                case 32:
                    sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(DWORD)*3;
                    break;
                case 24:
                    sizBMI = sizeof(BITMAPINFOHEADER);
                    break;
                default:
                    iNumClr = (1 << wBitCount);
                    sizBMI = sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*iNumClr;
                    break;
            }
            bCoreHdr = FALSE;
        }
    }

    if ((pbmi = (LPBITMAPINFO) LocalAlloc(LMEM_FIXED,sizBMI)) == NULL) {
        MessageBox(ghwndMain, GetStringRes(IDS_MALLOCFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
        goto ErrExit3;
    }

    //
    // Make sure we pass in a DWORD aligned BitmapInfo to CreateDIBitmap
    // Otherwise, exception on the MIPS platform
    // CR!!!  Equivalent to memcpy
    //
    pjTmp = (PBYTE)pbmi;

    while(sizBMI--)
    {
        *(((PBYTE)pjTmp)++) = *(((PBYTE)pbmh)++);
    }

    pMapFile = (PBYTE)pMapFile + ((BITMAPFILEHEADER *)pMapFile)->bfOffBits;

// !!! Use CreateBitmap for monochrome bitmap?

    //
    // Select the palette into the DC first before CreateDIBitmap()
    //
    bSelectDIBPal(hDC, pbmi, bCoreHdr);

// !!! We always pass a screen DC to this routine.
// !!! Maybe we should pass a metafile DC to this routine too.
// !!! The bitmap handle created for the screen DC won't give correct
// !!! color for the metafile DC.  So now, we always use the original
// !!! DIB info.
    if ((hBitmap = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)pbmi,
                        CBM_INIT, pMapFile, pbmi, DIB_RGB_COLORS)) == NULL) {
        SetWindowText(ghTextWnd, "Fail in creating DIB bitmap from file!");
        goto ErrExit4;
    }

    // reset gbUseDIB flag, now that we have opened up a new DIB
    gbUseDIB = FALSE;

// !!! Always use the DIB info o.w. metafile DC don't get the right color.
#if 0
    if (GetDeviceCaps(hDC, BITSPIXEL) < wBitCount) {
#endif
        gbUseDIB = TRUE;
        bFreeDibFile(&gDib);
        pFileInfo = &(gDib.rgFileInfo[0]);
        pFileInfo->hFile        = hFile;
        pFileInfo->hMapFile     = hMapFile;
        pFileInfo->lpvMapView   = pMapFileTmp;

        gDib.rgpjFrame[0]       = pMapFile;
        gDib.rgpbmi[0]          = pbmi;
        gDib.rgbCoreHdr[0]      = bCoreHdr;
        gDib.ulFrames           =
        gDib.ulFiles            = 1;
        return (hBitmap);
#if 0
    }
#endif

ErrExit4:
    LocalFree(pbmi);
ErrExit3:
    CloseHandle(hMapFile);
ErrExit2:
    CloseHandle(hFile);
ErrExit1:

    return (hBitmap);

}


/******************************Public*Routine******************************\
*
* bFreeDibFile
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

BOOL bFreeDibFile(PDIBDATA pDibData)
{
    ULONG               ulFiles;
    ULONG               ulFrames;
    ULONG               i;
    PFILEINFO           pFileInfo;

    ulFiles = pDibData->ulFiles;
    ulFrames = pDibData->ulFrames;

    for (i = 0; i < ulFrames; i++) {
        LocalFree(pDibData->rgpjFrame[i]);
        LocalFree(pDibData->rgpbmi[i]);
    }

    for (i = 0; i < ulFiles; i++) {
        pFileInfo = &(pDibData->rgFileInfo[i]);
        CloseHandle(pFileInfo->hFile);
        CloseHandle(pFileInfo->hMapFile);
        UnmapViewOfFile(pFileInfo->lpvMapView);
    }

    pDibData->ulFiles = 0;
    pDibData->ulFrames = 0;
    return TRUE;
}




/******************************Public*Routine******************************\
*
* bGetBMP
*
* Effects: call common dialog and pass the filename to hBmpLoadBitmapFile
*          return TRUE if successful, FALSE otherwise
*
* Warnings:
*
\**************************************************************************/

BOOL bGetBMP(HWND hwnd, BOOL bMask) {
    OPENFILENAME    ofn;
    char            szFile[256], szFileTitle[256];
    static char     *szFilter;
    BOOL            bSuccess;
    HDC             hDC;

    bSuccess = FALSE;

    BuildFilterStrs(-1);
    BuildFilterStrs(IDS_FT_DIB);
    szFilter = BuildFilterStrs(IDS_FT_RLE);

    strcpy(szFile, "*.bmp\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = (bMask ? GetStringRes(IDS_LDMSK) : "Load Bitmap");
    ofn.Flags = 0L;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "BMP";

    if (!GetOpenFileName(&ofn))
        return 0L;

    hDC = GetDC(ghwndDrawSurf);
    if (bMask) {
        ghBmpMask = hBmpLoadBitmapFile(hDC, szFile);
        if (ghBmpMask != NULL)
            bSuccess = TRUE;
    } else {
        ghBmp = hBmpLoadBitmapFile(hDC, szFile);
        if (ghBmp != NULL)
            bSuccess = TRUE;
    }
    ReleaseDC(ghwndDrawSurf, hDC);

    return bSuccess;
}

/******************************Public*Routine******************************\
*
* bHitTest
*
* Effects:  Enumerates metafile records
*           Calling bDoHitTest to process each record found.
*               The mouse position is passed to the bDoHitTest
*
* Warnings:
*
\**************************************************************************/

BOOL bHitTest(HDC hDC, INT x, INT y) {
    BOOL          bSuccess;
    ENHMETAHEADER EnhMetaHdr;
    RECT          rcClientDS;
    HTDATA        htData;
    static        HCURSOR hCurHT, hCurWait;

    bSuccess = TRUE;

    if (ghMetaf == 0)
        return 0L;

    hCurHT = LoadCursor(NULL, IDC_CROSS);
    hCurWait = LoadCursor(NULL, IDC_WAIT);

    GetEnhMetaFileHeader(ghMetaf, sizeof(EnhMetaHdr), &EnhMetaHdr);

    htData.point.x = x;
    htData.point.y = y;
    htData.iRecord = EnhMetaHdr.nRecords;

    SetCursor(hCurWait);
    if (gbFit2Wnd) {
        GetClientRect(ghwndDrawSurf, &rcClientDS);
        EnumEnhMetaFile(hDC, ghMetaf, (ENHMFENUMPROC)bDoHitTest, (LPVOID) &htData, (LPRECT)&rcClientDS);
    } else {
        RECT rc;

        rc.top = rc.left = 0;
        rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
        rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
        EnumEnhMetaFile(hDC, ghMetaf, (ENHMFENUMPROC)bDoHitTest, (LPVOID) &htData, (LPRECT)&rc);
    }
    SetCursor(hCurHT);

    return bSuccess;
}

/******************************Public*Routine******************************\
*
* bDoHitTest
*
* Effects:      Play all records related to transformation
*               Remember new mouse position if the record is a MoveTo
*               Convert rectangle, ellipse, lineto and bezier to path
*               Widen the path and convert it to region.
*               Test if the mouse position is inside the region.
*
* Warnings:     Only handle rectangle, ellipse, line and polybezier
*
\**************************************************************************/

BOOL APIENTRY bDoHitTest(HDC hDC, LPHANDLETABLE lpHandleTable,
                                  LPENHMETARECORD lpEnhMetaRecord,
                                  UINT nHandles,
                                  LPVOID lpData) {
    BOOL            bSuccess;
    char            ach[128];
    char            achTmp[128];
    POINT           PtOrg;
    LONG            lNumDword;
    XFORM           xfSave;
    SIZE            SizeWndEx, SizeViewEx;
    POINT           ptWndOrgin, ptViewOrgin;
    int             i, iMode;
    HRGN            hRgn;
    PPOINT          pPt, pPtTmp;
    static HGDIOBJ  hObjOld=NULL;
    static LONG     lCurX=0;
    static LONG     lCurY=0;
    static BOOL     bXform=FALSE;
    static int      iCnt=0;

    iCnt++;

    //
    // select a wide pen for widen path later on
    //
    hObjOld = SelectObject(hDC, ghpnWide);

    //
    // save the mouse hit position, this was passed in as a POINT structure
    //
    PtOrg.x = (((HTDATA *)lpData)->point).x;
    PtOrg.y = (((HTDATA *)lpData)->point).y;

    //
    // save the number of parameters for the GDI fcn concerned in DWORD.
    // This is the total size of metafile record in question less the
    // size of the GDI function
    //
    lNumDword = (lpEnhMetaRecord->nSize-8) / 4;

    switch (lpEnhMetaRecord->iType) {
    case EMR_SETWINDOWEXTEX:
    case EMR_SETWINDOWORGEX:
    case EMR_SETVIEWPORTEXTEX:
    case EMR_SETVIEWPORTORGEX:
    case EMR_SETMAPMODE:
    case EMR_SCALEVIEWPORTEXTEX:
    case EMR_SCALEWINDOWEXTEX:
    case EMR_SETMETARGN:
    case EMR_SAVEDC:
    case EMR_RESTOREDC:
    case EMR_SETWORLDTRANSFORM:
    case EMR_MODIFYWORLDTRANSFORM: {
        //
        // play all records related to transformation & font
        //
        PlayEnhMetaFileRecord(hDC, lpHandleTable,
                                   lpEnhMetaRecord, nHandles);
        bXform = TRUE;
        return TRUE;
    }
    //
    // convert the following GDI calls to path for hit testing
    //
    case EMR_RECTANGLE: {
        BeginPath(hDC);
        Rectangle(hDC, lpEnhMetaRecord->dParm[0], lpEnhMetaRecord->dParm[1],
                       lpEnhMetaRecord->dParm[2], lpEnhMetaRecord->dParm[3]);
        EndPath(hDC);
        break;
    }
    case EMR_ELLIPSE: {
        BeginPath(hDC);
        Ellipse(hDC, lpEnhMetaRecord->dParm[0], lpEnhMetaRecord->dParm[1],
                     lpEnhMetaRecord->dParm[2], lpEnhMetaRecord->dParm[3]);
        EndPath(hDC);
        break;
    }
    case EMR_MOVETOEX: {
        //
        // Remember our current position
        //
        lCurX = lpEnhMetaRecord->dParm[0];
        lCurY = lpEnhMetaRecord->dParm[1];
        return TRUE;
    }
    case EMR_LINETO: {
        BeginPath(hDC);
        MoveToEx(hDC, lCurX, lCurY, NULL);
        LineTo(hDC, lpEnhMetaRecord->dParm[0], lpEnhMetaRecord->dParm[1]);
        EndPath(hDC);
        break;
    }
    case EMR_POLYBEZIER16: {
        int         i;
        LONG        lSize;
        LONG        lPtCnt;

        lPtCnt = lpEnhMetaRecord->dParm[4];
        lSize = lPtCnt * sizeof(POINTL);

        if ((pPt = (PPOINT) LocalAlloc(LMEM_FIXED, lSize)) == NULL) {
            SetWindowText(ghTextWnd, GetStringRes(IDS_MALLOCFAILNOHIT));
            return TRUE;
        }

        pPtTmp = pPt;

        for (i=0; i < (INT) lPtCnt; i++, pPtTmp++) {
            pPtTmp->x = (LONG)(LOWORD(lpEnhMetaRecord->dParm[i+5]));
            pPtTmp->y = (LONG)(HIWORD(lpEnhMetaRecord->dParm[i+5]));
        }

        BeginPath(hDC);
        PolyBezier(hDC, (LPPOINT)pPt, (DWORD) lPtCnt);
        EndPath(hDC);
        LocalFree(pPt);
        break;
    }
    default:
        wsprintf((LPSTR) ach, GetStringRes(IDS_FMT_NOHTST), rgMetaName[lpEnhMetaRecord->iType]);
        SetWindowText(ghTextWnd, ach);
        return TRUE;
    }   //switch

    if (bXform) {
        //
        // Set World transform to identity temporarily so that pen width
        // is not affected by world to page transformation
        //
        GetWorldTransform(hDC, &xfSave);
        ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);

        //
        // Set Page transform to identity temporarily so that pen width
        // is not affected by page to device transformation
        //
        iMode = GetMapMode(hDC);

        if ((iMode == MM_ISOTROPIC) || (iMode == MM_ANISOTROPIC)) {
            GetWindowOrgEx(hDC, &ptWndOrgin);
            GetWindowExtEx(hDC, &SizeWndEx);
            GetViewportExtEx(hDC, &SizeViewEx);
            GetViewportOrgEx(hDC, &ptViewOrgin);
        }

        SetMapMode(hDC, MM_TEXT);
    }

    WidenPath(hDC);

    hRgn = PathToRegion(hDC);

    if (hRgn == 0) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_NULLRGNHTSTFAIL));
        DeleteObject(hRgn);
        return TRUE;
    }
    //DPtoLP(hDC, &PtOrg, 1);
    //SetPixel(hDC, PtOrg.x, PtOrg.y, RGB(0, 255, 0));
    //
    // test if mouse hit position is in region
    //
    bSuccess = PtInRegion(hRgn, PtOrg.x, PtOrg.y);
    //Temporily comment this out
    FillRgn(hDC, hRgn, ghbrRed);
    DeleteObject(hRgn);
    //
    // Set transform back.
    //
    if (bXform) {
        SetWorldTransform(hDC, &xfSave);
        SetMapMode(hDC, iMode);

        if ((iMode == MM_ISOTROPIC) || (iMode == MM_ANISOTROPIC)) {
            SetWindowOrgEx(hDC, ptWndOrgin.x, ptWndOrgin.y, NULL);
            SetWindowExtEx(hDC, SizeWndEx.cx, SizeWndEx.cy, NULL);
            SetViewportExtEx(hDC, SizeViewEx.cx, SizeViewEx.cy, NULL);
            SetViewportOrgEx(hDC, ptViewOrgin.x, ptViewOrgin.y, NULL);
        }
    }

    if (bSuccess) {
        Beep(440, 500);
        //
        // Reporting the metafile record number.  Then reset counter.
        //
        SetDlgItemInt(ghwndCtrlPanel, DID_COUNTER, iCnt, FALSE);
        iCnt=0;
        wsprintf((LPSTR) ach, GetStringRes(IDS_FMT_HIT), rgMetaName[lpEnhMetaRecord->iType]);

        for (i=0; i < lNumDword; i++) {
            wsprintf((LPSTR) achTmp, "%ld ", lpEnhMetaRecord->dParm[i]);
            if ((strlen(ach)+strlen(achTmp))/sizeof(char) >= 128)
                break;
            strcat(ach, achTmp);
        }

        SetWindowText(ghTextWnd, ach);
        SelectObject(hDC, hObjOld);
        bXform = FALSE;
        return FALSE;
    }
    SetWindowText(ghTextWnd, GetStringRes(IDS_NOHIT));
    if (iCnt >= ((HTDATA *)lpData)->iRecord)
        iCnt = 0;
    return TRUE;

    UNREFERENCED_PARAMETER(lpHandleTable);
    UNREFERENCED_PARAMETER(nHandles);

}

/******************************Public*Routine******************************\
*
* bChooseNewFont
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

BOOL bChooseNewFont(HWND hwnd, PLOGFONT plf, COLORREF *pClrRef) {
   HDC                  hDC;
   static CHOOSEFONT    chf;
   static BOOL          bInit=TRUE;


   if (bInit) {
        bInit = FALSE;

        hDC = GetDC( hwnd );
        chf.hDC = CreateCompatibleDC( hDC );
        ReleaseDC( hwnd, hDC );

        chf.lStructSize = sizeof(CHOOSEFONT);
        chf.hwndOwner = hwnd;
        chf.lpLogFont = plf;
        chf.Flags = CF_SCREENFONTS | CF_EFFECTS | CF_INITTOLOGFONTSTRUCT;
        chf.rgbColors = *pClrRef;
        chf.lCustData = 0;
        chf.hInstance = (HANDLE)NULL;
        chf.lpszStyle = (LPSTR)NULL;
        chf.nFontType = SCREEN_FONTTYPE;
        chf.nSizeMin = 0;
        chf.nSizeMax = 0;
        chf.lpfnHook = (LPCFHOOKPROC)NULL;
        chf.lpTemplateName = (LPSTR)NULL;
   }

   if (ChooseFont( &chf ) == FALSE ) {
        DeleteDC( hDC );
	return FALSE;
   }

   *pClrRef = chf.rgbColors;

   DeleteDC( hDC );
   return (TRUE);
}

/******************************Public*Routine******************************\
*
* bChooseNewColor
*
* Effects:  Returns TRUE if successful; lpdwRGB points the color selected.
*           Otherwise, FALSE.
*
* Warnings:
*
\**************************************************************************/

BOOL bChooseNewColor(HWND hwnd, LPDWORD lpdwRGB) {
    static DWORD argbCust[16] = {
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255),
        RGB(255, 255, 255), RGB(255, 255, 255)
    };
    CHOOSECOLOR cc;
    BOOL bResult;

    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hwnd;
    cc.hInstance = ghModule;
    cc.rgbResult = *lpdwRGB;
    cc.lpCustColors = argbCust;
    cc.Flags = CC_RGBINIT | CC_SHOWHELP;
    cc.lCustData = 0;
    cc.lpfnHook = NULL;
    cc.lpTemplateName = NULL;

    bResult = ChooseColor(&cc);

    if (bResult) {
        *lpdwRGB = cc.rgbResult;
        return TRUE;
    }

    return FALSE;
}


/******************************Public*Routine******************************\
*
* hBrCreateBrush
*
* Effects: Creates a brush with the specified RGB
*
* Warnings:
*
\**************************************************************************/

HBRUSH hBrCreateBrush(HDC hDC, DWORD dwRGB)
{
    HDC hdcMem;
    HBRUSH hbr;
    HBRUSH hbrOld;
    HBITMAP hbmPat;
    HBITMAP hbmOld;

    hbr = CreateSolidBrush(dwRGB);
    hdcMem = CreateCompatibleDC(hDC);

    //
    // Minimum size for a bitmap to be used in a fill pattern is 8x8
    //
    hbmPat = CreateCompatibleBitmap(hDC, 8, 8);

    hbmOld = SelectObject(hdcMem, hbmPat);
    hbrOld = SelectObject(hdcMem, hbr);
    PatBlt(hdcMem, 0, 0, 8, 8, PATCOPY);

    //
    // Deselect hbmPat and hbr
    //
    SelectObject(hdcMem, hbmOld);
    SelectObject(hdcMem, hbrOld);

    DeleteDC(hdcMem);
    DeleteObject(hbr);

    hbr = CreatePatternBrush(hbmPat);

    DeleteObject(hbmPat);

    return hbr;
}


/******************************Public*Routine******************************\
*
* bPrintMf  Brings up the print dialog for printer setup and then
*           starts printing the enhanced metafile.
*
*           pPD     Points to a PRTDATA structure that contains the
*                   the handle for the Enh. Metafile for printing.
*
* Effects:  Returns TRUE if sucessful.  Otherwise, it is FALSE.
*           GlobalFree pPD when exits.
*
* Warnings:
*
\**************************************************************************/

BOOL bPrintMf(PPRTDATA pPD) {
    DOCINFO         DocInfo;
    HDC             hDCPrinter;
    ENHMETAHEADER   EnhMetaHdr;
    HENHMETAFILE    hEnhMf;
    TCHAR           buf[128];
    PRINTDLG        pd;
    BOOL            bSuccess;
    int             iEntries;
    PLOGPALETTE     plogPal;
    PBYTE           pjTmp;
    HPALETTE        hPal;
    BOOL            bRCSet = FALSE;


    bSuccess = TRUE;

    if (pPD->hMetaf == 0) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_NOMFTOPRT));
        goto PMF_EXIT;
        bSuccess = FALSE;
    }

    hEnhMf = CopyEnhMetaFile(pPD->hMetaf, NULL);
    memset(&pd, 0, sizeof(PRINTDLG));
    pd.lStructSize = sizeof(PRINTDLG);
    pd.hwndOwner   = ghwndMain;
    pd.Flags       = PD_RETURNDC;
    pd.hInstance   = ghModule;

    if (!PrintDlg(&pd)) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_ESCPRT));
        goto PMF_EXIT;
        bSuccess = FALSE;
    }

    if (pd.hDC == NULL) {
        SetWindowText(ghTextWnd, GetStringRes(IDS_CREPRTDCFAIL));
        goto PMF_EXIT;
        bSuccess = FALSE;
    }

    hDCPrinter = pd.hDC;
    if (GetEnhMetaFileDescription(hEnhMf, 128, (LPTSTR)buf) == 0) {
        strcpy(buf, "No Title");
    }

    memset(&DocInfo, 0, sizeof(DOCINFO));
    DocInfo.cbSize      = sizeof(DOCINFO);
    DocInfo.lpszDocName = (LPTSTR) buf;
    DocInfo.lpszOutput  = NULL;
    StartDoc(hDCPrinter, &DocInfo);
    StartPage(hDCPrinter);

    SetWindowText(ghTextWnd, GetStringRes(IDS_PRT));

    iEntries = GetEnhMetaFilePaletteEntries(hEnhMf, 0, NULL);

    if (iEntries) {
        if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
                sizeof(DWORD) + sizeof(PALETTEENTRY)*iEntries )) == NULL) {
            MessageBox(ghwndMain, GetStringRes(IDS_CREPALFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
        }

        plogPal->palVersion = 0x300;
        plogPal->palNumEntries = (WORD) iEntries;
        pjTmp = (PBYTE) plogPal;
        pjTmp += 4;

        GetEnhMetaFilePaletteEntries(hEnhMf, iEntries, (PPALETTEENTRY)pjTmp);
        hPal = CreatePalette(plogPal);
        GlobalFree(plogPal);

        SelectPalette(hDCPrinter, hPal, FALSE);
        RealizePalette(hDCPrinter);
    }

    GetEnhMetaFileHeader(hEnhMf, sizeof(ENHMETAHEADER), &EnhMetaHdr);


    bRCSet = bSetRC2MatchEmfRC(hDCPrinter, EnhMetaHdr, hEnhMf, NULL);
    if (!bRCSet) {
        vErrOut("bPrintMF: failed");
    }

    if (pPD->bFit2Wnd) {
        int     iWidth, iHeight;
        RECT    rc;

        iWidth = GetDeviceCaps(hDCPrinter, HORZRES);
        iHeight = GetDeviceCaps(hDCPrinter, VERTRES);
        rc.left = rc.top = 0;
        rc.right = iWidth;
        rc.bottom = iHeight;
        bSuccess = PlayEnhMetaFile(hDCPrinter, hEnhMf, (LPRECT) &rc);
        if (!bSuccess) {
            char    text[128];

            wsprintf(text, GetStringRes(IDS_FMT_PLAYEMFFAIL), GetLastError());
            OutputDebugString(text);
        }


    } else {
        RECT rc;

        rc.top = rc.left = 0;
        rc.right = EnhMetaHdr.rclBounds.right - EnhMetaHdr.rclBounds.left;
        rc.bottom = EnhMetaHdr.rclBounds.bottom - EnhMetaHdr.rclBounds.top;
        bSuccess = PlayEnhMetaFile(hDCPrinter, hEnhMf, (LPRECT) &rc);
        if (!bSuccess) {
            char    text[128];

            wsprintf(text, GetStringRes(IDS_FMT_PLAYEMFFAIL), GetLastError());
            OutputDebugString(text);
        }
    }

//    if (gbDB && bSuccess)
//       SwapBuffers(hDCPrinter);

    EndPage(hDCPrinter);
    EndDoc(hDCPrinter);
    SetWindowText(ghTextWnd, GetStringRes(IDS_PRTTHRDDONE));

    if (bRCSet) {
        bCleanUpRC();
    }


PMF_EXIT:

    ExitThread(0);
    return bSuccess;

}

/******************************Public*Routine******************************\
*
* bSelectDIBPal
*
* Effects: Creates a logical palette from the DIB and select it into the DC
*          and realize the palette. Saving the hPal in the ghPal
*
* Warnings: Based on Windows NT DIB support.  If PM support 16,24,32 bpp
*           we need to modify this routine.
*           Global alert! ghPal is changed here...
*
\**************************************************************************/

BOOL bSelectDIBPal(HDC hDC, LPBITMAPINFO pbmi, BOOL bCoreHdr)
{
  LOGPALETTE    *plogPal;
  UINT          uiSizPal;
  INT           i, iNumClr;
  WORD          wBitCount;

  if (bCoreHdr) {
    wBitCount = ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcBitCount;
  } else {
    wBitCount = pbmi->bmiHeader.biBitCount;
  }

  switch (wBitCount) {
    case 16:
    case 24:
    case 32:                            // Does PM supports these?
        return FALSE;
    default:
        iNumClr = (1 << wBitCount);
        break;
  }

  uiSizPal = sizeof(WORD)*2 + sizeof(PALETTEENTRY)*iNumClr;
  if ((plogPal = (LOGPALETTE *) LocalAlloc(LMEM_FIXED,uiSizPal)) == NULL) {
      MessageBox(ghwndMain, GetStringRes(IDS_ALLOCPALFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
      ghPal = NULL;
      return FALSE;
  }

  plogPal->palVersion = 0x300;
  plogPal->palNumEntries = (WORD) iNumClr;

  if (bCoreHdr) {
    for (i=0; i<iNumClr; i++) {
        plogPal->palPalEntry[i].peRed   = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtRed;
        plogPal->palPalEntry[i].peGreen = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtGreen;
        plogPal->palPalEntry[i].peBlue  = ((LPBITMAPCOREINFO)pbmi)->bmciColors[i].rgbtBlue;
        plogPal->palPalEntry[i].peFlags = PC_RESERVED;
    }
  } else {
    for (i=0; i<iNumClr; i++) {
        plogPal->palPalEntry[i].peRed   = pbmi->bmiColors[i].rgbRed;
        plogPal->palPalEntry[i].peGreen = pbmi->bmiColors[i].rgbGreen;
        plogPal->palPalEntry[i].peBlue  = pbmi->bmiColors[i].rgbBlue;
        plogPal->palPalEntry[i].peFlags = PC_RESERVED;
    }
  }

  DeleteObject(ghPal);
  ghPal = CreatePalette((LPLOGPALETTE)plogPal);
  if ((ghPal) == NULL) {
      MessageBox(ghwndMain, GetStringRes(IDS_CREPALFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
      return FALSE;
  }

  if ((GetDeviceCaps(hDC, RASTERCAPS)) & RC_PALETTE) {
    SelectPalette(hDC, ghPal, FALSE);
    RealizePalette(hDC);
  }

  GlobalFree(plogPal);

  return TRUE;
}


/******************************Public*Routine******************************\
*
* bPlgBlt
*
* Effects:  If Source DIB bpp > Destination DC's
*           use Halftone for PlgBlt.
*
* Warnings: Global Alert!
*           gbUseDIB is always TRUE now.
*
\**************************************************************************/

BOOL bPlgBlt(HDC hDC, LPPOINT rgPtsBMP)
{
    HDC                  hDCRef;
    HDC                  hDCSrn;                // hDC can be metaf DC
    HGDIOBJ              hObjOld, hBmpMem;
    BITMAP               bm;
    INT                  iBpp;
    WORD                 wBitCnt;


    hDCSrn = GetDC(ghwndDrawSurf);
    hDCRef = CreateCompatibleDC(hDC);

    if (gbUseDIB) {
        int         cx, cy, dx, dy;
        PBITMAPINFO pbmi;

        pbmi = (gDib.rgpbmi[0]);
        dx = rgPtsBMP[0].x - rgPtsBMP[1].x;
        dy = rgPtsBMP[0].y - rgPtsBMP[1].y;
        cx = (INT) sqrt( dx * dx + dy * dy );

        dx = rgPtsBMP[0].x - rgPtsBMP[2].x;
        dy = rgPtsBMP[0].y - rgPtsBMP[2].y;
        cy = (INT) sqrt( dx * dx + dy * dy );

        iBpp = GetDeviceCaps(hDC, BITSPIXEL);

        if (gDib.rgbCoreHdr[0]) {
            wBitCnt = ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcBitCount;
        } else {
            wBitCnt = pbmi->bmiHeader.biBitCount;
        }

        if (iBpp < wBitCnt) {   // Do Halftone
            SetStretchBltMode(hDCRef, HALFTONE);
            if (ghHT) {
                SelectPalette(hDCRef, ghHT, FALSE);
                SelectPalette(hDC, ghHT, FALSE);
                SelectPalette(hDCSrn, ghHT, FALSE); // hDC can be metaf DC
                RealizePalette(hDCSrn);             // always realize the srn DC

                // Don't have to realize the palette in hDCRef
                // RealizePalette(hDCRef);

                // has to be compatible with screen DC, cannot be hDCRef
                // memory DC has no bitmap by default?
                // hDC may be a metafile DC, so use hDCSrn
                hBmpMem = CreateCompatibleBitmap(hDCSrn, cx, cy);
                SelectObject(hDCRef, hBmpMem);
            } else {
                MessageBox(ghwndMain, GetStringRes(IDS_HFTONEPALNULL),
                    GetStringRes(IDS_ERROR), MB_OK);
            }
        } else {
            SetStretchBltMode(hDCRef, COLORONCOLOR);
            if (ghPal) {
                if (ghDCMetaf == hDC)
                    CopyPalette(ghPal);
                SelectPalette(hDCRef, ghPal, FALSE);
                SelectPalette(hDC, ghPal, FALSE);
                SelectPalette(hDCSrn, ghPal, FALSE); // hDC can be metaf DC
                RealizePalette(hDCSrn);             // always realize the srn DC

                // Don't have to realize the palette in hDCRef
                // RealizePalette(hDCRef);

                // has to be compatible with screen DC, cannot be hDCRef
                // memory DC has no bitmap by default?
                // hDC may be a metafile DC, so use hDCSrn
                hBmpMem = CreateCompatibleBitmap(hDCSrn, cx, cy);
                SelectObject(hDCRef, hBmpMem);
            } else {
                MessageBox(ghwndMain, GetStringRes(IDS_PALNULL),
                    GetStringRes(IDS_ERROR), MB_OK);
            }
        }

        if (gDib.rgbCoreHdr[0]) {
            StretchDIBits(hDCRef, 0,0, cx, cy,
                          0,0, ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcWidth, ((LPBITMAPCOREINFO)pbmi)->bmciHeader.bcHeight,
                          gDib.rgpjFrame[0], pbmi, DIB_RGB_COLORS, SRCCOPY);
        } else {
            StretchDIBits(hDCRef, 0,0, cx, cy,
                          0,0, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight,
                          gDib.rgpjFrame[0], pbmi, DIB_RGB_COLORS, SRCCOPY);
        }

        PlgBlt(hDC, rgPtsBMP, hDCRef, 0, 0, cx, cy,
               ghBmpMask, 0, 0);

        DeleteObject(hBmpMem);

    } else {
        hObjOld = SelectObject(hDCRef, ghBmp);

        GetObject(ghBmpMask, sizeof(BITMAP), (LPSTR)&bm);
        if (bm.bmBitsPixel != 1) {
            SetWindowText(ghTextWnd, GetStringRes(IDS_MUSTMSKMCHROMEBM));
            ghBmpMask = NULL;
        }

        GetObject(ghBmp, sizeof(BITMAP), (LPSTR)&bm);

        if (ghPal) {
            SelectPalette(hDC, ghPal, FALSE);
            RealizePalette(hDC);
            SetStretchBltMode(hDC, COLORONCOLOR);
        }
        PlgBlt(hDC, rgPtsBMP, hDCRef, 0, 0, bm.bmWidth, bm.bmHeight,
               ghBmpMask, 0, 0);

        SelectObject(hDCRef, hObjOld);
    }

    DeleteDC(hDCRef);
    ReleaseDC(ghwndDrawSurf, hDCSrn);
    return TRUE;

}



/******************************Public*Routine******************************\
*
* HPALETTE CopyPalette
*
* Effects:
*
* Warnings:
*
\**************************************************************************/

HPALETTE CopyPalette(HPALETTE hPalSrc)
{
    PLOGPALETTE     plogPal;
    PBYTE           pjTmp;
    int             iNumEntries=0;
    HPALETTE        hPal;

    if ((iNumEntries = GetPaletteEntries(hPalSrc, 0, iNumEntries, NULL)) == 0) {
        MessageBox(ghwndMain, GetStringRes(IDS_NOPALENTRYFORCPY),
            GetStringRes(IDS_ERROR), MB_OK);
        return (HPALETTE) NULL;
    }

    if ((plogPal = (PLOGPALETTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
            sizeof(DWORD) + sizeof(PALETTEENTRY)*iNumEntries )) == NULL) {
        MessageBox(ghwndMain, GetStringRes(IDS_CPYPALFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
        return (HPALETTE) NULL;
    }

    plogPal->palVersion = 0x300;
    plogPal->palNumEntries = (WORD) iNumEntries;
    pjTmp = (PBYTE) plogPal;
    pjTmp += 4;
    GetPaletteEntries(hPalSrc, 0, iNumEntries, (PPALETTEENTRY)pjTmp);
    hPal = CreatePalette(plogPal);

    GlobalFree(plogPal);

    return hPal;
}




/******************************Public*Routine******************************\
*
* iTT
*
* Effects: set the global variable gbTT if the family is true type
*
* Warnings:
*
\**************************************************************************/

int CALLBACK iTT(
    LPLOGFONT    lpLF,
    LPTEXTMETRIC lpTM,
    DWORD        dwFontType,
    LPARAM       lpData)
{

    if (lpTM->tmPitchAndFamily & TMPF_TRUETYPE) {
        //OutputDebugString("TRUETYPE\n");
        *((BOOL *)lpData) = TRUE;
    } else {
        //OutputDebugString("NON-TRUETYPE\n");
        *((BOOL *)lpData) = FALSE;
    }

#if 0
    //
    // that's equivalent
    //
    if (dwFontType & TRUETYPE_FONTTYPE) {
        //OutputDebugString("TRUETYPE\n");
        *((BOOL *)lpData) = TRUE;
    } else {
        //OutputDebugString("NON-TRUETYPE\n");
        *((BOOL *)lpData) = FALSE;
    }
#endif
    return 0;

    UNREFERENCED_PARAMETER (lpLF);
    //UNREFERENCED_PARAMETER (lpTM);
    UNREFERENCED_PARAMETER (dwFontType);

}



/******************************Public*Routine******************************\
*
* CMTMLTFMT *pLoadMltFmtFile(VOID)
*
* Effects:  Load either EPS or enh mf
*
* Warnings: CR! change this to load multiple def of picture
*
\**************************************************************************/

CMTMLTFMT *pLoadMltFmtFile(VOID)
{
    OPENFILENAME    ofn;
    char            szDirName[256];
    char            szFile[256], szFileTitle[256];
    static char     *szFilter;
    HANDLE          hFile, hMapFile;
    LPVOID          pMapFile;
    DWORD           dwFileSizeLow, dwFileSizeHigh;
    CMTMLTFMT       *pMfmt;

    pMfmt = (CMTMLTFMT*)NULL;

    BuildFilterStrs(-1);
    BuildFilterStrs(IDS_FT_EPS);
    szFilter = BuildFilterStrs(IDS_FT_EMF);

    GetSystemDirectory((LPSTR) szDirName, 256);
    strcpy(szFile, "*.eps\0");
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = GetFocus();
    ofn.lpstrFilter = szFilter;
    ofn.lpstrCustomFilter = (LPSTR) NULL;
    ofn.nMaxCustFilter = 0L;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFileTitle = sizeof(szFileTitle);
    ofn.lpstrInitialDir = szDirName;
    ofn.lpstrTitle = (LPSTR) NULL;
    ofn.Flags = 0L;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = "EPS";

    if (!GetOpenFileName(&ofn)) {
        goto EXIT;
    }

    if ((hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL)) == (HANDLE)-1) {
        goto EXIT;
    }

    dwFileSizeLow = GetFileSize(hFile, &dwFileSizeHigh);
    if ((dwFileSizeLow == 0xFFFFFFFF) && (GetLastError() != NO_ERROR)) {
        goto EXIT;
    }

    //
    // Create a map file of the opened file
    //
    if ((hMapFile = CreateFileMapping(hFile, NULL,
                             PAGE_READONLY, 0, 0, NULL)) == (HANDLE)-1) {
        goto EXIT2;
    }

    //
    // Map a view of the whole file
    //
    if ((pMapFile = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0)) == NULL) {
        goto EXIT3;
    }

//
// CR!! In future, change this to load different def of the picture...
//
  {
    ULONG       ulSize;
    PBYTE       pjTmp;
    RECTL       rectl;

    ulSize = dwFileSizeLow+sizeof(CMTMLTFMT);
    if ((pMfmt = (CMTMLTFMT *) LocalAlloc(LMEM_FIXED, ulSize)) == NULL) {
        MessageBox(GetFocus(), GetStringRes(IDS_MALLOCFAIL),
            GetStringRes(IDS_ERROR), MB_OK);
        goto EXIT3;
    }

    pMfmt->ident = GDICOMMENT_IDENTIFIER;
    pMfmt->iComment = GDICOMMENT_MULTIFORMATS;
    pMfmt->nFormats = 1;
    pMfmt->aemrformat[0].cbData = dwFileSizeLow;
    pMfmt->aemrformat[0].offData = 11*sizeof(DWORD);

    // parse for %!PS-Adobe-3.0 EPSF keyword
    //           Enhanced Metafile signature
    // set EMRFORMAT.dSignature appropiately

    if (((ENHMETAHEADER *) pMapFile)->dSignature == ENHMETA_SIGNATURE) {

        pMfmt->aemrformat[0].dSignature = ENHMETA_SIGNATURE;
        pMfmt->aemrformat[0].nVersion = 0;                    // not for emf
        pMfmt->rclOutput.left   = ((ENHMETAHEADER *) pMapFile)->rclBounds.left;
        pMfmt->rclOutput.top    = ((ENHMETAHEADER *) pMapFile)->rclBounds.top;
        pMfmt->rclOutput.right  = ((ENHMETAHEADER *) pMapFile)->rclBounds.right;
        pMfmt->rclOutput.bottom = ((ENHMETAHEADER *) pMapFile)->rclBounds.bottom;
    }
    else    //assume it is Adobe EPS
    if (bGetEPSBounds(pMapFile, &rectl)) {

        char text[128];


        pMfmt->aemrformat[0].dSignature = 0x46535045;
        pMfmt->aemrformat[0].nVersion = 1;
        pMfmt->rclOutput.left   = rectl.left;
        pMfmt->rclOutput.top    = rectl.top;
        pMfmt->rclOutput.right  = rectl.right;
        pMfmt->rclOutput.bottom = rectl.bottom;

        wsprintf(text, "Bounds = %d %d %d %d",
                 rectl.left, rectl.top, rectl.right, rectl.bottom);
        MessageBox(GetFocus(), text, "Bounds", MB_OK);

    }
    else {
        // unknown file type
        Free(pMfmt);
        pMfmt = NULL;
        goto EXIT3;
    }

    pjTmp = (PBYTE)(((DWORD *)pMfmt->aemrformat)+4);
    while (dwFileSizeLow--) {
        *(((PBYTE)pjTmp)++) = *(((PBYTE)pMapFile)++);
    }

  }


EXIT3:
    CloseHandle(hMapFile);
EXIT2:
    CloseHandle(hFile);
EXIT:

    return pMfmt;
}


HLOCAL Free(CMTMLTFMT *pMfmt) {
    return LocalFree(pMfmt);
}

#define MYDBG 0

BOOL bIsAdobe(char *szStr)
{
    if (strcmp(szStr, "%!PS-Adobe-3.0") == 0)
        return TRUE;
    else
        return FALSE;
}

BOOL bIsEPS(char *szStr)
{
    if ((strcmp(szStr, "EPSF-3.0") == 0) ||
        (strcmp(szStr, "EPSF-2.0") == 0))
        return TRUE;
    else
        return FALSE;
}

BOOL bIsBndBox(char *szStr)
{
    if (strcmp(szStr, "%%BoundingBox:") == 0)
        return TRUE;
    else
        return FALSE;
}

BOOL bIsEOF(char *szStr)
{
    if (strcmp(szStr, "%%EOF") == 0)
        return TRUE;
    else
        return FALSE;
}


BOOL bGetEPSBounds(LPVOID lpData, RECTL *prctl)
{
    char szKeyWord[128], szValue[128];
    int  index;


    if (lpData == NULL) {
#if MYDBG
        MessageBox(GetFocus(), GetStringRes(IDS_NULLPTR),
            GetStringRes(IDS_ERROR), MB_OK);
#endif
        return FALSE;
    }

    index = 0;

    if (!bGetWord(lpData, szKeyWord, &index))
        return FALSE;

    if (!bIsAdobe(szKeyWord)) {
        MessageBox(GetFocus(), GetStringRes(IDS_NOTADOBE),
            GetStringRes(IDS_ERROR), MB_OK);
        return FALSE;
    }

    if (!bGetWord(lpData, szValue, &index))
        return FALSE;

    if (!bIsEPS(szValue)) {
        MessageBox(GetFocus(), "Not EPS!",
            GetStringRes(IDS_ERROR), MB_OK);
        return FALSE;
    }

    if (!bGoNextLine(lpData, &index))
        return FALSE;

    while ((bGetWord(lpData, szKeyWord, &index)) &&
           (!bIsBndBox(szKeyWord))) {
#if MYDBG
        MessageBox(GetFocus(), GetStringRes(IDS_SKPTOEOL),
            GetStringRes(IDS_ERROR), MB_OK);
#endif
        if (!bGoNextLine(lpData, &index)) {
            MessageBox(GetFocus(), GetStringRes(IDS_MUSTNOTEOF),
                GetStringRes(IDS_ERROR), MB_OK);
            return FALSE;
        }
    }

    if (bIsBndBox(szKeyWord)) {
        if (bGetWord(lpData, szValue, &index))
            prctl->left    = atol(szValue);
        else {
            MessageBox(GetFocus(), GetStringRes(IDS_GETBNDLFTFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
            return FALSE;
        }

        if (bGetWord(lpData, szValue, &index))
            prctl->top     = atol(szValue);
        else {
            MessageBox(GetFocus(), GetStringRes(IDS_GETBNDTOPFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
            return FALSE;
        }

        if (bGetWord(lpData, szValue, &index))
            prctl->right   = atol(szValue);
        else {
            MessageBox(GetFocus(), GetStringRes(IDS_GETBNDRGTFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
            return FALSE;
        }

        if (bGetWord(lpData, szValue, &index))
            prctl->bottom  = atol(szValue);
        else {
            MessageBox(GetFocus(), GetStringRes(IDS_GETBNDBOTFAIL),
                GetStringRes(IDS_ERROR), MB_OK);
            return FALSE;
        }
    }

    return TRUE;

}


BOOL bGetWord(LPVOID lpData, char *str, int* pi)
{
    char *pstr;

    pstr = str;

    while (((char *)lpData)[*pi] == ' ')
        (*pi)++;

    while ((((char *)lpData)[*pi] != ' ') &&
           (((char *)lpData)[*pi] != '\n') &&
           (((char *)lpData)[*pi] != '\r')) {
        *str++ = ((char *)lpData)[(*pi)++];
    }
    *str++ = '\0';

#if MYDBG
    {
    char text[128];

    wsprintf(text, "bGetWord gets %s", pstr);
    MessageBox(GetFocus(), text, "Info", MB_OK);
    }
#endif

    return TRUE;
}


BOOL bGoNextLine(LPVOID lpData, int* pi)
{
    char tmp[128];
    int  q;

    while ((((char *)lpData)[*pi] != '\n') &&
           (((char *)lpData)[*pi] != '\r'))
        (*pi)++;

    //
    // skip them
    //
    *pi += 2;

    q = *pi;

    if ((bGetWord(lpData, tmp, &q)) && (bIsEOF(tmp)) )
        return FALSE;

    return TRUE;
}


/***************************************************************************\
* bSetAdvancedGraphics
*
* Set the HDC to advanced graphics mode to get world transforms, and other
*  features...  However, only do this if we are running on Windows NT, as
*  the other platforms don't support GM_ADVANCED.
*
\***************************************************************************/
BOOL bSetAdvancedGraphics(HDC hdc)
{
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

    GetVersionEx (&osvi);
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)

      if ((SetGraphicsMode(hdc, GM_ADVANCED)) == 0) {
	 MessageBox(ghwndMain, GetStringRes(IDS_SETADVGRPHMDFAIL), NULL, MB_OK);
      }
    return TRUE;
}

/******************************************************************************\
*
*  FUNCTION:    GetStringRes (int id INPUT ONLY)
*
*  COMMENTS:    Load the resource string with the ID given, and return a
*               pointer to it.  Notice that the buffer is common memory so
*               the string must be used before this call is made a second time.
*
\******************************************************************************/

LPTSTR   GetStringRes (int id)
{
  static TCHAR buffer[MAX_PATH];

  buffer[0]=0;
  LoadString (GetModuleHandle (NULL), id, buffer, MAX_PATH);
  return buffer;
}

LPTSTR   GetStringRes2 (int id)
{
  static TCHAR buffer[MAX_PATH];

  buffer[0]=0;
  LoadString (GetModuleHandle (NULL), id, buffer, MAX_PATH);
  return buffer;
}

/******************************************************************************\
*
*  FUNCTION:    BuildFilterStrs (int id INPUT ONLY)
*
*  COMMENTS:    Construct a filter string by a series of LoadString calls. The
*		resulting string is as follows: "%OldStr%\0%IDStr%\0%(ID+1)Str%\0\0".
*		Note that each consecutive function call appends the new filter
*		string on to the end of a static buffer. Call the function with
*		id = -1 to reinitialize the buffer.
*
\******************************************************************************/

LPTSTR	BuildFilterStrs(int id)
{
  static TCHAR buffer[1024];
  static TCHAR *pBuf;

  if (id == -1)
  {
	pBuf = buffer;
  }
  else
  {
	LoadString(GetModuleHandle (NULL), id, pBuf, 1024 - (pBuf-buffer));
	pBuf += strlen(pBuf) + 1;
	LoadString(GetModuleHandle (NULL), id + 1, pBuf, 1024 - (pBuf-buffer));
	pBuf += strlen(pBuf) + 1;
  }

  *pBuf = 0;
  *(pBuf+1) = 0;
  return buffer;
}
