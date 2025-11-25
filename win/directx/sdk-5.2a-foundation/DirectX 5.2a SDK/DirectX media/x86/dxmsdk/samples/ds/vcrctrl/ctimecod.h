//===========================================================================
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//===========================================================================
//
//	filename: ctimecod.h
//
//	header for ctimecod.cpp - basic timecode support class
//
//
//
#ifndef CTIMECOD_H
#define	CTIMECOD_H	1

#include <vfw.h>

// Timecode framerate definitions
#define AM_TIMECODE_30DROP		0	// 30 fps, dropframe
#define AM_TIMECODE_30NONDROP	30	// 30 fps, non-dropframe
#define AM_TIMECODE_25			25	// 25 fps (PAL)
#define AM_TIMECODE_24			24	// 24 fps

// Timecode flag masks:
#define	AM_TIMECODE_FLAG_FCM			0x00000001	// Frame code mode:
													//  0=nondrop, 1=drop
#define	AM_TIMECODE_FLAG_CF				0x00000002	// color frame flag
#define	AM_TIMECODE_FLAG_FIELD			0x00000004	// field flag
#define	AM_TIMECODE_FLAG_DF				0x00000008	// drop frame flag - 
													//  from flags in actual
													//  timecode
#define	AM_TIMECODE_COLORFRAME			0x000000f0	// which frame in color 
													//  sequence
#define	AM_TIMECODE_COLORSEQUENCE		0x00000f00	// duration in frames of 
													//  complete sequence
#define	AM_TIMECODE_FILMSEQUENCE_TYPE	0x0000f000	// One of FILM_SEQUENCE_XXX
													//  defines
// upper 16 bits are reserved for future use - set to 0

// for informational purposes:
#define FILM_SEQUENCE_NONE       0
#define FILM_SEQUENCE_AABBBCCDDD 1
#define FILM_SEQUENCE_AAABBCCCDD 2


//
//	The timecode support class
//
class CTimecode {
public:

	// convert HH:MM:SS:FFs to binary.  Args are pointer to null-terminated
	//	"HH:MM:SS:FF' string and frame code mode:MC_FORMAT_SMPTE_30 ||
	//	MC_FORMAT_SMPTE_30DROP
	DWORD ConvertStringToTimecode(TCHAR *lpszTimecode, 
									PTIMECODE_SAMPLE pTimecodeSample);

	// convert binary frames to HH:MM:SS:FFs.  NOTE:
	//	DF algorithm does not account for mathematicaly-generated
	//	values, i.e., assumes all binary values came from Ascii-converted
	//	DF values.
	DWORD ConvertTimecodeToString(PTIMECODE_SAMPLE pTimecodeSample, 
									TCHAR *lpszTimecode);
};

#endif		// #ifndef CTIMECOD_H
// eof ctimecod.h

