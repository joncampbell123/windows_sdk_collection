//---------------------------------------------------------------------------
//
//   MIXER.H
//
//---------------------------------------------------------------------------
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/
#include <mmsystem.h>

//
//
//
//
#ifndef FIELD_OFFSET
    #define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))
#endif

//***************************************************************************
//***************************************************************************

#ifdef WASHTON
    #define MAXSOURCES      7
    #define MAXDESTINATIONS 3
    #define MAXCONTROLS     27
#endif
#ifdef NOVA
    #define MAXSOURCES      6
    #define MAXDESTINATIONS 3
    #define MAXCONTROLS     23
#endif
#ifdef MSSNDSYS
    #define MAXSOURCES      4
    #define MAXDESTINATIONS 3
    #define MAXCONTROLS     19
#endif

typedef struct tagMXDCLIENT NEAR *PMXDCLIENT ;

typedef struct tagMIXERINSTANCE
{
     PMXDCLIENT        pMxdClients ;

     MIXERLINE         mxlSources[ MAXSOURCES ] ;
     MIXERLINE         mxlDests[ MAXDESTINATIONS ] ;
     MIXERCONTROL      mxc[ MAXCONTROLS ] ;

     //
     // control mapping
     //
     // The control map describes the controls, their
     // # of channels and locations.
     //

     UINT              auControlMap[ MAXCONTROLS ][ 3 ] ;

     //
     // source mapping
     //
     // The source map describes the connections for each
     // destination.  This maps a relative source number at a
     // destination to an actual source number in the mixer
     // source list.
     //

     UINT              auSourceMap[ MAXDESTINATIONS ][ MAXSOURCES ] ;

     //
     // source controls maps
     //
     // This array describes the number of controls for a given
     // source at a destination.
     //

     UINT              auSourceControlsMap[ MAXDESTINATIONS ][ MAXSOURCES ] ;

     //
     // active/disconnect map
     //
     // This array describes the connection status for a given
     // source at a destination.  This allows a source to become
     // disconnected or active dynamically.
     //      

     DWORD             adwActiveMap[ MAXDESTINATIONS ][ MAXSOURCES ] ;

     //
     // Control value storage arrays...
     //

     union
     {
       DWORD           dwValue[ MAXCONTROLS ][ 2 ] ;
       LONG            lValue[ MAXCONTROLS ][ 2 ] ;
     };

} MIXERINSTANCE, FAR *PMIXERINSTANCE ;

typedef struct tagMXDCLIENT
{
    DWORD              fdwOpen ; 
    HMIXER             hmx ;           // handle that will be used
    DWORD              dwCallback ;    // callback
    UINT               fuCallback ;    // callback
    DWORD              dwInstance ;    // app's private instance information
    PHARDWAREINSTANCE  phwi ;
    PMXDCLIENT         pNext ;         // for list of clients

} MXDCLIENT ;

// driver internal defintion for line mute status

#define IMIXERLINE_LINEF_MUTED   0x08000000L

typedef MMRESULT FAR PASCAL CONTROLFUNCTION( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS ) ;
extern CONTROLFUNCTION* BCODE ControlFunctionTable[ MAXCONTROLS ] ;

extern UINT BCODE         gauControlMap[ MAXCONTROLS ][ 3 ] ;
extern UINT BCODE         gauSourceMap[ MAXDESTINATIONS ][ MAXSOURCES ] ;
extern UINT BCODE         gauSourceControlsMap[ MAXDESTINATIONS ][ MAXSOURCES ] ; 
extern MIXERLINE BCODE    gmxlSources[ MAXSOURCES ] ;
extern MIXERLINE BCODE    gmxlDests[ MAXDESTINATIONS ] ;
extern MIXERCONTROL BCODE gmxc[ MAXCONTROLS ] ;


//***************************************************************************
//***************************************************************************
//***************************************************************************

//Mixer.c

MMRESULT FAR PASCAL MxdInit( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL MxdEnd( PHARDWAREINSTANCE ) ;
MMRESULT FAR PASCAL MxdGetControlDetails( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS, DWORD ) ;
MMRESULT FAR PASCAL MxdSetControlDetails( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS, DWORD ) ;

VOID FAR PASCAL MxdUpdateLine( PHARDWAREINSTANCE, UINT, UINT, BOOL, UINT ) ;

#define MXDUPDATELINE_ACTIONF_LINESTATUS  0x0000
#define MXDUPDATELINE_ACTIONF_MUTESTATUS  0x0001

#define MXDUPDATELINE_ACTIONF_DESTINATION 0x0000
#define MXDUPDATELINE_ACTIONF_SOURCE      0x0010


/* Controls.C */

MMRESULT FAR PASCAL MixSetMasterVolume( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS ) ;
MMRESULT FAR PASCAL MixSetMidiVolume( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS ) ;
MMRESULT FAR PASCAL MixSetVolume( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS ) ;
#ifdef AZTECH
MMRESULT FAR PASCAL MixSetMonoVolume( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS ) ;
#endif
MMRESULT FAR PASCAL MixSetMute( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS ) ;
MMRESULT FAR PASCAL MixSetADCHardware
(
    PHARDWAREINSTANCE       phwi,
    LPMIXERCONTROLDETAILS   pmcd
) ;

MMRESULT FAR PASCAL MixSetFails( PHARDWAREINSTANCE, LPMIXERCONTROLDETAILS ) ;

MMRESULT     PASCAL MixUpdateGlobalLevels( PHARDWAREINSTANCE ) ;


#ifdef DEBUG
 #define AssertT(x) {  if(!(x)) {DPF( 1, "AssertT failed (" __FILE__ "):\r\n  "#x "\r\n"); DebugBreak();};};
 #define AssertF(x) {  if(!(x)) {DPF( 1, "AssertF failed (" __FILE__ "):\r\n  "#x "\r\n"); DebugBreak();};};
#else
 #define AssertT(x)
 #define AssertF(x)
#endif


//***************************************************************************
//***************************************************************************
// Defines for the uControlMap "columns"
#define CM_CHANNELS 2

//***************************************************************************
//***************************************************************************
// Define some stuff for the muxes
// NOTE: These are NOT the physical settings for the chip, they will get
//       translated to that later.
#ifdef WASHTON
    #define MUXINPUT_AUX1       0
    #define MUXINPUT_MIC        1
    #define MUXINPUT_CD         2
    #define MUXINPUT_MIX        3
#endif
#ifdef NOVA
    #define MUXINPUT_AUX1       0
    #define MUXINPUT_MIC        1
    #define MUXINPUT_MIX        2
#endif
#ifdef MSSNDSYS
    #define MUXINPUT_AUX1       0
    #define MUXINPUT_MIC        1
#endif

//***************************************************************************
//***************************************************************************
// Define some stuff for sources...
#ifdef WASHTON
    #define SOURCE_AUX1     0
    #define SOURCE_WAVEOUT  1
    #define SOURCE_MIDIOUT  2
    #define SOURCE_MIC      3
    #define SOURCE_MICOUT   4
    #define SOURCE_CD       5
    #define SOURCE_MIX      6
    #define SOURCE_INVALID           (UINT_MAX)   /// for source mapping
#endif
#ifdef NOVA
    #define SOURCE_AUX1     0
    #define SOURCE_WAVEOUT  1
    #define SOURCE_MIDIOUT  2
    #define SOURCE_MIC      3
    #define SOURCE_MICOUT   4
    #define SOURCE_MIX      5
    #define SOURCE_INVALID           (UINT_MAX)   /// for source mapping
#endif
#ifdef MSSNDSYS
    #define SOURCE_AUX1     0
    #define SOURCE_WAVEOUT  1
    #define SOURCE_MIDIOUT  2
    #define SOURCE_MIC      3
    #define SOURCE_INVALID           (UINT_MAX)   /// for source mapping
#endif

//***************************************************************************
//***************************************************************************
// Define some stuff for destinations...
#define DEST_LINEOUT      0
#define DEST_WAVEIN       1
#define DEST_VOICEIN      2

//***************************************************************************
//***************************************************************************
// Define some stuff for controls...

//
//Define Control numbers
//
#ifdef WASHTON
    #define MUX_WAVEIN                0
    #define MUX_VOICEIN               1
    
    #define VOL_OUTAUX1               2
    #define VOL_OUTDAC                3
    #define VOL_OUTMIDI               4
    #define VOL_OUTCD                 5
    #define VOL_OUTMIC                6
    #define VOL_OUTLINE               7
    #define VOL_W_INAUX1              8
    #define VOL_W_INMIC               9
    #define VOL_W_INCD               10
    #define VOL_W_INMIX              11
    #define VOL_V_INAUX1             12
    #define VOL_V_INMIC              13
    
    #define MUTE_OUTAUX1             14
    #define MUTE_OUTDAC              15
    #define MUTE_OUTMIDI             16
    #define MUTE_OUTCD               17
    #define MUTE_OUTMIC              18
    #define MUTE_OUTLINE             19
    
    #define VU_WAVEIN_AUX1           20
    #define VU_WAVEIN_MIC            21
    #define VU_WAVEIN_CD             22
    #define VU_WAVEIN_MIX            23
    #define VU_VOICEIN_AUX1          24
    #define VU_VOICEIN_MIC           25
    #define VU_WAVEOUT               26
#endif
#ifdef NOVA
    #define MUX_WAVEIN                0
    #define MUX_VOICEIN               1
    
    #define VOL_OUTAUX1               2
    #define VOL_OUTDAC                3
    #define VOL_OUTMIDI               4
    #define VOL_OUTMIC                5
    #define VOL_OUTLINE               6
    #define VOL_W_INAUX1              7
    #define VOL_W_INMIC               8
    #define VOL_W_INMIX               9
    #define VOL_V_INAUX1             10
    #define VOL_V_INMIC              11
    
    #define MUTE_OUTAUX1             12
    #define MUTE_OUTDAC              13
    #define MUTE_OUTMIDI             14
    #define MUTE_OUTMIC              15
    #define MUTE_OUTLINE             16
    
    #define VU_WAVEIN_AUX1           17
    #define VU_WAVEIN_MIC            18
    #define VU_WAVEIN_MIX            19
    #define VU_VOICEIN_AUX1          20
    #define VU_VOICEIN_MIC           21
    #define VU_WAVEOUT               22
#endif
#ifdef MSSNDSYS
    #define MUX_WAVEIN                0
    #define MUX_VOICEIN               1
    
    #define VOL_OUTAUX1               2
    #define VOL_OUTDAC                3
    #define VOL_OUTMIDI               4
    #define VOL_OUTLINE               5
    #define VOL_W_INAUX1              6
    #define VOL_W_INMIC               7
    #define VOL_V_INAUX1              8
    #define VOL_V_INMIC               9  
    
    #define MUTE_OUTAUX1             10
    #define MUTE_OUTDAC              11
    #define MUTE_OUTMIDI             12
    #define MUTE_OUTLINE             13
    
    #define VU_WAVEIN_AUX1           14
    #define VU_WAVEIN_MIC            15
    #define VU_VOICEIN_AUX1          16
    #define VU_VOICEIN_MIC           17
    #define VU_WAVEOUT               18
#endif

#define CONTROL_ATDEST           (UINT_MAX)
