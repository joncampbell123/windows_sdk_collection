/*===========================================================================*\
|
|  File:        cgdlist.h
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

#ifndef CGDLIST_H
#define CGDLIST_H

#include "cgupdate.h"

class CGameLevel;
class CGameGraphic;
class CGameScreen;

class CGameDisplayList
{
public:
    CGameDisplayList(char* pFileName, char* pLevelName, CGameLevel* pLevel);
    virtual ~CGameDisplayList();

    virtual void Update(CGameLevel* pLevel, CGameUpdateList* pUpdate);
    virtual void Render(CGameLevel* pLevel, CGameScreen* pScreen, CGameUpdateList* pUpdate);

    virtual void Insert( CGameGraphic* pGraphic );
    virtual void Remove( CGameGraphic* pGraphic );
    virtual void ReSort();

    virtual CGameGraphic * First()
    {
        return mpHead;
    }

protected:
    CGameGraphic* mpHead;   // ptr to head of our linked list
};

#endif // CGDLIST_H
