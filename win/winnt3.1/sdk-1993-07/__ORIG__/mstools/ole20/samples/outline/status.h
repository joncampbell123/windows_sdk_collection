/*************************************************************************
** 
**    OLE 2.0 Sample Code
**    
**    status.h
**    
**	  This file contains typedefs, defines, global variable declarations, 
**    and function prototypes for the status bar window.
**
**	  (c) Copyright Microsoft Corp. 1992 - 1993 All Rights Reserved
**
*************************************************************************/

// Sizes of statusbar items
#if defined( USE_STATUSBAR )
	#define STATUS_HEIGHT	23
#else 
	#define STATUS_HEIGHT	0
#endif 
#define STATUS_RLEFT	8
#define STATUS_RRIGHT	400
#define STATUS_RTOP		3
#define STATUS_RBOTTOM	20
#define STATUS_TTOP		4
#define STATUS_TLEFT	11
#define STATUS_THEIGHT	18


typedef enum {
	STATUS_READY,
	STATUS_BLANK
} STATCONTROL;

// Window for status bar.
extern HWND hwndStatusbar;

BOOL RegisterStatusClass(HINSTANCE hInstance);
HWND CreateStatusWindow(HWND hWndApp, HINSTANCE hInst);
void DestroyStatusWindow(HWND hWndStatusBar);

void AssignPopupMessage(HMENU hmenuPopup, char *szMessage);

void ItemMessage(HWND hWndStatusBar, UINT wIDItem, LPVOID lpDoc);
void PopupMessage(HWND hWndStatusBar, HMENU hmenuPopup, LPVOID lpDoc);
void SysMenuMessage(HWND hWndStatusBar, UINT wIDItem, LPVOID lpDoc);
void ControlMessage(HWND hWndStatusBar, STATCONTROL scCommand, LPVOID lpDoc);

void StringMessage(HWND hWndStatusBar, char *szMessage, LPVOID lpDoc);
