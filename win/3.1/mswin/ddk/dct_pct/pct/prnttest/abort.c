/*---------------------------------------------------------------------------*\
| PRINTER ABORT                                                               |
|   This module contains the printer Abort procedure.  It is used so that     |
|   Windows may still process while the application is printing.              |
|                                                                             |
| DATE   : June 05, 1989                                                      |
| Copyright 1989-1992 by Microsoft Corporation                                |
\*---------------------------------------------------------------------------*/

#include    <windows.h>
#include    "PrntTest.h"

/*---------------------------------------------------------------------------*\
| PRINT ABORT PROCEDURE                                                       |
|   This routine handles the message dispatching when the application is      |
|   printing.  It checks the message queue for any messages to process.       |
|   If the user responds to the ABORT dialog, then Printing is halted.        |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HDC   hDC   - Handle to the Printer Device Context.                       |
|   short nCode - Code passed to AbortProc indicating status info.            |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|  BOOL bAbort     - If set to TRUE, then begin abort-print process.           |
|  HWND hAbortDlg  - Determines if Dlg windows exists.                         |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE indicates that the user hasn't aborted printing.  FALSE       |
|          indicates printing should continue.                                |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL PrintAbortProc(HDC   hDC,
                               short nCode)
{
  extern BOOL bAbort;                         /* If TRUE, then abort      */
  extern HWND hAbortDlg;                      /* Handle to abort dialog   */

  MSG msg;                                    /* Message Structure        */

  /*-----------------------------------------*\
  | Retreive messages until none, or user     |
  | aborts print test.                        |
  \*-----------------------------------------*/
  while(!bAbort && PeekMessage(&msg,NULL,0,0,PM_REMOVE))
  {
    if(!hAbortDlg || !IsDialogMessage(hAbortDlg,&msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return(!bAbort);
}


/*---------------------------------------------------------------------------*\
| PRINT ABORT DIALOG PROCEDURE                                                |
|   This routine is handles the processing of the Abort Dialog Box.  It       |
|   It displays a message and waits for user to cancel job.                   |
|                                                                             |
| CALLED ROUTINES                                                             |
|   -none-                                                                    |
|                                                                             |
| PARAMETERS                                                                  |
|   HWND     hDlg     - The Window Handle.                                    |
|   unsigned iMessage - Message to be processed.                              |
|   WORD     wParam   - Information associated with message.                  |
|   LONG     lParam   - Information associated with message.                  |
|                                                                             |
| GLOBAL VARIABLES                                                            |
|  BOOL bAbort     - Set to TRUE if user hits OK to cancel.                    |
|  HWND hAbortDlg  - Set to NULL once the dialog window is destroyed.          |
|                                                                             |
| RETURNS                                                                     |
|   BOOL - TRUE indicates the message was processed, otherwise a FALSE        |
|          is returned.                                                       |
\*---------------------------------------------------------------------------*/
BOOL FAR PASCAL AbortDlg(HWND     hDlg,
                         unsigned iMessage,
                         WORD     wParam,
                         LONG     lParam)
{
  extern BOOL bAbort;
  extern HWND hAbortDlg;

  switch(iMessage)
  {
    /*------------------------------------*\
    | Initialize Window upon entering      |
    | the dialog procedure.                |
    \*------------------------------------*/
    case WM_INITDIALOG:
      SetWindowText(hDlg,"Windows Printer Device Test");
      EnableMenuItem(GetSystemMenu(hDlg,FALSE),SC_CLOSE,MF_GRAYED);
      break;

    /*------------------------------------*\
    | If user hits any KEY-->Quit Dialog.  |
    | Set bAbort=TRUE, and hAbortDlg=NULL. |
    \*------------------------------------*/
    case WM_COMMAND:
      bAbort = TRUE;
      EnableWindow(GetParent(hDlg),TRUE);
      break;

    /*------------------------------------*\
    | Message wasn't processed.            |
    \*------------------------------------*/
    default:
      return(FALSE);
  }

  return(TRUE);
}


/******************************************************************************

    Public Function:    TestAbortDoc

    Purpose:            Tests the AbortDoc call, or ABORTDOC escape.

    This test does a StartDoc, then prints a text line (which shouyldn't
    ever get printed), then does an AbortDoc.  If the AbortDoc fails, an
    EndDoc is issued to clean up.

    Parameters:
        None

    Side effects:

        None.  No print job is in progress at entry, nor at exit.

    Change History:

    2-12-1991   Coded it
    6-25-1991   Changed hdcTarget to hdcPrinter
   10-09-1991   StartDoc still uses hdcPrinter.  EndDoc (if the
                AbortDoc call fails) uses hdcTarget.

******************************************************************************/

BOOL TestAbortDoc(void)
{
  static char   sacBuffer[80], sacTestMessage[] = "If you see this message, "
                      "the AbortDoc function does not work!";
  RECT          rcText;
  DOCINFO       DocInfo;
  int           nEscape;

  /*--------------------------*\
  | START print job.           |
  \*--------------------------*/
  LoadString(hInst, IDS_TEST_JOBTITLE, sacBuffer, sizeof(sacBuffer));
  DocInfo.cbSize=sizeof(DOCINFO);
  DocInfo.lpszDocName=sacBuffer;
  DocInfo.lpszOutput=NULL;


  if(30 == wStyleSelected)
    nEscape=Escape(hdcPrinter,STARTDOC,lstrlen(sacBuffer),sacBuffer,NULL);
  else
  {
    FINDSTARTDOC lpFn;

    if(lpFn=Find31GDICall(STARTDOC_ORDINAL))
      nEscape=(*lpFn)(hdcPrinter, &DocInfo);
    else
      nEscape=-1;
  }

  if(nEscape < 0)
  {
    MessageBox(NULL, (wStyleSelected == 30) ?
          "Escape StartDoc (abort.c)" : "GDI Call StartDoc (abort.c)",
          "Assertion", MB_OK);

    return    FALSE;
  }

  SetRect(&rcText, 0, iPageHeight / 2, iPageWidth,
          (iPageHeight /2) + iHeightOfOneTextLineWithLeading);

  if  (!DoSomeText(&rcText, lstrlen(sacTestMessage), sacTestMessage))
  {
    MessageBox(NULL, "Failed to print text for AbortDoc Test (abort.c)",
          "Assertion", MB_OK);
    return    FALSE;
  }
    
  if(30 == wStyleSelected)
    nEscape=Escape(hdcTarget,ABORTDOC,NULL,NULL,NULL);
  else
  {
    FIND31GDICALL lpFn;

    if(lpFn=Find31GDICall(ABORTDOC_ORDINAL))
      nEscape=(*lpFn)(hdcTarget);
    else
      nEscape=-1;
  }

  if(nEscape < 0)
  {
    /*
      The AbortDoc failed.  Print the page, by issuing an EndDoc, and
      return FALSE to indicate failure of the test.  The caller takes
      care of special handling for metafiles.
    */

    if  (wStyleSelected == 30)
    {
      Escape(hdcTarget, NEWFRAME, 0, NULL, NULL);
      Escape(hdcTarget, ENDDOC, 0, NULL, NULL);
    }
    else
    {
      EndPage(hdcTarget);
      EndDoc(hdcTarget);
    }

    return FALSE;
  }


  return TRUE;
}
