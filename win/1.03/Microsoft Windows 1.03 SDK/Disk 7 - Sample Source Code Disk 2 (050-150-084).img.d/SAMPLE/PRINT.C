/*
 * Print.c   print functions for Sample file Application.
 *-------------------------------------------------------*/

/*
 * NOTE: these routines send a document to the print spooler, and also
 * control a modeless dialog box which lets the user abort the job.
 * The dialog box must have a text field (ID_NAME) and a cancel button (IDCANCEL).
 *
 * Also, the application using this module must export DlgFnAbort in
 * its .DEF file and must also include "declare.h" in its source file.
 *
 * Functions provided:
 *  PrintFile	- sends job to spooler and creates abort dialog box.
 *  AbortTest	- given to gdi to allow spooler to interrupt job.
 *  DlgFnAbort	- function passed to windows to control modeless abort dlgbox.
 *  GetPrinterDC - reads win.ini for printer info, and creates printer dc.
 *  PrintErr	- puts up a message box for printer errors.
 *  CenterPopup - centers an invisible popup and makes it visible.
 *  GetScreenCenter - gets the screen coordinates of the physical center.
 *
 *============================================================================*/

/*
 * The way this application prints is by a method called "nextbanding".  Instead
 * of sending a whole document at once to the spooler, nextbanding sends a portion
 * of the document at a time.  This method is faster and doesn't require as much
 * disk space.
 *					  printer page
 *				     _______________________
 *				    |			    |
 *				    |			    |
 *				    |			    |
 *				    |			    |
 *				    |____dxBand_____________|
 *		      rectBand ---> |			    dyBand
 *		(holds 4 coords.)   |_______________________|
 *				    |			    |
 *				    |			    | dyPrinter
 *				    |			    |
 *				    |			    |
 *				    |			    |
 *				    |			    |
 *				    |_______________________|
 *					    dxPrinter
 *
 * Each time you call Escape(..,NEXTBAND,..,..,..) you are given the coordinates
 * of the current band on the printer page.  You must then compute what you want
 * to print in this band, and then use GDI functions to write to the printerDC.
 *
 *=============================================================================*/


#include "windows.h"
#include "sample.h"
#include "declare.h"

extern BOOL fUntitled;	/* is there a current file? */
extern char szUntitled[];

BOOL	fAbortPrint;	/* abort sending job to spooler? */

HANDLE	hInst;		/* handle to calling application instance */
HWND	hWndAbort;	/* handle to modeless abort dialog box window */
HMENU	hmenuSys;	/* handle to system menu in abort dialog box */

char rgchFilename[MAX_FNAME_LEN];   /* file to print. (no path) */
char rgchAppName[MAX_STR_LEN];	    /* calling application name */

OFSTRUCT ofstrFile;	/* printing file's ofstruct. (full path here) */


/*===============================================================================
 PRINTFILE is called from an application which wishes to send a print job to the
 spooler.  Printfile reads win.ini to get information about the user's printer,
 creates a DC for the printer, puts up a modeless dialog box informing the user
 that the job is being sent, and finally sends the job to the spooler one band
 at a time.  In this sample application, the file is not actually opened and data
 from it sent;	printfile just sends a black band where in a real application it
 would send a bitmap or text from the source file.
===============================================================================*/
int FAR PASCAL PrintFile (hInstance, hWnd, iddlg, ofstrCur, szFilename, szAppName)
HANDLE hInstance;   /* application module instance handle */
HWND   hWnd;	    /* window handle of parent window */
int    iddlg;	    /* Abort dialog box id */
OFSTRUCT ofstrCur;  /* ofstruct of file to print.  (contains full path) */
char   *szFilename; /* name of file to print.  (no path!) */
char   *szAppName;  /* name of application */
{
    int     ierr;
    int     x0Src, y0Src;	    /* top left corner of current band of source */
    int     dxSrc, dySrc;	    /* width & height of current band of source */
    int     dxBand, dyBand;	    /* width & height of current band of printer */
    int     dxFile, dyFile;	    /* width & height of source document */
    int     dxPrinter, dyPrinter;   /* width & height of printer page */
    RECT    rectBand;		    /* 4 coordinates of current band of printer */
    char    rgchDocName[32];	    /* spooler only takes max. 32 char doc name */
    HDC     hPrinterDC; 	    /* DC of the printer */
    FARPROC lpDlgFnAbort;
    FARPROC lpAbortTest;

    hInst = hInstance;
    lstrcpy((LPSTR)rgchFilename, (LPSTR)szFilename);
    lstrcpy((LPSTR)rgchAppName, (LPSTR)szAppName);
    ofstrFile = ofstrCur;
    hWndAbort = NULL;
    fAbortPrint = FALSE;

    /* get printer information from win.ini and make its dc */
    hPrinterDC = GetPrinterDC();
    if (hPrinterDC == NULL) {
	PrintErr(hWnd, SP_ERROR);
	return;
	}

    /* put document name in rgchDocName */
    lstrcpy((LPSTR) rgchDocName, (LPSTR) rgchAppName);
    lstrcat((LPSTR) rgchDocName, (LPSTR) " - ");
    if (!fUntitled)
	lstrcat((LPSTR) rgchDocName, (LPSTR) rgchFilename);
    else
	lstrcat((LPSTR) rgchDocName, (LPSTR) szUntitled);


    /* set up abort dialog box */
    lpDlgFnAbort = MakeProcInstance((FARPROC) DlgFnAbort, hInst);
    EnableWindow(hWnd, FALSE);
    hWndAbort = CreateDialog(hInst,
			     MAKEINTRESOURCE(iddlg),
			     hWnd,
			     lpDlgFnAbort);

    /* give abort procedure to gdi to check abort print command */
    lpAbortTest = MakeProcInstance((FARPROC) AbortTest, hInst);
    Escape(hPrinterDC, SETABORTPROC, 0, (LPSTR) lpAbortTest, (LPSTR) 0L);

    /* send document name to spooler */
    if ((ierr = Escape(hPrinterDC, STARTDOC, lstrlen((LPSTR) rgchDocName),
		      (LPSTR) rgchDocName, (LPSTR)0L)) < 0)
	{
	PrintErr(hWnd, ierr);
	}
    /* do nextband only if startdoc succeeds */
    else if ((ierr = Escape(hPrinterDC, NEXTBAND, 0, (LPSTR) NULL,
			    (LPSTR) &rectBand)) < 0)
	{
	PrintErr(hWnd, ierr);
	}
    /* spooler accepting document; check for user input to abort dialog box */
    else
	{
	(*lpAbortTest)((HDC) NULL, 0);
	}

    /* Get size of printer page. In this sample, the source */
    /* is conceptually the same size as the printer page.   */
    dxFile = dxPrinter = GetDeviceCaps(hPrinterDC, HORZRES);
    dyFile = dyPrinter = GetDeviceCaps(hPrinterDC, VERTRES);

    /* Loop to transfer one band at a time to the spooler. */
    while (!fAbortPrint &&
	  (rectBand.left != 0 || rectBand.right  != 0 ||
	   rectBand.top  != 0 || rectBand.bottom != 0))
	{
	/* get upper left corner of source band */
	x0Src = rectBand.left;
	y0Src = rectBand.top;

	/* if source band is within bounds of source document... */
	if (x0Src < dxFile && y0Src < dyFile)
	    {
	    /* get dimensions of source and printer band */
	    dxSrc = dxBand = rectBand.right - rectBand.left;
	    dySrc = dyBand = rectBand.bottom - rectBand.top;

	    /* send the source band to the spooler */
	    if (dxSrc > 0 && dySrc > 0)
		{
		(*lpAbortTest)((HDC) NULL, 0);
		/*** here would go the actual code or procedure ***/
		/*** call to compute what to send the current	***/
		/*** printer band.   This sample application	***/
		/*** just sends a black band using PatBlt.	***/
		PatBlt(hPrinterDC, rectBand.left, rectBand.top, dxBand, dyBand, BLACKNESS);
		(*lpAbortTest)((HDC) NULL, 0);
		}
	    }

	(*lpAbortTest)((HDC) NULL, 0);

	/* get coordinates of next printer band */
	if ((ierr = Escape(hPrinterDC, NEXTBAND, 0, (LPSTR) NULL,
			   (LPSTR) &rectBand)) < 0)
	    {
	    PrintErr(hWnd, ierr);
	    }

	} /* end while */

    if (!fAbortPrint)
	{ /* sent all of document without problems; end it. */
	Escape(hPrinterDC, ENDDOC, 0, (LPSTR) NULL, (LPSTR)0L);

	if (hWndAbort != NULL) /* abort dialog box still up */
	    {
	    /* enable parent before destroy */
	    EnableWindow(hWnd, TRUE);
	    DestroyWindow(hWndAbort);
	    /* clear abort window handle after destroy */
	    hWndAbort = NULL;
	    }
	}
    else
	{ /* abort print command sent.	remove document from spooler. */
	Escape(hPrinterDC, ABORTDOC, 0, (LPSTR) NULL, (LPSTR)0L);
	}

    DeleteDC(hPrinterDC);

    FreeProcInstance(lpDlgFnAbort);
    FreeProcInstance(lpAbortTest);

} /* end printfile */


/*==============================================================================
 ABORTTEST is passed to gdi to allow the spooler to call it during spooling to let
 the application cancel the print job or to handle out-of-disk-space conditions.
 All it does is look in the message queue and see if the next message is for the
 abort dialog box, and if it is it sends it to the dialog box, and if it is not
 it is translated and dispatched like normal.
===============================================================================*/
int FAR PASCAL AbortTest (hDC, iReserved)
HDC	hDC;
int	iReserved;
{
    MSG msg;

    while (!fAbortPrint && PeekMessage((LPMSG) &msg, NULL, NULL, NULL, TRUE)) {
	    if ((hWndAbort == NULL) || !IsDialogMessage(hWndAbort, (LPMSG) &msg)) {
		    TranslateMessage((LPMSG) &msg);
		    DispatchMessage((LPMSG) &msg);
		    }
	    }

    return (!fAbortPrint);

} /* end aborttest */


/*============================================================================
 DLGFNABORT controls the modeless Abort dialog box.  It is called whenever a
 print job is sent to the spooler.
=============================================================================*/
int FAR PASCAL DlgFnAbort (hDlg, msg, wParam, lParam)
HWND	hDlg;
unsigned msg;
WORD	wParam;
LONG	lParam;
{
    switch (msg)
	{
	case WM_COMMAND:
	    fAbortPrint = TRUE;
	    EnableWindow(GetParent(hDlg), TRUE);
	    DestroyWindow(hDlg);
	    hWndAbort = NULL;
	    return (TRUE);

	case WM_INITDIALOG:
	    hmenuSys = GetSystemMenu(hDlg, FALSE);
	    SetWindowText(hDlg, (LPSTR)rgchAppName);
	    if (fUntitled)
		SetDlgItemText(hDlg, ID_NAME, (LPSTR)szUntitled);
	    else
		SetDlgItemText(hDlg, ID_NAME, (LPSTR)rgchFilename);
	    CenterPopup(hDlg);
	    SetFocus(hDlg);
	    return (TRUE);

	case WM_INITMENU:
	    EnableMenuItem(hmenuSys, SC_CLOSE, MF_GRAYED);
	    return (TRUE);

	default:
	    return (FALSE);

	} /* end case */

} /* end DlgFnAbort */


/*=============================================================================
 GETPRINTERDC reads the device string line from WIN.INI and trys to create a DC
 for  the device.  It rejects all DCs incapable of BITBLT.  It returns a DC or
 NULL if there is an error.
=============================================================================*/
HDC GetPrinterDC()
{
    char    *pchDevice,
	    *pchDriver,
	    *pchPort,
	    *pch;
    HDC     hDC;
    char    rgchProfile[81];

    GetProfileString((LPSTR) "Windows",
		     (LPSTR) "Device",
		     (LPSTR) NULL,
		     (LPSTR) rgchProfile,
		     81);

    if (*rgchProfile == NULL)
	return ((HANDLE)NULL);

    pch = rgchProfile;

    /* skip leading blanks */
    while (*pch != NULL && *pch == ' ') {
	    pch = (char *) AnsiNext((LPSTR) pch);
	    }

    pchDevice = pch;

    /* find end of device name */
    while (*pch != NULL && *pch != ',') {
	    pch = (char *) AnsiNext((LPSTR) pch);
	    }

    /* if there is no device, we can't output */
    if (*pch == NULL)
	return ((HANDLE)NULL);

    /* null terminate driver */
    *pch++ = NULL;

    /* skip leading blanks */
    while (*pch != NULL && *pch == ' ') {
	    if (*pch == '.') *pch = NULL;
	    pch = (char *) AnsiNext((LPSTR) pch);
	    }

    pchDriver = pch;

    /* find end of driver name */
    while (*pch != NULL && *pch != ',') {
	    pch = (char *) AnsiNext((LPSTR) pch);
	    }

    /* if the is no driver for device, we can't output */
    if (*pch == NULL)
	return ((HANDLE)NULL);

    /* null terminate device */
    *pch++ = NULL;

    /* skip leading blanks */
    while (*pch != NULL && *pch == ' ') {
	    pch = (char *) AnsiNext((LPSTR) pch);
	    }

    pchPort = pch;

    /* Create the DC. */
    hDC = CreateDC((LPSTR) pchDriver,
		   (LPSTR) pchDevice,
		   (LPSTR) pchPort,
		   (LPSTR) NULL);

    if (hDC != (HDC) NULL) {
	    /* if the DC cannot do BITBLT, ... */
	    if (!(GetDeviceCaps(hDC, RASTERCAPS) & RC_BITBLT)) {
		    DeleteDC(hDC);
		    hDC = (HDC) NULL;
		    }
	    }

    return (hDC);

}  /* end getprinterdc */


/*===================*/
 PrintErr (hWnd, ierr)	/* closes abort dialog box a tells why */
/*===================*/
HWND	hWnd;
int	ierr;
{
    WORD    ids;
    char    rgchError[128];

    fAbortPrint = TRUE;

    if (hWndAbort != NULL)
	{
	/* enable parent before destroy */
	EnableWindow(hWnd, TRUE);
	DestroyWindow(hWndAbort);
	/* clear abort window handle after destroy */
	hWndAbort = NULL;
	}

    if (ierr & SP_NOTREPORTED)
	{
	switch (ierr)
	    {
	    case SP_OUTOFDISK:
		ids = IDS_PRINTDISK;
		break;
	    case SP_OUTOFMEMORY:
		ids = IDS_PRINTMEM;
		break;
	    case SP_APPABORT:
	    case SP_USERABORT:
		return;
	    case SP_ERROR:
	    default:
		ids = IDS_PRINTERR;
		break;
	    }

	LoadString(hInst, ids, (LPSTR)rgchError, sizeof(rgchError));
	MessageBox(hWnd, (LPSTR)rgchError, (LPSTR)rgchAppName,
		   MB_ICONEXCLAMATION + MB_OK + MB_APPLMODAL);
	}

} /* end printerr */


/*================*/
 CenterPopup (hWnd) /* Center an invisible popup and make it visible. */
/*================*/
HWND	hWnd;
{
    POINT   ptCent;
    RECT    rectWnd;
    int     cx, cy;

    GetWindowRect(hWnd, (LPRECT) &rectWnd);
    cx = rectWnd.right - rectWnd.left;
    cy = rectWnd.bottom - rectWnd.top;
    GetScreenCenter(GetParent(hWnd), (LPPOINT) &ptCent, cx, cy);
    MoveWindow(hWnd,
	       ptCent.x - cx / 2,
	       ptCent.y - cy / 2,
	       cx,
	       cy,
	       FALSE);
    ShowWindow(hWnd, SHOW_OPENWINDOW);

} /* end centerpopup */


/*================================*/
 GetScreenCenter(hWnd, ppt, dx, dy)  /* get center in screen coords. */
/*================================*/
HWND	    hWnd;   /* handle to window on the screen */
LPPOINT     ppt;    /* ptr to point structure where center coords returned */
int	dx;	    /* width of window */
int	dy;	    /* height of window */
{
    HDC     hdcWnd;
    RECT    rect;
    int     dxScr;
    int     dyScr;

    hdcWnd = GetDC(hWnd);

    dxScr = GetDeviceCaps(hdcWnd, HORZRES);
    dyScr = GetDeviceCaps(hdcWnd, VERTRES);

    ReleaseDC(hWnd, hdcWnd);

    if (IsIconic(hWnd))
	{
	ppt->x = dxScr / 2;
	ppt->y = dyScr / 2;
	}
    else
	{
	GetWindowRect(hWnd, (LPRECT) &rect);

	ppt->x = (rect.right + rect.left) / 2;
	ppt->y = (rect.top + rect.bottom) / 2;

	if (ppt->x + dx / 2 > dxScr)
	    ppt->x = dxScr - dx / 2;
	else if (ppt->x - dx / 2 < 0)
	    ppt->x = dx / 2;

	if (ppt->y + dy / 2 > dyScr)
	    ppt->y = dyScr - dy / 2;
	else if (ppt->y - dy / 2 < 0)
	    ppt->y = dy / 2;
	}

} /* end getscreencenter */
