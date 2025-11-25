/*===========================================================================*\
|
|  File:        cgmisc.cpp
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995-1996 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#include <windows.h>
#ifdef __WATCOMC__
#include <mem.h>
#else
#include <memory.h>
#endif
#include "cgchdll.h"
#include "cgchar.h"
#include "cgtimer.h"
#include "cginput.h"
#include "cgimage.h"

char inifile [260];


HINSTANCE hInst = NULL;            // our library instance
// This is returned by the 'Ident' function, but isn't used internally at all
CGameVersionIdent version =
{
    RELEASE1_0,
    GAMEID
};

// prototypes so we can fill in the action arrays
int PlaneCreate( CGameCharacter *, CGameLevel * );
int PlaneAction( CGameCharacter *, CGameLevel * );
int PlaneDestroy( CGameCharacter *, CGameLevel * );
int PlaneCollide( CGameCharacter *, CGameCharacter *, CGameLevel * );

int CloudCreate( CGameCharacter *, CGameLevel * );
int CloudAction( CGameCharacter *, CGameLevel * );
int CloudDestroy( CGameCharacter *, CGameLevel * );
int CloudCollide( CGameCharacter *, CGameCharacter *, CGameLevel * );

// returned by 'Info' function, and isn't used either (internally)
CGameCharSequenceInfo char1seq[1] =
{
    {"plane.spr", "Fly", 15, {"plane.wav", NULL, 100, 1}}   
};

CGameCharSequenceInfo char2seq[1] =
{
    {"clouds.spr", "Cloud1", 30, {NULL, NULL, 100, 1}}  
};
CGameCharSequenceInfo char3seq[1] =
{
    {"clouds.spr", "Cloud2", 30, {NULL, NULL, 100, 1}}  
};
CGameCharSequenceInfo char4seq[1] =
{
    {"clouds.spr", "Cloud3", 30, {NULL, NULL, 100, 1}}  
};

CGameCharInfo character1 =
{
    "Plane",
    1,
    &char1seq[0],
    PlaneCreate,
    PlaneAction,
    PlaneDestroy,
    NULL,
    PlaneCollide
};

CGameCharInfo character2 =
{
    "Cloud1",
    1,
    &char2seq[0],
    CloudCreate,
    CloudAction,
    CloudDestroy,
    NULL,
    CloudCollide
};

CGameCharInfo character3 =
{
    "Cloud2",
    1,
    &char3seq[0],
    CloudCreate,
    CloudAction,
    CloudDestroy,
    NULL,
    CloudCollide
};

CGameCharInfo character4 =
{
    "Cloud3",
    1,
    &char4seq[0],
    CloudCreate,
    CloudAction,
    CloudDestroy,
    NULL,
    CloudCollide
};

// This array allows the caller to get our information directly
CGameCharInfo *characters[] =
{
    &character1,
    &character2,
    &character3,
    &character4
};

CGameInfo dllinfo =
{
    4,                 // number of characters implemented in
    // this DLL
    characters             // array of CGameCharInfo pointers
};

// EXPORTED as ordinal #1:
#ifdef __BORLANDC__
extern "C" void CALLBACK Ident( CGameVersionIdent * id )
#else
void CALLBACK Ident( CGameVersionIdent * id )
#endif
{    
    GetModuleFileName(NULL, inifile, 259);
    char *p = strrchr(inifile, '.');
    if (p)
        lstrcpy(p+1, "GAM");

    memcpy( id, &version, sizeof( version ) );
}

// EXPORTED as ordinal #2:
#ifdef __BORLANDC__
extern "C" void CALLBACK Info( CGameInfo * info )
#else
void CALLBACK Info( CGameInfo * info )
#endif
{    
    memcpy( info, &dllinfo, sizeof( dllinfo ) );
}

int     PlaneCreate( CGameCharacter *me, CGameLevel *level )
{
    int posx,posy;
    me->GetXY(&posx, &posy);
    me->SetVelocity(-32,0);
    me->MoveTo(level->GetMaxX(), posy);
    return ( ACTION_COMPLETED );
}

int     PlaneAction( CGameCharacter *me, CGameLevel *level )
{
    int posx, posy, velx, vely;
    int time = level->GetFrameTime();
    int slices = (me->mLastTime == -1) ? 1 : (time - me->mLastTime);
    me->mLastTime = time;

    me->GetVelocity(&velx, &vely);

    // remember to use sub-pixels!
    me->GetSubXY(&posx, &posy);


    posx += SUBPIXEL_DELTA(velx, slices);
    posy += SUBPIXEL_DELTA(vely, slices);

    // did we move off the screen?  If so, start over at maxx...
    if (SUB2WORLD(posx) < -level->GetMaxX())
        posx = WORLD2SUB(level->GetMaxX());

    me->SetAndMove(posx, posy);
    me->NextSprite(level->GetTimer()->Time, FALSE);

    return ( ACTION_COMPLETED );
}

int     PlaneDestroy( CGameCharacter *me, CGameLevel *level )
{
    return ( ACTION_COMPLETED );
}

int     PlaneCollide( CGameCharacter *me, CGameCharacter *other, CGameLevel *level )
{   
    return(ACTION_COMPLETED);   
}


int     CloudCreate( CGameCharacter *me, CGameLevel *level )
{
    me->SetVelocity(64 / (me->GetXParallax()+1),0);
    return ( ACTION_COMPLETED );
}

int     CloudAction( CGameCharacter *me, CGameLevel *level )
{
    int posx, posy, velx, vely;
    int time = level->GetFrameTime();
    int slices = (me->mLastTime == -1) ? 1 : (time - me->mLastTime);
    me->mLastTime = time;

    me->GetVelocity(&velx, &vely);

    // remember to use sub-pixels!
    me->GetSubXY(&posx, &posy);

    posx += SUBPIXEL_DELTA(velx, slices);

    // did we move off the screen?  If so, start over at maxx...
    if (posx > WORLD2SUB(SCREEN_WIDTH))
        posx = WORLD2SUB(-SCREEN_WIDTH);

    me->SetAndMove(posx, posy);

    return ( ACTION_COMPLETED );
}

int     CloudDestroy( CGameCharacter *me, CGameLevel *level )
{
    return ( ACTION_COMPLETED );
}

int     CloudCollide( CGameCharacter *me, CGameCharacter *other, CGameLevel *level )
{   
    return(ACTION_COMPLETED);   
}
