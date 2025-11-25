#define MAXFONT 100
#define MAXSIZE  20
#define BUFSIZE  82
#define LINENUM  52
#define MAXBUF  256

#define IDM_OPEN         100
#define IDM_PRNTSET      110
#define IDM_PRINT        120
#define IDM_QUIT         130
#define IDM_ABOUT        200
#define IDM_SYSTEM       300
#define IDM_SYSTEMFIXED  400
#define IDM_SELECT       600
#define IDM_LINEPRNT     700
#define IDM_NVERTIC      800
#define IDM_CLEAR        900
#define IDM_MODE        1000
#define IDC_FILENAME    1100
#define IDC_EDIT        1200
#define IDC_LISTBOX     1300
#define IDC_FILES       1400
#define IDC_PATH        1500
#define ID_FACES     2000
#define ID_SIZES     2100
#define ID_FILES     2200
#define ID_EDIT      2300
#define ID_PATH      2400
#define ID_FONT      2500

#define STR_NOPRNTDRV		3000
#define STR_CANNOTPRINT		3100
#define STR_CANNOTOPEN		3200
#define STR_SCREENONLY		3300
#define STR_FILTERSTRING	3400
#define STR_NOTLOADPRNTDRV	3500
#define STR_NOTGETADDRESS	3600

int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);
HANDLE GetPrinterDC(HWND, int);
void GetFonts(HWND);
void GetSizes(HWND, int);
void GetUseFont(HWND);
void SetInitPosition();
void GetClientSize(HWND);
void SetTitle(HWND, PSTR);
int f_gets(PSTR, int, int);
void StringOut(HWND, PSTR);
void Redraw(HWND, HDC);
void PrintOut(HDC, HWND);
int FAR PASCAL AbortProc(HDC, int);
int FAR PASCAL AbortDlg(HWND, UINT, WPARAM, LPARAM);
LRESULT FAR PASCAL MainWndProc(HWND, UINT, WPARAM, LPARAM);
HFILE OpenDlg(HWND);
BOOL FAR PASCAL About(HWND, UINT, WPARAM, LPARAM);
void ImeMoveConvertWin(HWND, int, int);
