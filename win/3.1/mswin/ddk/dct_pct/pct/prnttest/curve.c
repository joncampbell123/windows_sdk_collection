/*---------------------------------------------------------------------------*\
| CURVE TESTS                                                                 |
|   This module contains routines specific to testing the curve capabilities  |
|   of a printer driver.                                                      |
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"

typedef struct _CurveTest   CURVETESTINFO, FAR *LPCURVETESTINFO;

struct _CurveTest
  {
    int iPen;
    int iBrush;
    int iMode;
  };

/******************************************************************************

    Private Function:   CheckOutTheseCurves

    Purpose:            Drawing Object Function for the ellipse test

    Change History:

    10-29-1990  Some things are so {good | bad} they never change

******************************************************************************/
BOOL CheckOutTheseCurves(LPRECT          lprc,
                         WORD            wSize,
                         LPCURVETESTINFO lpcti)
{
  short      nObj;
  HPEN       hPen,hOldPen;
  HBRUSH     hBrush,hOldBrush;

  SetCurrentObject(tlTest.gtTest.hBrushes, lpcti -> iBrush);
  CopyDeviceObject((LPSTR)&nObj, tlTest.gtTest.hBrushes);
  SetCurrentObject(hBrushes, nObj);
  hBrush = CreateDeviceObject(hBrushes);

  SetCurrentObject(tlTest.gtTest.hPens, lpcti -> iPen);
  CopyDeviceObject((LPSTR)&nObj, tlTest.gtTest.hPens);
  SetCurrentObject(hPens, nObj);
  hPen = CreateDeviceObject(hPens);

  if    (!(hOldBrush = SelectObject(hdcTarget,hBrush)))
  {
    DeleteObject(hBrush);
    DeleteObject(hPen);
    return FALSE;
  }
  if    (!(hOldPen = SelectObject(hdcTarget,hPen)))
  {
    DeleteObject(SelectObject(hdcTarget,hOldBrush));
    DeleteObject(hPen);
    return(FALSE);
  }

  SetBkMode(hdcTarget,lpcti -> iMode);
  TstDrawObject(hdcTarget,lprc -> left, lprc -> top, RectWidth(*lprc),
        RectHeight(*lprc),OBJ_ELLIPSE);
  DeleteObject(SelectObject(hdcTarget,hOldBrush));
  DeleteObject(SelectObject(hdcTarget,hOldPen));

  return(TRUE);
}

/*---------------------------------------------------------------------------*\
| PRINT ELLIPSES TEST                                                         |
|   This routine prints ellipses to the device using the currently selected   |
|   pen and brush.                                                            |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   int  iMode     - Background Mode                                          |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC  hdcTarget - Target DC                                                |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if passed.                                                    |
\*---------------------------------------------------------------------------*/
BOOL PrintEllipses(int iMode)
{
  HANDLE        hBuffer;
  CURVETESTINFO cti;
  RECT          rc;

  /*-----------------------------------------*\
  | Set the x and y dimensions of the ellipse |
  | to be 1 inch by 1 inch.  This then looks  |
  | like a circle on the device.              |
  \*-----------------------------------------*/
  SetRect(&rc, 0, 0, iRight1Inch, iDown1Inch);

  cti.iMode = iMode;

  for (cti.iBrush=0;
       cti.iBrush < GetObjectCount(tlTest.gtTest.hBrushes);
       cti.iBrush++)
  {
    if ((hBuffer = LocalAlloc(LHND,80)))
    {
      LPSTR lpBuffer;

      if ((lpBuffer = LocalLock(hBuffer)))
      {
        RECT      rcText;
        wsprintf(lpBuffer,"%s Background - Brush %d",
                (LPSTR) ((iMode == OPAQUE) ? "Opaque" : "Transparent"),
                 cti.iBrush);

        SetRect(&rcText, 0, 0, iPageWidth, iHeightOfOneTextLine);
        OffsetRect(&rcText, rc.left, rc.top);
        if (!AddDrawObject(&rcText, DoSomeText, TEXT_OBJECT,
                           lstrlen(lpBuffer), lpBuffer))
        {
          LocalUnlock(hBuffer);
          LocalFree(hBuffer);
          return    FALSE;
        }

        LocalUnlock(hBuffer);
      }
      LocalFree(hBuffer);
    }

    if (!AdjustObject(&rc, 0, iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;

    for (cti.iPen=0;
         cti.iPen < GetObjectCount(tlTest.gtTest.hPens);
         cti.iPen++)
    {

      if (!AddDrawObject(&rc, CheckOutTheseCurves, GRAPHICS_OBJECT,
                         sizeof(CURVETESTINFO), &cti))
        return FALSE;

      ExtendRect(rc, 0, iHeightOfOneTextLineWithLeading);
      if (!AdjustObject(&rc, iRight5QuarterInches, iDown1Inch +
                        iHeightOfOneTextLineWithLeading, szDescription))
        return FALSE;

      ExtendRect(rc, 0, -iHeightOfOneTextLineWithLeading);
    }

    ExtendRect(rc, 0, iHeightOfOneTextLineWithLeading);
    if (!AdjustObject(&rc, 0, iDown3HalfInches +
                      iHeightOfOneTextLineWithLeading, szDescription))
      return FALSE;

    ExtendRect(rc, 0, -iHeightOfOneTextLineWithLeading);
  }

  if    (rc.top || rc.left)
    return PrintPage(szDescription);
  else
    return TRUE;
}

/*---------------------------------------------------------------------------*\
| PRINT CURVE TEST                                                            |
|   This routine peforms the curve printing test for the printer driver.      |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintEllipses                                                             |
|   PrintFooter()          - (misc.c)                                         |
|   PrintTestDescription() - (misc.c)                                         |
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
BOOL PrintCurves(void)
{
  LoadString(hInst,IDS_FOOT_CURVES,szDescription,STRING_SIZE);

  PrintTestDescription(IDS_TST_DSCR_CURV);
  if (!PrintPage(szDescription))
    return  FALSE;

  /*-----------------------------------------*\
  | TEST SUITE --> (append tests to list)     |
  \*-----------------------------------------*/
  PrintEllipses(OPAQUE);
  PrintEllipses(TRANSPARENT);

  return(TRUE);
}
