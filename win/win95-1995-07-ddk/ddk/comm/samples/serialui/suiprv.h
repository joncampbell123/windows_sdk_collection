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

//
// suiprv.h:  Includes all files that are to be part of the precompiled
//             header.
//

#ifndef __SUIPRV_H__
#define __SUIPRV_H__

/////////////////////////////////////////////////////  INCLUDES

#define STRICT
#define NOWINDOWSX

#define USECOMM

#include <windows.h>        
#include <windowsx.h>
#include <winerror.h>

#pragma warning(disable:4005)
#define WINAPI      _loadds _far _pascal
#define CALLBACK    _loadds _far _pascal

#include <shellapi.h>       // for registration functions
#include <regstr.h>
#include <prsht.h>          // Property sheet stuff
#include <setupx.h>         // PnP setup/installer services

// Porting macros
#define ISVALIDHINSTANCE(hinst) ((UINT)hinst>=(UINT)HINSTANCE_ERROR)
#define LOCALOF(p)      ((HLOCAL)OFFSETOF(p))

#define DATASEG_READONLY    "_TEXT"
#define DATASEG_PERINSTANCE
#define DATASEG_SHARED

#define PUBLIC          FAR PASCAL
#define CPUBLIC         FAR _cdecl
#define PRIVATE         NEAR PASCAL

#define MAXBUFLEN       256
#define MAXMSGLEN       512
#define MAXMEDLEN       64
#define MAXSHORTLEN     32

#define NULL_CHAR       '\0'

// local includes
//
#include "mcx16.h"
#include "cstrings.h"       // Read-only string constants
#include "comm.h"           // Common functions
#include "err.h"            // Error/debug code

extern HINSTANCE g_hinst;

#ifndef HANDLE_WM_NOTIFY
/* LRESULT Cls_OnNotify(HWND hwnd, int idFrom, NMHDR FAR* pnmhdr); */
#define HANDLE_WM_NOTIFY(hwnd, wParam, lParam, fn) \
    (fn)((hwnd), (int)(wParam), (NMHDR FAR*)(lParam))
#endif

#endif  //!__SUIPRV_H__

