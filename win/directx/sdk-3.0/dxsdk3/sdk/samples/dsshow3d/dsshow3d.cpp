/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       DSShow3d.c
 *  Content:    Direct Sound show-off, including 3D Sound.
 *
 *
 ***************************************************************************/

//#pragma warning( disable: 4102 4101 )

#define INITGUID
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <commdlg.h>
#include <cguid.h>

#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <dsound.h>

#include "DSShow3D.h"
#define INIT_GVARS
#include "GVars.h"

#include "MainWnd.h"
#include "FileInfo.h"

#include "wave.h"
#include "debug.h"

static char szOpenStartDir[MAX_PATH];

// Format codes used to compactly represent each possible output format
//
// The code is: FFCBB where...
//
//    FF -> Frequency 8=8000, 11=11025, 22=22050, 44=44100
//    C  -> # of channels (1 or 2)
//    BB -> Bits 08 is 8-bit, 16 is 16-bit
//
// Use the FormatCodeToWFX() function to set a WAVEFORMATEX structure
// based on a format code, or FormatCodeToText() to get a text string
// representing the format.
//

#define FC_GETFREQCODE(fc)  ((fc) / 1000)
#define FC_GETBITS(fc)      ((fc) % 100)
#define FC_GETCHANNELS(fc)  (((fc) % 1000) / 100)

// Functions limited in scope to this file

static BOOL InitializeDSound( void );
static void FreeDSound( void );
static BOOL InitInstance( HINSTANCE, LPSTR, int );
static BOOL InitPrimarySoundBuffer( void );
static void GetMediaPath( LPSTR, int );
static BOOL LoadRegistrySettings( void );
static BOOL SaveRegistrySettings( void );
static BOOL ParseCommandLine( LPSTR lpszCmdLine );
static BOOL fMatchToken( PSTR pszString, PSTR pszDatum, int cchLen );
static BOOL fGetToken( PSTR pszStart, PSTR *ppszRet, int *pcchRet );


/* InitializeDSound()
 *
 *    Initialize the DirectSound object and other stuff we'll use like the
 * primary buffer and 3D listener object.
 */
BOOL InitializeDSound( void )
    {
    HRESULT hr;

    hr = CoCreateInstance( CLSID_DirectSound, NULL, CLSCTX_INPROC_SERVER,
                    IID_IDirectSound, (void**)&gpds );

    if (FAILED(hr) || (NULL == gpds))
    {
    DPF( 0, "Could not create a DSound object (%s)", TranslateDSError(hr));
    MessageBox( AppWnd.GetHwnd(), "Unable to get a DirectSound object",
                    "COM Failure", MB_OK | MB_ICONSTOP );
    goto ID_ExitError;
    }
    DPF( 2, "Got an IDirectSound object" );

    if( grs.fDefaultDevice )
    {
    DPF( 2, "Using default device as first choice" );
    hr = gpds->Initialize( &GUID_NULL );
    }
    else
    {
    if( FAILED( hr = gpds->Initialize( &grs.guPreferredDevice )))
        {
        DPF( 0, "Initialize failed on preferred device, using default" );
        if( IDNO == MessageBox( GetActiveWindow(),
            "Unable to use preferred device. Use default instead?",
            gszAppName, MB_YESNO ))
        {
        DPF( 2, "User chose not to use default device instead" );
        goto ID_ExitReleaseDS;
        }
        else
        {
        DPF( 2, "Falling back to default device" );
        hr = gpds->Initialize( &GUID_NULL );
        }
        }
    }
    if (FAILED(hr))
    {
    DPF( 0, "Failed Initialize() on IDirectSound object (%s)", TranslateDSError(hr));
    MessageBox( AppWnd.GetHwnd(), "Could not initialize DirectSound object",
                    gszAppName, MB_OK | MB_ICONSTOP );
    goto ID_ExitReleaseDS;
    }

    if( grs.fUseExclusiveMode )
    {
    hr = gpds->SetCooperativeLevel( AppWnd.GetHwnd(), DSSCL_EXCLUSIVE);
        DPF( 3, "Received DSSCL_EXCLUSIVE" );
    }
    else
    {
    hr = gpds->SetCooperativeLevel( AppWnd.GetHwnd(), DSSCL_PRIORITY);
        DPF( 3, "Received DSSCL_PRIORITY" );
    }
    if (FAILED(hr))
    {
    DPF( 0, "Couldn't SetCooperativeLevel() (%s)", TranslateDSError(hr));
    MessageBox( AppWnd.GetHwnd(), "Could not SetCooperativeLevel()",
                    gszAppName, MB_OK | MB_ICONSTOP );
    goto ID_ExitReleaseDS;
    }

    DPF( 3, "Creating Primary Buffer" );

    if( !InitPrimarySoundBuffer())
    {
    DPF( 0, "Failed on call to InitPrimarySoundBuffer()" );
    goto ID_ExitReleaseDS;
    }

    // Return SUCCESS
    return TRUE;

ID_ExitReleaseDS:
    // The InitPrimarySoundBuffer() call should have cleaned up
    // after itself if it failed

    ASSERT( NULL == gp3DListener );
    ASSERT( NULL == gpdsbPrimary );

    if( NULL != gpds )
    {
    gpds->Release();
    gpds = NULL;
    }

ID_ExitError:
    return FALSE;
    }


void FreeDSound()
    {
    if( NULL != gpdsbPrimary )
    {
    gpdsbPrimary->Stop();
    gpdsbPrimary->Release();
    gpdsbPrimary = NULL;
    }
    if( NULL != gp3DListener )
    {
    gp3DListener->Release();
    gp3DListener = NULL;
    }
    if( NULL != gpds )
    {
    gpds->Release();
    gpds = NULL;
    }
    }


/* InitInstance()
 *
 *    This function is responsible for all initialization that must occur
 * when a new instance of our application is started.
 */
BOOL InitInstance( HINSTANCE hInstance, LPSTR lpszCommandLine, int cmdShow )
    {
    WNDCLASS    myClass;

    myClass.hCursor             = LoadCursor(NULL, IDC_ARROW);
    myClass.hIcon               = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_SPEAKER_RGB_3D));
    myClass.lpszMenuName        = MAKEINTRESOURCE(IDR_MAINMENU);
    myClass.lpszClassName       = (LPSTR)gszAppWndClass;
    myClass.hbrBackground       = (HBRUSH)(COLOR_APPWORKSPACE + 1);
    myClass.hInstance           = hInstance;
    myClass.style               = CS_HREDRAW | CS_VREDRAW;
    myClass.lpfnWndProc         = MainWndProc;
    myClass.cbClsExtra          = 0;
    myClass.cbWndExtra          = 0;

    if (!RegisterClass( &myClass ))
       return FALSE;

    // Load the current registry settings
    LoadRegistrySettings();
    gdwOutputFormat = grs.dwPreferredFormat;

    if( !AppWnd.Create())
    goto II_ExitError;

    AppWnd.ShowWindow( cmdShow );
    AppWnd.UpdateWindow();

    /* Continue doing other initialization stuff */

    // Setup the timer...
    if ((gdwTimer = SetTimer(AppWnd.GetHwnd(), 1, TIMERPERIOD, NULL)) == 0)
    {
    MessageBox(AppWnd.GetHwnd(), "Cannot allocate timer, aborting", gszAppCaption,
            MB_OK|MB_ICONSTOP);
    goto II_ExitError;
    }

    // Get the largest waveformatex structure.
    if (MMSYSERR_NOERROR != acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT,
                            &gcbMaxWaveFormatSize))
        {
    MessageBox(AppWnd.GetHwnd(), "ACM Metrics failed, aborting", gszAppCaption,
           MB_OK|MB_ICONSTOP);
    goto II_ExitError;
    }

    // Initialize the COM subsystem and create our DirectSound stuff

    if (!SUCCEEDED(CoInitialize(NULL)))
    {
    MessageBox( AppWnd.GetHwnd(), "Failed to initialize COM library",
            gszAppCaption, MB_OK | MB_ICONSTOP);
    goto II_ExitError;
    }
    else
    gfCOMInitialized = TRUE;

    // Initialize the DirectSound global interfaces
    if( !InitializeDSound())
    goto II_ExitShutdownCOM;

    if( !ParseCommandLine( lpszCommandLine ))
    goto II_ExitFreeDSound;

    return TRUE;    // TRUE indicates success


II_ExitFreeDSound:
    FreeDSound();

II_ExitShutdownCOM:
    if( gfCOMInitialized )
    {
    DPF( 0, "Calling CoUninitialize from InitInstance error cleanup" );
    CoUninitialize();
    }

II_ExitError:

    return FALSE;   // FALSE indicates failure on initialization
    }   // InitInstance()


/* WinMain()
 *
 *   Main entry point for this program's execution.  Everything starts here.
 */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpszCmdLine, int cmdShow)
    {
    MSG   msg;

    DbgInitialize( TRUE );

    InitCommonControls();

    // Save instance handle
    ghInst = hInstance;

    if (!InitInstance(hInstance, lpszCmdLine, cmdShow))
    return 0;

    /* Polling messages from event queue */
    while (GetMessage((LPMSG)&msg, NULL, 0, 0))
    {
    // Only Translate and Dispatch the message if it's not going
    // to one of our many modeless dialog windows
    if( !IsDialogMessage( ghwndListener, &msg )
        && (NULL == ghDlgActive || !IsDialogMessage( ghDlgActive, &msg )))
        {
        TranslateMessage((LPMSG)&msg);
        DispatchMessage((LPMSG)&msg);
        }
    }

    UnregisterClass(gszAppWndClass, hInstance);
    return (int)msg.wParam;
    }   // WinMain()


/* InitPrimarySoundBuffer()
 *
 *    Creates and initializes the primary sound buffer for the application.
 * We need the primary buffer in order to get the 3D listener interface and
 * also to select output format type.
 */
BOOL InitPrimarySoundBuffer( void )
    {
    HRESULT     hr;
    DSBUFFERDESC    dsbd;
    int         nCurFormat;

    ZeroMemory( &dsbd, sizeof(DSBUFFERDESC));
    
    gpwfxFormat = new WAVEFORMATEX;
    if( NULL == gpwfxFormat )
    return FALSE;

    ZeroMemory( gpwfxFormat, sizeof(WAVEFORMATEX));

    gpwfxFormat->wFormatTag = WAVE_FORMAT_PCM;

    FormatCodeToWFX( gdwOutputFormat, gpwfxFormat );
    DPF( 2, "Initial format code: %lu", gdwOutputFormat );

    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

    if( FAILED( hr = gpds->CreateSoundBuffer( &dsbd, &gpdsbPrimary, NULL )))
    {
    DPF( 0, "Couldn't create primary buffer (%s)", TranslateDSError(hr));
    goto IPSB_ExitError;
    }

    if( FAILED( hr = gpdsbPrimary->SetFormat( gpwfxFormat )))
    {
    DisableFormatCode( gdwOutputFormat );

    // If we couldn't load the desired format, then try everything starting
    // at really high quality, and degrading to 8M8 (yuck)
    nCurFormat = 0;
    DPF( 2, "Unable to SetFormat() preferred format to %lu", gdwOutputFormat );

    while( FAILED( hr ) && nCurFormat < NUM_FORMATENTRIES )
        {
        gdwOutputFormat = aFormatOrder[nCurFormat];
        FormatCodeToWFX( gdwOutputFormat, gpwfxFormat );
        DPF( 2, "Trying format code: %lu", gdwOutputFormat );

        hr = gpdsbPrimary->SetFormat( gpwfxFormat );
        if( FAILED(hr))
        {
        DisableFormatCode( gdwOutputFormat );
        DPF( 2, "Return from SetFormat on primary buffer = %s", TranslateDSError(hr));
        }
        else
        {
        EnableFormatCode( gdwOutputFormat );
        DPF( 2, "Succeeded on SetFormat() for code %lu", gdwOutputFormat );
        }

        nCurFormat++;
        }
    }
    if( FAILED( hr ))
    {
    DPF( 0, "Failed SetFormat() on all formats!" );
    goto IPSB_ExitError;
    }

    if( FAILED( hr = gpdsbPrimary->QueryInterface( IID_IDirectSound3DListener,
                        (void**)&gp3DListener)))
    {
    DPF( 0, "Couldn't QI primary buffer for 3D listener interface (%s)", TranslateDSError(hr));
    goto IPSB_ExitError;
    }
    if( FAILED( hr = gpdsbPrimary->Play( 0, 0, DSBPLAY_LOOPING )))
    {
    DPF( 0, "Couldn't play primary buffer (%s)", TranslateDSError(hr));
    goto IPSB_ExitRelease;
    }

    return TRUE;

IPSB_ExitRelease:
    if( gp3DListener )
    {
    DPF( 0, "Releasing 3D Listener in InitPrimarySoundBuffer() error cleanup" );
    gp3DListener->Release();
    gp3DListener = NULL;
    }
    if( gpdsbPrimary )
    {
    DPF( 0, "Releasing Primary in InitPrimarySoundBuffer() error cleanup" );
    gpdsbPrimary->Stop();
    gpdsbPrimary->Release();
    gpdsbPrimary = NULL;
    }

IPSB_ExitError:
    return FALSE;
    }


/*  This will pop up the open file dialog and allow the user to pick one file.

    Input:
    hWnd        -   Handle of parent window.
    pszFileName -   String to store filename in, must be >= MAX_PATH long.


    Output:
    TRUE if a file was picked successfully or FALSE if the user didn't
    pick a file)

 */
BOOL OpenFileDialog( HWND hWnd, LPSTR pszFileName,
                    int *nFileName, LPDWORD lpdwFlags )
    {
    BOOL            fReturn,
            fValid;
    OPENFILENAME    ofn;

    pszFileName[0]          = 0;

    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = hWnd;
    ofn.hInstance           = ghInst;
    ofn.lpstrFilter         = "Wave Files\0*.wav\0All Files\0*.*\0\0";
    ofn.lpstrCustomFilter   = NULL;
    ofn.nMaxCustFilter      = 0;
    ofn.nFilterIndex        = 1;
    ofn.lpstrFile           = pszFileName;
    ofn.nMaxFile            = MAX_PATH;
    ofn.lpstrFileTitle      = NULL;
    ofn.nMaxFileTitle       = 0;
    ofn.lpstrInitialDir     = grs.szInitialDir;
    ofn.lpstrTitle          = "File Open";
    ofn.Flags               = OFN_FILEMUSTEXIST | OFN_EXPLORER
                                | OFN_ENABLETEMPLATE | OFN_ENABLEHOOK
                | OFN_HIDEREADONLY;
    ofn.nFileOffset         = 0;
    ofn.nFileExtension      = 0;
    ofn.lpstrDefExt         = "wav";
    ofn.lCustData           = (LONG)lpdwFlags;
    ofn.lpfnHook            = FileOpenCustomTemplateDlgProc;
    ofn.lpTemplateName      = MAKEINTRESOURCE(IDD_FILEOPEN_NEST);

    fValid = FALSE;
    do
    {
    if( fReturn = GetOpenFileName( &ofn ))
    {
    fValid = IsValidWave( pszFileName );
    if( !fValid )
    MessageBox( hWnd, "Wave files must be PCM format!",
                "Invalid Wave File", MB_OK|MB_ICONSTOP );
    else
    *nFileName = ofn.nFileOffset;
    }
    else
    fValid = TRUE;         // Force break out of loop.
    } while( !fValid );

    return fReturn;
    }

/*
 *
 *
This function will determine if the filename passed
    in is a valid wave for this
    app, that is a PCM wave.

    Input:
    pszFileName -   FileName to check.

    Output:
    FALSE if not a valid wave, TRUE if it is.

*/
BOOL IsValidWave( LPSTR pszFileName )
    {
    BOOL        fReturn = FALSE;
    int             nError = 0;
    HMMIO           hmmio;
    MMCKINFO        mmck;
    WAVEFORMATEX    *pwfx;

    if ((nError = WaveOpenFile(pszFileName, &hmmio, &pwfx, &mmck)) != 0)
    {
    goto ERROR_IN_ROUTINE;
    }

    if (pwfx->wFormatTag != WAVE_FORMAT_PCM)
    {
    goto ERROR_IN_ROUTINE;
    }

    WaveCloseReadFile(&hmmio, &pwfx);

    fReturn = TRUE;

ERROR_IN_ROUTINE:
    return fReturn;
    }


/* AboutDlgProc()
 *
 *    Standard dialog procedure for the About box.  As simple as can be.
 */
BOOL CALLBACK AboutDlgProc( HWND hWnd, UINT uMsg,
                        WPARAM wParam, LPARAM lParam )
    {
    switch(uMsg)
    {
    case WM_INITDIALOG:
        break;

    case WM_COMMAND:
        switch(wParam)
        {
        case ID_OK:
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;

        default:
        break;

        }
        break;

    case WM_CLOSE:
        EndDialog(hWnd, 0);
        break;

    default:
        return FALSE;
        break;

    }

    return TRUE;
}


/* FileOpenCustomTemplateDlgProc()
 *
 * This "hook procedure" is called by the common dialog code for certain
 *   events that may occur during the life of our nested dialog structure.
 *   We nest the Explorer style dialog inside our file open dialog so we
 *   can add a check box for stick buffers.
 */
UINT CALLBACK FileOpenCustomTemplateDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
    {
    static LPOPENFILENAME   lpofn = NULL;
    static HWND         hStickyRadio, h3DCheck, hLocalRadio, hGlobalRadio;
    static HWND         hGetPosRadio, hGetPos2Radio;

    switch( message )
    {
    case WM_INITDIALOG:
        lpofn = (LPOPENFILENAME)lParam;

    h3DCheck = GetDlgItem( hDlg, IDC_FONEST_3D );
    hLocalRadio = GetDlgItem( hDlg, IDC_FONEST_LOCAL_RADIO );
        hStickyRadio = GetDlgItem( hDlg, IDC_FONEST_STICKY_RADIO );
    hGlobalRadio = GetDlgItem( hDlg, IDC_FONEST_GLOBAL_RADIO );
    hGetPosRadio = GetDlgItem( hDlg, IDC_FONEST_GETPOS_RADIO );
    hGetPos2Radio = GetDlgItem( hDlg, IDC_FONEST_GETPOS2_RADIO );

    Button_SetCheck( hGetPos2Radio, TRUE );

    if( grs.dwDefaultFocusFlag & DSBCAPS_STICKYFOCUS )
        Button_SetCheck( hStickyRadio, TRUE );
    else if( grs.dwDefaultFocusFlag & DSBCAPS_GLOBALFOCUS )
        Button_SetCheck( hGlobalRadio, TRUE );
    else
        {
        ASSERT( grs.dwDefaultFocusFlag == 0 );
        Button_SetCheck( hLocalRadio, TRUE );
        }

    Button_SetCheck( h3DCheck, grs.fOpen3D );

    *((LPDWORD)lpofn->lCustData) = 0;

        return TRUE;


    case WM_NOTIFY:
        switch(((LPOFNOTIFY)lParam)->hdr.code)
        {
        case CDN_SELCHANGE:
            /* Use this area to process anything that must be updated when the
             * user changes the selection in the Common Dialog Box.
             *   NOTE: Provided only for informational purposes
             */
            return FALSE;

        case CDN_FILEOK:
            /* We can do lots of things in this notification message.  The most
             * important is that we can decide whether the Common Dialog call
             * will go through or whether it will fail.  I decided to handle
             * the checkbox control in this one place versus 4 others...
             */
            ASSERT( lpofn != NULL );
        /* Set flags to match the current state of the check box controls */

        /* Use normal GetPosition */
        *((LPDWORD)lpofn->lCustData) |=
        Button_GetCheck(hGetPosRadio)? OPENFILENAME_F_GETPOS : 0;
        /* Use DSBCAPS_GETCURRENTPOSITION2 */
        *((LPDWORD)lpofn->lCustData) |=
        Button_GetCheck(hGetPos2Radio)? OPENFILENAME_F_GETPOS2 : 0;

        /* Local buffer focus */
        *((LPDWORD)lpofn->lCustData) |=
        Button_GetCheck(hStickyRadio)? OPENFILENAME_F_LOCALFOCUS : 0;
        /* Sticky buffer focus */
        *((LPDWORD)lpofn->lCustData) |=
        Button_GetCheck(hStickyRadio)? OPENFILENAME_F_STICKYFOCUS : 0;
        /* Global sound focus */
        *((LPDWORD)lpofn->lCustData) |=
        Button_GetCheck(hGlobalRadio)? OPENFILENAME_F_GLOBALFOCUS : 0;
        /* 3D Buffer */
        *((LPDWORD)lpofn->lCustData) |=
            Button_GetCheck(h3DCheck)? OPENFILENAME_F_3D : 0;
            /* Returning zero signifies that we "approve" of the OK command,
             * and allows the common dialog to finish.
             */
            return FALSE;
        }
        /* Let the default dialog do/continue processing */
        return FALSE;
    }
    return FALSE;
}


/* SettingsDlgProc()
 *
 *    DialogBox procedure for the Settings dialog, which sets a bunch of
 * the application-global settings stored in our global REGSETTINGS struct.
 */
BOOL CALLBACK SettingsDlgProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
    {
    static BOOL fSettingsSaved;
    static HWND hInitialDirEdit, hDeviceCombo, hDeviceText, hDefaultCheck;
    static HWND hFormatCombo, hExclusiveCheck, hOpen3DCheck, hFocusCombo;

    int     ndx, i, idxLocal, idxSticky, idxGlobal;
    char    szFormat[32];
    LPGUID  lpguTemp;

    switch( message )
    {
    case WM_INITDIALOG:
        hInitialDirEdit = GetDlgItem( hDlg, IDC_SETTINGS_INITIALDIR_EDIT );
        hDeviceCombo = GetDlgItem( hDlg, IDC_SETTINGS_DSD_DEVICE_COMBO );
        hFormatCombo = GetDlgItem( hDlg, IDC_SETTINGS_DSD_FORMAT_COMBO );
        hFocusCombo = GetDlgItem( hDlg, IDC_SETTINGS_FOCUS_COMBO );
        hDeviceText = GetDlgItem( hDlg, IDC_SETTINGS_DSD_DEVICE_TEXT );
        hDefaultCheck = GetDlgItem( hDlg, IDC_SETTINGS_DSD_DEFAULT_CHECK );
        hExclusiveCheck = GetDlgItem( hDlg, IDC_SETTINGS_EXCLUSIVE_CHECK );
        hOpen3DCheck = GetDlgItem( hDlg, IDC_SETTINGS_OPEN3D_CHECK );

        // We use this flag to determine the return value from DialogBox()
        // FALSE indicates no change, TRUE means a change occurred.
        fSettingsSaved = FALSE;

        // Add all the format strings to the combo box
        for( i = 0; i < NUM_FORMATENTRIES; i ++ )
        {
        // Only add formats that are available on this card (this will
        // ignore any formats we couldn't SetFormat() with at startup,
        // and any we have since found to be unusable as well).
        if( !fdFormats[i].fEnable )
            continue;

        if( FormatCodeToText(fdFormats[i].dwCode, szFormat, sizeof(szFormat)))
            {
            ndx = ComboBox_AddString( hFormatCombo, szFormat );
            ComboBox_SetItemData( hFormatCombo, ndx, fdFormats[i].dwCode );
            }
        }

        DirectSoundEnumerate( (LPDSENUMCALLBACK)DSEnumProc, (LPVOID)&hDeviceCombo );

        // Add the three focus choices to the listbox, and set the item data
        // for each to the appropriate flag
        idxLocal = ComboBox_AddString( hFocusCombo, "Local" );
        ComboBox_SetItemData( hFocusCombo, idxLocal, 0 );
        idxSticky = ComboBox_AddString( hFocusCombo, "Sticky" );
        ComboBox_SetItemData( hFocusCombo, idxSticky, DSBCAPS_STICKYFOCUS );
        idxGlobal = ComboBox_AddString( hFocusCombo, "Global" );
        ComboBox_SetItemData( hFocusCombo, idxGlobal, DSBCAPS_GLOBALFOCUS );

        // Select the proper Drop List item
        if( grs.dwDefaultFocusFlag == DSBCAPS_STICKYFOCUS )
        ComboBox_SetCurSel( hFocusCombo, idxSticky );
        else if( grs.dwDefaultFocusFlag == DSBCAPS_GLOBALFOCUS )
        ComboBox_SetCurSel( hFocusCombo, idxGlobal );
        else
        ComboBox_SetCurSel( hFocusCombo, idxLocal );

        // Set the states of checkboxes and controls that depend on them
        Button_SetCheck( hDefaultCheck, grs.fDefaultDevice );
        Button_SetCheck( hExclusiveCheck, grs.fUseExclusiveMode );
        Button_SetCheck( hOpen3DCheck, grs.fOpen3D );
        Static_Enable( hDeviceCombo, !grs.fDefaultDevice );
        Static_Enable( hDeviceText, !grs.fDefaultDevice );

        Edit_LimitText( hInitialDirEdit, sizeof(grs.szInitialDir));
        Edit_SetText( hInitialDirEdit, grs.szInitialDir );

        if (FormatCodeToText(grs.dwPreferredFormat, szFormat, sizeof(szFormat)))
        {
        ComboBox_SetCurSel( hFormatCombo, ComboBox_FindString( hFormatCombo,
                                    -1, szFormat ));
        }

        return TRUE;

    case WM_COMMAND:
        switch( LOWORD(wParam))
        {
        case IDC_SETTINGS_DSD_DEFAULT_CHECK:
            grs.fDefaultDevice = !grs.fDefaultDevice;
            Static_Enable( hDeviceCombo, !grs.fDefaultDevice );
            Static_Enable( hDeviceText, !grs.fDefaultDevice );
            // By selecting the first item, we wipe out the state where
            // the user could come in with the box checked, uncheck it,
            // and leave without selecting anything
            ComboBox_SetCurSel( hDeviceCombo, 0 );
            break;

        case IDC_SETTINGS_BROWSE_INITIALDIR_BUTTON:
            {
            OPENFILENAME    ofn;

            ZeroMemory( &ofn, sizeof(ofn));
            // Fill in the ofn structure and do the Common Dialog call
            }
            break;

        case IDOK:
            grs.fDefaultDevice = Button_GetCheck( hDefaultCheck );
            if( !grs.fDefaultDevice )
            {
            lpguTemp = (LPGUID)ComboBox_GetItemData( hDeviceCombo,
                        ComboBox_GetCurSel( hDeviceCombo ));
            if( NULL != lpguTemp )
                grs.guPreferredDevice = *lpguTemp;
            else
                grs.guPreferredDevice = GUID_NULL;
            }
            else
            grs.guPreferredDevice = GUID_NULL;

            grs.fOpen3D = Button_GetCheck( hOpen3DCheck );
            grs.fUseExclusiveMode = Button_GetCheck( hExclusiveCheck );
            grs.dwPreferredFormat = ComboBox_GetItemData( hFormatCombo,
                        ComboBox_GetCurSel( hFormatCombo ));
            grs.dwDefaultFocusFlag = ComboBox_GetItemData( hFocusCombo,
                        ComboBox_GetCurSel( hFocusCombo ));
            Edit_GetText( hInitialDirEdit, grs.szInitialDir,
                        sizeof(grs.szInitialDir));

            SaveRegistrySettings();
            fSettingsSaved = TRUE;
            SendMessage( hDlg, WM_CLOSE, 0, 0 );
            break;

        case IDCANCEL:
            fSettingsSaved = FALSE;
            SendMessage( hDlg, WM_CLOSE, 0, 0 );
            break;

        default:
            return FALSE;
        }
        return TRUE;
        break;

    case WM_CLOSE:
        EndDialog( hDlg, fSettingsSaved );
        return TRUE;

    case WM_DESTROY:
        while( ComboBox_GetCount( hDeviceCombo ))
        {
        lpguTemp = (LPGUID)ComboBox_GetItemData( hDeviceCombo, 0 );
        if( NULL != lpguTemp )
            delete lpguTemp;
        ComboBox_DeleteString( hDeviceCombo, 0 );
        }
        return TRUE;

    default:
        return FALSE;
    }
    ASSERT( FALSE );
    return FALSE;
    }


/* LoadRegistrySettings()
 *
 *    Load application global REGSETTINGS structure values from the registry.
 */
BOOL LoadRegistrySettings( void )
    {
    HKEY    hReg;
    DWORD   dwVal;
    DWORD   cbValSize;
    BOOL    fRet = TRUE;

    // Load current settings from our registry key
    if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_CURRENT_USER, REG_SETTINGS_KEY,
                    0, KEY_READ, &hReg ))
    {
    GetMediaPath( grs.szInitialDir, sizeof(grs.szInitialDir));
    
    grs.fDefaultDevice = TRUE;
    grs.fUseExclusiveMode = FALSE;
    grs.fOpen3D = FALSE;
    grs.dwDefaultFocusFlag = 0;
    grs.szInitialDir[0] = '\0'; 
    grs.dwPreferredFormat = aFormatOrder[0];
    fRet = TRUE;
    goto LRS_Return;
    }

    // Load the "Use DirectSound Default Device" flag
    cbValSize = sizeof(dwVal);
    if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_SETTING_DEVICE_DEFAULT,
                        NULL, NULL, (LPBYTE)&dwVal,
                        &cbValSize ))
    {
    grs.fDefaultDevice = TRUE;
    fRet = FALSE;
    }
    else
    {
    grs.fDefaultDevice = (BOOL)dwVal;

    if( !grs.fDefaultDevice )
        {
        // Load the GUID for the preferred device (only if it's not the default)
        cbValSize = sizeof(grs.guPreferredDevice);
        if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_SETTING_DEVICE_GUID,
                            NULL, NULL,
                            (LPBYTE)&grs.guPreferredDevice,
                            &cbValSize ))
        {
        // Copy GUID_NULL into the guPreferredDevice (only works in C++)
        grs.guPreferredDevice = GUID_NULL;
        fRet = FALSE;
        }
        }
    }

    cbValSize = sizeof(dwVal);
    if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_SETTING_USE_EXCLUSIVE,
                        NULL, NULL, (LPBYTE)&dwVal,
                        &cbValSize ))
    {
    grs.fUseExclusiveMode = FALSE;
    fRet = FALSE;
    }
    else
    grs.fUseExclusiveMode = (BOOL)dwVal;

    // Load the flag telling sus whether to default to 2D or 3D
    cbValSize = sizeof(dwVal);
    if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_SETTING_OPEN3D,
                        NULL, NULL, (LPBYTE)&dwVal,
                        &cbValSize ))
    {
    grs.fOpen3D = FALSE;
    fRet = FALSE;
    }
    else
    grs.fOpen3D = (BOOL)dwVal;

    // Load the coded version of the preferred output format
    cbValSize = sizeof(grs.dwPreferredFormat);
    if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_SETTING_OUTPUT_FORMAT,
                        NULL, NULL,
                        (LPBYTE)&grs.dwPreferredFormat,
                        &cbValSize ))
    {
    grs.dwPreferredFormat = aFormatOrder[0];
    fRet = FALSE;
    }

    // Load the default focus DSBCAPS flags
    cbValSize = sizeof(grs.dwDefaultFocusFlag);
    if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_SETTING_FOCUS_FLAG,
                        NULL, NULL,
                        (LPBYTE)&grs.dwDefaultFocusFlag,
                        &cbValSize ))
    {
    grs.dwDefaultFocusFlag = 0;
    fRet = FALSE;
    }

    // Load the initial directory for WAVE files
    cbValSize = sizeof(grs.szInitialDir);
    if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_SETTING_INITIAL_DIR,
                        NULL, NULL,
                        (LPBYTE)&grs.szInitialDir,
                        &cbValSize ))
    {
    GetMediaPath( grs.szInitialDir, sizeof(grs.szInitialDir));
    fRet = FALSE;
    }

    if( hReg != NULL )
    {
    RegCloseKey( hReg );
    hReg = NULL;
    }

LRS_Return:
    return fRet;
    }


/* SaveRegistrySettings()
 *
 *    Write the values in the REGSETTINGS global structure to the registry.
 */
BOOL SaveRegistrySettings( void )
    {
    HKEY    hReg;
    DWORD   dwVal, dwCreateDisposition;
    BOOL    fRet = TRUE;

    // Save current settings to our registry key
    if( ERROR_SUCCESS != RegCreateKeyEx( HKEY_CURRENT_USER, REG_SETTINGS_KEY,
                    0, NULL, 0, KEY_WRITE, NULL, &hReg,
                    &dwCreateDisposition ))
    {
    fRet = FALSE;
    goto SRS_Return;
    }

    // Save the "Use DirectSound Default Device" flag
    dwVal = (DWORD)grs.fDefaultDevice;
    if( ERROR_SUCCESS != RegSetValueEx( hReg, REG_SETTING_DEVICE_DEFAULT,
                        0, REG_DWORD, (LPBYTE)&dwVal,
                        sizeof(DWORD)))
    {
    fRet = FALSE;
    }
    else
    {
    if( !grs.fDefaultDevice )
        {
        // Save the GUID for the preferred device (only if it's not the default)
        if( ERROR_SUCCESS != RegSetValueEx( hReg, REG_SETTING_DEVICE_GUID,
                            0, REG_BINARY,
                            (LPBYTE)&grs.guPreferredDevice,
                            sizeof(GUID)))
        {
        fRet = FALSE;
        }
        }
    }

    // Use DSSCL_EXCLUSIVE ??
    dwVal = (DWORD)grs.fUseExclusiveMode;
    if( ERROR_SUCCESS != RegSetValueEx( hReg, REG_SETTING_USE_EXCLUSIVE,
                        0, REG_DWORD, (LPBYTE)&dwVal,
                        sizeof(DWORD)))
    {
    fRet = FALSE;
    }

    // Open in 3D by default ?
    dwVal = (DWORD)grs.fOpen3D;
    if( ERROR_SUCCESS != RegSetValueEx( hReg, REG_SETTING_OPEN3D,
                        0, REG_DWORD, (LPBYTE)&dwVal,
                        sizeof(DWORD)))
    {
    fRet = FALSE;
    }

    // Save the coded version of the preferred output format
    if( ERROR_SUCCESS != RegSetValueEx( hReg, REG_SETTING_OUTPUT_FORMAT,
                        0, REG_DWORD,
                        (LPBYTE)&grs.dwPreferredFormat,
                        sizeof(DWORD)))
    {
    fRet = FALSE;
    }

    // Save the coded version of the preferred output format
    if( ERROR_SUCCESS != RegSetValueEx( hReg, REG_SETTING_FOCUS_FLAG,
                        0, REG_DWORD,
                        (LPBYTE)&grs.dwDefaultFocusFlag,
                        sizeof(DWORD)))
    {
    fRet = FALSE;
    }

    // Save the initial directory for WAVE files
    if( ERROR_SUCCESS != RegSetValueEx( hReg, REG_SETTING_INITIAL_DIR,
                        0, REG_SZ,
                        (LPBYTE)&grs.szInitialDir,
                        lstrlen(grs.szInitialDir)))
    {
    fRet = FALSE;
    }

    RegCloseKey( hReg );
    hReg = NULL;

SRS_Return:
    return fRet;
    }


/* GetMediaPath()
 *
 *    In the absence of a registry value, this function is called to pick a
 * starting point for the File|Open dialog.  Usually \DXSDK\SDK\MEDIA.
 */
void GetMediaPath( LPSTR lpszBuf, int nBuf )
    {
    HKEY    hReg;
    DWORD   cbStartPathLen;

    if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                    REG_DIRECT3D_KEY,
                    0, KEY_READ, &hReg ))
    {
        goto REG_OPEN_FAILED;
    }
    else
    {
    // Query the Registry for the path to the media directory
    cbStartPathLen = sizeof( szOpenStartDir );
    if( ERROR_SUCCESS != RegQueryValueEx( hReg, REG_D3DPATH_VAL,
                        NULL, NULL,
                        (LPBYTE)szOpenStartDir,
                        &cbStartPathLen ))
        {
        goto REG_OPEN_FAILED;
        }
    RegCloseKey( hReg );
    hReg = NULL;
    }

    return;

REG_OPEN_FAILED:
    // Start off by getting the Windows directory -- we're trying to build a
    // file path like "C:\WINDOWS\MEDIA", but the WINDOWS directory could be
    // named anything, so we must ask.
    GetWindowsDirectory( szOpenStartDir, sizeof(szOpenStartDir));
    // If there's no trailing backslash, append one
    if( lstrcmp( &szOpenStartDir[lstrlen(szOpenStartDir)], TEXT("\\") ))
    lstrcat( szOpenStartDir, TEXT("\\"));
    // Now add on the MEDIA part of the path
    lstrcat( szOpenStartDir, TEXT("MEDIA"));
    }


/* FormatCodeToText()
 *
 *    This function reads format codes and puts out a text string for them.
 * It does not check for invalid codes.  FALSE return means the buffer was
 * invalid in some way, TRUE means success.
 *
 */
BOOL FormatCodeToText( DWORD dwFormat, LPSTR lpszBuf, int nBufSize )
    {
    DWORD   dwFreq;

    // The longest string we'll ever put in is 21 characters (including NULL)
    if( NULL == lpszBuf || nBufSize < 21 )
    return FALSE;

    // Extract the sample rate
    dwFreq = FC_GETFREQCODE(dwFormat);
    dwFreq = ( dwFreq == 8 ? 8000 : (dwFreq / 11) * 11025);

    wsprintf( lpszBuf, "%u Hz, %u-bit %s", dwFreq, FC_GETBITS(dwFormat),
        FC_GETCHANNELS(dwFormat) == 1 ? "Mono" : "Stereo" );

    return TRUE;
    }


/* FormatCodeToWFX()
 *
 *    This function reads format codes and fills most of the fields of a
 * WAVEFORMATEX structure based on the values read.  It does not fill the
 * wFormatTag or cbSize members.
 *
 */
BOOL FormatCodeToWFX( DWORD dwFormat, PWAVEFORMATEX pwfx )
    {
    DWORD   dwFreq;

    if( NULL == pwfx )
    return FALSE;

    // Extract the sample rate
    dwFreq = FC_GETFREQCODE(dwFormat);
    pwfx->nSamplesPerSec = ( dwFreq == 8 ? 8000 : (dwFreq / 11) * 11025);

    pwfx->wBitsPerSample = (WORD)FC_GETBITS(dwFormat);
    pwfx->nChannels = (WORD)FC_GETCHANNELS(dwFormat);

    // The nBlockAlign calculation below only works for whole-byte samples
    ASSERT( pwfx->wBitsPerSample % 8 == 0 );

    pwfx->nBlockAlign = pwfx->nChannels * (pwfx->wBitsPerSample / 8);
    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

    return TRUE;
    }


/* FormatCodeFromCommandID()
 *
 *    Returns the Format Code that matches the given command ID.
 */
DWORD FormatCodeFromCommandID( WORD wID )
    {
    int i;

    for( i = 0; i < NUM_FORMATENTRIES; i++ )
    {
    if( fdFormats[i].wCommandID == wID )
        return fdFormats[i].dwCode;
    }
    return 0;
    }


/* CommandIDFromFormatCode()
 *
 *    Searchs our FORMATDATA array and returns the Command ID corresponding
 * to the given format code, or 0 if there is no such valid format code.
 */
WORD CommandIDFromFormatCode( DWORD dwCode )
    {
    int i;

    for( i = 0; i < NUM_FORMATENTRIES; i++ )
    {
    if( fdFormats[i].dwCode == dwCode )
        return fdFormats[i].wCommandID;
    }
    return 0;
    }


void DisableFormatCode( DWORD dwCode )
    {
    int i;

    for( i = 0; i < NUM_FORMATENTRIES; i++ )
    {
    if( fdFormats[i].dwCode == dwCode )
        {
        fdFormats[i].fEnable = FALSE;
        break;
        }
    }
    }


void EnableFormatCode( DWORD dwCode )
    {
    int i;

    for( i = 0; i < NUM_FORMATENTRIES; i++ )
    {
    if( fdFormats[i].dwCode == dwCode )
        {
        fdFormats[i].fEnable = TRUE;
        break;
        }
    }
    }


/* DSEnumProc()
 *
 *   DirectSoundEnumerate() callback procedure which fills a combo box with
 * the description strings of all devices and attachs a pointer to a GUID,
 * which must be freed later by calling delete.
 */
BOOL CALLBACK DSEnumProc( LPGUID lpguDevice, LPSTR lpszDesc,
                                LPSTR lpszDrvName, LPVOID lpContext )
    {
    HWND   hCombo = *(HWND *)lpContext;
    LPGUID lpguTemp = NULL;
    int     idx;

    if( NULL != lpguDevice )
        {
        lpguTemp = new GUID;
    // We failed to allocate storage, so continue with next device
    if( NULL == lpguTemp )
        return( TRUE );

        CopyMemory( lpguTemp, lpguDevice, sizeof(GUID));
    }

    idx = ComboBox_AddString( hCombo, lpszDesc );
    ComboBox_SetItemData( hCombo,
                ComboBox_FindString( hCombo, 0, lpszDesc ),
                lpguTemp );

    if( !grs.fDefaultDevice )
    {
    if( NULL == lpguTemp )
        {
        if( grs.guPreferredDevice == GUID_NULL )
        ComboBox_SetCurSel( hCombo, idx );
        }
    else if( *lpguTemp == grs.guPreferredDevice )
        ComboBox_SetCurSel( hCombo, idx );
    }
    
    return( TRUE );
    }


/* fGetToken()
 *
 *    Parses the command-line string "in place" starting at pszStart.  A ptr
 * to the start of the next token and it's length will be the out parameters,
 * or NULL and 0 if no token.  Note that *ppszRet will NOT be NULL-terminated
 * since the string is part of another string.  That's what then length is for.
 *
 * Returns: TRUE if a token was retrieved, or FALSE if there was no token.
 */
BOOL fGetToken( PSTR pszStart, PSTR *ppszRet, int *pcchRet )
    {
    PSTR  pszCur = pszStart;
    PSTR  pszTokStart;

    if( !pszStart || NULL == ppszRet || NULL == pcchRet )
    return FALSE;

    // Skip leading whitespace
    while( *pszCur && (*pszCur == ' ' || *pszCur == '\t'))
    pszCur++;

    *ppszRet = NULL;
    *pcchRet = 0;

    if( *pszCur )
    {
    pszTokStart = pszCur;

    while( *pszCur && *pszCur != ' ' && *pszCur != '\t' )
        pszCur++;

    *ppszRet = pszTokStart;
    *pcchRet = (int)(pszCur - pszTokStart);
    }

    if( *pcchRet != 0 )
    return TRUE;
    else
    return FALSE;
    }


/* fMatchToken()
 *
 *    Attempts to match the first cchLen characters of pszDatum to the
 * string at pszString.  The comparison is case-insensitive (this function
 * is designed for command-line switch matching).
 *
 * Returns: TRUE if the first cchLen characters are a match, else FALSE.
 */
BOOL fMatchToken( PSTR pszString, PSTR pszDatum, int cchLen )
    {
    int i;

    for( i = 0; i < cchLen; i++ )
    {
    if( CharLower( (LPTSTR)MAKELONG( pszString[i], 0 ))
            != CharLower( (LPTSTR)MAKELONG( pszDatum[i], 0 )))
        return FALSE;
    }
    return TRUE;
    }


/* ParseCommandLine()
 *
 *    Given a command-line string without the module name, this function will
 * parse the command line and takes action on whatever it finds there.
 *
 * Returns: TRUE if successful, or FALSE if there was an error.
 */
BOOL ParseCommandLine(LPSTR lpszCmdLine)
    {
    PSTR    pszCur,pszToken;
    PSTR    ppszFiles[MAXCONTROLS];
    BOOL    fStartPlaying = FALSE, fStartLooping = FALSE;
    int     cchTokLen = 0, i, nFilesFound;

    pszCur = lpszCmdLine;

    // First get all the command line switches
    while( fGetToken(pszCur, &pszToken, &cchTokLen) &&
       (pszToken[0] == '/' || pszToken[0] == '-' ))
    {
    pszCur = pszToken + cchTokLen;
    pszToken++;

    if( fMatchToken( pszToken, "PLAY", 4 ))
        {
        fStartPlaying = TRUE;
        }
    else if( fMatchToken( pszToken, "LOOP", 4 ))
        {
        fStartLooping = TRUE;
        }
    else
        {
        // We don't recognize this mysterious switch, so eat it and move on
        }
    }

    // Anything left on the command-line will be treated as a filename and
    // we'll attempt to open it after we've found them all
    nFilesFound = 0;
    while( fGetToken(pszCur, &pszToken, &cchTokLen) && nFilesFound < MAXCONTROLS )
    {
    pszCur = pszToken + cchTokLen;
    ppszFiles[nFilesFound] = new char[cchTokLen+1];
    // Copy the token out of the command-line string and into our buffer
    CopyMemory( ppszFiles[nFilesFound], pszToken, cchTokLen*sizeof(char));
    // Append a NULL terminator to what we just copied (to be safe)
    *(ppszFiles[nFilesFound] + cchTokLen) = 0;
    nFilesFound++;
    }
    // This function will take the array of strings we've created and open
    // each string as a file.  It will obey the global fStartPlaying and
    // fStartLooping flags we may have already set above
    if( nFilesFound )
    AppWnd.BatchOpenFiles( ppszFiles, nFilesFound, fStartPlaying, fStartLooping );

    // Free the space we allocated
    for( i = 0; i < nFilesFound; i++ )
    {
    delete[] ppszFiles[i];
    ppszFiles[i] = NULL;
    }

    // Returning TRUE means the caller should continue doing what they
    // were doing: we succeeded.
    return TRUE;
    }


