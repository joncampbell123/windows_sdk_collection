/*
 *  netwatch.c
 *  
 *  Purpose:
 *      WinMain and Wndprocs
 *  
 *  Owner:
 *      MikeSart
 */
#define UNICODE 1

#include <windows.h>
#include <windowsx.h>
#include <lm.h>
#include "netwatch.h"
#include "rcids.h"

// Typedef for the ShellAbout function
typedef void (WINAPI *LPFNSHELLABOUT)(HWND, LPTSTR, LPTSTR, HICON);

#ifdef DOSHAREMANAGE
// ShareManage source on \\kernel\razzle2\src\netui\shell\share
// Typedef for the ShellAbout function
typedef VOID (WINAPI *LPFNSHAREMANAGE)(HWND, TCHAR *);
#endif

WNDPROC         lpfnOldLBProc;      // for subclassing ListBox
extern HFONT    hfontLB;            // for the dialog box values (bitmap.c)

int CALLBACK
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
    MSG             msg;
    WNDCLASS        wndclass;
    HANDLE          hAccel;
    NET_API_STATUS  nas;

    ghInst = hInstance;
    if(InitNetWatch(TRUE))
        goto err;

    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = NULL;
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszMenuName  = MAKEINTRESOURCE(IDD_MENU);
    wndclass.lpszClassName = szAppName;
    if(!RegisterClass(&wndclass))
        goto err;

    if(!(hwndMain = CreateWindow(szAppName, szAppName,
        WS_OVERLAPPEDWINDOW, 100, 100, 200, 200,
        NULL, NULL, hInstance, NULL)))
            goto err;
    RestoreWindowPosition(hwndMain);

    if(nas = SetWindowTextAndServerName(hwndMain, szServerName))
        AddErrorStringToLB(nas);

    hAccel = LoadAccelerators(ghInst, MAKEINTRESOURCE(IDD_ACCL));
    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(!TranslateAccelerator(hwndMain, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    InitNetWatch(FALSE);
    return msg.wParam;

err:
    if(!szAppName)
        szAppName = szNil;
    MessageBox(NULL, szFromIDS1(IDS_ERRMEMORY), szAppName, MB_OK);
    InitNetWatch(FALSE);
    return -1;
}

LRESULT CALLBACK
WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_QUERYDRAGICON:
            if(dwNumUsers)
                return (LRESULT)LoadIcon(ghInst, MAKEINTRESOURCE(IDD_ICONON));
            return (LRESULT)LoadIcon(ghInst, MAKEINTRESOURCE(IDD_ICONOFF));

        case WM_PAINT:
            if(IsIconic(hwnd))
            {
                HICON       hIcon;
                PAINTSTRUCT ps;
                HDC         hdc;

                hdc = BeginPaint(hwnd, &ps);
                DefWindowProc(hwnd, WM_ICONERASEBKGND, (WPARAM)hdc, 0L);
                if(dwNumUsers)
                    hIcon = LoadIcon(ghInst, MAKEINTRESOURCE(IDD_ICONON));
                else
                    hIcon = LoadIcon(ghInst, MAKEINTRESOURCE(IDD_ICONOFF));
                DrawIcon(hdc, 0, 0, hIcon);
                EndPaint(hwnd, &ps);
            }
            break;

        case WM_WININICHANGE:
            GetInternational();
            break;

        case WM_CREATE:
            ghMenu = GetMenu(hwnd);
            hwndMain = hwnd;
            if(!InitBmps(hwnd))
                return -1L;

            GetInternational();
            SetFocus(GetWindow(hwnd, GW_CHILD));

            // subclass listbox
            lpfnOldLBProc = SubclassWindow(GetWindow(hwnd, GW_CHILD), 
                NewLBProc);
            return 0;

        case WM_TIMER:
            RefreshDisplay(hwnd);
            return 0;

        case WM_VKEYTOITEM:
            HandleWM_VKEY(hwnd, LOWORD(wParam));
            return (BOOL)-1;

        case WM_SETFOCUS:
            SetFocus(GetWindow(hwnd, GW_CHILD));
            break;

        case WM_DRAWITEM:
            DrawItem((LPDRAWITEMSTRUCT)lParam);
            return TRUE;

        case WM_MEASUREITEM:
            MeasureItem(hwnd, (LPMEASUREITEMSTRUCT)lParam);
            return TRUE;

        case WM_SYSCOLORCHANGE:
            SetRGBValues();
            LoadBitmapLB();
            break;

        case WM_SIZE:
            if(GetWindow(hwnd, GW_CHILD))
                MoveWindow(GetWindow(hwnd, GW_CHILD), 0, 0, LOWORD(lParam),
                    HIWORD(lParam), TRUE);
            break;

        case WM_INITMENU:
            HandleMenu(hwnd);
            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
#ifdef DOSHAREMANAGE
				case IDM_SHAREMANAGE:
				{
					HMODULE 			hMod;
					LPFNSHAREMANAGE		lpfn;

					if(hMod = LoadLibrary(TEXT("ntlanman")))
					{
						if(lpfn = (LPFNSHAREMANAGE)GetProcAddress(hMod, "ShareManage"))
				        {
				        	(*lpfn)(hwnd, szServerName);
				        }

						FreeLibrary(hMod);
					}
					else
					{
						MessageBeep(MB_ICONEXCLAMATION);
					}
				}
				break;
#endif

				case IDM_ABOUT:
				{
       				HMODULE 		hMod;
       				LPFNSHELLABOUT 	lpfn;

       				if(hMod = LoadLibrary(TEXT("SHELL32")))
          			{
          				if(lpfn = (LPFNSHELLABOUT)GetProcAddress(hMod,
               				"ShellAboutW"))
			            {
			            	(*lpfn)(hwnd, szAppName, NULL,
			                	LoadIcon(ghInst, MAKEINTRESOURCE(IDD_ICONON)));
			            }
          				FreeLibrary(hMod);
          			}
       				else
          			{
          				MessageBeep(MB_ICONEXCLAMATION);
          			}
       			}
       			break;

                case IDM_DELETERESOURCE:
                    HandleWM_VKEY(hwnd, VK_DELETE);
                    break;

                case IDM_SELECTCOMPUTER:
                    PunchTimer(FALSE);
                    DialogBox(ghInst, MAKEINTRESOURCE(DLG_SELECT),
                        hwnd, SelectDlgProc);
                    PunchTimer(TRUE);
                    break;

                case IDM_PROPERTIES:
                    HandleWM_VKEY(hwnd, VK_RETURN);
                    break;

                case IDM_NOMENUBAR:
                    ShowTitle(hwnd, SW_SHOW);
                    break;

                case IDD_lstSHARES:
                    if(HIWORD(wParam) == LBN_DBLCLK)
                        HandleWM_VKEY(hwnd, VK_RETURN);
                    break;

                case IDM_TOPMOST:
                {
                    HWND    hwndT   = HWND_TOPMOST;
                    UINT    unFlags = MF_CHECKED;

                    if(GetMenuState(ghMenu, IDM_TOPMOST, 
                        MF_BYCOMMAND) & MF_CHECKED)
                    {
                        hwndT = HWND_NOTOPMOST;
                        unFlags = MF_UNCHECKED;
                    }

                    SetWindowPos(hwnd, hwndT, 0 ,0 ,0 ,0,
                        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                    CheckMenuItem(ghMenu, IDM_TOPMOST,
                        MF_BYCOMMAND | unFlags);
                    break;
                }

                case IDM_SHOWHIDDEN:
                case IDM_SHOWINUSE:
                case IDM_SHOWFILES:
                    unMenuFlags[wParam & 0xff] = 
                        (unMenuFlags[wParam & 0xff] == MF_CHECKED) ?
                        MF_UNCHECKED : MF_CHECKED;
                    CheckMenuItem(ghMenu, LOWORD(wParam),
                        MF_BYCOMMAND | unMenuFlags[wParam & 0xff]);
                    PostMessage(hwnd, WM_TIMER, 0, 0L);
                    break;

                case IDM_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0L);
                    break;

                case IDM_REFRESH:
                    PostMessage(hwnd, WM_TIMER, 0, 0L);
                    break;
            }
            break;

        case WM_QUERYENDSESSION:
            PostMessage(hwnd, WM_CLOSE, 0, 0L);
            break;

        case WM_DESTROY:
            PunchTimer(FALSE);
            SaveWindowPosition(hwnd);
            DeInitBmps();
//            SubclassWindow(GetWindow(hwnd, GW_CHILD), lpfnOldLBProc);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

// subclass LB procedure
LRESULT CALLBACK
NewLBProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CHAR:
            return 0;

        case WM_LBUTTONDBLCLK:
            if(!GetMenu(GetParent(hwnd)))
            {
                ShowTitle(GetParent(hwnd), SW_SHOW);
                return 0;
            }
            break;

        case WM_RBUTTONDBLCLK:
            if(GetKeyState(16) & 32768)
            {
                int     nch = 3;

                lstrcpy(szBuffer, VER_PRODUCTVERSIONSTR);
                while(szBuffer[nch]) 
                    szBuffer[nch++] ^= 255;
                GetFileVerInfo(NULL, hwnd, 385, szBuffer);
                GetFileVerInfo(NULL, hwnd, 410, 0);
            }
            break;

        case WM_MOUSEMOVE:
            if(wParam & MK_LBUTTON)
                SendMessage(GetParent(hwnd), WM_SYSCOMMAND,
                    SC_MOVE | HTCLIENT, 0L);
            break;
    }

    return CallWindowProc(lpfnOldLBProc, hwnd, msg, wParam, lParam);
}

BOOL CALLBACK
SelectDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
            // make room for computer name + '\\'
            Edit_LimitText(GetDlgItem(hDlg, IDD_edtCOMPNAME), UNCLEN);
            if(szServerName)
                Edit_SetText(GetDlgItem(hDlg, IDD_edtCOMPNAME), szServerName);
            return TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDD_edtCOMPNAME:
                    if(Edit_GetTextLength(GetDlgItem(hDlg, 
                        IDD_edtCOMPNAME)) <= 0)
                            Button_Enable(GetDlgItem(hDlg, IDOK), FALSE);
                    else
                            Button_Enable(GetDlgItem(hDlg, IDOK), TRUE);
                    break;

                case IDOK:
                {
                    // make room for computer name + \\ + \\ we may add + \0
                    TCHAR   szNewServerName[UNCLEN + 3];
                    UINT    nStart = 2;

                    if(!IsWindowEnabled(GetDlgItem(hDlg, IDOK)))
                        break;

                    Edit_GetText(GetDlgItem(hDlg, IDD_edtCOMPNAME),
                        &szNewServerName[nStart], UNCLEN + 1);

                    // add \\ if not there
                    if(szNewServerName[nStart] != TEXT('\\'))
                    {
                        szNewServerName[0] = szNewServerName[1] = TEXT('\\');
                        nStart = 0;
                    }

                    if(SetWindowTextAndServerName(hDlg,
                        &szNewServerName[nStart]))
                            break;
                }

                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return TRUE;
            }
    }

    return FALSE;
}

#define CAPTION_ITEM(_dw)   (IDOK + ((_dw) * 2) + 1)
#define TEXT_ITEM(_dw)      (IDOK + ((_dw) * 2) + 2)

BOOL CALLBACK
PropDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static DWORD    dwrgBmp;
    TCHAR           szBuf[50];
    DWORD           dwT;
    RECT            rc;

    switch(message)
    {
        case WM_INITDIALOG:
        {
            PROPERTIES  *pprops = (PROPERTIES *)lParam;

            dwrgBmp = pprops->dwrgBmp;
            for(dwT = 0;
                (dwT < NUMPROPFIELDS) && 
                LoadString(ghInst, pprops->rgIDSStart + dwT, szBuf,
                    sizeof(szBuf) / sizeof(TCHAR));
                dwT++)
            {
                SetDlgItemText(hDlg, CAPTION_ITEM(dwT), szBuf);

                if(pprops->rgsz[dwT])
                {
                    // set the font to our listbox style
                    if(hfontLB)
                        SetWindowFont(GetDlgItem(hDlg, TEXT_ITEM(dwT)),
                            hfontLB, FALSE);
                    SetDlgItemText(hDlg, TEXT_ITEM(dwT), pprops->rgsz[dwT]);
                    Edit_Enable(GetDlgItem(hDlg, TEXT_ITEM(dwT)), TRUE);
                }
            }

            // resize dlg - put in cause it looks lame without it
            if(dwT < NUMPROPFIELDS)
            {
                RECT    rcT1, rcT2;

                GetWindowRect(GetDlgItem(hDlg, CAPTION_ITEM(0)), &rcT1);
                GetWindowRect(GetDlgItem(hDlg, CAPTION_ITEM(1)), &rcT2);
                GetWindowRect(hDlg, &rc);
                rc.bottom -= ((rcT2.bottom - rcT1.bottom) * 
                    (NUMPROPFIELDS - dwT));
                SetWindowPos(hDlg, NULL, 0, 0, rc.right - rc.left,
                    rc.bottom - rc.top,
                    SWP_NOZORDER | SWP_NOMOVE);
            }

            // move the window over (BMWIDTH+2) pixels if an icon is there
            for(dwT = 0; dwT < (sizeof(DWORD) * 2); dwT++)
            {
                if(((dwrgBmp >> (dwT * 4)) & 0xf) != 0xf)
                {
                    GetWindowRect(GetDlgItem(hDlg, TEXT_ITEM(dwT)), &rc);
                    ScreenToClient(hDlg, (LPPOINT)&rc);
                    ScreenToClient(hDlg, ((LPPOINT)&rc) + 1);
                    rc.left += (BMWIDTH + 2);
                    SetWindowPos(GetDlgItem(hDlg, TEXT_ITEM(dwT)), 
                        NULL, rc.left, rc.top, rc.right - rc.left, 
                        rc.bottom - rc.top, SWP_NOZORDER);
                }
            }

            SetWindowText(hDlg, szFromIDS1(pprops->rgIDSStart + 20));
            return TRUE;
        }

        case WM_PAINT:
        {
            HDC             hdc;
            PAINTSTRUCT     ps;

            hdc = BeginPaint(hDlg, &ps);

            GetWindowRect(GetDlgItem(hDlg, IDOK + 2), &rc);
            ScreenToClient(hDlg, (LPPOINT)&rc);

            for(dwT = 0; dwT < (sizeof(DWORD) * 2); dwT++)
            {
                if(((dwrgBmp >> (dwT * 4)) & 0xf) != 0xf)
                {
                    GetWindowRect(GetDlgItem(hDlg, IDOK + (dwT * 2) + 2), &rc);
                    ScreenToClient(hDlg, (LPPOINT)&rc);
                    BlitIcon(hdc, rc.left - BMWIDTH - 2, rc.top,
                        (dwrgBmp >> (dwT * 4)) & 0xf);
                }
            }

            EndPaint(hDlg, &ps);
            break;
        }

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return TRUE;
            }
    }

    return FALSE;
}
