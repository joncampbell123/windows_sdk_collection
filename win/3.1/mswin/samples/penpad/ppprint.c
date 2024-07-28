/***************************************************************************
 *									   *
 *  MODULE	: PpPrint()						   *
 *									   *
 *  PURPOSE	: Printing code for penpad.				   *
 *									   *
 *  FUNCTIONS	: GetPrinterDC ()	   -  Creates a printer DC for the *
 *					      default device.		   *
 *									   *
 *		  AbortProc ()		   -  Export proc. for GDI to check*
 *					      print abort.		   *
 *									   *
 *		  PrintDlgProc ()	   -  Dialog function for the print*
 *					      cancel dialog.		   *
 *									   *
 *		  PrintFile ()		   -  Prints the contents of the   *
 *					      edit control.		   *
 *									   *
 *		  GetInitializationData () -  Gets DC initialisation data  *
 *					      from a DC supporting	   *
 *					      ExtDeviceMode().		   *
 *									   *
 ***************************************************************************/
#include "penpad.h"

BOOL fAbort;		/* TRUE if the user has aborted the print job	 */
HWND hwndPDlg;		/* Handle to the cancel print dialog		 */
char szDevice[160];	/* Contains the device, the driver, and the port */
PSTR szDriver;		/* Pointer to the driver name			 */
PSTR szPort;		/* Port, ie, LPT1				 */
PSTR szTitle;		/* Global pointer to job title			 */
int iPrinter = 0;	/* level of available printer support.		 */
			/* 0 - no printer available			 */
			/* 1 - printer available			 */
			/* 2 - driver supports 3.0 device initialization */
HANDLE hInitData=NULL;	/* handle to initialization data		 */

char szExtDeviceMode[] = "EXTDEVICEMODE";

int FAR PASCAL AbortProc (HDC,WORD);		 /* Prototype  */ 

/****************************************************************************
 *									    *
 *  FUNCTION   : GetPrinterDC ()					    *
 *									    *
 *  PURPOSE    : Creates a printer display context for the default device.  *
 *		 As a side effect, it sets the szDevice and szPort variables*
 *		 It also sets iPrinter to the supported level of printing.  *
 *									    *
 *  RETURNS    : HDC   - A handle to printer DC.			    *
 *									    *
 ****************************************************************************/
HDC FAR PASCAL GetPrinterDC(void)
{
    HDC      hdc;
    LPSTR    lpdevmode = NULL;

    iPrinter = 0;

    /* Get the printer information from win.ini into a buffer and
     * null terminate it.
     */
    GetProfileString ( "windows", "device", "" ,szDevice, sizeof(szDevice));
    for (szDriver = szDevice; *szDriver && *szDriver != ','; szDriver++)
	;
    if (*szDriver)
	*szDriver++ = 0;

    /* From the current position in the buffer, null teminate the
     * list of ports
     */
    for (szPort = szDriver; *szPort && *szPort != ','; szPort++)
	;
    if (*szPort)
	*szPort++ = 0;

    /* if the device, driver and port buffers all contain meaningful data,
     * proceed.
     */
    if (!*szDevice || !*szDriver || !*szPort){
	*szDevice = 0;
	return NULL;
    }

    /* Create the printer display context */
    if (hInitData){
	/* Get a pointer to the initialization data */
	lpdevmode = (LPSTR) LocalLock (hInitData);

	if (lstrcmp (szDevice, lpdevmode)){
	    /* User has changed the device... cancel this setup, as it is
	     * invalid (although if we worked harder we could retain some
	     * of it).
	     */
	    lpdevmode = NULL;
	    LocalUnlock (hInitData);
	    LocalFree (hInitData);
	    hInitData = NULL;
	}
    }
    hdc = CreateDC (szDriver, szDevice, szPort, lpdevmode);

    /* Unlock initialization data */
    if (hInitData)
	LocalUnlock (hInitData);

    if (!hdc)
	return NULL;


    iPrinter = 1;

    /* Find out if ExtDeviceMode() is supported and set flag appropriately */
    if (GetProcAddress (GetModuleHandle (szDriver), szExtDeviceMode))
	iPrinter = 2;

    return hdc;

}

/****************************************************************************
 *									    *
 *  FUNCTION   : AbortProc()						    *
 *									    *
 *  PURPOSE    : To be called by GDI print code to check for user abort.    *
 *									    *
 ****************************************************************************/
int FAR PASCAL AbortProc ( hdc, reserved )
HDC hdc;
WORD reserved;
{
    MSG msg;

	 hdc;					/* Needed to prevent compiler warning message */
	 reserved;			/* Needed to prevent compiler warning message */

    /* Allow other apps to run, or get abort messages */
    while (!fAbort && PeekMessage (&msg, NULL, NULL, NULL, TRUE))
	if (!hwndPDlg || !IsDialogMessage (hwndPDlg, &msg)){
	    TranslateMessage (&msg);
	    DispatchMessage  (&msg);
	}
    return !fAbort;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : PrintDlgProc ()					    *
 *									    *
 *  PURPOSE    : Dialog function for the print cancel dialog box.	    *
 *									    *
 *  RETURNS    : TRUE  - OK to abort/ not OK to abort			    *
 *		 FALSE - otherwise.					    *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL PrintDlgProc(HWND hwnd, WORD msg, WORD wParam, LONG lParam)
{
	 wParam;					/* Needed to prevent compiler warning message */
	 lParam;					/* Needed to prevent compiler warning message */

    switch (msg){
	case WM_INITDIALOG:
	    /* Set up information in dialog box */
	    SetDlgItemText (hwnd, IDD_PRINTDEVICE, (LPSTR)szDevice);
	    SetDlgItemText (hwnd, IDD_PRINTPORT, (LPSTR)szPort);
	    SetDlgItemText (hwnd, IDD_PRINTTITLE, (LPSTR)szTitle);
	    break;

	case WM_COMMAND:
	    /* abort printing if the only button gets hit */
	    fAbort = TRUE;
	    break;

	default:
	    return FALSE;
    }
    return TRUE;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : PrintFile ()						    *
 *									    *
 *  PURPOSE    : Prints the contents of the edit control.		    *
 *									    *
 ****************************************************************************/

VOID FAR PASCAL PrintFile(HWND hwnd)
{
    HDC     hdc;
    int     yExtPage;
    char    sz[32];
    WORD    cch;
    WORD    ich;
    PSTR    pch;
    WORD    iLine;
    WORD    nLinesEc;
    HANDLE  hT;
    FARPROC lpfnAbort;
    FARPROC lpfnPDlg;
    HWND    hwndPDlg;
    WORD    dy;
    int     yExtSoFar;
    WORD    fError = TRUE;
    HWND    hwndEdit;

    hwndEdit = (HWND)GetWindowWord(hwnd,GWW_HWNDEDIT);

    /* Create the job title by loading the title string from STRINGTABLE */
    cch = LoadString (hInst, IDS_PRINTJOB, sz, sizeof(sz));
    szTitle = sz + cch;
    cch += GetWindowText (hwnd, sz + cch, 32 - cch);
    sz[31] = 0;

    /* Make instances of the Abort proc. and the Print dialog function */
    lpfnAbort = MakeProcInstance (AbortProc, hInst);
    if (!lpfnAbort)
	goto getout;
    lpfnPDlg = MakeProcInstance (PrintDlgProc, hInst);
    if (!lpfnPDlg)
	goto getout4;

    /* Initialize the printer */
    hdc = GetPrinterDC();
    if (!hdc)
	goto getout5;

    /* Disable the main application window and create the Cancel dialog */
    EnableWindow (hwndFrame, FALSE);
    hwndPDlg = CreateDialog (hInst, IDD_PRINT, hwnd, lpfnPDlg);
    if (!hwndPDlg)
	goto getout3;
    ShowWindow (hwndPDlg, SW_SHOW);
    UpdateWindow (hwndPDlg);

    /* Allow the app. to inform GDI of the escape function to call */
    if (Escape (hdc, SETABORTPROC, 0, (LPSTR)lpfnAbort, NULL) < 0)
	goto getout1;

    /* Initialize the document */
    if (Escape (hdc, STARTDOC, cch, (LPSTR)sz, NULL) < 0)
	goto getout1;

    /* Get the height of one line and the height of a page */
    dy = HIWORD (GetTextExtent (hdc, "CC", 2));
    yExtPage = GetDeviceCaps (hdc, VERTRES);

    /* Get the lines in document and and a handle to the text buffer */
    iLine     = 0;
    yExtSoFar = 0;
    nLinesEc  = (WORD)SendMessage (hwndEdit, EM_GETLINECOUNT, 0, 0L);
    hT	      = (HANDLE)SendMessage (hwndEdit, EM_GETHANDLE, 0, 0L);

    /* While more lines print out the text */
    while (iLine < nLinesEc){
	if (yExtSoFar + (int)dy > yExtPage){
	    /* Reached the end of a page. Tell the device driver to eject a
	     * page
	     */
	    if (Escape (hdc, NEWFRAME, 0, NULL, NULL) < 0 || fAbort)
		goto getout2;
	    yExtSoFar = 0;
	}

	/* Get the length and position of the line in the buffer
	 * and lock from that offset into the buffer */
	ich = (WORD)SendMessage (hwndEdit, EM_LINEINDEX, iLine, 0L);
	cch = (WORD)SendMessage (hwndEdit, EM_LINELENGTH, ich, 0L);
	pch = LocalLock(hT) + ich;

	/* Print the line and unlock the text handle */
	TextOut (hdc, 0, yExtSoFar, (LPSTR)pch, cch);
	LocalUnlock (hT);

	/* Test and see if the Abort flag has been set. If yes, exit. */
	if (fAbort)
	    goto getout2;

	/* Move down the page */
	yExtSoFar += dy;
	iLine++;
    }

    /* Eject the last page. */
    if (Escape (hdc, NEWFRAME, 0, NULL, NULL) < 0)
	goto getout2;

    /* Complete the document. */
    if (Escape (hdc, ENDDOC, 0, NULL, NULL) < 0){
getout2:
	/* Ran into a problem before NEWFRAME? Abort the document */
	Escape( hdc, ABORTDOC, 0, NULL, NULL);
    }
    else
	fError=FALSE;

getout3:
    /* Close the cancel dialog and re-enable main app. window */
    EnableWindow (hwndFrame, TRUE);
    DestroyWindow (hwndPDlg);

getout1:
    DeleteDC(hdc);

getout5:
    /* Get rid of dialog procedure instances */
    FreeProcInstance (lpfnPDlg);

getout4:
    FreeProcInstance (lpfnAbort);

getout:

    /* Error? make sure the user knows... */
    if (fError)
	PPError (hwnd, MB_OK | MB_ICONEXCLAMATION, IDS_PRINTERROR, (LPSTR)szTitle);

    return;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : GetInitializationData()				    *
 *									    *
 *  PURPOSE    : Gets DC initialization data from a printer driver	    *
 *		 supporting ExtDeviceMode(). Called in response to the	    *
 *		 File/Printer setup menu selection.			    *
 *									    *
 *		 This function allows the user to change the printer	    *
 *		 settings FOR PENPAD ONLY.  This allows Penpad to print *
 *		 in a variety of settings without messing up any other	    *
 *		 applications. In a more sophisticated application, this    *
 *		 setup could even be saved on a document-by-document basis. *
 *									    *
 ****************************************************************************/
BOOL FAR PASCAL GetInitializationData( hwnd )
HWND hwnd ;
{
    LPSTR     lpOld;
    LPSTR     lpNew;
    int (FAR PASCAL *lpfn)(HWND, HANDLE, LPDEVMODE, LPSTR, LPSTR, LPDEVMODE, LPSTR, WORD);
    HANDLE    hT,hDrv;
    char      sz[32];
    WORD      cb;
    int       flag;

    /* Pop up dialog for user and retain data in app buffer */
    flag = DM_PROMPT | DM_COPY;

    /* Load the device driver and find the ExtDeviceMode() function */
    wsprintf (sz, "%s.drv", (LPSTR)szDriver);
    if ((hDrv = LoadLibrary (sz)) < 32)
	return FALSE;
    if (!((FARPROC)lpfn = GetProcAddress (hDrv, szExtDeviceMode)))
	return FALSE;

    if (hInitData){
	/* We have some old data... we want to modify the previously specified
	 * setup rather than starting with the default setup.
	 */
	lpOld = (LPSTR)LocalLock(hInitData);
	flag |= DM_MODIFY;
    }
    else
	lpOld = NULL;

    /* Get the number of bytes needed for the init data */
    cb = (*lpfn) (hwnd,
		  hDrv,
		  NULL,
		  (LPSTR)szDevice,
		  (LPSTR)szPort,
		  (LPDEVMODE)NULL,
		  (LPSTR)NULL,
		  0);

    /* Grab some memory for the new data and lock it. */
    hT	  = LocalAlloc (LHND,cb);
    lpNew = (LPSTR)LocalLock (hT);

    /* Post the device mode dialog. 0 flag iff user hits OK button */
    if ((*lpfn) (hwnd,
		 hDrv,
		 (LPDEVMODE)lpNew,
		 (LPSTR)szDevice,
		 (LPSTR)szPort,
		 (LPDEVMODE)lpOld,
		 (LPSTR)NULL,
		 flag)==IDOK)
	flag = 0;

    /* Unlock the input structures */
    LocalUnlock (hT);
    if (hInitData)
	LocalUnlock (hInitData);

    /* If the user hit OK and everything worked, free the original init.
     * data and retain the new one.  Otherwise, toss the new buffer.
     */
    if (flag)
	LocalFree (hT);
    else{
	if (hInitData)
	    LocalFree (hInitData);
	hInitData = hT;
    }

    FreeLibrary(hDrv);
    return (!flag);
}
