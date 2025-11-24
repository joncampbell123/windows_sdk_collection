//---------------------------------------------------------------------------
//
//  Module:   msmpu401.h
//
//  Description:
//     Header file for MSMPU401
//
//---------------------------------------------------------------------------
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1994 - 1995 Microsoft Corporation.	All Rights Reserved.
//
//---------------------------------------------------------------------------

#ifdef MIDI_STREAMS
 #include "streams.h"
#endif

#define Not_VxD
#define WANT_MSMPU401
#include <vxdpipe.h>

// want lots of symbols in DEBUG .sym file..

#define DRIVER_VERSION          0x0400
#define DEF_MIDIINPERSISTENCE   50

#define MSMPU401_VERSION_RQD              0x0400

// VxD PM API interface...

#define MSMPU401_API_Get_Version          0x0300
#define MSMPU401_API_Get_Info             0x0301
#define MSMPU401_API_GetInfoF_DevNode     0x0001

#define MSMPU401_API_Acquire              0x0302
#define MSMPU401_API_Release              0x0303

//
// MPU-401 commands (dumb mode)
//

#define MPU401_CMD_UART_MODE  0x3F
#define MPU401_CMD_RESET      0xFF

//
// MIDI input and output state flags...
//

#define MIF_ALLOCATED         0x01
#define MIF_STARTED           0x02
#define MIF_INISR             0x80


#define MOF_ALLOCATED         0x01

//
// typedefs
//

#define BCODE _based(_segname("_CODE"))


// NOTE! there is a parallel structure in mpu401.inc!
//
typedef struct tagHARDWAREINSTANCE
{
   //
   // Supporting VxD entry point
   //

   LPVOID      pVxDEntry ;
   DWORD       cAcquire ;

   // hardware instance information from VxD

   WORD        wHardwareOptions ;            // hardware options
   WORD        wIOAddressMPU401 ;            // base I/O
   BYTE        bIRQ ;                        // IRQ
   BYTE        bReserved ;                   // alignment
   DWORD       dn ;                          // DevNode

   //
   // other support data
   //

   UINT        uVxDId ;                      // supporting VxD ID
   WORD        cReference ;                  // reference count
   WORD        cEnable ;                     // enable count
   BOOL        fEnabled ;                    // TRUE if instance enabled

   // IRQ stuff

   UINT        uISRDataSel ;
   UINT        uISRCodeSel ;

   BYTE        bIntVector ;
   BYTE        bOrigIntMask ;
   LPVOID      lpOldISR ;
   WORD        wEOICommands ;
   BOOL        fISRCanReadData ;

   // isr pipe callback proc

   HPIPE          hpisr ;
   FNPIPECALLBACK fnisrPipe ;

   // midi flags

   BYTE        bMidiInFlags ;
   BYTE        bMidiOutFlags ;
   WORD        wMidiInPersistence ;

   // Allocated MIDI output/input clients, for this hardware
   // instance... if any.
   //
   // NOTE! These are used by the interrupt handler and 
   // output data functions to send/receive full-duplex data
   // when processing the MIDI stream.
   //

   VOID NEAR   *pmic ;
   VOID NEAR   *pmoc ;

   // link to next instance

   struct tagHARDWAREINSTANCE NEAR *pNext ;

   // midi streams information
   //
   #ifdef MIDI_STREAMS
    MIDISTREAMDATA msd;
   #endif

} HARDWAREINSTANCE, NEAR *PHARDWAREINSTANCE ;

typedef struct tagMPU401INFO
{
   DWORD   dwSize ;
   WORD    wHardwareOptions ;
   WORD    wIOAddressMPU401 ;
   BYTE    bIRQ ;
   BYTE    bReserved ;
   WORD    wVersionVxD ;
   WORD    wFlags ;
   WORD    wReserved ;

   DWORD   dn ;
   DWORD   dwIRQHandle ;
   DWORD   dwMPU401OwnerCur ;
   DWORD   dwMPU401OwnerLast ;
   DWORD   hMPU401Stubs ;

} MSMPU401INFO, FAR *LPMSMPU401INFO ;

#define MSMPU401_HWOPTIONSF_IRQSHARED  0x0001

// per allocation structure for midi port

typedef struct tagPORTALLOC
{
    PHARDWAREINSTANCE  phwi;        // ptr to hardware instance structure
    HMIDIOUT           hMidi;       // handle for stream
    DWORD              dwCallback;  // callback for stream
    DWORD              dwInstance;  // app's instance variable
    DWORD              dwFlags;     // allocation flags

} PORTALLOC, NEAR *PPORTALLOC;

// This MUST be an even power of 2!!!

#define MIDIIN_QUEUE_SIZE  64 

typedef struct tagMIDIINCLIENT
{
   PORTALLOC   pa;

   WORD        wDataQHead;
   WORD        wDataQTail;
   BYTE        abDataQueue[MIDIIN_QUEUE_SIZE];

   BYTE        fSysEx;
   BYTE        bStatus;

   BYTE        bBytesLeft;
   BYTE        bBytePos;

   DWORD       dwShortMsg;
   DWORD       dwMsgTime;
   DWORD       dwRefTime;

   DWORD       dwCurData;
   LPMIDIHDR   lpmhQueue;

} MIDIINCLIENT, NEAR *PMIDIINCLIENT ;

typedef struct tagMIDIOUTCLIENT
{
    PORTALLOC   pa;

    WORD        wMidiOutEntered;
    BYTE        bCurrentStatus;
    BYTE        bFill;

} MIDIOUTCLIENT, NEAR *PMIDIOUTCLIENT ;

//
// globals
//

// in init.c

#ifdef DEBUG
extern WORD         wDebugLevel;    // debug level
#endif
extern HANDLE       ghModule;       // our module handle

//
// prototypes
//

// assemu.asm:

BOOL NEAR PASCAL Create_ISR( PHARDWAREINSTANCE ) ;
BOOL NEAR PASCAL SetInterruptMask( BYTE bIRQ, BYTE fMask ) ;
LPVOID NEAR PASCAL SetInterruptVector( BYTE bIRQ, LPVOID lpNewISR ) ;


// drvproc.c:

extern LRESULT FAR PASCAL _loadds DriverProc
(
    DWORD           dwDriverID,
    HDRVR           hDriver,
    UINT            uiMessage,
    LPARAM          lParam1,
    LPARAM          lParam2
) ;

// init.c:

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

// midifix.c

LRESULT FAR PASCAL _loadds isrPipeProc
(
    HPIPE           hp,
    DWORD           dwMsg,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

void FAR PASCAL midiCallback
(
    PPORTALLOC      pPort,
    WORD            msg,
    DWORD           dw1,
    DWORD           dw2
) ;

LRESULT FAR PASCAL _loadds modMessage
(
    UINT            uDevId,
    UINT            uMsg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

// midiin.c

void NEAR PASCAL midSendPartBuffer
(
    PMIDIINCLIENT   pClient
) ;

LRESULT FAR PASCAL _loadds midMessage
(
    UINT            uDevId,
    UINT            uMsg,
    DWORD           dwUser,
    DWORD           dwParam1,
    DWORD           dwParam2
) ;

// midiout.c:

void FAR PASCAL modGetDevCaps
(
    PHARDWAREINSTANCE       phwi,
    MDEVICECAPSEX FAR*      lpCaps
) ;

// midia.asm

void FAR PASCAL mpuCommandWrite
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bCommand
) ;

void FAR PASCAL midStart
(
    PMIDIINCLIENT   pClient
) ;

void FAR PASCAL midStop
(
    PMIDIINCLIENT   pClient
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

BOOL FAR PASCAL AcquireMPU401
(
    PHARDWAREINSTANCE   phwi
) ;

BOOL FAR PASCAL ReleaseMPU401
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

// mpu401.asm

BOOL NEAR PASCAL modDataWrite
(
    PHARDWAREINSTANCE   phwi,
    BYTE                bDataByte
) ;

VOID FAR PASCAL MPU401InterruptHandler( WORD wPort ) ;

//
// strings
//

#ifndef NOSTR
extern char far gszPortKey[];
extern char far gszIntKey[];
extern char far gszDriverName[];
extern char far gszIniFile[];
#endif // NOSTR

//
//  Product Description strings.  For this driver, all descriptions are
//  the same..
//
#define IDS_MPU401PRODUCT   16
#define IDS_MPU401MIDIIN    IDS_MPU401PRODUCT
#define IDS_MPU401MIDIOUT   IDS_MPU401PRODUCT

//
// Debug output
//       

#ifdef DEBUG
    extern char STR_PROLOGUE[];
    extern char STR_CRLF[];
    extern char STR_SPACE[];
    extern WORD  wDebugLevel;     // debug level
    #define D( x )          { x }
    #define DPF( x, y ) if (x <= wDebugLevel) (OutputDebugStr(STR_PROLOGUE), OutputDebugStr(y), OutputDebugStr(STR_CRLF))
#else
    #define D( x )
    #define DPF( x, y )
#endif

#define QUOTE(x) #x
#define QQUOTE(y) QUOTE(y)
#define REMIND(str) __FILE__ "(" QQUOTE(__LINE__) ") : " str

//
// Assertion macros
//

#if defined DEBUG && !defined RC_INVOKED

 #pragma warning (disable:4704)
 #define INLINE_BREAK _asm {int 3}

 #define assert(cond) { \
    static char BCODE szErr[] = __FILE__ "(" QQUOTE(__LINE__) ") assert failed! " #cond ; \
    if (!(cond)) { \
        OutputDebugStr (STR_PROLOGUE); \
        OutputDebugStr (szErr); \
        OutputDebugStr (STR_CRLF); \
        INLINE_BREAK; \
        } \
    }

#else

 #define INLINE_BREAK
 #define assert(cond) {}

#endif

//---------------------------------------------------------------------------
//  End of File: msmpu401.h
//---------------------------------------------------------------------------
