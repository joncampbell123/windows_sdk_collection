/*===========================================================================*\
|
|  File:        cgupdate.cpp
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
#include "cgupdate.h"
#include "cgimage.h"
#include "cgglobl.h"

//#define FULL_SCREEN_DIRTY

/*---------------------------------------------------------------------------*\
|
|       Class CGameUpdateList
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameUpdateList::CGameUpdateList(
    )
{
    Clear();
}

CGameUpdateList::~CGameUpdateList()
{
    mDirty = FALSE;
}

// we make a copy of the rect & add the copy to the list
void
CGameUpdateList::AddRect(
    RECT Rect
    )
{
    // first clip new rect to screen boundaries
    RECT temp = 
    {
        0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1
    };

//#ifndef FULL_SCREEN_DIRTY
    if (!gUse_DDraw)
    {
        // only add if we're on the screen
        if (IntersectRect( &temp, &Rect, &temp))
        {
            if (mDirty)
            {
                // already have a rect. Let's make the union of them the new one...
                UnionRect(&temp, &mRect, &temp);
            }
            else
            {
                // don't have a rect yet... use this one
                mDirty = TRUE;
            }
            mRect = temp;
        }       
    }
    else
    {
        mDirty = TRUE;
        mRect = temp;
    }
}

BOOL CGameUpdateList::Intersect( RECT Rect )        // find out if rectangle intersects us...
{
    RECT temp;
    return(IntersectRect(&temp, &Rect, &mRect));
}   
