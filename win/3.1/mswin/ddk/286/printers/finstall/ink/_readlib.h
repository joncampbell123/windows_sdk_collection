/**[f******************************************************************
* $readlib.h
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
  
  
HANDLE FAR PASCAL ReadInstalledPrinterFonts(HWND, int, LPSTR, HANDLE, WORD FAR *);
BOOL FAR PASCAL SameFilename(LPSTR, LPSTR);
LPSTR FAR PASCAL LastPartOfFilename(LPSTR);
