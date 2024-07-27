/* file menu items */

#define     IDM_NEW      100
#define     IDM_OPEN     101
#define     IDM_SAVE     102
#define     IDM_SAVEAS   103
#define     IDM_PRINT    104
#define     IDM_EXIT     105
#define     IDM_ABOUT    106

/* edit menu items */

#define     IDM_UNDO     200
#define     IDM_CUT      201
#define     IDM_COPY     202
#define     IDM_PASTE    203
#define     IDM_CLEAR    204

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL OutputInit(HANDLE);
long FAR PASCAL EditMenuWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
