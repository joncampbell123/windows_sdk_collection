//---------------------------------------------------------------------------
//
//  Module:   init.c
//
//  Purpose:
//     Hardware initialization routines.
//
//     NOTE!: Version 4.00 of this driver requires the presence of the
//     VxD.  Because CONFIGMG controls the hardware configuration,
//     all detection has been removed from this driver.  It also
//     is enhanced mode only, no standard mode support is included
//     nor required for Chicago.
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
#include "config.h"
#define NOSTR
#include "mssndsys.h"
#include "driver.h"
#include "mixer.h"

#include <verinfo.h>
#include <wsshlpid.h>
//
// public data
// 

WORD               gwFailCount = 0 ;
HMODULE     ghModule ;              // our module handle

WORD        gwDriverFlags ;         // various state/configuration flags

#ifdef DEBUG
    WORD    wDebugLevel = 5 ;       // debug level
    //
    // these strings are NOT accessed at interrupt time, so they can
    // be based in discardable code (the init seg in this case).
    //
    static char BCODE STR_DRIVER[]     = "driver" ;
    static char BCODE STR_MMDEBUG[]    = "mmdebug" ;

    //
    // these strings are accessed at interrupt time, so they must be in
    // the fixed DS
    //
    char STR_PROLOGUE[] = "SNDSYS: " ;
    char STR_CRLF[]     = "\r\n" ;
    char STR_SPACE[]    = " " ;
#endif

static char BCODE gszRegKeyLeftMaster[]     = INI_STR_LEFTMASTER ;
static char BCODE gszRegKeyRightMaster[]    = INI_STR_RIGHTMASTER ;
static char BCODE gszRegKeyLeftLineIn[]     = INI_STR_LEFTLINEIN ;
static char BCODE gszRegKeyRightLineIn[]    = INI_STR_RIGHTLINEIN ;
static char BCODE gszRegKeyLeftDAC[]        = INI_STR_LEFTDAC ;
static char BCODE gszRegKeyRightDAC[]       = INI_STR_RIGHTDAC ;
#ifdef AZTECH
    static char BCODE gszRegKeyLeftCD[]         = INI_STR_LEFTCD ;
    static char BCODE gszRegKeyRightCD[]        = INI_STR_RIGHTCD ;
    static char BCODE gszRegKeyMonoMic[]        = INI_STR_MONOMIC ;
#endif

static char BCODE gszRegKeyLeftWaveADCMic[] =   INI_STR_LEFTWAVEADCMIC ;
static char BCODE gszRegKeyRightWaveADCMic[] =  INI_STR_RIGHTWAVEADCMIC ;
static char BCODE gszRegKeyLeftWaveADCLine[] =  INI_STR_LEFTWAVEADCLINE ;
static char BCODE gszRegKeyRightWaveADCLine[] = INI_STR_RIGHTWAVEADCLINE ;
#ifdef AZTECH
    static char BCODE gszRegKeyLeftWaveADCCD[] =    INI_STR_LEFTWAVEADCCD ;
    static char BCODE gszRegKeyRightWaveADCCD[] =   INI_STR_RIGHTWAVEADCCD ;
    static char BCODE gszRegKeyLeftWaveADCMix[] =   INI_STR_LEFTWAVEADCMIX ;
    static char BCODE gszRegKeyRightWaveADCMix[] =  INI_STR_RIGHTWAVEADCMIX ;
#endif
static char BCODE gszRegKeyWaveADCSource[] =    INI_STR_WAVEADCSOURCE ;
static char BCODE gszIniDefWaveADCSource[] =    INI_DEF_WAVEADCSOURCE ;

static char BCODE gszRegKeyLeftVoiceADCMic[] =   INI_STR_LEFTVOICEADCMIC ;
static char BCODE gszRegKeyRightVoiceADCMic[] =  INI_STR_RIGHTVOICEADCMIC ;
static char BCODE gszRegKeyLeftVoiceADCLine[] =  INI_STR_LEFTVOICEADCLINE ;
static char BCODE gszRegKeyRightVoiceADCLine[] = INI_STR_RIGHTVOICEADCLINE ;
static char BCODE gszRegKeyVoiceADCSource[] =    INI_STR_VOICEADCSOURCE ;
static char BCODE gszIniDefVoiceADCSource[] =    INI_DEF_VOICEADCSOURCE ;

static char BCODE gszRegKeyLeftSynth[]      = INI_STR_LEFTSYNTH ;
static char BCODE gszRegKeyRightSynth[]     = INI_STR_RIGHTSYNTH ;

static char BCODE gszRegKeyMute[]           = INI_STR_MUTE ;
static char BCODE gszRegKeyMuteMsg[]        = INI_STR_MUTEMSG ;

static char BCODE gszRegValInputLineIn[]    = INI_INPUT_LINEIN ;
static char BCODE gszRegValInputAux[]       = INI_INPUT_AUX ;
static char BCODE gszRegValInputMic[]       = INI_INPUT_MIC ;
#ifdef AZTECH
    static char BCODE gszRegValInputCD[]        = INI_INPUT_CD ;
#endif
static char BCODE gszRegValInputOutput[]    = INI_INPUT_OUTPUT ;

static char BCODE gszConfig[]               = "\\Config" ;


// hardware specific globals

WORD  gwGlobalStatus = STATUS_NO_ERROR ;

// Filter proc for F1 help

FARPROC  fnNextMessageFilterProc ;

// DMA page registers

const BYTE gabDMAPageReg[] =
        { 0x87, 0x83, 0x81, 0x82, 0x00, 0x8b, 0x89, 0x8a } ;

// Hardware instance list

PHARDWAREINSTANCE pHardwareList ;

//------------------------------------------------------------------------
//  VOID HardErrorMsgBox
//
//  Description:
//     Displays an error box.
//
//  Parameters:
//     WORD wStringID
//        string ID from string table
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

static VOID NEAR PASCAL HardErrorMsgBox
(
    WORD            wStringId
)
{
   char    szAlert[50];
   char    szErrorBuffer[512];     // buffer for error messages

   LoadString( ghModule, SR_ALERT, szAlert, sizeof( szAlert ) ) ;
   LoadString( ghModule, wStringId,
               szErrorBuffer, sizeof( szErrorBuffer ) ) ;
   MessageBox( NULL, szErrorBuffer, szAlert,
               MB_OK | MB_SYSTEMMODAL | MB_ICONHAND ) ;

} // end of HardErrorMsgBox()

//------------------------------------------------------------------------
//  VOID AlertBox
//
//  Description:
//     Displays an alert box.
//
//  Parameters:
//     HWND hwnd
//        handle to parent window
//
//     WORD wStrId
//        string ID of string in string table
//
//     ...
//
//  Return Value:
//     Nothing.
//
//
//------------------------------------------------------------------------

VOID FAR cdecl AlertBox
(
    HWND            hwnd,
    WORD            wStrId,
    ...
)
{
    char  szAlert[ 50 ] ; 
    char  szFormat[ 256 ] ;
    char  ach[ 512 ] ;


    LoadString( ghModule, SR_ALERT, szAlert, sizeof( szAlert ) ) ;
    LoadString( ghModule, wStrId, szFormat, sizeof( szFormat ) ) ;
    wvsprintf( ach, szFormat, (LPSTR)((LPWORD) &wStrId + 1) ) ;

    MessageBox( hwnd, ach, szAlert, MB_ICONINFORMATION | MB_OK ) ;

} // end of AlertBox()

//------------------------------------------------------------------------
//  VOID DisplayConfigErrors
//
//  Description:
//     Displays the message as required by the gwGlobalStatus,
//     clears the status and returns.
//
//  Parameters:
//     None.
//
//  Return (VOID):
//     None.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL DisplayConfigErrors()
{
   int  nID ;

   if (gwGlobalStatus)
   {
      switch (gwGlobalStatus)
      {
         case STATUS_CONFIG_BOARD_ERROR:
            nID = SR_ALERT_BOARD_ERROR ;
            break ;

         case STATUS_CONFIG_NO_VXD:
            nID = SR_ALERT_NO_VXD ;
            break ;

         case STATUS_CONFIG_WRONG_VXDVER:
            nID = SR_ALERT_WRONG_VXD ;
            break ;

         default:
            nID = 0 ;
      }
      if (nID)
         AlertBox( NULL, nID ) ;
      gwGlobalStatus = 0 ;
   }

} // DisplayConfigErrors()

//------------------------------------------------------------------------
//  LRESULT DrvSaveVolumeToReg
//
//  Description:
//     Saves the volume information and other information to the
//     SOUND.INI file.
//
//  Parameters:
//     PHARDWAREINSTANCE  phwi
//        pointer to hardware instance structure
//
//  Return Value (LRESULT):
//
//
//------------------------------------------------------------------------

LRESULT FAR PASCAL DrvSaveVolumeToReg
(
    PHARDWAREINSTANCE   phwi
)
{
   char             szDevNodeCfg[ 256 ] ;
   HKEY             hkSW ;
   LPSTR            lpsz ;
   PMIXERINSTANCE   pmi ;

   DPF( 1, "DrvSaveVolumeToReg" ) ;

   if (CM_Get_DevNode_Key( phwi -> dn, NULL, szDevNodeCfg, 
                           sizeof( szDevNodeCfg ), 
                           CM_REGISTRY_SOFTWARE ))
      return MMSYSERR_KEYNOTFOUND ;

   strcat( szDevNodeCfg, gszConfig ) ;

   if (RegOpenKey( HKEY_LOCAL_MACHINE, szDevNodeCfg, &hkSW ))
   {
      if (RegCreateKey( HKEY_LOCAL_MACHINE, szDevNodeCfg, &hkSW ))
         return MMSYSERR_BADDB ;
   }

   pmi = phwi -> pmi ;

   //---------------------------------
   //
   // Save mute
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyMute, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ MUTE_OUTLINE ][ 0 ]),
                  sizeof( DWORD ) ) ;

   //---------------------------------
   //
   // Master attenuation
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyLeftMaster, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTLINE ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightMaster, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTLINE ][ 1 ]),
                  sizeof( DWORD ) ) ;

   //---------------------------------
   //
   // Line-In attenuation
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyLeftLineIn, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTAUX1 ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightLineIn, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTAUX1 ][ 1 ]),
                  sizeof( DWORD ) ) ;

   //---------------------------------
   //
   // DAC attenuation
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyLeftDAC, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTDAC ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightDAC, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTDAC ][ 1 ]),
                  sizeof( DWORD ) ) ;

#ifdef AZTECH
#ifdef WASHTON

   //---------------------------------
   //
   // CD attenuation
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyLeftCD, 0, REG_BINARY,
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTCD ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightCD, 0, REG_BINARY,
                  (LPBYTE) &(pmi -> dwValue[ VOL_OUTCD ][ 1 ]),
                  sizeof( DWORD ) ) ;

#endif

   //---------------------------------
   //
   // Mono MIC output attenuation
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyMonoMic, 0, REG_BINARY,
                  (LPBYTE) &(pmi -> dwValue[  VOL_OUTMIC ][ 0 ]),
                  sizeof( DWORD ) ) ;
#endif

   //---------------------------------
   //
   // WAVE ADC attenuation & source
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyLeftWaveADCMic, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INMIC ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightWaveADCMic, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INMIC ][ 1 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyLeftWaveADCLine, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INAUX1 ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightWaveADCLine, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INAUX1 ][ 1 ]),
                  sizeof( DWORD ) ) ;

#ifdef AZTECH
#ifdef WASHTON

   RegSetValueEx( hkSW, gszRegKeyLeftWaveADCCD, 0, REG_BINARY,
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INCD ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightWaveADCCD, 0, REG_BINARY,
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INCD ][ 1 ]),
                  sizeof( DWORD ) ) ;

#endif

   RegSetValueEx( hkSW, gszRegKeyLeftWaveADCMix, 0, REG_BINARY,
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INMIX ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightWaveADCMix, 0, REG_BINARY,
                  (LPBYTE) &(pmi -> dwValue[ VOL_W_INMIX ][ 1 ]),
                  sizeof( DWORD ) ) ;
#endif

   // Save the WAVE-IN ADC source.

   switch (pmi -> dwValue[ MUX_WAVEIN ][ 0 ])
   {
      case MUXINPUT_MIC:
         lpsz = gszRegValInputMic ;
         break ;

      case MUXINPUT_AUX1:
         lpsz = gszRegValInputLineIn ;
         break ;
#ifdef AZTECH
#ifdef WASHTON

      case MUXINPUT_CD:
         lpsz = gszRegValInputCD ;
         break ;

#endif

      case MUXINPUT_MIX:
         lpsz = gszRegValInputOutput ;
         break ;
#endif
   }

   RegSetValueEx( hkSW, gszRegKeyWaveADCSource, 0, REG_SZ, 
                  (LPBYTE) lpsz, 0 ) ;

   //---------------------------------
   //
   // VOICE ADC attenuation & source
   //
   //---------------------------------

   RegSetValueEx( hkSW, gszRegKeyLeftVoiceADCMic, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_V_INMIC ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightVoiceADCMic, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_V_INMIC ][ 1 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyLeftVoiceADCLine, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_V_INAUX1 ][ 0 ]),
                  sizeof( DWORD ) ) ;

   RegSetValueEx( hkSW, gszRegKeyRightVoiceADCLine, 0, REG_BINARY, 
                  (LPBYTE) &(pmi -> dwValue[ VOL_V_INAUX1 ][ 1 ]),
                  sizeof( DWORD ) ) ;

   // Save the VOICEIN ADC source.

   switch (pmi -> dwValue[ MUX_VOICEIN ][ 0 ])
   {
      case MUXINPUT_MIC:
         lpsz = gszRegValInputMic ;
         break ;

      case MUXINPUT_AUX1:
         lpsz = gszRegValInputLineIn ;
         break ;
   }

   RegSetValueEx( hkSW, gszRegKeyVoiceADCSource, 0, REG_SZ, 
                  (LPBYTE) lpsz, 0 ) ;

   if (phwi -> wHardwareOptions & DAK_FMSYNTH)
   {
      //---------------------------------
      //
      // FM attenuation
      //
      //---------------------------------

      RegSetValueEx( hkSW, gszRegKeyLeftSynth, 0, REG_BINARY, 
                    (LPBYTE) &(pmi -> dwValue[ VOL_OUTMIDI ][ 0 ]),
                    sizeof( DWORD ) ) ;

      RegSetValueEx( hkSW, gszRegKeyRightSynth, 0, REG_BINARY, 
                    (LPBYTE) &(pmi -> dwValue[ VOL_OUTMIDI ][ 1 ]),
                    sizeof( DWORD ) ) ;
   }

   RegCloseKey( hkSW ) ;

   return MMSYSERR_NOERROR ;

} // DrvSaveVolumeToReg()

//------------------------------------------------------------------------
//  LRESULT DrvLoadVolumeFromReg
//
//  Description:
//     Load the volume information from the registry.
//
//  Parameters:
//     PHARDWAREINSTANCE  phwi
//        pointer to hardware instance structure
//
//  Return Value (LRESULT):
//
//
//------------------------------------------------------------------------

LRESULT FAR PASCAL DrvLoadVolumeFromReg
(
   PHARDWAREINSTANCE  phwi
)
{
   char                          szTemp[ 32 ], szDevNodeCfg[ 256 ] ;
   HKEY                          hkSW ;
   MIXERCONTROLDETAILS           mcd ;
   MIXERCONTROLDETAILS_BOOLEAN   mcd_f[ 2 ] ;  // !!! max multiple items is 2
   MIXERCONTROLDETAILS_UNSIGNED  mcd_u[ 2 ] ;
   PMIXERINSTANCE                pmi ;
   ULONG                         cbLen ;
   WORD                          wCount ;

   DPF( 1, "DrvLoadVolumeFromReg" ) ;

   if (CM_Get_DevNode_Key( phwi -> dn, NULL, szDevNodeCfg, 
                           sizeof( szDevNodeCfg ),
                           CM_REGISTRY_SOFTWARE ))
      return MMSYSERR_KEYNOTFOUND ;

   strcat( szDevNodeCfg, gszConfig ) ;

   if (RegOpenKey( HKEY_LOCAL_MACHINE, szDevNodeCfg, &hkSW ))
      hkSW = NULL ;

   pmi = phwi -> pmi ;

   //------------------------------------
   //
   // Get mute setting
   //
   //------------------------------------

   // default value is FALSE

   mcd_f[ 0 ].fValue = FALSE ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_BOOLEAN ) ;
      RegQueryValueEx( hkSW, gszRegKeyMute, NULL, NULL, 
                       (LPSTR) &mcd_f[ 0 ].fValue, &cbLen ) ;
   }

   //
   // Set the mute as appropriate.
   //

   mcd.dwControlID = MUTE_OUTLINE ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ MUTE_OUTLINE ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_BOOLEAN ) ;
   mcd.paDetails = mcd_f ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

#if 0
   AlertBox( NULL, SR_ALERT_MUTED ) ;
#endif

   //------------------------------------
   //
   // WAVE-IN mux setting & attenuations
   //
   //------------------------------------

   // default value is "Line-In"

   strcpy( szTemp, gszIniDefWaveADCSource ) ;

   if (hkSW)
   {
      cbLen = sizeof( szTemp ) ;
      RegQueryValueEx( hkSW, gszRegKeyWaveADCSource, NULL, NULL, 
                       (LPSTR) szTemp, &cbLen ) ;
   }

   mcd.dwControlID = MUX_WAVEIN ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;

   mcd.cChannels = 1 ; // UNIFORM mux
   mcd.cMultipleItems = pmi -> mxc[ mcd.dwControlID ].cMultipleItems ;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_BOOLEAN ) ;
   mcd.paDetails = mcd_f ;

   for (wCount = 0; wCount < (WORD) mcd.cMultipleItems; wCount++)
      mcd_f[ wCount ].fValue = FALSE ;

   if (!lstrcmpi( szTemp, gszRegValInputLineIn ))
      wCount = MUXINPUT_AUX1 ;
   else if (!lstrcmpi( szTemp, gszRegValInputMic ))
      wCount = MUXINPUT_MIC ;
#ifdef AZTECH
#ifdef WASHTON

   else if (!lstrcmpi( szTemp, gszRegValInputCD ))
      wCount = MUXINPUT_CD ;

#endif

   else if (!lstrcmpi( szTemp, gszRegValInputOutput ))
      wCount = MUXINPUT_MIX ;
#endif

   mcd_f[ wCount ].fValue = TRUE ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

   //
   // Attenuation levels for WAVE-IN (Line)
   //

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTADC ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTADC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftWaveADCLine, NULL, NULL, 
                       (LPSTR) &mcd_u[ 0 ].dwValue, 
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightWaveADCLine, NULL, NULL, 
                       (LPSTR) &mcd_u[ 1 ].dwValue, 
                       &cbLen ) ;
   }


   mcd.dwControlID = VOL_W_INAUX1 ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_W_INAUX1 ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

   //
   // Attenuation levels for WAVE-IN (mic)
   //

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTADC ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTADC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftWaveADCMic, NULL, NULL, 
                       (LPSTR) &mcd_u[ 0 ].dwValue, 
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightWaveADCMic, NULL, NULL, 
                       (LPSTR) &mcd_u[ 1 ].dwValue, 
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_W_INMIC ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_W_INMIC ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

#ifdef AZTECH
#ifdef WASHTON

   //
   // Attenuation levels for WAVE-IN (cd)
   //

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTADC ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTADC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftWaveADCCD, NULL, NULL,
                       (LPSTR) &mcd_u[ 0 ].dwValue,
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightWaveADCCD, NULL, NULL,
                       (LPSTR) &mcd_u[ 1 ].dwValue,
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_W_INCD ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_W_INCD ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

#endif

   //
   // Attenuation levels for WAVE-IN (mixer)
   //

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTADC ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTADC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftWaveADCMix, NULL, NULL,
                       (LPSTR) &mcd_u[ 0 ].dwValue,
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightWaveADCMix, NULL, NULL,
                       (LPSTR) &mcd_u[ 1 ].dwValue,
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_W_INMIX ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_W_INMIX ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;
#endif
   //------------------------------------
   //
   // VOICE-IN mux setting & attenuations
   //
   //------------------------------------

   // default value is "Line-In"

   strcpy( szTemp, gszIniDefVoiceADCSource ) ;

   if (hkSW)
   {
      cbLen = sizeof( szTemp ) ;
      RegQueryValueEx( hkSW, gszRegKeyVoiceADCSource, NULL, NULL, 
                       (LPSTR) szTemp, &cbLen ) ;
   }

   mcd.dwControlID = MUX_VOICEIN ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;

   mcd.cChannels = 1 ; // UNIFORM mux
   mcd.cMultipleItems = pmi -> mxc[ mcd.dwControlID ].cMultipleItems ;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_BOOLEAN ) ;
   mcd.paDetails = mcd_f ;

   for (wCount = 0; wCount < (WORD) mcd.cMultipleItems; wCount++)
      mcd_f[ wCount ].fValue = FALSE ;

   if (!lstrcmpi( szTemp, gszRegValInputLineIn ))
      wCount = MUXINPUT_AUX1 ;
   else if (!lstrcmpi( szTemp, gszRegValInputMic ))
      wCount = MUXINPUT_MIC ;

   mcd_f[ wCount ].fValue = TRUE ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

   //
   // Attenuation levels for VOICE-IN (Line)
   //

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTADC ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTADC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftVoiceADCLine, NULL, NULL, 
                       (LPSTR) &mcd_u[ 0 ].dwValue, 
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightVoiceADCLine, NULL, NULL, 
                       (LPSTR) &mcd_u[ 1 ].dwValue, 
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_V_INAUX1 ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_V_INAUX1 ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

   //
   // Attenuation levels for VOICE-IN (mic)
   //

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTADC ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTADC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftVoiceADCMic, NULL, NULL, 
                       (LPSTR) &mcd_u[ 0 ].dwValue, 
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightVoiceADCMic, NULL, NULL, 
                       (LPSTR) &mcd_u[ 1 ].dwValue, 
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_V_INMIC ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_V_INMIC ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

#ifdef AZTECH
   //--------------------------
   //
   // Mono MIC output attenuation
   //
   //--------------------------

   mcd_u[ 0 ].dwValue = INI_DEF_MONOMIC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyMonoMic, NULL, NULL,
                       (LPSTR) &mcd_u[ 0 ].dwValue,
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_OUTMIC ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTMIC ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

#ifdef WASHTON

   //--------------------------
   //
   // CD attenuation
   //
   //--------------------------

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTCD ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTCD ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftCD, NULL, NULL,
                       (LPSTR) &mcd_u[ 0 ].dwValue,
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightCD, NULL, NULL,
                       (LPSTR) &mcd_u[ 1 ].dwValue,
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_OUTCD ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTCD ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

#endif
#endif
   //--------------------------
   //
   // DAC attenuation
   //
   //--------------------------

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTDAC ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTDAC ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftDAC, NULL, NULL, 
                       (LPSTR) &mcd_u[ 0 ].dwValue, 
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightDAC, NULL, NULL, 
                       (LPSTR) &mcd_u[ 1 ].dwValue, 
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_OUTDAC ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTDAC ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

   //--------------------------
   //
   // FM attenuation
   //
   //--------------------------

   if (phwi -> wHardwareOptions & DAK_FMSYNTH)
   {
      mcd_u[ 0 ].dwValue = INI_DEF_LEFTSYNTH ;
      mcd_u[ 1 ].dwValue = INI_DEF_RIGHTSYNTH ;

      if (hkSW)
      {
         cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
         RegQueryValueEx( hkSW, gszRegKeyLeftSynth, NULL, NULL, 
                          (LPSTR) &mcd_u[ 0 ].dwValue, 
                          &cbLen ) ;

         cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
         RegQueryValueEx( hkSW, gszRegKeyRightSynth, NULL, NULL, 
                          (LPSTR) &mcd_u[ 1 ].dwValue, 
                          &cbLen ) ;
      }

      mcd.dwControlID = VOL_OUTMIDI ;
      mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
      mcd.cChannels = pmi -> auControlMap[ VOL_OUTMIDI ][ CM_CHANNELS ] ;
      mcd.cMultipleItems = 0;
      mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      mcd.paDetails = mcd_u ;

      MxdSetControlDetails( phwi, &mcd, 0 ) ;
   }

   //--------------------------
   //
   // Line-In attenuation
   //
   //--------------------------

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTLINEIN ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTLINEIN ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftLineIn, NULL, NULL, 
                       (LPSTR) &mcd_u[ 0 ].dwValue, 
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightLineIn, NULL, NULL, 
                       (LPSTR) &mcd_u[ 1 ].dwValue, 
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_OUTAUX1 ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTAUX1 ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

   //--------------------------
   //
   // Master attenuation
   //
   //--------------------------

   mcd_u[ 0 ].dwValue = INI_DEF_LEFTMASTER ;
   mcd_u[ 1 ].dwValue = INI_DEF_RIGHTMASTER ;

   if (hkSW)
   {
      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyLeftMaster, NULL, NULL, 
                       (LPSTR) &mcd_u[ 0 ].dwValue, 
                       &cbLen ) ;

      cbLen = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
      RegQueryValueEx( hkSW, gszRegKeyRightMaster, NULL, NULL, 
                       (LPSTR) &mcd_u[ 1 ].dwValue, 
                       &cbLen ) ;
   }

   mcd.dwControlID = VOL_OUTLINE ;
   mcd.cbStruct = sizeof( MIXERCONTROLDETAILS ) ;
   mcd.cChannels = pmi -> auControlMap[ VOL_OUTLINE ][ CM_CHANNELS ] ;
   mcd.cMultipleItems = 0;
   mcd.cbDetails  = sizeof( MIXERCONTROLDETAILS_UNSIGNED ) ;
   mcd.paDetails = mcd_u ;

   MxdSetControlDetails( phwi, &mcd, 0 ) ;

   if (hkSW)
      RegCloseKey( hkSW ) ;

   return MMSYSERR_NOERROR ;

} // end of DrvLoadVolumeFromReg()

//------------------------------------------------------------------------
//  VOID NEAR PASCAL CriticalSection( WORD wStatus )
//
//  Description:
//     Enters or leaves a critical section (disables/enables interrupts).
//
//  Parameters:
//     WORD wStatus
//       CS_BEGIN:  Enter critical section, claiming CLI if necessary.
//       CS_END:    Leave critical section, outgoing STI if necessary.
//
//  Return Value:
//     NONE.
//
//
//------------------------------------------------------------------------

VOID NEAR PASCAL CriticalSection
(
    WORD            wStatus
)
{
   static WORD wEnterCount = 0 ;
   static WORD wPreviousState = 0 ;

   switch (wStatus)
   {
      case CS_BEGIN:
         if (0 == wEnterCount)
         {
            _asm
            {
               pushf
               pop   ax
               mov   wPreviousState, ax
               cli
            }
         }
         wEnterCount++ ;
         break ;

      case CS_END:
         if (0 != wEnterCount)
         {
            wEnterCount-- ;
            if (0 == wEnterCount)
            {
               _asm
               {
                  mov   ax, wPreviousState
                  test  ah, 0x02
                  jz    SHORT No_Enable
                  sti
               No_Enable:
               }
            }
         }
         break ;
   }

} // end of CriticalSection()

//--------------------------------------------------------------------------
//  
//  VOID AddInstance
//  
//  Description:
//      Adds the instance to the device instance list.
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         ptr -> hardware instance structure
//  
//  Return (VOID):
//      Nothing.
//  
//--------------------------------------------------------------------------

VOID AddInstance
(
    PHARDWAREINSTANCE   phwi
)
{
   phwi -> pNext = pHardwareList ;
   pHardwareList = phwi ;

} // AddInstance()

//--------------------------------------------------------------------------
//  
//  VOID RemoveInstance
//  
//  Description:
//      Removes the instance from device's list
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//         ptr -> hardware instance structure
//
//  Return (VOID):
//      Nothing.
//
//--------------------------------------------------------------------------

VOID RemoveInstance
(
    PHARDWAREINSTANCE   phwi
)
{
   PHARDWAREINSTANCE *ppCur ;

   for (ppCur = &pHardwareList ;
        *ppCur != NULL;
        ppCur = &(*ppCur)->pNext)
   {
      if (*ppCur == phwi)
      {
         *ppCur = (*ppCur)-> pNext ;
         break ;
      }
   }

} // RemoveInstance()

//--------------------------------------------------------------------------
//  
//  HPIPE pipeOpen
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//      LPSTR psz
//  
//      PPIPEOPENSTRUCT pos
//  
//  Return (HPIPE):
//  
//--------------------------------------------------------------------------

HPIPE pipeOpen
(
    PHARDWAREINSTANCE   phwi,
    LPSTR               psz,
    PPIPEOPENSTRUCT     pos
)
{
   FNPIPEOPEN  fnpipeOpen ;

   if (NULL == (fnpipeOpen = glpVSNDSYSEntry))
      return NULL ;

   DPF( 1, "open pipe" ) ;

   _asm mov dx, PIPE_API_Open
   return fnpipeOpen( phwi -> dn, psz, pos ) ;

} // pipeOpen()

//--------------------------------------------------------------------------
//  
//  VOID pipeClose
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//      HPIPE hp
//  
//  Return (HPIPE):
//  
//--------------------------------------------------------------------------

VOID pipeClose
(
    PHARDWAREINSTANCE   phwi,
    HPIPE               hp
)
{
   FNPIPECLOSE  fnpipeClose ;

   if (NULL == (fnpipeClose = glpVSNDSYSEntry))
      return ;

   _asm mov dx, PIPE_API_Close
   fnpipeClose( phwi -> dn, hp ) ;

} // pipeClose()

//--------------------------------------------------------------------------
//  
//  LRESULT AddDevNode
//  
//  Description:
//      "Registers" a devnode with the driver.  If an instance of
//      this devnode already exists, the count is incremented.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL AddDevNode
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  phwi ;

   DPF( 1, "AddDevNode" ) ;

   if (NULL != (phwi = DevNodeToHardwareInstance( dn )))
   {
      phwi -> cReference++ ;
      return MMSYSERR_NOERROR ;
   }

   if (NULL == 
      (phwi = (PHARDWAREINSTANCE) LocalAlloc( LPTR, 
                                              sizeof( HARDWAREINSTANCE ) )))
      return MMSYSERR_NOMEM ;

   phwi -> dn = dn ;
   phwi -> cReference = 1 ;
   AddInstance( phwi ) ;

   return MMSYSERR_NOERROR ;

} // AddDevNode()

//--------------------------------------------------------------------------
//  
//  VOID HardwareInit
//  
//  Description:
//  
//  
//  Parameters:
//      PHARDWAREINSTANCE phwi
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID HardwareInit
(
    PHARDWAREINSTANCE   phwi
)
{
   DWORD  timeStart ;
   WORD   wCurPos, wSampleRate ;

   phwi -> fBadBoard = FALSE ;

   // Before we touch the hardware, set the timeout value...

   phwi -> dwWaitLoop = 0x10000 ;
   phwi -> wOldFormat = 0xffff ;

   // Acquire the hardware... initialization time!

   if (0 == HardwareAcquire( phwi, phwi -> wIOAddressCODEC, 
                             MSS_ASS_ACQUIRE_CODEC )  & 0x7FFF)
   {
      // Load mixer settings...
   
      if (DrvLoadVolumeFromReg( phwi ))
      {
         DPF( 1, "failed to load volume settings from registry!" ) ;
      }

      if (phwi -> wCODECClass == CODEC_J_CLASS)
         HwExtMute( phwi, TRUE ) ;

#ifdef AZTECH
       if (phwi -> wCODECClass == CODEC_KPLUS_CLASS)
          CODEC_RegWrite( phwi, REGISTER_CAP_DATAFORMAT, 0x4B ) ;
#endif

      //
      // Both parts (AD1848J and AD1848K) seem to exhibit problems
      // when restarting 16-bit DAC.  It appears that the DREQs
      // are not in the proper order on some VLSI machines, so
      // flush the noise.
      //

      wSampleRate = phwi -> wSamplesPerSec ;
      HwSetFormat( phwi, 22050, 1, FORMAT_16BIT, DIRECTION_DAC ) ; 
      phwi -> wSamplesPerSec = wSampleRate ;

      //
      // let the thing run a bit...
      //

      if (0 == (phwi -> wFlags & SSI_FLAG_BUSTYPE_PCMCIA))
      {
         phwi -> fJustStart = TRUE ;
         waveFillSilence( phwi -> dwWaveFormat, phwi -> lpDMABuffer,
                        2 * phwi -> wDMAHalfBufSize, 0, 0 ) ;
         HwBeginDACDMA( phwi ) ;
         timeStart = timeGetTime() ;
         do
         {
             HwGetCurPos( phwi, &wCurPos, GCP_OPTIONF_DAC ) ;
         }
         while ((wCurPos < 16) && (timeGetTime() - timeStart < 10)) ;
         HwStopDACDMA( phwi ) ;
         phwi -> fJustStart = FALSE ;
      }

      if (phwi -> wCODECClass == CODEC_J_CLASS)
         HwExtMute( phwi, FALSE ) ;

      HardwareRelease( phwi, phwi -> wIOAddressCODEC, MSS_ASS_ACQUIRE_CODEC ) ;
   }

} // HardwareInit()

//--------------------------------------------------------------------------
//  
//  LRESULT EnableDevNode
//  
//  Description:
//      Increments the enable count of the DevNode.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
//  
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL EnableDevNode
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  phwi ;
   PIPEOPENSTRUCT     pos ;
   DRVDATA            dd ;

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (phwi -> cEnable++)
      return MMSYSERR_NOERROR ;

   if (!GetVxDInfo( phwi, dn ))
   {
      phwi -> cEnable-- ;
      return MMSYSERR_BADDEVICEID ;
   }

   //
   // Note that dd.lpdi is not valid for this initialization phase
   // and is only used by the property sheet.
   //

   dd.lpdi = NULL ;
   DrvLoadUserConfig( dn, &dd ) ;

   phwi -> fAcceptCloseRates = dd.fAcceptCloseRates ;
   phwi -> fSingleModeDMA = dd.fSingleModeDMA ;
   phwi -> fDo11kHzMCE = dd.fDo11kHzMCE ;

   // Initialize DMA buffer variables...

   phwi -> lpDMABuffer = 
      (LPBYTE) (MAKELONG( 0, phwi -> wDMABufferSelector )) ;
   phwi -> wDMAPhysAddr = 
      LOWORD( phwi -> lpDMABufferPhys ) ;
   phwi -> bDMAPhysPage =
      LOBYTE( HIWORD( phwi -> lpDMABufferPhys ) ) ;
   phwi -> wDMABufferLenMinusOne =
      (WORD) (phwi -> dwDMABufferLen - 1) ;

   // Command structure needed to EOI the PIC

   phwi -> wEOICommands = (phwi -> bIRQ > 7) ?
     (0x6200 | (0x60 | (phwi -> bIRQ & 0x07)) ) : 
     ((WORD) (phwi -> bIRQ | 0x60) << 8) ;

   // Set up playback DMA

   phwi -> portPlaybackDMAPReg = 
      gabDMAPageReg[ phwi -> bPlaybackDMA ] ;
   phwi -> portPlaybackDMAEnable = 
      phwi -> bPlaybackDMA & (BYTE)0x03 ;
   phwi -> portPlaybackDMADisable = 
      (BYTE)0x04 + (phwi -> bPlaybackDMA & (BYTE)0x03) ;

   //
   // Use single mode DMA for Symphony chip sets.
   //

   phwi -> portPlaybackDMAWrite =
      ((phwi -> fSingleModeDMA) ? (BYTE) 0x58 : (BYTE) 0x18) +
       (phwi -> bPlaybackDMA & (BYTE) 0x03) ;

   // 8-bit DMA

   phwi -> portPlaybackDMAAdr = 
      (BYTE)(DMA8ADR + (phwi -> bPlaybackDMA & 0x03) * 2) ;
   phwi -> portPlaybackDMACnt = 
      (BYTE)(DMA8CNT + (phwi -> bPlaybackDMA & 0x03) * 2) ;
   phwi -> portPlaybackDMASMR = 
      DMA8SMR ;
   phwi -> portPlaybackDMAMod = 
      DMA8MOD ;
   phwi -> portPlaybackDMAClr = 
      DMA8CLR ;

   // Record DMA

   phwi -> portCaptureDMAPReg = 
      gabDMAPageReg[ phwi -> bCaptureDMA ] ;
   phwi -> portCaptureDMAEnable = 
      (BYTE)0x00 + (phwi -> bCaptureDMA & (BYTE)0x03) ;
   phwi -> portCaptureDMADisable = 
      (BYTE)0x04 + (phwi -> bCaptureDMA & (BYTE)0x03) ;

   //
   // Use single mode DMA for Symphony chip sets.
   //

   phwi -> portCaptureDMARead =
      ((phwi -> fSingleModeDMA) ? (BYTE) 0x54 : (BYTE) 0x14) +
        (phwi -> bCaptureDMA & (BYTE) 0x03) ;

   phwi -> portCaptureDMAAdr = 
      (BYTE)(DMA8ADR + phwi -> bCaptureDMA * 2) ;
   phwi -> portCaptureDMACnt = 
      (BYTE)(DMA8CNT + phwi -> bCaptureDMA * 2) ;
   phwi -> portCaptureDMASMR = 
      DMA8SMR ;
   phwi -> portCaptureDMAMod = 
      DMA8MOD ;
   phwi -> portCaptureDMAClr = 
      DMA8CLR ;


   if (!Create_ISR( phwi ))
   {
      DPF( 1, "Failed to create ISR!!!\r\n" ) ;

      phwi -> cEnable-- ;
      return MMSYSERR_NOTENABLED ;
   }


   // Hook the interrupt vector.

   DPF( 1, "hooking interrupt" ) ;

   phwi -> bOrigIntMask = 
      (BYTE) SetInterruptMask( phwi -> bIRQ, 1 ) ;

   phwi -> dwOldISR = 
      SetInterruptVector( phwi -> bIRQ, 
                          (LPVOID) MAKELONG( 0, phwi -> uISRCodeSel ) ) ;

   // Acknowledge interrupt just in case.

   if (0 == HardwareAcquire( phwi, phwi -> wIOAddressCODEC, 
                             MSS_ASS_ACQUIRE_CODEC )  & 0x7FFF)
   {
      WRITEPORT( CODEC_STATUS, 0x00 ) ;
      HardwareRelease( phwi, phwi -> wIOAddressCODEC, MSS_ASS_ACQUIRE_CODEC ) ;
   }

   SetInterruptMask( phwi -> bIRQ, 0 ) ;

   phwi -> fEnabled = 1 ;

   //
   // Do mixer init stuff...
   //

   MxdInit( phwi ) ;

   //
   // open the mixer pipe
   //

   pos.cbSize = sizeof( pos ) ;
   pos.pClientProc = mixerPipeProc ;
   if (NULL == (phwi -> hpmxd = pipeOpen( phwi, "MSOPLMXD", &pos )))
   {
      DPF( 1, "pipeOpen() failed!" ) ;
   }

   // Initialize the hardware...

   HardwareInit( phwi ) ;

   // Is the board functioning properly?

   if (phwi -> fBadBoard)
   {
      waveBadBoardError() ;
      phwi -> fEnabled = 0 ;
      return MMSYSERR_NOTENABLED ;
   }

   return MMSYSERR_NOERROR ;

} // EnableDevNode()

//--------------------------------------------------------------------------
//  
//  VOID PowerNotify_SuspendDevices
//  
//  Description:
//  
//  
//  Parameters:
//      None.
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID FAR PASCAL PowerNotify_SuspendDevices()
{
   PHARDWAREINSTANCE  phwi ;

   for (phwi = pHardwareList; phwi != NULL; phwi = phwi -> pNext)
   {
      if (phwi -> fEnabled)
      {
         DrvSaveVolumeToReg( phwi ) ;
         wodSuspend( phwi ) ;
         widSuspend( phwi ) ;
      }
   }

} // PowerNotify_SuspendDevices()

//--------------------------------------------------------------------------
//  
//  VOID PowerNotify_ResumeDevices
//  
//  Description:
//  
//  
//  Parameters:
//      None.
//  
//  Return (VOID):
//  
//  
//--------------------------------------------------------------------------

VOID FAR PASCAL PowerNotify_ResumeDevices()
{
   PHARDWAREINSTANCE phwi ;

   for (phwi = pHardwareList; phwi != NULL; phwi = phwi -> pNext)
   {
      if (phwi -> fEnabled)
      {
         HardwareInit( phwi ) ;
         wodReactivate( phwi ) ;
         widReactivate( phwi ) ;
      }
   }

} // PowerNotify_ResumeDevices()

//--------------------------------------------------------------------------
//  
//  LRESULT DisableDevNode
//  
//  Description:
//      Decrements the enabled count for the hardware instance.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL DisableDevNode
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  phwi ;

   DPF( 1, "DisableDevNode" ) ;

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (--phwi -> cEnable)
      return MMSYSERR_NOERROR ;

   // Disable the interrupt

   SetInterruptMask( phwi -> bIRQ, 1 ) ;
   SetInterruptVector( phwi -> bIRQ, (LPVOID) phwi -> dwOldISR ) ;
   SetInterruptMask( phwi -> bIRQ, phwi -> bOrigIntMask ) ;

   //
   // Free the ISR stub...
   //

   GlobalFree( phwi -> uISRDataSel ) ;
   FreeSelector( phwi -> uISRCodeSel ) ;
   phwi -> uISRDataSel = NULL ;
   phwi -> uISRCodeSel = NULL ;

   // Save mixer settings...
   
   if (DrvSaveVolumeToReg( phwi ))
   {
      DPF( 1, "failed to save volume settings to registry!" ) ;
   }

   // Close mixer pipe

   if (phwi -> hpmxd)
   {
      pipeClose( phwi, phwi -> hpmxd ) ;
      phwi -> hpmxd = NULL ;
   }

   //
   //  at this point, it is safe to shut things down hard
   //

   if (0 == HardwareAcquire( phwi, phwi -> wIOAddressCODEC, 
                             MSS_ASS_ACQUIRE_CODEC ) & 0x7FFF)
   {
      // Tell the CODEC to shut down.

      HwEndDSPAndDMA( phwi ) ;

      HardwareRelease( phwi, phwi -> wIOAddressCODEC, MSS_ASS_ACQUIRE_CODEC );
   }
   phwi -> fEnabled = 0 ;

   return MMSYSERR_NOERROR ;

} // DisableDevNode()

//--------------------------------------------------------------------------
//  
//  LRESULT RemoveDevNode
//  
//  Description:
//      Removes a reference to the given devnode, when reference count
//      of zero is reached the hardware instance structure is removed.
//  
//  Parameters:
//      DWORD dn
//         devnode from MMSYSTEM
//  
//  Return (LRESULT):
//      MMSYSERR_NOERROR if everything OK otherwise an appropriate
//      error code is returned.
//  
//--------------------------------------------------------------------------

LRESULT FAR PASCAL RemoveDevNode
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  phwi ;

   DPF( 1, "RemoveDevNode" ) ;

   if (NULL == (phwi = DevNodeToHardwareInstance( dn )))
      return MMSYSERR_INVALHANDLE ;

   if (--phwi -> cReference)
      return MMSYSERR_NOERROR ;

   if (phwi -> cEnable)
   {
      DPF( 1, "Trying to remove devnode but ref != enable" ) ;
      return MMSYSERR_ALLOCATED ;
   }

   // Clean up the mixer structures...

   MxdEnd( phwi ) ;

   RemoveInstance( phwi ) ;

   LocalFree( (LOCALHANDLE) phwi ) ;

   return MMSYSERR_NOERROR ;

} // RemoveDevNode()

//--------------------------------------------------------------------------
//  
//  PHARDWAREINSTANCE DevNodeToHardwareInstance
//  
//  Description:
//  
//  
//  Parameters:
//      DWORD dn
//  
//  Return (PHARDWAREINSTANCE):
//  
//--------------------------------------------------------------------------

PHARDWAREINSTANCE FAR PASCAL DevNodeToHardwareInstance
(
    DWORD           dn
)
{
   PHARDWAREINSTANCE  pCur ;

   if (!pHardwareList)
      return NULL ;

   for (pCur = pHardwareList; pCur != NULL; pCur = pCur -> pNext)
   {
      if (pCur -> dn == dn)
          return pCur ;
   }

   return NULL ;

} // DevNodeToHardwareInstance()

//------------------------------------------------------------------------
//  LRESULT FAR PASCAL DrvInit( VOID )
//
//  Description:
//     Verifies that the VxD loaded successfully.
//
//  Parameters:
//     Nothing.
//
//  Return Value (LRESULT):
//     MMSYSERR_NOERROR if no problem.
//
//------------------------------------------------------------------------

LRESULT FAR PASCAL DrvInit()
{
   WORD   wVxDVersion ;

   DPF( 1, "DrvInit" ) ;

   // NOTE: GetVxDVersion() returns 0 if the VxD is not found.

   if (wVxDVersion = GetVxDVersion())
   {
      if (wVxDVersion < MSSNDSYS_VERSION_RQD)
      {
         // The 4.0 VxD must be present for this driver
         // to operate correctly!!!

         gwGlobalStatus = STATUS_CONFIG_WRONG_VXDVER ;
         return MMSYSERR_ERROR ;
      }
   }
   else
   {
      gwGlobalStatus = STATUS_CONFIG_NO_VXD ;
      return MMSYSERR_ERROR ;
   }

   DPF( 1, "DrvInit() -- finished" ) ;
   return 0 ;

} // end of DrvInit()

//------------------------------------------------------------------------
//  VOID DrvEnd
//
//  Description:
//     Closes up the hardware and interrupts.
//
//  Parameters:
//     None.
//
//  Return Value:
//     None.
//
//
//------------------------------------------------------------------------

VOID FAR PASCAL DrvEnd()
{
   DPF( 1, "DrvEnd" ) ;

} // end of DrvEnd()

//--------------------------------------------------------------------------
//  
//  int LibMain
//  
//  Description:
//      Library initialization code
//  
//  Parameters:
//      HMODULE hModule
//         Module handle
//  
//      UINT uDataSeg
//         selector of data segment
//
//      UINT uHeapSize
//         Heap size as specified in .DEF
//  
//      LPSTR lpCmdLine
//         command line passed from kernel
//  
//  Return (int):
//      1 if successful
//  
//  
//--------------------------------------------------------------------------

int FAR PASCAL LibMain
(
    HMODULE         hModule,
    UINT            uDataSeg,
    UINT            uHeapSize,
    LPSTR           lpCmdLine
)
{
#ifdef DEBUG
    // get debug level - default is 3

    wDebugLevel = GetProfileInt( STR_MMDEBUG, STR_DRIVER, 3 ) ;
#endif

    //
    //  save our module handle
    //

    ghModule = hModule;

    //
    //  succeed the load...
    //
    return (1) ;

} // LibMain()


//---------------------------------------------------------------------------
//  End of File: init.c
//---------------------------------------------------------------------------
