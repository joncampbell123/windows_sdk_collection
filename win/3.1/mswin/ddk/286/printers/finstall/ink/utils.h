  
/*
 * $Header: 
 */

/*
 * $Log:
 */
  
/*
 * $Header: 
 */

/*
 * $Log:
 */
/**[f******************************************************************
* utils.h -
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
  
#include "tfmstruc.h"
  
extern long FAR PASCAL lmul(long, long);
extern long FAR PASCAL ldiv(long, long);
extern long FAR PASCAL lmod(long, long);
//extern unsigned long FAR PASCAL dmul(unsigned long, unsigned long);
//extern long FAR PASCAL ddiv(unsigned long, unsigned long);
extern long FAR PASCAL FontMEM(int, long, long);
extern int FAR PASCAL itoa(int, LPSTR);
extern int FAR PASCAL atoi(LPSTR);
extern int FAR PASCAL _lopenp(LPSTR, WORD);
extern void FAR PASCAL MakeAppName(LPSTR, LPSTR, LPSTR, int);
extern int FAR PASCAL myatoi(LPSTR);
extern long FAR PASCAL ltoa(long, LPSTR);
extern int FAR PASCAL lstrcpyn(LPSTR, LPSTR, int);
extern WORD FAR PASCAL duconvert(struct TFMType far *, int, short);
extern WORD FAR PASCAL fdiv5(long, long, long, long, long);
