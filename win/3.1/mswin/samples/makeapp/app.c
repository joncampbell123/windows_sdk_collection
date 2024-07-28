#include "makeapp.h"

// Globals

APP g_app;
UINT WM_MSGFILTER = 0;      // Private message

int PASCAL WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR lpszCmdLine, int cmdShow)
{
    // Initialize the APP structure
    //
    g_app.hinst       = hinst;
    g_app.hinstPrev   = hinstPrev;
    g_app.lpszCmdLine = lpszCmdLine;
    g_app.cmdShow     = cmdShow;
    g_app.hwndMain    = NULL;
    g_app.codeExit    = 1;      // Assume failure
    g_app.fQuit       = FALSE;

    // Initialize, run, and terminate the application
    //
    if (App_Initialize(&g_app))
        App_Run(&g_app);

    App_Terminate(&g_app, (g_app.codeExit == 0 ? TERM_QUIT : TERM_ERROR));

    return g_app.codeExit;
}

// Process messages until it's time to quit.
//
void App_Run(APP* papp)
{
    while (App_ProcessNextMessage(papp))
        ;
}

// Process the next incoming message, or, if none are available,
// perform a slice of idle processing.
//
// Returns TRUE to continue processing messages, FALSE to quit the app.
//
BOOL App_ProcessNextMessage(APP* papp)
{
    // If we've already processed a WM_QUIT message, just return TRUE.
    //
    if (papp->fQuit)
        return FALSE;

    // If a message exists in the queue, translate and dispatch it.
    //
    if (PeekMessage(&papp->msg, NULL, 0, 0, PM_REMOVE))
    {
        // See if it's time to quit...
        //
        if (papp->msg.message == WM_QUIT)
        {
            papp->codeExit = (int)papp->msg.wParam;
            papp->fQuit = TRUE;
            return FALSE;
        }

        // Call the message filter hook to handle
        // accelerators, modal dialog messages, and the like
        //
        if (!CallMsgFilter(&papp->msg, MSGF_MAINLOOP))
        {
            TranslateMessage(&papp->msg);
            DispatchMessage(&papp->msg);
        }
    }
    else
    {
        // No messages: do idle processing.
        // If the idle proc need not be called any longer,
        // call WaitMessage() to suspend the application.
        //
        if (!App_Idle(papp))
            WaitMessage();
    }
    return TRUE;
}

static HOOKPROC lpfnMsgFilter = NULL;
static HHOOK hhook = NULL;

// Register the "WM_MSGFILTER" window message, and install our
// message filter hook procedure.
//
BOOL App_InitializeHook(APP* papp)
{
    WM_MSGFILTER = RegisterWindowMessage("WM_MSGFILTER");
    if (WM_MSGFILTER == NULL)
        return FALSE;

    lpfnMsgFilter = (HOOKPROC)MakeProcInstance((FARPROC)App_MsgFilterHook, papp->hinst);
    if (lpfnMsgFilter == NULL)
        return FALSE;

    hhook = SetWindowsHook(WH_MSGFILTER, lpfnMsgFilter);
    if (hhook == NULL)
        return FALSE;

    return TRUE;
}

// If necessary, uninstall our message filter hook.
//
void App_TerminateHook(APP* papp)
{
    if (lpfnMsgFilter)
    {
        FreeProcInstance((FARPROC)lpfnMsgFilter);
        lpfnMsgFilter = NULL;
    }
}

// Message filter hook proc.  Calls App_MsgFilter to do the work,
// after guarding against infinite recursion.
//
LRESULT CALLBACK _export App_MsgFilterHook(int code, WPARAM wParam, LPARAM lParam)
{
    BOOL fProcessed = FALSE;
    static LPARAM lParamPrev = NULL;

    if (code < 0)
        return DefHookProc(code, wParam, lParam, &hhook);

    // Prevent infinite recursion if CallMsgFilter is called recursively,
    // such as from a call to IsDialogMessage(), which calls CallMsgFilter.
    //
    if (lParam == lParamPrev)
        return (LRESULT)FALSE;

    lParamPrev = lParam;

    // Call App_MsgFilter to filter the message.
    //
    fProcessed = App_MsgFilter(&g_app, (MSG FAR*)lParam, code);

    lParamPrev = NULL;

    return (LRESULT)fProcessed;
}

// All posted (queued) messages are routed here via the WH_MSGFILTER hook.
//
BOOL App_MsgFilter(APP* papp, MSG FAR* lpmsg, int context)
{
    HWND hwnd;
    HWND hwndNext;
    BOOL fSentToMain = FALSE;

    // Don't filter WM_PAINT messages, to make repainting
    // as efficient as possible.  It's not useful or possible
    // to filter WM_PAINT messages here anyhow since many are
    // sent directly to destination window via UpdateWindow().
    //
    if (lpmsg->message == WM_PAINT)
        return FALSE;

    // This loop enumerates the destination window and all its parents,
    // until a top-level window is reached.  Then, GetParent() enumerates
    // owner windows: in this way, an app's main window can filter messages
    // destined for its owned dialogs or dialog controls.
    //
    for (hwnd = lpmsg->hwnd; hwnd != NULL; hwnd = hwndNext)
    {
        // Get the "next" window before we send the message,
        // in case hwnd is destroyed while handling the message.
        //
        hwndNext = GetParent(hwnd);

        if (hwnd == papp->hwndMain)
            fSentToMain = TRUE;

        if (FORWARD_WM_MSGFILTER(hwnd, lpmsg, context, SendMessage))
            return TRUE;
    }

    // If we haven't already sent the message to the main window,
    // do so now.  This ensures that the main window has a chance
    // to filter any unfiltered messages for any window in this app.
    //
    if (!fSentToMain)
        return FORWARD_WM_MSGFILTER(papp->hwndMain, lpmsg, context, SendMessage);

    return FALSE;
}

// This function is called when there are no messages available in the
// queue, and can be used to perform "background" processing tasks.
//
// As with the processing of messages, you should not spend too much
// time in this call, or the application will not seem responsive to
// user actions.
//
// App_Idle() will continue to be called as long as the function returns
// TRUE.
//
BOOL App_Idle(APP* papp)
{
    // Return TRUE to get called again (i.e., there is more work to do),
    // FALSE if there is no more idle processing to do.
    //
    return FALSE;
}

// Initialize the application
//
// Startup information is provided in *papp.
//
BOOL App_Initialize(APP* papp)
{
    if (!App_InitializeHook(papp))
        return FALSE;

    if (!Frame_Initialize(papp))
        return FALSE;

    if (!Client_Initialize(papp))
        return FALSE;

    papp->hwndMain = Frame_CreateWindow(
            "MakeApp Application",
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            papp->hinst);

    if (papp->hwndMain == NULL)
	return FALSE;

    ShowWindow(papp->hwndMain, papp->cmdShow);

    return TRUE;
}

void App_Terminate(APP* papp, int codeTerm)
{
    // Called on app termination.  codeTerm indicates the
    // reason for the termination:
    //
    // TERM_QUIT: The application main loop terminated normally
    // due to recieving a WM_QUIT message.  You should destroy all
    // created windows, GDI objects, and any other resources that
    // may have been allocated.
    //
    // TERM_ENDSESSION: The application is being terminated because
    // the windows session is being ended by the user.  The application
    // need NOT destroy windows, GDI objects, or memory, but simply
    // flush disk caches, close files, etc.
    //
    // TERM_ERROR: An error occured while initializing the app,
    // or the application main loop was terminated abnormally with
    // a WM_QUIT message with a non-zero value in wParam.
    // The application should destroy all allocated windows, GDI
    // objects, and other allocated resources, as well as flush
    // caches and such to disk.
    //
    if (codeTerm != TERM_ENDSESSION)
    {
        if (papp->hwndMain)
        {
            DestroyWindow(papp->hwndMain);
            papp->hwndMain = NULL;
        }
    }

    Client_Terminate(papp, codeTerm);
    Frame_Terminate(papp, codeTerm);

    App_TerminateHook(papp);
}
