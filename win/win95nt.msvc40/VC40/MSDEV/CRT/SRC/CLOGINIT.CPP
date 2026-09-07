/***
*cloginit.cpp - definitions and initialization for predefined stream clog.
*
*       Copyright (c) 1991-1995, Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Definitions and initialization of predefined stream clog.
*
*******************************************************************************/
#include <cruntime.h>
#include <internal.h>
#include <iostream.h>
#include <fstream.h>
#include <dbgint.h>
#pragma hdrstop

// put contructors in special MS-specific XIFM segment
#pragma warning(disable:4074)   // ignore init_seg warning
#pragma init_seg(compiler)

#if (defined (DLL_FOR_WIN32S))
ostream_withassign clog;
#else  /* (defined (DLL_FOR_WIN32S)) */
ostream_withassign clog(_new_crt filebuf(2));
#endif  /* (defined (DLL_FOR_WIN32S)) */

static Iostream_init  __InitClog(clog);

