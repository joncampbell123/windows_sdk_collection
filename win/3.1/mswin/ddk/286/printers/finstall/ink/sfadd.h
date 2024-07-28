/**[f******************************************************************
* sfadd.h -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
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
  
HANDLE   FAR   PASCAL   AddFontsMode(HWND, HANDLE, WORD, WORD FAR *, BOOL, BOOL);
HANDLE   FAR   PASCAL   EndAddFontsMode(HWND, HANDLE, HANDLE, WORD);
HANDLE   FAR   PASCAL   AddFonts(HWND, HANDLE, WORD, HANDLE, WORD, HANDLE, BOOL FAR *, LPSTR, LPSTR, WORD FAR *, LPSTR, BOOL);
BOOL     FAR   PASCAL   MergePath(LPSTR, LPSTR, int, BOOL);
BOOL     FAR   PASCAL   GetTargDir(HWND, HANDLE, LPSTR, int, LPSTR);
BOOL     FAR   PASCAL   GetBothDirs(HWND, HANDLE, LPSTR, LPSTR, int, int, LPSTR);
BOOL     FAR   PASCAL   GetTypDir(HWND, HANDLE, LPSTR, int, LPSTR);
BOOL     FAR   PASCAL   existLBL(HANDLE, int, LPSTR, int);
BOOL     FAR   PASCAL   bDestDlgFn(HWND, unsigned, WORD, LONG);
BOOL     FAR   PASCAL   tDestDlgFn(HWND, unsigned, WORD, LONG);
