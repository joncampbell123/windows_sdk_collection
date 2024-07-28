/**[f******************************************************************
* message.c -
*
* Copyright (C) 1988,1989 Aldus Corporation.
* Copyright (C) 1988-1990 Microsoft Corporation.
* Copyright (C) 1989,1990, 1991 Hewlett-Packard Company.
* All rights reserved.
* Company confidential.
*
**f]*****************************************************************/
  
#define BUF_LEN 73                               /* BUG #655: increase for localization*/
  
  
/*  ErrorMsg
*
*  Dump out an error message to the user.
*/
static void ErrorMsg(LPDEVICE, WORD);
static void ErrorMsg(lpDevice, wID)
LPDEVICE lpDevice;
WORD wID;
{
    char    capbuf[BUF_LEN];
    char    textbuf[BUF_LEN];
    extern  HANDLE hLibInst;
  
    LoadString(hLibInst, ERROR_BASE, (LPSTR) capbuf, BUF_LEN);
    LoadString(hLibInst, wID, (LPSTR) textbuf, BUF_LEN);
    MessageBox(NULL,textbuf,capbuf,MB_OK);
}
  
  
/*  WarningMsg
*
*  Dump out a warning message.
*/
static void WarningMsg(LPDEVICE, WORD);
static void WarningMsg(lpDevice, wID)
LPDEVICE lpDevice;
WORD wID;
{
    char    capbuf[BUF_LEN];
    char    textbuf[BUF_LEN];
    extern  HANDLE hLibInst;
  
    LoadString(hLibInst, WARNING_BASE, (LPSTR) capbuf, BUF_LEN);
    LoadString(hLibInst, wID, (LPSTR) textbuf, BUF_LEN);
    MessageBox(NULL,textbuf,capbuf,MB_OK);
}
