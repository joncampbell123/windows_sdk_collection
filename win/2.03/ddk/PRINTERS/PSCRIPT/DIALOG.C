/*********************************************************************
 * DIALOG.C
 *
 * 7Jan87	sec		fixed setting of gl_dloaded
 * 23Jan87	sec		added junk for new dialog
 * 9Mar87	sjp		added IBM stuff
 * 7Apr87	sjp		added DataProducts LZR 2665 (1st cut)
 * 8Apr87	sjp		added DEC LN03R ScriptPrinter
 * 14Apr87	sjp		included printers.h--and printer caps stuff
 * 17Apr87	sjp		added greater paper and tray functionality.
 * 3Jun87	sjp		Modified WriteProfile() and ReadProfile() to save
 *					the orientation state in the win.ini, also some
 *					error checking in ReadProfile().
 * 3Jun87	sjp		Added MakeEnvironment() and SaveEnvironment().
 * 14Aug87	sjp		Moved MapProfile(), GetPaperType() and ReadProfile()
 *					to new segment PROFILE.
 *********************************************************************
 */

#include "pscript.h"
#include "printers.h"

/****************************************************************************/
/*#define DEBUG_ON*/
#ifdef DEBUG_ON
#define DBMSG(msg) printf msg
#else
#define DBMSG(msg)
#endif

/*#define DEBUG1_ON*/
#ifdef DEBUG1_ON
#define DBMSG1(msg) printf msg
#else
#define DBMSG1(msg)
#endif

/*#define DEBUG2_ON*/
#ifdef DEBUG2_ON
#define DBMSG2(msg) printf msg
#else
#define DBMSG2(msg)
#endif
/****************************************************************************/

DEVMODE devmode;
HINST hinstDlg;

extern char szKey[];     /* The profile key */

extern char szPaper[];

extern BOOL gl_dloaded;	/* header (supposedly) downloaded */
extern BOOL gl_tm;		/* "tile mode" (big printable area) */
extern BOOL gl_lie;		/* "little lie" (printable area = paper size) */

void FAR PASCAL SetKey(LPSTR);
void FAR PASCAL ReadProfile(HANDLE, DEVMODE FAR *, LPSTR);
int FAR PASCAL GetPaperType(HANDLE, int);
int FAR PASCAL MapProfile(HANDLE, LPSTR, int, int, int);

void FAR PASCAL SetWindowText(HWND, LPSTR);
int  FAR PASCAL GetWindowText(HWND, LPSTR, int);

void FAR PASCAL SetDlgItemText(HWND, int, LPSTR);
BOOL FAR PASCAL fnDialog(HWND, unsigned, WORD, LONG);

BOOL FAR PASCAL isUSA();
void FAR PASCAL lstrcpy(LPSTR, LPSTR);
int  FAR PASCAL lstrlen(LPSTR);
BOOL FAR PASCAL lstrcmp(LPSTR, LPSTR);
void FAR PASCAL lstrncat(LPSTR, LPSTR, int);

extern FAR PASCAL HeapInit(int);

/* holding array for the port which the driver will write to
 * 13 places = 8 for file name + 1 for '.' + 3 for extension + 1 for '\0'
 */
char gPort[13];

BOOL FAR PASCAL EnableWindow(HWND,BOOL);


/********************************************************************/

void PASCAL WriteProfileInt(lszApp, lszKey, iVal)
LPSTR lszApp;
LPSTR lszKey;
int iVal;
    {
    LPSTR  lpbDst;
    char sz[10];

    lpbDst = ((LPSTR) sz) + sizeof(sz);
    *--lpbDst = 0;
    while (iVal!=0)
        {
        *--lpbDst = (char)((iVal % 10) + (int)'0');
        iVal = iVal / 10;
        }
    if (*lpbDst==0)
        *--lpbDst = '0';
    WriteProfileString(lszApp, lszKey, lpbDst);
    }


/*******************************************************************
* Name: WriteProfile()
*
* Action: Write the device mode parameters out to win.ini.
*
********************************************************************
*/
void PASCAL WriteProfile(hinst, lszFile)
HANDLE hinst;	    /* The module instance handle */
LPSTR lszFile;	    /* Ptr to the com port's file name */
{

    char sz[64];
    LPSTR lpbDst;
    int iRes;
    int ids;
    int iFeed;


    SetKey(lszFile);

    ids = IDS_PR1 + devmode.iPrinter;
    LoadString(hinst, ids, (LPSTR) sz, sizeof(sz));
    WriteProfileString((LPSTR)szKey, (LPSTR) "device", (LPSTR) sz);

    WriteProfileInt((LPSTR) szKey, (LPSTR) "resolution", devmode.iRes);
    WriteProfileInt((LPSTR) szKey, (LPSTR) "orientation", devmode.fLandscape);
    WriteProfileInt((LPSTR) szKey, (LPSTR) "papersource", devmode.iFeed);


    for (iFeed=0; iFeed<=(IFEEDMAX - IFEEDMIN); ++iFeed){
        ids = IDS_PAPER0 + devmode.rgiPaper[iFeed] - LETTER;
        LoadString(hinst, ids, (LPSTR) sz, sizeof(sz));

        szPaper[sizeof("paperX") - 2] = (char)(iFeed + (int)'0');
        WriteProfileString((LPSTR)szKey, (LPSTR) szPaper, (LPSTR) sz);
    }
}


/********************************************************************/

void FAR PASCAL MakeEnvironment(HINST,LPSTR,LPSTR,LPDEVMODE);
void FAR PASCAL MakeEnvironment(hinst,lszDevice,lszFile,lpdevmode)
	HINST hinst;
	LPSTR lszDevice;
	LPSTR lszFile;
	LPDEVMODE lpdevmode;
{
	lstrcpy((LPSTR)lpdevmode->szDevice, lszDevice);

    if ((short) !GetEnvironment(lszFile, (LPSTR)lpdevmode, sizeof(DEVMODE))
		|| lstrcmp(lszDevice, (LPSTR)lpdevmode->szDevice))
	{
		/* always succeeds! */
		ReadProfile(hinst, lpdevmode, lszFile);
		lstrcpy((LPSTR)lpdevmode->szDevice, lszDevice);
	}
}


/********************************************************************/

void FAR PASCAL SaveEnvironment(HINST,LPSTR,LPSTR,LPDEVMODE,short,short);
void FAR PASCAL SaveEnvironment(hinst,lszDevice,lszFile,lpdevmode,mode1,mode2)
	HINST hinst;
	LPSTR lszDevice;
	LPSTR lszFile;
	LPDEVMODE lpdevmode;
	short mode1; /* 0=don't write profile, 1=write it */
	short mode2; /* 0=don't send message, 1=send message */
{
    SetEnvironment(lszFile, (LPSTR)lpdevmode, sizeof(DEVMODE));

    if(mode1) WriteProfile(hinst, lszFile);

    if(mode2) SendMessage((HWND)-1,WM_DEVMODECHANGE,(WORD)0,(LONG) lszDevice);
}


/*********************************************************************
* Name: DeviceMode()
*
* Action: Put up a dialog to get the new device mode settings.
*	  This routine is intended to be used in conjunction
*	  with the SetEnvironment and GetEnvironment routines
*	  to set up a dialog box that displays the current device
*	  environment and permits changes to be made.
*
**********************************************************************
*/
BOOL FAR PASCAL DeviceMode(hwnd, hinst, lszDevice, lszFile)
HWND hwnd;	    /* The window handle */
HINST hinst;	    /* The instance handle */
LPSTR lszDevice;      /* Far ptr to the device name string */
LPSTR lszFile;	    /* Far ptr to the output file name */
{

    static BOOL fIsBusy = FALSE;
    LPFN lpfn;

	DBMSG2(((LPSTR)">DeviceMode()\r"));

	/* save a copy of the output port name for later use by
	 * fnDialog
	 */
	lstrcpy((LPSTR)gPort,lszFile);

    /* Make sure we don't do two devmodes for this driver at once */
    if (fIsBusy) return(FALSE);
    else fIsBusy = TRUE;

	/* make the environment...doesn't fail */
	MakeEnvironment(hinst,lszDevice,lszFile,(LPDEVMODE)&devmode);

    hinstDlg = hinst;

    lpfn = MakeProcInstance((FARPROC) fnDialog, hinst);

/*	MessageBox(hwnd,(LPSTR)"Ready to open dialog box.",
 *		(LPSTR)"dialog.c",MB_OK | MB_ICONEXCLAMATION);
 */
    /* Put up the device mode dialog box */
    if (DialogBox(hinst, (LPSTR) "dtMode", hwnd, lpfn)==IDCANCEL){
		/* Return false if it was cancelled */
		fIsBusy = FALSE;
		FreeProcInstance(lpfn);
		DBMSG2(((LPSTR)"<DialogBox(): IDCANCEL\r"));
		return(FALSE);
	}
    FreeProcInstance(lpfn);

	DBMSG2(((LPSTR)" DeviceMode(): file=%ls, dev=%ls\r",lszFile,lszDevice));

    /* Update the environment with the new info and inform everyone */
	SaveEnvironment(hinst,lszDevice,lszFile,(LPDEVMODE)&devmode,
		1/*write profile*/,1/*send message*/);

    fIsBusy = FALSE;

	DBMSG2(((LPSTR)"<DeviceMode():\r"));
    return(TRUE);
}


/***********************************************************************/

BOOL PASCAL EnableTrays(hwnd, iPrinter)
HWND hwnd;
int iPrinter;
{
	PRINTER thePrinter;

	int i,iGrayFeed;
	char specialTray[SPECIALSTRINGSIZE];

	DBMSG1(((LPSTR)">EnableTrays(): iP=%d\r",iPrinter));

	if(!GetPrinterCaps(hinstDlg,iPrinter,(LPPRINTER)&thePrinter)){
		goto ERROR;
	}

	/* replace the upper tray entry with whatever is specified in
	 * in the printer capability structure
	 */
	DBMSG1(((LPSTR)"tP.s=%d\r",thePrinter.special));
	LoadString(hinstDlg,thePrinter.special,
		(LPSTR)specialTray,SPECIALSTRINGSIZE);
	DBMSG1(((LPSTR)"sT=%ls\r",(LPSTR)specialTray));
	SetDlgItemText(hwnd,UPPERTRAY,(LPSTR)specialTray);

	/* set the radio buttons to black or gray depending on the state
	 * of the booleans
	 */
	for(i=IFEEDMIN;i<=IFEEDMAX;i++){
		EnableWindow(GetDlgItem(hwnd,i),thePrinter.feed[i-IFEEDMIN]);
	}
	for(i=IPAPERMIN;i<=IPAPERMAX;i++){
		EnableWindow(GetDlgItem(hwnd,i),thePrinter.paper[i-IPAPERMIN]);
	}

	/* If the selected printer does not support the previously 
	 * selected paper source, then default to upper--this assumes
	 * that UPPERTRAY is always available on all printers, whether
	 * UPPERTRAY is "Upper", "Auto", etc.
	 */
	if(!thePrinter.feed[devmode.iFeed]){
		devmode.iFeed=UPPERTRAY-IFEEDMIN;
        CheckRadioButton(hwnd, IFEEDMIN, IFEEDMAX,UPPERTRAY);
	}
	/* If the selected printer does not support the previously 
	 * selected paper size then default to letter--this assumes that
	 * letter is always available on all printers.
	 */
	if(!thePrinter.paper[devmode.rgiPaper[devmode.iFeed]-IPAPERMIN]){
		devmode.rgiPaper[devmode.iFeed] = LETTER;
		CheckRadioButton(hwnd, IPAPERMIN, IPAPERMAX,
			devmode.rgiPaper[devmode.iFeed]);
	}

    devmode.iPrinter = iPrinter;

	DBMSG1(((LPSTR)"<EnableTrays()\r"));
	return(TRUE);

ERROR:
	DBMSG1(((LPSTR)"<EnableTrays(): ERROR\r"));
	return(FALSE);
}


/******************************************************************
* Name: fnDialog()
*
* Action: This is a callback function that handles the dialog
*	  messages sent from Windows.  The dialog is initiated
*	  from the DeviceMode function.
*
********************************************************************
*/

BOOL FAR PASCAL fnDialog(hwnd, uMsg, wParam, lParam)
HWND hwnd;
unsigned uMsg;
WORD wParam;
LONG lParam;
    {

    int i;
    HWND hwndCtl;
	BOOL fValOk;
    int ids;
    int iRes, oldRes;
    int iCopies;
    char szPrinter[40];

	DBMSG(((LPSTR)">fnDialog(): msg=%d,wP=%d,lP=%d\r",uMsg,wParam,lParam));

    switch(uMsg)
	{
	case WM_INITDIALOG:
		DBMSG(((LPSTR)"fnDialog(): WM_INITDIALOG\r"));
		{
			char caption[64];	/* to hold the dialog caption */
			int captionLen;

			/* get the dialog box caption and add the name of the currently
			 * selected port to it
			 */
			captionLen=GetWindowText(hwnd,(LPSTR)caption,sizeof(caption));
			DBMSG(((LPSTR)"fnDialog(): caption=%ls,cL=%d,sz=%d\r",
				(LPSTR)caption,captionLen,sizeof(caption)));

			/* check to see if present caption is already too large */
			if(captionLen<sizeof(caption)-1){
				lstrncat((LPSTR)caption,gPort,sizeof(caption));
			}
			DBMSG(((LPSTR)"fnDialog(): caption=%ls,sz=%d\r",
				(LPSTR)caption,sizeof(caption)));
			SetWindowText(hwnd,(LPSTR)caption);
		}

	    SendDlgItemMessage(hwnd, EDITFIELD, EM_LIMITTEXT, 4, (long) NULL);
	    SetDlgItemInt(hwnd, COPIES, devmode.iCopies, FALSE);

		CheckRadioButton(hwnd, PORTRAIT, LANDSCAPE,
			devmode.fLandscape ? LANDSCAPE : PORTRAIT);

	    SetDlgItemInt(hwnd, EDITFIELD, devmode.iRes, FALSE);

		switch (devmode.iRes) {
			case 75:
				CheckRadioButton (hwnd, RESMIN, RESMAX, RES75);
				break;
			case 100:
				CheckRadioButton (hwnd, RESMIN, RESMAX, RES100);
				break;
			case 150:
				CheckRadioButton (hwnd, RESMIN, RESMAX, RES150);
				break;
			case 300:
				CheckRadioButton (hwnd, RESMIN, RESMAX, RES300);
				break;
			default:
				CheckRadioButton (hwnd, RESMIN, RESMAX, RESCUSTOM);
				break;
		}

		CheckRadioButton(hwnd, IPAPERMIN, IPAPERMAX, devmode.iPaper);
		CheckRadioButton(hwnd, IFEEDMIN, IFEEDMAX, devmode.iFeed + IFEEDMIN);

		for (ids=IDS_PR1; ids<=IDS_PRMAX; ++ids){
			LoadString(hinstDlg, ids, (LPSTR) szPrinter, sizeof(szPrinter));
			SendDlgItemMessage(hwnd, LISTFIELD, LB_INSERTSTRING, (WORD)-1,
				(DWORD) (LPSTR) szPrinter);
		}

	    SendDlgItemMessage(hwnd, LISTFIELD, LB_SETCURSEL, devmode.iPrinter, (DWORD) 0);

		EnableWindow(GetDlgItem(hwnd, UPPERTRAY), TRUE);

		/* Get the printer capabilities...the only way this can
		 * fail is if the resource is not there or the memory
		 * cannot be locked or freed.
		 */
		if(!EnableTrays(hwnd, devmode.iPrinter)){
			goto END_DLG;
		}
	    hwndCtl = GetDlgItem(hwnd, devmode.fLandscape ? LANDSCAPE : PORTRAIT);
	    SetFocus(hwndCtl);

		DBMSG(((LPSTR)"<fnDialog() FALSE\r"));
	    return(FALSE);
	    break;

	case WM_COMMAND:
		DBMSG(((LPSTR)"fnDialog(): WM_COMMAND\r"));
	    switch(wParam){
			case PORTRAIT:
			case LANDSCAPE:
				DBMSG(((LPSTR)"fnDialog():  orientation\r"));
			    devmode.fLandscape = (wParam == LANDSCAPE);
			    CheckRadioButton(hwnd, PORTRAIT, LANDSCAPE, wParam);
		    break;

			case LETTER:
			case LEGAL:
			case TABLOID:
			case STATEMENT:
			case DINA3:
			case DINA4:
			case DINA5:
			case DINB4:
			case DINB5:
				DBMSG(((LPSTR)"fnDialog():  paper\r"));
				devmode.rgiPaper[devmode.iFeed] = wParam;
			    CheckRadioButton(hwnd, IPAPERMIN, IPAPERMAX, wParam);
		    break;

			case UPPERTRAY:
			case LOWERTRAY:
			case MANUALFEED:
				DBMSG(((LPSTR)"fnDialog():  tray\r"));
				if (devmode.iFeed != (wParam - IFEEDMIN)){
					devmode.iFeed = wParam - IFEEDMIN;
					CheckRadioButton(hwnd, IPAPERMIN, IPAPERMAX,
						devmode.rgiPaper[devmode.iFeed]);
					CheckRadioButton(hwnd, IFEEDMIN, IFEEDMAX, wParam);
				}
   	         break;


	         case LISTFIELD:

				DBMSG(((LPSTR)"fnDialog():  LISTFIELD\r"));
	            i = HIWORD(lParam);
				if (i==LBN_SELCHANGE){
	                i = (int)SendDlgItemMessage(hwnd, LISTFIELD, LB_GETCURSEL,
						0, 0L);
					/* Get the printer capabilities...the only way this can
					 * fail is if the resource is not there or the memory
					 * cannot be locked or freed.
					 */
					if(!EnableTrays(hwnd, i)){
						goto END_DLG;
					}
					if (i == LINOTYPE){
						devmode.iRes = 1270;
						SetDlgItemInt (hwnd, EDITFIELD, devmode.iRes, 0);
						CheckRadioButton (hwnd, RESMIN, RESMAX, RESCUSTOM);
					}else{
						devmode.iRes = 300;
						SetDlgItemInt (hwnd, EDITFIELD, devmode.iRes, 0);
						CheckRadioButton (hwnd, RESMIN, RESMAX, RES300);
					}
					devmode.iPrinter = i;
				}
			break;

			case RES75:
				DBMSG(((LPSTR)"fnDialog():  RES75\r"));
				iRes = 75;
				goto RES;
			case RES100:
				DBMSG(((LPSTR)"fnDialog():  RES100\r"));
				iRes = 100;
				goto RES;
			case RES150:
				DBMSG(((LPSTR)"fnDialog():  RES150\r"));
				iRes = 150;
				goto RES;
			case RES300:
				DBMSG(((LPSTR)"fnDialog():  RES300\r"));
				iRes = 300;
				goto RES;

RES:			devmode.iRes = iRes;
				SetDlgItemInt (hwnd, EDITFIELD, iRes, 0);
				CheckRadioButton (hwnd, RESMIN, RESMAX, wParam);
				break;
			case RESCUSTOM:
				DBMSG(((LPSTR)"fnDialog():  RESCUSTOM\r"));
				oldRes = devmode.iRes;
				iRes = GetDlgItemInt (hwnd, EDITFIELD, (BOOL FAR *) &fValOk, 0);
				if (!fValOk)
					iRes = oldRes;
				if (iRes < 50)
					iRes = 50;
				if (iRes > IRESMAX)
					iRes = IRESMAX;
				devmode.iRes = iRes;
				SetDlgItemInt (hwnd, EDITFIELD, iRes, 0);
				CheckRadioButton (hwnd, RESMIN, RESMAX, wParam);
				break;
			case EDITFIELD:
				DBMSG(((LPSTR)"fnDialog():  EDITFIELD\r"));
				if (HIWORD(lParam) == EN_CHANGE)
					CheckRadioButton (hwnd, RESMIN, RESMAX, RESCUSTOM);
				break;

			case IDOK:
				DBMSG(((LPSTR)"fnDialog():  IDOK\r"));
				devmode.iPaper = devmode.rgiPaper[devmode.iFeed];

				/* Get the new resolution */
				iRes = GetDlgItemInt
					  (
					  hwnd,
					  EDITFIELD,
					  (BOOL FAR *) &fValOk,
					  FALSE
					  );
				if (!fValOk)
				    break;

				/* Get the number of copies */
				iCopies = GetDlgItemInt
					  (
					  hwnd,
					  COPIES,
					  (BOOL FAR *) &fValOk,
					  FALSE
					  );
				if (!fValOk)
				    break;

				/* Check the resolution range */
				if (iRes<50)
				    iRes = 50;
 				else if (iRes>IRESMAX)
	                iRes = IRESMAX;
				devmode.iRes = iRes;

				/* Check the number of copies range */
				if (iCopies<=0)
				    iCopies = 1;
				else if (iCopies>200)
				    iCopies = 200;
				devmode.iCopies = iCopies;

				devmode.iPrinter = (int)SendDlgItemMessage(hwnd, LISTFIELD,
					LB_GETCURSEL, (WORD)0, 0L);


			case IDCANCEL:
				DBMSG(((LPSTR)"fnDialog():  IDCANCEL\r"));
			    EndDialog(hwnd, wParam);
			    break;
			default:
				DBMSG(((LPSTR)"<fnDialog() default FALSE\r"));
			    return(FALSE);
		}
	    break;
	default:
		DBMSG(((LPSTR)"<fnDialog() uMsg default FALSE\r"));
	    return(FALSE);

	}

END_DLG:
	DBMSG(((LPSTR)"<fnDialog() TRUE...end dialog\r"));
    return(TRUE);
}
