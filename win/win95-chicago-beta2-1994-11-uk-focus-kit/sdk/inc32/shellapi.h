/*****************************************************************************\
*                                                                             *
* shellapi.h -  SHELL.DLL functions, types, and definitions                   *
*                                                                             *
* Copyright (c) 1992-1994, Microsoft Corp.  All rights reserved               *
*                                                                             *
\*****************************************************************************/

#ifndef _INC_SHELLAPI
#define _INC_SHELLAPI


#include <pshpack1.h>

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


DECLARE_HANDLE(HDROP);

#if(WINVER >= 0x0400)
typedef struct _DRAGINFOA {
    UINT uSize;                 /* init with sizeof(DRAGINFO) */
    POINT pt;
    BOOL fNC;
    LPSTR   lpFileList;
    DWORD grfKeyState;
} DRAGINFOA, FAR* LPDRAGINFOA;
typedef struct _DRAGINFOW {
    UINT uSize;                 /* init with sizeof(DRAGINFO) */
    POINT pt;
    BOOL fNC;
    LPWSTR  lpFileList;
    DWORD grfKeyState;
} DRAGINFOW, FAR* LPDRAGINFOW;
#ifdef UNICODE
typedef DRAGINFOW DRAGINFO;
typedef LPDRAGINFOW LPDRAGINFO;
#else
typedef DRAGINFOA DRAGINFO;
typedef LPDRAGINFOA LPDRAGINFO;
#endif // UNICODE

// AppBar stuff
#define ABM_NEW           0x00000000
#define ABM_REMOVE        0x00000001
#define ABM_QUERYPOS      0x00000002
#define ABM_SETPOS        0x00000003
#define ABM_GETSTATE      0x00000004
#define ABM_GETTASKBARPOS 0x00000005

// these are put in the wparam of callback messages
#define ABN_STATECHANGE    0x0000000
#define ABN_POSCHANGED     0x0000001
#define ABN_FULLSCREENAPP  0x0000002
#define ABN_WINDOWARRANGE  0x0000003 // lParam == TRUE means hide

// flags for get state
#define ABS_AUTOHIDE    0x0000001
#define ABS_ALWAYSONTOP 0x0000002

#define ABE_LEFT        0
#define ABE_TOP         1
#define ABE_RIGHT       2
#define ABE_BOTTOM      3

typedef struct _AppBarData
{
    DWORD cbSize;
    HWND hWnd;
    UINT uCallbackMessage;
    UINT uEdge;
    RECT rc;
} APPBARDATA, *PAPPBARDATA;

UINT APIENTRY SHAppBarMessage(DWORD dwMessage, PAPPBARDATA pData);

/* ShellExecute() and ShellExecuteEx() error codes */

/* regular WinExec() codes */
#define SE_ERR_FNF              2       // file not found
#define SE_ERR_PNF              3       // path not found
#define SE_ERR_ACCESSDENIED     5       // access denied
#define SE_ERR_OOM              8       // out of memory
#endif /* WINVER >= 0x0400 */

UINT APIENTRY DragQueryFileA(HDROP,UINT,LPSTR,UINT);
UINT APIENTRY DragQueryFileW(HDROP,UINT,LPWSTR,UINT);
#ifdef UNICODE
#define DragQueryFile  DragQueryFileW
#else
#define DragQueryFile  DragQueryFileA
#endif // !UNICODE

BOOL APIENTRY DragQueryPoint(HDROP,LPPOINT);
VOID APIENTRY DragFinish(HDROP);
VOID APIENTRY DragAcceptFiles(HWND,BOOL);

HICON  APIENTRY ExtractIconA(HINSTANCE hInst, LPCSTR lpszExeFileName, UINT nIconIndex);
HICON  APIENTRY ExtractIconW(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex);
#ifdef UNICODE
#define ExtractIcon  ExtractIconW
#else
#define ExtractIcon  ExtractIconA
#endif // !UNICODE

/* error values for ShellExecute() beyond the regular WinExec() codes */
#define SE_ERR_SHARE		        26
#define SE_ERR_ASSOCINCOMPLETE		27
#define SE_ERR_DDETIMEOUT               28
#define SE_ERR_DDEFAIL                  29
#define SE_ERR_DDEBUSY                  30
#define SE_ERR_NOASSOC                  31
#if(WINVER >= 0x0400)
#define SE_ERR_DLLNOTFOUND              32


#ifdef UNICODE
#define InternalExtractIcon  InternalExtractIconW
#else
#define InternalExtractIcon  InternalExtractIconA
#endif // !UNICODE
DWORD   APIENTRY DoEnvironmentSubstA(LPSTR szString, UINT cbString);
DWORD   APIENTRY DoEnvironmentSubstW(LPWSTR szString, UINT cbString);
#ifdef UNICODE
#define DoEnvironmentSubst  DoEnvironmentSubstW
#else
#define DoEnvironmentSubst  DoEnvironmentSubstA
#endif // !UNICODE
LPSTR APIENTRY FindEnvironmentStringA(LPSTR szEnvVar);
LPWSTR APIENTRY FindEnvironmentStringW(LPWSTR szEnvVar);
#ifdef UNICODE
#define FindEnvironmentString  FindEnvironmentStringW
#else
#define FindEnvironmentString  FindEnvironmentStringA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */

HINSTANCE APIENTRY ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
HINSTANCE APIENTRY ShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
#ifdef UNICODE
#define ShellExecute  ShellExecuteW
#else
#define ShellExecute  ShellExecuteA
#endif // !UNICODE

HINSTANCE APIENTRY FindExecutableA(LPCSTR lpFile, LPCSTR lpDirectory, LPSTR lpResult);
HINSTANCE APIENTRY FindExecutableW(LPCWSTR lpFile, LPCWSTR lpDirectory, LPWSTR lpResult);
#ifdef UNICODE
#define FindExecutable  FindExecutableW
#else
#define FindExecutable  FindExecutableA
#endif // !UNICODE
TCHAR ** APIENTRY CommandLineToArgvW(TCHAR* lpCmdLine, int*pNumArgs);

INT   APIENTRY ShellAboutA(HWND hWnd, LPCSTR szApp, LPCSTR szOtherStuff, HICON hIcon);
INT   APIENTRY ShellAboutW(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff, HICON hIcon);
#ifdef UNICODE
#define ShellAbout  ShellAboutW
#else
#define ShellAbout  ShellAboutA
#endif // !UNICODE


#ifdef UNICODE
#define ExtractAssociatedIcon  ExtractAssociatedIconW
#else
#define ExtractAssociatedIcon  ExtractAssociatedIconA
#endif // !UNICODE


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#include <poppack.h>

#endif  /* _INC_SHELLAPI */

