/*===========================================================================*\
|
|  File:        cgsound.cpp
|
|  Description: 
|   Sample Immortal Klowns game sound effects routines.
|
|   Sound effects are implemented as an object which are
|   loaded from a wave file (.WAV) and can be mixed with 
|   other sounds as well as being algorithmically altered
|   (panned from left to right, pitch changes, and volume 
|   changes).
|
|   For demonstration purposes, sounds may be played using a
|   variety of methods.
|       1) DirectSound - the preferred method, requires
|           that a supported sound board be present
|           and that the DirectSound drivers be installed.
|           Offers greatest flexibility and lowest latency.
|           Doesn't support compressed wave files.
|       2) waveOut - standard Windows API for low-level sound
|           support.  Hardest to code for and not as flexible
|           as DirectSound nor as low in latency.  Assumes
|           similar format for all waves.  Only allows playing
|           one sound at a time (without using WaveMix).
|       3) sndPlaySound  - the simpliest interface which
|           yields the least flexibility and worst latency.
|           Only one sound may be played at a time!
|
|   If desired, individual sounds may be played using different
|   methods (there's no advantage to this, it's just for demos).
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

//** include files **
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "linklist.h"
#include "cgwave.h"
#include "cgsound.h"
#include "cgglobl.h"

//** local definitions **
// Default volume and panning values
#define MINFREQ_TB              0
#define MAXFREQ_TB              512
#define MINPAN_TB               0
#define MAXPAN_TB               127
#define MIDPAN_TB               64
#define MINVOL_TB               0
#define MAXVOL_TB               127

#define INITVOL_TB              0
#define INITPAN_TB              0

//** external functions **
//** external data **
//** public data **
BOOL gbQuiet = FALSE;

//** private data **

// To prevent having more than one instance of the same
// sound in memory, a list of all sounds is kept.
static CLinkedList  *mpWaveList = NULL;

// In order to turn off/on sounds we need a list of actively playing
// sounds.
static CLinkedList  *pNowPlayingList = NULL;

// Pointer to DirectSound object.
static LPDIRECTSOUND    gpds = NULL;
static LPDIRECTSOUNDBUFFER  lpDSPrimaryBuffer=NULL;

// Handle of WaveOut device.
static HWAVEOUT     hWaveDevice = NULL;

// ----------------------------------------------------------
// CreatePrimarySoundBuffer - create a primary sound buffer 
//  for direct sound.  Every DirectSound app needs to have
//  one (and only one) DirectSound buffer.
// ----------------------------------------------------------
BOOL CreatePrimarySoundBuffer()
{
    DSBUFFERDESC   dsbd;        // buffer description struct
    MMRESULT            mmResult;   // result of sound calls

    // Check if we already have a primary buffer
    if (gpds != NULL)
    {
        return(TRUE);
    }

    // Create the Direct Sound Object
    if (DirectSoundCreate(NULL,&gpds, NULL) != 0)
    {
        return(FALSE);
    }
    if( gpds->SetCooperativeLevel( ghMainWnd, DSSCL_NORMAL ) != DS_OK )
    {
           return(FALSE);
    }

    // Set up the primary direct sound buffer. 
    memset(&dsbd, 0, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
    dsbd.dwBufferBytes = 0;

    dsbd.lpwfxFormat = NULL;

    if ((mmResult = gpds->CreateSoundBuffer(&dsbd,
        &lpDSPrimaryBuffer,
        NULL)) != 0)
    {
        return(FALSE);
    }

    return(TRUE);
}

// ----------------------------------------------------------
// InitalizeWaveDevice - be sure wave method is ready for use.
// ----------------------------------------------------------
BOOL InitializeWaveDevice(
    int     WaveMode,   // sndPlaySound, waveOut, or DirectSound
    LPWAVEFORMATEX  pFormat     // default wave format
)
{
    // If we are doing waveOut's then we need a handle to the device
    // driver.
    if (WaveMode == USE_WAVEOUT)
    {

        // If there isn't a wave device already open, open one
        // using the given format
        if (hWaveDevice == NULL)
        {
            if (waveOutOpen((LPHWAVEOUT)&hWaveDevice
            , WAVE_MAPPER, pFormat, NULL, 0L, 0))
            {
                return(FALSE);
            }
        }

    // Must be using DirectSound, make sure we have a primary buffer
    } else {

        if (pNowPlayingList == NULL)
        {
            pNowPlayingList = new CLinkedList;      
        }

        // Create a DirectSound primary buffer
        return (CreatePrimarySoundBuffer());
    }
    return(TRUE);
}


// ----------------------------------------------------------
// LoadWaveData - read .WAV file information into appropriate
//  memory buffer for playback.
// ----------------------------------------------------------
LPWAVEHDR LoadWaveData(
    CSoundEffect    *pSound,    // sound effect instance
    LPSTR       WaveName,   // .WAV filename
    int     WaveMode    // sndPlaySound, waveOut or DirectSound
)
{
    LPBYTE      lpData = NULL;
    LPWAVEHDR   lpWaveHdr = NULL;
    DWORD       dwDataSize = 0;
    LPDIRECTSOUNDBUFFER lpDirectSoundBuffer = NULL;
    LPWAVEFORMATEX  pwfxInfo = NULL;

    // Check to be sure a non-null name was given
    if ((WaveName == NULL) || (*WaveName == '\0'))
    {
        return(NULL);
    }

    // For sndPlaySound, we just read the whole file into a buffer
    if (WaveMode == USE_SNDPLAY)
    {
        HANDLE  hFile = CreateFile(WaveName, GENERIC_READ, FILE_SHARE_READ
        , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return(NULL);
        }
            
        dwDataSize = GetFileSize(hFile, NULL);
        if ((dwDataSize == 0xFFFFFFFF) || (dwDataSize == 0))
        {
            CloseHandle(hFile);
            return(NULL);
        }

        // Allocate and lock memory for the waveform data. The memory
        // for waveform data must be globally allocated with
        // GMEM_MOVEABLE and GMEM_SHARE flags.
        if ((lpData = (LPBYTE) GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE
        , dwDataSize)) == NULL)
        {
            CloseHandle(hFile);
            return(NULL);
        }

        // Read the whole wave file in 
        ReadFile(hFile, lpData, dwDataSize, &dwDataSize, NULL);
        CloseHandle(hFile);

    // Either DirectSound or WaveOut
    } else {

        HMMIO       hmmioIn;    
        MMCKINFO    ckInRiff;
        MMCKINFO    ckIn;
        UINT        cbActualRead;
        UINT        cbSize;

        pwfxInfo = NULL;
        cbSize = 0;

        // Use routines in CGWAVE to open the sound file and
        // parse the data in it.    
        if (WaveOpenFile(WaveName, &hmmioIn, &pwfxInfo, &ckInRiff) != 0)
        {
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            return(NULL);
        }

        // Be sure we have an output device ready to play the sound
        if (!InitializeWaveDevice(WaveMode, pwfxInfo))
        {
            mmioClose(hmmioIn, 0);
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            return(NULL);
        }


        // Position the wave file for reading the wave data
        if (WaveStartDataRead(&hmmioIn, &ckIn, &ckInRiff) != 0)
        {
            mmioClose(hmmioIn, 0);
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            return(NULL);
        }

        // Ok, size of wave data is in ckIn, allocate that buffer.
        dwDataSize = ckIn.cksize;

        // waveOut requires that we allocate the data
        if (WaveMode == USE_WAVEOUT)
        {

            if ((lpData = (LPBYTE)GlobalAlloc(GMEM_FIXED
            , dwDataSize)) == NULL)
            {
                mmioClose(hmmioIn, 0);
                if (pwfxInfo != NULL)
                    GlobalFree(pwfxInfo);
                return(NULL);
            }
        // For DirectSound, we let it allocate the buffer for us.
        } else {
            DSBUFFERDESC    dsbd;
            DWORD   dwBSize;
            DWORD   dwWrapBSize;
            LPVOID  lpWrapPtr;
            DWORD           dw;     // size place holder
            
            // Set up the secondary direct sound buffer. 
            memset(&dsbd, 0, sizeof(DSBUFFERDESC));
            dsbd.dwSize = sizeof(DSBUFFERDESC);
            dsbd.dwFlags = DSBCAPS_CTRLDEFAULT;
            dsbd.dwBufferBytes = dwDataSize;
            dw = pwfxInfo->cbSize + sizeof(WAVEFORMATEX);

            if ((dsbd.lpwfxFormat = (LPWAVEFORMATEX)GlobalAllocPtr(GPTR|GMEM_ZEROINIT, dw)) == NULL)
            {
                mmioClose(hmmioIn, 0);
                return NULL;
            }

            // Setup the format, frequency, volume, etc.
            memcpy( dsbd.lpwfxFormat, pwfxInfo, dw );

            if (gpds->CreateSoundBuffer(&dsbd,
                    &lpDirectSoundBuffer,
                    NULL) != 0)
            {
                GlobalFreePtr(dsbd.lpwfxFormat);
                mmioClose(hmmioIn, 0);
                return NULL;
            }

            GlobalFreePtr(dsbd.lpwfxFormat);

            // Need to lock the buffer so that we can write to it!
            if (lpDirectSoundBuffer->Lock(
                    0,
                    dwDataSize,
                    (LPLPVOID)&lpData,
                    &dwBSize,
                    &lpWrapPtr,
                    &dwWrapBSize,
                    0L) != 0)
            {
                mmioClose(hmmioIn, 0);
                return NULL;
            } else {
                dwDataSize = dwBSize;
            }
        }

        // Now read the actual wave data into the data buffer.
        if (WaveReadFile(hmmioIn, dwDataSize, lpData, &ckIn
        , &cbActualRead) != 0)
        {
            // Data didn't get read for some reason!
            if (pwfxInfo != NULL)
                GlobalFree(pwfxInfo);
            if (lpData != NULL)
                GlobalFree(lpData);
        }    

        // Save info on size of data and close the wave file    
        cbSize = cbActualRead;
        dwDataSize = cbSize;
        mmioClose(hmmioIn, 0);

        // If we have a DirectSound buffer, set format & unlock it.
      if (lpDirectSoundBuffer != NULL)
        {
         lpDirectSoundBuffer->Unlock(lpData, cbActualRead, NULL, 0 );
        }

    }

    // Allocate and lock memory for the header. This memory must
    // also be globally allocated with GMEM_MOVEABLE and GMEM_SHARE flags.
    if ((lpWaveHdr = (LPWAVEHDR) GlobalAllocPtr(GMEM_MOVEABLE | GMEM_SHARE
    , (DWORD)sizeof(WAVEHDR))) == NULL) {
        GlobalFreePtr(lpData);
        return(NULL);
    }

    // After allocation, set up and prepare header.  This struct will be
    // used to play the sounds at the appropriate time.
    lpWaveHdr->lpData = (LPSTR) lpData;
    lpWaveHdr->dwBufferLength = dwDataSize;
    lpWaveHdr->dwFlags = DSBCAPS_CTRLDEFAULT;
    lpWaveHdr->dwLoops = 0L;

    pSound->pWaveInfo->lpDirectSoundBuffer = lpDirectSoundBuffer;
    pSound->pWaveInfo->pwfxInfo = pwfxInfo;

    return(lpWaveHdr);
}   

// ----------------------------------------------------------
// CSoundEffect constructor
// ----------------------------------------------------------
CSoundEffect::CSoundEffect(
    LPSTR   WaveName,   // name of wave file
    DWORD   Id,     // object id to allow one buffer per object
    BOOL    fLoopIt,    // loop the sound when played
    int WaveMode    // sndPlaySound, waveOut or DirectSound 
)
{
    pWaveInfo = NULL;
    fLoop = fLoopIt;
    fPlaying = FALSE;

    // Guard against wise-guys (we don't mind)
    if ((WaveName == NULL) || (*WaveName == '\0'))
        return;

    CSoundEffect::WaveMode = WaveMode;
    curVolume = MAXVOL_TB;
    fMuted = FALSE;

    // initialize wave list if necessary
    if (!mpWaveList)
    {
        mpWaveList = new CLinkedList;
    } else {
        // Need to search wave list to see if we already have this
        // wave data cached.
        WAVE_ENTRY  *pCurBuffer = (WAVE_ENTRY *) mpWaveList->GetFirst();

        while (pCurBuffer && !pWaveInfo)
        {
            // Do the name and mode match?
            if ((lstrcmpi( WaveName, pCurBuffer->mpFileName ) == 0 )
            && (pCurBuffer->hObjectId == Id)
            && (pCurBuffer->WaveMode == WaveMode))
            {
                pCurBuffer->mRefCount++;
                pWaveInfo = pCurBuffer;
            } else {
                pCurBuffer = (WAVE_ENTRY *) mpWaveList->GetNext();
            }
        }
    }

    // if we didn't find the wave file in our cache, we need to load it
    if (!pWaveInfo)
    {
        LPWAVEHDR   pWaveHdr;
        pWaveInfo = new WAVE_ENTRY;

        // Load up the wave data            
        if (pWaveHdr = LoadWaveData(this, WaveName, WaveMode))
        {
            // Add to list of known sounds
            pWaveInfo->mpFileName = new char [lstrlen(WaveName)+1];
            lstrcpy(pWaveInfo->mpFileName, WaveName);
            pWaveInfo->pWaveHdr = pWaveHdr;
            pWaveInfo->WaveMode = WaveMode;
            pWaveInfo->hObjectId = Id;
            pWaveInfo->mRefCount = 1;
            mpWaveList->Append( pWaveInfo );
            lpDirectSoundBuffer = pWaveInfo->lpDirectSoundBuffer;
        } else {
            delete pWaveInfo;
            pWaveInfo = NULL;
        }
    } else  {
        lpDirectSoundBuffer = pWaveInfo->lpDirectSoundBuffer;
    }
}

// ----------------------------------------------------------
// CSoundEffect destructor
// ----------------------------------------------------------
CSoundEffect::~CSoundEffect()
{
    Stop();

    if ((pWaveInfo != NULL) && (--pWaveInfo->mRefCount == 0))
    {
        mpWaveList->Remove( pWaveInfo );

        delete pWaveInfo->mpFileName;
        if (lpDirectSoundBuffer != NULL)
        {
         lpDirectSoundBuffer->Release();
        } else {
            GlobalFreePtr(pWaveInfo->pWaveHdr->lpData);
        }
        GlobalFreePtr(pWaveInfo->pwfxInfo);
        GlobalFreePtr(pWaveInfo->pWaveHdr);
        delete pWaveInfo;       // causes GP fault why?
        pWaveInfo = NULL;
        // !!! should we delete the list if empty?
    }
}

// ----------------------------------------------------------
// Play - 
// ----------------------------------------------------------
void CSoundEffect::Play()
{
    if ((pWaveInfo == NULL) || (gbQuiet))
    {
        return;
    }

    fPlaying = TRUE;

    switch (WaveMode)
    {
    case USE_WAVEOUT:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            pWaveInfo->pWaveHdr->dwLoops = fLoop;
            waveOutPrepareHeader(hWaveDevice, pWaveInfo->pWaveHdr, sizeof(WAVEHDR));
            waveOutWrite(hWaveDevice, pWaveInfo->pWaveHdr, sizeof(WAVEHDR));
            waveOutUnprepareHeader(hWaveDevice, pWaveInfo->pWaveHdr, sizeof(WAVEHDR));
        }
        break;

    case USE_DSOUND:
        if (lpDirectSoundBuffer != NULL)
        {

            // Add sound to active list if it isn't already there
            CSoundEffect *pTemp = (CSoundEffect *)pNowPlayingList->GetFirst();
            while (pTemp != NULL)
            {
                if (pTemp == this)
                {
                    break;
                }
                pTemp = (CSoundEffect *)pNowPlayingList->GetNext();
            }

            if (pTemp == NULL)
            {
                pNowPlayingList->Add(this);
            }

           if( fLoop )
            {
             lpDirectSoundBuffer->SetCurrentPosition( 0 );
             lpDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
            }
            else
            {
             lpDirectSoundBuffer->SetCurrentPosition( 0 );
             lpDirectSoundBuffer->Play(0, 0, 0);
            }
        }
        break;

    case USE_SNDPLAY:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            UINT    flags = SND_MEMORY | SND_ASYNC;

            if (fLoop)
                flags |= SND_LOOP;

            sndPlaySound((LPSTR)(pWaveInfo->pWaveHdr->lpData), flags);
        }
        break;
    }
}

// ----------------------------------------------------------
// Stop - 
// ----------------------------------------------------------
void CSoundEffect::Stop()
{
    if ((pWaveInfo == NULL) || (!fPlaying))
    {
        return;
    }

    fPlaying = FALSE;

    switch (WaveMode)
    {
    case USE_WAVEOUT:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            waveOutReset(hWaveDevice);
        }
        break;
    case USE_DSOUND:
        if (lpDirectSoundBuffer != NULL)
        {
            pNowPlayingList->Remove(this);
            lpDirectSoundBuffer->Stop();
        }
                break;

    case USE_SNDPLAY:
        if (pWaveInfo->pWaveHdr != NULL)
        {
            sndPlaySound(NULL, SND_ASYNC);
        }
        break;
    }
}

// ----------------------------------------------------------
// SetPan - 
// ----------------------------------------------------------
void CSoundEffect::SetPan(int PanFactor)
{
//  if ((pWaveInfo == NULL) || (curPan == PanFactor))
    if (pWaveInfo == NULL)
    {
        return;
    }

    curPan = PanFactor;

    switch (WaveMode)
    {
    case USE_WAVEOUT:
        break;
    case USE_DSOUND:
        if (lpDirectSoundBuffer != NULL)
        {
            // parameter pan is 0-127; dsound's pan is -10000 to +10000
            long    absPan;
            absPan = (10000 * ((curPan-64)>>2)) / 128;
                        lpDirectSoundBuffer->SetPan(absPan);
        }
        break;
    case USE_SNDPLAY:
        break;
    }
}


// ----------------------------------------------------------
// SetVolume - 
// ----------------------------------------------------------
void CSoundEffect::SetVolume(int Volume)
{
    if ((pWaveInfo == NULL)  || (curVolume == Volume))
    {
        return;
    }

    curVolume = Volume;
    switch (WaveMode)
    {
    case USE_WAVEOUT:
        break;
    case USE_DSOUND:
        if (lpDirectSoundBuffer != NULL)
        {
            // parameter volume is 0-127; dsound's volume is -10000 to 0
            long    absVolume;
            absVolume = -((127-curVolume)<<4);
                        lpDirectSoundBuffer->SetVolume(absVolume);
        }
        break;
    case USE_SNDPLAY:
        break;
    }
}


// ----------------------------------------------------------
// SetMute - 
// ----------------------------------------------------------
void CSoundEffect::SetMute(BOOL fMute)
{

    if ((pWaveInfo == NULL) || (fMuted == fMute))
    {
        return;
    }
    
    fMuted = fMute;

    if (fMuted)
    {
        Stop();
    } else {
        Play();
    }
}

void SetSilence(BOOL fQuiet)
{
    gbQuiet = fQuiet;
    if (lpDSPrimaryBuffer != NULL)
    {
        CSoundEffect *lpSound = (CSoundEffect *)pNowPlayingList->GetFirst();
        while (lpSound != NULL)
        {

            if (fQuiet)
            {
                    lpSound->lpDirectSoundBuffer->Stop();
            } else {
                if (lpSound->fLoop)
                {
                lpSound->lpDirectSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);
                } else {
                lpSound->lpDirectSoundBuffer->Play(0, 0, 0);
                }
            }

            lpSound = (CSoundEffect *)pNowPlayingList->GetNext();
        }
    }
}
