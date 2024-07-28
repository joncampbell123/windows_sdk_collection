/*****************************************************************************\
*                                                                             *
* commctrl.h -   Common control functions, types, and definitions             *
*                                                                             *
*               Version 1.0                                                   *
*                                                                             *
*               NOTE: windows.h must be #included first                       *
*                                                                             *
*               Copyright (c) 1992, Microsoft Corp.  All rights reserved.     *
*                                                                             *
\*****************************************************************************/

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif /* !RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#ifndef NOTOOLBAR

#ifndef _INC_TOOLBAR
#define _INC_TOOLBAR

typedef struct {
    int iBitmap;
    int idCommand;
    BYTE fsState;
    BYTE fsStyle;
    int idsHelp;
} TBBUTTON, NEAR *PTBBUTTON, FAR *LPTBBUTTON;

#define TBSTATE_CHECKED        0x01
#define TBSTATE_PRESSED        0x02
#define TBSTATE_ENABLED        0x04
#define TBSTATE_HIDDEN         0x08

#define TBSTYLE_BUTTON         0x00
#define TBSTYLE_SEP            0x01
#define TBSTYLE_CHECK          0x02
#define TBSTYLE_GROUP          0x04
#define TBSTYLE_CHECKGROUP    (TBSTYLE_GROUP | TBSTYLE_CHECK)

typedef struct {
    TBBUTTON tbButton;
    char szDescription[1];
} ADJUSTINFO, FAR *LPADJUSTINFO;

HWND    WINAPI CreateToolbar(HWND hwnd, DWORD ws, WORD wID, int nBitmaps, HINSTANCE hBMInst, WORD wBMID, LPTBBUTTON lpButtons, int iNumButtons);

typedef struct {
    COLORREF from;
    COLORREF to;
} COLORMAP, FAR *LPCOLORMAP;

HBITMAP WINAPI CreateMappedBitmap(HINSTANCE hInstance, int idBitmap, BOOL bDiscardable, LPCOLORMAP lpColorMap, int iNumMaps);

#define TB_ENABLEBUTTON       (WM_USER+1)
#define TB_CHECKBUTTON        (WM_USER+2)
#define TB_PRESSBUTTON        (WM_USER+3)
#define TB_HIDEBUTTON         (WM_USER+4)

/* Messages up to WM_USER+8 are reserved  */

#define TB_ISBUTTONENABLED    (WM_USER+9)    
#define TB_ISBUTTONCHECKED    (WM_USER+10)    
#define TB_ISBUTTONPRESSED    (WM_USER+11)    
#define TB_ISBUTTONHIDDEN     (WM_USER+12)    

/* Messages up to WM_USER+16 are reserved */

#define TB_SETSTATE           (WM_USER+17)
#define TB_GETSTATE           (WM_USER+18)
#define TB_ADDBITMAP          (WM_USER+19)
#define TB_ADDBUTTONS         (WM_USER+20)
#define TB_INSERTBUTTON       (WM_USER+21)
#define TB_DELETEBUTTON       (WM_USER+22)
#define TB_GETBUTTON          (WM_USER+23)
#define TB_BUTTONCOUNT        (WM_USER+24)
#define TB_COMMANDTOINDEX     (WM_USER+25)
#define TB_SAVERESTORE        (WM_USER+26)
#define TB_CUSTOMIZE          (WM_USER+27)

#endif    /* _INC_TOOLBAR */
#endif

#ifndef NOSTATUSBAR

#ifndef _INC_STATUSBAR
#define _INC_STATUSBAR

void WINAPI DrawStatusText(HDC hDC, LPRECT lprc, LPSTR szText, UINT uFlags);
HWND WINAPI CreateStatusWindow(LONG style, LPSTR lpszText,
      HWND hwndParent, WORD wID);
HWND WINAPI CreateHeaderWindow(LONG style, LPSTR lpszText,
      HWND hwndParent, WORD wID);

#define STATUSCLASSNAME "msctls_statusbar"
#define HEADERCLASSNAME "msctls_headerbar"

#define SB_SETTEXT            (WM_USER+1)
#define SB_GETTEXT            (WM_USER+2)
#define SB_GETTEXTLENGTH      (WM_USER+3)
#define SB_SETPARTS           (WM_USER+4)
#define SB_SETBORDERS         (WM_USER+5)
#define SB_GETPARTS           (WM_USER+6)
#define SB_GETBORDERS         (WM_USER+7)
#define SB_SETMINHEIGHT       (WM_USER+8)
#define SB_SIMPLE             (WM_USER+9)

#define HB_SAVERESTORE        (WM_USER+0x100)
#define HB_ADJUST             (WM_USER+0x101)
#define HB_SETWIDTHS          SB_SETPARTS
#define HB_GETWIDTHS          SB_GETPARTS
#define HB_GETPARTS           (WM_USER+0x102)
#define HB_SHOWTOGGLE         (WM_USER+0x103)

#define SBT_OWNERDRAW         0x1000
#define SBT_NOBORDERS         0x0100
#define SBT_POPOUT            0x0200
#define HBT_SPRING            0x0400

#endif /* _INC_STATUSBAR */

#endif

#ifndef NOMENUHELP

#ifndef _INC_MENUHELP
#define _INC_MENUHELP

BOOL WINAPI WritePrivateProfileStruct(LPSTR szSection, LPSTR szKey,
      LPBYTE lpStruct, UINT uSizeStruct, LPSTR szFile);
BOOL WINAPI GetPrivateProfileStruct(LPSTR szSection, LPSTR szKey,
      LPBYTE lpStruct, UINT uSizeStruct, LPSTR szFile);

void WINAPI MenuHelp(WORD iMessage, WPARAM wParam, LPARAM lParam,
      HMENU hMainMenu, HINSTANCE hInst, HWND hwndStatus, LPWORD lpwIDs);

BOOL WINAPI ShowHideMenuCtl(HWND hWnd, UINT uFlags, LPINT lpInfo);

void WINAPI GetEffectiveClientRect(HWND hWnd, LPRECT lprc, LPINT lpInfo);

#define MINSYSCOMMAND    SC_SIZE

#endif /* _INC_MENUHELP */

#endif

#ifndef NOBTNLIST

#ifndef _INC_BTNLIST
#define _INC_BTNLIST

/* Class name */
#define BUTTONLISTBOX           "ButtonListBox"

/* Button List Box Styles */
#define BLS_NUMBUTTONS      0x00FFL
#define BLS_VERTICAL        0x0100L
#define BLS_NOSCROLL        0x0200L

/* Button List Box Messages */
#define BL_ADDBUTTON        (WM_USER+1)
#define BL_DELETEBUTTON     (WM_USER+2)
#define BL_GETCARETINDEX    (WM_USER+3)
#define BL_GETCOUNT         (WM_USER+4)
#define BL_GETCURSEL        (WM_USER+5)
#define BL_GETITEMDATA      (WM_USER+6)
#define BL_GETITEMRECT      (WM_USER+7)
#define BL_GETTEXT          (WM_USER+8)
#define BL_GETTEXTLEN       (WM_USER+9)
#define BL_GETTOPINDEX      (WM_USER+10)
#define BL_INSERTBUTTON     (WM_USER+11)
#define BL_RESETCONTENT     (WM_USER+12)
#define BL_SETCARETINDEX    (WM_USER+13)
#define BL_SETCURSEL        (WM_USER+14)
#define BL_SETITEMDATA      (WM_USER+15)
#define BL_SETTOPINDEX      (WM_USER+16)

/* Button listbox notification codes send in WM_COMMAND */
#define BLN_ERRSPACE        (-2)
#define BLN_SELCHANGE       1
#define BLN_CLICKED         2
#define BLN_SELCANCEL       3
#define BLN_SETFOCUS        4
#define BLN_KILLFOCUS       5

/* Message return values */
#define BL_OKAY             0
#define BL_ERR              (-1)
#define BL_ERRSPACE         (-2)

typedef struct tagCLB {
    UINT        cbSize;     /* size of structure */
    DWORD       dwItemData; /* user defined item data */
                            /* for LB_GETITEMDATA and LB_SETITEMDATA */
    HBITMAP     hBitmap;    /* button bitmap */
    LPCSTR      lpszText;   /* button text */

} CREATELISTBUTTON;
typedef CREATELISTBUTTON FAR* LPCREATELISTBUTTON;


#endif /* _INC_BTNLIST */
#endif

#ifndef NOTRACKBAR

#ifndef _INC_TRACKBAR
#define _INC_TRACKBAR

#define TRACKBAR_CLASS          "msctls_trackbar"

/* Trackbar styles */
#define TBS_AUTOTICKS           0x0001L

#define TBM_GETPOS              (WM_USER)
#define TBM_GETRANGEMIN         (WM_USER+1)
#define TBM_GETRANGEMAX         (WM_USER+2)
#define TBM_GETTIC              (WM_USER+3)
#define TBM_SETTIC              (WM_USER+4)
#define TBM_SETPOS              (WM_USER+5)
#define TBM_SETRANGE            (WM_USER+6)
#define TBM_SETRANGEMIN         (WM_USER+7)
#define TBM_SETRANGEMAX         (WM_USER+8)
#define TBM_CLEARTICS           (WM_USER+9)
#define TBM_SETSEL              (WM_USER+10)
#define TBM_SETSELSTART         (WM_USER+11)
#define TBM_SETSELEND           (WM_USER+12)
#define TBM_GETPTICS            (WM_USER+14)
#define TBM_GETTICPOS           (WM_USER+15)
#define TBM_GETNUMTICS          (WM_USER+16)
#define TBM_GETSELSTART         (WM_USER+17)
#define TBM_GETSELEND           (WM_USER+18)
#define TBM_CLEARSEL            (WM_USER+19)

/* these match the SB_ (scroll bar messages) */

#define TB_LINEUP               0
#define TB_LINEDOWN             1
#define TB_PAGEUP               2
#define TB_PAGEDOWN             3
#define TB_THUMBPOSITION        4
#define TB_THUMBTRACK           5
#define TB_TOP                  6
#define TB_BOTTOM               7
#define TB_ENDTRACK             8

#endif /* _INC_TRACKBAR */
#endif /* NOTRACKBAR */

#ifndef NODRAGLIST
#ifndef _INC_DRAGLIST
#define _INC_DRAGLIST

typedef struct {
    UINT uNotification;
    HWND hWnd;
    POINT ptCursor;
} DRAGLISTINFO, FAR *LPDRAGLISTINFO;

#define DL_BEGINDRAG            (LB_MSGMAX+100)
#define DL_DRAGGING             (LB_MSGMAX+101)
#define DL_DROPPED              (LB_MSGMAX+102)
#define DL_CANCELDRAG           (LB_MSGMAX+103)

#define DL_CURSORSET            0
#define DL_STOPCURSOR           1
#define DL_COPYCURSOR           2
#define DL_MOVECURSOR           3

#define DRAGLISTMSGSTRING "commctrl_DragListMsg"

/* Exported functions and variables
 */
extern BOOL WINAPI MakeDragList(HWND hLB);
extern int WINAPI LBItemFromPt(HWND hLB, POINT pt, BOOL bAutoScroll);
extern void WINAPI DrawInsert(HWND handParent, HWND hLB, int nItem);

#endif    /* _INC_DRAGLIST */
#endif    /* NODRAGLIST */

#ifndef NOUPDOWN

#ifndef __INC_UPDOWN__
#define __INC_UPDOWN__

typedef struct tagUDACCEL {
    UINT nSec;
    UINT nInc;
} UDACCEL, FAR *LPUDACCEL;

#define UD_MAXVAL           0x7fff
#define UD_MINVAL           (-UD_MAXVAL)
#define UD_UNSIGNED_MAXVAL  0xFFFF
#define UD_UNSIGNED_MINVAL  0

#define UDS_WRAP            0x0001
#define UDS_SETBUDDYINT     0x0002
#define UDS_ALIGNRIGHT      0x0004
#define UDS_ALIGNLEFT       0x0008
#define UDS_AUTOBUDDY       0x0010
#define UDS_ARROWKEYS       0x0020


#define UDM_SETRANGE        (WM_USER+101)
#define UDM_GETRANGE        (WM_USER+102)
#define UDM_SETPOS          (WM_USER+103)
#define UDM_GETPOS          (WM_USER+104)
#define UDM_SETBUDDY        (WM_USER+105)
#define UDM_GETBUDDY        (WM_USER+106)
#define UDM_SETACCEL        (WM_USER+107)
#define UDM_GETACCEL        (WM_USER+108)
#define UDM_SETBASE         (WM_USER+109)
#define UDM_GETBASE         (WM_USER+110)

#define UPDOWN_CLASS "msctls_updown"

HWND WINAPI CreateUpDownControl(DWORD dwStyle, int x, int y, int cx, int cy,
                                HWND hParent, int nID, HINSTANCE hInst,
                                HWND hBuddy,
                int nUpper, int nLower, int nPos);

#endif /* __INC_UPDOWN__ */
#endif /* NOUPDOWN */

#define HBN_BEGINDRAG       0x0101
#define HBN_DRAGGING        0x0102
#define HBN_ENDDRAG         0x0103
#define HBN_BEGINADJUST     0x0111
#define HBN_ENDADJUST       0x0112
#define TBN_BEGINDRAG       0x0201
#define TBN_ENDDRAG         0x0203
#define TBN_BEGINADJUST     0x0204
#define TBN_ADJUSTINFO      0x0205
#define TBN_ENDADJUST       0x0206
#define TBN_RESET           0x0207
#define TBN_QUERYINSERT     0x0208
#define TBN_QUERYDELETE     0x0209
#define TBN_TOOLBARCHANGE   0x020a
#define TBN_CUSTHELP        0x020b

#define CCS_TOP             0x00000001L
#define CCS_NOMOVEY         0x00000002L
#define CCS_BOTTOM          0x00000003L
#define CCS_NORESIZE        0x00000004L
#define CCS_NOPARENTALIGN   0x00000008L
#define CCS_NOHILITE        0x00000010L
#define CCS_ADJUSTABLE      0x00000020L

void WINAPI InitCommonControls(void);


#ifdef __cplusplus
}
#endif    /* __cplusplus */

#ifndef RC_INVOKED
#pragma pack()
#endif  /* !RC_INVOKED */
