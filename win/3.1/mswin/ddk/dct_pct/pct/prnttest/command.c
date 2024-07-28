/******************************************************************************

    Source Module:  Command.C

    Purpose:        Functions associated with WM_COMMAND messages in the app.

This includes the basic code for running tests (yes, you finally found it!)

Copyright   1989-1992, by Microsoft Corporation

Change History:

    06-05-1989  Original code
    02-14-1991  Modified for Win 3.1
    08-01-1991  Added GetCharWidths test, changed ResetDC test

******************************************************************************/

#define PRINTING               // Include printing stuff
#include    <Windows.H>
#include    "PrntTest.H"

/******************************************************************************

    Static Data

    The aTestList structure is a list of test functions which can be
    executed, along with the information necessary to determine which tests
    need to be executed.

    The structure consists of a pointer to a WORD, which has bit flags
    controlling tests (note we can have as many as we like, now), a constant
    which shows a mask used to determine if the test is to be executed, and
    a pointer to the test routine.

    The problem of forcing a test to always execute is solved by the
    convention that a NULL pointer to a flag word indicates the test must
    always be called.

*******************************************************************************/

static struct
  {
    WORD    *pwFlag;
    WORD    wCheckValue;
    BOOL    (*pbfnTestFunction)(void);
  } aTestList[] = {
                   {&wHeaderSet, IDD_HEAD_TITLEPAGE, PrintTitlePage},
                   {&wHeaderSet, IDD_HEAD_ENTRY,     PrintFunctionSupport},
                   {&wHeaderSet, IDD_HEAD_GRAY,      PrintGrayScale},
                   {&wHeaderSet, IDD_HEAD_BOUNDS,    PrintPrintableArea},
                   {&wHeaderSet, IDD_HEAD_CAPS,      PrintDeviceCapabilities},
                   {&wHeaderSet, IDD_HEAD_FONT,      PrintDeviceFonts},
                   {&wHeaderSet, IDD_HEAD_BRSH,      PrintDeviceBrushes},
                   {&wHeaderSet, IDD_HEAD_PEN,       PrintDevicePens},
                   {&wTestsSet,  IDD_TEST_TEXT,      PrintText},
                   {NULL,        NULL,               PrintBrushSummary},
                   {NULL,        NULL,               PrintPenSummary},
                   {&wTestsSet,  IDD_TEST_BITMAPS,   PrintBitmaps},
                   {&wTestsSet,  IDD_TEST_POLYGONS,  PrintPolygons},
                   {&wTestsSet,  IDD_TEST_CHARWIDTH, PrintCharWidths},
                   {&wTestsSet,  IDD_TEST_CURVES,    PrintCurves},
                   {&wTestsSet,  IDD_TEST_LINES,     PrintLines}
                  };

static char szBuffer[80];

/******************************************************************************

    Private Function:   PrintEndDoc

    Purpose:            Issues an EndDoc or ENDDOC Escape

    This is a function, to allow it to be inserted in the Test Table, before
    the AbortDoc test.

    Change History:

    02-14-1991  Coded it (the St. Valentine's Day massacre)

******************************************************************************/

BOOL PrintEndDoc(void)
{
  BOOL bReturn=TRUE;

  if (wStyleSelected == 30)
    Escape(hdcPrinter,ENDDOC,0,NULL,NULL);
  else
  {
    FIND31GDICALL lpFn;

    if(lpFn=Find31GDICall(ENDDOC_ORDINAL))
      (*lpFn) (hdcPrinter);
    else
      bReturn=FALSE;
  }
  return bReturn;
}


void TidyMetaFile(void)
{
  HANDLE hMetaFile;

  if(bMetafile)
  {
    hMetaFile=CloseMetaFile(hdcTarget);
    if(!bAbort)
      PlayMetaFile(hdcPrinter,hMetaFile);
    DeleteMetaFile(hMetaFile);
  }
}


/*****************************************************************************

    Private Function:  TestCleanup

    Purpose:           Do cleanup for TestOnePrinter

    Put this there to prevent repeating code

    Change History:

    06/25/1991  Wrote it

******************************************************************************/

void TestCleanup(BOOL bDoEndDoc,
                 BOOL bDoMetaFile,
                 HWND hwnd,
                 HWND hAbortDlg)
{
  if(bDoMetaFile)
    TidyMetaFile();

  // Send EndDoc if it's needed
  if(bDoEndDoc)
    PrintEndDoc();

  // Re-enable parent window
  EnableWindow(hwnd, TRUE);

  // Nuke the printer DC
  DeleteDC(hdcPrinter);

  // Free up memory required for objects
  FreeDeviceObjects(hBrushes);
  FreeDeviceObjects(hPens);
  FreeDeviceObjects(hFonts);

  // Get rid of the abort window, if needed
  if(hAbortDlg)
  {
    DestroyWindow(hAbortDlg);
    hAbortDlg=NULL;
  }

  // Free Callback functions, if required
  if(lpAbortDlg)
  {
    FreeProcInstance(lpAbortDlg);
    lpAbortDlg=NULL;
  }

  if(lpAbortProc)
  {
    FreeProcInstance(lpAbortProc);
    lpAbortProc=NULL;
  }

  // Free memory for devmode
  if(hDevMode)
  {
    GlobalFree(hDevMode);
    hDevMode=NULL;
  }

  return;
}



/******************************************************************************

    Private Function:   TestOnePrinter

    Purpose:            Tests one printer

    This deciphers the given profile string, opens the DC, and tests the
    printer, as required.

    Change History:

    02-14-1991  Moved from the ProcessPrntTestCommands proc, to keep
                the code manageable.
    06-24-1991  ResetDC is now outside the regular suite of tests,
                to allow a broader range of testing.
    08-23-1991  returns TRUE if testing completed, otherwise FALSE.

******************************************************************************/

BOOL TestOnePrinter(HWND hwnd,
                    PSTR pstrBuffer)
{
  PSTR        pstrPrinterInfo;
  FARPROC     lpProc;
  int         iTest;
  DOCINFO     DocInfo;
  BOOL        bTestBegun;
  BOOL        bDoSomeMore;
  BOOL        bAbortNow;
  FARPROC     lpResetDC;
  int         nEscape;

  /*
    The Buffer has <profile name>:<Printer Info>
    We need to separate the two.
  */
  for   (pstrPrinterInfo = pstrBuffer;
        *pstrPrinterInfo && *pstrPrinterInfo != ':';
        pstrPrinterInfo++)
    ;

  if    (!*pstrPrinterInfo)
  {
    MessageBox(hwnd, pstrBuffer, "This string is garbage!", MB_OK);
    return FALSE;
  }

  *pstrPrinterInfo++ = '\0';    /*  The strings are now separated   */

  /*
    Get Printer DC to test.
  */

  if  (!(hdcPrinter = GetPrinterDC(hwnd, pstrBuffer, &hDevMode)))
  {
    MessageBox(hwnd, "Assertion: GetPrinterDC (misc.c)",
          pstrBuffer, MB_OK);
    return FALSE;
  }

  // Get printer information (parse buffer)
  GetPrinterInformation(hdcPrinter, &pPrinter, pstrBuffer,pstrPrinterInfo);

  // Initialize flags
  lpAbortProc=NULL;
  hAbortDlg=NULL;
  bTestBegun=FALSE;
  bDoSomeMore=FALSE;
  bAbort=FALSE;

  do // Do this loop until we're not testing ResetDC or until the
  {  // user decides he's had enough fun...

    /*
      Get the various global information objects filled in, and get the list
      of objects available for test from ISG_TEST.DLL
    */

    GetDeviceInfo(hdcPrinter, &dcDevCaps);

    if(bTestBegun)
    {
      // We've already looked at the device objects, but now we're going for
      // another pass.  We need to free the current objects before we
      // enumerate them again.

      FreeDeviceObjects(hFonts);
      FreeDeviceObjects(hBrushes);
      FreeDeviceObjects(hPens);
    }

    hFonts = GetDeviceObjects(hdcPrinter, DEV_FONT, wFontOptions);
    hBrushes = GetDeviceObjects(hdcPrinter, DEV_BRUSH, NULL);
    hPens = GetDeviceObjects(hdcPrinter, DEV_PEN, NULL);
    GetTextMetrics(hdcPrinter, &tmDefault);

    /*
      Call the Setup Objects Dialog to allow the user to select which
      objects (pens, brushes, fonts) are to be used in the test.  The
      decision to Band output (for Win 3.0 style printing) may also be
      made here.  The user may also cancel the test from this dialog.
    */

    if  (!(lpProc = MakeProcInstance(SetupObjectsDlg, hInst)))
    {
      MessageBox(hwnd, "Couldn't setup Object Selection Dialog", NULL,
            MB_OK);

      TestCleanup(FALSE,FALSE,hwnd,hAbortDlg);   // Don't do an EndDoc
      return FALSE;
    }

    bAbortNow = !DialogBox(hInst,MAKEINTRESOURCE(SETOBJECT),
                           lpAbortProc?hAbortDlg:hwnd, lpProc);
    FreeProcInstance(lpProc);

    if  (bAbortNow)
    {
      TestCleanup(FALSE,FALSE,hwnd,hAbortDlg);   // No EndDoc
      return FALSE;
    }

    if(!lpAbortProc)
    {
      /*--------------------------*\
      | Setup the Abort Dialog,    |
      | Disable main application   |
      | window.                    |
      \*--------------------------*/
      EnableWindow(hwnd, FALSE);
      bAbort           = FALSE;
      lpAbortDlg  = MakeProcInstance(AbortDlg, hInst);
      hAbortDlg   = CreateDialog(hInst,MAKEINTRESOURCE(ABORTDLG),hwnd,
                                 lpAbortDlg);
      lpAbortProc = MakeProcInstance(PrintAbortProc, hInst);


      if(30 == wStyleSelected)
        nEscape=Escape(hdcPrinter,SETABORTPROC,NULL,(LPSTR)lpAbortProc,NULL);
      else
      {
        FINDSETABORTPROC lpFn;

        if(lpFn=Find31GDICall(SETABORTPROC_ORDINAL))
          nEscape=(*lpFn) (hdcPrinter, (ABORTPROC) lpAbortProc);
        else
          nEscape=-1;
      }

      if(nEscape < 0)
      {
        MessageBox(hwnd, (wStyleSelected == 30) ?
              "Escape SetAbortProc (command.c)" :
              "GDI Call SetAbortProc (command.c)", "Assertion", MB_OK);

        TestCleanup(FALSE,FALSE,hwnd,hAbortDlg);    // Nothing sent so far
        return FALSE;
      }
    }

    /*
        If this is to be metafiled, create the metafile.  Otherwise,
        hdcTarget is identical to hdcPrinter.
    */

    hdcTarget = bMetafile ? CreateMetaFile(NULL) : hdcPrinter;
    if  (!hdcTarget)
    {
      MessageBox(hwnd, "Failed to create metafile DC (command.c)",
            "Assertion", MB_OK);

      TestCleanup(bTestBegun,FALSE,hwnd,hAbortDlg);  // Maybe already begun
      return FALSE;
    }

    /*-----------------------------------------------------*\
    | START print job, if not already in progress           |
    \*-----------------------------------------------------*/
    if(!bTestBegun)
    {
      LoadString(hInst,IDS_TEST_JOBTITLE,szBuffer,sizeof(szBuffer));
      DocInfo.cbSize=sizeof(DOCINFO);
      DocInfo.lpszDocName=szBuffer;
      DocInfo.lpszOutput=NULL;

      if(30 == wStyleSelected)
        nEscape=Escape(hdcTarget,STARTDOC,lstrlen(szBuffer),szBuffer,NULL);
      else
      {
        FINDSTARTDOC lpFn;

        if(lpFn=Find31GDICall(STARTDOC_ORDINAL))
          nEscape=(*lpFn)(hdcTarget, &DocInfo);
        else
          nEscape=-1;
      }

      if(nEscape < 0)
      {
        MessageBox(hwnd, (wStyleSelected == 30) ?
              "Escape StartDoc (command.c)" :
              "GDI Call StartDoc (command.c)", "Assertion", MB_OK);

        TestCleanup(FALSE,TRUE,hwnd,hAbortDlg);   // StartDoc failed
        return FALSE;
      }
    }    // End of if (!bTestBegun)

    // Set the flag to indicate that we've started a test
    bTestBegun=TRUE;

    /*
        Execute the tests out of the structure above.  Note that
        we can add new tests by simply editing the structure (and
        we're also saving code space).
    */

    for (iTest = 0;
        !bAbort && iTest < sizeof(aTestList) / sizeof(aTestList[0]);
        iTest++)
      if    (!aTestList[iTest].pwFlag ||
            *aTestList[iTest].pwFlag & aTestList[iTest].wCheckValue)
        (*aTestList[iTest].pbfnTestFunction)();

    /*
        If we're metafiling, close the metafile, and blast all this
        fun stuff we've been recording into the printer DC.
    */

    if  (bMetafile)
      TidyMetaFile();
    else                // Can't do ResetDC with metafiles....
    {
      // If we're testing ResetDC, we need to get the new config, and
      // go back for some more fun!

      if((BOOL)(wTestsSet & IDD_TEST_RESETDC) && !bAbort)
      {
        lpResetDC=MakeProcInstance(ResetDCDlg,hInst);
        bDoSomeMore=(BOOL)DialogBoxParam(hInst,MAKEINTRESOURCE(RESETDCDLG),
                                        hAbortDlg,lpResetDC,
                                        (DWORD)(LPSTR)&pPrinter);
        FreeProcInstance(lpResetDC);
      }
    }

  } while (bDoSomeMore && !bAbort);

  // The user has had enough fun with resetdc--end the doc, then test
  // AbortDoc if needed.

  if(!bAbort)
  {
    // Send an EndDoc to the printer.
    PrintEndDoc();

    if(wTestsSet & IDD_TEST_ABORTDOC)
    {
      if(bMetafile)                      // Generate a new metafile, since
        hdcTarget=CreateMetaFile(NULL);  // we closed the old one

      TestAbortDoc();
    }
  }

  TestCleanup(FALSE,TRUE,hwnd,hAbortDlg); // EndDoc already sent if needed

  return TRUE;
}


/******************************************************************************

    Public Function:    ProcessPrntTestCommands

    Purpose:            Handles WM_COMMAND messages

    This is the one that runs the tests, puts up the various dialogs, etc.

    Parameters:

    HWND hwnd - Application main window (client)

    WORD wParam -   The Command ID (from the WM_COMMAND message)

    Side Effects:

        Fills in various global structures associated with printing, when
        told to run the tests.

    Change History:

    06-05-1991  Coded original

    02-14-1991  Changes needed for Win 3.1

******************************************************************************/

BOOL ProcessPrntTestCommands(HWND hWnd, WORD wParam)
{
  FARPROC lpProc;
  int     nCount,nIdx;
  BOOL    bTestsRun=FALSE;

  switch(wParam)
    {
      /*------------------------------------*\
      | RUN Test.                            |
      \*------------------------------------*/
      case IDM_TEST_RUN:

        /*-------------------------------*\
        | Get the list of test profiles.  |
        \*-------------------------------*/
        nCount = (int)SendDlgItemMessage(hPrntDlg,IDD_INTRFACE_TEST,
                           LB_GETCOUNT,NULL,0l);

        for (nIdx=0; nIdx < nCount; nIdx++)
        {
          /*--------------------------*\
          | Get device string for this |
          | particular profile string. |
          \*--------------------------*/
          SendDlgItemMessage(hPrntDlg,IDD_INTRFACE_TEST,LB_GETTEXT,
                              nIdx,(LONG)(LPSTR)szBuffer);

          bTestsRun = TestOnePrinter(hWnd, szBuffer);
        }

        /*-----------------------------------*\
        | End of last run--Don't steal focus! |
        \*-----------------------------------*/
        if(bTestsRun && !bAbort)
        {
          HWND hCurrent;

          hCurrent=GetFocus();
          MessageBox(hCurrent,"Printer Tests Completed",PRNTTESTTITLE,MB_OK);
          if  (bAutoRun)
            ExitWindows((DWORD)NULL,0);
        }

        break;

      /*---------------------------------*\
      |  header settings.                 |
      \*---------------------------------*/
      case IDM_SETTINGS_HEADER:

        if (!(lpProc = MakeProcInstance(SetupHeaderDlg,hInst)))
          return(FALSE);
        DialogBox(hInst,MAKEINTRESOURCE(SETHEADER),hWnd,lpProc);
        FreeProcInstance(lpProc);
        SetFocus(hPrntDlg);
        break;

      /*---------------------------------*\
      |  tests settings.                  |
      \*---------------------------------*/
      case IDM_SETTINGS_TESTS:
        if (!(lpProc = MakeProcInstance(SetupTestsDlg,hInst)))
          return(FALSE);
        DialogBox(hInst,MAKEINTRESOURCE(SETTESTS),hWnd,lpProc);
        FreeProcInstance(lpProc);
        SetFocus(hPrntDlg);
        break;

      /*---------------------------------*\
      |  style settings.                  |
      \*---------------------------------*/
      case IDM_OPTIONS_STYLES:
        if (!(lpProc = MakeProcInstance(SelectStyleDlg,hInst)))
          return(FALSE);
        DialogBox(hInst,MAKEINTRESOURCE(SELECTENVIRONMENT),hWnd,lpProc);
        FreeProcInstance(lpProc);
        SetFocus(hPrntDlg);
        break;

      /*---------------------------------*\
      |  Device Fonts Options             |
      \*---------------------------------*/
      case IDM_OPTIONS_FONTS:
        if  (!(lpProc = MakeProcInstance(FontOptionsDlg,hInst)))
          return(FALSE);
        DialogBox(hInst,MAKEINTRESOURCE(FONTOPTIONS),hWnd,lpProc);
        FreeProcInstance(lpProc);
        SetFocus(hPrntDlg);
        break;

      /*------------------------------------*\
      | Display about dialog box.            |
      \*------------------------------------*/
      case IDM_HELP_ABOUT:
        if (!(lpProc = MakeProcInstance((FARPROC)AboutDlg,hInst)))
          return(FALSE);
        DialogBox(hInst,MAKEINTRESOURCE(ABOUTDLG),hWnd,lpProc);
        FreeProcInstance(lpProc);
        SetFocus(hPrntDlg);
        break;

      /*------------------------------------*\
      | No command found.                    |
      \*------------------------------------*/
      default:
        return(FALSE);
    }

  return(TRUE);
}


