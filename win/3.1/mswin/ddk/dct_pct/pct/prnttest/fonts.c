/*---------------------------------------------------------------------------*\
| FONTS                                                                       |
|   This module contains routines to handle the gathering of the device fonts.|
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright     1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"

// Originally, all of the possible fonts were enumerated here.  It seems
// more practical to me to enumerate the selected fonts, since a single
// mouse click selects all of them.  To build with the original code,
// define ENUM_ALL_FONTS here.  To enumerate only the selected fonts,
// leave it out.                                             6/28/1991

static PSTR         pBuffer = NULL,
                    strSample = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVv"
                                "WwXxYyZz",
                    szError[] = { "Error: Creating Font",
                                  "Error: Selecting Font"};

static char         szFormatString[11], szFormatNumber[11],
                    szFormatNamedValue[16];

static PRINTCAPS    apcCharSets[] = {{ANSI_CHARSET, "Ansi"},
                                     {SHIFTJIS_CHARSET, "Kanji"},
                                     {OEM_CHARSET, "OEM"},
                                     {0, NULL}},

                    apcOutputPrecision[] = {{OUT_DEFAULT_PRECIS, "Default"},
                                            {OUT_STRING_PRECIS, "String"},
                                            {OUT_CHARACTER_PRECIS, "Character"},
                                            {OUT_STROKE_PRECIS, "Stroke"},
                                            {0, NULL}},

                    apcClipPrecision[] = {{CLIP_DEFAULT_PRECIS, "Default"},
                                          {CLIP_CHARACTER_PRECIS, "Character"},
                                          {CLIP_STROKE_PRECIS, "Stroke"},
                                          {0, NULL}},

                    apcQuality[] = {{DEFAULT_QUALITY, "Default"},
                                    {DRAFT_QUALITY, "Draft"},
                                    {PROOF_QUALITY, "Proof"},
                                    {0, NULL}},

                    apcPitch[]   = {{DEFAULT_PITCH, "Default"},
                                    {FIXED_PITCH, "Fixed Pitch"},
                                    {VARIABLE_PITCH, "Variable Pitch"},
                                    {0, NULL}},

                    apcTMPitch[]   = {{0, "Fixed Pitch"},
                                      {1, "Variable Pitch"},
                                      {0, NULL}},

                    apcFamily[]  = {{FF_DECORATIVE, "Decorative"},
                                    {FF_MODERN,"Modern"},
                                    {FF_ROMAN, "Roman"},
                                    {FF_SCRIPT, "Script"},
                                    {FF_SWISS, "Swiss"},
                                    {0, NULL}};

static RECT         rc;
static FONT         fFont;

/******************************************************************************

    Private Function:   FontText

    Purpose:            Drawing Object Function for printing a string in
                        a given font.

    Change History:

    10-27-1990  Roted it.

******************************************************************************/

BOOL FontText(LPRECT lprc,
              WORD   wLength,
              LPINT  lpiFont)
{
  HFONT     hFont,hOldFont;
  int       nObj;

#ifdef ENUM_ALL_FONTS
  nObj=*lpiFont++;
#else
  SetCurrentObject(tlTest.gtTest.hFonts, *lpiFont++);
  CopyDeviceObject((LPSTR)&nObj,tlTest.gtTest.hFonts);
#endif

  SetCurrentObject(hFonts, nObj);
  CopyDeviceObject((LPSTR)&fFont, hFonts);

  /*-----------------------------------------*\
  | Select the font.                          |
  \*-----------------------------------------*/
  if (!(hFont = CreateFontIndirect(&fFont.lf)))
    return  FALSE;

  if (!(hOldFont = SelectObject(hdcTarget,hFont)))
  {
    DeleteObject(hFont);
    return    FALSE;
  }

  TextOut(hdcTarget, lprc -> left, lprc -> top, (LPSTR) lpiFont,
          wLength - sizeof(int));

  /*-----------------------------------------*\
  | Restore for the next font.                |
  \*-----------------------------------------*/
  DeleteObject(SelectObject(hdcTarget,hOldFont));

  return(TRUE);
}

/******************************************************************************

    Private Function:   PrintFontSample

    Purpose:            Prints a sample of the given font

    Change History:

    10-29-1990  Better living through sleep

******************************************************************************/

BOOL PrintFontSample(LPRECT    lprc,
                     WORD      wStructSize,
                     LPLOGFONT lplf)
{
  HFONT hFont, hOldFont;

  hFont = CreateFontIndirect(lplf);
  if (!hFont)
    return  FALSE;

  hOldFont = SelectObject(hdcTarget, hFont);
  if    (!hOldFont)
    return  FALSE;

  TextOut(hdcTarget, lprc -> left, lprc -> top, (LPSTR) strSample,
        lstrlen(strSample));
  DeleteObject(SelectObject(hdcTarget, hOldFont));

  return  TRUE;
}

/******************************************************************************

    Private Function:   NameFromSet

    Purpose:            Returns a pointer to the name matching the given
                        value, or the default name if nothing matches.

    Change History:

    10-27-1990  I did it, and you can't change perfection

*******************************************************************************/

LPSTR  NameFromSet(PPRINTCAPS apcSet,
                   short      sValue)
{
  short sIndex;

  for (sIndex = 0; !apcSet[sIndex].szType; sIndex++)
    if (apcSet[sIndex].nIndex == sValue)
      return apcSet[sIndex].szType;

  return    "Unknown";
}

/******************************************************************************

    Private Function:   SkipALine

    Purpose:            Adjusts the rectangle, etc.

*******************************************************************************/

BOOL SkipALine()
{
  if (!AdjustObject(&rc, 0, iHeightOfOneTextLine, szDescription))
    return  FALSE;

  if (!rc.left)
    OffsetRect(&rc, 5 * iAverageCharWidth, 0);

  return TRUE;
}

/******************************************************************************

    Private Function:   GetRidOfIt

    Purpose:            Sends the given string off to the printer, and
                        adjusts the rectangle, etc.

*******************************************************************************/

BOOL GetRidOfIt()
{
  return AddDrawObject(&rc, DoSomeText, TEXT_OBJECT, lstrlen(pBuffer),
                       pBuffer) && SkipALine();

}

/******************************************************************************

    Private Function:   SetWidth

    Purpose:            Kludge to cover wsprintf not being as functional
                        as sprintf.

    In sprintf, one can use %*s to read the size of a string field from the
    input list.  Alas, wsprintf no can do.  This function uses wsprintf to
    generate format strings for later use, for the three string types
    generated by this module.

    Change History:

    10-29-1990  The Happy Hacker goes Brooklyn

******************************************************************************/

VOID SetWidth(WORD wWidth)
{
  if (wWidth > 99)
    wWidth = 99;

  wsprintf(szFormatNumber, "%%-%us - %%d", wWidth);
  wsprintf(szFormatString, "%%-%us - %%s", wWidth);
  wsprintf(szFormatNamedValue, "%%-%us - %%s (%%d)", wWidth);
}

/******************************************************************************

    Private Function:   PrintNumber

    Purpose:            Generates a string describing the given value, and
                        sends it off to the printer.

*******************************************************************************/

BOOL PrintNumber(LPSTR lpstrName,
                 short sPrintValue)
{
  wsprintf(pBuffer, szFormatNumber, lpstrName, sPrintValue);

  return GetRidOfIt();
}

/******************************************************************************

    Private Function:   DescribeFlag

    Purpose:            Generates a string describing the given flag, and
                        sends it off to the printer.

*******************************************************************************/

BOOL DescribeFlag(LPSTR lpstrName,
                  BOOL  bValue)
{
  wsprintf(pBuffer, szFormatString, lpstrName, (LPSTR) (bValue ? "Yes" : "No"));

  return GetRidOfIt();
}

/******************************************************************************

    Private Function:   DescribeValue

    Purpose:            Generates a string describing the given value, and
                        sends it off to the printer.

*******************************************************************************/

BOOL DescribeValue(LPSTR      lpstrName,
                   PPRINTCAPS apcSet,
                   short      sCheckValue,
                   short      sPrintValue)
{
  wsprintf(pBuffer, szFormatNamedValue, lpstrName,
           NameFromSet(apcSet, sCheckValue), sPrintValue);

  return GetRidOfIt();
}

/******************************************************************************

    Private Function:   PrintName

    Purpose:            Generates a string describing the given string, and
                        sends it off to the printer.

*******************************************************************************/

BOOL PrintName(LPSTR lpstrName,
               LPSTR lpstrValue)
{
  wsprintf(pBuffer, szFormatString, lpstrName, lpstrValue);

  return GetRidOfIt();
}

/******************************************************************************

    Private Function:   RequestFontSample

    Purpose:            Asks object manager to print a font sample

    Change History:

    10-29-1990  Still at it, eh?

******************************************************************************/

BOOL RequestFontSample(LPLOGFONT lplf)
{
  HFONT         hFont, hOldFont;
  TEXTMETRIC    tm;

  hFont = CreateFontIndirect(lplf);
  if (!hFont)
    return FALSE;

  hOldFont = SelectObject(hdcTarget, hFont);
  if (!hOldFont)
    return FALSE;

  GetTextMetrics(hdcTarget, &tm);
  DeleteObject(SelectObject(hdcTarget, hOldFont));

  SetRect(&rc, 0, rc.top,
          min(lstrlen(strSample) * tm.tmMaxCharWidth, iPageWidth),
              rc.top + tm.tmHeight);
  return  AddDrawObject(&rc, PrintFontSample, TEXT_OBJECT,
                        sizeof(LOGFONT), lplf)             &&
          AdjustObject(&rc, 0, 2 * (tm.tmHeight + tm.tmExternalLeading),
                       szDescription);
}

/*---------------------------------------------------------------------------*\
| PRINT DEVICE FONTS TO PRINTER (Expanded Version)                            |
|   This routine prints out the expanded information for every font the       |
|   device supports.                                                          |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC hdcTarget - Target DC                                                 |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything was OK.                                         |
|                                                                             |
|  06/28/1991  added bOK to prevent exiting without freeing buffer.           |
|              Only provide info on selected fonts, not all!                  |
\*---------------------------------------------------------------------------*/
BOOL    PrintDeviceFonts(void)
{
  TEXTMETRIC    tm;
  HANDLE        hBuffer;
  HFONT         hFont;
  HFONT         hOldFont;
  PINT          piFont;
  BOOL          bOK=TRUE;
  int           nFontsToTest;
  int           nObj;

  LoadString(hInst,(wHeaderSet & IDD_HEAD_CON)?IDS_FOOT_FONT_SAMPLES:
             IDS_FOOT_FONT_DETAIL,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | For all fonts, print out the expanded     |
  | information.                              |
  \*-----------------------------------------*/
  SetRect(&rc, 0, 0, iPageWidth, iHeightOfOneTextLine);

  /*-----------------------------------------*\
  | Must have a local buffer to store strings.|
  \*-----------------------------------------*/
  if (!(hBuffer = LocalAlloc(LHND,(WORD)128)))
    return  FALSE;
  if (!(pBuffer = LocalLock(hBuffer)))
  {
    LocalUnlock(hBuffer);
    LocalFree(hBuffer);
    return    FALSE;
  }
  piFont = (PINT) pBuffer;
  pBuffer = (PSTR) (piFont + 1);

#ifdef ENUM_ALL_FONTS
  nFontsToTest=GetObjectCount(hFonts);                  // # of fonts
#else
  nFontsToTest=GetObjectCount(tlTest.gtTest.hFonts);    // # of font indices
#endif

  for (*piFont=0; bOK && *piFont < nFontsToTest; (*piFont)++)
  {
#ifdef ENUM_ALL_FONTS
    nObj=*piFont;
#else
    SetCurrentObject(tlTest.gtTest.hFonts, *piFont);
    CopyDeviceObject((LPSTR)&nObj, tlTest.gtTest.hFonts);
#endif

    SetCurrentObject(hFonts, nObj);
    CopyDeviceObject((LPSTR)&fFont,hFonts);

    if (wHeaderSet & IDD_HEAD_CON)
    {
      /*-----------------------------------------*\
      | Select the font.                          |
      \*-----------------------------------------*/
      if (!(hFont = CreateFontIndirect(&fFont.lf)))
      {
        if(bOK)
          bOK &= AddDrawObject(&rc, DoSomeText, TEXT_OBJECT,
                               lstrlen(szError[0]), szError[0]);
        if (bOK)
          bOK &= AdjustObject(&rc, 0, iHeightOfOneTextLineWithLeading,
                              szDescription);
        continue;
      }
      if (!(hOldFont = SelectObject(hdcTarget,hFont)))
      {
        DeleteObject(hFont);

        if (bOK)
          bOK &= AddDrawObject(&rc, DoSomeText, TEXT_OBJECT,
                               lstrlen(szError[1]), szError[1]);
        if (bOK)
          bOK &= AdjustObject(&rc, 0, iHeightOfOneTextLineWithLeading,
                              szDescription);
        continue;
      }

      GetTextMetrics(hdcTarget,&tm);

      /*-----------------------------------------*\
      | Restore for the next font.                |
      \*-----------------------------------------*/
      DeleteObject(SelectObject(hdcTarget,hOldFont));

      /*-----------------------------------------------------------*\
      | Print the condensed information.  Print the size in POINTS  |
      \*-----------------------------------------------------------*/
      GetFontName(&fFont.lf,pBuffer);

      ExtendRect(rc, 0, tm.tmHeight - iHeightOfOneTextLine);

      if (bOK)
        bOK &= AddDrawObject(&rc, FontText, TEXT_OBJECT, sizeof(int) +
                             lstrlen(pBuffer), piFont);
      if (bOK)
        bOK &= AdjustObject(&rc, 0, tm.tmHeight + tm.tmExternalLeading,
                            szDescription);

      ExtendRect(rc, 0, iHeightOfOneTextLine - tm.tmHeight);
    }
    else
    {
      /*-----------------------------------------*\
      | Print out the Logical Font Information.   |
      \*-----------------------------------------*/
      SetRect(&rc, 0, 0, iPageWidth, iHeightOfOneTextLine);

      lstrcpy(pBuffer, "LOGICAL FONT STRUCTURE INFORMATION");

      if (bOK)
        bOK &= GetRidOfIt();

      if (bOK)
        bOK &= SkipALine();

      ExtendRect(rc, -5 * iAverageCharWidth, 0);
//      OffsetRect(&rc, 5 * iAverageCharWidth, 0);
      SetWidth(26);

      if (bOK)
        bOK &= PrintName("FaceName", fFont.lf.lfFaceName);

      if (bOK)
        bOK &= PrintNumber("Font Height (points)",
                           DEVICE_TO_POINTSIZE(fFont.lf.lfHeight));
      if (bOK)
        bOK &= PrintNumber("Font Height (device units)", fFont.lf.lfHeight);

      if (bOK)
        bOK &= PrintNumber("Font Width (device units)", fFont.lf.lfWidth);

      if (bOK)
        bOK &= PrintNumber("Font Escapement", fFont.lf.lfEscapement);

      if (bOK)
        bOK &= PrintNumber("Font Orientation", fFont.lf.lfOrientation);

      if (bOK)
        bOK &= PrintNumber("Font Weight", fFont.lf.lfWeight);

      if (bOK)
        bOK &= DescribeFlag("Italicized", fFont.lf.lfItalic);

      if (bOK)
        bOK &= DescribeFlag("Underlined", fFont.lf.lfUnderline);

      if (bOK)
        bOK &= DescribeFlag("StrikeOut", fFont.lf.lfStrikeOut);

      if (bOK)
        bOK &= DescribeValue("Character Set", apcCharSets,
                             fFont.lf.lfCharSet, fFont.lf.lfCharSet);

      if (bOK)
        bOK &= DescribeValue("Output Precision", apcOutputPrecision,
                             fFont.lf.lfOutPrecision,
                             fFont.lf.lfOutPrecision);

      if (bOK)
        bOK &= DescribeValue("Clipping Precision", apcClipPrecision,
                             fFont.lf.lfClipPrecision,
                             fFont.lf.lfClipPrecision);

      if (bOK)
        bOK &= DescribeValue("Output Quality", apcQuality,
                             fFont.lf.lfQuality & 0x0F,
                             fFont.lf.lfQuality);

      if (bOK)
        bOK &= DescribeValue("Pitch", apcPitch,
                             fFont.lf.lfPitchAndFamily & 0x03,
                             fFont.lf.lfPitchAndFamily & 0x0F);

      if (bOK)
        bOK &= DescribeValue("Font Family", apcFamily,
                             fFont.lf.lfPitchAndFamily & 0xF0,
                            (fFont.lf.lfPitchAndFamily >>4) & 0x0F);

      if (bOK)
        bOK &= SkipALine();

      wsprintf(pBuffer, "TEXT METRIC STRUCTURE INFORMATION (in device units)");

      if (bOK)
        bOK &= GetRidOfIt();

      if (bOK)
        bOK &= SkipALine();

      if (bOK)
        bOK &= PrintNumber("Character Height", fFont.tm.tmHeight);

      if (bOK)
        bOK &= PrintNumber("Character Ascent", fFont.tm.tmAscent);

      if (bOK)
        bOK &= PrintNumber("Character Descent", fFont.tm.tmDescent);

      if (bOK)
        bOK &= PrintNumber("Character Internal Leading",
                           fFont.tm.tmInternalLeading);
      if (bOK)
        bOK &= PrintNumber("Character External Leading",
                           fFont.tm.tmExternalLeading);
      if (bOK)
        bOK &= PrintNumber("Average Character Width",
                           fFont.tm.tmAveCharWidth);
      if (bOK)
        bOK &= PrintNumber("Maximum Character Width",
                           fFont.tm.tmMaxCharWidth);
      if (bOK)
        bOK &= PrintNumber("Character Weight", fFont.tm.tmWeight);

      if (bOK)
        bOK &= DescribeFlag("Italicized", fFont.tm.tmItalic);

      if (bOK)
        bOK &= DescribeFlag("Underlined", fFont.tm.tmUnderlined);

      if (bOK)
        bOK &= DescribeFlag("Struck Out", fFont.tm.tmStruckOut);

      if (bOK)
        bOK &= PrintNumber("First Character Value", fFont.tm.tmFirstChar);

      if (bOK)
        bOK &= PrintNumber("Last Character Value", fFont.tm.tmLastChar);

      if (bOK)
        bOK &= PrintNumber("Default Character Value",
                           fFont.tm.tmDefaultChar);
      if (bOK)
        bOK &= PrintNumber("Break Character", fFont.tm.tmBreakChar);

      if (bOK)
        bOK &= DescribeValue("Pitch", apcTMPitch,
                             fFont.tm.tmPitchAndFamily & 0x01,
                             fFont.tm.tmPitchAndFamily & 0x0F);
      if (bOK)
        bOK &= DescribeValue("Font Family", apcFamily,
                             fFont.tm.tmPitchAndFamily & 0xF0,
                             (fFont.tm.tmPitchAndFamily >>4) & 0x0F);
      if (bOK)
        bOK &= DescribeValue("Character Set", apcCharSets,
                             fFont.tm.tmCharSet, fFont.tm.tmCharSet);
      if (bOK)
        bOK &= PrintNumber("Overhang", fFont.tm.tmOverhang);

      if (bOK)
        bOK &= PrintNumber("Digitized Aspect X",
                           fFont.tm.tmDigitizedAspectX);
      if (bOK)
        bOK &= PrintNumber("Digitized Aspect Y",
                           fFont.tm.tmDigitizedAspectY);

      /*-----------------------------------------*\
      | Print out the Font Type information.      |
      \*-----------------------------------------*/
      if (bOK)
        bOK &= SkipALine();

      wsprintf(pBuffer,(LPSTR)"FONT TYPE INFORMATION");
      if (bOK)
        bOK &= GetRidOfIt();

      if (bOK)
        bOK &= SkipALine();

      if(fFont.nFontType & TRUETYPE_FONTTYPE)
        lstrcpy(pBuffer, "True Type Font");
      else
        wsprintf(pBuffer, "%s-%s",
                (LPSTR)((fFont.nFontType & RASTER_FONTTYPE) ?
                "Raster-" : "Vector-"),
                (LPSTR)((fFont.nFontType & DEVICE_FONTTYPE) ?
                "Device Based Font" : "GDI Based Font"));

      if (bOK)
        bOK &= GetRidOfIt();

      /*-----------------------------------------*\
      | Print out a sample of the font.           |
      \*-----------------------------------------*/
      if (bOK)
        bOK &= SkipALine();

      if (bOK)
        bOK &= RequestFontSample( &(fFont.lf));

      if ((dcDevCaps.wTextCaps & TC_UA_ABLE) &&
           !fFont.lf.lfUnderline)
      {
        fFont.lf.lfUnderline = 1;
        if (bOK)
          bOK &= RequestFontSample( &(fFont.lf));

        fFont.lf.lfUnderline = 0;
      }
      if ((dcDevCaps.wTextCaps & TC_IA_ABLE) &&
           !fFont.lf.lfItalic)
      {
        fFont.lf.lfItalic = 1;
        if (bOK)
          bOK &= RequestFontSample( &(fFont.lf));

        fFont.lf.lfItalic = 0;
      }
      if ((dcDevCaps.wTextCaps & TC_SO_ABLE) &&
           !fFont.lf.lfStrikeOut)
      {
        fFont.lf.lfStrikeOut = 1;
        if (bOK)
          bOK &= RequestFontSample( &(fFont.lf));

        fFont.lf.lfStrikeOut = 0;
      }

      if(rc.top && bOK)
      {
        PrintPage(szDescription);
        SetRect(&rc, 0, 0, iPageWidth, iHeightOfOneTextLine);
      }
    }
  }

  LocalUnlock(hBuffer);
  LocalFree(hBuffer);

  if (!bOK)
    return FALSE;

  if (rc.top)
    return  PrintPage(szDescription);
  else
    return TRUE;
}
