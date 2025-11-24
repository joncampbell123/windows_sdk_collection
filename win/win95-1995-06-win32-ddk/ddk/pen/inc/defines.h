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

// defines.h - 
// The make file will determine exactly what file it is that we're going to
// build.  If UNIDRV is defined then we will build in all the different driver
// commands and misc info.	Else, the particular hardware type should be
// defined.

#ifdef UNIDRV
#define WACOM2
#define WACOM4
#define CPQ
#endif

#ifdef CPQ
#ifndef BIOS
#define BIOS
#endif
#endif

// This is VxD code blah, blah, blah...
#define WIN31COMPAT
#define VxD
#define CCODE
#define SysVMIsSpecial
