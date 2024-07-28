/*************************************************************************

      File:  FRAME.C

   Purpose:  Routines needed to support the frame window, menus, etc.

 Functions:  FrameWndProc
             DoCommands
             OpenDIBWindow
             OpenPaletteWindow
             EnableWindowOptionsMenus

  Comments:  

   History:   Date     Reason

            06/01/91   Created
            01/28/92   IDM_PALDIB bug fix : delete palette
                         returned by CurrentDIBPalette().

*************************************************************************/

#include <windows.h>
#include "child.h"
#include "dibview.h"
#include "file.h"
#include "palette.h"
#include "frame.h"
#include "init.h"
#include "dib.h"
#include "errors.h"
#include "options.h"
#include "print.h"
#include "clipbrd.h"
#include "capture.h"
#include "about.h"


typedef HWND FAR *LPHWND;


// Local function prototypes

void OpenPaletteWindow (LPSTR szTitle, HWND hOwner, HPALETTE hPal);
long DoCommands(HWND hWnd,
                WORD wParam, 
                LONG lParam);



char  szDIBPalTitle[] = "DIB Palette";       // Title on DIB Palette Window
char  szSysPalTitle[] = "System Palette";    // Title on System Palette Wnd
HMENU hFrameMenu      = NULL;                // Menu handle to frame menu.
HWND  hFrameWnd       = NULL;




//---------------------------------------------------------------------
//
// Function:   FrameWndProc
//
// Purpose:    Window procedure for MDI frame window.
//             Handles all messages destined for the frame.
//
// Parms:      hWnd    == Handle to the frame window.
//             message == Message for window.
//             wParam  == Depends on message.
//             lParam  == Depends on message.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

long FAR PASCAL FrameWndProc(HWND hWnd, 
			     UINT message,
			     WPARAM wParam,
			     LPARAM lParam)
{
   static BOOL bPalDev;         // Display device can animate palettes?

   switch (message) 
   {
      case WM_CREATE:
         {
         CLIENTCREATESTRUCT ccs;
         HDC                hDC;

            // Find window menu where children will be listed

         ccs.hWindowMenu  = GetSubMenu (GetMenu (hWnd), WINDOW_MENU);
         ccs.idFirstChild = IDM_WINDOWCHILD;


            // Create the MDI client filling the client area

         hWndMDIClient = CreateWindow (
                           "mdiclient",
                           NULL,
                           WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
                           0,
                           0,
                           0,
                           0,
                           hWnd,
                           0xCAC,
                           hInst,
                           (LPSTR)&ccs);

         ShowWindow (hWndMDIClient, SW_SHOW);

         hDC        = GetDC (NULL);
         bPalDev    = GetDeviceCaps (hDC, RASTERCAPS) & RC_PALETTE;
         hFrameMenu = GetMenu (hWnd);
         hFrameWnd  = hWnd;
         ReleaseDC (NULL, hDC);

         break;
         }



         // Initialize pull down menus.  Gray the File/SaveAs+Print
         //  if there is no current MDI child window.  Also, gray the
         //  palette options when appropriate (i.e. gray when non-palette
         //  device, gray animate if one is being animated or if there
         //  are no MDI child windows, gray restore if none are being
         //  animated).  The "Window" menu will also be grayed if there
         //  are no MDI children.

      case WM_INITMENUPOPUP:
         {
         HWND  hCurWnd;

         hCurWnd = GetCurrentMDIWnd ();

         EnableMenuItem (hFrameMenu, 
                         IDM_SAVE, 
                         MF_BYCOMMAND | (hCurWnd ? MF_ENABLED : MF_GRAYED));
         EnableMenuItem (hFrameMenu, 
                         IDM_PRINT,
                         MF_BYCOMMAND | (hCurWnd ? MF_ENABLED : MF_GRAYED));
         EnableMenuItem (hFrameMenu, 
                         IDM_OPTIONS,
                         MF_BYCOMMAND | (hCurWnd ? MF_ENABLED : MF_GRAYED));
         EnableMenuItem (hFrameMenu, 
                         IDM_PALDIB,
                         MF_BYCOMMAND | (hCurWnd ? MF_ENABLED : MF_GRAYED));
         EnableMenuItem (hFrameMenu, 
                         IDM_COPY,
                         MF_BYCOMMAND | (hCurWnd ? MF_ENABLED : MF_GRAYED));
         EnableMenuItem (hFrameMenu, 
                         IDM_PASTE,
                         MF_BYCOMMAND | 
                          IsClipboardFormatAvailable (CF_DIB) ||
                          IsClipboardFormatAvailable (CF_BITMAP) ? 
                             MF_ENABLED : MF_GRAYED);
         EnableMenuItem (hFrameMenu, 
                         IDM_PALANIMATE, 
                         MF_BYCOMMAND | 
                          (bPalDev && !hWndAnimate && hCurWnd ? MF_ENABLED : MF_GRAYED));
         EnableMenuItem (hFrameMenu, 
                         IDM_PALRESTORE, 
                         MF_BYCOMMAND | 
                          (bPalDev && hWndAnimate ? MF_ENABLED : MF_GRAYED));
         break;
         }




         // Go handle all WM_COMMAND messages in DoCommands().

      case WM_COMMAND:
         return DoCommands (hWnd, wParam, lParam);



         // Palette changed message -- someone changed the palette
         //  somehow.  We must make sure that all the MDI child windows
         //  realize their palettes here.

      case WM_PALETTECHANGED:
         {
         static BOOL bInPalChange = FALSE;

         if (bInPalChange)
            break;

         bInPalChange = TRUE;
         SendMessageToAllChildren (hWndMDIClient, message, wParam, lParam);
         bInPalChange = FALSE;
         break;
         }



         // We get a QUERYNEWPALETTE message when our app gets the
         //  focus.  We need to realize the currently active MDI
         //  child's palette as the foreground palette for the app.
         //  We do this by sending our MYWM_QUERYNEWPALETTE message
         //  to the currently active child's window procedure.  See
         //  the comments in CHILD.C (at the top of the file) for
         //  more info on this.

      case WM_QUERYNEWPALETTE:
         {
         HWND hActive;

         hActive = GetCurrentMDIWnd ();

         if (hActive)
            return SendMessage (hActive, MYWM_QUERYNEWPALETTE, hWnd, 0L);

         return FALSE;
         }


         // Terminate this app by posting a WM_QUIT message.

      case WM_DESTROY:
         PostQuitMessage(0);
         break;



         // We didn't handle, pass to DefFrameProc.

      default:
         return DefFrameProc (hWnd,hWndMDIClient, message, wParam, lParam);
    }

    return (NULL);
}



//---------------------------------------------------------------------
//
// Function:   DoCommands
//
// Purpose:    WM_COMMAND handler for MDI frame window.
//
// Parms:      hWnd    == Handle to this MDI child window.
//             wParam  == Depends on command.
//             lParam  == Depends on command.
//
// History:   Date      Reason
//             6/01/91  Created
//            10/26/91  Removed "message" parm (unused)
//                      Fixed bugs in IDM_PRINT:  was
//                        assigning incorrect values to
//                        wUnits for PRINT_BESTFIT and
//                        PRINT_STRETCH. 
//            10/29/91  Added passing of bUse31APIs to
//                        DIBPrint() in IDM_PRINT case.
//            10/30/91  Added capture code.
//            11/15/91  Added szFileName parm to OpenDIBFile
//                        in IDM_OPEN.  Added use of IDS_CAPTURE
//                        string table entry to IDM_CAPT*.
//            01/28/92  IDM_PALDIB bug fix : delete palette
//                        returned by CurrentDIBPalette().
//             
//---------------------------------------------------------------------

long DoCommands(HWND hWnd,
                WORD wParam, 
                LONG lParam)
{
   switch (wParam) 
   {

         // Put up the About box.

      case IDM_ABOUT:
         {
         FARPROC lpDlgProc;


         lpDlgProc = MakeProcInstance(AboutDlg, hInst);

         DialogBox(hInst,             // current instance
                   szAboutDLG,        // resource to use 
                   hWnd,              // parent handle   
                   lpDlgProc);        // About() instance address

         FreeProcInstance(lpDlgProc);
         break;
         }



         // Open up a DIB MDI child window.

      case IDM_OPEN:
         {
         HANDLE hDIB;
         char   szFileName [MAX_FILENAME];

         hDIB = OpenDIBFile (szFileName);

         if (hDIB)
            OpenDIBWindow (hDIB, szFileName);
         break;
         }




      case IDM_PRINT:
         {
         HWND        hDIBWnd;
         HANDLE      hDIB;
         RECT        rect;
         HANDLE      hDIBInfo;
         LPDIBINFO   lpDIBInfo;
         WORD        cxDIB, cyDIB, wUnits, cxScale, cyScale;
         BOOL        bUseBanding, bUse31APIs;
         char        lpszDocName [MAX_FILENAME];

         hDIBWnd = GetCurrentMDIWnd ();

         if (!hDIBWnd)
            break;

         hDIBInfo = GetWindowWord (hDIBWnd, WW_DIB_HINFO);

         if (!hDIBInfo)
            break;

         lpDIBInfo   = (LPDIBINFO) GlobalLock (hDIBInfo);
         hDIB        = lpDIBInfo->hDIB;
         cxDIB       = lpDIBInfo->wDIBWidth;
         cyDIB       = lpDIBInfo->wDIBWidth;
         wUnits      = lpDIBInfo->Options.wPrintOption;
         cxScale     = lpDIBInfo->Options.wXScale;
         cyScale     = lpDIBInfo->Options.wYScale;
         bUseBanding = lpDIBInfo->Options.bPrinterBand;
         bUse31APIs  = lpDIBInfo->Options.bUse31PrintAPIs;
         lstrcpy (lpszDocName, lpDIBInfo->szFileName);
         GlobalUnlock (hDIBInfo);

         switch (wUnits)
            {
            case PRINT_BESTFIT:
               wUnits = UNITS_BESTFIT;
               // rect is filled in by DIBPrint().
               break;

            case PRINT_STRETCH:
               wUnits = UNITS_STRETCHTOPAGE;
               // rect is filled in by DIBPrint().
               break;

            case PRINT_SCALE:
               wUnits      = UNITS_SCALE;
               rect.left   = cxScale;
               rect.top    = cyScale;
               // rect is modified to reflect actual width in DIBPrint.
               break;

            default:
               wUnits      = UNITS_PIXELS;
               rect.left   = 0;
               rect.top    = 0;
               rect.right  = cxDIB;
               rect.bottom = cyDIB;
               break;
            }

         if (hDIB)
            {
            DWORD dwError;

            if (dwError = DIBPrint (hDIB,
                                    &rect,
                                    wUnits,
                                    SRCPAINT,
                                    bUseBanding,
                                    bUse31APIs,
                                    lpszDocName))
               ShowPrintError (hWnd, dwError);
            }
         break;
         }


         // Save the current DIB to disk.

      case IDM_SAVE:
         SaveDIBFile ();
         break;


         // Handle the clipboard copy operation.

      case IDM_COPY:
         HandleCopyClipboard ();
         break;


         // Handle the clipboard paste operation.

      case IDM_PASTE:
         HandlePasteClipboard ();
         break;


         // Put up the DIB palette window if it isn't already.
         //  Remember to copy the DIB's palette, since if the
         //  DIB window is destroyed, the palette is destroyed
         //  with it.  Also, remember to change the palette flags
         //  so if the DIB is being animated, the palette window's
         //  palette doesn't have the PC_RESERVED flag set.

      case IDM_PALDIB:
         {
         HPALETTE hPalDIB, hNewPal;

         hPalDIB = CurrentDIBPalette ();

         if (!hPalDIB)
            break;

         hNewPal = CopyPaletteChangingFlags (hPalDIB, 0);
         DeleteObject (hPalDIB);

         OpenPaletteWindow (szDIBPalTitle, hWnd, hNewPal);
         break;
         }



         // Put up the system palette window if it isn't already.

      case IDM_PALSYS:
         OpenPaletteWindow (szSysPalTitle, hWnd, GetSystemPalette ());
         break;



         // Animate the DIB's palette.

      case IDM_PALANIMATE:
         {
         HWND hMDIChild;

         hMDIChild = GetCurrentMDIWnd ();

         if (hMDIChild)
            SendMessage (hMDIChild, MYWM_ANIMATE, 0, 0L);
         break;
         }


         // Restore the DIB's palette (after animation) to its original
         //  state.

      case IDM_PALRESTORE:
         {
         HWND hMDIChild;

         hMDIChild = GetCurrentMDIWnd ();

         if (hMDIChild)
            SendMessage (hMDIChild, MYWM_RESTOREPALETTE, 0, 0L);
         break;
         }



         // Tile MDI windows

      case IDM_WINDOWTILE:
         SendMessage (hWndMDIClient, WM_MDITILE, 0, 0L);
         break;



         // Cascade MDI windows

      case IDM_WINDOWCASCADE:
         SendMessage (hWndMDIClient, WM_MDICASCADE, 0, 0L);
         break;



         // Auto - arrange MDI icons

      case IDM_WINDOWICONS:
         SendMessage (hWndMDIClient, WM_MDIICONARRANGE, 0, 0L);
         break;



         // Close all MDI child windows.

      case IDM_WINDOWCLOSEALL:
         CloseAllDIBWindows ();
         break;



         // User wants to see the stretch dialog box.

      case IDM_OPTIONS:
         {
         HWND        hDIBWnd;
         HANDLE      hDIB;
         HANDLE      hDIBInfo;
         LPDIBINFO   lpDIBInfo;
         OPTIONSINFO OptionsInfo;
         BOOL        bOldStretch;

         hDIBWnd = GetCurrentMDIWnd ();

         if (!hDIBWnd)
            break;

         hDIBInfo = GetWindowWord (hDIBWnd, WW_DIB_HINFO);

         if (!hDIBInfo)
            break;


            // Set up data for stretch dialog box.

         lpDIBInfo   = (LPDIBINFO) GlobalLock (hDIBInfo);
         hDIB        = lpDIBInfo->hDIB;
         OptionsInfo = lpDIBInfo->Options;
         bOldStretch = OptionsInfo.bStretch;

         if (hDIB)
            {
            ShowOptions (hWnd, &OptionsInfo);
            lpDIBInfo->Options = OptionsInfo;
            }


            // If the stretch option changed, need to repaint.

         if (lpDIBInfo->Options.bStretch != bOldStretch)
            InvalidateRect (hDIBWnd, NULL, bOldStretch);

         GlobalUnlock (hDIBInfo);
         break;
         }


         // User wants to perform a capture operation.  Call the
         //  appropriate routine in CAPTURE.C for what the user
         //  wants to do.  If we get a handle to a DIB back, open
         //  up a new MDI child.

      case IDM_CAPTCLIENT:
      case IDM_CAPTWINDOW:
      case IDM_CAPTFULLSCREEN:
         {
         HANDLE      hDIB;
         char        szWindowText[20], szCapture[20]; 
         static WORD wWinNumber = 0;      // Capture Window number.

         if (wParam == IDM_CAPTFULLSCREEN)
            hDIB = CaptureFullScreen (hWnd);
         else
            hDIB = CaptureWindow (hWnd, (wParam == IDM_CAPTCLIENT));

         if (hDIB)
            {
               // Open up a new MDI child with the specified window title

            wWinNumber   = (wWinNumber + 1) % 10;
            szCapture[0] = '\0';
            LoadString (hInst, IDS_CAPTURE, szCapture, 20);

            wsprintf (szWindowText, szCapture, wWinNumber);

            OpenDIBWindow (hDIB, szWindowText);
            }

         break;
         }



         // Toggle the "Hide on Capture" option.

      case IDM_CAPTUREHIDE:
         {
         BOOL  bHide;
         HMENU hMenu;

         bHide = ToggleCaptureHide ();
         hMenu = GetMenu (hWnd);
         CheckMenuItem (hMenu, IDM_CAPTUREHIDE, MF_BYCOMMAND | 
                       (bHide ? MF_CHECKED : MF_UNCHECKED));
         break;
         }



         // User wants to exit DIBView.  Send ourselves a WM_CLOSE
         //  message (WM_CLOSE does some necessary clean up operations).

      case IDM_EXIT:
         SendMessage (hWnd, WM_CLOSE, 0, 0L);
         break;



         // Must be some system command -- pass it on to the default
         //  frame window procedure.

      default:
         return DefFrameProc(hWnd, hWndMDIClient, WM_COMMAND, wParam, lParam);
   }

   return NULL;
}





//---------------------------------------------------------------------
//
// Function:   OpenDIBWindow
//
// Purpose:    This routine opens up an MDI child window.  The child
//             window will be sized to the height/width of the DIB.
//
// Parms:      hDIB    == Handle to the DIB to put in the MDI child.
//             szTitle == Title for title bar (must be a valid DOS
//                        filename.
//
// History:   Date      Reason
//             6/01/91  Created
//            10/15/91  Got rid of use of handle for
//                      DIBCREATEINFO, it's kosher to
//                      pass a far pointer instead.
//             
//---------------------------------------------------------------------

HWND OpenDIBWindow (HANDLE hDIB, LPSTR szTitle)
{
   LPSTR           lpDIB;
   MDICREATESTRUCT mcs;
   HWND            hChild;
   DWORD           dwDIBHeight, dwDIBWidth;
   DIBCREATEINFO   DIBCreateInfo;
   RECT            rcWindow;

   SetCursor(LoadCursor(NULL, IDC_WAIT));

   if (hDIB)
      {
      lpDIB       = GlobalLock (hDIB);
      dwDIBHeight = DIBHeight (lpDIB);
      dwDIBWidth  = DIBWidth (lpDIB);
      GlobalUnlock (hDIB);
      }

   DIBCreateInfo.hDIB = hDIB;
   lstrcpy (DIBCreateInfo.szFileName, szTitle);


      // Determine the necessary window size to hold the DIB.

   rcWindow.left   = 0;
   rcWindow.top    = 0;
   rcWindow.right  = (int) dwDIBWidth;
   rcWindow.bottom = (int) dwDIBHeight;

   AdjustWindowRect (&rcWindow, WS_CHILD|WS_SYSMENU|WS_CAPTION|
                     WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX, FALSE);


      // Setup to call WM_MDICREATE.

   mcs.szTitle = (LPSTR) szTitle;
   mcs.szClass = szMDIChild;
   mcs.hOwner  = hInst;
   mcs.x       = CW_USEDEFAULT;
   mcs.y       = CW_USEDEFAULT;
   mcs.cx      = rcWindow.right - rcWindow.left;
   mcs.cy      = rcWindow.bottom - rcWindow.top;
   mcs.style   = WS_HSCROLL | WS_VSCROLL;
   mcs.lParam  = (LONG) (LPDIBCREATEINFO) &DIBCreateInfo;


      // If no other DIBs are being displayed, always force this
      //  window to the upper left corner of the MDI client.

   if (nDIBsOpen == 0)
      {
      mcs.x = 0;
      mcs.y = 0;
      }


      // Tell the MDI Client to create the child.

   hChild = (WORD) SendMessage (hWndMDIClient,
                                WM_MDICREATE,
                                0,
                                (LONG) (LPMDICREATESTRUCT) &mcs);

   if (!hChild)
      {
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      DIBError (ERR_CREATECHILD);
      return NULL;
      }

   SetCursor(LoadCursor(NULL, IDC_ARROW));
   return hChild;
}





//---------------------------------------------------------------------
//
// Function:   OpenPaletteWindo9w
//
// Purpose:    This routine opens up a popup window which displays
//             a palette.
//
//             Note that all the popup window's routines are in
//             PALETTE.C.
//
// Parms:      szTitle == Title for title bar.
//             hOwner  == Handle to the Window which owns this popup
//             hPal    == Palette to display.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------


void OpenPaletteWindow (LPSTR szTitle, HWND hOwner, HPALETTE hPal)
{
   HWND      hWnd;
   HDC       hDC;
   int       cxScr, cyScr;

   hDC        = GetDC (NULL);
   cxScr      = GetDeviceCaps (hDC, HORZRES);
   cyScr      = GetDeviceCaps (hDC, VERTRES);
   ReleaseDC (NULL, hDC);

   hWnd = CreateWindow (szPalClass,                // Class name
                        szTitle,                   // Title
                        WS_POPUP |                 // Styles -- Popup Window
                         WS_THICKFRAME |           //  Thick (resizable) frame
                         WS_SYSMENU |              //  Has a system menu
                         WS_CLIPSIBLINGS |         //  Clips other siblings
                         WS_CLIPCHILDREN |         //  Clips its children
                         WS_MAXIMIZEBOX |          //  Maximize button
                         WS_MINIMIZEBOX |          //  Minimize button
                         WS_VSCROLL |              //  Vertical scroll bar
                         WS_CAPTION,               //  Caption
                        0,                         // x
                        0,                         // y
                        cxScr / 4,                 // Width
                        cyScr / 4,                 // Height
                        hOwner,                    // Parent
                        NULL,                      // Menu
                        hInst,                     // Instance
                        NULL);                     // Extra parms

   SetPaletteWindowsPal (hWnd, hPal);

   ShowWindow (hWnd, SW_SHOWNORMAL);
   UpdateWindow (hWnd);
}




//---------------------------------------------------------------------
//
// Function:   EnableWindowOptionsMenus
//
// Purpose:    Enable or gray the "Window" and "Options" pull down 
//             menus.
//
// Parms:      bEnable == TRUE if "Window" should be enabled,
//                        FALSE if it should be gray.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

void EnableWindowAndOptionsMenus (BOOL bEnable)
{
   EnableMenuItem (hFrameMenu, 
                   WINDOW_MENU,
                   MF_BYPOSITION | (bEnable ? MF_ENABLED : MF_GRAYED));

   EnableMenuItem (hFrameMenu, 
                   IDM_OPTIONS,
                   MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));

   DrawMenuBar (hFrameWnd);
}
