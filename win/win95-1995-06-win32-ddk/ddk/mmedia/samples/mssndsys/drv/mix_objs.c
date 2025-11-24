//---------------------------------------------------------------------------
//
// MIX_OBJS.C
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
#include <limits.h>

#include "mssndsys.h"
#include "driver.h"
#include "mixer.h"

//
// These are the default definitions of the mixer "objects" for the
// AD1848J/K and CS4248 when using the WSS DAK (Hardware Developer's
// Assistance Kit) specifications.
//

// NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE!
//
// !!!!!!!!  DO NOT DEFINE THIS AS GLOBALS IN YOUR DATA SEGMENT.  !!!!!!!!
// !!!!!!!!          USE CODE SEGMENT BASED DATA INSTEAD          !!!!!!!!
//
// It is highly probable that your data segment is FIXED, this is
// unnecessary clutter and wasted valuable resources for simple data
// structures.  Define this "global" default information in your
// code segment (BCODE) and allocate the necessary memory (GMEM_MOVEABLE)
// to support your mixer device.
//
// !!!!!!!!  DO NOT DEFINE THIS AS GLOBALS IN YOUR DATA SEGMENT.  !!!!!!!!
// !!!!!!!!          USE CODE SEGMENT BASED DATA INSTEAD          !!!!!!!!
//
// NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE! NOTE!

//----
//
// control map (for more info see mixer.c)
//
// The first index is the control number.
// The first UINT is the dest #.
// The second UINT is the "relative" source #.
// The third is the # of channels for the control.
// When source # is -1, the control is "at the dest."
//
//----

#ifdef WASHTON

UINT BCODE gauControlMap[ MAXCONTROLS ][ 3 ] = 
{
   // Mux controls

   {DEST_WAVEIN,     UINT_MAX,  1}, //Control 0
   {DEST_VOICEIN,    UINT_MAX,  1}, //Control 1

   //Volume controls

   {DEST_LINEOUT,           0,  2}, //Control 2
   {DEST_LINEOUT,           1,  2}, //Control 3
   {DEST_LINEOUT,           2,  2}, //Control 4
   {DEST_LINEOUT,           3,  2}, //Control 5
   {DEST_LINEOUT,           4,  1}, //Control 6
   {DEST_LINEOUT,    UINT_MAX,  2}, //Control 7
   {DEST_WAVEIN,            0,  2}, //Control 8
   {DEST_WAVEIN,            1,  2}, //Control 9
   {DEST_WAVEIN,            2,  2}, //Control 10
   {DEST_WAVEIN,            3,  2}, //Control 11
   {DEST_VOICEIN,           0,  2}, //Control 12
   {DEST_VOICEIN,           1,  2}, //Control 13

   //Mute controls

   {DEST_LINEOUT,           0,  1}, //Control 14
   {DEST_LINEOUT,           1,  1}, //Control 15
   {DEST_LINEOUT,           2,  1}, //Control 16
   {DEST_LINEOUT,           3,  1}, //Control 17
   {DEST_LINEOUT,           4,  1}, //Control 18
   {DEST_LINEOUT,    UINT_MAX,  1}, //Control 20
                                    
   //Peak meter controls                    

   {DEST_WAVEIN,            0,  2}, //Control 21
   {DEST_WAVEIN,            1,  2}, //Control 22
   {DEST_WAVEIN,            2,  2}, //Control 23
   {DEST_WAVEIN,            3,  2}, //Control 24

   {DEST_VOICEIN,           0,  2}, //Control 25
   {DEST_VOICEIN,           1,  2}, //Control 26

   {DEST_LINEOUT,           1,  2}  //Control 27
} ;
#endif
#ifdef NOVA

UINT BCODE gauControlMap[ MAXCONTROLS ][ 3 ] = 
{
   // Mux controls

   {DEST_WAVEIN,     UINT_MAX,  1}, //Control 0
   {DEST_VOICEIN,    UINT_MAX,  1}, //Control 1

   //Volume controls

   {DEST_LINEOUT,           0,  2}, //Control 2
   {DEST_LINEOUT,           1,  2}, //Control 3
   {DEST_LINEOUT,           2,  2}, //Control 4
   {DEST_LINEOUT,           3,  1}, //Control 5
   {DEST_LINEOUT,    UINT_MAX,  2}, //Control 6
   {DEST_WAVEIN,            0,  2}, //Control 7
   {DEST_WAVEIN,            1,  2}, //Control 8
   {DEST_WAVEIN,            2,  2}, //Control 9
   {DEST_VOICEIN,           0,  2}, //Control 10
   {DEST_VOICEIN,           1,  2}, //Control 11

   //Mute controls

   {DEST_LINEOUT,           0,  1}, //Control 12
   {DEST_LINEOUT,           1,  1}, //Control 13
   {DEST_LINEOUT,           2,  1}, //Control 14
   {DEST_LINEOUT,           3,  1}, //Control 15
   {DEST_LINEOUT,    UINT_MAX,  1}, //Control 17
                                    
   //Peak meter controls                    

   {DEST_WAVEIN,            0,  2}, //Control 18
   {DEST_WAVEIN,            1,  2}, //Control 19
   {DEST_WAVEIN,            2,  2}, //Control 20

   {DEST_VOICEIN,           0,  2}, //Control 21
   {DEST_VOICEIN,           1,  2}, //Control 22

   {DEST_LINEOUT,           1,  2}  //Control 23
} ;

#endif
#ifdef MSSNDSYS

UINT BCODE gauControlMap[ MAXCONTROLS ][ 3 ] = 
{
   // Mux controls

   {DEST_WAVEIN,     UINT_MAX,  1}, //Control 0
   {DEST_VOICEIN,    UINT_MAX,  1}, //Control 1

   //Volume controls

   {DEST_LINEOUT,           0,  2}, //Control 2
   {DEST_LINEOUT,           1,  2}, //Control 3
   {DEST_LINEOUT,           2,  2}, //Control 4
   {DEST_LINEOUT,    UINT_MAX,  2}, //Control 5
   {DEST_WAVEIN,            0,  2}, //Control 6
   {DEST_WAVEIN,            1,  1}, //Control 7
   {DEST_VOICEIN,           0,  2}, //Control 8
   {DEST_VOICEIN,           1,  1}, //Control 9

   //Mute controls

   {DEST_LINEOUT,           0,  1}, //Control 10
   {DEST_LINEOUT,           1,  1}, //Control 11
   {DEST_LINEOUT,           2,  1}, //Control 12
   {DEST_LINEOUT,    UINT_MAX,  1}, //Control 13
                                    
   //Peak meter controls                    

   {DEST_WAVEIN,            0,  2}, //Control 14
   {DEST_WAVEIN,            1,  2}, //Control 15

   {DEST_VOICEIN,           0,  2}, //Control 16
   {DEST_VOICEIN,           1,  2}, //Control 17

   {DEST_LINEOUT,           1,  2}  //Control 18
} ;
#endif

//
// The number of controls "at" a destination is the number of controls above
// that had a destination == x and relative source # of UINT_MAX.
// These consts are plugged into the DefaultCardObjects structure.
//

#define CONTROLS_AT_DEST0 2
#define CONTROLS_AT_DEST1 1
#define CONTROLS_AT_DEST2 1
 
//----
//
// source map (for more info see mixer.c)
//
// The first index is the dest#.
// The second is the "relative" source #.
// The value is the real source # (to copy the info struct for).
//
//----

#ifdef WASHTON

UINT BCODE gauSourceMap[ MAXDESTINATIONS ][ MAXSOURCES ] =
{

   { SOURCE_AUX1, SOURCE_WAVEOUT, SOURCE_MIDIOUT, SOURCE_CD,  SOURCE_MICOUT, UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     SOURCE_CD,      SOURCE_MIX, UINT_MAX,      UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     UINT_MAX,       UINT_MAX,   UINT_MAX,      UINT_MAX }

/*
   { SOURCE_AUX1, SOURCE_WAVEOUT, SOURCE_MIDIOUT, UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     UINT_MAX,       UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     UINT_MAX,       UINT_MAX }
*/
} ;
#endif
#ifdef NOVA

UINT BCODE gauSourceMap[ MAXDESTINATIONS ][ MAXSOURCES ] =
{

   { SOURCE_AUX1, SOURCE_WAVEOUT, SOURCE_MIDIOUT, SOURCE_MICOUT, UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     SOURCE_MIX,     UINT_MAX,      UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     UINT_MAX,       UINT_MAX,      UINT_MAX }
} ;

#endif
#ifdef MSSNDSYS
UINT BCODE gauSourceMap[ MAXDESTINATIONS ][ MAXSOURCES ] =
{ 
   { SOURCE_AUX1, SOURCE_WAVEOUT, SOURCE_MIDIOUT, UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     UINT_MAX,       UINT_MAX },
   { SOURCE_AUX1, SOURCE_MIC,     UINT_MAX,       UINT_MAX }
} ;
#endif

//----
//
// source controls map (for more info see mixer.c)
//
//
// The first index is the dest#.
// The second is the "relative" source #.
// The value is the # of controls at
// this source as connected to this dest.
//
//----

#ifdef WASHTON

UINT BCODE gauSourceControlsMap[ MAXDESTINATIONS ][ MAXSOURCES ] =
{

   {  2,  3,  2,  2,  2,  0 },
   {  2,  2,  2,  2,  0,  0 },
   {  2,  2,  0,  0,  0,  0 }

/*
   {  2,  3,  2,  0 },
   {  2,  2,  0,  0 },
   {  2,  2,  0,  0 }
*/
} ;
#endif
#ifdef NOVA

UINT BCODE gauSourceControlsMap[ MAXDESTINATIONS ][ MAXSOURCES ] =
{

   {  2,  3,  2,  2,  0 },
   {  2,  2,  2,  0,  0 },
   {  2,  2,  0,  0,  0 }
} ;

#endif
#ifdef MSSNDSYS
UINT BCODE gauSourceControlsMap[ MAXDESTINATIONS ][ MAXSOURCES ] =
{
   {  2,  3,  2,  0 },
   {  2,  2,  0,  0 },
   {  2,  2,  0,  0 }
} ;
#endif

//***************************************************************************
//
//___READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS___
//
//  Be VERY CAREFUL when defining the initializers for these structures.
//  MSVC/C++ has a known problem when initializing structures within
//  unions.  YOU _MUST_ define each element in largest structure (e.g.
//  both elements in a Bounds structure).  If you fail to do so, the
//  compiler may generate invalid structures including additional
//  padding (sometimes 32 bytes) at the end of these array elements.
//  Unfortunately, this causes each following array element to be
//  misaligned.  Guarantee your structure definitions are correct by
//  defining the necessary elements as demonstrated below.  Also, it is
//  wise to generate a .COD file using the -Fc option to verify that
//  additional padding is not erroneously generated.
//
//___READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS___
//
//***************************************************************************

//
// Source defaults...
//

#ifdef WASHTON

MIXERLINE BCODE gmxlSources[ MAXSOURCES ] =
{
   // Source 0 - Aux1

   {
      sizeof(MIXERLINE),                     // cbStruct
      0,                                     // dwDestination
      SOURCE_AUX1,                           // dwSource
      (DWORD)-1L,                            // dwLineID
      MIXERLINE_LINEF_SOURCE,                // fdwLine
      0,                                     // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY, // dwComponentType
      2,                                     // cChannels
      0,                                     // cConnections
      0,                                     // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source1 - Wave Out
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_WAVEOUT,                         // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,    // dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_WAVEOUT,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source2 - MidiOut
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIDIOUT,                         // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,// dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source3 - Mic
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIC,                             // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, // dwComponentType

      2,                                      // cChannels

/*
#ifdef STEREOMIC
      2,                                      // cChannels
#else
      1,                                      // cChannels
#endif
*/
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   // Source 4 - MicOut
   // Mono MIC output connect to speaker, is different to record MIC
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MICOUT,                          // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, // dwComponentType
      1,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source5 - CD
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_CD,                              // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC,// dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source6 - Mixer
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIX,                             // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_ANALOG,     // dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   }

} ;
#endif
#ifdef NOVA

MIXERLINE BCODE gmxlSources[ MAXSOURCES ] =
{
   // Source 0 - Aux1

   {
      sizeof(MIXERLINE),                     // cbStruct
      0,                                     // dwDestination
      SOURCE_AUX1,                           // dwSource
      (DWORD)-1L,                            // dwLineID
      MIXERLINE_LINEF_SOURCE,                // fdwLine
      0,                                     // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY, // dwComponentType
      2,                                     // cChannels
      0,                                     // cConnections
      0,                                     // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source1 - Wave Out
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_WAVEOUT,                         // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,    // dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_WAVEOUT,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source2 - MidiOut
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIDIOUT,                         // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,// dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source3 - Mic
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIC,                             // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, // dwComponentType

      2,                                      // cChannels

/*
#ifdef STEREOMIC
      2,                                      // cChannels
#else
      1,                                      // cChannels
#endif
*/
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   // Source 4 - MicOut
   // Mono MIC output connect to speaker, is different to record MIC
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MICOUT,                          // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, // dwComponentType
      1,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source5 - Mixer
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIX,                             // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_ANALOG,     // dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   }

} ;

#endif
#ifdef MSSNDSYS
MIXERLINE BCODE gmxlSources[ MAXSOURCES ] =
{
   // Source 0 - Aux1

   {
      sizeof(MIXERLINE),                     // cbStruct
      0,                                     // dwDestination
      SOURCE_AUX1,                           // dwSource
      (DWORD)-1L,                            // dwLineID
      MIXERLINE_LINEF_SOURCE,                // fdwLine
      0,                                     // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY, // dwComponentType
      2,                                     // cChannels
      0,                                     // cConnections
      0,                                     // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source1 - Wave Out
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_WAVEOUT,                         // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,    // dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_WAVEOUT,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source2 - MidiOut
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIDIOUT,                         // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,// dwComponentType
      2,                                      // cChannels
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   },

   //Source3 - Mic
   {
      sizeof(MIXERLINE),                      // cbStruct
      0,                                      // dwDestination
      SOURCE_MIC,                             // dwSource
      (DWORD)-1L,                             // dwLineID
      MIXERLINE_LINEF_SOURCE,                 // fdwLine
      0,                                      // dwUser
      MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE, // dwComponentType
#ifdef STEREOMIC
      2,                                      // cChannels
#else
      1,                                      // cChannels
#endif
      0,                                      // cConnections
      0,                                      // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   }
} ;
#endif

//
// Destination defaults...
//

//***************************************************************************
//
//___READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS___
//
//    The destinations are inactive until a source on that destination
//    becomes active.  When a source at this destination becomes active
//    (e.g. wave-in is started for that selected source), the source @
//    that destination then becomes active and the destination becomes
//    active.
//
//    When a destination has a "persistent" source, such as line-in, the
//    destination is always active, unless this connection can be
//    selected using a mux or mixer - the destination then becomes active
//    when that the connection through the mux or mixer is enabled.
//
//___READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS__READTHIS___
//
//***************************************************************************

MIXERLINE BCODE gmxlDests[ MAXDESTINATIONS ] =
{
   // Destination0 - LineOut
   {
      sizeof(MIXERLINE),                     // cbStruct
      DEST_LINEOUT,                          // dwDestination
      0,                                     // dwSource
      (DWORD)-1L,                            // dwLineID
      0,                                     // fdwLine
      0,                                     // dwUser
      MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,  // dwComponentType
      2,                                     // cChannels
#ifdef MSSNDSYS
      3,                                     // cConnections
#endif
#ifdef WASHTON
      5,
#endif
#ifdef NOVA
      4,
#endif
      CONTROLS_AT_DEST0,                     // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_UNDEFINED,     
            0,                               // target dwDeviceID
            0,                               // target wMid
            0,                               // target wPid
            0,                               // target vDriverVersion
            { 0 }                            // target szPname
      }
   },

   // Destination 1 - WaveIn

   {
      sizeof(MIXERLINE),                     // cbStruct
      DEST_WAVEIN,                           // dwDestination
      0,                                     // dwSource
      (DWORD)-1L,                            // dwLineID
      0,                                     // fdwLine
      0,                                     // dwUser
      MIXERLINE_COMPONENTTYPE_DST_WAVEIN,    // dwComponentType
      2,                                     // cChannels
#ifdef MSSNDSYS
      2,                                     // cConnections
#endif
#ifdef WASHTON
      4,
#endif
#ifdef NOVA
      3,
#endif
      CONTROLS_AT_DEST1,                     // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_WAVEIN,
            0,                               // target dwDeviceID
            0,                               // target wMid
            0,                               // target wPid
            0,                               // target vDriverVersion
            { 0 }                            // target szPname
      }
   },

   // Destination 2 - VoiceIn

   {
      sizeof(MIXERLINE),                     // cbStruct
      DEST_VOICEIN,                          // dwDestination
      0,                                     // dwSource
      (DWORD)-1L,                            // dwLineID
      0,                                     // fdwLine
      SNDSYS_MIXERLINE_LOWPRIORITY,          // dwUser
      MIXERLINE_COMPONENTTYPE_DST_VOICEIN,   // dwComponentType
      2,                                     // cChannels
      2,                                     // cConnections
      CONTROLS_AT_DEST2,                     // cControls
      "",
      "",
      {
            MIXERLINE_TARGETTYPE_WAVEIN,
            0,      // target dwDeviceID
            0,      // target wMid
            0,      // target wPid
            0,      // target vDriverVersion
            { 0 }   // target szPname
      }
   }
} ;

//
// Control defaults...
//
#ifdef WASHTON

MIXERCONTROL BCODE gmxc[ MAXCONTROLS ] =
{
   // Control 0 - Mux that feeds Wavein

   {
      sizeof(MIXERCONTROL),               // cbStruct
      MUX_WAVEIN,                         // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUX,       // dwControlType
      MIXERCONTROL_CONTROLF_MULTIPLE |
      MIXERCONTROL_CONTROLF_UNIFORM,      // fdwControl
      4,
      "WaveInMux",                        // szShortName
      "Wave Input Mux",                   // szName
      {
         {
            0,                            // Bounds.dwMinimum
            1                             // Bounds.dwMaximum
         }
      },
      {
            1                             // Metrics
      }
   },

   // Control 1 - Mux that feeds Voicein

   {
      sizeof(MIXERCONTROL),               // cbStruct    
      MUX_VOICEIN,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUX,       // dwControlType
      MIXERCONTROL_CONTROLF_MULTIPLE |
      MIXERCONTROL_CONTROLF_UNIFORM,      // fdwControl
      2,                                  // cMultipleItems
      "VoiceInMux",                       // szShortName
      "Voice Input Mux",                  // szName
      {
         {
            0,                            // Bounds.dwMinimum
            1                             // Bounds.dwMaximum
         }
      },
      {
            1                             // Metrics
      }
   },

   // Control 2 -  Vol @ between Aux1 and Lineout

   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTAUX1,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "LineInVol",                        // szShortName
      "Volume Control for Line Input",    // szName
      {
         {
            0,                            // Bounds.dwMinimum
            0xFFFF                        // Bounds.dwMaximum
         }
      },
      {
            16                              // Metrics.cSteps
      }
   },

   //Control 3 - DAC output attenuation
   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTDAC,                         // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "WaveOutVol",                       // szShortName
      "Volume for Wave Out",              // szName
      {
         {
            0,                              // Bounds.dwMinimum
            0xFFFF                          // Bounds.dwMaximum
         }
      },
      {
            64                              // Metrics.cSteps
      }
   },

   //Control 4 - Midi out volume
   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTMIDI,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "MidiOutVol",                       // szShortName
      "Volume Control for MIDI Out",      // szName
      {
         {
            0,                              // Bounds.dwMinimum
            0xFFFF                          // Bounds.dwMaximum
         }
      },
      {
            64                              // Metrics.cSteps
      }
   },

   //Control 5 - CD out volume
   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTCD,                          // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "CDVol",                            // szShortName
      "Volume Control for CD",            // szName
      {
         {
            0,                            // Bounds.dwMinimum
            0xFFFF                        // Bounds.dwMaximum
         }
      },
      {
            16                              // Metrics.cSteps
      }
   },

   //Control 6 - Mono MIC out volume
       {
          sizeof(MIXERCONTROL),               // cbStruct
          VOL_OUTMIC,                         // dwControlID
          MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
          0,                                  // fdwControl
          0,                                  // cMultipleItems
          "MicVol",                           // szShortName
          "Volume Control for Microphone",    // szName
          {
             {
                0,                            // Bounds.dwMinimum
                0xFFFF                        // Bounds.dwMaximum
             }
          },
          {
                16                              // Metrics.cSteps
          }
       },

   //Control 7 - Master Output Volume
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_OUTLINE,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineOutVol",                   // szShortName
      "Volume for Line Out",          // szName
      {
         {
            0,                          // Bounds.dwMinimum
            0xFFFF,                     // Bounds.dwMaximum
         }
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 8 - Vol between Aux1 & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineInVol",                    // szShortName
      "Volume for Line In",           // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 9 - Vol between the Mic & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INMIC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MicVol",                       // szShortName
      "Volume for Microphone",        // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 10 - Vol between CD & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INCD,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "CDVol",                        // szShortName
      "Volume for CD",                // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 11 - Vol between Mix & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INMIX,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MixVol",                       // szShortName
      "Volume for Mix",             // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 12 - Vol between the AUX1 & Voicein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_V_INAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineInVol",                    // szShortName
      "Volume for Line In",           // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 13 - Vol between the Mic & Voicein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_V_INMIC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MicVol",                       // szShortName
      "Volume for Microphone",        // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 14 - Mute of Aux1 output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "LineInMute",                   // szShortName
      "Mute of Line Input",           // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 15 - Mute of DAC (wave-out) output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTDAC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "WaveOutMute",                  // szShortName
      "Mute of Wave Player",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 16 - Mute of MIDI output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTMIDI,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "MidiMute",                     // szShortName
      "Mute of Midi Output",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 17 - Mute of CD output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTCD,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "CDMute",                       // szShortName
      "Mute of CD Output",            // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 18 - Mute of Mono Mic output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTMIC,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "MicMute",                       // szShortName
      "Mute of Mic Output",            // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 19 - Mute of mixed DAC output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTLINE,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,     // fdwControl
      0,                              // cMultipleItems
      "LineOutMute",                  // szShortName
      "Mute of Line Output",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 20 - Peak Meter at WaveIn, AUX1
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_AUX1,                 // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-AUX1",                      // szShortName
      "VU for Wave Input (Aux)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 21 - Peak Meter at WaveIn, Mic
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_MIC,                  // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-Mic",                       // szShortName
      "VU for Wave Input (Mic)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 22 - Peak Meter at WaveIn, CD
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_CD,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-CD",                        // szShortName
      "VU for Wave Input (CD)",       // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 23 - Peak Meter at WaveIn, Mix
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_MIX,                  // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-Mix",                       // szShortName
      "VU for Wave Input (Mix)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 24 - Peak Meter at VoiceIn, AUX1
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_VOICEIN_AUX1,                // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-AUX1",                      // szShortName
      "VU for Voice Input (Aux)",     // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 25 - Peak Meter at VoiceIn, Mic
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_VOICEIN_MIC,                 // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-MIC",                       // szShortName
      "VU for Voice Input (Mic)",     // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 26 - Peak Meter at WaveOut source
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEOUT,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "WaveOutVu",                    // szShortName
      "VU for Wave Output",           // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   }

} ;
#endif
#ifdef NOVA

MIXERCONTROL BCODE gmxc[ MAXCONTROLS ] =
{
   // Control 0 - Mux that feeds Wavein

   {
      sizeof(MIXERCONTROL),               // cbStruct
      MUX_WAVEIN,                         // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUX,       // dwControlType
      MIXERCONTROL_CONTROLF_MULTIPLE |
      MIXERCONTROL_CONTROLF_UNIFORM,      // fdwControl
      3,
      "WaveInMux",                        // szShortName
      "Wave Input Mux",                   // szName
      {
         {
            0,                            // Bounds.dwMinimum
            1                             // Bounds.dwMaximum
         }
      },
      {
            1                             // Metrics
      }
   },

   // Control 1 - Mux that feeds Voicein

   {
      sizeof(MIXERCONTROL),               // cbStruct    
      MUX_VOICEIN,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUX,       // dwControlType
      MIXERCONTROL_CONTROLF_MULTIPLE |
      MIXERCONTROL_CONTROLF_UNIFORM,      // fdwControl
      2,                                  // cMultipleItems
      "VoiceInMux",                       // szShortName
      "Voice Input Mux",                  // szName
      {
         {
            0,                            // Bounds.dwMinimum
            1                             // Bounds.dwMaximum
         }
      },
      {
            1                             // Metrics
      }
   },

   // Control 2 - Vol @ between Aux1 and Lineout

   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTAUX1,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "LineInVol",                        // szShortName
      "Volume Control for Line Input",    // szName
      {
         {
            0,                            // Bounds.dwMinimum
            0xFFFF                        // Bounds.dwMaximum
         }
      },
      {
            16                              // Metrics.cSteps
      }
   },

   //Control 3 - DAC output attenuation
   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTDAC,                         // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "WaveOutVol",                       // szShortName
      "Volume for Wave Out",              // szName
      {
         {
            0,                              // Bounds.dwMinimum
            0xFFFF                          // Bounds.dwMaximum
         }
      },
      {
            64                              // Metrics.cSteps
      }
   },

   //Control 4 - Midi out volume
   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTMIDI,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "MidiOutVol",                       // szShortName
      "Volume Control for MIDI Out",      // szName
      {
         {
            0,                              // Bounds.dwMinimum
            0xFFFF                          // Bounds.dwMaximum
         }
      },
      {
            64                              // Metrics.cSteps
      }
   },

   //Control 5 - Mono MIC out volume
       {
          sizeof(MIXERCONTROL),               // cbStruct
          VOL_OUTMIC,                         // dwControlID
          MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
          0,                                  // fdwControl
          0,                                  // cMultipleItems
          "MicVol",                           // szShortName
          "Volume Control for Microphone",    // szName
          {
             {
                0,                            // Bounds.dwMinimum
                0xFFFF                        // Bounds.dwMaximum
             }
          },
          {
                16                              // Metrics.cSteps
          }
       },

   //Control 6 - Master Output Volume
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_OUTLINE,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineOutVol",                   // szShortName
      "Volume for Line Out",          // szName
      {
         {
            0,                          // Bounds.dwMinimum
            0xFFFF,                     // Bounds.dwMaximum
         }
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 7 - Vol between Aux1 & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineInVol",                    // szShortName
      "Volume for Line In",           // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 8 - Vol between the Mic & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INMIC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MicVol",                       // szShortName
      "Volume for Microphone",        // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 9 - Vol between Mix & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INMIX,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MixVol",                       // szShortName
      "Volume for Mix",             // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 10 - Vol between the AUX1 & Voicein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_V_INAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineInVol",                    // szShortName
      "Volume for Line In",           // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 11 - Vol between the Mic & Voicein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_V_INMIC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MicVol",                       // szShortName
      "Volume for Microphone",        // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control 12 - Mute of Aux1 output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "LineInMute",                   // szShortName
      "Mute of Line Input",           // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 13 - Mute of DAC (wave-out) output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTDAC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "WaveOutMute",                  // szShortName
      "Mute of Wave Player",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 14 - Mute of MIDI output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTMIDI,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "MidiMute",                     // szShortName
      "Mute of Midi Output",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 15 - Mute of Mono Mic output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTMIC,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "MicMute",                       // szShortName
      "Mute of Mic Output",            // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 16 - Mute of mixed DAC output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTLINE,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,     // fdwControl
      0,                              // cMultipleItems
      "LineOutMute",                  // szShortName
      "Mute of Line Output",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 17 - Peak Meter at WaveIn, AUX1
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_AUX1,                 // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-AUX1",                      // szShortName
      "VU for Wave Input (Aux)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 18 - Peak Meter at WaveIn, Mic
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_MIC,                  // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-Mic",                       // szShortName
      "VU for Wave Input (Mic)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 19 - Peak Meter at WaveIn, Mix
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_MIX,                  // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-Mix",                       // szShortName
      "VU for Wave Input (Mix)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 20 - Peak Meter at VoiceIn, AUX1
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_VOICEIN_AUX1,                // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-AUX1",                      // szShortName
      "VU for Voice Input (Aux)",     // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 21 - Peak Meter at VoiceIn, Mic
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_VOICEIN_MIC,                 // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-MIC",                       // szShortName
      "VU for Voice Input (Mic)",     // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control 22 - Peak Meter at WaveOut source
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEOUT,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "WaveOutVu",                    // szShortName
      "VU for Wave Output",           // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   }

} ;

#endif
#ifdef MSSNDSYS
MIXERCONTROL BCODE gmxc[ MAXCONTROLS ] =
{
   // Control 0 - Mux that feeds Wavein

   {
      sizeof(MIXERCONTROL),               // cbStruct
      MUX_WAVEIN,                         // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUX,       // dwControlType
      MIXERCONTROL_CONTROLF_MULTIPLE |
      MIXERCONTROL_CONTROLF_UNIFORM,      // fdwControl
      2,                                  // cMultipleItems
      "WaveInMux",                        // szShortName
      "Wave Input Mux",                   // szName
      {
         {
            0,                            // Bounds.dwMinimum
            1                             // Bounds.dwMaximum
         }
      },
      {
            1                             // Metrics
      }
   },

   // Control 1 - Mux that feeds Voicein

   {
      sizeof(MIXERCONTROL),               // cbStruct    
      MUX_VOICEIN,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUX,       // dwControlType
      MIXERCONTROL_CONTROLF_MULTIPLE |
      MIXERCONTROL_CONTROLF_UNIFORM,      // fdwControl
      2,                                  // cMultipleItems
      "VoiceInMux",                       // szShortName
      "Voice Input Mux",                  // szName
      {
         {
            0,                            // Bounds.dwMinimum
            1                             // Bounds.dwMaximum
         }
      },
      {
            1                             // Metrics
      }
   },

   // Control 2 -  Vol @ between Aux1 and Lineout

   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTAUX1,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "LineInVol",                        // szShortName
      "Volume Control for Line Input",    // szName
      {
         {
            0,                            // Bounds.dwMinimum
            0xFFFF                        // Bounds.dwMaximum
         }
      },
      {
            16                              // Metrics.cSteps
      }
   },

   //Control3     - DAC output attenuation
   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTDAC,                         // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "WaveOutVol",                       // szShortName
      "Volume for Wave Out",              // szName
      {
         {
            0,                              // Bounds.dwMinimum
            0xFFFF                          // Bounds.dwMaximum
         }
      },
      {
            64                              // Metrics.cSteps
      }
   },

   //Control4     - Midi out volume
   {
      sizeof(MIXERCONTROL),               // cbStruct    
      VOL_OUTMIDI,                        // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,    // dwControlType
      0,                                  // fdwControl
      0,                                  // cMultipleItems
      "MidiOutVol",                       // szShortName
      "Volume Control for MIDI Out",      // szName
      {
         {
            0,                              // Bounds.dwMinimum
            0xFFFF                          // Bounds.dwMaximum
         }
      },
      {
            64                              // Metrics.cSteps
      }
   },

   //Control5     -  "Master Output Volume"
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_OUTLINE,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineOutVol",                   // szShortName
      "Volume for Line Out",          // szName
      {
         {
            0,                          // Bounds.dwMinimum
            0xFFFF,                     // Bounds.dwMaximum
         }
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control6 -  Vol between Aux1 & Wavein 
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineInVol",                    // szShortName
      "Volume for Line In",           // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control7 - Vol between the Mic & Wavein
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_W_INMIC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MicVol",                       // szShortName
      "Volume for Microphone",        // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control8 -  Vol between the AUX1 & Voicein 
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_V_INAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "LineInVol",                    // szShortName
      "Volume for Line In",           // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control9 -  Vol between the Mic & Voicein 
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VOL_V_INMIC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_VOLUME,// dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "MicVol",                       // szShortName
      "Volume for Microphone",        // szName
      {
            0,                          // Bounds.dwMinimum
            0xFFFF                      // Bounds.dwMaximum
      },
      {
            16                          // Metrics.cSteps
      }
   },

   //Control10     - Mute of Aux1 output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTAUX1,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "LineInMute",                   // szShortName
      "Mute of Line Input",           // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control11     - Mute of DAC (wave-out) output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTDAC,                    // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "WaveOutMute",                  // szShortName
      "Mute of Wave Player",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control12     - Mute of MIDI output
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTMIDI,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,  // fdwControl
      0,                              // cMultipleItems
      "MidiMute",                     // szShortName
      "Mute of Midi Output",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control13     - Mute of mixed DAC output 
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      MUTE_OUTLINE,                   // dwControlID
      MIXERCONTROL_CONTROLTYPE_MUTE,  // dwControlType
      MIXERCONTROL_CONTROLF_UNIFORM,     // fdwControl
      0,                              // cMultipleItems
      "LineOutMute",                  // szShortName
      "Mute of Line Output",          // szName
      {
            0,                          // Bounds.dwMinimum
            1                           // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control14     - Peak Meter at WaveIn, AUX1
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_AUX1,                 // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-AUX1",                      // szShortName
      "VU for Wave Input (Aux)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control15     - Peak Meter at WaveIn, Mic
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEIN_MIC,                  // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-Mic",                       // szShortName
      "VU for Wave Input (Mic)",      // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control16     - Peak Meter at VoiceIn, AUX1
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_VOICEIN_AUX1,                // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-AUX1",                      // szShortName
      "VU for Voice Input (Aux)",     // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control17     - Peak Meter at VoiceIn, Mic
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_VOICEIN_MIC,                 // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "VU-MIC",                       // szShortName
      "VU for Voice Input (Mic)",     // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   },

   //Control18     - Peak Meter at WaveOut source
   {
      sizeof(MIXERCONTROL),           // cbStruct    
      VU_WAVEOUT,                     // dwControlID
      MIXERCONTROL_CONTROLTYPE_PEAKMETER,  // dwControlType
      0,                              // fdwControl
      0,                              // cMultipleItems
      "WaveOutVu",                    // szShortName
      "VU for Wave Output",           // szName
      {
            -32768,                     // Bounds.dwMinimum
            32767                       // Bounds.dwMaximum
      },
      {
            0                           // Metrics
      }
   }

} ;
#endif
