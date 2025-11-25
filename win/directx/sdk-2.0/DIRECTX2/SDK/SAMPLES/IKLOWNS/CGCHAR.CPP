/*===========================================================================*\
|
|  File:        cgchar.cpp
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
#include "cgdebug.h"
#include "cgglobl.h"
#include "cgtimer.h"
#include "cginput.h"
#include "cgchar.h"
#include "cgchdll.h"
#include "cgchrint.h"
#include "cgload.h"
#include "cgremote.h"
#include "strrec.h"

extern void dbgprintf(char *, ...);
extern CLoadingScreen *gLoadingScreen;

static int DefaultCharacterAction (CGameCharacter *me, CGameLevel *level)
{
    // move to next sprite sequence...
    me->NextSprite(level->GetFrameTime());

    // always stays in state 0 ...
    return(0);
}

static int DefaultCharacterCollide (CGameCharacter *me, CGameCharacter *other, CGameLevel *level)
{
    // what to do if we collided????
    return(ACTION_COMPLETED);
}

/*---------------------------------------------------------------------------*\
|
|       Class CGameCharacter
|
|  DESCRIPTION:
|
|
|
\*---------------------------------------------------------------------------*/
CGameCharacter::CGameCharacter(
    char* pFileName,
    char* pCharName,
    char* pGraphicKey,
    CGameLevel *pLevel,
    int minz,
    int maxz,
    int startx,
    int starty,
    void *pNewObjID,
    char *pRemoteName
    ) : CGameGraphic( minz ),
        mpActions( NULL ),
        mpName( NULL ),
        mNumActions( 0 ),
        mpCurAction( NULL ),
        mCurSubX( 0 ),
        mCurSubY( 0 ),
        mVelocityX( 0 ),
        mVelocityY( 0 ),
        mLastTime( -1 )
{
    static DWORD gObjInstance=0;
    // hook into the character DLL (or built-in) functionality, if present
    // note: if nothing is found, then we'll use *default* actions!
    mpLevel = pLevel;
    mpName = new char[lstrlen( pCharName ) + 1];
    lstrcpy( mpName, pCharName );

    mpCharInfo = FindCharInfo(pCharName);
    if (mpCharInfo)
    {
        mpCharInfo->Create(this, pLevel);
        pCollideFunc = mpCharInfo->Collide;
        mNumActions = mpCharInfo->NumSequences;
    }
    else
    {
        pActFunc = DefaultCharacterAction;
        pCollideFunc = DefaultCharacterCollide;
        mNumActions = 1;
    }

    // now that we know how many actions there are, allocate our array 
    mpActions = new CGameAction *[mNumActions];

    char    *actionBuf;
    char    defAction[] = "";       // no default action names

    if (mpCharInfo)
    {
        int ix = 0;
        for ( ix = 0; ix < mNumActions ; ix++ ) 
        {
            if (gLoadingScreen != NULL)
                gLoadingScreen->Update();

            mpActions[ix] = new CGameAction( pFileName, &(mpCharInfo->Sequences[ix])
            , (DWORD)this );
        }
    }
    else
    {
        actionBuf = new char [32 * mNumActions];
        GetPrivateProfileString( 
            pCharName,
            NULL,       // grab all the key names
            defAction,
            actionBuf,
            32 * mNumActions,
            pFileName
             );

        int ix = 0;
        for ( char *pAction = actionBuf; *pAction && ( ix < mNumActions ); pAction++, ix++ ) {
            if (gLoadingScreen != NULL)
                gLoadingScreen->Update();
            mpActions[ix] = new CGameAction( pFileName, pAction
            , (DWORD)this );
            pAction += lstrlen( pAction );  // move beyond terminator
        }

        delete [] actionBuf;
    }

    // Initialize remote stuff
    pObjID = (REMOTE_OBJECT *)pNewObjID;
    pActionQueue = NULL;
    fRemote = FALSE;
    if (mpCharInfo)
    {
        if (pObjID != NULL)
        {
            fRemote = TRUE;
            pActFunc = mpCharInfo->RemoteAction;
            
            // Create an action queue
            pActionQueue = CreateRemoteObjectQueue(pObjID, (void *)this);

        } else {
            if (mpCharInfo->RemoteAction != NULL)
            {
                if (pRemoteName == NULL)
                    pRemoteName = pCharName;

                pObjID = CreateRemotePeers(pRemoteName, gObjInstance++);
            }
            pActFunc = mpCharInfo->Action;
        }
    }

    mMaxZ = maxz;
    mMinZ = minz;
    mCurZ = (mMaxZ+mMinZ)/2;

    mpCurAction = mpActions[0];
    curState = -1;
    MoveTo(startx, starty);
}

CGameCharacter::~CGameCharacter()
{
    if (fRemote)
    {
        // Get rid of the action queue
        DestroyRemoteObjectQueue(pObjID);
    }
    else 
    {
            DestroyRemotePeer(pObjID);
    }

    // delete all the action objects
    for ( ; mNumActions > 0; mNumActions-- ) 
    {
        delete  mpActions[mNumActions - 1];
    }

    delete[] mpActions;
    delete[] mpName;
}

void CGameCharacter::Update(
    CGameLevel* pLevel,
    CGameUpdateList* pList      // we add to this if we move
    )
{
    if (mpNext)
        mpNext->Update(pLevel, pList);

    CGameAction* mpPrevAction = mpCurAction;

    int newState = (pActFunc)(this, pLevel);

    // set new state
    if (newState != -1)
    {
        // don't change state if out of bounds!
        if (newState >= mNumActions)
        {
            newState = curState;
        }

        if (newState >= 0)
            mpCurAction = mpActions[newState];
    }

    // See if this is a new action we've entered into.
    if (newState != curState)
    {
        mpPrevAction->DeActivate();
        curState = newState;
        if (curState != -1 && curState != -2)
        {
            mpCurAction->Activate();
        }
    }

    //if (mpCurAction != NULL)
    mpCurAction->Update(pLevel->World2ScreenX(SUB2WORLD(mCurSubX), mXParallax));

    if (newState == -1)
    {
        // kill me
        Destroy(this, pLevel);
        pLevel->Remove(this);
    }
}

// display the character in its position on given screen with given screen offset
void
CGameCharacter::Render(
    CGameLevel* pLevel,
    CGameScreen* pScreen,
    CGameUpdateList* pList
    )
{
    if (mpNext)
    {
        // recurse down the list to get background filled in
        mpNext->Render(pLevel, pScreen, pList);
    }

    RECT update;

    // only draw if we're in the invalid list:
    if (IntersectRect( &update, pList->GetDirtyRect(), &mRect))
    {
        mpCurAction->Render(
                pScreen,
                pLevel->World2ScreenX(SUB2WORLD(mCurSubX), mXParallax),
                pLevel->World2ScreenY(SUB2WORLD(mCurSubY), mYParallax),
                FALSE,  // don't ever reverse the image
                FALSE,
                pList->GetDirtyRect()
                );
    }
}   

void
CGameCharacter::MoveTo( int worldX, int worldY )
{
    mpLevel->AddInvalidRect(&mRect);

    mCurSubX = WORLD2SUB(worldX);
    mCurSubY = WORLD2SUB(worldY);

    mRect.left = mpLevel->World2ScreenX(worldX, mXParallax);
    mRect.top = mpLevel->World2ScreenY(worldY, mYParallax);
    mRect.right = mRect.left + GetCurWidth() - 1;
    mRect.bottom = mRect.top + GetCurHeight() - 1;

    mpLevel->AddInvalidRect(&mRect);
}

int
CGameCharacter::NextSprite(
    int time,   // current game time
    BOOL wrap   // should sequence wrap around?
    )
{
    mpLevel->AddInvalidRect(&mRect);
    int result = mpCurAction->NextSprite(time, wrap);

    // need to recalculate screen position in case screen moved
    mRect.left = mpLevel->World2ScreenX(SUB2WORLD(mCurSubX), mXParallax);
    mRect.top = mpLevel->World2ScreenY(SUB2WORLD(mCurSubY), mYParallax);
    mRect.right = mRect.left + GetCurWidth() - 1;
    mRect.bottom = mRect.top + GetCurHeight() - 1;

    mpLevel->AddInvalidRect(&mRect);

    return result;
}

int CGameCharacter::GetRemoteAction(
    void *&Data,
    DWORD &DataSize
)
{
    return(GetNextRemoteAction(pActionQueue, Data, DataSize));
}   

int CGameCharacter::TransmitRemoteAction(
    int action, 
    void *Data, 
    DWORD DataSize
)
{
    SendRemoteAction(pObjID, action, Data, DataSize);
    return(0);
}   

void CGameCharacter::ReleaseRemoteAction(
    void    *Data
)
{
    ReleaseRemoteData(Data);
}
