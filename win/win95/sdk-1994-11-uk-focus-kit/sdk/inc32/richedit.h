/*
 *	RICHEDIT.H
 *	
 *	Purpose:
 *		RICHEDIT public definitions
 *	
 *	Owner:
 *		David R. Fulmer
 */

#ifndef _RICHEDIT_
#define	_RICHEDIT_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define cchTextLimitDefault 32767

/* RichEdit messages */

#ifndef WM_CONTEXTMENU
#define WM_CONTEXTMENU			0x007B
#endif

#ifndef EM_GETLIMITTEXT
#define EM_GETLIMITTEXT			(WM_USER + 37)
#endif

#ifndef EM_POSFROMCHAR	
#define EM_POSFROMCHAR			(WM_USER + 38)
#define EM_CHARFROMPOS			(WM_USER + 39)
#endif

#ifndef EM_SCROLLCARET
#define EM_SCROLLCARET			(WM_USER + 49)
#endif
#define EM_CANPASTE				(WM_USER + 50)
#define EM_DISPLAYBAND			(WM_USER + 51)
#define EM_EXGETSEL				(WM_USER + 52)
#define EM_EXLIMITTEXT			(WM_USER + 53)
#define EM_EXLINEFROMCHAR		(WM_USER + 54)
#define EM_EXSETSEL				(WM_USER + 55)
#define EM_FINDTEXT				(WM_USER + 56)
#define EM_FORMATRANGE			(WM_USER + 57)
#define EM_GETCHARFORMAT		(WM_USER + 58)
#define EM_GETEVENTMASK			(WM_USER + 59)
#define EM_GETOLEINTERFACE		(WM_USER + 60)
#define EM_GETPARAFORMAT		(WM_USER + 61)
#define EM_GETSELTEXT			(WM_USER + 62)
#define EM_HIDESELECTION		(WM_USER + 63)
#define EM_PASTESPECIAL			(WM_USER + 64)
#define EM_REQUESTRESIZE		(WM_USER + 65)
#define EM_SELECTIONTYPE		(WM_USER + 66)
#define EM_SETBKGNDCOLOR		(WM_USER + 67)
#define EM_SETCHARFORMAT		(WM_USER + 68)
#define EM_SETEVENTMASK			(WM_USER + 69)
#define EM_SETOLECALLBACK		(WM_USER + 70)
#define EM_SETPARAFORMAT		(WM_USER + 71)
#define EM_SETTARGETDEVICE		(WM_USER + 72)
#define EM_STREAMIN				(WM_USER + 73)
#define EM_STREAMOUT			(WM_USER + 74)
#define EM_GETTEXTRANGE			(WM_USER + 75)
#define EM_FINDWORDBREAK		(WM_USER + 76)
#define EM_SETOPTIONS			(WM_USER + 77)
#define EM_GETOPTIONS			(WM_USER + 78)
#ifdef DBCS
#define EM_SETPUNCTUATION		(WM_USER + 100)
#define EM_GETPUNCTUATION		(WM_USER + 101)
#define EM_SETWORDWRAPMODE		(WM_USER + 102)
#define EM_GETWORDWRAPMODE		(WM_USER + 103)
#define EM_SETWRITINGMODE		(WM_USER + 104)
#endif

/* new notifications */

#define EN_MSGFILTER			0x0700
#define EN_REQUESTRESIZE		0x0701
#define EN_SELCHANGE			0x0702
#define EN_DROPFILES			0x0703
#define EN_PROTECTED			0x0704
/* PenWin specific */
#define EN_CORRECTTEXT			0x0705
/* back to new notifications */
#define EN_STOPNOUNDO			0x0706

/* event notification masks */

#define ENM_NONE				0x00000000
#define ENM_CHANGE				0x00000001
#define ENM_UPDATE				0x00000002
#define ENM_SCROLL				0x00000004
#define ENM_KEYEVENTS			0x00010000
#define ENM_MOUSEEVENTS			0x00020000
#define ENM_REQUESTRESIZE		0x00040000
#define ENM_SELCHANGE			0x00080000
#define ENM_DROPFILES			0x00100000
#define ENM_PROTECTED			0x00200000
/* PenWin specific */
#define ENM_CORRECTTEXT			0x00400000

/* new edit control styles */

#define ES_SAVESEL				0x00008000
#define ES_SUNKEN				0x00004000
#define ES_DISABLENOSCROLL		0x00002000
/* same as WS_MAXIMIZE, but that doesn't make sense so we re-use the value */
#define ES_SELECTIONBAR			0x01000000

/* These flag are used in the only FE Windows. */
//$ REVIEW:No Problem??? the following value is same as...
//			WS_DLGFRAME, WS_SYSMENU, WS_POPUP, WS_THICKFRAME...
#ifdef DBCS
#define ES_VERTICAL				0x00020000
#define	ES_NOIME				0x00080000
#define ES_SELFIME				0x00040000
#define ES_NOWORDWRAP			0x00010000
#else
// This is for US build, becasue US resource has FE ID in resource.
#define ES_VERTICAL				0	
#define	ES_NOIME				0
#define ES_SELFIME				0
#define ES_NOWORDWRAP			0
#endif

/* edit control options */
#define ECO_AUTOWORDSELECTION	0x00000001
#define ECO_AUTOVSCROLL			0x00000040
#define ECO_AUTOHSCROLL			0x00000080
#define ECO_NOHIDESEL			0x00000100
#define ECO_READONLY			0x00000800
#define ECO_WANTRETURN			0x00001000
#define ECO_SAVESEL				0x00008000
#define ECO_SELECTIONBAR		0x01000000
/* used only in FE Windows version */
#define ECO_VERTICAL			0x00020000

/* ECO operations */
#define ECOOP_SET				0x0001
#define ECOOP_OR				0x0002
#define ECOOP_AND				0x0003
#define ECOOP_XOR				0x0004

/* new word break function actions */
#define WB_CLASSIFY			3
#define WB_MOVEWORDLEFT		4
#define WB_MOVEWORDRIGHT	5
#define WB_LEFTBREAK		6
#define WB_RIGHTBREAK		7

/* Far East specific flags */
#define WB_MOVEWORDPREV		4
#define WB_MOVEWORDNEXT		5
#define WB_PREVBREAK		6
#define WB_NEXTBREAK		7

/* This is defined in the gapi.h
#ifdef DBCS
#define PC_FOLLOWING		1
#define	PC_LEADING			2
#define	PC_OVERFLOW			3
#define	PC_DELIMITER		4
#define WBF_WORDWRAP		0x010
#define WBF_WORDBREAK		0x020
#define	WBF_OVERFLOW		0x040	
#define WBF_LEVEL1			0x080
#define	WBF_LEVEL2			0x100
#define	WBF_CUSTOM			0x200
#endif
*/

/* word break flags (used with WB_CLASSIFY) */
#define WBF_CLASS			((BYTE) 0x0F)
#define WBF_ISWHITE			((BYTE) 0x10)
#define WBF_BREAKLINE		((BYTE) 0x20)
#define WBF_BREAKAFTER		((BYTE) 0x40)


/* new IME control messages */
#define IMC_SETCOMPOSITIONCOLOR	0x100
#define	IMC_GETCOMPOSITIONCOLOR	0x101



/* new data types */

/* all character format measurements are in twips */
typedef struct _charformat
{
	UINT		cbSize;
	DWORD		dwMask;
	DWORD		dwEffects;
	LONG		yHeight;
	LONG		yOffset;			/* > 0 for superscript, < 0 for subscript */
	COLORREF	crTextColor;
	BYTE		bCharSet;
	BYTE		bPitchAndFamily;
	TCHAR		szFaceName[LF_FACESIZE];
} CHARFORMAT;

/* CHARFORMAT masks */
#define CFM_BOLD		0x00000001
#define CFM_ITALIC		0x00000002
#define CFM_UNDERLINE	0x00000004
#define CFM_STRIKEOUT	0x00000008
#define CFM_PROTECTED	0x00000010
#define CFM_SIZE		0x80000000
#define CFM_COLOR		0x40000000
#define CFM_FACE		0x20000000
#define CFM_OFFSET		0x10000000
#define CFM_CHARSET		0x08000000

/* CHARFORMAT effects */
#define CFE_BOLD		0x0001
#define CFE_ITALIC		0x0002
#define CFE_UNDERLINE	0x0004
#define CFE_STRIKEOUT	0x0008
#define CFE_PROTECTED	0x0010
/* NOTE: CFE_AUTOCOLOR corresponds to CFM_COLOR, which controls it */
#define CFE_AUTOCOLOR	0x40000000

#define yHeightCharPtsMost 1638

/* EM_SETCHARFORMAT wParam masks */
#define SCF_SELECTION	0x0001
#define SCF_WORD		0x0002

typedef struct _charrange
{
	LONG	cpMin;
	LONG	cpMax;
} CHARRANGE;

typedef struct _textrange
{
	CHARRANGE chrg;
	LPSTR lpstrText;	/* allocated by caller, zero terminated by RichEdit */
} TEXTRANGE;

typedef DWORD (CALLBACK *EDITSTREAMCALLBACK)(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

typedef struct _editstream
{
	DWORD dwCookie;		/* user value passed to callback as first parameter */
	DWORD dwError;		/* last error */
	EDITSTREAMCALLBACK pfnCallback;
} EDITSTREAM;

/* stream formats */

#define SF_TEXT			0x0001
#define SF_RTF			0x0002
#define SF_RTFNOOBJS	0x0003		/* outbound only */
#define SF_TEXTIZED		0x0004		/* outbound only */

/* Flag telling stream operations to operate on the selection only */
/* EM_STREAMIN will replace the current selection */
/* EM_STREAMOUT will stream out the current selection */
#define SFF_SELECTION	0x8000

typedef struct _findtext
{
	CHARRANGE chrg;
	LPSTR lpstrText;
} FINDTEXT;

typedef struct _formatrange
{
	HDC hdc;
	HDC hdcTarget;
	RECT rc;
	RECT rcPage;
	CHARRANGE chrg;
} FORMATRANGE;

/* all paragraph measurements are in twips */

#define MAX_TAB_STOPS 32
#define lDefaultTab 720

typedef struct _paraformat
{
	UINT	cbSize;
	DWORD	dwMask;
	WORD	wNumbering;
	WORD	wReserved;
	LONG	dxStartIndent;
	LONG	dxRightIndent;
	LONG	dxOffset;
	WORD	wAlignment;
	SHORT	cTabCount;
	LONG	rgxTabs[MAX_TAB_STOPS];
} PARAFORMAT;

/* PARAFORMAT mask values */
#define PFM_STARTINDENT			0x00000001
#define PFM_RIGHTINDENT			0x00000002
#define PFM_OFFSET				0x00000004
#define PFM_ALIGNMENT			0x00000008
#define PFM_TABSTOPS			0x00000010
#define PFM_NUMBERING			0x00000020
#define PFM_OFFSETINDENT		0x80000000

/* PARAFORMAT numbering options */
#define PFN_BULLET		0x0001

/* PARAFORMAT alignment options */
#define PFA_LEFT	0x0001
#define PFA_RIGHT	0x0002
#define PFA_CENTER	0x0003

/* notification structures */

#ifndef WM_NOTIFY
#define WM_NOTIFY				0x004E

typedef struct _nmhdr
{
	HWND	hwndFrom;
	UINT	idFrom;
	UINT	code;
} NMHDR;
#endif  /* !WM_NOTIFY */

typedef struct _msgfilter
{
	NMHDR nmhdr;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
} MSGFILTER;

typedef struct _reqresize
{
	NMHDR nmhdr;
	RECT rc;
} REQRESIZE;

typedef struct _selchange
{
	NMHDR nmhdr;
	CHARRANGE chrg;
	WORD seltyp;
} SELCHANGE;

#define SEL_EMPTY		0x0000
#define SEL_TEXT		0x0001
#define SEL_OBJECT		0x0002
#define SEL_MULTICHAR	0x0004
#define SEL_MULTIOBJECT	0x0008

typedef struct _endropfiles
{
	NMHDR nmhdr;
	HANDLE hDrop;
	LONG cp;
	BOOL fProtected;
} ENDROPFILES;

typedef struct _enprotected
{
	NMHDR nmhdr;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
	CHARRANGE chrg;
} ENPROTECTED;

/* PenWin specific */
typedef struct _encorrecttext
{
	NMHDR nmhdr;
	CHARRANGE chrg;
	WORD seltyp;
} ENCORRECTTEXT;

#ifdef DBCS
typedef struct _punctuation
{
	UINT	iSize;
	LPSTR	szPunctuation;
} PUNCTUATION;

typedef struct _compcolor
{
	COLORREF crText;
	COLORREF crBackground;
	DWORD dwEffects;
}COMPCOLOR;

#endif

/* clipboard formats - use as parameter to RegisterClipboardFormat() */
#define CF_RTF "Rich Text Format"
#define CF_RETEXTOBJ "RichEdit Text and Objects"

/* Paste Special */
typedef struct _repastespecial
{
	DWORD	dwAspect;
	DWORD	dwParam;
} REPASTESPECIAL;

#ifdef MAC
BOOL WINAPI EntryFunc_RICHEDIT(HMODULE hmod, DWORD dwReason, LPVOID lpvReserved);
#endif

#if defined(CHICAGO)
void WINAPI RichEditSetMultiThreaded(void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !_RICHEDIT_ */
