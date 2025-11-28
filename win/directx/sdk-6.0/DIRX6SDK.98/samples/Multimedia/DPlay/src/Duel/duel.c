/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       duel.c
 *  Content:    Multi-player duel
 *
 *  This app is UNICODE ready.
 *
 *
 ***************************************************************************/
#define INITGUID
#include "duel.h"
#include "gameproc.h"
#include "gfx.h"
#include "comm.h"
#include "input.h"
#include "lobby.h"
#include "wizard.h"
#include "sfx.h"

// {33925241-05F8-11d0-8063-00A0C90AE891}
DEFINE_GUID(DUEL_GUID, 
0x33925241, 0x5f8, 0x11d0, 0x80, 0x63, 0x0, 0xa0, 0xc9, 0xa, 0xe8, 0x91);

/*
 * Externals
 */
extern DWORD			gdwFrameCount;
extern DWORD			gdwFrameTime;
extern int				gnProgramState;
extern SHIP				gOurShip;
extern LPDPLCONNECTION	glpdplConnection;
extern DPID				gOurID;
extern BOOL             gbNoField;


/*
 * Globals  are zeroed at load time.
 */
LPGUID                  glpGuid;				// Duel's GUID
HWND                    ghWndMain;				// Main application window handle
HINSTANCE				ghinst;					// Application instance handle		
BOOL                    gbShowFrameCount=TRUE;	// Show FPS ?
BOOL                    gbIsActive;				// Is the application active ?
BOOL                    gbUseEmulation;			// DDHEL or DDHAL for Graphics
BOOL					gbIsHost;				// Are we hosting or joining a game		
DWORD                   gdwKeys;				// User keyboard input
DWORD                   gdwOldKeys;             // Last frame's keyboard input
BOOL					gbFullscreen=FALSE;		// Window or FullScreen mode ?
RECT					grcWindow;				// client rectangle of main window
HANDLE					ghThread;				// handle to wizard thread
TCHAR					gtszClassName[MAX_CLASSNAME]; // Duel's class name	
BOOL                    gbReliable;             // sends are reliable
BOOL                    gbAsync;                // asynchronous sends
BOOL                    gbAsyncSupported;       // asynchronous sends supported
BOOL                    gbUseProtocol;          // DirectPlay Protocol messaging

/*
 * Statics
 */
static BOOL				gbReinitialize;			// used for switching display modes

/*
 * Local prototypes
 */
long WINAPI	MainWndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL		InitApplication(HINSTANCE hinst);
void		CleanupApplication(void);
BOOL		LaunchedByLobby(void);
BOOL		FinishLobbyLaunch(void);
void		DoHelp(void);

/*
 * WinMain
 */
int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                        int nCmdShow )
{
	BOOL bHelp=FALSE;
    MSG     msg;

	ghinst = hinstance;

	CoInitialize( NULL );

	// Parse command line
    while( lpCmdLine[0] == '-' )
    {
        lpCmdLine++;

        switch (*lpCmdLine++)
        {
        case 'e':					// DirectDraw emulation mode
            gbUseEmulation = TRUE;
            break;
        case 'd':					// game space is Devoid of any blocks
            gbNoField = TRUE;
            break;
        case '?':					// just show Help
		default:
            bHelp= TRUE;
            gbFullscreen= FALSE;	// windowed mode
            break;
        }

        while( isspace(*lpCmdLine) )
		{
			lpCmdLine++;
		}
    }

    /*
     * Give user help if asked for
     */

    if( bHelp )
    {
		DoHelp();
		return 0;
    }


    if( !InitApplication(hinstance) )
    {
        return 0;
    }

	// were we launched by a lobby ?
    if (LaunchedByLobby())
    {
		// start game
		PostMessage(ghWndMain, UM_LAUNCH, (WPARAM)TRUE, 0);
        gbIsActive = TRUE;
    }

    gdwFrameTime = timeGetTime();

	while( TRUE )
    {
        if (gbIsActive)
        {
            // any windows messages ? (returns immediately)
            if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
            {
                if( !GetMessage( &msg, NULL, 0, 0 ) )
                {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                // Poll our receive queue. Polling is used in the sample only for simplicity.
                // Receiving messages using an event is the recommended way.
		        if (gnProgramState != PS_SPLASH)
                {
                    ReceiveMessages();
					LobbyMessageReceive(LMR_PROPERTIES);
                }

                // update screen
	    		if (!UpdateFrame())
                {
                    ExitGame();		// posts QUIT msg
                }
		    }
        }
        else
        {
            // any windows messages ? (blocks until a message arrives)
            if( !GetMessage( &msg, NULL, 0, 0 ) )
            {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }	// message loop

	CoUninitialize();

    return msg.wParam;
} /* WinMain */


/*
 * MainWndproc
 *
 * Callback for all Windows messages
 */
long WINAPI MainWndproc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC         hdc;
	DWORD dwTid;

    switch( message )
    {
    case WM_SIZE:
    case WM_MOVE:
		// get the client rectangle
        if (gbFullscreen)
        {
            SetRect(&grcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
        }
        else
        {
            GetClientRect(hWnd, &grcWindow);
            ClientToScreen(hWnd, (LPPOINT)&grcWindow);
            ClientToScreen(hWnd, (LPPOINT)&grcWindow+1);
        }
		break;

    case WM_ACTIVATE:
		// ignore this message during reinitializing graphics
		if (gbReinitialize) return 0;

        // When we are deactivated, although we don't update our screen, we still need to
        // to empty our receive queue periodically as messages will pile up otherwise. 
        // Polling the receive queue continuously even when we are deactivated causes our app
        // to consume all the CPU time. To avoid hogging the CPU, we block on GetMessage() WIN API
        // and setup a timer to wake ourselves up at regular intervals to process our messages.

        if (LOWORD(wParam) == WA_INACTIVE)
        {
            // deactivated
            gbIsActive = FALSE;
            if (PS_ACTIVE == gnProgramState)
            {
                SetTimer(hWnd, RECEIVE_TIMER_ID, RECEIVE_TIMEOUT, NULL);
            }
        }
        else
        {
            // activated
            gbIsActive = TRUE;
            if (PS_ACTIVE == gnProgramState)
            {
                KillTimer(hWnd, RECEIVE_TIMER_ID);
            }
        }

        // set game palette, if activated in game mode
        if (gbIsActive && (gnProgramState != PS_SPLASH))
			SetGamePalette();

        ReacquireInputDevices();

        return 0;

    case WM_CREATE:
        break;

    case WM_SYSKEYUP:
		switch( wParam )
        {
        // handle ALT+ENTER (fullscreen/window mode)
        case VK_RETURN:
			// mode switch is allowed only during the game
			if (gnProgramState == PS_ACTIVE)
			{
				gbReinitialize = TRUE;
                ReleaseLocalData();  //only sound buffers have to be rels'd anyway.
                CleanupSfx();
                CleanupInput();
				CleanupGraphics();
				DestroyWindow(ghWndMain);
				gbFullscreen = !gbFullscreen;
				InitGraphics();
                InitInput();
                InitSfx();
                InitLocalSoundData();
				gbReinitialize = FALSE;
            }
            break;
        }
        break;

    case WM_KEYDOWN:
        switch( wParam )
        {
        case 'a':
        case 'A':
			// toggle Async sends on/off
			if (gbAsyncSupported)
			{
				gbAsync = !gbAsync;
				UpdateTitle();		// caption bar status
			}
            break;

        case 'r':
        case 'R':
			// toggle reliable sends
			gbReliable = !gbReliable;
			UpdateTitle();
            break;

		case VK_F1:
			// display help
			DoHelp();
			break;

        case VK_F5:
			// toggle frame rate display
            gbShowFrameCount = !gbShowFrameCount;
            if( gbShowFrameCount )
            {
                gdwFrameCount = 0;
                gdwFrameTime = timeGetTime();
            }
            break;

        case VK_RETURN:
			// launch game setup wizard
            if( (gnProgramState == PS_SPLASH) && !gbFullscreen)
			{
				// get connection settings from user
				ghThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE )DoWizard, 0, 0, &dwTid);
			}
            break;

        case VK_ESCAPE:
        case VK_F12:
			// adios
			ExitGame();
            return 0;
        }
        break;

    case WM_ERASEBKGND:
    	return 1;

    case WM_PAINT:
        hdc = BeginPaint( hWnd, &ps );
		if (gnProgramState == PS_SPLASH)
		{
            // display the splash screen
            bltSplash(NULL);
		}

        EndPaint( hWnd, &ps );
        return 1;

	case UM_LAUNCH:
	case UM_ABORT:
		// cleanup the wizard thread
		if (ghThread)
		{
			// wait for thread to exit
			WaitForSingleObject(ghThread, INFINITE);
			CloseHandle(ghThread);
			ghThread = NULL;
		}
		
		if (message == UM_LAUNCH && 
			// AND NOT (failed to finish a lobby launch)
			// where wParam is bLobbyLaunched
			!((BOOL)wParam && !FinishLobbyLaunch()))
		{
			// start the game in rest mode
			gnProgramState = PS_REST;
			LaunchGame();
			return 1;
		}
		// else aborting
        ExitGame();
		return 1;

    case WM_TIMER:
        ReceiveMessages();
		LobbyMessageReceive(LMR_PROPERTIES);
        break;

    case WM_DESTROY:
		// if gbReinitialize is TRUE don't quit, we are just switching display modes
		if (!gbReinitialize)
		{
			CleanupApplication();
	        PostQuitMessage( 0 );
		}
        return 0;

    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);

} /* MainWndproc */

/*
 * InitApplication
 *
 * Do that initialization stuff...
 */
BOOL InitApplication( HINSTANCE hinst )
{
    WNDCLASS    wc;
    BOOL        rc;

    glpGuid = (LPGUID) &DUEL_GUID;

	LoadString(ghinst, IDS_DUEL_CLASSNAME, gtszClassName, MAX_CLASSNAME);

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = MainWndproc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinst;
    wc.hIcon = LoadIcon( hinst, MAKEINTRESOURCE(DUEL_ICON));
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = gtszClassName;
    rc = RegisterClass( &wc );
    if( !rc )
    {
        return FALSE;
    }

	// Initialize all components
	if ((!InitGraphics()) || (!InitInput()) || (!InitSfx()))
	{
		return FALSE;
	}

	// start in splash mode
    gnProgramState = PS_SPLASH;

    return TRUE;

} /* initApplication */

/*
 * CleanupApplication
 *
 * Calls clean up on all components
 */
void CleanupApplication( void )
{
	CleanupComm();
    CleanupSfx();
	CleanupGraphics();
	CleanupInput();
    DPLobbyRelease();				// in case we were doing lobby messages
}

/*
 * LaunchedByLobby
 *
 * Determines if we were launched by a lobby. If so, gets the connection settings
 * and creates our player using the information from the lobby
 */
BOOL LaunchedByLobby(void)
{
    HRESULT hr;

    // create a lobby object
    hr = DPLobbyCreate();
    if (FAILED(hr))
    {
		// Interface is not available, so tell them to get DPlay 6 or later
		if (hr == E_NOINTERFACE)
			ShowError(IDS_DPLAY_ERROR_CLSID);
		else
			ShowError(IDS_DPLOBBY_ERROR_C);
        return FALSE;
    }

    // get connection settings from the lobby (into glpdplConnection)
    hr = DPLobbyGetConnectionSettings();
    if (FAILED(hr))
    {
        if (DPERR_NOTLOBBIED != hr)
        {
            ShowError(IDS_DPLOBBY_ERROR_GCS);
		    // Send Close message
			ExitGame();
        }
        // else we were not lobbied - start up game normally
		hr = DPLobbyRelease();
        return FALSE;
    }

    return TRUE;
}

/*
 * FinishLobbyLaunch
 *
 * Connect to the lobby and create our player
 */
BOOL FinishLobbyLaunch(void)
{
    HRESULT hr;

	if (!glpdplConnection)
	{
		TRACE(_T("FinishLobbyLaunch - No lobby interface\n"));
		return FALSE;
	}

	// are we hosting or joining ?
	gbIsHost = (glpdplConnection->dwFlags & DPLCONNECTION_CREATESESSION) != 0;

    // set our session flags
    glpdplConnection->lpSessionDesc->dwFlags = DPSESSION_MIGRATEHOST | 
                                               DPSESSION_KEEPALIVE;

    // let lobby know our connection flags
    hr = DPLobbySetConnectionSettings();
    if (FAILED(hr))
    {
        ShowError(IDS_DPLOBBY_ERROR_SCS);
        goto FAIL;
    }

    // connect to the lobby
	hr = DPLobbyConnect();
    if (FAILED(hr))
    {
        ShowError(IDS_DPLOBBY_ERROR_CONNECT);
        goto FAIL;
    }

	// create our player
    hr = DPlayCreatePlayer(&gOurID,
#ifdef UNICODE
							glpdplConnection->lpPlayerName->lpszShortName,
#else
							glpdplConnection->lpPlayerName->lpszShortNameA,
#endif
							NULL, NULL, 0);
    if (FAILED(hr))
    {
		ShowError(IDS_DPLAY_ERROR_CP);
        goto FAIL;
    }

    // Init lobby msg support for reporting score
	// Note that we don't release the lobby object
	LobbyMessageInit();

    // successful lobby launch
    return TRUE;

FAIL:
	// cleanup and exit
	DPLobbyRelease();
    return FALSE;
}

/*
 * Displays error to the user
 */
BOOL ShowError( int iStrID )
{
	TCHAR tszMsg[MAX_ERRORMSG];
	TCHAR tszTitle[MAX_WINDOWTITLE];

	LoadString(ghinst, iStrID, tszMsg, MAX_ERRORMSG);
	LoadString(ghinst, IDS_DUEL_ERROR_TITLE, tszTitle, MAX_WINDOWTITLE);
    MessageBox( ghWndMain, tszMsg, tszTitle, MB_OK );
    return FALSE;
}

/*
 * UpdateTitle
 *
 * Updates the window title based on application status
 */
void UpdateTitle(void)
{
	DWORD dwFeatures;
  	TCHAR tszTitle[MAX_WINDOWTITLE];
	PTCHAR pSeparator, ptszRemaining;

	// calculate title features
	dwFeatures = 0;
	if (gbIsHost)
		dwFeatures |= 1;
	if (gbUseProtocol)
		dwFeatures |= 2;
	if (gbReliable)
		dwFeatures |= 4;
	if (gbAsync)
		dwFeatures |= 8;

	// get appropriate window title for these features
	LoadString(ghinst,
			   dwFeatures == 0 ? IDS_DUEL_TITLE : IDS_DUEL_OPTIONS_TITLE,
			   tszTitle, MAX_WINDOWTITLE);

	if (dwFeatures > 0)
	{	// blank out any inactive status fields from title string
		// "Duel -|Host|Protocol|Reliable|Async|"
		// and replace separators with blanks
		if (!(ptszRemaining = _tcschr(tszTitle, _T('|'))))
			goto BADSTRING;
		*ptszRemaining = _T(' ');	// replace 1st separator
		do
		{
			BOOL bInactive = (dwFeatures & 1) == 0;
			dwFeatures >>= 1;	// shift to next bit flag
			if (!(pSeparator = _tcschr(ptszRemaining, _T('|'))))
				goto BADSTRING;
			if (bInactive)
			{
				*pSeparator = 0;	// temporarily terminate substring
				_tcsset(ptszRemaining, _T(' '));
			}
			// replace separator with blank or terminate if none are left
			*pSeparator = dwFeatures > 0 ? _T(' ') : 0;
			ptszRemaining = pSeparator;
		} while (dwFeatures > 0);
	}

    // change window title
    SetWindowText(ghWndMain, tszTitle);
	return;
BADSTRING:
    SetWindowText(ghWndMain, _T("Title string resource is corrupted!"));
}

/*
 * DoHelp
 *
 * Display a Help summary in a message box.
 */
void DoHelp(void)
{
	TCHAR tszHelpMsg[MAX_HELPMSG];
	TCHAR tszTitle[MAX_WINDOWTITLE];

	LoadString(ghinst, IDS_DUEL_HELP, tszHelpMsg, MAX_HELPMSG);
	LoadString(ghinst, IDS_DUEL_TITLE, tszTitle, MAX_WINDOWTITLE);
    MessageBox(ghWndMain, tszHelpMsg, tszTitle, MB_OK );
}

