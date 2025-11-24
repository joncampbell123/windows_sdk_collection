/*
 * STDEMO.H
 * StaStrip Test Version 1.00
 *
 * Definitions, structures, types, and function prototypes.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include <book1632.h>


/*
 * Resource identifiers
 */

//Resource IDs and definitions
#define IDR_MENU                            10
#define IDR_STATMESSAGEMAP                  11

#define ID_STATSTRIP                        1000

#define IDM_MESSAGESET                      100
#define IDM_MESSAGEGET                      101
#define IDM_MESSAGEGETLENGTH                102
#define IDM_MESSAGEENABLE                   103
#define IDM_MESSAGEDISABLE                  104
#define IDM_MESSAGESETFONT                  105


//ID's we use for popup menus and miscellaneous StatStrip messages
#define ID_MENUSYS                          500
#define ID_MENUMESSAGE                      501

#define ID_MESSAGEEMPTY                     502
#define ID_MESSAGEREADY                     503

#define CPOPUPMENUS                         1


//String IDs for StatStrip
#define IDS_SYSMESSAGESIZE                  16
#define IDS_SYSMESSAGEMOVE                  17
#define IDS_SYSMESSAGEMINIMIZE              18
#define IDS_SYSMESSAGEMAXIMIZE              19
#define IDS_SYSMESSAGENEXTWINDOW            20
#define IDS_SYSMESSAGEPREVWINDOW            21
#define IDS_SYSMESSAGECLOSE                 22
#define IDS_SYSMESSAGERESTORE               23
#define IDS_SYSMESSAGETASKLIST              24

#define IDS_ITEMMESSAGESET                  25
#define IDS_ITEMMESSAGEGET                  26
#define IDS_ITEMMESSAGEGETLENGTH            27
#define IDS_ITEMMESSAGEENABLE               28
#define IDS_ITEMMESSAGEDISABLE              29
#define IDS_ITEMMESSAGESETFONT              30

#define IDS_MENUMESSAGESYSTEM               31
#define IDS_MENUMESSAGEMESSAGE              32

#define IDS_EMPTYMESSAGE                    33
#define IDS_READYMESSAGE                    34


#define IDS_STATMESSAGEMIN                  IDS_SYSMESSAGESIZE
#define IDS_STATMESSAGEMAX                  IDS_READYMESSAGE

#define CSTATMESSAGES     (IDS_STATMESSAGEMAX-IDS_STATMESSAGEMIN+1)
#define CCHMESSAGEMAX     80



/*
 * Application-defined types.
 */

//Application-wide Variables
typedef struct
    {
    HINSTANCE           hInst;      //WinMain parameters
    HINSTANCE           hInstPrev;
    LPSTR               pszCmdLine;
    int                 nCmdShow;

    HWND                hWnd;       //Main window handle
    HWND                hWndST;     //StatStrip window handle

    HFONT               hFont;      //Current font
    } APPVARS, * PAPPVARS;

#define CBAPPVARS sizeof(APPVARS)


/*
 * Function prototypes.
 */

//INIT.C
PAPPVARS     AppPAllocate(PINT, HINSTANCE, HINSTANCE, LPSTR, int);
PAPPVARS     AppPFree(PAPPVARS);

//STDEMO.C
LRESULT APIENTRY StatDemoWndProc(HWND, UINT, WPARAM, LONG);
