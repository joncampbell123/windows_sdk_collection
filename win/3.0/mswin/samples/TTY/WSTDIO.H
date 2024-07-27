/* Header file for use with WSTDIO.c */

BOOL stdioInit(HANDLE);
HWND wopen(HWND,BOOL);
HWND CreateStdioWindow(LPSTR,DWORD,int,int,int,int,HWND,HANDLE,BOOL);
BOOL wputs(LPSTR);
long FAR PASCAL StdioWndProc(HWND,unsigned,WORD,LONG);
