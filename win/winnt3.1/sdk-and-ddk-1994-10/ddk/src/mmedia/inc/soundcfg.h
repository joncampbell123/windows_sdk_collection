
/*++ BUILD Version: 0001    // Increment this if a change has global effects


Copyright (c) 1990  Microsoft Corporation

Module Name:

    soundcfg.h

Abstract:

    This include file defines common strings and values for sound driver
	configuration.

Author:

    Robin Speed (RobinSp) 17-Oct-92

Revision History:
--*/

#define SOUND_REG_PORT (L"Port")
#define SOUND_REG_DMACHANNEL (L"DmaChannel")
#define SOUND_REG_INTERRUPT (L"Interrupt")
#define SOUND_REG_INPUTSOURCE (L"Input Source")
#define SOUND_REG_DMABUFFERSIZE (L"Dma Buffer Size")
#define SOUND_REG_CONFIGERROR (L"Configuration Error")

//
// Errors
//

#define SOUND_CONFIG_ERROR   0x00000000
#define SOUND_CONFIG_NOCARD  0x00000001
#define SOUND_CONFIG_BADINT  0x00000002
#define SOUND_CONFIG_BADDMA  0x00000003
#define SOUND_CONFIG_BADCARD 0x00000004

#define PARMS_SUBKEY                  L"Parameters"
#define REG_VALUENAME_LEFTMASTER      L"LeftMasterVolumeAtten"
#define REG_VALUENAME_RIGHTMASTER     L"RightMasterVolumeAtten"
#define REG_VALUENAME_LEFTLINEIN      L"LeftLineInAtten"
#define REG_VALUENAME_RIGHTLINEIN     L"RightLineInAtten"
#define REG_VALUENAME_LEFTDAC         L"LeftDACAtten"
#define REG_VALUENAME_RIGHTDAC        L"RightDACAtten"
#define REG_VALUENAME_LEFTMICMIX      L"LeftMicMixAtten"
#define REG_VALUENAME_RIGHTMICMIX     L"RightMicMixAtten"
#define REG_VALUENAME_LEFTADC         L"LeftADCAtten"
#define REG_VALUENAME_RIGHTADC        L"RightADCAtten"
#define REG_VALUENAME_LEFTSYNTH       L"LeftSynthAtten"
#define REG_VALUENAME_RIGHTSYNTH      L"RightSynthAtten"


//
// Input source selection
//

#define INPUT_LINEIN            0
#define INPUT_AUX               1
#define INPUT_MIC               2
#define INPUT_OUTPUT            3

//
// Default volume settings on initial install
//

#define DEF_ADC_VOLUME    0x24000000
#define DEF_DAC_VOLUME    0x24000000
#define DEF_SYNTH_VOLUME  0x24000000
#define DEF_AUX_VOLUME    0x24000000
#define DEF_MICMIX_VOLUME 0x00000000

