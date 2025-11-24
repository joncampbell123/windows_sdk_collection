//---------------------------------------------------------------------------
//
//  Module:   driver.h
//
//  Description:
//     Header file for global driver declarations
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

#define Not_VxD
#define WANT_MSOPL
#include <vxdpipe.h>

#ifndef SETUPX_INC
#include <setupx.h>
#endif

//
// BCODE is a macro to define a R/O variable in the code segment
//

#define BCODE _based(_segname("_CODE"))

#define MSSNDSYS_VERSION_RQD    0x0400  // rqd version of MSSNDSYS.VXD

//
// MSSNDSYS API definitions
//

#define MSS_API_Get_Version         0
#define MSS_API_Get_Info            1
#define MSS_API_GetInfoF_DevNode    0001
#define MSS_API_Acquire             2

//
// Flags for acquiring CODEC and OPL3
//

#define MSS_ASS_ACQUIRE_CODEC       0x0001
#define MSS_ASS_ACQUIRE_OPL3        0x0002

#define MSS_API_Release             3
#define MSS_API_Copy_To_Buffer      4
#define MSS_API_Copy_From_Buffer    5

#define MSS_API_Get_DMA_Count       6
#define sfSS_GDC_Invalid            0
#define sfSS_GDC_DAC_Count          1
#define sfSS_GDC_ADC_Count          2

// who is using the interrupt

#define INT_FREE                0       // This MUST be defined as 0
#define INT_WAVEOUT             1
#define INT_WAVEIN              2

//
// wid and wod state flags
//

#define WIF_ALLOCATED   0x0001
#define WIF_STARTED     0x0002
#define WIF_SUSPENDED   0x0004
#define WIF_RESTART     0x0008

#define WOF_ALLOCATED   0x0001
#define WOF_SUSPENDED   0x0004
#define WOF_RESTART     0x0008

// Supported formats

#define FORMAT_8BIT             (0x00)
#define FORMAT_MULAW            (0x01)
#define FORMAT_16BIT            (0x02)
#define FORMAT_ALAW             (0x03)
#define FORMAT_IMA_ADPCM        (0x05)
#define WOFORMAT_STEREO         (0x80)

// Adress of DMA channel registers

#define DMA8ADR                 (0x00)
#define DMA8CNT                 (0x01)
#define DMA8STAT                (0x08)
#define DMA8SMR                 (0x0a)
#define DMA8MOD                 (0x0b)
#define DMA8CLR                 (0x0c)

#define DMA16ADR                (0xc0)
#define DMA16CNT                (0xc2)
#define DMA16STAT               (0xd0)
#define DMA16SMR                (0xd4)
#define DMA16MOD                (0xd6)
#define DMA16CLR                (0xd8)

// PIC register information

#define PIC_MASTER_OCW_REG      0x20
#define PIC_MASTER_MASK_REG     0x21
#define PIC_SLAVE_OCW_REG       0xA0
#define PIC_SLAVE_MASK_REG      0xA1

#define PIC_NONSPEC_EOI         0x20
#define PIC_SPECIFIC_EOI        0x60
#define PIC_SPECIFIC_EOI_SLAVE  0x62

#define PIC_IRR_NEXT            0x0A
#define PIC_ISR_NEXT            0x0B
#define PIC_POLL_MODE           0x0C

#define PIC_POLL_IRQ_FIRED      0x80
#define PIC_POLL_IRQ_MASK       0x07

#define CODEC_ADDRESS           (0x00)
#define CODEC_DATA              (0x01)
#define CODEC_STATUS            (0x02)
#define CODEC_DIRECT            (0x03)

#define CODEC_IS_BUSY           (0x80)

#define REGISTER_LEFTINPUT      (0x00)
#define REGISTER_RIGHTINPUT     (0x01)
#define REGISTER_LEFTAUX1       (0x02)
#define REGISTER_RIGHTAUX1      (0x03)
#define REGISTER_LEFTAUX2       (0x04)
#define REGISTER_RIGHTAUX2      (0x05)
#define REGISTER_LEFTOUTPUT     (0x06)
#define REGISTER_RIGHTOUTPUT    (0x07)
#define REGISTER_DATAFORMAT     (0x08)
#define REGISTER_INTERFACE      (0x09)
#define REGISTER_DSP            (0x0a)
#define REGISTER_TEST           (0x0b)
#define REGISTER_MISC           (0x0c)
#define REGISTER_LOOPBACK       (0x0d)
#define REGISTER_UPPERBASE      (0x0e)
#define REGISTER_LOWERBASE      (0x0f)
#ifdef AZTECH
    #define REGISTER_LEFTLINEINPUT  (0x12)
    #define REGISTER_RIGHTLINEINPUT (0x13)
    #define REGISTER_MONOINPUT      (0x1A)
#endif

// CS4231 specific registers

#define REGISTER_CAP_DATAFORMAT (0x1C)
#define REGISTER_CAP_UPPERBASE  (0x1E)
#define REGISTER_CAP_LOWERBASE  (0x1F)

#define AD1848_CONFIG_PEN       0x01            // Playback Enable
#define AD1848_CONFIG_CEN       0x02            // Capture Enable
#define AD1848_CONFIG_SDC       0x04            // Single DMA Channel
#define AD1848_CONFIG_ACAL      0x08            // Auto-Calibrate
#define AD1848_CONFIG_RESERVED  0x30            // Reserved
#define AD1848_CONFIG_PPIO      0x40            // Playback PIO Enable
#define AD1848_CONFIG_CPIO      0x80            // Capture PIO Enable

#define AD1848_MODE_TRD         0x20            // transfer request disable
#define AD1848_MODE_MCE         0x40            // mode change enable

#define CS4231_MISC_MODE2       0x40            // MODE 2 select/detect

#define VER_AD1848J             0x09            // version marker of CODEC
#define VER_AD1848K             0x0A

#define VER_CSPROTO             0x81            // prototype, treat as 'J'
#define VER_CS4248              0x8A

#define CODEC_J_CLASS           0x00            // AD1848J
#define CODEC_K_CLASS           0x01            // AD1848K & CS4248
#define CODEC_KPLUS_CLASS       0x02            // CS4231

// AGA offsets (Compaq's Business Audio)

#define AGA_PCR                 0x00            // peripheral config reg.
#define AGA_OSCR                0x01            // option slot config reg.
#define AGA_PRMR                0x03            // PBIC resource mapping reg.

#define DIRECTION_DAC           (0x00)
#define DIRECTION_ADC           (0x01)

#define DMA_BUFFER_DONE         0
#define DMA_BUFFER_PING         1
#define DMA_BUFFER_PONG         2

//===========================================================================
//                   Bit fields used for phwi.wHardwareOptions
//===========================================================================

//
//                      Hardware
//                      Options
//
//          +---+---+---+---+---+---+---+---+
//  byte 0  | 0 | 0 | 0 | 0 | X | X | X | X |
//          +---+---+---+---+---+---+---+---+
//
//                                        ^
//                                        |
//                                    ^   +--------- MSFT Hardware?
//                                    |
//                                ^   +------------- MSFT Support?
//                                |
//                             ^  +----------------- Auto Select/Detect?
//                             |
//                             +-------------------- FM synth on board?
//
//          +---+---+---+---+---+---+---+---+
//  byte 1  | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |        RESERVED!
//          +---+---+---+---+---+---+---+---+

#define DAK_MSFTHARDWARE   0x0001      // obsolete
#define DAK_MSFTSUPPORT    0x0002      // obsolete
#define DAK_AUTOSELECT     0x0004      // PAL? (Auto-Options?)
#define DAK_FMSYNTH        0x0008      // synth on board?
#define DAK_COMPAQBA       0x0010      // Compaq Business Audio detected
#define DAK_COMPAQI        0x0020      // Compaq BA without AGA -
                                       //   requires COMPAQBA flag.
#define DAK_DUALDMA        0x0040      // Set when using sim. rec/play

//
// indices into string table
//

#define SR_ALERT                1
#define SR_ALERT_31             2
#define SR_ALERT_NOPATCH        10
#define SR_ALERT_NORESOURCE     11
#define SR_ALERT_MUTED          12

#define SR_STR_DRIVERWAVEIN     13
#define SR_STR_DRIVERWAVEOUT    14
#define SR_STR_DRIVERMIDIOUT    15
#define SR_STR_DRIVERAUX        16
#define SR_STR_DRIVERMIXER      17
#define SR_STR_VOLUME           18

#define SR_ALERT_BAD                19
#define SR_ALERT_BOARD_ERROR        20
#define SR_ALERT_NO_VXD             21
#define SR_ALERT_WRONG_VXD          22

#ifdef MSSNDSYS

#define SR_STR_MIXER_SOURCE0_LONG   40
#define SR_STR_MIXER_SOURCE1_LONG   41
#define SR_STR_MIXER_SOURCE2_LONG   42
#define SR_STR_MIXER_SOURCE3_LONG   43
#define SR_STR_MIXER_SOURCE0_SHORT  44
#define SR_STR_MIXER_SOURCE1_SHORT  45
#define SR_STR_MIXER_SOURCE2_SHORT  46
#define SR_STR_MIXER_SOURCE3_SHORT  47
#define SR_STR_MIXER_DEST0_LONG     48
#define SR_STR_MIXER_DEST1_LONG     49
#define SR_STR_MIXER_DEST2_LONG     50
#define SR_STR_MIXER_DEST0_SHORT    51
#define SR_STR_MIXER_DEST1_SHORT    52
#define SR_STR_MIXER_DEST2_SHORT    53

#define SR_STR_MIXER_CONTROL0_LONG    54
#define SR_STR_MIXER_CONTROL1_LONG    55
#define SR_STR_MIXER_CONTROL2_LONG    56
#define SR_STR_MIXER_CONTROL3_LONG    57
#define SR_STR_MIXER_CONTROL4_LONG    58
#define SR_STR_MIXER_CONTROL5_LONG    59
#define SR_STR_MIXER_CONTROL6_LONG    60
#define SR_STR_MIXER_CONTROL7_LONG    61
#define SR_STR_MIXER_CONTROL8_LONG    62
#define SR_STR_MIXER_CONTROL9_LONG    63
#define SR_STR_MIXER_CONTROL10_LONG   64
#define SR_STR_MIXER_CONTROL11_LONG   65
#define SR_STR_MIXER_CONTROL12_LONG   66
#define SR_STR_MIXER_CONTROL13_LONG   67
#define SR_STR_MIXER_CONTROL14_LONG   68
#define SR_STR_MIXER_CONTROL15_LONG   69
#define SR_STR_MIXER_CONTROL16_LONG   70
#define SR_STR_MIXER_CONTROL17_LONG   71
#define SR_STR_MIXER_CONTROL18_LONG   72

#define SR_STR_MIXER_CONTROL0_SHORT   73
#define SR_STR_MIXER_CONTROL1_SHORT   74
#define SR_STR_MIXER_CONTROL2_SHORT   75
#define SR_STR_MIXER_CONTROL3_SHORT   76
#define SR_STR_MIXER_CONTROL4_SHORT   77
#define SR_STR_MIXER_CONTROL5_SHORT   78
#define SR_STR_MIXER_CONTROL6_SHORT   79
#define SR_STR_MIXER_CONTROL7_SHORT   80
#define SR_STR_MIXER_CONTROL8_SHORT   81
#define SR_STR_MIXER_CONTROL9_SHORT   82
#define SR_STR_MIXER_CONTROL10_SHORT  83
#define SR_STR_MIXER_CONTROL11_SHORT  84
#define SR_STR_MIXER_CONTROL12_SHORT  85
#define SR_STR_MIXER_CONTROL13_SHORT  86
#define SR_STR_MIXER_CONTROL14_SHORT  87
#define SR_STR_MIXER_CONTROL15_SHORT  88
#define SR_STR_MIXER_CONTROL16_SHORT  89
#define SR_STR_MIXER_CONTROL17_SHORT  90
#define SR_STR_MIXER_CONTROL18_SHORT  91

#endif

#ifdef WASHTON

#define SR_STR_MIXER_SOURCE0_LONG   50
#define SR_STR_MIXER_SOURCE1_LONG   51
#define SR_STR_MIXER_SOURCE2_LONG   52
#define SR_STR_MIXER_SOURCE3_LONG   53
#define SR_STR_MIXER_SOURCE4_LONG   54
#define SR_STR_MIXER_SOURCE5_LONG   55
#define SR_STR_MIXER_SOURCE6_LONG   56

#define SR_STR_MIXER_SOURCE0_SHORT  60
#define SR_STR_MIXER_SOURCE1_SHORT  61
#define SR_STR_MIXER_SOURCE2_SHORT  62
#define SR_STR_MIXER_SOURCE3_SHORT  63
#define SR_STR_MIXER_SOURCE4_SHORT  64
#define SR_STR_MIXER_SOURCE5_SHORT  65
#define SR_STR_MIXER_SOURCE6_SHORT  66

#define SR_STR_MIXER_DEST0_LONG     70
#define SR_STR_MIXER_DEST1_LONG     71
#define SR_STR_MIXER_DEST2_LONG     72
#define SR_STR_MIXER_DEST0_SHORT    73
#define SR_STR_MIXER_DEST1_SHORT    74
#define SR_STR_MIXER_DEST2_SHORT    75

#define SR_STR_MIXER_CONTROL0_LONG    100
#define SR_STR_MIXER_CONTROL1_LONG    101
#define SR_STR_MIXER_CONTROL2_LONG    102
#define SR_STR_MIXER_CONTROL3_LONG    103
#define SR_STR_MIXER_CONTROL4_LONG    104
#define SR_STR_MIXER_CONTROL5_LONG    105
#define SR_STR_MIXER_CONTROL6_LONG    106
#define SR_STR_MIXER_CONTROL7_LONG    107
#define SR_STR_MIXER_CONTROL8_LONG    108
#define SR_STR_MIXER_CONTROL9_LONG    109
#define SR_STR_MIXER_CONTROL10_LONG   110
#define SR_STR_MIXER_CONTROL11_LONG   111
#define SR_STR_MIXER_CONTROL12_LONG   112
#define SR_STR_MIXER_CONTROL13_LONG   113
#define SR_STR_MIXER_CONTROL14_LONG   114
#define SR_STR_MIXER_CONTROL15_LONG   115
#define SR_STR_MIXER_CONTROL16_LONG   116
#define SR_STR_MIXER_CONTROL17_LONG   117
#define SR_STR_MIXER_CONTROL18_LONG   118
#define SR_STR_MIXER_CONTROL19_LONG   119
#define SR_STR_MIXER_CONTROL20_LONG   120
#define SR_STR_MIXER_CONTROL21_LONG   121
#define SR_STR_MIXER_CONTROL22_LONG   122
#define SR_STR_MIXER_CONTROL23_LONG   123
#define SR_STR_MIXER_CONTROL24_LONG   124
#define SR_STR_MIXER_CONTROL25_LONG   125
#define SR_STR_MIXER_CONTROL26_LONG   126

#define SR_STR_MIXER_CONTROL0_SHORT   200
#define SR_STR_MIXER_CONTROL1_SHORT   201
#define SR_STR_MIXER_CONTROL2_SHORT   202
#define SR_STR_MIXER_CONTROL3_SHORT   203
#define SR_STR_MIXER_CONTROL4_SHORT   204
#define SR_STR_MIXER_CONTROL5_SHORT   205
#define SR_STR_MIXER_CONTROL6_SHORT   206
#define SR_STR_MIXER_CONTROL7_SHORT   207
#define SR_STR_MIXER_CONTROL8_SHORT   208
#define SR_STR_MIXER_CONTROL9_SHORT   209
#define SR_STR_MIXER_CONTROL10_SHORT  210
#define SR_STR_MIXER_CONTROL11_SHORT  211
#define SR_STR_MIXER_CONTROL12_SHORT  212
#define SR_STR_MIXER_CONTROL13_SHORT  213
#define SR_STR_MIXER_CONTROL14_SHORT  214
#define SR_STR_MIXER_CONTROL15_SHORT  215
#define SR_STR_MIXER_CONTROL16_SHORT  216
#define SR_STR_MIXER_CONTROL17_SHORT  217
#define SR_STR_MIXER_CONTROL18_SHORT  218
#define SR_STR_MIXER_CONTROL19_SHORT  219
#define SR_STR_MIXER_CONTROL20_SHORT  220
#define SR_STR_MIXER_CONTROL21_SHORT  221
#define SR_STR_MIXER_CONTROL22_SHORT  222
#define SR_STR_MIXER_CONTROL23_SHORT  223
#define SR_STR_MIXER_CONTROL24_SHORT  224
#define SR_STR_MIXER_CONTROL25_SHORT  225
#define SR_STR_MIXER_CONTROL26_SHORT  226

#elif NOVA

#define SR_STR_MIXER_SOURCE0_LONG   50
#define SR_STR_MIXER_SOURCE1_LONG   51
#define SR_STR_MIXER_SOURCE2_LONG   52
#define SR_STR_MIXER_SOURCE3_LONG   53
#define SR_STR_MIXER_SOURCE4_LONG   54
#define SR_STR_MIXER_SOURCE5_LONG   55

#define SR_STR_MIXER_SOURCE0_SHORT  60
#define SR_STR_MIXER_SOURCE1_SHORT  61
#define SR_STR_MIXER_SOURCE2_SHORT  62
#define SR_STR_MIXER_SOURCE3_SHORT  63
#define SR_STR_MIXER_SOURCE4_SHORT  64
#define SR_STR_MIXER_SOURCE5_SHORT  65

#define SR_STR_MIXER_DEST0_LONG     70
#define SR_STR_MIXER_DEST1_LONG     71
#define SR_STR_MIXER_DEST2_LONG     72
#define SR_STR_MIXER_DEST0_SHORT    73
#define SR_STR_MIXER_DEST1_SHORT    74
#define SR_STR_MIXER_DEST2_SHORT    75

#define SR_STR_MIXER_CONTROL0_LONG    100
#define SR_STR_MIXER_CONTROL1_LONG    101
#define SR_STR_MIXER_CONTROL2_LONG    102
#define SR_STR_MIXER_CONTROL3_LONG    103
#define SR_STR_MIXER_CONTROL4_LONG    104
#define SR_STR_MIXER_CONTROL5_LONG    105
#define SR_STR_MIXER_CONTROL6_LONG    106
#define SR_STR_MIXER_CONTROL7_LONG    107
#define SR_STR_MIXER_CONTROL8_LONG    108
#define SR_STR_MIXER_CONTROL9_LONG    109
#define SR_STR_MIXER_CONTROL10_LONG   110
#define SR_STR_MIXER_CONTROL11_LONG   111
#define SR_STR_MIXER_CONTROL12_LONG   112
#define SR_STR_MIXER_CONTROL13_LONG   113
#define SR_STR_MIXER_CONTROL14_LONG   114
#define SR_STR_MIXER_CONTROL15_LONG   115
#define SR_STR_MIXER_CONTROL16_LONG   116
#define SR_STR_MIXER_CONTROL17_LONG   117
#define SR_STR_MIXER_CONTROL18_LONG   118
#define SR_STR_MIXER_CONTROL19_LONG   119
#define SR_STR_MIXER_CONTROL20_LONG   120
#define SR_STR_MIXER_CONTROL21_LONG   121
#define SR_STR_MIXER_CONTROL22_LONG   122

#define SR_STR_MIXER_CONTROL0_SHORT   200
#define SR_STR_MIXER_CONTROL1_SHORT   201
#define SR_STR_MIXER_CONTROL2_SHORT   202
#define SR_STR_MIXER_CONTROL3_SHORT   203
#define SR_STR_MIXER_CONTROL4_SHORT   204
#define SR_STR_MIXER_CONTROL5_SHORT   205
#define SR_STR_MIXER_CONTROL6_SHORT   206
#define SR_STR_MIXER_CONTROL7_SHORT   207
#define SR_STR_MIXER_CONTROL8_SHORT   208
#define SR_STR_MIXER_CONTROL9_SHORT   209
#define SR_STR_MIXER_CONTROL10_SHORT  210
#define SR_STR_MIXER_CONTROL11_SHORT  211
#define SR_STR_MIXER_CONTROL12_SHORT  212
#define SR_STR_MIXER_CONTROL13_SHORT  213
#define SR_STR_MIXER_CONTROL14_SHORT  214
#define SR_STR_MIXER_CONTROL15_SHORT  215
#define SR_STR_MIXER_CONTROL16_SHORT  216
#define SR_STR_MIXER_CONTROL17_SHORT  217
#define SR_STR_MIXER_CONTROL18_SHORT  218
#define SR_STR_MIXER_CONTROL19_SHORT  219
#define SR_STR_MIXER_CONTROL20_SHORT  220
#define SR_STR_MIXER_CONTROL21_SHORT  221
#define SR_STR_MIXER_CONTROL22_SHORT  222

#endif

#define IDS_MENUABOUT           32
#define IDS_VERSION             33


// Global error conditions

#define STATUS_NO_ERROR             0x0000
#define STATUS_CONFIG_BOARD_ERROR   0x0001
#define STATUS_CONFIG_NO_VXD        0x0002
#define STATUS_CONFIG_WRONG_VXDVER  0x0003

// Critical section definiton

#define CS_BEGIN                    0x0001
#define CS_END                      0x0002

// Max length definitions

#define MAXLEN_INIKEY             32

// Interrupt critical sections

#define ASCRITENTER         if (!(gwCritLevel++)) _asm { cli }
#define ASCRITLEAVE         if (!(--gwCritLevel)) _asm { sti }

//
// type definitions
//

// low priority storage

typedef struct tagLOWPRISAVE
{
   DWORD        dwUser ;                  // client's user handle
   BOOL         fwidStarted ;             // did client start wave-in?
   DWORD        dwWaveFormat ;            // client's wave format
   LPWAVEHDR    lpwidQueue ;              // pointer to wave-in queue
   WORD         wDMAHalfBufSize ;         // client's DMA buffer size
                                          //    (wave format dependant)

} LOWPRISAVE, NEAR *PLOWPRISAVE ;

typedef BYTE near * NPBYTE ;
typedef char huge * HPSTR ;

typedef VOID NEAR * PVOID ;

typedef struct tagHARDWAREINSTANCE
{

   // WARNING! DANGER! WARNING! DANGER! WARNING! DANGER! WARNING! DANGER!
   //
   // If you modify this structure be sure to reflect the changes in
   // the .ASM version found in DRIVER.INC.
   //
   // WARNING! DANGER! WARNING! DANGER! WARNING! DANGER! WARNING! DANGER!


   // hardware instance information from VxD

   WORD        wHardwareOptions ;
   WORD        wFlags ;                 // flags (bus-type etc.)
   WORD        wOEM_ID ;                // OEM ID for OEM specific extensions
   WORD        wIOAddressCODEC ;
   WORD        wIOAddressAGA ;
   WORD        wIOAddressOPL3 ;
   WORD        wSRAMBase ;
   BYTE        bIRQ ;
   BYTE        bPlaybackDMA ;
   BYTE        bCaptureDMA ;
   WORD        wCODECClass ;
   DWORD       lpDMABufferPhys ;
   DWORD       dwDMABufferLen ;
   WORD        wDMABufferSelector ;
   DWORD       dn ;                     // DevNode

   // hardware instance common

   WORD        cReference ;             // reference count
   WORD        cEnable ;                // enable count

   BOOL        fEnabled ;               // TRUE if instance enabled
   BOOL        fJustStart ;             // TRUE if just starting
   BOOL        fBadBoard ;              // TRUE if hardware error
   BOOL        fDo11kHzMCE ;            // TRUE if MCE after 11kHz or below
   BOOL        fSingleModeDMA ;         // TRUE if using single mode DMA
                                        // rather than demand mode DMA

   WORD        wLastDMAPos ;            // Last DMA position
   WORD        wSamplesPerSec ;         // Current samples per second
   BYTE        bMute ;                  // XTL or-ed with CODEC_REG_DSP
   BYTE        bModeChange ;            // or-ed with CODEC_ADDRESS
   BOOL        fwodDMABusy ;            // TRUE if DMA DAC active
   BOOL        fwidDMABusy ;            // TRUE if DMA ADC active
   LPVOID      pmi ;                    // mixer instance
   DWORD       dwWaitLoop ;             // time to wait for CODEC

   // acquire counts

   BYTE        bAcquireCountCODEC ;
   BYTE        bAcquireCountOPL3 ;

   // dma stuff

   BYTE        portCaptureDMAAdr, portCaptureDMACnt, portCaptureDMASMR,
               portCaptureDMAMod, portCaptureDMAClr, portCaptureDMAPReg,
               portCaptureDMAEnable, portCaptureDMADisable,
               portCaptureDMARead ;

   BYTE        portPlaybackDMAAdr, portPlaybackDMACnt, portPlaybackDMASMR,
               portPlaybackDMAMod, portPlaybackDMAClr, portPlaybackDMAPReg,
               portPlaybackDMAEnable, portPlaybackDMADisable,
               portPlaybackDMAWrite ;

   BOOL        fDMADone ;               // set when DMA is done
   WORD        wDMABufferLenMinusOne ;  // DMA buffer size - 1
   WORD        wDMAHalfBufSize ;        // current half buffer size
   WORD        wCODECSamples ;          // CODEC samples counter
   WORD        wBytesPerSample ;        // # of bytes per sample

   LPBYTE      lpDMABuffer ;            // ptr to DMA buffer

   WORD        wDMAPhysAddr ;           // DMA buffer physical address
   BYTE        bDMAPhysPage ;           // DMA buffer physical page

   // irq stuff

   UINT        uISRDataSel ;            // ISR data selector
   UINT        uISRCodeSel ;            //   & code selector

   BYTE        bIntVector ;
   BYTE        bOrigIntMask ;
   DWORD       dwOldISR ;               // old ISR
   WORD        wEOICommands ;

   BYTE        bIntUsed ;               // interrupt user (wod/wid)


   // wid/wod common

   BOOL        fAcceptCloseRates ;      // TRUE if accept rates (+/- 5%)
   DWORD       dwCurCODECOwner ;
   DWORD       dwWaveFormat ;           // channels & quantization
   WORD        wOldFormat ;             // set to last format...
   BYTE        bwidFlags ;
   BYTE        bwodFlags ;

   // wid storage

   BOOL        fwidStarted ;            // set to TRUE when wid started
   BOOL        fLowPriStateSaved ;
   DWORD       dwLowUser ;
   LOWPRISAVE  lps ;
   LPWAVEHDR   lpwidQueue ;             // wave output data buffer queue
   HPSTR       hpwidCurData ;           // pointer to data block of widQueue
   DWORD       dwwidCurCount ;          // bytes left in current wid block
   DWORD       dwwidLeftPeak ;
   DWORD       dwwidRightPeak ;

   // wod storage

   BOOL        fwodStarted ;            // set to TRUE when wod started
   LPSTR       lpSilenceStart ;         // where the padded silence starts
   WORD        wSilenceSize ;           // how big the padded silence is
   BOOL        fwodPaused ;             // has wave out been paused?
   LPWAVEHDR   lpwodQueue ;             // wave output data buffer queue
   LPWAVEHDR   lpwodDeadHeads ;         // headers ready to be signalled
   HPSTR       hpwodCurData ;           // pointer to data block of wodQueue
   DWORD       dwwodCurCount ;          // bytes left in current wod block
   LPWAVEHDR   lpwodLoopStart ;         // pointer to first block in wod loop
   DWORD       dwwodLoopCount ;         // count for current wod loop
   BOOL        fwodSkipThisLoop ;       // set to TRUE to skip the loop
   BOOL        fwodBreakLoop ;          // set to TRUE to break current loop
   DWORD       dwwodLeftPeak ;
   DWORD       dwwodRightPeak ;
   DWORD       dwInterruptTimeStamp;   // Time each int for waveOutGetPos
   DWORD       dwPauseTimeDelta;       // snapshot of clock when pasued
   DWORD       dwLastPosition ;

   // mixer storage

   HPIPE          hpmxd ;               // mixer pipe handle
   FNPIPECALLBACK fnmxdPipe ;           // mixer pipe proc

   BYTE        bDACToCODECLeft ;
   BYTE        bDACToCODECRight ;

   // link to next instance

   struct tagHARDWAREINSTANCE NEAR *pNext ;

   // WARNING! DANGER! WARNING! DANGER! WARNING! DANGER! WARNING! DANGER!
   //
   // If you modify this structure be sure to reflect the changes in
   // the .ASM version found in DRIVER.INC.
   //
   // WARNING! DANGER! WARNING! DANGER! WARNING! DANGER! WARNING! DANGER!

} HARDWAREINSTANCE, NEAR *PHARDWAREINSTANCE ;

#define SSI_FLAG_DISABLEWARNING     0x0001 // warnings disabled on contention
#define SSI_FLAG_SINGLEMODEDMA      0x0004 // for Symphony chip sets
#define SSI_FLAG_BUSTYPE_ISAPNP     0x0100 // ISAPNP enumerated
#define SSI_FLAG_BUSTYPE_PCMCIA     0x0200 // PCMCIA enumerated
#define SSI_FLAG_HWSB               0x1000 // Sound Blaster in HW
#define SSI_FLAG_SBACQUIRED         0x4000 // set when VM acquires virt SB
#define SSI_FLAG_IRQWASUNMASKED     0x8000 // irq being used was unmasked


// per allocation structure for wave

typedef struct wavealloc_tag
{
    PHARDWAREINSTANCE   phwi ;             // hardware instance information
    DWORD               dwCallback ;       // client's callback
    DWORD               dwInstance ;       // client's instance data
    HANDLE              hWave ;            // handle for stream
    DWORD               dwFlags ;          // allocation flags
    DWORD               dwByteCount ;      // byte count since last reset
    DWORD               dwHalfDMABuffers ; // num of DMA buffers completed
    PCMWAVEFORMAT       pcmwf ;            // format of wave data

} WAVEALLOC, NEAR *NPWAVEALLOC ;

typedef struct tagDRVDATA 
{
    LPDEVICE_INFO    lpdi ;
    LPCSTR           pszTemplate ;
    BOOL             fAcceptCloseRates ;
    BOOL             fSingleModeDMA ;
    BOOL             fSBEmulation ;
    BOOL             fDo11kHzMCE ;

} DRVDATA, NEAR* PDRVDATA, FAR* LPDRVDATA ;

//
// internal strings (in init.c):
//
//

#ifdef DEBUG
    extern char STR_PROLOGUE[];
    extern char STR_CRLF[];
    extern char STR_SPACE[];
#endif


//
// global declarations
//

// commona.asm:

extern LPVOID       glpVSNDSYSEntry ;

// init.c:

extern HMODULE      ghModule ;           // our module handle
extern WORD         gwGlobalStatus ;     // global status used during CONFIG

// hardware.c

extern WORD         gwCritLevel ;        // critical section counter

//
// function prototypes
//

// hardware.c

VOID FAR PASCAL HwSetFormat( PHARDWAREINSTANCE, DWORD, BYTE, BYTE, BYTE ) ;
WORD FAR PASCAL HwNearestRate( DWORD ) ;
VOID FAR PASCAL HwBeginADCDMA( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL HwEndADCDMA( PHARDWAREINSTANCE ) ;
LPWAVEHDR FAR PASCAL HwAddADCBlock( PHARDWAREINSTANCE, LPWAVEHDR, LPWAVEHDR ) ;
VOID FAR PASCAL HwBeginDACDMA( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL HwResumeDAC( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL HwPauseDAC( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL HwStopDACDMA( PHARDWAREINSTANCE ) ;
VOID HwAddDACBlock( PHARDWAREINSTANCE, LPWAVEHDR ) ;
LRESULT FAR PASCAL DrvInit( VOID ) ;
VOID FAR PASCAL DrvEnd( VOID );

BOOL FAR PASCAL MxdPeakMeter( PHARDWAREINSTANCE, DWORD, LPLONG ) ;
BOOL FAR PASCAL HwPeakMeter( PHARDWAREINSTANCE, LPBYTE, DWORD, LPLONG ) ;
WORD FAR PASCAL HwGetCurrentDMAPosition( PHARDWAREINSTANCE ) ;

// hardfix.c

VOID FAR PASCAL HwExtMute( PHARDWAREINSTANCE, WORD ) ;
BOOL FAR PASCAL CODEC_WaitForReady( PHARDWAREINSTANCE ) ;
WORD FAR PASCAL CODEC_RegRead( PHARDWAREINSTANCE, BYTE ) ;
BOOL FAR PASCAL CODEC_RegWrite( PHARDWAREINSTANCE, BYTE, BYTE ) ;
VOID FAR PASCAL HwEndDSPAndDMA( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL HwEnterMCE( PHARDWAREINSTANCE, BOOL ) ;
VOID FAR PASCAL HwLeaveMCE( PHARDWAREINSTANCE, BOOL ) ;

DWORD FAR PASCAL HwGetCurPos( PHARDWAREINSTANCE, LPWORD, DWORD ) ;
#define GCP_OPTIONF_DAC  0x0001
#define GCP_OPTIONF_ADC  0x0002

// auxil.c

VOID FAR PASCAL auxilGetDevCaps( PHARDWAREINSTANCE, MDEVICECAPSEX FAR * ) ;

// wave.c

DWORD NEAR PASCAL waveGetPos
(
    DWORD               dwUser,
    LPMMTIME            lpmmt,
    DWORD               dwSize,
    DWORD               fdwOptions
) ;

VOID FAR PASCAL waveBadBoardError( VOID ) ;
VOID FAR PASCAL widSendPartBuffer( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL wodGetDevCaps( PHARDWAREINSTANCE, MDEVICECAPSEX FAR * ) ;
VOID FAR PASCAL widGetDevCaps( PHARDWAREINSTANCE, MDEVICECAPSEX FAR * ) ;

#define WFS_FLAG_WAVJAMMER  0x0001

VOID FAR PASCAL waveFillSilence
(
    DWORD           dwFormat,
    LPSTR           lpBuf,
    WORD            wSize,
    DWORD           dwFlags,
    DWORD           dwParam
) ;

// commona.asm

WORD FAR PASCAL MSSNDSYS_Get_DMA_Count( DWORD, WORD ) ;
WORD FAR PASCAL HardwareAcquire( PHARDWAREINSTANCE, WORD, WORD ) ;
WORD FAR PASCAL HardwareRelease( PHARDWAREINSTANCE, WORD, WORD ) ;

WORD FAR PASCAL wodHardwareAcquire( PHARDWAREINSTANCE ) ;
WORD FAR PASCAL wodHardwareRelease( PHARDWAREINSTANCE ) ;
WORD FAR PASCAL widHardwareAcquire( PHARDWAREINSTANCE ) ;
WORD FAR PASCAL widHardwareRelease( PHARDWAREINSTANCE ) ;

LPVOID FAR PASCAL NMCMemCopySrc(LPVOID lpSrc, WORD wCount, WORD wSramDataReg );
LPVOID FAR PASCAL NMCMemCopyDest(LPVOID lpDst, WORD wCount, WORD wSramDataReg );
VOID FAR PASCAL NMCMemFillSilent(WORD wCount, WORD wValue, WORD wSramDataReg );
LPVOID FAR PASCAL MemCopySrc(LPVOID lpDst, LPVOID lpSrc, WORD wCount);
LPVOID FAR PASCAL MemCopyDest(LPVOID lpDst, LPVOID lpSrc, WORD wCount);
VOID FAR PASCAL MemFillSilent(LPVOID lpDst, WORD wCount, WORD wValue);

VOID FAR PASCAL ISR(VOID);

// assemu.asm

BOOL NEAR PASCAL GetVxDVersion( VOID ) ;
BOOL NEAR PASCAL GetVxDInfo( PHARDWAREINSTANCE, DWORD ) ;
WORD NEAR PASCAL GetVDSVersion( VOID ) ;

BOOL NEAR PASCAL Create_ISR( PHARDWAREINSTANCE ) ;
BOOL NEAR PASCAL SetInterruptMask( BYTE bIRQ, BYTE fMask ) ;
DWORD NEAR PASCAL SetInterruptVector( BYTE bIRQ, LPVOID lpNewISR ) ;

// drvproc.c

extern DWORD FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HANDLE hDriver, WORD wMessage, DWORD dwParam1, DWORD dwParam2);

// volume.c

extern WORD FAR PASCAL VolLogToLinear (WORD);
extern BYTE FAR PASCAL VolLinearToLog (WORD);

// wavefix.c

WORD NEAR PASCAL widFillBuffer( PHARDWAREINSTANCE, LPSTR, WORD );
WORD FAR PASCAL wodDMALoadBuffer( PHARDWAREINSTANCE, LPSTR, WORD, BOOL ) ;
VOID FAR PASCAL widBlockFinished( LPWAVEHDR ) ;
VOID FAR PASCAL wodBlockFinished( LPWAVEHDR ) ;
VOID FAR PASCAL wodPostAllHeaders( PHARDWAREINSTANCE ) ;
VOID FAR PASCAL waveCallBack( NPWAVEALLOC pWave, WORD msg, DWORD dw1 ) ;


// init.c

VOID FAR cdecl AlertBox(HWND hwnd, WORD wStrId, ...);
VOID FAR PASCAL DisplayConfigErrors( VOID ) ;

LRESULT FAR PASCAL DrvLoadVolumeFromReg
(
   PHARDWAREINSTANCE  phwi
) ;

LRESULT FAR PASCAL DrvSaveVolumeToReg
(
    PHARDWAREINSTANCE   phwi
) ;

int FAR PASCAL LibMain
(
    HMODULE         hModule,
    UINT            uDataSeg,
    UINT            uHeapSize,
    LPSTR           lpCmdLine
) ;

LRESULT FAR PASCAL AddDevNode( DWORD ) ;
LRESULT FAR PASCAL EnableDevNode( DWORD ) ;
LRESULT FAR PASCAL DisableDevNode( DWORD ) ;
LRESULT FAR PASCAL RemoveDevNode( DWORD ) ;
PHARDWAREINSTANCE FAR PASCAL DevNodeToHardwareInstance( DWORD ) ;
VOID FAR PASCAL PowerNotify_SuspendDevices() ;
VOID FAR PASCAL PowerNotify_ResumeDevices() ;

// proppage.c

VOID NEAR DrvLoadUserConfig
(
    DEVNODE         dn,
    LPDRVDATA       pDrvData
) ;

// wave.c

BOOL FAR PASCAL wodSuspend
(
    PHARDWAREINSTANCE   phwi
) ;

BOOL FAR PASCAL wodReactivate
(
    PHARDWAREINSTANCE   phwi
) ;

BOOL FAR PASCAL widSuspend
(
    PHARDWAREINSTANCE   phwi
) ;

BOOL FAR PASCAL widReactivate
(
    PHARDWAREINSTANCE   phwi
) ;

extern DWORD FAR  PASCAL _loadds widMessage
(
    UINT            uDevId,
    WORD            msg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

extern DWORD FAR  PASCAL _loadds wodMessage
(
    UINT            uDevId,
    WORD            msg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

// mixer.c

extern DWORD _far _pascal _export _loadds mxdMessage( UINT uDevId, UINT uMsg,
       DWORD dwUser, DWORD dwParam1, DWORD dwParam2);

LRESULT FAR PASCAL _loadds mixerPipeProc
(
    HPIPE           hp,
    DWORD           dwMsg,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

//
// macros
//

#define WRITE_PORT_UCHAR( x, y )  outp( x, y )
#define READ_PORT_UCHAR( x )      inp( x )
#define WRITEPORT( x, y )         outp( phwi -> wIOAddressCODEC+(x), (y) )
#define READPORT( x )             inp( (WORD)(phwi -> wIOAddressCODEC+(x)) )

//
// Debug output
//

#ifdef DEBUG
    extern WORD  wDebugLevel;     // debug level
    #define D( x )          { x }
    #define DPF( x, y ) if (x <= wDebugLevel) (OutputDebugStr(STR_PROLOGUE),OutputDebugStr(y),OutputDebugStr(STR_CRLF))
#else
    #define D( x )
    #define DPF( x, y )
#endif

#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") : " str

//---------------------------------------------------------------------------
//  End of File: driver.h
//---------------------------------------------------------------------------

