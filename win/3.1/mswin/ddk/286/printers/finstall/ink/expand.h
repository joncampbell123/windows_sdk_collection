/**[f******************************************************************
* expand.h -
*
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
  
// Definitions for calls to functions in LZEXPAND.DLL.
  
#ifndef GLOBAL
#define GLOBAL extern
#endif
  
// external pointer definitions
// Pointers are initialized in SFINSTAL.C
//
// In the .C file where these pointers are allocated, put this statement:
//
//  #define GLOBAL /* nothing */
  
#ifndef NOOPENFILE
GLOBAL int (FAR PASCAL *lpOpenFile)(LPSTR, LPOFSTRUCT, WORD);
#endif
  
GLOBAL int (FAR PASCAL *lpInit)(int);
GLOBAL long (FAR PASCAL *lpSeek)(int, long, int);
GLOBAL int (FAR PASCAL *lpRead)(int, LPSTR, int);
GLOBAL void (FAR PASCAL *lpClose)(int);
  
// macros
  
#ifndef NOOPENFILE
#define lzOpenFile(f, p, s) ((*lpOpenFile)(f, p, s))
#endif
  
#define lzInit(h)       ((*lpInit)(h))
#define lzSeek(n, l, s)     ((*lpSeek)(n, l, s))
#define lzRead(n, b, c)     ((*lpRead)(n, b, c))
#define lzClose(h)      ((*lpClose)(h))
  
