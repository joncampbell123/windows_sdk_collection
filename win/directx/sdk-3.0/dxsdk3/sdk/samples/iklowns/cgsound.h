/*===========================================================================*\
|
|  File:        cgsound.h
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

#ifndef _CGSOUND_H
#define _CGSOUND_H

#include <mmreg.h>
#include <msacm.h>
extern "C" {
    #include "dsound.h"
};

#define USE_DSOUND  0
#define USE_WAVEOUT 1
#define USE_SNDPLAY 2

// Master wave information structure -- one for each unique
// sound (not each instance).
typedef struct _WAVE_ENTRY
{
    LPSTR           mpFileName;
    DSBUFFERDESC        *dsbd;
    LPDIRECTSOUNDBUFFER lpDirectSoundBuffer;
    LPWAVEFORMATEX      pwfxInfo;
    LPWAVEHDR       pWaveHdr;
    DWORD           hObjectId;
    int         WaveMode;
    int         mRefCount;
} WAVE_ENTRY, *LPWAVE_ENTRY;


class CSoundEffect
{
private:
    LPWAVE_ENTRY    pWaveInfo;  // ptr to master wav info
    DSBUFFERDESC    *dsbd;      // instance DS buffer
    LPDIRECTSOUNDBUFFER lpDirectSoundBuffer;
    int     WaveMode;   // method for playing
    int     curVolume;  // 0-127
    int     curPan;     // 0=left, 127=right
    BOOL        fLoop;
    BOOL        fPlaying;
    BOOL        fMuted;
    int     curChannel;

public:
    CSoundEffect(LPSTR WaveFile, DWORD UniqueId=0, BOOL fLoopIt=FALSE
    , int Mode=USE_DSOUND); 
    ~CSoundEffect();

    void Play();
    void Stop();
    void SetPan(int PanFactor);
    void SetVolume(int Volume);
    void SetMute(BOOL);
    void SetFreq();

    friend LPWAVEHDR LoadWaveData(CSoundEffect *, LPSTR, int);
    friend void SetSilence(BOOL);   
};

#endif
