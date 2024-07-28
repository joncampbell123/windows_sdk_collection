/*
	palette.h
*/

#define IDS_APPNAME 	1		/* application (and class) name */

/* general stuff */

#define PIXELS      10          /* pixel size for each box */


/* macros */

#define EXPORT FAR PASCAL


/* extern declarations for various modules */

/* main module */

extern HANDLE hInst;			/* instance handle */
extern char szAppName[];		/* app name */
extern HWND hMainWnd;			/* handle to main window */

extern long EXPORT MainWndProc(HWND, UINT , WPARAM, LPARAM);
extern void Paint(HWND hWnd, HDC hDC);

/* Init.c */

extern BOOL InitFirstInstance(HANDLE);
extern int Error(LPSTR msg);
extern BOOL bHasPalette(HWND hWnd);
