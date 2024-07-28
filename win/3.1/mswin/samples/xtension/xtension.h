//*********************** Defines for the extension's menu items

// An extension's menu item ID should be in the range 1 through 99

#define IDM_STATUSWIN           10
#define IDM_GETFILESELLFN       15
#define IDM_GETDRIVEINFO        20
#define IDM_GETFOCUS            25
#define IDM_RELOADEXTENSIONS    30
#define IDM_REFRESHWINDOW       35
#define IDM_REFRESHALLWINDOWS   40
#define IDM_ABOUTEXT            45


//*********************** Defines for dialog boxes

#define IDD_PATH                206
#define IDD_VOLUME              207
#define IDD_SHARE               208
#define IDD_TOTALSPACE          209
#define IDD_FREESPACE           210

#define IDD_SELFILECOUNT        201
#define IDD_SELFILESIZE         202



//*********************** Misc. defines

#define STATUS_WIDTH            400
#define STATUS_HEIGHT           100
#define INFO_LINE_WIDTH         300
#define INFO_LINE_HEIGHT        18
#define INFO_LINE_X             10
#define INFO_LINE_Y             20
#define ID_STATUSTIMER          99
#define INFO_STR_LEN            50
#define TIMER_DURATION          1500        // 1.5 seconds
#define PATH_NAME_LEN           260
#define VOLUME_NAME_LEN         14
#define SHARE_NAME_LEN          128
#define SMALL_STR_LEN           12
#define LONG_STR_LEN            60



//*********************** Function prototypes

HMENU FAR PASCAL FMExtensionProc (HWND hwndXtension, WORD wMesssage,
                                  LONG lParam);

long FAR PASCAL StatusWndProc (HWND hWnd,   UINT uMessage,
			       WPARAM wParam, LPARAM lParam);

BOOL FAR PASCAL AboutDlgProc (HWND hDlg,   unsigned uMessage,
                              WORD wParam, LONG lParam);


BOOL FAR PASCAL DriveInfoDlgProc (HWND hDlg,   unsigned uMessage,
                                  WORD wParam, LONG lParam);

BOOL FAR PASCAL SelFileInfoDlgProc (HWND hDlg,   unsigned uMessage,
                                    WORD wParam, LONG lParam);

BOOL CreateStatusWindow (HWND hwndExtension);

void DisplayStatus (HWND hwndExtension, WORD wEvent);

