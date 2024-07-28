/****************************************************************************
 * debug.h
 *
 * this module contains macros for printing debugging information.
 * define the appropriate flags to get the info that you want.
 *
 * DEBUG_ON	turn on basic debugging msgs
 * DEBUG1_ON
 * DEBUG_ON3
 * DEBUG_ON4
 * DEBUG_STRIPCOLON
 * DEBUG_RC
 * DEBUG_PARAMS
 * DEBUG_CONTROL
 * DEBUG_ATalk
 * DEBUG_LISTBOX
 * DEBUG3_WRITEPROFILE
 * DEBUG_ONface
 * DEBUG_ONmap
 *
 * ACHECK	assertion checking on
 *
 ****************************************************************************/

int FAR cdecl WinPrintf(LPSTR,...);

#ifdef DEBUG_ON
#define DBMSG(msg) WinPrintf("%C"); WinPrintf msg
#else
#define DBMSG(msg)
#endif


#ifdef DEBUG1_ON
#define DBMSG1(msg) WinPrintf("%C"); WinPrintf msg
#else
#define DBMSG1(msg)
#endif

/*
 * DEBUG_ON3
 */

#ifdef DEBUG_ON3
#define DBMSG3(msg) WinPrintf msg
#else
#define DBMSG3(msg)
#endif

#ifdef DEBUG_ON4
#define DBMSG4(msg) WinPrintf msg
#else
#define DBMSG4(msg)
#endif


#ifdef DEBUG_RC
#define DBMSG2(msg) WinPrintf("%C"); WinPrintf msg
#else
#define DBMSG2(msg)
#endif

#ifdef DEBUG_STRIPCOLON
#define DB_STRIPCOLON(msg) WinPrintf msg
#else
#define DB_STRIPCOLON(msg)
#endif

#ifdef DEBUG_PARAMS
#define DBPARAMS(msg) WinPrintf("%C"); WinPrintf msg
#else
#define DBPARAMS(msg)
#endif

#ifdef DEBUG_CONTROL
#define DBMSG2(msg) WinPrintf msg
#else
#define DBMSG2(msg)
#endif

#ifdef DEBUG_ATalk
#define DBG_ATalk(msg) WinPrintf msg
#else
#define DBG_ATalk(msg)
#endif


#ifdef DEBUG_LISTBOX
#define DBMSG_LB(msg) WinPrintf msg
#else
#define DBMSG_LB(msg)
#endif

#ifdef DEBUG3_WRITEPROFILE
#define DBMSG_WP(msg) WinPrintf msg
#else
#define DBMSG_WP(msg)
#endif

#ifdef DEBUG_ONface
#define DBGface(msg) WinPrintf("%C"); WinPrintf msg
#else
#define DBGface(msg)
#endif

#ifdef DEBUG_ONmap
#define DBGmap(msg) WinPrintf msg
#else
#define DBGmap(msg)
#endif


/*
 * ACHECK
 *
 */

#ifdef ACHECK
#define ASSERT(p) if(!(p)) WinPrintf(" ASSERTion failed: %s\n", (LPSTR)#p);
#else
#define ASSERT(p)
#endif



