/*
 * ENUMC.C
 * Enumerator in C Chapter 3
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include <windows.h>
#include <malloc.h>
#include "enumc.h"


/*
 * WinMain
 *
 * Purpose:
 *  Main entry point of application.
 */

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev
    , LPSTR pszCmdLine, int nCmdShow)
    {
    MSG         msg;
    PAPPVARS    pAV;

    pAV=AppVarsConstructor(hInst, hInstPrev, nCmdShow);

    if (NULL==pAV)
        return -1;

    if (AppVarsFInit(pAV))
        {
        while (GetMessage(&msg, NULL, 0,0 ))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }

    AppVarsDestructor(pAV);
    return msg.wParam;
    }





/*
 * EnumWndProc
 *
 * Purpose:
 *  Standard window class procedure.
 */

LRESULT APIENTRY EnumWndProc(HWND hWnd, UINT iMsg, WPARAM wParam
    , LPARAM lParam)
    {
    PAPPVARS    pAV;
    RECT        rc;
    DWORD       cRect;
    BOOL        fRes;

    COMMANDPARAMS(wID, wCode, hWndMsg);

    pAV=(PAPPVARS)GetWindowLong(hWnd, ENUMWL_STRUCTURE);

    switch (iMsg)
        {
        case WM_NCCREATE:
            pAV=(PAPPVARS)(((LPCREATESTRUCT)lParam)->lpCreateParams);
            SetWindowLong(hWnd, ENUMWL_STRUCTURE, (LONG)pAV);
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));


        case WM_DESTROY:
            PostQuitMessage(0);
            break;


        case WM_COMMAND:
            switch (wID)
                {
                case IDM_ENUMCREATE:
                    if (NULL!=pAV->m_pIEnumRect)
                        {
                        pAV->m_pIEnumRect->lpVtbl->Release(pAV
                            ->m_pIEnumRect);
                        }

                    fRes = CreateRECTEnumerator(&pAV->m_pIEnumRect);

                    AppVarsMsg(pAV, fRes ? TEXT("Object created.")
                        : TEXT("Object creation failed."));

                    break;


                case IDM_ENUMRELEASE:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        AppVarsMsg(pAV, TEXT("There is no object."));
                        break;
                        }

                    if (0==pAV->m_pIEnumRect->lpVtbl->Release(pAV
                        ->m_pIEnumRect))
                        {
                        pAV->m_pIEnumRect=NULL;
                        AppVarsMsg(pAV, TEXT("Object released."));
                        }

                    break;


                case IDM_ENUMRUNTHROUGH:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        AppVarsMsg(pAV, TEXT("There is no object."));
                        break;
                        }

                    while (pAV->m_pIEnumRect->lpVtbl->Next(pAV
                        ->m_pIEnumRect, 1, &rc, &cRect))
                        ;

                    AppVarsMsg(pAV, TEXT("Enumeration Complete."));
                    break;


                case IDM_ENUMEVERYTHIRD:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        AppVarsMsg(pAV, TEXT("There is no object."));
                        break;
                        }

                    while (pAV->m_pIEnumRect->lpVtbl->Next(pAV
                        ->m_pIEnumRect , 1, &rc, &cRect))
                        {
                        if (!pAV->m_pIEnumRect->lpVtbl->Skip(pAV
                            ->m_pIEnumRect, 2))
                            break;
                        }

                    AppVarsMsg(pAV, TEXT("Enumeration Complete."));
                    break;


                case IDM_ENUMRESET:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        AppVarsMsg(pAV, TEXT("There is no object."));
                        break;
                        }

                    pAV->m_pIEnumRect->lpVtbl->Reset(pAV
                        ->m_pIEnumRect);
                    AppVarsMsg(pAV, TEXT("Reset complete."));
                    break;


                case IDM_ENUMEXIT:
                    PostMessage(hWnd, WM_CLOSE, 0, 0L);
                    break;
                }
            break;

        default:
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));
        }

    return 0L;
    }




/*
 * AppVarsConstructor
 *
 * Purpose:
 *  Constructor for the APPVARS structure that just stores vital
 *  application information in the class structure.  Initialization
 *  prone to failure happens in CAppVars::Init.
 *
 * Parameters:
 *  hInst           HINSTANCE of the Application from WinMain
 *  hInstPrev       HINSTANCE of a previous instance from WinMain
 *  nCmdShow        UINT specifying how to show the app window,
 *                  from WinMain.
 *
 * Return Value:
 *  PAPPVARS        Pointer to usable APPVARS structure or NULL.
 */

PAPPVARS AppVarsConstructor(HINSTANCE hInst, HINSTANCE hInstPrev
    , UINT nCmdShow)
    {
    PAPPVARS        pAV;

    pAV=(PAPPVARS)malloc(sizeof(APPVARS));

    if (NULL==pAV)
        return NULL;

    pAV->m_hInst     =hInst;
    pAV->m_hInstPrev =hInstPrev;
    pAV->m_nCmdShow  =nCmdShow;

    pAV->m_hWnd=NULL;
    pAV->m_pIEnumRect=NULL;

    return pAV;
    }







/*
 * AppVarsDestructor
 *
 * Purpose:
 *  AppVars destructor cleaning up any previous allocations.
 *
 * Parameters:
 *  pAV             PAPPVARS to clean up.
 */

void AppVarsDestructor(PAPPVARS pAV)
    {
    //Free any object we still hold on to
    if (NULL!=pAV->m_pIEnumRect)
        pAV->m_pIEnumRect->lpVtbl->Release(pAV->m_pIEnumRect);

    if (IsWindow(pAV->m_hWnd))
        DestroyWindow(pAV->m_hWnd);

    free(pAV);

    return;
    }







/*
 * AppVarsFInit
 *
 * Purpose:
 *  Initializes an APPVARS structure by registering window classes,
 *  creating the main window, and doing anything else prone to
 *  failure. If this function fails the caller should guarantee
 *  that the destructor is called.
 *
 * Parameters:
 *  pAV             PAPPVARS to initialize.
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */

BOOL AppVarsFInit(PAPPVARS pAV)
    {
    WNDCLASS    wc;

    if (!pAV->m_hInstPrev)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = (WNDPROC)EnumWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = CBWNDEXTRA;
        wc.hInstance      = pAV->m_hInst;
        wc.hIcon          = LoadIcon(pAV->m_hInst, TEXT("Icon"));
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU);
        wc.lpszClassName  = TEXT("ENUMC");

        if (!RegisterClass(&wc))
            return FALSE;
        }

    pAV->m_hWnd=CreateWindow(TEXT("ENUMC"), TEXT("Enumerator in C")
        , WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW
        ,35, 35, 350, 250, NULL, NULL, pAV->m_hInst, pAV);

    if (NULL==pAV->m_hWnd)
        return FALSE;

    ShowWindow(pAV->m_hWnd, pAV->m_nCmdShow);
    UpdateWindow(pAV->m_hWnd);

    return TRUE;
    }




/*
 * AppVarsMsg
 *
 * Purpose:
 *  Displays a message using a message box.  This is just to
 *  centralize the call to simpify other code.
 *
 * Parameters:
 *  pAV             PAPPVARS of the application.
 *  psz             LPTSTR to the string to display.
 *
 * Return Value:
 *  None
 */

void AppVarsMsg(PAPPVARS pAV, LPTSTR psz)
    {
    MessageBox(pAV->m_hWnd, psz, TEXT("Enum C++"), MB_OK);
    return;
    }
