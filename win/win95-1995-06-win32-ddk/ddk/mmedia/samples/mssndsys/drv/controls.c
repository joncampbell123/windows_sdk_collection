//---------------------------------------------------------------------------
//
//  Module: controls.c
//
//  Purpose: Mixer control interface for SNDSYS.DRV
//
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1991 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------

#include <windows.h>

// stop compiler from griping about in-line

#pragma warning (disable:4704)

#define Not_VxD
#include <vmm.h>
#include <configmg.h>

#include <mmsystem.h>
#include <mmddk.h>
#include <mmreg.h>
#include <ctype.h>
#include "mssndsys.h"
#include "driver.h"
#include "mixer.h"

#define SILENCE                 (192)

//***********

//
// The following table of functions determines what functions will get
// called to set the hardware setting for that control.
//

#ifdef MSSNDSYS

CONTROLFUNCTION* BCODE ControlFunctionTable[MAXCONTROLS] = 
{
   MixSetADCHardware,  // Control  0
   MixSetADCHardware,  // Control  1
   MixSetVolume,       // Control  2
   MixSetVolume,       // Control  3
   MixSetMidiVolume,   // Control  4
   MixSetMasterVolume, // Control  5
   MixSetADCHardware,  // Control  6
   MixSetADCHardware,  // Control  7
   MixSetADCHardware,  // Control  8
   MixSetADCHardware,  // Control  9
   MixSetMute,         // Control 10
   MixSetMute,         // Control 11
   MixSetMute,         // Control 12
   MixSetMute,         // Control 13
   MixSetFails,        // Control 14
   MixSetFails,        // Control 15
   MixSetFails,        // Control 16
   MixSetFails,        // Control 17
   MixSetFails         // Control 18
} ;
#endif
#ifdef WASHTON
CONTROLFUNCTION* BCODE ControlFunctionTable[MAXCONTROLS] = 
{
   MixSetADCHardware,  // Control  0
   MixSetADCHardware,  // Control  1
   MixSetVolume,       // Control  2
   MixSetVolume,       // Control  3
   MixSetMidiVolume,   // Control  4
   MixSetVolume,       // Control  5
   MixSetMonoVolume,   // Control  6
   MixSetMasterVolume, // Control  7
   MixSetADCHardware,  // Control  8
   MixSetADCHardware,  // Control  9
   MixSetADCHardware,  // Control 10
   MixSetADCHardware,  // Control 11
   MixSetADCHardware,  // Control 12
   MixSetADCHardware,  // Control 13
   MixSetMute,         // Control 14
   MixSetMute,         // Control 15
   MixSetMute,         // Control 16
   MixSetMute,         // Control 17
   MixSetMute,         // Control 18
   MixSetMute,         // Control 19
   MixSetFails,        // Control 20
   MixSetFails,        // Control 21
   MixSetFails,        // Control 22
   MixSetFails,        // Control 23
   MixSetFails,        // Control 24
   MixSetFails,        // Control 25
   MixSetFails         // Control 26
} ;
#endif

#ifdef NOVA
CONTROLFUNCTION* BCODE ControlFunctionTable[MAXCONTROLS] = 
{
   MixSetADCHardware,  // Control  0
   MixSetADCHardware,  // Control  1
   MixSetVolume,       // Control  2
   MixSetVolume,       // Control  3
   MixSetMidiVolume,   // Control  4
   MixSetMonoVolume,   // Control  5
   MixSetMasterVolume, // Control  6
   MixSetADCHardware,  // Control  7
   MixSetADCHardware,  // Control  8
   MixSetADCHardware,  // Control  9
   MixSetADCHardware,  // Control 10
   MixSetADCHardware,  // Control 11
   MixSetMute,         // Control 12
   MixSetMute,         // Control 13
   MixSetMute,         // Control 14
   MixSetMute,         // Control 15
   MixSetMute,         // Control 16
   MixSetFails,        // Control 17
   MixSetFails,        // Control 18
   MixSetFails,        // Control 19
   MixSetFails,        // Control 20
   MixSetFails,        // Control 21
   MixSetFails         // Control 22
} ;
#endif

//------------------------------------------------------------------------
//  MMRESULT MixSetMidiVolume
//
//  Description:
//     Sets the midi output device volume.
//
//  Parameters:
//     PMIXERHARDWAREINSTANCE phwi
//        pointer to hardware instance
//
//     LPMIXERCONTROLDETAILS pmcd
//        pointer to mixer control details structure
//
//  Return Value:
//     MMRESULT
//        MMSYSERR_NOERROR if everything A-OK.
//
//
//------------------------------------------------------------------------

MMRESULT FAR PASCAL MixSetMidiVolume
(
    PHARDWAREINSTANCE       phwi, 
    LPMIXERCONTROLDETAILS   pmcd
)
{
   WORD            wLeftVol, wRightVol;
#ifdef AZTECH
   WORD wTempVol; 
#endif
   PMIXERINSTANCE  pmi;

   DPF( 1, "MixSetMidiVolume" ) ;

   if (phwi -> fnmxdPipe)
   {
      pmi = phwi -> pmi ;

      wLeftVol = (WORD) pmi -> dwValue[ VOL_OUTMIDI ][ 0 ] ;
      wRightVol = (WORD) pmi -> dwValue[ VOL_OUTMIDI ][ 1 ] ;

      //
      // If the master mute or the midi mute is set, mute
      // the line.
      //

      if ((pmi -> dwValue[ MUTE_OUTMIDI ][ 0 ]) ||
         (pmi -> dwValue[ MUTE_OUTLINE ][ 0 ]))
      {
         wLeftVol = 0 ;
         wRightVol = 0 ;
      }

      //
      // If the control is at the min, also mute it.
      //

      if (pmi -> dwValue[ VOL_OUTMIDI ][ 0 ] ==
            pmi -> mxc[ VOL_OUTMIDI ].Bounds.dwMinimum)
         wLeftVol = 0x0000 ;
      if (pmi -> dwValue[ VOL_OUTMIDI ][ 1 ] ==
            pmi -> mxc[ VOL_OUTMIDI ].Bounds.dwMinimum)
         wRightVol = 0x0000 ;

#ifdef AZTECH
      wTempVol  = wLeftVol ;        // OPL3 left/right reverse
      wLeftVol  = wRightVol ;       // OPL3 left/right reverse
      wRightVol = wTempVol ;        // OPL3 left/right reverse
#endif

      phwi -> fnmxdPipe( phwi -> hpmxd, 
                         PIPE_MSG_CONTROL | 
                            MSOPL_CTL_SET_SYNTH_VOLUME,
                         phwi -> dn,
                         MAKELONG( wLeftVol, wRightVol ) ) ;
   }

   return ( MMSYSERR_NOERROR ) ;

} // end of MixSetMidiVolume()

//------------------------------------------------------------------------
//  MMRESULT MixSetVolume
//
//  Description:
//     Sets the volume for a control.
//
//  Parameters:
//     PMIXERHARDWAREINSTANCE phwi
//        pointer to hardware instance
//
//     LPMIXERCONTROLDETAILS pmcd
//        pointer to mixer control detailss structure
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

MMRESULT FAR PASCAL MixSetVolume
(
    PHARDWAREINSTANCE       phwi,
    LPMIXERCONTROLDETAILS   pmcd
)
{
   BYTE            bNumStepsL, bNumStepsR ;
   DWORD           dwControlID, dwMuteControlID ;
   PMIXERINSTANCE  pmi;

   DPF( 1, "MixSetVolume" ) ;

   pmi = phwi -> pmi ;
   dwControlID = pmcd -> dwControlID ;
   dwMuteControlID = (dwControlID - VOL_OUTAUX1) + MUTE_OUTAUX1 ;

   //
   // Convert to steps of 1.5dB attenuation for the CODEC
   //

   bNumStepsL =
      VolLinearToLog( (WORD) pmi -> dwValue[ dwControlID ][ 0 ] ) ;
   bNumStepsR =
      VolLinearToLog( (WORD) pmi -> dwValue[ dwControlID ][ 1 ] ) ;

   //
   // Add master attenuation
   //

   bNumStepsL +=
      VolLinearToLog( (WORD) pmi -> dwValue[ VOL_OUTLINE ][ 0 ] ) ;
   bNumStepsR +=
      VolLinearToLog( (WORD) pmi -> dwValue[ VOL_OUTLINE ][ 1 ] ) ;

#ifdef AZTECH
   //
   // 0x08 to (CODEC)CS4231's AUX1/AUX2/LINE-IN
   //

   if (dwControlID != VOL_OUTDAC )      
   {                                    
       bNumStepsL += 0x08 ;             // Max volume
       bNumStepsR += 0x08 ;             // Max volume
   }                                    

   // Mute when out of range...         

   if (bNumStepsL > 0x1F)               
      bNumStepsL = 0x9F ;               
   if (bNumStepsR > 0x1F)               
      bNumStepsR = 0x9F ;               
#endif

   //
   // If either this line's mute or the lineout (master)
   //    mute is on, mute it.
   //

   if ((pmi -> dwValue[ dwMuteControlID ][ 0 ]) ||
       (pmi -> dwValue[ MUTE_OUTLINE ][ 0 ]))
   {
#ifdef AZTECH
      bNumStepsL = 0x9F ;
      bNumStepsR = 0x9F ;
#endif
#ifdef MSSNDSYS
      bNumStepsL = 0x80 ;
      bNumStepsR = 0x80 ;
#endif
   }

D(
   char szDebug[ 80 ] ;

   wsprintf( szDebug, "bNumStepsL = %d, bNumStepsR = %d",
             bNumStepsL, bNumStepsR ) ;
   DPF( 1, szDebug ) ;
)

   //
   // Note that we perform this compare after setting the mute
   // bit and will reset the mute - the number of cycles wasted
   // vs. the compare of special casing the 0x80 is a wash.
   //

   switch (dwControlID)
   {
      case VOL_OUTAUX1:
#ifndef AZTECH
         // AUX1 added gain in the K grade parts...

         if (phwi -> wCODECClass != CODEC_J_CLASS)
         {
            // Mute when out of range...

            if (bNumStepsL > 0x1F)
               bNumStepsL = 0x80 ;
            if (bNumStepsR > 0x1F)
               bNumStepsR = 0x80 ;
         }
         else
         {
            // Mute when out of range...

            if (bNumStepsL > 0x0F)
               bNumStepsL = 0x80 ;
            if (bNumStepsR > 0x0F)
               bNumStepsR = 0x80 ;
         }
#endif

         if (0 == (HardwareAcquire( phwi, phwi -> wIOAddressCODEC, 
                                    MSS_ASS_ACQUIRE_CODEC ) & 0x7FFF))
         {
            CODEC_RegWrite( phwi, REGISTER_LEFTAUX1, bNumStepsL ) ;
            CODEC_RegWrite( phwi, REGISTER_RIGHTAUX1, bNumStepsR ) ;
            HardwareRelease( phwi, phwi -> wIOAddressCODEC, 
                             MSS_ASS_ACQUIRE_CODEC ) ;
         }

         break ;

      case VOL_OUTDAC:

#ifndef AZTECH
         // Mute when out of range...

         if (phwi -> wCODECClass != CODEC_J_CLASS)
         {
            if (bNumStepsL > 0x3F)
               bNumStepsL = 0x80 ;
            if (bNumStepsR > 0x3F)
               bNumStepsR = 0x80 ;
         }
         else
         {
            // Don't mute DAC, just fully attenuate on 'J' to avoid 
            // garbage with muting other sources (bug in chip).

            if (bNumStepsL > 0x3F)
               bNumStepsL = 0x3F ;
            if (bNumStepsR > 0x3F)
               bNumStepsR = 0x3F ;
         }
#endif

         phwi -> bDACToCODECLeft = bNumStepsL ;
         phwi -> bDACToCODECRight = bNumStepsR ;

         // Only write to hardware when active...  it should
         // be acquired already, no need to do it again.

         if (phwi -> bIntUsed == INT_WAVEOUT)
         {
            CODEC_RegWrite( phwi, REGISTER_LEFTOUTPUT, bNumStepsL ) ;
            CODEC_RegWrite( phwi, REGISTER_RIGHTOUTPUT, bNumStepsR ) ;
         }
         break ;
#ifdef WASHTON

      case VOL_OUTCD:                                                   

         if ((phwi -> wCODECClass == CODEC_KPLUS_CLASS)                 
             && (0 == (HardwareAcquire( phwi, phwi -> wIOAddressCODEC,  
                                    MSS_ASS_ACQUIRE_CODEC ) & 0x7FFF))) 
         {                                                              
            CODEC_RegWrite( phwi, REGISTER_LEFTLINEINPUT, bNumStepsL ) ;
            CODEC_RegWrite( phwi, REGISTER_RIGHTLINEINPUT, bNumStepsR ) ;
            HardwareRelease( phwi, phwi -> wIOAddressCODEC,             
                             MSS_ASS_ACQUIRE_CODEC ) ;                  
         }                                                              
         break ;                                                        

#endif
   }

   return ( MMSYSERR_NOERROR ) ;

} // end of MixSetVolume()

#ifdef AZTECH
//------------------------------------------------------------------------
//  MMRESULT MixSetMonoVolume
//
//  Description:
//     Sets the Mono volume for a control. 
//
//  Parameters:
//     PMIXERHARDWAREINSTANCE phwi
//        pointer to hardware instance
//
//     LPMIXERCONTROLDETAILS pmcd
//        pointer to mixer control detailss structure
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

MMRESULT FAR PASCAL MixSetMonoVolume
(
    PHARDWAREINSTANCE       phwi,
    LPMIXERCONTROLDETAILS   pmcd
)
{
   BYTE            bNumSteps, bMasterL, bMasterR ;
   DWORD           dwControlID ;
   PMIXERINSTANCE  pmi;

   DPF( 1, "MixSetVolume" ) ;

   pmi = phwi -> pmi ;
   dwControlID = pmcd -> dwControlID ;

   //
   // Convert to steps of 3dB attenuation for the CODEC
   //

   bNumSteps =
      VolLinearToLog( (WORD) pmi -> dwValue[ dwControlID ][ 0 ] ) ;

   //
   // Add master attenuation
   //

   bMasterL =
      VolLinearToLog( (WORD) pmi -> dwValue[ VOL_OUTLINE ][ 0 ] ) ;

   bMasterR =
      VolLinearToLog( (WORD) pmi -> dwValue[ VOL_OUTLINE ][ 1 ] ) ;

   if (bMasterL > bMasterR)
      bNumSteps += bMasterR;
   else
      bNumSteps += bMasterL;

   // Shift from 0xFFFFF to 0xFFFF

   bNumSteps = bNumSteps >>1;


   // Mute when out of range...

   if (bNumSteps >= 0x0F)
      bNumSteps = 0x4F ;

   //
   // If either this line's mute or the lineout (master)
   //    mute is on, mute it.
   //

   if ((pmi -> dwValue[ MUTE_OUTMIC ][ 0 ]) ||
      (pmi -> dwValue[ MUTE_OUTLINE ][ 0 ]))
       bNumSteps = 0x8F ;

D(
   char szDebug[ 80 ] ;

   wsprintf( szDebug, "bNumSteps = %d", bNumSteps ) ;
   DPF( 1, szDebug ) ;
)

    if ((phwi -> wCODECClass == CODEC_KPLUS_CLASS)
       && (0 == (HardwareAcquire( phwi, phwi -> wIOAddressCODEC,
                               MSS_ASS_ACQUIRE_CODEC ) & 0x7FFF)))
        {
        CODEC_RegWrite( phwi, REGISTER_MONOINPUT, bNumSteps ) ;
        HardwareRelease( phwi, phwi -> wIOAddressCODEC,
                         MSS_ASS_ACQUIRE_CODEC ) ;
        }

   return ( MMSYSERR_NOERROR ) ;

} // end of MixSetMonoVolume()

#endif

//--------------------------------------------------------------------------
//  
//  MMRESULT MixSetADCHardware
//  
//  Description:
//      Sets the ADC hardware...
//  
//  Parameters:
//      PMIXERHARDWAREINSTANCE phwi
//         pointer to hardware instance
//
//      LPMIXERCONTROLDETAILS pmcd
//         pointer to control details
//
//  Return (MMRESULT):
//      MMSYSERR_NOERROR if no problem
//  
//  
//--------------------------------------------------------------------------

MMRESULT FAR PASCAL MixSetADCHardware
(
    PHARDWAREINSTANCE       phwi,
    LPMIXERCONTROLDETAILS   pmcd
)
{
   BYTE             bSource;
   BYTE             bMicGainL = 0, bMicGainR = 0 ;
   UINT             uVolControl;
   BYTE             bNumStepsL, bNumStepsR;
   BYTE             bADCToCODECLeft, bADCToCODECRight;
   DWORD            dwADCControlID, dwMuxControlID ;
   PMIXERINSTANCE   pmi ;

   DPF( 1, "MixSetADCHardware" ) ;

   //
   // Determine if we should be changing the hardware
   //

   pmi = phwi -> pmi ;

   if (phwi -> dwCurCODECOwner == NULL)
   {
      DPF( 1, "MixSetADCHardware: No CODEC owner..." ) ;
      return MMSYSERR_NOERROR ;
   }

   if (phwi -> bIntUsed != INT_WAVEIN)
   {
      DPF( 1, "MixSetADCHardware: ADC not active." ) ;
      return MMSYSERR_NOERROR ;
   }

   if (phwi -> dwCurCODECOwner != phwi -> dwLowUser)
      dwMuxControlID = MUX_WAVEIN ;
   else
      dwMuxControlID = MUX_VOICEIN ;

   //
   // Is this a control of the currently selected source?
   // If so, just return...
   //

   dwADCControlID = pmi -> dwValue[ dwMuxControlID ][ 0 ] +
      (DWORD)((dwMuxControlID == MUX_WAVEIN) ?
                  VOL_W_INAUX1 : VOL_V_INAUX1) ;

   if (pmcd && (pmcd -> dwControlID != dwMuxControlID) &&
               (pmcd -> dwControlID != dwADCControlID))
   {
      DPF( 1, "MixSetADCHardware: User is adjusting an inactive input level..." ) ;
      return ( MMSYSERR_NOERROR ) ;
   }

   //
   // If we got here, we know that the user is altering a control that
   // is for the currently active device
   //

#ifdef MSSNDSYS
   if (pmi -> dwValue[ dwMuxControlID ][ 0 ] == MUXINPUT_MIC)
   {
      uVolControl =
         (phwi -> dwCurCODECOwner != phwi -> dwLowUser) ? 
            VOL_W_INMIC : VOL_V_INMIC ;

      bNumStepsL = VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 0 ] ) ) ;

#ifdef STEREOMIC
      bNumStepsR = VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 1 ] ) ) ;
#else
      // Mic is mono, so use Left setting for Right also...
      bNumStepsR = bNumStepsL ;
#endif

      // Don't use gain on Compaq BA hardware...
      // doesn't seem to like it and peaks quite easily.

      if (phwi -> wHardwareOptions & DAK_COMPAQBA)
      {
         // Bounding...

         if (bNumStepsL > 0x0F)
            bNumStepsL = 0x0F ;

         if (bNumStepsR > 0x0F)
            bNumStepsR = 0x0F ;

         // 16 steps for 22.5 dB of gain...

         bNumStepsL = 0x0F - bNumStepsL ;
         bNumStepsR = 0x0F - bNumStepsR ;
      }
      else
      {
         // Bounding...

         if (bNumStepsL > 28)
            bNumStepsL = 28 ;

         if (bNumStepsR > 28)
            bNumStepsR = 28 ;

         // 28 steps for 42.5 dB of gain...

         bNumStepsL = 28 - bNumStepsL ;
         bNumStepsR = 28 - bNumStepsR ;

         // set 20 dB gain accordingly

         if (bNumStepsL > 13)
         {
            bMicGainL = 0x20 ;
            bNumStepsL -= 13 ;
         }

         if (bNumStepsR > 13)
         {
            bMicGainR = 0x20 ;
            bNumStepsR -= 13 ;
         }
      }

      //
      // Now set bSource to Mic for hardware
      //
      bSource = 0x80;
   }
   else
   {
      uVolControl =
         (phwi -> dwCurCODECOwner != phwi -> dwLowUser) ?
            VOL_W_INAUX1 : VOL_V_INAUX1 ;

      bNumStepsL =
         VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 0 ] ) ) ;
      bNumStepsR =
         VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 1 ] ) ) ;

      // Bounding...

      if (bNumStepsL > 0x0F)
         bNumStepsL = 0x0F ;

      if (bNumStepsR > 0x0F)
         bNumStepsR = 0x0F ;

      bNumStepsL = 0x0F - bNumStepsL ;
      bNumStepsR = 0x0F - bNumStepsR ;

      //
      // Now set bSource to Line-In (for hardware)
      //

      bSource = 0x40;

   }
#endif
#ifdef AZTECH
// Add 2 recording sources (CD/Mixer) as follows:
   switch (pmi -> dwValue[ dwMuxControlID ][ 0 ])
   {
        case MUXINPUT_MIC :

            uVolControl =
               (phwi -> dwCurCODECOwner != phwi -> dwLowUser) ?
                  VOL_W_INMIC : VOL_V_INMIC ;

            bNumStepsL = VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 0 ] ) ) ;
            bNumStepsR = VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 1 ] ) ) ;

            //
            // Now set bSource to Mic for hardware
            //
            bSource = 0x80;

            break;

        case MUXINPUT_AUX1 :

            uVolControl =
               (phwi -> dwCurCODECOwner != phwi -> dwLowUser) ?
                  VOL_W_INAUX1 : VOL_V_INAUX1 ;

            bNumStepsL = VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 0 ] ) ) ;
            bNumStepsR = VolLinearToLog( LOWORD( pmi -> dwValue[ uVolControl ][ 1 ] ) ) ;

            //
            // Now set bSource to Line-In (for hardware)
            //
            bSource = 0x40;

            break;
#ifdef AZTECH
#ifdef WASHTON 

        case MUXINPUT_CD :

            bNumStepsL = VolLinearToLog( LOWORD( pmi -> dwValue[ VOL_W_INCD ][ 0 ] ) ) ;
            bNumStepsR = VolLinearToLog( LOWORD( pmi -> dwValue[ VOL_W_INCD ][ 1 ] ) ) ;

            //
            // Now set bSource to CD (for hardware)
            //
            bSource = 0x00;

            break;

#endif                          

        case MUXINPUT_MIX :

            bNumStepsL = VolLinearToLog( LOWORD( pmi -> dwValue[ VOL_W_INMIX ][ 0 ] ) ) ;
            bNumStepsR = VolLinearToLog( LOWORD( pmi -> dwValue[ VOL_W_INMIX ][ 1 ] ) ) ;

            //
            // Now set bSource to Line-In (for hardware)
            //
            bSource = 0xC0;

            break;
#endif
    }

   // Bounding...                       

   if (bNumStepsL > 0x0F)               
      bNumStepsL = 0x0F ;               

   if (bNumStepsR > 0x0F)               
      bNumStepsR = 0x0F ;               

   bNumStepsL = 0x0F - bNumStepsL ;     
   bNumStepsR = 0x0F - bNumStepsR ;     

#endif

   bADCToCODECLeft =  bSource | bMicGainL | bNumStepsL;
   bADCToCODECRight = bSource | bMicGainR | bNumStepsR;

   AssertT( (bADCToCODECLeft  & 0xC0) != 0xC0 );
   AssertT( (bADCToCODECRight & 0xC0) != 0xC0 );

#pragma message( REMIND( "Auto-calibrate when changing input levels on 'J'" ) )
  
   if (0 == (HardwareAcquire( phwi, phwi -> wIOAddressCODEC, 
                              MSS_ASS_ACQUIRE_CODEC ) & 0x7FFF))
   {
      CODEC_RegWrite( phwi, REGISTER_LEFTINPUT,  bADCToCODECLeft );
      CODEC_RegWrite( phwi, REGISTER_RIGHTINPUT, bADCToCODECRight );
      HardwareRelease( phwi, phwi -> wIOAddressCODEC, MSS_ASS_ACQUIRE_CODEC);
   }

} // MixSetADCHardware()

//--------------------------------------------------------------------------
//  
//  MMRESULT MixSetMasterVolume
//  
//  Description:
//      Sets the master volume
//  
//  Parameters:
//      PMIXERHARDWAREINSTANCE phwi
//         pointer to hardware instance
//
//      LPMIXERCONTROLDETAILS pmcd
//         pointer to control details
//  
//  Return (MMRESULT):
//      MMSYSERR_NOERROR if no problem
//  
//  
//--------------------------------------------------------------------------

MMRESULT FAR PASCAL MixSetMasterVolume
(   
    PHARDWAREINSTANCE       phwi, 
    LPMIXERCONTROLDETAILS   pmcd
)
{
   WORD            wLeftVol, wRightVol;
#ifdef AZTECH
   WORD wTempVol;
#endif
   PMIXERINSTANCE  pmi;

   DPF( 1, "MixSetMasterVolume" ) ;

   if (phwi -> fnmxdPipe)
   {
      pmi = phwi -> pmi ;

      wLeftVol = (WORD) pmi -> dwValue[ VOL_OUTLINE ][ 0 ] ;
      wRightVol = (WORD) pmi -> dwValue[ VOL_OUTLINE ][ 1 ] ;

      //
      // If the master mute or the midi mute is set, mute
      // the line.
      //

      if (pmi -> dwValue[ MUTE_OUTLINE ][ 0 ])
      {
         wLeftVol = 0 ;
         wRightVol = 0 ;
      }
#ifdef AZTECH
      wTempVol  = wLeftVol ;        // OPL3 left/right reverse
      wLeftVol  = wRightVol ;       // OPL3 left/right reverse
      wRightVol = wTempVol ;        // OPL3 left/right reverse
#endif

      phwi -> fnmxdPipe( phwi -> hpmxd, 
                            PIPE_MSG_CONTROL | 
                               MSOPL_CTL_SET_MASTER_VOLUME,
                         phwi -> dn,
                         MAKELONG( wLeftVol, wRightVol ) ) ;
   }

   return ( MixUpdateGlobalLevels( phwi ) ) ;

} // MixSetMasterVolume()

//--------------------------------------------------------------------------
//  
//  MMRESULT MixSetMute
//  
//  Description:
//      Turns the mute on and off
//  
//  Parameters:
//      PMIXERHARDWAREINSTANCE phwi
//         pointer to hardware instance
//
//      LPMIXERCONTROLDETAILS pmcd
//         pointer to control details
//
//  Return (MMRESULT):
//      MMSYSERR_NOERROR if no problem
//  
//  
//--------------------------------------------------------------------------

MMRESULT FAR PASCAL MixSetMute
(
    PHARDWAREINSTANCE       phwi,
    LPMIXERCONTROLDETAILS   pmcd
)
{
   DWORD           dwControlID ;
   PMIXERINSTANCE  pmi ;

   pmi = phwi -> pmi ;
   dwControlID = pmcd -> dwControlID ;

   //
   // The value in the dwValue field has already been updated.
   // Nothing to do now, but update all the actual levels.
   //

   switch (pmcd -> dwControlID)
   {
      case MUTE_OUTLINE:
         MxdUpdateLine( phwi, 
                        DEST_LINEOUT, (UINT) -1,
                        (BOOL) pmi -> dwValue[ dwControlID ][ 0 ],
                        MXDUPDATELINE_ACTIONF_MUTESTATUS |
                           MXDUPDATELINE_ACTIONF_DESTINATION ) ;
         break ;

      default:
      {
         DWORD  dwRelSource ;

         // NOTE!  Mute controls map 1:1 to relative sources on
         // the DEST_LINEOUT destination...

         dwRelSource = pmcd -> dwControlID - MUTE_OUTAUX1 ;
         MxdUpdateLine( phwi,
                        DEST_LINEOUT,
                        pmi -> auSourceMap[ DEST_LINEOUT ][ dwRelSource ],
                        (BOOL) pmi -> dwValue[ dwControlID ][ 0 ],
                        MXDUPDATELINE_ACTIONF_MUTESTATUS |
                           MXDUPDATELINE_ACTIONF_SOURCE ) ;
      }
   }

   MixUpdateGlobalLevels( phwi ) ;

   return MMSYSERR_NOERROR ;

} // MixSetMute()

//--------------------------------------------------------------------------
//  
//  MMRESULT MixSetFails
//  
//  Description:
//      Fails the {set/get} control details.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         pointer to hardware instance
//
//      LPMIXERCONTROLDETAILS pmcd
//  
//  Return (MMRESULT):
//      MIXERR_INVALCONTROL
//  
//  
//--------------------------------------------------------------------------

MMRESULT FAR PASCAL MixSetFails
(
    PHARDWAREINSTANCE       phwi,
    LPMIXERCONTROLDETAILS   pmcd
)
{
 //
 // This routine is called when a caller attempted to set something that
 // cannot/should not be set (VUMeter, etc.)
 //
 return MIXERR_INVALCONTROL ;

} // MixSetFails()

//--------------------------------------------------------------------------
//  
//  MMRESULT MixUpdateGlobalLevels
//  
//  Description:
//      Updates all controls.
//  
//  Parameters:
//      PMIXERHARDWAREINSTANCE phwi
//         pointer to hardware instance
//
//  Return (MMRESULT):
//      MMSYSERR_NOERROR if no problem
//  
//  
//--------------------------------------------------------------------------

MMRESULT PASCAL MixUpdateGlobalLevels
(
    PHARDWAREINSTANCE   phwi
)
{    
   MMRESULT                      Result ;
   MMRESULT                      Return = MMSYSERR_NOERROR ;
   MIXERCONTROLDETAILS           mcd ;
   MIXERCONTROLDETAILS_UNSIGNED  mcd_u[ 2 ] ;
   PMIXERINSTANCE                pmi ;

   pmi = phwi -> pmi ;

   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.paDetails = &mcd_u ;
   mcd.cbDetails = sizeof( MIXERCONTROLDETAILS_SIGNED ) ;
    
   //
   // Adjust MIDI out volume.
   //

   if (phwi -> wHardwareOptions & DAK_FMSYNTH)
   {
      mcd.dwControlID = VOL_OUTMIDI ;
      mcd.cChannels = pmi -> auControlMap[ VOL_OUTMIDI ][ CM_CHANNELS ] ;
      mcd.cMultipleItems = 0 ;
      mcd_u[ 0 ].dwValue =
         pmi -> dwValue[ VOL_OUTMIDI ][ 0 ] ;
      mcd_u[ 1 ].dwValue =
         pmi -> dwValue[ VOL_OUTMIDI ][ 1 ] ;

      Result = MixSetMidiVolume( phwi, &mcd ) ;
      if ( Result != MMSYSERR_NOERROR )
         Return = Result;
   }

   //
   // Adjust AUX1 volume.
   //

   mcd.dwControlID = VOL_OUTAUX1 ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTAUX1 ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0 ;
   mcd_u[ 0 ].dwValue =
      pmi -> dwValue[ VOL_OUTAUX1 ][ 0 ] ;
   mcd_u[ 1 ].dwValue =
      pmi -> dwValue[ VOL_OUTAUX1 ][ 1 ] ;

   Result = MixSetVolume( phwi, &mcd ) ;

   if ( Result != MMSYSERR_NOERROR )
      Return = Result ;

   //
   // Adjust DAC volume.
   //

   mcd.dwControlID = VOL_OUTDAC ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTDAC ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd_u[ 0 ].dwValue =
      pmi -> dwValue[ VOL_OUTDAC ][ 0 ] ;
   mcd_u[ 1 ].dwValue =
      pmi -> dwValue[ VOL_OUTDAC ][ 1 ] ;

   Result = MixSetVolume( phwi, &mcd ) ;

   if ( Result != MMSYSERR_NOERROR )
      Return = Result;

#ifdef WASHTON                  

   //
   // Adjust CD volume.     
   //

   mcd.dwControlID = VOL_OUTCD ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTCD ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0 ;
   mcd_u[ 0 ].dwValue =
      pmi -> dwValue[ VOL_OUTCD ][ 0 ] ;
   mcd_u[ 1 ].dwValue =
      pmi -> dwValue[ VOL_OUTCD ][ 1 ] ;

   Result = MixSetVolume( phwi, &mcd ) ;

   if ( Result != MMSYSERR_NOERROR )
      Return = Result ;

#endif                          

#ifdef AZTECH
   //
   // Adjust mono MIC output volume.    
   //

   mcd.dwControlID = VOL_OUTMIC ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTMIC ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0 ;
   mcd_u[ 0 ].dwValue =
      pmi -> dwValue[ VOL_OUTMIC ][ 0 ] ;

   Result = MixSetMonoVolume( phwi, &mcd ) ;

   if ( Result != MMSYSERR_NOERROR )
      Return = Result;
#endif

   return Return;

} // MixUpdateGlobalLevels()

//---------------------------------------------------------------------------
//  End of File: controls.c
//---------------------------------------------------------------------------
