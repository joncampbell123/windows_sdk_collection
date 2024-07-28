//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;
//
//  codec.h
//
//  Description:
//
//
//
//==========================================================================;

#ifndef _INC_CODEC
#define _INC_CODEC                  // #defined if codec.h has been included

#ifndef RC_INVOKED
#pragma pack(1)                     // assume byte packing throughout
#endif

#ifndef EXTERN_C
#ifdef __cplusplus
    #define EXTERN_C extern "C"
#else
    #define EXTERN_C extern 
#endif
#endif

#ifdef __cplusplus
extern "C"                          // assume C declarations for C++
{
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//  ACM Driver Version:
//
//  the version is a 32 bit number that is broken into three parts as 
//  follows:
//
//      bits 24 - 31:   8 bit _major_ version number
//      bits 16 - 23:   8 bit _minor_ version number
//      bits  0 - 15:   16 bit build number
//
//  this is then displayed as follows (in decimal form):
//
//      bMajor = (BYTE)(dwVersion >> 24)
//      bMinor = (BYTE)(dwVersion >> 16) & 
//      wBuild = LOWORD(dwVersion)
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

#define VERSION_ACM_DRIVER_MAJOR    2
#define VERSION_ACM_DRIVER_MINOR    1
#define VERSION_ACM_DRIVER_BUILD    0

#define VERSION_ACM_DRIVER      MAKE_ACM_VERSION(VERSION_ACM_DRIVER_MAJOR,  \
                                                 VERSION_ACM_DRIVER_MINOR,  \
                                                 VERSION_ACM_DRIVER_BUILD)

//
//  version of the ACM that this driver was designed for (requires)..
//
#define VERSION_MSACM       MAKE_ACM_VERSION(2, 1, 0)



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
// 
//  Win 16/32 portability stuff...
// 
// 
// 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

#ifndef WIN32
    #ifndef FNLOCAL
        #define FNLOCAL     NEAR PASCAL
        #define FNCLOCAL    NEAR _cdecl
        #define FNGLOBAL    FAR PASCAL
        #define FNCGLOBAL   FAR _cdecl
    #ifdef _WINDLL
        #define FNCALLBACK  FAR PASCAL _loadds
        #define FNEXPORT    FAR PASCAL _export
    #else
        #define FNCALLBACK  FAR PASCAL
        #define FNEXPORT    FAR PASCAL _export
    #endif
    #endif

    //
    //
    //
    //
    #ifndef FIELD_OFFSET
    #define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))
    #endif

    //
    //  based code makes since only in win 16 (to try and keep stuff out of
    //  our fixed data segment...
    //
    #define BCODE           _based(_segname("_CODE"))
    #define BDATA           _based(_segname("_DATA"))
    #define BSTACK          _based(_segname("_STACK"))

    #define HUGE            _huge


    //
    //  stuff for unicode in Win 32--make it a noop in Win 16
    //
    #ifndef _TCHAR_DEFINED
        #define _TCHAR_DEFINED
        typedef char            TCHAR, *PTCHAR;
        typedef unsigned char   TBYTE, *PTUCHAR;

        typedef PSTR            PTSTR, PTCH;
        typedef const PTSTR     PCTSTR;
        typedef LPSTR           LPTSTR, LPTCH;
        typedef LPCSTR          LPCTSTR;
    #endif

    #define TEXT(a)         a
    #define SIZEOF(x)       sizeof(x)

    //
    //  common message cracker macros available in windowx.h on NT--these
    //  should be added to the Win 16 windowsx.h and probably will be
    //  in the future.
    //
    //  there is a windowsx.h16 that ships with the NT PDK that defines
    //  these macros. so if that version is being used, don't redefine
    //  message crackers.
    //

#ifndef WM_CTLCOLORMSGBOX
    #define WM_CTLCOLORMSGBOX           0x0132
    #define WM_CTLCOLOREDIT             0x0133
    #define WM_CTLCOLORLISTBOX          0x0134
    #define WM_CTLCOLORBTN              0x0135
    #define WM_CTLCOLORDLG              0x0136
    #define WM_CTLCOLORSCROLLBAR        0x0137
    #define WM_CTLCOLORSTATIC           0x0138
#endif

#ifndef GET_WM_ACTIVATE_STATE
    #define GET_WM_ACTIVATE_STATE(wp, lp)           (wp)
    #define GET_WM_ACTIVATE_FMINIMIZED(wp, lp)      (BOOL)HIWORD(lp)
    #define GET_WM_ACTIVATE_HWND(wp, lp)            (HWND)LOWORD(lp)
    #define GET_WM_ACTIVATE_MPS(s, fmin, hwnd)      (WPARAM)(s), MAKELONG(hwnd, fmin)

    #define GET_WM_CHARTOITEM_CHAR(wp, lp)          (CHAR)(wp)
    #define GET_WM_CHARTOITEM_POS(wp, lp)           HIWORD(lp)
    #define GET_WM_CHARTOITEM_HWND(wp, lp)          (HWND)LOWORD(lp)
    #define GET_WM_CHARTOITEM_MPS(ch, pos, hwnd)    (WPARAM)(ch), MAKELONG(hwnd, pos)

    #define GET_WM_COMMAND_ID(wp, lp)               (wp)
    #define GET_WM_COMMAND_HWND(wp, lp)             (HWND)LOWORD(lp)
    #define GET_WM_COMMAND_CMD(wp, lp)              HIWORD(lp)
    #define GET_WM_COMMAND_MPS(id, hwnd, cmd)       (WPARAM)(id), MAKELONG(hwnd, cmd)

    #define GET_WM_CTLCOLOR_HDC(wp, lp, msg)        (HDC)(wp)
    #define GET_WM_CTLCOLOR_HWND(wp, lp, msg)       (HWND)LOWORD(lp)
    #define GET_WM_CTLCOLOR_TYPE(wp, lp, msg)       HIWORD(lp)
    #define GET_WM_CTLCOLOR_MPS(hdc, hwnd, type)    (WPARAM)(hdc), MAKELONG(hwnd, type)

    #define GET_WM_MENUSELECT_CMD(wp, lp)           (wp)
    #define GET_WM_MENUSELECT_FLAGS(wp, lp)         LOWORD(lp)
    #define GET_WM_MENUSELECT_HMENU(wp, lp)         (HMENU)HIWORD(lp)
    #define GET_WM_MENUSELECT_MPS(cmd, f, hmenu)    (WPARAM)(cmd), MAKELONG(f, hmenu)

    // Note: the following are for interpreting MDIclient to MDI child messages.
    #define GET_WM_MDIACTIVATE_FACTIVATE(hwnd, wp, lp)  (BOOL)(wp)
    #define GET_WM_MDIACTIVATE_HWNDDEACT(wp, lp)        (HWND)HIWORD(lp)
    #define GET_WM_MDIACTIVATE_HWNDACTIVATE(wp, lp)     (HWND)LOWORD(lp)

    // Note: the following is for sending to the MDI client window.
    #define GET_WM_MDIACTIVATE_MPS(f, hwndD, hwndA) (WPARAM)(hwndA), 0

    #define GET_WM_MDISETMENU_MPS(hmenuF, hmenuW)   0, MAKELONG(hmenuF, hmenuW)

    #define GET_WM_MENUCHAR_CHAR(wp, lp)            (CHAR)(wp)
    #define GET_WM_MENUCHAR_HMENU(wp, lp)           (HMENU)LOWORD(lp)
    #define GET_WM_MENUCHAR_FMENU(wp, lp)           (BOOL)HIWORD(lp)
    #define GET_WM_MENUCHAR_MPS(ch, hmenu, f)       (WPARAM)(ch), MAKELONG(hmenu, f)

    #define GET_WM_PARENTNOTIFY_MSG(wp, lp)         (wp)
    #define GET_WM_PARENTNOTIFY_ID(wp, lp)          HIWORD(lp)
    #define GET_WM_PARENTNOTIFY_HWNDCHILD(wp, lp)   (HWND)LOWORD(lp)
    #define GET_WM_PARENTNOTIFY_X(wp, lp)           (INT)LOWORD(lp)
    #define GET_WM_PARENTNOTIFY_Y(wp, lp)           (INT)HIWORD(lp)
    #define GET_WM_PARENTNOTIFY_MPS(msg, id, hwnd)  (WPARAM)(msg), MAKELONG(hwnd, id)
    #define GET_WM_PARENTNOTIFY2_MPS(msg, x, y)     (WPARAM)(msg), MAKELONG(x, y)

    #define GET_WM_VKEYTOITEM_CODE(wp, lp)          (wp)
    #define GET_WM_VKEYTOITEM_ITEM(wp, lp)          (INT)HIWORD(lp)
    #define GET_WM_VKEYTOITEM_HWND(wp, lp)          (HWND)LOWORD(lp)
    #define GET_WM_VKEYTOITEM_MPS(code, item, hwnd) (WPARAM)(code), MAKELONG(hwnd, item)

    #define GET_EM_SETSEL_START(wp, lp)             LOWORD(lp)
    #define GET_EM_SETSEL_END(wp, lp)               HIWORD(lp)
    #define GET_EM_SETSEL_MPS(iStart, iEnd)         0, MAKELONG(iStart, iEnd)

    #define GET_EM_LINESCROLL_MPS(vert, horz)       0, MAKELONG(vert, horz)

    #define GET_WM_CHANGECBCHAIN_HWNDNEXT(wp, lp)   (HWND)LOWORD(lp)

    #define GET_WM_HSCROLL_CODE(wp, lp)             (wp)
    #define GET_WM_HSCROLL_POS(wp, lp)              LOWORD(lp)
    #define GET_WM_HSCROLL_HWND(wp, lp)             (HWND)HIWORD(lp)
    #define GET_WM_HSCROLL_MPS(code, pos, hwnd)     (WPARAM)(code), MAKELONG(pos, hwnd)

    #define GET_WM_VSCROLL_CODE(wp, lp)             (wp)
    #define GET_WM_VSCROLL_POS(wp, lp)              LOWORD(lp)
    #define GET_WM_VSCROLL_HWND(wp, lp)             (HWND)HIWORD(lp)
    #define GET_WM_VSCROLL_MPS(code, pos, hwnd)     (WPARAM)(code), MAKELONG(pos, hwnd)
#endif

#else
    #ifndef FNLOCAL
        #define FNLOCAL     _stdcall
        #define FNCLOCAL    _stdcall
        #define FNGLOBAL    _stdcall
        #define FNCGLOBAL   _stdcall
        #define FNCALLBACK  CALLBACK
        #define FNEXPORT    CALLBACK
    #endif

    #define HUGE

    //
    //  we compile with UNICODE--we are a mm driver...
    //
    #ifdef UNICODE
        #define SIZEOF(x)   (sizeof(x)/sizeof(WCHAR))
    #else
        #define SIZEOF(x)   sizeof(x)
    #endif

    #define BCODE
    #define BDATA
    #define BSTACK

    //
    //  win32 doesn't have to worry about 'huge' data
    //
    void *memcpy(void *dest, void *src, size_t count);
    #define hmemcpy     memcpy
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//  misc defines for misc sizes and things...
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

//
//  bilingual. this allows the same identifier to be used in resource files
//  and code without having to decorate the id in your code.
//
#ifdef RC_INVOKED
    #define RCID(id)    id
#else
    #define RCID(id)    MAKEINTRESOURCE(id)
#endif


//
//
//
#define SIZEOF_ARRAY(ar)            (sizeof(ar)/sizeof((ar)[0]))


//
//  macros to compute block alignment and convert between samples and bytes
//  of PCM data. note that these macros assume:
//
//      wBitsPerSample  =  8 or 16
//      nChannels       =  1 or 2
//
//  the pwf argument is a pointer to a PCMWAVEFORMAT structure.
//
#define PCM_BLOCKALIGNMENT(pwfx)        (UINT)(((pwfx)->wBitsPerSample >> 3) << ((pwfx)->nChannels >> 1))
#define PCM_AVGBYTESPERSEC(pwfx)        (DWORD)((pwfx)->nSamplesPerSec * (pwfx)->nBlockAlign)
#define PCM_BYTESTOSAMPLES(pwfx, cb)    (DWORD)(cb / PCM_BLOCKALIGNMENT(pwfx))
#define PCM_SAMPLESTOBYTES(pwfx, dw)    (DWORD)(dw * PCM_BLOCKALIGNMENT(pwfx))



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

typedef struct tDRIVERINSTANCE
{
    //
    //  although not required, it is suggested that the first two members
    //  of this structure remain as fccType and DriverProc _in this order_.
    //  the reason for this is that the driver will be easier to combine
    //  with other types of drivers (defined by AVI) in the future.
    //
    FOURCC          fccType;        // type of driver: 'audc'
    DRIVERPROC      fnDriverProc;   // driver proc for the instance

    //
    //  the remaining members of this structure are entirely open to what
    //  your driver requires.
    //
    HDRVR           hdrvr;          // driver handle we were opened with
    DWORD           vdwACM;         // current version of ACM opening you
    DWORD           fdwOpen;        // flags from open description

    LPDRVCONFIGINFO pdci;
    DWORD           fdwConfig;      // driver instance configuration flags

} DRIVERINSTANCE, *PDRIVERINSTANCE, FAR *LPDRIVERINSTANCE;



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 


//
//
//
//
typedef LRESULT (FNGLOBAL *STREAMCONVERTPROC)
(
    LPACMDRVSTREAMINSTANCE  padsi,
    LPACMDRVSTREAMHEADER    padsh
);


//
//
//
//
typedef struct tSTREAMINSTANCE
{
    STREAMCONVERTPROC   fnConvert;  // stream instance conversion proc
    DWORD               fdwConfig;  // stream instance configuration flags


    //
    //  only used on echo filter..
    //
    BYTE _huge *        hpbHistory;
    DWORD               dwPlace;
    DWORD		dwHistoryDone;
} STREAMINSTANCE, *PSTREAMINSTANCE, FAR *LPSTREAMINSTANCE;



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//  resource id's
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

#define ICON_ACM_DRIVER             RCID(10)

#define IDS_ACM_DRIVER_SHORTNAME    (1)     // ACMDRIVERDETAILS.szShortName
#define IDS_ACM_DRIVER_LONGNAME     (2)     // ACMDRIVERDETAILS.szLongName
#define IDS_ACM_DRIVER_COPYRIGHT    (3)     // ACMDRIVERDETAILS.szCopyright
#define IDS_ACM_DRIVER_LICENSING    (4)     // ACMDRIVERDETAILS.szLicensing
#define IDS_ACM_DRIVER_FEATURES     (5)     // ACMDRIVERDETAILS.szFeatures

//
//  ACMFILTERTAGDETAILS.szFilterTag
//
//
#define IDS_ACM_DRIVER_TAG_NAME_VOLUME  (20)
#define IDS_ACM_DRIVER_FORMAT_VOLUME    (21)

//
//  ACMFILTERTAGDETAILS.szFilterTag
//
//
#define IDS_ACM_DRIVER_TAG_NAME_ECHO    (40)
#define IDS_ACM_DRIVER_FORMAT_ECHO      (41)


//
//  global variables, etc...
//
extern HINSTANCE    ghinst;         // DLL instance handle (module in Win 16)



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 
//
//
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ; 

#ifndef RC_INVOKED
#pragma pack()                      // revert to default packing
#endif

#ifdef __cplusplus
}                                   // end of extern "C" { 
#endif

#endif // _INC_CODEC

