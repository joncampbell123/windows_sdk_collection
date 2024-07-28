/**[f******************************************************************
* neededh.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
/*  NEEDEDH.H
*
*  This file contains the needed header file information that is
*  going to differ based upon which application the soft font installer
*  is being compiled with.
*/
// history
//
// 19 sep 89    peterbe comment-out _llseek() etc.
  
typedef unsigned short UWORD;
typedef UWORD far * LPUWORD;
  
  
#include "utils.h"
//LONG far PASCAL _llseek( int, long, int );
//WORD far PASCAL _lread( int, LPSTR, int );
//WORD far PASCAL _lwrite( int, LPSTR, int );
//void far PASCAL _lclose( int );
  
#include "debug.h"
  
LPSTR FAR PASCAL lmemcpy(LPSTR, LPSTR, WORD);
LPSTR FAR PASCAL lmemset(LPSTR, BYTE, WORD);
//LPSTR FAR PASCAL lstrcpy(LPSTR, LPSTR);
//LPSTR FAR PASCAL lstrcat( LPSTR, LPSTR );
//int FAR PASCAL lstrcmp(LPSTR, LPSTR);
//int FAR PASCAL lstrlen(LPSTR);
