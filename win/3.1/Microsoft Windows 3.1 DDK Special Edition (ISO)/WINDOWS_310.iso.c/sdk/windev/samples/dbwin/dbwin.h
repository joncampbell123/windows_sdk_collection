#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <toolhelp.h>
#include "dbwindll.h"
#include "dlgdefs.h"

// Message sent when second instance of app is run,
// in order to activate first instance.
//
#define WM_ACTIVATEFIRST        WM_USER

//----- Non-dialog resource IDs

#define IDR_MAINACCEL           1
#define IDR_MAINMENU            2

//----- Menu command IDs

/* File menu */
#define CMD_FILESAVESETTINGS    1000
#define CMD_FILESAVEBUFFER      1001
#define CMD_FILEEXIT            1002

/* Edit menu */
#define CMD_EDITCOPY            1100
#define CMD_EDITCLEARBUFFER     1101
#define CMD_EDITSELECTALL       1102

/* Options menu */
#define CMD_OPTIONSSETTINGS     1200
#define CMD_OPTIONSALLOCBREAK   1201
#define CMD_OPTIONSOUTWINDOW    1202
#define CMD_OPTIONSOUTCOM1      1203
#define CMD_OPTIONSOUTCOM2      1204
#define CMD_OPTIONSOUTMONO      1205
#define CMD_OPTIONSOUTNONE      1206
#define CMD_OPTIONSTOPMOST      1220

/* Help menu */
#define CMD_HELPABOUT           1300

//----- Function declarations

BOOL DBWinInit(HINSTANCE hinst, HINSTANCE hinstPrev, int sw, LPSTR szCmdLine);
void DBWinTerminate(BOOL fEndSession);
LRESULT CALLBACK _export DBWinWndProc(HWND hwnd, UINT uiMessage, WPARAM wParam, LPARAM lParam);

void DoCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK _export NotifyCallback(UINT id, DWORD dwData);
BOOL SaveBuffer(HWND hwnd);

void OnBufferNotEmpty(void);

UINT CmdFromMode(UINT mode);
UINT ModeFromCmd(UINT cmd);

void DoInitMenu(HWND hwnd);
void SetTopmost(HWND hwnd, BOOL fTopmost);
void WriteDBWinState(HWND hwndMain);
void ReadDBWinState(HWND hwndMain);

void DoDebugOptions(HWND hwnd);
void DoSaveOptions(HWND hwnd);
void DoAllocBrk(HWND hwnd);
BOOL IsDebugSystem(HWND hwnd);

void DoAbout(HWND hwnd);
BOOL CALLBACK _export AboutDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
