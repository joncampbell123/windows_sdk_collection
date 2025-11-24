/*****************************************************************************\
*                                                                             *
* shellapi.h -  SHELL.DLL functions, types, and definitions                   *
*                                                                             *
* Copyright (c) 1992-1995, Microsoft Corp.      All rights reserved           *
*                                                                             *
\*****************************************************************************/

#ifndef _INC_SHELLAPI
#define _INC_SHELLAPI

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */
    
    
//
// Define API decoration for direct importing of DLL references.
//
#ifndef WINSHELLAPI
#if !defined(_SHELL32_) && defined(_WIN32)
#define WINSHELLAPI DECLSPEC_IMPORT
#else
#define WINSHELLAPI
#endif
#endif // WINSHELLAPI

    

DECLARE_HANDLE(HDROP);

UINT  WINAPI DragQueryFile(HDROP, UINT, LPSTR, UINT);
BOOL  WINAPI DragQueryPoint(HDROP, POINT FAR*);
void  WINAPI DragFinish(HDROP);
void  WINAPI DragAcceptFiles(HWND, BOOL);

HICON WINAPI ExtractIcon(HINSTANCE hInst, LPCSTR lpszFile, UINT nIconIndex);

/* ShellExecute() and ShellExecuteEx() error codes */

/* regular WinExec() codes */
#define SE_ERR_FNF              2	// file not found
#define SE_ERR_PNF              3	// path not found
#define SE_ERR_OOM              8	// out of memory

/* values beyond the regular WinExec() codes */
#define SE_ERR_SHARE            26
#define SE_ERR_ASSOCINCOMPLETE  27
#define SE_ERR_DDETIMEOUT       28
#define SE_ERR_DDEFAIL          29
#define SE_ERR_DDEBUSY          30
#define SE_ERR_NOASSOC          31
#define SE_ERR_DLLNOTFOUND      32

HINSTANCE WINAPI FindExecutable(LPCSTR lpFile, LPCSTR lpDirectory, LPSTR lpResult);	
HINSTANCE WINAPI ShellExecute(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, int iShowCmd);

int     WINAPI ShellAbout(HWND hWnd, LPCSTR szApp, LPCSTR szOtherStuff, HICON hIcon);
DWORD   WINAPI DoEnvironmentSubst(LPSTR szString, UINT cbString);                                  
LPSTR 	WINAPI FindEnvironmentString(LPSTR szEnvVar);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _INC_SHELLAPI */
