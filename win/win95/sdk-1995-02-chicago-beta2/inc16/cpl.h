/*****************************************************************************\
*                                                                             *
* cpl.h -       Control panel extension DLL definitions                       *
*                                                                             *
* Version 3.10								      *
*                                                                             *
* Copyright (c) 1992-1994, Microsoft Corp.	All rights reserved	      *
*                                                                             *
******************************************************************************
*  General rules for being installed in the Control Panel:
*
*      1) The DLL must export a function named CPlApplet which will handle
*         the messages discussed below.
*      2) If the applet needs to save information in CONTROL.INI minimize
*         clutter by using the application name [MMCPL.appletname].
*      2) If the applet is refrenced in CONTROL.INI under [MMCPL] use
*         the following form:
*              ...
*              [MMCPL]
*              uniqueName=c:\mydir\myapplet.dll
*              ...
*
*
*  The order applet DLL's are loaded by CONTROL.EXE is not guaranteed.
*  Control panels may be sorted for display, etc.
*
*/
#ifndef _INC_CPL
#define _INC_CPL

#ifndef RC_INVOKED
#pragma pack(1)         /* Assume byte packing throughout */
#endif /* RC_INVOKED */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

/*
 * CONTROL.EXE will answer this message and launch an applet
 *
 * WM_CPL_LAUNCH
 *
 *      wParam      - window handle of calling app
 *      lParam      - LPSTR of name of applet to launch
 *
 * WM_CPL_LAUNCHED
 *
 *      wParam      - TRUE/FALSE if applet was launched
 *      lParam      - NULL
 *
 * CONTROL.EXE will post this message to the caller when the applet returns
 * (ie., when wParam is a valid window handle)
 *
 */
#define WM_CPL_LAUNCH   (WM_USER+1000)
#define WM_CPL_LAUNCHED (WM_USER+1001)

/* A function prototype for CPlApplet() */

typedef LRESULT (CALLBACK *APPLET_PROC)(HWND hwndCpl, UINT msg, LPARAM lParam1, LPARAM lParam2);

/* The data structures CPlApplet() must fill in. */

typedef struct tagCPLINFO
{
    int     idIcon;     /* icon resource id, provided by CPlApplet() */
    int     idName;     /* name string res. id, provided by CPlApplet() */
    int     idInfo;     /* info string res. id, provided by CPlApplet() */
    LONG    lData;      /* user defined data */
} CPLINFO, *PCPLINFO, FAR *LPCPLINFO;

typedef struct tagNEWCPLINFO
{
    DWORD       dwSize;         /* similar to the commdlg */
    DWORD	dwFlags;
    DWORD       dwHelpContext;  /* help context to use */
    LONG        lData;          /* user defined data */
    HICON       hIcon;          /* icon to use, this is owned by CONTROL.EXE (may be deleted) */
    char        szName[32];     /* short name */
    char        szInfo[64];     /* long name (status line) */
    char        szHelpFile[128];/* path to help file to use */
} NEWCPLINFO, *PNEWCPLINFO, FAR *LPNEWCPLINFO;

#define CPL_DYNAMIC_RES 0
// This constant may be used in place of real resource IDs for the idIcon,
// idName or idInfo members of the CPLINFO structure.  Normally, the system
// uses these values to extract copies of the resources and store them in a
// cache.  Once the resource information is in the cache, the system does not
// need to load a CPL unless the user actually tries to use it.
// CPL_DYNAMIC_RES tells the system not to cache the resource, but instead to
// load the CPL every time it needs to display information about an item.  This
// allows a CPL to dynamically decide what information will be displayed, but
// is SIGNIFICANTLY SLOWER than displaying information from a cache.
// Typically, CPL_DYNAMIC_RES is used when a control panel must inspect the
// runtime status of some device in order to provide text or icons to display.

/* The messages CPlApplet() must handle: */

#define CPL_INIT        1
/*  This message is sent to indicate CPlApplet() was found. */
/*  lParam1 and lParam2 are not defined. */
/*  Return TRUE or FALSE indicating whether the control panel should proceed. */

#define CPL_GETCOUNT    2
/*  This message is sent to determine the number of applets to be displayed. */
/*  lParam1 and lParam2 are not defined. */
/*  Return the number of applets you wish to display in the control */
/*  panel window. */

#define CPL_INQUIRE     3
/*  This message is sent for information about each applet. */

/*  A CPL SHOULD HANDLE BOTH THE CPL_INQUIRE AND CPL_NEWINQUIRE MESSAGES. */
/*  The developer must not make any assumptions about the order or dependance */
/*  of CPL inquiries. */

/*  lParam1 is the applet number to register, a value from 0 to */
/*  (CPL_GETCOUNT - 1).  lParam2 is a far ptr to a CPLINFO structure. */
/*  Fill in CPLINFO's idIcon, idName, idInfo and lData fields with */
/*  the resource id for an icon to display, name and description string ids, */
/*  and a long data item associated with applet #lParam1.  This information */
/*  may be cached by the caller at runtime and/or across sessions. */
/*  To prevent caching, see CPL_DYNAMIC_RES, above.  */

#define CPL_SELECT      4
/*  The CPL_SELECT message has been deleted. */

#define CPL_DBLCLK      5
/*  This message is sent when the applet's icon has been double-clicked */
/*  upon.  lParam1 is the applet number which was selected.  lParam2 is the */
/*  applet's lData value. */
/*  This message should initiate the applet's dialog box. */

#define CPL_STOP        6
/*  This message is sent for each applet when the control panel is exiting. */
/*  lParam1 is the applet number.  lParam2 is the applet's lData  value. */
/*  Do applet specific cleaning up here. */

#define CPL_EXIT        7
/*  This message is sent just before the control panel calls FreeLibrary. */
/*  lParam1 and lParam2 are not defined. */
/*  Do non-applet specific cleaning up here. */

#define CPL_NEWINQUIRE	8
/* Same as CPL_INQUIRE execpt lParam2 is a pointer to a NEWCPLINFO struct. */

/*  A CPL SHOULD HANDLE BOTH THE CPL_INQUIRE AND CPL_NEWINQUIRE MESSAGES. */
/*  The developer must not make any assumptions about the order or dependance */
/*  of CPL inquiries. */

#define CPL_STARTWPARMS 9
/* this message parallels CPL_DBLCLK in that the applet should initiate 
** its dialog box.  where it differs is that this invocation is coming
** out of RUNDLL, and there may be some extra directions for execution.
** lParam1: the applet number.
** lParam2: an LPSTR to any extra directions that might exist.
** returns: TRUE if the message was handled; FALSE if not.
*/

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#ifndef RC_INVOKED
#pragma pack()
#endif  /* RC_INVOKED */

#endif  /* _INC_CPL */
