/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       main.cpp
 *
 *
 ***************************************************************************/

#define INITGUID
#include "donuts.h"

/*
 * Globals defined elsewhere
 */
extern  CMyMonitor      Monitor[9];
extern  CShip           DL;
extern  double          Dirx[40];
extern  double          Diry[40];

/*
 * Globals defined here
 */
BOOL                    bSoundEnabled   = FALSE;
BOOL                    bPlayIdle       = FALSE;
BOOL                    bPlayBuzz       = FALSE;
BOOL                    bPlayRev        = FALSE;
#ifdef USE_DSOUND
LPDIRECTSOUND           lpDS;
HSNDOBJ                 hsoBeginLevel     = NULL;
HSNDOBJ                 hsoEngineIdle     = NULL;
HSNDOBJ                 hsoEngineRev      = NULL;
HSNDOBJ                 hsoSkidToStop     = NULL;
HSNDOBJ                 hsoShieldBuzz     = NULL;
HSNDOBJ                 hsoShipExplode    = NULL;
HSNDOBJ                 hsoFireBullet     = NULL;
HSNDOBJ                 hsoShipBounce     = NULL;
HSNDOBJ                 hsoDonutExplode   = NULL;
HSNDOBJ                 hsoPyramidExplode = NULL;
HSNDOBJ                 hsoCubeExplode    = NULL;
HSNDOBJ                 hsoSphereExplode  = NULL;
#endif

HWND                    hWndMain;
HACCEL                  hAccel;
HINSTANCE               hInst;
BOOL                    bIsActive;
BOOL                    bSpecialEffects     = FALSE;
BOOL                    bWantSound          = TRUE;
BOOL                    bShowFrameCount     = TRUE;

int                     showDelay           = 0;
BOOL                    bMouseVisible;
DWORD                   lastTickCount;
int                     score;
int                     ProgramState;
int                     level;
int                     restCount;
DWORD                   ShowLevelCount      = 3000;
int                     iForceErase         = 0;
RECT                    rcVirtualDesktop;

/*
 * Use this to choose which monitor has the normal window
 * (stays -1 if there isn't one)
 */
int                     nMonitorForWindow   = -1;



/*****************************************************************
    Functions
*****************************************************************/



// the compiler should optimize this call down to nothing in retail
void DebugSpew( LPSTR sz, ... )
{
#ifdef DEBUG
    va_list vlist;
    va_start( vlist, sz );

    int     nLen = strlen(sz) + 1024;
    char*   szTemp = (char*) malloc( nLen );

    vsprintf( szTemp, sz, vlist );
    OutputDebugString( szTemp );
    OutputDebugString( "\n" );

    free( szTemp );
#endif
}


//
// FUNCTION:    WinMain
//
// DESCRIPTION: Contains main program loop
//
// NOTES:       - program starts here!
//
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
			int nCmdShow )
{
    MSG msg;
    int rc = -1;

#ifdef _DEBUG
    int memflags;
    memflags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|memflags);
#endif
    
    /*
     * Save application instance
     */
    hInst = hInstance;

    /*
     * Process any command line parameters
     */
    while (lpCmdLine[0] == '-')
    {
	lpCmdLine++;

	switch (*lpCmdLine++)
	{
	case 'S': case 's':
	    bWantSound = FALSE;
	    break;

	case 'W': case 'w':
	    nMonitorForWindow = getint( &lpCmdLine, -1 );
	    break;
	}
	while (IS_SPACE(*lpCmdLine))
	{
	    lpCmdLine++;
	}
    }

    /*
     * Initialize stuff
     */
    if (FAILED(InitApplication(hInstance, nCmdShow)))
    {
	goto Exit;
    }

    while( 1 )
    {
	if (PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE))
	{
	    if (!GetMessage( &msg, NULL, 0, 0 ))
	    {
		rc = (int) msg.wParam;
		break;
	    }
	    if (!TranslateAccelerator( hWndMain, hAccel, &msg ))
	    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	}
	else if (bIsActive)
	{
	    /*
	     * Draw the next frame
	     */
	    if (FAILED(UpdateFrame()))
	    {
		DebugSpew("UpdateFrame failed");
		goto Exit;
	    }
	}
	else
	{
	    WaitMessage();
	}
    }

Exit:
    CleanupAndExit(NULL);

    DebugSpew("End run Multinuts rc = %d", rc );

    return rc;
} /* WinMain */

											
//
// FUNCTION:    MainWndproc
//
// DESCRIPTION: Window procedure for message handling
//
// NOTES:               
//
long FAR PASCAL MainWndproc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    UINT cmd;

    switch( message )
    {
    case WM_ACTIVATEAPP:
	bIsActive = (BOOL) wParam;
	if( bIsActive )
	{
	    bMouseVisible = TRUE;
	    lastTickCount = GetTickCount();
	    bSpecialEffects = FALSE;
	    /*
	     * We are active, need to reacquire the keyboard
	     */
	    ReacquireInput();
	}
	else
	{
	    bMouseVisible = TRUE;
	    /*
	     * DirectInput automatically unacquires for us in FOREGROUND mode
	     */
	}
	break;

    case WM_SETCURSOR:
	if( !bMouseVisible )
	{
	    SetCursor(NULL);
	}
	else
	{
	    SetCursor(LoadCursor( NULL, IDC_ARROW ));
	}
	return TRUE;

    case WM_COMMAND:
	cmd = GET_WM_COMMAND_ID(wParam, lParam);

	if (cmd >= IDC_DEVICES && cmd < IDC_DEVICES + 100)
	{
	    PickInputDevice(cmd - IDC_DEVICES);
	}
	else
	{
	    switch (cmd)
	    {
	    case IDC_FRAMERATE:
		bShowFrameCount = !bShowFrameCount;
		break;

	    case IDC_STARTGAME:
		if (ProgramState == PS_SPLASH)
		{
		    ProgramState = PS_BEGINREST;
		    SetupGame();
		}
		break;

	    case IDC_QUIT:
		PostMessage( hWnd, WM_CLOSE, 0, 0 );
		return 0;

	    case IDC_AUDIO:
#ifdef USE_DSOUND
		if(bWantSound)
		{
		    if( bSoundEnabled )
		    {
			DestroySound();
		    }
		    else
		    {
			InitializeSound();
		    }
		}
#endif
		break;

	    case IDC_TRAILS:
		bSpecialEffects = !bSpecialEffects;
		break;
	    }
	    break;
	}

    case WM_INITMENU:
	CheckMenuItems(hWndMain);
	break;

    case WM_DESTROY:
	PostQuitMessage( 0 );
	break;

    case WM_ENTERMENULOOP:
	AppPause();
	break;

    case WM_EXITMENULOOP:
	AppUnpause();
	break;

    case WM_MOVE:
	AppUnpause();
	break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
} /* MainWndproc */


//
// FUNCTION:    InitApplication
//
// DESCRIPTION: Perform window creation
//
// NOTES:               
//
HRESULT InitApplication( HANDLE hInstance, int nCmdShow )
{
    HRESULT     hr;
    WNDCLASS    wc;
    BOOL        rc;
    RECT        rect;
    DWORD       dwFlags;

    /*
     * Initialize display system
     */
    hr = CMyMonitor::Initialize();
    if (FAILED(hr))
    {
	CleanupAndExit("Couldn't initialize multimonitor system", hr);
	return hr;
    }
    DebugSpew("Initialized mulitmonitor system");

    /*
     * Store the coordinates of the virtual desktop
     */
    rcVirtualDesktop.left   = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rcVirtualDesktop.right  = rcVirtualDesktop.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rcVirtualDesktop.top    = GetSystemMetrics(SM_YVIRTUALSCREEN);
    rcVirtualDesktop.bottom = rcVirtualDesktop.left + GetSystemMetrics(SM_CYVIRTUALSCREEN);

    /*
     * Register window class
     */
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc      = MainWndproc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = (HINSTANCE)hInstance;
    wc.hIcon            = LoadIcon( (HINSTANCE)hInstance, MAKEINTRESOURCE(DONUTS_ICON));
    wc.hCursor          = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground    = (HBRUSH)GetStockObject( GRAY_BRUSH );
    wc.lpszMenuName     = MAKEINTRESOURCE(DONUTS_MENU);
    wc.lpszClassName    = "DonutsClass";
    rc = RegisterClass( &wc );
    if (!rc)
    {
	CleanupAndExit("RegisterClass failed (InitApplication)");
	return DDERR_GENERIC;
    }
    DebugSpew("Registered window class");

    /*
     * Load accelerators
     */
    hAccel = LoadAccelerators( (HINSTANCE)hInstance, MAKEINTRESOURCE(DONUTS_ACCEL));
    if (!hAccel)
    {
	CleanupAndExit("LoadAccelerators failed (InitApplication)");
	return DDERR_GENERIC;
    }
    DebugSpew("Loaded accelerators");

    /*
     * Create the window
     */
    hWndMain = CreateWindowEx(
	0,
	"DonutsClass",
	"Donuts",
	WS_POPUP |   // non-app window
	WS_CAPTION | // so our menu doesn't look ultra-goofy
	WS_SYSMENU,  // so we get an icon in the tray
	0,
	0,
	100,
	100,
	NULL,
	NULL,
	(HINSTANCE)hInstance,
	NULL );

    if (!hWndMain)
    {
	CleanupAndExit("CreateWindowEx failed (InitApplication)");
	return DDERR_GENERIC;
    }
    DebugSpew("Created window");

    if (0 == CMonitor::iNumberOfMonitors)
    {
    	CleanupAndExit("This application requires multiple monitors");
    	return DDERR_GENERIC;
    }

    /*
     * Calculate window size
     */
    if (nMonitorForWindow == -1)
    {
	memcpy(&rect,Monitor[0].lpMonitorRect,sizeof(RECT));
    }
    else
    {
	CMyMonitor* pMon = &Monitor[nMonitorForWindow];

	pMon->dwWidth /= 2;
	pMon->dwHeight /= 2;

	rect.left = 0;
	rect.top = 0;
	rect.right = (int) (pMon->dwWidth * 1.25);
	rect.bottom = (int) (pMon->dwHeight * 1.25);

	/*
	 * This call will give the RECT negative coordinates
	 * in order to keep the topleft corner of the client at 0,0.
	 * So we adjust for that afterwards.
	 */
	AdjustWindowRectEx(&rect,
		GetWindowStyle(hWndMain),
		GetMenu(hWndMain) != NULL,
		GetWindowExStyle(hWndMain));

	rect.right -= rect.left;
	rect.left = 0;
	rect.bottom -= rect.top;
	rect.top = 0;

	/*
	 * Now add monitor offsets
	 */
	rect.left += pMon->lpMonitorRect->left;
	rect.right += pMon->lpMonitorRect->left;
	rect.top += pMon->lpMonitorRect->top;
	rect.bottom += pMon->lpMonitorRect->top;

	/*
	 * Update boundary lines
	 */
	for (int i = 0; i < pMon->iNumLines; i++)
	{
	    // adjust line coords for half screen
	    pMon->Line[i].left /= 2;
	    pMon->Line[i].right /= 2;
	    pMon->Line[i].top /= 2;
	    pMon->Line[i].bottom /= 2;
	}
    }

    SetWindowPos(hWndMain, NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
	SWP_NOZORDER | SWP_NOACTIVATE );

    ShowWindow(hWndMain, SW_SHOW);

    /*
     * Initialize DDraw
     */
    for (int i = 0; i < CMonitor::iNumberOfMonitors; i++)
    {
	if (nMonitorForWindow == i)
	{
	    dwFlags = CREATEWINDOWED;
	}
	else
	{
	    dwFlags = CREATEFULLSCREEN|ENABLEMULTIMON;

	    if (nMonitorForWindow == -1 && i == 0)
	    {
		dwFlags |= SETDEVICEWND;
	    }
	}
	    
	hr = Monitor[i].DDInit(hWndMain, dwFlags );
	if (FAILED(hr))
	{
	    CleanupAndExit( Monitor[i].szErrMsg, hr );
	    return hr;
	}
    }
    DebugSpew("DDInited monitors");
    
    /*
     * Intiailize DSound
     */
#ifdef USE_DSOUND
    if (!bWantSound)
    {
	EnableMenuItem(GetMenu(hWndMain), IDC_AUDIO, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }
    else
    {
	InitializeSound();
    }
#endif

    /*
     * Initialize DInput
     */
    if (!InitInput(hInst, hWndMain))
    {
	CleanupAndExit("DirectInput initialization failed (InitApplication)");
	return DDERR_GENERIC;
    }
    DebugSpew("Initalized DirectInput");

    score = 0;

    lastTickCount = GetTickCount();

    ProgramState = PS_SPLASH;

    return DD_OK;
}


//
// FUNCTION:    AppPause
//
// DESCRIPTION: Draw menu bar
//
// NOTES:               
//
void AppPause()
{
    DrawMenuBar(hWndMain);
    RedrawWindow(hWndMain, NULL, NULL, RDW_FRAME);
    Monitor[0].lpDD->FlipToGDISurface();
}

//
// FUNCTION:    AppUnpause
//
// DESCRIPTION: Reset the various time counters so the donuts don't suddenly
//              jump halfway across the screen and so the frame rate remains accurate.
//
// NOTES:               
//
void AppUnpause()
{
    iForceErase = 2;
    lastTickCount = GetTickCount();
}

//
// FUNCTION:    CheckOneMenuItem
//
// DESCRIPTION: Checks a single item in a menu
//
// NOTES:               
//
void CheckOneMenuItem(HMENU hmenu, UINT idc, BOOL fCheck)
{
    CheckMenuItem(hmenu, idc,
		  fCheck ? (MF_BYCOMMAND | MF_CHECKED)
			 : (MF_BYCOMMAND | MF_UNCHECKED));
}

//
// FUNCTION:    CheckMenuItems
//
// DESCRIPTION: Sync menu checkmarks with internal variables
//
// NOTES:               
//
void CheckMenuItems(HWND hwnd)
{
    HMENU hmenu = GetMenu(hwnd);

    CheckOneMenuItem(hmenu, IDC_TRAILS, bSpecialEffects);

#ifdef USE_DSOUND
    CheckOneMenuItem(hmenu, IDC_AUDIO, bWantSound && bSoundEnabled);
#endif

    CheckOneMenuItem(hmenu, IDC_FRAMERATE, bShowFrameCount);

}


//
// FUNCTION:    BltScore
//
// DESCRIPTION: Performs letter & number blitting for frame rate, score, and level ###
//
// NOTES:               
//
HRESULT BltScore( char *num, int x, int y )
{
    HRESULT hr;
    int     i;
    DWORD   dwFlag;
    RECT    src;
    RECT    dst = { x, y, x + 16, y + 16 };

    dwFlag = DDBLT_KEYSRC;

    for (char* c=num; *c != '\0'; c++)
    {
	while (1)
	{
	    i = *c - '0';
	    src.left = i*16;
	    src.top = 0;
	    src.right = src.left + 16;
	    src.bottom = src.top + 16;
	    
	    hr = Monitor[DL.iMonitor].lpBackBuffer->Blt( 
		&dst, Monitor[DL.iMonitor].lpNum, &src, dwFlag, NULL );

	    if (hr == DD_OK)
	    {
		break;
	    }
	    if (hr == DDERR_SURFACELOST)
	    {
		hr = Monitor[DL.iMonitor].RestoreSurfaces();
		if (FAILED(hr))
		{
		    CleanupAndExit( Monitor[DL.iMonitor].szErrMsg, hr );
		    return hr;
		}
	    }
	    if (hr != DDERR_WASSTILLDRAWING )
	    {
		CleanupAndExit("Blt failed (BltScore)", hr);
		return hr;
	    }
	}
	dst.left += 16;
	dst.right += 16;
    }
    return DD_OK;
}


//
// FUNCTION:    DisplayFrameRate
//
// DESCRIPTION: Blit the frame rate
//
// NOTES:               
//
HRESULT DisplayFrameRate( void )
{
    static DWORD dwFrameTime  = timeGetTime();
    static DWORD dwFrameCount = 0;
    static DWORD dwFrames     = 0;
    DWORD  dwTime2;
    char   buf[256];

    dwFrameCount++;
    dwTime2 = timeGetTime() - dwFrameTime;
    if (dwTime2 > 1000)
    {
	dwFrames = (dwFrameCount*1000)/dwTime2;
	dwFrameTime = timeGetTime();
	dwFrameCount = 0;
    }
    if (dwFrames == 0)
    {
	return DD_OK;
    }
    if (dwFrames > 99)
    {
	dwFrames = 99;
    }
    buf[0] = (char)((dwFrames / 10) + '0');
    buf[1] = (char)((dwFrames % 10) + '0');
    buf[2] = '\0';
    
    return BltScore(buf, (int)(0.02 * (double) Monitor[DL.iMonitor].dwWidth), 
	(int)(0.02 * (double) Monitor[DL.iMonitor].dwHeight) );
}


//
// FUNCTION:    DisplayLevel
//
// DESCRIPTION: Blit "Level ###"
//
// NOTES:               
//
HRESULT DisplayLevel( void )
{
    HRESULT hr;
    char buf[10];
    int  left, top;
	
    hr = EraseScreen();
    if (FAILED(hr))
    {
	return hr;
    }

    buf[0] = 10 + '0';
    buf[1] = 11 + '0';
    buf[2] = 12 + '0';
    buf[3] = 11 + '0';
    buf[4] = 10 + '0';
    buf[5] = '\0';

    top = Monitor[DL.iMonitor].dwHeight / 2 - 8;
    left = (Monitor[DL.iMonitor].dwWidth - 16 * 9) / 2;
    
    hr = BltScore( buf, left, top );
    if (FAILED(hr))
    {
	return hr;
    }

    buf[0] = level / 100 + '0';
    buf[1] = level / 10 + '0';
    buf[2] = level % 10 + '0';
    buf[3] = '\0';

    left += 16 * 6;
    
    hr = BltScore( buf, left, top );
    if (FAILED(hr))
    {
	return hr;
    }
    return FlipScreen();
}

//
// FUNCTION:    BltSplash
//
// DESCRIPTION: Display intro screen on the primary monitor
//
// NOTES:               
//
HRESULT BltSplash( void )
{
    HRESULT     hr;
    HBITMAP     hbm;
    int         iMonitor;
	
    /*
     * If we're in an 8bpp display mode we need to set the palette to get
     * colours that look right.
     */
    if (Monitor[0].ScreenBpp == 8)
    {
	while (1)
	{
	    hr = Monitor[0].lpFrontBuffer->SetPalette( Monitor[0].lpSplashPalette );
	    if (hr == DD_OK)
	    {
		break;
	    }
	    else if (hr == DDERR_SURFACELOST)
	    {
		// virtual fct. call
		hr = Monitor[0].RestoreSurfaces();
		if (FAILED(hr))
		{   
		    CleanupAndExit("RestoreSurfaces (BltSplash) failed", hr);
		    return hr;
		}
	    }
	    else 
	    {   
		CleanupAndExit("SetPalette (BltSplash) failed", hr);
		return hr;
	    }
	}
	DebugSpew("Palette set (BltSplash)");
    }
    
    hbm = (HBITMAP)LoadImage( GetModuleHandle( NULL ), "SPLASH", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
    if ( hbm == NULL )
    {
	CleanupAndExit("LoadImage failed (BltSplash)");
	return DDERR_GENERIC;
    }

    /*
     * If the surface is lost, DDCopyBitmap will fail and the surface will
     * be restored in FlipScreen.
     */
    DDCopyBitmap( Monitor[0].lpBackBuffer, hbm, 0, 0, 0, 0 );

    DeleteObject( hbm );

    /*
     * Blank out other monitors
     */
    for (iMonitor = 1; iMonitor < CMonitor::iNumberOfMonitors; iMonitor++)
    {
	hr = Monitor[iMonitor].Blank(); 
	if (FAILED(hr))
	{
	    CleanupAndExit( Monitor[iMonitor].szErrMsg, hr );
	    return hr;
	}
    }
    return FlipScreen();
}


//
// FUNCTION:    SetupGame
//
// DESCRIPTION: Perform start time initialization
//
// NOTES:               
//
HRESULT SetupGame(void)
{
    HRESULT hr;

    restCount = GetTickCount();
    InitLevel (++level);

    /*
     * If we're in an 8bpp display mode we need to set the palette to get
     * colours that look right.
     */
    for(int i=0; i< CMonitor::iNumberOfMonitors; i++)
    {
	if (Monitor[i].ScreenBpp == 8)
	{
	    while (1)
	    {
		hr = Monitor[i].lpFrontBuffer->SetPalette( Monitor[i].lpArtPalette );
		if (hr == DD_OK)
		{
		    break;
		}
		else if (hr == DDERR_SURFACELOST)
		{
		    // virtual fct. call
		    hr = Monitor[i].RestoreSurfaces();
		    if (FAILED(hr))
		    {   
			CleanupAndExit("RestoreSurfaces (SetupGame) failed", hr);
			return hr;
		    }
		}
		else 
		{   
		    CleanupAndExit("SetPalette (SetupGame) failed", hr);
		    return hr;
		}
	    }
	    DebugSpew("Palette set (SetupGame)");
	}
    }
    return DD_OK;
}

//
// FUNCTION:    EraseScreen
//
// DESCRIPTION: Fill back buffers with background color and draws a border
//
// NOTES:               
//
HRESULT EraseScreen()
{
    HRESULT hr;

    if (iForceErase > 0)
    {
	--iForceErase;
    }
    else if (bSpecialEffects)
    {
	return DD_OK;
    }

    for(int iMonitor=0;iMonitor<CMonitor::iNumberOfMonitors;iMonitor++)
    {
	hr = Monitor[iMonitor].Blank();
	if (FAILED(hr))
	{
	    CleanupAndExit( Monitor[iMonitor].szErrMsg, hr );
	    return hr;
	}
		
	hr = Monitor[iMonitor].DrawBorder();
	if (FAILED(hr))
	{
	    CleanupAndExit( Monitor[iMonitor].szErrMsg, hr );
	    return hr;
	}
    }
    return DD_OK;
}

//
// FUNCTION:    FlipScreen
//
// DESCRIPTION: Flip back buffers to front buffers
//
// NOTES:               
//
HRESULT FlipScreen()
{
    HRESULT hr;

    for(int iMonitor=0;iMonitor<CMonitor::iNumberOfMonitors;iMonitor++)
    {
	hr = Monitor[iMonitor].Flip();
	if (FAILED(hr))
	{
	    CleanupAndExit( Monitor[iMonitor].szErrMsg, hr );
	    return hr;
	}
    }
    return DD_OK;
}


//
// FUNCTION:    InitLevel
//
// DESCRIPTION: Perform new level initialization
//
// NOTES:               
//
void InitLevel( int level )
{
    double  startx, starty;

    /*
     * Clear any stray bullets out of the display list
     */
    while( DL.next != &DL )
    {
	DL.next->DeleteFromList();
    }

    /* 
     * Add some donuts
     */
    for (int i=0; i<(2*level-1); i++)
    {
	CObject* pObj = new CDonut;

	startx = randDouble( Monitor[DL.iMonitor].lpMonitorRect->left,
	    Monitor[DL.iMonitor].lpMonitorRect->right - pObj->km_iSize );

	starty = randDouble( Monitor[DL.iMonitor].lpMonitorRect->top,
	    Monitor[DL.iMonitor].lpMonitorRect->bottom - pObj->km_iSize );
		
	// virtual fct call
	pObj->Init( startx, starty );
    }
    
    /*
     * Initialize the ship
     */
    DL.Init();
    showDelay = DEF_SHOW_DELAY;
}
	

//
// FUNCTION:    UpdateFrame
//
// DESCRIPTION: Draw the next frame
//
// NOTES:       - depends on program state
//              - called from main program loop when no messages to process
//
HRESULT UpdateFrame( void )
{
    HRESULT hr;

    switch( ProgramState )
    {
    case PS_SPLASH:
	// display the splash screen
	return BltSplash();

    case PS_ACTIVE:
	UpdateDisplayList();
	CheckForHits();
	hr = DrawDisplayList();
	if (FAILED(hr))
	{
	    DebugSpew("DrawDisplayList failed");
	    return hr;
	}

	// check if display list is empty
	if ( DL.next == &DL && DL.prev == &DL )
	{
#ifdef USE_DSOUND
	    if(bWantSound)
	    {
		SndObjStop(hsoEngineIdle);
		SndObjStop(hsoEngineRev);
	    }
#endif
	    bPlayIdle = FALSE;
	    bPlayRev = FALSE;
	    ProgramState = PS_BEGINREST;
	    restCount = GetTickCount();
	    InitLevel( ++level );
	}
	break;

    case PS_BEGINREST:
#ifdef USE_DSOUND
	if(bWantSound)
	{
	    SndObjPlay(hsoBeginLevel, 0);
	}
#endif
	ProgramState = PS_REST;
	//
	// FALLTHRU
	//
    case PS_REST:
	if( ( GetTickCount() - restCount ) > ShowLevelCount )
	{
#ifdef USE_DSOUND
	    if(bWantSound)
	    {
		SndObjPlay(hsoEngineIdle, DSBPLAY_LOOPING);
	    }
#endif
	    bPlayIdle = TRUE;
	    lastTickCount = GetTickCount();
	    ProgramState = PS_ACTIVE;
	}
	else
	{
	    hr = DisplayLevel();
	    if (FAILED(hr))
	    {
		return hr;
	    }
	}
	break;
    }
    return DD_OK;
}


//
// FUNCTION:    UpdateDisplayList
//
// DESCRIPTION: Add and remove objects from the display list
//
// NOTES:       - reads user input
//              - adjust position, speed, and animation frame of objects
//
void UpdateDisplayList()
{
    static BOOL lastThrust = FALSE;
    CObject     *cur, *save;
    DWORD       input;
    DWORD       thisTickCount = GetTickCount();
    DWORD       tickDiff = thisTickCount - lastTickCount;
    BOOL        event = FALSE;
	
    lastTickCount = thisTickCount;

    input = ReadGameInput();

    if (showDelay)
    {
	showDelay -= (int)tickDiff;
	if (showDelay < 0)
	{
	    showDelay = 0;
	    DL.Init();
	}
    }

    /*
     * Check if we should play some sounds
     */
    event = DL.CheckPosition( tickDiff );

    if (event)
    {
#ifdef USE_DSOUND
	if(bWantSound)
	{
	    PlayPanned(hsoShipBounce, DL.posx);
	}
#endif
	event = FALSE;
    }

    if ((event = (showDelay || ((input & KEY_SHIELD) == KEY_SHIELD))) != DL.bShieldsOn)
    {
	if (event && !showDelay)
	{
#ifdef USE_DSOUND
	    if(bWantSound)
	    {
		SndObjPlay(hsoShieldBuzz, DSBPLAY_LOOPING);
	    }
#endif
	    bPlayBuzz = TRUE;
	}
	else
	{
#ifdef USE_DSOUND
	    if(bWantSound)
	    {
		SndObjStop(hsoShieldBuzz);
	    }
#endif
	    bPlayBuzz = FALSE;
	}
	DL.bShieldsOn = event;
    }
    if (event)
    {
	input &= ~(KEY_FIRE);
    }

    if (input & KEY_FIRE)
    {
	if( !showDelay )
	{
	    /*
	     * Add a bullet to the scene
	     */
#ifdef USE_DSOUND
	    if(bWantSound)
	    {
		SndObjPlay(hsoFireBullet, 0);
	    }
#endif
	    CObject* pObj = new CBullet;
	    // virtual fct. call
	    pObj->Init( Dirx[(int)DL.frame]*6.0 + 16.0 + DL.posx, Diry[(int)DL.frame]*6.0 + 16.0 + DL.posy );
	    pObj->velx = Dirx[(int)DL.frame]*500.0/1000.0;
	    pObj->vely = Diry[(int)DL.frame]*500.0/1000.0;

	    --score;
	    if(score < 0)
		score = 0;
	}
    }

    /*
     * Update the ship's frame depending on input
     */
    event = FALSE;
    if( input & KEY_LEFT )
    {
	DL.frame -= 1.0;
	if( DL.frame < 0.0 )
	    DL.frame += DL.km_iMaxFrame;
    }
    if( input & KEY_RIGHT )
    {
	DL.frame += 1.0;
	if( DL.frame >= DL.km_iMaxFrame )
	    DL.frame -= DL.km_iMaxFrame;
    }
    if( input & KEY_UP )
    {
	DL.velx += Dirx[(int)DL.frame] * 10.0/1000.0;
	DL.vely += Diry[(int)DL.frame] * 10.0/1000.0;
	event = TRUE;
    }
    if( input & KEY_DOWN )
    {
	DL.velx -= Dirx[(int)DL.frame] * 10.0/1000.0;
	DL.vely -= Diry[(int)DL.frame] * 10.0/1000.0;
	event = TRUE;
    }

    if (event != lastThrust)
    {
	if (event)
	{
	    input &= ~KEY_STOP;
#ifdef USE_DSOUND
	    if(bWantSound)
	    {
		SndObjStop(hsoSkidToStop);
		SndObjPlay(hsoEngineRev, DSBPLAY_LOOPING);
	    }
#endif
	    bPlayRev = TRUE;
	}
	else
	{
#ifdef USE_DSOUND
	    if(bWantSound)
	    {
		SndObjStop(hsoEngineRev);
	    }
#endif
	    bPlayRev = FALSE;
	}
	lastThrust = event;
    }

    if( input & KEY_STOP )
    {
#ifdef USE_DSOUND
	if(bWantSound)
	{
	    if (DL.velx || DL.vely)
		PlayPanned(hsoSkidToStop, DL.posx);
	}
#endif
	DL.velx = 0;
	DL.vely = 0;
    }

    /*
     * Update all object positions
     */
    cur = DL.next;
    do
    {
	cur->CheckPosition( tickDiff ); 
	cur = cur->next;
    }
    while( cur != &DL );

    /*
     * Update all object's current frame
     */
    cur = DL.next;
    do
    {
	/*
	 * Virtual fct. call. If returns TRUE we need to remove
	 * the object from the display list.
	 */
	if (cur->CheckFrame())
	{
	    save = cur;
	    cur = cur->next;
	    save->DeleteFromList();
	}
	else
	{
	    cur = cur->next;
	}
    }
    while( cur != &DL );
}


//
// FUNCTION:    CheckForHits
//
// DESCRIPTION: Handle object collisions
//
// NOTES:       - updates object bounding boxes
//              - look for intersections of bullet-like objects with target-like objects
//
void CheckForHits()
{
    CObject     *bullet, *target, *save;
    int         x, y;
    BOOL        bRemoveBullet;

    /*
     * Update boundary rectangles
     */
    target = &DL;
    do
    {
	target->UpdateRect();
	target = target->next;
    }
    while (target != &DL);

    /*
     * First we traverse the list of objects looking for intersections of
     * bullet-like objects with target-like objects. When an intersection is
     * found the target is marked as hit and the bullet is removed from the list.
     * Then we traverse the list again and explode each marked object.
     */
    bullet=&DL;
    do
    {
	bRemoveBullet = FALSE;

	if( !bullet->CanBeBullet() )
	{
	    bullet = bullet->next;
	    continue;
	}

	/*
	 * If our 'bullet' is the ship with shields on, ignore it
	 */
	if (bullet == &DL && DL.bShieldsOn)
	{
	    bullet = bullet->next;
	    continue;
	}

	/*
	 * Calculate our collision "hot spot"
	 */
	x = (bullet->dst.left + bullet->dst.right) / 2;
	y = (bullet->dst.top + bullet->dst.bottom) / 2;

	/*
	 * Walk through the list to see if this 'bullet' hit anything.
	 */
	for(target = DL.next; target != &DL; target = target->next)
	{
	    if( !target->CanBeTarget() )
		continue;

	    if( (x >= target->dst.left) && (x < target->dst.right) &&
		(y >= target->dst.top)  && (y < target->dst.bottom) )
	    {
		target->Hit();
		DebugSpew("target Hit");

		if (bullet == &DL && !showDelay)
		{
		    DL.Hit();
		    DebugSpew("ship hit");
		}
		else
		{
		    bRemoveBullet = TRUE;
		}
		// TODO: each bullet can only hit one target?
		break;
	    }
	} //for

	/*
	 * If a bullet hit something, get rid of the bullet
	 */
	if (bRemoveBullet)
	{
	    // get rid of the bullet
	    save = bullet;
	    bullet = bullet->next;

	    save->DeleteFromList();
	    DebugSpew("bullet deleted");
	}
	else
	{
	    bullet = bullet->next;
	}
    } while (bullet != &DL);

    /*
     * Walk through the list again and explode any hit objects
     */
    target = DL.next;
    do
    { 
	if (target->IsHit())
	{
	    score += target->Explode();
	    save = target;
	    target = target->next;

	    save->DeleteFromList();
	}
	else
	{
	    target=target->next;
	}
    } while (target != &DL);

    /*
     * If the ship was it, explode it too.
     */
    if (DL.IsHit())
    {
	score += DL.Explode();
	DebugSpew("ship exploded");
	if (score < 0)
	    score = 0;
	showDelay = DEF_SHOW_DELAY;
    }            
}


//
// FUNCTION:    DrawDisplayList
//
// DESCRIPTION: Blits everything the the screen
//
// NOTES:
//
HRESULT DrawDisplayList( void )
{
    HRESULT     hr;
    CObject     *cur, *last;
    char        scorebuf[11];
    int         rem;

    /*
     * Calculate score string
     */
    scorebuf[0] = score/10000000 + '0';
    rem = score % 10000000;
    scorebuf[1] = rem/1000000 + '0';
    rem = score % 1000000;
    scorebuf[2] = rem/100000 + '0';
    rem = score % 100000;
    scorebuf[3] = rem/10000 + '0';
    rem = score % 10000;
    scorebuf[4] = rem/1000 + '0';
    rem = score % 1000;
    scorebuf[5] = rem/100 + '0';
    rem = score % 100;
    scorebuf[6] = rem/10 + '0';
    rem = score % 10;
    scorebuf[7] = rem + '0';

#ifdef USE_DSOUND
    if( bSoundEnabled )
    {
	scorebuf[8] = 14 + '0';
	scorebuf[9] = 13 + '0';
	scorebuf[10] = '\0';
    }
    else
#endif
    {
	scorebuf[8] = '\0';
    }

    /*
     * Wipe the background
     */
    hr = EraseScreen();
    if (FAILED(hr))
    {
	return hr;
    }

    /*
     * Now blt the objects in the display list
     */
    cur = &DL;
    last = &DL;

    if (showDelay)
    {
	// don't show the ship
	cur = cur->prev;
    }

    do
    {
	hr = cur->Blt();
	if (FAILED(hr))
	{
	    DebugSpew("Object blt failed");
	    return hr;
	}
	cur = cur->prev;
    }
    while (cur != last);

    /*
     * Blt the score
     */
    hr = BltScore(scorebuf, (int)(0.10 * Monitor[DL.iMonitor].dwWidth), (int)(0.90 * Monitor[DL.iMonitor].dwHeight));
    if (FAILED(hr))
    {
	return hr;
    }

    /*
     * Blt the frame rate
     */
    if (bShowFrameCount)
    {
	hr = DisplayFrameRate();
	if (FAILED(hr))
	{
	    return hr;
	}
    }
    return FlipScreen();
}



//
// FUNCTION:    CleanupAndExit
//
// DESCRIPTION: Shut everything down and display a box if an error occured
//
// NOTES:
//
void CleanupAndExit( char *err )
{
    CObject *cur, *temp;
    static bAlreadyCalled = FALSE;

    DebugSpew("CleanupAndExit: %s", err );

    if (!bAlreadyCalled)
    {
	bAlreadyCalled = TRUE;

	// make the cursor visible
	SetCursor(LoadCursor( NULL, IDC_ARROW ));
	bMouseVisible = TRUE;

	/*
	 * Release display list
	 */
	for (cur = DL.next; cur != &DL; )
	{
	    temp = cur->next;
	    delete cur;
	    cur = temp;
	}
    
	/*
	 * Release display system
	 */
	CMyMonitor::Uninit();
	DebugSpew("Uninitialized monitors");

	/*
	 * Clean up DirectInput objects
	 */
	CleanupInput();
	DebugSpew("Cleaned up DirectInput");

	/*
	 * Warn user
	 */
	
	if (err) 
	{
	    MessageBox( hWndMain, err, "ERROR", MB_OK );
	    DebugSpew("Back from MessageBox");
	}
	if (IsWindow(hWndMain))
	{
	    DestroyWindow(hWndMain);
	    DebugSpew("Called DestroyWindow");
	}                
	
    }    
    DebugSpew("End of CleanupAndExit");
}

void CleanupAndExit( char* err, HRESULT ddrval )
{
    char* szText;

    if (!err)
    {
	szText = new char[256];
	wsprintf( szText, "Err: 0x%08x", ddrval );
    }
    else
    {
	szText = new char[strlen(err) + 256];
	wsprintf( szText, "%s (0x%08x)", err, ddrval );
    }
    CleanupAndExit( szText );

    delete [] szText;
}

#ifdef USE_DSOUND

//
// FUNCTION:    PlayPanned
//
// DESCRIPTION: Play a sound with left-right relative volume adjusted depending on location
//              within the virtual desktop.
//
// NOTES:
//
void PlayPanned(HSNDOBJ hSO, double posx )
{
    static int nMiddle = (rcVirtualDesktop.left + rcVirtualDesktop.right) / 2;
    static int nWidth = rcVirtualDesktop.right - rcVirtualDesktop.left;

    if(!bWantSound)
	return;

    IDirectSoundBuffer *pDSB = SndObjGetFreeBuffer(hSO);

    if (pDSB)
    {
	LONG pos = (LONG)((posx - nMiddle) / (double) nWidth * 20000.0);
	pDSB->SetPan(pos);
	pDSB->Play( 0, 0, 0);
    }
}

//
// FUNCTION:    InitializeSound
//
// DESCRIPTION: Initialize DirectSound objects
//
// NOTES:
//
void InitializeSound( void )
{
    if(!bWantSound)
	return;

    bSoundEnabled = FALSE;

    if (SUCCEEDED(DirectSoundCreate(NULL, &lpDS, NULL)))
    {
	if (SUCCEEDED(lpDS->SetCooperativeLevel(hWndMain, DSSCL_NORMAL)))
	{
	    hsoBeginLevel     = SndObjCreate(lpDS, "BeginLevel",      1);
	    hsoEngineIdle     = SndObjCreate(lpDS, "EngineIdle",      1);
	    hsoEngineRev      = SndObjCreate(lpDS, "EngineRev",       1);
	    hsoSkidToStop     = SndObjCreate(lpDS, "SkidToStop",      1);
	    hsoShieldBuzz     = SndObjCreate(lpDS, "ShieldBuzz",      1);
	    hsoShipExplode    = SndObjCreate(lpDS, "ShipExplode",     1);
	    hsoFireBullet     = SndObjCreate(lpDS, "Gunfire",        25);
	    hsoShipBounce     = SndObjCreate(lpDS, "ShipBounce",      4);
	    hsoDonutExplode   = SndObjCreate(lpDS, "DonutExplode",   10);
	    hsoPyramidExplode = SndObjCreate(lpDS, "PyramidExplode", 12);
	    hsoCubeExplode    = SndObjCreate(lpDS, "CubeExplode",    15);
	    hsoSphereExplode  = SndObjCreate(lpDS, "SphereExplode",  10);
	    bSoundEnabled = TRUE;

	    if( bPlayIdle )
		SndObjPlay(hsoEngineIdle, DSBPLAY_LOOPING);

	    if( bPlayBuzz )
		SndObjPlay(hsoShieldBuzz, DSBPLAY_LOOPING);

	    if( bPlayRev )
		SndObjPlay(hsoEngineRev, DSBPLAY_LOOPING);
	}
	else
	{
	    lpDS->Release();
	    lpDS = NULL;
	}
    }
}

//
// FUNCTION:    DestroySound
//
// DESCRIPTION: Release DirectSound objects
//
// NOTES:
//
void DestroySound( void )
{
    if(!bWantSound)
	return;

    bSoundEnabled = FALSE;
    if (lpDS)
    {
	SndObjDestroy(hsoBeginLevel);
	hsoBeginLevel = NULL;
	SndObjDestroy(hsoEngineIdle);
	hsoEngineIdle = NULL;
	SndObjDestroy(hsoEngineRev);
	hsoEngineRev = NULL;
	SndObjDestroy(hsoSkidToStop);
	hsoSkidToStop = NULL;
	SndObjDestroy(hsoShieldBuzz);
	hsoShieldBuzz = NULL;
	SndObjDestroy(hsoShipExplode);
	hsoShipExplode = NULL;
	SndObjDestroy(hsoFireBullet);
	hsoFireBullet = NULL;
	SndObjDestroy(hsoShipBounce);
	hsoShipBounce = NULL;
	SndObjDestroy(hsoDonutExplode);
	hsoDonutExplode = NULL;
	SndObjDestroy(hsoPyramidExplode);
	hsoPyramidExplode = NULL;
	SndObjDestroy(hsoCubeExplode);
	hsoCubeExplode = NULL;
	SndObjDestroy(hsoSphereExplode);
	hsoSphereExplode = NULL;

	lpDS->Release();
	lpDS = NULL;
    }
}
#endif

