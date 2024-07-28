/**[f******************************************************************
 * windows.h - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Company confidential.
 *
 **f]*****************************************************************/

#include "style.h"

#ifndef NOMINMAX
#define max(a,b)	    ((a) > (b) ? (a) : (b))
#define min(a,b)	    ((a) < (b) ? (a) : (b))
#endif

#define INTENSITY(r,g,b)	(BYTE)(((WORD)((r) * 30) + (WORD)((g) * 59) + (WORD)((b) * 11))/100)
#define RGB(r,g,b)  ((DWORD)( (WORD)((BYTE)(r)|((BYTE)(g)<<8)) | (((DWORD)(BYTE)(b))<<16)) )


#define ODD(X)	((X) & 1)	/* Returns TRUE if X is odd */
#define EVEN(X) (!((X) & 1))	/* Returns TRUE if X is even */

#define TRUE	1
#define FALSE	0
#define NULL	0



/* Combo Box return Values */
#define CB_OKAY 	    0
#define CB_ERR		    (-1)
#define CB_ERRSPACE	    (-2)


/* Combo Box Notification Codes */
#define CBN_ERRSPACE	    (-1)
#define CBN_SELCHANGE	    1
#define CBN_DBLCLK	    2
#define CBN_SETFOCUS	    3
#define CBN_KILLFOCUS	    4
#define CBN_EDITCHANGE      5
#define CBN_EDITUPDATE      6
#define CBN_DROPDOWN        7


/* Combo Box messages */
#define STM_SETICON        (WM_USER+0)

#define CB_GETEDITSEL	   (WM_USER+0)
#define CB_LIMITTEXT	   (WM_USER+1)
#define CB_SETEDITSEL	   (WM_USER+2)
#define CB_ADDSTRING	   (WM_USER+3)
#define CB_DELETESTRING	   (WM_USER+4)
#define CB_DIR             (WM_USER+5)
#define CB_GETCOUNT	   (WM_USER+6)
#define CB_GETCURSEL	   (WM_USER+7)
#define CB_GETLBTEXT	   (WM_USER+8)
#define CB_GETLBTEXTLEN	   (WM_USER+9)
#define CB_INSERTSTRING    (WM_USER+10)
#define CB_RESETCONTENT	   (WM_USER+11)
#define CB_FINDSTRING	   (WM_USER+12)
#define CB_SELECTSTRING	   (WM_USER+13)
#define CB_SETCURSEL	   (WM_USER+14)
#define CB_RECALCULATEINTERNALS (WM_USER+15)
#define CB_MSGMAX          (WM_USER+16)

/* Dialog Box Command IDs */
#define IDOK		    1
#define IDCANCEL	    2
#define IDABORT 	    3
#define IDRETRY 	    4
#define IDIGNORE	    5
#define IDYES		    6
#define IDNO		    7


#define FAR	far
#define NEAR	near
#define PASCAL	pascal

#ifdef BUILDDLL 				/* ;Internal */
#define API		    _loadds far pascal	/* ;Internal */
#define CALLBACK	    _loadds far pascal	/* ;Internal */
#else						/* ;Internal */
#define API		    far pascal
#define CALLBACK	    far pascal
#endif						/* ;Internal */
#define WINAPI              API

typedef struct {
	int x;
	int y;
} POINT;
typedef POINT FAR *LPPOINT;

typedef struct tagRECT {
    int 	left;
    int 	top;
    int 	right;
    int 	bottom;
} RECT;

typedef RECT FAR *LPRECT;

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned int    UINT;
typedef long		LONG;
typedef unsigned long	DWORD;
typedef WORD		HANDLE;
#define DECLARE_HANDLE(name)    typedef HANDLE name;
typedef HANDLE		HDC;
typedef HANDLE		HCURSOR;
typedef HANDLE		HFONT;
typedef HANDLE		HICON;
typedef HANDLE          HMODULE;
typedef HANDLE          HINSTANCE;
typedef HANDLE FAR *	LPHANDLE;
typedef int		BOOL;
typedef int FAR	*	LPBOOL;
typedef short FAR *	LPSHORT;
typedef WORD FAR *	LPWORD;
typedef char FAR *	LPSTR;
typedef char *          PSTR;

typedef int (FAR PASCAL *FARPROC)();

typedef WORD HWND;
typedef WORD HINST;


#define MAKEINTRESOURCE(i)  (LPSTR)((DWORD)((WORD)(i)))
#define MAKELONG(a, b)	((long)(((unsigned)(a)) | ((unsigned long)((unsigned)(b))) << 16))
#define LOWORD(l)	((WORD)(l))
#define HIWORD(l)	((WORD)(((DWORD)(l) >> 16) & 0xffff))
#define LOBYTE(w)	    ((BYTE)(w))
#define HIBYTE(w)	    ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define GetRValue(rgb)		((BYTE)(rgb))
#define GetGValue(rgb)		((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)		((BYTE)((rgb)>>16))

#define RT_RCDATA	    MAKEINTRESOURCE(10)

#define LBN_SELCHANGE   1
#define LBN_DBLCLK      2


/* Listbox Return Values */
#define LB_OKAY 	    0
#define LB_ERR		    (-1)
#define LB_ERRSPACE	    (-2)



/* Listbox messages */
#define LB_ADDSTRING	       (WM_USER+1)
#define LB_INSERTSTRING        (WM_USER+2)
#define LB_DELETESTRING        (WM_USER+3)
#define LB_RESETCONTENT        (WM_USER+5)
#define LB_SETSEL	       (WM_USER+6)
#define LB_SETCURSEL	       (WM_USER+7)
#define LB_GETSEL	       (WM_USER+8)
#define LB_GETCURSEL	       (WM_USER+9)
#define LB_GETTEXT	       (WM_USER+10)
#define LB_GETTEXTLEN	       (WM_USER+11)
#define LB_GETCOUNT	       (WM_USER+12)
#define LB_SELECTSTRING        (WM_USER+13)
#define LB_DIR		       (WM_USER+14)
#define LB_GETTOPINDEX	       (WM_USER+15)
#define LB_FINDSTRING	       (WM_USER+16)
#define LB_GETSELCOUNT	       (WM_USER+17)
#define LB_GETSELITEMS	       (WM_USER+18)
#define LB_SETTABSTOPS         (WM_USER+19)
#define LB_GETHORIZONTALEXTENT (WM_USER+20)
#define LB_SETHORIZONTALEXTENT (WM_USER+21)
#define LB_SETCOLUMNWIDTH      (WM_USER+22)
#define LB_SETTOPINDEX	       (WM_USER+24)
#define LB_GETITEMRECT	       (WM_USER+25)
#define LB_GETITEMDATA         (WM_USER+26)
#define LB_SETITEMDATA         (WM_USER+27)
#define LB_SELITEMRANGE        (WM_USER+28)
#define LB_MSGMAX	       (WM_USER+33)

/* notification codes */
#define EN_SETFOCUS   0x0100
#define EN_KILLFOCUS  0x0200
#define EN_CHANGE     0x0300
#define EN_ERRSPACE   0x0500
#define EN_HSCROLL    0x0601
#define EN_VSCROLL    0x0602


#define MB_OK			0x0000
#define MB_OKCANCEL		0x0001
#define MB_ICONQUESTION		0x0020
#define MB_ICONEXCLAMATION	0x0030


/* Interface to global memory manager */
#define GMEM_FIXED	    0x0000
#define GMEM_MOVEABLE	    0x0002
#define GMEM_NOCOMPACT	    0x0010
#define GMEM_NODISCARD	    0x0020
#define GMEM_ZEROINIT	    0x0040
#define GMEM_MODIFY	    0x0080
#define GMEM_DISCARDABLE    0x0F00
#define GHND	(GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR	(GMEM_FIXED    | GMEM_ZEROINIT)
#define GDLLHND   (GHND | GMEM_SHARE)
#define GDLLPTR   (GPTR | GMEM_SHARE)
#define GMEM_SHARE          0x2000
#define GMEM_SHAREALL       0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_LOWER          0x1000
#define GMEM_NOTIFY         0x4000

#define WM_PSDEVMODECHANGE    0x001b
#define WM_INITDIALOG	    0x0110
#define WM_COMMAND	    0x0111

#define WM_USER 	 0x0400
#define EM_LIMITTEXT	 WM_USER+21


#define UnlockResource(h)   GlobalUnlock(h)

HANDLE	FAR PASCAL FindResource( HANDLE, LPSTR, LPSTR );
int	FAR PASCAL AccessResource( HANDLE, HANDLE );
HANDLE	FAR PASCAL LoadResource( HANDLE, HANDLE );
BOOL	FAR PASCAL FreeResource( HANDLE );
LPSTR	FAR PASCAL LockResource( HANDLE );
HANDLE	FAR PASCAL GetModuleHandle( LPSTR );
WORD	FAR PASCAL SizeofResource( HANDLE, HANDLE );


void	FAR PASCAL FatalExit( int );
HANDLE	FAR PASCAL FindResource( HANDLE, LPSTR, LPSTR );
HANDLE	FAR PASCAL LoadResource( HANDLE, HANDLE );
BOOL	FAR PASCAL FreeResource( HANDLE );
LPSTR	FAR PASCAL LockResource( HANDLE );
WORD	FAR PASCAL SizeofResource( HANDLE, HANDLE );

HANDLE	FAR PASCAL GlobalAlloc( WORD, DWORD );
HANDLE	FAR PASCAL GlobalFree( HANDLE );
LPSTR	FAR PASCAL GlobalLock( HANDLE );
BOOL	FAR PASCAL GlobalUnlock( HANDLE );
DWORD	FAR PASCAL GlobalCompact(DWORD);

HANDLE	FAR PASCAL LocalAlloc( WORD, WORD );
HANDLE	FAR PASCAL LocalReAlloc(HANDLE, WORD, WORD);
HANDLE	FAR PASCAL LocalFree( HANDLE );
BOOL	FAR PASCAL LocalUnlock( HANDLE );
char NEAR * FAR PASCAL LocalLock( HANDLE );
HANDLE	FAR PASCAL LocalHandle(WORD);

int FAR PASCAL wvsprintf(LPSTR,LPSTR,LPSTR);

/* Local Memory Flags */
#define LMEM_FIXED	    0x0000
#define LMEM_MOVEABLE	    0x0002
#define LMEM_NOCOMPACT	    0x0010
#define LMEM_NODISCARD	    0x0020
#define LMEM_ZEROINIT	    0x0040
#define LMEM_MODIFY	    0x0080
#define LMEM_DISCARDABLE    0x0F00

#define LHND		    (LMEM_MOVEABLE | LMEM_ZEROINIT)
#define LPTR		    (LMEM_FIXED | LMEM_ZEROINIT)


DWORD	    FAR PASCAL GlobalCompact( DWORD );
#define GlobalDiscard( h ) GlobalReAlloc( h, 0L, GMEM_MOVEABLE )
DWORD	    FAR PASCAL GlobalHandle( WORD );
HANDLE	    FAR PASCAL GlobalReAlloc( HANDLE, DWORD, WORD );
DWORD	    FAR PASCAL GlobalSize( HANDLE );
WORD	    FAR PASCAL GlobalFlags( HANDLE );

int	    FAR PASCAL LoadString( HANDLE, unsigned, LPSTR, int );
long	    FAR PASCAL SendMessage(HWND, unsigned, WORD, LONG);
short	    FAR PASCAL SetEnvironment(LPSTR, LPSTR, WORD);
short	    FAR PASCAL GetEnvironment(LPSTR, LPSTR, WORD);
HWND	    FAR PASCAL SetFocus(HWND);

int	    FAR PASCAL GetProfileInt( LPSTR, LPSTR, int );
int	    FAR PASCAL GetProfileString( LPSTR, LPSTR, LPSTR, LPSTR, int );
BOOL	    FAR PASCAL WriteProfileString( LPSTR, LPSTR, LPSTR );
FARPROC     FAR PASCAL MakeProcInstance(FARPROC, HANDLE);
void	    FAR PASCAL FreeProcInstance(FARPROC);

/* MessageBox() Flags */
#define MB_OK		    0x0000
#define MB_OKCANCEL	    0x0001
#define MB_ABORTRETRYIGNORE 0x0002
#define MB_YESNOCANCEL	    0x0003
#define MB_YESNO	    0x0004
#define MB_RETRYCANCEL	    0x0005

#define MB_ICONHAND	    0x0010
#define MB_ICONQUESTION	    0x0020
#define MB_ICONEXCLAMATION  0x0030
#define MB_ICONASTERISK     0x0040

#define MB_ICONINFORMATION  MB_ICONASTERISK
#define MB_ICONSTOP	    MB_ICONHAND

#define MB_DEFBUTTON1	    0x0000
#define MB_DEFBUTTON2	    0x0100
#define MB_DEFBUTTON3	    0x0200

#define MB_APPLMODAL	    0x0000
#define MB_SYSTEMMODAL	    0x1000
#define MB_TASKMODAL	    0x2000

#define MB_NOFOCUS	    0x8000

#define MB_TYPEMASK	    0x000F
#define MB_ICONMASK	    0x00F0
#define MB_DEFMASK	    0x0F00
#define MB_MODEMASK	    0x3000
#define MB_MISCMASK	    0xC000


int  FAR PASCAL MessageBox(HWND, LPSTR, LPSTR, WORD);

int    	FAR PASCAL GetWindowText(HWND, LPSTR, int);
void   	FAR PASCAL SetWindowText(HWND, LPSTR);
BOOL 	FAR PASCAL ShowWindow(HWND, int);

/* ShowWindow() Commands */
#define SW_HIDE		    0
#define SW_SHOWNORMAL	    1
#define SW_RESTORE	    1
#define SW_NORMAL	    1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE	    3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW		    5
#define SW_MINIMIZE	    6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA	    8


/* Character Translation Routines */
BOOL  FAR PASCAL AnsiToOem(LPSTR, LPSTR);
BOOL  FAR PASCAL OemToAnsi(LPSTR, LPSTR);
LPSTR FAR PASCAL AnsiUpper(LPSTR);
LPSTR FAR PASCAL AnsiLower(LPSTR);
LPSTR FAR PASCAL AnsiNext(LPSTR);
LPSTR FAR PASCAL AnsiPrev(LPSTR, LPSTR);

WORD FAR PASCAL GetVersion(void);
BOOL FAR PASCAL IsRectEmpty(LPRECT);
int  FAR PASCAL IntersectRect(LPRECT, LPRECT, LPRECT);
int  FAR PASCAL SetRectEmpty(LPRECT);
void FAR PASCAL SetRect(LPRECT, int, int, int, int);

HWND FAR PASCAL FindWindow(LPSTR, LPSTR);
HCURSOR FAR PASCAL SetCursor(HCURSOR);
HCURSOR FAR PASCAL LoadCursor( HANDLE, LPSTR );
HWND FAR PASCAL GetFocus(void);
int   FAR PASCAL ReleaseDC(HWND, HDC);
HDC   FAR PASCAL GetDC(HWND);

HWND FAR PASCAL CreateDialog(HANDLE, LPSTR, HWND, FARPROC);
HWND FAR PASCAL CreateDialogIndirect(HANDLE, LPSTR, HWND, FARPROC);
int  FAR PASCAL DialogBox(HANDLE, LPSTR, HWND, FARPROC);
int  FAR PASCAL DialogBoxIndirect(HANDLE, HANDLE, HWND, FARPROC);
void FAR PASCAL EndDialog(HWND, int);
HWND FAR PASCAL GetDlgItem(HWND, int);
void FAR PASCAL SetDlgItemInt(HWND, int, WORD, BOOL);
WORD FAR PASCAL GetDlgItemInt(HWND, int, BOOL FAR *, BOOL);
void FAR PASCAL SetDlgItemText(HWND, int, LPSTR);
int  FAR PASCAL GetDlgItemText(HWND, int, LPSTR, int);
void FAR PASCAL CheckDlgButton(HWND, int, WORD);
void FAR PASCAL CheckRadioButton(HWND, int, int, int);
WORD FAR PASCAL IsDlgButtonChecked(HWND, int);
LONG FAR PASCAL SendDlgItemMessage(HWND, int, WORD, WORD, LONG);
HWND FAR PASCAL GetNextDlgGroupItem(HWND, HWND, BOOL);
HWND FAR PASCAL GetNextDlgTabItem(HWND, HWND, BOOL);
int  FAR PASCAL GetDlgCtrlID(HWND);

HANDLE	FAR PASCAL LoadLibrary(LPSTR);
HANDLE	FAR PASCAL FreeLibrary(HANDLE);

int	FAR PASCAL GetModuleFileName(HANDLE, LPSTR, int);
FARPROC FAR PASCAL GetProcAddress(HANDLE, LPSTR);

BOOL   FAR PASCAL Yield(void);
HANDLE FAR PASCAL GetCurrentTask(void);
int    FAR PASCAL SetPriority(HANDLE, int);


/* Message structure */
typedef struct tagMSG
  {
    HWND	hwnd;
    WORD	message;
    WORD	wParam;
    LONG	lParam;
    DWORD	time;
    POINT	pt;
  } MSG;
typedef MSG		    *PMSG;
typedef MSG NEAR	    *NPMSG;
typedef MSG FAR 	    *LPMSG;

/* Message Function Templates */
BOOL FAR PASCAL GetMessage(LPMSG, HWND, WORD, WORD);
BOOL FAR PASCAL TranslateMessage(LPMSG);
LONG FAR PASCAL DispatchMessage(LPMSG);
BOOL FAR PASCAL PeekMessage(LPMSG, HWND, WORD, WORD, WORD);
BOOL  FAR PASCAL PostMessage(HWND, WORD, WORD, LONG);
HICON FAR PASCAL LoadIcon(HANDLE, LPSTR);

BOOL FAR PASCAL DestroyWindow(HWND);
BOOL FAR PASCAL EnableWindow(HWND,BOOL);
BOOL FAR PASCAL IsWindowVisible(HWND);
void FAR PASCAL MoveWindow(HWND, int, int, int, int, BOOL);
int FAR PASCAL GetDeviceCaps(HDC, int);
BOOL FAR PASCAL MessageBeep(WORD);
void FAR PASCAL UpdateWindow(HWND);

/* undocumented exported windows functions from winexp.h */

int         far PASCAL OpenPathname( LPSTR, int );
int         far PASCAL DeletePathname( LPSTR );
int         far PASCAL _lopen( LPSTR, int );
void        far PASCAL _lclose( int );
int         far PASCAL _lcreat( LPSTR, int );
WORD        far PASCAL _ldup( int );
LONG        far PASCAL _llseek( int, long, int );
WORD        far PASCAL _lread( int, LPSTR, int );
WORD        far PASCAL _lwrite( int, LPSTR, int );

int FAR cdecl wsprintf  (LPSTR, LPSTR, ...);


#define  SEEK_SET 0	/* beginning of file */
#define  SEEK_CUR 1
#define  SEEK_END 2

#define READ        0   /* Flags for _lopen */
#define WRITE       1
#define READ_WRITE  2


/* User Button Notification Codes */
#define BN_CLICKED	   0
#define BN_PAINT	   1
#define BN_HILITE	   2
#define BN_UNHILITE	   3
#define BN_DISABLE	   4
#define BN_DOUBLECLICKED   5

/* Button Control Messages */
#define BM_GETCHECK	   (WM_USER+0)
#define BM_SETCHECK	   (WM_USER+1)
#define BM_GETSTATE	   (WM_USER+2)
#define BM_SETSTATE	   (WM_USER+3)
#define BM_SETSTYLE	   (WM_USER+4)


/*  Ternary raster operations */
#define SRCCOPY 	    (DWORD)0x00CC0020 /* dest = source			 */
#define SRCPAINT	    (DWORD)0x00EE0086 /* dest = source OR dest		 */
#define SRCAND		    (DWORD)0x008800C6 /* dest = source AND dest 	 */
#define SRCINVERT	    (DWORD)0x00660046 /* dest = source XOR dest 	 */
#define SRCERASE	    (DWORD)0x00440328 /* dest = source AND (NOT dest )	 */
#define NOTSRCCOPY	    (DWORD)0x00330008 /* dest = (NOT source)		 */
#define NOTSRCERASE	    (DWORD)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY	    (DWORD)0x00C000CA /* dest = (source AND pattern)	 */
#define MERGEPAINT	    (DWORD)0x00BB0226 /* dest = (NOT source) OR dest	 */
#define PATCOPY 	    (DWORD)0x00F00021 /* dest = pattern 		 */
#define PATPAINT	    (DWORD)0x00FB0A09 /* dest = DPSnoo			 */
#define PATINVERT	    (DWORD)0x005A0049 /* dest = pattern XOR dest	 */
#define DSTINVERT	    (DWORD)0x00550009 /* dest = (NOT dest)		 */
#define BLACKNESS	    (DWORD)0x00000042 /* dest = BLACK			 */
#define WHITENESS	    (DWORD)0x00FF0062 /* dest = WHITE			 */


/* OpenFile() Structure */
typedef struct tagOFSTRUCT
  {
    BYTE	cBytes;
    BYTE	fFixedDisk;
    WORD	nErrCode;
    BYTE	reserved[4];
    BYTE	szPathName[128];
  } OFSTRUCT;
typedef OFSTRUCT	    *POFSTRUCT;
typedef OFSTRUCT NEAR	    *NPOFSTRUCT;
typedef OFSTRUCT FAR	    *LPOFSTRUCT;

/* OpenFile() Flags */
#define OF_READ 	    0x0000
#define OF_WRITE	    0x0001
#define OF_READWRITE	    0x0002
#define OF_PARSE	    0x0100
#define OF_DELETE	    0x0200
#define OF_VERIFY	    0x0400
#define OF_CANCEL	    0x0800
#define OF_CREATE	    0x1000
#define OF_PROMPT	    0x2000
#define OF_EXIST	    0x4000
#define OF_REOPEN	    0x8000

int  FAR PASCAL OpenFile(LPSTR, LPOFSTRUCT, WORD);
FARPROC FAR PASCAL SetResourceHandler(HANDLE, LPSTR, FARPROC);

/* Predefined Resource Types */
#define RT_CURSOR	    MAKEINTRESOURCE(1)
#define RT_BITMAP	    MAKEINTRESOURCE(2)
#define RT_ICON 	    MAKEINTRESOURCE(3)
#define RT_MENU 	    MAKEINTRESOURCE(4)
#define RT_DIALOG	    MAKEINTRESOURCE(5)
#define RT_STRING	    MAKEINTRESOURCE(6)
#define RT_FONTDIR	    MAKEINTRESOURCE(7)
#define RT_FONT 	    MAKEINTRESOURCE(8)
#define RT_ACCELERATOR	    MAKEINTRESOURCE(9)
#define RT_RCDATA	    MAKEINTRESOURCE(10)
#define RT_ERRTABLE	    MAKEINTRESOURCE(11)


/*  Help engine section.  */

/* Commands to pass WinHelp() */
#define HELP_CONTEXT	0x0001	 /* Display topic in ulTopic */
#define HELP_QUIT	0x0002	 /* Terminate help */
#define HELP_INDEX	0x0003	 /* Display index */
#define HELP_HELPONHELP 0x0004	 /* Display help on using help */
#define HELP_SETINDEX	0x0005	 /* Set the current Index for multi index help */
#define HELP_KEY	0x0101	 /* Display topic for keyword in offabData */
#define HELP_MULTIKEY   0x0201

BOOL FAR PASCAL WinHelp(HWND hwndMain, LPSTR lpszHelp, WORD usCommand, DWORD ulData);

typedef struct tagMULTIKEYHELP
  {
    WORD    mkSize;
    BYTE    mkKeylist;
    BYTE    szKeyphrase[1];
  } MULTIKEYHELP;

int         FAR PASCAL lstrcmp( LPSTR, LPSTR );
int         FAR PASCAL lstrcmpi( LPSTR, LPSTR );
LPSTR       FAR PASCAL lstrcpy( LPSTR, LPSTR );
LPSTR       FAR PASCAL lstrcat( LPSTR, LPSTR );
int         FAR PASCAL lstrlen( LPSTR );
DWORD FAR PASCAL GetTickCount(void);

void FAR PASCAL GetWindowsDirectory(LPSTR, int);
void FAR PASCAL GetSystemDirectory(LPSTR,int);
HWND FAR PASCAL GetActiveWindow(void);

LONG FAR PASCAL GetWinFlags(void);

#define WF_PMODE	0x0001
#define WF_CPU286	0x0002
#define WF_CPU386	0x0004
#define WF_CPU486	0x0008
#define WF_STANDARD	0x0010
#define WF_WIN286	0x0010
#define WF_ENHANCED	0x0020
#define WF_WIN386	0x0020
#define WF_CPU086	0x0040
#define WF_CPU186	0x0080
#define WF_LARGEFRAME	0x0100
#define WF_SMALLFRAME	0x0200
#define WF_80x87	0x0400

/* SetWindowPos Flags */
#define SWP_NOSIZE	    0x0001
#define SWP_NOMOVE	    0x0002
#define SWP_NOZORDER	    0x0004
#define SWP_NOREDRAW	    0x0008
#define SWP_NOACTIVATE	    0x0010
#define SWP_DRAWFRAME	    0x0020
#define SWP_SHOWWINDOW	    0x0040
#define SWP_HIDEWINDOW	    0x0080
#define SWP_NOCOPYBITS	    0x0100
#define SWP_NOREPOSITION    0x0200

HWND FAR PASCAL GetDlgItem(HWND, int);
void FAR PASCAL SetWindowPos(HWND, HWND, int, int, int, int, WORD);
void FAR PASCAL GetClientRect(HWND, LPRECT);
void FAR PASCAL GetWindowRect(HWND, LPRECT);
void FAR PASCAL ScreenToClient(HWND, LPPOINT);
void FAR PASCAL DebugBreak(void);

HCURSOR FAR PASCAL LoadCursor(HANDLE, LPSTR);
HCURSOR FAR PASCAL CreateCursor(HANDLE, int, int, int, int, LPSTR, LPSTR);
BOOL    FAR PASCAL DestroyCursor(HCURSOR);
int	FAR PASCAL ShowCursor(BOOL);

/* Standard Cursor IDs */
#define IDC_ARROW	    MAKEINTRESOURCE(32512)
#define IDC_IBEAM	    MAKEINTRESOURCE(32513)
#define IDC_WAIT	    MAKEINTRESOURCE(32514)
#define IDC_CROSS	    MAKEINTRESOURCE(32515)
#define IDC_UPARROW	    MAKEINTRESOURCE(32516)
#define IDC_SIZE	    MAKEINTRESOURCE(32640)
#define IDC_ICON	    MAKEINTRESOURCE(32641)
#define IDC_SIZENWSE	    MAKEINTRESOURCE(32642)
#define IDC_SIZENESW	    MAKEINTRESOURCE(32643)
#define IDC_SIZEWE	    MAKEINTRESOURCE(32644)
#define IDC_SIZENS	    MAKEINTRESOURCE(32645)

/*  added by lins to support DocInfo Escape */

typedef struct {
    short   cbSize;
    LPSTR   lpszDocName;
    LPSTR   lpszOutput;
    }	DOCINFO, FAR * LPDOCINFO;

int API StartDoc(HDC, LPDOCINFO);
int API StartPage(HDC);
int API EndPage(HDC);
int API EndDoc(HDC);
int API SetAbortProc(HDC, FARPROC);
int API AbortDoc(HDC);

// add more for F1 key help support.
// Peter: please clean it up by using "print.h".

BOOL	API GetMessage(MSG FAR*, HWND, WORD, WORD);

BOOL	API PostMessage(HWND, WORD, WORD, LONG);

#define WH_MSGFILTER	    (-1)

#define MSGF_DIALOGBOX		 0

typedef FARPROC HOOKPROC;
HOOKPROC API SetWindowsHook(int, HOOKPROC);
BOOL	API UnhookWindowsHook(int, HOOKPROC);
DWORD	API DefHookProc(int, WORD, DWORD, HOOKPROC FAR *);

#define WM_KEYDOWN	    0x0100
#define VK_F1		    0x70

HWND	API GetParent(HWND);
WORD API RegisterWindowMessage(LPSTR);
int API MulDiv(int, int, int);

typedef WORD                ATOM;
ATOM	API GlobalAddAtom(LPSTR);
ATOM	API GlobalDeleteAtom(ATOM);
ATOM	API GlobalFindAtom(LPSTR);
WORD  API GlobalGetAtomName(ATOM, LPSTR, int);
