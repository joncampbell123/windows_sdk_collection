
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

#include "resource.h"

#if defined(WIN16)
#define APIENTRY FAR PASCAL
typedef int (APIENTRY *WNDPROC)();
typedef WORD WPARAM;
typedef VOID *PVOID;
typedef WORD UINT;
#elif defined(_MAC)
#ifdef APIENTRY
#undef APIENTRY
#endif
#define APIENTRY CALLBACK
#endif

#define APPCLASS   "GDIDEMO"
#define APPTITLE   "Windows GDI Demonstration"

#define COLOR_SCALE_RED     1
#define COLOR_SCALE_GREEN   2
#define COLOR_SCALE_BLUE    3
#define COLOR_SCALE_GRAY    4

#define CLIENTWND           0


/*
** MAIN WINDOW ROUTINES (gdidemo.c)
*/
LONG  APIENTRY WndProc(HWND,UINT,WPARAM,LONG);
#if defined(_MAC)
VOID		   SetupMacHelp(HINSTANCE, HWND);
#endif
BOOL           CreateProc(HWND);
VOID           DestroyProc(HWND);
BOOL           CommandProc(HWND,WPARAM,LONG);
VOID           PaintProc(HWND);
DWORD FAR      lRandom(VOID);


/*
** INITIALIZATION ROUTINES (init.c)
*/
BOOL FAR RegisterAppClass(HANDLE);
VOID FAR UnregisterAppClass(HANDLE);
HWND FAR CreateAppWindow(HANDLE);
HWND FAR CreateMDIClientWindow(HWND);



/*
** DIALOG ROUTINES (dialog.c)
*/
BOOL APIENTRY AboutDlgProc(HWND,UINT,WPARAM,LONG);
int  FAR      DisplayDialogBox(HWND,LPSTR,WNDPROC,LONG);
VOID          PaintWindow(HWND,HPALETTE,int);
HPALETTE      CreateColorScalePalette(HDC,int);


/*
** WINDOW ROUTINES (wininfo.c)
*/
BOOL  FAR AllocWindowInfo(HWND,WORD);
PVOID FAR LockWindowInfo(HWND);
BOOL  FAR UnlockWindowInfo(HWND);
BOOL  FAR FreeWindowInfo(HWND);




#ifdef TESTDEBUG
#define DEBUGOUT(lpszString) OutputDebugString(lpszString)
#else
#define DEBUGOUT(lpszString) {}
#endif


/*
** Porting macros...
*/
#if defined(_WIN32) || defined(WIN32)
#define GETINSTANCE(hWnd)   (HANDLE)GetWindowLong(hWnd,GWL_HINSTANCE)
#define GETCLASSBRUSH(hWnd) (HBRUSH)GetClassLong(hWnd,GCL_HBRBACKGROUND)
#else
#define GETINSTANCE(hWnd)   (HANDLE)GetWindowWord(hWnd,GWW_HINSTANCE)
#define GETCLASSBRUSH(hWnd) (HBRUSH)GetClassWord(hWnd,GCW_HBRBACKGROUND)
#endif
