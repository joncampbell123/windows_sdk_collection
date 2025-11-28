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
//	   filename: ctimecod.cpp
//
//     SMPTE Timecode support class
//
//---------------------------------------------------------
//
//	This code is provided for reference purposes only
//
//	NOTES:
//	 1. Code is un-optimized for clarity
//	 2. Minimal error checking.
//	 3. Negative t/c values not supported
//	 4. File logging functions are here for illustrative 
//		purposes but not used.
//
//---------------------------------------------------------


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <vfw.h>
#include <streams.h>
#include "ctimecod.h" 

// timecode conversion tables
#define TC_LENGTH 11*sizeof(TCHAR)		// length of a t/c string
static long lNDFTable[] = {1080000L,108000L,18000L,1800L,300L,30L,10L,1L};
static long lDFTable[] = {1078920L,107892L,17982L,1798L,300L,30L,10L,1L};
static long l25Table[] = {900000L,90000L,15000L,1500L,250L,25L,10L,1L};
static long l24Table[] = {864000L,86400L,14400L,1440L,240L,24L,10L,1L};

//---------------------------------------------------------
//
//	ConvertStringToTimecode
//
// Timecode sample passed in contains desired framerate for conversion
//
//---------------------------------------------------------
DWORD
CTimecode::ConvertStringToTimecode(TCHAR *lpszTimecode, 
								   PTIMECODE_SAMPLE pTimecodeSample)
{
	int i, j;
	long *table;
	TCHAR temp;

	// select conversion table
	switch (pTimecodeSample->timecode.wFrameRate)
	{
		case AM_TIMECODE_30DROP:
			table = lDFTable;
			break;
		case AM_TIMECODE_30NONDROP:
			table = lNDFTable;
			break;
		case AM_TIMECODE_25:
			table = l25Table;
			break;
		case AM_TIMECODE_24:
			table = l24Table;
			break;
		default:
			table = lNDFTable;
	}
	pTimecodeSample->timecode.dwFrames = 0L;
	j = 0;
	for ( i = 0; lpszTimecode[i]!= 0; i++ )
	{
		if ( (lpszTimecode[i] > '9') || (lpszTimecode[i] < '0') )
			continue;
		temp = (lpszTimecode[i] - '0');
		pTimecodeSample->timecode.dwFrames += (DWORD)temp * table[j];
		j++;
	}
	return 0L;
}
//---------------------------------------------------------
//
//	ConvertTimecodeToString
//
//---------------------------------------------------------
DWORD 
CTimecode::ConvertTimecodeToString(PTIMECODE_SAMPLE pTimecodeSample, 
								   TCHAR *lpszTimecode)
{
	int i, j, temp;
	long timecode;
	long *table;
	WORD wFCM;
	int flag = 0;

	// make local copies
	wFCM = pTimecodeSample->timecode.wFrameRate;
	timecode = pTimecodeSample->timecode.dwFrames;
	
	// select conversion table
	switch (wFCM)
	{
		case AM_TIMECODE_30DROP:
			table = lDFTable;
			break;
		case AM_TIMECODE_30NONDROP:
			table = lNDFTable;
			break;
		case AM_TIMECODE_25:
			table = l25Table;
			break;
		case AM_TIMECODE_24:
			table = l24Table;
			break;
		default:
			table = lNDFTable;
	}
	for ( i=0, j=0; j<(TC_LENGTH); i++, j++ )
	{
		temp = (int)( timecode / table[i]);
		lpszTimecode[j] = temp + '0';	// ASCII conversion
		timecode -= temp * table[i];
		// if minutesx10, do the drop frame test
		if ( i == 2 ) {
			if ( timecode < 2)
				flag = 1;	// this means we are on a 10 minute
							// boundary and 2 frames should be 
							// added later
			else
				timecode-=2L;
		}
		if ( i == 3 ) {
			// this is where the 2 frames get added in
			if ( !flag )
				timecode+=2L;
		}
		// add punctuation
		if ( 1 == j || 4 == j || 7 == j )
		{
			j++;
			if ( 8 == j && wFCM == AM_TIMECODE_30DROP )
				lpszTimecode[j] = ';';
			else
				lpszTimecode[j] = ':';
		}
	}
	lpszTimecode[j] = 0;	// null terminate
	return 0L;
}

// eof ctimecod.c
