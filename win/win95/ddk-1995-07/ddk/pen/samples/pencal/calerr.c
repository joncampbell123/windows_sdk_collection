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

/* CALERR.c: Calibrate Error handling

	Calibrate error handling.

		This module contains error/debug messages and asserts.
		It does little beyond providing correct interface to 
                shared code.

*/

/******************* Includes and Controlling Defines ***********************/
#include "cal.h"
#include "calerr.h"
#include "_caldlg.h"

/******************* Defines ************************************************/

/******************* Macros *************************************************/

/******************* Typedefs ***********************************************/

/******************* Variables **********************************************/
BOOL fWinOk = fTrue;
HWND vhDlgErrMsg = NULL;
char vszErrMsg[cchResMax];

/******************* Local prototypes ***************************************/
VOID PRIVATE MyUserErrMsg(UINT);

/******************* EXPORT FUNCTIONS ***************************************/
BOOL FAR PASCAL _loadds ErrMsgDlg(HWND, WORD, WORD, LONG);

/*+-------------------------------------------------------------------------*/
VOID CalError(
	WORD 	wErr)
	{
	fDrawCursor = TRUE;

	switch (wErr)
		{
		case CALERR_OPAQUE:
			MyUserErrMsg(rsOpaque);
			break;
		case CALERR_OLDPENDRIVER:
			MyUserErrMsg(rsOldPenDriver);
			break;
		case CALERR_CANTOPENPENDRIVER:
			MyUserErrMsg(rsCantOpenPenDriver);
			break;
		case CALERR_NOPEN:
			MyUserErrMsg(rsNoPen);
			break;
		case CALERR_UNUSUALINPUT:
			MyUserErrMsg(rsUnusualInput);
			break;
		default:
			break;
		}
	PostMessage(vhDlgCal, WM_CLOSE, NULL, (long) NULL);
	fDrawCursor = FALSE;
	}


VOID PRIVATE MyUserErrMsg(
	UINT	rs)
	{
	FARPROC lpfn = MakeProcInstance((FARPROC)ErrMsgDlg, vhInst);

	LoadString(vhInst, rs, (LPSTR)vszErrMsg, cchResMax);

	DialogBox(vhInst, (LPSTR)MAKEINTRESOURCE(iddErrMsg),
		(HWND)vhDlgCal, (DLGPROC)lpfn);
	FreeProcInstance(lpfn);
	}


BOOL FAR PASCAL _loadds ErrMsgDlg(
	HWND 	hDlg,
	WORD 	message,
	WORD 	wParam,
	LONG 	lParam)
	{
	switch (message)
		{
		case WM_INITDIALOG:
			BLOCK
				{
				RECT 	rectParent, rectMe;
				POINT pntCenter;
				int 	nWidthMe, nHeightMe;

				vhDlgErrMsg = hDlg;
				GetWindowRect(hDlg, (LPRECT) &rectMe);
				rectParent.left = rectParent.top = 0;
				rectParent.right  = GetSystemMetrics(SM_CXSCREEN);
				rectParent.bottom = GetSystemMetrics(SM_CYSCREEN);
				pntCenter.x = (rectParent.left + rectParent.right) / 2;
				pntCenter.y = (rectParent.top + rectParent.bottom) / 2;
				nWidthMe = rectMe.right - rectMe.left;
				nHeightMe = rectMe.bottom - rectMe.top;

				SetWindowPos(hDlg, NULL,
					pntCenter.x - (nWidthMe/2), pntCenter.y - (nHeightMe/2),
					nWidthMe, nHeightMe, SWP_NOZORDER);
				SetDlgItemText(hDlg, idcErrMsg, (LPSTR)vszErrMsg);
				}
			return TRUE;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDOK:
					PostMessage(vhDlgCal, WM_RefuseChanges, NULL, NULL);
					EndDialog(hDlg, 0);
					break;
				}
			return TRUE;
		case WM_CLOSE:
			EndDialog(vhDlgErrMsg, FALSE);
			vhDlgErrMsg = NULL;
			return TRUE;
		}
	return FALSE;
	}

