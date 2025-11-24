//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------
//
//  Module: fmsynth.c
//
//  Description:
//     MIDI routines for FM synthesis.
//
//---------------------------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include "msopl.h"

// patch library

LPPATCHES glpPatch = NULL ;          // points to the patches

// voices being played

static DWORD  gdwCurTime = 1 ;               // for note on/off
static WORD   gwIODelay = 25 ;

// operator offset location

static WORD BCODE gw2OpOffset[ NUM2VOICES ][ 2 ] =
{
   { 0x000,0x003 },
   { 0x001,0x004 },
   { 0x002,0x005 },
   { 0x008,0x00b },
   { 0x009,0x00c },
   { 0x00a,0x00d },
   { 0x010,0x013 },
   { 0x011,0x014 },
   { 0x012,0x015 },

   { 0x100,0x103 },
   { 0x101,0x104 },
   { 0x102,0x105 },
   { 0x108,0x10b },
   { 0x109,0x10c },
   { 0x10a,0x10d },
   { 0x110,0x113 },
   { 0x111,0x114 },
   { 0x112,0x115 },
} ;

static BYTE BCODE gbPercMap[53][2] =
{
   {  0, 35 },
   {  0, 35 },
   {  2, 52 },
   {  3, 48 },
   {  4, 58 },
   {  5, 60 },
   {  6, 47 },
   {  7, 43 },
   {  6, 49 },
   {  9, 43 },
   {  6, 51 },
   { 11, 43 },
   {  6, 54 },
   {  6, 57 },
   { 14, 72 },
   {  6, 60 },
   { 16, 76 },
   { 17, 84 },
   { 18, 36 },
   { 19, 76 },
   { 20, 84 },
   { 21, 83 },
   { 22, 84 },
   { 23, 24 },
   { 16, 77 },
   { 25, 60 },
   { 26, 65 },
   { 27, 59 },
   { 28, 51 },
   { 29, 45 },
   { 30, 71 },
   { 31, 60 },
   { 32, 58 },
   { 33, 53 },
   { 34, 64 },
   { 35, 71 },
   { 36, 61 },
   { 37, 61 },
   { 38, 48 },
   { 39, 48 },
   { 40, 69 },
   { 41, 68 },
   { 42, 63 },
   { 43, 74 },
   { 44, 60 },
   { 45, 80 },
   { 46, 64 },
   { 47, 69 },
   { 48, 73 },
   { 49, 75 },
   { 50, 68 },
   { 51, 48 },
   { 52, 53 }
} ;

// pitch values, from middle c to octave above

static DWORD BCODE gdwPitch[ 12 ] =
   { PITCH(C), PITCH(CSHARP), PITCH(DNOTE), PITCH(DSHARP),
     PITCH(E), PITCH(F), PITCH(FSHARP), PITCH(G),
     PITCH(GSHARP), PITCH(A), PITCH(ASHARP), PITCH(B) } ;

// transformation of linear velocity value to
// logarithmic attenuation

static BYTE BCODE gbVelocityAtten[ 32 ] =
   { 40, 36, 32, 28, 23, 21, 19, 17,
     15, 14, 13, 12, 11, 10, 9, 8,
     7, 6, 5, 5, 4, 4, 3, 3,
     2, 2, 1, 1, 1, 0, 0, 0 } ;

DWORD      dwCurData = 0L ;     // position in long message buffer
BYTE       status = 0 ;

static WORD      wFMEntered = 0 ;    // reentrancy check

//--------------------------------------------------------------------------
//  
//  BYTE VolLinearToLog
//  
//  Description:
//      Converts a linear scale to logarithm (0xFFFF -> 0, 0x0001 -> 191)
//  
//  Parameters:
//      WORD wVolume
//         MMSystem.DLL's volume (0x0000 to 0xFFFF)
//  
//  Return (BYTE):
//      Value in decibels attenuation, each unit is 1.5 dB
//  
//  
//--------------------------------------------------------------------------

BYTE FAR PASCAL VolLinearToLog
(
    WORD            wVolume
)
{
   WORD    gain, shift ;
   WORD    temp ;
   WORD    lut[16] = {0,0,0,1,1,1,2,2,2,2,3,3,3,3,3,3} ;
   BYTE    out ;

   //
   // Catch boundary conditions...
   //

   if (wVolume == 0xFFFF)
      return ( 0 ) ;

   if (wVolume == 0x0000)
      return ( SILENCE ) ;

   // Get an estimate to within 6 dB of gain

   for (temp = wVolume, gain = 0, shift = 0;
         temp != 0;
         gain += 4, temp >>= 1, shift++);

   // Look at highest 3 bits in number into
   // look-up-table to find how many more dB

   if (shift > 5)
      temp = wVolume >> (shift - 5) ;
   else if (shift < 5)
      temp = wVolume << (5 - shift) ; 
   else
      temp = wVolume ;
   temp &= 0x000f ;

   gain += lut[ temp ] ;

   out = (BYTE) ((16 * 4) + 3 - gain) ;
   return (out < 128) ? out : (BYTE) 127 ;

} // VolLinearToLog()

//------------------------------------------------------------------------
//  VOID fmSend
//
//  Description:
//     Sends a byte to the FM chip.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     WORD wAddress
//        0x00 to 0x1ff (bank and register)
//
//     BYTE bValue
//        value written
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL fmSend
(
    PHARDWAREINSTANCE  phwi,
    WORD               wAddress,
    BYTE               bValue
)
{
   UINT  i ;

   // port is the address #1, port + 1 is the data #1,
   // port + 2 is the address #2, and port + 3 is the data #2


   WRITE_PORT_UCHAR( (wAddress < 0x100) ? 
                        phwi -> wIOAddressSynth : phwi -> wIOAddressSynth + 2,
                     (BYTE) wAddress ) ;

   // wait for OPL3

   for (i = 0; i < gwIODelay; i++)
      READ_PORT_UCHAR( phwi -> wIOAddressSynth ) ;

   WRITE_PORT_UCHAR( phwi -> wIOAddressSynth + 1, bValue ) ;

   // wait for OPL3

   for (i = 0; i < gwIODelay; i++)
      READ_PORT_UCHAR( phwi -> wIOAddressSynth ) ;

} // end of fmSend()

//------------------------------------------------------------------------
//  VOID fmNote
//
//  Description:
//     Turns on an FM-synthesizer note.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     WORD wNote
//        the note number from 0 to NUMVOICES
//
//     LPNOTE lpN
//        structure containing information about what
//        is to be played.
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID NEAR PASCAL fmNote
(
    PHARDWAREINSTANCE   phwi, 
    WORD                wNote,
    LPNOTE              lpN
)
{
   WORD            i ;
   WORD            wOffset ;
   LPOPER          lpOP ;

   // write out a note off, just to make sure...

   wOffset =
      (wNote < (NUM2VOICES / 2)) ? wNote : (wNote + 0x100 - 9) ;
   fmSend( phwi, AD_BLOCK + wOffset, 0 ) ;

   // writing the operator information

   for (i = 0; i < 2; i++)
   {
      lpOP = &lpN -> op[ i ] ;
      wOffset = gw2OpOffset[ wNote ][ i ] ;
      fmSend( phwi, 0x20 + wOffset, lpOP -> bAt20) ;
      fmSend( phwi, 0x40 + wOffset, lpOP -> bAt40) ;
      fmSend( phwi, 0x60 + wOffset, lpOP -> bAt60) ;
      fmSend( phwi, 0x80 + wOffset, lpOP -> bAt80) ;
      fmSend( phwi, 0xE0 + wOffset, lpOP -> bAtE0) ;
   }

   // write out the voice information

   wOffset = (wNote < 9) ? wNote : (wNote + 0x100 - 9) ;
   fmSend( phwi, 0xa0 + wOffset, lpN -> bAtA0[ 0 ] ) ;
   fmSend( phwi, 0xc0 + wOffset, lpN -> bAtC0[ 0 ] ) ;

   // Note on...

   fmSend( phwi, 0xb0 + wOffset,
               (BYTE)(lpN -> bAtB0[ 0 ] | 0x20) ) ;

} // end of fmNote()

//------------------------------------------------------------------------
//  WORD fmFindEmptySlot
//
//  Description:
//     This finds an empty note-slot for a MIDI voice.
//     If there are no empty slots then this looks for the oldest
//     off note.  If this doesn't work then it looks for the oldest
//     on-note of the same patch.  If all notes are still on then
//     this finds the oldests turned-on-note.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bPatch
//        MIDI patch that will replace it.
//
//  Return Value:
//     WORD
//        note slot #
//
//
//------------------------------------------------------------------------

WORD NEAR PASCAL fmFindEmptySlot
(
    PHARDWAREINSTANCE  phwi,
    BYTE               bPatch
)
{
   WORD   i, found, wNumVoices ;
   DWORD  dwOldest ;

   // First, look for a slot with a time == 0

   wNumVoices = 
      (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED) ? 
         NUM2VOICES : NUM2VOICES / 2 ;

   for (i = 0;  i < wNumVoices; i++)
      if (!phwi -> aVoiceSlots[ i ].dwTime)
         return ( i ) ;

   // Now, look for a slot of the oldest off-note

   dwOldest = 0xffffffff ;
   found = 0xffff ;

   for (i = 0; i < wNumVoices; i++)
      if (!phwi -> aVoiceSlots[ i ].bOn && (phwi -> aVoiceSlots[ i ].dwTime < dwOldest))
      {
         dwOldest = phwi -> aVoiceSlots[ i ].dwTime ;
         found = i ;
      }
   if (found != 0xffff)
      return ( found ) ;

   // Now, look for a slot of the oldest note with
   // the same patch

   dwOldest = 0xffffffff ;
   found = 0xffff ;
   for (i = 0; i < wNumVoices; i++)
      if ((phwi -> aVoiceSlots[ i ].bPatch == bPatch) && (phwi -> aVoiceSlots[ i ].dwTime < dwOldest))
      {
         dwOldest = phwi -> aVoiceSlots[ i ].dwTime ;
         found = i ;
      }
   if (found != 0xffff)
      return ( found ) ;

   // Now, just look for the oldest voice

   found = 0 ;
   dwOldest = phwi -> aVoiceSlots[ found ].dwTime ;
   for (i = (found + 1); i < wNumVoices; i++)
      if (phwi -> aVoiceSlots[ i ].dwTime < dwOldest)
      {
         dwOldest = phwi -> aVoiceSlots[ i ].dwTime ;
         found = i ;
      }

   return ( found ) ;

} // end of fmFindEmptySlot()

//------------------------------------------------------------------------
//  WORD fmFindFullSlot
//
//  Description:
//     This finds a slot with a specific note, and channel.
//     If it is not found then 0xFFFF is returned.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bNote
//        MIDI note number
//
//     BYTE bChannel
//        MIDI channel #
//
//  Return Value:
//     WORD
//        note slot #, or 0xFFFF if can't find it
//
//
//------------------------------------------------------------------------

WORD NEAR PASCAL fmFindFullSlot
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bNote,
    BYTE                bChannel
)
{
   WORD  i, wNumVoices ;

   wNumVoices = 
      (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED) ? 
         NUM2VOICES : NUM2VOICES / 2 ;

   for (i = 0; i < wNumVoices; i++)
      if ((bChannel == phwi -> aVoiceSlots[ i ].bChannel) &&
          (bNote == phwi -> aVoiceSlots[ i ].bNote) && (phwi -> aVoiceSlots[ i ].bOn))
   return ( i ) ;

   // couldn't find it

   return ( 0xFFFF ) ;

} // end of fmFindFullSlot()

//------------------------------------------------------------------------
//  DWORD fmCalcBend
//
//  Description:
//     This calculates the effects of pitch bend
//     on an original value.
//
//  Parameters:
//     DWORD dwOrig
//        original frequency
//
//     short iBend
//        from -32768 to 32768, -2 half steps to +2
//
//  Return Value:
//     DWORD
//        new frequency
//
//
//------------------------------------------------------------------------

DWORD NEAR PASCAL fmCalcBend
(
    DWORD           dwOrig,
    short           iBend
)
{
   DWORD  dw ;

   // Do different things depending on positive
   // or negative bend

   if (iBend > 0)
   {
      dw = (DWORD)((iBend * (LONG)(256.0 * (EQUAL * EQUAL - 1.0))) >> 8) ;
      dwOrig += (DWORD)((dw * dwOrig) >> 15) ;
   }
   else if (iBend < 0)
   {
      dw = (DWORD)(((-(LONG)iBend) * (LONG)(256.0 * (1.0 - 1.0 / EQUAL / EQUAL))) >> 8) ;
      dwOrig -= (DWORD)((dw * dwOrig) >> 15) ;
   }

   return ( dwOrig ) ;

} // end of fmCalcBend()

//------------------------------------------------------------------------
//  WORD fmCalcFAndB
//
//  Description:
//     Calculates the FNumber and Block given a frequency.
//
//  Parameters:
//     DWORD dwPitch
//        pitch
//
//  Return Value:
//     WORD
//        High byte contains the 0xB0 section of the
//        block and fNum, and the low byte contains
//        the 0xA0 section of the fNumber.
//
//
//------------------------------------------------------------------------

WORD NEAR PASCAL fmCalcFAndB
(
    DWORD           dwPitch
)
{
   BYTE  bBlock ;

   // bBlock is like an exponential to dwPitch (or FNumber)

   for (bBlock = 1; dwPitch >= 0x400; dwPitch >>= 1, bBlock++) ;

   if (bBlock > 0x07)
      bBlock = 0x07 ;  // we can't do anything about this

   // put in high two bits of F-num into bBlock

   return ( ((WORD) bBlock << 10) | (WORD) dwPitch ) ;

} // end of fmCalcFAndB()

//------------------------------------------------------------------------
//  BYTE fmCalcVolume
//
//  Description:
//     This calculates the attenuation for an operator.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bOrigAtten
//        original attenuation in .75 dB units
//
//     BYTE bChannel
//        MIDI channel
//
//     BYTE bVelocity
//        velocity of the note
//
//     BYTE bOper
//        operator number (from 0 to 3)
//
//     BYTE bMode
//        voice mode (from 0 through 7 for modulator/carrier selection)
//
//  Return Value:
//     BYTE
//        new attenuation in 0.75 dB units, maxing out at 0x3f
//
//
//------------------------------------------------------------------------

BYTE NEAR PASCAL fmCalcVolume
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bOrigAtten,
    BYTE                bChannel,
    BYTE                bVelocity,
    BYTE                bOper,
    BYTE                bMode
)
{
   BYTE  bVolume ;
   WORD  wTemp ;
   WORD  wMin ;

   switch (bMode)
   {
      case 0:
         bVolume = (BYTE)(bOper == 3) ;
         break ;

      case 1:
         bVolume = (BYTE)((bOper == 1) || (bOper == 3)) ;
         break ;

      case 2:
         bVolume = (BYTE)((bOper == 0) || (bOper == 3)) ;
         break ;

      case 3:
         bVolume = (BYTE)(bOper != 1) ;
         break ;

      case 4:
         bVolume = (BYTE)((bOper == 1) || (bOper == 3)) ;
         break ;

      case 5:
         bVolume = (BYTE)(bOper >= 1) ;
         break ;

      case 6:
         bVolume = (BYTE)(bOper <= 2) ;
         break ;

      case 7:
         bVolume = TRUE ;
         break ;
   }

   if (!bVolume)
      return bOrigAtten ;   // this is a modulator wave

   wMin = min( phwi -> wSynthAttenL, phwi -> wSynthAttenR ) ;
   wTemp =
      bOrigAtten + ((wMin << 1) + phwi -> abChanAttens[ bChannel ] +
                     gbVelocityAtten[ bVelocity >> 2 ] ) ;
   return ( (wTemp > 0x3f) ? (BYTE) 0x3f : (BYTE) wTemp ) ;

} // end of fmCalcVolume()

//------------------------------------------------------------------------
//  BYTE fmCalcStereoMask
//
//  Description:
//     This calculates the stereo mask.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bChannel
//        MIDI channel
//
//  Return Value:
//     BYTE
//        mask (for register 0xC0-C8) for eliminating the
//        left/right/both channels
//
//
//------------------------------------------------------------------------

BYTE NEAR PASCAL fmCalcStereoMask
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bChannel
)
{
   WORD   wLeft, wRight ;

   // Figure out the basic levels of the 2 channels

   wLeft = (phwi -> wSynthAttenL << 1) + phwi -> abChanAttens[ bChannel ] ;
   wRight = (phwi -> wSynthAttenR << 1) + phwi -> abChanAttens[ bChannel ] ;

   // If both are too quiet then mask to nothing...

   if ((wLeft > 0x3f) && (wRight > 0x3f))
      return 0xcf ;

   // If one channel is significantly quieter than the other
   // then eliminate it...

   if ((wLeft + 8) < wRight)
      // right is too quiet so eliminate
      return (BYTE)(0xef & phwi -> abStereoMasks[ bChannel ]) ;
   else if ((wRight + 8) < wLeft)
      // left if too quiet so eliminate
      return (BYTE)(0xdf & phwi -> abStereoMasks[ bChannel ]) ;
   else
      // use both channels
      return (BYTE)(phwi -> abStereoMasks[ bChannel ]) ;

} // end of fmCalcStereoMask()

//------------------------------------------------------------------------
//  WORD fmNoteOn
//
//  Description:
//     This turns on a note, including drums with a patch # of the
//     drum note + 0x80.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bPatch
//        MIDI patch
//
//     BYTE bNote
//        MIDI note
//
//     BYTE bChannel
//        MIDI channel
//
//     BYTE bVelocity
//        velocity value
//
//     short iBend
//        current pitch bend from -32768 to 32767
//
//  Return Value:
//     WORD
//        note slot #, or 0xFFFF if it is inaudible
//
//
//------------------------------------------------------------------------

#pragma optimize( "leg", off )

WORD NEAR PASCAL fmNoteOn
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bPatch,
    BYTE                bNote,
    BYTE                bChannel,
    BYTE                bVelocity,
    short               iBend
)
{
   WORD             wTemp, i, j ;
   BYTE             bTemp, bMode, bStereo ;
   LPPATCHES        lpPS ;
   DWORD            dwBasicPitch, dwPitch[ 2 ] ;
   NOTE             NS ;

   // Get a pointer to the patch

   lpPS = glpPatch + bPatch ;

#if 0
{
   char szDebug[ 80 ] ;

   wsprintf( szDebug, "bPatch = %d, bNote = %d", bPatch, bNote ) ;
   DPF( 3, szDebug ) ;
}
#endif

   // Find out the basic pitch according to our
   // note value.  This may be adjusted because of
   // pitch bends or special qualities for the note.

   dwBasicPitch = gdwPitch[ bNote % 12 ] ;
   bTemp = bNote / (BYTE) 12 ;
   if (bTemp > (BYTE) (60 / 12))
      dwBasicPitch <<= (BYTE)(bTemp - (BYTE)(60/12)) ;
   else if (bTemp < (BYTE) (60/12))
      dwBasicPitch >>= (BYTE)((BYTE) (60/12) - bTemp) ;

   // Copy the note information over and modify
   // the total level and pitch according to
   // the velocity, midi volume, and tuning.

   _asm cld ;
   _fmemcpy( (LPSTR) &NS, (LPSTR) &lpPS -> note, sizeof( NOTE ) ) ;

   for (j = 0; j < 2; j++)
   {
      // modify pitch

      dwPitch[ j ] = dwBasicPitch ;
      bTemp = (BYTE)((NS.bAtB0[ j ] >> 2) & 0x07) ;
      if (bTemp > 4)
         dwPitch[ j ] <<= (BYTE)(bTemp - (BYTE)4) ;
      else if (bTemp < 4)
         dwPitch[ j ] >>= (BYTE)((BYTE)4 - bTemp) ;

      wTemp = fmCalcFAndB( fmCalcBend( dwPitch[ j ], iBend ) ) ;
      NS.bAtA0[ j ] = (BYTE) wTemp ;
      NS.bAtB0[ j ] = (BYTE) 0x20 | (BYTE) (wTemp >> 8) ;
   }

   // Modify level for each operator, but only
   // if they are carrier waves

   bMode = (BYTE) ((NS.bAtC0[ 0 ] & 0x01) * 2 + 4) ;

   for (i = 0; i < 2; i++)
   {
      wTemp =
         (BYTE) fmCalcVolume ( phwi, (BYTE)(NS.op[ i ].bAt40 & (BYTE) 0x3f),
                               bChannel, bVelocity, (BYTE) i, bMode ) ;
      NS.op[ i ].bAt40 = (NS.op[ i ].bAt40 & (BYTE)0xc0) | (BYTE) wTemp ;
   }

   // Do stereo panning, but cutting off a left or
   // right channel if necessary...

   bStereo = fmCalcStereoMask( phwi, bChannel ) ;
   NS.bAtC0[ 0 ] &= bStereo ;

   // Find an empty slot, and use it...

   wTemp = fmFindEmptySlot( phwi, bPatch ) ;

#if 0
{
   char  szDebug[ 80 ] ;

   wsprintf( szDebug, "Found empty slot: %d", wTemp ) ;
   DPF( 3, szDebug ) ;

}
#endif

   fmNote( phwi, wTemp, &NS ) ;
   phwi -> aVoiceSlots[ wTemp ].bNote = bNote ;
   phwi -> aVoiceSlots[ wTemp ].bChannel = bChannel ;
   phwi -> aVoiceSlots[ wTemp ].bPatch = bPatch ;
   phwi -> aVoiceSlots[ wTemp ].bVelocity = bVelocity ;
   phwi -> aVoiceSlots[ wTemp ].bOn = TRUE ;
   phwi -> aVoiceSlots[ wTemp ].dwTime = gdwCurTime++ ;
   phwi -> aVoiceSlots[ wTemp ].dwOrigPitch[0] = dwPitch[ 0 ] ;  // not including bend
   phwi -> aVoiceSlots[ wTemp ].dwOrigPitch[1] = dwPitch[ 1 ] ;  // not including bend
   phwi -> aVoiceSlots[ wTemp ].bBlock[0] = NS.bAtB0[ 0 ] ;
   phwi -> aVoiceSlots[ wTemp ].bBlock[1] = NS.bAtB0[ 1 ] ;

   return ( wTemp ) ;

} // end of fmNoteOn()

#pragma optimize( "", on )

//------------------------------------------------------------------------
//  VOID fmNoteOff
//
//  Description:
//     This turns off a note, including drums with a patch
//     # of the drum note + 128.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bPatch
//        MIDI patch
//
//     BYTE bNote
//        MIDI note
//
//     BYTE bChannel
//        MIDI channel
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL fmNoteOff
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bPatch,
    BYTE                bNote,
    BYTE                bChannel
)
{
   LPPATCHES        lpPS ;
   WORD             wOffset, wTemp ;

   // Find the note slot

   wTemp = fmFindFullSlot( phwi, bNote, bChannel ) ;

   if (wTemp != 0xffff)
   {
      // get a pointer to the patch

      lpPS = glpPatch + (BYTE) phwi -> aVoiceSlots[ wTemp ].bPatch ;

      // shut off the note portion

      // we have the note slot, turn it off.

      wOffset = (wTemp < (NUM2VOICES / 2)) ? wTemp : (wTemp + 0x100 - 9) ;
      fmSend( phwi, AD_BLOCK + wOffset,
                  (BYTE)(phwi -> aVoiceSlots[ wTemp ].bBlock[ 0 ] & 0x1f) ) ;

      // Note this...

      phwi -> aVoiceSlots[ wTemp ].bOn = FALSE ;
      phwi -> aVoiceSlots[ wTemp ].bBlock[ 0 ] &= 0x1f ;
      phwi -> aVoiceSlots[ wTemp ].bBlock[ 1 ] &= 0x1f ;
      phwi -> aVoiceSlots[ wTemp ].dwTime = gdwCurTime ;
   }

} // end of fmNoteOff()

//------------------------------------------------------------------------
//  VOID fmNewVolume
//
//  Description:
//     This should be called if a volume level has changed.
//     This will adjust the levels of all the playing voices.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bChannel
//        channel # of 0xFF for all channels
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL fmNewVolume
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bChannel
)
{
   WORD            i, j, wTemp, wNumVoices, wOffset ;
   LPNOTE          lpPS ;
   BYTE            bMode, bStereo ;

   // Make sure that we are actually open...

   if (!glpPatch)
      return ;

   if (phwi -> fSoftwareVolumeEnabled)
   {
      phwi -> wSynthAttenL =
         VolLinearToLog( LOWORD( phwi -> dwSynthVolume ) ) +
            VolLinearToLog( LOWORD( phwi -> dwMasterVolume ) ) ;

      phwi -> wSynthAttenR =
         VolLinearToLog( HIWORD( phwi -> dwSynthVolume ) ) +
            VolLinearToLog( HIWORD( phwi -> dwMasterVolume ) ) ;
   }
   else
   {
      phwi -> wSynthAttenL = 0 ;
      phwi -> wSynthAttenR = 0 ;
   }

   //
   // Watch the range...
   //

   phwi -> wSynthAttenL *= 2 ; // FM synth is .75 dB steps

   if (phwi -> wSynthAttenL > SILENCE)
      phwi -> wSynthAttenL = SILENCE ;

   phwi -> wSynthAttenR *= 2 ; // FM synth is .75 dB steps

   if (phwi -> wSynthAttenR > SILENCE)
      phwi -> wSynthAttenR = SILENCE ;

   //
   // If it's not in use, then we're done.
   // 

   if (!phwi -> fInUse)
      return ;

   // Loop through all the notes looking for the right
   // channel.  Anything with the right channel gets
   // its pitch bent.

   wNumVoices = 
      (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED) ? 
         NUM2VOICES : NUM2VOICES / 2 ;
   
   for (i = 0; i < wNumVoices; i++)
      if ((phwi -> aVoiceSlots[ i ].bChannel == bChannel) || (bChannel == 0xff))
      {
         // Get a pointer to the patch

         lpPS = &(glpPatch + phwi -> aVoiceSlots[ i ].bPatch) -> note ;

         // Modify level for each operator, but only if they
         // are carrier waves...

         bMode = (BYTE) ( (lpPS->bAtC0[0] & 0x01) * 2 + 4);

         for (j = 0; j < 2; j++)
          {
            wTemp =
               (BYTE) fmCalcVolume( phwi, 
                                    (BYTE)(lpPS -> op[ j ].bAt40 & (BYTE) 0x3f),
                                    phwi -> aVoiceSlots[ i ].bChannel,
                                    phwi -> aVoiceSlots[i].bVelocity, (BYTE) j, bMode ) ;

            // Write the new value out.

            wOffset = gw2OpOffset[ i ][ j ] ;
            fmSend( phwi, 0x40 + wOffset,
                        (BYTE) ((lpPS -> op[ j ].bAt40 & (BYTE)0xc0) |
                                (BYTE) wTemp) ) ;
         }

         // Do stereo panning, but cutting off a left or right
         // channel if necessary.

         bStereo = fmCalcStereoMask( phwi, phwi -> aVoiceSlots[ i ].bChannel ) ;
         wOffset = (i < (NUM2VOICES / 2)) ? i : (i + 0x100 - 9) ;
         fmSend( phwi, 0xc0 + wOffset, (BYTE)(lpPS -> bAtC0[ 0 ] & bStereo) ) ;
      }

} // end of fmNewVolume()

//------------------------------------------------------------------------
//  VOID fmPitchBend
//
//  Description:
//     This pitch bends a channel.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     BYTE bChannel
//        channel
//
//     short iBend
//        values from -32768 to 32767, being -2 to +2 half steps
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID NEAR PASCAL fmPitchBend
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bChannel,
    UINT                uBend
)
{
   WORD   i, wTemp[ 2 ], wOffset, wNumVoices, j ;
   DWORD  dwNew ;

   // Remember the current bend..

   phwi -> auBend[ bChannel ] = uBend ;

   // Loop through all the notes looking for the right
   // channel.  Anything with the right channel gets its
   // pitch bent...

   wNumVoices = 
      (phwi -> wHardwareOptions & MSOPL_HWOPTIONSF_OPL3DETECTED) ? 
         NUM2VOICES : NUM2VOICES / 2 ;

   for (i = 0; i < wNumVoices; i++)
      if (phwi -> aVoiceSlots[ i ].bChannel == bChannel)
      {
         j = 0 ;
         dwNew = fmCalcBend( phwi -> aVoiceSlots[ i ].dwOrigPitch[ j ],
                             uBend ) ;
         wTemp[ j ] = fmCalcFAndB( dwNew ) ;
         phwi -> aVoiceSlots[ i ].bBlock[ j ] =
            (phwi -> aVoiceSlots[ i ].bBlock[ j ] & (BYTE) 0xe0) |
               (BYTE) (wTemp[ j ] >> 8) ;

         wOffset = (i < (NUM2VOICES / 2)) ? i : (i + 0x100 - 9) ;
         fmSend( phwi, AD_BLOCK + wOffset, phwi -> aVoiceSlots[ i ].bBlock[ 0 ] ) ;
         fmSend( phwi, AD_FNUMBER + wOffset, (BYTE) wTemp[ 0 ] ) ;
      }

} // end of fmPitchBend()

//------------------------------------------------------------------------
//  VOID fmMidiMessage
//
//  Description:
//     This handles a MIDI message.  This does not do running status.
//
//  Parameters:
//     PHARDWAREINSTANCE phwi
//        pointer to hardware instance structure
//  
//     DWORD dwData
//        up to 4 bytes of MIDI data depending on the message
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID NEAR PASCAL fmMidiMessage
(
    PHARDWAREINSTANCE   phwi,
    DWORD               dwData
)
{
   BYTE  bChannel, bVelocity, bNote ;
   WORD  wTemp ;
   
   bChannel = (BYTE) dwData & (BYTE)0x0f ;
   bVelocity = (BYTE) (dwData >> 16) & (BYTE)0x7f ;
   bNote = (BYTE) ((WORD) dwData >> 8) & (BYTE)0x7f ;

   switch ((BYTE)dwData & 0xf0)
   {
      case 0x90:
      {
         // turn key on, or key off if volume == 0

         if (bVelocity)
         {
            if (bChannel == DRUMCHANNEL)
            {
               if (bNote >= 35 && bNote <= 87)
               {
                  fmNoteOn( phwi, 
                            (BYTE) (gbPercMap[ bNote - 35 ][ 0 ] + 0x80),
                            gbPercMap[ bNote - 35 ][ 1 ],
                            bChannel, bVelocity,
                            (short) phwi -> auBend[ bChannel ] ) ;
               }
            }
            else
               fmNoteOn( phwi,
                         (BYTE) phwi -> abPatch[ bChannel ], bNote, bChannel,
                         bVelocity, (short) phwi -> auBend[ bChannel ] ) ;
            break ;

         }
      }

      // else, continue through and turn key off

      case 0x80:
      {
         // Turn key off...

         if (bChannel == DRUMCHANNEL)
         {
            if (bNote >= 35 && bNote <= 87)
               fmNoteOff( phwi, (BYTE) (gbPercMap[ bNote - 35 ][ 0 ] + 0x80),
                            gbPercMap[ bNote - 35 ][ 1 ], bChannel ) ;
         }
         else
            fmNoteOff( phwi, (BYTE) phwi -> abPatch[bChannel], 
                       bNote, bChannel ) ;
         break ;
      }

      case 0xb0:
      {
         // Change control

         switch (bNote)
         {
            case 7:
            {
               // Set the main volume for each channel,
               // converting the linear value into logarithmic
               // attenuation.

               phwi -> abChanAttens[ bChannel ] =
                  gbVelocityAtten[ (bVelocity & 0x7f) >> 2 ] ;
               break ;
            }

            case 8:
            case 10:
            {
               // Change the pan level

               if (bVelocity > (64 + 16))
                  // right channel only...
                  phwi -> abStereoMasks[ bChannel ] = 0xdf ;
               else if (bVelocity < (64 - 16))
                  // left channel only...
                  phwi -> abStereoMasks[ bChannel ] = 0xef ;
               else
                  // both channels
                  phwi -> abStereoMasks[ bChannel ] = 0xff ;

               // change any currently playing patches

               fmNewVolume( phwi, bChannel ) ;

               break ;
            }

	    case 123:	// All Notes Off
	    case 124:	// Omni Mode Off (All Notes Off)
	    case 125:	// Omni Mode On (All Notes Off)
	    case 126:	// Mono Mode On (Poly Mode Off) (All Notes Off)
	    case 127:	// Poly Mode On (Mono Mode Off) (All Notes Off)
	    {
		// For these, we just do All Notes Off for the channel
		UINT i ;
		
		for (i = 0; i < NUM2VOICES; i++) {
		    if (bChannel == phwi -> aVoiceSlots[ i ].bChannel) {
			fmNoteOff( phwi,
				   phwi -> aVoiceSlots[ i ].bPatch,
				   phwi -> aVoiceSlots[ i ].bNote,
				   phwi -> aVoiceSlots[ i ].bChannel ) ;
		    }
		}

		break ;
	    }
	 }

         break ;
	 
      }

      case 0xc0:
      {
         // change patch

         phwi -> abPatch[ bChannel ] = bNote ;
         break ;
      }

      case 0xe0:
      {
         DPF( 3, "Bend" ) ;

         // Pitch bend

         wTemp = ((WORD) bVelocity << 9) | ((WORD) bNote << 2) ;

         // Watch this... 0x8000 wraps around, cause it's off
         // by one, and you'll get a pitch bend in the completely
         // opposite direction!

         phwi -> auBend[ bChannel ] = (short) (WORD) (wTemp + 0x7fff) ;
         fmPitchBend( phwi, bChannel, phwi -> auBend[ bChannel ] ) ;

         break ;
      }
   }

   return ;

} // end of fmMidiMessage()

//------------------------------------------------------------------------
//  VOID fmCallback
//
//  Description:
//     This calls DriverCallback for a MIDI device.
//
//  Parameters:
//     PFMALLOC pClient
//        pointer to client information
//
//     WORD msg
//        the message to send
//
//     DWORD dw1
//        message dependant parameter
//
//     DWORD dw2
//        message dependant parameter
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL fmCallback
(
    PFMALLOC        pClient,
    WORD            msg,
    DWORD           dw1,
    DWORD           dw2
)
{

   // Invoke the callback function, if it exists.  dwFlags contains
   // driver-specific flags in the LOWORD and generic driver flags
   // in the HIWORD.

   if (HIWORD(pClient -> dwFlags) )
      DriverCallback( pClient -> dwCallback,
                      HIWORD( pClient -> dwFlags ),
                      pClient -> hMidi, msg, pClient -> dwInstance, dw1, dw2 ) ;

} // end of fmCallback()

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
   LPMIDIHDR  lpHdr ;
   LPSTR      lpBuf ;          // current spot in the long msg buf
   DWORD      dwBytesRead ;    // how far are we in the buffer
   DWORD      dwMsg = 0 ;      // short midi message sent to synth
   BYTE       bBytePos = 0 ;   // shift current byte by dwBytePos
   BYTE       bBytesLeft = 0 ; // how many dat bytes needed
   BYTE       curByte ;        // current byte in long buffer

   PFMALLOC            pClient ;
   PHARDWAREINSTANCE   phwi ;

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

         DPF( 1, "MODM_ENABLE" ) ;

         // dwParam2 == PnP DevNode

         // first, query the supporting VxD ID

         cIds = 1 ;

         if (midiOutMessage( (HMIDIOUT) dwParam1, DRV_QUERYDRIVERIDS,
                              (DWORD) (LPWORD) &uVxDId, 
                              (DWORD) (LPDWORD) &cIds ))
            return MMSYSERR_INVALPARAM ;

         // dwParam1 == PnP DevNode

         return (EnableDevNode( dwParam2, uVxDId )) ;
      }
      break ;

      case DRVM_DISABLE:
      {
         DPF( 1, "MODM_DISABLE" ) ;

         // dwParam2 == PnP DevNode

         return (DisableDevNode( dwParam2 )) ;
      }
      break ;

      case DRVM_EXIT:
      {
         DPF( 1, "MODM_EXIT" ) ;

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

         return (fmOpen( phwi, 
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

   pClient = (PFMALLOC) LOWORD( dwUser ) ;
   phwi = pClient -> phwi ;

   switch (uMsg)
   {
      case MODM_CLOSE:
      {
         DPF( 1,"MODM_CLOSE");

         //
         // shut up the FM synthesizer
         //

         fmClose( pClient ) ;

         //
         // we're not used any more
         //

         phwi -> fInUse = FALSE ;

         return MMSYSERR_NOERROR;
      }

      case MODM_RESET:
      {
         DPF( 1,"MODM_RESET");

         //
         //  turn off FM synthesis
         //
         //  note that we increment our 're-entered' counter so that a
         //  background interrupt handler doesn't mess up our resetting
         //  of the synth by calling midiOut[Short|Long]Msg.. just
         //  practicing safe midi. NOTE: this should never be necessary
         //  if a midi app is properly written!
         //

         wFMEntered++ ;
         {
            if (wFMEntered == 1)
            {
               fmReset( phwi ) ;
               dwParam1 = 0L ;
            }
            else
            {
               DPF( 1, "MODM_RESET reentered!" ) ;
               dwParam1 = MIDIERR_NOTREADY ;
            }
         }
         wFMEntered-- ;
         return ( dwParam1 ) ;
      }

      case MODM_DATA:
      {
         // message is in dwParam1
         // make sure we're not being reentered

         wFMEntered++ ;

         if (wFMEntered > 1)
         {
            DPF( 1, "MODM_DATA reentered!" ) ;
            wFMEntered-- ;
            return ( MIDIERR_NOTREADY ) ;
         }

         //
         // if have repeated messages
         //
         if (dwParam1 & 0x00000080)
            // status byte...
            status = LOBYTE( LOWORD( dwParam1 ) ) ;
         else
            dwParam1 = (dwParam1 << 8) | ((DWORD) status) ;

         //
         // if not, have an FM synth message
         //
         fmMidiMessage( phwi, dwParam1 ) ;

         wFMEntered-- ;
         return MMSYSERR_NOERROR;
      }

      case MODM_LONGDATA:
      {
         // far pointer to header in dwParam1

         //
         // make sure we're not being reentered
         //
         wFMEntered++ ;

         if (wFMEntered > 1)
         {
            DPF( 1, "MODM_LONGDATA reentered!" ) ;
            wFMEntered-- ;
            return ( MIDIERR_NOTREADY ) ;
         }

         //
         // check if it's been prepared
         //
         lpHdr = (LPMIDIHDR) dwParam1 ;
         if (!(lpHdr -> dwFlags & MHDR_PREPARED))
         {
            wFMEntered-- ;
            return ( MIDIERR_UNPREPARED ) ;
         }

         lpBuf = lpHdr -> lpData ;
         dwBytesRead = 0 ;
         curByte = *lpBuf ;

         while (TRUE)
         {
            // if its a system realtime message send it and continue
            // this does not affect the running status

            if (curByte >= 0xf8)
               fmMidiMessage( phwi, 0x000000ff & curByte ) ;
            else if (curByte >= 0xf0)
            {
               status = 0 ;     // kill running status
               dwMsg = 0L ;     // throw away any incomplete data
               bBytePos = 0 ;   // start at beginning of message

               switch (curByte)
               {
                  case 0xf0:      // SysEx, ignore
                  case 0xf7:
                     break ;

                  case 0xf4:      // System common, no data
                  case 0xf5:
                  case 0xf6:
                     fmMidiMessage( phwi, 0x000000ff & curByte ) ;
                     break ;

                  case 0xf1:      // System common, one data byte
                  case 0xf3:
                     dwMsg |= curByte;
                     bBytesLeft = 1;
                     bBytePos = 1;
                     break ;

                  case 0xf2:      // System common, 2 data bytes
                     dwMsg |= curByte ;
                     bBytesLeft = 2 ;
                     bBytePos = 1 ;
                     break ;
               }
            }

            // else its a channel message

            else if (curByte >= 0x80)
            {
               status = curByte ;
               dwMsg = 0L ;

               switch (curByte & 0xf0)
               {
                  case 0xc0:      // channel message, one data
                  case 0xd0:
                     dwMsg |= curByte ;
                     bBytesLeft = 1 ;
                     bBytePos = 1 ;
                     break ;

                  case 0x80:      // two bytes
                  case 0x90:
                  case 0xa0:
                  case 0xb0:
                  case 0xe0:
                     dwMsg |= curByte ;
                     bBytesLeft = 2 ;
                     bBytePos = 1 ;
                     break ;
               }
            }

            // else it may an expected data byte

            else if (bBytePos != 0)
            {
               dwMsg |= ((DWORD)curByte) << (bBytePos++ * 8) ;
               if (--bBytesLeft == 0)
               {
                  fmMidiMessage( phwi, dwMsg ) ;

                  if (status)
                  {
                     dwMsg |= status ;
                     bBytesLeft = bBytePos - (BYTE)1 ;
                     bBytePos = 1 ;
                  }
                  else
                  {
                     dwMsg = 0L ;
                     bBytePos = 0 ;
                  }
               }
            }

            // read the next byte if there is one...

            // NOTE!  We have already processed one byte, that
            // has not yet been counted, thus, the pre-inc.

            if (++dwBytesRead >= lpHdr -> dwBufferLength)
               break ;
            curByte = *++lpBuf ;
         }

         // return buffer to client

         lpHdr -> dwFlags |= MHDR_DONE ;
         fmCallback( pClient, MOM_DONE, dwParam1, 0L ) ;

         wFMEntered-- ;
         return MMSYSERR_NOERROR;
      }

      case MODM_SETVOLUME:
      {
         if (NULL == 
               (phwi = DevNodeToHardwareInstance( dwParam2 )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         phwi -> dwSynthVolume = dwParam1 ;
         if (phwi -> fSoftwareVolumeEnabled)
         {
            fmNewVolume( phwi, 0xFF ) ;

            if (phwi -> fnmxdPipe)
               phwi -> fnmxdPipe( phwi -> hpmxd, 
                                  PIPE_MSG_NOTIFY | 
                                     MSOPL_NFY_CONTROL_CHANGE,
                                  phwi -> dn,
                                  dwParam1 ) ;

            return MMSYSERR_NOERROR ;

         }
         else if (phwi -> fnmxdPipe)
         {
            phwi -> fnmxdPipe( phwi -> hpmxd, 
                               PIPE_MSG_CONTROL | 
                                  MSOPL_CTL_SET_SYNTH_VOLUME,
                               phwi -> dn,
                               phwi -> dwSynthVolume ) ;

            return MMSYSERR_NOERROR ;
         }
         else
            return MMSYSERR_NOTSUPPORTED ;
      }

      case MODM_GETVOLUME:
      {
         if (NULL == 
               (phwi = DevNodeToHardwareInstance( dwParam2 )))
         {
            DPF( 1, "devnode not associated with hardware instance???" ) ;
            return MMSYSERR_BADDEVICEID ;
         }

         *((LPDWORD) dwParam1) = phwi ->  dwSynthVolume ;
         return MMSYSERR_NOERROR ;
      }

     default:
       return ( MMSYSERR_NOTSUPPORTED ) ;
   }

   return ( MMSYSERR_NOTSUPPORTED ) ;

} // end of modMessage()

//---------------------------------------------------------------------------
//  End of File: fmsynth.c
//---------------------------------------------------------------------------
