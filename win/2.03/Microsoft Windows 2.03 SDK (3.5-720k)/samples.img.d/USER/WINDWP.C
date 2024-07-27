
/*/
 *  windwp.c
 *  DefWindowProc
/*/

/*% MOVEABLE DS - SL */

#define LSTRING
#define NOCLIPBOARD
#define NOMENUS
#define NOCLIPBOARD
#define NOCTLMGR
#define NOSYSSTRUCTS
#include "user.h"
#include "winmgr.h"


#define SYS_ALTERNATE 0x2000
#define MENUSYSMENU ' '         /* space character */

extern SCRN scrn;
extern HWND hwndKeyCvt;
extern BOOL fMessageBox;
extern HWND hwndActive;
extern HWND hwndActivePrev;
extern HCURSOR hCursNormal;
extern HANDLE hCursIcon;
extern HANDLE hCursNormal;
extern HANDLE hCursSize;
extern BYTE rgbKeyState[];
extern HBRUSH hbrWhite;

extern SYSCLROBJECTS sysClrObjects;

BOOL far DefSetText(HWND, LPSTR);
void FAR EndScroll(HWND, BOOL);
void FAR FillWindow();
int FAR InWnd();
HWND GetChildParent();
HBRUSH far GetBackBrush(HWND);
HWND GetEnabledPopup(HWND);


extern DWORD far DefMsgProc(HWND, int, WORD, long, DWORD far *);
extern void far DrawCaption(HWND, HDC, BOOL, BOOL);

int fMenuStatus = FALSE;
static BOOL fF10Status  = FALSE;

/*******************************************************************/

long far DefWindowProc(hwnd, message, wParam, lParam)
register HWND hwnd;
unsigned message;
register WORD wParam;
LONG lParam;
{
    HDC hdc;
    int cch;
    PAINTSTRUCT ps;
    HICON hIcon;
    RECT rc;
    HANDLE hCurs;
    HBRUSH hbr;
    HWND hwndT;
    int code;
    int sysMsg;
    DWORD ret;

    extern BOOL fMenu;
    extern CARET caret;
    extern SYSCOLORS sysColors;
    extern SYSCLROBJECTS sysClrObjects;
    extern int FAR VKConvert();
    extern DWORD FAR CallWindowMgrHook(int, WORD, DWORD);
    extern HWND hwndCapture;
    extern HWND hwndFocus;
    extern HWND hwndIconTitle;
    extern HWND hwndDesktop;
    extern int (*pfnSB)();
    void far HideCaret2();
    void far ShowCaret2();
    int * far InitPwSB(HWND);

    if (!CheckHwnd(hwnd))
        return((DWORD)FALSE);

#ifdef WINMGRHOOK
    /* First ask the window manager if it wants the message.
     * Q: Is this too much overhead?
     */
    msg.hwnd = hwnd;
    msg.message = message;
    msg.wParam = wParam;
    msg.lParam = lParam;
    if (CallWindowMgrHook(WC_DEFWINDOWPROC, NULL, (DWORD)(LPMSG)&msg) != 0)
        return(msg.lParam);
#else
    if (DefMsgProc(hwnd, message, wParam, lParam, (DWORD far *)&ret)) {
        return(ret);
    }
#endif

    switch (message) {
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
        WinInput(hwnd, message, wParam, lParam);
        break;

    case WM_CANCELMODE:
        if (hwndCapture == hwnd && pfnSB != NULL)
            EndScroll(hwnd, TRUE);
        /* if the capture is still set, just release at this point.
         * Can put other End* functions in later.
         */
        if (hwnd == hwndCapture)
            ReleaseCapture();
        break;

    case WM_NCCREATE:
        if (TestWF(hwnd, (WFHSCROLL | WFVSCROLL))) {
            if (InitPwSB(hwnd) == NULL)
                return((long)FALSE);
        }
        return((long)DefSetText(hwnd, ((LPCREATESTRUCT)lParam)->lpszName));
        break;

    case WM_NCDESTROY:
        if (hwnd->szName)
            FreeP((HANDLE)(hwnd->szName));
        if (hwnd->rgwScroll)
            FreeP((HANDLE)(hwnd->rgwScroll));
        break;

    case WM_NCPAINT:
        SetWF(hwnd, WFMENUDRAW);       /* force drawing of menu    */
        DrawWindowFrame(hwnd, (HRGN)wParam);
        ClrWF(hwnd, WFMENUDRAW);
        break;

    case WM_SETTEXT:
        DefSetText(hwnd, (LPSTR)lParam);
        if ((TestWF(hwnd, WFBORDERMASK) == (BYTE)LOBYTE(WFCAPTION)) && IsWindowVisible(hwnd)) {
            hdc = (HDC)GetWindowDC(hwnd);
#ifdef WINMGRHOOK
            CallWindowMgrHook(WC_DRAWCAPTION, (WORD)hwnd, MAKELONG((WORD)hdc, FALSE));
#else
            DrawCaption(hwnd, hdc, FALSE, TestWF(hwnd, WFFRAMEON));
#endif
            ReleaseADc(hdc);
        }
        break;


    case WM_GETTEXT:
        cch = 0;
        if (hwnd->szName && wParam != 0) {
            cch = min(lstrlen((LPSTR)hwnd->szName), wParam - 1);
            LCopyStruct((LPSTR)hwnd->szName, (LPSTR)lParam, cch);
        }
        ((LPSTR)lParam)[cch] = 0;
        return((long)cch);
        break;

    case WM_GETTEXTLENGTH:
        if (hwnd->szName)
            return(lstrlen((LPSTR)hwnd->szName));
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_PAINT:
        BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
        EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
        break;

    case WM_PAINTICON:
        {
            HDC hdc;
            BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
            /* wParam == TRUE to draw icon, FALSE to ignore paint */
            if (wParam) {
                /* draw icon through window dc since in 1.03 we never
                   used own dc. If own dc is used we the mapping
                   modes may not be MM_TEXT and then we are scrwed
                   (raor 06/10)
                */


                hdc = GetWindowDC(hwnd);
                hIcon = (HICON)(PCLS)(hwnd->pcls)->hIcon;
                GetClientRect(hwnd, (LPRECT)&rc);

#if     0
                if ((hbr = GetBackBrush(hwnd)) == NULL)
                    hbr = sysClrObjects.hbrDesktop;
                FillRect(ps.hdc, (LPRECT)&rc, hbr);
#endif
                rc.left = (rc.right - GetSystemMetrics(SM_CXICON)) >> 1;
                rc.top = (rc.bottom - GetSystemMetrics(SM_CYICON)) >> 1;
                DrawIcon(hdc, rc.left, rc.top, hIcon);
                ReleaseDC(hwnd, hdc);
            }
            /* delete the update region             */
            EndPaint1(hwnd, (LPPAINTSTRUCT)&ps, TRUE);
        }
	break;

    case WM_ICONERASEBKGND:
#if 0
	if (lParam) {
	    /* active icon has white background */
	    hbr = hbrWhite;
	} else {
	    /* non-active icon has screen colored background */
	    hbr = sysClrObjects.hbrDesktop;
	}
	hbr = sysClrObjects.hbrDesktop;
	GetClientRect(hwnd, (LPRECT)&rc);
	FillRect((HDC)wParam, (LPRECT)&rc, hbr);
	return((long)TRUE);
	break;
#endif
        {
            HDC hdc;

            hdc = GetWindowDC(hwnd);
            hbr = sysClrObjects.hbrDesktop;
            GetClientRect(hwnd, (LPRECT)&rc);
            FillRect(hdc, (LPRECT)&rc, hbr);
            ReleaseDC(hwnd, hdc);
            return((long)TRUE);
        }

    case WM_SYNCPAINT:
        /*
         * wParam         = flags
         * LOWORD(lParam) = hrgnClip
         * HIWORD(lPAram) = hwndSkip
         */
        DoSyncPaint(hwnd, wParam, LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_ERASEBKGND:

        if ((hbr = GetBackBrush(hwnd)) != NULL) {
            FillWindow(hwnd, hwnd, (HDC)wParam, hbr);
            return((long)TRUE);
        }
        break;

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
        return((long)TRUE);
        break;

    case WM_SYSCOMMAND:
        SysCommand(hwnd, wParam, lParam);
        break;

    case  WM_KEYDOWN:

        if (wParam == VK_F10 && (!(GetAppVer() != VER)))
            fF10Status = TRUE;
        break;

    case  WM_SYSKEYDOWN:
        /* Special case stuff */
        /* if syskey is down */
        if (HIWORD(lParam) & SYS_ALTERNATE) {
            /* don't have to lock hwndActive because it's processing this key */
            if (wParam == VK_MENU) {
                if (!fMenuStatus)
                    fMenuStatus++;
            }else
                fMenuStatus = 0;
            fF10Status = FALSE;

            switch (wParam) {

/********* This was removed on 3/17/87 by bobgu ********
 *	    case VK_RETURN:
 *		SendMessage(hwndActive, WM_SYSCOMMAND,
 *			((rgbKeyState[VK_SHIFT] & 0x80) ? SC_ICON : SC_ZOOM), 0L);
 *		break;
 */

            case VK_TAB:
            case VK_ESCAPE:
            case VK_F6:
                SendMessage(hwndActive, WM_SYSCOMMAND,
                  (GetKeyState(VK_SHIFT) < 0 ? SC_NEXTWINDOW :
                  SC_PREVWINDOW), (long)wParam);
                break;

	    case VK_F4:
		if (!TestCF(hwndActive, CFNOCLOSE)) {
		    sysMsg = SC_CLOSE;
		    goto sendsys;
		}
		break;

	    case VK_F5:
		if (TestwndPopup(hwndActive) && TestwndNIPopup(hwndActive))
		    break;
		if (TestWF(hwndActive, WFMINIMIZED) || TestWF(hwndActive, WFMAXIMIZED)) {
		    sysMsg = SC_RESTORE;
		    goto sendsys;
		}
		break;

	    case VK_F7:
		/* no move if maximized (bobgu 6/3/87) */
		if (TestWF(hwndActive, WFMAXIMIZED))
		    break;
		sysMsg = SC_MOVE;
		goto sendsys;

	    case VK_F8:
		/* no size if maximized (bobgu 6/3/87) */
		if (TestWF(hwndActive, WFMAXIMIZED))
		    break;
		if (TestWF(hwndActive, WFSIZEBOX) && !TestWF(hwndActive, WFMINIMIZED)) {
		    sysMsg = SC_SIZE;
		    goto sendsys;
		}
		break;

	    case VK_F9:
		if (TestwndPopup(hwndActive) && TestwndNIPopup(hwndActive))
		    break;
                if (TestWF(hwndActive, WFMINBOX)) {
                    if (TestWF(hwndActive, WFMINIMIZED))
                        sysMsg = SC_RESTORE;
                    else
                        sysMsg = SC_MINIMIZE;
		    goto sendsys;
		}
		break;

	    case VK_F10:
		if (TestwndPopup(hwndActive) && TestwndNIPopup(hwndActive))
		    break;
                if (TestWF(hwndActive, WFMAXBOX)) {
                    if (TestWF(hwndActive, WFMAXIMIZED))
                        sysMsg = SC_RESTORE;
                    else
		    sysMsg = SC_MAXIMIZE;
		    goto sendsys;
		}
		break;

sendsys:
                /* do not change focus if the child window has focus    */

                if (hwndFocus == NULL || GetTopLevelWindow(hwndFocus) != hwndActive)
                    SetFocus(hwndActive);
		SendMessage(hwndActive, WM_SYSCOMMAND, sysMsg, 0L);
		break;

	    }
        } else {
            if (wParam == VK_F10) {
                fF10Status = TRUE;
            } else {
                if (wParam == VK_ESCAPE && (GetKeyState(VK_SHIFT)))
                    SendMessage(hwnd, WM_SYSCOMMAND, SC_KEYMENU, (DWORD)MENUSYSMENU);
            }
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        if ((wParam == VK_MENU && (fMenuStatus == 1)) || (wParam == VK_F10 && fF10Status) ){
            SendMessage(hwnd, WM_SYSCOMMAND, SC_KEYMENU, (DWORD)0);
        }
        fF10Status = fMenuStatus = FALSE;

        break;

    case WM_SYSCHAR:
        /* If syskey is down and we have a char */
        fMenuStatus = 0;
        if ((HIWORD(lParam) & SYS_ALTERNATE) && wParam) {
	    /* Don't look at these... */
#if 0
	    if (wParam == VK_TAB || wParam == VK_ESCAPE || wParam == VK_F6)
		break;
#endif
            if (wParam == VK_TAB || wParam == VK_ESCAPE)
		break;
	    SendMessage(hwnd, WM_SYSCOMMAND, SC_KEYMENU, (DWORD)wParam);
	} else {
	    MessageBeep(0);
	}
        break;

    case WM_ACTIVATE:
#ifdef DISABLE
        if (wParam && !HIWORD(lParam)) {
#endif
        if (wParam) {
            SetFocus(hwnd);
        }
        break;

    case WM_SETREDRAW:
        SetRedraw(hwnd, wParam);
        break;

    case WM_SHOWWINDOW:
        /* Non null show window descriptor = implied popup hide or show */
        if (LOWORD(lParam) != 0 && TestwndPopup(hwnd)) {
            /* if NOT(showing, invisible, and not set as hidden) AND
            /* NOT(hiding and not visible) */
            if (!(wParam != 0 && !TestWF(hwnd, WFVISIBLE) && !TestWF(hwnd, WFHIDDENPOPUP)) &&
                    !(wParam == 0 && !TestWF(hwnd, WFVISIBLE))) {
                if (wParam) {
                    /* if showing, clear the hidden popup flag */
                    ClrWF(hwnd, WFHIDDENPOPUP);
                } else {
                    /* if hiding, set the hidden popup flag */
                    SetWF(hwnd, WFHIDDENPOPUP);
                }
                ShowWindow(hwnd, (wParam ? SHOW_OPENNOACTIVATE : HIDE_WINDOW));
            }
        }
        break;

    case WM_CTLCOLOR:
        if (HIWORD(lParam) != CTLCOLOR_SCROLLBAR) {
            SetBkColor((HDC)wParam, sysColors.clrWindow);
            SetTextColor((HDC)wParam, sysColors.clrWindowText);
            hbr = sysClrObjects.hbrWindow;
        } else {
            SetBkColor((HDC)wParam, sysColors.clrCaptionText);
            SetTextColor((HDC)wParam, sysColors.clrCaption);
            hbr = sysClrObjects.hbrScrollbar;
            UnrealizeObject(hbr);
        }
#if 0
    /* only unrealize scrollbar brush.  window background brush is solid. */
        /* This brush should not be selected anywhere at this moment */
        UnrealizeObject(hbr);
#endif
        SemRectReadEnter();
#if 0
        /* this set org is causing hell of lot problems  (raor)  */

        SetBrushOrg((HDC)wParam, hwnd->rcClient.left, hwnd->rcClient.top);
#endif
        SemRectReadLeave();
        return((DWORD)hbr);
        break;


    case WM_SETCURSOR:
        /* wParam  == hwnd that cursor is over
           lParamL == Hit test area code (result of WM_NCHITTEST)
           lParamH == Mouse message number
        */
        if ((hwndT = GetChildParent(hwnd)) != NULL &&
                (BOOL)SendMessage(hwndT, WM_SETCURSOR, wParam, lParam))
            return((LONG)TRUE);

        if (HIWORD(lParam) == 0) {
            switch(LOWORD(lParam)) {
#if 0
/* we don't use these two cursors anymore. 14-Jul-1987. davidhab. */
            case MSGF_MOVE:
                hCurs = hCursIcon;
                break;

            case MSGF_SIZE:
                hCurs = hCursSize;
                break;
#endif
            default:
                hCurs = hCursNormal;
                break;
            }
            SetCursor(hCurs);
        } else {
            switch (LOWORD(lParam)) {
            case HTCLIENT:
                if ( ((HWND)wParam)->pcls->hCursor != NULL)
                    SetCursor( ((HWND)wParam)->pcls->hCursor);
                break;
            case HTERROR:
                switch (HIWORD(lParam)) {
                case WM_LBUTTONDOWN:
		    if ((hwndT = GetEnabledPopup(hwnd)) != NULL) {
			if (hwndT != hwndDesktop->hwndChild) {
			    SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                            SetActiveWindow(hwndT);

#if     0
			    SetWindowPos(hwndT, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE);
#endif
			    break;
			}
		    }
		    /* drop through */
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                    MessageBeep(MB_ICONHAND);
                    break;
                default:
                    break;
                }
                /* Fall thru */
            default:
                SetCursor(hCursNormal);
                break;
            }
        }
        return((long)FALSE);
        break;

    case WM_MOUSEACTIVATE:
        if ((hwndT = GetChildParent(hwnd)) != NULL &&
                (code = (int)SendMessage(hwndT, WM_MOUSEACTIVATE, wParam, lParam)) != 0)
            return((long)code);

        switch (LOWORD(lParam)) {
        case HTCAPTION:
        case HTSIZE:
#if 0
/* 09-Jun-1987. Activate anyways to be consistent with sysmenu & max icons */
        case HTREDUCE:
        case HTLEFT:
        case HTRIGHT:
        case HTTOP:
        case HTTOPLEFT:
        case HTTOPRIGHT:
        case HTBOTTOM:
        case HTBOTTOMLEFT:
        case HTBOTTOMRIGHT:
#endif

            /* For moving, sizing and minimizing, don't activate until AFTER
               we take action */
            return((long)MA_NOACTIVATE);
            break;
        }

        return((long)MA_ACTIVATE);
        break;


    case WM_DOQUIT:
        PostQuitMessage(0);
        break;

    case WM_DOSETWINDOWPOS:
        SetWindowPos( ((LPSWP)lParam)->hwnd,
                      ((LPSWP)lParam)->hwndInsertAfter,
                      ((LPSWP)lParam)->x,
                      ((LPSWP)lParam)->y,
                      ((LPSWP)lParam)->cx,
                      ((LPSWP)lParam)->cy,
                      ((LPSWP)lParam)->flags );
        break;

    case WM_DOMINIMIZE:
    case WM_DOMAXIMIZE:
        /* Go through SysCommand... the code is already there */
        SysCommand(hwnd, (message == WM_DOMINIMIZE ? SC_MINIMIZE : SC_MAXIMIZE), 0L);
        break;

    case WM_SYNCTASK:
        if (wParam == ST_BEGINSWP)
            SuspendTask();
        else
            ReleaseTask();
        /* fall thru to break */

    case WM_DOSUSPEND:
        break;
    }
    return(0L);
}

HBRUSH far GetBackBrush(hwnd)
HWND hwnd;
{
    register HBRUSH hbr, hbrt;

    /* Fill the LVB with the background color */
    if ((hbrt = hbr = hwnd->pcls->hbrBackground) != NULL) {
        if ((unsigned)hbr <= COLOR_APPWORKSPACE + 1)
            hbr = *(((HBRUSH *)&sysClrObjects) + ((unsigned)hbr) - 1);
        /* dont waste time unrealizing if solid windowbackground brush */
        if (hbrt != (HBRUSH)(COLOR_BACKGROUND+1))
            UnrealizeObject(hbr);
    }
    return(hbr);
}

BOOL far DefSetText(hwnd, lpsz)
register HWND hwnd;
LPSTR lpsz;
{
    char *sz;
    register int cch;

    if (hwnd->szName != NULL)
        hwnd->szName = FreeP(hwnd->szName);
    if (lpsz != NULL) {
        cch = lstrlen((LPSTR)lpsz) + 1;
        /* Map HIWORD of lpsz to DS handle (in case DS moves) */
        SwapHandle((WORD *)&lpsz + 1);
        if ((sz = (char *)AllocP(cch)) == NULL) {
            /* if hardsysmodal msgbox, and alloc fails, just have no
             * caption. 14-Oct-1987. davidhab.
             */
            if (fMessageBox) {
                hwnd->szName = NULL;
                return(TRUE);
            } else {
                return(FALSE);      /* out of memory */
            }
        }
        hwnd->szName = sz;
        /* Map HIWORD of lpsz from DS Handle to DS pointer */
        SwapHandle((WORD *)&lpsz + 1);
        LCopyStruct(lpsz, (LPSTR)(hwnd->szName), cch);
    }
    return(TRUE);
}



HWND GetEnabledPopup(hwndStart)
register HWND hwndStart;
{

    HWND    hwndT;

    extern HWND hwndDesktop, hwndSysModal;

    HQ hqStart = hwndStart->hq;
    register HWND hwnd = hwndStart->hwndNext;

    if (hwndSysModal)
	return(NULL);

    while (hwnd != hwndStart) {
	if (hwnd == NULL) {
	    hwnd = hwndDesktop->hwndChild;
	    continue;
	}
	if (hwnd->hq ==  hqStart) {
	    if (!TestWF(hwnd, WFDISABLED) && TestWF(hwnd, WFVISIBLE)) {
                hwndT = hwnd->hwndOwner;

                /* bring up only one if this window is parent of one
                   the popup window
                */
                while(hwndT) {
                    if (hwndT == hwndStart)
                        return(hwnd);

                    hwndT = hwndT->hwndOwner;
                }

                return((HWND)0);
	    }
	}
	hwnd = hwnd->hwndNext;
    }
    return((HWND)0);
}
