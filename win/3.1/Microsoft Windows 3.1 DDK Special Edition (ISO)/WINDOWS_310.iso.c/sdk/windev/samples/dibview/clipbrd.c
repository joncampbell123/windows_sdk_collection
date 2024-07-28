/*************************************************************************

      File:  CLIPBRD.C

   Purpose:  Contains routines related to cutting/pasting bitmaps to/from
             the clipboard.

 Functions:  CopyHandle
             CropBitmap
             RenderFormat
             HandleCopyClipboard
             HandlePasteClipboard

  Comments:  

   History:   Date      Reason
             6/ 1/91     Created

*************************************************************************/

#include <windows.h>
#include "child.h"
#include "clipbrd.h"
#include "dib.h"
#include "errors.h"
#include "frame.h"
#include "palette.h"

RECT  grcClip     = {0,0,0,0};    // Clipboard rectangle at time of copy.



//---------------------------------------------------------------------
//
// Function:   CopyHandle
//
// Purpose:    Makes a copy of the given global memory block.  Returns
//             a handle to the new memory block (NULL on error).
//
//             Routine stolen verbatim out of ShowDIB.
//
// Parms:      h == Handle to global memory to duplicate.
//
// Returns:    Handle to new global memory block.
//
// History:   Date      Reason
//             ???      Created
//             
//---------------------------------------------------------------------

HANDLE CopyHandle (HANDLE h)
{
   BYTE huge *lpCopy;
   BYTE huge *lp;
   HANDLE     hCopy;
   DWORD      dwLen;

   if (!h)
      return NULL;

   dwLen = GlobalSize (h);

   if (hCopy = GlobalAlloc (GHND, dwLen)) 
      {
      lpCopy = (BYTE huge *)GlobalLock (hCopy);
      lp     = (BYTE huge *)GlobalLock (h);

      while (dwLen--) 
         *lpCopy++ = *lp++;

      GlobalUnlock (hCopy);
      GlobalUnlock (h);
      }

   return hCopy;
}





//---------------------------------------------------------------------
//
// Function:   CropBitmap
//
// Purpose:    Crops a bitmap to a new size specified by the lpRect
//             parameter.  The lpptSize parameter is used to determine
//             how much to stretch/compress the bitmap.  Returns a
//             handle to a new bitmap.  If lpRect is empty, copies the
//             bitmap to a new one.
//
//             Stolen almost verbatim out of ShowDIB.
//
// Parms:      hbm      == Handle to device dependent bitmap to crop.
//             hPal     == Palette to use in cropping (NULL for default pal.)
//             lpRect   == New bitmap's size (size we're cropping to).
//             lpptSize == A scaling factor scale by the proportion:
//                              Bitmap Width / lpptSize->x horizontally,
//                              Bitmap Height / lpptSize->y horizontally.
//                           Note that if lpptSize is set to the bitmap's
//                           dimensions, no scaling occurs.
//             
//
// History:   Date      Reason
//            6/15/91   Stolen from ShowDIB
//             
//---------------------------------------------------------------------

HBITMAP CropBitmap (HBITMAP hbm, 
                   HPALETTE hPal, 
                     LPRECT lpRect, 
                    LPPOINT lpptSize)
{
   HDC      hMemDCsrc;
   HDC      hMemDCdst;
   HBITMAP  hNewBm = NULL;
   BITMAP   bm;
   int      dxDst,dyDst, dxSrc, dySrc;
   double   cxScale, cyScale;
   HPALETTE hOldPal1 = NULL;
   HPALETTE hOldPal2 = NULL;

   if (!hbm)
      return NULL;

   GetObject (hbm, sizeof(BITMAP), (LPSTR)&bm);

   hMemDCsrc = CreateCompatibleDC (NULL);
   hMemDCdst = CreateCompatibleDC (NULL);

   if (hPal)
      {
      hOldPal1 = SelectPalette (hMemDCsrc, hPal, FALSE);
      hOldPal2 = SelectPalette (hMemDCdst, hPal, FALSE);
      RealizePalette (hMemDCdst);
      }

   dxDst     = lpRect->right  - lpRect->left;
   dyDst     = lpRect->bottom - lpRect->top;
   cxScale   = (double) bm.bmWidth  / lpptSize->x;
   cyScale   = (double) bm.bmHeight / lpptSize->y;
   dxSrc     = (int) ((lpRect->right - lpRect->left) * cxScale);
   dySrc     = (int) ((lpRect->bottom - lpRect->top) * cyScale);

   if (dxDst == 0 || dyDst == 0)
      {
      dxDst = bm.bmWidth;
      dyDst = bm.bmHeight;
      }

   if (dxSrc == 0)
      dxSrc = 1;

   if (dySrc == 0)
      dySrc = 1;

   hNewBm = CreateBitmap (dxDst, dyDst, bm.bmPlanes, bm.bmBitsPixel, NULL);

   if (hNewBm)
      {
      HBITMAP hOldBitmap1, hOldBitmap2;

      hOldBitmap1 = SelectObject (hMemDCsrc, hbm);
      hOldBitmap2 = SelectObject (hMemDCdst, hNewBm);

      StretchBlt (hMemDCdst,
                  0,
                  0,
                  dxDst,
                  dyDst,
                  hMemDCsrc,
                  (int) (lpRect->left * cxScale),
                  (int) (lpRect->top  * cyScale),
                  dxSrc,
                  dySrc,
                  SRCCOPY);

      SelectObject (hMemDCsrc, hOldBitmap1);
      SelectObject (hMemDCdst, hOldBitmap2);
      }

   if (hOldPal1)
      SelectPalette (hMemDCsrc, hOldPal1, FALSE);

   if (hOldPal2)
      SelectPalette (hMemDCdst, hOldPal1, FALSE);

   DeleteDC (hMemDCsrc);
   DeleteDC (hMemDCdst);

   return hNewBm;
}



//---------------------------------------------------------------------
//
// Function:   RenderFormat
//
// Purpose:    Renders an object for the clipboard.  The format is
//             specified in the "cf" variable (either CF_BITMAP,
//             CF_DIB, or CF_PALETTE).
//
//             Stolen almost verbatim out of ShowDIB.
//
// Parms:      hWndClip == Window clipboard belongs to, and where our
//                         image is stored).
//             cf       == Format to render (CF_BITMAP, CF_DIB, CF_PALETTE)
//             ptDIBSize== Size of the DIB in the given window.
//
// History:   Date      Reason
//             ???      Created
//             
//---------------------------------------------------------------------

HANDLE RenderFormat (HWND hWndClip, int cf, POINT ptDIBSize)
{
   HANDLE    h = NULL;
   HBITMAP   hBitmap;
   HANDLE    hDIB;
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;
   HPALETTE  hPalette;        // Handle to the bitmap's palette.


   if (!hWndClip)
      return NULL;

   hDIBInfo = GetWindowWord (hWndClip, WW_DIB_HINFO);

   if (!hDIBInfo)
      return NULL;

   lpDIBInfo    = (LPDIBINFO) GlobalLock (hDIBInfo);
   hDIB         = lpDIBInfo->hDIB;
   hPalette     = lpDIBInfo->hPal;
   hBitmap      = lpDIBInfo->hBitmap;
   GlobalUnlock (hDIBInfo);

   switch (cf)
      {
      case CF_BITMAP:
         h = CropBitmap (hBitmap, hPalette, &grcClip, &ptDIBSize);
         break;


      case CF_DIB:
         {
         HBITMAP hbm;

            // NOTE:  For simplicity, we use the display device to crop
            //        the bitmap.  This means that we may lose color
            //        precision (if the display device has less color
            //        precision than the DIB).  This isn't usually a
            //        problem, as users shouldn't really be editting
            //        images on devices that can't display them.
            
         hbm = RenderFormat (hWndClip, CF_BITMAP, ptDIBSize);

         if (hbm)
            {
            h = BitmapToDIB (hbm, hPalette);
            DeleteObject (hbm);
            }
         break;
         }


      case CF_PALETTE:
         if (hPalette)
            h = CopyPaletteChangingFlags (hPalette, 0);
         break;
   }

   return h;
}




//---------------------------------------------------------------------
//
// Function:   HandleCopyClipboard
//
// Purpose:    User wants to copy the current DIB to the clipboard.
//             Tell the clipboard we can render a DIB, DDB, and a
//             palette (defer rendering until we get a WM_RENDERFORMAT
//             in our MDI child window procedure in CHILD.C).
//
// Parms:      None
//             
//
// History:   Date      Reason
//            6/1/91    Created
//           11/4/91    Init grcClip to full DIB size if
//                      it is currently empty.
//             
//---------------------------------------------------------------------

void HandleCopyClipboard (void)
{
   HWND    hDIBWnd;

   hDIBWnd = GetCurrentMDIWnd ();

   if (!hDIBWnd)
      {
      DIBError (ERR_NOCLIPWINDOW);
      return;
      }

      // Clean clipboard of contents, and tell it we can render
      //  a DIB, a DDB, and/or a palette.

   if (OpenClipboard (hDIBWnd))
      {
      EmptyClipboard ();
      SetClipboardData (CF_DIB ,NULL);
      SetClipboardData (CF_BITMAP  ,NULL);
      SetClipboardData (CF_PALETTE ,NULL);
      CloseClipboard ();


         // Set our globals to tell our app which child window
         //  owns the clipboard, and the clipping rectangle at
         //  the time of the copy.  If the clipping rectangle is
         //  empty, then use the entire DIB window.

      hWndClip   = hDIBWnd;
      grcClip    = GetCurrentClipRect (hWndClip);
      ptClipSize = GetCurrentDIBSize (hWndClip);

      if (IsRectEmpty (&grcClip))
         {
         grcClip.left   = 0;
         grcClip.top    = 0;
         grcClip.right  = ptClipSize.x;
         grcClip.bottom = ptClipSize.y;
         }
      }
   else
      DIBError (ERR_CLIPBUSY);
}



//---------------------------------------------------------------------
//
// Function:   HandlePasteClipboard
//
// Purpose:    User wants to paste the clipboard's contents to our
//             app.  Open a new DIB window with the bitmap in the
//             clipboard.
//
// Parms:      None
//             
// History:   Date      Reason
//            6/1/91    Created
//             
//---------------------------------------------------------------------

void HandlePasteClipboard (void)
{
   HANDLE     hDIB;
   HBITMAP    hBitmap;
   HPALETTE   hPal;
   char       szTitle [20];
   static int nPasteNum = 1;     // For window title

      // Open up the clipboard.  This routine assumes our app has
      //  the focus (which it should, as the user just picked the
      //  paste operation off the menu, and Windows is a non-preemptive
      //  system.  First we try for a DIB; if that's not available go
      //  for a bitmap (and a palette if one can be had).  Whatever
      //  format's available, we have to copy immediately (since the
      //  handle returned by GetClipboardData() belongs to the clipboard.
      //  Finally, go create the new MDI child window.

   if (OpenClipboard (GetFocus ()))
      {
      if (IsClipboardFormatAvailable (CF_DIB))
         {
         hDIB = CopyHandle (GetClipboardData (CF_DIB));
         }

      else if (IsClipboardFormatAvailable (CF_BITMAP))
         {
         hBitmap = GetClipboardData (CF_BITMAP);

         if (IsClipboardFormatAvailable (CF_PALETTE))
            hPal = GetClipboardData (hPal);
         else
            hPal = GetStockObject (DEFAULT_PALETTE);

         hDIB = BitmapToDIB (hBitmap, hPal);
         }

      else
         DIBError (ERR_NOCLIPFORMATS);

      CloseClipboard ();


         // The window title is of the form: "Clipboard1".  The
         //  number in the title is changed for each paste operation.

      wsprintf (szTitle, "Paste%d", nPasteNum++);


         // Perform the actual window opening.

      OpenDIBWindow (hDIB, szTitle);
      }
   else
      DIBError (ERR_CLIPBUSY);
}


