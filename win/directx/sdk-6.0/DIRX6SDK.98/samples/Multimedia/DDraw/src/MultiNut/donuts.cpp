/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       donuts.cpp
 *
 *
 ***************************************************************************/

#include "donuts.h"

/*
 * Globals defined here.
 *
 * The display list is a linked list of CObject-derived objects
 * that are currently in play. The list is anchored with the 
 * player's ship which always exists.
 */
CShip	DL;         // display list

/*
 * Globals defined elsewhere
 */
extern	CMyMonitor	Monitor[MAX_MONITOR];
extern	BOOL		bWantSound;

#ifdef USE_DSOUND
extern HSNDOBJ		hsoDonutExplode;
extern HSNDOBJ		hsoPyramidExplode;
extern HSNDOBJ		hsoCubeExplode;
extern HSNDOBJ		hsoSphereExplode;
extern HSNDOBJ		hsoShipExplode;
#endif


/*
 *
 * CObject
 *
 */

// constructor
CObject::CObject( int nMaxFrame, int nSize )
{
    /*
     * Constants
     */
    km_iMaxFrame = nMaxFrame;
    km_iSize = nSize;

    /*
     * Defaults
     */
    next = prev = this;
    posx = posy = velx = vely = frame = delay = 0.0;
    iMonitor = 0;
}

//
// CLASS:	CObject
//
// FUNCTION:	Init
//
// DESCRIPTION:	Initializes object data, links it to the display list
//
// NOTES:		
//
void CObject::Init( double x, double y )
{
    iMonitor = DL.iMonitor;
    posx = x;
    posy = y;
    frame = randDouble( 0, 30 );

    /*
     * Link the object to the display list
     */
    next = DL.next;
    prev = &DL;
    DL.next->prev = this;
    DL.next = this;
    bHit = FALSE;

    UpdateRect();
}

//
// CLASS:	CObject
//
// FUNCTION:	DeleteFromList
//
// DESCRIPTION:	Removes this object from the display list
//
// NOTES:		
//
void CObject::DeleteFromList()
{
    next->prev = prev;
    prev->next = next;
    delete this;
}

//
// CLASS:	CObject
//
// FUNCTION:	UpdateRect
//
// DESCRIPTION:	Computes new dst and src rects
//
// NOTES:	- src rect calculation performed by pure virtual fct UpdateSrcRect
//
void CObject::UpdateRect()
{
    dst.left = (DWORD)posx;
    dst.top = (DWORD)posy;
    dst.right = dst.left + km_iSize;
    dst.bottom = dst.top + km_iSize;

    // pure virtual fct call
    UpdateSrcRect();
}


//
// CLASS:	CObject
//
// FUNCTION:	Blt
//
// DESCRIPTION:	Blts the object's bitmap to the back buffer
//
// NOTES:	- surface bitmap determined by pure virtual fct GetSurface()
//
HRESULT CObject::Blt()
{
    RECT	rc;
    HRESULT	ddrval;

    rc.top = dst.top - Monitor[iMonitor].lpMonitorRect->top;
    rc.left = dst.left - Monitor[iMonitor].lpMonitorRect->left;

    if (!Monitor[iMonitor].bFullscreen)
    {
	rc.top /= 2;
	rc.left /= 2;
    }

    rc.bottom = rc.top + (src.bottom - src.top);
    rc.right = rc.left + (src.right - src.left);
    
    while (1)
    {
        ddrval = Monitor[iMonitor].lpBackBuffer->Blt( &rc, GetSurface(),
            &src, DDBLT_KEYSRC, NULL );
	    
        if( ddrval == DD_OK )
        {
            break;
        }
        else if( ddrval == DDERR_SURFACELOST )
        {
            ddrval = Monitor[iMonitor].RestoreSurfaces();
            if (FAILED(ddrval))
	    {
                CleanupAndExit( Monitor[iMonitor].szErrMsg, ddrval );
	        return ddrval;
	    }
        }
        if (ddrval != DDERR_WASSTILLDRAWING)
        {
            CleanupAndExit("Blt failed (CObject::Blt)", ddrval );
            return ddrval;
        }
    }
    return DD_OK;
}

//
// CLASS:	CObject
//
// FUNCTION:	UpdateFrame
//
// DESCRIPTION:	Updates position and frame
//
// NOTES:	- virtual fct
//
void CObject::UpdateFrame( DWORD tickDiff )
{
    posx += velx * (double)tickDiff;
    posy += vely * (double)tickDiff;
    frame += delay * (double)tickDiff;
}

//
// CLASS:	CObject
//
// FUNCTION:	CheckPosition
//
// DESCRIPTION:	Identifies location on virtual desktop (which monitor)
//
// NOTES:	- returns TRUE if the object has bounced off a wall
//
BOOL CObject::CheckPosition( DWORD tickDiff )
{
    int	    iNewMonitor;
    double  maxx, minx, maxy, miny, maxframe;
    BOOL    event;

    // virtual fct call
    UpdateFrame( tickDiff );

    maxx = (double) Monitor[iMonitor].lpMonitorRect->right - km_iSize;
    minx = (double) Monitor[iMonitor].lpMonitorRect->left;
    maxy = (double) Monitor[iMonitor].lpMonitorRect->bottom - km_iSize;
    miny = (double) Monitor[iMonitor].lpMonitorRect->top;

    maxframe = (double)km_iMaxFrame;

    if( velx > 0 && posx > maxx )
    {
	iNewMonitor = GetMonitorX();
	if ( iNewMonitor == iMonitor )
	{
	    // bounce off the walls
	    posx = maxx;
	    velx = -velx;
	    event = TRUE;
	}
	else
	{
	    iMonitor = iNewMonitor;
	}
    }
    else if ( velx < 0 && posx < minx )
    {
	iNewMonitor = GetMonitorX();
	if ( iNewMonitor == iMonitor )
	{
	    posx = minx;
	    velx = -velx;
	    event = TRUE;
	}
	else
	{
	    iMonitor = iNewMonitor;
	}
    }
    else if( vely > 0 && posy > maxy )
    {
	iNewMonitor = GetMonitorY();
	if ( iNewMonitor == iMonitor )
	{
	    posy = maxy;
	    vely = -vely;
	    event = TRUE;
	}
	else
	{
    	    iMonitor = iNewMonitor;
	}
    }
    else if ( vely < 0 && posy < miny )
    {
	iNewMonitor = GetMonitorY();
	if ( iNewMonitor == iMonitor )
	{
	    posy = miny;
	    vely = -vely;
	    event = TRUE;
        }
	else
	{
	    iMonitor = iNewMonitor;
	}
    }

    return event;
}

//
// CLASS:	CObject
//
// FUNCTION:	CheckFrame
//
// DESCRIPTION:	Checks for frame overflow
//
// NOTES:	- returns TRUE if the object is to be removed from the display list
//		- in this implementation, FALSE is always returned
//
BOOL CObject::CheckFrame()
{
    if (frame >= km_iMaxFrame)
    {
	frame -= km_iMaxFrame;
    }
    return FALSE;
}

//
// CLASS:	CObject
//
// FUNCTION:	GetMonitorX
//
// DESCRIPTION:	Checks for object proximity to monitor x boundary
//
// NOTES:	- returns the monitor on which the object is now located
//
int CObject::GetMonitorX()
{
    POINT pt;

    for (int i = 0; i < CMonitor::iNumberOfMonitors; i++)
    {
	// don't check the monitor we're on
	if ( i == iMonitor )
	    continue;
	
	pt.y = (long) posy;
	pt.x = (long) posx - km_iSize - 1;
			
	if ( PtInRect( Monitor[i].lpMonitorRect, pt ))
	{
	    return i;
	}
	pt.x = (long) posx + km_iSize + 1;
	if ( PtInRect(Monitor[i].lpMonitorRect, pt ))
	{
	    return i;
	}
    }
    // if we get here, not close to any monitors
    return iMonitor;
}

//
// CLASS:	CObject
//
// FUNCTION:	GetMonitorY
//
// DESCRIPTION:	Checks for object proximity to monitor y boundary
//
// NOTES:	- returns the monitor on which the object is now located
//
int CObject::GetMonitorY()
{
    int i;
    POINT pt;

    for (i = 0; i < CMonitor::iNumberOfMonitors; i++)
    {
	// don't check the monitor we're on
	if ( i == iMonitor )
	    continue;

	pt.x = (long) posx;
	pt.y = (long) posy - km_iSize - 1;

	if ( PtInRect(Monitor[i].lpMonitorRect, pt ))
	{
	    return i;
	}
	pt.y = (long) posy + km_iSize + 1;
	if ( PtInRect(Monitor[i].lpMonitorRect, pt ))
	{
	    return i;
	}
    }
    // if we get here, not close to any monitors
    return iMonitor;
}

/*
 *
 * CDonut
 *
 */

// constructor
CDonut::CDonut() :
    CObject (30, 64)
{
}

//
// CLASS:	CDonut
//
// FUNCTION:	Init
//
// DESCRIPTION:	Initializes donut data
//
// NOTES:	- calls CObject::Init
//
void CDonut::Init( double x, double y )
{
    CObject::Init( x, y );
    velx = randDouble( -50.0/1000.0, 50.0/1000.0 );
    vely = randDouble( -50.0/1000.0, 50.0/1000.0 );
    delay = 30.0*randDouble( 0.1, 0.4 )/1000.0;
}

//
// CLASS:	CDonut
//
// FUNCTION:	UpdateSrcRect
//
// DESCRIPTION:	Updates the donut src rect
//
// NOTES:		
//
void CDonut::UpdateSrcRect()
{
    DWORD dwframe = (DWORD) frame;
    src.left = km_iSize * (dwframe % 5);
    src.top = km_iSize * (dwframe /5);
    src.right = src.left + km_iSize;
    src.bottom = src.top + km_iSize;
}

//
// CLASS:	CDonut
//
// FUNCTION:	GetSurface
//
// DESCRIPTION:	Returns the DDraw surface to be used for blitting
//
// NOTES:		
//
LPDIRECTDRAWSURFACE4 CDonut::GetSurface()
{
    return Monitor[iMonitor].lpDonut;
}

//
// CLASS:	CDonut
//
// FUNCTION:	Explode
//
// DESCRIPTION:	Adds objects to the display list and returns the score bonus
//
// NOTES:		
//
int CDonut::Explode()
{
#ifdef USE_DSOUND
    if(bWantSound)
    {
	PlayPanned(hsoDonutExplode, posx);
    }
#endif
    CObject* pObj;

    pObj = new CPyramid;
    pObj->Init(dst.left, dst.top);
    
    pObj = new CPyramid;
    pObj->Init(dst.left, dst.top);

    pObj = new CPyramid;
    pObj->Init(dst.left, dst.top);

    return 10;
}


/*
 *
 * CPyramid
 *
 */

// constructor
CPyramid::CPyramid() :
    CObject (40, 32)
{
}

//
// CLASS:	CPyramid
//
// FUNCTION:	Init
//
// DESCRIPTION:	Initializes pyramid data
//
// NOTES:	- calls CObject::Init
//
void CPyramid::Init( double x, double y )
{
    CObject::Init( x, y );

    velx = 1.5*randDouble( -50.0/1000.0, 50.0/1000.0 );
    vely = 1.5*randDouble( -50.0/1000.0, 50.0/1000.0 );
    delay = 40.0*randDouble( 0.3, 1.0 )/1000.0;
}

//
// CLASS:	CPyramid
//
// FUNCTION:	UpdateSrcRect
//
// DESCRIPTION:	Updates the pyramid src rect
//
// NOTES:		
//
void CPyramid::UpdateSrcRect()
{
    DWORD dwFrame = (DWORD) frame;

    src.left = km_iSize * (dwFrame % 10);
    src.top = km_iSize * (dwFrame /10);
    src.right = src.left + km_iSize;
    src.bottom = src.top + km_iSize;
}

//
// CLASS:	CPyramid
//
// FUNCTION:	GetSurface
//
// DESCRIPTION:	Returns the DDraw surface to be used for blitting
//
// NOTES:		
//
LPDIRECTDRAWSURFACE4 CPyramid::GetSurface()
{
    return Monitor[iMonitor].lpPyramid;
}

//
// CLASS:	CPyramid
//
// FUNCTION:	Explode
//
// DESCRIPTION:	Adds objects to the display list and returns the score bonus
//
// NOTES:		
//
int CPyramid::Explode()
{
#ifdef USE_DSOUND
    if(bWantSound)
    {
        PlayPanned(hsoPyramidExplode, posx);
    }
#endif
    
    CObject* pObj = new CSphere;
    pObj->Init( dst.left, dst.top);

    pObj = new CCube;
    pObj->Init( dst.left, dst.top );

    pObj = new CCube;
    pObj->Init( dst.left, dst.top );
    
    return 20;
}

/*
 *
 * CCube
 *
 */

// constructor
CCube::CCube() :
    CObject (40, 16)
{
}


//
// CLASS:	CCube
//
// FUNCTION:	Init
//
// DESCRIPTION:	Initializes cube data
//
// NOTES:	- calls CObject::Init
//
void CCube::Init( double x, double y )
{
    CObject::Init( x, y );

    velx = 4.0*randDouble( -50.0/1000.0, 50.0/1000.0 );
    vely = 4.0*randDouble( -50.0/1000.0, 50.0/1000.0 );
    delay = 40.0*randDouble( 0.8, 2.0 )/1000.0;
}

//
// CLASS:	CCube
//
// FUNCTION:	UpdateSrcRect
//
// DESCRIPTION:	Updates the cube src rect
//
// NOTES:		
//
void CCube::UpdateSrcRect()
{
    DWORD dwFrame = (DWORD) frame;

    src.left = km_iSize * (dwFrame % 20);
    src.top = km_iSize * (dwFrame /20);
    src.right = src.left + km_iSize;
    src.bottom = src.top + km_iSize;
}


//
// CLASS:	CCube
//
// FUNCTION:	GetSurface
//
// DESCRIPTION:	Returns the DDraw surface to be used for blitting
//
// NOTES:		
//
LPDIRECTDRAWSURFACE4 CCube::GetSurface()
{
    return Monitor[iMonitor].lpCube;
}


//
// CLASS:	CCube
//
// FUNCTION:	Explode
//
// DESCRIPTION:	Adds objects to the display list and returns the score bonus
//
// NOTES:		
//
int CCube::Explode()
{
#ifdef USE_DSOUND
    if(bWantSound)
    {
        PlayPanned(hsoCubeExplode, posx);
    }
#endif

    CObject* pObj = new CSphere;
    pObj->Init( dst.left, dst.top );

    pObj = new CSphere;
    pObj->Init( dst.left, dst.top );

    return 40;
}


/*
 *
 * CSphere
 *
 */

// constructor
CSphere::CSphere() :
    CObject (40, 16)
{
}

//
// CLASS:	CSphere
//
// FUNCTION:	Init
//
// DESCRIPTION:	Initializes sphere data
//
// NOTES:	- calls CObject::Init
//
void CSphere::Init( double x, double y )
{
    CObject::Init( x, y );

    velx = 3.0*randDouble( -50.0/1000.0, 50.0/1000.0 );
    vely = 3.0*randDouble( -50.0/1000.0, 50.0/1000.0 );
    delay = 40.0*randDouble( 1.5, 2.0 )/1000.0;
}

//
// CLASS:	CSphere
//
// FUNCTION:	UpdateSrcRect
//
// DESCRIPTION:	Updates the sphere src rect
//
// NOTES:		
//
void CSphere::UpdateSrcRect()
{
    DWORD dwFrame = (DWORD) frame;
    src.left = km_iSize * (dwFrame % 20);
    src.top = km_iSize * (dwFrame /20);
    src.right = src.left + km_iSize;
    src.bottom = src.top + km_iSize;
}


//
// CLASS:	CSphere
//
// FUNCTION:	GetSurface
//
// DESCRIPTION:	Returns the DDraw surface to be used for blitting
//
// NOTES:		
//
LPDIRECTDRAWSURFACE4 CSphere::GetSurface()
{
    return Monitor[iMonitor].lpSphere;
}

//
// CLASS:	CSphere
//
// FUNCTION:	Explode
//
// DESCRIPTION:	Adds objects to the display list and returns the score bonus
//
// NOTES:		
//
int CSphere::Explode()
{
#ifdef USE_DSOUND
    if(bWantSound)
    {
        PlayPanned(hsoSphereExplode, posx);
    }
#endif
    return 20;
}


/*
 *
 * CShip
 *
 */

// constructor
CShip::CShip() :
    CObject (40, 32)
{
}

//
// CLASS:	CShip
//
// FUNCTION:	Init
//
// DESCRIPTION:	Initializes ship data
//
// NOTES:		
//
void CShip::Init()
{
    LPRECT lprect = Monitor[iMonitor].lpMonitorRect;

    // center the ship
    posx = lprect->left + (double)(lprect->right - lprect->left - km_iSize) /2;
    posy = lprect->top  + (double)(lprect->bottom - lprect->top - km_iSize) /2;
    frame = 0.0;
    velx = vely = 0.0;	// not moving
    bHit = FALSE;
    bShieldsOn = FALSE;
}

//
// CLASS:	CShip
//
// FUNCTION:	UpdateSrcRect
//
// DESCRIPTION:	Updates the ship src rect
//
// NOTES:		
//
void CShip::UpdateSrcRect()
{
    DWORD dwFrame = (DWORD) frame;

    if (bShieldsOn)
    {
        src.top = km_iSize * (dwFrame / 10) + 128;
    }
    else
    {
        src.top = km_iSize * (dwFrame /10);
    }
    src.left = km_iSize * (dwFrame % 10);
    src.right = src.left + km_iSize;
    src.bottom = src.top + km_iSize;
}

//
// CLASS:	CShip
//
// FUNCTION:	GetSurface
//
// DESCRIPTION:	Returns the DDraw surface to be used for blitting
//
// NOTES:		
//
LPDIRECTDRAWSURFACE4 CShip::GetSurface()
{
    return Monitor[iMonitor].lpShip;
}

//
// CLASS:	CShip
//
// FUNCTION:	Explode
//
// DESCRIPTION:	Adds objects to the display list and returns the score bonus
//
// NOTES:		
//
int CShip::Explode()
{
#ifdef USE_DSOUND
    if(bWantSound)
    {
        PlayPanned(hsoShipExplode, posx);
    }
#endif
    CObject* pObj;

    for (int i = 0; i < 4; i++)
    {
	pObj = new CSphere;
	pObj->Init( dst.left, dst.top );
    }

    for (i = 0; i < 10; i++)
    {
	pObj = new CBullet;
	pObj->Init( dst.left, dst.top );
	pObj->velx = randDouble( -0.5, 0.5 );
	pObj->vely = randDouble( -0.5, 0.5 );
    }
    Init();

    return -150;
}

//
// CLASS:	CShip
//
// FUNCTION:	UpdateFrame
//
// DESCRIPTION:	Updates ship position
//
// NOTES:	- unlike other objects, frame is not time-dependent
//
void CShip::UpdateFrame( DWORD tickDiff )
{
    posx += velx * (double)tickDiff;
    posy += vely * (double)tickDiff;
    // don't change the frame based on time
}



/*
 *
 * CBullet
 *
 */

// constructor
CBullet::CBullet() :
    CObject (400, 3)
{
}

//
// CLASS:	CBullet
//
// FUNCTION:	Init
//
// DESCRIPTION:	Initializes bullet data
//
// NOTES:		
//
void CBullet::Init( double x, double y )
{
    CObject::Init( x, y );

    frame = 0.0;
    delay = 1.0;
}



//
// CLASS:	CBullet
//
// FUNCTION:	UpdateSrcRect
//
// DESCRIPTION:	Updates the bullet src rect
//
// NOTES:		
//
void CBullet::UpdateSrcRect()
{
    DWORD dwFrame = (DWORD)frame/20 % 4;

    src.left = BULLET_X + dwFrame*4;
    src.top = BULLET_Y;
    src.right = src.left + km_iSize;
    src.bottom = src.top + km_iSize;
}

//
// CLASS:	CBullet
//
// FUNCTION:	GetSurface
//
// DESCRIPTION:	Returns the DDraw surface to be used for blitting
//
// NOTES:		
//
LPDIRECTDRAWSURFACE4 CBullet::GetSurface()
{
    return Monitor[iMonitor].lpNum;
}


//
// CLASS:	CBullet
//
// FUNCTION:	Explode
//
// DESCRIPTION:	Never called, bullets can't be hit
//
// NOTES:	- an implementation must be provided because Hit is declared
//		  pure virtual in the base class
//
int CBullet::Explode()
{
    return 0;
}

//
// CLASS:	CBullet
//
// FUNCTION:	CheckFrame
//
// DESCRIPTION:	Checks for frame overflow
//
// NOTES:	- returns TRUE if the bullet has expired
//
BOOL CBullet::CheckFrame()
{
    return (frame >= km_iMaxFrame);
}


