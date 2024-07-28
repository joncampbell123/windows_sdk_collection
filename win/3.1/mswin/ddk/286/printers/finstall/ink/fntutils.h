/**[f******************************************************************
* fntutils.h -
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
  
HANDLE FAR PASCAL InitWinSF(LPSTR);
int FAR PASCAL NextWinSF(HANDLE, LPSTR, WORD);
int FAR PASCAL EndWinSF(HANDLE);
int FAR PASCAL NextWinCart(HANDLE, LPSTR, WORD);
int FAR PASCAL GetCartName(LPSTR,LPSTR, WORD);
