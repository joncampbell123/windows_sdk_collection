/**[f******************************************************************
 * ttfont.c - TrueType font management module.
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1990,1991 Microsoft Corporation.
 * Company confidential.
 *
 * This module is responsible for handling all the issues related to the
 * downloading and maintaining of TrueType fonts as Adobe Type 1 or Type
 * 3 fonts.
 *
 * msd 12Mar91 Added support for devices that accept fonts in the
 *             TrueType format.
 **f]*****************************************************************/

#include "pscript.h"
#include "channel.h"
#include "etm.h"
#include "truetype.h"
#include "resource.h"
#include "printers.h"
#include "getdata.h"
#include "utils.h"
#include "pst1enc.h"
#include "adobe.h"
#include "psdata.h"

/* windows.h hack: rather than include the whole file, include the
** extra functions we need.
*/
#define SYMBOL_CHARSET		2
DWORD FAR PASCAL GetFontData(HDC, DWORD, DWORD, LPSTR, DWORD);
int FAR PASCAL GetTextFace(HDC, int, LPSTR);
HFONT FAR PASCAL CreateFontIndirect(LOGFONT FAR *);
HANDLE FAR PASCAL SelectObject(HDC, HANDLE);
BOOL   FAR PASCAL DeleteObject(HANDLE);

/* FONTSKELSIZE is a guestimate of the printer VM chewed up by a skeleton
** font definition(i.e. no character definitions.)  This is used to 
** maintain our guess of how much memory is left in the printer.  When we
** have bidirectionality this hack can be removed.
*/
#define FONTSKELSIZE    1536

/* Macro to compute how many bytes bi bits takes when aligned on an al-byte
** boundary.
*/
#define BiToByAl(bi,al) (((bi)+(al)*8-1)/((al)*8)*(al))

//  macro to truncate the fraction from 16-16 fixed pt numbers

#define TRUNC(fixed)  ((LONG)fixed & 0xffff0000)

/* List of font weight strings that will be plugged into the Type 1 header. */

static char *szWeight[] = { "Thin", "ExtraLight", "Light", "Normal", 
                            "Medium", "SemiBold", "Bold", "ExtraBold", 
                            "Heavy"
                          };


/* Data structures for reading TrueType font files and computing
** the exact length of the data.
*/
typedef struct TT_OFFSET_TABLE
   {
   long    version;
   USHORT  numTables;
   USHORT  searchRange;
   USHORT  entrySelector;
   USHORT  rangeShift;
   } TT_OFFSET_TABLE;

typedef struct TT_DIRECTORY
   {
   ULONG  tag;
   ULONG  checkSum;
   ULONG  offset;
   ULONG  length;
   } TT_DIRECTORY;

/****************** Prototypes of local functions *************************/

WORD  FAR PASCAL ReturnRefEM(LPDF  lpdf);
DWORD  PASCAL PutT3CharDown(LPDV lpdv, LPDF lpdf, LPTTFONTINFO lpttfi,
                          int ch, LPBYTE lpBuf, DWORD dwBufSize);
void PASCAL PutT1CharDown(LPDV lpdv, LPDF lpdf, LPTTFONTINFO lpttfi,
                          int ch);
int NEAR PASCAL TTDownLoadT1Font(LPDV lpdv, LPDF lpdf, BOOL bFullFont);
int NEAR PASCAL TTDownLoadT3Font(LPDV lpdv, LPDF lpdf, BOOL bFullFont);
int NEAR PASCAL TTDownLoadTTFont(LPDV lpdv, LPDF lpdf, BOOL bFullFont);
int NEAR PASCAL TTUpdateT1Font(LPDV lpdv, LPDF lpdf, LPBYTE lpStr, int cb);
int NEAR PASCAL TTUpdateT3Font(LPDV lpdv, LPDF lpdf, LPBYTE lpStr, int cb);
int FAR PASCAL GetRefEM(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                           DWORD dwData);

WORD NEAR PASCAL TranslMotorolaShort(WORD motorola_short);
DWORD NEAR PASCAL TranslMotorolaLong(DWORD motorola_long);
DWORD NEAR PASCAL sizesfnt(HDC hDC);

WORD FAR PASCAL TTGetDLFlags(LPDV lpdv, LPDF lpdf);
void FAR PASCAL TTSetDLFlags(LPDV lpdv, LPDF lpdf, WORD wNewFlags);

void FAR PASCAL OutputDebugString(LPSTR);

// array to hold outline definition.  This is a temporary fix until the
// rasterizer returns the outline buffer size required.  That is in now
// so this array will go away soon.
static char buf[4000];


/****************************************************************
* Name: TTDownloadFont()
*
* Action: Main routine to download a TrueType font.  lpdf points to
*	  the physical font created by TTRealizeFont().  bFullFont
*	  takes on the following meaning:
*
*	     TRUE   download font header and all characters
*            FALSE  download font header only
*
*	  After doing some common housekeeping the code calls the Type 1
*	  or Type 3 font generator as appropriate.  The decision is
*         determined in TTRealizeFont() and reflected in the TYPE_OUTLINE
*         bit of lpdv->dfType.
*
* Returns: 1 if successful, 0 otherwise.
*
**************************************************************/

int FAR PASCAL TTDownLoadFont(LPDV lpdv, LPDF lpdf, BOOL bFullFont)
{
    WORD wFlags;            /* current font flags */
    int rc;                 /* return value of function */
    LPTTFONTINFO lpttfi;    /* TrueType info structure */
    WORD  TTRefEM ;         /*  Local copy of TTRefEM  */

    rc = 1;


    /*  NOT!  EngineEnumerateFont(((LPSTR)lpdf) + lpdf->dfFace, GetRefEM, 0L);  */
    //  make this call to obtain TTRefEM - Above call requires hDC

    /* get TrueType info (stored as offset in dfBitsOffset) */
    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);

    TTRefEM = lpttfi->TTRefEM;

    /* validate pointers in lpdf and get the download flags */
    wFlags = TTLockFont(lpdv, lpdf);

    /* If we are supposed to download new font definitions on every page
    ** AND we have started a new page, then flush the download state.
    */
    if (lpdv->bPerPage && lpttfi->iPageNumber != lpdv->iPageNumber) {
        wFlags &= ~TTFLAGS_HEADERDOWN;
        TTSetDLFlags(lpdv, lpdf, wFlags);
        lmemset(lpttfi->prgfGlyphDown, 0, CWGLYPHFLAGS * sizeof(WORD));
        lpdv->DLState = DL_NOTHING;
    }

    /* If the skeleton is already downloaded then there is nothing left
    ** to do.  Note: We don't have to consider bFullFont here because
    ** the header is already in place and the caller should have set
    ** bFullFont the first time if he/she wanted the full character set
    ** downloaded.
    */
    if (wFlags & TTFLAGS_HEADERDOWN)
        goto done0;

    /* output document comment noting beginning of font resource */
    PrintChannel(lpdv, "\n%%%%BeginResource: font %s\n", lpttfi->TTFaceName);

    /* add size of font header to our "used printer VM" guestimate */
    lpdv->dwCurVM += FONTSKELSIZE;

    /* Call the appropriate font generation routine */
    if (lpdf->dfType & TYPE_OUTLINE) {
        if (lpdv->DevMode.iDLFontFmt == DLFMT_TRUETYPE && !lpdv->fDoEps) {
            if (!TTDownLoadTTFont(lpdv, lpdf, TRUE)) {
                rc = 0;
                goto done1;
            }
        } else if (!TTDownLoadT1Font(lpdv, lpdf, bFullFont)) {
            rc = 0;
            goto done1;
        }
    } else {
        if (!TTDownLoadT3Font(lpdv, lpdf, bFullFont)) {
            rc = 0;
            goto done1;
        }
    }

done1:
    /* output document comment noting end of font resource */
    PrintChannel(lpdv, "%%%%EndResource\n\n");

    /* mark the header as downloaded now */
    wFlags |= TTFLAGS_HEADERDOWN;
    TTSetDLFlags(lpdv, lpdf, wFlags);

done0:
    TTUnlockFont(lpdv, lpdf);
    return rc;
}


/****************************************************************
* Name: TTDownloadT3Font()
*
* Action: Generates the necessary font definition code to create a
*	  new Type 3 font.  The parameters are the same as TTDownloadFont.
*
* Returns: 1 if successful, 0 otherwise.
*
**************************************************************/

int NEAR PASCAL TTDownLoadT3Font(LPDV lpdv, LPDF lpdf, BOOL bFullFont)
{
    LONG EmPels;            // # pels per EM 
    LPTTFONTINFO lpttfi;
    LPBYTE lpBuf;            // buffer to hold TrueType character bitmaps
    HANDLE hBuf;            // handle to lpBuf
    DWORD dwBufSize, dwNewBufSize;
                            // size of lpBuf in bytes (supplied and needed).
    int ch;                 // character index
    SCALABLEFONTINFO FAR *lpScaFnt;
    WORD  UlineThick;
    int   UlinePos;         //  negative means below baseline.



    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);

    EmPels = ((long) Scale(lpttfi->lfPoints, lpdv->iRes, 72)) << 16;

    lpScaFnt = (SCALABLEFONTINFO FAR *)lpdf;

    //  note possibility of overflowing range of WORD when mul by 1000

    UlineThick = lpScaFnt->erUnderlineThick *
        1000 / (WORD)(EmPels >> 16);
    UlinePos = (lpScaFnt->erAscent - lpScaFnt->erUnderlinePos);
    UlinePos *= (int)(1000 / (WORD)(EmPels >> 16));
    

    /* There are some helper routines defined in the PS_T3HEADER resource
    ** that define a Type 3 font and add characters to it.  This only has
    ** to be downloaded once (or once per page if lpdv->bPerPage is TRUE.)
    */
    if (!(lpdv->DLState & DL_T3HEADER)) {
        DumpResourceString(lpdv, PS_DATA, PS_T3HEADER);
        lpdv->DLState |= DL_T3HEADER;
    }

    /* Output the command to generate the Type 3 font */
    PrintChannel(lpdv, "/%s [%F 0 0 0 0 0] %d %d [%F %F %F %F] [1 %ld div 0 0 1 %ld div 0 0] /%s GreNewFont\n",
                (LPSTR)lpttfi->TTFaceName,
                EmPels,
                UlineThick, UlinePos,
                -EmPels, -EmPels, EmPels, EmPels,
                EmPels >> 16, EmPels >> 16,
                (LPSTR)lpttfi->TTFaceName
                );

    /* remember the page we defined the font on */
    lpttfi->iPageNumber = lpdv->iPageNumber;

    /* Add font to list of supplied fonts */
    AddString(lpdv->slSupplied, lpttfi->TTFaceName, 0L);

    /* if a full font download is requested do it. */
    if (bFullFont) {
        /* because calling spooler may task switch, we must do this
           before every call to EngineGetGlyphBmp().  This call moved.
        EngineSetFontContext(lpdf, lpttfi->lfCopy.lfEscapement);  */

        /* compute the buffer size needed (remember DWORD alignment!) */
        dwBufSize = BiToByAl(lpdf->dfMaxWidth, 4);
        dwBufSize *= lpdf->dfPixHeight;

        /* get the buffer */
        hBuf = GlobalAlloc(GHND, dwBufSize);
        if (!hBuf)
            return 0;

        lpBuf = GlobalLock(hBuf);

        /* download all the character definitions */
        for (ch = (int)lpdf->dfFirstChar; ch <= (int)lpdf->dfLastChar; ++ch) 
        {
            dwNewBufSize = PutT3CharDown(lpdv, lpdf, lpttfi, ch, lpBuf, dwBufSize);
            if(dwNewBufSize)
            {
                GlobalUnlock(hBuf);
                hBuf = GlobalReAlloc(hBuf, dwNewBufSize, GHND);
                if (!hBuf)
                    return 0;

                lpBuf = GlobalLock(hBuf);
                dwBufSize = dwNewBufSize;
                PutT3CharDown(lpdv, lpdf, lpttfi, ch, lpBuf, dwBufSize);
            }
        }

        GlobalUnlock(hBuf);
        GlobalFree(hBuf);
    }

    return 1;
}


/****************************************************************
* Name: PutT3CharDown()
*
* Action: Generates the definition of a single character.  We currently
*	  use the imagemask operator to blast the character on the paper
*         where the character shape is used as mask to apply the current
*         color through.  The format of a character definition is as 
*         follows (example is for 'A', ASCII 65d, 41h):
*
*               /G41                % Character encoding name 
*               [xInc 0             % X advance and Y advance of origin to 
*                                   % next char
*               ulx uly lrx lry]    % bounding box of character (used by font
*                                   % cache)
*               /G41                % Character encoding name 
*               {                   % begin proc that draws character
*                 cx cy             % width and height of bitmap
*                 true              % image must be inverted (black <=> white)
*                 [1 0 0 -1 tx ty]  % 2D transform to convert bitmap 
*                                   % coordinates to user coordinates
*                 {<023F...>}       % bitmap data (hexadecimal format)
*                 imagemask         % draw the character
*               }                   % end of character draw proc
*               65                  % index of 'A' in Encoding array
*               /G41                % Character encoding name
*               /TT_Arial           % Font character should be added to
*               AddChar             % Helper function to define character
*
*
* Returns: zero  if all goes well else returns buf size needed for
*           bitmap if the one supplied was inadequate.
*
**************************************************************/

DWORD PASCAL PutT3CharDown(LPDV lpdv, LPDF lpdf, LPTTFONTINFO lpttfi,
                          int ch, LPBYTE lpBuf, DWORD dwBufSize)
{
    BITMAPMETRICS bmm;
    unsigned bpl, bpl32;
    int i, j, ctr;
    long  rc;
    DWORD  bufsizNeeded, scanlen;


    /* tell GDI to get ready for some font partying    */
    EngineSetFontContext(lpdf, lpttfi->lfCopy.lfEscapement);  
    /* Ask the rasterizer for the bitmap */
    rc = EngineGetGlyphBmp(lpdv->hdc, lpdf, ch, FD_QUERY_CHARIMAGE, 
                       lpBuf, dwBufSize, &bmm);

    if(rc == -1)  // failed
        rc = EngineGetGlyphBmp(lpdv->hdc, lpdf, 0, FD_QUERY_CHARIMAGE, 
                lpBuf, dwBufSize, &bmm);

    if(rc == -1)  // still failed
        return(0);  //  there is nothing we can do!

    scanlen = BiToByAl(bmm.sizlExtent.cx, 4);  // force to lie on DWORD boundary

    if((bufsizNeeded =  scanlen * bmm.sizlExtent.cy) > dwBufSize)
        return(bufsizNeeded);
        

#ifdef DEBUG   
    /* Sanity Check: bmm.pfxCharInc.x better match width reported by
     * EngineGetCharWidth!
     */
    if (!(lpdf->dfType & TYPE_HAVEWIDTHS)) {
        OutputDebugString("can't check for width mismatch yet!!!\r\n");
    } else if (HIWORD(bmm.pfxCharInc.x) != lpttfi->rgwWidths[ch]) {
        char szBuf[80];

        wsprintf(szBuf, "width mismatch: bmm=%d, width=%d\r\n",
                 HIWORD(bmm.pfxCharInc.x), lpttfi->rgwWidths[ch]);
        OutputDebugString(szBuf);
    }

#endif


    /* figure out # bytes per scan for source and destination bitmaps */
    bpl = (LOWORD(bmm.sizlExtent.cx) + 7) / 8;
    bpl32 = (LOWORD(bmm.sizlExtent.cx) + 31) / 32 * 4;

    
    /* send prefix of font definition */
    PrintChannel(lpdv, "/G%2X [%F %F %F %F %F %F]\n/G%2X {\n",
                    ch,
                    bmm.pfxCharInc.x,
                    bmm.pfxCharInc.y,
                    bmm.pfxOrigin.x,
                    bmm.pfxOrigin.y - ((LONG)bmm.sizlExtent.cy << 16),
                    bmm.pfxOrigin.x + ((LONG)bmm.sizlExtent.cx << 16),
                    bmm.pfxOrigin.y,
                    ch);

    /* Don't output non-marking glyphs! */
	 if (bmm.sizlExtent.cx && bmm.sizlExtent.cy) {
        /* send rest of prefix to actual bitmap data */
        PrintChannel(lpdv, "    %d %d true [1 0 0 -1 %F %F] {<", 
                        (int) bmm.sizlExtent.cx, (int) bmm.sizlExtent.cy, 
                        -bmm.pfxOrigin.x, bmm.pfxOrigin.y);

        /* dump the bitmap contents in hexadecimal format */
        ctr = 0;
        for (i = 0; i < (int) LOWORD(bmm.sizlExtent.cy); i++)
            for (j = 0; j < (int) bpl; j++) {
                PrintChannel(lpdv, "%2X", lpBuf[i * bpl32 + j]);
                if (!(++ctr % 40))
                    PrintChannel(lpdv, newline);
            }

        /* send rest of character drawing code */
        PrintChannel(lpdv, ">} imagemask \n  }\n");
  
        /* add size of bitmap */
        lpdv->dwCurVM += dwBufSize;

    } else {
        /* generate null drawing procedure */
        PrintChannel(lpdv, "} \n");
    }

    /* send rest of character definition command */
    PrintChannel(lpdv, "  %d /G%2X %s AddChar\n", ch, ch, lpttfi->TTFaceName);

    /* remember that we downloaded this character */
    SETCHARDOWN(lpttfi, ch);
    return(0);
}


/****************************************************************
* Name: TTDownloadT1Font()
*
* Action: Generates the necessary font definition code to create a
*	  new Type 1 font.  The parameters are the same as TTDownloadFont.
*
* Returns: 1 if successful, 0 otherwise.
*
**************************************************************/


#define  SHIFTBITS  0

// See pst1enc.c - CSAddNumber()  which also defines SHIFTBITS

int NEAR PASCAL TTDownLoadT1Font(LPDV lpdv, LPDF lpdf, BOOL bFullFont)
{
    HANDLE hBuf;
    PBYTE pBuf;
    WORD            wBufLen;
    WORD  TTRefEM ;         /*  Local copy of TTRefEM  */
    DWORD           dw;
    LONG EmPels;
    LPTTFONTINFO lpttfi;
    SCALABLEFONTINFO FAR *lpScaFnt;
    LPDF  lpdfRef;
    int wt, ch;
    int i;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
    TTRefEM = lpttfi->TTRefEM;

    lpdfRef = TTGetBaseFont(lpdv, lpdf);

    lpScaFnt = (SCALABLEFONTINFO FAR *)lpdfRef;

    EmPels = ((long) TTRefEM) << SHIFTBITS;

    /*  Convert weight to index into weight string table 
        *  (formula based on FW_* constants) 
        */
    wt = max(0, (lpdf->dfWeight / 100) - 1);

    /* output the ASCII text portion of the Type 1 header */
    PrintChannel(lpdv, MAKEINTRESOURCE(PS_T1HEADER1),
                    lpttfi->TTFaceName,
                    lpttfi->TTFaceName,
                    (LPSTR) szWeight[wt],
                    lpttfi->lfCopy.lfEscapement / 10,
                    (LPSTR) ((lpdf->dfPitchAndFamily & 1) ? "false" : "true"),
                    //  From header.ps line 861
                    //  the underline info should be expressed
                    //  in a cooridinate grid of dimensions 1000 x 1000.
                    Scale((lpScaFnt->erAscent - lpScaFnt->erUnderlinePos),
                        1000 , TTRefEM),
                    Scale(lpScaFnt->erUnderlineThick,
                        1000 , TTRefEM),
                    lpttfi->TTFaceName,
                    0,  // paint type (0 = filled, 2 = outline)
                    EmPels,  // the character outline data is not rasterized
                    EmPels,  //  on a 1000 x 1000 grid nor is it ever
                             //  converted to that format.
                    lpdf->dfFirstChar,
                    lpdf->dfLastChar,
                    0, 0, 0, 0
                );

    /* tell support code we are starting the encrypted portion */
    StartEExec(lpdv);

    /* output the encrypted header.  The last parameter is the number of 
    ** characters in the character set.
    */
    efprintf(lpdv, MAKEINTRESOURCE(PS_T1EHEADER1), 256);

    /* remember the page we downloaded the font on in case lpdv->bPerPage is
    ** TRUE.
    */
    lpttfi->iPageNumber = lpdv->iPageNumber;

    /* Add font to list of supplied fonts */
    AddString(lpdv->slSupplied, lpttfi->TTFaceName, 0L);

    /* If the entire character set should be downloaded now do it. */
    if (bFullFont) {
        /* moved   EngineSetFontContext(lpdfRef, 0);  */

        for (ch = (int)lpdf->dfFirstChar; ch <= (int)lpdf->dfLastChar; ++ch)
            PutT1CharDown(lpdv, lpdf, lpttfi, ch);
    }

    /* Every font must have a .notdef character! */
    CharString(STARTCHAR);
    CharString(SBW, 0L, 0L, 0L, 0L);    // make the origin stay the same

    dw = CharString(ENDCHAR);
    hBuf = HIWORD(dw);
    wBufLen = LOWORD(dw);
    pBuf = (PBYTE) LocalLock(hBuf);

    efprintf(lpdv, "/.notdef %d RD ", wBufLen);
    eexec(lpdv, pBuf, wBufLen, FALSE);
    efprintf(lpdv, " ND\n");

    /* add size of .notdef definition */
    lpdv->dwCurVM += wBufLen;

    LocalUnlock(hBuf);
    LocalFree(hBuf);

    /* finish up the encrypted potion */
    efprintf(lpdv, MAKEINTRESOURCE(PS_T1EFOOTER1));

    /* tell support code we're done with encrypted part */
    EndEExec(lpdv);

    /* eexec encryption requires that eexec data is followed by 512 ASCII 
    ** '0's and a cleartomark operator.  Stupid but that's the way it is.
    */
    PrintChannel(lpdv, newline);
    for (i = 0; i < 512; ++i) {
        WriteChannelChar(lpdv, '0');
        if (!((i+1) & 63))
            PrintChannel(lpdv, newline);
    }
    PrintChannel(lpdv, "\ncleartomark\n");

    return 1;
}

/****************************************************************
* Name: PutT1CharDown()
*
* Action: Generates the necessary font definition code to create a
*	  new Type 3 font.  The parameters are the same as TTDownloadFont.
*
* Returns: 1 if successful, 0 otherwise.
*
**************************************************************/

void PASCAL PutT1CharDown(LPDV lpdv, LPDF lpdf, LPTTFONTINFO lpttfi,
                          int ch)
{
    HANDLE hBuf;
    PBYTE pBuf;
    BITMAPMETRICS bmm;
    LPPOLYGONHEADER lpPoly;
    LPFDPOLYCURVE   lpfdPoly;
    ULONG           cbPoly, cbRem; 
    LONG            xCurrent, yCurrent;
    WORD            wBufLen;
    DWORD           dw;
    USHORT          us;
    POINTFX         fxpt[3];
    LPDF            lpdfRef;

    /* tell GDI to get ready for some font partying    */
    lpdfRef = TTGetBaseFont(lpdv, lpdf);
    EngineSetFontContext(lpdfRef, 0 /*  Escapement  */);  

    cbPoly = EngineGetGlyphBmp(lpdv->hdc, lpdfRef, ch, FD_QUERY_OUTLINE, (LPBYTE) buf, 4000,
                                &bmm);

    if (cbPoly == -1) {
#ifdef DEBUG
        char s[80];

        wsprintf(s, "Warning: Char %d outline unavailable!\r\n", ch);
        OutputDebugString(s);
#endif
        SETCHARDOWN(lpttfi, ch);
        return;
    }

    // signal start of character definition
    CharString(STARTCHAR);

    /* Output the sidebearing and width information.  Use the horizontal
    ** side bearing/width operator if the Y origin and widths are 0. 
    */
    xCurrent = bmm.pfxOrigin.x;
    yCurrent = bmm.pfxOrigin.y;

    CharString(SBW, xCurrent, yCurrent, bmm.pfxCharInc.x, 
                bmm.pfxCharInc.y);

    if (!cbPoly)  //  if no Glyph exists in case of blank characaters...
        goto DoneOutlineDefinition;

    cbRem = cbPoly;
    lpfdPoly = (LPFDPOLYCURVE)buf;

    do {
        lpPoly = (LPPOLYGONHEADER) lpfdPoly;
        lpfdPoly = (LPFDPOLYCURVE) ((LPSTR)lpPoly + sizeof(POLYGONHEADER));

        cbPoly = lpPoly->cb - sizeof(POLYGONHEADER);
        cbRem -= sizeof(POLYGONHEADER);

        /* remember last point so we can generate relative commands */
        CharString(RMOVETO, TRUNC(lpPoly->pteStart.x) - TRUNC(xCurrent),
            TRUNC(lpPoly->pteStart.y) - TRUNC(yCurrent));
        xCurrent = lpPoly->pteStart.x;
        yCurrent = lpPoly->pteStart.y;

        while (cbPoly > 0) {
            switch (lpfdPoly->iType) {
                case FD_PRIM_QSPLINE:

                    for (us = 0; (us+1) < lpfdPoly->cptfx; ++us) {
                        if ((us + 2) < lpfdPoly->cptfx) {
                            fxpt[2].x = (lpfdPoly->pte[us].x + lpfdPoly->pte[us+1].x) >> 1;
                            fxpt[2].y = (lpfdPoly->pte[us].y + lpfdPoly->pte[us+1].y) >> 1;
                        } else {
                            fxpt[2] = lpfdPoly->pte[us+1];
                        }
                        fxpt[0].x = (xCurrent + (lpfdPoly->pte[us].x << 1)) / 3;
                        fxpt[0].y = (yCurrent + (lpfdPoly->pte[us].y << 1)) / 3;
                        fxpt[1].x = (fxpt[2].x + (lpfdPoly->pte[us].x << 1)) / 3;
                        fxpt[1].y = (fxpt[2].y + (lpfdPoly->pte[us].y << 1)) / 3;

                        CharString(RRCURVETO, 
                            TRUNC(fxpt[0].x) - TRUNC(xCurrent),
                            TRUNC(fxpt[0].y) - TRUNC(yCurrent), 
                            TRUNC(fxpt[1].x) - TRUNC(fxpt[0].x), 
                            TRUNC(fxpt[1].y) - TRUNC(fxpt[0].y), 
                            TRUNC(fxpt[2].x) - TRUNC(fxpt[1].x), 
                            TRUNC(fxpt[2].y) - TRUNC(fxpt[1].y));
                        xCurrent = fxpt[2].x;
                        yCurrent = fxpt[2].y;
                    }
                    break;

                default:
                    for (us = 0; us < lpfdPoly->cptfx; ++us) {
                        CharString(RLINETO, 
                            TRUNC(lpfdPoly->pte[us].x) - TRUNC(xCurrent),
                            TRUNC(lpfdPoly->pte[us].y) - TRUNC(yCurrent));
                        xCurrent = lpfdPoly->pte[us].x;
                        yCurrent = lpfdPoly->pte[us].y;
                    }
                    break;
            }

            us = (lpfdPoly->cptfx - 1) * sizeof(POINTFX) + sizeof(FDPOLYCURVE);
            lpfdPoly = (LPFDPOLYCURVE) ((LPSTR)lpfdPoly + us);
            cbPoly -= us;
            cbRem -= us;
        }

        CharString(CLOSEPATH);

    } while (cbRem > 0);

DoneOutlineDefinition:
    dw = CharString(ENDCHAR);
    hBuf = HIWORD(dw);
    wBufLen = LOWORD(dw);
    pBuf = (PBYTE) LocalLock(hBuf);

    efprintf(lpdv, "/G%2X %d RD ", ch, wBufLen);
    eexec(lpdv, pBuf, wBufLen, FALSE);
    efprintf(lpdv, " ND ");

    /* add size of .notdef definition */
    lpdv->dwCurVM += wBufLen;

    LocalUnlock(hBuf);
    LocalFree(hBuf);

    SETCHARDOWN(lpttfi, ch);
}

/****************************************************************
* Name: TTDownloadTTFont()
*
* Action: FOR DEVICES THAT ACCEPT RAW TRUETYPE FONTS, e.g., TrueImage
*         printers, dump down the entire TrueType font using the
*         readhexsfnt operator.
*
*         Font size calculation code compliments of ccteng.
*
* Returns: 1 if successful, 0 otherwise.
*
**************************************************************/

int NEAR PASCAL TTDownLoadTTFont(LPDV lpdv, LPDF lpdf, BOOL bFullFont)
{
#define MAX_TTCOL_WIDTH 80 /* must be an even number */
#define MAX_TTBUF_SIZE  (MAX_TTCOL_WIDTH * 200) /* 16Kb xfer buffer */

    int success = 0;
    WORD i;
    WORD j;
    WORD wNum;
    DWORD dwTTSize;
    DWORD dwBufSize;
    DWORD dwNumBytes;
    DWORD dwOffset;
    HDC hDC;
    HFONT hTTFont;
    HFONT hPrevFont;
    HANDLE hBuf;
    LPTTFONTINFO lpttfi;
    LPSTR lpBuf;
    LOGFONT logfont;
    char facenm[LF_FACESIZE];

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);

    /* Get the screen DC.
    */
    if (!(hDC=GetDC(NULL)))
        goto backout0;

    /* Create the TrueType engine font using the stored copy
    ** of the logical font structure.  If we do not actually
    ** get the TrueType font then we're hosed.
    */
    logfont = lpttfi->lfCopy;
#if 0
    /* Symbol PS has the wrong character set and pitch and family,
    ** but it doesn't really matter because the printer does not
    ** accept the downloaded font anyways.
    */
    if (lstrcmpi(logfont.lfFaceName, "Symbol PS") == 0) {
        logfont.lfCharSet = SYMBOL_CHARSET;
        logfont.lfPitchAndFamily = 0;
    }
#endif
    if (!(hTTFont=CreateFontIndirect(&logfont)))
        goto backout1;
    if (!(hPrevFont=SelectObject(hDC,hTTFont)))
        goto backout2;

    if (!GetTextFace(hDC,sizeof(facenm),facenm))
        goto backout3;
    facenm[lstrlen(logfont.lfFaceName)] = '\0';
    if (lstrcmpi(facenm,lpttfi->lfCopy.lfFaceName))
        goto backout3;

    /* Compute the size of the font by adding up the sizes in
    ** the font's header.  This is more robust than assuming that
    ** the font file itself is the correct length.  If we download
    ** more or less bytes than indicated in the font header then
    ** the printer will fall out of sync and most likely will not
    ** recover.
    */
    if (!(dwTTSize=sizesfnt(hDC)))
        goto backout3;

    /* Allocate a buffer used for copying the font.  We will be
    ** using the bintoascii proc which requires a buffer twice
    ** the size of the data.
    */
    if (dwTTSize > (MAX_TTBUF_SIZE / 2))
        dwBufSize = MAX_TTBUF_SIZE;
    else
        dwBufSize = dwTTSize * 2;
    if (!(hBuf = GlobalAlloc(GMEM_MOVEABLE, dwBufSize)))
        goto backout3;
    if (!(lpBuf = GlobalLock(hBuf)))
        goto backout4;

    /* Prepend the data with the command to build a sfnt.
    */
    PrintChannel(lpdv, "mark { currentfile readhexsfnt } stopped\n");

    /* Dump the sfnt (TrueType font) in hex format.
    */
    for (dwOffset = 0L; dwOffset < dwTTSize; dwOffset += dwNumBytes) {

        /* Compute max number of bytes we can read (up to half
        ** the size of the buffer, the other half is used in the
        ** hex conversion).
        */
        if ((dwNumBytes = dwBufSize / 2) > (dwTTSize - dwOffset))
            dwNumBytes = dwTTSize - dwOffset;

        if ((dwNumBytes = GetFontData(hDC, 0L, dwOffset, lpBuf,
                dwNumBytes)) > 0) {

            /* Convert from binary to hex.
            */
            Bin2Ascii(lpBuf, (WORD)dwNumBytes);

            /* Walk the buffer dumping in MAX_TTCOL_WIDTH chunks.
            */
            for (i = 0, wNum = (WORD)dwNumBytes * 2; i < wNum; i += j) {
               if ((j = MAX_TTCOL_WIDTH) > (wNum - i))
                    j = wNum - i;
                WriteChannel(lpdv, &lpBuf[i], j);
                PrintChannel(lpdv, newline);
            }
        } else {
            /* We encountered an error attempting to read the font,
            ** so pad out the rest of the length of the font with
            ** hex zeroes (there are two 0's for each byte of data
            ** remaining to be dumped).
            */
            lmemset(lpBuf, (BYTE)'0', (WORD)dwBufSize);

            for (; dwOffset < dwTTSize; dwOffset += dwNumBytes) {

                if ((dwNumBytes = dwBufSize / 2) > (dwTTSize - dwOffset))
                    dwNumBytes = dwTTSize - dwOffset;

                for (wNum = (WORD)dwNumBytes * 2; wNum > 0; wNum -= j) {
                   if ((j = MAX_TTCOL_WIDTH) > wNum)
                        j = wNum;
                    WriteChannel(lpdv, lpBuf, j);
                    PrintChannel(lpdv, newline);
                }
            }
        }
    }

    /* Conclude the data by assigning a font name to the new sfnt.
    ** If the printer encountered an error during the download, then
    ** we just pop everything without assigning a name to the font.
    */
    PrintChannel(lpdv, "not { /%s exch definefont } if cleartomark\n",
        lpttfi->TTFaceName);

    /* Update current VM value, accomodate what has already
    ** been added for this font.
    */
    lpdv->dwCurVM += dwTTSize;

    success = 1;

    GlobalUnlock(hBuf);
backout4:
    GlobalFree(hBuf);
backout3:
    SelectObject(hDC,hPrevFont);
backout2:
    DeleteObject(hTTFont);
backout1:
    ReleaseDC(NULL,hDC);
backout0:
    return success;
}


/****************************************************************
* Name: sizesfnt()
*
* Action: Reads a TrueType font and computes its exact size based
*         upon the numbers in the font's tables.
*
* Returns: The size of the font in bytes.
*
**************************************************************/

DWORD NEAR PASCAL sizesfnt(HDC hDC)
{
    int i;
    int nTables;
    DWORD dwTblSize;
    DWORD dwMaxOffset;
    DWORD dwMaxLength = 0;
    DWORD dwOffset;
    DWORD dwPad;
    HANDLE hTbl;
    TT_DIRECTORY FAR *lpTbl;
    TT_OFFSET_TABLE dir;

    /* Get the file header.
    */
    if (GetFontData(hDC, 0L, 0L, (LPSTR)&dir,
            sizeof(TT_OFFSET_TABLE)) != sizeof(TT_OFFSET_TABLE)) {
        goto backout0;
    }
    nTables = TranslMotorolaShort(dir.numTables);

    /* Allocate an array of directory structures (one for each table).
    */
    dwTblSize = (DWORD)nTables * sizeof(TT_DIRECTORY);
    if (!(hTbl = GlobalAlloc(GMEM_MOVEABLE, dwTblSize)))
        goto backout0;
    if (!(lpTbl = (TT_DIRECTORY FAR *)GlobalLock(hTbl)))
        goto backout1;

    /* Read the array of tables.
    */
    if (GetFontData(hDC, 0L, sizeof(TT_OFFSET_TABLE), (LPSTR)lpTbl,
            dwTblSize) != dwTblSize) {
        goto backout2;
    }

    /* Walk the array of tables finding the last one in the
    ** file.  Its location and length rounded out to the nearest
    ** 4 byte boundary constitutes the number of bytes the printer
    ** will expect.
    */
    for (i = 0, dwMaxOffset = 0; i < nTables; i++, lpTbl++) {
        dwOffset = TranslMotorolaLong(lpTbl->offset);
        if (dwMaxOffset < dwOffset) {
            dwMaxOffset = dwOffset;
            dwMaxLength = TranslMotorolaLong(lpTbl->length);
        }
    }
    if (dwPad = dwMaxLength % 4)
        dwPad = 4 - dwPad;
    dwMaxLength += dwMaxOffset + dwPad;

backout2:
    GlobalUnlock(hTbl);
backout1:
    GlobalFree(hTbl);
backout0:
    return(dwMaxLength);
}


/****************************************************************
* Name: TranslMotorolaShort()
*
* Action: Converts Motorola short to Intel format.
*
* Returns: Intel short.
*
**************************************************************/

WORD NEAR PASCAL TranslMotorolaShort(WORD motorola_short)
{
   BYTE FAR *motorola_bytes = (BYTE FAR *)&motorola_short;

   return (  ((WORD) motorola_bytes[1]) |
            (((WORD) motorola_bytes[0]) << 8) );
}


/****************************************************************
* Name: TranslMotorolaLong()
*
* Action: Converts Motorola long to Intel format.
*
* Returns: Intel long.
*
**************************************************************/

DWORD NEAR PASCAL TranslMotorolaLong(DWORD motorola_long)
{
   BYTE FAR *motorola_bytes = (BYTE FAR *)&motorola_long;

   return ( (((DWORD) motorola_bytes[0]) << 24) |
            (((DWORD) motorola_bytes[1]) << 16) |
            (((DWORD) motorola_bytes[2]) << 8 ) |
             ((DWORD) motorola_bytes[3]) );
}

/****************************************************************
* Name: TTUpdateT3Font()
*
* Action: Adds all of the characters from lpStr that aren't already
*	  downloaded for the TrueType font in lpdf.
*
* Returns: 1 if successful, 0 otherwise.
*
**************************************************************/

int NEAR PASCAL TTUpdateT3Font(LPDV lpdv, LPDF lpdf, LPBYTE lpStr, int cb)
{
    LPTTFONTINFO lpttfi;
    HANDLE hBuf;
    LPBYTE lpBuf;
    DWORD dwBufSize, dwNewBufSize;
    BOOL bNoUpdate = TRUE;
    int ch;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);

    /* make sure we have a non-NULL pointer... */
    if (lpStr) {

        /* if cb is 0 we must calculate length */
        if (!cb)
            cb = lstrlen(lpStr);

        for (; cb; --cb, ++lpStr) {
            ch = *lpStr;

            /* if the character is not in the font or we already downloaded
            ** it go to next character.
            */
            if (ch < (int)lpdf->dfFirstChar 
                || ch > (int)lpdf->dfLastChar
                || ISCHARDOWN(lpttfi,ch))
                continue;

            /* make sure we have initialized ourself */
            if (bNoUpdate) {
                WORD  nlines;
                /*  moved  EngineSetFontContext(lpdf, lpttfi->lfCopy.lfEscapement); */

                /* compute buffer size (DWORD aligned) */
                dwBufSize = BiToByAl(lpdf->dfMaxWidth, 4);
                nlines = lpdf->dfPixHeight;
                dwBufSize *= nlines;

                hBuf = GlobalAlloc(GHND, dwBufSize);
                if (!hBuf)
                    return 0;

                lpBuf = GlobalLock(hBuf);

                /* send document comment stating start of font resource */
                PrintChannel(lpdv, "\n%%%%BeginResource: font %s\n", 
                             (LPSTR) lpttfi->TTFaceName);

                /* remember we have initialized */
                bNoUpdate = FALSE;
            }

            /* Generate the character definition */
            dwNewBufSize = PutT3CharDown(lpdv, lpdf, lpttfi, *lpStr, lpBuf, dwBufSize);
            if(dwNewBufSize)
            {
                GlobalUnlock(hBuf);
                hBuf = GlobalReAlloc(hBuf, dwNewBufSize, GHND);
                if (!hBuf)
                    return 0;

                lpBuf = GlobalLock(hBuf);
                dwBufSize = dwNewBufSize;
                PutT3CharDown(lpdv, lpdf, lpttfi, *lpStr, lpBuf, dwBufSize);
            }
        }

        /* If we initialized then cleanup */
        if (!bNoUpdate) {
            /* send document comment stating end of font resource */
            PrintChannel(lpdv, "%%%%EndResource\n\n");

            GlobalUnlock(hBuf);
            GlobalFree(hBuf);
        }
    }

    return 1;
}


/****************************************************************
* Name: TTUpdateT1Font()
*
* Action: Adds all of the characters from lpStr that aren't already
*	  downloaded for the TrueType font in lpdf.
*
* Returns: 1 if successful, 0 otherwise.
*
**************************************************************/

int NEAR PASCAL TTUpdateT1Font(LPDV lpdv, LPDF lpdf, LPBYTE lpStr, int cb)
{
    LPTTFONTINFO lpttfi;
    LPBYTE lpTmp;
    BOOL bNoUpdate;
    int i;

    if (!lpStr)
        return 1;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);

    /* moved inside PutT1CharDown    lpdfRef = TTGetBaseFont(lpdv, lpdf); */

    bNoUpdate = TRUE;

    // download the new glyphs
    for (lpTmp = lpStr, i = 0; i < cb; ++i, ++lpTmp) {
        if (ISCHARDOWN(lpttfi, *lpTmp))
            continue;

        if (bNoUpdate) {
            PrintChannel(lpdv, "%%%%BeginResource: font %s\ncurrentfile eexec\n", lpttfi->TTFaceName);

            StartEExec(lpdv);

            efprintf(lpdv, MAKEINTRESOURCE(PS_T1EHEADER2), lpttfi->TTFaceName);

            /*  moved  EngineSetFontContext(lpdfRef, 0); */

            bNoUpdate = FALSE;
        }

        PutT1CharDown(lpdv, lpdf, lpttfi, *lpTmp);
    }

    if (!bNoUpdate) {
        efprintf(lpdv, MAKEINTRESOURCE(PS_T1EFOOTER2));

        EndEExec(lpdv);

        PrintChannel(lpdv, newline);
        for (i = 0; i < 512; ++i) {
            WriteChannelChar(lpdv, '0');
            if (!((i+1) & 63))
                PrintChannel(lpdv, newline);
        }

        PrintChannel(lpdv, "\ncleartomark\n%%%%EndResource\n");
    }

    return 1;
}

int FAR PASCAL TTUpdateFont(LPDV lpdv, LPDF lpdf, LPSTR lpStr, int cb)
{
    int rc;
    LPTTFONTINFO lpttfi;

    rc = 1;
    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
    TTLockFont(lpdv, lpdf);

    if (lpdf->dfType & TYPE_OUTLINE) {
        if (lpdv->DevMode.iDLFontFmt == DLFMT_TRUETYPE && !lpdv->fDoEps)
            goto done;
        else if (!TTUpdateT1Font(lpdv, lpdf, lpStr, cb)) {
            rc = 0;
            goto done;
        }
    } else {
        if (!TTUpdateT3Font(lpdv, lpdf, lpStr, cb)) {
            rc = 0;
            goto done;
        }
    }

done:
    TTUnlockFont(lpdv, lpdf);
    return rc;
}

int FAR PASCAL TTLockFont(LPDV lpdv, LPDF lpdf)
{
    int rc;
    DWORD dw;
    HANDLE h;
    LPTTFONTINFO lpttfi;
    WORD  TTRefEM;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
    rc = 1;

    if (!IsStringInList(lpdv->slTTFonts, lpttfi->TTFaceName)) {
        h = LocalAlloc(LHND, sizeof(DWORD) + 2 * CWGLYPHFLAGS * sizeof(WORD));
        if (!h)
            goto error1;

        dw = MAKELONG(h, 0);

        if (!AddString(lpdv->slTTFonts, lpttfi->TTFaceName, dw))
            goto error2;

        if (lpdf->dfType & TYPE_OUTLINE) {
            LPDF *plpdf;

            plpdf = (LPDF *)LocalLock(h);
            
            /* create an unhinted font if it hasn't been created yet. */
            if (!*plpdf) {
                LOGFONT lfCopy;
                int cb;
                
                TTRefEM = lpttfi->TTRefEM;

                /* make font as unhinted with 0 escapment with rest 
                 * of LOGFONT same  */
                lfCopy = lpttfi->lfCopy;
                lfCopy.lfWidth = Scale(lfCopy.lfWidth, TTRefEM, 
                                       lpdf->dfPixHeight - lpdf->dfInternalLeading);
                lfCopy.lfHeight = -TTRefEM;
                lfCopy.lfEscapement = lfCopy.lfOrientation = 0;

                /* allocate space for new font */
                cb = EngineRealizeFont(&lfCopy, &lpttfi->ftCopy, NULL);
                if (!cb)
                    goto error3;
                *plpdf = (LPDF)MAKELONG(0, GlobalAlloc(GDLLPTR, cb));
                if (!*plpdf)
                    goto error3;

                /* fill in the font */
                cb = EngineRealizeFont(&lfCopy, &lpttfi->ftCopy, *plpdf);
            }

            LocalUnlock(h);
        }
    } else {
        dw = GetStringData(lpdv->slTTFonts, lpttfi->TTFaceName);
    }

    lpttfi->prgfGlyphDown = (WORD *)(LocalLock(LOWORD(dw)) + sizeof(DWORD));
    return HIWORD(dw);

error3:
    LocalUnlock(h);

error2: 
    LocalFree(h);

error1:
    return -1;
}


void FAR PASCAL TTUnlockFont(LPDV lpdv, LPDF lpdf)
{
    LPTTFONTINFO lpttfi;
    DWORD dw;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
    dw = GetStringData(lpdv->slTTFonts, lpttfi->TTFaceName);
    
    LocalUnlock(LOWORD(dw));
}


void FAR PASCAL TTFlushFonts(LPDV lpdv)
/*  this function has been declawed - all it does
    is reset the download flags.  */
{
    int nItems, i;
    DWORD dwItem;
    LPBYTE  lpLocalBuf;
    HANDLE  hStrList;

    hStrList = lpdv->slTTFonts;

    nItems = (int) SendMessage(hStrList, LB_GETCOUNT, 0, 0L);
    
    for (i = 0; i < nItems; ++i) 
    {
        dwItem = SendMessage(hStrList, LB_GETITEMDATA, i, 0L);
        if (LOWORD(dwItem))  
        {
            lpLocalBuf = (LPBYTE)LocalLock(LOWORD(dwItem));
            if (lpLocalBuf)
            {
                lmemset(lpLocalBuf + sizeof(DWORD), 0, 
                   2 * CWGLYPHFLAGS * sizeof(WORD));  // clear download flags
            }
            LocalUnlock(LOWORD(dwItem));
        }
        dwItem &= 0x0ffffL;  // clear highword
        SendMessage(hStrList, LB_SETITEMDATA, i, dwItem);
    }
}


WORD FAR PASCAL TTGetDLFlags(LPDV lpdv, LPDF lpdf)
{
    LPTTFONTINFO lpttfi;
    DWORD dw;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
    dw = GetStringData(lpdv->slTTFonts, lpttfi->TTFaceName);
    return HIWORD(dw);
}


void FAR PASCAL TTSetDLFlags(LPDV lpdv, LPDF lpdf, WORD wNewFlags)
{
    LPTTFONTINFO lpttfi;
    DWORD dw;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
    dw = GetStringData(lpdv->slTTFonts, lpttfi->TTFaceName);
    dw = MAKELONG(LOWORD(dw), wNewFlags);
    SetStringData(lpdv->slTTFonts, lpttfi->TTFaceName, dw);
}

LPDF FAR PASCAL TTGetBaseFont(LPDV lpdv, LPDF lpdf)
{
    LPDF lpdfBase;
    LPTTFONTINFO lpttfi;
    DWORD dw;

    lpttfi = (LPTTFONTINFO)((LPSTR)lpdf + lpdf->dfBitsOffset);
    dw = GetStringData(lpdv->slTTFonts, lpttfi->TTFaceName);

    lpdfBase = *((LPDF *)LocalLock(LOWORD(dw)));
    LocalUnlock(LOWORD(dw));

    return lpdfBase;
}

void FAR PASCAL TTSetBaseFont(LPDV lpdv, LPDF lpdf, LPDF lpdfBase)
{
    LPTTFONTINFO lpttfi;
    DWORD dw;

    lpttfi = (LPTTFONTINFO) ((LPSTR) lpdf + lpdf->dfBitsOffset);
    dw = GetStringData(lpdv->slTTFonts, lpttfi->TTFaceName);

    *((LPDF *)LocalLock(LOWORD(dw))) = lpdfBase;
    LocalUnlock(LOWORD(dw));
}

BOOL FAR PASCAL IsTrueTypeEnabled(void)
{
    BOOL bEnabled = FALSE;

    if (GetProfileInt("TrueType", "TTEnable", 1) &&
        GetProcAddress(GetModuleHandle("GDI"), "EngineDeleteFont") &&
        (GetWinFlags() & WF_PMODE))
            bEnabled = TRUE;

    return bEnabled;
}




int FAR PASCAL GetRefEM(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                           DWORD dwData)
{
    *((LPWORD)dwData) = ((LPNEWTEXTMETRIC)lptm)->ntmSizeEM;

    return 1;
}

WORD  FAR PASCAL ReturnRefEM(LPDF  lpdf)
{
    WORD  TTRefEM;

    EngineEnumerateFont(((LPSTR)lpdf) + lpdf->dfFace, GetRefEM, 
        (DWORD)(LPWORD)&TTRefEM);
    //  make this call to obtain TTRefEM for this font.

    return(TTRefEM);
}
