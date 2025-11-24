/* getsys.h - header file for the GETSYS sample. */


/* function prototypes.  Window procedures first. */
LRESULT CALLBACK MainDlgProc(HWND, UINT, WPARAM, LPARAM);

VOID doSysColors (HWND);
VOID doInfo      (HWND);
VOID doDirectory (HWND);
VOID doMetrics   (HWND);
VOID doPalette   (HWND);
VOID doLocalTime (HWND);
VOID doTime      (HWND);


/* declare global variable to be used by all of the do... functions */
char buffer[100];



/* the control ID's from the dialog box. */
#define DID_SYSCOLORS   0x0065
#define DID_DIRECTORY   0x0066
#define DID_INFO        0x0067
#define DID_METRICS     0x0068
#define DID_PALETTE     0x0069
#define DID_LOCALTIME   0x006A
#define DID_TIME        0x006B
#define DID_LISTBOX     0x006C
#define DID_TEXT        0x006D


/* structure for the lookup tables.  */
typedef struct tagLookupEntry{
    int     Value;
    char    String[100];
} LookupEntry;


/* GetSysColor() codes from WINUSER.H */
#define NSYSCOLORS COLOR_BTNHIGHLIGHT+1 // assumes order puts BTNHIGHLIGHT last
LookupEntry  SysColors[NSYSCOLORS] =
    {{ COLOR_SCROLLBAR          , "COLOR_SCROLLBAR       \t%08lx"},
     { COLOR_BACKGROUND         , "COLOR_BACKGROUND      \t%08lx"},
     { COLOR_ACTIVECAPTION      , "COLOR_ACTIVECAPTION   \t%08lx"},
     { COLOR_INACTIVECAPTION    , "COLOR_INACTIVECAPTION \t%08lx"},
     { COLOR_MENU               , "COLOR_MENU            \t%08lx"},
     { COLOR_WINDOW             , "COLOR_WINDOW          \t%08lx"},
     { COLOR_WINDOWFRAME        , "COLOR_WINDOWFRAME     \t%08lx"},
     { COLOR_MENUTEXT           , "COLOR_MENUTEXT        \t%08lx"},
     { COLOR_WINDOWTEXT         , "COLOR_WINDOWTEXT      \t%08lx"},
     { COLOR_CAPTIONTEXT        , "COLOR_CAPTIONTEXT     \t%08lx"},
     { COLOR_ACTIVEBORDER       , "COLOR_ACTIVEBORDER    \t%08lx"},
     { COLOR_INACTIVEBORDER     , "COLOR_INACTIVEBORDER  \t%08lx"},
     { COLOR_APPWORKSPACE       , "COLOR_APPWORKSPACE    \t%08lx"},
     { COLOR_HIGHLIGHT          , "COLOR_HIGHLIGHT       \t%08lx"},
     { COLOR_HIGHLIGHTTEXT      , "COLOR_HIGHLIGHTTEXT   \t%08lx"},
     { COLOR_BTNFACE            , "COLOR_BTNFACE         \t%08lx"},
     { COLOR_BTNSHADOW          , "COLOR_BTNSHADOW       \t%08lx"},
     { COLOR_GRAYTEXT           , "COLOR_GRAYTEXT        \t%08lx"},
     { COLOR_BTNTEXT            , "COLOR_BTNTEXT         \t%08lx"},
     { COLOR_INACTIVECAPTIONTEXT, "COLOR_INACTIVECAPTIONTEXT \t%08lx"},
     { COLOR_BTNHIGHLIGHT       , "COLOR_BTNHIGHLIGHT    \t%08lx"}};



/* GetSystemMetrics() codes from WINUSER.H */
//#define NSYSMETRICS SM_CMETRICS
//LookupEntry  SystemMetrics[NSYSMETRICS] =
LookupEntry  SystemMetrics[] =
    {{ SM_CXSCREEN         , "SM_CXSCREEN         \t%d"},
     { SM_CYSCREEN         , "SM_CYSCREEN         \t%d"},
     { SM_CXVSCROLL        , "SM_CXVSCROLL        \t%d"},
     { SM_CYHSCROLL        , "SM_CYHSCROLL        \t%d"},
     { SM_CYCAPTION        , "SM_CYCAPTION        \t%d"},
     { SM_CXBORDER         , "SM_CXBORDER         \t%d"},
     { SM_CYBORDER         , "SM_CYBORDER         \t%d"},
     { SM_CXDLGFRAME       , "SM_CXDLGFRAME       \t%d"},
     { SM_CYDLGFRAME       , "SM_CYDLGFRAME       \t%d"},
     { SM_CYVTHUMB         , "SM_CYVTHUMB         \t%d"},
     { SM_CXHTHUMB         , "SM_CXHTHUMB         \t%d"},
     { SM_CXICON           , "SM_CXICON           \t%d"},
     { SM_CYICON           , "SM_CYICON           \t%d"},
     { SM_CXCURSOR         , "SM_CXCURSOR         \t%d"},
     { SM_CYCURSOR         , "SM_CYCURSOR         \t%d"},
     { SM_CYMENU           , "SM_CYMENU           \t%d"},
     { SM_CXFULLSCREEN     , "SM_CXFULLSCREEN     \t%d"},
     { SM_CYFULLSCREEN     , "SM_CYFULLSCREEN     \t%d"},
     { SM_CYKANJIWINDOW    , "SM_CYKANJIWINDOW    \t%d"},
     { SM_MOUSEPRESENT     , "SM_MOUSEPRESENT     \t%d"},
     { SM_CYVSCROLL        , "SM_CYVSCROLL        \t%d"},
     { SM_CXHSCROLL        , "SM_CXHSCROLL        \t%d"},
     { SM_DEBUG            , "SM_DEBUG            \t%d"},
     { SM_SWAPBUTTON       , "SM_SWAPBUTTON       \t%d"},
     { SM_RESERVED1        , "SM_RESERVED1        \t%d"},
     { SM_RESERVED2        , "SM_RESERVED2        \t%d"},
     { SM_RESERVED3        , "SM_RESERVED3        \t%d"},
     { SM_RESERVED4        , "SM_RESERVED4        \t%d"},
     { SM_CXMIN            , "SM_CXMIN            \t%d"},
     { SM_CYMIN            , "SM_CYMIN            \t%d"},
     { SM_CXSIZE           , "SM_CXSIZE           \t%d"},
     { SM_CYSIZE           , "SM_CYSIZE           \t%d"},
     { SM_CXFRAME          , "SM_CXFRAME          \t%d"},
     { SM_CYFRAME          , "SM_CYFRAME          \t%d"},
     { SM_CXMINTRACK       , "SM_CXMINTRACK       \t%d"},
     { SM_CYMINTRACK       , "SM_CYMINTRACK       \t%d"},
     { SM_CXDOUBLECLK      , "SM_CXDOUBLECLK      \t%d"},
     { SM_CYDOUBLECLK      , "SM_CYDOUBLECLK      \t%d"},
     { SM_CXICONSPACING    , "SM_CXICONSPACING    \t%d"},
     { SM_CYICONSPACING    , "SM_CYICONSPACING    \t%d"},
     { SM_MENUDROPALIGNMENT, "SM_MENUDROPALIGNMENT\t%d"},
     { SM_PENWINDOWS       , "SM_PENWINDOWS       \t%d"},
     { SM_DBCSENABLED      , "SM_DBCSENABLED      \t%d"},
     { SM_CMOUSEBUTTONS    , "SM_CMOUSEBUTTONS    \t%d"},
     { SM_CXEDGE           , "SM_CXEDGE           \t%d"},
     { SM_CYEDGE           , "SM_CYEDGE           \t%d"},
     { SM_CXMINSPACING     , "SM_CXMINSPACING     \t%d"},
     { SM_CYMINSPACING     , "SM_CYMINSPACING     \t%d"},
     { SM_CXSMICON         , "SM_CXSMICON         \t%d"},
     { SM_CYSMICON         , "SM_CYSMICON         \t%d"},
     { SM_CYSMCAPTION      , "SM_CYSMCAPTION      \t%d"},
     { SM_CXSMSIZE         , "SM_CXSMSIZE         \t%d"},
     { SM_CYSMSIZE         , "SM_CYSMSIZE         \t%d"},
     { SM_CXMENUSIZE       , "SM_CXMENUSIZE       \t%d"},
     { SM_CYMENUSIZE       , "SM_CYMENUSIZE       \t%d"},
     { SM_ARRANGE          , "SM_ARRANGE          \t%d"},
     { SM_CXMINIMIZED      , "SM_CXMINIMIZED      \t%d"},
     { SM_CYMINIMIZED      , "SM_CYMINIMIZED      \t%d"},
     { SM_CXMAXTRACK       , "SM_CXMAXTRACK       \t%d"},
     { SM_CYMAXTRACK       , "SM_CYMAXTRACK       \t%d"},
     { SM_CXMAXIMIZED      , "SM_CXMAXIMIZED      \t%d"},
     { SM_CYMAXIMIZED      , "SM_CYMAXIMIZED      \t%d"},
//     { SM_SHOWSOUNDS       , "SM_SHOWSOUNDS       \t%d"},
//     { SM_KEYBOARDPREF     , "SM_KEYBOARDPREF     \t%d"},
//     { SM_HIGHCONTRAST     , "SM_HIGHCONTRAST     \t%d"},
//     { SM_SCREENREADER     , "SM_SCREENREADER     \t%d"},
     { SM_CLEANBOOT        , "SM_CLEANBOOT        \t%d"},
     { SM_CXDRAG           , "SM_CXDRAG           \t%d"},
     { SM_CYDRAG           , "SM_CYDRAG           \t%d"},
     { SM_NETWORK          , "SM_NETWORK          \t%d"}
    };
