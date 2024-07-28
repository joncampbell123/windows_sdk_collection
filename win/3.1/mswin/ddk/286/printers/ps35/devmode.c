/**[f******************************************************************
 * devmode.c - 
 *
 * Copyright (C) 1988-1989 Aldus Corporation, Microsoft Corporation.
 * Copyright (C) 1989 Microsoft Corporation.
 * All rights reserved.  Company confidential.
 *
 **f]*****************************************************************/

/***************************************************************************/
/******************************   devmode.c   ******************************/
/*
 *  Procs for handling driver-specific dialog.
 *
 *   1-12-89	jimmat	Most DeviceMode local data is now allocated via
 *			GlobalAlloc() instead of statically allocated in DGROUP.
 *   1-13-89	jimmat	Reduced # of redundant strings by adding lclstr.h
 *   1-17-89	jimmat	Added PCL_* entry points to lock/unlock data seg.
 *   1-25-89	jimmat	Use global hLibInst instead of GetModuleHandle().
 *   1-26-89	jimmat	Added dynamic link to Font Installer library.
 *   2-07-89	jimmat	Driver Initialization changes.
 *   2-20-89	jimmat	Driver/Font Installer use same WIN.INI section (again)!
 *   2-21-89	jimmat	Device Mode Dialog box changes for Windows 3.0.
 *   2-24-89	jimmat	Removed parameters from lp_enbl() & lp_disable().
 *   3-13-89	chrisg	cloning over to the PS driver.
 *   3-16-89	chrisg	seems to be working...
 */

#include "pscript.h"
#include <winexp.h>
#include "driver.h"
#include "profile.h"
#include "utils.h"
#include "debug.h"
#include "getdata.h"
#include "dmrc.h"
#include "control2.h"
#include "psver.h"
#include "pserrors.h"

#define MMPERIN 254	/* mm per inch */

BOOL DevModeBusy = FALSE;

PSDEVMODE CurEnv, OldEnv;
char gPort[66];

extern HANDLE ghInst;
extern BOOL bInAppSetup;

// 09/23/91 ZhanW
// define global variables for supporting F1 help.
FARPROC lpfnFilterProc = NULL;
FARPROC lpfnNextHook = NULL;
WORD	wHelpMessage = 0;


BOOL FAR PASCAL fnDialog(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL fnOptionsDialog(HWND hwnd, unsigned uMsg, WORD wParam, LONG lParam);

void MergeEnvironment(LPPSDEVMODE, LPPSDEVMODE);


/***********************************************************************
		       E X T D E V I C E M O D E
 ***********************************************************************/

/*
 * NOTE: be very careful of any static data used/set by this function
 * (and the functions it calls) because this routine may be reentered
 * by different applications.	For example, one app may have caused the
 * device mode dialog to appear (DM_PROMPT), and another app might
 * request a copy of the current DEVMODE settings (DM_COPY).  Multiple
 * calls for DM_PROMPT and/or DM_UPDATE processing are not allowed.
 *
 * whatever lpProfile points to better be locked because things
 * may move around while the dialog box is up.
 */

short FAR PASCAL ExtDeviceMode(
	HWND hWnd, 
	HANDLE hInst, 
	LPPSDEVMODE lpdmOutput,
	LPSTR lpDeviceName, 
	LPSTR lpPort, 
	LPPSDEVMODE lpdmInput,
	LPSTR lpProfile, 
	WORD Mode)
{
	short	rc, exclusive;
	char DeviceName[80];		/* keep copy if callers DS moves */

	DBMSG(("ExtDeviceMode(%d,%d,%lp,%lp,%lp,%lp,%lp,%d)\n",
	    hWnd, hInst, lpdmOutput, lpDeviceName, lpPort, lpdmInput,
	    lpProfile, Mode));
	DBMSG(("     DeviceName = ->%ls<-   Port = ->%ls<-\n",
	    lpDeviceName ? lpDeviceName : (LPSTR) "NULL",
	    lpPort ? lpPort : (LPSTR) "NULL"));
	DBMSG(("     Profile    = ->%ls<-\n",
	    lpProfile ? lpProfile : (LPSTR) "NULL"));
	DBMSG(("     Mode = %ls %ls %ls %ls\n", Mode & DM_UPDATE ? (LPSTR)"UPDATE" : (LPSTR)"",
	    Mode & DM_COPY ? (LPSTR)"COPY" : (LPSTR)"", Mode & DM_PROMPT ? (LPSTR)"PROMPT" : 
	    (LPSTR)"", Mode & DM_MODIFY ? (LPSTR)"MODIFY" : (LPSTR)""));

	lstrcpy(DeviceName, lpDeviceName);


	/* Mode == 0 is a request for the full size of the DEVMODE structure */

	if (!Mode) {
		DBMSG(("ExtDeviceMode returning size: %d\n", sizeof(PSDEVMODE)));
		return sizeof(PSDEVMODE);
	}


	/* Okay, there is some real work to do.  Make sure we haven't been
           (re)entered more than once to UPDATE or PROMPT (possibly by two or
           more applications), then allocate and lock down our data areas */

	exclusive = Mode & (DM_UPDATE | DM_PROMPT);

	if (DevModeBusy)
	    {
	    if (exclusive)
		{
		PSError(PS_DLGBUSY);
		return -1;
		}
	    }
	else
	    DevModeBusy = exclusive;

	/* Initialize a few items in the DevMode data area */

	lstrcpy(gPort, lpPort);

	/* Get a copy of the environment--build one if the user gave us a
           private .INI file, or there is no current environment, or it doesn't
           match our device */

#if 0
	lstrcpy(CurEnv.dm.dmDeviceName, lpDeviceName);

	if (lpProfile || 
	    !GetEnvironment(lpPort, (LPSTR)&CurEnv, sizeof(PSDEVMODE)) || 
	    lstrcmpi(lpDeviceName, CurEnv.dm.dmDeviceName))
#endif

		MakeEnvironment(DeviceName, gPort, &CurEnv, lpProfile);

    OldEnv = CurEnv;   //  must save a copy of the original Env
                        //  SaveEnv  uses this to write only
                        //  changed settings to File.

	/* If the user passed in a DEVMODE structure, merge it with the current
           environment before going futher */

	if ((Mode & DM_MODIFY) && lpdmInput) {

		/* if this is one of ours we need to get some stuff */

		if (lpdmInput->dm.dmSize == sizeof(DEVMODE) &&
		    lpdmInput->dm.dmDriverVersion == DRIVER_VERSION &&
		    lpdmInput->dm.dmSpecVersion == GDI_VERSION &&
		    lpdmInput->dm.dmDriverExtra == sizeof(PSDEVMODE)-sizeof(DEVMODE) &&
		    !lstrcmpi(lpDeviceName, lpdmInput->dm.dmDeviceName)) {

			DBMSG((" Input Env is one of ours, using it's extra data\n"));

			lmemcpy((LPSTR)&CurEnv   + sizeof(DEVMODE),
		    		(LPSTR)lpdmInput + sizeof(DEVMODE),
			    	sizeof(PSDEVMODE) - sizeof(DEVMODE));
		} else {
			DBMSG((" Input Env not one of ours\n"));
		}

		MergeEnvironment(&CurEnv, lpdmInput);
	}

	/* Throw-up the device mode dialog box if the caller wants us to
           prompt the user for any changes */

	if (Mode & DM_PROMPT)
    {
        PPSDEVMODE  pPrevsEnv;

        pPrevsEnv = AllocDevMode();
        if (pPrevsEnv) {
           *pPrevsEnv = CurEnv;   //  PrevsEnv saves settings if user aborts the
                           //  dialog Box.

	    wHelpMessage = RegisterWindowMessage((LPSTR)"pscript_help");
	    lpfnFilterProc = GetProcAddress(ghInst, (LPSTR)(LONG)250);
	    lpfnNextHook = SetWindowsHook(WH_MSGFILTER, lpfnFilterProc);

	    rc = DialogBox(ghInst, "DM", hWnd, fnDialog);

	    UnhookWindowsHook(WH_MSGFILTER, lpfnFilterProc);

	    if(rc != IDOK)
                 CurEnv = *pPrevsEnv;

           FreeDevMode(pPrevsEnv);
        } else {
           rc = IDCANCEL;
        }
	}
    else
		rc = IDOK;	/* didn't prompt, but we still give the okay return */


	/* If the caller wants a copy of the resulting environment,
           give it to 'em */

	if ((Mode & DM_COPY) && lpdmOutput) {

		*(LPPSDEVMODE)lpdmOutput = CurEnv;	/* check rc == IDOK ? */
	}

#ifdef LOCAL_DEBUG
	dumpDevMode(&CurEnv);
#endif

	/* Finally, update the default environment if everything is okay so far
           (and the user didn't Cancel the dialog box), and the caller wants us
           to do so */

	if ((Mode & DM_UPDATE) && rc == IDOK) {

		SaveEnvironment(DeviceName, gPort, &CurEnv, &OldEnv, lpProfile, TRUE, TRUE);
	}


	if (exclusive)			/* since there can only be 1 exclusive	   */
		DevModeBusy = FALSE;	/* invocation, no longer "busy" if this it */

	DBMSG(("ExtDeviceMode() returning %d\n", rc));

	return rc;
}


/***********************************************************************
			 D E V I C E M O D E
 ***********************************************************************/

int FAR PASCAL DeviceMode(HANDLE hWnd, HANDLE hInst, LPSTR lpDevice, LPSTR lpPort)
{

	return (ExtDeviceMode(hWnd, hInst, NULL, lpDevice, lpPort, NULL, NULL,
	    DM_PROMPT | DM_UPDATE) == IDOK);
}


/***********************************************************************
		  A D V A N C E D  S E T U P  D I A L O G
 ***********************************************************************/

short FAR PASCAL AdvancedSetupDialog(HWND hWnd, HANDLE hInst, 
                                     LPPSDEVMODE lpdmInput,
	                             LPPSDEVMODE lpdmOutput)
{
    int rval = -1;	   /* return value from extdevicemode */

    /* Make sure we don't re-enter ourselves */
    if (DevModeBusy)
	{
	PSError(PS_DLGBUSY);
	return rval;
	}

    DevModeBusy = TRUE;     /* prevent DeviceMode dialog from coming up too */
    bInAppSetup = TRUE;     /* we are entering the Advanced Setup Dialog... */

    OldEnv = CurEnv;        /* save the current environment */

    /* Initialize the current environment */

    MakeEnvironment(lpdmInput ? lpdmInput->dm.dmDeviceName : "A", gPort, &CurEnv, NULL);

    if (lpdmInput) {
        if (lpdmInput->dm.dmSize == sizeof(DEVMODE) &&
	    lpdmInput->dm.dmDriverVersion == DRIVER_VERSION &&
	    lpdmInput->dm.dmSpecVersion == GDI_VERSION &&
	    lpdmInput->dm.dmDriverExtra == sizeof(PSDEVMODE)-sizeof(DEVMODE)) {
	        DBMSG((" Input Env is one of ours, using it's extra data\n"));

	        lmemcpy((LPSTR)&CurEnv   + sizeof(DEVMODE),
		        (LPSTR)lpdmInput + sizeof(DEVMODE),
		        sizeof(PSDEVMODE) - sizeof(DEVMODE));
	} else
	    {
	    DBMSG((" Input Env not one of ours\n"));
	    goto error;
	    }

        MergeEnvironment(&CurEnv, lpdmInput);
    }

    wHelpMessage = RegisterWindowMessage((LPSTR)"pscript_help");
    lpfnFilterProc = GetProcAddress(ghInst, (LPSTR)(LONG)250);
    lpfnNextHook = SetWindowsHook(WH_MSGFILTER, lpfnFilterProc);

    /* bring up the More (options) box */
    rval = (DialogBox(hInst, "OP", hWnd, fnOptionsDialog) == IDOK);

    UnhookWindowsHook(WH_MSGFILTER, lpfnFilterProc);

    /* save/update the current environment */
    if (rval) {
        if (lpdmOutput)
            *(LPPSDEVMODE)lpdmOutput = CurEnv;
    } else {
        CurEnv = OldEnv;
    }

error:
    bInAppSetup = FALSE;    /* we have left the Advanced Setup Dialog */
    DevModeBusy = FALSE;    /* DeviceMode dialog allowed now */
    
    return rval;    
}

/***********************************************************************
		    M E R G E  E N V I R O N M E N T
 ***********************************************************************/

/*  Merge source and destination environments into the destination. */

void MergeEnvironment(LPPSDEVMODE lpDest, LPPSDEVMODE lpSrc)
{
	PPRINTER pPrinter;
	short	value;
	long	Fields = lpSrc->dm.dmFields;
	PPAPER pPaper, pP;


    /* TT font Substitution  and consistency check*/
    if(lpSrc->dm.dmFields & DM_TTOPTION)
    {
        if(lpSrc->dm.dmTTOption == DMTT_DOWNLOAD)
        {
            lpDest->dm.dmTTOption = DMTT_DOWNLOAD;
            lpDest->bSubFonts = FALSE;
        }
        else if(lpSrc->dm.dmTTOption == DMTT_SUBDEV)
        {
            lpDest->dm.dmTTOption = DMTT_SUBDEV;
            lpDest->bSubFonts = TRUE;
        }

        //  let lpDest determine its own future.
        //  just make sure its consistent.

        else if(lpDest->dm.dmFields & DM_TTOPTION)
        {
            if(lpDest->dm.dmTTOption == DMTT_DOWNLOAD)
                lpDest->bSubFonts = FALSE;
            else if(lpDest->dm.dmTTOption == DMTT_SUBDEV)
                lpDest->bSubFonts = TRUE;
            else if(lpDest->bSubFonts == TRUE)
                lpDest->dm.dmTTOption = DMTT_SUBDEV;
            else
                lpDest->dm.dmTTOption = DMTT_DOWNLOAD;
        }
        else if(lpDest->bSubFonts == TRUE)
            lpDest->dm.dmTTOption = DMTT_SUBDEV;
        else
            lpDest->dm.dmTTOption = DMTT_DOWNLOAD;
    }
    else 
    {
        lpDest->bSubFonts = lpSrc->bSubFonts;

        if(lpDest->bSubFonts == TRUE)
            lpDest->dm.dmTTOption = DMTT_SUBDEV;
        else
            lpDest->dm.dmTTOption = DMTT_DOWNLOAD;
    }

    lpDest->dm.dmFields |= DM_TTOPTION;

	/* portrait/landscape */

	if (Fields & DM_ORIENTATION)
		if ((value = lpSrc->dm.dmOrientation) == DMORIENT_PORTRAIT || 
		    value == DMORIENT_LANDSCAPE)
			lpDest->dm.dmOrientation = value;

	/* Copies?	We can do that! */

	if (Fields & DM_COPIES)
		lpDest->dm.dmCopies = lpSrc->dm.dmCopies;

	if (Fields & DM_DUPLEX) {
		if ((value = lpSrc->dm.dmDuplex) >= DMDUP_SIMPLEX &&
		    value <= DMDUP_HORIZONTAL)
		    	lpDest->dm.dmDuplex = value;
	}

	/* PrintQuality?  No problem! */

	/* The allowed range of paper sizes also depends of printer type */

	pPrinter = GetPrinter(lpDest->iPrinter);

	/* if they specify a paper width and length search for one that
	 * might match and set the dmPaperSize if that size is found */

	if (pPrinter && (Fields & (DM_PAPERLENGTH | DM_PAPERWIDTH))) {

		for (pP = pPaper = GetPaperMetrics(pPrinter, lpDest); pP->iPaper; pP++)

			if (pP->cxPaper == Scale(lpSrc->dm.dmPaperWidth, MMPERIN, 100) &&
			    pP->cyPaper == Scale(lpSrc->dm.dmPaperLength, MMPERIN, 100))
				break;

		if (pP->iPaper)
			lpDest->dm.dmPaperSize = pP->iPaper;

		LocalFree((HANDLE)pPaper);
	}


	if (pPrinter && (Fields & DM_PAPERSIZE)) {

		if (pPrinter->feed[lpDest->dm.dmDefaultSource - DMBIN_FIRST])
			lpDest->dm.dmPaperSize = lpSrc->dm.dmPaperSize;
	}

	if (pPrinter && (Fields & DM_DEFAULTSOURCE) && (lpDest->dm.dmFields & DM_DEFAULTSOURCE)) {

		if (pPrinter->feed[lpDest->dm.dmDefaultSource - DMBIN_FIRST])
			lpDest->dm.dmDefaultSource = lpSrc->dm.dmDefaultSource;
	}

	if (pPrinter)
		FreePrinter(pPrinter);

	/* copy over the scale value */

	if (Fields & DM_SCALE)
		lpDest->dm.dmScale = lpSrc->dm.dmScale;

	if (Fields & DM_COLOR)
		lpDest->dm.dmColor = lpSrc->dm.dmColor;

#ifdef DEBUG
	DBMSG(("MergeEnvironment: merged PSDEVMODE follows:\n"));
	dumpDevMode(lpDest);
#endif

}


#ifdef DEBUG

/***********************************************************************
		     D E B U G	  R O U T I N E S
 ***********************************************************************/

LOCAL void dumpDevMode(LPPSDEVMODE lpEnv) 
{

	DBMSG(("     dmDeviceName: %ls\n", (LPSTR)lpEnv->dm.dmDeviceName));
	DBMSG(("     dmSpecVersion: %4xh\n", lpEnv->dm.dmSpecVersion));
	DBMSG(("     dmDriverVersion: %4xh\n", lpEnv->dm.dmDriverVersion));
	DBMSG(("     dmSize: %d\n", lpEnv->dm.dmSize));
	DBMSG(("     dmDriverExtra: %d\n", lpEnv->dm.dmDriverExtra));
	DBMSG(("     dmFields: %8lxh\n", lpEnv->dm.dmFields));
	DBMSG(("     dmOrientation: %d\n", lpEnv->dm.dmOrientation));
	DBMSG(("     dmPaperSize: %d\n", lpEnv->dm.dmPaperSize));
	DBMSG(("     dmCopies: %d\n", lpEnv->dm.dmCopies));
	DBMSG(("     dmDefaultSource: %d\n", lpEnv->dm.dmDefaultSource));
	DBMSG(("     dmPrintQuality: %d\n", lpEnv->dm.dmPrintQuality));
	DBMSG(("     dmColor: %d\n", lpEnv->dm.dmColor));
	DBMSG(("     dmDuplex: %d\n", lpEnv->dm.dmDuplex));
	DBMSG(("     iScale: %d\n", lpEnv->iScale));

}


#endif
