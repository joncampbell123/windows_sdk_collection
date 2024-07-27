#define IDM_ABOUT 100

/* file menu items */

#define     IDM_NEW      101
#define     IDM_OPEN     102
#define     IDM_SAVE     103
#define     IDM_SAVEAS   104
#define     IDM_PRINT    105
#define     IDM_EXIT     106

/* edit menu items */

#define     IDM_UNDO     200
#define     IDM_CUT      201
#define     IDM_COPY     202
#define     IDM_PASTE    203
#define     IDM_CLEAR    204

/* child control i.d. */

#define     IDC_EDIT     300

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
