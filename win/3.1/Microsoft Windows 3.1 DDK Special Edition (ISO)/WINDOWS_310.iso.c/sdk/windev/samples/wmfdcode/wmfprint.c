/***********************************************************************

  MODULE     : WMFPRINT.C

  FUNCTIONS  : PrintWMF
               GetPrinterDC
               AbortProc
               AbortDlg

  COMMENTS   :

************************************************************************/

#include "windows.h"
#include "wmfdcode.h"

/***********************************************************************

  FUNCTION   : PrintWMF

  PARAMETERS : void

  PURPOSE    : draw the metafile on a printer dc

  CALLS      : WINDOWS
                 wsprintf
                 MessageBox
                 MakeProcInstance
                 Escape
                 CreateDialog
                 SetMapMode
                 SetViewportOrg
                 SetViewportExt
                 EnableWindow
                 PlayMetaFile
                 DestroyWindow
                 DeleteDC

               APP
                 WaitCursor
                 GetPrinterDC
                 SetPlaceableExts
                 SetClipMetaExts

  MESSAGES   : none

  RETURNS    : BOOL - 0 if unable to print 1 if successful

  COMMENTS   :

  HISTORY    : 1/16/91 - created - drc - modification of code originally
               contained in SDK sample app PRNTFILE

************************************************************************/

BOOL PrintWMF()
{
  char str[50];

  /* display the hourglass cursor */
  WaitCursor(TRUE);

  /* get a DC for the printer */
  hPr = GetPrinterDC();

  /* if a DC could not be created then report the error and return */
  if (!hPr) {
    WaitCursor(FALSE);
    wsprintf((LPSTR)str, "Cannot print %s", (LPSTR)fnameext);
    MessageBox(hWndMain, (LPSTR)str, NULL, MB_OK | MB_ICONHAND);
    return (FALSE);
  }

  /* get a proc address for the abort dialog and procedure */
  lpAbortDlg =  MakeProcInstance(AbortDlg, hInst);
  lpAbortProc = MakeProcInstance(AbortProc, hInst);

  /* define the abort function */
  Escape(hPr, SETABORTPROC, NULL,
        (LPSTR) (long) lpAbortProc,
        (LPSTR) NULL);

  /* start the print job */
  if (Escape(hPr, STARTDOC, 4, "Metafile", (LPSTR) NULL) < 0)  {
    MessageBox(hWndMain, "Unable to start print job",
               NULL, MB_OK | MB_ICONHAND);
    FreeProcInstance(lpAbortDlg);
    FreeProcInstance(lpAbortProc);
                        DeleteDC(hPr);
  }

  /* clear the abort flag */
  bAbort = FALSE;

  /* Create the Abort dialog box (modeless) */
  hAbortDlgWnd = CreateDialog(hInst, "AbortDlg", hWndMain, lpAbortDlg);

  /* if the dialog was not created report the error */
  if (!hAbortDlgWnd) {
    WaitCursor(FALSE);
    MessageBox(hWndMain, "NULL Abort window handle",
               NULL, MB_OK | MB_ICONHAND);
    return (FALSE);
  }

  /* show Abort dialog */
  ShowWindow (hAbortDlgWnd, SW_NORMAL);

  /* disable the main window to avoid reentrancy problems */
  EnableWindow(hWndMain, FALSE);
  WaitCursor(FALSE);

  /* if we are still committed to printing */
  if (!bAbort) {

    /* if this is a placeable metafile then set its origins and extents */
    if (bAldusMeta)
        SetPlaceableExts(hPr, aldusMFHeader, WMFPRINTER);

    /* if this is a metafile contained within a clipboard file then set
       its origins and extents accordingly */
    if ( (bMetaInRam) && (!bAldusMeta) )
        SetClipMetaExts(hPr, MFP, WMFPRINTER);

    /* if this is a "traditional" windows metafile */
    if (!bMetaInRam)
        {
          SetMapMode(hPr, MM_TEXT);
          SetViewportOrg(hPr, 0, 0);

          /* set the extents to the driver supplied values for horizontal
             and vertical resolution */
          SetViewportExt(hPr, GetDeviceCaps(hPr, HORZRES),
                              GetDeviceCaps(hPr, VERTRES) );
        }

    /* play the metafile directly to the printer.  No enumeration involved
       here */
    PlayMetaFile(hPr, hMF);
  }

  /* end the print job */
  Escape(hPr, NEWFRAME, 0, 0L, 0L);
  Escape(hPr, ENDDOC, 0, 0L, 0L);

  EnableWindow(hWndMain, TRUE);

  /* destroy the Abort dialog box */
  DestroyWindow(hAbortDlgWnd);

  FreeProcInstance(lpAbortDlg);
  FreeProcInstance(lpAbortProc);

  DeleteDC(hPr);

  return(TRUE);
}

/***********************************************************************

  FUNCTION   : GetPrinterDC

  PARAMETERS : void

  PURPOSE    : Get hDc for current device on current output port according
               to info in WIN.INI.

  CALLS      : WINDOWS
                 GetProfileString
                 AnsiNext
                 CreateDC

  MESSAGES   : none

  RETURNS    : HANDLE - hDC > 0 if success  hDC = 0 if failure

  COMMENTS   : Searches WIN.INI for information about what printer is
               connected, and if found, creates a DC for the printer.

  HISTORY    : 1/16/91 - created - originally from the PRNTFILE sample
               supplied with the SDK

************************************************************************/

HANDLE GetPrinterDC()
{
  char pPrintInfo[80];
  LPSTR lpTemp;
  LPSTR lpPrintType;
  LPSTR lpPrintDriver;
  LPSTR lpPrintPort;

  /* get the current printer from the win.ini */
  if (!GetProfileString("windows", "Device", (LPSTR)"", pPrintInfo, 80))
      return (NULL);

  lpTemp = lpPrintType = pPrintInfo;
  lpPrintDriver = lpPrintPort = 0;
  while (*lpTemp) {
      if (*lpTemp == ',') {
          *lpTemp++ = 0;
          while (*lpTemp == ' ')
              lpTemp = AnsiNext(lpTemp);
          if (!lpPrintDriver)
              lpPrintDriver = lpTemp;
          else {
              lpPrintPort = lpTemp;
              break;
          }
      }
      else
          lpTemp = AnsiNext(lpTemp);
  }

  /* return a dc for the printer */
  return (CreateDC(lpPrintDriver, lpPrintType, lpPrintPort, (LPSTR) NULL));
}

/***********************************************************************

  FUNCTION   : AbortProc

  PARAMETERS : HDC hPr - printer DC
               int Code - printing status

  PURPOSE    : process messages for the abort dialog box

  CALLS      : WINDOWS
                 PeekMessage
                 IsDialogMessage
                 TranslateMessage
                 DispatchMessage

  MESSAGES   : none

  RETURNS    : int

  COMMENTS   :

  HISTORY    : 1/16/91 - created - originally from the SDK sample app
               PRNTFILE

************************************************************************/

int FAR PASCAL AbortProc(hPr, Code)
HDC hPr;
int Code;
{
  MSG msg;

  /* Process messages intended for the abort dialog box */

  while (!bAbort && PeekMessage(&msg, NULL, NULL, NULL, TRUE))
      if (!IsDialogMessage(hAbortDlgWnd, &msg)) {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
      }

  /* bAbort is TRUE (return is FALSE) if the user has aborted */

  return (!bAbort);
}

/***********************************************************************

  FUNCTION   : AbortDlg

  PARAMETERS : HWND hDlg;
               unsigned msg;
               WORD wParam;
               LONG lParam;

  PURPOSE    : Processes messages for printer abort dialog box

  CALLS      : WINDOWS
                 SetFocus

  MESSAGES   : WM_INITDIALOG - initialize dialog box
               WM_COMMAND    - Input received

  RETURNS    : int

  COMMENTS   : This dialog box is created while the program is printing,
               and allows the user to cancel the printing process.

  HISTORY    : 1/16/91 - created - originally from the SDK sample app
               PRNTFILE

************************************************************************/

int FAR PASCAL AbortDlg(hDlg, msg, wParam, lParam)
HWND hDlg;
unsigned msg;
WORD wParam;
LONG lParam;
{
    switch(msg) {

        /* Watch for Cancel button, RETURN key, ESCAPE key, or SPACE BAR */

        case WM_COMMAND:
            return (bAbort = TRUE);

        case WM_INITDIALOG:

            /* Set the focus to the Cancel box of the dialog */

            SetFocus(GetDlgItem(hDlg, IDCANCEL));
            return (TRUE);
        }
    return (FALSE);
}
