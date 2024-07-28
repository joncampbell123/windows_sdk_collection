/**[f******************************************************************
* $ut.h
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
  
/* ut.h */
/* Copyright (C) Hewlett Packard, 1990. All rights reserved.
*/
/******************************************************************
*   Include file for Utility functions
******************************************************************/
/* History:
*
*    6-Jun-90 rcm  Created.
*/
  
//davew: Added from the local copy of port.h
typedef struct stat far *LPSTAT;
#define LSEEK_SET (int)0
#define LSEEK_CUR (int)1
#define LSEEK_END (int)2
  
extern LONG FAR PASCAL atol(LPSTR);
extern int FAR PASCAL atoi(LPSTR);
GLOBAL ULONG UTfileSize(LPSTR);
GLOBAL VOID UTgetString(LPSTR, LPSTR, WORD);
extern int FAR PASCAL myfstat(int, LPSTAT);
LONG FAR PASCAL myfilelength(int);
extern int FAR PASCAL lstrncmp(LPSTR, LPSTR, int);
extern int FAR PASCAL lstrncmpi(LPSTR, LPSTR, int);
extern VOID FAR PASCAL MyFreeMem(HANDLE FAR *);
extern BOOL FAR PASCAL unlink(LPSTR);
extern LPSTR FAR PASCAL lstrncat(LPSTR, LPSTR, int);
  
  
LPSTR       far PASCAL lmemcpy( LPSTR, LPSTR, WORD );
LPSTR       far PASCAL lmemset( LPSTR, BYTE, WORD );
  
int         far PASCAL lstrcmp( LPSTR, LPSTR );
LPSTR       far PASCAL lstrcpy( LPSTR, LPSTR );
LPSTR       far PASCAL lstrcat( LPSTR, LPSTR );
int         far PASCAL lstrlen( LPSTR );
LPSTR       far PASCAL lstrncpy( LPSTR, LPSTR, int );
int         far PASCAL lmemcmp( LPSTR, LPSTR, int );
VOID FAR PASCAL MyRewind(int);
LONG FAR PASCAL myftell(int);
int FAR PASCAL myaccess(LPSTR, int);
