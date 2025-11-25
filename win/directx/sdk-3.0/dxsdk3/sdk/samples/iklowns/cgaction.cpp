/*===========================================================================*\
|
|  File:        cgaction.cpp
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


#include <ctype.h>
#include <windows.h>
#ifdef __WATCOMC__
#include <stdlib.h>
#endif
#include "cgglobl.h"
#include "cgdebug.h"
#include "cgsprite.h"
#include "strrec.h"
#include "cgaction.h"
#include "cgimage.h"

/*---------------------------------------------------------------------------*\
|
|       Class CGameAction
|
|  DESCRIPTION:
|       
|
|
\*---------------------------------------------------------------------------*/
CGameAction::CGameAction(
    char* pFileName,
    char* pActionName,
    DWORD objID
    ) : mpSequence( NULL )
{
    // get the sequence info
    char sequenceBuf[256];
    char fileBuf[256];
    char defSequence[] = "";    // no default sequence names
    int frameRate;

    GetPrivateProfileString(
        pActionName,
        "SequenceName",
        defSequence,
        sequenceBuf,
        sizeof( sequenceBuf ),
        pFileName
        );

    GetPrivateProfileString(
        pActionName,
        "SequenceFile",
        defSequence,
        fileBuf,
        sizeof( fileBuf ),
        pFileName
        );

    frameRate = GetPrivateProfileInt(
        pActionName,
        "Rate",
        30,
        pFileName
        );

    // should we keep a mirror?
    BOOL mirrorHorz = (int) GetPrivateProfileInt(
                pActionName,
                "MirrorHorizontal",
                FALSE,
                pFileName
                );

    BOOL mirrorVert = (int) GetPrivateProfileInt(
                pActionName,
                "MirrorVertical",
                FALSE,
                pFileName

                );

    char dirBuf[256];
    mpSequence = new CGameSpriteSequence( fileBuf, sequenceBuf, frameRate, mirrorHorz, mirrorVert );

    // See if there is a sound associated with this action
    GetPrivateProfileString ( 
        pActionName,
        "Sound",
        "",
        dirBuf,
        sizeof( dirBuf ),
        pFileName
    );

    if (dirBuf[0] != '\0')
    {
        CStringRecord fields( dirBuf, "," );
        BOOL fLoop=FALSE;

        // See if the sound is to be looped when it is played
        if ((fields.GetNumFields() > 2) && (toupper(*fields[2]) == 'L'))
        {
            fLoop = TRUE;
        }

        // Create sound effect object based on WAV file
        pSoundEffect = new CSoundEffect(fields[0], objID, fLoop, gSoundMode);

        // If a volume was specified and sound got created ok,
        // set the defaul volume
        if ((pSoundEffect != NULL) && (fields.GetNumFields() > 1))
        {
            defaultVolume = atoi(fields[1]);
            pSoundEffect->SetVolume(defaultVolume);
        }
     } else {
        pSoundEffect = NULL;
     }
    // See if there is an ending sound associated with this action
    GetPrivateProfileString ( 
        pActionName,
        "SoundEnd",
        "",
        dirBuf,
        sizeof( dirBuf ),
        pFileName
    );

    if (dirBuf[0] != '\0')
    {
        CStringRecord fields( dirBuf, "," );

        // Create sound effect object based on WAV file
        pEndSoundEffect = new CSoundEffect(fields[0], objID, FALSE, gSoundMode);

        // If a volume was specified and sound got created ok,
        // set the defaul volume
        if ((pEndSoundEffect != NULL) && (fields.GetNumFields() > 1))
        {
            defaultVolume = atoi(fields[1]);
            pEndSoundEffect->SetVolume(defaultVolume);
        }
     } else {
        pEndSoundEffect = NULL;
     }
}

CGameAction::CGameAction(
    char* pFileName, 
    CGameCharSequenceInfo *pSequence,
    DWORD objID
    ) : mpSequence( NULL )
{
    // get the sequence info
    char sequenceBuf[256];
    char fileBuf[256];
    char defSequence[] = "";    // no default sequence names
    int frameRate;
    lstrcpy(sequenceBuf, pSequence->SequenceName);

    lstrcpy(fileBuf, pSequence->SequenceFile);

    frameRate = pSequence->Rate;

    // should we keep a mirror?
    BOOL mirrorHorz = FALSE;
    BOOL mirrorVert = FALSE;

    mpSequence = new CGameSpriteSequence( fileBuf, sequenceBuf, frameRate, mirrorHorz, mirrorVert );

    if (pSequence->Sound.Sound != NULL)
    {
        BOOL fLoop=FALSE;

        fLoop = pSequence->Sound.Loop;

        // Create sound effect object based on WAV file
        pSoundEffect = new CSoundEffect(pSequence->Sound.Sound
        , objID, fLoop, gSoundMode);

        // If a volume was specified and sound got created ok,
        // set the defaul volume
        if ((pSoundEffect != NULL))
        {
            defaultVolume = pSequence->Sound.Rate; //atoi(fields[1]);
            pSoundEffect->SetVolume(defaultVolume);
        }
     } else {
        pSoundEffect = NULL;
     }

    if (pSequence->Sound.SoundEnd != NULL)
    {

        // Create sound effect object based on WAV file
        pEndSoundEffect = new CSoundEffect(pSequence->Sound.SoundEnd
        , objID, FALSE, gSoundMode);

        // If a volume was specified and sound got created ok,
        // set the defaul volume
        if (pEndSoundEffect != NULL)
        {
            defaultVolume = pSequence->Sound.Rate;
            pEndSoundEffect->SetVolume(defaultVolume);
        }
     } else {
        pEndSoundEffect = NULL;
     }
}


CGameAction::~CGameAction()
{
    if (pSoundEffect != NULL)
        delete pSoundEffect;
    if (pEndSoundEffect != NULL)
        delete pEndSoundEffect;
    delete mpSequence;
}

BOOL    // return TRUE if more sprites to go after this one
CGameAction::Activate()
{
    if (pSoundEffect != NULL)
        pSoundEffect->Play();
    return(TRUE);
}

BOOL    // return TRUE if more sprites to go after this one
CGameAction::DeActivate()
{
    // reset the current sequence to 0
    mpSequence->SetCurSprite( 0 );
    if (pSoundEffect != NULL)
        pSoundEffect->Stop();
    if (pEndSoundEffect != NULL)
        pEndSoundEffect->Play();
    return TRUE;
}

BOOL
GetStereoValues(
    int screenX,
    int &vol,
    int &pan
)
{
    BOOL    fOffScreen = FALSE;

    if (vol == 0)
        return FALSE;

    // Determine if object making sound is off screen to the
    // left...
    if (screenX < 0)
    {
        fOffScreen = TRUE;
        pan = 0;    // place off left speaker
    }

    // or to the right...
    else if (screenX > SCREEN_WIDTH)
    {
        fOffScreen = TRUE;
        pan = 127;      // place off right speaker

    // or somewhere on screen
    } else {
        pan = (screenX * 127) / SCREEN_WIDTH;
    }


    // If it is off screen, lower the volume accordingly
    if (fOffScreen)
    {
        int howFarX;

        // Figure out how far to the left or right the object is
        // off screen.
        howFarX = (screenX > 0) ? screenX-SCREEN_WIDTH : -screenX;

        // Adjust the volume proportionally so that it is full
        // volume when it reaches the screen and zero volume when
        // it is a full screen widths away.
        vol = vol - (howFarX * vol / SCREEN_WIDTH);

        if (vol < 0)
            vol = 0;
    }

    // return TRUE if a sound should be made
    return (vol != 0);
}


BOOL    // return TRUE if more sprites to go after this one
CGameAction::Update(int screenX)
{
    int PanVal;
    int curVolume = defaultVolume;

    if (pSoundEffect != NULL)
    {
        // Place sound in accordance with objects position in the world!
        if (GetStereoValues(screenX, curVolume, PanVal))
        {
            pSoundEffect->SetVolume(curVolume);
            pSoundEffect->SetPan(PanVal);
            pSoundEffect->SetMute(FALSE);       // make sure it is playing
        } else {
            pSoundEffect->SetMute(TRUE);        // make sure it is stopped
        }
    }

    return mpSequence->Frame();
}

void
CGameAction::Render(
    CGameScreen* pScreen,
    int x,
    int y,
    BOOL revX,
    BOOL revY,
    LPRECT pDirty
    )
{
    mpSequence->Render( pScreen, x, y, revX, revY, pDirty );
}   
