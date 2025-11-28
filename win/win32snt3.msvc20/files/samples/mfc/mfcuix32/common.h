/*
 * COMMON.H
 *
 * Structures and definitions applicable to all OLE 2.0 UI dialogs.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */


#ifndef _COMMON_H_
#define _COMMON_H_


//Macros to handle control message packing between Win16 and Win32
#ifndef COMMANDPARAMS
#define COMMANDPARAMS(wID, wCode, hWndMsg)                          \
	WORD        wID     = LOWORD(wParam);                           \
	WORD        wCode   = HIWORD(wParam);                           \
	HWND        hWndMsg = (HWND)(UINT)lParam;
#endif  //COMMANDPARAMS

#ifndef SendCommand
#define SendCommand(hWnd, wID, wCode, hControl)                     \
			SendMessage(hWnd, WM_COMMAND, MAKELONG(wID, wCode)      \
						, (LPARAM)hControl)
#endif  //SendCommand


//Property labels used to store dialog structures and fonts
#define STRUCTUREPROP       TEXT("Structure")
#define FONTPROP            TEXT("Font")


/*
 * Standard structure for all dialogs.  This commonality lets us make
 * a single piece of code that will validate this entire structure and
 * perform any necessary initialization.
 */

typedef struct tagOLEUISTANDARD
{
	//These IN fields are standard across all OLEUI dialog functions.
	DWORD           cbStruct;       //Structure Size
	DWORD           dwFlags;        //IN-OUT:  Flags
	HWND            hWndOwner;      //Owning window
	LPCTSTR         lpszCaption;    //Dialog caption bar contents
	LPFNOLEUIHOOK   lpfnHook;       //Hook callback
	LPARAM          lCustData;      //Custom data to pass to hook
	HINSTANCE       hInstance;      //Instance for customized template name
	LPCTSTR         lpszTemplate;   //Customized template name
	HRSRC           hResource;      //Customized template handle
} OLEUISTANDARD, *POLEUISTANDARD, FAR *LPOLEUISTANDARD;


//Function prototypes
//COMMON.C
UINT  WINAPI  UStandardValidation(LPOLEUISTANDARD, const UINT, const HGLOBAL FAR *);
UINT  WINAPI  UStandardInvocation(DLGPROC, LPOLEUISTANDARD, HGLOBAL, LPTSTR);

LPVOID WINAPI LpvStandardInit(HWND, UINT, BOOL, HFONT FAR *);
LPVOID WINAPI LpvStandardEntry(HWND, UINT, WPARAM, LPARAM, UINT FAR *);
UINT WINAPI   UStandardHook(LPVOID, HWND, UINT, WPARAM, LPARAM);
void WINAPI   StandardCleanup(LPVOID, HWND);
void WINAPI   StandardShowDlgItem(HWND hDlg, int idControl, int nCmdShow);

// shared globals: registered messages
extern UINT uMsgHelp;
extern UINT uMsgEndDialog;
extern UINT uMsgBrowse;
extern UINT uMsgChangeIcon;
extern UINT uMsgFileOKString;
extern UINT uMsgCloseBusyDlg;

//Standard control identifiers
#define ID_NULL                         98

#endif //_COMMON_H_
