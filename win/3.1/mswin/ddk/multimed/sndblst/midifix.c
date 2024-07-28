/****************************************************************************
 *
 *   midifix.c
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 *   NOTE - This code makes assumptions about machine architecture. It
 *      is written relying on 'Intel' lo-byte, hi-byte data storage.
 *
 ***************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "sndblst.h"

/****************************************************************************

    MIDI defines

 ***************************************************************************/

#define MIDI_DATA_FIRST             0x00
#define MIDI_DATA_LAST              0x7F
#define MIDI_STATUS_FIRST           0x80
#define MIDI_STATUS_LAST            0xFF


/* 'channel' status bytes */
#define MIDI_STATUS_CHANNEL_FIRST   0x80
#define MIDI_STATUS_CHANNEL_LAST    0xE0
#define MIDI_STATUS_CHANNEL_MASK    0xF0

/* channel voice messages */
#define MIDI_VOICE_NOTE_OFF         0x80
#define MIDI_VOICE_NOTE_ON          0x90
#define MIDI_VOICE_POLY_PRESSURE    0xA0
#define MIDI_VOICE_CONTROL_CHANGE   0xB0
#define MIDI_VOICE_PROGRAM_CHANGE   0xC0
#define MIDI_VOICE_CHANNEL_PRESSURE 0xD0
#define MIDI_VOICE_PITCH_BEND       0xE0

/* channel mode messages */
#define MIDI_MODE_CHANNEL           MIDI_VOICE_CONTROL_CHANGE


/* 'system' status bytes */
#define MIDI_STATUS_SYSTEM_FIRST    0xF0
#define MIDI_STATUS_SYSTEM_LAST     0xFF

/* system exclusive messages */
#define MIDI_SYSEX_BEGIN            0xF0
#define MIDI_SYSEX_EOX              0xF7

/* system common messages */
#define MIDI_COMMON_TCQF            0xF1    /* time code quarter frame  */
#define MIDI_COMMON_SONG_POSITION   0xF2
#define MIDI_COMMON_SONG_SELECT     0xF3
#define MIDI_COMMON_UNDEFINED_F4    0xF4
#define MIDI_COMMON_UNDEFINED_F5    0xF5
#define MIDI_COMMON_TUNE_REQUEST    0xF6

/* system real-time messages */
#define MIDI_RTIME_TIMING_CLOCK     0xF8
#define MIDI_RTIME_UNDEFINED_F9     0xF9
#define MIDI_RTIME_START            0xFA
#define MIDI_RTIME_CONTINUE         0xFB
#define MIDI_RTIME_STOP             0xFC
#define MIDI_RTIME_UNDEFINED_FD     0xFD
#define MIDI_RTIME_ACTIVE_SENSING   0xFE
#define MIDI_RTIME_SYSTEM_RESET     0xFF

/*****************************************************************************

    public data

 ****************************************************************************/ 

PORTALLOC       gMidiInClient;      /* input client information structure */
MIDIINMSGCLIENT gMIMC;              /* MIDI input msg client */

/*****************************************************************************

    local data

 ****************************************************************************/ 

static PORTALLOC    gMidiOutClient;         /* client information */
static BYTE         gbMidiOutCurrentStatus  = 0;

#ifdef DEBUG
#define D(x)    {x;}
DWORD   gdwDebugMODWriteErrors  = 0;
DWORD   gdwDebugMODataWrites    = 0;
DWORD   gdwDebugMOShortMsgs     = 0;
DWORD   gdwDebugMOShortMsgsRS   = 0;
DWORD   gdwDebugMOShortMsgsBogus= 0;
DWORD   gdwDebugMOLongMsgs      = 0;

DWORD   gdwDebugMIBytesRcvd     = 0;
DWORD   gdwDebugMIShortMsgsRcvd = 0;
DWORD   gdwDebugMILongMsgsRcvd  = 0;
WORD    gwDebugMILongErrors     = 0;
WORD    gwDebugMIShortErrors    = 0;

DWORD   gdwDebugMidiDrvCallbacks= 0;
#else
#define D(x)
#endif


#define MSGLENCHANNEL(bStatus)  gabMsgLenChannel[(BYTE)((bStatus) >> 4) - (BYTE)8]
#define MSGLENSYSTEM(bStatus)   gabMsgLenSystem[(BYTE)(bStatus - MIDI_STATUS_SYSTEM_FIRST)];

/* channel status message lengths */
static BYTE gabMsgLenChannel[] =
{
    3,      /* 0x80 note off        */
    3,      /* 0x90 note on         */
    3,      /* 0xA0 key pressure    */
    3,      /* 0xB0 control change  */
    2,      /* 0xC0 program change  */
    2,      /* 0xD0 channel pressure*/
    3       /* 0xE0 pitch bend      */
};

/** system status message lengths **/
static BYTE gabMsgLenSystem[] =
{
    1,      /* 0xF0 sysex begin     */
    2,      /* 0xF1 midi tcqf       */
    3,      /* 0xF2 song position   */
    2,      /* 0xF3 song select     */
    1,      /* 0xF4 undefined       */
    1,      /* 0xF5 undefined       */
    1,      /* 0xF6 tune request    */
    1,      /* 0xF7 sysex eox       */

    1,      /* 0xF8 timing clock    */
    1,      /* 0xF9 undefined       */
    1,      /* 0xFA start           */
    1,      /* 0xFB continue        */
    1,      /* 0xFC stop            */
    1,      /* 0xFD undefined       */
    1,      /* 0xFE active sensing  */
    1       /* 0xFF system reset    */
};

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | midiCallback | This calls DriverCallback for a midi device.
 *
 * @parm NPPORTALLOC| pPort | Pointer to the PORTALLOC.
 *
 * @parm WORD | msg | The message to send.
 *
 * @parm DWORD | dw1 | Message-dependent parameter.
 *
 * @parm DWORD | dw2 | Message-dependent parameter.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
void FAR PASCAL midiCallback(NPPORTALLOC pPort, WORD msg, DWORD dw1, DWORD dw2)
{
#ifdef DEBUG
    switch (msg) {
        case MIM_DATA:
            gdwDebugMIShortMsgsRcvd++;
            break;

        case MIM_LONGDATA:
            gdwDebugMILongMsgsRcvd++;
            break;

        case MIM_ERROR:
            D2("MIM_ERROR");
            gwDebugMIShortErrors++;
            break;

        case MIM_LONGERROR:
            D2("MIM_LONGERROR");
            gwDebugMILongErrors++;
            break;
    }
    gdwDebugMidiDrvCallbacks++;
#endif

    /*  Invoke the callback function, if it exists.  dwFlags contains driver-
     *  specific flags in the LOWORD and generic driver flags in the HIWORD
     *
     *  DON'T switch stacks in DriverCallback - we already did at the
     *  beginning of our ISR (using StackEnter).  No need to burn another
     *  stack, we should have plenty of room for the callback.  Also,
     *  we may not have been called from an ISR.  In that case, we know
     *  that we are on an app's stack, and this should be ok.
     */
    if (pPort->dwCallback)
        DriverCallback(pPort->dwCallback,       /* client's callback DWORD */
                       HIWORD(pPort->dwFlags) | DCB_NOSWITCH,  /* flags */
                       pPort->hMidi,            /* handle to the wave device */
                       msg,                     /* the message */
                       pPort->dwInstance,       /* client's instance data */
                       dw1,                     /* first DWORD */
                       dw2);                    /* second DWORD */
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | midBufferWrite | This function writes a byte into the long
 *      message buffer.  If the buffer is full or end-of-sysex byte is
 *      received, the buffer is marked as 'done' and it's owner is called
 *      back.
 *
 * @parm BYTE | byte | The byte received.
 *
 * @rdesc There is no return value
 ***************************************************************************/
static void NEAR PASCAL midBufferWrite( BYTE bByte )
{
LPMIDIHDR   lpmh;

    /* if no buffers, nothing happens */
    if ( !(lpmh = gMIMC.lpmhQueue) ) 
        return;

    /* if the long message is being terminated, only save eox byte */
    if ( (bByte < MIDI_STATUS_FIRST) || (bByte == MIDI_SYSEX_EOX) || (bByte == MIDI_SYSEX_BEGIN) ) {
        /* write the data into the long message buffer */
        *((HPSTR)(lpmh->lpData) + gMIMC.dwCurData++) = bByte;

        /* if !(end of sysex or buffer full), return */
        if ( !((bByte == MIDI_SYSEX_EOX) || (gMIMC.dwCurData >= lpmh->dwBufferLength)) )
            return;
    }

    /* send client back the data buffer */
    D4("bufferdone");
    gMIMC.lpmhQueue       = gMIMC.lpmhQueue->lpNext;
    lpmh->dwBytesRecorded = gMIMC.dwCurData;
    gMIMC.dwCurData       = 0L;
    lpmh->dwFlags        |= MHDR_DONE;
    lpmh->dwFlags        &= ~MHDR_INQUEUE;
    midiCallback( &gMidiInClient, MIM_LONGDATA, (DWORD)lpmh, gMIMC.dwMsgTime );
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | midByteRec | This function constructs the complete midi
 *      messages from the individual bytes received and passes the message
 *      to the client via his callback.
 *
 * @rdesc There is no return value
 *
 * @comm NOTE: running status is not turned off on errors
 ***************************************************************************/
void NEAR PASCAL midByteRec( BYTE bByte )
{
DWORD   dwCurTime;

    /* time byte received */
    dwCurTime = timeGetTime() - gMIMC.dwRefTime;

    D( gdwDebugMIBytesRcvd++ );

    /*  System Real-Time Messages (SRTM) range from 0xF8 to 0xFF.
     *
     *  There are no data bytes attached to SRTM status bytes and
     *  they can appear _anywhere_ in the data stream.  They should
     *  affect _nothing_.  They do not terminate SysEx, do not reset
     *  running status, nothing.  Just send them up.
     */
    if ( bByte >= MIDI_RTIME_TIMING_CLOCK ) {
        D4("rt");
        midiCallback( &gMidiInClient, MIM_DATA, (DWORD)bByte, dwCurTime );
    }

    /* if the high bit is set (>= 0x80), then it is a status byte */
    else if ( bByte >= (BYTE)MIDI_STATUS_FIRST ) {
        /*  SysEx, if going, can be terminated by either an End of 
         *  Exclusion (EOX, 0xF7) or any other Status Byte (except real-
         *  time messages which are handled above).  An EOX _should_
         *  always be sent at the end of a SysEx message, but any status
         *  byte is legal.
         */
        if ( gMIMC.fSysEx ) {
            /* bStatus should never be set if in SysEx */
            AssertT( gMIMC.bStatus );

            /* reset SysEx flag--it has been terminated */
            gMIMC.fSysEx = FALSE;

            /* post sysex data back to caller */
            midBufferWrite( bByte );

            /*  If this was an EOX, then we are done.  If it is a different
             *  status byte, then we have terminated the SysEx, but we still
             *  need to process the new status byte.
             */
            if ( bByte == MIDI_SYSEX_EOX )
                return;
        }

        /*  If there is a partially recorded short message, then post
         *  it with an error (it's considered garbage).  The first byte
         *  of the message is non-zero if partly recorded.
         */
        if ( (BYTE)gMIMC.dwShortMsg ) {
            D2("bogusshortmsg");
            midiCallback( &gMidiInClient, MIM_ERROR, gMIMC.dwShortMsg, gMIMC.dwMsgTime );
            gMIMC.dwShortMsg = 0L;
        }

        /*  The message time is always the time at which the 'status' byte
         *  for the message was received.  So set this.
         */
        gMIMC.dwMsgTime = dwCurTime;

        /*  There are two different types of messages that bByte could
         *  represent: channel messages (0x80 - 0xE0) or system messages
         *  (0xF0 - 0xFF).  We already took care of the system 'real-time
         *  messages' (0xF8 - 0xFF) above, so we don't need to check for
         *  them.
         *
         *  So is it a 'system' status byte or a 'channel' status byte?
         */
        if ( bByte >= MIDI_STATUS_SYSTEM_FIRST ) {
            /* running status applies to channel messages only */
            gMIMC.bStatus = 0;

            switch ( bByte ) {
                case MIDI_SYSEX_BEGIN:
                    D4("sysexbegin");
                    gMIMC.fSysEx = TRUE;
                    midBufferWrite( bByte );
                    break;

                case MIDI_SYSEX_EOX:
                    D4("sysexeox error");
                    gMIMC.bBytePos = 0;
                    goto midByteRecBadData;

                case MIDI_COMMON_UNDEFINED_F4:
                case MIDI_COMMON_UNDEFINED_F5:
                case MIDI_COMMON_TUNE_REQUEST:
                    D4("common0");
                    midiCallback( &gMidiInClient, MIM_DATA, (DWORD)bByte, gMIMC.dwMsgTime );
                    gMIMC.bBytePos = 0;
                    break;

                case MIDI_COMMON_TCQF:
                case MIDI_COMMON_SONG_SELECT:
                    D4("common1");
                    (BYTE)gMIMC.dwShortMsg = bByte;
                    gMIMC.bBytesLeft = 1;
                    gMIMC.bBytePos = 1;
                    break;

                case MIDI_COMMON_SONG_POSITION:
                    D4("common2");
                    (BYTE)gMIMC.dwShortMsg = bByte;
                    gMIMC.bBytesLeft = 2;
                    gMIMC.bBytePos = 1;
                    break;

#ifdef DEBUG
                default:
                    D1("VERY VERY BAD SYSTEM STATUS MSG!!!");
                    AssertT( 1 );
                    break;
#endif
            }
        }

        /* it is a 'channel' voice status byte (0x80 - 0xE0) */
        else {
            D4("voice");

            /* running status applies to channel messages only */
            gMIMC.bStatus = bByte;
            (BYTE)gMIMC.dwShortMsg = bByte;
            gMIMC.bBytePos = 1;

#ifdef DEBUG
            /* this cannot happen with the current code logic */
            if ((bByte < MIDI_STATUS_CHANNEL_FIRST) || (bByte >= MIDI_STATUS_SYSTEM_FIRST)) {
                D1("VERY VERY BAD CHANNEL STATUS MSG!!!");
                AssertT( 1 );
                gMIMC.bBytesLeft = 0;
            }
            else
#endif
            /* convert channel status byte to number of bytes remaining */
            gMIMC.bBytesLeft = MSGLENCHANNEL(bByte) - (BYTE)1;
        }
    } /* if (bByte == status byte) */

    /*  bByte is not a status byte (it is <= 0x7F and is considered a data
     *  byte).
     */
    else {
        /* if in SysEx receive mode, then record byte in long message */
        if ( gMIMC.fSysEx ) {
            D4("sx");

            /* write in long message buffer */
            midBufferWrite( bByte );
        }

        /* else if it's an expected data byte for a short message */
        else if ( gMIMC.bBytePos != 0 ) {
            D4("data");

            /* if running status */
            if ( gMIMC.bStatus && (gMIMC.bBytePos == 1) ) {
                /* setup for next short message */
                (BYTE)gMIMC.dwShortMsg = gMIMC.bStatus;
                gMIMC.dwMsgTime = dwCurTime;
            }

            /*** not portable! (like most of the code) ***/
            ((LPBYTE)&gMIMC.dwShortMsg)[ gMIMC.bBytePos++ ] = bByte;

            if ( --(gMIMC.bBytesLeft) == 0 ) {
                midiCallback( &gMidiInClient, MIM_DATA, gMIMC.dwShortMsg, gMIMC.dwMsgTime );
                gMIMC.dwShortMsg = 0L;

                if ( gMIMC.bStatus ) {
                    gMIMC.bBytesLeft = gMIMC.bBytePos - (BYTE)1;
                    gMIMC.bBytePos = 1;
                }

                else
                    gMIMC.bBytePos = 0;
            }
        }

        else {
            D2("baddata");

midByteRecBadData:
            midiCallback( &gMidiInClient, MIM_ERROR, (DWORD)bByte, gMIMC.dwMsgTime );
        }
    }
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | modSendData | This function sends RAW MIDI data; keeping
 *      track of the running status correctly.
 *
 * @rdesc The is no return value.
 *
 * @comm 
 ***************************************************************************/
static void NEAR PASCAL modSendData( HPSTR lpBuf, DWORD dwLength )
{
BYTE    bByte;

    while ( dwLength-- ) {
        bByte = *lpBuf++;

        if ( (bByte >= MIDI_STATUS_FIRST) && (bByte < MIDI_RTIME_TIMING_CLOCK) )
        {
            gbMidiOutCurrentStatus = (BYTE)((bByte < MIDI_STATUS_SYSTEM_FIRST) ? bByte : 0);
        }

#ifdef DEBUG
        if ( modDataWrite( bByte ) )
            gdwDebugMODWriteErrors++;
        else
            gdwDebugMODataWrites++;
#else
        modDataWrite( bByte );
#endif
    }
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | modSendLongData | This function sends a long message.
 *
 * @rdesc The return value is an error code (0L if success).
 ***************************************************************************/
static DWORD NEAR PASCAL modSendLongData( LPMIDIHDR lpHdr )
{
    D( gdwDebugMOLongMsgs++ );

    /*  Check if it's been prepared.  NOTE: this check is ONLY necessary
     *  for compatibility with V1.0 of MMSYSTEM.  All later versions of
     *  MMSYSTEM validate this flag before the driver is called.
     */
    if ( lpHdr->dwFlags & MHDR_PREPARED ) {
        /*  NOTE: clearing the DONE bit or setting the INQUEUE bit
         *  isn't necessary here since this function is synchronous -
         *  the client will not get control back until it's done.
         */
        modSendData( lpHdr->lpData, lpHdr->dwBufferLength );

        /* set the done bit */
        lpHdr->dwFlags |= MHDR_DONE;

        /* notify client */
        midiCallback(&gMidiOutClient, MOM_DONE, (DWORD)lpHdr, 0L);
        return 0L;
    }

    /* oops! */
    else
        return MIDIERR_UNPREPARED;
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | modSendShortMsg | This function sends a short message.
 *
 * @rdesc The return value is the number of bytes transmitted.
 ***************************************************************************/
static BYTE NEAR PASCAL modSendShortMsg( DWORD dwShortMsg )
{
BYTE    bByte   = (BYTE)dwShortMsg;
BYTE    bLength;

    /* if the short msg starts with a status msg, compute length */
    if ( bByte >= MIDI_STATUS_FIRST ) {
        bLength = (bByte < MIDI_STATUS_SYSTEM_FIRST) ? MSGLENCHANNEL(bByte) : MSGLENSYSTEM(bByte);
        D( gdwDebugMOShortMsgs++ );
    }

    /* use previous running status length */
    else if ( !gbMidiOutCurrentStatus ) {
        D( gdwDebugMOShortMsgsBogus++ );
        return 0;
    }

    /* subtract one because we don't have a status byte */
    else {
        bLength = MSGLENCHANNEL(gbMidiOutCurrentStatus) - (BYTE)1;
        D( gdwDebugMOShortMsgsRS++ );
    }

    /* send the data */
    modSendData( (HPSTR)&dwShortMsg, bLength );

    /* return number of bytes sent */
    return ( bLength );
}

/****************************************************************************
 * @doc INTERNAL
 *
 * @api void | modReset | This function turns all notes OFF.
 *
 * @rdesc There is no return value.
 ***************************************************************************/
static void NEAR PASCAL modReset( void )
{
WORD    i, j;

#ifdef DEBUG
    WORD wOldDebugLevel = wDebugLevel;

    /* for normal debugging, don't flood output with note off msgs! */
    if ( (wDebugLevel > 2) && (wDebugLevel < 5) )
        wDebugLevel = 2;
#endif

    D2("modresetBEGIN");

    /*  !!! this is not recommended by midi spec !!!
     *  send a note off to each key on each channel
     */
    for ( i = 0; i < 16; i++ ) {
        /* turn off damper pedal (sustain) */
        modSendShortMsg( (WORD)0x40B0 | i );

        /* prime the running status for 'Note Off' events */
        modSendShortMsg( 0x00400080 | i );

        /* using running status, send 'Note Off's on all patches */
        for ( j = 1; j < 128; j++ )
            modSendShortMsg( ((WORD)0x4000 | j) );
    }

    D2("modresetEND");

#ifdef DEBUG
    wDebugLevel = wOldDebugLevel;

    gdwDebugMODWriteErrors  = 0;
    gdwDebugMODataWrites    = 0;
    gdwDebugMOShortMsgs     = 0;
    gdwDebugMOShortMsgsRS   = 0;
    gdwDebugMOShortMsgsBogus= 0;
    gdwDebugMOLongMsgs      = 0;

    gdwDebugMIBytesRcvd     = 0;
    gdwDebugMIShortMsgsRcvd = 0;
    gdwDebugMILongMsgsRcvd  = 0;
    gwDebugMILongErrors     = 0;
    gwDebugMIShortErrors    = 0;

    gdwDebugMidiDrvCallbacks= 0;
#endif

    /* !!! reset running status */
    gbMidiOutCurrentStatus = 0;
}

/****************************************************************************

    This function conforms to the standard MIDI output driver message proc

 ***************************************************************************/
DWORD FAR PASCAL _loadds modMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2)
{
static WORD wMidiOutEntered = 0;        /* reentrancy check */
DWORD       dwReturn;

    if ( !gfEnabled ) {
        if ( msg == MODM_INIT ) {
            D1("MODM_INIT");
            InitDisplayConfigErrors();
            return 0L;
        }

        D1("modMessage called while disabled");
        return ( (msg == MODM_GETNUMDEVS) ? 0L : MMSYSERR_NOTENABLED );
    }

    /* this driver only supports one device */
    if ( id != 0 ) {
        D1("invalid midi device id");
        return MMSYSERR_BADDEVICEID;
    }

    switch ( msg ) {
        case MODM_GETNUMDEVS:
            D1("MODM_GETNUMDEVS");
            return 1L;

        case MODM_GETDEVCAPS:
            D1("MODM_GETDEVCAPS");
            modGetDevCaps((LPBYTE)dwParam1, (WORD)dwParam2);
            return 0L;

        case MODM_OPEN:
            D1("MODM_OPEN");

            /* now attempt to 'acquire' the MIDI output hardware */
            if ( modAcquireHardware() ) {
                D1("MIDI output hardware is unavailable!");
                return MMSYSERR_ALLOCATED;
            }

            /* save client information */
            gMidiOutClient.dwCallback = ((LPMIDIOPENDESC)dwParam1)->dwCallback;
            gMidiOutClient.dwInstance = ((LPMIDIOPENDESC)dwParam1)->dwInstance;
            gMidiOutClient.hMidi      = ((LPMIDIOPENDESC)dwParam1)->hMidi;
            gMidiOutClient.dwFlags    = dwParam2;

            /* !!! reset running status */
            gbMidiOutCurrentStatus = 0;
            
            /* notify client */
            midiCallback(&gMidiOutClient, MOM_OPEN, 0L, 0L);

            return 0L;

        case MODM_CLOSE:
            D1("MODM_CLOSE");

            /* notify client */
            midiCallback(&gMidiOutClient, MOM_CLOSE, 0L, 0L);

            /* now 'release' the MIDI output hardware */
            if ( modReleaseHardware() ) {
                D1("MIDI output hardware could NOT be released!");
            }

            return 0L;

        case MODM_RESET:
            D1("MODM_RESET");

            /* make sure we're not being reentered */
            wMidiOutEntered++;
            {
                if ( wMidiOutEntered != 1 ) {
                    D1("MODM_DATA reentered!");
                    dwReturn = MIDIERR_NOTREADY;
                }

                else {
                    /* turn all notes off */
                    modReset();
                    dwReturn = 0L;
                }
            }
            wMidiOutEntered--;
            return ( dwReturn );

        case MODM_DATA:             /* message is in dwParam1 */
            D4("MODM_DATA");

            /* make sure we're not being reentered */
            wMidiOutEntered++;
            {
                if ( wMidiOutEntered != 1 ) {
                    D1("MODM_DATA reentered!");
                    dwReturn = MIDIERR_NOTREADY;
                }

                else {
                    modSendShortMsg( dwParam1 );
                    dwReturn = 0L;
                }
            }
            wMidiOutEntered--;
            return ( dwReturn );

        case MODM_LONGDATA:         /* far pointer to header in dwParam1 */
            D4("MODM_LONGDATA");

            /* make sure we're not being reentered */
            wMidiOutEntered++;
            {
                if ( wMidiOutEntered != 1 ) {
                    D1("MODM_LONGDATA reentered!");
                    dwReturn = MIDIERR_NOTREADY;
                }

                else
                    dwReturn = modSendLongData( (LPMIDIHDR)dwParam1 );
            }
            wMidiOutEntered--;
            return ( dwReturn );

        default:
            return MMSYSERR_NOTSUPPORTED;
    }

    /* should never get here */
    AssertF(0);
    return MMSYSERR_NOTSUPPORTED;
}
