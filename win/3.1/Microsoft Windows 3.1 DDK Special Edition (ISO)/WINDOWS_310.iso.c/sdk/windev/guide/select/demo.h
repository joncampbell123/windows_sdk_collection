
#define IDM_BOX     101
#define IDM_BLOCK   102
#define IDM_RETAIN  103
#define IDM_ABOUT   104

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL DemoInit(HANDLE);
long FAR PASCAL DemoWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
