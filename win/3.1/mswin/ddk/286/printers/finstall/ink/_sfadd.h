/**[f******************************************************************
* $sfadd.h
*
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
  
int FAR PASCAL SSBoxDlgFn(HWND, unsigned, WORD, LONG);
int FAR PASCAL GetSourceDlgFn(HWND, unsigned, WORD, LONG);
int FAR PASCAL GetDestinationDlgFn(HWND, unsigned, WORD, LONG);
int FAR PASCAL SelectFontsDlgFn(HWND, unsigned, WORD, LONG);
HANDLE FAR PASCAL WriteSelections(HWND, LPSTR, LPSTR, WORD FAR *, HANDLE,HANDLE);
HANDLE FAR PASCAL ReadSourceFonts(HWND, LPSTR, int, LPSTR, HANDLE, LPWORD);
int FAR PASCAL AdFontsMode(HWND, HANDLE, WORD FAR *);
