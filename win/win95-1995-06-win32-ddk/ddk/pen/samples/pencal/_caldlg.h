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

/* _CALDLG.h: Calibrate DLL dialog header

	General internal header for pencal.dll
*/

#ifndef _INCLUDE_CALDLGH
#define _INCLUDE_CALDLGH

/******************* Includes ***********************************************/

/******************* Defines ************************************************/
/******************* Macros *************************************************/
// arbitrary number of pixels from side of window at which to put crosses
#define dxOffset				75
#define dyOffset				75
#define dxLine					20
#define dyLine					20

// private windows messages
#define WM_RunCalibrate 	WM_USER+1
#define WM_AcceptChanges 	WM_USER+2
#define WM_RefuseChanges 	WM_USER+3

// length of instructions text
#define INST_STR_MAX_LEN 	(3*STR_MAX_LEN)

/******************* Typedefs ***********************************************/

/******************* Externals **********************************************/

/******************* Public Prototypes **************************************/

/******************* Export Prototypes **************************************/
VOID PRIVATE RepaintMe(HWND);
VOID PRIVATE HandlePenDown(HWND, WORD, LONG);
VOID PRIVATE AcceptChanges(VOID);

BOOL FAR PASCAL _loadds VerifyChangesDlg(HWND, WORD, WORD, LONG);

#endif // _INCLUDE_CALDLGH
