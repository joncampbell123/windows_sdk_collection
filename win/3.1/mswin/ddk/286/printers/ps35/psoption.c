/**[f******************************************************************
 * psoption.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/*********************************************************************
 * PSOPTIONS.C
 *
 * 12/28/88 chrisg
 *	removed the MakeProcInstance calls for all dialog box functions.
 *	Since we are a driver (DLL) and have only one instance
 *	(data segment) we do not need to call through thunks to have
 *	our DS set to an instance data segment.  since we are a library
 *	our DS gets set for us in the function prolog code.
 *
 * 3/25/91 msd
 *	renamed Reserved to iDeviceRes & added support for duplex.
 *********************************************************************/

#include "pscript.h"
#include <winexp.h>
#include "psoption.h"
#include "pserrors.h"
#include "psprompt.h"
#include "atprocs.h"
#include "psdata.h"
#include "dmrc.h"
#include "atstuff.h"
#include "channel.h"
#include "utils.h"
#include "resource.h"
#include "getdata.h"
#include "debug.h"
#include "etm.h"
#include "fonts.h"
#include "enum.h"
#include "profile.h"
#include "pshelp.h"

#define IDC_ARROW	MAKEINTRESOURCE(32512)
#define IDC_IBEAM	MAKEINTRESOURCE(32513)
#define IDC_WAIT	MAKEINTRESOURCE(32514)

#define DMUNIT_TENTHMM		0   // 0.1 mm
#define DMUNIT_HUNDREDTHINCH	1   // 0.01 inch

#define MetricToBritish(x) (MulDiv(x, 100, 254))
#define BritishToMetric(x) (MulDiv(x, 254, 100))

#define SUBID_MISSINGFONT   1

extern BOOL bTTEnabled;
extern BOOL bInAppSetup;
extern PSDEVMODE CurEnv;
extern HANDLE ghInst;
extern WORD wHelpMessage;

/*--------------------------------- local data ----------------------------*/

char gFileName[64];	/* holds name of file returned from fnFileDialog() */

/*----------------------------- local functions ---------------------------*/

short	DumpResourceToFile(LPSTR,LPSTR,short,short,short,...);
BOOL	MakeTempLPDV(LPDV, LPSTR);

/* dialog functions.  all of these need to be exported in the .DEF file
 * we don't need thunks for these because we are a DLL */

BOOL FAR PASCAL fnHeaderDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnHeaderDLDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnHandshakeDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnAdvancedDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnErrorDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnFileDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnSubstituteDialog(HWND, unsigned, WORD, LONG);

void NEAR PASCAL InitItemData(LPDV, short, HWND, HWND);
BOOL NEAR PASCAL WriteItemData(HWND hwndTT, HWND hwndDV);
void NEAR PASCAL InitAppSetup(HWND hwnd);
void NEAR PASCAL InitDuplex(HWND hwnd, PPRINTER pPrinter);
void NEAR PASCAL SetDuplex(HWND hwnd, WORD dmDuplex, WORD dmOrient);
BOOL NEAR PASCAL GetCommParameters(int FAR *piBaud, int FAR *piOptions);
LPSTR NEAR PASCAL NextField(LPSTR lpsz, char ch);
int FAR PASCAL SubFontProc(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                           DWORD dwData);
    //  from  profile.c

short NEAR PASCAL GetOneUserPaperSize(HWND hDlg, int nDlgItemID, 
                                      short sMin, short sMax,
				                          unsigned nOutOfRangeMsgID);

BOOL MakeTempLPDV(tempLPDV,lpFileName)
LPDV tempLPDV;
LPSTR lpFileName;
{
	DBMSG((">MakeTempLPDV: file=%ls\n", (LPSTR)lpFileName));

	lstrcpy(tempLPDV->szFile,lpFileName);
	tempLPDV->fContext=FALSE;	/* is not an info context */

	DBMSG((" MakeTempLPDV: Before GetDC()\n"));

	if (!(tempLPDV->hdc = GetDC((HWND)0))) {	/* needed for OpenChannel */
		DBMSG(("<MakeTempLPDV: ERROR\n"));
		return FALSE;
	}

	DBMSG(("<MakeTempLPDV:\n"));
	return TRUE;
}


LPDV FAR PASCAL StartTmpJob(LPSTR lpFileName, LPSTR lpTitle, PPRINTER pPrinter)
{
   HANDLE hDV;
   LPDV lpdv;
	char buf[80];
#ifdef APPLETALK
	char userName[40];	/* used with AppleTalk */
#endif

	if (!(hDV = GlobalAlloc(GDLLPTR, (long)sizeof(DV))) ||
	    !(lpdv = (LPDV)GlobalLock(hDV))) {
		goto ERROR1;
	}

	if (!MakeTempLPDV(lpdv,lpFileName)){
		goto ERROR1;
	}

#ifdef APPLE_TALK
	if (TryAT()) goto ERROR2;
#endif

	if (OpenChannel(lpdv,lpTitle, 0) < 0) {
		goto ERROR2;
	}

   lpdv->fDirty = TRUE;

#ifdef APPLE_TALK
	if (pPrinter->fEOF && !lpdv->fDoEps && !ATState())
#else
	if (pPrinter->fEOF && !lpdv->fDoEps)
#endif
		WriteChannelChar(lpdv, EOF);

	/* Print minimum Adobe Header Comments */
	LoadString(ghInst, IDS_PSHEAD, buf, sizeof(buf));
	PrintChannel(lpdv, buf);
	LoadString(ghInst, IDS_PSTITLE, buf, sizeof(buf));
	PrintChannel(lpdv, buf, (LPSTR)lpTitle);
	LoadString(ghInst, IDS_PSJOB, buf, sizeof(buf));
	PrintChannel(lpdv, buf, (LPSTR)lpTitle);

#ifdef APPLE_TALK
	if (ATState()){
		/* use "userName" since it is already available */
		LoadString(ghInst, IDS_PREPARE, userName, sizeof(userName));
		ATMessage(-1,0, (LPSTR)userName);
	}
#endif

   return lpdv;

ERROR2:
#ifdef APPLE_TALK
	KillAT();
#endif

	ReleaseDC((HWND)0, lpdv->hdc);

ERROR1:
	if (hDV)
		GlobalFree(hDV);

	return NULL;
}


void FAR PASCAL EndTempJob(LPDV lpdv, PPRINTER pPrinter)
{
#ifdef APPLE_TALK
	if (pPrinter->fEOF && !lpdv->fDoEps && !ATState())
#else
	if (pPrinter->fEOF && !lpdv->fDoEps)
#endif
		WriteChannelChar(lpdv, EOF);

	CloseChannel(lpdv);

#ifdef APPLE_TALK
	KillAT();
#endif

	ReleaseDC((HWND)0,lpdv->hdc);

	GlobalFree(LOWORD(GlobalHandle(HIWORD((DWORD)lpdv))));
}


void NEAR PASCAL InitAppSetup(HWND hwnd)
{
#if 0
    RECT rcHeader, rcHelp;
    POINT pt;

    /* remember coordinates of Header and help buttons */
    GetWindowRect(GetDlgItem(hwnd, OP_HEADER), &rcHeader);
    GetWindowRect(GetDlgItem(hwnd, IDHELP), &rcHelp);

    /* disable and hide areas that are accessable only to shell */
    ShowWindow(GetDlgItem(hwnd, OP_HEADER), SW_HIDE);
    EnableWindow(GetDlgItem(hwnd, OP_HEADER), FALSE);
    ShowWindow(GetDlgItem(hwnd, OP_ADVANCED), SW_HIDE);
    EnableWindow(GetDlgItem(hwnd, OP_ADVANCED), FALSE);

    /* move the Help button below the Cancel button */
    pt.x = rcHeader.left;
    pt.y = rcHeader.top;
    ScreenToClient(hwnd, &pt);
    MoveWindow(GetDlgItem(hwnd, IDHELP), pt.x, pt.y,
               rcHelp.right - rcHelp.left, rcHelp.bottom - rcHelp.top,
               FALSE);
#endif
}

/*
 * short DumpResourceToFile(lpFileName,lpTitle,resType,resID,numExtra,
 *			 rT1,rID1,rT2,rID2)
 * dump stuff from resource to lpFileName
 *
 * this is primariarly used to dump the header to the printer and to
 * a file.
 *
 * the variable number of params let you dump more than one resource
 * at a time.
 *
 */

short DumpResourceToFile(lpFileName,lpTitle,resType,resID,numExtra,
			 rT1,rID1,rT2,rID2)
LPSTR lpFileName;	/* file name to dump to (LPT1:, file.prn) */
LPSTR lpTitle;
short resType;		/* resource type */
short resID;		/* resource id */
short numExtra;		/* number of extra resources to dump */
short rT1;		/* first extra resource type */
short rID1;		/* first extra resource ID */
short rT2;		/* second... */
short rID2;
{
	PPRINTER pPrinter = NULL;
	LPDV lpdv;		/* temp DV for doing channel output */
	short rc=0;

	DBMSG((">DumpResourceToFile(): lpFile=%ls,lpTitle=%ls,Type=%d,ID=%d\n",
		lpFileName,lpTitle,resType,resID));

	/* go to hourglass mode */
	SetCursor(LoadCursor(NULL,IDC_WAIT));

	if (!(pPrinter = GetPrinter(CurEnv.iPrinter))) {
		rc=3;
		goto CLEANUP1;
	}

   if (!(lpdv = StartTmpJob(lpFileName, lpTitle, pPrinter)))
      goto CLEANUP1;

	if (!DumpResourceString(lpdv,resType,resID)){
		rc=3;
		goto CLEANUP2;
	}

	if (numExtra > 0) {
		if (!DumpResourceString(lpdv,rT1,rID1)) {
			rc=3;
			goto CLEANUP2;
		}
	}
	if (numExtra > 1) {
		if (!DumpResourceString(lpdv,rT2,rID2)) {
			rc=3;
			goto CLEANUP2;
		}
	}

CLEANUP2:
   EndTempJob(lpdv, pPrinter);

CLEANUP1:
	/* go to arrow mode */
	SetCursor(LoadCursor(NULL,IDC_ARROW));

   if (pPrinter)
      FreePrinter(pPrinter);

	return rc;
}



/******************************************************************
* Name: fnOptionsDialog()
*
* Action: This is a callback function that handles the dialog
*	  messages sent from Windows.  The dialog is initiated
*	  from the OptionsDeviceMode function.
*
*********************************************************************/

BOOL FAR PASCAL fnOptionsDialog(hwnd, uMsg, wParam, lParam)
	HWND hwnd;
	unsigned uMsg;
	WORD wParam;
	LONG lParam;
{
	BOOL fValOk;
	PPRINTER pPrinter;

	switch (uMsg) {
	case WM_INITDIALOG:
		DBMSG(((LPSTR)" fnOptionsDialog(): WM_INITDIALOG\n"));


		if (!(pPrinter = GetPrinter(CurEnv.iPrinter)))
			goto ERROR;

	    	CheckDlgButton(hwnd, USE_COLOR, CurEnv.dm.dmColor == DMCOLOR_COLOR);
		EnableWindow(GetDlgItem(hwnd, USE_COLOR), pPrinter->fColor);

		SendMessage(GetDlgItem(hwnd, HEADER_YES), BM_SETCHECK,
			CurEnv.fHeader, 0L);

		InitDuplex(hwnd, pPrinter);

		FreePrinter(pPrinter);

		CheckRadioButton(hwnd, TO_PRINTER, TO_EPS,
			CurEnv.fDoEps ? TO_EPS: TO_PRINTER);

		SetDlgItemText(hwnd, FILE_EDIT, CurEnv.EpsFile);
		EnableWindow(GetDlgItem(hwnd, FILE_TEXT), CurEnv.fDoEps);
		EnableWindow(GetDlgItem(hwnd, FILE_EDIT), CurEnv.fDoEps);
		SendDlgItemMessage(hwnd, FILE_EDIT, EM_LIMITTEXT, sizeof(CurEnv.EpsFile)-1, 0L);

		SendDlgItemMessage(hwnd, ID_SCALE,   EM_LIMITTEXT, 4, 0L);
		SetDlgItemInt(hwnd, ID_SCALE,   CurEnv.dm.dmScale, FALSE);

		CheckRadioButton(hwnd, DEFAULT_MARGINS,
#if 0
				TILE_MARGINS,
#else
				ZERO_MARGINS,
#endif
			CurEnv.marginState);

                /* remove "shell only" buttons if called from app */
                if (bInAppSetup)
                    InitAppSetup(hwnd);

		return FALSE;
		break;

	case WM_COMMAND:
		DBMSG((" fnOptionsDialog(): WM_COMMAND\n"));
		switch(wParam) {

		case DEFAULT_MARGINS:
		case ZERO_MARGINS:
		/* case TILE_MARGINS: */
			DBMSG((" fnOptionsDialog():>margins\n"));

			CheckRadioButton(hwnd, DEFAULT_MARGINS, 
#if 0
				TILE_MARGINS,
#else
				ZERO_MARGINS,
#endif
				wParam);
			/* update the margin state variable */
			CurEnv.marginState=wParam;
			break;

		case TO_PRINTER:
		case TO_EPS:
			CheckRadioButton(hwnd, TO_PRINTER, TO_EPS, wParam);

			CurEnv.fDoEps = (wParam == TO_EPS);

			EnableWindow(GetDlgItem(hwnd, FILE_TEXT), CurEnv.fDoEps);
			EnableWindow(GetDlgItem(hwnd, FILE_EDIT), CurEnv.fDoEps);

			if (CurEnv.fDoEps)
				SetFocus(GetDlgItem(hwnd, FILE_EDIT));
			break;

		case DUPLEX_NONE:
		case DUPLEX_LONGEDGE:
		case DUPLEX_SHORTEDGE:
			switch (wParam) {
				/* See note on duplex terminology in SetDuplex.
				*/
				case DUPLEX_NONE:
					CurEnv.dm.dmDuplex = DMDUP_SIMPLEX;
					break;
				case DUPLEX_LONGEDGE:
					CurEnv.dm.dmDuplex = DMDUP_VERTICAL;
					break;
				case DUPLEX_SHORTEDGE:
					CurEnv.dm.dmDuplex = DMDUP_HORIZONTAL;
					break;
			}
			SetDuplex(hwnd, CurEnv.dm.dmDuplex, CurEnv.dm.dmOrientation);
			break;

		case IDOK:
			DBMSG((" fnOptionsDialog():>IDOK\n"));

			if (CurEnv.fDoEps) {
				GetDlgItemText(hwnd, FILE_EDIT, CurEnv.EpsFile, sizeof(CurEnv.EpsFile)-1);
			}

			CurEnv.dm.dmScale = GetDlgItemInt(hwnd, ID_SCALE, &fValOk, TRUE);
			if (!fValOk) {
				PSError(PS_SCALING0);
				break;
			}

			if (CurEnv.dm.dmScale < 10)
				CurEnv.dm.dmScale = 10;	/* min scale value */
			else if (CurEnv.dm.dmScale > 400)
				CurEnv.dm.dmScale = 400;	/* and max */

			CurEnv.fHeader = IsDlgButtonChecked(hwnd, HEADER_YES);
			if (IsDlgButtonChecked(hwnd, USE_COLOR))
			    CurEnv.dm.dmColor = DMCOLOR_COLOR;
			else
			    CurEnv.dm.dmColor = DMCOLOR_MONOCHROME;

			DBMSG((" fnOptionsDialog():<IDOK\n"));

			/* fall through... */

		case IDCANCEL:
			DBMSG((" fnOptionsDialog():  IDCANCEL\n"));
			WinHelp(hwnd, gszHelpFile, HELP_QUIT, 0L);
			EndDialog(hwnd, wParam);
			break;

		case OP_HEADER:
		        DialogBox(ghInst, "OPHD", hwnd, fnHeaderDLDialog);
		//	DialogBox(ghInst, "OPH", hwnd, fnHeaderDialog);
			break;

                case OP_ADVANCED:
			DialogBox(ghInst, "AV", hwnd, fnAdvancedDialog);
			break;

		case IDHELP:
		     WinHelp(hwnd, gszHelpFile, HELP_CONTEXT,
			(IsWindowVisible(GetDlgItem(hwnd, DUPLEX_WNDFRAME)) ?
		       IDH_DLG_MORE_DUPLEX: IDH_DLG_MORE));
		     break;

		default:
			return FALSE;
		}
		break;

	default:
	    if (uMsg == wHelpMessage)
		{
		WinHelp(hwnd, gszHelpFile, HELP_CONTEXT,
		   (IsWindowVisible(GetDlgItem(hwnd, DUPLEX_WNDFRAME)) ?
		       IDH_DLG_MORE_DUPLEX: IDH_DLG_MORE));
		return(TRUE);
		}
	    else
		return FALSE;

	}

	return TRUE;

ERROR:
    EndDialog(hwnd, FALSE);
    return FALSE;
}




BOOL FAR PASCAL fnHeaderDLDialog(hwnd, uMsg, wParam, lParam)
HWND	hwnd;
unsigned uMsg;
WORD	wParam;
LONG	lParam;
{
	PPRINTER pPrinter;
	short resID;
	short rc=0;

	switch (uMsg) {

	case WM_INITDIALOG:
		*gFileName = 0;
		CheckRadioButton(hwnd, OP_HEADER_PRINTER, OP_HEADER_FILE,
			OP_HEADER_PRINTER);
		EnableWindow(GetDlgItem(hwnd, OP_FILELABEL), FALSE);
		EnableWindow(GetDlgItem(hwnd, OP_FILE), FALSE);
                SetFocus(GetDlgItem(hwnd, OP_HEADER_PRINTER));
		SendDlgItemMessage(hwnd, OP_FILE, EM_LIMITTEXT,
		    sizeof(gFileName)-1, 0);
		return FALSE;

	case WM_COMMAND:
		switch (wParam) {

		case OP_HEADER_PRINTER:
		case OP_HEADER_FILE:
			CheckRadioButton(hwnd, OP_HEADER_PRINTER, OP_HEADER_FILE,
				wParam);
			if (wParam == OP_HEADER_FILE)
			    SetFocus(GetDlgItem(hwnd, OP_FILE));

			EnableWindow(GetDlgItem(hwnd, OP_FILELABEL), wParam == OP_HEADER_FILE);
			EnableWindow(GetDlgItem(hwnd, OP_FILE), wParam == OP_HEADER_FILE);
			break;

		case IDOK:
			if (pPrinter = GetPrinter(CurEnv.iPrinter)) {
				if (IS_TRUEIMAGE(pPrinter))
					resID = TI_HEADER;
				else
					resID = PS_HEADER;
				FreePrinter(pPrinter);
			} else
				resID = PS_HEADER;

			if (IsDlgButtonChecked(hwnd, OP_HEADER_PRINTER)) {
			    /* header -> printer */
			    if (rc=DumpResourceToFile(gPort, "PSHeader",
				    PS_DATA,PS_DL_PREFIX,
				    2,
				    PS_DATA,resID,PS_DATA,
				    PS_DL_SUFFIX))
			    {
				    goto ERROR;
			    }
			    CurEnv.fHeader=FALSE;

			} else
			{
			    short len;

			    /* header -> file */
			//  DialogBox(ghInst, "OPF", hwnd, fnFileDialog);
			    if (len = GetDlgItemText(hwnd, OP_FILE, gFileName,
				sizeof(gFileName) - 1))
				{
				gFileName[len] = '\0';
				if (gFileName[0]) {
				    if (rc=DumpResourceToFile(gFileName,
					    "PSHeader",
					    PS_DATA,PS_DL_PREFIX,
					    2,
					    PS_DATA,resID,PS_DATA,
					    PS_DL_SUFFIX))
				    {
					    goto ERROR;
				    }
				}
			    }
			}

                        /* Fall through from IDOK to IDCANCEL */

		case IDCANCEL:
		    WinHelp(hwnd, gszHelpFile, HELP_QUIT, 0L);
		    EndDialog(hwnd, wParam);
		    break;

		case IDHELP:
		    WinHelp(hwnd, gszHelpFile, HELP_CONTEXT, IDH_DLG_HEADER);
		    break;

		default:
		    return FALSE;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;

ERROR:
	PSDownloadError(rc);
	EndDialog(hwnd, wParam);
	return FALSE;
}

#if 0
1/17/92 LinS:  Not used any more.

/******************************************************************
* Name: fnHandshakeDialog()
*
* Action: This is a callback function that handles the dialog
*	  messages sent from Windows.  The dialog is initiated
*	  from the HandshakeDeviceMode function.
*
********************************************************************
*/

BOOL FAR PASCAL fnHandshakeDialog(hwnd, uMsg, wParam, lParam)
HWND		hwnd;
unsigned	uMsg;
WORD		wParam;
LONG		lParam;
{
	int iJobTimeout;
	BOOL fValOk;

	switch (uMsg) {
	case WM_INITDIALOG:
		SendDlgItemMessage(hwnd, JOBTIMEOUT, EM_LIMITTEXT, 5, 0L);
		SetDlgItemInt(hwnd, JOBTIMEOUT, CurEnv.iJobTimeout, FALSE);

		SetFocus(GetDlgItem(hwnd, JOBTIMEOUT));

		return FALSE;

	case WM_COMMAND:

		switch (wParam) {

		case IDOK:

			/* Get the job timeout in seconds */
			iJobTimeout = GetDlgItemInt(hwnd,JOBTIMEOUT,(BOOL FAR *)
				&fValOk,FALSE);
			if (!fValOk){
				PSError(PS_JOBTIMEOUT0);
			    break;
			}
			/* Check the job timeout range */
			if (iJobTimeout <= 0 || iJobTimeout > 3000)
				iJobTimeout = 0;
			CurEnv.iJobTimeout = iJobTimeout;

                        /* Fall through from IDOK to IDCANCEL */

		case IDCANCEL:
			EndDialog(hwnd, wParam);
			break;

		default:
			return FALSE;
		}
		break;

	default:
		return FALSE;

	}
	return TRUE;
}
#endif

void NEAR PASCAL EnableNonDSCOptions(HWND hwnd, BOOL bEnable)
{
    EnableWindow(GetDlgItem(hwnd, IDFREQUENCY), bEnable);
    EnableWindow(GetDlgItem(hwnd, IDFREQUENCYTEXT), bEnable);
    EnableWindow(GetDlgItem(hwnd, IDANGLE), bEnable);
    EnableWindow(GetDlgItem(hwnd, IDANGLETEXT), bEnable);
    // get the current button state so we can restore it
    if (!bEnable)
	 CurEnv.bPerPage = IsDlgButtonChecked(hwnd, IDPERPAGE);
    CheckDlgButton(hwnd, IDPERPAGE, bEnable ? CurEnv.bPerPage : TRUE);
    EnableWindow(GetDlgItem(hwnd, IDPERPAGE), bEnable);
    EnableWindow(GetDlgItem(hwnd, IDERRHANDLER), bEnable);
}

// !!! This routine must be internationalized
int NEAR PASCAL GetDlgItemTenths(HWND hwnd, int id, BOOL FAR *bTranslated, 
                                 BOOL bSigned)
{
    char buf[20];
    LPSTR p;
    int i;

    i = 10 * GetDlgItemInt(hwnd, id, bTranslated, bSigned);
    GetDlgItemText(hwnd, id, buf, sizeof(buf));

    for (p = buf; *p && *p != '.'; ++p)
        ;

    if (*p == '.') 
        ++p;

    if (*p)
        i = (i >= 0) ? (i + *p - '0') : (i - *p + '0');

    *bTranslated = TRUE;

    return i;
}


BOOL FAR PASCAL fnAdvancedDialog(hwnd, uMsg, wParam, lParam)
HWND		hwnd;
unsigned	uMsg;
WORD		wParam;
LONG		lParam;
{
    static BOOL bDSC;
    PPRINTER pPrinter;
    int i, j;
    BOOL bTranslated;
    char buf[32];
    int iRes[5], nRes, n;

    switch (uMsg) {
        case WM_INITDIALOG:
            wsprintf(buf, "%d.%d", CurEnv.ScreenFrequency / 10, 
                     CurEnv.ScreenFrequency % 10);
            SetDlgItemText(hwnd, IDFREQUENCY, buf);

            wsprintf(buf, "%d.%d", CurEnv.ScreenAngle / 10, 
                     CurEnv.ScreenAngle % 10);
            SetDlgItemText(hwnd, IDANGLE, buf);

            SetDlgItemInt(hwnd, PRINTERVM, CurEnv.iMaxVM, FALSE);

            /* Fill in resolution list */
            pPrinter = GetPrinter(CurEnv.iPrinter);
            if (pPrinter) {
                nRes = 0;
                iRes[nRes++] = pPrinter->defRes;

                for (i = 0; i < NUMDEVICERES; ++i) {
                    n = pPrinter->iDeviceRes[i];
                    if (!n)
                        break;

                    /* insert in sorted order */
                    for (j = nRes; j > 0 && iRes[j-1] > n; --j)
                        iRes[j] = iRes[j-1];
                    iRes[j] = n;
                    ++nRes;
                }

                /* Fill in download font formats.
                */
                LoadString(ghInst, IDS_DLFMTTYPE1, buf, sizeof(buf));
                SendDlgItemMessage(hwnd, IDDLFORMAT, CB_ADDSTRING, 0,
                                    (DWORD)((LPSTR)buf));
                LoadString(ghInst, IDS_DLFMTTYPE3, buf, sizeof(buf));
                SendDlgItemMessage(hwnd, IDDLFORMAT, CB_ADDSTRING, 0,
                                    (DWORD)((LPSTR)buf));
                if (ACCEPTS_TRUETYPE(pPrinter)) {
                    LoadString(ghInst, IDS_DLFMTTRUETYPE, buf, sizeof(buf));
                    SendDlgItemMessage(hwnd, IDDLFORMAT, CB_ADDSTRING, 0,
                                        (DWORD)((LPSTR)buf));
                } else if (CurEnv.iDLFontFmt == DLFMT_TRUETYPE)
                    CurEnv.iDLFontFmt = DLFMT_TYPE1;

                FreePrinter(pPrinter);

                for (i = 0; i < nRes; ++i) {
                    wsprintf(buf, "%d", iRes[i]);
                    SendDlgItemMessage(hwnd, IDRESOLUTION, CB_ADDSTRING, 0,
                                        (DWORD)((LPSTR)buf));
                }

            }

            /* Choose the current resolution */

//	    wsprintf(buf, "%d", CurEnv.iRes);
//	    SetDlgItemText(hwnd, IDRESOLUTION, buf);

	    for (i = 0; i < nRes; i++)
		if (iRes[i] == CurEnv.iRes)
		    {
		    SendDlgItemMessage(hwnd, IDRESOLUTION, CB_SETCURSEL, i, 0L);
		    break;
		    }

            CheckDlgButton(hwnd, IDNEGIMAGE, CurEnv.bNegImage);
            CheckDlgButton(hwnd, IDPERPAGE, CurEnv.bPerPage);
	    if (CurEnv.bNoDownLoad)
		{
		CheckDlgButton(hwnd, NODOWNLOAD, TRUE);
		CheckDlgButton(hwnd, SUBSTITUTE, FALSE);
		}
	    else
		CheckDlgButton(hwnd, SUBSTITUTE, CurEnv.bSubFonts ? 1 : 0);
            CheckDlgButton(hwnd, IDMIRROR, CurEnv.bMirror);
            CheckDlgButton(hwnd, IDCOLORTOBLACK, CurEnv.bColorToBlack);
            CheckDlgButton(hwnd, IDCOMPRESS, CurEnv.bCompress);
            bDSC = CurEnv.bDSC;
            CheckDlgButton(hwnd, IDCONFORMING, CurEnv.bDSC);
            CheckDlgButton(hwnd, IDERRHANDLER, CurEnv.bErrHandler);

            SendDlgItemMessage(hwnd, IDDLFORMAT, CB_SETCURSEL,
		(CurEnv.iDLFontFmt == DLFMT_TRUETYPE ? 2 :
		CurEnv.iDLFontFmt == DLFMT_TYPE3 ? 1 : 0), 0L);

	    EnableWindow(GetDlgItem(hwnd, NODOWNLOAD), bTTEnabled);
	    EnableWindow(GetDlgItem(hwnd, IDDLFORMATLABEL), !CurEnv.bNoDownLoad && bTTEnabled);
	    EnableWindow(GetDlgItem(hwnd, SUBSTITUTE), !CurEnv.bNoDownLoad && bTTEnabled);
	    EnableWindow(GetDlgItem(hwnd, FONTSUBSTITUTION), !CurEnv.bNoDownLoad && bTTEnabled);

            if (bDSC)
                EnableNonDSCOptions(hwnd, FALSE);
 
            SetFocus(GetDlgItem(hwnd, PRINTERVM));
	    return FALSE;
	    break;

        case WM_COMMAND:

	    switch (wParam) {

	        case IDOK:

                    i = GetDlgItemTenths(hwnd, IDFREQUENCY, &bTranslated, FALSE);
                    if (bTranslated)
                        CurEnv.ScreenFrequency = i;

                    i = GetDlgItemTenths(hwnd, IDANGLE, &bTranslated, TRUE);
                    if (bTranslated)
                        CurEnv.ScreenAngle = i;

                    i = GetDlgItemInt(hwnd, IDRESOLUTION, &bTranslated, FALSE);
                    if (bTranslated)
                        CurEnv.iRes = i;

                    i = GetDlgItemInt(hwnd, PRINTERVM, &bTranslated, FALSE);
                    if (bTranslated)
                        CurEnv.iMaxVM = i;

                    i = (int)SendDlgItemMessage(hwnd, IDDLFORMAT, CB_GETCURSEL, 0, 0L);
                    switch (i)
                    {
			case 1:
                            CurEnv.iDLFontFmt = DLFMT_TYPE3;
                            break;
			case 2:
                            CurEnv.iDLFontFmt = DLFMT_TRUETYPE;
                            break;
			default:
                            CurEnv.iDLFontFmt = DLFMT_TYPE1;
			    break;
		    }

                    CurEnv.bPerPage = IsDlgButtonChecked(hwnd, IDPERPAGE);
                    CurEnv.bNegImage = IsDlgButtonChecked(hwnd, IDNEGIMAGE);
                    CurEnv.bSubFonts = IsDlgButtonChecked(hwnd, SUBSTITUTE);

                    if(CurEnv.bSubFonts)
                        CurEnv.dm.dmTTOption = DMTT_SUBDEV;
                    else
                        CurEnv.dm.dmTTOption = DMTT_DOWNLOAD;
                    CurEnv.dm.dmFields |= DM_TTOPTION;
                        
                    CurEnv.bMirror = IsDlgButtonChecked(hwnd, IDMIRROR);
                    CurEnv.bColorToBlack = IsDlgButtonChecked(hwnd, IDCOLORTOBLACK);
                    CurEnv.bCompress = IsDlgButtonChecked(hwnd, IDCOMPRESS);
                    CurEnv.bErrHandler = IsDlgButtonChecked(hwnd, IDERRHANDLER);
                    CurEnv.bDSC = bDSC;

                    /* fall through to cancel */

	        case IDCANCEL:
		    WinHelp(hwnd, gszHelpFile, HELP_QUIT, 0L);
		    EndDialog(hwnd, wParam);
		    break;

                case IDDEFAULT:
                    pPrinter = GetPrinter(CurEnv.iPrinter);
                    if (!pPrinter)
                        break;

                    wsprintf(buf, "%d.%d", pPrinter->ScreenFreq / 10, 
                            pPrinter->ScreenFreq % 10);
                    SetDlgItemText(hwnd, IDFREQUENCY, buf);

                    wsprintf(buf, "%d.%d", pPrinter->ScreenAngle / 10, 
                            pPrinter->ScreenAngle % 10);
                    SetDlgItemText(hwnd, IDANGLE, buf);

                    SetDlgItemInt(hwnd, PRINTERVM, pPrinter->iMaxVM, FALSE);

                    wsprintf(buf, "%d", pPrinter->defRes);
                    SendDlgItemMessage(hwnd, IDRESOLUTION, CB_SELECTSTRING, -1,
                                        (DWORD)((LPSTR)buf));

                    FreePrinter(pPrinter);
                    break;
                    
                case IDCONFORMING:
                    if (HIWORD(lParam) == BN_CLICKED) {
                        bDSC = !bDSC;
                        EnableNonDSCOptions(hwnd, !bDSC);
                        CheckDlgButton(hwnd, IDCONFORMING, bDSC);
                    }
                    break;

		case NODOWNLOAD:
		    if (HIWORD(lParam) == BN_CLICKED)
			{
			if (CurEnv.bNoDownLoad = !CurEnv.bNoDownLoad)
			    {
			    CurEnv.bSubFonts = IsDlgButtonChecked(hwnd, SUBSTITUTE);
			    CheckDlgButton(hwnd, SUBSTITUTE, FALSE);
			    }
			else
			    CheckDlgButton(hwnd, SUBSTITUTE, CurEnv.bSubFonts);

			CheckDlgButton(hwnd, NODOWNLOAD, CurEnv.bNoDownLoad);

			EnableWindow(GetDlgItem(hwnd, SUBSTITUTE), !CurEnv.bNoDownLoad);
			EnableWindow(GetDlgItem(hwnd, FONTSUBSTITUTION), !CurEnv.bNoDownLoad);
			EnableWindow(GetDlgItem(hwnd, IDDLFORMATLABEL), !CurEnv.bNoDownLoad);
			EnableWindow(GetDlgItem(hwnd, IDDLFORMAT), !CurEnv.bNoDownLoad);
			}
		    break;

                case FONTSUBSTITUTION:
                    DialogBox(ghInst, "SUB", hwnd, fnSubstituteDialog);
                    break;

                case IDHELP:
		    WinHelp(hwnd, gszHelpFile, HELP_CONTEXT, IDH_DLG_ADVANCEDOPT);
                    break;


	        default:
		    return FALSE;
	    }
	    break;

        default:
	    if (uMsg == wHelpMessage)
		{
		WinHelp(hwnd, gszHelpFile, HELP_CONTEXT, IDH_DLG_ADVANCEDOPT);
		return(TRUE);
		}
	    else
		return FALSE;

    }

    return TRUE;
}


/******************************************************************
* Name: fnSubstituteDialog()
*
* Action: This is the dialog procedure for the Font Substitution 
*         dialog.
*
********************************************************************
*/

BOOL FAR PASCAL fnSubstituteDialog(hwnd, uMsg, wParam, lParam)
HWND		hwnd;
unsigned	uMsg;
WORD		wParam;
LONG		lParam;
{
	char szBuf[256], szTitle[32];
        DWORD dw;
        int i,j;
 static DV dv;
        HWND hwndTT, hwndDV;

	switch (uMsg) {

	case WM_INITDIALOG:
                /* set margin so that listboxes will scroll horizontally */
                hwndTT = GetDlgItem(hwnd, LBTRUETYPE);
                hwndDV = GetDlgItem(hwnd, LBDEVICE);

                /* dummy up a PDEVICE sufficient for EnumFonts() and MapFont() */
                dv.iPrinter = CurEnv.iPrinter;
                dv.iRes = CurEnv.iRes;
		dv.DevMode.hSubTable = 0;
                if (!LoadFontDir(CurEnv.iPrinter, gPort))
                    return FALSE;

                /* fill in the TrueType listbox */
                EnumFonts(&dv, NULL, SubFontProc, (LPSTR)MAKELONG(hwndTT, 0),
                          ENUM_TRUETYPE);

                /* fill in the Device listbox */
                LoadString(ghInst, IDS_DOWNLOAD, szBuf, sizeof(szBuf));
                SendMessage(hwndDV, LB_ADDSTRING, 0, (DWORD)((LPSTR)szBuf));
                EnumFonts(&dv, NULL, SubFontProc, (LPSTR)MAKELONG(hwndDV, 0),
			  ENUM_INTERNAL | ENUM_SOFTFONTS);

                /* fill in the associations */
		InitItemData(&dv, TRUE, hwndTT, hwndDV);

                /* select the first item in the TrueType listbox */
                SendMessage(hwndTT, LB_SETCURSEL, 0, 0L);
                SendMessage(hwnd, WM_COMMAND, LBTRUETYPE, 
                            MAKELONG(hwndTT, LBN_SELCHANGE));
		break;

	case WM_COMMAND:
		switch (wParam) {
		case IDOK:

		/* write out a new table */
		hwndTT = GetDlgItem(hwnd, LBTRUETYPE);
		hwndDV = GetDlgItem(hwnd, LBDEVICE);

		if(UpdateSubTable(&dv, hwndTT, hwndDV))
		    WriteSubTable(&dv);
		    /* fall through */

		case IDCANCEL:
                    DeleteFontDir(CurEnv.iPrinter);
		    FreeSubTable(&dv);
		    WinHelp(hwnd, gszHelpFile, HELP_QUIT, 0L);
		    EndDialog(hwnd, wParam);
		    break;

            case LBTRUETYPE:
                if (HIWORD(lParam) != LBN_SELCHANGE)
                    break;

                i = (int)SendMessage(LOWORD(lParam), LB_GETCURSEL, 0, 0L);
                dw = SendMessage(LOWORD(lParam), LB_GETITEMDATA, i, 0L);
                hwndDV = GetDlgItem(hwnd,LBDEVICE);
                SendMessage(hwndDV, LB_SETCURSEL, LOWORD(dw), 0L);

                /* post a selection change message for the device
                    * listbox since USER doesn't do it in response to
                    * a LB_SETCURSEL message.
                    */
                PostMessage(hwnd, WM_COMMAND, LBDEVICE, 
                            MAKELONG(hwndDV, LBN_SELCHANGE));
                break;

            case LBDEVICE:
                if (HIWORD(lParam) != LBN_SELCHANGE)
                    break;

                hwndDV = (HWND)LOWORD(lParam);

                i = (int)SendMessage(hwndDV, LB_GETCURSEL, 0, 0L);
	  //	dw = SendMessage(hwndDV, LB_GETITEMDATA, i, 0L);

#if 0
                /* update the status line */
                if (HIWORD(dw) & SUBID_MISSINGFONT)
                    LoadString(ghInst, IDS_SUBFONTMISSING, szBuf, sizeof(szBuf));
                else
                    *szBuf = '\0';
                SetWindowText(GetDlgItem(hwnd, IDSUBSTATUS), szBuf);
#endif

                hwndTT = GetDlgItem(hwnd, LBTRUETYPE);
                j = (int)SendMessage(hwndTT, LB_GETCURSEL, 0, 0L);
                SendMessage(hwndTT, LB_SETITEMDATA, j, MAKELONG(i,0));
                break;

            case IDHELP:
		WinHelp(hwnd, gszHelpFile, HELP_CONTEXT, IDH_DLG_SUBSTITUTION);
                break;
                   
            case IDDEFAULT:
                if (!LoadString(ghInst, IDS_SUBVERIFYDEFAULT, szBuf, sizeof(szBuf)))
                    break;

                if (!LoadString(ghInst, IDS_WARNING, szTitle, sizeof(szTitle)))
                    break;

                if (MessageBox(hwnd, szBuf, szTitle, MB_YESNO | MB_ICONEXCLAMATION)
                    == IDNO)
                    break;

                hwndTT = GetDlgItem(hwnd, LBTRUETYPE);
                hwndDV = GetDlgItem(hwnd, LBDEVICE);

	     // EraseSubTable();    /* remove win.ini entries */
	     // FreeSubTable();     /* delete the memory copy */
		InitItemData(&dv, FALSE, hwndTT, hwndDV);   /* reset connections */

                /* force connection of selected TrueType font to be
                 * updated.
                 */
                PostMessage(hwnd, WM_COMMAND, LBTRUETYPE, 
                            MAKELONG(hwndTT, LBN_SELCHANGE));
                break;
                    
		default:
			return FALSE;
		}
		break;

	default:
	    if (uMsg == wHelpMessage)
		{
		WinHelp(hwnd, gszHelpFile, HELP_CONTEXT, IDH_DLG_SUBSTITUTION);
		return(TRUE);
		}
	    else
		return FALSE;

	}

	return TRUE;
}

int FAR PASCAL SubFontProc(LPLOGFONT lplf, LPTEXTMETRIC lptm, short nType,
                           DWORD dwData)
{
    HWND hwnd;
    int  index;  

    hwnd = LOWORD(dwData);

    index = (int)SendMessage(hwnd, LB_FINDSTRING, -1, (DWORD)((LPSTR)lplf->lfFaceName));

    if(index == LB_ERR)  // string not found
        SendMessage(hwnd, LB_ADDSTRING, 0, (DWORD)((LPSTR)lplf->lfFaceName));

    return 1;
}

void NEAR PASCAL InitItemData(lpdv, bUser, hwndTT, hwndDV)
LPDV lpdv;
HWND hwndTT;
HWND hwndDV;
{
    LPSUBTAB lpTable;
    LPSUBENTRY lpCurEnt;
    int nSub, i, iTTFont, iDevFont;

    /* start by initializing all entries to "download" */
    nSub = (short)SendMessage(hwndTT, LB_GETCOUNT, 0, 0L);
    for (i = 0; i < nSub; ++i) {
        SendMessage(hwndTT, LB_SETITEMDATA, i, 0L);
    }

    lpTable = LockSubTable(lpdv, bUser);
    if (!lpTable)
        return;

    nSub = lpTable->nSub;
    lpCurEnt = &lpTable->SubEnt[0];

    for (i = 0; i < nSub; ++i, lpCurEnt++)
	{
	if (!lpCurEnt->rgDevFont[0])
	    continue;

        /* Find TrueType font & skip if not available */
	iTTFont = (int)FindLBIndex(hwndTT, &lpCurEnt->rgTTFont);
        if (iTTFont < 0)
            continue;

        /* Find Device font & skip if not available */
	iDevFont = (int)SendMessage(hwndDV, LB_FINDSTRING, -1, (DWORD)((LPSTR)(lpCurEnt->rgDevFont)));

	if (iDevFont < 0)  
        iDevFont = 0;
//      this is a useless informative message!
//	    MessageBox(0, "This should not happen, cannot find font", "", MB_OK);

        /* Associate the device font with the TrueType font */
        SendMessage(hwndTT, LB_SETITEMDATA, iTTFont, MAKELONG(iDevFont, 0));

    //	SendMessage(hwndDV, LB_SETITEMDATA, iDevFont, MAKELONG(0, wFlags));
    }

    UnlockSubTable(lpdv);
}



/* ConfigureCommPort - downloads a job telling the PostScript printer what
 * Control Panel has set for the port it is connected to.  This only occurs
 * if the printer is connected to a COMM port.
 *
 * Returns: TRUE if job was downloaded, FALSE if there was an error or
 * not connected to COMM port.
 */
BOOL FAR PASCAL ConfigureCommPort(void)
{
   PPRINTER pPrinter = NULL;
   LPDV lpdv;
   int iBaud, iOptions;
   BOOL bSuccess;
   char szBuf[40];

   bSuccess = GetCommParameters(&iBaud, &iOptions);
   if (!bSuccess)
      goto cleanup1;

   pPrinter = GetPrinter(CurEnv.iPrinter);
   if (!pPrinter) 
      goto cleanup1;

   if (!LoadString(ghInst, IDS_SETCOMMJOBNAME, szBuf, sizeof(szBuf)))
      *szBuf = '\0';

   lpdv = StartTmpJob(gPort, szBuf, pPrinter);
   if (!lpdv)
      goto cleanup1;

   PrintChannel(lpdv, "%d %d\n", iBaud, iOptions);
   DumpResourceString(lpdv, PS_DATA, PS_SETCOMM);

   bSuccess = TRUE;

   if (lpdv && pPrinter)
      EndTempJob(lpdv, pPrinter);

cleanup1:
   if (pPrinter)
      FreePrinter(pPrinter);

   return bSuccess;
}


/* Get Control Panel's port settings for the printers COMM channel and 
 * return the parameters necessary to set the comm parameters.  
 * 
 * return value: TRUE if printer is on COMM port and parameters fetched.
 *
 * the values in piBaud and piOptions will be passed to the PostScript 
 * engine for use with the setsccbatch operator.  The options field is
 * organized as follows (based on p. 121 of the LaserWriter II NT/NTX
 * owners guide):
 *
 * bit   description (all values in binary)
 * ---   -------------------------------------------
 *  7    # stop bits (0 = 1 stop bit, 1 = 2 stop bits)
 * 6,5   # data bits (01 = 7 bits, 10 = 8 bits)
 *  4    0
 * 3,2   handshaking method:  00 = XON/XOFF (Software)
 *                            01 = DSR/DTR  (Hardware)
 *                            10 = ETX/ACK
 * 1,0   parity: 00 = None/Space 
 *               01 = Odd
 *               10 = Even
 *               11 = None/Mark
 * 
 */
BOOL NEAR PASCAL GetCommParameters(int FAR *piBaud, int FAR *piOptions)
{
    char szIniPorts[12], szPort[40];
    char chTmp, FAR *lpsz;
    int  iDiff, iTmp;
    BOOL bSuccess = FALSE;

    /* we can only check if the printer is connected to a comm port */
    chTmp = gPort[3];
    gPort[3] = '\0';
    iDiff = lstrcmpi(gPort, "COM");
    gPort[3] = chTmp;
    if (iDiff)
        goto cleanup1;

    /* get Control Panel's port settings from win.ini */
    if (!LoadString(ghInst, IDS_PORTS, szIniPorts, sizeof(szIniPorts)))
        goto cleanup1;
    if (!GetProfileString(szIniPorts, gPort, szNull, szPort, sizeof(szPort)))
        goto cleanup1;

    /* parse the port line and compute the option value */
    *piOptions = 0;
    lpsz = farGetDecimal(szPort, piBaud);

    /* find the parity field */
    lpsz = NextField(lpsz, ',');
    if (!*lpsz)
        goto cleanup1;

    /* encode the parity value */
    switch (*lpsz) {
        case 'n':
        case 'N':
        case 's':
        case 'S':
           /* parity setting is 0 */ 
           break;

        case 'o':
        case 'O':
           *piOptions |= 1;
           break;

        case 'e':
        case 'E':
           *piOptions |= 2;
           break;

        case 'm':
        case 'M':
           *piOptions |= 3;
           break;
    }

    /* find the data bits field */
    lpsz = NextField(lpsz, ',');
    if (!*lpsz)
        goto cleanup1;
    lpsz = farGetDecimal(lpsz, &iTmp);

    /* encode the data bits field */
    switch (iTmp) {
        case 7:
           *piOptions |= 32;
           break;

        case 8:
           *piOptions |= 64;
           break;

        default:
           goto cleanup1;
    }

    /* find the stop bits field */
    lpsz = NextField(lpsz, ',');
    if (!*lpsz)
        goto cleanup1;
    lpsz = farGetDecimal(lpsz, &iTmp);

    /* encode the stop bits field */
    switch (iTmp) {
        case 1:
           /* stop bit setting is 0 */
           break;

        case 2:
           *piOptions |= 128;
           break;

        default:
           goto cleanup1;
    }

    /* find the handshake field */
    lpsz = NextField(lpsz, ',');

    /* encode the handshake field */
    switch (*lpsz) {
        case 'p':
        case 'P':
           *piOptions |= 4;
           break;
 
        case '\0':
        case 'x':
        case 'X':
           /* software handshaking is 0 */
           break;

        default:
           goto cleanup1;
    }

    bSuccess = TRUE;

cleanup1:
   return bSuccess;
}


/*
 * NextField - points to the next field in the string.  The field separator
 * is specified by ich.
 *
 */
LPSTR NEAR PASCAL NextField(LPSTR lpsz, char ch)
{
    while (*lpsz && *lpsz != ch)
        lpsz = AnsiNext(lpsz);

    if (*lpsz)
        lpsz = AnsiNext(lpsz);

    return lpsz;
}


void NEAR PASCAL InitDuplex(HWND hwnd, PPRINTER pPrinter)
{
	HWND hwndDup;
	RECT rectDlg;
	RECT rectDup;

	/* If duplex is supported, then make sure the devmode is
	** valid and turn off buttons for which we have no PostScript
	** strings.
	*/
	if (IS_DUPLEX(pPrinter) && (pPrinter->iCapsBits & CAPS_SIMPLEX)) {

		/* If devmode is invalid then set the default.
		*/
		if (!(CurEnv.dm.dmFields & DM_DUPLEX) ||
				CurEnv.dm.dmDuplex < DMDUP_SIMPLEX ||
				CurEnv.dm.dmDuplex > DMDUP_HORIZONTAL) {
			CurEnv.dm.dmFields |= DM_DUPLEX;
			CurEnv.dm.dmDuplex = DMDUP_SIMPLEX;
		}

		/* If we do not have a vertical binding string then
		** disable the radio button.
		*/
		if (!(pPrinter->iCapsBits & CAPS_DUPLEX_VERT)) {
			EnableWindow(GetDlgItem(hwnd, DUPLEX_LONGEDGE), FALSE);
			if (CurEnv.dm.dmDuplex == DMDUP_VERTICAL)
				CurEnv.dm.dmDuplex = DMDUP_SIMPLEX;
		}

		/* If we do not have a horizontal binding string then
		** disable the radio button.
		*/
		if (!(pPrinter->iCapsBits & CAPS_DUPLEX_HORZ)) {
		    	EnableWindow(GetDlgItem(hwnd, DUPLEX_SHORTEDGE), FALSE);
			if (CurEnv.dm.dmDuplex == DMDUP_HORIZONTAL)
				CurEnv.dm.dmDuplex = DMDUP_SIMPLEX;
		}

		/* Select the radio button and icon.
		*/
		SetDuplex(hwnd, CurEnv.dm.dmDuplex, CurEnv.dm.dmOrientation);
	} else {
		/* This printer does not support duplex or does not
		** have a simplex string (required), so hide the
		** duplex box (we do this by resizing the More dialog
		** so its bottom aligns with the top of the duplex box).
		*/
		hwndDup = GetDlgItem(hwnd, DUPLEX_WNDFRAME);
		GetWindowRect(hwnd, &rectDlg);
		GetWindowRect(hwndDup, &rectDup);
		ShowWindow(hwndDup, SW_HIDE);
		MoveWindow(hwnd, rectDlg.left, rectDlg.top, rectDlg.right - rectDlg.left,
			rectDup.top - rectDlg.top + 2, TRUE);
	}
}

void NEAR PASCAL SetDuplex(HWND hwnd, WORD dmDuplex, WORD dmOrient)
{
	HANDLE	hIcon;
	WORD button, wVers;

	/* A note on duplex binding terms:
	**
	**  Orientation  Windows term   HP term      PostScript term
	**  -----------  ------------   -------      ---------------
	**  portrait     horizontal     short edge   tumble
	**               vertical       long edge    no tumble
	**  landscape    vertical       short edge   tumble
	**               horizontal     long edge    no tumble
	**
	** Notice that the HP and PostScript terms are in sync, but the
	** Windows terms switch meaning depending upon the orientation.
	** However, the two LaserJet drivers assume that "horizontal"
	** and "short edge" are synonomous regardless of the orientation,
	** and the same for "vertical" and "long edge," so we make the
	** same assumption to be consistent.
	*/
	if (dmOrient == DMORIENT_LANDSCAPE) {
		switch (dmDuplex) {
			case DMDUP_SIMPLEX:
				button = DUPLEX_NONE;
				hIcon = LoadIcon(ghInst, "D_LN");
				break;
			case DMDUP_HORIZONTAL:
				button = DUPLEX_SHORTEDGE;
				hIcon = LoadIcon(ghInst, "D_LV");
				break;
			case DMDUP_VERTICAL:
				button = DUPLEX_LONGEDGE;
				hIcon = LoadIcon(ghInst, "D_LH");
				break;
		}
	} else {
		switch (dmDuplex) {
			case DMDUP_SIMPLEX:
				button = DUPLEX_NONE;
				hIcon = LoadIcon(ghInst, "D_PN");
				break;
			case DMDUP_HORIZONTAL:
				button = DUPLEX_SHORTEDGE;
				hIcon = LoadIcon(ghInst, "D_PH");
				break;
			case DMDUP_VERTICAL:
				button = DUPLEX_LONGEDGE;
				hIcon = LoadIcon(ghInst, "D_PV");
				break;
		}
	}

	CheckRadioButton(hwnd, DUPLEX_NONE, DUPLEX_SHORTEDGE, button);

    wVers = GetVersion();

    if (LOBYTE(wVers) > 0x03 || (LOBYTE(wVers) == 0x03 && HIBYTE(wVers) > 0x00))
        SendDlgItemMessage(hwnd, IDICON, STM_SETICON, (WORD)hIcon, 0L);
    else
    	SetDlgItemText(hwnd, IDICON, MAKEINTRESOURCE(hIcon));
}


LONG FAR PASCAL FindLBIndex(HWND hwndList, LPSTR lpStr)
{
    char szBuf[128];
    int i;

    i = (int)SendMessage(hwndList, LB_FINDSTRING, -1, (DWORD)lpStr);

    /* if prefix not found then string doesn't exist */
    if (i < 0)
        return -1L;

    /* get the entry and compare to the string we're looking for */
    SendMessage(hwndList, LB_GETTEXT, i, (DWORD)((LPSTR)szBuf));
    return lstrcmpi(lpStr, szBuf) ? -1L : (DWORD)i;
}

//--------------------------------------------------------------------------
// Function: GetOneUserPaperSize(HWND, int, short, short, unsigned)
//
// Action:
//	    This function extracts the required paper dimension (X or Y)
//	    value from the given dialog control and check if it is valid.
//	    It returns the dimension size (in the current unit) if it's
//	    valid. Otherwise it returns -1.
//
// Reference:
//	    gInstUnidrv.
//
//-------------------------------------------------------------------------------
short NEAR PASCAL GetOneUserPaperSize(hDlg, nDlgItemID, sMin, sMax,
				      nOutOfRangeMsgID)
HWND	hDlg;	      // handle to the dialog box.
int	nDlgItemID;   // id of the edit-control interested.
short	sMin, sMax;   // the dimension limit in the current measure unit.
unsigned nOutOfRangeMsgID;
{
    BOOL bOK;
    unsigned nSize;

    nSize = GetDlgItemInt(hDlg, nDlgItemID, &bOK, FALSE);
    // check if the user input is legal
    if (bOK && nSize >= sMin && nSize <= sMax)
	return(nSize);
    else
	{
	char szCap[40];
	char szText[80];

	LoadString(ghInst, IDS_MSG_USERSIZE, (LPSTR)szCap, sizeof(szCap));
	LoadString(ghInst, nOutOfRangeMsgID, (LPSTR)szText, sizeof(szText));
	MessageBox(hDlg, (LPSTR)szText, (LPSTR)szCap, MB_ICONSTOP | MB_OK);
	return(-1);
	}
}

BOOL FAR PASCAL PaperSizeDlgProc(hDlg, dMsg, wParam, lParam)
HWND hDlg;	// handle to the dialog box
unsigned dMsg;	// message sent to the dialog box
WORD wParam;
LONG lParam;
{
    short sWidth, sLength; // user-defined paper size in current measure unit.
    POINT ptMinSize, ptMaxSize;
    static short sLastSizeUnit; // used to get around a Win3.0 bug (infinite
				// loop when displaying a message box in
				// response to clicking a radio button).
    int iMaxDimension;

    switch (dMsg)
    {
    case WM_INITDIALOG:
	// check the proper unit radio button.
	CheckRadioButton(hDlg, IDD_TENTHMM, IDD_HUNDREDTHINCH,
			 CurEnv.dmSizeUnit + IDD_UNIT_OFFSET);

        iMaxDimension = MulDiv(32767, 
                               (CurEnv.dmSizeUnit == DMUNIT_TENTHMM) ? 254 : 100,
                               CurEnv.iRes);

	SetDlgItemInt(hDlg, IDD_MAXWIDTH, iMaxDimension, TRUE);
	SetDlgItemInt(hDlg, IDD_MAXLENGTH, iMaxDimension, TRUE);
	SetDlgItemInt(hDlg, IDD_MINWIDTH, 0, TRUE);
	SetDlgItemInt(hDlg, IDD_MINLENGTH, 0, TRUE);

	// display the current user-defined paper size
	if (CurEnv.dmSizeUnit == DMUNIT_TENTHMM)
	    {
	    SetDlgItemInt(hDlg, IDD_PAPERWIDTH, CurEnv.iCustomWidth, TRUE);
	    SetDlgItemInt(hDlg, IDD_PAPERLENGTH,CurEnv.iCustomHeight, TRUE);
	    }
	else
	    {
	    // convert the size from 0.1 mm to 0.01 inch.
	    SetDlgItemInt(hDlg, IDD_PAPERWIDTH,
			  MetricToBritish(CurEnv.iCustomWidth), TRUE);
	    SetDlgItemInt(hDlg, IDD_PAPERLENGTH,
			  MetricToBritish(CurEnv.iCustomHeight), TRUE);
	    }
        sLastSizeUnit = CurEnv.dmSizeUnit;
	return (TRUE);

    case WM_COMMAND:
	switch (wParam)
	{
	case IDD_TENTHMM:
	case IDD_HUNDREDTHINCH:
	    // if the unit hasn't really changed since last time, do nothing.
	    if (wParam == IDD_UNIT_OFFSET + sLastSizeUnit)
		{
		// get out of the infinite loop and resume to normal.
		sLastSizeUnit = CurEnv.dmSizeUnit;
		return(TRUE);
		}

            iMaxDimension = MulDiv(32767, 
                            (wParam - IDD_UNIT_OFFSET == DMUNIT_TENTHMM) ? 254 : 100,
                            CurEnv.iRes);

	    // Otherwise, retain what the user has specified in the old unit.
            ptMinSize.x = ptMinSize.y = 0;
            ptMaxSize.x = ptMaxSize.y = iMaxDimension;

	    sLastSizeUnit = wParam - IDD_UNIT_OFFSET;

	    if ((sWidth = GetOneUserPaperSize(hDlg, IDD_PAPERWIDTH,
				ptMinSize.x, ptMaxSize.x, IDS_MSG_WIDTHTOOBIG))
		  == -1 ||
		(sLength = GetOneUserPaperSize(hDlg, IDD_PAPERLENGTH,
				ptMinSize.y, ptMaxSize.y, IDS_MSG_LENGTHTOOBIG))
		  == -1)
		{
		return(TRUE);
		}

	    // re-display everything in the new measure unit.
	    CurEnv.dmSizeUnit = wParam - IDD_UNIT_OFFSET;
	    CheckRadioButton(hDlg, IDD_TENTHMM, IDD_HUNDREDTHINCH, wParam);

            iMaxDimension = MulDiv(32767, 
                            (CurEnv.dmSizeUnit == DMUNIT_TENTHMM) ? 254 : 100,
                            CurEnv.iRes);

	    // Otherwise, retain what the user has specified in the old unit.
            ptMinSize.x = ptMinSize.y = 0;
            ptMaxSize.x = ptMaxSize.y = iMaxDimension;

	    SetDlgItemInt(hDlg, IDD_MAXWIDTH, ptMaxSize.x, TRUE);
	    SetDlgItemInt(hDlg, IDD_MAXLENGTH, ptMaxSize.y, TRUE);
	    SetDlgItemInt(hDlg, IDD_MINWIDTH, ptMinSize.x, TRUE);
	    SetDlgItemInt(hDlg, IDD_MINLENGTH, ptMinSize.y, TRUE);

	    // display the current user-defined paper size.
	    // NOTE: we know for sure that the unit has changed.
	    if (CurEnv.dmSizeUnit == DMUNIT_TENTHMM)
		{
		// convert from 0.01 inch units to 0.1 mm units.
		SetDlgItemInt(hDlg, IDD_PAPERWIDTH, BritishToMetric(sWidth), TRUE);
		SetDlgItemInt(hDlg, IDD_PAPERLENGTH, BritishToMetric(sLength), TRUE);
		}
	    else
		{
		// convert the size from 0.1 mm to 0.01 inch.
		SetDlgItemInt(hDlg, IDD_PAPERWIDTH, MetricToBritish(sWidth), TRUE);
		SetDlgItemInt(hDlg, IDD_PAPERLENGTH, MetricToBritish(sLength), TRUE);
		}
	    return(TRUE);

	case IDOK:
            iMaxDimension = MulDiv(32767, 
                            (CurEnv.dmSizeUnit == DMUNIT_TENTHMM) ? 254 : 100,
                            CurEnv.iRes);

	    // get the user-specified values and check if they are valid.
            ptMinSize.x = ptMinSize.y = 0;
            ptMaxSize.x = ptMaxSize.y = iMaxDimension;

	    if ((sWidth = GetOneUserPaperSize(hDlg, IDD_PAPERWIDTH,
				ptMinSize.x, ptMaxSize.x, IDS_MSG_WIDTHTOOBIG))
		  == -1 ||
		(sLength = GetOneUserPaperSize(hDlg, IDD_PAPERLENGTH,
				ptMinSize.y, ptMaxSize.y, IDS_MSG_LENGTHTOOBIG))
		  == -1)
		{
		return(TRUE);
		}
	    // convert the width and length measure into 0.1mm unit,
	    // if necessary.
	    if (CurEnv.dmSizeUnit != DMUNIT_TENTHMM)
		{
		CurEnv.iCustomWidth = BritishToMetric(sWidth);
		CurEnv.iCustomHeight = BritishToMetric(sLength);
		}
	    else
		{
		CurEnv.iCustomWidth = sWidth;
		CurEnv.iCustomHeight = sLength;
		}
	    // fall through the IDCANCEL case.
	case IDCANCEL:
	    EndDialog(hDlg, wParam);
	    return(TRUE);

	default:
	    return (FALSE);
	}
    default:
	return (FALSE);   /* didn't process the message */
    }
}
