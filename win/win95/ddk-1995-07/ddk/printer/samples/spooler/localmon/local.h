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

/*++

    local.h

--*/


// ---------------------------------------------------------------------
// EXTERN VARIABLES
// ---------------------------------------------------------------------
extern  TCHAR FAR *szRegistryConfig;
extern  TCHAR FAR *szDefault;
extern  TCHAR FAR *szRegistryProvidors;
extern  TCHAR FAR *szRegistryPrinters;
extern  TCHAR FAR *szRegistryMonitors;
extern  TCHAR FAR *szRegistryEnvironments;
extern  TCHAR FAR *szPrinterData;
extern  TCHAR FAR *szDriverVersion;
extern  TCHAR FAR *szDriversKey;
extern  TCHAR FAR *szPrintProcKey;
extern  TCHAR FAR *szPrintersKey;
extern  TCHAR FAR *szDirectory;
extern  TCHAR FAR *szDriverFile;
extern  TCHAR FAR *szConfigurationKey;
extern  TCHAR FAR *szDataFileKey;
extern  TCHAR FAR *szHelpFile;
extern  TCHAR FAR *szAllShadows;
extern  TCHAR FAR *szComma;
extern  TCHAR FAR *szName;
extern  TCHAR FAR *szShare;
extern  TCHAR FAR *szPorts;
extern  TCHAR FAR *szMonitor;
extern  TCHAR FAR *szPrintProcessor;
extern  TCHAR FAR *szDriver;
extern  TCHAR FAR *szLocation;
extern  TCHAR FAR *szDescription;
extern  TCHAR FAR *szAttributes;
extern  TCHAR FAR *szStatus;
extern  TCHAR FAR *szPriority;
extern  TCHAR FAR *szUntilTime;
extern  TCHAR FAR *szStartTime;
extern  TCHAR FAR *szParameters;
extern  TCHAR FAR *szSepFile;
extern  TCHAR FAR *szDevMode;
extern  TCHAR FAR *szDatatype;
extern  TCHAR FAR *szDependentFiles;
extern  TCHAR FAR *szDNSTimeout;
extern  TCHAR FAR *szTXTimeout;


extern  TCHAR FAR *szPrintProvidorName;
extern  TCHAR FAR *szPrintProvidorDescription;
extern  TCHAR FAR *szPrintProvidorComment;

extern  TCHAR FAR *szRemoteDoc;
extern  TCHAR FAR *szDriverDir;
extern  TCHAR FAR *szPrintProcDir;
extern  TCHAR FAR *szPrinterDir;

extern  TCHAR FAR *szNull;

// datatypes
extern  TCHAR FAR *szRAW;
extern  TCHAR FAR *szEMF;

extern  HANDLE   hInst;


// ---------------------------------------------------------------------
// MACRO
// ---------------------------------------------------------------------
#define ONEDAY  60*24
#define offsetof(type, identifier) (DWORD)(&(((type)0)->identifier))

#define DEFAULT_DNS_TIMEOUT     15000
#define DEFAULT_TX_TIMEOUT      45000

#ifndef NNLEN
#define NNLEN 12    // max length for a printer share name
#endif


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


VOID cdecl LogEvent( // ccteng add cdecl
    WORD   EventType,
    DWORD  EventID,
    LPTSTR pFirstString,
    ...
);





PINIPORT
FindPortFromList(
    PINIENTRY pIniEntry,
    LPSTR pName
);

LPBYTE
PackStrings(
   LPTSTR *pSource,
   LPBYTE pDest,
   DWORD  *DestOffsets,
   LPBYTE pEnd
);

int
cdecl
Message(
    HWND hwnd,
    DWORD Type,
    int CaptionID,
    int TextID,
    ...
);



PINIENTRY
FindIniKey(
   PINIENTRY pIniEntry,
   LPTSTR lpName
);




#if 1
BOOL
CheckLptOrCom(
    LPSTR lpName,
    DWORD dwFlag
);

#define CHECK_LPT_ONLY      0x00000001
#define CHECK_COM_ONLY      0x00000002
#define CHECK_LPT_AND_COM   0x00000003

#define IsLptOrCom(lpName)  CheckLptOrCom(lpName, CHECK_LPT_AND_COM)
#define IsLpt(lpName)       CheckLptOrCom(lpName, CHECK_LPT_ONLY)
#define IsCom(lpName)       CheckLptOrCom(lpName, CHECK_COM_ONLY)
#else
BOOL IsLptOrCom(LPSTR);
BOOL IsLpt(LPSTR);
BOOL IsCom(LPSTR);
#endif

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

#endif // UNICODE //
