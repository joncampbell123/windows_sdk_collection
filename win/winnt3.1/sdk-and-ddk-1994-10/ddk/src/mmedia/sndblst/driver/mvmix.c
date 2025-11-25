/*++

Copyright (c) 1992 Media Vision, Inc

Module Name:

    mvmix.c

Abstract:

    This module contains code for controlling the Media Vision Mixer
     hardware.  Media Vision has used only two software-controllable
     mixer chips so far.  The MV 508 is used on the PAS 16 only.  All
     other chips use the National part XXXX.  Programming the
     Nationial mixer chip is VOODOO BLACK MAGIC and will be very slow
     under NT.

Environment:

    Kernel mode

Revision History:

    27-Sep-1992
        Initial revision

--*/

#include <sound.h>

void SetEq(PFOUNDINFO pFI, USHORT P_output, USHORT P_EQtype,USHORT  P_level);
void SetEqMode(PFOUNDINFO pFI, USHORT P_loudness, USHORT P_enhance);
void NationalMix(PFOUNDINFO pFI, USHORT wData);
void NationalVolume(PFOUNDINFO pFI, USHORT wVolume,UCHAR wVolumeRegister);
ULONG Scale100ToFFFF(USHORT wVal);
BYTE ScaleFFFFTo100(USHORT wVal);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(init,SetOutput)
#pragma alloc_text(init,SetFilter)
#endif

//
//-------------------------========================---------------------------
//------------------------====< DATA SECTION >====---------------------------
//-------------------------========================---------------------------
//
//
// These variables are DMA/Buffer control variables
//

//
// These variables mirror the hardware state
//
UCHAR audiomixr= SERIAL_MIX_CLOCK+SERIAL_MIX_DUALFM ;
UCHAR audiofilt;          //  B8A Audio Filter Control Register

//
// a linear table of filter values - from mute to high
//
UCHAR   FilterTable[]=
    {
    0x00,       //   000000b    mute - goes to PC speaker
    0x24,       //   100100b    20hz to  2.9khz
    0x39,       //   111001b    20hz to  5.9khz
    0x31,       //   110001b    20hz to  8.9khz
    0x29,       //   101001b    20hz to 11.9khz
    0x22,       //   100010b    20hz to 15.9khz
    0x21,       //   100001b    20hz to 17.8khz
    };
//
//Mixer settings (0 - 12)
//
UCHAR   mixersettings[]=
    {
    0x00,       // level 0
    0x20,       // level 1
    0x10,       // level 2
    0x08,       // level 3
    0x04,       // level 4
    0x02,       // level 5
    0x12,       // level 6
    0x2A,       // level 7
    0x16,       // level 8
    0x01,       // level 9
    0x29,       // level A
    0x1D,       // level B
    0x2F        // level C
    };

UCHAR   SampleFilterSetting=0;  // default setting based on sample rate

//
// Volume settings (0 - 12)
//

UCHAR volsettings[]=
    {
    0x00,       // level 0
    0x60,       // level 1
    0x50,       // level 2
    0x48,       // level 3
    0x44,       // level 4
    0x42,       // level 5
    0x52,       // level 6
    0x6A,       // level 7
    0x56,       // level 8
    0x41,       // level 9
    0x69,       // level A
    0x5D,       // level B
    0x6F        // level C
    };

//
// Xlat table   lookup= 0-31
//             result= 0-12
//

UCHAR Scale32To12[]=
    {
    0 , 1, 1, 2, 2, 3, 3, 3,    //  0- 7
    4 , 4, 4, 5, 5, 5, 6, 6,    //  8-15
    6 , 7, 7, 7, 8, 8, 8, 9,    // 16-23
    9 , 9,10,10,11,11,12,12     // 24-31
    };


UCHAR   Scale64To40[]=
    {
    0 , 1, 2, 3, 4, 5, 6, 7,   //  0- 7
    8 , 8, 9,10,10,11,12,12,   //  8-15
    13,14,14,15,16,16,17,18,   // 16-23
    18,19,20,20,21,22,22,23,   // 24-31
    24,24,25,26,26,27,28,28,   // 32-39
    29,29,30,30,31,31,32,32,   // 40-47
    32,33,33,34,34,35,35,35,   // 48-55
    36,36,37,37,38,38,39,40    // 56-63
    };

UCHAR   BassTreb32to13[]=
    {
    0 , 1, 1, 2, 2, 3, 3, 3,   //  0- 7
    4 , 4, 4, 5, 5, 5, 6, 6,   //  8-15
    6 , 7, 7, 7, 8, 8, 8, 9,   // 16-23
    9 , 9,10,10,11,11,12,12    // 24-31
    };


    /*\
;---|*|-=< void SetInput (USHORT P_input_num, USHORT P_volume_lvl,USHORT P_channel,
;---|*|            USHORT P_crossover,USHORT P_output_num )
;---|*|
;---|*|       Set the selected channel within the Input Mixer
;---|*|
;---|*| Entry Conditions:
;---|*|       P_input_num   = Input  # (0-6 for serial mixer, 0-7 for 508)
;---|*|       P_volume_lvl = volume level (0-FFFF)
;---|*|       P_channel    = LEFT, RIGHT or BOTH
;---|*|       P_crossover   = Crossover Info (required onlby for 508)
;---|*|       P_output_num = Output #   (0-1 for either mixer)
;---|*|
;---|*| Exit Conditions:
;---|*|       None
;---|*|
;   \*/

void
SetInput (PFOUNDINFO pFI, UCHAR P_input_num, USHORT P_volume_lvl,USHORT P_channel,
           USHORT P_crossover,UCHAR P_output_num )
{
    UCHAR   bTemp,bTemp1;

    P_volume_lvl>>=(16-5);              // convert 0-ffff to 0-31

    if (pFI->Caps.CapsBits.Mixer_508)
        {
        bTemp=(UCHAR) P_input_num;          // Channel #

        bTemp|=(P_channel<<5);              // get left/right
        bTemp|=MV_508_ADDRESS;              // 508 ADDRESS BIT
        bTemp|=MV_508_INPUT;                // 508 INPUT BIT

        dprintf4(("Input Mixer 508 Address   : %04x", bTemp));
        PASX_OUT(pFI, MIXER_508_REG, bTemp);

        bTemp=P_volume_lvl;     // get volume level reduced to (0-31 range)

        bTemp1=P_crossover;     // get CROSSOVER

        if (bTemp1==MIXCROSSCAPS_NORMAL_STEREO)
            {
            }
        else
            if (bTemp1 & MIXCROSSCAPS_REVERSE_STEREO)
                {
                bTemp |= MV_508_SWAP;       // this channel's swapped
                }
            else
                {
                if (P_channel==_LEFT)
                    {
                    if (bTemp1 & MIXCROSSCAPS_LEFT_TO_RIGHT)
                        bTemp |= MV_508_SWAP;       // this channel's swapped
                    }
                else
                    {
                    if (bTemp1 & MIXCROSSCAPS_LEFT_TO_RIGHT)
                        bTemp |= MV_508_SWAP;       // this channel's swapped
                    }
                }

        bTemp|=((P_output_num & 1 )<<5);            // select output number
        dprintf4(("Input Mixer 508 Data   : %04x", bTemp));
        PASX_OUT(pFI, MIXER_508_REG,bTemp);

        }
    else    // National (serially programmed) mixer
        {
        dprintf2(("-----------Using Serial Mixer!"));
        //
        // send out the mixer channel #
        //

        bTemp=P_input_num;                      // Channel #
        bTemp++;                                // channel 0 was XXX

        if (P_channel==_RIGHT)
            bTemp+=7;    // this magic number is the offset to right channel

        bTemp|=NATIONAL_COMMAND;
        NationalMix(pFI, bTemp);

        // select the correct mixer
        bTemp1=P_output_num<<6;                 // get output number

        //
        // send out the mixer data
        //

        bTemp=P_volume_lvl &= 0x1F;                 // limit to 31
        bTemp=Scale32To12[bTemp];
        bTemp=mixersettings[bTemp];

        NationalMix(pFI, bTemp);
        }
}

    /*\
;---|*|--------====< void SetOutput (line, level, channel ) >====--------
;---|*|
;---|*|       This routine outputs a new setting for a volume channel.
;---|*|
;---|*| Entry Conditions:
;---|*|       WParm1 is a value from 0 - 1
;---|*|       WParm2 is a value to be written to the control (0-63)
;---|*|       WParm3 signifies left or right
;---|*|
;---|*| Exit Conditions:
;---|*|       None
;---|*|
;   \*/
void
SetOutput (PFOUNDINFO pFI, UCHAR P_output_num,USHORT P_volume_lvl,USHORT P_channel )
{
    UCHAR   bTemp,bTemp1;

    dprintf2(("SetOutput Entered"));
    P_volume_lvl>>=(16-6);              // convert 0-ffff to 0-63

    if (pFI->Caps.CapsBits.Mixer_508)
        {
        //EnterCrit             /// don't interrupt me!

        // investigate keSynchronizeExecution

        bTemp=(UCHAR) P_output_num;
        bTemp++;                // Output number need to be 1 based

        bTemp|=(P_channel<<5);  // get left/right
        bTemp|=MV_508_ADDRESS;

        dprintf4(("Output Mixer 508 Address   : %04x", bTemp));
        PASX_OUT(pFI, MIXER_508_REG,bTemp);

#if 0
        if (fMuting)
            {
            bTemp=0;
            }
        else
#endif
            {
            bTemp=P_volume_lvl;     // get volume level (0-63 range)

            if (P_output_num!=OUT_AMPLIFIER)
                bTemp>>=2;          // output B of MV508 has 0-15 range
            }
        dprintf4(("  Mixer 508 Data   : %04x", bTemp));
        PASX_OUT(pFI, MIXER_508_REG,bTemp);
        }
    else    // NATIONAL (SERIAL PROGRAMMED) MIXER
        {
        dprintf2(("Using Serial Mixer!"));
        if (P_output_num==0)    // serial device has volume on output 0 only
            {
#if 0
            if (fMuting)
                {
                bTemp=0;
                }
            else
#endif
                {
                bTemp=Scale64To40[P_volume_lvl];
                }
            NationalVolume(pFI, (USHORT)bTemp,(UCHAR)((P_channel&1)+NATIONAL_LEFT_VOL_REG)); // see Pas-1 spec p.15 (LEFT VOLUME CONTROL)
            }
        }

}

    /*\
;---|*|--------====< void SetEQ (line, EQ, level ) >====--------
;---|*|
;---|*|       This routine outputs a new setting for a volume channel.
;---|*|
;---|*| Entry Conditions:
;---|*|       WParm1 is line number (range 0 - 1; 1 is don't care)
;---|*|       WParm2 is EQ type (ie. Loudness, Stereo Enhance, BMT)
;---|*|       WParm3 is level (range 0-31)
;---|*|
;---|*| Exit Conditions:
;---|*|       None
;---|*|
;   \*/

void
SetEq(PFOUNDINFO pFI, USHORT P_output, USHORT P_EQtype,USHORT  P_level)
{
    UCHAR bTemp;

    if (P_output!=0)
        return;

    if (pFI->Caps.CapsBits.Mixer_508)
        {
        switch (P_EQtype)
            {
            case _BASS:
                PASX_OUT(pFI, MIXER_508_REG,MV_508_ADDRESS + MV_508_BASS);
                PASX_OUT(pFI, MIXER_508_REG, BassTreb32to13[P_level]);
                break;

            case _TREBLE:
                PASX_OUT(pFI, MIXER_508_REG,MV_508_ADDRESS + MV_508_TREBLE);
                PASX_OUT(pFI, MIXER_508_REG, BassTreb32to13[P_level]);
                break;
            }
        }
    else    // NATIONAL (SERIAL PROGRAMMED) MIXER
        {
        bTemp=Scale32To12[P_level];
        NationalVolume(pFI, (USHORT)bTemp,(UCHAR)((P_EQtype&1)+NATIONAL_BASS_REG) );  // see Pas-1 spec p.15 (LEFT VOLUME CONTROL)
        }
}


    /*\
;---|*|--------====< void SetEqMode (Loudness, Enhance ) >====--------
;---|*|
;---|*|       This routine sets loundess and stereo enhance modes
;---|*|
;---|*| Entry Conditions:
;---|*|       loudness  (Z vs NZ)
;---|*|       Stereo Enhance (range 0-3)
;---|*|
;---|*| Exit Conditions:
;---|*|       None
;---|*|
;   \*/

void
SetEqMode(PFOUNDINFO pFI, USHORT P_loudness, USHORT P_enhance)
{
    UCHAR   bTemp=0;

    if (pFI->Caps.CapsBits.Mixer_508)
        {
        PASX_OUT(pFI, MIXER_508_REG,MV_508_ADDRESS + MV_508_EQMODE);
        if (P_loudness)
            bTemp+=MV_508_LOUDNESS;

        bTemp+=P_enhance & MV_508_ENHANCE;
        PASX_OUT(pFI, MIXER_508_REG,bTemp);
        }
    else
        {
        if (P_loudness)
            bTemp=NATIONAL_LOUDNESS;        // Loudness bit

        if (P_enhance)
        bTemp|=NATIONAL_ENHANCE;        // stereo enhance bit

        NationalVolume(pFI, bTemp,NATIONAL_LOUD_ENH_REG);    // see Pas-1 spec p.15 (LEFT VOLUME CONTROL)
        }
}

 /* ----------------------====< NationalMix >====-------------------------- */
    /*\                                                                         |
;---|*| NationalMix -- Load The National Mixer                                  |
;---|*|                                                                         |
;---|*| Entry Conditions                                                            |
;---|*| wData = index/data                                                          |
;---|*|                                                                         |
;   \*/
void
NationalMix(PFOUNDINFO pFI, USHORT wData)
{
    USHORT  i,bTemp=0;

    //  EnterCrit

    bTemp = audiomixr;                           // get current hardware state
    // bTemp &= SERIAL_MIX_REALSOUND+SERIAL_MIX_DUALFM;  // save state of only these bits
    bTemp|=~(SERIAL_MIX_REALSOUND+SERIAL_MIX_DUALFM); // turn on all other bits
                                                      // all clocks and strobes should be 1
    PASX_OUT(pFI, SERIAL_MIXER,bTemp);
    KeStallExecutionProcessor(5);       // wait 5 us

    for (i=0; i<8; i++) {

        // output clock is 0
        PASX_OUT(pFI, SERIAL_MIXER,bTemp & (~SERIAL_MIX_CLOCK));
        KeStallExecutionProcessor(5);       // wait 5 us

        PASX_OUT(pFI, SERIAL_MIXER,(wData>>i)& 1);   //send data, clock is 0
        KeStallExecutionProcessor(5);       // wait 5 us

        PASX_OUT(pFI, SERIAL_MIXER,
            ((wData>>i)& 1)^SERIAL_MIX_CLOCK);  //send data, clock is 1
        KeStallExecutionProcessor(5);       // wait 5 us
    }

    PASX_OUT(pFI, SERIAL_MIXER,
        ((wData>>i)& 1)|SERIAL_MIX_CLOCK|SERIAL_MIX_STROBE); //strobe it in
    KeStallExecutionProcessor(5);       // wait 5 us

    PASX_OUT(pFI, SERIAL_MIXER,bTemp);
    audiomixr=bTemp;                   // save the last state
    KeStallExecutionProcessor(5);       // wait 5 us

}
 /* ---;--------------------====< NationalVolume >====------------------------------- */
    /*\;
;---|*|; NationalVolume -- Load Volume control Register
;---|*|;
;---|*|; Entry Conditions:
;---|*|;    bl = parameter register (volume control channel 0-7)
;---|*|;    ah = data to transfer   (new channel setting)
;   \*/

void
NationalVolume(PFOUNDINFO pFI, USHORT wVolume, UCHAR wLeftRight)
{
//
// pass everything, but left/right volume directly to the device
//
// left & right volume are presented
// to the logical level as 0 - 40,
// where 0 is the lowest, and 40 is
// the highest. In reality, this
// is backwards, that is, 40 is the
// lowest, and 0 is the highest. We will
// correct the value here...

    USHORT  bTemp,i;
    short   sVolume=wVolume;

// Perform the volume control output

if (( wLeftRight==NATIONAL_LEFT_VOL_REG) ||
     (wLeftRight==NATIONAL_RIGHT_VOL_REG ))
    {
    sVolume -=0x40;     // is backwards, that is, 40 is the
    sVolume = -sVolume; // lowest, and 0 is the highest. We will
                        // correct the value here...
    }
//
// all 1s but volume enable and clock
//
    bTemp=audiomixr;           // save the realsound & dual fm bits
    bTemp &= SERIAL_MIX_REALSOUND+SERIAL_MIX_DUALFM;
    bTemp |= ~ (SERIAL_MIX_REALSOUND+SERIAL_MIX_DUALFM+SERIAL_MIX_MASTER+SERIAL_MIX_CLOCK);

    // EnterCrit
    PASX_OUT(pFI, SERIAL_MIXER,bTemp);       // note: clock is off

    for (i=0; i<8; i++) {

        bTemp=(wLeftRight>>i)& 1;
        PASX_OUT(pFI, SERIAL_MIXER,(bTemp | SERIAL_MIX_CLOCK)); // clock's on
        KeStallExecutionProcessor(5);       // wait 5 us

        PASX_OUT(pFI, SERIAL_MIXER,(bTemp));                       // clock's off
        KeStallExecutionProcessor(5);       // wait 5 us
    }

    // write with volume control enable
    // which starts data loading
    PASX_OUT(pFI, SERIAL_MIXER,bTemp|SERIAL_MIX_MASTER);
    KeStallExecutionProcessor(6);       // wait 6 us

    for (i=0; i<8; i++) {

        bTemp=bTemp & (~D0);            // mask off D0
        bTemp |= ((sVolume>>i) & D0);       // move current bit into D0

        PASX_OUT(pFI, SERIAL_MIXER,bTemp+SERIAL_MIX_CLOCK);
        KeStallExecutionProcessor(6);       // wait 6 us

        PASX_OUT(pFI, SERIAL_MIXER,bTemp);
        KeStallExecutionProcessor(6);       // wait 5 us
    }

    for (i=0; i<12; i++) {

        PASX_OUT(pFI, SERIAL_MIXER,bTemp);
        KeStallExecutionProcessor(6);       // wait 6 us
    }

    // toggle volume control enable

    PASX_OUT(pFI, SERIAL_MIXER,bTemp|SERIAL_MIX_MASTER);
    KeStallExecutionProcessor(6);       // wait 6 us

    PASX_OUT(pFI, SERIAL_MIXER,bTemp|SERIAL_MIX_MASTER);
    KeStallExecutionProcessor(6);       // wait 6 us

    PASX_OUT(pFI, SERIAL_MIXER,bTemp|SERIAL_MIX_MASTER);
    KeStallExecutionProcessor(6);       // wait 6 us

    PASX_OUT(pFI, SERIAL_MIXER,bTemp);
    KeStallExecutionProcessor(6);       // wait 6 us

    audiomixr=bTemp;               // save the last state
}

    /*\
;---|*|--------------====< void SetFilter (int setting ) >===---------------
;---|*|
;---|*|       This routine selects a filter setting from mute to high freq filter.
;---|*|
;---|*| Entry Conditions:
;---|*|       WParm1 is a value from 0 - 6
;---|*|
;---|*| Exit Conditions:
;---|*|       None
;---|*|
;---|*|
;   \*/

void
SetFilter(PFOUNDINFO pFI, USHORT wSetting)
{
    USHORT wTemp;

    if (wSetting <= FILTERMAX) {

        wSetting=FilterTable[wSetting];

        wTemp = audiofilt;                     // get current bits
        wTemp &= ~(fFIdatabits+fFImutebits);   // save everthing but

        wSetting|=wTemp;

        //  EnterCrit
        PASX_OUT(pFI, FILTER_REGISTER, wSetting);
        KeStallExecutionProcessor(6);           // wait 6 us

        audiofilt=wSetting;                    // set current bits
    }

}
#if 0
ULONG
Scale100ToFFFF(WORD wVal)
{
    ULONG dwScaled;

    dwScaled=((ULONG)wVal) * (0xFFFFL/100L);
    dwScaled=(dwScaled & 0xff00L)+ (dwScaled >>8);

    return(dwScaled);
}

UCHAR
ScaleFFFFTo100(WORD wVal)
{
    UCHAR bScaled;

    bScaled=((wVal>>8)*100)/255;

    return(bScaled);
}
#endif

