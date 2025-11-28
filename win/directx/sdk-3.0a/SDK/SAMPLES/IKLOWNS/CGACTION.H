/*===========================================================================*\
|
|  File:        cgaction.h
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

#ifndef CGACTION_H
#define CGACTION_H

#include "cgsprite.h"
#include "cgsound.h"
#include "cgchdll.h"

class CGameScreen;

class CGameAction
{
public:
    CGameAction(char* pFileName, char* pActionName, DWORD objID=0);
    CGameAction(char* pFileName, CGameCharSequenceInfo *pSequence, DWORD objID=0);
    virtual ~CGameAction();

    virtual BOOL Activate();
    virtual BOOL DeActivate();
    virtual BOOL Update(int x); // change to next sprite in sequence
    virtual void Render(CGameScreen* pScreen, int x, int y, BOOL revX, BOOL revY, LPRECT pDirty);

    virtual int GetCurWidth()
    {
        return(mpSequence->GetCurWidth());
    }
    virtual int GetCurHeight()
    {
        return(mpSequence->GetCurHeight());
    }
    virtual int NextSprite(int time, BOOL wrap = TRUE)
    {   
        return(mpSequence->NextSprite(time, wrap));
    }

protected:
    CGameSpriteSequence* mpSequence;
    CSoundEffect    *pSoundEffect;
    CSoundEffect    *pEndSoundEffect;
    BOOL        fSoundOnlyWhenVisable;
    int     defaultVolume;
};

#endif // CGACTION_H
