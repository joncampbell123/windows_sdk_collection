/*---------------------------------------------------------------------------*\
| DEVICE CAPABILITIES                                                         |
|   This module contains routines for outputing the device capabilities of    |
|   of the device.                                                            |
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"

static PSTR szTechnology[] = {"VECTOR PLOTTER"  ,"RASTER DISPLAY",
                              "RASTER PRINTER"  ,"RASTER CAMERA",
                              "CHARACTER STREAM","METAFILE",
                              "DISPLAY FILE"};

static PRINTCAPS pcClipCaps[] = {CP_RECTANGLE,"Can Clip to Rectangle"},


        pcRasterCaps[] = {RC_BITBLT, "Capable of transfering Bitmaps",
                          RC_BANDING, "Requires banding support",
                          RC_SCALING, "Capable of scaling",
                          RC_BITMAP64, "Can support bitmaps > 64K",
                          RC_GDI20_OUTPUT, "Has 2.0 output calls",
                          RC_DI_BITMAP, "Supports DIB to Memory",
                          RC_PALETTE, "Supports a Palette",
                          RC_DIBTODEV, "Supports DIBitsToDevice",
                          RC_BIGFONT, "Supports Fonts > 64K",
                          RC_STRETCHBLT, "Supports StrecthBlt",
                          RC_FLOODFILL, "Supports Flood Filling"},

        pcCurveCaps[] = {CC_NONE, "Curves Not Supported",
                         CC_CIRCLES, "Device can do Circles",
                         CC_PIE, "Device can do Pie-Wedges",
                         CC_CHORD, "Device can do Chord-Arcs",
                         CC_ELLIPSES, "Device can do Ellipses",
                         CC_WIDE, "Device can do Wide Lines",
                         CC_STYLED, "Device can do Styled Borders",
                         CC_WIDESTYLED, "Device can do Wide Borders",
                         CC_INTERIORS, "Device can do Interiors"},

        pcLineCaps[] = {LC_NONE, "Lines Not Supported",
                        LC_POLYLINE, "Device can do Poly Lines",
                        LC_MARKER, "Device can do Markers",
                        LC_POLYMARKER, "Device can do Poly Markers",
                        LC_WIDE, "Device can do Wide Lines",
                        LC_STYLED, "Device can do Styled Lines",
                        LC_WIDESTYLED, "Device can do Wide-Styled Lines",
                        LC_INTERIORS, "Device can do Interiors"},

        pcPolygonCaps[] = {PC_NONE, "Polygonals Not Supported",
                           PC_POLYGON, "Device can do Polygons",
                           PC_RECTANGLE, "Device can do Rectangles",
                           PC_WINDPOLYGON, "Device can do Winding Polygons",
                           PC_SCANLINE, "Device can do Wide ScanLines",
                           PC_WIDE, "Device can do Wide Lines",
                           PC_STYLED, "Device can do Styled Lines",
                           PC_WIDESTYLED, "Device can do Wide-Styled Lines",
                           PC_INTERIORS, "Device can do Interiors"},

        pcTextCaps[] = {TC_OP_CHARACTER, "Device can do Character Output Precision",
                        TC_OP_STROKE, "Device can do Stroke Output Precision",
                        TC_CP_STROKE, "Device can do Stroke Clip Precision",
                        TC_CR_90, "Device can do 90-degree Character Rotation",
                        TC_CR_ANY, "Device can do any Character Rotation",
                        TC_SF_X_YINDEP, "Device can do Scaling Independent of X and Y",
                        TC_SA_DOUBLE, "Device can do Doubled Character  or scaling",
                        TC_SA_INTEGER, "Device can do Integer Multiples for scaling",
                        TC_SA_CONTIN, "Device can do any Multiples for exact scaling",
                        TC_EA_DOUBLE, "Device can do Double-Weight Characters",
                        TC_IA_ABLE, "Device can do Italicizing",
                        TC_UA_ABLE, "Device can do Underlining",
                        TC_SO_ABLE, "Device can do Strike-Outs",
                        TC_RA_ABLE, "Device can do Raster Fonts",
                        TC_VA_ABLE, "Device can do Vector Fonts"};

typedef struct _Capability  CAPABILITY, *PCAPABILITY;

struct _Capability
  {
    PSTR        pstrName;
    WORD        wCapabilities;
    PRINTCAPS   *apcDetails;
  };

static CAPABILITY   capClipping = {"CLIPPING",
                                   sizeof(pcClipCaps) / sizeof(PRINTCAPS),
                                   pcClipCaps},

                    capRaster = {"RASTER",
                                 sizeof(pcRasterCaps) / sizeof(PRINTCAPS),
                                 pcRasterCaps},

                    capCurve = {"CURVE",
                                sizeof(pcCurveCaps) / sizeof(PRINTCAPS),
                                pcCurveCaps},

                    capLine = {"LINE", sizeof(pcLineCaps) / sizeof(PRINTCAPS),
                               pcLineCaps},

                    capPolygon = {"POLYGON",
                                 sizeof(pcPolygonCaps) / sizeof(PRINTCAPS),
                                 pcPolygonCaps},

                    capText = {"TEXT", sizeof(pcTextCaps) / sizeof(PRINTCAPS),
                               pcTextCaps};

static LPSTR    lpBuffer = NULL;
static RECT     rc;
static char     szFormatCapability[15];

/******************************************************************************

    Private Function:   SayItAndSprayIt

    Purpose:            Prints out given text strings for the first half
                        of the output.

    Change History:

    10-29-1990  As usual, I was wrong...

******************************************************************************/

BOOL SayItAndSprayIt(LPSTR   lpstrDescription,
                     char    cFormat,
                     LPVOID  lpValue)
{
  /*
    First, we create the appropriate format string for printing this gem
    of knowledge- the length is hard-coded, you'll notice.

    Then, we use the format string to generate the actual string we want
    to print.

  */

  wsprintf(szFormatCapability, "%%-23s - %%%c", cFormat);
  wsprintf(lpBuffer, szFormatCapability, lpstrDescription, (cFormat == 's')
           ? lpValue : (LPVOID) *((LPWORD) lpValue));

  return  AddDrawObject(&rc, DoSomeText, TEXT_OBJECT, lstrlen(lpBuffer),
                        lpBuffer) && AdjustObject(&rc, iPageWidth / 2,
                        iHeightOfOneTextLineWithLeading, szDescription);
}

/******************************************************************************

    Private Function:   SpillYourGuts

    Purpose:            Describes the bit flags encoded in the various
                        capabilities, using the structures defined above.

    Change History:

    10-29-1990  Coding away, on the thin ice of a nude day.

******************************************************************************/

BOOL SpillYourGuts(PCAPABILITY pcapRoadMap,
                   int iVictim)
{
  WORD  wLongest = 0, wIndex;

  /*
    First thing we do is check and see if the list of capabilities will fit
    on the rest of this page.
  */

  ExtendRect(rc, 0, (1 + pcapRoadMap -> wCapabilities) *
             iHeightOfOneTextLineWithLeading);

  if (!AdjustObject(&rc, 0, 0, szDescription))
    return FALSE;

  ExtendRect(rc, 0, -(1 + pcapRoadMap -> wCapabilities) *
             iHeightOfOneTextLineWithLeading);

  /*
    We'll begin by printing out the capability name.
  */

  wsprintf(lpBuffer, "%s CAPABILITIES", (LPSTR) pcapRoadMap -> pstrName);
  if (!AdjustObject(&rc, 0, iHeightOfOneTextLineWithLeading, szDescription) ||
      !AddDrawObject(&rc, DoSomeText, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer)                                              ||
      !AdjustObject(&rc, 0, iHeightOfOneTextLineWithLeading,szDescription))
    return  FALSE;

  /*
    Next, let's find out what the longest string is, so we can space them
    all out the same amount.
  */

  for (wIndex = pcapRoadMap -> wCapabilities; wIndex--;)
    wLongest = max(wLongest,
                  (WORD) lstrlen(pcapRoadMap -> apcDetails[wIndex].szType));

  /*
    Now, we encode that into a format string for wsprintf, using wsprintf.
    The format string is "%-nns - %s", where nn is our maximum length- this
    left justifies the strings, as we print them.
  */

  wsprintf(szFormatCapability, "%%-%us - %%s", wLongest);

  /*
    We change our rectangle, so it starts in 5 characters- indenting the
    remaining output.
  */

  ExtendRect(rc, -5 * iAverageCharWidth, 0);
  OffsetRect(&rc, 5 * iAverageCharWidth, 0);

  /*
    Now, we just go through and tell it like it is, following our roadmap.
    If the given bitfield is set, we say "Yes", otherwise, "No"
  */

  for (wIndex = 0; wIndex < pcapRoadMap -> wCapabilities; wIndex++)
  {
    if    (!rc.left)
      OffsetRect(&rc, 5 * iAverageCharWidth, 0);

    wsprintf(lpBuffer, szFormatCapability,
            (LPSTR) pcapRoadMap -> apcDetails[wIndex].szType,
            (LPSTR) ((pcapRoadMap -> apcDetails[wIndex].nIndex & iVictim) ?
            "Yes" : "No"));

    if    (!AddDrawObject(&rc, DoSomeText, TEXT_OBJECT, lstrlen(lpBuffer),
                          lpBuffer) ||
           !AdjustObject(&rc, 0, iHeightOfOneTextLineWithLeading,
                         szDescription))
      return FALSE;
  }

  /*
    Return the rectangle to 0, and we're finished.  Note that Adjust Object
    might have put it there, already.
  */

  ExtendRect(rc, 5 * iAverageCharWidth, 0);
  if    (rc.left)
    OffsetRect(&rc, -5 *iAverageCharWidth, 0);

  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| PRINT THE DEVICE CAPABILITIES                                               |
|   This routine prints the device capabilities to the printer.  It shows the |
|   values in which other applications can use in deriving the printer        |
|   characteristics.                                                          |
|                                                                             |
| CALLED ROUTINES                                                             |
|   EndOfPage()   - (misc.c)                                                  |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   none                                                                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC  hdcTarget - Target DC                                                |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL PrintDeviceCapabilities(void)
{
  HANDLE     hBuffer;
  BOOL       bContinue=TRUE;

  LoadString(hInst,IDS_FOOT_CAPS,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Must have a local buffer to store strings.|
  \*-----------------------------------------*/
  if    (!(hBuffer = LocalAlloc(LHND,(WORD)128)))
    return  FALSE;
  if    (!(lpBuffer = (LPSTR)LocalLock(hBuffer)))
  {
    LocalFree(hBuffer);
    return    FALSE;
  }

  /*-----------------------------------------*\
  | Output the device information.            |
  \*-----------------------------------------*/
  SetRect(&rc, 0, 0, iPageWidth / 2, iHeightOfOneTextLine);

  bContinue &= SayItAndSprayIt("Driver Version", 'X',
                               &dcDevCaps.nDriverVersion);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Driver Technology", 's',
                                 szTechnology[dcDevCaps.nTechnology]);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Horizontal Size (mm)", 'd',
                                 &dcDevCaps.nHorzSizeMM);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Number of Brushes", 'd',
                                 &dcDevCaps.nBrushes);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Vertical Size (mm)", 'd',
                                 &dcDevCaps.nVertSizeMM);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Number of Pens", 'd',
                                 &dcDevCaps.nPens);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Horizontal Resolution", 'd',
                                 &dcDevCaps.nHorzRes);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Number of Fonts", 'd',
                                 &dcDevCaps.nFonts);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Vertical Resolution", 'd',
                                 &dcDevCaps.nVertRes);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Number of Colors", 'd',
                                 &dcDevCaps.nColors);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Logical Pixels (x)", 'd',
                                 &dcDevCaps.nLogPixelsX);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Bits per Pixel", 'd',
                                 &dcDevCaps.nBitsPixel);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Logical Pixels (y)", 'd',
                                 &dcDevCaps.nLogPixelsY);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Number of Color Planes", 'd',
                                 &dcDevCaps.nPlanes);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Aspect ratio (width)", 'd',
                                 &dcDevCaps.nAspectX);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Size of Physical Device", 'd',
                                 &dcDevCaps.nPDeviceSize);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Aspect ratio (height)", 'd',
                                 &dcDevCaps.nAspectY);

  if (bContinue)
    bContinue &= SayItAndSprayIt("Diagonal ratio (width)", 'd',
                                 &dcDevCaps.nAspectXY);

  /*
    Done with the simple stuff- now, it's time to decipher the capabilities
    Before we do that, we'll have to adjust the rectangle a bit.
  */

  if  (!rc.left && bContinue)
    bContinue &= AdjustObject(&rc, 0, iHeightOfOneTextLineWithLeading,
                              szDescription);

  ExtendRect(rc, iPageWidth/2, 0);

  /*
    OK, driver- time to start spilling your guts- tell us everything, or
    the results could be MOST unpleasant (for ewe!)!!!
  */

  if (bContinue)
    bContinue &= SpillYourGuts(&capClipping, dcDevCaps.wClipCaps);

  if (bContinue)
    bContinue &= SpillYourGuts(&capRaster, dcDevCaps.wRasterCaps);

  if (bContinue)
    bContinue &= SpillYourGuts(&capCurve, dcDevCaps.wCurveCaps);

  if (bContinue)
    bContinue &= SpillYourGuts(&capLine, dcDevCaps.wLineCaps);

  if (bContinue)
    bContinue &= SpillYourGuts(&capPolygon, dcDevCaps.wPolygonCaps);

  if (bContinue)
    bContinue &= SpillYourGuts(&capText, dcDevCaps.wTextCaps);

  LocalUnlock(hBuffer);
  LocalFree(hBuffer);

  return  bContinue?PrintPage(szDescription):FALSE;
}
