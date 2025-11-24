/*
 *  netwatch.h
 *  
 *  Purpose:
 *      main header file
 *  
 *  Owner:
 *      MikeSart
 */

// max size of strings in STRINGTABLE for LoadString
#define MAX_STRINGTABLE_LEN 200

#define FILE_ENUM_ERROR     0x20000000
#define TIMERID             1

// listbox defines
#define chBOLD              TEXT('\b')
#define chUNDERLINE         TEXT('\v')
#define chTAB               TEXT('\t')
#define chBITMAP            TEXT('\001')

#define BMWIDTH             16
#define BMHEIGHT            16
#define NUMBMPS             9
#define RGBREPLACE          0x00FF0000 // solid blue

#define NUMPROPFIELDS       6
typedef struct
{
    WORD    rgIDSStart;
    DWORD   dwrgBmp;
    TCHAR   *rgsz[NUMPROPFIELDS];
} PROPERTIES;

#define GlobalFreeNullPtr(_ptr) \
    if(_ptr) GlobalFreePtr(_ptr)

/*
 *  global variables from globals.c
 */
extern TCHAR        *szAppName;
extern DWORD        dwTimerInterval;
extern TCHAR        *szServerName;
extern TCHAR        szFmtNum[];
extern TCHAR        szNil[];
extern UINT         unMenuFlags[];
extern TCHAR        szBuffer[];
extern HINSTANCE    ghInst;
extern HWND         hwndMain;
extern HMENU        ghMenu;
extern DWORD        dwNumUsers; // number of users connected to server

/*
 *  function prototypes
 */
// bitmap.c
void                DrawItem(LPDRAWITEMSTRUCT pDI);
VOID                MeasureItem(HANDLE hwnd, LPMEASUREITEMSTRUCT mis);
void                SetRGBValues(void);
BOOL                InitBmps(HWND hwnd);
void                DeInitBmps(void);
BOOL                LoadBitmapLB(void);
void                BlitIcon(HDC hdc, LONG x, LONG y, int nBitmap);

// datetime.c
void                GetInternational(void);

// netwatch.c
LRESULT CALLBACK    NewLBProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK       SelectDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK       PropDlgProc(HWND, UINT, WPARAM, LPARAM);

// utils.c
void                RefreshDisplay(HWND hwnd);
void                ShowTitle(HWND hwnd, int nCmdShow);
DWORD               SetWindowTextAndServerName(HWND, LPTSTR);
void                UpdateWindowText(HWND hwnd, BOOL fForceUpdate);
void                PunchTimer(BOOL fNewState);
BOOL                InitNetWatch(BOOL fInit);
void                RestoreWindowPosition(HWND hwnd);
void                SaveWindowPosition(HWND hwnd);
TCHAR               *szFromIDS2(UINT unID);
TCHAR               *szFromIDS1(UINT unID);
TCHAR               *AllocAndLoadString(UINT unID);
TCHAR               *GetSystemErrMessage(DWORD dwError);

// net.c
void                RefreshDisplay(HWND hwnd);
void                HandleWM_VKEY(HWND hwnd, WORD wAction);
void                AddErrorStringToLB(DWORD nas);
void                HandleMenu(HWND hwnd);
