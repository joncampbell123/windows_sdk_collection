// App structure declarations
//
#ifndef _INC_APP
#define _INC_APP

// Public declarations

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int cmdShow);

typedef struct tagAPP
{
    MSG         msg;
    HINSTANCE   hinst;
    HINSTANCE   hinstPrev;
    LPSTR       lpszCmdLine;
    int         cmdShow;
    HWND        hwndMain;
    int         codeExit;
    BOOL        fQuit : 1;
} APP;

// Public globals

extern APP g_app;
extern UINT WM_MSGFILTER;

// Message crackers for new WM_MSGFILTER message

/* Cls_OnMsgFilter(HWND hwnd, MSG FAR* lpmsg, int context); */
#define HANDLE_WM_MSGFILTER(hwnd, wParam, lParam, fn) \
    (LRESULT)(DWORD)(fn)((hwnd), (MSG FAR*)(lParam), (int)(wParam))
#define FORWARD_WM_MSGFILTER(hwnd, lpmsg, context, fn) \
    (BOOL)(UINT)(fn)((hwnd), WM_MSGFILTER, (WPARAM)(context), (LPARAM)(lpmsg))

// Private declarations

BOOL App_Initialize(APP* papp);
void App_Run(APP* papp);

// App_Terminate() exit codes
#define TERM_QUIT           0
#define TERM_ENDSESSION     1
#define TERM_ERROR          2

void App_Terminate(APP* papp, int codeTerm);

BOOL App_ProcessNextMessage(APP* papp);
BOOL App_MsgFilter(APP* papp, MSG FAR* lpmsg, int context);
BOOL App_Idle(APP* papp);

BOOL App_InitializeHook(APP* papp);
void App_TerminateHook(APP* papp);
LRESULT CALLBACK _export App_MsgFilterHook(int code, WPARAM wParam, LPARAM lParam);

#endif  // !_INC_APP
