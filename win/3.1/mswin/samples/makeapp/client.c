#include "makeapp.h"

BOOL Client_Initialize(APP* papp)
{
    if (!papp->hinstPrev)
    {
        WNDCLASS cls;

        cls.hCursor         = LoadCursor(NULL, IDC_ARROW);
        cls.hIcon           = NULL;
        cls.lpszMenuName    = NULL;
        cls.hInstance       = papp->hinst;
        cls.lpszClassName   = "MakeApp_Client";
        cls.hbrBackground   = (HBRUSH)(COLOR_WINDOW+1);
        cls.lpfnWndProc     = Client_WndProc;
        cls.style           = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        cls.cbWndExtra      = sizeof(CLIENT*);
        cls.cbClsExtra      = 0;

        if (!RegisterClass(&cls))
            return FALSE;
    }

    return TRUE;
}

void Client_Terminate(APP* papp, int codeTerm)
{
}

LRESULT CALLBACK _export Client_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    CLIENT* pcli = Client_GetPtr(hwnd);

    if (pcli == NULL)
    {
        if (msg == WM_NCCREATE)
        {
            pcli = (CLIENT*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(CLIENT));

            if (pcli == NULL)
                return 0L;

            pcli->hwnd = hwnd;
            Client_SetPtr(hwnd, pcli);
        }
        else
        {
            return Client_DefProc(hwnd, msg, wParam, lParam);
        }
    }

    if (msg == WM_NCDESTROY)
    {
        //DWORD result = HANDLE_MSG(hwnd, WM_NCDESTROY, Client_OnNCDestroy);

        LocalFree((HLOCAL)pcli);
        pcli = NULL;
        Client_SetPtr(hwnd, NULL);

        //return result;
    }

    switch (msg)
    {
        HANDLE_MSG(pcli, WM_CREATE, Client_OnCreate);
        HANDLE_MSG(pcli, WM_DESTROY, Client_OnDestroy);

        HANDLE_MSG(pcli, WM_PAINT, Client_OnPaint);
        HANDLE_MSG(pcli, WM_ERASEBKGND, Client_OnEraseBkgnd);

        HANDLE_MSG(pcli, WM_QUERYENDSESSION, Client_OnQueryEndSession);
        HANDLE_MSG(pcli, WM_ENDSESSION, Client_OnEndSession);

        HANDLE_MSG(pcli, WM_COMMAND, Client_OnCommand);
    default:
        return Client_DefProc(hwnd, msg, wParam, lParam);
    }
}

// Private structure used for passing info via WM_CREATE message
typedef struct
{
    LPCSTR lpszText;
    COLORREF clrText;
} CLIENT_INIT;

HWND Client_CreateWindow(HWND hwndParent, int x, int y, int cx, int cy, BOOL fVisible, COLORREF clrText, LPCSTR lpszText)
{
    CLIENT_INIT init;

    init.lpszText = lpszText;
    init.clrText = clrText;

    return CreateWindowEx(
	    0L, 			    // extendedStyle
            "MakeApp_Client",               // class name
            NULL,                           // text
            (fVisible ? (WS_CHILD | WS_VISIBLE) : WS_CHILD),
            x, y, cx, cy,                   // x, y, cx, cy
            hwndParent,                     // hwndParent
	    NULL,			    // hmenu
            g_app.hinst,                    // hInstance
            &init);                         // lpCreateParams
}

BOOL Client_OnCreate(CLIENT* pcli, CREATESTRUCT FAR* lpCreateStruct)
{
    CLIENT_INIT FAR* pinit = (CLIENT_INIT FAR*)lpCreateStruct->lpCreateParams;

    pcli->lpszText = pinit->lpszText;
    pcli->clrText  = pinit->clrText;
    return TRUE;
}

void Client_OnDestroy(CLIENT* pcli)
{
}

void Client_OnPaint(CLIENT* pcli)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rc;

    hdc = BeginPaint(pcli->hwnd, &ps);

    GetClientRect(pcli->hwnd, &rc);
    //
    // No need to erase the background: that was done by WM_ERASEBKGND handling
    //
    InflateRect(&rc, -4, -4);
    FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));

    if (pcli->lpszText)
    {
        int cch = lstrlen(pcli->lpszText);
        int x;
        int y;
        SIZE size;

        GetTextExtentPoint(hdc, pcli->lpszText, cch, &size);

        x = rc.left + (rc.right - rc.left - size.cx) / 2;
        y = rc.top + (rc.bottom - rc.top - size.cy) / 2;

        SetTextColor(hdc, pcli->clrText);
        TextOut(hdc, x, y, pcli->lpszText, cch);
    }

    EndPaint(pcli->hwnd, &ps);
}

BOOL Client_OnEraseBkgnd(CLIENT* pcli, HDC hdc)
{
    return FORWARD_WM_ERASEBKGND(pcli->hwnd, hdc, Client_DefProc);
}

BOOL Client_OnQueryEndSession(CLIENT* pcli)
{
    // Signal that it's OK to end the session.
    //
    return TRUE;
}

void Client_OnEndSession(CLIENT* pcli, BOOL fEnding)
{
    // No end session cleanup to do.
}

void Client_OnCommand(CLIENT* pcli, int id, HWND hwndCtl, UINT codeNotify)
{
    // NOTE: WM_COMMAND not handled by the frame are passed to the client.
    //
    switch (id)
    {
    case CMD_SAMPLEDLG:
        SampleDlg_Do(pcli->hwnd);
        break;
    default:
        MessageBeep(0);
        break;
    }
}
