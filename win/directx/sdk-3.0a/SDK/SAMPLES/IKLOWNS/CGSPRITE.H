/*===========================================================================*\
|
|  File:        cgsprite.h
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

#ifndef CGSPRITE_H
#define CGSPRITE_H

#include <linklist.h>
#include "cgbitbuf.h"

struct SpriteRenderInfo
{
    CGameBitBuffer* mpBits;     // image bits
    int mSrcX;
    int mSrcY;
    int mWidth;
    int mHeight;

    BOOL mRevX;
    BOOL mRevY;

    BOOL mNewBits;      // flag that mpBits needs deleted
};

#define SRI_NORMAL 0
#define SRI_HORZMIRROR 1
#define SRI_VERTMIRROR 2
#define SRI_BOTHMIRROR 3

#define SRI_INFOCOUNT 4

struct CGameSprite
{
    CGameSprite(
        char* pFileName,
        char* pSpriteName,
        CGameBitBuffer* pBits,
        BOOL needHorzMirror = FALSE,
        BOOL needVertMirror = FALSE
        );
    virtual ~CGameSprite();

    SpriteRenderInfo mInfo[SRI_INFOCOUNT];
};

class CGameSpriteBuffer
{
public:
    CGameSpriteBuffer( char* pFileName );
    virtual ~CGameSpriteBuffer();

    char* mpFileName;
    CGameBitBuffer* mpBitBuffer;        // a single DIB containing sprites
    int mRefCount;              // for knowing when to remove a buffer

private:
    void SprToBmp(char* pSprName,char* pOutBuffer,int size);
};

class CGameScreen;
class CGameUpdateList;

class CGameSpriteSequence
{
public:
    CGameSpriteSequence(
        char* pFileName,
        char* pSequenceName,
        int rate,
        BOOL needHorzMirror = FALSE,
        BOOL needVertMirror = FALSE
        );

    virtual ~CGameSpriteSequence();

    virtual BOOL Frame();
    virtual void Render(
            CGameScreen* pScreen,
            int x,
            int y,
            BOOL revX,
            BOOL revY,
            LPRECT pDirty
            );
    virtual int GetCurWidth()
    {
        return mpSpriteArray[mCurSprite]->mInfo[0].mWidth;
    }
    virtual int GetCurHeight()
    {
        return mpSpriteArray[mCurSprite]->mInfo[0].mHeight;
    }

    virtual int NextSprite(int time, BOOL wrap=TRUE);

    void SetCurSprite( int sprite )
    {
        mCurSprite = sprite;
        mLastTime = -1;
    }

protected:
    static LinkedList* mpBufferList;    // list of CGameSpriteBuffers containing all sprites

    CGameSprite** mpSpriteArray;        // sprites for this sequence
    int mNumSprites;            // size of the array
    int mCurSprite;             // which sprite is in the current frame
    int mPeriod;                // number of time slices per frame
    int mLastTime;              // last time we changed frames

    CGameSpriteBuffer* mpMyBuffer;      // bit buffer for this instance's sprites

    // return a ptr to the sprite buffer corresponding .spr filename
    CGameSpriteBuffer* LoadSpriteBits( char* pFileName );

    // decrement reference count & delete buffer if 0
    void UnloadSpriteBits();
};

#endif // CGSPRITE_H
