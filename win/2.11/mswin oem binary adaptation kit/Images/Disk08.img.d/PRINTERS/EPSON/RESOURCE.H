/* stuff from windows.h */

/* Conventional dialog box and message box command IDs */
#define IDOK         1
#define IDCANCEL     2
#define IDABORT      3
#define IDRETRY      4
#define IDIGNORE     5
#define IDYES        6
#define IDNO         7

/* Window styles */
#define WS_TILED        0x00000000L
#define WS_POPUP        0x80000000L
#define WS_CHILD        0x40000000L
#define WS_ICONIC       0x20000000L
#define WS_VISIBLE      0x10000000L
#define WS_DISABLED     0x08000000L
#define WS_CLIPSIBLINGS 0x04000000L
#define WS_CLIPCHILDREN 0x02000000L

#define WS_BORDER       0x00800000L
#define WS_CAPTION      0x00c00000L
#define WS_DLGFRAME     0x00400000L
#define WS_VSCROLL      0x00200000L
#define WS_HSCROLL      0x00100000L
#define WS_SYSMENU      0x00080000L
#define WS_SIZEBOX      0x00040000L
#define WS_GROUP        0x00020000L
#define WS_TABSTOP      0x00010000L

/* ** Window Procedure Messages */

#define WM_NULL             0x0000
#define WM_CREATE           0x0001
#define WM_DESTROY          0x0002
#define WM_MOVE             0x0003
#define WM_SIZEWAIT         0x0004
#define WM_SIZE             0x0005
#define WM_ACTIVATE         0x0006
#define WM_SETFOCUS         0x0007
#define WM_KILLFOCUS        0x0008
#define WM_SETVISIBLE       0x0009
#define WM_ENABLE           0x000a
#define WM_SETREDRAW        0x000b
#define WM_SETTEXT          0x000c
#define WM_GETTEXT          0x000d
#define WM_GETTEXTLENGTH    0x000e
#define WM_PAINT            0x000f
#define WM_CLOSE            0x0010
#define WM_QUERYENDSESSION  0x0011
#define WM_QUIT             0x0012
#define WM_QUERYOPEN        0x0013
#define WM_ERASEBKGND       0x0014
#define WM_SYSCOLORCHANGE   0x0015
#define WM_ENDSESSION       0x0016
#define WM_SYSTEMERROR      0x0017
#define WM_SHOWWINDOW       0x0018
#define WM_CTLCOLOR         0x0019
#define WM_WININICHANGE     0x001a
#define WM_DEVMODECHANGE    0x001b
#define WM_ACTIVATEAPP      0x001c
#define WM_FONTCHANGE       0x001d
#define WM_TIMECHANGE       0x001e
#define WM_CANCELMODE       0x001f
#define WM_SETCURSOR        0x0020
#define WM_MOUSEACTIVATE    0x0021
#define WM_CHILDACTIVATE    0x0022
#define WM_QUEUESYNC        0x0023
#define WM_GETMINMAXINFO    0x0024
#define WM_OTHERWINDOWDESTROY 0x0025

#define WINDOW_TEXT_LENGTH 50

/* Static control constants */

#define SS_LEFT       0L
#define SS_CENTER     1L
#define SS_RIGHT      2L
#define SS_ICON       3L
#define SS_BLACKRECT  4L
#define SS_GRAYRECT   5L
#define SS_WHITERECT  6L
#define SS_BLACKFRAME 7L
#define SS_GRAYFRAME  8L
#define SS_WHITEFRAME 9L
#define SS_USERITEM   10L
#define SS_SIMPLE     11L

/* device mode constants */

#define PORTRAIT    10
#define LANDSCAPE   11

#define LETTER      20
#define DINA4       21
#define FANFOLD     22

#define HIGH        16
#define LOW         17

#define PAPERWIDTH  23

#define ALL_COLOR   12
#define BLACK_COLOR 13

#define PRINTER     0x01
#define ORIENT      0x02
#define FEED        0x04
#define SIZE        0x08
#define MYFONT      257
