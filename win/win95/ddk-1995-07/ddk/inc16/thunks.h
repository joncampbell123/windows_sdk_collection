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

/*****************************************************************************\
*                                                                             *
* thunks.h -  thunking functions, types, and definitions                      *
*                                                                             *
* Version 1.0								      *
*                                                                             *
* NOTE: windows.h must be #included first				      *
*                                                                             *
\*****************************************************************************/

#ifndef _INC_THUNKS
#define _INC_THUNKS

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif  /* RC_INVOKED */

#ifndef _INC_WINDOWS    /* Must include windows.h first */
#error windows.h must be included before thunks.h
#endif  /* _INC_WINDOWS */

DWORD  WINAPI  MapSL(DWORD);
DWORD  WINAPI  MapLS(DWORD);
VOID   WINAPI  UnMapLS(LPVOID);

#endif  /* _INC_THUNKS */
