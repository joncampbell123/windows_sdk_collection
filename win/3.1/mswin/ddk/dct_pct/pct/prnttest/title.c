/*---------------------------------------------------------------------------*\
| PRINT TITLE PAGE INFORMATION (header)                                       |
|   This module contains the routines necessary to print out the header       |
|   information.                                                              |
|                                                                             |
| DATE   : June 05, 1989                                                      |
|   Copyright   1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"

/******************************************************************************

    Private Function:   CenterThisBaby

    Purpose:            Prints a line of text, centered in the given
                        rectangle

    Change History:

    10-26-1990  I did it, I did it, I dided it!

******************************************************************************/

BOOL CenterThisBaby(LPRECT lprcWhere,
                    WORD   wLength,
                    LPVOID lpBuffer)
{

  WORD  wOldTextAlign;
  BOOL  bYouTellEm;

  wOldTextAlign = GetTextAlign(hdcTarget);
  SetTextAlign(hdcTarget,TA_CENTER | TA_NOUPDATECP);

  bYouTellEm = TextOut(hdcTarget,
                       lprcWhere -> left + RectWidth(*lprcWhere)/2,
                       lprcWhere -> top, lpBuffer, wLength);

  SetTextAlign(hdcTarget,wOldTextAlign);
  return    bYouTellEm;
}

/******************************************************************************

    Private Function:   LineDrive

    Purpose:            Prints a line of text, centered in the given
                        rectangle (horizontally) with text set in the
                        baseline.

    Change History:

    10-26-1990  I did it, I dun it, I duded it!

******************************************************************************/

BOOL LineDrive(LPRECT lprcWhere,
               WORD   wLength,
               LPVOID lpBuffer)
{

  WORD  wOldTextAlign;
  BOOL  bYouTellEmBabe;

  wOldTextAlign = GetTextAlign(hdcTarget);
  SetTextAlign(hdcTarget, TA_BASELINE | TA_CENTER | TA_NOUPDATECP);

  bYouTellEmBabe = TextOut(hdcTarget,
                           lprcWhere -> left + RectWidth(*lprcWhere)/2,
                           lprcWhere -> top + iTextAscent, lpBuffer, wLength);

  SetTextAlign(hdcTarget, wOldTextAlign);
  return    bYouTellEmBabe;
}

/*---------------------------------------------------------------------------*\
| PRINT TITLE PAGE                                                            |
|   This routine prints out the title page information as the first page of   |
|   the header.                                                               |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (Misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
| - None -                                                                    |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   BOOL bPrintAbort - Printer abort flag.                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything went smoothly.                                  |
\*---------------------------------------------------------------------------*/
BOOL    PrintTitlePage(void)
{
  HANDLE        hBuffer;
  LPSTR         lpBuffer;
  DATETIME      dtDateTime;
  RECT          rcText;

  LoadString(hInst,IDS_FOOT_TITLE,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Must have a local buffer to store strings.|
  \*-----------------------------------------*/
  if (!(hBuffer = LocalAlloc(LHND,128)))
    return(FALSE);

  if (!(lpBuffer = (LPSTR)LocalLock(hBuffer)))
  {
    LocalFree(hBuffer);
    return(FALSE);
  }

  /*-----------------------------------------*\
  | Output Title page information.            |
  \*-----------------------------------------*/

  SetRect(&rcText, 0, 0, iPageWidth, iHeightOfOneTextLine);
  LoadString(hInst,IDS_HEAD_TITLEPAGE,lpBuffer,128);

  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;

  GetSystemDateTime(&dtDateTime);
  wsprintf(lpBuffer, "%02d/%02d/%d", dtDateTime.bMonth, dtDateTime.bDay,
           dtDateTime.wYear);

  OffsetRect(&rcText, 0, iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;

  wsprintf(lpBuffer, "%02d:%02d:%02d", dtDateTime.bHours, dtDateTime.bMinutes,
           dtDateTime.bSeconds);

  OffsetRect(&rcText, 0, iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;


  lstrcpy(lpBuffer,"Printer Name   - ");
  lstrcat(lpBuffer,pPrinter.szName);

  OffsetRect(&rcText, 0, 10 * iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;


  lstrcpy(lpBuffer,"Printer Driver - ");
  lstrcat(lpBuffer,pPrinter.szDriver);

  OffsetRect(&rcText, 0, iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;


  lstrcpy(lpBuffer,"Printer Port   - ");
  lstrcat(lpBuffer,pPrinter.szPort);

  OffsetRect(&rcText, 0, iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;


  lstrcpy(lpBuffer,"Test Profile   - ");
  lstrcat(lpBuffer,pPrinter.szProfile);

  OffsetRect(&rcText, 0, iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;


  lstrcpy(lpBuffer,"Window Version - ");
  lstrcat(lpBuffer,pPrinter.szSystemVer);

  OffsetRect(&rcText, 0, iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                     lpBuffer))
    return FALSE;


  lstrcpy(lpBuffer,"Driver Version - ");
  lstrcat(lpBuffer,pPrinter.szDriverVer);

  OffsetRect(&rcText, 0, iHeightOfOneTextLineWithLeading);
  if (!AddDrawObject(&rcText, CenterThisBaby, TEXT_OBJECT, lstrlen(lpBuffer),
                    lpBuffer))
    return FALSE;

  LocalUnlock(hBuffer);
  LocalFree(hBuffer);

  /*-----------------------------------------*\
  | Print footer at bottom of page.           |
  \*-----------------------------------------*/
  return PrintPage(szDescription);

}

/******************************************************************************

    Private Function:   MakeABigX

    Purpose:            Draws the given rectangle, and pens an X in it

    Change History:

    10-26-1990  Lifted from previous code
    08-26-1991  Changed to provide more info on same page.

******************************************************************************/

BOOL MakeABigX(LPRECT lprcWhere,
               WORD wIgnoreMe,
               LPVOID lpIgnoreMeToo)
{
  // Make a box, but intentionally extend the inches beyond the edges.
  // We should get clipped, either by Windows, the driver, or the device
  // itself.  In any case, if the lines extend beyond the intersection,
  // there's a problem.
  // Don't forget to subtract 1 when drawing lines on right and bottom
  // edges!

  MoveTo(hdcTarget, lprcWhere->left-iRight1Inch,lprcWhere->top);
  LineTo(hdcTarget, lprcWhere->right+iRight1Inch,lprcWhere->top);
  MoveTo(hdcTarget, lprcWhere->right-1,lprcWhere->top-iDown1Inch);
  LineTo(hdcTarget, lprcWhere->right-1,lprcWhere->bottom+iDown1Inch);
  MoveTo(hdcTarget, lprcWhere->right+iRight1Inch,lprcWhere->bottom-1);
  LineTo(hdcTarget, lprcWhere->left-iRight1Inch,lprcWhere->bottom-1);
  MoveTo(hdcTarget, lprcWhere->left,lprcWhere->bottom+iDown1Inch);
  LineTo(hdcTarget, lprcWhere->left,lprcWhere->top-iDown1Inch);

  // Now make a big X through the center -- set the background mode
  MoveTo(hdcTarget, lprcWhere -> left, lprcWhere -> top);
  LineTo(hdcTarget, lprcWhere -> right, lprcWhere -> bottom);
  MoveTo(hdcTarget, lprcWhere -> right, lprcWhere -> top);
  LineTo(hdcTarget, lprcWhere -> left, lprcWhere -> bottom);
  return TRUE;
}



/***************************************************************************

    Private Function:   BoxTheHardWay

    Purpose:            Draws a box with 4 lines so center is undisturbed.

    Change History:
    08/26/1991 Isn't that special!

****************************************************************************/
BOOL BoxTheHardWay (LPRECT lprcWhere,
                    WORD   wIgnoreMe,
                    LPVOID lpIgnoreMeToo)
{
  MoveTo(hdcTarget,lprcWhere->left,lprcWhere->top);
  LineTo(hdcTarget,lprcWhere->left,lprcWhere->bottom-1);
  LineTo(hdcTarget,lprcWhere->right-1,lprcWhere->bottom-1);
  LineTo(hdcTarget,lprcWhere->right-1,lprcWhere->top);
  LineTo(hdcTarget,lprcWhere->left,lprcWhere->top);

  return TRUE;
}



/******************************************************************************

    Private Function:   WreckThisAngle

    Purpose:            Draws the given rectangle

    Change History:

    10-26-1990  I hate to admit it, though
    08-26-1991  Added wFlags to set bkmode to transparent if
                non-zero.
******************************************************************************/

BOOL WreckThisAngle(LPRECT lprcWhere,
                    WORD   wIgnoreMe,
                    LPVOID lpIgnoreMeToo)
{
  Rectangle(hdcTarget, lprcWhere -> left, lprcWhere -> top,
            lprcWhere -> right, lprcWhere -> bottom);

  return    TRUE;
}

/*---------------------------------------------------------------------------*\
| PRINT PRINTABLE AREA OF DEVICE (header)                                     |
|   This routine outputs a rectangle to the printer indicating the device     |
|   boundries.                                                                |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (Misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC               hDC       - Handle to device context.                   |
|   LPDEVCAPS         lpDevCaps - Device capabilities structure.              |
|   LPSTR             bAbort    - L-Pointer to printer abort flag.            |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   BOOL bPrintAbort - Printer abort flag.                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything went smoothly.                                  |
\*---------------------------------------------------------------------------*/
BOOL    PrintPrintableArea(void)
{
  HANDLE        hBuffer;
  LPSTR         lpBuffer;
  RECT          rc;
  POINT         pPhysPageSize;
  POINT         pOffset;
  WORD          wFlag=TRANSPARENT;

  /*-----------------------------------------*\
  | Must have a local buffer to store strings.|
  \*-----------------------------------------*/
  if (!(hBuffer = LocalAlloc(LHND,128)))
    return  FALSE;

  if (!(lpBuffer = (LPSTR)LocalLock(hBuffer)))
  {
    LocalFree(hBuffer);
    return    FALSE;
  }

  /*-----------------------------------------*\
  | Output the border-rect to the device.     |
  \*-----------------------------------------*/

  SetRect(&rc, 0, 0, iPageWidth, iPageHeight);

  if (!AddDrawObject(&rc, MakeABigX, GRAPHICS_OBJECT, 0, NULL))
  {
    LocalUnlock(hBuffer);
    LocalFree(hBuffer);
    return  FALSE;
  }

  /*--------------------------------------------------*\
  | Add a rectangle 1 inch from physical page borders. |
  \*--------------------------------------------------*/
  if (-1 != Escape (hdcPrinter,GETPHYSPAGESIZE,NULL,NULL,(LPSTR)&pPhysPageSize))
  {
    if(-1 != Escape (hdcPrinter,GETPRINTINGOFFSET,NULL,NULL,(LPSTR)&pOffset))
    {
      // Calculate a rectangle 1 inch from physical page sides
      SetRect(&rc,
              iRight1Inch-pOffset.x,
              iDown1Inch-pOffset.y,
              pPhysPageSize.x-pOffset.x-iRight1Inch,
              pPhysPageSize.y-pOffset.y-iDown1Inch);

      if(!AddDrawObject(&rc, BoxTheHardWay,GRAPHICS_OBJECT,NULL,NULL))
      {
        LocalUnlock(hBuffer);
        LocalFree(hBuffer);
        return  FALSE;
      }
    }
  }


  /*-----------------------------------------*\
  | Display Centered text comment.            |
  \*-----------------------------------------*/
  LoadString(hInst,IDS_HEAD_PRINTAREA,lpBuffer,128);

  SetRect(&rc, 0, iPageHeight / 2 - iTextAscent, iPageWidth,
        iPageHeight / 2 + iTextDescent);

  if (!AddDrawObject(&rc,LineDrive,TEXT_OBJECT,lstrlen(lpBuffer),lpBuffer))
  {
    LocalUnlock(hBuffer);
    LocalFree(hBuffer);
    return  FALSE;
  }

  /*-----------------------------------------*\
  | Print out a 2x2 inch box to exemplify the |
  | logical pixels/inch spacing.              |
  \*-----------------------------------------*/
  SetRect(&rc, iPageWidth / 2 - iRight1Inch, 1 * iDown1Inch,
               iPageWidth / 2 + iRight1Inch, 3 * iDown1Inch);

  if (!AddDrawObject(&rc, BoxTheHardWay, GRAPHICS_OBJECT,NULL,NULL))
  {
    LocalUnlock(hBuffer);
    LocalFree(hBuffer);
    return  FALSE;
  }

  LoadString(hInst,IDS_HEAD_PRINTAREA1,lpBuffer,128);

  SetRect(&rc, 0, 2 * iDown1Inch - iTextAscent, iPageWidth,
        2 * iDown1Inch + iTextDescent);

  if (!AddDrawObject(&rc, LineDrive, TEXT_OBJECT, lstrlen(lpBuffer), lpBuffer))
  {
    LocalUnlock(hBuffer);
    LocalFree(hBuffer);
    return  FALSE;
  }

  LocalUnlock(hBuffer);
  LocalFree(hBuffer);

  /*-----------------------------------------*\
  | Don't print footer, force a page.         |
  \*-----------------------------------------*/
  return    PrintPage(NULL);
}

/******************************************************************************

    Private Function:   JustAShadeOfGray

    Purpose:            Does a Gray Scale test in the given rectangle

    Change History:

    10-26-1990  I had to do something to kill time!

******************************************************************************/

BOOL JustAShadeOfGray(LPRECT lprcWhere,
                      WORD   wIgnoreMe,
                      LPVOID lpIgnoreMeToo)
{
  TstGrayScale(hdcTarget, lprcWhere -> left, lprcWhere -> top,
        RectWidth(*lprcWhere), RectHeight(*lprcWhere));

  return    TRUE;
}


/*---------------------------------------------------------------------------*\
| PRINT GRAY SCALE (header)                                                   |
|   This routine outputs a rectangle with the gray-scale for the printer.     |
|   The gray-scale ranges (1,1,1) -> (255,255,255).                           |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (Misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC               hDC       - Handle to device context.                   |
|   LPDEVCAPS         lpDevCaps - Device capabilities structure.              |
|   LPSTR             bAbort    - L-Pointer to printer abort flag.            |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   BOOL bPrintAbort - Printer abort flag.                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything went smoothly.                                  |
\*---------------------------------------------------------------------------*/
BOOL    PrintGrayScale(void)
{
  short nHeight;
  RECT  rc;

  LoadString(hInst,IDS_FOOT_GRAY,szDescription,STRING_SIZE);

  nHeight = PrintTestDescription(IDS_TST_HEAD_GRAY);

  SetRect(&rc, 0, 0, iPageWidth, 2 * iDown1Inch);
  OffsetRect(&rc, 0, nHeight + iDown1HalfInch);
  if (!AddDrawObject(&rc, JustAShadeOfGray, GRAPHICS_OBJECT, 0, NULL))
    return FALSE;

  return PrintPage(szDescription);
}

/*---------------------------------------------------------------------------*\
| PRINT SUPPORTED FUNCTIONS                                                   |
|   This routine outputs a rectangle with the gray-scale for the printer.     |
|   The gray-scale ranges (1,1,1) -> (255,255,255).                           |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (Misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC               hDC       - Handle to device context.                   |
|   LPDEVCAPS         lpDevCaps - Device capabilities structure.              |
|   LPSTR             bAbort    - L-Pointer to printer abort flag.            |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   BOOL bPrintAbort - Printer abort flag.                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything went smoothly.                                  |
\*---------------------------------------------------------------------------*/
BOOL    PrintFunctionSupport(void)
{
  short         nIdx;
  HANDLE        hLibrary;
  char          lpBuffer[80];
  static PSTR   pSupport[] = {"Enable"             ,"Disable"      ,
                              "Control"            ,"DeviceMode"   ,
                              "BitBlt"             ,"StrBlt"       ,
                              "ExtTextOut"         ,"StretchBlt"   ,
                              "FastBorder"         ,"Output"       ,
                              "Pixel"              ,"ScanLR"       ,
                              "ColorInfo"          ,"EnumObj"      ,
                              "EnumDFonts"         ,"GetCharWidth" ,
                              "DeviceBitmap"       ,"RealizeObject",
                              "SetAttribute"       ,"ExtDeviceMode",
                              "DeviceCapabilities" ,"AdvancedSetupDialog"};
  RECT          rc;

  LoadString(hInst,IDS_FOOT_ENTRY,szDescription,STRING_SIZE);

  SetRect(&rc, 5, 0, iPageWidth, iHeightOfOneTextLine);
  OffsetRect(&rc, 0, PrintTestDescription(IDS_TST_HEAD_FUNC));

  lstrcpy(lpBuffer,pPrinter.szDriver);
  lstrcat(lpBuffer,".DRV");
  AnsiUpper(lpBuffer);
  if ((hLibrary = LoadLibrary(lpBuffer)) < 32)
    return FALSE;

  for (nIdx = 0; nIdx < 22; nIdx++)
  {
    wsprintf(lpBuffer, "%20s -%s Supported", (LPSTR) pSupport[nIdx],
          (LPSTR) (GetProcAddress(hLibrary, pSupport[nIdx]) ? "" : " Not"));

    AddDrawObject(&rc, DoSomeText, TEXT_OBJECT, lstrlen(lpBuffer), lpBuffer);

    OffsetRect(&rc, 0, iHeightOfOneTextLineWithLeading);
  }

  FreeLibrary(hLibrary);

  return PrintPage(szDescription);
}
