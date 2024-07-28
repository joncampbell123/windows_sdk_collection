/*---------------------------------------------------------------------------*\
| POLYGON TESTS                                                               |
|   This module contains routnes specific to testing the polygon capabilities |
|   of a printer driver.                                                      |
|                                                                             |
| DATE   : June 05, 1989                                                      |
|   Copyright   1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <windows.h>
#include    "PrntTest.h"

typedef struct _Star    STAR, FAR *LPSTAR;

struct _Star
  {
    int         iPen;
    int         iBrush;
    int         iBkMode;
    int         iFlMode;
  };

/*---------------------------------------------------------------------------*\
| PRINT POLYGON STAR                                                          |
|   This routine outputs an object created using the polygon function.  it    |
|   varies the pen and brush as well as the background modes for all          |
|   combinations.                                                             |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   LRECT   rcBounds   - Points to bounding rectangle                         |
|   WORD    wBlkSize   - Size of data block                                   |
|   LPSTAR  lpStarTest - Pointer to test data                                 |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC     hdcTarget  - Target DC (Printer or metafile)                      |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL AStarForYourEyes(LPRECT rcBounds,
                      WORD   wBlkSize,
                      LPSTAR lpstarTest)
{
  int        nObj;
  HPEN       hPen,hOldPen;
  HBRUSH     hBrush,hOldBrush;

  SetCurrentObject(tlTest.gtTest.hBrushes, lpstarTest -> iBrush);
  CopyDeviceObject((LPSTR) &nObj, tlTest.gtTest.hBrushes);
  SetCurrentObject(hBrushes, nObj);
  hBrush = CreateDeviceObject(hBrushes);

  SetCurrentObject(tlTest.gtTest.hPens, lpstarTest -> iPen);
  CopyDeviceObject((LPSTR) &nObj, tlTest.gtTest.hPens);
  SetCurrentObject(hPens, nObj);
  hPen = CreateDeviceObject(hPens);

  if(!(hOldBrush = SelectObject(hdcTarget,hBrush)))
  {
    DeleteObject(hBrush);
    DeleteObject(hPen);
    return(FALSE);
  }

  if(!(hOldPen = SelectObject(hdcTarget,hPen)))
  {
    DeleteObject(SelectObject(hdcTarget,hOldBrush));
    DeleteObject(hPen);
    return(FALSE);
  }

  SetBkMode(hdcTarget,lpstarTest -> iBkMode);
  SetPolyFillMode(hdcTarget,lpstarTest -> iFlMode);
  TstDrawObject(hdcTarget, rcBounds -> left, rcBounds -> top,
                RectWidth(*rcBounds), RectHeight(*rcBounds), OBJ_POLYGON);

  DeleteObject(SelectObject(hdcTarget,hOldBrush));
  DeleteObject(SelectObject(hdcTarget,hOldPen));

  return(TRUE);
}

/*---------------------------------------------------------------------------*\
| PRINT POLYGON STAR                                                          |
|   This routine outputs an object created using the polygon function.  it    |
|   varies the pen and brush as well as the background modes for all          |
|   combinations.                                                             |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   int               nBkMode   - Background mode.                            |
|   int  nFlMode - Fill Mode for polygon
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL PrintPolygonStar(int nFlMode,
                      int nBkMode)
{
  TEXTMETRIC    tm;
  HANDLE        hBuffer;
  LPSTR         lpBuffer;
  RECT          rcTest;
  STAR          starTest;

  LoadString(hInst,IDS_FOOT_POLYGONS,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Set the text height (for brush mode and   |
  | type description).  Also, adjust the page |
  | height to accomodate the footer.          |
  \*-----------------------------------------*/
  GetTextMetrics(hdcTarget,&tm);

  /*-----------------------------------------*\
  | Draw the ellipses varying the brushes and |
  | pens which is stored in the global arrays.|
  \*-----------------------------------------*/

  SetRect(&rcTest, 0, 0, iRight1Inch, iDown1Inch);
  starTest.iFlMode = nFlMode;
  starTest.iBkMode = nBkMode;

  for   (starTest.iBrush = 0;
         starTest.iBrush < GetObjectCount(tlTest.gtTest.hBrushes);
         starTest.iBrush++)
  {
    if    ((hBuffer = LocalAlloc(LHND,80)))
    {
      if    ((lpBuffer = LocalLock(hBuffer)))
      {
        RECT    rcText;

        wsprintf(lpBuffer,"%s Fill: %s - Brush %d",
                (LPSTR)((nFlMode == ALTERNATE) ? "Alternate" : "Winding"),
                (LPSTR)((nBkMode == OPAQUE) ? "Opaque" : "Transparent"),
                 starTest.iBrush);

        SetRect(&rcText, 0, 0, StringWidth(lpBuffer),
                iHeightOfOneTextLine);
        OffsetRect(&rcText, rcTest.left, rcTest.top);

        AddDrawObject(&rcText, DoSomeText, TEXT_OBJECT, lstrlen(lpBuffer),
                      lpBuffer);

        LocalUnlock(hBuffer);
      }
      LocalFree(hBuffer);
    }

    if (!AdjustObject(&rcTest, 0, iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;

    for (starTest.iPen=0;
         starTest.iPen < GetObjectCount(tlTest.gtTest.hPens);
         starTest.iPen++)
    {
      AddDrawObject(&rcTest, AStarForYourEyes, GRAPHICS_OBJECT, sizeof(STAR),
                    &starTest);

      if (!AdjustObject(&rcTest, iRight5QuarterInches,
                        iDown1Inch + iHeightOfOneTextLineWithLeading,
                        szDescription))
        return  FALSE;
    }

    if (!AdjustObject(&rcTest, 0, iDown3HalfInches, szDescription))
      return FALSE ;

  }

  if (rcTest.top || rcTest.left)
    return PrintPage(szDescription);
  else
    return TRUE;
}

/*---------------------------------------------------------------------------*\
| PRINT POLYGON TEST                                                          |
|   This routine peforms the polygon printing test for the printer driver.    |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintTestDescription() - (misc.c)                                         |
|   PrintPage()          - (misc.c)                                           |
|   PrintPolgonStar()                                                         |
|                                                                             |
| PARAMETERS                                                                  |
|   none                                                                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   none                                                                      |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if passed.                                                    |
\*---------------------------------------------------------------------------*/
BOOL PrintPolygons(void)
{
  LoadString(hInst,IDS_FOOT_POLYGONS,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Print the test description.               |
  \*-----------------------------------------*/
  PrintTestDescription(IDS_TST_DSCR_POLY);
  if(!PrintPage(szDescription))
    return FALSE;

  /*-----------------------------------------*\
  | TEST SUITE ---> (append tests to list)    |
  \*-----------------------------------------*/
  PrintPolygonStar(ALTERNATE, OPAQUE);
  PrintPolygonStar(ALTERNATE, TRANSPARENT);
  PrintPolygonStar(WINDING, OPAQUE);
  PrintPolygonStar(WINDING, TRANSPARENT);

  return(TRUE);
}
