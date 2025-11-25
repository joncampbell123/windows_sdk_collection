//////////////////////////////////////////////////////////////////////////
//
//  pick3.cpp
//
//  This is the windows application that is used to host the 
//  DAViewerControl.
//
//
//  (C) Copyright 1997 by Microsoft Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////////
#define INITGUID
#include <windows.h>

#include "dactl.h"
#include "ctlhost.h"
#include "resource.h"

#define WINDOW_CLASS    "PICK3"
#define WINDOW_TITLE    "PICK3"

VARIANT_BOOL fCaptioningOn = FALSE;
VARIANT_BOOL fStatusOn = TRUE;
VARIANT_BOOL fToolBarOn = TRUE;

// Local Function Prototypes
HRESULT loadDXAControl(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

WORD wLastMenuItem;
CControlHost *ocHost = NULL;               // oc host

//////////////////////////////////////////////////////////////////////////
//
//  WinMain
//
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR szCmdLine, int iCmdShow)
{
    WNDCLASS    wndclass;
    MSG         msg;
    HWND        hwnd;
    OleInitialize(NULL);

    wndclass.style              = CS_OWNDC;
    wndclass.lpfnWndProc        = WndProc;
    wndclass.cbClsExtra         = 0;
    wndclass.cbWndExtra         = sizeof(LONG)*4;
    wndclass.hInstance          = hInstance;
    wndclass.hIcon              = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wndclass.hCursor            = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground      = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wndclass.lpszMenuName       = NULL;
    wndclass.lpszClassName      = WINDOW_CLASS;

    RegisterClass(&wndclass);

    hwnd = CreateWindow(WINDOW_CLASS,
                        WINDOW_TITLE,
                        WS_OVERLAPPED|
                        WS_SYSMENU,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        300,
                        300,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    ocHost = new CControlHost(NULL);

    if (!ocHost)
        return FALSE;

    // Load the control.
    if(FAILED(loadDXAControl(hwnd)) )
         return FALSE;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    OleUninitialize();

    return (msg.wParam);
}

//////////////////////////////////////////////////////////////////////////
HRESULT loadDXAControl(HWND hwnd)
{   
    ocHost->SetHwnd(hwnd);
    return ocHost->CreateControl();
}

//////////////////////////////////////////////////////////////////////////
//
//  WndProc
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(msg == WM_CLOSE || msg == WM_DESTROY)
    {
        if(ocHost)
        {
            ocHost->DeleteControl();
            ocHost = NULL;
        }
        PostQuitMessage(0);
        return (0);
    }
    
    return (DefWindowProc(hwnd, msg, wParam, lParam));
}

