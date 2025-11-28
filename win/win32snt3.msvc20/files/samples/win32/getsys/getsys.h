
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

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
    {{ COLOR_SCROLLBAR          , "COLOR_SCROLLBAR       \t%lx"},
     { COLOR_BACKGROUND         , "COLOR_BACKGROUND      \t%lx"},
     { COLOR_ACTIVECAPTION      , "COLOR_ACTIVECAPTION   \t%lx"},
     { COLOR_INACTIVECAPTION    , "COLOR_INACTIVECAPTION \t%lx"},
     { COLOR_MENU               , "COLOR_MENU            \t%lx"},
     { COLOR_WINDOW             , "COLOR_WINDOW          \t%lx"},
     { COLOR_WINDOWFRAME        , "COLOR_WINDOWFRAME     \t%lx"},
     { COLOR_MENUTEXT           , "COLOR_MENUTEXT        \t%lx"},
     { COLOR_WINDOWTEXT         , "COLOR_WINDOWTEXT      \t%lx"},
     { COLOR_CAPTIONTEXT        , "COLOR_CAPTIONTEXT     \t%lx"},
     { COLOR_ACTIVEBORDER       , "COLOR_ACTIVEBORDER    \t%lx"},
     { COLOR_INACTIVEBORDER     , "COLOR_INACTIVEBORDER  \t%lx"},
     { COLOR_APPWORKSPACE       , "COLOR_APPWORKSPACE    \t%lx"},
     { COLOR_HIGHLIGHT          , "COLOR_HIGHLIGHT       \t%lx"},
     { COLOR_HIGHLIGHTTEXT      , "COLOR_HIGHLIGHTTEXT   \t%lx"},
     { COLOR_BTNFACE            , "COLOR_BTNFACE         \t%lx"},
     { COLOR_BTNSHADOW          , "COLOR_BTNSHADOW       \t%lx"},
     { COLOR_GRAYTEXT           , "COLOR_GRAYTEXT        \t%lx"},
     { COLOR_BTNTEXT            , "COLOR_BTNTEXT         \t%lx"},
     { COLOR_INACTIVECAPTIONTEXT, "COLOR_INACTIVECAPTIONTEXT \t%lx"},
     { COLOR_BTNHIGHLIGHT       , "COLOR_BTNHIGHLIGHT    \t%lx"}};



/* GetSystemMetrics() codes from WINUSER.H */
#define NSYSMETRICS SM_CMETRICS
LookupEntry  SystemMetrics[NSYSMETRICS] =
    {{ SM_CXSCREEN         , "SM_CXSCREEN      \t%d"},
     { SM_CYSCREEN         , "SM_CYSCREEN      \t%d"},
     { SM_CXVSCROLL        , "SM_CXVSCROLL     \t%d"},
     { SM_CYHSCROLL        , "SM_CYHSCROLL     \t%d"},
     { SM_CYCAPTION        , "SM_CYCAPTION     \t%d"},
     { SM_CXBORDER         , "SM_CXBORDER      \t%d"},
     { SM_CYBORDER         , "SM_CYBORDER      \t%d"},
     { SM_CXDLGFRAME       , "SM_CXDLGFRAME    \t%d"},
     { SM_CYDLGFRAME       , "SM_CYDLGFRAME    \t%d"},
     { SM_CYVTHUMB         , "SM_CYVTHUMB      \t%d"},
     { SM_CXHTHUMB         , "SM_CXHTHUMB      \t%d"},
     { SM_CXICON           , "SM_CXICON        \t%d"},
     { SM_CYICON           , "SM_CYICON        \t%d"},
     { SM_CXCURSOR         , "SM_CXCURSOR      \t%d"},
     { SM_CYCURSOR         , "SM_CYCURSOR      \t%d"},
     { SM_CYMENU           , "SM_CYMENU        \t%d"},
     { SM_CXFULLSCREEN     , "SM_CXFULLSCREEN  \t%d"},
     { SM_CYFULLSCREEN     , "SM_CYFULLSCREEN  \t%d"},
     { SM_CYKANJIWINDOW    , "SM_CYKANJIWINDOW \t%d"},
     { SM_MOUSEPRESENT     , "SM_MOUSEPRESENT  \t%d"},
     { SM_CYVSCROLL        , "SM_CYVSCROLL     \t%d"},
     { SM_CXHSCROLL        , "SM_CXHSCROLL     \t%d"},
     { SM_DEBUG            , "SM_DEBUG         \t%d"},
     { SM_SWAPBUTTON       , "SM_SWAPBUTTON    \t%d"},
     { SM_RESERVED1        , "SM_RESERVED1     \t%d"},
     { SM_RESERVED2        , "SM_RESERVED2     \t%d"},
     { SM_RESERVED3        , "SM_RESERVED3     \t%d"},
     { SM_RESERVED4        , "SM_RESERVED4     \t%d"},
     { SM_CXMIN            , "SM_CXMIN         \t%d"},
     { SM_CYMIN            , "SM_CYMIN         \t%d"},
     { SM_CXSIZE           , "SM_CXSIZE        \t%d"},
     { SM_CYSIZE           , "SM_CYSIZE        \t%d"},
     { SM_CXFRAME          , "SM_CXFRAME       \t%d"},
     { SM_CYFRAME          , "SM_CYFRAME       \t%d"},
     { SM_CXMINTRACK       , "SM_CXMINTRACK    \t%d"},
     { SM_CYMINTRACK       , "SM_CYMINTRACK    \t%d"},
     { SM_CXDOUBLECLK      , "SM_CXDOUBLECLK   \t%d"},
     { SM_CYDOUBLECLK      , "SM_CYDOUBLECLK   \t%d"},
     { SM_CXICONSPACING    , "SM_CXICONSPACING \t%d"},
     { SM_CYICONSPACING    , "SM_CYICONSPACING \t%d"},
     { SM_MENUDROPALIGNMENT, "SM_MENUDROPALIGNMENT\t%d"},
     { SM_PENWINDOWS       , "SM_PENWINDOWS    \t%d"},
     { SM_DBCSENABLED      , "SM_DBCSENABLED   \t%d"},
     { SM_CMOUSEBUTTONS    , "SM_CMOUSEBUTTONS \t%d"}};
