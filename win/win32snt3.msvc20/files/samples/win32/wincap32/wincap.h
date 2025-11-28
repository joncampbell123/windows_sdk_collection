/*    PortTool v2.2     wincap.h          */

/* Header file for wincap.c */
/* Copyright (c) 1991 Microsoft Corporation. All rights reserved. */
 
//
// constants for menu items 
//


#define IDM_SAVE            110
#define IDM_PRINT           120
#define IDM_EXIT            130
#define IDM_ABOUT           140
#define IDM_EDITCOPY        180
#define IDM_CAPTWINDOW      210
#define IDM_CAPTCLIENT      220
#define IDM_CAPTRECT        230
#define IDM_CAPTUREHIDE     240
#define IDM_ACTIVEWINDOW    250
#define IDM_DESKTOP         260
#define IDM_VIEWFULL        310
#define IDM_VIEWRESTORED    320
#define IDM_VIEWCLEAR       330
#define IDM_CURSORSELECT    340
#define IDM_SELECTRECT      350


// Constants for string table

#define IDS_SAVEASTITLE      1



#define OPTION_FILE        0x0001
#define OPTION_PRINTER     0x0002

// For common dialog file/save

#define IDD_FILETYPE        101
#define IDD_RGB             102
#define IDD_RLE4            103
#define IDD_RLE8            104
#define IDD_PM              105
#define IDD_1               106
#define IDD_4               107
#define IDD_8               108
#define IDD_24              109
#define IDD_FILETYPEGROUP   110
#define IDD_BPP             111

// Stuff for "File.Print" dialog

typedef struct tagOPTIONS {
    int iOption;       // Print Options
    int iXScale;       // X scale factor (if iOption is DLGPRINT_SCALE)
    int iYScale;       // Y scale factor ('')
    } OPTIONSTRUCT, *LPOPTIONSTRUCT;

/*
 * function prototypes
 */

void cwCenter(HWND, int);
LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY PrintDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
int WINAPI InstallHook (HWND hWnd, BOOL bCode );
BOOL WINAPI HelpDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY SavingDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL GetFileName (HWND hWndOwner, LPSTR szFileName, LPDWORD dwFlags);
void DrawIndent(HWND hDlg, int ID, int iType);
void StretchIconToWindow(HWND hWndDlg, LPSTR szIconName);
void DoCapture(HWND hWnd, WORD wCommand);
void RubberBandScreen(LPRECT lpRect);
void DrawSelect( HDC hdc, BOOL fDraw, LPRECT lprClip);
void WINAPI NormalizeRect (LPRECT prc);
LONG APIENTRY FullViewWndProc(HWND, UINT, WPARAM, LPARAM);
void SaveMe(void);
void PrintMe(void);
void DoSize(HWND);
void ReallyGetClientRect(HWND, LPRECT);
void SetupScrollBars(HWND, WORD, WORD);
void DoScroll(HWND, int, int, int);
void DoPaint(HWND);
BOOL CALLBACK SelectDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK RectangleDlgProc(HWND, UINT, WPARAM, LPARAM);
void FrameWindow(HWND);
