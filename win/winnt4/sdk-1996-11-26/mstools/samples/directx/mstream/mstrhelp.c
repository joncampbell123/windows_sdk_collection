/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   mstream.c
 *  Content:   Illustrates streaming data from a disk MIDI file to a
 *             midiStream buffer for playback.
 *
 ***************************************************************************/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <memory.h>
#include <mmreg.h>

#include "debug.h"
#include "resource.h"
#include "midstuff.h"
#include "mstream.h"

extern char szAppTitle[64];
extern char szAppCaption[64];
extern char szFileBuffer[64];
extern char szFileTitle[64];
extern char szTempo[64];
extern char szVolume[64];
extern char szProgress[64];
extern char szTemp[256];
extern char szDebug[256];

extern HWND hWndMain, hWndTempo, hWndVol, hWndProg, hWndPlay, hWndPause;
extern HWND hWndStop, hWndTempoText, hWndVolText, hWndLoopCheck, hWndProgText;
extern HINSTANCE hInst;
extern HMIDISTRM    hStream;

extern BOOL bFileOpen, bPlaying, bPaused, bInsertTempo;
extern int  nTextControlHeight;
extern DWORD    dwBufferTickLength, dwTempoMultiplier, dwCurrentTempo;
extern DWORD    dwProgressBytes, dwVolumePercent, dwVolCache[NUM_CHANNELS];

#ifdef DEBUG
extern HWND hWndList;
#endif

/*****************************************************************************/
/* CreateChildren()                                                          */
/*                                                                           */
/*   This function creates a bunch of child controls for the main window.    */
/* Most of them are used for controling various things about a playing sound */
/* file, like volume and panning. Returns FALSE if no errors, TRUE otherwise.*/
/*                                                                           */
/*****************************************************************************/
int CreateChildren( RECT crect )
    {
    SIZE  Size;
    HDC   hDC;
    int   x, y;
    UINT  uType;
    char  szTemplate[128], szType[32];
    LPSTR lpszControl;

    LoadString( hInst, IDS_ERROR_CHILDTEMPLATE, szTemplate, sizeof(szTemplate));

    /* Don't handle failure for this one, because the app will still run fine */
    CreateWindow( "static", NULL, WS_CHILD | WS_VISIBLE | SS_ETCHEDHORZ,
            0, 0, crect.right, 2, hWndMain, (HMENU)0, hInst, NULL );

    hDC = GetDC( hWndMain );
    if( !GetTextExtentPoint32( hDC, szProgress, strlen(szProgress), &Size ))
    {
    ErrorMessageBox( IDS_ERROR_GETTEXTEXTENT, MB_ICONEXCLAMATION );
    ReleaseDC( hWndMain, hDC );
    return( TRUE );
    }
    ReleaseDC( hWndMain, hDC );
    nTextControlHeight = Size.cy;

    y = BORDER_SPACE_CY;

    /* STATIC control -- text label for the TEMPO trackbar */
    if(( hWndTempoText = CreateWindow( "static", szTempo,
                                    WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
                                    BORDER_SPACE_CX,
                                    y,
                                    TEMPO_TEXT_CX, nTextControlHeight,
                                    hWndMain, (HMENU)0, hInst, NULL)) == NULL )
        {
        lpszControl = szTempo;
    uType = IDS_ERROR_STATICTEXT;
        goto DISPLAY_CREATE_FAILURE;
    }

    /* Create the TEMPO trackbar */
    if(( hWndTempo = CreateWindow( TRACKBAR_CLASS, NULL,
                WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_BOTTOM,
                BORDER_SPACE_CX,
                y + nTextControlHeight + TEXT_SPACE_CY,
                TEMPO_TB_CX, TEMPO_TB_CY,
                hWndMain, (HMENU)0, hInst, NULL)) == NULL )
        {
        lpszControl = szTempo;
    uType = IDS_ERROR_TRACKBAR;
        goto DISPLAY_CREATE_FAILURE;
    }

    dwTempoMultiplier = 100;
    SendMessage( hWndTempo, TBM_SETRANGE, FALSE,
                                        MAKELONG( TEMPO_MIN, TEMPO_MAX ));
    SendMessage( hWndTempo, TBM_SETPOS, TRUE, dwTempoMultiplier );

    /* STATIC control -- text label for the VOLUME trackbar */
    if(( hWndVolText = CreateWindow( "static", szVolume,
                                    WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 
                                    BORDER_SPACE_CX + TEMPO_TB_CX
                                    + CONTROL_SPACE_CX,
                                    y,
                                    VOL_TEXT_CX, nTextControlHeight,
                                    hWndMain, (HMENU)0, hInst, NULL)) == NULL )
        {
        lpszControl = szVolume;
    uType = IDS_ERROR_STATICTEXT;
        goto DISPLAY_CREATE_FAILURE;
    }
    y += nTextControlHeight + TEXT_SPACE_CY;

    /* Create the VOLUME trackbar */
    if(( hWndVol = CreateWindow( TRACKBAR_CLASS, NULL,
                WS_CHILD | WS_VISIBLE | TBS_HORZ | TBS_BOTTOM,
                BORDER_SPACE_CX + TEMPO_TB_CX
                + CONTROL_SPACE_CX,
                y, VOL_TB_CX, VOL_TB_CY,
                hWndMain, (HMENU)0, hInst, NULL)) == NULL )
        {
        lpszControl = szVolume;
    uType = IDS_ERROR_TRACKBAR;
        goto DISPLAY_CREATE_FAILURE;
    }

    SendMessage( hWndVol, TBM_SETRANGE, FALSE,
                                        MAKELONG( VOL_TB_MIN, VOL_TB_MAX ));
    SendMessage( hWndVol, TBM_SETPOS, TRUE, VOL_TB_MAX );
    SendMessage( hWndVol, TBM_SETPAGESIZE, 0L, VOL_PAGESIZE );

    x = BORDER_SPACE_CX + TEMPO_TB_CX + 2 * CONTROL_SPACE_CX + VOL_TB_CX;
    y = BORDER_SPACE_CY;

    /* Create the LOOPED CHECKBOX */
    LoadString( hInst, IDS_CHECK_LOOPED, szTemp, sizeof(szTemp));
    if(( hWndLoopCheck = CreateWindow( "button", szTemp,
                                WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
                                x, y, CHECK_CX, CHECK_CY, hWndMain,
                                (HMENU)IDC_LOOPCHECK, hInst, NULL )) == NULL )
        {
        lpszControl = szTemp;
    uType = IDS_ERROR_CHECK;
        goto DISPLAY_CREATE_FAILURE;
    }

    x = BORDER_SPACE_CX + TEMPO_TB_CX + VOL_TB_CX + 2 * CONTROL_SPACE_CX + CHECK_CX
    - 3 * BUTTON_CX - 2 * BUTTON_SPACE_CX;
    y = BORDER_SPACE_CY + nTextControlHeight + TEXT_SPACE_CY + TEMPO_TB_CY
    + CONTROL_SPACE_CY;

    /* STATIC control -- text label for the progress trackbar. */
    if(( hWndProgText = CreateWindow( "static", szProgress,
                                    WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
                                    BORDER_SPACE_CX,
                                    y,
                                    x - BORDER_SPACE_CX, nTextControlHeight,
                                    hWndMain, (HMENU)0, hInst, NULL)) == NULL )
        {
        lpszControl = szProgress;
    uType = IDS_ERROR_STATICTEXT;
        goto DISPLAY_CREATE_FAILURE;
    }

    /* Create the PLAY BUTTON */
    LoadString( hInst, IDS_BUTTON_PLAY, szTemp, sizeof(szTemp));
    if(( hWndPlay = CreateWindow( "button", szTemp,
                                    WS_CHILD | WS_VISIBLE | WS_DISABLED,
                                    x, y, BUTTON_CX, BUTTON_CY, hWndMain,
                                    (HMENU)IDC_PLAY, hInst, NULL )) == NULL )
        {
        lpszControl = szTemp;
    uType = IDS_ERROR_BUTTON;
        goto DISPLAY_CREATE_FAILURE;
    }
    x += BUTTON_CX + BUTTON_SPACE_CX;

    /* Create the PAUSE BUTTON */
    LoadString( hInst, IDS_BUTTON_PAUSE, szTemp, sizeof(szTemp));
    if(( hWndPause = CreateWindow( "button", szTemp,
                    WS_CHILD | WS_VISIBLE | WS_DISABLED,
                    x, y, BUTTON_CX, BUTTON_CY, hWndMain,
                    (HMENU)IDC_PAUSE, hInst, NULL )) == NULL )
    {
    lpszControl = szTemp;
    uType = IDS_ERROR_BUTTON;
    goto DISPLAY_CREATE_FAILURE;
    }
    x += BUTTON_CX + BUTTON_SPACE_CX;

    /* Create the STOP BUTTON */
    LoadString( hInst, IDS_BUTTON_STOP, szTemp, sizeof(szTemp));
    if(( hWndStop = CreateWindow( "button", szTemp,
                                    WS_CHILD | WS_VISIBLE | WS_DISABLED,
                                    x, y, BUTTON_CX, BUTTON_CY, hWndMain,
                                    (HMENU)IDC_STOP, hInst, NULL )) == NULL )
        {
        lpszControl = szTemp;
    uType = IDS_ERROR_BUTTON;
        goto DISPLAY_CREATE_FAILURE;
    }

    UpdateFromControls();
    goto RETURN_NORMAL;

DISPLAY_CREATE_FAILURE:
    LoadString( hInst, uType, szType, sizeof(szType));
    wsprintf( szTemp, szTemplate, lpszControl, szType );
    MessageBox( GetActiveWindow(), szTemp,
                        szAppTitle, MB_OK | MB_ICONEXCLAMATION );
    return( TRUE );

RETURN_NORMAL:
    return( FALSE );
    }


/********************************************************************************/
/* HandleTempoScroll()                                                          */
/*                                                                              */
/*   Handles the tempo trackbar scroll when a WM_HSCROLL is received.           */
/*                                                                              */
/********************************************************************************/
void HandleTempoScroll( int nCode, int nPos )
    {
    long  lTempo, lDelta;

    lTempo = (LONG)SendMessage( hWndTempo, TBM_GETPOS, (WPARAM)0, (LPARAM)0 );

    switch( nCode )
        {
        case TB_LINEUP:
            if( lTempo >= TEMPO_MIN-1 )
                lDelta = -1;
            break;
        case TB_LINEDOWN:
            if( lTempo <= TEMPO_MAX+1 )
                lDelta = 1;
            break;
        case TB_PAGEUP:
            if( lTempo >= TEMPO_MIN - TEMPO_PAGESIZE )
                lDelta = -TEMPO_PAGESIZE;
            break;
        case TB_PAGEDOWN:
            if( lTempo <= TEMPO_MAX + TEMPO_PAGESIZE )
                lDelta = TEMPO_PAGESIZE;
            break;
        case TB_ENDTRACK:
            return;
        default:
            lDelta = 0;
        }

    if( lDelta )
    {
    SendMessage( hWndTempo, TBM_SETPOS, TRUE, lTempo + lDelta );
    dwTempoMultiplier = (DWORD)( lTempo + lDelta );
    }
    else
    {
    SendMessage( hWndTempo, TBM_SETPOS, TRUE, (long)nPos );
    dwTempoMultiplier = (DWORD)nPos;
    }

    UpdateFromControls();
    }


/********************************************************************************/
/* HandleVolScroll()                                                            */
/*                                                                              */
/*   Handles the volume trackbar scrolling when a WM_HSCROLL is received.       */
/*                                                                              */
/********************************************************************************/
void HandleVolScroll( int nCode, int nPos )
    {
    long  lVol, lDelta;

    lVol = (LONG)SendMessage( hWndVol, TBM_GETPOS, (WPARAM)0, (LPARAM)0 );

    switch( nCode )
        {
        case TB_LINEDOWN:
            if( lVol <= VOL_TB_MAX - 1 )
                lDelta = 1;
            break;
        case TB_LINEUP:
            if( lVol >= VOL_TB_MIN + 1 )
                lDelta = -1;
            break;
        case TB_PAGEDOWN:
            if( lVol <= VOL_TB_MAX - VOL_PAGESIZE )
                lDelta = VOL_PAGESIZE;
            break;
        case TB_PAGEUP:
            if( lVol >= VOL_TB_MIN + VOL_PAGESIZE )
                lDelta = -VOL_PAGESIZE;
            break;
        case TB_ENDTRACK:
            return;
        default:
            lDelta = 0;
        }

    if( lDelta )
        SendMessage( hWndVol, TBM_SETPOS, TRUE, (lVol + lDelta));
    else
        SendMessage( hWndVol, TBM_SETPOS, TRUE, (long)nPos );

    UpdateFromControls();
    }


/********************************************************************************/
/* UpdateFromControls()                                                         */
/*                                                                              */
/*    This function gets all the required values from the DirectSoundBuffer and */
/* updates the screen interface controls.                                       */
/*                                                                              */
/********************************************************************************/
void UpdateFromControls( void )
    {
    long    lTempo;

    lTempo = (LONG)SendMessage( hWndTempo, TBM_GETPOS, (WPARAM)0, (LPARAM)0 );
    dwVolumePercent = (WORD)SendMessage( hWndVol, TBM_GETPOS,
                                (WPARAM)0, (LPARAM)0 );

    /* Set the Volume text */
    wsprintf( szTemp, "%s: %lu%%", szVolume, dwVolumePercent / 10 );
    Static_SetText( hWndVolText, szTemp );
    if( hStream )
    SetAllChannelVolumes( dwVolumePercent );

    /* Set the Tempo text */
    wsprintf( szTemp, "%s: %li%%", szTempo, lTempo );
    Static_SetText( hWndTempoText, szTemp );
    bInsertTempo = TRUE;

    /* Set the Progress text */
    wsprintf( szTemp, "%s: %lu bytes", szProgress, dwProgressBytes );
    Static_SetText( hWndProgText, szTemp );

    return;
    }


/****************************************************************************/
/* ErrorMessageBox()                                                        */
/*                                                                          */
/*   A little routine to load error messages from the string resource table */
/* and pop them up in a MessageBox() for the world to see. The dwMBFlags    */
/* parameter allows the caller to specify the type of icon to use.          */
/*                                                                          */
/****************************************************************************/
void ErrorMessageBox( UINT uID, DWORD dwMBFlags )
    {
    LoadString( hInst, uID, szTemp, sizeof(szTemp));
    MessageBox( GetActiveWindow(), szTemp, szAppTitle, MB_OK | dwMBFlags );
#ifdef DEBUG
    wsprintf( szDebug, "General error: %s", szTemp );
    DebugPrint( szDebug );
#endif
    }


/****************************************************************************/
/* MidiErrorMessageBox()                                                    */
/*                                                                          */
/*   Calls the midiOutGetErrorText() function and displays the text which   */
/* corresponds to a midi subsystem error code.                              */
/*                                                                          */
/****************************************************************************/
void MidiErrorMessageBox( MMRESULT mmr )
    {
    midiOutGetErrorText( mmr, szTemp, sizeof(szTemp));
    MessageBox( GetActiveWindow(), szTemp, szAppTitle,
                MB_OK | MB_ICONSTOP );
#ifdef DEBUG
    wsprintf( szDebug, "Midi subsystem error: %s", szTemp );
    DebugPrint( szDebug );
#endif
    }


/*****************************************************************************/
/* HandleCommDlgError()                                                      */
/*                                                                           */
/*    The function translates extended common dialog error codes into a      */
/* string resource ID, loads that string from our module, and displays it in */
/* a message box. This implementation only covers the general CD error codes.*/
/*                                                                           */
/*****************************************************************************/
int HandleCommDlgError( DWORD dwError )
    {
    char szTitle[128];
    UINT uMsgID;

    if( dwError == CDERR_DIALOGFAILURE )
        uMsgID = IDS_CDERR_DIALOGFAILURE;
    else
        uMsgID = (UINT)dwError + IDS_CDERR_GENERAL_BASE;

    LoadString( hInst, uMsgID, szTemp, sizeof(szTemp));
    LoadString( hInst, IDS_CDERR_TITLESTRING, szTitle, sizeof(szTitle));
    MessageBox( GetActiveWindow(), szTemp, szTitle,
                    MB_OK | MB_ICONEXCLAMATION );
        
    return( 0 );
    }


/****************************************************************************/
/* BuildTitleBarText()                                                      */
/*                                                                          */
/*   Helper function designed to updated the title bar text of the main     */
/* window to reflect the currently loaded file (if there is one).           */
/*                                                                          */
/****************************************************************************/
void BuildTitleBarText( void )
    {
    char szTitle[sizeof(szAppCaption) + MAX_PATH + sizeof( " -  (Paused)")];

    lstrcpy( szTitle, szAppCaption );
    if( bFileOpen )
    {
    lstrcat( szTitle, " - " );
    lstrcat( szTitle, szFileTitle );
    }
    if( bPaused )
        lstrcat( szTitle, " (Paused)" );
    SetWindowText( hWndMain, szTitle );
    }


/****************************************************************************/
/* SetAllChannelVolumes()                                                   */
/*                                                                          */
/*   Given a percent in tenths of a percent, sets volume on all channels to */
/* reflect the new value.                                                   */
/****************************************************************************/
void SetAllChannelVolumes( DWORD dwVolumePercent )
    {
    DWORD   dwEvent, dwStatus, dwVol, idx;
    MMRESULT    mmrRetVal;

    if( !bPlaying )
    return;


    for( idx = 0, dwStatus = MIDI_CTRLCHANGE; idx < NUM_CHANNELS; idx++,
                                dwStatus++ )
    {
    dwVol = ( dwVolCache[idx] * dwVolumePercent ) / 1000;
    dwEvent = dwStatus | ((DWORD)MIDICTRL_VOLUME << 8)
            | ((DWORD)dwVol << 16);
    if(( mmrRetVal = midiOutShortMsg( (HMIDIOUT)hStream, dwEvent ))
                            != MMSYSERR_NOERROR )
        {
        MidiErrorMessageBox( mmrRetVal );
        return;
        }
    }
    }


/****************************************************************************/
/* SetChannelVolume()                                                       */
/*                                                                          */
/*   Given a percent in tenths of a percent, sets volume on a specified     */
/* channel to reflect the new value.                                        */
/****************************************************************************/
void SetChannelVolume( DWORD dwChannel, DWORD dwVolumePercent )
    {
    DWORD   dwEvent, dwVol;
    MMRESULT    mmrRetVal;

    if( !bPlaying )
    return;

    dwVol = ( dwVolCache[dwChannel] * dwVolumePercent ) / 1000;
    dwEvent = MIDI_CTRLCHANGE | dwChannel | ((DWORD)MIDICTRL_VOLUME << 8)
                            | ((DWORD)dwVol << 16);
    if(( mmrRetVal = midiOutShortMsg( (HMIDIOUT)hStream, dwEvent ))
                            != MMSYSERR_NOERROR )
    {
    MidiErrorMessageBox( mmrRetVal );
    return;
    }
    }
