#include "makeapp.h"

BOOL Frame_Initialize(APP* papp)
{
    if (!papp->hinstPrev)
    {
        WNDCLASS cls;

        cls.hCursor         = LoadCursor(NULL, IDC_ARROW);
        cls.hIcon           = LoadIcon(papp->hinst, MAKEINTRESOURCE(IDR_MAINICON));
        cls.lpszMenuName    = MAKEINTRESOURCE(IDR_MAINMENU);
        cls.hInstance       = papp->hinst;
        cls.lpszClassName   = "MakeApp_Frame";
        cls.hbrBackground   = (HBRUSH)(COLOR_WINDOW+1);
        cls.lpfnWndProc     = Frame_WndProc;
        cls.style           = CS_DBLCLKS;
        cls.cbWndExtra      = sizeof(FRAME*);
        cls.cbClsExtra      = 0;

        if (!RegisterClass(&cls))
            return FALSE;
    }
    return TRUE;
}

void Frame_Terminate(APP* papp, int codeTerm)
{
}

LRESULT CALLBACK _export Frame_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    FRAME* pfrm = Frame_GetPtr(hwnd);

    if (pfrm == NULL)
    {
        if (msg == WM_NCCREATE)
        {
            pfrm = (FRAME*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(FRAME));

            if (pfrm == NULL)
                return (LRESULT)FALSE;

            pfrm->hwnd = hwnd;
            Frame_SetPtr(hwnd, pfrm);
        }
        else
        {
            return Frame_DefProc(hwnd, msg, wParam, lParam);
        }
    }

    if (msg == WM_NCDESTROY)
    {
        //LRESULT result = HANDLE_MSG(pfrm, WM_NCDESTROY, Frame_OnNCDestroy);

        LocalFree((HLOCAL)pfrm);
        pfrm = NULL;
        Frame_SetPtr(hwnd, NULL);

        //return result;
    }

    if (msg == WM_MSGFILTER)
        HANDLE_WM_MSGFILTER(pfrm, wParam, lParam, Frame_OnMsgFilter);

    switch (msg)
    {
        HANDLE_MSG(pfrm, WM_CREATE, Frame_OnCreate);
        HANDLE_MSG(pfrm, WM_DESTROY, Frame_OnDestroy);

        HANDLE_MSG(pfrm, WM_CLOSE, Frame_OnClose);
        HANDLE_MSG(pfrm, WM_QUERYENDSESSION, Frame_OnQueryEndSession);
        HANDLE_MSG(pfrm, WM_ENDSESSION, Frame_OnEndSession);

        HANDLE_MSG(pfrm, WM_SIZE, Frame_OnSize);

        HANDLE_MSG(pfrm, WM_ACTIVATE, Frame_OnActivate);

        HANDLE_MSG(pfrm, WM_INITMENU, Frame_OnInitMenu);
        HANDLE_MSG(pfrm, WM_INITMENUPOPUP, Frame_OnInitMenuPopup);

        HANDLE_MSG(pfrm, WM_COMMAND, Frame_OnCommand);
    default:
        return Frame_DefProc(hwnd, msg, wParam, lParam);
    }
}

HWND Frame_CreateWindow(
        LPCSTR lpszText,
        int x,
        int y,
        int cx,
        int cy,
        HINSTANCE hinst)
{
    return CreateWindowEx(
            0L,
            "MakeApp_Frame",
            lpszText,
            WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            x, y, cx, cy,
            NULL,
            NULL,
            hinst,
            NULL);
}

BOOL Frame_OnCreate(FRAME* pfrm, CREATESTRUCT FAR* lpCreateStruct)
{
    pfrm->haccel = LoadAccelerators(lpCreateStruct->hInstance,
            MAKEINTRESOURCE(IDR_MAINACCEL));

    if (!pfrm->haccel)
        return FALSE;

    pfrm->hwndClient = Client_CreateWindow(pfrm->hwnd,
        0, 0, 0, 0,
        TRUE,
        RGB(0, 0, 255), "Hello World!");

    if (!pfrm->hwndClient)
        return FALSE;

    return TRUE;
}

void Frame_OnDestroy(FRAME* pfrm)
{
    pfrm->haccel = NULL;
    pfrm->hwndClient = NULL;
}

void Frame_OnClose(FRAME* pfrm)
{
    PostQuitMessage(0);
}

BOOL Frame_OnQueryEndSession(FRAME* pfrm)
{
    return FORWARD_WM_QUERYENDSESSION(pfrm->hwndClient, SendMessage);
}

void Frame_OnEndSession(FRAME* pfrm, BOOL fEnding)
{
    FORWARD_WM_ENDSESSION(pfrm->hwndClient, fEnding, SendMessage);

    // Call app termination handler after client handles it.
    //
    if (fEnding)
        App_Terminate(&g_app, TRUE);
}

void Frame_OnSize(FRAME* pfrm, UINT state, int cx, int cy)
{
    SetWindowPos(pfrm->hwndClient, NULL,
            0, 0, cx, cy,
            SWP_NOACTIVATE | SWP_NOZORDER);
}

void Frame_OnActivate(FRAME* pfrm, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
    switch (state)
    {
    case WA_INACTIVE:
	break;

    case WA_ACTIVE:
        SetFocus(pfrm->hwndClient);
        break;

    case WA_CLICKACTIVE:
	break;
    }
}

void Frame_OnInitMenu(FRAME* pfrm, HMENU hMenu)
{
}

void Frame_OnInitMenuPopup(FRAME* pfrm, HMENU hMenu, int item, BOOL fSystemMenu)
{
}

BOOL Frame_OnMsgFilter(FRAME* pfrm, MSG FAR* lpmsg, int context)
{
    // If we're in our main loop, a dialog is up, or we're in
    // menu mode, check for an accelerator press.  Otherwise,
    // don't do any filtering.
    //
    // Note that this allows accelerators to work while a menu
    // is pulled down.  We must explicitly roll the menu back up
    // with a WM_CANCELMODE message, however.
    //
    switch (context)
    {
    case MSGF_MAINLOOP:
    case MSGF_DIALOGBOX:
    case MSGF_MENU:
        if (!TranslateAccelerator(pfrm->hwnd, pfrm->haccel, lpmsg))
            return FALSE;

        // An accelerator was translated: if we're in menu mode,
        // be sure to roll up the menu before leaving...
        //
        if (context == MSGF_MENU)
        {
            MessageBeep(0);
            FORWARD_WM_CANCELMODE(GetActiveWindow(), SendMessage);
        }

        return TRUE;
        break;

    default:
        return FALSE;
    }
}

void Frame_OnCommand(FRAME* pfrm, int id, HWND hwndCtl, UINT code)
{
    switch (id)
    {
    case CMD_HELPABOUT:
        AboutDlg_Do(pfrm->hwnd);
        break;

    case CMD_FILEEXIT:
	PostQuitMessage(0);
	break;

    default:
        //
        // Pass commands not handled by frame on to the client
        //
        FORWARD_WM_COMMAND(pfrm->hwndClient, id, hwndCtl, code, SendMessage);
	break;
    }
}

// Simple Help About box
//
void AboutDlg_Do(HWND hwndOwner)
{
    DLGPROC lpfndp;

    lpfndp = (DLGPROC)MakeProcInstance((FARPROC)AboutDlg_DlgProc, g_app.hinst);

    if (!lpfndp)
        return;

    DialogBoxParam(g_app.hinst, MAKEINTRESOURCE(IDR_ABOUTDLG),
            hwndOwner, lpfndp, 0L);

    FreeProcInstance((FARPROC)lpfndp);
}

BOOL CALLBACK _export AboutDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        if (wParam == CTL_OK || wParam == CTL_CANCEL)
            EndDialog(hwndDlg, TRUE);
        return TRUE;
        break;

    case WM_INITDIALOG:
        return TRUE;
    }
    return FALSE;
}
