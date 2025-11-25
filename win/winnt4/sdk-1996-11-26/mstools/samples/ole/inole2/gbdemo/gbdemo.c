/*
 * GBDEMO.C
 * GizmoBar Test Version 1.01
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
#include "gbdemo.h"


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
 * GBDemoWndProc
 *
 * Purpose:
 *  Window class procedure.  Standard callback.
 *
 * Parameters:
 *  Standard
 *
 * Return Value:
 *  Standard
 */

LRESULT APIENTRY GBDemoWndProc(HWND hWnd, UINT iMsg, WPARAM wParam
    , LPARAM lParam)
    {
    static int  cShow=0;
    PAPPVARS    pAV;
    LPTSTR      psz;
    TCHAR       szTemp[128];
    UINT        dx, dy;
    BOOL        fTrans;
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


        case WM_ERASEBKGND:
            //Eat this since we never paint on this window.
            break;


        case WM_COMMAND:
            switch (wID)
                {
                case IDM_GIZMOSCREATE:
                    dx=pAV->tdd.cxButton;
                    dy=pAV->tdd.cyButton;

                    SendMessage(pAV->hWndGB, WM_SETREDRAW, FALSE
                        , 0L);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_TEXT
                        , pAV->iGizmo++, GIZMOID_TEXT, 40, 16
                        , TEXT("T&ext:"), NULL, 0, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_LISTBOX
                        , pAV->iGizmo++, GIZMOID_LISTBOX, 100, 100
                        , TEXT("List"), NULL, 0, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_EDIT
                        , pAV->iGizmo++, GIZMOID_EDIT, 50
                        , pAV->tdd.cyBar-5, TEXT("Edit"), NULL, 0
                        , GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_COMBOBOX
                        , pAV->iGizmo++, GIZMOID_COMBOBOX, 120, 100
                        , TEXT("Combobox"), NULL, 0, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_SEPARATOR
                        , pAV->iGizmo++, GIZMOID_SEPARATOR1, 6, dy
                        , NULL, NULL, 0, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_BUTTONNORMAL
                        , pAV->iGizmo++, GIZMOID_BUTTON1, 36
                        , pAV->tdd.cyBar-5, TEXT("Go!"), NULL, 3
                        , GIZMO_NORMAL);


                    //File new, open, save.
                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_BUTTONCOMMAND
                        , pAV->iGizmo++, GIZMOID_BUTTON2, dx, dy
                        , NULL, NULL, 3, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_BUTTONCOMMAND
                        , pAV->iGizmo++, GIZMOID_BUTTON3, dx, dy
                        , NULL, NULL, 4, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_BUTTONCOMMAND
                        , pAV->iGizmo++, GIZMOID_BUTTON4, dx, dy
                        , NULL, NULL, 5, GIZMO_NORMAL);

                    //Bold, italic, underline
                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_SEPARATOR
                        , pAV->iGizmo++, GIZMOID_SEPARATOR2, 6, dy
                        , NULL, NULL, 0, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB
                        , GIZMOTYPE_BUTTONATTRIBUTEIN, pAV->iGizmo++
                        , GIZMOID_BUTTON5, dx, dy, NULL, pAV->hBmp
                        , 0, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB
                        , GIZMOTYPE_BUTTONATTRIBUTEIN, pAV->iGizmo++
                        , GIZMOID_BUTTON6, dx, dy, NULL, pAV->hBmp
                        , 1, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB
                        , GIZMOTYPE_BUTTONATTRIBUTEIN, pAV->iGizmo++
                        , GIZMOID_BUTTON7, dx, dy, NULL, pAV->hBmp
                        , 2, GIZMO_NORMAL);


                    //Left, center, right, justify
                    GBGizmoAdd(pAV->hWndGB, GIZMOTYPE_SEPARATOR
                        , pAV->iGizmo++, GIZMOID_SEPARATOR3, 6, dy
                        , NULL, NULL, 0, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB
                        , GIZMOTYPE_BUTTONATTRIBUTEEX, pAV->iGizmo++
                        , GIZMOID_BUTTON8, dx, dy, NULL, pAV->hBmp
                        , 3, ATTRIBUTEBUTTON_DOWN);

                    GBGizmoAdd(pAV->hWndGB
                        , GIZMOTYPE_BUTTONATTRIBUTEEX, pAV->iGizmo++
                        , GIZMOID_BUTTON9, dx, dy, NULL, pAV->hBmp
                        , 4, GIZMO_NORMAL);

                    GBGizmoAdd(pAV->hWndGB
                        , GIZMOTYPE_BUTTONATTRIBUTEEX, pAV->iGizmo++
                        , GIZMOID_BUTTON10, dx, dy, NULL, pAV->hBmp
                        , 5, GIZMO_NORMAL);

                    SendMessage(pAV->hWndGB, WM_SETREDRAW, TRUE, 0L);

                    GBGizmoAdd(pAV->hWndGB
                        , GIZMOTYPE_BUTTONATTRIBUTEEX, pAV->iGizmo++
                        , GIZMOID_BUTTON11, dx, dy, NULL, pAV->hBmp
                        , 6, GIZMO_NORMAL);

                    EnableMenuItem(GetMenu(hWnd), IDM_GIZMOSCREATE
                        , MF_BYCOMMAND | MF_GRAYED | MF_DISABLED);

                    GBGizmoFocusSet(pAV->hWndGB, GIZMOID_LISTBOX);
                    break;


                case IDM_GIZMOSDISABLEINDIVIDUALLY:
                    SendMessage(pAV->hWndGB, WM_SETREDRAW, FALSE,0L);

                    for (wID=GIZMOID_MIN; wID <= GIZMOID_MAX; wID++)
                        GBGizmoEnable(pAV->hWndGB, wID, FALSE);

                    SendMessage(pAV->hWndGB, WM_SETREDRAW, TRUE, 0L);
                    InvalidateRect(pAV->hWndGB, NULL, FALSE);
                    UpdateWindow(pAV->hWndGB);
                    break;

                case IDM_GIZMOSENABLEINDIVIDUALLY:
                    SendMessage(pAV->hWndGB, WM_SETREDRAW, FALSE,0L);

                    for (wID=GIZMOID_MIN; wID <= GIZMOID_MAX; wID++)
                        GBGizmoEnable(pAV->hWndGB, wID, TRUE);

                    SendMessage(pAV->hWndGB, WM_SETREDRAW, TRUE, 0L);
                    InvalidateRect(pAV->hWndGB, NULL, FALSE);
                    UpdateWindow(pAV->hWndGB);
                    break;


                case IDM_GIZMOSDISABLEALL:
                    EnableWindow(pAV->hWndGB, FALSE);
                    break;


                case IDM_GIZMOSENABLEALL:
                    EnableWindow(pAV->hWndGB, TRUE);
                    break;


                case IDM_GIZMOSCHANGEFONT:
                    hFontT=(HFONT)SendMessage(pAV->hWndGB
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
                            SendMessage(pAV->hWndGB, WM_SETFONT
                                , (WPARAM)pAV->hFont, 1L);

                            if (NULL!=hFontT)
                                DeleteObject(hFontT);
                            }
                        else
                            pAV->hFont=hFontT;
                        }

                    break;


                case IDM_GIZMOSSHOWHIDELISTBOX:
                    GBGizmoShow(pAV->hWndGB, GIZMOID_LISTBOX
                        , (0x0001 & cShow++));
                    break;

                case IDM_GIZMOSSENDMESSAGES:
                    GBGizmoSendMessage(pAV->hWndGB, GIZMOID_LISTBOX
                        , CB_ADDSTRING, 0
                        , (LONG)(LPTSTR)TEXT("Llamma"));
                    GBGizmoSendMessage(pAV->hWndGB, GIZMOID_LISTBOX
                        , CB_ADDSTRING, 0
                        , (LONG)(LPTSTR)TEXT("Camel"));
                    GBGizmoSendMessage(pAV->hWndGB, GIZMOID_LISTBOX
                        , CB_ADDSTRING, 0
                        , (LONG)(LPTSTR)TEXT("Dromedary"));

                    GBGizmoSendMessage(pAV->hWndGB, GIZMOID_EDIT
                        , EM_LIMITTEXT, 100, 0L);
                    GBGizmoSendMessage(pAV->hWndGB, GIZMOID_COMBOBOX
                        , CB_DIR, DDL_DIRECTORY | DDL_DRIVES
                        , (LONG)(LPTSTR)TEXT("*.*"));
                    break;

                case IDM_GIZMOSNOTIFYON:
                    for (wID=GIZMOID_MIN; wID <= GIZMOID_MAX; wID++)
                        GBGizmoNotifySet(pAV->hWndGB, wID, TRUE);
                    break;

                case IDM_GIZMOSNOTIFYOFF:
                    for (wID=GIZMOID_MIN; wID <= GIZMOID_MAX; wID++)
                        GBGizmoNotifySet(pAV->hWndGB, wID, FALSE);
                    break;

                case IDM_GIZMOSDATASET:
                    //Attach a name to the left align button.
                    GBGizmoDataSet(pAV->hWndGB, GIZMOID_BUTTON8
                        , (DWORD)(LPTSTR)TEXT("Wooley Llamma"));
                    break;

                case IDM_GIZMOSDATAGET:
                    psz=(LPTSTR)GBGizmoDataGet(pAV->hWndGB
                        , GIZMOID_BUTTON8);

                    if (NULL!=psz)
                        {
                        MessageBox(hWnd, psz, TEXT("Gizmo Data")
                            , MB_OK);
                        }
                    else
                        {
                        MessageBox(hWnd, TEXT("No Data Set")
                            , TEXT("Gizmo Data"), MB_OK);
                        }
                    break;

                case IDM_GIZMOSSETGETTEXT:
                    GBGizmoTextSet(pAV->hWndGB, GIZMOID_EDIT
                        , TEXT("Koala"));
                    GBGizmoTextGet(pAV->hWndGB, GIZMOID_EDIT, szTemp
                        , sizeof(szTemp));
                    MessageBox(hWnd, szTemp, TEXT("GBGizmoGetText")
                        ,MB_OK);
                    break;

                case IDM_GIZMOSSETGETINT:
                    GBGizmoIntSet(pAV->hWndGB, GIZMOID_EDIT, 1234
                        , FALSE);
                    dx=GBGizmoIntGet(pAV->hWndGB, GIZMOID_EDIT
                        , &fTrans, FALSE);
                    wsprintf(szTemp
                        , TEXT("Integer=%u, fTranslated=%d")
                        , dx, fTrans);
                    MessageBox(hWnd, szTemp, TEXT("GBGizmoGetInt")
                        , MB_OK);
                    break;

                }
            break;

        case WM_SIZE:
            dx=LOWORD(lParam);
            dy=HIWORD(lParam);

            //Change the children's widths to match.
            SetWindowPos(pAV->hWndGB, NULL, 0, 0, dx, pAV->tdd.cyBar
                , SWP_NOMOVE | SWP_NOZORDER);
            SetWindowPos(pAV->hWndC,  NULL, 0, pAV->tdd.cyBar, dx
                , dy-pAV->tdd.cyBar, SWP_NOMOVE | SWP_NOZORDER);
            break;


        default:
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));
        }

    return 0L;
    }




/*
 * GBClientWndProc
 *
 * Purpose:
 *  Window class procedure.  Standard callback.
 *
 * Parameters:
 *  Standard
 *
 * Return Value:
 *  Standard
 */

LRESULT APIENTRY GBClientWndProc(HWND hWnd, UINT iMsg, WPARAM wParam
    , LPARAM lParam)
    {
    TCHAR       szTemp[256];
    HDC         hDC;

    COMMANDPARAMS(wID, wCode, hWndMsg);

    switch (iMsg)
        {
        case WM_COMMAND:
            //Report on the commands from the gizmobar.
            wsprintf(szTemp
                , TEXT("WM_COMMAND: wID=%u, wParam=%u, lParam=%lu")
                , wID, wParam, lParam);

            hDC=GetDC(hWnd);

            SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
            SetBkColor(hDC, GetSysColor(COLOR_WINDOW));
            TextOut(hDC, 0, 0, szTemp, lstrlen(szTemp));

            ReleaseDC(hWnd, hDC);
            break;

        default:
            return (DefWindowProc(hWnd, iMsg, wParam, lParam));
        }

    return 0L;
    }
