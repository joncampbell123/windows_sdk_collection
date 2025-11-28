/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:	duel.h
 *  Content:	main include file
 *
 *
 ***************************************************************************/

#ifndef DUEL_INCLUDED
#define DUEL_INCLUDED

/* bcc32 does not support nameless unions in C mode */
#if defined(__BORLANDC__) && !defined(__cplusplus)
#define NONAMELESSUNION
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "resource.h"
#include <wtypes.h>
#include <tchar.h>

// Using DEBUG conditional flag but support _DEBUG
#if !defined(DEBUG) && defined(_DEBUG)
#define DEBUG
#endif

/*
 * Application messages
 */

#define UM_LAUNCH       WM_USER
#define UM_ABORT        WM_USER+1
#define UM_RESTARTTIMER WM_USER+2

// program states
enum
{
    PS_SPLASH,
    PS_ACTIVE,
    PS_REST
};

#define     MAX_SCREEN_X    639
#define     MAX_SCREEN_Y    479
#define     MAX_PLAYERNAME  50
#define		MAX_SESSIONNAME 50
#define		MAX_SPNAME		50
#define		MAX_CLASSNAME	50
#define		MAX_WINDOWTITLE 50
#define		MAX_ERRORMSG	256
#define		MAX_FONTNAME	50
#define		MAX_HELPMSG		512

#define     RECEIVE_TIMER_ID    1
#define     RECEIVE_TIMEOUT     1000    // in milliseconds

#define     ENUM_TIMER_ID    2
#define     ENUM_TIMEOUT     2000    // in milliseconds

// default window size
#define		MAX_DEFWIN_X	640
#define		MAX_DEFWIN_Y	480


// tree view image info
#define CX_BITMAP		25
#define CY_BITMAP		25
#define NUM_BITMAPS		 2

// registry info
#define DUEL_KEY (TEXT("Software\\Microsoft\\Duel"))

/*
 * fn prototypes
 */

BOOL	ShowError( int err );
void	UpdateTitle(void);

// Functions defined in util.c
int		StringFromGUID(GUID *lpguid, LPWSTR lpsz);
BOOL	IsEqualGuid(GUID *lpguid1, GUID *lpguid2);
HRESULT GUIDFromString(LPWSTR lpWStr, GUID *pGuid);
int		WideToAnsi(LPSTR lpStr,LPWSTR lpWStr,int cchStr);
int		AnsiToWide(LPWSTR lpWStr,LPSTR lpStr,int cchWStr);
int		randInt( int low, int high );
double	randDouble( double low, double high );

// diagnostic trace	(in util.c)
#define TRACE	dtrace 
void dtrace(LPTSTR szFormat, ...);

#endif


