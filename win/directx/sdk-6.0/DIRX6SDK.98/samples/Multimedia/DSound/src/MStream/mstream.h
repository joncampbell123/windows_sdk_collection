/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:	MStream.h
 *  Content:	Main header file for the MSTREAM sample application
 *
 ***************************************************************************/
#ifndef __MSTREAM_INCLUDED__
#define __MSTREAM_INCLUDED__

/*****************************************************************************/
/* Controls for buffer size, etc. */

#define TRACK_BUFFER_SIZE		1024
#define OUT_BUFFER_SIZE                 1024 // Max stream buffer size in bytes
#define BUFFER_TIME_LENGTH		60   // Amount to fill in milliseconds
#define NUM_STREAM_BUFFERS              2

#define DEBUG_CALLBACK_TIMEOUT		2000	// Wait 2 seconds for callback

/*****************************************************************************/
/* A bunch of constants used for calculating position */
/* and size of child controls for the main window.    */

#define BORDER_SPACE_CX		10
#define BORDER_SPACE_CY		10
#define CONTROL_SPACE_CX	4
#define CONTROL_SPACE_CY	4
#define TEXT_SPACE_CY		3
#define TEXT_SPACE_CX		2

#define BUTTON_CX		68
#define BUTTON_CY		26
#define CHECK_CX		68
#define CHECK_CY		26
#define BUTTON_SPACE_CX		4
#define BUTTON_SPACE_CY		4
#define CHECK_SPACE_CY		4

#define TEMPO_TB_CX		150
#define TEMPO_TB_CY             32
#define VOL_TB_CX		150
#define VOL_TB_CY               32

#define TEMPO_TEXT_CX		TEMPO_TB_CX
#define VOL_TEXT_CX		VOL_TB_CX

#define VOL_TB_MIN		0
#define VOL_TB_MAX		1000
#define VOL_PAGESIZE		10
#define VOL_MIN			0
#define VOL_MAX			127
#define VOL_CACHE_INIT		100
#define TEMPO_MIN		1
#define TEMPO_MAX		500
#define TEMPO_PAGESIZE		10


#define WM_MSTREAM_PROGRESS	(WM_USER + 100)
#define WM_MSTREAM_UPDATEVOLUME	(WM_USER + 101)

/*****************************************************************************/
/* All sorts of bit flags and error codes for */
/* communicating between various subsystems.  */

#define MSTREAM_STOPF_NOREOPEN		0x0001

#define STREAMF_BUFFER_WARNING		0x00000002

#define CONVERTF_RESET			0x00000001

#define CONVERTF_STATUS_DONE		0x00000001
#define CONVERTF_STATUS_STUCK		0x00000002
#define CONVERTF_STATUS_GOTEVENT	0x00000004

#define CONVERTERR_NOERROR	0	// No error occured
#define CONVERTERR_CORRUPT	-101	// The input file is corrupt
// The converter has already encountered a corrupt file and cannot convert any
// more of this file -- you must reset the converter
#define CONVERTERR_STUCK	-102
#define CONVERTERR_DONE		-103	// Converter is done
#define CONVERTERR_BUFFERFULL	-104	// The buffer is full
#define CONVERTERR_METASKIP	-105	// Skipping unknown meta event

#define STATUS_KILLCALLBACK	100	// Signals that the callback should die
#define STATUS_CALLBACKDEAD	200	// Signals callback is done processing
#define STATUS_WAITINGFOREND	300    // Callback's waiting for buffers to play

#define VOLUME_BUFFER		3567	// Random value which acts as a buffer id


#ifdef DEBUG
#define DebugPrint( sz )        DPF( 3, sz )
#else
#define DebugPrint( sz )
#endif



/*
 *   This structure is used to pass information to the ConvertToBuffer()
 * system and then internally by that function to send information about the
 * target stream buffer and current state of the conversion process to lower
 * level conversion routines internal to the MSTRCONV module.  See that source
 * file for specific details.
 */
typedef struct _ConvertInfo
{
    MIDIHDR	mhBuffer;	    // Standard Windows stream buffer header
    DWORD	dwStartOffset;	    // Start offset from mhStreamBuffer.lpStart
    DWORD	dwMaxLength;	    // Max length to convert on this pass

    DWORD	dwBytesRecorded;    // Used internally by the MSTRCONV module
    DWORD	tkStart;	    // Used internally by the MSTRCONV module
    BOOL	bTimesUp;	    // Used internally by the MSTRCONV module
} CONVERTINFO, *LPCONVERTINFO;


/*****************************************************************************/
/* Function declarations */

LRESULT CALLBACK MainWindowProc( HWND, unsigned, WPARAM, LPARAM );
BOOL CALLBACK DLG_About( HWND, UINT, WPARAM, LPARAM );
void CALLBACK MidiProc( HMIDIIN, UINT, DWORD, DWORD, DWORD );

void BuildTitleBarText( void );
void ErrorMessageBox( UINT, DWORD );
void HandleTempoScroll( int, int );
void HandleVolScroll( int, int );
void MidiErrorMessageBox( MMRESULT );
void SetAllChannelVolumes( DWORD dwVolumePercent );
void SetChannelVolume( DWORD dwChannel, DWORD dwVolumePercent );
void UpdateFromControls( void );

int CreateChildren( RECT );
int HandleCommDlgError( DWORD );
BOOL StreamBufferSetup( void );

BOOL 	ConverterInit( LPSTR szInFile );
void 	ConverterCleanup( void );
int	ConvertToBuffer( DWORD, LPCONVERTINFO );

#endif /* __MSTREAM_INCLUDED__ */


