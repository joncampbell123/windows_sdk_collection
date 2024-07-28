/*---------------------------------------------------------------------------*\
| DEVICE OBJECTS MODULE                                                       |
|   This is an object oriented library for common routines used for testing   |
|   applications.  It is accompanied with it's own API routines.              |
|                                                                             |
| OBJECT <DEVOBJECT>                                                          |
|                                                                             |
| METHODS                                                                     |
|   GetDeviceObjects()   - Enumerates all device supplied objects.            |
|   FreeDeviceObjects()  - Frees all device supplied objects.                 |
|   CopyDeviceObject()   - Copy current object structure to caller.           |
|   SetCurrentObject()   - Set the current object.                            |
|   GetCurrentObject()   - Get the current object.                            |
|   GetObjectCount()     - Get the number of objects.                         |
|   CreateDeviceObject() - Create a object for selection into a DC.           |
|   AddObject()          -                                                    |
|   RemoveObject()       -                                                    |
|                                                                             |
| Copyright 1990-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "isg_test.h"

extern HANDLE hInst;

/*---------------------------------------------------------------------------*\
| GET DEVICE OBJECTS - <Method>                                               |
|   This routine retrieves all the device pens, brushes or fonts depending    |
|   on the object specifier (wObject).  The routine stores these structures   |
|   in a global memory object along with a count and type of the object.      |
|                                                                             |
| CALLED ROUTINES                                                             |
|   EnumAllDevicePens()                                                       |
|   EnumAllDeviceBrushes()                                                    |
|   EnumAllDeviceFonts()                                                      |
|   EnumAllFontFaces()                                                        |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC         hDC       - Handle to the device context.                     |
|   WORD        wObject   - (DEV_PEN, DEV_BRUSH, DEV_FONT).                   |
|   WORD        wFlags    - Special Flags (device fonts only, etc.)           |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   HDEVOBJECT - Global handle to the device object.                          |
\*---------------------------------------------------------------------------*/
HDEVOBJECT PASCAL GetDeviceObjects(HDC  hDC,
                                   WORD wObject,
                                   WORD wFlags)
{
  FARPROC     lpProc;
  LPDEVOBJECT lpObjects;
  HDEVOBJECT  hObjects;
  BOOL        bError;

  /*-----------------------------------------*\
  | Create device object memory.              |
  \*-----------------------------------------*/
  if(!(hObjects = GlobalAlloc(GHND,(DWORD)sizeof(DEVOBJECT))))
       return(NULL);
  if(!(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects)))
  {
       GlobalFree(hObjects);
       return(NULL);
  }

  bError = FALSE;
  switch(wObject)
  {
    /*------------------------------------*\
    | Get Device Pens.                     |
    \*------------------------------------*/
    case DEV_PEN:
      if(lpObjects->hMem = GlobalAlloc(GHND,1l))
      {
        lpObjects->hDC      = hDC;
        lpObjects->wType    = DEV_PEN;
        lpObjects->nCount   = 0;
        lpObjects->nCurrent = 0;
        lpProc=MakeProcInstance(EnumAllDevicePens,hInst);
        if(!EnumObjects(hDC,OBJ_PEN,lpProc,(LPSTR)lpObjects))
          bError = TRUE;
        FreeProcInstance(lpProc);
      }
      else
        bError = TRUE;
      break;

    /*------------------------------------*\
    | Get Device Brushes.                  |
    \*------------------------------------*/
    case DEV_BRUSH:
      if(lpObjects->hMem = GlobalAlloc(GHND,1l))
      {
        lpObjects->hDC      = hDC;
        lpObjects->wType    = DEV_BRUSH;
        lpObjects->nCount   = 0;
        lpObjects->nCurrent = 0;
        lpProc=MakeProcInstance(EnumAllDeviceBrushes,hInst);
        if(!EnumObjects(hDC,OBJ_BRUSH,lpProc,(LPSTR)lpObjects))
          bError = TRUE;
        FreeProcInstance(lpProc);
      }
      else
        bError = TRUE;
      break;

    /*------------------------------------*\
    | Get Device Fonts.                    |
    \*------------------------------------*/
    case DEV_FONT:
      {
        DEVOBJECT    doFontFaces;
        LPSTR        lpFontFaces;
        int          nReturn;
        register int nIdx;

        /*-------------------------------*\
        | Retrieve font faces first.      |
        \*-------------------------------*/
        if(doFontFaces.hMem = GlobalAlloc(GHND,1l))
        {
          doFontFaces.nCount = 0;
          doFontFaces.wFlags=wFlags;
          lpProc=MakeProcInstance(EnumAllFontFaces,hInst);
          nReturn=EnumFonts(hDC,NULL,lpProc,(LPSTR)&doFontFaces);
          FreeProcInstance(lpProc);
          if(!nReturn)
          {
            GlobalFree(doFontFaces.hMem);
            GlobalUnlock(hObjects);
            GlobalFree(hObjects);
            return(NULL);
          }
        }
        else
        {
          GlobalUnlock(hObjects);
          GlobalFree(hObjects);
          return(NULL);
        }

        /*-------------------------------*\
        | Retrieve fonts assoc w/faces.   |
        \*-------------------------------*/
        if(!(lpFontFaces = GlobalLock(doFontFaces.hMem)))
        {
          GlobalFree(doFontFaces.hMem);
          GlobalUnlock(hObjects);
          GlobalFree(hObjects);
          return(NULL);
        }
        if(lpObjects->hMem = GlobalAlloc(GHND,1l))
        {
          lpObjects->hDC      = hDC;
          lpObjects->wType    = DEV_FONT;
          lpObjects->wFlags   = wFlags;
          lpObjects->nCount   = 0;
          lpObjects->nCurrent = 0;
          lpProc=MakeProcInstance(EnumAllDeviceFonts,hInst);

          for(nIdx=0; nIdx < doFontFaces.nCount; nIdx++)
            if(!EnumFonts(hDC,lpFontFaces+(nIdx*LF_FACESIZE),lpProc,
                         (LPSTR)lpObjects))
            {
              FreeProcInstance(lpProc);
              GlobalUnlock(doFontFaces.hMem);
              GlobalFree(doFontFaces.hMem);
              lpObjects->hMem = GlobalFree(lpObjects->hMem);
              return(NULL);
            }
          FreeProcInstance(lpProc);
          GlobalUnlock(doFontFaces.hMem);
          GlobalFree(doFontFaces.hMem);
        }
        else
        {
          GlobalUnlock(doFontFaces.hMem);
          GlobalFree(doFontFaces.hMem);
          GlobalUnlock(hObjects);
          GlobalFree(hObjects);
          return(NULL);
        }
      }
      break;

    /*------------------------------------*\
    | Reserve memory for object indicies.  |
    \*------------------------------------*/
    case DEV_INDEX:
      if(lpObjects->hMem = GlobalAlloc(GHND,1l))
      {
        lpObjects->hDC      = hDC;
        lpObjects->wType    = DEV_INDEX;
        lpObjects->nCount   = 0;
        lpObjects->nCurrent = 0;
      }
      else
        bError = TRUE;
      break;

    default:
      bError = TRUE;
  }
  GlobalUnlock(hObjects);

  if(bError)
  {
    GlobalFree(hObjects);
    hObjects = NULL;
  }

  return(hObjects);
}


/*---------------------------------------------------------------------------*\
| FREE DEVICE OBJECTS - <Method>                                              |
|   This routine removes the objects obtained by the GetDeviceObjects()       |
|   method.                                                                   |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT hObjects - Handle to Device Object memory.                     |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   HDEVOBJECT - NULL if sucessful.  Otherwise, the Device Object handle.     |
\*---------------------------------------------------------------------------*/
HDEVOBJECT PASCAL FreeDeviceObjects(HDEVOBJECT hObjects)
{
  LPDEVOBJECT lpObjects;

  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    if(lpObjects->hMem)
      GlobalFree(lpObjects->hMem);
    GlobalUnlock(hObjects);
  }

  return(GlobalFree(hObjects));
}


/*---------------------------------------------------------------------------*\
| SET CURRENT OBJECT - <Method>                                               |
|   This routine sets the current object for the device objects.              |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT hObjects - Handle to Device Object structure.                  |
|   short      nCurrent - Indicates the new current object to set.            |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   integer - current object.  Otherwise, returns -1.                         |
\*---------------------------------------------------------------------------*/
int PASCAL SetCurrentObject(HDEVOBJECT hObjects,
                            short      nCurrent)
{
  register LPDEVOBJECT lpObjects;

  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    if((nCurrent < lpObjects->nCount) && (nCurrent >= 0))
      lpObjects->nCurrent = nCurrent;
    else
      nCurrent = lpObjects->nCurrent;
    GlobalUnlock(hObjects);
    return(nCurrent);
  }

  return(-1);
}


/*---------------------------------------------------------------------------*\
| GET CURRENT OBJECT - <Method>                                               |
|   This routine gets the current object for the device objects.              |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT hObjects - Handle to Device Object structure.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   integer - current object.  Otherwise, -1 for failure.                     |
\*---------------------------------------------------------------------------*/
int PASCAL GetCurrentObject(HDEVOBJECT hObjects)
{
  register LPDEVOBJECT lpObjects;
  register short       nCurrent;

  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    nCurrent = lpObjects->nCurrent;
    GlobalUnlock(hObjects);
    return(nCurrent);
  }

  return(-1);
}


/*---------------------------------------------------------------------------*\
| GET OBJECT COUNT - <Method>                                                 |
|   This routine gets the number of objects for the hObject.                  |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT hObjects - Handle to Device Object structure.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   integer - current object.                                                 |
\*---------------------------------------------------------------------------*/
int PASCAL GetObjectCount(HDEVOBJECT hObjects)
{
  register LPDEVOBJECT lpObjects;
  register short       nObjects;

  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    nObjects = lpObjects->nCount;
    GlobalUnlock(hObjects);
    return(nObjects);
  }

  return(-1);
}


/*---------------------------------------------------------------------------*\
| ADD OBJECT - <Method>                                                       |
|   This routine adds an object type to the array of objects.                 |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT hObjects - Handle to Device Object structure.                  |
|   LPSTR      lpItem   - Pointer to the object to add.                       |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   integer - current object.                                                 |
\*---------------------------------------------------------------------------*/
int PASCAL AddObject(HDEVOBJECT hObjects,
                     LPSTR      lpItem)
{
  register LPDEVOBJECT  lpObjects;
  register short        nCurrent;
  register GLOBALHANDLE hMem;
  LPVOID                lpMem;

  nCurrent = -1;
  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    switch(lpObjects->wType)
    {
      case DEV_PEN:
        if(hMem = GlobalReAlloc(lpObjects->hMem,(DWORD)(sizeof(LOGPEN)*(1+lpObjects->nCount)),GHND))
        {
          nCurrent = lpObjects->nCount;
          lpMem = (LPVOID)GlobalLock(hMem);
          *(((LPLOGPEN)lpMem)+nCurrent) = *((LPLOGPEN)lpItem);
          lpObjects->nCurrent = lpObjects->nCount;
          GlobalUnlock(hMem);
          lpObjects->nCount++;
        }
        break;

      case DEV_BRUSH:
        if(hMem = GlobalReAlloc(lpObjects->hMem,(DWORD)(sizeof(LOGBRUSH)*(1+lpObjects->nCount)),GHND))
        {
          nCurrent = lpObjects->nCount;
          lpMem = (LPVOID)GlobalLock(hMem);
          *(((LPLOGBRUSH)lpMem)+nCurrent) = *((LPLOGBRUSH)lpItem);
          lpObjects->nCurrent = lpObjects->nCount;
          GlobalUnlock(hMem);
          lpObjects->nCount++;
        }
        break;
      case DEV_FONT:
        if(hMem = GlobalReAlloc(lpObjects->hMem,(DWORD)(sizeof(FONT)*(1+lpObjects->nCount)),GHND))
        {
          nCurrent = lpObjects->nCount;
          lpMem = (LPVOID)GlobalLock(hMem);
          *(((LPFONT)lpMem)+nCurrent) = *((LPFONT)lpItem);
          lpObjects->nCurrent = lpObjects->nCount;
          GlobalUnlock(hMem);
          lpObjects->nCount++;
        }
        break;
      case DEV_INDEX:
        if(hMem = GlobalReAlloc(lpObjects->hMem,(DWORD)(sizeof(int)*(1+lpObjects->nCount)),GHND))
        {
          nCurrent = lpObjects->nCount;
          lpMem = (LPINT)GlobalLock(hMem);
          *(((LPINT)lpMem)+nCurrent) = *(LPINT)lpItem;
          lpObjects->nCurrent = lpObjects->nCount;
          GlobalUnlock(hMem);
          lpObjects->nCount++;
        }
        break;
    }
    GlobalUnlock(hObjects);
  }

  return(nCurrent);
}


/*---------------------------------------------------------------------------*\
| REMOVE OBJECT - <Method>                                                    |
|   This routine removes the number from the current position for an          |
|   object type.                                                              |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT hObjects - Handle to Device Object structure.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   integer - current object.                                                 |
\*---------------------------------------------------------------------------*/
int PASCAL RemoveObject(HDEVOBJECT hObjects)
{
  register LPDEVOBJECT  lpObjects;
  register short        nCurrent;
  LPINT                 lpMem;
  int                   nIdx;

  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    if(lpObjects->wType != DEV_INDEX)
    {
      GlobalUnlock(hObjects);
      return(-1);
    }

    lpMem = (LPINT)GlobalLock(lpObjects->hMem);

    for(nIdx=lpObjects->nCurrent; nIdx < lpObjects->nCount-1; nIdx++)
      *(lpMem+nIdx) = *(lpMem+nIdx+1);
    GlobalUnlock(lpObjects->hMem);
    lpObjects->nCount--;
    lpObjects->nCurrent=0;
    GlobalReAlloc(lpObjects->hMem,(DWORD)lpObjects->nCount,GHND);
    GlobalUnlock(hObjects);

    return(nCurrent);
  }

  return(-1);
}


/*---------------------------------------------------------------------------*\
| COPY DEVICE OBJECT - <Method>                                               |
|   This routine copies the current device object into the memory specified   |
|   by lpCopy.  Uses the current object.                                      |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPSTR      lpCopy   - Pointer to memory to store the object.              |
|   HDEVOBJECT hObjects - Handle to Device Object structure.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOLEAN - TRUE if object was copied.                                      |
\*---------------------------------------------------------------------------*/
BOOL PASCAL CopyDeviceObject(LPSTR      lpCopy,
                             HDEVOBJECT hObjects)
{
  LPDEVOBJECT     lpObjects;
  register LPVOID lpMem;
  register short  nIdx;

  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    nIdx = lpObjects->nCurrent;
    switch(lpObjects->wType)
    {
      case DEV_PEN:
        lpMem = (LPVOID)GlobalLock(lpObjects->hMem);
        *((LPLOGPEN)lpCopy) = *(((LPLOGPEN)lpMem)+nIdx);
        GlobalUnlock(lpObjects->hMem);
        break;

      case DEV_BRUSH:
        lpMem = (LPVOID)GlobalLock(lpObjects->hMem);
        *((LPLOGBRUSH)lpCopy) = *(((LPLOGBRUSH)lpMem)+nIdx);
        GlobalUnlock(lpObjects->hMem);
        break;

      case DEV_FONT:
        lpMem = (LPVOID)GlobalLock(lpObjects->hMem);
        *((LPFONT)lpCopy) = *(((LPFONT)lpMem)+nIdx);
        GlobalUnlock(lpObjects->hMem);
        break;

      case DEV_INDEX:
        lpMem = (LPVOID)GlobalLock(lpObjects->hMem);
        *((LPINT)lpCopy) = *(((LPINT)lpMem)+nIdx);
        GlobalUnlock(lpObjects->hMem);
        break;

      default:
        GlobalUnlock(hObjects);
        return(FALSE);
    }
    GlobalUnlock(hObjects);
    return(TRUE);
  }

  return(FALSE);
}


/*---------------------------------------------------------------------------*\
| CREATE DEVICE OBJECT - <Method>                                             |
|   This routine creates a GDI recognizable object representing the device    |
|   object.  This function uses the current selected object for creation.     |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDEVOBJECT hObjects - Handle to Device Object structure.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   HANDLE - Handle to GDI object (hPen, hBrush, hFont).                      |
\*---------------------------------------------------------------------------*/
HANDLE PASCAL CreateDeviceObject(HDEVOBJECT hObjects)
{
  register short  nIdx;
  register LPVOID lpMem;
  HANDLE          hObject;
  LPDEVOBJECT     lpObjects;

  hObject = NULL;
  if(lpObjects = (LPDEVOBJECT)GlobalLock(hObjects))
  {
    nIdx = lpObjects->nCurrent;
    switch(lpObjects->wType)
    {
      case DEV_PEN:
        lpMem = (LPVOID)GlobalLock(lpObjects->hMem);
        hObject = (HPEN)CreatePenIndirect(((LPLOGPEN)lpMem)+nIdx);
        GlobalUnlock(lpObjects->hMem);
        break;

      case DEV_BRUSH:
        lpMem = (LPVOID)GlobalLock(lpObjects->hMem);
        hObject = (HBRUSH)CreateBrushIndirect(((LPLOGBRUSH)lpMem)+nIdx);
        GlobalUnlock(lpObjects->hMem);
        break;

      case DEV_FONT:
        lpMem = (LPVOID)GlobalLock(lpObjects->hMem);
        hObject = (HFONT)CreateFontIndirect(&(((LPFONT)lpMem)+nIdx)->lf);
        GlobalUnlock(lpObjects->hMem);
        break;

      case DEV_INDEX:
      default:
        hObject = NULL;
    }
    GlobalUnlock(hObjects);
  }

  return(hObject);
}


/*---------------------------------------------------------------------------*\
| ENUMERATE ALL FONT TYPEFACES                                                |
|   This routine enumerates all Font Type-Facess associated with a given      |
|   device.  It stores the character string representing the typeface in      |
|   a globally defined block (eFaces.Mem).  This is called by Windows via     |
|   the EnumFonts() call.                                                     |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPLOGFONT    lf     - A logical font structure for device font.           |
|   LPTEXTMETRIC tm     - A text metric structure for font.                   |
|   short        nType  - Type of font.                                       |
|   LPDEVOBJECT  eFaces - Global structure containing enumed font faces.      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   This routine returns a non-zero integer until all font faces have         |
|   been enumerated, then it returns (0).                                     |
\*---------------------------------------------------------------------------*/
int FAR PASCAL EnumAllFontFaces(LPLOGFONT    lf,
                                LPTEXTMETRIC tm,
                                short        nType,
                                LPDEVOBJECT  eFaces)
{
  register GLOBALHANDLE hMem;
  register LPSTR        lpFaces;

  /*-----------------------------------------*\
  | ReAlloc space required for next storage of|
  | the font face, then store the font face.  |
  \*-----------------------------------------*/
  if(hMem = GlobalReAlloc(eFaces->hMem,(DWORD)LF_FACESIZE*(1+eFaces->nCount),GMEM_MOVEABLE))
  {
    if(lpFaces = GlobalLock(hMem))
    {
      lstrcpy(lpFaces+(eFaces->nCount*LF_FACESIZE),lf->lfFaceName);
      GlobalUnlock(hMem);
      eFaces->nCount++;
      return(1);
    }
  }

  return(0);
}

///////////////////////////////////////////////////////////////
//
//  Date Created: 1991
//  Module:       Object
//
//  Description:  Categorizes an enumerated font as TrueType,
//                Device (Non-TrueType), or GDI.
//
//  Calling Function:
//
//  Referenced Functions:
//
//  Return Value:
//
//  History:
//
//////////////////////////////////////////////////////////////
WORD EvaluateFontType(int nType)
{
  if(nType & TRUETYPE_FONTTYPE)
    return TT_FONTS;
  else
  {
    if(nType & DEVICE_FONTTYPE)
      return NON_TT_DEVICE_FONTS;
    else
      return NON_TT_NON_DEVICE_FONTS;
  }
}


/*---------------------------------------------------------------------------*\
| ENUMERATE ALL FONTS                                                         |
|   This routine enumerates all fonts associated with a particular type-      |
|   face.  For the given type-face, this routine stores the logical font      |
|   as well as the textmetric structures in Global memory.                    |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPLOGFONT    lf     - A logical font structure for device font.           |
|   LPTEXTMETRIC tm     - A text metric structure for font.                   |
|   short        nType  - Type of font.                                       |
|   LPDEVOBJECT  eFaces - Global structure containing enumed font faces.      |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   This routine returns a non-zero integer until all font facess have        |
|   been enumerated, then it returns (0).                                     |
\*---------------------------------------------------------------------------*/
int FAR PASCAL EnumAllDeviceFonts(LPLOGFONT    lf,
                                  LPTEXTMETRIC tm,
                                  short        nType,
                                  LPDEVOBJECT  eFonts)
{
  register GLOBALHANDLE hMem;
  register LPFONT       lpFonts;
  static int            nFonts=0;

  // The very first thing we want to do is decide whether or not we
  // want to enumerate this font. Compare eFonts->wFlags and the font type.
  if(!(eFonts->wFlags & EvaluateFontType(nType)))
    return 1;

  /*-----------------------------------------*\
  | ReAlloc space required for next storage   |
  | of font structure.  Set the new block up  |
  | by the size of the FONT structure.        |
  \*-----------------------------------------*/
  if(hMem = GlobalReAlloc(eFonts->hMem,(DWORD)(sizeof(FONT)*((DWORD)(1+eFonts->nCount))),GMEM_MOVEABLE))
  {
    if(lpFonts=(LPFONT)GlobalLock(hMem))
    {
      if (hMem != eFonts->hMem) /* new selector has been assigned; */
      {    /* save new values for caller.  */
        eFonts->hMem = hMem;
      }
      (lpFonts+eFonts->nCount)->nFontType = nType;
      (lpFonts+eFonts->nCount)->lf     = *lf;
      (lpFonts+eFonts->nCount)->tm     = *tm,
      GlobalUnlock(hMem);
      eFonts->nCount++;
      return(++nFonts);
    }
  }
  return(0);
}


/*---------------------------------------------------------------------------*\
| ENUMERATE DEVICE PENS                                                       |
|   This routine enumerates all solid color brushes associated with a         |
|   particular device.  It receives a lp to a Logical Brush struct, and       |
|   stores only BS_SOLID colors.  It stores the DWORD (color) values in       |
|   the enumerate structure (eBrushes).                                       |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPLOGPEN    lp    - Logical Pen struct.                                   |
|   LPDEVOBJECT ePens - Long pointer to global Pens structure.                |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|  -none-                                                                     |
|                                                                             |
| RETURNS                                                                     |
|   Returns a zero when all pens have been enumerated.                        |
\*---------------------------------------------------------------------------*/
int FAR PASCAL EnumAllDevicePens(LPLOGPEN    lp,
                                 LPDEVOBJECT ePens)
{
  register GLOBALHANDLE hMem;
  register LPLOGPEN     lpPen;

  /*-----------------------------------------*\
  | This is a hack for devices which don't do |
  | their own enumeration.  ie. printers.     |
  \*-----------------------------------------*/
  if(lp->lopnColor != GetNearestColor(ePens->hDC,lp->lopnColor))
    return(1);

  /*-----------------------------------------*\
  | Realloc space required for next storage   |
  | of pen structure.  Set the new block up   |
  | by the size of a Logical Pen struct.   |
  \*-----------------------------------------*/
  if(hMem = GlobalReAlloc(ePens->hMem,(DWORD)(sizeof(LOGPEN)*(1+ePens->nCount)),GMEM_MOVEABLE))
  {
    if(lpPen=(LPLOGPEN)GlobalLock(ePens->hMem))
    {
      *(lpPen+ePens->nCount) = *lp;
      GlobalUnlock(hMem);
      ePens->nCount++;
      return(1);
    }
  }

  return(0);
}


/*---------------------------------------------------------------------------*\
| ENUMERATE DEVICE BRUSHES                                                    |
|   This routine enumerates all solid color brushes associated with a         |
|   particular device.  It receives a lp to a Logical Brush struct, and       |
|   stores the brush structure.  It stores onley the DWORD (color) values     |
|   in the structure eBrushes.                                                |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   LPLOGBRUSH  lb        - logical brush struct.                             |
|   LPENUMERATE eBrushes  - Long pointer to Brushe structure.                 |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   Returns a zero when all brushes have been enumerated.                     |
\*---------------------------------------------------------------------------*/
int FAR PASCAL EnumAllDeviceBrushes(LPLOGBRUSH  lb,
                                    LPDEVOBJECT eBrushes)
{
  register GLOBALHANDLE hMem;
  register LPLOGBRUSH   lpBrush;

  /*-----------------------------------------*\
  | This is a hack for devices which don't do |
  | their own enumeration.  ie. printers.     |
  \*-----------------------------------------*/
  if(lb->lbColor != GetNearestColor(eBrushes->hDC,lb->lbColor))
    return(1);

  /*-----------------------------------------*\
  | Realloc space required for next storage   |
  | of brush structure.  Set the new block up |
  | by the size of a DWORD.          |
  \*-----------------------------------------*/
  if(hMem = GlobalReAlloc(eBrushes->hMem,
                         (DWORD)(sizeof(LOGBRUSH)*(1+eBrushes->nCount)),
                          GMEM_MOVEABLE))
  {
    if(lpBrush=(LPLOGBRUSH)GlobalLock(eBrushes->hMem))
    {
      *(lpBrush+eBrushes->nCount) = *lb;
      GlobalUnlock(hMem);
      eBrushes->nCount++;
      return(1);
    }
  }

  return(0);
}
