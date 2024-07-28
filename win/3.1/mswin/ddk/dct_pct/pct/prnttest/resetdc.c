/******************************************************************************

    Source Module:  ResetDC.C

    Purpose:    Contains source for testing the new ResetDC GDI call.

    The original test was simply to switch from portrait to landscape
    or landscape to portrait, then to perform the PrintPrintableArea
    test.  Unfortunately, this really doesn't provide much coverage.
    To provide more coverage, ResetDC is now removed from the list of
    normal tests.  When the option is selected, the suite of tests
    is run, then a dialog prompting to change the settings, continue
    the test, cancel the test, or end the test.  This way, all of the
    printer options can be controlled, and the other tests can be
    leveraged.  Note that when we change printer settings, we may
    change some of the device objects (especially fonts!), so we
    need to bring up the object selection dialog again.

    COPYRIGHT   1991, 1992, by Microsoft Corporation

    Change History:

    02-06-1991  Coded it (Happy birthday, Marti!)
    06-26-1991  Modified the code to leverage the other tests.

******************************************************************************/

#include    <windows.h>
#include    "PrntTest.h"

/******************************************************************************

    Private Function:    GetNewDCInfo

    Purpose:             calls ExtDeviceMode to get new printer layout.

    Called Routines:

    Side Effects:        If successful, replaces hdcPrinter with the new
                         settings the user chooses.

    Change History:

    02-06-1991  Guilty, with an explanation
    06-16-1991  Changed from test to support function

******************************************************************************/

BOOL    GetNewDCInfo(HWND      hParent,
                     LPPRINTER lpPrint)
{
  LPDEVMODE   lpDevMode;
  HDC         hdcNewDC;
  char        szDriverName[25];
  HANDLE      hLibrary;
  LPFNDEVMODE lpfnExtDeviceMode;
  int         nReturn;

  if    (!hDevMode)
  {
    MessageBox(hParent, "No ExtDeviceMode function in this driver!",
              "Notice!", MB_OK);
    return FALSE;
  }

  lpDevMode = (LPDEVMODE) GlobalLock(hDevMode);

  if (!lpDevMode)
  {
    MessageBox(hParent, "Can't lock hDevMode (ResetDC.C)", "Assertion",
          MB_OK);
    return FALSE;
  }

  // Load the printer driver, then call ExtDeviceMode to reconfigure.
  lstrcpy(szDriverName,lpPrint->szDriver);
  lstrcat(szDriverName,".DRV");

  hLibrary=LoadLibrary(szDriverName);
  if(hLibrary<32)
  {
    MessageBox(hParent, szDriverName, "LoadLibrary failed!", MB_OK);
    GlobalUnlock(hDevMode);
    return FALSE;
  }

  if(lpfnExtDeviceMode=GetProcAddress(hLibrary,"ExtDeviceMode"))
  {
    nReturn=(*lpfnExtDeviceMode)(hParent,hLibrary,lpDevMode,lpPrint->szName,
                                 lpPrint->szPort,lpDevMode,
                                 lpPrint->szProfile,
                                 DM_MODIFY | DM_PROMPT | DM_COPY);
    FreeLibrary(hLibrary);
    if(IDOK == nReturn)
    {
      if(hdcNewDC = ResetDC(hdcPrinter, lpDevMode))
      {
        hdcPrinter=hdcNewDC;
        GlobalUnlock(hDevMode);
        return TRUE;
      }
      else
      {
        MessageBox(hParent, "Printer doesn't support ResetDC, or ResetDC failed",
                  "Notice!", MB_OK);
        GlobalUnlock(hDevMode);
        return FALSE;
      }
    }
    else
    {
      GlobalUnlock(hDevMode);
      return FALSE;
    }
  }

  FreeLibrary(hLibrary);
  MessageBox(hParent, "Unable to get ExtDeviceMode dialog!",NULL,MB_OK);
  GlobalUnlock(hDevMode);

  return FALSE;
}



/*****************************************************************************\

    Public Function:   ResetDCDlg

    Purpose:           Dialog function for ResetDC test

    Called Routines:   GetNewDCInfo

    Return:            TRUE iff IDD_RESETDC_CONTINUE is pushed
                       bAbort is TRUE is IDD_RESET_ABORT is pushed.

    Change History:

    06-16-1991  Wrote it

******************************************************************************/
BOOL FAR PASCAL ResetDCDlg (HWND hDlg,
                            WORD wMsg,
                            WORD wParam,
                            LONG lParam)
{
  static LPPRINTER lpPrint;
  HWND             hItem;

  switch(wMsg)
  {
    case WM_INITDIALOG:
      lpPrint=(LPPRINTER)lParam;
      EnableMenuItem(GetSystemMenu(hDlg,FALSE),SC_CLOSE,MF_GRAYED);
      SetFocus(GetDlgItem(hDlg,IDD_RESETDC_SETUP));
      return TRUE;

    case WM_COMMAND:
      switch(wParam)
      {
        case IDD_RESETDC_SETUP:
          if(GetNewDCInfo(hDlg,lpPrint))
            hItem=GetDlgItem(hDlg,IDD_RESETDC_CONTINUE);
          else
            hItem=GetDlgItem(hDlg,IDD_RESETDC_END);

          SendMessage(hDlg,WM_NEXTDLGCTL,(WORD)hItem,1L);
          return TRUE;

        case IDD_RESETDC_ABORT:
          bAbort=TRUE;
          EndDialog(hDlg,FALSE);
          return TRUE;

        case IDD_RESETDC_END:
          EndDialog(hDlg,FALSE);
          return TRUE;

        case IDD_RESETDC_CONTINUE:
          EndDialog(hDlg,TRUE);
          return TRUE;
      }
  }
  return FALSE;
}

