// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
//
//  FILE:     mixinfo.h
//
//  PURPOSE:  Header file for mixinfo.c
//
//  COMMENTS: The mixer simulation data is provided for the purpose of enabling
//            the MixTree sample to run on systems that do not include a mixer
//            device.  It does not provide all the initialization information
//            required of actual mixer controls and is not a complete 
//            simulation.

#define SIMLINES        7  // total number of simulated mixer lines
#define SIMDESTLINES    3  // number of simulated destination lines
#define SIMSRCLINES     4  // number of simulated source lines
#define MAXSRCLINES     3  // max source lines associated with a destination
#define SIMCONTROLS     19 // number of simulated mixer controls
#define MAXLINECONTROLS 3  // max controls associated with a single line

// array of simulated mixer lines
// destination lines MUST come first!
MIXERLINE SimLine[SIMLINES] =
{
    // Destination 0 = SimLine[0]
    {
        sizeof(MIXERLINE),
        0,
        0,
        0,
        0,
        0,
        MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
        2,
        3,
        2,
        "Line Out",
        "Line Output",
        {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,
            0,
            0,
            0,
            "",
        }
    },

    // Destination 1 = SimLine[1]
    {
        sizeof(MIXERLINE),
        1,
        0,
        1,
        0,
        0,
        MIXERLINE_COMPONENTTYPE_DST_WAVEIN,
        2,
        2,
        1,
        "Wave In",
        "Wave Input",
        {
            MIXERLINE_TARGETTYPE_WAVEIN,
            0,
            0,
            0,
            0,
            "Simulated Mixer Device",
        }
    },

    // Destination 2 = SimLine[2]
    {
        sizeof(MIXERLINE),
        2,
        0,
        2,
        0,
        0,
        MIXERLINE_COMPONENTTYPE_DST_VOICEIN,
        2,
        2,
        1,
        "Voice In",
        "Voice Input",
        {
            MIXERLINE_TARGETTYPE_WAVEIN,
            0,
            0,
            0,
            0,
            "Simulated Mixer Device",
        }
    },

    // Source 0 = SimLine[3]
    {
        sizeof(MIXERLINE),
        0,
        0,
        3,
        MIXERLINE_LINEF_SOURCE,
        0,
        MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY,
        2,
        0,
        3,
        "Aux In",
        "Auxiliary Input",
        {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,
            0,
            0,
            0,
            "",
        }
    },

    // Source 1 = SimLine[4]
    {
        sizeof(MIXERLINE),
        0,
        1,
        4,
        MIXERLINE_LINEF_SOURCE,
        0,
        MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,
        2,
        0,
        3,
        "Wave Out",
        "Wave Output",
        {
            MIXERLINE_TARGETTYPE_WAVEOUT,
            0,
            0,
            0,
            0,
            "Simulated Mixer Device",
        }
    },

    // Source 2 = SimLine[5]
    {
        sizeof(MIXERLINE),
        0,
        2,
        5,
        MIXERLINE_LINEF_SOURCE,
        0,
        MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,
        2,
        0,
        2,
        "Synth Out",
        "Synthesizer Output",
        {
            MIXERLINE_TARGETTYPE_MIDIOUT,
            0,
            0,
            0,
            0,
            "Simulated Synth Device",
        }
    },

    // Source 3 = SimLine[6]
    {
        sizeof(MIXERLINE),
        0,
        3,
        6,
        MIXERLINE_LINEF_SOURCE,
        0,
        MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE,
        2,
        0,
        2,
        "Mic In",
        "Microphone Input",
        {
            MIXERLINE_TARGETTYPE_UNDEFINED,
            0,
            0,
            0,
            0,
            "Simulated Mixer Device",
        }
    }
};

// array of source line ID's associated with destination lines
DWORD SimSourceID[SIMDESTLINES][MAXSRCLINES] = {
    {
        3, 4, 5   // dest 0 (Line Out)
    }, 
    {
        3, 6, 0   // dest 1 (Wave In)
    },
    {
        3, 6, 0   // dest 2 (Voice In)
    }
};

// array of control ID's associated with simulated mixer lines
DWORD SimLineControlID[SIMLINES][MAXLINECONTROLS] = {
    {
        5, 10, 0   // dest 0 (Line Out)
    },
    {   
        1, 0,  0   // dest 1 (Wave In)
    },
    { 
        0, 0,  0   // dest 2 (Voice In)
    },
    {    
        2, 7,  11  // src 0  (Aux In)
    },
    {   
        3, 8,  13  // src 1  (Wave Out)
    },                       
    {   
        4, 9,  0   // src 2  (Synth Out)
    },                       
    {   
        6, 12, 0   // src 3  (Mic In)
    }
};


// array of simulated mixer line controls
MIXERCONTROL SimLineControl[SIMCONTROLS] = {
    // Control 0
    {
        sizeof(MIXERCONTROL),
        0,
        MIXERCONTROL_CONTROLTYPE_MUX,
        0,
        2,
        "VoxInMux",
        "Voice Input Multiplex",
        {
            {
                0,
                1
            }
        },
        {
            1
        }
    },

    // Control 1
    {
        sizeof(MIXERCONTROL),
        1,
        MIXERCONTROL_CONTROLTYPE_MUX,
        0,
        2,
        "WavInMux",
        "Wave Input Multiplex",
        {
            {
                0,
                1
            }
        },
        {
            1
        }
    },

    // Control 2
    {
        sizeof(MIXERCONTROL),
        2,
        MIXERCONTROL_CONTROLTYPE_VOLUME,
        0,
        0,
        "AuxInVol",
        "Auxiliary Input Volume",
        {
            {
                0,
                0xFFFF
            }
        },
        {
            16
        }
    },

    // Control 3
    {
        sizeof(MIXERCONTROL),
        3,
        MIXERCONTROL_CONTROLTYPE_VOLUME,
        0,
        0,
        "WaveOutVol",
        "Wave Output Volume",
        {
            {
                0,
                0xFFFF
            }
        },
        {
            16
        }
    },

    // Control 4
    {
        sizeof(MIXERCONTROL),
        4,
        MIXERCONTROL_CONTROLTYPE_VOLUME,
        0,
        0,
        "MidiOutVol",
        "MIDI Output Volume",
        {
            {
                0,
                0xFFFF
            }
        },
        {
            16
        }
    },
    
    // Control 5
    {
        sizeof(MIXERCONTROL),
        5,
        MIXERCONTROL_CONTROLTYPE_VOLUME,
        0,
        0,
        "LineOutVol",
        "Line Output Volume",
        {
            {
                0,
                0xFFFF
            }
        },
        {
            16
        }
    },

    // Control 6
    {
        sizeof(MIXERCONTROL),
        6,
        MIXERCONTROL_CONTROLTYPE_VOLUME,
        0,
        0,
        "MicInVol",
        "Microphone Input Volume",
        {
            {
                0,
                0xFFFF
            }
        },
        {
            16
        }
    },

    // Control 7
    {
        sizeof(MIXERCONTROL),
        7,
        MIXERCONTROL_CONTROLTYPE_MUTE,
        0,
        0,
        "AuxInMute",
        "Auxiliary Input Mute",
        {
            {
                0,
                1
            }
        },
        {
            0
        }
    },

    // Control 8
    {
        sizeof(MIXERCONTROL),
        8,
        MIXERCONTROL_CONTROLTYPE_MUTE,
        0,
        0,
        "WaveOutMute",
        "Wave Output Mute",
        {
            {
                0,
                1
            }
        },
        {
            0
        }
    },

    // Control 9
    {
        sizeof(MIXERCONTROL),
        9,
        MIXERCONTROL_CONTROLTYPE_MUTE,
        0,
        0,
        "MidiOutMute",
        "MIDI Output Mute",
        {
            {
                0,
                1
            }
        },
        {
            0
        }
    },

    // Control 10
    {
        sizeof(MIXERCONTROL),
        10,
        MIXERCONTROL_CONTROLTYPE_MUTE,
        0,
        0,
        "LineOutMute",
        "Line Output Mute",
        {
            {
                0,
                1
            }
        },
        {
            0
        }
    },

    // Control 11
    {
        sizeof(MIXERCONTROL),
        11,
        MIXERCONTROL_CONTROLTYPE_PEAKMETER,
        0,
        0,
        "AuxInPeak",
        "Auxiliary Input Peak Meter",
        {
            {
                -32768,
                32767
            }
        },
        {
            0
        }
    },

    // Control 12
    {
        sizeof(MIXERCONTROL),
        12,
        MIXERCONTROL_CONTROLTYPE_PEAKMETER,
        0,
        0,
        "MicInPeak",
        "Microphone Input Peak Meter",
        {
            {
                -32768,
                32767
            }
        },
        {
            0
        }
    },

    // Control 13
    {
        sizeof(MIXERCONTROL),
        13,
        MIXERCONTROL_CONTROLTYPE_PEAKMETER,
        0,
        0,
        "WaveOutPeak",
        "Wave Output Peak Meter",
        {
            {
                -32768,
                32767
            }
        },
        {
            0
        }
    }   
};

