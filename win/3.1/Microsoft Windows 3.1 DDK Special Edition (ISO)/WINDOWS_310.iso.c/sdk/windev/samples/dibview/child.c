/*************************************************************************

      File:  CHILD.C

   Purpose:  Contains the routines to implement displaying a bitmap in
             an MDI child window.  Each window has its own set of
             information stored in its window words which identifies
             the bitmap to be displayed, etc..

 Functions:  ChildWndProc
             ChildWndCreate
             ChildWndPaint
             ChildWndDestroy
             ChildWndScroll
             ChildWndKeyDown
             ChildWndQueryNewPalette
             ChildWndPaletteChanged
             ChildWndStartAnimate
             ChildWndLeftButton
             ChildWndSize
             SetupScrollBars
             ScrollBarsOff
             GetCurrentMDIWnd
             CurrentDIBPalette
             GetCurrentDIBStretchFlag
             SetCurrentDIBStretchFlag
             ReallyGetClientRect
             Hourglass
             SetMessageToAllChildren
             CloseAllDIBWindows
             TrackMouse
             NormalizeRect
             DrawSelect
             GetCurrentClipRect
             GetCurrentDIBSize

  Comments:  Special considerations are made in this module to get
             Windows to handle the system palette correctly for
             child windows (MDI child windows in this case).  
             
             Noramlly, an application with the focus has the
             "foreground" palette.  Since this application uses
             multiple palettes (one per bitmap), special handling
             must be used to force all the windows without the focus
             to "background" palettes.

             We accomplish this by forcing the "active" child window's
             palette to be the whole application's foreground palette
             (i.e. get a DC for the frame, and realize the child's
             palette in that DC as a foreground palette).

             All other times we realize palettes, we realize them
             as background palettes.  If the palette being realized
             was realized as the frame's foreground palette, then
             when it is realized as a background palette, all the
             colors will map to the correct colors!  If the palette
             being realized was not the foreground palette, the colors
             are mapped into the system palette on a "best match"
             basis.

   History:   Date      Reason
             6/ 1/91     Created
            12/03/91     Fixed palette handling code.

*************************************************************************/



#include <windows.h>
#include <assert.h>
#include "child.h"
#include "errors.h"
#include "paint.h"
#include "dib.h"
#include "palette.h"
#include "dibview.h"
#include "clipbrd.h"
#include "file.h"
#include "frame.h"



   // Some magic numbers.

#define TIMER_ID        1     // Timer ID when palette animating.
#define TIMER_INTERVAL  100   // # of ms between timer ticks when animating.
#define SCROLL_RATIO    4     // WM_VSCROLL scrolls DIB by 1/x of client area.



   // The following defines are the default values for the OPTIONSINFO
   //  structure stored in the child window's INFOSTRUCT (which is a
   //  structure containing information on the child window's bitmap,
   //  and list of options).  See ChildWndCreate() to see how these
   //  are used.

#define OPT_DEF_STRETCH   FALSE         // Don't stretch the DIB on display.
#define OPT_DEF_BANDING   TRUE          // Band DIB to printer.
#define OPT_DEF_USE31PRNAPIS FALSE      // Don't use the 3.1 Printing APIs.
#define OPT_DEF_DISP      DISP_USE_DDBS // Display DDBs instead of DIBs.
#define OPT_DEF_PRNSIZE   PRINT_BESTFIT // Print in "best fit" mode.
#define OPT_DEF_PRNSCALEX 1             // X stretch factor = 1 (in PRINT_STRETCH mode)
#define OPT_DEF_PRNSCALEY 1             // Y stretch factor = 1 (in PRINT_STRETCH mode)



   // Some useful macros.

#define MAX(a,b)     ((a) > (b) ? (a) : (b))
#define MIN(a,b)     ((a) < (b) ? (a) : (b))



   // Some globals.

HWND  hWndAnimate = NULL;     // HWND of currently palette animated DIB.
HWND  hWndClip    = NULL;     // Current Window to be rendered to clipboard.
POINT ptClipSize  = {0,0};    // Size of DIB at time of copy (i.e. was the DIB stretched?)
int   nDIBsOpen   = 0;        // # of MDI child windows currently open.


   // Local function prototypes.

void  ChildWndCreate      (HWND hWnd, LPDIBCREATEINFO lpDIBCreateInfo);
void  ChildWndPaint       (HWND hWnd);
void  ChildWndDestroy     (HWND hWnd);
void  ChildWndScroll      (HWND hWnd, int message, WORD wPos, WORD wScrollType);
BOOL  ChildWndQueryNewPalette (HWND hWnd, HWND hWndFrame);
void  ChildWndPaletteChanged (HWND hWnd);
void  ChildWndStartAnimate(HWND hWnd);
void  ChildWndLeftButton  (HWND hWnd, int x, int y);
DWORD ChildWndKeyDown     (HWND hWnd, WORD wKeyCode, LONG lParam);
void  ChildWndSize        (HWND hWnd);
void  SetupScrollBars     (HWND hWnd, WORD cxDIB, WORD cyDIB);
void  ScrollBarsOff       (HWND hWnd);
void  ReallyGetClientRect (HWND hWnd, LPRECT lpRect);
void  Hourglass           (BOOL bDisplay);
void  TrackMouse          (HWND hWnd, LPRECT lpClipRect, int cxDIB, int cyDIB);
void  NormalizeRect       (LPRECT lpRect);
void  DrawSelect          (HDC hDC, RECT rcClip);


//---------------------------------------------------------------------
//
// Function:   ChildWndProc
//
// Purpose:    Window procedure for DIB MDI child windows.
//             Handles all messages destined for these windows.
//
// Parms:      hWnd    == Handle to this MDI child window.
//             message == Message for window.
//             wParam  == Depends on message.
//             lParam  == Depends on message.
//
// History:   Date      Reason
//             6/01/91  Created
//            10/15/91  Moved WM_CREATE handler to own function.
//                      Moved WM_DESTROY handler to own function.
//                      Moved WM_PAINT handler to own function.
//                      Moved WM_?SCROLL handler to own function.
//             
//---------------------------------------------------------------------

long FAR PASCAL ChildWndProc (HWND hWnd, 
			      UINT message,
			      WPARAM wParam,
			      LPARAM lParam)
{
   switch (message)
      {
         // Window being created, do initialization.  In MDI, the lParam
         //  is a far pointer to a CREATESTRUCT.  The CREATESTRUCT's
         //  lpCreateParams is a far pointer to an MDICREATESTRUCT.
         //  The MDICREATESTRUCT's lParam is an application supplied
         //  LONG.  In DIBView, this LONG is actually a far pointer to
         //  a DIBCREATEINFO structure.  This structure is initialized
         //  by the routine which sent the WM_MDICREATE message to the
         //  MDI frame (in FRAME.C).

      case WM_CREATE:
         ChildWndCreate (hWnd,
                         (LPDIBCREATEINFO) 
                         ((LPMDICREATESTRUCT) 
                          ((LPCREATESTRUCT) lParam)->lpCreateParams)->lParam);
         break;



         // If this window is being activated, simulate a 
         //  MYWM_QUERYNEWPALETTE message.

      case WM_MDIACTIVATE:
         {
         HWND hWndFrame;

         if (wParam)
            {
            hWndFrame = GetParent (GetParent (hWnd));
            SendMessage (hWnd, MYWM_QUERYNEWPALETTE, hWndFrame, 0L);
            }

         break;
         }



         // Need to paint, call the paint routine.

      case WM_PAINT:
         ChildWndPaint (hWnd);
         break;



         // User's dragging a minimized MDI child, return the cursor to drag.

      case WM_QUERYDRAGICON:
         return LoadCursor (hInst, DRAGCURSOR);



         // Window's being destroyed, call the destroy routine.

      case WM_DESTROY:
         ChildWndDestroy (hWnd);
         break;



         // Ensure that the clipboard data can be rendered even though
         //  this window is being destroyed.  First open the clipboard.
         //  Then empty what we put there earlier and re-render everything.

      case WM_RENDERALLFORMATS:
         {
         if (!OpenClipboard (hWnd))
            break;

         EmptyClipboard ();

         SendMessage(hWnd, WM_RENDERFORMAT, CF_DIB,     0L);
         SendMessage(hWnd, WM_RENDERFORMAT, CF_BITMAP,  0L);
         SendMessage(hWnd, WM_RENDERFORMAT, CF_PALETTE, 0L);

         CloseClipboard ();
         break;
         }


         // Format the data in the manner requested and pass the handle of
         // the data to the clipboard.

      case WM_RENDERFORMAT:
         {
         HANDLE hClipBoardData;

         hClipBoardData = RenderFormat (hWndClip, wParam, ptClipSize);

         if (hClipBoardData)
            SetClipboardData (wParam, hClipBoardData);
         break;
         }


         // Window's being scrolled, call the scroll handler.

      case WM_HSCROLL:
      case WM_VSCROLL:
         ChildWndScroll (hWnd, message, LOWORD (lParam), wParam);
         break;



         // Keypress -- go handle it.

      case WM_KEYDOWN:
         return ChildWndKeyDown (hWnd, wParam, lParam);



         // Window's getting focus, realize our palette.  We set it
         //  up so that the HWND of the frame window is in wParam.
         //  This is so we can realize our palette as the foreground
         //  palette of the *entire* application (Windows is designed
         //  so that the application has one foreground palette -- owned
         //  by the top-level window of the app).  We could realize it
         //  as foreground, and supply our own hWnd, but this can lead
         //  to some weird results...

      case MYWM_QUERYNEWPALETTE:
         return ChildWndQueryNewPalette (hWnd, (HWND) wParam);



         // Someone changed the system's palette.  Update our window
         //  to reflect the new palette.

      case WM_PALETTECHANGED:
         if (hWnd == (HWND) wParam)
            break;

         ChildWndPaletteChanged (hWnd);
         break;



         // User wants to animate palette, call routine to start
         //  animation.

      case MYWM_ANIMATE:
         ChildWndStartAnimate (hWnd);
         break;




         // Timer went off -- this only happens when we're animating
         //  the palette.

      case WM_TIMER:
         {
         HANDLE    hDIBInfo;
         LPDIBINFO lpDIBInfo;

         hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

         if (!hDIBInfo)
            break;

         lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);

         MyAnimatePalette (hWnd, lpDIBInfo->hPal);

         GlobalUnlock (hDIBInfo);
         break;
         }



         // Restore the DIB's palette after palette animation.  Stop
         //  animation.  Delete what we created for animation.  Also, 
         //  we need to re-create the bitmap, since we earlier re-created
         //  it to reflect the animation palette (see MYWM_ANIMATE).  
         //  Finally, re-draw the bitmap.

      case MYWM_RESTOREPALETTE:
         {
         HANDLE    hDIBInfo;
         LPDIBINFO lpDIBInfo;

         SendMessage (hWnd, WM_RBUTTONDOWN, 0, 0L);

         hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

         if (!hDIBInfo)
            break;

         lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);

         if (lpDIBInfo->hBitmap)
            DeleteObject (lpDIBInfo->hBitmap);

         if (lpDIBInfo->hPal)
            DeleteObject (lpDIBInfo->hPal);

         lpDIBInfo->hPal    = CreateDIBPalette (lpDIBInfo->hDIB);
         lpDIBInfo->hBitmap = DIBToBitmap (lpDIBInfo->hDIB, lpDIBInfo->hPal);

         GlobalUnlock (hDIBInfo);

         InvalidateRect (hWnd, NULL, FALSE);
         break;
         }




         // If the user presses the right mouse button and we're
         //  palette animating, stop animating.  The bitmap is
         //  left in it's animated state, as is the palette.  The
         //  restore option must be picked to return the bitmap/
         //  palette to their original states.

      case WM_RBUTTONDOWN:
         if (hWndAnimate == hWnd)
            {
            KillTimer (hWnd, TIMER_ID);
            hWndAnimate = NULL;
            }
         break;



         // Left button pressed -- track a clipping rectangle for clipboard.

      case WM_LBUTTONDOWN:
          ChildWndLeftButton (hWnd, LOWORD (lParam), HIWORD (lParam));
          break;




         // Handle the WM_SIZE message.
         //
         // Note:  This routine calls SetupScrollBars, which can
         //        change the size of the window's client rectangle.
         //        This, in turn, can send a WM_SIZE message to
         //        the window.  An infinite loop occurs because of
         //        this -- therefore, a semaphore is set up to not
         //        allow WM_SIZE to be processed while another
         //        WM_SIZE is still being processed.

      case WM_SIZE:
         {
         static BOOL bInSize = FALSE;

            // Check the semaphore, return if it's set.

         if (bInSize)
            return NULL;

         bInSize = TRUE;
         ChildWndSize (hWnd);
         bInSize = FALSE;
         }


         /*** WM_SIZE falls through to default (necessary for MDI) ****/




	         // Since the MDI default behavior is a little different,
            // call DefMDIChildProc instead of DefWindowProc().

      default:
         return DefMDIChildProc (hWnd, message, wParam, lParam);
      }


   return NULL;
}




//---------------------------------------------------------------------
//
// Function:   ChildWndCreate
//
// Purpose:    Called by ChildWndProc() on WM_CREATE.  Does initial
//             setup of MDI child winodw.  
//
//             The lpDIBCreateInfo contains a handle to the DIB to
//             be displayed in this window.  Get information on this
//             DIB, create an INFOSTRUCT, and store the handle to
//             this INFOSTRUCT in this window's window words.  This
//             information is then used extensively on many messages
//             handled by ChildWndProc().
//
//             An OPTIONSINFO structure is in the INFOSTRUCT.  All
//             the options are set by the options dialog (in OPTIONS.C).
//             During creation of the DIB window, options are set to
//             default values.
//
//             Also, on creation, set the focus to this window.
//             And, incremente the # of windows open global variable.
//
// Parms:      hWnd            == Handle to window being created.
//             lpDIBCreateInfo == Far pointer to DIBCREATEINFO structure
//                                passed in during WM_MDICREATE message
//                                (by FRAME.C).
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_CREATE case.
//                      Also cleaned up code, and got rid
//                      of the hDIBCreateInfo handle.
//            10/27/91  Added bPrinterBand to options.
//                      Use #define's for options.
//            10/28/91  Added bUse31PrintAPIs
//             
//---------------------------------------------------------------------

void ChildWndCreate (HWND hWnd, LPDIBCREATEINFO lpDIBCreateInfo)
{
   HANDLE          hDIB = NULL;
   LPSTR           lpDIB;
   HANDLE          hDIBInfo = NULL;
   LPDIBINFO       lpDIBInfo;
   DWORD           dwDIBHeight, dwDIBWidth;
   WORD            wBPP, wCompression;
   char            szFileName [129];
   char            szTitleBuf[160];

   hDIB = lpDIBCreateInfo->hDIB;
   lstrcpy (szFileName, lpDIBCreateInfo->szFileName);


      // Get some information about the DIB.  Some of the info
      //  is obtained from within the header (be it a BITMAPINFOHEADER
      //  or a BITMAPCOREHEADER).

   lpDIB       = GlobalLock (hDIB);
   dwDIBHeight = DIBHeight (lpDIB);
   dwDIBWidth  = DIBWidth (lpDIB);
   if (IS_WIN30_DIB (lpDIB))
      {
      wCompression = (WORD) ((LPBITMAPINFOHEADER) lpDIB)->biCompression;
      wBPP = ((LPBITMAPINFOHEADER) lpDIB)->biBitCount;
      }
   else
      {
      wCompression = BI_PM;
      wBPP = ((LPBITMAPCOREHEADER) lpDIB)->bcBitCount;
      }
   GlobalUnlock (hDIB);


      // Allocate room for the DIBINFO structure and fill it in.

   if (hDIB)
      hDIBInfo = GlobalAlloc (GHND, sizeof (DIBINFO));

   if (hDIBInfo)
      {
      lpDIBInfo                = (LPDIBINFO) GlobalLock (hDIBInfo);
      lpDIBInfo->hDIB          = hDIB;
      lpDIBInfo->hPal          = CreateDIBPalette (hDIB);
      lpDIBInfo->hBitmap       = DIBToBitmap (hDIB, lpDIBInfo->hPal);

      lpDIBInfo->wDIBType      = wCompression;
      lpDIBInfo->wDIBBits      = wBPP;
      lpDIBInfo->wDIBWidth     = (WORD) dwDIBWidth;
      lpDIBInfo->wDIBHeight    = (WORD) dwDIBHeight;

      lpDIBInfo->rcClip.left   = 0;
      lpDIBInfo->rcClip.right  = 0;
      lpDIBInfo->rcClip.top    = 0;
      lpDIBInfo->rcClip.bottom = 0;

      lstrcpy (lpDIBInfo->szFileName, szFileName);

      lpDIBInfo->Options.bStretch        = OPT_DEF_STRETCH;
      lpDIBInfo->Options.bPrinterBand    = OPT_DEF_BANDING;
      lpDIBInfo->Options.bUse31PrintAPIs = OPT_DEF_USE31PRNAPIS;
      lpDIBInfo->Options.wDispOption     = OPT_DEF_DISP;
      lpDIBInfo->Options.wPrintOption    = OPT_DEF_PRNSIZE;
      lpDIBInfo->Options.wXScale         = OPT_DEF_PRNSCALEX;
      lpDIBInfo->Options.wYScale         = OPT_DEF_PRNSCALEY;

      wsprintf((LPSTR)szTitleBuf, (LPSTR)"%s (%ld x %ld x %d BPP)",
               (LPSTR) szFileName, dwDIBWidth, dwDIBHeight, wBPP);

      SetWindowText (hWnd, (LPSTR) szTitleBuf);

      GlobalUnlock (hDIBInfo);
      }
   else
      DIBError (ERR_MEMORY);


      // Set the window word for the handle to the DIBINFO structure.

   SetWindowWord (hWnd, WW_DIB_HINFO, hDIBInfo);


      // On initial creation, focus isn't set to us, so set it
      //  explicitly.

   SetFocus (hWnd);


      // Increment the # of DIBs open variable and insure that the
      //  "Window" pull down menu is not grayed.

   nDIBsOpen++;
   EnableWindowAndOptionsMenus (TRUE);
}





//---------------------------------------------------------------------
//
// Function:   ChildWndPaint
//
// Purpose:    Called by ChildWndProc() on WM_PAINT.  Does all paints
//             for this MDI child window.
//
//             Reads position of scroll bars to find out what part
//             of the DIB to display.
//
//             Checks the stretching flag in the DIBINFO structure for
//             this window to see if we are stretching to the window
//             (if we're iconic, we always stretch to a tiny bitmap).
//
//             Selects/Realizes the palette as a background palette.
//             ChildWndQueryNewPalette realized it already as the
//             foreground palette if this window is the active MDI
//             child.
//
//             Calls the appropriate paint routine depending on the
//             option set for this window (i.e. DIB, DDB, or SetDIBits).
//
//             Draws the selection rectangle for copying to the
//             clipboard.
//
// Parms:      hWnd == Handle to window being painted.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_PAINT case.
//            12/03/91  Always force SelectPalette() to a
//                        background palette.  If it was
//                        the foreground palette, it would
//                        have been realized during
//                        WM_QUERYNEWPALETTE.
//             
//---------------------------------------------------------------------

void ChildWndPaint (HWND hWnd)
{
   HDC         hDC;
   PAINTSTRUCT ps;
   int         xScroll, yScroll;
   HPALETTE    hOldPal = NULL;
   RECT        rectClient, rectDDB;
   BOOL        bStretch;
   HANDLE      hDIBInfo;
   LPDIBINFO   lpDIBInfo;
   BITMAP      Bitmap;

   Hourglass (TRUE);

   hDC      = BeginPaint (hWnd, &ps);
   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);
   xScroll  = GetScrollPos  (hWnd, SB_HORZ);
   yScroll  = GetScrollPos  (hWnd, SB_VERT);

   if (!hDIBInfo)
      goto ABORTPAINT;

   lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);

   if (!lpDIBInfo->hDIB || !lpDIBInfo->hBitmap)
      {
      GlobalUnlock (hDIBInfo);
      goto ABORTPAINT;
      }

   bStretch = lpDIBInfo->Options.bStretch;
      

      // When we're iconic, we'll always stretch the DIB
      //  to our icon.  Otherwise, we'll use the stretching
      //  option the user picked.

   if (IsIconic (hWnd))
      bStretch = TRUE;
   else
      bStretch = lpDIBInfo->Options.bStretch;


      // Set up the scroll bars appropriately.

   if (bStretch)
      ScrollBarsOff (hWnd);
   else
      SetupScrollBars (hWnd, lpDIBInfo->wDIBWidth, lpDIBInfo->wDIBHeight);


      // Set up the necessary rectangles -- i.e. the rectangle
      //  we're rendering into, and the rectangle in the DIB.

   GetClientRect (hWnd, &rectClient);
   GetObject (lpDIBInfo->hBitmap, sizeof (Bitmap), (LPSTR) &Bitmap);
   
   if (bStretch)
      {
      rectDDB.left   = 0;
      rectDDB.top    = 0;
      rectDDB.right  = Bitmap.bmWidth;
      rectDDB.bottom = Bitmap.bmHeight;
      }
   else
      {
      rectDDB.left   = xScroll;
      rectDDB.top    = yScroll;
      rectDDB.right  = xScroll + rectClient.right - rectClient.left;
      rectDDB.bottom = yScroll + rectClient.bottom - rectClient.top;

      if (rectDDB.right > Bitmap.bmWidth)
         {
         int dx;

         dx = Bitmap.bmWidth - rectDDB.right;

         rectDDB.right     += dx;
         rectClient.right  += dx;
         }

      if (rectDDB.bottom > Bitmap.bmHeight)
         {
         int dy;

         dy = Bitmap.bmHeight - rectDDB.bottom;

         rectDDB.bottom    += dy;
         rectClient.bottom += dy;
         }
      }


      // Setup the palette.

   if (lpDIBInfo->hPal)
      hOldPal = SelectPalette (hDC, lpDIBInfo->hPal, TRUE);

   RealizePalette (hDC);


      // Go do the actual painting.

   switch (lpDIBInfo->Options.wDispOption)
      {
      case DISP_USE_DIBS:
         DIBPaint (hDC, &rectClient, lpDIBInfo->hDIB, &rectDDB, 
                     lpDIBInfo->hPal);
         break;


      case DISP_USE_SETDIBITS:
         SetDIBitsPaint (hDC, &rectClient, lpDIBInfo->hDIB, &rectDDB,
                           lpDIBInfo->hPal);
         break;


      case DISP_USE_DDBS:
      default:
         DDBPaint (hDC, &rectClient, lpDIBInfo->hBitmap, &rectDDB, 
                     lpDIBInfo->hPal);
         break;
      }


      // Draw the clipboard selection rubber-band.

   SetWindowOrg (hDC, 
                  GetScrollPos (hWnd, SB_HORZ), 
                  GetScrollPos (hWnd, SB_VERT));
   DrawSelect (hDC, lpDIBInfo->rcClip);


   if (hOldPal)
      SelectPalette (hDC, hOldPal, FALSE);

   GlobalUnlock (hDIBInfo);

ABORTPAINT:

   EndPaint (hWnd, &ps);

   Hourglass (FALSE);
}



//---------------------------------------------------------------------
//
// Function:   ChildWndDestroy
//
// Purpose:    Called by ChildWndProc() on WM_DESTROY.  Window is
//             being destroyed, do all necessary cleanup.
//
//             Window's going away, free up the DIB, DDB, Palette, and
//             DIBINFO structure.
//
//             If we're palette animating, kill the timer and free
//             up animation palette.  
//
//             If we have the clipboard, send the WM_RENDERALLFORMATS to 
//             our window (Windows will only send it to our app if the
//             main window owns the clipboard;  in this app, the MDI child
//             window owns the clipboard).
//
//             Decrement the # of DIB windows open global variable.
//
//
// Parms:      hWnd == Handle to window being destroyed.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_DESTROY case.
//             
//---------------------------------------------------------------------

void ChildWndDestroy (HWND hWnd)
{
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;


      // If we have the clipboard, render all our formats
      //  now.

   if (hWnd == GetClipboardOwner ())
      {
      SendMessage (hWnd, WM_RENDERALLFORMATS, 0, 0L);
      hWndClip = NULL;
      }


      // Free up resources connected to this window.

   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);
   if (hDIBInfo)
      {
      lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);

      if (lpDIBInfo->hDIB)
         GlobalFree (lpDIBInfo->hDIB);

      if (lpDIBInfo->hPal)
         DeleteObject (lpDIBInfo->hPal);

      if (lpDIBInfo->hBitmap)
         DeleteObject (lpDIBInfo->hBitmap);

      GlobalUnlock (hDIBInfo);
      GlobalFree (hDIBInfo);

      SetWindowWord (hWnd, WW_DIB_HINFO, NULL);

      if (--nDIBsOpen == 0)
         EnableWindowAndOptionsMenus (FALSE);
      }


      // If we're animating, turn off the timer.

   if (hWndAnimate == hWnd)
      {
      KillTimer (hWnd, TIMER_ID);
      hWndAnimate = NULL;
      }
}



//---------------------------------------------------------------------
//
// Function:   ChildWndScroll
//
// Purpose:    Called by ChildWndProc() on WM_HSCROLL and WM_VSCROLL.
//             Window needs to be scrolled (user has clicked on one
//             of the scroll bars.
//
//             Does scrolling in both horiziontal and vertical directions.
//             Note that the variables are all named as if we were
//             doing a horizontal scroll.  However, if we're doing a
//             vertical scroll, they are initialized to the appropriate
//             values for a vertical scroll.
//
//             If we scroll by one (i.e. user clicks on one of the
//             scrolling arrows), we scroll the window by 1/SCROLL_RATIO
//             of the client area.  In other words, if SCROLL_RATION==4,
//             then we move the client area over a 1/4 of the width/height
//             of the screen.
//
//             If the user is paging up/down we move a full client area's
//             worth.
//
//             If the user moves the thumb to an absolute position, we
//             just move there.
//
//             ScrollWindow/re-painting do the actual work of scrolling.
//
// Parms:      hWnd        == Handle to window being scrolled.
//             message     == Message being handled (WM_HSCROLL or WM_VSCROLL)
//             wPos        == Thumb position (only valid for SB_THUMBPOSITION
//                            and SB_THUMBTRACK).
//             wScrollType == wParam to WM_SCROLL (one of the SB_* constants)
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_?SCROLL case.
//             
//---------------------------------------------------------------------

void ChildWndScroll (HWND hWnd, int message, WORD wPos, WORD wScrollType)
{
   int  xBar;                       // Where scrollbar is now.
   int  nMin;                       // Minumum scroll bar value.
   int  nMax;                       // Maximum scroll bar value.
   int  dx;                         // How much to move.
   int  nOneUnit;                   // # of pixels for LINEUP/LINEDOWN
   int  cxClient;                   // Width of client area.
   int  nHorzOrVert;                // Doing the horizontal or vertical?
   RECT rect;                       // Client area.


   GetClientRect (hWnd, &rect);

   if (message == WM_HSCROLL)
      {
      nHorzOrVert = SB_HORZ;
      cxClient    = rect.right - rect.left;
      }
   else
      {
      nHorzOrVert = SB_VERT;
      cxClient    = rect.bottom - rect.top;
      }

      // One a SB_LINEUP/SB_LINEDOWN we will move the DIB by
      //  1/SCROLL_RATIO of the client area (i.e. if SCROLL_RATIO
      //  is 4, it will scroll the DIB a quarter of the client
      //  area's height or width.

   nOneUnit = cxClient / SCROLL_RATIO;
   if (!nOneUnit)
      nOneUnit = 1;

   xBar = GetScrollPos (hWnd, nHorzOrVert);
   GetScrollRange (hWnd, nHorzOrVert, &nMin, &nMax);

   switch (wScrollType)
      {
      case SB_LINEDOWN:             // One line right.
         dx = nOneUnit;
         break;

      case SB_LINEUP:               // One line left.
         dx = -nOneUnit;
         break;

      case SB_PAGEDOWN:             // One page right.
         dx = cxClient;
         break;

      case SB_PAGEUP:               // One page left.
         dx = -cxClient;
         break;

      case SB_THUMBPOSITION:        // Absolute position.
         dx = wPos - xBar;
         break;

      default:                      // No change.
         dx = 0;
         break;
      }

   if (dx)
      {
      xBar += dx;

      if (xBar < nMin)
         {
         dx  -= xBar - nMin;
         xBar = nMin;
         }

      if (xBar > nMax)
         {
         dx  -= xBar - nMax;
         xBar = nMax;
         }

      if (dx)
         {
         SetScrollPos (hWnd, nHorzOrVert, xBar, TRUE);

         if (nHorzOrVert == SB_HORZ)
            ScrollWindow (hWnd, -dx, 0, NULL, NULL);
         else
            ScrollWindow (hWnd, 0, -dx, NULL, NULL);

         UpdateWindow (hWnd);
         }
      }
}



//---------------------------------------------------------------------
//
// Function:   ChildWndKeyDown
//
// Purpose:    Called by ChildWndProc() on WM_KEYDOWN.  Keyboard interface
//             for MDI DIB Child window.
//
//             Keyboard interface.  Handles scrolling around the DIB
//             using the keypad, and translates ESC's into WM_RBUTTONDOWN
//             messages (to stop any palette animation in progress).
//
//             The numeric keypad/arrows are translated into scroll
//             bar messages.
//
// Parms:      hWnd     == Handle to window where key was pressed.
//             wKeyCode == Key code pressed (wParam to WM_KEYDOWN -- one
//                         of the VK_* constants)
//             lParam   == lParam for WM_KEYDOWN.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_KEYDOWN case.
//             
//---------------------------------------------------------------------

DWORD ChildWndKeyDown (HWND hWnd, WORD wKeyCode, LONG lParam)
{
   unsigned uMsg;
   WORD     wSB;

   switch (wKeyCode)
      {
      case VK_ESCAPE:
         SendMessage (hWnd, WM_RBUTTONDOWN, 0, 0L);
         break;

      case VK_UP:
         uMsg = WM_VSCROLL;
         wSB  = SB_LINEUP;
         break;

      case VK_DOWN:
         uMsg = WM_VSCROLL;
         wSB  = SB_LINEDOWN;
         break;

      case VK_LEFT:
         uMsg = WM_HSCROLL;
         wSB  = SB_LINEUP;
         break;

      case VK_RIGHT:
         uMsg = WM_HSCROLL;
         wSB  = SB_LINEDOWN;
         break;

      case VK_NUMPAD9:
         uMsg = WM_VSCROLL;
         wSB  = SB_PAGEUP;
         break;

      case VK_NUMPAD3:
         uMsg = WM_VSCROLL;
         wSB  = SB_PAGEDOWN;
         break;

      case VK_NUMPAD7:
         uMsg = WM_HSCROLL;
         wSB  = SB_PAGEUP;
         break;

      case VK_NUMPAD1:
         uMsg = WM_HSCROLL;
         wSB  = SB_PAGEDOWN;
         break;

      default:
         return DefMDIChildProc (hWnd, uMsg, wSB, lParam);
      }

   return SendMessage (hWnd, uMsg, wSB, 0L);
}




//---------------------------------------------------------------------
//
// Function:   ChildWndQueryNewPalette
//
// Purpose:    Called by ChildWndProc() on WM_QUERYNEWPALETTE.
//
//             We get this message when an MDI child is getting
//             focus (by hocus pockus in FRAME.C, and by passing
//             this message when we get WM_MDIACTIVATE).  Normally
//             this message is passed only to the top level window(s)
//             of an application.
//
//             We want this window to have the foreground palette when this
//             happens, so we select and realize the palette as
//             a foreground palette (of the frame Window).  Then make
//             sure the window repaints, if necessary.
//
// Parms:      hWnd      == Handle to window getting WM_QUERYNEWPALETTE.
//             hWndFrame == Handle to the frame window (i.e. the top-level
//                            window of this app.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_QUERYNEWPALETTE case.
//            12/03/91  Added hWndFrame parameter, realization
//                      of palette as palette of frame window,
//                      and updating the window only if the
//                      palette changed.
//             
//---------------------------------------------------------------------

BOOL ChildWndQueryNewPalette (HWND hWnd, HWND hWndFrame)
{
   HPALETTE  hOldPal;
   HDC       hDC;
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;
   int       nColorsChanged;

   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

   if (!hDIBInfo)
      return FALSE;

   lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);

   if (!lpDIBInfo->hPal)
      {
      GlobalUnlock (hDIBInfo);
      return FALSE;
      }


      // We're going to make our palette the foreground palette for
      //  this application.  Window's palette manager expects the
      //  top-level window of the application to have the palette,
      //  so, we get a DC for the frame here!

   hDC     = GetDC (hWndFrame);
   hOldPal = SelectPalette (hDC, lpDIBInfo->hPal, FALSE);
   
   nColorsChanged = RealizePalette (hDC);

   if (nColorsChanged)
      InvalidateRect (hWnd, NULL, FALSE);

   if (hOldPal)
      SelectPalette (hDC, hOldPal, FALSE);

   ReleaseDC (hWndFrame, hDC);

   GlobalUnlock (hDIBInfo);

   return (nColorsChanged != 0);
}




//---------------------------------------------------------------------
//
// Function:   ChildWndPaletteChanged
//
// Purpose:    Called by ChildWndProc() on WM_PALETTECHANGED.
//
//             WM_PALETTECHANGED messages are passed to all MDI
//             children by the frame window (in FRAME.C).  Normally,
//             these messages are only sent to the top-level window
//             in an application.
//
//             On a palette changed, we want to realize this window's
//             palette.  We realize it always as a background palette.
//             See the comments section at the top of this file for
//             an explanation why.
//
// Parms:      hWnd == Handle to window getting WM_PALETTECHANGED.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_PALETTECHANGED case.
//            12/03/91  Always force SelectPalette() to a
//                        background palette.  If it was
//                        the foreground palette, it would
//                        have been realized during
//                         WM_QUERYNEWPALETTE.
//             
//---------------------------------------------------------------------

void ChildWndPaletteChanged (HWND hWnd)
{
   HPALETTE  hOldPal;
   HDC       hDC;
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;

   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

   if (!hDIBInfo)
      return;

   lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);

   if (!lpDIBInfo->hPal)
      {
      GlobalUnlock (hDIBInfo);
      return;
      }

   hDC     = GetDC (hWnd);
   hOldPal = SelectPalette (hDC, lpDIBInfo->hPal, TRUE);

   GlobalUnlock (hDIBInfo);
   
   RealizePalette (hDC);
   UpdateColors (hDC);

   if (hOldPal)
      SelectPalette (hDC, hOldPal, FALSE);

   ReleaseDC (hWnd, hDC);
}




//---------------------------------------------------------------------
//
// Function:   ChildWndStartAnimate
//
// Purpose:    Called by ChildWndProc() on MYWM_ANIMATE.
//
//             Animate this DIB's palette.  First do some setup (see if
//             we're already doing animation, and start a timer).  Then
//             Create a new palette with the PC_RESERVED flag so it can
//             be animated.  Then re-create the bitmap so it uses this
//             new palette.  Finally, set our window words and re-draw
//             the bitmap.
//
//
// Parms:      hWnd == Handle to window getting MYWM_ANIMATE.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from MYWM_ANIMATE case.
//             
//---------------------------------------------------------------------

void ChildWndStartAnimate (HWND hWnd)
{
   HPALETTE  hNewPal;
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;


      // Don't allow more than one window to animate at a time.

   if (hWndAnimate)
      {
      DIBError (ERR_ANIMATE);
      return;
      }


      // Set a timer to animate on.

   if (!SetTimer (hWnd, TIMER_ID, TIMER_INTERVAL, NULL))
      {
      DIBError (ERR_NOTIMERS);
      return;
      }


      // Remember who's animating, get the palette for this window,
      //  copy it changing its flags to PC_RESERVED (so each palette
      //  entry can be animated).

   hWndAnimate = hWnd;
   hDIBInfo    = GetWindowWord (hWnd, WW_DIB_HINFO);

   if (!hDIBInfo)
      return;

   lpDIBInfo   = (LPDIBINFO) GlobalLock (hDIBInfo);
   hNewPal     = CopyPalForAnimation (lpDIBInfo->hPal);


      // Delete the old device dependent bitmap, and palette.  Device
      //  dependent bitmaps rely on colors mapping to the exact same
      //  place in the system palette (for speed reasons).  Se we
      //  changed the palette flags to PC_RESERVED, the palette entries
      //  might not map to the same palette entries.  Therefore, it is
      //  necessary to create a new device dependent bitmap here!

   if (lpDIBInfo->hBitmap)
      DeleteObject (lpDIBInfo->hBitmap);

   if (lpDIBInfo->hPal)
      DeleteObject (lpDIBInfo->hPal);

   lpDIBInfo->hBitmap = DIBToBitmap (lpDIBInfo->hDIB, hNewPal);
   lpDIBInfo->hPal    = hNewPal;

   GlobalUnlock (hDIBInfo);

   InvalidateRect (hWnd, NULL, FALSE);
}



//---------------------------------------------------------------------
//
// Function:   ChildWndLeftButton
//
// Purpose:    Called by ChildWndProc() on WM_LBUTTONDOWN.
//
//             If the user presses the left button, erase the currently
//             selected rectangle.  Then, start drawing a
//             rectangle (for the area of the DIB to be put in the
//             clipboard on an Edit/Paste operation).
//
//
// Parms:      hWnd == Handle to window getting WM_LBUTTONDOWN.
//
// History:   Date      Reason
//
//            10/15/91  Cut code out from WM_LBUTTONDOWN case.
//             
//---------------------------------------------------------------------

void ChildWndLeftButton (HWND hWnd, int x, int y)
{
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;
   HDC       hDC;
   RECT      rcClip;
   int       cxDIB, cyDIB;


      // Find the old clip rectangle and erase it.

   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

   if (!hDIBInfo)
      return;

   hDC       = GetDC (hWnd);
   lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);
   SetWindowOrg (hDC, 
                 GetScrollPos (hWnd, SB_HORZ), 
                 GetScrollPos (hWnd, SB_VERT));
   DrawSelect (hDC, lpDIBInfo->rcClip);
   ReleaseDC (hWnd, hDC);


      // Determine the DIB's extents.  This is different than the
      //  DIB's height/width when the DIB's stretched.

   if (lpDIBInfo->Options.bStretch)
      {
      RECT rcClient;

      GetClientRect (hWnd, &rcClient);
      cxDIB = rcClient.right;
      cyDIB = rcClient.bottom;
      }
   else
      {
      cxDIB = lpDIBInfo->wDIBWidth;
      cyDIB = lpDIBInfo->wDIBHeight;
      }


      // Start a new clip rectangle.  Track the rubber band. Rubber
      //  band won't be allowed to extend past the extents of the
      //  DIB.

   rcClip.top  = y;
   rcClip.left = x;
   TrackMouse (hWnd, &rcClip, cxDIB, cyDIB);


      // Store the new clipboard coordinates.

   lpDIBInfo->rcClip = rcClip;
   GlobalUnlock (hDIBInfo);
}




//---------------------------------------------------------------------
//
// Function:   ChildWndSize
//
// Purpose:    Called by ChildWndProc() on WM_SIZE.
//
//             When the window is sized -- set up the scroll bars.
//             Also, if we're in "stretch to window" mode, the entire
//             client area must be repainted.  
//
//             The window will be repainted if the new size, combined
//             with the current scroll bar positions would create "white
//             space at the left or bottom of the window.  For example,
//             if the DIB is 100x100, the window _was_ 50x50, the new
//             size of the window is 75x75, and the current window is
//             scrolled 50 units to the right; then, if the current
//             scroll position weren't changed, we'd need to paint
//             starting at row 50, and extending through row 125.  BUT
//             since the DIB is only 100 pixels wide, white space would
//             appear at the right margin!  Instead, the thumb is placed
//             at column 25 (in SetScrollPos), and columns 25 through 
//             100 are displayed (by invalidating the client window!
//
//             Re-read the above paragraph (slowly this time)!
//
// Parms:      hWnd == Handle to window getting WM_SIZE.
//
// History:   Date      Reason
//            10/15/91  Cut code out from WM_SIZE case.
//             
//---------------------------------------------------------------------

void ChildWndSize (HWND hWnd)
{
   HANDLE      hDIBInfo;
   LPDIBINFO   lpDIBInfo;
   LPSTR       lpDIB;
   int         cxScroll, cyScroll, cxDIB = 0, cyDIB = 0;
   RECT        rect;


   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

   if (hDIBInfo)
      {
      lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);


         // Find out the DIB's height/width.

      if (lpDIBInfo->hDIB)
         {
         lpDIB = GlobalLock (lpDIBInfo->hDIB);
         cxDIB = (int) DIBWidth (lpDIB);
         cyDIB = (int) DIBHeight (lpDIB);
         GlobalUnlock (lpDIBInfo->hDIB);
         }


         // Find out the dimensions of the window, and the current
         //  thumb positions.

      GetClientRect (hWnd, &rect);
      cxScroll = GetScrollPos (hWnd, SB_HORZ);
      cyScroll = GetScrollPos (hWnd, SB_VERT);


         // If we are in "stretch to window" more, or the current
         //  thumb positions would cause "white space" at the right
         //  or bottom of the window, repaint.

      if (lpDIBInfo->Options.bStretch || 
            cxScroll + rect.right > cxDIB ||
            cyScroll + rect.bottom > cyDIB)
         InvalidateRect (hWnd, NULL, FALSE);

      if (!IsIconic (hWnd) && !lpDIBInfo->Options.bStretch)
         SetupScrollBars (hWnd, lpDIBInfo->wDIBWidth, lpDIBInfo->wDIBHeight);

      GlobalUnlock (hDIBInfo);
      }
}






//---------------------------------------------------------------------
//
// Function:   SetupScrollBars
//
// Purpose:    Sets up MDI Child's scroll bars
//
//             Either we display both scroll bars, or no scroll bars.
//
// Parms:      hWnd == Handle to window who's scroll bars we'll set up.
//             hDIB == A handle to the current DIB.
//
// History:   Date      Reason
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

void SetupScrollBars (HWND hWnd, WORD cxDIB, WORD cyDIB)
{
   RECT        rect;                         // Client Rectangle.
   BOOL        bNeedScrollBars = FALSE;      // Need Scroll bars?
   unsigned    cxWindow,                     // Width of client area.
               cyWindow;                     // Height of client area.
   int         cxRange         = 0,          // Range needed for horz bar.
               cyRange         = 0;          // Range needed for vert bar.


      // Do some initialization.

   ReallyGetClientRect (hWnd, &rect);

   cxWindow = rect.right - rect.left;
   cyWindow = rect.bottom - rect.top;


      // Now determine if we need the scroll bars.  Since the
      //  window is not allowed to be larger than the DIB, if
      //  we need one scroll bar, we need _both_.  Since if
      //  one scroll bar is turned on, it eats up some of
      //  the client area.

   if ((cxWindow < (unsigned) cxDIB) || (cyWindow < (unsigned) cyDIB))
      bNeedScrollBars = TRUE;


      // Setup the scroll bar ranges.  We want to be able to
      //  scroll the window so that all the DIB can appear
      //  within the client area.  Take into account that
      //  if the opposite scroll bar is activated, it eats
      //  up some client area.

   if (bNeedScrollBars)
      {
      cyRange = (unsigned) cyDIB - cyWindow - 1 + GetSystemMetrics (SM_CYHSCROLL);
      cxRange = (unsigned) cxDIB - cxWindow - 1 + GetSystemMetrics (SM_CXVSCROLL);
      }


      // Set the ranges we've calculated (0->0 means invisible scrollbar).

   SetScrollRange (hWnd, SB_VERT, 0, cyRange, TRUE);
   SetScrollRange (hWnd, SB_HORZ, 0, cxRange, TRUE);
}



//---------------------------------------------------------------------
//
// Function:   ScrollBarsOff
//
// Purpose:    Turns off scroll bars on the specified window.
//
// Parms:      hWnd == Handle to window to turn the scroll bars off in.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//----------------------------------------------------------------------

void ScrollBarsOff (HWND hWnd)
{
   SetScrollRange (hWnd, SB_VERT, 0, 0, TRUE);
   SetScrollRange (hWnd, SB_HORZ, 0, 0, TRUE);
}





//---------------------------------------------------------------------
//
// Function:   GetCurrentMDIWnd
//
// Purpose:    Returns the currently active MDI child window.
//
// Parms:      None.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

HWND GetCurrentMDIWnd (void)
{
   return LOWORD (SendMessage (hWndMDIClient, WM_MDIGETACTIVE, 0, 0L));
}




//---------------------------------------------------------------------
//
// Function:   CurrentDIBPalette
//
// Purpose:    Returns a handle to a duplicate of the current MDI
//             child window's palette.
//
//             This is used whenever anyone wants to use the same
//             palette -- a duplicate is created, since the MDI
//             child window could be destroyed at any time (and it's
//             palette is destroyed when the window goes away).
//
// Parms:      None.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

HPALETTE CurrentDIBPalette (void)
{
   HWND         hWnd;
   HPALETTE     hDIBPal;
   HANDLE       hDIBInfo;
   LPDIBINFO    lpDIBInfo;


      // Get a handle to the current MDI Child.

   hWnd = GetCurrentMDIWnd ();

   if (!hWnd)
      return NULL;


      // Get the current palette from the MDI Child's window words.

   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

   if (!hDIBInfo)
      return NULL;

   lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);
   hDIBPal   = lpDIBInfo->hPal;
   GlobalUnlock (hDIBInfo);

   if (!hDIBPal)
      return NULL;

   return ((HPALETTE)CopyPalette (hDIBPal));
}



//---------------------------------------------------------------------
//
// Function:   GetCurrentDIBStretchFlag
//
// Purpose:    Returns the current MDI child window's stretch flag
//             (stored in the INFOSTRUCT stored in a DIB window's
//             window words).
//
// Parms:      None.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

BOOL GetCurrentDIBStretchFlag (void)
{
   HWND      hWndDIB;
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;
   BOOL      bStretch;

   hWndDIB = GetCurrentMDIWnd ();

   if (hWndDIB)
      {
      hDIBInfo = GetWindowWord (hWndDIB, WW_DIB_HINFO);

      if (!hDIBInfo)
         return FALSE;

      lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);
      bStretch  = lpDIBInfo->Options.bStretch;
      GlobalUnlock (hDIBInfo);

      return bStretch;
      }
   else
      return FALSE;
}


//---------------------------------------------------------------------
//
// Function:   SetCurrentDIBStretchFlag
//
// Purpose:    Sets the current MDI child window's stretch flag
//             (stored in the INFOSTRUCT stored in a DIB window's
//             window words).
//
// Parms:      bFlag == New flag setting.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

void SetCurrentDIBStretchFlag (BOOL bFlag)
{
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;
   HWND      hWnd;

   hWnd = GetCurrentMDIWnd ();
   
   if (!hWnd)
      return;
      
   hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

   if (!hDIBInfo)
      return;

   lpDIBInfo                   = (LPDIBINFO) GlobalLock (hDIBInfo);
   lpDIBInfo->Options.bStretch = bFlag;
   GlobalUnlock (hDIBInfo);
}



//---------------------------------------------------------------------
//
// Function:   ReallyGetClientRect
//
// Purpose:    Gets the rectangular area of the client rect including
//             the area underneath visible scroll bars.  Stolen from
//             ShowDIB.
//
// Parms:      hWnd   == Window to get the client area of.
//             lpRect == Where to copy the rectangle to.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

void ReallyGetClientRect (HWND hWnd, LPRECT lpRect)
{
   DWORD dwWinStyle;

   dwWinStyle = GetWindowLong (hWnd, GWL_STYLE);

   GetClientRect (hWnd, lpRect);

   if (dwWinStyle & WS_HSCROLL)
      lpRect->bottom += GetSystemMetrics (SM_CYHSCROLL);

   if (dwWinStyle & WS_VSCROLL)
      lpRect->right  += GetSystemMetrics (SM_CXVSCROLL);
}





//---------------------------------------------------------------------
//
// Function:   Hourglass
//
// Purpose:    Displays or hides the hourglass during lengthy operations.
//
// Parms:      bDisplay == TRUE to display, false to put it away.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

void Hourglass (BOOL bDisplay)
{
   static HCURSOR hOldCursor = NULL;
   static int     nCount     = 0;


   if (bDisplay)
      {
         // Check if we already have the hourglass up and increment
         //  the number of times Hourglass (TRUE) has been called.

      if (nCount++)
         return;

      hOldCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

         // If this machine doesn't have a mouse, display the
         //  hourglass by calling ShowCursor(TRUE) (if it does
         //  have a mouse this doesn't do anything much).

      ShowCursor (TRUE);
      }
   else
      {
         // If we haven't changed the cursor, return to caller.

      if (!nCount)
         return;


         // If our usage count drops to zero put back the cursor
         //  we originally replaced.

      if (!(--nCount))
         {
         SetCursor (hOldCursor);
         hOldCursor = NULL;
         ShowCursor (FALSE);
         }
      }
}



//---------------------------------------------------------------------
//
// Function:   SetMessageToAllChildren
//
// Purpose:    Passes a message to all children of the specified window.
//
// Parms:      hWnd == Parent window.
//             message == message to pass to all children.
//             wParam  == wParam of message to pass to all children.
//             lParam  == lParam of message to pass to all children.
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

void SendMessageToAllChildren (HWND hWnd, 
                           unsigned message, 
                               WORD wParam, 
                               LONG lParam)
{
   HWND hChild;

   if (hChild = GetWindow(hWnd, GW_CHILD))     // Get 1st child.
      do
         SendMessage(hChild, message, wParam, lParam);
      while (hChild = GetWindow(hChild, GW_HWNDNEXT));
}




//---------------------------------------------------------------------
//
// Function:   CloseAllDIBWindows
//
// Purpose:    Close all DIB windows currently open.
//
//             First hides the MDI client -- this insures that no drawing
//             occurs in the DIB windows while we delete windows.
//             Then enumerate all the MDI client's children.  If the
//             child is a DIB window, delete it.  If it's a title bar,
//             leave it alone, and let Windows take care of it.
//
//
// Parms:      None
//
// History:   Date      Reason
//
//            6/1/91    Created.
//             
//---------------------------------------------------------------------

void CloseAllDIBWindows (void)
{
    register HWND hChild;
    BOOL          bWasVisible;


      // hide the MDI client window to avoid multiple repaints

   bWasVisible = ShowWindow (hWndMDIClient, SW_HIDE);


      // As long as the MDI client has a child, destroy it

   while (hChild = GetWindow (hWndMDIClient, GW_CHILD))
      {
         // Skip the icon title windows (they have owners, MDI children
         //  don't -- child windows have parents, not owners; title
         //  bars' owners are the MDI child windows themselves).

      while (hChild && GetWindow (hChild, GW_OWNER))
         hChild = GetWindow (hChild, GW_HWNDNEXT);

      if (!hChild)
         break;

      SendMessage (hWndMDIClient, WM_MDIDESTROY, (WORD)hChild, 0L);
      }


      // Make the MDI Client visible again, if it was visible when
      //  we called ShowWindow(..., SW_HIDE).

   if (bWasVisible)
      ShowWindow(hWndMDIClient, SW_SHOWNORMAL);
}




//---------------------------------------------------------------------
//
// Function:   TrackMouse
//
// Purpose:    This routine is called when the left mouse button is
//             held down.  It will continuously draw a rectangle
//             showing where the user has selected for cutting to the
//             clipboard.  When this routine is called, lpClipRect's
//             top/left should point at the point in the client area
//             where the left mouse button was hit.  It will return the
//             full sized rectangle the user selected.  Never allow the
//             rubber band to extend beyond the DIB's margins.
//
//             Code was stolen almost verbatim from ShowDIB.
//
// Parms:      hWnd       == Handle to this MDI child window.
//             lpClipRect == Rectangle enclosed by tracking box.
//             cxDIB      == Width of DIB.  Won't allow tracking box
//                             to go beyond the width.
//             cyDIB      == Height of DIB.  Won't allow tracking box
//                             to go beyond the height.
//
// History:   Date      Reason
//             ????     Created
//             9/1/91   Added cxDIB/cyDIB to not allow garbage
//                        to be pasted to the clipboard.
//             
//---------------------------------------------------------------------

void TrackMouse (HWND hWnd, LPRECT lpClipRect, int cxDIB, int cyDIB)
{
   HDC   hDC;
   MSG   msg;
   POINT ptOrigin, ptStart;
   RECT  rcClient;

   hDC = GetDC (hWnd);
   SetCapture (hWnd);
   GetClientRect (hWnd, &rcClient);


      // Get mouse coordinates relative to origin of DIB.  Then
      //  setup the clip rectangle accordingly (it already should
      //  contain the starting point in its top/left).

   ptOrigin.x         = GetScrollPos (hWnd, SB_HORZ);
   ptOrigin.y         = GetScrollPos (hWnd, SB_VERT);
   lpClipRect->top   += ptOrigin.x;
   lpClipRect->left  += ptOrigin.y;
   lpClipRect->right  = lpClipRect->left;
   lpClipRect->bottom = lpClipRect->top;
   ptStart.x          = lpClipRect->left;    // Need to remember the
   ptStart.y          = lpClipRect->top;     //  starting point.



      // Display the starting coordinates.

   SetWindowOrg (hDC, ptOrigin.x, ptOrigin.y);
   DrawSelect (hDC, *lpClipRect);


    // Eat mouse messages until a WM_LBUTTONUP is encountered. Meanwhile
    // continue to draw a rubberbanding rectangle and display it's dimensions
    
   for (;;)
      {
      WaitMessage ();

      if (PeekMessage (&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
         {
            // Erase the old.

         DrawSelect (hDC, *lpClipRect);


            // Determine new coordinates.

         lpClipRect->left   = ptStart.x;
         lpClipRect->top    = ptStart.y;
         lpClipRect->right  = LOWORD (msg.lParam) + ptOrigin.x;
         lpClipRect->bottom = HIWORD (msg.lParam) + ptOrigin.y;
         NormalizeRect (lpClipRect);


            // Keep the rectangle within the bounds of the DIB.

         lpClipRect->left   = MAX(lpClipRect->left,   0);
         lpClipRect->top    = MAX(lpClipRect->top,    0);
         lpClipRect->right  = MAX(lpClipRect->right,  0);
         lpClipRect->bottom = MAX(lpClipRect->bottom, 0);

         lpClipRect->left   = MIN(lpClipRect->left,   cxDIB);
         lpClipRect->top    = MIN(lpClipRect->top,    cyDIB);
         lpClipRect->right  = MIN(lpClipRect->right,  cxDIB);
         lpClipRect->bottom = MIN(lpClipRect->bottom, cyDIB);



            // Draw the new rectangle.

         DrawSelect (hDC, *lpClipRect);


            // If the button is released, quit.

         if (msg.message == WM_LBUTTONUP)
         	break;
        }
      else
         continue;
      }


      // Clean up.

   ReleaseCapture ();
   ReleaseDC (hWnd, hDC);
}




//---------------------------------------------------------------------
//
// Function:   NormalizeRect
//
// Purpose:    Insure that the upper/left corner of the rectangle is
//             kept in rect.top/rect.right.  Swaps around coordinates
//             in the rectangle to make sure this is true.
//
//             Code was stolen verbatim from ShowDIB.
//
// Parms:      lpRect == Far pointer to RECT to normalize.
//
// History:   Date      Reason
//             ????     Created
//             
//---------------------------------------------------------------------

void NormalizeRect (LPRECT lpRect)
{
    if (lpRect->right < lpRect->left)
        SWAP (lpRect->right,lpRect->left);

    if (lpRect->bottom < lpRect->top)
        SWAP (lpRect->bottom,lpRect->top);
}



//---------------------------------------------------------------------
//
// Function:   DrawSelect
//
// Purpose:    Draw the rubberbanding rectangle with the specified
//             dimensions on the specified DC.  Rectangle includes
//             a string with its dimensions centered within it.
//
//             Code was stolen almost verbatim from ShowDIB.
//
// Parms:      hDC    == DC to draw into.
//             rcClip == Rectangle to draw.
//
// History:   Date      Reason
//             ????     Created
//             
//---------------------------------------------------------------------

void DrawSelect (HDC hDC, RECT rcClip)
{
   char    szStr[80];
   DWORD   dwExt;
   int     x, y, nLen, dx, dy;
   HDC     hDCBits;
   HBITMAP hBitmap;


      // Don't have anything to do if the rectangle is empty.

   if (IsRectEmpty (&rcClip)) 
      return;


      // Draw rectangular clip region

   PatBlt (hDC, 
            rcClip.left,
            rcClip.top,
            rcClip.right - rcClip.left,
            1,            
            DSTINVERT);

   PatBlt (hDC,
            rcClip.left,
            rcClip.bottom,
            1,
            -(rcClip.bottom - rcClip.top),
            DSTINVERT);

   PatBlt (hDC,
            rcClip.right - 1,
            rcClip.top,
            1,
            rcClip.bottom - rcClip.top,
            DSTINVERT);

   PatBlt (hDC,
            rcClip.right,
            rcClip.bottom - 1,
            -(rcClip.right - rcClip.left),
            1,
            DSTINVERT);


      // Format the dimensions string ...

   wsprintf (szStr,
   	         "%dx%d",
               rcClip.right - rcClip.left,
   	         rcClip.bottom - rcClip.top );
               nLen = lstrlen(szStr);


      // ... and center it in the rectangle

   dwExt   = GetTextExtent (hDC, szStr, nLen);
   dx      = LOWORD (dwExt);
   dy      = HIWORD (dwExt);
   x       = (rcClip.right  + rcClip.left - dx) / 2;
   y       = (rcClip.bottom + rcClip.top  - dy) / 2;
   hDCBits = CreateCompatibleDC (hDC);


      // Output the text to the DC 

   SetTextColor (hDCBits, RGB (255, 255, 255));
   SetBkColor (hDCBits,   RGB (0,   0,   0));

   if (hBitmap = CreateBitmap (dx, dy, 1, 1, NULL))
      {
      hBitmap = SelectObject (hDCBits, hBitmap);

      ExtTextOut (hDCBits, 0, 0, 0, NULL, szStr, nLen, NULL);
      BitBlt (hDC, x, y, dx, dy, hDCBits, 0, 0, SRCINVERT);
      hBitmap = SelectObject (hDCBits, hBitmap);

      DeleteObject (hBitmap);
      }

   DeleteDC (hDCBits);
}



//---------------------------------------------------------------------
//
// Function:   GetCurrentClipRect
//
// Purpose:    Return the rectangular dimensions of the clipboard
//             rectangle for the specified window.
//
// Parms:      hWnd == Window to retrieve the clipboard rectangle info
//                     from.
//
// History:   Date      Reason
//             6/1/91   Created
//             
//---------------------------------------------------------------------

RECT GetCurrentClipRect (HWND hWnd)
{
   RECT      rect = {0,0,0,0};
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;

   if (hWnd)
      {
      hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

      if (hDIBInfo)
         {
         lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);
         rect      = lpDIBInfo->rcClip;
         GlobalUnlock (hDIBInfo);
         }
      }

   return rect;
}



//---------------------------------------------------------------------
//
// Function:   GetCurrentDIBSize
//
// Purpose:    Return the dimensions of the DIB for the given MDI child
//             window.  Dimensions are returned as a POINT.
//
//             If the DIB is stretched on the screen, return the client
//             area of the window.  Otherwise, retrieve the info from
//             the DIBINFO structure stored in the window's words.
//
// Parms:      hWnd == Window to retrieve the DIB dimensions from.
//
// History:   Date      Reason
//             6/1/91   Created
//             
//---------------------------------------------------------------------

POINT GetCurrentDIBSize (HWND hWnd)
{
   HANDLE    hDIBInfo;
   LPDIBINFO lpDIBInfo;
   POINT     pt = {0,0};
   RECT      rcClient;

   if (hWnd)
      {
      hDIBInfo = GetWindowWord (hWnd, WW_DIB_HINFO);

      if (hDIBInfo)
         {
         lpDIBInfo = (LPDIBINFO) GlobalLock (hDIBInfo);

         if (lpDIBInfo->Options.bStretch)
            {
            GetClientRect (hWnd, &rcClient);
            pt.x = rcClient.right;
            pt.y = rcClient.bottom;
            }
         else
            {
            pt.x = lpDIBInfo->wDIBWidth;
            pt.y = lpDIBInfo->wDIBHeight;
            }

         GlobalUnlock (hDIBInfo);
         }
      }

   return pt;
}
