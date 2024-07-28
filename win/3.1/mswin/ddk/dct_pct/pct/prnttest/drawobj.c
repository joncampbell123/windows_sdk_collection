/******************************************************************************

    DrawObj.C

    Drawing Object and Page Image Management module

    Copyright   1990-1992 by Microsoft Corporation

    Change History:

    10-15-1990  Created and coded it
    02-15-1991  Added Print style changes and Copyright notice

******************************************************************************/

#include    <Windows.H>

#define     GLOBALVAR
#include    "PrntTest.H"

typedef struct _DrawObject  DRAWOBJECT, FAR *LPDRAWOBJECT;
typedef struct _PageImage   IMAGE, FAR *LPIMAGE;

struct _DrawObject
  {
    RECT        rc;
    WORD        wfAttrs;
    LPDRAWFN    lpDrawFn;
    WORD        wArgBlockSize;
  };

struct _PageImage
  {
    WORD    wSize;
    WORD    wfAttrs;
    WORD    wObjects;
    RECT    rcGraphicsBounds;
  };

typedef HANDLE          HGMEM;
typedef unsigned char   FAR *LPBYTE;

static HGMEM    hgmemPage = NULL;

/******************************************************************************

    Private Function:   InitPage

    Purpose:            Initialize the page control structure

    Parameters:         None

    Side effects:

        If successful, a page control structure is allocated in global
            memory.

    Change History:

    10-24-1990  Started this fiasco

******************************************************************************/

BOOL InitPage(void)
{
  LPIMAGE    lpimgPage;

  /*
    If global memory is already allocated to a page image, free it.
  */

  if    (hgmemPage)
    GlobalFree(hgmemPage);

  /*
    Allocate and fill in an empty page image.
  */

  hgmemPage = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (DWORD) sizeof(IMAGE));
  if (!hgmemPage)
    return  FALSE;

  lpimgPage = (LPIMAGE) GlobalLock(hgmemPage);
  if (!lpimgPage)
  {
    GlobalFree(hgmemPage);
    return    FALSE;
  }

  lpimgPage -> wSize = sizeof(IMAGE);

  GlobalUnlock(hgmemPage);
  return    TRUE;
}

/******************************************************************************

    Macro:      NextObject

    Purpose:    Given an object pointer, advances pointer to next object.

    The Image of the object has a fixed length part (the DRAWOBJECT
    structure), and a variable length part (the Argument Block).  The length
    of the variable length part is in the wArgBlockSize field of the
    DRAWOBJECT.  I use a cast to force byte addition, then cast the result to
    LPDRAWOBJECT to keep the correct typing.

******************************************************************************/

#define NextObject(x)   ((LPDRAWOBJECT) (((LPBYTE) (x)) +\
                         (x -> wArgBlockSize + sizeof(DRAWOBJECT))))

/******************************************************************************

    Public Function:    AddObject

    Purpose:            Adds the given object to the list of objects to be
                        drawn on the current page.

    Parameters:
        prcBounds -     The bounding rectangle for the object.
        lpDrawFn  -     The rendering routine for the object.
        wfAttr -        Flags denoting whether the object is text/graphics.
        wArgBlockSize - Size in bytes of the arguments to the function.
        lpstrArgBlock - Image of the arguments to the rendering routine.

    Side effects:
        If successful, increases the size of the allocated page image by
            wArgBlockSize + sizeof(DRAWOBJECT) bytes.

    Change History:

    10-24-1990  My fault
    07-01-1991  Added kludge to prevent text from being overwritten
                by the footer.
    10-04-1991  Removed the kludge by shortening page by height
                of one line of text for all objects.

******************************************************************************/

BOOL  AddDrawObject(PRECT    prcBounds,
                    LPDRAWFN lpDrawFn,
                    WORD     wfAttrs,
                    WORD     wArgBlockSize,
                    LPSTR    lpstrArgBlock)

{
  WORD          wSizeofObject, wPageSize;
  LPIMAGE       lpimgPage;
  LPDRAWOBJECT  lpObject;
  LPBYTE        lpbTarget;

  if (!hgmemPage && !InitPage())
    return  FALSE;

  wSizeofObject = wArgBlockSize + sizeof(DRAWOBJECT);

  if (wSizeofObject < wArgBlockSize)
    return  FALSE;      /*  Object is too big   */

  wPageSize = LOWORD(GlobalSize(hgmemPage));

  lpimgPage = (LPIMAGE) GlobalLock(hgmemPage);
  if (!lpimgPage)
    return  FALSE;

  if    (lpimgPage -> wSize + wSizeofObject <=
         min(wSizeofObject, lpimgPage -> wSize))
  {
    GlobalUnlock(hgmemPage);
    return    FALSE;
  }

  if (lpimgPage -> wSize + wSizeofObject > wPageSize)
  {
    wPageSize = lpimgPage -> wSize + wSizeofObject;
    GlobalUnlock(hgmemPage);
    hgmemPage = GlobalReAlloc(hgmemPage, (DWORD) wPageSize, GMEM_MOVEABLE);
    if (!hgmemPage)
      return  FALSE;

    lpimgPage = (LPIMAGE) GlobalLock(hgmemPage);
    if (!lpimgPage)
      return  FALSE;
  }

  lpimgPage -> wfAttrs |= wfAttrs & (TEXT_OBJECT | GRAPHICS_OBJECT);
  if (wfAttrs & GRAPHICS_OBJECT)
    UnionRect(&lpimgPage -> rcGraphicsBounds, prcBounds,
              &lpimgPage -> rcGraphicsBounds);

  /*
    Place the object pointer at the end of the buffer
  */

  lpObject = (LPDRAWOBJECT) (((LPBYTE) lpimgPage) + lpimgPage -> wSize);

  /*
    Move the object image into the buffer
  */

  lpObject -> rc.left = prcBounds -> left;
  lpObject -> rc.top = prcBounds -> top;
  lpObject -> rc.right = prcBounds -> right;
  lpObject -> rc.bottom = prcBounds -> bottom;
  lpObject -> lpDrawFn = lpDrawFn;
  lpObject -> wfAttrs = wfAttrs;
  lpObject -> wArgBlockSize = wArgBlockSize;
  for (lpbTarget = (LPBYTE) (lpObject + 1); wArgBlockSize--;)
    *lpbTarget++ = *lpstrArgBlock++;

  /*
    Update the records of page image size & object count, then clean up and
    go home!
  */

  lpimgPage -> wSize += wSizeofObject;
  lpimgPage -> wObjects++;
  GlobalUnlock(hgmemPage);

  return    TRUE;
}

/******************************************************************************

    Public Function:    DrawPage

    Purpose:            Renders each page object, and does the proper banding
                        or framing, as specified by the user (or the
                        realities of the printer).

    Change History:

    10-24-1990  I did it (the smoking gun tells all!)
    11-17-1991  Added bAbort to the loops here, so we can
                abort in the middle of a page.  Since we're
                not going to finish the document anyway, it
                doesn't break anything.

******************************************************************************/

BOOL cdecl  DrawPage(void)
{
  LPIMAGE           lpimgPage;
  WORD              wObjects;
  LPDRAWOBJECT      lpdwobjCurrent;
  RECT              rcThisBand;
  BOOL              bPassed;
  BANDINFOSTRUCT    bis;
  int               nEscape;

  if (!hgmemPage || !hdcTarget)
    return  FALSE;      /*  No page to render?  */

  lpimgPage = (LPIMAGE) GlobalLock(hgmemPage);
  if (!lpimgPage)
    return  FALSE;

  if (!bUseBanding)
  {

    /*
      This is the easy case- print everything on the page, and issue a
      NEWFRAME escape, when you're done!

      Well, it used to be that way- if you're Win 3.0 style- that still
      be the case.  However, in Win 3.1 style, you issue StartPage, then
      draw, then EndPage.
    */

    if    (wStyleSelected > 30)
    {
      FIND31GDICALL lpFn;

      if(lpFn=Find31GDICall(STARTPAGE_ORDINAL))
        nEscape=(*lpFn)(hdcTarget);
      else
        nEscape=-1;

      if  (nEscape < 0)
      {
        MessageBox(NULL, "StartPage GDI Call failed? (DrawObj.C)",
              "Assertion", MB_OK);
        return  FALSE;
      }
    }

    // This for loop actually replays to the page.  Let's add a condition
    // for bAbort, otherwise it takes close to forever to abort if we're
    // already in this loop.
    lpdwobjCurrent = (LPDRAWOBJECT) (lpimgPage + 1);
    for (wObjects = lpimgPage -> wObjects;
         !bAbort && wObjects-- &&
         // This is where the callback functions get called
         (*lpdwobjCurrent -> lpDrawFn) (&(lpdwobjCurrent -> rc),
                                          lpdwobjCurrent -> wArgBlockSize,
                                          lpdwobjCurrent + 1);
          lpdwobjCurrent = NextObject(lpdwobjCurrent));

    GlobalFree(hgmemPage);
    hgmemPage = NULL;

    if(30 == wStyleSelected)
      nEscape=Escape(hdcTarget,NEWFRAME,NULL,NULL,NULL);
    else
    {
      FIND31GDICALL lpFn;

      if(lpFn=Find31GDICall(ENDPAGE_ORDINAL))
        nEscape=(*lpFn)(hdcTarget);
      else
        nEscape=-1;
    }
    if((nEscape < 0) || bAbort)
      return  FALSE;

    return  !(++wObjects);
  }

  /*
    Alas, if we get here, we've been a-band-oned!  (Aaarrrggghh?)
  */

  if (Escape(hdcTarget, NEXTBAND, NULL, NULL, (LPSTR) &rcThisBand) <= 0 ||
         bAbort)
  {
    GlobalFree(hgmemPage);
    return    FALSE;
  }

  bis.fGraphics = !!(lpimgPage -> wfAttrs & GRAPHICS_OBJECT);
  bis.fText = !!(lpimgPage -> wfAttrs & TEXT_OBJECT);
  bis.rcGraphics.left = lpimgPage -> rcGraphicsBounds.left;
  bis.rcGraphics.top = lpimgPage -> rcGraphicsBounds.top;
  bis.rcGraphics.right = lpimgPage -> rcGraphicsBounds.right;
  bis.rcGraphics.bottom = lpimgPage -> rcGraphicsBounds.bottom;

  bPassed = TRUE;
  while (!IsRectEmpty(&rcThisBand) && !bAbort)
  {
    RECT  rcDummy;

    Escape(hdcTarget, BANDINFO, sizeof(BANDINFOSTRUCT), (LPSTR) &bis,
          (LPSTR) &bis);

    lpdwobjCurrent = (LPDRAWOBJECT) (lpimgPage + 1);
    for   (wObjects = lpimgPage -> wObjects;
           wObjects-- && !bAbort;
           lpdwobjCurrent = NextObject(lpdwobjCurrent))

      if  (IntersectRect(&rcDummy, &rcThisBand, &(lpdwobjCurrent -> rc)) &&
          (((lpdwobjCurrent -> wfAttrs & GRAPHICS_OBJECT) && bis.fGraphics) ||
          ((lpdwobjCurrent -> wfAttrs & TEXT_OBJECT) && bis.fText)))

        bPassed &= (*lpdwobjCurrent -> lpDrawFn)
                   (&(lpdwobjCurrent -> rc), lpdwobjCurrent -> wArgBlockSize,
                   lpdwobjCurrent + 1);
      ;

    if (Escape(hdcTarget, NEXTBAND, NULL, NULL,
              (LPSTR) &rcThisBand) <= 0 || bAbort)
    {
      GlobalFree(hgmemPage);
      return    FALSE;
    }
  }

  GlobalFree(hgmemPage);
  hgmemPage = NULL;
  return    bPassed;
}

/******************************************************************************

    Public Function:    AdjustPosition

    Purpose:            Ensures a given rectangle is on the page.


    Basically, if the rectangle is off the page, its rectangle is adjusted to
    make it fit.  If there is no room left at all, the page is printed, and
    the rectangle is moved to 0,0.  If the given X-offset is 0, the object
    is moved to the "next line"- X offset is set to 0, y is added.

    Change History:

    10-25-1990  My fault
    07-01-1991  Added kludge to prevent text from being overwritten
                by the footer.
    10-04-1991  Removed the kludge by shortening page by height
                of footer text for all objects.

******************************************************************************/

BOOL cdecl  AdjustObject(LPRECT lprcBounds,
                         int    iXOffset,
                         int    iYOffset,
                         LPSTR  lpstrDescription)

{
  OffsetRect(lprcBounds, iXOffset ? iXOffset : -lprcBounds -> left,
             iXOffset ? 0 : iYOffset);

  if ((lprcBounds -> right <= iPageWidth) &&
     ((lprcBounds -> bottom) <= (iPageHeight-iHeightOfOneTextLineWithLeading)))
    return  TRUE;

  if (iXOffset)
  {
    OffsetRect(lprcBounds, -lprcBounds -> left, iYOffset);
    if (lprcBounds -> bottom <= (iPageHeight-iHeightOfOneTextLineWithLeading))
      return  TRUE;
  }

  /*
    If you got to this point, you've reached the end of the page.  Time to
    print it, and place the object at the top of the next page.
  */

  if (!PrintPage(lpstrDescription))
    return  FALSE;

  OffsetRect(lprcBounds, -lprcBounds -> left, -lprcBounds -> top);
  return    TRUE;
}

/******************************************************************************

    Private Function:   LetsPlayFootsie

    Purpose:            Prints the footer (drawing object function)

    Change History:

    10-27-1990  What, me work?

******************************************************************************/

BOOL LetsPlayFootsie(LPRECT lprc,
                     WORD   wLength,
                     LPSTR  lpBuffer)
{
  int        nWidth;

  /*-----------------------------------------*\
  | If no Caption, then do not print footer.  |
  \*-----------------------------------------*/
  if    (!wLength)
    return  TRUE;

  /*-----------------------------------------*\
  | Get info based on default font.           |
  \*-----------------------------------------*/
  nWidth=(int)LOWORD(GetTextExtent(hdcTarget,lpBuffer,wLength));
  TextOut(hdcTarget,(lprc->right-nWidth)/2,lprc->top,lpBuffer,wLength);

  return(TRUE);
}

/*---------------------------------------------------------------------------*\
| PRINT FOOTER LINE                                                           |
|   This routine is used to print a footer-message at the bottom of each page |
|   of the test output.  The test will pass a string to the routine which     |
|   identifys the test being output.  If szCaption is NULL, then no footer    |
|   is to be printed.                                                         |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPSTR             szCaption   - String to append to footer.                 |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if everything is OK, FALSE if error occured.                  |
\*---------------------------------------------------------------------------*/
BOOL PrintPage(LPSTR szCaption)
{

  /*-----------------------------------------*\
  | If no Caption, then do not print footer.  |
  \*-----------------------------------------*/
  if    (szCaption)
  {
    HANDLE        hBuffer;
    LPSTR         lpBuffer;
    RECT          rc;

    /*-----------------------------------------*\
    | Get the test string for the test.         |
    \*-----------------------------------------*/
    if    (!(hBuffer = LocalAlloc(LHND,128)))
      return(FALSE);

    if    (!(lpBuffer = (LPSTR)LocalLock(hBuffer)))
    {
      LocalFree(hBuffer);
      return(FALSE);
    }

    /*-----------------------------------------*\
    | Create the footer string.  Then output to |
    | the printer at the bottom of the page.    |
    | Make sure to restore the old text align.  |
    \*-----------------------------------------*/
    lstrcpy(lpBuffer,"PrntTest -> ");
    lstrcat(lpBuffer,szCaption);
    SetRect(&rc, 0, iPageHeight - (iHeightOfOneTextLineWithLeading),
            iPageWidth, iPageHeight);

    if (!AddDrawObject(&rc, LetsPlayFootsie, TEXT_OBJECT, lstrlen(lpBuffer),
                       lpBuffer))
      return  FALSE;

    LocalUnlock(hBuffer);
    LocalFree(hBuffer);
  }

  return DrawPage();
}
