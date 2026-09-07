
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples.
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved.
*       This source code is only intended as a supplement to
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the
*       Microsoft samples programs.
\******************************************************************************/

#include "resource.h"

// Porting macro
#ifdef WIN32
#define LONG2POINT(l, pt)  ((pt).x = (SHORT)LOWORD(l), (pt).y = (SHORT)HIWORD(l))
#else
#define LONG2POINT(l, pt)  ((pt).x = (INT)LOWORD(l), (pt).y = (INT)HIWORD(l))
#endif

#ifdef _MAC
#ifdef APIENTRY
#undef APIENTRY
#endif
#define APIENTRY CALLBACK
#endif

/* Defines for top-level menu positions */
#define IDFILE_POS     0
#define IDCOLORS_POS   1
#define IDSTATES_POS   2
#define IDHELP_POS     3

#define MEASUREITEMWIDTH  40
#define MEASUREITEMHEIGHT 40

/* BitBlt Raster operation */
#define ROP_PSDPxax     0x00B8074AL

BOOL NEAR PASCAL MenuInit(HANDLE);
LONG APIENTRY MenuWndProc(HWND, UINT, UINT, LONG);
BOOL APIENTRY About(HWND, UINT, UINT, LONG);

