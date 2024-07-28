/****************************************************************************
 *
 *   wavein.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "sndblst.h"

/*****************************************************************************

    internal function prototypes

 ****************************************************************************/ 

static void NEAR PASCAL widFreeQ(void);
static void NEAR PASCAL widGetDevCaps(LPBYTE lpCaps, WORD wSize);

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | widFreeQ | Free all buffers.
 *
 * @rdesc There is no return value.
 ***************************************************************************/ 
static void NEAR PASCAL widFreeQ(void)
{
LPWAVEHDR lpH, lpN;

    lpH = glpWIQueue;              /* point to top of the queue */
    glpWIQueue = NULL;             /* mark the queue as empty */

    while (lpH) {
        lpN = lpH->lpNext;
        widBlockFinished(lpH);
        lpH = lpN;
    }
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | widAddBuffer | Adds a buffer to our input queue.
 *
 * @rdesc The return value is an MMSYS error code (0L if success).
 ***************************************************************************/ 
static DWORD NEAR PASCAL widAddBuffer( LPWAVEHDR lpwh )
{
LPWAVEHDR   lpN;

    /* check if it's been prepared */
    if (!(lpwh->dwFlags & WHDR_PREPARED))
        return WAVERR_UNPREPARED;

    /* check if it's in our queue already */
    if (lpwh->dwFlags & WHDR_INQUEUE)
        return WAVERR_STILLPLAYING;

    /* add the buffer to our queue */
    lpwh->dwFlags |= WHDR_INQUEUE;
    lpwh->dwFlags &= ~WHDR_DONE;

    /* sanity */
    lpwh->dwBytesRecorded = 0;
    lpwh->lpNext = NULL;

    CritEnter();
    {
        if ( lpN = glpWIQueue ) {
            while ( lpN->lpNext && (lpN = lpN->lpNext) )
                ;

            lpN->lpNext = lpwh;
        }

        else 
            glpWIQueue = lpwh;
    }
    CritLeave();

    /* return success */
    return ( 0L );
}

/****************************************************************************
 * @doc INTERNAL
 * 
 * @api WORD | widSendPartBuffer | This function is called from widStop().
 *     It looks at the buffer at the head of the queue and, if it contains
 *     any data, marks it as done as sends it back to the client.
 * 
 * @rdesc The return value is the number of bytes transfered. A value of zero
 *     indicates that there was no more data in the input queue.
 ***************************************************************************/ 
void NEAR PASCAL widSendPartBuffer(void)
{
LPWAVEHDR lpH;

    /* Note that unlike midi input, we don't have to check if the
     * current buffer has data - it is only called if wave input is
     * started, which means it has data (whereas midi input being
     * started doesn't necessarily mean data has been received).
     */
    if (glpWIQueue) {
        lpH = glpWIQueue;
        glpWIQueue = glpWIQueue->lpNext;
        lpH->dwFlags |= WHDR_DONE;
        lpH->dwFlags &= ~WHDR_INQUEUE;
        dwCurInCount = 0L;
        hpCurInData = NULL;
        widBlockFinished(lpH);
    }
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | widGetDevCaps | Get the device capabilities.
 *
 * @parm LPBYTE | lpCaps | Far pointer to a WAVEINCAPS structure to
 *     receive the information.
 *
 * @parm WORD | wSize | Size of the WAVEINCAPS structure.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
static void NEAR PASCAL widGetDevCaps(LPBYTE lpCaps, WORD wSize)
{
WAVEINCAPS wc;

    wc.wMid = MM_MICROSOFT;
    wc.wPid = MM_SNDBLST_WAVEIN;
    wc.vDriverVersion = DRIVER_VERSION;
    wc.dwFormats = WAVE_FORMAT_1M08;
    wc.wChannels = 1;
    LoadString(ghModule, IDS_SNDBLSTWAVEIN, wc.szPname, MAXPNAMELEN);

    MemCopy(lpCaps, &wc, min(wSize, sizeof(wc)));
}

/****************************************************************************

    This function conforms to the standard wave input driver message proc

****************************************************************************/
DWORD FAR PASCAL _loadds widMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2)
{
const WAVEFORMAT FAR *lpFmt;      /* pointer to passed format */
NPWAVEALLOC           pInClient;  /* pointer to client information structure */

    if (!gfEnabled) {
        if ( msg == WIDM_INIT ) {
            D1("WIDM_INIT");
            InitDisplayConfigErrors();
            return 0L;
        }

        D1("widMessage called while disabled");
        return ((msg == WIDM_GETNUMDEVS) ? 0L : MMSYSERR_NOTENABLED);
    }

    /* this driver only supports one device */
    if (id != 0) {               
        D1("invalid wave device id");
        return MMSYSERR_BADDEVICEID;
    }

    switch (msg) {
        case WIDM_GETNUMDEVS:
            D1("WIDM_GETNUMDEVS");
            return 1L;

        case WIDM_GETDEVCAPS:
            D1("WIDM_GETDEVCAPS");
            widGetDevCaps((LPBYTE)dwParam1, (WORD)dwParam2);
            return 0L;

        case WIDM_OPEN:
            D1("WIDM_OPEN");

            /* dwParam1 contains a pointer to a WAVEOPENDESC
             * dwParam2 contains wave driver specific flags in the LOWORD
             * and generic driver flags in the HIWORD
             */

            /* make sure we can handle the format */
            lpFmt = ((LPWAVEOPENDESC)dwParam1)->lpFormat;
            if ((lpFmt->wFormatTag != WAVE_FORMAT_PCM) ||
                (lpFmt->nChannels != 1) ||
                (lpFmt->nSamplesPerSec < 4000) ||
                (lpFmt->nSamplesPerSec > 12000) ||
                (lpFmt->nAvgBytesPerSec != lpFmt->nSamplesPerSec) ||
                (lpFmt->nBlockAlign != 1) ||
                (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample != 8))
            {
                return WAVERR_BADFORMAT;
            }

            /* did they just want format information? */
            if (dwParam2 & WAVE_FORMAT_QUERY)
                return 0L;

            /* attempt to 'acquire' the Wave input hardware */
            if ( widAcquireHardware() ) {
                D1("Wave input hardware is not available!");
                return MMSYSERR_ALLOCATED;
            }

            /* allocate my per-client structure */
            pInClient = (NPWAVEALLOC)LocalAlloc(LPTR, sizeof(WAVEALLOC));
            if (pInClient == NULL) {
                widReleaseHardware();
                return MMSYSERR_NOMEM;
            }

            /* and fill it with info */
            pInClient->dwCallback  = ((LPWAVEOPENDESC)dwParam1)->dwCallback;
            pInClient->dwInstance  = ((LPWAVEOPENDESC)dwParam1)->dwInstance;
            pInClient->hWave       = ((LPWAVEOPENDESC)dwParam1)->hWave;
            pInClient->dwFlags     = dwParam2;
            pInClient->dwByteCount = 0L;
            pInClient->pcmwf       = *((LPPCMWAVEFORMAT)lpFmt);

            /* set the sample rate */
            dspSetSampleRate((WORD)lpFmt->nSamplesPerSec);

            /* give the client my driver dw */
            *((LPDWORD)dwUser) = MAKELONG(pInClient, 0);

            /* call client's callback */
            waveCallback(pInClient, WIM_OPEN, 0L);

            return 0L;

        case WIDM_CLOSE:
            D1("WIDM_CLOSE");

            if (glpWIQueue)
                return WAVERR_STILLPLAYING;

            /* just in case they started input without adding buffers... */
            widStop();

            /* call client's callback */
            pInClient = (NPWAVEALLOC)LOWORD(dwUser);
            waveCallback(pInClient, WIM_CLOSE, 0L);

            /* free the allocated memory */
            LocalFree((LOCALHANDLE)pInClient);

            /* now 'release' the Wave input hardware */
            if ( widReleaseHardware() ) {
                D1("Wave input hardware could NOT be released!");
            }

            return 0L;

        case WIDM_ADDBUFFER:
            D1("WIDM_ADDBUFFER");
            AssertF(dwParam1 != NULL);
            AssertF(!(((LPWAVEHDR)dwParam1)->dwFlags & ~(WHDR_INQUEUE|WHDR_DONE|WHDR_PREPARED)));
            ((LPWAVEHDR)dwParam1)->dwFlags &= (WHDR_INQUEUE|WHDR_DONE|WHDR_PREPARED);

            /* store the pointer to my WAVEALLOC structure in the wavehdr */
            pInClient = (NPWAVEALLOC)LOWORD(dwUser);
            ((LPWAVEHDR)dwParam1)->reserved = (DWORD)(LPSTR)pInClient;

            /* add the buffer to our queue */
            return widAddBuffer((LPWAVEHDR)dwParam1);

        case WIDM_START:
            D1("WIDM_START");
            widStart();
            return 0L;

        case WIDM_STOP:
            D1("WIDM_STOP");
            widStop();
            return 0L;

        case WIDM_RESET:
            D1("WIDM_RESET");

            /* stop if it is started and release all buffers */
            widStop(); 
            widFreeQ(); 

            /* reset byte count */
            pInClient = (NPWAVEALLOC)LOWORD(dwUser);
            pInClient->dwByteCount = 0L;

            return 0L;

        case WIDM_GETPOS:
            D1("WIDM_GETPOS");
            return waveGetPos(dwUser, (LPMMTIME)dwParam1, (WORD)dwParam2);

        default:
            return MMSYSERR_NOTSUPPORTED;
    }

    /* should never get here... */
    AssertF(0);
    return MMSYSERR_NOTSUPPORTED;
}
