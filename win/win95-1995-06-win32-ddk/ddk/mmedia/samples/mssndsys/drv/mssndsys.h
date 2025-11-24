//---------------------------------------------------------------------------
//
//  Module:   mssndsys.h
//
//  Description: header file for MSSNDSYS.DRV
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//---------------------------------------------------------------------------


/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

#define INI_STR_LEFTMASTER        "LeftMasterVol"
#define INI_STR_RIGHTMASTER       "RightMasterVol"
#define INI_STR_LEFTLINEIN        "LeftLineInVol"
#define INI_STR_RIGHTLINEIN       "RightLineInVol"
#define INI_STR_LEFTDAC           "LeftDACVol"
#define INI_STR_RIGHTDAC          "RightDACVol"

#ifdef AZTECH
    #define INI_STR_LEFTCD            "LeftCDVol"
    #define INI_STR_RIGHTCD           "RightCDVol"
    #define INI_STR_MONOMIC           "MonoMICVol"
#endif

#define INI_STR_LEFTWAVEADCMIC    "LeftWaveADCMicVol"
#define INI_STR_RIGHTWAVEADCMIC   "RightWaveADCMicVol"
#define INI_STR_LEFTWAVEADCLINE   "LeftWaveADCLineVol"
#define INI_STR_RIGHTWAVEADCLINE  "RightWaveADCLineVol"
#ifdef AZTECH
    #define INI_STR_LEFTWAVEADCCD     "LeftWaveADCCdVol"
    #define INI_STR_RIGHTWAVEADCCD    "RightWaveADCCdVol"
    #define INI_STR_LEFTWAVEADCMIX    "LeftWaveADCMixerVol"
    #define INI_STR_RIGHTWAVEADCMIX   "RightWaveADCMixerVol"
#endif
#define INI_STR_WAVEADCSOURCE     "WaveADCSource"
#define INI_DEF_WAVEADCSOURCE     INI_INPUT_MIC

#define INI_STR_LEFTVOICEADCMIC   "LeftVoiceADCMicVol"
#define INI_STR_RIGHTVOICEADCMIC  "RightVoiceADCMicVol"
#define INI_STR_LEFTVOICEADCLINE  "LeftVoiceADCLineVol"
#define INI_STR_RIGHTVOICEADCLINE "RightVoiceADCLineVol"
#define INI_STR_VOICEADCSOURCE    "VoiceADCSource"
#define INI_DEF_VOICEADCSOURCE    INI_INPUT_MIC

#define INI_STR_LEFTSYNTH         "LeftSynthVol"
#define INI_STR_RIGHTSYNTH        "RightSynthVol"

#define INI_STR_MUTE              "Muted"
#define INI_STR_MUTEMSG           "StartupMuteMsg"

#define INI_YES                 "Yes"
#define INI_NO                  "No"

#define INI_TRUE                  "TRUE"
#define INI_FALSE                 "FALSE"
#define INI_ON                    "ON"
#define INI_OFF                   "OFF"

#define INI_DEF_LEFTMASTER      32767
#define INI_DEF_RIGHTMASTER     32767
#define INI_DEF_LEFTLINEIN      32767
#define INI_DEF_RIGHTLINEIN     32767
#define INI_DEF_LEFTDAC         32767
#define INI_DEF_RIGHTDAC        32767
#ifdef AZTECH
    #define INI_DEF_LEFTCD          32767
    #define INI_DEF_RIGHTCD         32767
    #define INI_DEF_MONOMIC         32767
#endif  
#define INI_DEF_LEFTADC         32767
#define INI_DEF_RIGHTADC        32767
#define INI_DEF_LEFTSYNTH       32767
#define INI_DEF_RIGHTSYNTH      32767

#define INI_DEF_MUTE            INI_NO
#define INI_DEF_MUTEMSG         INI_NO

//
//  Following taken directly from the vxd headers (except for
//   DO11KHZMCE).  These MUST MATCH!
//
#define REGSTR_SUBKEY_CONFIG           "Config"
#define REGSTR_VAL_SINGLEMODEDMA       "Single Mode DMA"
#define REGSTR_VAL_ACCEPTCLOSERATES    "Accept Close Sampling Rates"
#define REGSTR_VAL_SBEMULATION         "Sound Blaster Emulation"
#define REGSTR_VAL_DO11KHZMCE          "Reduce Noise For Low Rates"

#define DEFAULT_SINGLEMODEDMA           FALSE
#define DEFAULT_ACCEPTCLOSERATES        FALSE
#define DEFAULT_SBEMULATION             TRUE
#define DEFAULT_DO11KHZMCE              FALSE

//
// Manufacturer and product ID definitions
//

#ifdef MSSNDSYS
    #define MID_MICROSOFT      (MM_MICROSOFT)
    #define DRV_VERSION        (0x400)
    #define PID_WAVEIN         (MM_MSFT_WSS_WAVEIN)
    #define PID_WAVEOUT        (MM_MSFT_WSS_WAVEOUT)
    #define PID_SYNTH          (MM_MSFT_MSOPL_SYNTH)
    #define PID_MIXER          (MM_MSFT_WSS_MIXER)
    #define PID_AUX            (MM_MSFT_WSS_AUX)
#endif
#ifdef AZTECH
    #define MID_MICROSOFT      (MM_AZTECH)
    #define DRV_VERSION        (0x400)
#ifdef NOVA
    #define PID_WAVEIN         (MM_AZTECH_NOVA16_WAVEIN)
    #define PID_WAVEOUT        (MM_AZTECH_NOVA16_WAVEOUT)
    #define PID_MIXER          (MM_AZTECH_NOVA16_MIXER)
#endif
#ifdef WASHTON
    #define PID_WAVEIN         (MM_AZTECH_WASH16_WAVEIN)
    #define PID_WAVEOUT        (MM_AZTECH_WASH16_WAVEOUT)
    #define PID_MIXER          (MM_AZTECH_WASH16_MIXER)
#endif
    #define PID_SYNTH          (MM_MSFT_MSOPL_SYNTH)
    #define PID_AUX            (MM_AZTECH_AUX)
#endif

//
//  WSS specific MIXERLINE flags placed in MIXERLINE.dwUser field. these
//  flags are used by the WSS mixer application and Voice Pilot. these
//  flags are _ignored_ by any generic mixer application.
//
#define SNDSYS_MIXERLINE_LOWPRIORITY    (0x00000001L)

/***********************************************************************
Microsoft Windows Sound System specific messages */

#define MYBASE                  (0x4000)
#define WIDM_LOWPRIORITY                  (MYBASE+0x93)

/***********************************************************************
record from */

#define INI_INPUT_LINEIN        "LineIn"
#define INI_INPUT_AUX           "Synth"
#define INI_INPUT_MIC           "MIC"
#ifdef AZTECH
    #define INI_INPUT_CD            "CD"
#endif
#define INI_INPUT_OUTPUT        "Feedback"

//---------------------------------------------------------------------------
//  End of File: mssndsys.h
//---------------------------------------------------------------------------

