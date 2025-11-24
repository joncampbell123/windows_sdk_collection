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

/* EXPORTS.c: 

	Contains all exports for DLL.	Also contains all INI file strings
	that are external in other source files.<nl>

	Routines contained in module:<nl>
		<l ConfigDialog.ConfigDialog><nl>
		<l CalbDialog.CalbDialog><nl>
*/

#include <windows.h>
#include "..\inc\penui.h"

#ifdef DEBUG
const char gszDllName[]="PenUI.Dll";
#endif

// global variables that will be used for all files
HWND 	ghMainDlg;
HDRVR  	ghPenDriver;	// handle to pen driver that DLL can use
HANDLE 	ghInst;		// file global variables


int CALLBACK LibMain(
	HANDLE	hInstance,
	WORD 	wDataSeg,
	WORD 	cbHeapSize,
	LPSTR 	lpszCmdLine)
	{
   ghInst = hInstance;
   return 1;
	}


int CALLBACK WEP(
	int	nExitType)
	{
	return 1;
	}


DWORD FAR PASCAL _loadds _export ConfigDialog(HDRVR hDriver, HANDLE hParent )
	{
	FARPROC 	lpfnCDlgProc;

	ghPenDriver=hDriver;

	DBGTRACE(hParent, "ConfigDialog: Entering");

	// Load the appropriate Configure dialog for the appropriate windows
	// version.
	if(hParent == NULL)
		hParent=GetFocus();

	lpfnCDlgProc = MakeProcInstance((FARPROC)ConfigDlgProc, (HINSTANCE)ghInst);

	DialogBox((HINSTANCE)ghInst, (LPCSTR)"PenConfigDlg", (HWND)hParent,
		(DLGPROC)lpfnCDlgProc);

	FreeProcInstance(lpfnCDlgProc);
	DBGTRACE(hParent,"ConfigDialog: Leaving");

	return 1L;
	}

//End-Of-File
