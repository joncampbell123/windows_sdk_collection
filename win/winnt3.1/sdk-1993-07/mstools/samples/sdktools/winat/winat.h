//============================================================================
//
// Module: WINAT.H
//
// Purpose: Header file for WINAT.C
//
// Author: Kory Gill
// Date  : March 1993
//
// Copyright: Copyright Microsoft Corporation 1993
//
//============================================================================


//============================================================================
// defines
//============================================================================
#define WM_SPINBUTTON      (WM_USER + 0)

#define DAYSINWEEK         (7)
#define DEFAULT_AMPM       "AM"
#define DEFAULT_HOUR       (12)
#define DEFAULT_MIN        (0)
#define DEL_ALL            (1)
#define DEL_ID             (0)
#define DEL_ID_CHANGE      (2)
#define DOWN               (-1)
#define DWH                (320)
#define DWW                (570)
#define DWX                (0)
#define DWY                (0)
#define FIRST_TIME         (0)
#define REG_HKEY		   HKEY_CURRENT_USER			   // old: HKEY_LOCAL_MACHINE
#define MAXCOMMANDLEN      (128)
#define MAXCOMPUTERNAMELEN (17)                            // 15 char name, 2 '\'
#define MAXWHENDAYLEN      (108+TIME_LEN+1)
#define MAX_ATCMD_LEN      (5+MAXWHENDAYLEN+MAXCOMMANDLEN) //id, tab, when/days, tab, cmd
#define MAX_SELCOMMANDS    (256)
#define NUMDAYITEMS        (DAYSINWEEK+31)
#define NUM_TIMES          (48)
#define REFRESH_ERROR      (5)
#define REFRESH_FALSE      (0)
#define REFRESH_QUERY      (2)
#define REFRESH_RATE       (15000)
#define REFRESH_RESTORE    (4)
#define REFRESH_SAVE       (3)                                        
#define REFRESH_TRUE       (1)
#define SCS_BOTTOM		   (0)
#define SCS_NOCHANGE       (1)
#define SCS_TOP            (2)
#define SCS_SELINHIGHWORD  (3)
#define SCS_NONE		   (4)     
#define SZAPPTITLE		   "WINAT"
#define SZWINDOWTITLE      "Command Scheduler"
#define SZCREDITS		   "Developed for Microsoft\rby Kory Gill"
#define SZGENERROR         "Problem accessing computer specified."
#define SZNOENTRIES        "No entries in list."
#define SZREGPATH          "Software\\Microsoft\\Resource Kit\\Winat"
#define TIME_LEN           (5)
#define UP                 (1)


//============================================================================
// user-defined types
//============================================================================

typedef struct _ATTIME {
    UCHAR szTime[TIME_LEN+1];
    DWORD dwMsSinceMidnight;
} ATTIME, *PATTIME;


//============================================================================
// internal function prototypes
//============================================================================

BOOL APIENTRY    AddChangeDlgProc            ( HWND, UINT, UINT, LONG      );
UINT             AreYouSure                  ( HWND                        );
LRESULT APIENTRY AmPmProc                    ( HWND, UINT, UINT, LONG      );
WCHAR *          AsciizToUnicode             ( CHAR *                      );
LRESULT APIENTRY DownButtonProc				 ( HWND, UINT, UINT, LONG      );
void             DrawBitmap                  ( HDC, LONG, LONG, HBITMAP, DWORD );
LRESULT APIENTRY HourProc                    ( HWND, UINT, UINT, LONG      );
NET_API_STATUS   JobAdd                      ( VOID                        );
NET_API_STATUS   JobEnum                     ( DWORD                       );
NET_API_STATUS   JobDel                      ( UINT, HWND                  );
DWORD            MessagePrint                ( DWORD, ...                  );
LRESULT APIENTRY MinProc                     ( HWND, UINT, UINT, LONG      );
UINT             Refresh                     ( UINT                        );
BOOL APIENTRY    SelectComputerProc          ( HWND, UINT, UINT, LONG      );
BOOL             ServiceNotStartedHandler    ( VOID                        );
INT  APIENTRY	 ShellAbout			         ( HWND, LPCSTR, LPCSTR, HICON );
LRESULT APIENTRY UpButtonProc				 ( HWND, UINT, UINT, LONG      );
BOOL             ValidateAddChangeArguments  ( HWND                        );
BOOL APIENTRY    WINATDlgProc                ( HWND, UINT, UINT, LONG      );


//============================================================================
// external function prototypes
//============================================================================

#define IDC_COMPUTER            100
#define IDC_ATCOMMANDS          101
#define IDC_ADD                 102
#define IDC_CHANGE              103
#define IDC_REMOVE              105
#define IDC_HELP                106
#define IDC_NEWCOMMAND          107
#define IDC_TODAY               108
#define IDC_TOMORROW            109
#define IDC_EVERY               110
#define IDC_NEXT                111
#define IDC_DAYS                112
#define IDC_HOUR                113
#define IDC_MIN                 114
#define IDC_AMPM                115
#define IDC_ADDCHANGEHELP       117
#define IDC_REFRESH             118
#define IDC_SELECTCOMPUTER      119
#define IDM_SAVE_TO_REGISTRY    120
#define IDM_GET_FROM_REGISTRY   121

#define IDM_HELPCONTENTS        300
#define IDM_HELPSEARCH          301
#define IDM_HELPHELP            302
#define IDM_ABOUT               303
#define IDM_SELECTCOMPUTER      304
#define IDM_EXIT                305

#define DLG_VERFIRST            400
#define DLG_VERLAST             404

#define IDD_UPBUT 				500
#define IDD_DOWNBUT             501
