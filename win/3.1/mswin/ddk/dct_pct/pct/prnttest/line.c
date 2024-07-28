/*---------------------------------------------------------------------------*\
| LINE TESTS                                                                  |
|   This module contains routnes specific to testing the line capabilities of |
|   a printer driver.                                                         |
|                                                                             |
| DATE   : June 05, 1989                                                      |
|   Copyright   1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <windows.h>
#include    "PrntTest.h"

typedef struct _TestInfo POLYLINETESTINFO, FAR *LPPOLYLINETESTINFO;

struct _TestInfo
  {
    int iPen;
    int iPenWidth;
    int iBkMode;
  };

/*---------------------------------------------------------------------------*\
| PRINT POLYLINES                                                             |
|   This routine outputs an object created using the polylines function.  it  |
|   varies the pen width and background modes for all combinations of pens    |
|   and background modes.                                                     |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   LPRECT             lprc      - Bounding box for test                      |
|   WORD               wBlkSize  - # of bytes in test info                    |
|   LPPOLYLINETESTINFO lpplti    - points to data for test                    |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC                hdcTarget - Target DC                                  |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL PolyDrivesMeCrackers(LPRECT                lprc,
                          WORD                  wBlksize,
                          LPPOLYLINETESTINFO    lpplti)
{
  int        nObj;
  LOGPEN     lpModPen;
  HPEN       hPen,hOldPen;

  SetCurrentObject(tlTest.gtTest.hPens,lpplti -> iPen);
  CopyDeviceObject((LPSTR)&nObj, tlTest.gtTest.hPens);
  SetCurrentObject(hPens, nObj);
  CopyDeviceObject((LPSTR)&lpModPen, hPens);

  lpModPen.lopnWidth.x = lpplti -> iPenWidth;
  lpModPen.lopnWidth.y = lpplti -> iPenWidth;
  if (!(hPen = CreatePenIndirect(&lpModPen)))
    return  FALSE;

  if    (!(hOldPen = SelectObject(hdcTarget,hPen)))
  {
    DeleteObject(hPen);
    return    FALSE;
  }

  SetBkMode(hdcTarget,lpplti -> iBkMode);
  TstDrawObject(hdcTarget, lprc -> left, lprc -> top, RectWidth(*lprc),
                RectHeight(*lprc), OBJ_POLYLINE);

  DeleteObject(SelectObject(hdcTarget,hOldPen));

  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| PRINT POLYLINES                                                             |
|   This routine outputs an object created using the polylines function.  it  |
|   varies the pen width and background modes for all combinations of pens    |
|   and background modes.                                                     |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   int iBkMode   - Background mode to use for test                           |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC hdcTarget - Target DC (printer or metafile)                           |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL PrintPolyLines(int iBkMode)
{
  HANDLE            hBuffer;
  LPSTR             lpBuffer;
  POLYLINETESTINFO  plti;
  RECT              rc;

  /*-----------------------------------------*\
  | Draw the ellipses varying the brushes and |
  | pens which is stored in the global arrays.|
  \*-----------------------------------------*/
  SetRect(&rc, 0, 0, iRight1Inch, iDown1Inch);
  plti.iBkMode = iBkMode;

  for   (plti.iPen=0;
         plti.iPen < GetObjectCount(tlTest.gtTest.hPens);
         plti.iPen++)
  {
    if    ((hBuffer = LocalAlloc(LHND,80)))
    {
      BOOL  bByeBye = FALSE;

      if    ((lpBuffer = LocalLock(hBuffer)))
      {
        RECT  rcText;

        wsprintf(lpBuffer,"%s Background: Pen %d",
                (LPSTR)((iBkMode == TRANSPARENT) ?
                 "Transparent" : "Opaque"), plti.iPen);

        SetRect(&rcText, rc.left, rc.top,
                rc.left + StringWidth(lpBuffer),
                rc.top + iHeightOfOneTextLine);

        bByeBye = !AddDrawObject(&rcText, DoSomeText, TEXT_OBJECT,
                                 lstrlen(lpBuffer), lpBuffer);

        LocalUnlock(hBuffer);
      }
      LocalFree(hBuffer);
      if (bByeBye)
        return  FALSE;
    }

    OffsetRect(&rc, 0, iHeightOfOneTextLineWithLeading);

    for (plti.iPenWidth=0; plti.iPenWidth < 8; plti.iPenWidth++)
    {

      if (!AddDrawObject(&rc, PolyDrivesMeCrackers, GRAPHICS_OBJECT,
                         sizeof(POLYLINETESTINFO), &plti))
        return  FALSE;

      ExtendRect(rc, 0, iHeightOfOneTextLineWithLeading);
      if (!AdjustObject(&rc, iRight5QuarterInches, iDown1Inch +
                        iHeightOfOneTextLineWithLeading, szDescription))
        return  FALSE;

      ExtendRect(rc, 0, -iHeightOfOneTextLineWithLeading);
    }

    ExtendRect(rc, 0, iHeightOfOneTextLineWithLeading);
    if (!AdjustObject(&rc, 0, iDown3HalfInches, szDescription))
      return  FALSE;

    ExtendRect(rc, 0, -iHeightOfOneTextLineWithLeading);
  }

  if (rc.top || rc.left)
    return PrintPage(szDescription);
  else
    return TRUE;

}

/*---------------------------------------------------------------------------*\
| PRINT LINE TEST                                                             |
|   This routine peforms the line printing test for the printer driver.       |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintTestDescription() - (misc.c)                                         |
|   PrintPolyLines()                                                          |
|                                                                             |
| PARAMETERS                                                                  |
|   none                                                                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if passed.                                                    |
\*---------------------------------------------------------------------------*/
BOOL PrintLines(void)
{
  LoadString(hInst,IDS_FOOT_LINES,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Print out exclaimation of test.           |
  \*-----------------------------------------*/
  PrintTestDescription(IDS_TST_DSCR_LINE);
  if (!PrintPage(szDescription))
    return  FALSE;

  /*-----------------------------------------*\
  | TEST SUITE ---> (append tests to list)    |
  \*-----------------------------------------*/
  PrintPolyLines(TRANSPARENT);
  PrintPolyLines(OPAQUE);

  return(TRUE);
}
