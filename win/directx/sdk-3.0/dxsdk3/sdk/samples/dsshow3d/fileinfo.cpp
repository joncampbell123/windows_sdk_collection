///////////////////////////////////////////////////////////////////////////////
//
// FileInfo.cpp
//
//   Implementation of the FileInfo class, which is the main class used to
// data and user interface elements associated with a sound buffer in DSShow3D.
//
//


#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include <commctrl.h>

#include "DSShow3D.h"
#include "GVars.h"
#include "FileInfo.h"
#include "wave.h"
#include "debug.h"


int FileInfo::m_xNextPos = 0;
int FileInfo::m_yNextPos = 0;

///////////////////////////////////////////////////////////////////////////////
// FileInfo()
//
//    Class constructor -- the pmw parameter defaults to NULL, but it should be
// something useful before the class is really used.
//
FileInfo::FileInfo( MainWnd *pmw )
{
    m_pmwOwner = pmw;
    m_pbData = NULL;
    m_pwfx = NULL;
    ZeroMemory( m_szFileName, sizeof(m_szFileName));
    m_nFileIndex = 0;
    m_hwndInterface = NULL;
    ZeroMemory( &m_ht, sizeof(HWNDTABLE));
    m_dwInternalFlags = 0;
    m_fPlayButtonSaysPlay = TRUE;

    ZeroMemory( &m_dsbd, sizeof(DSBUFFERDESC));
    m_pDSB = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// ~FileInfo()
//
//    Class destructor.
//
FileInfo::~FileInfo()
{
    m_pmwOwner = NULL;

    if( m_pDSB != NULL )
    {
    m_pDSB->Release();
    m_pDSB = NULL;
    }
    if( m_pwfx )
    {
    GlobalFree( m_pwfx );
    m_pwfx = NULL;
    }
    if( m_pbData )
    {
    GlobalFree( m_pbData );
    m_pbData = NULL;
    }
}


///////////////////////////////////////////////////////////////////////////////
// SetFileName()
//
//    Set the internal filename data variable and implicitly update the caption   
// to reflect the change.
//
void FileInfo::SetFileName( LPSTR lpsz, int nIndx )
{
    lstrcpy( m_szFileName, lpsz );
    m_nFileIndex = nIndx;

    // If this Assert fails, then we were handed a bad index value
    ASSERT( m_nFileIndex < lstrlen( m_szFileName ));

    UpdateFileName();
}


///////////////////////////////////////////////////////////////////////////////
// LoadWave()
//
//    Given a filename and an index to the partial filename (i.e. an index into
// the possibly full pathname where the actual filename begins), this function
// will do everything needed to load a file into the FileInfo structure.
//
// Returns 0 on success, non-zero on failure.
//
int FileInfo::LoadWave( LPSTR lpszFile, int nIndx )
{
    SetFileName( lpszFile, nIndx );

    // TODO: Need to add in support for ACM filters here

    // TODO: Need to determine what's "too big" to load static and then
    //       setup something for streaming the buffer instead.
    if( WaveLoadFile( m_szFileName, &m_cbDataSize, &m_pwfx, &m_pbData ) != 0 )
    {
    // There had better be a MainWnd object, or something is really messed
    ASSERT( m_pmwOwner );
    m_pmwOwner->MessageBox( "Bad wave file or file too big to fit in memory",
                            MB_OK|MB_ICONSTOP );
    goto LW_Error;
    }

    if( NewDirectSoundBuffer() != 0 )
    {
    // There had better be a MainWnd object, or something is really messed
    ASSERT( m_pmwOwner );
    m_pmwOwner->MessageBox( "Cannot create new DirectSoundBuffer object",
                            MB_OK|MB_ICONSTOP );
    goto LW_Error;
    }

    m_dwInternalFlags |= FI_INTERNALF_LOADED;

    // If we haven't failed in loading so far, this point will be a valid
    // pointer to the wave's data.
    ASSERT( NULL != m_pbData );

    // Create the ControlPod interface object
    CreateInterface( m_pmwOwner->GetHwnd());

    return 0;

LW_Error:
    return -1;
}


///////////////////////////////////////////////////////////////////////////////
// NewDirectSoundBuffer()
//
//    This function does all the work to create a new DirectSound buffer, and
// gets the interface pointers for both 2D and 3D (if the buffer is 3D).
//
int FileInfo::NewDirectSoundBuffer()
    {
    DSBCAPS         dsbc;
    HRESULT         hr;
    BYTE            *pbWrite1   = NULL;
    BYTE            *pbWrite2    = NULL;
    DWORD           cbLen1;
    DWORD           cbLen2;

    /* Set up the direct sound buffer. */
    m_dsbd.dwSize       = sizeof(DSBUFFERDESC);

    // We already set the flags to zero in the constructor.  Don't do it again
    // or we might wipe out anything a derived class has setup.
    m_dsbd.dwFlags    |= DSBCAPS_STATIC;
    m_dsbd.dwFlags    |= DSBCAPS_CTRLDEFAULT;   // !!! default
    // The derived class will pick its 3D flags
    if( m_dwInternalFlags & IsSticky())
        {
    ASSERT( !IsGlobal());
    m_dsbd.dwFlags |= DSBCAPS_STICKYFOCUS;
    }
    if( m_dwInternalFlags & IsGlobal())
    {
    ASSERT( !IsSticky());
        m_dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
    }

    // This flag can only be set if the open dialog detected emulation and
    // allowed the proper radio buttons to be enabled
    if( m_dwInternalFlags & IsUsingGetPos2())
    m_dsbd.dwFlags |= DSBCAPS_GETCURRENTPOSITION2;

    m_dsbd.dwBufferBytes    = m_cbDataSize;
    m_dsbd.lpwfxFormat      = m_pwfx;

    /* Make sure these are NULL before we start */    
    m_pDSB = NULL;

    if( FAILED( hr = gpds->CreateSoundBuffer( &m_dsbd, &m_pDSB, NULL )))
    {
    goto ERROR_IN_ROUTINE;
    }

    /* Ok, lock the sucker down, and copy the memory to it. */
    if( FAILED( hr = m_pDSB->Lock( 0, m_cbDataSize, &pbWrite1, &cbLen1,
                        &pbWrite2, &cbLen2, 0L )))
    {
    goto ERROR_IN_ROUTINE;
    }

    ASSERT( pbWrite1 != NULL );
    ASSERT( cbLen1 == m_cbDataSize );

    CopyMemory( pbWrite1, m_pbData, m_cbDataSize );

    ASSERT( 0 == cbLen2 );
    ASSERT( NULL == pbWrite2 );

    /* Ok, now unlock the buffer, we don't need it anymore. */
    if( FAILED( hr = m_pDSB->Unlock( pbWrite1, m_cbDataSize, pbWrite2, 0 )))
    {
    goto ERROR_IN_ROUTINE;
    }

    pbWrite1 = NULL;

    if (FAILED(hr = m_pDSB->SetVolume( MAXVOL_VAL )))
    {
    goto ERROR_IN_ROUTINE;
    }

    if (!Is3D()) {
        if( FAILED( hr = m_pDSB->SetPan( MIDPAN_VAL )))
        {
        goto ERROR_IN_ROUTINE;
        }
    }

    dsbc.dwSize = sizeof(dsbc);
    if( hr = m_pDSB->GetCaps( &dsbc ))
    {
    goto ERROR_IN_ROUTINE;
    }

    if( dsbc.dwFlags & DSBCAPS_LOCHARDWARE )
    {
    m_dwInternalFlags |= FI_INTERNALF_HARDWARE;
    }
    else
    {
    m_dwInternalFlags &= ~FI_INTERNALF_HARDWARE;
    }

    goto DONE_ROUTINE;

ERROR_IN_ROUTINE:
    if( pbWrite1 != NULL )
    {
    hr = m_pDSB->Unlock( pbWrite1, m_cbDataSize, pbWrite2, 0 );
    pbWrite1 = NULL;
    }

    if( NULL != m_pDSB )
    {
    m_pDSB->Release();
    m_pDSB = NULL;
    }

DONE_ROUTINE:
    return hr;
    }


///////////////////////////////////////////////////////////////////////////////
// SendDestroyRequest()
//
//    Ask the owning window object to destroy us and remove any information
// it may be keeping about our existence.
//
void FileInfo::SendDestroyRequest()
    {
    // We must have an owner to send the request to.  We should never
    // have gotten any further than the CreateInterface() without one.
    ASSERT( NULL != m_pmwOwner );

    m_pmwOwner->DestroyFileInfo( this );
    }


///////////////////////////////////////////////////////////////////////////////
// SendDestroyRequest()
//
//    Plays a buffer or updates playback flags according to some class state
// variables like our looping flag.
//
void FileInfo::PlayBuffer( void )
{
    if( m_pDSB )
    {
    if( IsLooped())
        m_pDSB->Play( 0, 0, DSBPLAY_LOOPING );
    else
        m_pDSB->Play( 0, 0, 0 );

    m_dwInternalFlags |= FI_INTERNALF_PLAYING;
    }
}


///////////////////////////////////////////////////////////////////////////////
// StopBuffer()
//
//    Stop the buffer and reset it's position to the start.
//
void FileInfo::StopBuffer( void )
{
    if( m_pDSB )
    {
    m_pDSB->Stop();
    m_pDSB->SetCurrentPosition( 0 );

    // Clear our internal state bit
    m_dwInternalFlags &= ~FI_INTERNALF_PLAYING;
    }
}


void FileInfo::Close( void )
    {
    SendMessage( m_hwndInterface, WM_COMMAND, MAKELONG(IDCANCEL, 0), 0L );
    }


///////////////////////////////////////////////////////////////////////////////
// DuplicateBuffer()
//
//    Initializes this FileInfo object by duplicating the given one.
//
void FileInfo::Duplicate( FileInfo *pfiSource )
    {
    if( NULL == pfiSource || NULL == pfiSource->m_pDSB ||
    !(pfiSource->m_dwInternalFlags & FI_INTERNALF_LOADED))
    return;

    m_cbDataSize = pfiSource->m_cbDataSize;
    m_nFileIndex = pfiSource->m_nFileIndex;
    m_dwInternalFlags = pfiSource->m_dwInternalFlags;
    m_pmwOwner = pfiSource->m_pmwOwner;

    m_pwfx = (PWAVEFORMATEX)new BYTE[sizeof(WAVEFORMATEX)
                    + pfiSource->m_pwfx->cbSize];
    m_pbData = new BYTE[pfiSource->m_cbDataSize];

    CopyMemory( m_pbData, pfiSource->m_pbData, pfiSource->m_cbDataSize );
    CopyMemory( m_pwfx, pfiSource->m_pwfx,
        sizeof(WAVEFORMATEX) + pfiSource->m_pwfx->cbSize);
    CopyMemory( &m_dsbd, &pfiSource->m_dsbd, sizeof(m_dsbd));
    CopyMemory( &m_szFileName, pfiSource->m_szFileName, sizeof(m_szFileName));

    ASSERT( NULL != pfiSource->m_pDSB );
    ASSERT( NULL != gpds );

    HRESULT hr;

    if( FAILED( hr = gpds->DuplicateSoundBuffer(pfiSource->m_pDSB, &m_pDSB)))
    {
    DPF( 0, "Failed to DuplicateSoundBuffer() (%s)", TranslateDSError(hr));
    }
    else
    {
    CreateInterface( m_pmwOwner->GetHwnd());
    }
    }


///////////////////////////////////////////////////////////////////////////////
// CreateInterface()
//
//    Creates an interface window. A return of TRUE indicates success.
//
BOOL FileInfo::CreateInterface( HWND hwndOwner )
    {
    m_hwndInterface = CreateDialogParam( ghInst, MAKEINTRESOURCE(IDD_BUFFER),
                   hwndOwner, (DLGPROC)FileInfoDlgProc,
                   (LPARAM)this );

    if( NULL == m_hwndInterface )
    goto FICI_Fail;

    UpdateFileName();

    CascadeWindow();

    ShowWindow( m_hwndInterface, SW_SHOW );

    // This flag tells us an interface window was successfully created
    m_dwInternalFlags |= FI_INTERNALF_INTERFACE;
    return TRUE;


FICI_Fail:
    if( m_hwndInterface )
    {
    DestroyWindow( m_hwndInterface );
    m_hwndInterface = NULL;
    }
    // Clear the flag that says we have a good interface window created
    m_dwInternalFlags &= ~FI_INTERNALF_INTERFACE;
    return FALSE;
    }


///////////////////////////////////////////////////////////////////////////////
// ResetCascade()
//
//
//
void FileInfo::ResetCascade( void )
    {
    POINT   ptParent;

    ptParent.x = ptParent.y = 0;
    ClientToScreen( m_pmwOwner->GetHwnd(), &ptParent );
    m_xNextPos = ptParent.x;
    m_yNextPos = ptParent.y;
    }


///////////////////////////////////////////////////////////////////////////////
// MinimizeWindow()
//
//
//
void FileInfo::MinimizeWindow( void )
    {
    ShowWindow( m_hwndInterface, SW_MINIMIZE );
    }


///////////////////////////////////////////////////////////////////////////////
// RestoreWindow()
//
//
//
void FileInfo::RestoreWindow( void )
    {
    SendMessage( m_hwndInterface, WM_SYSCOMMAND, SC_RESTORE, 0L );
    }


///////////////////////////////////////////////////////////////////////////////
// CascadeWindow()
//
//
//
void FileInfo::CascadeWindow( void )
    {
    RECT    rcWind;
    int     nStep;

    // Don't move minimized windows
    if( IsIconic( m_hwndInterface ))
    return;

    GetWindowRect( m_hwndInterface, &rcWind );

    if( m_xNextPos + (rcWind.right - rcWind.left) > GetSystemMetrics(SM_CXSCREEN))
    ResetCascade();
    else if( m_yNextPos + (rcWind.bottom - rcWind.top) > GetSystemMetrics(SM_CYSCREEN))
    ResetCascade();

    SetWindowPos( m_hwndInterface, NULL, m_xNextPos, m_yNextPos,
                    0, 0, SWP_NOSIZE | SWP_NOZORDER );
    // Move diagonally by the height of the title bar
    nStep = GetSystemMetrics(SM_CYCAPTION);
    m_xNextPos += nStep;
    m_yNextPos += nStep;
    }


///////////////////////////////////////////////////////////////////////////////
// FileInfoDlgProc()
//
//    Window message callback function for all Interface DLGs'.  Route messages
// to message handler functions
//
BOOL CALLBACK FileInfoDlgProc( HWND hDlg, UINT message,
                        WPARAM wParam, LPARAM lParam )
    {
    FileInfo *pfi;

    switch( message )
    {
    // The first step is to stash our class object pointer in the user data
    // and Initialize all our controls and internal data members.
    case WM_INITDIALOG:
        ASSERT( NULL != lParam );
        pfi = (FileInfo *)lParam;
        SetWindowLong( hDlg, DWL_USER, (LONG)pfi );

        if( !pfi->OnInitDialog( hDlg, wParam ))
        {
        DestroyWindow( hDlg );
        }
        return TRUE;

    // By setting the global variable that tracks the active dialog,
    // we can easily dispatch keyboard messages to the proper dialog
    // through IsDialogMessage() in our main message pump.
    case WM_ACTIVATE:
        if( !wParam )
        ghDlgActive = NULL;
        else
        ghDlgActive = hDlg;
        return TRUE;

    case WM_INITMENU:
        pfi = (FileInfo*)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pfi );
        return pfi->OnInitMenu( wParam, lParam );

    case WM_DROPFILES:
        pfi = (FileInfo*)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pfi );
        return pfi->m_pmwOwner->SendMessage( WM_DROPFILES, wParam, lParam );

    case WM_COMMAND:
        pfi = (FileInfo*)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pfi );
        return pfi->OnCommand( wParam, lParam );

    // Handle this to deal with right-clicks on our controls -- we have a
    // bunch of different context menus that we can popup
    case WM_CONTEXTMENU:
        pfi = (FileInfo*)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pfi );
        return pfi->OnContextMenu( hDlg, LOWORD(lParam), HIWORD(lParam));

    // Trackbar slider notifications come through here
    case WM_HSCROLL:
        pfi = (FileInfo*)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pfi );
        return pfi->OnHScroll( LOWORD(wParam), (LONG)HIWORD(wParam), (HWND)lParam );

    case WM_DESTROY:
        pfi = (FileInfo*)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pfi );
        pfi->OnDestroy();
        return TRUE;

    default:
        return FALSE;
    }

    ASSERT( FALSE );
    }


/* OnInitMenu()
 *
 *    Updates the state of items on the menus in response to a WM_INITMENU
 * message, which means the menu is about to be displayed.
 */
BOOL FileInfo::OnInitMenu( WPARAM wParam, LPARAM lParam )
    {
    HMENU   hSys = GetSystemMenu( m_hwndInterface, FALSE );

    EnableMenuItem( hSys, SC_MAXIMIZE, MF_BYCOMMAND | MF_GRAYED );
    EnableMenuItem( hSys, SC_SIZE, MF_BYCOMMAND | MF_GRAYED );
    return TRUE;
    }


/* OnInitDialog()
 *
 *    Handles the initialization of the FilInfo interface window, which is
 * actually a modeless dialog.
 */
BOOL FileInfo::OnInitDialog( HWND hDlg, WPARAM wParam )
    {
    TCHAR   szBuf1[64], tszFmt[64];

    // Grab a bunch of handles and store them for easy access later
    m_ht.hPlayButton = GetDlgItem( hDlg, IDC_BUFFERDLG_PLAY_BUTTON );
    m_ht.hLoopedCheck = GetDlgItem( hDlg, IDC_BUFFERDLG_LOOPED_CHECK );
    m_ht.hProgressSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_PROGRESS_SLIDER );
    m_ht.hProgressText = GetDlgItem( hDlg, IDC_BUFFERDLG_PROGRESS_TEXT );
    m_ht.hProgressSpin = GetDlgItem( hDlg, IDC_BUFFERDLG_PROGRESS_SPIN );
    m_ht.hFreqText = GetDlgItem( hDlg, IDC_BUFFERDLG_FREQ_TEXT );
    m_ht.hFreqSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_FREQ_SLIDER );
    m_ht.hVolText = GetDlgItem( hDlg, IDC_BUFFERDLG_VOL_TEXT );
    m_ht.hVolSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_VOL_SLIDER );
    m_ht.hPanText = GetDlgItem( hDlg, IDC_BUFFERDLG_PAN_TEXT );
    m_ht.hPanSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_PAN_SLIDER );
    m_ht.hPlayCursorText = GetDlgItem( hDlg, IDC_BUFFERDLG_PLAYCURSOR_TEXT );
    m_ht.hWriteCursorText = GetDlgItem( hDlg, IDC_BUFFERDLG_WRITECURSOR_TEXT );
    m_ht.hDataFormatText = GetDlgItem( hDlg, IDC_BUFFERDLG_DATAFORMAT_TEXT );
    m_ht.hBufferTypeText = GetDlgItem( hDlg, IDC_BUFFERDLG_BUFFERTYPE_TEXT );
    m_ht.hFocusModeText = GetDlgItem( hDlg, IDC_BUFFERDLG_FOCUS_TEXT );
    m_ht.hGetPosModeText = GetDlgItem( hDlg, IDC_BUFFERDLG_GETPOS_TEXT );

    // Load, fill in and set the string describing the format of the sound data
    if( m_pwfx->nChannels == 1 )
    LoadString( ghInst, IDS_DATAFORMAT_MONO, tszFmt, sizeof(tszFmt)-1);
    else
    LoadString( ghInst, IDS_DATAFORMAT_STEREO, tszFmt, sizeof(tszFmt)-1);
    wsprintf( szBuf1, tszFmt, m_pwfx->nSamplesPerSec, m_pwfx->wBitsPerSample );
                        
    Static_SetText( m_ht.hDataFormatText, szBuf1 );

    // Set the Buffer Type text to HARDWARE or SOFTWARE
    if( IsHardware())
    LoadString( ghInst, IDS_BUFFERTYPE_HARDWARE, szBuf1, sizeof(szBuf1)-1);
    else
    LoadString( ghInst, IDS_BUFFERTYPE_SOFTWARE, szBuf1, sizeof(szBuf1)-1);
    Static_SetText( m_ht.hBufferTypeText, szBuf1 );

    // Set the focus type to LOCAL, STICKY, or GLOBAL
    if( IsSticky())
    LoadString( ghInst, IDS_FOCUSMODE_STICKY, szBuf1, sizeof(szBuf1)-1);
    else if( IsGlobal())
    LoadString( ghInst, IDS_FOCUSMODE_GLOBAL, szBuf1, sizeof(szBuf1)-1);
    else
    LoadString( ghInst, IDS_FOCUSMODE_LOCAL, szBuf1, sizeof(szBuf1)-1);
    Static_SetText( m_ht.hFocusModeText, szBuf1 );

    if( Is3D())
    {
    Static_Enable( m_ht.hPanText, FALSE );
    Static_Enable( m_ht.hPanSlider, FALSE );
    }

    // Set the range, page size, etc. of our "slider" (trackbar) controls
    SetSliders();

    // Update the state of the UI elements that change
    UpdateUI();
    return TRUE;
    }


///////////////////////////////////////////////////////////////////////////////
// OnContextMenu()
//
//    Handles a right-click in the client area by popping up a menu if we have
// one we'd like to display, or returning otherwise. TRUE indicates the message
// was "handled" and no further processing is needed.  FALSE means the message
// was not handled.
//
BOOL FileInfo::OnContextMenu( HWND hwnd, int x, int y )
    {
    HMENU   hm, hSub;
    int     nSubMenu = -1, idFrom = 0;
    POINT   pt = { x, y };
    RECT    rectWind1, rectWind2;

    // Set the sub-menu to the Frequency context menu if we hit the Freq. Slider
    // or text control
    GetWindowRect( m_ht.hFreqSlider, &rectWind1 );
    GetWindowRect( m_ht.hFreqText, &rectWind2 );

    if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt ))
    nSubMenu = 0;

    // Set the sub-menu to the Volume context menu if we hit the Vol. Slider
    // or text control
    GetWindowRect( m_ht.hVolSlider, &rectWind1 );
    GetWindowRect( m_ht.hVolText, &rectWind2 );

    if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt ))
    nSubMenu = 1;

    // Only have pan on non-3D buffers
    if (!Is3D())
    {
        // Set the sub-menu to the Pan context menu if we hit the Pan Slider
        // or text control
        GetWindowRect( m_ht.hPanSlider, &rectWind1 );
        GetWindowRect( m_ht.hPanText, &rectWind2 );

        if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt ))
        nSubMenu = 2;
        }

    // We didn't detect any "interesting" hotspots, so return as unprocessed.
    if( nSubMenu < 0 )
    return FALSE;

    // If we make it here, we're gonna popup a context menu of some sort

    // Attempt to load our menu.  If we fail, we still handled the message
    // so return TRUE
    if(( hm = LoadMenu( ghInst, MAKEINTRESOURCE(IDM_POPUPS))) == NULL )
    return TRUE;

    hSub = GetSubMenu( hm, nSubMenu );
    TrackPopupMenu( hSub, TPM_LEFTALIGN | TPM_RIGHTBUTTON,
            pt.x, pt.y, 0, m_hwndInterface, NULL );
    
    DestroyMenu( hm );

    return TRUE;
    }


///////////////////////////////////////////////////////////////////////////////
// SetSliders()
//
//    Sets the range, page size, tic frequency, and other parameters of all the
// trackbar controls we use in the interface.
//
void FileInfo::SetSliders( void )
    {
    DWORD   dSampleRateRange = FREQ_SLIDER_MAX - FREQ_SLIDER_MIN;
    UDACCEL udAccel;

    // Scale the sample rate range into an acceptable span and keep
    // track of the multiplying constant we'll have to use later when
    // we want to set positions based on real values
    m_dwFreqSliderFactor = 1;
    while( dSampleRateRange > 10000 )
    {
    dSampleRateRange /= 10;
    m_dwFreqSliderFactor *= 10;
    }

    SendMessage( m_ht.hFreqSlider, TBM_SETRANGEMIN,
            FALSE, (LPARAM)FREQ_SLIDER_MIN / m_dwFreqSliderFactor );
    SendMessage( m_ht.hFreqSlider, TBM_SETRANGEMAX,
            FALSE, (LPARAM)FREQ_SLIDER_MAX / m_dwFreqSliderFactor );
    SendMessage( m_ht.hFreqSlider, TBM_SETPAGESIZE, 0,
                FREQ_SLIDER_PAGESIZE_HZ / m_dwFreqSliderFactor );

    SendMessage( m_ht.hProgressSpin, UDM_SETBUDDY, (WPARAM)m_ht.hProgressSlider, 0 );
    SendMessage( m_ht.hProgressSpin, UDM_SETRANGE, 0, MAKELONG(PROGRESS_MAX, PROGRESS_MIN));
    SendMessage( m_ht.hProgressSpin, UDM_SETPOS, 0, MAKELONG(0,0));
    udAccel.nSec = 0;
    udAccel.nInc = PROGRESS_MAX / 20;
    SendMessage( m_ht.hProgressSpin, UDM_SETACCEL, 1, (LPARAM)&udAccel );

    SendMessage( m_ht.hProgressSlider, TBM_SETRANGEMIN, FALSE, (LPARAM)PROGRESS_MIN );
    SendMessage( m_ht.hProgressSlider, TBM_SETRANGEMAX, FALSE, (LPARAM)PROGRESS_MAX );
    SendMessage( m_ht.hProgressSlider, TBM_SETPAGESIZE, FALSE, (LPARAM)PROGRESS_MAX / 20 );
    SendMessage( m_ht.hProgressSlider, TBM_SETTICFREQ, (WPARAM)PROGRESS_TIC, 0 );

    // Intentionally set the range backwards because a large number means
    // a smaller value
    SendMessage( m_ht.hVolSlider, TBM_SETRANGEMIN, FALSE,
        (LPARAM)(VOL_MIN + VOL_SLIDER_SHIFT) / VOL_SLIDER_FACTOR );
    SendMessage( m_ht.hVolSlider, TBM_SETRANGEMAX, FALSE,
        (LPARAM)(VOL_MAX + VOL_SLIDER_SHIFT) / VOL_SLIDER_FACTOR );
    SendMessage( m_ht.hVolSlider, TBM_SETPAGESIZE, 0,
        VOL_SLIDER_PAGE / VOL_SLIDER_FACTOR );
    // NOTE: No TICs on the volume slider

    SendMessage( m_ht.hPanSlider, TBM_SETRANGEMIN, FALSE,
        (LPARAM)(PAN_MIN + PAN_SLIDER_SHIFT) / PAN_SLIDER_FACTOR );
    SendMessage( m_ht.hPanSlider, TBM_SETRANGEMAX, FALSE,
        (LPARAM)(PAN_MAX + PAN_SLIDER_SHIFT) / PAN_SLIDER_FACTOR );
    SendMessage( m_ht.hPanSlider, TBM_SETPAGESIZE, 0,
        PAN_SLIDER_PAGE / PAN_SLIDER_FACTOR );
    // NOTE: No TICs on the pan slider

    // Update the display from the buffer's current settings
    UpdateFreqUI( 0, TRUE );
    UpdateVolUI( 0, TRUE );
    UpdatePanUI( 0, TRUE );
    }


///////////////////////////////////////////////////////////////////////////////
// UpdateUI()
//
//    This function is normally called from the MainWnd object's timer handler
// to refresh the UI elements that are time-dependent.  These are things like
// the play and write cursors and the progress.
//
void FileInfo::UpdateUI( void )
    {
    char    szText[8];
    DWORD   dwStatus, dwPlay, dwWrite;

    if( NULL == m_pDSB )
    return;

    if( FAILED( m_pDSB->GetStatus( &dwStatus )))
    return;

    if( dwStatus & DSBSTATUS_BUFFERLOST )
    {
    LPBYTE  pb1, pb2;
    DWORD   cb1, cb2;

    if( SUCCEEDED( m_pDSB->Restore()))
        {
        if( SUCCEEDED( m_pDSB->Lock( 0, m_cbDataSize, &pb1, &cb1, &pb2, &cb2, 0 )))
        {
        ASSERT( m_cbDataSize == cb1 );
        if( NULL != m_pbData )
            CopyMemory( pb1, m_pbData, m_cbDataSize );
        m_pDSB->Unlock( pb1, m_cbDataSize, pb2, 0 );

        if(IsPlaying())
            PlayBuffer();
        }
        }
    }
    else
    {
    if( dwStatus & DSBSTATUS_PLAYING )
        SetPlaying( TRUE );
    else
        SetPlaying( FALSE );
    }

    Button_SetCheck( m_ht.hLoopedCheck, IsLooped());

    UpdatePlayButton();

    m_pDSB->GetCurrentPosition( &dwPlay, &dwWrite );
    wsprintf( szText, "%u", dwPlay );
    Static_SetText( m_ht.hPlayCursorText, szText );
    wsprintf( szText, "%u", dwWrite );
    Static_SetText( m_ht.hWriteCursorText, szText );

    UpdateProgressUI( dwPlay );

    return;
    }


///////////////////////////////////////////////////////////////////////////////
// UpdateProgressUI()
//
//
//
void FileInfo::UpdateProgressUI( DWORD dwPlayPos )
    {
    char    szText[8];
    FLOAT   fPercentage;

    // TODO: Set the progress slider position
    fPercentage = ((FLOAT)dwPlayPos / (FLOAT)(m_cbDataSize-1));

    SendMessage( m_ht.hProgressSlider, TBM_SETPOS, TRUE, (LPARAM)(PROGRESS_MAX * fPercentage));
    SendMessage( m_ht.hProgressSpin, UDM_SETPOS, TRUE, MAKELONG((PROGRESS_MAX * fPercentage), 0));

    wsprintf( szText, "%i%%", (int)(100 * fPercentage));
    Static_SetText( m_ht.hProgressText, szText );
    }

///////////////////////////////////////////////////////////////////////////////
// UpdateVolUI()
//
//    Updates the position of the volume slider and text. You can specify that
// it get the volume from the buffer by passing TRUE to fFromBuffer, or you can
// specify a volume by putting it in lForceVol and setting fFromBuffer = FALSE.
//
void FileInfo::UpdateVolUI( LONG lForceVol, BOOL fFromBuffer )
    {
    LONG    lVol;
    char    szText[16];

    if( fFromBuffer )
        {
    if( NULL != m_pDSB )
        {
        m_pDSB->GetVolume( &lVol );
        }
    else
        lVol = 0;
    }
    else
    lVol = lForceVol;

    SendMessage( m_ht.hVolSlider, TBM_SETPOS, (WPARAM)TRUE,
            (LPARAM)(lVol + VOL_SLIDER_SHIFT) / VOL_SLIDER_FACTOR );

    // Print volume in decibels
    wsprintf( szText, "%i dB", lVol / 100 );

    Static_SetText( m_ht.hVolText, szText );
    }


///////////////////////////////////////////////////////////////////////////////
// UpdatePanUI()
//
//    Updates the position of the panning slider and text. You can specify that
// it get the pan from the buffer by passing TRUE to fFromBuffer, or you can
// specify a pan by putting it in lForcePan and setting fFromBuffer = FALSE.
//
void FileInfo::UpdatePanUI( LONG lForcePan, BOOL fFromBuffer )
    {
    LONG    lPan;
    char    szText[16];

    if ( Is3D())
    {
    lForcePan = 0;
    fFromBuffer = FALSE;
    }

    if( fFromBuffer )
        {
    if( NULL != m_pDSB )
        {
        m_pDSB->GetPan( &lPan );
        }
    else
        lPan = 0;
    }
    else
    lPan = lForcePan;

    SendMessage( m_ht.hPanSlider, TBM_SETPOS, (WPARAM)TRUE,
            (LPARAM)(lPan + PAN_SLIDER_SHIFT) / PAN_SLIDER_FACTOR );

    // Print pan in decibels
    wsprintf( szText, "%i dB", lPan / 100 );

    Static_SetText( m_ht.hPanText, szText );
    }


///////////////////////////////////////////////////////////////////////////////
// UpdateFreqUI()
//
//    Updates the position of the freq slider and text. You can specify that
// it get the freq from the buffer by passing TRUE to fFromBuffer, or you can
// specify a freq by putting it in dwForceFreq and setting fFromBuffer = FALSE.
//
void FileInfo::UpdateFreqUI( DWORD dwForceFreq, BOOL fFromBuffer )
    {
    DWORD   dwFreq;
    char    szText[16];

    if( fFromBuffer )
        {
    if( NULL != m_pDSB )
        m_pDSB->GetFrequency( &dwFreq );
    else
        dwFreq = 0;
    }
    else
    dwFreq = dwForceFreq;

    SendMessage( m_ht.hFreqSlider, TBM_SETPOS,
            (WPARAM)TRUE, (LPARAM)dwFreq / m_dwFreqSliderFactor );

    wsprintf( szText, "%u Hz", dwFreq );

    Static_SetText( m_ht.hFreqText, szText );
    }


///////////////////////////////////////////////////////////////////////////////
// UpdatePlayButton()
//
//    Uses an internal state flag and the "IsPlaying" flag to determine and set
// the proper text (Play/Stop) for the Play button.
//
void FileInfo::UpdatePlayButton( void )
    {
    if( m_fPlayButtonSaysPlay && IsPlaying())
    {
    // Set to "Stop"
    m_fPlayButtonSaysPlay = FALSE;
    Button_SetText( m_ht.hPlayButton, TEXT("Stop"));
    }
    else if( !m_fPlayButtonSaysPlay && !IsPlaying())
    {
    // Set to "Play"
    Button_SetText( m_ht.hPlayButton, TEXT("Play"));
    m_fPlayButtonSaysPlay = TRUE;
    }
    else
    return;
    }


///////////////////////////////////////////////////////////////////////////////
// OnHScroll()
//
//    Main message handler for the WM_HSCROLL message.  this function basically
// figures out which horizontal scrolling control is responsible for sending
// the message and passes on handling to an appropriate function for handling.
//
BOOL FileInfo::OnHScroll( WORD wNotification, LONG lPos, HWND hControl )
    {
    if( !hControl )
    return FALSE;
    
    if( hControl == m_ht.hProgressSpin )
    {
    HandleProgressSpinScroll( wNotification, lPos );
    return TRUE;
    }
    else if( hControl == m_ht.hProgressSlider )
    {
    HandleProgressSliderScroll( wNotification, lPos );
    return TRUE;
    }
    else if( hControl == m_ht.hFreqSlider )
    {
    HandleFreqSliderScroll( wNotification, lPos );
    return TRUE;
    }
    else if( hControl == m_ht.hVolSlider )
    {
    HandleVolSliderScroll( wNotification, lPos );
    return TRUE;
    }
    else if( hControl == m_ht.hPanSlider )
    {
    HandlePanSliderScroll( wNotification, lPos );
    return TRUE;
    }
    else
    return FALSE;
    }


///////////////////////////////////////////////////////////////////////////////
// HandleFreqSliderScroll()
//
//    Helper function for OnHScroll() which handles the WM_HSCROLL message for
// the Frequency slider.  Figures out the next position, sets it, and updates
// the UI elements that are affected (text).
//
void FileInfo::HandleFreqSliderScroll( WORD wNot, LONG lPos )
    {
    DWORD dwFreq;

    switch( wNot )
    {
    case TB_THUMBTRACK:
        if( NULL != m_pDSB )
        {
        if( SUCCEEDED( m_pDSB->SetFrequency( lPos * m_dwFreqSliderFactor )))
            {
            UpdateFreqUI( lPos * m_dwFreqSliderFactor, FALSE );
            }
        }
        break;

    case TB_ENDTRACK:
    case TB_LINEDOWN:
    case TB_LINEUP:
    case TB_PAGEDOWN:
    case TB_PAGEUP:
        lPos = SendMessage( m_ht.hFreqSlider, TBM_GETPOS, 0, 0 );
        if( NULL != m_pDSB )
        {
        if( SUCCEEDED( m_pDSB->SetFrequency( lPos * m_dwFreqSliderFactor )))
            {
            UpdateFreqUI( lPos * m_dwFreqSliderFactor, FALSE );
            }
        else
            {
            if( SUCCEEDED( m_pDSB->GetFrequency( &dwFreq )))
            {
            SendMessage( m_ht.hFreqSlider, TBM_SETPOS,
                    TRUE, dwFreq / m_dwFreqSliderFactor );
            }
            }
        }
        break;
    }
    
    }


///////////////////////////////////////////////////////////////////////////////
// HandleProgressSliderScroll()
//
//
//
void FileInfo::HandleProgressSliderScroll( WORD wNot, LONG lPos )
    {
    BOOL fUpdate = TRUE;

    switch( wNot )
    {
    case TB_THUMBTRACK:
        if( IsPlaying())
        fUpdate = FALSE;
        break;

    case TB_ENDTRACK:
        fUpdate = FALSE;
        break;
    case TB_LINEDOWN:
    case TB_LINEUP:
    case TB_PAGEDOWN:
    case TB_PAGEUP:
        lPos = SendMessage( m_ht.hProgressSlider, TBM_GETPOS, 0, 0 );
        break;

    default:
        fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB )
    {
    FLOAT   fPercentage = ((FLOAT)lPos / 10000);

    m_pDSB->SetCurrentPosition( (DWORD)(fPercentage * (m_cbDataSize-1)));
    UpdateUI();
//  UpdateProgressUI(  (DWORD)(fPercentage * (m_cbDataSize-1)));
    }
    }


///////////////////////////////////////////////////////////////////////////////
// HandleProgressSpinScroll()
//
//
//
void FileInfo::HandleProgressSpinScroll( WORD wNot, LONG lPos )
    {
    BOOL fUpdate = TRUE;

    switch( wNot )
    {
    case SB_THUMBPOSITION:
        lPos = SendMessage( m_ht.hProgressSpin, UDM_GETPOS, 0, 0 );
        break;

    default:
        fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB )
    {
    FLOAT   fPercentage = ((FLOAT)lPos / 10000);

    m_pDSB->SetCurrentPosition( (DWORD)(fPercentage * (m_cbDataSize-1)));
    UpdateUI();
    UpdateProgressUI(  (DWORD)(fPercentage * (m_cbDataSize-1)));
    }
    }


///////////////////////////////////////////////////////////////////////////////
// HandleVolSliderScroll()
//
//    Helper function for OnHScroll() which handles the WM_HSCROLL message for
// the Volume slider.  Figures out the next position, sets it, and updates
// the UI elements that are affected (text).
//
void FileInfo::HandleVolSliderScroll( WORD wNot, LONG lPos )
    {
    BOOL fUpdate = TRUE;

    switch( wNot )
    {
    case TB_THUMBTRACK:
        break;

    case TB_ENDTRACK:
    case TB_LINEDOWN:
    case TB_LINEUP:
    case TB_PAGEDOWN:
    case TB_PAGEUP:
        lPos = SendMessage( m_ht.hVolSlider, TBM_GETPOS, 0, 0 );
        break;

    default:
        fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB )
    {
    m_pDSB->SetVolume( (lPos * VOL_SLIDER_FACTOR) - VOL_SLIDER_SHIFT );
    DPF( 1, "SetVolume: %i", (lPos * VOL_SLIDER_FACTOR) - VOL_SLIDER_SHIFT );
    UpdateVolUI( (lPos * VOL_SLIDER_FACTOR) - VOL_SLIDER_SHIFT, FALSE );
    }
    }


///////////////////////////////////////////////////////////////////////////////
// HandlePanSliderScroll()
//
//    Helper function for OnHScroll() which handles the WM_HSCROLL message for
// the Pan slider.  Figures out the next position, sets it, and updates the UI
// elements that are affected (text).
//
void FileInfo::HandlePanSliderScroll( WORD wNot, LONG lPos )
    {
    BOOL fUpdate = TRUE;

    switch( wNot )
    {
    case TB_THUMBTRACK:
        break;

    case TB_ENDTRACK:
    case TB_LINEDOWN:
    case TB_LINEUP:
    case TB_PAGEDOWN:
    case TB_PAGEUP:
        lPos = SendMessage( m_ht.hPanSlider, TBM_GETPOS, 0, 0 );
        break;

    default:
        fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB )
    {
    m_pDSB->SetPan( (lPos * PAN_SLIDER_FACTOR) - PAN_SLIDER_SHIFT );
    UpdatePanUI( (lPos * PAN_SLIDER_FACTOR) - PAN_SLIDER_SHIFT, FALSE );
    }
    }


/* OnDestroy()
 *
 */
void FileInfo::OnDestroy()
    {
    SendDestroyRequest();
    return;
    }


///////////////////////////////////////////////////////////////////////////////
// OnCommand()
//
//    Handles WM_COMMAND messages sent to the dialog interface.
//
BOOL FileInfo::OnCommand( WPARAM wParam, LPARAM lParam )
    {
    ASSERT( NULL != m_hwndInterface );

    // These three functions break out the handling of the WM_COMMAND messages
    // that will be sent by various context menus.  There's no real difference
    // between these and other messages, but keeping them seperate emphasizes
    // where they come from and keeps our switch a bit shorter.
    if( HandleFreqContext( wParam ) || HandleVolContext( wParam )
        || HandlePanContext( wParam ))
    {
    return TRUE;
    }

    switch( LOWORD( wParam ))
    {
    case ID_BUFFERDLG_FILE_OPEN:
        // For convenience, the File|Open command is on the dialog's menu.
        // If we see it, we should reflect it to the parent for processing.
        ASSERT( NULL != m_pmwOwner );
        SendMessage( m_pmwOwner->GetHwnd(), WM_COMMAND,
                    MAKEWPARAM( IDC_FILE_OPEN, 0 ), 0L );
        break;

    case ID_BUFFERDLG_DUPLICATE:
        m_pmwOwner->DuplicateBuffer( this );
        break;

    case IDCANCEL:
        // This is what the dialog subsystem will send us when the user
        // clicks the Close button from the caption bar or the System menu
        DestroyWindow( m_hwndInterface );
        break;

        case IDC_BUFFERDLG_PLAY_BUTTON:
        // Handle the Play button depending on whether or not the buffer is
        // playing or stopped.
        if( IsPlaying())
        {
        StopBuffer();
        }
        else
        {
        PlayBuffer();
        }
        UpdatePlayButton();
        break;

    case IDC_BUFFERDLG_LOOPED_CHECK:
        // Toggle the state of the looping flag with an XOR
        m_dwInternalFlags ^= FI_INTERNALF_LOOPED;

        // Calling Play will update the looping state in DSound
        if( IsPlaying())
        PlayBuffer();
        break;

    default:
        // FALSE means we didn't want to deal with the message
        return FALSE;
    }

    // TRUE means we processed the message
    return TRUE;
    }


///////////////////////////////////////////////////////////////////////////////
// UpdateFileName()
//
//    Updates the file name which is displayed in the dialog window caption.
//
void FileInfo::UpdateFileName( void )
    {
    if( NULL != m_hwndInterface )
    {
    SendMessage( m_hwndInterface, WM_SETTEXT, 0L,
                    (LPARAM)&m_szFileName[m_nFileIndex] );
    }
    }


///////////////////////////////////////////////////////////////////////////////
// HandlePanContext()
//
//    Handle WM_COMMAND messages from the Pan context menu.  Returns TRUE if a
// message was handled.
//
BOOL FileInfo::HandlePanContext( WPARAM wParam )
    {
    switch( LOWORD( wParam ))
    {
    case ID_PANCONTEXT_CENTER:
        if( m_pDSB )
        {
        m_pDSB->SetPan(0);
        UpdatePanUI( 0, FALSE );
        }
        break;
    case ID_PANCONTEXT_10DB_LEFT:
        if( m_pDSB )
        {
        m_pDSB->SetPan(-1000);
        UpdatePanUI( -1000, FALSE );
        }
        break;
    case ID_PANCONTEXT_10DB_RIGHT:
        if( m_pDSB )
        {
        m_pDSB->SetPan(1000);
        UpdatePanUI( 1000, FALSE );
        }
        break;
    case ID_PANCONTEXT_FULL_LEFT:
        if( m_pDSB )
        {
        m_pDSB->SetPan(-10000);
        UpdatePanUI( -10000, FALSE );
        }
        break;
    case ID_PANCONTEXT_FULL_RIGHT:
        if( m_pDSB )
        {
        m_pDSB->SetPan(10000);
        UpdatePanUI( 10000, FALSE );
        }
        break;

    default:
        return FALSE;
    }

    return TRUE;
    }


///////////////////////////////////////////////////////////////////////////////
// HandleVolContext()
//
//    Handle WM_COMMAND messages from the Vol context menu.  Returns TRUE if a
// message was handled.
//
BOOL FileInfo::HandleVolContext( WPARAM wParam )
    {
    switch( LOWORD( wParam ))
    {
    case ID_VOLCONTEXT_0DB:
        if( m_pDSB )
        {
        m_pDSB->SetVolume(0);
        UpdateVolUI( 0, FALSE );
        }
        break;
    case ID_VOLCONTEXT_10DB:
        if( m_pDSB )
        {
        m_pDSB->SetVolume(-1000);
        UpdateVolUI( -1000, FALSE );
        }
        break;
    case ID_VOLCONTEXT_20DB:
        if( m_pDSB )
        {
        m_pDSB->SetVolume(-2000);
        UpdateVolUI( -2000, FALSE );
        }
        break;
    case ID_VOLCONTEXT_30DB:
        if( m_pDSB )
        {
        m_pDSB->SetVolume(-3000);
        UpdateVolUI( -3000, FALSE );
        }
        break;
    case ID_VOLCONTEXT_100DB:
        if( m_pDSB )
        {
        m_pDSB->SetVolume(-10000);
        UpdateVolUI( -10000, FALSE );
        }
        break;

    default:
        return FALSE;
    }

    return TRUE;
    }


///////////////////////////////////////////////////////////////////////////////
// HandleFreqContext()
//
//    Handle WM_COMMAND messages from the Freq context menu.  Returns TRUE if a
// message was handled.
//
BOOL FileInfo::HandleFreqContext( WPARAM wParam )
    {
    switch( LOWORD( wParam ))
    {
    case ID_FREQCONTEXT_FILEDEFAULT:
        if( m_pDSB )
        {
        if( SUCCEEDED( m_pDSB->SetFrequency(m_pwfx->nSamplesPerSec)))
            UpdateFreqUI( m_pwfx->nSamplesPerSec, FALSE );
        }
        break;

    case ID_FREQCONTEXT_8000HZ:
        if( m_pDSB )
        {
        if( SUCCEEDED( m_pDSB->SetFrequency(8000)))
            UpdateFreqUI( 8000, FALSE );
        }
        break;

    case ID_FREQCONTEXT_11025HZ:
        if( m_pDSB )
        {
        if( SUCCEEDED( m_pDSB->SetFrequency(11025)))
            UpdateFreqUI( 11025, FALSE );
        }
        break;

    case ID_FREQCONTEXT_22050HZ:
        if( m_pDSB )
        {
        if( SUCCEEDED( m_pDSB->SetFrequency(22050)))
            UpdateFreqUI( 22050, FALSE );
        }
        break;

    case ID_FREQCONTEXT_44100HZ:
        if( m_pDSB )
        {
        if( SUCCEEDED( m_pDSB->SetFrequency(44100)))
            UpdateFreqUI( 44100, FALSE );
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
    }

