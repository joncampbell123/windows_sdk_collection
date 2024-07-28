/*---------------------------------------------------------------------------*\
| BITMAP MODULE                                                               |
|   This is a common library module for manipulating DDB's and DIB's.         |
|                                                                             |
| STRUCTURE (----)                                                            |
|                                                                             |
| FUNCTION EXPORTS                                                            |
|   CreateBrushBitmap()                                                       |
|   CreatePixelBitmap()                                                       |
|   CreateRGBBitmap()                                                         |
|   OutputDDBToDevice()                                                       |
|   OutputDIBToDevice()                                                       |
|   ConvertDDBtoDIB()                                                         |
|   GetColorTableSize()                                                       |
|   GetMemoryBitmap()                                                         |
|   CompareBitmaps()                                                          |
|                                                                             |
| Copyright 1990-1992 by Microsoft Corporation                                |
| SEGMENT: _BITMAP                                                            |
\*---------------------------------------------------------------------------*/

#include <windows.h>
#include "isg_test.h"

/*---------------------------------------------------------------------------*\
| CREATE BRUSH BITMAP                                                         |
|   This routine creates a rectangular bitmap representing the brush passed   |
|   to the funtion.                                                           |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC    hDC     - Handle to device context.                                |
|   int    iWidth  - Width in device units.                                   |
|   int    iHeight - Height in device units.                                  |
|   HBRUSH hBrush  - Handle to a brush.                                       |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   HBITMAP - Handle to a Device Dependant Bitmap.                            |
\*---------------------------------------------------------------------------*/
HBITMAP FAR PASCAL CreateBrushBitmap(HDC            hDC,
                                     register short nWidth,
                                     register short nHeight,
                                     HBRUSH         hBrush)
{
  HDC      hMemDC;
  register HBITMAP hBitmap;

  if(hMemDC = CreateCompatibleDC(hDC))
  {
    if(hBitmap = CreateCompatibleBitmap(hDC,nWidth,nHeight))
    {
      if(SelectObject(hMemDC,hBitmap) && SelectObject(hMemDC,hBrush))
      {
        PatBlt(hMemDC,0,0,nWidth,nHeight,PATCOPY);
        DeleteDC(hMemDC);
        return(hBitmap);
      }
      DeleteObject(hBitmap);
    }
    DeleteDC(hMemDC);
  }
  return(NULL);
}

/*---------------------------------------------------------------------------*\
| CREATE PIXEL BITMAP                                                         |
|                                                                             |
|   This routine creates a bitmap representation of the pixel point specified |
|   to the funtion.                                                           |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC    hDC     - Handle to device context.                                |
|   int    iWidth  - Width in device units.                                   |
|   int    iHeight - Height in device units.                                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   HBITMAP - Handle to a Device Dependant Bitmap.                            |
\*---------------------------------------------------------------------------*/
HBITMAP FAR PASCAL CreatePixelBitmap(HDC            hDC,
                                     register short x,
                                     register short y)
{
  HDC              hMemDC;
  register HBITMAP hBitmap;
  BITMAP           bm;

  if(hMemDC = CreateCompatibleDC(hDC))
  {
    if(hBitmap = CreateCompatibleBitmap(hDC,1,1))
    {
      if(SelectObject(hMemDC,hBitmap))
      {
        GetObject(hBitmap,sizeof(BITMAP),(LPSTR)&bm);
        BitBlt(hMemDC,0,0,bm.bmWidth,bm.bmHeight,hDC,x,y,SRCCOPY);
        DeleteDC(hMemDC);
        return(hBitmap);
      }
      DeleteObject(hBitmap);
    }
    DeleteDC(hMemDC);
  }
  return(NULL);
}


/*---------------------------------------------------------------------------*\
| CREATE RGB BITMAP                                                           |
|   This routine creates a bitmap representation of the rgb color passed      |
|   to the funtion.                                                           |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC      hDC     - Handle to device context.                              |
|   int      iWidth  - Width in device units.                                 |
|   int      iHeight - Height in device units.                                |
|   COLORREF rgb     - RGB color for bitmap.                                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   HBITMAP - Handle to a Device Dependant Bitmap.                            |
\*---------------------------------------------------------------------------*/
HBITMAP FAR PASCAL CreateRGBBitmap(HDC      hDC,
                                   short    nWidth,
                                   short    nHeight,
                                   COLORREF rgb)
{
  HDC          hMemDC;
  HBITMAP      hBitmap;
  register int nIdx,nIdy;

  if(hMemDC = CreateCompatibleDC(hDC))
  {
    if(hBitmap = CreateCompatibleBitmap(hDC,nWidth,nHeight))
    {
      if(SelectObject(hMemDC,hBitmap))
      {
        for(nIdy=0; nIdy < nHeight; nIdy++)
          for(nIdx=0; nIdx < nWidth; nIdx++)
            SetPixel(hMemDC,nIdx,nIdy,rgb);
        DeleteDC(hMemDC);
        return(hBitmap);
      }
      DeleteObject(hBitmap);
    }
    DeleteDC(hMemDC);
  }
  return(NULL);
}


/*---------------------------------------------------------------------------*\
| OUTPUT DDB TO DEVICE                                                        |
|   This routine outputs the bitmap to the device.  It performs the BitBlt    |
|   function in transfering the bitmap to the DC.                             |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC     hDC     - Handle to device context.                               |
|   int     xStart  - x start.                                                |
|   int     yStart  - y start.                                                |
|   HBITMAP hBitmap - Handle to bitmap.                                       |
|   DWORD   dwROP   - ROP Code for BitBlt.                                    |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if successful.                                                |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL OutputDDBToDevice(HDC              hDC,
                                  register short   xStart,
                                  register short   yStart,
                                  register HBITMAP hBitmap,
                                  DWORD            dwROP)
{
  BITMAP bm;
  HDC    hMemDC;

  if(hMemDC = CreateCompatibleDC(hDC))
  {
    if(SelectObject(hMemDC,hBitmap))
    {
      GetObject(hBitmap,sizeof(BITMAP),(LPSTR)&bm);
      BitBlt(hDC,xStart,yStart,bm.bmWidth,bm.bmHeight,hMemDC,0,0,dwROP);
      DeleteDC(hMemDC);
      return(TRUE);
    }
    DeleteDC(hMemDC);
  }
  return(FALSE);
}


/*---------------------------------------------------------------------------*\
| CONVERT DDB TO DIB                                                          |
|   This routine converts a Device Dependant Bitmap to a Device Indepentant   |
|   bitmap.  The bitmap passed to this function is deleted, and a new HANDLE  |
|   to a DIB is returned.                                                     |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC     hDC        - Handle to a Device Context.                          |
|   HBITMAP hBitmap    - Handle to a DDBitmap.                                |
|   WORD    wDIBType   - DIB Type (either OS/2 or WIN format).                |
|   DWORD   dwCompress - Compression method                                   |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   HDC     hdcTarget  - DC for target device (Printer or metafile)           |
|                                                                             |
| RETURNS                                                                     |
|   HANDLE - Handle to a Device Independant Bitmap.                           |
\*---------------------------------------------------------------------------*/
HANDLE PASCAL ConvertDDBToDIB(HDC     hDC,
                              HBITMAP hBitmap,
                              WORD    wDIBType,
                              DWORD   dwCompress)
{
  BITMAP             bm;
  BITMAPINFOHEADER   bmi;
  BITMAPCOREHEADER   bmc;
  LPBITMAPINFOHEADER lpbi;
  LPBITMAPCOREHEADER lpbc;
  GLOBALHANDLE       hdib,hrea;
  int                nTblSize;
  DWORD              dwLen;

  /*-----------------------------------------*\
  | Get information on Device-Depend Bitmap.  |
  \*-----------------------------------------*/
  if(!GetObject(hBitmap,sizeof(bm),(LPSTR)&bm))
      return(NULL);

  /*-----------------------------------------*\
  | Determine size of color table needed.     |
  \*-----------------------------------------*/
  switch(bm.bmPlanes*bm.bmBitsPixel)
  {
    case 1:
      nTblSize = (wDIBType == DIB_OS2 ? 2*sizeof(RGBTRIPLE) :
                                        2*sizeof(RGBQUAD));
      break;

    case 4:
      nTblSize = (wDIBType == DIB_OS2 ? 16*sizeof(RGBTRIPLE) :
                                        16*sizeof(RGBQUAD));
      break;

    case 8:
      nTblSize = (wDIBType == DIB_OS2 ? 256*sizeof(RGBTRIPLE) :
                                        256*sizeof(RGBQUAD));
      break;

    default:
      nTblSize = 0;
      break;
  }

  switch(wDIBType)
  {
    case DIB_OS2:
      bmc.bcSize     = sizeof(BITMAPCOREHEADER);
      bmc.bcWidth    = bm.bmWidth;
      bmc.bcHeight   = bm.bmHeight;
      bmc.bcPlanes   = 1;
      bmc.bcBitCount = bm.bmBitsPixel*bm.bmPlanes;

      if(hdib = GlobalAlloc(GHND,(DWORD)(sizeof(BITMAPCOREHEADER)+nTblSize)))
      {
        if(lpbc = (LPBITMAPCOREHEADER)GlobalLock(hdib))
        {
          *lpbc = bmc;
          dwLen = (sizeof(BITMAPCOREHEADER)+nTblSize+(bm.bmWidthBytes*bm.bmHeight*bm.bmPlanes));
          GlobalUnlock(hdib);

          if(hrea = GlobalReAlloc(hdib,dwLen,0))
          {
            hdib = hrea;
            if(lpbc = (LPBITMAPCOREHEADER)GlobalLock(hdib))
            {
              if(GetDIBits(hDC,hBitmap,0,(WORD)bmc.bcHeight,
                          (LPSTR)lpbc+lpbc->bcSize+nTblSize,
                          (LPBITMAPINFO)lpbc,DIB_RGB_COLORS))
              {
                DeleteObject(hBitmap);
                GlobalUnlock(hdib);
                return hdib;
              }
              GlobalUnlock(hdib);
            }
          }
        }
        GlobalFree(hdib);
      }
      break;

    case DIB_WIN:
      bmi.biSize          = sizeof(BITMAPINFOHEADER);
      bmi.biWidth         = bm.bmWidth;
      bmi.biHeight        = bm.bmHeight;
      bmi.biPlanes        = 1;
      bmi.biBitCount      = bm.bmBitsPixel*bm.bmPlanes;
      bmi.biCompression   = dwCompress;
      bmi.biSizeImage     = 0;
      bmi.biXPelsPerMeter = 0;
      bmi.biYPelsPerMeter = 0;
      bmi.biClrUsed       = 0;
      bmi.biClrImportant  = 0;

      if(hdib = GlobalAlloc(GHND,(DWORD)(sizeof(BITMAPINFOHEADER)+nTblSize)))
      {
        if(lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib))
        {
          *lpbi = bmi;
          GetDIBits(hDC,hBitmap,0,(WORD)bmi.biHeight,NULL,(LPBITMAPINFO)lpbi,DIB_RGB_COLORS);
          bmi = *lpbi;
          dwLen = (DWORD)(lpbi->biSize+nTblSize+lpbi->biSizeImage);
          GlobalUnlock(hdib);

          if(hrea = GlobalReAlloc(hdib,dwLen,0))
          {
            hdib = hrea;
            if(lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib))
            {
              if(GetDIBits(hDC,hBitmap,0,(WORD)bmi.biHeight,
                          (LPSTR)lpbi+lpbi->biSize+nTblSize,
                          (LPBITMAPINFO)lpbi,DIB_RGB_COLORS))
              {
                DeleteObject(hBitmap);
                GlobalUnlock(hdib);
                return hdib;
              }
              GlobalUnlock(hdib);
            }
          }
        }
        GlobalFree(hdib);
      }
      break;

    default:
      break;
  }
  return(NULL);
}


/*---------------------------------------------------------------------------*\
| OUTPUT DIB TO DEVICE                                                        |
|   This routine outputs a Device Independant Bitmap (DIB) to the output      |
|   device.                                                                   |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC     hDC   - Handle to a Device Context.                               |
|   short   x     - x coordinate.                                             |
|   short   y     - y coordinate.                                             |
|   HANDLE  hdib  - Handle to a DIBitmap.                                     |
|   DWORD   dwRop - ROP Code (Ternary).                                       |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|   -none-                                                                    |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE if the output was successful.                                 |
\*---------------------------------------------------------------------------*/
BOOL PASCAL OutputDIBToDevice(HDC            hDC,
                              register short x,
                              register short y,
                              HANDLE         hdib,
                              DWORD          dwRop)
{
  LPBITMAPINFOHEADER lpdib;
  register int       nTblSize;

  if(lpdib = (LPVOID)GlobalLock(hdib))
  {
    nTblSize = GetColorTableSize(lpdib);
    SetDIBitsToDevice(hDC,x,y,(WORD)lpdib->biWidth,(WORD)lpdib->biHeight,
                      0,0,0,(WORD)lpdib->biHeight,
                      (LPSTR)lpdib+lpdib->biSize+nTblSize,
                      (LPBITMAPINFO)lpdib,DIB_RGB_COLORS);
    GlobalUnlock(hdib);
    return(TRUE);
  }
  return(FALSE);
}

int PASCAL GetColorTableSize(LPBITMAPINFOHEADER lpdib)
{
  LPBITMAPCOREHEADER lpbc;
  LPBITMAPINFOHEADER lpbi;
  register int       nBitCount,nColorSize,nTblSize;

  lpbi = ((LPBITMAPINFOHEADER)lpdib);
  lpbc = ((LPBITMAPCOREHEADER)lpdib);

  if(lpbi->biSize == sizeof(BITMAPINFOHEADER))
  {
    nBitCount = lpbi->biBitCount;
    nColorSize = sizeof(RGBQUAD);
  }
  else
  {
    nBitCount = lpbc->bcBitCount;
    nColorSize = sizeof(RGBTRIPLE);
  }

  switch(nBitCount)
  {
    case 1:
      nTblSize = 2*nColorSize;
      break;

    case 4:
      nTblSize = 16*nColorSize;
      break;

    case 8:
      nTblSize = 256*nColorSize;
      break;

    default:
      nTblSize = 0;
      break;
  }

  return(nTblSize);
}

GLOBALHANDLE PASCAL GetMemoryBitmap(register HBITMAP hBitmap)
{
  GLOBALHANDLE    hMem;
  register LPSTR  pMem;
  BITMAP          bm;
  DWORD           dwLength;

  /*----------------------------------------*\
  | Determine size of memory to hold bitmap. |
  \*----------------------------------------*/
  if(!GetObject(hBitmap,sizeof(BITMAP),(LPSTR)&bm))
    return(NULL);

  dwLength = (DWORD)(bm.bmHeight*(bm.bmWidthBytes*(BYTE)bm.bmPlanes));

  /*----------------------------------------*\
  |                                          |
  \*----------------------------------------*/
  if(hMem = GlobalAlloc(GHND,dwLength))
  {
    if(pMem = GlobalLock(hMem))
    {
      GetBitmapBits(hBitmap,dwLength,pMem);
      GlobalUnlock(hMem);
      return(hMem);
    }
    GlobalFree(hMem);
  }
  return(NULL);
}

BOOL PASCAL CompareBitmaps(HBITMAP hDst,
                           HBITMAP hSrc)
{
  BITMAP         bm;
  GLOBALHANDLE   hDstMem,hSrcMem;
  register LPSTR lpDstMem,lpSrcMem;
  DWORD          dwDst;
//   DWORD          dwSrc;
  BYTE           bMask;

  /*-----------------------------------------*\
  | Calculate the size of the memory for test.|
  \*-----------------------------------------*/
  GetObject(hDst,sizeof(BITMAP),(LPSTR)&bm);
//  dwDst = (DWORD)(bm.bmHeight*(bm.bmWidthBytes*(BYTE)bm.bmPlanes));

  GetObject(hSrc,sizeof(BITMAP),(LPSTR)&bm);
//  dwSrc = (DWORD)(bm.bmHeight*(bm.bmWidthBytes*(BYTE)bm.bmPlanes));

//  if(dwDst ^ dwSrc)
//    return(FALSE);

  hDstMem = GetMemoryBitmap(hDst);
  hSrcMem = GetMemoryBitmap(hSrc);
  lpDstMem = GlobalLock(hDstMem);
  lpSrcMem = GlobalLock(hSrcMem);

  switch(bm.bmBitsPixel)
  {
    case 1:
      bMask = 0x80;
      break;

    case 2:
      bMask = 0xC0;
      break;

    case 3:
      bMask = 0xE0;
      break;

    case 4:
      bMask = 0xF0;
      break;

    default:
      bMask = 0x00;
      break;
  }

dwDst = bm.bmWidth;
  while(dwDst--)
  {
    if((*lpDstMem++ & bMask) ^ (*lpSrcMem++ & bMask))
    {
      GlobalUnlock(hDstMem);
      GlobalUnlock(hSrcMem);
      GlobalFree(hDstMem);
      GlobalFree(hSrcMem);
      return(FALSE);
    }
  }

  GlobalUnlock(hDstMem);
  GlobalUnlock(hSrcMem);
  GlobalFree(hDstMem);
  GlobalFree(hSrcMem);
  return(TRUE);
}
