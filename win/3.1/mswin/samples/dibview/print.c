/*************************************************************************

      File:  PRINT.C

   Purpose:  Routines called to print a DIB.  Uses banding or lets GDI
             do the banding.  Works with 3.0 (i.e. via escapes) or 3.1
             (i.e. via printing APIs).

 Functions:  DIBPrint
             BandDIBToPrinter
             PrintABand
             DeviceSupportsEscape
             TranslatePrintRect
             GetPrinterDC
             PrintAbortProc
             PrintAbortDlg
             DoStartDoc
             DoSetAbortProc
             DoStartPage
             DoEndPage
             DoEndDoc
             FindGDIFunction
             ShowPrintError

  Comments:  

   History:   Date      Reason
             6/ 1/91    Created

*************************************************************************/

#include <windows.h>
#include <string.h>
#include <commdlg.h>
#include "dib.h"
#include "print.h"
#include "dibview.h"


// The following typedef's are for printing functions.  They are defined
//  in PRINT.H (!!!!!!!!!!!!!!!!!!!!!!!?????) included with the 3.1
//  SDK -- as this app is supposed to compile in 3.0, I define them
//  here instead.

typedef struct 
   {
   BOOL bGraphics;            // Band includes graphics
   BOOL bText;                // Band includes text.
   RECT GraphicsRect;         // Rectangle to output graphics into.
   }
BANDINFOSTRUCT;


// LPDOCINFO is now defined in 3.1's WINDOWS.H.  We're compiling under
//  both 3.0 and 3.1.  For now, we'll define our own LPDOCINFO here.
//  This is a LESS than adequate solution!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

typedef struct
   {
   short cbSize;
   LPSTR lpszDocName;
   LPSTR lpszOutput;
   }
OURDOCINFO, far * LPOURDOCINFO;



// The following typedef's and string variables are used to link to
//  printing functions on-the-fly in Windows 3.1.  These API are not
//  present in Windows 3.0!  As such, we must use GetModuleHandle() and
//  GetProcAddress() to find and call them.

typedef int (FAR PASCAL *LPSTARTDOC)     (HDC, LPOURDOCINFO);
typedef int (FAR PASCAL *LPSETABORTPROC) (HDC, FARPROC);
typedef int (FAR PASCAL *LPSTARTPAGE)    (HDC);
typedef int (FAR PASCAL *LPENDPAGE)      (HDC);
typedef int (FAR PASCAL *LPENDDOC)       (HDC);


   // The following strings are used to link to function within
   //  GDI on-the-fly.  These functions were added in 3.1.  We
   //  can't call them directly, because that would not allow this
   //  application to run under Windows 3.0.  We, therefore, use
   //  GetModuleHandle()/GetProcAddress() to link to these functions.
   //  See FindGDIFunction() below.

char szGDIModule[]    = "GDI";         // Module name for GDI in Win31.
char szStartDoc[]     = "StartDoc";    // StartDoc() function in GDI.
char szSetAbortProc[] = "SetAbortProc";// SetAbortProc() function in GDI.
char szStartPage[]    = "StartPage";   // StartPage() function in GDI.
char szEndPage[]      = "EndPage";     // EndPage function in GDI.
char szEndDoc[]       = "EndDoc";      // EndDoc function in GDI.




// Globals for this module.

static HWND hDlgAbort    = NULL;        // Handle to abort dialog box.
static char szPrintDlg[] = "PrintDLG";  // Name of Print dialog from .RC
static BOOL bAbort       = FALSE;       // Abort a print operation?
static BOOL gbUseEscapes = TRUE;        // Use Escape() or 3.1 printer APIs?


// Macros

#define ChangePrintPercent(nPct)    SendMessage(hDlgAbort, MYWM_CHANGEPCT, nPct, NULL)



// Function prototypes.

BOOL FAR PASCAL PrintAbortProc (HDC hDC, short code);
int  FAR PASCAL PrintAbortDlg  (HWND hWnd, 
                            unsigned msg, 
                                WORD wParam, 
                                LONG lParam);

DWORD    BandDIBToPrinter    (HDC hPrnDC, 
                            LPSTR lpDIBHdr, 
                            LPSTR lpBits, 
                           LPRECT lpPrintRect);
DWORD    PrintABand          (HDC hDC,
                           LPRECT lpRectOut,
                           LPRECT lpRectClip,
                             BOOL fDoText,
                             BOOL fDoGraphics,
                            LPSTR lpDIBHdr,
                            LPSTR lpDIBBits);
HDC     GetPrinterDC        (void);
void    TranslatePrintRect  (HDC hDC, 
                          LPRECT lpPrintRect, 
                            WORD wUnits,
                            WORD cxDIB,
                            WORD cyDIB);
DWORD    DoStartDoc          (HDC hPrnDC, LPSTR lpszDocName);
DWORD    DoEndPage           (HDC hPrnDC);
DWORD    DoSetAbortProc      (HDC hPrnDC, 
                          FARPROC lpfnAbortProc);
DWORD    DoStartPage         (HDC hPrnDC);
DWORD    DoEndPage           (HDC hPrnDC);
DWORD    DoEndDoc            (HDC hPrnDC);
FARPROC  FindGDIFunction     (LPSTR lpszFnName);
BOOL     DeviceSupportsEscape(HDC hDC, 
                             int nEscapeCode);



//---------------------------------------------------------------------
//
// Function:   DIBPrint
//
// Purpose:    This routine drives the printing operation.  It has the code
//             to handle both banding and non-banding printers.  A banding
//             printer can be distinguished by the GetDeviceCaps() API (see
//             the code below.  On banding devices, must repeatedly call the
//             NEXTBAND escape to get the next banding rectangle to print
//             into.  If the device supports the BANDINFO escape, it should
//             be used to determine whether the band "wants" text or
//             graphics (or both).  On non-banding devices, we can ignore
//             all this and call PrintPage() on the entire page!
//
// Parms:      hDIB        == Handle to global memory with a DIB spec in it.
//                              can be either a Win30 DIB or an OS/2 PM DIB.
//             lpPrintRect == Rect to print (decoded based on next parm)
//             wUnits      == Units lpPrintRect is in (see 
//                              TranslatePrintRect()).
//             dwROP       == Raster operation to use.
//!!!!!!!!!!!!!!!!!!!!dwROP isn't used !!!!!!!!!!!!!!!!!!!!!!
//             fBanding    == TRUE when want to do banding (use NEXTBAND).
//
// Returns:   Encoded error value -- bitwise combination of ERR_PRN_*
//             in PRINT.H.  More than one error can be returned --
//             the application can parse the bits in the DWORD returned,
//             or call ShowPrintError() to display all the errors
//             that ocurred.
//
// History:   Date      Reason
//             6/01/91  Created
//            10/26/91  Added error return codes.
//                      Use DeviceSupportsEscape() instead
//                        of QUERYESCSUPPORT.
//            10/29/91  Added the fUse31APIs flag.
//            11/14/91  Added more error checking.
//                      Added lpDocName as a parameter.
//             
//---------------------------------------------------------------------

DWORD DIBPrint (HANDLE hDIB,
                LPRECT lpPrintRect,
                  WORD wUnits,
                 DWORD dwROP,
                  BOOL fBanding,
                  BOOL fUse31APIs,
                 LPSTR lpszDocName)
{
   HDC            hPrnDC;
   RECT           rect;
   static FARPROC lpAbortProc;
   static FARPROC lpAbortDlg;
   LPSTR          lpDIBHdr, lpBits;
   DWORD          dwErr = ERR_PRN_NONE;


      // Do some setup (like getting pointers to the DIB and its header,
      //  and a printer DC).  Also, set the global gbUseEscapes to force
      //  using printer escapes or the 3.1 printing API.

   if (!hDIB)
      return ERR_PRN_NODIB;

   gbUseEscapes = !fUse31APIs;
   lpDIBHdr     = GlobalLock (hDIB);
   lpBits       = FindDIBBits (lpDIBHdr);

   if (hPrnDC = GetPrinterDC ())
      {
      SetStretchBltMode (hPrnDC, COLORONCOLOR);
      TranslatePrintRect (hPrnDC, 
                          lpPrintRect, 
                          wUnits, 
                          (WORD) DIBWidth (lpDIBHdr),
                          (WORD) DIBHeight (lpDIBHdr));


         // Initialize the abort procedure.  Then STARTDOC.

      lpAbortProc = MakeProcInstance(PrintAbortProc, hInst);
      lpAbortDlg  = MakeProcInstance(PrintAbortDlg,  hInst);
      hDlgAbort   = CreateDialog(hInst, szPrintDlg, GetFocus (), lpAbortDlg);
      bAbort      = FALSE;

      if (dwErr |= DoSetAbortProc (hPrnDC, lpAbortProc))
         goto PRINTERRORCLEANUP;

      if (dwErr |= DoStartDoc (hPrnDC, lpszDocName))
         goto PRINTERRORCLEANUP;

      if (fBanding) 
         dwErr |= BandDIBToPrinter (hPrnDC, lpDIBHdr, lpBits, lpPrintRect);
      else
         {
            // When not doing banding, call PrintABand() to dump the
            //  entire page to the printer in one shot (i.e. give it
            //  a band that covers the entire printing rectangle,
            //  and tell it to print graphics and text).

         rect = *lpPrintRect;

         dwErr |= PrintABand (hPrnDC,
                              lpPrintRect,
                              &rect,
                              TRUE,
                              TRUE,
                              lpDIBHdr,
                              lpBits);
            

            // Non-banding devices need the NEWFRAME or EndPage() call.

         dwErr |= DoEndPage (hPrnDC);
         }



         // End the print operation.  Only send the ENDDOC if
         //   we didn't abort or error.

      if (!bAbort)
         dwErr |= DoEndDoc (hPrnDC);


         // All done, clean up.

PRINTERRORCLEANUP:
      DestroyWindow (hDlgAbort);

      FreeProcInstance(lpAbortProc);
      FreeProcInstance(lpAbortDlg);

      DeleteDC (hPrnDC);
      }
   else
      dwErr |= ERR_PRN_NODC;

   GlobalUnlock (hDIB);

   return dwErr;
}



//---------------------------------------------------------------------
//
// Function:   BandDIBToPrinter
//
// Purpose:    Repeatedly call the NEXTBAND escape to get the next
//             banding rectangle to print into.  If the device supports
//             the BANDINFO escape, use it to determine whether the band
//             wants text or graphics (or both).  For each band, call
//             PrintABand() to do the actual output.
//
// Parms:      hPrnDC   == DC to printer.
//             lpDIBHdr == Ptr to DIB header (BITMAPINFOHEADER or 
//                         BITMAPCOREHEADER)
//             lpBits   == Ptr to DIB's bitmap bits.
//
// Returns:    WORD -- One (or more) of the printer errors defined as
//             ERR_PRN_* in PRINT.H.
//
//             ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//            10/26/91  Chopped out of DIBPrint().
//                      Use DeviceSupportsEscape() instead of
//                        QUERYESCSUPPORT.
//            11/14/91  Added Error return codes ERR_PRN_BANDINFO
//                        and errors from PrintABand.
//            01/22/91  Fixed NEXTBAND error return check (was checking
//                        if != 0, now check if > 0).
//             
//---------------------------------------------------------------------

DWORD BandDIBToPrinter (HDC hPrnDC, 
                      LPSTR lpDIBHdr, 
                      LPSTR lpBits, 
                     LPRECT lpPrintRect)
{
   BANDINFOSTRUCT bi;
   BOOL           bBandInfoDevice;
   RECT           rect;
   DWORD          dwError = ERR_PRN_NONE;
   int            nEscRet;


      // All printers should support the NEXTBAND escape -- we'll
      //  check here, just in case, though!

   if (!DeviceSupportsEscape (hPrnDC, NEXTBAND))
      return ERR_PRN_CANTBAND;


      // Check if device supports the BANDINFO escape.  Then setup
      //  the BANDINFOSTRUCT (we'll use the values we put into it
      //  here later even if the device doesn't support BANDINFO).

   bBandInfoDevice = DeviceSupportsEscape (hPrnDC, BANDINFO);
   bi.bGraphics    = TRUE;
   bi.bText        = TRUE;
   bi.GraphicsRect = *lpPrintRect;


      // Enter the banding loop.  For each band, call BANDINFO if
      //  appropriate.  Then call PrintABand() to do the actual
      //  output.  Terminate loop when NEXTBAND returns an empty rect.

   while (((nEscRet = Escape (hPrnDC, NEXTBAND, NULL, NULL, (LPSTR) &rect)) > 0) &&
         !IsRectEmpty (&rect))
      {
      if (bBandInfoDevice)
         if (!Escape (hPrnDC, 
                      BANDINFO, 
                      sizeof (BANDINFOSTRUCT), 
                      (LPSTR) &bi, 
                      (LPSTR) &bi))
            dwError |= ERR_PRN_BANDINFO;

      dwError |= PrintABand (hPrnDC, 
                             lpPrintRect, 
                             &rect,
                             bi.bText,
                             bi.bGraphics,
                             lpDIBHdr,
                             lpBits);
      }

   if (nEscRet <= 0)
      dwError |= ERR_PRN_NEXTBAND;

   return dwError;
}




//---------------------------------------------------------------------
//
// Function:   PrintABand
//
// Purpose:    This routine does ALL output to the printer.  It is driven by
//             BandDIBToPrinter().  It is called for both banding and non-
//             banding printing devices.  lpClipRect contains the rectangular
//             area we should do our output into (i.e. we should clip our
//             output to this area).  The flags fDoText and fDoGraphics
//             should be set appropriately (if we want any text output to
//             the rectangle, set fDoText to true).  Normally these flags
//             are returned on banding devices which support the BANDINFO
//             escape.  On non-banding devices, all output goes to the
//             entire page, so this routine is passes a rectangle for
//             the entire page, and fDoText = fDoGraphics = TRUE.
//
//             This routine is also responsible for doing stretching of
//             the DIB.  As such, the lpRectOut parameter points to a
//             rectangle on the printed page where the entire DIB will
//             fit -- the DIB is stretched appropriately to fit in this
//             rectangle.
//
//             After printing a band, updates the print % shown in the
//             abort dialog box.
//
// Parms:      hDC         == DC to do output into.
//             lpRectOut   == Rectangle on DC DIB should fit in.
//             lpRectClip  == Rectangle to output during THIS call.
//             fDoText     == Output text into this rectangle (unused by DIBView)?
//             fDoGraphics == Output graphics into this rectangle?
//             lpDIBHdr    == Pointer to DIB's header (either a
//                              BITMAPINFOHEADER or a BITMAPCOREHEADER)
//             lpDIBBits   == Pointer to the DIB's bitmap bits.
//
// Returns:    One or more of the ERR_PRN_* errors in PRINT.H (or'd
//             together. ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

DWORD PrintABand (HDC hDC,
               LPRECT lpRectOut,
               LPRECT lpRectClip,
                 BOOL fDoText,
                 BOOL fDoGraphics,
                LPSTR lpDIBHdr,
                LPSTR lpDIBBits)
{
   int    nxLogPix, nyLogPix;
   RECT   rect;
   double dblXScaling, dblYScaling;
   DWORD  dwError = ERR_PRN_NONE;


   if (fDoGraphics)
      {
      nxLogPix = GetDeviceCaps (hDC, LOGPIXELSX);
      nyLogPix = GetDeviceCaps (hDC, LOGPIXELSY);

      dblXScaling = ((double) lpRectOut->right - lpRectOut->left) / 
                     DIBWidth (lpDIBHdr);
      dblYScaling = ((double) lpRectOut->bottom - lpRectOut->top) /
                     DIBHeight (lpDIBHdr);


         // Now we set up a temporary rectangle -- this rectangle
         //  holds the coordinates on the paper where our bitmap
         //  WILL be output.  We can intersect this rectangle with
         //  the lpClipRect to see what we NEED to output to this
         //  band.  Then, we determine the coordinates in the DIB
         //  to which this rectangle corresponds (using dbl?Scaling).

      IntersectRect (&rect, lpRectOut, lpRectClip);

      if (!IsRectEmpty (&rect))
         {
         RECT rectIn;
         int  nPct;

         rectIn.left   = (int) ((rect.left - lpRectOut->left) / 
                                 dblXScaling + 0.5);
         rectIn.top    = (int) ((rect.top  - lpRectOut->top) / 
                                 dblYScaling + 0.5);
         rectIn.right  = (int) (rectIn.left + (rect.right  - rect.left) / 
                                 dblXScaling + 0.5);
         rectIn.bottom = (int) (rectIn.top  +  (rect.bottom - rect.top) / 
                                 dblYScaling + 0.5);

            // Could just always call StretchDIBits() below, but
            //  we want to give SetDIBitsToDevice() a work out, too!

         if ((rect.right - rect.left == rectIn.right - rectIn.left) &&
             (rect.bottom - rect.top == rectIn.bottom - rectIn.top))
            {
            if (!SetDIBitsToDevice (hDC,                            // DestDC
                                    rect.left,                      // DestX
                                    rect.top,                  // DestY
                                    rect.right - rect.left,    // DestWidth
                                    rect.bottom - rect.top,    // DestHeight
                                    rectIn.left,               // SrcX
                                    (int) DIBHeight (lpDIBHdr)-// SrcY
                                       rectIn.top - 
                                       (rectIn.bottom - rectIn.top),
                                    0,                         // nStartScan
                                    (int) DIBHeight (lpDIBHdr),// nNumScans
                                    lpDIBBits,                 // lpBits
                                    (LPBITMAPINFO) lpDIBHdr,   // lpBitInfo
                                    DIB_RGB_COLORS))           // wUsage
               dwError |= ERR_PRN_SETDIBITSTODEV;
            }
         else
            {
            if (!StretchDIBits (hDC,                           // DestDC
                                rect.left,                     // DestX
                                rect.top,                      // DestY
                                rect.right - rect.left,        // DestWidth
                                rect.bottom - rect.top,        // DestHeight
                                rectIn.left,                   // SrcX
                                (int) DIBHeight (lpDIBHdr) -   // SrcY
                                   rectIn.top - 
                                   (rectIn.bottom - rectIn.top),
                                rectIn.right - rectIn.left,    // SrcWidth
                                rectIn.bottom - rectIn.top,    // SrcHeight
                                lpDIBBits,                     // lpBits
                                (LPBITMAPINFO) lpDIBHdr,       // lpBitInfo
                                DIB_RGB_COLORS,                // wUsage
                                SRCCOPY))                      // dwROP
               dwError |= ERR_PRN_STRETCHDIBITS;
            }


            // Change percentage of print shown in abort dialog.

         nPct = MulDiv (rect.bottom, 
                        100, 
                        lpRectOut->bottom);
         ChangePrintPercent (nPct);
         }
      }

   return dwError;
}



//---------------------------------------------------------------------
//
// Function:   DeviceSupportsEscape
//
// Purpose:    Uses QUERYESCSUPPORT to see if the given device
//             supports the given escape code.
//
// Parms:      hDC         == Device to check if escape is supported on.
//             nEscapeCode == Escape code to check for.
//
// History:   Date      Reason
//            10/26/91  Created
//             
//---------------------------------------------------------------------

BOOL DeviceSupportsEscape (HDC hDC, int nEscapeCode)
{
   return Escape(hDC, QUERYESCSUPPORT, sizeof(int), (LPSTR) &nEscapeCode, NULL);
}



//---------------------------------------------------------------------
//
// Function:   TranslatePrintRect
//
// Purpose:    Given a rectangle and what units that rectangle is in,
//             translate the rectangle to the appropriate value in
//             device units.
//
// Parms:      hDC         == DC translation is relative to.
//             lpPrintRect == Pointer to rectangle to translate.
//             wUnits      == Units lpPrintRect is in:
//                              UNITS_INCHES == Units are in inches, stretch
//                                                DIB to this size on page.
//                              UNITS_STRETCHTOPAGE == lpPrintRect doesn't
//                                                matter, stretch DIB to
//                                                fill the entire page.
//                              UNITS_BESTFIT == lpPrintRect doesn't matter,
//                                                stretch DIB as much as
//                                                possible horizontally,
//                                                and preserve its aspect
//                                                ratio vertically.
//                              UNITS_SCALE == lpPrintRect->top is factor to
//                                                stretch vertically.
//                                                lpPrintRect->left is
//                                                factor to stretch horiz.
//                              UNITS_PIXELS == lpPrintRect is in pixels.
//             cxDIB       == DIB's width.
//             cyDIB       == DIB's height.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

void TranslatePrintRect (HDC hDC, 
                      LPRECT lpPrintRect, 
                        WORD wUnits,
                        WORD cxDIB,
                        WORD cyDIB)
{
   int cxPage, cyPage, cxInch, cyInch;

   if (!hDC)
      return;

   cxPage = GetDeviceCaps (hDC, HORZRES);
   cyPage = GetDeviceCaps (hDC, VERTRES);
   cxInch = GetDeviceCaps (hDC, LOGPIXELSX);
   cyInch = GetDeviceCaps (hDC, LOGPIXELSY);

   switch (wUnits)
      {
         // lpPrintRect contains units in inches.  Convert to pixels.

      case UNITS_INCHES:
         lpPrintRect->top    *= cyInch;
         lpPrintRect->left   *= cxInch;
         lpPrintRect->bottom *= cyInch;
         lpPrintRect->right  *= cxInch;
         break;


         // lpPrintRect contains no pertinent info -- create a rectangle
         //  which covers the entire printing page.

      case UNITS_STRETCHTOPAGE:
         lpPrintRect->top    = 0;
         lpPrintRect->left   = 0;
         lpPrintRect->bottom = cyPage;
         lpPrintRect->right  = cxPage;
         break;


         // lpPrintRect contains no pertinent info -- create a rectangle
         //  which preserves the DIB's aspect ratio, and fills the page
         //  horizontally.  NOTE:  Assumes DIB is 1 to 1 aspect ratio,
         //  could use biXPelsPerMeter in a DIB to munge these values
         //  for non 1 to 1 aspect ratio DIBs (I've never seen such
         //  a beast, though)!

      case UNITS_BESTFIT:
         lpPrintRect->top    = 0;
         lpPrintRect->left   = 0;
         lpPrintRect->bottom = (int)(((double) cyDIB * cyPage * cyInch) /
                                     ((double) cxDIB * cxInch));
         lpPrintRect->right  = cxPage;
         break;



         // lpPrintRect's top/left contain multipliers to multiply the
         //  DIB's height/width by.

      case UNITS_SCALE:
         {
         int cxMult, cyMult;

         cxMult              = lpPrintRect->left;
         cyMult              = lpPrintRect->top;
         lpPrintRect->top    = 0;
         lpPrintRect->left   = 0;
         lpPrintRect->bottom = cyDIB * cyMult;
         lpPrintRect->right  = cxDIB * cxMult;
         }


         // lpPrintRect already contains device units, don't touch it.

      case UNITS_PIXELS:
      default:
         // Don't touch the units...
         break;
      }
}





//---------------------------------------------------------------------
//
// Function:   GetPrinterDC
//
// Purpose:    Returns a DC to the currently selected printer.  Returns
//             NULL on error.
//
// Parms:      None
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

HDC GetPrinterDC (void)
{
   PRINTDLG pd;

   pd.lStructSize          = sizeof (pd);
   pd.hwndOwner            = NULL;
   pd.hDevMode             = NULL;
   pd.hDevNames            = NULL;
   pd.hDC                  = NULL;
   pd.Flags                = PD_RETURNDC | PD_RETURNDEFAULT;
   pd.nFromPage            = 0;
   pd.nToPage              = 0;
   pd.nMinPage             = 0;
   pd.nMaxPage             = 0;
   pd.nCopies              = 0;
   pd.hInstance            = NULL;
   pd.lCustData            = NULL;
   pd.lpfnPrintHook        = NULL;
   pd.lpfnSetupHook        = NULL;
   pd.lpPrintTemplateName  = NULL;
   pd.lpSetupTemplateName  = NULL;
   pd.hPrintTemplate       = NULL;
   pd.hSetupTemplate       = NULL;

   if (PrintDlg (&pd))
      return pd.hDC;
   else
      return NULL;
}






//-------------------- Abort Routines ----------------------------


//---------------------------------------------------------------------
//
// Function:   PrintAbortProc
//
// Purpose:    Abort procedure while printing is occurring.  Registered
//             with Windows via the SETABORTPROC escape.  This routine
//             is called regularly by the sytem during a print operation.
//
//             By putting a PeekMessage loop here, multitasking can occur.
//             PeekMessage will yield to other apps if they have messages
//             waiting for them.
//
//             Doesn't bother if the global, bAbort, is set.  This var
//             is set by PrintAbortDlg() when a user cancels a print
//             operation.
//
// Parms:      hDC  == DC printing is being done to
//             code == Error code (see docs for SETABORTPROC printer
//                      escape).
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

BOOL FAR PASCAL PrintAbortProc(HDC hDC, short code)
{
   MSG msg;

   bAbort |= (code != 0);

   while (!bAbort && PeekMessage (&msg, 0, 0, 0, PM_REMOVE))
      if (!IsDialogMessage (hDlgAbort, &msg))
         {
         TranslateMessage (&msg);
         DispatchMessage (&msg);
         }

   return (!bAbort);
}



//---------------------------------------------------------------------
//
// Function:   PrintAbortDlg
//
// Purpose:    Dialog box window procedure for the "cancel" dialog
//             box put up while DIBView is printing.
//
//             Functions sets bAbort (a global variable) to true
//             if the user aborts the print operation.  Other functions
//             in this module then "do the right thing."
//
//             Also handles MYWM_CHANGEPCT to change % done displayed
//             in dialog box.
//
// Parms:      hWnd    == Handle to this dialog box.
//             message == Message for window.
//             wParam  == Depends on message.
//             lParam  == Depends on message.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

int FAR PASCAL PrintAbortDlg(HWND hWnd, unsigned msg, WORD wParam, LONG lParam)
{
   switch (msg)
      {
      case WM_INITDIALOG:
         SetFocus(hWnd);
         return TRUE;


      case WM_COMMAND:
         bAbort = TRUE;
         return TRUE;


      case MYWM_CHANGEPCT:
         {
         char szBuf[20];

         wsprintf (szBuf, "%3d%% done", wParam);
         SetDlgItemText (hWnd, IDD_PRNPCT, szBuf);
         return TRUE;
         }
      }

   return FALSE;
}



//---------------------------------------------------------------------
//
// Function:   DoStartDoc
//
// Purpose:    Called at the beginning of printing a document.  Does
//             the "right thing," depending on whether we're using
//             3.0 style printer escapes, or the 3.1 printing API
//             (i.e. either does an Escape(STARTDOC) or StartDoc()).
//
//             Note that it uses FindGDIFunction() to find the address
//             of StartDoc() we can't just put a call to StartDoc()
//             here because we want this .EXE file to be compatible with
//             Windows 3.0 as well as 3.1.  3.0 didn't have a function
//             "StartDoc()!"
//
// Parms:      hPrnDC == DC to printer
//
// Returns:    An error code defined as ERR_PRN_* in PRINT.H:
//                ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//            11/14/91  Added error return code.
//             
//---------------------------------------------------------------------

DWORD DoStartDoc (HDC hPrnDC, LPSTR lpszDocName)
{
   if (gbUseEscapes)
      {
      if (Escape (hPrnDC, STARTDOC, lstrlen (lpszDocName), 
                  lpszDocName, NULL) < 0)
         return ERR_PRN_STARTDOC;
      }
   else
      {
      LPSTARTDOC Win31StartDoc;
      OURDOCINFO DocInfo;

      Win31StartDoc       = (LPSTARTDOC) FindGDIFunction (szStartDoc);
      DocInfo.cbSize      = sizeof (DocInfo);
      DocInfo.lpszDocName = lpszDocName;
      DocInfo.lpszOutput  = NULL;

      if (Win31StartDoc)
         {
         if (Win31StartDoc (hPrnDC, &DocInfo) < 0)
            return ERR_PRN_STARTDOC;
         }
      else
         return ERR_PRN_NOFNSTARTDOC;
      }

   return ERR_PRN_NONE;
}




//---------------------------------------------------------------------
//
// Function:   DoSetAbortProc
//
// Purpose:    Called at the beginning of printing a document.  Does
//             the "right thing," depending on whether we're using
//             3.0 style printer escapes, or the 3.1 printing API
//             (i.e. either does an Escape(SETABORTPROC) or SetAbortProc()).
//
//             Note that it uses FindGDIFunction() to find the address
//             of SetAbortProc() we can't just put a call to SetAbortProc()
//             here because we want this .EXE file to be compatible with
//             Windows 3.0 as well as 3.1.  3.0 didn't have a function
//             "SetAbortProc()!"
//
// Parms:      hPrnDC == DC to printer
//
// Returns:    An error code defined as ERR_PRN_* in PRINT.H:
//                ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//            11/14/91  Added error return code.
//             
//---------------------------------------------------------------------

DWORD DoSetAbortProc (HDC hPrnDC, FARPROC lpfnAbortProc)
{
   LPSETABORTPROC Win31SetAbortProc;

   if (gbUseEscapes)
      {
      if (Escape(hPrnDC, SETABORTPROC, NULL, (LPSTR) lpfnAbortProc, NULL) < 0)
         return ERR_PRN_SETABORTPROC;
      }
   else
      {
      Win31SetAbortProc = (LPSETABORTPROC) FindGDIFunction (szSetAbortProc);
      if (Win31SetAbortProc)
         {
         if (Win31SetAbortProc (hPrnDC, lpfnAbortProc) < 0)
           return ERR_PRN_SETABORTPROC;
         }
      else
         return ERR_PRN_NOFNSETABORTPROC;
      }

   return ERR_PRN_NONE;
}



//---------------------------------------------------------------------
//
// Function:   DoStartPage
//
// Purpose:    Called at the beginning of printing a page.  Does
//             the "right thing," depending on whether we're using
//             3.0 style printer escapes, or the 3.1 printing API.
//             Routine does nothing under 3.0 or when using the 3.0
//             Escapes, as there was no equivalent to StartPage()
//             in 3.0.
//
//             Note that it uses FindGDIFunction() to find the address
//             of StartPage() we can't just put a call to StartPage()
//             here because we want this .EXE file to be compatible with
//             Windows 3.0 as well as 3.1.  3.0 didn't have a function
//             "StartPage()!"
//
// Parms:      hPrnDC == DC to printer
//
// Returns:    An error code defined as ERR_PRN_* in PRINT.H:
//                ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//            11/14/91  Added error return code.
//             
//---------------------------------------------------------------------

DWORD DoStartPage (HDC hPrnDC)
{
   LPSTARTPAGE Win31StartPage;

   if (!gbUseEscapes)
      {
      Win31StartPage = (LPSTARTPAGE) FindGDIFunction (szStartPage);
      if (Win31StartPage)
         {
         if (!Win31StartPage (hPrnDC))
            return ERR_PRN_STARTPAGE;
         }
      else
         return ERR_PRN_NOFNSTARTPAGE;
      }

   return ERR_PRN_NONE;
}



//---------------------------------------------------------------------
//
// Function:   DoEndPage
//
// Purpose:    Called at the end of printing a page.  Does the
//             "right thing," depending on whether we're using
//             3.0 style printer escapes, or the 3.1 printing API
//             (i.e. either does an Escape(NEWFRAME) or EndPage()).
//
//             Note that it uses FindGDIFunction() to find the address
//             of EndPage() we can't just put a call to EndPage()
//             here because we want this .EXE file to be compatible with
//             Windows 3.0 as well as 3.1.  3.0 didn't have a function
//             "EndPage()!"
//
// Parms:      hPrnDC == DC to printer
//
// Returns:    An error code defined as ERR_PRN_* in PRINT.H:
//                ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//            11/14/91  Added error return code.
//             
//---------------------------------------------------------------------

DWORD DoEndPage (HDC hPrnDC)
{
   LPENDPAGE Win31EndPage;

   if (gbUseEscapes)
      {
      if (Escape (hPrnDC, NEWFRAME, NULL, NULL, NULL) < 0)
         return ERR_PRN_NEWFRAME;
      }
   else
      {
      Win31EndPage = (LPENDPAGE) FindGDIFunction (szEndPage);
      if (Win31EndPage)
         {
         if (Win31EndPage (hPrnDC) < 0)
            return ERR_PRN_NEWFRAME;
         }
      else
         return ERR_PRN_NOFNENDPAGE;
      }

   return ERR_PRN_NONE;
}



//---------------------------------------------------------------------
//
// Function:   DoEndDoc
//
// Purpose:    Called at the end of printing a document.  Does
//             the "right thing," depending on whether we're using
//             3.0 style printer escapes, or the 3.1 printing API
//             (i.e. either does an Escape(ENDDOC) or EndDoc()).
//
//             Note that it uses FindGDIFunction() to find the address
//             of EndDoc() we can't just put a call to EndDoc()
//             here because we want this .EXE file to be compatible with
//             Windows 3.0 as well as 3.1.  3.0 didn't have a function
//             "EndDoc()!"
//
// Parms:      hPrnDC == DC to printer
//
// Returns:    An error code defined as ERR_PRN_* in PRINT.H:
//                ERR_PRN_NONE (0) if no error.
//
// History:   Date      Reason
//             6/01/91  Created
//            11/14/91  Added error return code.
//             
//---------------------------------------------------------------------

DWORD DoEndDoc (HDC hPrnDC)
{
   LPENDDOC Win31EndDoc;

   if (gbUseEscapes)
      {
      if (Escape(hPrnDC, ENDDOC, NULL, NULL, NULL) < 0)
         return ERR_PRN_ENDDOC;
      }
   else
      {
      Win31EndDoc = (LPENDDOC) FindGDIFunction (szEndDoc);
      if (Win31EndDoc)
         {
         if (Win31EndDoc (hPrnDC) < 0)
            return ERR_PRN_ENDDOC;
         }
      else
         return ERR_PRN_NOFNENDDOC;
      }

   return ERR_PRN_NONE;
}




//---------------------------------------------------------------------
//
// Function:   FindGDIFunction
//
// Purpose:    Uses GetModuleHandle() and GetProcAddress() to find
//             a given function inside GDI itself.  This is useful
//             to "link" to functions on-the-fly that are only
//             present in Windows 3.1.  If we were to call these
//             functions directly from this EXE file, Windows 3.0 would
//             complain that we are not calling valid functions.
//
// Parms:      lpszFnName == Name of function within GDI.EXE to find
//                            an address for.
//
// History:   Date      Reason
//             6/01/91  Created
//             
//---------------------------------------------------------------------

FARPROC FindGDIFunction (LPSTR lpszFnName)
{
   HANDLE hGDI;

   hGDI = GetModuleHandle (szGDIModule);

   if (!hGDI)
      return NULL;

   return GetProcAddress (hGDI, lpszFnName);
}





//---------------------------------------------------------------------
//
// Function:   ShowPrintError
//
// Purpose:    Decode a printing error and display a message box
//             which describes what the error is.
//
//             Errors are stored in a bitwise fashion, so we 
//             check all the valid error bits in the error, and
//             display a messagebox for each error.
//
// Parms:      hWnd   == Parent for message box which shows error.
//             wError == Error bitfield (see ERR_PRN_* in PRINT.H).
//
// History:   Date      Reason
//            11/14/91  Created
//             
//---------------------------------------------------------------------

void ShowPrintError (HWND hWnd, DWORD dwError)
{
   char szError[100];
   int  i = 0;

   if (dwError == ERR_PRN_NONE)
      {
      if (LoadString (hInst, IDS_PRN_NONE, szError, 100))
         MessageBox (hWnd, szError, NULL, MB_OK);
      return;
      }

   while (dwError)
      {
      i++;

      if (dwError & 1)
         {
         if (LoadString (hInst, i + IDS_PRN_NONE, szError, 100))
            MessageBox (hWnd, szError, NULL, MB_OK);
         else
            MessageBeep (0);
         }

      dwError >>= 1;
      }
}



