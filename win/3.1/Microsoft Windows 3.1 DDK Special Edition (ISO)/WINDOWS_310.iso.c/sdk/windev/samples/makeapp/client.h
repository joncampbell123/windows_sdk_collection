// Client window class declarations
//
#ifndef _INC_CLIENT
#define _INC_CLIENT

// Public declarations

BOOL Client_Initialize(APP* papp);
void Client_Terminate(APP* papp, int codeTerm);

HWND Client_CreateWindow(HWND hwndParent, int x, int y, int cx, int cy, BOOL fVisible, COLORREF clrText, LPCSTR lpszText);

// Private declarations

// Window procedure

LRESULT CALLBACK _export Client_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Default message handler

#define Client_DefProc  DefWindowProc

// Instance data structure

typedef struct tagCLIENT
{
    HWND hwnd;
    COLORREF clrText;
    LPCSTR lpszText;
} CLIENT;

// Instance data pointer access functions

#if defined(M_I86SM) || defined(M_I86MM) || defined(__SMALL__) || defined(__MEDIUM__)
#define Client_GetPtr(hwnd)         (CLIENT*)GetWindowWord((hwnd), 0)
#define Client_SetPtr(hwnd, pcli)   (CLIENT*)SetWindowWord((hwnd), 0, (WORD)(pcli))
#else
#define Client_GetPtr(hwnd)         (CLIENT*)GetWindowLong((hwnd), 0)
#define Client_SetPtr(hwnd, pcli)   (CLIENT*)SetWindowLong((hwnd), 0, (LONG)(pcli))
#endif

// Message handler functions

BOOL Client_OnCreate(CLIENT* pcli, CREATESTRUCT FAR* lpCreateStruct);
void Client_OnDestroy(CLIENT* pcli);

void Client_OnPaint(CLIENT* pcli);
BOOL Client_OnEraseBkgnd(CLIENT* pcli, HDC hdc);

BOOL Client_OnQueryEndSession(CLIENT* pcli);
void Client_OnEndSession(CLIENT* pcli, BOOL fEnding);

void Client_OnCommand(CLIENT* pcli, int id, HWND hwndCtl, UINT codeNotify);

#endif  // !_INC_CLIENT
