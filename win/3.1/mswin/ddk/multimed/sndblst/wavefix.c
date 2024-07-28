/****************************************************************************
 *
 *   wavefix.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "sndblst.h"

/* Driver specific bit used to mark a wave header as being done with
 * all DMA activity--so we can post the WOM_DONE message at the correct
 * time.  See dmaLoadBuffer in wavefix.c for more information.
 */
#define WHDR_REALLYDONE 0x80000000l /* internal driver flag for wave headers */

/*****************************************************************************

    public data

 ****************************************************************************/ 

HPSTR     hpCurInData     = NULL;  /* ptr to data block of current input hdr */
DWORD     dwCurInCount    = 0L;    /* bytes left in current input block */
HPSTR     hpCurData       = NULL;  /* ptr to data block of current output hdr */
DWORD     dwCurCount      = 0L;    /* bytes left in current output block */
LPWAVEHDR lpLoopStart     = NULL;  /* pointer to first block of a loop */
LPWAVEHDR lpDeadHeads     = NULL;  /* death row for wave headers */
DWORD     dwLoopCount;             /* count for current loop */
BYTE      bBreakLoop      = 0;     /* set to non-zero to break loop */
BYTE      gfWaveOutPaused = FALSE; /* are we paused? */
LPWAVEHDR glpWOQueue      = NULL;  /* wave output data buffer queue */
LPWAVEHDR glpWIQueue      = NULL;  /* wave input data buffer queue */
LPSTR     lpSilenceStart  = NULL;  /* where the padded silence starts */
WORD      wSilenceSize    = 0;     /* how big the padded silence is */

/***************************************************************************/

void FAR PASCAL wodPostAllHeaders( void )
{
LPWAVEHDR   lpNuke;             /* wavehdr to free */

    D2("postALLdeadheads");

    /* free the lpDeadHeads */
    while ( lpNuke = lpDeadHeads ) {
        lpDeadHeads = lpDeadHeads->lpNext;
        wodBlockFinished( lpNuke );
    }
}

/***************************************************************************/

void NEAR PASCAL wodPostDoneHeaders( void )
{
LPWAVEHDR   lpNuke;             /* wavehdr to free */
LPWAVEHDR   lpPrev;             /* previous wavehdr (temporary) */

    D2("postdeadheads");

    lpPrev = NULL;
    for ( lpNuke = lpDeadHeads; lpNuke; lpNuke = lpNuke->lpNext ) {
        if ( lpNuke->dwFlags & WHDR_REALLYDONE ) {
            if ( lpPrev )
                lpPrev->lpNext = NULL;
            else
                lpDeadHeads = NULL;

            /* from lpNuke down, we need to wodBlockFinished() */
            while ( lpPrev = lpNuke ) {
                lpNuke = lpNuke->lpNext;
                wodBlockFinished( lpPrev );
            }

            /* break completely out of the for() loop */
            break;
        }

        lpNuke->dwFlags |= WHDR_REALLYDONE;
        lpPrev = lpNuke;
    }
}

/****************************************************************************
 * @doc INTERNAL
 * 
 * @api WORD | wodLoadDMABuffer | This function loads a DMA buffer from the
 *      data queue.
 * 
 * @parm LPSTR | lpBuffer | Far pointer to the DMA buffer.
 * 
 * @parm WORD | wBufSize | Size of the buffer in bytes.
 * 
 * @rdesc The return value is the number of bytes transferred. A value of zero
 *     indicates that there was no more data in the output queue.
 * 
 * @comm This routine is called once when DMA is started and then again at
 *     interrupt time when a DMA block is complete.  It will in turn call a
 *     callback funtion to the app if it empties a request queue block and a
 *     callback function is defined.
 ***************************************************************************/ 
WORD FAR PASCAL wodLoadDMABuffer(LPSTR lpBuffer, WORD wBufSize)
{
WORD      wBytesTransferred; /* how many bytes transferred to DMA buffer */
WORD      wToGo;             /* min(buf space left, bytes left in data block) */
LPWAVEHDR lpNuke;            /* wavehdr to free */
LPWAVEHDR lpQ;               /* wavehdr temp for traversing list (queue) */
int       i;                 /* how many bytes to fill right */

    /* if any 'deadheads' are around, post them back to the app */
    if ( lpDeadHeads )
        wodPostDoneHeaders();

    /* don't destroy position of glpWOQueue (unless we are looping) */
    lpQ = glpWOQueue;
    wBytesTransferred = 0;

    if ( !lpQ || gfWaveOutPaused )     /* no queue at all */
        goto ldb_Fill_Silence;

    while (wBytesTransferred < wBufSize) {
        /* we break if we have no data left or we complete the request */
        if (hpCurData == NULL) {
            /* first time in for this queue */
            D3("firstq");
            if (gfDMABusy)
                D3("dmabusy");
            hpCurData = lpQ->lpData;
            dwCurCount = lpQ->dwBufferLength;

            /* check if this is the start of a loop */
            if (lpQ->dwFlags & WHDR_BEGINLOOP) {
                lpLoopStart = lpQ;
                dwLoopCount = lpQ->dwLoops;
            }
        }

        /* test if there is a request to break the loop */
        if (bBreakLoop) {
            D3("BREAKLOOP");
            dwLoopCount = 0L;
            bBreakLoop = 0;
        }

        /* If we are looping and no more loops need executing, then copy */
        /* nothing.  We still need to go through the motions to keep */
        /* everything updated correctly. */
        if ( lpLoopStart && (dwLoopCount == 0) )
            dwCurCount = 0;

        /* don't waste time if curcount is zero */
        if ( dwCurCount ) {
            /* hpCurData points to some chunk we can grab */
            wToGo = (WORD)min(dwCurCount,(DWORD)(wBufSize - wBytesTransferred));

            /* fill the buffer */
            hpCurData = MemCopySrc(lpBuffer, hpCurData, wToGo);
            lpBuffer += wToGo;
            dwCurCount -= wToGo;
            wBytesTransferred += wToGo;
            ((NPWAVEALLOC)LOWORD(lpQ->reserved))->dwByteCount += wToGo;
        }

        /* see if that emptied the current buffer */
        if (dwCurCount == 0) {
            D4("blockfin");

            if (lpQ->dwFlags & WHDR_ENDLOOP) {
                if (dwLoopCount == 0) {
                    D3("loop0");
                    lpNuke  = lpLoopStart;
                    lpQ = lpQ->lpNext;
                    while (lpNuke != lpQ) {
                        /* free up loop blocks */
                        LPWAVEHDR lpKillMe;

                        /* move the 'almost done' blocks to death row */
                        lpKillMe = lpNuke;
                        lpNuke = lpNuke->lpNext;
                        lpKillMe->lpNext = lpDeadHeads;
                        lpDeadHeads = lpKillMe;
                    }
                    lpLoopStart = NULL;
                }
                else {
                    D3("loop--");
                    dwLoopCount--;

                    /* back to the beginning of the loop */
                    lpQ = glpWOQueue = lpLoopStart;
                }
            }
            else {
                /* move the 'almost done' block into the lpDeadHeads list */
                lpNuke = lpQ;
                lpQ = lpQ->lpNext;
                if (lpLoopStart == NULL) {
                    lpNuke->lpNext = lpDeadHeads;
                    lpDeadHeads = lpNuke;
                }
            }

            if (lpQ == NULL) {
                /* end of the list */
                D3("endofq");
                hpCurData = NULL;
                dwCurCount = 0L;

                /* from the while loop (return wBytesTransferred) */
                break;
            }

            else {
                hpCurData = lpQ->lpData;
                dwCurCount = lpQ->dwBufferLength;
                if (lpLoopStart == NULL) {
                    /* check if this is the start of a loop */
                    if (lpQ->dwFlags & WHDR_BEGINLOOP) {
                        D3("loopStart");
                        lpLoopStart = lpQ;
                        dwLoopCount = lpQ->dwLoops;
                    }
                }
            }
        }
    }

    if ( !(glpWOQueue = lpQ) && lpLoopStart )
        glpWOQueue = lpLoopStart;

    if (wBytesTransferred == 0) {
        /* if DMA not active, release all deadheads - this gets rid of any */
        /* zero length waveheaders.  A zero length buffer will not kick */
        /* interrupts into action, so we MUST post these headers back NOW. */
        if (!gfDMABusy) {
            D2("zerolenfree");
            wodPostAllHeaders();
        }
    }

ldb_Fill_Silence:

    /* pad out with silence */
    if ( i = wBufSize - wBytesTransferred ) {
        lpSilenceStart = lpBuffer;
        wSilenceSize = i;
        MemFillSilent(lpSilenceStart, wSilenceSize);
    }

    else {
        lpSilenceStart = NULL;
    }

    return wBytesTransferred;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | waveCallback | This calls DriverCallback for a WAVEHDR.
 *
 * @parm PWAVEALLOC | pWave | Pointer to wave device.
 *
 * @parm WORD | msg | The message.
 *
 * @parm DWORD | dw1 | message DWORD (dw2 is always set to 0).
 *
 * @rdesc There is no return value.
 ***************************************************************************/
void FAR PASCAL waveCallback(NPWAVEALLOC pWave, WORD msg, DWORD dw1)
{

    /* Invoke the callback function, if it exists.  dwFlags contains
     * wave driver specific flags in the LOWORD and generic driver
     * flags in the HIWORD.
     * 
     * DON'T switch stacks in DriverCallback - we already did at the
     * beginning of our ISR (using StackEnter).  No need to burn another
     * stack, as we should have plenty of room for the callback.  Also,
     * we may not have been called from an ISR.  In that case, we know
     * that we are on an app's stack, and this should be ok.
     */

    if (pWave->dwCallback)
        DriverCallback(pWave->dwCallback,       /* user's callback DWORD */
                       HIWORD(pWave->dwFlags) | DCB_NOSWITCH,  /* flags */
                       pWave->hWave,            /* handle to the wave device */
                       msg,                     /* the message */
                       pWave->dwInstance,       /* user's instance data */
                       dw1,                     /* first DWORD */
                       0L);                     /* second DWORD */
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | wodBlockFinished | This function sets the done bit and invokes
 *     the callback function if there is one.
 *
 * @parm LPWAVEHDR | lpHdr | Far pointer to the header.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
void FAR PASCAL wodBlockFinished(LPWAVEHDR lpHdr)
{
NPWAVEALLOC pWav;

    D3("blkfin");

    /* set the 'done' bit */
    lpHdr->dwFlags |= WHDR_DONE;

    /* We are giving the block back to the application.  The header is no
     * longer in our queue, so we reset the WHDR_INQUEUE bit.  Also, we
     * clear our driver specific bit and cauterize the lpNext pointer.
     */
    lpHdr->dwFlags &= ~(WHDR_INQUEUE | WHDR_REALLYDONE);
    lpHdr->lpNext = NULL;

    pWav = (NPWAVEALLOC)(lpHdr->reserved);

    /* invoke the callback function */
    waveCallback(pWav, WOM_DONE, (DWORD)lpHdr);
}

/****************************************************************************
 * @doc INTERNAL
 * 
 * @api WORD | widFillBuffer | This function fills a buffer from the DMA
 *     buffer.
 * 
 * @parm LPSTR | lpBuffer | Far pointer to the DMA buffer.
 * 
 * @parm WORD | wBufSize | Size of the buffer in bytes.
 * 
 * @rdesc The return value is the number of bytes Transferred. A value of zero
 *     indicates that there was no more data in the input queue.
 * 
 * @comm This routine is called once when DMA is started and then again at
 *     interrupt time when a DMA block is complete.  It will in turn call a
 *     callback funtion to the app if it fills a request queue block and a
 *     callback function is defined.
 ***************************************************************************/ 
WORD NEAR PASCAL widFillBuffer(LPSTR lpBuffer, WORD wBufSize)
{
WORD      wBytesTransferred; /* how many bytes transferred to DMA buffer */
WORD      wToGo;             /* min(buf space left, bytes left in data block) */
LPWAVEHDR lpNext;            /* next WAVEHDR in queue */

    /* if no queue, vamoose */
    if (!glpWIQueue)
        return 0;

    wBytesTransferred = 0;
    while (wBytesTransferred < wBufSize) {
        /* we break if we have no data left or we complete the request */
        if (hpCurInData == NULL) {
            /* first time in for this queue */
            hpCurInData = glpWIQueue->lpData;
            dwCurInCount = glpWIQueue->dwBufferLength;
            glpWIQueue->dwBytesRecorded = 0;
        }

        /* hpCurInData points to an empty spot in a buffer */
        wToGo = (WORD)min(dwCurInCount, (DWORD)(wBufSize - wBytesTransferred));

        /* fill the buffer */
        hpCurInData = MemCopyDst(hpCurInData, lpBuffer, wToGo);
        lpBuffer += wToGo;
        dwCurInCount -= wToGo;
        wBytesTransferred += wToGo;
        glpWIQueue->dwBytesRecorded += wToGo;
        ((NPWAVEALLOC)LOWORD(glpWIQueue->reserved))->dwByteCount += wToGo;

        /* see if that filled the current buffer */
        if (dwCurInCount == 0) {
            D4("loopfin");

            /* move on to the next block in the queue */
            lpNext = glpWIQueue->lpNext;
            glpWIQueue->dwFlags |= WHDR_DONE;
            glpWIQueue->dwFlags &= ~WHDR_INQUEUE;

            /* release the data block */
            widBlockFinished(glpWIQueue);
            glpWIQueue = lpNext;

            if (glpWIQueue == NULL) {
                /* end of the list */
                D3("endofq");
                hpCurInData = NULL;
                dwCurInCount = 0L;

                /* from the while loop (return wBytesTransferred) */
                break;
            }

            else {
                hpCurInData = glpWIQueue->lpData;
                dwCurInCount = glpWIQueue->dwBufferLength;
                glpWIQueue->dwBytesRecorded = 0;
            }
        }
    }

    return wBytesTransferred;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | widBlockFinished | This function sets the done bit and invokes
 *     the callback function if there is one.
 *
 * @parm LPWAVEHDR | lpHdr | Far pointer to the header.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
void FAR PASCAL widBlockFinished(LPWAVEHDR lpHdr)
{
NPWAVEALLOC  pInClient;

    /* if it's an empty block, set the 'done' bit and length field */
    if (!(lpHdr->dwFlags & WHDR_DONE)) {
        lpHdr->dwFlags |= WHDR_DONE;
        lpHdr->dwFlags &= ~WHDR_INQUEUE;
        lpHdr->dwBytesRecorded = 0;
    }

    pInClient = (NPWAVEALLOC)(lpHdr->reserved);

    /* call client's callback */
    waveCallback(pInClient, WIM_DATA, (DWORD)lpHdr);
}
