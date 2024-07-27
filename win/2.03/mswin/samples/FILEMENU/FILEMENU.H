/* file menu items */

#define     IDM_NEW      100
#define     IDM_OPEN     101
#define     IDM_SAVE     102
#define     IDM_SAVEAS   103
#define     IDM_PRINT    104
#define     IDM_EXIT     105
#define     IDM_ABOUT    106

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL OutputInit(HANDLE);
long FAR PASCAL FileMenuWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
