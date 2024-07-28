// Frame window class declarations
//
#ifndef _INC_FRAME
#define _INC_FRAME

// Public declarations

BOOL Frame_Initialize(APP* papp);
void Frame_Terminate(APP* papp, int codeTerm);

HWND Frame_CreateWindow(
        LPCSTR lpszText,
        int x,
        int y,
        int cx,
        int cy,
        HINSTANCE hinst);

// AboutBox definitions

void AboutDlg_Do(HWND hwndOwner);

BOOL CALLBACK _export AboutDlg_DlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);

// Private declarations

// Window procedure

LRESULT CALLBACK _export Frame_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Default message handler

#define Frame_DefProc   DefWindowProc

// Instance data structure

typedef struct tagFRAME
{
    HWND hwnd;
    HACCEL haccel;
    HWND hwndClient;
} FRAME;

// Instance data pointer access functions

#if defined(M_I86SM) || defined(M_I86MM) || defined(__SMALL__) || defined(__MEDIUM__)
#define Frame_GetPtr(hwnd)          (FRAME*)GetWindowWord((hwnd), 0)
#define Frame_SetPtr(hwnd, pfrm)    (FRAME*)SetWindowWord((hwnd), 0, (WORD)(pfrm))
#else
#define Frame_GetPtr(hwnd)          (FRAME*)GetWindowLong((hwnd), 0)
#define Frame_SetPtr(hwnd, pfrm)    (FRAME*)SetWindowLong((hwnd), 0, (LONG)(pfrm))
#endif

// Message handler declarations

BOOL Frame_OnCreate(FRAME* pfrm, CREATESTRUCT FAR* lpCreateStruct);
void Frame_OnDestroy(FRAME* pfrm);
void Frame_OnClose(FRAME* pfrm);
BOOL Frame_OnQueryEndSession(FRAME* pfrm);
void Frame_OnEndSession(FRAME* pfrm, BOOL fEnding);
void Frame_OnSize(FRAME* pfrm, UINT state, int cx, int cy);
void Frame_OnActivate(FRAME* pfrm, UINT state, HWND hwndActDeact, BOOL fMinimized);
void Frame_OnInitMenu(FRAME* pfrm, HMENU hMenu);
void Frame_OnInitMenuPopup(FRAME* pfrm, HMENU hMenu, int item, BOOL fSystemMenu);
void Frame_OnCommand(FRAME* pfrm, int id, HWND hwndCtl, UINT codeNotify);
BOOL Frame_OnMsgFilter(FRAME* pfrm, MSG FAR* lpmsg, int context);


#endif  // !_INC_FRAME
