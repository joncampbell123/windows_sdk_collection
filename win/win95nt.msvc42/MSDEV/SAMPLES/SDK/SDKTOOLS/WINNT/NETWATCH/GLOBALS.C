/*
 *  globals.c
 *  
 *  Purpose:
 *      globals
 *  
 *  Owner:
 *      MikeSart
 */
#define UNICODE     1

#include <windows.h>
#include "rcids.h"


TCHAR       *szAppName;             // appname from STRINGTABLE
TCHAR       *szServerName   = NULL; // server we be viewing
DWORD       dwTimerInterval = 30000;// timer interval
TCHAR       szNil[]         = TEXT("");
TCHAR       szFmtNum[]      = TEXT("%lu");

DWORD       dwNumUsers      = 0;    // number of users connected to server

// showhidden, showinuse, showfiles
UINT        unMenuFlags[3]  = { MF_UNCHECKED, MF_UNCHECKED, MF_UNCHECKED };

TCHAR       szBuffer[512];
HWND        hwndMain;
HMENU       ghMenu;
HINSTANCE   ghInst;
