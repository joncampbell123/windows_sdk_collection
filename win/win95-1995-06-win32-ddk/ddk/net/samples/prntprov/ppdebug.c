/*****************************************************************/ 
/**               Microsoft Windows 95                          **/
/**           Copyright (C) Microsoft Corp., 1991-1995          **/
/*****************************************************************/ 

/* PPDEBUG.C
 *   
 * Handy debugging functions for MS Network print provider
 *
 */      

#include "mspp.h"

#ifdef DEBUG

static nDebugLevel      = DBG_LEV_VERBOSE;   
static nDebugBreakLevel = DBG_BREAK_OFF;

/*****************************************************************
 DbgSetLevel

 Sets the current debug level.  

*****************************************************************/
void DbgSetLevel(int nLevel) {

  nDebugLevel = nLevel;

}

/*****************************************************************
 DbgGetLevel

 Returns the current debug level.

*****************************************************************/
int DbgGetLevel() {

  return nDebugLevel;

}

/*****************************************************************
 DbgSetBreakLevel

 Sets the current debug level.  

*****************************************************************/
void DbgSetBreakLevel(int nLevel) {

  nDebugBreakLevel = nLevel;

}

/*****************************************************************
 DbgGetBreakLevel

 Returns the current debug level.

*****************************************************************/
int DbgGetBreakLevel() {

  return nDebugBreakLevel;

}

/*****************************************************************
 DbgMsg
 
 Writes a string composed from a variable argument list to the
 debugging device.
*****************************************************************/
VOID cdecl DbgMsg(LPSTR MsgFormat, ... ) {
    char MsgText[256];

    wvsprintf(MsgText,MsgFormat,(LPSTR)(((LPSTR)(&MsgFormat))+sizeof(MsgFormat)) );
    lstrcat( MsgText, "\r");

    OutputDebugString( MsgText );
}

#endif       // #ifdef DEBUG

