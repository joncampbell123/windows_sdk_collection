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

/* CAL.h: Calibrate DLL header

	General header for pencal.dll
*/

#ifndef _INCLUDE_CALH
#define _INCLUDE_CALH

/******************* Includes ***********************************************/
#include <windows.h>
#include <penwin.h>
#include "res.h"

/******************* Defines ************************************************/
#define STRICT
#define REGISTRY
#define PRIVATE		NEAR PASCAL
#define CODECONST
#define DLGFN			
#define NOREF
#define Unref(var)	var;

#ifndef RC_INVOKED
#define BLOCK
#endif

/******************* Macros *************************************************/
#define cchResMax		128
#define cbSzRcMax		128
#define STR_MAX_LEN	(256)

#define fTrue			1
#define fFalse			0

#ifdef DEBUG
#define DbgS(lpstr)	OutputDebugString((LPCSTR)lpstr)
#else
#define DbgS(lpstr)	;
#endif

/******************* Typedefs ***********************************************/

/******************* Externals **********************************************/
extern HANDLE vhInst;
extern char szAppName[];
extern HWND vhDlgCal;
extern BOOL fDrawCursor;

/******************* Public Prototypes **************************************/

/******************* Export Prototypes **************************************/
BOOL FAR PASCAL _loadds CalDlg(HWND hDlg, WORD wMessage, WORD wParam, LONG lParam);

#endif // _INCLUDE_CALH
