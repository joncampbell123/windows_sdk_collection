/*******************************************************************************
 *									       *
 *  MODULE	: Print.c						       *
 *									       *
 *  DESCRIPTION : Routines used for printing.				       *
 *									       *
 *  FUNCTIONS	: GetPrinterDC()   - Gets default printer from WIN.INI and     *
 *				     creates a DC for it.		       *
 *									       *
 *		  InitPrinting()   - Initializes print job.		       *
 *									       *
 *		  TermPrinting()   - Terminates print job.		       *
 *									       *
 *		  PrintDlgProc()   - Dialog function for the "Cancel Printing" *
 *				     dialog.				       *
 *									       *
 *		  AbortProc()	   - Peeks at message queue for messages from  *
 *				     the print dialog.			       *
 *									       *
 *******************************************************************************/

#include <windows.h>
#include <string.h>
#include "showdib.h"

FARPROC  lpfnAbortProc	  = NULL;
FARPROC  lpfnPrintDlgProc = NULL;
HWND	 hWndParent	  = NULL;
HWND	 hDlgPrint	  = NULL;
BOOL	 bError;
BOOL	 bUserAbort;


BOOL FAR PASCAL AbortProc (HDC, short);
BOOL FAR PASCAL PrintDlgProc (HWND, unsigned, WORD, DWORD);

#pragma alloc_text(_PRINT, AbortProc, PrintDlgProc)

/****************************************************************************
 *									    *
 *  FUNCTION   : GetPrinterDC() 					    *
 *									    *
 *  PURPOSE    : Read WIN.INI for default printer and create a DC for it.   *
 *									    *
 *  RETURNS    : A handle to the DC if successful or NULL otherwise.	    *
 *									    *
 ****************************************************************************/
HDC PASCAL GetPrinterDC()
{
    static char szPrinter [80];
    char    *szDevice, *szDriver, *szOutput;

    GetProfileString ("windows", "device", "", szPrinter, sizeof(szPrinter));

    if ((szDevice = strtok (szPrinter, "," )) &&
	(szDriver = strtok (NULL,      ", ")) &&
	(szOutput = strtok (NULL,      ", ")))

	return CreateDC (szDriver, szDevice, szOutput, NULL) ;

    return NULL;
}
/****************************************************************************
 *									    *
 *  FUNCTION   : InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)  *
 *									    *
 *  PURPOSE    : Makes preliminary driver calls to set up print job.	    *
 *									    *
 *  RETURNS    : TRUE  - if successful. 				    *
 *		 FALSE - otherwise.					    *
 *									    *
 ****************************************************************************/
BOOL PASCAL InitPrinting(HDC hDC, HWND hWnd, HANDLE hInst, LPSTR msg)
{

    bError     = FALSE;     /* no errors yet */
    bUserAbort = FALSE;     /* user hasn't aborted */

    hWndParent = hWnd;	    /* save for Enable at Term time */

    lpfnPrintDlgProc = MakeProcInstance (PrintDlgProc, hInst);
    lpfnAbortProc    = MakeProcInstance (AbortProc, hInst);

    hDlgPrint = CreateDialog (hInst, "PRTDLG", hWndParent, lpfnPrintDlgProc);

    if (!hDlgPrint)
	return FALSE;

    SetWindowText (hDlgPrint, msg);
    EnableWindow (hWndParent, FALSE);	     /* disable parent */

    if ((Escape (hDC, SETABORTPROC, 0, (LPSTR)lpfnAbortProc, NULL) > 0) &&
	(Escape (hDC, STARTDOC, lstrlen(msg), msg, NULL) > 0))
	bError = FALSE;
    else
	bError = TRUE;

    /* might want to call the abort proc here to allow the user to
     * abort just before printing begins */
    return TRUE;
}
/****************************************************************************
 *									    *
 *  FUNCTION   :  TermPrinting(HDC hDC) 				    *
 *									    *
 *  PURPOSE    :  Terminates print job. 				    *
 *									    *
 ****************************************************************************/
void PASCAL TermPrinting(HDC hDC)
{
    if (!bError)
	Escape(hDC, ENDDOC, 0, NULL, NULL);

    if (bUserAbort)
	Escape (hDC, ABORTDOC, 0, NULL, NULL) ;
    else {
	EnableWindow(hWndParent, TRUE);
	DestroyWindow(hDlgPrint);
    }

    FreeProcInstance(lpfnAbortProc);
    FreeProcInstance(lpfnPrintDlgProc);
}
/****************************************************************************
 *									    *
 *  FUNCTION   :PrintDlgProc (HWND, unsigned , WORD , DWORD )		    *
 *									    *
 *  PURPOSE    :Dialog function for the "Cancel Printing" dialog. It sets   *
 *		the abort flag if the user presses <Cancel>.		    *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL PrintDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, DWORD lParam)
{
    switch (iMessage) {
    case WM_INITDIALOG:

	    EnableMenuItem (GetSystemMenu (hDlg, FALSE), SC_CLOSE, MF_GRAYED);
	    break;

    case WM_COMMAND:
	    bUserAbort = TRUE;
	    EnableWindow (hWndParent, TRUE);
	    DestroyWindow (hDlg);
	    hDlgPrint = 0;
	    break;

    default:
	    return FALSE;
    }
    return TRUE;
}

/****************************************************************************
 *									    *
 *  FUNCTION   :AbortProc (HDC hPrnDC, short nCode)			    *
 *									    *
 *  PURPOSE    :Checks message queue for messages from the "Cancel Printing"*
 *		dialog. If it sees a message, (this will be from a print    *
 *		cancel command), it terminates. 			    *
 *									    *
 *  RETURNS    :Inverse of Abort flag					    *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL AbortProc (HDC hPrnDC, short nCode)
{
    MSG   msg;

    while (!bUserAbort && PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
	if (!hDlgPrint || !IsDialogMessage(hDlgPrint, &msg)) {
	    TranslateMessage (&msg);
	    DispatchMessage (&msg);
	}
    }
    return !bUserAbort;
}
