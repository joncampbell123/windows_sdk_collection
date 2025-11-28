/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       monitor.h
 *
 *
 ***************************************************************************/


#ifndef MONITOR_H
#define MONITOR_H

#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "multimon.h"	// make sure to include this before ddraw.h for multimon
#include <ddraw.h>

#include "resource.h"
#include "ddutil.h"

//
// The CMonitor class encapsulates basic functionality
// for programming graphical display output.
// It is multimonitor aware and DirectDraw enabled.
//
// The static Initialize routine assumes the existance
// of a global array of CMonitor-based objects, and employs the new
// DirectDrawEnumerateEx fct. to set up descriptive data structures.
// 
// CMonitor also supports a bare-bones DirectDraw framework:
// an IDirectDraw2 interface and an IDirectDrawSurface primary
// surface with one back buffer as a flipping chain.
// The three surface-related functions (DDInit, RestoreSurfaces,
// and Release) are virtual to allow a derived class to support any
// number of additional surfaces while remaining compatible with the
// generic Blank and Flip routines.
//

#define MAX_MONITOR                     9

/*
 * Flags for DDInit
 */
#define CREATEWINDOWED                  1
#define CREATEFULLSCREEN                2
#define ENABLEMULTIMON                  4
#define SETDEVICEWND                    8

class CMonitor
{
public:
    CMonitor();
    ~CMonitor();

    static int	iNumberOfMonitors;

    // Initializes the common data of global Monitor array
    static HRESULT  Initialize();

    // Uninitializes all monitors in the global Monitor array
    static void	    Uninit();
    
    // Basic DirectDraw initialization
    virtual HRESULT DDInit( HWND hWnd, DWORD dwFlags );

    // Restores the primary surface
    virtual HRESULT RestoreSurfaces();

    // Releases the primary surface and DirectDraw object
    virtual void    Release();

    // Fills the back buffer with the specified color
    HRESULT	    Blank( DWORD dwFillColor );

    // Flips the back buffer to the front buffer
    HRESULT	    Flip();
    
    // descriptive data structures
    HMONITOR	    hMonitor;
    GUID*	    lpGUID;
    MONITORINFOEX   MonitorInfo;
    LPRECT	    lpMonitorRect;
    DWORD	    dwWidth;
    DWORD	    dwHeight;
    DWORD	    ScreenBpp;
    BOOL	    bFullscreen;
    HWND	    hWnd;

    // use this to return detailed error messages
    char	    szErrMsg[256];

    // DDraw interface pointers
    LPDIRECTDRAW4          lpDD;
    LPDIRECTDRAWSURFACE4   lpFrontBuffer;
    LPDIRECTDRAWSURFACE4   lpBackBuffer;
};


//
// The CMyMonitor derives from CMonitor and builds
// upon that foundation to implement additional graphical
// display functionality specific to this application.
//
// The Initialize routine now calculates the monitor boundary
// locations where there is monitor adjacency as defined in
// the Display/Settings tab of the Control Panel, and DrawBorder
// displays a white line around the monitor edges except at
// these points of adjacency.
//
// DDInit, RestoreSurfaces, and Release have been extended to
// support all the off-screen sprite bitmaps.
//
class CMyMonitor : public CMonitor
{
public:
    CMyMonitor();

    static HRESULT  Initialize();
    static void	    Uninit();

    virtual HRESULT DDInit( HWND hwnd, DWORD dwFlags );
    virtual HRESULT RestoreSurfaces();
    virtual void    Release();

    HRESULT	    Blank();
    HRESULT	    DrawBorder();

    // types
    enum { LEFT, RIGHT, TOP, BOTTOM };

    typedef struct 
    {
        RGNDATAHEADER hdr;
        RECT rgndata[4];
    } CLIPLIST, *LPCLIPLIST;

    typedef struct 
    {
	int left, top, right, bottom;
	int type;
    } LINE, *LPLINE;        

    // data
    int		    iNumLines;
    LINE            Line[9];

    DWORD           dwFillColor;
    DWORD           dwLineColor;

    // more DDraw objects
    CLIPLIST		    ClipList;
    LPDIRECTDRAWSURFACE4    lpDonut;
    LPDIRECTDRAWSURFACE4    lpPyramid;
    LPDIRECTDRAWSURFACE4    lpCube;
    LPDIRECTDRAWSURFACE4    lpSphere;
    LPDIRECTDRAWSURFACE4    lpShip;
    LPDIRECTDRAWSURFACE4    lpNum;
    LPDIRECTDRAWSURFACE4    lpBackground;
    LPDIRECTDRAWPALETTE     lpArtPalette;
    LPDIRECTDRAWPALETTE     lpSplashPalette;
    LPDIRECTDRAWCLIPPER     lpDDClipper;
};

#endif
