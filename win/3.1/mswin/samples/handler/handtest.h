
#define IDM_GET     101
#define IDM_SET     102
#define IDM_RESTORE 103
#define IDM_ABOUT   104

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL DemoInit(HANDLE);
long FAR PASCAL DemoWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

