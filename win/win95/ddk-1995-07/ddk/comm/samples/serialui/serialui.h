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

//---------------------------------------------------------------------------
//
// File: serialui.h
//
// This files contains the shared prototypes and macros.
//
//---------------------------------------------------------------------------


#ifndef __SERIALUI_H__
#define __SERIALUI_H__

#define MAXPORTNAME     13
#define MAXFRIENDLYNAME LINE_LEN        // LINE_LEN is defined in setupx.h

// Internal structure shared between port property pages.
//
typedef struct _PORTINFO
    {
    UINT            uFlags;             // One of SIF_* values
    WIN32DCB        dcb;
    LPCOMMCONFIG    pcc;                // Read-only
    int             idRet;

    char            szFriendlyName[MAXFRIENDLYNAME];
    } PortInfo, FAR * LPPORTINFO;

// PortInfo Flags
#define SIF_FROM_DEVMGR         0x0001


//-------------------------------------------------------------------------
//  PORT.C
//-------------------------------------------------------------------------

BOOL CALLBACK Port_WrapperProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#endif // __SERIALUI_H__

