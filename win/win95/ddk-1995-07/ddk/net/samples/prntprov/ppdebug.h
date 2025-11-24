/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPDEBUG.H
 *   
 * Handy debugging functions for MS Network print provider
 *
 */      

#include "assert.h"

//
// Severity levels for DBGMSG messages
//
#define DBG_LEV_FATAL   3
#define DBG_LEV_ERROR   2
#define DBG_LEV_WARN    1
#define DBG_LEV_VERBOSE 0

#define DBG_BREAK_OFF  99

#ifdef DEBUG

//
// Prototypes for debugging functions
//
void cdecl DbgMsg(LPSTR MsgFormat, ... );

void DbgSetLevel(int);
int DbgGetLevel();

void DbgSetBreakLevel(int);
int DbgGetBreakLevel();

//
// Debug printf macro borrowed from ccteng. Double braces are needed 
// for this one, e.g.:
//
//     DBGMSG( DBG_LEV_ERROR, ( "Error code %d", Error ) );
//
// This is because we can't use variable parameter lists in macros.
// The statement gets pre-processed to a semi-colon in non-debug mode.
// Call the function DbgSetLevel from the debugger to set debugging
// options.
//
// The function DbgSetBreakLevel allows you to set a debug level at
// which DBGMSG will cause a break with int 3 after displaying its
// message.  By default, the debug break level is set to DBG_BREAK_OFF
// (never break). 
//
//
#define DBGMSG(Level,MsgAndArgs) \
{    \
  if (Level >= DbgGetLevel()) DbgMsg MsgAndArgs;\
  if (Level >= DbgGetBreakLevel()) _asm {int 3}\
}

#else
//
// If DEBUG is not defined, no debug code is generated
//

#define DBGMSG(a,b)

#endif



