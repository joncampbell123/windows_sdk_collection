#define ID_ABOUT 100

extern HANDLE hInst;                          /* Stores the current instance */

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
long FAR PASCAL MemoryWndProc(HWND, unsigned, WORD, LONG);
BOOL MemoryInit(HANDLE);
