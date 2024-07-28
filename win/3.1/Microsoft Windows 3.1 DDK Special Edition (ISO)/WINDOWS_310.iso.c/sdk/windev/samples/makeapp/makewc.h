// MakeWc window class declarations
//
#ifndef _INC_MAKEWC
#define _INC_MAKEWC

// Public declarations

BOOL MakeWc_Initialize(APP* papp);
void MakeWc_Terminate(APP* papp);

HWND MakeWc_CreateWindow(HWND hwndParent, int x, int y, int cx, int cy, BOOL fVisible);

// Private declarations

// Window procedure

LRESULT CALLBACK _export MakeWc_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Default message handler

#define MakeWc_DefProc  DefWindowProc

// Instance data structure

typedef struct tagMAKEWC
{
    HWND hwnd;
} MAKEWC;

// Instance data pointer access functions

#if defined(M_I86SM) || defined(M_I86MM) || defined(__SMALL__) || defined(__MEDIUM__)
#define MakeWc_GetPtr(hwnd)         (MAKEWC*)GetWindowWord((hwnd), 0)
#define MakeWc_SetPtr(hwnd, pmwc)   (MAKEWC*)SetWindowWord((hwnd), 0, (WORD)(pmwc))
#else
#define MakeWc_GetPtr(hwnd)         (MAKEWC*)GetWindowLong((hwnd), 0)
#define MakeWc_SetPtr(hwnd, pmwc)   (MAKEWC*)SetWindowLong((hwnd), 0, (LONG)(pmwc))
#endif

// Message handler functions

BOOL MakeWc_OnCreate(MAKEWC* pmwc, CREATESTRUCT FAR* lpCreateStruct);
void MakeWc_OnDestroy(MAKEWC* pmwc);

void MakeWc_OnPaint(MAKEWC* pmwc);
BOOL MakeWc_OnEraseBkgnd(MAKEWC* pmwc, HDC hdc);

#endif  // !_INC_MAKEWC
