/**[f******************************************************************
* neededh.h -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.  Company confidential.
*
**f]*****************************************************************/
  
/*  NEEDEDH.H
*
*  This file contains the needed header file information that is
*  going to differ based upon which application the soft font installer
*  is being compiled with.
*/
  
#ifdef SF_TRANSMIN
#define SFINSTAL_NEEDEDH
#include "device.h"
#endif
  
#ifdef SF_UTILS
#define NO_OUTUTIL
#include "utils.h"
LONG far PASCAL _llseek( int, long, int );
WORD far PASCAL _lread( int, LPSTR, int );
WORD far PASCAL _lwrite( int, LPSTR, int );
void far PASCAL _lclose( int );
#endif
#include "debug.h"
  
/* TETRA -- added lstrncpy() declaration -- KLO */
LPSTR FAR PASCAL lstrncpy(LPSTR, LPSTR, WORD);
LPSTR FAR PASCAL lmemcpy(LPSTR, LPSTR, WORD);
LPSTR FAR PASCAL lmemset(LPSTR, BYTE, WORD);
LPSTR FAR PASCAL lstrcpy(LPSTR, LPSTR);
LPSTR FAR PASCAL lstrcat( LPSTR, LPSTR );
int FAR PASCAL lstrcmp(LPSTR, LPSTR);
int FAR PASCAL lstrlen(LPSTR);
