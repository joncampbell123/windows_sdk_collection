/*
 * STDEMO.C
 * StatStrip Test Version 1.00
 *
 * Main entry code for application.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include <windows.h>
#include <commdlg.h>
#include <memory.h>
#include <stastrip.h>
#include "stdemo.h"




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
    BOOL        fSuccess=TRUE;
    PAPPVARS    pAV;

    //Attempt to allocate and initialize the application
    pAV=AppPAllocate(&fSuccess, hInst, hInstPrev, pszCmdLine
        , nCmdShow);

    if (NULL==pAV || !fSuccess)
        {
        AppPFree(pAV);
        return 0;
        }

    while (GetMessage(&msg, NULL, 0,0 ))
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }

    AppPFree(pAV);
    return msg.wParam;
    }





/*
 * StatDemoWndProc
 *
 * Purpose:
 *  Window class procedure.  Standard callback.
 */

LRESULT APIENTRY StatDemoWndProc(HWND hWnd, UINT iMsg
    , WPARAM wParam, LPARAM lParam)
    {
    PAPPVARS    pAV;
    TCHAR       szTemp[128];
    UINT        dx, dy;
    CHOOSEFONT  cf;
    LOGFONT     lf;
    HFONT       hFontT;

    COMMANDPARAMS(wID, wCode, hWndMsg);

    pAV=(PAPPVARS)GetWindowLong(hWnd, 0);

    switch (iMsg)
        {
        case WM_NCCREATE:
            pAV=(PAPPVARS)((LONG)((LPCREATESTRUCT)lParam)
                ->lpCreateParams);
            SetWindowLong(hWnd, 0, (LONG)pAV);
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));


        case WM_DESTROY:
            PostQuitMessage(0);
            break;


        case WM_COMMAND:
            switch (wID)
                {
                case IDM_MESSAGESET:
                    SetWindowText(pAV->hWndST
                        , TEXT("This is a message"));
                    break;

                case IDM_MESSAGEGET:
                    GetWindowText(pAV->hWndST, szTemp
                        ,sizeof(szTemp));
                    MessageBox(hWnd, szTemp
                        , TEXT("StatStrip Message"), MB_OK);
                    break;

                case IDM_MESSAGEGETLENGTH:
                    dx=GetWindowTextLength(pAV->hWndST);
                    wsprintf(szTemp, TEXT("Length=%d"), dx);
                    MessageBox(hWnd, szTemp
                        , TEXT("StatStrip Message Length"), MB_OK);
                    break;


                case IDM_MESSAGEENABLE:
                    EnableWindow(pAV->hWndST, TRUE);
                    break;


                case IDM_MESSAGEDISABLE:
                    EnableWindow(pAV->hWndST, FALSE);
                    break;


                case IDM_MESSAGESETFONT:
                    hFontT=(HFONT)SendMessage(pAV->hWndST
                        , WM_GETFONT, 0, 0L);

                    memset(&cf, 0, sizeof(CHOOSEFONT));
                    memset(&lf, 0, sizeof(LOGFONT));
                    GetObject(hFontT, sizeof(LOGFONT), (LPVOID)&lf);

                    cf.lStructSize=sizeof(CHOOSEFONT);
                    cf.hwndOwner=hWnd;
                    cf.lpLogFont=&lf;
                    cf.Flags=CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;

                    if (ChooseFont(&cf))
                        {
                        hFontT=pAV->hFont;

                        pAV->hFont=CreateFontIndirect(&lf);

                        if (NULL!=pAV->hFont)
                            {
                            SendMessage(pAV->hWndST, WM_SETFONT
                                , (WPARAM)pAV->hFont, 1L);

                            if (NULL!=hFontT)
                                DeleteObject(hFontT);
                            }
                        else
                            pAV->hFont=hFontT;
                        }

                    break;
                }
            break;


        case WM_MENUSELECT:
            StatStripMenuSelect(pAV->hWndST, wParam, lParam);
            break;


        case WM_SIZE:
            dx=LOWORD(lParam);
            dy=HIWORD(lParam);

            //Change the StatStrip's size and position
            SetWindowPos(pAV->hWndST, NULL, 0, dy-CYSTATSTRIP
                , dx, CYSTATSTRIP, SWP_NOZORDER);
            break;


        default:
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));
        }

    return 0L;
    }
