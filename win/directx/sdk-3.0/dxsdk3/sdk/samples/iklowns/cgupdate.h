/*===========================================================================*\
|
|  File:        cgupdate.h
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

#ifndef CGUPDATE_H
#define CGUPDATE_H

#include <linklist.h>

// NOTE: all update rectangles are inclusive; both coordinates in the
// RECT are included in the update
class CGameUpdateList
{
public:
    CGameUpdateList();
    virtual ~CGameUpdateList();

    virtual void AddRect( RECT Rect );      // add to update list
    virtual BOOL Intersect( RECT Rect );        // find out if rectangle intersects us...
    virtual void Clear() {                      // remove all rectangles
        mRect.top = mRect.bottom = mRect.left = mRect.right = 0;
        mDirty = FALSE;
    };                      
    virtual LPRECT GetDirtyRect()
    {
        return &mRect;
    }

protected:
    RECT    mRect;
    BOOL    mDirty;
};

#endif // CGUPDATE_H
