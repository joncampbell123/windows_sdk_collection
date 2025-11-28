/*==========================================================================
 *
 *  Copyright (C) 1995-1997 Microsoft Corporation. All Rights Reserved.
 *
 *  File:   mstream.c
 *  Content:   Illustrates streaming data from a disk MIDI file to a
 *             MIDI stream buffer for playback.
 *
 ***************************************************************************/
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <memory.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <stdio.h>

#include "resource.h"
#include "debug.h"
#include "midstuff.h"
#include "mstream.h"

//////////////////////////////////////////////////////////////////////////////
// Lots of global variables

char szAppClass[] = "MStreamWndClass";
char szAppName[]  = "MStream";

char szAppTitle[64];
char szAppCaption[64];
char szOpenFilter[128];
char szOpenDLGTitle[64];
char szProgress[64];
char szTempo[64];
char szVolume[64];

char szTemp[256];
char szDebug[256];
char szFileBuffer[MAX_PATH];
char szFileTitle[MAX_PATH];

HWND    hWndMain, hWndProgText, hWndProg, hWndVolText, hWndVol, hWndTempoText;
HWND    hWndTempo, hWndLoopCheck, hWndPlay, hWndPause, hWndStop;

HINSTANCE       hInst;

BOOL    bFileOpen = FALSE, bPlaying = FALSE, bBuffersPrepared = FALSE;
BOOL    bPaused = FALSE, bLooped = FALSE;
UINT    uMIDIDeviceID = MIDI_MAPPER, uCallbackStatus;
int     nTextControlHeight, nCurrentBuffer, nEmptyBuffers;
DWORD   dwBufferTickLength, dwTempoMultiplier, dwCurrentTempo, dwProgressBytes;
DWORD   dwVolumePercent, dwVolCache[NUM_CHANNELS];

HMIDISTRM    hStream;
CONVERTINFO  ciStreamBuffers[NUM_STREAM_BUFFERS];

// Private to this module...
static HANDLE   hBufferReturnEvent;

// From another module...
extern INFILESTATE      ifs;

///////////////////////////////////////////////////////////////////////////////
// Module-scope function declarations

static BOOL InitApp( HINSTANCE );
static BOOL InitInstance( HINSTANCE, int );
static void FreeBuffers( void );

/******************************************************************************
 * WinMain()
 *
 * Entry point for all Windows programs - performs initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow )
    {
    MSG     msg;

    hInst = hInstance;

    /* Make sure the common controls are loaded for our use */
    InitCommonControls();

// Turn debugging output on or off
#ifdef DEBUG
    DbgInitialize( TRUE );
#else
    DbgInitialize( FALSE );
#endif

    if( !hPrevInstance )
	if( !InitApp( hInstance ))
	    {
	    ErrorMessageBox( IDS_ERROR_APPINIT, MB_ICONSTOP );
	    return( FALSE );
	    }

    if( !InitInstance( hInstance, nCmdShow ))
	{
	ErrorMessageBox( IDS_ERROR_INSTANCEINIT, MB_ICONSTOP );
	return( FALSE );
	}

    while( GetMessage((LPMSG)&msg, NULL, 0, 0 ))
	{
	TranslateMessage( &msg );
	DispatchMessage( &msg );
	}

    UnregisterClass( szAppClass, hInstance );
    return( msg.wParam );
    } /* End of WinMain() */


/*****************************************************************************/
/* InitApp()                                                                 */
/*                                                                           */
/*   Inits things that only need to be created once for the this application */
/* (like creating the window class).                                         */
/*****************************************************************************/
static BOOL InitApp( HINSTANCE hInstance )
    {
    WNDCLASS    wc;

    /* Set up and register a window class */
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpszClassName    = szAppClass;
    wc.lpfnWndProc      = (WNDPROC)MainWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof( DWORD );
    wc.hInstance        = hInstance;
    wc.hIcon            = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON3 ));
    wc.hCursor          = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground    = (HBRUSH)( COLOR_WINDOW );
    wc.lpszMenuName     = MAKEINTRESOURCE( IDR_MAINMENU );

    if( !RegisterClass( &wc ))
	{
	ErrorMessageBox( IDS_ERROR_REGISTERCLASS, MB_ICONSTOP );
	return( FALSE );
	}
    return( TRUE );
    } /* End of InitApp() */


/*****************************************************************************/
/* InitInstance()                                                            */
/*                                                                           */
/* Performs initialization that must be done for each application instance.  */
/*                                                                           */
/*****************************************************************************/
static BOOL InitInstance( HINSTANCE hInstance, int nCmdShow )
    {
    HWND        hWnd;
    RECT        crect;
    UINT        uCharsRead;
    MMRESULT    mmrRetVal;

    LoadString( hInstance, IDS_APP_TITLE, szAppTitle, sizeof(szAppTitle));
    LoadString( hInstance, IDS_APP_CAPTION, szAppCaption, sizeof(szAppCaption));
    LoadString( hInstance, IDS_TBTITLE_VOLUME, szVolume, sizeof(szVolume));
    LoadString( hInstance, IDS_TBTITLE_TEMPO, szTempo, sizeof(szTempo));
    LoadString( hInstance, IDS_TBTITLE_PROGRESS,
					    szProgress, sizeof(szProgress));
    LoadString( hInstance, IDS_OPEN_DLGTITLE,
					szOpenDLGTitle, sizeof(szOpenDLGTitle));
/* This is a little trick designed to allow us to load a common dialog box
 * filter string, which is really a concatentation of several NULL-terminated
 * strings. Note that while is is possible to enter something else into the
 * resource as placeholders for the NULL characters, this has the undesireable
 * effect of forcing us to search-and-replace byte-by-byte and doesn't make it
 * as easy to internationalize our strings...
 */
    memset( szOpenFilter, 0, sizeof(szOpenFilter));
    uCharsRead = LoadString( hInstance, IDS_OPEN_FILTER1,
				szOpenFilter, sizeof(szOpenFilter)) + 1;
    uCharsRead += LoadString( hInstance, IDS_OPEN_FILTER2,
				&szOpenFilter[uCharsRead],
				sizeof(szOpenFilter) - uCharsRead ) + 1;
    uCharsRead += LoadString( hInstance, IDS_OPEN_FILTER3,
				&szOpenFilter[uCharsRead],
				sizeof(szOpenFilter) - uCharsRead ) + 1;
    LoadString( hInstance, IDS_OPEN_FILTER4,
				&szOpenFilter[uCharsRead],
				sizeof(szOpenFilter) - uCharsRead );

    /* Create an application window */
    hWnd = CreateWindow( szAppClass,            /* class name */
			szAppCaption,           /* caption for window */
			WS_OVERLAPPEDWINDOW,    /* style */
			CW_USEDEFAULT,          /* x position */
			CW_USEDEFAULT,          /* y position */
			CW_USEDEFAULT,          /* width */
			CW_USEDEFAULT,          /* height */
			NULL,                   /* parent window */
			NULL,                   /* menu */
			hInstance,              /* instance */
			NULL );                 /* parms */

    if( !hWnd )
	{
	ErrorMessageBox( IDS_ERROR_MAINWNDCREATE, MB_ICONSTOP );
	return( FALSE );
	}

    hWndMain = hWnd;
    GetClientRect( hWndMain, &crect );

    /* Create some controls for things like volume, tempo, progress, etc. */
    if( CreateChildren( crect ))
	return( FALSE );

    // Resize window, now that we know the height of the static text controls
    SetWindowPos( hWnd, NULL, 0, 0,
		    2 * BORDER_SPACE_CX + TEMPO_TB_CX + 2 * CONTROL_SPACE_CX
		    + VOL_TB_CX + CHECK_CX,
		    2 * BORDER_SPACE_CY + nTextControlHeight + TEXT_SPACE_CY
		    + CONTROL_SPACE_CY + BUTTON_CY + TEMPO_TB_CY,
		    SWP_NOMOVE | SWP_NOZORDER );

    ShowWindow( hWnd, nCmdShow );
    UpdateWindow( hWnd );

    if(( mmrRetVal = midiStreamOpen( &hStream,
				    &uMIDIDeviceID,
				    (DWORD)1, (DWORD)MidiProc,
				    (DWORD)0,
				    CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
	{
	MidiErrorMessageBox( mmrRetVal );
	return( FALSE );
	}

    return( TRUE );
    } /* End of InitInstance() */


/****************************************************************************/
/* MainWindowProc()                                                         */
/*                                                                          */
/*    Messages for our main window are handled here                         */
/*                                                                          */
/****************************************************************************/
LRESULT CALLBACK MainWindowProc( HWND hWnd, unsigned uMsg,
						WPARAM wParam, LPARAM lParam )
    {
    LPMINMAXINFO    lpMinMax;
    DWORD       dwCDErr = 0;
    BOOL        bResult = FALSE;
    MMRESULT    mmrRetVal;
    
    switch( uMsg )
	{
	case WM_CREATE:
	    hBufferReturnEvent = CreateEvent( NULL, FALSE,
					FALSE, "Wait For Buffer Return" );
	    break;

	case WM_MSTREAM_UPDATEVOLUME:
	    SetChannelVolume( wParam, dwVolumePercent );
	    break;

	case WM_MSTREAM_PROGRESS:
	    /* Set the Progress text */
	    wsprintf( szTemp, "%s: %lu bytes", szProgress, dwProgressBytes );
	    Static_SetText( hWndProgText, szTemp );
	    break;

	case WM_COMMAND:
	    switch( LOWORD( wParam ))
		{
		case IDM_FILE_OPEN:
		    {
		    OPENFILENAME        ofn;
			char				szFileBufferTemp[MAX_PATH];
    /*
     * Clear out and fill in an OPENFILENAME structure in preparation
     * for creating a common dialog box to open a file.
     */
		    memset( &ofn, 0, sizeof(OPENFILENAME));
		    ofn.lStructSize     = sizeof(OPENFILENAME);
		    ofn.hwndOwner       = hWnd;
		    ofn.hInstance       = hInst;
		    ofn.lpstrFilter     = szOpenFilter;
		    ofn.nFilterIndex    = 1;
		    szFileBufferTemp[0] = '\0';
		    ofn.lpstrFile       = szFileBufferTemp;
		    ofn.nMaxFile        = sizeof(szFileBufferTemp);
		    ofn.lpstrFileTitle  = szFileTitle;
		    ofn.nMaxFileTitle   = sizeof(szFileTitle);
		    ofn.lpstrDefExt     = "MID";
		    ofn.lpstrTitle      = szOpenDLGTitle;
		    ofn.Flags           = OFN_FILEMUSTEXIST;

			// get the initial directory from the registry settings:
			{
				static TCHAR strInitialDir[512];
				HKEY   key;
				TCHAR  strPath[512];
				DWORD  type, size = 512;
				
				if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
									    TEXT("Software\\Microsoft\\DirectX"),
									    0, KEY_READ, &key ) )
				{
					if( ERROR_SUCCESS == RegQueryValueEx( key, 
						                    TEXT("DX6SDK Samples Path"), NULL,
											&type, (BYTE*)strPath, &size ) )
					{
						sprintf( strInitialDir, TEXT("%s\\DSound\\Media"), strPath );
						ofn.lpstrInitialDir = strInitialDir;
					}
					RegCloseKey( key );
				}
			}

		    bResult = GetOpenFileName( &ofn ); /* Do the dialog box */

    /*
     *  A return of TRUE indicates that the user did not select a filename.
     * The possible reasons are: Cancel was clicked, or an error occured.
     * If Cancel was clicked, the CommDlgExtendedError() function will not
     * return a valid code.  For anything else, an error code will come back.
     */
		    if( bResult == FALSE )
			{
			dwCDErr = CommDlgExtendedError();
			if( dwCDErr )
			    {
			    /* Handle a common dialog box error */
			    HandleCommDlgError( dwCDErr );
			    }
			else    /* Clicked Cancel, so finish msg processing */
			    return( 0 );
			}
		    else
			{
		    memcpy( szFileBuffer, szFileBufferTemp, sizeof(szFileBuffer));
			if( bFileOpen )
			    {
// Need to close the previous file before we open a new one.  The best way to
// do this is by faking a menu command, so that we only have the actual code in
// one place and it can easily be changed.
			    SendMessage( hWnd, WM_COMMAND,
					MAKEWPARAM( IDM_FILE_CLOSE, 0 ), 0L );
			    }
					
			if( StreamBufferSetup())
			    {
			    // Error opening the MIDI file so abort
			    // The function already took care of notification
			    break;
			    }
			else
			    {
			    bFileOpen = TRUE;
			    EnableWindow( hWndPlay, TRUE );
			    BuildTitleBarText();
			    }
			}
		    }
		    break;

		case IDM_FILE_CLOSE:
		    SendMessage( hWnd, WM_COMMAND,
					MAKEWPARAM( IDC_STOP,
					MSTREAM_STOPF_NOREOPEN ), 0L );
		    BuildTitleBarText();
		    break;

		case IDM_HELP_ABOUT:
		    DialogBox( hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndMain,
				(DLGPROC)DLG_About );
		    break;

		case IDM_ACTIONS_PAUSE:
		case IDC_PAUSE:
		    if( bPaused )
			midiStreamRestart( hStream );
		    else
			midiStreamPause( hStream );
		    bPaused = !bPaused;
		    // If we're paused, the title bar will show (Paused)
		    BuildTitleBarText();
		    break;

		case IDM_ACTIONS_LOOPED:
		case IDC_LOOPCHECK:
		    Button_SetCheck( hWndLoopCheck, !bLooped );
		    bLooped = !bLooped;
		    break;

		case IDM_ACTIONS_PLAY:
		case IDC_PLAY:
		    // Clicking play while playback is paused will un-pause it
		    if( bPaused )
			{
			SendMessage( hWnd, WM_COMMAND,
					MAKEWPARAM( IDC_PAUSE, 0 ), 0L );
			break;
			}
		    // Clicking play while playing will restart from scratch
		    if( bPlaying )
			{
			// Stop the file, allowing it to bve reset so that we
			// can start it over again from the beginning
			SendMessage( hWnd, WM_COMMAND,
					MAKEWPARAM( IDC_STOP, 0 ), 0L );
			}

		    if( bFileOpen )
			{
			// Clear the status of our callback so it will handle
			// MOM_DONE callbacks once more
			uCallbackStatus = 0;
			if(( mmrRetVal = midiStreamRestart( hStream ))
							!= MMSYSERR_NOERROR )
			    {
			    MidiErrorMessageBox( mmrRetVal );
			    break;
			    }
			}
		    else
			{
			bPlaying = FALSE;
			break;
			}

		    bPlaying = TRUE;
		    EnableWindow( hWndPause, TRUE );
		    EnableWindow( hWndStop, TRUE );
		    break;

		case IDM_ACTIONS_STOP:
		case IDC_STOP:
		    if( bFileOpen || bPlaying
				|| ( uCallbackStatus != STATUS_CALLBACKDEAD ))
			{
			EnableWindow( hWndStop, FALSE );
			EnableWindow( hWndPause, FALSE );
			bPlaying = bPaused = FALSE;
			if( uCallbackStatus != STATUS_CALLBACKDEAD && uCallbackStatus != STATUS_WAITINGFOREND )
			    uCallbackStatus = STATUS_KILLCALLBACK;

			if(( mmrRetVal = midiStreamStop( hStream ))
							!= MMSYSERR_NOERROR )
			    {
			    MidiErrorMessageBox( mmrRetVal );
			    break;
			    }
			if(( mmrRetVal = midiOutReset( (HMIDIOUT)hStream ))
							!= MMSYSERR_NOERROR )
			    {
			    MidiErrorMessageBox( mmrRetVal );
			    break;
			    }
// Wait for the callback thread to release this thread, which it will do by
// calling SetEvent() once all buffers are returned to it
			if( WaitForSingleObject( hBufferReturnEvent,
						DEBUG_CALLBACK_TIMEOUT )
							    == WAIT_TIMEOUT )
			    {
// Note, this is a risky move because the callback may be genuinely busy, but
// when we're debugging, it's safer and faster than freezing the application,
// which leaves the MIDI device locked up and forces a system reset...
			    DebugPrint( "Timed out waiting for MIDI callback" );
			    uCallbackStatus = STATUS_CALLBACKDEAD;
			    }
			}

		    if( uCallbackStatus == STATUS_CALLBACKDEAD )
			{
			uCallbackStatus = 0;
			if( bFileOpen )
			    {
			    ConverterCleanup();
			    FreeBuffers();
			    if( hStream )
				{
				if(( mmrRetVal = midiStreamClose( hStream ))
							!= MMSYSERR_NOERROR )
				    {
				    MidiErrorMessageBox( mmrRetVal );
				    }
				hStream = NULL;
				}

			    EnableWindow( hWndPlay, FALSE );
			    bFileOpen = FALSE;
			    }
			
			if(!( HIWORD(wParam) & MSTREAM_STOPF_NOREOPEN ))
			    {
			    if( StreamBufferSetup())
				{
				// Error setting up for MIDI file
				// Notification is already taken care of...
				break;
				}
			    else
				{
				bFileOpen = TRUE;
				EnableWindow( hWndPlay, TRUE );
				}
			    }
			BuildTitleBarText();    // Update the title bar
			}
		    break;

		case IDM_FILE_EXIT:
		    DestroyWindow( hWnd );
		    break;
		}
	    break;

	case WM_INITMENU:
	    if( bFileOpen )
		{
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_PLAY,
					    MF_BYCOMMAND | MF_ENABLED );
		EnableMenuItem( GetMenu( hWnd ), IDM_FILE_CLOSE,
					    MF_BYCOMMAND | MF_ENABLED );
		}
	    else
		{
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_PLAY,
					    MF_BYCOMMAND | MF_GRAYED );
		EnableMenuItem( GetMenu( hWnd ), IDM_FILE_CLOSE,
					    MF_BYCOMMAND | MF_GRAYED );
		}
	    if( bLooped )
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_LOOPED,
					    MF_BYCOMMAND | MF_ENABLED );
	    else
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_LOOPED,
					    MF_BYCOMMAND | MF_GRAYED );
	    if( bPlaying )
		{
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_PAUSE,
					    MF_BYCOMMAND | MF_ENABLED );
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_STOP,
					    MF_BYCOMMAND | MF_ENABLED );
		}
	    else
		{
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_PAUSE,
					    MF_BYCOMMAND | MF_GRAYED );
		EnableMenuItem( GetMenu( hWnd ), IDM_ACTIONS_STOP,
					    MF_BYCOMMAND | MF_GRAYED );
		}
	    break;

	case WM_GETMINMAXINFO:
    /*
     * We know exactly how big this window should be, and it's sort of a
     * little pop-up control panel, so we can disable window sizing by
     * forcing all the minimum and maximum sizes to be the calculated size.
     */
	    lpMinMax = (LPMINMAXINFO)lParam;

	    lpMinMax->ptMaxSize.x = 2*CONTROL_SPACE_CX + 2*BORDER_SPACE_CX
				    + CHECK_CX + TEMPO_TB_CX + VOL_TB_CX
				    + 2*GetSystemMetrics( SM_CXBORDER );
	    lpMinMax->ptMaxSize.y = 2*(BORDER_SPACE_CY
				    + GetSystemMetrics( SM_CYBORDER ))
				    + TEXT_SPACE_CY + nTextControlHeight
				    + TEMPO_TB_CY + BUTTON_CY
				    + CONTROL_SPACE_CY
				    + GetSystemMetrics( SM_CYMENU )
				    + GetSystemMetrics( SM_CYCAPTION );

	    lpMinMax->ptMinTrackSize.x = lpMinMax->ptMaxTrackSize.x
						    = lpMinMax->ptMaxSize.x;

	    lpMinMax->ptMinTrackSize.y = lpMinMax->ptMaxTrackSize.y
						    = lpMinMax->ptMaxSize.y;
	    break;

	case WM_HSCROLL:
	    if(((HWND)lParam == hWndTempo) && bFileOpen )
		{
		HandleTempoScroll( (int)LOWORD(wParam), (int)HIWORD(wParam));
		}
	    else if(((HWND)lParam == hWndVol) && bFileOpen )
		{
		HandleVolScroll( (int)LOWORD(wParam), (int)HIWORD(wParam));
		}
	    break;

	case WM_ENDSESSION:
	    // If the sesson is ending, we need to do our WM_DESTROY processing
	    if (!wParam) break;
	    // NOTE!!! we are falling through to the WM_CLOSE processing.
	case WM_DESTROY:
	    // Stop anything that might be playing and send a flag which will
	    // tell the code not to automatically reload the file for replay.
	    if( hStream )
		SendMessage( hWnd, WM_COMMAND,
			  MAKEWPARAM( IDC_STOP, MSTREAM_STOPF_NOREOPEN ), 0 );

	    FreeBuffers();

	    if( hStream )
		{
		if(( mmrRetVal = midiStreamClose( hStream ))
							!= MMSYSERR_NOERROR )
		    {
		    MidiErrorMessageBox( mmrRetVal );
		    }
		hStream = NULL;
		}

	    CloseHandle( hBufferReturnEvent );
	    
	    PostQuitMessage( 0 );
	    break;

	default:
	    return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
    return 0L;
    } /* MainWindowProc */


/*****************************************************************************/
/* DLG_About()                                                               */
/*                                                                           */
/*   Dialog procedure for the Help...About... box which simply pops up a     */
/* little copyright message and brief program description.                   */
/*                                                                           */
/*****************************************************************************/
BOOL CALLBACK DLG_About( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
    {
    switch( msg )
	{
	case WM_INITDIALOG:
	    break;

	case WM_COMMAND:
	    switch( LOWORD(wParam))
		{
		case IDOK:
		    EndDialog( hDlg, FALSE );
		    return( TRUE );

		default:
		    break;
		}
	    break;

	default:
	    return( FALSE );
	}

    return( FALSE );
    }


/*****************************************************************************/
/* FreeBuffers()                                                             */
/*                                                                           */
/*   This function unprepares and frees all our buffers -- something we must */
/* do to work around a bug in MMYSYSTEM that prevents a device from playing  */
/* back properly unless it is closed and reopened after each stop.           */
/*****************************************************************************/
void FreeBuffers( void )
    {
    DWORD       idx;
    MMRESULT    mmrRetVal;

    if( bBuffersPrepared )
	{
	for( idx = 0; idx < NUM_STREAM_BUFFERS; idx++ )
	    if(( mmrRetVal = midiOutUnprepareHeader( (HMIDIOUT)hStream,
				&ciStreamBuffers[idx].mhBuffer,
				sizeof(MIDIHDR)))
						!= MMSYSERR_NOERROR )
		{
		MidiErrorMessageBox( mmrRetVal );
		}
	    bBuffersPrepared = FALSE;
	    }
	// Free our stream buffers...
	for( idx = 0; idx < NUM_STREAM_BUFFERS; idx++ )
	    if( ciStreamBuffers[idx].mhBuffer.lpData )
		{
		GlobalFreePtr( ciStreamBuffers[idx].mhBuffer.lpData );
		ciStreamBuffers[idx].mhBuffer.lpData = NULL;
		}
    }


/*****************************************************************************/
/* StreamBufferSetup()                                                       */
/*                                                                           */
/*    This function uses the filename stored in the global character array to*/
/* open a MIDI file. Then it goes about converting at least the first part of*/
/* that file into a midiStream buffer for playback.                          */
/*****************************************************************************/
BOOL StreamBufferSetup( void )
    {
    int     nChkErr;
    BOOL    bFoundEnd = FALSE;
    DWORD   dwConvertFlag, idx;

    MMRESULT            mmrRetVal;
    MIDIPROPTIMEDIV     mptd;

    if( !hStream )
	if(( mmrRetVal = midiStreamOpen( &hStream,
				    &uMIDIDeviceID,
				    (DWORD)1, (DWORD)MidiProc,
				    (DWORD)0,
				    CALLBACK_FUNCTION )) != MMSYSERR_NOERROR )
	    {
	    MidiErrorMessageBox( mmrRetVal );
	    return( TRUE );
	    }

    for( idx = 0; idx < NUM_STREAM_BUFFERS; idx++ )
	{
	ciStreamBuffers[idx].mhBuffer.dwBufferLength = OUT_BUFFER_SIZE;
	if(( ciStreamBuffers[idx].mhBuffer.lpData
		= GlobalAllocPtr( GHND, OUT_BUFFER_SIZE )) == NULL )
	    {
	    // Buffers we already allocated will be killed by WM_DESTROY
	    // after we fail on the create by returning with -1
	    return( -1 );
	    }
	}
    if( ConverterInit( szFileBuffer ))
	return( TRUE );

    // Initialize the volume cache array to some pre-defined value
    for( idx = 0; idx < NUM_CHANNELS; idx++ )
	dwVolCache[idx] = VOL_CACHE_INIT;

    mptd.cbStruct = sizeof(mptd);
    mptd.dwTimeDiv = ifs.dwTimeDivision;
    if(( mmrRetVal = midiStreamProperty( hStream, (LPBYTE)&mptd,
					    MIDIPROP_SET | MIDIPROP_TIMEDIV ))
							!= MMSYSERR_NOERROR )
	{
	MidiErrorMessageBox( mmrRetVal );
	ConverterCleanup();
	return( TRUE );
	}

    nEmptyBuffers = 0;
    dwConvertFlag = CONVERTF_RESET;

    for( nCurrentBuffer = 0; nCurrentBuffer < NUM_STREAM_BUFFERS;
							    nCurrentBuffer++ )
	{
    // Tell the converter to convert up to one entire buffer's length of output
    // data. Also, set a flag so it knows to reset any saved state variables it
    // may keep from call to call.
	ciStreamBuffers[nCurrentBuffer].dwStartOffset = 0;
	ciStreamBuffers[nCurrentBuffer].dwMaxLength = OUT_BUFFER_SIZE;
	ciStreamBuffers[nCurrentBuffer].tkStart = 0;
	ciStreamBuffers[nCurrentBuffer].bTimesUp = FALSE;

	if(( nChkErr = ConvertToBuffer( dwConvertFlag,
					&ciStreamBuffers[nCurrentBuffer] ))
							!= CONVERTERR_NOERROR )
	    {
	    if( nChkErr == CONVERTERR_DONE )
		{
		bFoundEnd = TRUE;
		}
	    else
		{
		DebugPrint( "Initial conversion pass failed" );
		ConverterCleanup();
		return( TRUE );
		}
	    }
	ciStreamBuffers[nCurrentBuffer].mhBuffer.dwBytesRecorded
			    = ciStreamBuffers[nCurrentBuffer].dwBytesRecorded;

	if( !bBuffersPrepared )
	    if(( mmrRetVal = midiOutPrepareHeader( (HMIDIOUT)hStream,
				    &ciStreamBuffers[nCurrentBuffer].mhBuffer,
				    sizeof(MIDIHDR))) != MMSYSERR_NOERROR )
		{
		MidiErrorMessageBox( mmrRetVal );
		ConverterCleanup();
		return( TRUE );
		}
	if(( mmrRetVal = midiStreamOut( hStream,
				    &ciStreamBuffers[nCurrentBuffer].mhBuffer,
				    sizeof(MIDIHDR))) != MMSYSERR_NOERROR )
	    {
	    MidiErrorMessageBox( mmrRetVal );
	    break;
	    }
	dwConvertFlag = 0;

	if( bFoundEnd )
	    break;
	}

    bBuffersPrepared = TRUE;
    nCurrentBuffer = 0;
    UpdateFromControls();
    return( FALSE );
    }


/*****************************************************************************/
/* MidiProc()                                                                */
/*                                                                           */
/*   This is the callback handler which continually refills MIDI data buffers*/
/* as they're returned to us from the audio subsystem.                       */
/*****************************************************************************/
void CALLBACK MidiProc( HMIDIIN hMidi, UINT uMsg, DWORD dwInstance,
			DWORD dwParam1, DWORD dwParam2 )
    {
    static int  nWaitingBuffers = 0;
    MIDIEVENT   *pme;
    MIDIHDR     *pmh;

    MMRESULT    mmrRetVal;
    int         nChkErr;


    switch( uMsg )
	{
	case MOM_DONE:
	    if( uCallbackStatus == STATUS_CALLBACKDEAD )
		return;

	    nEmptyBuffers++;

	    if( uCallbackStatus == STATUS_WAITINGFOREND )
		{
		if( nEmptyBuffers < NUM_STREAM_BUFFERS )
		    {
		    return;
		    }
		else
		    {
		    uCallbackStatus = STATUS_CALLBACKDEAD;
		    PostMessage( hWndMain, WM_COMMAND,
					MAKEWPARAM( IDC_STOP, 0 ), 0L );
		    SetEvent( hBufferReturnEvent );
		    return;
		    }
		}

	// This flag is set whenever the callback is waiting for all buffers to
	// come back.
	    if( uCallbackStatus == STATUS_KILLCALLBACK )
		{
		// Count NUM_STREAM_BUFFERS-1 being returned for the last time
		if( nEmptyBuffers < NUM_STREAM_BUFFERS )
		    {
		    return;
		    }
		// Then send a stop message when we get the last buffer back...
		else
		    {
		    // Change the status to callback dead
		    uCallbackStatus = STATUS_CALLBACKDEAD;
		    SetEvent( hBufferReturnEvent );
		    return;
		    }
		}

	    dwProgressBytes
		+= ciStreamBuffers[nCurrentBuffer].mhBuffer.dwBytesRecorded;
	    PostMessage( hWndMain, WM_MSTREAM_PROGRESS, 0L, 0L );

///////////////////////////////////////////////////////////////////////////////
// Fill an available buffer with audio data again...

	    if( bPlaying && nEmptyBuffers )
		{
		ciStreamBuffers[nCurrentBuffer].dwStartOffset = 0;
		ciStreamBuffers[nCurrentBuffer].dwMaxLength = OUT_BUFFER_SIZE;
		ciStreamBuffers[nCurrentBuffer].tkStart = 0;
		ciStreamBuffers[nCurrentBuffer].dwBytesRecorded = 0;
		ciStreamBuffers[nCurrentBuffer].bTimesUp = FALSE;

		if(( nChkErr = ConvertToBuffer( 0,
					    &ciStreamBuffers[nCurrentBuffer] ))
							!= CONVERTERR_NOERROR )
		    {
		    if( nChkErr == CONVERTERR_DONE )
			{
			// Don't include this one in the count
			nWaitingBuffers = NUM_STREAM_BUFFERS - 1;
			uCallbackStatus = STATUS_WAITINGFOREND;
			return;
			}
		    else
			{
			DebugPrint( "MidiProc() conversion pass failed!" );
			ConverterCleanup();
			return;
			}
		    }

		ciStreamBuffers[nCurrentBuffer].mhBuffer.dwBytesRecorded
			    = ciStreamBuffers[nCurrentBuffer].dwBytesRecorded;

		if(( mmrRetVal = midiStreamOut( hStream,
				    &ciStreamBuffers[nCurrentBuffer].mhBuffer,
				    sizeof(MIDIHDR))) != MMSYSERR_NOERROR )
		    {
		    MidiErrorMessageBox( mmrRetVal );
		    ConverterCleanup();
		    return;
		    }
		nCurrentBuffer = ( nCurrentBuffer + 1 ) % NUM_STREAM_BUFFERS;
		nEmptyBuffers--;
		}

	    break;

	case MOM_POSITIONCB:
	    pmh = (MIDIHDR *)dwParam1;
	    pme = (MIDIEVENT *)(pmh->lpData + pmh->dwOffset);
	    if( MIDIEVENT_TYPE( pme->dwEvent ) == MIDI_CTRLCHANGE )
		{
		if( MIDIEVENT_DATA1( pme->dwEvent ) == MIDICTRL_VOLUME_LSB )
		    {
		    DebugPrint( "Got an LSB volume event" );
		    break;
		    }
		if( MIDIEVENT_DATA1( pme->dwEvent ) != MIDICTRL_VOLUME )
		    break;

		// Mask off the channel number and cache the volume data byte
		dwVolCache[ MIDIEVENT_CHANNEL( pme->dwEvent )]
					= MIDIEVENT_VOLUME( pme->dwEvent );
		// Post a message so that the main program knows to counteract
		// the effects of the volume event in the stream with its own
		// generated event which reflects the proper trackbar position.
		PostMessage( hWndMain, WM_MSTREAM_UPDATEVOLUME,
				MIDIEVENT_CHANNEL( pme->dwEvent ), 0L );
		}
	    break;

	default:
	    break;
	}

    return;
    }

