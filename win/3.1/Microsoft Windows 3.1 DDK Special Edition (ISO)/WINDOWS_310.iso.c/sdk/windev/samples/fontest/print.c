/***************************************************************************
 * print.c
 *
 * routines used for printing
 *
 *
 * NOTE: be sure to place these in your def file
 * 	AbortProc
 * 	PrintDlgProc
 *
 ***************************************************************************/

#include <windows.h>
#include "print.h"

FARPROC lpfnAbortProc = NULL;
FARPROC lpfnPrintDlgProc = NULL;
HWND hDlgPrint = NULL;
BOOL bError;
BOOL bUserAbort;
char szNull[] = "";


BOOL FAR PASCAL _export AbortProc(HDC, short);
BOOL FAR PASCAL _export PrintDlgProc(HWND, unsigned, WORD, DWORD);

#pragma alloc_text(_PRINT, AbortProc, PrintDlgProc)


HDC PASCAL GetPrinterDC()
{
	char	msgbuf[128];
	LPSTR	 pch;
	LPSTR	 pchFile;
	LPSTR	 pchPort;

	if (!GetProfileString("windows", "device", szNull, msgbuf, sizeof(msgbuf)))
		return NULL;

	for (pch = msgbuf; *pch && *pch != ','; pch = AnsiNext(pch));

	if (*pch)
		*pch++ = 0;

	/* Skip tabs, control chars */
	while (*pch && *pch <= ' ')
		pch=AnsiNext(pch);

	pchFile = pch;
	while (*pch && *pch != ',' && *pch > ' ')
		pch = AnsiNext(pch);

	if (*pch)
		*pch++ = 0;

	while (*pch && (*pch <= ' ' || *pch == ','))
		pch = AnsiNext(pch);

	pchPort = pch;
	while (*pch && *pch > ' ')
		pch = AnsiNext(pch);

	*pch = 0;

	return CreateDC(pchFile, msgbuf, pchPort, NULL);
}



BOOL PASCAL InitPrinting(HDC hdc, HWND hWnd, LPSTR msg)
{
	HANDLE hInst = GetWindowWord(hWnd, GWW_HINSTANCE);

	bError = FALSE;		/* no errors yet */
	bUserAbort = FALSE;	/* user hasn't aborted */

	hWnd = hWnd;	/* save for Enable at Term time */

	lpfnPrintDlgProc = MakeProcInstance(PrintDlgProc, hInst);
	lpfnAbortProc    = MakeProcInstance(AbortProc, hInst);

	hDlgPrint = CreateDialogParam(hInst, "PRTDLG", hWnd, lpfnPrintDlgProc, NULL);

	if (!hDlgPrint)
		return FALSE;

	SetWindowText(hDlgPrint, msg);

	EnableWindow(hWnd, FALSE);	/* disable parent */

	if ((Escape(hdc, SETABORTPROC, 0, (LPSTR)lpfnAbortProc, NULL) > 0) && 
	    (Escape(hdc, STARTDOC, lstrlen(msg), msg, NULL) > 0))
		bError = FALSE;
	else
		bError = TRUE;

	return !bError;
}


void PASCAL TermPrinting(HDC hdc, HWND hWnd)
{
	if (!bError) {
		Escape(hdc, ENDDOC, 0, NULL, NULL);
	}

	if (bUserAbort) {		/* already been done? */
		Escape (hdc, ABORTDOC, 0, NULL, NULL) ;
	} else {
		EnableWindow(hWnd, TRUE);/* turn parent back on */
		DestroyWindow(hDlgPrint);/* toast out dialog */
	}

	FreeProcInstance(lpfnAbortProc);
	FreeProcInstance(lpfnPrintDlgProc);
}



/*
 * PrintDlgProc()	printing dialog proc (has a cancle button on it)
 *
 * globals used:
 *	szAppName	name of the app for dialog caption
 *	bUserAbort	sets this when the user hits cancel
 *	hDlgPrint	
 */

BOOL FAR PASCAL _export PrintDlgProc (HWND hDlg, unsigned iMessage, WORD wParam, DWORD lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG:

		EnableMenuItem(GetSystemMenu(hDlg, FALSE), SC_CLOSE, MF_GRAYED);
		// SetWindowLong(hDlg, DWL_USER, lParam);
		break;

	case WM_COMMAND:
		// lpPrintData = GetWindowLong(hDlg, DWL_USER);
		// lpPrintData->bUserAbort = TRUE;
		bUserAbort = TRUE;
		EnableWindow(GetParent(hDlg), TRUE);
		DestroyWindow(hDlg);
		hDlgPrint = 0;
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


/*
 * AbortProc()	printing abort procedure
 *
 * globals used:
 *	bUserAbort 	indicates the user hit CANCLE on the print dialog
 *	hDlgPrint	handle of the print dialog
 *
 */

BOOL FAR PASCAL _export AbortProc (HDC hPrnDC, short nCode)
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
