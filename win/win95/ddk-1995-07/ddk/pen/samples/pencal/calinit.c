/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/* CALINIT.c: Calibrate Initialization handling

	Calibrate initialization handling.

		This module contains calibration initialization routines.

*/

/******************* Includes and Controlling Defines ***********************/
#include "cal.h"
#include "calerr.h"

/******************* Defines ************************************************/

/******************* Macros *************************************************/

/******************* Typedefs ***********************************************/

/******************* Variables **********************************************/
HANDLE 		vhInst = 0;				// current instance handle
char 		szAppName[cbSzRcMax];
HWND 		hwndControlPanel = NULL;

char 		szHelpMessageName[] = "ShellHelp";	// name of registered help message
WORD 		wHelpMessage = WM_NULL;			// registered message for help
char 		szHelpFile[] = "CONTROL.HLP";		// help file name

/******************* Local prototypes ***************************************/
BOOL PRIVATE FInitCalibrate(BOOL);

/******************* EXPORT FUNCTIONS ***************************************/
BOOL FAR 	PASCAL LibMain(HANDLE, WORD, WORD, LPSTR);
WORD WINAPI WEP(int);
BOOL FAR 	PASCAL _loadds _export Calibrate(HWND);

/*+-------------------------------------------------------------------------*/
BOOL FAR PASCAL LibMain(
	HANDLE 	hInstance,
	WORD 		segDgroup,
	WORD 		cbHeapSize,
	LPSTR 	lpszCmdLine)
	{
	// Avoid warnings on unused (but required) formal parameters.
	segDgroup   = segDgroup;
	cbHeapSize  = cbHeapSize;
	lpszCmdLine = lpszCmdLine;

	vhInst = hInstance;
	return FInitCalibrate(TRUE);
	}


/*+-------------------------------------------------------------------------*/
WORD WINAPI WEP(int nParam)
	{
	Unref(nParam);
	return 1;
	}


/*+-------------------------------------------------------------------------*/
BOOL PRIVATE FInitCalibrate(
	BOOL fInit)
	{
	if (GetSystemMetrics(SM_PENWINDOWS) == NULL)
		return FALSE;

	LoadString(vhInst, idsAppName, szAppName, cbSzRcMax);
	return TRUE;
	}


/*+-------------------------------------------------------------------------*/
BOOL FAR PASCAL _loadds _export Calibrate(
	HWND	hwndCpl)
	{
	int		iRet;
	FARPROC 	lpfn = MakeProcInstance((FARPROC)CalDlg, vhInst);

	if ((hwndCpl == (HWND)NULL) || (lpfn == (FARPROC)NULL))
		return (FALSE);	// Some Message?

	iRet = DialogBox(vhInst, MAKEINTRESOURCE(iddCal), hwndCpl, (DLGPROC)lpfn);

	if (iRet == -1)
		return (FALSE);	// Some Message?

	FreeProcInstance(lpfn);
	return (TRUE);
	}
