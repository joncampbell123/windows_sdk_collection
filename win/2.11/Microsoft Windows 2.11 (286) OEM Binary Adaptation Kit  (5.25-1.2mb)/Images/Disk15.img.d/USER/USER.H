/* include defs:
    NOMENUS
    NOSYSSTRUCTS
    NOCARET

   Include defs:
    RESOURCESTRINGS
*/
#define VK_ALTERNATE VK_MENU

#define VER 0x201
#define PMVER 0x0200
#define PASCAL
#define WIN_INTERNAL

/* Window class structure */
typedef struct cls {
    struct cls *pclsNext;
    unsigned clsMagic;
    unsigned atomClassName;
    char *pdce;                 /* DCE * to dc associated with class */
/* The remainder of this structure is the same as the WNDCLASS structure
   defined in windows.h (with a few renamed field names).  This order is
   ASSUMED by GetClassPtr and CreateWindow */
    unsigned style;
    long (far *lpfnWndProc)();
    int  cbclsExtra;
    int  cbwndExtra;
    char *hInstance;
    char *hIcon;
    char *hCursor;
    char *hbrBackground;
    char far *lpszMenuName;
    char far *lpszClassName;
/* end of ordered fields */
} CLS;
typedef CLS *PCLS;
typedef CLS far *LPCLS;

/* offset from beginning of CLS to start of WNDCLASS section.
   Used by TestCF macro */
#define CFOFFSET (sizeof(CLS) - sizeof(WNDCLASS))

typedef struct {
    int left, top, right, bottom;
} RECT;

/* Window instance structure */
typedef struct wnd {
    struct wnd *hwndNext;   /* handle to the next window                         */
    struct wnd *hwndChild;  /* Handle to child                                   */
    PCLS pcls;              /* pointer to window class                            */
    char *hProp;            /* handle to the first property in the property list */
    unsigned char cLock;    /* LockWindow count */
    unsigned char cRegDestroy;  /* Destroy register count */
    int *rgwScroll;         /* Words used for scrolling */
    char *hq;               /* queue handle */
    char *hrgnUpdate;       /* accumulated paint region */
    char *pdce;             /* DCE * of own DC if present */
    char *hSysMenu;         /* handle to system menu */
    struct wnd *hwndOwner;  /* Popup window owner field */
    RECT rcWindow;          /* Window outer rectangle */
    RECT rcClient;          /* Client rectangle       */

/* Order of following fields is assumed */
    unsigned state;         /* state flags */
    unsigned long style;    /* style flags */
    char *hMenu;            /* menu handle or ID */
    char *szName;
    struct wnd *hwndParent; /* Backpointer to parent window (children) */
    char *hInstance;        /* handle to module instance data */
    long (far *lpfnWndProc)();
/* end of ordered fields */
} WND;
typedef WND *HWND;

/* NOTE!!! The following is the offset into the window structure of the
   state field.  Be sure to change this definition when the WND
   structure changes.  This hack is used to make the compiler generate
   decent code for the Set/Clr/TestWF macros */

#define STATEOFFSET \
        (sizeof(WND) - sizeof(FARPROC) - sizeof(char *) - sizeof(int) - \
        sizeof(char *) - sizeof(char *) - sizeof(unsigned long) - sizeof(unsigned))

/* Window flag masks

   Hi byte of word is byte index, low byte is mask
   These masks assume the order of the state and style fields of a
   window instance structure.

*/

/* Window styles */
#define WFTYPEMASK      0x05c0
#define WFTILED         0x0500
#define WFICONICPOPUP   0x05c0
#define WFPOPUP         0x0580
#define WFCHILD         0x0540
#define WFMINIMIZED     0x0520
#define WFVISIBLE       0x0510
#define WFDISABLED      0x0508
#define WFDISABLE       WFDISABLED
#define WFCLIPSIBLINGS  0x0504
#define WFCLIPCHILDREN  0x0502
#define WFMAXIMIZED     0x0501
#define WFCLIPCHILD     WFCLIPCHILDREN
#define WFOVERLAP       WFCLIPSIBLINGS
#define WFICONIC        WFMINIMIZED

#define WFMINBOX        0x0402
#define WFMAXBOX        0x0401

#define WFBORDERMASK    0x04c0
#define WFBORDER        0x0480
#define WFCAPTION       0x04c0
#define WFDLGFRAME      0x0440
#define WFTOPLEVEL      0x0440

#define WFVSCROLL       0x0420
#define WFHSCROLL       0x0410
#define WFSYSMENU       0x0408
#define WFSIZEBOX       0x0404
#define WFGROUP         0x0402
#define WFTABSTOP       0x0401

/* Class styles */
#define CFVREDRAW       0x0001
#define CFHREDRAW       0x0002
#define CFKANJIWINDOW   0x0004
#define CFDBLCLKS       0x0008
#define CFOEMCHARS      0x0010
#define CFOWNDC         0x0020
#define CFCLASSDC       0x0040
#define CFPARENTDC      0x0080
#define CFNOKEYCVT      0x0101
#define CFNOCLOSE       0x0102
#define CFLVB           0x0104
#define CFCLSDC         CFCLASSDC
#define CFSAVEPOPUPBITS 0x0108
#define CFBYTEALIGNCLIENT 0x0110
#define CFBYTEALIGNWINDOW 0x0120


/* State flags */

#define WFMPRESENT      0x0001
#define WFVPRESENT      0x0002
#define WFHPRESENT      0x0004
#define WFCPRESENT      0x0008
#define WFSENDSIZEMOVE  0x0010
#define WFNOPAINT       0x0020
#define WFFRAMEON       0x0040
#define WFHASSPB        0x0080
#define WFNONCPAINT	0x0101
#define WFSENDERASEBKGND 0x0102
#define WFERASEBKGND    0x0104
#define WFFRAMECHILDREN 0x0108
#define WFUNUSED5       0x0110
#define WFUPDATEDIRTY   0x0120
#define WFHIDDENPOPUP   0x0140
#define WFMENUDRAW      0x0180

/* ActivateWindow() commands */
#define AW_USE       1
#define AW_TRY       2
#define AW_SKIP      3
#define AW_TRY2      4
#define AW_USE2      5

/* Class styles */

/**  HACK ALERT

   The low byte of the WF?PRESENT state flags must NOT be the
   same as the low byte of the WFBORDER and WFCAPTION flags,
   since these are used as paint hint masks.  The masks are calculated with
   the MaskWF macro below.

*/
#define TestWF(hwnd, flag) ((BYTE)*((BYTE *)hwnd + STATEOFFSET + HIBYTE(flag)) & (BYTE)LOBYTE(flag))
#define SetWF(hwnd, flag)  ((BYTE)*((BYTE *)hwnd + STATEOFFSET + HIBYTE(flag)) |= (BYTE)LOBYTE(flag))
#define ClrWF(hwnd, flag)  ((BYTE)*((BYTE *)hwnd + STATEOFFSET + HIBYTE(flag)) &= ~(BYTE)LOBYTE(flag))
#define MaskWF(flag)       ((WORD)( (HIBYTE(flag) & 1) ? LOBYTE(flag) << 8 : LOBYTE(flag)) )

#define TestCF(hwnd, flag)  (*((BYTE *)hwnd->pcls + CFOFFSET + HIBYTE(flag)) & LOBYTE(flag))
#define SetCF(hwnd, flag)  (*((BYTE *)hwnd->pcls + CFOFFSET + HIBYTE(flag)) |= LOBYTE(flag))
#define TestCF2(pcls, flag)  (*((BYTE *)pcls + CFOFFSET + HIBYTE(flag)) & LOBYTE(flag))
#define SetCF2(pcls, flag)  (*((BYTE *)pcls + CFOFFSET + HIBYTE(flag)) |= LOBYTE(flag))

#define TestwndChild(hwnd) (TestWF(hwnd, WFTYPEMASK) == (BYTE)LOBYTE(WFCHILD))
#define TestwndTiled(hwnd) (TestWF(hwnd, WFTYPEMASK) == (BYTE)LOBYTE(WFTILED))
#define TestwndIPopup(hwnd) (TestWF(hwnd, WFTYPEMASK) == (BYTE)LOBYTE(WFICONICPOPUP))
#define TestwndNIPopup(hwnd) (TestWF(hwnd, WFTYPEMASK) == (BYTE)LOBYTE(WFPOPUP))
#define TestwndPopup(hwnd) (TestwndNIPopup(hwnd) || TestwndIPopup(hwnd))
#define TestwndHI(hwnd) (TestwndTiled(hwnd) || TestwndIPopup(hwnd))

/* Internal window class names */
#define MENUCLASS       0x8000
#define DESKTOPCLASS    0x8001
#define DIALOGCLASS     0x8002
#define MESSAGEBOXCLASS 0x8003
#define ICONNAMECLASS   0x8004

/* ZOrdering() return values */
#define ZO_ERROR        (-1)
#define ZO_EQUAL        0
#define ZO_DISJOINT     1
#define ZO_ABOVE        2
#define ZO_BELOW        3

/* Null string */
extern char far szNull[];
/* Single character string ('0') */
extern char far szOneChar;

#ifdef WIN_INTERNAL
    #ifndef CTLMGR
    #define NOCTLMGR
    #endif
    #ifndef GDICAPMASKS
    #define NOGDICAPMASKS
    #endif
#endif

typedef char *HANDLE;

#define NOSOUND
#define NOCOMM
#include "windows.h"
#include "winexp.h"

#define APP_FIXED_FONT  12

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
int min(int, int);
int max(int, int);


typedef struct {
    int yMsgText;
    int yBtnText;
    int len;
    int style;
    int lEdge;
    int cxText;
    int cyText;
    char *pszMBText;
    RECT rc1;
    RECT rc2;
    RECT rc3;
    WORD flags;
    HWND hwndOwner;
    BOOL fEnd;
    int result;
    BOOL fMbText;
} MBS;


typedef struct BWL {
    struct BWL *pbwlNext;
    HWND *phwndMax;
    HWND rghwnd[1];
} BWL;
typedef BWL *PBWL;

typedef struct {
    DWORD semaphore;
    char *hqOwner;
    BYTE cBusy;
    BYTE bOrder;
} SEMAPHORE;
typedef SEMAPHORE FAR *LPSEM;

/* Secretly exported Kernel routines */
HANDLE FAR GetTaskQueue(HANDLE);
HANDLE FAR SetTaskQueue(HANDLE, HANDLE);
void   FAR LockCurrentTask(BOOL);
WORD   FAR GlobalSetID( HANDLE, WORD );
void   FAR DosSemSet(LPSEM);
void   FAR DosSemWait(LPSEM, DWORD);
void   FAR DosSemClear(LPSEM);

/* Secretly exported GDI routines */
short FAR IntersectVisRect(HDC, short, short, short, short);
short FAR ExcludeVisRect(HDC, short, short, short, short);
long  FAR SetDCOrg(HDC, short, short);
DWORD FAR GetDCOrg(HDC);
int   FAR GetRgnBox(HRGN, LPRECT);
short FAR SelectVisRgn(HDC, HANDLE);
short FAR SaveVisRgn(HDC);
short FAR RestoreVisRgn(HDC);
HRGN  FAR InquireVisRgn(HDC);
void  FAR SetRectRgn(HRGN, short, short, short, short);

typedef HANDLE HDCS;

HDCS FAR PASCAL GetDCState(HDC);
BOOL FAR PASCAL SetDCState(HDC, HDCS);

/* GDI LVB routines */
HDC     FAR CreateLVB();
HBITMAP FAR SelectLVB(HDC, HANDLE, BOOL);
BOOL    FAR UpdateLVB(HDC);
BOOL    FAR EnableLVB(HDC, BOOL);
BOOL    FAR IsDCDirty(HDC, LPRECT);
BOOL    FAR SetDCStatus(HDC, BOOL, LPRECT);

/* Routine in separate segments */
FAR SwapHandle(WORD *);
FAR DrawCaret(HDC);
FAR DestroyAllWindows(void);

/* Suppport routines for sep seg stuff */
void FAR FarLCopyStruct();
int  FAR FarMultDiv();
HDC  FAR FarGetScreenDc();
BOOL FAR FarReleaseADc(HDC);
HDC  FAR FarGetClientDc();
long XCallWindowProc(HWND, unsigned, WORD, LONG);
HANDLE FAR FarHqCurrent(void);
HANDLE HqCurrent(void);
WORD FAR GetAppVer(void);
BOOL FAR LockWindowVerChk(HWND);
BOOL FAR ReleaseFrameDC(WORD FAR *);
void FAR FarOffsetChildren();

PBWL FAR BuildHwndList(HWND, int);
#define BWL_ENUMCHILDREN    1
#define BWL_ENUMLIST        2

void FAR FreeHwndList(PBWL);

/* Utility routines */
/* these two take SS relative pointers */
void CopyWindowRect(HWND, RECT *);
void CopyClientRect(HWND, RECT *);

/* Internal memory allocation routines */
HANDLE     XAllocH(int);
HANDLE     XAllocP(int);
HANDLE     XReAllocH(HANDLE, int);
HANDLE     XFreeH(HANDLE);
char *     XLockH(HANDLE);
BOOL       XUnlockH(HANDLE);

/* Semaphore ID's and macros */
/* Note: Read/Write semaphores take two slots */
#define SEM_PROPLIST     0   /* propery list management */
#define SEM_TIMER        1   /* timer code */
#define SEM_WINHOOK      2   /* windows hooks */
#define SEM_DRAWTEXT     3   /* used for DrawText */
#define SEM_SCROLL       4   /* used for scroll window */
#define SEM_LINK         5   /* used for linking / unlinking lists */
#define SEM_CLIPBOARD    6   /* some clipboard routines */
#define SEM_MENU         7   /* menu service routine globals */
#define SEM_CURSOR       8   /* cursor showing/hiding/setting routines */
#define SEM_CRIT         9   /* critical section semaphore */
#define SEM_WNDLOCK      10  /* window locking */
#define SEM_CACHEDC      11  /* Cache DC */
#define SEM_CLASSDC      12  /* Class DC */
#define SEM_SPB          13  /* SPB list */
#define SEM_VISRGN       14  /* window visrgns */
#define SEM_SB           15  /* Scroll Bars */
#define SEM_SEMRW        16  /* Processing Read/Write semaphores */
#define SEMRW_GDIOBJECTS 17  /* R/W: hdcBits & sysClrObjects */
#define SEMRW_GLOBAL     19  /* R/W: test, sets, and calls around globals */
#define SEMRW_DC         21  /* R/W: DCE manipulation */
#define SEMRW_RECT       23  /* R/W: hwnd->rcWindow and hwnd->rcClient */
#define SEMRW_LOCKWINDOW 25  /* R/W: LockAllWindows and WindowLockStart */
#define SEMRW_UPDATERGN  27  /* R/W: Window update region manipulation */
#define CSEMAPHORES      29

#ifdef SEM

void FAR FarSemaphoreEnter(int);
void FAR FarSemaphoreLeave(int);
void FAR FarSemaphoreWriteEnter(int);
void FAR FarSemaphoreReadEnter(int);
void FAR FarSemaphoreWriteLeave(int);
void FAR FarSemaphoreReadLeave(int);
void FAR CheckSemaphoreEntered(int);
void FAR FarLockAllWindows2(void);
void FAR FarUnlockAllWindows2(void);

#ifdef DEBUG
void far CheckSemaphoreEntered(int);
int far FInSem(int);
#define CheckSemEntered(i) CheckSemaphoreEntered(i)
#else
#define CheckSemEntered(i)
#endif

#define SemEnter(i) SemaphoreEnter(i)
#define SemLeave(i) SemaphoreLeave(i)
#define FarSemEnter(i) FarSemaphoreEnter(i)
#define FarSemLeave(i) FarSemaphoreLeave(i)
#define SemReadEnter(i) SemaphoreReadEnter(i)
#define FarSemReadEnter(i) FarSemaphoreReadEnter(i)
#define SemReadLeave(i) SemaphoreReadLeave(i)
#define FarSemReadLeave(i) FarSemaphoreReadLeave(i)
#define SemWriteEnter(i) SemaphoreWriteEnter(i)
#define FarSemWriteEnter(i) FarSemaphoreWriteEnter(i)
#define SemWriteLeave(i) SemaphoreWriteLeave(i)
#define FarSemWriteLeave(i) FarSemaphoreWriteLeave(i)

#define SemDcReadEnter() SemaphoreDcReadEnter()
#define SemRectReadEnter() SemaphoreRectReadEnter()
#define SemGlobalReadEnter() SemaphoreGlobalReadEnter()
#define SemGdiObjectsReadEnter() SemaphoreGdiObjectsReadEnter()
#define SemDcReadLeave() SemaphoreDcReadLeave()
#define SemRectReadLeave() SemaphoreRectReadLeave()
#define SemGlobalReadLeave() SemaphoreGlobalReadLeave()
#define SemGdiObjectsReadLeave() SemaphoreGdiObjectsReadLeave()


#else

#define CheckSemEntered(i)
#define SemEnter(i)
#define SemLeave(i)
#define FarSemEnter(i)
#define FarSemLeave(i)
#define SemReadEnter(i)
#define FarSemReadEnter(i)
#define SemReadLeave(i)
#define FarSemReadLeave(i)
#define SemWriteEnter(i)
#define FarSemWriteEnter(i)
#define SemWriteLeave(i)
#define FarSemWriteLeave(i)

#define SemDcReadEnter()
#define SemRectReadEnter()
#define SemGlobalReadEnter()
#define SemGdiObjectsReadEnter()
#define SemDcReadLeave()
#define SemRectReadLeave()
#define SemGlobalReadLeave()
#define SemGdiObjectsReadLeave()

#define UserLockWindow(hwnd) (hwnd)
#define UserUnlockWindow(hwnd) (hwnd)
#define WindowLockStart(hwnd)
#define WindowLockEnd()
#define LockAllWindows()
#define FarLockAllWindows()
#define UnlockAllWindows()
#define FarUnlockAllWindows()

#endif

/* Window handle validation stuff */
BOOL far ValidateWindow(HWND);
BOOL far ValidateWindowNull(HWND);
#define CheckHwnd(hwnd)     ValidateWindow(hwnd)
#define CheckHwndNull(hwnd) ValidateWindowNull(hwnd)

#define AllocH(cb)      XAllocH(cb)
#define AllocP(cb)      XAllocP(cb)
#define ReAllocH(h, cb) XReAllocH(h, cb)
#define ReallocH(h, cb) ReAllocH(h, cb)
#define FreeH(h)        XFreeH(h)
#define FreeP(h)        XFreeH(h)
#define FreePG(h)       GlobalFree(h)
#define FreeHG(h)       GlobalFree(h)
#define LockH(h)        XLockH(h)
#define UnlockH(h)      XUnlockH(h)
#define LockHG(handle)          GlobalLock(handle)
#define UnlockHG(handle)        GlobalUnlock(handle)
#define ReAllocP(handle, cnt)   LocalReAlloc(handle, cnt, LMEM_MOVEABLE+LMEM_ZEROINIT)
#define ReallocPG(handle, cnt)  GlobalReAlloc(handle, cnt, GMEM_MOVEABLE+GMEM_ZEROINIT)
#define ReAllocPG(handle, cnt)  ReallocPG(handle, cnt)
#define ReallocHG(handle, cnt)  GlobalReAlloc(handle, cnt, GMEM_MOVEABLE+GMEM_ZEROINIT)
#define ReAllocHG(handle, cnt)  ReallocHG(handle, cnt)
#define AllocPG(cnt)            GlobalAlloc(GPTR, cnt)
#define AllocHG(cnt)            GlobalAlloc(GPTR, cnt)

/* Special global memory share bit */
#define GMEM_SHAREALL       0x2000
/* #define GMEM_SHARE	       0x1000  */
#define GMEM_LOWER	    0x1000

/* GDI raster ops */
#define srcCopy     SRCCOPY
#define srcPaint    SRCPAINT
#define srcAnd      SRCAND
#define srcInvert   SRCINVERT
#define srcErase    SRCERASE
#define notSrcCopy  NOTSRCCOPY
#define notSrcErase NOTSRCERASE
#define mergeCopy   MERGECOPY
#define mergePaint  MERGEPAINT
#define patCopy     PATCOPY
#define patPaint    PATPAINT
#define patInvert   PATINVERT
#define dstInvert   DSTINVERT

#define dpHorzRes   HORZRES
#define dpVertRes   VERTRES

#define XCOORD(l)   ((int)LOWORD(l))
#define YCOORD(l)   ((int)HIWORD(l))

/* Structure filled in by WINWhere */
typedef struct {
    HWND hwnd;
    int  iPart;
} WNDPOS;

typedef struct {
    WORD    fmt;
    HANDLE  hData;
    HANDLE  CopyHandle;
    HANDLE  CopyAnotherHandle;
} CLIP;

typedef CLIP *PCLIP;

#define LockLStr(a)  (PSTR)LockH(a)
#define LockGStr(a)  (LPSTR)LockGH(a)

#define UnLockStr(a)    UnlockH(a)

#ifndef NOSYSSTRUCTS
typedef struct event {
    unsigned message;
    WORD paramL;
    WORD paramH;
    DWORD time;
} SYSMSG;

typedef struct {
    long resolution;
} TIMERINFO;

typedef struct {
    BYTE  Begin_First_range;
    BYTE  End_First_range;
    BYTE  Begin_Second_range;
    BYTE  End_Second_range;
    short stateSize;
    short numLights;
    short hasBreak;
    short rate;
} KBINFO;

typedef struct {
    char  fExist;
    char  fRelative;
    short cButton;
    short cmsRate;
    short xThreshold;
    short yThreshold;
    short cxResolution;     /* resolution needed for absolute mouse coordinate */
    short cyResolution;
} MOUSEINFO;

typedef struct {
    short   csXRate;
    short   csYRate;
} CURSORINFO;

typedef struct {
    short xHotSpot;
    short yHotSpot;
    short cx;
    short cy;
    short cbWidth;  /* Bytes per row, accounting for word alignment. */
    short clr;
} CURSORSHAPE;
typedef CURSORSHAPE *PCURSORSHAPE;
typedef CURSORSHAPE FAR * LPCURSORSHAPE;

typedef struct {
    int     iIconCurrent;
    int     fHeightChange;
    int     crw;        /*current no of rows            */
    int     cIconInRow;       /*maximum icons in a row        */
    int     cIcon;
    int     wEvent;
} ICONINFO;

#endif


/***************** Stuff related to color   **********************/
/* PHILHE: boosted system colors from 10 to 12 by adding
 * support for in/active borders.
 */
/* increased from 12 to 13 to support AppWorkspace color for MDI apps. davidhab */
#define CSYSCOLORS      13

/* These flags have to reconstructed if the "syscolors" structurs
   changes.
*/
#define     ALL_REPAINT     0x120
#define     FRAME_REPAINT   0x2dd
#define     DESK_REPAINT    0x002


/* PHILHE: added last two brush handles for in/active borders.
 */
typedef struct {
    HBRUSH      hbrScrollbar;
    HBRUSH      hbrDesktop;
    HBRUSH      hbrActiveCaption;
    HBRUSH      hbrInactiveCaption;
    HBRUSH      hbrMenu;
    HBRUSH      hbrWindow;
    HBRUSH      hbrCaption;
    HBRUSH      hbrMenuText;
    HBRUSH      hbrWindowText;
    HBRUSH      hbrCaptionText;
    HBRUSH      hbrActiveBorder;
    HBRUSH      hbrInactiveBorder;
    HBRUSH      hbrAppWorkspace;
} SYSCLROBJECTS;

/* PHILHE: added last two system colors for in/active borders.
 */
typedef struct sysclrs {
    long        clrScrollbar;
    long        clrDesktop;
    long        clrActiveCaption;
    long        clrInactiveCaption;
    long        clrMenu;
    long        clrWindow;
    long        clrCaption;
    long        clrMenuText;
    long        clrWindowText;
    long        clrCaptionText;
    long        clrActiveBorder;
    long        clrInactiveBorder;
    long        clrAppWorkspace;
} SYSCOLORS;

#ifndef NOCARET
typedef struct {
    HWND    hwnd;
    BOOL    fVisible;
    BOOL    fOn;
    int     iHideLevel;
    int     x,y,cy,cx;
    HBITMAP hBitmap;
    WORD    cmsBlink;   /* blink time in milliseconds */
    WORD    hTimer;
} CARET;
#endif

typedef struct {
    int     cy;
    int     cx;
    RECT    rc;
    int     cLock;
    int     cclm;       /*Number of organized columns   */
    int     cclmSave;
    int     cwnd;
} SCRN;
typedef SCRN *PSCRN;

#ifndef NOMENUS

/* resource ID of system menus */
#define ID_SYSMENU   MAKEINTRESOURCE(1)
#define ID_CLOSEMENU MAKEINTRESOURCE(2)

/* menu flags and structures */
    /* for pItem's */
#define MRGFDISABLED 0x03
#define MFBITMAP     0x04
#define MFCHECKED    0x08
#define MFHASPOPUP   0x10
#define MRGFBREAK    0x60
#define MFENDMENU    0x80
#define MFRECTHILITE 0x2000
#define MFHELP       0x4000
#define MRGFGRAYED   0x01


    /* for pMenu's */
#define MFISPOPUP    0x01
#define MFLOCKMENU   0x02
#define MFMULTIROW   0x04

/* defines for menu focus */
#define FREEHOLD    0
#define MOUSEHOLD  -1
#define KEYBDHOLD   1


#define TMF(pitem, flag)  (*((char *)pitem) &   flag)
#define SMF(pitem, flag)  (*((char *)pitem) |=  flag)
#define CMF(pitem, flag)  (*((char *)pitem) &= ~flag)

/* SMF can only be used on the single bit flags.  */
/*         (not MRGFDISABLED or MRGFBREAK).       */

/* Menu string structure */
typedef struct {
    int     cch;
    char    name[1];
} STRG;
typedef STRG    *PSTRG;

typedef struct {
    unsigned int    fItem;
    HANDLE          cmdMenu;
    int             xItem;
    int             yItem;
    int             cxItem;
    int             cyItem;
    int             dxTab;
    HANDLE          hItem;
} ITEM;
typedef ITEM        *PITEM;
typedef ITEM FAR    *LPITEM;

typedef struct {
    unsigned int    fMenu;
    int             iItem;
    int             cbMenu;    /* size of the menu in bytes */
    int             cxMenu;
    int             cyMenu;
    int             cItems;
    HWND            hwndMenu;
    ITEM            rgItems[1];
} MENU;
typedef MENU     *PMENU;
#endif

/* Accelerator table structure */
typedef struct {
    BYTE   fVirt;
    WORD   key;
    WORD   cmd;
} ACCEL;
typedef HANDLE HACCEL;
typedef ACCEL FAR *LPACCEL;

typedef struct {
    HBITMAP hBitmap;
    short   cx;
    short   cy;
} OEMBITMAPINFO;

typedef struct oeminfo {
    OEMBITMAPINFO bmFull;
    OEMBITMAPINFO bmAdjust;
    OEMBITMAPINFO bmUpArrow;
    OEMBITMAPINFO bmDnArrow;
    OEMBITMAPINFO bmRgArrow;
    OEMBITMAPINFO bmLfArrow;
    OEMBITMAPINFO bmSize;
    OEMBITMAPINFO bmCheck;
    OEMBITMAPINFO bmbtnbmp;
    short cxbmpChk;
    short cybmpChk;
    short cxbmpHThumb;
    short cybmpVThumb;
    short cxMin;
    short cyMin;
    short cxIconSlot;
    short cyIconSlot;
    short cxIcon;
    short cyIcon;
    short dxIcon;       /* Icon x ratio */
    short dyIcon;       /* Icon y ratio */
    short cxCursor;
    short cyCursor;
    short dxCursor;     /* Cursor x ratio */
    short dyCursor;     /* Cursor y ratio */
    short cSKanji;
    short fMouse;
} OEMINFO;
extern  OEMINFO     oemInfo;


typedef struct resInfo {
    /* the next 9 match resInfo */
    int dxClose;
    int dxSize;
    int dxUpArrow;
    int dxDnArrow;
    int dxRgArrow;
    int dxLfArrow;
    int dxBtSize;
    int dxCheck;
    int dxCheckBoxes;
    /* these aren't in oemInfo */
    int dxBtnCorners;
    int dxReduce;
    int dxZoom;
    int dxRestore;
    HBITMAP hbmBits;
    int cxBits;
    int cyBits;
} RESINFO;

extern RESINFO resInfo;




typedef struct taskEntry {
    char *hq;
    HWND  hwnd;
    short ID;
    unsigned count, freq, ready;
    FARPROC lpfnTask;
} TASK;

/* DC cache entry structure (DCE) */

#define CACHESIZE  5

typedef struct dce {
    struct dce *pdceNext;
    HWND hwnd;
    HDC  hdc;
    BYTE type;
    BYTE cBusy;
    BYTE fVisDirty;
    BYTE unused;
    int xLVBOrg;
    int yLVBOrg;
    HWND hwndOrg;
    HRGN hrgnClip;
} DCE;
typedef DCE *PDCE;

/* CalcVisRgn DC type bits */
#define DCUNUSED        0x00        /* unused cache entry */
#define DCC             0x01        /* client area */
#define DCW             0x02        /* window area */
#define DCSAVEDVISRGN   0x04
#define DCCLIPRGN       0x08
#define DCNOCHILDCLIP   0x10        /* nochildern clip       */
#define DCSAVEVIS       0x20        /* Save vis rgn before calculating */
#define DCCACHE         0x40

HDC GetDC2(HWND, int);

/* Saved Popup Bits structure */

typedef struct spb {
    struct spb *pspbNext;
    HWND hwnd;
    HBITMAP hbm;
    RECT rc;
    HRGN hrgn;
} SPB;

/* ScreenUpdateRgn command bits */

#define SUENUMCHILDREN      0x0001
#define SUSTOPCLIPCHILDREN  0x0004
#define SUVALIDATE          0x8000
#define SUADDTOALL          0x4000
/* Scroll bar info structure */

typedef struct {
    int pos;
    int posMin;
    int posMax;
    int cpxThumb;
    int cpxArrow;
    int cpx;
    int pxMin;
    int cxBorder, cyBorder;
} SBINFO;

/* Application queue structure */

#ifndef NOMSG

#define MSGQSIZE    10

typedef HANDLE HQ;
typedef struct {
    HQ hqNext;
    HANDLE hTask;
    int cbEntry;
    int cMsgs;
    unsigned pmsgRead;
    unsigned pmsgWrite;
    unsigned pmsgMax;
    long timeLast;          /* time, position, and ID of last message */
    POINT ptLast;
    int idLast;
    BYTE cLock;
    BYTE cLockAll;
    LONG lParam;
    WORD wParam;
    int message;
    HWND hwnd;
    DWORD result;
    int cQuit;
    int exitCode;
    unsigned flags;
    FARPROC lpfnAppProc;
    FARPROC lpfnMsgFilter;
    HANDLE hDS;
    int wVersion;
    HQ hqSender;
    HQ hqSendList;
    HQ hqSendNext;
    BYTE cPaintsReady;
    BYTE cTimersReady;
    WORD changebits;
    WORD wakebits;
    WORD wakemask;
    WORD pResult;
    WORD pResultSend;
    WORD pResultReceive;
/*  WORD unused[3];  */

    HWND hwndLast;	    /* most recently active window in task */
    WORD unused[2];

    DWORD semInput;
    HQ hqSemNext;
/*  DWORD sembits; */
/*  BYTE rgcSemBusy[CSEMAPHORES]; */
/*  WORD cdcq; */
    MSG rgmsg[MSGQSIZE];
} Q;
typedef Q far *LPQ;
LPQ LpqFromHq(HQ);

#define WKB_MOUSE         0x01
#define WKB_KEYBOARD      0x02
#define WKB_QUEUEMSG      0x04
#define WKB_TIMER         0x08
#define WKB_PAINT         0x10
#define WKB_NULL          0x20
#define WKB_SENDMSG       0x40
#define WKB_SMRESULT      0x80
#define WKB_SMPARAMSFREE  0x100
#define WKB_RUNTASK       0x200
#define WKB_INPUT         (WKB_MOUSE | WKB_KEYBOARD)
#define WKB_ALLINPUT      (WKB_INPUT | WKB_QUEUEMSG | WKB_TIMER | WKB_PAINT | WKB_NULL)

#define QF_SEMWAIT        0x01
#define QF_INIT           0x02

#endif

/**************************************************/
/* Capture codes */

#define NO_CAP_CLIENT  0        /* no capture; in client area */
#define NO_CAP_SYS     1        /* no capture; in sys area */
#define CLIENT_CAPTURE 2        /* client-relative capture */
#define WINDOW_CAPTURE 3        /* window-relative capture */
#define SCREEN_CAPTURE 4        /* screen-relative capture */

#define CBEDITEXTRA     6
#define CBSTATICEXTRA   6
#define CBBUTTONEXTRA   5

/* RIP error codes (also defined in debug.inc) */

#define RIP_SEMCHECK       -12
#define RIP_SWP            -15  /* SetMultipleWindowPos */
#define RIP_MEMALLOC        1
#define RIP_MEMREALLOC      2
#define RIP_MEMFREE         3
#define RIP_MEMLOCK         4
#define RIP_MEMUNLOCK       5
#define RIP_SENDMESSAGELOCK 6
#define RIP_BADWINDOWHANDLE 7
#define RIP_DCBUSY          8
#define RIP_NODEFWINDOWPROC 9
#define RIP_CLIPBOARDOPEN   10
#define RIP_DCCACHEFULL     11
#define RIP_INVALKEYBOARD   12
#define RIP_INVALMOUSE      13
#define RIP_INVALCURSOR     14
#define RIP_DSUNLOCKED      15
#define RIP_INVALLOCKSYSQ   16
#define RIP_CARETBUSY       17
#define RIP_GETCWRANGE      18
#define RIP_HWNDOWNSDCS     19  /* One hwnd owns all the DCs */
#define RIP_BADHQ           20  /* operation on something of wrong task */
#define RIP_BADDCGRAY       21  /* bad dc gray               */

/* Moveable DS PTR SAFE procs */

BOOL far    PSTextOut(HDC, int, int, LPSTR, int);
long far    PSGetTextExtent(HDC, LPSTR, int);
DWORD far   PSMGetTextExtent(HDC, LPSTR, int);
void far    PSFillRect(HDC, LPRECT, HBRUSH);
void far    PSInvertRect(HDC, LPRECT);
BOOL far    SysHasKanji(void);
void FAR    PSMTextOut(HDC, int, int, LPSTR, int);

#ifdef RESOURCESTRINGS
/* Windows string ids */
#define    STR_UNTITLED                0
#define    STR_ERROR                   1
#define    STR_MBOK                    2
#define    STR_MBCANCEL                3
#define    STR_MBABORT                 4
#define    STR_MBRETRY                 5
#define    STR_MBIGNORE                6
#define    STR_MBYES                   7
#define    STR_MBNO                    8

#define    CNTSTRINGS                  9
#endif

#define BM_CLICK        WM_USER+99
#define CH_PREFIX	'&'
#define CH_HELPPREFIX   0x08

#ifdef KANJI
#define CH_KANJI1	0x1d
#define CH_KANJI2	0x1e
#define CH_KANJI3	0x1f
#endif


/* These are exported for the Window Manager Hook */
HDC  FAR PASCAL GetFrameDC(HWND, HRGN, WORD far *);
BOOL FAR PASCAL ReleaseFrameDC(WORD far *);
void FAR PASCAL CalcClientRect(HWND, LPRECT);
void FAR PASCAL LockPaints(BOOL);
void FAR PASCAL BltColor(HDC, HBRUSH, HBITMAP, int, int, int, int, int, int, BOOL);
int FAR PASCAL MenuCompute(HMENU, HWND, int, int, int);

BOOL FAR PASCAL LockAllWindowVisRgns(BOOL);
BOOL FAR PASCAL LockScreen(BOOL, LPRECT);

void FAR PASCAL DrawFrame(HDC, LPRECT, int, int);

/* clborder factor for dialog boxes */
#define CLDLGFRAME 4

#define WM_DOQUIT           0x0119
#define WM_DOSETWINDOWPOS   0x011a
#define WM_DOSUSPEND        0x011b
#define WM_DOMINIMIZE       0x011c
#define WM_DOMAXIMIZE       0x011d
#define WM_DOACTIVATE       0x011e


/* constans for onboard bitmap save     */

#define     ONBOARD_SAVE    0x0000
#define     ONBOARD_RESTORE 0x0001
#define     ONBOARD_CLEAR   0x0002

/* bitmap resource id's */
#define BMR_ICON    1
#define BMR_BITMAP  2
#define BMR_CURSOR  3

#define BMR_DEVDEP  0
#define BMR_DEVIND  1
#define BMR_DEPIND  2

/* pid definitions  */

#define get_PID               0
#define get_EMSSave_area      1
#define dont_free_banks       2
#define free_PIDs_banks       3
#define free_handle           4
#define memory_sizes          5
#define DDE_shared            6


typedef struct {
    int     pid;
    int     regSet;
    HANDLE  pidHandle;
}EMSCOPYINFO;
typedef EMSCOPYINFO far *LPEMSCOPYINFO;


/* This was added at the last minute to allow user to pre-allocate a
 * set of moveable global handles that have values < 0x2000.  This is
 * needed for edit controls in a dialog box that use their own DS.  The
 * handle for this DS MUST be < 0x2000 to allow stack patching.
 * (Wed 11-Nov-1987 : bobgu)
 */
#define     MAXCACHEHANDLES	10
typedef struct {
    HANDLE  hDS[MAXCACHEHANDLES];
    BOOL    fDSinuse[MAXCACHEHANDLES];
} DSHANDLECACHE;
