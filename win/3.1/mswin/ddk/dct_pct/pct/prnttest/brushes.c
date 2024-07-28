/*---------------------------------------------------------------------------*\
| BRUSHES                                                                     U
|   This module contains the routines necessary to handle the GDI brush       |
|   objects for the device.                                                   |
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"

#define MAXBRUSHSTYLE (BS_DIBPATTERN+1)
#define MAXHATCHSTYLE (HS_DIAGCROSS+1)

LPSTR lpBrushStyle[] = {"BS_SOLID",
                        "BS_NULL",
                        "BS_HATCHED",
                        "BS_PATTERN",
                        "BS_INDEXED",
                        "BS_DIBPATTERN",
                        "UNDEFINED"};

LPSTR lpHatchStyle[] = {"HS_HORIZONTAL",
                        "HS_VERTICAL",
                        "HS_FDIAGONAL",
                        "HS_BDIAGONAL",
                        "HS_CROSS",
                        "HS_DIAGCROSS",
                        "UNDEFINED"};


/******************************************************************************

    Private Function:   WreckedAngle

    Purpose:            Draws a rectangle with the given brush

    Change History:

    10-29-1990  I'd sure like to (wouldn't you?)

******************************************************************************/

BOOL WreckedAngle(LPRECT lprc, WORD wSoWhat, LPINT lpiBrush)
{
  HBRUSH     hBrush,hOldBrush;

  SetCurrentObject(hBrushes, *lpiBrush);

  if (!(hBrush = CreateDeviceObject(hBrushes)))
    return  FALSE;

  if (!(hOldBrush = SelectObject(hdcTarget, hBrush)))
  {
    DeleteObject(hBrush);
    return    FALSE;
  }

  Rectangle(hdcTarget, lprc->left, lprc->top, lprc->right, lprc->bottom);
  DeleteObject(SelectObject(hdcTarget, hOldBrush));

  return(TRUE);
}

/*---------------------------------------------------------------------------*\
| PRINT DEVICE BRUSHES TO PRINTER                                             |
|   This routine draws a sample of every brush enumerated by the printer      |
|                                                                             |
| PARAMETERS                                                                  |
|   none                                                                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything was OK.                                         |
\*---------------------------------------------------------------------------*/

BOOL PrintDeviceBrushes(void)
{
  RECT  rc;
  int   iBrush;

  LoadString(hInst,IDS_FOOT_BRUSHES,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Set the x and y dimensions of the rect    |
  | to be 1 inch by 1 inch.  This then looks  |
  | like a square on the device.              |
  \*-----------------------------------------*/
  SetRect(&rc, 0, 0, iRight1Inch, iDown1Inch);
  OffsetRect(&rc, 0, iHeightOfOneTextLineWithLeading);

  /*-----------------------------------------*\
  | Draw the brushes.                         |
  \*-----------------------------------------*/
  for   (iBrush=0; iBrush < GetObjectCount(hBrushes); iBrush++)
  {

    if (!AddDrawObject(&rc, WreckedAngle, GRAPHICS_OBJECT, sizeof(int),
                  &iBrush))
      return  FALSE;

    ExtendRect(rc, 0, iHeightOfOneTextLineWithLeading);
    if (!AdjustObject(&rc, iRight5QuarterInches, iDown1Inch +
                  iHeightOfOneTextLineWithLeading, szDescription))
      return FALSE;

    ExtendRect(rc, 0, -iHeightOfOneTextLineWithLeading);
  }

  if (rc.top || rc.left)
    return  PrintPage(szDescription);
  else
    return  TRUE;
}



///////////////////////////////////////////////////////////////
//
//  Date Created: 12/05/91
//  Module:       BRUSHES
//
//  Description:  This routine prints out summary info for each
//                selected brush
//
//  Calling Function:
//
//  Referenced Functions:
//
//  Return Value: BOOL if everything worked OK
//
//  History:
//
//////////////////////////////////////////////////////////////
BOOL SummarizeBrushes(void)
{
  RECT     rc;
  int      iBrush;
  BYTE     szBuffer[80];
  int      iLength;
  LOGBRUSH lb;
  int      iIndexInFullList;


  LoadString(hInst,IDS_FOOT_BRUSH_INFO,szDescription,STRING_SIZE);

  SetRect(&rc,0,0,iPageWidth,iHeightOfOneTextLineWithLeading);

  /*-----------------------------------------*\
  | Get the brush info & report it            |
  \*-----------------------------------------*/
  for (iBrush=0; iBrush < GetObjectCount(tlTest.gtTest.hBrushes); iBrush++)
  {
    // Get the info on this object
    SetCurrentObject(tlTest.gtTest.hBrushes,iBrush);
    CopyDeviceObject((LPSTR)&iIndexInFullList,tlTest.gtTest.hBrushes);
    SetCurrentObject(hBrushes,iIndexInFullList);

    if(!CopyDeviceObject((LPSTR)&lb,hBrushes))
      return FALSE;

    // Number of Brush
    iLength=wsprintf(szBuffer,"Brush Number: %u",iBrush);
    if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
      return FALSE;
    if (!AdjustObject(&rc,iPageWidth,iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;

    // Style of Brush
    iLength=wsprintf(szBuffer,"  Style      : %s",
                    lpBrushStyle[min((WORD)lb.lbStyle,MAXBRUSHSTYLE)]);
    if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
      return FALSE;
    if (!AdjustObject(&rc,iPageWidth,iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;

    // Hatch pattern of brush, if style is BS_HATCHED
    if(BS_HATCHED == lb.lbStyle)
    {
      iLength=wsprintf(szBuffer,"  Hatch      : %s",
                      lpHatchStyle[min((WORD)lb.lbHatch,MAXHATCHSTYLE)]);
      if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
        return FALSE;
      if (!AdjustObject(&rc,iPageWidth,iHeightOfOneTextLineWithLeading,
                        szDescription))
        return FALSE;
    }

    // Color of Brush--won't always apply, depending on style
    iLength=wsprintf(szBuffer,"  Color (RGB): %#02X%02X%02X",
                     GetRValue(lb.lbColor),
                     GetGValue(lb.lbColor),
                     GetBValue(lb.lbColor));
    if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
      return FALSE;
    if (!AdjustObject(&rc,iPageWidth,2*iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;
  }

  if    (rc.top || rc.left)
    return  PrintPage(szDescription);
  else
    return  TRUE;
}



///////////////////////////////////////////////////////////////
//
//  Date Created: 12/05/91
//  Module:       BRUSHES
//
//  Description:  This routine decides whether brush info is needed
//                or not, then calls SummarizeBrushes() if needed.
//                Note that we violate the model by making the decision
//                here instead of in command, but that's life!  If
//                we decide to fix this later, SummarizeBrushes() can
//                be called directly from TestOnePrinter()
//
//  Calling Function:
//
//  Referenced Functions:
//
//  Return Value: BOOL if everything worked OK
//
//  History:
//
//////////////////////////////////////////////////////////////
BOOL PrintBrushSummary(void)
{
  if(wTestsSet  & (IDD_TEST_BITMAPS | IDD_TEST_POLYGONS |
                   IDD_TEST_CURVES  | IDD_TEST_LINES))
    return SummarizeBrushes();
  else
    return TRUE;
}


