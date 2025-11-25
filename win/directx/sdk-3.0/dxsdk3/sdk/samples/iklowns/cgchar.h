/*===========================================================================*\
|
|  File:        cgchar.h
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

#ifndef CGCHAR_H
#define CGCHAR_H

#include "cgupdate.h"
#include "cgaction.h"
#include "cggraph.h"
#include "cgchdll.h"
#include "cgremote.h"

// maximum number of actions per character
#define MAX_ACTIONS 32

// NOTE: our internal mCurSubX values are in 256 sub-pixel units
// externally, they are pixels -- these macro convert between the 2
#define SUB2WORLD( x ) ( x >> 8 )
#define WORLD2SUB( x ) ( x << 8 )

class CGameCharacter : public CGameGraphic
{
public:
    CGameCharacter( char* pFileName, 
                    char* pCharName, 
                    char* pGraphicKey,
                    CGameLevel * pLevel,
                    int minz, 
                    int maxz, 
                    int startx=0,
                    int starty=0,
                    void *pNewObjID=NULL,
                    char *pRemoteName=NULL);
    virtual ~CGameCharacter();
    // 'instance' data so characters can keep private data if needed
    void * mpPrivateData;

    // NOTE: our internal mCurSubX values are pixels * 256!
    // externally, they are pixels
    virtual void MoveTo( int worldX, int worldY );

    virtual void GetXY( int *worldX, int * worldY )
    {
        *worldX = SUB2WORLD(mCurSubX);
        *worldY = SUB2WORLD(mCurSubY);
    }

    virtual void SetVelocity( int vX, int vY )
    {
        mVelocityX = vX;
        mVelocityY = vY;
    }

    virtual void GetVelocity( int* pvX, int* pvY )
    {
        *pvX = mVelocityX;
        *pvY = mVelocityY;
    }

    virtual void Update(CGameLevel* pLevel, CGameUpdateList* pUpdate);
    virtual void Render(CGameLevel* pLevel, CGameScreen* pScreen, CGameUpdateList* pUpdate);

    virtual HPALETTE GetHPalette()
    {
        return NULL;    // characters can't return palettes
    }

    virtual int GetCurWidth() { return mpCurAction?mpCurAction->GetCurWidth():0; }
    virtual int GetCurHeight() { return mpCurAction?mpCurAction->GetCurHeight():0; }
    virtual int NextSprite(int time, BOOL wrap = TRUE);

    virtual int GetMinZ()
    {
        return mMinZ;
    }

    virtual int GetMaxZ()
    {
        return mMaxZ;
    }

    virtual int GetXParallax()
    {
        return mXParallax;
    }

    virtual int GetYParallax()
    {
        return mYParallax;
    }

    virtual void SetCurrentZ( int newZ )
    {
        mCurZ = newZ;       
        mpLevel->ReSort();
    }

    // Remote interaction methods
    virtual int GetRemoteAction(void *&Data, DWORD &DataSize);
    virtual int TransmitRemoteAction(int action, void *Data, DWORD DataSize);
    virtual void ReleaseRemoteAction(void *);
    BOOL IsRemoteControlled()
    {
        return fRemote;
    }

    int mLastTime;      // last time (in Timer units) we updated position

    virtual int Collided (CGameCharacter *other)
    {
        if (pCollideFunc)
            return (*pCollideFunc)( this, other, mpLevel);
        else 
            return 0;
    }

    virtual char * GetName() 
    {
        return mpName;
    }

    virtual void Destroy (CGameCharacter *me, CGameLevel *lev)
    {
        if (mpCharInfo)
            mpCharInfo->Destroy(me,lev);
    }

    // NOTE: our internal mCurSubX values are pixels * 256!
    // externally, they are pixels -- this allows the DLL
    // functions to get & set the actual values as well as move
    virtual void SetAndMove( int subX, int subY )
    {
        MoveTo( SUB2WORLD(subX), SUB2WORLD(subY) ); // this sets curworld
        mCurSubX = subX;        // so we reset
        mCurSubY = subY;
    }

    virtual void GetSubXY( int *subX, int * subY )
    {
        *subX = mCurSubX;
        *subY = mCurSubY;
    }

    // to allow DLL's to easily override our default behavior:
    CGameCharInfo * mpCharInfo;

protected:
    // for Z ordering....
    int     mMinZ, mMaxZ;

    char* mpName;

    CGameAction** mpActions;    // array of action ptrs
    int mNumActions;
    CGameAction* mpCurAction;   // our current action

    int curState;

    // NOTE: our internal mCurSubX values are pixels * 256!
    // externally, they are pixels
    int mCurSubX;       // current position in world coordinates * 256
    int mCurSubY;       // current position in world coordinates * 256

    int mVelocityX;     // current velocity ((pixels/256)/ms) in X dimension (sign == direction)
    int mVelocityY;     // current velocity ((pixels/256)/ms) in Y dimension (sign == direction)

    pActionFunction pActFunc;
    pCollideFunction pCollideFunc;
    BOOL        fRemote;    // TRUE -> controlled remotely
    REMOTE_OBJECT   *pObjID;    // unique object ID across all peers
    CLinkedList *pActionQueue;  // actions received from remotes
    CGameLevel * mpLevel;
};

#endif // CGCHAR_H
