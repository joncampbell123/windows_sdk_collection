/*---------------------------------------------------------------------------*\
|   PROGRESS.H								      |
|                                                                             |
|                                                                             |
\*---------------------------------------------------------------------------*/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#define DLG_PROGRESS 100
#define ID_BAR	     100
#define PRO_CLASS    "PRO"

#define ID_STATUS1   10
#define ID_STATUS2   11
#define ID_STATUS3   12
#define ID_STATUS4   13



#ifndef RC_INVOKED
    BOOL FAR PASCAL _export ProDlgProc(HWND, unsigned, WORD, LONG);

    BOOL FAR PASCAL ProInit(HANDLE,HANDLE);
    void FAR PASCAL ProClear(HWND hDlg);
    HWND FAR PASCAL ProOpen(HWND,int);
    BOOL FAR PASCAL ProClose(void);
    BOOL FAR PASCAL ProSetBarRange(int);
    BOOL FAR PASCAL ProSetBarPos(int);
    BOOL FAR PASCAL ProDeltaPos(int);
    BOOL FAR PASCAL ProSetText (int,LPSTR);
    BOOL FAR cdecl  ProPrintf (int,LPSTR,...);
#endif
    