//---------------------------------------------------------------------------
//
//  Module:   midifix.c
//
//  Description:
//      FIXED midi routines
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>

#include "msmpu401.h"

//
// MIDI defines...
//

#define MIDI_DATA_FIRST             0x00
#define MIDI_DATA_LAST              0x7F
#define MIDI_STATUS_FIRST           0x80
#define MIDI_STATUS_LAST            0xFF


/** 'channel' status bytes **/
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


/** 'system' status bytes **/
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

#ifdef DEBUG
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
#endif


#define MSGLENCHANNEL(bStatus)  gabMsgLenChannel[(BYTE)((bStatus) >> 4) - (BYTE)8]
#define MSGLENSYSTEM(bStatus)   gabMsgLenSystem[(BYTE)(bStatus - MIDI_STATUS_SYSTEM_FIRST)];


/** channel status message lengths **/
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


//--------------------------------------------------------------------------
//  
//  void midiCallback
//  
//  Description:
//  
//  
//  Parameters:
//      PPORTALLOC pPort
//  
//      WORD msg
//  
//      DWORD dw1
//  
//      DWORD dw2
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void FAR PASCAL midiCallback
(
    PPORTALLOC      pPort,
    WORD            msg,
    DWORD           dw1,
    DWORD           dw2
)
{
#ifdef DEBUG
    switch (msg)
    {
        case MIM_DATA:
            gdwDebugMIShortMsgsRcvd++;
            break;

        case MIM_LONGDATA:
            gdwDebugMILongMsgsRcvd++;
            break;

        case MIM_ERROR:
            DPF(2, "MIM_ERROR") ;
            gwDebugMIShortErrors++;
            break;

        case MIM_LONGERROR:
            DPF( 2, "MIM_LONGERROR" ) ;
            gwDebugMILongErrors++;
            break;
    }
    gdwDebugMidiDrvCallbacks++;
#endif

    // invoke the callback function, if it exists.  dwFlags contains driver-
    // specific flags in the LOWORD and generic driver flags in the HIWORD

    if (HIWORD(pPort->dwFlags))
        DriverCallback(pPort->dwCallback,       // client's callback DWORD
                       HIWORD(pPort->dwFlags),  // callback flags
                       pPort->hMidi,            // handle to the wave device
                       msg,                     // the message
                       pPort->dwInstance,       // client's instance data
                       dw1,                     // first DWORD
                       dw2);                    // second DWORD
} // midiCallback()


//--------------------------------------------------------------------------
//  
//  void midBufferWrite
//  
//  Description:
//      This function writes a byte into the long message buffer.  If
//      the buffer is full or end-of-sysex byte is received, the buffer
//      is marked as 'done' and it's owner is called back.
//  
//  Parameters:
//      PMIDIINCLIENT pmic
//  
//      BYTE bByte
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void NEAR PASCAL midBufferWrite
(
    PMIDIINCLIENT   pmic,
    BYTE            bByte
)
{
    LPMIDIHDR   lpmh;

    // if no buffers, nothing happens

    if (!(lpmh = pmic -> lpmhQueue)) 
        return ;

    // If the long message is being terminated, only save eox byte

    if ((bByte < MIDI_STATUS_FIRST) || 
        (bByte == MIDI_SYSEX_EOX) || 
        (bByte == MIDI_SYSEX_BEGIN))
    {
       // write the data into the long message buffer

       *((HPSTR)(lpmh->lpData) + pmic -> dwCurData++) = bByte ;

       // if !(end of sysex or buffer full), return
       if (!((bByte == MIDI_SYSEX_EOX) || 
             (pmic -> dwCurData >= lpmh -> dwBufferLength)))
          return ;
    }

    // send client back the data buffer

    DPF( 4, "bufferdone" ) ;

    pmic -> lpmhQueue  = pmic -> lpmhQueue -> lpNext ;
    lpmh->dwBytesRecorded = pmic -> dwCurData ;
    pmic -> dwCurData  = 0L ;
    lpmh -> dwFlags      |= MHDR_DONE ;
    lpmh -> dwFlags      &= ~MHDR_INQUEUE ;

    midiCallback( (PPORTALLOC) pmic, MIM_LONGDATA, 
                  (DWORD)lpmh, pmic -> dwMsgTime ) ;

} // midBufferWrite()

//--------------------------------------------------------------------------
//  
//  void midByteRec
//  
//  Description:
//      This function constructs the complete midi messages from the
//      individual bytes received and passes the message to the client
//      via his callback.
//  
//  Parameters:
//      PMIDIINCLIENT pmic
//  
//      BYTE bByte
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void NEAR PASCAL midByteRec
(
    PMIDIINCLIENT       pmic,
    BYTE                bByte
)
{
   DWORD   dwCurTime ;

   // time byte received...

   dwCurTime = timeGetTime() - pmic -> dwRefTime;

   D( 
      gdwDebugMIBytesRcvd++ ;
   )

   //
   // System Real-Time Messages (SRTM) range from 0xF8 to 0xFF.
   //
   // There are no data bytes attached to SRTM status bytes and
   // they can appear _anywhere_ in the data stream.  They should
   // affect _nothing_.  They do not terminate SysEx, do not reset
   // running status, nothing.  Just send them up.
   //
   
   if (bByte >= MIDI_RTIME_TIMING_CLOCK)
   {
      DPF( 4, "rt" ) ;
      midiCallback( (PPORTALLOC) pmic, MIM_DATA, (DWORD)bByte, dwCurTime ) ;
   }
   else if ( bByte >= (BYTE)MIDI_STATUS_FIRST )
   {
      // if the high bit is set (>= 0x80), then it is a status byte...

      //
      // SysEx, if going, can be terminated by either an End of 
      // Exclusion (EOX, 0xF7) or any other Status Byte (except real-
      // time messages which are handled above).  An EOX _should_
      // always be sent at the end of a SysEx message, but any status
      // byte is legal...
      //
      
      if (pmic -> fSysEx)
      {
         // bStatus should never be set if in SysEx...
         assert ( pmic -> bStatus );

         // reset SysEx flag--it has been terminated
         pmic -> fSysEx = FALSE ;

         // post sysex data back to caller...

         midBufferWrite( pmic, bByte ) ;

         //
         // If this was an EOX, then we are done.  If it is a different
         // status byte, then we have terminated the SysEx, but we still
         // need to process the new status byte...
         //

         if (bByte == MIDI_SYSEX_EOX)
            return ;
      }

      //
      // If there is a partially recorded short message, then post
      // it with an error (it's considered garbage).  The first byte
      // of the message is non-zero if partly recorded...
      //

      if ((BYTE) pmic -> dwShortMsg)
      {
         DPF( 2, "bogusshortmsg" ) ;
         midiCallback( (PPORTALLOC) pmic, MIM_ERROR, 
                       pmic -> dwShortMsg, pmic -> dwMsgTime );
         pmic -> dwShortMsg = 0L ;
      }

      //
      // The message time is always the time at which the 'status' byte
      // for the message was received.  So set this.
      //
     
      pmic -> dwMsgTime = dwCurTime ;

      //
      // There are two different types of messages that bByte could
      // represent: channel messages (0x80 - 0xE0) or system messages
      // (0xF0 - 0xFF).  We already took care of the system 'real-time
      // messages' (0xF8 - 0xFF) above, so we don't need to check for
      // them.
      //

      // So is it a 'system' status byte or a 'channel' status byte?
     
      if (bByte >= MIDI_STATUS_SYSTEM_FIRST)
      {
         // running status applies to channel messages only

         pmic -> bStatus = 0 ;

         switch ( bByte )
         {
            case MIDI_SYSEX_BEGIN:
               DPF( 4, "sysexbegin" ) ;
               pmic -> fSysEx = TRUE ;
               midBufferWrite( pmic, bByte ) ;
               break;

            case MIDI_SYSEX_EOX:
               DPF( 4, "sysexeox error" ) ;
               pmic -> bBytePos = 0 ;
               midiCallback( (PPORTALLOC) pmic, MIM_ERROR, 
                            (DWORD) bByte, pmic -> dwMsgTime ) ;
               break ;

            case MIDI_COMMON_UNDEFINED_F4:
            case MIDI_COMMON_UNDEFINED_F5:
            case MIDI_COMMON_TUNE_REQUEST:
               DPF( 4, "common0" ) ;
               midiCallback( (PPORTALLOC) pmic, MIM_DATA, 
                             (DWORD)bByte, pmic -> dwMsgTime );
               pmic -> bBytePos = 0 ;
               break ;

            case MIDI_COMMON_TCQF:
            case MIDI_COMMON_SONG_SELECT:
               DPF( 4, "common1" ) ;

               (BYTE) pmic -> dwShortMsg = bByte ;
               pmic -> bBytesLeft = 1 ;
               pmic -> bBytePos = 1 ;
               break ;

            case MIDI_COMMON_SONG_POSITION:
               DPF( 4, "common2" ) ;
               (BYTE)pmic -> dwShortMsg = bByte ;
               pmic -> bBytesLeft = 2 ;
               pmic -> bBytePos = 1 ;
               break;

          #ifdef DEBUG
            default:
               DPF( 1, "VERY VERY BAD SYSTEM STATUS MSG!!!" ) ;
               INLINE_BREAK;
               break;
          #endif
         }

      }
      else
      {
         // it is a 'channel' voice status byte (0x80 - 0xE0)

         DPF( 4, "voice" ) ;
                          
         // running status applies to channel messages only

         pmic -> bStatus = bByte ;
         (BYTE) pmic -> dwShortMsg = bByte ;
         pmic -> bBytePos = 1 ;

        #ifdef DEBUG
         // this cannot happen with the current code logic
         if ((bByte < MIDI_STATUS_CHANNEL_FIRST) || 
             (bByte >= MIDI_STATUS_SYSTEM_FIRST))
         {
            DPF( 1, "VERY VERY BAD CHANNEL STATUS MSG!!!" ) ;
            INLINE_BREAK ;
            pmic -> bBytesLeft = 0 ;
         }
         else
        #endif
         // convert channel status byte to number of bytes remaining...

         pmic -> bBytesLeft = MSGLENCHANNEL(bByte) - (BYTE)1 ;
      }
   }
   else
   {
      //
      // bByte is not a status byte (it is <= 0x7F and is considered
      // a data byte).
      //

      // if in SysEx receive mode, then record byte in long message
      if (pmic -> fSysEx)
      {
         DPF( 4, "sx" ) ;

         // write in long message buffer

         midBufferWrite( pmic, bByte ) ;
      }
      else if ( pmic -> bBytePos != 0 )
      {
         // else if it's an expected data byte for a short message

         DPF( 4, "data");

         // if running status

         if (pmic -> bStatus && (pmic -> bBytePos == 1))
         {
            //* setup for next short message

            (BYTE)pmic -> dwShortMsg = pmic -> bStatus ;
            pmic -> dwMsgTime = dwCurTime ;
         }

         // not portable! (like most of the code)

         ((LPBYTE)&pmic -> dwShortMsg)[ pmic -> bBytePos++ ] = bByte ;

         if (--(pmic -> bBytesLeft) == 0)
         {
            midiCallback( (PPORTALLOC) pmic, MIM_DATA, 
                          pmic -> dwShortMsg, pmic -> dwMsgTime ) ;
            pmic -> dwShortMsg = 0L;

            if ( pmic -> bStatus )
            {
               pmic -> bBytesLeft = pmic -> bBytePos - (BYTE)1;
               pmic -> bBytePos = 1;
            }

            else
               pmic -> bBytePos = 0;
         }
      }
      else
      {
         DPF( 2, "baddata" ) ;

         midiCallback( (PPORTALLOC) pmic, MIM_ERROR, 
                       (DWORD) bByte, pmic -> dwMsgTime );
      }
   }

} // midByteRec()

//===========================================================================
//===========================================================================

//--------------------------------------------------------------------------
//  
//  void modSendData
//  
//  Description:
//      This function sends RAW MIDI data while keeping track of the
//      running status.
//  
//  Parameters:
//      PMIDIOUTCLIENT pmoc
//  
//      HPSTR lpBuf
//  
//      DWORD dwLength
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void NEAR PASCAL modSendData
(
    PMIDIOUTCLIENT      pmoc,
    HPSTR               lpBuf,
    DWORD               dwLength
)
{
   BYTE  bByte ;

   while (dwLength--)
   {
      bByte = *lpBuf++;

      if ( (bByte >= MIDI_STATUS_FIRST) && (bByte < MIDI_RTIME_TIMING_CLOCK) )
      {
         pmoc -> bCurrentStatus = 
            (BYTE)((bByte < MIDI_STATUS_SYSTEM_FIRST) ? bByte : 0) ;
      }

#ifdef DEBUG
      if (modDataWrite( pmoc -> pa.phwi, bByte ))
         gdwDebugMODWriteErrors++ ;
      else
         gdwDebugMODataWrites++ ;
#else
      modDataWrite( pmoc -> pa.phwi, bByte );
#endif
   }

} // modSendData()

//--------------------------------------------------------------------------
//  
//  DWORD modSendLongData
//  
//  Description:
//      This function sends a long message.
//  
//  Parameters:
//      PMIDIOUTCLIENT pmoc
//  
//      LPMIDIHDR lpHdr
//  
//  Return (DWORD):
//  
//--------------------------------------------------------------------------

DWORD NEAR PASCAL modSendLongData
(
    PMIDIOUTCLIENT      pmoc,
    LPMIDIHDR           lpHdr
)
{
   D(
      gdwDebugMOLongMsgs++ ;
   )

   //
   // check if it's been prepared.  NOTE: this check is ONLY necessary
   // for compatibility with V1.0 of MMSYSTEM.  All later versions of
   // MMSYSTEM validate this flag before the driver is called.
   //
   
   if (lpHdr->dwFlags & MHDR_PREPARED)
   {
      // NOTE: clearing the done bit or setting the inqueue bit
      // isn't necessary here since this function is synchronous -
      // the client will not get control back until it's done.
      
      modSendData( pmoc, lpHdr -> lpData, lpHdr -> dwBufferLength ) ;

      // Set the done bit...

      lpHdr -> dwFlags |= MHDR_DONE ;

      // Notify client

      midiCallback( (PPORTALLOC) pmoc, MOM_DONE, (DWORD)lpHdr, 0L ) ;
      return MMSYSERR_NOERROR ;
   }
   else
      return MIDIERR_UNPREPARED ;

} // modSendLongData()

//--------------------------------------------------------------------------
//  
//  BYTE modSendShortMsg
//  
//  Description:
//      This function sends a short message.
//  
//  Parameters:
//      PMIDIOUTCLIENT pmoc
//  
//      DWORD dwShortMsg
//  
//  Return (BYTE):
//  
//--------------------------------------------------------------------------

BYTE NEAR PASCAL modSendShortMsg
(
    PMIDIOUTCLIENT      pmoc,
    DWORD               dwShortMsg
)
{
    BYTE    bByte = (BYTE) dwShortMsg ;
    BYTE    bLength;

    // if the short msg starts with a status msg, compute length..

    if (bByte >= MIDI_STATUS_FIRST)
    {
      bLength = (bByte < MIDI_STATUS_SYSTEM_FIRST) ? MSGLENCHANNEL(bByte) : MSGLENSYSTEM(bByte);
      D(
         gdwDebugMOShortMsgs++ ;
      )
    }
    else if (!pmoc -> bCurrentStatus)
    {
       // use previous running status length...

       D(
          gdwDebugMOShortMsgsBogus++ ;
       )

       return 0 ;
    }
    else
    {
       // Subtract one because we don't have a status byte

       bLength = MSGLENCHANNEL(pmoc -> bCurrentStatus) - (BYTE)1;
       D(
          gdwDebugMOShortMsgsRS++ ;
       )
    }

    // Send the data...

    modSendData( pmoc, (HPSTR)&dwShortMsg, bLength ) ;

    // return number of bytes sent

    return bLength ;

} // modSendShortMsg()

//--------------------------------------------------------------------------
//  
//  void modReset
//  
//  Description:
//      This function turns all notes OFF.
//  
//  Parameters:
//      PMIDIOUTCLIENT pmoc
//  
//  Return (void):
//  
//--------------------------------------------------------------------------

void NEAR PASCAL modReset
(
    PMIDIOUTCLIENT      pmoc
)
{
    WORD    i, j;

#ifdef DEBUG
    WORD wOldDebugLevel = wDebugLevel;

    /* for normal debugging, don't flood output with note off msgs! */
    if ( (wDebugLevel > 2) && (wDebugLevel < 5) )
        wDebugLevel = 2;
#endif

    DPF( 2, "modresetBEGIN" ) ;

    //
    //  !!! this is not recommended by midi spec !!!
    //  send a note off to each key on each channel
    //

    for (i = 0; i < 16; i++)
    {
       // turn off damper pedal (sustain)

       modSendShortMsg( pmoc, (WORD)0x40B0 | i ) ;

       // prime the running status for 'Note Off' events

       modSendShortMsg( pmoc, 0x00400080 | i ) ;

       // using running status, send 'Note Off's on all patches

       for (j = 1; j < 128; j++)
          modSendShortMsg( pmoc, ((WORD)0x4000 | j) ) ;
    }

    DPF( 2, "modresetEND" ) ;

#ifdef DEBUG
    wDebugLevel = wOldDebugLevel ;

    gdwDebugMODWriteErrors  = 0 ;
    gdwDebugMODataWrites    = 0 ;
    gdwDebugMOShortMsgs     = 0 ;
    gdwDebugMOShortMsgsRS   = 0 ;
    gdwDebugMOShortMsgsBogus= 0 ;
    gdwDebugMOLongMsgs      = 0 ;

    gdwDebugMIBytesRcvd     = 0 ;
    gdwDebugMIShortMsgsRcvd = 0 ;
    gdwDebugMILongMsgsRcvd  = 0 ;
    gwDebugMILongErrors     = 0 ;
    gwDebugMIShortErrors    = 0 ;

    gdwDebugMidiDrvCallbacks= 0 ;
#endif

    // !!! reset running status...

    pmoc -> bCurrentStatus = 0 ;

} // modReset()

//--------------------------------------------------------------------------
//  
//  LRESULT modOpen
//  
//  Description:
//      This should be called when a midi file is opened.
//      It initializes some variables and client structure.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance structure
//
//      LPDWORD pdwUser
//         pointer to user return
//
//      LPMIDIOPENDESC pod
//         pointer to open description structure
//
//      DWORD dwFlags
//         options
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if successful, else error
//  
//--------------------------------------------------------------------------

#pragma optimize( "leg", off )

LRESULT FAR PASCAL modOpen
(
    PHARDWAREINSTANCE   phwi,
    LPDWORD             pdwUser,
    LPMIDIOPENDESC      pod,
    DWORD               dwFlags
)
{
   WORD           wFlags ;
   PMIDIOUTCLIENT pmoc ;

   DPF( 3, "modOpen" ) ;

   if (phwi -> bMidiOutFlags & MOF_ALLOCATED)
      return MMSYSERR_ALLOCATED ;

   if (!AcquireMPU401( phwi ))
      return MMSYSERR_ALLOCATED ;

   pmoc = (PMIDIOUTCLIENT) LocalAlloc( LPTR, sizeof( MIDIOUTCLIENT ) ) ;
   if (NULL == pmoc)
   {   ReleaseMPU401( phwi ) ;
       return MMSYSERR_NOMEM ;
   }

   // save client information

   ((PPORTALLOC) pmoc) -> dwCallback = pod -> dwCallback ;
   ((PPORTALLOC) pmoc) -> dwInstance = pod -> dwInstance ;
   ((PPORTALLOC) pmoc) -> hMidi      = pod -> hMidi ;
   ((PPORTALLOC) pmoc) -> dwFlags    = dwFlags ;
   ((PPORTALLOC) pmoc) -> phwi       = phwi ;

   #ifdef MIDI_STREAMS
    {
    LRESULT mmr = modStreamOpen (pmoc, 0, pod);
    if (mmr)
       {
       LocalFree ((HLOCAL)pmoc);
       ReleaseMPU401 (phwi);
       return mmr;
       }
    }
   #endif

   // init the midi out specific data...

   pmoc -> bCurrentStatus = 0 ;
   pmoc -> wMidiOutEntered = 0 ;

   phwi -> pmoc = pmoc ;

   *pdwUser = MAKELONG( (WORD) pmoc, NULL ) ;

   phwi -> bMidiOutFlags |= MOF_ALLOCATED ;

   if (phwi -> fnisrPipe)
   {
      _asm
      {
         pushf
         pop   wFlags
         cli
      }

      phwi -> fnisrPipe( phwi -> hpisr, 
                         PIPE_MSG_CONTROL |
                            MSMPU401_CTL_SET_ISR,
                         phwi -> dn,
                         (DWORD) MPU401InterruptHandler ) ;

      phwi -> fnisrPipe( phwi -> hpisr, 
                         PIPE_MSG_CONTROL |
                            MSMPU401_CTL_SET_INSTANCE,
                         phwi -> dn,
                         MAKELONG( (WORD) phwi, 0 ) ) ;
      _asm
      {
         mov   ax, wFlags
         test  ah, 2
         jz    SHORT No_Enable
         sti

      No_Enable:
      }
   }

   // notify client

   midiCallback( (PPORTALLOC) pmoc, MOM_OPEN, 0L, 0L ) ;

   return MMSYSERR_NOERROR ;

} // modOpen()

//--------------------------------------------------------------------------
//  
//  VOID modClose
//  
//  Description:
//      Closes the MIDI-in device.
//  
//  Parameters:
//      PMIDIOUTCLIENT pmoc
//         pointer to client structure
//  
//  Return (VOID):
//      Nothing.
//  
//--------------------------------------------------------------------------

VOID FAR PASCAL modClose
(
    PMIDIOUTCLIENT        pmoc
)
{
   PHARDWAREINSTANCE phwi ;
   WORD              wFlags ;

   DPF( 3, "modClose" ) ;

   #ifdef MIDI_STREAMS
    modStreamClose (pmoc);
   #endif

   phwi = pmoc -> pa.phwi ;
   phwi -> pmoc = NULL ;

   ReleaseMPU401( phwi ) ;

   phwi -> bMidiOutFlags &= ~MOF_ALLOCATED ;

   //
   // notify client
   //

   midiCallback( (PPORTALLOC) pmoc, MOM_CLOSE, 0L, 0L ) ;

   if (phwi -> fnisrPipe)
   {
      _asm
      {
         pushf
         pop   wFlags
         cli
      }

      phwi -> fnisrPipe( phwi -> hpisr, 
                         PIPE_MSG_CONTROL | 
                            MSMPU401_CTL_SET_ISR,
                         phwi -> dn,
                         NULL ) ;
      _asm
      {
         mov   ax, wFlags
         test  ah, 2
         jz    SHORT No_Enable
         sti

      No_Enable:
      }
   }

   LocalFree( (HLOCAL) pmoc ) ;

} // modClose()

#pragma optimize( "", on )

//--------------------------------------------------------------------------
//  
//  LRESULT modMessage
//  
//  Description:
//     This function conforms to the standard MIDI output driver
//     message proc modMessage, which is documented in mmddk.h.
//  
//  Parameters:
//      UINT uDevId
//         device id
//
//      UINT msg
//         driver message
//  
//      DWORD dwUser
//         user info
//  
//      DWORD dwParam1
//         message specific
//  
//      DWORD dwParam2
//         message specific
//  
//  Return (LRESULT):
//      Error status
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL _loadds modMessage
(
    UINT            uDevId,
    UINT            uMsg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
)
{
   PMIDIOUTCLIENT      pmoc ;
   PHARDWAREINSTANCE   phwi ;
   DWORD               dwReturn ;

   // take care of init time messages...

   switch (uMsg)
   {
      case MODM_INIT:
      {
         DPF( 1, "MODM_INIT" ) ;

         // dwParam2 == PnP DevNode

         return (AddDevNode( dwParam2 )) ;
      }
      break ;

      case DRVM_ENABLE:
      {
         UINT   uVxDId ;
         ULONG  cIds ;

         DPF( 3, "MODM_ENABLE" ) ;

         // dwParam2 == PnP DevNode

         // Query the supporting VxD ID

         cIds = 1 ;

         if (midiOutMessage( (HMIDIOUT) dwParam1, DRV_QUERYDRIVERIDS,
                             (DWORD) (LPWORD) &uVxDId, 
                             (DWORD) (LPDWORD) &cIds ))
            return MMSYSERR_INVALPARAM ;

         return (EnableDevNode( dwParam2, uVxDId )) ;

      }
      break ;

      case DRVM_DISABLE:
      {
         DPF( 3, "MODM_DISABLE" ) ;

         // dwParam2 == PnP DevNode

         return (DisableDevNode( dwParam2 )) ;

      }
      break ;

      case DRVM_EXIT:
      {
         DPF( 3, "MODM_EXIT" ) ;

         // dwParam2 == PnP DevNode

         return (RemoveDevNode( dwParam2 )) ;

      }
      break ;

      case MODM_GETNUMDEVS:
      {
         DPF( 1, "MODM_GETNUMDEVS" ) ;

         if (NULL == (phwi = DevNodeToHardwareInstance( dwParam1 )))
            return MAKELONG( 0, MMSYSERR_INVALPARAM ) ;

         if (phwi -> fEnabled)
            return 1L ;
         else
            return 0L ;
      }
      break ;

      case MODM_OPEN:
      {
         DWORD  dn ;

         DPF( 1, "MODM_OPEN" ) ;

         if (uDevId > 1)
            return MMSYSERR_BADDEVICEID ;

         dn = ((LPMIDIOPENDESC) dwParam1) -> dnDevNode ;

         if (NULL == 
               (phwi = DevNodeToHardwareInstance( dn )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         if (!phwi -> fEnabled)
            return MMSYSERR_NOTENABLED ;

         return (modOpen( phwi, 
                          (LPDWORD) dwUser, 
                          (LPMIDIOPENDESC) dwParam1, 
                          dwParam2 )) ;
      }
      break ;

      case MODM_GETDEVCAPS:

      {
         DPF( 1, "MODM_GETDEVCAPS" ) ;

         if (uDevId > 1)
            return MMSYSERR_BADDEVICEID ;

         if (NULL == 
               (phwi = DevNodeToHardwareInstance( dwParam2 )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         if (!phwi -> fEnabled)
            return MMSYSERR_NOTENABLED ;

         modGetDevCaps( phwi, (MDEVICECAPSEX FAR *) dwParam1 ) ;

         return MMSYSERR_NOERROR ;
      }
      break ;

   }

   // Bad device ID???

   if (uDevId > 1)
      return MMSYSERR_BADDEVICEID ;

   pmoc = (PMIDIOUTCLIENT) LOWORD( dwUser ) ;
   phwi = pmoc -> pa.phwi ;

   switch ( uMsg )
   {
      case MODM_CLOSE:
         modClose( pmoc ) ;
         return MMSYSERR_NOERROR ;

      case MODM_RESET:
      {
         DPF( 1, "MODM_RESET" ) ;

         pmoc -> wMidiOutEntered++ ;

         // make sure we're not being reentered...

         if (pmoc -> wMidiOutEntered != 1)
         {
            DPF( 1, "MODM_RESET reentered!" ) ;
            dwReturn = MIDIERR_NOTREADY;
         }
         else
         {
            // turn all notes off...
           #ifdef MIDI_STREAMS
            modStreamReset (pmoc);
           #else
            modReset( pmoc ) ;
           #endif
            dwReturn = MMSYSERR_NOERROR ;
         }
         pmoc -> wMidiOutEntered--;
         return dwReturn ;
      }

      case MODM_DATA:
      {
         DPF( 4, "MODM_DATA" ) ;

         // message is in dwParam1

         pmoc -> wMidiOutEntered++ ;

         // make sure we're not being reentered...

         if (pmoc -> wMidiOutEntered != 1)
         {
            DPF( 1, "MODM_DATA reentered!" ) ;
            dwReturn = MIDIERR_NOTREADY;
         }
         else
         {
            modSendShortMsg( pmoc, dwParam1 ) ;
            dwReturn = MMSYSERR_NOERROR ;
         }
         pmoc -> wMidiOutEntered--;
         return dwReturn ;
      }

      case MODM_LONGDATA:
      {  
         DPF( 4, "MODM_LONGDATA" ) ;

         // far pointer to header in dwParam1

         pmoc -> wMidiOutEntered++ ;

         if (pmoc -> wMidiOutEntered != 1)
         {
            DPF( 1, "MODM_LONGDATA reentered!" ) ;
            dwReturn = MIDIERR_NOTREADY;
         }
         else
           dwReturn = modSendLongData( pmoc, (LPMIDIHDR)dwParam1 ) ;
         pmoc -> wMidiOutEntered-- ;
         return dwReturn ;
      }

      // -------------- begin MIDI Stream messages -------------------
      //
     #ifdef MIDI_STREAMS

      case MODM_PROPERTIES:
         return modProperty (pmoc, (LPBYTE)dwParam1, dwParam2);

      case MODM_STRMDATA:
         return modStreamData (pmoc, (LPMIDIHDR)dwParam1, (UINT)dwParam2);

      case MODM_GETPOS:
         return modGetStreamPosition (pmoc, (LPMMTIME)dwParam1);

      case MODM_STOP:
         return modStreamReset (pmoc);

      case MODM_RESTART:
         return modStreamRestart (pmoc, dwParam1, dwParam2);

      case MODM_PAUSE:
         return modStreamPause (pmoc);

      default:
         return modMisc (pmoc, uMsg, dwParam1, dwParam2);

     #endif // MIDI_STREAMS
      //
      // -------------- end MIDI Stream messages -------------------

   }

   return MMSYSERR_NOTSUPPORTED ;

} // modMessage()

//---------------------------------------------------------------------------
//  End of File: midifix.c
//---------------------------------------------------------------------------
