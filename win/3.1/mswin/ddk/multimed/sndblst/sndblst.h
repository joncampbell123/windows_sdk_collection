/****************************************************************************
 *
 *   sndblst.h
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#ifdef DEBUG
    #define static
#endif

#define DRIVER_VERSION          0x0101
#define MAX_ERR_STRING          250      /* max length of string table errors */
#define DLG_CONFIG              42       /* dialog box resource id */

#define DSP_VERSION_REQD        0x0200   /* DSP version required for driver */
#define DSP_VERSION_PRO         0x0300   /* DSP version for the Pro card */

#ifndef DRVCNF_RESTART
    #define DRVCNF_CANCEL       0x0000   /* DRV_CANCEL */
    #define DRVCNF_OK           0x0001   /* DRV_OK */
    #define DRVCNF_RESTART      0x0002   /* DRV_RESTART */
#endif

/****************************************************************************

       typedefs

 ***************************************************************************/

/* per allocation structure for midi */
typedef struct portalloc_tag {
    DWORD               dwCallback;     /* client's callback */
    DWORD               dwInstance;     /* client's instance data */
    HMIDIOUT            hMidi;          /* handle for stream */
    DWORD               dwFlags;        /* allocation flags */
}PORTALLOC, NEAR *NPPORTALLOC;

typedef struct tMIDIMSGCLIENT {
    BYTE        fSysEx;
    BYTE        bStatus;

    BYTE        bBytesLeft;
    BYTE        bBytePos;

    DWORD       dwShortMsg;
    DWORD       dwMsgTime;
    DWORD       dwRefTime;

    DWORD       dwCurData;
    LPMIDIHDR   lpmhQueue;

} MIDIINMSGCLIENT, NEAR *NPMIDIINMSGCLIENT, *PMIDIINMSGCLIENT, FAR *LPMIDIINMSGCLIENT;

/* per allocation structure for wave */
typedef struct wavealloc_tag {
    DWORD               dwCallback;     /* client's callback */
    DWORD               dwInstance;     /* client's instance data */
    HANDLE              hWave;          /* handle for stream */
    DWORD               dwFlags;        /* allocation flags */
    DWORD               dwByteCount;    /* byte count since last reset */
    PCMWAVEFORMAT       pcmwf;          /* format of wave data */
}WAVEALLOC, NEAR * NPWAVEALLOC;

typedef char huge *HPSTR;

/****************************************************************************

       strings - all non-localized strings can be found in initc.c

 ***************************************************************************/

#ifndef NOSTR
extern char far STR_PORT[];
extern char far STR_INT[];
extern char far STR_DRIVERNAME[];
extern char far STR_INIFILE[];
extern char far STR_PRODUCTNAME[];
  #ifdef DEBUG
  extern char far STR_CRLF[];
  extern char far STR_SPACE[];
  #endif
#endif /* NOSTR */

/*  Error strings... */
#define IDS_ERRTWODRIVERS       1
#define IDS_ERRMCANOTSUPPORTED  2
#define IDS_ERROLDVDMAD         3
#define IDS_ERRBADPORT          4
#define IDS_ERRBADVERSION       5
#define IDS_ERRBADINT           6

#define IDS_ERRBADCONFIG        16
#define IDS_WARNPROCARD         17
#define IDS_WARNTHUNDER         18

/*  Product description strings - for this driver, they are all the */
/*  same text, so make all ID's equal */
#define IDS_SNDBLSTPRODUCT      32
#define IDS_SNDBLSTWAVEIN       IDS_SNDBLSTPRODUCT
#define IDS_SNDBLSTWAVEOUT      IDS_SNDBLSTPRODUCT
#define IDS_SNDBLSTMIDIIN       IDS_SNDBLSTPRODUCT
#define IDS_SNDBLSTMIDIOUT      IDS_SNDBLSTPRODUCT

/****************************************************************************

       globals

 ***************************************************************************/

/* in initc.c */
extern HANDLE    ghModule;      /* our module handle */

/* in wavefix.c */
extern HPSTR     hpCurInData;   /* points to data block of current input hdr */
extern DWORD     dwCurInCount;  /* bytes left in current input block */
extern HPSTR     hpCurData;     /* points to data block of current output hdr */
extern DWORD     dwCurCount;    /* bytes left in current output block */
extern LPWAVEHDR lpLoopStart;   /* pointer to first block of a loop */
extern DWORD     dwLoopCount;   /* count for current loop */
extern BYTE      bBreakLoop;    /* set to non-zero to break loop */

/* in midifix.c */
extern PORTALLOC gMidiInClient;  /* input client information structure */

/* in sndblst.asm */
extern BYTE      gfEnabled;      /* has the card been enabled? */
extern BYTE      gfDMABusy;      /* is DMA in progress? */
extern LPWAVEHDR glpWOQueue;     /* wave output data buffer queue */
extern LPWAVEHDR lpDeadHeads;    /* wave output dead data headers */
extern LPSTR     lpSilenceStart; /* where the padded silence starts */
extern WORD      wSilenceSize;   /* how big the padded silence is */
extern LPWAVEHDR glpWIQueue;     /* wave input data buffer queue */

/***************************************************************************

    prototypes

***************************************************************************/

/* commona.asm */
extern void FAR  PASCAL MemCopy(LPVOID lpDst, LPVOID lpSrc, WORD cnt);

/* inita.asm */
extern WORD FAR PASCAL Enable(void);
extern WORD FAR PASCAL Disable(void);
extern WORD NEAR PASCAL InitPreliminary(void);
extern DWORD FAR PASCAL InitVerifyConfiguration(WORD wPort, BYTE bInt, BYTE bDMAChannel);
extern WORD NEAR PASCAL InitSetConfiguration(WORD wPort, BYTE bInt, BYTE bDMAChannel, WORD wMidiInPersistence);
extern DWORD FAR PASCAL InitGetConfiguration(void);

/* midia.asm */
extern void FAR PASCAL midStart(void);
extern void FAR PASCAL midStop(void);
extern void FAR PASCAL CritEnter(void);
extern void FAR PASCAL CritLeave(void);
extern WORD FAR PASCAL modAcquireHardware(void);
extern WORD FAR PASCAL modReleaseHardware(void);
extern WORD FAR PASCAL midAcquireHardware(void);
extern WORD FAR PASCAL midReleaseHardware(void);

/* wavea.asm */
extern void   FAR  PASCAL dspSetSampleRate(WORD);
extern void NEAR PASCAL wodWrite(LPWAVEHDR);
extern WORD FAR  PASCAL wodPause(void);
extern WORD FAR  PASCAL wodResume(void);
extern void FAR PASCAL wodHaltDMA(void);
extern void FAR  PASCAL widStart(void);
extern void FAR  PASCAL wodWaitForDMA(void);
extern void FAR  PASCAL widStop(void);
extern WORD FAR PASCAL wodAcquireHardware(void);
extern WORD FAR PASCAL wodReleaseHardware(void);
extern WORD FAR PASCAL widAcquireHardware(void);
extern WORD FAR PASCAL widReleaseHardware(void);

/* sndblst.asm */
extern BOOL NEAR PASCAL modDataWrite(BYTE);
extern LPVOID NEAR PASCAL MemCopySrc(LPVOID lpDst, LPVOID lpSrc, WORD cnt);
extern LPVOID NEAR PASCAL MemCopyDst(LPVOID lpDst, LPVOID lpSrc, WORD cnt);
extern void   NEAR PASCAL MemFillSilent(LPSTR lpDst, WORD cnt);

/* config.c */
extern int FAR PASCAL Config(HWND hWnd, HANDLE hInstance);
extern void FAR PASCAL ConfigRemove(void);
extern int FAR  PASCAL _loadds ConfigDlgProc(HWND hDlg, WORD msg, WORD wParam, LONG lParam);

/* drvproc.c */
extern LRESULT FAR PASCAL _loadds DriverProc(DWORD dwDriverID, HDRVR hDriver, UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

/* initc.c */
extern int NEAR PASCAL LibMain(HANDLE hModule, WORD wHeapSize, LPSTR lpCmdLine);
extern BYTE NEAR PASCAL ConfigGetDMAChannel(void);
extern BYTE NEAR PASCAL ConfigGetIRQ(void);
extern WORD NEAR PASCAL ConfigGetPortBase(void);
extern BOOL FAR PASCAL InitDisplayConfigErrors(void);

/* midifix.c */
extern void  FAR  PASCAL midiCallback(NPPORTALLOC pPort, WORD msg, DWORD dw1, DWORD dw2);
extern DWORD FAR  PASCAL _loadds modMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2);

/* midiin.c */
extern void  NEAR PASCAL midSendPartBuffer(void);
extern DWORD FAR  PASCAL _loadds midMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2);

/* midiout.c */
extern void FAR PASCAL modGetDevCaps(LPBYTE lpCaps, WORD wSize);

/* wavefix.c */
extern void FAR  PASCAL waveCallback(NPWAVEALLOC pWave, WORD msg, DWORD dw1);
extern void FAR  PASCAL wodBlockFinished(LPWAVEHDR lpHdr);
extern WORD NEAR PASCAL widFillBuffer(LPSTR lpBuffer, WORD wBufSize);
extern void FAR  PASCAL widBlockFinished(LPWAVEHDR lpHdr);

/* wavein.c */
extern void  NEAR PASCAL widSendPartBuffer(void);
extern DWORD FAR  PASCAL _loadds widMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2);

/* waveout.c */
extern DWORD NEAR PASCAL waveGetPos(DWORD dwUser, LPMMTIME lpmmt, WORD wSize);
extern DWORD FAR  PASCAL _loadds wodMessage(WORD id, UINT msg, DWORD dwUser, DWORD dwParam1, DWORD dwParam2);

/****************************************************************************

       Configuration support

 ***************************************************************************/

#define IDC_FIRSTINT  0x100
#define IDC_2         0x100
#define IDC_3         0x101
#define IDC_5         0x102
#define IDC_7         0x103
#define IDC_LASTINT   0x103

#define IDC_FIRSTPORT 0x201
#define IDC_210       0x201
#define IDC_220       0x202
#define IDC_230       0x203
#define IDC_240       0x204
#define IDC_250       0x205
#define IDC_260       0x206
#define IDC_LASTPORT  0x206

#define IDC_PORTGRP   0x401
#define IDC_INTGRP    0x402

/****************************************************************************

       Debug output

 ***************************************************************************/
#ifdef DEBUG
   extern WORD  wDebugLevel;     /* debug level */

   #define D1(sz) if (wDebugLevel >= 1) (OutputDebugStr(STR_CRLF),OutputDebugStr(sz))
   #define D2(sz) if (wDebugLevel >= 2) (OutputDebugStr(STR_SPACE),OutputDebugStr(sz))
   #define D3(sz) if (wDebugLevel >= 3) (OutputDebugStr(STR_SPACE),OutputDebugStr(sz))
   #define D4(sz) if (wDebugLevel >= 4) (OutputDebugStr(STR_SPACE),OutputDebugStr(sz))
#else
   #define D1(sz) 0
   #define D2(sz) 0
   #define D3(sz) 0
   #define D4(sz) 0
#endif

/****************************************************************************

       Assert

 ***************************************************************************/
#ifdef DEBUG
    extern void FAR PASCAL AssertBreak(void);
    #define AssertF(exp) \
        if ((exp) == 0)  \
        { \
            D1("AssertF failed (" #exp ")"); \
            AssertBreak(); \
        }

    #define AssertT(exp) \
        if ((exp) != 0)  \
        { \
            D1("AssertT failed (" #exp ")"); \
            AssertBreak(); \
        }
#else
    #define AssertBreak()
    #define AssertF(exp) 
    #define AssertT(exp) 
#endif
