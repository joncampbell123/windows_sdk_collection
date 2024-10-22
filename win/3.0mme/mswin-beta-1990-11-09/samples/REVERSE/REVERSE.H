/* 
 * WAVEEDIT.H
 * 
 * WaveEdit wrkbench/mediaman application global header file.  Defines
 * global variables, public procedures, and media IDs.
 * 
 * 
 */

/*
 *  GLOBAL VARIABLES
 */
extern	char	szAppName[];
extern	char	szSampleName[];
extern	HANDLE	hInstApp;
extern	HWND	hwndApp;

extern HWND	hwndName;
extern HWND	hwndPlay;
extern HWND	hwndQuit;

typedef char huge * HPSTR;

/*
 *  Initalization Routine for startup
 */
BOOL  AppInit(HANDLE hInst,HANDLE hPrev,WORD sw,LPSTR szCmdLine);

/*
 *  Main Window Procedure.
 */
LONG FAR PASCAL ReverseWndProc(HWND hwnd,unsigned msg,WORD wParam,LONG lParam);
/* Child window identifiers */
#define IDE_NAME		200
#define IDB_PLAY		201
#define IDB_QUIT		202


/* position and size info */
#define WMAIN_DX		207
#define WMAIN_DY		120

#define NAME_DX			180
#define NAME_DY			30
#define NAME_X			10
#define NAME_Y			10
#define PLAY_DX			85
#define PLAY_DY			35
#define PLAY_X			10
#define PLAY_Y			50
#define QUIT_DX			85
#define QUIT_DY			35
#define QUIT_X			105
#define QUIT_Y			50

