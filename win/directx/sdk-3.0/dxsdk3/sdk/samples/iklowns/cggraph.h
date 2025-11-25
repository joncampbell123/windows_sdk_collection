/*===========================================================================*\
|
|  File:        cggraph.h
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

#ifndef CGGRAPH_H
#define CGGRAPH_H

#include <windows.h>
#include "cglevel.h"
#include "cgscreen.h"
#include "cgupdate.h"

class CGameGraphic
{
public:
    CGameGraphic(
        int curz = 0
        );

    virtual ~CGameGraphic();

    virtual void Update(CGameLevel* pLevel, CGameUpdateList* pUpdate) = 0;
    virtual void Render(CGameLevel* pLevel, CGameScreen* pScreen, CGameUpdateList* pUpdate) = 0;
    virtual void MoveTo( int worldX, int worldY ) {};

    virtual HPALETTE GetHPalette() = 0;

    // return this graphic's current z position
    virtual int GetCurrentZ()
    {
        return mCurZ;
    }

    virtual int GetMinZ()
    {
        return mCurZ;       // generic graphics cannot have range
    }

    virtual int GetMaxZ()
    {
        return mCurZ;       // generic graphics cannot have range
    }

    virtual int GetWidth()
    {
        return mWidth;
    }

    virtual int GetHeight()
    {
        return mHeight;
    }

    virtual CGameGraphic* GetNext()
    {
        return mpNext;
    }

    virtual void SetNext( CGameGraphic * next)
    {
        mpNext = next;
    }

    // insert given graphic into the list after us
    virtual void Add( CGameGraphic* pGraphic )
    {
        // simple insertion into the list
        pGraphic->mpNext = mpNext;
        mpNext = pGraphic;
    }

    virtual RECT * GetRect() { return &mRect; };
    virtual void SetRect(RECT *rect) {
        mRect.top = rect->top;
        mRect.left = rect->left;
        mRect.bottom = rect->bottom;
        mRect.right = rect->right;
    };

protected:
    RECT mRect;
    BOOL mIsValid;      // flag whether this graphic object is valid
    int mXpos;
    int mYpos;
    int mWidth;     // width in pixels
    int mHeight;        // height in pixels

    int mCurZ;      // our current Z position
    int mXParallax;
    int mYParallax;

    CGameGraphic* mpNext;   // we're in a linked list of CGameGraphic
};

#endif // CGGRAPH
