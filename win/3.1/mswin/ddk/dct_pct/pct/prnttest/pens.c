/*---------------------------------------------------------------------------*\
| PENS                                                                        |
|   This module contains the routines necessary to handle the device pens     |
|   for the device.                                                           |
|                                                                             |
| DATE   : June 05, 1989                                                      |
|   Copyright   1989-1991 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <windows.h>
#include    "PrntTest.H"

#define MAXPENSTYLE (PS_INSIDEFRAME+1)

LPSTR lpPenStyle[] = {"PS_SOLID",
                      "PS_DASH",
                      "PS_DOT",
                      "PS_DASHDOT",
                      "PS_DASHDOTDOT",
                      "PS_NULL",
                      "PS_INSIDEFRAME",
                      "UNDEFINED"};

/******************************************************************************

    Private Function:   DrawOnePen

    Purpose:            Drawing Method for test of one pen

    Input:

        LPRECT  lprcWhere   -   gives rectangle to perform test in.

        WORD    wDataSize   -   size of data block pointed at by lppenTest

        LPPEN   lppenTest   -   points to PEN structure for pen to use

    Side Effects:

        Draws the pen test.  Changes current position in printer.

    Returns:

        TRUE if successful, FALSE if something failed.

    Change History:

    10-25-1990  Lifted from original code (now object-oriented)

******************************************************************************/

BOOL DrawOnePen(LPRECT lprcWhere, WORD wIgnore, LPWORD lpwPen)
{
  HPEN  hPen,hOldPen;

  /*
    STEP    1:  Get the pen we're going to test.
  */

  SetCurrentObject(hPens, *lpwPen);
  if    (!(hPen = CreateDeviceObject(hPens)))
    return(FALSE);

  if    (!(hOldPen = SelectObject(hdcTarget,hPen)))
  {
    DeleteObject(hPen);
    return(FALSE);
  }

  /*
    STEP    2:  Draw a rectangle bounding the test area.
  */

  Rectangle(hdcTarget, lprcWhere -> left, lprcWhere -> top,
        lprcWhere -> right, lprcWhere -> bottom);

  /*
    STEP    3:  Draw an "X" over the rectangle with the pen
  */

  MoveTo(hdcTarget, lprcWhere -> left, lprcWhere -> top);
  LineTo(hdcTarget, lprcWhere -> right, lprcWhere -> bottom);
  MoveTo(hdcTarget, lprcWhere -> right, lprcWhere -> top);
  LineTo(hdcTarget, lprcWhere -> left, lprcWhere -> bottom);

  /*
    STEP    4:  Leave the campground the way you found it, and go home!
  */

  DeleteObject(SelectObject(hdcTarget, hOldPen));

  return(TRUE);
}

/*---------------------------------------------------------------------------*\
| PRINT DEVICE PENS TO PRINTER                                                |
|   This routine prints out the device pen information.                       |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|   InitPage()    - (DrawObj.C)                                               |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT        lpPens    - Array of logical pen structures.            |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything was OK.                                         |
\*---------------------------------------------------------------------------*/
BOOL    PrintDevicePens(void)
{
  RECT          rcTest;
  WORD          wPen;

  LoadString(hInst,IDS_FOOT_PENS,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Set the x and y dimensions of the rect    |
  | to be 1 inch by 1 inch.  This then looks  |
  | like a square on the device.              |
  \*-----------------------------------------*/

  SetRect(&rcTest, 0, 0, iRight1Inch, iDown1Inch);

  /*-----------------------------------------*\
  | Draw the pen objects.                     |
  \*-----------------------------------------*/

  /*
    Add one test object for each pen to the page, until the page is full,
    or we are done.  Once a page is full, print it.  Note all we basically
    do at this point is calculate the bounding rectangles, increment the
    pen number, and send it off to the object manager.  The drawing occurs
    when the page is full.
  */

  for   (wPen=0; wPen < (WORD) GetObjectCount(hPens); wPen++)
    {
      if    (!AddDrawObject(&rcTest, DrawOnePen, GRAPHICS_OBJECT, sizeof(WORD), &wPen))
        return  FALSE;

      if    (!AdjustObject(&rcTest, iRight5QuarterInches, iDown1Inch +
              2 * (iHeightOfOneTextLineWithLeading), szDescription))
        return  FALSE;
    }

  /*
    Print any remaining incomplete page, then go home.
  */

  if    (rcTest.left || rcTest.top)
    return  PrintPage(szDescription);
  else
    return  TRUE;

}


///////////////////////////////////////////////////////////////
//
//  Date Created: 12/05/91
//  Module:       PENS
//
//  Description:  This routine prints out summary info for each
//                selected pen
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
BOOL SummarizePens(void)
{
  RECT     rc;
  int      iPen;
  BYTE     szBuffer[80];
  int      iLength;
  LOGPEN   lp;
  int      iIndexInFullList;

  LoadString(hInst,IDS_FOOT_PEN_INFO,szDescription,STRING_SIZE);

  SetRect(&rc,0,0,iPageWidth,iHeightOfOneTextLineWithLeading);

  /*-----------------------------------------*\
  | Get the pen info & report it              |
  \*-----------------------------------------*/
  for (iPen=0; iPen < GetObjectCount(tlTest.gtTest.hPens); iPen++)
  {
    // Get the info on this object
    SetCurrentObject(tlTest.gtTest.hPens,iPen);
    CopyDeviceObject((LPSTR)&iIndexInFullList,tlTest.gtTest.hPens);
    SetCurrentObject(hPens,iIndexInFullList);

    if(!CopyDeviceObject((LPSTR)&lp,hPens))
      return FALSE;

    // Pen Number
    iLength=wsprintf(szBuffer,"Pen Number: %u",iPen);
    if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
      return FALSE;
    if (!AdjustObject(&rc,iPageWidth,iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;

    // Pen Style
    iLength=wsprintf(szBuffer,"  Style: %s",
                     lpPenStyle[min(lp.lopnStyle,MAXPENSTYLE)]);
    if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
      return FALSE;
    if (!AdjustObject(&rc,iPageWidth,iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;

    // Pen Width
    iLength=wsprintf(szBuffer,"  Width: %u",lp.lopnWidth.x);
    if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
      return FALSE;
    if (!AdjustObject(&rc,iPageWidth,iHeightOfOneTextLineWithLeading,
                          szDescription))
      return FALSE;

    iLength=wsprintf(szBuffer,"  Color (RGB): %#02X%02X%02X",
                     GetRValue(lp.lopnColor),
                     GetGValue(lp.lopnColor),
                     GetBValue(lp.lopnColor));
    if (!AddDrawObject(&rc,DoSomeText,TEXT_OBJECT,iLength,szBuffer))
      return FALSE;
    if (!AdjustObject(&rc,iPageWidth,2*iHeightOfOneTextLineWithLeading,
                      szDescription))
      return FALSE;
  }

  if (rc.top || rc.left)
    return PrintPage(szDescription);
  else
    return TRUE;
}



///////////////////////////////////////////////////////////////
//
//  Date Created: 12/05/91
//  Module:       PENS
//
//  Description:  This routine decides whether pen info is needed
//                or not, then calls SummarizePens() if needed.
//                Note that we violate the model by making the decision
//                here instead of in command, but that's life!  If
//                we decide to fix this later, SummarizePens() can
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
BOOL PrintPenSummary(void)
{
  if(wTestsSet  & (IDD_TEST_POLYGONS | IDD_TEST_CURVES  | IDD_TEST_LINES))
    return SummarizePens();
  else
    return TRUE;
}

