#define IDM_ABOUT 100
#define IDM_ALLOC 101
#define IDM_INT   102
#define IDM_FREE  103


int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, UINT, WPARAM,LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

DWORD FAR PASCAL GlobalDosAlloc (DWORD);
UINT FAR PASCAL GlobalDosFree (UINT);

void SizeWindow (short *, short *);
void TSR_Check (void);
void TSR_Request (void);

