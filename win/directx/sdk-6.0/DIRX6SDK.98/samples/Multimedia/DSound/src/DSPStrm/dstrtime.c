/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:               Dstrtime.c
 *  Content:    This source file contains a Multimedia Timer Callback
 *             procedure which is used by the DirectSound Stream Sample
 *             application to update the playback buffer at regular intervals
 *
 ***************************************************************************/
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include <commctrl.h>
#include <commdlg.h>
#include <memory.h>
#include <cderr.h>

#include "dsstream.h"

extern HWND  hWndMain;
extern char  szDebug[];
extern WAVEINFOCA wiWave;

LONG lInTimer = FALSE;

void CALLBACK TimeFunc( UINT uTimerID, UINT uMsg, DWORD dwUser,
							DWORD dw1, DWORD dw2 )
    {
    LPBYTE      lpWrite1, lpWrite2, lpTemp;
    DWORD       dwLen1, dwLen2, dwPlay, dwWrite, dwPlayedLength, cbCopyTemp;
    DWORD       dwWriteLength;
    int         nChkErr;
    UINT        uActualBytesWritten;
    DWORD       dwStatus;
    BOOL        fRefillLostBuffer = FALSE;
    HRESULT     hr;

    if (InterlockedExchange(&lInTimer, TRUE)) return;

    /* See if the buffer has been lost */
    IDirectSoundBuffer_GetStatus( wiWave.lpDSBStreamBuffer, &dwStatus);
    if (DSBSTATUS_BUFFERLOST & dwStatus) {
	//
	// Restore the buffer, reset some variables, and play it again
	//
	hr = IDirectSoundBuffer_Restore( wiWave.lpDSBStreamBuffer );
	if (DS_OK != hr) {
	    InterlockedExchange(&lInTimer, FALSE);
	    return;
	}

	wiWave.dwNextWriteOffset = 0;

	// Setting this flag causes the buffer to be filled with enough data to make it
	// until the next timer event (we can't Play the restored buffer until we have
	// restored the data that was in it).
	fRefillLostBuffer = TRUE;
    }

    /* Get and print the current position of the play cursor */
    IDirectSoundBuffer_GetCurrentPosition( wiWave.lpDSBStreamBuffer,
					   &dwPlay, &dwWrite );

    /* If the play cursor is at the same spot as the last call, there are two
     * possibilities.  The first is that we were called extremely late and
     * happened to land on an integer number of complete buffer cycles.  This
     * is not very likely.  The other is that the play cursor didn't move.
     * Since we're oversampling, this is very likely.  In this case, we should
     * bail.
     */

    if( dwPlay == wiWave.dwNextWriteOffset && !fRefillLostBuffer)
	{
	InterlockedExchange(&lInTimer, FALSE);
	return;
	}
#ifdef DEBUG
    PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
		MAKEWPARAM( DEBUGF_PLAYPOSITION, 0 ), dwPlay );
#endif

#ifdef DEBUG
    PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
		    MAKEWPARAM( DEBUGF_WRITEPOSITION, 0 ),
		    wiWave.dwNextWriteOffset );
#endif

    /* Have we found the end of the file and passed the buffer end? */
    if( wiWave.bFoundEnd && !wiWave.cbBytesRemaining )
	{
	if( !wiWave.bDonePlaying )
	    {
	    wiWave.bDonePlaying = TRUE;
	    PostMessage( hWndMain, WM_DSSTREAM_DONE, (WPARAM)0, (LPARAM)0 );
	    }
	InterlockedExchange(&lInTimer, FALSE);
	return;
	}

    if( dwPlay < wiWave.dwNextWriteOffset )
	{
	/* Calculate how many writeable bytes there are behind the play cursor */
	dwPlayedLength = (dwPlay + wiWave.dwBufferSize - wiWave.dwNextWriteOffset);
	}
    else
	{
	/* Calculate how many writeable bytes there are behind the play cursor */
	dwPlayedLength = (dwPlay - wiWave.dwNextWriteOffset);
	}

    // The number of bytes we want to write may not be equal to the # of bytes
    // that were played since the last timer proc.  One such case is when we're
    // restoring a lost buffer.
    if( fRefillLostBuffer )
	dwWriteLength = wiWave.dwBufferSize;
    else
	dwWriteLength = dwPlayedLength;

    wiWave.dwProgress += dwPlayedLength;

    PostMessage( hWndMain, WM_DSSTREAM_PROGRESS, 0L, wiWave.dwProgress );

    /*
     *  If wiWave.bFoundEnd == TRUE, then we've finished reading in the file,
     * but we need to wait until the buffer's play cursor passes the point we
     * were at when we found out we were done reading.
     */
    if( wiWave.bFoundEnd && wiWave.cbBytesRemaining )
	{
	// Decrement the count of how many bytes we have to wait before
	// before we can safely shut down the playing buffer and end the
	// timer procedure callbacks
	if( dwPlayedLength > wiWave.cbBytesRemaining )
	    wiWave.cbBytesRemaining = 0;
	else
	    wiWave.cbBytesRemaining -= dwPlayedLength;

	hr = IDirectSoundBuffer_Lock(wiWave.lpDSBStreamBuffer,
					 wiWave.dwNextWriteOffset,
					 dwWriteLength,
					 &((LPVOID)lpWrite1), &dwLen1,
					 &((LPVOID)lpWrite2), &dwLen2,
					 0 );
	if (DS_OK != hr)
	    {
		// OutputDebugString( "TimeFunc() could not lock DirectSoundBuffer" );
		InterlockedExchange(&lInTimer, FALSE);
		return;
	    }

	/* Silence out both parts of the locked buffer */
		FillMemory( lpWrite1, dwLen1,
					(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0 ));

	if( lpWrite2 && dwLen2 )
			FillMemory( lpWrite2, dwLen2,
					(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0 ));

	wiWave.lpDSBStreamBuffer->lpVtbl->Unlock( wiWave.lpDSBStreamBuffer,
		(LPVOID)lpWrite1, dwLen1,
		(LPVOID)lpWrite2, dwLen2 );
	/*
	 * This code is stolen from the end of the routine -- we need to keep
	 * silencing out the buffer while we're waiting for the play cursor to
	 * catch up to the end of the WAVE data.
	 */
	wiWave.dwNextWriteOffset += dwWriteLength;
	if( wiWave.dwNextWriteOffset >= wiWave.dwBufferSize )
	    wiWave.dwNextWriteOffset -= wiWave.dwBufferSize;
#ifdef DEBUG
	PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
			MAKEWPARAM( DEBUGF_NEXTWRITE, 0 ),
			    wiWave.dwNextWriteOffset );
#endif
	if( fRefillLostBuffer )
	    IDirectSoundBuffer_Play( wiWave.lpDSBStreamBuffer, 0, 0, DSBPLAY_LOOPING );
	InterlockedExchange(&lInTimer, FALSE);
	return;
	}

    /* Lock a segment of memory that is behind the play cursor */
    hr = IDirectSoundBuffer_Lock( wiWave.lpDSBStreamBuffer,
				  wiWave.dwNextWriteOffset,
				  dwWriteLength,
				  &((LPVOID)lpWrite1), &dwLen1,
				  &((LPVOID)lpWrite2), &dwLen2,
				  0 );
    if (DS_OK != hr)
    {
	// OutputDebugString( "TimeFunc() could not lock DirectSoundBuffer" );
	InterlockedExchange(&lInTimer, FALSE);
	return;
    }

    if( dwLen1 && !wiWave.bDonePlaying )
	{
	nChkErr = WaveReadFile( wiWave.hmmio, (UINT)dwLen1, lpWrite1,
				&wiWave.mmck, &uActualBytesWritten );
	if( (DWORD)uActualBytesWritten < dwLen1 )
	    {
	    if( !wiWave.bLoopFile )
		{
		/* Silence out the rest of this block */
		FillMemory( lpWrite1+uActualBytesWritten, dwLen1-uActualBytesWritten,
				(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0 ));
/* Enable play completion detection code at the beginning of the next call */
		wiWave.bFoundEnd = TRUE;
		if( dwPlay > wiWave.dwNextWriteOffset )
		    wiWave.cbBytesRemaining = wiWave.dwNextWriteOffset
						+ wiWave.dwBufferSize - dwPlay;
		else
		    wiWave.cbBytesRemaining = wiWave.dwNextWriteOffset - dwPlay;
		}
	     else
		{
		cbCopyTemp = dwLen1;
		lpTemp = lpWrite1;
		do
		    {
		    /* Continue decrementing our count and moving our temp
		     * pointer forward until we've read the file enough times
		     * to fill the buffer.  NOTE: It's probably not efficient
		     * to bother with the overhead of streaming a file that's
		     * not at least as large as the buffer... */
		    lpTemp += uActualBytesWritten;
		    cbCopyTemp -= uActualBytesWritten;
		    nChkErr = WaveStartDataRead( &wiWave.hmmio,
						    &wiWave.mmck,
						    &wiWave.mmckInRIFF );
		    nChkErr = WaveReadFile( wiWave.hmmio, (UINT)cbCopyTemp,
					    lpTemp,
					    &wiWave.mmck, &uActualBytesWritten );
		    } while( uActualBytesWritten < cbCopyTemp );
		}
	    }
	}
    /*
    * The bDonePlaying flag is set by the caller if the user stops playback
    * before the end of the WAVE file is encountered.  It tells us to cut this
    * racket out and play nothing in case it takes the caller a couple
    * interrupts to shut off the timer.
    */
    else if( dwLen1 && wiWave.bDonePlaying )
	{
	// Set the appropriate silence value
	FillMemory( lpWrite1, dwLen1,
			(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0 ));
	}

    if( dwLen2 && !wiWave.bDonePlaying )
	{
	nChkErr = WaveReadFile( wiWave.hmmio, (UINT)dwLen2, lpWrite2,
				&wiWave.mmck, &uActualBytesWritten );
	if( (DWORD)uActualBytesWritten < dwLen2 )
	    {
	    if( !wiWave.bLoopFile )
		{
		// Set the appropriate silence value
		FillMemory( lpWrite2, dwLen2,
				(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0 ));
		/* Enable play completion detection code at the beginning
		 * of the next call
		 */
		wiWave.bFoundEnd = TRUE;
		if( dwPlay > wiWave.dwNextWriteOffset )
		    wiWave.cbBytesRemaining = wiWave.dwNextWriteOffset
						+ wiWave.dwBufferSize - dwPlay;
		else
		    wiWave.cbBytesRemaining = wiWave.dwNextWriteOffset - dwPlay;
		}
	    else
		{
		lpTemp = lpWrite2;
		cbCopyTemp = dwLen2;
		do
		    {
		    /* Continue decrementing our count and moving our temp
		     * pointer forward until we've read the file enough times
		     * to fill the buffer.  NOTE: It's probably not efficient
		     * to bother with the overhead of streaming a file that's
		     * not at least as large as the buffer... */
		    lpTemp += uActualBytesWritten;
		    cbCopyTemp -= uActualBytesWritten;
		    nChkErr = WaveStartDataRead( &wiWave.hmmio,
						    &wiWave.mmck,
						    &wiWave.mmckInRIFF );
		    nChkErr = WaveReadFile( wiWave.hmmio, (UINT)cbCopyTemp,
					    lpTemp,
					    &wiWave.mmck, &uActualBytesWritten );
		    } while( uActualBytesWritten < cbCopyTemp );
		}
	    }
	}
    else if( dwLen2 && wiWave.bDonePlaying )
	{
	// Set the appropriate silence value
	FillMemory( lpWrite2, dwLen2,
			(BYTE)(wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0 ));
	}

    wiWave.lpDSBStreamBuffer->lpVtbl->Unlock( wiWave.lpDSBStreamBuffer,
						(LPVOID)lpWrite1, dwLen1,
						(LPVOID)lpWrite2, dwLen2 );

    wiWave.dwNextWriteOffset += dwWriteLength;
    if( wiWave.dwNextWriteOffset >= wiWave.dwBufferSize )
	wiWave.dwNextWriteOffset -= wiWave.dwBufferSize;

#ifdef DEBUG
    PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
		    MAKEWPARAM( DEBUGF_NEXTWRITE, 0 ),
		    wiWave.dwNextWriteOffset );
#endif
    if( fRefillLostBuffer )
	IDirectSoundBuffer_Play( wiWave.lpDSBStreamBuffer, 0, 0, DSBPLAY_LOOPING );
    InterlockedExchange(&lInTimer, FALSE);
    return;
    }

