/*************************************************************\
* UTSamp.h: Header file for UTSamp sample		      *
*                                                             *
\*************************************************************/

/******* Menu Defines *******/

#define IDM_EXIT       1001
#define IDM_GETUSER    1002
#define IDM_GETMEM     1003
#define IDM_ABOUT      1004

#ifndef APIENTRY /* to prevent problems w/ 16 bit compilers  */
#define APIENTRY /* and the function prototypes.	     */
#endif

/*** Function Prototypes ****/

DWORD APIENTRY MyGetFreeSpace(void);
DWORD APIENTRY MyWNetGetUser(LPSTR lpszLocalName, LPSTR lpszUserName, LPDWORD lpcchBuffer);

/* constants for dispatcher */

#define MYGETFREESPACE 0
#define MYWNETGETUSER  1

#ifdef W32SUT_16

/* define for WNetGetUser, documented in Windows DDK */

WORD FAR PASCAL WNetGetUser( LPSTR szUser, WORD FAR *nBufferSize);

#endif
