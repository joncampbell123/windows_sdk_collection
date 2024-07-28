#define IDM_ABOUT 100
#define IDM_INT   102


int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);

void SizeWindow (short *, short *);
void TSR_Check (void);
void TSR_Request (void);

// if compiling for Windows 3.0, uncomment the following two lines
// int FAR PASCAL SetSelectorBase (WORD, DWORD);
// int FAR PASCAL SetSelectorLimit (WORD, DWORD);


