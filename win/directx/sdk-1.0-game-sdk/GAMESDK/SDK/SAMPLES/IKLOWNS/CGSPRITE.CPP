/*===========================================================================*\
|
|  File:        cgsprite.cpp
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

#include <windows.h>
#include "cgdebug.h"
#include "cgglobl.h"
#include "cgtimer.h"
#include "cgscreen.h"
#include "cgsprite.h"
#include "cgupdate.h"

extern CGameTimer* Timer;

char seqSection[] = "Sequences";
char seqKey[] = "Sequence";
char spriteKey[] = "Sprite";
char sprExtension[] = "spr";
char bmpExtension[] = "bmp";

/*---------------------------------------------------------------------------*\
|
|       Class CGameSprite
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameSprite::CGameSprite(
    char* pFileName,
    char* pSpriteName,
    CGameBitBuffer* pBits,
    BOOL needHorzMirror,
    BOOL needVertMirror
    ) 
{
    // grab this sprite's parameters from the .spr file
    // and put into mInfo[0]
    mInfo[SRI_NORMAL].mSrcX = (int) GetPrivateProfileInt(
        pSpriteName,
        "X",
        0,  // default to 0
        pFileName
        );

    mInfo[SRI_NORMAL].mSrcY = (int) GetPrivateProfileInt(
        pSpriteName,
        "Y",
        0,  // default to 0
        pFileName
        );

    mInfo[SRI_NORMAL].mpBits = pBits;
    mInfo[SRI_NORMAL].mNewBits = FALSE;

    mInfo[SRI_NORMAL].mWidth = (int) GetPrivateProfileInt(
        pSpriteName,
        "Width",
        0,  // default to 0
        pFileName
        );

    mInfo[SRI_NORMAL].mHeight = (int) GetPrivateProfileInt(
        pSpriteName,
        "Height",
        0,  // default to 0
        pFileName
        );

    // create mirrored images if necessary
    if (needHorzMirror)
    {
        if (gUse_DDraw)
        {
            mInfo[SRI_HORZMIRROR].mpBits = new CGameDDrawBitBuffer(
                            mInfo[SRI_NORMAL].mWidth,
                            mInfo[SRI_NORMAL].mHeight,
                            pBits->GetHPalette(),
                            pBits->GetTransColor()
                            );
        }
        else
        {
            mInfo[SRI_HORZMIRROR].mpBits = new CGameDSBitBuffer(
                            mInfo[SRI_NORMAL].mWidth,
                            mInfo[SRI_NORMAL].mHeight,
                            pBits->GetHPalette(),
                            pBits->GetTransColor()
                            );
        }

        mInfo[SRI_HORZMIRROR].mNewBits = TRUE;
        mInfo[SRI_HORZMIRROR].mWidth = mInfo[SRI_NORMAL].mWidth;
        mInfo[SRI_HORZMIRROR].mHeight = mInfo[SRI_NORMAL].mHeight;
        mInfo[SRI_HORZMIRROR].mSrcX = 0;
        mInfo[SRI_HORZMIRROR].mSrcY = 0;

        mInfo[SRI_HORZMIRROR].mpBits->Blt(
                mInfo[SRI_HORZMIRROR].mWidth-1,
                0,
                -mInfo[SRI_HORZMIRROR].mWidth,  // create mirror image
                mInfo[SRI_HORZMIRROR].mHeight,
                pBits,
                mInfo[SRI_NORMAL].mSrcX,
                mInfo[SRI_NORMAL].mSrcY,
                SRCCOPY
                );
    }
    else
    {
        // just make a copy of normal image
        mInfo[SRI_HORZMIRROR] = mInfo[SRI_NORMAL];

        // negate the width to force a mirrored blt
        mInfo[SRI_HORZMIRROR].mWidth = -mInfo[SRI_NORMAL].mWidth;

        // make sure we don't delete it
        mInfo[SRI_HORZMIRROR].mNewBits = FALSE;
    }

    if (needVertMirror)
    {
        if (gUse_DDraw)
        {
            mInfo[SRI_VERTMIRROR].mpBits = new CGameDDrawBitBuffer(
                            mInfo[SRI_NORMAL].mWidth,
                            mInfo[SRI_NORMAL].mHeight,
                            pBits->GetHPalette(),
                            pBits->GetTransColor()
                            );

        }
        else
        {
            mInfo[SRI_VERTMIRROR].mpBits = new CGameDSBitBuffer(
                            mInfo[SRI_NORMAL].mWidth,
                            mInfo[SRI_NORMAL].mHeight,
                            pBits->GetHPalette(),
                            pBits->GetTransColor()
                            );
        }

        mInfo[SRI_VERTMIRROR].mNewBits = TRUE;
        mInfo[SRI_VERTMIRROR].mWidth = mInfo[SRI_NORMAL].mWidth;
        mInfo[SRI_VERTMIRROR].mHeight = mInfo[SRI_NORMAL].mHeight;
        mInfo[SRI_VERTMIRROR].mSrcX = 0;
        mInfo[SRI_VERTMIRROR].mSrcY = 0;

        mInfo[SRI_VERTMIRROR].mpBits->Blt(
                0,
                mInfo[SRI_VERTMIRROR].mHeight-1,
                mInfo[SRI_VERTMIRROR].mWidth,
                -mInfo[SRI_VERTMIRROR].mHeight, // create mirror image
                pBits,
                mInfo[SRI_NORMAL].mSrcX,
                mInfo[SRI_NORMAL].mSrcY,
                SRCCOPY
                );
    }
    else
    {
        // just make a copy of normal image
        mInfo[SRI_VERTMIRROR] = mInfo[SRI_NORMAL];

        // negate the height to force a mirrored blt
        mInfo[SRI_VERTMIRROR].mHeight = -mInfo[SRI_NORMAL].mHeight;

        // make sure we don't delete it
        mInfo[SRI_VERTMIRROR].mNewBits = FALSE;
    }

    // do we need both horizontal and vertical?
    if (needHorzMirror && needVertMirror)
    {
        if (gUse_DDraw)
        {
            mInfo[SRI_BOTHMIRROR].mpBits = new CGameDDrawBitBuffer(
                            mInfo[SRI_NORMAL].mWidth,
                            mInfo[SRI_NORMAL].mHeight,
                            pBits->GetHPalette(),
                            pBits->GetTransColor()
                            );

        }
        else
        {
            mInfo[SRI_BOTHMIRROR].mpBits = new CGameDSBitBuffer(
                            mInfo[SRI_NORMAL].mWidth,
                            mInfo[SRI_NORMAL].mHeight,
                            pBits->GetHPalette(),
                            pBits->GetTransColor()
                            );
        }

        mInfo[SRI_BOTHMIRROR].mNewBits = TRUE;
        mInfo[SRI_BOTHMIRROR].mWidth = mInfo[SRI_NORMAL].mWidth;
        mInfo[SRI_BOTHMIRROR].mHeight = mInfo[SRI_NORMAL].mHeight;
        mInfo[SRI_BOTHMIRROR].mSrcX = 0;
        mInfo[SRI_BOTHMIRROR].mSrcY = 0;


        mInfo[SRI_BOTHMIRROR].mpBits->Blt(
                mInfo[SRI_BOTHMIRROR].mWidth-1,
                mInfo[SRI_NORMAL].mHeight-1,
                -mInfo[SRI_BOTHMIRROR].mWidth,  // create mirror image
                -mInfo[SRI_BOTHMIRROR].mHeight,
                pBits,
                mInfo[SRI_NORMAL].mSrcX,
                mInfo[SRI_NORMAL].mSrcY,
                SRCCOPY
                );
    }
    else
    {
        // just make a copy of normal image
        mInfo[SRI_BOTHMIRROR] = mInfo[SRI_NORMAL];

        // negate the width and height to force a mirrored blt
        mInfo[SRI_BOTHMIRROR].mWidth = -mInfo[SRI_NORMAL].mHeight;
        mInfo[SRI_BOTHMIRROR].mHeight = -mInfo[SRI_NORMAL].mHeight;

        // make sure we don't delete it
        mInfo[SRI_BOTHMIRROR].mNewBits = FALSE;
    }
}

CGameSprite::~CGameSprite()
{
    for (int i = SRI_INFOCOUNT; i>0; i--)
    {
        if (mInfo[i-1].mNewBits)
        {
            delete mInfo[i-1].mpBits;
        }
    }
}

/*---------------------------------------------------------------------------*\
|
|       class CGameSpriteBuffer
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameSpriteBuffer::CGameSpriteBuffer(
    char* pFileName
    ) : mpFileName( NULL ),
        mpBitBuffer( NULL ),
        mRefCount( 0 )
{
    mpFileName = new char[lstrlen( pFileName ) +1];
    lstrcpy( mpFileName, pFileName );

    // open the set's DIB file
    char bmpName[256];
    SprToBmp( pFileName, bmpName, 256 );

    CGameDIB dib( bmpName );

    if (gUse_DDraw)
    {
        mpBitBuffer = new CGameDDrawBitBuffer( &dib );
        if (!mpBitBuffer->IsValid())
        {
            // we didn't get video memory, so delete that buffer
            delete mpBitBuffer;
            // & create a system memory one
            mpBitBuffer = new CGameDSBitBuffer( &dib );
        }
    }
    else
    {
        mpBitBuffer = new CGameDSBitBuffer( &dib );
    }
    ++mRefCount;
}

CGameSpriteBuffer::~CGameSpriteBuffer()
{
    delete mpBitBuffer;
    delete[] mpFileName;
}

void
CGameSpriteBuffer::SprToBmp(
    char* pSprName,
    char* pOutBuffer,
    int size
    )
{
    lstrcpyn( pOutBuffer, pSprName, size );

    int dotPos = strcspn( pOutBuffer, "." );

    if (dotPos < lstrlen( pOutBuffer ))
    {
        pOutBuffer[dotPos] = '\0';
        lstrcat( pOutBuffer, "." );
        lstrcat( pOutBuffer, bmpExtension );
    }
}   

/*---------------------------------------------------------------------------*\
|
|       Class CGameSpriteSequence
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/

LinkedList* CGameSpriteSequence::mpBufferList = NULL;

CGameSpriteSequence::CGameSpriteSequence(
    char* pFileName,
    char* pSequenceName,
    int rate,
    BOOL mirrorHorz,        // keep mirror images of sprites?
    BOOL mirrorVert
    ) : mpSpriteArray( NULL ),
        mNumSprites( 0 ),
        mCurSprite( 0 ),
        mpMyBuffer( NULL ),
        mLastTime( -1 )
{
    char    sprFile[MAX_PATH];

    lstrcpy(sprFile, gDataPath);
    lstrcat(sprFile, "\\");
    lstrcat(sprFile, pFileName);

    // get a ptr to this sequence's source bitmap
    mpMyBuffer = LoadSpriteBits( pFileName );

    // get the number of sprites
    mNumSprites = GetPrivateProfileInt(
            "Sequences",
            pSequenceName,
            0,
            sprFile
            );

    mpSpriteArray = new CGameSprite*[mNumSprites];

    // get the sprite list
    char spriteBuf[256];
    char defSprite[] = "";  // no default sprite names

    GetPrivateProfileString(
        pSequenceName,
        NULL,       // grab all the key names
        defSprite,
        spriteBuf,
        sizeof( spriteBuf ),
        sprFile
        );

    int i=0;
    for (char *pSprite = spriteBuf; *pSprite && (i<mNumSprites); pSprite++)
    {
        mpSpriteArray[i] = new CGameSprite( sprFile, pSprite, mpMyBuffer->mpBitBuffer, mirrorHorz, mirrorVert );
        pSprite += lstrlen( pSprite );  // move beyond terminator
        ++i;
    }

    mPeriod = 1000 / rate;
}

CGameSpriteSequence::~CGameSpriteSequence()
{
    // !!! assumes that mpSpriteArray is valid if mNumSprites>0
    for (; mNumSprites>0; mNumSprites--)
    {
        delete mpSpriteArray[mNumSprites-1];
    }

    // remove our file from list
    UnloadSpriteBits();

    delete[] mpSpriteArray;
}

BOOL    // return TRUE if the are more sprites in sequence
CGameSpriteSequence::Frame()
{
    return TRUE;
}

int // return mCurSprite .... is Zero if no more (i.e., wrapped)
CGameSpriteSequence::NextSprite(
    int time,   // current game time
    BOOL wrap   // wrap around (i.e. loop) at end of sequence
    )
{
    if (mLastTime == -1)
        mLastTime = time-mPeriod;

    int elapsed = time - mLastTime;
    if (elapsed >= mPeriod)
    {
        mLastTime = time;
        mCurSprite += elapsed / mPeriod;
        if (mCurSprite >= mNumSprites)
        {
            mCurSprite = wrap ? (mCurSprite % mNumSprites) : 0;
        }
    }
    return mCurSprite;
}

void
CGameSpriteSequence::Render(
    CGameScreen* pScreen,
    int x,
    int y,
    BOOL revX,  // reverse the image?
    BOOL revY,  // reverse the image?
    LPRECT pDirty   // invalid rectangle in screen coordinates
    )
{
    int which = (int) revX + ((int) revY * 2);
    RECT update;
    int clipX = max(x, pDirty->left);
    int clipY = max(y, pDirty->top);

    int offsetX = max(pDirty->left - x, 0);
    int offsetY = max(pDirty->top - y, 0);

    update.left = mpSpriteArray[mCurSprite]->mInfo[which].mSrcX +
            offsetX;

    update.top = mpSpriteArray[mCurSprite]->mInfo[which].mSrcY +
            offsetY;

    // we're using right & bottom for width & height
    update.right = min( mpSpriteArray[mCurSprite]->mInfo[which].mWidth -
            offsetX, pDirty->right-clipX+1 );

    update.bottom = min( mpSpriteArray[mCurSprite]->mInfo[which].mHeight -
            offsetY, pDirty->bottom-clipY+1 );

    if ((update.right > 0) && (update.bottom > 0))
        pScreen->TransRender(
                clipX,
                clipY,
                update.right,
                update.bottom,
                mpSpriteArray[mCurSprite]->mInfo[which].mpBits,
                update.left,
                update.top
                );
}

// return a ptr to the sprite buffer corresponding to this .spr filename
// keeps track of whether file is already loaded & maintains reference count
CGameSpriteBuffer*
CGameSpriteSequence::LoadSpriteBits( char* pFileName )
{
    CGameSpriteBuffer* pResult = NULL;
    CGameSpriteBuffer* pCurBuffer = NULL;

    // initialize buffer list if necessary
    if (!mpBufferList)
    {
        mpBufferList = new LinkedList;
    }
    else
    {
        pCurBuffer = (CGameSpriteBuffer*) mpBufferList->GetFirst();

        while (pCurBuffer && !pResult)
        {
            if (lstrcmpi( pFileName, pCurBuffer->mpFileName ) == 0 )
            {
                pResult = pCurBuffer;
                pCurBuffer->mRefCount++;
            }
            else
            {
                pCurBuffer = (CGameSpriteBuffer*) mpBufferList->GetNext();
            }
        }
    }

    // if we didn't find it, we need to load it
    if (!pResult)
    {
        pResult = new CGameSpriteBuffer(pFileName);     // increments ref count

        mpBufferList->Append( pResult );
    }

    return pResult;
}


// decrement the ref count for the given file & remove it if 0
void
CGameSpriteSequence::UnloadSpriteBits()
{
    if (--mpMyBuffer->mRefCount == 0)
    {
        mpBufferList->Remove( mpMyBuffer );
        delete mpMyBuffer;

        // !!! should we delete the list if empty?
    }
}
