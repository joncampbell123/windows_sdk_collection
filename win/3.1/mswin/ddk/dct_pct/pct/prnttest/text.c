/*---------------------------------------------------------------------------*\
| TEXT TESTS                                                                  |
|   This module contains the routines for testing a printer devices text      |
|   capabilities.                                                             |
|                                                                             |
| DATE   : Jun 05, 1989                                                       |
|   Copyright   1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <windows.h>
#include    "PrntTest.h"

typedef struct _TextInfo    TEXTINFOSTRUCT, FAR *LPTEXTINFOSTRUCT;

struct _TextInfo
  {
    BOOL    bOpaque;
    int     iFont;
    int     iLocation;
    int     iSpaces;
    int     iFontHeight;
    int     iFontWidth;
    int     iSynthesis;
  };

typedef struct {
               int   iFont;
               int   iANSIOffset;
               int   iNumericOffset;
               BYTE  cTestChar;
               } WIDTHINFO, FAR *LPWIDTHINFO;

HFONT hANSIFont=0;


/******************************************************************************

    Private Function:   GetFont

    Purpose:            Gets the desired font

    Why code it over and over, when you can get it wrong once, and be done
    with it?

    Change History:

    10-26-1990  I'm responsible (nobody else believes that, though!)

******************************************************************************/

HFONT PASCAL GetFont(int iSynthesis,
                     int iFont)
{
  FONT  fFont;
  int   iObj;

  SetCurrentObject(tlTest.gtTest.hFonts, iFont);
  CopyDeviceObject((LPSTR)&iObj, tlTest.gtTest.hFonts);
  SetCurrentObject(hFonts, iObj);

  if (iSynthesis &
     (SYNFONT_ITALIC | SYNFONT_UNDERLINED | SYNFONT_STRIKEOUT))
  {

    CopyDeviceObject((LPSTR)&fFont, hFonts);

    if (iSynthesis & SYNFONT_ITALIC)
      fFont.lf.lfItalic = TRUE;
    if (iSynthesis & SYNFONT_UNDERLINED)
      fFont.lf.lfUnderline = TRUE;
    if (iSynthesis & SYNFONT_STRIKEOUT)
      fFont.lf.lfStrikeOut = TRUE;

    return CreateFontIndirect(&fFont.lf);
  }

  return CreateDeviceObject(hFonts);
}



void GetANSIFont(int  nHeight)
{
  static LOGFONT lf;

  if(hANSIFont)
    if(hANSIFont != GetStockObject(ANSI_VAR_FONT))
      DeleteObject(hANSIFont);

  lf.lfHeight=nHeight;
  lf.lfWidth=0;
  lf.lfEscapement=0;
  lf.lfOrientation=0;
  lf.lfWeight=400;
  lf.lfItalic=0;
  lf.lfUnderline=0;
  lf.lfStrikeOut=0;
  lf.lfCharSet=ANSI_CHARSET;
  lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
  lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
  lf.lfQuality=PROOF_QUALITY;
  lf.lfPitchAndFamily=VARIABLE_PITCH | FF_SWISS;
  lstrcpy(lf.lfFaceName,"Arial");

  if(!(hANSIFont=CreateFontIndirect(&lf)))
    hANSIFont=GetStockObject(ANSI_VAR_FONT);
}



DWORD GetANSIFontHeightWidth(int nHeight)
{
  TEXTMETRIC tm;
  HFONT      hOldFont;

  GetANSIFont(nHeight);

  hOldFont=SelectObject(hdcTarget,hANSIFont);
  GetTextMetrics(hdcTarget,&tm);
  SelectObject(hdcTarget,hOldFont);

  return (DWORD)MAKELONG(tm.tmHeight+tm.tmExternalLeading,tm.tmMaxCharWidth);
}


/*---------------------------------------------------------------------------*\
| ExtTextOut test                                                             |
|   This routine actually calls ExtTextOut along a given rectangle            |
|                                                                             |
| CALLED ROUTINES                                                             |
|                                                                             |
| PARAMETERS                                                                  |
|  hDC      Printer's DC                                                      |
|  wOptions Options passed to ExtTextOut                                        |
|  lpRect   Rectangle for clipping                                            |
|  lpString pointer to string to be output                                    |
|  nStrPos  identifies where text should be placed on rectangle               |
|  lpDX     pointer to array of additional spacing                            |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| 12/17/91  Moved from isg_misc.c to here.                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL TstExtTextOutRect(HDC    hDC,
                                  WORD   wOptions,
                                  LPRECT lpRect,
                                  LPSTR  lpString,
                                  short  nStrPos,
                                  LPINT  lpDX)
{
  short  nRectHeight,nRectWidth,nStrHeight,nStrWidth,nStrX,nStrY;
  HBRUSH hBrush;
  HPEN   hPen;

  // PatBlt a hatched brush into the rectangle to check for correct
  // opaquing (or lack thereof).  Only do this if the ETO_OPAQUE
  // bit is set in wOptions

  if(wOptions & ETO_OPAQUE)
  {
    hBrush=CreateHatchBrush(HS_CROSS,RGB(0,0,0));
    FillRect(hDC,lpRect,hBrush);
    DeleteObject(hBrush);
  }

  nRectHeight = lpRect->bottom - lpRect->top;
  nRectWidth  = lpRect->right - lpRect->left;
  nStrHeight  = HIWORD(GetTextExtent(hDC,lpString,lstrlen(lpString)));
  nStrWidth   = LOWORD(GetTextExtent(hDC,lpString,lstrlen(lpString)));

  switch(nStrPos)
  {
      case 0:
            nStrX = lpRect->left - (nStrWidth >> 1);
            nStrY = lpRect->top - (nStrHeight >> 1);
            break;
      case 1:
            nStrX = lpRect->left;
            nStrY = lpRect->top - (nStrHeight >> 1);
            break;
      case 2:
            nStrX = lpRect->right - (nStrWidth >> 1);
            nStrY = lpRect->top - (nStrHeight >> 1);
            break;
      case 3:
            nStrX = lpRect->left - (nStrWidth >> 1);
            nStrY = lpRect->top;
            break;
      case 4:
            nStrX = lpRect->left;
            nStrY = lpRect->top;
            break;
      case 5:
            nStrX = lpRect->right - (nStrWidth >> 1);
            nStrY = lpRect->top;
            break;
      case 6:
            nStrX = lpRect->left - (nStrWidth >> 1);
            nStrY = lpRect->top + (nRectHeight >> 1);
            break;
      case 7:
            nStrX = lpRect->left;
            nStrY = lpRect->top + (nRectHeight >> 1);
            break;
      case 8:
            nStrX = lpRect->right - (nStrWidth >> 1);
            nStrY = lpRect->top + (nRectHeight >> 1);
            break;
      default:
            return(FALSE);
  }

  ExtTextOut(hDC,nStrX,nStrY,wOptions,lpRect,
             lpString,lstrlen(lpString),lpDX);

  hBrush = SelectObject(hDC,GetStockObject(NULL_BRUSH));
  hPen = SelectObject(hDC,GetStockObject(BLACK_PEN));
  Rectangle(hDC,lpRect->left,lpRect->top,lpRect->right,lpRect->bottom);
  SelectObject(hDC,hBrush);
  SelectObject(hDC,hPen);

  return(TRUE);
}


/*---------------------------------------------------------------------------*\
| PRINT EXTTEXTOUT TEXT                                                       |
|   This routine test the output of a text string within a clipping rectangle |
|   defined on a page.  It prints the text using both ETO_CLIPPED and         |
|   ETO_OPAQUED rectangles.  The text itself is printed at nine positions on  |
|   the rectangle to verify the proper clipping of the string.  An array of   |
|   charcter spacings is also generated & used.                               |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter()    - misc.c                                                 |
|   TestExtTextOutRect()                                                      |
|                                                                             |
| PARAMETERS                                                                  |
|   LPRECT            lprcBounds   - pointer to placement rectangle           |
|   WORD              wBlkSize     - Size of data block                       |
|   LPTEXTINFOSTRUCT  lptis        - pointer to TEXTINFOSTRUCT                |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HANDLE hInst - Module instance of library (PrntFram)                      |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL LetMyETO(LPRECT           lprcBounds,
              WORD             wBlkSize,
              LPTEXTINFOSTRUCT lptis)
{
  BOOL          bResult=FALSE;
  RECT          rcRect;
  GLOBALHANDLE  hdx=NULL;
  LPINT         lpdx=NULL;
  HANDLE        hBuffer=NULL;
  LPSTR         lpBuffer=NULL;
  HFONT         hFont=NULL;
  HFONT         hOldFont=NULL;

  /*-----------------------------------------*\
  | Get the test string for the test.         |
  \*-----------------------------------------*/
  if( (hBuffer  = LocalAlloc(LHND,(WORD)128)) &&
      (lpBuffer = (LPSTR)LocalLock(hBuffer)) )
  {
    LoadString(hInst,IDS_TEST_TEXT_STR1,lpBuffer,128);

    CopyRect(&rcRect, lprcBounds);
    ExtendRect(rcRect, 0, -2 * lptis -> iFontHeight);

    if( (hFont=GetFont(lptis -> iSynthesis, lptis -> iFont)) &&
        (hOldFont = SelectObject(hdcTarget,hFont)))
    {
      BOOL bCantAlloc=FALSE;

      /*-----------------------------------------------*\
      | Alloc/Lock buffer for integer arrays, if needed |
      \*-----------------------------------------------*/
      if    (lptis -> iSpaces)
      {
        int    nIdx;
        int    nLength;
        HANDLE hWidth=NULL;
        LPINT  lpWidth=NULL;

        if( (hdx = GlobalAlloc(GHND,(DWORD)lstrlen(lpBuffer)*sizeof(int))) &&
            (lpdx= (LPINT)GlobalLock(hdx))                                 &&
            (hWidth=GlobalAlloc(GHND,(DWORD)(256*sizeof(int))))            &&
            (lpWidth=(LPINT)GlobalLock(hWidth)) )
        {
          /*----------------------------------------------------------*\
          | Initialize test character spacing. -- insert iSpaces * ave |
          | width BETWEEN characters.  This means we need to add the   |
          | width of the current glyph to this deltaX.                 |
          \*----------------------------------------------------------*/
          GetCharWidth(hdcTarget,0,255,lpWidth);
          nLength=lstrlen(lpBuffer);

          for   (nIdx=0; nIdx < nLength; nIdx++)
            lpdx[nIdx] = lpWidth[lpBuffer[nIdx]] +
                         (lptis -> iFontWidth * lptis -> iSpaces);
        }
        else
          bCantAlloc=TRUE;

        // Free up memory used for widths
        if(lpWidth)
          GlobalUnlock(hWidth);
        if(hWidth)
          GlobalFree(hWidth);
      }
      else
        lpdx = NULL;

      if(!bCantAlloc)
      {
        TstExtTextOutRect(hdcTarget, 
                          lptis -> bOpaque ? ETO_OPAQUE : ETO_CLIPPED,
                          &rcRect, lpBuffer, lptis -> iLocation, lpdx);
        bResult=TRUE;
      }

      // Cleanup time--in reverse order from setup
      if(lpdx)
        GlobalUnlock(hdx);
      if(hdx)
        GlobalFree(hdx);
    }
    if(hOldFont)
      SelectObject(hdcTarget,hOldFont);
    if(hFont)
      DeleteObject(hFont);
  }
  if(lpBuffer)
    LocalUnlock(hBuffer);
  if(hBuffer)
    LocalFree(hBuffer);

  return bResult;
}

/*---------------------------------------------------------------------------*\
| PRINT EXTTEXTOUT TEXT                                                       |
|   This routine test the output of a text string within a clipping rectangle |
|   defined on a page.  It prints the text using both ETO_CLIPPED and         |
|   ETO_OPAQUED rectangles.  The text itself is printed at nine positions on  |
|   the rectangle to verify the proper clipping of the string.  An array of   |
|   character spacing is also used to ensure the clipping.                    |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter()    - misc.c    (PRNTFRAM)                                   |
|   ExtTextOutClip() - library.c (ITE_TEXT)                                   |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC               hDC          - Handle to a printer device context.      |
|   int               nCharSpacing - Char spacing between characters.         |
|   LPDEVCAPS         lpDevCaps    - Device capabilities structure.           |
|   LPENUMERATE       lpFonts      - Array of logical font structures.        |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HANDLE hInst - Module instance of library (PrntFram)                      |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL PrintExtTextOut(int iCharSpacing, int iSynthesis)
{
  TEXTINFOSTRUCT    tis;
  LOGFONT           lf;

  tis.iSpaces = iCharSpacing;
  tis.iSynthesis = iSynthesis;

  /*-----------------------------------------*\
  | For all fonts; perform the tests.         |
  \*-----------------------------------------*/
  for   (tis.iFont=0;
         tis.iFont < GetObjectCount(tlTest.gtTest.hFonts) && !bAbort;
         tis.iFont++)
  {
    {
      HFONT       hFont,hOldFont;
      TEXTMETRIC  tm;

      if (!(hFont = GetFont(tis.iSynthesis, tis.iFont)))
        return FALSE;

      GetObject(hFont,sizeof(LOGFONT),&lf);

      if (!(hOldFont = SelectObject(hdcTarget,hFont)))
      {
        DeleteObject(hFont);
        return FALSE;
      }

      GetTextMetrics(hdcTarget,&tm);
      tis.iFontHeight = tm.tmHeight+tm.tmExternalLeading;
      tis.iFontWidth = tm.tmAveCharWidth;
      DeleteObject(SelectObject(hdcTarget,hOldFont));
    }

    for (tis.bOpaque = 0; tis.bOpaque < 2 && !bAbort; tis.bOpaque++)
    {
      RECT      rcTest;
      HANDLE    hBuffer;
      LPSTR     lpBuffer;

      if (!(hBuffer = LocalAlloc(LHND,STRING_SIZE)))
        return(FALSE);

      if (!(lpBuffer = (LPSTR)LocalLock(hBuffer)))
      {
        LocalFree(hBuffer);
        return(FALSE);
      }

      wsprintf(lpBuffer, "%s Rectangle: Spacing - %d: Font ",
              (LPSTR) (tis.bOpaque ? "Opaque" : "Clipped"), iCharSpacing);

      GetFontName(&lf,&lpBuffer[lstrlen(lpBuffer)]);

      SetRect(&rcTest, 0, 0, StringWidth(lpBuffer), iHeightOfOneTextLine);

      if (!AddDrawObject(&rcTest, DoSomeText, TEXT_OBJECT, lstrlen(lpBuffer),
                         lpBuffer))
        return FALSE;

      LocalUnlock(hBuffer);
      LocalFree(hBuffer);

      /*------------------------------------*\
      | Output string at all 9 pts on rect.  |
      \*------------------------------------*/
      rcTest.top    = 2 * tis.iFontHeight + tis.iFontHeight /2;
      rcTest.left   = 0;
      rcTest.bottom = rcTest.top + 3 * tis.iFontHeight;
      rcTest.right  = iPageWidth/2;

      for (tis.iLocation=0; tis.iLocation < 9 && !bAbort; tis.iLocation++)
      {
        if (!rcTest.top)
          OffsetRect(&rcTest, 0, tis.iFontHeight + tis.iFontHeight / 2);

        // Center rectangle horizontally
        OffsetRect(&rcTest, iPageWidth/4, 0);

        if (!AddDrawObject(&rcTest, LetMyETO, GRAPHICS_OBJECT | TEXT_OBJECT,
                           sizeof(TEXTINFOSTRUCT), &tis))
          return FALSE;


        if (!AdjustObject(&rcTest, 0, 3 * tis.iFontHeight,
                          szDescription))
          return FALSE;

      }
      if (rcTest.top)
        if (!PrintPage(szDescription))
          return(FALSE);
    }
  }

  return !bAbort;
}

/*---------------------------------------------------------------------------*\
| PRINT TEXT TEST                                                             |
|   This routine performs the outputing of text tests to the printer.         |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintTestDescription() - (misc.c)                                         |
|   PrintFooter()          - (misc.c)                                         |
|                                                                             |
| PARAMETERS                                                                  |
|   -none-                                                                    |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL    PrintText(void)
{
  LoadString(hInst,IDS_FOOT_TEXT,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Print out exclaimation of test.           |
  \*-----------------------------------------*/
  PrintTestDescription(IDS_TST_DSCR_TEXT);
  if (!PrintPage(szDescription))
    return FALSE;

  /*-----------------------------------------*\
  | TEST SUITE ---> (append tests to list)    |
  \*-----------------------------------------*/
  PrintExtTextOut(0, 0);
  PrintExtTextOut(2, 0);
  PrintExtTextOut(0,SYNFONT_ITALIC);
  PrintExtTextOut(2,SYNFONT_UNDERLINED);
  PrintExtTextOut(4,SYNFONT_STRIKEOUT);

  return    TRUE;
}




/*----------------------------------------------------------------------------*\
|  GetCharWidths/ANSI conformity test                                          |
|                                                                              |
|  This test is very simple:  For each selected font, we:                      |
|  1) Determine what range of characters are valid in this font                |
|  2) Get the character width values for each of the fonts                     |
|  3) Print out the header to indicate what we're doing                        |
|  4) For each supported character:                                            |
|     a) Start from a known horizontal position on the page (6 in)             |
|     b) Subtract ten times the reported width of the character.               |
|     c) Use TextOut to send out the string, plus a ']'                        |
|     d) Move to the right half an inch and print the ANSI value               |
|                                                                              |
|                                                                              |
\*----------------------------------------------------------------------------*/
#define NUM_CHARS   10
#define MAX_COLUMNS 10

BOOL PrintFontName(LPRECT      lpRect,
                   WORD        wIgnoreMe,
                   LPWIDTHINFO lpwi)
{
  HFONT   hFont;
  HFONT   hOldFont;
  BYTE    szBuffer[128];
  LOGFONT lf;
  int     nWidth;
  int     nLength;

  // Get the Font
  if(!(hFont=GetFont(NULL,lpwi->iFont)))
    return FALSE;

  hOldFont=SelectObject(hdcTarget,hFont);

  GetObject(hFont,sizeof(LOGFONT),&lf);
  nLength=GetFontName(&lf,szBuffer);
  nWidth=(int)LOWORD(GetTextExtent(hdcTarget,szBuffer,nLength));

  TextOut(hdcTarget,(iPageWidth-nWidth)/2,lpRect->top,szBuffer,nLength);

  DeleteObject(SelectObject(hdcTarget,hOldFont));

  return TRUE;
}



BOOL WidthPrint (LPRECT      lpRect,
                 WORD        wIgnoreMe,
                 LPWIDTHINFO lpwi)
{
  char  szBuffer[NUM_CHARS+2];
  WORD  wFill;
  HFONT hFont;
  HFONT hOldFont;

  // Build the buffer to spit out, then do it!
  for(wFill=0;wFill<NUM_CHARS;wFill++)
    szBuffer[wFill]=lpwi->cTestChar;
  szBuffer[wFill++]=']';
  szBuffer[wFill++]='\0';

  // Get the desired font
  hFont=GetFont(NULL,lpwi->iFont);
  hOldFont=SelectObject(hdcTarget,hFont);

  TextOut(hdcTarget,lpRect->left,lpRect->top,szBuffer,lstrlen(szBuffer));

  if(lpwi->iANSIOffset)
  {
    // For the ANSI conformity, we need to do two things:
    //
    // 1) Print the numeric value of the font.
    // 2) Print the Glyph in an ANSI font (Arial, if possible)

    wsprintf(szBuffer,"%d",(WORD)(BYTE)lpwi->cTestChar);
    TextOut(hdcTarget,lpRect->right+lpwi->iNumericOffset,
            lpRect->top,szBuffer,lstrlen(szBuffer));

    SelectObject(hdcTarget,hANSIFont);

    TextOut(hdcTarget,lpRect->right+lpwi->iANSIOffset,
            lpRect->top,&lpwi->cTestChar,sizeof(char));
  }

  SelectObject(hdcTarget,hOldFont);
  DeleteObject(hFont);

  // If we've aborted, destroy hANSIFont if needed
  if(bAbort)
    if(hANSIFont != GetStockObject(ANSI_VAR_FONT))
    {
      DeleteObject(hANSIFont);
      hANSIFont=0;
      return FALSE;
    }

  return TRUE;
}



///////////////////////////////////////////////////////////////
//
//  Date Created:  11/17/91
//  Module:        TEXT
//
//  Description:   This is the code to determine the extent of
//                 a test string.
//
//  Calling Function:  PrintCharWidths
//
//  Referenced Functions:
//
//  Return Value:  TRUE if no error has occurred, otherwise FALSE
//
//  History:  11/17/91  pulled this code from PrintCharWidths to make
//                      it easier to place TT fonts
//
//////////////////////////////////////////////////////////////
BOOL PlaceCharWidth(int          nChar,
                    PRECT        rc,
                    PINT         iRight,
                    int          iColumns,
                    int          iHeight,
                    int          iANSIOffset,
                    int          iNumericOffset,
                    LPINT        lpWidths,
                    LPTEXTMETRIC lptm,
                    int          nFont)
{
  WIDTHINFO  wi;
  BYTE       cChar;
  static int iColumnNumber;
  int        nExtent;

  if(bAbort)
    return FALSE;

  cChar=(BYTE)nChar;
  wi.cTestChar=cChar;
  wi.iFont=nFont;
  wi.iANSIOffset=iANSIOffset;
  wi.iNumericOffset=iNumericOffset;

  if(!rc->top)
  {
    // Spit out the font name in the current font at top of each page
    // (Since we set the rectangle to the top of the page when we
    // start, this also initializes the first character to the first
    // column)
    if(!AddDrawObject(rc,PrintFontName, GRAPHICS_OBJECT | TEXT_OBJECT,
                      sizeof(WIDTHINFO), &wi))
      return FALSE;

    if(!AdjustObject(rc,0,iHeight,szDescription))
      return FALSE;

    iColumnNumber=0;
  }

  // Calculate the location on the page for the string.  The algorithm is:
  // 1) Start at the right edge of the current column
  // 2) Find the width of the string, which is
  //    NUM_CHARS * width of test char + width of ']'
  // 3) Starting location = (1) - (2)

  rc->right=iRight[iColumnNumber];

  nExtent= NUM_CHARS*lpWidths[cChar - lptm->tmFirstChar];
  nExtent+=lpWidths[']' - lptm->tmFirstChar];

  rc->left=rc->right-nExtent;

  // Now add this object to the list of objects to be drawn

  if(!AddDrawObject(rc,WidthPrint, GRAPHICS_OBJECT | TEXT_OBJECT,
                    sizeof(WIDTHINFO),&wi))
    return FALSE;

  if(++iColumnNumber == min(iColumns,MAX_COLUMNS))   // Max # columns reached
  {
    // Let AdjustObject take care of stuff like page breaks, etc.
    if(!AdjustObject(rc,NULL,iHeight,szDescription))
      return FALSE;

    iColumnNumber=0;
  }

  return TRUE;
}



BOOL PrintCharWidths (void)
{
  int         nNumFonts;
  int         nChar;
  int         nFont;
  HANDLE      hWidths;
  LPINT       lpWidths;
  HFONT       hFont;
  HFONT       hOldFont;
  RECT        rc;
  int         iLineHeight;
  BOOL        bContinue;
  DWORD       dwANSIFontInfo;
  int         iMaxANSIWidth;
  int         iRightEdge[MAX_COLUMNS];
  int         iColumnWidth;
  int         iColumns;
  int         iANSIOffset;
  int         iNumericOffset;
  int         iBetweenColumns;
  TEXTMETRIC  tm;
  LOGFONT     lf;

  // Do we do the test at all?

  nNumFonts=GetObjectCount(tlTest.gtTest.hFonts);
  if(!nNumFonts)
    return FALSE;

  LoadString(hInst,IDS_FOOT_CHARWIDTH,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Print out description of test.            |
  \*-----------------------------------------*/
  PrintTestDescription(IDS_TST_DSCR_WDTH);
  if    (!PrintPage(szDescription))
    return  FALSE;

  if(!(hWidths=LocalAlloc(LMEM_MOVEABLE|LMEM_ZEROINIT,256*sizeof(int))))
    return FALSE;

  lpWidths=(LPINT)LocalLock(hWidths);

  // Loop through all fonts, abort only the current font in case of error,
  // but exit the entire loop if someone aborts.

  for (nFont=0;(nFont < nNumFonts) && !bAbort;nFont++)
  {
    hFont=GetFont(NULL,nFont);

    if(!hFont)
      continue;

    hOldFont=SelectObject(hdcTarget,hFont);

    GetTextMetrics(hdcTarget,&tm);

    // Get required data for right justification.  (char widths)
    GetCharWidth(hdcTarget,tm.tmFirstChar,tm.tmLastChar,lpWidths);

    GetObject(hFont,sizeof(LOGFONT),&lf);

    // Restore the original state of the DC.
    DeleteObject(SelectObject(hdcTarget,hOldFont));

    dwANSIFontInfo=GetANSIFontHeightWidth(tm.tmHeight);

    iMaxANSIWidth=(int)LOWORD(dwANSIFontInfo);
    iLineHeight=max(tm.tmHeight+tm.tmExternalLeading,
                   (int)LOWORD(dwANSIFontInfo));

    rc.left=0;
    rc.top=0;
    rc.right=iPageWidth;
    rc.bottom=iLineHeight;

    // Determine how many columns we want to use.  Width of each column 
    // will be the maximum of:
    // 1) 1.5 inches
    // 2) NUM_CHARS+1 * Maximum character width, rounded up to next half inch

    iColumnWidth=max(iRight1Inch+iRight1HalfInch,
                    (NUM_CHARS+1)*tm.tmMaxCharWidth);

    // Determine the spacing between columns.  There are two parts to this.
    // The first is the column on ANSI glyphs, the second is the column
    // of numeric values.  The key issue here is that the glyphs are far
    // enough apart to prevent the ANSI glyph from clipping the test
    // glyph, especially in large, italic TT fonts.
    // ANSI white space:  The maximum of 1/4" and the width of the right
    //                    bracket in the test font.
    // Numeric space: The maximum of 1/4" and the widest ANSI glyph.

    iANSIOffset=max(lpWidths[']'-tm.tmFirstChar],iRight1QuarterInch);
    iNumericOffset=iANSIOffset + iMaxANSIWidth +  // Width of ANSI column
                   max(iMaxANSIWidth,iRight1QuarterInch);

    iBetweenColumns=iNumericOffset + 3*tm.tmMaxCharWidth + iRight1HalfInch;
                        
    // Now set up the columns, leaving spaces
    iColumns=min(MAX_COLUMNS,iPageWidth/(iColumnWidth+iBetweenColumns));

    if(iColumns)
    {
      int iLoop;

      iRightEdge[0]=iColumnWidth;

      for(iLoop=1;iLoop<iColumns;iLoop++)
        iRightEdge[iLoop]=iRightEdge[iLoop-1]+iColumnWidth+iBetweenColumns;
    }
    else   // Super-wide fonts:  Use entire page, no ANSI or Numeric data
    {
      iColumns=1;
      iRightEdge[0]=iPageWidth;
      iANSIOffset=0;           // Flag to indicate no ANSI/Numeric info
    }

    // Now, for each of the supported characters, we need to calculate
    // where to place the left edge of the string, so the right edge
    // is right justified.  Don't build the string here.

    // Don't use a BYTE for the loop counter, otherwise we're hosed
    // if tm.tmLastChar = 0xFF
    for(nChar=(int)tm.tmFirstChar,bContinue=TRUE;
        bContinue && nChar<=(int)tm.tmLastChar;
        nChar++)
      bContinue=PlaceCharWidth(nChar,&rc,iRightEdge,iColumns,iLineHeight,
                               iANSIOffset,iNumericOffset,
                               lpWidths,&tm,nFont);

    // Print out the page (unless we just did & are at top left corner)
    if(rc.top || rc.left)
      PrintPage(szDescription);

    // Tidy up the ANSI font
    if(hANSIFont != GetStockObject(ANSI_VAR_FONT))
      DeleteObject(hANSIFont);

    hANSIFont = 0;
  }

  // Tidy up and return success
  LocalUnlock(hWidths);
  LocalFree(hWidths);
  return TRUE;;
}
