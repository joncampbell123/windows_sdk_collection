/*---------------------------------------------------------------------------*\
| RASTER                                                                      |
|   This module contains the routines for handling the RASTER tests for       |
|   the printer driver.                                                       |
|                                                                             |
| DATE   : June 05, 1989                                                      |
|   Copyright   1989-1992 by Microsoft Corporation                            |
\*---------------------------------------------------------------------------*/

#include    <Windows.H>
#include    "PrntTest.H"


static struct
  {
    DWORD nIndex;
    PSTR  szTitle;
  } aRopCodes[] = {BLACKNESS , "Blackness" , NOTSRCERASE, "NotSrcErase",
                   NOTSRCCOPY, "NotSrcCopy", SRCERASE   , "SrcErase",
                   DSTINVERT , "DstInvert" , PATINVERT  , "PatInvert",
                   SRCINVERT , "SrcInvert" , SRCAND     , "SrcAnd",
                   MERGEPAINT, "MergePaint", MERGECOPY  , "MergeCopy",
                   SRCCOPY   , "SrcCopy"   , SRCPAINT   , "SrcPaint",
                   PATCOPY   , "PatCopy"   , PATPAINT   , "PatPaint",
                   WHITENESS , "Whiteness"};

typedef struct _BitBltTest  BITBLTTEST, FAR *LPBITBLTTEST;

struct _BitBltTest
  {
    int iSrcBrush;
    int iDestBrush;
    int iRopIndex;
  };

/*---------------------------------------------------------------------------*\
| PRINT STRETCHBIT TEST                                                       |
|   This routine performst the stretching of bitmaps on the printer device.   |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|                                                                             |
| PARAMETERS                                                                  |
|   none                                                                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HANDLE hInst - Module instance of EXE                                     |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL PrintStretchBlt(void)
{
  extern HANDLE hInst;

  HBITMAP hBitmap;
  BITMAP  bm;
  HDC     hMemDC;

  /*-----------------------------------------*\
  | Load mono-bitmap from resource file.      |
  \*-----------------------------------------*/
  if (!(hBitmap = LoadBitmap(hInst,"Bitmap1")))
    return(FALSE);

  /*-----------------------------------------*\
  | Create memory DC for BitBlt transfer.     |
  \*-----------------------------------------*/
  if (!(hMemDC = CreateCompatibleDC(hdcPrinter)))
    return(FALSE);

  /*-----------------------------------------*\
  | Select bitmap into DC-ready for transfer. |
  \*-----------------------------------------*/
  if (!SelectObject(hMemDC,hBitmap))
  {
    DeleteDC(hMemDC);
    return(FALSE);
  }

  /*-----------------------------------------*\
  | Transfer to Printer DC.                   |
  \*-----------------------------------------*/
  GetObject(hBitmap,sizeof(BITMAP),(LPSTR)&bm);
  StretchBlt(hdcTarget,0,0,576,576,hMemDC,0,0,
             bm.bmWidth,bm.bmHeight,SRCCOPY);

  /*-----------------------------------------*\
  | Clean up.                                 |
  \*-----------------------------------------*/
  DeleteDC(hMemDC);
  DeleteObject(hBitmap);
  if (!PrintPage(szDescription))
    return(FALSE);

  return(TRUE);
}


/*---------------------------------------------------------------------------*\
| BITBLT/ROP TEST                                                             |
|   This routine performs the BitBlt of Source/Destination/Pattern bitmap     |
|   combinations.  Currently, it does this for the 15 defined Rop Codes.      |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter()                                                             |
|   TestBitBltRop()                                                           |
|                                                                             |
| PARAMETERS                                                                  |
|   LPRECT            lprcWhere - Pointer to bounding rectangle.              |
|   WORD              wBlkSize  - Length of data block.                       |
|   LPBITBITBLTTEST   lpbbt     - Pointer to data block.                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL DoMeABitBlt(LPRECT       lprcWhere,
                 WORD         wBlkSize,
                 LPBITBLTTEST lpbbt)
{
  HBRUSH     hPatBrsh,hDstBrsh,hSrcBrsh;
  int        nObj;

  if (!(hPatBrsh = CreateHatchBrush(HS_FDIAGONAL,RGB(0,0,0))))
    return(FALSE);

  /*----------------------------*\
  | Create the Destination Brush.|
  \*----------------------------*/

  SetCurrentObject(tlTest.gtTest.hBrushes, lpbbt -> iDestBrush);
  CopyDeviceObject((LPSTR) &nObj, tlTest.gtTest.hBrushes);
  SetCurrentObject(hBrushes, nObj);
  hDstBrsh = CreateDeviceObject(hBrushes);

  /*---------------------*\
  | Create the Src Bitmap.|
  \*---------------------*/

  SetCurrentObject(tlTest.gtTest.hBrushes, lpbbt -> iSrcBrush);
  CopyDeviceObject((LPSTR) &nObj, tlTest.gtTest.hBrushes);
  SetCurrentObject(hBrushes, nObj);
  hSrcBrsh = CreateDeviceObject(hBrushes);

  TstBitBltRop(hdcTarget, lprcWhere -> left, lprcWhere -> top,
               RectHeight(*lprcWhere), RectWidth(*lprcWhere), hSrcBrsh,
               hDstBrsh, hPatBrsh, aRopCodes[lpbbt -> iRopIndex].nIndex,
               FALSE);

  /*-----------------------------------------*\
  | Free all remaining objects.               |
  \*-----------------------------------------*/

  DeleteObject(hSrcBrsh);
  DeleteObject(hDstBrsh);
  DeleteObject(hPatBrsh);

  return(TRUE);
}

/*---------------------------------------------------------------------------*\
| BITBLT/ROP TEST                                                             |
|   This routine performs the BitBlt of Source/Destination/Pattern bitmap     |
|   combinations.  Currently, it does this for the 15 defined Rop Codes.      |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintFooter() - (misc.c)                                                  |
|   BitBltRop()   - (ITE_BITM)                                                |
|                                                                             |
| PARAMETERS                                                                  |
|   none                                                                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL PrintBitBlt(void)

{
  BITBLTTEST    bbtToDo;
  RECT          rcText, rcTest;

  /*-----------------------------------------*\
  | Set the height/width of the bitmaps.      |
  \*-----------------------------------------*/
  SetRect(&rcTest, 0, 0, iRight3QuarterInches, iDown3QuarterInches);

  for   (bbtToDo.iDestBrush = 0;
         bbtToDo.iDestBrush < GetObjectCount(tlTest.gtTest.hBrushes);
         bbtToDo.iDestBrush++)
  {
    for   (bbtToDo.iSrcBrush = 0;
           bbtToDo.iSrcBrush < GetObjectCount(tlTest.gtTest.hBrushes);
           bbtToDo.iSrcBrush++)
    {
      for   (bbtToDo.iRopIndex = 0;
             bbtToDo.iRopIndex < 15;
             bbtToDo.iRopIndex++)
      {
        if (!rcTest.left)
          OffsetRect(&rcTest, 0, iHeightOfOneTextLineWithLeading);

        SetRect(&rcText, 0, 0,
                StringWidth(aRopCodes[bbtToDo.iRopIndex].szTitle),
                iHeightOfOneTextLine);

        OffsetRect(&rcText, rcTest.left,
                   rcTest.top - iHeightOfOneTextLineWithLeading);

        if (!AddDrawObject(&rcText, DoSomeText, TEXT_OBJECT,
                           lstrlen(aRopCodes[bbtToDo.iRopIndex].szTitle),
                           aRopCodes[bbtToDo.iRopIndex].szTitle))
          return FALSE;

        if (!AddDrawObject(&rcTest, DoMeABitBlt, GRAPHICS_OBJECT,
                           sizeof(BITBLTTEST), &bbtToDo))
          return FALSE;

        ExtendRect(rcTest, 0, iHeightOfOneTextLineWithLeading +
                   iDown1QuarterInch);

        if (!AdjustObject(&rcTest, iRight1Inch + iRight3QuarterInches,
                         iDown1Inch + iHeightOfOneTextLineWithLeading,
                         szDescription))
          return FALSE;

        ExtendRect(rcTest, 0, -(iHeightOfOneTextLineWithLeading +
                   iDown1QuarterInch));
      }
    }
  }

  if (rcTest.top || rcTest.left)
    return PrintPage(szDescription);
  else
    return TRUE;
}

HBRUSH  GetNextTestBrush(HDEVOBJECT     hAllBrushes,
                         HDEVOBJECT     hTstIndexes,
                         register LPINT lpiDestBrush)
{
  LOGBRUSH lb;
  int      nObj;

  do
  {
    SetCurrentObject(hTstIndexes,*lpiDestBrush);
    CopyDeviceObject((LPSTR)&nObj,hTstIndexes);
    SetCurrentObject(hAllBrushes,nObj);
    CopyDeviceObject((LPSTR)&lb,hAllBrushes);
  }
  while((lb.lbStyle != BS_SOLID) &&
       (++(*lpiDestBrush) < GetObjectCount(hTstIndexes)));

  if(lb.lbStyle != BS_SOLID)
    return(NULL);

  return(CreateDeviceObject(hAllBrushes));
}


/*---------------------------------------------------------------------------*\
| PRINT BITMAPS                                                               |
|   This routine peforms the raster tests for the printer driver.             |
|                                                                             |
| CALLED ROUTINES                                                             |
|   PrintTestDescription() - (misc.c)                                         |
|   PrintFooter()          - (misc.c)                                         |
|                                                                             |
| PARAMETERS                                                                  |
|   none                                                                      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   DEVCAPS         dcDevCaps - Device capabilities structure.                |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if passed.                                                    |
\*---------------------------------------------------------------------------*/
BOOL    PrintBitmaps(void)
{
  LoadString(hInst,IDS_FOOT_BITMAPS,szDescription,STRING_SIZE);

  /*-----------------------------------------*\
  | Print out test description.               |
  \*-----------------------------------------*/
  PrintTestDescription(IDS_TST_DSCR_RAST);
  if (!PrintPage(szDescription))
    return(FALSE);

  /*-----------------------------------------*\
  | Check to see if device supports bitmaps.  |
  | If it doesn't, then skip the test.        |
  \*-----------------------------------------*/
  if(dcDevCaps.wRasterCaps)
  {
/*  PrintStretchBlt();
*/  PrintBitBlt();
  }

  return(TRUE);
}
