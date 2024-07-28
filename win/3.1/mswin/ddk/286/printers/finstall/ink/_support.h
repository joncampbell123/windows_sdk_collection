/**[f******************************************************************
* $support.h
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
  
#define COPY_SUCCESS    1  /* Files were the same or copy sucessfully completed      */
#define COPYERR_MEMORY -1  /* Unable to allocate or lock memory to do copy operation */
#define COPYERR_OPEN   -2  /* Unable to open source file                             */
#define COPYERR_CREATE -3  /* Unable to open destination file for writing            */
#define COPYERR_WRITE  -4  /* Error while writing destination file                   */
#define COPYERR_NOROOM -5  /* All bytes were not written                             */
/* ------------------------------------------------------ */
  
BOOL FAR PASCAL ConfigBullet(HWND, HANDLE, LPSTR, LPSTR, LPSTR, LPSTR, LPSTR);
int  FAR PASCAL QueryCopyFile(LPSTR, LPSTR, HWND, LPSTR);
BOOL FAR PASCAL unlink(LPSTR);
BOOL FAR PASCAL FileExist(LPSTR);
