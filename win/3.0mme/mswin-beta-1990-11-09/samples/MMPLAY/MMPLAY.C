/*
 * MMPLAY.C -- Plays MacroMind Director(TM) movie files.
 */

#include <windows.h>
#include <mmp.h>
#include <wincom.h>
#include "MMPLAY.H"

#define MMP_FRAME_FIRST 1                   // Frame counter constants
#define MMP_FRAME_LAST  0x7FFF

#define STATE_NOTLOADED 0                   // Menu states
#define STATE_STOPPED   1
#define STATE_RUNNING   2

HANDLE  hInst;                              // App instance handle
HWND    hMainWnd;                           // Main app window handle
HWND    hFrameWnd;                          // Frame counter window handle

MMPID   idMovie;                            // Movie ID of the driver instance
BOOL    bRunning = FALSE;                   // Movie running?

char szAppName[] =        "Movie Player";
char szFullScreenName[] = "Full Screen Window";
char szTextBuffer[256];

FARPROC lpFrameHookProc;                    // Pointer to callback function
FARPROC lpOldFrameHookProc;                 // Pointer to former hooked fn

/* WinMain - Entry point for MMPLAY.
 */
int PASCAL  WinMain (
    HANDLE  hInstance,
    HANDLE  hPrevInstance,
    LPSTR   lpszCmdLind,
    int     nCmdShow )
{
    MSG msg;
    int iAnimateResult;

    if( (hMainWnd = AppInit(hInstance,hPrevInstance)) == NULL )
        return FALSE;

    /* This is a fairly standard message loop for an application playing
     * Macromind Director movie files. We only call MMPAnimate when there
     * are no other messages waiting.
     */    
    for( ;; )
    {
        if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
        {
               if( msg.message == WM_QUIT )
                   break;
               TranslateMessage(&msg);
               DispatchMessage(&msg);
        }
        else
        {
            iAnimateResult = mmpAnimate(idMovie);
            if(iAnimateResult != ANIM_RUNNING)
            {
                if ( bRunning && iAnimateResult == ANIM_STOPPED )
                {
                    MessageBox (hMainWnd,
                                "Movie's over.  Rewind to play again.",
                                szAppName, MB_OK | MB_ICONINFORMATION );

                    SendMessage(hMainWnd,WM_COMMAND,IDM_STOP,0L);
                }
                WaitMessage();
            }
        }
    }
    return msg.wParam;
}




/* AppInit -- Register main window and full-screen stage window classes.
 * Create main window.
 *
 * Params:  hInstance -- App instance handle
 *          hPrevInstance -- Previous app instance handle
 */

HWND AppInit (
    HANDLE hInstance,
    HANDLE hPrevInstance )
{
    WNDCLASS wndclass;
    hInst = hInstance;

    if( !hPrevInstance )
    {
        /* Register main window class.
         */
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = MainWndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(hInstance, "MPPlayIcon");
        wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
        wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = "MMLoadMenu";
        wndclass.lpszClassName = szAppName;

        if (!RegisterClass (&wndclass))
            return FALSE;

        /* Register full-screen stage window class.
         */
        wndclass.style = NULL;
        wndclass.lpfnWndProc = FullScreenWndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = NULL;
        wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
        wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = szFullScreenName;

        if (!RegisterClass (&wndclass))
            return FALSE;

        /* Register frame counter window class.
         */
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = CounterWndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = NULL;
        wndclass.hCursor = LoadCursor(NULL,IDC_ARROW);
        wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = "CounterClass";

        if (!RegisterClass (&wndclass))
            return FALSE;
    }

    return CreateWindow(szAppName,szAppName,
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN,
                        0, 0,CW_USEDEFAULT,CW_USEDEFAULT,
                        NULL, NULL,hInstance,NULL);
}



/* MainWndProc -- Window function for main application window.
 */

LONG FAR PASCAL MainWndProc (
    HWND hWnd,
    unsigned iMessage,
    WORD wParam,
    LONG lParam )
{
    PAINTSTRUCT ps;

    switch( iMessage )
    {
        case WM_CREATE:

            /* Open an instance of the MMP driver.
             */
            idMovie = mmpOpen(hInst, hWnd, MMP_OPEN_STATIC );
            if( idMovie == NULL )
            {
                MessageBox(hWnd, "Failed to start driver.",
                            szAppName, MB_OK | MB_ICONEXCLAMATION );
                PostQuitMessage(1);
                return -1;
            }
            break;

        case WM_COMMAND :
            return HandleCommands(hWnd, iMessage, wParam, lParam);
            break;

        case WM_PAINT :

            BeginPaint(hWnd, &ps);
            mmpUpdate(idMovie, ps.hdc, &ps.rcPaint);
            EndPaint(hWnd, &ps);
            break;

        case WM_MONITOR_TICK:
            MessageBox(hWnd,"Tick.",szAppName,MB_OK);
            break;

        case WM_DESTROY :

            /* When the user closes the window, we need to stop the animation
             * and close the MMP driver.
             */
            if ( bRunning )
            {
                mmpStopAnimating(idMovie, MMP_FRAME_DRAW);
            }

            if ( hFrameWnd )
                SendMessage(hFrameWnd,WM_CLOSE,0,0);

            if ( !mmpClose(idMovie, 0))
            {
                PrintError("Failed to stop driver.");
            }

            PostQuitMessage(0);

            break;

        default:
            return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }
    return 0L;
}


/* HandleCommands -- This function handles command messages for the
 * main application window. This is where much of the animation calls
 * are made.
 *
 * Params: Standard parameters passed to window function.
 */
LONG HandleCommands (
    HWND hWnd,
    int iMessage,
    WORD wParam,
    LONG lParam )
{
    static short nLastFrame;
    short nFrame;
    HMENU hMenu;
    FARPROC fpfn;

    switch( wParam )
        {
        case IDM_EXIT :
            PostMessage(hWnd, WM_CLOSE, 0, 0L);
            break;

        case IDM_ABOUT:
            fpfn = MakeProcInstance(AppAbout,hInst);
            DialogBox(hInst,"ABOUTBOX",hWnd,fpfn);
            FreeProcInstance (fpfn);
            break;

        case IDM_OPEN :

            /* Open new movie file. If function returns TRUE, we
             * have a valid file loaded. Set appropriate menu
             * states.
             */
            if( OpenMovie(hWnd) )
            {
                bRunning = !mmpAnimStopped(idMovie);
                if( bRunning )
                {
                    SetMenuStates(hWnd, STATE_RUNNING);
                }
                else
                {
                    SetMenuStates(hWnd, STATE_STOPPED);
                }
            }
            else
            {
                SetMenuStates(hWnd, STATE_NOTLOADED);
            }
            break;

        case IDM_CLOSE :
            if( bRunning )
            {
                SendMessage(hWnd,WM_COMMAND,IDM_STOP,0L);
            }
            if (mmpFreeFile(idMovie, MMP_FRAME_ERASE))
                SetMenuStates(hWnd, STATE_NOTLOADED);
            else
                PrintError("Could not free movie.");
            SetWindowText(hWnd,szAppName);
            break;

        case IDM_FULLSCREEN :

            /* Since we have no menu access during a full-screen playback
             * we'll start the movie if it isn't already running. Also, set
             * it to repeat automatically at the end of the movie.
             */
            if( !bRunning )
            {
                SendMessage(hWnd,WM_COMMAND,IDM_START,0L);
            }
            if ( !mmpGetRepeat(idMovie))
            {
                SendMessage(hWnd,WM_COMMAND,IDM_LOOPMOVIE,0L);
            }

            /* Create the full-screen stage window and switch movie
             * playback to it.
             */
            if( !CreateFullScreenStage(hWnd) )
            {
                MessageBox(hWnd, "Failed to create window.",
                            szAppName, MB_OK);
            }
            break;

        case IDM_STOP :
            mmpStopAnimating(idMovie, MMP_FRAME_IMAGE);
            bRunning = FALSE;
            SetMenuStates(hWnd, STATE_STOPPED);
            break;
            
        case IDM_START :
            mmpStartAnimating(idMovie, MMP_FRAME_DRAW);
            bRunning = TRUE;
            SetMenuStates(hWnd, STATE_RUNNING);

            break;

        case IDM_LOOPMOVIE:

            /* Here, we basically toggle the repeat state, changing
             * the menu option as appropriate.
             */                 
            hMenu = GetMenu(hWnd);

            if( mmpGetRepeat(idMovie) )
            {
                mmpSetRepeat(idMovie, FALSE);
                ModifyMenu(hMenu, IDM_LOOPMOVIE, MF_BYCOMMAND,
                                  IDM_LOOPMOVIE, "Start &looping");
            }
            else
            {
                mmpSetRepeat(idMovie, TRUE);
                ModifyMenu(hMenu, IDM_LOOPMOVIE, MF_BYCOMMAND,
                                  IDM_LOOPMOVIE, "Stop &looping");
                if( mmpAnimStopped(idMovie) )
                {
                      PostMessage(hWnd,WM_COMMAND,IDM_START, 0L);
                }

            }
            DrawMenuBar(hWnd);
            break;

        case IDM_REWIND:                    // Rewind
            mmpRewind(idMovie);
            break;

        case IDM_STEPFORWARD:                // Goto frame

            /* If the movie doesn't advance to the next
             * frame, we know we're at the end of the movie.
             */
            nFrame = mmpFrameCtr(idMovie);
            if (!mmpGoToFrame(idMovie, nFrame+1, MMP_FRAME_DRAW))
                PrintError("Could not go to specified frame.");
            if( nFrame == mmpFrameCtr(idMovie) )
            {
                MessageBox(hWnd,"Can't step beyond end.", szAppName, MB_OK);
            }
            break;
                                            
        case IDM_STEPBACKWARD:                // Goto frame

            /* Frame counting starts at 1, so if we end up with 0,
             * we can't go back any further.
             */
            nFrame = mmpFrameCtr(idMovie) - 1;
            if( nFrame < 1 )
            {
                MessageBox(hWnd,"Can't step before beginning.",
                            szAppName, MB_OK);
            }
            else
            {
                if (!mmpGoToFrame(idMovie, nFrame, MMP_FRAME_DRAW))
                    PrintError("Could not go to specified frame.");
            }
            break;

        case IDM_FRAMECTR:
            /* If frame counter window is active, destroy it (thus unhooking
             * function).
             */
            if ( hFrameWnd != NULL )
            {
                SendMessage(hFrameWnd,WM_CLOSE,0,0);
                break;
            }

            /* Create procedure instance for callback function.
             */
            lpFrameHookProc = MakeProcInstance(MonitorFrame,hInst);

            /* Create frame counter window.
             */
            hFrameWnd = CreateWindow("CounterClass","Frame",
                        WS_CHILD | WS_CAPTION | WS_VISIBLE | WS_SYSMENU,
                        0, 0, 100, 50,
                        hMainWnd, NULL,hInst,NULL);

            /* Save existing frame hook (should be NULL).
             */
            lpOldFrameHookProc = mmpGetFrameHook(idMovie);

            /* Set the hook!
             */
            if (!mmpSetFrameHook(idMovie,lpFrameHookProc))
                PrintError("mmpSetFrameHook failed.");

            break;


        default :
            return DefWindowProc (hWnd, iMessage, wParam, lParam);
        }
    return 0L;
}


/* FullScreenWndProc -- Window function for full-screen stage window.
 *
 * Params: Standard parameters passed to window function.
 */
LONG FAR PASCAL FullScreenWndProc (
    HWND hWnd,
    unsigned iMessage,
    WORD wParam,
    LONG lParam )
{
    PAINTSTRUCT ps;
    HWND hWndOwner;

    switch( iMessage )
    {
        case WM_KEYDOWN:

            /* When user presses a key, transfer the movie to 
             * the original stage.
             */
            hWndOwner = GetWindow(hWnd, GW_OWNER);

            if (!mmpSetStage(idMovie,hWndOwner,NULL,NULL))
                PrintError("Could not set stage.");

            DestroyWindow(hWnd);
            break;

        case WM_PAINT:

            BeginPaint(hWnd, &ps);
            mmpUpdate(idMovie, ps.hdc, &ps.rcPaint);
            EndPaint(hWnd, &ps);
            break;

        default:
            return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }
    return 0L;
}



/* CounterWndProc -- This is the window function for the frame counter window.
 * This window receives messages from the frame callback function, hooked
 * into the playback under the IDM_FRAMECTR case of the main window function.
 *
 * Params: Standard window function parameters.
 */
LONG FAR PASCAL CounterWndProc (
    HWND hWnd,
    unsigned iMessage,
    WORD wParam,
    LONG lParam )
{
    static short nFrame;
    static short nSubFrame;
    PAINTSTRUCT ps;

    switch( iMessage )
    {
        case WM_CREATE :
            nFrame = 0;
            nSubFrame = 0;
            break;

        case WM_PAINT :
            BeginPaint(hWnd, &ps);
            wsprintf(szTextBuffer,"Frame: %d.  ",nFrame);
            TextOut(ps.hdc,10,5,szTextBuffer,lstrlen(szTextBuffer));
            EndPaint(hWnd, &ps);
            break;

        case WM_FRAME_CTR:                      // Record frame and subframe
            nFrame = LOWORD(lParam);            // number (subframe not used
            nSubFrame = HIWORD(lParam);         // this release)
            InvalidateRect(hWnd,NULL,TRUE);
            break;

        case WM_DESTROY:
            mmpSetFrameHook(idMovie,lpOldFrameHookProc);
            FreeProcInstance(lpFrameHookProc);
            hFrameWnd = NULL;
            break;

        default:
            return DefWindowProc(hWnd, iMessage, wParam, lParam);
    }
    return 0L;
}


/* AppAbout -- Handles "About" dialog box message handling.
 *
 * Params:  Standard parameters passed to dialog box function.
 */
BOOL FAR PASCAL AppAbout(
    HWND     hDlg,
    unsigned msg,
    WORD     wParam,
    LONG     lParam )
{
    switch (msg) {
        case WM_COMMAND:
            if (wParam == IDOK)
            {
                EndDialog(hDlg,TRUE);
            }
	    break;

	case WM_INITDIALOG:
	    return TRUE;
    }
    return FALSE;
}



/* MonitorFrame -- This is a callback function to be hooked into the playback.
 * It uses a table of frame numbers and messages to monitor the playback,
 * sending the appropriate message to the appropriate window when certain
 * frame numbers are reached.
 *
 * Params: id -- Movie ID of the MMP driver instance calling this function
 *         sFrame -- Frame number of the frame displayed in stage window
 *         sSubFrame -- Not used this release (always 0)
 */
BOOL FAR PASCAL MonitorFrame(
    MMPID id,
    short sFrame,
    short sSubFrame)
{
    /* Tell the frame counter window the new frame and subframe number.
     */         
    SendMessage(hFrameWnd,WM_FRAME_CTR,0,MAKELONG(sFrame,sSubFrame));

    return TRUE;
}


/* CreateFullScreenStage - Create full-screen stage window and transfer
 * movie playback to it.
 *
 * Params:  hMainWindow - Handle to main application window.
 *
 * Return:  TRUE if window created successfully, FALSE otherwise.
 */
BOOL PASCAL CreateFullScreenStage (
    HWND hMainWindow )
{
    HWND  hFullWnd;
    short xScreen, yScreen;

    /* Get screen size.
     */
    xScreen = GetSystemMetrics (SM_CXSCREEN);
    yScreen = GetSystemMetrics (SM_CYSCREEN);

    hFullWnd = CreateWindow(szFullScreenName, NULL, WS_POPUP | WS_VISIBLE,
                            0, 0, xScreen, yScreen,
                            hMainWindow, NULL, hInst, NULL);

    if( hFullWnd == NULL )
    {
        return FALSE;
    }

    /* Switch the stage to the full-screen window.
     */
    if (!mmpSetStage(idMovie,hFullWnd, NULL, MMP_STAGE_BORDER))
        PrintError("Could not set stage.");

    return TRUE;
}


/* OpenMovie -- This function uses the common code library OpenFileDialog
 * function to get a movie filename for playback. It then loads the movie
 * file.
 *
 * Params:  hWnd -- Window handle of main application window
 *
 * Return:  TRUE if movie loaded when function finished, FALSE otherwise.
 */
BOOL OpenMovie (
    HWND hWnd )
{
    BOOL bLoadResult;                       // Result of loading file
    char szFilename[128];
    int i;
    MMPFileInfo mmpFileInfo;

    HCURSOR hSaveCursor;
    HCURSOR hHourGlass;

    /* First, get a filename. If user presses cancel, return a value
     * indicating whether a movie is currently loaded.
     */    
    if( OpenFileDialog(hWnd, "Open Animation File",
                       "*.mmm", DLGOPEN_MUSTEXIST | DLGOPEN_OPEN,
                       szFilename, 127) != DLG_OKFILE )
    {
        return ((BOOL)mmpFileLoaded(idMovie));
    }

    /* If a movie is loaded, unload it and erase the stage window.
     */
    if( mmpFileLoaded(idMovie) )
    {
        if( !mmpAnimStopped(idMovie) )
        {
            SendMessage(hWnd, WM_COMMAND, IDM_STOP, 0L);
        }
        if( !mmpFreeFile(idMovie, MMP_FRAME_ERASE))
            PrintError("Could not free movie file.");

        SetWindowText(hWnd,szAppName);
    }

    /* Load the new movie file.
     */
    hHourGlass = LoadCursor(NULL, IDC_WAIT);
    hSaveCursor = SetCursor(hHourGlass);

    bLoadResult = mmpLoadFile(idMovie, szFilename, MMP_FRAME_DRAW);

    SetCursor(hSaveCursor);

    if( bLoadResult )
    {
          mmpFileInfo.achFullName[0] = 0;
          mmpGetFileInfo(idMovie,szFilename,&mmpFileInfo);
          if( mmpFileInfo.achFullName[0] )
          {
              wsprintf(szTextBuffer,"%s - %s",
                       (LPSTR)szAppName,(LPSTR)mmpFileInfo.achFullName);
          }
          else
          {
              for( i = lstrlen(szFilename)-1;
                   i && szFilename[i] != '\\' && szFilename[i] != ':'; i-- );
              if( szFilename[i] == '\\' || szFilename[i] == ':' )
                  i++;
              wsprintf(szTextBuffer,"%s - %s",
                       (LPSTR)szAppName,(LPSTR)(szFilename+i));
          }
        SetWindowText(hWnd,szTextBuffer);
    }
    else
    {
        PrintError("Could not load movie file.");
    }

    return bLoadResult;
}


/* SetMenuStates -- Enable or disable pull down menus as needed depending
 * on whether the movie file is absent, stopped, or running.
 *
 * Params:  hWnd -- Handle to main application window
 *          wState -- Flag indicating state (absent, stopped, running)
 */

void SetMenuStates (
    HWND hWnd,
    WORD wState )
{
    HMENU hMenu;

    switch( wState )
    {
        case STATE_NOTLOADED :
            hMenu = LoadMenu(hInst, "MMLoadMenu");
            break;
        
        case STATE_STOPPED :
            hMenu = LoadMenu(hInst, "MMStopMenu");
            if( mmpGetRepeat(idMovie) )
            {
                ModifyMenu(hMenu, IDM_LOOPMOVIE, MF_BYCOMMAND,
                                  IDM_LOOPMOVIE, "Stop &looping");
            }
            break;

        case STATE_RUNNING :
            hMenu = LoadMenu(hInst, "MMRunMenu");
            if( mmpGetRepeat(idMovie) )
            {
                ModifyMenu(hMenu, IDM_LOOPMOVIE, MF_BYCOMMAND,
                                IDM_LOOPMOVIE, "Stop &looping");
            }
            break;
    }
    SetMenu(hWnd, hMenu);
}



/* PrintError -- This function calls the MMP driver error function to
 * get an error message to display.
 *
 * Params: lpszIntro - Introductory text for error message
 */
void PrintError(
    LPSTR lpszIntro )
{
    char *szT;

    wsprintf((LPSTR)szTextBuffer,"%s  MMP driver error was:  ", lpszIntro);
    
    szT = szTextBuffer+lstrlen(szTextBuffer);
    mmpError(idMovie,szT);

    MessageBox(hMainWnd,szTextBuffer,szAppName,MB_OK | MB_ICONEXCLAMATION);
}
