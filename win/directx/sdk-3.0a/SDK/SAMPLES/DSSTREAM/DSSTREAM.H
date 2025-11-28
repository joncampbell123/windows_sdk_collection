/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:               DSStream.h
 *  Content:    DirectSound Stream Sample Application Header
 *
 ***************************************************************************/
#ifndef __DSSTREAM_INCLUDED__
#define __DSSTREAM_INCLUDED__

#include <dsound.h>
#include "resource.h"
#include "debug.h"

/* WARNING -- Be careful about changing NUM_BUFFER_SEGMENTS -- it should be at
 * least 3, or there will probably not be enough time for the interrupt to do
 * its thing before the player catches up to it.  Also, PLAYBACK_TIMER_PERIOD
 * is set at 200 milliseconds because we are OVERSAMPLING by a factor of four.
 * This is so we can shift the frequency on the fly and still keep up with the
 * play cursor.  The timer routine will only fill when necessary.  Experiments
 * have shown that the timer still operates well when interrupted every 50ms
 * (and possibly even more often than that).  If you do not require real-time
 * frequency changes, you reduce the OVERSAMPLE as well as the TIMER_PERIOD.
 * However, note that since a buffer segment is only an approximation of the
 * number of bytes played per interrupt, it's a good idea to interupt a bit more
 * often than the TIMER_PERIOD, just to make sure the timer isn't falling behind
 * the player.  If your frequency is fixed, the call to initialize the timer
 * event could be changed to first calculate something like (TIMER_PERIOD * .8).
 * This should ensure that the buffer is well maintained and system performance
 * is maximized.
 */

#define NUM_BUFFER_SEGMENTS     6
#define PLAYBACK_TIMER_PERIOD   200
#define PLAYBACK_OVERSAMPLE     4
#define PLAYBACK_TIMER_ACCURACY 10

#define DSSTREAM_STOPF_NOREOPEN 0x0001
#define DSSTREAM_STOPF_NOEXIT   0x0002  // Prevent the stop code from calling exit

#define ERR_WAVE_OPEN_FAILED    -100
#define ERR_WAVE_INVALID_FORMAT -101
#define ERR_CREATEDSB_FAILED    -102
#define ERR_WAVE_CORRUPTED_FILE -103


#define BORDER_SPACE_CX         10
#define BORDER_SPACE_CY         10
#define CONTROL_SPACE_CX        4
#define CONTROL_SPACE_CY        4
#define TEXT_SPACE_CY           0
#define TEXT_SPACE_CX           2

#define BUTTON_CX               70
#define BUTTON_CY               32
#define CHECK_CX                70
#define CHECK_CY                22
#define BUTTON_SPACE_CY         6

#define PAN_TEXT_CX             140
#define VOL_TEXT_CX             140
#define FREQ_TEXT_CX            140
#define PROG_TEXT_CX            140

#define PAN_TB_CX               200
#define PAN_TB_CY               30
#define VOL_TB_CX               200
#define VOL_TB_CY               30
#define FREQ_TB_CX              200
#define FREQ_TB_CY              30
#define PROG_TB_CX              200
#define PROG_TB_CY              30

// The values for PAN may change in range...

//#define PAN_TB_MIN              0
//#define PAN_TB_MAX              2000
//#define PAN_TB_CENTER           1000
//#define PAN_MULTIPLIER          1
#define PAN_MIN                 0
#define PAN_MAX                 800
#define PAN_CENTER              400
#define PAN_SHIFT               (-400)
#define PAN_PAGESIZE            10
#define PAN_DIV                 10
#define PAN_MULT                10

//#define PAN_DSB_MIN           (-400)
//#define PAN_DSB_MAX           400
//#define PAN_DSB_CENTER        0

//#define VOL_TB_MIN              0
//#define VOL_TB_MAX              1000
//#define VOL_MULTIPLIER          1
#define VOL_SHIFT               (-400)
#define VOL_MIN                 0
#define VOL_MAX                 400
#define VOL_PAGESIZE            10
#define VOL_DIV                 10
#define VOL_MULT                10
#define FREQ_MIN                441
#define FREQ_MAX                4410
#define FREQ_PAGESIZE           100
#define FREQ_MULTIPLIER         10
#define PROG_MIN                0
#define PROG_MAX                10000
#define PROG_MULTIPLIER         100


/* WAVE I/O subsystem defines */

#define WAVEVERSION 1

#ifndef ER_MEM
#define ER_MEM                  0xe000
#endif

#ifndef ER_CANNOTOPEN
#define ER_CANNOTOPEN           0xe100
#endif

#ifndef ER_NOTWAVEFILE
#define ER_NOTWAVEFILE          0xe101
#endif

#ifndef ER_CANNOTREAD
#define ER_CANNOTREAD           0xe102
#endif

#ifndef ER_CORRUPTWAVEFILE
#define ER_CORRUPTWAVEFILE      0xe103
#endif

#ifndef ER_CANNOTWRITE
#define ER_CANNOTWRITE          0xe104
#endif

/* Streaming communication defines and structures */

#define WM_DSSTREAM_DONE        WM_USER + 0x100 /* Make our own app messages */
#define WM_DSSTREAM_DEBUG       WM_USER + 0x101
#define WM_DSSTREAM_PROGRESS    WM_USER + 0x102

#define DEBUGF_PLAYPOSITION     0x0300
#define DEBUGF_WRITEPOSITION    0x0301
#define DEBUGF_NEXTWRITE        0x0302
#define DEBUGF_SKIP             0x0303

/*
 * This structure keeps all the data that the TimeFunc callback uses in one
 * place.  In this implementation, that means the global data segement.  This
 * is setup so that if you wanted to put your callback in a DLL, all you'd need
 * to do is pass the address of this structure as a parameter.
 */

typedef struct waveinfoca_tag
{
    WAVEFORMATEX         *pwfx;             /* Wave Format data structure */
    HMMIO                hmmio;             /* MM I/O handle for the WAVE */
    MMCKINFO             mmck;              /* Multimedia RIFF chunk */
    MMCKINFO             mmckInRIFF;        /* Use in opening a WAVE file */
    LPDIRECTSOUNDBUFFER  lpDSBStreamBuffer; /* Points to DirectSoundBuffer */
    DWORD                dwBufferSize;      /* Size of the entire buffer */
    DWORD                dwNextWriteOffset; /* Offset to next buffer segment */
    DWORD                dwProgress;        /* Used with above to show prog. */
    DWORD                dwBytesRemaining;  /* Bytes 'til timer shutdown */
    BOOL                 bDonePlaying;      /* Signals early abort to timer */
    BOOL                 bLoopFile;         /* Should we loop playback? */
    BOOL                 bFoundEnd;         /* Timer found file end */
} WAVEINFOCA, *LPWAVEINFOCA;

/* Function declarations */

LRESULT CALLBACK MainWindowProc( HWND, unsigned, WPARAM, LPARAM );
BOOL CALLBACK DLG_About( HWND, UINT, WPARAM, LPARAM );
void CALLBACK TimeFunc( UINT, UINT, DWORD, DWORD, DWORD );
BOOL CALLBACK DSEnumDlgProc( HWND, UINT, WPARAM, LPARAM );
BOOL CALLBACK DSEnumProc( LPGUID, LPSTR, LPSTR, LPVOID );

void ErrorMessageBox( UINT, DWORD );
void HandlePanScroll( int, int );
void HandleVolScroll( int, int );
void HandleFreqScroll( int, int );
void ResetWavePlayer( void );
void UpdateFromControls( void );

int CreateChildren( RECT );
int HandleCommDlgError( DWORD );
int StreamBufferSetup( void );

BOOL DoDSoundEnumerate( LPGUID );

int WaveOpenFile(char *, HMMIO *, WAVEFORMATEX **, MMCKINFO *);
int WaveStartDataRead(HMMIO *, MMCKINFO *, MMCKINFO *);
int WaveReadFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *);
int WaveCloseReadFile(HMMIO *, WAVEFORMATEX **);

int WaveCreateFile(char *, HMMIO *, WAVEFORMATEX *, MMCKINFO *, MMCKINFO *);
int WaveStartDataWrite(HMMIO *, MMCKINFO *, MMIOINFO *);
int WaveWriteFile(HMMIO, UINT, BYTE *, MMCKINFO *, UINT *, MMIOINFO *);
int WaveCloseWriteFile(HMMIO *, MMCKINFO *, MMCKINFO *, MMIOINFO *, DWORD);

int WaveLoadFile(char *, UINT *, DWORD *, WAVEFORMATEX **, BYTE **);
int WaveSaveFile(char *, UINT, DWORD, WAVEFORMATEX *, BYTE *);

int WaveCopyUselessChunks(HMMIO *, MMCKINFO *, MMCKINFO *, HMMIO *,
                                MMCKINFO *, MMCKINFO *);
BOOL riffCopyChunk(HMMIO, HMMIO, const LPMMCKINFO);

#endif /* __DSSTREAM_INCLUDED__ */

