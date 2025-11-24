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
//  Module: msopl.h
//
//  Purpose: Header file for MSOPL.DRV
//---------------------------------------------------------------------------

#define Not_VxD
#define WANT_MSOPL
#include <vxdpipe.h>

//
// BCODE is a macro to define a R/O variable in the code segment
//

#define BCODE _based(_segname("_CODE"))

#define MSOPL_VERSION_RQD              0x0400

// VxD PM API interface...

#define MSOPL_API_Get_Version          0x0100
#define MSOPL_API_Get_Info             0x0101
#define MSOPL_API_GetInfoF_DevNode     0x0001

#define MSOPL_API_Acquire              0x0102
#define MSOPL_API_Release              0x0103

#define SYSEX_ERROR             0xFF    // internal error code for
                                        // sysexes on input

//
// macros
//

#define WRITE_PORT_UCHAR( x, y )  outp( x, y )
#define READ_PORT_UCHAR( x )      inp( x )

#define WRITEPORT( x, y )         outp( phwi -> wIOAddressSynth+(x), (y) )
#define READPORT( x )             inp( (WORD)(phwi -> wIOAddressSynth+(x)) )

// Global error conditions

#define STATUS_NO_ERROR             0x0000
#define STATUS_CONFIG_LOAD_ERROR    0x0001
#define STATUS_CONFIG_NO_VXD        0x0002

// Critical section definiton

#define CS_BEGIN                    0x0001
#define CS_END                      0x0002

// Critical section macros

#define ASCRITENTER         if (!(gwCritLevel++)) _asm { cli }
#define ASCRITLEAVE         if (!(--gwCritLevel)) _asm { sti }

#define NUMPATCHES                      (256)
#define NUMCHANNELS                     (16)
#define DRUMCHANNEL                     (9)     /* midi channel 10 */
#define RIFF_PATCH                      (mmioFOURCC('P','t','c','h'))
#define RIFF_FM4                        (mmioFOURCC('f','m','4',' '))
#define NUM2VOICES                      18

// indexed FM registers

#define AD_LSI                          (0x000)
#define AD_LSI2                         (0x101)
#define AD_TIMER1                       (0x001)
#define AD_TIMER2                       (0x002)
#define AD_MASK                         (0x004)
#define AD_CONNECTION                   (0x104)
#define AD_NEW                          (0x105)
#define AD_NTS                          (0x008)
#define AD_MULT                         (0x020)
#define AD_MULT2                        (0x120)
#define AD_LEVEL                        (0x040)
#define AD_LEVEL2                       (0x140)
#define AD_AD                           (0x060)
#define AD_AD2                          (0x160)
#define AD_SR                           (0x080)
#define AD_SR2                          (0x180)
#define AD_FNUMBER                      (0x0a0)
#define AD_FNUMBER2                     (0x1a0)
#define AD_BLOCK                        (0x0b0)
#define AD_BLOCK2                       (0x1b0)
#define AD_DRUM                         (0x0bd)
#define AD_FEEDBACK                     (0x0c0)
#define AD_FEEDBACK2                    (0x1c0)
#define AD_WAVE                         (0x0e0)
#define AD_WAVE2                        (0x1e0)

#define SILENCE          (192)

// Tuning information

#define FSAMP            (50000.0)

// x is the desired frequency, == FNUM at b = 1

#define PITCH(x)         ((DWORD)((x) * (double) (1L << 19) / FSAMP))


#define EQUAL            (1.059463094359)
#ifdef EUROPE
   #define  A            (442.0)
#else
   #define  A            (440.0)
#endif
#define ASHARP           (A * EQUAL)
#define B                (ASHARP * EQUAL)
#define C                (B * EQUAL / 2.0)
#define CSHARP           (C * EQUAL)
#define DNOTE            (CSHARP * EQUAL)
#define DSHARP           (DNOTE * EQUAL)
#define E                (DSHARP * EQUAL)
#define F                (E * EQUAL)
#define FSHARP           (F * EQUAL)
#define G                (FSHARP * EQUAL)
#define GSHARP           (G * EQUAL)

//
// type definitions
//

typedef struct tagVOICE
{
   BYTE   bNote ;              // note played
   BYTE   bChannel ;           // channel played
   BYTE   bPatch ;             // what patch is the note,
                               //    drums patch = drum note + 128
   BYTE   bOn ;                // TRUE if note is on, FALSE if off
   BYTE   bVelocity ;          // velocity
   BYTE   bReserved ;          // filler
   DWORD  dwTime ;             // time that was turned on/off;
                               //    0 time indicates that its not in use
   DWORD  dwOrigPitch[ 2 ] ;   // original pitch, for pitch bend
   BYTE   bBlock[ 2 ] ;        // value sent to the block

} VOICE ;

typedef struct tagHARDWAREINSTANCE
{
   //
   // Supporting VxD entry point
   //

   LPVOID      pVxDEntry ;
   DWORD       cAcquire ;

   // hardware instance information from VxD

   WORD        wHardwareOptions ;            // hardware options
   WORD        wIOAddressSynth ;             // base I/O
   DWORD       dn ;                          // DevNode

   //
   // other support data
   //

   WORD        cReference ;                  // reference count
   WORD        cEnable ;

   BOOL        fEnabled ;                    // TRUE if instance enabled
   BOOL        fInUse ;                      // TRUE If currently in use

   UINT        auBend[ NUMCHANNELS ] ;       // bend for each channel
   BYTE        abPatch[ NUMCHANNELS ] ;      // patch number for channel

   // voices...

   VOICE       aVoiceSlots[ NUM2VOICES ] ;   // voice usage

   // channel volume information

   BYTE        abChanAttens[ NUMCHANNELS ] ;  // attenuation of each channel
   BYTE        abStereoMasks[ NUMCHANNELS ] ; // mask for left/right

   // software attenuation control values

   DWORD       dwMasterVolume ;
   DWORD       dwSynthVolume ;

   BOOL        fSoftwareVolumeEnabled ;       // set if using software
                                              //    volume control
               
   WORD        wSynthAttenL ;
   WORD        wSynthAttenR ;

   // mixer pipe callback proc

   HPIPE          hpmxd ;
   FNPIPECALLBACK fnmxdPipe ;

   // link to next instance

   struct tagHARDWAREINSTANCE NEAR *pNext ;

} HARDWAREINSTANCE, NEAR *PHARDWAREINSTANCE ;

typedef struct tagMSOPLINFO
{
   DWORD   dwSize ;
   WORD    wHardwareOptions ;
   WORD    wIOAddressSynth ;
   WORD    wVersionVxD ;
   WORD    wFlags ;
   DWORD   dn ;
   DWORD   dwSynthOwnerCur ;
   DWORD   dwSynthOwnerLast ;
   DWORD   hOPLStubs ;

} MSOPLINFO, FAR *LPMSOPLINFO ;

#define MSOPL_HWOPTIONSF_OPL3DETECTED  0x0001

// per allocation structure for midi

typedef struct tagFMALLOC
{
    DWORD               dwCallback;     // client's callback
    DWORD               dwInstance;     // client's instance data
    HMIDIOUT            hMidi;          // handle for stream
    DWORD               dwFlags;        // allocation flags
    PHARDWAREINSTANCE   phwi ;

} FMALLOC, NEAR *PFMALLOC ;

/****************************************************************************

       strings - all strings can be found in initc.c

 ***************************************************************************/

#ifdef DEBUG
    extern char STR_PROLOGUE[];
    extern char STR_CRLF[];
    extern char STR_SPACE[];
#endif

/****************************************************************************

       globals

 ***************************************************************************/

// init.c

extern HMODULE  ghModule ;          // our module handle
extern char     gszPatchLib[] ;
extern WORD     gwGlobalStatus ;    // global status used during CONFIG

//
// function prototypes
//

LRESULT FAR  PASCAL _loadds modMessage
(
    UINT            uDevID,
    UINT            msg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

// vxdiface.c:

BOOL NEAR PASCAL GetVxDInfo
(
    PHARDWAREINSTANCE   phwi,
    DWORD               dn
) ;

LPVOID NEAR PASCAL GetVxDEntry
(
    UINT            uVxDId
) ;


WORD NEAR PASCAL GetVxDVersion
(
    LPVOID          pVxDEntry
) ;

BOOL NEAR PASCAL AcquireFM
(
    PHARDWAREINSTANCE   phwi
) ;

BOOL NEAR PASCAL ReleaseFM
(
    PHARDWAREINSTANCE   phwi
) ;

HPIPE pipeOpen
(
    PHARDWAREINSTANCE   phwi,
    LPSTR               psz,
    PPIPEOPENSTRUCT     pos
) ;

VOID pipeClose
(
    PHARDWAREINSTANCE   phwi,
    HPIPE               hp
) ;

// drvproc.c:

LRESULT FAR PASCAL _loadds DriverProc
(
    DWORD           dwDriverID,
    HDRVR           hDriver,
    UINT            uMessage,
    LPARAM          lParam1,
    LPARAM          lParam2
) ;

// init.c

LRESULT FAR PASCAL AddDevNode( DWORD ) ;
LRESULT FAR PASCAL EnableDevNode( DWORD, UINT ) ;
LRESULT FAR PASCAL DisableDevNode( DWORD ) ;
LRESULT FAR PASCAL RemoveDevNode( DWORD ) ;
PHARDWAREINSTANCE FAR PASCAL DevNodeToHardwareInstance( DWORD ) ;
LRESULT FAR PASCAL DrvInit( VOID ) ;
VOID FAR PASCAL DrvEnd( VOID ) ;

int FAR PASCAL LibMain
(
    HMODULE         hModule,
    UINT            uDataSeg,
    UINT            uHeapSize,
    LPSTR           lpCmdLine
) ;

// fmsynth.c

VOID FAR PASCAL fmNewVolume
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bChannel
) ;

VOID NEAR PASCAL fmMidiMessage
(
    PHARDWAREINSTANCE   phwi,
    DWORD               dwData
) ;

VOID FAR PASCAL fmNoteOff
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bPatch,
    BYTE                bNote,
    BYTE                bChannel
) ;

VOID FAR PASCAL fmSend
(
    PHARDWAREINSTANCE   phwi,
    WORD                wAddress,
    BYTE                bValue
) ;

void FAR PASCAL fmCallback( PFMALLOC, WORD, DWORD, DWORD ) ;

// fmmain.c

VOID FAR PASCAL fmQuiet
(
    PHARDWAREINSTANCE   phwi
) ;

LRESULT FAR PASCAL fmOpen
(
    PHARDWAREINSTANCE   phwi,
    LPDWORD             pdwUser,
    LPMIDIOPENDESC      pod,
    DWORD               dwFlags
) ;

VOID FAR PASCAL fmClose
(
    PFMALLOC        pClient
) ;

VOID FAR PASCAL fmReset
(
    PHARDWAREINSTANCE   phwi
) ;

BOOL FAR PASCAL fmPatchPrepare( VOID ) ;
BOOL FAR PASCAL fmPatchUnprepare( VOID ) ;

void FAR PASCAL modGetDevCaps
(
    PHARDWAREINSTANCE   phwi,
    MDEVICECAPSEX FAR   *lpCaps
) ;

DWORD FAR PASCAL _loadds mixerPipeProc
(
    HPIPE           hp,
    DWORD           dwMsg,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

//
// product IDs, manufacturer IDs, etc.
//

#define MID_MICROSOFT   (MM_MICROSOFT)
#define DRV_VERSION     (0x0400)

#define PID_SYNTH       (MM_MSFT_MSOPL_SYNTH)

/* typedefs for MIDI patches */
#define NUMOPS                  (4)
#define PATCH_1_4OP             (0) /* use 4-operator patch */
#define PATCH_2_2OP             (1) /* use two 2-operator patches */
#define PATCH_1_2OP             (2) /* use one 2-operator patch */

typedef struct tagOPER
{
    BYTE    bAt20;              /* flags which are send to 0x20 on fm */
    BYTE    bAt40;              /* flags seet to 0x40 */
                                /* the note velocity & midi velocity affect total level */
    BYTE    bAt60;              /* flags sent to 0x60 */
    BYTE    bAt80;              /* flags sent to 0x80 */
    BYTE    bAtE0;              /* flags send to 0xe0 */

} OPER, FAR *LPOPER ;

typedef struct tagNOTE
{
    OPER    op[NUMOPS];         /* operators */
    BYTE    bAtA0[2];           /* send to 0xA0, A3 */
    BYTE    bAtB0[2];           /* send to 0xB0, B3 */
                                /* use in a patch, the block should be 4 to indicate
                                    normal pitch, 3 => octave below, etc. */
    BYTE    bAtC0[2];           /* sent to 0xc0, C3 */
    BYTE    bOp;                /* see PATCH_??? */
    BYTE    bDummy;             /* place holder */

} NOTE, FAR *LPNOTE ;

typedef struct tagPATCHES
{
   NOTE     note ;

} PATCHES, FAR *LPPATCHES ;

// patch library...

LPPATCHES glpPatch ;

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
//  End of File: msopl.h
//---------------------------------------------------------------------------
