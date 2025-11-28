/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       monitor.cpp
 *
 *
 ***************************************************************************/


#define COMPILE_MULTIMON_STUBS
#include "monitor.h"
#include "util.h"

/*
 * Globals defined here
 */
CMyMonitor Monitor[MAX_MONITOR];

/*
 * Globals defined elsewhere
 */
extern void DebugSpew(LPSTR,...);

/*
 *
 * CMonitor
 *
 */

/* 
 * Static class member initialization
 */
int CMonitor::iNumberOfMonitors = 0;

// constructor
CMonitor::CMonitor() :
    lpMonitorRect( NULL ),
    hMonitor( (HMONITOR)INVALID_HANDLE_VALUE ),
    dwWidth(0),
    dwHeight(0),
    ScreenBpp(0),
    lpDD(NULL),
    lpFrontBuffer(NULL),
    lpBackBuffer(NULL),
    bFullscreen( FALSE ),
    hWnd( (HWND)INVALID_HANDLE_VALUE )
{
    szErrMsg[0] = '\0';
}

// destructor
CMonitor::~CMonitor()
{
    Release();
    if (lpGUID)
    {
	delete lpGUID;
    }
}


//
// FUNCTION:    EnumDeviceCallback
//
// DESCRIPTION: Initializes common data of consecutive elements in the global Monitor array
//
// NOTES:       - callback fct for DirectDrawEnumerateEx
//
BOOL CALLBACK EnumDeviceCallback( GUID* lpGUID, LPSTR szName, LPSTR szDevice, LPVOID lParam, HMONITOR hMonitor )
{
    DebugSpew("EnumDeviceCallback: %s", szDevice);

    if (hMonitor != NULL && CMonitor::iNumberOfMonitors < MAX_MONITOR)
    {
	CMonitor& curMonitor = Monitor[CMonitor::iNumberOfMonitors];

	curMonitor.hMonitor = hMonitor;

	if ( lpGUID == NULL )
	{
	    curMonitor.lpGUID = NULL;
	}
	else
	{
	    curMonitor.lpGUID = new GUID;
	    memcpy( curMonitor.lpGUID, lpGUID, sizeof(GUID) );
	}

	curMonitor.MonitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo( hMonitor, (LPMONITORINFOEX)(&curMonitor.MonitorInfo) );

	curMonitor.lpMonitorRect = &curMonitor.MonitorInfo.rcMonitor;

	curMonitor.dwWidth = curMonitor.lpMonitorRect->right - curMonitor.lpMonitorRect->left;
	curMonitor.dwHeight = curMonitor.lpMonitorRect->bottom - curMonitor.lpMonitorRect->top;

	++CMonitor::iNumberOfMonitors;

	DebugSpew("monitor %d: left=%d top=%d right=%d bottom=%d",
		CMonitor::iNumberOfMonitors,
		curMonitor.lpMonitorRect->left,
		curMonitor.lpMonitorRect->top,
		curMonitor.lpMonitorRect->right,
		curMonitor.lpMonitorRect->bottom );
    }
    return TRUE;
}


//
// CLASS:       CMonitor
//
// FUNCTION:    Initialize
//
// DESCRIPTION: Initializes the common data of global Monitor array
//
// NOTES:       - all the work is done in EnumMonitorCallback
//              - static function
//
HRESULT CMonitor::Initialize()
{
    HMODULE hModule;
    LPDIRECTDRAWENUMERATEEX lpDDEnum;

    hModule = GetModuleHandle( "ddraw.dll" );
    lpDDEnum = (LPDIRECTDRAWENUMERATEEX) GetProcAddress(hModule, "DirectDrawEnumerateExA");

    if (lpDDEnum == NULL)
    {
	DebugSpew("Couldn't find DirectDrawEnumerateEx");
	return DDERR_GENERIC;
    }
    else
    {
	lpDDEnum(EnumDeviceCallback, (LPVOID) NULL, DDENUM_ATTACHEDSECONDARYDEVICES);
    }
    return DD_OK;
}


//
// CLASS:       CMonitor
//
// FUNCTION:    DDInit
//
// DESCRIPTION: Basic DirectDraw initialization for this monitor
//
// NOTES:       - creates DirectDraw object and a primary surface with one back buffer
//              - uses new SetCooperativeLevel flags for multimon fullscreen exclusive
//
HRESULT CMonitor::DDInit( HWND hWindow, DWORD dwFlags )
{
    HRESULT         ddrval;
    DDSCAPS2        ddscaps;
    DDSURFACEDESC2  ddsd;
    LPDIRECTDRAW    lpDDTemp;
    DWORD           dwTotal, dwFree;

    if (dwFlags & CREATEWINDOWED)
    {
	if (dwFlags & CREATEFULLSCREEN)
	{
	    wsprintf(szErrMsg, "Cannot specify CREATEWINDOWED and CREATEFULLSCREEN");
	    return DDERR_INVALIDPARAMS;
	}
	if (dwFlags & SETDEVICEWND)
	{
	    wsprintf(szErrMsg, "Cannot specify CREATEWINDOWED and SETDEVICEWND");
	    return DDERR_INVALIDPARAMS;
	}
    }

    bFullscreen = (dwFlags & CREATEFULLSCREEN);
    hWnd = hWindow;

    /*
     * DirectDrawCreate
     */
    DirectDrawCreate( lpGUID, &lpDDTemp, NULL);
    if( lpDDTemp == NULL)
    {
	wsprintf( szErrMsg, "DirectDrawCreate failed");
	return DDERR_GENERIC;
    }
    DebugSpew("DirectDrawCreate succeeded");
    
    /*
     * QueryInterface
     */
    ddrval = lpDDTemp->QueryInterface(IID_IDirectDraw4, (void**) &lpDD);
    lpDDTemp->Release();
    if(FAILED(ddrval))
    {
	wsprintf( szErrMsg, "QueryInterface failed");
	return ddrval;
    }
    DebugSpew("QueryInterface returned IDirectDraw2");

    /*
     * SetCooperativeLevel
     */
    if (bFullscreen)
    {
	if (dwFlags & ENABLEMULTIMON)
	{
	    if (dwFlags & SETDEVICEWND)
	    {
		/*
		 * The window passed is our device window
		 */
		ddrval = lpDD->SetCooperativeLevel( hWnd, DDSCL_SETFOCUSWINDOW );
		if (FAILED(ddrval))
		{
		    wsprintf( szErrMsg, "SetCooperativeLevel failed");
		    return ddrval;
		}

		ddrval = lpDD->SetCooperativeLevel( hWnd, 
			DDSCL_SETDEVICEWINDOW | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
	    }
	    else
	    {
		/*
		 * We must create our own device window
		 */
		ddrval = lpDD->SetCooperativeLevel( hWnd,
			DDSCL_SETFOCUSWINDOW | 
			DDSCL_EXCLUSIVE | 
			DDSCL_FULLSCREEN | 
			DDSCL_CREATEDEVICEWINDOW);
	    }
	}
	else
	{
	    /*
	     * Regular fullscreen
	     */
	    ddrval = lpDD->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
	}
    }
    else
    {
	/*
	 * Regular window
	 */
	ddrval = lpDD->SetCooperativeLevel( NULL, DDSCL_NORMAL );
    }

    if(FAILED(ddrval))
    {
	wsprintf( szErrMsg, "SetCooperativeLevel failed");
	return ddrval;
    }
    DebugSpew("SetCooperativeLevel set");

    /*
     * Create surfaces
     */
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    if (bFullscreen)
    {
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;
    }
    else
    {
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    }

    ddrval = lpDD->CreateSurface( &ddsd, &lpFrontBuffer, NULL );
    if(FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateSurface FrontBuffer failed");
	return ddrval;
    }
    DebugSpew("Created primary surface");

    if (bFullscreen)
    {
	/*
	 * Get a pointer to the back buffer
	 */
	ZeroMemory(&ddscaps,sizeof(ddscaps));
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

	ddrval = lpFrontBuffer->GetAttachedSurface( &ddscaps, &lpBackBuffer );
    }
    else
    {
	/*
	 * Create the backbuffer separately
	 */
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = dwWidth;
	ddsd.dwHeight = dwHeight;

	ddrval = lpDD->CreateSurface( &ddsd, &lpBackBuffer, NULL );
    }
    if(FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't get back buffer failed");
	return ddrval;
    }

    ddrval = lpBackBuffer->GetSurfaceDesc(&ddsd);
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "GetSurfaceDesc failed (backbuffer)");
	return ddrval;
    }

    ScreenBpp = ddsd.ddpfPixelFormat.dwRGBBitCount;

    DebugSpew("**********************************************************");

    if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
    {
	DebugSpew("Back buffer is in VIDEO memory");
    }
    else if (ddsd.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
    {
	DebugSpew("Back buffer is in SYSTEM memory");
    }

    DebugSpew("Created back buffer");

    /*
     * Video mem check
     */
    ddscaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

    ddrval = lpDD->GetAvailableVidMem(&ddscaps,&dwTotal,&dwFree);
    if (FAILED(ddrval))
    {
	DebugSpew("GetAvailableVidMem failed (0x%08x)", ddrval );

	DDCAPS ddcaps;
	ZeroMemory(&ddcaps,sizeof(ddcaps));
	ddcaps.dwSize=sizeof(ddcaps);
	
	ddrval = lpDD->GetCaps(&ddcaps,NULL);
	if (FAILED(ddrval))
	{
	    DebugSpew("GetCaps failed (0x%08x)", ddrval);
	}
	else
	{
	    if (ddcaps.dwCaps & DDCAPS_NOHARDWARE)
	    {
		DebugSpew("GetCaps says I have NO hardware!");
	    }
	    else
	    {
		DebugSpew("GetCaps says I have some hardware");
	    }
	}
    }
    else
    {
	DebugSpew("Video Memory: Total = %d, Free = %d", dwTotal, dwFree );
    }

    DebugSpew("**********************************************************");

    return DD_OK;
}

//
// CLASS:       CMonitor
//
// FUNCTION:    RestoreSurfaces
//
// DESCRIPTION: Restores the primary surface
//
// NOTES:               
//
HRESULT CMonitor::RestoreSurfaces()
{
    HRESULT ddrval;

    /*
     * Restore back buffer
     */
    if (!bFullscreen)
    {
	ddrval = lpBackBuffer->Restore();
	if (FAILED(ddrval))
	{
	    wsprintf( szErrMsg, "Couldn't restore back buffer" );
	    return ddrval;
	}
	DebugSpew("Back buffer restored");
    }

    /*
     * Restore front buffer
     */
    ddrval = lpFrontBuffer->Restore();
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't restore front buffer" );
	return ddrval;
    }
    DebugSpew("Front buffer restored");

    return DD_OK;
}


//
// CLASS:       CMonitor
//
// FUNCTION:    Blank
//
// DESCRIPTION: Fills the back buffer with the specified color
//
// NOTES:               
//
HRESULT CMonitor::Blank( DWORD dwFillColor )
{
    HRESULT ddrval;
    DDBLTFX ddbltfx;
    
    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = dwFillColor;

    while (1)
    {
	ddrval = lpBackBuffer->Blt( NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );

	if (ddrval == DD_OK)
	{
	    break;
	}
	else if (ddrval == DDERR_SURFACELOST)
	{
	    // virtual fct call
	    ddrval = RestoreSurfaces();
	    if ( ddrval != DD_OK )
	    {
		// error message already set
		return ddrval;
	    }
	}
	else if (ddrval != DDERR_WASSTILLDRAWING)
	{
	    wsprintf( szErrMsg, "Blt failed (CMonitor::Blank)" );
	    return ddrval;
	}
    }
    return DD_OK;
}

//
// CLASS:       CMonitor
//
// FUNCTION:    Flip
//
// DESCRIPTION: Flips the back buffer to the front buffer
//
// NOTES:               
//
HRESULT CMonitor::Flip()
{
    HRESULT ddrval;

    if (bFullscreen)
    {
	/*
	 * Flip back to front
	 */
	while (1)
	{
	    ddrval = lpFrontBuffer->Flip( NULL, 0 );

	    if (ddrval == DD_OK)
	    {
		break;
	    }
	    else if( ddrval == DDERR_SURFACELOST )
	    {
		// virtual fct call
		ddrval = RestoreSurfaces();
		if (FAILED(ddrval))
		{
		    return ddrval;
		}
	    }
	    else if (ddrval != DDERR_WASSTILLDRAWING)
	    {
		wsprintf( szErrMsg, "Blt failed (CMonitor::Blank)" );
		return ddrval;
	    }
	}
    }
    else
    {
	/*
	 * Blt back to front
	 */
	RECT rcRectSrc;
	RECT rcRectDest;
	RECT rc;
	POINT p;

	SetRect(&rcRectSrc, 0, 0, dwWidth, dwHeight);
	SetRect(&rcRectDest, 0, 0, dwWidth, dwHeight );
    
	/*
	 * We need to figure out where on the primary surface our window lives
	 */
	p.x = 0;
	p.y = 0;

	/*
	 * Get virtual desktop coords of client (0,0)
	 */
	ClientToScreen( hWnd, &p);

	/*
	 * Transform to monitor local coords
	 */
	p.x -= lpMonitorRect->left;
	p.y -= lpMonitorRect->top;

	/*
	 * Center in client rect (assumes client area is slightly bigger
	 * than display area (dwWidth x dwHeight). When we set this up we
	 * used a stretch factor of 1.25
	 */
	GetClientRect( hWnd, &rc );
	p.x += (rc.right - rc.left - dwWidth) / 2;
	p.y += (rc.bottom - rc.top - dwHeight) / 2;
	OffsetRect(&rcRectDest, p.x, p.y);
    
	while (1)
	{
	    ddrval = lpFrontBuffer->Blt( &rcRectDest, lpBackBuffer, &rcRectSrc, DDBLT_WAIT, NULL);

	    if (ddrval == DD_OK)
	    {
		break;
	    }
	    else if (ddrval == DDERR_SURFACELOST)
	    {
		// virtual fct call
		ddrval = RestoreSurfaces();
		if ( ddrval != DD_OK )
		{
		    return ddrval;
		}
	    }
	    else if( ddrval != DDERR_WASSTILLDRAWING )
	    {
		wsprintf( szErrMsg, "Blt failed (CMonitor::Blank)" );
		return ddrval;
	    }
	}
    }
    return DD_OK;
}


//
// CLASS:       CMonitor
//
// FUNCTION:    Uninit
//
// DESCRIPTION: Uninitializes all monitors in the global Monitor array
//
// NOTES:       - calls CMonitor::Release
//              - static function
//
void CMonitor::Uninit()
{
    for (int i = 0; i < iNumberOfMonitors; i++)
    {
	// virtual fct call
	Monitor[i].Release();
    }
}

//
// CLASS:       CMonitor
//
// FUNCTION:    Release
//
// DESCRIPTION: Releases the primary surface and DirectDraw object
//
// NOTES:               
//
void CMonitor::Release()
{
    /*
     * Release back buffer
     */
    if (!bFullscreen && lpBackBuffer != NULL)
    {
	lpBackBuffer->Release();
	lpBackBuffer = NULL;
	DebugSpew("Back buffer released");
    }

    /*
     * Release front buffer
     */
    if (lpFrontBuffer != NULL)
    {
	lpFrontBuffer->Release();
	lpFrontBuffer = NULL;
	DebugSpew("Front buffer released");
    }

    /*
     * Release DDraw
     */
    if (lpDD != NULL)
    {
	lpDD->Release();
	lpDD = NULL;
	DebugSpew("DDraw released");
    }
}



/*
 *
 * CMyMonitor
 *
 */

// constructor
CMyMonitor::CMyMonitor() :
    iNumLines(0),
    lpDonut(NULL),
    lpPyramid(NULL),
    lpCube(NULL),
    lpSphere(NULL),
    lpShip(NULL),
    lpNum(NULL),
    lpArtPalette(NULL),
    lpSplashPalette(NULL),
    lpDDClipper(NULL)
{}


//
// CLASS:       CMyMonitor
//
// FUNCTION:    Initialize
//
// DESCRIPTION: Initializes the global Monitor array
//
// NOTES:       - calls CMonitor::Initialize to set up common data for all monitors
//              - then calculates where monitors touch for border drawing
//
HRESULT CMyMonitor::Initialize()
{
    HRESULT ddrval;

    /*
     * Enumerate display devices
     */
    ddrval = CMonitor::Initialize();
    if (FAILED(ddrval))
    {
	return ddrval;
    }

    /*
     * Figure out where monitors touch
     */
    for(int iMonitor=0; iMonitor< iNumberOfMonitors; iMonitor++)
    {
	CMyMonitor* pMyMon = &Monitor[iMonitor];

	for (int i = 0; i < iNumberOfMonitors; i++)
	{
	    BOOL hit = FALSE;

	    if (i == iMonitor)
		continue;

	    if (pMyMon->lpMonitorRect->top == Monitor[i].lpMonitorRect->bottom)
	    {
		if ( pMyMon->lpMonitorRect->left >= Monitor[i].lpMonitorRect->right ||
		     pMyMon->lpMonitorRect->right <= Monitor[i].lpMonitorRect->left )
		{
		    continue;
		}

		pMyMon->Line[pMyMon->iNumLines].type = TOP;
		pMyMon->Line[pMyMon->iNumLines].top = pMyMon->lpMonitorRect->top;

		if ( pMyMon->lpMonitorRect->left > Monitor[i].lpMonitorRect->left )
		{
		    pMyMon->Line[pMyMon->iNumLines].left = pMyMon->lpMonitorRect->left;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].left = Monitor[i].lpMonitorRect->left;
		}
		if ( pMyMon->lpMonitorRect->right <= Monitor[i].lpMonitorRect->right )
		{
		    pMyMon->Line[pMyMon->iNumLines].right = pMyMon->lpMonitorRect->right - 1;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].right = Monitor[i].lpMonitorRect->right;
		}
		hit = TRUE;
	    }
		
	    if (pMyMon->lpMonitorRect->bottom == Monitor[i].lpMonitorRect->top)
	    {
		if ( pMyMon->lpMonitorRect->left >= Monitor[i].lpMonitorRect->right ||
		     pMyMon->lpMonitorRect->right <= Monitor[i].lpMonitorRect->left )
		{
		    continue;
		}

		pMyMon->Line[pMyMon->iNumLines].type = BOTTOM;
		pMyMon->Line[pMyMon->iNumLines].bottom = pMyMon->lpMonitorRect->bottom - 1;

		if ( pMyMon->lpMonitorRect->left > Monitor[i].lpMonitorRect->left )
		{
		    pMyMon->Line[pMyMon->iNumLines].left = pMyMon->lpMonitorRect->left;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].left = Monitor[i].lpMonitorRect->left;
		}
		if ( pMyMon->lpMonitorRect->right <= Monitor[i].lpMonitorRect->right )
		{
		    pMyMon->Line[pMyMon->iNumLines].right = pMyMon->lpMonitorRect->right - 1;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].right = Monitor[i].lpMonitorRect->right;
		}
		hit = TRUE;
	    }

	    if (pMyMon->lpMonitorRect->left == Monitor[i].lpMonitorRect->right)
	    {
		if ( pMyMon->lpMonitorRect->top >= Monitor[i].lpMonitorRect->bottom ||
		     pMyMon->lpMonitorRect->bottom <= Monitor[i].lpMonitorRect->top )
		{
		    continue;
		}

		pMyMon->Line[pMyMon->iNumLines].type = LEFT;
		pMyMon->Line[pMyMon->iNumLines].left = pMyMon->lpMonitorRect->left;

		if ( pMyMon->lpMonitorRect->bottom <= Monitor[i].lpMonitorRect->bottom )
		{
		    pMyMon->Line[pMyMon->iNumLines].bottom = pMyMon->lpMonitorRect->bottom - 1;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].bottom = Monitor[i].lpMonitorRect->bottom;
		}
		if ( pMyMon->lpMonitorRect->top > Monitor[i].lpMonitorRect->top )
		{
		    pMyMon->Line[pMyMon->iNumLines].top = pMyMon->lpMonitorRect->top;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].top = Monitor[i].lpMonitorRect->top;
		}
		hit = TRUE;
	    }
		
	    if (pMyMon->lpMonitorRect->right == Monitor[i].lpMonitorRect->left)
	    {
		if ( pMyMon->lpMonitorRect->top >= Monitor[i].lpMonitorRect->bottom ||
		     pMyMon->lpMonitorRect->bottom <= Monitor[i].lpMonitorRect->top )
		{
		    continue;
		}

		pMyMon->Line[pMyMon->iNumLines].type = RIGHT;
		pMyMon->Line[pMyMon->iNumLines].right = pMyMon->lpMonitorRect->right - 1;

		if ( pMyMon->lpMonitorRect->bottom <= Monitor[i].lpMonitorRect->bottom )
		{
		    pMyMon->Line[pMyMon->iNumLines].bottom = pMyMon->lpMonitorRect->bottom - 1;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].bottom = Monitor[i].lpMonitorRect->bottom;
		}
		if ( pMyMon->lpMonitorRect->top > Monitor[i].lpMonitorRect->top )
		{
		    pMyMon->Line[pMyMon->iNumLines].top = pMyMon->lpMonitorRect->top;
		}
		else
		{
		    pMyMon->Line[pMyMon->iNumLines].top = Monitor[i].lpMonitorRect->top;
		}
		hit = TRUE;
	    }
	    if (hit)
	    {
		pMyMon->Line[pMyMon->iNumLines].left -= pMyMon->lpMonitorRect->left;
		pMyMon->Line[pMyMon->iNumLines].top -= pMyMon->lpMonitorRect->top;
		pMyMon->Line[pMyMon->iNumLines].right -= pMyMon->lpMonitorRect->left;
		pMyMon->Line[pMyMon->iNumLines].bottom -= pMyMon->lpMonitorRect->top;
		++pMyMon->iNumLines;
	    }
	}
    }
    return DD_OK;
}

//
// CLASS:       CMyMonitor
//
// FUNCTION:    Uninit
//
// DESCRIPTION: Uninitializes the global Monitor array
//
// NOTES:       - calls CMonitor::Uninit
//
void CMyMonitor::Uninit()
{
    CMonitor::Uninit();
}

//
// CLASS:       CMyMonitor
//
// FUNCTION:    DDInit
//
// DESCRIPTION: Initializes DirectDraw objects
//
// NOTES:       - calls CMonitor::DDInit for common init
//              - creates surfaces for game graphics
//              - creates a clipper
//
HRESULT CMyMonitor::DDInit( HWND hwnd, DWORD dwFlags )
{
    DDSURFACEDESC2  ddsd;
    HRESULT         ddrval;
    RECT            rc;

    /*
     * Creates DDraw object + front and back buffers
     */
    ddrval = CMonitor::DDInit(hwnd, dwFlags);
    if (FAILED(ddrval))
    {
	return ddrval;
    }

    /*
     * Create offscreen surfaces
     */
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

    ddsd.dwWidth = 320;
    ddsd.dwHeight = 384;
    
    ddrval = lpDD->CreateSurface( &ddsd, &lpDonut, NULL );
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateSurface lpDonut failed");
	return ddrval;
    }
    DebugSpew("Donut created");

    ddsd.dwHeight = 128;
    ddrval = lpDD->CreateSurface( &ddsd, &lpPyramid, NULL );
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateSurface lpPyramid failed");
	return ddrval;
    }
    DebugSpew("Pyramid created");

    ddsd.dwHeight = 32;
    ddrval = lpDD->CreateSurface( &ddsd, &lpCube, NULL );
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateSurface lpCube failed");
	return ddrval;
    }
    DebugSpew("Cube created");

    ddsd.dwHeight = 32;
    ddrval = lpDD->CreateSurface( &ddsd, &lpSphere, NULL );
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateSurface lpShere failed");
	return ddrval;
    }
    DebugSpew("Sphere created");
    
    ddsd.dwHeight = 256;
    ddrval = lpDD->CreateSurface( &ddsd, &lpShip, NULL );
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateSurface lpShip failed");
	return ddrval;
    }
    DebugSpew("Ship created");

    ddsd.dwHeight = 16;
    ddrval = lpDD->CreateSurface( &ddsd, &lpNum, NULL );
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateSurface lpNum failed");
	return ddrval;
    }
    DebugSpew("Num created");

    /*
     * Create a clipper
     */
    ddrval = lpDD->CreateClipper((DWORD)0,&lpDDClipper,NULL);
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "CreateClipper failed");
	return ddrval;
    }
    DebugSpew("Clipper created");

    SetRect(&rc, 0, 0, dwWidth, dwHeight);

    ClipList.hdr.dwSize = sizeof(RGNDATAHEADER);
    ClipList.hdr.iType = RDH_RECTANGLES;
    ClipList.hdr.nCount = 1;
    ClipList.hdr.nRgnSize = 0;
    memcpy(&ClipList.hdr.rcBound, &rc, sizeof(RECT));
    memcpy(&ClipList.rgndata, &rc, sizeof(RECT));

    ddrval = lpDDClipper->SetClipList((LPRGNDATA)&ClipList,0);
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "SetClipList failed");
	return ddrval;
    }
    DebugSpew("ClipList set");
    
    ddrval = lpBackBuffer->SetClipper(lpDDClipper);            
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "SetClipper failed");
	return ddrval;
    }
    DebugSpew("Clipper set on back buffer");

    return RestoreSurfaces();
}

//
// CLASS:       CMyMonitor
//
// FUNCTION:    RestoreSurfaces
//
// DESCRIPTION: Initializes DirectDraw objects
//
// NOTES:       - calls CMonitor::RestoreSurfaces
//              - loads game graphics
//
HRESULT CMyMonitor::RestoreSurfaces()
{
    HRESULT ddrval;
    HBITMAP hbm;
    
    /*
     * Restore front and back buffers
     */
    ddrval = CMonitor::RestoreSurfaces();
    if (FAILED(ddrval))
    {
	return ddrval;
    }

    /*
     * Restore offscreen surfaces
     */
    ddrval = lpDonut->Restore();
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't restore donut" );
	return ddrval;
    }
    DebugSpew("Donut restored");

    ddrval = lpPyramid->Restore();
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't restore pyramid" );
	return ddrval;
    }
    DebugSpew("Pyramid restored");

    ddrval = lpCube->Restore();
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't restore cube" );
	return ddrval;
    }
    DebugSpew("Cube restored");

    ddrval = lpSphere->Restore();
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't restore sphere" );
	return ddrval;
    }
    DebugSpew("Sphere restored");

    ddrval = lpShip->Restore();
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't restore ship" );
	return ddrval;
    }
    DebugSpew("Ship restored");

    ddrval = lpNum->Restore();
    if (FAILED(ddrval))
    {
	wsprintf( szErrMsg, "Couldn't restore num" );
	return ddrval;
    }
    DebugSpew("Num restored");

    /*
     * Create palette for the splash bitmap
     */
    if (lpSplashPalette != NULL)
    {
	lpSplashPalette->Release();
	lpSplashPalette = NULL;
    }
    lpSplashPalette = DDLoadPalette( lpDD, "SPLASH" );
    if( lpSplashPalette == NULL )
    {
	wsprintf( szErrMsg, "DDLoadPalette SPLASH failed");
	return DDERR_GENERIC;
    }
    DebugSpew("SplashPalette loaded");

    /*
     * Create and set the palette for the art bitmap
     */
    if (lpArtPalette != NULL)
    {
	lpArtPalette->Release();
	lpArtPalette = NULL;
    }
    lpArtPalette = DDLoadPalette( lpDD, "DONUTS8" );
    if( lpArtPalette == NULL )
    {
	wsprintf( szErrMsg, "DDLoadPalette DONUTS8 failed");
	return DDERR_GENERIC;
    }
    DebugSpew("ArtPalette loaded");

    /*
     * Set the palette before loading the art - if we're 8bpp
     */
    if (ScreenBpp == 8)
    {
	ddrval = lpFrontBuffer->SetPalette( lpArtPalette );
	if (FAILED(ddrval))
	{
	    wsprintf( szErrMsg, "SetPalette failed (art)");
	    return ddrval;
	}
    }

    /*
     * Load art
     */
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "DONUTS8", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
    if( hbm == NULL )
    {
	wsprintf( szErrMsg, "LoadImage DONUTS8 failed");
	return DDERR_GENERIC;
    }

    ddrval = DDCopyBitmap( lpDonut, hbm, 0, 0, 320, 384 );
    if (FAILED(ddrval))
    {
	DeleteObject( hbm );
	wsprintf( szErrMsg, "DDCopyBitmap lpDonut failed");
	return ddrval;
    }
    DebugSpew("Loaded Donut");

    // NOTE: Why are we calling LoadImage again?  StretchBlt (which is
    // called in DDCopyBitmap) does not work properly when performing
    // an 8-bpp to 24- or 32-bpp blt multiple times from the same
    // bitmap.  The workaround is to call LoadImage before each
    // StretchBlt because the first StretchBlt after a LoadImage will
    // work.
    if(ScreenBpp >= 24)
    {
	DeleteObject( hbm );
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "DONUTS8", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );

	if( hbm == NULL )
	{
	    wsprintf( szErrMsg, "LoadImage DONUTS8 failed");
	    return DDERR_GENERIC;
	}
    }

    ddrval = DDCopyBitmap( lpPyramid, hbm, 0, 384, 320, 128 );
    if (FAILED(ddrval))
    {
	DeleteObject( hbm );
	wsprintf( szErrMsg, "DDCopyBitmap lpPyramid failed");
	return ddrval;
    }
    DebugSpew("Loaded pyramid");

    if(ScreenBpp >= 24)
    {
	DeleteObject( hbm );
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "DONUTS8", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );

	if( hbm == NULL )
	{
	    wsprintf( szErrMsg, "LoadImage DONUTS8 failed");
	    return DDERR_GENERIC;
	}
    }

    ddrval = DDCopyBitmap( lpSphere, hbm, 0, 512, 320, 32 );
    if (FAILED(ddrval))
    {
	DeleteObject( hbm );
	wsprintf( szErrMsg, "DDCopyBitmap lpSphere failed");
	return ddrval;
    }
    DebugSpew("Loaded sphere");

    if(ScreenBpp >= 24)
    {
	DeleteObject( hbm );
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "DONUTS8", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
	
	if( hbm == NULL )
	{
	    wsprintf( szErrMsg, "LoadImage DONUTS8 failed");
	    return DDERR_GENERIC;
	}
    }  

    ddrval = DDCopyBitmap( lpCube, hbm, 0, 544, 320, 32 );
    if (FAILED(ddrval))
    {
	DeleteObject( hbm );
	wsprintf( szErrMsg, "DDCopyBitmap lpCube failed");
	return ddrval;
    }
    DebugSpew("Loaded Cube");

    if(ScreenBpp >= 24)
    {
	DeleteObject( hbm );
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "DONUTS8", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
	
	if( hbm == NULL )
	{
	    wsprintf( szErrMsg, "LoadImage DONUTS8 failed");
	    return DDERR_GENERIC;
	}
    }

    ddrval = DDCopyBitmap( lpShip, hbm, 0, 576, 320, 256 );
    if (FAILED(ddrval))
    {
	DeleteObject( hbm );
	wsprintf( szErrMsg, "DDCopyBitmap lpShip failed");
	return ddrval;
    }
    DebugSpew("Loaded ship");

    if(ScreenBpp >= 24)
    {
	DeleteObject( hbm );
	hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "DONUTS8", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION );
	    
	if( hbm == NULL )
	{
	    wsprintf( szErrMsg, "LoadImage DONUTS8 failed");
	    return DDERR_GENERIC;
	}

    }

    ddrval = DDCopyBitmap( lpNum, hbm, 0, 832, 320, 16 );
    if (FAILED(ddrval))
    {
	DeleteObject( hbm );
	wsprintf( szErrMsg, "DDCopyBitmap lpNum failed");
	return ddrval;
    }
    DebugSpew("Loaded Num");

    DeleteObject( hbm );

    /*
     * Set colorfill colors and color keys according to bitmap contents
     */
    dwFillColor = DDColorMatch(lpDonut, CLR_INVALID);
    dwLineColor = DDColorMatch(lpBackBuffer,RGB(255,255,255));

    DDSetColorKey( lpDonut, CLR_INVALID );
    DDSetColorKey( lpPyramid, CLR_INVALID );
    DDSetColorKey( lpCube, CLR_INVALID );
    DDSetColorKey( lpSphere, CLR_INVALID );
    DDSetColorKey( lpShip, CLR_INVALID );
    DDSetColorKey( lpNum, CLR_INVALID );
    
    DebugSpew("Color keys set");

    return DD_OK;
}

//
// CLASS:       CMyMonitor
//
// FUNCTION:    Release
//
// DESCRIPTION: Releases all DirectDraw objects
//
// NOTES:       - releases off-screen surfaces
//              - calls CMonitor::Release
//
void CMyMonitor::Release()
{
    /*
     * Release offscreen surfaces
     */
    if (lpDonut)
    {
	lpDonut->Release();
	lpDonut = NULL;
    }
    if (lpPyramid)
    {
	lpPyramid->Release();
	lpPyramid = NULL;
    }
    if (lpCube)
    {
	lpCube->Release();
	lpCube = NULL;
    }
    if (lpSphere)
    {
	lpSphere->Release();
	lpSphere = NULL;
    }
    if (lpShip)
    {
	lpShip->Release();
	lpShip = NULL;
    }
    if (lpNum)
    {
	lpNum->Release();
	lpNum = NULL;
    }
    if (lpArtPalette)
    {
	lpArtPalette->Release();
	lpArtPalette = NULL;
    }
    if (lpSplashPalette)
    {
	lpSplashPalette->Release();
	lpSplashPalette = NULL;
    }
    if (lpDDClipper)
    {
	lpDDClipper->Release();
	lpDDClipper = NULL;
    }
    DebugSpew("Offscreen things released");

    /*
     * Release primary ddraw objects
     */
    CMonitor::Release();
}


//
// CLASS:       CMyMonitor
//
// FUNCTION:    DrawBorder
//
// DESCRIPTION: Draws a white border around the screen except where another monitor touches
//
// NOTES:               
//
HRESULT CMyMonitor::DrawBorder()
{
    HRESULT             ddrval;
    RECT                rect;
    DDBLTFX             ddbltfx;
    
    ddbltfx.dwSize = sizeof( ddbltfx );
    ddbltfx.dwFillColor = dwLineColor;

    /*
     * Draw a white line on each side of the monitor
     */
    for (int i = 0; i < 4; i++)
    {
	switch (i)
	{
	case 0:
	    rect.left = 0;
	    rect.top = 0;
	    rect.right = dwWidth - 1;
	    rect.bottom = 1;
	    break;

	case 1:
	    rect.left = dwWidth - 2;
	    rect.top = 0;
	    rect.right = dwWidth - 1;
	    rect.bottom = dwHeight - 1;
	    break;

	case 2:
	    rect.left = 0;
	    rect.top = dwHeight - 2;
	    rect.right = dwWidth - 1;
	    rect.bottom = dwHeight - 1;
	    break;

	case 3:
	    rect.left = 0;
	    rect.top = 0;
	    rect.right = 1;
	    rect.bottom = dwHeight - 1;
	    break;
	}
	    
	while (1)
	{
	    ddrval = lpBackBuffer->Blt( &rect, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
	    
	    if (ddrval == DD_OK)
	    {
		break;
	    }
	    else if (ddrval == DDERR_SURFACELOST)
	    {
		// virtual fct call
		ddrval = RestoreSurfaces();
		if ( ddrval != DD_OK )
		{
		    // error message already set
		    return ddrval;
		}
	    }
	    else if (ddrval != DDERR_WASSTILLDRAWING)
	    {
		wsprintf( szErrMsg, "Blt failed (CMonitor::Blank)" );
		return ddrval;
	    }
	}
    }

    /*
     * Now 'undraw' the areas where two monitors touch
     */
    ddbltfx.dwFillColor = dwFillColor;
    for (i = 0; i < iNumLines; i++)
    {
	switch (Line[i].type)
	{
	case TOP:
	    Line[i].bottom = Line[i].top + 1;
	    break;

	case LEFT:
	    Line[i].right = Line[i].left + 1;
	    break;

	case BOTTOM:
	    Line[i].top = Line[i].bottom - 1;
	    break;

	case RIGHT:
	    Line[i].left = Line[i].right - 1;
	    break;
	}
	SetRect( &rect, Line[i].left, Line[i].top, Line[i].right, Line[i].bottom );

	/*
	DebugSpew("Undraw line: left = %d top = %d right = %d bottom = %d", 
		Line[i].left, Line[i].top, Line[i].right, Line[i].bottom );
	*/
       
	while (1)
	{
	    ddrval = lpBackBuffer->Blt( &rect, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
	    
	    if (ddrval == DD_OK)
	    {
		break;
	    }
	    else if (ddrval == DDERR_SURFACELOST)
	    {
		// virtual fct call
		ddrval = RestoreSurfaces();
		if ( ddrval != DD_OK )
		{
		    // error message already set
		    return ddrval;
		}
	    }
	    else if (ddrval != DDERR_WASSTILLDRAWING)
	    {
		wsprintf( szErrMsg, "Blt failed (CMonitor::Blank)" );
		return ddrval;
	    }
	}
    }           
    return ddrval;              
}

//
// CLASS:       CMyMonitor
//
// FUNCTION:    Blank
//
// DESCRIPTION: Fills the screen with background color
//
// NOTES:       - calls CMonitor::Blank
//
HRESULT CMyMonitor::Blank()
{
    return CMonitor::Blank( dwFillColor );
}

