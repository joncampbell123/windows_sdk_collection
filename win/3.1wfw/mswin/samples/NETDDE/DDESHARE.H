/*****************************************************************************\
*                                                                             *
* ddeshare.h -  WFWRK Network DDE Share Manager includes                      *
*                                                                             *
*               Version 3.1                                                   *
*                                                                             *
*        Copyright (c) 1992-1993, Microsoft Corp.  All rights reserved.       *
*                                                                             *
\*****************************************************************************/

/* menu ids */
#define    IDM_ADDSHARE        100
#define    IDM_DELETE          101
#define    IDM_PROPERTIES      104
#define    IDM_REFRESH         106
#define    IDM_EXIT            107
#define    IDM_ABOUT           108

/* resource IDs */
#define    ID_ICON             300
#define    ID_ACCS             301

/* control if of listbox */
#define IDLISTBOX    1000

/* external functions */
extern int PASCAL WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
extern BOOL InitApplication(HINSTANCE);
extern BOOL InitInstance(HINSTANCE, int);
extern long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
