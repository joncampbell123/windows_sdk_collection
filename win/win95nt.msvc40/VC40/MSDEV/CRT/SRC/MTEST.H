/***
*mtest.h - Multi-thread testing include file
*
*       Copyright (c) 1988-1995, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This source contains prototypes and definitions used for multi-thread
*       testing.  In order to use the debug flavor of these routines, you
*       MUST link special debug versions of multi-thread crt0dat.obj and
*       mlock.obj into your program.  In addition, mtest.obj contains the
*       routines prototyped in this include file.
*
*       [NOTE:  This source module is NOT included in the C runtime library;
*       it is used only for testing.]
*
*       [Internal]
*
****/

#ifndef _INC_MTEST
#define _INC_MTEST

#ifndef _CRTBLD
/*
 * This is an internal C runtime header file. It is used when building
 * the C runtimes only. It is not to be used as a public header file.
 */
#error ERROR: Use of C runtime library internal header file.
#endif  /* _CRTBLD */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <cruntime.h>

/* Maximum thread count that mtest.c can handle */
#define _THREADMAX_  256

/* Define thread loop count for mtest.c optional code path */
#define _THREADLOOPCNT_  5

/* sleep macro */
#define _SLEEP_(l)      DOS32SLEEP(l)

#ifdef _DEBUG
int printlock(int locknum);
int print_single_locks(void);
int print_stdio_locks(void);
int print_lowio_locks(void);
int print_iolocks(void);
int print_locks(void);
#endif  /* _DEBUG */

void __cdecl _print_tiddata(int);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _INC_MTEST */
