/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:               Dstrtime.c
 *  Content:    This source file contains a Multimedia Timer Callback
 *             procedure which is used by the DirectSound Stream Sample
 *             application to update the playback buffer at regular intervals
 *
 ***************************************************************************/
#define WIN32_LEAN_AND_MEAN
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

void CALLBACK TimeFunc( UINT uTimerID, UINT uMsg, DWORD dwUser,
                                                        DWORD dw1, DWORD dw2 )
    {
    static      bInTimer = FALSE;
    LPBYTE      lpWrite1, lpWrite2, lpTemp;
    DWORD       dwLen1, dwLen2, dwPlay, dwWrite;
    int         nChkErr;
    UINT        uChkErr;

    if( bInTimer )
        {
        return;
        }
    else
        {
        bInTimer = TRUE;
        }

    /* Get and print the current position of the play cursor */
    wiWave.lpDSBStreamBuffer->lpVtbl->GetCurrentPosition( 
                    wiWave.lpDSBStreamBuffer, &dwPlay, &dwWrite );
    
#ifdef DEBUG
    PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
                MAKEWPARAM( DEBUGF_PLAYPOSITION, 0 ), dwPlay );
#endif
    if(( wiWave.dwNextWriteOffset < dwPlay )
        && (( wiWave.dwNextWriteOffset + wiWave.dwBufferSegSize ) > dwPlay ))
        {
#ifdef DEBUG
        PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
                        MAKEWPARAM( DEBUGF_SKIP, 0 ), 0L );
#endif
        bInTimer = FALSE;
        return;
        }

#ifdef DEBUG
    PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
                    MAKEWPARAM( DEBUGF_WRITEPOSITION, 0 ),
                    wiWave.dwNextWriteOffset );
#endif

    /* Have we found the end of the file and passed the buffer end? */
    if( wiWave.bFoundEnd && !wiWave.nRemainingSegments )
        {
        if( !wiWave.bDonePlaying )
            {
            wiWave.bDonePlaying = TRUE;
            PostMessage( hWndMain, WM_DSSTREAM_DONE, (WPARAM)0, (LPARAM)0 );
            }
        bInTimer = FALSE;
        return;
        }

    if( dwPlay < wiWave.dwPlayLast )
        wiWave.dwProgress += (dwPlay + wiWave.dwBufferSize - wiWave.dwPlayLast);
    else
        wiWave.dwProgress += (dwPlay - wiWave.dwPlayLast);
    wiWave.dwPlayLast = dwPlay;
    PostMessage( hWndMain, WM_DSSTREAM_PROGRESS, 0L, 0L );

    /*
     *  If wiWave.bFoundEnd == TRUE, then we've finished reading in the file,
     * but we need to wait until the buffer's play cursor passes the point we
     * were at when we found out we were done reading.
     */
    if( wiWave.bFoundEnd && wiWave.nRemainingSegments )
        {
        wiWave.nRemainingSegments--;

        if( wiWave.nRemainingSegments > 1 ) {
        if( wiWave.lpDSBStreamBuffer->lpVtbl->Lock( wiWave.lpDSBStreamBuffer,
                                                wiWave.dwNextWriteOffset,
                                                wiWave.dwBufferSegSize,
                                                &((LPVOID)lpWrite1), &dwLen1,
                                                &((LPVOID)lpWrite2), &dwLen2,
                                                0 ) != 0 )
            {
            OutputDebugString( "TimeFunc() could not lock DirectSoundBuffer" );
            bInTimer = FALSE;
            return;
            }

        if( wiWave.pwfx->wBitsPerSample == 8 )
            _fmemset( lpWrite1, 128, dwLen1 );
        else if( wiWave.pwfx->wBitsPerSample == 16 )
            _fmemset( lpWrite1, 0, dwLen1 );
        wiWave.lpDSBStreamBuffer->lpVtbl->Unlock( wiWave.lpDSBStreamBuffer,
                                                    (LPVOID)lpWrite1, dwLen1,
                                                    (LPVOID)lpWrite2, dwLen2 );
    /*
     * This code is stolen from the end of the routine -- we need to keep
     * zeroing out buffer segments while we're waiting for the play cursor to
     * catch up to the end of the WAVE data.
     */
        wiWave.dwNextWriteOffset += wiWave.dwBufferSegSize;
        if( wiWave.dwNextWriteOffset >= wiWave.dwBufferSize )
            wiWave.dwNextWriteOffset -= wiWave.dwBufferSize;
        }
        bInTimer = FALSE;
        return;
        }

    /* Lock a segment of memory that is behind the play cursor */
    if( wiWave.lpDSBStreamBuffer->lpVtbl->Lock( wiWave.lpDSBStreamBuffer,
                                            wiWave.dwNextWriteOffset,
                                            wiWave.dwBufferSegSize,
                                            &((LPVOID)lpWrite1), &dwLen1,
                                            &((LPVOID)lpWrite2), &dwLen2,
                                            0 ) != 0 )
        {
        OutputDebugString( "TimeFunc() could not lock DirectSoundBuffer" );
        bInTimer = FALSE;
        return;
        }
        
    if( dwLen1 && !wiWave.bDonePlaying )
        {
        nChkErr = WaveReadFile( wiWave.hmmio, (UINT)dwLen1, lpWrite1,
                                &wiWave.mmck, &uChkErr );
        if( uChkErr < (UINT)dwLen1 )
            {
            if( !wiWave.bLoopFile )
                {
                /* Zero out the rest of this block */
                if( wiWave.pwfx->wBitsPerSample == 8 )
                    memset( lpWrite1+uChkErr, 128, ((UINT)dwLen1-uChkErr));
                else if( wiWave.pwfx->wBitsPerSample == 16 )
                    memset( lpWrite1+uChkErr, 0, ((UINT)dwLen1-uChkErr));
/* Enable play completion detection code at the beginning of the next call */
                wiWave.bFoundEnd = TRUE;
                if( dwPlay > wiWave.dwNextWriteOffset )
                    wiWave.nRemainingSegments = (wiWave.dwNextWriteOffset
                                                + wiWave.dwBufferSize - dwPlay)
                                                / wiWave.dwBufferSegSize;
                else
                    wiWave.nRemainingSegments = (wiWave.dwNextWriteOffset
                                                - dwPlay)
                                                / wiWave.dwBufferSegSize;
                }
             else
                {
                lpTemp = lpWrite1;
                do
                    {
                    /* Continue decrementing our count and moving our temp
                     * pointer forward until we've read the file enough times
                     * to fill the buffer.  NOTE: It's probably not efficient
                     * to bother with the overhead of streaming a file that's
                     * not at least as large as the buffer... */
                    lpTemp += uChkErr;
                    dwLen1 -= uChkErr;
                    nChkErr = WaveStartDataRead( &wiWave.hmmio,
                                                    &wiWave.mmck,
                                                    &wiWave.mmckInRIFF );
                    nChkErr = WaveReadFile( wiWave.hmmio, (UINT)dwLen1,
                                            lpTemp,
                                            &wiWave.mmck, &uChkErr );
                    } while( uChkErr < dwLen1 );
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
        _fmemset( lpWrite1,
                    wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0,
                    dwLen1);
        }

    if( dwLen2 && !wiWave.bDonePlaying )
        {
        nChkErr = WaveReadFile( wiWave.hmmio, (UINT)dwLen2, lpWrite2,
                                &wiWave.mmck, &uChkErr );
        if( uChkErr < (UINT)dwLen2 )
            {
            if( !wiWave.bLoopFile )
                {
                /* Zero out the rest of this block */
                if( wiWave.pwfx->wBitsPerSample == 8 )
                    memset( lpWrite2+uChkErr, 128, ((UINT)dwLen2-uChkErr));
                else if( wiWave.pwfx->wBitsPerSample == 16 )
                    memset( lpWrite2+uChkErr, 0, ((UINT)dwLen2-uChkErr));
                /* Enable play completion detection code at the beginning
                 * of the next call
                 */
                wiWave.bFoundEnd = TRUE;
                if( dwPlay > wiWave.dwNextWriteOffset )
                    wiWave.nRemainingSegments = (wiWave.dwNextWriteOffset
                                                + wiWave.dwBufferSize - dwPlay)
                                                / wiWave.dwBufferSegSize;
                else
                    wiWave.nRemainingSegments = (wiWave.dwNextWriteOffset
                                                - dwPlay)
                                                / wiWave.dwBufferSegSize;
                }
            else
                {
                lpTemp = lpWrite2;
                do
                    {
                    /* Continue decrementing our count and moving our temp
                     * pointer forward until we've read the file enough times
                     * to fill the buffer.  NOTE: It's probably not efficient
                     * to bother with the overhead of streaming a file that's
                     * not at least as large as the buffer... */
                    lpTemp += uChkErr;
                    dwLen2 -= uChkErr;
                    nChkErr = WaveStartDataRead( &wiWave.hmmio,
                                                    &wiWave.mmck,
                                                    &wiWave.mmckInRIFF );
                    nChkErr = WaveReadFile( wiWave.hmmio, (UINT)dwLen2,
                                            lpTemp,
                                            &wiWave.mmck, &uChkErr );
                    } while( uChkErr < dwLen2 );
                }
            }
        }
    else if( dwLen2 && wiWave.bDonePlaying )
        {
        // Set the appropriate silence value
        _fmemset( lpWrite2,
                    wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0,
                    dwLen2 );
        }

    wiWave.lpDSBStreamBuffer->lpVtbl->Unlock( wiWave.lpDSBStreamBuffer,
                                                (LPVOID)lpWrite1, dwLen1,
                                                (LPVOID)lpWrite2, dwLen2 );

    wiWave.dwNextWriteOffset += wiWave.dwBufferSegSize;
    if( wiWave.dwNextWriteOffset >= wiWave.dwBufferSize )
        wiWave.dwNextWriteOffset -= wiWave.dwBufferSize;

#ifdef DEBUG
    PostMessage( hWndMain, WM_DSSTREAM_DEBUG,
                    MAKEWPARAM( DEBUGF_NEXTWRITE, 0 ),
                    wiWave.dwNextWriteOffset );
#endif
    bInTimer = FALSE;
    return;
    }
