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

// ------------------------------------------------------------
// WINPRINT
#define IDS_BANNERTITLE1            521
#define IDS_BANNERTITLE2            522
#define IDS_BANNERJOB               523
#define IDS_BANNERUSER              524
#define IDS_BANNERDATE              525
#define IDS_BANNERSIMPLE            526
#define IDS_BANNERFULL              527
// ------------------------------------------------------------

// ------------------------------------------------------------
// WINPRINT
#define IDC_STANDBAN    600
#define RT_CLIPFILE     601
// ------------------------------------------------------------


// ---------------------------------------------------------------------
// EXTERN VARIABLES
// ---------------------------------------------------------------------

extern  HANDLE   hInst;


// ---------------------------------------------------------------------
// DEBUG STUFF
// ---------------------------------------------------------------------
#ifdef DEBUG

extern  DWORD SplDbgLevel;

VOID cdecl DbgMsg( LPSTR MsgFormat, ... );

/* These flags are not used as arguments to the DBGMSG macro.
 * You have to set the high word of the global variable to cause it to break.
 * It is ignored if used with DBGMSG.
 * (Here mainly for explanatory purposes.)
 */
#define DBG_BREAK_ON_WARNING    ( DBG_WARNING << 16 )
#define DBG_BREAK_ON_ERROR      ( DBG_ERROR << 16 )

#define DBG_NONE                0
#define DBG_INFO                1
#define DBG_TRACE               DBG_INFO
#define DBG_WARNING             2
#define DBG_ERROR               4

/* Double braces are needed for this one, e.g.:
 *
 *     DBGMSG( DBG_ERROR, ( "Error code %d", Error ) );
 *
 * This is because we can't use variable parameter lists in macros.
 * The statement gets pre-processed to a semi-colon in non-debug mode.
 *
 * Set the global variable GLOBAL_DEBUG_FLAGS via the debugger.
 * Setting the flag in the low word causes that level to be printed;
 * setting the high word causes a break into the debugger.
 * E.g. setting it to 0x00040006 will print out all warning and error
 * messages, and break on errors.
 */

#define DBGMSG( Level, MsgAndArgs ) {if (Level >= SplDbgLevel) {DbgMsg MsgAndArgs;}}
#define DBGBREAK() {DebugBreak();}
#define ASSERT( Expr, MsgAndArgs ) {if (!Expr) {DbgMsg MsgAndArgs; DebugBreak();}}
VOID SplInSem(VOID);
VOID SplOutSem(VOID);

#else

#define DBGMSG( Level, MsgAndArgs )
#define DBGBREAK()
#define ASSERT( Expr, MsgAndArgs )
#define SplInSem()
#define SplOutSem()

#endif

// ---------------------------------------------------------------------
// FUNCTION PROTOTYPE
// ---------------------------------------------------------------------

#define AllocSplMem(a)      LocalAlloc( LPTR, a )
#define FreeSplMem(a, b)    LocalFree( a )

LPVOID
ReallocSplMem(
   LPVOID lpOldMem,
   DWORD cbOld,
   DWORD cbNew
);

LPTSTR
AllocSplStr(
    LPTSTR lpStr
);

BOOL
FreeSplStr(
   LPTSTR lpStr
);

BOOL
ReallocSplStr(
   LPTSTR FAR *plpStr,
   LPTSTR lpStr
);


// ---------------------------------------------------------------------
// UNICODE TO ANSI MACRO
// ??? !!! we should get rid of these sooner or later
// ---------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef UNICODE

LPSTR
mystrstr(
    LPSTR cs,
    LPSTR ct
);

LPSTR
mystrrchr(
    LPSTR cs,
    char c
);

LPSTR
mystrchr(
    LPSTR cs,
    char c
);

int
mystrnicmp(
    LPSTR cs,
    LPSTR ct,
    int n
);

#define wcscat(a, b) lstrcat(a, b)
#define wcscmp(a, b) lstrcmp(a, b)
#define wcscpy(a, b) lstrcpy(a, b)
#define wcslen(a) lstrlen(a)

#undef wcsicmp
#define wcsicmp(a, b) lstrcmpi(a, b)

#define wcschr(a, b) mystrchr(a, b)
#define wcsrchr(a, b) mystrrchr(a, b)
// #define wcsncmp(a, b, c) strncmp(a, b, c)

#undef wcsnicmp
#define wcsnicmp(a, b, c) mystrnicmp(a, b, c)

#define wcsstr(a, b) mystrstr(a, b)

#endif // UNICODE // ccteng
