/*===========================================================================*\
|
|  File:        cglevel.h
|
|  Description: 
|       
|-----------------------------------------------------------------------------
|
|  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
|
|  Written by Moss Bay Engineering, Inc. under contract to Microsoft Corporation
|
\*===========================================================================*/

/**************************************************************************

    (C) Copyright 1995 Microsoft Corp.  All rights reserved.

    You have a royalty-free right to use, modify, reproduce and 
    distribute the Sample Files (and/or any modified version) in 
    any way you find useful, provided that you agree that 
    Microsoft has no warranty obligations or liability for any 
    Sample Application Files which are modified. 

    we do not recomend you base your game on IKlowns, start with one of
    the other simpler sample apps in the GDK

 **************************************************************************/

#ifndef CGLEVEL_H
#define CGLEVEL_H

#include "cgdlist.h"

// We use millisecond timing values; our velocity is specified in
// units of (pixels/256) per millisecond
// This macro converts a velocity and elapsed milliseconds into sub-pixels
#define SUBPIXEL_DELTA( vel, ms ) (vel * ms)

class CGameTimer;
class CGameInput;
class CGameScreen;
class CGameGraphic;
class CGameCharacter;

class CGameLevel
{
public:
    CGameLevel(
        char* pFileName,
        char* pLevelName,
        CGameTimer* pTimer,
        CGameInput* pInput,
        CGameScreen* pScreen
        );

    virtual ~CGameLevel();

    virtual void Animate(HWND, CGameScreen* pScreen);

    virtual int GetOffsetX()
    {
        return mOffsetX;
    }

    virtual int GetOffsetY()
    {
        return mOffsetY;
    }

    virtual int GetMaxX()
    {
        return mMaxWorldX;
    }

    virtual int GetMaxY()
    {
        return mMaxWorldY;
    }

    virtual int Screen2WorldX( int screenX )
    {
        return screenX + mOffsetX;
    };

    virtual int Screen2WorldY( int screenY )
    {
        return screenY + mOffsetY;
    };

    virtual int World2ScreenX( int worldX )
    {
        return worldX - mOffsetX;
    };

    virtual int World2ScreenY( int worldY )
    {
        return worldY - mOffsetY;
    };

    virtual int Screen2WorldX( int screenX, int parallax )
    {
        return screenX + (mOffsetX >> parallax);
    };

    virtual int Screen2WorldY( int screenY, int parallax )
    {
        return screenY + (mOffsetY >> parallax);
    };

    virtual int World2ScreenX( int worldX, int parallax )
    {
        return worldX - (mOffsetX >> parallax);
    };

    virtual int World2ScreenY( int worldY, int parallax )
    {
        return worldY - (mOffsetY >> parallax);
    };

    virtual void SetOffsetX(int newx)
    {
        if ((newx >= 0) && (newx <= mMaxWorldX))
            mOffsetX = newx;
    }

    virtual void SetOffsetY(int newy)
    {
        if ((newy >= 0) && (newy <= mMaxWorldY))
            mOffsetY = newy;
    }

    virtual void ForceOnScreen(int *x, int *y, int wide, int high, BOOL primary=TRUE);

    virtual CGameTimer* GetTimer()
    {
        return mpTimer;
    }

    virtual CGameInput* GetInput()
    {
        return mpInput;
    }

    virtual CGameScreen* GetScreen()
    {
        return mpScreen;
    }

    virtual CGameCharacter * Add(char *name, int curz, 
                                int curx=0, int cury=0,
                                    void *pObjID=NULL);

    virtual void Remove(CGameGraphic * stale)
    {
        if (stale != NULL)
            mpGraphics->Remove(stale);
    }

    virtual void AddInvalidRect( LPRECT rect)
    {
        if (mpUpdateList)
            mpUpdateList->AddRect(*rect);
    }

    virtual char *GetLevelName()
    {
        return (pLevName);
    }

    virtual char *GetProfileName()
    {
        return (pFilName);
    }

    virtual char *GetSectionName()
    {
        return (pLevName);
    }

    virtual int GetFrameTime()
    {
        return mFrameTime;
    }

    virtual void GameOver();
    virtual void StopAnimating();
    virtual void ReSort() 
    {
        mpGraphics->ReSort();
    }

    // members which let us access the game type and main characters directly 
    // from within the DLL
    int mGameType;
    int mNumComputerKlowns;
    CGameCharacter *mMainKlown;     // always the main player
    CGameCharacter *mComputerKlowns[4]; // always the computer-generated klown(s)
    CGameCharacter *mSecondKlown;   // second players' klown
    BOOL mFastKlown;

protected:
    // a level keeps a linked list of display objects, sorted in z-order
    CGameDisplayList* mpGraphics;   // our set of display objects
    CGameUpdateList * mpUpdateList;

    int mMaxWorldX;     // maximum x pixel value in our world
    int mMaxWorldY;     // maximum y pixel value in our world

    int mOffsetX;       // current offset of screen within world
    int mOffsetY;       // current offset of screen within world

    int mFrameTime;     // time at beginning of current frame

    CGameTimer* mpTimer;    // ptr to the game's timer
    CGameInput* mpInput;    // ptr to the game's input object
    CGameScreen* mpScreen;  // ptr to the game's screen object

    char *pLevName;
    char *pFilName;
    char *mpGraphicsKey;    // the .gam file section for our graphics list

    char* mpProfile;

    void MatchProfile( char* pFileName );
};

#endif // CGLEVEL_H
