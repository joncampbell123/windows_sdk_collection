/*************************************************************************

      File:  PALETTE.C

   Purpose:  Contains routines to display a palette in a popup window.
             Allows the user to scroll around the palette and dump info
             on particular colors in the palette.  Has much the same
             look as the Clipboard's palette dump.

 Functions:  PaletteWndProc
             HighlightSquare   
             UnHighlightSquare
             PalRowsAndColumns 
             SetPaletteWindowsPal
             PalEntriesOnDevice
             GetSystemPalette
             ColorsInPalette
             MyAnimatePalette
             CopyPaletteChangingFlags

  Comments:  The routines in this module are provided mostly for
             debugging purposes.  There are definite improvements
             that can be made.  For example, the scroll bars now
             let you scroll way beyond the existing colors in a
             palette.  They should probably be limited to the number
             of rows to be displayed in the palette window.

   History:   Date      Reason
             6/ 1/91     Created
            11/15/91     Use LoadString instead of a hardcoded
                         array of strings.  This frees up some
                         DS, and is better for localizing for
                         international markets.
             1/28/92     Added WM_QUERYNEWPALETTE message,
                         and always select palette as
                         a background palette in WM_PAINT.

*************************************************************************/


#include <windows.h>
#include <assert.h>
#include "dibview.h"
#include "errors.h"
#include "math.h"
#include "palette.h"



   // Useful magic numbers.

#define PAL_SIZE_DEF       PALSIZE_MEDIUM    // Default palette square size.
#define ID_INFO_CHILD      1                 // Palette information window ID


int nEntriesPerInch [4] = {15,               // Tiny mode  squares/inch
                           10,               // Small mode squares/inch
                            6,               // Medium mode squares/inch
                            4};              // Large mode squares/inch




   // Local function prototypes.

int  PalEntriesOnDevice  (HDC hDC);

void PalRowsAndColumns  (HWND hWnd, 
                          int nSquareSize, 
                        LPINT lpnCols, 
                        LPINT lpnRows,
                        LPINT lpcxSquare,
                        LPINT lpcySquare);

void HighlightSquare     (HDC hDC, 
                     HPALETTE hPal,
                         HWND hInfoBar,
                         WORD wEntry, 
                          int cxSquare, 
                          int cySquare, 
                          int nCols,
                          int nColors,
                          int nScroll);

void UnHighlightSquare   (HDC hDC, 
                         WORD wEntry, 
                          int cxSquare, 
                          int cySquare,
                          int nCols,
                          int nScroll);






//---------------------------------------------------------------------
//
// Function:   PaletteWndProc
//
// Purpose:    Window procedure for palette child windows.  These
//             windows are responsible for displaying a color palette
//             a-la the Clipboard Viewer.  It also dumps info on each
//             palette color.
//
// Parms:      hWnd    == Handle to this child window.
//             message == Message for window.
//             wParam  == Depends on message.
//             lParam  == Depends on message.
//
// History:   Date      Reason
//             6/01/91  Created
//             1/28/92  Added WM_QUERYNEWPALETTE message,
//                        and always select palette as
//                        a background palette in WM_PAINT.
//             
//---------------------------------------------------------------------

long FAR PASCAL PaletteWndProc (HWND hWnd,
				UINT message,
				WPARAM wParam,
				LPARAM lParam)
{
   switch (message)
      {
      case WM_CREATE:
         {
         HWND      hInfoBar;
         HANDLE    hPalInfo;

         hInfoBar = CreateWindow ("static",       // Class
                                  NULL,           // Name
					                   WS_CHILD |      // Styles
                                   WS_VISIBLE | 
                                   SS_CENTER,
					                   0,              // x (set in WM_SIZE)
					                   0,              // y (set in WM_SIZE)
					                   100,            // Width (set in WM_SIZE)
					                   30,             // Height (set in WM_SIZE)
					                   hWnd,           // hParent
					                   ID_INFO_CHILD,  // Child Window ID
					                   hInst,          // Instance
					                   NULL);          // Extra information

         hPalInfo = GlobalAlloc (GHND, sizeof (PALINFO));

         if (hPalInfo)
            {
            LPPALINFO lpPalInfo;

            lpPalInfo              = (LPPALINFO) GlobalLock (hPalInfo);
            lpPalInfo->hPal        = NULL;
            lpPalInfo->wEntries    = 0;
            lpPalInfo->nSquareSize = PAL_SIZE_DEF;
            lpPalInfo->hInfoWnd    = hInfoBar;
            lpPalInfo->nRows       = 0;
            lpPalInfo->nCols       = 0;
            lpPalInfo->cxSquare    = 0;
            lpPalInfo->cySquare    = 0;
            lpPalInfo->wEntry      = 0;
            GlobalUnlock (hPalInfo);
            }

         SetWindowWord (hWnd, WW_PAL_HPALINFO, hPalInfo);

            // Set the palette square size to the default value.
            //  This must be done AFTER the PALINFO structure's
            //  handle is put into the window words.

         SendMessage (hWnd, WM_COMMAND, PAL_SIZE_DEF, 0L);
         break;
         }



         // If the window is re-sized, move the information bar (a static
         //  text control) accordingly.

      case WM_SIZE:
         {
         HANDLE      hPalInfo;
         LPPALINFO   lpPalInfo;
         HWND        hInfoBar;
         HDC         hDC;
         TEXTMETRIC  tm;
         RECT        rect;
         static BOOL bInSize = FALSE;

            // We implement a semaphore here to avoid an
            //  infinte loop.  Scroll bars are set up in
            //  PalRowsAndColumns().  This, in turn, can
            //  effect the size of the window's client area.
            //  Thus, sending the window another WM_SIZE
            //  message.  We ignore this 2nd WM_SIZE message
            //  if we're already processing one.

         if (bInSize)
            break;

         bInSize = TRUE;


            // Get a handle to the info bar window.

         hPalInfo = GetWindowWord (hWnd, WW_PAL_HPALINFO);
         if (!hPalInfo)
            {
            bInSize = FALSE;
            break;
            }

         lpPalInfo = (LPPALINFO) GlobalLock (hPalInfo);
         hInfoBar  = lpPalInfo->hInfoWnd;

         PalRowsAndColumns (hWnd, lpPalInfo->nSquareSize,
                            &lpPalInfo->nCols, &lpPalInfo->nRows,
                            &lpPalInfo->cxSquare, &lpPalInfo->cySquare);

         GlobalUnlock (hPalInfo);

         if (!hInfoBar)
            {
            bInSize = FALSE;
            break;
            }


            // The size of the info bar is dependent on the size of the
            //  system font.

         hDC = GetDC (NULL);
         GetTextMetrics (hDC, &tm);
         ReleaseDC (NULL, hDC);

         GetClientRect (hWnd, &rect);

            // Now actually move the icon bar.  It should be flush
            //  left with the palette window's client area, and should
            //  be the height of some text.

         SetWindowPos (hInfoBar,
                       NULL,
                       0,
                       rect.bottom - tm.tmHeight,
                       rect.right - rect.left,
                       tm.tmHeight,
                       SWP_NOZORDER | SWP_NOACTIVATE);


            // Clear our semaphore.

         bInSize = FALSE;
         break;
         }



      case WM_PAINT:
         {
         RECT        rect;
         HDC         hDC;
         PAINTSTRUCT ps;
         int         nCols;
         int         nColors, i, nPerInch;
         int         cxSquare,  cySquare;
         int         nScroll;
         HPALETTE    hPal, hOldPal = NULL;
         HANDLE      hPalInfo;
         LPPALINFO   lpPalInfo;
         HWND        hInfoBar;
         WORD        wCurrent;


            // First do some setup.

         GetClientRect (hWnd, &rect);

         hDC       = BeginPaint (hWnd, &ps);
         nScroll   = GetScrollPos (hWnd, SB_VERT);
         hPalInfo  = GetWindowWord (hWnd, WW_PAL_HPALINFO);

         if (!hPalInfo)
            goto ENDPAINT;

         lpPalInfo = (LPPALINFO) GlobalLock (hPalInfo);
         nPerInch  = nEntriesPerInch [lpPalInfo->nSquareSize];
         hPal      = lpPalInfo->hPal;
         nColors   = lpPalInfo->wEntries;
         cxSquare  = lpPalInfo->cxSquare;
         cySquare  = lpPalInfo->cySquare;
         wCurrent  = lpPalInfo->wEntry;
         nCols     = lpPalInfo->nCols;
         hInfoBar  = lpPalInfo->hInfoWnd;
         GlobalUnlock (hPalInfo);

         assert (nPerInch);
         assert (cxSquare);
         assert (cySquare);
         assert (nCols);


            // Change our window origin to reflect the current
            //  scroll bar state.

         SetWindowOrg (hDC, 0, GetScrollPos (hWnd, SB_VERT) * cySquare);


            // Let's paint -- first realize the palette.  Note that
            //  we ALWAYS realize the palette as if it were a background
            //  palette (i.e. the last parm is TRUE).  We do this, since
            //  we will already be the foreground palette if we are
            //  supposed to be (because we handle the WM_QUERYNEWPALETTE
            //  message).

         if (hPal)
            hOldPal = SelectPalette (hDC, hPal, TRUE);
        
         else
            {
            char szErr[20];

            szErr[0] = '\0';
            LoadString (hInst, IDS_PAL_NOPAL, szErr, sizeof (szErr)-1);

            TextOut (hDC, 0, 0, szErr, lstrlen (szErr));
            goto ENDPAINT;
            }

         RealizePalette (hDC);


            // Go through the palette displaying each color
            //  as a rectangle.

         for (i = 0;  i < nColors;  i++)
            {
            HBRUSH hBrush, hOldBrush;

            hBrush = CreateSolidBrush (PALETTEINDEX (i));

            if (hBrush)
               {
               POINT pt;

               hOldBrush = SelectObject (hDC, hBrush);

               pt.x = (i % nCols) * cxSquare;
               pt.y = (i / nCols) * cySquare;

               Rectangle (hDC,
                          pt.x,
                          pt.y,
                          pt.x + cxSquare,
                          pt.y + cySquare);

               SelectObject (hDC, hOldBrush);
               DeleteObject (hBrush);
               }
            }


            // Highlight the currently selected palette square,
            //  and change the info window to reflect this
            //  square.

         HighlightSquare (hDC, 
                          hPal, 
                          hInfoBar, 
                          wCurrent,
                          cxSquare, 
                          cySquare, 
                          nCols,
                          nColors,
                          nScroll);


            // Clean up.

         if (hOldPal)
            SelectPalette (hDC, hOldPal, FALSE);


ENDPAINT:

         EndPaint (hWnd, &ps);
         break;
         }



         // If the user hits the left mouse button, change the
         //  selected palette entry.

      case WM_LBUTTONDOWN:
         {
         HDC       hDC;
         HANDLE    hPalInfo;
         LPPALINFO lpPalInfo;
         int       nRow, nCol;
         int       nScroll;

         hPalInfo = GetWindowWord (hWnd, WW_PAL_HPALINFO);
         if (!hPalInfo)
            break;

         nScroll   = GetScrollPos (hWnd, SB_VERT);
         lpPalInfo = (LPPALINFO) GlobalLock (hPalInfo);
         nRow      = (HIWORD (lParam) ) / lpPalInfo->cySquare;
         nCol      = LOWORD (lParam) / lpPalInfo->cxSquare;
         hDC       = GetDC (hWnd);

         UnHighlightSquare (hDC, 
                            lpPalInfo->wEntry, 
                            lpPalInfo->cxSquare,
                            lpPalInfo->cySquare,
                            lpPalInfo->nCols,
                            nScroll);
                  

            // Determine which entry is the new highlighted entry.
            //  Take into account the scroll bar position.

         lpPalInfo->wEntry = nRow * lpPalInfo->nCols + nCol +
                              lpPalInfo->nCols * nScroll;


            // Don't let the selected palette entry be greater
            //  than the # of palette entries available.

         if (lpPalInfo->wEntry >= lpPalInfo->wEntries)
            lpPalInfo->wEntry = lpPalInfo->wEntries - 1;

         HighlightSquare (hDC, 
                          lpPalInfo->hPal, 
                          lpPalInfo->hInfoWnd, 
                          lpPalInfo->wEntry,
                          lpPalInfo->cxSquare, 
                          lpPalInfo->cySquare, 
                          lpPalInfo->nCols,
                          lpPalInfo->wEntries,
                          nScroll);

         ReleaseDC (hWnd, hDC);
         GlobalUnlock (hPalInfo);
         break;
         }


         // Do that horizontal scroll bar thing.

      case WM_VSCROLL:
         {
         HWND      hBar = HIWORD (lParam);      // HWND of scrollbar
         int       yBar;                        // Where scrollbar is now.
         int       nMin;                        // Minumum scroll bar value.
         int       nMax;                        // Maximum scroll bar value.
         int       dy;                          // How much to move.
         int       cyClient;                    // Width of client area.
         int       cySquare;                    // Height of a palette square.
         RECT      rect;                        // Client area.
         HANDLE    hPalInfo;                    // Handle to PALINFO struct.
         LPPALINFO lpPalInfo;                   // Pointer to PALINFO struct.


         hPalInfo = GetWindowWord (hWnd, WW_PAL_HPALINFO);
         if (!hPalInfo)
            break;

         lpPalInfo = (LPPALINFO) GlobalLock (hPalInfo);
         cySquare  = lpPalInfo->cySquare;
         GlobalUnlock (hPalInfo);

         if (!hBar)
            hBar = hWnd;

         GetClientRect (hWnd, &rect);
         GetScrollRange (hBar, SB_VERT, &nMin, &nMax);

         cyClient = rect.bottom - rect.top;
         yBar     = GetScrollPos (hBar, SB_VERT);

         switch (wParam)
            {
            case SB_LINEDOWN:             // One line right.
               dy = 1;
               break;

            case SB_LINEUP:               // One line left.
               dy = -1;
               break;

            case SB_PAGEDOWN:             // One page right.
               dy = cyClient / cySquare;
               break;

            case SB_PAGEUP:               // One page left.
               dy = -cyClient / cySquare;
               break;

            case SB_THUMBPOSITION:        // Absolute position.
               dy = LOWORD (lParam) - yBar;
               break;

            default:                      // No change.
               dy = 0;
               break;
            }

         if (dy)
            {
            yBar += dy;

            if (yBar < nMin)
               {
               dy  -= yBar - nMin;
               yBar = nMin;
               }

            if (yBar > nMax)
               {
               dy  -= yBar - nMax;
               yBar = nMax;
               }

            if (dy)
               {
               SetScrollPos (hBar, SB_VERT, yBar, TRUE);
               InvalidateRect (hWnd, NULL, TRUE);
               UpdateWindow (hWnd);
               }
            }
         break;
         }



         // If the system palette changes, we need to re-draw, re-mapping
         //  our palette colors.  Otherwise they will be completely
         //  wrong (and if any palette animation is going on, they
         //  will start animating)!!

      case WM_PALETTECHANGED:
         if (hWnd != (HWND) wParam)
            InvalidateRect (hWnd, NULL, FALSE);
         break;



         // If we get the focus on this window, we want to insure
         //  that we have the foreground palette.

      case WM_QUERYNEWPALETTE:
         {
         HANDLE    hPalInfo;
         LPPALINFO lpPalInfo;
         HPALETTE  hPal, hOldPal;
         HDC       hDC;

         hPalInfo  = GetWindowWord (hWnd, WW_PAL_HPALINFO);
         if (!hPalInfo)
            break;
         lpPalInfo = (LPPALINFO) GlobalLock (hPalInfo);
         hPal      = lpPalInfo->hPal;
         GlobalUnlock (hPalInfo);

         if (hPal)
            {
            hDC       = GetDC (hWnd);
            hOldPal   = SelectPalette (hDC, hPal, FALSE);

            RealizePalette (hDC);
            SelectPalette (hDC, hOldPal, FALSE);
            ReleaseDC (hWnd, hDC);

            InvalidateRect (hWnd, NULL, FALSE);
            return TRUE;
            }
         break;
         }


         // Window's going away, destroy the palette, and the PALINFO
         //  structure.

      case WM_DESTROY:
         {
         HANDLE    hPalInfo;
         LPPALINFO lpPalInfo;

         hPalInfo = GetWindowWord (hWnd, WW_PAL_HPALINFO);

         if (!hPalInfo)
            break;

         lpPalInfo = (LPPALINFO) GlobalLock (hPalInfo);

         if (lpPalInfo->hPal)
            DeleteObject (lpPalInfo->hPal);

         GlobalUnlock (hPalInfo);
         GlobalFree (hPalInfo);
         SetWindowWord (hWnd, WW_PAL_HPALINFO, NULL);
         break;
         }



         // If we're getting the focus, our palette may not be the
         //  currently selected palette.  Therefore, force a repaint.

      case WM_SETFOCUS:
	 InvalidateRect (hWnd, NULL, TRUE);
         break;



         // Something was picked off a menu.  If the user is asking
         //  to change the way we represent the palette, uncheck the
         //  old way, check the new way, and force a repaint.  If not,
         //  pass the value on to DefWindowProc().

      case WM_COMMAND:
         {
         if ((wParam >= IDM_PAL_TINY) && (wParam <= IDM_PAL_LARGE))
            {
            HANDLE    hPalInfo;
            LPPALINFO lpPalInfo;
            HMENU     hMenu;
            WORD      wOldItem;

            hPalInfo = GetWindowWord (hWnd, WW_PAL_HPALINFO);
            hMenu    = GetMenu (hWnd);

            if (!hPalInfo)
               break;

            lpPalInfo = (LPPALINFO) GlobalLock (hPalInfo);
            wOldItem  = lpPalInfo->nSquareSize;

            CheckMenuItem (hMenu, 
                           wOldItem + IDM_PAL_TINY, 
                           MF_BYCOMMAND | MF_UNCHECKED);

            CheckMenuItem (hMenu, 
                           wParam, 
                           MF_BYCOMMAND | MF_CHECKED);


            lpPalInfo->nSquareSize = wParam - IDM_PAL_TINY;
            PalRowsAndColumns (hWnd, lpPalInfo->nSquareSize,
                               &lpPalInfo->nCols, &lpPalInfo->nRows,
                               &lpPalInfo->cxSquare, &lpPalInfo->cySquare);

            GlobalUnlock (hPalInfo);

            InvalidateRect (hWnd, NULL, TRUE);
            }
         else
            return DefWindowProc (hWnd, message, wParam, lParam);

         break;
         }



      default:
         return DefWindowProc (hWnd, message, wParam, lParam);
      }

    return (NULL);
}




//---------------------------------------------------------------------
//
// Function:   HighlightSquare
//
// Purpose:    Highlight the currently selected palette entry, and
//             change the info bar to reflect it.
//
// Parms:      hDC      == DC where we want to highlight a pal. square.
//             hPal     == Handle to the palette we're displaying info on.
//             hInfoBar == Handle to the information bar window.
//             wEntry   == Entry to highlight.
//             cxSquare == Width a a palette square.
//             cySquare == Height of a palette square.
//             nCols    == # of columns currently displayed in window.
//             nColors  == # of colors in the palette.
//             nScroll  == # of rows the window has scrolled.
//
// History:   Date      Reason
//             6/01/91  Created
//            11/18/91  Added call to LoadString instead of
//                      using a hardcoded array of strings.
//             
//---------------------------------------------------------------------

void HighlightSquare (HDC hDC, 
                 HPALETTE hPal,
                     HWND hInfoBar,
                     WORD wEntry, 
                      int cxSquare, 
                      int cySquare, 
                      int nCols,
                      int nColors,
                      int nScroll)
{
   RECT         rect;
   HBRUSH       hBrush;
   PALETTEENTRY pe;
   char         szStr [70], szFlag [20], szFormat [70];
   DWORD        dwOrg;

   rect.left   = (wEntry % nCols) * cxSquare;
   rect.top    = (wEntry / nCols) * cySquare;
   rect.right  = rect.left + cxSquare;
   rect.bottom = rect.top  + cySquare;
   hBrush      = CreateHatchBrush (HS_BDIAGONAL, 
                                    GetSysColor (COLOR_HIGHLIGHT));

   dwOrg = SetWindowOrg (hDC, 0, nScroll * cySquare);
   FrameRect (hDC, &rect, hBrush);
   dwOrg = SetWindowOrg (hDC, LOWORD (dwOrg), HIWORD (dwOrg));

   GetPaletteEntries (hPal, wEntry, 1, &pe);

      // If the palette entry we just got is just an index into the
      //  system palette, get it's RGB value.

   if (pe.peFlags == PC_EXPLICIT)
      {
      COLORREF cref;
      HPALETTE hOldPal;

      cref       = PALETTEINDEX ((WORD) pe.peRed + (pe.peGreen << 4));
      hOldPal    = SelectPalette (hDC, hPal, FALSE);
      cref       = GetNearestColor (hDC, cref);
      pe.peRed   = (BYTE)  (cref & 0x0000FF);
      pe.peGreen = (BYTE) ((cref & 0x00FF00) >> 8);
      pe.peBlue  = (BYTE) ((cref & 0xFF0000) >> 16);  

      SelectPalette (hDC, hOldPal, FALSE);
      }


      // Decode the palette flag by loading the appropriate description
      //  string from the string table.  Note that we also keep the
      //  format string for wsprintf() in the string table.  Don't
      //  change wsprintf() below without also examining IDS_PAL_DISPRGB
      //  in DIBVIEW.RC's string table!

   szFlag[0] = '\0';
   LoadString (hInst, IDS_PAL_RGB + pe.peFlags, szFlag, sizeof (szFlag)-1);

   if (LoadString (hInst, IDS_PAL_DISPRGB, szFormat, sizeof (szFormat-1)))
      wsprintf (szStr, szFormat,
                  pe.peRed, pe.peGreen, pe.peBlue,
                  wEntry + 1, nColors, 
                  (LPSTR) szFlag);


   SetWindowText (hInfoBar, szStr);

   DeleteObject (hBrush);
}





//---------------------------------------------------------------------
//
// Function:   UnHighlightSquare
//
// Purpose:    Un-Highlight a palette entry.
//
// Parms:      hDC      == DC where we want to unhighlight a pal. square.
//             wEntry   == Entry to highlight.
//             cxSquare == Width a a palette square.
//             cySquare == Height of a palette square.
//             nCols    == # of columns currently displayed in window.
//             nScroll  == # of rows the window has scrolled.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

void UnHighlightSquare (HDC hDC, 
                       WORD wEntry, 
                        int cxSquare, 
                        int cySquare,
                        int nCols,
                        int nScroll)
{
   RECT  rect;
   DWORD dwOrg;

   rect.left   = (wEntry % nCols) * cxSquare;
   rect.top    = (wEntry / nCols) * cySquare;
   rect.right  = rect.left + cxSquare;
   rect.bottom = rect.top  + cySquare;

   dwOrg = SetWindowOrg (hDC, 0, nScroll * cySquare);
   FrameRect (hDC, &rect, GetStockObject (BLACK_BRUSH));
   SetWindowOrg (hDC, LOWORD (dwOrg), HIWORD (dwOrg));
}





//---------------------------------------------------------------------
//
// Function:   PalRowsAndColumns
//
// Purpose:    Given a square size, determine the # of Rows/Columns
//             that will fit in the client area of a palette window.
//             Also, set the # of pixels per square for height/width.
//             Finally, set up the scroll bar.
//
// Parms:      hWnd       == Window we're doing this for.
//             nSquareSize== Size to make squares (offset into 
//                            nEntriesPerInch array).
//             lpnCols    == far ptr to # Cols to display, set dependent
//                            on nSquareSize and dimeinsions of window.
//             lpnRows    == Ditto -- for # of rows to display.
//             lpcxSquare == Ditto -- width of a palette square.
//             lpcySquare == Ditto -- height of a palette square.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

void PalRowsAndColumns  (HWND hWnd, 
                          int nSquareSize, 
                        LPINT lpnCols, 
                        LPINT lpnRows,
                        LPINT lpcxSquare,
                        LPINT lpcySquare)
{
   HDC  hDC;
   int  cxInch, cyInch;
   int  nPerInch;
   RECT rect;

   hDC    = GetDC (NULL);
   cxInch = GetDeviceCaps (hDC, LOGPIXELSX);
   cyInch = GetDeviceCaps (hDC, LOGPIXELSY);
   ReleaseDC (NULL, hDC);

   GetClientRect (hWnd, &rect);

   nPerInch    = nEntriesPerInch [nSquareSize];
   *lpcxSquare = cxInch / nPerInch;
   *lpcySquare = cyInch / nPerInch;

   if (*lpcxSquare == 0)
      *lpcxSquare = 1;

   if (*lpcySquare == 0)
      *lpcySquare = 1;


      // Translate palette squares per inch into # of columns,
      //  and pixels per square.  Insure that we have at least
      //  one column, and 1 pix per side on our squares.

   *lpnCols = (int) ((((long) rect.right - rect.left) * nPerInch) / cxInch);
   *lpnRows = (int) ((((long) rect.bottom - rect.top) * nPerInch) / cyInch);

   if (!*lpnCols)
      *lpnCols = 1;

   if (!*lpnRows)
      *lpnRows = 1;
}





//---------------------------------------------------------------------
//
// Function:   SetPaletteWindowsPal
//
// Purpose:    Set a palette Window's hPal in its PALINFO structure.
//             This sets the palette that will be displayed in the
//             given window.  Also sets up the other structure members
//             of the PALINFO structure.
//
// Parms:      hWnd == Window we're going to display palette in.
//             hPal == Palette to display in the window.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

void SetPaletteWindowsPal (HWND hWnd, HPALETTE hPal)
{
   HANDLE    hPalInfo;
   LPPALINFO lpPalInfo;

   if (!hPal)
      return;

   hPalInfo = GetWindowWord (hWnd, WW_PAL_HPALINFO);

   if (hPalInfo)
      {
      lpPalInfo           = (LPPALINFO) GlobalLock (hPalInfo);
      lpPalInfo->hPal     = hPal;
      lpPalInfo->wEntries = ColorsInPalette (hPal);

      GlobalUnlock (hPalInfo);
      }
}



//---------------------------------------------------------------------
//
// Function:   PalEntriesOnDevice
//
// Purpose:    Returns the number of colors a device supports.
//
// Parms:      hDC == DC for the device we want # of colors for.
//
// Returns:    # of colors that the given device can represent.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

int PalEntriesOnDevice (HDC hDC)
{
   int nColors;

      // Find out the number of palette entries on this
      //  defice.

   nColors = GetDeviceCaps (hDC, SIZEPALETTE);


      // For non-palette devices, we'll use the # of system
      //  colors for our palette size.

   if (!nColors)
      nColors = GetDeviceCaps (hDC, NUMCOLORS);

   assert (nColors);

   return nColors;
}





//---------------------------------------------------------------------
//
// Function:   GetSystemPalette
//
// Purpose:    This routine returns a handle to a palette which represents
//             the system palette (each entry is an offset into the system
//             palette instead of an RGB with a flag of PC_EXPLICIT).
//             Useful for dumping the system palette.
//
// Parms:      None
//
// Returns:    Handle to a palette consisting of the system palette
//             colors.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

HPALETTE GetSystemPalette (void)
{
   HDC           hDC;
   HPALETTE      hPal = NULL;
   HANDLE        hLogPal;
   LPLOGPALETTE  lpLogPal;
   int           i, nColors;


      // Find out how many palette entries we want.

   hDC = GetDC (NULL);
   if (!hDC)
      {
      DIBError (ERR_GETDC);
      return NULL;
      }

   nColors = PalEntriesOnDevice (hDC);
   ReleaseDC (NULL, hDC);


      // Allocate room for the palette and lock it.
      
   hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) + 
                           nColors * sizeof (PALETTEENTRY));

   if (!hLogPal)
      {
      DIBError (ERR_CREATEPAL);
      return NULL;
      }

   lpLogPal = (LPLOGPALETTE) GlobalLock (hLogPal);

   lpLogPal->palVersion    = PALVERSION;
   lpLogPal->palNumEntries = nColors;

   for (i = 0;  i < nColors;  i++)
      {
      lpLogPal->palPalEntry[i].peBlue  = 0;
      *((LPWORD) (&lpLogPal->palPalEntry[i].peRed)) = i;
      lpLogPal->palPalEntry[i].peFlags = PC_EXPLICIT;
      }



      // Go ahead and create the palette.  Once it's created,
      //  we no longer need the LOGPALETTE, so free it.

   hPal = CreatePalette (lpLogPal);

   GlobalUnlock (hLogPal);
   GlobalFree (hLogPal);

   return hPal;
}





//---------------------------------------------------------------------
//
// Function:   ColorsInPalette
//
// Purpose:    Given a handle to a palette, returns the # of colors
//             in that palette.
//
// Parms:      hPal == Handle to palette we want info on.
//
// Returns:    # of colors in the palette.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

int ColorsInPalette (HPALETTE hPal)
{
   int nColors;

   if (!hPal)
      return 0;

   GetObject (hPal, sizeof (nColors), (LPSTR) &nColors);

   return nColors;
}




//---------------------------------------------------------------------
//
// Function:   MyAnimatePalette
//
// Purpose:    This routine animates the given palette.  It does this
//             by moving all the palette entries down one in the palette,
//             and putting the first entry at the end of the palette (we
//             could do something different here, like run various shades
//             of a certain color through the palette, or run random colors
//             through the palette).
//
//             Not really that useful -- it just creates a pretty funky
//             "psychadelic" effect.  It does show how to do palette
//             animation, though.
//
//             This routine is called by CHILD.C.
//
// Parms:      hWnd == Window we're animating.
//             hPal == Palette we're animating.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

void MyAnimatePalette (HWND hWnd, HPALETTE hPal)
{
   HDC            hDC;
   HANDLE         hPalEntries;
   LPPALETTEENTRY lpPalEntries;
   WORD           wEntries, i;
   HPALETTE       hOldPal;
   PALETTEENTRY   pe;

   if (!hPal)
      return;

   wEntries = ColorsInPalette (hPal);

   if (!wEntries)
      return;

   hPalEntries = GlobalAlloc (GHND, sizeof (PALETTEENTRY) * wEntries);

   if (!hPalEntries)
      return;

   lpPalEntries = (LPPALETTEENTRY) GlobalLock (hPalEntries);

   GetPaletteEntries (hPal, 0, wEntries, lpPalEntries);

   pe = lpPalEntries[0];

   for (i = 0;  i < wEntries - 1;  i++)
      lpPalEntries[i] = lpPalEntries[i+1];

   lpPalEntries[wEntries - 1] = pe;

   hDC     = GetDC (hWnd);
   hOldPal = SelectPalette (hDC, hPal, FALSE);

   AnimatePalette (hPal, 0, wEntries, lpPalEntries);

   if (hOldPal)
      SelectPalette (hDC, hOldPal, FALSE);

   ReleaseDC (hWnd, hDC);

   GlobalUnlock (hPalEntries);
   GlobalFree (hPalEntries);
}




//---------------------------------------------------------------------
//
// Function:   CopyPaletteChangingFlags
//
// Purpose:    Duplicate a given palette, changing all the flags in
//             it to a certain flag value (i.e. peFlags member of
//             the PALETTEENTRY structure).
//
// Parms:      hPal     == Handle to palette to duplicate.
//             bNewFlag == New peFlags PALETTEENTRY flag.  Set
//                         to DONT_CHANGE_FLAGS if don't want
//                         to touch the flags.
//
// Returns:    Handle to the new palette.  NULL on error.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

HPALETTE CopyPaletteChangingFlags (HPALETTE hPal, BYTE bNewFlag)
{
   WORD         wEntries, i;
   HANDLE       hLogPal;
   LPLOGPALETTE lpLogPal;

   if (!hPal)
      return NULL;

   wEntries = ColorsInPalette (hPal);

   if (!wEntries)
      return NULL;

   hLogPal = GlobalAlloc (GHND, 
               sizeof (LOGPALETTE) + sizeof (PALETTEENTRY) * wEntries);

   if (!hLogPal)
      return NULL;

   lpLogPal = (LPLOGPALETTE) GlobalLock (hLogPal);

   lpLogPal->palVersion    = PALVERSION;
   lpLogPal->palNumEntries = wEntries;

   GetPaletteEntries (hPal, 0, wEntries, lpLogPal->palPalEntry);

   if (bNewFlag != DONT_CHANGE_FLAGS)
      for (i = 0;  i < wEntries;  i++)
         lpLogPal->palPalEntry[i].peFlags = bNewFlag;

   hPal = CreatePalette (lpLogPal);

   GlobalUnlock (hLogPal);
   GlobalFree (hLogPal);

   return hPal;
}
