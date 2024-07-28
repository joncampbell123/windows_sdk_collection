// Function Prototypes...

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
long FAR PASCAL MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL FAR PASCAL About(HWND, unsigned, WORD, LONG);
BOOL FAR PASCAL SampleDialog(HWND, unsigned, WORD, LONG);
DWORD FAR PASCAL HelpMessageFilterHook(int, WORD, LPMSG);

// Constant definitions...
#define helphook                     33

#define IDM_ABOUT                   100
#define IDM_DLG1                    101
#define IDM_DLG2                    102
#define IDM_DLG3                    103
#define IDM_DLG4                    104
#define IDM_DLG5                    105
#define IDM_DLG6                    106
#define IDM_DLG7                    107
#define IDM_DLG8                    108
#define IDM_DLG9                    109
#define ID_HELP                     201
#define ID_HELPCONTEXT              1000
