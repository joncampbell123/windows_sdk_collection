#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include <commctrl.h>

#include "DSShow3D.h"
#include "debug.h"
#include "GVars.h"
#include "LsnrInfo.h"


ListenerInfo::ListenerInfo()
    {
    m_hDistanceEdit = m_hDopplerEdit = m_hRolloffEdit = NULL;
    m_hXSlider = m_hYSlider = m_hZSlider = NULL;
    m_hXText = m_hYText = m_hZText = NULL;

    m_vPos.x = m_vPos.y = m_vPos.z = (D3DVALUE)0.0;
    }


ListenerInfo::~ListenerInfo()
    {
    }


BOOL ListenerInfo::OnInitDialog( HWND hDlg, WPARAM wParam )
    {
    m_hDistanceEdit = GetDlgItem( hDlg, IDC_LISTENER_DISTANCEFACTOR_EDIT );
    m_hDopplerEdit = GetDlgItem( hDlg, IDC_LISTENER_DOPPLERFACTOR_EDIT );
    m_hRolloffEdit = GetDlgItem( hDlg, IDC_LISTENER_ROLLOFFFACTOR_EDIT );

    m_hXSlider = GetDlgItem( hDlg, IDC_LISTENER_X_SLIDER );
    m_hYSlider = GetDlgItem( hDlg, IDC_LISTENER_Y_SLIDER );
    m_hZSlider = GetDlgItem( hDlg, IDC_LISTENER_Z_SLIDER );

    m_hXText = GetDlgItem( hDlg, IDC_LISTENER_X_TEXT );
    m_hYText = GetDlgItem( hDlg, IDC_LISTENER_Y_TEXT );
    m_hZText = GetDlgItem( hDlg, IDC_LISTENER_Z_TEXT );

    // Initialize the distance factor to metres
    Edit_SetText( m_hDistanceEdit, "1.0" );
    Edit_SetText( m_hRolloffEdit, "1.0" );

    gp3DListener->SetPosition( D3DVAL(0.0f), D3DVAL(0.0f), D3DVAL(-10.0f), DS3D_IMMEDIATE );
//    gp3DListener->CommitDeferredSettings();

    SetSliders();

    return TRUE;
    }


void ListenerInfo::UpdateUI( void )
    {
    UpdateXSliderUI( 0, TRUE );
    UpdateYSliderUI( 0, TRUE );
    UpdateZSliderUI( 0, TRUE );
    }


BOOL ListenerInfo::OnCommand( WPARAM wParam, LPARAM lParam )
    {
    char    szEdit[16];

    if( HIWORD(wParam) == EN_CHANGE )
    {
    if( (HWND)lParam == m_hDistanceEdit )
        {
        Edit_GetText( m_hDistanceEdit, szEdit, sizeof(szEdit));

        ASSERT( NULL != gp3DListener );
        DPF( 3, "Setting Distance Factor: %s", szEdit );
        gp3DListener->SetDistanceFactor(D3DVAL(atof(szEdit)), DS3D_IMMEDIATE );
        }
    else if((HWND)lParam == m_hDopplerEdit )
        {
        Edit_GetText( m_hDopplerEdit, szEdit, sizeof(szEdit));

        ASSERT( NULL != gp3DListener );
        DPF( 3, "Setting Doppler Factor: %s", szEdit );
        gp3DListener->SetDopplerFactor(D3DVAL(atof(szEdit)), DS3D_IMMEDIATE );
        }
    else if((HWND)lParam == m_hRolloffEdit )
        {
        Edit_GetText( m_hRolloffEdit, szEdit, sizeof(szEdit));

        ASSERT( NULL != gp3DListener );
        DPF( 3, "Setting Rolloff Factor: %s", szEdit );
        gp3DListener->SetRolloffFactor(D3DVAL(atof(szEdit)), DS3D_IMMEDIATE );
        }
    }
    return FALSE;
    }


///////////////////////////////////////////////////////////////////////////////
// OnHScroll()
//
//    Main message handler for the WM_HSCROLL message.  this function basically
// figures out which horizontal scrolling control is responsible for sending
// the message and passes on handling to an appropriate function for handling.
//
BOOL ListenerInfo::OnHScroll( WORD wNotification, LONG lPos, HWND hControl )
    {
    if( !hControl )
    return FALSE;
    
    if( hControl == m_hXSlider )
    {
    HandleXSliderScroll( wNotification, lPos );
    return TRUE;
    }
    else if( hControl == m_hYSlider )
    {
    HandleYSliderScroll( wNotification, lPos );
    return TRUE;
    }
    else if( hControl == m_hZSlider )
    {
    HandleZSliderScroll( wNotification, lPos );
    return TRUE;
    }
    else
    return FALSE;
    }


void ListenerInfo::OnDestroy( void )
    {
    }


BOOL CALLBACK ListenerInfoDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
    {
    PLISTENERINFO   pli;

    switch( message )
    {
    case WM_INITDIALOG:
        ASSERT( lParam != NULL );
        pli = (PLISTENERINFO)lParam;
        SetWindowLong( hDlg, DWL_USER, (LONG)pli );

        if( !pli->OnInitDialog( hDlg, wParam ))
        DestroyWindow( hDlg );
        return TRUE;

    case WM_HSCROLL:
        pli = (PLISTENERINFO)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pli );
        return !pli->OnHScroll( LOWORD(wParam), (LONG)HIWORD(wParam), (HWND)lParam );

    case WM_COMMAND:
        pli = (PLISTENERINFO)GetWindowLong( hDlg, DWL_USER );
        // It's possible to get child notifications before the
        // INITDIALOG message, so we'll handle a NULL class item
        // here a less stringently
        if( !pli )
        return FALSE;
        return !pli->OnCommand( wParam, lParam );

    case WM_DESTROY:
        pli = (PLISTENERINFO)GetWindowLong( hDlg, DWL_USER );
        ASSERT( NULL != pli );
        pli->OnDestroy();
        return TRUE;

    default:
        return FALSE;
    }
    }


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ListenerInfo::SetSliders( void )
    {
    SendMessage( m_hXSlider, TBM_SETRANGEMIN, FALSE,
    (LPARAM)(LISTENER_SLIDER_MIN + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    SendMessage( m_hXSlider, TBM_SETRANGEMAX, FALSE,
    (LPARAM)(LISTENER_SLIDER_MAX + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    SendMessage( m_hXSlider, TBM_SETPAGESIZE, 0, LISTENER_SLIDER_FACTOR);

    SendMessage( m_hYSlider, TBM_SETRANGEMIN, FALSE,
    (LPARAM)(LISTENER_SLIDER_MIN + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    SendMessage( m_hYSlider, TBM_SETRANGEMAX, FALSE,
    (LPARAM)(LISTENER_SLIDER_MAX + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    SendMessage( m_hYSlider, TBM_SETPAGESIZE, 0, LISTENER_SLIDER_FACTOR);

    SendMessage( m_hZSlider, TBM_SETRANGEMIN, FALSE,
    (LPARAM)(LISTENER_SLIDER_MIN + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    SendMessage( m_hZSlider, TBM_SETRANGEMAX, FALSE,
    (LPARAM)(LISTENER_SLIDER_MAX + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    SendMessage( m_hZSlider, TBM_SETPAGESIZE, 0, LISTENER_SLIDER_FACTOR);

    UpdateUI();
    }


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ListenerInfo::HandleXSliderScroll( WORD wNot, LONG lPos )
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
        lPos = SendMessage( m_hXSlider, TBM_GETPOS, 0, 0 );
        break;

    default:
        fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != gp3DListener )
    {
    m_vPos.x = D3DVAL(lPos * LISTENER_SLIDER_FACTOR) - LISTENER_SLIDER_SHIFT;
    DPF( 3, "Setting listener pos: (%i, %i, %i)", (int)m_vPos.x, (int)m_vPos.y, (int)m_vPos.z );
    if( FAILED( hr = gp3DListener->SetPosition( m_vPos.x, m_vPos.y,
                        m_vPos.z, DS3D_IMMEDIATE )))
        DPF( 0, "IDirectSound3DListener::SetPosition returned : %s", TranslateDSError(hr));
    UpdateXSliderUI((lPos * LISTENER_SLIDER_FACTOR) - LISTENER_SLIDER_SHIFT, FALSE );
    }
    }


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ListenerInfo::HandleYSliderScroll( WORD wNot, LONG lPos )
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
        lPos = SendMessage( m_hYSlider, TBM_GETPOS, 0, 0 );
        break;

    default:
        fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != gp3DListener )
    {
    m_vPos.y = D3DVAL(lPos * LISTENER_SLIDER_FACTOR) - LISTENER_SLIDER_SHIFT;
    DPF( 3, "Setting listener pos: (%i, %i, %i)", (int)m_vPos.x, (int)m_vPos.y, (int)m_vPos.z );
    if( FAILED( hr = gp3DListener->SetPosition( m_vPos.x, m_vPos.y,
                        m_vPos.z, DS3D_IMMEDIATE )))
        DPF( 0, "IDirectSound3DListener::SetPosition returned : %s", TranslateDSError(hr));
    UpdateYSliderUI((lPos * LISTENER_SLIDER_FACTOR) - LISTENER_SLIDER_SHIFT, FALSE );
    }
    }


///////////////////////////////////////////////////////////////////////////////
//
//
//
//
void ListenerInfo::HandleZSliderScroll( WORD wNot, LONG lPos )
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
        lPos = SendMessage( m_hZSlider, TBM_GETPOS, 0, 0 );
        break;

    default:
        fUpdate = FALSE;
    }
    
    if( fUpdate && NULL != gp3DListener )
    {
    m_vPos.z = D3DVAL(lPos * LISTENER_SLIDER_FACTOR) - LISTENER_SLIDER_SHIFT;
    DPF( 3, "Setting listener pos: (%i, %i, %i)", (int)m_vPos.x, (int)m_vPos.y, (int)m_vPos.z );
    if( FAILED( hr = gp3DListener->SetPosition( m_vPos.x, m_vPos.y,
                        m_vPos.z, DS3D_IMMEDIATE )))
        DPF( 0, "IDirectSound3DListener::SetPosition returned : %s", TranslateDSError(hr));
    UpdateZSliderUI((lPos * LISTENER_SLIDER_FACTOR) - LISTENER_SLIDER_SHIFT, FALSE );
    }
    }


///////////////////////////////////////////////////////////////////////////////
// UpdateXSliderUI()
//
//    Update anything that displays the position of the X slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void ListenerInfo::UpdateXSliderUI( LONG lForcePos, BOOL fFromInterface )
    {
    char    szText[8];
    LONG    lPos;

    if( fFromInterface )
        {
    if( NULL != gp3DListener )
        {
        gp3DListener->GetPosition( &m_vPos );
        lPos = (LONG)m_vPos.x;
        }
    else
        lPos = 0;
    }
    else
    lPos = lForcePos;

    SendMessage( m_hXSlider, TBM_SETPOS, (WPARAM)TRUE,
            (LPARAM)(lPos + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_hXText, szText );
    }


///////////////////////////////////////////////////////////////////////////////
// UpdateYSliderUI()
//
//    Update anything that displays the position of the Y slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void ListenerInfo::UpdateYSliderUI( LONG lForcePos, BOOL fFromInterface )
    {
    char    szText[8];
    LONG    lPos;

    if( fFromInterface )
        {
    if( NULL != gp3DListener )
        {
        gp3DListener->GetPosition( &m_vPos );
        lPos = (LONG)m_vPos.y;
        }
    else
        lPos = 0;
    }
    else
    lPos = lForcePos;

    SendMessage( m_hYSlider, TBM_SETPOS, (WPARAM)TRUE,
            (LPARAM)(lPos + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_hYText, szText );
    }


///////////////////////////////////////////////////////////////////////////////
// UpdateZSliderUI()
//
//    Update anything that displays the position of the Z slider in some manner
// to reflect the new position, either from the buffer or from the parameter.
//
void ListenerInfo::UpdateZSliderUI( LONG lForcePos, BOOL fFromInterface )
    {
    char    szText[8];
    LONG    lPos;

    if( fFromInterface )
        {
    if( NULL != gp3DListener )
        {
        gp3DListener->GetPosition( &m_vPos );
        lPos = (LONG)m_vPos.z;
        }
    else
        lPos = 0;
    }
    else
    lPos = lForcePos;

    SendMessage( m_hZSlider, TBM_SETPOS, (WPARAM)TRUE,
            (LPARAM)(lPos + LISTENER_SLIDER_SHIFT) / LISTENER_SLIDER_FACTOR );
    wsprintf( szText, "%i", lPos );
    Static_SetText( m_hZText, szText );
    }




