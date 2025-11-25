//********************************************************************
//
//  print.c
//
//  Source file for Device-Independent Bitmap (DIB) API.  Provides
//  the following functions:
//
//  PrintWindow()       - Prints all or part of a window
//  PrintScreen()       - Prints the entire screen
//  PrintDIB()          - Prints the specified DIB
//
// Written by Microsoft Product Support Services, Developer Support.
// Copyright (C) 1991-1996 Microsoft Corporation. All rights reserved.
//********************************************************************

#define     STRICT      // enable strict type checking

#include <windows.h>
#include <string.h>
#include "dibdll.h"     // Header for printing dialog & DLL instance handle
#include "dibapi.h"     // Header for DIB functions
#include "dibutil.h"    // Auxiliary functions


extern HANDLE   ghDLLInst;      // Global handle to DLL's instance

/***************************************************************
 * Typedefs
 **************************************************************/

// Structure used for Banding

typedef struct
{
   BOOL bGraphics;
   BOOL bText;
   RECT GraphicsRect;
} BANDINFOSTRUCT;


/****************************************************************
 * Variables
 ***************************************************************/

HWND hDlgAbort;                    // Handle to Abort Dialog
char szPrintDlg[] = "PRINTING";    // Name of Print dialog from .RC
BOOL bAbort = FALSE;               // Abort a print operation?
char gszDevice[50];                // Keeps track out device (e.g. "HP LaserJet")
char gszOutput[50];                // Output device (e.g. "LPT1:")

/***************************************************************
 * Function prototypes for functions local to this module
 **************************************************************/

BOOL CALLBACK PrintAbortProc(HDC, int);
int CALLBACK PrintAbortDlg(HWND, UINT, WPARAM, LPARAM);
WORD PrintBand(HDC, LPRECT, LPRECT, BOOL, BOOL, LPBITMAPINFOHEADER, LPSTR);
HDC GetPrinterDC(void);
void CalculatePrintRect(HDC, LPRECT, WORD, DWORD, DWORD);


/**********************************************************************
 *
 * PrintWindow()
 *
 *
 * Description:
 *
 * This function prints the specified window on the default
 * printer.
 *
 * Parameters:
 *
 * HWND hWnd       - Specifies the window to print.  The window must
 *                   not be iconic and must be topmost on the display.
 *
 * WORD fPrintArea - Specifies the area of the window to print.  Must be
 *                   one of PW_ALL, PW_CLIENT, PW_CAPTION,  or PW_MENUBAR
 *
 * WORD fPrintOpt  - Print options (one of PW_BESTFIT, PW_STRETCHTOPAGE, or
 *                   PW_SCALE)
 *
 * WORD wXScale, wYScale - X and Y scaling factors if PW_SCALE is specified
 *
 * LPSTR szJobName - Name that you would like to give to this print job (this
 *                   name shows up in the Print Manager as well as the
 *                   "Now Printing..." dialog box).
 * Return Value:
 *      ERR_DIBFUNCTION or any return value from PrintDIB
 *
 **********************************************************************/


WORD PrintWindow(HWND hWnd, WORD fPrintArea, WORD fPrintOpt, WORD wXScale,
        WORD wYScale, LPSTR szJobName)
{
    HDIB    hDib;          // Handle to the DIB
    WORD    wReturn;       // our return value

    // Parameter validation

    if (!hWnd)
        return ERR_INVALIDHANDLE;  // Invalid Window

    // Copy the Window to a DIB and print it.

    hDib = CopyWindowToDIB(hWnd, fPrintArea);
    if (!hDib)
        return ERR_DIBFUNCTION; // CopyWindowToDIB failed!

    wReturn = PrintDIB(hDib, fPrintOpt, wXScale, wYScale, szJobName);

    // Call DestroyDIB to free the memory the dib takes up.

    DestroyDIB(hDib);
    return wReturn;   // return the value from PrintDIB
}


/**********************************************************************
 *
 * PrintScreen()
 *
 *
 * Description:
 *
 * This function prints the specified portion of the display screen on the
 * default printer using the print options specified.  The print
 * options are listed in dibapi.h.
 *
 * Parameters:
 *
 * LPRECT rRegion  - Specifies the region of the screen (in screen
 *                   coordinates) to print
 *
 * WORD fPrintOpt  - Print options  (PW_BESTFIT, PW_STRETCHTOPAGE, or PW_SCALE)
 *
 * WORD wXScale, wYScale - X and Y scaling factors if PW_SCALE is specified
 *
 * LPSTR szJobName - Name that you would like to give to this print job (this
 *                   name shows up in the Print Manager as well as the
 *                   "Now Printing..." dialog box).
 *
 * Return Value:
 *      ERR_DIBFUNCTION or any return value from PrintDIB
 *
 **********************************************************************/

WORD PrintScreen(LPRECT rRegion, WORD fPrintOpt, WORD wXScale, WORD wYScale,
        LPSTR szJobName)
{
    HDIB     hDib;          // A Handle to our DIB
    WORD     wReturn;       // Return value

    // Copy the screen contained in the specified rectangle to a DIB

    hDib = CopyScreenToDIB(rRegion);

    if (!hDib)
        return ERR_DIBFUNCTION;   // CopyScreenToDIB failed!

    wReturn = PrintDIB(hDib, fPrintOpt, wXScale, wYScale, szJobName);
    DestroyDIB(hDib);
    return wReturn; // Return the value that PrintDIB returned
}


/**********************************************************************
 *
 * PrintDIB()
 *
 * Description:
 *
 * This routine prints the specified DIB.  The actual printing is done
 * in the PrintBand() routine (see below), this procedure drives the
 * printing operation.  PrintDIB() has the code to handle both banding
 * and non-banding printers.  A banding printer can be distinguished by
 * the GetDeviceCaps() API (see the code below).  On banding devices,
 * must repeatedly call the NEXTBAND escape to get the next banding
 * rectangle to print into.  If the device supports the BANDINFO escape,
 * it should be used to determine whether the band "wants" text or
 * graphics (or both).  On non-banding devices, we can ignore all this
 * and call PrintBand() on the entire page.
 *
 * Parameters:
 *
 * HDIB hDib       - Handle to dib to be printed
 *
 * WORD fPrintOpt  - tells which print option to use (PW_BESTFIT,
 *                   PW_STRETCHTOPAGE, OR PW_SCALE)
 *
 * WORD wXScale, wYScale - X and Y scaling factors (integers) for
 *                   printed output if the PW_SCALE option is used.
 *
 * LPSTR szJobName - Name that you would like to give to this print job (this
 *                   name shows up in the Print Manager as well as the
 *                   "Now Printing..." dialog box).
 *
 * Return Value:  (see errors.h for description)
 *
 * One of: ERR_INVALIDHANDLE
 *         ERR_LOCK
 *         ERR_SETABORTPROC
 *         ERR_STARTDOC
 *         ERR_NEWFRAME
 *         ERR_ENDDOC
 *         ERR_GETDC
 *         ERR_STRETCHDIBITS
 *
 ********************************************************************/

WORD PrintDIB(HDIB hDib, WORD fPrintOpt, WORD wXScale, WORD wYScale,
        LPSTR szJobName)
{
    HDC                 hPrnDC;         // DC to the printer
    RECT                rect;           // Rect structure used for banding
    LPSTR               lpBits;         // pointer to the DIB bits
    LPBITMAPINFOHEADER  lpDIBHdr;       // Pointer to DIB header
    int                 nBandCount = 0; // used for print dialog box to count bands
    WORD                wErrorCode = 0; // Error code to return
    RECT                rPrintRect;     // specifies the area on the printer
                                        // (in printer coordinates) which we
                                        // want the DIB to go to
    char                szBuffer[70];   // Buffer to hold message for "Printing" dlg box
    char                szJobNameTrunc[35];     // szJobName truncated to 31
                                        // characters, since STARTDOC can't
                                        // accept a string longer than 31
    DOCINFO             DocInfo;        // structure for StartDoc
    int                 nTemp;          // used to check banding capability
    CHAR                lpBuffer[128];  // Buffer for strings retrieved from resources


    // Paramter validation

    if (!hDib)
        return ERR_INVALIDHANDLE;

    // Get pointer to DIB header

    lpDIBHdr = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    if (!lpDIBHdr) // Check that we have a valid pointer
        return ERR_LOCK;
    lpBits = FindDIBBits((LPSTR)lpDIBHdr); // Find pointer to DIB bits

    if (hPrnDC = GetPrinterDC())
    {
        SetStretchBltMode(hPrnDC, COLORONCOLOR);

        // Determine rPrintRect (printer area to print to) from the
        // fPrintOpt.  Fill in rPrintRect.left and .top from wXScale and
        // wYScale just in case we use PW_SCALE (see the function
        // CalculatePrintRect).

        rPrintRect.left = wXScale;
        rPrintRect.top = wYScale;
        CalculatePrintRect(hPrnDC, &rPrintRect, fPrintOpt, lpDIBHdr->biWidth,
                         lpDIBHdr->biHeight);

        // Initialize the abort procedure.

        hDlgAbort = CreateDialog(ghDLLInst, szPrintDlg, GetFocus(),
                (DLGPROC)PrintAbortDlg);

        // ISet the text inside the dialog to the name of our print job

        lstrcpy(szJobNameTrunc, szJobName);
        szJobNameTrunc[31] = '\0';           // Truncate string to 31 chars
        LoadString(ghDLLInst, IDS_PRINTMSG, lpBuffer, sizeof(lpBuffer));
        wsprintf(szBuffer, lpBuffer, (LPSTR)szJobNameTrunc);
        SetDlgItemText(hDlgAbort, IDC_PRINTTEXT1, (LPSTR)szBuffer);

        // Set global variable bAbort to FALSE.  This will get set to TRUE
        // in our PrintAbortDlg() procedure if the user selects the
        // CANCEL button in our dialog box

        bAbort = FALSE;

        // set up the Abort Procedure

        if (SetAbortProc(hPrnDC, (ABORTPROC)PrintAbortProc) < 0)
                return ERR_SETABORTPROC;

        // start print job

        ZeroMemory(&DocInfo, sizeof(DOCINFO));
        DocInfo.cbSize = sizeof(DOCINFO);
        DocInfo.lpszDocName = (LPTSTR)szJobNameTrunc;
        DocInfo.lpszOutput = NULL;

        if (StartDoc(hPrnDC, &DocInfo) <= 0)
        {
            // Oops, something happened, let's clean up here and return

             DestroyWindow(hDlgAbort);   // Remove abort dialog box
             DeleteDC(hPrnDC);
             GlobalUnlock(hDib);
             return ERR_STARTDOC;
        }

// Note: the following banding code applies to Windows 3.1.  With the new
//       printing architecture of Win32, send out both the graphics and
//       text in one band (like a non-banding device).  This code is used
//       for Win32s since Win32s depends on Windows 3.1 printing architecture.
//
        // Check if need to do banding.  If we do, loop through
        // each band in the page, calling NEXTBAND and BANDINFO
        // (if supported) calling PrintBand() on the band.  Else,
        // call PrintBand() with the entire page as our clipping
        // rectangle!

        // If Wincap32 is running on Win32s, then use banding

        nTemp = NEXTBAND;
        if (Escape(hPrnDC, QUERYESCSUPPORT, sizeof(int), (LPSTR)&nTemp, NULL) &&
                (GetVersion() & 0x80000000) && (LOWORD(GetVersion()) == 3))
        {
            BOOL                bBandInfoDevice;
            BANDINFOSTRUCT      biBandInfo;         // Used for banding

            // Fill in initial values for our BandInfo Structure to
            // tell driver we can want to do graphics and text, and
            // also which area we want the graphics to go in.

            biBandInfo.bGraphics = TRUE;
            biBandInfo.bText = TRUE;
            biBandInfo.GraphicsRect = rPrintRect;

            // Check if device supports the BANDINFO escape.

            nTemp = BANDINFO;
            bBandInfoDevice = Escape(hPrnDC, QUERYESCSUPPORT, sizeof(int),
                    (LPSTR)&nTemp, NULL);

            // Do each band -- Call Escape() with NEXTBAND, then the
            // rect structure returned is the area where we are to
            // print in.  This loop exits when the rect area is empty.

            while (Escape(hPrnDC, NEXTBAND, 0, NULL, (LPSTR)&rect) && !
                IsRectEmpty(&rect))
            {
                char szTmpBuf[100];


                // Do the BANDINFO, if needed.

                if (bBandInfoDevice)
                    Escape(hPrnDC, BANDINFO, sizeof(BANDINFOSTRUCT), (LPSTR)&
                            biBandInfo, (LPSTR)&biBandInfo);
                LoadString(ghDLLInst, IDS_BANDNMBR, lpBuffer, sizeof(lpBuffer));
                wsprintf(szTmpBuf, lpBuffer, ++nBandCount);
                SetDlgItemText(hDlgAbort, IDC_PERCENTAGE, (LPSTR)szTmpBuf);

                // Call PrintBand() to do actual output into band.
                // Pass in our band-info flags to tell what sort
                // of data to output into the band.  Note that on
                // non-banding devices, we pass in the default bandinfo
                // stuff set above (i.e. bText=TRUE, bGraphics=TRUE).

                wErrorCode = PrintBand(hPrnDC, &rPrintRect, &rect,
                        biBandInfo.bText, biBandInfo.bGraphics, lpDIBHdr,
                        lpBits);
            }
        }
        else
        {
            // Print the whole page -- non-banding device.

            if (StartPage(hPrnDC) <= 0)
                return ERR_STARTPAGE;

            rect = rPrintRect;
            LoadString(ghDLLInst, IDS_SENDINGBAND, lpBuffer, sizeof(lpBuffer));
            SetDlgItemText(hDlgAbort, IDC_PERCENTAGE, lpBuffer);
            wErrorCode = PrintBand(hPrnDC, &rPrintRect, &rect, TRUE, TRUE,
                    lpDIBHdr, lpBits);

            // Non-banding devices need a NEWFRAME

            if (EndPage(hPrnDC) <= 0)
                return ERR_ENDPAGE;
        }


        // End the print operation.  Only send the ENDDOC if
        //  we didn't abort or error.

        if (!bAbort)
        {
            // We errored out on ENDDOC, but don't return here - we still
            // need to close the dialog box, free proc instances, etc.

            if (EndDoc(hPrnDC) <= 0)
                wErrorCode = ERR_ENDDOC;

            DestroyWindow(hDlgAbort);
        }

        // All done, clean up.

        DeleteDC(hPrnDC);
    }
    else
        wErrorCode = ERR_GETDC;   // Couldn't get Printer DC!

    GlobalUnlock(hDib);
    return wErrorCode;
}




// *******************************************************************
// Auxiliary Functions
//     -- Local to this module only
// *******************************************************************


/*********************************************************************
 *
 * CalculatePrintRect()
 *
 * Given fPrintOpt and a size of the DIB, return the area on the
 * printer where the image should go (in printer coordinates).  If
 * fPrintOpt is PW_SCALE, then lpPrintRect.left and .top should
 * contain WORDs which specify the scaling factor for the X and
 * Y directions, respecively.
 *
 ********************************************************************/

void CalculatePrintRect(HDC hDC, LPRECT lpPrintRect, WORD fPrintOpt,
        DWORD cxDIB, DWORD cyDIB)
{
    int  cxPage, cyPage, cxInch, cyInch;

    if (!hDC)
        return;

    // Get some info from printer driver

    cxPage = GetDeviceCaps(hDC, HORZRES);   // Width of printr page - pixels
    cyPage = GetDeviceCaps(hDC, VERTRES);   // Height of printr page - pixels
    cxInch = GetDeviceCaps(hDC, LOGPIXELSX);// Printer pixels per inch - X
    cyInch = GetDeviceCaps(hDC, LOGPIXELSY);// Printer pixels per inch - Y

    switch (fPrintOpt)
    {

        // Best Fit case -- create a rectangle which preserves
        // the DIB's aspect ratio, and fills the page horizontally.

        // The formula in the "->bottom" field below calculates the Y
        // position of the printed bitmap, based on the size of the
        // bitmap, the width of the page, and the relative size of
        // a printed pixel (cyInch / cxInch).

        case PW_BESTFIT:
            lpPrintRect->top = 0;
            lpPrintRect->left = 0;
            lpPrintRect->bottom = (int)(((double)cyDIB * cxPage * cyInch) /
                    ((double)cxDIB * cxInch));
            lpPrintRect->right = cxPage;
            break;

        // Scaling option -- lpPrintRect's top/left contain
        // multipliers to multiply the DIB's height/width by.

        case PW_SCALE:
        {
            int     cxMult, cyMult;

            cxMult = lpPrintRect->left;
            cyMult = lpPrintRect->top;
            lpPrintRect->top = 0;
            lpPrintRect->left = 0;
            lpPrintRect->bottom = (int)(cyDIB * cyMult);
            lpPrintRect->right = (int)(cxDIB * cxMult);
            break;
        }

        // Stretch To Page case -- create a rectangle
        // which covers the entire printing page (note that this
        // is also the default).

        case PW_STRETCHTOPAGE:

        default:
            lpPrintRect->top = 0;
            lpPrintRect->left = 0;
            lpPrintRect->bottom = cyPage;
            lpPrintRect->right = cxPage;
            break;
    }
}


/*********************************************************************
 *
 * PrintBand()
 *
 * This routine does ALL output to the printer.  It is called from
 * the PrintDIB() routine.  It is called for both banding and non-
 * banding printing devices.  lpRectClip contains the rectangular
 * area we should do our output into (i.e. we should clip our output
 * to this area).  The flags fDoText and fDoGraphics should be set
 * appropriately (if we want any text output to the rectangle, set
 * fDoText to true).  Normally these flags are returned on banding
 * devices which support the BANDINFO escape.
 *
 ********************************************************************/

WORD PrintBand(HDC hDC, LPRECT lpRectOut, LPRECT lpRectClip, BOOL fDoText,
        BOOL fDoGraphics, LPBITMAPINFOHEADER lpDIBHdr, LPSTR lpDIBBits)
{
    RECT    rect;           // Temporary rectangle
    double  dblXScaling,    // X and Y scaling factors
            dblYScaling;
    WORD    wReturn = 0;    // Return code

    if (fDoGraphics)
    {
        dblXScaling = ((double)lpRectOut->right - lpRectOut->left) / (double)
                lpDIBHdr->biWidth;
        dblYScaling = ((double)lpRectOut->bottom - lpRectOut->top) / (double)
                lpDIBHdr->biHeight;

        // Now we set up a temporary rectangle -- this rectangle
        // holds the coordinates on the paper where our bitmap
        // WILL be output.  We can intersect this rectangle with
        // the lpClipRect to see what we NEED to output to this
        // band.  Then, we determine the coordinates in the DIB
        // to which this rectangle corresponds (using dbl?Scaling).

        IntersectRect(&rect, lpRectOut, lpRectClip);
        if (!IsRectEmpty(&rect))
        {
            RECT    rectIn;

            rectIn.left = (int)((rect.left - lpRectOut->left) / dblXScaling +
                    0.5
                       );
            rectIn.top = (int)((rect.top - lpRectOut->top) / dblYScaling + 0.5);
            rectIn.right = (int)(rectIn.left + (rect.right - rect.left) /
                    dblXScaling + 0.5);
            rectIn.bottom = (int)(rectIn.top + (rect.bottom - rect.top) /
                    dblYScaling + 0.5);
            if (!StretchDIBits(hDC, rect.left, rect.top,
                    rect.right - rect.left, rect.bottom - rect.top,
                    rectIn.left, (int)(lpDIBHdr->biHeight) -
                    rectIn.top - (rectIn.bottom - rectIn.top),
                    rectIn.right - rectIn.left, rectIn.bottom - rectIn.top,
                    lpDIBBits, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS,
                    SRCCOPY))
                wReturn = ERR_STRETCHDIBITS; // StretchDIBits() failed!
        }
    }

    return wReturn;
}


/***********************************************************************
 *
 * GetPrinterDC()
 *
 * Uses PrinDlg common dialog for printer selection and creates a dc.
 * Returns NULL on error.
 *
 ***********************************************************************/

HDC GetPrinterDC() {

    PRINTDLG pd;

    ZeroMemory(&pd, sizeof(pd));

    pd.lStructSize = sizeof(PRINTDLG);
    pd.Flags = PD_RETURNDC;

    if (PrintDlg(&pd) == TRUE)
    {
        DEVNAMES    *pDevNames = GlobalLock(pd.hDevNames);

        lstrcpy((LPSTR)gszDevice,
                (LPSTR)((char *)pDevNames+pDevNames->wDeviceOffset));

        if(!lstrcmpi((LPSTR)((char *)pDevNames+pDevNames->wDeviceOffset),
                (LPSTR)((char *)pDevNames+pDevNames->wOutputOffset)))
            lstrcpy((LPSTR)gszOutput, "net:");
        else
            lstrcpy((LPSTR)gszOutput,
                    (LPSTR)((char *)pDevNames+pDevNames->wOutputOffset));

        GlobalUnlock(pd.hDevNames);
        return pd.hDC;
    }

    else
        return NULL;
}


/**********************************************************************
 * PrintAbortProc()
 *
 * Abort procedure - contains the message loop while printing is
 * in progress.  By using a PeekMessage() loop, multitasking
 * can occur during printing.
 *
 **********************************************************************/

BOOL CALLBACK PrintAbortProc(HDC hDC, int code)
{
    MSG  msg;

    while (!bAbort && PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        if (!IsDialogMessage(hDlgAbort, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    return !bAbort;
}

/***********************************************************************
 *
 * PrintAbortDlg()
 *
 *
 * This is the Dialog Procedure which will handle the "Now Printing"
 * dialog box.  When the user presses the "Cancel" button, the
 * global variable bAbort is set to TRUE, which causes the
 * PrintAbortProc to exit, which in turn causes the printing
 * operation to terminate.
 *
 ***********************************************************************/

int CALLBACK PrintAbortDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            char szBuffer[100];
            CHAR msgBuffer[128];


            // Fill in the text which specifies where this bitmap
            // is going ("on HP LaserJet on LPT1", for example)
            LoadString(ghDLLInst, IDS_ABORTSTRING, msgBuffer, sizeof(msgBuffer));
            wsprintf(szBuffer, msgBuffer, (LPSTR)gszDevice,
                    (LPSTR)gszOutput);
            SetDlgItemText(hWnd, IDC_PRINTTEXT2, (LPSTR)szBuffer);
            SetFocus(GetDlgItem(hWnd, IDCANCEL));
            return TRUE;     // Return TRUE because we called SetFocus()
        }

        case WM_COMMAND:
            bAbort = TRUE;
            DestroyWindow(hWnd);
            return TRUE;
            break;
    }

    return FALSE;
}
