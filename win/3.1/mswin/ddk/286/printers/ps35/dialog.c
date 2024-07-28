/**[f******************************************************************
 * dialog.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/


/*********************************************************************
 * DIALOG.C
 *
 * 7Jan87	sec	fixed setting of gl_dloaded
 * 23Jan87	sec	added junk for new dialog
 * 9Mar87	sjp	added IBM stuff
 * 7Apr87	sjp	added DataProducts LZR 2665 (1st cut)
 * 8Apr87	sjp	added DEC LN03R ScriptPrinter
 * 14Apr87	sjp	included printers.h--and printer caps stuff
 * 17Apr87	sjp	added greater paper and tray functionality.
 * 3Jun87	sjp	Modified WriteProfile() and ReadProfile() to save
 *		   	the orientation state in the win.ini, also some
 *		   	error checking in ReadProfile().
 * 3Jun87	sjp	Added MakeEnvironment() and SaveEnvironment().
 * 14Aug87	sjp	Moved MapProfile(), GetPaperType() and ReadProfile()
 *		   	to new segment PROFILE.
 * 12/28/88 chrisg
 *	removed the MakeProcInstance calls for all dialog box functions.
 *	We are DLL and do not need thunks to set our DS.
 *
 *********************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "driver.h"
#include "pserrors.h"
#include "psoption.h"
#include "utils.h"
#include "debug.h"
#include "psdata.h"
#include "atprocs.h"
#include "atstuff.h"
#include "profile.h"
#include "resource.h"
#include "dmrc.h"
#include "getdata.h"
#include "pshelp.h"


#define MAXLISTBOXWIDTH 40
#define SOURCELISTBOXWIDTH		26
#define SIZELISTBOXWIDTH		26
#define SOURCELISTBOXWIDTHINATOMS	104
#define SIZELISTBOXWIDTHINATOMS		84

/*--------------------------- global data ------------------------------*/

extern PSDEVMODE CurEnv;

extern WORD wHelpMessage;
extern FARPROC	lpfnNextHook;

#ifdef APPLE_TALK
HWND ghwndDlg = NULL;
#endif

int	FAR PASCAL AddPrinter(HWND hwnd);

/*-------------------------- local functions --------------------------*/

BOOL FAR PASCAL fnDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnAbout(HWND hwnd, unsigned uMsg, WORD wParam, LONG lParam);
short	PASCAL MapListBoxPositiontoIDS(LPBOOL, short, short, short);
short	PASCAL MapIDStoListBoxPosition(LPBOOL, short, short, short);
short	PASCAL HiLiteListBoxItem(HWND, short, LPBOOL, short, short, short, short);
short	PASCAL MapIDStoListBoxPosition(LPBOOL, short, short, short);
BOOL PASCAL EnableTrays(HWND hwnd, PPRINTER pPrinter, LPPSDEVMODE lpdm);
void 	NEAR PASCAL SetOrient(HWND hwnd, WORD orient);
void	FAR PASCAL InitDefaults(HWND hwnd, PPRINTER pPrinter);

BOOL FAR PASCAL PaperSizeDlgProc(HWND hDlg, unsigned dMsg, WORD wParam, LONG lParam);

/*-------------------------- local data --------------------------*/

// use gszHelpFile[] set up in LibMain() function.
//char szHelpFile[] = "pscript.hlp";


/***********************************************************************/


short PASCAL MapListBoxPositiontoIDS(list, theLBPosition, minIDS, maxIDS)
LPBOOL	list;		/* list of Boolean capabilities */
short	theLBPosition;	/* the IDS value in question */
short	minIDS;
short	maxIDS;
{
	short	i;
	short	LBPosition = 0;
	short	theEntry = minIDS;

	/* search through all the IDS values for this listbox */
	for (i = minIDS; i <= maxIDS; i++) {

		/* if this entry is supported by the given printer... */
		if (list[i-minIDS]) {

			/* is the listbox position thus far the same as
			 * that requested?
			 */
			if (LBPosition == theLBPosition) {
				theEntry = i;
				break;
			}
			/* otherwise wait */
			LBPosition++;
		}
	}
	DBMSG(("*MapListBoxPositiontoIDS(): LBP=%d,tE=%d,min=%d,max=%d\n",
	    LBPosition, theEntry, minIDS, maxIDS));
	return theEntry;
}


/***********************************************************************/

short	PASCAL MapIDStoListBoxPosition(list, theEntry, minIDS, maxIDS)
LPBOOL	list;		/* list of Boolean capabilities */
short	theEntry;	/* the IDS value in question */
short	minIDS;
short	maxIDS;
{
	short	i;
	short	LBPosition = 0;

	for (i = minIDS; i <= maxIDS; i++) {
		/* if this is entry then exit */
		if (i == theEntry) 
			break;

		/* if the entry is supported then increment the
		 * position value
		 */
		if (list[i-minIDS]) 
			LBPosition++;
	}
	DBMSG(("*MapIDStoListBoxPosition(): tE=%d,LBP=%d,min=%d,max=%d\n",
	    theEntry, LBPosition, minIDS, maxIDS));
	return LBPosition;
}


/***********************************************************************
 *
 * note:
 *	theEntry and defEntry are in the range minIDS - maxIDS.  thus
 *	sub minIDS from these to get zero based offsets.
 *
 ***********************************************************************/

short PASCAL HiLiteListBoxItem(hWnd, listBox, list, theEntry, defEntry, minIDS, maxIDS)
HWND	hWnd;
short	listBox;
LPBOOL	list;
short	theEntry;
short	defEntry;
short	minIDS;
short	maxIDS;
{
	short	i;
	short	rc = -1; /* -1	-->	the feed is not valid nor could it be
			  *		made valid
			  * >=0	-->	the list box entry high lighted
			  */

	DBMSG((">HiLiteListBoxItem(): iP=%d,ID=%d, entry=%d,default=%d,min=%d,max=%d\n",
	    CurEnv.iPrinter, listBox, theEntry, defEntry, minIDS, maxIDS));

	/* If the request entry (theEntry) to highlight is not supported by
	 * the printer then first check to see if the the value passed as the
	 * default is valid, if not then search for the first entry in the
	 * entire list that is supported. */

	DBMSG((" HiLiteListBoxItem(): list[entry]%d list[default]=%d\n",
	    list[theEntry-minIDS], list[defEntry-minIDS]));

	if (!list[theEntry-minIDS]) {
		
		DBMSG((" invalid request: "));

		/* use the default*/
		if (list[defEntry - minIDS]) {
			DBMSG(("using default\n"));
			theEntry = defEntry;

			/* try to find one that will work */
		} else {
			DBMSG((" search for alternate\n"));
			DBMSG((" HiLiteListBoxItem(): WIN.INI error\n"));
			for (i = minIDS; i <= maxIDS; i++) {

				DBMSG(("[%d]%d ", i, list[i-minIDS]));
				if (list[i-minIDS]) {
					DBMSG(("*[%d]%d* ", i, list[i-minIDS]));
					theEntry = i;
					break;
				}
				DBMSG_LB(("\n"));
			}
		}
	}

	/* If the entry is supported or the entry is available in the
	 * printer then highlight it.  In case of a null feed list we won't
	 * highlight anything.
	 */
	if (theEntry >= minIDS && theEntry <= maxIDS) {

		SendDlgItemMessage(hWnd, listBox, CB_SETCURSEL,
		    MapIDStoListBoxPosition(list, theEntry, minIDS, maxIDS),
		    0L);
		rc = theEntry;
	}

	DBMSG(("<HiLiteListBoxItem(): rc=%d\n", rc));

	return rc;
}


/*
 *
 * iPaperType	the DMPAPER_* paper type (size)
 *
 */

int NEAR PASCAL HiLiteSize(HWND hWnd, PPRINTER pPrinter, LPPSDEVMODE lpdm, int iPaperType)
{
	int	i;
        PAPER_REC   paper;

	/* If the request entry (theEntry) to highlight is not supported by
	 * the printer then first check to see if the the value passed as the
	 * default is valid, if not then search for the first entry in the
	 * entire list that is supported. */

	if (!PaperSupported(pPrinter, lpdm, iPaperType)) {

		if (!PaperSupported(pPrinter, lpdm, iPaperType = GetDefaultPaper())) {
			iPaperType = GetPaperEntry(pPrinter, lpdm, 0);
		}
	}

	/* If the entry is supported or the entry is available in the
	 * printer then highlight it.  In case of a null feed list we won't
	 * highlight anything.
	 */
        i = FindPaperType(pPrinter, lpdm, iPaperType, &paper);

	SendDlgItemMessage(hWnd, SIZELIST, CB_SETCURSEL, i, 0L);

	return iPaperType;
}






/*
 * BOOL PASCAL EnableTrays(hwnd, pPrinter)
 *
 * return TRUE	for success
 * return FALSE for failure (error condition)
 */

BOOL PASCAL EnableTrays(HWND hwnd, PPRINTER pPrinter, LPPSDEVMODE lpdm)
{
	char entryName[MAXLISTBOXWIDTH];
	BOOL rc;
	int i, iPaper;
	int source;

	source = CurEnv.dm.dmDefaultSource;


	DBMSG((">EnableTrays(): iP=%d,iJT=%d,iR=%d\n",
	    CurEnv.iPrinter, CurEnv.iJobTimeout, CurEnv.iRes));

	SendDlgItemMessage(hwnd, SIZELIST, CB_RESETCONTENT, 0, 0L);
	SendDlgItemMessage(hwnd, SOURCELIST, CB_RESETCONTENT, 0, 0L);

        i = 0;
        while ((iPaper = GetPaperEntry(pPrinter, lpdm, i++)) >= 0) {
		LoadString(ghInst, iPaper + DMPAPER_BASE, entryName, sizeof(entryName));
		SendDlgItemMessage(hwnd, SIZELIST, CB_INSERTSTRING, (WORD)-1,
			(DWORD)(LPSTR)entryName);
        }

	for (i = DMBIN_FIRST + DMBIN_BASE; i <= DMBIN_LAST + DMBIN_BASE; i++) {

		if (pPrinter->feed[i-(DMBIN_FIRST + DMBIN_BASE)]) {

			LoadString(ghInst, i, entryName, sizeof(entryName));
			SendDlgItemMessage(hwnd, SOURCELIST, CB_INSERTSTRING, (WORD)-1,
			    (DWORD)(LPSTR)entryName);
		}
	}

	/* If the selected printer does not support the previously 
	 * selected paper source, then default to the first feed source
	 * available to that printer. */

	/* Note: DMBIN_BASE is the resource number offset!
	 * dmDefaultSource is in the range DMBIN_FIRST - DMBIN_LAST */

	// make sure source is in range to prevent GP faults

	if ((source < DMBIN_FIRST) || (source > DMBIN_LAST))
		source = DMBIN_FIRST;
	 
	rc = HiLiteListBoxItem(hwnd, SOURCELIST, pPrinter->feed,
	  		source + DMBIN_BASE, 
			pPrinter->defFeed + DMBIN_BASE,
			DMBIN_FIRST + DMBIN_BASE, DMBIN_LAST + DMBIN_BASE);
			
	if (rc >= 0) {

		/* update the feed source */
		CurEnv.dm.dmDefaultSource = rc - DMBIN_BASE;

		DBMSG((" EnableTrays(): source:%d rgiPaper[source]:%d\n",
		    source, CurEnv.rgiPaper[source]));

		rc = HiLiteSize(hwnd, pPrinter, &CurEnv, CurEnv.dm.dmPaperSize);
		CurEnv.rgiPaper[source] = rc;

		DBMSG((" After HiLite: source:%d rgiPaper[source]:%d\n",
			    source, CurEnv.rgiPaper[source]));
	}

	DBMSG(("<EnableTrays(): iP=%d,iJT=%d,iR=%d\n",
	    CurEnv.iPrinter, CurEnv.iJobTimeout, CurEnv.iRes));

	return TRUE;
}


void NEAR PASCAL SetOrient(HWND hwnd, WORD orient)
{
	HANDLE	hIcon;
	WORD	word,  wVers;

	if (orient == DMORIENT_LANDSCAPE) {
		word = LANDSCAPE;
		hIcon = LoadIcon(ghInst, "L");

	} else {
		word = PORTRAIT;
		hIcon = LoadIcon(ghInst, "P");
	}

	CheckRadioButton(hwnd, PORTRAIT, LANDSCAPE, word);

    wVers = GetVersion();

    if (LOBYTE(wVers) > 0x03 || (LOBYTE(wVers) == 0x03 && HIBYTE(wVers) > 0x00))
        SendDlgItemMessage(hwnd, IDICON, STM_SETICON, (WORD)hIcon, 0L);
    else
        SetDlgItemText(hwnd, IDICON, MAKEINTRESOURCE(hIcon));
}


void FAR PASCAL InitDefaults(HWND hwnd, PPRINTER pPrinter)
{
	CurEnv.iRes = pPrinter->defRes;
	CurEnv.dm.dmDefaultSource = pPrinter->defFeed;
	CurEnv.iJobTimeout = pPrinter->defJobTimeout;
	CurEnv.dm.dmColor = pPrinter->fColor ? DMCOLOR_COLOR : DMCOLOR_MONOCHROME;
	CurEnv.dm.dmOrientation = DMORIENT_PORTRAIT;

	SetOrient(hwnd, CurEnv.dm.dmOrientation);

	CheckDlgButton(hwnd, USE_COLOR, pPrinter->fColor);

	EnableWindow(GetDlgItem(hwnd, USE_COLOR), pPrinter->fColor);

	EnableTrays(hwnd, pPrinter, &CurEnv);
}

			


/******************************************************************
* Name: fnDialog()
*
* Action: This is the dialog function put up by DeviceMode()
*
*
*
********************************************************************/

BOOL FAR PASCAL fnDialog(hwnd, uMsg, wParam, lParam)
HWND		hwnd;
unsigned	uMsg;
WORD		wParam;
LONG		lParam;
{
	short	rc;
	int	i;
	BOOL	fValOk;
	int	iCopies;
        int     cbBuf;
	char	buf[64];	/* to hold the dialog caption and stuff */
	static BOOL fHelp;
	PPRINTER pPrinter;
        HCURSOR hOldCursor = NULL;

	switch (uMsg) {

	case WM_INITDIALOG:

                hOldCursor = SetCursor(LoadCursor(ghInst, IDC_WAIT));

		fHelp = FALSE;
		*gFileName = 0;

#ifdef APPLE_TALK
		*buf = 0;
                ghwndDlg = hwnd;

		LoadString(ghInst, IDS_APPLETALK, buf, sizeof(buf));

		/* do a case insensitive compare of the port names */
		if (lstrcmpi(AnsiUpper(gPort), AnsiUpper(buf))) {
			DBMSG((" fnDialog(): NOT AppleTalk\n"));
			ShowWindow(GetDlgItem(hwnd, IDAPLTALK), SW_HIDE);
		}
#else
		ShowWindow(GetDlgItem(hwnd, IDAPLTALK), SW_HIDE);
#endif

		SendDlgItemMessage(hwnd, COPIES,     EM_LIMITTEXT, 4, 0L);

		SetDlgItemInt(hwnd, COPIES, CurEnv.dm.dmCopies, FALSE);

		SetOrient(hwnd, CurEnv.dm.dmOrientation);

		if (!(pPrinter = GetPrinter(CurEnv.iPrinter)))
			goto error;

		// Build the Dialog caption
#if 0
		lstrcpy(buf, pPrinter->Name);
#else
		// display the custom printer name (10/21/91 ZhanW)
		lstrcpy(buf, CurEnv.dm.dmDeviceName);
#endif
                cbBuf = lstrlen(buf);
                if (!LoadString(ghInst, IDS_ON, buf + cbBuf, sizeof(buf) - cbBuf))
                    goto error;
		lstrncat(buf, gPort, sizeof(buf));
		SetWindowText(hwnd, buf);

		/* Since printer list is not dynamic there is no need to
		 * use MapIDStoListBoxPosition(). */

		DBMSG((" INITDIALOG: iPrinter %d\n", CurEnv.iPrinter));

		EnableTrays(hwnd, pPrinter, &CurEnv);

		SetFocus(GetDlgItem(hwnd, CurEnv.dm.dmOrientation == DMORIENT_LANDSCAPE ? LANDSCAPE : PORTRAIT));

		FreePrinter(pPrinter);

                SetCursor(hOldCursor);

		DBMSG(("<fnDialog() FALSE\n"));
		return FALSE;			/* we have set the focus */
		break;

	case WM_COMMAND:

		DBMSG((" fnDialog(): WM_COMMAND\n"));

		switch (wParam) {

		case IDABOUT:
			DialogBox(ghInst, "AB", hwnd, fnAbout);
			break;

		case PORTRAIT:
		case LANDSCAPE:

			DBMSG((" fnDialog():  orientation\n"));

			if (wParam == LANDSCAPE)
				CurEnv.dm.dmOrientation = DMORIENT_LANDSCAPE;
			else
				CurEnv.dm.dmOrientation = DMORIENT_PORTRAIT;

			SetOrient(hwnd, CurEnv.dm.dmOrientation);
			break;


		case SIZELIST:

			DBMSG((" fnDialog():  SOURCELIST\n"));

			if (HIWORD(lParam) == CBN_SELCHANGE) {

				if (!(pPrinter = GetPrinter(CurEnv.iPrinter)))
					goto error;

				i = (int)SendDlgItemMessage(hwnd, SIZELIST, 
				    CB_GETCURSEL, 0, 0L);

				CurEnv.rgiPaper[CurEnv.dm.dmDefaultSource] = 
                                    GetPaperEntry(pPrinter, &CurEnv, i);

            if (CurEnv.rgiPaper[CurEnv.dm.dmDefaultSource] == DMPAPER_USER) {
			         DialogBox(ghInst, "DLG_PAPERSIZE", hwnd, PaperSizeDlgProc);
            }

				FreePrinter(pPrinter);
			}
			break;

		case SOURCELIST:

			DBMSG((" fnDialog():  SOURCELIST\n"));

			if (HIWORD(lParam) == CBN_SELCHANGE) {

				if (!(pPrinter = GetPrinter(CurEnv.iPrinter)))
					break;

				i = (int)SendDlgItemMessage(hwnd, SOURCELIST, CB_GETCURSEL, 0, 0L);


				CurEnv.dm.dmDefaultSource = MapListBoxPositiontoIDS(pPrinter->feed, i,
				    DMBIN_FIRST, DMBIN_LAST);

				DBMSG((" fnDialog(): SOURCELIST iF=%d,rgiP[]=%d\n",
				    CurEnv.dm.dmDefaultSource, CurEnv.rgiPaper[CurEnv.dm.dmDefaultSource]));

				rc = HiLiteSize(hwnd, pPrinter, &CurEnv, GetDefaultPaper());

				CurEnv.rgiPaper[CurEnv.dm.dmDefaultSource] = rc;

				FreePrinter(pPrinter);

				DBMSG(("Change made.\n"));
			}
			break;

		case IDOK:

			DBMSG((" fnDialog():>IDOK\n"));

			CurEnv.dm.dmPaperSize = CurEnv.rgiPaper[CurEnv.dm.dmDefaultSource];

			DBMSG(("dmPaperSize:%d\n", CurEnv.dm.dmPaperSize));

			/* Get the number of copies */

			iCopies = GetDlgItemInt(hwnd, COPIES, &fValOk, FALSE);
			if (!fValOk) {
				PSError(PS_COPIES0);
				break;
			}
			/* Check the number of copies range */
			if (iCopies <= 0) 
				iCopies = 1;
#if 0
			else if (iCopies > 200) 
				iCopies = 200;
#endif
			CurEnv.dm.dmCopies = iCopies;

			CurEnv.iPrinter = MatchPrinter(CurEnv.dm.dmDeviceName);

			DBMSG(("printer # %d\n", CurEnv.iPrinter));

         #ifdef APPLE_TALK
         ghwndDlg = NULL;
         #endif

			DBMSG((" fnDialog():<IDOK\n"));

			/* fallthrough... */

		case IDCANCEL:
			DBMSG((" fnDialog(): IDCANCEL\n"));
			if (fHelp)
				WinHelp(hwnd, gszHelpFile, HELP_QUIT, 0L);

			EndDialog(hwnd, wParam);
			break;

		case IDOPTIONS:
			DialogBox(ghInst, "OP", hwnd, fnOptionsDialog);
			break;
#ifdef APPLE_TALK
		case IDAPLTALK:
			DBMSG((" fnDialog():  IDAPLTALK\n"));
			if (LoadAT()) {
				ATChooser(hwnd);
				UnloadAT();
			}
			break;
#endif
		case IDHELP:
			//fHelp = WinHelp(hwnd, gszHelpFile, HELP_INDEX, 0L);
			fHelp = WinHelp(hwnd, gszHelpFile, HELP_CONTEXT,
					IDH_DLG_PRINTERSET);
			break;


		default:
			DBMSG(("<fnDialog() default FALSE\n"));
			return FALSE;
		}
		break;

	default:
	    if (uMsg == wHelpMessage)
		{
		fHelp = WinHelp(hwnd, gszHelpFile, HELP_CONTEXT, IDH_DLG_PRINTERSET);
		return(TRUE);
		}
	    else
		return FALSE;
	}

	DBMSG(("<fnDialog() TRUE...end dialog\n"));
	return TRUE;

error:
	DBMSG(("<fnDialog() FALSE, ERROR\n"));
	EndDialog(hwnd, IDCANCEL);

        if (hOldCursor)
                SetCursor(hOldCursor);

	return FALSE;
}

BOOL FAR PASCAL fnAbout(HWND hwnd, unsigned uMsg, WORD wParam, LONG lParam)
{
	switch (uMsg) {

	case WM_INITDIALOG:

		SetFocus(GetDlgItem(hwnd, IDOK));

		return FALSE;	/* we set the focus */

	case WM_COMMAND:
		if (wParam == IDOK)
			EndDialog(hwnd, wParam);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

//------------------------*MessageFilter*------------------------------------
// Action: filter messages sent to the device mode dialog boxes so we
//  can catch the F1 key-down message and display the context-sensitive
//  help panel.
//  Adopted from UniDrv. (9/23/91 ZhanW)
//---------------------------------------------------------------------------

int FAR PASCAL MessageFilter(nCode, wParam, lpMsg)
int   nCode;
WORD  wParam;
MSG FAR * lpMsg;
{
   if (nCode < 0)                         // MUST return DefHookProc()
      return DefHookProc(nCode, wParam, (DWORD)lpMsg, (FARPROC FAR *)&lpfnNextHook);

   if (nCode == MSGF_DIALOGBOX)
      {
      if ((lpMsg->message == WM_KEYDOWN) && (lpMsg->wParam == VK_F1))
         {
	 PostMessage(GetParent(lpMsg->hwnd), wHelpMessage, lpMsg->hwnd, 0L);
         return 1L;                       // Handled it
         }
      else
         return 0L;                       // Did not handle it
      }
   else                                   // I.e., MSGF_MENU
      return 0L;                       // Did not handle it
}
