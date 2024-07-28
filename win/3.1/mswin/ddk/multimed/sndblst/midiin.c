/****************************************************************************
 *
 *   midiin.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "sndblst.h"

extern MIDIINMSGCLIENT  gMIMC;          /* MIDI input msg client */

/*****************************************************************************

    internal function prototypes

 ****************************************************************************/ 

static void NEAR PASCAL midFreeQ(void);
static void NEAR PASCAL midGetDevCaps(LPBYTE lpCaps, WORD wSize);

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | midFreeQ | Free all buffers in the MIQueue.
 *
 * @comm Currently this is only called after sending off any partially filled
 *     buffers, so all buffers here are empty.  The timestamp value is 0 in
 *     this case.
 *
 * @rdesc There is no return value.
 ***************************************************************************/ 
static void NEAR PASCAL midFreeQ(void)
{
LPMIDIHDR   lpH, lpN;
DWORD       dwTime;

    lpH = gMIMC.lpmhQueue;              /* point to top of the queue */
    gMIMC.lpmhQueue = NULL;             /* mark the queue as empty */
    gMIMC.dwCurData = 0L;

    dwTime = timeGetTime() - gMIMC.dwRefTime;

    while (lpH) {
        lpN = lpH->lpNext;
        lpH->dwFlags |= MHDR_DONE;
        lpH->dwFlags &= ~MHDR_INQUEUE;
        lpH->dwBytesRecorded = 0;
        midiCallback(&gMidiInClient, MIM_LONGDATA, (DWORD)lpH, dwTime);
        lpH = lpN;
    }
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | midAddBuffer | This function adds a buffer to the list of
 *      wave input buffers.
 *
 * @parm LPWAVEHDR | lpWIHdr | Far pointer to a wave input data header.
 *
 * @rdesc The return value is an MMSYS error code (0L if success).
 *
 * @comm We assume that the header and block are both page locked when they
 *      get here.  That is, it has been 'prepared.'
 ***************************************************************************/ 
static DWORD NEAR PASCAL midAddBuffer( LPMIDIHDR lpmh )
{
LPMIDIHDR   lpN;

    /* check if it's been prepared */
    if (!(lpmh->dwFlags & MHDR_PREPARED))
        return MIDIERR_UNPREPARED;

    /* check if it's in our queue already */
    if (lpmh->dwFlags & MHDR_INQUEUE)
        return MIDIERR_STILLPLAYING;

    /* add the buffer to our queue */
    lpmh->dwFlags |= MHDR_INQUEUE;
    lpmh->dwFlags &= ~MHDR_DONE;

    /* sanity */
    lpmh->dwBytesRecorded = 0;
    lpmh->lpNext = NULL;

    CritEnter();
    {
        if ( lpN = gMIMC.lpmhQueue ) {
            while ( lpN->lpNext && (lpN = lpN->lpNext) )
                ;

            lpN->lpNext = lpmh;
        }

        else 
            gMIMC.lpmhQueue = lpmh;
    }
    CritLeave();

    /* return success */
    return ( 0L );
}

/****************************************************************************
 * @doc INTERNAL
 * 
 * @api WORD | midSendPartBuffer | This function is called from midStop().
 *     It looks at the buffer at the head of the queue and, if it contains
 *     any data, marks it as done as sends it back to the client.
 * 
 * @rdesc The return value is the number of bytes transfered. A value of zero
 *     indicates that there was no more data in the input queue.
 ***************************************************************************/ 
void NEAR PASCAL midSendPartBuffer(void)
{
LPMIDIHDR lpH;

    if ( gMIMC.lpmhQueue && gMIMC.dwCurData ) {
        lpH = gMIMC.lpmhQueue;
        gMIMC.lpmhQueue = gMIMC.lpmhQueue->lpNext;
        gMIMC.dwCurData = 0L;
        lpH->dwFlags |= MHDR_DONE;
        lpH->dwFlags &= ~MHDR_INQUEUE;
        midiCallback(&gMidiInClient, MIM_LONGERROR, (DWORD)lpH,gMIMC.dwMsgTime);
    }
}

/*****************************************************************************
 * @doc INTERNAL
 *
 * @api void | midGetDevCaps | Get the capabilities of the port.
 *
 * @parm LPBYTE | lpCaps | Far pointer to a MIDIINCAPS structure.
 *
 * @parm WORD | wSize | Size of the MIDIINCAPS structure.
 *
 * @rdesc There is no return value.
 ****************************************************************************/
static void NEAR PASCAL midGetDevCaps(LPBYTE lpCaps, WORD wSize)
{
MIDIINCAPS mc;

    mc.wMid = MM_MICROSOFT;
    mc.wPid = MM_SNDBLST_MIDIIN;
    mc.vDriverVersion = DRIVER_VERSION;
    LoadString(ghModule, IDS_SNDBLSTMIDIIN, mc.szPname, MAXPNAMELEN);

    MemCopy(lpCaps, &mc, min(wSize, sizeof(MIDIINCAPS)));
}

/****************************************************************************

    This function conforms to the standard MIDI input driver message proc

****************************************************************************/
DWORD FAR PASCAL _loadds midMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2)
{
    if ( !gfEnabled ) {
        if ( msg == MIDM_INIT ) {
            D1("MIDM_INIT");
            InitDisplayConfigErrors();
            return 0L;
        }

        D1("midMessage called while disabled");
        return ( (msg == MIDM_GETNUMDEVS) ? 0L : MMSYSERR_NOTENABLED );
    }

    /* this driver only supports one device */
    if ( id != 0 ) {
        D1("invalid midi device id");
        return MMSYSERR_BADDEVICEID;
    }

    switch ( msg ) {
        case MIDM_GETNUMDEVS:
            D1("MIDM_GETNUMDEVS");
            return 1L;

        case MIDM_GETDEVCAPS:
            D1("MIDM_GETDEVCAPS");
            midGetDevCaps((LPBYTE)dwParam1, (WORD)dwParam2);
            return 0L;

        case MIDM_OPEN:
            D1("MIDM_OPEN");

            /* now attempt to 'acquire' the MIDI input hardware */
            if ( midAcquireHardware() ) {
                D1("MIDI input hardware is unavailable!");
                return MMSYSERR_ALLOCATED;
            }

            /* allocate my structure containing info about my client (global */
            /* because I only allow one client to access midi input). */
            gMidiInClient.dwCallback = ((LPMIDIOPENDESC)dwParam1)->dwCallback;
            gMidiInClient.dwInstance = ((LPMIDIOPENDESC)dwParam1)->dwInstance;
            gMidiInClient.hMidi      = ((LPMIDIOPENDESC)dwParam1)->hMidi;
            gMidiInClient.dwFlags    = dwParam2;
            
            /* initialize queue stuff */
            gMIMC.dwCurData = 0;
            gMIMC.lpmhQueue = 0;

            /*  NOTE: we must initialize reference time in case someone adds */
            /*  longdata buffers after opening, then resets the midi stream */
            /*  without starting midi input.  Otherwise, midFreeQ would give */
            /*  inconsistent timestamps */
            gMIMC.dwRefTime = timeGetTime();

            /* notify client */
            midiCallback(&gMidiInClient, MIM_OPEN, 0L, 0L);

            return 0L;

        case MIDM_CLOSE:
            D1("MIDM_CLOSE");

            if ( gMIMC.lpmhQueue )
                return MIDIERR_STILLPLAYING;

            /* just in case they started input without adding buffers */
            midStop();

            /* now 'release' the MIDI input hardware */
            if ( midReleaseHardware() ) {
                D1("MIDI input hardware could NOT be released!");
            }

            /* notify client */
            midiCallback(&gMidiInClient, MIM_CLOSE, 0L, 0L);

            return 0L;

        case MIDM_ADDBUFFER:
            D1("MIDM_ADDBUFFER");

            /* attempt to add the buffer */
            return midAddBuffer((LPMIDIHDR)dwParam1);

        case MIDM_START:
            D1("MIDM_START");

            /* initialize all the parsing status variables */
            gMIMC.fSysEx = 0;
            gMIMC.bStatus = 0;
            gMIMC.bBytesLeft = 0;
            gMIMC.bBytePos = 0;
            gMIMC.dwShortMsg = 0;
            gMIMC.dwMsgTime = 0;
            gMIMC.dwRefTime = 0;
            gMIMC.dwCurData = 0;

            /* get a new reference time */
            gMIMC.dwRefTime = timeGetTime();

            midStart();
            return 0L;

        case MIDM_STOP:
            D1("MIDM_STOP");
            midStop();
            return 0L;

        case MIDM_RESET:
            D1("MIDM_RESET");

            /* stop if it is started and release all buffers */
            midStop();
            midFreeQ();
            return 0L;

        default:
            return MMSYSERR_NOTSUPPORTED;
    }

    /* should never get here */
    AssertF(0);
    return MMSYSERR_NOTSUPPORTED;
}
