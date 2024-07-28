/**[f******************************************************************
* $utils.h
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

extern long FAR PASCAL labdivc(long, long, long);
extern long FAR PASCAL lmul(long, long);
extern long FAR PASCAL ldiv(long, long);
#ifdef nodef
extern long FAR PASCAL convert(long, LPSTR);
#endif
extern LPSTR FAR PASCAL itoa(int, LPSTR);
extern int FAR PASCAL atoi(LPSTR);
