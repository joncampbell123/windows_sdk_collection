/*****************************************************************************\
*                                                                             *
* wfext.h -     Windows File Manager Extensions definitions (Win32 variant)   *
*                                                                             *
*               Version 3.10                                                  *
*                                                                             *
*               Copyright (c) 1991-1993, Microsoft Corp. All rights reserved. *
*                                                                             *
*******************************************************************************/

#ifndef _INC_WFEXT
#define _INC_WFEXT            /* #defined if wfext.h has been included */

#ifdef __cplusplus            /* Assume C declaration for C++ */
extern "C" {
#endif  /* __cplusplus */

#define MENU_TEXT_LEN           40

#define FMMENU_FIRST            1
#define FMMENU_LAST             99

#define FMEVENT_LOAD            100
#define FMEVENT_UNLOAD          101
#define FMEVENT_INITMENU        102
#define FMEVENT_USER_REFRESH    103
#define FMEVENT_SELCHANGE       104
#define FMEVENT_TOOLBARLOAD     105
#define FMEVENT_HELPSTRING      106
#define FMEVENT_HELPMENUITEM    107

#define FMFOCUS_DIR             1
#define FMFOCUS_TREE            2
#define FMFOCUS_DRIVES          3
#define FMFOCUS_SEARCH          4

#define FM_GETFOCUS           (WM_USER + 0x0200)
#define FM_GETDRIVEINFO       (WM_USER + 0x0201)
#define FM_GETSELCOUNT        (WM_USER + 0x0202)
#define FM_GETSELCOUNTLFN     (WM_USER + 0x0203)  /* LFN versions are odd */
#define FM_GETFILESEL         (WM_USER + 0x0204)
#define FM_GETFILESELLFN      (WM_USER + 0x0205)  /* LFN versions are odd */
#define FM_REFRESH_WINDOWS    (WM_USER + 0x0206)
#define FM_RELOAD_EXTENSIONS  (WM_USER + 0x0207)


typedef struct _FMS_GETFILESEL {
   FILETIME ftTime ;
   DWORD dwSize;
   BYTE bAttr;
   CHAR  szName[260];               // always fully qualified
} FMS_GETFILESEL, FAR *LPFMS_GETFILESEL;

typedef struct _FMS_GETDRIVEINFO {       // for drive
   DWORD dwTotalSpace;
   DWORD dwFreeSpace;
   CHAR  szPath[260];                    // current directory
   CHAR  szVolume[14];                   // volume label
   CHAR  szShare[128];                   // if this is a net drive
} FMS_GETDRIVEINFO, FAR *LPFMS_GETDRIVEINFO;

typedef struct _FMS_LOAD {
   DWORD dwSize;                        // for version checks
   CHAR  szMenuName[MENU_TEXT_LEN];     // output
   HMENU hMenu;                         // output
   UINT  wMenuDelta;                    // input
} FMS_LOAD, FAR *LPFMS_LOAD;


// Toolbar definitions

typedef struct tagEXT_BUTTON {
   WORD idCommand;                 /* menu command to trigger */
   WORD idsHelp;                   /* help string ID */
   WORD fsStyle;                   /* button style */
} EXT_BUTTON, FAR *LPEXT_BUTTON;

typedef struct tagFMS_TOOLBARLOAD {
   DWORD dwSize;                   /* for version checks */
   LPEXT_BUTTON lpButtons;         /* output */
   WORD cButtons;                  /* output, 0==>no buttons */
   WORD cBitmaps;                  /* number of non-sep buttons */
   WORD idBitmap;                  /* output */
   HBITMAP hBitmap;                /* output if idBitmap==0 */
} FMS_TOOLBARLOAD, FAR *LPFMS_TOOLBARLOAD;

typedef struct tagFMS_HELPSTRING
{
   INT   idCommand;       /* input, -1==>the menu was selected */
   HMENU hMenu;           /* input, the extensions menu */
   CHAR  szHelp[128];     /* output, the help string */
} FMS_HELPSTRING, FAR *LPFMS_HELPSTRING;

typedef DWORD (APIENTRY *FM_EXT_PROC)(HWND, WORD, LONG);
typedef DWORD (APIENTRY *FM_UNDELETE_PROC)(HWND, LPSTR);

LONG WINAPI FMExtensionProc(HWND hwnd, WORD wEvent, LONG lParam);

#ifdef __cplusplus
}                  /* End of extern "C" { */
#endif             /* __cplusplus */

#endif             /* _INC_WFEXT */

