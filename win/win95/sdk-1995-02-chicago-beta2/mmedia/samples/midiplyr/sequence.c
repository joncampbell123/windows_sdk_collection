/*****************************************************************************
*
*  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
*  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
*  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
*  A PARTICULAR PURPOSE.
*
*  Copyright (C) 1993, 1994 Microsoft Corporation. All Rights Reserved.
*
******************************************************************************
*
* Sequence.C
*
* Sequencer engine for MIDI player app
*
*****************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <limits.h>

#include "debug.h"
#include "seq.h"

PRIVATE VOID FNLOCAL seqFinishPreroll(PSEQ pSeq, LPMIDIHDR lpmh);
PRIVATE MMRESULT FNLOCAL XlatSMFErr(SMFRESULT smfrc);

/***************************************************************************
*  
* seqAllocBuffers
*
* Allocate buffers for this instance.
*
* pSeq                      - The sequencer instance to allocate buffers for.
*
* Returns
*   MMSYSERR_NOERROR If the operation was successful.
*
*   MCIERR_OUT_OF_MEMORY  If there is insufficient memory for
*     the requested number and size of buffers.
*
* seqAllocBuffers allocates playback buffers based on the
* cbBuffer and cBuffer fields of pSeq. cbBuffer specifies the
* number of bytes in each buffer, and cBuffer specifies the
* number of buffers to allocate.
*
* seqAllocBuffers must be called before any other sequencer call
* on a newly allocted SEQUENCE structure. It must be paired with
* a call to seqFreeBuffers, which should be the last call made
* before the SEQUENCE structure is discarded.
*
***************************************************************************/
MMRESULT FNLOCAL seqAllocBuffers(
    PSEQ                    pSeq)
{
    DWORD                   dwEachBufferSize;
    DWORD                   dwAlloc;
    UINT                    i;
    LPBYTE                  lpbWork;

    assert(pSeq != NULL);

    pSeq->uState    = SEQ_S_NOFILE;
    pSeq->lpmhFree  = NULL;
    pSeq->lpbAlloc  = NULL;
    pSeq->hSmf      = (HSMF)NULL;
    
    /* First make sure we can allocate the buffers they asked for
    */
    dwEachBufferSize = sizeof(MIDIHDR) + (DWORD)(pSeq->cbBuffer);
    dwAlloc          = dwEachBufferSize * (DWORD)(pSeq->cBuffer);
    
    pSeq->lpbAlloc = GlobalAllocPtr(GMEM_MOVEABLE|GMEM_SHARE, dwAlloc);
    if (NULL == pSeq->lpbAlloc)
        return MCIERR_OUT_OF_MEMORY;

    /* Initialize all MIDIHDR's and throw them into a free list
    */
    pSeq->lpmhFree = NULL;

    lpbWork = pSeq->lpbAlloc;
    for (i=0; i < pSeq->cBuffer; i++)
    {
        ((LPMIDIHDR)lpbWork)->lpNext            = pSeq->lpmhFree;

        ((LPMIDIHDR)lpbWork)->lpData            = lpbWork + sizeof(MIDIHDR);
        ((LPMIDIHDR)lpbWork)->dwBufferLength    = pSeq->cbBuffer;
        ((LPMIDIHDR)lpbWork)->dwBytesRecorded   = 0;
        ((LPMIDIHDR)lpbWork)->dwUser            = (DWORD)(UINT)pSeq;
        ((LPMIDIHDR)lpbWork)->dwFlags           = 0;

        pSeq->lpmhFree = (LPMIDIHDR)lpbWork;

        lpbWork += dwEachBufferSize;
    }

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*  
* seqFreeBuffers
*
* Free buffers for this instance.
*
* pSeq                      - The sequencer instance to free buffers for.
*   
* seqFreeBuffers frees all allocated memory belonging to the
* given sequencer instance pSeq. It must be the last call
* performed on the instance before it is destroyed.
*       
****************************************************************************/
VOID FNLOCAL seqFreeBuffers(
    PSEQ                    pSeq)
{
    LPMIDIHDR               lpmh;
    
    assert(pSeq != NULL);

    if (NULL != pSeq->lpbAlloc)
    {
        lpmh = (LPMIDIHDR)pSeq->lpbAlloc;
        assert(!(lpmh->dwFlags & MHDR_PREPARED));
        
        GlobalFreePtr(pSeq->lpbAlloc);
    }
}

/***************************************************************************
*  
* seqOpenFile
*
* Associates a MIDI file with the given sequencer instance.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If there is already a file open
*     on this instance.
*     
*   MCIERR_OUT_OF_MEMORY If there was insufficient memory to
*     allocate internal buffers on the file.
*
*   MCIERR_INVALID_FILE If initial attempts to parse the file
*     failed (such as the file is not a MIDI or RMI file).
*
* seqOpenFile may only be called if there is no currently open file
* on the instance. It must be paired with a call to seqCloseFile
* when operations on this file are complete.
*
* The pstrFile field of pSeq contains the name of the file
* to open. This name will be passed directly to mmioOpen; it may
* contain a specifcation for a custom MMIO file handler. The task
* context used for all I/O will be the task which calls seqOpenFile.
*
***************************************************************************/
MMRESULT FNLOCAL seqOpenFile(
    PSEQ                    pSeq)
{                            
    MMRESULT                rc      = MMSYSERR_NOERROR;
    SMFOPENFILESTRUCT       sofs;
    SMFFILEINFO             sfi;
    SMFRESULT               smfrc;
    DWORD                   cbBuffer;

    assert(pSeq != NULL);

    if (pSeq->uState != SEQ_S_NOFILE)
    {
        return MCIERR_UNSUPPORTED_FUNCTION;
    }

    assert(pSeq->pstrFile != NULL);
    
    sofs.pstrName     = pSeq->pstrFile;

    smfrc = smfOpenFile(&sofs);
    if (SMF_SUCCESS != smfrc)
    {
        rc = XlatSMFErr(smfrc);
        goto Seq_Open_File_Cleanup;
    }

    pSeq->hSmf = sofs.hSmf;
    smfGetFileInfo(pSeq->hSmf, &sfi);
    
    pSeq->dwTimeDivision = sfi.dwTimeDivision;
    pSeq->tkLength       = sfi.tkLength;
    pSeq->cTrk           = sfi.dwTracks;
               
    /* Track buffers must be big enough to hold the state data returned
    ** by smfSeek()
    */
    cbBuffer = min(pSeq->cbBuffer, smfGetStateMaxSize());
    
    DPF(1, "seqOpenFile: tkLength %lu", (DWORD)pSeq->tkLength);

Seq_Open_File_Cleanup:    
    if (MMSYSERR_NOERROR != rc)
        seqCloseFile(pSeq);
    else
        pSeq->uState = SEQ_S_OPENED;

    return rc;
}

/***************************************************************************
*  
* seqCloseFile
*
* Deassociates a MIDI file with the given sequencer instance.
*
* pSeq                      -  The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     stopped.
*     
* A call to seqCloseFile must be paired with a prior call to
* seqOpenFile. All buffers associated with the file will be
* freed and the file will be closed. The sequencer must be
* stopped before this call will be accepted.
*
***************************************************************************/
MMRESULT FNLOCAL seqCloseFile(
    PSEQ                    pSeq)
{
    LPMIDIHDR               lpmh;
    
    assert(pSeq != NULL);
    
    if (SEQ_S_PREROLLED != pSeq->uState && SEQ_S_OPENED != pSeq->uState)
        return MCIERR_UNSUPPORTED_FUNCTION;
    
    if ((HSMF)NULL != pSeq->hSmf)
    {
        smfCloseFile(pSeq->hSmf);
        pSeq->hSmf = (HSMF)NULL;
    }

    /* If we were prerolled, need to clean up -- have an open MIDI handle
    ** and buffers in the ready queue
    */

    while (pSeq->lpmhReadyFront)
    {
        lpmh = pSeq->lpmhReadyFront;
        pSeq->lpmhReadyFront = lpmh->lpNext;

        if (pSeq->hmidi != NULL)
            midiOutUnprepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh));

        lpmh->lpNext = pSeq->lpmhFree;
        pSeq->lpmhFree = lpmh;
    }

    pSeq->lpmhReadyRear = NULL;

    if (pSeq->hmidi != NULL)
    {
        midiStreamClose(pSeq->hmidi);
        pSeq->hmidi = NULL;
    }

    pSeq->uState = SEQ_S_NOFILE;

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*  
* seqPreroll
*
* Prepares the file for playback at the given position.
*
* pSeq                      - The sequencer instance.
*
* lpPreroll                 - Specifies the starting and ending tick
*                             positions to play between.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     opened or prerolled.
*
* Open the device so we can initialize channels.
*
* Loop through the tracks. For each track, seek to the given position and
* send the init data SMF gives us to the handle.
*
* Wait for all init buffers to finish.
*
* Unprepare the buffers (they're only ever sent here; the sequencer
* engine merges them into a single stream during normal playback) and
* refill them with the first chunk of data from the track. 
*
*     
****************************************************************************/
MMRESULT FNLOCAL seqPreroll(
    PSEQ                    pSeq,
    LPPREROLL               lpPreroll)
{
    SMFRESULT               smfrc;
    MMRESULT                mmrc        = MMSYSERR_NOERROR;
    MIDIPROPTIMEDIV         mptd;
    LPMIDIHDR               lpmh        = NULL;
    UINT                    uDeviceID;

    DPF(1, "seqPreroll Part 1");
    
    assert(pSeq != NULL);

    if (pSeq->uState != SEQ_S_OPENED &&
        pSeq->uState != SEQ_S_PREROLLED)
        return MCIERR_UNSUPPORTED_FUNCTION;

    pSeq->fdwSeq |= SEQ_F_WAITING;

    pSeq->tkBase = lpPreroll->tkBase;
    pSeq->tkEnd  = lpPreroll->tkEnd;
    pSeq->uBuffersInMMSYSTEM = 0;

    if (pSeq->uState == SEQ_S_PREROLLED)
    {
        lpmh = pSeq->lpmhReadyFront;
        while (lpmh != NULL)
        {
            pSeq->lpmhReadyFront = pSeq->lpmhReadyFront->lpNext;

            lpmh->lpNext = pSeq->lpmhFree;
            pSeq->lpmhFree = lpmh;
            
            lpmh = pSeq->lpmhReadyFront;
        }

        pSeq->lpmhReadyRear = NULL;
    }
    
    pSeq->uState = SEQ_S_PREROLLING;
    
    /*
    ** We've successfully opened the file; now
    ** open the MIDI device and set the time division.
    **
    ** NOTE: seqPreroll is equivalent to seek; device might already be open
    */
    if (NULL == pSeq->hmidi)
    {
        DPF(1, "seqPreroll: midiStreamOpen");
        uDeviceID = pSeq->uDeviceID;
        if ((mmrc = midiStreamOpen(&pSeq->hmidi,
                                   &uDeviceID,
                                   1,
                                   (DWORD)(WORD)pSeq->hWnd,
                                   0,
                                   CALLBACK_WINDOW)) != MMSYSERR_NOERROR)
        {
            DPF(1, "midiStreamOpen() -> %lu", (DWORD)mmrc);
            mmrc = MCIERR_DEVICE_NOT_READY;
            goto seq_Preroll_Cleanup;
        }
        
        DPF(1, "seqPreroll: midiStreamOpen done HMIDI=%04X", (WORD)pSeq->hmidi);
        
        mptd.cbStruct  = sizeof(mptd);
        mptd.dwTimeDiv = pSeq->dwTimeDivision;
        if ((mmrc = midiStreamProperty(
                                       (HMIDI)pSeq->hmidi,
                                       (LPBYTE)&mptd,
                                       MIDIPROP_SET|MIDIPROP_TIMEDIV)) != MMSYSERR_NOERROR)
        {
            DPF(1, "midiStreamProperty() -> %04X", (WORD)mmrc);
            midiStreamClose(pSeq->hmidi);
            mmrc = MCIERR_DEVICE_NOT_READY;
            goto seq_Preroll_Cleanup;
        }

        DPF(1, "seqPreroll: midiStreamProperty done!");
    }

    assert((HMIDIOUT)NULL != pSeq->hmidi);
    assert(NULL != pSeq->lpmhFree);

    lpmh = pSeq->lpmhFree;
    pSeq->lpmhFree = lpmh->lpNext;

    if (SMF_SUCCESS != (smfrc = smfSeek(pSeq->hSmf, pSeq->tkBase, lpmh)))
    {
        DPF(1, "smfSeek() returned %lu", (DWORD)smfrc);
        mmrc = XlatSMFErr(smfrc);
        goto seq_Preroll_Cleanup;
    }

    if (lpmh->dwBytesRecorded)
    {
        DPF(1, "sizeof(*lpmh) %u", sizeof(*lpmh));
        if (MMSYSERR_NOERROR != (mmrc = midiOutPrepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh))))
        {
            DPF(1, "midiOutPrepare(preroll) -> %lu!", (DWORD)mmrc);

            mmrc = MCIERR_DEVICE_NOT_READY;
            goto seq_Preroll_Cleanup;
        }

        ++pSeq->uBuffersInMMSYSTEM;

        if (MMSYSERR_NOERROR != (mmrc = midiStreamOut(pSeq->hmidi, lpmh, sizeof(*lpmh))))
        {
            DPF(1, "midiStreamOut(preroll) -> %lu!", (DWORD)mmrc);

            mmrc = MCIERR_DEVICE_NOT_READY;
            --pSeq->uBuffersInMMSYSTEM;
            goto seq_Preroll_Cleanup;
        }
    }
    else
    {
        // Empty buffer, we'll never get to preroll cleanup
        //
        seqFinishPreroll(pSeq, lpmh);
    }

seq_Preroll_Cleanup:
    if (MMSYSERR_NOERROR != mmrc)
    {
        DPF(1, "seqPreroll Part 1 failure");
        if (NULL != lpmh)
        {
            if (lpmh->dwFlags & MHDR_PREPARED)
            {
                midiOutUnprepareHeader(pSeq->hmidi, lpmh, sizeof(lpmh));
            }

            lpmh->lpNext = pSeq->lpmhFree;
            pSeq->lpmhFree = lpmh;
        }

        /* Only on success will seqFinishPreroll be called from the
        ** callback to complete -- in any other case, indicate immediate
        ** failure so background task doesn't block waiting for us
        */
        pSeq->uState = SEQ_S_OPENED;
        pSeq->fdwSeq &= ~SEQ_F_WAITING;
    }
    else
    {
        DPF(1, "seqPreroll Part 1 success");
    }

    return mmrc;
}

/***************************************************************************
*  
* seqFinishPreroll
*
* Completes the preroll operation after the seek buffer has been sent.
*
* pSeq                      - The sequencer instance.
*
* lpPreroll                 - Specifies the starting and ending tick
*                             positions to play between.
*
* Put the seek buffer back into the free list.
*
* Read as much of the file as we can into the buffers from the free list.
* Prepare them and put them into the ready queue.
* Send an MMSG_DONE to the callback to let them know we're ready to play.
*     
****************************************************************************/
PRIVATE VOID FNLOCAL seqFinishPreroll(
    PSEQ                    pSeq,
    LPMIDIHDR               lpmh)
{
    SMFRESULT               smfrc;
    MMRESULT                mmrc;

    /* lpmh is the first header from the free queue -- use it and the
    ** rest to gather the first 'n' buffers of MIDI data and prepare
    ** the headers.
    ** 
    ** NOTE: Preparing the already prepared buffer is benign;
    ** midiOutPrepareHeader() will just succeed if the prepare flag
    ** is already set.
    */
    assert(NULL == pSeq->lpmhReadyRear);

    DPF(1, "seqPreroll Part 2");

    pSeq->fdwSeq &= ~SEQ_F_EOF;

    do
    {
        smfrc = smfReadEvents(pSeq->hSmf, lpmh, pSeq->tkEnd);

        if (SMF_SUCCESS != smfrc && SMF_END_OF_FILE != smfrc)
        {
            DPF(1, "SFP: smfReadEvents() -> %u", (UINT)smfrc);
            pSeq->mmrcLastErr = XlatSMFErr(smfrc);
            goto seq_Finish_Preroll_Cleanup;
        }

        if (MMSYSERR_NOERROR != midiOutPrepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh)))
        {
            DPF(1, "SFP: midiOutPrepareHeader failed");
            pSeq->mmrcLastErr = MCIERR_DEVICE_NOT_READY;
            goto seq_Finish_Preroll_Cleanup;
        }

        if (NULL == pSeq->lpmhReadyRear)
        {
            pSeq->lpmhReadyRear = pSeq->lpmhReadyFront = lpmh;
        }
        else
        {
            pSeq->lpmhReadyRear->lpNext = lpmh;
            pSeq->lpmhReadyRear         = lpmh;
        }

        lpmh->lpNext = NULL;

        if (SMF_END_OF_FILE == smfrc)
        {
            pSeq->fdwSeq |= SEQ_F_EOF;
            break;
        }

        if (NULL != (lpmh = pSeq->lpmhFree))
        {
            pSeq->lpmhFree = pSeq->lpmhFree->lpNext;
        }
    } while (NULL != lpmh);

seq_Finish_Preroll_Cleanup:

    if (MMSYSERR_NOERROR != pSeq->mmrcLastErr)
    {
        DPF(1, "seqPreroll Part 2 failure [%lu]", (DWORD)pSeq->mmrcLastErr);
        lpmh = pSeq->lpmhReadyFront;

        while (lpmh != NULL)
        {
            pSeq->lpmhReadyFront = lpmh->lpNext;

            mmrc = midiOutUnprepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh));
            if (MMSYSERR_NOERROR != mmrc)
            {
                DPF(1, "midiOutUnprepareHeader failed in cleanup!! (%lu)", (DWORD)mmrc);
            }

            lpmh->lpNext = pSeq->lpmhFree;
            pSeq->lpmhFree = lpmh;

            lpmh = pSeq->lpmhReadyFront;
        }

        pSeq->lpmhReadyRear = NULL;

        pSeq->uState = SEQ_S_OPENED;
    }
    else
    {
        pSeq->uState = SEQ_S_PREROLLED;
        DPF(1, "seqPreroll Part 2 success");
    }

    pSeq->fdwSeq &= ~SEQ_F_WAITING;

    PostMessage(pSeq->hWnd, MMSG_DONE, (WPARAM)pSeq, 0L);
}

/***************************************************************************
*  
* seqStart
*
* Starts playback at the current position.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     stopped.
*
*   MCIERR_DEVICE_NOT_READY If the underlying MIDI device could
*     not be opened or fails any call.
* 
* The sequencer must be prerolled before seqStart may be called.
*
* Just feed everything in the ready queue to the device.
*       
***************************************************************************/
MMRESULT FNLOCAL seqStart(
    PSEQ                    pSeq)
{
    MMRESULT                mmrc;
    LPMIDIHDR               lpmWork;

    assert(NULL != pSeq);

    if (SEQ_S_PREROLLED != pSeq->uState)
    {
        DPF(1, "seqStart(): State is wrong! [%u]", pSeq->uState);
        return MCIERR_UNSUPPORTED_FUNCTION;
    }

    pSeq->uBuffersInMMSYSTEM = 0;
    pSeq->uState = SEQ_S_PLAYING;
    pSeq->fdwSeq |= SEQ_F_WAITING;

    DPF(1, "About to feed ready queue to MMSYSTEM [First %08lX]", (DWORD)(pSeq->lpmhReadyFront));

    while (pSeq->lpmhReadyFront)
    {
        /* NOTE: Don't need critical section here because no-one will
        ** be reinserting into this queue until after we're stopped again
        */
        lpmWork = pSeq->lpmhReadyFront;
        if (NULL == (pSeq->lpmhReadyFront = lpmWork->lpNext))
            pSeq->lpmhReadyRear = NULL;

        ++pSeq->uBuffersInMMSYSTEM;
        #ifdef DEBUG	// this block can be removed when I get midiStreamOut to work
        if( IsBadWritePtr( lpmWork->lpData, lpmWork->dwBufferLength ) )
        	MessageBox( NULL, "bad MIDIHDR.lpdata", "debug info", MB_OK );
        if( lpmWork->dwBytesRecorded > lpmWork->dwBufferLength )
        	MessageBox( NULL, "lpmWork->dwBytesRecorded > lpmWork->dwBufferLength", "debug info", MB_OK );
        #endif
        if ((mmrc = midiStreamOut(pSeq->hmidi, lpmWork, sizeof(*lpmWork))) != MMSYSERR_NOERROR)
        {
            DPF(1, "midiStreamOut failed!");
            
            --pSeq->uBuffersInMMSYSTEM;
            pSeq->uState = SEQ_S_STOPPING;
            midiOutReset(pSeq->hmidi);
            
            return MCIERR_DEVICE_NOT_READY;
        }
    }

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*  
* seqPause
*
* Pauses playback of the instance.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     playing.
*
* The sequencer must be playing before seqPause may be called.
* Pausing the sequencer will cause all currently on notes to be turned
* off. This may cause playback to be slightly inaccurate on restart
* due to missing notes.
*       
***************************************************************************/
MMRESULT FNLOCAL seqPause(
    PSEQ                    pSeq)
{
    assert(NULL != pSeq);
    
    if (SEQ_S_PLAYING != pSeq->uState)
        return MCIERR_UNSUPPORTED_FUNCTION;

    pSeq->uState = SEQ_S_PAUSED;
    midiStreamPause(pSeq->hmidi);
    
    return MMSYSERR_NOERROR;
}

/***************************************************************************
*  
* seqRestart
*
* Restarts playback of an instance after a pause.
*
* pSeq                      - The sequencer instance.
*
* Returns
*    MMSYSERR_NOERROR If the operation is successful.
*    
*    MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     paused.
*
* The sequencer must be paused before seqRestart may be called.
*
***************************************************************************/
MMRESULT FNLOCAL seqRestart(
    PSEQ                    pSeq)
{
    assert(NULL != pSeq);
    
    if (SEQ_S_PAUSED != pSeq->uState)
        return MCIERR_UNSUPPORTED_FUNCTION;

    pSeq->uState = SEQ_S_PLAYING;
    midiStreamRestart(pSeq->hmidi);

    return MMSYSERR_NOERROR;
}

/***************************************************************************
*  
* seqStop
*
* Totally stops playback of an instance.
*
* pSeq                      - The sequencer instance.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     paused or playing.
*
* The sequencer must be paused or playing before seqStop may be called.
*
***************************************************************************/
MMRESULT FNLOCAL seqStop(
    PSEQ                    pSeq)
{
    assert(NULL != pSeq);

    /* Automatic success if we're already stopped
    */
    if (SEQ_S_PLAYING != pSeq->uState &&
        SEQ_S_PAUSED != pSeq->uState)
    {
        pSeq->fdwSeq &= ~SEQ_F_WAITING;
        return MMSYSERR_NOERROR;
    }

    pSeq->uState = SEQ_S_STOPPING;
    pSeq->fdwSeq |= SEQ_F_WAITING;
    
    if (MMSYSERR_NOERROR != (pSeq->mmrcLastErr = midiStreamStop(pSeq->hmidi)))
    {
        DPF(1, "midiOutStop() returned %lu in seqStop()!", (DWORD)pSeq->mmrcLastErr);
        
        pSeq->fdwSeq &= ~SEQ_F_WAITING;
        return MCIERR_DEVICE_NOT_READY;
    }
    
    return MMSYSERR_NOERROR;
}

/***************************************************************************
*  
* seqTime
*
* Determine the current position in playback of an instance.
*
* pSeq                      - The sequencer instance.
*
* pTicks                    - A pointer to a DWORD where the current position
*                             in ticks will be returned.
*
* Returns
*   MMSYSERR_NOERROR If the operation is successful.
*
*   MCIERR_DEVICE_NOT_READY If the underlying device fails to report
*     the position.
*    
*   MCIERR_UNSUPPORTED_FUNCTION If the sequencer instance is not
*     paused or playing.
*
* The sequencer must be paused, playing or prerolled before seqTime
* may be called.
*
***************************************************************************/
MMRESULT FNLOCAL seqTime(
    PSEQ                    pSeq,
    PTICKS                  pTicks)
{
    MMRESULT                mmr;
    MMTIME                  mmt;
    
    assert(pSeq != NULL);

    if (SEQ_S_PLAYING != pSeq->uState &&
        SEQ_S_PAUSED != pSeq->uState &&
        SEQ_S_PREROLLING != pSeq->uState &&
        SEQ_S_PREROLLED != pSeq->uState &&
        SEQ_S_OPENED != pSeq->uState)
    {
        DPF(1, "seqTime(): State wrong! [is %u]", pSeq->uState);
        return MCIERR_UNSUPPORTED_FUNCTION;
    }

    *pTicks = 0;
    if (SEQ_S_OPENED != pSeq->uState)
    {
        *pTicks = pSeq->tkBase;
        if (SEQ_S_PREROLLED != pSeq->uState)
        {
            mmt.wType = TIME_TICKS;
            mmr = midiStreamPosition(pSeq->hmidi, &mmt, sizeof(mmt));
            if (MMSYSERR_NOERROR != mmr)
            {
                DPF(1, "midiStreamPosition() returned %lu", (DWORD)mmr);
                return MCIERR_DEVICE_NOT_READY;
            }

            *pTicks += mmt.u.ticks;
        }
    }

    return MMSYSERR_NOERROR;
}
                              
/***************************************************************************
*  
* seqMillisecsToTicks
*
* Given a millisecond offset in the output stream, returns the associated
* tick position.
*
* pSeq                      - The sequencer instance.
*
* msOffset                  - The millisecond offset into the stream.
*
* Returns the number of ticks into the stream.
*
***************************************************************************/
TICKS FNLOCAL seqMillisecsToTicks(
    PSEQ                    pSeq,
    DWORD                   msOffset)
{
    return smfMillisecsToTicks(pSeq->hSmf, msOffset);
}

/***************************************************************************
*  
* seqTicksToMillisecs
*
* Given a tick offset in the output stream, returns the associated
* millisecond position.
*
* pSeq                      - The sequencer instance.
*
* tkOffset                  - The tick offset into the stream.
*
* Returns the number of milliseconds into the stream.
*
***************************************************************************/
DWORD FNLOCAL seqTicksToMillisecs(
    PSEQ                    pSeq,
    TICKS                   tkOffset)
{
    return smfTicksToMillisecs(pSeq->hSmf, tkOffset);
}

/***************************************************************************
*  
* seqBufferDone
*
* Specifies that the given buffer has completed playback.
*
* lpmh                      - The buffer that has completed playback.
*
* This function should be called by the user whenever he receives an
* MM_MOM_DONE message. 
*
***************************************************************************/
VOID FNLOCAL seqBufferDone(
    LPMIDIHDR               lpmh)
{
    PSEQ                    pSeq;
    MMRESULT                mmrc;
    SMFRESULT               smfrc;

    assert(lpmh != NULL);
    
    pSeq = (PSEQ)(lpmh->dwUser);

    assert(pSeq != NULL);

    --pSeq->uBuffersInMMSYSTEM;
    
    /* If we're SEQ_S_PREROLLING, we're doing a preroll -- don't try to refill
    ** the buffer
    */
    if (SEQ_S_PREROLLING == pSeq->uState)
    {
        /* If all buffers have completed, we've completed the preroll --
        ** call the completion routine to clean up and notify foreground.
        **
        ** This should only ever be one buffer
        */
        if (0 == pSeq->uBuffersInMMSYSTEM)
            seqFinishPreroll(pSeq, lpmh);
        
        return;
    }

    if ((SEQ_S_STOPPING == pSeq->uState) || (pSeq->fdwSeq & SEQ_F_EOF))
    {
        DPF(1, "EOF set or stopping");
        /*
        ** Reached EOF, just put the buffer back on the free
        ** list 
        */
        lpmh->lpNext   = pSeq->lpmhFree;
        pSeq->lpmhFree = lpmh;

        if (MMSYSERR_NOERROR != (mmrc = midiOutUnprepareHeader(pSeq->hmidi, lpmh, sizeof(*lpmh))))
        {
            DPF(1, "midiOutUnprepareHeader failed in seqBufferDone! (%lu)", (DWORD)mmrc);
        }

        if (0 == pSeq->uBuffersInMMSYSTEM)
        {
            DPF(1, "seqBufferDone: normal sequencer shutdown.");
            
            /* Totally done! Free device and notify.
            */
            midiOutReset(pSeq->hmidi);
            midiStreamClose(pSeq->hmidi);
            
            pSeq->hmidi = NULL;
            pSeq->uState = SEQ_S_OPENED;
            pSeq->mmrcLastErr = MMSYSERR_NOERROR;
            pSeq->fdwSeq &= ~SEQ_F_WAITING;
            
            PostMessage(pSeq->hWnd, MMSG_DONE, (WPARAM)pSeq, 0L);
        }
    }
    else
    {
        /*
        ** Not EOF yet; attempt to fill another buffer
        */
        smfrc = smfReadEvents(pSeq->hSmf, lpmh, pSeq->tkEnd);
        
        switch(smfrc)
        {
            case SMF_SUCCESS:
                break;

            case SMF_END_OF_FILE:
                pSeq->fdwSeq |= SEQ_F_EOF;
                smfrc = SMF_SUCCESS;
                break;

            default:
                DPF(1, "smfReadEvents returned %lu in callback!", (DWORD)smfrc);
                pSeq->uState = SEQ_S_STOPPING;
                break;
        }

        if (SMF_SUCCESS == smfrc)
        {
            ++pSeq->uBuffersInMMSYSTEM;
            mmrc = midiStreamOut(pSeq->hmidi, lpmh, sizeof(*lpmh));
            if (MMSYSERR_NOERROR != mmrc)
            {
                DPF(1, "seqBufferDone(): midiStreamOut() returned %lu!", (DWORD)mmrc);
                
                --pSeq->uBuffersInMMSYSTEM;
                pSeq->uState = SEQ_S_STOPPING;
            }
        }
    }
}

/***************************************************************************
*  
* XlatSMFErr
*
* Translates an error from the SMF layer into an appropriate MCI error.
*
* smfrc                     - The return code from any SMF function.
*
* Returns
*   A parallel error from the MCI error codes.   
*
***************************************************************************/
PRIVATE MMRESULT FNLOCAL XlatSMFErr(
    SMFRESULT               smfrc)
{
    switch(smfrc)
    {
        case SMF_SUCCESS:
            return MMSYSERR_NOERROR;

        case SMF_NO_MEMORY:
            return MCIERR_OUT_OF_MEMORY;

        case SMF_INVALID_FILE:
        case SMF_OPEN_FAILED:
        case SMF_INVALID_TRACK:
            return MCIERR_INVALID_FILE;

        default:
            return MCIERR_UNSUPPORTED_FUNCTION;
    }
}

