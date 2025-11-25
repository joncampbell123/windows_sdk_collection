#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <commctrl.h>
#include <dsound.h>
#include <math.h>
#include <stdio.h>

#include "wave.h"
#include "debug.h"
#include "DSShow3D.h"
#include "FInfo3D.h"
#include "lsnrinfo.h"
#include "GVars.h"


///////////////////////////////////////////////////////////////////////////////
// FileInfo3D()
//
//    Class constructor for the 3D interface/information class.
//
FileInfo3D::FileInfo3D( MainWnd * pmw ) : FileInfo( pmw )
{
    ZeroMemory( &m_ht3d, sizeof(HWNDTABLE3D));
    ZeroMemory( &m_vPos, sizeof(D3DVECTOR));
    ZeroMemory( &m_vVelo, sizeof(D3DVECTOR));
    m_flMaxDistance = 50.f;
    m_flMinDistance = 1.f;
    m_pDSB3D = NULL;

    m_dwInternalFlags |= FI_INTERNALF_3D;

    m_dwInnerAngle = FI3D_DEFAULT_INNER_ANGLE;
    m_dwOuterAngle = FI3D_DEFAULT_OUTER_ANGLE;
}


///////////////////////////////////////////////////////////////////////////////
// ~FileInfo3D()
//
//   Virtual destructor for the FileInfo3D class.  Should deallocate any space
// allocated by the constructor.
//
FileInfo3D::~FileInfo3D()
{
    // If we do an allocation of memory specific to the FileInfo3D class,
    // we should free it here.

    // Release the 3D interface
    if( m_pDSB3D != NULL )
    {
        m_pDSB3D->Release();
        m_pDSB3D = NULL;
    }
}


///////////////////////////////////////////////////////////////////////////////
// NewDirectSoundBuffer()
// 
//    Virtual function which handles opening a new sound buFfer by setting some
// flags and calling down to the base class handler, which actually does most
// of the work.  Upon return, this function will do a bit more work to get a 3D
// interface.
//
int FileInfo3D::NewDirectSoundBuffer()
{
    HRESULT	hr;

    m_dsbd.dwFlags = DSBCAPS_CTRL3D; 

    if( !FileInfo::NewDirectSoundBuffer())
    {
        // Try to grab the 3D interface pointer

        if( FAILED( hr = m_pDSB->QueryInterface( IID_IDirectSound3DBuffer,
                                (void **)&m_pDSB3D )))
        {
            DPF( 0, "QI for DS3DBuffer interface: %s", TranslateDSError(hr));
            return -1;
        }
        return 0;
    }
    else
        return -1;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL FileInfo3D::OnInitDialog( HWND hDlg, WPARAM wParam )
{
    HRESULT hr;
    TCHAR   szTS[6];

    // Grab some 3D interface HWND's and pass along to the base class
    m_ht3d.hXSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_X_SLIDER );
    m_ht3d.hYSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_Y_SLIDER );
    m_ht3d.hZSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_Z_SLIDER );
    m_ht3d.hXText = GetDlgItem( hDlg, IDC_BUFFERDLG_X_TEXT );
    m_ht3d.hYText = GetDlgItem( hDlg, IDC_BUFFERDLG_Y_TEXT );
    m_ht3d.hZText = GetDlgItem( hDlg, IDC_BUFFERDLG_Z_TEXT );

    m_ht3d.hVxSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_VX_SLIDER );
    m_ht3d.hVySlider = GetDlgItem( hDlg, IDC_BUFFERDLG_VY_SLIDER );
    m_ht3d.hVzSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_VZ_SLIDER );
    m_ht3d.hVxText = GetDlgItem( hDlg, IDC_BUFFERDLG_VX_TEXT );
    m_ht3d.hVyText = GetDlgItem( hDlg, IDC_BUFFERDLG_VY_TEXT );
    m_ht3d.hVzText = GetDlgItem( hDlg, IDC_BUFFERDLG_VZ_TEXT );

    m_ht3d.hOuterVolSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_OUTERVOL_SLIDER );
    m_ht3d.hOuterVolText = GetDlgItem( hDlg, IDC_BUFFERDLG_OUTERVOL_TEXT );

    m_ht3d.hInnerAngleEdit = GetDlgItem( hDlg, IDC_BUFFERDLG_INNERANGLE_EDIT );
    m_ht3d.hInnerAngleSpin = GetDlgItem( hDlg, IDC_BUFFERDLG_INNERANGLE_SPIN );
    m_ht3d.hOuterAngleEdit = GetDlgItem( hDlg, IDC_BUFFERDLG_OUTERANGLE_EDIT );
    m_ht3d.hOuterAngleSpin = GetDlgItem( hDlg, IDC_BUFFERDLG_OUTERANGLE_SPIN );

    m_ht3d.hDisable3D = GetDlgItem( hDlg, IDC_BUFFERDLG_DISABLE );

    m_ht3d.hMaxDistSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_MAXMUTE_SLIDER );
    m_ht3d.hMaxDistText = GetDlgItem( hDlg, IDC_BUFFERDLG_MAXMUTE_TEXT );
    m_ht3d.hMinDistSlider = GetDlgItem( hDlg, IDC_BUFFERDLG_MINDIST_SLIDER );
    m_ht3d.hMinDistText = GetDlgItem( hDlg, IDC_BUFFERDLG_MINDIST_TEXT );
    m_ht3d.hCurDistText = GetDlgItem( hDlg, IDC_BUFFERDLG_CURDIST_TEXT );


    // The tab order in the dialog must be such that the spin control is
    // immediately after the edit control it is expected to auto-buddy to.
    // That case is tested here.
    ASSERT( (HWND)SendMessage( m_ht3d.hInnerAngleSpin,
                UDM_GETBUDDY, 0, 0 ) == m_ht3d.hInnerAngleEdit );

    // If this fails, the tab order in the dialog resource is wrong (see above)
    ASSERT( (HWND)SendMessage( m_ht3d.hOuterAngleSpin,
                UDM_GETBUDDY, 0, 0 ) == m_ht3d.hOuterAngleEdit );

    SendMessage( m_ht3d.hInnerAngleSpin, UDM_SETRANGE, 0, MAKELONG( 360, 0 ));
    SendMessage( m_ht3d.hOuterAngleSpin, UDM_SETRANGE, 0, MAKELONG( 360, 0 ));

    SendMessage( m_ht3d.hInnerAngleSpin, UDM_SETPOS,
                 0, MAKELONG(m_dwInnerAngle, 0));
    wsprintf( szTS, "%u", m_dwInnerAngle );
    Edit_SetText( m_ht3d.hInnerAngleEdit, szTS );

    SendMessage( m_ht3d.hOuterAngleSpin, UDM_SETPOS,
                 0, MAKELONG(m_dwOuterAngle, 0));
    wsprintf( szTS, "%u", m_dwOuterAngle );
    Edit_SetText( m_ht3d.hOuterAngleEdit, szTS );

    if( m_pDSB3D )
    {
        if( FAILED( hr = m_pDSB3D->SetConeAngles( m_dwInnerAngle,
                                                  m_dwOuterAngle, DS3D_IMMEDIATE )))
            DPF( 0, "SetConeAngles: %s", TranslateDSError(hr));

        m_pDSB3D->GetPosition( &m_vPos );
        m_pDSB3D->GetVelocity( &m_vVelo );
        m_pDSB3D->SetMaxDistance( m_flMaxDistance, DS3D_IMMEDIATE );
        m_pDSB3D->SetMinDistance( m_flMinDistance, DS3D_IMMEDIATE );
    }

    return FileInfo::OnInitDialog( hDlg, wParam );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL FileInfo3D::OnCommand( WPARAM wParam, LPARAM lParam )
{
    BOOL fRet = FALSE;

    if( HandleOuterVolContext( wParam ) 
        || HandlePositionContext( wParam )
        || HandleVelocityContext( wParam ) ) 
        fRet = TRUE;
    else if( HandleConeInnerEditNotify( wParam, lParam ))
        fRet = TRUE;
    else if( HandleConeOuterEditNotify( wParam, lParam ))
        fRet = TRUE;
    else if( HandleDisableNotify( wParam, lParam ))
        fRet = TRUE;
    else
        fRet = FileInfo::OnCommand( wParam, lParam );

    return fRet;
}


///////////////////////////////////////////////////////////////////////////////
// OnHScroll()
//
//
//
BOOL FileInfo3D::OnHScroll( WORD wNotification, LONG lPos, HWND hControl )
{
    if( !hControl )
        return FALSE;
    if( hControl == m_ht3d.hOuterVolSlider )
        HandleOuterVolSliderScroll( wNotification, lPos );

    else if( hControl == m_ht3d.hXSlider )
        HandleXSliderScroll( wNotification, lPos );
    else if( hControl == m_ht3d.hYSlider )
        HandleYSliderScroll( wNotification, lPos );
    else if( hControl == m_ht3d.hZSlider )
        HandleZSliderScroll( wNotification, lPos );

    else if( hControl == m_ht3d.hVxSlider )
        HandleVxSliderScroll( wNotification, lPos );
    else if( hControl == m_ht3d.hVySlider )
        HandleVySliderScroll( wNotification, lPos );
    else if( hControl == m_ht3d.hVzSlider )
        HandleVzSliderScroll( wNotification, lPos );

    else if( hControl == m_ht3d.hMaxDistSlider )
        HandleMaxDistSliderScroll( wNotification, lPos );
    else if( hControl == m_ht3d.hMinDistSlider )
        HandleMinDistSliderScroll( wNotification, lPos );
    
    // All messages we don't handle pass through to the base class
    else
        return FileInfo::OnHScroll( wNotification, lPos, hControl );
    
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::HandleOuterVolSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hOuterVolSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        if( FAILED( hr = m_pDSB3D->SetConeOutsideVolume(( lPos * VOL_SLIDER_FACTOR )
                        - VOL_SLIDER_SHIFT, DS3D_IMMEDIATE )))
            DPF( 0, "SetConeOutsideVolume: %s", TranslateDSError(hr));
        UpdateOuterVolUI(( lPos * VOL_SLIDER_FACTOR ) - VOL_SLIDER_SHIFT,
                                        FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::HandleXSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hXSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_vPos.x = D3DVAL(lPos * X_SLIDER_FACTOR) - POS_SLIDER_SHIFT;
        if( FAILED( hr = m_pDSB3D->SetPosition( m_vPos.x, m_vPos.y,
                            m_vPos.z, DS3D_IMMEDIATE )))
            DPF( 0, "SetPosition: %s", TranslateDSError(hr));
        UpdateXSliderUI((lPos * X_SLIDER_FACTOR) - POS_SLIDER_SHIFT, FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::HandleYSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hYSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_vPos.y = D3DVAL(lPos * Y_SLIDER_FACTOR) - POS_SLIDER_SHIFT;
        if( FAILED( hr = m_pDSB3D->SetPosition( m_vPos.x, m_vPos.y,
                            m_vPos.z, DS3D_IMMEDIATE )))
            DPF( 0, "SetPosition: %s", TranslateDSError(hr));
        UpdateYSliderUI((lPos * Y_SLIDER_FACTOR) - POS_SLIDER_SHIFT, FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::HandleZSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hZSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_vPos.z = D3DVAL(lPos * Z_SLIDER_FACTOR) - POS_SLIDER_SHIFT;
        if( FAILED( hr = m_pDSB3D->SetPosition( m_vPos.x, m_vPos.y,
                                                m_vPos.z, DS3D_IMMEDIATE )))
            DPF( 0, "SetPosition: %s", TranslateDSError(hr));
        UpdateZSliderUI((lPos * Z_SLIDER_FACTOR) - POS_SLIDER_SHIFT, FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::HandleVxSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hVxSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_vVelo.x = D3DVAL(lPos * X_SLIDER_FACTOR) - POS_SLIDER_SHIFT;
        if( FAILED( hr = m_pDSB3D->SetVelocity( m_vVelo.x, m_vVelo.y,
                            m_vVelo.z, DS3D_IMMEDIATE )))
            DPF( 0, "SetVelocity: %s", TranslateDSError(hr));
        UpdateVxSliderUI((lPos * X_SLIDER_FACTOR) - POS_SLIDER_SHIFT, FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::HandleVySliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hVySlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_vVelo.y = D3DVAL(lPos * Y_SLIDER_FACTOR) - POS_SLIDER_SHIFT;
        if( FAILED( hr = m_pDSB3D->SetVelocity( m_vVelo.x, m_vVelo.y,
                            m_vVelo.z, DS3D_IMMEDIATE )))
            DPF( 0, "SetVelocity: %s", TranslateDSError(hr));
        UpdateVySliderUI((lPos * Y_SLIDER_FACTOR) - POS_SLIDER_SHIFT, FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::HandleVzSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hVzSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_vVelo.z = D3DVAL(lPos * Z_SLIDER_FACTOR) - POS_SLIDER_SHIFT;
        if( FAILED( hr = m_pDSB3D->SetVelocity( m_vVelo.x, m_vVelo.y,
                                                m_vVelo.z, DS3D_IMMEDIATE )))
            DPF( 0, "SetVelocity: %s", TranslateDSError(hr));
        UpdateVzSliderUI((lPos * Z_SLIDER_FACTOR) - POS_SLIDER_SHIFT, FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
// HandleMaxDistSliderScroll
// function:        handles WM_HSCROLL messages sent on behalf of IDC_BUFFERDLG_MAXMUTE_SLIDER
//
void FileInfo3D::HandleMaxDistSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hMaxDistSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_flMaxDistance = D3DVAL(lPos * MUTEMAX_SLIDER_FACTOR);
        if( FAILED( hr = m_pDSB3D->SetMaxDistance( m_flMaxDistance, DS3D_IMMEDIATE )))
        {
            DPF( 0, "SetPosition: %s", TranslateDSError(hr));
        }

        UpdateMuteMaxSliderUI((lPos * MUTEMAX_SLIDER_FACTOR), FALSE );
    }
}

///////////////////////////////////////////////////////////////////////////////
// HandleMinDistSliderScroll
// function:        handles WM_HSCROLL messages sent on behalf of IDC_BUFFERDLG_MINDIST_SLIDER
//
void FileInfo3D::HandleMinDistSliderScroll( WORD wNot, LONG lPos )
{
    HRESULT hr;
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
            lPos = SendMessage( m_ht3d.hMinDistSlider, TBM_GETPOS, 0, 0 );
            break;

        default:
            fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != m_pDSB3D )
    {
        m_flMinDistance = D3DVAL(lPos * MINDIST_SLIDER_FACTOR);
        if( FAILED( hr = m_pDSB3D->SetMinDistance( m_flMinDistance, DS3D_IMMEDIATE )))
        {
            DPF( 0, "SetPosition: %s", TranslateDSError(hr));
        }

        UpdateMinDistSliderUI((lPos * MINDIST_SLIDER_FACTOR), FALSE );
    }
}


///////////////////////////////////////////////////////////////////////////////
// UpdateOuterVolUI()
//
//
//
//
//
void FileInfo3D::UpdateOuterVolUI( LONG lForceVol, BOOL fFromBuffer )
{
    HRESULT hr;
    LONG    lVol;
    TCHAR   szText[16];

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            if( FAILED( hr = m_pDSB3D->GetConeOutsideVolume( &lVol )))
            DPF( 0, "GetConeOutsideVolume: %s", TranslateDSError(hr));
        }
        else
            lVol = 0;
    }
    else
        lVol = lForceVol;

    SendMessage( m_ht3d.hOuterVolSlider, TBM_SETPOS, (WPARAM)TRUE,
                 (LPARAM)(lVol + VOL_SLIDER_SHIFT) / VOL_SLIDER_FACTOR );

    // Print volume in decibels
    wsprintf( szText, "%i dB", lVol / 100 );

    Static_SetText( m_ht3d.hOuterVolText, szText );
}

///////////////////////////////////////////////////////////////////////////////
//
//
//
//
BOOL FileInfo3D::OnContextMenu( HWND hDlg, int x, int y )
{
    HMENU   hm, hSub;
    int     nSubMenu = -1;
    POINT   pt = { x, y };
    RECT    rectWind1, rectWind2, rectWind3;

    // Set the sub-menu to the Outer Volume context menu if we hit the proper
    // slider or text control
    GetWindowRect( m_ht3d.hOuterVolSlider, &rectWind1 );
    GetWindowRect( m_ht3d.hOuterVolText, &rectWind2 );

    if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt ))
        nSubMenu = 3;

    // Set the POS_CONTEXT submenu if there's a click on the position sliders
    // or text controls
    GetWindowRect( m_ht3d.hXSlider, &rectWind1 );
    GetWindowRect( m_ht3d.hYSlider, &rectWind2 );
    GetWindowRect( m_ht3d.hZSlider, &rectWind3 );

    if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt )
        || PtInRect( &rectWind3, pt ))
        nSubMenu = 4;

    GetWindowRect( m_ht3d.hXText, &rectWind1 );
    GetWindowRect( m_ht3d.hYText, &rectWind2 );
    GetWindowRect( m_ht3d.hZText, &rectWind3 );

    if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt )
        || PtInRect( &rectWind3, pt ))
        nSubMenu = 4;

    // Set the VELO_CONTEXT submenu if there's a click on the position sliders
    // or text controls
    GetWindowRect( m_ht3d.hVxSlider, &rectWind1 );
    GetWindowRect( m_ht3d.hVySlider, &rectWind2 );
    GetWindowRect( m_ht3d.hVzSlider, &rectWind3 );

    if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt )
        || PtInRect( &rectWind3, pt ))
        nSubMenu = 5;

    GetWindowRect( m_ht3d.hVxText, &rectWind1 );
    GetWindowRect( m_ht3d.hVyText, &rectWind2 );
    GetWindowRect( m_ht3d.hVzText, &rectWind3 );

    if( PtInRect( &rectWind1, pt ) || PtInRect( &rectWind2, pt )
        || PtInRect( &rectWind3, pt ))
        nSubMenu = 5;

    // We didn't detect any "interesting" hotspots, so pass along to base class
    if( nSubMenu < 0 )
        return FileInfo::OnContextMenu( hDlg, x, y );

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
//
//
//
//
void FileInfo3D::OnDestroy()
{
    FileInfo::OnDestroy();
    return;
}


void FileInfo3D::Duplicate( FileInfo *pfiSource )
{
    HRESULT	hr;

    FileInfo::Duplicate( pfiSource );

    if( FAILED( hr = m_pDSB->QueryInterface( IID_IDirectSound3DBuffer,
                                             (void **)&m_pDSB3D )))
    {
        DPF( 0, "QI for DS3DBuffer interface: %s", TranslateDSError(hr));
    }
    else
    {
        // Update the UI from the interface we just got
        UpdateOuterVolUI( 0, TRUE );
        UpdateXSliderUI( 0, TRUE );
        UpdateYSliderUI( 0, TRUE );
        UpdateZSliderUI( 0, TRUE );
        UpdateVxSliderUI( 0, TRUE );
        UpdateVySliderUI( 0, TRUE );
        UpdateVzSliderUI( 0, TRUE );

        m_pDSB3D->GetConeAngles( &m_dwInnerAngle, &m_dwOuterAngle );

		// update the UI.
		UpdateMinDistSliderUI( 0, TRUE );
		UpdateMuteMaxSliderUI(0, TRUE);

        DWORD	dwMode;
        TCHAR	szTS[6];

        SendMessage( m_ht3d.hInnerAngleSpin, UDM_SETPOS,
                     0, MAKELONG(m_dwInnerAngle, 0));
        wsprintf( szTS, "%u", m_dwInnerAngle );
        Edit_SetText( m_ht3d.hInnerAngleEdit, szTS );

        SendMessage( m_ht3d.hOuterAngleSpin, UDM_SETPOS,
                     0, MAKELONG(m_dwOuterAngle, 0));
        wsprintf( szTS, "%u", m_dwOuterAngle );
        Edit_SetText( m_ht3d.hOuterAngleEdit, szTS );

        m_pDSB3D->GetMode( &dwMode );
        Button_SetCheck( m_ht3d.hDisable3D, dwMode == DS3DMODE_DISABLE );
    }
}


///////////////////////////////////////////////////////////////////////////////
// CreateInterface()
// 
//    Very similar to the interface creation code for 2D objects, but creates a
// different dialog.  (Eventually may do more stuff related to D3D display).
//
BOOL FileInfo3D::CreateInterface( HWND hwndOwner )
{
    m_hwndInterface = CreateDialogParam( ghInst, MAKEINTRESOURCE(IDD_BUFFER3D),
                                         hwndOwner, (DLGPROC)FileInfo3DDlgProc,
                                         (LPARAM)this );

    if( NULL == m_hwndInterface )
        goto FICI_Fail;
    else
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
//
//
//
//
void FileInfo3D::SetSliders( void )
{
    // Do 3D controls slider initialization

    // Intentionally set the range backwards because a large number means
    // a smaller value
    SendMessage( m_ht3d.hOuterVolSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(VOL_MIN + VOL_SLIDER_SHIFT) / VOL_SLIDER_FACTOR );
    SendMessage( m_ht3d.hOuterVolSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(VOL_MAX + VOL_SLIDER_SHIFT) / VOL_SLIDER_FACTOR );
    SendMessage( m_ht3d.hOuterVolSlider, TBM_SETPAGESIZE, 0,
                 VOL_SLIDER_PAGE / VOL_SLIDER_FACTOR );
    // NOTE: No TICs on the cone volume slider

    // position sliders
    SendMessage( m_ht3d.hXSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(POS_SLIDER_MIN + POS_SLIDER_SHIFT) / X_SLIDER_FACTOR );
    SendMessage( m_ht3d.hXSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(POS_SLIDER_MAX + POS_SLIDER_SHIFT) / X_SLIDER_FACTOR );
    SendMessage( m_ht3d.hXSlider, TBM_SETPAGESIZE, 0, X_SLIDER_FACTOR );

    SendMessage( m_ht3d.hYSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(POS_SLIDER_MIN + POS_SLIDER_SHIFT) / Y_SLIDER_FACTOR );
    SendMessage( m_ht3d.hYSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(POS_SLIDER_MAX + POS_SLIDER_SHIFT) / Y_SLIDER_FACTOR );
    SendMessage( m_ht3d.hYSlider, TBM_SETPAGESIZE, 0, Y_SLIDER_FACTOR );

    SendMessage( m_ht3d.hZSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(POS_SLIDER_MIN + POS_SLIDER_SHIFT) / Z_SLIDER_FACTOR );
    SendMessage( m_ht3d.hZSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(POS_SLIDER_MAX + POS_SLIDER_SHIFT) / Z_SLIDER_FACTOR );
    SendMessage( m_ht3d.hZSlider, TBM_SETPAGESIZE, 0, Z_SLIDER_FACTOR );

    // velocity sliders
    SendMessage( m_ht3d.hVxSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(POS_SLIDER_MIN + POS_SLIDER_SHIFT) / X_SLIDER_FACTOR );
    SendMessage( m_ht3d.hVxSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(POS_SLIDER_MAX + POS_SLIDER_SHIFT) / X_SLIDER_FACTOR );
    SendMessage( m_ht3d.hVxSlider, TBM_SETPAGESIZE, 0, X_SLIDER_FACTOR );

    SendMessage( m_ht3d.hVySlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(POS_SLIDER_MIN + POS_SLIDER_SHIFT) / Y_SLIDER_FACTOR );
    SendMessage( m_ht3d.hVySlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(POS_SLIDER_MAX + POS_SLIDER_SHIFT) / Y_SLIDER_FACTOR );
    SendMessage( m_ht3d.hVySlider, TBM_SETPAGESIZE, 0, Y_SLIDER_FACTOR );

    SendMessage( m_ht3d.hVzSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(POS_SLIDER_MIN + POS_SLIDER_SHIFT) / Z_SLIDER_FACTOR );
    SendMessage( m_ht3d.hVzSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(POS_SLIDER_MAX + POS_SLIDER_SHIFT) / Z_SLIDER_FACTOR );
    SendMessage( m_ht3d.hVzSlider, TBM_SETPAGESIZE, 0, Z_SLIDER_FACTOR );

    // max distance slider
    SendMessage( m_ht3d.hMaxDistSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(MUTEMAX_SLIDER_MIN) / MUTEMAX_SLIDER_FACTOR );
    SendMessage( m_ht3d.hMaxDistSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(MUTEMAX_SLIDER_MAX) / MUTEMAX_SLIDER_FACTOR );
    SendMessage( m_ht3d.hMaxDistSlider, TBM_SETPAGESIZE, 0, MUTEMAX_SLIDER_FACTOR );

    // Min distance slider
    SendMessage( m_ht3d.hMinDistSlider, TBM_SETRANGEMIN, FALSE,
                 (LPARAM)(MINDIST_SLIDER_MIN) / MINDIST_SLIDER_FACTOR );
    SendMessage( m_ht3d.hMinDistSlider, TBM_SETRANGEMAX, FALSE,
                 (LPARAM)(MINDIST_SLIDER_MAX) / MINDIST_SLIDER_FACTOR );
    SendMessage( m_ht3d.hMinDistSlider, TBM_SETPAGESIZE, 0, MINDIST_SLIDER_FACTOR );

//    SendMessage( m_ht3d.hOuterVolSlider, TBM_SETPAGESIZE, 0,
//	            VOL_SLIDER_PAGE / VOL_SLIDER_FACTOR );
    // NOTE: No TICs on the position sliders

    // Update the display from the buffer's current settings
    UpdateOuterVolUI( 0, TRUE );
    UpdateXSliderUI( 0, TRUE );
    UpdateYSliderUI( 0, TRUE );
    UpdateZSliderUI( 0, TRUE );
    UpdateVxSliderUI( 0, TRUE );
    UpdateVySliderUI( 0, TRUE );
    UpdateVzSliderUI( 0, TRUE );
    UpdateMuteMaxSliderUI( 0, TRUE );
    UpdateMinDistSliderUI( 0, TRUE );

    FileInfo::SetSliders();
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void FileInfo3D::UpdateUI( void )
{
    UpdatePosAndVelo();
    UpdateCurrentDistUI();
    UpdateXSliderUI( 0, TRUE );
    UpdateYSliderUI( 0, TRUE );
    UpdateZSliderUI( 0, TRUE );
    FileInfo::UpdateUI();
}


///////////////////////////////////////////////////////////////////////////////
// UpdateFileName()
//
//    Updates the file name which is displayed in the dialog window caption.
//
void FileInfo3D::UpdateFileName( void )
{
    TCHAR   szTitle[MAX_PATH + 5];

    if( NULL != m_hwndInterface )
    {
        lstrcpy( szTitle, &m_szFileName[m_nFileIndex] );
        lstrcat( szTitle, TEXT(" (3D)"));
        SendMessage( m_hwndInterface, WM_SETTEXT, 0L, (LPARAM)szTitle );
    }
}


///////////////////////////////////////////////////////////////////////////////
// FileInfo3DDlgProc()
//
//    
//
BOOL CALLBACK FileInfo3DDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    FileInfo3D *pfi3d;

    switch( message )
    {
        // The first step is to stash our class object pointer in the user data
        // and Initialize all our controls and internal data members.
        case WM_INITDIALOG:
            ASSERT( NULL != lParam );
            pfi3d = (FileInfo3D *)lParam;
            SetWindowLong( hDlg, DWL_USER, (LONG)pfi3d );

            if( !pfi3d->OnInitDialog( hDlg, wParam ))
            {
            DestroyWindow( hDlg );
            }
            return TRUE;

        case WM_COMMAND:
            pfi3d = (FileInfo3D*)GetWindowLong( hDlg, DWL_USER );
            // It's possible to get notification messages from child controls
            // before we have been given a WM_INITDIALOG message.  This is not
            // a good thing for dereferencing the pointer because we won't have
            // class info. Specifically, the spin controls in our dialog force
            // the edit control to send EN_CHANGE and EN_UPDATE messages when
            // they set the text of the edit control they're tied to.
            if( NULL == pfi3d || NULL == pfi3d->m_hwndInterface )
            return FALSE;
            return !pfi3d->OnCommand( wParam, lParam );

        // Handle this to deal with right-clicks on our controls -- we have a
        // bunch of different context menus that we can popup
        case WM_CONTEXTMENU:
            pfi3d = (FileInfo3D*)GetWindowLong( hDlg, DWL_USER );
            ASSERT( NULL != pfi3d );
            return pfi3d->OnContextMenu( hDlg, LOWORD(lParam), HIWORD(lParam));

        // Trackbar slider notifications come through here
        case WM_HSCROLL:
            pfi3d = (FileInfo3D*)GetWindowLong( hDlg, DWL_USER );
            ASSERT( NULL != pfi3d );
            return pfi3d->OnHScroll( LOWORD(wParam), (LONG)HIWORD(wParam), (HWND)lParam );

        case WM_DESTROY:
            pfi3d = (FileInfo3D*)GetWindowLong( hDlg, DWL_USER );
            ASSERT( NULL != pfi3d );
            pfi3d->OnDestroy();
            return TRUE;

        default:
            return FileInfoDlgProc( hDlg, message, wParam, lParam );
    }

    ASSERT( FALSE );
}


///////////////////////////////////////////////////////////////////////////////
// HandleOuterVolContext()
//
//    Pre-process the parameters to WM_COMMAND messages we get and sees if we
// knows how to handle any of them.  We know how to handle any message that
// comes from the Outer Volume controls' context menu.
//
BOOL FileInfo3D::HandleOuterVolContext( WPARAM wParam )
{
    HRESULT hr;

    switch( wParam )
    {
        case ID_OUTERVOLCONTEXT_0DB:
            if( FAILED( hr = m_pDSB3D->SetConeOutsideVolume( 0,
                                    DS3D_IMMEDIATE )))
            DPF( 0, "SetConeOutsideVolume: %s", TranslateDSError(hr));
            UpdateOuterVolUI( 0, TRUE );
            break;

        case ID_OUTERVOLCONTEXT_10DB:
            if( FAILED( hr = m_pDSB3D->SetConeOutsideVolume( -1000,
                                    DS3D_IMMEDIATE )))
            DPF( 0, "SetConeOutsideVolume: %s", TranslateDSError(hr));
            UpdateOuterVolUI( -1000, TRUE );
            break;

        case ID_OUTERVOLCONTEXT_20DB:
            if( FAILED( hr = m_pDSB3D->SetConeOutsideVolume( -2000,
                                    DS3D_IMMEDIATE )))
            DPF( 0, "SetConeOutsideVolume: %s", TranslateDSError(hr));
            UpdateOuterVolUI( -2000, TRUE );
            break;

        case ID_OUTERVOLCONTEXT_30DB:
            if( FAILED( hr = m_pDSB3D->SetConeOutsideVolume( -3000,
                                    DS3D_IMMEDIATE )))
            DPF( 0, "SetConeOutsideVolume: %s", TranslateDSError(hr));
            UpdateOuterVolUI( -3000, TRUE );
            break;

        case ID_OUTERVOLCONTEXT_100DB:
            if( FAILED( hr = m_pDSB3D->SetConeOutsideVolume( -10000,
                                    DS3D_IMMEDIATE )))
            DPF( 0, "SetConeOutsideVolume: %s", TranslateDSError(hr));
            UpdateOuterVolUI( -10000, TRUE );
            break;

        default:
            return FALSE;
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// HandlePositionContext()
//
//    Pre-process the parameters to WM_COMMAND messages we get and sees if we
// knows how to handle any of them.  We know how to handle any message that
// comes from the Position controls' context menu.
//
BOOL FileInfo3D::HandlePositionContext( WPARAM wParam )
{
    D3DVECTOR	d3dListener;

    switch( wParam )
    {
        case ID_POSCONTEXT_ORIGIN:
            ASSERT( NULL != m_pDSB3D );
            m_pDSB3D->SetPosition( D3DVAL(0), D3DVAL(0), D3DVAL(0), DS3D_IMMEDIATE );
            UpdateXSliderUI( 0, TRUE );
            UpdateYSliderUI( 0, TRUE );
            UpdateZSliderUI( 0, TRUE );
            break;

        case ID_POSCONTEXT_LISTENERPOSITION:
            ASSERT( NULL != m_pDSB3D );
            ASSERT( NULL != gp3DListener );
            gp3DListener->GetPosition( &d3dListener );
            m_pDSB3D->SetPosition( d3dListener.x, d3dListener.y,
                        d3dListener.z, DS3D_IMMEDIATE );
            UpdateXSliderUI( (DWORD)d3dListener.x, TRUE );
            UpdateYSliderUI( (DWORD)d3dListener.y, TRUE );
            UpdateZSliderUI( (DWORD)d3dListener.z, TRUE );
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// HandleVelocityContext()
//
//    Pre-process the parameters to WM_COMMAND messages we get and sees if we
// knows how to handle any of them.  We know how to handle any message that
// comes from the Position controls' context menu.
//
BOOL FileInfo3D::HandleVelocityContext( WPARAM wParam )
{
    switch( wParam )
    {
        case ID_VELOCONTEXT_ORIGIN:
            ASSERT( NULL != m_pDSB3D );
            m_pDSB3D->SetVelocity( D3DVAL(0), D3DVAL(0), D3DVAL(0), DS3D_IMMEDIATE );
            UpdateVxSliderUI( 0, TRUE );
            UpdateVySliderUI( 0, TRUE );
            UpdateVzSliderUI( 0, TRUE );
            m_vVelo.x = m_vVelo.y = m_vVelo.z = 0.f;
            break;

        default:
            return FALSE;
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// UpdateXSliderUI()
//
//    Update anything that displays the position of the X slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void FileInfo3D::UpdateXSliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR   szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetPosition( &m_vPos );
            lPos = (LONG)m_vPos.x;
        }
        else
            lPos = 0;
    }
    else
        lPos = lForcePos;

    SendMessage( m_ht3d.hXSlider, TBM_SETPOS, (WPARAM)TRUE,
                 (LPARAM)(lPos + POS_SLIDER_SHIFT) / X_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hXText, szText );

    // anything that updates position will change the current distance
    UpdateCurrentDistUI();
}


///////////////////////////////////////////////////////////////////////////////
// UpdateYSliderUI()
//
//    Update anything that displays the position of the Y slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void FileInfo3D::UpdateYSliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR   szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetPosition( &m_vPos );
            lPos = (LONG)m_vPos.y;
        }
        else
            lPos = 0;
    }
    else
        lPos = lForcePos;

    SendMessage( m_ht3d.hYSlider, TBM_SETPOS, (WPARAM)TRUE,
                 (LPARAM)(lPos + POS_SLIDER_SHIFT) / Y_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hYText, szText );

    // anything that updates position will change the current distance
    UpdateCurrentDistUI();
}


///////////////////////////////////////////////////////////////////////////////
// UpdateZSliderUI()
//
//    Update anything that displays the position of the Z slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void FileInfo3D::UpdateZSliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR	szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetPosition( &m_vPos );
            lPos = (LONG)m_vPos.z;
        }
        else
            lPos = 0;
    }
    else
        lPos = lForcePos;

    SendMessage( m_ht3d.hZSlider, TBM_SETPOS, (WPARAM)TRUE,
            (LPARAM)(lPos + POS_SLIDER_SHIFT) / Z_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hZText, szText );

    // anything that updates position will change the current distance
    UpdateCurrentDistUI();
}

///////////////////////////////////////////////////////////////////////////////
// UpdateVxSliderUI()
//
//    Update anything that displays the position of the Vx slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void FileInfo3D::UpdateVxSliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR   szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetVelocity( &m_vVelo );
            lPos = (LONG)m_vVelo.x;
        }
        else
            lPos = 0;
    }
    else
        lPos = lForcePos;

    SendMessage( m_ht3d.hVxSlider, TBM_SETPOS, (WPARAM)TRUE,
                 (LPARAM)(lPos + POS_SLIDER_SHIFT) / X_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hVxText, szText );

    // anything that updates position will change the current distance
    UpdateCurrentDistUI();
}


///////////////////////////////////////////////////////////////////////////////
// UpdateVySliderUI()
//
//    Update anything that displays the position of the Vy slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void FileInfo3D::UpdateVySliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR   szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetVelocity( &m_vVelo );
            lPos = (LONG)m_vVelo.y;
        }
        else
            lPos = 0;
    }
    else
        lPos = lForcePos;

    SendMessage( m_ht3d.hVySlider, TBM_SETPOS, (WPARAM)TRUE,
                 (LPARAM)(lPos + POS_SLIDER_SHIFT) / Y_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hVyText, szText );

    // anything that updates position will change the current distance
    UpdateCurrentDistUI();
}


///////////////////////////////////////////////////////////////////////////////
// UpdateVzSliderUI()
//
//    Update anything that displays the position of the Vz slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void FileInfo3D::UpdateVzSliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR	szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetVelocity( &m_vVelo );
            lPos = (LONG)m_vVelo.z;
        }
        else
            lPos = 0;
    }
    else
        lPos = lForcePos;

    SendMessage( m_ht3d.hVzSlider, TBM_SETPOS, (WPARAM)TRUE,
            (LPARAM)(lPos + POS_SLIDER_SHIFT) / Z_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hVzText, szText );

    // anything that updates position will change the current distance
    UpdateCurrentDistUI();
}

///////////////////////////////////////////////////////////////////////////////
// UpdateMuteMaxSliderUI()
//
// function:    Update anything that displays the position of the MaxDistance
//              slider in some manner to reflect the new position, either from 
//              the buffer or from the parameter.
//
void FileInfo3D::UpdateMuteMaxSliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR	szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetMaxDistance( &m_flMaxDistance );
            lPos = (LONG)m_flMaxDistance;
        }
        else
        {
            lPos = 0;
        }
    }
    else
    {
        lPos = lForcePos;
    }

    SendMessage( m_ht3d.hMaxDistSlider, TBM_SETPOS, (WPARAM)TRUE,
                 (LPARAM)lPos / MUTEMAX_SLIDER_FACTOR );

    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hMaxDistText, szText );
}

///////////////////////////////////////////////////////////////////////////////
// UpdateMinDistSliderUI()
//
// function:    Update anything that displays the position of the MinDistance
//              slider in some manner to reflect the new position, either from 
//              the buffer or from the parameter.
//
void FileInfo3D::UpdateMinDistSliderUI( LONG lForcePos, BOOL fFromBuffer )
{
    TCHAR	szText[8];
    LONG    lPos;

    if( fFromBuffer )
    {
        if( NULL != m_pDSB3D )
        {
            m_pDSB3D->GetMinDistance( &m_flMinDistance );
            lPos = (LONG)m_flMinDistance;
        }
        else
        {
            lPos = 0;
        }
    }
    else
    {
        lPos = lForcePos;
    }

    SendMessage( m_ht3d.hMinDistSlider, TBM_SETPOS, (WPARAM)TRUE,
                 (LPARAM)lPos / MINDIST_SLIDER_FACTOR );

    wsprintf( szText, "%i", lPos );
    Static_SetText( m_ht3d.hMinDistText, szText );
}

///////////////////////////////////////////////////////////////////////////////
// UpdateCurrentDistUI()
//
// function:    Update Current Distance text box in response to change in either
//              buffer or listener position
//
void FileInfo3D::UpdateCurrentDistUI( void )
{
    TCHAR	    szText[24];
    D3DVECTOR   d3dListPos;
    D3DVALUE    dx, dy, dz;
    double		dist;
    HRESULT     hr;
    // get current listener position
    if(SUCCEEDED (hr = gp3DListener->GetPosition( &d3dListPos )))
    {
        // find distance from listener to buffer
        dx = d3dListPos.x - m_vPos.x;
        dy = d3dListPos.y - m_vPos.y;
        dz = d3dListPos.z - m_vPos.z;

        dist = sqrt((dx * dx) + (dy * dy) + (dz * dz));

        // update the text box
        sprintf( szText, "%.2f", dist );
        Static_SetText( m_ht3d.hCurDistText, szText );
    }
}

///////////////////////////////////////////////////////////////////////////////
// UpdatePosAndVelo()
//
// function:    Update position as a function of velocity text box in response 
//              to timer click 
//
UpdatePosAndVelo();
void FileInfo3D::UpdatePosAndVelo( void )
{
    float flTimerPeriod = 100.f / (float)TIMERPERIOD;

    m_vPos.x += m_vVelo.x * flTimerPeriod;
    m_vPos.y += m_vVelo.y * flTimerPeriod;
    m_vPos.z += m_vVelo.z * flTimerPeriod;

    if(m_vPos.x > POS_SLIDER_MAX)
    {
        m_vPos.x = (float)POS_SLIDER_MAX;
        m_vVelo.x = -m_vVelo.x;
    }
    if(m_vPos.x < POS_SLIDER_MIN)
    {
        m_vPos.x = (float)POS_SLIDER_MIN;
        m_vVelo.x = -m_vVelo.x;
    }
    if(m_vPos.y > POS_SLIDER_MAX)
    {
        m_vPos.y = (float)POS_SLIDER_MAX;
        m_vVelo.y = -m_vVelo.y;
    }
    if(m_vPos.y < POS_SLIDER_MIN)
    {
        m_vPos.y = (float)POS_SLIDER_MIN;
        m_vVelo.y = -m_vVelo.y;
    }
    if(m_vPos.z > POS_SLIDER_MAX)
    {
        m_vPos.z = (float)POS_SLIDER_MAX;
        m_vVelo.z = -m_vVelo.z;
    }
    if(m_vPos.z < POS_SLIDER_MIN)
    {
        m_vPos.z = (float)POS_SLIDER_MIN;
        m_vVelo.z = -m_vVelo.z;
    }

    if( NULL != m_pDSB3D )
    {
        m_pDSB3D->SetPosition( m_vPos.x, m_vPos.y, m_vPos.z, DS3D_IMMEDIATE );
        m_pDSB3D->SetVelocity( m_vVelo.x, m_vVelo.y, m_vVelo.z, DS3D_IMMEDIATE );
        UpdateVxSliderUI(0, TRUE);
        UpdateVySliderUI(0, TRUE);
        UpdateVzSliderUI(0, TRUE);
    }
}


///////////////////////////////////////////////////////////////////////////////
BOOL FileInfo3D::HandleConeInnerEditNotify( WPARAM wParam, LPARAM lParam )
{
    HRESULT hr;
    BOOL    fRet = FALSE;
    LONG    lValue;
    TCHAR   szTS[6];

    if( HIWORD( wParam ) == EN_CHANGE )
    {
        if( (HWND)lParam == m_ht3d.hInnerAngleEdit )
        {
            Edit_GetText( m_ht3d.hInnerAngleEdit, szTS, sizeof(szTS));
            lValue = atol(szTS);
            if( lValue < 0 )
            {
                Edit_SetText( m_ht3d.hInnerAngleEdit, "0" );
                return TRUE;
            }
            else
                m_dwInnerAngle = (DWORD)lValue;

            // Make sure we haven't incremented the inner cone larger than
            // the outer cone
            if( m_dwInnerAngle > m_dwOuterAngle )
            {
                m_dwOuterAngle = m_dwInnerAngle;
                SendMessage( m_ht3d.hOuterAngleSpin, UDM_SETPOS,
                            0, MAKELONG(m_dwOuterAngle, 0));
                wsprintf( szTS, "%u", m_dwOuterAngle );
                Edit_SetText( m_ht3d.hOuterAngleEdit, szTS );
            }
            if( m_pDSB3D )
                if( FAILED( hr = m_pDSB3D->SetConeAngles( m_dwInnerAngle,
                            m_dwOuterAngle, DS3D_IMMEDIATE )))
                    DPF( 0, "SetConeAngles: %s", TranslateDSError(hr));
            fRet = TRUE;
        }
    }

    return fRet;
}


BOOL FileInfo3D::HandleConeOuterEditNotify( WPARAM wParam, LPARAM lParam )
{
    HRESULT hr;
    BOOL    fRet = FALSE;
    LONG    lValue;
    TCHAR   szTS[6];

    if( HIWORD( wParam ) == EN_CHANGE )
    {
        if( (HWND)lParam == m_ht3d.hOuterAngleEdit )
        {
            Edit_GetText( m_ht3d.hOuterAngleEdit, szTS, sizeof(szTS));
            lValue = atol(szTS);
            if( lValue < 0 )
            {
                Edit_SetText( m_ht3d.hOuterAngleEdit, "0" );
                return TRUE;
            }
            else
                m_dwOuterAngle = (DWORD)lValue;

            // Make sure we haven't decremented the outer cone smaller than
            // the inner cone
            if( m_dwInnerAngle > m_dwOuterAngle )
            {
                m_dwInnerAngle = m_dwOuterAngle;
                SendMessage( m_ht3d.hInnerAngleSpin, UDM_SETPOS,
                            0, MAKELONG(m_dwInnerAngle, 0));
                wsprintf( szTS, "%u", m_dwInnerAngle );
                Edit_SetText( m_ht3d.hInnerAngleEdit, szTS );
            }
            if( m_pDSB3D )
                if( FAILED( hr = m_pDSB3D->SetConeAngles( m_dwInnerAngle,
                            m_dwOuterAngle, DS3D_IMMEDIATE )))
                    DPF( 0, "SetConeAngles: %s", TranslateDSError(hr));
            fRet = TRUE;
        }
    }

    return fRet;
}

BOOL FileInfo3D::HandleDisableNotify( WPARAM wParam, LPARAM lParam )
{
    HRESULT hr;
    BOOL    fRet = FALSE;

    if( (HWND)lParam == m_ht3d.hDisable3D )
    {
        if( Button_GetCheck( m_ht3d.hDisable3D ))
        {
            if( m_pDSB3D ) {
            if( FAILED( hr = m_pDSB3D->SetMode( DS3DMODE_DISABLE, 0 )))
                DPF( 0, "SetMode: %s", TranslateDSError(hr));
            fRet = TRUE;
            }
        }
        else
        {
            if( m_pDSB3D ) {
            if( FAILED( hr = m_pDSB3D->SetMode( DS3DMODE_NORMAL, 0 )))
                DPF( 0, "SetMode: %s", TranslateDSError(hr));
            fRet = TRUE;
            }
        }
    }

    return fRet;
}

