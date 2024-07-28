/****************************************************************************
 *
 *   waveout.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "sndblst.h"

extern BYTE gfWaveOutPaused;        /* wavefix.c */

/*****************************************************************************

    internal function prototypes

 ****************************************************************************/ 

static void NEAR PASCAL wodFreeQ(void);
static void NEAR PASCAL wodGetDevCaps(LPBYTE lpCaps, WORD wSize);

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | wodFreeQ | Free all buffers.
 *
 * @rdesc There is no return value.
 ***************************************************************************/ 
static void NEAR PASCAL wodFreeQ(void)
{
extern void FAR PASCAL wodPostAllHeaders( void );
LPWAVEHDR lpH, lpN;

    AssertF(!gfDMABusy);         /* DMA better not be going!! */

    /* first free the lpDeadHeads... */
    wodPostAllHeaders();

    if (lpLoopStart)
       lpH = lpLoopStart;
    else
       lpH = glpWOQueue;           /* point to top of the queue */

    glpWOQueue = NULL;             /* mark the queue as empty */
    lpLoopStart = NULL;
    hpCurData = NULL;
    dwCurCount = 0L;
    dwLoopCount = 0L;
    gfDMABusy = 0;

    while (lpH) {
        lpN = lpH->lpNext;
        wodBlockFinished(lpH);
        lpH = lpN;
    }
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | wodGetDevCaps | Get the device capabilities.
 *
 * @parm LPBYTE | lpCaps | Far pointer to a WAVEOUTCAPS structure to
 *      receive the information.
 *
 * @parm WORD | wSize | Size of the WAVEOUTCAPS structure.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
static void NEAR PASCAL wodGetDevCaps(LPBYTE lpCaps, WORD wSize)
{
WAVEOUTCAPS wc;

    wc.wMid = MM_MICROSOFT;
    wc.wPid = MM_SNDBLST_WAVEOUT;
    wc.vDriverVersion = DRIVER_VERSION;
    wc.dwFormats = WAVE_FORMAT_1M08 | WAVE_FORMAT_2M08;
    wc.wChannels = 1;
    wc.dwSupport = 0;
    LoadString(ghModule, IDS_SNDBLSTWAVEOUT, wc.szPname, MAXPNAMELEN);

    MemCopy(lpCaps, &wc, min(wSize, sizeof(wc)));
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api DWORD | waveGetPos | Get the stream position in samples.
 *
 * @parm DWORD | dwUser | The DWORD passed to the driver from the open call.
 *
 * @parm LPBYTE | lpInfo | Far pointer to an MMTIME structure.
 *
 * @parm WORD | wSize | Size of the MMTIME structure.
 *
 * @rdesc The return value is zero if successful.
 ***************************************************************************/
DWORD NEAR PASCAL waveGetPos(DWORD dwUser, LPMMTIME lpmmt, WORD wSize)
{
NPWAVEALLOC pClient;

    if (wSize < sizeof(MMTIME))
        return MMSYSERR_ERROR;

    pClient = (NPWAVEALLOC)LOWORD(dwUser);

    if (lpmmt->wType == TIME_BYTES) {
        lpmmt->u.cb = pClient->dwByteCount;
    }

    /* default is samples - the sndblst card only supports 8 bit mono, */
    /* so the sample count is equal to the byte count */
    else {
        lpmmt->wType = TIME_SAMPLES;
        lpmmt->u.sample = pClient->dwByteCount;
        D2("POS: #DX#AX");
    }

    return 0L;
}

/****************************************************************************

    This function conforms to the standard Wave output driver message proc

****************************************************************************/
DWORD FAR PASCAL _loadds wodMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2)
{
const WAVEFORMAT FAR *lpFmt;      /* pointer to passed format */
NPWAVEALLOC           pOutClient; /* pointer to client information structure */

    if (!gfEnabled) {
        if ( msg == WODM_INIT ) {
            D1("WODM_INIT");
            InitDisplayConfigErrors();
            return 0L;
        }

        D1("wodMessage called while disabled");
        return ((msg == WODM_GETNUMDEVS) ? 0L : MMSYSERR_NOTENABLED);
    }

    /* this driver only supports one device */
    if (id != 0) {
        D1("invalid wave device id");
        return MMSYSERR_BADDEVICEID;
    }

    switch (msg) {
        case WODM_GETNUMDEVS:
            D1("WODM_GETNUMDEVS");
            return 1L;

        case WODM_GETDEVCAPS:
            D1("WODM_GETDEVCAPS");
            wodGetDevCaps((LPBYTE)dwParam1, (WORD)dwParam2);
            return 0L;

        case WODM_OPEN:
            D1("WODM_OPEN");

            /*  dwParam1 contains a pointer to a WAVEOPENDESC
             *  dwParam2 contains wave driver specific flags in the LOWORD
             *  and generic driver flags in the HIWORD
             */

            /* make sure we can handle the format */
            lpFmt = ((LPWAVEOPENDESC)dwParam1)->lpFormat;
            if ((lpFmt->wFormatTag != WAVE_FORMAT_PCM) ||
                (lpFmt->nChannels != 1) ||
                (lpFmt->nSamplesPerSec < 4000) ||
                (lpFmt->nSamplesPerSec > 23000) ||
                (lpFmt->nAvgBytesPerSec != lpFmt->nSamplesPerSec) ||
                (lpFmt->nBlockAlign < 1) ||
                (((LPPCMWAVEFORMAT)lpFmt)->wBitsPerSample != 8))
            {
                return WAVERR_BADFORMAT;
            }

            /* did they just want format information? */
            if (dwParam2 & WAVE_FORMAT_QUERY)
                return 0L;

            /* attempt to 'acquire' the Wave output hardware */
            if ( wodAcquireHardware() ) {
                D1("Wave output hardware is not available!");
                return MMSYSERR_ALLOCATED;
            }

            /* allocate my per-client structure */
            pOutClient = (NPWAVEALLOC)LocalAlloc(LPTR, sizeof(WAVEALLOC));
            if (pOutClient == NULL) {
                wodReleaseHardware();
                return MMSYSERR_NOMEM;
            }

            /* and fill it with info */
            pOutClient->dwCallback  = ((LPWAVEOPENDESC)dwParam1)->dwCallback;
            pOutClient->dwInstance  = ((LPWAVEOPENDESC)dwParam1)->dwInstance;
            pOutClient->hWave       = ((LPWAVEOPENDESC)dwParam1)->hWave;
            pOutClient->dwFlags     = dwParam2;
            pOutClient->dwByteCount = 0L;
            pOutClient->pcmwf       = *((LPPCMWAVEFORMAT)lpFmt);

            /* give the client my driver dw */
            *((LPDWORD)dwUser) = MAKELONG(pOutClient, 0);

            /* set the sample rate */
            dspSetSampleRate((WORD)lpFmt->nSamplesPerSec);

            /* sent client his OPEN callback message */
            waveCallback(pOutClient, WOM_OPEN, 0L);

            return 0L;

        case WODM_CLOSE:
            D1("WODM_CLOSE");

            if (glpWOQueue)
                return WAVERR_STILLPLAYING;

            /* wait, in case there's one last 2K block being played */
            wodWaitForDMA();

            /* call client's callback */
            pOutClient = (NPWAVEALLOC)LOWORD(dwUser);
            waveCallback(pOutClient, WOM_CLOSE, 0L);

            /* free the allocated memory */
            LocalFree((LOCALHANDLE)pOutClient);

            /* now 'release' the Wave output hardware */
            if ( wodReleaseHardware() ) {
                D1("Wave output hardware could NOT be released!");
            }

            return 0L;

        case WODM_WRITE:
            D1("WODM_WRITE");
            AssertF(dwParam1 != NULL);
            AssertF(!(((LPWAVEHDR)dwParam1)->dwFlags & ~(WHDR_INQUEUE|WHDR_DONE|WHDR_PREPARED|WHDR_BEGINLOOP|WHDR_ENDLOOP)));

            ((LPWAVEHDR)dwParam1)->dwFlags &= (WHDR_INQUEUE|WHDR_DONE|WHDR_PREPARED|WHDR_BEGINLOOP|WHDR_ENDLOOP);

            AssertF(((LPWAVEHDR)dwParam1)->dwFlags & WHDR_PREPARED);

            /* check if it's been prepared */
            if (!(((LPWAVEHDR)dwParam1)->dwFlags & WHDR_PREPARED))
                return WAVERR_UNPREPARED;

            AssertF(!(((LPWAVEHDR)dwParam1)->dwFlags & WHDR_INQUEUE));

            /* if it is already in our Q, then we cannot do this */
            if ( ((LPWAVEHDR)dwParam1)->dwFlags & WHDR_INQUEUE )
                return ( WAVERR_STILLPLAYING );

            /* store the pointer to my WAVEALLOC structure in the wavehdr */
            pOutClient = (NPWAVEALLOC)LOWORD(dwUser);
            ((LPWAVEHDR)dwParam1)->reserved = (DWORD)(LPSTR)pOutClient;

            /* add the buffer to our queue */
            ((LPWAVEHDR)dwParam1)->dwFlags |= WHDR_INQUEUE;
            ((LPWAVEHDR)dwParam1)->dwFlags &= ~WHDR_DONE;
            wodWrite((LPWAVEHDR)dwParam1);

            return 0L;

        case WODM_PAUSE:
            D1("WODM_PAUSE");
            wodPause();
            AssertF(!gfDMABusy);
            return 0L;

        case WODM_RESTART:
            D1("WODM_RESTART");
            wodResume();
            return 0L;

        case WODM_RESET:
            D1("WODM_RESET");

            /* halt DMA immediately--will always set gfDMAbusy = FALSE */
            wodHaltDMA();
            wodFreeQ();          /* free all buffers */

            AssertF(!gfDMABusy);
            gfWaveOutPaused = 0;
            bBreakLoop = 0;

            /* reset byte count */
            pOutClient = (NPWAVEALLOC)LOWORD(dwUser);
            pOutClient->dwByteCount = 0L;

            return 0L;

        case WODM_BREAKLOOP:
            D1("WODM_BREAKLOOP");
            if (glpWOQueue)
                bBreakLoop = 1;
            return 0L;

        case WODM_GETPOS:
            D1("WODM_GETPOS");
            return waveGetPos(dwUser, (LPMMTIME)dwParam1, (WORD)dwParam2);

        default:
            return MMSYSERR_NOTSUPPORTED;
    }

    /* should never get here... */
    AssertF(0);
    return MMSYSERR_NOTSUPPORTED;
}
