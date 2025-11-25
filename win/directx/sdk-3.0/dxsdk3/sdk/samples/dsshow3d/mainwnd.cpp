#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>

#include "resource.h"
#include "DSShow3D.h"
#include "GVars.h"

#include "MainWnd.h"
#include "FInfo3D.h"
#include "FileInfo.h"
#include "LsnrInfo.h"
#include "wave.h"
#include "debug.h"

static HWND hMainWndClient;

MainWnd::MainWnd()
{
    m_fCreated = FALSE;
    m_hwnd = NULL;
    m_n3DBuffers = 0;
    ZeroMemory( &m_dscaps, sizeof(DSCAPS));
    m_dscaps.dwSize = sizeof(m_dscaps);
}


MainWnd::~MainWnd()
{
    // This situation should never really occur, but we should make sure
    // that there are no coding errors by asserting that fact.
    if( m_hwnd )
    {
    ASSERT_HWND(m_hwnd);
    ::DestroyWindow( m_hwnd );
    m_hwnd = NULL;
    m_fCreated = FALSE;
    }
}


BOOL MainWnd::Create()
{
    if( m_fCreated )
    return FALSE;

    ASSERT( NULL == m_hwnd );
    m_hwnd = CreateWindowEx( WS_EX_ACCEPTFILES,
                gszAppWndClass,
                gszAppCaption,
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                200,
                150,
                (HWND)NULL,
                (HMENU)NULL,
                (HANDLE)ghInst,
                (LPSTR)NULL );

    if( !m_hwnd )
    return FALSE;

    SetWindowLong( m_hwnd, GWL_USERDATA, (LONG)this );
    m_fCreated = TRUE;

    EnableMenuItem(GetMenu(m_hwnd), 2, MF_BYPOSITION |
           (m_dlInfoList.GetElementCount() ? MF_ENABLED : MF_GRAYED));
    DrawMenuBar( m_hwnd );

    UpdateStatus();

    return TRUE;
}


int MainWnd::MessageBox( LPCSTR lpcszMessage, UINT uType )
{
    return ::MessageBox( m_hwnd, lpcszMessage, gszAppName, uType );
}

int MainWnd::MessageBox( UINT uResID, UINT uType )
{
    // TODO Make the 512 some defined constant
    LPTSTR  lptszMessage = new TCHAR[512];
    int     nRet;

    if( !lptszMessage )
    return 0;

    LoadString( ghInst, uResID, lptszMessage, 512 );
    nRet = ::MessageBox( m_hwnd, lptszMessage, gszAppName, uType );
    delete[] lptszMessage;

    return nRet;
}

BOOL MainWnd::OnTimer( WPARAM wParam, LPARAM lParam )
{
    // Move to the Head of the list
    m_dlInfoList.SetAtHead();
    
    if( NULL != gpListenerInfo )
    gpListenerInfo->UpdateUI();

    // While the current element isn't NULL, call OnTimer on each FileInfo
    for( int i = 0; i < m_dlInfoList.GetElementCount(); i++ )
    {
    m_dlInfoList.GetCurrent()->UpdateUI();
    // Overloaded increment moves to next position in the list
    m_dlInfoList++;
    }
    return TRUE;
}

void MainWnd::OnDestroy()
{
    HRESULT     hr = 0;

    if( gdwTimer != 0 )
    {
    KillTimer( m_hwnd, gdwTimer );
    gdwTimer = 0;
    }

    if( NULL != gpwfxFormat )
    {
    delete gpwfxFormat;
    gpwfxFormat = NULL;
    }

    /* Destroy the direct sound object. */
    if( gp3DListener != NULL )
    {
    DPF( 3, "Releasing 3D Listener in MainWnd::OnDestroy()" );
    gp3DListener->Release();
    gp3DListener = NULL;
    }
    if( gpdsbPrimary )
    {
    DPF( 3, "Releasing Primary in MainWnd::OnDestroy()" );
    gpdsbPrimary->Stop();
    gpdsbPrimary->Release();
    gpdsbPrimary = NULL;
    }
    if( gpds != NULL )
    {
    DPF( 3, "Releasing DSound object in MainWnd::OnDestroy()" );
    gpds->Release();
    gpds = NULL;
    }

    if( gfCOMInitialized )
    CoUninitialize();

//    WriteProfileString( gszAppName, "EnumDrivers", gfEnumDrivers ? "1" : "0" );

    if( m_hwnd )
    m_hwnd = NULL;
    return;
}


/* MainWndProc()
 *
 *    Window message handler function for the main application window.
 * For the most part, this sucker just dispatchs calls to some helper
 * functions in this module which do the actual handling.
 */
LRESULT CALLBACK MainWndProc( HWND hWnd, unsigned message, WPARAM wParam, LPARAM lParam )
{
    if( !hWnd )
    return 0L;

    // This will be a valid pointer to the class object for any message
    // after the WM_CREATE
    MainWnd *pmw = (MainWnd *)GetWindowLong( hWnd, GWL_USERDATA );
    
    switch (message)
    {
    case WM_CREATE:
        // If we want to do anything with the class here, we'll have
        // to modify the code to pass pmw in LPARAM since there's no
        // other way to get it yet: the code that sets the WindowLong
        // user data has not yet executed
        break;

    case WM_ACTIVATE:
        ASSERT( NULL != pmw );
        pmw->UpdateStatus();
        break;

    case WM_TIMER:
        ASSERT( NULL != pmw );
        if (!pmw->OnTimer(wParam, lParam))
        return(DefWindowProc(hWnd, message, wParam, lParam));
        break;

    case WM_DROPFILES:
        ASSERT( NULL != pmw );
        if( !pmw->OnDropFiles( wParam ))
        return(DefWindowProc(hWnd, message, wParam, lParam));
        break;

    case WM_PAINT:
        ASSERT( NULL != pmw );
        if( !pmw->OnPaint(wParam, lParam))
        return(DefWindowProc(hWnd, message, wParam, lParam));
        break;

    case WM_INITMENU:
        ASSERT( NULL != pmw );
        if((HMENU)wParam != GetMenu( hWnd ))
        break;
        return pmw->OnInitMenu( wParam );
        break;

    case WM_COMMAND:
        ASSERT( NULL != pmw );
        if (!pmw->OnCommand(wParam, lParam))
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;

    case WM_DESTROY:
        ASSERT( NULL != pmw );
        pmw->OnDestroy();
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc( hWnd, message, wParam, lParam );
        break;
    }

    return 0L;
}


BOOL MainWnd::OnInitMenu( WPARAM wParam )
{
    int i;

    CheckMenuItem( (HMENU)wParam,
           CommandIDFromFormatCode( gdwOutputFormat ),
           MF_BYCOMMAND | MF_CHECKED );

    for( i = 0; i < NUM_FORMATENTRIES; i++ )
    {
    if( !fdFormats[i].fEnable )
        EnableMenuItem((HMENU)wParam,
               CommandIDFromFormatCode( fdFormats[i].dwCode ),
               MF_BYCOMMAND | MF_GRAYED );
    else
        EnableMenuItem((HMENU)wParam,
               CommandIDFromFormatCode( fdFormats[i].dwCode ),
               MF_BYCOMMAND | MF_ENABLED );
    }
    // If there are no buffers open, then disable the buffers menu
    EnableMenuItem((HMENU)wParam, 2, MF_BYPOSITION |
           (m_dlInfoList.GetElementCount() ? MF_ENABLED : MF_GRAYED));
    return TRUE;
}

/* OnDropFiles()
 *
 *    Handles the WM_DROPFILES message, which is a message sent when someone
 * drags and drops files onto our application window.
 */
BOOL MainWnd::OnDropFiles( WPARAM wParam )
{
    WORD    wNumFiles = DragQueryFile((HDROP)wParam, (UINT)-1, NULL, (UINT)0 );
    WORD    nFile, nPathLength;
    LPSTR   lpszFile;

    for( nFile = 0; nFile < wNumFiles; nFile++ )
    {
    nPathLength = DragQueryFile((HDROP)wParam, nFile, NULL, 0 ) + 1;
    if(( lpszFile = new char[nPathLength] ) == NULL )
        continue;
    DragQueryFile((HDROP)wParam, nFile, lpszFile, nPathLength );
    
    // Open the file
    OnFileOpen( lpszFile );
    
    delete[] lpszFile;
    }

    DragFinish((HDROP)wParam);
    return TRUE;
}


/* OnCommand()
 *
 *    WM_COMMAND message handler for the main window procedure.
 */
BOOL MainWnd::OnCommand( WPARAM wParam, LPARAM lParam )
{
    HRESULT hr;
    DWORD   dwNewFormat;
    int     i = 0;

    if(( dwNewFormat = FormatCodeFromCommandID( LOWORD(wParam))) != 0 )
    {
    CheckMenuItem( GetMenu( m_hwnd ),
                CommandIDFromFormatCode( gdwOutputFormat ),
                MF_BYCOMMAND | MF_UNCHECKED );
    FormatCodeToWFX( dwNewFormat, gpwfxFormat );
    DPF( 2, "Setting output format to code: %lu", dwNewFormat );
    hr = gpdsbPrimary->SetFormat( gpwfxFormat );
    if( SUCCEEDED( hr ))
        gdwOutputFormat = dwNewFormat;
    else
        {
        DisableFormatCode( dwNewFormat );
        EnableMenuItem( GetMenu( m_hwnd ),
                CommandIDFromFormatCode( dwNewFormat ),
                MF_BYCOMMAND | MF_GRAYED );
        FormatCodeToWFX( gdwOutputFormat, gpwfxFormat );
        gpdsbPrimary->SetFormat( gpwfxFormat );
        }
    return TRUE;
    }

    switch( LOWORD(wParam))
    {
    case IDC_FILE_EXIT:
        PostMessage(WM_CLOSE, 0, 0);
        break;

    case IDC_FILE_OPEN:
        OnFileOpen( NULL );
        UpdateStatus();
        break;

    case IDC_OPTIONS_SETTINGS:
        DialogBox(ghInst, MAKEINTRESOURCE(IDD_SETTINGS),
                        m_hwnd, (DLGPROC)SettingsDlgProc);
        UpdateStatus();
        break;

    case IDC_BUFFERS_MINIMIZEALL:
        m_dlInfoList.SetAtHead();
        for( i = 0; i < m_dlInfoList.GetElementCount(); i++)
        {
        m_dlInfoList.GetCurrent()->MinimizeWindow();
        m_dlInfoList++;
        }
        break;

    case IDC_BUFFERS_RESTOREALL:
        m_dlInfoList.SetAtHead();
        for( i = 0; i < m_dlInfoList.GetElementCount(); i++)
        {
        m_dlInfoList.GetCurrent()->RestoreWindow();
        m_dlInfoList++;
        }
        break;

    case IDC_BUFFERS_CLOSEALL:
        while( m_dlInfoList.GetElementCount())
        {
        m_dlInfoList.SetAtHead();
        m_dlInfoList.GetCurrent()->Close();
        // Element is removed from the list by processing elsewhere
        }
        EnableMenuItem(GetMenu(m_hwnd), 2, MF_BYPOSITION |
               (m_dlInfoList.GetElementCount() ? MF_ENABLED : MF_GRAYED));
        DrawMenuBar( m_hwnd );
        break;

    case IDC_BUFFERS_CASCADE:
        m_dlInfoList.SetAtHead();
        for( i = 0; i < m_dlInfoList.GetElementCount(); i++)
        {
        if( 0 == i )
            m_dlInfoList.GetCurrent()->ResetCascade();
        m_dlInfoList.GetCurrent()->CascadeWindow();
        m_dlInfoList++;
        }
        break;

    case IDC_HELP_ABOUT:
        DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUT),
                        m_hwnd, (DLGPROC)AboutDlgProc);
        break;

    default:
        return FALSE;
    }

    /* If we broke out of the switch, it means we handled the message
     * so we return TRUE to indicate this.
     */
    return TRUE;
}


/* DuplicateBuffer()
 *
 *    Does the work of duplicating a FileInfo object.
 */
void MainWnd::DuplicateBuffer( FileInfo *pfiSource )
{
    FileInfo    *pfiNew;

    if( NULL == pfiSource )
    return;

    if( m_dlInfoList.GetElementCount() >= MAXCONTROLS )
    {
    MessageBox( "No more controls allowed" );
        return;
    }

    if( pfiSource->Is3D())
    {
    m_n3DBuffers++;
    pfiNew = new FileInfo3D( this );
    }
    else
    pfiNew = new FileInfo( this );

    if( NULL == pfiNew )
    return;

    pfiNew->Duplicate( pfiSource );
    m_dlInfoList.Append( pfiNew );
}


/* OnFileOpen()
 *
 *    Handles the File|Open menu command.
 */
FileInfo *MainWnd::OnFileOpen( LPSTR lpszForcePath )
{
    char            szFileName[MAX_PATH];
    FileInfo        *pFileInfo = NULL;
    LPSTR       lpszFileTitle;
    int             nFileIndex;
    DWORD       dwFlags;

    if( m_dlInfoList.GetElementCount() >= MAXCONTROLS )
    {
    MessageBox( "No more controls allowed" );
        return NULL;
    }

    if( NULL != lpszForcePath )
    {
    dwFlags = OPENFILENAME_F_GETPOS2;
    if( grs.dwDefaultFocusFlag & DSBCAPS_STICKYFOCUS )
        dwFlags |= OPENFILENAME_F_STICKYFOCUS;
    else if( grs.dwDefaultFocusFlag & DSBCAPS_GLOBALFOCUS )
        dwFlags |= OPENFILENAME_F_GLOBALFOCUS;

    if( grs.fOpen3D )
        dwFlags |= OPENFILENAME_F_3D;
    lpszFileTitle = strrchr( lpszForcePath, '\\' );
    if( NULL == lpszFileTitle )
        nFileIndex = 0;
    else
        nFileIndex =  lpszFileTitle - lpszForcePath + sizeof(char);
    lstrcpy( szFileName, lpszForcePath );
    }
    else
    {
    // Open the file, and check its format, etc.
    if( !OpenFileDialog( m_hwnd, szFileName, &nFileIndex, &dwFlags ))
        return NULL;
    }

    // Allocate the memory for the structure.
    if( dwFlags & OPENFILENAME_F_3D )
    {
    pFileInfo = new FileInfo3D( this );
    }
    else
    pFileInfo = new FileInfo( this );

    if( NULL == pFileInfo )
    {
    MessageBox( "Cannot add this file -- out of memory",
                            MB_OK|MB_ICONSTOP );
    goto ERROR_DONE_ROUTINE;
    }

    if( m_dlInfoList.GetElementCount() == 0 )
    pFileInfo->ResetCascade();

    m_dlInfoList.Append( pFileInfo );

    pFileInfo->SetSticky((dwFlags & OPENFILENAME_F_STICKYFOCUS)
                            ? TRUE : FALSE );
    pFileInfo->SetGlobal((dwFlags & OPENFILENAME_F_GLOBALFOCUS)
                            ? TRUE : FALSE );
    pFileInfo->Set3D((dwFlags & OPENFILENAME_F_3D) ? TRUE : FALSE );
    pFileInfo->SetUseGetPos2((dwFlags & OPENFILENAME_F_GETPOS2)
                            ? TRUE : FALSE );

    if( pFileInfo->LoadWave( szFileName, nFileIndex ) != 0 )
    {
    m_dlInfoList.Remove( pFileInfo );
    goto ERROR_DONE_ROUTINE;
    }

    // If we fail after this, make sure to update the list!!!
    if( pFileInfo->Is3D())
    {
    if( m_n3DBuffers++ == 0 )
        {
        // We only want to create this dialog once, and if this is the
        // first 3D buffer opened, then there should be no listener DLG yet
        ASSERT( NULL == ghwndListener );

        gpListenerInfo = new ListenerInfo;
        ghwndListener = CreateDialogParam( ghInst,
                        MAKEINTRESOURCE(IDD_3DLISTENER),
                        AppWnd.GetHwnd(),
                        (DLGPROC)ListenerInfoDlgProc,
                        (LPARAM)(gpListenerInfo));
        ::ShowWindow( ghwndListener, SW_SHOW );
        }
    }

    // If there are no buffers open, then disable the buffers menu
    EnableMenuItem(GetMenu(m_hwnd), 2, MF_BYPOSITION |
           (m_dlInfoList.GetElementCount() ? MF_ENABLED : MF_GRAYED));
    DrawMenuBar( m_hwnd );

    return pFileInfo;


ERROR_DONE_ROUTINE:
    if( pFileInfo != NULL )
    {
    delete pFileInfo;
    pFileInfo = NULL;
    }
    return NULL;
}


/* DestroyFileInfo()
 *
 *    Destroys a FileInfo structure and removes it from the list.
 */
void MainWnd::DestroyFileInfo( FileInfo *pfi )
{
    if( NULL == pfi )
    return;

    if( pfi->Is3D() )
    {
    if( --m_n3DBuffers == 0 )
        {
        ::DestroyWindow( ghwndListener );
        ghwndListener = NULL;
        delete gpListenerInfo;
        gpListenerInfo = NULL;
        }
    }

    m_dlInfoList.Remove( pfi );
    delete pfi;
    UpdateStatus();
}


BOOL MainWnd::OnPaint( WPARAM wParam, LPARAM lParam )
    {
    PAINTSTRUCT ps;
    RECT    rcClient;
    SIZE    sizeExtent;
    char    szText[128];

    if( !BeginPaint( m_hwnd, &ps ))
    return FALSE;

    GetClientRect( m_hwnd, &rcClient );
    
    SetBkMode( ps.hdc, TRANSPARENT );

    wsprintf( szText, "Free HW Mem: %luKb", m_dscaps.dwFreeHwMemBytes / 1024 );
    GetTextExtentPoint32( ps.hdc, szText, lstrlen(szText), &sizeExtent );
    
    DrawText( ps.hdc, szText, -1, &rcClient, DT_TOP | DT_LEFT );

    wsprintf( szText, "Free HW Buffers: %lu", m_dscaps.dwFreeHwMixingStaticBuffers );
    rcClient.top += sizeExtent.cy;
    DrawText( ps.hdc, szText, -1, &rcClient, DT_TOP | DT_LEFT );

    EndPaint( m_hwnd, &ps );
    return TRUE;
    }


void MainWnd::UpdateStatus( void )
    {
    if( gpds )
    {
    // Get updated statistics on the DSound device
    gpds->GetCaps( &m_dscaps );
    
    // Force a window client area repaint
    InvalidateRect( m_hwnd, NULL, TRUE );
    UpdateWindow();
    }
    }

////////////////////////////////////////////////////////////////////////////
// BatchOpenFiles()
//
//    Takes an array of string pointers and tries to open each as a file to
// playback.  If fPlay is TRUE, the files will be played as they are being
// opened.  If fLoop is TRUE, they will also be set to loop.
//
// Returns: FALSE in the event of catastrophic failure, otherwise TRUE.
//
BOOL MainWnd::BatchOpenFiles( PSTR *ppszFiles, int nFiles, BOOL fPlay, BOOL fLoop )
{
    FileInfo    *pfi;
    int     i;

    // Cap the number of files we can load out of the given set if we'd load
    // too many otherwise
    if( m_dlInfoList.GetElementCount() >= MAXCONTROLS )
    {
    MessageBox( "No more controls allowed" );
        return FALSE;
    }

    for( i = 0; i < nFiles; i++ )
    {
    if(( pfi = OnFileOpen( ppszFiles[i] )) == NULL )
        continue;

    // LOOP is only obeyed if PLAY was also specified
    if( fPlay )
    {
        if( fLoop )
        {
        pfi->SetLooped( TRUE );
        }
        pfi->PlayBuffer();
    }

    pfi->UpdateUI();
    }

    return TRUE;
}

