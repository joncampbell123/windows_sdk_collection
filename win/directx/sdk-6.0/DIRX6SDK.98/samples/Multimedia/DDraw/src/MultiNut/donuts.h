/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       donuts.h
 *  Content:    main include file
 *
 *
 ***************************************************************************/

#ifndef DONUTS_INCLUDED
#define DONUTS_INCLUDED

#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include "monitor.h"
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <mmsystem.h>
#include <ddraw.h>

#ifdef USE_DSOUND
#include <dsound.h>
#endif

#include "resource.h"
#include "ddutil.h"
#include "input.h"
#include "util.h"

#ifdef USE_DSOUND
#include "dsutil.h"
#endif

#define DEF_SHOW_DELAY     (2000)

/*
 * Program states
 */
enum { PS_SPLASH, PS_ACTIVE, PS_BEGINREST, PS_REST };

/*
 * Class definitions
 */
class CObject
{
public:
    CObject( int nMaxFrame, int nSize );

    // Checks for object proximity to monitor x boundary
    int     GetMonitorX();

    // Checks for object proximity to monitor y boundary
    int     GetMonitorY();

    // Computes new dst and src rects
    void    UpdateRect();

    // Blts the object's bitmap to the appropriate back buffer
    HRESULT Blt();

    // Identifies location on virtual desktop (which monitor)
    BOOL    CheckPosition( DWORD tickDiff );

    // Removes this object from the display list
    void    DeleteFromList();

    // Tag this object as having been hit
    void    Hit()               { bHit = TRUE; };

    // Check if this object has been hit
    BOOL    IsHit()             { return bHit; };
    
    /*
     * Virtual fcts
     */

    // Initializes object data, links it to the display list
    virtual void Init( double x, double y );

    // Checks for frame overflow
    virtual BOOL CheckFrame();

    // Updates position and frame
    virtual void UpdateFrame( DWORD tickDiff );

    // Check if this object is considered a 'bullet'
    virtual BOOL CanBeBullet()  { return FALSE; };

    // Check if this object is considered a 'target'
    virtual BOOL CanBeTarget()  { return FALSE; };
    
    // pure virtual - must be implemented by derived classes
    virtual void UpdateSrcRect() = 0;
    virtual int  Explode() = 0;
    virtual LPDIRECTDRAWSURFACE4 GetSurface() = 0;

    /*
     * Constants
     */
    int km_iMaxFrame;
    int km_iSize;

    /*
     * Data
     */
    CObject*	next;
    CObject*	prev;
    double      posx, posy; // actual x and y position
    double      velx, vely; // x and y velocity (pixels/millisecond)
    double      frame;      // current frame
    double      delay;   
    RECT        src, dst;
    int		iMonitor;
    BOOL        bHit;
};



class CDonut : public CObject
{
public:
    CDonut();
    
    virtual void UpdateSrcRect();
    virtual LPDIRECTDRAWSURFACE4 GetSurface();
    virtual int  Explode();
    
    virtual void Init( double x, double y );
    virtual BOOL CanBeTarget() { return TRUE; };
};



class CPyramid : public CObject
{
public:
    CPyramid();

    virtual void UpdateSrcRect();
    virtual LPDIRECTDRAWSURFACE4 GetSurface();
    virtual int  Explode();

    virtual void Init( double x, double y );
    virtual BOOL CanBeTarget() { return TRUE; };
};



class CCube : public CObject
{
public:
    CCube();

    virtual void UpdateSrcRect();
    virtual LPDIRECTDRAWSURFACE4 GetSurface();
    virtual int  Explode();

    virtual void Init( double x, double y );
    virtual BOOL CanBeTarget() { return TRUE; };
};



class CSphere : public CObject
{
public:
    CSphere();

    virtual void UpdateSrcRect();
    virtual LPDIRECTDRAWSURFACE4 GetSurface();
    virtual int  Explode();

    virtual void Init( double x, double y );
    virtual BOOL CanBeTarget() { return TRUE; };
};



class CShip : public CObject
{
public:
    CShip();

    virtual void UpdateSrcRect();
    virtual LPDIRECTDRAWSURFACE4 GetSurface();
    virtual int  Explode();

    virtual void Init();
    virtual void UpdateFrame( DWORD tickDiff );
    virtual BOOL CanBeBullet() { return TRUE; };

    BOOL    bShieldsOn;
};



class CBullet : public CObject
{
    enum { BULLET_X = 304, BULLET_Y = 0 };

public:
    CBullet();

    virtual void UpdateSrcRect();
    virtual LPDIRECTDRAWSURFACE4 GetSurface();
    virtual int  Explode();

    virtual void Init( double x, double y );
    virtual BOOL CheckFrame();
    virtual BOOL CanBeBullet() { return TRUE; };
};


/*
 * Function prototypes
 */
void	AppPause();
void	AppUnpause();
void	CheckOneMenuItem(HMENU hmenu, UINT idc, BOOL fCheck);
void	CheckMenuItems(HWND hwnd);

HRESULT InitApplication(HANDLE hInstance, int nCmdShow);
HRESULT SetupGame();
void	InitLevel(int level);

HRESULT UpdateFrame();
void	UpdateDisplayList();
void	CheckForHits();
HRESULT DrawDisplayList();

HRESULT DisplayFrameRate();
HRESULT	DisplayLevel();
HRESULT BltScore(char *num, int x, int y);
HRESULT BltSplash();
HRESULT EraseScreen();
HRESULT FlipScreen();
void    CleanupAndExit(char *err);
void	CleanupAndExit(char *err, HRESULT ddrval);

#ifdef USE_DSOUND
void	PlayPanned(HSNDOBJ hSO, double posx);
void	InitializeSound();
void	DestroySound();
#endif

#endif

