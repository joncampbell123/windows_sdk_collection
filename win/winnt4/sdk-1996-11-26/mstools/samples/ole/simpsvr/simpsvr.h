//**********************************************************************
// File name: simpsvr.h
//
// Copyright (c) 1993-1996 Microsoft Corporation. All rights reserved.
//**********************************************************************
#define IDM_ABOUT 100
#define IDM_INSERT  101
#define IDM_VERB0 1000


int PASCAL WinMain(HANDLE hInstance,HANDLE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
BOOL InitApplication(HANDLE hInstance);
BOOL InitInstance(HANDLE hInstance, int nCmdShow);
//@@WTK WIN32, UNICODE _export EXPORT
long FAR PASCAL EXPORT MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
long FAR PASCAL EXPORT DocWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//@@WTK WIN32, UNICODE
//BOOL FAR PASCAL export About(HWND hDlg, unsigned message, WORD wParam, LONG lParam);
BOOL FAR PASCAL EXPORT About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
