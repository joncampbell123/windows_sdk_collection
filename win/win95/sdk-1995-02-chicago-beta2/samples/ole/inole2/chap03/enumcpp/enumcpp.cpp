/*
 * ENUMCPP.CPP
 * Enumerator in C++ Chapter 3
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
#include "enumcpp.h"


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

    pAV=new CAppVars(hInst, hInstPrev, nCmdShow);

    if (NULL==pAV)
        return -1;

    if (pAV->FInit())
        {
        while (GetMessage(&msg, NULL, 0,0 ))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }

    delete pAV;
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
                        pAV->m_pIEnumRect->Release();

                    fRes=CreateRECTEnumerator(&pAV->m_pIEnumRect);

                    pAV->Message(fRes ? TEXT("Object created.")
                        : TEXT("Object creation failed."));

                    break;


                case IDM_ENUMRELEASE:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        pAV->Message(TEXT("There is no object."));
                        break;
                        }

                    if (0==pAV->m_pIEnumRect->Release())
                        {
                        pAV->m_pIEnumRect=NULL;
                        pAV->Message(TEXT("Object released."));
                        }

                    break;


                case IDM_ENUMRUNTHROUGH:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        pAV->Message(TEXT("There is no object."));
                        break;
                        }

                    while (pAV->m_pIEnumRect->Next(1, &rc, &cRect))
                        ;

                    pAV->Message(TEXT("Enumeration complete."));
                    break;


                case IDM_ENUMEVERYTHIRD:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        pAV->Message(TEXT("There is no object."));
                        break;
                        }

                    while (pAV->m_pIEnumRect->Next(1, &rc, &cRect))
                        {
                        if (!pAV->m_pIEnumRect->Skip(2))
                            break;
                        }

                    pAV->Message(TEXT("Enumeration complete."));
                    break;


                case IDM_ENUMRESET:
                    if (NULL==pAV->m_pIEnumRect)
                        {
                        pAV->Message(TEXT("There is no object."));
                        break;
                        }

                    pAV->m_pIEnumRect->Reset();
                    pAV->Message(TEXT("Reset complete."));
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
 * CAppVars::CAppVars
 * CAppVars::~CAppVars
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the Application from WinMain
 *  hInstPrev       HINSTANCE of a previous instance from WinMain
 *  nCmdShow        UINT specifying how to show the app window,
 *                  from WinMain.
 */

CAppVars::CAppVars(HINSTANCE hInst, HINSTANCE hInstPrev
    , UINT nCmdShow)
    {
    //Initialize WinMain parameter holders.
    m_hInst     =hInst;
    m_hInstPrev =hInstPrev;
    m_nCmdShow  =nCmdShow;

    m_hWnd=NULL;
    m_pIEnumRect=NULL;

    return;
    }


CAppVars::~CAppVars(void)
    {
    //Free the enumerator object if we have one.
    if (NULL!=m_pIEnumRect)
        m_pIEnumRect->Release();

    return;
    }






/*
 * CAppVars::FInit
 *
 * Purpose:
 *  Initializes an CAppVars object by registering window classes,
 *  creating the main window, and doing anything else prone to
 *  failure.  If this function fails the caller should guarantee
 *  that the destructor is called.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */

BOOL CAppVars::FInit(void)
    {
    WNDCLASS    wc;

    if (!m_hInstPrev)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = EnumWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = CBWNDEXTRA;
        wc.hInstance      = m_hInst;
        wc.hIcon          = LoadIcon(m_hInst, TEXT("Icon"));
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU);
        wc.lpszClassName  = TEXT("ENUMCPP");

        if (!RegisterClass(&wc))
            return FALSE;
        }

    m_hWnd=CreateWindow(TEXT("ENUMCPP"), TEXT("Enumerator in C++")
        , WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW
        ,35, 35, 350, 250, NULL, NULL, m_hInst, this);

    if (NULL==m_hWnd)
        return FALSE;

    ShowWindow(m_hWnd, m_nCmdShow);
    UpdateWindow(m_hWnd);

    return TRUE;
    }




/*
 * CAppVars::Message
 *
 * Purpose:
 *  Displays a message using a message box.  This is just to
 *  centralize the call to simpify other code.
 *
 * Parameters:
 *  psz             LPTSTR to the string to display.
 *
 * Return Value:
 *  None
 */

void inline CAppVars::Message(LPTSTR psz)
    {
    MessageBox(m_hWnd, psz, TEXT("Enum C++"), MB_OK);
    return;
    }
