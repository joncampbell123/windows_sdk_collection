#include "makeapp.h"

BOOL MakeWc_Initialize(APP* papp)
{
    if (!papp->hinstPrev)
    {
        WNDCLASS cls;

        cls.hCursor         = LoadCursor(NULL, IDC_ARROW);
        cls.hIcon           = NULL;
        cls.lpszMenuName    = NULL;
        cls.hInstance       = papp->hinst;
        cls.lpszClassName   = "MakeApp_MakeWc";
        cls.hbrBackground   = (HBRUSH)(COLOR_WINDOW+1);
        cls.lpfnWndProc     = MakeWc_WndProc;
        cls.style           = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        cls.cbWndExtra      = sizeof(MAKEWC*);
        cls.cbClsExtra      = 0;

        if (!RegisterClass(&cls))
            return FALSE;
    }

    return TRUE;
}

void MakeWc_Terminate(APP* papp)
{
}

LRESULT CALLBACK _export MakeWc_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    MAKEWC* pmwc = MakeWc_GetPtr(hwnd);

    if (pmwc == NULL)
    {
        if (msg == WM_NCCREATE)
        {
            pmwc = (MAKEWC*)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, sizeof(MAKEWC));

            if (pmwc == NULL)
                return 0L;

            pmwc->hwnd = hwnd;
            MakeWc_SetPtr(hwnd, pmwc);
        }
        else
        {
            return MakeWc_DefProc(hwnd, msg, wParam, lParam);
        }
    }

    if (msg == WM_NCDESTROY)
    {
        //DWORD result = HANDLE_MSG(hwnd, WM_NCDESTROY, MakeWc_OnNCDestroy);

        LocalFree((HLOCAL)pmwc);
        pmwc = NULL;
        MakeWc_SetPtr(hwnd, NULL);

        //return result;
    }

    switch (msg)
    {
        HANDLE_MSG(pmwc, WM_CREATE, MakeWc_OnCreate);
        HANDLE_MSG(pmwc, WM_DESTROY, MakeWc_OnDestroy);

        HANDLE_MSG(pmwc, WM_PAINT, MakeWc_OnPaint);
        HANDLE_MSG(pmwc, WM_ERASEBKGND, MakeWc_OnEraseBkgnd);

    default:
        return MakeWc_DefProc(hwnd, msg, wParam, lParam);
    }
}

HWND MakeWc_CreateWindow(HWND hwndParent, int x, int y, int cx, int cy, BOOL fVisible)
{
    return CreateWindowEx(
	    0L, 			    // extendedStyle
            "MakeApp_MakeWc",               // class name
            NULL,                           // text
            (fVisible ? (WS_CHILD | WS_VISIBLE) : WS_CHILD),
            x, y, cx, cy,                   // x, y, cx, cy
            hwndParent,                     // hwndParent
	    NULL,			    // hmenu
            g_app.hinst,                    // hInstance
            NULL);                          // lpCreateParams
}

BOOL MakeWc_OnCreate(MAKEWC* pmwc, CREATESTRUCT FAR* lpCreateStruct)
{
    return TRUE;
}

void MakeWc_OnDestroy(MAKEWC* pmwc)
{
}

void MakeWc_OnPaint(MAKEWC* pmwc)
{
    PAINTSTRUCT ps;
    HDC hdc;

    hdc = BeginPaint(pmwc->hwnd, &ps);
    EndPaint(pmwc->hwnd, &ps);
}

BOOL MakeWc_OnEraseBkgnd(MAKEWC* pmwc, HDC hdc)
{
    return FORWARD_WM_ERASEBKGND(pmwc->hwnd, hdc, MakeWc_DefProc);
}
