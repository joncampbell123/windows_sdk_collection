/*** 
*spoly2.cpp
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  This module is the main entry point for the sample IDispatch polygon
*  server, spoly2.exe.
*
*  This program is intended to demonstrate an implementation of the IDispatch
*  interface. Spoly is a very simple app, that implements two simple objects,
*  CPoly and CPoint and exposes their properties and methods for programatic
*  and cross-process access via IDispatch.
*
*Implementation Notes:
*
*****************************************************************************/

#include <stdio.h>
#include <string.h>

#include <windows.h>
#include <ole2.h>
#include <dispatch.h>

#include "common.h"

#include "statbar.h"

#include "spoly.h"
#include "cpoint.h"
#include "cpoly.h"


HANDLE g_hinst = 0;

HWND g_hwndFrame = 0;
HWND g_hwndClient = 0;

char g_szFrameWndClass[] = "FrameWClass";

DWORD g_dwPolyCF = 0L;
DWORD g_dwPointCF = 0L;

IClassFactory FAR* g_ppolyCF = NULL;
IClassFactory FAR* g_ppointCF = NULL;

BOOL g_fExitOnLastRelease = FALSE;

CStatBar FAR* g_psb = NULL;


HRESULT InitOle(void);
void UninitOle(void);
BOOL InitApplication(HANDLE);
BOOL InitInstance(HANDLE, int);

extern "C" int PASCAL WinMain(HANDLE, HANDLE, LPSTR, int);
extern "C" long FAR PASCAL FrameWndProc(HWND, UINT, WPARAM, LPARAM);


extern "C" int PASCAL
WinMain(
    HANDLE hinst,
    HANDLE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    MSG msg;
    int retval;
    HRESULT hresult;

    if(!hPrevInstance)
      if(!InitApplication(hinst))
	return FALSE;

    if((hresult = InitOle()) != NOERROR)
      return FALSE;

    if(!InitInstance(hinst, nCmdShow)){
      retval = FALSE;
      goto LExit;
    }

    if(STRSTR(lpCmdLine, "-Embedding"))
      g_fExitOnLastRelease = TRUE;

    while(GetMessage(&msg, NULL, NULL, NULL)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    CPoly::PolyTerm();

    retval = msg.wParam;

LExit:;
    UninitOle();

    return retval;
}


BOOL
InitApplication(HANDLE hinst)
{
    WNDCLASS  wc;

    wc.style		= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc	= FrameWndProc;
    wc.cbClsExtra	= 0;
    wc.cbWndExtra	= 0;
    wc.hInstance	= hinst;
    wc.hIcon		= LoadIcon(hinst, "SPOLY");
    wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH) (COLOR_APPWORKSPACE+1);
    wc.lpszMenuName	= "SPolyMenu";
    wc.lpszClassName	= g_szFrameWndClass;

    if(!RegisterClass(&wc))
      return FALSE;

    return TRUE;
}


BOOL
InitInstance(HANDLE hinst, int nCmdShow)
{
    g_hinst = hinst;

    // Create a main frame window
    //
    g_hwndFrame = CreateWindow(
      g_szFrameWndClass,
      "IDispatch Polygon Server #2",
      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      NULL,
      NULL,
      hinst,
      NULL);
    if(!g_hwndFrame)
      return FALSE;

    g_hwndClient = GetWindow(g_hwndFrame, GW_CHILD);
    if(!g_hwndClient)
      return FALSE;

    // create the status bar
    //
    g_psb = CStatBar::Create(g_hinst, g_hwndFrame);
    if(!g_psb)
      return FALSE;

    // initialize and show the status bar
    //
    g_psb->SetHeight(GetSystemMetrics(SM_CYCAPTION) - 1);
    g_psb->SetFont(GetStockObject(SYSTEM_FONT));
    g_psb->SetText("");
    g_psb->Show();

    ShowWindow(g_hwndFrame, nCmdShow);

    UpdateWindow(g_hwndFrame);

    return TRUE;
}


/***
*HRESULT InitOle(void)
*Purpose:
*  Initialize Ole, and register our class factories.
*
*Entry:
*  None
*
*Exit:
*  None
*
***********************************************************************/
HRESULT
InitOle()
{
    HRESULT hresult;

    if((hresult = OleInitialize(NULL)) != NOERROR)
      goto LError0;

    // Register the CPoint Class Factory
    //
    if((g_ppointCF = CPointCF::Create()) == NULL){
      hresult = ResultFromScode(E_OUTOFMEMORY);
      goto LError1;
    }

    hresult = CoRegisterClassObject(
      CLSID_CPoint2,
      g_ppointCF,
      CLSCTX_LOCAL_SERVER,
      REGCLS_MULTIPLEUSE,
      &g_dwPointCF);
    if(hresult != NOERROR)
      goto LError1;

    // Register the CPoly Class Factory.
    //
    if((g_ppolyCF = CPolyCF::Create()) == NULL){
      hresult = ResultFromScode(E_OUTOFMEMORY);
      goto LError1;
    }

    hresult = CoRegisterClassObject(
      CLSID_CPoly2,
      g_ppolyCF,
      CLSCTX_LOCAL_SERVER,
      REGCLS_MULTIPLEUSE,
      &g_dwPolyCF);
    if(hresult != NOERROR)
      goto LError1;

    g_ppolyCF->Release();

    g_ppointCF->Release();

    return NOERROR;


LError1:;
    if(g_ppolyCF != NULL)
      g_ppolyCF->Release();

    if(g_ppointCF != NULL)
      g_ppointCF->Release();

    UninitOle();

LError0:;
    return hresult;
}


void
UninitOle()
{
    // Tell Ole to release our class factories.
    //
    if(g_dwPointCF != 0L)
      CoRevokeClassObject(g_dwPointCF);

    if(g_dwPolyCF != 0L)
      CoRevokeClassObject(g_dwPolyCF);

    OleUninitialize();
}


void
FrameWndOnCreate(HWND hwnd)
{
    CLIENTCREATESTRUCT ccs;

    ccs.hWindowMenu = NULL;
    ccs.idFirstChild = IDM_FIRSTCHILD;

    g_hwndClient = CreateWindow(
      "MDICLIENT",
      0,
      WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
      0, 0, 0, 0,
      hwnd,
      (HMENU) 1,
      g_hinst,
      &ccs);
}


void
FrameWndOnSize(HWND hwnd)
{
    RECT rc;
    int height;

    // Get the client rectangle for the frame window
    GetClientRect(hwnd, &rc);

    height = g_psb->GetHeight();

    // adjust the client win to make room for the status bar.
    //
    MoveWindow(
      g_hwndClient,
      rc.left,
      rc.top,
      rc.right - rc.left,
      rc.bottom - rc.top - height,
      TRUE);

    // move the status bar to the bottom of the newly positioned window.
    //
    g_psb->SetX(rc.left);
    g_psb->SetY(rc.bottom - height),
    g_psb->SetWidth(rc.right - rc.left);
    g_psb->Move();
}


extern "C" long FAR PASCAL
FrameWndProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch(message){
    case WM_COMMAND:
      switch(wParam){
      case IDM_DUMP:
	CPoly::PolyDump();
	return 0;

      case IDM_CLEAR:
	InvalidateRect(g_hwndClient, NULL, TRUE);
	return 0;
      }
      break;

    case WM_CREATE:
      FrameWndOnCreate(hwnd);
      break;

    case WM_SIZE:
      FrameWndOnSize(hwnd);
      return 1;

    case WM_PAINT:
      CPoly::PolyDraw();
      break;

    case WM_CLOSE:
      DestroyWindow(hwnd);
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    }
    return DefFrameProc(hwnd, g_hwndClient, message, wParam, lParam);
}
