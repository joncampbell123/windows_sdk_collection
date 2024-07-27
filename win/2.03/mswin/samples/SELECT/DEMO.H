#define ID_ABOUT    100

#define IDM_BOX     101
#define IDM_BLOCK   102
#define IDM_RETAIN  103

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL DemoInit(HANDLE);
long FAR PASCAL DemoWndProc(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
