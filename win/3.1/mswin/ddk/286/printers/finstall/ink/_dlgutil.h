/**[f******************************************************************
* $dlgutils.h
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
  
int FAR PASCAL MyDialogBox(HANDLE,int,HWND,FARPROC);
  
void FAR PASCAL CenterDlg (HWND);
int FAR PASCAL GenericWndProc(HWND, unsigned, WORD, long);
int FAR PASCAL GetDirectoryWndProc(HWND, unsigned, WORD, long);
int FAR PASCAL AddDlgFn(HWND, unsigned, WORD, long);
VOID FAR PASCAL MyMultiSelect(HWND, unsigned, WORD, long);
int FAR PASCAL MyMessageBox(HWND, WORD, LPSTR, WORD);
  
#define DS_FOUND    0x0001
#define DS_FILES    0x0002
